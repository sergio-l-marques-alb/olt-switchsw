/** \file tunnel_init.c
 *  General TUNNEL encapsulation functionality for DNX.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>

#include <bcm/types.h>
#include <bcm/tunnel.h>

#include <bcm_int/common/debug.h>

#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/instru/instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <bcm_int/dnx/port/port_tpid.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_qos_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_tunnel_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * MACROs and ENUMs
 * {
 */

/*
 * End of MACROs
 * }
 */

/*
 * Verify functions
 * {
 */

/*
 * return weather tunnel type is ipv6
 */
static void
dnx_tunnel_initiator_type_is_ipv6(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_ipv6)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeVxlan6:
        case bcmTunnelTypeVxlan6Gpe:
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeSR6:
            *is_ipv6 = TRUE;
            break;
        default:
            *is_ipv6 = FALSE;
            break;
    }
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_initiator_create_verify function
 * to check tunnel type support.
 */
static shr_error_e
dnx_tunnel_initiator_tunnel_type_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify that tunnel type is supported */
    switch (tunnel_type)
    {
        /** This is the list of all supported tunnel types */
        
        case bcmTunnelTypeGreAnyIn4:
        {
            break;
        }
        case bcmTunnelTypeIpAnyIn4:
        {
            break;
        }
        case bcmTunnelTypeUdp:
        {
            break;
        }
        case bcmTunnelTypeVxlan:
        {
            break;
        }
        case bcmTunnelTypeVxlanGpe:
        {
            break;
        }
        case bcmTunnelTypeIpAnyIn6:
        {
            break;
        }
        case bcmTunnelTypeGreAnyIn6:
        {
            break;
        }
        case bcmTunnelTypeVxlan6:
        {
            break;
        }
        case bcmTunnelTypeVxlan6Gpe:
        {
            break;
        }
        case bcmTunnelTypeUdp6:
        {
            break;
        }
        case bcmTunnelTypeSR6:
        {
            break;
        }
        case bcmTunnelTypeErspan:
        {
            break;
        }
        case bcmTunnelTypeRspanAdvanced:
        {
            break;
        }
        case bcmTunnelTypeNone:
        {
            /** for tunnel encapsulation, tunnelType is mandatory */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel encapsulation type value (%d) is invalid, type is a required field",
                         tunnel_type);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Tunnel encapsulation type value (%d) is not supported", tunnel_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_initiator_create to verify API input, intf and tunnel structs.
 */
static shr_error_e
dnx_tunnel_initiator_create_verify(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "intf");
    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    /** Verify that tunnel type is supported */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_tunnel_type_verify(unit, tunnel->type));

    /** Verify that tunnel->vlan is not initialized - no longer used in JR2 */
    if (tunnel->vlan)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Param: tunnel->vlan must be 0 - field not supported on JR2, ETH-RIF properties can only be set by an ARP entry!");
    }
    /** Verify TTL */
    if ((tunnel->egress_qos_model.egress_ttl != bcmQosEgressModelUniform) &&
        (tunnel->egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "model support only bcmQosEgressModelPipeMyNameSpace and bcmQosEgressModelUniform");
    }
    if ((tunnel->egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace) && (tunnel->ttl != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "If ttl model is not bcmQosEgressModelPipeMyNameSpace, ttl should be 0");
    }
    if (!BCM_TTL_VALID(tunnel->ttl))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Param: tunnel->ttl is not valid. Valid range is 0-255");
    }
    /** Verify WITH_ID and REPLACE flags */
    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE) && !_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_TUNNEL_REPLACE can't be used without BCM_TUNNEL_WITH_ID");
    }
    /** Verify BCM_TUNNEL_INIT_WIDE flag is not used */
    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_WIDE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_TUNNEL_INIT_WIDE is not supported in JR2, please read the backward compatibility document");
    }
    /** Verify the flow_label to be zero*/
    if (tunnel->flow_label != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flow label has to be zero");
    }
    /** Check QOS model is allowed */
    if ((tunnel->egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace) &&
        (tunnel->egress_qos_model.egress_qos != bcmQosEgressModelPipeMyNameSpace) &&
        (tunnel->egress_qos_model.egress_qos != bcmQosEgressModelUniform))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "qos model (%d) is not allowed. Available models are PipeNextNameSpace, PipeMyNameSpace and Uniform.",
                     tunnel->egress_qos_model.egress_qos);
    }
    /** If QOS mode is not pipe, dscp should be 0*/
    if ((tunnel->egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace) &&
        (tunnel->egress_qos_model.egress_qos != bcmQosEgressModelPipeMyNameSpace) && (tunnel->dscp != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "If qos model is not pipe, dscp should be 0");
    }
    /** qos map id */
    if ((tunnel->qos_map_id != DNX_QOS_INITIAL_MAP_ID) && !DNX_QOS_MAP_IS_REMARK(tunnel->qos_map_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "qos profile(%d) is not remark profile", tunnel->qos_map_id);
    }
    /** Verify the udp source and destination port are only supplied if the tunnel is a UDP tunnel. */
    if (tunnel->type != bcmTunnelTypeUdp && ((tunnel->udp_dst_port != 0) || (tunnel->udp_dst_port != 0)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "UDP Source and Destination port can only be configured for bcmTunnelTypeUdp");
    }
    /** Verify the smac field is not supplied as API input*/
    if (!BCM_MAC_IS_ZERO(tunnel->smac))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The functionality is no longer available, please read the backward compatibility document");
    }
    /** Verify the outlif_counting_profile is not supplied as API input*/
    if ((tunnel->outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Statistics are supported with dedicated API, please read the backward compatibility document");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_initiator_clear to verify API input parameters.
 */
static shr_error_e
dnx_tunnel_initiator_clear_verify(
    int unit,
    bcm_l3_intf_t * intf)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "intf");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_initiator_get to verify API input parameters.
 */
static shr_error_e
dnx_tunnel_initiator_get_verify(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "intf");
    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_tunnel_initiator_traverse
 * \param [in] unit - the unit number.
 * \param [in] cb - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  *
 */
static shr_error_e
dnx_tunnel_initiator_traverse_verify(
    int unit,
    bcm_tunnel_initiator_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "No callback function has been provided to the traverse.");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_initiator_create to verify API input, intf and tunnel structs.
 */
static shr_error_e
dnx_instru_trajectory_trace_create_verify(
    int unit,
    bcm_instru_trajectory_trace_t * trajectory_trace)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(trajectory_trace, _SHR_E_PARAM, "trajectory_trace");

exit:
    SHR_FUNC_EXIT;
}

/*
 * End of Verify functions
 * }
 */

/*
 * Inner functions
 * {
 */

/**
 * \brief
 * The function removes a single DBAL entry.
*/
static shr_error_e
dnx_tunnel_initiator_dbal_entry_clear(
    int unit,
    uint32 dbal_table_id,
    uint32 dbal_key_field,
    uint32 dbal_key_value,
    uint32 result_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_key_field, dbal_key_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function converts BCM tunnel-type to DBAL result-type.
*/
static shr_error_e
dnx_tunnel_init_bcm_to_dbal_result_type(
    int unit,
    bcm_tunnel_type_t bcm_tunnel_type,
    uint32 bcm_tunnel_flags,
    uint32 *dbal_result_type,
    uint32 *lif_additional_header_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_result_type, _SHR_E_PARAM, "dbal_result_type");

    *dbal_result_type = 0;
    *lif_additional_header_profile = 0;

    switch (bcm_tunnel_type)
    {
        case bcmTunnelTypeGreAnyIn4:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_GRE_GENEVE;
            if (bcm_tunnel_flags == BCM_TUNNEL_INIT_GRE_KEY_USE_LB)
            {
                *lif_additional_header_profile =
                    DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY;
            }
            else if (bcm_tunnel_flags == BCM_TUNNEL_INIT_GRE_WITH_SN)
            {
                *lif_additional_header_profile = DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_GRE8_W_SN;
            }
            else
            {
                *lif_additional_header_profile = DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE;
            }
            break;
        }
        case bcmTunnelTypeErspan:
        {
            if (bcm_tunnel_flags & BCM_TUNNEL_INIT_ERSPAN_TYPE3)
            {
                *dbal_result_type = DBAL_RESULT_TYPE_EEDB_ERSPAN_ETPS_ERSPANV3;
            }
            else
            {
                *dbal_result_type = DBAL_RESULT_TYPE_EEDB_ERSPAN_ETPS_ERSPANV2;
            }

            break;
        }
        case bcmTunnelTypeIpAnyIn4:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL;
            *lif_additional_header_profile = DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE;
            break;
        }
        case bcmTunnelTypeUdp:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP;
            *lif_additional_header_profile = DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE;
            break;
        }
        case bcmTunnelTypeVxlan:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE;
            *lif_additional_header_profile =
                DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY;
            break;
        }
        case bcmTunnelTypeVxlanGpe:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE;
            *lif_additional_header_profile =
                DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4;
            break;
        }
        case bcmTunnelTypeIpAnyIn6:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_RAW;
            break;
        }

        case bcmTunnelTypeGreAnyIn6:
        {
            if ((bcm_tunnel_flags & BCM_TUNNEL_INIT_GRE_KEY_USE_LB) != 0)
            {
                *lif_additional_header_profile =
                    DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY;
            }
            else
            {
                *lif_additional_header_profile = DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE;
            }
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GRE;
            break;
        }
        case bcmTunnelTypeUdp6:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_UDP;
            *lif_additional_header_profile = DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE;
            break;
        }
        case bcmTunnelTypeVxlan6:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN;
            *lif_additional_header_profile =
                DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY;
            break;
        }
        case bcmTunnelTypeVxlan6Gpe:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN;
            *lif_additional_header_profile =
                DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4;
            break;
        }
        case bcmTunnelTypeSR6:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_RAW;
            break;
        }
        case bcmTunnelTypeRspanAdvanced:
        {
            *dbal_result_type = DBAL_RESULT_TYPE_EEDB_RSPAN_ETPS_RSPAN;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel encapsulation type value (%d) is not supported", bcm_tunnel_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function converts DBAL result-type to BCM tunnel-type.
*/
static shr_error_e
dnx_tunnel_init_dbal_to_bcm_tunnel_type(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 dbal_result_type,
    uint32 lif_additional_header_profile,
    bcm_tunnel_type_t * bcm_tunnel_type,
    uint32 *bcm_tunnel_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_tunnel_type, _SHR_E_PARAM, "bcm_tunnel_type");

    *bcm_tunnel_type = bcmTunnelTypeNone;

    if (dbal_table_id != DBAL_TABLE_EEDB_IPV6_TUNNEL)
    {
        switch (dbal_result_type)
        {
            case DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_GRE_GENEVE:
            {
                if (lif_additional_header_profile ==
                    DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY)
                {
                    *bcm_tunnel_flags |= BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
                }
                else if (lif_additional_header_profile ==
                         DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_GRE8_W_SN)
                {
                    *bcm_tunnel_flags |= BCM_TUNNEL_INIT_GRE_WITH_SN;
                }

                *bcm_tunnel_type = bcmTunnelTypeGreAnyIn4;
                break;
            }
            case DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL:
            {
                *bcm_tunnel_type = bcmTunnelTypeIpAnyIn4;
                break;
            }
            case DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP:
            {
                *bcm_tunnel_type = bcmTunnelTypeUdp;
                break;
            }
            case DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE:
            {
                if (lif_additional_header_profile ==
                    DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4)
                {
                    *bcm_tunnel_type = bcmTunnelTypeVxlanGpe;
                }
                else
                {
                    *bcm_tunnel_type = bcmTunnelTypeVxlan;
                }
                break;
            }
            default:
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "DBAL type value (%d) is not supported", dbal_result_type);
            }
        }
    }
    else
    {
        switch (dbal_result_type)
        {
            case DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GRE:
            {
                *bcm_tunnel_type = bcmTunnelTypeGreAnyIn6;
                if (lif_additional_header_profile ==
                    DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_OR_LIF_AHP_ELOELI_OR_LIF_AHP_GRE8_W_KEY)
                {
                    *bcm_tunnel_flags |= BCM_TUNNEL_INIT_GRE_KEY_USE_LB;
                }
                break;
            }
            case DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_RAW:
            {
                *bcm_tunnel_type = bcmTunnelTypeIpAnyIn6;
                break;
            }
            case DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_UDP:
            {
                *bcm_tunnel_type = bcmTunnelTypeUdp6;
                break;
            }
            case DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN:
            {
                if (lif_additional_header_profile ==
                    DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_VXLAN_GPE_OR_LIF_AHP_ESI_OR_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4)
                {
                    *bcm_tunnel_type = bcmTunnelTypeVxlan6Gpe;
                }
                else
                {
                    *bcm_tunnel_type = bcmTunnelTypeVxlan6;
                }
                break;
            }
            default:
            {
                SHR_ERR_EXIT(BCM_E_INTERNAL, "DBAL type value (%d) is not supported", dbal_result_type);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function returns whether LLVP profile exist or not based on the tunnel result type
*/
static int
dnx_tunnel_utility_llvp_profile_exist(
    int unit,
    uint32 result_type,
    uint32 tunnel_is_ipv6)
{
    int retVal = FALSE;
    
    return retVal;
}

/**
 * \brief
 * The following function writes SIP value to HW.
 */
static shr_error_e
dnx_tunnel_initiator_sip_addr_hw_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 sip_profile)
{
    uint32 entry_handle_id;
    uint32 tunnel_is_ipv6;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dnx_tunnel_initiator_type_is_ipv6(unit, tunnel->type, &tunnel_is_ipv6);
    if (tunnel_is_ipv6)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6, &entry_handle_id));
        /** Set data field */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SIP, INST_SINGLE, tunnel->sip6);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &entry_handle_id));
        /** Set data field */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, INST_SINGLE, tunnel->sip);
    }

    /** Set key field*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, sip_profile);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * For Tunnel initiator, SIP is not fully provided to EEDB entry, instead only limited number of SIP addresses can be
 * supported and they are managed by SIP profile (sip_idx). The following function allocate entry for tunnel's
 * SIP address using template management and get corresponding index.
 * And write to HW (DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4) if new template was allocated.
 *
 *   \param [in] unit           - The unit number.
 *   \param [in] tunnel       - A pointer to the struct that holds information for the
 *     IPv4/IPv6 tunnel initiator entry.
 *   \param [out] sip_profile   - Valid pointer to SIP index allocated by template management.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \ref shr_error_e.
 *
 * \remark
 *   * DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4 dbal - ETPPB_SOURCE_ADDRESS_MAP
 */
