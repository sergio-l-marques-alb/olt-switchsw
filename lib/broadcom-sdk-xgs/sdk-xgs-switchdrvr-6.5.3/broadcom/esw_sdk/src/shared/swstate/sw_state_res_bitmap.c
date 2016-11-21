/*
 * $Id: sw_state_res_bitmap.c,v 1.12 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * Indexed resource management -- simple bitmap
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <bcm/error.h>
#include <soc/cm.h>
#include <shared/swstate/sw_state_res_bitmap.h>
#include <shared/swstate/sw_state_res_tag_bitmap.h>
#include <shared/swstate/access/sw_state_access.h>

/*
 *  Macros and other things that change according to settings...
 */
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
#define SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT (handle->nextAlloc)
#else /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
#define SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT (handle->count - count)
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */

/* For code simplicity, we use tag bitmap as the engine for regular bitmaps. As a placeholder, we use this flag to comment out
   the bitmap code. */
#define SW_STATE_RES_BITMAP_USE_TAG_BITMAP 1

/*
 * Convert input hash table handle to index in 'occupied_hashs' array.
 * Convert input index in 'occupied_hashs' array to hash table handle.
 * Indices go from 0 -> (occupied_hashs - 1)
 * Handles go from 1 -> occupied_hashs
 */
#define SW_STATE_RES_BMP_CONVERT_HANDLE_TO_BMP_INDEX(_bmp_index,_handle) (_bmp_index = _handle - 1)
#define SW_STATE_RES_BMP_CONVERT_BMP_INDEX_TO_HANDLE(_handle,_bmp_index) (_handle = _bmp_index + 1)


#define SW_STATE_ACCESS_ERROR_CHECK(rv) \
    if (rv != BCM_E_NONE) {             \
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,    \
                  (BSL_META("Error in sw state access\n")));    \
        return BCM_E_INTERNAL;  \
    }

#if !(SW_STATE_RES_BITMAP_USE_TAG_BITMAP)
/* inline */ static int
_sw_state_res_bitmap_check_all(int unit,
                          sw_state_res_bitmap_handle_t handle,
                          int count,
                          int index)
{
    int offset;
    int freed = 0;
    int inuse = 0;

    /* scan the block */
    for (offset = 0; offset < count; offset++) {
        if (SHR_BITGET(handle->data, index + offset)) {
            inuse++;
        } else {
            freed++;
        }
    } /* for (offset = 0; offset < count; offset++) */
    if (inuse == count) {
        /* block is entirely in use */
        return BCM_E_FULL;
    } else if (freed == count) {
        /* block is entirely free */
        return BCM_E_EMPTY;
    } else {
        /* block is partially free and partially in use */
        return BCM_E_EXISTS;
    }
}

/* inline */ static int
_sw_state_res_bitmap_check_all_sparse(int unit,
                                 sw_state_res_bitmap_handle_t handle,
                                 uint32 pattern,
                                 int length,
                                 int repeats,
                                 int base)
{
    int index;
    int offset;
    int elem;
    int elemCount;
    int usedCount;

    for (index = 0, elem = base, elemCount = 0, usedCount = 0;
         index < repeats;
         index++) {
        for (offset = 0; offset < length; offset++, elem++) {
            if (pattern & (1 << offset)) {
                /* this element is in the pattern */
                elemCount++;
                if (SHR_BITGET(handle->data, elem)) {
                    /* this element is in use */
                    usedCount++;
                }
            } /* if (pattern & (1 << offset)) */
        } /* for (the length of the pattern) */
    } /* for (as many times as the pattern repeats) */
    if (elemCount == usedCount) {
        /* block is entirely in use */
        return BCM_E_FULL;
    } else if (0 == usedCount) {
        /* block is entirely free */
        return BCM_E_EMPTY;
    } else {
        /* block is partially free and partially in use */
        return BCM_E_EXISTS;
    }
}

#endif

