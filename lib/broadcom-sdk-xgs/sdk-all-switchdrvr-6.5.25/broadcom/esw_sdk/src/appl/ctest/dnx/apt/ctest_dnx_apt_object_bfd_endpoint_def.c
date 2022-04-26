/**
 * \file ctest_dnx_apt_object_bfd_endpoint_def.c
 * Contains the functions and definitions for BFD MEP API performance test objects.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
#include <shared/bsl.h>

/*
 * Include files.
 * {
 */

#include "ctest_dnx_apt.h"
#include <bcm/port.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/vlan.h>
#include <bcm/vswitch.h>
#include <bcm/bfd.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS                 10
#define DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS                1000

#define DNX_APT_BFD_ENDPOINT_UDP_IPV4_SHOP_ACC_INSERT_RATE          2500
#define DNX_APT_BFD_ENDPOINT_UDP_IPV6_SHOP_ACC_INSERT_RATE          1100
#define DNX_APT_BFD_ENDPOINT_MPLS_TP_CC_ACC_INSERT_RATE             2550
#define DNX_APT_BFD_ENDPOINT_MPLS_IPV4_SHOP_ACC_INSERT_RATE         2500
#define DNX_APT_BFD_ENDPOINT_PWE_CONTROL_WORD_ACC_INSERT_RATE       2300
#define DNX_APT_BFD_ENDPOINT_PWE_ROUTER_ALTER_ACC_INSERT_RATE       2300
#define DNX_APT_BFD_ENDPOINT_PWE_TTL_ACC_INSERT_RATE                2300
#define DNX_APT_BFD_ENDPOINT_PWE_GAL_ACC_INSERT_RATE                2300

#define DNX_APT_BFD_ENDPOINT_UDP_IPV4_SHOP_ACC_UPDATE_RATE          1360
#define DNX_APT_BFD_ENDPOINT_UDP_IPV6_SHOP_ACC_UPDATE_RATE          285
#define DNX_APT_BFD_ENDPOINT_MPLS_TP_CC_ACC_UPDATE_RATE             1120
#define DNX_APT_BFD_ENDPOINT_MPLS_IPV4_SHOP_ACC_UPDATE_RATE         1120
#define DNX_APT_BFD_ENDPOINT_PWE_CONTROL_WORD_ACC_UPDATE_RATE       1120
#define DNX_APT_BFD_ENDPOINT_PWE_ROUTER_ALTER_ACC_UPDATE_RATE       1120
#define DNX_APT_BFD_ENDPOINT_PWE_TTL_ACC_UPDATE_RATE                1120
#define DNX_APT_BFD_ENDPOINT_PWE_GAL_ACC_UPDATE_RATE                1120

#define DNX_APT_BFD_ENDPOINT_UDP_IPV4_SHOP_ACC_DESTROY_RATE         1650
#define DNX_APT_BFD_ENDPOINT_UDP_IPV6_SHOP_ACC_DESTROY_RATE         420
#define DNX_APT_BFD_ENDPOINT_MPLS_TP_CC_ACC_DESTROY_RATE            1380
#define DNX_APT_BFD_ENDPOINT_MPLS_IPV4_SHOP_ACC_DESTROY_RATE        1380
#define DNX_APT_BFD_ENDPOINT_PWE_CONTROL_WORD_ACC_DESTROY_RATE      1360
#define DNX_APT_BFD_ENDPOINT_PWE_ROUTER_ALTER_ACC_DESTROY_RATE      1360
#define DNX_APT_BFD_ENDPOINT_PWE_TTL_ACC_DESTROY_RATE               1360
#define DNX_APT_BFD_ENDPOINT_PWE_GAL_ACC_DESTROY_RATE               1360

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(_unit_,  _test_obj_, _test_type_) \
        DNX_APT_RATE_TO_TIME(DNX_APT_BFD_ENDPOINT_##_test_obj_##_##_test_type_##_RATE)

#define DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(_info_, _test_obj_, _test_type_) \
            sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus", \
                        DNX_APT_RATE_TO_TIME(DNX_APT_BFD_ENDPOINT_##_test_obj_##_##_test_type_##_RATE))

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the BFD Endpoint API performance tests */
typedef struct
{
    bcm_bfd_endpoint_info_t ep;
} dnx_apt_bfd_endpoint_info_t;

typedef struct
{
    /*
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
    uint8 oam_default_profile;

    /*
     * Output
     */
    int rif;
    uint8 skip_mymac;           /* If true, mymac will not be set. Make sure you set it elsewhere. */
} create_l3_intf_t;

/* Creating L3 egress */
typedef struct
{
    /*
     * Input
     */
    uint32 allocation_flags;    /* BCM_L3_XXX */
    uint32 l3_flags;            /* BCM_L3_XXX */
    uint32 l3_flags2;           /* BCM_L3_FLAGS2_XXX */

    /*
     * ARP
     */
    int vlan;                   /* Outgoing vlan-VSI, relevant for ARP creation. In case set then SA MAC address is
                                 * retreived from this VSI. */
    bcm_mac_t next_hop_mac_addr;        /* Next-hop MAC address, relevant for ARP creation */
    int qos_map_id;             /* General QOS map id */

    /*
     * FEC
     */
    bcm_if_t out_tunnel_or_rif; /* *Outgoing intf, can be tunnel/rif, relevant for FEC creation */
    bcm_gport_t out_gport;      /* *Outgoing port , relevant for FEC creation */
    bcm_failover_t failover_id; /* Failover Object Index. */
    bcm_if_t failover_if_id;    /* Failover Egress Object index. */

    /*
     * Input/Output ID allocation
     */
    bcm_if_t fec_id;            /* *FEC ID */
    bcm_if_t arp_encap_id;      /* *ARP ID, may need for allocation ID or for FEC creation */
} create_l3_egress_t;

typedef struct
{
    int flags;
    int flags_out;
    int flags_out_2;
    int flags_out_3;
    int force_flags;            /* Pass force_flags=1 for independent configuration; otherwise, flags will be
                                 * configured automatically */
    int force_flags_out;
    bcm_mpls_egress_action_t egress_action;
    bcm_mpls_label_t label_in;
    bcm_mpls_label_t label_out; /* 0 in case only one label is created */
    bcm_mpls_label_t label_out_2;       /* 0 in case only 1-2 labels are created */
    bcm_mpls_label_t label_out_3;       /* 0 in case only 1-3 labels are created */
    int next_pointer_intf;
    int tunnel_id;              /* out parameter, created tunnel id */
    bcm_failover_t egress_failover_id;
    bcm_if_t egress_failover_if_id;
    uint8 ttl;
    uint8 exp;
    uint8 ext_ttl;
    uint8 ext_exp;
    int with_exp;
    bcm_mpls_special_label_push_type_t spl_label_push_type;
    bcm_encap_access_t encap_access;
} mpls__egress_tunnel_utils_t;

typedef struct
{
    int ac_port;                /* incoming port from ac side */
    int pwe_port;               /* incoming port from pwe side */
    int pwe_intf;               /* pwe RIF */
    bcm_vpn_t vpn;              /* vsi id */
    bcm_gport_t mpls_port_id_ingress[DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS];
    /*
     * global lif encoded as MPLS port, used for ingress, must be as egress
     */
    bcm_gport_t mpls_port_id_egress[DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS];
    /*
     * global lif encoded as MPLS port, used for egress, must be as ingress
     */
    bcm_gport_t vlan_port_id;   /* vlan port id of the ac_port, used for ingress */
    bcm_if_t encap_id[DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS];
    /*
     * The global lif of the EEDB PWE entry.
     */
    int core_arp_id;            /* Id for ac ARP entry */
    int mpls_encap_fec_id;      /* ac fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    bcm_mac_t ac_intf_mac_address;      /* mac for ac RIF */
    bcm_mac_t ac_mac_address;   /* mac for ingress next hop */
    bcm_mac_t pwe_intf_mac_address;     /* mac for pwe RIF */
    bcm_mac_t pwe_arp_next_hop_mac;     /* mac for egress next hop */
    bcm_if_t mpls_tunnel_id;    /* tunnel id for encapsulation entry */
    bcm_mpls_label_t mpls_tunnel_label; /* pushed label */
    bcm_mpls_label_t pwe_label; /* pwe label */
    bcm_mpls_label_t pwe_termination_label;     /* label to be terminated */
    int cw_present;             /* indication to whether CW is present */
    bcm_if_t mpls_tunnel_id_ingress;    /* tunnel id for termination entry */
    int skip_auto_fec_allocation;       /* In case the FEC ID needs to be allocated outside of the cint */
} vpls_basic_info_t;

typedef enum dnx_apt_bfd_endpoint_test_obj_e
{
    dnxAPTBFDEndpointTestUdpIpv4ShopAcc = 0,
    dnxAPTBFDEndpointTestUdpIpv4MhopAcc = 1,
    dnxAPTBFDEndpointTestUdpIpv6ShopAcc = 2,
    dnxAPTBFDEndpointTestUdpIpv6MhopAcc = 3,
    dnxAPTBFDEndpointTestMplsIpv4ShopAcc = 4,
    dnxAPTBFDEndpointTestMplsIpv4MhopAcc = 5,
    dnxAPTBFDEndpointTestMplsIpv6ShopAcc = 6,
    dnxAPTBFDEndpointTestMplsIpv6MhopAcc = 7,
    dnxAPTBFDEndpointTestMplsTpCCAcc = 8,
    dnxAPTBFDEndpointTestPWEControlWordAcc = 9,
    dnxAPTBFDEndpointTestPWERouterAlertAcc = 10,
    dnxAPTBFDEndpointTestPWETtlAcc = 11,
    dnxAPTBFDEndpointTestPWEGalAcc = 12,
    dnxAPTBFDEndpointTestCount = 13     /* Always Last. Not a usable value. */
} dnx_apt_bfd_endpoint_test_obj_t;

/*
 * }
 */

/*
 * GLOBALs
 * {
 */
int port_1 = 200;
int port_2 = 201;
int ctest_bfd_egress_interface;
int tunnel_id = 99;
int mpls_label = 3000;
int mpls_label_termination = 2000;
bcm_mpls_tunnel_switch_t tunnel_switch[DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS];

vpls_basic_info_t dnx_apt_vpls_basic_info = {
    /*
     * ports : ac_port | pwe_port
     */
    200, 201,
    /*
     * pwe_intf
     */
    30,
    /*
     * vpn
     */
    5,
    /*
     * mpls_port_id_ingress |  mpls_port_id_egress | vlan_port_id
     */
    {9999}, {28888}, 0,
    /*
     * encap_id
     */
    {28888},
    /*
     * core_arp_id
     */
    29001,
    /*
     * mpls_encap_fec_id
     */
    50000,
    /*
     * ac_intf_mac_address |  ac_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x11, 0x00, 0x00, 0x01, 0x12},
    /*
     * pwe_intf_mac_address | pwe_arp_next_hop_mac
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * mpls_tunnel_id
     */
    16388,
    /*
     * mpls_tunnel_label , pwe_label
     */
    3333, 3456,
    /*
     * pwe_termination_label
     */
    3333,
    /*
     * cw_present
     */
    0,
    /*
     * mpls_tunnel_id_ingress
     */
    0,
    /*
     * skip_auto_fec_allocation
     */
    0
};

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

int
get_fec_range(
    int unit,
    int hierarchy,
    int *start,
    int *nof_fecs)
{
    int rv;
    bcm_switch_fec_property_config_t fec_config;

    if (hierarchy == 0)
    {
        fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    }
    else if (hierarchy == 1)
    {
        fec_config.flags = BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY;
    }
    else if (hierarchy == 2)
    {
        fec_config.flags = BCM_SWITCH_FEC_PROPERTY_3RD_HIERARCHY;
    }
    else
    {
        sal_printf("Error, Unsupported hierarchy %d \n", hierarchy);
        return BCM_E_PARAM;
    }

    rv = bcm_switch_fec_property_get(unit, &fec_config);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, bcm_switch_fec_property_get\n");
        return rv;
    }

    *start = fec_config.start;
    *nof_fecs = fec_config.end - fec_config.start + 1;
    return rv;
}

