/*
 * ! \file ctest_dnx_l2.c Contains all of the L2 Ctest commands
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_L2TABLE

/*************
 * INCLUDES  *
 *************/
/** sal */
#include <sal/appl/config.h>
#include <sal/compiler.h>

/** appl */
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_l2.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

/** shared */
#include <shared/bsl.h>
#include <shared/utilex/utilex_framework.h>

/** bcm */
#include <bcm/l2.h>
#include <bcm/vswitch.h>
#include <bcm/rx.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/l2/l2_learn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

/** soc */
#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>

/*************
 * EXTERNS   *
 *************/
extern void increment_macaddr(
    sal_mac_addr_t macaddr,
    int amount);
/*************
 * DEFINES   *
 *************/

/*************
* FUNCTIONS *
*************/

uint32 mdb_timers_group;
uint32 flush_timers_group;

/* *INDENT-OFF* */
static sh_sand_invoke_t dnx_l2_mact_tests[] = {
    {"add1000", "count=1000 port=200", CTEST_POSTCOMMIT},
    {NULL}
};

static sh_sand_option_t dnx_l2_mact_arguments[] = {
    {"mac", SAL_FIELD_TYPE_MAC, "mac_addr", "01:02:03:04:05:06"},
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN ID", "1", NULL, "1-4095"},
    {"vsi", SAL_FIELD_TYPE_INT32, "VSI", "0"},
    {"count", SAL_FIELD_TYPE_UINT32, "Number of addresses to add", "0"},
    {"portbitmap", SAL_FIELD_TYPE_PORT, "portbitmap or port id", "none"},
    {NULL}
};

sh_sand_man_t dnx_l2_mact_man = {
    "Test L2 Table operations",
    "Test adding entries to MACT"
};
/* *INDENT-ON* */

static shr_error_e
dnx_l2_add_entries(
    int unit,
    uint32 entries_count,
    uint32 is_static,
    bcm_vlan_t vsi)
{
    bcm_mac_t mac_addr = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    bcm_l2_addr_t l2addr;
    int mac_ind;
    sal_usecs_t usec;

    SHR_FUNC_INIT_VARS(unit);

    bcm_l2_addr_t_init(&l2addr, mac_addr, vsi);

    l2addr.port = 200;

    if (is_static)
    {
        l2addr.flags = BCM_L2_STATIC;
    }

    usec = sal_time_usecs();

    for (mac_ind = 0; mac_ind < entries_count; mac_ind++)
    {
        SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));
        increment_macaddr(l2addr.mac, 1);
    }

    usec = sal_time_usecs() - usec;
    LOG_CLI((BSL_META("%d addresses added in %u us\n"), entries_count, usec));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Call back to print entry includeing the group
 */
volatile int call_back_done = FALSE;
int call_back_user_data;
int call_back_counter;
int
call_back_get_block(
    int unit,
    bcm_l2_addr_t * l2e,
    void *ud)
{
    int *user_data = ud;

    printf("call_back_get_block: user_data = %d.\n", *user_data);

    if (l2e == NULL)
    {
        printf("call_back_get_block: finished, no more data.\n");
        call_back_done = TRUE;
    }
    else
    {
        printf("| %02x:%02x:%02x:%02x:%02x:%02x |",
               l2e->mac[0], l2e->mac[1], l2e->mac[2], l2e->mac[3], l2e->mac[4], l2e->mac[5]);
        printf(" VLAN=0x%04x | PORT=0x%08x |", l2e->vid, l2e->port);
        printf("GROUP=0x%x|\n", l2e->group);

        call_back_counter++;
    }

    return BCM_E_NONE;
}

static shr_error_e
dnx_l2_flush_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_mac_t mac_addr = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    uint32 vsi;
    uint32 mac_entries_count;
    uint32 is_static;
    uint32 non_blocking;
    bcm_l2_addr_t l2addr;
    sal_usecs_t usec;
    int flags = 0;
    int flush_done = 0;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("vsi", vsi);
    SH_SAND_GET_INT32("count", mac_entries_count);
    SH_SAND_GET_INT32("is_static", is_static);
    SH_SAND_GET_INT32("non_blocking", non_blocking);

    if (non_blocking)
    {
        is_static = 0;
        flags = BCM_L2_REPLACE_IGNORE_PENDING;
    }
    else
    {
        flags = BCM_L2_REPLACE_MATCH_STATIC;
    }
    /** Add MAC entries */
    SHR_IF_ERR_EXIT(dnx_l2_add_entries(unit, mac_entries_count, is_static, vsi));

    /** Measure traverse time */
    usec = sal_time_usecs();

    SHR_IF_ERR_EXIT(bcm_l2_traverse(unit, NULL, NULL));

    usec = sal_time_usecs() - usec;
    LOG_CLI((BSL_META("Traversing through all the entries took %u us\n"), usec));

    /*
     * Print all VSI entries: 
     */
    if (non_blocking)
    {
        bcm_l2_addr_t match_addr;
        uint32 match_flags;

        bcm_l2_addr_t_init(&match_addr, mac_addr, vsi);
        match_flags = BCM_L2_TRAVERSE_MATCH_VLAN | BCM_L2_TRAVERSE_IGNORE_PENDING;
        call_back_user_data = vsi;
        call_back_counter = 0;
        call_back_done = FALSE;
        LOG_CLI((BSL_META("Starting L2 match traverse non-blocking test:\n")));

        SHR_IF_ERR_EXIT(bcm_l2_matched_traverse
                        (unit, match_flags, &match_addr, call_back_get_block, &call_back_user_data));

        while (call_back_done == FALSE)
        {
            LOG_CLI((BSL_META("Waiting for call-back to finish.\n")));
            sal_sleep(1);
        }

        LOG_CLI((BSL_META("Call-back finished, call_back_counter = %d.\n"), call_back_counter));
        if (call_back_counter != mac_entries_count)
        {
            LOG_CLI((BSL_META("Error! count = %d but call_back_counter = %d!\n"), mac_entries_count,
                     call_back_counter));
        }
        call_back_counter = 0;
        call_back_done = FALSE;
    }

    flags |= BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_NO_CALLBACKS | BCM_L2_REPLACE_IGNORE_DES_HIT;

    bcm_l2_addr_t_init(&l2addr, mac_addr, vsi);

    /** In case that vsi is provided, check if it is valid */
    if (vsi > 0)
    {
        SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
        if (vsi >= nof_vsis)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. vsi = %d is not a valid vsi. Expected a number smaller than %d \n", vsi,
                         nof_vsis);
        }

        l2addr.vid = vsi;

        flags |= BCM_L2_REPLACE_MATCH_VLAN;
    }

    usec = sal_time_usecs();

    SHR_IF_ERR_EXIT(bcm_l2_replace(unit, flags, &l2addr, 0, 0, 0));

    usec = sal_time_usecs() - usec;
    LOG_CLI((BSL_META("Delete the entries took %u us\n"), usec));

    if (non_blocking)
    {
        while (!flush_done)
        {
            bcm_switch_control_get(unit, bcmSwitchTraverseCommitDone, &flush_done);
            sal_sleep(1);
        }

        bcm_switch_control_set(unit, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleClear);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Measure time for Adding MACT entries
*****************************************************/
static shr_error_e
dnx_l2_mact_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_mac_t mac_addr;
    bcm_vlan_t vlan;
    uint32 vsi;
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    int nof_ports;
    bcm_l2_addr_t l2addr;
    int count;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_MAC("mac", mac_addr);
    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_PORT("portbitmap", logical_ports);
    SH_SAND_GET_INT32("vsi", vsi);
    SH_SAND_GET_UINT32("count", count);

    bcm_l2_addr_t_init(&l2addr, mac_addr, vlan);

    /** In case that vsi is provided, check if it is valid */
    if (vsi > 0)
    {
        SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
        if (vsi >= nof_vsis)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. vsi = %d is not a valid vsi. Expected a number smaller than %d \n", vsi,
                         nof_vsis);
        }
        /*
         * If vsi is provided, the entry will be added to FWD_MACT_IVL
         * In IVL mode the user should set a VSI value.
         * Since there is no VSI field in bcm_l2_addr_t we are currently using the modid for setting the VSI.
         */
        l2addr.modid = vsi;
    }

    BCM_PBMP_COUNT(logical_ports, nof_ports);
    if (nof_ports == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No port on input\n");
    }

    BCM_PBMP_ITER(logical_ports, port)
    {
        int mac_ind;
        sal_usecs_t usec;
        l2addr.port = port;

        SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("mdb em measurements", &mdb_timers_group));
        SHR_IF_ERR_EXIT(utilex_ll_timer_clear_all(mdb_timers_group));

        usec = sal_time_usecs();
        for (mac_ind = 0; mac_ind < count; mac_ind++)
        {
            SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));
            increment_macaddr(l2addr.mac, 1);
        }
        usec = sal_time_usecs() - usec;
        LOG_CLI((BSL_META("%d addresses %u ms\n"), count, usec));
        utilex_ll_timer_print_all(mdb_timers_group);
        utilex_ll_timer_clear_all(mdb_timers_group);

        utilex_ll_timer_group_free(mdb_timers_group);
    }
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t limit_types[] = {
    {"global", BCM_L2_LEARN_LIMIT_SYSTEM, "Global limitation on number of MAC addresses learned"},
    {"vsi", BCM_L2_LEARN_LIMIT_VLAN, "Limitation Per VSI"},
    {"lif", BCM_L2_LEARN_LIMIT_PORT, "Limitation Per GLobal IN_LIF"},
    {NULL}
};

