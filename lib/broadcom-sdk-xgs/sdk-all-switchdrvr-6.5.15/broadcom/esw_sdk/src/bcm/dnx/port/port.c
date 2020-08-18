/*
 ** \file port.c $Id$ PORT procedures for DNX.
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
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/legacy/mbcm.h>
#include <soc/dnx/stat/jer2_stat.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/field/field_port.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/link/link.h>
#include <src/bcm/dnx/cosq/flow_control_imp.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/port/port.h>
#include "port_utils.h"
#include <bcm_int/dnx/ptp/ptp.h>
#include <bcm_int/common/link.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/switch/switch.h>

/*
 * }
 */

/*
 * Macros.
 * {
 */

/*
 * }
 */

/**
 * \brief - Verify Port, pclass and class_id parameters for BCM-API: bcm_dnx_port_class_get
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Physical port.
 * \param [in] pclass - Typedef enum value for specific port configurations. See .h file.
 * \param [out] class_id - Class id.
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
dnx_port_class_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_class_t pclass,
    uint32 *class_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(class_id, _SHR_E_PARAM, "class_id");

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 *  Set ingress Lif orientation value
 */
shr_error_e static
dnx_port_class_ingress_orientation_set(
    int unit,
    bcm_gport_t port,
    int orientation)
{
    int old_in_lif_profile = 0, new_in_lif_profile = 0;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int in_lif_profile_orientation_number_of_values;
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    in_lif_profile_orientation_number_of_values = dnx_data_lif.in_lif.in_lif_profile_allocate_orientaion_get(unit);
    if (utilex_log2_round_up(in_lif_profile_orientation_number_of_values) < utilex_log2_round_up(orientation))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Not enough bits for orientation were allocated!\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
    /** Allocate handle to the inlif table to get the in_lif_profile value*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &old_in_lif_profile));

    /** Get the data stored for this in_lif_profile*/
    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));
    in_lif_profile_info.egress_fields.in_lif_orientation = orientation;

    /** Exchange for new in_lif_profile, based on the new orientation*/
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     gport_hw_resources.inlif_dbal_table_id));

    /** Update inLIF table with the new in_lif_profile */
    if (old_in_lif_profile != new_in_lif_profile)
    {
        /** Update the LIF table with the new in_lif_profile*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, new_in_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Get ingress Lif orientation value
 */
shr_error_e static
dnx_port_class_ingress_orientation_get(
    int unit,
    bcm_gport_t port,
    uint32 *orientation)
{
    int in_lif_profile = 0;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &in_lif_profile));

    /** Based on the inlif profile - get the orientation*/
    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
    *orientation = in_lif_profile_info.egress_fields.in_lif_orientation;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Get egress Lif orientation value
 */
shr_error_e static
dnx_port_class_egress_orientation_get(
    int unit,
    bcm_gport_t port,
    uint32 *orientation)
{
    uint32 entry_handle_id, local_out_lif, dbal_table_id, result_type, out_lif_profile_get;
    out_lif_profile_info_t out_lif_profile_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int out_lif_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));
    local_out_lif = gport_hw_resources.local_out_lif;
    dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
    result_type = gport_hw_resources.outlif_dbal_result_type;

    /** Allocate handle to the outlif table and get the outlif profile*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile_get));
    out_lif_profile = (int) out_lif_profile_get;

    /** Get the allocated outlif profile info based on the outlif profile*/
    out_lif_profile_info_t_init(unit, &out_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info, dbal_table_id));
    *orientation = out_lif_profile_info.out_lif_orientation;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Set egress Lif orientation value
 */