static shr_error_e
dnx_tunnel_initiator_sip_addr_allocate_and_hw_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 *sip_profile)
{
    uint8 is_first_sip_ref;
    uint32 alloc_flags;
    source_address_entry_t source_address_entry;
    uint32 tunnel_is_ipv6;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sip_profile, _SHR_E_PARAM, "sip_profile");

    dnx_tunnel_initiator_type_is_ipv6(unit, tunnel->type, &tunnel_is_ipv6);

    /** Initialize variables */
    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));
    alloc_flags = 0;
    if (tunnel_is_ipv6)
    {
        source_address_entry.address_type = source_address_type_ipv6;
        sal_memcpy(&source_address_entry.address.ipv6_address, &(tunnel->sip6), sizeof(bcm_ip6_t));
    }
    else
    {
        source_address_entry.address_type = source_address_type_ipv4;
        sal_memcpy(&source_address_entry.address.ipv4_address, &tunnel->sip, sizeof(bcm_ip_t));
    }

    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.allocate_single
                    (unit, _SHR_CORE_ALL, alloc_flags, &source_address_entry, NULL, (int *) sip_profile,
                     &is_first_sip_ref));

    /** write to HW if new SIP index was allocated */
    if (is_first_sip_ref)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_sip_addr_hw_set(unit, tunnel, *sip_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The following function exchanges existing SIP and TTL profiles. It writes SIP and TTL values to HW if new templates
 * were allocated. And returns a notification if old TTL and SIP values should be removed from HW.
 * \par DIRECT INPUT:
 *   \param [in] unit                       - The unit number.
 *   \param [in] tunnel                     - The BCM API input, SIP and TTL used for template exchange.
 *   \param [in] old_sip_profile            - Existing SIP profile index.
 *   \param [in] old_ttl_profile            - Existing TTL profile index.
 *   \param [in] is_ttl_pipe_mode_old       - TTL PIPE mode notification.
 *   \param [out] new_sip_profile           - New SIP profile index.
 *   \param [out] new_ttl_profile           - New TTL profile index.
 *   \param [out] remove_old_sip_profile    - Remove old SIP profile value from HW.
 *   \param [out] remove_old_ttl_profile    - Remove old TTL profile value from HW.
 *   \param [in,out] new_qos_idx          - QOS profile index.
 *   \param [out] remove_old_qos_profile    - Remove old qos profile value from HW.
 *   \param [in] is_qos_model_pipe_mode_old - QOS PIPE mode notification.
 */
static shr_error_e
dnx_tunnel_initiator_ttl_sip_template_exchange(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 old_sip_profile,
    uint32 old_ttl_profile,
    uint32 is_ttl_pipe_mode_old,
    uint32 *new_sip_profile,
    uint32 *new_ttl_profile,
    uint8 *remove_old_sip_profile,
    uint8 *remove_old_ttl_profile,
    uint32 *new_qos_idx,
    uint8 *remove_old_qos_profile,
    uint32 is_qos_model_pipe_mode_old)
{
    uint8 is_first_profile_ref;
    uint8 is_last_profile_ref;
    uint32 alloc_flags;
    source_address_entry_t source_address_entry;
    uint32 is_ttl_pipe_mode_new;
    uint32 is_qos_model_pipe_mode_new;
    uint32 tunnel_type_is_ipv6;
    uint8 nwk_qos_pipe_first_reference = FALSE;
    uint8 nwk_qos_pipe_last_reference = FALSE;
    uint32 old_qos_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");
    SHR_NULL_CHECK(new_sip_profile, _SHR_E_PARAM, "new_sip_profile");
    SHR_NULL_CHECK(new_ttl_profile, _SHR_E_PARAM, "new_ttl_profile");
    SHR_NULL_CHECK(remove_old_sip_profile, _SHR_E_PARAM, "remove_old_sip_profile");
    SHR_NULL_CHECK(remove_old_ttl_profile, _SHR_E_PARAM, "remove_old_ttl_profile");

    /** Initialize variables */
    *remove_old_sip_profile = 0;
    *remove_old_ttl_profile = 0;
    is_first_profile_ref = 0;
    is_last_profile_ref = 0;
    alloc_flags = 0;
    *new_sip_profile = old_sip_profile;
    *new_ttl_profile = old_ttl_profile;
    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    dnx_tunnel_initiator_type_is_ipv6(unit, tunnel->type, &tunnel_type_is_ipv6);
    if (tunnel_type_is_ipv6)
    {
        source_address_entry.address_type = source_address_type_ipv6;
        sal_memcpy(&source_address_entry.address.ipv6_address, &(tunnel->sip6), sizeof(bcm_ip6_t));
    }
    else
    {
        source_address_entry.address_type = source_address_type_ipv4;
        sal_memcpy(&source_address_entry.address.ipv4_address, &(tunnel->sip), sizeof(bcm_ip_t));
    }

    /** Exchange SIP template*/
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.exchange
                    (unit, _SHR_CORE_ALL, alloc_flags, &source_address_entry, old_sip_profile, NULL,
                     (int *) new_sip_profile, &is_first_profile_ref, &is_last_profile_ref));

    /** write to HW if new SIP profile was allocated */
    if (is_first_profile_ref)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_sip_addr_hw_set(unit, tunnel, *new_sip_profile));
    }

    /** If last profile was deallocated remove the old entry from HW */
    if (is_last_profile_ref)
    {
        *remove_old_sip_profile = 1;
    }

    /** Check if new TTL model is PIPE */
    if (tunnel->egress_qos_model.egress_ttl == bcmQosEgressModelUniform)
    {
        is_ttl_pipe_mode_new = FALSE;
    }
    else if (tunnel->egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
    {
        is_ttl_pipe_mode_new = TRUE;
    }
    else
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "NOT support TTL model[%d]", tunnel->egress_qos_model.egress_ttl);
    }
    is_first_profile_ref = 0;
    is_last_profile_ref = 0;

    /** Allocate TTL template */
    if (is_ttl_pipe_mode_old && is_ttl_pipe_mode_new)
    {
        /** In this case the old and new TTL model is PIPE - exchange templates */

        /** Exchange TTL PIPE profile */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.exchange
                        (unit, _SHR_CORE_ALL, alloc_flags, &tunnel->ttl, old_ttl_profile, NULL,
                         (int *) new_ttl_profile, &is_first_profile_ref, &is_last_profile_ref));

        /** If last profile was deallocated remove the old entry from HW */
        if (is_last_profile_ref)
        {
            *remove_old_ttl_profile = 1;
        }
    }
    else if (!is_ttl_pipe_mode_old && is_ttl_pipe_mode_new)
    {
        /** In this case the old model is UNIFORM and new TTL model is PIPE - allocate new profile */

        /** allocate TTL PIPE profile */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                        (unit, _SHR_CORE_ALL, alloc_flags, &tunnel->ttl, NULL, (int *) new_ttl_profile,
                         &is_first_profile_ref));
    }
    else if (is_ttl_pipe_mode_old && !is_ttl_pipe_mode_new)
    {
        /** In this case the old model is PIPE and new TTL model is UNIFORM - remove old template */

        /** Free old TTL profile template */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single
                        (unit, _SHR_CORE_ALL, old_ttl_profile, &is_last_profile_ref));

        /** If last profile was deallocated remove the entry from HW */
        if (is_last_profile_ref)
        {
            *remove_old_ttl_profile = 1;
        }
    }

     /** Write to HW if new TTL profile was allocated */
    if (is_first_profile_ref)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set(unit, *new_ttl_profile, tunnel->ttl));
    }

    old_qos_idx = *new_qos_idx;

    /** Check if new QOS model is PIPE */
    if (tunnel->egress_qos_model.egress_qos == bcmQosEgressModelUniform)
    {
        is_qos_model_pipe_mode_new = FALSE;
    }
    else if ((tunnel->egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
             (tunnel->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
    {
        is_qos_model_pipe_mode_new = TRUE;
    }
    else
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "QOS model does not support [%d] now", tunnel->egress_qos_model.egress_qos);
    }

    /** Exchange QOS idx */
    if (is_qos_model_pipe_mode_old && is_qos_model_pipe_mode_new)
    {
        /** In this case the old and new model is PIPE - exchange templates */

        /** Exchange QOS PIPE profile */
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_update
                        (unit, (uint16) tunnel->dscp, (uint16) tunnel->dscp, 0, (int *) new_qos_idx,
                         &nwk_qos_pipe_first_reference, &nwk_qos_pipe_last_reference));
        if (nwk_qos_pipe_first_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                            (unit, *new_qos_idx, (uint8) tunnel->dscp, (uint8) tunnel->dscp, 0));
        }
    }
    else if (!is_qos_model_pipe_mode_old && is_qos_model_pipe_mode_new)
    {
        /** In this case the old model is UNIFORM and new model is PIPE - allocate new profile */

        /** allocate QOS PIPE profile */
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate
                        (unit, (uint8) tunnel->dscp, (uint8) tunnel->dscp, 0, (int *) new_qos_idx,
                         &nwk_qos_pipe_first_reference));
        if (nwk_qos_pipe_first_reference)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                            (unit, *new_qos_idx, (uint8) tunnel->dscp, (uint8) tunnel->dscp, 0));
        }
    }
    else if (is_qos_model_pipe_mode_old && !is_qos_model_pipe_mode_new)
    {
        /** In this case the old model is PIPE and new model is UNIFORM - remove old template */

        /** Free old QOS profile template */
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                        (unit, (int) old_qos_idx, &nwk_qos_pipe_last_reference));
    }

    if (nwk_qos_pipe_last_reference)
    {
        *remove_old_qos_profile = TRUE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The following function allocates SIP, TTL and QOS profiles. It writes the SIP, TTL and QOS values to HW if new
 * templates were allocated. And returns the created template profiles (sip_profile, ttl_pipe_profile
 * and nwk_qos_profile).
 */
static shr_error_e
dnx_tunnel_initiator_ttl_sip_template_alloc_and_hw_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 *sip_profile,
    uint32 *ttl_pipe_profile,
    uint32 *nwk_qos_profile)
{
    uint8 is_first_ttl_profile_ref;
    uint8 is_first_pipe_propag_profile_ref;
    uint32 alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");
    SHR_NULL_CHECK(sip_profile, _SHR_E_PARAM, "sip_profile");
    SHR_NULL_CHECK(ttl_pipe_profile, _SHR_E_PARAM, "ttl_pipe_profile");
    SHR_NULL_CHECK(nwk_qos_profile, _SHR_E_PARAM, "nwk_qos_profile");

    is_first_ttl_profile_ref = 0;
    alloc_flags = 0;

    *nwk_qos_profile = 0;
    *ttl_pipe_profile = 0;

    /** Allocate SIP index using template manager */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_sip_addr_allocate_and_hw_set(unit, tunnel, sip_profile));

    /** Allocate TTL model */
    if (tunnel->egress_qos_model.egress_ttl != bcmQosEgressModelUniform)
    {
        /** allocate TTL PIPE profile */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                        (unit, _SHR_CORE_ALL, alloc_flags, &tunnel->ttl, NULL, (int *) ttl_pipe_profile,
                         &is_first_ttl_profile_ref));

        /** Write to HW if new TTL profile was allocated */
        if (is_first_ttl_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set(unit, *ttl_pipe_profile, (uint32) tunnel->ttl));
        }
    }

    /** QOS PIPE Model */
    if ((tunnel->egress_qos_model.egress_qos != bcmQosEgressModelUniform))
    {
        /** Allocate QOS profile. The API allocates profile using template manager and writes to HW */
        /** currently only JR mode is supported, in which dcurrent and next layer values are equal and next layer dp
         *  value is 0 */
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate
                        (unit, (uint8) tunnel->dscp, (uint8) tunnel->dscp, 0, (int *) nwk_qos_profile,
                         &is_first_pipe_propag_profile_ref));

        if (is_first_pipe_propag_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                            (unit, *nwk_qos_profile, (uint8) tunnel->dscp, (uint8) tunnel->dscp, 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The following function writes/clears the HW entry for UDP ports profile.
 */
static shr_error_e
dnx_tunnel_udp_ports_profile_hw_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 udp_ports_profile,
    uint8 write_0_clear_1)
{
    uint32 additional_header_specific_information[2];
    uint32 tmp;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify input.
     */
    if (write_0_clear_1 > 1)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "No valid operation given for hw set.");
    }
    if (write_0_clear_1 == 0)
    {
        SHR_NULL_CHECK(tunnel, _SHR_E_INTERNAL, "tunnel");
    }

    /*
     * If it's the first time these ports are used, then add them to the additional headers data table.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES,
                               ADDITIONAL_HEADERS_UDP_USER_DEFINED(udp_ports_profile));

    if (write_0_clear_1 == 0)
    {
        /*
         * By setting TRUE, UDP destination port is set by protocol specific information.
         */
        tmp = TRUE;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                        DBAL_FIELD_DESTINATION_PORT_TYPE, &tmp,
                                                        additional_header_specific_information));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION, DBAL_FIELD_SOURCE_PORT_TYPE, &tmp,
                         additional_header_specific_information));

        tmp = tunnel->udp_dst_port;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                        DBAL_FIELD_DESTINATION_PORT, &tmp,
                                                        additional_header_specific_information));

        tmp = tunnel->udp_src_port;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                                        DBAL_FIELD_SOURCE_PORT, &tmp,
                                                        additional_header_specific_information));

        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_UDP_ADDITIONAL_HEADER_SPECIFIC_INFORMATION,
                                         INST_SINGLE, additional_header_specific_information);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The following function allocates/exchanges/removes the UDP ports profile, and returns the created allocated profile
 * and remove from hw indication.
 *
 * For allocation operation: Pass valid pointers in new_udp_ports_profile and write_new_udp_ports_profile, and -1 in NULL
 *                          in old_udp_ports_profile and remove_old_udp_ports_profile respectively.
 * For exchange operation: Pass valid pointers in new_udp_ports_profile, write_new_udp_ports_profile, and
 *                              remove_old_udp_ports_profile and a valid old profile in old_udp_ports_profile.
 * For free operation: Pass valid pointer and value in remove_old_udp_ports_profile and old_udp_ports_profile respectively,
 *                      and NULL in new_udp_ports_profile and write_new_udp_ports_profile.
 */