static sh_sand_invoke_t dnx_l2_limit_tests[] = {
    {"global", "type=global count=10 clean", CTEST_POSTCOMMIT},
    {"lif", "type=lif vsi=100 count=2 mac=11:22:33:44:55:66", CTEST_POSTCOMMIT},
    {"vsi", "type=vsi vsi=100 add=3", CTEST_POSTCOMMIT},
    {NULL}
};

static sh_sand_man_t dnx_l2_limit_man = {
    "Test L2 Learn Limits",
};

static sh_sand_option_t dnx_l2_limit_arguments[] = {
    {"type", SAL_FIELD_TYPE_ENUM, "Type of limit to impose", "global", (void *) limit_types},
    {"clean", SAL_FIELD_TYPE_BOOL, "Flag to enforce clearing the environment after the test", "No"},
    {"lif", SAL_FIELD_TYPE_BOOL, "Flag to limit per OUT_LIF instead of VSI", "No"},
    {"vsi", SAL_FIELD_TYPE_INT32, "VSI for Learn Limit Test", "0"},
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN ID. 0 means no vlan limit will be imposed", "0", NULL, "0-4095"},
    {"mac", SAL_FIELD_TYPE_MAC, "MAC Address to be used as first one in generated set", "00:01:02:04:08:00"},
    {"count", SAL_FIELD_TYPE_INT32, "Maximum number of MAC addresses that may be learned, accordingly to the type",
     "1", NULL, "1-0x1FFFFF"},
    {"add", SAL_FIELD_TYPE_INT32, "Number of additional mac addresses to generate overflowing MACT", "1"},
    {NULL}
};

static shr_error_e
ctest_dnx_l2_send_packet_stream(
    int unit,
    bcm_port_t from_port,
    uint32 vlan,
    sal_mac_addr_t dst_mac_addr,
    sal_mac_addr_t src_mac_addr,
    int dst_count)
{
    rhhandle_t packet_h = NULL;
    char mac_str[RHNAME_MAX_SIZE];
    int i_pkt;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_get(unit, from_port, &flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));

    for (i_pkt = 0; i_pkt < dst_count; i_pkt++)
    {
        sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                     dst_mac_addr[0], dst_mac_addr[1], dst_mac_addr[2], dst_mac_addr[3], dst_mac_addr[4],
                     dst_mac_addr[5] + i_pkt);
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", mac_str));
        sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                     src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4],
                     src_mac_addr[5] + i_pkt);
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, from_port, packet_h, SAND_PACKET_RX));
    }
    /*
     * Allow to possible other threads: learning/rx to kick in
     */
    sal_usleep(10000);

exit:
    diag_sand_packet_free(unit, packet_h);
    SHR_FUNC_EXIT;

}

/*
 * This routine assigns both ports to the same LIF, whereas ctest_dnx_l2_vsi_config creats 2 lif on the same VSI
 */
shr_error_e
ctest_dnx_l2_vsi_config_match(
    int unit,
    bcm_port_t port1,
    bcm_port_t port2,
    bcm_vlan_t vlan)
{
    bcm_vlan_port_t port_vlan;
    bcm_pbmp_t pbmp, ubmp;
    bcm_port_match_info_t port_match;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * VSI Create 
     */
    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vlan));

    bcm_vlan_port_t_init(&port_vlan);

    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    port_vlan.match_vlan = vlan;
    port_vlan.port = port1;
    port_vlan.vsi = 0;

    /** LIF Create*/
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &port_vlan));

    /*
     * Assign VSI to LIF 
     */
    SHR_IF_ERR_EXIT(bcm_vswitch_port_add(unit, vlan, port_vlan.vlan_port_id));

    bcm_port_match_info_t_init(&port_match);

    port_match.match = BCM_PORT_MATCH_PORT_CVLAN;
    port_match.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    port_match.port = port2;
    port_match.match_vlan = vlan;
    /*
     * Assign port2 to the same LIF 
     */
    SHR_IF_ERR_EXIT(bcm_port_match_add(unit, port_vlan.vlan_port_id, &port_match));

    /*
     * Add ports to vlan 
     */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
config_dnx_l2_vsi_set_ivl(
    int unit,
    bcm_vlan_t vsi)
{
    bcm_vlan_control_vlan_t control;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_control_vlan_get(unit, vsi, &control));

    control.flags2 = BCM_VLAN_FLAGS2_IVL;

    SHR_IF_ERR_EXIT(bcm_vlan_control_vlan_set(unit, vsi, control));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
