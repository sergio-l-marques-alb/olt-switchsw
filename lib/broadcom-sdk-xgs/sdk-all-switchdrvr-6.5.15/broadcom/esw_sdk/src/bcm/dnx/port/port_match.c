/*
 ** \file port_match.c $Id$ PORT Match procedures for DNX.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/port.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port_match.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/** L2 gport: VLAN_PORT, EXTENDER_PORT or MPLS_port.*/
#define DNX_GPORT_IS_L2_GPORT(_gport) (BCM_GPORT_IS_VLAN_PORT(_gport) || BCM_GPORT_IS_EXTENDER_PORT(_gport) || \
                                       BCM_GPORT_IS_MPLS_PORT(_gport) || BCM_GPORT_IS_TUNNEL(_gport))

/** MPLS L2 GPorts: MPLS_PORT, TUNNEL (for EVPN) */
#define DNX_GPORT_IS_MPLS_L2_GPORT(_gport) (BCM_GPORT_IS_MPLS_PORT(_gport) || BCM_GPORT_IS_TUNNEL(_gport))


/** Enum for dnx_port_match_esem_cmd_update.*/
typedef enum
{
    /**
     * Delete ESEM default profile from the CMD
     */
    DNX_PORT_ESEM_CMD_DEL = 0,

    /**
     * Add ESEM default profile to the CMD
     */
    DNX_PORT_ESEM_CMD_ADD
} dnx_port_match_esem_cmd_oper_e;

/**
 * \brief
 * Verify function for BCM-API:
 *    bcm_dnx_port_match_add
 *    bcm_dnx_port_match_delete
 */