int
sw_state_res_bitmap_create(int unit,
                      uint32 *handle,
                      int low_id,
                      int count)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int rv, found;
    uint32 nof_used_bmps, max_nof_bmps, bmp_index;
    uint8 bit_val;

    /* check arguments */
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must have a positive number of elements\n")));
        return BCM_E_PARAM;
    }
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument must not be NULL\n")));
        return BCM_E_PARAM;
    }
    /* Make sure we can allocate another bitmap. */

    rv = RES_BMP_ACCESS.nof_in_use.get(unit, &nof_used_bmps);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    rv = RES_BMP_ACCESS.max_nof_bmps.get(unit, &max_nof_bmps);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    if (nof_used_bmps >= max_nof_bmps)
    {
      /*
       * If number of occupied bitmap structures is beyond the
       * maximum then quit with error.
       */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
            (BSL_META("Maximum number of resources pools exceeded.\n")));
        return BCM_E_FULL;
    }
    /*
     * Increment number of 'in_use' to cover the one we now intend to capture.
     */
    rv = RES_BMP_ACCESS.nof_in_use.set(unit, (nof_used_bmps + 1));
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    /*
     * Find a free hash (a cleared bit in 'occupied_hashs'). At this point,
     * there must be one.
     */
    found = 0 ;
    for (bmp_index = 0 ; bmp_index < max_nof_bmps ; bmp_index++)
    {
      rv = RES_BMP_ACCESS.occupied_ids.bit_get(unit, bmp_index, &bit_val);
      SW_STATE_ACCESS_ERROR_CHECK(rv);
      if (bit_val == 0)
      {
        /*
         * 'hash_table_index' is now the index of a free entry.
         */
        found = 1 ;
        break ;
      }
    }
    if (!found)
    {
      LOG_ERROR(BSL_LS_SHARED_SWSTATE,
            (BSL_META("No free bitmap handle found.\n")));
        return BCM_E_FULL;
    }
    rv = RES_BMP_ACCESS.occupied_ids.bit_set(unit, bmp_index);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    rv = RES_BMP_ACCESS.bitmap_lists.alloc(unit, bmp_index);
    SW_STATE_ACCESS_ERROR_CHECK(rv);
    /*
     * Set the returned handle. Note that legal handles start at '1', not at '0'.
     */
    SW_STATE_RES_BMP_CONVERT_BMP_INDEX_TO_HANDLE(*handle, bmp_index);

    rv = RES_BMP_ACCESS.bitmap_lists.data.alloc_bitmap(unit, bmp_index, count);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    /* init descriptor and data */

    rv = RES_BMP_ACCESS.bitmap_lists.low.set(unit, bmp_index, low_id);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    rv = RES_BMP_ACCESS.bitmap_lists.count.set(unit, bmp_index, count);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    /* all's well if we got here */
    return BCM_E_NONE;
#else
    /* The grain size is the entire bitmap, and the tag is non existent. */
    return sw_state_res_tag_bitmap_create(unit, handle, low_id, count, count, 0);
#endif
}

int
sw_state_res_bitmap_destroy(int unit,
                       sw_state_res_bitmap_handle_t handle)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    if (handle) {
        sal_free(handle);
        return BCM_E_NONE;
    } else {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to free NULL handle\n")));
        return BCM_E_PARAM;
    }
#else 
    return sw_state_res_tag_bitmap_destroy(unit, handle);
#endif
}