config_dnx_l2_vsi_set_aging_cycles(
    int unit,
    bcm_vlan_t vsi,
    int cycles)
{
    bcm_vlan_control_vlan_t control;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_control_vlan_get(unit, vsi, &control));

    control.aging_cycles = cycles;

    SHR_IF_ERR_EXIT(bcm_vlan_control_vlan_set(unit, vsi, control));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
config_dnx_l2_vsi_set_mc(
    int unit,
    bcm_port_t port1,
    bcm_port_t port2,
    bcm_vlan_t vsi,
    bcm_multicast_t mc_id)
{
    bcm_multicast_replication_t rep_array[2];
    bcm_vlan_control_vlan_t control;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_control_vlan_get(unit, vsi, &control));

    SHR_IF_ERR_EXIT(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, &mc_id));

    sal_memset(&rep_array, 0, sizeof(bcm_multicast_replication_t) * 2);

    rep_array[0].port = port1;
    rep_array[1].port = port2;

    SHR_IF_ERR_EXIT(bcm_multicast_add(unit, mc_id, BCM_MULTICAST_INGRESS_GROUP, 2, rep_array));

    control.unknown_unicast_group = control.unknown_multicast_group = control.broadcast_group = mc_id;

    SHR_IF_ERR_EXIT(bcm_vlan_control_vlan_set(unit, vsi, control));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
config_dnx_l2_vsi_create(
    int unit,
    bcm_port_t port1,
    bcm_port_t port2,
    bcm_vlan_t vlan,
    bcm_vlan_t vsi,
    bcm_gport_t * gport1_p,
    bcm_gport_t * gport2_p)
{
    bcm_vlan_port_t port_vlan;
    bcm_pbmp_t pbmp, ubmp;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_dnx_l2_vsi_exists(unit, vsi) == _SHR_E_NONE)
    {
        /** nothing to do - already exists */
        SHR_EXIT();
    }
    /** VSI Create */
    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vsi));

    bcm_vlan_port_t_init(&port_vlan);
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    port_vlan.match_vlan = vlan;
    port_vlan.port = port1;
    port_vlan.vsi = 0;

    /** 1st LIF Create*/
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &port_vlan));
    if (gport1_p != NULL)
    {
        *gport1_p = port_vlan.vlan_port_id;
    }

    /** Assign VSI to 1st LIF */
    SHR_IF_ERR_EXIT(bcm_vswitch_port_add(unit, vsi, port_vlan.vlan_port_id));

    bcm_vlan_port_t_init(&port_vlan);
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    port_vlan.match_vlan = vlan;
    port_vlan.port = port2;
    port_vlan.vsi = 0;

    /** 2nd LIF Create*/
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &port_vlan));
    if (gport2_p != NULL)
    {
        *gport2_p = port_vlan.vlan_port_id;
    }

    /** Assign VSI to 2nd LIF */
    SHR_IF_ERR_EXIT(bcm_vswitch_port_add(unit, vsi, port_vlan.vlan_port_id));

    /** Add both ports to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l2_limit_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int vlan, limit, cur_limit, add_count, packets2send;
    uint32 result_counter, prev_counter;
    int clean, type;
    int vsi;
    bcm_port_t port1 = 200;
    bcm_port_t port2 = 203;
    sal_mac_addr_t mac_addr;
    sal_mac_addr_t mac_addr_2 = { 00, 11, 22, 33, 44, 55 };
    bcm_gport_t gport1 = 0;
    bcm_gport_t gport2 = 0;
    dnx_learn_limit_mode_t learn_limit_mode = dnx_data_l2.general.l2_learn_limit_mode_get(unit);
    bcm_l2_learn_limit_t l2_learn_limit;
    char *new_mode = NULL;
    rhhandle_t ctest_soc_set_h = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("vsi", vsi);
    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_INT32("count", limit);
    SH_SAND_GET_INT32("add", add_count);
    SH_SAND_GET_BOOL("clean", clean);
    SH_SAND_GET_ENUM("type", type);
    SH_SAND_GET_MAC("mac", mac_addr);

    /** Verify that learn mode is suited to the command */
    if ((type == BCM_L2_LEARN_LIMIT_VLAN) && (learn_limit_mode != LEARN_LIMIT_MODE_VLAN))
    {
        new_mode = "VLAN";
    }
    if ((type == BCM_L2_LEARN_LIMIT_PORT) && (learn_limit_mode != LEARN_LIMIT_MODE_VLAN_PORT))
    {
        new_mode = "VLAN_PORT";
    }

    if (new_mode != NULL)
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"l2_learn_limit_mode", new_mode}
            ,
            {NULL}
        };

        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }
    /*
     * Prepare l2_learn_limit and fetch current limit for requested configuration
     */
    bcm_l2_learn_limit_t_init(&l2_learn_limit);

    if (type == BCM_L2_LEARN_LIMIT_SYSTEM)
    {
        l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_SYSTEM;
        LOG_CLI((BSL_META("Testing global learn limit\n")));
    }
    else
    {
        dnx_learn_limit_mode_t learn_limit_mode;

        if (vlan == 0)
        {   /** To distinguish between vlan based and vsi based objeccs */
            vlan = vsi + 100;
        }
        SHR_IF_ERR_EXIT(config_dnx_l2_vsi_create(unit, port1, port2, vlan, vsi, &gport1, &gport2));
        if (type == BCM_L2_LEARN_LIMIT_PORT)
        {
            l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_PORT;
            l2_learn_limit.port = gport1;
            LOG_CLI((BSL_META("Testing learn limit on gport:0x%08x\n"), gport1));
            if ((learn_limit_mode = dnx_data_l2.general.l2_learn_limit_mode_get(unit)) != LEARN_LIMIT_MODE_VLAN_PORT)
            {
                SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Supported only in VLAN_PORT mode\n");
            }
        }
        else
        {
            l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_VLAN;
            l2_learn_limit.vlan = vsi;
            LOG_CLI((BSL_META("Testing learn limit on vsi:%d\n"), vsi));
            if ((learn_limit_mode = dnx_data_l2.general.l2_learn_limit_mode_get(unit)) != LEARN_LIMIT_MODE_VLAN)
            {
                SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Supported only in VLAN mode\n");
            }
        }
    }

    SHR_IF_ERR_EXIT(bcm_l2_learn_limit_get(unit, &l2_learn_limit));
    cur_limit = l2_learn_limit.limit;

    LOG_CLI((BSL_META("Current limit:%d Setting limit:%d\n"), cur_limit, limit));

    if (clean == TRUE)
    {
        /*
         * Clear the entire table
         */
        SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));
        prev_counter = 0;
    }
    else
    {
        if (type == BCM_L2_LEARN_LIMIT_PORT)
        {
            SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_inlif_count(unit, gport1, &prev_counter));
        }
        else if (type == BCM_L2_LEARN_LIMIT_VLAN)
        {
            SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_vsi_count(unit, vsi, &prev_counter));
        }
        else
        {
            SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_count(unit, &prev_counter));
        }
    }
    /*
     * Set new limit
     */
    l2_learn_limit.limit = limit;
    SHR_IF_ERR_EXIT(bcm_l2_learn_limit_set(unit, &l2_learn_limit));
    /*
     * Send packet stream with more different mac addresses than requested
     */
    if (limit < prev_counter)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error: The number of MACT entries:%d is already over the limit:%d\n",
                     prev_counter, limit);
        /*
         * On failure leave without clean - to see what went wrong
         */
    }

    packets2send = limit - prev_counter + add_count;
    LOG_CLI((BSL_META("Sending %d packets with different SA\n"), packets2send));
    SHR_IF_ERR_EXIT(ctest_dnx_l2_send_packet_stream(unit, port1, vlan, mac_addr, mac_addr_2, packets2send));
    /*
     * Obtain MACT count and compare it to the limit
     */
    if (type == BCM_L2_LEARN_LIMIT_PORT)
    {
        SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_inlif_count(unit, gport1, &result_counter));
    }
    else if (type == BCM_L2_LEARN_LIMIT_VLAN)
    {
        SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_vsi_count(unit, vsi, &result_counter));
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_count(unit, &result_counter));
    }

    if (result_counter != limit)
    {
        LOG_CLI((BSL_META("Error: The number of MACT entries:%d vs limit:%d\n"), result_counter, limit));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        /*
         * On failure leave without clean - to see what went wrong
         */
        SHR_EXIT();
    }
    else
    {
        LOG_CLI((BSL_META("Success: The number of MACT entries:%d fits the limit:%d\n"), result_counter, limit));
    }

    if (clean == TRUE)
    {
        /*
         * Clean the environment - set the previous limit back
         */
        l2_learn_limit.limit = cur_limit;
        SHR_IF_ERR_EXIT(bcm_l2_learn_limit_set(unit, &l2_learn_limit));
        LOG_CLI((BSL_META("Limit of %d successfully enforced, setting back to %d\n"), limit, cur_limit));
        /*
         * Clear the table to allow next test to run
         */
        SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));
    }

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