static shr_error_e
dnx_port_match_verify(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    int is_ingress, is_egress, is_native_ac;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(match_info, _SHR_E_PARAM, "match_info");

    /*
     * Verify flags:
     *  - One of ingres/egress MUST be set.
     *  - Only one of ingres/egress is allowed to be set.
     */
    is_ingress = _SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress = _SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_EGRESS_ONLY) ? TRUE : FALSE;
    is_native_ac = _SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_NATIVE);

    if ((is_ingress == FALSE) && (is_egress == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Neither INGRESS nor EGRESS are set!!! flags = 0x%08X\n",
                     match_info->flags);
    }

    if ((is_ingress == TRUE) && (is_egress == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Both INGRESS and EGRESS are set!!! flags = 0x%08X\n",
                     match_info->flags);
    }

    if (is_ingress == TRUE)
    {
        /*
         * match criteria for ingress:
         */

        if (is_native_ac)
        {
            /*
             * Native matching over PWE validation
             */
            if (!BCM_GPORT_IS_VLAN_PORT(port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Ingress unsupported match. VLAN tags in Native ETH can only be matched to VLAN port.");
            }
            if (!DNX_GPORT_IS_MPLS_L2_GPORT(match_info->port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Ingress unsupported match. VLAN tags in Native ETH can only be matched over MPLS L2 port "
                             "(MPLS Port or EVPN).");
            }
        }
        else
        {
            /*
             * Verify that the gport type is VLAN PORT
             */
            if (BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(port)) == -1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. port = 0x%08X is not a VLAN port LIF.\n", port);
            }
        }

        /*
         * Verify match criteria for ingress:
         */
        switch (match_info->match)
        {
                
            case BCM_PORT_MATCH_PORT:
                break;

            case BCM_PORT_MATCH_PORT_VLAN:
            case BCM_PORT_MATCH_PORT_CVLAN:
                /*
                 * Verify match_vlan is in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);

                /*
                 * BCM_PORT_MATCH_PORT_CVLAN is same as BCM_PORT_MATCH_PORT_VLAN for Native.
                 */
                if ((is_native_ac == TRUE) && (match_info->match == BCM_PORT_MATCH_PORT_CVLAN))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, Native Ingress unsupported match! match = BCM_PORT_MATCH_PORT_CVLAN (%d) is no supported for Native. Please use BCM_PORT_MATCH_PORT_VLAN (%d) instead. flags = 0x%08X\n",
                                 BCM_PORT_MATCH_PORT_CVLAN, BCM_PORT_MATCH_PORT_VLAN, match_info->flags);
                }

                break;

            case BCM_PORT_MATCH_PORT_VLAN_STACKED:

                /*
                 * Verify match_vlan and match_inner_vlan are in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_inner_vlan);
                break;

            case BCM_PORT_MATCH_PORT_PCP_VLAN:

                if (is_native_ac)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "BCM_PORT_MATCH_PORT_PCP_VLAN (%d) match for native AC is not supported. flags = 0x%08X\n",
                                 BCM_PORT_MATCH_PORT_PCP_VLAN, match_info->flags);
                }

                /*
                 * Verify match_vlan and match_pcp are in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);
                BCM_DNX_PCP_CHK_ID(unit, match_info->match_pcp);
                break;

            case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:

                if (is_native_ac)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED (%d) match for native AC is not supported. flags = 0x%08X\n",
                                 BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED, match_info->flags);
                }

                /*
                 * Verify match_vlan match_pcp and match_inner_vlan are in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_inner_vlan);
                BCM_DNX_PCP_CHK_ID(unit, match_info->match_pcp);
                break;

            case BCM_PORT_MATCH_PORT_UNTAGGED:

                if (is_native_ac)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "BCM_PORT_MATCH_PORT_UNTAGGED (%d) match for native AC is not supported. flags = 0x%08X\n",
                                 BCM_PORT_MATCH_PORT_UNTAGGED, match_info->flags);
                }

                break;

            default:

                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Ingress unsupported match. match = %d, only BCM_PORT_MATCH_PORT(%d), BCM_PORT_MATCH_PORT_VLAN(%d),"
                             "BCM_PORT_MATCH_PORT_VLAN_STACKED(%d), BCM_PORT_MATCH_PORT_CVLAN(%d), BCM_PORT_MATCH_PORT_PCP_VLAN(%d), "
                             "BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED(%d) and BCM_PORT_MATCH_PORT_UNTAGGED (%d) are supported\n",
                             match_info->match, BCM_PORT_MATCH_PORT, BCM_PORT_MATCH_PORT_VLAN,
                             BCM_PORT_MATCH_PORT_VLAN_STACKED, BCM_PORT_MATCH_PORT_CVLAN,
                             BCM_PORT_MATCH_PORT_PCP_VLAN, BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED,
                             BCM_PORT_MATCH_PORT_UNTAGGED);
                break;
        }
    }
    else
    {
        /*
         * match criteria for egress:
         */
        switch (match_info->match)
        {
            case BCM_PORT_MATCH_PORT:
            {
                if (BCM_GPORT_IS_SET(port))
                {
                    uint8 is_phy_port = FALSE;

                    /*
                     * In case of adding esem default result to a port/LIF,
                     * 'port' should be virtual gport for esem default result profile.
                     * 'match_info->port' must be gport for vlan_port or extender_port, mpls_port, or
                     * a physical port/lag gport.
                     */
                    if (((BCM_GPORT_IS_VLAN_PORT(port) || (BCM_GPORT_IS_EXTENDER_PORT(port))) &&
                         BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(port)))
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, match_info->port, &is_phy_port));
                        if (is_phy_port == FALSE)
                        {
                            /** port should be a L2 GPORT: vlan_port, extender_port, mpls_port.*/
                            if (!DNX_GPORT_IS_L2_GPORT(match_info->port))
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM,
                                             "In case of adding esem default result to out-lif, Match Port = 0x%08X "
                                             "should be vlan/extender/mpls port or mpls tunnel for EVPN.\n",
                                             match_info->port);
                            }
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "in case of adding esem default result to port/LIF, Port = 0x%08X should be with subtype of virtual egress default.\n",
                                     port);
                    }
                }
                else
                {
                    /*
                     * Add esem match entries to physical port, port should be virtual gport.
                     * So, 'port' must be a gport for virtual gport or outlif-id
                     */
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Port = 0x%08X should not be a physical port.\n", port);
                }
                break;
            }
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Egress unsupported match. match = %d, only BCM_PORT_MATCH_PORT(%d) is supported\n",
                             match_info->match, BCM_PORT_MATCH_PORT);
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API bcm_dnx_port_match_set
 */
