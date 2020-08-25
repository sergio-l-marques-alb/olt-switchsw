/** \file instru.c
 * $Id$
 *
 * Visibility procedures for DNX in instrumentation module.
 *
 * This file contains functions for visibility configuration.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/instru/instru_ipt.h>
#include <bcm_int/dnx/instru/instru.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include "instru_internal.h"
#include <bcm/types.h>

/*
 * }
 */

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/instru.h>

/*
 * }
 */

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
 * \brief - The API sets the visibility mode
 *
 * \param [in] unit - Relevant unit
 * \param [out] mode - visibility mode enum
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_instru_vis_mode_set(
    int unit,
    bcm_vis_mode_t mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_instru_vis_mode_set not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The API sets the visibility mode
 *
 * \param [in] unit - Relevant unit
 * \param [out] mode - visibility mode enum
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_instru_vis_mode_get(
    int unit,
    bcm_vis_mode_t * mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_instru_vis_mode_set not supported ");

exit:
    SHR_FUNC_EXIT;
}

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
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_dnx_instru_gport_control_get - invalid type=%d\r\n", type);
    }

exit:
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