static sh_sand_invoke_t dnx_l2_forward_tests[] = {
    {"SVL", "vsi=222", CTEST_POSTCOMMIT},
    {"IVL", "vsi=333 ivl", CTEST_POSTCOMMIT},
    {NULL}
};

static sh_sand_man_t dnx_l2_forward_man = {
    "L2 Forward Tests - IVL & SVL",
};

static sh_sand_option_t dnx_l2_forward_arguments[] = {
    {"port1", SAL_FIELD_TYPE_INT32, "1st port to be used in test", "200"},
    {"port2", SAL_FIELD_TYPE_INT32, "2nd port to be used in test", "201"},
    {"clean", SAL_FIELD_TYPE_BOOL, "Flag to enforce clearing the environment after the test", "No"},
    {"vsi", SAL_FIELD_TYPE_INT32, "VSI ID to be established", "100"},
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN ID. If 0 VLAN will be (vsi + 1)", "0", NULL, "0-4095"},
    {"mac1", SAL_FIELD_TYPE_MAC, "MAC Address to be used as first one in generated set", "00:01:02:04:08:00"},
    {"mac2", SAL_FIELD_TYPE_MAC, "MAC Address to be used as first one in generated set", "00:03:06:02:04:00"},
    {"ivl", SAL_FIELD_TYPE_BOOL, "IVL Flag, if it TRUE VSI is created in IVL mode, otherwise in SVL", "No"},
    {"flood", SAL_FIELD_TYPE_BOOL,
     "Flooding Flag, if it TRUE MC Group is created and associated with VSI for unknown DA", "No"},
    {NULL}
};

static shr_error_e
dnx_l2_forward_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int vlan, vsi;
    int ivl_flag, flooding_flag;
    bcm_port_t port1, port2;
    int core1, core2;
    sal_mac_addr_t mac_addr1, mac_addr2;
    bcm_gport_t gport1 = 0, gport2 = 0;
    uint32 tm_port;
    char return_value[DSIG_MAX_SIZE_STR];
    bcm_multicast_t mc_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port1", port1);
    SH_SAND_GET_INT32("port2", port2);
    SH_SAND_GET_INT32("vsi", vsi);
    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_MAC("mac1", mac_addr1);
    SH_SAND_GET_MAC("mac2", mac_addr2);
    SH_SAND_GET_BOOL("ivl", ivl_flag);
    SH_SAND_GET_BOOL("flood", flooding_flag);

    if (vlan == 0)
    {   /** To distinguish between vlan based and vsi based objects */
        vlan = vsi + 1;
    }

    SHR_IF_ERR_EXIT(config_dnx_l2_vsi_create(unit, port1, port2, vlan, vsi, &gport1, &gport2));

    if (ivl_flag == TRUE)
    {
        SHR_IF_ERR_EXIT(config_dnx_l2_vsi_set_ivl(unit, vsi));
    }
    if (flooding_flag == TRUE)
    {
        mc_id = vsi + 10;
        SHR_IF_ERR_EXIT(config_dnx_l2_vsi_set_mc(unit, port1, port2, vsi, mc_id));
    }

    dnx_visibility_resume(unit, BCM_CORE_ALL,
                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
    /*
     * These 2 packets are to establish entries
     */
    SHR_IF_ERR_EXIT(ctest_dnx_l2_send_packet_stream(unit, port1, vlan, mac_addr1, mac_addr2, 1));
    SHR_IF_ERR_EXIT(ctest_dnx_l2_send_packet_stream(unit, port2, vlan, mac_addr2, mac_addr1, 1));

    /*
     * No we can send packets to see how they are going through
     */
    SHR_IF_ERR_EXIT(ctest_dnx_l2_send_packet_stream(unit, port2, vlan, mac_addr2, mac_addr1, 1));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port1, &core1, &tm_port));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, core1, "ETPP", "PRP2", "", "Out_TM_Port", (uint32 *) &port1, 1, NULL, return_value, 0));

    dnx_visibility_resume(unit, BCM_CORE_ALL,
                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);

    SHR_IF_ERR_EXIT(ctest_dnx_l2_send_packet_stream(unit, port1, vlan, mac_addr1, mac_addr2, 1));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port2, &core2, &tm_port));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, core2, "ETPP", "PRP2", "", "Out_TM_Port", (uint32 *) &port2, 1, NULL, return_value, 0));

    SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));

    LOG_CLI((BSL_META("L2 Forward Test Completed Successfully between ports:%d - %d vsi:%d\n"), port1, port2, vsi));

exit:
    diag_sand_rx_clean(unit);
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_l2_flood_arguments[] = {
    {"port1", SAL_FIELD_TYPE_INT32, "1st port to be used in test", "200"},
    {"port2", SAL_FIELD_TYPE_INT32, "2nd port to be used in test", "201"},
    {"clean", SAL_FIELD_TYPE_BOOL, "Flag to enforce clearing the environment after the test", "No"},
    {"vsi", SAL_FIELD_TYPE_INT32, "VSI ID to be established", "100"},
    {"vlan", SAL_FIELD_TYPE_UINT32, "VLAN ID. If 0 VLAN will be (vsi + 1)", "0", NULL, "0-4095"},
    {"mac1", SAL_FIELD_TYPE_MAC, "MAC Address to be used as first one in generated set", "00:01:02:04:08:00"},
    {"mac2", SAL_FIELD_TYPE_MAC, "MAC Address to be used as first one in generated set", "00:03:06:02:04:00"},
    {"ivl", SAL_FIELD_TYPE_BOOL, "IVL Flag, if it TRUE VSI is created in IVL mode, otherwise in SVL", "No"},
    {"mc_id", SAL_FIELD_TYPE_INT32, "Multicast ID. If 0 It will be (vsi + 10)", "0", NULL, "0-4095"},
    {NULL}
};