shr_error_e static
dnx_port_class_egress_orientation_set(
    int unit,
    bcm_gport_t port,
    int orientation)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    dbal_tables_e dbal_table_id;
    uint32 local_out_lif;
    uint32 entry_handle_id;
    uint32 res_type;
    out_lif_profile_info_t out_lif_profile_info;
    int old_out_lif_profile, new_out_lif_profile;
    uint32 old_out_lif_profile_get;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));

    local_out_lif = gport_hw_resources.local_out_lif;
    dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
    res_type = gport_hw_resources.outlif_dbal_result_type;

    /** Allocate handle to the oulif table and get the outlif profile data*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /** write map value  */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /** read outlif profile for the given LIF */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &old_out_lif_profile_get));
    old_out_lif_profile = (int) old_out_lif_profile_get;

    /** Get the allocated outlif profile info based on the outlif profile*/
    out_lif_profile_info_t_init(unit, &out_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_out_lif_profile_get_data(unit, old_out_lif_profile, &out_lif_profile_info, dbal_table_id));
    out_lif_profile_info.out_lif_orientation = orientation;
    /** Exchange the outlif profile info according to the input parameters*/
    SHR_IF_ERR_EXIT(dnx_out_lif_profile_exchange
                    (unit, dbal_table_id, 0, &out_lif_profile_info, old_out_lif_profile, &new_out_lif_profile));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, new_out_lif_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get port classification ID to aggregate a group of ports for further processing such as Vlan
 *  translation and field processing.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Physical port
 * \param [in] pclass - Typedef enum value for specific port configurations. See .h file.
 * \param [out] class_id - Class id.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_class_get(
    int unit,
    bcm_port_t port,
    bcm_port_class_t pclass,
    uint32 *class_id)
{
    uint8 is_phy_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_class_get_verify(unit, port, pclass, class_id));

    /** Check if the port is physical */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    /*
     * If the port is physical then get pp port vlan domain
     * If the port is not physical then get the lif vlan domain
     */
    if (is_phy_port != FALSE)
    {
        /** Get Port + Core */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    }
    /*
     * Check pclass, for each class call appropriate function
     */
    /*
     * Note: Do not add code inside the case!!! Only through separate function!
     */
    switch (pclass)
    {
        case bcmPortClassId:
        {
            if (is_phy_port != FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "GPORT 0x%x is not physical, and thus cannot be input.\r\n", port);
            }
            break;
        }
        case bcmPortClassIngress:
        {
            if (BCM_GPORT_IS_TUNNEL(port) || BCM_GPORT_IS_MPLS_PORT(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_ingress_vlan_domain_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "port 0x%x is not tunnel/mpls GPORT, and thus cannot be input.\r\n", port);
            }
            break;
        }
        case bcmPortClassEgress:
        {
            if (BCM_GPORT_IS_TUNNEL(port) || BCM_GPORT_IS_MPLS_PORT(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_egress_vlan_domain_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "port 0x%x is not tunnel/mpls GPORT, and thus cannot be input.\r\n", port);
            }
            break;
        }
        case bcmPortClassVlanMember:
        {
            if (is_phy_port != FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_membership_if_get
                                (unit, gport_info.internal_port_pp_info.core_id[0],
                                 gport_info.internal_port_pp_info.pp_port[0], class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "GPORT 0x%x is not physical, and thus cannot be input.\r\n", port);
            }
            break;
        }
        case bcmPortClassFieldIngressVport:
        case bcmPortClassFieldEgressVport:
        case bcmPortClassFieldIngressPMF1PacketProcessingPort:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh:
        case bcmPortClassFieldIngressPMF3PacketProcessingPort:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh:
        case bcmPortClassFieldEgressPacketProcessingPort:
        case bcmPortClassFieldEgressPacketProcessingPortCs:
        case bcmPortClassFieldExternalPacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF1TrafficManagementPort:
        case bcmPortClassFieldIngressPMF1TrafficManagementPortCs:
        case bcmPortClassFieldIngressPMF3TrafficManagementPort:
        case bcmPortClassFieldIngressPMF3TrafficManagementPortCs:
        case bcmPortClassFieldEgressTrafficManagementPort:
        case bcmPortClassFieldExternalTrafficManagementPortCs:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_gport_get(unit, pclass, port, class_id));
            break;
        }
        case bcmPortClassForwardIngress:
        {
            if (BCM_GPORT_IS_TUNNEL(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_class_ingress_orientation_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_get for port %d is not supported\n", port);
            }
            break;
        }
        case bcmPortClassForwardEgress:
        {
            if (BCM_GPORT_IS_TUNNEL(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_class_egress_orientation_get(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_get for port %d is not supported\n", port);
            }
            break;
        }
        default:
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify Port, pclass and class_id parameters for
 * BCM-API: bcm_dnx_port_class_set()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *  Relevant unit.
 *   \param [in] port -
 *  Port - physical port
 *   \param [in] pclass -
 *  Typedef enum value for specific port configurations. See .h
 *   \param [in] class_id -
 *   Class id.
 * \par INDIRECT INPUT:
     None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
     None
 * \remark
     None
 * \see
     None
 */
static shr_error_e
dnx_port_class_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_class_t pclass,
    uint32 class_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify port, type and for
 * BCM-API: bcm_port_control_set()/ bcm_port_control_get()
 *
 * \param [in] unit - unit id
 * \param [in] port - physical port for updating
 * \param [in] is_phy_port - If the port is a physical port
 * \param [in] type - bcm_port_control_t
 * \param [in] is_set - Boolean. is set operation
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
dnx_port_control_port_and_type_verify(
    int unit,
    bcm_port_t port,
    uint8 is_phy_port,
    bcm_port_control_t type,
    uint32 is_set)
{
    int system_headers_mode;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t local_port;
    dnx_algo_port_type_e port_type;
    int prd_enable_mode;
    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    switch (type)
    {
        case bcmPortControlFloodUnknownUcastGroup:
        case bcmPortControlFloodUnknownMcastGroup:
        case bcmPortControlFloodBroadcastGroup:
        {
            SHR_ERR_EXIT(BCM_E_UNAVAIL,
                         "Port control (%d) is not supported. Call API bcm_port_flood_group_get instead!\n", type);
            break;
        }
        case bcmPortControlEgressFilterDisable:
        {
            if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
            {
                SHR_ERR_EXIT(BCM_E_UNAVAIL,
                             "Port control (%d) is not supported when system_headers_mode = %d. \n", type,
                             system_headers_mode);
            }
            else if (is_phy_port == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "port: 0x%X, physical port is required!\n", port);
            }
            break;
        }
        case bcmPortControlDiscardMacSaAction:
        case bcmPortControl1588P2PDelay:
        {
            if (is_phy_port == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "port: 0x%X, physical port is required!\n", port);
            }
            break;
        }
        case bcmPortControlExtenderType:
            /** Verify PRD is disabled for NIF ports */
            
            if ((!SOC_IS_J2C(unit)) && (is_set == TRUE))
            {
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &gport_info));

                BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, local_port, &port_type));
                    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, FALSE, FALSE))
                    {
                        SHR_IF_ERR_EXIT(imb_prd_enable_get(unit, local_port, &prd_enable_mode));
                        if (prd_enable_mode != bcmCosqIngressPortDropDisable)
                        {
                            SHR_ERR_EXIT(_SHR_E_CONFIG,
                                         "Extender type cannot be configured if PRD is enabled on local port %d.\nPlease disable PRD.\n",
                                         local_port);
                        }
                    }
                }
            }
            break;

        default:
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify port, type and value for
 * BCM-API: bcm_port_control_set()
 *
 * \param [in] unit - unit id
 * \param [in] port - physical port for updating
 * \param [in] is_phy_port - If the port is a physical port
 * \param [in] type - bcm_port_control_t
 * \param [in] value - changes according to the type
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
dnx_port_control_set_verify(
    int unit,
    bcm_port_t port,
    uint8 is_phy_port,
    bcm_port_control_t type,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_control_port_and_type_verify(unit, port, is_phy_port, type, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify port, type and value for
 * BCM-API: bcm_port_control_get()
 *
 * \param [in] unit - unit id
 * \param [in] port - physical port for updating
 * \param [in] is_phy_port - If the port is a physical port
 * \param [in] type - bcm_port_control_t
 * \param [in] *value - changes according to the type
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
dnx_port_control_get_verify(
    int unit,
    bcm_port_t port,
    uint8 is_phy_port,
    bcm_port_control_t type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "A valid pointer is needed for values!");

    SHR_IF_ERR_EXIT(dnx_port_control_port_and_type_verify(unit, port, is_phy_port, type, FALSE));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set port classification ID to aggregate a
 *      group of ports for further processing such as Vlan
 *      translation and field processing.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Physical port.
 * \param [in] pclass - Typedef enum value for specific port configurations. See .h file.
 * \param [in] class_id - Class id.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_class_set(
    int unit,
    bcm_port_t port,
    bcm_port_class_t pclass,
    uint32 class_id)
{
    uint8 is_phy_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_class_set_verify(unit, port, pclass, class_id));

    /*
     * Check pclass, for each class call appropriate function
     */
    /*
     * Note: Do not add code inside the case!!! Only through separate function!
     */
    switch (pclass)
    {
        case bcmPortClassId:
        {

            /** Check if it is a physical port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
            /*
             * If the port is physical then configure pp port vlan domain
             */
            if (is_phy_port != FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The provided port is not physical and cannot be input to the function! Use bcmPortClassIngress or bcmPortClassEgress instead.\n");
            }
            break;
        }
        case bcmPortClassIngress:
        {
            /** Check if it is a physical port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
            if (is_phy_port == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_ingress_vlan_domain_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The provided port is physical and cannot be input to the function! Use bcmPortClassId instead. \n");
            }
            break;
        }
        case bcmPortClassEgress:
        {
            /** Check if it is a physical port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
            if (is_phy_port == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_lif_egress_vlan_domain_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The provided port is physical and cannot be input to the function! Use bcmPortClassId instead.\n");
            }
            break;
        }
        case bcmPortClassVlanMember:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_membership_if_set(unit, port, class_id));
            break;
        }
        case bcmPortClassFieldIngressVport:
        case bcmPortClassFieldEgressVport:
        case bcmPortClassFieldIngressPMF1PacketProcessingPort:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData:
        case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh:
        case bcmPortClassFieldIngressPMF3PacketProcessingPort:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData:
        case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh:
        case bcmPortClassFieldEgressPacketProcessingPort:
        case bcmPortClassFieldEgressPacketProcessingPortCs:
        case bcmPortClassFieldExternalPacketProcessingPortCs:
        case bcmPortClassFieldIngressPMF1TrafficManagementPort:
        case bcmPortClassFieldIngressPMF1TrafficManagementPortCs:
        case bcmPortClassFieldIngressPMF3TrafficManagementPort:
        case bcmPortClassFieldIngressPMF3TrafficManagementPortCs:
        case bcmPortClassFieldEgressTrafficManagementPort:
        case bcmPortClassFieldExternalTrafficManagementPortCs:
        {
            SHR_IF_ERR_EXIT(dnx_field_port_profile_gport_set(unit, pclass, port, class_id));
            break;
        }
        case bcmPortClassForwardIngress:
        {
            if (BCM_GPORT_IS_TUNNEL(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_class_ingress_orientation_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_set for port %d is not supported\n", port);
            }
            break;
        }
        case bcmPortClassForwardEgress:
        {
            if (BCM_GPORT_IS_TUNNEL(port))
            {
                SHR_IF_ERR_EXIT(dnx_port_class_egress_orientation_set(unit, port, class_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_port_class_set for port %d is not supported\n", port);
            }
            break;
        }
        default:
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            SHR_EXIT();
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  Enable Same interface Filter per port
 */
shr_error_e static
dnx_port_same_interface_filter_set(
    int unit,
    bcm_port_t port,
    int filter_flags)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_pbmp_t pbmp_tm_ports;
    uint32 port_tm, pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    BCM_PBMP_CLEAR(pbmp_tm_ports);

    /*
     * Analyze the filter flags
     */
    if ((filter_flags > 1) || (filter_flags < 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "value: 0x%X, Invalid filter flags!\n", filter_flags);
    }
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /** Set filter enablers to HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_tm_pbmp_get(unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                        gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                        &pbmp_tm_ports));
        BCM_PBMP_ITER(pbmp_tm_ports, port_tm)
        {
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, port_tm);
            /** Value fields */
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_ENABLE, INST_SINGLE,
                                        filter_flags);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Get the value of Same Interface Filter Enable/Disable per port
 */
static shr_error_e
dnx_port_same_interface_filter_get(
    int unit,
    bcm_port_t port,
    int *filter_flags)
{
    uint32 entry_handle_id;
    uint8 filter_flag;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tm_core_id, port_tm;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, (bcm_core_t *) & tm_core_id, &port_tm));
    /** Get the filter enablers from HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, port_tm);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE_ENABLE, INST_SINGLE, &filter_flag);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *filter_flags = filter_flag;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Enable/ Disable Split Horizon Filter per port
 */
shr_error_e static
dnx_port_split_horizon_set(
    int unit,
    bcm_port_t port,
    int filter_flags)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 set_val;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    set_val = (filter_flags == FALSE) ? 0 : 1;

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Set Split Horizon Enable/Disable for the PP port
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, set_val);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  Get Split Horizon Filter per port value
 */
shr_error_e static
dnx_port_split_horizon_get(
    int unit,
    bcm_port_t port,
    int *filter_flags)
{
    uint32 entry_handle_id, filter_flag;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Get Split Horizon Enable/Disable for the PP port
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, &filter_flag);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    *filter_flags = filter_flag;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get the GPORT ID for the specified physical port
 *
 * \param [in] unit - unit ID
 * \param [in] port - logical port or gport 'local port'
 * \param [out] gport - MODPORT of the given gport
 * \return
 *     shr_err_e
 *
 * \remark
 *    * None
 * \see
 *    * None
 */
int
bcm_dnx_port_gport_get(
    int unit,
    bcm_port_t port,
    bcm_gport_t * gport)
{
    algo_gpm_gport_verify_type_e allowed_types[] =
        { ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_PORT };
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_core_t core_id;
    uint32 tm_port, modid;
    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, port, COUNTOF(allowed_types), allowed_types));

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /** get modid and tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core_id, &tm_port));
    SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get(unit, core_id, &modid));

    /** create the required gport*/
    BCM_GPORT_MODPORT_SET(*gport, modid, tm_port);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set various features at the port level.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] value - Value of the bit field in port table
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_control_set
 */