int
sw_state_res_bitmap_alloc(int unit,
                     sw_state_res_bitmap_handle_t handle,
                     uint32 flags,
                     int count,
                     int *elem)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int index;
    int offset;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must allocate at least one element\n")));
        return BCM_E_PARAM;
    }
    if (SW_STATE_RES_BITMAP_ALLOC_REPLACE ==
        (flags & (SW_STATE_RES_BITMAP_ALLOC_REPLACE |
                  SW_STATE_RES_BITMAP_ALLOC_WITH_ID))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must use WITH_ID when using REPLACE\n")));
        return BCM_E_PARAM;
    }

    if (flags & SW_STATE_RES_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < handle->low) {
            /* not valid ID */
            result = BCM_E_PARAM;
        }
        index = *elem - handle->low;
        if (index + count > handle->count) {
            /* not valid ID */
            result = BCM_E_PARAM;
        }
        if (BCM_E_NONE == result) {
            result = _sw_state_res_bitmap_check_all(unit, handle, count, index);
            switch (result) {
            case BCM_E_FULL:
                if (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE) {
                    result = BCM_E_NONE;
                } else {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("proposed block %p base %d count %d"
                               " already exists\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = BCM_E_RESOURCE;
                }
                break;
            case BCM_E_EMPTY:
                if (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("proposed block %p base %d count %d"
                               " does not exist\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = BCM_E_NOT_FOUND;
                } else {
                    result = BCM_E_NONE;
                }
                break;
            case BCM_E_EXISTS:
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("proposed block %p base %d count %d"
                           " would merge/expand existing block(s)\n"),
                           (void*)handle,
                           *elem,
                           count));
                result = BCM_E_RESOURCE;
                break;
            default:
                /* should never see this */
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("unexpected result checking proposed block:"
                           " %d (%s)\n"),
                           result,
                           _SHR_ERRMSG(result)));
                if (BCM_E_NONE == result) {
                    result = BCM_E_INTERNAL;
                }
            }
        }
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SW_STATE_RES_ALLOC_WITH_ID) */
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
        /* see if there are enough elements after last free */
        index = handle->lastFree;
        if (index + count < handle->count) {
            /* it might fit */
            for (offset = 0; offset < count; offset++) {
                if (SHR_BITGET(handle->data, index + offset)) {
                    result = BCM_E_EXISTS;
                    break;
                }
            }
        } else {
            result = BCM_E_EXISTS;
        }
        if (BCM_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            handle->lastFree = index + count;
        } else { /* if (BCM_E_NONE == result) */
            /* start searching after last successful alloc */
            index = handle->nextAlloc;
            while (index <= handle->count - count) {
                while (SHR_BITGET(handle->data, index) &&
                       (index > handle->count - count)) {
                    index++;
                }
                if (index <= handle->count - count) {
                    /* have a candidate; see if block is big enough */
                    result = BCM_E_NONE;
                    for (offset = 0; offset < count; offset++) {
                        if (SHR_BITGET(handle->data, index + offset)) {
                            /* not big enough; skip this block */
                            result = BCM_E_EXISTS;
                            index += offset + 1;
                            break;
                        }
                    } /* for (offset = 0; offset < count; offset++) */
                } /* if (index <= desc->count - count) */
                if (BCM_E_NONE == result) {
                    /* found a sufficient block */
                    break;
                }
            } /* while (index <= desc->count - count) */
            if (BCM_E_NONE != result) {
                /* no space, so try space before last successful alloc */
#else /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
                result = BCM_E_RESOURCE;
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
                index = 0;
                while (index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                    while ((index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) &&
                           SHR_BITGET(handle->data, index)) {
                        index++;
                    }
                    if (index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                        /* have a candidate; see if block is big enough */
                        result = BCM_E_NONE;
                        for (offset = 0; offset < count; offset++) {
                            if (SHR_BITGET(handle->data, index + offset)) {
                                /* not big enough; skip this block */
                                result = BCM_E_EXISTS;
                                index += offset + 1;
                                break;
                            }
                        } /* for (offset = 0; offset < count; offset++) */
                    } /* if (index < data->next_alloc) */
                    if (BCM_E_NONE == result) {
                        /* found a sufficient block */
                        break;
                    }
                } /* while (index < data->next_alloc) */
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
            } /* if (BCM_E_NONE != result) */
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
            if (BCM_E_NONE != result) {
                /* still no space; give up */
                result = BCM_E_RESOURCE;
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
            } else {
                /* got some space; update next alloc  */
                handle->nextAlloc = index + count;
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
            }
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
        } /* if (BCM_E_NONE == result) */
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
    } /* if (flags & SW_STATE_RES_ALLOC_WITH_ID) */
    if (BCM_E_NONE == result) {
        /* return the beginning element */
        *elem = index + handle->low;
        /* mark the block as in use */
        SHR_BITSET_RANGE(handle->data, index, count);
        if (0 == (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE)) {
            /* only adjust accounting if not replacing existing block */
            handle->used += count;
        }
    } /* if (BCM_E_NONE == result) */
    return result;
#else 
    return sw_state_res_tag_bitmap_alloc(unit, handle, flags, count, elem);
#endif
}