/*
 * Get the first FEC ID of the given hierarchy which isn't in the ECMP range
 */
int
get_first_fec_in_range_which_not_in_ecmp_range(
    int unit,
    int hierarchy,
    int *fec_id)
{
    int rv;
    int nof_fecs;
    uint32 ecmp_range;

    ecmp_range = dnx_data_l3.ecmp.total_nof_ecmp_get(unit);
    rv = get_fec_range(unit, hierarchy, fec_id, &nof_fecs);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, get_first_fec_in_range\n");
        return rv;
    }
    if (*fec_id < ecmp_range)
    {
        if (nof_fecs > ecmp_range)
        {
            *fec_id = ecmp_range;
        }
        else
        {
            sal_printf("Error, can't find a FEC ID outside of the ECMP range\n");
            return BCM_E_NOT_FOUND;
        }
    }

    return rv;
}

int
dnx_apt_vlan__open_vlan_per_mc(
    int unit,
    int vlan,
    bcm_multicast_t mc_group)
{
    bcm_vlan_control_vlan_t control_vlan;
    int rc;

    bcm_vlan_control_vlan_t_init(&control_vlan);

    /*
     * The vlan in this case may also represent a vsi. in that case, it should be created with a different api
     */
    if (vlan <= BCM_VLAN_MAX)
    {
        rc = bcm_vlan_create(unit, vlan);
    }
    else
    {
        rc = bcm_vswitch_create_with_id(unit, vlan);
    }
    if (rc != BCM_E_NONE)
    {
        sal_printf("fail open vlan(%d)", vlan);
        if (rc != BCM_E_EXISTS)
        {
            return rc;
        }
    }

    /*
     * Set VLAN with MC
     */
    rc = bcm_vlan_control_vlan_get(unit, vlan, &control_vlan);
    BCM_IF_ERROR_RETURN(rc);

    control_vlan.unknown_unicast_group = mc_group;
    control_vlan.unknown_multicast_group = mc_group;
    control_vlan.broadcast_group = mc_group;
    rc = bcm_vlan_control_vlan_set(unit, vlan, control_vlan);
    BCM_IF_ERROR_RETURN(rc);

    return rc;
}

/*
 * Set In-Port default ETH-RIF:
 * - in_port: Incoming port ID
 * - eth_rif: ETH-RIF
 */
int
dnx_apt_in_port_intf_set(
    int unit,
    int in_port,
    int eth_rif)
{
    bcm_vlan_port_t vlan_port;
    int rc;
    char *proc_name;

    proc_name = "dnx_apt_in_port_intf_set";
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = eth_rif;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rc = bcm_vlan_port_create(unit, &vlan_port);
    if (rc != BCM_E_NONE)
    {
        sal_printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rc;
    }

    rc = bcm_vlan_gport_add(unit, eth_rif, in_port, 0);
    if (rc != BCM_E_NONE)
    {
        sal_printf("Error in %s(): bcm_vlan_gport_add \n", proc_name);
        return rc;
    }

    return rc;
}

/*
 * Create VSI ETH-RIF and set initial properties:
 * - my_mac - My-MAC address
 */
int
dnx_apt_intf_eth_rif_create(
    int unit,
    int eth_rif_id,
    bcm_mac_t my_mac)
{
    bcm_l3_intf_t l3if;
    int rc;

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    /*
     * My-MAC
     */
    sal_memcpy(l3if.l3a_mac_addr, my_mac, 6);
    l3if.l3a_intf_id = l3if.l3a_vid = eth_rif_id;
    /*
     * set qos map id to 0 as default
     */
    l3if.dscp_qos.qos_map_id = 0;

    rc = bcm_l3_intf_create(unit, &l3if);
    if (rc != BCM_E_NONE)
    {
        sal_printf("Error, bcm_l3_intf_create %d\n", rc);
        return rc;
    }

    return rc;
}