static shr_error_e
dnx_port_match_set_verify(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_port_match_info_t * match_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(match_array, _SHR_E_PARAM, "match_array");

    SHR_MAX_VERIFY(1, size, _SHR_E_PARAM, "size must be higher than 0");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Ingress - decide what to do according to match criteria
 */
static shr_error_e
dnx_port_match_ingress_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t global_lif;

    SHR_FUNC_INIT_VARS(unit);
    global_lif = port;

    /*
     * Get local In-LIF using DNX Algo Gport Management:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, global_lif, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
        {
            /*
             * MATCH_PORT criteria
             */
            if (match_info->match_ethertype)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, match_info->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                 match_info->match_ethertype, BCM_DNX_PCP_INVALID, gport_hw_resources.local_in_lif));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_default
                                (unit, match_info->port, gport_hw_resources.local_in_lif));
            }

            break;
        }
        case BCM_PORT_MATCH_PORT_VLAN:
        {
            /*
             * MATCH_PORT_VLAN criteria
             */
            if (match_info->match_ethertype)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, match_info->port, match_info->match_vlan, BCM_VLAN_INVALID,
                                 match_info->match_ethertype, BCM_DNX_PCP_INVALID, gport_hw_resources.local_in_lif));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan
                                (unit, match_info->port, match_info->match_vlan, gport_hw_resources.local_in_lif));
            }

            break;
        }
        case BCM_PORT_MATCH_PORT_CVLAN:
        {
            /*
             * MATCH_PORT_VLAN criteria
             */
            if (match_info->match_ethertype)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, match_info->port, match_info->match_vlan, BCM_VLAN_INVALID,
                                 match_info->match_ethertype, BCM_DNX_PCP_INVALID, gport_hw_resources.local_in_lif));

            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_c_vlan
                                (unit, match_info->port, match_info->match_vlan, gport_hw_resources.local_in_lif));
            }

            break;
        }
        case BCM_PORT_MATCH_PORT_VLAN_STACKED:
        {
            /*
             * MATCH_PORT_VLAN_STACKED criteria
             */
            if (match_info->match_ethertype)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, match_info->port, match_info->match_vlan, match_info->match_inner_vlan,
                                 match_info->match_ethertype, BCM_DNX_PCP_INVALID, gport_hw_resources.local_in_lif));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan
                                (unit, match_info->port, match_info->match_vlan, match_info->match_inner_vlan,
                                 gport_hw_resources.local_in_lif));
            }

            break;
        }
        case BCM_PORT_MATCH_PORT_PCP_VLAN:
        {
            /*
             * BCM_PORT_MATCH_PORT_PCP_VLAN criteria
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                            (unit, match_info->port, match_info->match_vlan, BCM_VLAN_INVALID,
                             match_info->match_ethertype, match_info->match_pcp, gport_hw_resources.local_in_lif));

            break;
        }
        case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            /*
             * BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED criteria
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                            (unit, match_info->port, match_info->match_vlan, match_info->match_inner_vlan,
                             match_info->match_ethertype, match_info->match_pcp, gport_hw_resources.local_in_lif));

            break;
        }
        case BCM_PORT_MATCH_PORT_UNTAGGED:
        {
            /*
             * BCM_PORT_MATCH_PORT_UNTAGGED criteria
             */
            if (match_info->match_ethertype)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, match_info->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                 match_info->match_ethertype, BCM_DNX_PCP_INVALID, gport_hw_resources.local_in_lif));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_untagged
                                (unit, match_info->port, gport_hw_resources.local_in_lif));
            }

            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Ingress unsupported match. match = %d, only BCM_PORT_MATCH_PORT, BCM_PORT_MATCH_PORT_VLAN, "
                         "BCM_PORT_MATCH_PORT_PCP_VLAN,BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED"
                         "BCM_PORT_MATCH_PORT_CVLAN, BCM_PORT_MATCH_PORT_VLAN_STACKED and BCM_PORT_MATCH_PORT_UNTAGGED are supported\n",
                         match_info->match);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Ingress - get vlan_domain and check whether in_lif_profile is LIF scope
*/
shr_error_e
dnx_ingress_native_ac_interface_namespace_check(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    uint32 *vlan_domain,
    uint32 *is_intf_namespace)
{
    uint32 in_lif_profile = 0;
    in_lif_profile_info_t in_lif_profile_info;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /* get in_lif_profile and vlan_domain */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    if(vlan_domain)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, vlan_domain));
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &in_lif_profile));

    
    /** Get in_lif_porfile data: */
    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
            
    *is_intf_namespace = _SHR_IS_FLAG_SET(in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags, DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Ingress - decide what to do according to match criteria
 */
static shr_error_e
dnx_port_match_ingress_native_ac_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{

    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_result;
    uint32 matched_global_in_lif;
    uint32 result_local_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Global In-LIF of the match_info->port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, match_info->port,
                                                                   DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                                   &gport_hw_resources_match), _SHR_E_NOT_FOUND,
                                _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /* get vlan_domain and check whether LIF or NETWORK scope should be used */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check(unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    /*
     * Get the resulting local in-lif from gport to hw resources.
     */
    sal_memset(&gport_hw_resources_result, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port,
                                                                   DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                                   &gport_hw_resources_result), _SHR_E_NOT_FOUND,
                                _SHR_E_PARAM);
    result_local_in_lif = gport_hw_resources_result.local_in_lif;

    /*
     * Grab a handle for the correct table and set the separate key fields.
     */
    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
            /*
             * Port only match, only take handle.
             */
            
            if(is_intf_namespace)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_0_VLANS,
                                 &entry_handle_id));
                /** Set the extracted global lif in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS,
                                 &entry_handle_id));
                /** Set the extracted vlan domain in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            }
            break;
        case BCM_PORT_MATCH_PORT_VLAN:
            /*
             * Port * VLAN match, take handle and set the vlan to match.
             */
            if(is_intf_namespace)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS,
                                 &entry_handle_id));
                /** Set the extracted global lif in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS,
                                 &entry_handle_id));
                /** Set the extracted vlan domain in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            }
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, match_info->match_vlan);
            break;
        case BCM_PORT_MATCH_PORT_VLAN_STACKED:
            /*
             * Port * VLAN * VLAN match, take handle and set both vlans to match.
             */
            if(is_intf_namespace)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS,
                                 &entry_handle_id));
                /** Set the extracted global lif in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS,
                                 &entry_handle_id));
                /** Set the extracted vlan domain in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            }
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_info->match_vlan);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_info->match_inner_vlan);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress unsupported match. match = %d, only BCM_PORT_MATCH_PORT, "
                         "BCM_PORT_MATCH_PORT_VLAN and BCM_PORT_MATCH_PORT_VLAN_STACKED are supported\n",
                         match_info->match);
    }

    /*
     * Set the core id in the key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /*
     * Set the new local in-LIF to be used
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, result_local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function clears inner ETH AC ISEM lookup.
 */