static sh_sand_man_t dnx_l2_flood_man = {
    "L2 Flood Tests - IVL & SVL",
};

static sh_sand_invoke_t dnx_l2_flood_tests[] = {
    {"SVL", "vsi=444", CTEST_POSTCOMMIT},
    {"IVL", "vsi=555 ivl", CTEST_POSTCOMMIT},
    {NULL}
};

static shr_error_e
dnx_l2_flood_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int vsi;
    uint32 vlan;
    int ivl_flag;
    bcm_port_t port1, port2;
    int core1, core2;
    sal_mac_addr_t mac_addr1, mac_addr2;
    bcm_gport_t gport1 = 0, gport2 = 0;
    uint32 tm_port;
    char return_value[DSIG_MAX_SIZE_STR];
    bcm_multicast_t mc_id = 0;
    int rx_pkt_count;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port1", port1);
    SH_SAND_GET_INT32("port2", port2);
    SH_SAND_GET_INT32("vsi", vsi);
    SH_SAND_GET_UINT32("vlan", vlan);
    SH_SAND_GET_MAC("mac1", mac_addr1);
    SH_SAND_GET_MAC("mac2", mac_addr2);
    SH_SAND_GET_BOOL("ivl", ivl_flag);
    SH_SAND_GET_INT32("mc_id", mc_id);

    if (vlan == 0)
    {   /** To distinguish between vlan based and vsi based objects */
        vlan = vsi + 1;
    }

    SHR_IF_ERR_EXIT(config_dnx_l2_vsi_create(unit, port1, port2, vlan, vsi, &gport1, &gport2));

    if (ivl_flag == TRUE)
    {
        SHR_IF_ERR_EXIT(config_dnx_l2_vsi_set_ivl(unit, vsi));
    }

    if (mc_id == 0)
    {
        mc_id = vsi + 10;
    }
    SHR_IF_ERR_EXIT(config_dnx_l2_vsi_set_mc(unit, port1, port2, vsi, mc_id));

    dnx_visibility_resume(unit, BCM_CORE_ALL,
                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
    /*
     * Now we can send packets to see how they are going through
     * TWe send 1 packet and w'll be expecting on each core to see non zero output port
     */
    SHR_IF_ERR_EXIT(ctest_dnx_l2_send_packet_stream(unit, port1, vlan, mac_addr1, mac_addr2, 1));

    if (DNXCMN_CHIP_IS_MULTI_CORE(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port1, &core1, &tm_port));
        SHR_IF_ERR_EXIT(sand_signal_verify
                        (unit, core1, "ETPP", "PRP2", "", "Out_TM_Port", (uint32 *) &port1, 1, NULL, return_value, 0));

        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port2, &core2, &tm_port));
        SHR_IF_ERR_EXIT(sand_signal_verify
                        (unit, core2, "ETPP", "PRP2", "", "Out_TM_Port", (uint32 *) &port2, 1, NULL, return_value, 0));
    }

    SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));

    if ((rx_pkt_count = diag_sand_rx_nof(unit)) != 2)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Number of actually received packets is:%d, should be 2\n", rx_pkt_count);
    }
    LOG_CLI((BSL_META("L2 Flood Test Completed Successfully between ports:%d - %d vsi:%d\n"), port1, port2, vsi));

    {
        rhhandle_t packet_h = NULL;
        int match_count;
        char mac_str[RHNAME_MAX_SIZE];

        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
        sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                     mac_addr2[0], mac_addr2[1], mac_addr2[2], mac_addr2[3], mac_addr2[4], mac_addr2[5]);
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));

        SHR_IF_ERR_EXIT(diag_sand_rx_compare(unit, packet_h, &match_count));
        if (match_count == 0)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "No packet received with expected fields\n");
        }
    }
exit:
    diag_sand_rx_clean(unit);
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_l2_aging_arguments[] = {
    {"port1", SAL_FIELD_TYPE_INT32, "1st port to be used in test", "200"},
    {"port2", SAL_FIELD_TYPE_INT32, "2nd port to be used in test", "201"},
    {"clean", SAL_FIELD_TYPE_BOOL, "Flag to enforce clearing the environment after the test", "No"},
    {"vsi", SAL_FIELD_TYPE_INT32, "VSI ID to be established", "100"},
    {"vlan", SAL_FIELD_TYPE_INT32, "VLAN ID. If 0 VLAN will be (vsi + 1)", "0", NULL, "0-4095"},
    {"mac1", SAL_FIELD_TYPE_MAC, "MAC Address to be used as first one in generated set", "00:01:02:04:08:00"},
    {"mac2", SAL_FIELD_TYPE_MAC, "MAC Address to be used as first one in generated set", "00:03:06:02:04:00"},
    {"ivl", SAL_FIELD_TYPE_BOOL, "IVL Flag, if it TRUE VSI is created in IVL mode, otherwise in SVL", "No"},
    {"flood", SAL_FIELD_TYPE_BOOL,
     "Flooding Flag, if it TRUE MC Group is created and associated with VSI for unknown DA", "No"},
    {"cycle", SAL_FIELD_TYPE_INT32, "Cycle Time(in seconds)", "1"},
    {"count", SAL_FIELD_TYPE_INT32, "Cycle Number", "8"},
    {"tx", SAL_FIELD_TYPE_BOOL, "Send reverse packets to verify learn enrtreis existence", "No"},
    {NULL}
};

static sh_sand_option_t dnx_l2_flush_arguments[] = {
    {"vsi", SAL_FIELD_TYPE_INT32, "VSI ID to be deleted", "0"},
    {"count", SAL_FIELD_TYPE_INT32, "Number of MAC entries to add", "1"},
    {"is_static", SAL_FIELD_TYPE_INT32, "Strength of the entries to add", "1"},
    {"non_blocking", SAL_FIELD_TYPE_INT32, "Set flush machine without waiting for it finish", "0"},
    {NULL}
};

static sh_sand_man_t dnx_l2_aging_man = {
    "L2 Aging Tests - IVL & SVL",
};

static sh_sand_invoke_t dnx_l2_aging_tests[] = {
    {"SVL", "vsi=666", CTEST_POSTCOMMIT},
    {"IVL", "vsi=777 ivl", CTEST_POSTCOMMIT},
    {NULL}
};

static sh_sand_invoke_t dnx_l2_flush_tests[] = {
    {"vsi", "vsi=1", CTEST_POSTCOMMIT},
    {"count", "count=1", CTEST_POSTCOMMIT},
    {"is_static", "is_static=1", CTEST_POSTCOMMIT},
    {"blocking", "non_blocking=0", CTEST_POSTCOMMIT},
    {"non_blocking", "vsi=100 count=15 is_static=0 non_blocking=1", CTEST_POSTCOMMIT},
    {NULL}
};