int
dnx_apt_l3__intf_rif__create(
    int unit,
    create_l3_intf_t * l3_intf)
{
    bcm_l3_intf_t l3if, l3if_old;
    int rc;
    bcm_l3_ingress_t l3_ing_if;
    int enable_public = 0;

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    bcm_l3_intf_t_init(&l3if_old);
    bcm_l3_ingress_t_init(&l3_ing_if);

    l3if.l3a_flags = BCM_L3_WITH_ID;    /* WITH-ID or without-ID does not really matter. Anyway for now RIF equal VSI */
    if ((l3_intf->no_publc == 0) && (   /* Update the in_rif to have public searches enabled for vrf == 0 */
                                        (l3_intf->vrf == 0)))
    {
        l3_intf->vrf_valid = 1;
        enable_public = 1;
    }

    l3if.l3a_vid = l3_intf->vsi;
    l3if.l3a_ttl = 31;  /* default settings */
    if (l3_intf->ttl_valid)
    {
        l3if.l3a_ttl = l3_intf->ttl;
    }
    l3if.l3a_mtu = 1524;        /* default settings */
    if (l3_intf->mtu_valid)
    {
        l3if.l3a_mtu = l3_intf->mtu;
        l3if.l3a_mtu_forwarding = l3_intf->mtu_forwarding;
    }
    l3if.native_routing_vlan_tags = l3_intf->native_routing_vlan_tags;
    l3_intf->rif = l3if.l3a_intf_id = l3_intf->vsi;     /* In DNX Arch VSI always equal RIF */

    sal_memcpy(l3if.l3a_mac_addr, l3_intf->my_lsb_mac, 6);
    sal_memcpy(l3if.l3a_mac_addr, l3_intf->my_global_mac, 4);   /* ovewriting 4 MSB bytes with global MAC configuration 
                                                                 */
    l3if.native_routing_vlan_tags = l3_intf->native_routing_vlan_tags;

    l3if_old.l3a_intf_id = l3_intf->vsi;

    if (l3_intf->qos_map_valid)
    {
        l3if.dscp_qos.qos_map_id = l3_intf->qos_map_id;
    }

    rc = bcm_l3_intf_create(unit, &l3if);
    BCM_IF_ERROR_RETURN(rc);

    if (l3_intf->vrf_valid || l3_intf->rpf_valid)
    {
        l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;       /* must, as we update exist RIF */
        l3_ing_if.vrf = l3_intf->vrf;

        /*
         * set RIF enable RPF
         */
        /*
         * In Arad+ the urpf mode is per RIF (if the SOC property bcm886XX_l3_ingress_urpf_enable is set).
         */
        if (l3_intf->rpf_valid)
        {
            /*
             * Set uRPF global configuration
             */
            rc = bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, l3_intf->urpf_mode);
            if (rc != BCM_E_NONE)
            {
                return rc;
            }
        }
        if (l3_intf->flags & BCM_L3_RPF)
        {
            l3_ing_if.urpf_mode = l3_intf->urpf_mode;
        }
        else
        {
            l3_ing_if.urpf_mode = bcmL3IngressUrpfDisable;
        }

        if ((l3_intf->ingress_flags & BCM_L3_INGRESS_GLOBAL_ROUTE) || ((enable_public == 1)))
        {
            l3_ing_if.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
        }
        if (l3_intf->ingress_flags & BCM_L3_INGRESS_DSCP_TRUST)
        {
            l3_ing_if.flags |= BCM_L3_INGRESS_DSCP_TRUST;
        }
        if (l3_intf->ingress_flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST)
        {
            l3_ing_if.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST;
        }
        if (l3_intf->ingress_flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST)
        {
            l3_ing_if.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST;
        }
        if (l3_intf->ingress_flags & BCM_L3_INGRESS_ROUTE_DISABLE_MPLS)
        {
            l3_ing_if.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_MPLS;
        }

        if (l3_intf->qos_map_valid)
        {
            l3_ing_if.qos_map_id = l3_intf->qos_map_id;
        }

        if (l3_intf->oam_default_profile)
        {
            l3_ing_if.oam_default_profile = l3_intf->oam_default_profile;
        }

        rc = bcm_l3_ingress_create(unit, &l3_ing_if, &l3if.l3a_intf_id);
        if (rc != BCM_E_NONE)
        {
            sal_printf("Error, bcm_l3_ingress_create\n");
            return rc;
        }
        l3_intf->rif = l3if.l3a_intf_id;
    }

    sal_printf("created ingress interface = 0x%08x, on vlan = %d in unit %d, vrf = %d\n",
               l3_intf->rif, l3_intf->vsi, unit, l3_intf->vrf);
    sal_printf("mac-address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",
               l3_intf->my_global_mac[0],
               l3_intf->my_global_mac[1],
               l3_intf->my_global_mac[2], l3_intf->my_global_mac[3], l3_intf->my_global_mac[4],
               l3_intf->my_global_mac[5]);

    return rc;
}

/* set egress action over this l3 interface, so packet forwarded to this interface will be tunneled/swapped/popped */
int
dnx_apt_mpls__create_tunnel_initiator__set(
    int unit,
    mpls__egress_tunnel_utils_t * mpls_tunnel_properties)
{
    bcm_mpls_egress_label_t label_array[2];
    int num_labels;
    int mpls_ttl = 20;
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    if (mpls_tunnel_properties->force_flags)
    {
        label_array[0].flags = mpls_tunnel_properties->flags;
    }
    else
    {
        label_array[0].flags = mpls_tunnel_properties->flags | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    }

    label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;

    if (mpls_tunnel_properties->flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID)
    {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        label_array[0].action = mpls_tunnel_properties->egress_action;
    }
    label_array[0].label = mpls_tunnel_properties->label_in;
    label_array[0].ttl = mpls_tunnel_properties->ttl ? mpls_tunnel_properties->ttl : mpls_ttl;
    label_array[0].spl_label_push_type = mpls_tunnel_properties->spl_label_push_type;

    if (mpls_tunnel_properties->tunnel_id != 0)
    {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
        label_array[0].tunnel_id = mpls_tunnel_properties->tunnel_id;
    }
    num_labels = 1;

    if (mpls_tunnel_properties->flags & BCM_MPLS_EGRESS_LABEL_PROTECTION)
    {
        label_array[0].egress_failover_id = mpls_tunnel_properties->egress_failover_id;
        label_array[0].egress_failover_if_id = mpls_tunnel_properties->egress_failover_if_id;
    }

    label_array[0].encap_access = mpls_tunnel_properties->encap_access;
    label_array[0].l3_intf_id = mpls_tunnel_properties->next_pointer_intf;
    /*
     * in JR2 l3_intf_id and encap_access must be similar for all provided labels
     */
    label_array[1].l3_intf_id = label_array[0].l3_intf_id;
    label_array[1].encap_access = label_array[0].encap_access;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    mpls_tunnel_properties->tunnel_id = label_array[0].tunnel_id;

    return rv;
}

/* Set a FEC entry, without allocating ARP entry */
int
dnx_apt_l3__egress_only_fec__create(
    int unit,
    create_l3_egress_t * l3_egress)
{
    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_l3_egress_t_init(&l3eg);

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

    rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | l3_egress->allocation_flags, &l3eg, &l3egid);
    if (rc != BCM_E_NONE)
    {
        sal_printf("Error, create egress object, unit=%d, \n", unit);
        return rc;
    }

    l3_egress->fec_id = l3egid;

    return rc;
}

int
dnx_apt_l3__egress_only_encap__create(
    int unit,
    create_l3_egress_t * l3_egress)
{
    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */

    bcm_l3_egress_t_init(&l3eg);

    l3eg.flags = l3_egress->l3_flags;
    l3eg.flags2 = l3_egress->l3_flags2;
    sal_memcpy(l3eg.mac_addr, l3_egress->next_hop_mac_addr, 6);
    l3eg.vlan = l3_egress->vlan;
    l3eg.encap_id = l3_egress->arp_encap_id;
    if (BCM_L3_ITF_TYPE_IS_LIF(l3_egress->out_tunnel_or_rif))
    {
        l3eg.intf = l3_egress->out_tunnel_or_rif;
    }
    l3eg.qos_map_id = l3_egress->qos_map_id;

    rc = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | l3_egress->allocation_flags, &l3eg, &l3egid_null);
    BCM_IF_ERROR_RETURN(rc);

    l3_egress->arp_encap_id = l3eg.encap_id;

    return rc;
}

/* Create egress object(FEC and ARP entry)*/
int
dnx_apt_l3__egress__create(
    int unit,
    create_l3_egress_t * l3_egress)
{
    int rc;

    /*
     * In JR2, FEC and ARP created sperately
     */
    int arp_encap_id = 0;
    rc = dnx_apt_l3__egress_only_encap__create(unit, l3_egress);
    if (rc != BCM_E_NONE)
    {
        sal_printf("Error, dnx_apt_l3__egress_only_encap__create\n");
        return rc;
    }
    arp_encap_id = l3_egress->arp_encap_id;

    /*
     * JR2, Set intf in FEC if it exists, otherwise arp_encap_id. But won't both unless intf is RIF.
     */
    if (BCM_L3_ITF_TYPE_IS_LIF(l3_egress->arp_encap_id) && BCM_L3_ITF_TYPE_IS_LIF(l3_egress->out_tunnel_or_rif))
    {
        /** FEC can't carry two outlifs, if encap_id was created in above call, set it to 0
         * temporarily.*/
        l3_egress->arp_encap_id = 0;
    }
    rc = dnx_apt_l3__egress_only_fec__create(unit, l3_egress);
    if (rc != BCM_E_NONE)
    {
        sal_printf("Error, dnx_apt_l3__egress_only_fec__create\n");
        return rc;
    }

    /*
     * Set back arp_encap_id
     */
    l3_egress->arp_encap_id = arp_encap_id;

    return rc;
}