static shr_error_e
dnx_port_match_ingress_native_ac_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Global In-LIF of the match_info->port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, match_info->port,
                                                                   DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                                   &gport_hw_resources_match), _SHR_E_NOT_FOUND,
                                _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /* get vlan_domain and check whether LIF or NETWORK scope should be used */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check(unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    /*
     * Grab a handle for the correct table and set the separate key fields.
     */
    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
            /*
             * Port only match, only take handle.
             */
            
            if(is_intf_namespace)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_0_VLANS,
                                 &entry_handle_id));
                /** Set the extracted global lif in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS,
                                 &entry_handle_id));
                /** Set the extracted vlan domain in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            }
            break;
        case BCM_PORT_MATCH_PORT_VLAN:
            /*
             * Port * VLAN match, take handle and set the vlan to match.
             */
            if(is_intf_namespace)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS,
                                 &entry_handle_id));
                /** Set the extracted global lif in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS,
                                 &entry_handle_id));
                /** Set the extracted vlan domain in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            }
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, match_info->match_vlan);
            break;
        case BCM_PORT_MATCH_PORT_VLAN_STACKED:
            /*
             * Port * VLAN * VLAN match, take handle and set both vlans to match.
             */
            if(is_intf_namespace)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS,
                                 &entry_handle_id));
                /** Set the extracted global lif in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS,
                                 &entry_handle_id));
                /** Set the extracted vlan domain in the key field */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            }
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_info->match_vlan);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_info->match_inner_vlan);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress native unsupported match. match = %d, only BCM_PORT_MATCH_PORT, "
                         "BCM_PORT_MATCH_PORT_VLAN and BCM_PORT_MATCH_PORT_VLAN_STACKED are supported\n",
                         match_info->match);
    }

    /*
     * Set the core id in the key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /*
     * clear DBAL entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function clears ISEM match.
 */