int
sw_state_res_bitmap_alloc_align(int unit,
                           sw_state_res_bitmap_handle_t handle,
                           uint32 flags,
                           int align,
                           int offs,
                           int count,
                           int *elem)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int index;
    int offset;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must allocate at least one element\n")));
        return BCM_E_PARAM;
    }
    if (SW_STATE_RES_BITMAP_ALLOC_REPLACE ==
        (flags & (SW_STATE_RES_BITMAP_ALLOC_REPLACE |
                  SW_STATE_RES_BITMAP_ALLOC_WITH_ID))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must use WITH_ID when using REPLACE\n")));
        return BCM_E_PARAM;
    }

    if (flags & SW_STATE_RES_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < handle->low) {
            /* not valid ID */
            result = BCM_E_PARAM;
        }
        index = *elem - handle->low;
        if (index + count > handle->count) {
            /* not valid ID */
            result = BCM_E_PARAM;
        }
        if (BCM_E_NONE == result) {
            /* make sure caller's request is valid */
            if (flags & SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO) {
                /* alignment is against zero */
                offset = (*elem) % align;
            } else {
                /* alignment is against low */
                offset = ((*elem) - handle->low) % align;
            }
            if (offset != offs) {
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("provided first element %d does not conform"
                           " to provided align %d + offset %d values"
                           " (actual offset = %d)\n"),
                           *elem,
                           align,
                           offset,
                           offs));
                result = BCM_E_PARAM;
            }
        }
        if (BCM_E_NONE == result) {
            result = _sw_state_res_bitmap_check_all(unit, handle, count, index);
            switch (result) {
            case BCM_E_FULL:
                if (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE) {
                    result = BCM_E_NONE;
                } else {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("proposed block %p base %d count %d"
                               " already exists\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = BCM_E_RESOURCE;
                }
                break;
            case BCM_E_EMPTY:
                if (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("proposed block %p base %d count %d"
                               " does not exist\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = BCM_E_NOT_FOUND;
                } else {
                    result = BCM_E_NONE;
                }
                break;
            case BCM_E_EXISTS:
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("proposed block %p base %d count %d"
                           " would merge/expand existing block(s)\n"),
                           (void*)handle,
                           *elem,
                           count));
                result = BCM_E_RESOURCE;
                break;
            default:
                /* should never see this */
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("unexpected result checking proposed block:"
                           " %d (%s)\n"),
                           result,
                           _SHR_ERRMSG(result)));
                if (BCM_E_NONE == result) {
                    result = BCM_E_INTERNAL;
                }
            }
        }
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SW_STATE_RES_ALLOC_WITH_ID) */
        if (flags & SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO) {
            /* alignment is against zero, not start of pool */
            offs += align - (handle->low % align);
        }
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
        /* see if there are enough elements after last free */
        index = (((handle->lastFree + align - 1) / align) * align) + offs;
        if (index + count < handle->count) {
            /* it might fit */
            for (offset = 0; offset < count; offset++) {
                if (SHR_BITGET(handle->data, index + offset)) {
                    result = BCM_E_EXISTS;
                    break;
                }
            }
        } else {
            result = BCM_E_EXISTS;
        }
        if (BCM_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            if (0 == offs) {
                handle->lastFree = index + count;
            }
        } else { /* if (BCM_E_NONE == result) */
            /* start searching after last successful alloc */
            index = (((handle->nextAlloc + align - 1) / align) * align) + offs;
            while (index <= handle->count - count) {
                while ((index <= handle->count - count) &&
                       SHR_BITGET(handle->data, index)) {
                    index += align;
                }
                if (index <= handle->count - count) {
                    /* have a candidate; see if block is big enough */
                    result = BCM_E_NONE;
                    for (offset = 0; offset < count; offset++) {
                        if (SHR_BITGET(handle->data, index + offset)) {
                            /* not big enough; skip this block */
                            result = BCM_E_EXISTS;
                            index = (((index + offset + align) / align) * align) + offs;
                            break;
                        }
                    } /* for (offset = 0; offset < count; offset++) */
                } /* if (index <= desc->count - count) */
                if (BCM_E_NONE == result) {
                    /* found a sufficient block */
                    break;
                }
            } /* while (index <= desc->count - count) */
            if (BCM_E_NONE != result) {
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
                /* no space, so try space before last successful alloc */
                index = offs;
                while (index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                    while ((index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) &&
                           SHR_BITGET(handle->data, index)) {
                        index += align;
                    }
                    if (index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                        /* have a candidate; see if block is big enough */
                        result = BCM_E_NONE;
                        for (offset = 0; offset < count; offset++) {
                            if (SHR_BITGET(handle->data, index + offset)) {
                                /* not big enough; skip this block */
                                result = BCM_E_EXISTS;
                                index = (((index + offset + align) / align) * align) + offs;
                                break;
                            }
                        } /* for (offset = 0; offset < count; offset++) */
                    } /* if (index < data->next_alloc) */
                    if (BCM_E_NONE == result) {
                        /* found a sufficient block */
                        break;
                    }
                } /* while (index < data->next_alloc) */
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
            } /* if (BCM_E_NONE != result) */
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
            if (BCM_E_NONE != result) {
                /* still no space; give up */
                result = BCM_E_RESOURCE;
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
            } else {
                /* got some space; update next alloc  */
                handle->nextAlloc = index + count;
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
            }
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
        } /* if (BCM_E_NONE == result) */
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
    } /* if (flags & SW_STATE_RES_ALLOC_WITH_ID) */
    if (BCM_E_NONE == result) {
        /* return the beginning element */
        *elem = index + handle->low;
        /* mark the block as in use */
        SHR_BITSET_RANGE(handle->data, index, count);
        if (0 == (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE)) {
            /* only adjust accounting if not replacing existing block */
            handle->used += count;
        }
    } /* if (BCM_E_NONE == result) */
    return result;
