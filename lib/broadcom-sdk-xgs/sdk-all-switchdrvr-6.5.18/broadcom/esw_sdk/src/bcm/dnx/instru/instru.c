/** \file instru.c
 * $Id$
 *
 * Visibility procedures for DNX in instrumentation module.
 *
 * This file contains functions for visibility configuration.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU
/*
 * Include
 * {
 */
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/pll/pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_instru.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_u64.h>

#include <bcm_int/dnx/instru/instru_ipt.h>
#include <bcm_int/dnx/instru/instru.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm/types.h>
#include <bcm/instru.h>
#include <bcm_int/dnx/lif/lif_lib.h>

#include "instru_internal.h"

/*
 * }
 */

/*
 * Function Declaration.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */
#define INSTRU_IFA_OAM_LIF_SET 1
 /*
  * }
  */

 /*
  * Internal functions.
  * {
  */

 /*
  * }
  */

/**
 * \brief - verify procedure for dnx_instru_trace_probability_get()
 */
static shr_error_e
dnx_instru_trace_probability_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int *percentage)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags provided %d", flags);
    }

    /** null check */
    SHR_NULL_CHECK(percentage, _SHR_E_PARAM, "percentage");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for dnx_instru_trace_probability_set()
 */
static shr_error_e
dnx_instru_trace_probability_set_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int percentage)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags provided %d", flags);
    }

    if (percentage < 0 || percentage > 1000)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid percentage value was provided %d", percentage);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port trace probability, the returned probability is given in units of one-tenth percentage.
 */
static shr_error_e
dnx_instru_trace_probability_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int *percentage)
{
    uint32 entry_handle_id, field32;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_trace_probability_get_verify(unit, flags, gport, percentage));

    /** Get Port + Core */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &gport_info));

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

    /** set keys - pp port and core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);

    /** request trace probability */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRACE_PROBABILITY, INST_SINGLE, &field32);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * calculate percentage in units of one-tenth from HW field.
     * The HW formula should be as following:
     *
     * (MAX_FIELD_VAL - hw_field)
     * -------------------------- x 1000
     *        MAX_FIELD_VAL
     */
    {
        int max_field_val;

        SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, DBAL_TABLE_INGRESS_PP_PORT, DBAL_FIELD_TRACE_PROBABILITY,
                                                        FALSE, 0, 0, &max_field_val));

        *percentage = ((max_field_val - field32) * 1000) / max_field_val;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set port trace probability, the provided probability is given in units of one-tenth percentage.
 */
