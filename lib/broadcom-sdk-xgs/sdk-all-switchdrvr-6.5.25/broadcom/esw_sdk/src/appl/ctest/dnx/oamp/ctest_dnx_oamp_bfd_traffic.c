/** \file ctest_dnx_oamp_traffic.c
 *
 * Tests for OAMP
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/** sal */
#include <sal/appl/sal.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/dnx/diag_dnx_cmdlist.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <src/appl/ctest/dnx/oamp/ctest_dnx_oamp.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm/bfd.h>
#include <bcm/l3.h>
#include <bcm/l2.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#ifdef BCM_DNX2_SUPPORT
/* { */
#include <include/bcm_int/dnx/algo/oamp/algo_oamp_v1.h>
/* } */
#endif
/** Event type interrupts */
#define EVENT_TYPE_TIMEOUT 0
#define EVENT_TYPE_TIMEIN 1
#define EVENT_TYPE_STATE 2
#define EVENT_TYPE_ALL 3

#define OAMP_MEP_DB_THRESHOLD_STR_SIZE 10

/**
* Expected time creation per MEP in micro-seconds.
* Could be increased in the time, since new feature will be added.
* Time changes in micro-seconds:
*       Update when increase the time:
*         Date         |        value
*       07.03.2018              12000
*       05.12.2018              3000 | device
*/
#define BFD_EXPECTED_CREATE_TIME_PER_MEP 5000
/**
* Since the first pair will also set the needed DBs
* the creation time will took longer.
* If a change in the actions or profiles are made,
* probably this time will need to be increased.
* Time changes in micro-seconds:
*       Update when increase the time:
*         Date         |        value
*       07.03.2018              1900000
*       05.12.2018              15000 | device
*       31.01.2019              50000 | device
*/
#define BFD_EXPECTED_CREATE_TIME_FIRST_PAIR 50000
#define TEST_BFD_VLAN                       10  /* VID = 10 */
#define TEST_BFD_DETEC_MULT_3               3   /* Detect Multiplier = 3 */
#define TEST_BFD_DETECTION_TIME_10MS        12000       /* Detection time = 10ms */
#define TEST_BFD_MAX_NUM_PERIODS            7   /* Maximum number of allowed periods */
#define TEST_BFD_NUM_DEF_PERIODS            6   /* Number of predefined values for EP periods */
#define TEST_BFD_MAX_NUM_INTERVALS          8   /* Maximum number of values for Required Min RX and Desire Min TX
                                                 * Interval */
#define TEST_BFD_NUM_IP                     16  /* Maximum number of IPs */

/**
 * Global Variables
 */
/** Number of bcmBFDEventEndpointTimeout occurred */
static int test_bfd_timeout_event_counts = 0;
/** Number of bcmBFDEventEndpointTimein occurred */
static int test_bfd_timein_event_counts = 0;
/** Number of bcmBFDEventStateChange occurred */
static int test_bfd_state_event_counts = 0;
int nof_called = 0;
/** Random predefined values for BFD transmission rate for local endpoints */
int test_bfd_period_ctest[TEST_BFD_MAX_NUM_PERIODS];
/** Predefined values for BFD transmission rate for local endpoints */
int test_bfd_def_period_ctest[TEST_BFD_NUM_DEF_PERIODS];
/** IPv4 addresses */
int test_bfd_IP_address_ctest[TEST_BFD_NUM_IP];
/** IPv6 addresses */
bcm_ip6_t test_bfd_IPv6_address[TEST_BFD_NUM_IP];
/** Values for Desired Min TX and Required Min RX Intervals */
int test_bfd_intervals_ctest[TEST_BFD_MAX_NUM_INTERVALS];
/** Number of times that state will be changed */
int test_bfd_number_states = 0;
/** Determine traffic or semantic run */
int sematic_scale_validation;
diag_dnx_oamp_test_bfd_lock_t test_bfd_info_lock_ctest;

/**
 * Structure to keep global values of parameters
*/
typedef struct diag_dnx_oamp_bfd_test_general_s
{
    int num_entries;
    int is_state_update;
    int is_ipv6;
    int is_update_test;
    bcm_port_t port_1;
    bcm_port_t port_2;
    bcm_if_t local_encap_id;
    bcm_if_t remote_encap_id;
    int threshold;

} diag_dnx_oamp_bfd_test_general_t;

/**
 * Structure to keep values of IP addresses
*/
typedef struct diag_dnx_oamp_bfd_test_ip_addr_s
{
    int ipv4_addr;
    bcm_ip6_t ipv6_addr;
} diag_dnx_oamp_bfd_test_ip_addr_t;

/**
 * Structure to values for keep L3 egress
*/
typedef struct diag_dnx_oamp_test_bfd_create_l3_egress_s
{
    /**
     * Input 
     */
    uint32 allocation_flags;    /* BCM_L3_XXX */
    uint32 l3_flags;            /* BCM_L3_XXX */
    uint32 l3_flags2;           /* BCM_L3_FLAGS2_XXX */

    /**
     * ARP 
     */
    int vlan;                   /* Outgoing vlan-VSI, relevant for ARP creation. In case set then SA MAC address is
                                 * retrieved from this VSI. */
    bcm_mac_t next_hop_mac_addr;        /* Next-hop MAC address, relevant for ARP creation */
    int qos_map_id;             /* General QOS map id */

    /**
     * FEC 
     */
    bcm_if_t out_tunnel_or_rif; /* *Outgoing intf, can be tunnel/rif, relevant for FEC creation */
    bcm_gport_t out_gport;      /* *Outgoing port , relevant for FEC creation */
    bcm_failover_t failover_id; /* Fail over Object Index. */
    bcm_if_t failover_if_id;    /* Fail over Egress Object index. */

    /**
     * Input/Output ID allocation 
     */
    bcm_if_t fec_id;            /* *FEC ID */
    bcm_if_t arp_encap_id;      /* *ARP ID, may need for allocation ID or for FEC creation */
} diag_dnx_oamp_test_bfd_create_l3_egress_t;

/**
 * Structure to keep values for L3 interface
*/
typedef struct diag_dnx_oamp_test_bfd_create_l3_intf_s
{
    /**     
     * Input 
    */
    uint32 flags;               /* BCM_L3_XXX */
    uint32 ingress_flags;       /* BCM_L3_INGRESS_XXX */
    int no_publc;               /* Used to force no public, public is forced if vrf = 0 or scale feature is turned on */
    int vsi;
    bcm_mac_t my_global_mac;
    bcm_mac_t my_lsb_mac;
    int vrf_valid;              /* Do we need to set vrf */
    int vrf;
    int rpf_valid;              /* Do we need to set rpf */
    bcm_l3_ingress_urpf_mode_t urpf_mode;       /* avail. when BCM_L3_RPF is set */
    int mtu_valid;
    int mtu;
    int mtu_forwarding;
    int qos_map_valid;
    int qos_map_id;
    int ttl_valid;
    int ttl;
    uint8 native_routing_vlan_tags;

    /**
     * Output
     */
    int rif;
    uint8 skip_mymac;           /* If true, mymac will not be set. Make sure you set it elsewhere. */
} diag_dnx_oamp_test_bfd_create_l3_intf_t;

/**
 * Custom structure which hold all needed information about EP.
*/
typedef struct diag_dnx_oamp_bfd_test_ep_create_info_s
{
    int id;
    int vlan;
    int mac;
    int acc_profile_id;
    int memory_type;
    int name;
    int level;
    int counter_base_id;
    uint8 is_rdi;
    bcm_gport_t gport;
    bcm_port_t port;

} diag_dnx_oamp_bfd_test_ep_create_info_t;

/**
 * \brief - Initialization of global variables.
 *
 * \param
 *   * none
 * \return
 *   * none
 * \remark
 *   * None
 * \see
 *   * None
 */
