/** \file port_pp.c
 * $Id$
 *
 * PP Port procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/stg/stg.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port_tpid.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/common/multicast.h>
#include <soc/dnx/swstate/auto_generated/access/algo_port_pp_access.h>
#include <src/bcm/dnx/init/init_pp.h>
#include <src/bcm/dnx/vxlan/vxlan.h>
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
/** Check if the value of vlan_domain is valid  */
#define DNX_VLAN_DOMAIN_VALID(_unit, _vlan_domain) (_vlan_domain < dnx_data_l2.vlan_domain.nof_vlan_domains_get(_unit))
/** Check if dp profile is valid  */
#define DNX_DP_PROFILE_VALID(_unit, _profile) (_profile < dnx_data_qos.qos.nof_ingress_policer_profiles_get(unit))
/** Check if the value of vlan_membership_if is valid  */
#define DNX_VLAN_MEMBERSHIP_IF_VALID(_unit, _vlan_membership_if) ((_vlan_membership_if >= 0) && (_vlan_membership_if < dnx_data_port.general.nof_vlan_membership_if_get(_unit)))
/** ITMH A1 parsing context msbs */
#define ITMH_A1_PARSING_CONTEXT_MSBS 2
/*
 * }
 */

/*
 * Local data structures
 * {
 */

/**
 * \brief
 *  Egress PP Port result info
 */
typedef struct
{
    /*
     * Trap context port profile
     */
    int trap_context_port_profile;
    /*
     * Enabler for egress PP filters
     */
    int egress_pp_filters_enable;
    /*
     * Enabler for egress TM filters
     */
    int egress_tm_filters_enable;
    /*
     * default port ESEM command
     */
    int default_esem_cmd;
    /*
     * Port profile
     */
    dbal_enum_value_field_egress_fwd_code_port_profile_e fwd_code_port_profile;
    /*
     * PRP port profile
     */
    dbal_enum_value_field_etpp_prp2_port_cs_var_e prp2_port_profile;
    /*
     * Termination port profile
     */
    dbal_enum_value_field_etpp_termination_port_cs_var_e term_port_profile;
} dnx_port_pp_egress_port_info_t;

/*
 * }
 */

/*
 * Declaration of extern functions
 * {
 */
extern shr_error_e dbal_entry_handle_update_field_ids(
    int unit,
    uint32 entry_handle_id);
/*
 * }
 */
/**
 * \brief
 * Verify vid parameter for BCM-API: bcm_dnx_port_untagged_vlan_set().
 */
static int
dnx_port_untagged_vlan_set_verify(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check vid < 4K
     */
    BCM_DNX_VLAN_CHK_ID(unit, vid);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function for dnx_port_pp_vlan_domain_set()
 */
static shr_error_e
dnx_port_pp_vlan_domain_set_verify(
    int unit,
    bcm_port_t port,
    uint32 vlan_domain)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if the vlan_domain is valid */
    if (!DNX_VLAN_DOMAIN_VALID(unit, vlan_domain))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The vlan_domain %d is not valid!\n", vlan_domain);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_pp_vlan_domain_set(
    int unit,
    bcm_port_t port,
    uint32 vlan_domain)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_pp_vlan_domain_set_verify(unit, port, vlan_domain));

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Write to INGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        /** Set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        /** Set vlan_domain */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Write to EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_PP_PORT, entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        /** Set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        /** Set vlan_domain*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify Port
 *   parameter for BCM-API: bcm_dnx_port_class_get()
 */
static shr_error_e
dnx_port_pp_vlan_membership_if_get_verify(
    int unit,
    bcm_core_t internal_core_id,
    bcm_port_t internal_port_pp,
    uint32 *vlan_mem_if)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_mem_if, _SHR_E_PARAM, "vlan_mem_if");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the VLAN Membership-IF from internal PP port.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] internal_core_id - Internal Core id
 *   \param [in] internal_port_pp - Internal PP port
 *   \param [out] vlan_mem_if - Vlan membership interface
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  We assume that VLAN-membership-IF mapping from local port is
 *  symmetric.Because of that, it is enough to get the
 *  information from Ingress Port table.
 * \see
 *   * None
 */
shr_error_e
dnx_port_pp_vlan_membership_if_get(
    int unit,
    bcm_core_t internal_core_id,
    bcm_port_t internal_port_pp,
    uint32 *vlan_mem_if)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_pp_vlan_membership_if_get_verify
                          (unit, internal_core_id, internal_port_pp, vlan_mem_if));

    /*
     * Reading from Ingress PORT_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, internal_port_pp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, internal_core_id);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_IF, INST_SINGLE, vlan_mem_if);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_pp_vlan_membership_if_set
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] vlan_mem_if - VLAN Membership-IF
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
dnx_port_pp_vlan_membership_if_set_verify(
    int unit,
    bcm_port_t port,
    uint32 vlan_mem_if)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if vlan_mem_if is valid */
    if (!DNX_VLAN_MEMBERSHIP_IF_VALID(unit, vlan_mem_if))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vlan_membership_if %d is not valid! It should be value between 0 and %d\n",
                     vlan_mem_if, (dnx_data_port.general.nof_vlan_membership_if_get(unit) - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
int
dnx_port_pp_vlan_membership_if_set(
    int unit,
    bcm_port_t port,
    uint32 vlan_mem_if)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_pp_vlan_membership_if_set_verify(unit, port, vlan_mem_if));

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Write to Ingress PORT_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_IF, INST_SINGLE, vlan_mem_if);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Write to Egress PORT_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_PP_PORT, entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_IF, INST_SINGLE, vlan_mem_if);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_pp_vlan_domain_get
 */
static shr_error_e
dnx_port_pp_vlan_domain_get_verify(
    int unit,
    bcm_port_t port,
    uint32 *vlan_domain)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_domain, _SHR_E_PARAM, "vlan_domain");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get egress VLAN domain
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] vlan_domain - Vlan domain
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
dnx_port_pp_vlan_domain_get(
    int unit,
    bcm_port_t port,
    uint32 *vlan_domain)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_pp_vlan_domain_get_verify(unit, port, vlan_domain));

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Read EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);

    /*
     * getting the entry with the default values
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set DP profile for IVE on IN LIF.
 */
shr_error_e
dnx_port_pp_ive_dp_profile_set(
    int unit,
    bcm_port_t port,
    qos_dp_profile_application_type_e type,
    int map_id)
{
    int old_in_lif_profile = 0, new_in_lif_profile = 0;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;
    uint32 flags;
    uint32 profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile = DNX_QOS_MAP_PROFILE_GET(map_id);
    if (!DNX_QOS_MAP_IS_POLICER(map_id) || !DNX_DP_PROFILE_VALID(unit, profile))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The DP profile %d is not valid!\n", map_id);
    }
    if (((type == QOS_DP_PROFILE_OUTER) && (profile >= IVE_OUTER_QOS_DP_PROFILE_NOF_VALUES)) ||
        ((type == QOS_DP_PROFILE_INNER) && profile >= IVE_INNER_QOS_DP_PROFILE_NOF_VALUES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The DP profile %d is bigger than outer %d or inner %d!\n", profile,
                     IVE_OUTER_QOS_DP_PROFILE_NOF_VALUES, IVE_INNER_QOS_DP_PROFILE_NOF_VALUES);
    }
    /*
     * Get the old in_lif_profile.
     */
    flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port, flags, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                                        (uint32 *) &old_in_lif_profile));

    /*
     * Exchange for new in_lif_profile.
     */
    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));

    if (type == QOS_DP_PROFILE_OUTER)
    {
        in_lif_profile_info.egress_fields.ive_outer_qos_dp_profile = profile;
    }
    else
    {
        in_lif_profile_info.egress_fields.ive_inner_qos_dp_profile = profile;
    }

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     gport_hw_resources.inlif_dbal_table_id));

    /*
     * Update inLIF table with the new in_lif_profile
     */
    if (old_in_lif_profile != new_in_lif_profile)
    {
        /** Update the LIF table with the new in_lif_profile*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.inlif_dbal_table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, new_in_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get DP profile for IVE on IN LIF.
 */
shr_error_e
dnx_port_pp_ive_dp_profile_get(
    int unit,
    bcm_port_t port,
    qos_dp_profile_application_type_e type,
    int *map_id)
{
    int in_lif_profile;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;
    uint32 flags;
    uint32 profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(map_id, _SHR_E_PARAM, "ive dp profile");

    /** get local_in_lif */
    flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port, flags, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /** get in_lif_profile from LIF table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &in_lif_profile));

    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));

    if (type == QOS_DP_PROFILE_OUTER)
    {
        profile = in_lif_profile_info.egress_fields.ive_outer_qos_dp_profile;
    }
    else
    {
        profile = in_lif_profile_info.egress_fields.ive_inner_qos_dp_profile;
    }

    *map_id = profile;
    /** set map policer */
    DNX_QOS_POLICER_MAP_SET(*map_id);
    /** set map direction*/
    DNX_QOS_INGRESS_MAP_SET(*map_id);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function of bcm_dnx_port_untagged_vlan_get API
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - A gport indicating physical port
 * \param [out] vid - VLAN Identifier
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_untagged_vlan_get_verify(
    int unit,
    bcm_port_t port,
    bcm_vlan_t * vid)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify vid is not NULL
     */
    SHR_NULL_CHECK(vid, _SHR_E_PARAM, "vid should be a valid pointer!");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Port default vlan
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - A gport indicating physical port
 * \param [out] vid - VLAN Identifier
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
bcm_dnx_port_untagged_vlan_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_t * vid)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_untagged_vlan_get_verify(unit, port, vid));

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Get from INGRESS_PP_PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, vid);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/** See .h */
shr_error_e
dnx_port_untagged_vlan_set_internal(
    int unit,
    bcm_port_t pp_port,
    bcm_core_t core_id,
    bcm_vlan_t vid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to INGRESS_PP_PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, vid);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - This API determines VLANs according to Port-VID (PVID) for untag cases.
 *          Update HW Ingress PP Port table
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - A gport indicating physical port
 * \param [in] vid - VLAN Identifier
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
bcm_dnx_port_untagged_vlan_set(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_untagged_vlan_set_verify(unit, port, vid));

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Write to INGRESS_PP_PORT table
     */

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_port_untagged_vlan_set_internal
                        (unit, gport_info.internal_port_pp_info.pp_port[pp_port_index],
                         gport_info.internal_port_pp_info.core_id[pp_port_index], vid));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_pp_port_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_sit_init(unit));

    /*
     * Allocate and init the default esem resources.
     */
    SHR_IF_ERR_EXIT(dnx_port_esem_default_resource_init(unit));

    /*
     * Init LLVP
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_default_init(unit));

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_pp_port_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
dnx_port_initial_eth_properties_set(
    int unit,
    bcm_port_t port,
    uint32 header_type)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 entry_handle_id;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    switch (header_type)
    {
        case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
        case BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW:
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP:
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE:
        {
            int initial_default_in_lif;
            dbal_enum_value_field_irpp_1st_parser_parser_context_e parser_context =
                DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ETH_A1;

            if (header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE)
            {
                parser_context = DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ITMH_JR1_A1;
            }

            /*
             * Allocate Ingress LLVP profile per port
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_default_per_port_init(unit, port));

            /*
             * Configure the INGRESS_LLR_CONTEXT_PROPERTIES table:
             * Enable LLVP and Initial VID for Bridge Context ID per port.
             */

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_LLR_CONTEXT_PROPERTIES, &entry_handle_id));
            for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT,
                                           gport_info.internal_port_pp_info.pp_port[pp_port_index]);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                           gport_info.internal_port_pp_info.core_id[pp_port_index]);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_ENABLE, INST_SINGLE, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_VID_ENABLE, INST_SINGLE, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLR_CONTEXT_ID, INST_SINGLE,
                                             DBAL_ENUM_FVAL_LLR_CONTEXT_ID_BRIDGE);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_SEL_ENABLE, INST_SINGLE, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLR_PTC_PROFILE_ENABLE, INST_SINGLE, 1);
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FILTER_SA_EQ_DA_ENABLE, INST_SINGLE, 1);
                /*
                 * Enable acceptable frame type filter
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ENABLE,
                                             INST_SINGLE, TRUE);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            /*
             * Configure the INGRESS_PP_PORT table:
             *  1. Enable learn-lif.
             *  2. Set default In-LIF
             */

            /*
             * Get initial default_in_lif (local in-lif)
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_default_lif_get(unit, &initial_default_in_lif));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_PP_PORT, entry_handle_id));
            for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                           gport_info.internal_port_pp_info.pp_port[pp_port_index]);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                           gport_info.internal_port_pp_info.core_id[pp_port_index]);
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LEARN_LIF_ENABLE, INST_SINGLE, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE,
                                             initial_default_in_lif);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT,
                                             INST_SINGLE, parser_context);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_SAME_INTERFACE_FILTER_PROFILE,
                                             INST_SINGLE, DBAL_ENUM_FVAL_IN_PORT_SAME_INTERFACE_FILTER_PROFILE_ENABLE);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            break;
        }
        default:
        {
            /*
             * Configure the INGRESS_LLR_CONTEXT_PROPERTIES table:
             * Disable LLVP and Initial VID per port.
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_LLR_CONTEXT_PROPERTIES, &entry_handle_id));
            for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT,
                                           gport_info.internal_port_pp_info.pp_port[pp_port_index]);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                           gport_info.internal_port_pp_info.core_id[pp_port_index]);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_ENABLE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_VID_ENABLE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLR_CONTEXT_ID, INST_SINGLE,
                                             DBAL_ENUM_FVAL_LLR_CONTEXT_ID_GENERAL);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_SEL_ENABLE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLR_PTC_PROFILE_ENABLE, INST_SINGLE, 0);
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FILTER_SA_EQ_DA_ENABLE, INST_SINGLE, 0);
                /*
                 * Disable acceptable frame type filter
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ENABLE,
                                             INST_SINGLE, 0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            /*
             * Configure the INGRESS_PP_PORT table: disable learn-lif.
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_PP_PORT, entry_handle_id));
            for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                           gport_info.internal_port_pp_info.pp_port[pp_port_index]);
                dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                          gport_info.internal_port_pp_info.core_id[pp_port_index]);
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LEARN_LIF_ENABLE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT,
                                             INST_SINGLE, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ITMH_A1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_SAME_INTERFACE_FILTER_PROFILE,
                                             INST_SINGLE, DBAL_ENUM_FVAL_IN_PORT_SAME_INTERFACE_FILTER_PROFILE_DISABLE);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Provide mapping between source system port and gport
 *
 * \param [in] unit - Unit id
 * \param [in] core_id - In core id
 * \param [in] pp_port - In pp port
 * \param [in] src_system_port - Source system port
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
dnx_port_pp_src_system_port_set(
    int unit,
    bcm_core_t core_id,
    uint32 pp_port,
    uint32 src_system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure SOURCE_SYSTEM_PORT in dbal table INGRESS_PP_PORT
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE, src_system_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the spanning tree state for a port.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Port - physical port or logical port
 * \param [in] state - State to place port in, one of BCM_PORT_STP_xxx.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   All STGs containing all VLANs containing the given port are updated.
 *
 * \see
 *   dnx_port_stp_get
 *
 */