static shr_error_e
dnx_instru_trace_probability_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int percentage)
{
    uint32 probability;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_trace_probability_set_verify(unit, flags, gport, percentage));

    /**
     * calculate HW probability from percentage given in units of one tenth.
     * The HW formula should be as following:
     *
     * (1000 - percentage)
     * -------------------- x MAX_FIELD_VAL
     *        1000
     */
    {
        int max_field_val;

        SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, DBAL_TABLE_INGRESS_PP_PORT, DBAL_FIELD_TRACE_PROBABILITY,
                                                        FALSE, 0, 0, &max_field_val));

        probability = ((1000 - percentage) * max_field_val) / 1000;
    }

    /** Set probability to HW */
    {
        uint32 entry_handle_id;
        dnx_algo_gpm_gport_phy_info_t gport_info;
        int pp_port_index;

        /** Get Port + Core */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /** create handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));

        /** iterate over pp port and commit to HW */
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            /** set keys - pp port and core */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);

            /** set value - probability */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRACE_PROBABILITY, INST_SINGLE, probability);

            /** commit */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for configuring various general instrumentation controls
 *
 * Use cases:
 *
 * ** Use case: "IPT SWITCH ID"
 * Set Switch ID for IPT (INT/Tail-Edit) applications
 *  Parameters:
 *  - flags - 0
 *  - type - bcmInstruControlIptSwitchId
 *  - arg - switch id
 *
 *
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - description per use case.
 * \param [in] type - description per use case.
 * \param [in] arg  - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_instru_control_set(
    int unit,
    uint32 flags,
    bcm_instru_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    switch (type)
    {
        case bcmInstruControlIptSwitchId:
            SHR_IF_ERR_EXIT(dnx_instru_ipt_switch_id_set(unit, arg));
            break;
        case bcmInstruControlVisMode:
        {
            /*
             * For VisMode arg is a mode and flags are not in use currently
             */
            bcm_instru_vis_mode_control_t mode = (bcm_instru_vis_mode_control_t) arg;
            SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, mode));
            break;
        }
        case bcmInstruControlVisSampling:
            /*
             * For VisSampling flags is direction, arg is period
             */
            SHR_IF_ERR_EXIT(dnx_visibility_sampling_set(unit, flags, (uint32) arg));
            break;
        case bcmInstruControlSFlowSamplingRate:
            SHR_IF_ERR_EXIT(dnx_instru_sflow_sampling_rate_set(unit, arg));
            break;
        case bcmInstruControlSFlowAgentIPAddress:
            SHR_IF_ERR_EXIT(dnx_instru_sflow_agent_id_address_set(unit, (bcm_ip_t) arg));
            break;
        case bcmInstruControlSFlowUpTime:
            SHR_IF_ERR_EXIT(dnx_instru_sflow_uptime_set(unit, arg));
            break;
        case bcmInstruControlSFlowMyRouterASNumber:
            SHR_IF_ERR_EXIT(dnx_instru_sflow_myrouter_as_number_set(unit, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_instru_control_set - invalid type=%d\r\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for getting configuration of general instrumentation controls
 * For detailed description refer to \ref bcm_dnx_instru_control_set()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - description per use case.
 * \param [in] type - description per use case.
 * \param [out] arg  - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_instru_control_get(
    int unit,
    uint32 flags,
    bcm_instru_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    switch (type)
    {
        case bcmInstruControlIptSwitchId:
            SHR_IF_ERR_EXIT(dnx_instru_ipt_switch_id_get(unit, arg));
            break;
        case bcmInstruControlVisMode:
        {
            /*
             * For VisMode arg is a pointer to the mode and flags are not in use currently
             */
            bcm_instru_vis_mode_control_t mode;
            SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &mode, TRUE));
            *arg = (int) mode;
            break;
        }
        case bcmInstruControlVisSampling:
            SHR_IF_ERR_EXIT(dnx_visibility_sampling_get(unit, flags, (uint32 *) arg));
            break;
        case bcmInstruControlSFlowSamplingRate:
            SHR_IF_ERR_EXIT(dnx_instru_sflow_sampling_rate_get(unit, arg));
            break;
        case bcmInstruControlSFlowAgentIPAddress:
            SHR_IF_ERR_EXIT(dnx_instru_sflow_agent_id_address_get(unit, (bcm_ip_t *) arg));
            break;
        case bcmInstruControlSFlowUpTime:
            SHR_IF_ERR_EXIT(dnx_instru_sflow_uptime_get(unit, arg));
            break;
        case bcmInstruControlSFlowMyRouterASNumber:
            SHR_IF_ERR_EXIT(dnx_instru_sflow_myrouter_as_number_get(unit, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_instru_control_get - invalid type=%d\r\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for configuring various per gport instrumentation controls
 *
 * Use cases:
 *
 * ** Use case: "Trace Probability"
 * Set trace probability
 *  Parameters:
 *  - flags - 0
 *  - gport - port to set probability for.
 *  - type - bcmInstruGportControlTraceProbability
 *  - arg - probability in units of one-tenth (for example arg=300 means 30% probability)
 *
 *
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - description per use case.
 * \param [in] flags - description per use case.
 * \param [in] type - description per use case.
 * \param [in] arg  - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_instru_gport_control_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_instru_gport_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    switch (type)
    {
        case bcmInstruGportControlTraceProbability:
            SHR_IF_ERR_EXIT(dnx_instru_trace_probability_set(unit, flags, gport, arg));
            break;
        case bcmInstruGportControlVisEnable:
            SHR_IF_ERR_EXIT(dnx_visibility_port_enable_set(unit, flags, gport, arg));
            break;
        case bcmInstruGportControlVisForce:
            SHR_IF_ERR_EXIT(dnx_visibility_port_force_set(unit, flags, gport, arg));
            break;
        case bcmInstruGportControlIptTrapToRcyEnable:
            SHR_IF_ERR_EXIT(dnx_instru_ipt_trap_to_rcy_set(unit, flags, gport, arg));
            break;
        case bcmInstruGportControlIfaInitiatorRcyEnable:
            SHR_IF_ERR_EXIT(dnx_instru_ifa_initiator_rcy_set(unit, flags, gport, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_instru_gport_control_set - invalid type=%d\r\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for getting configuration of per gport instrumentation controls
 * For detailed description refer to \ref bcm_dnx_instru_gport_control_set()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - description per use case.
 * \param [in] flags - description per use case.
 * \param [in] type - description per use case.
 * \param [out] arg  - description per use case.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_instru_gport_control_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_instru_gport_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    switch (type)
    {
        case bcmInstruGportControlTraceProbability:
            SHR_IF_ERR_EXIT(dnx_instru_trace_probability_get(unit, flags, gport, arg));
            break;
        case bcmInstruGportControlVisEnable:
            SHR_IF_ERR_EXIT(dnx_visibility_port_enable_get(unit, flags, gport, arg));
            break;
        case bcmInstruGportControlVisForce:
            SHR_IF_ERR_EXIT(dnx_visibility_port_force_get(unit, flags, gport, arg));
            break;
        case bcmInstruGportControlIptTrapToRcyEnable:
            SHR_IF_ERR_EXIT(dnx_instru_ipt_trap_to_rcy_get(unit, flags, gport, arg));
            break;
        case bcmInstruGportControlIfaInitiatorRcyEnable:
            SHR_IF_ERR_EXIT(dnx_instru_ifa_initiator_rcy_get(unit, flags, gport, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_instru_gport_control_get - invalid type=%d\r\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set encap lif for IFA counting
 *
 * \param [in] unit - Relevant unit
 * \param [in] local_outlif - Local sFlow encap lif
 * \param [in] counter_type - the counter_type to be stamped on metadata.
 * \param [in] dbal_result_type - the EEDB SFLOW result type.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ifa_encap_eedb_write(
    int unit,
    int local_outlif,
    uint16 counter_type,
    uint32 dbal_result_type)
{
    uint32 entry_handle_id;
    uint32 entry_data[3];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_outlif);

    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);

    switch (dbal_result_type)
    {
        case DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND:
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE,
                                         INSTRU_IFA_OAM_LIF_SET);

            /*
             * Set entry raw data.
             * entry_data[0] : const 0.
             * entry_data[1] : const 0.
             * entry_data[2] : counter_type (MSBs).
             */
            entry_data[0] = 0;
            entry_data[1] = 0;
            entry_data[2] = counter_type << 24;

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Unsupported result type: dbal_result_type = %d. Supported result_type is: DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND = %d\n",
                         dbal_result_type, DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND);
        }
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, entry_data);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Parameters verification function used for 
 * bcm_dnx_instru_ifa_encap_create
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ifa_info - ifa configuration structure
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_instru_ifa_encap_create_verify(
    int unit,
    bcm_instru_ifa_info_t * ifa_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((ifa_info->flags & BCM_INSTRU_IFA_ENCAP_REPLACE) == BCM_INSTRU_IFA_ENCAP_REPLACE)
    {
        if (bcm_instru_ifa_encap_get(unit, ifa_info) == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "REPLACE flag must be used on existing IFA entity.\n");
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Allocate out-lif and add oam entry for counting IFA packets
 *
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ifa_info - ifa configuration structure
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_instru_ifa_encap_create(
    int unit,
    bcm_instru_ifa_info_t * ifa_info)
{
    uint32 entry_handle_id;
    int local_out_lif;
    lif_mngr_local_outlif_info_t outlif_info;
    int lif_alloc_flags = 0, global_lif_id = 0;
    uint32 result_type;
    int ifa_metadata_counter_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Counter Type = 0 
     */
    ifa_metadata_counter_type = 0;

    result_type = DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND;
    /*
     * Allocate LIF for IFA encap entry.
     */
    if (_SHR_IS_FLAG_SET(ifa_info->flags, BCM_INSTRU_IFA_ENCAP_WITH_ID))
    {
        /*
         * If WITH_ID flag is used - get the global out-lif from the user input and
         * add alloc_with_id flag
         */
        global_lif_id = ifa_info->ifa_encap_id;
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }
    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_SFLOW;
    outlif_info.dbal_result_type = result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SFLOW;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));
    local_out_lif = outlif_info.local_outlif;

    /*
     * Return the allocated global lif 
     */
    ifa_info->ifa_encap_id = global_lif_id;

    /*
     * ETPS Configuration 
     */
    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_eedb_write(unit, local_out_lif, ifa_metadata_counter_type, result_type));

    /*
     * Add entry to glem 
     */
    if ((ifa_info->flags & BCM_INSTRU_IFA_ENCAP_REPLACE) != BCM_INSTRU_IFA_ENCAP_REPLACE)
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif_id, local_out_lif));
    }

    /*
     ******************** */
    /*
     * Add OAM_LIF_DB entry 
     */
    /*
     ******************** */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ZERO_PADDING, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, local_out_lif);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE, ifa_info->stat_cmd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                 ifa_info->counter_command_id);

    if ((ifa_info->flags & BCM_INSTRU_IFA_ENCAP_REPLACE) == BCM_INSTRU_IFA_ENCAP_REPLACE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify ifa encap is indeed exists and it is with the correct table/result_type/phase
 *
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ifa_encap_id - global lif
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ifa_encap_id_verify(
    int unit,
    int ifa_encap_id)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Turn the global lif into a tunnel, then call gport to hw resources.
     */
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, ifa_encap_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    if ((hw_res.outlif_dbal_table_id != DBAL_TABLE_EEDB_SFLOW)
        || (hw_res.outlif_dbal_result_type != DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND)
        || (hw_res.outlif_phase != LIF_MNGR_OUTLIF_PHASE_SFLOW))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! IFA encap 0x%x is not of type IFA, table_id = %d, result_type = %d, outlif_phase = %d\n",
                     ifa_encap_id, hw_res.outlif_dbal_table_id, hw_res.outlif_dbal_result_type, hw_res.outlif_phase);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify ifa entity get/delete request
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ifa_info - ifa configuration structure
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ifa_encap_get_delete_verify(
    int unit,
    bcm_instru_ifa_info_t * ifa_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ifa_info, _SHR_E_PARAM, "ifa_info");

    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_id_verify(unit, ifa_info->ifa_encap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Given an instru global lif, returns the local lif and it's result type.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ifa_encap_id - global lif
 * \param [out] local_lif - local lif associated with the given global_lif
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ifa_encap_id_to_local_lif(
    int unit,
    int ifa_encap_id,
    int *local_lif)
{
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t encap_in_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Turn the global lif into a tunnel, then call gport to hw resources.
     */
    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    BCM_GPORT_TUNNEL_ID_SET(encap_in_tunnel, ifa_encap_id);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, encap_in_tunnel,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &hw_res));

    *local_lif = hw_res.local_out_lif;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear ifa EEDB entry
 *
 * \param [in] unit - Relevant unit
 * \param [in] ifa_local_outlif - the ifa lif id, key to the dbal table
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ifa_encap_clear(
    int unit,
    int ifa_local_outlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SFLOW, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, ifa_local_outlif);
    /** Set RESULT_TYPE field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_SFLOW_ETPS_SFLOW_EXT_SECOND);

    /** Clear dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * ifa lif delete
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ifa_info - holds ifa_encap_id for the ifa entity to be deleted
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_instru_ifa_lif_delete(
    int unit,
    bcm_instru_ifa_info_t * ifa_info)
{
    int local_lif;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get local_lif 
     */
    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_id_to_local_lif(unit, ifa_info->ifa_encap_id, &local_lif));

    /** Remove global lif from GLEM */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, ifa_info->ifa_encap_id));

    /*
     * delete ifa encap entry 
     */
    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_clear(unit, local_lif));

    /*
     * Delete global and local lifs. 
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, ifa_info->ifa_encap_id, NULL, local_lif));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Generic API function used for configuration of IFA
 * instrumentation.
 * \ref bcm_dnx_instru_ifa_encap_get() 
 *      bcm_dnx_instru_ifa_encap_delete()
 *      bcm_dnx_instru_ifa_encap_traverse()  
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ifa_info -  ifa configuration structure
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   bcm_dnx_instru_ifa_encap_get
 *   bcm_dnx_instru_ifa_encap_delete
 *   bcm_dnx_instru_ifa_encap_traverse
 */