static sh_sand_man_t dnx_l2_flush_man = {
    "L2 Flush Tests",
};

static shr_error_e
dnx_l2_aging_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int vlan, vsi;
    int ivl_flag, flooding_flag, tx_flag;
    bcm_port_t port1, port2;
    int core1;
    sal_mac_addr_t mac_addr1, mac_addr2, mac_addr3 = { 11, 22, 33, 44, 55, 66 };
    bcm_gport_t gport1 = 0, gport2 = 0;
    uint32 tm_port;
    char return_value[DSIG_MAX_SIZE_STR];
    bcm_multicast_t mc_id = 0;
    int cycle_time, cycle_num, global_cycle_time;
    bcm_vlan_control_vlan_t control;
    int i_cycle;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port1", port1);
    SH_SAND_GET_INT32("port2", port2);
    SH_SAND_GET_INT32("vsi", vsi);
    SH_SAND_GET_INT32("vlan", vlan);
    SH_SAND_GET_MAC("mac1", mac_addr1);
    SH_SAND_GET_MAC("mac2", mac_addr2);
    SH_SAND_GET_BOOL("ivl", ivl_flag);
    SH_SAND_GET_BOOL("flood", flooding_flag);
    SH_SAND_GET_INT32("cycle", cycle_time);
    SH_SAND_GET_INT32("count", cycle_num);
    SH_SAND_GET_BOOL("tx", tx_flag);

    if (vlan == 0)
    {   /** To distinguish between vlan based and vsi based objects */
        vlan = vsi + 1;
    }

    SHR_IF_ERR_EXIT(config_dnx_l2_vsi_create(unit, port1, port2, vlan, vsi, &gport1, &gport2));

    if (ivl_flag == TRUE)
    {
        SHR_IF_ERR_EXIT(config_dnx_l2_vsi_set_ivl(unit, vsi));
    }

    if (flooding_flag == TRUE)
    {
        mc_id = vsi + 10;
        SHR_IF_ERR_EXIT(config_dnx_l2_vsi_set_mc(unit, port1, port2, vsi, mc_id));
    }
    /*
     * Set aging parameters
     */
    SHR_IF_ERR_EXIT(config_dnx_l2_vsi_set_aging_cycles(unit, vsi, cycle_num));
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_set(unit, cycle_time));
    /*
     * Get aging parameters back
     */
    SHR_IF_ERR_EXIT(bcm_vlan_control_vlan_get(unit, vsi, &control));
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_get(unit, &global_cycle_time));
    /*
     * Verify that there were set successfully
     */
    if (cycle_time != global_cycle_time)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Set cycle time:%d is not equal to get:%d\n", cycle_time, global_cycle_time);
    }
    if (cycle_num != control.aging_cycles)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Set cycle number:%d is not equal to get:%d\n", cycle_num, control.aging_cycles);
    }
    LOG_CLI((BSL_META("Aging time for VSI:%d is %d sec (%d cycles * %d sec cycle time)\n"),
             vsi, control.aging_cycles * global_cycle_time, control.aging_cycles, global_cycle_time));

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port1, &core1, &tm_port));
    /*
     * Send packet that will insert needed entry into MACT
     */
    SHR_IF_ERR_EXIT(ctest_dnx_l2_send_packet_stream(unit, port1, vlan, mac_addr3, mac_addr2, 1));
    for (i_cycle = 0; i_cycle < control.aging_cycles + 1;)
    {
        uint32 vsi_counter, global_counter, expected_counter;
        int rx_nof = 0;

        sal_sleep(global_cycle_time);
        i_cycle++;

        if (tx_flag == TRUE)
        {
            /*
             * Send packet that is supposed to have a hit for its DA in MACT and make successful exit
             */
            dnx_visibility_resume(unit, BCM_CORE_ALL,
                                  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
            SHR_IF_ERR_EXIT(ctest_dnx_l2_send_packet_stream(unit, port2, vlan, mac_addr2, mac_addr1, 1));
            rx_nof = diag_sand_rx_nof(unit);
        }
        SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_vsi_count(unit, vsi, &vsi_counter));
        SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_count(unit, &global_counter));
        if (tx_flag == TRUE)
        {
            if (rx_nof != 0)
            {
                SHR_IF_ERR_EXIT(sand_signal_verify
                                (unit, core1, "ETPP", "PRP2", "", "Out_TM_Port", (uint32 *) &port1, 1, NULL,
                                 return_value, 0));
            }
            LOG_CLI((BSL_META("Waiting %d seconds - MACT Counter:%d VSI Counter:%d Rx:%d\n"),
                     i_cycle * cycle_time, global_counter, vsi_counter, rx_nof));
            expected_counter = 1;
        }
        else
        {
            LOG_CLI((BSL_META("Waiting %d seconds - MACT Counter:%d VSI Counter:%d\n"),
                     i_cycle * cycle_time, global_counter, vsi_counter));
            expected_counter = 0;
        }

        if ((global_counter == expected_counter) && (vsi_counter == expected_counter) && (rx_nof == 0))
        {
            LOG_CLI((BSL_META("L2 Entry aged\n")));
            break;
        }
        else
        {
            LOG_CLI((BSL_META("L2 Entry not aged\n")));
        }
    }
    /*
     * We have been waiting for proper time plus some surplus - 1 sec, if more we'll consider it as failure
     */
    if (i_cycle != control.aging_cycles)
    {
        LOG_CLI((BSL_META("L2 Refresh Test Failed after maximum:%d seconds\n"), i_cycle * cycle_time));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
    }
    else
    {
        LOG_CLI((BSL_META("L2 Refresh Test Completed Successfully with aging time:%d seconds\n"),
                 i_cycle * cycle_time));
    }

exit:
    diag_sand_rx_clean(unit);
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_l2_vsi_performance_man = {
    "Test VSI related to time performance of the following APIs:\n - bcm_vlan_create\n - bcm_stg_vlan_add\n - bcm_mpls_vpn_id_create",
};

/**
 * Test the performance of VSI-related APIs.
 */