#else
    return sw_state_res_tag_bitmap_alloc_align(unit, handle, flags, align, offs, count, elem);
#endif
}

int
sw_state_res_bitmap_alloc_align_sparse(int unit,
                                  sw_state_res_bitmap_handle_t handle,
                                  uint32 flags,
                                  int align,
                                  int offs,
                                  uint32 pattern,
                                  int length,
                                  int repeats,
                                  int *elem)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int index;
    int offset;
    int repeat;
    int current;
    int first;
    int count;
    int result = BCM_E_NONE;
    uint32 pattern_mask;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return BCM_E_PARAM;
    }
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must be at least one long\n")));
        return BCM_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must not be longer than 32\n")));
        return BCM_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must allocate at least one pattern\n")));
        return BCM_E_PARAM;
    }
    pattern_mask = (((uint32)1 << (length - 1)) - 1) | ((uint32)1 << (length - 1));
    if (0 == (pattern & pattern_mask)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must contain at least one element\n")));
        return BCM_E_PARAM;
    }
    if (pattern & ~pattern_mask) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must not contain unused bits\n")));
        return BCM_E_PARAM;
    }
    if (SW_STATE_RES_BITMAP_ALLOC_REPLACE ==
        (flags & (SW_STATE_RES_BITMAP_ALLOC_REPLACE |
                  SW_STATE_RES_BITMAP_ALLOC_WITH_ID))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must use WITH_ID when using REPLACE\n")));
        return BCM_E_PARAM;
    }
    /* find the final set bit of the repeated pattern */
    index = length;
    count = 0;
    do {
        index--;
        if (pattern & (1 << index)) {
            count = index;
            break;
        }
    } while (index > 0);
    count += (length * (repeats - 1));
    /* find the first set bit of the repeated pattern */
    for (first = 0; first < length; first++) {
        if (pattern & (1 << first)) {
            break;
        }
    }

    if (flags & SW_STATE_RES_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < handle->low) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("first element is too low\n")));
            result = BCM_E_PARAM;
        }
        index = *elem - handle->low;
        if (index + count > handle->count) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                      (BSL_META("final element is too high\n")));
            result = BCM_E_PARAM;
        }
        if (BCM_E_NONE == result) {
            /* make sure caller's request is valid */
            if (flags & SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO) {
                /* alignment is against zero */
                offset = (*elem) % align;
            } else {
                /* alignment is against low */
                offset = ((*elem) - handle->low) % align;
            }
            if (offset != offs) {
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("provided first element %d does not conform"
                           " to provided align %d + offset %d values"
                           " (actual offset = %d)\n"),
                           *elem,
                           align,
                           offset,
                           offs));
                result = BCM_E_PARAM;
            }
        } /* if (BCM_E_NONE == result) */
        if (BCM_E_NONE == result) {
            result = _sw_state_res_bitmap_check_all_sparse(unit, 
                                                      handle,
                                                      pattern,
                                                      length,
                                                      repeats,
                                                      index);
            switch (result) {
            case BCM_E_FULL:
                if (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE) {
                    result = BCM_E_NONE;
                } else {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("proposed block %p base %d pattern %08X"
                               " length %d repeat %d already exists\n"),
                               (void*)handle,
                               *elem,
                               pattern,
                               length,
                               repeats));
                    result = BCM_E_RESOURCE;
                }
                break;
            case BCM_E_EMPTY:
                if (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("proposed block %p base %d pattern %08X"
                               " length %d repeat %d does not exist\n"),
                               (void*)handle,
                               *elem,
                               pattern,
                               length,
                               repeats));
                    result = BCM_E_NOT_FOUND;
                } else {
                    result = BCM_E_NONE;
                }
                break;
            case BCM_E_EXISTS:
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("proposed block %p base %d pattern %08X"
                           " length %d repeat %d would merge/expand"
                           " existing block(s)\n"),
                           (void*)handle,
                           *elem,
                           pattern,
                           length,
                           repeats));
                result = BCM_E_RESOURCE;
                break;
            default:
                /* should never see this */
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                          (BSL_META("unexpected result checking proposed block:"
                           " %d (%s)\n"),
                           result,
                           _SHR_ERRMSG(result)));
                if (BCM_E_NONE == result) {
                    result = BCM_E_INTERNAL;
                }
            } /* switch (result) */
        } /* if (BCM_E_NONE == result) */
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SW_STATE_RES_ALLOC_WITH_ID) */
        if (flags & SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO) {
            /* alignment is against zero, not start of pool */
            offs += align - (handle->low % align);
        }
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
        /* see if it fits after the last free */
        index = (((handle->lastFree + align - 1) / align) * align) + offs;
        if (index + count < handle->count) {
            /* it might fit */
            for (repeat = 0, current = index; repeat < repeats; repeat++) {
                for (offset = 0; offset < length; offset++, current++) {
                    if (pattern & (1 << offset)) {
                        if (SHR_BITGET(handle->data, current)) {
                            result = BCM_E_EXISTS;
                            break;
                        }
                    }
                }
            }
        } else {
            result = BCM_E_EXISTS;
        }
        if (BCM_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            if (0 == offs) {
                handle->lastFree = index + count;
            }
        } else { /* if (BCM_E_NONE == result) */
            /* start searching after last successful alloc */
            index = (((handle->nextAlloc + align - 1) / align) * align) + offs;
            while (index <= handle->count - count) {
                while ((index <= handle->count - count) &&
                       SHR_BITGET(handle->data, index + first)) {
                    index += align;
                }
                if (index <= handle->count - count) {
                    /* have a candidate; see if block is big enough */
                    result = BCM_E_NONE;
                    for (repeat = 0, current = index;
                         repeat < repeats;
                         repeat++) {
                        for (offset = 0; offset < length; offset++, current++) {
                            if (pattern & (1 << offset)) {
                                if (SHR_BITGET(handle->data, current)) {
                                    /* an element is in use */
                                    result = BCM_E_EXISTS;
                                    /* skip to next alignment point */
                                    index += align;
                                    /* start comparing again */
                                    break;
                                } /* if (this element is in use) */
                            } /* if (this element is in the pattern) */
                        } /* for (length of the pattern) */
                    } /* for (number of repetitions of the pattern) */
                } /* if (index <= desc->count - count) */
                if (BCM_E_NONE == result) {
                    /* found a sufficient block */
                    break;
                }
            } /* while (index <= desc->count - count) */
            if (BCM_E_NONE != result) {
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
                /* no space, so try space before last successful alloc */
                index = offs;
                while (index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                    while ((index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) &&
                           SHR_BITGET(handle->data, index + first)) {
                        index += align;
                    }
                    if (index < SW_STATE_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                        /* have a candidate; see if block is big enough */
                        result = BCM_E_NONE;
                        for (repeat = 0, current = index;
                             repeat < repeats;
                             repeat++) {
                            for (offset = 0;
                                 offset < length;
                                 offset++, current++) {
                                if (pattern & (1 << offset)) {
                                    if (SHR_BITGET(handle->data, current)) {
                                        /* an element is in use */
                                        result = BCM_E_EXISTS;
                                        /* skip to next alignment point */
                                        index += align;
                                        /* start comparing again */
                                        break;
                                    } /* if (this element is in use) */
                                } /* if (this element is in the pattern) */
                            } /* for (length of the pattern) */
                        } /* for (number of repetitions of the pattern) */
                    } /* if (index < end of possible space) */
                    if (BCM_E_NONE == result) {
                        /* found a sufficient block */
                        break;
                    }
                } /* while (index < data->next_alloc) */
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
            } /* if (BCM_E_NONE != result) */
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
            if (BCM_E_NONE != result) {
                /* still no space; give up */
                result = BCM_E_RESOURCE;
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
            } else {
                /* got some space; update next alloc  */
                handle->nextAlloc = index + count;
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
            }
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
        } /* if (BCM_E_NONE == result) */
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
    } /* if (flags & SW_STATE_RES_ALLOC_WITH_ID) */
    if (BCM_E_NONE == result) {
        /* return the beginning element */
        *elem = index + handle->low;
        /* mark the block as in use */
        for (repeat = 0, count = 0, current = index;
             repeat < repeats;
             repeat++) {
            for (offset = 0; offset < length; offset++, current++) {
                if (pattern & (1 << offset)) {
                    SHR_BITSET(handle->data, current);
                    count++;
                } /* if (this element is in the pattern) */
            } /* for (length of the pattern) */
        } /* for (number of repetitions of the pattern) */
        if (0 == (flags & SW_STATE_RES_BITMAP_ALLOC_REPLACE)) {
            /* only adjust accounting if not replacing existing block */
            handle->used += count;
        }
    } /* if (BCM_E_NONE == result) */
    return result;
