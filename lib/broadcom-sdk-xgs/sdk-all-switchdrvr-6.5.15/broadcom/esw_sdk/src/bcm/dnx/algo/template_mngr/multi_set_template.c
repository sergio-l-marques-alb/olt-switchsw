/** \file multi_set_template.c
 *
 * Wrapper functions for sw_state_multi_set.
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
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * Other include files. 
 * { 
 */
#include <bcm/types.h>
#include <shared/swstate/sw_state.h>

#include <bcm_int/dnx/algo/template_mngr/multi_set_template.h>

shr_error_e
multi_set_template_create(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t * multi_set_template,
    sw_state_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_members,
    uint32 alloc_flags)
{
    sw_state_multi_set_info_t multi_set_init_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init multi set.
     */
    sal_memset(&multi_set_init_info, 0x0, sizeof(sw_state_multi_set_info_t));
    multi_set_init_info.max_duplications = create_data->max_references;
    multi_set_init_info.member_size = create_data->data_size;
    multi_set_init_info.nof_members = create_data->nof_profiles;
    multi_set_init_info.first_profile = create_data->first_profile;
    multi_set_init_info.data_size = create_data->data_size;

    SHR_IF_ERR_EXIT(sw_state_multi_set_create
                    (unit, module_id, multi_set_template, &multi_set_init_info, nof_members, alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_template_destroy(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t * multi_set_template,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_template_allocate(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{
    uint8 add_success;
    int profile_alloc = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (nof_references > 1)
    {
        if (!_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Can't allocate several entries without SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID.");
        }

        profile_alloc = *profile - multi_set_template->init_info.first_profile;

        SHR_IF_ERR_EXIT(sw_state_multi_set_member_add_at_index_nof_additions
                        (unit, module_id, multi_set_template, (SW_STATE_MULTI_SET_KEY *) profile_data,
                         (uint32) profile_alloc, nof_references, first_reference, &add_success));

    }
    else if (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        profile_alloc = *profile - multi_set_template->init_info.first_profile;

        SHR_IF_ERR_EXIT(sw_state_multi_set_member_add_at_index
                        (unit, module_id, multi_set_template, (SW_STATE_MULTI_SET_KEY *) profile_data,
                         (uint32) profile_alloc, first_reference, &add_success));
    }
    else
    {
        /*
         * Add new data
         */
        SHR_IF_ERR_EXIT(sw_state_multi_set_member_add
                        (unit, module_id, multi_set_template, (SW_STATE_MULTI_SET_KEY *) profile_data,
                         (uint32 *) &profile_alloc, first_reference, &add_success));
        *profile = profile_alloc + multi_set_template->init_info.first_profile;
    }

    if (!add_success)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Failed to allocate profile.");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_template_free(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference)
{
    int profile_alloc = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Remove old profile
     */
    profile_alloc = profile - multi_set_template->init_info.first_profile;
    SHR_IF_ERR_EXIT(sw_state_multi_set_member_remove_by_index_multiple
                    (unit, module_id, multi_set_template, profile_alloc, nof_references, last_reference));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_template_free_all(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    int profile)
{
    uint32 ref_count;
    uint8 last_appear;
    SW_STATE_MULTI_SET_KEY *tmp_profile_data = NULL;
    SHR_FUNC_INIT_VARS(unit);

    tmp_profile_data = sal_alloc(multi_set_template->init_info.data_size, "Data buffer for tmp template data.");

    SHR_NULL_CHECK(tmp_profile_data, _SHR_E_PARAM, "tmp_profile_data");

    SHR_IF_ERR_EXIT(sw_state_multi_set_get_by_index
                    (unit, module_id, multi_set_template, profile, tmp_profile_data, &ref_count));

    SHR_IF_ERR_EXIT(sw_state_multi_set_member_remove_by_index_multiple
                    (unit, module_id, multi_set_template, profile, ref_count, &last_appear));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_template_profile_data_get(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    int profile,
    int *ref_count,
    void *data)
{
    int profile_alloc;

    SHR_FUNC_INIT_VARS(unit);

    profile_alloc = profile - multi_set_template->init_info.first_profile;

    SHR_IF_ERR_EXIT(sw_state_multi_set_get_by_index
                    (unit, module_id, multi_set_template, profile_alloc, data, (uint32 *) ref_count));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_template_profile_get(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    const void *data,
    int *profile)
{
    uint32 profile_alloc;
    uint32 ref_count;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_multi_set_member_lookup
                    (unit, module_id, multi_set_template, (SW_STATE_MULTI_SET_KEY *) data, &profile_alloc, &ref_count));

    if (ref_count == 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }
    else
    {
        *profile = profile_alloc + multi_set_template->init_info.first_profile;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_template_clear(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_multi_set_clear(unit, module_id, multi_set_template));

exit:
    SHR_FUNC_EXIT;
}