bcm_error_t
bcm_dnx_port_stp_set(
    int unit,
    bcm_port_t port,
    int state)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_stp_set(unit, port, state));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the spanning tree state for a port in the default STG.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Port - physical port or logical port
 * \param [out] state - Pointer where state stored.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   dnx_port_stp_get
 *
 */
bcm_error_t
bcm_dnx_port_stp_get(
    int unit,
    bcm_port_t port,
    int *state)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_stp_get(unit, port, state));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API bcm_dnx_port_vlan_member_set
 */
static shr_error_e
dnx_port_vlan_member_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The possible flags that can be set are: 0, BCM_PORT_VLAN_MEMBER_INGRESS, BCM_PORT_VLAN_MEMBER_EGRESS
     * or both BCM_PORT_VLAN_MEMBER_INGRESS and BCM_PORT_VLAN_MEMBER_EGRESS
     */
    if ((flags & (~(BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS))) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The flag %d that is provided to dnx_port_vlan_member_set is not supported!",
                     flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set current behavior of tagged packets arriving/leaving on a port not a member of the specified VLAN.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port number to operate on
 * \param [in] flags - Flags to indicate port filtering mode
 *  0 - Disable port filtering.
 *  BCM_PORT_VLAN_MEMBER_INGRESS - Indicate ingress port filtering.
 *  BCM_PORT_VLAN_MEMBER_EGRESS - Indicate egress port filtering.
 *  BCM_PORT_VLAN_MEMBER_INGRESS|BCM_PORT_VLAN_MEMBER_EGRESS - Indicate both ingress and egress port filtering.
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
bcm_dnx_port_vlan_member_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint32 vlan_mem_if;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_vlan_member_set_verify(unit, port, flags));

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Get VLAN Domain */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_port_pp_vlan_membership_if_get
                        (unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                         gport_info.internal_port_pp_info.pp_port[pp_port_index], &vlan_mem_if));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_DOMAIN_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_mem_if);

        if (_SHR_IS_FLAG_SET(flags, BCM_PORT_VLAN_MEMBER_INGRESS))
        {
            /*
             * In case BCM_PORT_VLAN_MEMBER_INGRESS flag is set
             * All arriving/leaving packets on a port with a VLAN tag identifying a VLAN
             * of which the port is not a member will be dropped.
             * The VLAN membership check is enabled.
             */

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_ENABLE, INST_SINGLE, 1);
        }
        else
        {
            /*
             * In case no flag is set or BCM_PORT_VLAN_MEMBER_EGRESS is set, ingress VLAN membership check is disabled.
             */

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_ENABLE, INST_SINGLE, 0);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_PP_PORT, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        if (_SHR_IS_FLAG_SET(flags, BCM_PORT_VLAN_MEMBER_EGRESS))
        {
            /*
             * In case BCM_PORT_VLAN_MEMBER_EGRESS flag is set
             * All arriving/leaving packets on a port with a VLAN tag identifying a VLAN
             * of which the port is not a member will be dropped.
             * The VLAN membership check is enabled.
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_ENABLE, INST_SINGLE, 1);

        }
        else
        {
            /*
             * In case no flag is set or BCM_PORT_VLAN_MEMBER_INGRESS, egress VLAN membership check is disabled.
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_ENABLE, INST_SINGLE, 0);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API bcm_dnx_port_vlan_member_get
 */
static shr_error_e
dnx_port_vlan_member_get_verify(
    int unit,
    bcm_port_t port,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Return filter mode for a port.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port number to operate on
 * \param [out] flags - Filter mode
 *  BCM_PORT_VLAN_MEMBER_INGRESS - Indicate ingress port filtering.
 *  BCM_PORT_VLAN_MEMBER_EGRESS - Indicate egress port filtering.
 *  0 - No port filtering.
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
bcm_dnx_port_vlan_member_get(
    int unit,
    bcm_port_t port,
    uint32 *flags)
{
    uint32 vlan_mem_if;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 vlan_membership_enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_vlan_member_get_verify(unit, port, flags));

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    *flags = 0;
    vlan_membership_enable = 0;

    /** Get VLAN Domain */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_membership_if_get
                    (unit, gport_info.internal_port_pp_info.core_id[0], gport_info.internal_port_pp_info.pp_port[0],
                     &vlan_mem_if));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_DOMAIN_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_mem_if);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_ENABLE, INST_SINGLE,
                               &vlan_membership_enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** If the requested value is 1, this means that BCM_PORT_VLAN_MEMBER_INGRESS is set */
    if (vlan_membership_enable == 1)
    {
        *flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_PP_PORT, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_MEMBERSHIP_ENABLE, INST_SINGLE,
                               &vlan_membership_enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** If the requested value is 1, this means that BCM_PORT_VLAN_MEMBER_EGRESS is set */
    if (vlan_membership_enable == 1)
    {
        *flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify the inputs to BCM-API: bcm_dnx_port_learn_set()
 *
 * \param [in] unit - Relevant unit
 * \param [in] is_physical_port - Whether the port is physical port or not
 * \param [in] flags - Flags for Learn mode
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_learn_set_verify(
    int unit,
    uint8 is_physical_port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the flags available*/
    if ((flags & (~(BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD))) != 0)
    {
        SHR_ERR_EXIT(BCM_E_UNAVAIL, "0x%X: unsupported learn flags!\n", flags);
    }

    if (is_physical_port == FALSE)
    {
        /** Verify the flags for virtual port*/
        if (!_SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_FWD))
        {
            SHR_ERR_EXIT(BCM_E_UNAVAIL, "0x%X: uncomplete learn flags, FWD must present for LIF learn control\n",
                         flags);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify the inputs to BCM-API: bcm_dnx_port_learn_get()
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Pointer for Flags for Learn mode
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_learn_get_verify(
    int unit,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the flags is not NULL*/
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "Flags should be a valid pointer!");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify the inputs to BCM-API: bcm_dnx_port_learn_modify()
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - The port input
 * \param [in] is_physical_port - Whether the port is physical port or not
 * \param [in] add_flags - Flags for Learn mode to add
 * \param [in] remove_flags - Flags for Learn mode to remove
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_learn_modify_verify(
    int unit,
    bcm_port_t port,
    uint8 is_physical_port,
    uint32 add_flags,
    uint32 remove_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the flags is available*/
    if ((add_flags & (~(BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD))) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "0x%X: unsupported learn flags. Only BCM_PORT_LEARN_ARL and BCM_PORT_LEARN_FWD are supported!\n",
                     add_flags);
    }

    if ((remove_flags & (~(BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD))) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "0x%X: unsupported learn flags. Only BCM_PORT_LEARN_ARL and BCM_PORT_LEARN_FWD are supported!!\n",
                     remove_flags);
    }

    if ((!_SHR_IS_FLAG_SET(add_flags, BCM_PORT_LEARN_FWD)) &&
        (!_SHR_IS_FLAG_SET(add_flags, BCM_PORT_LEARN_ARL)) &&
        (!_SHR_IS_FLAG_SET(remove_flags, BCM_PORT_LEARN_FWD)) && (!_SHR_IS_FLAG_SET(remove_flags, BCM_PORT_LEARN_ARL)))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Flags (0x%X) indicate no action is required!\n", add_flags | remove_flags);
    }

    if (is_physical_port == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "0x%X: API works for physical ports only!\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Translate the learn flags to trap-id
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Flags for Learn mode
 * \param [out] trap_id - Ingress Trap ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_port_learn_flags_from_trap_id
 */
static shr_error_e
dnx_port_learn_flags_to_trap_id(
    int unit,
    uint32 flags,
    int *trap_id)
{
    int trap_code;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(trap_id, _SHR_E_INTERNAL, "A valid pointer is needed!");

    /*
     * Translate the flags to trap code
     */
    trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND0;
    if ((_SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_FWD)) && (_SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_ARL)))
    {
        /** learn and forward */
        trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND0;
    }
    else if (!_SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD))
    {
        /** neither: drop and don't learn*/
        trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND3;
    }
    else if (!_SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_FWD))
    {
        /** only learn */
        trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND2;
    }
    else if (!_SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_ARL))
    {
        /** only forward */
        trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND1;
    }

    *trap_id = trap_code;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Translate the learn flags from trap-id
 *
 * \param [in] unit - Relevant unit
 * \param [in] trap_id - Ingress Trap ID
 * \param [out] flags - Flags for Learn mode
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_port_learn_flags_to_trap_id
 */
static shr_error_e
dnx_port_learn_flags_from_trap_id(
    int unit,
    int trap_id,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "A valid pointer is needed!");

    /*
     * Translate the flags from trap code
     */
    switch (trap_id)
    {
        case DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND0:
        {
            /** learn and forward */
            *flags = BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_ARL;
            break;
        }
        case DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND1:
        {
            /** only forward */
            *flags = BCM_PORT_LEARN_FWD;
            break;
        }
        case DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND2:
        {
            /** only learn */
            *flags = BCM_PORT_LEARN_ARL;
            break;
        }
        case DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND3:
        {
            /** neither: drop and don't learn*/
            *flags = 0;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Trap code (%d) is not expected!", trap_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set the learning mode based on the flags parameter to a virtual port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port refer to a lif in gport format.
 * \param [in] flags - Flags for learning mode. Possible values:
 *                     BCM_PORT_LEARN_ARL
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_lif_learn_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags)
{
    uint32 entry_handle_id;
    int learn_enable;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get hw resources related the gport
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport,
                                                                   DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                                   &gport_hw_resources), _SHR_E_NOT_FOUND,
                                _SHR_E_PARAM);

    learn_enable = _SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_ARL) ? TRUE : FALSE;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, learn_enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 *  Get the learning mode from a logical port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port refer to a lif in gport format.
 * \param [out] flags - Flags for learning mode. Possible values:
 *                      BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_lif_learn_set
 */