#else
    return BCM_E_UNAVAIL; /* Not currently used. Add support seperately. */
#endif
}

int
sw_state_res_bitmap_free(int unit,
                    sw_state_res_bitmap_handle_t handle,
                    int count,
                    int elem)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int index;
    int offset;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must free at least one element\n")));
        return BCM_E_PARAM;
    }

    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        /* check whether the block is in use */
        for (offset = 0; offset < count; offset++) {
            if (!SHR_BITGET(handle->data, index + offset)) {
                /* not entirely in use */
                result = BCM_E_NOT_FOUND;
                break;
            }
        } /* for (offset = 0; offset < count; offset++) */
    } /* if (BCM_E_NONE == result) */
    if (BCM_E_NONE == result) {
        /* looks fine, so mark the block as free */
        SHR_BITCLR_RANGE(handle->data, index, count);
        handle->used -= count;
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
        handle->lastFree = index;
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
    } /* if (BCM_E_NONE == result) */
    /* return the result */
    return result;
#else
    return sw_state_res_tag_bitmap_free(unit, handle, count, elem);
#endif
}

int
sw_state_res_bitmap_free_sparse(int unit,
                           sw_state_res_bitmap_handle_t handle,
                           uint32 pattern,
                           int length,
                           int repeats,
                           int elem)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int index;
    int offset;
    int final;
    int result = BCM_E_NONE;
    uint32 pattern_mask;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("first element is too low\n")));
        result = BCM_E_PARAM;
    }
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must be at least one long\n")));
        return BCM_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must not be longer than 32\n")));
        return BCM_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must check at least one pattern\n")));
        return BCM_E_PARAM;
    }
    pattern_mask = (((uint32)1 << (length - 1)) - 1) | ((uint32)1 << (length - 1));
    if (0 == (pattern & pattern_mask)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must contain at least one element\n")));
        return BCM_E_PARAM;
    }
    if (pattern & ~pattern_mask) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must not contain unused bits\n")));
        return BCM_E_PARAM;
    }
    index = length;
    final = 0;
    do {
        index--;
        if (pattern & (1 << index)) {
            final = index;
            break;
        }
    } while (index > 0);
    final += (length * (repeats - 1));

    elem -= handle->low;
    if (elem + final > handle->count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("last element is too high\n")));
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        /* check whether the block is in use */
        result = _sw_state_res_bitmap_check_all_sparse(unit, 
                                                  handle,
                                                  pattern,
                                                  length,
                                                  repeats,
                                                  elem);
        if (BCM_E_FULL == result) {
            /* block is fully in use */
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
            handle->lastFree = elem;
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
            for (index = 0; index < repeats; index++) {
                for (offset = 0; offset < length; offset++, elem++) {
                    if (pattern & (1 << offset)) {
                        SHR_BITCLR(handle->data, elem);
                        handle->used--;
                    } /* if (this element is in the pattern) */
                } /* for (pattern length) */
            } /* for (all repeats) */
            result = BCM_E_NONE;
        } else { /* if (BCM_E_FULL == result) */
            /* not entirely in use */
            result = BCM_E_NOT_FOUND;
        } /* if (BCM_E_FULL == result) */
    } /* if (BCM_E_NONE == result) */
    /* return the result */
    return result;
