/*
 * $Id: utilex_res_tag_bitmap.c,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Indexed resource management -- simple bitmap
 */

typedef int make_iso_compilers_happy_utilex_res_rag_bitmap;

#ifdef BCM_DNX_SUPPORT

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_HASHDNX

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <shared/bitop.h>
#include <soc/drv.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_res_tag_bitmap.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/auto_generated/types/utilex_res_tag_bitmap_types.h>
#include <soc/dnx/swstate/auto_generated/access/utilex_res_tag_bitmap_access.h>
#endif
#include <bcm/error.h>

#ifdef BCM_DNX_SUPPORT

/*
 *  This is a fairly brute-force implementation of bitmap, with minimal
 *  optimisations or improvements.  It could probably be enhanced somewhat by
 *  some subtleties, such as checking whether a SHR_BITDCL is all ones before
 *  scanning individual bits when looking for free space.
 */

#define UTILEX_RES_TAG_BITMAP_TAG_SIZE_MAX   20

/* 
 * Since we use sparse_alloc and sparse_free as internal implementation of all allocation and free 
 *  operations, we use this as pattern and set the repeats = count.
 */
#define UTILEX_RES_TAG_BITMAP_PATTERN_NOT_SPARSE 0x1

/*
 *  If the max_tag_value is over UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE, then only allocate 1 member for the next_alloc and last_free arrays.
 */
#define UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE 10000

/*
 *  Sw state access macro.
 *   Since this module is used for both DNX (JR2) and DPP (JR+ and below), we need to seperate
 *     it by compilation.
 */
#define RES_BMP_ACCESS res_tag_bmp_info_db
/*
 * Convert input bmp handle to index in 'occupied_ids' array.
 * Convert input index in 'occupied_ids' array to bmp handle.
 * Indices go from 0 -> (occupied_ids - 1)
 * Handles go from 1 -> occupied_ids
 */
#define UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(_bmp_index,_handle) (_bmp_index = _handle - 1)
#define UTILEX_RES_TAG_BMP_CONVERT_BMP_INDEX_TO_HANDLE(_handle,_bmp_index) (_handle = _bmp_index + 1)

#define _UTILEX_RES_TAG_BITMAP_DATA_SET_ALL (1)

static shr_error_e
_utilex_res_tag_bitmap_data_get(
    int unit,
    int bmp_index,
    _utilex_res_tag_bitmap_list_t * bmp_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.used.get(unit, bmp_index, &bmp_data->used));

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.low.get(unit, bmp_index, &bmp_data->low));

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.grainSize.get(unit, bmp_index, &bmp_data->grainSize));

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagSize.get(unit, bmp_index, &bmp_data->tagSize));

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.count.get(unit, bmp_index, &bmp_data->count));

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.max_tag_value.get(unit, bmp_index, &bmp_data->max_tag_value));

    bmp_data->nextAlloc = NULL;
    bmp_data->lastFree = NULL;
    bmp_data->tagData = NULL;
    bmp_data->data = NULL;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_utilex_res_tag_bitmap_data_set(
    int unit,
    uint32 flags,
    int bmp_index,
    _utilex_res_tag_bitmap_list_t * bmp_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.used.set(unit, bmp_index, bmp_data->used));

    /*
     * Set the bitmap "static" data as well. 
     */
    if (flags & _UTILEX_RES_TAG_BITMAP_DATA_SET_ALL)
    {

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.count.set(unit, bmp_index, bmp_data->count));

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.low.set(unit, bmp_index, bmp_data->low));

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.grainSize.set(unit, bmp_index, bmp_data->grainSize));

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagSize.set(unit, bmp_index, bmp_data->tagSize));

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.max_tag_value.set(unit, bmp_index, bmp_data->max_tag_value));

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Initialize the res tag bitmap sw state.
 */
shr_error_e
utilex_res_tag_bitmap_init(
    int unit,
    int nof_bitmaps)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {

        /*
         * Init the sw state. 
         */
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.init(unit));

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.ptr_alloc(unit, nof_bitmaps));

        /*
         * Set the max nof bmps 
         */
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.max_nof_bmps.set(unit, nof_bitmaps));

        /*
         * Allocate the occupation bitmap. 
         */
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.occupied_ids.alloc_bitmap(unit, nof_bitmaps));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Free the res tag bitmap sw state.
 */