int
dnx_apt_add_host(
    int unit,
    uint32 addr,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_host_t l3host;

    bcm_l3_host_t_init(&l3host);

    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = addr;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = intf;
    l3host.l3a_port_tgid = 0;

    rc = bcm_l3_host_add(unit, &l3host);
    if (rc != BCM_E_NONE)
    {
        sal_printf("bcm_l3_host_add failed: %x \n", rc);
    }

    return rc;
}

/* Add term entry to perform pop
*/
int
dnx_apt_mpls_add_term_entry_ex(
    int unit,
    int term_label,
    bcm_mpls_tunnel_switch_t * entry)
{
    int rv;

    bcm_mpls_tunnel_switch_t_init(entry);
    entry->action = BCM_MPLS_SWITCH_ACTION_POP;
    entry->label = term_label;
    entry->qos_map_id = 0;      /* qos not rellevant for BFD */

    rv = bcm_mpls_tunnel_switch_create(unit, entry);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}

/* creating l3 interface */
int
dnx_apt_l3_interface_init(
    int unit,
    int in_sysport,
    int out_sysport,
    int *_next_hop_mac_encap_id,
    int *_tunnel_id,
    int create_mpls_tunnel)
{
    int rv;
/*    int ing_intf_in;*/
    int ing_intf_out;
    int fec[2] = { 0x0, 0x0 };
    int in_vlan = 10;
    int out_vlan = 100;
    int vrf;
    /*
     * int host;
     */
    int encap_id[2] = { 0 };
    int route;
    int mask;
    int l3_eg_int;
    /*
     * int trap_id;
     */
    /*
     * bcm_l3_intf_t intf_sbfd;
     */
    bcm_mac_t interface_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x0a };   /* my-MAC */
    bcm_mac_t mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    /*
     * bcm_mac_t internal_my_mac = {0x00, 0x0c, 0x00, 0x02,0x00, 0x22};
     *//*
     * my-MAC used reflector 2nd process after recycle
     */
    bcm_mac_t next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };    /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2 = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x2d };   /* bfd dst mac */
    bcm_l3_route_t bfd_route;
    create_l3_intf_t intf;
    create_l3_egress_t l3eg;
    create_l3_egress_t l3eg1;
    mpls__egress_tunnel_utils_t mpls_tunnel_properties;

    sal_memset(&intf, 0x00, sizeof(create_l3_intf_t));
    sal_memset(&mpls_tunnel_properties, 0x00, sizeof(mpls__egress_tunnel_utils_t));
    sal_memset(&l3eg, 0x00, sizeof(create_l3_egress_t));
    sal_memset(&l3eg1, 0x00, sizeof(create_l3_egress_t));

    vrf = 1;

    /*** create ingress router interface ***/
    rv = dnx_apt_vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
    BCM_IF_ERROR_RETURN(rv);

    intf.vsi = in_vlan;
    sal_memcpy(intf.my_global_mac, mac_address, 6);
    sal_memcpy(intf.my_lsb_mac, interface_mac_address, 6);
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;

    /*** create egress router interface ***/
    rv = dnx_apt_vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        sal_printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
        return rv;
    }

    intf.vsi = out_vlan;

    rv = dnx_apt_l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, dnx_apt_l3__intf_rif__create\n");
    }

    /*** Create tunnel if required ***/
    if (create_mpls_tunnel)
    {
        *_tunnel_id = 0;
        mpls_tunnel_properties.label_in = mpls_label_termination;
        mpls_tunnel_properties.label_out = 0;

        sal_printf("Trying to create tunnel initiator\n");
        rv = dnx_apt_mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
        if (rv != BCM_E_NONE)
        {
            sal_printf("Error, in dnx_apt_mpls__create_tunnel_initiator__set\n");
            return rv;
        }

        *_tunnel_id = mpls_tunnel_properties.tunnel_id;
        sal_printf("tunnel_id (egress_interface):0x%08x \n", *_tunnel_id);
        l3_eg_int = *_tunnel_id;
    }
    else
    {
        l3_eg_int = ing_intf_out;
    }

    /*** Create egress object1 ***/
    l3eg.out_tunnel_or_rif = l3_eg_int;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac, 6);
    l3eg.vlan = out_vlan;
    l3eg.arp_encap_id = encap_id[0];
    l3eg.fec_id = fec[0];
    l3eg.allocation_flags = 0;
    l3eg.out_gport = out_sysport;

    rv = dnx_apt_l3__egress__create(unit, &l3eg);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_l3__egress__create  \n", rv);
    }
    encap_id[0] = l3eg.arp_encap_id;
    fec[0] = l3eg.fec_id;

    if (!create_mpls_tunnel)
    {
        *_tunnel_id = encap_id[0];
    }

    /*** add host ***/
    rv = dnx_apt_add_host(unit, /* 0x7fffff03 */ 0x030F0701, vrf, fec[0]);
    BCM_IF_ERROR_RETURN(rv);

    /*** create egress object 2***/
    /*
     * We're allocating a lif. out_sysport unit should be first, and it's already first
     */

    l3eg1.out_tunnel_or_rif = l3_eg_int;
    sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2, 6);
    l3eg1.vlan = out_vlan;      /* bfd out vlan */
    l3eg1.arp_encap_id = encap_id[1];
    l3eg1.fec_id = fec[1];
    l3eg1.allocation_flags = 0;
    l3eg1.out_gport = out_sysport;

    rv = dnx_apt_l3__egress__create(unit, &l3eg1);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in bcm_vlan_create with vlan %d continue \n", rv, l3eg.vlan);
    }
    encap_id[1] = l3eg1.arp_encap_id;
    fec[1] = l3eg1.fec_id;
    *_next_hop_mac_encap_id = encap_id[1];      /* bfd egress interface */

    /*** add route point to FEC2 ***/
    bcm_l3_route_t_init(&bfd_route);
    route = 0x7fffff00;
    mask = 0xfffffff0;
    bfd_route.l3a_subnet = route;
    bfd_route.l3a_ip_mask = mask;
    bfd_route.l3a_vrf = vrf;
    bfd_route.l3a_intf = fec[1];

    bfd_route.l3a_flags2 = 0;

    rv = bcm_l3_route_add(unit, &bfd_route);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

/*
* Creating mpls tunnel and termination.
*/
int
dnx_apt_mpls_init(
    int unit,
    int *tunnel_id,
    int is_BFDCC,
    int is_muti_hop_mpls,
    int port1,
    int port2)
{
    bcm_error_t rv;
    int create_mpls_tunnel;
    int iter = 0;

    port_1 = port1;
    port_2 = port2;

    if (is_BFDCC)
    {
        create_mpls_tunnel = 1;
    }
    else
    {
        create_mpls_tunnel = is_muti_hop_mpls ? 1 : 0;
    }

    rv = dnx_apt_l3_interface_init(unit, port_1, port_2, &ctest_bfd_egress_interface, tunnel_id, create_mpls_tunnel);
    if (rv != BCM_E_NONE)
    {
        sal_printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    for (iter = 0; iter < DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS; iter++)
    {
        rv = dnx_apt_mpls_add_term_entry_ex(unit, mpls_label + iter, &tunnel_switch[iter]);
        if (rv != BCM_E_NONE)
        {
            sal_printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

int
dnx_apt_vpls_configure_port_properties(
    int unit,
    int ac_port,
    int pwe_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;

    /*
     * set class for both ports
     */
    rv = bcm_port_class_set(unit, ac_port, bcmPortClassId, ac_port);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, in bcm_port_class_set, port=%d, \n", ac_port);
        return rv;
    }

    rv = bcm_port_class_set(unit, pwe_port, bcmPortClassId, pwe_port);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, in bcm_port_class_set, port=%d, \n", pwe_port);
        return rv;
    }

    dnx_apt_vpls_basic_info.ac_port = ac_port;
    dnx_apt_vpls_basic_info.pwe_port = pwe_port;

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = dnx_apt_in_port_intf_set(unit, dnx_apt_vpls_basic_info.pwe_port, dnx_apt_vpls_basic_info.pwe_intf);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, dnx_apt_in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
    vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, bcm_vlan_port_create vlan_port\n");
        return rv;
    }

    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = dnx_apt_vpls_basic_info.pwe_port;
    rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rv != 0)
    {
        sal_printf("Error, in bcm_port_match_add with port(%d)\n", match_info.port);
        return rv;
    }

    match_info.port = dnx_apt_vpls_basic_info.ac_port;
    rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rv != 0)
    {
        sal_printf("Error, in bcm_port_match_add with port(%d)\n", match_info.port);
        return rv;
    }

    return rv;
}

