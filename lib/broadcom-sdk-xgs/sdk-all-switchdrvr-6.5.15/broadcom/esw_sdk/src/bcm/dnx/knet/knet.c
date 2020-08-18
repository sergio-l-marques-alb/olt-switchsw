/*
 * $Id: knet.c,v 1.0 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        knet.c
 * Purpose:     Kernel Networking Management
 *
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_KNET

#include <bcm/error.h>
#include <bcm/knet.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm/trunk.h>
#include <bcm/stack.h>
#include <bcm/switch.h>
#include <soc/drv.h>
#include <soc/knet.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/shared/dcbformats/type36.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_trunk.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

#ifdef INCLUDE_KNET

/** DNX Meta data */
typedef struct dnx_meta_data_s
{
#ifdef LE_HOST
    uint32 trap_qualifier:16, trap_id:16;
#else
    uint32 trap_id:16, trap_qualifier:16;
#endif
#ifdef LE_HOST
    uint32 forward_domain:16,   /* Bridging: VSI, Routing: VRF, FCoE: VFT, etc */
        spa:16;                 /* Source-system-port-aggregate */
#else
    uint32 spa:16,              /* Source-system-port-aggregate */
        forward_domain:16;      /* Bridging: VSI, Routing: VRF, FCoE: VFT, etc */
#endif
#ifdef  LE_HOST
    uint32 count:16,            /* Transferred byte count */
        end:1,                  /* End bit (RX) */
        start:1,                /* Start bit (RX) */
        error:1,                /* Cell Error (RX) */
        ecc_error:1,            /* packet ECC Error (RX) */
    :   11,                     /* Reserved */
        done:1;                 /* Descriptor Done */
#else
    uint32 done:1,              /* Descriptor Done */
    :   11,                     /* Reserved */
        ecc_error:1,            /* Packet ECC Error (RX) */
        error:1,                /* Cell Error (RX) */
        start:1,                /* Start bit (RX) */
        end:1,                  /* End bit (RX) */
        count:16;               /* Transferred byte count */
#endif
} dnx_meta_data_t;

/*
 * ITMH base header
 */
typedef union dnx_knet_itmh_base_u
{
    struct
    {
        uint8 bytes[5];
    } raw;
    struct
    {
#if defined(LE_HOST)
        unsigned inbound_mirror_disable:1;
        unsigned pph_type:2;
        unsigned injected_as_extension_present:1;
        unsigned forward_action_strength:3;
        unsigned reserved:1;
        unsigned destination_hi:6;
        unsigned drop_precedence:2;
        unsigned destination_mi:8;
        unsigned snoop_profile_hi:1;
        unsigned destination_lo:7;
        unsigned itmh_base_ext:1;
        unsigned traffic_class:3;
        unsigned snoop_profile_lo:4;
#else
        unsigned reserved:1;
        unsigned forward_action_strength:3;
        unsigned injected_as_extension_present:1;
        unsigned pph_type:2;
        unsigned inbound_mirror_disable:1;
        unsigned drop_precedence:2;
        unsigned destination_hi:6;
        unsigned destination_mi:8;
        unsigned destination_lo:7;
        unsigned snoop_profile_hi:1;
        unsigned snoop_profile_lo:4;
        unsigned traffic_class:3;
        unsigned itmh_base_ext:1;
#endif
    } jr2_common;

    struct
    {
        unsigned _rsvd_0:8;
#if defined(LE_HOST)
        unsigned destination_hi:6;
        unsigned _rsvd_1:2;
        unsigned destination_mi:8;
        unsigned _rsvd_2:1;
        unsigned destination_lo:7;
#else
        unsigned _rsvd_1:2;
        unsigned destination_hi:6;
        unsigned destination_mi:8;
        unsigned destination_lo:7;
        unsigned _rsvd_2:1;
#endif
        unsigned _rsvd_3:8;
    } jr2_dest_info;
} dnx_knet_itmh_base_t;

/* PTCH_2 */
#define DNX_KNET_PTCH_2_SIZE          2
/* ITMH */
#define DNX_KNET_ITMH_SIZE            5
/* Modlue Header */
#define DNX_KNET_MODULE_HEADER_SIZE   16

