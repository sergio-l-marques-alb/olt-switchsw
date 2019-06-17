/** \file utilex_seq.c
 *
 * Provide the a utlity that helps to run a sequence backward and forward.
 * Adittional tools provided:
 * * Sequence Logging
 * * Time tracking
 * * Time testing
 * * Tests for memory leaks
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/utilex/utilex_seq.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_time_analyzer.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/mem_measure_tool.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_ha.h>
#endif

#include <soc/drv.h>
/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
utilex_seq_t_init(
    int unit,
    utilex_seq_t * seq)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(seq, _SHR_E_INTERNAL, "seq");

    sal_memset(seq, 0, sizeof(utilex_seq_t));
    seq->first_step = UTILEX_SEQ_STEP_INVALID;
    seq->last_step = UTILEX_SEQ_STEP_INVALID;
    seq->control_prefix = NULL;
    seq->log_severity = bslSeverityNormal;
    sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "SEQ:");
    seq->last_passed_step = -1;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_step_t_init(
    int unit,
    utilex_seq_step_t * step)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(step, _SHR_E_INTERNAL, "step");

    sal_memset(step, 0, sizeof(utilex_seq_step_t));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - counts the length og step list
 * \param [in] unit - unit #.
 * \param [in] step_list - pointer to a list of steps.
 * \param [in] nof_steps - pointer to the required number of steps.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_nof_steps_get(
    int unit,
    utilex_seq_step_t * step_list,
    int *nof_steps)
{
    int step_index;
    SHR_FUNC_INIT_VARS(unit);

    /** every list must have at least one member - the last one - with step_id == UTILEX_SEQ_STEP_LAST */
    for (step_index = 0; step_list[step_index].step_id != UTILEX_SEQ_STEP_LAST; step_index++)
    {
        /** Do nothing */
    }

    *nof_steps = step_index;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify 'utilex_seq_run()' Input
 *
 */