static shr_error_e
dnx_port_match_ingress_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    bcm_vlan_port_t port_info;

    SHR_FUNC_INIT_VARS(unit);

    port_info.port = match_info->port;
    port_info.match_vlan = match_info->match_vlan;
    port_info.match_inner_vlan = match_info->match_inner_vlan;
    port_info.match_ethertype = match_info->match_ethertype;
    port_info.match_pcp = match_info->match_pcp;
    /** This field is irrelevant for clearing the ISEM, used for debug purposes */
    port_info.vlan_port_id = port;

    /** Mapping bcm_vlan_port_match_t enum to bcm_port_match_t enum */
    switch (match_info->match)
    {
        case BCM_PORT_MATCH_NONE:
        {
            port_info.criteria = BCM_VLAN_PORT_MATCH_NONE;
            break;
        }

        case BCM_PORT_MATCH_PORT:
        {
            port_info.criteria = BCM_VLAN_PORT_MATCH_PORT;
            break;
        }

        case BCM_PORT_MATCH_PORT_VLAN:
        {
            port_info.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            break;
        }

        case BCM_PORT_MATCH_PORT_CVLAN:
        {
            port_info.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
            break;
        }

        case BCM_PORT_MATCH_PORT_VLAN_STACKED:
        {
            port_info.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            break;
        }

        case BCM_PORT_MATCH_PORT_PCP_VLAN:
        {
            port_info.criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
            break;
        }

        case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            port_info.criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
            break;
        }

        case BCM_PORT_MATCH_PORT_UNTAGGED:
        {
            port_info.criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, gport = 0x%08X contains unknown/unsupported criteria (=%d)!\n",
                         port, match_info->match);
            break;
        }
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_non_native_match_clear(unit, &port_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get ESEM access key type, aka app-db-id, according to port match criteria.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] match_info - Port match information.
 * \param [out] esem_app_db - Esem access app-db-id.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_criteria_to_esem_app_db_get(
    int unit,
    bcm_port_match_info_t * match_info,
    dbal_enum_value_field_esem_app_db_id_e * esem_app_db)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_app_db, _SHR_E_INTERNAL, "A valid pointer for esem_app_db is needed!\n");

    /** Init esem-app-db-id */
    *esem_app_db = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_NAMESPACE_CTAG;

    /** Assign esem-app-db-id according to criteria */
    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
        {
            *esem_app_db = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_NAMESPACE_CTAG;
            if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_NATIVE))
            {
                *esem_app_db = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_OUTLIF;
            }
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unavailable criteria(%d) in ESEM!\n", match_info->match);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set an esem access command to a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, or out-lif-id in gport format.
 * \param [in] esem_cmd - esem access command that should be linked to the port
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_esem_cmd_port_set(
    int unit,
    bcm_gport_t port,
    int esem_cmd)
{
    uint8 is_phy_port;
    uint32 entry_handle_id, pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Link the cmd to port/LIF
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (is_phy_port == FALSE)
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 flags;

        /** Get the local-out-lif.*/
        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port, flags, &gport_hw_resources),
                                    _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /** Update the esem-cmd in OUT-LIF table.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.outlif_dbal_result_type);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, esem_cmd);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;

        /** Get the local-pp-port.*/
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /** Update the esem-cmd in EGR_PORT table.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, INST_SINGLE, esem_cmd);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the esem access command from a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, or out-lif-id in gport format.
 * \param [out] esem_cmd - esem access command that the port was using before.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_esem_cmd_port_get(
    int unit,
    bcm_gport_t port,
    int *esem_cmd)
{
    uint8 is_phy_port;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_cmd, _SHR_E_INTERNAL, "A valid pointer for receive the original esem access cmd is needed!");

    /*
     * Retrieve the old esem-cmd from port/LIF
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (is_phy_port == FALSE)
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 flags;
        int res = _SHR_E_NONE;

        /** Get the local-out-lif.*/
        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port, flags, &gport_hw_resources),
                                    _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /** Get the old esem-cmd.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.outlif_dbal_result_type);
        res = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (res == _SHR_E_NOT_FOUND)
        {
            *esem_cmd = dnx_data_esem.access_cmd.no_action_get(unit);
        }
        else
        {
            SHR_IF_ERR_EXIT(res);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, (uint32 *) esem_cmd));
        }
    }
    else
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;

        /** Get the local-pp-lport.*/
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /** Get the old esem-cmd.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[0]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[0]);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, INST_SINGLE, (uint32 *) esem_cmd);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange a ESEM access command and update the port or LIF with it.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - virtual gport representing a ESEM default profile.
 * \param [in] match_info - pointer to the match information.
 * \param [in] add_or_delete - Is add or delete operation, see dnx_port_match_esem_cmd_oper_e.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_esem_cmd_update(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info,
    dnx_port_match_esem_cmd_oper_e add_or_delete)
{
    int esem_default_result_profile;
    int new_esem_cmd, old_esem_cmd;
    uint8 is_last = 0, is_first = 0;
    uint32 esem_cmd_flags;
    dnx_esem_access_if_t esem_if;
    dnx_esem_cmd_data_t esem_cmd_data;
    dnx_esem_access_type_t esem_access_type, old_access_type;
    dbal_enum_value_field_esem_app_db_id_e esem_app_db;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** In case of default_result_profile is provided (must be), use it.*/
    esem_default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(port);

    /** Get esem access type according to default result entry */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get(unit, esem_default_result_profile, &esem_access_type));

    /** Get esem access app_db_id according to match criteria */
    SHR_IF_ERR_EXIT(dnx_port_match_criteria_to_esem_app_db_get(unit, match_info, &esem_app_db));

    /*
     * 1) Get the original ESEM_ACC_CMND
     */
    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_port_get(unit, match_info->port, &old_esem_cmd));

    /** Get the original ESEM_ACC_CMND profile data*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, old_esem_cmd, &esem_cmd_data));

    /** Get the original ESEM_ACC_CMND interface*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_if_get(unit, esem_access_type, &esem_cmd_data, &esem_if, NULL));

    /** In case of deleting, Checking whether the given ESEM default profile is in use*/
    if (add_or_delete == DNX_PORT_ESEM_CMD_DEL)
    {
        int old_esem_handle;

        old_esem_handle = esem_cmd_data.esem[esem_if].default_result_profile;

        if (old_esem_handle != esem_default_result_profile)
        {
            BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(old_esem_handle, old_esem_handle);
            BCM_GPORT_VLAN_PORT_ID_SET(old_esem_handle, old_esem_handle);

            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The default AC gport (0x%08X) to remove doesn't match with the one (0x%08X) in use by the port (%d)!\n",
                         port, old_esem_handle, match_info->port);
        }

        /** Reset the default profile to predefined default entry*/
        esem_default_result_profile = (esem_access_type == ESEM_ACCESS_TYPE_ETH_AC) ?
            dnx_data_esem.default_result_profile.default_ac_get(unit) :
            (esem_access_type == ESEM_ACCESS_TYPE_ETH_NATIVE_AC) ?
            dnx_data_esem.default_result_profile.default_native_get(unit) :
            dnx_data_esem.default_result_profile.default_dual_homing_get(unit);
    }

    /*
     * 2) Allocate ESEM_ACC_CMND by exchanging
     *    For add, esem_default_result_profile is from input;
     *    for delete, it is reset to default predefined.
     */

    /** We suppose to allocate a new ESEM cmd with an access. It may be updated later. */
    esem_cmd_flags = DNX_PORT_ESEM_CMD_ALLOCATE;

    /** Check the original cmd and decide to either update the default profile or add new access.*/
    if (old_esem_cmd != dnx_data_esem.access_cmd.no_action_get(unit))
    {
        if (esem_cmd_data.esem[esem_if].valid == TRUE)
        {
            /** Update the existing cmd with the given default profile.*/
            SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get(unit,
                                                              esem_cmd_data.esem[esem_if].default_result_profile,
                                                              &old_access_type));
            /**Check if the access type matches*/
            if (old_access_type != esem_access_type)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The access types(%d, %d) don't match between the given and orignal default result profile!\n",
                             esem_access_type, old_access_type);
            }
            else
            {
                /** Using the original app_db_id and access_type, only update the default profile.*/
                esem_app_db = esem_cmd_data.esem[esem_if].app_db_id;
            }
        }

        esem_cmd_flags = DNX_PORT_ESEM_CMD_UPDATE;
    }

    /** Allocate an esem access cmd by exchanging.*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, match_info->port, esem_cmd_flags, esem_app_db,
                                               esem_access_type, esem_default_result_profile, old_esem_cmd,
                                               &new_esem_cmd, &esem_cmd_data, &is_first, &is_last));

    /*
     * 3)Connect Port->ESEM_ACC_CMND->AC_PROFILE
     */
    if (is_first == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, new_esem_cmd, esem_cmd_data));
    }
    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_port_set(unit, match_info->port, new_esem_cmd));

    /*
     * 4) Clear the old cmd entry in hardware if necessary
     */
    if ((is_last == TRUE) && (new_esem_cmd != old_esem_cmd))
    {
        /** Remove the old attributes from esem-cmd table in the old cmd-id*/
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, old_esem_cmd));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange an ESEM access cmd and link the default esem entry to the given
 *  port/LIF by the cmd.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - virtual gport representing a ESEM default profile.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  Configure VLAN PORT default egress configuration(egress vlan encapsulation)
 *  per port by configuring tables that map the following:
 *  PORT ->ESEM_ACCESS_CMD -> AC_Profile -> AC info
 *
 * \see
 *  dnx_vlan_port_egress_match_port_create
 *  dnx_port_match_esem_access_cmd_alloc
 */
