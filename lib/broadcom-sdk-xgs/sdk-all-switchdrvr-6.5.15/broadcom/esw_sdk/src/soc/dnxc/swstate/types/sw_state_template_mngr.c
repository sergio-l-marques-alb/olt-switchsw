/** \file sw_state_template_mngr.c
 *
 * Template management functions.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

/*
 * Other include files.
 * {
 */
#include <bcm/types.h>
#include <shared/swstate/sw_state.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/multithread_analyzer.h>

#ifdef BCM_DNX_SUPPORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm_int/dnx/algo/template_mngr/multi_set_template.h>
#include <soc/dnxc/swstate/types/sw_state_template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_callbacks.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
/*
 * }
 */

/*
 * }
 */

/**
 * }
 */

/*
 * Pool core is an array index. When templates are not duplicated per core, the first array entry (0)
 *   represents all cores. 
 * Since the input core for these templates is always _SHR_CORE_ALL, fix it to 0.
 */
#define SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(_core_id) ((_core_id == _SHR_CORE_ALL) ? 0 : _core_id)

/*
 * Verify input for dnx_algo_template_create.
 */
static shr_error_e
sw_state_algo_template_create_verify(
    int unit,
    sw_state_algo_template_create_data_t * data)
{
    uint8 use_advanced_algorithm;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "create_data");

    /*
     * Verify generic data.
     */
    if (data->first_profile < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "First profile can't be negative. Got: %d", data->first_profile);
    }

    if (data->nof_profiles < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of profiles must be at least 1. Got: %d", data->nof_profiles);
    }

    if (data->data_size < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Data size must be at least 1. Got: %d", (int) data->data_size);
    }

    if (data->max_references < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max references must be at least 1. Got: %d", (int) data->max_references);
    }

    /*
     * Verify advanced algorithm. SW_STATE_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM
     */
    use_advanced_algorithm = _SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM);
    if (use_advanced_algorithm)
    {
        if (data->advanced_algorithm < DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC
            || data->advanced_algorithm >= DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The advanced algorithm is invalid. Must be at least %d and no bigger than but %d we got: %d",
                         DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC, DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT,
                         data->advanced_algorithm);
        }
    }
    else
    {
        if (data->advanced_algorithm != DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "If the SW_STATE_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM flag is not set, the advanced algorithm should be %d, but it is %d",
                         DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC, data->advanced_algorithm);
        }
    }

    /*
     *  Verify default profile if it's in use.
     */
    if (_SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        int max_profile;
        if (data->default_profile < data->first_profile)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Default profile is too small. Must be at least %d but got: %d",
                         data->first_profile, data->default_profile);
        }

        max_profile = data->first_profile + data->nof_profiles - 1;
        if (data->default_profile > max_profile)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Default profile is too high. Must be at most %d but got: %d",
                         max_profile, data->default_profile);
        }

        if (data->default_data == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "If default profile is in use, default data must be provided.");
        }
    }

    if (Template_callbacks_map_sw[data->advanced_algorithm].create_cb == NULL
        || Template_callbacks_map_sw[data->advanced_algorithm].allocate_cb == NULL
        || Template_callbacks_map_sw[data->advanced_algorithm].profile_data_get_cb == NULL
        || Template_callbacks_map_sw[data->advanced_algorithm].profile_get_cb == NULL
        || Template_callbacks_map_sw[data->advanced_algorithm].free_cb == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "All mandatory APIs, create_cb, allocate_cb, profile_data_get_cb, profile_get_cb and free_cb"
                     "must be provided.\n"
                     "Advanced algorithm index is: %d\n"
                     "Pointers are: %p, %p, %p, %p, %p",
                     data->advanced_algorithm,
                     Template_callbacks_map_sw[data->advanced_algorithm].create_cb,
                     Template_callbacks_map_sw[data->advanced_algorithm].allocate_cb,
                     Template_callbacks_map_sw[data->advanced_algorithm].profile_data_get_cb,
                     Template_callbacks_map_sw[data->advanced_algorithm].profile_get_cb,
                     Template_callbacks_map_sw[data->advanced_algorithm].free_cb);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Register the template's data upon creation. 
 * All data is considered to already be checked by sw_state_algo_template_create_verify.
 */