static shr_error_e
dnx_tunnel_udp_ports_profile_allocator(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 old_udp_ports_profile,
    uint32 *new_udp_ports_profile,
    uint32 *write_new_udp_ports_profile,
    uint32 *remove_old_udp_ports_profile)
{
    udp_ports_t udp_ports;
    int udp_ports_profile;
    uint8 first_reference, last_reference;
    int remove, allocate;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First find which operation is used and verify input.
     */
    allocate = remove = FALSE;

    if (old_udp_ports_profile != -1)
    {
        SHR_NULL_CHECK(remove_old_udp_ports_profile, _SHR_E_INTERNAL, "remove_old_udp_ports_profile");

        *remove_old_udp_ports_profile = FALSE;
        remove = TRUE;
    }

    if (new_udp_ports_profile != NULL || write_new_udp_ports_profile != NULL)
    {
        SHR_NULL_CHECK(new_udp_ports_profile, _SHR_E_INTERNAL, "new_udp_ports_profile");
        SHR_NULL_CHECK(write_new_udp_ports_profile, _SHR_E_INTERNAL, "write_new_udp_ports_profile");
        SHR_NULL_CHECK(tunnel, _SHR_E_INTERNAL, "tunnel");

        udp_ports.dst_port = tunnel->udp_dst_port;
        udp_ports.src_port = tunnel->udp_src_port;
        *write_new_udp_ports_profile = FALSE;
        allocate = TRUE;
    }

    if (!allocate && !remove)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "No valid operation given for allocator.");
    }

    if (allocate && (udp_ports.dst_port != 0 || udp_ports.src_port != 0))
    {
        /*
         * If the new ports are 0, then no allocation is actually possible. This can happen if changing from a UDP tunnel
         * that uses flexible ports to a UDP tunnel using fixed ports.
         */
        if (remove && old_udp_ports_profile != 0)
        {
            /*
             * Exchange.
             * Don't exchange if the old profile is 0, it just means that it's chaging from UDP tunnel with fixed ports
             * to UDP tunnel with flexible ports.
             */
            SHR_IF_ERR_EXIT(algo_tunnel_db.
                            udp_ports_profile.exchange(unit, _SHR_CORE_ALL, 0, &udp_ports, old_udp_ports_profile, NULL,
                                                       &udp_ports_profile, &first_reference, &last_reference));
            *remove_old_udp_ports_profile = last_reference;
        }
        else
        {
            /*
             * Allocate only.
             */
            SHR_IF_ERR_EXIT(algo_tunnel_db.udp_ports_profile.allocate_single(unit, _SHR_CORE_ALL, 0, &udp_ports, NULL,
                                                                             &udp_ports_profile, &first_reference));
        }
        *write_new_udp_ports_profile = first_reference;
        *new_udp_ports_profile = udp_ports_profile;
    }
    else if (remove)
    {
        /*
         * Remove only.
         */
        SHR_IF_ERR_EXIT(algo_tunnel_db.
                        udp_ports_profile.free_single(unit, _SHR_CORE_ALL, old_udp_ports_profile, &last_reference));
        *remove_old_udp_ports_profile = last_reference;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write to IP tunnel encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds
 *     information for the IPv4/IPv6 tunnel encapsulation entry,
 *     see
 *     \ref bcm_tunnel_initiator_t.
 *   \param [in] dbal_table_id - Relevant dbal table ID
 *   \param [in] tunnel_result_type - Relevant dbal result type
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [in] next_local_outlif -
 *     Local-Out-LIF for DIP6 or arp whose entry should be added
 *     to out-LIF table. This parameter is not used when called
 *     for replace functionality.
 *   \param [in] sip_profile -
 *      Allocated SIP profile (template managed).
 *   \param [in] ttl_pipe_profile -
 *      Allocated TTL PIPE profile (template managed).
 *   \param [in] nwk_qos_profile -
 *      Allocated QOS PIPE profile (template managed).
 *   \param [in] out_lif_llvp_profile -
 *      Allocated Out LIF LLVP profile (template managed or Egress Default Native AC).
 *   \param [in] esem_cmd_profile -
 *      Allocated esem access cmd profile (template managed).
 *   \param [in] lif_additional_header_profile -
 *      Use -1 if it's a fixed value for entry type, or template manager if it's a profile
 *      that can be allocated.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_IPV6_TUNNEL
 */
static shr_error_e
dnx_tunnel_initiator_out_lif_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    dbal_tables_e dbal_table_id,
    uint32 tunnel_result_type,
    uint32 local_outlif,
    uint32 next_local_outlif,
    uint32 sip_profile,
    uint32 ttl_pipe_profile,
    uint32 nwk_qos_profile,
    uint32 out_lif_llvp_profile,
    uint32 esem_cmd_profile,
    uint32 lif_additional_header_profile)
{
    uint32 entry_handle_id;
    uint32 tunnel_is_ipv6;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    dnx_tunnel_initiator_type_is_ipv6(unit, tunnel->type, &tunnel_is_ipv6);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, tunnel_result_type);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_local_outlif);

    /** For vxlan only, set esem command: indicate that a ESEM
     *  lookup is expected, the ESEM result is the VNI */
    if ((tunnel->type == bcmTunnelTypeVxlan) || (tunnel->type == bcmTunnelTypeVxlan6) ||
        (tunnel->type == bcmTunnelTypeVxlanGpe) || (tunnel->type == bcmTunnelTypeVxlan6Gpe))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, esem_cmd_profile);
    }

    if (!tunnel_is_ipv6)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_HEADER_DF_FLAG, INST_SINGLE,
                                     _SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_IPV4_SET_DF));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE, tunnel->dip);
    }
    if ((tunnel->type == bcmTunnelTypeGreAnyIn4) || (tunnel->type == bcmTunnelTypeUdp)
        || (tunnel->type == bcmTunnelTypeGreAnyIn6)
        || (tunnel->type == bcmTunnelTypeVxlan) || (tunnel->type == bcmTunnelTypeVxlan6)
        || (tunnel->type == bcmTunnelTypeVxlanGpe) || (tunnel->type == bcmTunnelTypeVxlan6Gpe))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_ADDITIONAL_HEADER_PROFILE, INST_SINGLE,
                                     lif_additional_header_profile);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, sip_profile);

     /** TTL decrement is always set */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_DECREMENT_DISABLE, INST_SINGLE, FALSE);

    /** remark profile always set, used for previous layer */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                 DNX_QOS_MAP_PROFILE_GET(tunnel->qos_map_id));

    /** Set TTL model */
    if (tunnel->egress_qos_model.egress_ttl == bcmQosEgressModelUniform)
    {
        /** TTL UNIFORM model */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_TTL_MODE_UNIFORM);
    }
    else
    {
         /** TTL PIPE model */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, ttl_pipe_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_TTL_MODE_PIPE);
    }

    /** Set QOS model and profile */
    SHR_IF_ERR_EXIT(dnx_qos_egress_model_type_to_model(unit, tunnel->egress_qos_model.egress_qos, &encap_qos_model));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, encap_qos_model);

    if ((tunnel->egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
        (tunnel->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_profile);
    }

    /** Set llvp_profile to Default Native AC */
    if (dnx_tunnel_utility_llvp_profile_exist(unit, tunnel_result_type, tunnel_is_ipv6) == TRUE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PROFILE, INST_SINGLE, out_lif_llvp_profile);
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write to IP tunnel encapsulation out-LIF data entry table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information for the IPv4 tunnel encapsulation entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *    \param [in] next_local_outlif - next local out lif for
 *           outlif entry
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_DATA_ENTRY
 */
static shr_error_e
dnx_tunnel_initiator_ipv6_data_entry_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 local_outlif,
    uint32 next_local_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, tunnel->dip6);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_local_outlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE,
                                 DBAL_DEFINE_DATA_ENTRY_RESERVED_ETPS_TYPE_MSBS);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * read from IP tunnel encapsulation out-LIF data entry table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds
 *     information for the IPv6 tunnel encapsulation entry, see
 *     \ref bcm_tunnel_initiator_t.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [in] eedb_data_entry_result_type - result type of the eedb data entry
 *    \param [out] local_next_outlif - next local out lif for
 *           outlif entry
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is read from DBAL_TABLE_EEDB_DATA_ENTRY
 */
