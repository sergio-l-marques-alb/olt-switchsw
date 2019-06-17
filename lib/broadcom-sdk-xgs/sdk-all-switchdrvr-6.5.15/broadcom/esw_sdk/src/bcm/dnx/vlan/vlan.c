/** \file vlan.c
 * $Id$
 *
 * General VLAN functionality for DNX.
 * Dedicated set of VLAN APIs are distributed between vlan_*.c files: \n
 * vlan_port.c - BCM_VLAN_PORT (Attachment-Circuit) functionality.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/field/field_port.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/stg/stg.h>
#include <bcm_int/dnx/vsi/vswitch_vpn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <bcm_int/common/multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/sand/sand_aux_access.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * DEFINES
 * {
 */
/** Check if the value of vsi is valid  */
#define DNX_VSI_VALID(_unit, _vsi) (_vsi < dnx_data_l2.vsi.nof_vsis_get(_unit))
/*
 * }
 */

/**
 * \brief
 *   Verify vlanId parameter for BCM-API: bcm_dnx_vlan_create()
 */
static shr_error_e
dnx_vlan_create_verify(
    int unit,
    bcm_vlan_t vid)
{
    SHR_FUNC_INIT_VARS(unit);

    BCM_DNX_VLAN_CHK_ID(unit, vid);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify vid_ptr null pointer for BCM-API: bcm_dnx_vlan_default_get_verify()
 */
static shr_error_e
dnx_vlan_default_get_verify(
    int unit,
    bcm_vlan_t * vid_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vid_ptr, _SHR_E_PARAM, "vid_ptr");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify vlanId parameter for BCM-API: bcm_dnx_vlan_default_set_verify()
 */
static shr_error_e
dnx_vlan_default_set_verify(
    int unit,
    bcm_vlan_t vid)
{
    SHR_FUNC_INIT_VARS(unit);
    BCM_DNX_VLAN_CHK_ID(unit, vid);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify Input parameters of bcm_dnx_vlan_gport_get_all() API.
 */
static shr_error_e
dnx_vlan_gport_get_all_verify(
    int unit,
    bcm_vlan_t vlan,
    int array_max,
    bcm_gport_t * gport_array,
    int *is_untagged,
    int *array_size)
{
    SHR_FUNC_INIT_VARS(unit);

    BCM_DNX_VLAN_CHK_ID(unit, vlan);

    SHR_NULL_CHECK(gport_array, _SHR_E_PARAM, "gport_array");
    SHR_NULL_CHECK(is_untagged, _SHR_E_PARAM, "is_untagged");
    SHR_NULL_CHECK(array_size, _SHR_E_PARAM, "array_size");

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Verify vlanId parameter for BCM-API: bcm_dnx_vlan_destroy()
 */
static shr_error_e
dnx_vlan_destroy_verify(
    int unit,
    bcm_vlan_t vid)
{
    SHR_FUNC_INIT_VARS(unit);

    BCM_DNX_VLAN_CHK_ID(unit, vid);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify parameters for BCM-API: bcm_vlan_control_port_set()
 */
static shr_error_e
dnx_vlan_control_port_set_verify(
    int unit,
    int port,
    bcm_vlan_control_port_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Verify arg pointer for BCM-API: bcm_vlan_control_port_get()
 */
static shr_error_e
dnx_vlan_control_port_get_verify(
    int unit,
    int port,
    bcm_vlan_control_port_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg_ptr");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify the parameters provided to bcm_vlan_gport_add() API are correct.
 */
static shr_error_e
dnx_vlan_gport_add_verify(
    int unit,
    bcm_vlan_t vlan,
    bcm_gport_t port,
    int flags)
{
    uint32 supported_flags_ingress, supported_flags_egress, supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    supported_flags_ingress =
        BCM_VLAN_GPORT_ADD_INGRESS_ONLY | BCM_VLAN_GPORT_ADD_MEMBER_REPLACE | BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE;

    supported_flags_egress =
        BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_MEMBER_REPLACE | BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE |
        BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE | BCM_VLAN_GPORT_ADD_UNTAGGED;

    supported_flags = BCM_VLAN_GPORT_ADD_MEMBER_REPLACE | BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE |
        BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE | BCM_VLAN_GPORT_ADD_UNTAGGED;

    /** If the given port is invalid - return error*/
    if (port == BCM_GPORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The given port is invalid");
    }
    /*
     * Check the provided VLAN ID is in range and the port is not invalid
     */
    BCM_DNX_VLAN_CHK_ID(unit, vlan);
    /*
     * Check if the set flags are supported
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The used flag BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD is not supported in J2, \
                     multicast is supported with bcm_multicast_* APIs, please read the backward compatibility document");
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_SERVICE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The used flag BCM_VLAN_GPORT_ADD_SERVICE is not supported in J2, please read the backward compatibility document");
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_INGRESS_ONLY)
        && _SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given invalid flags can be set either Ingress ONLY or egress ONLY not both");
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_INGRESS_ONLY))
    {
        supported_flags = supported_flags_ingress;
    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_EGRESS_ONLY))
    {
        supported_flags = supported_flags_egress;
    }
    if ((flags & ~supported_flags))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "The used flag or combination of flags = 0x%08X is not in the range of supported ones", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_vsi_table_default_set(
    int unit,
    bcm_vlan_t vsi)
{
    uint32 entry_handle_id;
    bcm_stg_t stg_defl = BCM_STG_INVALID;
    uint8 stp_topology_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the default STG
     */
    SHR_IF_ERR_EXIT(bcm_dnx_stg_default_get(unit, &stg_defl));

    stp_topology_id = BCM_DNX_STG_TO_TOPOLOGY_ID_GET(stg_defl);

    /*
     * Write to HW VSI table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    /*
     *    * DBAL Field STP topology ID has convertion based on HW device.
     *    * See DBAL Field STP topology ID logical to physical convertion for full details.
     *                                                                                     .
     *    * DBAL Field DA_NOT_FOUND_DEST is configuring the MC group for unknown DA flooding
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_BASIC);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOPOLOGY_ID, INST_SINGLE, stp_topology_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, vsi);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_vsi_table_clear_set(
    int unit,
    bcm_vlan_t vsi)
{
    uint32 entry_handle_id;
    int intf_id;
    uint8 intf_exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Update the properties of the vsi to be the default ones */
    SHR_IF_ERR_EXIT(dnx_vsi_profile_set_to_default(unit, vsi));

    intf_exists = 0;
    intf_id = vsi;
    SHR_IF_ERR_EXIT(dnx_l3_check_if_ingress_intf_exists(unit, intf_id, &intf_exists));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    if (intf_exists)
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOPOLOGY_ID, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, 0);
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
 *   Verify parameters for BCM-API: bcm_dnx_vlan_list()
 */
static shr_error_e
dnx_vlan_list_verify(
    int unit,
    bcm_vlan_data_t ** listp,
    int *countp)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(listp, _SHR_E_PARAM, "listp_ptr");
    SHR_NULL_CHECK(countp, _SHR_E_PARAM, "countp_ptr");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify vlan and control parameters for bcm_dnx_vlan_control_vlan_set
 */
static shr_error_e
dnx_vlan_control_vlan_set_verify(
    int unit,
    bcm_vlan_t vlan,
    bcm_vlan_control_vlan_t * control)
{
    int uc, mc, bc;
    uint8 is_vsi_allocated;

    SHR_FUNC_INIT_VARS(unit);

    if (!DNX_VSI_VALID(unit, vlan))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. vlan = %d is not a valid vsi.\n", vlan);
    }

    if (!DNX_VSI_VALID(unit, control->forwarding_vlan))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong parameter. control->forwarding_vlan = %d is not a valid vsi.\n",
                     control->forwarding_vlan);
    }

    /** Since FID and VSI are always equal in JER2, vlan must be equal to control->forwarding_vlan */
    if (vlan != control->forwarding_vlan)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vlan and control->forwarding_vlan must be equal.\r\n");
    }

    /** Make sure VSI is allocated */
    is_vsi_allocated = 0;
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vlan, &is_vsi_allocated));
    if (!is_vsi_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VSI %d must be allocated before calling this API.\r\n", vlan);
    }

    uc = control->unknown_unicast_group;
    mc = control->unknown_multicast_group;
    bc = control->broadcast_group;

    /** For configuring different MC group ID for different packet type use bcm_port_control_set */
    if (uc != mc || uc != bc)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unicast, Broadcast and Multicast must be configured to the same value.\r\n");
    }

    if (control->entropy_id != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->entropy_id in the bcm_vlan_control_vlan_set");
    }
    if (control->flags != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->flags");
    }
    if (control->vrf != 0)
    {
        SHR_ERR_EXIT(BCM_E_UNAVAIL, "No support for control->flags, use bcm_petra_l3_intf_create to update vrf ");
    }
    if ((control->outer_tpid != 0x8100) && (control->outer_tpid != 0))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->outer_tpid");
    }

    if (control->l2_mcast_flood_mode != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->l2_mcast_flood_mode");
    }

    if (control->ip4_mcast_flood_mode != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->ip4_mcast_flood_mode");
    }

    if (control->ip6_mcast_flood_mode != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->ip6_mcast_flood_mode");
    }
    if (control->forwarding_mode != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->forwarding_mode");
    }
    if (control->urpf_mode != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->urpf_mode");
    }

    if (control->cosq != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->cosq");
    }

    if (control->ingress_if != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->ingress_if");
    }

    if (control->qos_map_id != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->qos_map_id");
    }

    if (control->distribution_class != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->distribution_class");
    }

    if (control->trill_nonunicast_group != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->trill_nunicast_group");
    }

    if (control->source_trill_name != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->source_trill_name");
    }

    if (control->trunk_index != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->trunk_index");
    }

    if (control->nat_realm_id != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->nat_realm_id");
    }

    if (control->l3_if_class != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->l3_if_class");
    }

    if (control->vp_mc_ctrl != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->vp_mc_ctrl");
    }

    if (control->vpn != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->vpn");
    }

    if (control->egress_vlan != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->egress_vlan");
    }

    if (control->learn_flags != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->learn_flags");
    }

    if (control->sr_flags != 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->sr_flags");
    }

    if ((control->flags2 != 0) && (control->flags2 != BCM_VLAN_FLAGS2_IVL))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->flags2 equals to %d", control->flags2);
    }

    if (control->if_class < 0 || control->if_class > 3)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No support for control->if_class");
    }

    /*
     * Statistics check
     */
    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, BCM_CORE_ALL, control->ingress_stat_id, control->ingress_stat_pp_profile,
                     bcmStatCounterInterfaceIngressReceivePp));
    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, BCM_CORE_ALL, control->egress_stat_id, control->egress_stat_pp_profile,
                     bcmStatCounterInterfaceEgressTransmitPp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable ignoring incoming VID
 */