void
diag_dnx_oamp_bfd_test_bfd_set_global_values(
    void)
{
    int i;
    bcm_ip6_t ipv6_1_MEP =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_2_MEP =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_3_MEP =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_4_MEP =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_5_MEP =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_6_MEP =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_7_MEP =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t ipv6_8_MEP =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };

    for (i = 0; i < TEST_BFD_MAX_NUM_PERIODS; i++)
    {
        test_bfd_period_ctest[i] = sal_rand() % 996 + 4;        /* random value between 4 - 1000 */
    }

    for (i = 0; i < TEST_BFD_MAX_NUM_INTERVALS; i++)
    {
        test_bfd_intervals_ctest[i] = sal_rand() % 1000000;     /* random value between 0 - 1,000,000 */
    }

    /**
     * Predefined periods.
    */
    test_bfd_def_period_ctest[0] = 4;
    test_bfd_def_period_ctest[1] = 10;
    test_bfd_def_period_ctest[2] = 20;
    test_bfd_def_period_ctest[3] = 50;
    test_bfd_def_period_ctest[4] = 100;
    test_bfd_def_period_ctest[5] = 1000;

    /**
     * IPv4 addresses for MEPs 
     */
    test_bfd_IP_address_ctest[0] = 0x01010164;
    test_bfd_IP_address_ctest[1] = 0x02010164;
    test_bfd_IP_address_ctest[2] = 0x03010164;
    test_bfd_IP_address_ctest[3] = 0x04010164;
    test_bfd_IP_address_ctest[4] = 0x05010164;
    test_bfd_IP_address_ctest[5] = 0x06010164;
    test_bfd_IP_address_ctest[6] = 0x07010164;
    test_bfd_IP_address_ctest[7] = 0x08010164;

    /**
     * IPv6 addresses for MEPs 
     */
    sal_memcpy(test_bfd_IPv6_address[0], ipv6_1_MEP, 16);
    sal_memcpy(test_bfd_IPv6_address[1], ipv6_2_MEP, 16);
    sal_memcpy(test_bfd_IPv6_address[2], ipv6_3_MEP, 16);
    sal_memcpy(test_bfd_IPv6_address[3], ipv6_4_MEP, 16);
    sal_memcpy(test_bfd_IPv6_address[4], ipv6_5_MEP, 16);
    sal_memcpy(test_bfd_IPv6_address[5], ipv6_6_MEP, 16);
    sal_memcpy(test_bfd_IPv6_address[6], ipv6_7_MEP, 16);
    sal_memcpy(test_bfd_IPv6_address[7], ipv6_8_MEP, 16);
}

/**
 * \brief - Calculate current and maximum time.
 *
 * \param [in/out] time_param - Used to store time variables.
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void
diag_dnx_oamp_test_bfd_time_calculation(
    diag_dnx_oamp_traffic_test_time_calc_t * time_param)
{
    /**
    * The first pair of MEPs were created.
    * The needed time is more than the rest,
    * because the DB and profiles must be configured for,
    * the first time.
    */
    if (time_param->interaction == 0)
    {
        time_param->expected_max_create_time += BFD_EXPECTED_CREATE_TIME_FIRST_PAIR;
    }
    time_param->actual_create_time += (time_param->end_time - time_param->start_time);
    time_param->interaction += 2;
    time_param->expected_max_create_time += (BFD_EXPECTED_CREATE_TIME_PER_MEP * 2);
}