static shr_error_e
dnx_tunnel_initiator_ipv6_data_entry_table_get(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 local_outlif,
    uint32 eedb_data_entry_result_type,
    uint32 *local_next_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

   /** read DATA_ENTRY table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 eedb_data_entry_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (eedb_data_entry_result_type == DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6)
    {
        dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, tunnel->dip6);

        /** Get next outlif pointer - an ARP entry if exist */
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                            INST_SINGLE, local_next_outlif);

    }
    else
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Illegal result type for table EEDB_DATA_ENTRY[%d]", eedb_data_entry_result_type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The tunnel id may be passed to the API by tunnel->tunnel_id or intf->l3a_intf_id. This function converts both
 * parameters to GPort ids and returns the value, in addition it also returns indication of whether the found GPort
 * is valid.
 * \param [in] unit -
 *     The unit number.
 * \param [in] intf -
 *     * intf->l3a_intf_id - if set, global if id of the tunnel to get.
 * \param [in] tunnel -
 *      * tunnel->tunnel_id - if set, global GPORT id of the tunnel to get.
 * \param [out] tunnel_gport -
 *      GPort of the tunnel to get.
 * \param [out] is_tunneled -
 *      indication that the returned GPort is a valid tunnel GPort.
*/
static shr_error_e
dnx_tunnel_initiator_gport_get(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t * tunnel_gport,
    int *is_tunneled)
{
    bcm_gport_t gport_from_l3_intf;

    int gport_from_l3_intf_valid;
    int gport_from_tunnel_valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "intf");
    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");
    SHR_NULL_CHECK(tunnel_gport, _SHR_E_PARAM, "tunnel_gport");
    SHR_NULL_CHECK(is_tunneled, _SHR_E_PARAM, "is_tunneled");

    *is_tunneled = 0;
    gport_from_l3_intf_valid = 0;
    gport_from_tunnel_valid = 0;

    /** get from tunnel interface */
    if (BCM_GPORT_IS_TUNNEL(tunnel->tunnel_id))
    {
        gport_from_tunnel_valid = 1;
    }

    /** get from l3 intf */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_from_l3_intf, intf->l3a_intf_id);
    if (BCM_GPORT_IS_TUNNEL(gport_from_l3_intf))
    {
        gport_from_l3_intf_valid = 1;
    }

     /** check that tunnel and l3 intf have same GPort */
    if (gport_from_l3_intf_valid && gport_from_tunnel_valid && (tunnel->tunnel_id != gport_from_l3_intf))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "tunnel-id given in interface and tunnel-gport mismatch");
    }

    /** Set found tunnel GPort value and indication */
    if (gport_from_l3_intf_valid)
    {
        *tunnel_gport = gport_from_l3_intf;
        *is_tunneled = 1;
    }
    else if (gport_from_tunnel_valid)
    {
        *tunnel_gport = tunnel->tunnel_id;
        *is_tunneled = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function receives local-out-lif of the tunnel to return, reads all relevant fields from dbal and
 * sets returned tunnel struct parameters. It also checks thet the returned entry is of IPv4-Tunnel type.
*/
static shr_error_e
dnx_tunnel_initiator_out_lif_entry_get(
    int unit,
    uint32 local_out_lif,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    bcm_tunnel_initiator_t * tunnel)
{
    uint32 entry_handle_id;
    uint32 sip_profile;
    uint32 local_next_outlif = 0;
    uint32 arp_next_outlif = 0;
    uint32 flags;
    uint32 ttl_mode;
    uint32 ttl_pipe_profile;
    uint32 qos_model;
    uint32 qos_index;
    bcm_qos_egress_model_type_t qos_model_type;
    uint32 remark_profile = 0;
    uint8 dscp;
    int ref_count;
    source_address_entry_t source_address_entry;
    uint8 qos_profile_get_dummy_var;
    uint32 lif_additional_header_profile = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    flags = 0;
    ref_count = 0;
    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Not all ipv4/ipv6 eedb table have the lif_additional_header_profile field
     */
    if (((dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL) &&
         ((result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE) ||
          (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_GRE_GENEVE) ||
          (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP))) ||
        ((dbal_table_id == DBAL_TABLE_EEDB_IPV6_TUNNEL) &&
         ((result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN) ||
          (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GRE))))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LIF_ADDITIONAL_HEADER_PROFILE, INST_SINGLE,
                         &lif_additional_header_profile));
    }
    /** use result type to get tunnel type */
    SHR_IF_ERR_EXIT(dnx_tunnel_init_dbal_to_bcm_tunnel_type
                    (unit, dbal_table_id, result_type, lif_additional_header_profile, &(tunnel->type),
                     &(tunnel->flags)));
    if (dbal_table_id == DBAL_TABLE_EEDB_IPV6_TUNNEL)
    {
        /** Get next outlif pointer - Dip 6 entry */
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                            &local_next_outlif);

        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, &sip_profile);

         /** get SIP value from template manager */
        SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.profile_data_get
                        (unit, _SHR_CORE_ALL, sip_profile, &ref_count, (void *) &source_address_entry));
        sal_memcpy(tunnel->sip6, source_address_entry.address.ipv6_address, sizeof(bcm_ip6_t));

        /** read DATA_ENTRY table */
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_ipv6_data_entry_table_get
                        (unit, tunnel, local_next_outlif, DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6, &arp_next_outlif));
    }
    else
    {
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_HEADER_DF_FLAG, INST_SINGLE, &flags);
        if (flags)
        {
            tunnel->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;
        }
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, INST_SINGLE, &(tunnel->dip));
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, &sip_profile);

        /** get SIP value from template manager */
        SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.profile_data_get
                        (unit, _SHR_CORE_ALL, sip_profile, &ref_count, (void *) &source_address_entry));
        tunnel->sip = source_address_entry.address.ipv4_address;

        /** Get next outlif pointer - an ARP entry if exist */
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                            &arp_next_outlif);
    }

    /** Get TTL configuration */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                                        &ttl_mode));
    if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
    {
        tunnel->egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE,
                                                            INST_SINGLE, &ttl_pipe_profile));
        /** get TTL value from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, ttl_pipe_profile, &ref_count, (void *) &(tunnel->ttl)));
    }
    else if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_UNIFORM)
    {
        tunnel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    }

    /** Get QOS configuration */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                                        &qos_model));

    SHR_IF_ERR_EXIT(dnx_qos_egress_model_to_model_type(unit, qos_model, &qos_model_type));
    tunnel->egress_qos_model.egress_qos = qos_model_type;

    if ((qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE) ||
        (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                            INST_SINGLE, &qos_index));
        /** get DHCP value from template manager */
        /** Currently: network_qos_pipe=network_qos_pipe_mapped and dp_pipe_mapped=0 */
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get
                        (unit, qos_index, &dscp, &qos_profile_get_dummy_var, &qos_profile_get_dummy_var));
        tunnel->dscp = (int) dscp;
    }
    else
    {
        tunnel->dscp = 0;
    }

    /** remark profile, used for previous layer */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE,
                                                        INST_SINGLE, &remark_profile));
    if (remark_profile != DNX_QOS_INITIAL_MAP_ID)
    {
        DNX_QOS_REMARK_MAP_SET(remark_profile);
        DNX_QOS_EGRESS_MAP_SET(remark_profile);
        tunnel->qos_map_id = remark_profile;
    }

    /*
     * UDP src/dst ports.
     */
    if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP && lif_additional_header_profile != 0)
    {
        udp_ports_t udp_ports;

        SHR_IF_ERR_EXIT(algo_tunnel_db.udp_ports_profile.profile_data_get(unit, _SHR_CORE_ALL,
                                                                          lif_additional_header_profile, NULL,
                                                                          &udp_ports));

        tunnel->udp_src_port = udp_ports.src_port;
        tunnel->udp_dst_port = udp_ports.dst_port;
    }

    if (arp_next_outlif != 0)
    {
        /** map local to global LIF and set */
        bcm_gport_t next_gport = BCM_GPORT_INVALID;
        /** In case local lif is not found, return next pointer 0 */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                    arp_next_outlif, &next_gport));
        if (next_gport != BCM_GPORT_INVALID)
        {
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel->l3_intf_id, next_gport);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function receives local_oulif id, validates that it is an
 * ip tunnel eedb entry, and returns template managed profile
 * indices.