static shr_error_e
dnx_port_ignore_pkt_tag_set(
    int unit,
    int port,
    int arg)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    /** Set key fields */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        /** Enable ignoring incoming VID */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_INCOMING_VID_ENABLE, INST_SINGLE, arg);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the value of IGNORE_INCOMING_VID_ENABLE
 */
static shr_error_e
dnx_port_ignore_pkt_tag_get(
    int unit,
    int port,
    int *arg)
{
    uint32 entry_handle_id;
    uint32 ignore_incoming_vid_enable;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);

    /** Get the value of IGNORE_INCOMING_VID_ENABLE */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_INCOMING_VID_ENABLE, INST_SINGLE,
                                   &ignore_incoming_vid_enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *arg = ignore_incoming_vid_enable;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set default LIF for the given pp-port
 */
shr_error_e
dnx_vlan_control_port_default_lif_set(
    int unit,
    int port,
    uint32 default_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    /** Set key fields */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        /** Set default lif */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, default_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* see .h file for description
*/
shr_error_e
dnx_vlan_control_port_default_lif_get(
    int unit,
    int port,
    uint32 *default_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);

    /** Get the value of DBAL_FIELD_DEFAULT_LIF */
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, default_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify BCM-API: bcm_vlan_control_port_set/get for bcmVlanTranslateIngressMissDrop
 * Verify that the default-LIF of the port is one of the initial default LIFs: 
 * - initial drop LIF 
 * - initial bridge LIF
 * In addition, it returns the drop In-LIF and the current HW default In-LIF for further processing.
 */
static shr_error_e
dnx_vlan_control_port_set_get_miss_drop_verify(
    int unit,
    int port,
    uint32 *drop_in_lif,
    uint32 *hw_in_lif)
{
    int initial_bridge_in_lif;

    SHR_FUNC_INIT_VARS(unit);

    *drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);

    /*
     * Get initial bridge in_lif (local in-lif)
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, &initial_bridge_in_lif));

    /*
     * Retrieve the port's default In-LIF from the HW
     */
    SHR_IF_ERR_EXIT(dnx_vlan_control_port_default_lif_get(unit, port, hw_in_lif));

    if ((*hw_in_lif != *drop_in_lif) && (*hw_in_lif != initial_bridge_in_lif))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error - Port default LIF (%d) is not one of drop-LIF (%d) and initial-bridge-LIF (%d).",
                     *hw_in_lif, *drop_in_lif, initial_bridge_in_lif);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify vlan and control parameters for BCM-API: bcm_dnx_vlan_control_vlan_get()
 */
static shr_error_e
dnx_vlan_control_vlan_get_verify(
    int unit,
    bcm_vlan_t vlan,
    bcm_vlan_control_vlan_t * control)
{
    int vsi;
    uint8 is_vsi_allocated;

    SHR_FUNC_INIT_VARS(unit);

    vsi = vlan;

    /** Verify that we are in VLAN mode */
    DNX_L2_LEARN_MODE_VERIFY_AS_VLAN();

    if (!DNX_VSI_VALID(unit, vsi))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.\n", vsi);
    }

    /** Make sure VSI is allocated */
    is_vsi_allocated = 0;
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vlan, &is_vsi_allocated));
    if (!is_vsi_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VSI %d must be allocated before calling this API.\r\n", vlan);
    }

    SHR_NULL_CHECK(control, _SHR_E_PARAM, "control");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set properties per VSI
 *
 * \param [in] unit
 * \param [in] vlan - VSI\n
 * \param [in] control - additional properties of the vsi\n
 *     Fields description:
 *      * control.forwarding_vlan - must equal to VSI\n
 *      * control.unknown_unicast_group - multicast group ID for unknown-DA\n
 *      * control.unknown_multicast_group - must be set to the same values as unknown_unicast_group \n
 *      * control.broadcast_group - must be set to the same values as unknown_unicast_group \n
 *      * control.aging_cycles - the number of meta cycles before age out. By default according 8 cycles\n
 *      * control.ingress_stat_id - counter id for ingress VSI\n
 *      * control.ingress_stat_profile - counter profile for ingress VSI\n
 *      * control.egress_stat_id - counter id for engress VSI\n
 *      * control.egress_stat_profile - counter profile for egress VSI\n
 *      * control.flags2 - supported flag is BCM_VLAN_FLAGS2_IVL - if it is set the vsi profile will be set to IVL mode.
 *      Otherwise the vsi profile will be set to SVL mode.
 *
 * * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   * Use bcm_vlan_control_vlan_get before using this function and update only the relevant fields in control
 * \see
 *   * None
 */
int
bcm_dnx_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_vlan_control_vlan_t control)
{
    int vsi;
    int mc_id;
    uint32 entry_handle_id;
    uint32 learn_entry_handle_id;
    uint32 result_type;
    int old_vsi_profile;
    int new_vsi_profile;
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_control_vlan_set_verify(unit, vlan, &control));

    old_vsi_profile = 0;
    new_vsi_profile = 0;
    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    vsi = vlan;
    mc_id = _BCM_MULTICAST_ID_GET(control.unknown_unicast_group);

    /** Configure unknown DA destination */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));

    /** Replace the old VSI profile with old VSI data */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE, (uint32 *) &old_vsi_profile));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, (uint32 *) &result_type));
    old_vsi_profile = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, old_vsi_profile);

    {
        /** Get in_lif_profile_info for old_vsi_profile */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_vsi_profile, &in_lif_profile_info, ETH_RIF));
        if (control.flags2 & BCM_VLAN_FLAGS2_IVL)
        {
            /** Update the mact_mode in in_lif_profile_info to IVL */
            in_lif_profile_info.ingress_fields.mact_mode = DBAL_ENUM_FVAL_VSI_PROFILE_IVL_SVL_IVL;
        }
        else
        {
            /** Update the mact_mode in in_lif_profile_info to SVL */
            in_lif_profile_info.ingress_fields.mact_mode = DBAL_ENUM_FVAL_VSI_PROFILE_IVL_SVL_SVL;
        }
        /** Update in_lif_profile_info */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_vsi_profile, &new_vsi_profile, ETH_RIF,
                         DBAL_TABLE_ING_VSI_INFO_DB));
        new_vsi_profile = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, new_vsi_profile);
        /*
         * Set the VSI profile with field bits
         */
        SHR_IF_ERR_EXIT(dnx_field_port_profile_vsi_set(unit, control.if_class, &new_vsi_profile));

        if (old_vsi_profile != new_vsi_profile)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_LEARN, &learn_entry_handle_id));
            dbal_entry_key_field32_set(unit, learn_entry_handle_id, DBAL_FIELD_VSI_PROFILE, new_vsi_profile);
            if (control.flags2 & BCM_VLAN_FLAGS2_IVL)
            {
                /** Set LEARN_KEY_CONTEXT for the new_vsi_profile to IVL */
                dbal_entry_value_field32_set(unit, learn_entry_handle_id, DBAL_FIELD_LEARN_KEY_CONTEXT, INST_SINGLE,
                                             DBAL_ENUM_FVAL_LEARN_KEY_CONTEXT_IVL_OUTER_VID_VSI);
            }
            else
            {
                /** Set LEARN_KEY_CONTEXT for the new_vsi_profile to SVL */
                dbal_entry_value_field32_set(unit, learn_entry_handle_id, DBAL_FIELD_LEARN_KEY_CONTEXT, INST_SINGLE,
                                             DBAL_ENUM_FVAL_LEARN_KEY_CONTEXT_SVL);
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, learn_entry_handle_id, DBAL_COMMIT));
        }
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);

    if (control.ingress_stat_pp_profile != STAT_PP_PROFILE_INVALID)
    {
        if (result_type != DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_WITH_STATS)
        {
            SHR_IF_ERR_EXIT(dbal_entry_result_type_update(unit, entry_handle_id,
                                                          DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_WITH_STATS,
                                                          DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE));
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                     control.ingress_stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(control.ingress_stat_pp_profile));
    }
    else if (result_type != DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_BASIC)
    {
        SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_entry_result_type_update(unit, entry_handle_id,
                                                                DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_BASIC,
                                                                DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE), _SHR_E_PARAM);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, mc_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE, new_vsi_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (control.egress_stat_pp_profile != STAT_PP_PROFILE_INVALID)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGR_VSI_INFO_DB, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(control.egress_stat_pp_profile));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Update number of aging cycles for the vsi */
    SHR_IF_ERR_EXIT(dnx_l2_nof_age_cycles_before_ageout_set(unit, vsi, control.aging_cycles));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve properties per VSI
 *
 * \param [in] unit - Relevant unit
 * \param [in] vlan - VSI id
 * \param [out] control - Properties of VSI
 *     Fields description:
 *      * control->forwarding_vlan - equal to Vlan/VSI\n
 *      * control->unknown_unicast_group - Group to handle unknown unicast frames\n
 *      * control->unknown_multicast_group -  Group to handle unknown multicast frames\n
 *      * control->broadcast_group - Group to handle broadcast frames\n
 *      * control->ingress_stat_id - counter id for ingress VSI\n
 *      * control->ingress_stat_profile - counter profile for ingress VSI\n
 *      * control->egress_stat_id - counter id for egress VSI\n
 *      * control->egress_stat_profile - counter profile for egress VSI\n
 *
 * \return
 *   int
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_vlan_control_vlan_t * control)
{
    uint32 entry_handle_id;
    uint32 result_type = 0;
    int vsi;
    int old_vsi_profile;
    int vsi_learning_profile;
    uint32 stat_pp_profile;
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    vsi = vlan;

    bcm_vlan_control_vlan_t_init(control);

    control->forwarding_vlan = vsi;

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_control_vlan_get_verify(unit, vsi, control));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));

    /** Set key field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);

    /** Perform the action - all results fields of the table will be read to handle buffer */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE,
                     (uint32 *) (&control->unknown_multicast_group)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, INST_SINGLE, (uint32 *) &old_vsi_profile));

    old_vsi_profile = CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX, old_vsi_profile);

    /*
     * Set the VSI profile with field bits
     */
    SHR_IF_ERR_EXIT(dnx_field_port_profile_vsi_get(unit, old_vsi_profile, &(control->if_class)));

    if (dnx_stat_pp_result_type_verify(unit, DBAL_TABLE_ING_VSI_INFO_DB, result_type) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id,
                                                            DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                &control->ingress_stat_id));
            STAT_PP_ENGINE_PROFILE_SET(control->ingress_stat_pp_profile, stat_pp_profile,
                                       bcmStatCounterInterfaceIngressReceivePp);
        }
    }

    /**Egress statistics */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGR_VSI_INFO_DB, entry_handle_id));

    /** Set key field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);

    /** Perform the action - all results fields of the table will be read to handle buffer */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (dnx_stat_pp_result_type_verify(unit, DBAL_TABLE_EGR_VSI_INFO_DB, 0) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id,
                                                            DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            STAT_PP_ENGINE_PROFILE_SET(control->egress_stat_pp_profile, stat_pp_profile,
                                       bcmStatCounterInterfaceEgressTransmitPp);
        }
    }

    _BCM_MULTICAST_GROUP_SET(control->unknown_unicast_group, _BCM_MULTICAST_TYPE_L2, control->unknown_multicast_group);
    _BCM_MULTICAST_GROUP_SET(control->broadcast_group, _BCM_MULTICAST_TYPE_L2, control->unknown_multicast_group);
    _BCM_MULTICAST_GROUP_SET(control->unknown_multicast_group, _BCM_MULTICAST_TYPE_L2,
                             control->unknown_multicast_group);

    /** Get the vsi learning profile of the vsi */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &vsi_learning_profile));

    /** Get number of aging meta-cycles */
    SHR_IF_ERR_EXIT(dnx_l2_nof_age_cycles_before_ageout_get(unit, vsi, &control->aging_cycles));

    /** Get in_lif_profile_info */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_vsi_profile, &in_lif_profile_info, ETH_RIF));

    if (in_lif_profile_info.ingress_fields.mact_mode == DBAL_ENUM_FVAL_VSI_PROFILE_IVL_SVL_IVL)
    {
        control->flags2 = control->flags2 | BCM_VLAN_FLAGS2_IVL;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Create a VSI and set its default values
 *
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] vid -
 *   The incoming VSI ID, must be in the range of 0-4K.
 *   VSI ID 0 is invalid value.
 *   VSI - Virtual Switching Instance is a generalization of the VLAN concept used primarily in advanced bridging
 *   application. A VSI interconnects Logical Interfaces (LIFs).
 *   VSI is a logical partition of the MAC table and a flooding domain (comprising its member interfaces).
 *   For more information about VSI , see the PG PP document.
 * \return
 *   \retval Negative in case of an error. See shr_error_e, for example: VSI already created or input VSI range value is incorrect
 *   \retval Zero in case of NO ERROR
 * \remark
 *  API is avaiable only for the first 4K VSIs, in case a VSI larger than 4K then use bcm_vswitch_create. \n
 *  In nutshell, the API write to HW VSI table, Update Allocation-MNGR of VSI ID.
 *  In addition, note that by default VSI is added to the default STG ID (Spanning Tree Group). \n
 *  STG attribute used to filter incoming and outgoing packets according to Port, VSI and STP-state. \n
 *  For more information see bcm_stg_xxx APIs.
 */