static shr_error_e
sw_state_algo_template_register_data(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    sw_state_algo_template_create_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If this is the first core using this template, we need to save the data to the sw state structure.
     * First core to use the template is always 0 (enforced by input validation).
     */
    if (core_id == 0)
    {
        /*
         * Simply save the data to the sw state structure.
         */
        DNX_SW_STATE_MEMCPY(unit,
                            module_id,
                            algo_temp->flags,
                            &(data->flags), algo_temp->flags, DNXC_SW_STATE_NO_FLAGS, "Template flags.");

        DNX_SW_STATE_MEMCPY(unit,
                            module_id,
                            algo_temp->first_profile,
                            &(data->first_profile),
                            algo_temp->first_profile, DNXC_SW_STATE_NO_FLAGS, "Template first_profile.");

        DNX_SW_STATE_MEMCPY(unit,
                            module_id,
                            algo_temp->nof_profiles,
                            &(data->nof_profiles),
                            algo_temp->nof_profiles, DNXC_SW_STATE_NO_FLAGS, "Template nof_profiles.");

        DNX_SW_STATE_MEMCPY(unit,
                            module_id,
                            algo_temp->data_size,
                            &(data->data_size), algo_temp->data_size, DNXC_SW_STATE_NO_FLAGS, "Template data_size.");

        DNX_SW_STATE_MEMCPY(unit,
                            module_id,
                            algo_temp->default_profile,
                            &(data->default_profile),
                            algo_temp->default_profile, DNXC_SW_STATE_NO_FLAGS, "Template default_profile.");

        DNX_SW_STATE_MEMCPY(unit,
                            module_id,
                            algo_temp->max_references,
                            &(data->max_references),
                            algo_temp->max_references, DNXC_SW_STATE_NO_FLAGS, "Template max_references.");

        DNX_SW_STATE_MEMCPY(unit,
                            module_id,
                            algo_temp->advanced_algorithm,
                            &(data->advanced_algorithm),
                            algo_temp->advanced_algorithm, DNXC_SW_STATE_NO_FLAGS, "Template advanced_algorithm.");

        sw_state_string_strncpy
                (unit, module_id, algo_temp->name, sizeof(char)*(SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH-1), data->name);

        if (_SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
        {
            DNX_SW_STATE_ALLOC(unit,
                               module_id,
                               algo_temp->default_data,
                               DNX_SW_STATE_BUFF, data->data_size, DNXC_SW_STATE_NO_FLAGS, "Template default_data.");

            DNX_SW_STATE_MEMWRITE(unit,
                                  module_id,
                                  data->default_data,
                                  algo_temp->default_data,
                                  0, data->data_size, DNXC_SW_STATE_NO_FLAGS, "Template default_data.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_algo_template_create(
    int unit,
    uint32 module_id,
    sw_state_algo_template_t * algo_temp,
    sw_state_algo_template_create_data_t * data,
    void *extra_arguments,
    uint32 nof_members,
    uint32 alloc_flags)
{
    int current_core, nof_cores_to_use;
    uint8 first_reference;
    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META
                     ("\n sw_state_algo_template_create(unit:%d, name:%s, data->flags:%d, data->first_profile:%d, data->nof_profiles:%d, data->max_references:%d,"),
                     unit, data->name, data->flags, data->first_profile, data->nof_profiles, data->max_references));
        if (_SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                        (BSL_META
                         (" data->default_profile:%d, data->default_data:%p,"),
                         data->default_profile, data->default_data));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META(" data->data_size:%d, extra_arguments:%p)\n"), data->data_size, extra_arguments));
    }

    /*
     * 0. Verify that the template was not allocated and the input data.
     */
    if (*algo_temp != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "The template pointer is already allocated.\n");
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_create_verify(unit, data));

    /*
     * 1. Allocate template.
     */
    DNX_SW_STATE_ALLOC(unit, module_id, *algo_temp, **algo_temp, /* number of elements */ 1, 0,
                       "sw_state algo_temp");

    /*
     * 2. Call the create function and register the template data. This should be done per core if it's a duplicate
     *      per core template.
     *      If it's a separate per core template, this API will be called again and it will be created in that call.
     */

    nof_cores_to_use = (_SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE))
        ? dnx_data_device.general.nof_cores_get(unit) : 1;

    DNX_SW_STATE_ALLOC(unit, module_id, (*algo_temp)->multi_set_template, sw_state_multi_set_t,
                       nof_cores_to_use, 0, "(*algo_res)->multi_set_template");

    for (current_core = 0; current_core < nof_cores_to_use; current_core++)
    {
        /*
         * If we are using the SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE flag for creating the template and
         * if the SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE is not,
         * the max_references should be increased by one.
         */
        if (!_SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE)
            && _SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
        {
            data->max_references = data->max_references + 1;
        }

        /*
         * 2.1 Call the create function.
         */

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[data->advanced_algorithm].create_cb
                        (unit, module_id, &((*algo_temp)->multi_set_template[current_core]), data,
                         extra_arguments, nof_members, alloc_flags));

        if (_SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
        {
            SHR_IF_ERR_EXIT(Template_callbacks_map_sw[data->advanced_algorithm].allocate_cb
                            (unit, module_id, (*algo_temp)->multi_set_template[current_core],
                             SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, data->max_references, data->default_data,
                             NULL, &data->default_profile, &first_reference));
        }

        /*
         * Returnting the real value of max references.
         */
        if (!_SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE)
            && _SHR_IS_FLAG_SET(data->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
        {
            data->max_references = data->max_references - 1;
        }
        /*
         * 2.2 Register the template data.
         */
        SHR_IF_ERR_EXIT(sw_state_algo_template_register_data(unit, current_core, module_id, *algo_temp, data));
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_create(name:%s) Result:%d\n"), (*algo_temp)->name,
                     SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_algo_template_destroy(
    int unit,
    uint32 module_id,
    sw_state_algo_template_t * algo_temp,
    void *extra_arguments)
{
    int current_core, nof_cores_to_use;
    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_destroy(unit:%d, name:%s, extra_arguments:%p)\n"), unit,
                     (*algo_temp)->name, extra_arguments));
    }

    /*
     * Verify that the template exist.
     */
    if ((*algo_temp) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Template doesn't exist.");
    }

    /*
     * 1. Call the destroy function for the algorithm.
     *    This should be done per core for multi core templates.
     */
    nof_cores_to_use = (_SHR_IS_FLAG_SET((*algo_temp)->flags, SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE))
        ? dnx_data_device.general.nof_cores_get(unit) : 1;

    for (current_core = 0; current_core < nof_cores_to_use; current_core++)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[(*algo_temp)->advanced_algorithm].destroy_cb
                        (unit, module_id, &((*algo_temp)->multi_set_template[current_core]), extra_arguments));
    }

    DNX_SW_STATE_FREE(unit, module_id, (*algo_temp)->multi_set_template, "sw_state algo_temp multiset");

    DNX_SW_STATE_FREE(unit, module_id, *algo_temp, "sw_state algo_temp");

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_destroy(name:%s) Result:%d\n"), (*algo_temp)->name,
                     SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

/*
 * Verify that a profile is within legal range of the template.
 */
static shr_error_e
sw_state_algo_template_profile_verify(
    int unit,
    int pool_core,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the template exist.
     */
    if (algo_temp == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Template doesn't exist.");
    }

    /*
     * Verify.
     */
    if (profile < algo_temp->first_profile)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile is too small. Profile is %d and minimum is %d ", profile,
                     algo_temp->first_profile);
    }

    if (profile >= algo_temp->first_profile + algo_temp->nof_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile is too large. Profile is %d and maximum is %d ", profile,
                     algo_temp->first_profile + algo_temp->nof_profiles);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify that template is legal, and supports the given core_id. 
 */