/**
 * \brief -Create mutex.
 *
 * \param
 *   unit
 * \return
 *   * none
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_lock_init(
    int unit)
{
    diag_dnx_oamp_test_bfd_lock_t *temp_state = &test_bfd_info_lock_ctest;
    SHR_FUNC_INIT_VARS(unit);

    if (temp_state->is_init == 0)
    {
        sal_memset(temp_state, 0, sizeof(diag_dnx_oamp_test_bfd_lock_t));
        temp_state->dnx_bfd_lock[unit] = sal_mutex_create("test_bfd_info_lock_ctest");
        if (!temp_state->dnx_bfd_lock[unit])
        {
            SHR_CLI_EXIT(_SHR_E_RESOURCE, "Test failed! Failed to create mutex. Aborting test.\n");
        }
        temp_state->is_init = 1;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - take lock of state update.
 *
 * \param
 *   unit
 * \return
 *   * none
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
diag_dnx_oamp_test_bfd_lock_take(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (0 != sal_mutex_take(test_bfd_info_lock_ctest.dnx_bfd_lock[unit], sal_mutex_FOREVER))
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "unable to take unit %d counter dma fifo mutex\n", unit);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -give lock of state update.
 *
 * \param
 *   unit
 * \return
 *   * none
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
diag_dnx_oamp_test_bfd_lock_give(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (0 != sal_mutex_give(test_bfd_info_lock_ctest.dnx_bfd_lock[unit]))
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "unable to give unit %d counter dma fifo mutex\n", unit);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add Host <vrf, addr> --> intf.
 *
 * \param [in] unit     - Number of hardware unit used.
 * \param [in] addr     - IP address 32 bit value.
 * \param [in] vrf      - VRF value.
 * \param [in] intf     - egress object created using create_l3_egress.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_add_host(
    int unit,
    uint32 addr,
    int vrf,
    int intf)
{

    bcm_l3_host_t l3host;
    SHR_FUNC_INIT_VARS(unit);
    bcm_l3_host_t_init(&l3host);

    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = addr;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = intf;
    l3host.l3a_port_tgid = 0;

    SHR_CLI_EXIT_IF_ERR(bcm_l3_host_add(unit, &l3host), "bcm_l3_host_add failed:\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add Host <vrf, addr> --> intf.
 *
 * \param [in] unit     - Number of hardware unit used.
 * \param [in] addr     - IP address 32 bit value.
 * \param [in] vrf      - VRF value.
 * \param [in] intf     - egress object created using create_l3_egress.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_add_ipv6_host(
    int unit,
    bcm_ip6_t addr,
    int vrf,
    int intf)
{

    bcm_l3_host_t l3host;
    SHR_FUNC_INIT_VARS(unit);
    bcm_l3_host_t_init(&l3host);

    sal_memcpy(l3host.l3a_ip6_addr, addr, 16);
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = intf;     /* FEC-ID */
    l3host.l3a_flags = BCM_L3_IP6;

    SHR_CLI_EXIT_IF_ERR(bcm_l3_host_add(unit, &l3host), "bcm_l3_host_add failed:\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set a FEC entry, without allocating ARP entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] l3_egress - l3 egress object information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_l3_egress_only_fec_create(
    int unit,
    diag_dnx_oamp_test_bfd_create_l3_egress_t * l3_egress)
{
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_egress_t_init(&l3eg);
    /*
     * FEC properties 
     */
    /*
     * In previous devices, in case single Global Out-LIF pointer in FEC entry was required, we could placed it in
     * either intf or encap_id fields. In JR2, it is possible to place it in intf field only. encap_id can not be used
     * when only one single Out-LIF is required. 
     */
    l3eg.intf = l3_egress->out_tunnel_or_rif ? l3_egress->out_tunnel_or_rif : l3_egress->arp_encap_id;
    l3eg.encap_id = l3_egress->out_tunnel_or_rif ? l3_egress->arp_encap_id : 0;

    l3eg.port = l3_egress->out_gport;
    l3eg.vlan = l3_egress->vlan;
    /*
     * FEC properties - protection 
     */
    l3eg.failover_id = l3_egress->failover_id;
    l3eg.failover_if_id = l3_egress->failover_if_id;

    l3eg.flags = l3_egress->l3_flags;
    l3eg.flags2 = l3_egress->l3_flags2;
    l3egid = l3_egress->fec_id;

    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | l3_egress->allocation_flags, &l3eg, &l3egid),
                        "Error, create egress object, unit=%d, \n", unit);

    l3_egress->fec_id = l3egid;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set an ARP entry, without allocating FEC entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] l3_egress - l3 egress object information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_l3_egress_only_encap_create(
    int unit,
    diag_dnx_oamp_test_bfd_create_l3_egress_t * l3_egress)
{

    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_egress_t_init(&l3eg);

    sal_memcpy(l3eg.mac_addr, l3_egress->next_hop_mac_addr, 6);
    l3eg.vlan = l3_egress->vlan;
    l3eg.encap_id = l3_egress->arp_encap_id;
    if (BCM_L3_ITF_TYPE_IS_LIF(l3_egress->out_tunnel_or_rif))
    {
        l3eg.intf = l3_egress->out_tunnel_or_rif;
    }

    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create
                        (unit, BCM_L3_EGRESS_ONLY | l3_egress->allocation_flags, &l3eg, &l3egid_null),
                        "Error, create egress object, \n");

    l3_egress->arp_encap_id = l3eg.encap_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Open new vsi and attach it to the give mc_group.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] vlan - The vlan to open.
 * \param [in] mc_group - The mc_group to be assigned.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_vlan_open_vlan_per_mc(
    int unit,
    int vlan,
    bcm_multicast_t mc_group)
{
    bcm_vlan_control_vlan_t control_vlan;

    SHR_FUNC_INIT_VARS(unit);
    bcm_vlan_control_vlan_t_init(&control_vlan);

    /*
     * The vlan in this case may also represent a vsi. in that case, it should be created with a different api 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_create(unit, vlan), "fail open vlan(%d)", vlan);

    /*
     * Set VLAN with MC 
     */
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_control_vlan_get(unit, vlan, &control_vlan), "fail get control vlan(%d)\n", vlan);

    control_vlan.unknown_unicast_group = mc_group;
    control_vlan.unknown_multicast_group = mc_group;
    control_vlan.broadcast_group = mc_group;

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_control_vlan_set(unit, vlan, control_vlan), "fail set control vlan(%d)\n", vlan);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Creates Router interface.
 *          - packets sent in from this interface identified by <port,
 *              vlan> with specified MAC address is subject of routing
 *          - packets sent out through this interface will be encapsulated with <vlan, mac_addr>
 *
 * \param [in] unit      - Number of hardware unit used.
 * \param [out] l3_intf  - l3 interface object information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_l3_intf_rif_create(
    int unit,
    diag_dnx_oamp_test_bfd_create_l3_intf_t * l3_intf)
{
    bcm_l3_intf_t l3if, l3if_old;
    bcm_l2_station_t station;
    bcm_l3_ingress_t l3_ing_if;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize a bcm_l3_intf_t structure. 
     */
    bcm_l3_intf_t_init(&l3if);
    bcm_l3_intf_t_init(&l3if_old);
    bcm_l2_station_t_init(&station);
    bcm_l3_ingress_t_init(&l3_ing_if);

    l3if.l3a_flags = BCM_L3_WITH_ID;    /* WITH-ID or without-ID does not really matter. Anyway for now RIF equal VSI */

    l3if.l3a_vid = l3_intf->vsi;
    l3if.l3a_ttl = 31;  /* default settings */
    l3if.l3a_mtu = 1524;        /* default settings */
    l3if.l3a_mtu = l3_intf->mtu;
    l3if.l3a_mtu_forwarding = l3_intf->mtu_forwarding;

    l3_intf->rif = l3if.l3a_intf_id = l3_intf->vsi;     /* In DNX Arch VSI always equal RIF */

    sal_memcpy(l3if.l3a_mac_addr, l3_intf->my_lsb_mac, 6);
    sal_memcpy(l3if.l3a_mac_addr, l3_intf->my_global_mac, 4);   /* overwriting 4 MSB bytes with global MAC
                                                                 * configuration */

    l3if_old.l3a_intf_id = l3_intf->vsi;

    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Error, bcm_l3_intf_create\n");

    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = l3_intf->vrf;

    l3_ing_if.urpf_mode = bcmL3IngressUrpfDisable;

    SHR_CLI_EXIT_IF_ERR(bcm_l3_ingress_create(unit, &l3_ing_if, &l3if.l3a_intf_id), "Error, bcm_l3_ingress_create\n");

    l3_intf->rif = l3if.l3a_intf_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Creating l3 interface.
 *
 * \param [in] unit                     - Number of hardware unit used.
 * \param [in] in_sysport               - In port.
 * \param [in] out_sysport              - Out port.
 * \param [in] is_ipv6                  - Provide information for L3 interface.
 * \param [out] _next_hop_mac_encap_id   - Encap ID.
 * \param [out] _tunnel_id               - Tunnel ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_l3_interface_init(
    int unit,
    int in_sysport,
    int out_sysport,
    int is_ipv6,
    int *_next_hop_mac_encap_id,
    int *_tunnel_id)
{

    int i_ip;
    int ing_intf_out;
    int fec[2] = { 0x0, 0x0 };
    int in_vlan = 1;
    int out_vlan = 10;
    int vrf = 0;
    int encap_id[2] = { 0 };
    int route;
    int mask;
    int l3_eg_int;
    bcm_mac_t mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t next_hop_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };    /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };   /* next_hop_mac2 */
    bcm_ip6_t ip_route_basic_ipv6_route =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10 };
    bcm_ip6_t ip_route_basic_ipv6_mask =
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };

    bcm_l3_route_t bfd_route;
    diag_dnx_oamp_test_bfd_create_l3_intf_t intf = { 0 };
    diag_dnx_oamp_test_bfd_create_l3_egress_t l3eg = { 0 };
    diag_dnx_oamp_test_bfd_create_l3_egress_t l3eg1 = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_route_t_init(&bfd_route);
    /*** create ingress router interface ***/
    in_vlan = 10;
    vrf = 1;

    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_vlan_open_vlan_per_mc(unit, in_vlan, 0x1),
                        "{1} Error, open_vlan=%d, in unit %d \n", in_vlan, unit);

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0),
                        "{2} fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);

    intf.vsi = in_vlan;
    sal_memcpy(intf.my_lsb_mac, mac_address, 6);
    sal_memcpy(intf.my_global_mac, mac_address, 6);     /* overwriting 4 MSB bytes with global MAC configuration */

    intf.vrf_valid = 1;
    intf.vrf = vrf;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;

    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_l3_intf_rif_create(unit, &intf), "{3} Error, l3__intf_rif__create1\n");

    /*** create egress router interface ***/
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0),
                        "{5} fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);

    intf.vsi = out_vlan;
    ing_intf_out = intf.rif;
    l3_eg_int = ing_intf_out;
    /*** Create egress object 1 ***/
    l3eg.out_tunnel_or_rif = l3_eg_int;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac, 6);
    l3eg.vlan = out_vlan;
    l3eg.arp_encap_id = encap_id[0];
    l3eg.fec_id = fec[0];
    l3eg.allocation_flags = 0;
    l3eg.out_gport = out_sysport;

    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_l3_egress_only_encap_create(unit, &l3eg),
                        "{7} Error, in l3__egress_only_encap__create with vlan %d continue \n", l3eg.vlan);

    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_l3_egress_only_fec_create(unit, &l3eg),
                        "{8} Error, in l3__egress_only_fec__create with vlan %d continue \n", l3eg.vlan);

    encap_id[0] = l3eg.arp_encap_id;
    fec[0] = l3eg.fec_id;

    *_tunnel_id = encap_id[0];

    /*** add host ***/
    for (i_ip = 0; i_ip < 8; i_ip++)
    {
        if (is_ipv6)
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_add_ipv6_host(unit, test_bfd_IPv6_address[i_ip], vrf, fec[0]),
                                "{9}Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
        else
        {
            SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_add_host(unit, test_bfd_IP_address_ctest[i_ip], vrf, fec[0]),
                                "{9}Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
    }

    /*** create egress object 2***/
    /*
     * We're allocating a lif. out_sysport unit should be first, and it's already first 
     */
    l3eg1.out_tunnel_or_rif = l3_eg_int;
    sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2, 6);
    l3eg1.vlan = out_vlan;
    l3eg1.arp_encap_id = encap_id[1];
    l3eg1.fec_id = fec[1];
    l3eg1.allocation_flags = 0;
    l3eg1.out_gport = out_sysport;

    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_l3_egress_only_encap_create(unit, &l3eg1),
                        "{10}Error, in bcm_vlan_create with vlan %d continue \n", l3eg.vlan);

    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_l3_egress_only_fec_create(unit, &l3eg1),
                        "{11}Error, in bcm_vlan_create with vlan %d continue \n", l3eg.vlan);

    encap_id[1] = l3eg1.arp_encap_id;
    fec[1] = l3eg1.fec_id;
    *_next_hop_mac_encap_id = encap_id[1];

    /*** add route point to FEC2 ***/
    if (is_ipv6)
    {
        sal_memcpy(bfd_route.l3a_ip6_net, ip_route_basic_ipv6_route, 16);
        sal_memcpy(bfd_route.l3a_ip6_mask, ip_route_basic_ipv6_mask, 16);
        bfd_route.l3a_flags = BCM_L3_IP6;
    }
    else
    {
        route = 0x7fffff00;
        mask = 0xfffffff0;
        bfd_route.l3a_subnet = route;
        bfd_route.l3a_ip_mask = mask;
    }
    bfd_route.l3a_vrf = vrf;
    bfd_route.l3a_intf = fec[1];
    SHR_CLI_EXIT_IF_ERR(bcm_l3_route_add(unit, &bfd_route),
                        "{13}Error, create egress object, in_sysport=%d in unit %d, \n", in_sysport, unit);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculate additional data index.
 *
 * \param [in] local_discr - MEP's local discriminator.
 *
 * \return
 *   extra_data_indx
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
diag_dnx_oamp_test_bfd_extra_data_index_calculate(
    int local_discr,
    int threshold)
{
    int extra_data_indx = 0;

    /** 
    * If Local DISC is lower then threshold we are in short-entry format.
    * In short-entry format use default value for extra_data_indx 
    */
    if (threshold > local_discr)
    {
        extra_data_indx = 0;
        /** No extra effort is needed */
        return extra_data_indx;
    }
    /** Check how many banks are allocated for short entries */
    /** Calculate the extra_data_index */
    switch (threshold)
    {
        case 0:
               /** Only self-contained */
            /** Up to 3 Banks can be used for MEPs */
            extra_data_indx = (((local_discr / 8192) + 1) *  /** calculate bank index */
                               (3 * 8192)) +                 /** calculate how far will be first extra data */
                (local_discr % 8192);                        /** calculate MEP index in the bank */
            break;
        case 8192:
                  /** Bank 0 */
        case 16384:
                   /** Bank 0 - 1 */
            /** Up to 2 Banks can be used for MEPs */
            extra_data_indx = (((local_discr / 8192) + 1) *  /** calculate bank index */
                               (3 * 8192)) +                 /** calculate how far will be first extra data */
                (local_discr % 8192);                       /** calculate MEP index in the bank */
            break;
        case 24576:
                   /** Bank 0 - 2 */
            /** 
            * Up to 2 Banks can be used for MEPs
            * For maximum performance we will use bank 3,4 for MEPs
            * MEPs in bank[3] will use Bank[6,7,8] for extra data.
            * MEPs in bank[4] will use Bank[9,10,11] for extra data.
            */
            extra_data_indx = (((local_discr / 8192) - 3) * 24576) + (local_discr % 8192) + 49152;
        case 32768:
                   /** Bank 0 - 3 */
            /** 
            * Up to 2 Banks can be used for MEPs
            * For maximum performance we will use bank 4,5 for MEPs
            * MEPs in bank[4] will use Bank[6,7,8] for extra data.
            * MEPs in bank[5] will use Bank[9,10,11] for extra data.
            */
            extra_data_indx = (((local_discr / 8192) - 4) * 24576) + (local_discr % 8192) + 49152;
            break;
        case 40960:
                   /** Bank 0 - 4 */
        case 49152:
                   /** Bank 0 - 5 */
        case 57344:
                   /** Bank 0 - 6 */
            /** Only one bank can be used for MEPs, so we will use first additional bank for extra data */
            extra_data_indx = 65536 + (local_discr % 8192);
            break;
        case 65536:
                   /** Only short entries */
            extra_data_indx = 0;
            break;
        default:
            /** Something went wrong */
            extra_data_indx = -1;
            break;
    }

    return extra_data_indx;
}