int
bcm_dnx_vlan_create(
    int unit,
    bcm_vlan_t vid)
{
    int vsi;
    bcm_stg_t stg_defl = BCM_STG_INVALID;
    uint8 replaced = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_create_verify(unit, vid));

    vsi = vid;
    /*
     * DNX SW Algorithm, allocate VSI.
     * sw_state_algo_res_allocate() will return with error if resource for specified VSI has already been allocated.
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_alloc
                    (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, _bcmDnxVsiTypeVswitch, &vsi, &replaced));
    if (replaced)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS,
                     "vsi equals vlan %d was already allocated by other VPN API. In case API bcm_vlan_create is used, it should be the first VPN API \n",
                     vsi);
    }

    /*
     * Write to HW VSI table
     */
    SHR_IF_ERR_EXIT(dnx_vsi_table_default_set(unit, vsi));

    /*
     * Add the vsi to default STG
     */
    SHR_IF_ERR_EXIT(bcm_dnx_stg_default_get(unit, &stg_defl));
    SHR_IF_ERR_EXIT(bcm_dnx_stg_vlan_add(unit, stg_defl, vsi));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set not found destination field in VSI to black hole (drop).
 *        Applies only to basic result type.
 *
 */
static shr_error_e
dnx_vlan_set_unknown_da_drop(
    int unit,
    bcm_vlan_t vlan)
{
    int vsi;
    uint32 entry_handle_id;
    uint32 destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    vsi = vlan;
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, BCM_GPORT_BLACK_HOLE, &destination));

    /** Configure unknown DA destination */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_BASIC);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, destination & 0x7ffff);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - TX Tag get functionality , given core_id and pp port return if TX tag is valid and its vid
 *
 */
static shr_error_e
dnx_vlan_tx_tag_get(
    int unit,
    int core_id,
    int pp_port,
    uint32 *tx_tag_vid,
    uint32 *tx_tag_valid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TX_OUTER_TAG_VALID, INST_SINGLE, tx_tag_valid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TX_OUTER_TAG_VID, INST_SINGLE, tx_tag_vid);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify vlanId parameter for BCM-API: bcm_dnx_vlan_port_add()
 */
static shr_error_e
dnx_vlan_port_add_verify(
    int unit,
    bcm_vlan_t vid,
    bcm_pbmp_t * pbmp_pp_arr,
    bcm_pbmp_t * ubmp_pp_arr)
{
    uint32 tx_tag_vid;
    uint32 tx_tag_valid;
    int core_id;
    int pp_port_i;

    SHR_FUNC_INIT_VARS(unit);

    BCM_DNX_VLAN_CHK_ID(unit, vid);

    /*
     * Verify TX tag functionality for certain VID x UBMP
     * Illegal: Port is already legal untag on VID X and now required also on VID Y
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_ITER(pbmp_pp_arr[core_id], pp_port_i)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_get(unit, core_id, pp_port_i, &tx_tag_vid, &tx_tag_valid));

            if (tx_tag_valid && BCM_PBMP_MEMBER(ubmp_pp_arr[core_id], pp_port_i) && tx_tag_vid != vid)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Local pp port 0x%x core-id %d is already tagged on vid 0x%x \r\n",
                             pp_port_i, core_id, tx_tag_vid);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - VLAN membership set functionality
 * Given vid , set the VLAN membership IF bitmap
 * The Procedure will commit to HW(DBAL) to VLAN membership table.
 * The functionality is assumed to be symmetric between Ingress and Egress when adding port to VID with bcm_vlan_port_add API.
 * For asymmetric functionality is used bcm_vlan_gport_add, which provides special flags for configuring the ingress/egress membership.
 */
static shr_error_e
dnx_vlan_membership_set(
    int unit,
    int vid,
    bcm_pbmp_t vlan_membership_if_pbmp_ingress,
    bcm_pbmp_t vlan_membership_if_pbmp_egress,
    uint32 flags)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to Ingress VLAN_BMP table if the flag BCM_VLAN_GPORT_ADD_EGRESS_ONLY is unset
     */
    if (!(_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_EGRESS_ONLY)))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_MEMBERSHIP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_ID, vid);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBER_DOMAIN_BMP, INST_SINGLE,
                                         vlan_membership_if_pbmp_ingress.pbits);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Write to Egress VLAN_BMP table if the flag BCM_VLAN_GPORT_ADD_INGRESS_ONLY is unset
     */
    if (!(_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_INGRESS_ONLY)))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_MEMBERSHIP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_ID, vid);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBER_DOMAIN_BMP, INST_SINGLE,
                                         vlan_membership_if_pbmp_egress.pbits);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - TX Tag functionality
 * Given core_id and pp_port, set the Egress TX TAG / unTAG fucntioanlity.
 * TX Tag/Untag fucntionality will decide if to keep the Outer Tag or to remove it when going out to the out-port interface.
 * The functionality is done according to out-port (i.e. core-id and pp_port).
 * The HW has two fields,
 * tx_tag_valid - to do the functionality or not
 * tx_tag_vid - which VID to use in case it is invalid.
 */
