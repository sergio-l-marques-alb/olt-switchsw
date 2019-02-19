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
 * $Copyright: (c) 2017 Broadcom.
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
#include <shared/shrextend/shrextend_debug.h>
#include <shared/mem_measure_tool.h>

#include <soc/drv.h>
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
    seq->log_prefix = "SEQ:";
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
     * check that MEM_LEAK and TIEM_STAMP flags are not used when
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
 *          The resulted info will be stored in step->internal_info
 * 
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in,out] step - ptr to relevant step  
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
        step->internal_info.total_flags |= UTILEX_SEQ_STEP_F_SKIP;
    }

    /** Is last done? */
    if (*is_last_done == 1)
    {
        /** Skip this step */
        step->internal_info.total_flags |= UTILEX_SEQ_STEP_F_SKIP;
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
 * \param [out] memory_overall - total memory allocated,
 *        including for SW state, for the step list
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
    unsigned long alloc_start;
    unsigned long free_start;
    unsigned long alloc_end;
    unsigned long free_end;
    char measurment_handle[30];
    char seq_prefix[20];
    int time_elapsed;

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    for (step_index = 0; step_index < nof_steps_in_list; ++step_index)
    {
        current_step = &(step_list[step_index]);

        /** Save time stamp and mem allocation at beginning of step */
        if (seq->time_log_en || seq->time_test_en)
        {
            current_step->internal_info.time_stamp_start = sal_time_usecs();
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the beginning of step */
            sal_get_alloc_counters(&(alloc_start), &(free_start));
            current_step->internal_info.total_memory_alocation_start = alloc_start;
        }

        /** Start the memory allocation measurement for SW state in the beginning of step */
        if (seq->swstate_log_en)
        {
            utilex_str_replace_whitespace(seq_prefix, seq->log_prefix);
            sal_sprintf(measurment_handle, "Utilex%s%d", seq_prefix, current_step->step_id);
            SHR_IF_ERR_EXIT(memory_consumption_start_measurement_dnx(measurment_handle));
        }

        /** Run Flag function if exists */
        current_step->internal_info.total_flags |= current_step->static_flags;
        if (current_step->dyn_flags != NULL)
        {
            dynamic_flags = 0;
            SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
            current_step->internal_info.total_flags |= dynamic_flags;
        }

        /** Decisions made by the management logic are stronger then ones made by the flag cb function  */
        SHR_IF_ERR_EXIT(utilex_seq_managment_logic_forward(unit, seq, current_step, is_first_done, is_last_done));

        /** Logging severity */
        log_severity = BSL_SEVERITY_ENC(seq->log_severity);
        if (current_step->internal_info.total_flags & UTILEX_SEQ_STEP_F_VERBOSE)
        {
            log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
        }

        if ((current_step->internal_info.total_flags & UTILEX_SEQ_STEP_F_SKIP) != 0)
        {
            /** Skip */
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s     # (skipped)\n", current_step->step_name));
        }
        else if (seq->warmboot && (current_step->internal_info.total_flags & UTILEX_SEQ_STEP_F_WB_SKIP) != 0)
        {
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s ", seq->log_prefix));
            for (depth_index = 0; depth_index < depth; depth_index++)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("   - "));
            }
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s     # (skipped in warm reboot)\n", current_step->step_name));
        }
        else if (seq->access_only &&
                 ((current_step->internal_info.total_flags &
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

            /** If step is diasabled with SOC property, skip step */
            if ((current_step->internal_info.total_flags &
                 (UTILEX_SEQ_STEP_F_SOC_PROP_DIS | UTILEX_SEQ_STEP_F_SOC_PROP_EN)) != 0)
            {
                defl = ((current_step->internal_info.total_flags & UTILEX_SEQ_STEP_F_SOC_PROP_EN) ? 1 : 0);
                SHR_IF_ERR_EXIT(utilex_seq_step_enabled
                                (unit, log_severity, seq->control_prefix, current_step, defl, &is_enabled));
            }

            if (is_enabled == 1)
            {
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
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
                time_elapsed =
                    (current_step->internal_info.time_stamp_end - current_step->internal_info.time_stamp_start);
                if (time_elapsed > 2000)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("Time: %d MILLIseconds\n", (time_elapsed / 1000)));
                }
                else
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("Time: %d microseconds\n", time_elapsed));
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
        if (seq->swstate_test_en || seq->swstate_log_en)
        {
            SHR_IF_ERR_EXIT(memory_measurement_dnx_sw_state_get
                            (measurment_handle, &(current_step->internal_info.swstate_allocated_recources), TRUE));
            SHR_IF_ERR_EXIT(memory_consumption_clear_measurement_dnx(measurment_handle));

            if (current_step->step_sub_array == NULL)
            {
                /** Add the memory allocated for SW state of each step to the overal for the sequence */
                *swstate_overall += current_step->internal_info.swstate_allocated_recources;
            }

            if (seq->swstate_log_en)
            {
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("%s", seq->log_prefix));
                for (depth_index = 0; depth_index < depth; depth_index++)
                {
                    BSL_LOG(BSL_LOG_MODULE | log_severity, ("       "));
                }
                BSL_LOG(BSL_LOG_MODULE | log_severity, ("     - "));
                BSL_LOG(BSL_LOG_MODULE | log_severity,
                        ("SW state:  %d bytes allocated\n", (current_step->internal_info.swstate_allocated_recources)));
            }

            if (seq->swstate_test_en)
            {
                /** Currently there is nothing to be tested regarding the SW state resources, need to provide the capability to add a test in the future */
            }
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            /** Check memory allocated and memory freed in the end of step */
            sal_get_alloc_counters(&(alloc_end), &(free_end));
            current_step->internal_info.total_memory_alocation_end = alloc_end;

            if (current_step->step_sub_array == NULL)
            {
                /** Add the memory allocated of each step to the overal for the sequence */
                *memory_overall += (current_step->internal_info.total_memory_alocation_end -
                                    current_step->internal_info.total_memory_alocation_start);
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
                        ("Memory:  %d bytes allocated\n", (current_step->internal_info.total_memory_alocation_end -
                                                           current_step->internal_info.total_memory_alocation_start)));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Managment function for running step list (backward 
 *        direction).
 *          The resulted info will be stored in step->internal_info
 * 
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info
 * \param [in,out] step - ptr to relevant step 
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
    utilex_seq_step_t * step)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Do nothing */

    SHR_FUNC_EXIT;
}