static shr_error_e
dnx_port_lif_learn_get(
    int unit,
    bcm_gport_t gport,
    uint32 *flags)
{
    uint32 entry_handle_id;
    uint32 gport_flags;
    uint8 learn_enable;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get hw resources related the gport
     */
    gport_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, gport_flags, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** User should ensure that the LIF in gport format can be used for learnning*/
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, &learn_enable));

    /*
     * Assign the flags accordingly.
     */
    *flags = BCM_PORT_LEARN_FWD;
    if (learn_enable != FALSE)
    {
        *flags |= BCM_PORT_LEARN_ARL;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 *  Set the learning mode based on the flags parameter to a physical port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] core_id - Core ID the port resides in.
 * \param [in] pp_port - Physical port ID in the core.
 * \param [in] flags - Flags for learning mode. Possible values:
 *                     BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_lif_learn_set
 */
shr_error_e
dnx_port_pp_learn_set(
    int unit,
    int core_id,
    int pp_port,
    uint32 flags)
{
    uint32 entry_handle_id;
    int sa_not_found_action_profile, learn_enable;
    int trap_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Decode learn-enabler and sa_not_found_action_profile from the flags*/
    SHR_IF_ERR_EXIT(dnx_port_learn_flags_to_trap_id(unit, flags, &trap_id));
    sa_not_found_action_profile = trap_id - DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND0;
    learn_enable = _SHR_IS_FLAG_SET(flags, BCM_PORT_LEARN_ARL) ? TRUE : FALSE;

    /** Set learn mode to the port */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, learn_enable);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_SA_NOT_FOUND_ACTION_PROFILE, INST_SINGLE,
                                sa_not_found_action_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 *  Get the learning mode from a physical port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] core_id - Core ID the port resides in.
 * \param [in] pp_port - Physical port ID in the core.
 * \param [out] flags - Flags for learning mode. Possible values:
 *                      BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_lif_learn_set
 */
static shr_error_e
dnx_port_pp_learn_get(
    int unit,
    int core_id,
    int pp_port,
    uint32 *flags)
{
    uint32 entry_handle_id;
    uint8 sa_not_found_action_profile;
    int trap_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set learn mode to the port */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SA_NOT_FOUND_ACTION_PROFILE, INST_SINGLE,
                              &sa_not_found_action_profile);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Calculate the learn flags */
    trap_id = sa_not_found_action_profile + DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND0;
    SHR_IF_ERR_EXIT(dnx_port_learn_flags_from_trap_id(unit, trap_id, flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 *  Set the learning mode based on the flags parameter to a port.
 *  In case the port is physical port, the trap code will be updated according to the learning mode.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - local port, or logical port in gport format.
 * \param [in] flags - Flags for learning mode. Possible values:
 *                     BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   ARL and FWD is supported on physical port and Only ARL is supported on logical port.
 *       Flags
 *   ARL      FWD     |     ACTION            | TRAP CODE
 *    V        V      |  learn & Forward      | bcmRxTrapL2Learn0
 *    X        V      |  don't learn & Forward| bcmRxTrapL2Learn1
 *    V        X      |  learn & Drop         | bcmRxTrapL2Learn2
 *    X        X      |  don't learn & Drop   | bcmRxTrapL2Learn3
 *
 * \see
 *   bcm_dnx_port_learn_get
 */
bcm_error_t
bcm_dnx_port_learn_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint8 is_physical_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));

    /** Get physical port info */
    if (is_physical_port != FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                        &gport_info));

    }

    /*
     * Verification the inputs
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_learn_set_verify(unit, is_physical_port, flags));

    /*
     * Set the attributes to the entry
     */
    if (is_physical_port == FALSE)
    {
        /** Procedure for virtual port */
        SHR_IF_ERR_EXIT(dnx_port_lif_learn_set(unit, port, flags));
    }
    else
    {
        /** Procedure for physical port */
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_learn_set
                            (unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                             gport_info.internal_port_pp_info.pp_port[pp_port_index], flags));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the learning mode from a port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, or logical port in gport format.
 * \param [out] flags - Flags for learning mode. Possible values:
 *                      BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_learn_set
 */
bcm_error_t
bcm_dnx_port_learn_get(
    int unit,
    bcm_port_t port,
    uint32 *flags)
{
    uint8 is_physical_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));

    /** Get physical port info */
    if (is_physical_port != FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                        &gport_info));
    }

    /*
     * Verification the inputs
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_learn_get_verify(unit, flags));

    /*
     * Set the atrributes to the entry
     */
    if (is_physical_port == FALSE)
    {
        /** Procedure for virtual port */
        SHR_IF_ERR_EXIT(dnx_port_lif_learn_get(unit, port, flags));
    }
    else
    {
        /** Procedure for physical port */
        SHR_IF_ERR_EXIT(dnx_port_pp_learn_get
                        (unit, gport_info.internal_port_pp_info.core_id[0], gport_info.internal_port_pp_info.pp_port[0],
                         flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Modify the learning mode for a port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port.
 * \param [in] add - Flags for learning mode to be added. Possible values:
 *                      BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 * \param [in] remove - Flags for learning mode to be removed. Possible values:
 *                      BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   This API works for physical port only, since only ARL is changable for logical
 *   port. If it is required to enable or disable learn for logical port (LIF.),
 *   bcm_port_learn_set should be called.
 *
 * \see
 *   bcm_dnx_port_learn_get
 *   bcm_dnx_port_learn_set
 */
bcm_error_t
bcm_dnx_port_learn_modify(
    int unit,
    bcm_port_t port,
    uint32 add,
    uint32 remove)
{
    uint8 is_physical_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    uint32 learn_flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));

    /*
     * Verification the inputs
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_learn_modify_verify(unit, port, is_physical_port, add, remove));

    /** Get physical port info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Update the learn mode for the port */
    learn_flags = 0;

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_port_pp_learn_get
                        (unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                         gport_info.internal_port_pp_info.pp_port[pp_port_index], &learn_flags));

        learn_flags |= add;
        learn_flags &= ~remove;

        SHR_IF_ERR_EXIT(dnx_port_pp_learn_set
                        (unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                         gport_info.internal_port_pp_info.pp_port[pp_port_index], learn_flags));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  See port_pp.h for more information.
 */
shr_error_e
dnx_port_pp_l2_unknown_filter_set(
    int unit,
    bcm_port_t port,
    int filter_flags)
{
    uint32 entry_handle_id;
    uint8 unknown_bc_filter, unknown_mc_filter, unknown_uc_filter;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Analyze the filter flags
     */
    if (filter_flags == BCM_PORT_CONTROL_FILTER_DISABLE_ALL)
    {
        /** Disable all filters */
        unknown_bc_filter = unknown_mc_filter = unknown_uc_filter = FALSE;
    }
    else
    {
        /** Verify the filter flags*/
        if (filter_flags &
            ~(BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC | BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC |
              BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "value: 0x%X, Invalid filter flags!\n", filter_flags);
        }

        /** Enable all filters by default*/
        unknown_bc_filter = unknown_mc_filter = unknown_uc_filter = TRUE;

        /** Remove filter enabler per indication */
        if (filter_flags & BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC)
        {
            unknown_bc_filter = FALSE;
        }
        if (filter_flags & BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC)
        {
            unknown_mc_filter = FALSE;
        }
        if (filter_flags & BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC)
        {
            unknown_uc_filter = FALSE;
        }
    }

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Set filter enablers to HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_BC_DA_FILTER_ENABLE, INST_SINGLE,
                                    unknown_bc_filter);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_MC_DA_FILTER_ENABLE, INST_SINGLE,
                                    unknown_mc_filter);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_UC_DA_FILTER_ENABLE, INST_SINGLE,
                                    unknown_uc_filter);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 *  See port_pp.h for more information.
 */
shr_error_e
dnx_port_pp_lif_same_interface_disable_set(
    int unit,
    bcm_port_t port,
    int filter_flags)
{
    int old_in_lif_profile = 0, new_in_lif_profile = 0;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int in_lif_profile_same_if_mode_number_of_bits, system_headers_mode;
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    in_lif_profile_same_if_mode_number_of_bits =
        dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit);
    if (in_lif_profile_same_if_mode_number_of_bits < utilex_log2_round_up(DISABLE_SAME_INTERFACE_LIF_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Not enough bits for same interface mode were allocated!\n");
    }
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &old_in_lif_profile));

    in_lif_profile_info_t_init(unit, &in_lif_profile_info);

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        if (filter_flags == 0)
        {
            in_lif_profile_info.egress_fields.lif_same_interface_mode = DISABLE_SAME_INTERFACE_LIF_PROFILE;
        }
        else
        {
            in_lif_profile_info.egress_fields.lif_same_interface_mode = DEVICE_SCOPE_LIF_PROFILE;
        }
    }
    else
    {
        if (filter_flags == 0)
        {
            in_lif_profile_info.egress_fields.lif_same_interface_mode = DEVICE_SCOPE_JR_MODE_LIF_PROFILE_DISABLE;
        }
        else
        {
            in_lif_profile_info.egress_fields.lif_same_interface_mode = DEVICE_SCOPE_JR_MODE_LIF_PROFILE;
        }
    }
    /** Exchange for new in_lif_profile, based on the new same interface mode*/
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
 *  See port_pp.h for more information.
 */
shr_error_e
dnx_port_pp_lif_same_interface_disable_get(
    int unit,
    bcm_port_t port,
    int *filter_flags)
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

    /** Based on the inlif profile - get the same interface mode - device or system scope lif*/
    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
    if (in_lif_profile_info.egress_fields.lif_same_interface_mode == DISABLE_SAME_INTERFACE_LIF_PROFILE)
    {
        *filter_flags = 0;
        in_lif_profile_info.egress_fields.lif_same_interface_mode = DISABLE_SAME_INTERFACE_LIF_PROFILE;
    }
    else
    {
        *filter_flags = 1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See port_pp.h for more information.
 */
shr_error_e
dnx_port_pp_in_lif_same_interface_set(
    int unit,
    bcm_port_t gport,
    int in_lif_same_interface_mode)
{
    int old_in_lif_profile = 0, new_in_lif_profile = 0, system_headers_mode;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &old_in_lif_profile));

    in_lif_profile_info_t_init(unit, &in_lif_profile_info);

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        if (in_lif_same_interface_mode == 0)
        {
            in_lif_profile_info.egress_fields.lif_same_interface_mode = DISABLE_SAME_INTERFACE_LIF_PROFILE;
        }
        else
        {
            in_lif_profile_info.egress_fields.lif_same_interface_mode = SYSTEM_SCOPE_LIF_PROFILE;
        }
    }
    else
    {
        if (in_lif_same_interface_mode == 0)
        {
            in_lif_profile_info.egress_fields.lif_same_interface_mode = SYSTEM_SCOPE_JR_MODE_LIF_PROFILE_DISABLE;
        }
        else
        {
            in_lif_profile_info.egress_fields.lif_same_interface_mode = SYSTEM_SCOPE_JR_MODE_LIF_PROFILE;
        }
    }

    /** Exchange for new in_lif_profile, based on the new same interface mode*/
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
 *  See port_pp.h for more information.
 */
shr_error_e
dnx_port_pp_in_lif_same_interface_get(
    int unit,
    bcm_port_t gport,
    int *in_lif_same_interface_mode)
{
    int in_lif_profile = 0;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &in_lif_profile));

    /** Based on the inlif profile - get the same interface mode - device or system scope lif*/
    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
    *in_lif_same_interface_mode = in_lif_profile_info.egress_fields.lif_same_interface_mode;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 *  See port_pp.h for more information.
 */