static shr_error_e
utilex_seq_run_verify(
    int unit,
    utilex_seq_t * seq,
    int forward)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(seq, _SHR_E_INTERNAL, "seq");
    SHR_NULL_CHECK(seq->log_prefix, _SHR_E_INTERNAL, "seq->log_prefix");
    SHR_NULL_CHECK(seq->step_list, _SHR_E_INTERNAL, "seq->step_list");

    /**
     * check that MEM_LEAK and TIME_STAMP flags are not used when
     * PARTIAL_INIT is used.
     */
    if ((seq->mem_test_en || seq->time_test_en) &&
        (seq->first_step != UTILEX_SEQ_STEP_INVALID || seq->last_step != UTILEX_SEQ_STEP_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Running time or memory test is not supported on partial sequence\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Managment function for running step list (forward
 *        direction).
 *          The resulted info will be added to total_flags
 *
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in] step - ptr to relevant step
 * \param [in,out] total_flags - flags relevant to this step
 * \param [in,out] is_first_done - assign to one if first done (this step or before)
 * \param [in,out] is_last_done - assign to one if last done (this step or before)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_managment_logic_forward(
    int unit,
    utilex_seq_t * seq,
    utilex_seq_step_t * step,
    uint32 *total_flags,
    int *is_first_done,
    int *is_last_done)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Am I First? */
    if (step->step_id == seq->first_step || seq->first_step == UTILEX_SEQ_STEP_INVALID)
    {
        *is_first_done = 1;
    }

    /** Is first NOT done and NOT a list */
    if (*is_first_done == 0 && step->step_sub_array == NULL)
    {
        /** Skip this step */
        *total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Is last done? */
    if (*is_last_done == 1)
    {
        /** Skip this step */
        *total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Am I Last? */
    if (step->step_id == seq->last_step)
    {
        *is_last_done = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to check weather a step is enabled/disabled
 *        according to SOC Property.
 *
 * \param [in] unit - unit #
 * \param [in] log_severity - logging severity for the shell
 *        output
 * \param [in] contol_prefix - prefix of the SOC property,
 *        common for the entire list
 * \param [in] current_step - pointer to the structure with the
 *        parameters of the step
 * \param [in] defl - default value for the SOC Property
 * \param [out] is_enabled - is the step enabled
 *
 * \return
 *  See shr_error_e
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_step_enabled(
    int unit,
    uint32 log_severity,
    char *contol_prefix,
    utilex_seq_step_t * current_step,
    int defl,
    int *is_enabled)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_enabled = soc_property_suffix_num_get(unit, 0, contol_prefix, current_step->soc_prop_suffix, defl);

    SHR_FUNC_EXIT;
}

/**
 * \brief - function to run a list of steps (forward direction)
 *
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in,out] depth - recursive list depth
 * \param [in,out] step_list - ptr to relevant step list
 * \param [out] time_overall - total time it took to complete
 *        the step list
 * \param [out] swstate_overall - total memory allocated for SW
 *        state for the step list
 * \param [out] memory_overall - Non-SW state Memory allocated,
 *        excluding for SW state, for the step list
 * \param [in,out] is_first_done - assign to one if first done (this step or before)
 * \param [in,out] is_last_done - assign to one if last done (this step or before)
 *
 * \return
 *  See shr_error_e
 * \remark
 *   the function is also responsible for making decisions regarding
 *   time stamping, memory leak detections and skipping steps according
 *   to seq and step_list.
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_run_step_list_forward(
    int unit,
    utilex_seq_t * seq,
    int depth,
    utilex_seq_step_t * step_list,
    sal_time_t * time_overall,
    uint32 *swstate_overall,
    uint32 *memory_overall,
    int *is_first_done,
    int *is_last_done)
{
    int step_index;
    int dynamic_flags = 0;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step;
    int is_enabled = 1;
    int defl;
    int depth_index;
    uint32 log_severity;
    uint32 swstate_allocated;
    unsigned long alloc_start;
    unsigned long free_start;
    unsigned long alloc_end;
    unsigned long free_end;
    char measurment_handle[30];
    char seq_prefix[20];
    uint32 time_elapsed;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        uint32 total_flags = 0;
        current_step = &(step_list[step_index]);

        /** init internal_info */
        sal_memset(&(current_step->internal_info), 0, sizeof(utilex_seq_step_internal_t));

        /** Save time stamp and mem allocation at beginning of step */
        if (seq->time_log_en || seq->time_test_en)
        {
            current_step->internal_info.time_stamp_start = sal_time_usecs();
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the beginning of step */
            sal_get_alloc_counters(&(alloc_start), &(free_start));
        }

        /** Start the memory allocation measurement for SW state in the beginning of step */
        if (seq->swstate_log_en)
        {
            utilex_str_replace_whitespace(seq_prefix, seq->log_prefix);
            sal_sprintf(measurment_handle, "Utilex%s%d", seq_prefix, current_step->step_id);
            SHR_IF_ERR_EXIT(memory_consumption_start_measurement_dnx(measurment_handle));
        }

        /** Run Flag function if exists */
        total_flags |= current_step->static_flags;
        if (current_step->dyn_flags != NULL)
        {
            dynamic_flags = 0;
            SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
            total_flags |= dynamic_flags;
        }

        /** Decisions made by the management logic are stronger then ones made by the flag cb function  */
        SHR_IF_ERR_EXIT(utilex_seq_managment_logic_forward
                        (unit, seq, current_step, &total_flags, is_first_done, is_last_done));

        /** Logging severity */
        log_severity = BSL_SEVERITY_ENC(seq->log_severity);
        if (total_flags & UTILEX_SEQ_STEP_F_VERBOSE)
        {
            log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
        }

        if ((total_flags & UTILEX_SEQ_STEP_F_SKIP) != 0)
        {
            /** Skip */
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s     # (skipped)\n", current_step->step_name));
        }
        else if (seq->warmboot && (total_flags & UTILEX_SEQ_STEP_F_WB_SKIP) != 0)
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s     # (skipped in warm reboot)\n", current_step->step_name));
        }
        else if (seq->access_only &&
                 ((total_flags &
                   (UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY | UTILEX_SEQ_STEP_F_REQUIRED_FOR_PORT_ACCESS_ONLY)) ==
                  0))
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity,
                    ("%s    # (skipped in access only boot)\n", current_step->step_name));
        }
        else
        {

            /** If step is disabled with SOC property, skip step */
            if ((total_flags & (UTILEX_SEQ_STEP_F_SOC_PROP_DIS | UTILEX_SEQ_STEP_F_SOC_PROP_EN)) != 0)
            {
                defl = ((total_flags & UTILEX_SEQ_STEP_F_SOC_PROP_EN) ? 1 : 0);
                SHR_IF_ERR_EXIT(utilex_seq_step_enabled
                                (unit, log_severity, seq->control_prefix, current_step, defl, &is_enabled));
            }

            if (is_enabled == 1)
            {

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                /** Allow writing to registers and changing SW state during WB*/
                if (seq->warmboot && (total_flags & UTILEX_SEQ_STEP_F_WB_DISABLE_CHECKS) != 0)
                {
                    if (seq->tmp_allow_access_enable != NULL)
                    {
                        SHR_IF_ERR_CONT(seq->tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SCHAN));
                    }
                    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_DBAL));
                    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SW_STATE));
                }
#endif

                /** Run Step INIT function if exists  */
                if (current_step->forward != NULL)
                {
                    /** Log info  */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s\n", current_step->step_name));
                    /** In case time analyzer operation mode is per step - clear time analyzer DB before running the step*/
                    if (seq->time_analyzer_mode == UTILEX_TIME_ANALYZER_MODE_PER_INIT_STEP)
                    {
                        SHR_IF_ERR_EXIT(utilex_time_analyzer_clear(unit));
                    }
                    /** Run init function */
                    SHR_IF_ERR_EXIT(current_step->forward(unit));
                    /** Store the last passed-id - used for error recovery */
                    seq->last_passed_step = current_step->step_id;
                }

                /** If list - init list steps  */
                if (current_step->step_sub_array != NULL)
                {
                    /** Log info */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("    - "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s:\n", current_step->step_name));

                    /** Run init list function */
                    SHR_IF_ERR_EXIT(utilex_seq_run_step_list_forward
                                    (unit, seq, depth + 1, current_step->step_sub_array, time_overall, swstate_overall,
                                     memory_overall, is_first_done, is_last_done));
                }

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                /** Return to warmboot normal mode */
                if (seq->warmboot && (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
                    && (total_flags & UTILEX_SEQ_STEP_F_WB_DISABLE_CHECKS) != 0)
                {
                    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SW_STATE));
                    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_DBAL));
                    if (seq->tmp_allow_access_disable != NULL)
                    {
                        SHR_IF_ERR_CONT(seq->tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SCHAN));
                    }
                }