static shr_error_e
sw_state_algo_template_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp)
{
    uint8 multi_core;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the template exist.
     */
    if (algo_temp == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Template doesn't exist.");
    }

    /*
     * Verify core. 
     */
    if ((core_id < 0) && (core_id != _SHR_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core id can't be negative: %d", core_id);
    }

    if ((core_id > 0) && (core_id >= dnx_data_device.general.nof_cores_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core id is too high: given %d but maximum is %d", core_id,
                     dnx_data_device.general.nof_cores_get(unit) - 1);
    }

    /*
     * Multi core templates must be called separately per core. 
     *   Otherwise, must be called with _SHR_CORE_ALL. 
     */
    multi_core = (_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE));

    if (multi_core && core_id == _SHR_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Template must be used one core a time: \"%s.\"", algo_temp->name);
    }

    if (!multi_core && core_id != _SHR_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Template must be used with _SHR_CORE_ALL: \"%s.\"", algo_temp->name);
    }

    /* log this access in multi threading analyzer tool */
    /* for now always log write=TRUE */
    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_log_resource_use(unit,
                             MTA_RESOURCE_TMP_MGR,
                             dnxc_multithread_analyzer_dynamic_sub_res_get(unit, MTA_DYN_SUB_RES_TMP_MGR, algo_temp->name),
                             TRUE)));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify input for sw_state_algo_template_allocate.
 */
static shr_error_e
sw_state_algo_template_allocate_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    uint32 flags,
    void *profile_data,
    int *profile,
    uint8 *first_reference)
{
    int pool_core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile_data, _SHR_E_PARAM, "profile_data");
    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");
    SHR_NULL_CHECK(first_reference, _SHR_E_PARAM, "first_reference")

    /*
     * Verify that the resource exist.
     */
    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

    if (_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Can't use allocate function when default profile is used: \"%s.\"",
                     algo_temp->name);
    }

    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);
        SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_profile_verify(unit, pool_core, module_id, algo_temp, *profile));
    }

    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_MAXIMUM_REFERENCES) && !_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "If ALLOCATE_MAXIMUM_REFERENCES flag is set, then ALLOCATE_WITH_ID must also be set. Given flags are 0x%x", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in sw_state_template_mngr.h.
 */
shr_error_e
sw_state_algo_template_allocate(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    uint32 flags,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{
    int pool_core;
    int nof_ref_to_alloc = 1;
    SW_STATE_MULTI_SET_KEY *tmp_profile_data = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META
                     ("\n sw_state_algo_template_allocate(unit:%d, core_id:%d, name:%s, flags:%d, profile_data:%p, extra_arguments:%p"),
                     unit, core_id, algo_temp->name, flags, profile_data, extra_arguments));
        if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", profile:%d"), *profile));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(")\n")));
    }

    /*
     * Verify input.
     */

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_allocate_verify
                          (unit, core_id, module_id, algo_temp, flags, profile_data, profile, first_reference));

    /*
     * Get the pool_core.
     */
    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    if(_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_MAXIMUM_REFERENCES))
    {
        int current_ref_count = 0;
        tmp_profile_data = sal_alloc(algo_temp->data_size, "Data buffer for tmp template data.");
        SHR_NULL_CHECK(tmp_profile_data, _SHR_E_PARAM, "tmp_profile_data");

        SHR_IF_ERR_EXIT(sw_state_algo_template_profile_data_get
                    (unit, core_id, module_id, algo_temp, *profile, &current_ref_count, tmp_profile_data));

        nof_ref_to_alloc = algo_temp->max_references - current_ref_count;
    }

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
            (unit, module_id, algo_temp->multi_set_template[pool_core], flags, nof_ref_to_alloc, profile_data, extra_arguments, profile, first_reference));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_allocate(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                        (BSL_META(", allocated_profile:%d, first_reference:%d"), *profile, *first_reference));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    SHR_FREE(tmp_profile_data);
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Common verification function for sw_state_algo_template_profile_data_get and sw_state_algo_template_free.
 */
static shr_error_e
sw_state_algo_template_and_profile_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile)
{
    int pool_core;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify template. 
     */
    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    /*
     * Verify profile.
     */
    SHR_IF_ERR_EXIT(sw_state_algo_template_profile_verify(unit, pool_core, module_id, algo_temp, profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verification for sw_state_algo_template_profile_data_get.
 */
static shr_error_e
sw_state_algo_template_profile_data_get_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    int *ref_count,
    void *profile_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (ref_count == NULL && profile_data == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The pointers ref_count=\"%p.\" and profile_data=\"%p.\" can't be NULL at the same time",
                     ref_count, profile_data);
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_and_profile_verify(unit, core_id, module_id, algo_temp, profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See description in sw_state_template_mngr.h.
 */
shr_error_e
sw_state_algo_template_profile_data_get(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    int *ref_count,
    void *profile_data)
{
    int pool_core;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_profile_data_get(unit:%d, core_id:%d, name:%s, profile:%d)\n"),
                     unit, core_id, algo_temp->name, profile));
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_profile_data_get_verify
                          (unit, core_id, module_id, algo_temp, profile, ref_count, profile_data));

    /*
     * Fetch the algo_instance id and call the function from the callback.
     */
    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                    (unit, module_id, algo_temp->multi_set_template[pool_core], profile, ref_count, profile_data));

    if (!_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE)
        && _SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        if (algo_temp->default_profile == profile)
        {
            *ref_count = *ref_count - 1;
        }
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_profile_data_get(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                        (BSL_META(", ref_count:%d, profile_data:%p"), *ref_count, profile_data));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Verification for sw_state_algo_template_free.
 */
static shr_error_e
sw_state_algo_template_free_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(last_reference, _SHR_E_PARAM, "last_reference");

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_and_profile_verify(unit, core_id, module_id, algo_temp, profile));

    /*
     * Verify that free operation is allowed on this template (not allowed when there's 
     *   a default profile) 
     */

    if (_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Can't use allocate function when default profile is used: \"%s.\"",
                     algo_temp->name);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See description in sw_state_template_mngr.h.
 */
