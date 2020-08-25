/** \file sw_state_resource_tag_bitmap.c
 *
 * Indexed resource management -- simple bitmap
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
typedef int make_iso_compilers_happy_sw_state_resource_rag_bitmap;

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <shared/bitop.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/types/sw_state_resource_tag_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>

#ifdef BCM_DNX_SUPPORT

/*
 *  This is a fairly brute-force implementation of bitmap, with minimal
 *  optimisations or improvements.  It could probably be enhanced somewhat by
 *  some subtleties, such as checking whether a SHR_BITDCL is all ones before
 *  scanning individual bits when looking for free space.
 */

#define SW_STATE_RESOURCE_TAG_BITMAP_TAG_SIZE_MAX   20

/* 
 * Since we use sparse_alloc and sparse_free as internal implementation of all allocation and free 
 *  operations, we use this as pattern and set the repeats = count.
 */
#define SW_STATE_RESOURCE_TAG_BITMAP_PATTERN_NOT_SPARSE 0x1

/*
 *  If the max_tag_value is over SW_STATE_RESOURCE_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE, then only allocate 1 member for the next_alloc and last_free arrays.
 */
#define SW_STATE_RESOURCE_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE 10000

/*
 * Pass this as force_tag if you don't wish to change the current force setting.
 */
#define SW_STATE_RESOURCE_TAG_BITMAP_FORCE_TAG_NO_CHANGE    2

/*
 *  Sw state access macro.
 *   Since this module is used for both DNX (JR2) and DPP (JR+ and below), we need to seperate
 *     it by compilation.
 */
#define RES_BMP_ACCESS res_tag_bmp_info_db

#define _SW_STATE_RESOURCE_TAG_BITMAP_DATA_SET_ALL (1)

#define _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(flags, error)                \
    do                                                                                  \
    {                                                                                   \
        if(_SHR_IS_FLAG_SET(flags, SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY))      \
        {                                                                               \
            SHR_SET_CURRENT_ERR(_SHR_E_RESOURCE);                                       \
            SHR_EXIT();                                                                 \
        }                                                                               \
    }                                                                                   \
    while(0)

shr_error_e
sw_state_resource_tag_bitmap_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    sw_state_resource_tag_bitmap_create_info_t create_info,
    uint32 count,
    uint32 alloc_flags)
{
    uint32 tag_size;
    uint32 nof_grains;
    int use_tag;
    uint32 nof_next_pointers, current_next_pointer;
    uint32 _count;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments 
     */
    if (SOC_WARM_BOOT(unit))
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED, "Can't create bitmap during init\n");
    }

    /*
     * Replace the count if the value is specified in the xml module file.
     */
    if (count != 0)
    {
        _count = count;
    }
    else
    {
        _count = create_info.count;
    }

    if (0 >= _count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must have a positive number of elements\n");
    }
    if (0 >= create_info.grain_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must have at least one element per grain\n");
    }
    if (_count % create_info.grain_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Count %d is not a multiple of grain size %d\n", _count,
                     create_info.grain_size);
    }

    if (create_info.max_tag_value > SW_STATE_RESOURCE_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE)
    {
        create_info.flags = create_info.flags | SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG;
    }

    DNX_SW_STATE_ALLOC(unit, module_id, *res_tag_bitmap, **res_tag_bitmap, /* number of elements */ 1, 0,
                       "sw_state res_tag_bitmap");

    DNX_SW_STATE_ALLOC_BITMAP(unit, module_id, (*res_tag_bitmap)->data, _count, 0,
                              "(*res_tag_bitmap)->data");

    use_tag = create_info.max_tag_value > 0;

    nof_grains = (use_tag) ? _count / create_info.grain_size : 0;
    tag_size = (use_tag) ? utilex_msb_bit_on(create_info.max_tag_value) + 1 : 0;

    if (use_tag)
    {
        DNX_SW_STATE_ALLOC_BITMAP(unit, module_id, (*res_tag_bitmap)->tagData,
                                  (tag_size) * (nof_grains), 0, "(*res_tag_bitmap)->data");

        if (create_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
        {
            DNX_SW_STATE_ALLOC(unit, module_id, (*res_tag_bitmap)->nof_allocated_elements_per_grain, int,
                                   nof_grains, 0, "(*res_tag_bitmap)->nof_allocated_elements_per_grain");
        }

        if (_SHR_IS_FLAG_SET(create_info.flags, SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG))
        {
            DNX_SW_STATE_ALLOC_BITMAP(unit, module_id, (*res_tag_bitmap)->tag_tracking_bitmap, _count, alloc_flags,
                                      "(*res_tag_bitmap)->tag_tracking_bitmap");
        }

        if (_SHR_IS_FLAG_SET(create_info.flags, SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
        {
            DNX_SW_STATE_ALLOC_BITMAP(unit, module_id, (*res_tag_bitmap)->forced_tag_indication, nof_grains, 0,
                                      "(*res_tag_bitmap)->forced_tag_indication");
        }

    }

    /*
     * Allocate the nextAlloc and lastFree pointers.
     */
    if (use_tag && !_SHR_IS_FLAG_SET(create_info.flags, SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG))
    {
        nof_next_pointers = create_info.max_tag_value + 1;
    }
    else
    {
        nof_next_pointers = 1;
    }

    DNX_SW_STATE_ALLOC(unit, module_id, (*res_tag_bitmap)->nextAlloc, int,
                       nof_next_pointers,
                       0,
                       "(*res_tag_bitmap)->nextAlloc");
    DNX_SW_STATE_ALLOC(unit, module_id, (*res_tag_bitmap)->lastFree, int,
                       nof_next_pointers,
                       0,
                       "(*res_tag_bitmap)->lastFree");

    /*
     * Set the lastFree value to count so it won't be used if nothing was actually freed.
     */
    for (current_next_pointer = 0 ; current_next_pointer < nof_next_pointers ; current_next_pointer++)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &(*res_tag_bitmap)->lastFree[current_next_pointer],
            &_count,
            sizeof(create_info.count),
            DNXC_SW_STATE_NO_FLAGS,
            "(*res_tag_bitmap)->lastFree");
    }



    /*
     * Set the bitmap "static" data.
     */
    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &(*res_tag_bitmap)->count,
            &_count,
            sizeof(create_info.count),
            DNXC_SW_STATE_NO_FLAGS,
            "(*res_tag_bitmap)->count");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &(*res_tag_bitmap)->low,
            &create_info.low_id,
            sizeof(create_info.low_id),
            DNXC_SW_STATE_NO_FLAGS,
            "(*hash_table)->low");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &(*res_tag_bitmap)->grainSize,
            &create_info.grain_size,
            sizeof(create_info.grain_size),
            DNXC_SW_STATE_NO_FLAGS,
            "(*hash_table)->grainSize");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &(*res_tag_bitmap)->tagSize,
            &tag_size,
            sizeof(tag_size),
            DNXC_SW_STATE_NO_FLAGS,
            "(*hash_table)->tagSize");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &(*res_tag_bitmap)->max_tag_value,
            &create_info.max_tag_value,
            sizeof(create_info.max_tag_value),
            DNXC_SW_STATE_NO_FLAGS,
            "(*hash_table)->max_tag_value");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &(*res_tag_bitmap)->flags,
            &create_info.flags,
            sizeof(create_info.flags),
            DNXC_SW_STATE_NO_FLAGS,
            "(*hash_table)->flags");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_destroy(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free the sw state memory. Should be used only at detach.
     */

    DNX_SW_STATE_FREE(unit, module_id, (*res_tag_bitmap)->data, "(*res_tag_bitmap)->data");     /* The bitmap data */
    DNX_SW_STATE_FREE(unit, module_id, (*res_tag_bitmap)->tagData, "(*res_tag_bitmap)->tagData");       /* The bitmap
                                                                                                         * tag */
    DNX_SW_STATE_FREE(unit, module_id, (*res_tag_bitmap), "(*res_tag_bitmap)"); /* The bitmap itself */

exit:
    SHR_FUNC_EXIT;
}

/*
 * Compare tags by element. 
 */