/**
 * \brief - Create BFD Endpoint.
 *
 * \param [in] unit         - Number of hardware unit used.
 * \param [in] local_port   - port on which the MEP resides.
 * \param [in] encap_id     - Egress interface.
 * \param [in] is_ipv6      - Indicate for IPv6 BFD MEP.
 * \param [in] src_ip       - Source IP address of the MEP.
 * \param [in] remote_ip    - Destination IP address of the MEP.
 * \param [in] local_discr  - Local discriminator of the MEP.
 * \param [in] remote_discr - Remote discriminator of the MEP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_createEndpoint(
    int unit,
    int local_port,
    int encap_id,
    diag_dnx_oamp_bfd_test_general_t global_parameters,
    diag_dnx_oamp_bfd_test_ip_addr_t src_ip,
    diag_dnx_oamp_bfd_test_ip_addr_t remote_ip,
    int local_discr,
    int remote_discr)
{

    bcm_bfd_endpoint_info_t bfd_endpoint_info;
    int extra_data_indx = 0;
    int rx_i, tx_i;
    SHR_FUNC_INIT_VARS(unit);

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);

    bfd_endpoint_info.type = bcmBFDTunnelTypeUdp;
    bfd_endpoint_info.flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP;

    if (global_parameters.is_ipv6)
    {
        sal_memcpy(&(bfd_endpoint_info.src_ip6_addr), &src_ip.ipv6_addr, 16);
        sal_memcpy(&(bfd_endpoint_info.dst_ip6_addr), &remote_ip.ipv6_addr, 16);
        /**Calculate extra data index for ipv6*/
        extra_data_indx = diag_dnx_oamp_test_bfd_extra_data_index_calculate(local_discr, global_parameters.threshold);

        if (extra_data_indx == -1)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected threshold value, %d\n", global_parameters.threshold);
        }
        if (global_parameters.threshold <= local_discr)
        {
            /** In case of 1/4 MEPs the extra data should be 0 */
            bfd_endpoint_info.ipv6_extra_data_index = extra_data_indx;
        }
        bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_IPV6;
    }
    else
    {
        bfd_endpoint_info.src_ip_addr = src_ip.ipv4_addr;
        bfd_endpoint_info.dst_ip_addr = remote_ip.ipv4_addr;
    }
    tx_i = sal_rand() % 8;
    rx_i = sal_rand() % 8;

    BCM_GPORT_SYSTEM_PORT_ID_SET(bfd_endpoint_info.tx_gport, local_port);

    bfd_endpoint_info.loc_clear_threshold = 1;
    bfd_endpoint_info.bfd_period = 4;
    if (sematic_scale_validation == TEST_TYPE_SEMANTIC)
    {
        bfd_endpoint_info.bfd_period = 0;
    }
    bfd_endpoint_info.ip_ttl = 255;
    bfd_endpoint_info.ip_tos = 255;
    bfd_endpoint_info.udp_src_port = 0xC001;
    bfd_endpoint_info.int_pri = 1;
    bfd_endpoint_info.egress_if = encap_id;

    bfd_endpoint_info.local_discr = local_discr;
    bfd_endpoint_info.remote_discr = remote_discr;

    bfd_endpoint_info.local_min_tx = test_bfd_intervals_ctest[tx_i];
    bfd_endpoint_info.local_min_rx = test_bfd_intervals_ctest[rx_i];
    bfd_endpoint_info.local_detect_mult = 30;
    bfd_endpoint_info.remote_detect_mult = 30;
    bfd_endpoint_info.loc_clear_threshold = 1;
    bfd_endpoint_info.local_flags = 0;
    bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME;
    bfd_endpoint_info.bfd_detection_time = TEST_BFD_DETECTION_TIME_10MS;
    bfd_endpoint_info.remote_state = 2;
    bfd_endpoint_info.local_state = 2;
    bfd_endpoint_info.local_diag = 2;
    bfd_endpoint_info.remote_diag = 2;

    bfd_endpoint_info.remote_gport = BCM_GPORT_INVALID;
    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, &bfd_endpoint_info), "Endpoint create failed!");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Change local_state of the endpoints with random value between 0-3.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - ID of the MEP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_update_state(
    int unit,
    int endpoint_id)
{

    bcm_bfd_endpoint_info_t bfd_endpoint_info;
    int rv;
    uint8 lock_taken = FALSE;
    int state;                  /* Next state */
    int origin_state;           /* Original state of the Endpoint */

    SHR_FUNC_INIT_VARS(unit);

    rv = diag_dnx_oamp_test_bfd_lock_take(unit);
    SHR_IF_ERR_EXIT(rv);
    lock_taken = TRUE;

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);

    bfd_endpoint_info.id = endpoint_id;
    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_id, &bfd_endpoint_info), "[STATE] Endpoint get failed!");

    origin_state = bfd_endpoint_info.local_state;
    do
    {
        state = sal_rand() % 4;
    }
    while (state == origin_state);

    bfd_endpoint_info.local_state = state;
    bfd_endpoint_info.local_diag = 7;
    bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_UPDATE;
    bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_WITH_ID;

    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, &bfd_endpoint_info), "[STATE] Endpoint update failed!");