shr_error_e
sw_state_algo_template_free(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    uint8 *last_reference)
{
    int pool_core;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_free(unit:%d, core_id:%d, name:%s, profile:%d)\n"), unit,
                     core_id, algo_temp->name, profile));
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_free_verify
                          (unit, core_id, module_id, algo_temp, profile, last_reference));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                    (unit, module_id, algo_temp->multi_set_template[pool_core], profile, 1, last_reference));

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META("\n sw_state_algo_template_free(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", last_reference:%d"), *last_reference));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Verification for sw_state_algo_template_exchange.
 */
static shr_error_e
sw_state_algo_template_exchange_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    uint32 flags,
    const void *profile_data,
    int old_profile,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    int pool_core;
    int ref_count, profile;
    int rv = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(new_profile, _SHR_E_PARAM, "new_profile");
    SHR_NULL_CHECK(first_reference, _SHR_E_PARAM, "last_reference");
    SHR_NULL_CHECK(last_reference, _SHR_E_PARAM, "last_reference");

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

    /*
     * Check flags. 
     * Full explanation is in sw_state_template_mngr.h.
     */
    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
    {
        if (!_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When using SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA flag, "
                         "SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID must also be set.");
        }
    }
    else
    {
        /*
         * If ignore data flag is not set, then profile_data can't be NULL. 
         */
        SHR_NULL_CHECK(profile_data, _SHR_E_PARAM, "profile_data");
    }

    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_ALL))
    {
        if (!_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA)
            || _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When using SW_STATE_ALGO_TEMPLATE_EXCHANGE_ALL flag, "
                         "SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA must also be set and SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE must not.");
        }
    }

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    /*
     * Verify new_profile if WITH_ID.
     */
    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_profile_verify(unit, pool_core, module_id, algo_temp, *new_profile));
    }

    /*
     * Verify old profile.
     */
    SHR_IF_ERR_EXIT(sw_state_algo_template_profile_verify(unit, pool_core, module_id, algo_temp, old_profile));

    if (!_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE))
    {
        SHR_IF_ERR_EXIT(sw_state_algo_template_profile_data_get
                        (unit, core_id, module_id, algo_temp, old_profile, &ref_count, NULL));
        if (ref_count == 0)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "old_profile %d doesn't exist. Please use IGNORE_NOT_EXIST_OLD_PROFILE if that's intentional",
                         old_profile);
        }
    }

    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
        {
            SHR_IF_ERR_EXIT(sw_state_algo_template_profile_data_get
                            (unit, core_id, module_id, algo_temp, *(int *) new_profile, &ref_count, NULL));
            if (ref_count == 0)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "New profile %d doesn't exist and IGNORE_DATA flag is set. Can't add a new profile without data.",
                             *(int *) new_profile);
            }
        }
        else
        {
            rv = sw_state_algo_template_profile_get(unit, core_id, module_id, algo_temp, profile_data, &profile);
            if (rv != _SHR_E_NOT_FOUND && profile != *(int *) new_profile)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Can't have two profiles with the same data. Given data already exists in entry %d, and required profile is %d",
                             profile, *new_profile);
            }
            else
            {
                rv = _SHR_E_NONE;
            }
        }
    }

    if ((_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
        && (!_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE)))
    {
        SHR_IF_ERR_EXIT(sw_state_algo_template_profile_data_get
                        (unit, core_id, module_id, algo_temp, old_profile, &ref_count, NULL));
        if (ref_count == 0 && old_profile == algo_temp->default_profile)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "more references were freed from default profile than it has.");
        }
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verification for sw_state_algo_template_replace_data.
 */
static shr_error_e
sw_state_algo_template_replace_data_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    const void *new_profile_data)
{
    int pool_core;
    int ref_count;
    int rv;
    int tmp_profile;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

    /*
     * The new_profile_data can't be NULL.
     */
    SHR_NULL_CHECK(new_profile_data, _SHR_E_PARAM, "new_profile_data");

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    /*
     * Verify the profile.
     */
    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_profile_verify(unit, pool_core, module_id, algo_temp, profile));

    SHR_IF_ERR_EXIT(sw_state_algo_template_profile_data_get
                    (unit, core_id, module_id, algo_temp, profile, &ref_count, NULL));
    if (ref_count == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "We can't change the data of a non-existing profile.");
    }

    rv = sw_state_algo_template_profile_get(unit, core_id, module_id, algo_temp, new_profile_data, &tmp_profile);
    if (rv == _SHR_E_NONE && tmp_profile != profile)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "Can't have two profiles with the same data. Data already exists in profile %d",
                     tmp_profile);
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        rv = _SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT;
}
/*
 * Replace a data pointer with new data.
 */

static shr_error_e
sw_state_algo_template_generic_replace_data(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    const void *new_profile_data)
{
    SW_STATE_MULTI_SET_KEY *profile_old_data = NULL;
    uint8 old_data_removed = FALSE;
    uint8 new_data_added = FALSE;
    uint8 last_reference, first_reference;
    int pool_core;
    int ref_count;
    SHR_FUNC_INIT_VARS(unit);

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);;

    /*
     * Verify the profile.
     */
    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_profile_verify(unit, pool_core, module_id, algo_temp, profile));

    /*
     *  Get the current data at index (and the ref count to it) and save it into the buffer in case of failure.
     */
    profile_old_data = sal_alloc(algo_temp->data_size, "Old data of new profile..");

    SHR_NULL_CHECK(profile_old_data, _SHR_E_MEMORY, "Failed to allocate pointer profile_old_data.");

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                    (unit, module_id, algo_temp->multi_set_template[pool_core], profile, &ref_count, profile_old_data));

    /*
     *  Free all members using the old data.
     */
    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_all_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_all_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], profile));
    }
    else
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], profile, ref_count,
                         &last_reference));
    }
    old_data_removed = TRUE;

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                    (unit, module_id, algo_temp->multi_set_template[pool_core], SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                     ref_count, (SW_STATE_MULTI_SET_KEY *) new_profile_data, NULL, &profile, &first_reference));
    new_data_added = TRUE;

exit:
    SHR_FREE(profile_old_data);
    if (old_data_removed == TRUE && new_data_added == FALSE)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core],
                         SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, ref_count, profile_old_data, NULL, &profile,
                         &first_reference));
    }
    SHR_FUNC_EXIT;
}
/*
 * See description in sw_state_template_mngr.h.
 */