int
dnx_apt_vpls_create_l3_interfaces(
    int unit)
{
    int rv;

    /*
     * Create ETH-RIF and set its properties
     */
    rv = dnx_apt_intf_eth_rif_create(unit, dnx_apt_vpls_basic_info.pwe_intf,
                                     dnx_apt_vpls_basic_info.pwe_intf_mac_address);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, dnx_apt_intf_eth_rif_create pwe_intf\n");
    }

    return rv;
}

int
dnx_apt_vpls_create_arp_entry(
    int unit)
{
    int rv;
    create_l3_egress_t l3eg;
    sal_memset(&l3eg, 0x00, sizeof(create_l3_egress_t));

    l3eg.arp_encap_id = dnx_apt_vpls_basic_info.core_arp_id;
    sal_memcpy(l3eg.next_hop_mac_addr, dnx_apt_vpls_basic_info.pwe_arp_next_hop_mac, 6);
    l3eg.vlan = dnx_apt_vpls_basic_info.pwe_intf;

    /*
     * Configure ARP entry
     */
    rv = dnx_apt_l3__egress_only_encap__create(unit, &l3eg);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, create AC egress object ARP only\n");
    }

    dnx_apt_vpls_basic_info.core_arp_id = l3eg.arp_encap_id;

    return rv;
}

int
dnx_apt_vpls_create_fec_entry(
    int unit)
{
    int rv;
    create_l3_egress_t l3eg;
    sal_memset(&l3eg, 0x00, sizeof(create_l3_egress_t));

    if (!dnx_apt_vpls_basic_info.skip_auto_fec_allocation)
    {
        /*
         * Jericho 2 support static FEC allocation as other cints uses the first FEC ID add +8
         */
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &dnx_apt_vpls_basic_info.mpls_encap_fec_id);
        dnx_apt_vpls_basic_info.mpls_encap_fec_id += 8;
        if (rv != BCM_E_NONE)
        {
            sal_printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }

    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Tunnel id of the encapsulation entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */
    l3eg.fec_id = dnx_apt_vpls_basic_info.mpls_encap_fec_id;
    l3eg.out_tunnel_or_rif = dnx_apt_vpls_basic_info.mpls_tunnel_id;
    l3eg.out_gport = dnx_apt_vpls_basic_info.pwe_port;

    rv = dnx_apt_l3__egress_only_fec__create(unit, &l3eg);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, create egress object FEC only\n");
    }

    /*
     * The termination scenario doesn't need FEC.
     * We bridge out of the core with no usage of FEC.
     */
    return rv;
}

int
dnx_apt_vpls_create_mpls_tunnel(
    int unit)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int rv;
    bcm_mpls_label_t mpls_tunnel_label;
    bcm_if_t mpls_tunnel_id;
    int core_arp_id;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    mpls_tunnel_label = dnx_apt_vpls_basic_info.mpls_tunnel_label;
    mpls_tunnel_id = dnx_apt_vpls_basic_info.mpls_tunnel_id;
    core_arp_id = dnx_apt_vpls_basic_info.core_arp_id;

    /*
     * Label to be pushed as part of the MPLS tunnel.
     */
    label_array[0].label = mpls_tunnel_label;
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID;
    BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel_id);

    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
     * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = core_arp_id;
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    dnx_apt_vpls_basic_info.mpls_tunnel_id = label_array[0].tunnel_id;

    return rv;
}

int
dnx_apt_vpls_create_termination_stack(
    int unit)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv;

    /*
     * Create a stack of MPLS labels to be terminated
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    /*
     * incoming label
     * only the mpls tunnel label needs to be defined here.
     * pwe label will be handed as part of the ingress mpls_port_add api
     */
    entry.label = dnx_apt_vpls_basic_info.pwe_termination_label;
    entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    dnx_apt_vpls_basic_info.mpls_tunnel_id_ingress = entry.tunnel_id;
    return rv;

}

int
dnx_apt_vpls_l2_addr_add(
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, dnx_apt_vpls_basic_info.ac_mac_address, vpn);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.port = dnx_apt_vpls_basic_info.vlan_port_id;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in bcm_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}

int
dnx_apt_vpls_mpls_port_add_encapsulation(
    int unit)
{
    int rv = BCM_E_NONE;
    int iter = 0;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    int mpls_port_id_egress_start = 8888;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

    if (dnx_apt_vpls_basic_info.cw_present)
    {
        mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    for (iter = 0; iter < DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS; iter++)
    {
        BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, mpls_port_id_egress_start + iter);
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
        mpls_port.egress_label.label = dnx_apt_vpls_basic_info.pwe_label + iter;

        rv = bcm_mpls_port_add(unit, 0, &mpls_port);
        if (rv != BCM_E_NONE)
        {
            sal_printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
            return rv;
        }

        dnx_apt_vpls_basic_info.mpls_port_id_egress[iter] = mpls_port.mpls_port_id;
        dnx_apt_vpls_basic_info.encap_id[iter] = mpls_port.encap_id;
    }

    return rv;
}

int
dnx_apt_vpls_mpls_port_add_termination(
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int iter = 0;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    int mpls_port_id_ingress_start = 9999;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;

    if (dnx_apt_vpls_basic_info.cw_present)
    {
        mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
    mpls_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    /*
     * encap_id is the egress outlif - used for learning
     */
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    /** connect PWE entry to created MPLS encapsulation entry for learning */
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, dnx_apt_vpls_basic_info.mpls_encap_fec_id);

    for (iter = 0; iter < DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS; iter++)
    {
        BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, mpls_port_id_ingress_start + iter);
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
        mpls_port.match_label = dnx_apt_vpls_basic_info.pwe_label + iter;
        mpls_port.encap_id = dnx_apt_vpls_basic_info.encap_id[iter];

        rv = bcm_mpls_port_add(unit, vpn, &mpls_port);
        if (rv != BCM_E_NONE)
        {
            sal_printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
            return rv;
        }

        dnx_apt_vpls_basic_info.mpls_port_id_ingress[iter] = mpls_port.mpls_port_id;
    }

    return rv;
}

int
dnx_apt_vpls_vswitch_add_access_port(
    int unit,
    bcm_gport_t port_id)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port_id;
    vlan_port.match_vlan = dnx_apt_vpls_basic_info.vpn;
    vlan_port.egress_vlan = 0;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    dnx_apt_vpls_basic_info.vlan_port_id = vlan_port.vlan_port_id;

    rv = bcm_vswitch_port_add(unit, dnx_apt_vpls_basic_info.vpn, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, dnx_apt_vpls_basic_info.vpn, port_id, 0);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}

/*
 * Main function for basic mpls vpls scenario.
 */