int
bcm_dnx_instru_ifa_encap_create(
    int unit,
    bcm_instru_ifa_info_t * ifa_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_create_verify(unit, ifa_info));

    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_create(unit, ifa_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * API for deleting IFA 1.0 entity 
 * 
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ifa_info - ifa configuration structure
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   bcm_dnx_instru_ifa_encap_create
 *   bcm_dnx_instru_ifa_encap_get
 *   bcm_dnx_instru_ifa_encap_traverse
 */
int
bcm_dnx_instru_ifa_encap_delete(
    int unit,
    bcm_instru_ifa_info_t * ifa_info)
{
    uint32 entry_handle_id;
    int local_lif;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_id_to_local_lif(unit, ifa_info->ifa_encap_id, &local_lif));

    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_get_delete_verify(unit, ifa_info));

    SHR_IF_ERR_EXIT(dnx_instru_ifa_lif_delete(unit, ifa_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, local_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * API for getting IFA 1.0 entity info
 * 
 *
 * \param [in] unit -  Unit-ID
 * \param [in,out] ifa_info - ifa_encap_id should be supplied as
 *        input. Structure will contain all information as
 *        output
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   bcm_dnx_instru_ifa_encap_create
 *   bcm_dnx_instru_ifa_encap_delete
 *   bcm_dnx_instru_ifa_encap_traverse
 */
int
bcm_dnx_instru_ifa_encap_get(
    int unit,
    bcm_instru_ifa_info_t * ifa_info)
{
    uint32 entry_handle_id;
    uint8 counter_if;
    uint32 counter_index;
    int local_lif;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_id_to_local_lif(unit, ifa_info->ifa_encap_id, &local_lif));

    SHR_IF_ERR_EXIT(dnx_instru_ifa_encap_get_delete_verify(unit, ifa_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, local_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE, &counter_index);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &counter_if);
    ifa_info->stat_cmd = counter_index;
    ifa_info->counter_command_id = counter_if;

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * API for traversing IFA 1.0 entity
 * 
 *
 * \param [in] unit -  Unit-ID
 * \param [in] cb - Call back function
 * \param [in] user_data - Pointer to user data structure
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   bcm_dnx_instru_ifa_encap_create
 *   bcm_dnx_instru_ifa_encap_get
 *   bcm_dnx_instru_ifa_encap_delete
 */
int
bcm_dnx_instru_ifa_encap_traverse(
    int unit,
    bcm_instru_ifa_encap_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end = 0;
    bcm_instru_ifa_info_t ifa_info;
    uint32 *field_value;
    uint8 counter_if;
    uint32 counter_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    field_value = NULL;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        /*
         * Receive key and value fields of the entry.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE,
                                                              field_value));
        ifa_info.ifa_encap_id = field_value[0];

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE, &counter_index));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &counter_if));
        ifa_info.stat_cmd = counter_index;
        ifa_info.counter_command_id = counter_if;

        /*
         * If user provided a name of the callback function, it will be invoked with sending the ifa_info structure
         * of the entry that was found.
         */
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &ifa_info, user_data));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong callback function was provided");
        }
        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_instru_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Init IPT (INT + Tail-Edit) */
    SHR_IF_ERR_EXIT(dnx_instru_ipt_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Function to verify the input parameters of APIs:
 * - bcm_instru_synced_counters_config_set
 * - bcm_instru_synced_counters_config_get
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  Flags
 * \param [in] source_type -  Type of Synchronous Counters mechanism
 * \param [in] config -  Configuration structure for Synchronous Counters mechanism
 * \param [in] is_set -  indicates if the verify function was called from the "set" API
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_instru_synced_counters_config_verify(
    int unit,
    uint32 flags,
    bcm_instru_synced_counters_source_type_t source_type,
    bcm_instru_synced_counters_config_t * config,
    uint8 is_set)
{
    dnxcmn_time_t time_src;
    dnxcmn_time_t time_dest;
    uint32 interval_period_nof_bits;
    uint64 immediate_start;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify input flags */
    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "Unsupported flags for synchronized counters configuration!\n");

    /** Verify input source type */
    if ((source_type != bcmInstruSyncedCountersTypeIcgm) && (source_type != bcmInstruSyncedCountersTypeNif))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported source type %d for synchronized counters configuration!\n",
                     source_type);
    }

    if (is_set == TRUE)
    {
        /** Verify input enable */
        SHR_BOOL_VERIFY(config->enable, _SHR_E_PARAM, "Invalid enable, must be TRUE or FALSE\n");
        if (config->enable == FALSE)
        {
            /** When disabling the instrumentation all other parameters are irrelevant */
            SHR_EXIT();
        }

        /** Verify input start time */
        COMPILER_64_SET(immediate_start, -1, -1);
        if (COMPILER_64_NE(config->start_time, immediate_start))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported start time currently only immediate (-1) is supported!\n");
        }

        /** Verify input period */
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, DBAL_FIELD_INTERVAL_PERIOD, &interval_period_nof_bits));
        /** In HW the value is in nanoseconds, while the user input is in miliseconds. Max value is 64 bits */
        COMPILER_64_MASK_CREATE(time_src.time, interval_period_nof_bits, 0);
        time_src.time_units = DNXCMN_TIME_UNIT_NSEC;
        SHR_IF_ERR_EXIT(dnxcmn_time_units_convert(unit, &time_src, DNXCMN_TIME_UNIT_MILISEC, &time_dest));
        if (COMPILER_64_GT(config->period, time_dest.time))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Period is out of range, minimum is 0 and maximum is %u:%u!\n",
                         COMPILER_64_HI(time_dest.time), COMPILER_64_LO(time_dest.time));
        }

        /** Verify input enable */
        SHR_BOOL_VERIFY(config->enable, _SHR_E_PARAM, "Invalid enable, must be TRUE or FALSE\n");
        /** Verify input is_eventor_collection */
        SHR_BOOL_VERIFY(config->is_eventor_collection, _SHR_E_PARAM,
                        "Invalid is_eventor_collection, must be TRUE or FALSE\n");
        /** Verify input is_continuous */
        SHR_BOOL_VERIFY(config->is_continuous, _SHR_E_PARAM, "Invalid is_continuous, must be TRUE or FALSE\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set Synced counters configuration to HW
 */
static shr_error_e
dnx_instru_synced_counters_config_hw_set(
    int unit,
    bcm_instru_synced_counters_source_type_t source_type,
    uint8 enable,
    uint64 start_time,
    uint64 interval_period,
    uint16 nof_intervals,
    uint8 is_continuous)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_TYPE, source_type);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_START_TIME, INST_SINGLE, start_time);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_INTERVAL_PERIOD, INST_SINGLE, interval_period);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NOF_INTERVALS, INST_SINGLE, nof_intervals);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTINUOUS, INST_SINGLE, is_continuous);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Get Synced counters configuration from HW
 */
