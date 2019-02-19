/** \file appl_ref_init_utils.c
 * Common init and deinit functions to be used by the INIT_DNX command.
 */

/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/*
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_seq.h>

#include <soc/property.h>

#include <appl/reference/dnxc/appl_ref_init_utils.h>

/*
 * }
 */

/*
 * Typedefs.
 * {  
 */

/*
 * }
 */

/*
 * Globals.
 * {  
 */

/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
appl_dnxc_init_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list)
{
    int step_index;
    SHR_FUNC_INIT_VARS(unit);

    if (step_list != NULL)
    {
        /** recursive destroy */
        for (step_index = 0; step_list[step_index].step_id != UTILEX_SEQ_STEP_LAST; step_index++)
        {
            if (step_list[step_index].step_sub_array != NULL)
            {
                SHR_IF_ERR_CONT(appl_dnxc_init_step_list_destory(unit, step_list[step_index].step_sub_array));
            }
        }

        /** destroy current */
        sal_free(step_list);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - counts NOF members in step list
 * 
 * \param [in] unit - unit #
 * \param [in] step_list - pointer to step list
 * \param [out] nof_steps - returned result
 * \return
 *   See shr_error_e
 * \remark
 *   list MUST contain at least one member (last member) with
 *   name  == NULL.
 * \see
 *   * None
 */
static shr_error_e
appl_dnxc_init_step_list_count_steps(
    int unit,
    const appl_dnxc_init_step_t * step_list,
    int *nof_steps)
{
    int step_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * all lists must have at least one member - the last one - with name = NULL
     */
    for (step_index = 0; step_list[step_index].name != NULL; ++step_index)
    {
        /*
         * Do nothing 
         */
    }

    *nof_steps = step_index;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_init_step_list_convert(
    int unit,
    const appl_dnxc_init_step_t * dnx_step_list,
    utilex_seq_step_t ** step_list)
{
    int list_size;
    int step_index;
    utilex_seq_step_t *step;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get list size
     */
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_count_steps(unit, dnx_step_list, &list_size));
    list_size++; /** Count the last step too */

    /*
     * Allocate memory for list
     */
    *step_list =
        (utilex_seq_step_t *) sal_alloc(sizeof(utilex_seq_step_t) * list_size, "dnx port add remove sequence list");
    SHR_NULL_CHECK(*step_list, _SHR_E_MEMORY, "failed to allocate memory for step list");

    /*
     * Convert each step
     */
    for (step_index = 0; step_index < list_size; step_index++)
    {
        step = &((*step_list)[step_index]);
        SHR_IF_ERR_EXIT(utilex_seq_step_t_init(unit, step));

        step->step_id = dnx_step_list[step_index].step_id;
        step->step_name = dnx_step_list[step_index].name;
        step->soc_prop_suffix = dnx_step_list[step_index].suffix;
        step->forward = dnx_step_list[step_index].init_func;
        step->backward = dnx_step_list[step_index].deinit_func;
        step->static_flags = dnx_step_list[step_index].step_flags;

        if (dnx_step_list[step_index].sub_list != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_convert
                            (unit, dnx_step_list[step_index].sub_list, &(step->step_sub_array)));
        }

        if (dnx_step_list[step_index].time_thresh != 0)
        {
            step->time_thresh = dnx_step_list[step_index].time_thresh;
        }
        else
        {
            step->time_thresh = APPL_INIT_STEP_TIME_THRESH_DEFAULT;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_init_seq_flags_get(
    int unit,
    int *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "time_test_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_TIME_STAMP;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "time_log_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_TIME_LOG;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mem_test_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_MEM_LEAK_DETECT;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mem_log_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_MEM_LOG;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "swstate_log_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_SWSTATE_LOG;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "swstate_test_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_SWSTATE_RESOURCE;
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_init_seq_convert(
    int unit,
    const appl_dnxc_init_step_t * step_list,
    int forward,
    utilex_seq_t * seq)
{
    int flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init seq structure
     */
    SHR_IF_ERR_EXIT(utilex_seq_t_init(unit, seq));

    
    appl_init_seq_flags_get(unit, &flags);

    /*
     * Configure sequence 
     */
    /** Logging */
    seq->bsl_flags = BSL_LOG_MODULE;
    if (forward)
    {
        seq->log_prefix = "APPL Init:";
    }
    else
    {
        seq->log_prefix = "APPL Deinit:";
    }
    seq->control_prefix = spn_APPL_ENABLE;
    seq->log_severity = bslSeverityInfo;

    seq->time_log_en = (flags & APPL_INIT_ADVANCED_F_TIME_LOG) ? 1 : 0;
    seq->time_test_en = (flags & APPL_INIT_ADVANCED_F_TIME_STAMP) ? 1 : 0;
    seq->time_thresh = APPL_INIT_TIME_THRESH_DEFAULT;
    seq->mem_log_en = (flags & APPL_INIT_ADVANCED_F_MEM_LOG) ? 1 : 0;
    seq->mem_test_en = (flags & APPL_INIT_ADVANCED_F_MEM_LEAK_DETECT) ? 1 : 0;
    seq->swstate_log_en = (flags & APPL_INIT_ADVANCED_F_SWSTATE_LOG) ? 1 : 0;
    seq->swstate_test_en = (flags & APPL_INIT_ADVANCED_F_SWSTATE_RESOURCE) ? 1 : 0;

    /** sequence */
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_convert(unit, step_list, &seq->step_list));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_steps_convert_and_run(
    int unit,
    const appl_dnxc_init_step_t * step_list,
    int forward)
{
    utilex_seq_t seq;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    /** Convert step list to general utilex_seq list */
    SHR_IF_ERR_EXIT(appl_dnxc_init_seq_convert(unit, step_list, forward, &seq));

    /** Run list forward, Check error just after utilex seq list destroy */
    rv = utilex_seq_run(unit, &seq, forward);

    /** Destory step list (allocated in 'dnx_port_add_remove_seq_convert()') */
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_destory(unit, seq.step_list));

    /** Check error */
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}