*/
static shr_error_e
dnx_tunnel_initiator_eedb_entry_get(
    int unit,
    uint32 dbal_table_id,
    uint32 local_out_lif,
    uint32 result_type,
    uint32 *next_local_outlif,
    uint32 *sip_profile,
    uint32 *ttl_pipe_profile,
    uint32 *is_ttl_pipe_mode,
    uint32 *qos_model_idx,
    uint32 *is_qos_model_pipe_mode,
    uint32 *out_lif_llvp_profile,
    uint32 *esem_cmd_profile,
    uint32 *lif_additional_header_profile)
{
    uint32 entry_handle_id;
    bcm_tunnel_type_t tunnel_type = 0;
    uint32 ttl_mode;
    uint32 qos_model;
    uint32 tunnel_flags = 0;
    uint32 tunnel_is_ipv6;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sip_profile, _SHR_E_PARAM, "sip_profile");
    SHR_NULL_CHECK(ttl_pipe_profile, _SHR_E_PARAM, "ttl_pipe_profile");
    SHR_NULL_CHECK(is_ttl_pipe_mode, _SHR_E_PARAM, "is_ttl_pipe_mode");
    SHR_NULL_CHECK(is_qos_model_pipe_mode, _SHR_E_PARAM, "is_qos_model_pipe_mode");

    *is_ttl_pipe_mode = 0;
    *is_qos_model_pipe_mode = 0;

    /** Read EEDB entry from DBAL */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get and verify tunnel type support */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                        &result_type));

    /*
     * Not all ipv4/ipv6 eedb table have the lif_additional_header_profile field
     */
    if (((dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL) &&
         ((result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE) ||
          (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_GRE_GENEVE) ||
          (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP))) ||
        ((dbal_table_id == DBAL_TABLE_EEDB_IPV6_TUNNEL) &&
         ((result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN) ||
          (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GRE))))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LIF_ADDITIONAL_HEADER_PROFILE, INST_SINGLE,
                         lif_additional_header_profile));
    }

    /** Get esem cmd Profile, Not all IPV4_TUNNEL entries have this field */
    *esem_cmd_profile = dnx_data_esem.access_cmd.no_action_get(unit);
    if ((dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL)
        && (result_type != DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, esem_cmd_profile));
    }

    /** use result type to get tunnel type */
    SHR_IF_ERR_EXIT(dnx_tunnel_init_dbal_to_bcm_tunnel_type
                    (unit, dbal_table_id, result_type, *lif_additional_header_profile, &tunnel_type, &tunnel_flags));
    dnx_tunnel_initiator_type_is_ipv6(unit, tunnel_type, &tunnel_is_ipv6);

    /** Get next local outlif */
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                        next_local_outlif);

    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_tunnel_type_verify(unit, tunnel_type));

    /** Get SIP Profile */
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, sip_profile);

    /** Get TTL mode */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                                        &ttl_mode));
    if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
    {
        /** Get TTL PIPE profile */
        *is_ttl_pipe_mode = 1;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE,
                                                            INST_SINGLE, ttl_pipe_profile));
    }

    /** Get QOS index */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                                        &qos_model));
    if ((qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE))
    {
        /** Get QOS pipe index */
        *is_qos_model_pipe_mode = 1;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                            INST_SINGLE, qos_model_idx));
    }

    if (dnx_tunnel_utility_llvp_profile_exist(unit, result_type, tunnel_is_ipv6) == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LLVP_PROFILE, INST_SINGLE, out_lif_llvp_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function receives template profiles and removes them from template manager.
 * Supports SIP, TTL and QOS templates.
*/
static shr_error_e
dnx_tunnel_initiator_template_entries_free(
    int unit,
    uint32 local_out_lif,
    uint32 sip_profile,
    uint32 ttl_pipe_profile,
    uint32 is_ttl_pipe_mode,
    uint8 *remove_sip_profile,
    uint8 *remove_ttl_pipe_profile,
    uint32 qos_model_idx,
    uint8 *remove_qos_pipe_profile,
    uint32 is_qos_model_pipe_mode)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(remove_sip_profile, _SHR_E_PARAM, "remove_sip_profile");
    SHR_NULL_CHECK(remove_ttl_pipe_profile, _SHR_E_PARAM, "remove_ttl_pipe_profile");

    *remove_sip_profile = 0;
    *remove_ttl_pipe_profile = 0;

    /** Free SIP template */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.free_single
                    (unit, _SHR_CORE_ALL, sip_profile, remove_sip_profile));

    /** Free TTL profile template */
    if (is_ttl_pipe_mode)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single
                        (unit, _SHR_CORE_ALL, ttl_pipe_profile, remove_ttl_pipe_profile));
    }

    /** remove QOS profile from template manager */
    if (is_qos_model_pipe_mode)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                        (unit, (int) qos_model_idx, remove_qos_pipe_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function receives template profiles and removes them from HW.
 * Currently supports only SIP and TTL templates.
*/
static shr_error_e
dnx_tunnel_initiator_sip_ttl_qos_table_delete(
    int unit,
    uint32 local_out_lif,
    uint32 sip_profile,
    uint32 ttl_pipe_profile,
    uint32 qos_pipe_profile,
    uint8 remove_sip_profile,
    uint8 remove_ttl_profile,
    uint8 remove_qos_profile,
    uint32 tunnel_is_ipv6)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Remove SIP entry from HW */
    if (remove_sip_profile)
    {
        if (tunnel_is_ipv6)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, sip_profile);

        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, sip_profile);
        }
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Remove TTL entry from HW */
    if (remove_ttl_profile)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, ttl_pipe_profile));
    }
    /** Remove qos entry from HW */
    if (remove_qos_profile)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, qos_pipe_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Map IP-Tunnel EEDB access stage to lif_mngr eedb phase.
 */
static shr_error_e
dnx_tunnel_initiator_encap_access_to_outlif_phase(
    int unit,
    bcm_encap_access_t encap_access,
    lif_mngr_outlif_phase_e * outlif_phase)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(outlif_phase, _SHR_E_PARAM, "outlif_phase");

    switch (encap_access)
    {
        case bcmEncapAccessInvalid:
        case bcmEncapAccessTunnel1:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1;
            break;
        }
        case bcmEncapAccessTunnel2:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_2;
            break;
        }
        case bcmEncapAccessTunnel3:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_3;
            break;
        }
        case bcmEncapAccessTunnel4:
        {
            *outlif_phase = LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_4;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel encapsulation encap_access value (%d) is not supported", encap_access);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write to ERSPAN encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information for ERSPAN encapsulation entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *    \param [in] next_local_outlif - next local out lif for
 *           outlif entry
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_ERSPAN.
 */
static shr_error_e
dnx_tunnel_initiator_erspan_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 local_outlif,
    uint32 next_local_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_ERSPAN, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Set DATA fields */
    if (tunnel->flags & BCM_TUNNEL_INIT_ERSPAN_TYPE3)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_EEDB_ERSPAN_ETPS_ERSPANV3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWITCH_ID, INST_SINGLE, tunnel->switch_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_ID, INST_SINGLE, tunnel->hw_id);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_EEDB_ERSPAN_ETPS_ERSPANV2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERSPANV2_HEADER_INDEX_17_19, INST_SINGLE,
                                     tunnel->aux_data);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, next_local_outlif);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * read from ERSPAN encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [in] eedb_data_entry_result_type - result type of the eedb data entry
 *   \param [out] next_local_outlif - next local out lif for
 *           outlif entry
 *   \param [out] tunnel - A pointer to the struct that holds
 *     information for ERSPAN tunnel encapsulation entry, see
 *     \ref bcm_tunnel_initiator_t.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is read from DBAL_TABLE_EEDB_ERSPAN.
 */
static shr_error_e
dnx_tunnel_initiator_erspan_table_get(
    int unit,
    uint32 local_outlif,
    uint32 eedb_data_entry_result_type,
    uint32 *next_local_outlif,
    bcm_tunnel_initiator_t * tunnel)
{
    uint32 entry_handle_id, ip_out_lif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_ERSPAN, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 eedb_data_entry_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Read ERSPAN type */
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                        &eedb_data_entry_result_type);

    if (tunnel != NULL)
    {
        tunnel->type = bcmTunnelTypeErspan;

        /** Get DATA fields */
        if (eedb_data_entry_result_type == DBAL_RESULT_TYPE_EEDB_ERSPAN_ETPS_ERSPANV3)
        {
            tunnel->flags |= BCM_TUNNEL_INIT_ERSPAN_TYPE3;
            dbal_entry_handle_value_field16_get(unit, entry_handle_id, DBAL_FIELD_SWITCH_ID, INST_SINGLE,
                                                &tunnel->switch_id);
            dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_HW_ID, INST_SINGLE, &tunnel->hw_id);
        }
        else if (eedb_data_entry_result_type == DBAL_RESULT_TYPE_EEDB_ERSPAN_ETPS_ERSPANV2)
        {
            dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ERSPANV2_HEADER_INDEX_17_19,
                                                INST_SINGLE, &tunnel->aux_data);
        }
        else
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "Illegal result type for table EEDB_ERSPAN[%d]", eedb_data_entry_result_type);
        }
    }

    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                        &ip_out_lif);

    if (tunnel != NULL)
    {
        if (ip_out_lif != 0)
        {
            /** map local to global LIF and set */
            bcm_gport_t next_gport = BCM_GPORT_INVALID;
            /** In case local lif is not found, return next pointer 0 */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                        ip_out_lif, &next_gport));
            if (next_gport != BCM_GPORT_INVALID)
            {
                BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel->l3_intf_id, next_gport);
            }
        }
    }

    if (next_local_outlif != NULL)
    {
        *next_local_outlif = ip_out_lif;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * clear ERSPAN tunnel initiator.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] hw_res -
 *     A pointer to ERSPAN tunnel hw resources.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * Clears ERSPAN entry in DBAL_TABLE_EEDB_ERSPAN.
 */
static shr_error_e
dnx_tunnel_initiator_erspan_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * hw_res)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, hw_res->global_out_lif));

    /** Remove tunnel data from EEDB outlif table */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_dbal_entry_clear
                    (unit, hw_res->outlif_dbal_table_id, DBAL_FIELD_OUT_LIF,
                     hw_res->local_out_lif, hw_res->outlif_dbal_result_type));

    /** Delete global and local out lifs allocations */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, hw_res->global_out_lif, NULL, hw_res->local_out_lif));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write to RSPAN Advanced encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds information for RSPAN Advanced encapsulation entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_RSPAN.
 */
static shr_error_e
dnx_tunnel_initiator_rspan_advanced_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 local_outlif)
{
    uint32 old_esem_cmd_profile;
    uint32 new_esem_cmd_profile;
    uint8 remove_old_esem_cmd_profile = 0;
    uint8 set_new_esem_cmd_profile = 0;
    dnx_esem_cmd_data_t esem_cmd_data;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Allocate an esem access cmd and configure the cmd table. */
    old_esem_cmd_profile = dnx_data_esem.access_cmd.no_action_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, tunnel->tunnel_id,
                                               DNX_PORT_ESEM_CMD_ALLOCATE | DNX_PORT_ESEM_CMD_RANGE_0_63,
                                               DBAL_ENUM_FVAL_ESEM_APP_DB_ID_SSP_NAMESPACE,
                                               ESEM_ACCESS_TYPE_PORT_SA,
                                               DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID,
                                               (int) old_esem_cmd_profile,
                                               (int *) &new_esem_cmd_profile, &esem_cmd_data,
                                               &set_new_esem_cmd_profile, &remove_old_esem_cmd_profile));

    if (set_new_esem_cmd_profile == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, new_esem_cmd_profile, esem_cmd_data));
    }

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RSPAN, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_RSPAN_ETPS_RSPAN);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, new_esem_cmd_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE, tunnel->class_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PROFILE, INST_SINGLE, 0);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * read from RSPAN Advanced encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [out] tunnel - A pointer to the struct that holds
 *     information for RSPAN Advanced tunnel encapsulation entry, see
 *     \ref bcm_tunnel_initiator_t.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is read from DBAL_TABLE_EEDB_RSPAN.
 */
static shr_error_e
dnx_tunnel_initiator_rspan_advanced_table_get(
    int unit,
    uint32 local_outlif,
    bcm_tunnel_initiator_t * tunnel)
{
    uint32 entry_handle_id, class_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RSPAN, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_RSPAN_ETPS_RSPAN);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get DATA fields */
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE, &class_id);

    tunnel->type = bcmTunnelTypeRspanAdvanced;
    tunnel->class_id = (uint16) class_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write to RSPAN Advanced encapsulation out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] hw_res - A pointer to RSPAN Advanced tunnel hw resources.
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * Clears RSPAN Advanced entry in DBAL_TABLE_EEDB_RSPAN.
 */