shr_error_e
utilex_res_tag_bitmap_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_res_tag_bitmap_create(
    int unit,
    utilex_res_tag_bitmap_handle_t * handle,
    int low_id,
    int count,
    int grain_size,
    uint32 max_tag_value)
{
    int found;
    uint32 nof_used_bmps, max_nof_bmps, bmp_index;
    uint8 bit_val;
    _utilex_res_tag_bitmap_list_t bmp_data;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments 
     */
    if (SOC_WARM_BOOT(unit))
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED, "Can't create bitmap during init\n");
    }
    SHR_NULL_CHECK(handle, _SHR_E_PARAM, "handle");
    if (0 >= count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must have a positive number of elements\n");
    }
    if (0 >= grain_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must have at least one element per grain\n");
    }
    if (count % grain_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Count %d is not a multiple of grains %d\n", count, grain_size);
    }

    /*
     * Make sure we can allocate another bitmap. 
     */

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.nof_in_use.get(unit, &nof_used_bmps));

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.max_nof_bmps.get(unit, &max_nof_bmps));

    if (nof_used_bmps >= max_nof_bmps)
    {
        /*
         * If number of occupied bitmap structures is beyond the
         * maximum then quit with error.
         */
        SHR_ERR_EXIT(_SHR_E_FULL, "Maximum number of resources pools exceeded.\n");
    }
    /*
     * Increment number of 'in_use' to cover the one we now intend to capture.
     */
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.nof_in_use.set(unit, (nof_used_bmps + 1)));

    /*
     * Find a free hash (a cleared bit in 'occupied_hashs'). At this point,
     * there must be one.
     */
    found = 0;
    for (bmp_index = 0; bmp_index < max_nof_bmps; bmp_index++)
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.occupied_ids.bit_get(unit, bmp_index, &bit_val));
        if (bit_val == 0)
        {
            /*
             * 'hash_table_index' is now the index of a free entry.
             */
            found = 1;
            break;
        }
    }
    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "No free bitmap handle found.\n");
    }
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.occupied_ids.bit_set(unit, bmp_index));

    /*
     * Set the returned bmp_data. Note that legal handles start at '1', not at '0'.
     */
    UTILEX_RES_TAG_BMP_CONVERT_BMP_INDEX_TO_HANDLE(*handle, bmp_index);

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.alloc(unit, bmp_index));

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.alloc_bitmap(unit, bmp_index, count));

    if (max_tag_value < UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE)
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.nextAlloc.alloc(unit, bmp_index, max_tag_value + 1));
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.lastFree.alloc(unit, bmp_index, max_tag_value + 1));
    }
    else
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.nextAlloc.alloc(unit, bmp_index, 1));
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.lastFree.alloc(unit, bmp_index, 1));
    }
    if (max_tag_value > 0)
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagData.alloc_bitmap(unit, bmp_index,
                                                                         (utilex_msb_bit_on(max_tag_value) +
                                                                          1) * (count / grain_size)));
    }

    /*
     * init descriptor and data 
     */

    sal_memset(&bmp_data, 0, sizeof(bmp_data));

    bmp_data.low = low_id;
    bmp_data.count = count;
    bmp_data.grainSize = grain_size;
    if (max_tag_value > 0)
    {
        bmp_data.tagSize = utilex_msb_bit_on(max_tag_value) + 1;
    }
    else
    {
        bmp_data.tagSize = 0;
    }
    bmp_data.max_tag_value = max_tag_value;

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_set(unit, _UTILEX_RES_TAG_BITMAP_DATA_SET_ALL, bmp_index, &bmp_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_res_tag_bitmap_destroy(
    int unit,
    utilex_res_tag_bitmap_handle_t handle)
{
    uint32 bmp_index;
    uint8 used;
    uint32 nof_in_use;
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.occupied_ids.bit_get(unit, bmp_index, &used));

    if (used)
    {
        /*
         * Free the memory used by the pool. 
         */
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.is_allocated(unit, bmp_index, &is_allocated));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.free(unit, bmp_index));    /* The bitmap itself */
        }

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagData.is_allocated(unit, bmp_index, &is_allocated));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagData.free(unit, bmp_index)); /* Bitmap tag */
        }

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.is_allocated(unit, bmp_index, &is_allocated));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.free(unit, bmp_index)); /* Bitmap struct */
        }

        if (!SOC_IS_DETACHING(unit))
        {
            /*
             * Remove the bitmap from the count and occupation bitmap. 
             * Only do this if soc is not detaching. If soc is detaching, then we either need to 
             * keep it in WB, or we just don't care. 
             */
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.nof_in_use.get(unit, &nof_in_use));

            nof_in_use--;

            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.nof_in_use.set(unit, nof_in_use));

            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.occupied_ids.bit_clear(unit, bmp_index));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unable to free unallocated bitmap.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Compare tags by element. 
 */