/**
 * \brief - function to run a list of steps (backward direction)
 *  
 * \param [in] unit - unit #
 * \param [in] seq - main structure with global info 
 * \param [in,out] depth - recursive list depth   
 * \param [in,out] step_list - ptr to relevant step list  
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
    utilex_seq_step_t * step_list)
{
    int step_index;
    int nof_steps_in_list = 0;
    unsigned long total_memory_alocation_end;
    unsigned long total_memory_alocation_start;
    int memory_freed_during_deinit;
    int memory_alocation_during_init;
    utilex_seq_step_t *current_step;
    int depth_index;
    uint32 log_severity;
    unsigned long free_start;
    unsigned long free_end;
    int dynamic_flags;
    int is_enabled = 1;
    int defl;
    uint32 sw_state_freed_resources;
    char measurment_handle[30];
    char seq_prefix[20];

    SHR_FUNC_INIT_VARS(unit);

    /** Find NOF steps in list */
    SHR_IF_ERR_EXIT(utilex_seq_nof_steps_get(unit, step_list, &nof_steps_in_list));

    /** Iterate over steps */
    for (step_index = nof_steps_in_list - 1; step_index >= 0; --step_index)
    {
        current_step = &(step_list[step_index]);

        /** Init step vars */
        total_memory_alocation_end = 0;
        total_memory_alocation_start = 0;

        if (seq->mem_test_en)
        {
            /** Check memory allocated and memory freed in the start of deinit */
            sal_get_alloc_counters(&(total_memory_alocation_end), &(free_end));
        }

        /** Start the memory allocation measurment for SW state in the beggining of dinit */
        if (seq->swstate_test_en)
        {
            utilex_str_replace_whitespace(seq_prefix, seq->log_prefix);
            sal_sprintf(measurment_handle, "Utilex%s%d", seq_prefix, current_step->step_id);
            SHR_IF_ERR_EXIT(memory_consumption_start_measurement_dnx(measurment_handle));
        }

        /** Run Flag function if exists */
        current_step->internal_info.total_flags |= current_step->static_flags;
        if (current_step->dyn_flags != NULL)
        {
            dynamic_flags = 0;
            SHR_IF_ERR_EXIT(current_step->dyn_flags(unit, &dynamic_flags));
            current_step->internal_info.total_flags |= dynamic_flags;
        }

        /** At deinit, the only reason to run the managment logic is inorder to switch the step_list_ptr to the sub-list */
        SHR_IF_ERR_EXIT(utilex_seq_managment_logic_backward(unit, seq, current_step));

        /** Logging severity  */
        log_severity = BSL_SEVERITY_ENC(seq->log_severity);
        if (current_step->internal_info.total_flags & UTILEX_SEQ_STEP_F_VERBOSE)
        {
            log_severity = BSL_SEVERITY_ENC(bslSeverityVerbose);
        }

        if ((current_step->internal_info.total_flags & UTILEX_SEQ_STEP_F_SKIP) != 0)
        {
            /** Skip */
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("Step %s is skipped\n", current_step->step_name));
        }
        else if (SOC_WARM_BOOT(unit) && (current_step->internal_info.total_flags & UTILEX_SEQ_STEP_F_WB_SKIP) != 0)
        {
            /** Skip due to WB */
            BSL_LOG(BSL_LOG_MODULE | log_severity, ("Step %s is skipped during WB\n", current_step->step_name));
        }
        else
        {
            /** If step is diasabled with SOC property, skip step */
            if ((current_step->internal_info.total_flags &
                 (UTILEX_SEQ_STEP_F_SOC_PROP_DIS | UTILEX_SEQ_STEP_F_SOC_PROP_EN)) != 0)
            {
                defl = ((current_step->internal_info.total_flags & UTILEX_SEQ_STEP_F_SOC_PROP_EN) ? 1 : 0);
                SHR_IF_ERR_EXIT(utilex_seq_step_enabled
                                (unit, log_severity, seq->control_prefix, current_step, defl, &is_enabled));
            }

            if (is_enabled)
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
                                    (unit, seq, depth + 1, current_step->step_sub_array));
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

        if (seq->mem_test_en)
        {
            /** Check memory allocated and memory freed in the end of deinit */
            sal_get_alloc_counters(&(total_memory_alocation_start), &(free_start));
            memory_alocation_during_init =
                current_step->internal_info.total_memory_alocation_end -
                current_step->internal_info.total_memory_alocation_start;
            memory_freed_during_deinit = total_memory_alocation_end - total_memory_alocation_start;
            if (memory_freed_during_deinit != memory_alocation_during_init)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory leak detected in step %s, terminating driver\n",
                             current_step->step_name);
            }
        }

        if (seq->swstate_test_en)
        {
            /** Check memory allocated and memory freed for SW state in the end of deinit */
            SHR_IF_ERR_EXIT(memory_measurement_dnx_sw_state_get(measurment_handle, &sw_state_freed_resources, FALSE));
            SHR_IF_ERR_EXIT(memory_consumption_clear_measurement_dnx(measurment_handle));
            if (sw_state_freed_resources < current_step->internal_info.swstate_allocated_recources)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "SW state memory leak detected in step %s, terminating driver\n",
                             current_step->step_name);
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
    log_severity = BSL_SEVERITY_ENC(bslSeverityNormal);
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
            /** Not all steps can be ran alone due to dependancies
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
                        (unit, seq, 0, seq->step_list, &time_overall, &swstate_overall, &memory_overall, &is_first_done,
                         &is_last_done));
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

        if (seq->swstate_test_en || seq->swstate_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | BSL_SEVERITY_ENC(seq->log_severity),
                    ("%s Done (Total SW state: %u bytes allocated)\n", seq->log_prefix, swstate_overall));
        }

        if (seq->mem_test_en || seq->mem_log_en)
        {
            BSL_LOG(BSL_LOG_MODULE | BSL_SEVERITY_ENC(seq->log_severity),
                    ("%s Done (Total Memory: %u bytes allocated)\n", seq->log_prefix, memory_overall));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_seq_run_step_list_backward(unit, seq, 0, seq->step_list));
    }

exit:
    SHR_FUNC_EXIT;
}