bcm_error_t
bcm_dnx_port_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value)
{
    uint8 is_phy_port;
    bcm_port_t port_local;
    SHR_FUNC_INIT_VARS(unit);

    /** Check if it is a physical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));

    /** Verify the inputs*/
    SHR_INVOKE_VERIFY_DNX(dnx_port_control_set_verify(unit, port, is_phy_port, type, value));

    switch (type)
    {
        case bcmPortControlMplsFRREnable:
        {
            dbal_enum_value_field_ingress_port_termination_profile_e llr_profile;
            dbal_enum_value_field_ingress_port_termination_profile_e vt_profile;
            /*
             * Get two parameters: llr_profile, vt_profile. The second must be written back as is.
             */
            SHR_IF_ERR_EXIT(dnx_port_pp_llr_vtt_profile_get(unit, port, &llr_profile, &vt_profile));
            /*
             * We expect llr_profile to be one of the four options below. Any other value
             * is considered illegal.
             */
            if ((llr_profile != DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_DEFAULT) &&
                (llr_profile != DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR) &&
                (llr_profile != DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_SUPPORT_FRR_OPTIMIZATION) &&
                (llr_profile != DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Procedure '%s' was called with 'bcmPortControlMplsFRREnable'\r\n"
                             "==> but llr_profile is none of DEFAULT,802_1_BR,SUPPORT_FRR_OPTIMIZATION,802_1_BR_WITH_FRR_OPTIMIZATION.\r\n"
                             "==> It is %d. This is illegal. Quit.", __FUNCTION__, llr_profile);
            }
            /*
             * If 'value' is non-zero then enable FRR for MPLS.
             *   If FRR is already enabled, inject error.
             * If 'value' is zero then disable FRR for MPLS.
             *   If FRR is already disabled, inject error.
             */
            if (value != 0)
            {
                if (llr_profile == DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_DEFAULT)
                {
                    llr_profile = DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_SUPPORT_FRR_OPTIMIZATION;
                }
                else if (llr_profile == DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR)
                {
                    llr_profile = DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Requesting to enable FRR when it is already enabled (llr_profile = %d). Reject request with error\r\n",
                                 llr_profile);
                }
            }
            else
            {
                if (llr_profile == DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_SUPPORT_FRR_OPTIMIZATION)
                {
                    llr_profile = DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_DEFAULT;
                }
                else if (llr_profile == DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION)
                {
                    llr_profile = DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Requesting to disable FRR when it is already disabled (llr_profile = %d). Reject request with error\r\n",
                                 llr_profile);
                }
            }
            /*
             * Set the two parameters: llr_profile, vt_profile. The second is written back as is.
             */
            SHR_IF_ERR_EXIT(dnx_port_pp_llr_vtt_profile_set(unit, port, llr_profile, vt_profile));
            break;
        }
        case bcmPortControlDiscardMacSaAction:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_l2_sa_drop_action_profile_set(unit, port, value));
            break;
        }
        case bcmPortControlEgressFilterDisable:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_l2_unknown_filter_set(unit, port, value));
            break;
        }
        case bcmPortControlOuterPolicerRemark:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ive_dp_profile_set(unit, port, QOS_DP_PROFILE_OUTER, value));
            break;
        }

        case bcmPortControlInnerPolicerRemark:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ive_dp_profile_set(unit, port, QOS_DP_PROFILE_INNER, value));
            break;
        }
        case bcmPortControlExtenderType:
        {
            dbal_enum_value_field_ingress_port_termination_profile_e llr_profile;
            int header_type;

            /** add restriction only in header type is ETH */
            SHR_IF_ERR_EXIT(dnx_switch_header_type_get(unit, port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN, &header_type));
            if (header_type != BCM_SWITCH_PORT_HEADER_TYPE_ETH) 
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Port %d is not supported due to port header type is not ETH\n", port);
            }

            llr_profile =
                (value ==
                 BCM_PORT_EXTENDER_TYPE_SWITCH) ? DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR :
                DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_DEFAULT;
            SHR_IF_ERR_EXIT(dnx_port_ptc_profile_set
                            (unit, port, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET, llr_profile));
            break;
        }
        case bcmPortControlBridge:
        {
            if (is_phy_port == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_port_same_interface_filter_set(unit, port, value));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_lif_same_interface_disable_set(unit, port, value));
            }
            break;
        }
        case bcmPortControlLogicalInterfaceSameFilter:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_in_lif_same_interface_set(unit, port, value));
            break;
        }
        case bcmPortControlForwardNetworkGroup:
        {
            if (is_phy_port == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_port_split_horizon_set(unit, port, value));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_port_control_set port %d is not supported\n", port);
            }
            break;
        }
        /** NIF handler */
        case bcmPortControlLinkFaultLocal:
        case bcmPortControlLinkFaultRemote:
        case bcmPortControlLinkFaultRemoteEnable:
        case bcmPortControlLinkFaultLocalEnable:
        case bcmPortControlPadToSize:
        case bcmPortControlPCS:
        case bcmPortControlLinkDownPowerOn:
        case bcmPortControlPrbsMode:
        case bcmPortControlPrbsPolynomial:
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
        case bcmPortControlLowLatencyLLFCEnable:
        case bcmPortControlFecErrorDetectEnable:
        case bcmPortControlLlfcCellsCongestionIndEnable:
        case bcmPortControlLLFCAfterFecEnable:
        case bcmPortControlControlCellsFecBypassEnable:
        case bcmPortControlStatOversize:
        case bcmPortControlRxEnable:
        case bcmPortControlTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_if_control_set(unit, port, type, value));
            break;
        }
        case bcmPortControlPFCRefreshTime:
        {
            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(dnx_fc_pfc_refresh_set(unit, port_local, value));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlPFCTransmit:
        case bcmPortControlLLFCTransmit:
        {
            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                if (type == bcmPortControlPFCTransmit)
                {
                    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set
                                    (unit, port_local, bcmCosqFlowControlGeneration, FALSE, value));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set
                                    (unit, port_local, bcmCosqFlowControlGeneration, value, FALSE));
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlPFCReceive:
        case bcmPortControlLLFCReceive:
        {
            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                if (type == bcmPortControlPFCReceive)
                {
                    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set
                                    (unit, port_local, bcmCosqFlowControlReception, FALSE, value));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set
                                    (unit, port_local, bcmCosqFlowControlReception, value, FALSE));
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControl1588P2PDelay:
        {
            SHR_IF_ERR_EXIT(dnx_ptp_port_delay_set(unit, port, value));
            break;
        }
        case bcmPortControlIngressQosModelRemark:
        case bcmPortControlIngressQosModelPhb:
        case bcmPortControlIngressQosModelTtl:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_port_model_set(unit, port, type, value));
            break;
        }
        case bcmPortControlOverlayRecycle:
        {
            if (value == 1)
            {
                /*
                 * update llr, vt1 and vt1 context selection profile for recycle port to: 2nd pass drop and continue. Used
                 * by VTT1 context selection 
                 */
                SHR_IF_ERR_EXIT(dnx_port_pp_llr_vtt_profile_set
                                (unit, port,
                                 DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_2ND_PASS_DROP_AND_CONTINUE,
                                 DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_2ND_PASS_DROP_AND_CONTINUE));
                /*
                 * update parsing context to RCH_A1
                 */
                SHR_IF_ERR_EXIT(dnx_port_pp_parsing_context_set
                                (unit, port, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_RCH_A1));
            }
            else if (value == 0)
            {
                /*
                 * update llr, vt1 and vt1 context selection profile for recycle port to: default. Used
                 * to reset profiles for recycle port
                 */
                SHR_IF_ERR_EXIT(dnx_port_pp_llr_vtt_profile_set
                                (unit, port,
                                 DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_DEFAULT,
                                 DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_DEFAULT));
            }

            break;
        }
        case bcmPortControlExtenderEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_coe_enable_set(unit, port, value));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_control_set type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get various features at the port level.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] *value - Value of the bit field in port table
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_control_set
 */