shr_error_e
utilex_res_tag_bitmap_compare_tags(
    int unit,
    uint32 bmp_index,
    int tag_index,
    uint32 tag,
    uint8 *equal)
{
    int tag_size;
    SHR_BITDCL tmp;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagSize.get(unit, bmp_index, &tag_size));

    /*
     * Start by assuming that the tags are equal. If one element is different, change it. 
     */
    *equal = TRUE;
    tmp = 0;

    /*
     * Compare the tag with the tagData.
     */
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.
                    tagData.bit_range_read(unit, bmp_index, (tag_index * tag_size), 0, tag_size, &tmp));
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
_utilex_res_tag_bitmap_tag_check(
    int unit,
    uint32 flags,
    uint32 bmp_index,
    int elemIndex,
    int elemCount,
    uint32 tag,
    int *result)
{
    int grain_size, tag_size;
    uint8 always_check = _SHR_IS_FLAG_SET(flags, UTILEX_RES_TAG_BITMAP_ALWAYS_CHECK_TAG);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.grainSize.get(unit, bmp_index, &grain_size));
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagSize.get(unit, bmp_index, &tag_size));

    *result = 0;

    /*
     * Check if this bitmap is even using tags.
     */
    if (tag_size > 0)
    {
        int current_elem;
        uint8 temp, equal;
        int grain_start, tag_index;

        /*
         * Iterate over all the tags that are attached to the element block, and verify that they either have 
         *   the same tag, or don't have a tag. 
         */
        for (current_elem = elemIndex; current_elem < elemIndex + elemCount; current_elem += grain_size)
        {
            temp = 0;
            if (!always_check)
            {
                /*
                 * If we don't force check for the tag, then we check if there are any allocated elements in the 
                 * current grain. If there are, it means the tag is set for this range. 
                 */
                grain_start = (current_elem / grain_size) * grain_size;
                SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.bit_range_test(unit, bmp_index, grain_start,
                                                                                grain_size, &temp));
            }

            if (temp || always_check)
            {
                /*
                 * the grain is used by at least one other block 
                 */
                tag_index = current_elem / grain_size;
                SHR_IF_ERR_EXIT(utilex_res_tag_bitmap_compare_tags(unit, bmp_index, tag_index, tag, &equal));

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
_utilex_res_tag_bitmap_check_all_tag(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
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
    uint32 bmp_index;
    _utilex_res_tag_bitmap_list_t bmp_data;
    SHR_FUNC_INIT_VARS(unit);

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data));

    if (tag > bmp_data.max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag, bmp_data.max_tag_value);
    }

    /*
     * scan the block 
     */
    for (offset = 0; offset < count; offset++)
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index + offset, &bit_used));
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
        if (0 < bmp_data.tagSize)
        {
            tagbase = index / bmp_data.grainSize;
            for (offset = 0; offset < count; offset += bmp_data.grainSize, tagbase++)
            {
                SHR_IF_ERR_EXIT(utilex_res_tag_bitmap_compare_tags(unit, bmp_index, tagbase, tag, &equal));
                if (!equal)
                {
                    /*
                     * tags are not equal, so different blocks 
                     */
                    result = _SHR_E_CONFIG;
                }
            }   /* for (all grains in the block) */
        }       /* if (0 < bmp_data.tagSize) */
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
 *  Ensure that all grains in an alloc are tagged accordingly.
 */