int
dnx_apt_vpls_main(
    int unit,
    int ac_port,
    int pwe_port)
{
    int rv = BCM_E_NONE;

    dnx_apt_vpls_basic_info.ac_port = ac_port;
    dnx_apt_vpls_basic_info.pwe_port = pwe_port;

    /*
     * Configure AC and PWE ports
     */
    rv = dnx_apt_vpls_configure_port_properties(unit, ac_port, pwe_port);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_configure_port_properties\n", rv);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn)
     */
    rv = bcm_vlan_create(unit, dnx_apt_vpls_basic_info.vpn);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Failed (%d) to create VLAN %d\n", rv, dnx_apt_vpls_basic_info.vpn);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = dnx_apt_vpls_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = dnx_apt_vpls_create_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = dnx_apt_vpls_create_mpls_tunnel(unit);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = dnx_apt_vpls_create_fec_entry(unit);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    rv = dnx_apt_vpls_create_termination_stack(unit);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_create_termination_stack\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = dnx_apt_vpls_mpls_port_add_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = dnx_apt_vpls_mpls_port_add_termination(unit, dnx_apt_vpls_basic_info.vpn);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_mpls_port_add_termination\n", rv);
        return rv;
    }

    /** define ac and pwe ports */
    rv = dnx_apt_vpls_vswitch_add_access_port(unit, ac_port);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_vswitch_add_access_port\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    rv = dnx_apt_vpls_l2_addr_add(unit, dnx_apt_vpls_basic_info.vpn);
    if (rv != BCM_E_NONE)
    {
        sal_printf("Error(%d), in dnx_apt_vpls_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}

int
dnx_apt_bfd_ipv4_example_init(
    int unit,
    int p1,
    int p2)
{
    int rv;

    port_1 = p1;
    port_2 = p2;

    rv = dnx_apt_l3_interface_init(unit, port_1, port_2, &ctest_bfd_egress_interface, &tunnel_id, 0);
    BCM_IF_ERROR_RETURN(rv);

    sal_printf("Created egress interface 0x%x\n", ctest_bfd_egress_interface);

    return rv;
}

int
dnx_apt_bfd_ipv6_example_init(
    int unit,
    int p1,
    int p2)
{
    int rv;

    port_1 = p1;
    port_2 = p2;

    rv = dnx_apt_l3_interface_init(unit, port_1, port_2, &ctest_bfd_egress_interface, &tunnel_id, 0);
    BCM_IF_ERROR_RETURN(rv);

    sal_printf("Created egress interface 0x%x\n", ctest_bfd_egress_interface);

    return rv;
}

int
dnx_apt_bfd_mpls_tp_cc_example_init(
    int unit,
    int p1,
    int p2)
{
    int rv;

    port_1 = p1;
    port_2 = p2;

    rv = dnx_apt_mpls_init(unit, &tunnel_id, 1, 0, port_1, port_2);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

int
dnx_apt_bfd_mpls_example_init(
    int unit,
    int p1,
    int p2)
{
    int rv;

    port_1 = p1;
    port_2 = p2;

    rv = dnx_apt_mpls_init(unit, &tunnel_id, 0, 0, port_1, port_2);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

int
dnx_apt_bfd_pwe_example_init(
    int unit,
    int p1,
    int p2)
{
    int rv;

    port_1 = p1;
    port_2 = p2;

    dnx_apt_vpls_basic_info.cw_present = 1;
    rv = dnx_apt_vpls_main(unit, port_1, port_2);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

/***********************************************************************************************************************
 * BFD Endpoint time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_insert_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, UDP_IPV4_SHOP_ACC, INSERT);
}

static double
dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_insert_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, UDP_IPV6_SHOP_ACC, INSERT);
}

static double
dnx_apt_bfd_endpoint_mpls_tp_cc_acc_insert_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, MPLS_TP_CC_ACC, INSERT);
}

static double
dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_insert_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, MPLS_IPV4_SHOP_ACC, INSERT);
}

static double
dnx_apt_bfd_endpoint_pwe_control_word_acc_insert_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_CONTROL_WORD_ACC, INSERT);
}

static double
dnx_apt_bfd_endpoint_pwe_router_alter_acc_insert_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_ROUTER_ALTER_ACC, INSERT);
}

static double
dnx_apt_bfd_endpoint_pwe_ttl_acc_insert_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_TTL_ACC, INSERT);
}

static double
dnx_apt_bfd_endpoint_pwe_gal_acc_insert_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_GAL_ACC, INSERT);
}

static double
dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_update_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, UDP_IPV4_SHOP_ACC, UPDATE);
}

static double
dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_update_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, UDP_IPV6_SHOP_ACC, UPDATE);
}

static double
dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, MPLS_TP_CC_ACC, UPDATE);
}

static double
dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, MPLS_IPV4_SHOP_ACC, UPDATE);
}

static double
dnx_apt_bfd_endpoint_pwe_control_word_acc_update_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_CONTROL_WORD_ACC, UPDATE);
}

static double
dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_ROUTER_ALTER_ACC, UPDATE);
}

static double
dnx_apt_bfd_endpoint_pwe_ttl_acc_update_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_TTL_ACC, UPDATE);
}

static double
dnx_apt_bfd_endpoint_pwe_gal_acc_update_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_GAL_ACC, UPDATE);
}

static double
dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_destroy_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, UDP_IPV4_SHOP_ACC, DESTROY);
}

static double
dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_destroy_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, UDP_IPV6_SHOP_ACC, DESTROY);
}

static double
dnx_apt_bfd_endpoint_mpls_tp_cc_acc_destroy_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, MPLS_TP_CC_ACC, DESTROY);
}

static double
dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_destroy_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, MPLS_IPV4_SHOP_ACC, DESTROY);
}

static double
dnx_apt_bfd_endpoint_pwe_control_word_acc_destroy_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_CONTROL_WORD_ACC, DESTROY);
}

static double
dnx_apt_bfd_endpoint_pwe_router_alter_acc_destroy_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_ROUTER_ALTER_ACC, DESTROY);
}

static double
dnx_apt_bfd_endpoint_pwe_ttl_acc_destroy_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_TTL_ACC, DESTROY);
}

static double
dnx_apt_bfd_endpoint_pwe_gal_acc_destroy_time_threshold(
    int unit)
{
    return DNX_APT_BFD_ENDPOINT_TIME_THRESHOLD(unit, PWE_GAL_ACC, DESTROY);
}

/***********************************************************************************************************************
 * BFD Endpoint optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, UDP_IPV4_SHOP_ACC, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, UDP_IPV6_SHOP_ACC, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_mpls_tp_cc_acc_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, MPLS_TP_CC_ACC, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, MPLS_IPV4_SHOP_ACC, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_control_word_acc_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_CONTROL_WORD_ACC, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_router_alter_acc_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_ROUTER_ALTER_ACC, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_ttl_acc_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_TTL_ACC, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_gal_acc_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_GAL_ACC, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, UDP_IPV4_SHOP_ACC, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, UDP_IPV6_SHOP_ACC, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, MPLS_TP_CC_ACC, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, MPLS_IPV4_SHOP_ACC, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_control_word_acc_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_CONTROL_WORD_ACC, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_ROUTER_ALTER_ACC, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_ttl_acc_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_TTL_ACC, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_gal_acc_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_GAL_ACC, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, UDP_IPV4_SHOP_ACC, DESTROY);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, UDP_IPV6_SHOP_ACC, DESTROY);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_mpls_tp_cc_acc_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, MPLS_TP_CC_ACC, DESTROY);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, MPLS_IPV4_SHOP_ACC, DESTROY);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_control_word_acc_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_CONTROL_WORD_ACC, DESTROY);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_router_alter_acc_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_ROUTER_ALTER_ACC, DESTROY);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_ttl_acc_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_TTL_ACC, DESTROY);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_bfd_endpoint_pwe_gal_acc_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_BFD_ENDPOINT_OPTIONAL_THRESHOLD_INFO(info, PWE_GAL_ACC, DESTROY);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * BFD Endpoint shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_alloc(
    int unit,
    int number_of_meps,
    dnx_apt_bfd_endpoint_info_t ** bfd_endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*bfd_endpoint_info, (sizeof(dnx_apt_bfd_endpoint_info_t) * number_of_meps), "dnx_apt_bfd_endpoint_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_bfd_endpoint_generate(
    int unit,
    dnx_apt_bfd_endpoint_test_obj_t test_obj,
    int number_of_meps,
    dnx_apt_bfd_endpoint_info_t * bfd_endpoint_info,
    int is_update)
{
    int iter = 0;
    int iter2 = 0;
    int nof_meps_in_loop = is_update ? 2 : 1;
    bcm_bfd_endpoint_info_t *pEndpoint;
    int flags_stats_supported[] = {
        0,
        BCM_BFD_ENDPOINT_FLAGS2_RX_STATISTICS,
        BCM_BFD_ENDPOINT_FLAGS2_TX_STATISTICS,
        BCM_BFD_ENDPOINT_FLAGS2_RX_STATISTICS | BCM_BFD_ENDPOINT_FLAGS2_TX_STATISTICS,
    };
    int bfd_tx_periods[] = { 0,
        3,
        10,
        100,
        500,
        1000,
        1200,
        1500
    };
    int bfd_req_intervals[8] = { 0 };
    int internal_priorities[8] = { 0 };
    int pwe_ttl_exp[16] = { 0 };
    int src_ip_prefix = 0;
    int shop_tos = 0;
    int udp_sport = 0;
    int i = 0, index = 0;
    bcm_bfd_tunnel_type_t type_per_test[] = {
        bcmBFDTunnelTypeUdp,
        bcmBFDTunnelTypeUdp,
        bcmBFDTunnelTypeUdp,
        bcmBFDTunnelTypeUdp,
        bcmBFDTunnelTypeMpls,
        bcmBFDTunnelTypeMpls,
        bcmBFDTunnelTypeMpls,
        bcmBFDTunnelTypeMpls,
        bcmBFDTunnelTypeMplsTpCc,
        bcmBFDTunnelTypePweControlWord,
        bcmBFDTunnelTypePweRouterAlert,
        bcmBFDTunnelTypePweTtl,
        bcmBFDTunnelTypePweGal
    };
    char *type_str_per_test[] = {
        "bcmBFDTunnelTypeUdp",
        "bcmBFDTunnelTypeUdp",
        "bcmBFDTunnelTypeUdp",
        "bcmBFDTunnelTypeUdp",
        "bcmBFDTunnelTypeMpls",
        "bcmBFDTunnelTypeMpls",
        "bcmBFDTunnelTypeMpls",
        "bcmBFDTunnelTypeMpls",
        "bcmBFDTunnelTypeMplsTpCc",
        "bcmBFDTunnelTypePweControlWord",
        "bcmBFDTunnelTypePweRouterAlert",
        "bcmBFDTunnelTypePweTtl",
        "bcmBFDTunnelTypePweGal"
    };
    bcm_ip6_t src_ipv6_tmp =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_ip6_t dst_ipv6_tmp =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x80, 0x01, 0x01 };

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Generate parameters for test_obj(%d) with test_type(%s)\n"), test_obj,
             type_str_per_test[test_obj]));

    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS));
    dnx_apt_random_seed(0);

    SHR_NULL_CHECK(bfd_endpoint_info, _SHR_E_INTERNAL, "bfd_endpoint_info");

    src_ip_prefix = dnx_apt_random_get() & 0x1FFFFF00;
    shop_tos = dnx_apt_random_get() & 0xFF;
    udp_sport = 0xC000 + (dnx_apt_random_get() & 0x3FFF);
    for (i = 0; i < 8; i++)
    {
        bfd_req_intervals[i] = dnx_apt_random_get();
        internal_priorities[i] = dnx_apt_random_get() & 0x1F;
    }
    for (i = 0; i < 16; i++)
    {
        pwe_ttl_exp[i] = dnx_apt_random_get() & 0x3FF;
    }

    for (iter = 0; iter < number_of_meps; iter++)
    {
        pEndpoint = &bfd_endpoint_info[iter].ep;
        bcm_bfd_endpoint_info_t_init(pEndpoint);

        pEndpoint->type = type_per_test[test_obj];
        pEndpoint->flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME;
        if (test_obj == dnxAPTBFDEndpointTestUdpIpv6ShopAcc)
        {
            pEndpoint->flags |= BCM_BFD_ENDPOINT_IPV6;
        }
        pEndpoint->flags2 |= flags_stats_supported[dnx_apt_random_get() & 0x3];
        if (test_obj == dnxAPTBFDEndpointTestMplsTpCCAcc)
        {
            /*
             * in lif - instead of your discriminator
             */
            pEndpoint->gport = tunnel_switch[dnx_apt_unique_random_get(iter)].tunnel_id;
        }
        else if (test_obj == dnxAPTBFDEndpointTestPWEControlWordAcc ||
                 test_obj == dnxAPTBFDEndpointTestPWERouterAlertAcc ||
                 test_obj == dnxAPTBFDEndpointTestPWETtlAcc || test_obj == dnxAPTBFDEndpointTestPWEGalAcc)
        {
            /*
             * in lif - instead of your discriminator
             */
            pEndpoint->gport = dnx_apt_vpls_basic_info.mpls_port_id_ingress[dnx_apt_unique_random_get(iter)];
        }

        BCM_GPORT_SYSTEM_PORT_ID_SET(pEndpoint->tx_gport, port_1);
        if (test_obj == dnxAPTBFDEndpointTestMplsIpv4ShopAcc)
        {
            pEndpoint->egress_if = tunnel_id;
        }
        else if (test_obj == dnxAPTBFDEndpointTestUdpIpv4ShopAcc || test_obj == dnxAPTBFDEndpointTestUdpIpv6ShopAcc
                 || test_obj == dnxAPTBFDEndpointTestMplsTpCCAcc)
        {
            pEndpoint->egress_if = ctest_bfd_egress_interface;
        }
        else if (test_obj == dnxAPTBFDEndpointTestPWEControlWordAcc ||
                 test_obj == dnxAPTBFDEndpointTestPWERouterAlertAcc ||
                 test_obj == dnxAPTBFDEndpointTestPWETtlAcc || test_obj == dnxAPTBFDEndpointTestPWEGalAcc)
        {
            pEndpoint->egress_if = dnx_apt_vpls_basic_info.core_arp_id;
        }
        pEndpoint->remote_gport = BCM_GPORT_INVALID;
        if (is_update)
        {
            sal_memcpy(&bfd_endpoint_info[iter + number_of_meps].ep, pEndpoint, sizeof(bcm_bfd_endpoint_info_t));
        }

        for (iter2 = 0; iter2 < nof_meps_in_loop; iter2++)
        {
            pEndpoint = iter2 ? (&bfd_endpoint_info[iter + number_of_meps].ep) : (&bfd_endpoint_info[iter].ep);

            pEndpoint->loc_clear_threshold = dnx_apt_random_get() % 3;
            pEndpoint->int_pri = internal_priorities[dnx_apt_random_get() % 8];

            if (test_obj == dnxAPTBFDEndpointTestMplsTpCCAcc ||
                test_obj == dnxAPTBFDEndpointTestMplsIpv4ShopAcc ||
                test_obj == dnxAPTBFDEndpointTestPWEControlWordAcc ||
                test_obj == dnxAPTBFDEndpointTestPWERouterAlertAcc ||
                test_obj == dnxAPTBFDEndpointTestPWETtlAcc || test_obj == dnxAPTBFDEndpointTestPWEGalAcc)
            {
                pEndpoint->label = mpls_label_termination;
                index = dnx_apt_random_get() & 0xF;
                pEndpoint->egress_label.ttl = pwe_ttl_exp[index] & 0xFF;
                pEndpoint->egress_label.exp = pwe_ttl_exp[index] >> 8;
            }

            if (test_obj == dnxAPTBFDEndpointTestUdpIpv4ShopAcc || test_obj == dnxAPTBFDEndpointTestMplsIpv4ShopAcc)
            {
                pEndpoint->ip_ttl = 1;
                pEndpoint->ip_tos = shop_tos;
                pEndpoint->src_ip_addr = src_ip_prefix | (dnx_apt_random_get() & 0xFF);
                pEndpoint->dst_ip_addr = 0x7fffff03;
                pEndpoint->udp_src_port = udp_sport;
            }
            else if (test_obj == dnxAPTBFDEndpointTestUdpIpv6ShopAcc)
            {
                pEndpoint->ip_ttl = 1;
                pEndpoint->ip_tos = shop_tos;
                sal_memcpy(pEndpoint->src_ip6_addr, src_ipv6_tmp, sizeof(bcm_ip6_t));
                sal_memcpy(pEndpoint->dst_ip6_addr, dst_ipv6_tmp, sizeof(bcm_ip6_t));
                pEndpoint->udp_src_port = udp_sport;
            }

            pEndpoint->bfd_period = bfd_tx_periods[dnx_apt_random_get() % 8];
            pEndpoint->bfd_period_cluster = 0;
            pEndpoint->bfd_detection_time = dnx_apt_random_get() % (1024 * 1000000);
            pEndpoint->local_min_tx = bfd_req_intervals[dnx_apt_random_get() & 0x7];
            pEndpoint->local_min_rx = bfd_req_intervals[dnx_apt_random_get() & 0x7];
            pEndpoint->local_state = dnx_apt_random_get() & 0x3;
            pEndpoint->local_flags = dnx_apt_random_get() & 0x3A;
            pEndpoint->local_detect_mult = dnx_apt_random_get() & 0xFF;
            pEndpoint->remote_detect_mult = dnx_apt_random_get() & 0xFF;
            pEndpoint->local_discr = dnx_apt_unique_random_get(iter) << 2;
            pEndpoint->remote_discr = dnx_apt_random_get();
            pEndpoint->faults |= BCM_BFD_ENDPOINT_REMOTE_LOC;

            if (test_obj == dnxAPTBFDEndpointTestUdpIpv6ShopAcc)
            {
                pEndpoint->ipv6_extra_data_index = 0x10000 + pEndpoint->local_discr;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_ipv4_example_init(unit, 200, 201));

    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc(unit, number_of_calls, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestUdpIpv4ShopAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 0));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_ipv6_example_init(unit, 200, 201));

    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc(unit, number_of_calls, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestUdpIpv6ShopAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 0));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_mpls_tp_cc_acc_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_mpls_tp_cc_example_init(unit, 200, 201));

    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc(unit, number_of_calls, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestMplsTpCCAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 0));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_mpls_example_init(unit, 200, 201));

    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc(unit, number_of_calls, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestMplsIpv4ShopAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 0));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pwe_control_word_acc_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_pwe_example_init(unit, 200, 201));

    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc(unit, number_of_calls, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestPWEControlWordAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 0));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pwe_router_alter_acc_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_pwe_example_init(unit, 200, 201));

    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc(unit, number_of_calls, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestPWERouterAlertAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 0));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pwe_ttl_acc_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_pwe_example_init(unit, 200, 201));

    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc(unit, number_of_calls, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestPWETtlAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 0));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pwe_gal_acc_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_pwe_example_init(unit, 200, 201));

    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc(unit, number_of_calls, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestPWEGalAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 0));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_update_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_ipv4_example_init(unit, 200, 201));

    /*
     * The first  half endpoints are used for updating
     * The second half endpoints are used for creating
     */
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc
                    (unit, number_of_calls * 2, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestUdpIpv4ShopAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 1));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_update_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_ipv6_example_init(unit, 200, 201));

    /*
     * The first  half endpoints are used for updating
     * The second half endpoints are used for creating
     */
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc
                    (unit, number_of_calls * 2, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestUdpIpv6ShopAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 1));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_mpls_tp_cc_example_init(unit, 200, 201));

    /*
     * The first  half endpoints are used for updating
     * The second half endpoints are used for creating
     */
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc
                    (unit, number_of_calls * 2, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestMplsTpCCAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 1));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_mpls_example_init(unit, 200, 201));

    /*
     * The first  half endpoints are used for updating
     * The second half endpoints are used for creating
     */
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc
                    (unit, number_of_calls * 2, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestMplsIpv4ShopAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 1));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pwe_control_word_acc_update_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_pwe_example_init(unit, 200, 201));

    /*
     * The first  half endpoints are used for updating
     * The second half endpoints are used for creating
     */
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc
                    (unit, number_of_calls * 2, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestPWEControlWordAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 1));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_pwe_example_init(unit, 200, 201));

    /*
     * The first  half endpoints are used for updating
     * The second half endpoints are used for creating
     */
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc
                    (unit, number_of_calls * 2, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestPWERouterAlertAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 1));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pwe_ttl_acc_update_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_pwe_example_init(unit, 200, 201));

    /*
     * The first  half endpoints are used for updating
     * The second half endpoints are used for creating
     */
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc
                    (unit, number_of_calls * 2, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestPWETtlAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 1));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pwe_gal_acc_update_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_bfd_pwe_example_init(unit, 200, 201));

    /*
     * The first  half endpoints are used for updating
     * The second half endpoints are used for creating
     */
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_alloc
                    (unit, number_of_calls * 2, (dnx_apt_bfd_endpoint_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_bfd_endpoint_generate
                    (unit, dnxAPTBFDEndpointTestPWEGalAcc, number_of_calls,
                     (dnx_apt_bfd_endpoint_info_t *) * custom_data, 1));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pre_update_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int start_index = DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS;
    int iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_bfd_endpoint_create
                        (unit, &((dnx_apt_bfd_endpoint_info_t *) custom_data)[start_index + iter].ep));
        /*
         * Store the endpoint id 
         */
        ((dnx_apt_bfd_endpoint_info_t *) custom_data)[iter].ep.id =
            ((dnx_apt_bfd_endpoint_info_t *) custom_data)[start_index + iter].ep.id;
        ((dnx_apt_bfd_endpoint_info_t *) custom_data)[iter].ep.remote_id =
            ((dnx_apt_bfd_endpoint_info_t *) custom_data)[start_index + iter].ep.remote_id;
        ((dnx_apt_bfd_endpoint_info_t *) custom_data)[iter].ep.flags |= BCM_BFD_ENDPOINT_UPDATE;
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_pre_destroy_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_bfd_endpoint_create(unit, &((dnx_apt_bfd_endpoint_info_t *) custom_data)[iter].ep));
    }
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_bfd_endpoint_create(unit, &((dnx_apt_bfd_endpoint_info_t *) custom_data)[call_iter].ep));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_destroy_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_bfd_endpoint_destroy(unit, ((dnx_apt_bfd_endpoint_info_t *) custom_data)[call_iter].ep.id));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_bfd_endpoint_destroy(unit, ((dnx_apt_bfd_endpoint_info_t *) custom_data)[iter].ep.id));
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * BFD Endpoint test deinit functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_bfd_endpoint_deinit(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * GLOBALs
 * {
 */

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_insert_object = {
    /** Test name */
    "BFD_ENDPOINT_UDP_IPV4_SIP_ACC_Insert",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when inserting UDP IPv4 Single Hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_insert_object = {
    /** Test name */
    "BFD_ENDPOINT_UDP_IPV6_SIP_ACC_Insert",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when inserting UDP IPv6 Single Hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_mpls_tp_cc_acc_insert_object = {
    /** Test name */
    "BFD_ENDPOINT_MPLS_TP_CC_ACC_Insert",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when inserting MPLS TP CC Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_insert_object = {
    /** Test name */
    "BFD_ENDPOINT_MPLS_IPV4_SHOP_ACC_Insert",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when inserting MPLS IPV4 Single hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_control_word_acc_insert_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_CONTROL_WORD_ACC_Insert",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when inserting PWE Control Word Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_router_alter_acc_insert_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_ROUTE_ALTER_ACC_Insert",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when inserting PWE Router Alter Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_ttl_acc_insert_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_TTL_ACC_Insert",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when inserting PWE TLL Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_gal_acc_insert_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_GAL_ACC_Insert",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when inserting PWE GAL Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_gal_acc_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_gal_acc_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_gal_acc_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_update_object = {
    /** Test name */
    "BFD_ENDPOINT_UDP_IPV4_SIP_ACC_Update",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when updating UDP IPv4 Single Hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_update_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_update_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_update_object = {
    /** Test name */
    "BFD_ENDPOINT_UDP_IPV6_SIP_ACC_Update",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when updating UDP IPv6 Single Hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_update_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_update_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_object = {
    /** Test name */
    "BFD_ENDPOINT_MPLS_TP_CC_ACC_Update",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when updating MPLS TP CC Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_update_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_update_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_object = {
    /** Test name */
    "BFD_ENDPOINT_MPLS_IPV4_SHOP_ACC_Update",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when updating MPLS IPV4 Single hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_update_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_update_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_control_word_acc_update_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_CONTROL_WORD_ACC_Update",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when updating PWE Control Word Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_update_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_update_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_ROUTE_ALTER_ACC_Update",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when updating PWE Router Alter Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_update_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_update_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_ttl_acc_update_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_TTL_ACC_Update",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when updating PWE TLL Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_update_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_update_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_gal_acc_update_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_GAL_ACC_Update",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when updating PWE GAL Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_gal_acc_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_gal_acc_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_gal_acc_update_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_update_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_add_exec,
    /** Run deinit procedure */
    dnx_apt_bfd_endpoint_clear_exec,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_destroy_object = {
    /** Test name */
    "BFD_ENDPOINT_UDP_IPV4_SIP_ACC_Destroy",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when destroying UDP IPv4 Single Hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_udp_ipv4_shop_acc_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_udp_ipv4_sip_acc_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_destroy_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_destroy_object = {
    /** Test name */
    "BFD_ENDPOINT_UDP_IPV6_SIP_ACC_Destroy",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when destroying UDP IPv6 Single Hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_udp_ipv6_shop_acc_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_udp_ipv6_sip_acc_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_destroy_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_mpls_tp_cc_acc_destroy_object = {
    /** Test name */
    "BFD_ENDPOINT_MPLS_TP_CC_ACC_Destroy",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when destroying MPLS TP CC Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_mpls_tp_cc_acc_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_destroy_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_destroy_object = {
    /** Test name */
    "BFD_ENDPOINT_MPLS_IPV4_SHOP_ACC_Destroy",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when destroying MPLS IPV4 Single hop Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_mpls_ipv4_shop_acc_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_destroy_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_control_word_acc_destroy_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_CONTROL_WORD_ACC_Destroy",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when destroying PWE Control Word Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_control_word_acc_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_destroy_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_router_alter_acc_destroy_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_ROUTE_ALTER_ACC_Destroy",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when destroying PWE Router Alter Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_router_alter_acc_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_destroy_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_ttl_acc_destroy_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_TTL_ACC_Destroy",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when destroying PWE TLL Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_ttl_acc_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_destroy_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

const dnx_apt_object_def_t dnx_apt_bfd_endpoint_pwe_gal_acc_destroy_object = {
    /** Test name */
    "BFD_ENDPOINT_PWE_GAL_ACC_Destroy",
    /** Test description */
    "Measures the performance of bcm_bfd_endpoint_create() when destroying PWE GAL Acc entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_BFD_ENDPOINT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_bfd_endpoint_pwe_gal_acc_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_bfd_endpoint_pwe_gal_acc_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_bfd_endpoint_pwe_gal_acc_init,
    /** Run init procedure */
    dnx_apt_bfd_endpoint_pre_destroy_exec,
    /** Test execution procedure */
    dnx_apt_bfd_endpoint_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_bfd_endpoint_deinit
};

/*
 * }
 */