bcm_error_t
bcm_dnx_port_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value)
{
    uint8 is_phy_port;
    uint32 llfc_enabled;
    uint32 pfc_enabled;
    bcm_port_t port_local;
    SHR_FUNC_INIT_VARS(unit);

    /** Check if it is a physical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));

    /** Verify the inputs*/
    SHR_INVOKE_VERIFY_DNX(dnx_port_control_get_verify(unit, port, is_phy_port, type, value));

    switch (type)
    {
        case bcmPortControlDiscardMacSaAction:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_l2_sa_drop_action_profile_get(unit, port, value));
            break;
        }
        case bcmPortControlEgressFilterDisable:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_l2_unknown_filter_get(unit, port, value));
            break;
        }
        case bcmPortControlOuterPolicerRemark:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ive_dp_profile_get(unit, port, QOS_DP_PROFILE_OUTER, value));
            break;
        }
        case bcmPortControlInnerPolicerRemark:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_ive_dp_profile_get(unit, port, QOS_DP_PROFILE_INNER, value));
            break;
        }
        case bcmPortControlBridge:
        {
            if (is_phy_port != FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_port_same_interface_filter_get(unit, port, value));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_lif_same_interface_disable_get(unit, port, value));
            }
            break;
        }
        case bcmPortControlLogicalInterfaceSameFilter:
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_in_lif_same_interface_get(unit, port, value));
            break;
        }
        case bcmPortControlForwardNetworkGroup:
        {
            if (is_phy_port == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_port_split_horizon_get(unit, port, value));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_port_control_seg port %d is not supported\n", port);
            }
            break;
        }
        case bcmPortControlExtenderType:
        {
            dbal_enum_value_field_port_termination_ptc_profile_e prt_profile = 0;
            dbal_enum_value_field_ingress_port_termination_profile_e llr_profile = 0;

            SHR_IF_ERR_EXIT(dnx_port_ptc_profile_get(unit, port, &prt_profile, &llr_profile));
            *value = ((llr_profile == DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR) 
                || (llr_profile == DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION)) ?
                BCM_PORT_EXTENDER_TYPE_SWITCH : BCM_PORT_EXTENDER_TYPE_NONE;
            break;
        }

        /** NIF handler */
        case bcmPortControlLinkFaultLocal:
        case bcmPortControlLinkFaultRemote:
        case bcmPortControlLinkFaultRemoteEnable:
        case bcmPortControlLinkFaultLocalEnable:
        case bcmPortControlPadToSize:
        case bcmPortControlPCS:
        case bcmPortControlLinkDownPowerOn:
        case bcmPortControlPrbsMode:
        case bcmPortControlPrbsPolynomial:
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
        case bcmPortControlPrbsRxStatus:
        case bcmPortControlLowLatencyLLFCEnable:
        case bcmPortControlFecErrorDetectEnable:
        case bcmPortControlLlfcCellsCongestionIndEnable:
        case bcmPortControlLLFCAfterFecEnable:
        case bcmPortControlControlCellsFecBypassEnable:
        case bcmPortControlStatOversize:
        case bcmPortControlRxEnable:
        case bcmPortControlTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_if_control_get(unit, port, type, value));
            break;
        }
        case bcmPortControlPFCRefreshTime:
        {
            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(dnx_fc_pfc_refresh_get(unit, port_local, value));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlPFCTransmit:
        case bcmPortControlLLFCTransmit:
        {
            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                                (unit, port_local, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
                if (type == bcmPortControlPFCTransmit)
                {
                    *value = pfc_enabled;
                }
                else
                {
                    *value = llfc_enabled;
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControlPFCReceive:
        case bcmPortControlLLFCReceive:
        {
            if (BCM_GPORT_IS_LOCAL(port) || ((port >= 0) && (port < SOC_MAX_NUM_PORTS)))
            {
                port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;

                SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                                (unit, port_local, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
                if (type == bcmPortControlPFCReceive)
                {
                    *value = pfc_enabled;
                }
                else
                {
                    *value = llfc_enabled;
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }
            break;
        }
        case bcmPortControl1588P2PDelay:
        {
            SHR_IF_ERR_EXIT(dnx_ptp_port_delay_get(unit, port, value));
            break;
        }
        case bcmPortControlIngressQosModelRemark:
        case bcmPortControlIngressQosModelPhb:
        case bcmPortControlIngressQosModelTtl:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_port_model_get(unit, port, type, value));
            break;
        }
        case bcmPortControlOverlayRecycle:
        {
            dbal_enum_value_field_ingress_port_termination_profile_e llr_profile;
            dbal_enum_value_field_ingress_port_termination_profile_e vt_profile;
            /** get the ingress port termination profile and compare it with default */
            SHR_IF_ERR_EXIT(dnx_port_pp_llr_vtt_profile_get(unit, port, &llr_profile, &vt_profile));

            if (llr_profile == DBAL_ENUM_FVAL_INGRESS_PORT_TERMINATION_PROFILE_2ND_PASS_DROP_AND_CONTINUE)
            {
                *value = 1;
            }
            else
            {
                *value = 0;
            }

            break;
        }
        case bcmPortControlExtenderEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_coe_enable_get(unit, port, value));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_port_control_get type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_phy_get
 * Description:
 *      General PHY register read
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - (OUT) Data that was read
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnx_port_phy_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 *phy_data)
{
    int rc;
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(phy_data, _SHR_E_PARAM, "phy_data");

    if(flags & BCM_PORT_PHY_NOMAP)
    {
        logical_port = port;
    }else{
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    }

    rc = MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_port_phy_get, (unit, logical_port, flags, phy_reg_addr, phy_data));
    SHR_IF_ERR_EXIT(rc);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnx_port_phy_get: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x, phy_data=0x%08x\n"), unit, port, flags, phy_reg_addr, *phy_data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_phy_set
 * Description:
 *      General PHY register write
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnx_port_phy_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data)
{
    int rc;
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnx_port_phy_set: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x phy_data=0x%08x\n"), unit, port, flags, phy_reg_addr, phy_data));

    if(flags & BCM_PORT_PHY_NOMAP)
    {
        logical_port = port;
    }else{
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    }

    rc = MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_port_phy_set, (unit, logical_port, flags, phy_reg_addr, phy_data));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_phy_modify
 * Description:
 *      General PHY register modify
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 *      phy_mask - Bits to modify using phy_data
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnx_port_phy_modify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data,
    uint32 phy_mask)
{
    int rc;
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnx_port_phy_modify: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x phy_data=0x%08x phy_mask=0x%08x\n"),
              unit, port, flags, phy_reg_addr, phy_data, phy_mask));

    if(flags & BCM_PORT_PHY_NOMAP)
    {
        logical_port = port;
    }else{
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    }

    rc = MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_port_phy_modify,
                              (unit, logical_port, flags, phy_reg_addr, phy_data, phy_mask));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_linkscan_get
 * Purpose:
 *      Get the link scan state of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      linkscan - (OUT) Linkscan value (None, S/W, H/W)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnx_port_linkscan_get(
    int unit,
    bcm_port_t port,
    int *linkscan)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(linkscan, _SHR_E_PARAM, "linkscan");

    /*
     * Should not take API lock - bcm_dnx_linkscan_mode_get will take lock
     */
    rv = bcm_dnx_linkscan_mode_get(unit, port, linkscan);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_port_linkscan_set
 * Purpose:
 *      Set the link scan mode of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      linkscan - Linkscan mode (None, S/W, H/W)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnx_port_linkscan_set(
    int unit,
    bcm_port_t port,
    int linkscan)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_linkscan_mode_set(unit, port, linkscan));