static shr_error_e
sh_dnx_l2_vsi_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int entry_id;
    int rv = BCM_E_NONE;
    int is_active;
    bcm_mpls_vpn_config_t info;
    uint32 nof_entries = 1000;
    uint32 hits = 0;
    uint32 timer_idx[3] = { 1, 2, 3 };
    uint32 total_time[3];
    uint32 timers_group;
    
    uint32 average_time[3] = { 315, 6000, 300 };
    bcm_stg_t stg;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    char *name;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(group_name, 0, sizeof(char) * UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME);
    sal_strncpy(group_name, "L2 VSI performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    /** bcm_vlan_create */
    for (entry_id = 0; entry_id < nof_entries; entry_id++)
    {
        utilex_ll_timer_set("bcm_vlan_create", timers_group, timer_idx[0]);
        rv = bcm_vlan_create(unit, entry_id + 2);
        utilex_ll_timer_stop(timers_group, timer_idx[0]);
    }

    /** bcm_stg_vlan_add */
    rv = bcm_stg_create(unit, &stg);
    for (entry_id = 0; entry_id < nof_entries; entry_id++)
    {
        utilex_ll_timer_set("bcm_stg_vlan_add", timers_group, timer_idx[1]);
        rv = bcm_stg_vlan_add(unit, stg, entry_id + 2);
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
    }

    /** bcm_mpls_vpn_id_create */
    for (entry_id = 0; entry_id < nof_entries; entry_id++)
    {
        bcm_mpls_vpn_config_t_init(&info);
        info.vpn = entry_id + 2;
        info.flags = BCM_MPLS_VPN_VPLS | BCM_MPLS_VPN_WITH_ID;
        info.broadcast_group = entry_id + 2;
        info.unknown_multicast_group = entry_id + 2;
        info.unknown_unicast_group = entry_id + 2;
        utilex_ll_timer_set("bcm_mpls_vpn_id_create", timers_group, timer_idx[2]);
        rv = bcm_mpls_vpn_id_create(unit, &info);

        utilex_ll_timer_stop(timers_group, timer_idx[2]);
    }

    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[0], &is_active, &name, &hits, &total_time[0]));
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[1], &is_active, &name, &hits, &total_time[1]));
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[2], &is_active, &name, &hits, &total_time[2]));

    printf("_______________________________________________________\n\n");
    printf("AVERAGE time for bcm_vlan_create - %u us, expected approximately %u\n",
           (total_time[0] / nof_entries), average_time[0]);
    printf("AVERAGE time for bcm_stg_vlan_add - %u us, expected approximately %u\n",
           (total_time[1] / nof_entries), average_time[1]);
    printf("AVERAGE time for bcm_mpls_vpn_id_create - %u us, expected approximately %u\n",
           (total_time[2] / nof_entries), average_time[2]);
    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);
    /** Verify that the average execution time for adding a single entry is within limits. */
#if defined(ADAPTER_SERVER_MODE)
    /** Skip comparing with average expected time for cmodel execution. */
#else
    if ((total_time[0] / nof_entries) > (average_time[0] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Performance of bcm_vlan_create is out of limits - value should be less than %f\n",
                     (average_time[0] * 1.1));
    }
    if ((total_time[1] / nof_entries) > (average_time[1] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Performance of bcm_stg_vlan_add is out of limits - value should be less than %f\n",
                     (average_time[1] * 1.1));
    }
    if ((total_time[2] / nof_entries) > (average_time[2] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Performance of bcm_mpls_vpn_id_create is out of limits - value should be less than %f\n",
                     (average_time[2] * 1.1));
    }
#endif
    /** clean-up functions */
    rv = bcm_stg_vlan_remove_all(unit, stg);
    rv = bcm_stg_destroy(unit, stg);
    rv = bcm_mpls_vpn_id_destroy_all(unit);
    rv = bcm_vlan_destroy_all(unit);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 intf CREATE TIME PERFORMANCE TEST END.\n")));
exit:
    rv = utilex_ll_timer_group_free(timers_group);
    SHR_FUNC_EXIT;
}
/*
 * \brief
 * Basic bridge ctest
 * Verify L2 Basic Bridge functionality
 * Validate signals
 */
static shr_error_e
sh_dnx_l2_basic_bridge_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 masked_out_port;
    uint32 vid = 5;
    uint32 vsi = 10;
    uint32 flags;
    uint32 signal_values[] = { 0x2, 0x14, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    bcm_mac_t dmac_addr = { 0x00, 0x00, 0x00, 0x00, 0x12, 0x34 };
    bcm_port_t in_port = 200;
    bcm_port_t out_port = 202;
    bcm_l2_addr_t l2addr;
    bcm_vlan_port_t vlan_port;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t untag_pbmp;
    rhhandle_t packet_tx = NULL;
    rhhandle_t packet_rx = NULL;
    int match_count;
    int eth_layer_protocol;
    int physical_port_type;
    int forward_code_eth;
    int pph_type;
    int type_vsi;
    int c_tag_val;
    int system_headers_mode;
    char return_value[DSIG_MAX_SIZE_STR];
    char *smac = "00:00:00:00:00:01";
    char *dmac = "00:00:00:00:12:34";
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, in_port, bcmPortClassId, in_port));
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, out_port, bcmPortClassId, out_port));

    /**Create VSI*/
    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vsi));

    /** Set vlan port membership*/
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);
    BCM_PBMP_PORT_ADD(pbmp, out_port);
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vsi, pbmp, untag_pbmp));

    /**Create InLif*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vsi;
    vlan_port.port = in_port;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    bcm_l2_addr_t_init(&l2addr, dmac_addr, vsi);
    l2addr.port = out_port;
    l2addr.flags = BCM_L2_STATIC;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid, in_port, 0));

    dnx_visibility_resume(unit, BCM_CORE_ALL,
                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);

    /** Allocate and init packet_tx.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info));
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_tx));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "ETH1.DA", dmac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "ETH1.SA", smac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_tx, "ETH1.VLAN.VID", &vid, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_tx, "IPv4"));
    /** Send packet.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, in_port, packet_tx, SAND_PACKET_RX));
    SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));
    /** Check the received packet.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_rx));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_rx, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "ETH1.DA", dmac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_rx, "ETH1.SA", smac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_rx, "ETH1.VLAN.VID", &vid, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_rx, "IPv4"));

    SHR_IF_ERR_EXIT(diag_sand_rx_compare(unit, packet_rx, &match_count));
    if (match_count == 0)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "No packet received with expected fields\n");
    }

    dnx_dbal_fields_enum_value_get(unit, "LAYER_TYPES", "ETHERNET", &eth_layer_protocol);
    dnx_dbal_fields_enum_value_get(unit, "EGRESS_FWD_CODE", "ETHERNET", &forward_code_eth);
    dnx_dbal_fields_enum_value_get(unit, "PPH_TYPE", "PPH_BASE_PLUS_TS", &pph_type);
    dnx_dbal_fields_enum_value_get(unit, "FODO_ENCODING", "VSI", &type_vsi);
    dnx_dbal_fields_enum_value_get(unit, "DESTINATION_ENCODING", "PORT_ID", &physical_port_type);
    dnx_dbal_fields_enum_value_get(unit, "INCOMING_TAG_STRUCTURE", "C_TAG_1", &c_tag_val);

    /**Validate signals*/

    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "Layer_Offsets.0", &signal_values[0], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "Layer_Offsets.1", &signal_values[1], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "Layer_Protocols.0", (uint32 *) &eth_layer_protocol,
                     1, NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "Layer_Qualifiers.1", &signal_values[2], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT5", "", "In_Port", &mapping_info.pp_port, 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "LLR", "", "Incoming_Tag_Exist", &signal_values[3], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "LLR", "", "Initial_VID", &vid, 1, NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "LLR", "", "Vlan_Domain", (uint32 *) &in_port, 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "Fwd_Domain_ID.type", (uint32 *) &type_vsi, 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "Fwd_Domain_ID.value", &vsi, 1, NULL, return_value,
                     0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "Incoming_Tag_Structure", (uint32 *) &c_tag_val, 1,
                     NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "VLAN_Edit_Cmd", &signal_values[4], 39, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "VTT1", "", "Fwd_Layer_Index", &signal_values[5], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "FWD2", "", "Fwd_Action_Dst.Type", (uint32 *) &physical_port_type,
                     1, NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "FWD2", "", "Fwd_Action_Dst.Value", (uint32 *) &out_port, 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "FWD2", "", "Out_LIFs", &signal_values[6], 44, NULL, return_value,
                     0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "FER", "", "Fwd_Action_Dst.Type", (uint32 *) &physical_port_type,
                     1, NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "IRPP", "FER", "", "Fwd_Action_Dst.Value", (uint32 *) &out_port, 1, NULL,
                     return_value, 0));

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(sand_signal_verify
                        (unit, mapping_info.core, "ERPP", "Fabric", "",
                         "FTMH_Base.FTMH_Multicast_ID_or_MC_REP_IDX_or_OutLIF_0", &signal_values[7], 1, NULL,
                         return_value, 0));
        masked_out_port = (out_port & 0xff);
        SHR_IF_ERR_EXIT(sand_signal_verify
                        (unit, mapping_info.core, "ERPP", "Fabric", "", "FTMH_Base.FTMH_PP_DSP", &masked_out_port, 1,
                         NULL, return_value, 0));
    }
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "PRP2", "", "Fabric_or_Egress_MC", &signal_values[8], 1, NULL,
                     return_value, 0));

    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "PRP2", "", "Fwd_Code", (uint32 *) &forward_code_eth, 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "PRP2", "", "EPP_Layer_Protocols.0",
                     (uint32 *) &eth_layer_protocol, 1, NULL, return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "PRP2", "", "EPP_Layer_Offsets.0", &signal_values[9], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "Term", "", "ETPS_Valid.1", &signal_values[10], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "Term", "", "ETPS_Valid.2", &signal_values[11], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "Term", "", "ETPS_Valid.3", &signal_values[12], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "Term", "", "ETPS_Valid.4", &signal_values[13], 1, NULL,
                     return_value, 0));
    SHR_IF_ERR_EXIT(sand_signal_verify
                    (unit, mapping_info.core, "ETPP", "FWD", "", "Current_Protocol_Type",
                     (uint32 *) &eth_layer_protocol, 1, NULL, return_value, 0));

    /*
     * Cleanup
     */

    SHR_CLI_EXIT_IF_ERR(bcm_vlan_destroy_all(unit), "Test failed. Could not delete vlan_port\n");
    SHR_CLI_EXIT_IF_ERR(bcm_l2_addr_delete(unit, dmac_addr, vsi),
                        "Test failed. Could not delete an L2 address entry\n");
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_gport_delete(unit, vid, in_port),
                        "Test failed. Could not delete an L2 address entry\n");
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Basic bridge TEST END.\nResult: PASS!\n")));
exit:

    diag_sand_packet_free(unit, packet_tx);
    diag_sand_packet_free(unit, packet_rx);
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_l2_start_man = {
    "Start L2 Timers",
};