static shr_error_e
dnx_tunnel_initiator_rspan_advanced_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * hw_res)
{
    uint32 esem_cmd_profile;
    uint8 remove_esem_cmd_profile;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    remove_esem_cmd_profile = 0;

    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, hw_res->global_out_lif));

    /** Read EEDB entry from DBAL */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RSPAN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, hw_res->local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_RSPAN_ETPS_RSPAN);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, &esem_cmd_profile));

    /** remove ESEM cmd profile from template manager */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_free(unit, esem_cmd_profile, &remove_esem_cmd_profile));

    if (remove_esem_cmd_profile == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, esem_cmd_profile));
    }

    /** Remove tunnel data from EEDB outlif table */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_dbal_entry_clear
                    (unit, hw_res->outlif_dbal_table_id, DBAL_FIELD_OUT_LIF,
                     hw_res->local_out_lif, hw_res->outlif_dbal_result_type));

    /** Delete global and local out lifs allocations */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, hw_res->global_out_lif, NULL, hw_res->local_out_lif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * End of Inner functions
 * }
 */

/*
 * APIs
 * {
 */
/**
 * \brief
 * Create IP-Tunnel initiator object and set some of its properties.
 *
 *    \param [in] unit -Relevant unit.
 *    \param [in] intf -
 *              * intf->l3a_intf_id - As output - global if id of the created tunnel.
 *    \param [in] tunnel -
 *     A pointer to the struct that holds information for the
 *     IPv4/IPv6 tunnel initiator entry, see bcm_tunnel_initiator_t.
 *     *
 *     Fields description:
 *              * tunnel->flags:
 *                  * BCM_TUNNEL_WITH_ID -
 *                    create a tunnel with ID passed by tunnel->tunnel_id or intf->l3a_intf_id.
 *                  * BCM_TUNNEL_REPLACE -
 *                    replace an already allocated tunnel initiator entry.
 *                    The flag can be used only with the BCM_TUNNEL_WITH_ID flag and a valid tunnel_id.
 *                  * BCM_TUNNEL_INIT_GRE_KEY_USE_LB - used with bcmTunnelTypeGreAnyIn4 tunnel type.
 *                    If this flag is set a Load balancing key will be added to the GRE4 header.
 *                  When using this mode the ttl value will be inherited from the forwarding value and decreased by 1.
 *                  * BCM_TUNNEL_INIT_IPV4_SET_DF - set Ipv4 Defragment.
 *              * tunnel->type - Encapsulated tunnel type. Supported tunnel types:
 *                  * bcmTunnelTypeGreAnyIn4
 *                  * bcmTunnelTypeIpAnyIn4
 *                  * bcmTunnelTypeUdp
 *                  * bcmTunnelTypeIpAnyIn6
 *                  * bcmTunnelTypeGreAnyIn6
 *                  * bcmTunnelTypeUdp6
 *                  * bcmTunnelTypeVxlan
 *                  * bcmTunnelTypeErspan
 *                  * bcmTunnelTypeRspanAdvanced
 *                  * bcmTunnelTypeVxlan6:
 *              * tunnel->dip - DIP of the initialized IPV4 tunnel header.
 *                  (This field will be not used when tunnel type is ERSPAN or IPV6)
 *              * tunnel->sip - SIP of the initialized IPV4 tunnel header.
 *                  (This field will be not used when tunnel type is ERSPAN. or IPV6)
 *              * tunnel->dip6 - DIP of the initialized IPV6 tunnel header.
 *              * tunnel->sip6 - SIP of the initialized IPV6 tunnel header.
 *              * tunnel->ttl - value of the ttl field on the encapsulated tunnel in pipe mode.
 *              * tunnel->egress_qos_model: QOS and TTL model.
 *              * tunnel->dscp - dscp value to be used in case of pipe mode of inheritance.
 *              * tunnel->tunnel_id - As output - global GPORT id of the created tunnel.
 *              * tunnel->l3_intf_id - Next outlif pointer if id (usually ARP entry).
 *                (It is usually IP entry when tunnel type is ERSPAN.)
 *              * tunnel->aux_data - index_17_19 of ERSPANv2 header.
 *              * tunnel->hw_id - Unique identifier of an ERSPANv3 engine within a system.
 *              * tunnel->switch_id - Identifies a source switch in ERSPANv3 header.
 * \return
 *    \retval Zero in case of NO ERROR.
 *       \retval Negative in case of an error, see \ref shr_error_e.
 * \par INDIRECT OUTPUT
 *   * Allocated tunnel initiator Local-Out-LIF and Global-Out-LIF.
 *   * Allocated tunnel initiator DIP6 Local-Out-LIF and
 *     Global-Out-LIF.
 *   * Allocate SIP index. intf->l3a_intf_id - See
 *     'intf->l3a_intf_id' in DIRECT INPUT above.
 *   * tunnel->tunnel_id - See 'tunnel->tunnel_id' in DIRECT INPUT above.
 *   * Write to tunnel initiator out-LIF table
 *     (DBAL_TABLE_EEDB_IPV4_TUNNEL/DBAL_TABLE_EEDB_IPV6_TUNNEL).
 *   * Write to tunnel initiator DATA_ENTRY table
 *     (DBAL_TABLE_EEDB_DATA_ENTRY)
 *   * Write to IP source address table (DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4).
 *   * Write to ERSPAN table when tunnel type is ERSPAN (DBAL_TABLE_EEDB_ERSPAN).
 */