exit:
    SHR_FUNC_EXIT;

}

/*
 * Function:
 *      bcm_dnx_port_link_status_get
 * Purpose:
 *      Return current Link up/down status, queries linkscan, if unable to
 *      retrieve status queries the PHY.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      status - (OUT) BCM_PORT_LINK_STATUS_DOWN \ BCM_PORT_LINK_STATUS_UP
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnx_port_link_status_get(
    int unit,
    bcm_port_t port,
    int *status)
{
    int link;
    int rc;
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(status, _SHR_E_PARAM, "status");

    /*
     * Get local port from port
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    rc = _bcm_link_get(unit, logical_port, &link);
    if (rc == BCM_E_DISABLED)
    {
        SHR_IF_ERR_EXIT(dnx_port_link_get(unit, port, &link));
    }
    else
    {
        SHR_IF_ERR_EXIT(rc);
    }

    if (link)
    {
        *status = BCM_PORT_LINK_STATUS_UP;
    }
    else
    {
        *status = BCM_PORT_LINK_STATUS_DOWN;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the specified statistics from the device for
 *        the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [in]  stat  - SNMP statistics type defined in bcm_port_stat_t.
 * \param [out] val - collected 64-bit statistics values.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_stat_get(
    int unit,
    bcm_gport_t port,
    bcm_port_stat_t stat,
    uint64 *val)
{
    bcm_stat_val_t type;
    dnx_algo_port_type_e port_type;
    bcm_port_t logical_port = 0;
    int index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    switch (stat)
    {
        case bcmPortStatIngressPackets:
        {
            type = DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1) ? snmpEtherStatsRXNoErrors : snmpIfInUcastPkts;
            break;
        }
        case bcmPortStatEgressPackets:
        {
            type = DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1) ? snmpEtherStatsTXNoErrors : snmpIfOutUcastPkts;
            break;
        }
        case bcmPortStatIngressBytes:
        {
            type = snmpIfInOctets;
            break;
        }
        case bcmPortStatEgressBytes:
        {
            type = snmpIfOutOctets;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_port_stat_get type %d is not supported\n", stat);
    }

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    if (stat == bcmPortStatIngressBytes && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
    {
        index = soc_jer2_counters_ilkn_rx_byte_counter;
        SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, port, stat, index, val));
    }
    else if (stat == bcmPortStatEgressBytes && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
    {
        index = soc_jer2_counters_ilkn_tx_byte_counter;
        SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, port, stat, index, val));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_stat_get(unit, logical_port, type, val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_stat_multi_get() API
 */