exit:
    nof_called++;
    if (lock_taken)
    {
        /*
         * Give counter processor's dma fifos lock.
         */
        rv = diag_dnx_oamp_test_bfd_lock_give(unit);
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the event counters.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_type - EVENT_TYPE_TIMEOUT to reset timeout event counters,
 *                          EVENT_TYPE_TIMEIN to reset timein event counters,
 *                          EVENT_TYPE_STATE to reset rdi event counters,
 *                          EVENT_TYPE_ALL to reset all event counters,
 * \param [in/out] num_entries - Length of the event array.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_reset_event_counters(
    int unit,
    int event_type,
    int num_entries)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (event_type)
    {
        /**
        * Reset timeout event counters
        */
        case EVENT_TYPE_TIMEOUT:
        {
            test_bfd_timeout_event_counts = 0;
            break;
        }
        /**
        * Reset timein event counters
        */
        case EVENT_TYPE_TIMEIN:
        {
            test_bfd_timein_event_counts = 0;
            break;
        }
        /**
        * Reset state event counters
        */
        case EVENT_TYPE_STATE:
        {
            test_bfd_state_event_counts = 0;
            break;
        }
        default:
        /**
        * Reset all event counters
        */
        {
            test_bfd_state_event_counts = 0;
            test_bfd_timein_event_counts = 0;
            test_bfd_timeout_event_counts = 0;
            break;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Validate that the raised events count is not different than
 *          the expected number of events.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_type - EVENT_TYPE_TIMEOUT for timeout events,
                            EVENT_TYPE_TIMEIN for timein events,
                            EVENT_TYPE_STATE for rdi events.
 * \param [in] expected_value - number if expected events.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_check_for_exact_event_interupts(
    int unit,
    int event_type,
    int expected_value)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Wait a bit to raise the interrupts */
    sal_msleep(10);

    /** The adapter doesn't reproduce events */
#if defined(ADAPTER_SERVER_MODE)
    expected_value = 0;
#endif

    switch (event_type)
    {
        case EVENT_TYPE_TIMEOUT:
            if (test_bfd_timeout_event_counts != expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected TIMEOUT events, expected %d actual %d\n",
                             expected_value, test_bfd_timeout_event_counts);
            }
            break;
        case EVENT_TYPE_TIMEIN:
            if (test_bfd_timein_event_counts != expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected TIMEIN events, expected %d actual %d\n",
                             expected_value, test_bfd_timein_event_counts);
            }
            break;
        case EVENT_TYPE_STATE:
            if (test_bfd_state_event_counts != expected_value)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Unexpected STATE events, expected %d actual %d\n",
                             expected_value, test_bfd_state_event_counts);
            }
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test failed! Not supported event type!\n");
    }

    LOG_CLI((BSL_META("Event validation pass, non unexpected interrupts were raised!\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Change the state of of a MEP.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - Endpoint ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_change_state(
    int unit,
    int endpoint_id)
{

    int exp_event_cnt;
    SHR_FUNC_INIT_VARS(unit);

    nof_called = 0;
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_update_state(unit, endpoint_id), "{6.1} Error in cb update\n");

    /*
     * Wait for all events to occur
     */
    sal_usleep(test_bfd_number_states * 1000000);

    /**
    * Check that non unexpected events were raised.
    */
    exp_event_cnt = test_bfd_number_states;
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_check_for_exact_event_interupts
                        (unit, EVENT_TYPE_STATE, exp_event_cnt),
                        "{6.2}\t Test failed! Unexpected number of timeout events raised!\n");

    test_bfd_state_event_counts = 0;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Change the period of pair of MEPs.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] global_parameters - test parameter which will be validated.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_update_endpoint(
    int unit,
    int endpoint_1_id,
    int endpoint_2_id)
{

    bcm_bfd_endpoint_info_t bfd_endpoint_info_1;
    bcm_bfd_endpoint_info_t bfd_endpoint_info_2;
    int detTime;
    int period_i;

    SHR_FUNC_INIT_VARS(unit);

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info_1);
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info_2);

    period_i = sal_rand() % 7;
    detTime = sal_rand() % 1000 + 2500; /** Detection time is in microsecond */

    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_1_id, &bfd_endpoint_info_1),
                        "{5.1} Test failed! Getting of Endpoint %d has failed!", endpoint_1_id);

    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_get(unit, endpoint_2_id, &bfd_endpoint_info_2),
                        "{5.2} Test failed! Getting of Endpoint %d has failed!", endpoint_2_id);

    /** Period is in milliseconds, detection time is in microseconds */
    bfd_endpoint_info_1.bfd_period = test_bfd_period_ctest[period_i];
    bfd_endpoint_info_2.bfd_detection_time = detTime * test_bfd_period_ctest[period_i];

    LOG_CLI((BSL_META("Update MEP with period [%d] and detect %d\n"), test_bfd_period_ctest[period_i],
             (detTime * test_bfd_period_ctest[period_i])));

    if (bfd_endpoint_info_1.flags & BCM_BFD_ENDPOINT_IPV6)
    {
        bcm_bfd_endpoint_destroy(unit, endpoint_1_id);
    }
    else
    {
        bfd_endpoint_info_1.flags |= BCM_BFD_ENDPOINT_UPDATE;
    }
    bfd_endpoint_info_1.flags |= BCM_BFD_ENDPOINT_WITH_ID;
    bfd_endpoint_info_1.flags |= BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME;

    if (bfd_endpoint_info_2.flags & BCM_BFD_ENDPOINT_IPV6)
    {
        bcm_bfd_endpoint_destroy(unit, endpoint_2_id);
    }
    else
    {
        bfd_endpoint_info_2.flags |= BCM_BFD_ENDPOINT_UPDATE;
    }
    bfd_endpoint_info_2.flags |= BCM_BFD_ENDPOINT_UPDATE;
    bfd_endpoint_info_2.flags |= BCM_BFD_ENDPOINT_WITH_ID;
    bfd_endpoint_info_2.flags |= BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME;

    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, &bfd_endpoint_info_1),
                        "{5.2} Test failed! Updating of Endpoint [%d] has failed!", endpoint_1_id);

    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_create(unit, &bfd_endpoint_info_2),
                        "{5.2} Test failed! Updating of Endpoint [%d] has failed!", endpoint_2_id);

    /*
     * Updating the period may have a ~2 second delay 
     */
    LOG_CLI((BSL_META("Wait %d ms to occur the events\n"),
             (((detTime * test_bfd_period_ctest[period_i]) + 1000000) / 1000)));
    sal_usleep((detTime * test_bfd_period_ctest[period_i]) + 1000000);

    /** On each update we got single event */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set callback to calculate event counts.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Not used in this scope.
 * \param [in] event_type - Event type, could be:
 *                              Timeout,
 *                              Timein,
 *                              State.
 * \param [in] group - Not used in this scope.
 * \param [in] endpoint - MEP id, used to calculate EP index.
 * \param [in] user_data - Additional user data if it's needed.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_cb(
    int unit,
    uint32 flags,
    bcm_bfd_event_types_t event_type,
    bcm_bfd_endpoint_t endpoint,
    void *user_data)
{
    bcm_bfd_endpoint_info_t ep_get;
    int event_i;

    SHR_FUNC_INIT_VARS(unit);

    bcm_bfd_endpoint_info_t_init(&ep_get);
    for (event_i = 0; event_i < bcmBFDEventCount; event_i++)
    {
        if (BCM_BFD_EVENT_TYPE_GET(event_type, event_i))
        {
            switch (event_i)
            {
                case bcmBFDEventEndpointTimeout:
                    test_bfd_timeout_event_counts++;
                    LOG_CLI((BSL_META("Timeout event on Endpoint 0x%d.\n"), endpoint));
                    break;
                case bcmBFDEventEndpointTimein:
                    test_bfd_timein_event_counts++;
                    LOG_CLI((BSL_META("Timein event on Endpoint 0x%d.\n"), endpoint));
                    break;
                case bcmBFDEventStateChange:
                    LOG_CLI((BSL_META("State event on Endpoint 0x%d.\n"), endpoint));
                    test_bfd_state_event_counts++;
                    if (test_bfd_number_states <= nof_called)
                    {
                        break;
                    }
                    else
                    {
                        LOG_CLI((BSL_META("Updating Endpoint 0x%d.\n"), endpoint));
                        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_update_state(unit, endpoint),
                                            "Error in cb update\n");
                        break;
                    }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register all types of events that are going to be tested.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_register_events(
    int unit)
{
    bcm_bfd_event_types_t event;
    SHR_FUNC_INIT_VARS(unit);

    BCM_BFD_EVENT_TYPE_CLEAR_ALL(event);
    BCM_BFD_EVENT_TYPE_SET(event, bcmBFDEventEndpointTimeout);
    SHR_CLI_EXIT_IF_ERR(bcm_bfd_event_register(unit, event, diag_dnx_oamp_test_bfd_cb, (void *) 1),
                        "Test failed! Register of timeout event failed.");
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(event);
    BCM_BFD_EVENT_TYPE_SET(event, bcmBFDEventEndpointTimein);
    SHR_CLI_EXIT_IF_ERR(bcm_bfd_event_register(unit, event, diag_dnx_oamp_test_bfd_cb, (void *) 1),
                        "Test failed! Register of timein event failed.");
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(event);
    BCM_BFD_EVENT_TYPE_SET(event, bcmBFDEventStateChange);
    SHR_CLI_EXIT_IF_ERR(bcm_bfd_event_register(unit, event, diag_dnx_oamp_test_bfd_cb, (void *) 1),
                        "Test failed! Register of State event failed.");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create basic OAM environment.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in/out] global_parameters - control parameter for the environment.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_environment_init(
    int unit,
    diag_dnx_oamp_bfd_test_general_t * global_parameters)
{

    SHR_FUNC_INIT_VARS(unit);

    test_bfd_number_states = global_parameters->is_state_update;

    /**
    * Create mutex.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_lock_init(unit), "Test failed! Mutex creation failed!\n");

    /**
    * Add loop-back connection to both ports.
    */
    if (sematic_scale_validation != TEST_TYPE_SEMANTIC)
    {
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                            (unit, global_parameters->port_1, global_parameters->port_2, LOOP_BACK_ENABLE),
                            "Test failed! LoopBack enable failed!\n");
    }

    /**
    * Define all global parameters.
    */
    diag_dnx_oamp_bfd_test_bfd_set_global_values();

    /**
    * Classify both ports.
    */
    SHR_CLI_EXIT_IF_ERR(bcm_port_class_set(unit, global_parameters->port_1, bcmPortClassId, global_parameters->port_1);,
                        "Test failed! Class set of port = %d failed\n", global_parameters->port_1);
    SHR_CLI_EXIT_IF_ERR(bcm_port_class_set(unit, global_parameters->port_2, bcmPortClassId, global_parameters->port_2);,
                        "Test failed! Class set of port = %d failed\n", global_parameters->port_2);

    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_l3_interface_init
                        (unit, global_parameters->port_1, global_parameters->port_2, global_parameters->is_ipv6,
                         &global_parameters->local_encap_id, &global_parameters->remote_encap_id),
                        "Test failed! l3_interface_init failed.\n");

    /**
    * Define all type tested events.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_register_events(unit), "Test failed! Event registration failed\n");

    /**
    * Reset event counters.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_reset_event_counters
                        (unit, EVENT_TYPE_ALL, global_parameters->num_entries), "Test failed! Events are not reset!\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Perform basic validation checks.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] global_parameters - test parameter which will be validated.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dnx_oamp_test_bfd_validity_check(
    int unit,
    diag_dnx_oamp_bfd_test_general_t global_parameters)
{
    int nof_max_endpoints;
    SHR_FUNC_INIT_VARS(unit);

    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    /** MEP index array limit */
    if ((global_parameters.num_entries % 2) != 0)
    {
        /*
         * Number of endpoint pairs is limited by the throughput of the ports
         */
        SHR_CLI_EXIT(_SHR_E_PARAM, "{1.1} Test failed! The number of entries should be even. Aborting test.\n");
    }

    /** MEP index array limit */
    if (global_parameters.is_ipv6)
    {
        if (global_parameters.threshold == 0)
        {
            if (global_parameters.num_entries > 6144)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM,
                             "{1.2} Test failed! Max number of MEP pairs for IPv6 is 6144, actual:%d. Aborting test.\n",
                             global_parameters.num_entries);
            }
        }
        else
        {
            if (global_parameters.num_entries >
                global_parameters.threshold + ((nof_max_endpoints - global_parameters.threshold) / 4))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM,
                             "{1.2} Test failed! Max number of MEP pairs for IPv6 is %d, actual:%d. Aborting test.\n",
                             global_parameters.threshold, global_parameters.num_entries);
            }
        }
    }
    else
    {
        if (global_parameters.threshold == 0)
        {
            if (global_parameters.num_entries > 16384)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM,
                             "{1.2} Test failed! Max number of MEP pairs for IPv4 is 16384, actual:%d. Aborting test2.\n",
                             global_parameters.num_entries);
            }
        }
        else
        {
            if (global_parameters.num_entries >
                global_parameters.threshold + ((nof_max_endpoints - global_parameters.threshold) / 4))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM,
                             "{1.2} Test failed! Max number of MEP pairs for IPv4 is %d, actual:%d. Aborting test1.\n",
                             (global_parameters.threshold + ((nof_max_endpoints - global_parameters.threshold) / 4)),
                             global_parameters.num_entries);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests OAMP with traffic.
 *          Creates different pairs of MEPs and validate event changes.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line
 *        arguments were parsed (partially)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