shr_error_e
sw_state_algo_template_replace_data(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    const void *new_profile_data)
{
    int pool_core;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META
                     ("\n sw_state_algo_template_replace_data(unit:%d, core_id:%d, name:%s, profile:%d, new_profile_data:%p)\n"),
                     unit, core_id, algo_temp->name, profile, new_profile_data));
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_replace_data_verify(unit, core_id, module_id, algo_temp, profile,
                                                                     new_profile_data));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].replace_data_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].replace_data_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], profile, new_profile_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(sw_state_algo_template_generic_replace_data
                        (unit, core_id, module_id, algo_temp, profile, new_profile_data));
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_replace_data(name:%s) Result:%d"), algo_temp->name,
                     SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_algo_template_profile_get_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    const void *profile_data,
    int *profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile_data, _SHR_E_PARAM, "profile_data");
    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_algo_template_profile_get(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    const void *profile_data,
    int *profile)
{
    int pool_core;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_profile_get(unit:%d, core_id:%d, name:%s, profile_data:%p)\n"),
                     unit, core_id, algo_temp->name, profile_data));
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_profile_get_verify
                          (unit, core_id, module_id, algo_temp, profile_data, profile));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    rv = Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_get_cb(unit, module_id,
                                                                                 algo_temp->multi_set_template
                                                                                 [pool_core], profile_data, profile);

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_profile_get(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", profile:%d"), *profile));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_algo_template_clear(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp)
{
    int pool_core;
    int current_profile;
    int max_references;
    uint8 first_reference;
    SW_STATE_MULTI_SET_KEY *default_data = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_clear(unit:%d, core_id:%d, name:%s,)\n"), unit, core_id,
                     algo_temp->name));
    }

    current_profile = 0;
    first_reference = 0;
    max_references = algo_temp->max_references;

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].clear_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].clear_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core]));
    }
    else
    {
        while (TRUE)
        {
            SHR_IF_ERR_EXIT(sw_state_algo_template_get_next(unit, core_id, module_id, algo_temp, &current_profile));
            if (current_profile == SW_STATE_ALGO_TEMPLATE_ILLEGAL_PROFILE)
            {
                break;
            }
            SHR_IF_ERR_EXIT(sw_state_algo_template_free_all(unit, core_id, module_id, algo_temp, current_profile++));
        }
    }

    if (_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        if (!_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE))
        {
            max_references = max_references + 1;
        }

        default_data = sal_alloc(algo_temp->data_size, "Default data backup");
        SHR_NULL_CHECK(default_data, _SHR_E_MEMORY, "default_data");
        sal_memset(default_data, 0, algo_temp->data_size);

        DNX_SW_STATE_MEMREAD(unit,
                             default_data,
                             algo_temp->default_data, 0, algo_temp->data_size, 0, "template_mngr default_data");

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core],
                         SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, max_references, default_data, NULL,
                         &(algo_temp->default_profile), &first_reference));
    }

exit:
    SHR_FREE(default_data);
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_clear(name:%s,) Result:%d\n"), algo_temp->name,
                     SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    SHR_FUNC_EXIT;
}

/*
 * Verification function for sw_state_algo_template_profile_get_next .
 */
static shr_error_e
sw_state_algo_template_get_next_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int *profile)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

    /*
     * Verify profile.
     */
    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "The current profile is not allocated.");

    if (*profile < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The current profile %d is illegal.", *profile);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_algo_template_get_next(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int *current_profile)
{
    int pool_core;
    int profile_end, profile_iter;
    int ref_count;
    SW_STATE_MULTI_SET_KEY *tmp_profile_data = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_get_next(unit:%d, core_id:%d, name:%s)\n"), unit, core_id,
                     algo_temp->name));
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_get_next_verify(unit, core_id, module_id, algo_temp, current_profile));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].get_next_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].get_next_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], current_profile));
    }
    else
    {
        tmp_profile_data = sal_alloc(algo_temp->data_size, "Data buffer for tmp template data.");

        SHR_NULL_CHECK(tmp_profile_data, _SHR_E_PARAM, "tmp_profile_data");

        profile_end = algo_temp->first_profile + algo_temp->nof_profiles;
        profile_iter = UTILEX_MAX(algo_temp->first_profile, *current_profile);

        for (; profile_iter < profile_end; profile_iter++)
        {
            SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                            (unit, module_id, algo_temp->multi_set_template[pool_core], profile_iter, &ref_count,
                             tmp_profile_data));
            if (ref_count > 0)
            {
                break;
            }
        }

        if (profile_iter < profile_end)
        {
            *current_profile = profile_iter;
        }
        else
        {
            *current_profile = SW_STATE_ALGO_TEMPLATE_ILLEGAL_PROFILE;
        }
    }

exit:
    SHR_FREE(tmp_profile_data);
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_get_next(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", current_profile:%d"), *current_profile));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

/*
 * Verification for sw_state_algo_template_free_all.
 */
static shr_error_e
sw_state_algo_template_free_all_verify(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile)
{
    int pool_core;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    /*
     * Verify profile.
     */
    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_profile_verify(unit, pool_core, module_id, algo_temp, profile));

    /*
     * Verify that free operation is allowed on this template (not allowed when there's
     *   a default profile)
     */

    if (_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Can't use free function when default profile is used: \"%s.\"", algo_temp->name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_algo_template_free_all(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile)
{
    int pool_core;
    int ref_count;
    uint8 last_reference;
    SW_STATE_MULTI_SET_KEY *tmp_profile_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_free_all(unit:%d, core_id:%d, name:%s, profile:%d)\n"), unit,
                     core_id, algo_temp->name, profile));
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_free_all_verify(unit, core_id, module_id, algo_temp, profile));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_all_cb != NULL)
    {
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_all_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], profile));
    }
    else
    {
        tmp_profile_data = sal_alloc(algo_temp->data_size, "Data buffer for tmp template data.");
        SHR_NULL_CHECK(tmp_profile_data, _SHR_E_PARAM, "tmp_profile_data");

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], profile, &ref_count,
                         tmp_profile_data));

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], profile, ref_count,
                         &last_reference));
    }

exit:
    SHR_FREE(tmp_profile_data);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_free_all(name:%s) Result:%d\n"), algo_temp->name,
                     SHR_GET_CURRENT_ERR()));
    }

    SHR_FUNC_EXIT;
}

/*
 * This function frees one reference from old_profile, add one reference to data.
 */