static shr_error_e
dnx_port_match_egress_match_port_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * This code is handling Default port match on egress.
     * bcm_vlan_port_create allocates AC_PROFILE, returns allocated id
     * bcm_port_match_add will get allocated ac_profile as parameter (through gport) and
     * 1. Allocate ESEM_ACC_CMND
     * 2. Connect Port->ESEM_ACC_CMND->AC_PROFILE
     */

    /*
     * Allocate esem cmd with the new default profile and connect it to port
     */
    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_update(unit, port, match_info, DNX_PORT_ESEM_CMD_ADD));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Link an esem entry pointed by virtual gport to a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, virtual port or out-lif-id in gport format.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  The ESEM match entry and its virtual gport should be created by other APIs
 *  per corresponding applications, such as for ac, they should be created by
 *  bcm_vlan_port_create with the flag of BCM_VLAN_PORT_VLAN_TRANSLATION.
 *  Note that the creation won't link the ESEM match entry to port/LIF except
 *  for port default cases. User need to call bcm_port_match_add for the linkage.
 *  In a word, bcm_vlan_port_create create ESEM match entry or ESEM default entry,
 *  and bcm_port_match_add link the entry to port/LIF. Two type of virtual gport
 *  is supported.
 *  if the virtual gport is an ESEM match entry:
 *     Same match criteria and paramters should be set in match_info to ensure
 *     the exact ESEM match entry is linked.
 *     'port' here is the target to attach the entry.
 *  if the virtual gport is an ESEM default profile:
 *     match_info->port is the target to attach the entry.
 *     'port' here is the virtual gport representing an ESEM default profile.
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_egress_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
        {
            SHR_IF_ERR_EXIT(dnx_port_match_egress_match_port_add(unit, port, match_info));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported match criteria(%d), only BCM_PORT_MATCH_PORT is supported\n",
                         match_info->match);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Delete the default esem entry from the given port/LIF and free the ESEM access cmd. 
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - virtual gport representing a ESEM default profile.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  dnx_port_match_egress_match_port_add
 */