/**
 * \brief - Get the switch header type of in port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] incoming_header_type - Switch port header type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_knet_switch_header_type_in_get(
    int unit,
    bcm_port_t port,
    int *incoming_header_type)
{
    dbal_enum_value_field_port_termination_ptc_profile_e prt_profile;
    dbal_enum_value_field_ingress_port_termination_profile_e llr_profile;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(incoming_header_type, _SHR_E_PARAM, "incoming_header_type");

    /** Get the Prt ptc profile */
    SHR_IF_ERR_EXIT(dnx_port_ptc_profile_get(unit, port, &prt_profile, &llr_profile));

    switch (prt_profile)
    {
        case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET:
        case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_COE:
        {
            *incoming_header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
            break;
        }
        case DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2:
        {
            if (SOC_PBMP_MEMBER(PBMP_E_ALL(unit), port))
            {
                *incoming_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
            }
            else
            {
                *incoming_header_type = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2;
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid prt_profile =%d\n", prt_profile);
        }
    }

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Create a kernel network interface.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_create(
    int unit,
    bcm_knet_netif_t * netif)
{
    kcom_msg_netif_create_t netif_create;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t default_ssp = 0;
    bcm_port_t ptch_ssp;
    bcm_core_t ptch_ssp_core;
    int header_type_incoming;
    uint8 *system_headers;
    int cpu_channel;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_NULL_CHECK(netif, _SHR_E_PARAM, "netif");

    sal_memset(&netif_create, 0, sizeof(netif_create));
    netif_create.hdr.opcode = KCOM_M_NETIF_CREATE;
    netif_create.hdr.unit = unit;

    switch (netif->type)
    {
        case BCM_KNET_NETIF_T_TX_CPU_INGRESS:
            netif_create.netif.type = KCOM_NETIF_T_VLAN;
            break;
        case BCM_KNET_NETIF_T_TX_LOCAL_PORT:
            netif_create.netif.type = KCOM_NETIF_T_PORT;
            break;
        case BCM_KNET_NETIF_T_TX_META_DATA:
            netif_create.netif.type = KCOM_NETIF_T_META;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported interface type\n");
    }

    if (netif->flags & BCM_KNET_NETIF_F_ADD_TAG)
    {
        netif_create.netif.flags |= KCOM_NETIF_F_ADD_TAG;
    }
    if (netif->flags & BCM_KNET_NETIF_F_RCPU_ENCAP)
    {
        netif_create.netif.flags |= KCOM_NETIF_F_RCPU_ENCAP;
    }
    if (netif->flags & BCM_KNET_NETIF_F_KEEP_RX_TAG)
    {
        netif_create.netif.flags |= KCOM_NETIF_F_KEEP_RX_TAG;
    }

    netif_create.netif.vlan = netif->vlan;
    netif_create.netif.port = netif->port;

    /** Get PP Port of configured port */
    if (netif->type == BCM_KNET_NETIF_T_TX_CPU_INGRESS)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, netif->port, &flags, &interface_info, &mapping_info));
    }
    else if (netif->type == BCM_KNET_NETIF_T_TX_LOCAL_PORT)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, default_ssp, &flags, &interface_info, &mapping_info));
    }

    ptch_ssp = mapping_info.pp_port;
    ptch_ssp_core = mapping_info.core;

    /** Check if psrc is trunk */
    if (ptch_ssp == 0xFFFFFFFF)
    {
        int modid;
        int sysport;
        bcm_gport_t gport;
        bcm_trunk_t tid_find;
        bcm_trunk_pp_port_allocation_info_t allocation_info;

        /** Get device modid */
        SHR_IF_ERR_EXIT(bcm_stk_modid_get(unit, &modid));
        /** Switch local to sysport */
        SHR_IF_ERR_EXIT(appl_dnx_logical_port_to_sysport_get(unit, modid, netif->port, &sysport));
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, sysport);
        /** Get trunk id from sysport */
        SHR_IF_ERR_EXIT(bcm_trunk_find(unit, modid, gport, &tid_find));
        /** Get the pp port allocation info of a given trunk */
        SHR_IF_ERR_EXIT(bcm_trunk_pp_port_allocation_get(unit, tid_find, 0, &allocation_info));
        /** Check that trunk has a core on this member port */
        if ((SAL_BIT(ptch_ssp_core) & allocation_info.core_bitmap) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Source port %d is not a valid member of Trunk %d\n", netif->port, tid_find);
        }
        else
        {
            ptch_ssp = allocation_info.pp_port_per_core_array[ptch_ssp_core];
        }
    }

    SHR_IF_ERR_EXIT(dnx_knet_switch_header_type_in_get(unit, ptch_ssp, &header_type_incoming));
    system_headers = netif_create.netif.system_headers;
    sal_memset(system_headers, 0, sizeof(netif_create.netif.system_headers));
    /** Zero system_headers_size */
    netif_create.netif.system_headers_size = 0;

    if ((header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP) ||
        (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2))
    {
        /** In tm port is always same as in pp port */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, ptch_ssp, &cpu_channel));
        /** Set cpu channel to first byte of Module Header*/
        system_headers[0] = cpu_channel;
        netif_create.netif.system_headers_size = DNX_KNET_MODULE_HEADER_SIZE;
        if (netif->type == BCM_KNET_NETIF_T_TX_CPU_INGRESS)
        {
            /** Append PTCH */
            system_headers[DNX_KNET_MODULE_HEADER_SIZE] = 0xd0;
            system_headers[DNX_KNET_MODULE_HEADER_SIZE + 1] = ptch_ssp & 0xff;
            netif_create.netif.system_headers_size += DNX_KNET_PTCH_2_SIZE;
        }
        else if (netif->type == BCM_KNET_NETIF_T_TX_LOCAL_PORT)
        {
            dnx_knet_itmh_base_t *itmh;
            /** Append PTCH: Parser-Program-Control = 0, indicate next header is ITMH */
            system_headers[DNX_KNET_MODULE_HEADER_SIZE] = 0x50;
            system_headers[DNX_KNET_MODULE_HEADER_SIZE + 1] = 0x0;
            /** ITMH */
            itmh = (dnx_knet_itmh_base_t *) & system_headers[DNX_KNET_MODULE_HEADER_SIZE + DNX_KNET_PTCH_2_SIZE];
            /** 01100(5) + System_Port_Agg(16) */
            itmh->jr2_dest_info.destination_hi = 0x18 | ((netif->port >> 15) & 0x1);
            itmh->jr2_dest_info.destination_mi = (netif->port >> 7) & 0xff;
            itmh->jr2_dest_info.destination_lo = netif->port & 0x7f;
            netif_create.netif.system_headers_size += DNX_KNET_PTCH_2_SIZE;
            netif_create.netif.system_headers_size += DNX_KNET_ITMH_SIZE;
        }
    }

    sal_memcpy(netif_create.netif.macaddr, netif->mac_addr, 6);
    sal_memcpy(netif_create.netif.name, netif->name, sizeof(netif_create.netif.name) - 1);
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & netif_create, sizeof(netif_create), sizeof(netif_create)));
    /** ID and interface name are assigned by kernel */
    netif->id = netif_create.netif.id;
    sal_memcpy(netif->name, netif_create.netif.name, sizeof(netif->name) - 1);
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Destroy a kernel network interface.
 *
 * \param [in] unit - Relevant unit
 * \param [in] netif_id - Network interface ID
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_destroy(
    int unit,
    int netif_id)
{
    kcom_msg_netif_destroy_t netif_destroy;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    sal_memset(&netif_destroy, 0, sizeof(netif_destroy));
    netif_destroy.hdr.opcode = KCOM_M_NETIF_DESTROY;
    netif_destroy.hdr.unit = unit;
    netif_destroy.hdr.id = netif_id;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & netif_destroy, sizeof(netif_destroy), sizeof(netif_destroy)));
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get a kernel network interface configuration.
 *
 * \param [in] unit - Relevant unit
 * \param [in] netif_id - Network interface ID
 * \param [out] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_get(
    int unit,
    int netif_id,
    bcm_knet_netif_t * netif)
{
    kcom_msg_netif_get_t netif_get;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_NULL_CHECK(netif, _SHR_E_PARAM, "netif");
    sal_memset(&netif_get, 0, sizeof(netif_get));
    netif_get.hdr.opcode = KCOM_M_NETIF_GET;
    netif_get.hdr.unit = unit;
    netif_get.hdr.id = netif_id;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & netif_get, sizeof(netif_get.hdr), sizeof(netif_get)));
    bcm_knet_netif_t_init(netif);
    switch (netif_get.netif.type)
    {
        case KCOM_NETIF_T_VLAN:
            netif->type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
            break;
        case KCOM_NETIF_T_PORT:
            netif->type = BCM_KNET_NETIF_T_TX_LOCAL_PORT;
            break;
        case KCOM_NETIF_T_META:
            netif->type = BCM_KNET_NETIF_T_TX_META_DATA;
            break;
        default:
            /*
             * Unknown type - defaults to VLAN
             */
            break;
    }

    if (netif_get.netif.flags & KCOM_NETIF_F_ADD_TAG)
    {
        netif->flags |= BCM_KNET_NETIF_F_ADD_TAG;
    }
    if (netif_get.netif.flags & KCOM_NETIF_F_RCPU_ENCAP)
    {
        netif->flags |= BCM_KNET_NETIF_F_RCPU_ENCAP;
    }
    if (netif_get.netif.flags & KCOM_NETIF_F_KEEP_RX_TAG)
    {
        netif->flags |= BCM_KNET_NETIF_F_KEEP_RX_TAG;
    }

    netif->id = netif_get.netif.id;
    netif->vlan = netif_get.netif.vlan;
    netif->port = netif_get.netif.port;
    sal_memcpy(netif->mac_addr, netif_get.netif.macaddr, 6);
    sal_memcpy(netif->name, netif_get.netif.name, sizeof(netif->name) - 1);
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverse kernel network interface objects.
 *
 * \param [in] unit - Relevant unit
 * \param [in] trav_fn - User provided call back function
 * \param [out] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_traverse(
    int unit,
    bcm_knet_netif_traverse_cb trav_fn,
    void *user_data)
{
    int idx;
    bcm_knet_netif_t netif;
    kcom_msg_netif_list_t netif_list;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "KNET: trav_fn is NULL");
    sal_memset(&netif_list, 0, sizeof(netif_list));
    netif_list.hdr.opcode = KCOM_M_NETIF_LIST;
    netif_list.hdr.unit = unit;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & netif_list, sizeof(netif_list.hdr), sizeof(netif_list)));
    for (idx = 0; idx < netif_list.ifcnt; idx++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_knet_netif_get(unit, netif_list.id[idx], &netif));
        SHR_IF_ERR_EXIT(trav_fn(unit, &netif, user_data));
    }

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Create a kernel packet filter.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_create(
    int unit,
    bcm_knet_filter_t * filter)
{
    int idx, pdx;
    int oob_size;
    int data_offset;
    kcom_msg_filter_create_t filter_create;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_NULL_CHECK(filter, _SHR_E_PARAM, "filter");
    sal_memset(&filter_create, 0, sizeof(filter_create));
    filter_create.hdr.opcode = KCOM_M_FILTER_CREATE;
    filter_create.hdr.unit = unit;
    filter_create.filter.type = KCOM_FILTER_T_RX_PKT;
    switch (filter->dest_type)
    {
        case BCM_KNET_DEST_T_NULL:
            filter_create.filter.dest_type = KCOM_DEST_T_NULL;
            break;
        case BCM_KNET_DEST_T_NETIF:
            filter_create.filter.dest_type = KCOM_DEST_T_NETIF;
            break;
        case BCM_KNET_DEST_T_BCM_RX_API:
            filter_create.filter.dest_type = KCOM_DEST_T_API;
            break;
        case BCM_KNET_DEST_T_CALLBACK:
            filter_create.filter.dest_type = KCOM_DEST_T_CB;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported destination type\n");
    }

    switch (filter->mirror_type)
    {
        case BCM_KNET_DEST_T_NULL:
            filter_create.filter.mirror_type = KCOM_DEST_T_NULL;
            break;
        case BCM_KNET_DEST_T_NETIF:
            filter_create.filter.mirror_type = KCOM_DEST_T_NETIF;
            break;
        case BCM_KNET_DEST_T_BCM_RX_API:
            filter_create.filter.mirror_type = KCOM_DEST_T_API;
            break;
        case BCM_KNET_DEST_T_CALLBACK:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Cannot mirror to callback\n");
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported mirror type\n");
    }

    if (filter->flags & BCM_KNET_FILTER_F_STRIP_TAG)
    {
        filter_create.filter.flags |= KCOM_FILTER_F_STRIP_TAG;
    }

    filter_create.filter.dest_id = filter->dest_id;
    filter_create.filter.dest_proto = filter->dest_proto;
    filter_create.filter.mirror_id = filter->mirror_id;
    filter_create.filter.mirror_proto = filter->mirror_proto;
    filter_create.filter.priority = filter->priority;
    sal_strncpy(filter_create.filter.desc, filter->desc, KCOM_FILTER_DESC_MAX);
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported filter to match source module port\n");
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported filter to match source module ID\n");
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_REASON)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported filter to match rx reason\n");
    }

    oob_size = 0;
    filter_create.filter.oob_data_offset = 0;
    if (filter->match_flags & ~BCM_KNET_FILTER_M_RAW)
    {
        oob_size = sizeof(dnx_meta_data_t);
        filter_create.filter.oob_data_size = oob_size;
    }

    /*
     * Create inverted mask
     */
    for (idx = 0; idx < oob_size; idx++)
    {
        filter_create.filter.mask.b[idx] = 0xff;
    }

    if (SOC_IS_DNX(unit))
    {
        dnx_meta_data_t *meta_data = (dnx_meta_data_t *) & filter_create.filter.data;
        dnx_meta_data_t *meta_mask = (dnx_meta_data_t *) & filter_create.filter.mask;
        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN)
        {
            meta_data->forward_domain = filter->m_vlan;
            meta_mask->forward_domain = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
        {
            uint32 spa;
            uint32 spa_mask = 0x0;
            if (BCM_GPORT_IS_TRUNK(filter->m_ingport))
            {
                uint32 members_nof_bits;
                int spa_member_shift;
                uint32 trunk_id;
                int pool;
                /** Convert trunk gport to system port aggregate id */
                SHR_IF_ERR_EXIT(bcm_dnx_trunk_gport_to_spa(unit, filter->m_ingport, 0, &spa));
                trunk_id = BCM_GPORT_TRUNK_GET(filter->m_ingport);
                /** Get pool from trunk gport */
                BCM_TRUNK_ID_POOL_GET(pool, trunk_id);
                /** Mask out lag member from SPA */
                members_nof_bits = dnx_data_trunk.parameters.pool_info_get(unit, pool)->pool_hw_mode;
                spa_member_shift = 13 - members_nof_bits;
                SHR_BITSET_RANGE(&spa_mask, spa_member_shift, members_nof_bits);
                meta_data->spa = spa;
                meta_mask->spa = spa_mask;
            }
            else
            {
                uint32 flags;
                bcm_port_interface_info_t interface_info;
                bcm_port_mapping_info_t mapping_info;

                /** If system port decode port from gport */
                if (BCM_GPORT_IS_SYSTEM_PORT(filter->m_ingport))
                {
                    filter->m_ingport = BCM_GPORT_SYSTEM_PORT_ID_GET(filter->m_ingport);
                }

                /** Get PP Port */
                SHR_IF_ERR_EXIT(bcm_port_get(unit, filter->m_ingport, &flags, &interface_info, &mapping_info));
                /** Check if is member of trunk */
                if (mapping_info.pp_port == 0xFFFFFFFF)
                {
                    int modid;
                    int sysport;
                    bcm_gport_t gport;
                    bcm_trunk_t tid_find;
                    bcm_trunk_info_t trunk_info;
                    int max_member_count = 256;
                    bcm_trunk_member_t trunk_port_members[256];
                    int member_count;
                    int index;
                    bcm_gport_t trunk_gport;
                    uint32 lag_member = 0;
                    /** Get device modid */
                    SHR_IF_ERR_EXIT(bcm_stk_modid_get(unit, &modid));
                    /** Switch local to sysport */
                    SHR_IF_ERR_EXIT(appl_dnx_logical_port_to_sysport_get(unit, modid, filter->m_ingport, &sysport));
                    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, sysport);
                    /** Get trunk id from sysport */
                    SHR_IF_ERR_EXIT(bcm_trunk_find(unit, modid, gport, &tid_find));
                    SHR_IF_ERR_EXIT(bcm_trunk_get
                                    (unit, tid_find, &trunk_info, max_member_count, trunk_port_members, &member_count));
                    /** Get the pp port allocation info of a given trunk */
                    for (index = 0; index < member_count; index++)
                    {
                        if (gport == trunk_port_members[index].gport)
                        {
                            lag_member = index;
                            break;
                        }
                    }

                    BCM_GPORT_TRUNK_SET(trunk_gport, tid_find);
                    SHR_IF_ERR_EXIT(bcm_dnx_trunk_gport_to_spa(unit, trunk_gport, lag_member, &spa));
                    meta_data->spa = spa;
                    meta_mask->spa = spa_mask;
                }
                else
                {
                    meta_data->spa = mapping_info.pp_port;
                    meta_mask->spa = spa_mask;
                }
            }
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE)
        {
            meta_data->trap_id = (filter->m_fp_rule >> 16) & 0xffff;
            meta_data->trap_qualifier = filter->m_fp_rule & 0xffff;
            meta_mask->trap_id = 0;
            meta_mask->trap_qualifier = 0;
            if (meta_data->trap_id)
            {
                meta_mask->trap_id = 0;
            }
            if (meta_data->trap_qualifier)
            {
                meta_mask->trap_qualifier = 0;
            }

        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR)
        {
            meta_data->error = 1;
            meta_mask->error = 0;
        }

        /*
         * Invert inverted mask
         */
        for (idx = 0; idx < oob_size; idx++)
        {
            filter_create.filter.mask.b[idx] ^= 0xff;
        }

        if (filter->match_flags & BCM_KNET_FILTER_M_RAW)
        {
            data_offset = 0;
            for (pdx = 0; pdx < filter->raw_size; pdx++)
            {
                if (filter->m_raw_mask[pdx] != 0)
                {
                    data_offset = pdx;
                    break;
                }
            }
            idx = oob_size;
            for (; pdx < filter->raw_size; pdx++)
            {
                /*
                 * Check for array overflow
                 */
                if (idx >= KCOM_FILTER_BYTES_MAX)
                {
                    return BCM_E_PARAM;
                }
                filter_create.filter.data.b[idx] = filter->m_raw_data[pdx];
                filter_create.filter.mask.b[idx] = filter->m_raw_mask[pdx];
                idx++;
            }
            filter_create.filter.pkt_data_offset = data_offset;
            filter_create.filter.pkt_data_size = filter->raw_size - data_offset;
        }
    }

    /*
     * Dump raw data for debugging purposes
     */
    for (idx = 0; idx < BYTES2WORDS(oob_size); idx++)
    {
        LOG_VERBOSE_EX(BSL_LOG_MODULE,
                       "OOB[%d]: 0x%08x [0x%08x]\n%s",
                       idx, filter_create.filter.data.w[idx], filter_create.filter.mask.w[idx], EMPTY);
    }
    for (idx = 0; idx < filter_create.filter.pkt_data_size; idx++)
    {
        LOG_VERBOSE_EX(BSL_LOG_MODULE,
                       "PKT[%d]: 0x%02x [0x%02x]\n%s",
                       idx + filter_create.filter.pkt_data_offset,
                       filter_create.filter.data.b[idx + oob_size], filter_create.filter.mask.b[idx + oob_size], EMPTY);
    }

    if (SOC_IS_DNX(unit) && (!SOC_WARM_BOOT(unit)))
    {
        int index;
        uint32 entry_handle_id;

        /** Read LB_KEY SIZE, Stacking size ,internal header base size */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE,
                                   &filter_create.filter.ftmh_lb_key_ext_size);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                                   &filter_create.filter.ftmh_stacking_ext_size);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE,
                                   &filter_create.filter.pph_base_size);
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_USER_HEADER_ENABLE, INST_SINGLE,
                                  &filter_create.filter.udh_enable);
        /*
         * 0x0: No LIF extension
         */
        filter_create.filter.pph_lif_ext_size[0] = 0;
        /** Read 1xOutLIF, 2xOutLIF, 3xOutLIF extension header size */
        for (index = 0; index < 3; index++)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, index,
                                       &filter_create.filter.pph_lif_ext_size[index + 1]);
        }
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * 0x4: InLIF + InLIF-Profile
         */
        filter_create.filter.pph_lif_ext_size[4] = 0;
        /*
         * 0x5-0x7: Reserved
         */
        filter_create.filter.pph_lif_ext_size[5] = 0;
        filter_create.filter.pph_lif_ext_size[6] = 0;
        filter_create.filter.pph_lif_ext_size[7] = 0;

        filter_create.filter.udh_length_type[0] = dnx_data_field.udh.type_0_length_get(unit);
        filter_create.filter.udh_length_type[1] = dnx_data_field.udh.type_1_length_get(unit);
        filter_create.filter.udh_length_type[2] = dnx_data_field.udh.type_2_length_get(unit);
        filter_create.filter.udh_length_type[3] = dnx_data_field.udh.type_3_length_get(unit);
    }

    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_create, sizeof(filter_create), sizeof(filter_create)));
    /*
     * ID is assigned by kernel
     */
    filter->id = filter_create.filter.id;
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Destroy a kernel packet filter.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter_id - Rx packet filter ID
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_destroy(
    int unit,
    int filter_id)
{
    kcom_msg_filter_destroy_t filter_destroy;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    sal_memset(&filter_destroy, 0, sizeof(filter_destroy));
    filter_destroy.hdr.opcode = KCOM_M_FILTER_DESTROY;
    filter_destroy.hdr.unit = unit;
    filter_destroy.hdr.id = filter_id;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_destroy, sizeof(filter_destroy), sizeof(filter_destroy)));
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get a kernel packet filter configuration.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter_id - Rx packet filter ID
 * \param [out] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_get(
    int unit,
    int filter_id,
    bcm_knet_filter_t * filter)
{
    kcom_msg_filter_get_t filter_get;
    int idx, rdx, fdx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    sal_memset(&filter_get, 0, sizeof(filter_get));
    filter_get.hdr.opcode = KCOM_M_FILTER_GET;
    filter_get.hdr.unit = unit;
    filter_get.hdr.id = filter_id;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_get, sizeof(filter_get.hdr), sizeof(filter_get)));
    bcm_knet_filter_t_init(filter);
    switch (filter_get.filter.type)
    {
        case KCOM_FILTER_T_RX_PKT:
            filter->type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        default:
            /*
             * Unknown type
             */
            break;
    }

    switch (filter_get.filter.dest_type)
    {
        case KCOM_DEST_T_NETIF:
            filter->dest_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->dest_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
            filter->dest_type = BCM_KNET_DEST_T_CALLBACK;
            break;
        default:
            filter->dest_type = BCM_KNET_DEST_T_NULL;
            break;
    }

    switch (filter_get.filter.mirror_type)
    {
        case KCOM_DEST_T_NETIF:
            filter->mirror_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->mirror_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
            /*
             * Should never get here, but keep for completeness
             */
            filter->mirror_type = BCM_KNET_DEST_T_CALLBACK;
            break;
        default:
            filter->mirror_type = BCM_KNET_DEST_T_NULL;
            break;
    }

    if (filter_get.filter.flags & KCOM_FILTER_F_STRIP_TAG)
    {
        filter->flags |= BCM_KNET_FILTER_F_STRIP_TAG;
    }

    filter->dest_id = filter_get.filter.dest_id;
    filter->dest_proto = filter_get.filter.dest_proto;
    filter->mirror_id = filter_get.filter.mirror_id;
    filter->mirror_proto = filter_get.filter.mirror_proto;
    filter->id = filter_get.filter.id;
    filter->priority = filter_get.filter.priority;
    sal_memcpy(filter->desc, filter_get.filter.desc, sizeof(filter->desc) - 1);

    /** There is criterion in meta data */
    if (filter_get.filter.oob_data_size)
    {
        dnx_meta_data_t *meta_data;
        dnx_meta_data_t *meta_mask;
        meta_data = (dnx_meta_data_t *) & filter_get.filter.data;
        meta_mask = (dnx_meta_data_t *) & filter_get.filter.mask;

        if (meta_mask->forward_domain)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_VLAN;
            filter->m_vlan = meta_data->forward_domain;
        }
        if (meta_mask->spa)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_INGPORT;
            filter->m_ingport = meta_data->spa;
        }
        filter->m_fp_rule = 0;
        if (meta_mask->trap_id)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_FP_RULE;
            filter->m_fp_rule |= (meta_data->trap_id & 0xffff) << 16;
        }
        if (meta_mask->trap_qualifier)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_FP_RULE;
            filter->m_fp_rule |= meta_data->trap_qualifier & 0xffff;
        }
        if (meta_mask->error)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_ERROR;
        }
    }

    if (filter_get.filter.pkt_data_size)
    {
        filter->match_flags |= BCM_KNET_FILTER_M_RAW;
        rdx = filter_get.filter.pkt_data_offset;
        fdx = filter_get.filter.oob_data_size;
        for (idx = 0; idx < filter_get.filter.pkt_data_size; idx++)
        {
            filter->m_raw_data[rdx] = filter_get.filter.data.b[fdx];
            filter->m_raw_mask[rdx] = filter_get.filter.mask.b[fdx];
            rdx++;
            fdx++;
        }
        filter->raw_size = rdx;
    }

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverse kernel packet filter objects.
 *
 * \param [in] unit - Relevant unit
 * \param [in] trav_fn - User provided call back function
 * \param [in] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_traverse(
    int unit,
    bcm_knet_filter_traverse_cb trav_fn,
    void *user_data)
{
    int idx;
    bcm_knet_filter_t filter;
    kcom_msg_filter_list_t filter_list;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "KNET: trav_fn is NULL\n");
    sal_memset(&filter_list, 0, sizeof(filter_list));
    filter_list.hdr.opcode = KCOM_M_FILTER_LIST;
    filter_list.hdr.unit = unit;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_list, sizeof(filter_list.hdr), sizeof(filter_list)));
    for (idx = 0; idx < filter_list.fcnt; idx++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_knet_filter_get(unit, filter_list.id[idx], &filter));
        SHR_IF_ERR_EXIT(trav_fn(unit, &filter, user_data));
    }

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Call back function to delete packet filters.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter - Rx packet filter ID
 * \param [in] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