shr_error_e
dnx_port_pp_l2_unknown_filter_get(
    int unit,
    bcm_port_t port,
    int *filter_flags)
{
    uint32 entry_handle_id;
    uint8 unknown_bc_filter, unknown_mc_filter, unknown_uc_filter;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Get the filter enablers from HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_BC_DA_FILTER_ENABLE, INST_SINGLE,
                              &unknown_bc_filter);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_MC_DA_FILTER_ENABLE, INST_SINGLE,
                              &unknown_mc_filter);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_UC_DA_FILTER_ENABLE, INST_SINGLE,
                              &unknown_uc_filter);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Resolve the filter enablers to flags
     * Assume all filters are enabled by default.
     */
    *filter_flags = 0;
    if ((unknown_bc_filter == FALSE) && (unknown_bc_filter == FALSE) && (unknown_uc_filter == FALSE))
    {
        *filter_flags |= BCM_PORT_CONTROL_FILTER_DISABLE_ALL;
    }
    else
    {
        if (unknown_bc_filter == FALSE)
        {
            *filter_flags |= BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC;
        }
        if (unknown_mc_filter == FALSE)
        {
            *filter_flags |= BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC;
        }
        if (unknown_uc_filter == FALSE)
        {
            *filter_flags |= BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
/*
 * See header on port_pp.h
 */
shr_error_e
dnx_port_pp_llr_vtt_profile_set(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_ingress_port_termination_profile_e llr_profile,
    dbal_enum_value_field_ingress_port_termination_profile_e vt_profile)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tm_core_id, port_tm;
    bcm_port_t local_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    /*
     * Set the prt ptc_profile per tm port
     */
    BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
    {
        /** Key fields */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, local_port, (bcm_core_t *) & tm_core_id, &port_tm));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PTC, port_tm);
        /** Value fields */
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_PORT_TERMINATION_PROFILE, INST_SINGLE,
                                    llr_profile);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VT_PTC_PROFILE, INST_SINGLE, vt_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** update default lif per port */
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See header on port_pp.h
 */
shr_error_e
dnx_port_pp_llr_vtt_profile_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_ingress_port_termination_profile_e * llr_profile_p,
    dbal_enum_value_field_ingress_port_termination_profile_e * vt_profile_p)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tm_core_id, port_tm;
    bcm_port_t local_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    /*
     * Set the prt ptc_profile per tm port
     */
    BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
    {
        uint8 llr_profile_val;
        uint8 vt_profile_val;

        llr_profile_val = 0;
        vt_profile_val = 0;
        /*
         * Key fields
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, local_port, (bcm_core_t *) & tm_core_id, &port_tm));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PTC, port_tm);
        /*
         * Value fields
         */
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_PORT_TERMINATION_PROFILE, INST_SINGLE,
                                  &llr_profile_val);
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_VT_PTC_PROFILE, INST_SINGLE, &vt_profile_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        *llr_profile_p = llr_profile_val;
        *vt_profile_p = vt_profile_val;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See port_pp.h for more information.
 */

shr_error_e
dnx_port_ptc_profile_set(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_port_termination_ptc_profile_e prt_profile,
    dbal_enum_value_field_ingress_port_termination_profile_e llr_profile)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tm_core_id, port_tm;
    bcm_port_t local_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    /*
     * Set the prt ptc_profile per tm port
     */
    BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
    {
        /** Key fields */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, local_port, (bcm_core_t *) & tm_core_id, &port_tm));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PTC, port_tm);
        /** Value fields */
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_PORT_TERMINATION_PROFILE, INST_SINGLE,
                                    llr_profile);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, INST_SINGLE,
                                    prt_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ptc_profile_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_port_termination_ptc_profile_e * prt_profile,
    dbal_enum_value_field_ingress_port_termination_profile_e * llr_profile)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint8 field_val = 0;
    uint8 ptc_profile_val = 0;
    uint32 tm_core_id, port_tm;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /*
     * Get the prt ptc_profile per tm port, using first local port in case of LAG
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    /** Key fields */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, (bcm_core_t *) & tm_core_id, &port_tm));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PTC, port_tm);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_PORT_TERMINATION_PROFILE, INST_SINGLE,
                              &field_val);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, INST_SINGLE,
                              &ptc_profile_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *llr_profile = field_val;
    *prt_profile = ptc_profile_val;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See port_pp.h for more information.
 */
shr_error_e
dnx_port_pp_l2_sa_drop_action_profile_set(
    int unit,
    bcm_port_t port,
    int action_flags)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_sa_drop_action_profile_e action_profile;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Analyze the action flags
     */
    if (_SHR_IS_FLAG_SET(action_flags, BCM_PORT_CONTROL_DISCARD_MACSA_NONE))
    {
        action_profile = DBAL_ENUM_FVAL_SA_DROP_ACTION_PROFILE_NONE;
    }
    else if (_SHR_IS_FLAG_SET(action_flags, BCM_PORT_CONTROL_DISCARD_MACSA_DROP) &&
             _SHR_IS_FLAG_SET(action_flags, BCM_PORT_CONTROL_DISCARD_MACSA_TRAP))
    {
        action_profile = DBAL_ENUM_FVAL_SA_DROP_ACTION_PROFILE_TRAP_AND_DROP;
    }
    else if (_SHR_IS_FLAG_SET(action_flags, BCM_PORT_CONTROL_DISCARD_MACSA_DROP))
    {
        action_profile = DBAL_ENUM_FVAL_SA_DROP_ACTION_PROFILE_DROP;
    }
    else if (_SHR_IS_FLAG_SET(action_flags, BCM_PORT_CONTROL_DISCARD_MACSA_TRAP))
    {
        action_profile = DBAL_ENUM_FVAL_SA_DROP_ACTION_PROFILE_TRAP;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "value: 0x%X, is not a valid flag\n", action_flags);
    }

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Set action profile to HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_SA_DROP_ACTION_PROFILE, INST_SINGLE,
                                    action_profile);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the action profile for sa-drop from port.
 *  See port_pp.h for more information.
 */
shr_error_e
dnx_port_pp_l2_sa_drop_action_profile_get(
    int unit,
    bcm_port_t port,
    int *action_flags)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_sa_drop_action_profile_e action_profile;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Set action profile to HW */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SA_DROP_ACTION_PROFILE, INST_SINGLE,
                              (uint8 *) &action_profile);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Analyze the action flags
     */
    *action_flags = 0;
    if (action_profile == DBAL_ENUM_FVAL_SA_DROP_ACTION_PROFILE_NONE)
    {
        *action_flags |= BCM_PORT_CONTROL_DISCARD_MACSA_NONE;
    }
    else if (action_profile == DBAL_ENUM_FVAL_SA_DROP_ACTION_PROFILE_DROP)
    {
        *action_flags |= BCM_PORT_CONTROL_DISCARD_MACSA_DROP;
    }
    else if (action_profile == DBAL_ENUM_FVAL_SA_DROP_ACTION_PROFILE_TRAP)
    {
        *action_flags |= BCM_PORT_CONTROL_DISCARD_MACSA_TRAP;
    }
    else if (action_profile == DBAL_ENUM_FVAL_SA_DROP_ACTION_PROFILE_TRAP_AND_DROP)
    {
        *action_flags |= BCM_PORT_CONTROL_DISCARD_MACSA_TRAP;
        *action_flags |= BCM_PORT_CONTROL_DISCARD_MACSA_DROP;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "sa_drop_action_profile: %d, is not correct!\n", action_profile);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function for dnx_port_lif_vlan_domain_set()
 */
static shr_error_e
dnx_port_lif_vlan_domain_set_verify(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_class_t pclass,
    uint32 vlan_domain)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 esem_name_space_max_val = 0, esem_name_space_min_val = 0;
    uint32 next_layer_network_domain_max_value = 0, next_layer_network_domain_min_value = 0;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    /** Check if the vlan_domain is valid */
    if (!DNX_VLAN_DOMAIN_VALID(unit, vlan_domain))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The vlan_domain %d is not valid!\n", vlan_domain);
    }

    /** Check if the gport is of type TUNNEL or MPLS PORT*/
    if (!BCM_GPORT_IS_TUNNEL(gport_lif) && !BCM_GPORT_IS_MPLS_PORT(gport_lif))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport %d must be of type BCM_GPORT_TYPE_TUNNEL or BCM_GPORT_TYPE_MPLS_PORT!\n",
                     gport_lif);
    }

    if (pclass == bcmPortClassIngress)
    {
        /** Get gport hw resources */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gport_lif,
                                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                              gport_hw_resources.inlif_dbal_table_id,
                                                              DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN,
                                                              FALSE, gport_hw_resources.inlif_dbal_result_type, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE,
                                                              &next_layer_network_domain_max_value));

        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                              gport_hw_resources.inlif_dbal_table_id,
                                                              DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN,
                                                              FALSE, gport_hw_resources.inlif_dbal_result_type, 0,
                                                              DBAL_PREDEF_VAL_MIN_VALUE,
                                                              &next_layer_network_domain_min_value));

        if ((vlan_domain > next_layer_network_domain_max_value) || (vlan_domain < next_layer_network_domain_min_value))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid vlan_domain %d! In case of bcmPortClassIngress the vlan_domain can be a value in range from %d to %d!."
                         "DBAL table: %s, DBAL result type: %d\n", vlan_domain, next_layer_network_domain_min_value,
                         next_layer_network_domain_max_value, dbal_logical_table_to_string(unit,
                                                                                           gport_hw_resources.inlif_dbal_table_id),
                         gport_hw_resources.inlif_dbal_result_type);
        }
    }

    if (pclass == bcmPortClassEgress)
    {
        /** Get gport hw resources */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gport_lif,
                                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        if ((gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL))
        {
            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  gport_hw_resources.outlif_dbal_table_id,
                                                                  DBAL_FIELD_ESEM_NAME_SPACE,
                                                                  FALSE,
                                                                  gport_hw_resources.outlif_dbal_result_type,
                                                                  0, DBAL_PREDEF_VAL_MAX_VALUE,
                                                                  &esem_name_space_max_val));
            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  gport_hw_resources.outlif_dbal_table_id,
                                                                  DBAL_FIELD_ESEM_NAME_SPACE,
                                                                  FALSE, gport_hw_resources.outlif_dbal_result_type, 0,
                                                                  DBAL_PREDEF_VAL_MIN_VALUE, &esem_name_space_min_val));
            if ((vlan_domain > esem_name_space_max_val) || (vlan_domain < esem_name_space_min_val))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid vlan_domain %d! In case of bcmPortClassEgress the vlan_domain of IP tunnel can be a value in range from %d to %d!\n",
                             vlan_domain, esem_name_space_min_val, esem_name_space_max_val);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_lif_ingress_vlan_domain_set(
    int unit,
    bcm_gport_t gport_lif,
    uint32 vlan_domain)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_port_class_t pclass;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    pclass = bcmPortClassIngress;

    SHR_INVOKE_VERIFY_DNX(dnx_port_lif_vlan_domain_set_verify(unit, gport_lif, pclass, vlan_domain));

    /** Get gport hw resources */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport_lif,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    if (gport_hw_resources.local_in_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport LIF %d is not valid!\n", gport_lif);
    }

    /** Configure vlan domain per In-Lif */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, vlan_domain);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_lif_egress_vlan_domain_set(
    int unit,
    bcm_gport_t gport_lif,
    uint32 vlan_domain)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_port_class_t pclass;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    pclass = bcmPortClassEgress;
    SHR_INVOKE_VERIFY_DNX(dnx_port_lif_vlan_domain_set_verify(unit, gport_lif, pclass, vlan_domain));

    /** Get gport hw resources */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport_lif,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    if (gport_hw_resources.local_out_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport LIF %d is not valid!\n", gport_lif);
    }
    /** Configure vlan domain per Out-Lif  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.outlif_dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE, vlan_domain);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function for dnx_port_lif_vlan_domain_get()
 */