static shr_error_e
dnx_port_match_egress_match_port_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate esem cmd with the default AC profile and connect it to port
     */
    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_update(unit, port, match_info, DNX_PORT_ESEM_CMD_DEL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  delete an esem entry pointed by virtual gport from a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, virtual port or out-lif-id in gport format.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  dnx_port_match_egress_add
 */
static shr_error_e
dnx_port_match_egress_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
        {
            SHR_IF_ERR_EXIT(dnx_port_match_egress_match_port_delete(unit, port, match_info));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported match criteria(%d), only BCM_PORT_MATCH_PORT is supported\n",
                         match_info->match);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API for add a match to an existing port.
 * This API adds an entry to the correct ISEM/ESEM table to
 * points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to the match information.
 *   [in] match_info.port - Match port \n
 *   [in] match_info.flags - \n
 *        BCM_PORT_MATCH_INGRESS_ONLY - Indicates Ingress settings \n
 *        BCM_PORT_MATCH_NATIVE - Indicates Native Ethernet settings \n
 *        BCM_PORT_MATCH_EGRESS_ONLY - Indicates Egress settings \n
 *   [in] match_info.match - Match criteria \n
 *        BCM_PORT_MATCH_PORT - Match on module/port or trunk \n
 *        BCM_PORT_MATCH_PORT_VLAN - Match on module/port/trunk + outer VLAN \n
 *        BCM_PORT_MATCH_PORT_CVLAN - Match on module/port/trunk + C-VLAN \n
 *        BCM_PORT_MATCH_PORT_VLAN_STACKED - Match on module/port/trunk + inner/outer VLAN \n
 *   [in] match_info.match_vlan - Outer VLAN ID to match, when flags!=BCM_PORT_MATCH_PORT \n
 *   [in] match_info.match_inner_vlan - Inner VLAN ID to match, when flagsPORT_VLAN_STACKED \n
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  For ingress, It creates an ISEM entry pointing to an IN-LIF.
 *
 *  For egress, It allocates an esem access cmd, and links an esem match entry
 *  to a port or LIF by the match criteria.
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_match_verify(unit, port, match_info));

    /*
     * Different meaning in ingress and egress
     */
    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_INGRESS_ONLY))
    {
        if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_NATIVE))
        {
            /*
             * Add ISEM entry for native (inner) vlans
             */
            SHR_IF_ERR_EXIT(dnx_port_match_ingress_native_ac_add(unit, port, match_info));
        }
        else
        {
            /*
             * Match outer vlans and port
             */
            SHR_IF_ERR_EXIT(dnx_port_match_ingress_add(unit, port, match_info));
        }
    }
    else
    {
        /*
         * Egress match criteria.
         */
        SHR_IF_ERR_EXIT(dnx_port_match_egress_add(unit, port, match_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API for deleting a given match of an existing port.
 * This API remove an entry from the correct ISEM/ESEM table which
 * points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to the match information.
 *
 *   [in] match_info.port - Match port \n
 *   [in] match_info.flags - \n
 *        BCM_PORT_MATCH_INGRESS_ONLY - Indicates Ingress settings \n
 *        BCM_PORT_MATCH_NATIVE - Indicates Native Ethernet settings \n
 *        BCM_PORT_MATCH_EGRESS_ONLY - Indicates Egress settings, unsupported yet \n
 *   [in] match_info.match - Match criteria \n
 *        BCM_PORT_MATCH_PORT - Remove match on module/port or trunk \n
 *        BCM_PORT_MATCH_PORT_VLAN - Remove match on module/port/trunk + outer VLAN \n
 *        BCM_PORT_MATCH_PORT_CVLAN - Remove match on module/port/trunk + C-VLAN \n
 *        BCM_PORT_MATCH_PORT_VLAN_STACKED - Remove match on module/port/trunk + inner/outer VLAN \n
 *   [in] match_info.match_vlan - Outer VLAN ID match to remove, when flags!=BCM_PORT_MATCH_PORT \n
 *   [in] match_info.match_inner_vlan - Inner VLAN ID match to remove, when flagsPORT_VLAN_STACKED \n
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  For ingress, It removes an entry from ISEM pointing to an IN-LIF.
 *
 *  For egress, It resets the ESEM access cmd of a port/LIF to default and try to
 *  release the access cmd.
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_match_verify(unit, port, match_info));

    /*
     * Different meaning in ingress and egress
     */
    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_INGRESS_ONLY))
    {
        /*
         * Virtual-Native: can be identified by it's gport.
         * Non-virtual Native: can be identified by it's dbal table.
         * Note:
         * Both Virtual and Non-virtual has same dbal table thus can check dbal table 
         * for identify native or non-native.
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

        if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION)
        {
            /*
             * delete an ISEM entry from native (inner) vlans
             */
            SHR_IF_ERR_EXIT(dnx_port_match_ingress_native_ac_delete(unit, port, match_info));
        }
        else
        {
            /*
             * delete match of outer vlans and port
             */
            SHR_IF_ERR_EXIT(dnx_port_match_ingress_delete(unit, port, match_info));
        }
    }
    else
    {
        /*
         * Delete Egress match criteria.
         */
        SHR_IF_ERR_EXIT(dnx_port_match_egress_delete(unit, port, match_info));

    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
bcm_dnx_port_match_delete_all(
    int unit,
    bcm_gport_t port)
{
    return -1;
}

shr_error_e
bcm_dnx_port_match_multi_get(
    int unit,
    int size,
    bcm_gport_t port,
    bcm_port_match_info_t * match_array,
    int *count)
{
    return -1;
}

/**
 * \brief -
 * BCM API to replace a match to an existing port with a new match.
 * This API adds an entry and removes another entry to/from the correct
 * ISEM/ESEM table that points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] old_match - pointer to the match information to replace.
 * \param [in] new_match - pointer to the match information to replace with.
 *   [in] match_info.port - Match port \n
 *   [in] match_info.flags - \n
 *        BCM_PORT_MATCH_INGRESS_ONLY - Indicates Ingress settings \n
 *        BCM_PORT_MATCH_NATIVE - Indicates Native Ethernet settings \n
 *        BCM_PORT_MATCH_EGRESS_ONLY - Indicates Egress settings \n
 *   [in] match_info.match - Match criteria \n
 *        BCM_PORT_MATCH_PORT - Match on module/port or trunk \n
 *        BCM_PORT_MATCH_PORT_VLAN - Match on module/port/trunk + outer VLAN \n
 *        BCM_PORT_MATCH_PORT_CVLAN - Match on module/port/trunk + C-VLAN \n
 *        BCM_PORT_MATCH_PORT_VLAN_STACKED - Match on module/port/trunk + inner/outer VLAN \n
 *   [in] match_info.match_vlan - Outer VLAN ID to match, when flags!=BCM_PORT_MATCH_PORT \n
 *   [in] match_info.match_inner_vlan - Inner VLAN ID to match, when flagsPORT_VLAN_STACKED \n
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * old_match,
    bcm_port_match_info_t * new_match)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * New match will be verified in bcm_dnx_port_match_add.
     * Checking the old match here as well in case it isn't
     * valid which will prevent adding the match.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_match_verify(unit, port, old_match));

    /*
     * Add new match
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_match_add(unit, port, new_match));

    /*
     * Delete old match
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_match_delete(unit, port, old_match));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API to add a set of matches to an existing port.
 * This API adds several entries to the correct ISEM/ESEM table that
 * points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] size - size of array.
 * \param [in] match_array - a pointer to an array of matches information.
 *   [in] match_info.port - Match port \n
 *   [in] match_info.flags - \n
 *        BCM_PORT_MATCH_INGRESS_ONLY - Indicates Ingress settings \n
 *        BCM_PORT_MATCH_NATIVE - Indicates Native Ethernet settings \n
 *        BCM_PORT_MATCH_EGRESS_ONLY - Indicates Egress settings \n
 *   [in] match_info.match - Match criteria \n
 *        BCM_PORT_MATCH_PORT - Match on module/port or trunk \n
 *        BCM_PORT_MATCH_PORT_VLAN - Match on module/port/trunk + outer VLAN \n
 *        BCM_PORT_MATCH_PORT_CVLAN - Match on module/port/trunk + C-VLAN \n
 *        BCM_PORT_MATCH_PORT_VLAN_STACKED - Match on module/port/trunk + inner/outer VLAN \n
 *   [in] match_info.match_vlan - Outer VLAN ID to match, when flags!=BCM_PORT_MATCH_PORT \n
 *   [in] match_info.match_inner_vlan - Inner VLAN ID to match, when flagsPORT_VLAN_STACKED \n
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  For ingress, It creates an ISEM entry pointing to an IN-LIF.
 *
 *  For egress, It allocates an esem access cmd, and links an esem match entry
 *  to a port or LIF by the match criteria.
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_port_match_info_t * match_array)
{
    uint32 array_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_match_set_verify(unit, port, size, match_array));

    for (array_index = 0; array_index < size; array_index++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_match_add(unit, port, match_array + array_index));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