static shr_error_e
dnx_vlan_tx_tag_set(
    int unit,
    int core_id,
    int pp_port,
    uint32 tx_tag_vid,
    uint32 tx_tag_valid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LOG_MODULE, "TX TAG : core_id %d  out_pp_port 0x%x tx_untag_valid %d tx_untag_vid 0x%x \n", core_id,
                pp_port, tx_tag_valid, tx_tag_vid);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_OUTER_TAG_VALID, INST_SINGLE, tx_tag_valid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_OUTER_TAG_VID, INST_SINGLE, tx_tag_vid);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This procedure converts an array of pbmp of pp ports
 *        per core to TX tag valid per PP port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] pbmp_pp_arr - Input param. Given the array of
 *          pbmp of pp ports per core.
 *   \param [in] tx_tag_pp_arr - Output param. Indicates which
 *          pp ports the functionality of TX tag is valid.
 *
 * \par INDIRECT INPUT:
 * \par DIRECT OUTPUT:
 *   shr_error_e Error handling
 * \par INDIRECT OUTPUT
 *   * tx_tag_pp_arr See DIRECT_INPUT.
 *   * Clears valid TX tag from HW(DBAL) Egress-Port table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_convert_pp_pbmp_to_tx_tag_valid_pbmp(
    int unit,
    bcm_pbmp_t * pbmp_pp_arr,
    bcm_pbmp_t * tx_tag_pp_arr)
{
    int core_id;
    int pp_port;
    uint32 tx_tag_valid;
    uint32 tx_tag_vid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pbmp_pp_arr, _SHR_E_PARAM, "pbmp_pp_arr");
    SHR_NULL_CHECK(tx_tag_pp_arr, _SHR_E_PARAM, "tx_tag_pp_arr");

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_CLEAR(tx_tag_pp_arr[core_id]);
        /*
         * convert pp port bmp(bitmap) to tx_tag pp bmp, required per core
         */
        BCM_PBMP_ITER(pbmp_pp_arr[core_id], pp_port)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_get(unit, core_id, pp_port, &tx_tag_vid, &tx_tag_valid));
            if (tx_tag_valid)
            {
                BCM_PBMP_PORT_ADD(tx_tag_pp_arr[core_id], pp_port);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert an array of pbmp of pp port for each core to VLAN Membership IF bitmap \n
 * Input: pbmp_pp_arr - Array of PP port bitmap per core \n
 * Output: VLAN membership interface bitmap (shared between cores) \n
 * Note: VLAN Membership IF bitmap is shared between cores and not per core
 *
 */
static shr_error_e
dnx_vlan_pbmp_pp_to_vlan_mem_if_bmp(
    int unit,
    bcm_pbmp_t * pbmp_pp_arr,
    bcm_pbmp_t * vlan_mem_if_bmp)
{
    int vlan_mem_if;
    uint32 class_id;
    bcm_gport_t gport;
    bcm_port_t pp_port_i;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pbmp_pp_arr, _SHR_E_PARAM, "pbmp_pp_arr");
    SHR_NULL_CHECK(vlan_mem_if_bmp, _SHR_E_PARAM, "vlan_mem_if_bmp");

    BCM_PBMP_CLEAR(*vlan_mem_if_bmp);
    /*
     * The below loop will go over each core and update the related HW vlan_mem_domain
     */

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_ITER(pbmp_pp_arr[core_id], pp_port_i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_gport_get(unit, core_id, pp_port_i, &gport));
            SHR_IF_ERR_EXIT(bcm_dnx_port_class_get(unit, gport, bcmPortClassVlanMember, &class_id));
            vlan_mem_if = class_id;
            BCM_PBMP_PORT_ADD(*vlan_mem_if_bmp, vlan_mem_if);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * The function converts pbmp and ubmp to arrays of local pp ports in pbmp_pp_arr and ubmp_pp_arr
 * In addition the function converts the local pp ports to tx_tag_pp_arr - indicating the ports, for with tx tag is enabled
 */
static shr_error_e
dnx_vlan_pbmp_pp_arr_get(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t ubmp,
    bcm_pbmp_t * pbmp_pp_arr,
    bcm_pbmp_t * ubmp_pp_arr,
    bcm_pbmp_t * tx_tag_pp_arr,
    int flags)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pbmp_pp_arr, _SHR_E_PARAM, "pbmp_pp_arr");
    SHR_NULL_CHECK(ubmp_pp_arr, _SHR_E_PARAM, "ubmp_pp_arr");
    SHR_NULL_CHECK(tx_tag_pp_arr, _SHR_E_PARAM, "tx_tag_pp_arr");
    /*
     * Retrieve information before verify:
     * Local pp ports according to pbmp, ubmp input params
     * Existance of TX-Tag on pp ports which are indicated on ubmp
     */
    /*
     * Map pbmp, ubmp to local pp port pbmp
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logical_pbmp_to_pp_pbmp_get(unit, pbmp, pbmp_pp_arr));
    SHR_IF_ERR_EXIT(dnx_algo_port_logical_pbmp_to_pp_pbmp_get(unit, ubmp, ubmp_pp_arr));
    /*
     * Retrieve TX tag valid local pp pbmp, if the flag BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE is unset
     */
    if (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_convert_pp_pbmp_to_tx_tag_valid_pbmp(unit, pbmp_pp_arr, tx_tag_pp_arr));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - VLAN membership get functionality
 * Given vid, and flags get the respective VLAN membership IF bitmap.
 * The Procedure will get information from HW(DBAL) VLAN membership tables - INGRESS_VLAN_MEMBERSHIP and EGRESS_VLAN_MEMBERSHIP.
 * The ingress vlan membership bitmap will be received on vlan_membership_if_pbmp_ingress and the egress one on vlan_membership_if_pbmp_egress.
 */
static shr_error_e
dnx_vlan_membership_get(
    int unit,
    int vid,
    bcm_pbmp_t * vlan_membership_if_pbmp_ingress,
    bcm_pbmp_t * vlan_membership_if_pbmp_egress,
    int flags)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get from Ingress VLAN_BMP table information for the vlan membership if pbmp, if the EGRESS_ONLY flag is unset
     */
    if (!(_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_EGRESS_ONLY)))
    {
        SHR_NULL_CHECK(vlan_membership_if_pbmp_ingress, _SHR_E_PARAM, "vlan_membership_if_pbmp_ingress");
        BCM_PBMP_CLEAR(*vlan_membership_if_pbmp_ingress);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_MEMBERSHIP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_ID, vid);
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBER_DOMAIN_BMP, INST_SINGLE,
                                       vlan_membership_if_pbmp_ingress->pbits);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Get from Egress VLAN_BMP table information for the vlan membership if pbmp, if the INGRESS_ONLY flag is unset
     */
    if (!(_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_INGRESS_ONLY)))
    {
        SHR_NULL_CHECK(vlan_membership_if_pbmp_egress, _SHR_E_PARAM, "vlan_membership_if_pbmp_egress");
        BCM_PBMP_CLEAR(*vlan_membership_if_pbmp_egress);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_MEMBERSHIP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_ID, vid);
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBER_DOMAIN_BMP, INST_SINGLE,
                                       vlan_membership_if_pbmp_egress->pbits);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function is mutual for bcm_dnx_vlan_port_add and bcm_dnx_vlan_gport_add APIs \n
 * Based on the incoming vlan, local pbmp and ubmp arrays, it sets the vlan membership and  tx_tag.
 * If the function is being called from bcm_dnx_vlan_gport_add, dnx_vlan_port_add will update all relevant HW fields according to the provided flags.
 * Since the bcm_dnx_vlan_gport_add allows using the BCM_VLAN_GPORT_ADD_MEMBER_REPLACE flag, the API can also remove previously added port to the vlan membership.
 * If the function is invoked from bcm_dnx_vlan_port_add, replace can't be performed.
 * We use bcm_vlan_port_remove instead.
 */