int
bcm_dnx_tunnel_initiator_create(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    bcm_gport_t arp_id = 0;
    int global_lif_id;
    lif_mngr_local_outlif_info_t outlif_info;
    lif_mngr_local_outlif_info_t dip_outlif_info;
    uint32 lif_alloc_flags;
    int is_tunneled;
    bcm_gport_t tunnel_gport = 0;
    uint32 lif_get_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 old_sip_profile;
    uint32 old_ttl_pipe_profile;
    uint32 old_esem_cmd_profile;
    uint32 new_sip_profile;
    uint32 new_ttl_pipe_profile;
    uint32 new_esem_cmd_profile;
    uint32 is_ttl_pipe_mode;
    uint32 nwk_qos_profile;
    uint8 remove_old_sip_profile = 0;
    uint8 remove_old_ttl_profile = 0;
    uint8 remove_old_qos_profile = 0;
    uint8 remove_old_esem_cmd_profile = 0;
    uint8 set_new_esem_cmd_profile = 0;
    uint32 esem_cmd_range_flags;
    uint32 qos_model_idx;
    uint32 is_qos_model_pipe_mode;
    uint32 tunnel_is_ipv6;
    dbal_tables_e dbal_table_id;
    uint32 next_local_outlif = 0;
    uint32 arp_out_lif = 0;
    uint32 lif_additional_header_profile = 0;
    uint32 out_lif_llvp_profile;
    dnx_esem_cmd_data_t esem_cmd_data;
    uint32 old_udp_ports_profile = 0;
    uint32 new_udp_ports_profile = 0;
    uint32 write_new_udp_ports_profile = FALSE;
    uint32 remove_old_udp_ports_profile = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    lif_get_flags = 0;
    is_tunneled = 0;
    lif_alloc_flags = 0;

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_initiator_create_verify(unit, intf, tunnel));

    dnx_tunnel_initiator_type_is_ipv6(unit, tunnel->type, &tunnel_is_ipv6);
    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    /** WITH_ID flag is used - get global out-lif ID */
    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_WITH_ID))
    {
        /** Get tunnel GPORT if WITH_ID flag is set */
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_gport_get(unit, intf, tunnel, &tunnel_gport, &is_tunneled));
        /** Add alloc_with_id flag */
        global_lif_id = BCM_GPORT_TUNNEL_ID_GET(tunnel_gport);
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }

    if (tunnel->l3_intf_id != 0)
    {
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(arp_id, tunnel->l3_intf_id);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, arp_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));

        arp_out_lif = gport_hw_resources.local_out_lif;
    }

    /** Allocate new tunnel out-LIF - REPLACE flag is not set */
    if (!_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        if (tunnel_is_ipv6)
        {
            dbal_table_id = DBAL_TABLE_EEDB_IPV6_TUNNEL;
            if (tunnel->type != bcmTunnelTypeSR6)
            {
                /** create the second local lif for DIP */
                /** fill the local outlif info. */
                sal_memset(&dip_outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
                dip_outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
                dip_outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6;

                SHR_IF_ERR_EXIT(dnx_tunnel_initiator_encap_access_to_outlif_phase
                                (unit, tunnel->encap_access + 1, &dip_outlif_info.outlif_phase));
                /** Allocate LIF  */
                SHR_IF_ERR_EXIT(dnx_lif_lib_allocate
                                (unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &dip_outlif_info));

                next_local_outlif = dip_outlif_info.local_outlif;
            }
            else
            {
                next_local_outlif = arp_out_lif;
            }
        }
        else if (tunnel->type == bcmTunnelTypeErspan)
        {
            dbal_table_id = DBAL_TABLE_EEDB_ERSPAN;
            next_local_outlif = arp_out_lif;
        }
        else if (tunnel->type == bcmTunnelTypeRspanAdvanced)
        {
            dbal_table_id = DBAL_TABLE_EEDB_RSPAN;
        }
        else
        {
            dbal_table_id = DBAL_TABLE_EEDB_IPV4_TUNNEL;
            next_local_outlif = arp_out_lif;
        }

        /** fill the local outlif info. */
        sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
        outlif_info.dbal_table_id = dbal_table_id;
        SHR_IF_ERR_EXIT(dnx_tunnel_init_bcm_to_dbal_result_type
                        (unit, tunnel->type, tunnel->flags, (uint32 *) &outlif_info.dbal_result_type,
                         &lif_additional_header_profile));
        if (tunnel->type != bcmTunnelTypeSR6)
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_encap_access_to_outlif_phase
                            (unit, tunnel->encap_access, &outlif_info.outlif_phase));
        }
        else
        {
            
            outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_IPV6_RAW_SRV6_TUNNEL;
        }
        /** Allocate LIF - WITH_ID if the flag was set */
        SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));

        /** update returned Tunnel ID's */
        BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id, global_lif_id);

        if (tunnel->type == bcmTunnelTypeErspan)
        {
            /** Add entry to ERSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_erspan_table_set
                            (unit, tunnel, outlif_info.local_outlif, next_local_outlif));
        }
        else if (tunnel->type == bcmTunnelTypeRspanAdvanced)
        {
            /** Add entry to RSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_rspan_advanced_table_set(unit, tunnel, outlif_info.local_outlif));
        }
        else
        {
            /** Allocate template managed fields and add them to HW - SIP, TTL, DHCP */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_ttl_sip_template_alloc_and_hw_set
                            (unit, tunnel, &new_sip_profile, &new_ttl_pipe_profile, &nwk_qos_profile));

            /*
             * If the entry type requires lif additional header profile that needs allocation, set it here.
             */
            if ((tunnel->type == bcmTunnelTypeUdp) && ((tunnel->udp_dst_port != 0) || (tunnel->udp_src_port != 0)))
            {
                /*
                 * For udp tunnels that don't use the default src/dst ports, allocate a UDP ports profile and
                 * fill it.
                 */
                SHR_IF_ERR_EXIT(dnx_tunnel_udp_ports_profile_allocator(unit, tunnel, -1, &new_udp_ports_profile,
                                                                       &write_new_udp_ports_profile, NULL));
                lif_additional_header_profile = new_udp_ports_profile;
            }

            if (tunnel_is_ipv6 && (tunnel->type != bcmTunnelTypeSR6))
            {
                /** Add entry to DATA ENTRY table */
                SHR_IF_ERR_EXIT(dnx_tunnel_initiator_ipv6_data_entry_table_set
                                (unit, tunnel, dip_outlif_info.local_outlif, arp_out_lif));
            }

            if (dnx_tunnel_utility_llvp_profile_exist(unit, outlif_info.dbal_result_type, tunnel_is_ipv6) == TRUE)
            {
                out_lif_llvp_profile = dnx_data_port_pp.general.default_egress_native_ac_llvp_profile_id_get(unit);
            }
            else
            {
                /** llvp profile is irrelevant, just set it to zero */
                out_lif_llvp_profile = 0;
            }

            if ((tunnel->type == bcmTunnelTypeVxlan) || (tunnel->type == bcmTunnelTypeVxlan6) ||
                (tunnel->type == bcmTunnelTypeVxlanGpe) || (tunnel->type == bcmTunnelTypeVxlan6Gpe))
            {
                /** Allocate an esem access cmd and configure the cmd table. */
                esem_cmd_range_flags = DNX_PORT_ESEM_CMD_RANGE_0_3;
                old_esem_cmd_profile = dnx_data_esem.access_cmd.no_action_get(unit);
                SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, tunnel->tunnel_id,
                                                           DNX_PORT_ESEM_CMD_ALLOCATE | esem_cmd_range_flags,
                                                           DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_NAMESPACE_CTAG,
                                                           ESEM_ACCESS_TYPE_VXLAN_VNI,
                                                           DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID,
                                                           (int) old_esem_cmd_profile,
                                                           (int *) &new_esem_cmd_profile, &esem_cmd_data,
                                                           &set_new_esem_cmd_profile, &remove_old_esem_cmd_profile));
                if (set_new_esem_cmd_profile == TRUE)
                {
                    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, new_esem_cmd_profile, esem_cmd_data));
                }
            }

            if (write_new_udp_ports_profile)
            {
                SHR_IF_ERR_EXIT(dnx_tunnel_udp_ports_profile_hw_set(unit, tunnel, new_udp_ports_profile, 0));
            }

            /** Add entry to out-LIF table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_out_lif_table_set
                            (unit, tunnel, dbal_table_id, outlif_info.dbal_result_type, outlif_info.local_outlif,
                             next_local_outlif, new_sip_profile, new_ttl_pipe_profile, nwk_qos_profile,
                             out_lif_llvp_profile, new_esem_cmd_profile, lif_additional_header_profile));

        }

        /** Write global to local LIF mapping to GLEM. */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif_id, outlif_info.local_outlif));
    }
    /** Replace existing out-LIF */
    else
    {
        /** get GPort HW resources (global and local tunnel outlif) */
        lif_get_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, tunnel_gport, lif_get_flags, &gport_hw_resources));

        /** Check that eep is an index of a Tunnel OutLIF (in SW DB) */
        if ((!tunnel_is_ipv6 && ((gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_IPV4_TUNNEL)
                                 && (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_ERSPAN))) ||
            (tunnel_is_ipv6 && (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_IPV6_TUNNEL)))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal to replace non IP-Tunnel OutLIF to IP Tunnel OutLIF");
        }

        if (tunnel->type == bcmTunnelTypeErspan)
        {
            /** Get entry information from ERSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_erspan_table_get
                            (unit, gport_hw_resources.local_out_lif, gport_hw_resources.outlif_dbal_result_type,
                             &next_local_outlif, NULL));

            /** Update entry to ERSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_erspan_table_set
                            (unit, tunnel, gport_hw_resources.local_out_lif, next_local_outlif));
        }
        else if (tunnel->type == bcmTunnelTypeRspanAdvanced)
        {
            /** Update entry to RSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_rspan_advanced_table_set
                            (unit, tunnel, gport_hw_resources.local_out_lif));
        }
        else
        {
            /** Validate tunnel type and get template managed profiles */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_eedb_entry_get
                            (unit, gport_hw_resources.outlif_dbal_table_id, gport_hw_resources.local_out_lif,
                             gport_hw_resources.outlif_dbal_result_type, &next_local_outlif, &old_sip_profile,
                             &old_ttl_pipe_profile, &is_ttl_pipe_mode, &qos_model_idx, &is_qos_model_pipe_mode,
                             &out_lif_llvp_profile, &old_esem_cmd_profile, &lif_additional_header_profile));

            /** Replace template managed fields
             *  If needed new TTL, SIP and QOS values are written to HW. Old values are NOT deleted from HW */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_ttl_sip_template_exchange
                            (unit, tunnel, old_sip_profile, old_ttl_pipe_profile, is_ttl_pipe_mode, &new_sip_profile,
                             &new_ttl_pipe_profile, &remove_old_sip_profile, &remove_old_ttl_profile, &qos_model_idx,
                             &remove_old_qos_profile, is_qos_model_pipe_mode));

            if (tunnel_is_ipv6)
            {
                /** Update dip6  entry to DATA ENTRY table */
                SHR_IF_ERR_EXIT(dnx_tunnel_initiator_ipv6_data_entry_table_set
                                (unit, tunnel, next_local_outlif, arp_out_lif));
            }

            /*
             * If the entry type requires lif additional header profile that needs allocation, set it here.
             */
            if ((tunnel->type == bcmTunnelTypeUdp && (tunnel->udp_dst_port != 0 || tunnel->udp_src_port != 0))
                || (gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP
                    && lif_additional_header_profile != DBAL_ENUM_FVAL_LIF_ADDITIONAL_HEADER_PROFILE_LIF_AHP_NONE))
            {
                /*
                 * For udp tunnels that don't use the default src/dst ports, allocate/exchange the UDP ports profile.
                 * Otherwise, if the new tunnel uses the default ports but the last one didn't, then remove the old
                 * profile.
                 */
                old_udp_ports_profile = lif_additional_header_profile;
                SHR_IF_ERR_EXIT(dnx_tunnel_udp_ports_profile_allocator(unit, tunnel, old_udp_ports_profile,
                                                                       &new_udp_ports_profile,
                                                                       &write_new_udp_ports_profile,
                                                                       &remove_old_udp_ports_profile));
                lif_additional_header_profile = new_udp_ports_profile;
            }

            /*
             * Allocate an esem access cmd and configure the cmd table.
             * The old cmd table will be be overriden in case of the old cmd is reused, so it is not cleared here.
             */
            if ((tunnel->type == bcmTunnelTypeVxlan) || (tunnel->type == bcmTunnelTypeVxlan6) ||
                (tunnel->type == bcmTunnelTypeVxlanGpe) || (tunnel->type == bcmTunnelTypeVxlan6Gpe))
            {
                /** Allocate an esem access cmd and configure the cmd table. */
                esem_cmd_range_flags = DNX_PORT_ESEM_CMD_RANGE_0_3;
                SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, tunnel->tunnel_id,
                                                           DNX_PORT_ESEM_CMD_ALLOCATE | esem_cmd_range_flags,
                                                           DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_NAMESPACE_CTAG,
                                                           ESEM_ACCESS_TYPE_VXLAN_VNI,
                                                           DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID,
                                                           (int) old_esem_cmd_profile,
                                                           (int *) &new_esem_cmd_profile, &esem_cmd_data,
                                                           &set_new_esem_cmd_profile, &remove_old_esem_cmd_profile));
                if (set_new_esem_cmd_profile == TRUE)
                {
                    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, new_esem_cmd_profile, esem_cmd_data));
                }
            }

            if (write_new_udp_ports_profile)
            {
                SHR_IF_ERR_EXIT(dnx_tunnel_udp_ports_profile_hw_set(unit, tunnel, new_udp_ports_profile, 0));
            }

            /** Update IP tunnel encapsulation out-LIF table. */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_out_lif_table_set
                            (unit, tunnel, gport_hw_resources.outlif_dbal_table_id,
                             gport_hw_resources.outlif_dbal_result_type, gport_hw_resources.local_out_lif,
                             next_local_outlif, new_sip_profile, new_ttl_pipe_profile, qos_model_idx,
                             out_lif_llvp_profile, new_esem_cmd_profile, lif_additional_header_profile));

            /** Remove deallocated templates from HW */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_sip_ttl_qos_table_delete
                            (unit, gport_hw_resources.local_out_lif,
                             old_sip_profile, old_ttl_pipe_profile, qos_model_idx, remove_old_sip_profile,
                             remove_old_ttl_profile, remove_old_qos_profile, tunnel_is_ipv6));
            if (remove_old_esem_cmd_profile == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, old_esem_cmd_profile));
            }

            if (remove_old_udp_ports_profile)
            {
                SHR_IF_ERR_EXIT(dnx_tunnel_udp_ports_profile_hw_set(unit, NULL, old_udp_ports_profile, 1));
            }
        }
    }

    /** update returned Interface ID's */
    BCM_L3_ITF_SET(intf->l3a_intf_id, BCM_L3_ITF_TYPE_LIF, global_lif_id);

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/** Trajectory Trace EEDB entry */

/**
 * \brief
 * Write to IPFIX/PASMAP encapsulation out-LIF table.
 *
 * \par DIRECT INPUT
 *   \param [in] unit - Relevant unit.
 *   \param [in] trajectory_trace - A pointer to the struct that holds information for the IPFIX/PSAMP encapsulation entry,
 *     see \ref bcm_tunnel_initiator_t.
 *   \param [in] local_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_IPFIX_PSAMP
 */
static shr_error_e
dnx_ipfix_out_lif_table_set(
    int unit,
    bcm_instru_trajectory_trace_t * trajectory_trace,
    uint32 local_outlif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t tunnel_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(trajectory_trace, _SHR_E_PARAM, "trajectory_trace");

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_IPFIX_PSAMP, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_IPFIX_PSAMP_ETPS_IPFIX_PSAMP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OBSERVATION_DOMAIN, INST_SINGLE,
                                 trajectory_trace->observation_domain);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TEMPLATE_ID, INST_SINGLE,
                                 trajectory_trace->template_id);

    if (trajectory_trace->egress_tunnel_if != 0)
    {
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(tunnel_id, trajectory_trace->egress_tunnel_if);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     gport_hw_resources.local_out_lif);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, 0);
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * * Write to IPFIX/PSAMP encapsulation out-LIF table.
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *     \param [in] trajectory_trace -
 *     A pointer to the struct that holds information for the trajectory trace entry,
 *     see \ref bcm_instru_trajectory_trace_t.
 * \par DIRECT OUTPUT:
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \ref shr_error_e.
 */