static shr_error_e
sw_state_algo_template_generic_exchange(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    uint32 flags,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    SW_STATE_MULTI_SET_KEY *old_profile_data = NULL, *new_profile_old_data = NULL;
    int ref_count_old;
    int new_profile_old_ref_count;
    int nof_ref_to_exchange;
    uint8 old_profile_freed = FALSE;
    uint8 add_success = TRUE;
    int pool_core;

    SHR_FUNC_INIT_VARS(unit);

    nof_ref_to_exchange = 1;

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    /*
     * First, we save the old data, to restore it in case the allocation fails, or in case of test.
     */
    old_profile_data = sal_alloc(algo_temp->data_size, "Data buffer old.");

    SHR_NULL_CHECK(old_profile_data, _SHR_E_PARAM, "old_profile_data");

    SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                    (unit, module_id, algo_temp->multi_set_template[pool_core], old_profile, &ref_count_old,
                     old_profile_data));

    if ((ref_count_old == 0) && !_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE))
    {
        /*
         * User gave old profile that was empty
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given old_profile %d doesn't exist.", old_profile);
    }

    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_ALL))
    {
        nof_ref_to_exchange = ref_count_old;
    }

    if (ref_count_old != 0)
    {
        /*
         *  Remove old profile if it exists.
         */

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], old_profile, nof_ref_to_exchange,
                         last_reference));

        old_profile_freed = TRUE;
    }

    /*
     * WITH_ID means that the user would like to exchange the current profile for a specific new profile
     *   (as opposed to changing the profile to any new profile containing the supplied data).
     * If IGNORE_DATA is not specified, then the user would also like to change the value of the new template.
     */
    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {

        /*
         *  Get the current data at index (and the ref count to it) and save it into the buffer in case of failure.
         */
        new_profile_old_data = sal_alloc(algo_temp->data_size, "Old data of new profile..");

        SHR_NULL_CHECK(new_profile_old_data, _SHR_E_PARAM, "new_profile_old_data");

        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].profile_data_get_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], *new_profile,
                         &new_profile_old_ref_count, new_profile_old_data));

        /*
         *  If we ignore the data or we have a new profile, then we need not change the existing profile value.
         */
        if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA) || (new_profile_old_ref_count > 0))
        {
            /*
             *  Ignore data or data already exists, take the existing data.
             */
            SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                            (unit, module_id, algo_temp->multi_set_template[pool_core], flags, nof_ref_to_exchange,
                             new_profile_old_data, (void *) extra_arguments, new_profile, first_reference));
            add_success = TRUE;
        }
        else
        {
            /*
             * Use the new data.
             */
            SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                            (unit, module_id, algo_temp->multi_set_template[pool_core], flags, nof_ref_to_exchange,
                             (void *) profile_data, (void *) extra_arguments, new_profile, first_reference));
            add_success = TRUE;
        }
    }
    else        /* if (!WITH_ID) */
    {
        /*
         *  Add new data.
         */
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], flags, nof_ref_to_exchange,
                         (void *) profile_data, (void *) extra_arguments, new_profile, first_reference));
        add_success = TRUE;

    }

    /*
     * If this is just an exchange test, then free the entries that were just allocated.
     */
    if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_TEST))
    {
        uint8 test_last_reference;

        /*
         * Deallocate only one reference.
         */
        SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].free_cb
                        (unit, module_id, algo_temp->multi_set_template[pool_core], *new_profile, nof_ref_to_exchange,
                         &test_last_reference));

    }

