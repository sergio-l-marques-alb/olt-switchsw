/** \file smart_template.c
 *
 * A template manager algorithm that is managed by a resource manager.
 *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/multi_set_template.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/res_mngr/simple_bitmap_wrap.h>

/** } **/

/** 
 *  Smart template advanced algorithm.
 *  
 *  The algorithm works as following:
 *  
 *  We hold a res tag bitmap that has the same ID range as the template.
 *  When allocating an entry for the first time, we allocate it in the res tag bitmap, since it allows
 *   us to allocate according to specific user requirements, like ranged allocation, tagging, allocating
 *   several indexes per entry, etc., functionalities that don't exist in template manager.
 *   Then we use the ID allocated in the resource manager to save the entry in the template manager.
 *  
 *  {
 */

shr_error_e
dnx_algo_smart_template_create(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t * multi_set_template,
    sw_state_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_members,
    uint32 alloc_flags)
{
    sw_state_algo_res_create_data_t bitmap_create_info;
    smart_template_create_info_t *extra_create_info = (smart_template_create_info_t *) extra_arguments;
    uint32 res_flags;
    simple_bitmap_wrap_create_info_t *resource_extra_create_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * 1. Create a regular multi_set_template.
     */
    SHR_IF_ERR_EXIT(multi_set_template_create
                    (unit, module_id, multi_set_template, create_data, NULL, nof_members, alloc_flags));

    /*
     * 2. Create a res_tag_bitmap to manage the entries.
     *    Use the simple bitmap wrap interface because it already holds BCM flags for the
     *    inner algorithm.
     */
    sal_memset(&bitmap_create_info, 0, sizeof(sw_state_algo_res_create_data_t));
    bitmap_create_info.first_element = create_data->first_profile;
    bitmap_create_info.nof_elements = create_data->nof_profiles;

    if (extra_create_info != NULL)
    {
        res_flags = extra_create_info->resource_flags;
        resource_extra_create_info = &extra_create_info->resource_create_info;
    }
    else
    {
        res_flags = 0;
        resource_extra_create_info = NULL;
    }

    SHR_IF_ERR_EXIT(dnx_algo_res_simple_bitmap_create(unit, module_id, &((*multi_set_template)->allocation_bitmap),
                                                      _SHR_CORE_ALL, 0, &bitmap_create_info, resource_extra_create_info,
                                                      0, res_flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_allocate(
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
    int tmp_profile, rv;
    uint8 allocation_needed = TRUE, with_id;
    smart_template_alloc_info_t *extra_alloc_info;
    uint32 resource_flags;
    simple_bitmap_wrap_alloc_info_t *resource_alloc_info;
    SHR_FUNC_INIT_VARS(unit);

    with_id = (_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID));

    extra_alloc_info = (smart_template_alloc_info_t *) extra_arguments;
    if (extra_alloc_info != NULL)
    {
        resource_flags = extra_alloc_info->resource_flags;
        resource_alloc_info = &extra_alloc_info->resource_alloc_info;
    }
    else
    {
        resource_alloc_info = NULL;
        resource_flags = 0;
    }

    /*
     * 1. Check if the data already exists or if it needs to be allocated.
     */
    rv = multi_set_template_profile_get(unit, module_id, multi_set_template, profile_data, &tmp_profile);

    /*
     * If E_NOT_FOUND was returned, it means the data wasn't allocated yet and we need to allocate it. 
     * Otherwise, check for errors, and if none were returned, it means the data is already allocated. 
     * If WITH_ID, make sure the existing profile match the given profile.
     * Otherwise, if some allocation criteria was given, verify that the existing profile matches these
     *  criteria.
     * If no errors were found, save the profile we found and use it with the multi set template.
     */
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(rv);
        if (with_id && (*profile != tmp_profile))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "This data already exists in profile %d. Given profile %d", tmp_profile,
                         *profile);
        }

        if (resource_alloc_info != NULL)
        {
            /*
             * We want to verify that the allocation info given matches the actual allocated entry.
             * Run an allocation simulation WITH_ID.
             * This will trigger all the bitmap input verification, and will fail if the input info doesn't match
             * the given entry.
             * If all the verification succeeds, we'll get E_RESOURCE.
             */
            uint32 tmp_flags = resource_flags
                | SW_STATE_ALGO_RES_ALLOCATE_WITH_ID | SW_STATE_ALGO_RES_ALLOCATE_SIMULATION;
            rv = dnx_algo_res_simple_bitmap_allocate(unit, module_id, multi_set_template->allocation_bitmap,
                                                     tmp_flags, resource_alloc_info, &tmp_profile);

            if (rv != _SHR_E_RESOURCE)
            {
                SHR_ERR_EXIT(rv, "The given data is already allocated on profile %d, and the given"
                             "allocation information (in extra_arguments) doesn't match the existing entry.",
                             tmp_profile);
            }
        }

        allocation_needed = FALSE;
        *profile = tmp_profile;
    }

    /*
     * 2. If data doesn't already exist, allocate in the res tag bitmap.
     */
    if (allocation_needed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_res_simple_bitmap_allocate(unit, module_id, multi_set_template->allocation_bitmap,
                                                            resource_flags, resource_alloc_info, &tmp_profile));
        *profile = tmp_profile;
    }

    /*
     * 3. Use the allocated profile for the multi set template WITH_ID.
     */
    SHR_IF_ERR_EXIT(multi_set_template_allocate
                    (unit, module_id, multi_set_template, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, nof_references,
                     profile_data, NULL, profile, first_reference));

    /*
     * Sanity check: allocation_needed and first_reference should be the same.
     */
    if (allocation_needed != *first_reference)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Algorithm inconsistency. allocation_needed: %d, first_reference: %d",
                     allocation_needed, *first_reference);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_free(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free from the multi set template.
     */
    SHR_IF_ERR_EXIT(multi_set_template_free
                    (unit, module_id, multi_set_template, profile, nof_references, last_reference));

    
    if (*last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_algo_res_simple_bitmap_free(unit,
                                                        module_id, multi_set_template->allocation_bitmap, profile));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_clear(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Clear template and resource.
     */
    SHR_IF_ERR_EXIT(multi_set_template_clear(unit, module_id, multi_set_template));

    SHR_IF_ERR_EXIT(dnx_algo_res_simple_bitmap_clear(unit, module_id, multi_set_template->allocation_bitmap));

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