shr_error_e
utilex_res_tag_bitmap_tag_set(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 tag,
    int elemIndex,
    int elemCount)
{
    int index;
    int count;
    int offset;
    uint32 bmp_index;
    SHR_BITDCL tmp;
    _utilex_res_tag_bitmap_list_t bmp_data;
    SHR_FUNC_INIT_VARS(unit);

    tmp = tag;

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data));

    if (tag > bmp_data.max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag, bmp_data.max_tag_value);
    }

    if (bmp_data.tagSize)
    {
        index = elemIndex / bmp_data.grainSize;
        count = (elemCount + bmp_data.grainSize - 1) / bmp_data.grainSize;
        for (offset = 0; offset < count; offset++)
        {
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.
                            tagData.bit_range_write(unit, bmp_index, ((index + offset) * bmp_data.tagSize), 0,
                                                    bmp_data.tagSize, &tmp));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_res_tag_bitmap_tag_get(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int elemIndex,
    uint32 *tag)
{
    uint32 bmp_index;
    _utilex_res_tag_bitmap_list_t bmp_data;
    SHR_BITDCL tmp;
    SHR_FUNC_INIT_VARS(unit);

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data));

    tmp = 0;
    *tag = 0;
    if (bmp_data.tagSize)
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagData.bit_range_read(unit, bmp_index,
                                                                           ((elemIndex / bmp_data.grainSize) *
                                                                            bmp_data.tagSize), 0, bmp_data.tagSize,
                                                                           &tmp));
        *tag = tmp;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_res_tag_bitmap_check_all_tag(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 tag,
    int count,
    int elem)
{
    int index;
    uint32 bmp_index;
    _utilex_res_tag_bitmap_list_t bmp_data;
    SHR_FUNC_INIT_VARS(unit);

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data));

    /*
     * check arguments 
     */
    /*
     * check arguments 
     */
    if (!handle)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal handle id. Must be >0");
    }
    if (elem < bmp_data.low)
    {
        /*
         * not valid ID 
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Starting element is too small: %d. Must be at least %d.", elem, bmp_data.low);
    }
    if (0 >= count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Count must be >0. Was: %d", count);
    }
    index = elem - bmp_data.low;
    if (index + count > bmp_data.count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Element + count is too high: %d. Must be at most %d.", elem + count,
                     bmp_data.low + bmp_data.count);
    }
    if (tag > bmp_data.max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag, bmp_data.max_tag_value);
    }

    SHR_SET_CURRENT_ERR(_utilex_res_tag_bitmap_check_all_tag(unit, handle, tag, count, index));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_res_tag_bitmap_check(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int count,
    int elem)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = utilex_res_tag_bitmap_check_all_tag(unit, handle, 0, count, elem);

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
utilex_res_tag_bitmap_check_all(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int count,
    int elem)
{
    uint32 first_tag;
    uint32 bmp_index;
    int index;
    _utilex_res_tag_bitmap_list_t bmp_data;
    SHR_FUNC_INIT_VARS(unit);

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data));

    if (!handle)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal handle id. Must be >0");
    }
    if (elem < bmp_data.low)
    {
        /*
         * not valid ID 
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Starting element is too small: %d. Must be at least %d.", elem, bmp_data.low);
    }
    if (0 >= count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Count must be >0. Was: %d", count);
    }
    index = elem - bmp_data.low;
    if (index + count > bmp_data.count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Element + count is too high: %d. Must be at most %d.", elem + count,
                     bmp_data.low + bmp_data.count);
    }
    /*
     * here we assume the tag from the first grain in the range. Copy it to the buffer. 
     */
    SHR_IF_ERR_EXIT(utilex_res_tag_bitmap_tag_get(unit, handle, index, &first_tag));

    SHR_SET_CURRENT_ERR(_utilex_res_tag_bitmap_check_all_tag(unit, handle, first_tag, count, index));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_res_tag_bitmap_nof_free_elements_get(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int *nof_free_elements)
{
    uint32 bmp_index;
    _utilex_res_tag_bitmap_list_t bmp_data;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments 
     */
    if (!handle)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal handle id. Must be >0");
    }
    SHR_NULL_CHECK(nof_free_elements, _SHR_E_PARAM, "nof_free_elements");

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);
    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data));

    /*
     * The number of free elements is the number of existing elements (count) -
     *    the number of used elements.
     */
    *nof_free_elements = bmp_data.count - bmp_data.used;

exit:
    SHR_FUNC_EXIT;
}

#if 0
/* 
 * Right now we don't need this dump function, but keep it around anyway. 
 */
int
utilex_res_tag_bitmap_dump(
    int unit,
    const utilex_res_tag_bitmap_handle_t handle)
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
    _utilex_res_tag_bitmap_list_t bmp_data;
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
    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    UTILEX_ACCESS_ERROR_CHECK(result);

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

shr_error_e
utilex_res_tag_bitmap_alloc_tag(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    uint32 tag,
    int count,
    int *elem)
{
    /*
     * Regular allocation is identical to aligned allocation with an alignement of 1, offset 0 
     */
    int offs = 0, align = 1;
    return utilex_res_tag_bitmap_alloc_align_tag(unit, handle, flags, align, offs, tag, count, elem);
}

shr_error_e
utilex_res_tag_bitmap_alloc(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    int count,
    int *elem)
{
    return utilex_res_tag_bitmap_alloc_tag(unit, handle, flags, 0, count, elem);
}