static shr_error_e
dnx_vlan_port_add(
    int unit,
    bcm_vlan_t vlan,
    bcm_pbmp_t * pbmp_pp_arr,
    bcm_pbmp_t * ubmp_pp_arr,
    bcm_pbmp_t * tx_tag_pp_arr,
    int flags)
{
    int core_id;
    bcm_port_t pp_port_i;
    uint32 tx_tag_vid;
    uint32 tx_tag_valid;
    uint32 tx_tag_update;
    bcm_pbmp_t vlan_mem_if_bmp_set;
    bcm_pbmp_t vlan_mem_if_bmp_update_ingress;
    bcm_pbmp_t vlan_mem_if_bmp_update_egress;

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(vlan_mem_if_bmp_set);
    BCM_PBMP_CLEAR(vlan_mem_if_bmp_update_ingress);
    BCM_PBMP_CLEAR(vlan_mem_if_bmp_update_egress);

    /*
     * Based on the input flags update the ingress/egress vlan membership with the provided port
     */
    if ((!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE)))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_pbmp_pp_to_vlan_mem_if_bmp(unit, pbmp_pp_arr, &vlan_mem_if_bmp_set));
        SHR_IF_ERR_EXIT(dnx_vlan_membership_get
                        (unit, vlan, &vlan_mem_if_bmp_update_ingress, &vlan_mem_if_bmp_update_egress, flags));
        if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_MEMBER_REPLACE))
        {
            if (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_EGRESS_ONLY))
            {
                BCM_PBMP_XOR(vlan_mem_if_bmp_update_ingress, vlan_mem_if_bmp_set);
            }
            if (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_INGRESS_ONLY))
            {
                BCM_PBMP_XOR(vlan_mem_if_bmp_update_egress, vlan_mem_if_bmp_set);
            }
        }
        else
        {
            if (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_EGRESS_ONLY))
            {
                BCM_PBMP_OR(vlan_mem_if_bmp_update_ingress, vlan_mem_if_bmp_set);
            }
            if (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_INGRESS_ONLY))
            {
                BCM_PBMP_OR(vlan_mem_if_bmp_update_egress, vlan_mem_if_bmp_set);
            }
        }
        SHR_IF_ERR_EXIT(dnx_vlan_membership_set
                        (unit, vlan, vlan_mem_if_bmp_update_ingress, vlan_mem_if_bmp_update_egress, flags));
    }
    /*
     * Based on the input flags update the egress tx tag vlan membership with the provided port
     */
    if (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_INGRESS_ONLY)
        && (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE)))
    {
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            if (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_MEMBER_REPLACE))
            {
                BCM_PBMP_ITER(pbmp_pp_arr[core_id], pp_port_i)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_get(unit, core_id, pp_port_i, &tx_tag_vid, &tx_tag_valid));
                    /*
                     * Disable TX tag in case port was TX-tag valid and now it is not.
                     * BCM_PBMP_MEMBER(tx_tag_pp_arr[core_id], pp_port_i) indicates whether port 'pp_port_i' was 'TX-tag valid'.
                     * BCM_PBMP_MEMBER(ubmp_pp_arr[core_id], pp_port_i) indicates whether port is now set to 'NOT TX-tag valid'.
                     */
                    if (BCM_PBMP_MEMBER(tx_tag_pp_arr[core_id], pp_port_i)
                        && !BCM_PBMP_MEMBER(ubmp_pp_arr[core_id], pp_port_i) && (vlan == tx_tag_vid))
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_set(unit, core_id, pp_port_i, 0, FALSE));
                    }
                    /*
                     * Enable TX tag in case port is ubmp (not matter if it was or currently is)
                     */
                    if (BCM_PBMP_MEMBER(ubmp_pp_arr[core_id], pp_port_i))
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_set(unit, core_id, pp_port_i, vlan, TRUE));
                    }
                }
            }
            else
            {
                BCM_PBMP_ITER(pbmp_pp_arr[core_id], pp_port_i)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_get(unit, core_id, pp_port_i, &tx_tag_vid, &tx_tag_valid));
                    /*
                     * Replace logic - the variable tx_tag_valid indicates if the port is added to untagged pbmp or not.
                     * Retrieve information about the current tx_tag indication on the pp_port_i and update the membership with the opposite value,
                     * if the port is added as untagged for the given vlan.
                     */
                    if (vlan == tx_tag_vid)
                    {
                        tx_tag_update = !tx_tag_valid;
                        SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_set(unit, core_id, pp_port_i, vlan, tx_tag_update));
                    }
                    /*
                     * If the port is not added with tx_tag_valid indication, but we want to replace this - meaning enable tx_tag_valid
                     */
                    else if (tx_tag_vid == 0)
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_set(unit, core_id, pp_port_i, vlan, TRUE));
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This procedure converts an array of pbmp of pp ports
 *        per core to TX tag valid per PP port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] vid - Input param Cheks if the tx_tag_vid matches the given vid
 *   \param [in] pbmp_pp_arr - Input param. Given the array of
 *          pbmp of pp ports per core.
 *   \param [in] ubmp_pp_arr - Output param. Indicates which
 *          pp ports the functionality of TX tag is valid .
 *
 * \par INDIRECT INPUT:
 * \par DIRECT OUTPUT:
 *   shr_error_e Error handling
 * \par INDIRECT OUTPUT
 *   * ubmp_pp_arr See DIRECT_INPUT.
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_convert_pp_pbmp_to_ubmp(
    int unit,
    bcm_vlan_t vid,
    bcm_pbmp_t * pbmp_pp_arr,
    bcm_pbmp_t * ubmp_pp_arr)
{
    int core_id;
    int pp_port;
    uint32 tx_tag_valid;
    uint32 tx_tag_vid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pbmp_pp_arr, _SHR_E_PARAM, "pbmp_pp_arr");
    SHR_NULL_CHECK(ubmp_pp_arr, _SHR_E_PARAM, "ubmp_pp_arr");

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_CLEAR(ubmp_pp_arr[core_id]);
        /*
         * convert pp port bmp(bitmap) to tx_tag pp bmp, required per core
         */
        BCM_PBMP_ITER(pbmp_pp_arr[core_id], pp_port)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_get(unit, core_id, pp_port, &tx_tag_vid, &tx_tag_valid));
            if (tx_tag_valid & (vid == tx_tag_vid))
            {
                BCM_PBMP_PORT_ADD(ubmp_pp_arr[core_id], pp_port);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function unifies the behavior between bcm_vlan_port_remove and bcm_vlan_gport_delete
 *  \param [in] unit - Relevant unit.
 *  \param [in] vid  - VLAN Identifier.
 *  \param [in] pbmp_pp_arr - PP ports bitmap per core, containing the relevant ports to be removed.
 *
 *  \return
 *          Error indication according to shr_error_e enum
 * \remark
 * None
 */
shr_error_e
dnx_vlan_port_remove(
    int unit,
    bcm_vlan_t vid,
    bcm_pbmp_t * pbmp_pp_arr)
{
    bcm_pbmp_t vlan_mem_if_bmp_ingress;
    bcm_pbmp_t vlan_mem_if_bmp_egress;
    bcm_pbmp_t vlan_mem_if_bmp_pbmp;
    bcm_pbmp_t vlan_mem_if_bmp_neg;
    bcm_pbmp_t ubmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    int pp_port_i;
    int core_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Retrieve information of the vlan membership interface pbmp, based on vid for both ingress and egress sides.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_membership_get(unit, vid, &vlan_mem_if_bmp_ingress, &vlan_mem_if_bmp_egress, 0));
    SHR_IF_ERR_EXIT(dnx_vlan_pbmp_pp_to_vlan_mem_if_bmp(unit, pbmp_pp_arr, &vlan_mem_if_bmp_pbmp));

    /*
     * Retrieve information of the physical ports, associated to the vlan membership, which have tx tag enabled
     * The function will return array of physical ubmp per core.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_convert_pp_pbmp_to_ubmp(unit, vid, pbmp_pp_arr, ubmp_pp_arr));

    /*
     * Get the negative of the input pbmp, perform logical AND with the vlan membership pbmp.
     * This will reside in pbmp, containing only the ports, which should be set on the device.
     */

    BCM_PBMP_NEGATE(vlan_mem_if_bmp_neg, vlan_mem_if_bmp_pbmp);
    BCM_PBMP_AND(vlan_mem_if_bmp_ingress, vlan_mem_if_bmp_neg);
    BCM_PBMP_AND(vlan_mem_if_bmp_egress, vlan_mem_if_bmp_neg);

    /*
     * Update the vlan membership for both ingress and egress vlan membership table.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_membership_set(unit, vid, vlan_mem_if_bmp_ingress, vlan_mem_if_bmp_egress, 0));

    /*
     * Set egress transmit to be untagged for all removed ports, for the given VID, if the tx tag was set to be valid for them.
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_ITER(ubmp_pp_arr[core_id], pp_port_i)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_set(unit, core_id, pp_port_i, 0, FALSE));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API sets VLAN port membership using VLAN ID and a set of ports. \n
 * The input pbmp will also be updated to be the egress Multicast VLAN group ID (TBD). \n
 * In addition, HW is set to either keep tag or to remove it when packets go out of specified ports.
 * This option is selected based on input 'ubmp'.
 *
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *   \param [in] vid -
 *     VLAN Identifier.
 *   \param [in] pbmp -
 *     logical ports bitmap. Assume symmetric (Ingress & Egress) configuration
 *   \param [in] ubmp -
 *     logical ports bitmap that should be untagged when outgoing packet is transmitted with vid
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for example: ubmp invalid cases.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * Update HW Ingress and Egress VLAN-Port membership table
 *   * Update HW Egress outgoing port untag functionality.
 *   * Update HW Egress VLAN Multicast member (TBD)
 * \remark
 *   1. Unlike previous devices, outgoing port untag functionality is more limited. \n
 *   In previous DPP devices, configuration could allow for any Out-Port x VLAN to be untagged. \n
 *   In DNX devices, configuration allow only one VLAN per port to be untagged. \n
 *   Example: \n
 *      In DPP devices, port 13 can set VLAN 15 , VLAN 16 to be set untagged \n
 *      In DNX devices, it is not possible. Port 13 can set VLAN 15 or VLAN 16 to be untagged but not both.
 *   2. VLAN membership table is accessed not according to port but according to port mapping "VLAN membership IF". \n
 *   VLAN membership IF provides a namespace to VLAN membership from a port either physical (local port) or logical (LIF gport for example: PWE) \n
 *   This API only configure Simple-Bridge application and so relates only to the physical part of VLAN membership.
 *   For Logical port part see \ref bcm_vlan_gport_add.
 *   VLAN membership IF is updated according to \ref bcm_port_class_set. \n
 *   This API only assume it gets the information that was configurd before. \n
 *   Note: In case VLAN membership IF mapping is updated there is no automatic update of the VLAN membership table. This is the responsibility of the user.
 */
shr_error_e
bcm_dnx_vlan_port_add(
    int unit,
    bcm_vlan_t vid,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t ubmp)
{
    bcm_pbmp_t pbmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t ubmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t tx_tag_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /*
     * Retrieve information before verify:
     * Local pp ports according to pbmp, ubmp input params
     * Existance of TX-Tag on pp ports which are indicated on ubmp
     */
    /*
     * Map pbmp, ubmp to local pp port pbmp
     */
    SHR_IF_ERR_EXIT(dnx_vlan_pbmp_pp_arr_get(unit, pbmp, ubmp, pbmp_pp_arr, ubmp_pp_arr, tx_tag_pp_arr, 0));
    /*
     * Verify is done after convert of bitmaps in order to validate better that the ubmp is provided as expected.
     * See more in the verify function.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_add_verify(unit, vid, ubmp_pp_arr, tx_tag_pp_arr));

    /*
     * Set VLAN membership functionality
     * Convert to VLAN membership IF and commit
     * For VLAN membership IF informaiton , see remark number 2 in Procedure documentation.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_add(unit, vid, pbmp_pp_arr, ubmp_pp_arr, tx_tag_pp_arr, 0));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert an array of vlan membership bitmap IF to pbmp of pp port for each core \n
 * Input: VLAN membership interface bitmap (shared between cores) \n pbmp_pp_arr - Array of PP port bitmap per core \n
 * Output:  pbmp_pp_arr - Array of PP port bitmap per core \n
 * Note: VLAN Membership IF bitmap is shared between cores and not per core
 *
 */

static shr_error_e
dnx_vlan_mem_if_bmp_to_pbmp_pp(
    int unit,
    bcm_pbmp_t vlan_mem_if_bmp,
    bcm_pbmp_t * pbmp_pp_arr)
{
    int port_i;
    uint32 class_id;
    int vlan_member;
    int port_in_lag;
    bcm_pbmp_t logical_ports;
    /*
     * bitmap of the ports associated with the VLAN membership IF
     */
    bcm_pbmp_t vlan_mem_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pbmp_pp_arr, _SHR_E_PARAM, "pbmp_pp_arr");

    BCM_PBMP_CLEAR(*pbmp_pp_arr);
    BCM_PBMP_CLEAR(vlan_mem_bmp);
    BCM_PBMP_CLEAR(logical_ports);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_ports));
    BCM_PBMP_ITER(logical_ports, port_i)
    {
        /*
         *  The below loop will go over each element of the vlan_mem_domain and update pbmp_pp_arr per core
         */
        BCM_PBMP_ITER(vlan_mem_if_bmp, vlan_member)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port_i, &port_in_lag));
            if (port_in_lag == TRUE)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(bcm_dnx_port_class_get(unit, port_i, bcmPortClassVlanMember, &class_id));
            if (vlan_member == class_id)
                BCM_PBMP_PORT_ADD(vlan_mem_bmp, port_i);
        }
    }
    /*
     * Convert the vlan_mem_bmp to array of physical pp ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logical_pbmp_to_pp_pbmp_get(unit, vlan_mem_bmp, pbmp_pp_arr));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API removes ports from the VLAN port membership \n
 * the removal is done based on the incoming vid and pbmp \n
 * The pbmp contains the ports, to be removed. \n
 *  \param [in] unit - Relevant unit.
 *  \param [in] vid  - VLAN Identifier.
 *  \param [in] pbmp - Logical ports bitmap, containing the relevant ports to be removed.
 *
 *  \return
 *          Error indication according to shr_error_e enum
 * \remark
 *    vlan_mem_if_bmp_ingress is decided according to Ingress table and vlan_mem_if_bmp_egress is decided based on the egress Table.
 *    The functionality for this API is assumed to be symmetric for both tables.
 *    The removal is done for both tagged and untagged functionality.
 *    The removal is symmetric of the VLAN port membership for both Ingress and Egress.
 *    The egress transmit is updated to be untagged for all removed ports.
 *    The vlan membership if is updated according to the removed ports.
 */
shr_error_e
bcm_dnx_vlan_port_remove(
    int unit,
    bcm_vlan_t vid,
    bcm_pbmp_t pbmp)
{
    bcm_pbmp_t pbmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Map the incoming pbmp to local pp port pbmp
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logical_pbmp_to_pp_pbmp_get(unit, pbmp, pbmp_pp_arr));
    SHR_IF_ERR_EXIT(dnx_vlan_port_remove(unit, vid, pbmp_pp_arr));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API gets VLAN port membership using VLAN ID \n
 * Based on the input vid the API gets information for the vlan membership \n
 * From the vlan membership if pbmp get the pbmp and the ubmp.
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *   \param [in] vid -
 *     VLAN Identifier.
 *   \param [in] pbmp -
 *     logical ports bitmap. Assume symmetric (Ingress & Egress) configuration
 *   \param [in] ubmp -
 *     logical ports bitmap that should be untagged when outgoing packet is transmitted with vid
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for example: ubmp invalid cases.
 *   \retval Zero in case of NO ERROR
 *   \retval pbmp - port bitmap of the membered ports
 *   \retval ubmp - port bitmap of the membered ports, which has tx_tag valid
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *    pbmp is decided according to Ingress table since it is symmetric between Ingress & Egress.
 */