static shr_error_e
dnx_port_lif_vlan_domain_get_verify(
    int unit,
    bcm_gport_t gport_lif,
    uint32 *vlan_domain)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_domain, _SHR_E_PARAM, "vlan_domain");

    /** Check if the gport is of type TUNNEL or MPLS PORT*/
    if (!BCM_GPORT_IS_TUNNEL(gport_lif) && !BCM_GPORT_IS_MPLS_PORT(gport_lif))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport %d must be of type BCM_GPORT_TYPE_TUNNEL or BCM_GPORT_TYPE_MPLS_PORT!\n",
                     gport_lif);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_lif_ingress_vlan_domain_get(
    int unit,
    bcm_gport_t gport_lif,
    uint32 *vlan_domain)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    SHR_INVOKE_VERIFY_DNX(dnx_port_lif_vlan_domain_get_verify(unit, gport_lif, vlan_domain));

    /** Get gport hw resources */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport_lif,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    if (gport_hw_resources.local_in_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport LIF %d is not valid!\n", gport_lif);
    }

    /** Get vlan domain per In-Lif */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, vlan_domain));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_lif_egress_vlan_domain_get(
    int unit,
    bcm_gport_t gport_lif,
    uint32 *vlan_domain)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    SHR_INVOKE_VERIFY_DNX(dnx_port_lif_vlan_domain_get_verify(unit, gport_lif, vlan_domain));

    /** Get gport hw resources */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport_lif,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    if (gport_hw_resources.local_out_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport LIF %d is not valid!\n", gport_lif);
    }

    /** Configure vlan domain per Out-Lif  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.outlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE, vlan_domain));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Initialization function for the egress per port filters enable and trap context port profile.
 */