static shr_error_e
dnx_port_multi_get_verify(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_port_stat_t * stat_arr,
    uint64 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * make sure port argumet is local port, not Gport.
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    if (nstat <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nstat");
    }
    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_arr");
    SHR_NULL_CHECK(value_arr, _SHR_E_PARAM, "value_arr");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Get the specified statistics from the device for
 *        the port.
 *
 * \param [in]  unit  - chip unit id.
 * \param [in]  port  - logical port.
 * \param [in]  nstat - number of elements in stat array.
 * \param [in]  stat_arr  - array of SNMP statistics types defined in bcm_port_stat_t.
 * \param [out] value_arr - collected 64-bit statistics values.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_stat_multi_get(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_port_stat_t * stat_arr,
    uint64 *value_arr)
{
    int i, rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_multi_get_verify(unit, port, nstat, stat_arr, value_arr));

    for (i = 0; i < nstat; i++)
    {
        rv = bcm_dnx_port_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for JER2
 *
 * \return
 *   BCM_E_UNAVAIL
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_stat_get32(
    int unit,
    bcm_gport_t port,
    bcm_port_stat_t stat,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for JER2.\n");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for JER2
 *
 * \return
 *   BCM_E_UNAVAIL
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_stat_multi_get32(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_port_stat_t * stat_arr,
    uint32 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for JER2.\n");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_link_get(
    int unit,
    bcm_port_t port,
    int *link)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(link, _SHR_E_PARAM, "link");

    /*
     * Get local port from port
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_link_get(unit, logical_port, 0, link));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_pp_dsp_table_mapping_clear(
    int unit,
    int core,
    int pp_dsp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_pp_dsp_to_tm_mapping_set(
    int unit,
    int core,
    int pp_dsp,
    int tm_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, INST_SINGLE, (uint32) tm_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_pp_dsp_to_tm_mapping_get(
    int unit,
    int core,
    int pp_dsp,
    int *tm_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TM_PORT, INST_SINGLE, (uint32 *) tm_port);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_dsp_table_mapping_set(
    int unit,
    int tm_port,
    int core,
    int out_pp_port,
    uint32 destination_system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    /**
     * when using this function, it is assumed that:
     *                  TM_PORT == PP_DSP
     * and no conversion between the two is required
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, tm_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, (uint32) out_pp_port);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, destination_system_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_port_dsp_table_mapping_get(
    int unit,
    int core,
    int tm_port,
    int *out_pp_port,
    uint32 *destination_system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    /**
     * when using this function, it is assumed that:
     *                  TM_PORT == PP_DSP
     * and no conversion between the two is required
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, tm_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, (uint32 *) out_pp_port);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, destination_system_port);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_pp_mapping_set(
    int unit,
    bcm_core_t core_id,
    uint32 out_pp_port,
    uint32 tm_port,
    uint32 src_system_port,
    uint32 dst_system_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Set out_pp_port and destination_system_port */
    SHR_IF_ERR_EXIT(dnx_port_dsp_table_mapping_set(unit, tm_port, core_id, out_pp_port, dst_system_port));

    /** Provide mapping between source system port and gport */
    SHR_IF_ERR_EXIT(dnx_port_pp_src_system_port_set(unit, core_id, out_pp_port, src_system_port));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_rate_egress_pps_set(
    int unit,
    bcm_port_t port,
    uint32 pps,
    uint32 burst)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(bcm_dnx_fabric_if_link_rate_egress_pps_set(unit, logical_port, pps, burst));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_rate_egress_pps_get(
    int unit,
    bcm_port_t port,
    uint32 *pps,
    uint32 *burst)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(bcm_dnx_fabric_if_link_rate_egress_pps_get(unit, logical_port, pps, burst));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_local_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_t * local_port)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(local_port, _SHR_E_PARAM, "local_port");

    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport_info.local_port));

    *local_port = gport_info.local_port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for the following APIs:
 *
 *    dnx_port_info_set/get
 *    dnx_port_selective_set/get
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] info - port info
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_info_verify(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "port info");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the port info according the action mask. 
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] info - port info
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_selective_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    bcm_port_t logical_port = 0;
    uint32 mask = 0, support_action_mask, support_action_mask2;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_info_verify(unit, port, info));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(dnx_port_info_support_action_mask_get
                    (unit, logical_port, &support_action_mask, &support_action_mask2));

    mask = info->action_mask & support_action_mask;

    if (mask & BCM_PORT_ATTR_ENABLE_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, logical_port, &info->enable));
    }

    if (mask & BCM_PORT_ATTR_LINKSTAT_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_link_status_get(unit, logical_port, &info->linkstatus));
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK)
    {
        bcm_port_resource_t resource;
        SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, logical_port, &resource));

        info->speed = resource.speed;
    }

    /*
     * Get rx pause and tx pause 
     */
    if (mask & BCM_PORT_ATTR_PAUSE_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, logical_port, bcmPortControlLLFCTransmit, &info->pause_tx));
        SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, logical_port, bcmPortControlLLFCReceive, &info->pause_rx));
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_linkscan_get(unit, logical_port, &info->linkscan));
    }

    if (mask & BCM_PORT_ATTR_LEARN_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_learn_get(unit, logical_port, &info->learn));
    }

    if (mask & BCM_PORT_ATTR_VLANFILTER_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_vlan_member_get(unit, logical_port, &info->vlanfilter));
    }

    if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_untagged_vlan_get(unit, logical_port, &info->untagged_vlan));
    }

    if (mask & BCM_PORT_ATTR_STP_STATE_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_stp_get(unit, logical_port, &info->stp_state));
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_loopback_get(unit, logical_port, &info->loopback));
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_frame_max_get(unit, logical_port, &info->frame_max));
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_autoneg_get(unit, logical_port, &info->autoneg));
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_duplex_get(unit, logical_port, &info->duplex));
    }

    if (mask & BCM_PORT_ATTR_FAULT_MASK)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_fault_get(unit, logical_port, &info->fault));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get all the support port info for the given port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] info - port info
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_info_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_info_verify(unit, port, info));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    bcm_port_info_t_init(info);
    SHR_IF_ERR_EXIT(dnx_port_info_support_action_mask_get(unit, logical_port, &info->action_mask, &info->action_mask2));
    SHR_IF_ERR_EXIT(dnx_port_selective_get(unit, logical_port, info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set 1588 protocol configuaion for port 
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] config_count - (IN) config_array size 
 * \param [in] config_array - (IN) 1588 port configuration
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *      Disable port 1588 configuration  - config_count = 0
 *      Enable  port 1588 configuration  - config_count = 1 
 *                       config_array->flags =
 *                       BCM_PORT_TIMESYNC_DEFAULT            mast be on 
 *                       BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP when one step Tranparent clock (TC) is enabled system updates the 
 *                                                            correction field in 1588 Event 1588 messages.    
 *                       BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP when step step Tranparent clock (TC) is enabled system records the 
 *                                                            1588 Event 1588 messages TX time.    
 *                         1588 event messages:
 *                           1. SYNC
 *                           2. DELAY_REQ
 *                           3. PDELAY_REQ
 *                           4. PDELAY_RESP
 *                       config_array->pkt_drop, config_array->pkt_tocpu - bit masks indicating wehter to forward (drop-0,tocpu-0), 
 *                       drop(drop-1) or trap(tocpu-1) the packet

 * \see
 *   * None
 */

int
bcm_dnx_port_timesync_config_set(
    int unit,
    bcm_port_t port,
    int config_count,
    bcm_port_timesync_config_t * config_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ptp_timesync_config_set_verify(unit, port, config_count, config_array));
    SHR_IF_ERR_EXIT(dnx_ptp_port_timesync_config_set(unit, port, config_count, config_array));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - get 1588 protocol port configuaion
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] array_size - config_array size
 * \param [out] config_array - 1588 port configuration
 * \param [out] array_count - config_array size 
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 * output will be the follwing:
 * when port 1588 configuration Disabled - *array_count = 0
 * when port 1588 configuration Enabled  - *array_count = 1, config_array is set with the port configuration
 * \see
 *   * None
 */