shr_error_e
bcm_dnx_vlan_port_get(
    int unit,
    bcm_vlan_t vid,
    bcm_pbmp_t * pbmp,
    bcm_pbmp_t * ubmp)
{
    bcm_pbmp_t vlan_mem_if_bmp_ingress, vlan_mem_if_bmp_egress;
    bcm_pbmp_t pbmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t ubmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_core_t core;
    bcm_pbmp_t pbmp_core, ubmp_core;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve information of the vlan membership if port bitmap based on given vid
     */
    SHR_IF_ERR_EXIT(dnx_vlan_membership_get(unit, vid, &vlan_mem_if_bmp_ingress, &vlan_mem_if_bmp_egress, 0));
    BCM_PBMP_AND(vlan_mem_if_bmp_ingress, vlan_mem_if_bmp_egress);
    /*
     * Retrieve information of the physical ports, associated to the vlan membership
     * The function will return array of physical pbmp per core.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_mem_if_bmp_to_pbmp_pp(unit, vlan_mem_if_bmp_ingress, pbmp_pp_arr));

    /*
     * Retrieve information of the physical ports, associated to the vlan membership, which have tx tag enabled
     * The function will return array of physical ubmp per core.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_convert_pp_pbmp_to_ubmp(unit, vid, pbmp_pp_arr, ubmp_pp_arr));

    /*
     * Map pbmp_pp_arr and ubmp_pp_arr to pbmp and ubmp
     */
    BCM_PBMP_CLEAR(*pbmp);
    BCM_PBMP_CLEAR(*ubmp);
    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_pbmp_to_logical_pbmp_get(unit, core, pbmp_pp_arr[core], &pbmp_core));
        BCM_PBMP_OR(*pbmp, pbmp_core);

        SHR_IF_ERR_EXIT(dnx_algo_port_pp_pbmp_to_logical_pbmp_get(unit, core, ubmp_pp_arr[core], &ubmp_core));
        BCM_PBMP_OR(*ubmp, ubmp_core);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Destroy an allocated VSI
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *   \param [in] vid -
 *     The incoming VSI ID, must be in the range of 0-4K.
 *     VSI ID 0 is invalid parameter and VSI ID 1 is default VSI.
 *     VSI - Virtual Switching Instance is a generalization of the VLAN concept used primarily in advanced bridging
 *     application. A VSI interconnects Logical Interfaces (LIFs).
 *     VSI is a logical partition of the MAC table and a flooding domain (comprising its member interfaces).
 *     For more information about VSI , see the PG PP document.
 * \par INDIRECT INPUT:
*   * Allocation-MNGR of VSI ID.
 * \par DIRECT OUTPUT:
 *  Negative in case of an error. See shr_error_e, for
 *           example: VSI was not created or input VSI range
 *           value is incorrect
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * Write to HW VSI table, allocation-MNGR of VSI ID.
 * \remark
 *
 */