shr_error_e
sw_state_resource_tag_bitmap_compare_tags(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int tag_index,
    uint32 tag,
    uint8 *equal)
{
    SHR_BITDCL tmp;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Start by assuming that the tags are equal. If one element is different, change it. 
     */
    *equal = TRUE;
    tmp = 0;

    /*
     * Compare the tag with the tagData.
     */
    DNX_SW_STATE_BIT_RANGE_READ(unit, module_id, 0, res_tag_bitmap->tagData, (tag_index * (res_tag_bitmap->tagSize)), 0,
                                res_tag_bitmap->tagSize, &tmp);
    if (tag != tmp)
    {
        *equal = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Check that all elements that are required for the current allocation either have the same tag
 *    as the grain they're in, or are in a block that has no allocated elements.
 *  
 *  If all the elements meet these conditions, then result will be 0. Otherwise, it will be -1.
 *  
 *  If the tag size is zero, nothing to do here.
 */
static shr_error_e
_sw_state_resource_tag_bitmap_tag_check(
    int unit,
    uint32 flags,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int elemIndex,
    int elemCount,
    uint32 tag,
    int *result)
{
    int grain_size, tag_size;
    uint32 create_flags;
    uint8 always_check = _SHR_IS_FLAG_SET(flags, SW_STATE_RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG);
    SHR_FUNC_INIT_VARS(unit);

    grain_size = res_tag_bitmap->grainSize;
    tag_size = res_tag_bitmap->tagSize;
    create_flags = res_tag_bitmap->flags;

    *result = 0;

    /*
     * Check if this bitmap is even using tags.
     */
    if (tag_size > 0)
    {
        int current_elem;
        uint8 grain_tag_valid, equal;
        int grain_start, tag_index;

        /*
         * Iterate over all the tags that are attached to the element block, and verify that they either have 
         *   the same tag, or don't have a tag. 
         */
        for (current_elem = elemIndex; current_elem < elemIndex + elemCount; current_elem += grain_size)
        {
            grain_tag_valid = 0;
            tag_index = current_elem / grain_size;

            if (!always_check)
            {
                /*
                 * If we don't force check for the tag, then we check if there are any allocated elements in the 
                 * current grain. If there are, it means the tag is set for this range. 
                 */
                grain_start = (current_elem / grain_size) * grain_size;
                if (_SHR_IS_FLAG_SET(create_flags, SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
                {
                    DNX_SW_STATE_BIT_GET(unit, module_id, 0, res_tag_bitmap->forced_tag_indication, tag_index, &grain_tag_valid);
                }

                if (!grain_tag_valid)
                {
                    /*
                     * Tag is not forced, then check if it's already allocated.
                     */
                    if (_SHR_IS_FLAG_SET(create_flags, SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG))
                    {
                        DNX_SW_STATE_BIT_RANGE_TEST(unit, module_id, 0, res_tag_bitmap->tag_tracking_bitmap, grain_start, grain_size,
                                                    &grain_tag_valid);
                    }
                    else
                    {
                        DNX_SW_STATE_BIT_RANGE_TEST(unit, module_id, 0, res_tag_bitmap->data, grain_start, grain_size,
                                                    &grain_tag_valid);
                    }
                }
            }

            if (grain_tag_valid || always_check)
            {
                /*
                 * the grain is used by at least one other block 
                 */
                SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_compare_tags
                                (unit, module_id, res_tag_bitmap, tag_index, tag, &equal));

                /*
                 * If the tags are not equal, mark it, and break.
                 */
                if (!equal)
                {
                    *result = -1;
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Scan a range as per 'check all'.  Basically this needs to check whether a
 *  range is: all free, all in-use (with the same tag), all in-use (but
 *  different tags), or a mix of free and in-use (same or different tags).
 */
static int
_sw_state_resource_tag_bitmap_check_all_tag(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int count,
    int index)
{
    int tagbase;
    int offset;
    int freed = 0;
    int inuse = 0;
    int result = _SHR_E_NONE;
    uint8 bit_used = 0, equal = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (tag > res_tag_bitmap->max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag,
                     res_tag_bitmap->max_tag_value);
    }

    /*
     * scan the block 
     */
    for (offset = 0; offset < count; offset++)
    {
        DNX_SW_STATE_BIT_GET(unit, module_id, 0, res_tag_bitmap->data, index + offset, &bit_used);
        if (bit_used)
        {
            inuse++;
        }
        else
        {
            freed++;
        }
    }   /* for (offset = 0; offset < count; offset++) */
    if (inuse == count)
    {
        /*
         * block is entirely in use 
         */
        result = _SHR_E_FULL;
        /*
         * now verify that all involved grains have same tag 
         */
        if (0 < res_tag_bitmap->tagSize)
        {
            tagbase = index / res_tag_bitmap->grainSize;
            for (offset = 0; offset < count; offset += res_tag_bitmap->grainSize, tagbase++)
            {
                SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_compare_tags
                                (unit, module_id, res_tag_bitmap, tagbase, tag, &equal));
                if (!equal)
                {
                    /*
                     * tags are not equal, so different blocks 
                     */
                    result = _SHR_E_CONFIG;
                }
            }   /* for (all grains in the block) */
        }       /* if (0 < res_tag_bitmap->tagSize) */
    }
    else if (freed == count)
    {
        /*
         * block is entirely free 
         */
        result = _SHR_E_EMPTY;
    }
    else
    {
        /*
         * block is partially free and partially in use 
         */
        result = _SHR_E_EXISTS;
    }

    /*
     * Not actually error, just return value.
     */
    SHR_SET_CURRENT_ERR(result);
exit:
    SHR_FUNC_EXIT;
}

/*
 *  Set the tag for the given number of elements. The elemIndex here is
 *  offset from the start of the bitmap.
 */
shr_error_e
sw_state_resource_tag_bitmap_tag_set_internal(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int force_tag,
    int elemIndex,
    int elemCount)
{
    int index;
    int count;
    int offset;
    int force_tag_allowed;
    SHR_BITDCL tmp[1];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * 'tag' is asymmetrical after free, which results in comparison journal issue in 'alloc -> free' scenarios.
     */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    tmp[0] = tag;

    if (tag > res_tag_bitmap->max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag,
                     res_tag_bitmap->max_tag_value);
    }

    force_tag_allowed = _SHR_IS_FLAG_SET(res_tag_bitmap->flags, SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG)
            && force_tag != SW_STATE_RESOURCE_TAG_BITMAP_FORCE_TAG_NO_CHANGE;

    if (force_tag && (force_tag != SW_STATE_RESOURCE_TAG_BITMAP_FORCE_TAG_NO_CHANGE) && !force_tag_allowed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "force_tag can't be set if the ALLOW_FORCING_TAG flag wasn't set on init.");
    }

    if (res_tag_bitmap->tagSize)
    {
        index = elemIndex / res_tag_bitmap->grainSize;
        count = (elemCount + res_tag_bitmap->grainSize - 1) / res_tag_bitmap->grainSize;
        for (offset = 0; offset < count; offset++)
        {

            DNX_SW_STATE_BIT_RANGE_WRITE(unit, module_id, 0, res_tag_bitmap->tagData,
                                         ((index + offset) * res_tag_bitmap->tagSize), 0, res_tag_bitmap->tagSize,
                                         tmp);

            if (force_tag_allowed)
            {
                if (force_tag)
                {
                    DNX_SW_STATE_BIT_SET(unit, module_id, 0, res_tag_bitmap->forced_tag_indication, index + offset);
                }
                else
                {
                    DNX_SW_STATE_BIT_CLEAR(unit, module_id, 0, res_tag_bitmap->forced_tag_indication, index + offset);
                }
            }
        }
    }

exit:
    dnx_state_comparison_suppress(unit, FALSE);
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_resource_tag_bitmap_tag_set(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int force_tag,
    int element,
    int elemCount)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_set_internal(unit, module_id, res_tag_bitmap, tag,
            force_tag, element - res_tag_bitmap->low, elemCount));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_resource_tag_bitmap_tag_get_internal(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int elemIndex,
    uint32 *tag)
{
    SHR_BITDCL tmp;
    SHR_FUNC_INIT_VARS(unit);

    tmp = 0;
    *tag = 0;
    if (res_tag_bitmap->tagSize)
    {
        DNX_SW_STATE_BIT_RANGE_READ(unit, module_id, 0, res_tag_bitmap->tagData,
                                    ((elemIndex / res_tag_bitmap->grainSize) * res_tag_bitmap->tagSize), 0,
                                    res_tag_bitmap->tagSize, &tmp);
        *tag = tmp;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_tag_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint32 *tag)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_get_internal(unit, module_id, res_tag_bitmap,
        element - res_tag_bitmap->low, tag));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_resource_tag_bitmap_check_all_tag(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int count,
    int elem)
{
    int index;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments 
     */
    /*
     * check arguments 
     */
    if (elem < res_tag_bitmap->low)
    {
        /*
         * not valid ID 
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Starting element is too small: %d. Must be at least %d.", elem,
                     res_tag_bitmap->low);
    }
    if (0 >= count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Count must be >0. Was: %d", count);
    }
    index = elem - res_tag_bitmap->low;
    if (index + count > res_tag_bitmap->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Element + count is too high: %d. Must be at most %d.", elem + count,
                     res_tag_bitmap->low + res_tag_bitmap->count);
    }
    if (tag > res_tag_bitmap->max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag,
                     res_tag_bitmap->max_tag_value);
    }

    SHR_SET_CURRENT_ERR(_sw_state_resource_tag_bitmap_check_all_tag
                        (unit, module_id, res_tag_bitmap, tag, count, index));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_check(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int count,
    int elem)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = sw_state_resource_tag_bitmap_check_all_tag(unit, module_id, res_tag_bitmap, 0, count, elem);

    /*
     * Map the check_all_tag return value to _check return value.
     */
    if (rv == _SHR_E_FULL || rv == _SHR_E_CONFIG)
    {
        rv = _SHR_E_EXISTS;
    }
    else if (rv == _SHR_E_EMPTY)
    {
        rv = _SHR_E_NOT_FOUND;
    }
    else if (rv != _SHR_E_EXISTS)
    {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_SET_CURRENT_ERR(rv);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_check_all(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int count,
    int elem)
{
    uint32 first_tag;
    int index;
    SHR_FUNC_INIT_VARS(unit);

    if (elem < res_tag_bitmap->low)
    {
        /*
         * not valid ID 
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Starting element is too small: %d. Must be at least %d.", elem,
                     res_tag_bitmap->low);
    }
    if (0 >= count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Count must be >0. Was: %d", count);
    }
    index = elem - res_tag_bitmap->low;
    if (index + count > res_tag_bitmap->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Element + count is too high: %d. Must be at most %d.", elem + count,
                     res_tag_bitmap->low + res_tag_bitmap->count);
    }
    /*
     * here we assume the tag from the first grain in the range. Copy it to the buffer. 
     */
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_get_internal(unit, module_id, res_tag_bitmap, index, &first_tag));

    SHR_SET_CURRENT_ERR(_sw_state_resource_tag_bitmap_check_all_tag
                        (unit, module_id, res_tag_bitmap, first_tag, count, index));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_nof_free_elements_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int *nof_free_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments 
     */

    SHR_NULL_CHECK(nof_free_elements, _SHR_E_PARAM, "nof_free_elements");

    /*
     * The number of free elements is the number of existing elements (count) -
     *    the number of used elements.
     */
    *nof_free_elements = res_tag_bitmap->count - res_tag_bitmap->used;

exit:
    SHR_FUNC_EXIT;
}

#if 0
/* 
 * Right now we don't need this dump function, but keep it around anyway. 
 */
int
sw_state_resource_tag_bitmap_dump(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    )
{
    int result;
    int error = FALSE;
    int elemsUsed;
    int grainsFree;
    int grainsFull;
    int grainUse;
    int index;
    int offset;
    int elemOffset;
    int tagOffset;
    int tagStart;
    uint32 bmp_index;
    _sw_state_resource_tag_bitmap_list_t bmp_data;
    uint8 temp;
    uint8 *tagData = NULL;
    int nofGrains;

    if (!handle)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("must provide non-NULL handle\n")));
        return _SHR_E_PARAM;
    }

    /*
     * Copy the bitmap's data. 
     */
    SW_STATE_RESOURCE_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _sw_state_resource_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /*
     * Allocate memory to copy the bitmap's tag. 
     */
    nofGrains = bmp_data.count / bmp_data.grainSize;
    tagData = (uint8 *) sal_alloc(nofGrains * bmp_data.tagSize, "Tag data duplication");

    /*
     * Copy the bitmap's tag. 
     */
    for (tagOffset = 0; tagOffset < nofGrains * bmp_data.tagSize; tagOffset++)
    {
        result = RES_BMP_ACCESS.bitmap_lists.tagData.bit_get(unit, bmp_index, tagOffset, &tagData[tagOffset]);
        if (result)
        {
            goto error;
        }
    }

    LOG_CLI((BSL_META("Data for %sbitmap #%d:\n"), (bmp_data.tagSize) ? "tagged " : "", handle));
    LOG_CLI((BSL_META("  lowest ID     = %08X\n"), bmp_data.low));
    LOG_CLI((BSL_META("  element count = %08X\n"), bmp_data.count));
    LOG_CLI((BSL_META("  used elements = %08X\n"), bmp_data.used));
    LOG_CLI((BSL_META("  grain size    = %08X elements\n"), bmp_data.grainSize));
    LOG_CLI((BSL_META("  tag size      = %08X bytes\n"), bmp_data.tagSize));
    LOG_CLI((BSL_META("  last free     = %08X %s\n"),
             bmp_data.lastFree, (error |= (bmp_data.lastFree > bmp_data.count)) ? "INVALID" : ""));
    LOG_CLI((BSL_META("  next alloc    = %08X %s\n"),
             bmp_data.nextAlloc, (error |= (bmp_data.nextAlloc > bmp_data.count)) ? "INVALID" : ""));
    LOG_CLI((BSL_META("  element map:\n")));
    LOG_CLI((BSL_META("    1st Elem (index)    State of elements (1 = used)        Tag bytes\n")));
    LOG_CLI((BSL_META("    -------- --------   --------------------------------    ----------------\n")));
    elemsUsed = 0;
    grainsFree = 0;
    grainsFull = 0;
    for (index = 0; index < bmp_data.count; index += bmp_data.grainSize)
    {
        LOG_CLI((BSL_META("    %08X %08X   "), index + bmp_data.low, index));
        elemOffset = 0;
        tagOffset = 0;
        grainUse = 0;
        while ((elemOffset < bmp_data.grainSize) && (tagOffset < bmp_data.tagSize))
        {
            for (offset = 0; offset < 32; offset++)
            {
                if (elemOffset < bmp_data.grainSize)
                {
                    result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index + elemOffset, &temp);
                    if (result)
                    {
                        /*
                         * Something went wrong in sw state. Break the loop. 
                         */
                        goto error;
                    }
                    if (temp)
                    {
                        LOG_CLI((BSL_META("1")));
                        grainUse++;
                    }
                    else
                    {
                        LOG_CLI((BSL_META("0")));
                    }
                }
                else
                {
                    LOG_CLI((BSL_META(" ")));
                }
                elemOffset++;
            }   /* for (offset = 0; offset < 32; offset++) */
            LOG_CLI((BSL_META("    ")));
            tagStart = (index / bmp_data.grainSize) * bmp_data.tagSize;
            for (offset = 0; offset < 8; offset++)
            {
                if (tagOffset < bmp_data.tagSize)
                {
                    LOG_CLI((BSL_META("%02X"), tagData[tagStart + tagOffset]));
                }
                tagOffset++;
            }   /* for (offset = 0; offset < 8; offset++) */
            if ((elemOffset < bmp_data.grainSize) || (tagOffset < bmp_data.tagSize))
            {
                LOG_CLI((BSL_META("\n                         ")));
            }
            else
            {
                LOG_CLI((BSL_META("\n")));
            }
        }       /* while (either grain member state or tag remains to show) */
        elemsUsed += grainUse;
        if (0 == grainUse)
        {
            grainsFree++;
        }
        else if (grainUse == bmp_data.grainSize)
        {
            grainsFull++;
        }
    }   /* for all grains */
    LOG_CLI((BSL_META("  empty grains  = %08X\n"), grainsFree));
    LOG_CLI((BSL_META("  full grains   = %08X\n"), grainsFull));
    LOG_CLI((BSL_META("  counted elems = %08X %s\n"),
             elemsUsed, (error |= (elemsUsed != bmp_data.used)) ? "INVALID" : ""));
error:
    if (tagData)
    {
        sal_free(tagData);
    }
    if (error)
    {
        LOG_CLI((BSL_META("tagged bitmap %d appears to be corrupt\n"), handle));
        result = _SHR_E_INTERNAL;
    }
    else
    {
        result = _SHR_E_NONE;
    }
    return result;
}
#endif

static shr_error_e
sw_state_resource_tag_bitmap_nof_allocated_elements_per_bank_update(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int repeats,
    int element,
    int grainSize,
    uint8 operation)
{
    uint32 grain_index = 0;
    int tmp_repeats = 0;
    int grain_element_index = 0;
    uint32 tmp_elements_to_update = 0;
    SHR_FUNC_INIT_VARS(unit);

    grain_index = element / grainSize;
    tmp_repeats = repeats;
    grain_element_index = element;

    while (tmp_repeats > 0)
    {
        /*
         * Check if the temporal repeats are from multiple grains.
         */
        if ((tmp_repeats + grain_element_index) > (grainSize * (grain_index + 1)))
        {
            /*
             * Increase/Decrease the number of allocated elemenst acording to operation.
             * operation is TRUE if we are adding elements and FALSE if we are subtracting.
             */
            if (operation)
            {
                tmp_elements_to_update = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index]
                                         + (((grain_index * grainSize) + grainSize) - grain_element_index);
            }
            else
            {
                tmp_elements_to_update = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index]
                                                         - (((grain_index * grainSize) + grainSize) - grain_element_index);
            }
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &res_tag_bitmap->nof_allocated_elements_per_grain[grain_index],
                    &tmp_elements_to_update,
                    sizeof(tmp_elements_to_update),
                    DNXC_SW_STATE_NO_FLAGS,
                    "hash_table->nof_allocated_elements_per_grain");

            tmp_repeats -= (((grain_index * grainSize) + grainSize) - grain_element_index);
        }
        else
        {
            /*
             * Increase/Decrease the number of allocated elemenst acording to operation.
             * operation is TRUE if we are adding elements and FALSE if we are subtracting.
             */
            if (operation)
            {
                tmp_elements_to_update = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index] + tmp_repeats;
            }
            else
            {
                tmp_elements_to_update = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index] - tmp_repeats;
            }
            /*
             * Since the repeats are only in the current grain set tmp_repeats to 0, to break the loop.
             */
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &res_tag_bitmap->nof_allocated_elements_per_grain[grain_index],
                    &tmp_elements_to_update,
                    sizeof(tmp_elements_to_update),
                    DNXC_SW_STATE_NO_FLAGS,
                    "hash_table->nof_allocated_elements_per_grain");

            tmp_repeats = 0;
        }

        grain_index++;
        grain_element_index = grain_index * grainSize;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_sw_state_resource_tag_bitmap_restore_last_free_and_next_alloc(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Supress journaling comparison - this data is not supposed to be restored after add-delete. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    DNX_SW_STATE_MEMCPY_BASIC(unit,
        module_id,
        &res_tag_bitmap->lastFree[tag],
        &(res_tag_bitmap->count),
        sizeof(res_tag_bitmap->count),
        DNXC_SW_STATE_NO_FLAGS,
        "res_tag_bitmap->lastFree");

    DNX_SW_STATE_MEMSET(unit,
        module_id,
        &res_tag_bitmap->nextAlloc[tag],
        0,
        0,
        sizeof(int),
        DNXC_SW_STATE_NO_FLAGS,
        "res_tag_bitmap->nextAlloc");

    /** Continue journaling comparison. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_free(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    sw_state_resource_tag_bitmap_alloc_info_t free_info,
    int elem)
{
    int offset;
    int final_bit_in_pattern;
    uint32 pattern_mask;
    uint8 temp;
    uint8 free_element;
    int current_element, first_index_in_pattern;
    int repeat_index;
    int use_sparse;
    uint32 currentTag;
    uint8 empty_grain = FALSE;
    uint32 nof_next_last_pointers = 0;
    uint32 current_next_last_pointer = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Translate the relevant argument if the ALLOC_SPARSE is not set.
     */
    if (!(free_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_SPARSE))
    {
        free_info.pattern = SW_STATE_RESOURCE_TAG_BITMAP_PATTERN_NOT_SPARSE;
        free_info.length = 1;
        free_info.repeats = free_info.count;
    }

    /*
     * check arguments
     */
    if (elem < res_tag_bitmap->low)
    {
        /*
         * not valid ID 
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Starting element is too small: %d. Must be at least %d.", elem,
                     res_tag_bitmap->low);
    }
    if (0 >= free_info.length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must free a positive number of elements. Length was %d", free_info.length);
    }
    if (32 < free_info.length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must not be longer than 32. Was %d", free_info.length);
    }
    if (0 >= free_info.repeats)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must check at least one pattern. Repeats was %d\n", free_info.repeats);
    }
    pattern_mask = (((uint32) 1 << (free_info.length - 1)) - 1) | ((uint32) 1 << (free_info.length - 1));
    if (0 == (free_info.pattern & pattern_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must contain at least one element. Pattern was 0x%x and length was %d",
                     free_info.pattern, free_info.length);
    }
    if (free_info.pattern & ~pattern_mask)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must not contain unused bits. Pattern was 0x%x and length was %d",
                     free_info.pattern, free_info.length);
    }

    final_bit_in_pattern = (utilex_msb_bit_on(free_info.pattern) + 1) + (free_info.length * (free_info.repeats - 1));
    first_index_in_pattern = elem - res_tag_bitmap->low - utilex_lsb_bit_on(free_info.pattern);

    if (first_index_in_pattern + final_bit_in_pattern > res_tag_bitmap->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given input will exceed the maximum elemnt in the resource. "
                     "Element was %d, pattern was 0x%x and repeats was %d", elem, free_info.pattern, free_info.repeats);
    }


    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_get_internal
                    (unit, module_id, res_tag_bitmap, first_index_in_pattern, &currentTag));


    /*
     * If the pattern is simple one, we can save run time but not going bit by bit, but by going en mass.
     */
    use_sparse = free_info.pattern != SW_STATE_RESOURCE_TAG_BITMAP_PATTERN_NOT_SPARSE;

    for (free_element = 0; free_element < 2; free_element++)
    {
        current_element = first_index_in_pattern;
        for (repeat_index = 0; repeat_index < free_info.repeats; repeat_index++)
        {
            for (offset = 0; offset < free_info.length; offset++, current_element++)
            {
                if (free_info.pattern & (1 << offset))
                {
                    DNX_SW_STATE_BIT_GET(unit, module_id, 0, res_tag_bitmap->data, current_element, &temp);
                    if (temp)
                    {
                        if (free_element)
                        {
                            if (use_sparse)
                            {
                                DNX_SW_STATE_BIT_CLEAR(unit, module_id, 0, res_tag_bitmap->data, current_element);
                                if (res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                                {
                                    DNX_SW_STATE_BIT_CLEAR(unit, module_id, 0, res_tag_bitmap->tag_tracking_bitmap,
                                                           current_element);
                                }
                                DNX_SW_STATE_COUNTER_DEC(unit,
                                        module_id,
                                        res_tag_bitmap->used,
                                        1,
                                        res_tag_bitmap->used,
                                        DNXC_SW_STATE_NO_FLAGS,
                                        "res_tag_bitmap->used");

                                /*
                                 * Decrement the number of allocated elements per grain.
                                 */
                                if (res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
                                {
                                    DNX_SW_STATE_COUNTER_DEC(unit,
                                            module_id,
                                            res_tag_bitmap->nof_allocated_elements_per_grain[current_element/res_tag_bitmap->grainSize],
                                            1,
                                            res_tag_bitmap->nof_allocated_elements_per_grain[current_element/res_tag_bitmap->grainSize],
                                            DNXC_SW_STATE_NO_FLAGS,
                                            "res_tag_bitmap->nof_allocated_elements_per_grain");

                                    if (!(res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG)
                                        && (res_tag_bitmap->nof_allocated_elements_per_grain[currentTag] == 0))
                                    {
                                        empty_grain = TRUE;
                                        SHR_IF_ERR_EXIT(_sw_state_resource_tag_bitmap_restore_last_free_and_next_alloc(unit,
                                                         module_id, res_tag_bitmap, currentTag));
                                    }

                                }
                            }
                            else
                            {
                                /*
                                 * We can just free the entire range when we get to the first bit, then break the loop.
                                 *   In this case, repeats indicates how many elements to deallocate. 
                                 */
                                DNX_SW_STATE_BIT_RANGE_CLEAR(unit, module_id, 0, res_tag_bitmap->data, current_element,
                                                             free_info.repeats);

                                if (res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                                {
                                    DNX_SW_STATE_BIT_RANGE_CLEAR(unit, module_id, 0,
                                                                 res_tag_bitmap->tag_tracking_bitmap, current_element,
                                                                 free_info.repeats);
                                }
                                DNX_SW_STATE_COUNTER_DEC(unit,
                                                module_id,
                                                res_tag_bitmap->used,
                                                free_info.repeats,
                                                res_tag_bitmap->used,
                                                DNXC_SW_STATE_NO_FLAGS,
                                                "res_tag_bitmap->used");

                                /*
                                 * Update the number of allocated elements in the relevant grain. The operation is set to FALSE because we are freeing elements.
                                 */
                                if (res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
                                {
                                    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_nof_allocated_elements_per_bank_update
                                                    (unit, module_id, res_tag_bitmap, free_info.repeats,
                                                     current_element, res_tag_bitmap->grainSize, FALSE));

                                    if (!(res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG)
                                        && (res_tag_bitmap->nof_allocated_elements_per_grain[currentTag] == 0))
                                    {
                                        empty_grain = TRUE;
                                        SHR_IF_ERR_EXIT(_sw_state_resource_tag_bitmap_restore_last_free_and_next_alloc(unit,
                                                         module_id, res_tag_bitmap, currentTag));
                                    }
                                }

                                /*
                                 * Set loop variables to maximum to break the loop.
                                 */
                                offset = free_info.length;
                                repeat_index = free_info.repeats;
                            }
                        }
                    }   /* if(_SHR_E_FULL == result) */
                    else
                    {
                        /*
                         * This bit was given as part of the pattern, but is not allocated. Break all loops and
                         * return error.
                         */

                        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Given element %d does not exist.", current_element);
                    }
                }       /* if (this element is in the pattern) */
            }   /* for (pattern length) */
        }       /* for (repeat_index = 0; repeat_index < repeats; repeat_index++) */
    }   /* for(free_element = 0; free_element < 2; free_element++) */

    /** Supress journaling comparison - this data is not supposed to be restored after add-delete. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    /*
     * Update the last free indication with the first bit that was freed in this pattern.
     */
    if (!(res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG)
            && (res_tag_bitmap->lastFree[currentTag] >= first_index_in_pattern) && !empty_grain)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                module_id,
                &res_tag_bitmap->lastFree[currentTag],
                &first_index_in_pattern,
                sizeof(first_index_in_pattern),
                DNXC_SW_STATE_NO_FLAGS,
                "res_tag_bitmap->lastFree[currentTag]");
    }

    /*
     * Reset nextAlloc and lastFree if the resource is empty.
     */
    if(res_tag_bitmap->used == 0)
    {
        empty_grain = TRUE;
        if (!(res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG))
        {
            nof_next_last_pointers = res_tag_bitmap->max_tag_value + 1;
        }
        else
        {
            nof_next_last_pointers = 1;
        }

        DNX_SW_STATE_MEMSET(unit,
                            module_id,
                            res_tag_bitmap->nextAlloc,
                            0,
                            0,
                            nof_next_last_pointers*sizeof(int),
                            DNXC_SW_STATE_NO_FLAGS,
                            "(*res_tag_bitmap)->nextAlloc");

        /*
         * Set the lastFree value to count so it won't be used if nothing was actually freed.
         */
        for (current_next_last_pointer = 0 ; current_next_last_pointer < nof_next_last_pointers ; current_next_last_pointer++)
        {
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                module_id,
                &res_tag_bitmap->lastFree[current_next_last_pointer],
                &res_tag_bitmap->count,
                sizeof(res_tag_bitmap->count),
                DNXC_SW_STATE_NO_FLAGS,
                "(*res_tag_bitmap)->lastFree");
        }
    }

    /** Continue journaling comparison. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_resource_check_element_for_allocation(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int pattern,
    int length,
    int repeats,
    int element,
    int *can_allocate)
{
    int repeat, current_index, bit_offset, use_sparse;
    uint8 temp;
    SHR_FUNC_INIT_VARS(unit);

    *can_allocate = TRUE;
    use_sparse = pattern != SW_STATE_RESOURCE_TAG_BITMAP_PATTERN_NOT_SPARSE;

    if (use_sparse)
    {
        for (repeat = 0, current_index = element; repeat < repeats; repeat++)
        {
            for (bit_offset = 0; bit_offset < length; bit_offset++, current_index++)
            {
                if (pattern & (1 << bit_offset))
                {
                    DNX_SW_STATE_BIT_GET(unit, module_id, 0, res_tag_bitmap->data, current_index, &temp);

                    if (temp)
                    {
                        /*
                         * One of the required bits is already taken. Can't allocate here.
                         */
                        *can_allocate = FALSE;
                        SHR_EXIT();
                    }   /* if (temp) */
                }       /* if(pattern & (1 << offset)) */
            }   /* for (bit_offset = 0; bit_offset < length; bit_offset++, current++) */
        }       /* for (repeat = 0, current=index; repeat < repeats; repeat++) */
    }
    else
    {
        DNX_SW_STATE_BIT_RANGE_TEST(unit, module_id, 0, res_tag_bitmap->data, element, repeats, &temp);
        if (temp)
        {
            /*
             * One of the required bits is already taken. Can't allocate here.
             */
            *can_allocate = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_resource_allocate_next_free_element_in_range(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    sw_state_resource_tag_bitmap_alloc_info_t alloc_info,
    int range_start,
    int range_end,
    int *elem,
    int *found)
{
    int index, last_legal_index, pattern_first_set_bit;
    int found_match, allocated_block_length;
    int tag_compare_result, skip_block;
    int grain_size, grain_count;
    uint8 temp;
    int current_offset_index;
    SHR_FUNC_INIT_VARS(unit);

    grain_size = res_tag_bitmap->grainSize;
    current_offset_index = 0;

    /*
     * Iterate between range_start and range_end, and check for suitable elemnts in the middle.
     */
    found_match = FALSE;
    index = range_start;
    allocated_block_length = (utilex_msb_bit_on(alloc_info.pattern) + 1) + (alloc_info.length * (alloc_info.repeats - 1));
    last_legal_index = range_end - allocated_block_length;
    pattern_first_set_bit = utilex_lsb_bit_on(alloc_info.pattern);

    while (index <= last_legal_index)
    {
        /*
         * First, skip grains until a grain with an appropriate tag is found.
         */
        do
        {
            skip_block = FALSE;
            SHR_IF_ERR_EXIT(_sw_state_resource_tag_bitmap_tag_check
                            (unit, alloc_info.flags, module_id, res_tag_bitmap, index, allocated_block_length, alloc_info.tag,
                             &tag_compare_result));

            if (!tag_compare_result || (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
            {
                /*
                 * Tags are matching, but skip grain if it's full.
                 */
                int first_element_in_grain = index - index % grain_size;
                DNX_SW_STATE_BIT_RANGE_COUNT(unit, module_id, 0, res_tag_bitmap->data, first_element_in_grain,
                                             grain_size, &grain_count);

                if (grain_count == grain_size)
                {
                    /*
                     * Grain is full, skip it.
                     */
                    skip_block = TRUE;
                }
            }
            else
            {
                /*
                 * Tag mismatch, skip block.
                 */
                skip_block = TRUE;
            }

            if (skip_block)
            {
                /*
                 * At least one element has a different tag.
                 */
                found_match = FALSE;
                /*
                 * Skip to beginning of next grain
                 */
                index += grain_size - index % grain_size;
                /*
                 * Realign after this grain.
                 */
                index = (((index + alloc_info.align - 1) / alloc_info.align) * alloc_info.align) + alloc_info.offs[0];
            }
            else
            {
                /*
                 * Found a matching grain, use it.
                 */
                break;
            }
        }
        while (skip_block && index <= last_legal_index);

        if (index > last_legal_index)
        {
            /*
             * We didn't find a matching tag in the required range.
             */
            break;
        }

        DNX_SW_STATE_BIT_GET(unit, module_id, 0, res_tag_bitmap->data, index + pattern_first_set_bit, &temp);

        /*
         * temp indicates whether the first required element in the pattern starting from index is taken. Keep searching until
         *  it's false, or until we finished scanning the range.
         * Remove the offset before the first iteration because we already added it while searching for the tag.
         */
        index -= alloc_info.offs[0];
        while (temp && (index <= last_legal_index))
        {
            index += alloc_info.align;
            for (current_offset_index = 0 ; current_offset_index < alloc_info.nof_offsets ; current_offset_index++)
            {
                DNX_SW_STATE_BIT_GET(unit, module_id, 0, res_tag_bitmap->data, index + pattern_first_set_bit + alloc_info.offs[current_offset_index], &temp);
                if (!temp)
                {
                    break;
                }
            }
        }

        /*
         * Add the offset that we used in the last iteration to find the correct element.
         */
        index += alloc_info.offs[current_offset_index];

        if (index > last_legal_index)
        {
            /*
             * We didn't find a matching element in range.
             */
            break;
        }

        /*
         * We found a candidate; see if block matches the required pattern.
         */
        SHR_IF_ERR_EXIT(sw_state_resource_check_element_for_allocation(unit, module_id, res_tag_bitmap, alloc_info.pattern,
                                                                       alloc_info.length, alloc_info.repeats, index, &found_match));

        if (found_match)
        {
            /*
             * Found a matching block with matching tag. We might have exceeded grain boundry, 
             * so check the tag again. 
             */
            SHR_IF_ERR_EXIT(_sw_state_resource_tag_bitmap_tag_check
                            (unit, alloc_info.flags, module_id, res_tag_bitmap, index, allocated_block_length, alloc_info.tag,
                             &tag_compare_result));

            if (tag_compare_result && !(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
            {
                /*
                 * Element tag doesn't match. Continue to next grain.
                 */
                found_match = FALSE;
            }
            else
            {
                /*
                 * Otherwise, we found a match. Break.
                 */
                break;
            }
        }

        if (!found_match)
        {
            /*
             * Either the entries are not free, or the tag doesn't match. Advance the index and continue to loop.
             */
            index -= alloc_info.offs[current_offset_index];
            index += alloc_info.align;
            index += alloc_info.offs[0];
            continue;
        }
    }

    *elem = index;
    *found = found_match;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_resource_tag_bitmap_alloc_verify(
    int unit,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    sw_state_resource_tag_bitmap_alloc_info_t alloc_info,
    int *elem)
{
    uint32 pattern_mask;
    int pattern_first_index;
    int allocated_block_length;
    int current_offset_index = 0;
    uint8 wrong_offset;
    SHR_FUNC_INIT_VARS(unit);

    /*
      * check arguments
      */
     SHR_NULL_CHECK(elem, _SHR_E_PARAM, "elem \n");

     if (0 >= alloc_info.length)
     {
         _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
         SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern length must be at least 1. \n");
     }

     if (32 < alloc_info.length)
     {
         _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
         SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern length must be at most 32. \n");
     }

     if (0 >= alloc_info.repeats)
     {
         _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
         SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern repeats must be at least 1. \n");
     }

     pattern_mask = (((uint32) 1 << (alloc_info.length - 1)) - 1) | ((uint32) 1 << (alloc_info.length - 1));
     if (0 == (alloc_info.pattern & pattern_mask))
     {
         _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
         SHR_ERR_EXIT(_SHR_E_PARAM, "pattern must contain at least one element. \n");
     }

     if (alloc_info.pattern & ~pattern_mask)
     {
         _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
         SHR_ERR_EXIT(_SHR_E_PARAM, "pattern must not contain unused bits. \n");
     }

     if (alloc_info.tag > res_tag_bitmap->max_tag_value)
     {
         _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
         SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d. \n", alloc_info.tag,
                      res_tag_bitmap->max_tag_value);
     }

     if (_SHR_IS_FLAG_SET(alloc_info.flags, SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
     {
         if (_SHR_IS_FLAG_SET(alloc_info.flags, SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
                 && *elem < alloc_info.range_start && *elem > alloc_info.range_end)
         {
             _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
             SHR_ERR_EXIT(_SHR_E_PARAM, "Given element WITH_ID that is not in the given range. Element is %d,"
                     "range start is %d and range end is %d.\n", *elem, alloc_info.range_start, alloc_info.range_end);
         }
         if (alloc_info.range_start < res_tag_bitmap->low)
         {
             _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
             SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Start ID for allocation in range %d cannot be smaller than the first element of the resource %d.\n",
                     alloc_info.range_start, res_tag_bitmap->low);
         }
         if (alloc_info.range_end > (alloc_info.range_start + res_tag_bitmap->count))
         {
             _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
             SHR_ERR_EXIT(_SHR_E_PARAM,
                     "End ID for allocation in range %d cannot be larger than the first invalid element of the resource %d.\n",
                     alloc_info.range_end, alloc_info.range_start + res_tag_bitmap->count);
         }
     }

     /*
      * Calculate the length of the allocated block. We can ignore trailing 0s in the pattern.
      */
     allocated_block_length =
         (utilex_msb_bit_on(alloc_info.pattern) + 1) + (alloc_info.length * (alloc_info.repeats - 1));

     if (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
     {
         int element_offset;
         /*
          * WITH_ID, so only try the specifically requested block.
          */
         if (*elem < res_tag_bitmap->low)
         {
             _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
             SHR_ERR_EXIT(_SHR_E_PARAM, "Given element %d is too low. Must be at least %d. \n", *elem,
                          res_tag_bitmap->low);
         }
         pattern_first_index = *elem - res_tag_bitmap->low - utilex_lsb_bit_on(alloc_info.pattern);
         if (pattern_first_index + allocated_block_length > res_tag_bitmap->count)
         {
             _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
             SHR_ERR_EXIT(_SHR_E_PARAM,
                          "Allocating %d elements starting from element %d will exceed the maximum element %d. \n",
                          allocated_block_length, *elem, res_tag_bitmap->low + res_tag_bitmap->count - 1);
         }

         if (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_SPARSE)
         {
             int bit_offset = 0;

             if (pattern_first_index < 0)
             {
                 _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
                 SHR_ERR_EXIT(_SHR_E_PARAM, "The pattern first index %d is lower than 0. \n", pattern_first_index);
             }

             bit_offset = *elem - pattern_first_index;

             if(!(alloc_info.pattern & (1<<bit_offset)))
             {
                 _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
                 SHR_ERR_EXIT(_SHR_E_PARAM,
                              "The pattern first index %d is not aligned with %d. \n",
                              pattern_first_index, alloc_info.align);
             }
         }

         /*
          * Check that the element given conforms with the given align and offset.
          */
         wrong_offset = FALSE;
         if (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO)
         {
             /*
              * Alignment is against zero. We should calculate the alignment of the input pattern first index, not the internal one.
              */
             element_offset = (pattern_first_index + res_tag_bitmap->low) % alloc_info.align;
         }
         else
         {
             /*
              * alignment is against low
              */
             element_offset = (pattern_first_index) % alloc_info.align;
         }

         if (alloc_info.nof_offsets == 0)
         {
             if (element_offset != 0)
             {
                 wrong_offset = TRUE;
             }
         }
         else
         {
             for(current_offset_index = 0; current_offset_index < alloc_info.nof_offsets; current_offset_index++)
             {
                 if (element_offset == alloc_info.offs[current_offset_index])
                     break;
             }
             if(current_offset_index == alloc_info.nof_offsets)
             {
                 wrong_offset = TRUE;
             }
         }

         if (wrong_offset)
         {
             _SW_STATE_RESOURCE_ERROR_EXIT_NO_MSH_IF_CHECK_ONLY(alloc_info.flags, _SHR_E_PARAM);
             SHR_ERR_EXIT(_SHR_E_PARAM, "provided first element %d does not conform"
                         " to provided align %d + offset values" " (actual offset = %d)\n", *elem, alloc_info.align, element_offset);
         }
     }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_alloc(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    sw_state_resource_tag_bitmap_alloc_info_t alloc_info,
    int *elem)
{
    int pattern_first_index;
    int allocated_block_length;
    int can_allocate;
    int tag_compare_result;
    int allocation_success = FALSE;
    int update_last_free = FALSE;
    int update_next_alloc = FALSE;
    int tmp_update_value = 0;
    int nextAlloc = 0, lastFree = 0;
    int current_offset_index = 0;
    int tag_for_pointers;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Translate the relevant argument if the ALLOC_SPARSE is not set.
     */
    if (!(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_SPARSE))
    {
        alloc_info.pattern = SW_STATE_RESOURCE_TAG_BITMAP_PATTERN_NOT_SPARSE;
        alloc_info.length = 1;
        alloc_info.repeats = alloc_info.count;
    }

    /*
     * Translate the relevant argument if the ALLOC_ALIGN and ALLOC_SPARSE is not set.
     */
    if (!(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN)
        && !(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_SPARSE))
    {
        alloc_info.align = 1;
    }

    if (res_tag_bitmap->count == res_tag_bitmap->used)
    {
        if (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_RESOURCE);
            SHR_EXIT();
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Can't allocate element because the resource is already full. \n");
        }
    }

    /*
     * If we allocate ignoring flags, or the bitmap is not optimized per tag, then
     * we need to read the next alloc and last free pointers from index 0, and not from the
     * given tag.
     */
    if (_SHR_IS_FLAG_SET(alloc_info.flags, SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)
            || _SHR_IS_FLAG_SET(res_tag_bitmap->flags, SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG))
    {
        tag_for_pointers = 0;
    }
    else
    {
        tag_for_pointers = alloc_info.tag;
    }

    /*
     * Minimum nof_offsets is 1.
     */
    if(alloc_info.nof_offsets < 1)
    {
        alloc_info.nof_offsets = 1;
    }

    if (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY)
    {
        SHR_SET_CURRENT_ERR(sw_state_resource_tag_bitmap_alloc_verify(unit, res_tag_bitmap, alloc_info, elem));
        if(SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
        {
            SHR_EXIT();
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_alloc_verify(unit, res_tag_bitmap, alloc_info, elem));
    }

    /*
     * Calculate the length of the allocated block. We can ignore trailing 0s in the pattern.
     */
    allocated_block_length =
        (utilex_msb_bit_on(alloc_info.pattern) + 1) + (alloc_info.length * (alloc_info.repeats - 1));

    if (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
    {
        /*
         * WITH_ID, so only try the specifically requested block.
         */
        pattern_first_index = *elem - res_tag_bitmap->low - utilex_lsb_bit_on(alloc_info.pattern);

        /*
         * check whether the block is free
         */
        SHR_IF_ERR_EXIT(sw_state_resource_check_element_for_allocation
                        (unit, module_id, res_tag_bitmap, alloc_info.pattern, alloc_info.length, alloc_info.repeats,
                         pattern_first_index, &can_allocate));

        if (!can_allocate)
        {
            /*
             * In use; can't do WITH_ID alloc of this block
             */
            if (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_RESOURCE);
                SHR_EXIT();
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Given element, or some of its block, already exists. Asked for %d elements "
                            "starting from index %d\n", allocated_block_length, *elem);
            }
        }
        if (!(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
        {
            SHR_IF_ERR_EXIT(_sw_state_resource_tag_bitmap_tag_check
                            (unit, alloc_info.flags, module_id, res_tag_bitmap, pattern_first_index,
                             allocated_block_length, alloc_info.tag, &tag_compare_result));
            if (tag_compare_result)
            {
                /*
                 * One of the elements has a mismatching tag.
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM, "Given element, or some of its block, have a mismatching tag."
                             "Asked for %d elements starting from index %d with tag %d \n",
                             allocated_block_length, *elem, alloc_info.tag);
            }
        }
        /*
         * If we got here then we successfully allocated. 
         * Don't adjust last free or next alloc for WITH_ID.
         */
        allocation_success = TRUE;
    }
    else
    {   /* if (flags & SHR_RES_ALLOC_WITH_ID) */

        if (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO)
        {
            /*
             * Adjust offset to represent alignment against zero, not start of pool.
             */
             for(current_offset_index = 0; current_offset_index < alloc_info.nof_offsets; current_offset_index++)
            {
                alloc_info.offs[current_offset_index] =
                    (alloc_info.offs[current_offset_index] + alloc_info.align - (res_tag_bitmap->low % alloc_info.align)) % alloc_info.align;
            }
        }

        /*
         * Try to allocate immediately after the last freed element.
         */
        lastFree = res_tag_bitmap->lastFree[tag_for_pointers];

        pattern_first_index = UTILEX_ALIGN_UP(lastFree, alloc_info.align) + alloc_info.offs[0];
        if (pattern_first_index + allocated_block_length < res_tag_bitmap->count)
        {
            /*
             * it might fit
             */
            SHR_IF_ERR_EXIT(sw_state_resource_check_element_for_allocation
                            (unit, module_id, res_tag_bitmap, alloc_info.pattern, alloc_info.length, alloc_info.repeats,
                             pattern_first_index, &can_allocate));

            if (can_allocate)
            {
                SHR_IF_ERR_EXIT(_sw_state_resource_tag_bitmap_tag_check
                                (unit, alloc_info.flags, module_id, res_tag_bitmap, pattern_first_index,
                                 allocated_block_length, alloc_info.tag, &tag_compare_result));
                if (!tag_compare_result || (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
                {
                    /*
                     * Looks good; mark allocation success, and adjust last free to miss this block.
                     */
                    allocation_success = TRUE;
                    update_last_free = TRUE;
                }
            }
        }

        /*
         * Allocate an ID in the given range if flag is provided and either allocation was not successful or
         * if previous allocation is out of range.
         */
        if ((alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE) && ((allocation_success == FALSE)
             || ((allocation_success == TRUE) && (pattern_first_index < (alloc_info.range_start - res_tag_bitmap->low)
               || pattern_first_index > (alloc_info.range_end - res_tag_bitmap->low)))))
        {
            update_last_free = FALSE;
            SHR_IF_ERR_EXIT(sw_state_resource_allocate_next_free_element_in_range(unit, module_id, res_tag_bitmap,
                            alloc_info, alloc_info.range_start - res_tag_bitmap->low,
                            alloc_info.range_end - res_tag_bitmap->low, &pattern_first_index, &allocation_success));
        }

        if (!allocation_success && !(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
        {

            /*
             * We couldn't reuse the last freed element. 
             * Start searching after last successful allocation.
             */
            nextAlloc = res_tag_bitmap->nextAlloc[tag_for_pointers];

            pattern_first_index =
                (((nextAlloc + alloc_info.align - 1) / alloc_info.align) * alloc_info.align) + alloc_info.offs[0];
            SHR_IF_ERR_EXIT(sw_state_resource_allocate_next_free_element_in_range(unit, module_id, res_tag_bitmap,
                            alloc_info, pattern_first_index, res_tag_bitmap->count, &pattern_first_index ,&allocation_success));

            if (!allocation_success)
            {
                /*
                 * Couldn't place element after the last successfully allocated element.
                 * Try searching from the start of the pool.
                 */
                pattern_first_index = alloc_info.offs[0];
                SHR_IF_ERR_EXIT(sw_state_resource_allocate_next_free_element_in_range(unit, module_id, res_tag_bitmap,
                                alloc_info, pattern_first_index, nextAlloc, &pattern_first_index, &allocation_success));
            }

            if (allocation_success)
            {
                /*
                 * got some space; update next alloc.
                 */
                update_next_alloc = TRUE;
            }
        }       /* if (_SHR_E_NONE == result) */
    }   /* if (flags & SHR_RES_ALLOC_WITH_ID) */
    if ((allocation_success) && !(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY))
    {
        int repeat, bit_offset, current, allocated_bits, use_sparse;
        int new_used_count = 0;
        /*
         * set the tag for all grains involved 
         * don't set tag if IGNORE_TAG flag is set.
         */
        if (!(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
        {
            SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_set_internal
                            (unit, module_id, res_tag_bitmap, alloc_info.tag,
                                    SW_STATE_RESOURCE_TAG_BITMAP_FORCE_TAG_NO_CHANGE, pattern_first_index, alloc_info.length));
        }
        /*
         * mark the block as in-use 
         */
        use_sparse = alloc_info.pattern != SW_STATE_RESOURCE_TAG_BITMAP_PATTERN_NOT_SPARSE;
        if (use_sparse)
        {
            for (repeat = 0, allocated_bits = 0, current = pattern_first_index; repeat < alloc_info.repeats; repeat++)
            {
                for (bit_offset = 0; bit_offset < alloc_info.length; bit_offset++, current++)
                {
                    if (alloc_info.pattern & (1 << bit_offset))
                    {
                        DNX_SW_STATE_BIT_SET(unit, module_id, 0, res_tag_bitmap->data, current);
                        if ((res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                            && !(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
                        {
                            DNX_SW_STATE_BIT_SET(unit, module_id, 0, res_tag_bitmap->tag_tracking_bitmap, current);
                        }
                        allocated_bits++;

                        /*
                         * Increment the number of allocated elements per grain.
                         */
                        if (res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
                        {
                            DNX_SW_STATE_COUNTER_INC(unit,
                                    module_id,
                                    res_tag_bitmap->nof_allocated_elements_per_grain[current/res_tag_bitmap->grainSize],
                                    1,
                                    res_tag_bitmap->nof_allocated_elements_per_grain[current/res_tag_bitmap->grainSize],
                                    DNXC_SW_STATE_NO_FLAGS,
                                    "res_tag_bitmap->nof_allocated_elements_per_grain[current / res_tag_bitmap->grainSize]");
                        }
                    }
                }
            }
        }
        else
        {
            DNX_SW_STATE_BIT_RANGE_SET(unit, module_id, 0, res_tag_bitmap->data, pattern_first_index,
                                       alloc_info.repeats);
            if ((res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                && !(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
            {
                DNX_SW_STATE_BIT_RANGE_SET(unit, module_id, 0, res_tag_bitmap->tag_tracking_bitmap, pattern_first_index,
                                           alloc_info.repeats);
            }
            allocated_bits = alloc_info.repeats;

            /*
             * Update the number of allocated elements in the relevant grain. The operation is set to TRUE because we are allocating elements.
             */
            if (res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
            {
                SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_nof_allocated_elements_per_bank_update
                                (unit, module_id, res_tag_bitmap, alloc_info.repeats, pattern_first_index,
                                 res_tag_bitmap->grainSize, TRUE));
            }
        }

        /** Supress journaling comparison - this data is not supposed to be restored after add-delete. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));
        if ((update_last_free == TRUE) && (!(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)))
        {
            tmp_update_value = pattern_first_index + allocated_block_length;
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &res_tag_bitmap->lastFree[tag_for_pointers],
                    &tmp_update_value, sizeof(tmp_update_value),
                    DNXC_SW_STATE_NO_FLAGS,
                    "res_tag_bitmap->lastFree[alloc_info.tag]");
        }

        if ((update_next_alloc == TRUE) && (!(alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)))
        {
            tmp_update_value = pattern_first_index + allocated_block_length;
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &res_tag_bitmap->nextAlloc[tag_for_pointers],
                    &tmp_update_value,
                    sizeof(tmp_update_value),
                    DNXC_SW_STATE_NO_FLAGS,
                    "res_tag_bitmap->nextAlloc[alloc_info.tag]");
        }
        /** Continue journaling comparison. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

        new_used_count = res_tag_bitmap->used + allocated_bits;

        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &res_tag_bitmap->used, &new_used_count, sizeof(new_used_count), DNXC_SW_STATE_NO_FLAGS, "res_tag_bitmap->used");

        /*
         * return the first allocated element in the pattern. 
         */
        *elem = pattern_first_index + res_tag_bitmap->low + utilex_lsb_bit_on(alloc_info.pattern);
    }
    else if (!allocation_success && (alloc_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_RESOURCE);
        SHR_EXIT();
    }
    else if (!allocation_success)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "No free element matching required conditions. "
                     "Asked for %d elements aligned to %d with offs %d \n", allocated_block_length, alloc_info.align,
                     alloc_info.offs[0]);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_clear(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap)
{
    int i;
    int zero = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_BIT_RANGE_CLEAR(unit, module_id, 0, res_tag_bitmap->data, 0, res_tag_bitmap->count);

    if (res_tag_bitmap->tagSize > 0)
    {
        DNX_SW_STATE_BIT_RANGE_CLEAR(unit, module_id, 0, res_tag_bitmap->tagData, 0,
                                     res_tag_bitmap->tagSize * (res_tag_bitmap->count / res_tag_bitmap->grainSize));
    }

    for (i = 0; i < res_tag_bitmap->max_tag_value; i++)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                module_id,
                &res_tag_bitmap->lastFree[i],
                &zero, sizeof(zero),
                DNXC_SW_STATE_NO_FLAGS,
                "res_tag_bitmap->lastFree");

        DNX_SW_STATE_MEMCPY_BASIC(unit,
                module_id,
                &res_tag_bitmap->nextAlloc[i],
                &zero,
                sizeof(zero),
                DNXC_SW_STATE_NO_FLAGS,
                "res_tag_bitmap->nextAlloc");

        if (res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG)
            break;
    }

    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &res_tag_bitmap->used, &zero, sizeof(zero), DNXC_SW_STATE_NO_FLAGS, "res_tag_bitmap->used");

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_tag_bitmap_nof_used_elements_in_grain_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 grain_index,
    uint32 *nof_allocated_elements_per_grain)
{
    SHR_FUNC_INIT_VARS(unit);

    if (res_tag_bitmap->flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
    {
        *nof_allocated_elements_per_grain = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index];
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "sw_state_resource_tag_bitmap_nof_used_elements_in_grain_get is not available for the bitmap. \n"
                     "This API can be used only if SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN flag was given during creation of the resource\n");
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
int sw_state_resource_tag_bitmap_size_get(
    sw_state_resource_tag_bitmap_create_info_t create_info)
{
    int size = 0;

    /*
     * Add the resource tag bitmap structure size.
     */
    size += (sizeof(sw_state_resource_tag_bitmap_t));

    /*
     * Add res_tag_bitmap->data bitmap size.
     */
    size += (sizeof(SHR_BITDCL)* (_SHR_BITDCLSIZE(create_info.count) + 1));

    if (create_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
    {
        /*
         * Add res_tag_bitmap->tag_tracking_bitmap bitmap size.
         */
        size += (sizeof(SHR_BITDCL)* (_SHR_BITDCLSIZE(create_info.count) + 1));
    }

    /*
     * Add nextAlloc and lastFree arrays size.
     */
    if (!(create_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG))
    {
        size += (sizeof(int)* (create_info.max_tag_value + 1));
        size += (sizeof(int)* (create_info.max_tag_value + 1));
    }
    else
    {
        size += (sizeof(int));
        size += (sizeof(int));
    }

    /*
     * Add the data bitmap size.
     */
    if (create_info.max_tag_value > 0)
    {
        size += (sizeof(SHR_BITDCL)* (_SHR_BITDCLSIZE((utilex_msb_bit_on(create_info.max_tag_value) +
                                                        1) * (create_info.count / create_info.grain_size)) + 1));
    }

    /*
     * Add nof_allocated_elements_per_grain array size.
     */
    if (create_info.flags & SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
    {
        size += (sizeof(int)* ((create_info.count / create_info.grain_size) + 1));
    }

    return size;
}

shr_error_e
sw_state_resource_tag_bitmap_create_info_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    sw_state_resource_tag_bitmap_create_info_t * create_info)
{
    SHR_FUNC_INIT_VARS(unit);

    create_info->low_id = res_tag_bitmap->low;
    create_info->count = res_tag_bitmap->count;
    create_info->grain_size = res_tag_bitmap->grainSize;
    create_info->flags = res_tag_bitmap->flags;
    create_info->max_tag_value = res_tag_bitmap->max_tag_value;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_resource_nof_allocated_elements_in_range_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    if (range_start < res_tag_bitmap->low)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range start(%d) must be bigger than resource tag bitmap low(%d).\n"
                        , range_start, res_tag_bitmap->low);
    }

    if ((range_start + nof_elements_in_range) > (res_tag_bitmap->low + res_tag_bitmap->count))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range end(%d) must be smaller than resource tag bitmap last element(%d).\n"
                         , (range_start + nof_elements_in_range), (res_tag_bitmap->low + res_tag_bitmap->count));
    }

    if (nof_elements_in_range < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range size(%d) must be at least 1.\n", nof_elements_in_range);
    }

    DNX_SW_STATE_BIT_RANGE_COUNT
        (unit, module_id, 0, res_tag_bitmap->data, range_start - res_tag_bitmap->low, nof_elements_in_range, nof_allocated_elements);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int sw_state_resource_tag_bitmap_print(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap)
{
    uint8 temp;
    uint32 index;
    uint8 el_count;
    SHR_FUNC_INIT_VARS(unit);

    if (res_tag_bitmap == NULL)
    {
        SHR_EXIT();
    }

    DNX_SW_STATE_PRINT(unit, "\nLowest element ID:%d | :", res_tag_bitmap->low);
    DNX_SW_STATE_PRINT(unit, "Number of elements:%d | :", res_tag_bitmap->count);
    DNX_SW_STATE_PRINT(unit, "Number of elements in use:%d | :", res_tag_bitmap->used);
    DNX_SW_STATE_PRINT(unit, "Grain size:%d | :", res_tag_bitmap->grainSize);
    DNX_SW_STATE_PRINT(unit, "Tag size:%d | :", res_tag_bitmap->tagSize);
    DNX_SW_STATE_PRINT(unit, "Max tag value:%d |\n:", res_tag_bitmap->max_tag_value);

    if (res_tag_bitmap->used > 0)
    {
        DNX_SW_STATE_PRINT(unit, "Used Elements:|");
        el_count = 0;
        for(index = 0; index < (res_tag_bitmap->low + res_tag_bitmap->count); index++)
        {
            temp = 0;
            DNX_SW_STATE_BIT_GET(unit, module_id, 0, res_tag_bitmap->data, index, &temp);
            if (temp)
            {
                DNX_SW_STATE_PRINT(unit, " %d |", index);
                el_count++;
            }

            if (el_count == 20)
            {
                DNX_SW_STATE_PRINT(unit, "\n");
                el_count = 0;
            }
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

#else
/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e
sw_state_resource_tag_bitmap_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

#endif /* BCM_DNX_SUPPORT */