diag_dnx_oamp_bfd_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    diag_dnx_oamp_bfd_test_general_t global_parameters = { 0 };
    diag_dnx_oamp_traffic_test_time_calc_t time_parameters = { 0 };
    int desc_ep_1, desc_ep_2;
    int endpoint_id = 0;
    int exp_event_cnt = 0;
#if !defined(ADAPTER_SERVER_MODE)
    int first_pair_create_time = 0;
#endif
    diag_dnx_oamp_bfd_test_ip_addr_t src_ip_addr;
    diag_dnx_oamp_bfd_test_ip_addr_t remote_ip_addr;
    uint8 ip_i_1, ip_i_2;
    uint8 reset = 0;
    uint32 nof_short_entries = 0;
    uint32 nof_short_entries_created = 0;
    uint32 highest_mep_db_index = 0;
    uint32 tot_nof_of_entries;
    sh_sand_arg_t *sand_arg;
    SHR_FUNC_INIT_VARS(unit);

    /** Port on which will reside the MEPs */
    SH_SAND_GET_INT32("Port", global_parameters.port_1);
    /** Port on which will reside the MEPs */
    SH_SAND_GET_INT32("Port2", global_parameters.port_2);
    /** Number pairs of MEPs */
    SH_SAND_GET_INT32("Num_ENTries", global_parameters.num_entries);
    /** Number of updated that will be performed during the run of the test */
    SH_SAND_GET_INT32("State_UPDate", global_parameters.is_state_update);
    /** Create Ipv6 L3 interface */
    SH_SAND_GET_INT32("is_IPv6", global_parameters.is_ipv6);
    /** Test the events */
    SH_SAND_GET_INT32("Run_with_Updates", global_parameters.is_update_test);
    /** change MEP DB threshold */
    SH_SAND_GET_INT32("MEP_THReshold", global_parameters.threshold);

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        if (!sal_strncasecmp(SH_SAND_GET_NAME(sand_arg), "dport1", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
        {
            global_parameters.port_1 = SH_SAND_ARG_UINT32_DATA(sand_arg);
        }
        else if (!sal_strncasecmp(SH_SAND_GET_NAME(sand_arg), "dport2", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
        {
            global_parameters.port_2 = SH_SAND_ARG_UINT32_DATA(sand_arg);
        }
    }

    /** Test the events */
    SH_SAND_GET_INT32("SEMantic_run_ONly", sematic_scale_validation);

    if (global_parameters.is_ipv6)
    {
        LOG_CLI((BSL_META("INPUT: Running BFD IPv6 test with threshold[%d],\n"), global_parameters.threshold));
    }
    else
    {
        LOG_CLI((BSL_META("INPUT: Running BFD IPv4 test with threshold[%d],\n"), global_parameters.threshold));
    }
    LOG_CLI((BSL_META("\t *INPUT:  [%d] number of entries\n"), global_parameters.num_entries));
    tot_nof_of_entries = global_parameters.num_entries;
    LOG_CLI((BSL_META("\t\t * [%d] state updates per entry\n"), global_parameters.is_state_update));
    LOG_CLI((BSL_META("\t * On port1[%d] and port2[%d]\n"), global_parameters.port_1, global_parameters.port_2));

#if !defined(ADAPTER_SERVER_MODE)
    /** In case of scanner limitation skip this test */
    if (dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_scanner_limitation))
    {
        LOG_CLI((BSL_META("\nThe device has scanner_limitation, aborting the run..\n")));
        SHR_FUNC_EXIT;
    }
#endif

#if defined(ADAPTER_SERVER_MODE)
    /** In case of scanner limitation skip this test */
    if (sematic_scale_validation == TEST_TYPE_SEMANTIC)
    {
        LOG_CLI((BSL_META("\nadapter is limited for scale testing, aborting the run..\n")));
        SHR_FUNC_EXIT;
    }
#endif

    LOG_CLI((BSL_META("\nRunning please wait..\n")));
    /** Perform basic validity checks */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_validity_check(unit, global_parameters), "{1}\t Test failed! \n");

    /** 
    * Init time variables
    */
    diag_dnx_oamp_traffic_test_time_calc_t_init(&time_parameters);

    /**
    * Configure BFD environment.
    * Set LoopBack on both ports.
    * Set L3 interface
    * Register events.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_environment_init(unit, &global_parameters),
                        "{2}\t Test failed! Init of the environment has failed! \n");
#ifdef BCM_DNX2_SUPPORT
    if (DNX_OAMP_IS_V1(unit))
    {
        dnx_oamp_v1_mep_db_highest_index_get(unit, &highest_mep_db_index);
    }
#endif
    /** Because of limitation in the MDB profile we are able to create up to 6k MEPs for now */
    if (global_parameters.threshold)
    {
        if (global_parameters.is_ipv6)
        {
            highest_mep_db_index = 6144;
            global_parameters.threshold = highest_mep_db_index;
            LOG_CLI((BSL_META("UPDATED due MDB and scanner Limit: Running BFD IPv6 test with threshold[%d],\n"),
                     global_parameters.threshold));

        }
        else
        {
            global_parameters.threshold = highest_mep_db_index;
            LOG_CLI((BSL_META("UPDATED due MDB and scanner Limit: Running BFD IPv4 test with threshold[%d],\n"),
                     global_parameters.threshold));
        }
        tot_nof_of_entries = 7 * dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) + global_parameters.threshold;
        if (tot_nof_of_entries > global_parameters.num_entries)
        {
            tot_nof_of_entries = global_parameters.num_entries;
        }
        LOG_CLI((BSL_META("\t *UPDATED due MDB and scanner Limit:  [%d] number of entries\n"), tot_nof_of_entries));
    }

    /**
    * Loop "N" number of times based on number of entries.
    * In each iteration create 2 MEPs
    */
    for (endpoint_id = 0; endpoint_id < (tot_nof_of_entries - nof_short_entries_created); endpoint_id++)
    {

        /** 
        * Create N number of 1/4 MEPs where N == global_parameters.threshold.
        * Then reset the loop variable to continue from the last MEP ID,
        * For example if global_parameters.threshold=8k, first self-contained id should be 8k,
        * Once passed through this never go again
        */
        if ((endpoint_id == global_parameters.threshold) && !reset && global_parameters.threshold)
        {
            nof_short_entries = global_parameters.threshold < 8191 ? 8192 : global_parameters.threshold;
            nof_short_entries_created = global_parameters.threshold;
            global_parameters.threshold = 0;
            endpoint_id = 0;
            reset = 1;
        }
        /** Calculate MEP IDs */

        desc_ep_1 = endpoint_id * (global_parameters.threshold > endpoint_id ? 1 : 4) + nof_short_entries;
        endpoint_id++;
        /** Calculate MEP IDs */
        desc_ep_2 = endpoint_id * (global_parameters.threshold > endpoint_id ? 1 : 4) + nof_short_entries;

        /** Choose random source and destination IP addresses from predefined structure */
        do
        {
            ip_i_1 = sal_rand() % 8;
            ip_i_2 = sal_rand() % 8;
        }
        while (ip_i_1 == ip_i_2);

        sal_memcpy(src_ip_addr.ipv6_addr, test_bfd_IPv6_address[ip_i_1], 16);
        sal_memcpy(remote_ip_addr.ipv6_addr, test_bfd_IPv6_address[ip_i_2], 16);
        src_ip_addr.ipv4_addr = test_bfd_IP_address_ctest[ip_i_1];
        remote_ip_addr.ipv4_addr = test_bfd_IP_address_ctest[ip_i_2];

        /**
        * Start the timer.
        */
        time_parameters.start_time = sal_time_usecs();

        /**
        * Create one PAIR of MEPs.
        */
        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_createEndpoint
                            (unit, global_parameters.port_2, global_parameters.local_encap_id, global_parameters,
                             src_ip_addr, remote_ip_addr, desc_ep_1, desc_ep_2),
                            "{3}\t Test failed! Endpoint creation failed!\n");

        SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_createEndpoint
                            (unit, global_parameters.port_1, global_parameters.remote_encap_id, global_parameters,
                             remote_ip_addr, src_ip_addr, desc_ep_2, desc_ep_1),
                            "{4}\t Test failed! Endpoint creation failed!\n");

        /**
        * Stop timer when both MEPs with RMEPs are created.
        */
        time_parameters.end_time = sal_time_usecs();

        /**
        * Calculate the time for the first pair.
        */