int
bcm_dnx_vlan_destroy(
    int unit,
    bcm_vlan_t vid)
{

    int vsi;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    bcm_stg_t stg;
    uint8 is_vsi_used;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_destroy_verify(unit, vid));
    vsi = vid;

    /*
     * Check that the vsi was allocated for Vswitch
     */
    is_vsi_used = 0;
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, vsi, _bcmDnxVsiTypeVswitch, &is_vsi_used));

    if (is_vsi_used == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "vsi %d doesn't exist for VLAN/Vswitch\n", vsi);
    }

    /*
     * Clear vlan membership
     */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_get(unit, vid, &pbmp, &ubmp));
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_remove(unit, vid, pbmp));

    /*
     * Remove vid from its spanning tree group, and don't add it to default STG.
     */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_stg_get(unit, vid, &stg));
    SHR_IF_ERR_EXIT(dnx_stg_vlan_remove(unit, stg, vid, FALSE));

    /** check if VSI used by only by Vswitch */
    is_vsi_used = 0;
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_check_except_type(unit, vsi, _bcmDnxVsiTypeVswitch, &is_vsi_used));

    /** free vsi resource */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_dealloc(unit, _bcmDnxVsiTypeVswitch, vsi));

    /** re-initialize vswitch instance, clear MC group */
    if (!is_vsi_used)
    {
        SHR_IF_ERR_EXIT(dnx_vsi_table_clear_set(unit, vsi));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Destroy all allocated VSIs between BCM_VLAN_MIN till BCM_VLAN_COUNT.
 * Does not destroy: default VSI (derived from bcm_vlan_default_get).
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * Write to HW clear VSI table, delete allocation-MNGR of all but default
 *     VSI IDs.
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_vlan_destroy
 *
 */
int
bcm_dnx_vlan_destroy_all(
    int unit)
{

    int vsi;
    bcm_vlan_t default_vsi;
    uint8 is_allocated = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /*
     * Get the value of the default VSI from SW state, in order not to be destroyed.
     */
    SHR_IF_ERR_EXIT(vlan_db.vlan_info.default_vid.get(unit, &default_vsi));

    for (vsi = BCM_VLAN_MIN; vsi < BCM_VLAN_COUNT; vsi++)
    {
        if (BCM_VLAN_VALID(vsi))
        {
            /*
             * Check whether the vsi was allocated:
             */
            SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, vsi, _bcmDnxVsiTypeVswitch, &is_allocated));
            if ((is_allocated == TRUE) && (vsi != default_vsi))
            {
                SHR_IF_ERR_EXIT(bcm_dnx_vlan_destroy(unit, vsi));
            }
        }
        else
        {
            continue;
        }
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_vlan_default_set_internal(
    int unit,
    bcm_vlan_t vid,
    int is_init)
{
    bcm_vlan_t default_vid;
    SHR_FUNC_INIT_VARS(unit);
    if (!is_init)
    {
        /*
         * Destroy previous default vlan
         */
        SHR_IF_ERR_EXIT(vlan_db.vlan_info.default_vid.get(unit, &default_vid));
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_destroy(unit, default_vid));
    }
    SHR_IF_ERR_EXIT(vlan_db.vlan_info.default_vid.set(unit, vid));
    SHR_IF_ERR_EXIT(vlan_db.vlan_info.default_vid.get(unit, &default_vid));
    if (default_vid)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_create(unit, default_vid));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_vlan_init(
    int unit)
{
    uint8 is_init;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init vlan sw state
     */
    SHR_IF_ERR_EXIT(vlan_db.is_init(unit, &is_init));
    if (!is_init)
    {
        SHR_IF_ERR_EXIT(vlan_db.init(unit));
    }

    /*
     * Init default vlan
     */

    /*
     * Set default VLAN
     */
    SHR_IF_ERR_EXIT(dnx_vlan_default_set_internal(unit, BCM_VLAN_DEFAULT, TRUE));
    /*
     * Init VLAN-PORT
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_default_init(unit));
    /*
     * Init Default ESEM entries
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_default_esem_init(unit));
    /*
     * Init VLAN range compression
     */
    SHR_IF_ERR_EXIT(dnx_vlan_range_template_init(unit));
    /*
     * Set not found destination to "drop" for unsupported VSI 0
     */
    SHR_IF_ERR_EXIT(dnx_vlan_set_unknown_da_drop(unit, BCM_VLAN_NONE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_vlan_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Get default vlan value from the sw state
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *    \param [in] vid_ptr -
 *     Pointer to receive default VID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   \retval The value of the returned default vlan from the sw state.
 * \remark
 *   * None
 *
 */
shr_error_e
bcm_dnx_vlan_default_get(
    int unit,
    bcm_vlan_t * vid_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_default_get_verify(unit, vid_ptr));

    SHR_IF_ERR_EXIT(vlan_db.vlan_info.default_vid.get(unit, vid_ptr));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Set default vlan value to the sw state, based on the incoming vid
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *    \param [in] vid -
 *     Relevant vid.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * Write to SW state the value of the default vlan
 * \remark
 *   * The function calls dnx_vlan_default_set_internal with FALSE parameter to indicate non init time.
 *
 */
shr_error_e
bcm_dnx_vlan_default_set(
    int unit,
    bcm_vlan_t vid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_default_set_verify(unit, vid));
    /*
     * Call dnx_vlan_default_set_internal with parameter FALSE to indicate non init time.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_default_set_internal(unit, vid, FALSE));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get miscellaneous port-specific VLAN options.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port number
 * \param [in] type - A value from the bcm_vlan_control_port_t enumerated list
 *  Used vlan control port type: bcmVlanPortIgnorePktTag -
 *      If the argument is non-zero, then it ignores the packet VLAN tag on the port.
 *      Treats the packet as untagged packet.
 * \param [out] arg - Value whose meaning is dependent on 'type'
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
bcm_dnx_vlan_control_port_get(
    int unit,
    int port,
    bcm_vlan_control_port_t type,
    int *arg)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_control_port_get_verify(unit, port, type, arg));
    switch (type)
    {
        case bcmVlanPortIgnorePktTag:
        {
            SHR_IF_ERR_EXIT(dnx_port_ignore_pkt_tag_get(unit, port, arg));
            break;
        }
        case bcmVlanTranslateIngressMissDrop:
        {
            uint32 hw_in_lif;
            uint32 drop_in_lif;

            /*
             * check that the port default-LIF is one of the initial default LIFs
             */
            SHR_IF_ERR_EXIT(dnx_vlan_control_port_set_get_miss_drop_verify(unit, port, &drop_in_lif, &hw_in_lif));

            /*
             * If the port default LIF is the drop-LIF, returns "drop Enabled" (i.e 1)
             * else return "drop Disabled" (i.e 0)
             */
            *arg = (hw_in_lif == drop_in_lif) ? 1 : 0;

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
 * \brief - Set miscellaneous port-specific VLAN options.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port number
 * \param [in] type - A value from the bcm_vlan_control_port_t enumerated list
 *  Used vlan control port type:
 *  bcmVlanPortIgnorePktTag -
 *      If the argument is non-zero, then it ignores the packet VLAN tag on the port.
 *      Treats the packet as untagged packet.
 *  bcmVlanTranslateIngressMissDrop - 
 *      Disable/Enable packet drop per this port (arg=0 disable drop, arg=non-zero enable drop)
 * \param [in] arg - Value whose meaning is dependent on 'type'
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
bcm_dnx_vlan_control_port_set(
    int unit,
    int port,
    bcm_vlan_control_port_t type,
    int arg)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_control_port_set_verify(unit, port, type, arg));

    switch (type)
    {
        case bcmVlanPortIgnorePktTag:
        {
            SHR_IF_ERR_EXIT(dnx_port_ignore_pkt_tag_set(unit, port, UTILEX_NUM2BOOL(arg)));
            break;
        }
        case bcmVlanTranslateIngressMissDrop:
        {
            uint32 hw_in_lif;
            uint32 drop_in_lif;

            /*
             * check that the port default-LIF is one of the initial default LIFs
             */
            SHR_IF_ERR_EXIT(dnx_vlan_control_port_set_get_miss_drop_verify(unit, port, &drop_in_lif, &hw_in_lif));

            if (arg == 0)
            {
                /*
                 * Disable drop for this port (using initial bridge LIF)
                 */
                int initial_bridge_in_lif;

                /*
                 * Get initial bridge in_lif (local in-lif)
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, &initial_bridge_in_lif));

                /*
                 * Set port default-LIF to initial bridge in_lif
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_control_port_default_lif_set(unit, port, initial_bridge_in_lif));
            }
            else
            {
                /*
                 * Enable drop for this port (using drop LIF)
                 */

                /*
                 * Set port default-LIF to drop-LIF
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_control_port_default_lif_set(unit, port, drop_in_lif));
            }

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type %d is not supported!\n", type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The motivation of the API is to be able to update {VLAN, Port} configuration asymmetric between Ingress and Egress. 
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vlan - VLAN ID (0-4K).
 * \param [in] port - gport, must by physical (i.e.gport to phy must expect local-port),
 *                    can be in-port or out-port or both (in case of symmetric).
 * \param [in] flags - the supported flags are the following:
 *                   * BCM_VLAN_GPORT_ADD_INGRESS_ONLY - Only Ingress functionality changes (i.e. working on port as in-port).
 *                   * BCM_VLAN_GPORT_ADD_EGRESS_ONLY - Only Egress functionality changes (i.e. working on port as out-port).
 *                   * BCM_VLAN_GPORT_ADD_MEMBER_REPLACE - change API attribute to opposite value (can be ingress or egress).
 *                   * BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE - do no update TX tag functionality (only egress functionality).
 *                   * BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE - Do not update VLAN membership.
 *                   * BCM_VLAN_PORT_UNTAGGED - indicates if the port should be added to untagged pbmp.
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   * The API supports asymmetric configuration of {VLAN, Port} between Ingress and Egress.
 *   * The functionality of the API is very similar to
 *     bcm_dnx_vlan_port_add, but provides better capabilities
 *     for handling asymmetric cases, and in addition enables
 *     trunk gport configuration that can't be performed using
 *     bcm_vlan_port_add().
 *   * For example, if customer wants to have VLAN 5 be member
 *     of In-port 3 but not member of Out-port 3,
 *   * then it would not be possible using bcm_vlan_port_add but possible to be done by bcm_vlan_gport_add, providing the correct flags.
 *   * Since the bcm_dnx_vlan_gport_add allows using the BCM_VLAN_GPORT_ADD_MEMBER_REPLACE flag, the API can also remove previously added port to the vlan membership.
 * \see
 *   * \ref bcm_dnx_vlan_port_add
 */
shr_error_e
bcm_dnx_vlan_gport_add(
    int unit,
    bcm_vlan_t vlan,
    bcm_gport_t port,
    int flags)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_pbmp_t pbmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t ubmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t tx_tag_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verify the input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_gport_add_verify(unit, vlan, port, flags));

    /*
     * Reset the bitmap arrays
     */
    sal_memset(pbmp_pp_arr, 0, sizeof(pbmp_pp_arr));
    sal_memset(ubmp_pp_arr, 0, sizeof(ubmp_pp_arr));

    /*
     * Retrieve the PP-Ports 
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Traverse the PP-Ports (for LAG) and update a PP-Ports bitmap for the participating ports 
     * and for the untagged ports 
     */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        BCM_PBMP_PORT_ADD(pbmp_pp_arr[gport_info.internal_port_pp_info.core_id[pp_port_index]],
                          gport_info.internal_port_pp_info.pp_port[pp_port_index]);

        if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_UNTAGGED))
        {
            BCM_PBMP_PORT_ADD(ubmp_pp_arr[gport_info.internal_port_pp_info.core_id[pp_port_index]],
                              gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        }
    }

    /*
     * Retrieve TX tag valid local pp pbmp, if the flag BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE is unset
     */
    if (!_SHR_IS_FLAG_SET(flags, BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_convert_pp_pbmp_to_tx_tag_valid_pbmp(unit, pbmp_pp_arr, tx_tag_pp_arr));
    }

    /*
     * Based on the incoming flags set/replace the ingress/egress vlan membership
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_add(unit, vlan, pbmp_pp_arr, ubmp_pp_arr, tx_tag_pp_arr, flags));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API removes ports from the VLAN port membership \n
 * The removal is done based on the incoming vlan and port \n
 *
 * \param [in] unit -   Relevant unit
 * \param [in] vlan -   VLAN ID (0-4K)
 * \param [in] port -   Relevant port to remove
 *
 * \return
 *                      Error indication according to shr_error_e enum
 * \remark
 *    The API is calling bcm_dnx_vlan_port_remove after adding the port to pbmp.
 *    For more information on the implementation and functionality see \ref bcm_dnx_vlan_port_remove
 * \see
 *   * \ref bcm_dnx_vlan_port_remove
 */
shr_error_e
bcm_dnx_vlan_gport_delete(
    int unit,
    bcm_vlan_t vlan,
    bcm_gport_t port)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int pp_port_index, core_id;
    bcm_pbmp_t pbmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    BCM_DNX_VLAN_CHK_ID(unit, vlan);

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_CLEAR(pbmp_pp_arr[core_id]);
    }

    /** Get PP Ports + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        BCM_PBMP_CLEAR(pbmp_pp_arr[gport_info.internal_port_pp_info.core_id[pp_port_index]]);
        BCM_PBMP_PORT_ADD(pbmp_pp_arr[gport_info.internal_port_pp_info.core_id[pp_port_index]],
                          gport_info.internal_port_pp_info.pp_port[pp_port_index]);
    }
    SHR_IF_ERR_EXIT(dnx_vlan_port_remove(unit, vlan, pbmp_pp_arr));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API removes all ports from port membership per given vlan identifier \n
 * The API supports asymmetric removal of ports.
 *
 * \param [in] unit -   Relevant unit
 * \param [in] vlan -   VLAN ID (0-4K)
 *
 * \return
 *                      Error indication according to shr_error_e enum
 * \remark
 *    The API is calling gets information about the ingress and egress vlan membership.
 *    Based on the added ports, retrieve the physical ones for each side - ingress and egress.
 *    then it converts it to logical bitmaps.
 *    For each of the bitmaps is called the API \ref bcm_dnx_vlan_port_remove, making sure all ports are removed for all sides.
 *
 * \see
 *   * \ref bcm_dnx_vlan_port_remove
 */
shr_error_e
bcm_dnx_vlan_gport_delete_all(
    int unit,
    bcm_vlan_t vlan)
{
    bcm_pbmp_t pbmp_ingress;
    bcm_pbmp_t pbmp_egress;
    bcm_pbmp_t vlan_mem_if_bmp_ingress;
    bcm_pbmp_t vlan_mem_if_bmp_egress;
    bcm_pbmp_t pbmp_core;
    bcm_pbmp_t pbmp_pp_arr_ingress[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t pbmp_pp_arr_egress[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    BCM_DNX_VLAN_CHK_ID(unit, vlan);
    BCM_PBMP_CLEAR(pbmp_ingress);
    BCM_PBMP_CLEAR(pbmp_egress);
    BCM_PBMP_CLEAR(vlan_mem_if_bmp_ingress);
    BCM_PBMP_CLEAR(vlan_mem_if_bmp_egress);

    /*
     * Retrieve information of the ingress and egress vlan membership if port bitmaps based on given vid
     */
    SHR_IF_ERR_EXIT(dnx_vlan_membership_get(unit, vlan, &vlan_mem_if_bmp_ingress, &vlan_mem_if_bmp_egress, 0));
    /*
     * Retrieve information of the physical ports, associated to the vlan memberships
     * The function will return array of physical pbmp per core and side.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_mem_if_bmp_to_pbmp_pp(unit, vlan_mem_if_bmp_ingress, pbmp_pp_arr_ingress));
    SHR_IF_ERR_EXIT(dnx_vlan_mem_if_bmp_to_pbmp_pp(unit, vlan_mem_if_bmp_egress, pbmp_pp_arr_egress));

    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_pbmp_to_logical_pbmp_get(unit, core, pbmp_pp_arr_ingress[core], &pbmp_core));
        BCM_PBMP_OR(pbmp_ingress, pbmp_core);
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_pbmp_to_logical_pbmp_get(unit, core, pbmp_pp_arr_egress[core], &pbmp_core));
        BCM_PBMP_OR(pbmp_egress, pbmp_core);
    }
    /*
     * Perform logical OR on ingress and egress bitmaps, to remove them at once. Remove the ports associated with both
     * ingress and egress sides. The below API will also remove the tx_tag valid (if configured) per port for both
     * memberships.
     */
    BCM_PBMP_OR(pbmp_ingress, pbmp_egress);
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_remove(unit, vlan, pbmp_ingress));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API gets information about the provided port for the given VLAN Identifier \n
 *
 * \param [in] unit -   Relevant unit
 * \param [in] vlan -   VLAN ID (0-4K)
 * \param [in] port -   Relevant port
 * \param [out] flags - Based on the port - retrieve the set flags.
 *                     * BCM_VLAN_GPORT_ADD_INGRESS_ONLY - if the port is part of the ingress membership.
 *                     * BCM_VLAN_GPORT_ADD_EGRESS_ONLY - if the port is part of the egress membership.
 *                     * BCM_VLAN_GPORT_ADD_UNTAGGED - if the port has enabled untagged functionality.
 * \return
 *                     * Error indication according to shr_error_e enum
 *                     * Error if the port is not added to membership
 * \remark
 * None
 * \see
 *   * \ref bcm_dnx_vlan_port_remove
 *   * \ref bcm_dnx_vlan_gport_delete
 */
shr_error_e
bcm_dnx_vlan_gport_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_gport_t port,
    int *flags)
{
    int core_id, pp_port_index;
    int ingress_member, egress_member;
    bcm_pbmp_t pbmp, pbmp_ingress, pbmp_egress, pbmp_ingress_get, pbmp_egress_get;
    uint32 pp_port_i, tx_tag_vid, tx_tag_valid;
    bcm_pbmp_t pbmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t pbmp_pp_arr_ingress[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t pbmp_pp_arr_egress[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t vlan_mem_if_bmp_ingress;
    bcm_pbmp_t vlan_mem_if_bmp_egress;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    BCM_DNX_VLAN_CHK_ID(unit, vlan);
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(pbmp_ingress);
    BCM_PBMP_CLEAR(pbmp_egress);
    BCM_PBMP_CLEAR(pbmp_ingress_get);
    BCM_PBMP_CLEAR(pbmp_egress_get);

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_CLEAR(pbmp_pp_arr[core_id]);
        BCM_PBMP_CLEAR(pbmp_pp_arr_ingress[core_id]);
        BCM_PBMP_CLEAR(pbmp_pp_arr_egress[core_id]);
    }

    ingress_member = FALSE;
    egress_member = FALSE;
    *flags = 0;

     /** Get PP Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Retrieve information of the vlan membership if port bitmap based on given vid
     */
    SHR_IF_ERR_EXIT(dnx_vlan_membership_get(unit, vlan, &vlan_mem_if_bmp_ingress, &vlan_mem_if_bmp_egress, 0));
    /*
     * Retrieve information of the physical ports, associated with the ingress and egress vlan memberships
     * The function will return array of physical pbmp per core.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_mem_if_bmp_to_pbmp_pp(unit, vlan_mem_if_bmp_ingress, pbmp_pp_arr_ingress));
    SHR_IF_ERR_EXIT(dnx_vlan_mem_if_bmp_to_pbmp_pp(unit, vlan_mem_if_bmp_egress, pbmp_pp_arr_egress));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_pbmp_to_logical_pbmp_get
                        (unit, core_id, pbmp_pp_arr_ingress[core_id], &pbmp_ingress_get));
        BCM_PBMP_OR(pbmp_ingress, pbmp_ingress_get);

        SHR_IF_ERR_EXIT(dnx_algo_port_pp_pbmp_to_logical_pbmp_get
                        (unit, core_id, pbmp_pp_arr_egress[core_id], &pbmp_egress_get));
        BCM_PBMP_OR(pbmp_egress, pbmp_egress_get);
    }

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_logical_get
                        (unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                         gport_info.internal_port_pp_info.pp_port[pp_port_index], &logical_port));
        if (BCM_PBMP_MEMBER(pbmp_ingress, logical_port))
        {
            ingress_member = TRUE;
        }
        if (BCM_PBMP_MEMBER(pbmp_egress, logical_port))
        {
            egress_member = TRUE;
        }
        BCM_PBMP_PORT_ADD(pbmp, logical_port);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_logical_pbmp_to_pp_pbmp_get(unit, pbmp, pbmp_pp_arr));
    /*
     * Retrieve information of the tx tag, set the flag BCM_VLAN_GPORT_ADD_UNTAGGED if the port is with tx_tag_valid
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_ITER(pbmp_pp_arr[core_id], pp_port_i)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_get(unit, core_id, pp_port_i, &tx_tag_vid, &tx_tag_valid));
            if ((vlan == tx_tag_vid) && tx_tag_valid == 1)
            {
                *flags |= BCM_VLAN_GPORT_ADD_UNTAGGED;
            }
        }
    }
    if (ingress_member && !egress_member)
    {
        *flags |= BCM_VLAN_GPORT_ADD_INGRESS_ONLY;
    }

    if (egress_member && !ingress_member)
    {
        *flags |= BCM_VLAN_GPORT_ADD_EGRESS_ONLY;
    }
    if (!ingress_member && !egress_member)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The port does not match to vlan");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API gets information about the provided vlan ID \n
 *
 * \param [in] unit         -   Relevant unit
 * \param [in] vlan         -   VLAN ID (0-4K)
 * \param [in] array_max    -  Max number of ports to be retrieved for this vlan
 * \param [out] gport_array - array of ports associated with the vlan
 * \param [out] is_untagged - indication per port, whether it's untagged
 * \param [out] array_size  - actual array size
 * \remark
 * None
 * \see
 *   * \ref bcm_dnx_vlan_port_get
 */
shr_error_e
bcm_dnx_vlan_gport_get_all(
    int unit,
    bcm_vlan_t vlan,
    int array_max,
    bcm_gport_t * gport_array,
    int *is_untagged,
    int *array_size)
{
    bcm_pbmp_t vlan_mem_if_bmp_ingress, vlan_mem_if_bmp_egress, pbmp_ingress_get,
        pbmp_egress_get, pbmp_ingress, pbmp_egress, logical_ports;
    bcm_pbmp_t pbmp_pp_arr_ingress[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    bcm_pbmp_t pbmp_pp_arr_egress[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];

    int port_count, port_i;
    uint32 pp_port, tx_tag_vid, tx_tag_valid;
    bcm_core_t core_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_gport_get_all_verify(unit, vlan, array_max, gport_array, is_untagged, array_size));
    BCM_PBMP_CLEAR(pbmp_ingress);
    BCM_PBMP_CLEAR(pbmp_egress);

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_CLEAR(pbmp_pp_arr_ingress[core_id]);
        BCM_PBMP_CLEAR(pbmp_pp_arr_egress[core_id]);
    }

    /*
     * Retrieve information of the vlan membership if port bitmap based on given vid
     */
    SHR_IF_ERR_EXIT(dnx_vlan_membership_get(unit, vlan, &vlan_mem_if_bmp_ingress, &vlan_mem_if_bmp_egress, 0));
    /*
     * Retrieve information of the physical ports, associated with the ingress and egress vlan memberships
     * The function will return array of physical pbmp per core.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_mem_if_bmp_to_pbmp_pp(unit, vlan_mem_if_bmp_ingress, pbmp_pp_arr_ingress));
    SHR_IF_ERR_EXIT(dnx_vlan_mem_if_bmp_to_pbmp_pp(unit, vlan_mem_if_bmp_egress, pbmp_pp_arr_egress));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_pbmp_to_logical_pbmp_get
                        (unit, core_id, pbmp_pp_arr_ingress[core_id], &pbmp_ingress_get));
        BCM_PBMP_OR(pbmp_ingress, pbmp_ingress_get);

        SHR_IF_ERR_EXIT(dnx_algo_port_pp_pbmp_to_logical_pbmp_get
                        (unit, core_id, pbmp_pp_arr_egress[core_id], &pbmp_egress_get));
        BCM_PBMP_OR(pbmp_egress, pbmp_egress_get);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_ports));

    port_count = 0;
    *array_size = 0;
    BCM_PBMP_ITER(logical_ports, port_i)
    {
        if (BCM_PBMP_MEMBER(pbmp_ingress, port_i) || (BCM_PBMP_MEMBER(pbmp_egress, port_i)))
        {
            if (*array_size > array_max)
            {
                break;
            }
            gport_array[port_count] = port_i;
            port_count ++;
            *array_size = *array_size + 1;
        }
    }
    /** Iterate over the gport_array and check if the port has a tx_tag_valid indication*/
    for (port_i = 0; port_i < *array_size; port_i ++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, gport_array[port_i], &core_id, &pp_port));
        SHR_IF_ERR_EXIT(dnx_vlan_tx_tag_get(unit, core_id, pp_port, &tx_tag_vid, &tx_tag_valid));
        if ((vlan == tx_tag_vid) && tx_tag_valid == 1)
        {
            is_untagged[port_i] = BCM_VLAN_GPORT_ADD_UNTAGGED;
        }
        else
        {
            is_untagged[port_i] = 0;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_vlan_list_internal
 * Purpose:
 *      Main body of bcm_dnx_vlan_list() and bcm_dnx_vlan_list_by_pbmp().
 *      Assumes locking already done.
 * Parameters:
 *      list_all - if TRUE, lists all ports and ignores list_pbmp.
 *      list_pbmp - if list_all is FALSE, lists only VLANs containing
 *              any of the ports in list_pbmp.
 */
STATIC int
dnx_vlan_list_internal(
    int unit,
    bcm_vlan_data_t ** listp,
    int *countp,
    int list_all,
    bcm_pbmp_t list_pbmp)
{
    bcm_vlan_data_t *list = NULL;
    int list_idx, rv;
    int idx;
    bcm_pbmp_t pbmp, ubmp, tbmp;

    SHR_FUNC_INIT_VARS(unit);
    *countp = 0;
    *listp = NULL;
    list_idx = 0;

    SHR_ALLOC(list, BCM_VLAN_COUNT * sizeof(list[0]), "vlan_list", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (list == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "unable to allocate memory for vlan list" " of %d entries", BCM_VLAN_COUNT);
    }
    /*
     * Iterate over all vlans
     */
    for (idx = (BCM_VLAN_MIN + 1); idx < BCM_VLAN_COUNT; idx++)
    {
        rv = bcm_dnx_vlan_port_get(unit, idx, &pbmp, &ubmp);
        if (BCM_FAILURE(rv))
        {
            SHR_FREE(list);
            SHR_IF_ERR_EXIT(rv);
        }

        BCM_PBMP_ASSIGN(tbmp, list_pbmp);
        BCM_PBMP_AND(tbmp, pbmp);
        if (list_all || BCM_PBMP_NOT_NULL(pbmp) || BCM_PBMP_NOT_NULL(ubmp))
        {
            list[list_idx].vlan_tag = idx;
            BCM_PBMP_ASSIGN(list[list_idx].port_bitmap, pbmp);
            BCM_PBMP_ASSIGN(list[list_idx].ut_port_bitmap, ubmp);
            list_idx++;

        }
    }

    *countp = list_idx;
    *listp = list;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API gets information about the all VLANs\n
 *
 * \param [in] unit - Relevant unit
 * \param [out] listp - List to store all vlans info.
 * \param [out] countp -The total number of vlan.
 * \return
 *        * Error indication according to shr_error_e enum
 * \remark
 * None
 * \see
 *  * \ref dnx_vlan_list_internal
 */
shr_error_e
bcm_dnx_vlan_list(
    int unit,
    bcm_vlan_data_t ** listp,
    int *countp)
{
    bcm_pbmp_t empty_pbm;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_list_verify(unit, listp, countp));

    BCM_PBMP_CLEAR(empty_pbm);
    SHR_IF_ERR_EXIT(dnx_vlan_list_internal(unit, listp, countp, FALSE, empty_pbm));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * This API free the resource allocated by bcm_dnx_vlan_list s\n
 *
 * \param [in] unit - Relevant unit
 * \param [in] list - List to store all vlans info.
 * \param [in] count - The total number of vlan.
 * \return
 *  * Error indication according to shr_error_e enum
 * \remark
 * None
 */
shr_error_e
bcm_dnx_vlan_list_destroy(
    int unit,
    bcm_vlan_data_t * list,
    int count)
{
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_REFERENCE(count);

    SHR_FREE(list);

    SHR_FUNC_EXIT;
}

shr_error_e
bcm_dnx_vlan_gport_info_get(
    int unit,
    bcm_vlan_gport_info_t * vlan_gport_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is not supported for JR2 device, please read the backward compatibility document\n");
    SHR_EXIT ();
exit:
    SHR_FUNC_EXIT;
}