shr_error_e
dnx_instru_tragectory_trace_create(
    int unit,
    bcm_instru_trajectory_trace_t * trajectory_trace)
{
    int global_lif_id;
    lif_mngr_local_outlif_info_t outlif_info;
    uint32 lif_alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    lif_alloc_flags = 0;

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_trajectory_trace_create_verify(unit, trajectory_trace));

    /** WITH_ID flag is used - get global out-lif ID */
    if (_SHR_IS_FLAG_SET(trajectory_trace->flags, BCM_TUNNEL_WITH_ID))
    {
        /** Add alloc_with_id flag */
        global_lif_id = BCM_GPORT_TUNNEL_ID_GET(trajectory_trace->trajectory_trace_id);
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }
     /** Allocate new tunnel out-LIF - REPLACE flag is not set */
    if (!_SHR_IS_FLAG_SET(trajectory_trace->flags, BCM_TUNNEL_REPLACE))
    {
        /** fill the local outlif info. */
        sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
        outlif_info.dbal_table_id = DBAL_TABLE_EEDB_IPFIX_PSAMP;

        /**SHR_IF_ERR_EXIT(dnx_tunnel_initiator_encap_access_to_outlif_phase
                        (unit, tunnel->encap_access, &outlif_info.outlif_phase));*/

        /** Allocate LIF - WITH_ID if the flag was set */
        SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));

        /** Add entry to out-LIF table */
        SHR_IF_ERR_EXIT(dnx_ipfix_out_lif_table_set(unit, trajectory_trace, outlif_info.local_outlif));

        /** Write global to local LIF mapping to GLEM. */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif_id, outlif_info.local_outlif));
    }

    /** update returned Tunnel ID's */
    BCM_GPORT_TUNNEL_ID_SET(trajectory_trace->trajectory_trace_id, global_lif_id);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get IP-Tunnel initiator object pointed by intf->l3a_intf_id or tunnel->tunnel_id.
 *
 * \param [in] unit -
 *     The unit number.
 * \param [in,out] intf -
 *     * intf->l3a_intf_id - As input - global if id of the tunnel to get. As output - global if id of the
 *     returned tunnel.
 * \param [in,out] tunnel -
 *      A pointer to the struct that holds information for the returned IPv4 tunnel initiator entry.
 *      see \ref bcm_tunnel_initiator_t. All fields except for tunnel_id are output only, fields description:
 *      * tunnel->type - Encapsulated tunnel type.
 *      * tunnel->flags -
 *          * BCM_TUNNEL_INIT_IPV4_SET_DF -if the tunnel was set with IPV4_DF flag.
 *          * BCM_TUNNEL_INIT_USE_OUTER_TTL - invalid, insteaded by egress_qos_model.
 *          * BCM_TUNNEL_INIT_GRE_KEY_USE_LB -if the tunnel is GRE8oIPV4 with LB key.
 *      * tunnel->dip - DIP of the tunnel header. (This field will be not used when tunnel type is ERSPAN.)
 *      * tunnel->sip - SIP of the tunnel header. (This field will be not used when tunnel type is ERSPAN.)
 *      * tunnel->dip6 - DIP of the tunnel header, in case the tunnel is IPV6.
 *      * tunnel->sip6 - SIP of the tunnel header, in case the tunnel is IPV6.
 *      * tunnel->egress_qos_model - 
 *          * Qos model - bcmQosEgressModelUniform and bcmQosEgressModelPipeNextNameSpace, instead of dscp_sel
 *          * TTL model -  bcmQosEgressModelUniform and bcmQosEgressModelPipeMyNameSpace, instead of flag BCM_TUNNEL_INIT_USE_OUTER_TTL
 *          * ECN eligible - only support for IPv4/6 vxlan
 *      * tunnel->dscp - dscp value, 0 is case of uniform inheritance mode.
 *      * tunnel->tunnel_id - As output - global GPORT id of the tunnel to get. As input, global GPORT id of the
 *        tunnel to get.
 *      * tunnel->l3_intf_id - Next outlif pointer if id (usually ARP entry).
 *        (It is usually IP entry when tunnel type is ERSPAN.)
 *      * tunnel->aux_data - index_17_19 of ERSPANv2 header.
 *      * tunnel->hw_id - Unique identifier of an ERSPANv3 engine within a system.
 *      * tunnel->switch_id - Identifies a source switch in ERSPANv3 header.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 */
int
bcm_dnx_tunnel_initiator_get(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    int is_tunneled;
    bcm_gport_t tunnel_gport;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    is_tunneled = 0;
    tunnel_gport = 0;

    /** Verification */
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_initiator_get_verify(unit, intf, tunnel));

    /** get tunnel gport */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_gport_get(unit, intf, tunnel, &tunnel_gport, &is_tunneled));
    if (is_tunneled == 0)
    {
        SHR_ERR_EXIT(BCM_E_NOT_FOUND, "Tunnel is not found on this interface.\n");
    }

    /** get GPort hw resources (global and local tunnel outlif)*/
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, tunnel_gport, lif_flags, &gport_hw_resources));

    if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_ERSPAN)
    {
        /** Read ERSPAN data from EEDB outlif table */
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_erspan_table_get
                        (unit, gport_hw_resources.local_out_lif, gport_hw_resources.outlif_dbal_result_type, NULL,
                         tunnel));
    }
    else if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_RSPAN)
    {
        /** Read RSPAN Advanced data from EEDB outlif table */
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_rspan_advanced_table_get(unit, gport_hw_resources.local_out_lif, tunnel));
    }
    else
    {
        /** Read tunnel data from EEDB outlif table */
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_out_lif_entry_get
                        (unit, gport_hw_resources.local_out_lif, gport_hw_resources.outlif_dbal_table_id,
                         gport_hw_resources.outlif_dbal_result_type, tunnel));
    }

    /** Set tunnels ID's in API params */
    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id, gport_hw_resources.global_out_lif);
    BCM_L3_ITF_SET(intf->l3a_intf_id, BCM_L3_ITF_TYPE_LIF, gport_hw_resources.global_out_lif);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete IP-Tunnel initiator object pointed by intf->l3a_intf_id.
 *
 * \param [in] unit - The unit number.
 * \param [in] intf -
 *     * intf->l3a_intf_id - global if id of the tunnel to get.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 */
int
bcm_dnx_tunnel_initiator_clear(
    int unit,
    bcm_l3_intf_t * intf)
{
    bcm_gport_t tunnel_gport;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 sip_profile;
    uint32 ttl_pipe_profile;
    uint32 esem_cmd_profile;
    uint32 is_ttl_pipe_mode;
    uint8 remove_sip_profile;
    uint8 remove_ttl_pipe_profile;
    uint8 remove_qos_pipe_profile;
    uint8 remove_esem_cmd_profile;
    uint32 qos_model_idx;
    uint32 is_qos_model_pipe_mode;
    uint32 next_local_outlif = 0;
    uint32 tunnel_is_ipv6 = 0;
    uint32 out_lif_llvp_profile;
    uint32 lif_additional_header_profile;
    uint32 remove_old_udp_ports_profile = FALSE;
    uint32 old_udp_ports_profile = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sip_profile = 0;
    remove_sip_profile = 0;
    remove_ttl_pipe_profile = 0;
    remove_qos_pipe_profile = 0;
    remove_esem_cmd_profile = 0;

    /** Verification */
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_initiator_clear_verify(unit, intf));

    /** Get GPort */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(tunnel_gport, intf->l3a_intf_id);
    if (!BCM_GPORT_IS_TUNNEL(tunnel_gport))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Tunnel is not found on provided interface");
    }

    /** get GPort hw resources (global and local tunnel outlif) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, tunnel_gport, lif_flags, &gport_hw_resources));

    if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_ERSPAN)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_erspan_clear(unit, &gport_hw_resources));
        SHR_EXIT();
    }

    if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_RSPAN)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_rspan_advanced_clear(unit, &gport_hw_resources));
        SHR_EXIT();
    }

    /** Validate tunnel type and get template managed profiles */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_eedb_entry_get
                    (unit, gport_hw_resources.outlif_dbal_table_id, gport_hw_resources.local_out_lif,
                     gport_hw_resources.outlif_dbal_result_type, &next_local_outlif, &sip_profile,
                     &ttl_pipe_profile, &is_ttl_pipe_mode, &qos_model_idx, &is_qos_model_pipe_mode,
                     &out_lif_llvp_profile, &esem_cmd_profile, &lif_additional_header_profile));

    /** Delete global to local mapping from GLEM */
    
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, gport_hw_resources.global_out_lif));

    /** Remove SIP and TTL values from HW */
    if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_IPV6_TUNNEL)
    {
        tunnel_is_ipv6 = TRUE;
    }

    /** Remove allocated SIP, TTL amd QOS templates from template manager */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_template_entries_free
                    (unit, gport_hw_resources.local_out_lif, sip_profile, ttl_pipe_profile, is_ttl_pipe_mode,
                     &remove_sip_profile, &remove_ttl_pipe_profile, qos_model_idx, &remove_qos_pipe_profile,
                     is_qos_model_pipe_mode));

    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_sip_ttl_qos_table_delete
                    (unit, gport_hw_resources.local_out_lif,
                     sip_profile, ttl_pipe_profile, qos_model_idx, remove_sip_profile, remove_ttl_pipe_profile,
                     remove_qos_pipe_profile, tunnel_is_ipv6));

    /*
     * Remove allocated UDP ports profile, if valid.
     */
    if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL &&
        gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP
        && lif_additional_header_profile != 0)
    {
        old_udp_ports_profile = lif_additional_header_profile;
        SHR_IF_ERR_EXIT(dnx_tunnel_udp_ports_profile_allocator(unit, NULL, old_udp_ports_profile,
                                                               NULL, NULL, &remove_old_udp_ports_profile));
    }

    /** Remove allocated esem command, if valid. */
    if ((gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE) ||
        (gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN))
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_free(unit, esem_cmd_profile, &remove_esem_cmd_profile));
    }

    if (remove_esem_cmd_profile == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, esem_cmd_profile));
    }

    if (tunnel_is_ipv6)
    {
        /** Remove tunnel data from EEDB outlif data entry table */
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_dbal_entry_clear
                        (unit, DBAL_TABLE_EEDB_DATA_ENTRY, DBAL_FIELD_OUT_LIF, next_local_outlif,
                         DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6));
    }

    if (remove_old_udp_ports_profile)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_udp_ports_profile_hw_set(unit, NULL, old_udp_ports_profile, 1));
    }

    /** Remove tunnel data from EEDB outlif table */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_dbal_entry_clear
                    (unit, gport_hw_resources.outlif_dbal_table_id, DBAL_FIELD_OUT_LIF,
                     gport_hw_resources.local_out_lif, gport_hw_resources.outlif_dbal_result_type));

    /** Delete global and local out lifs allocations */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, gport_hw_resources.global_out_lif, NULL, gport_hw_resources.local_out_lif));

    /** Release out lif llvp profile*/
    if (dnx_tunnel_utility_llvp_profile_exist(unit, gport_hw_resources.outlif_dbal_result_type, tunnel_is_ipv6) == TRUE)
    {
        /** if it is not default native ac llvp profile, need to "free" it at the template manager */
        if (((int) out_lif_llvp_profile) != dnx_data_port_pp.general.default_egress_native_ac_llvp_profile_id_get(unit))
        {
            uint8 is_last;

            SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_egress.free_single
                            (unit, _SHR_CORE_ALL, (int) out_lif_llvp_profile, &is_last));

            if (is_last == TRUE)
            {
                /*
                 * Clear the HW:
                 */
                SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_block_clear(unit, (int) out_lif_llvp_profile));
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all IP-Tunnel initiator objects.
 * \param [in] unit - the unit number.
 * \param [in] cb - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry.
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  *
 */
shr_error_e
bcm_dnx_tunnel_initiator_traverse(
    int unit,
    bcm_tunnel_initiator_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_tables[] = { DBAL_TABLE_EEDB_IPV4_TUNNEL, DBAL_TABLE_EEDB_IPV6_TUNNEL, DBAL_TABLE_EEDB_ERSPAN };
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table;
    uint32 result_field;
    uint32 key_field_value[1];
    int is_end;
    bcm_tunnel_initiator_t tunnel;
    bcm_l3_intf_t intf;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_initiator_traverse_verify(unit, cb, user_data));

    /*
     * Iterate over all tables and all their entries
     */
    for (table = 0; table < nof_tables; table++)
    {
        /*
         * Allocate handle to the table of the iteration and initialise an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, key_field_value));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_field));
            bcm_tunnel_initiator_t_init(&tunnel);
            bcm_l3_intf_t_init(&intf);
            if (dbal_tables[table] == DBAL_TABLE_EEDB_ERSPAN)
            {
                /** Read ERSPAN data from EEDB outlif table */
                SHR_IF_ERR_EXIT(dnx_tunnel_initiator_erspan_table_get
                                (unit, key_field_value[0], result_field, NULL, &tunnel));
            }
            else
            {
                /** Read tunnel data from EEDB outlif table */
                SHR_IF_ERR_EXIT(dnx_tunnel_initiator_out_lif_entry_get
                                (unit, key_field_value[0], dbal_tables[table], result_field, &tunnel));
            }
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL,
                             key_field_value[0], &tunnel.tunnel_id));
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf.l3a_intf_id, tunnel.tunnel_id);
            if (cb != NULL)
            {
                /*
                 * Invoke callback function
                 */
                SHR_IF_ERR_EXIT((*cb) (unit, &tunnel, user_data));
            }

            /** Receive next entry in table.*/
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * End of APIs
 * }
 */
