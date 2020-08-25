/*! \file simple_bitmap_wrap.c
 *
 * Resource manager wrapper functions for simple bitmap.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR
/*!
* INCLUDE FILES:
* {
*/
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_callbacks.h>
#include <bcm_int/dnx/algo/res_mngr/simple_bitmap_wrap.h>
/*
 * }
 */
#include <bcm/types.h>
#include <bcm/error.h>

/** 
 * }
 */

shr_error_e
dnx_algo_res_simple_bitmap_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    int core_id,
    int sub_resource_index,
    sw_state_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    sw_state_resource_tag_bitmap_create_info_t res_tag_bitmap_create_info;
    simple_bitmap_wrap_create_info_t *simple_bitmap_wrap_create_info;
    uint32 _flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(create_data, _SHR_E_PARAM, "create_data");

    /*
     * Init local variables.
     */
    _flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;
    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(sw_state_resource_tag_bitmap_create_info_t));

    if (extra_arguments == NULL)
    {
        res_tag_bitmap_create_info.grain_size = create_data->nof_elements;
        res_tag_bitmap_create_info.max_tag_value = 0;
        res_tag_bitmap_create_info.flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;
    }
    else
    {
        simple_bitmap_wrap_create_info = (simple_bitmap_wrap_create_info_t *) extra_arguments;
        res_tag_bitmap_create_info.grain_size = simple_bitmap_wrap_create_info->grain_size;
        res_tag_bitmap_create_info.max_tag_value = simple_bitmap_wrap_create_info->max_tag_value;
    }

    _flags |= (_SHR_IS_FLAG_SET(create_data->flags, DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_ALLOW_IGNORING_TAG)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG : 0;

    _flags |= (_SHR_IS_FLAG_SET(create_data->flags, DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG : 0;

    _flags |= (_SHR_IS_FLAG_SET(create_data->flags, DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN : 0;

    _flags |= (_SHR_IS_FLAG_SET(create_data->flags, DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_ALLOW_FORCING_TAG)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG : 0;

    res_tag_bitmap_create_info.flags = _flags;

    res_tag_bitmap_create_info.low_id = create_data->first_element;
    res_tag_bitmap_create_info.count = create_data->nof_elements;

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_create
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_create_info, nof_elements, alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_destroy(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_destroy(unit, module_id, &res_tag_bitmap));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_allocate_several(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element)
{
    uint32 internal_flags;
    uint8 with_id;
    uint8 alloc_simulation;
    int rv;
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    simple_bitmap_wrap_alloc_info_t *simple_bitmap_wrap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");

    /*
     * Translate the allocation flags.
     * The input flags are of type SW_STATE_ALGO_RES_ALLOCATE_*, but the sw_state_res_tag_bitmap use a
     * different set of flags.
     */
    with_id = _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID);
    alloc_simulation = _SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_SIMULATION);
    internal_flags = 0;
    internal_flags |= (with_id) ? SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID : 0;
    internal_flags |= (alloc_simulation) ? SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY : 0;

    internal_flags |= (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_ALIGN_ZERO)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO : 0;

    internal_flags |= (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_ALIGN)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN : 0;

    internal_flags |= (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_SIMPLE_BITMAP_ALWAYS_CHECK_TAG)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG : 0;

    internal_flags |= (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_IGNORE_TAG)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG : 0;

    internal_flags |= (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_SPARSE)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_SPARSE : 0;

    internal_flags |= (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_IN_RANGE)) ?
        SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE : 0;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));

    if (extra_arguments != NULL)
    {
        simple_bitmap_wrap_alloc_info = (simple_bitmap_wrap_alloc_info_t *) extra_arguments;
        res_tag_bitmap_alloc_info.align = simple_bitmap_wrap_alloc_info->align;
        sal_memcpy(res_tag_bitmap_alloc_info.offs, simple_bitmap_wrap_alloc_info->offs,
                   sizeof(int) * SW_STATE_RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS);
        res_tag_bitmap_alloc_info.nof_offsets = simple_bitmap_wrap_alloc_info->nof_offsets;
        res_tag_bitmap_alloc_info.tag = simple_bitmap_wrap_alloc_info->tag;
        res_tag_bitmap_alloc_info.pattern = simple_bitmap_wrap_alloc_info->pattern;
        res_tag_bitmap_alloc_info.length = simple_bitmap_wrap_alloc_info->length;
        res_tag_bitmap_alloc_info.repeats = simple_bitmap_wrap_alloc_info->repeats;
        /** Transfer the range_start and range_end parameters if ALLOC_IN_RANGE flag is defined. */
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_IN_RANGE))
        {
            res_tag_bitmap_alloc_info.range_start = simple_bitmap_wrap_alloc_info->range_start;
            res_tag_bitmap_alloc_info.range_end = simple_bitmap_wrap_alloc_info->range_end;
        }
    }

    res_tag_bitmap_alloc_info.flags = internal_flags;
    res_tag_bitmap_alloc_info.count = nof_elements;

    rv = sw_state_resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element);

    if (!alloc_simulation)
        SHR_IF_ERR_EXIT(rv);
    else
        SHR_SET_CURRENT_ERR(rv);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_allocate(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dnx_algo_res_simple_bitmap_allocate_several(unit, module_id, res_tag_bitmap, flags, 1, extra_arguments,
                                                     element);

    if (!_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_SIMULATION))
        SHR_IF_ERR_EXIT(rv);
    else
        return rv;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_is_allocated(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = sw_state_resource_tag_bitmap_check(unit, module_id, res_tag_bitmap, 1, element);
    if (rv == _SHR_E_NOT_FOUND)
    {
        *is_allocated = FALSE;
    }
    else if (rv == _SHR_E_EXISTS)
    {
        *is_allocated = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_free_several(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int element)
{
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_free_info.count = nof_elements;

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_free
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, element));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_free(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_res_simple_bitmap_free_several(unit, module_id, res_tag_bitmap, 1, element));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_clear(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_clear(unit, module_id, res_tag_bitmap));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_nof_free_elements_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int *nof_free_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_free_elements, _SHR_E_PARAM, "nof_free_elements");

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_nof_free_elements_get
                    (unit, module_id, res_tag_bitmap, nof_free_elements));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_nof_allocated_elements_in_range_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_allocated_elements, _SHR_E_PARAM, "nof_elements");

    SHR_IF_ERR_EXIT(sw_state_resource_nof_allocated_elements_in_range_get
                    (unit, module_id, res_tag_bitmap, range_start, nof_elements_in_range, nof_allocated_elements));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_tag_set(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    simple_bitmap_wrap_tag_info_t * algorithm_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(algorithm_info, _SHR_E_PARAM, "algorithm_info");

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_set(unit, module_id, res_tag_bitmap,
                                                         algorithm_info->tag, algorithm_info->force_tag,
                                                         algorithm_info->element, algorithm_info->nof_elements));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_simple_bitmap_tag_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    simple_bitmap_wrap_tag_info_t * algorithm_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(algorithm_info, _SHR_E_PARAM, "algorithm_info");

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap,
                                                         algorithm_info->element, &algorithm_info->tag));

exit:
    SHR_FUNC_EXIT;
}