static shr_error_e
dnx_port_pp_egress_hw_set(
    int unit,
    bcm_port_t port,
    dnx_port_pp_egress_port_info_t * egress_port_info)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                     egress_port_info->trap_context_port_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, INST_SINGLE,
                                     egress_port_info->default_esem_cmd);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RQP_DISCARD_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_tm_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INVALID_OTM_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_tm_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSS_STACKING_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_tm_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_MULTICAST_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_tm_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLEM_PP_TRAP_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_DA_ENABLE, INST_SINGLE,
                                         egress_port_info->egress_pp_filters_enable);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLEM_NON_PP_TRAP_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_SCOPING_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ZERO_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ONE_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_FILTERS_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_FILTERS_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_4_FILTERS_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_CONTEXT_PORT_PROFILE,
                                     INST_SINGLE, egress_port_info->term_port_profile);
        /*
         * Set acceptable frame type per port enablers
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ENABLE, INST_SINGLE,
                                     egress_port_info->egress_pp_filters_enable);
        /*
         * Set PRP port profile
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                     egress_port_info->prp2_port_profile);

        /*
         * Set Forward port profile
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_FWD_CODE_PORT_PROFILE, INST_SINGLE,
                                     egress_port_info->fwd_code_port_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_port_pp_egress_set(
    int unit,
    bcm_port_t port,
    int switch_header_type)
{
    
    dnx_port_pp_egress_port_info_t egress_port_info;
    dnx_algo_gpm_gport_phy_info_t gport_info, gport_info_oamp, gport_info_olp;
    int count_oamp, count_olp, system_headers_mode;
    bcm_gport_t oamp_gport[2];
    bcm_gport_t olp_gport;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&egress_port_info, 0, sizeof(dnx_port_pp_egress_port_info_t));
    egress_port_info.default_esem_cmd = dnx_data_esem.access_cmd.no_action_get(unit);
    egress_port_info.fwd_code_port_profile = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PORT_PROFILE_NORMAL;
    egress_port_info.prp2_port_profile = DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_ETHERNET_TYPICAL;
    egress_port_info.term_port_profile = DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_ETHERNET_TYPICAL;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    sal_memset(oamp_gport, 0, sizeof(oamp_gport));

    
    switch (switch_header_type)
    {
        case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
        {
            egress_port_info.trap_context_port_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_NORMAL;

            /*
             * Enable Filters per Egress PP port
             */
            egress_port_info.egress_pp_filters_enable = TRUE;

            /*
             * Allocate Egress LLVP profile per port
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_default_per_port_init(unit, port));

            /*
             * Set port default AC for eth ports
             */
            egress_port_info.default_esem_cmd = dnx_data_esem.access_cmd.default_ac_get(unit);

            break;
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP is not supported on egress!");
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE is not supported on egress!");
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW is not supported on egress!");
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_RAW:
        {
            egress_port_info.trap_context_port_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_RAW_PORT;
            break;
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_CPU:
        case BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU:
        {
            /*
             * Allocate Egress LLVP profile per port
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_default_per_port_init(unit, port));

            /*
             * Set port default AC for eth ports
             */
            egress_port_info.default_esem_cmd = dnx_data_esem.access_cmd.default_ac_get(unit);
            egress_port_info.trap_context_port_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_CPU_PORT;
            egress_port_info.fwd_code_port_profile = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PORT_PROFILE_CPU_PORT;
            if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU)
            {
                egress_port_info.prp2_port_profile = DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_KEEP_ORIG_ENC_ON_TRAPPED;
            }
            else
            {
                egress_port_info.prp2_port_profile = DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_ETHERNET_TYPICAL;
            }
            break;
        }

        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2 is not supported on egress!");
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE is not supported on egress!");
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Switch header type (%d) is not expected!", switch_header_type);
            break;
        }
    }

    SHR_IF_ERR_EXIT(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, oamp_gport, &count_oamp));
    SHR_IF_ERR_EXIT(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OLP, 1, &olp_gport, &count_olp));
    if (((count_oamp > 1) || (count_olp == 1)) && (!BCM_GPORT_IS_TRUNK(port)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, oamp_gport[1], DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info_oamp));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, olp_gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info_olp));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        if (gport_info.local_port == gport_info_oamp.local_port)
        {
            egress_port_info.term_port_profile = DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_OAMP_BFDV6;
        }
        if ((gport_info.local_port == gport_info_olp.local_port)
            && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE))
        {
            egress_port_info.term_port_profile =
                DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_JR1_COMP_MACT_LEARNING_WA_JR2_TO_JR1_RCY_PORT;
        }
    }

    SHR_IF_ERR_EXIT(dnx_port_pp_egress_hw_set(unit, port, &egress_port_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function for dnx_port_pp_prt_ptc_profile_internal_set
 */
static shr_error_e
dnx_port_pp_prt_ptc_profile_internal_set_verify(
    int unit,
    dnx_algo_gpm_gport_phy_info_t * gport_info,
    uint32 header_type)
{
    int system_headers_mode = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_info, _SHR_E_PARAM, "gport_info");

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if ((header_type != BCM_SWITCH_PORT_HEADER_TYPE_ETH) && (header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
        && (header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
        && ((system_headers_mode != dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            && (header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
        && ((system_headers_mode != dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            && (header_type != BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Switch header type (%d) is not supported for the used direction!", header_type);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See h. for reference
 */
shr_error_e
dnx_port_pp_prt_ptc_profile_set(
    int unit,
    bcm_port_t port,
    uint32 header_type)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** get gport info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** use internal function to map pp port to ptc */
    SHR_IF_ERR_EXIT(dnx_port_pp_prt_ptc_profile_internal_set(unit, &gport_info, header_type));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See h. for reference
 */
shr_error_e
dnx_port_pp_prt_ptc_profile_internal_set(
    int unit,
    dnx_algo_gpm_gport_phy_info_t * gport_info,
    uint32 header_type)
{
    uint32 entry_handle_id;
    int port_termination_ptc_profile;
    uint32 tm_core_id, port_tm;
    bcm_port_t local_port;
    uint32 pp_port_index;
    int system_headers_mode = 0;
    uint32 prt_qualifier_raw = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_pp_prt_ptc_profile_internal_set_verify(unit, gport_info, header_type));

    switch (header_type)
    {
        case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
        {
            port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
            break;
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP:
        {
            port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2;
            break;
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE:
        {
            system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
            if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            {
                port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Switch header type (%d) is not supported for system_headers_mode=%d!",
                             header_type, system_headers_mode);
            }
            prt_qualifier_raw = ITMH_A1_PARSING_CONTEXT_MSBS;
            break;
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2:
        {
            port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2;
            break;
        }
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE:
        {
            system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
            if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
            {
                port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Switch header type (%d) is not supported for system_headers_mode=%d!",
                             header_type, system_headers_mode);
            }
            prt_qualifier_raw = ITMH_A1_PARSING_CONTEXT_MSBS;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Switch header type (%d) is not expected!", header_type);
            break;
        }

    }
    /** Configure INGRESS_PP_PTC_PORT */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));

    BCM_PBMP_ITER(gport_info->local_port_bmp, local_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, local_port, (bcm_core_t *) & tm_core_id, &port_tm));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, port_tm);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, INST_SINGLE,
                                     port_termination_ptc_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_RAW, INST_SINGLE,
                                     prt_qualifier_raw);
        for (pp_port_index = 0; pp_port_index < gport_info->internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            if (gport_info->internal_port_pp_info.core_id[pp_port_index] == tm_core_id)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE,
                                             gport_info->internal_port_pp_info.pp_port[pp_port_index]);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            else if (gport_info->internal_port_pp_info.nof_pp_ports - 1 == pp_port_index)
            {
                /** In case that there is no match in all the iterations, return an error */
                SHR_ERR_EXIT(_SHR_E_PARAM, "No PP port corresponds to TM port %d on core %d", port_tm, tm_core_id);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - verify function for IN-LIF wide data set/get.
*
* \param [in] unit - Unit ID
* \param [in] gport_hw_resources - holds the parameters associated to the given gport in the API
* \param [in] flags - flags
* \param [out] dbal_info - generic information for the table, type and field
*
* \return
*   See shr_error_r
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_port_pp_wide_data_common_verify(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 flags,
    dbal_table_field_info_t * dbal_info)
{
    shr_error_e rv;
    int legal_flags = BCM_PORT_WIDE_DATA_INGRESS;

    SHR_FUNC_INIT_VARS(unit);

    /** verify flags */
    SHR_MASK_VERIFY(flags, legal_flags, _SHR_E_PARAM, "some of the flags are not supported.\n");

    /*
     * generic data is supported only for ingress IN-LIF, therefore, the flag must be set.
     */
    if (!_SHR_IS_FLAG_SET(flags, BCM_PORT_WIDE_DATA_INGRESS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_WIDE_DATA_INGRESS flag must be set \n");
    }

    /** Generic data is currenly supported only for AC formats.
        PEMA has access only from VTT stages 1 and 5 which are used for AC IN-LIF,
        so all other IN-LIF types currently not supported */
    if (gport_hw_resources->inlif_dbal_table_id != DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION &&
        gport_hw_resources->inlif_dbal_table_id != DBAL_TABLE_IN_AC_INFO_DB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "generic data is supported only for AC IN-LIF types \n");
    }

    /** get from dbal, the information for the table, type and field */
    rv = dbal_tables_field_info_get_no_err(unit, gport_hw_resources->inlif_dbal_table_id, DBAL_FIELD_LIF_GENERIC_DATA_0,
                                           0, gport_hw_resources->inlif_dbal_result_type, 0, dbal_info);

    /** if field not found - it means that the current gport belong to a format that do not contain generic data */
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "generic data is not supported for the given gport \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - verify function for IN-LIF wide data set.
*
* \param [in] unit - Unit ID
* \param [in] gport_hw_resources - holds the parameters associated to the given gport in the API
* \param [in] flags - flags
* \param [in] data - wide data (max value depends on the LIF type and its format)
*
* \return
*   See shr_error_r
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_port_pp_wide_data_set_verify(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 flags,
    uint64 data)
{
    dbal_table_field_info_t dbal_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters for flags and gport information */
    sal_memset(&dbal_info, 0, sizeof(dbal_table_field_info_t));
    SHR_IF_ERR_EXIT(dnx_port_pp_wide_data_common_verify(unit, gport_hw_resources, flags, &dbal_info));

    if ((COMPILER_64_HI(data) != 0) || (COMPILER_64_LO(data) > dbal_info.max_value))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "data={0x%X, 0x%X}. max allowed=0x%X\n",
                     COMPILER_64_HI(data), COMPILER_64_LO(data), dbal_info.max_value);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - verify function for API IN-LIF wide data get.
*
* \param [in] unit - Unit ID
* \param [in] gport_hw_resources - holds the parameters associated to the given gport in the API
* \param [in] flags - flags
* \param [in] data - data pointer
*
* \return
*   See shr_error_r
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_port_pp_wide_data_get_verify(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 flags,
    uint64 *data)
{
    dbal_table_field_info_t dbal_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters for flags and gport information */
    sal_memset(&dbal_info, 0, sizeof(dbal_table_field_info_t));
    SHR_IF_ERR_EXIT(dnx_port_pp_wide_data_common_verify(unit, gport_hw_resources, flags, &dbal_info));

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - write to HW the wide data for the given local in lif.
*
* \param [in] unit - Unit ID
* \param [in] table_id - the dbal table to write into
* \param [in] type - the type of format to write into
* \param [in] local_in_lif - local in lif id
* \param [in] data - wide data
*
* \return
*   See shr_error_r
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_port_pp_wide_data_hw_set(
    int unit,
    dbal_tables_e table_id,
    int type,
    int local_in_lif,
    uint32 data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /** Set keys */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
    /** Set values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, INST_SINGLE, data);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - get from HW the wide data for the relevant IN-LIF ID.
*
* \param [in] unit - Unit ID
* \param [in] table_id - the dbal table to write into
* \param [in] type - the type of format to write into
* \param [in] local_in_lif - local in lif id
* \param [in] data - wide data
*
* \return
*   See shr_error_r
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_port_pp_wide_data_hw_get(
    int unit,
    dbal_tables_e table_id,
    int type,
    int local_in_lif,
    uint32 *data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set keys */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, INST_SINGLE, data);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - API implementation for IN-LIF wide data.
*    function add generic data for IN-LIF ARR entry, per gport.
*    The data can be used as qualifier in the ingress PMF.
*
* \param [in] unit - Unit ID
* \param [in] gport - LIF gport , used to derive the local-inlif and its result type
* \param [in] flags - flags
* \param [in] data - wide data (max value depends on the LIF type and its format)
*
* \return
*   See shr_error_r
* \remark
*   wide data is supported only to specific LIF types (according to architucture design)
* \see
*   * None
*/
int
bcm_dnx_port_wide_data_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    uint64 data)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    SHR_FUNC_INIT_VARS(unit);

    /** get the local in-lif hw resource associated to the gport */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_pp_wide_data_set_verify(unit, &gport_hw_resources, flags, data));

    /** write to HW the data. */
    SHR_IF_ERR_EXIT(dnx_port_pp_wide_data_hw_set(unit, gport_hw_resources.inlif_dbal_table_id,
                                                 gport_hw_resources.inlif_dbal_result_type,
                                                 gport_hw_resources.local_in_lif, COMPILER_64_LO(data)));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - API implementation for IN-LIF wide data get.
*    function return the generic data associated to the given gport.
*
* \param [in] unit - Unit ID
* \param [in] gport - holds decodeing of the local IN-LIF and its type
* \param [in] flags - flags
* \param [out] data - wide data
*
* \return
*   See shr_error_r
* \remark
*   wide data is not supported in all LIF types (according to architucture design)
* \see
*   * None
*/
int
bcm_dnx_port_wide_data_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    uint64 *data)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    /** get the local in-lif hw resource associated to the gport */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_pp_wide_data_get_verify(unit, &gport_hw_resources, flags, data));

    /** get from HW the data. */
    SHR_IF_ERR_EXIT(dnx_port_pp_wide_data_hw_get(unit, gport_hw_resources.inlif_dbal_table_id,
                                                 gport_hw_resources.inlif_dbal_result_type,
                                                 gport_hw_resources.local_in_lif, &value));

    COMPILER_64_ZERO((*data));
    COMPILER_64_ADD_32((*data), value);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify function for bcm_port_flood_group_set()
 *
 * \param [in] unit - Relevant uni.
 * \param [in] port - Physical port or gport for lif-id.
 * \param [in] is_phy_port - Whether the port is physical port or not.
 * \param [in] flags - Unused now.
 * \param [in] flood_groups - The flooding groups for unknown packets.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *  bcm_dnx_port_flood_group_set
 */
static shr_error_e
dnx_port_flood_group_set_verify(
    int unit,
    bcm_gport_t port,
    uint8 is_phy_port,
    uint32 flags,
    bcm_port_flood_group_t * flood_groups)
{
    uint32 eth_qual_bc_with_mc_set;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify the port */
    if (is_phy_port == FALSE)
    {
        if (!(BCM_GPORT_IS_VLAN_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port) ||
              BCM_GPORT_IS_EXTENDER_PORT(port) || BCM_GPORT_IS_VXLAN_PORT(port)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The given gport 0x%X is not physical port and not the LIF types that represents vlan/mpls/extender/vxlan port!\n",
                         port);
        }
    }

    /*
     * JR2_B0 WA: broadcast_group must be the same with unknown_multicast_group,
     * since BC is also recognized as MC and unknown MC destination has a higher priority.
     */
    eth_qual_bc_with_mc_set = dnx_data_l2.hw_bug.feature_get(unit, dnx_data_l2_hw_bug_eth_qual_is_mc);
    if (eth_qual_bc_with_mc_set)
    {
        if (flood_groups->broadcast_group != flood_groups->unknown_multicast_group)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "broadcast_group (0x%08X) and unknown_multicast_group (0x%08X) must be the same in current device!\n",
                         flood_groups->broadcast_group, flood_groups->unknown_multicast_group);
        }
    }

    /** Verify the flooding groups */
    SHR_NULL_CHECK(flood_groups, _SHR_E_PARAM, "A valid pointer to flooding groups is needed!");
    if ((flood_groups->unknown_unicast_group == BCM_GPORT_INVALID) &&
        (flood_groups->unknown_multicast_group == BCM_GPORT_INVALID) &&
        (flood_groups->broadcast_group == BCM_GPORT_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "All destination offset in flood groups are not valid!\n");
    }

    if ((!BCM_GPORT_IS_BLACK_HOLE(flood_groups->unknown_unicast_group) &&
         !BCM_GPORT_IS_MCAST(flood_groups->unknown_unicast_group) &&
         (flood_groups->unknown_unicast_group != BCM_GPORT_INVALID)) ||
        (!BCM_GPORT_IS_BLACK_HOLE(flood_groups->unknown_multicast_group) &&
         !BCM_GPORT_IS_MCAST(flood_groups->unknown_multicast_group) &&
         (flood_groups->unknown_multicast_group != BCM_GPORT_INVALID)) ||
        (!BCM_GPORT_IS_BLACK_HOLE(flood_groups->broadcast_group) &&
         !BCM_GPORT_IS_MCAST(flood_groups->broadcast_group) && (flood_groups->broadcast_group != BCM_GPORT_INVALID)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only black hole, multicast and invalid gport are supported!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify function for bcm_port_flood_group_get()
 *
 * \param [in] unit - Relevant uni.
 * \param [in] port - Physical port or gport for lif-id.
 * \param [in] is_phy_port - Whether the port is physical port or not.
 * \param [in] flags - Unused now.
 * \param [in] flood_groups - The flooding groups for unknown packets.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *  bcm_dnx_port_flood_group_get
 */
static shr_error_e
dnx_port_flood_group_get_verify(
    int unit,
    bcm_gport_t port,
    uint8 is_phy_port,
    uint32 flags,
    bcm_port_flood_group_t * flood_groups)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the port */
    if (is_phy_port == FALSE)
    {
        if (!(BCM_GPORT_IS_VLAN_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port) ||
              BCM_GPORT_IS_EXTENDER_PORT(port) || BCM_GPORT_IS_VXLAN_PORT(port)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The given gport 0x%X is not physical port and not the LIF types that represents vlan/mpls/extender/vxlan port!\n",
                         port);
        }
    }

    /** Verify the flooding groups */
    SHR_NULL_CHECK(flood_groups, _SHR_E_PARAM, "A valid pointer to flooding groups is needed!");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Translate the multicast groups to default desitinations or offsets for
 *  unknown packtes.
 *
 * \param [in] unit - Relevant uni.
 * \param [in] flood_groups - The flooding groups for unknown packets.
 * \param [out] default_frwrd_dst - Default desitination or offset.
 *              It is a array with 3 members for unknown UC, MC and BC packets.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *  dnx_port_flood_map_groups_from_default_frwrd_dst
 */
static shr_error_e
dnx_port_flood_map_groups_to_default_frwrd_dst(
    int unit,
    bcm_port_flood_group_t flood_groups,
    dnx_default_frwrd_dst_t * default_frwrd_dst)
{
    dbal_enum_value_field_destination_distribution_type_e da_type;
    bcm_gport_t defaut_dst_offset[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(default_frwrd_dst, _SHR_E_PARAM, "A valid pointer is needed!");

    /*
     * Calculate the destination offset for kinds of DA types
     */
    sal_memset(default_frwrd_dst, 0,
               sizeof(dnx_default_frwrd_dst_t) * DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES);

    defaut_dst_offset[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC] = flood_groups.unknown_multicast_group;
    defaut_dst_offset[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_BC] = flood_groups.broadcast_group;
    defaut_dst_offset[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_UC] = flood_groups.unknown_unicast_group;

    for (da_type = DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC;
         da_type < DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES; da_type++)
    {
        /*
         * Currently, assume only drop and MC destination per LIF/PORT is supported.
         * If not drop, then it must be MC.
         */
        if (BCM_GPORT_IS_BLACK_HOLE(defaut_dst_offset[da_type]))
        {
            default_frwrd_dst[da_type].add_vsi_dst = FALSE;
            default_frwrd_dst[da_type].offset = 0;
            SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                            (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, BCM_GPORT_BLACK_HOLE,
                             &default_frwrd_dst[da_type].destination));
        }
        else
        {
            default_frwrd_dst[da_type].add_vsi_dst = TRUE;
            default_frwrd_dst[da_type].offset = _BCM_MULTICAST_ID_GET(defaut_dst_offset[da_type]);
            default_frwrd_dst[da_type].destination = 0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Translate the default desitinations or offsets to multicast groups for
 *  unknown packtes.
 *
 * \param [in] unit - Relevant uni.
 * \param [in] default_frwrd_dst - Default desitination or offset.
 *             It is a array with 3 members for unknown UC, MC and BC packets.
 * \param [out] flood_groups - The flooding groups for unknown packets.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *  dnx_port_flood_map_groups_to_default_frwrd_dst
 */
static shr_error_e
dnx_port_flood_map_groups_from_default_frwrd_dst(
    int unit,
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES],
    bcm_port_flood_group_t * flood_groups)
{
    dbal_enum_value_field_destination_distribution_type_e da_type;
    bcm_gport_t defaut_dst_offset[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flood_groups, _SHR_E_PARAM, "A valid pointer is needed!");

    /*
     * Get the flood groups from offset for kinds of DA types
     */
    for (da_type = DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC;
         da_type < DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES; da_type++)
    {
        /** Translate the destination to gport*/
        if (default_frwrd_dst[da_type].add_vsi_dst == FALSE)
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                            (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, default_frwrd_dst[da_type].destination,
                             &defaut_dst_offset[da_type]));
        }
        else
        {
            defaut_dst_offset[da_type] = default_frwrd_dst[da_type].offset;
        }

        /** Check the data retrieved is valid */
        if ((default_frwrd_dst[da_type].add_vsi_dst == FALSE) && (defaut_dst_offset[da_type] != BCM_GPORT_BLACK_HOLE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid flooding destination (0x%08X) is found on DA type 0x%X!\n",
                         defaut_dst_offset[da_type], da_type);
        }
    }

    flood_groups->broadcast_group = defaut_dst_offset[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_BC];
    flood_groups->unknown_multicast_group = defaut_dst_offset[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC];
    flood_groups->unknown_unicast_group = defaut_dst_offset[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_UC];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Configure default forwarding destinations according to LIF flooding profile.
 *  The LIF flooding profile is also exchanged with default forwarding destionations.
 *
 * \param [in] unit - The unit ID.
 * \param [in] port - Gport for lif-id.
 * \param [in] default_frwrd_dst - Array with destination offset for unknown DAs.
 * \param [out] old_in_lif_profile - Original inlif profile.
 * \param [out] new_in_lif_profile - New inlif profile.
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  In-lif-profile is exchanged according to default_frwrd_dst and updated in INLIF
 *  table. default_frwrd_dst is also used to exchange a da_not_found_dst_profile
 *  per LIF and configured in the default forwarding destination table.
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
static shr_error_e
dnx_port_lif_flood_profile_set(
    int unit,
    bcm_gport_t port,
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES],
    int *old_in_lif_profile,
    int *new_in_lif_profile)
{
    uint32 entry_handle_id;
    int in_lif_profile;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(old_in_lif_profile, _SHR_E_INTERNAL, "A valid pointer for old in_lif_profile is needed!");
    SHR_NULL_CHECK(new_in_lif_profile, _SHR_E_INTERNAL, "A valid pointer for new in_lif_profile is needed!");

    /*
     * Get the old in_lif_profile.
     */
    flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port, flags, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                                        (uint32 *) &in_lif_profile));
    *old_in_lif_profile = in_lif_profile;

    /*
     * Exchange for new in_lif_profile.
     */
    in_lif_profile_info_t_init(unit, &in_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));

    sal_memcpy(in_lif_profile_info.ingress_fields.default_frwrd_dst, default_frwrd_dst,
               sizeof(dnx_default_frwrd_dst_t) * DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES);

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, in_lif_profile, new_in_lif_profile, LIF,
                     gport_hw_resources.inlif_dbal_table_id));

    /*
     * Update inLIF table with the new in_lif_profile
     */
    if (*old_in_lif_profile != *new_in_lif_profile)
    {
        /** Update the LIF table with the new in_lif_profile*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.inlif_dbal_table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                     *new_in_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set the flooding destinations for the unknown packets.
 *  See port_pp.h for details.
 */
shr_error_e
dnx_port_lif_flood_profile_action_set(
    int unit,
    int port_flood_profile,
    int lif_flood_profile,
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES])
{
    uint32 entry_handle_id;
    dbal_enum_value_field_destination_distribution_type_e da_type;
    int port_flood_profile_i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * In case of port flooding, set the table with port_flood_profile = 1 only.
     * In case of LIF flooding, set the table with port_flood_profile = 0 and 1.
     * port_flood_profile = lif_flood_profile = 0 should be the system default values.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_FORWARD_UNKNOWN_DA_PROFILE, &entry_handle_id));

    for (port_flood_profile_i = port_flood_profile;
         port_flood_profile_i < DBAL_NOF_ENUM_DA_NOT_FOUND_ACTION_PROFILE_VALUES; port_flood_profile_i++)
    {
        /** Set for all DA types*/
        for (da_type = DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC;
             da_type < DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES; da_type++)
        {
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_ACTION_PROFILE,
                                      port_flood_profile_i);
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_DESTINATION_PROFILE,
                                      lif_flood_profile);
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, da_type);

            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ADD_VSI_DEST, INST_SINGLE,
                                        default_frwrd_dst[da_type].add_vsi_dst);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                         ((default_frwrd_dst[da_type].add_vsi_dst == TRUE) ?
                                          default_frwrd_dst[da_type].offset : default_frwrd_dst[da_type].destination));

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_FORWARD_UNKNOWN_DA_PROFILE, entry_handle_id));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set the flooding groups for unknown packets to a port or InLIF.
 *  Currently, only drop and Multicast destination per LIF/PORT is supported.
 *
 * \param [in] unit - The unit ID.
 * \param [in] port - Physical port or gport for lif-id.
 * \param [in] flags - Unused now.
 * \param [in] flood_groups - The flooding destinations for unknown packets, include:
 *             -unknown_unicast_group - Destination for unknown unicast packets.
 *             -unknown_multicast_group - Destination for unknown multicast packets.
 *             -broadcast_group - Destination for broadcast packets.
 *
 * \return
 *  \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  The flood groups are actually gport for unknow packets. If they are black hole,
 *  they are used as the final destinations, otherwise the final default destionations
 *  are calculated as VSI.default-destination + offset-calculated-from-the-groups.
 *
 *  From hw point of view, these groups are retrieved by {da-type, lif-flood-profile,
 *  port-flood-profile}. By default, the configurations guarantee that VSI.default-destination
 *  is used for all ports/LIFs with port-flood-profile = lif-flood-profile = 0.
 *
 *  If any member of flood_groups is BCM_GPORT_INVALID, it means the previous
 *  configuration for this case is used.
 *  For physical port, in case of all the members are equal and not black hole,
 *  The default configurations will be used. Otherwise update the destinations.
 *
 * \see
 *   bcm_dnx_port_flood_group_get.
 */