exit:
    if (SHR_FUNC_ERR() || !add_success || _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_EXCHANGE_TEST))
    {
        int rv;
        /*
         *  Add new failed, or test. Restore old data.
         */
        if (!add_success)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_FULL);
        }

        if (old_profile_freed)
        {
            uint8 dummy_first_reference;

            rv = Template_callbacks_map_sw[algo_temp->advanced_algorithm].allocate_cb(unit,
                                                                                      module_id,
                                                                                      algo_temp->multi_set_template
                                                                                      [pool_core],
                                                                                      SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                                                                                      nof_ref_to_exchange,
                                                                                      old_profile_data,
                                                                                      (void *) extra_arguments,
                                                                                      &old_profile,
                                                                                      &dummy_first_reference);
            if (SHR_FAILURE(rv))
            {
                /*
                 *  Can't restore data. Internal error
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            }
        }
    }

    /*
     * Free pointers.
     */
    SHR_FREE(old_profile_data);
    SHR_FREE(new_profile_old_data);
    SHR_FUNC_EXIT;
}

/*
 * See description in sw_state_template_mngr.h.
 */
shr_error_e
sw_state_algo_template_exchange(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    uint32 flags,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    int pool_core;
    int rv, tmp_profile;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META
                     ("\n sw_state_algo_template_exchange(unit:%d, core_id:%d, name:%s, flags:%d, profile_data:%p, old_profile:%d"),
                     unit, core_id, algo_temp->name, flags, profile_data, old_profile));
        if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(", new_profile:%d"), *new_profile));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(")\n")));
    }

    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_exchange_verify
                          (unit, core_id, module_id, algo_temp, flags, profile_data, old_profile, new_profile,
                           first_reference, last_reference));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    /*
     * Special case - if the data of the new profile already exists in the old profile, 
     * then we just remove and add a pointer to the same profile, which is actually meaningless.
     * In this case, we just return the old profile and no first/last indication.
     */
    rv = sw_state_algo_template_profile_get(unit, core_id, module_id, algo_temp, profile_data, &tmp_profile);
    if (rv == _SHR_E_NOT_FOUND)
    {
        /*
         * If the new data doesn't exist, then the special case is irrelevant. Just ignore it.
         */
        tmp_profile = SW_STATE_ALGO_TEMPLATE_ILLEGAL_PROFILE;
        rv = _SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(rv);

    if (tmp_profile == old_profile)
    {
        /*
         * Special case fulfilled.
         */
        *new_profile = old_profile;
        *first_reference = FALSE;
        *last_reference = FALSE;
    }
    else
    {
        if (Template_callbacks_map_sw[algo_temp->advanced_algorithm].exchange_cb != NULL)
        {
            SHR_IF_ERR_EXIT(Template_callbacks_map_sw[algo_temp->advanced_algorithm].exchange_cb
                            (unit, module_id, algo_temp->multi_set_template[pool_core], flags, 1, profile_data,
                             old_profile, extra_arguments, new_profile, first_reference, last_reference));
        }
        else
        {
            SHR_IF_ERR_EXIT(sw_state_algo_template_generic_exchange
                            (unit, core_id, module_id, algo_temp, flags, (void *) profile_data, old_profile,
                             (void *) extra_arguments, new_profile, first_reference, last_reference));
        }
    }

    /*
     * If a profile was cleared, then used again, then the HW entry can just be overridden. No need to indicate that it's
     * last because it won't be deleted.
     */
    if (*first_reference && *last_reference && old_profile == *new_profile)
    {
        *last_reference = FALSE;
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_TEMPLATEMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                    (BSL_META("\n sw_state_algo_template_exchange(name:%s"), algo_temp->name));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR,
                        (BSL_META(", allocated new_profile:%d, first_reference:%d, last_reference:%d"), *new_profile,
                         *first_reference, *last_reference));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_TEMPLATEMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR);
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_algo_template_dump_info_get(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    sw_state_algo_template_dump_data_t * info)
{
    int current_profile;
    int pool_core;

    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META
                     ("\n sw_state_algo_template_dump_info_get(unit:%d, core_id:%d, name:%s)\n"), unit, core_id,
                     algo_temp->name));
    }

    /*
     * Verify template.
     */
    SHR_INVOKE_VERIFY_DNX(sw_state_algo_template_verify(unit, core_id, module_id, algo_temp));

    pool_core = SW_STATE_ALGO_TEMPLATE_GET_POOL_CORE(core_id);

    info->create_data.flags = algo_temp->flags;
    info->create_data.first_profile = algo_temp->first_profile;
    info->create_data.nof_profiles = algo_temp->nof_profiles;
    info->create_data.data_size = algo_temp->data_size;
    info->create_data.default_profile = algo_temp->default_profile;
    info->create_data.max_references = algo_temp->max_references;
    info->create_data.advanced_algorithm = algo_temp->advanced_algorithm;

    current_profile = info->create_data.first_profile;
    info->nof_used_profiles = 0;
    info->nof_free_profiles = 0;

    while (1)
    {
        if (_SHR_IS_FLAG_SET(info->create_data.flags, SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE))
        {
            SHR_IF_ERR_EXIT(sw_state_algo_template_get_next(unit, pool_core, module_id, algo_temp, &current_profile));
        }
        else
        {
            SHR_IF_ERR_EXIT(sw_state_algo_template_get_next(unit, core_id, module_id, algo_temp, &current_profile));
        }

        if (current_profile == SW_STATE_ALGO_TEMPLATE_ILLEGAL_PROFILE)
        {
            break;
        }

        info->nof_used_profiles++;
        current_profile++;
    }
    info->nof_free_profiles = info->create_data.nof_profiles - info->nof_used_profiles;

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n sw_state_algo_template_dump_info_get(name:%s) Result:%d\n"), algo_temp->name,
                     SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_algo_template_data_print(
    int unit,
    int profile_number,
    int ref_count,
    dnx_algo_template_print_t * print_cb_data)
{
    int var_count = 0;
    int sub_struct_offset;
    char string_to_print[DNX_ALGO_TEMPLATE_PRINT_CB_MAX_PRINT_STRING_SIZE] = { 0 };

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNX_SW_STATE_PRINT(unit, "| Profile ID:%d: |", profile_number);
    DNX_SW_STATE_PRINT(unit, " Nof ref:%d: |", ref_count);

    while (print_cb_data[var_count].type != TEMPLATE_MNGR_PRINT_TYPE_COUNT)
    {
        DNX_SW_STATE_PRINT(unit, "\n\t\t\t      ");
        sub_struct_offset = 0;
        sub_struct_offset = print_cb_data[var_count].sub_struct_level;
        while (sub_struct_offset--)
        {
            DNX_SW_STATE_PRINT(unit, "  ");
        }
        DNX_SW_STATE_PRINT(unit, "|");

        if (print_cb_data[var_count].name != NULL)
        {
            DNX_SW_STATE_PRINT(unit, " Field Name :");
            if (print_cb_data[var_count].is_arr_flag)
            {
                sal_sprintf(string_to_print, "%s[%d]", print_cb_data[var_count].name,
                            print_cb_data[var_count].arr_index);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %s |", print_cb_data[var_count].name);
            }
        }

        DNX_SW_STATE_PRINT(unit, " Value :");
        switch (print_cb_data[var_count].type)
        {
            case TEMPLATE_MNGR_PRINT_TYPE_UINT8:
            {
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                *(uint8 *) print_cb_data[var_count].data);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %u |", *(uint8 *) print_cb_data[var_count].data);
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_UINT16:
            {
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                *(uint16 *) print_cb_data[var_count].data);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %u |", *(uint16 *) print_cb_data[var_count].data);
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_UINT32:
            {
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                *(uint32 *) print_cb_data[var_count].data);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %u |", *(uint32 *) print_cb_data[var_count].data);
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_CHAR:
            {
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                *(char *) print_cb_data[var_count].data);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %c |", *(char *) print_cb_data[var_count].data);
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_SHORT:
            {
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                *(short *) print_cb_data[var_count].data);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %hu |", *(short *) print_cb_data[var_count].data);
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_INT:
            {
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                *(int *) print_cb_data[var_count].data);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %d |", *(int *) print_cb_data[var_count].data);
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_MAC:
            {
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                print_cb_data[var_count].data[5], print_cb_data[var_count].data[4],
                                print_cb_data[var_count].data[3], print_cb_data[var_count].data[2],
                                print_cb_data[var_count].data[1], print_cb_data[var_count].data[0]);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %02X:%02X:%02X:%02X:%02X:%02X |",
                                        print_cb_data[var_count].data[5], print_cb_data[var_count].data[4],
                                        print_cb_data[var_count].data[3], print_cb_data[var_count].data[2],
                                        print_cb_data[var_count].data[1], print_cb_data[var_count].data[0]);
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_IPV4:
            {
                bcm_ip_t ipv4;
                sal_memcpy(&ipv4, print_cb_data[var_count].data, sizeof(bcm_ip_t));
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string, (ipv4 >> 24) & 0xff,
                                (ipv4 >> 16) & 0xff, (ipv4 >> 8) & 0xff, ipv4 & 0xff);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %d.%d.%d.%d |",
                                        (ipv4 >> 24) & 0xff, (ipv4 >> 16) & 0xff, (ipv4 >> 8) & 0xff, ipv4 & 0xff);
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_IPV6:
            {
                bcm_ip6_t ipv6;
                sal_memcpy(ipv6, print_cb_data[var_count].data, sizeof(ipv6));
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                (((uint16) ipv6[0] << 8) | ipv6[1]), (((uint16) ipv6[2] << 8) | ipv6[3]),
                                (((uint16) ipv6[4] << 8) | ipv6[5]), (((uint16) ipv6[6] << 8) | ipv6[7]),
                                (((uint16) ipv6[8] << 8) | ipv6[9]), (((uint16) ipv6[10] << 8) | ipv6[11]),
                                (((uint16) ipv6[12] << 8) | ipv6[13]), (((uint16) ipv6[14] << 8) | ipv6[15]));
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %x:%x:%x:%x:%x:%x:%x:%x |",
                                        (((uint16) ipv6[0] << 8) | ipv6[1]), (((uint16) ipv6[2] << 8) | ipv6[3]),
                                        (((uint16) ipv6[4] << 8) | ipv6[5]), (((uint16) ipv6[6] << 8) | ipv6[7]),
                                        (((uint16) ipv6[8] << 8) | ipv6[9]), (((uint16) ipv6[10] << 8) | ipv6[11]),
                                        (((uint16) ipv6[12] << 8) | ipv6[13]), (((uint16) ipv6[14] << 8) | ipv6[15]));
                }
                break;
            }
            case TEMPLATE_MNGR_PRINT_TYPE_STRING:
            {
                char string_var[DNX_ALGO_TEMPLATE_PRINT_CB_MAX_PRINT_STRING_SIZE];
                sal_memcpy(string_var, print_cb_data[var_count].data,
                           DNX_ALGO_TEMPLATE_PRINT_CB_MAX_PRINT_STRING_SIZE - 1);
                if (print_cb_data[var_count].format_string != NULL)
                {
                    sal_sprintf(string_to_print, print_cb_data[var_count].format_string,
                                (char *) print_cb_data[var_count].data);
                    DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);

                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, " %s |", (char *) print_cb_data[var_count].data);
                }
                break;
            }
            default:
            {
                DNX_SW_STATE_PRINT(unit, " Invalid Type. |");

                break;
            }
        }

        if (print_cb_data[var_count].comment != NULL)
        {
            DNX_SW_STATE_PRINT(unit, " Info: %s |", print_cb_data[var_count].comment);
        }

        var_count++;
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