int
bcm_dnx_port_timesync_config_get(
    int unit,
    bcm_port_t port,
    int array_size,
    bcm_port_timesync_config_t * config_array,
    int *array_count)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_ptp_timesync_config_get_verify(unit, port, array_size, config_array, array_count));
    SHR_IF_ERR_EXIT(dnx_ptp_port_timesync_config_get(unit, port, array_size, config_array, array_count));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set various PHY timesync controls for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [in] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_control_phy_timesync_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ptp_phy_timesync_set_verify(unit, port, type, value));
    SHR_IF_ERR_EXIT(dnx_ptp_phy_timesync_set(unit, port, type, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get various PHY timesync controls for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [out] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_control_phy_timesync_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ptp_phy_timesync_get_verify(unit, port, type, value));
    SHR_IF_ERR_EXIT(dnx_ptp_phy_timesync_get(unit, port, type, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set various PHY timesync configuaion for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_timesync_config_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ptp_phy_timesync_config_set_verify(unit, port, conf));
    SHR_IF_ERR_EXIT(dnx_ptp_phy_timesync_config_set(unit, port, conf));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get various PHY timesync configuaion for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_timesync_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ptp_phy_timesync_config_get_verify(unit, port, conf));
    SHR_IF_ERR_EXIT(dnx_ptp_phy_timesync_config_get(unit, port, conf));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_port_logical_verify(
    int unit,
    bcm_gport_t port)
{
    algo_gpm_gport_verify_type_e allowed_types[] =
        { ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_PORT,
        ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_FABRIC
    };
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify gport */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, port, COUNTOF(allowed_types), allowed_types));

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /** verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport_info.local_port));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_port_logical_get(
    int unit,
    bcm_gport_t port,
    bcm_port_t * logical_port)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    *logical_port = gport_info.local_port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to set the source MAC address transmitted in MAC 
 *           control pause frames for a specified port. MAC control pause frames are transmitted with 
 *           a well known destination address (01:80:c2:00:00:01) that is not controllable. However, 
 *           the source address can be set and retrieved using the bcm_port_pause_addr_set/get calls. 
 *           On switch initialization, the application software should set the pause frame source address 
 *           for all ports. These values are persistent across calls that enable or disable the transmission 
 *           of pause frames and need only be set once. Only MAC control pause frames generated by the MACs 
 *           in the switch device will contain the specified source address. The CPU is able to generate MAC 
 *           control packets with any source address that will be transmitted unchanged.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set the MAC address
 *   \param [in] mac - MAC address to transmit as the source address in MAC control pause frames
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_pause_addr_set(
    int unit,
    bcm_port_t port,
    bcm_mac_t mac)
{
    bcm_port_t port_local;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port_local));

    /** Set the MAC address */
    SHR_IF_ERR_EXIT(dnx_fc_port_mac_sa_set(unit, port_local, mac));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to get the source MAC address transmitted in MAC 
 *           control pause frames for a specified port 
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set the MAC address
 *   \param [out] mac - MAC address
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_pause_addr_get(
    int unit,
    bcm_port_t port,
    bcm_mac_t mac)
{
    bcm_port_t port_local;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    port_local = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port_local));

    /** Get the MAC address */
    SHR_IF_ERR_EXIT(dnx_fc_port_mac_sa_get(unit, port_local, mac));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_resource_speed_get API is not supported. Use bcm_port_resource_get API instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_resource_speed_set API is not supported. Use bcm_port_resource_set API instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_resource_speed_multi_set API is not supported. Use bcm_port_resource_multi_set API instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_speed_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_speed_set is no longer supported, please use bcm_port_resource_set to control the speed.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_speed_get(
    int unit,
    bcm_port_t port,
    int *speed)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_speed_get is no longer supported, please use bcm_port_resource_get to get the speed.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_speed_max(
    int unit,
    bcm_port_t port,
    int *speed)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_speed_max is no longer supported, please use bcm_port_resource_get to get the speed.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_interface_set(
    int unit,
    bcm_port_t port,
    bcm_port_if_t intf)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_interface_set is no longer supported, please use bcm_port_resource_set to align interface properties.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_interface_get(
    int unit,
    bcm_port_t port,
    bcm_port_if_t * intf)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_interface_get is no longer supported, please use bcm_port_resource_get to get interface properties.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_phy_reset(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_selective_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_selective_set(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_info_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_info_get(unit, port, info));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_info_restore(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_info_save(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_info_set(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}