#if !defined(ADAPTER_SERVER_MODE)
        if (endpoint_id == 1 && !reset)
        {
            first_pair_create_time = time_parameters.end_time - time_parameters.start_time;
        }
#endif
        /**
        * Calculate current time interval for pair creation.
        * Add it to the actual maximum time.
        * Increase the expected time with 2 iterations.
        */
        diag_dnx_oamp_test_bfd_time_calculation(&time_parameters);

        /** If the update is enabled but state not, update the MEP periods */
        if (global_parameters.is_update_test)
        {
            if (!global_parameters.is_state_update)
            {
                /** 
                * Update MEPs intervals
                */
                sal_usleep(100);
                SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_update_endpoint(unit, desc_ep_1, desc_ep_2),
                                    "{5}\t Test failed! Update attempt failed!\n");
            }
            else
            {
                /** 
                * Update MEPs state
                */
                sal_usleep(100);
                SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_change_state(unit, desc_ep_1),
                                    "{6}\t Test failed! State update attempt failed!\n");
            }
        }
    }

    /** Wait 5 seconds */
    sal_sleep(5);

    /** After a 5 seconds delay in working environment, we don't expect any events */
    exp_event_cnt = 0;
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_check_for_exact_event_interupts
                        (unit, EVENT_TYPE_TIMEOUT, exp_event_cnt),
                        "{7}\t Test failed! Unexpected number of timeout events raised!\n");
    exp_event_cnt = 0;
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_check_for_exact_event_interupts
                        (unit, EVENT_TYPE_TIMEIN, exp_event_cnt),
                        "{8}\t Test failed! Unexpected number of timein events raised!\n");
    exp_event_cnt = 0;
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_check_for_exact_event_interupts
                        (unit, EVENT_TYPE_STATE, exp_event_cnt),
                        "{9}\t Test failed! Unexpected number of state events raised!\n");

    /**
    * Validate that the number of entries is equal to number of time calculations.
    */
    if (time_parameters.interaction != tot_nof_of_entries)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "{10}\t Actual calculated MEPs[%d] is different than the actual MEPs[%d]\n",
                     time_parameters.interaction, tot_nof_of_entries);
    }

/** Don't test the timing on adapter */
#if !defined(ADAPTER_SERVER_MODE)
    /**
    * Validate that actual time is lower than the expected.
    */
    if (!global_parameters.is_ipv6)
    {
        if (first_pair_create_time > BFD_EXPECTED_CREATE_TIME_FIRST_PAIR)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "{11}\t Actual creation time:%d is higher than expected:%d\n",
                         first_pair_create_time, BFD_EXPECTED_CREATE_TIME_FIRST_PAIR);
        }

        /**
        * Validate that actual time is lower than the expected.
        */
        if (time_parameters.actual_create_time > time_parameters.expected_max_create_time)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "{11}\t Actual creation time:%d is higher than expected:%d\n",
                         time_parameters.actual_create_time, time_parameters.expected_max_create_time);
        }
    }
#endif

    /** In semantic aspect we need to validate the maximum scale and not the traffic */
    if (sematic_scale_validation == TEST_TYPE_SEMANTIC)
    {
        /**
        * Clean up.
        */
        LOG_CLI((BSL_META("SEMANTIC TEST PASS !!! Cleaning up..\n")));

        /** Destroy all MEPs */
        SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_destroy_all(unit),
                            "{20}\t Test failed! bcm_oam_endpoint_destroy_all failed!\n");
        SHR_FUNC_EXIT;
    }

    /**
    * Disable the loop-back to raise timeout events.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                        (unit, global_parameters.port_1, global_parameters.port_2, LOOP_BACK_DISABLE),
                        "{12}\t Test failed! LoopBack disable failed!\n");

    /** Wait 5 second to raise all events */
    sal_sleep(5);

    /**
    * Check that the correct amount of timeout events were raised.
    * We expect single LOC per MEP
    */
    exp_event_cnt = endpoint_id;