shr_error_e
utilex_res_tag_bitmap_alloc_align_tag(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    int align,
    int offs,
    uint32 tag,
    int count,
    int *elem)
{
    uint32 pattern;
    int length = 1, repeats = count;

    pattern = UTILEX_RES_TAG_BITMAP_PATTERN_NOT_SPARSE;

    return utilex_res_tag_bitmap_alloc_align_tag_sparse(unit, handle, flags, align, offs, tag, pattern, length,
                                                        repeats, elem);
}

shr_error_e
utilex_res_tag_bitmap_alloc_align(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    int align,
    int offs,
    int count,
    int *elem)
{
    return utilex_res_tag_bitmap_alloc_align_tag(unit, handle, flags, align, offs, 0, count, elem);
}

shr_error_e
utilex_res_tag_bitmap_free(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    int count,
    int elem)
{
    uint32 pattern;
    uint32 length;
    int repeats;

    pattern = UTILEX_RES_TAG_BITMAP_PATTERN_NOT_SPARSE;
    length = 1;
    repeats = count;

    return utilex_res_tag_bitmap_free_sparse(unit, handle, pattern, (int) length, repeats, elem);
}

shr_error_e
utilex_res_tag_bitmap_free_sparse(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 pattern,
    int length,
    int repeats,
    int elem)
{
    int offset;
    int final_bit_in_pattern;
    uint32 pattern_mask;
    uint32 bmp_index;
    _utilex_res_tag_bitmap_list_t bmp_data;
    uint8 temp;
    uint8 free_element;
    int current_element, first_index_in_pattern;
    int repeat_index;
    int use_sparse;
    uint32 currentTag;
    SHR_FUNC_INIT_VARS(unit);

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data));

    /*
     * check arguments
     */
    if (!handle)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal handle id. Must be >0");
    }
    if (elem < bmp_data.low)
    {
        /*
         * not valid ID 
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Starting element is too small: %d. Must be at least %d.", elem, bmp_data.low);
    }
    if (0 >= length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must free a positive number of elements. Length was %d", length);
    }
    if (32 < length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must not be longer than 32. Was %d", length);
    }
    if (0 >= repeats)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must check at least one pattern. Repeats was %d\n", repeats);
    }
    pattern_mask = (((uint32) 1 << (length - 1)) - 1) | ((uint32) 1 << (length - 1));
    if (0 == (pattern & pattern_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must contain at least one element. Pattern was 0x%x and length was %d",
                     pattern, length);
    }
    if (pattern & ~pattern_mask)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must not contain unused bits. Pattern was 0x%x and length was %d", pattern,
                     length);
    }

    final_bit_in_pattern = (utilex_msb_bit_on(pattern) + 1) + (length * (repeats - 1));
    first_index_in_pattern = elem - bmp_data.low - utilex_lsb_bit_on(pattern);

    if (first_index_in_pattern + final_bit_in_pattern > bmp_data.count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given input will exceed the maximum elemnt in the resource. "
                     "Element was %d, pattern was 0x%x and repeats was %d", elem, pattern, repeats);
    }

    /*
     * If the pattern is simple one, we can save run time but not going bit by bit, but by going en mass.
     */
    use_sparse = pattern != UTILEX_RES_TAG_BITMAP_PATTERN_NOT_SPARSE;

    for (free_element = 0; free_element < 2; free_element++)
    {
        current_element = first_index_in_pattern;
        for (repeat_index = 0; repeat_index < repeats; repeat_index++)
        {
            for (offset = 0; offset < length; offset++, current_element++)
            {
                if (pattern & (1 << offset))
                {
                    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, current_element, &temp));
                    if (temp)
                    {
                        if (free_element)
                        {
                            if (use_sparse)
                            {
                                RES_BMP_ACCESS.bitmap_lists.data.bit_clear(unit, bmp_index, current_element);
                                bmp_data.used--;
                            }
                            else
                            {
                                /*
                                 * We can just free the entire range when we get to the first bit, then break the loop.
                                 *   In this case, repeats indicates how many elements to deallocate. 
                                 */
                                RES_BMP_ACCESS.bitmap_lists.data.bit_range_clear(unit, bmp_index, current_element,
                                                                                 repeats);
                                bmp_data.used -= repeats;

                                /*
                                 * Set loop variables to maximum to break the loop.
                                 */
                                offset = length;
                                repeat_index = repeats;
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

    /*
     * Update the last free indication with the first bit that was freed in this pattern.
     */

    SHR_IF_ERR_EXIT(utilex_res_tag_bitmap_tag_get(unit, handle, first_index_in_pattern, &currentTag));
    if (bmp_data.max_tag_value < UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE)
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.lastFree.set(unit, bmp_index, currentTag, first_index_in_pattern));
    }

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_set(unit, 0, bmp_index, &bmp_data));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
utilex_check_element_for_allocation(
    int unit,
    int bmp_index,
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
    use_sparse = pattern != UTILEX_RES_TAG_BITMAP_PATTERN_NOT_SPARSE;

    if (use_sparse)
    {
        for (repeat = 0, current_index = element; repeat < repeats; repeat++)
        {
            for (bit_offset = 0; bit_offset < length; bit_offset++, current_index++)
            {
                if (pattern & (1 << bit_offset))
                {
                    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, current_index, &temp));

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
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.bit_range_test(unit, bmp_index, element, repeats, &temp));
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
utilex_allocate_next_free_element_in_range(
    int unit,
    int bmp_index,
    int flags,
    int range_start,
    int range_end,
    int align,
    int offs,
    uint32 tag,
    uint32 pattern,
    int length,
    int repeats,
    int *elem,
    int *found)
{
    int index, last_legal_index, pattern_first_set_bit;
    int found_match, allocated_block_length;
    int tag_compare_result, skip_block;
    int grain_size, grain_count;
    uint8 temp;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.grainSize.get(unit, bmp_index, &grain_size));

    /*
     * Iterate between range_start and range_end, and check for suitable elemnts in the middle.
     */
    found_match = FALSE;
    index = range_start;
    allocated_block_length = (utilex_msb_bit_on(pattern) + 1) + (length * (repeats - 1));
    last_legal_index = range_end - allocated_block_length;
    pattern_first_set_bit = utilex_lsb_bit_on(pattern);

    while (index <= last_legal_index)
    {
        /*
         * First, skip grains until a grain with an appropriate tag is found.
         */
        do
        {
            skip_block = FALSE;
            SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_tag_check
                            (unit, flags, bmp_index, index, allocated_block_length, tag, &tag_compare_result));

            if (!tag_compare_result)
            {
                /*
                 * Tags are matching, but skip grain if it's full.
                 */
                int first_element_in_grain = index - index % grain_size;
                SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.
                                data.bit_range_count(unit, bmp_index, first_element_in_grain, grain_size,
                                                     &grain_count));

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
                index = (((index + align - 1) / align) * align) + offs;
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

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.
                        data.bit_get(unit, bmp_index, index + pattern_first_set_bit, &temp));

        /*
         * temp indicates whether the first required element in the pattern starting from index is taken. Keep searching until 
         *  it's false, or until we finished scanning the range.
         */
        while (temp && (index <= last_legal_index))
        {
            index += align;
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.
                            data.bit_get(unit, bmp_index, index + pattern_first_set_bit, &temp));
        }

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
        SHR_IF_ERR_EXIT(utilex_check_element_for_allocation(unit, bmp_index, pattern,
                                                            length, repeats, index, &found_match));

        if (found_match)
        {
            /*
             * Found a matching block with matching tag. We might have exceeded grain boundry, 
             * so check the tag again. 
             */
            SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_tag_check
                            (unit, flags, bmp_index, index, allocated_block_length, tag, &tag_compare_result));

            if (tag_compare_result)
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
            index += align;
            continue;
        }
    }

    *elem = index;
    *found = found_match;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_res_tag_bitmap_alloc_align_tag_sparse(
    int unit,
    utilex_res_tag_bitmap_handle_t handle,
    uint32 flags,
    int align,
    int offs,
    uint32 tag,
    uint32 pattern,
    int length,
    int repeats,
    int *elem)
{
    int pattern_first_index;
    int allocated_block_length;
    uint32 pattern_mask;
    int can_allocate;
    uint32 bmp_index;
    _utilex_res_tag_bitmap_list_t bmp_data;
    int tag_compare_result;
    int allocation_success = FALSE;
    int update_last_free = FALSE;
    int update_next_alloc = FALSE;
    int nextAlloc = 0, lastFree = 0;
    SHR_FUNC_INIT_VARS(unit);

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data));

    /*
     * check arguments
     */
    if (!handle)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal handle id. Must be >0");
    }
    SHR_NULL_CHECK(elem, _SHR_E_PARAM, "elem");
    if (0 >= length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern length must be at least 1.");
    }
    if (32 < length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern length must be at most 32.");
    }
    if (0 >= repeats)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern repeats must be at least 1.");
    }
    pattern_mask = (((uint32) 1 << (length - 1)) - 1) | ((uint32) 1 << (length - 1));
    if (0 == (pattern & pattern_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "pattern must contain at least one element.");
    }
    if (pattern & ~pattern_mask)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "pattern must not contain unused bits.");
    }
    if (bmp_data.count == bmp_data.used)
    {
        if (flags & UTILEX_RES_TAG_BITMAP_ALLOC_CHECK_ONLY)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_RESOURCE);
            SHR_EXIT();
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Can't allocate element because the resource is already full.");
        }
    }
    if (tag > bmp_data.max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag, bmp_data.max_tag_value);
    }

    /*
     * Calculate the length of the allocated block. We can ignore trailing 0s in the pattern.
     */
    allocated_block_length = (utilex_msb_bit_on(pattern) + 1) + (length * (repeats - 1));

    if (flags & UTILEX_RES_TAG_BITMAP_ALLOC_WITH_ID)
    {
        int element_offset;
        /*
         * WITH_ID, so only try the specifically requested block.
         */
        if (*elem < bmp_data.low)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Given element %d is too low. Must be at least %d.", *elem, bmp_data.low);
        }
        pattern_first_index = *elem - bmp_data.low - utilex_lsb_bit_on(pattern);
        if (pattern_first_index + allocated_block_length > bmp_data.count)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Allocating %d elements starting from element %d will exceed the maximum element %d.",
                         allocated_block_length, *elem, bmp_data.low + bmp_data.count - 1);
        }

        /*
         * Make sure caller's request is valid.
         */
        if (flags & UTILEX_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO)
        {
            /*
             * alignment is against zero
             */
            element_offset = (*elem) % align;
        }
        else
        {
            /*
             * alignment is against low
             */
            element_offset = ((*elem) - bmp_data.low) % align;
        }
        if (element_offset != offs)
        {
            
        }
        /*
         * check whether the block is free
         */
        SHR_IF_ERR_EXIT(utilex_check_element_for_allocation(unit, bmp_index, pattern,
                                                            length, repeats, pattern_first_index, &can_allocate));

        if (!can_allocate)
        {
            /*
             * In use; can't do WITH_ID alloc of this block
             */
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Given element, or some of its block, already exists. Asked for %d elements "
                         "starting from index %d\n", allocated_block_length, *elem);
        }

        SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_tag_check
                        (unit, flags, bmp_index, pattern_first_index, allocated_block_length, tag,
                         &tag_compare_result));
        if (tag_compare_result)
        {
            /*
             * One of the elements has a mismatching tag.
             */
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Given element, or some of its block, have a mismatching tag."
                         "Asked for %d elements starting from index %d with tag %d",
                         allocated_block_length, *elem, tag);
        }
        /*
         * If we got here then we successfully allocated. 
         * Don't adjust last free or next alloc for WITH_ID.
         */
        allocation_success = TRUE;
    }
    else
    {   /* if (flags & SHR_RES_ALLOC_WITH_ID) */

        if (flags & UTILEX_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO)
        {
            /*
             * Adjust offset to represent alignment against zero, not start of pool.
             */
            offs = (offs + align - (bmp_data.low % align)) % align;
        }

        /*
         * Try to allocate immediatly after the last freed element.
         */
        if (bmp_data.max_tag_value < UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE)
        {
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.lastFree.get(unit, bmp_index, tag, &lastFree));
        }
        pattern_first_index = UTILEX_ALIGN_UP(lastFree, align) + offs;
        if (pattern_first_index + allocated_block_length < bmp_data.count)
        {
            /*
             * it might fit
             */
            SHR_IF_ERR_EXIT(utilex_check_element_for_allocation(unit, bmp_index, pattern,
                                                                length, repeats, pattern_first_index, &can_allocate));

            if (can_allocate)
            {
                SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_tag_check
                                (unit, flags, bmp_index, pattern_first_index, allocated_block_length, tag,
                                 &tag_compare_result));
                if (!tag_compare_result)
                {
                    /*
                     * Looks good; mark allocation success, and adjust last free to miss this block.
                     */
                    allocation_success = TRUE;
                    update_last_free = TRUE;
                }
            }
        }

        if (!allocation_success)
        {

            /*
             * We couldn't reuse the last freed element. 
             * Start searching after last successful allocation.
             */
            if (bmp_data.max_tag_value < UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE)
            {
                SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.nextAlloc.get(unit, bmp_index, tag, &nextAlloc));
            }
            pattern_first_index = (((nextAlloc + align - 1) / align) * align) + offs;
            SHR_IF_ERR_EXIT(utilex_allocate_next_free_element_in_range(unit, bmp_index, flags, pattern_first_index,
                                                                       bmp_data.count, align, offs, tag,
                                                                       pattern, length, repeats,
                                                                       &pattern_first_index, &allocation_success));

            if (!allocation_success)
            {
                /*
                 * Couldn't place element after the last succesfully allocated element. 
                 * Try searching from the start of the pool.
                 */
                pattern_first_index = offs;
                SHR_IF_ERR_EXIT(utilex_allocate_next_free_element_in_range
                                (unit, bmp_index, flags, pattern_first_index, nextAlloc, align, offs, tag,
                                 pattern, length, repeats, &pattern_first_index, &allocation_success));
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
    if ((allocation_success) && !(flags & UTILEX_RES_TAG_BITMAP_ALLOC_CHECK_ONLY))
    {
        int repeat, bit_offset, current, allocated_bits, use_sparse;
        /*
         * set the tag for all grains involved 
         */
        SHR_IF_ERR_EXIT(utilex_res_tag_bitmap_tag_set(unit, handle, tag, pattern_first_index, length));
        /*
         * mark the block as in-use 
         */
        use_sparse = pattern != UTILEX_RES_TAG_BITMAP_PATTERN_NOT_SPARSE;
        if (use_sparse)
        {
            for (repeat = 0, allocated_bits = 0, current = pattern_first_index; repeat < repeats; repeat++)
            {
                for (bit_offset = 0; bit_offset < length; bit_offset++, current++)
                {
                    if (pattern & (1 << bit_offset))
                    {
                        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.bit_set(unit, bmp_index, current));
                        allocated_bits++;
                    }
                }
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.
                            data.bit_range_set(unit, bmp_index, pattern_first_index, repeats));
            allocated_bits = repeats;
        }

        if (update_last_free == TRUE && bmp_data.max_tag_value < UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE)
        {
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.
                            lastFree.set(unit, bmp_index, tag, pattern_first_index + allocated_block_length));
        }

        if (update_next_alloc == TRUE && bmp_data.max_tag_value < UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE)
        {
            SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.
                            nextAlloc.set(unit, bmp_index, tag, pattern_first_index + allocated_block_length));
        }

        bmp_data.used += allocated_bits;

        SHR_IF_ERR_EXIT(_utilex_res_tag_bitmap_data_set(unit, 0, bmp_index, &bmp_data));

        /*
         * return the first allocated element in the pattern. 
         */
        *elem = pattern_first_index + bmp_data.low + utilex_lsb_bit_on(pattern);
    }
    else if (!allocation_success && (flags & UTILEX_RES_TAG_BITMAP_ALLOC_CHECK_ONLY))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_RESOURCE);
        SHR_EXIT();
    }
    else if (!allocation_success)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "No free element matching required conditions. "
                     "Asked for %d elements aligned to %d with offs %d", allocated_block_length, align, offs);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