int
sw_state_algo_template_print(
    int unit,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    dnx_algo_template_print_data_cb print_cb)
{
    sw_state_algo_template_dump_data_t data;
    dnx_algo_template_print_t *print_cb_data = NULL;
    void *tmp_data = NULL;
    int real_core_id, current_core;
    int end;
    int nof_cores;
    int current_profile;
    int tmp_ref_count;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the template exist.
     */
    if (algo_temp == NULL)
    {
        SHR_EXIT();
    }

    print_cb_data = sal_alloc(sizeof(dnx_algo_template_print_t) * DNX_ALGO_TEMPLATE_PRINT_CB_MAX_SIZE, "print_cb_data");
    sal_memset(print_cb_data, 0, sizeof(dnx_algo_template_print_t) * DNX_ALGO_TEMPLATE_PRINT_CB_MAX_SIZE);
    print_cb_data[0].type = TEMPLATE_MNGR_PRINT_TYPE_COUNT;

    nof_cores = (_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE)) ?
        dnx_data_device.general.nof_cores_get(unit) : 1;

    DNX_SW_STATE_PRINT(unit, "\n Template :%s", algo_temp->name);
    for (current_core = 0 ; current_core < nof_cores; current_core++)
    {
        real_core_id =
            (_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE)) ?
            current_core : _SHR_CORE_ALL;

        SHR_IF_ERR_EXIT(sw_state_algo_template_dump_info_get(unit, real_core_id, module_id, algo_temp, &data));

        if (current_core == 0) {
            DNX_SW_STATE_PRINT(unit, " (Advanced Algorithm - %s)", (data.create_data.advanced_algorithm==DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC ? "No" : "Yes"));
        }

        if (_SHR_IS_FLAG_SET(algo_temp->flags, SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE))
        {
            DNX_SW_STATE_PRINT(unit, "\n| Core ID:%d: ", real_core_id);
        }
        DNX_SW_STATE_PRINT(unit, "| First profile:%d: ", algo_temp->first_profile);
        DNX_SW_STATE_PRINT(unit, "| Nof profiles:%d: ", algo_temp->nof_profiles);
        DNX_SW_STATE_PRINT(unit, "| Nof profiles in use:%d: ", data.nof_used_profiles);
        DNX_SW_STATE_PRINT(unit, "| Nof free profiles:%d:|\n", data.nof_free_profiles);

        if (data.nof_used_profiles)
        {
            current_profile = algo_temp->first_profile;
            end = algo_temp->nof_profiles + algo_temp->first_profile;

            tmp_ref_count = 0;
            tmp_data = sal_alloc(algo_temp->data_size, "Tmp data");
            SHR_NULL_CHECK(tmp_data, _SHR_E_MEMORY, "tmp_data");

            while (1)
            {
                SHR_IF_ERR_EXIT(sw_state_algo_template_get_next
                                (unit, real_core_id, module_id, algo_temp, &current_profile));

                if ((current_profile == SW_STATE_ALGO_TEMPLATE_ILLEGAL_PROFILE) || (current_profile > end))
                {
                    break;
                }

                SHR_IF_ERR_EXIT(sw_state_algo_template_profile_data_get
                                (unit, real_core_id, module_id, algo_temp, current_profile, &tmp_ref_count, tmp_data));

                if (print_cb != NULL)
                {
                    print_cb(unit, tmp_data, print_cb_data);
                    DNX_SW_STATE_PRINT(unit, "\n");

                    SHR_IF_ERR_EXIT(sw_state_algo_template_data_print
                                    (unit, current_profile, tmp_ref_count, print_cb_data));
                }
                else
                {
                    DNX_SW_STATE_PRINT(unit, "The print callback is not set.\n");
                }

                current_profile++;
            }
        }
    }
    DNX_SW_STATE_PRINT(unit, "\n");

exit:
    SHR_FREE(print_cb_data);
    SHR_FREE(tmp_data);
    SHR_FUNC_EXIT;
}

#else
/*
 * {
 */

/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e
sw_state_algo_temp_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/*
 * }
 */
#endif /* BCM_DNX_SUPPORT */