sh_sand_man_t dnx_l2_stop_man = {
    "Stop L2 Timers and print the result",
};

sh_sand_man_t dnx_l2_vsi_man = {
    "Test VSI related APIs",
};

sh_sand_man_t dnx_l2_basic_bridge_man = {
    "Basic bridge configuration",
};

uint32 l2_timers_group;

/**
* \brief
* Initiate L2 timers
*/
static shr_error_e
dnx_l2_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("L2 Time measurements", &l2_timers_group));
    SHR_IF_ERR_EXIT(utilex_ll_timer_clear_all(l2_timers_group));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Stop L2 timers and show the results
*/
static shr_error_e
dnx_l2_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    utilex_ll_timer_print_all(l2_timers_group);
    utilex_ll_timer_clear_all(l2_timers_group);

    utilex_ll_timer_group_free(l2_timers_group);

    SHR_FUNC_EXIT;
}

sh_sand_cmd_t dnx_l2_vsi_test_cmds[] = {
    {"performance", sh_dnx_l2_vsi_performance_cmd, NULL, NULL, &sh_dnx_l2_vsi_performance_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {NULL}
};

/** List of vlan tests   */
/* *INDENT-OFF* */
sh_sand_cmd_t dnx_l2_test_cmds[] = {
    {"table",       dnx_l2_mact_cmd,            NULL,                 dnx_l2_mact_arguments,    &dnx_l2_mact_man,        NULL, dnx_l2_mact_tests,    CTEST_POSTCOMMIT},
    {"limit",       dnx_l2_limit_cmd,           NULL,                 dnx_l2_limit_arguments,   &dnx_l2_limit_man,       NULL, dnx_l2_limit_tests,   CTEST_POSTCOMMIT | SH_CMD_CONDITIONAL, sh_cmd_is_device},
    {"forward",     dnx_l2_forward_cmd,         NULL,                 dnx_l2_forward_arguments, &dnx_l2_forward_man,     NULL, dnx_l2_forward_tests, CTEST_POSTCOMMIT | SH_CMD_ROLLBACK | SH_CMD_CONDITIONAL, sh_cmd_is_device},
    {"flood",       dnx_l2_flood_cmd,           NULL,                 dnx_l2_flood_arguments,   &dnx_l2_flood_man,       NULL, dnx_l2_flood_tests,   CTEST_POSTCOMMIT | SH_CMD_ROLLBACK | SH_CMD_CONDITIONAL, sh_cmd_is_device},
    {"aging",       dnx_l2_aging_cmd,           NULL,                 dnx_l2_aging_arguments,   &dnx_l2_aging_man,       NULL, dnx_l2_aging_tests,   CTEST_POSTCOMMIT | SH_CMD_ROLLBACK | SH_CMD_CONDITIONAL, sh_cmd_is_device},
    {"flush",       dnx_l2_flush_cmd,           NULL,                 dnx_l2_flush_arguments,   &dnx_l2_flush_man,       NULL, dnx_l2_flush_tests,   CTEST_POSTCOMMIT | SH_CMD_ROLLBACK | SH_CMD_CONDITIONAL, sh_cmd_is_device},
    {"start",       dnx_l2_start_cmd,           NULL,                 NULL,                     &dnx_l2_start_man,       NULL, NULL,                 SH_CMD_SKIP_EXEC},
    {"stop",        dnx_l2_stop_cmd,            NULL,                 NULL,                     &dnx_l2_stop_man,        NULL, NULL,                 SH_CMD_SKIP_EXEC},
    {"vsi",         NULL,                       dnx_l2_vsi_test_cmds, NULL,                     &dnx_l2_vsi_man},
    {"basic_bridge",sh_dnx_l2_basic_bridge_cmd, NULL,                 NULL,                     &dnx_l2_basic_bridge_man,NULL, NULL,                 CTEST_POSTCOMMIT | SH_CMD_ROLLBACK },
    {NULL}
};
/* *INDENT-ON* */