#else 
    return BCM_E_UNAVAIL; /* Sparse allocation not yet supported. */
#endif
}

int
sw_state_res_bitmap_check(int unit,
                     sw_state_res_bitmap_handle_t handle,
                     int count,
                     int elem)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int index;
    int offset;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must check at least one element\n")));
        return BCM_E_PARAM;
    }

    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        /* check whether the block is in use */
        result = BCM_E_NOT_FOUND;
        for (offset = 0; offset < count; offset++) {
            if (SHR_BITGET(handle->data, index + offset)) {
                /* not entirely free */
                result = BCM_E_EXISTS;
                break;
            }
        } /* for (offset = 0; offset < count; offset++) */
    } /* if (BCM_E_NONE == result) */
    /* return the result */
    return result;
#else
    return sw_state_res_tag_bitmap_check(unit, handle, count, elem);
#endif
}

int
sw_state_res_bitmap_check_all(int unit,
                         sw_state_res_bitmap_handle_t handle,
                         int count,
                         int elem)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int index;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must check at least one element\n")));
        return BCM_E_PARAM;
    }

    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        result = _sw_state_res_bitmap_check_all(unit, handle, count, index);
    }
    /* return the result */
    return result;
#else
    return sw_state_res_tag_bitmap_check_all(unit, handle, count, elem);