#if defined(ADAPTER_SERVER_MODE)
    exp_event_cnt = 0;
#endif
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_check_for_exact_event_interupts(unit, EVENT_TYPE_TIMEOUT, exp_event_cnt),
                        "{13}\t Test failed! Not enough timeout events were raised!\n");

    /**
    * Enable the loop-back to restore the connection.
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                        (unit, global_parameters.port_1, global_parameters.port_2, LOOP_BACK_ENABLE),
                        "{14}\t Test failed! LoopBack disable failed!\n");

    /** Wait 5 second to clear the LOC and raise timein events */
    sal_sleep(5);

    /** 
    * Check that the correct amount of timein events were raised.
    * We expect single LOC clear per MEP
    */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_check_for_exact_event_interupts(unit, EVENT_TYPE_TIMEIN, exp_event_cnt),
                        "{15}\t Test failed! Not enough timein events were raised!\n");

    /** 
    * During this phase we don't expect any state event changes
    */
    exp_event_cnt = 0;
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_check_for_exact_event_interupts
                        (unit, EVENT_TYPE_STATE, exp_event_cnt),
                        "{16}\t Test failed! Unexpected number of state events raised!\n");

    /** Disable the loop-back. */
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_loopback_on_off
                        (unit, global_parameters.port_1, global_parameters.port_2, LOOP_BACK_DISABLE),
                        "{17}\t Test failed! LoopBack disable failed!\n");

    /** Wait a bit to avoid warnings during the MEPs' destroy */
    sal_sleep(5);

    /** 
    * Check that the correct amount of LOC events were raised.
    */
    exp_event_cnt = endpoint_id * 2;
#if defined(ADAPTER_SERVER_MODE)
    exp_event_cnt = 0;
#endif
    SHR_CLI_EXIT_IF_ERR(diag_dnx_oamp_test_bfd_check_for_exact_event_interupts(unit, EVENT_TYPE_TIMEOUT, exp_event_cnt),
                        "{18}\t Test failed! Not enough timein events were raised!\n");

    /**
    * Clean up.
    */
    LOG_CLI((BSL_META("TRAFFIC TEST PASS !!! Cleaning up..\n")));

    /** Destroy all MEPs */
    SHR_CLI_EXIT_IF_ERR(bcm_bfd_endpoint_destroy_all(unit),
                        "{20}\t Test failed! bcm_oam_endpoint_destroy_all failed!\n");

exit:

    /** End of the test */
    LOG_CLI((BSL_META("*********************** TEST END !!! ***********************\n")));
    SHR_FUNC_EXIT;
}

/**
 * \brief - Callback returns variable type and unique id
 *          for dynamic option.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] keyword - Option name
 * \param [out] type_p - Opttion data type
 * \param [out] id_p - Option identifier to be used by shell command
 * \param [in] ext_ptr_p - Pointer for different kind os extensions
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
diag_dnx_oamp_bfd_test_option_cb(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    if (!sal_strncasecmp(keyword, "dport1", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
        || !sal_strncasecmp(keyword, "dport2", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
    {
        if (type_p != NULL)
            *type_p = SAL_FIELD_TYPE_INT32;
        if (id_p != NULL)
            *id_p = 1004;
        if (ext_ptr_p != NULL)
        {
            *ext_ptr_p = (void *) cmd_dnx_sh_valid_ports_table;
        }
        return _SHR_E_NONE;
    }
    else
        return _SHR_E_NOT_FOUND;
}

/** Supported endpoint types   */
static sh_sand_enum_t diag_dnx_oamp_bfd_test_type_enum_table[] = {
    {"TRAFFIC", TEST_TYPE_TRAFFIC},
    {"SEMANTIC", TEST_TYPE_SEMANTIC},
    {NULL}
};

/** Test arguments   */
sh_sand_option_t diag_dnx_oamp_bfd_test_options[] = {
    {"Port", SAL_FIELD_TYPE_INT32, "Port on which the mep reside", "13"}
    ,
    {"Port2", SAL_FIELD_TYPE_INT32, "Port on which the mep reside", "14"}
    ,
    {"Num_ENTries", SAL_FIELD_TYPE_INT32, "Number of OAMP entries", "2"}
    ,
    {"State_UPDate", SAL_FIELD_TYPE_INT32, "Number of updates", "0"}
    ,
    {"is_IPv6", SAL_FIELD_TYPE_INT32, "Run BFDoIpv6", "0"}
    ,
    {"Run_with_Updates", SAL_FIELD_TYPE_INT32, "Validate event changes", "0"}
    ,
    {"MEP_THReshold", SAL_FIELD_TYPE_INT32, "Configure the mep db full entry threshold.", "0"}
    ,
    {"SEMantic_run_ONly", SAL_FIELD_TYPE_INT32, "Configure the mep only, without traffic.", "TRAFFIC",
     (void *) diag_dnx_oamp_bfd_test_type_enum_table}
    ,
    {NULL}
};

/** Test manual   */
sh_sand_man_t diag_dnx_oamp_bfd_test_man = {
    /** Brief */
    "Traffic test of basic OAMP BFD Ipv4/Ipv6 MEPs.",
    /** Full */
    "Create OAMP BFD MEP pairs, up to maximum scale(64k-short, 16k-self). Then..:"
        "1. Check for unexpected LOC set, LOC clear, State events."
        "2. Check for expected LOC set, LOC clear, State events."
        "3. Validate that the expected time is not changed."
        "Possible parameters:"
        "Port               - Port on which the first MEP will reside."
        "Port2              - Port on which the second MEP will reside."
        "is_IPv6            - choose the type of the MEP."
        "is_IPv6=0          - Ipv4 only"
        "is_IPv6=1          - Ipv6 only"
        "Num_ENTries        - Number of entries, should be always even, > 0, and lower then 65k(short) or 16k(self)."
        "State_UPDate       - If value is bigger than -0-, perform value amount of updates **per pair!"
        "Run_with_Updates   - Will perform update after every create of pair of MEPs."
        "MEP_THReshold      - Can determine the amount of short entries, must be divided on 8k"
        "SEMantic_run_ONly  - Run the test on semantic level"
    /** Synopsis   */
        "ctest oamp bfd traffic [Port=<value> Port2=<value> is_IPv6=<value> Num_ENTries=<value> State_UPDate=<value> Run_with_Updates=<value> MEP_THReshold=<value> SEMantic_run_ONly=<TRAFFIC/SEMANTIC>",
    /** Example   */
    "ctest oamp bfd traffic Port=13 Port2=14 Num_ENTries=1 State_UPDate=1 Run_with_Updates=1 MEP_THReshold=0 SEMantic_run_ONly=TRAFFIC",
};

/** Automatic test list (ctest oamp run)   */
sh_sand_invoke_t diag_dnx_oamp_bfd_tests[] = {
    {"bfd_ipv4_scale_test", "dport1=eth1 dport2=eth2 Num_eNTries=1000 is_IPv6=0 Run_with_Updates=0 State_UPDate=0"}
    ,
    {"bfd_ipv6_scale_test", "dport1=eth1 dport2=eth3 Num_eNTries=100 is_IPv6=1 Run_with_Updates=10 State_UPDate=5"}
    ,
    {"bfd_ipv4_time_interval_test", "dport1=eth1 dport2=eth2 Num_eNTries=10 is_IPv6=0 Run_with_Updates=1 State_UPDate=0"}
    ,
    {"bfd_ipv4_state_test", "dport1=eth2 dport2=eth3 Num_eNTries=10 is_IPv6=0 Run_with_Updates=1 State_UPDate=5"}
    ,
    {"bfd_ipv6_10_eps_test", "dport1=eth2 dport2=eth3 Num_ENTries=10"}
    ,
    {"bfd_ipv6_2_eps_events_test", "dport1=eth2 dport2=eth3 Num_ENTries=2 Run_with_Updates=1"}
    ,
    {"bfd_ipv6_2_eps_no_updates", "dport1=eth2 dport2=eth4 Num_ENTries=2 is_IPv6=1 Run_with_Updates=0 State_UPDate=0"}
    ,
    {NULL}
};