#endif
                current_step->disabled_by_soc_prop = FALSE;
            }
            else
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("%s     # (skipped due to SOC property)\n", current_step->step_name));

                current_step->disabled_by_soc_prop = TRUE;
            }
        }

        if (seq->time_log_en || seq->swstate_log_en || seq->mem_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s Done\n", current_step->step_name));

            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("     "));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("Statistics:\n"));
        }

        /** Save time stamp and mem allocation at the end of the step */
        if (seq->time_log_en || seq->time_test_en)
        {
            current_step->internal_info.time_stamp_end = sal_time_usecs();

            if (seq->time_log_en)
            {
                time_elapsed =
                    (current_step->internal_info.time_stamp_end - current_step->internal_info.time_stamp_start);
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
                if (time_elapsed > 2000)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("Time: %u MILLIseconds\n", (time_elapsed / 1000)));
                }
                else
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("Time: %u microseconds\n", time_elapsed));
                }
            }

            if (current_step->step_sub_array == NULL)
            {
                /** Add the time of each step to the overal time for the sequence */
                *time_overall +=
                    (current_step->internal_info.time_stamp_end - current_step->internal_info.time_stamp_start);

                if (seq->time_test_en)
                {
                    /** Check if the time for the current step is exceeding the user defined threshold */
                    if ((current_step->internal_info.time_stamp_end - current_step->internal_info.time_stamp_start) >
                        current_step->time_thresh)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "Overal time to run the step exceeds the permitted threshold!\nTime elapsed: %u microseconds\nThreshold: %lu microseconds\n",
                                     (current_step->internal_info.time_stamp_end -
                                      current_step->internal_info.time_stamp_start), current_step->time_thresh);
                    }
                }
            }
        }

        /** Stop the memory allocation measurment for SW state at the end of step */
        if (seq->swstate_log_en)
        {
            SHR_IF_ERR_EXIT(memory_measurement_dnx_sw_state_get(measurment_handle, &swstate_allocated, TRUE));
            SHR_IF_ERR_EXIT(memory_consumption_clear_measurement_dnx(measurment_handle));

            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("SW state:  %d bytes allocated\n", swstate_allocated));

            if (current_step->step_sub_array == NULL)
            {
                /** Add the memory allocated for SW state of each step to the overall for the sequence */
                *swstate_overall += swstate_allocated;
            }
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the end of step */
            sal_get_alloc_counters(&(alloc_end), &(free_end));

            /** Calculate the total step memory allocation excluding resources allocated for SW state */
            current_step->internal_info.non_swstate_memory_allocation =
                (alloc_end - alloc_start) - (free_end - free_start);

            if (current_step->step_sub_array == NULL)
            {
                /** Add the memory allocated of each step to the overall for the sequence */
                *memory_overall += current_step->internal_info.non_swstate_memory_allocation;
            }

            if (seq->mem_log_en)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("Memory:  %d bytes allocated\n", current_step->internal_info.non_swstate_memory_allocation));
            }
        }

        if (current_step->forward != NULL && seq->time_analyzer_mode == UTILEX_TIME_ANALYZER_MODE_PER_INIT_STEP)
        {
            int nof_modules;
            utilex_time_analyzer_diag_info_t diag_info[UTILEX_TIME_ANALYZER_MAX_NOF_MODULES];
            int module_index;
            char *time_units;
            uint32 time;

            SHR_IF_ERR_EXIT(utilex_time_analyzer_diag_info_get
                            (unit, UTILEX_TIME_ANALYZER_MAX_NOF_MODULES, diag_info, &nof_modules));

            for (module_index = 0; module_index < nof_modules; module_index++)
            {
                if (diag_info[module_index].occurences == 0)
                {
                    continue;
                }

                if (diag_info[module_index].time > /* 2 seconds */ 2 * 1000 * 1000)
                {
                    time = diag_info[module_index].time / (1000 * 1000);
                    time_units = "[sec] ";
                }
                else
                {
                    time = diag_info[module_index].time / 1000;
                    time_units = "[msec]";
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("%-15s: Time %-4u %s | Occ %-6d | Avg Time %-6u [usec]\n", diag_info[module_index].name, time,
                         time_units, diag_info[module_index].occurences,
                         diag_info[module_index].time / diag_info[module_index].occurences));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Managment function for running step list (backward
 *        direction).
 *          The resulted info will be added to total_flags
 *
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in] step - ptr to relevant step
 * \param [in,out] total_flags - flags relevant to this step
 * \param [in,out] is_first_done - assign to one if first done (this step or before)
 * \param [in,out] is_last_done - assign to one if last done (this step or before)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_managment_logic_backward(
    int unit,
    utilex_seq_t * seq,
    utilex_seq_step_t * step,
    uint32 *total_flags,
    int *is_first_done,
    int *is_last_done)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Am I First? - this is the backwards direction - the input last step is actually the first step to be deinit */
    if (step->step_id == seq->last_step || seq->last_step == UTILEX_SEQ_STEP_INVALID)
    {
        *is_first_done = 1;
    }

    /** Is first NOT done and NOT a list */
    if (*is_first_done == 0 && step->step_sub_array == NULL)
    {
        /** Skip this step */
        *total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Is last done? */
    if (*is_last_done == 1)
    {
        /** Skip this step */
        *total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Am I Last? - this is the backwards direction - the input first step is actually the last step to be deinit */
    if (step->step_id == seq->first_step)
    {
        *is_last_done = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - function to run a list of steps (backward direction)
 *
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in,out] depth - recursive list depth
 * \param [in,out] step_list - ptr to relevant step list
 * \param [in,out] is_first_done - assign to one if first done (this step or before)
 * \param [in,out] is_last_done - assign to one if last done (this step or before)
 *
 * \return
 *  See shr_error_e
 * \remark
 *   the funtion is also responsible for making decisions regarding
 *   memory leak detections according to seq and step_list.
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_run_step_list_backward(
    int unit,
    utilex_seq_t * seq,
    int depth,
    utilex_seq_step_t * step_list,
    int *is_first_done,
    int *is_last_done)
{
    int step_index;
    int nof_steps_in_list = 0;
    unsigned long total_memory_alocation_end;
    unsigned long total_memory_alocation_start;
    unsigned long total_memory_free_start;
    unsigned long total_memory_free_end;
    int memory_freed_during_deinit;
    int memory_alocation_during_init;
    utilex_seq_step_t *current_step;
    int depth_index;
    uint32 log_severity;
    int dynamic_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    /** Iterate over steps */
    for (step_index = nof_steps_in_list - 1; step_index >= 0; --step_index)
    {
        uint32 total_flags = 0;
        current_step = &(step_list[step_index]);

        /** Init step vars */
        total_memory_alocation_end = 0;
        total_memory_alocation_start = 0;
        total_memory_free_end = 0;
        total_memory_free_start = 0;

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the start of deinit */
            sal_get_alloc_counters(&(total_memory_alocation_end), &(total_memory_free_end));
        }

        /** At deinit, the only reason to run the managment logic is inorder to switch the step_list_ptr to the sub-list */
        SHR_IF_ERR_EXIT(utilex_seq_managment_logic_backward
                        (unit, seq, current_step, &total_flags, is_first_done, is_last_done));

        /** Run Flag function if exists (unless step was marked as skipped) */
        total_flags |= current_step->static_flags;
        if ((current_step->dyn_flags != NULL) && !(total_flags & UTILEX_SEQ_STEP_F_SKIP))
        {
            dynamic_flags = 0;
            SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
            total_flags |= dynamic_flags;
        }

        /** Logging severity  */
        log_severity = BSL_SEVERITY_ENC(seq->log_severity);
        if (total_flags & UTILEX_SEQ_STEP_F_VERBOSE)
        {
            log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
        }

        if ((total_flags & UTILEX_SEQ_STEP_F_SKIP) != 0)
        {
            /** Skip */
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("Step %s is skipped\n", current_step->step_name));
        }
        else if (SOC_WARM_BOOT(unit) && (total_flags & UTILEX_SEQ_STEP_F_WB_SKIP) != 0)
        {
            /** Skip due to WB */
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("Step %s is skipped during WB\n", current_step->step_name));
        }
        else
        {
            if (current_step->disabled_by_soc_prop == FALSE)
            {
                /** If list - deinit list steps */
                if (current_step->step_sub_array != NULL)
                {
                    /** Log info */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("    - "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s: \n", current_step->step_name));

                    /** Run deinit list */
                    SHR_IF_ERR_CONT(utilex_seq_run_step_list_backward
                                    (unit, seq, depth + 1, current_step->step_sub_array, is_first_done, is_last_done));
                }

                /** Run Step DEINIT function if exists */
                if (current_step->backward != NULL)
                {
                    /** Log info */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("    - "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s\n", current_step->step_name));

                    /** Run deinit function */
                    SHR_IF_ERR_CONT(current_step->backward(unit));
                    /** Store the last passed-id - used for error recovery */
                    seq->last_passed_step = current_step->step_id;
                }
            }
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the end of deinit */
            sal_get_alloc_counters(&(total_memory_alocation_start), &(total_memory_free_start));

            memory_alocation_during_init = current_step->internal_info.non_swstate_memory_allocation;
            memory_freed_during_deinit =
                (total_memory_free_start - total_memory_free_end) - (total_memory_alocation_start -
                                                                     total_memory_alocation_end);

            if (seq->mem_log_en)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("     - Memory:  %d bytes freed\n", memory_freed_during_deinit));
                if (memory_freed_during_deinit != memory_alocation_during_init)
                {
                        /** incase that allocated != freed, print both for convenience  */
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - Delta was found between init and deinit\n"));

                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - Memory allocated at init: %d bytes\n",
                                                            memory_alocation_during_init));

                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                    for (depth_index = 0; depth_index < depth; depth_index++)
                    {
                        BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                    }
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - Memory freed at deinit:   %d bytes\n",
                                                            memory_freed_during_deinit));
                }

            }

            if (seq->mem_test_en)
            {
                if (memory_freed_during_deinit != memory_alocation_during_init)
                {
                    SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory leak detected in step %s, terminating driver\n",
                                 current_step->step_name);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Run forward or backward function of a given step
 */
shr_error_e
utilex_seq_run_step(
    int unit,
    utilex_seq_step_t * current_step,
    int forward)
{
    uint32 log_severity;
    utilex_seq_step_t *sub_step;
    int step_index, nof_steps_in_list;
    SHR_FUNC_INIT_VARS(unit);

    /** Logging severity  */
    log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
    if (current_step->static_flags & UTILEX_SEQ_STEP_F_VERBOSE)
    {
        log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
    }

    if (forward == 1)
    {
        if (current_step->forward != NULL)
        {
            /** Log info */
            BSL_LOG(BSL_LOG_MODULE | log_severity,
                    ("Running forward function for step: %s\n", current_step->step_name));

            /** Run forward function */
            SHR_IF_ERR_EXIT(current_step->forward(unit));
        }

        if (current_step->step_sub_array != NULL)
        {

            /** Find NOF steps in list */
            SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, current_step->step_sub_array, &nof_steps_in_list));

            /** Run sub steps in forward direction */
            for (step_index = 0; step_index < nof_steps_in_list; step_index++)
            {
                sub_step = &(current_step->step_sub_array[step_index]);
                SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, sub_step, forward));
            }
        }

    }
    else
    {
        if (current_step->step_sub_array != NULL)
        {

            /** Find NOF steps in list */
            SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, current_step->step_sub_array, &nof_steps_in_list));

            /** Run sub steps in backward direction */
            for (step_index = (nof_steps_in_list - 1); step_index >= 0; step_index--)
            {
                sub_step = &(current_step->step_sub_array[step_index]);
                SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, sub_step, forward));
            }
        }

        if (current_step->backward != NULL)
        {
            /** Log info */
            BSL_LOG(BSL_LOG_MODULE | log_severity,
                    ("Running backward function for step: %s\n", current_step->step_name));

            /** Run backward function */
            SHR_IF_ERR_EXIT(current_step->backward(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_run_step_by_name(
    int unit,
    utilex_seq_step_t * step_list,
    char *step_name,
    int forward,
    int *step_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *step_found = 0;
    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_run_step_by_name
                            (unit, current_step->step_sub_array, step_name, forward, step_found));

            if ((*step_found) == 1)
            {
                /** Step is found in sub list, no need to continue */
                break;
            }
        }

        if (!sal_strncmp(step_name, current_step->step_name, strlen(step_name)))
        {
            /** Step is found */
            /** Not all steps can be ran alone due to dependancies
            *  between the steps */
            if (current_step->static_flags & UTILEX_SEQ_STEP_F_STANDALONE_EN)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, current_step, forward));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Step %s can not be ran as standalone!\n", current_step->step_name);
            }
            *step_found = 1;
            break;
        }

        if (step_index == (nof_steps_in_list - 1))
        {
            /** Step is not found */
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_run_step_by_id(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    int forward,
    int *step_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *step_found = 0;
    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id
                            (unit, current_step->step_sub_array, step_id, forward, step_found));

            if ((*step_found) == 1)
            {
                /** Step is found in sub list, no need to continue */
                break;
            }
        }

        if (step_id == current_step->step_id)
        {
            /** Step is found */
            /** Not all steps can be ran alone due to dependencies
            *  between the steps */
            if (current_step->static_flags & UTILEX_SEQ_STEP_F_STANDALONE_EN)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step(unit, current_step, forward));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Step %d can not be ran as standalone!\n", step_id);
            }
            *step_found = 1;
            break;
        }

        if (step_index == (nof_steps_in_list - 1))
        {
            /** Step is not found */
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Find step structure by its id
 *
 * \param [in] unit - unit #
 * \param [in] step_list - ptr to relevant step list
 * \param [in] step_id - ID of step to run
 * \param [in] step - pointer to required step
 * \param [out] is_found - indicates weather the step was
 *        found or not
 *
 * \return
 *  See shr_error_e
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_find_step_by_id(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    utilex_seq_step_t ** step,
    int *is_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *is_found = 0;
    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_find_step_by_id(unit, current_step->step_sub_array, step_id, step, is_found));

            if ((*is_found) == 1)
            {
                /** Step is found in sub list, no need to continue */
                break;
            }
        }

        if (step_id == current_step->step_id)
        {
            *step = current_step;
            *is_found = 1;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_run(
    int unit,
    utilex_seq_t * seq,
    int forward)
{
    int is_first_done = 0;
    int is_last_done = 0;
    sal_time_t time_overall = 0;
    uint32 swstate_overall = 0;
    uint32 memory_overall = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_seq_run_verify(unit, seq, forward));

    if (forward)
    {
        /** set warmboot indication */
        seq->warmboot = SOC_WARM_BOOT(unit);
        SHR_IF_ERR_EXIT(utilex_seq_run_step_list_forward
                        (unit, seq, 0, seq->step_list, &time_overall, &swstate_overall, &memory_overall,
                         &is_first_done, &is_last_done));
        if (seq->time_test_en || seq->time_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | BSL_SEVERITY_ENC(seq->log_severity),
                    ("%s Done (Total Time: %lu MILLIseconds)\n", seq->log_prefix, (time_overall / 1000)));

            if (seq->time_test_en && (time_overall > seq->time_thresh))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Overal time to run the sequence exceeds the permitted threshold!\nOveral time elapsed: %lu microseconds\nThreshold: %lu microseconds\n",
                             time_overall, seq->time_thresh);
            }
        }

        if (seq->swstate_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | BSL_SEVERITY_ENC(seq->log_severity),
                    ("%s Done (Total SW state: %u bytes allocated)\n", seq->log_prefix, swstate_overall));
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | BSL_SEVERITY_ENC(seq->log_severity),
                    ("%s Done (Non-SW state Memory: %u bytes allocated)\n", seq->log_prefix, memory_overall));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(utilex_seq_run_step_list_backward
                                 (unit, seq, 0, seq->step_list, &is_first_done, &is_last_done),
                                 "An Error has occurred in one of the steps above, please check log to understand from which step it originated from\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return next step id given current step id. The next step id can be either next one in forward run or the
 *          next on in backawrd run.
 * \param [in] unit - unit #.
 * \param [in] step_list - List of steps
 * \param [in] step_id - current step id
 * \param [out] next_step - pointer to next step
 * \param [out] is_found - indicates whether step is found
 * \return
 *   See shr_error_e
 * \remark
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_next_step_get(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    utilex_seq_step_t ** next_step,
    int *is_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *is_found = UTILEX_SEQ_STEP_NOT_FOUND;

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_next_step_get(unit, current_step->step_sub_array, step_id, next_step, is_found));

            /** Step is found no need to continue */
            if (*is_found == UTILEX_SEQ_NEXT_STEP_FOUND)
            {
                break;
            }

            /** step found in sub list as the last step */
            if (*is_found == UTILEX_SEQ_STEP_FOUND)
            {
                if (step_index != (nof_steps_in_list - 1))
                {
                    *next_step = &(step_list[step_index + 1]);
                    *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
                }
                break;
            }
        }

        /** step found */
        if (step_id == current_step->step_id)
        {
            /** next step is the first element in the sublist */
            if (current_step->step_sub_array != NULL)
            {
                *next_step = &(current_step->step_sub_array[0]);
                *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
            }
            /** next step is first element in the next sublist */
            else if (step_index == (nof_steps_in_list - 1))
            {
                *is_found = UTILEX_SEQ_STEP_FOUND;
            }
            else
            {
                *next_step = &(step_list[step_index + 1]);
                *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return last active step id given a step list.
 * \param [in] unit - unit #.
 * \param [in] step_list - List of steps
 * \param [out] active_step - pointer to last active step
 * \return
 *   See shr_error_e
 * \remark
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_last_active_step_get(
    int unit,
    utilex_seq_step_t * step_list,
    utilex_seq_step_t ** active_step)
{
    int nof_steps_in_list = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    while (step_list[nof_steps_in_list - 1].step_sub_array != NULL)
    {
        step_list = step_list[nof_steps_in_list - 1].step_sub_array;
        SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));
    }

    *active_step = &step_list[nof_steps_in_list - 1];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return previous step id given current step id.
 * \param [in] unit - unit #.
 * \param [in] step_list - List of steps
 * \param [in] step_id - current step id
 * \param [out] previous_step - pointer to previous step
 * \param [out] is_found - indicates whether step is found
 * \return
 *   See shr_error_e
 * \remark
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_previous_step_get(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    utilex_seq_step_t ** previous_step,
    int *is_found)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *is_found = UTILEX_SEQ_STEP_NOT_FOUND;

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any */
            SHR_IF_ERR_EXIT(utilex_seq_previous_step_get
                            (unit, current_step->step_sub_array, step_id, previous_step, is_found));

            /** Step is found no need to continue */
            if (*is_found == UTILEX_SEQ_NEXT_STEP_FOUND)
            {
                break;
            }

            /** step found in sub list as the last step */
            if (*is_found == UTILEX_SEQ_STEP_FOUND)
            {
                if (step_index > 0)
                {
                    if (step_list[step_index - 1].step_sub_array != NULL)
                    {
                        SHR_IF_ERR_EXIT(utilex_seq_last_active_step_get(unit,
                                                                        (step_list[step_index - 1]).step_sub_array,
                                                                        previous_step));
                    }
                    else
                    {
                        *previous_step = &(step_list[step_index - 1]);
                    }

                    *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
                }
                break;
            }
        }

        /** step found */
        if (step_id == current_step->step_id)
        {
            if (step_index == 0)
            {
                *is_found = UTILEX_SEQ_STEP_FOUND;
            }
            else
            {
                if (step_list[step_index - 1].step_sub_array != NULL)
                {
                    SHR_IF_ERR_EXIT(utilex_seq_last_active_step_get(unit, (step_list[step_index - 1]).step_sub_array,
                                                                    previous_step));
                }
                else
                {
                    *previous_step = &(step_list[step_index - 1]);
                }

                *is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return indication whether the step or one of its father lists is marked as skipped
 * \param [in] unit - unit #.
 * \param [in] step_list - List of steps
 * \param [in] step_id - current step id
 * \param [out] is_found -indicates whether step was found
 * \param [out] is_skip -indicates whether step should be skipped
 * \return
 *   See shr_error_e
 * \remark
 * \see
 *   * None
 */
static shr_error_e
utilex_seq_step_is_skipped_get(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    int *is_found,
    int *is_skip)
{
    int step_index;
    int nof_steps_in_list = 0;
    utilex_seq_step_t *current_step = NULL;
    int dynamic_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    *is_found = 0;
    *is_skip = 0;

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        if (current_step->step_sub_array != NULL)
        {
            /** Search sub array, if any  */
            SHR_IF_ERR_EXIT(utilex_seq_step_is_skipped_get
                            (unit, current_step->step_sub_array, step_id, is_found, is_skip));

            /** Step is found no need to continue */
            if ((*is_found) == 1)
            {
                dynamic_flags = 0;
                if (current_step->dyn_flags != NULL)
                {
                    SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
                }
                /** mark as skipped */
                if (_SHR_IS_FLAG_SET(current_step->static_flags, UTILEX_SEQ_STEP_F_SKIP) ||
                    _SHR_IS_FLAG_SET(dynamic_flags, UTILEX_SEQ_STEP_F_SKIP))
                {
                    *is_skip = 1;
                }

                break;
            }
        }

        /** step found */
        if (step_id == current_step->step_id)
        {
            dynamic_flags = 0;
            if (current_step->dyn_flags != NULL)
            {
                SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
            }
            /** mark as skipped */
            if (_SHR_IS_FLAG_SET(current_step->static_flags, UTILEX_SEQ_STEP_F_SKIP) ||
                _SHR_IS_FLAG_SET(dynamic_flags, UTILEX_SEQ_STEP_F_SKIP))
            {
                *is_skip = 1;
            }

            *is_found = 1;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_next_active_step_get(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    utilex_seq_step_t ** next_step,
    int *is_found)
{
    SHR_FUNC_INIT_VARS(unit);

    /** get next step */
    SHR_IF_ERR_EXIT(utilex_seq_next_step_get(unit, step_list, step_id, next_step, is_found));
    if (*is_found != UTILEX_SEQ_NEXT_STEP_FOUND)
    {
        /** end of list */
        SHR_EXIT();
    }

    while (((*next_step)->forward == NULL) && ((*next_step)->backward == NULL))
    {
        /** get next step */
        SHR_IF_ERR_EXIT(utilex_seq_next_step_get(unit, step_list, (*next_step)->step_id, next_step, is_found));

        if (*is_found != UTILEX_SEQ_NEXT_STEP_FOUND)
        {
            /** end of list */
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_seq_error_recovery_test_run(
    int unit,
    utilex_seq_t * seq,
    int first_step_id,
    int *steps_to_skip,
    int nof_steps_to_skip,
    utilex_seq_err_recovery_test_t * test_info)
{
    int original_last_step_id, previous_step_id, current_step_id;
    unsigned long alloc_start, free_start, alloc_end, free_end;
    utilex_seq_step_t *current_step, *first_step;
    int is_found, is_skip, ii;
    int rv;
    int first_run = 1; /** required in order to run first step in the list if it was provided explicitly */

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(test_info, 0, sizeof(utilex_seq_err_recovery_test_t));

    /** get original last step id in order to know when to finish the test */
    if (seq->last_step == UTILEX_SEQ_STEP_INVALID)
    {
        original_last_step_id = -1;
    }
    else
    {
        original_last_step_id = seq->last_step;
    }

    /**
     * Find first step, first step can be either first step in list (by default) or provided explicitly
     */
    if (first_step_id == UTILEX_SEQ_STEP_INVALID)
    {
        first_step = &(seq->step_list[0]);
    }
    else                                                                                 /** test a given step */
    {
        /** lookup first step by its id */
        SHR_IF_ERR_EXIT(utilex_seq_find_step_by_id(unit, seq->step_list, first_step_id, &first_step, &is_found));
        if (is_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "First Step id %d wasn't found in the step list\n", first_step_id);
        }
    }

    /** if first step is not active, find next step which is active and set it as first step */
    if (first_step->forward == NULL && first_step->backward == NULL)
    {
        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq->step_list, first_step_id, &first_step, &is_found));
    }

    /** by default, previous step is the same as first */
    if ((first_step_id == UTILEX_SEQ_STEP_INVALID) || (first_step_id == (seq->step_list[0].step_id)))
    {
        previous_step_id = first_step->step_id;
        current_step_id = previous_step_id;
    }
    else
    {
        current_step_id = first_step->step_id;

        /** find previous step */
        SHR_IF_ERR_EXIT(utilex_seq_previous_step_get(unit, seq->step_list, first_step_id, &current_step, &is_found));
        if (is_found != UTILEX_SEQ_NEXT_STEP_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "First Step id %d wasn't found in the step list\n", first_step_id);
        }
        previous_step_id = current_step->step_id;
    }

    /** set current step as first */
    current_step = first_step;

    /** iterate until last step id */
    while ((previous_step_id != original_last_step_id) || first_run)
    {
        first_run = 0;

        BSL_LOG(BSL_LOG_MODULE, ("\n"));
        BSL_LOG(BSL_LOG_MODULE, ("\n"));
        BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
        BSL_LOG(BSL_LOG_MODULE, ("Start test for Step %s\n", (current_step->step_name)));
        BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));

        /** run sequence until previous step (in case this is not the first step) */
        if (previous_step_id != current_step->step_id)
        {
            seq->last_step = previous_step_id;
            sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "Test Init:");
            SHR_IF_ERR_EXIT(utilex_seq_run(unit, seq, TRUE));
        }

        /** get memory allocation before running current step */
        sal_get_alloc_counters(&(alloc_start), &(free_start));

        /** get indication whether current step should be skipped */
        SHR_IF_ERR_EXIT(utilex_seq_step_is_skipped_get(unit, seq->step_list, current_step_id, &is_found, &is_skip));

        /** check whether the step should be skipped due to test requirements */
        if (is_skip == FALSE)
        {
            for (ii = 0; ii < nof_steps_to_skip; ii++)
            {
                if (steps_to_skip[ii] == current_step_id)
                {
                    is_skip = TRUE;
                    break;
                }
            }
        }

        /** run last step if step should not be skipped */
        BSL_LOG(BSL_LOG_MODULE, ("--------------------------------------------------\n"));
        if (is_skip == FALSE)
        {
            /**
             * Verify error recovery - run backward step function without running the forward function.
             * Errors are allowed, crashes not!
             */
            if (current_step->backward != NULL)
            {
                BSL_LOG(BSL_LOG_MODULE, ("Verify error-recovery for step %s - running backward function before "
                                         "forward\n", current_step->step_name));
                rv = current_step->backward(unit);
                if (rv != _SHR_E_NONE)
                {
                    BSL_LOG(BSL_LOG_MODULE, ("Backward run for step %s returned error %d, continue testing \n",
                                             current_step->step_name, rv));
                }
            }

            /** run forward step function */
            if (current_step->forward != NULL)
            {
                BSL_LOG(BSL_LOG_MODULE, ("Running forward function for step: %s\n", current_step->step_name));
                SHR_IF_ERR_EXIT(current_step->forward(unit));
            }

            /** run backward step function */
            if (current_step->backward != NULL)
            {
                BSL_LOG(BSL_LOG_MODULE, ("Running backward function for step: %s\n", current_step->step_name));
                SHR_IF_ERR_EXIT(current_step->backward(unit));
            }
        }
        else
        {
            BSL_LOG(BSL_LOG_MODULE, ("############ Step %s is not tested since it's skipped ###########\n",
                                     (current_step->step_name)));
        }
        BSL_LOG(BSL_LOG_MODULE, ("--------------------------------------------------\n"));

        /** get memory allocation after deinitializating current step */
        sal_get_alloc_counters(&(alloc_end), &(free_end));

        /** run deinit sequence (in case this is not the first step) */
        if (previous_step_id != current_step->step_id)
        {
            sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "Test Deinit:");
            SHR_IF_ERR_EXIT(utilex_seq_run(unit, seq, FALSE));
        }

        /** verify whether deinit freed all allocated memory */
        if ((alloc_start - free_start) != (alloc_end - free_end))
        {
            BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
            BSL_LOG(BSL_LOG_MODULE, ("Error!!! Step %s Failed!\n", (current_step->step_name)));
            BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));

            if (test_info->fail_count < UTILEX_SEQ_NOF_FAILED_STEPS)
            {
                test_info->fail_step_name[test_info->fail_count] = current_step->step_name;
                test_info->fail_mem_size[test_info->fail_count] = (alloc_end - free_end) - (alloc_start - free_start);
                test_info->fail_step_id[test_info->fail_count] = current_step->step_id;
                (test_info->fail_count)++;
            }
        }
        else
        {
            BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
            BSL_LOG(BSL_LOG_MODULE, ("Step %s PASSED!\n", (current_step->step_name)));
            BSL_LOG(BSL_LOG_MODULE, ("#############################################\n"));
        }

        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq->step_list, current_step_id, &current_step,
                                                        &is_found));

        /** only current step found, break since we reached end of list */
        if (is_found == UTILEX_SEQ_STEP_FOUND)
        {
            /** end of list */
            break;
        }

        if (is_found == UTILEX_SEQ_STEP_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Step %s wasn't found in the step list\n", current_step->step_name);
        }

        /** Proceed to next step */
        previous_step_id = current_step_id;
        current_step_id = current_step->step_id;
    }

exit:
    SHR_FUNC_EXIT;
}