#endif
}

int
sw_state_res_bitmap_check_all_sparse(int unit,
                                sw_state_res_bitmap_handle_t handle,
                                uint32 pattern,
                                int length,
                                int repeats,
                                int elem)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int index;
    int final;
    int result = BCM_E_NONE;
    uint32 pattern_mask;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("first element is too low\n")));
        result = BCM_E_PARAM;
    }
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must be at least one long\n")));
        return BCM_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must not be longer than 32\n")));
        return BCM_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must check at least one pattern\n")));
        return BCM_E_PARAM;
    }
    pattern_mask = (((uint32)1 << (length - 1)) - 1) | ((uint32)1 << (length - 1));
    if (0 == (pattern & pattern_mask)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must contain at least one element\n")));
        return BCM_E_PARAM;
    }
    if (pattern & ~pattern_mask) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("pattern must not contain unused bits\n")));
        return BCM_E_PARAM;
    }
    /* find the final set bit of the repeated pattern */
    index = length;
    final = 0;
    do {
        index--;
        if (pattern & (1 << index)) {
            final = index;
            break;
        }
    } while (index > 0);
    final += (length * (repeats - 1));

    elem -= handle->low;
    if (elem + final > handle->count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("last element is too high\n")));
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        result = _sw_state_res_bitmap_check_all_sparse(unit, 
                                                  handle,
                                                  pattern,
                                                  length,
                                                  repeats,
                                                  elem);
    }
    /* return the result */
    return result;
#else
    return BCM_E_UNAVAIL; /* Sparse allocation not yet supported. */
#endif
}

int
sw_state_res_bitmap_dump(int unit,
                    const sw_state_res_bitmap_handle_t handle)
{
#if !SW_STATE_RES_BITMAP_USE_TAG_BITMAP
    int result;
    int error = FALSE;
    int elemsUsed;
    int index;
    int offset;
    int elemOffset;
    int rowUse;

    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must provide non-NULL handle\n")));
        return BCM_E_PARAM;
    }
    LOG_CLI((BSL_META("sw_state_res_bitmap at %p:\n"), (const void*)handle));
    LOG_CLI((BSL_META("  lowest ID     = %08X\n"), handle->low));
    LOG_CLI((BSL_META("  element count = %08X\n"), handle->count));
    LOG_CLI((BSL_META("  used elements = %08X\n"), handle->used));
#if SW_STATE_RES_BITMAP_SEARCH_RESUME
    LOG_CLI((BSL_META("  last free     = %08X %s\n"),
             handle->lastFree,
             (error |= (handle->lastFree > handle->count))?"INVALID":""));
    LOG_CLI((BSL_META("  next alloc    = %08X %s\n"),
             handle->nextAlloc,
             (error |= (handle->nextAlloc > handle->count))?"INVALID":""));
#endif /* SW_STATE_RES_BITMAP_SEARCH_RESUME */
    LOG_CLI((BSL_META("  element map:\n")));
    LOG_CLI((BSL_META("    1st Elem (index)    State of elements (1 = used)\n")));
    LOG_CLI((BSL_META("    -------- --------   --------------------------------------------------\n")));
    elemsUsed = 0;
    for (index = 0; index < handle->count; /* increment in loop */) {
        LOG_CLI((BSL_META("    %08X %08X   "), index + handle->low, index));
        elemOffset = 0;
        rowUse = 0;
        for (offset = 0; offset < 48; offset++) {
            if ((16 == offset) || (32 == offset)) {
                LOG_CLI((BSL_META(" ")));
            }
            if (index < handle->count) {
                if (SHR_BITGET(handle->data, index + elemOffset)) {
                    LOG_CLI((BSL_META("1")));
                    rowUse++;
                } else {
                    LOG_CLI((BSL_META("0")));
                }
                index++;
            }
        }
        LOG_CLI((BSL_META("\n")));
        elemsUsed += rowUse;
    } /* for all grains */
    LOG_CLI((BSL_META("  counted elems = %08X %s\n"),
             elemsUsed,
             (error |= (elemsUsed != handle->used))?"INVALID":""));
    if (error) {
        LOG_CLI((BSL_META("bitmap %p appears to be corrupt\n"),
                 (void*)handle));
        result = BCM_E_INTERNAL;
    } else {
        result = BCM_E_NONE;
    }
    return result;
#else
    return sw_state_res_tag_bitmap_dump(unit, handle);
#endif
}