utilex_res_tag_bitmap_clear(
    int unit,
    utilex_res_tag_bitmap_handle_t handle)
{
    int count;
    int tag_size, max_tag_value, grain_size;
    int i;
    uint32 bmp_index;
    SHR_FUNC_INIT_VARS(unit);

    UTILEX_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.count.get(unit, bmp_index, &count));
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.max_tag_value.get(unit, bmp_index, &max_tag_value));
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagSize.get(unit, bmp_index, &tag_size));
    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.grainSize.get(unit, bmp_index, &grain_size));

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.data.bit_range_clear(unit, bmp_index, 0, count));

    if (tag_size > 0)
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.tagData.bit_range_clear(unit, bmp_index, 0,
                                                                            tag_size * (count / grain_size)));
    }

    for (i = 0; i < max_tag_value; i++)
    {
        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.lastFree.set(unit, bmp_index, i, 0));

        SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.nextAlloc.set(unit, bmp_index, i, 0));

        if (max_tag_value >= UTILEX_RES_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE)
            break;
    }

    SHR_IF_ERR_EXIT(RES_BMP_ACCESS.bitmap_lists.used.set(unit, bmp_index, 0));

exit:
    SHR_FUNC_EXIT;
}

#else
/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e
utilex_res_tag_bitmap_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

#endif

#endif /* BCM_DNX_SUPPORT */