STATIC int
dnx_trav_filter_clean(
    int unit,
    bcm_knet_filter_t * filter,
    void *user_data)
{
    return bcm_dnx_knet_filter_destroy(unit, filter->id);
}

/**
 * \brief Clean up the kernel networking subsystem.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_cleanup(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_knet_filter_traverse(unit, dnx_trav_filter_clean, NULL));
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Initialize the kernel networking subsystem.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_init(
    int unit)
{
    bcm_knet_filter_t filter;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_knet_cleanup(unit));
    bcm_knet_filter_t_init(&filter);
    filter.type = BCM_KNET_FILTER_T_RX_PKT;
    filter.dest_type = BCM_KNET_DEST_T_BCM_RX_API;
    filter.priority = 255;
    sal_strncpy(filter.desc, "DefaultRxAPI", KCOM_FILTER_DESC_MAX);
    SHR_IF_ERR_EXIT(bcm_dnx_knet_filter_create(unit, &filter));
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}
#else
/**
 * \brief Create a kernel network interface.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_create(
    int unit,
    bcm_knet_netif_t * netif)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_netif_create is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Destroy a kernel network interface.
 *
 * \param [in] unit - Relevant unit
 * \param [in] netif_id - Network interface ID
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_destroy(
    int unit,
    int netif_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_netif_destroy is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get a kernel network interface configuration.
 *
 * \param [in] unit - Relevant unit
 * \param [in] netif_id - Network interface ID
 * \param [out] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_get(
    int unit,
    int netif_id,
    bcm_knet_netif_t * netif)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_netif_get is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverse kernel network interface objects.
 *
 * \param [in] unit - Relevant unit
 * \param [in] trav_fn - User provided call back function
 * \param [out] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_traverse(
    int unit,
    bcm_knet_netif_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_netif_traverse is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Create a kernel packet filter.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_create(
    int unit,
    bcm_knet_filter_t * filter)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_filter_create is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Destroy a kernel packet filter.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter_id - Rx packet filter ID
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_destroy(
    int unit,
    int filter_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_filter_destroy is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get a kernel packet filter configuration.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter_id - Rx packet filter ID
 * \param [out] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_get(
    int unit,
    int filter_id,
    bcm_knet_filter_t * filter)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_filter_get is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverse kernel packet filter objects.
 *
 * \param [in] unit - Relevant unit
 * \param [in] trav_fn - User provided call back function
 * \param [in] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_traverse(
    int unit,
    bcm_knet_filter_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_filter_traverse is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Clean up the kernel networking subsystem.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_cleanup(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_cleanup is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Initialize the kernel networking subsystem.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_init is unavailable\n");
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

#endif