bcm_error_t
bcm_dnx_port_flood_group_set(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_flood_group_t * flood_groups)
{
    uint8 is_phy_port;
    uint8 is_lif_flood;
    uint8 is_dst_update;
    uint32 pp_port_index;

    uint32 entry_handle_id;

    int port_flood_profile, lif_flood_profile;
    bcm_port_flood_group_t flood_groups_org;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Validate the inputs
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));

    SHR_INVOKE_VERIFY_DNX(dnx_port_flood_group_set_verify(unit, port, is_phy_port, flags, flood_groups));

    /*
     * Get the original flooding groups
     */
    sal_memset(&flood_groups_org, 0, sizeof(bcm_port_flood_group_t));
    SHR_IF_ERR_EXIT(bcm_dnx_port_flood_group_get(unit, port, flags, &flood_groups_org));

    /*
     * Update the flooding groups with valid destination offset
     */
    if (flood_groups->unknown_unicast_group != BCM_GPORT_INVALID)
    {
        flood_groups_org.unknown_unicast_group = flood_groups->unknown_unicast_group;
    }

    if (flood_groups->unknown_multicast_group != BCM_GPORT_INVALID)
    {
        flood_groups_org.unknown_multicast_group = flood_groups->unknown_multicast_group;
    }

    if (flood_groups->broadcast_group != BCM_GPORT_INVALID)
    {
        flood_groups_org.broadcast_group = flood_groups->broadcast_group;
    }

    /*
     * Calculate the destination offset for kinds of DA types
     */
    SHR_IF_ERR_EXIT(dnx_port_flood_map_groups_to_default_frwrd_dst(unit, flood_groups_org, default_frwrd_dst));

    /*
     * Check the flooding destination type: Port or LIF
     */
    is_lif_flood = (is_phy_port == TRUE) ? FALSE : TRUE;

    /** Assume the destinations is not need to updated by default.*/
    is_dst_update = FALSE;

    if (is_lif_flood == FALSE)
    {
        /** In case of not lif flooding. lif_flood_profile is always 0. */
        lif_flood_profile = dnx_data_lif.in_lif.default_da_not_found_destination_profile_get(unit);

        /*
         * Ratiocinate the proper port flood profile.
         */
        if ((flood_groups_org.unknown_unicast_group == flood_groups_org.broadcast_group) &&
            (flood_groups_org.unknown_multicast_group == flood_groups_org.broadcast_group) &&
            (flood_groups_org.broadcast_group != BCM_GPORT_BLACK_HOLE) &&
            (_BCM_MULTICAST_ID_GET(flood_groups_org.broadcast_group) == 0))
        {
            /** Use default flood destination: port_flood_profile = lif_flood_profile = 0.*/
            port_flood_profile = DBAL_ENUM_FVAL_DA_NOT_FOUND_ACTION_PROFILE_NONE;
        }
        else
        {
            port_flood_profile = DBAL_ENUM_FVAL_DA_NOT_FOUND_ACTION_PROFILE_DESTINATION;
            is_dst_update = TRUE;
        }

        /*
         * Update the default destinations first
         */
        if (is_dst_update == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_port_lif_flood_profile_action_set
                            (unit, port_flood_profile, lif_flood_profile, default_frwrd_dst));
        }

        /*
         * Update the PORT TABLE with the new port_flood_profile then.
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_ACTION_PROFILE, INST_SINGLE,
                                        port_flood_profile);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    else
    {
        int old_in_lif_profile = 0, new_in_lif_profile = 0;

        port_flood_profile = DBAL_ENUM_FVAL_DA_NOT_FOUND_ACTION_PROFILE_NONE;

        /** Configure InLIF flood profile*/
        SHR_IF_ERR_EXIT(dnx_port_lif_flood_profile_set
                        (unit, port, default_frwrd_dst, &old_in_lif_profile, &new_in_lif_profile));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the flooding groups for unknown packets from a port or InLIF.
 *  Currently, only drop and Multicast destination per LIF/PORT is supported.
 *
 * \param [in] unit - The unit ID.
 * \param [in] port - Physical port or gport for lif-id.
 * \param [in] flags - Unused now.
 * \param [out] flood_groups - The flooding destinations for unknown packets.
 *                             see bcm_port_flood_group_t for detail.
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  See bcm_dnx_port_flood_group_set for more information.
 *
 * \see
 *   bcm_dnx_port_flood_group_set.
 */