static shr_error_e
dnx_instru_synced_counters_config_hw_get(
    int unit,
    bcm_instru_synced_counters_source_type_t source_type,
    uint8 *enable,
    uint64 *start_time,
    uint64 *interval_period,
    uint16 *nof_intervals,
    uint8 *is_continuous)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_TYPE, source_type);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_START_TIME, INST_SINGLE, start_time);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_INTERVAL_PERIOD, INST_SINGLE, interval_period);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_NOF_INTERVALS, INST_SINGLE, nof_intervals);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_CONTINUOUS, INST_SINGLE, is_continuous);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * API to configure and enable/disable synchronous instrumentation counters per type
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  Flags
 * \param [in] source_type -  Type of Synchronous Counters mechanism
 * \param [in] config -  Configuration structure for Synchronous Counters mechanism
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_instru_synced_counters_config_set(
    int unit,
    uint32 flags,
    bcm_instru_synced_counters_source_type_t source_type,
    bcm_instru_synced_counters_config_t * config)
{
    uint8 enable = 0;
    uint8 is_continuous = 0;
    uint64 start_time;
    uint64 interval_period;
    uint16 nof_intervals = 0;
    uint64 timestamp;
    uint64 immediate_start;
    dnxcmn_time_t time_src;
    dnxcmn_time_t time_dest;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    COMPILER_64_ZERO(start_time);
    COMPILER_64_ZERO(interval_period);
    COMPILER_64_ZERO(timestamp);

    /** Verify input parameters */
    SHR_IF_ERR_EXIT(dnx_instru_synced_counters_config_verify(unit, flags, source_type, config, TRUE));

    enable = (config->enable == 0) ? FALSE : TRUE;

    if (enable == TRUE)
    {
        /** Set start time */
        COMPILER_64_SET(immediate_start, -1, -1);
        if (COMPILER_64_NE(config->start_time, immediate_start))
        {
            SHR_IF_ERR_EXIT(soc_dnx_pll_timestamp_get(unit, 1, &timestamp));
            /*
             * Add one ms to the current timestamp to compensate the delay for setting it to HW
             * Timestamp is in nanoseconds, so convert 1 ms to ns and add it.
             */
            COMPILER_64_SET(time_src.time, 0, 1);
            time_src.time_units = DNXCMN_TIME_UNIT_MILISEC;
            SHR_IF_ERR_EXIT(dnxcmn_time_units_convert(unit, &time_src, DNXCMN_TIME_UNIT_NSEC, &time_dest));
            COMPILER_64_ADD_64(timestamp, time_dest.time);
            COMPILER_64_COPY(start_time, timestamp);
        }

        /** Set interval period */
        COMPILER_64_COPY(interval_period, config->period);

        /** Set number of intervals - eventor collection nof_interval=1, otherwise, nof_interval=max */
        nof_intervals =
            (config->is_eventor_collection == TRUE) ? 1 : dnx_data_instru.synced_counters.max_nof_intervals_get(unit);

        is_continuous = (config->enable == 0) ? FALSE : TRUE;
    }

    /** Set the Synced counters configuration to HW */
    SHR_IF_ERR_EXIT(dnx_instru_synced_counters_config_hw_set
                    (unit, source_type, enable, start_time, interval_period, nof_intervals, is_continuous));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * API to get synchronous instrumentation counters configuration per type
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  Flags
 * \param [in] source_type -  Type of Synchronous Counters mechanism
 * \param [out] config -  Configuration structure for Synchronous Counters mechanism
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_instru_synced_counters_config_get(
    int unit,
    uint32 flags,
    bcm_instru_synced_counters_source_type_t source_type,
    bcm_instru_synced_counters_config_t * config)
{
    uint8 enable;
    uint64 start_time;
    uint64 interval_period;
    uint16 nof_intervals;
    uint8 is_continuous;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    COMPILER_64_ZERO(start_time);
    COMPILER_64_ZERO(interval_period);

    /** Verify input parameters */
    SHR_IF_ERR_EXIT(dnx_instru_synced_counters_config_verify(unit, flags, source_type, config, FALSE));

    /** Get the Synced counters configuration from HW */
    SHR_IF_ERR_EXIT(dnx_instru_synced_counters_config_hw_get
                    (unit, source_type, &enable, &start_time, &interval_period, &nof_intervals, &is_continuous));

    /** Get start time */
    COMPILER_64_COPY(config->start_time, start_time);

    /** Get interval period */
    COMPILER_64_COPY(config->period, interval_period);

    /** Get is_eventor_collection - eventor collection nof_interval=1, otherwise, nof_interval=max */
    config->is_eventor_collection = (nof_intervals == 1) ? TRUE : FALSE;

    /** Get is_continuous */
    config->is_continuous = is_continuous;

exit:
    SHR_FUNC_EXIT;
}