bcm_error_t
bcm_dnx_port_flood_group_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_flood_group_t * flood_groups)
{
    uint8 is_phy_port;
    uint8 is_lif_flood;
    uint32 entry_handle_id;

    int port_flood_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Validate the inputs
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    SHR_INVOKE_VERIFY_DNX(dnx_port_flood_group_get_verify(unit, port, is_phy_port, flags, flood_groups));

    /*
     * Check the flooding destination type: Port or LIF
     */
    is_lif_flood = (is_phy_port == TRUE) ? FALSE : TRUE;

    /*
     * Get the destination information
     */
    if (is_lif_flood == FALSE)
    {
        /*
         * In case of port floodinng
         */
        int lif_flood_profile;
        dnx_algo_gpm_gport_phy_info_t gport_info;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[0]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[0]);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_ACTION_PROFILE, INST_SINGLE,
                                   (uint32 *) &port_flood_profile);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (port_flood_profile == DBAL_ENUM_FVAL_DA_NOT_FOUND_ACTION_PROFILE_NONE)
        {
            flood_groups->unknown_unicast_group = 0;
            flood_groups->unknown_multicast_group = 0;
            flood_groups->broadcast_group = 0;
        }
        else
        {
            uint8 add_vsi_dst;
            uint32 destination;
            dbal_enum_value_field_destination_distribution_type_e da_type;
            dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES];

            /** In case of port flooding, lif_flood_profile is always 0. */
            lif_flood_profile = 0;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_FORWARD_UNKNOWN_DA_PROFILE, entry_handle_id));
            for (da_type = DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC;
                 da_type < DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES; da_type++)
            {
                dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_ACTION_PROFILE,
                                          port_flood_profile);
                dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_DESTINATION_PROFILE,
                                          lif_flood_profile);
                dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, da_type);

                dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_ADD_VSI_DEST, INST_SINGLE, &add_vsi_dst);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                default_frwrd_dst[da_type].add_vsi_dst = add_vsi_dst;
                if (add_vsi_dst == FALSE)
                {
                    default_frwrd_dst[da_type].offset = 0;
                    default_frwrd_dst[da_type].destination = destination;
                }
                else
                {
                    default_frwrd_dst[da_type].offset = destination;
                    default_frwrd_dst[da_type].destination = 0;
                }

                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_FORWARD_UNKNOWN_DA_PROFILE, entry_handle_id));
            }

            /** Calculate the flood groups. */
            SHR_IF_ERR_EXIT(dnx_port_flood_map_groups_from_default_frwrd_dst(unit, default_frwrd_dst, flood_groups));
        }
    }
    else
    {
        /*
         * In case of LIF floodinng
         */
        /** get local_in_lif */
        int in_lif_profile;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        in_lif_profile_info_t in_lif_profile_info;
        uint32 flags;

        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port, flags, &gport_hw_resources),
                                    _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /** get in_lif_profile from LIF table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) &in_lif_profile));

        in_lif_profile_info_t_init(unit, &in_lif_profile_info);
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));

        /** Calculate the flood groups. */
        SHR_IF_ERR_EXIT(dnx_port_flood_map_groups_from_default_frwrd_dst
                        (unit, in_lif_profile_info.ingress_fields.default_frwrd_dst, flood_groups));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_trunk_egress_pp_port_update(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 core,
    uint32 tm_port)
{
    uint32 pp_port = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the trunk`s pp port */
    SHR_IF_ERR_EXIT(dnx_trunk_pp_port_get(unit, trunk_id, core, &pp_port));

    /** Get the dbal entry from Sw state */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Update the dbal handle so that to commit the values that are retrieved from Sw State */
    SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, entry_handle_id));

    /*
     * Currently the pp_port has tm_port resolution and it should be updated accordingly
     * Commit the pp properties of the trunk pp port that are taken from the Sw state of EGRESS_PP_PORT
     * so that to update the hw tables that are per tm_port
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_port_pp_parsing_context_set(
    int unit,
    bcm_port_t port,
    uint32 parsing_context)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 entry_handle_id;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_1ST_PARSER_PARSER_CONTEXT, INST_SINGLE,
                                     parsing_context);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 *  Force Forward
 * {
 */
/**
 * \brief -
 *  Map pp-port to ingress trap and forward_strength
 *
 * \param [in] unit - The unit ID.
 * \param [in] core_id - core ID
 * \param [in] pp_port - pp port #
 * \param [in] trap_strength - trap strength
 * \param [in] snp_strength - snoop strength
 * \param [in] trap_id - ingress trap #
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *     None
 *
 * \see
 *   None
 */
static shr_error_e
dnx_port_force_forward_pp_port_to_ingress_trap_set(
    int unit,
    bcm_core_t core_id,
    int pp_port,
    uint32 trap_strength,
    uint32 snp_strength,
    uint32 trap_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** check if trap is enable for port */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE, trap_strength);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE, snp_strength);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, INST_SINGLE, trap_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get mapping of pp-port to ingress trap and forward_strength
 *
 * \param [in] unit - The unit ID.
 * \param [in] core_id - core ID
 * \param [in] pp_port - pp port #
 * \param [out] trap_strength - trap strength
 * \param [out] snoop_strength - snoop strength
 * \param [out] trap_id - ingress trap #
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *     None
 *
 * \see
 *   None
 */
static shr_error_e
dnx_port_force_forward_pp_port_to_ingress_trap_get(
    int unit,
    bcm_core_t core_id,
    int pp_port,
    uint32 *trap_strength,
    uint32 *snoop_strength,
    uint32 *trap_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** check if trap is enable for port */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE, trap_strength);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE, snoop_strength);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, INST_SINGLE, trap_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *  Get mapping from pp port to  trap_id and trap_index
 *
 * \param [in] unit - The unit ID.
 * \param [in] core_id - core ID
 * \param [in] pp_port - pp port #
 * \param [out] trap_index - the index of the trap in template manager and sw state or -1 if there is no trap.
 * \param [out] trap_id - the trap ID or -1 if there is no trap
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *     implemented using traps
 *
 * \see
 *   None
 */
static shr_error_e
dnx_port_force_forward_trap_get(
    int unit,
    bcm_core_t core_id,
    int pp_port,
    int *trap_index,
    uint32 *trap_id)
{
    uint32 trap_id_i;
    uint32 trap_strength, snoop_strength;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    *trap_index = -1;
    *trap_id = -1;

    /** check if trap is enable for port */
    SHR_IF_ERR_EXIT(dnx_port_force_forward_pp_port_to_ingress_trap_get
                    (unit, core_id, pp_port, &trap_strength, &snoop_strength, trap_id));

    if (trap_strength != 0)
    {
        /** look for trap index */
        for (index = 0; index < dnx_data_port.general.nof_pp_ports_get(unit); index++)
        {
            SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.trap_id.get(unit, core_id, index, &trap_id_i));
            if (trap_id_i == *trap_id)
            {
                *trap_index = index;
                break;
            }
        }
    }
    else
    {
        /** make sure trap id is marked as -1 in this case (trap_strength == 0)*/
        *trap_id = -1;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - verify input parameters for "bcm_dnx_port_force_forward_get()"
 */
static shr_error_e
dnx_port_force_forward_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_t * egr_port,
    int *enabled)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    algo_gpm_gport_verify_type_e allowed_port_types[] =
        { ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_TRUNK };
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /** verify ingress port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, port, COUNTOF(allowed_port_types), allowed_port_types));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport_info.local_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, gport_info.local_port, &port_type));
    if (!DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expected port type to be PP\n.");
    }

    /** verify egress port */
    SHR_NULL_CHECK(egr_port, _SHR_E_PARAM, "egr_port");

    /** verify enabled */
    SHR_NULL_CHECK(enabled, _SHR_E_PARAM, "enabled");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *  Get destination of force forward of any packet sent through port
 *
 * \param [in] unit - The unit ID.
 * \param [in] port - logical port / local port gport / LAG
 * \param [out] egr_port - Gport which represent a destination. (set to -1, when enable==0)
 *                         Supported gports: logical gport, local port gport, system port gport, mcast gport and unicast queue group gport
 * \param [out] enabled - enable or disable trapping
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *     implemented using traps
 *
 * \see
 *   None
 */
int
bcm_dnx_port_force_forward_get(
    int unit,
    bcm_port_t port,
    bcm_port_t * egr_port,
    int *enabled)
{
    bcm_core_t core_id;
    uint32 pp_port;
    int trap_index;
    uint32 trap_id;
    uint32 trap_strength, snoop_strength;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_port_force_forward_get_verify(unit, port, egr_port, enabled));

    /** get in pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    pp_port = gport_info.internal_port_pp_info.pp_port[0];
    core_id = gport_info.internal_port_pp_info.core_id[0];
    /** get trap_id + index in sw state*/
    SHR_IF_ERR_EXIT(dnx_port_force_forward_trap_get(unit, core_id, pp_port, &trap_index, &trap_id));

    /** enable */
    *enabled = (trap_id == -1) ? 0 : 1;

    /** destination */
    if (*enabled)
    {
        if (trap_index != -1)
        {
                /** trap that overrides destination created by bcm_dnx_port_force_forward_set() - read the destination */
            SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_get(unit, trap_id, &trap_config));
            if (trap_config.flags & BCM_RX_TRAP_DEST_MULTICAST)
            {
                *egr_port = trap_config.dest_group;
            }
            else
            {
                *egr_port = trap_config.dest_port;
            }
        }
        else
        {
                /** trap provided by the user - return trap gport */
            SHR_IF_ERR_EXIT(dnx_port_force_forward_pp_port_to_ingress_trap_get
                            (unit, core_id, pp_port, &trap_strength, &snoop_strength, &trap_id));
            BCM_GPORT_TRAP_SET(*egr_port, trap_id, trap_strength, snoop_strength);
        }
    }
    else
    {
        *egr_port = -1;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - verify input parameters for "bcm_dnx_port_force_forward_set()"
 */
static shr_error_e
dnx_port_force_forward_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_t egr_port,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    algo_gpm_gport_verify_type_e allowed_port_types[] =
        { ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_TRUNK
    };
    uint32 trap_data;
    SHR_FUNC_INIT_VARS(unit);

    /** verify ingress port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, port, COUNTOF(allowed_port_types), allowed_port_types));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    if (gport_info.internal_port_pp_info.nof_pp_ports == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expected port type to be PP\n.");
    }

    /** verify enable value */
    SHR_RANGE_VERIFY(enable, 0, 1, _SHR_E_PARAM, "Expected boolean value (0 or 1), got %d.\n", enable);

    /** verify that egr_port is valid destination */
    if (enable)
    {
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport(unit, 0, egr_port, &trap_data));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Remove trap_id and trap_index (sw and hw)
 *
 * \param [in] unit - The unit ID.
 * \param [in] core_id - core ID
 * \param [in] trap_index - the index of the trap in template manager and sw state.
 * \param [in] trap_id - the trap ID
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *     implemented using traps
 *
 * \see
 *   None
 */
static shr_error_e
dnx_port_force_forward_trap_remove(
    int unit,
    bcm_core_t core_id,
    int trap_index,
    uint32 trap_id)
{
    uint8 is_last;

    SHR_FUNC_INIT_VARS(unit);

    /** remove from template manager */
    SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.mngr.free_single(unit, core_id, trap_index, &is_last));

    /** check if it is the only trap with the same forward trap */
    if (is_last)
    {
        /** remove sw state mapping */
        SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.trap_id.set(unit, core_id, trap_index, 0));

        /** destroy trap */
        SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_destroy(unit, trap_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Force / Unforce  forward any packet sent through port to egr port
 *
 * \param [in] unit - The unit ID.
 * \param [in] port - logical port / local port gport / LAG
 * \param [in] egr_port - Gport which represent a destination. (set to -1, when enable==0)
 * *                      Supported gports: logical gport, local port gport, system port gport, mcast gport and unicast queue group gport
 * \param [in] enable - enable or disable trapping
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *     implemented using traps
 *
 * \see
 *   None
 */
int
bcm_dnx_port_force_forward_set(
    int unit,
    bcm_port_t port,
    bcm_port_t egr_port,
    int enable)
{
    uint32 trap_data;
    uint32 trap_id = 0, strength, snoop_strength;
    bcm_rx_trap_config_t trap_config;
    int trap_index;
    uint8 is_first;
    bcm_core_t core_id;
    int pp_port, port_index;
    int prev_trap_index;
    uint32 prev_trap_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_port_force_forward_set_verify(unit, port, egr_port, enable));

    /** iterate over in ports */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    for (port_index = 0; port_index < gport_info.internal_port_pp_info.nof_pp_ports; port_index++)
    {
        core_id = gport_info.internal_port_pp_info.core_id[port_index];
        pp_port = gport_info.internal_port_pp_info.pp_port[port_index];

        /*
         * Create trap
         */
        if (enable)
        {
            /** Get previous traps*/
            SHR_IF_ERR_EXIT(dnx_port_force_forward_trap_get(unit, core_id, pp_port, &prev_trap_index, &prev_trap_id));

            /** reuse traps with the same trap data - if exist, otherwise allocate new trap id */
            /** get trap data */
            SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport(unit, 0, egr_port, &trap_data));

            if (BCM_GPORT_IS_TRAP(egr_port))
            {
                /** use the given trap */
                trap_id = BCM_GPORT_TRAP_GET_ID(egr_port);
                strength = BCM_GPORT_TRAP_GET_STRENGTH(egr_port);
                snoop_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(egr_port);
            }
            else
            {
                /** create a trap that overrides destination */

                SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.
                                mngr.allocate_single(unit, core_id, 0, &trap_data, NULL, &trap_index, &is_first));

                if (is_first)
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, (int *) &trap_id));
                    /** set trap */
                    /** set forward action */
                    bcm_rx_trap_config_t_init(&trap_config);
                    trap_config.snoop_strength = 0;
                    trap_config.trap_strength = 0;

                    if (BCM_GPORT_IS_MCAST(egr_port))
                    {
                        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_DEST_MULTICAST | BCM_RX_TRAP_TRAP;
                        trap_config.dest_group = egr_port;
                    }
                    else
                    {
                        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
                        trap_config.dest_port = egr_port;
                    }

                    /** set the trap */
                    SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_set(unit, trap_id, &trap_config));

                    /*
                     * update sw state - mapping of trap_index to trap_id
                     */
                    SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.trap_id.set(unit, core_id, trap_index, trap_id));
                }
                else
                {
                    /** get trap id from sw state */
                    SHR_IF_ERR_EXIT(algo_port_pp_db.force_forward.trap_id.get(unit, core_id, trap_index, &trap_id));
                }

                strength = dnx_data_trap.strength.max_strength_get(unit);
                snoop_strength = 0;
            }

            /** map port to trap id */
            SHR_IF_ERR_EXIT(dnx_port_force_forward_pp_port_to_ingress_trap_set
                            (unit, core_id, pp_port, strength, snoop_strength, trap_id));

            /** remove previous trap */
            if (prev_trap_index != -1)
            {
                SHR_IF_ERR_EXIT(dnx_port_force_forward_trap_remove(unit, core_id, prev_trap_index, prev_trap_id));
            }
        }
        else
        {
            int default_trap_id;
            /** Get previous traps*/
            SHR_IF_ERR_EXIT(dnx_port_force_forward_trap_get(unit, core_id, pp_port, &prev_trap_index, &prev_trap_id));

            /** map pp_port to default_trap*/
            SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapDefault, &default_trap_id));
            strength = 0;
            snoop_strength = 0;
            SHR_IF_ERR_EXIT(dnx_port_force_forward_pp_port_to_ingress_trap_set
                            (unit, core_id, pp_port, strength, snoop_strength, default_trap_id));

            /** remove previous trap */
            if (prev_trap_index != -1)
            {
                SHR_IF_ERR_EXIT(dnx_port_force_forward_trap_remove(unit, core_id, prev_trap_index, prev_trap_id));
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Force Forward
 * }
 */
