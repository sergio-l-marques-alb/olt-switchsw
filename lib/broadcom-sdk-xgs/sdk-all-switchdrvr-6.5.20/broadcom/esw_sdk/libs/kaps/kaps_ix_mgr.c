/*******************************************************************************
 *
 * Copyright 2011-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kaps_ix_mgr.h"
#include "kaps_device_internal.h"
#include "kaps_ad.h"
#include "kaps_ad_internal.h"
#include "kaps_algo_common.h"
#include "kaps_algo_hw.h"
#include "kaps_fib_lsnmc.h"
#include "kaps_fib_lsnmc_hw.h"
#include "kaps_errors.h"
int32_t
kaps_ix_chunk_get_num_prefixes(
    void *lsn,
    struct kaps_ix_chunk *cur_chunk,
    int32_t * has_reserved_solt)
{
    kaps_lsn_mc *self = (kaps_lsn_mc *) lsn;

    *has_reserved_solt = 0;

    if (self->m_pSettings->m_isPerLpuGran)
    {
        kaps_lpm_lpu_brick *ixBrick = NULL;

        kaps_assert(self->m_ixInfo == NULL, "Invalid Ix info in the LSN ");

        ixBrick = self->m_lpuList;
        while (ixBrick)
        {
            if (ixBrick->m_ixInfo == cur_chunk)
                break;
            ixBrick = ixBrick->m_next_p;
        }

        if (!ixBrick && self->m_ixInfo_specialEntry == cur_chunk)
            return 1;

        kaps_sassert(ixBrick);
        if (ixBrick->m_hasReservedSlot)
            *has_reserved_solt = 1;
        return ixBrick->m_numPfx;
    }
    else
    {
        return self->m_nNumPrefixes;
    }
}

/**
 * @cond INTERNAL
 *
 * @addtogroup IX_MGR
 * @{
 *
 * @section IX_MGR Index Manager
 *
 * The Index Manager manages the index space. The index space consists of indexes between
 * a start index value and an end index value. For instance, the index manager is given the range
 * of indexes to manage (100, 1000) . When a request for 10 indexes comes, the index manager can
 * assign any set of contiguous indexes such as (100, 109) or (501, 510)  to satisfy the request.
 * Chunks are maintained in the index manager to store a contiguous range of indexes. These chunks are
 * of two types either - allocated chunks or free chunks.
 *
 * The data structures used for managing the memory consist of:
 *
 * 1. A doubly linked list called the neighbor list consisting of ALL chunks that are present in the index space.
 * The chunks include allocated and  free chunks. The chunks are maintained in the ascending order in the linked list
 * So if two chunks are adjacent to each other in the index space then they will be
 * neighbors in the linked list
 *
 *
 * 2. An array of free lists - There is an array of pointers maintained where each member of the array points to a doubly
 * linked list of index space chunks. array[0] points to the doubly linked list that has all the free chunks of size 1.
 * array[i] points to the doubly linked list that has all the free chunks of size i+1. However the last member
 * of the array, that is array[MAX_NUM_CHUNK_LEVELS - 1] points to the doubly linked list that has free chunks of
 * size greater than or equal to MAX_NUM_CHUNK_LEVELS.
 *
 *
 * Note that the same chunk can be a member of the neighbor list and the free list
 *
 *
 * <hr>
 * @}
 *
 * @endcond
 */

int32_t
kaps_ix_mgr_ix_to_sb(
    struct kaps_ix_mgr * mgr,
    uint32_t ix)
{
    uint32_t i;

    for (i = 0; i < KAPS_HW_MAX_UDA_SB; ++i)
    {
        if (mgr->uda_translate[i] <= ix && ix <= (mgr->uda_translate[i] + mgr->num_entries_per_sb - 1))
        {
            return i;
        }
    }

    return -1;
}

static uint32_t
kaps_optimized_ix_to_sb(
    struct kaps_ix_mgr *mgr,
    uint32_t guess_sb,
    uint32_t rqt_ix)
{
    uint32_t found;
    int32_t sb_no;

    /*
     * Try to find the rqt index from the guess super block onwards to the last super block
     */
    found = 0;
    sb_no = guess_sb;
    while (sb_no < KAPS_HW_MAX_UDA_SB)
    {
        if (mgr->uda_translate[sb_no] == -1)
        {
            ++sb_no;
            continue;
        }

        if (mgr->uda_translate[sb_no] <= rqt_ix && rqt_ix < mgr->uda_translate[sb_no] + mgr->num_entries_per_sb)
        {
            found = 1;
            break;
        }

        ++sb_no;
    }

    /*
     * If we didn't find the rqt index, then call the im_mgr_ix_to_sb to start searching from SB-0
     */
    if (!found)
    {
        sb_no = kaps_ix_mgr_ix_to_sb(mgr, rqt_ix);
    }

    if (sb_no == -1 || sb_no >= KAPS_HW_MAX_UDA_SB)
    {
        kaps_assert(0, "Unable to find the SB for IX");
        return 0;
    }

    return (uint32_t) sb_no;
}

uint32_t
kaps_is_chunk_in_continuous_sb(
    struct kaps_ix_mgr * mgr,
    struct kaps_ix_chunk * cur_chunk)
{
    uint32_t i;
    static uint32_t start_ix_sb = 0;
    uint32_t end_ix_sb;
    struct kaps_device *device = NULL;
    uint32_t is_continuous;

    if (mgr->main_ad_db)
    {
        device = mgr->main_ad_db->db_info.device;
        if (0 && device->main_bc_device)
            device = device->main_bc_device;
    }

    /*
     * For optimization reasons, start_ix_sb is static so that we can continue from the start_ix_sb in the previous
     * function call
     */
    /*
     * Find the SB of the start_ix and the SB of the end_ix of the current chunk
     */
    start_ix_sb = kaps_optimized_ix_to_sb(mgr, start_ix_sb, cur_chunk->start_ix);
    end_ix_sb = kaps_optimized_ix_to_sb(mgr, start_ix_sb, cur_chunk->start_ix + cur_chunk->size - 1);

    is_continuous = 1;

    /*
     * Start from start_ix_sb + 1 onwards till end_ix_sb and break out if we the SBs are discontinuous
     */
    i = start_ix_sb + 1;
    while (i <= end_ix_sb)
    {
        if (mgr->uda_translate[i] == -1)
        {
            is_continuous = 0;
            break;
        }

        if (mgr->uda_translate[i - 1] + mgr->num_entries_per_sb != mgr->uda_translate[i])
        {
            is_continuous = 0;
            break;
        }

        ++i;
    }

    start_ix_sb = end_ix_sb;

    return is_continuous;
}

static uint32_t
kaps_check_ad_grow(
    struct kaps_ix_mgr *mgr,
    uint32_t chunk_loc_1,
    uint32_t chunk_loc_2,
    uint32_t * uda_translate,
    uint32_t num_entries_per_sb)
{
    uint32_t sb_no_1 = 0, sb_no_2 = 0;
    uint32_t sb_start_loc, sb_end_loc;
    int32_t i;
    struct kaps_device *device = NULL;

    if (mgr->main_ad_db)
    {
        device = mgr->main_ad_db->db_info.device;
        if (0 && device->main_bc_device)
            device = device->main_bc_device;
    }

    for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
    {
        if (uda_translate[i] == -1)
            continue;

        sb_start_loc = uda_translate[i];
        sb_end_loc = uda_translate[i] + num_entries_per_sb - 1;
        if (sb_start_loc <= chunk_loc_1 && chunk_loc_1 <= sb_end_loc)
        {
            sb_no_1 = i;
            break;
        }

    }

    for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
    {
        if (uda_translate[i] == -1)
            continue;
        sb_start_loc = uda_translate[i];
        sb_end_loc = uda_translate[i] + num_entries_per_sb - 1;
        if (sb_start_loc <= chunk_loc_2 && chunk_loc_2 <= sb_end_loc)
        {
            sb_no_2 = i;
            break;
        }
    }

    if (sb_no_1 == sb_no_2)
    {
        return 1;
    }

    if (sb_no_2 == (sb_no_1 + 1))
    {
        return 1;
    }

    return 0;

}

static uint32_t
kaps_ad_get_sb_for_chunk(
    struct kaps_ix_mgr *mgr,
    uint32_t start_loc,
    uint32_t size,
    uint32_t rqt_size,
    uint32_t * uda_translate,
    uint32_t num_entries_per_sb)
{
    int32_t i;
    uint32_t sb_end_loc;
    struct kaps_device *device = NULL;

    if (mgr->main_ad_db)
    {
        device = mgr->main_ad_db->db_info.device;
        if (0 && device->main_bc_device)
            device = device->main_bc_device;
    }

    /*
     * Find the USB where the AD request should be allocated 
     */
    for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
    {
        if (uda_translate[i] == -1)
            continue;
        sb_end_loc = uda_translate[i] + num_entries_per_sb - 1;
        if (start_loc > sb_end_loc)
            continue;

        if (start_loc >= uda_translate[i])
        {
            /*
             * Request can be entertained in current USB
             */
            if ((start_loc + rqt_size - 1) <= sb_end_loc)
                return i;
            /*
             * Request can be entertained in an overlap area of back to back USBs
             */
            if (((i + 1) < KAPS_HW_MAX_UDA_SB) && (uda_translate[i + 1] == sb_end_loc + 1))
            {
                return i;
            }

        }

    }
    return -1;
}

static uint32_t
kaps_ad_get_chunk(
    struct kaps_ix_mgr *mgr,
    struct kaps_ad_db *ad_db,
    uint32_t rqt_size,
    uint32_t level,
    struct kaps_ix_chunk **chunk)
{
    struct kaps_ix_chunk *temp = NULL;
    uint32_t num_entries_per_sb = (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / ad_db->db_info.width.ad_width_1;
    uint32_t sb_no = -1;

    temp = mgr->free_list[level];

    /*
     * Look for possible chunks in the current level 
     */
    while (temp)
    {
        kaps_sassert(temp->size >= rqt_size);
        sb_no =
            kaps_ad_get_sb_for_chunk(mgr, temp->start_ix, temp->size, rqt_size, mgr->uda_translate, num_entries_per_sb);
        if (sb_no != -1)
        {
            *chunk = temp;
            return sb_no;
        }
        temp = temp->next_free_chunk;
    }

    return -1;
}

static uint32_t
kaps_ix_mgr_get_seg_no(
    struct kaps_ix_mgr *mgr,
    uint32_t start_ix)
{
    uint32_t ix_seg_no;

    for (ix_seg_no = 0; ix_seg_no < KAPS_HW_MAX_UDA_SB; ix_seg_no++)
    {
        if (mgr->ix_segments[ix_seg_no] == -1)
            continue;
        if (mgr->ix_segments[ix_seg_no] <= start_ix)
        {
            if (mgr->ix_segments_end[ix_seg_no] >= start_ix)
            {
                break;
            }
        }
    }
    return ix_seg_no;
}

static struct kaps_ix_chunk *
kaps_merge_lists(
    struct kaps_ix_chunk *ptr1,
    struct kaps_ix_chunk *ptr2)
{
    struct kaps_ix_chunk *head, *tmp;

    if (!ptr1)
        return ptr2;
    else if (!ptr2)
        return ptr1;

    if (ptr1->start_ix > ptr2->start_ix)
    {
        tmp = ptr1;
        ptr1 = ptr2;
        ptr2 = tmp;
    }
    head = ptr1;

    while (ptr1 && ptr2)
    {
        if (ptr1->next_neighbor)
        {
            if (ptr1->next_neighbor->start_ix > ptr2->start_ix)
            {
                tmp = ptr2->next_neighbor;
                ptr2->next_neighbor = ptr1->next_neighbor;
                ptr1->next_neighbor = ptr2;
                ptr2 = tmp;
                ptr1 = ptr1->next_neighbor;
            }
            else
            {
                ptr1 = ptr1->next_neighbor;
            }
        }
        else
        {
            ptr1->next_neighbor = ptr2;
            break;
        }
    }

    return head;
}

static struct kaps_ix_chunk *
kaps_divide_and_sort(
    struct kaps_ix_chunk *ptr,
    uint32_t num_elements)
{
    struct kaps_ix_chunk *first_list_head;
    struct kaps_ix_chunk *second_list_head;
    uint32_t first_list_count, second_list_count;
    uint32_t i;

    if (num_elements <= 1)
    {
        if (ptr)
        {
            ptr->next_neighbor = NULL;
        }
        return ptr;
    }

    first_list_head = ptr;
    first_list_count = num_elements / 2;

    for (i = 0; i < first_list_count; ++i)
    {
        ptr = ptr->next_neighbor;
    }

    second_list_head = ptr;
    second_list_count = num_elements - first_list_count;

    first_list_head = kaps_divide_and_sort(first_list_head, first_list_count);
    second_list_head = kaps_divide_and_sort(second_list_head, second_list_count);

    return kaps_merge_lists(first_list_head, second_list_head);

}

static struct kaps_ix_chunk *
kaps_merge_sort(
    struct kaps_ix_chunk *list)
{
    struct kaps_ix_chunk *cur_node = list;
    uint32_t count = 0;

    /*
     * Count Elements 
     */
    while (cur_node)
    {
        count++;
        cur_node = cur_node->next_neighbor;
    }

    /*
     * Sort the list 
     */
    list = kaps_divide_and_sort(list, count);
    cur_node = list;
    cur_node->prev_neighbor = NULL;

    while (cur_node->next_neighbor)
    {
        cur_node->next_neighbor->prev_neighbor = cur_node;
        cur_node = cur_node->next_neighbor;
    }

    while (cur_node->prev_neighbor)
    {
        cur_node = cur_node->prev_neighbor;
    }
/*    list = cur_node;*/

    return list;
}

static void
kaps_handle_error(
    void)
{
    kaps_sassert(0);
}

static void
kaps_ix_mgr_remove_from_free_list(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_chunk)
{
    uint32_t level = cur_chunk->size - 1;

    if (level >= mgr->max_num_chunk_levels)
        level = mgr->max_num_chunk_levels - 1;

    if (cur_chunk->prev_free_chunk)
    {
        cur_chunk->prev_free_chunk->next_free_chunk = cur_chunk->next_free_chunk;
    }
    else
    {
        mgr->free_list[level] = cur_chunk->next_free_chunk;
    }

    mgr->num_ix_in_free_list[level] -= cur_chunk->size;

    if (cur_chunk->next_free_chunk)
    {
        cur_chunk->next_free_chunk->prev_free_chunk = cur_chunk->prev_free_chunk;
    }
}

static void
kaps_ix_mgr_add_to_free_list(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_chunk)
{
    uint32_t level = cur_chunk->size - 1;

    if (level >= mgr->max_num_chunk_levels)
        level = mgr->max_num_chunk_levels - 1;

    cur_chunk->prev_free_chunk = NULL;
    cur_chunk->next_free_chunk = mgr->free_list[level];

    mgr->free_list[level] = cur_chunk;
    mgr->num_ix_in_free_list[level] += cur_chunk->size;

    if (cur_chunk->next_free_chunk)
        cur_chunk->next_free_chunk->prev_free_chunk = cur_chunk;

}

static void
kaps_ix_mgr_remove_from_neighbor_list(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_chunk)
{
    if (cur_chunk->prev_neighbor)
    {
        cur_chunk->prev_neighbor->next_neighbor = cur_chunk->next_neighbor;
    }
    else
    {
        mgr->neighbor_list = cur_chunk->next_neighbor;
    }

    if (cur_chunk == mgr->neighbor_list_end)
    {
        mgr->neighbor_list_end = cur_chunk->prev_neighbor;
    }

    if (cur_chunk->next_neighbor)
    {
        cur_chunk->next_neighbor->prev_neighbor = cur_chunk->prev_neighbor;
    }
}

static void
kaps_ix_mgr_add_to_neighbor_list(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *prev_chunk,
    struct kaps_ix_chunk *cur_chunk)
{
    cur_chunk->prev_neighbor = prev_chunk;

    if (!prev_chunk)
    {
        cur_chunk->next_neighbor = mgr->neighbor_list;
        mgr->neighbor_list = cur_chunk;
    }
    else
    {
        cur_chunk->next_neighbor = prev_chunk->next_neighbor;
        prev_chunk->next_neighbor = cur_chunk;
    }

    if (prev_chunk == mgr->neighbor_list_end)
    {
        mgr->neighbor_list_end = cur_chunk;
    }

    if (cur_chunk->next_neighbor)
        cur_chunk->next_neighbor->prev_neighbor = cur_chunk;

}

static void
kaps_ix_mgr_add_to_cb_list(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_chunk,
    struct kaps_ix_chunk **cb_head_pp,
    struct kaps_ix_chunk **cb_tail_pp)
{
    struct kaps_ix_chunk *cb_head = *cb_head_pp, *cb_tail = *cb_tail_pp;

    cur_chunk->next_neighbor = NULL;

    if (cb_head)
    {
        cb_tail->next_neighbor = cur_chunk;
        *cb_tail_pp = cur_chunk;
    }
    else
    {
        *cb_head_pp = cur_chunk;
        *cb_tail_pp = cur_chunk;
    }
}

static void
kaps_ix_mgr_remove_from_cb_list(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *cur_chunk,
    struct kaps_ix_chunk **cb_head_pp,
    struct kaps_ix_chunk **cb_tail_pp)
{
    struct kaps_ix_chunk *cb_head = *cb_head_pp;

    kaps_sassert(cur_chunk == cb_head);

    *cb_head_pp = cur_chunk->next_neighbor;

    if (!(*cb_head_pp))
        *cb_tail_pp = NULL;

    cur_chunk->next_neighbor = NULL;
}

static void
kaps_ix_mgr_add_to_playback_list(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_compaction_chunk *playback_chunk,
    struct kaps_ix_compaction_chunk **playback_head_pp,
    struct kaps_ix_compaction_chunk **playback_tail_pp)
{
    struct kaps_ix_compaction_chunk *playback_head = *playback_head_pp, *playback_tail = *playback_tail_pp;

    playback_chunk->next = NULL;

    if (playback_head)
    {
        playback_tail->next = playback_chunk;
        *playback_tail_pp = playback_chunk;
    }
    else
    {
        *playback_head_pp = playback_chunk;
        *playback_tail_pp = playback_chunk;
    }
}

/*Given the start_ix, find the longest possible end ix that is continuous*/
void
kaps_find_longest_continuous_chunk(
    struct kaps_ix_mgr *mgr,
    uint32_t cur_chunk_start_ix,
    uint32_t final_end_ix,
    uint32_t * cur_chunk_end_ix_p)
{
    struct kaps_device *device = NULL;
    uint32_t i;
    static uint32_t cur_sb = 0;

    if (mgr->main_ad_db)
    {
        device = mgr->main_ad_db->db_info.device;
        if (0 && device->main_bc_device)
            device = device->main_bc_device;
    }

    *cur_chunk_end_ix_p = 0;

    cur_sb = kaps_optimized_ix_to_sb(mgr, cur_sb, cur_chunk_start_ix);

    /*
     * final_end_ix is the end ix of the original old chunk. If the final_end_ix is within cur_sb, then return
     */
    if (final_end_ix < mgr->uda_translate[cur_sb] + mgr->num_entries_per_sb)
    {
        *cur_chunk_end_ix_p = final_end_ix;
        return;
    }

    /*
     * update cur_chunk_end_ix_p to the end of the current super block
     */
    *cur_chunk_end_ix_p = mgr->uda_translate[cur_sb] + mgr->num_entries_per_sb - 1;

    i = cur_sb + 1;
    while (i < KAPS_HW_MAX_UDA_SB)
    {
        if (mgr->uda_translate[i] == -1)
        {
            break;
        }

        if (mgr->uda_translate[i - 1] + mgr->num_entries_per_sb != mgr->uda_translate[i])
        {
            break;
        }

        /*
         * final_end_ix is the end ix of the original old chunk. If the final_end_ix is within this super block, then
         * return
         */
        if (final_end_ix < mgr->uda_translate[i] + mgr->num_entries_per_sb)
        {
            *cur_chunk_end_ix_p = final_end_ix;
            break;
        }

        /*
         * update cur_chunk_end_ix_p to the end of this super block
         */
        *cur_chunk_end_ix_p = mgr->uda_translate[i] + mgr->num_entries_per_sb - 1;

        ++i;
    }
}

/*If a free chunk spans across discontinuous SBs, then split the free chunk*/
kaps_status
kaps_ix_mgr_split_free_chunk(
    struct kaps_ix_mgr *mgr,
    struct kaps_ix_chunk *old_free_chunk)
{
    struct kaps_ix_chunk *prev_free_chunk, *next_free_chunk;
    uint32_t num_chunks;
    uint32_t is_old_chunk_at_end = 0;
    uint32_t old_chunk_start_ix, old_chunk_end_ix;
    uint32_t cur_chunk_start_ix, cur_chunk_end_ix;

    prev_free_chunk = old_free_chunk->prev_neighbor;
    if (old_free_chunk->next_neighbor == NULL)
    {
        is_old_chunk_at_end = 1;        /* This is needed for updating the neighbor_list_end */
    }

    old_chunk_start_ix = old_free_chunk->start_ix;
    old_chunk_end_ix = old_free_chunk->start_ix + old_free_chunk->size - 1;

    /*
     * Remove the old free chunk from the neighbor list and free list
     */
    kaps_ix_mgr_remove_from_neighbor_list(mgr, old_free_chunk);
    kaps_ix_mgr_remove_from_free_list(mgr, old_free_chunk);

    num_chunks = 0;
    cur_chunk_start_ix = old_chunk_start_ix;

    while (cur_chunk_start_ix <= old_chunk_end_ix)
    {

        /*
         * Find the end ix of longest chunk that starts at cur_chunk_start_ix and is continuous
         */
        cur_chunk_end_ix = 0;
        kaps_find_longest_continuous_chunk(mgr, cur_chunk_start_ix, old_chunk_end_ix, &cur_chunk_end_ix);

        /*
         * Create a new chunk
         */
        POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, next_free_chunk);
        if (!next_free_chunk)
            return KAPS_OUT_OF_MEMORY;

        kaps_memset(next_free_chunk, 0, sizeof(*next_free_chunk));

        next_free_chunk->start_ix = cur_chunk_start_ix;
        next_free_chunk->size = cur_chunk_end_ix - cur_chunk_start_ix + 1;

        next_free_chunk->type = IX_FREE_CHUNK;

        kaps_ix_mgr_add_to_free_list(mgr, next_free_chunk);
        kaps_ix_mgr_add_to_neighbor_list(mgr, prev_free_chunk, next_free_chunk);

        if (is_old_chunk_at_end)
            mgr->neighbor_list_end = next_free_chunk;

        ++num_chunks;

        prev_free_chunk = next_free_chunk;

        cur_chunk_start_ix = cur_chunk_end_ix + 1;

    }

    /*
     * Free the old free chunk that has been split
     */
    POOL_FREE(kaps_ix_chunk, &mgr->ix_chunk_pool, old_free_chunk);

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_normalize_free_chunks(
    struct kaps_ix_mgr * mgr)
{
    struct kaps_ix_chunk *cur_chunk, *next_chunk;

    cur_chunk = mgr->neighbor_list;

    /*
     * During kaps_ix_mgr_init and during warmboot, the free chunks in discontinuous SBs are merged. So we want to now
     * split the free chunks so that each free chunk resides only in continuous super blocks
     */

    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            if (!kaps_is_chunk_in_continuous_sb(mgr, cur_chunk))
            {
                KAPS_STRY(kaps_ix_mgr_split_free_chunk(mgr, cur_chunk));
            }
        }

        cur_chunk = next_chunk;
    }

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_init(
    struct kaps_device * device,
    uint32_t start_ix,
    uint32_t end_ix,
    uint32_t max_alloc_chunk_size,
    struct kaps_ix_mgr ** mgr_pp,
    struct kaps_ad_db * ad_db,
    uint8_t is_warmboot)
{
    struct kaps_ix_mgr *mgr = NULL;
    struct kaps_ix_chunk *first_free_chunk = NULL;
    uint32_t max_num_chunk_levels = 0;
    int32_t cur_start_ix = start_ix;
    int32_t cur_end_ix = end_ix;
    uint32_t num_entries_per_sb = 0;
    int32_t i;
    uint32_t is_dynamic_ix;
    uint32_t alloc_cb;
    struct kaps_db *algo_db = NULL;

    if (start_ix > end_ix)
    {
        kaps_sassert(0);
        return KAPS_INVALID_ARGUMENT;
    }

    if (ad_db && ad_db->db_info.hw_res.ad_res->is_1_1 == 0)
    {
        cur_end_ix = start_ix - 1;
    }

    if (ad_db && ad_db->db_info.hw_res.ad_res->is_1_1 == 1)
    {
        num_entries_per_sb = (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / ad_db->db_info.width.ad_width_1;
    }
    else
    {
        num_entries_per_sb = 64 * 1024;
    }

    if (ad_db && (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INPLACE || ad_db->user_width_1 == 0))
    {
        int ad_capacity = ad_db->db_info.common_info->capacity;

        if (ad_capacity == 0)
            ad_capacity = 512 * 1024;

        /*
         * Dummy Index Space for INplace AD
         */
        cur_start_ix = start_ix;
        cur_end_ix = start_ix + (ad_db->db_info.common_info->capacity * 2) - 1;
    }

    if (mgr_pp)
        *mgr_pp = NULL;

    /*
     * Make the number of levels to be nearest power of 2 that is <= than max_ix_chunk_size This should work for small
     * max_ix_chunk_size values 
     */
    max_num_chunk_levels = 1;
    while (max_num_chunk_levels < max_alloc_chunk_size)
    {
        max_num_chunk_levels = max_num_chunk_levels << 1;
    }

    if (is_warmboot)
        max_num_chunk_levels = max_alloc_chunk_size;

    mgr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_ix_mgr));
    if (!mgr)
        return KAPS_OUT_OF_MEMORY;

    mgr->free_list =
        device->alloc->xcalloc(device->alloc->cookie, max_num_chunk_levels, sizeof(struct kaps_ix_chunk *));
    if (!mgr->free_list)
    {
        kaps_ix_mgr_destroy(mgr);
        return KAPS_OUT_OF_MEMORY;
    }

    mgr->num_ix_in_free_list = device->alloc->xcalloc(device->alloc->cookie, max_num_chunk_levels, sizeof(uint32_t));
    if (!mgr->num_ix_in_free_list)
    {
        kaps_ix_mgr_destroy(mgr);
        return KAPS_OUT_OF_MEMORY;
    }

    POOL_INIT(kaps_ix_chunk, &mgr->ix_chunk_pool, device->alloc);

    mgr->start_ix = cur_start_ix;
    mgr->end_ix = cur_end_ix;
    mgr->size = cur_end_ix - cur_start_ix + 1;

    is_dynamic_ix = 0;
    if (ad_db && ad_db->db_info.common_info->ad_info.db->common_info->enable_dynamic_allocation)
        is_dynamic_ix = 1;

    alloc_cb = 0;
    if (is_dynamic_ix)
    {
        if (mgr->size > (2 * max_alloc_chunk_size))
        {
            alloc_cb = 1;
        }
    }
    else
    {
        if (mgr->size > KAPS_MAX_IX_INVOLVED_IN_COMPACTION)
        {
            alloc_cb = 1;
        }
    }

    if (alloc_cb)
    {
        mgr->cb_start_ix = cur_start_ix;
        mgr->cb_end_ix = mgr->cb_start_ix + (2 * max_alloc_chunk_size) - 1;
        mgr->cb_size = mgr->cb_end_ix - mgr->cb_start_ix + 1;

        cur_start_ix += 2 * max_alloc_chunk_size;
        mgr->start_ix = cur_start_ix;
    }

    mgr->num_allocated_ix = 0;
    mgr->max_alloc_chunk_size = max_alloc_chunk_size;
    mgr->max_num_chunk_levels = max_num_chunk_levels;
    mgr->device = device;
    mgr->num_entries_per_sb = num_entries_per_sb;

    if (!is_warmboot && (cur_end_ix - cur_start_ix + 1))
    {

        POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, first_free_chunk);
        if (!first_free_chunk)
            return KAPS_OUT_OF_MEMORY;
        kaps_memset(first_free_chunk, 0, sizeof(*first_free_chunk));

        first_free_chunk->start_ix = cur_start_ix;
        first_free_chunk->size = cur_end_ix - cur_start_ix + 1;
        first_free_chunk->type = IX_FREE_CHUNK;

        kaps_ix_mgr_add_to_free_list(mgr, first_free_chunk);
        mgr->neighbor_list = first_free_chunk;
        mgr->neighbor_list_end = first_free_chunk;
    }

    if (ad_db)
    {

        kaps_sassert(ad_db->mgr[device->core_id] == NULL);
        ad_db->mgr[device->core_id] = mgr;

        algo_db = (struct kaps_db *) ad_db->db_info.common_info->ad_info.db;
        if (algo_db->common_ad_owner_db)
        {
            struct kaps_ad_db *common_ad_db = (struct kaps_ad_db *) algo_db->common_info->ad_info.ad;

            while (common_ad_db)
            {
                if (common_ad_db->mgr[device->core_id] == ad_db->mgr[device->core_id])
                    break;
                common_ad_db = common_ad_db->next;
            }
            kaps_sassert(common_ad_db);
            mgr->main_ad_db = ad_db;
        }
        else
        {
            mgr->main_ad_db = ad_db;
        }

        if (ad_db->next)
            KAPS_STRY(kaps_ix_mgr_init
                      (device, cur_end_ix + 1, end_ix, max_alloc_chunk_size, &mgr->next, ad_db->next, is_warmboot));
    }

    /*
     * Initialize the ix_segments and assign the first segment 
     */
    for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
        mgr->ix_segments[i] = -1;

    mgr->ix_segments[0] = mgr->start_ix - mgr->cb_size;
    mgr->ix_segments_end[0] = mgr->end_ix;

    if (mgr_pp)
    {
        *mgr_pp = mgr;
        (*mgr_pp)->ref_count++;
    }

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_expand(
    struct kaps_ix_mgr * mgr,
    uint32_t * uda_translate,
    int32_t sb_no,
    struct kaps_ad_db * ad_db,
    uint32_t is_warmboot)
{
    struct kaps_ix_mgr *cur_mgr = mgr;
    struct kaps_ix_chunk *expanded_free_chunk = NULL;
    int32_t num_entries_per_sb;
    int32_t end_ix = -1;
    int32_t first_chunk = 0;
    int32_t i;
    int32_t empty_ix_segment;
    int32_t is_fused;

    if (!is_warmboot)
    {
        POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, expanded_free_chunk);
        if (!expanded_free_chunk)
            return KAPS_OUT_OF_MEMORY;
        kaps_memset(expanded_free_chunk, 0, sizeof(*expanded_free_chunk));
    }

    while (mgr)
    {
        if (end_ix < mgr->end_ix)
            end_ix = mgr->end_ix;
        mgr = mgr->next;
    }

    kaps_sassert(ad_db);
    cur_mgr = ad_db->mgr[ad_db->db_info.device->core_id];
    kaps_sassert(cur_mgr);

    num_entries_per_sb = (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / ad_db->db_info.width.ad_width_1;

    if (cur_mgr->size == 0)
    {
        cur_mgr->start_ix = end_ix + 1;
        cur_mgr->end_ix = end_ix;

        cur_mgr->cb_start_ix = cur_mgr->start_ix;
        cur_mgr->cb_end_ix = cur_mgr->cb_start_ix + (2 * cur_mgr->max_alloc_chunk_size) - 1;
        cur_mgr->cb_size = cur_mgr->cb_end_ix - cur_mgr->cb_start_ix + 1;

        cur_mgr->start_ix += (2 * cur_mgr->max_alloc_chunk_size);
        cur_mgr->end_ix += num_entries_per_sb;
        cur_mgr->size += num_entries_per_sb;

        /*
         * Initialize the ix_segments and assign the first segment 
         */
        for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
            cur_mgr->ix_segments[i] = -1;

        cur_mgr->ix_segments[0] = cur_mgr->start_ix - cur_mgr->cb_size;
        cur_mgr->ix_segments_end[0] = cur_mgr->end_ix;
        if (!is_warmboot)
        {
            expanded_free_chunk->start_ix = cur_mgr->start_ix;
            expanded_free_chunk->size = num_entries_per_sb - 2 * cur_mgr->max_alloc_chunk_size;
        }
        else
        {
            *uda_translate = cur_mgr->start_ix - cur_mgr->cb_size;
        }
        first_chunk = 1;
    }
    else
    {
        cur_mgr->end_ix = end_ix + num_entries_per_sb;
        kaps_sassert(cur_mgr->end_ix <= (1 << 25));     /* it should actually be much less than 32m */
        cur_mgr->size += num_entries_per_sb;

        /*
         * Try to merge with existing segment, else create new segment 
         */
        empty_ix_segment = -1;
        is_fused = 0;   /* Indicates if we have merged the IX segments or not */
        for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
        {
            if (cur_mgr->ix_segments[i] == -1)
            {
                if (empty_ix_segment == -1)
                {
                    empty_ix_segment = i;
                }
            }
            else
            {
                if (cur_mgr->ix_segments_end[i] == end_ix)
                {
                    cur_mgr->ix_segments_end[i] = end_ix + num_entries_per_sb;
                    empty_ix_segment = -1;
                    is_fused = 1;
                    break;
                }
            }
        }

        if (empty_ix_segment == -1)
        {
            kaps_assert(is_fused, "Problem with IX expansion \n");
        }
        else
        {
            cur_mgr->ix_segments[empty_ix_segment] = end_ix + 1;
            cur_mgr->ix_segments_end[empty_ix_segment] = cur_mgr->end_ix;
        }

        if (!is_warmboot)
        {
            expanded_free_chunk->start_ix = end_ix + 1;
            expanded_free_chunk->size = num_entries_per_sb;
        }
        else
        {
            *uda_translate = end_ix + 1;
        }
    }

    if (!is_warmboot)
    {
        expanded_free_chunk->type = IX_FREE_CHUNK;
        *uda_translate = expanded_free_chunk->start_ix;
        if (first_chunk)
            *uda_translate -= cur_mgr->cb_size;

        kaps_sassert(cur_mgr->ix_base_to_sb[*uda_translate / num_entries_per_sb] == -1);

        cur_mgr->ix_base_to_sb[*uda_translate / num_entries_per_sb] = sb_no;

        kaps_ix_mgr_add_to_free_list(cur_mgr, expanded_free_chunk);
        kaps_ix_mgr_add_to_neighbor_list(cur_mgr, cur_mgr->neighbor_list_end, expanded_free_chunk);
    }

    return KAPS_OK;
}

kaps_status
kaps_kaps_ix_mgr_expand_dummy_ix(
    struct kaps_ix_mgr * mgr,
    struct kaps_ad_db * ad_db,
    uint32_t is_warmboot)
{
    struct kaps_ix_mgr *cur_mgr = mgr;
    struct kaps_ix_chunk *expanded_free_chunk = NULL;
    int32_t num_entries_per_sb = 64 * 1024;
    int32_t end_ix = -1;
    int32_t i;
    int32_t empty_ix_segment;
    int32_t is_fused;

    if (!is_warmboot)
    {
        POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, expanded_free_chunk);
        if (!expanded_free_chunk)
            return KAPS_OUT_OF_MEMORY;
        kaps_memset(expanded_free_chunk, 0, sizeof(*expanded_free_chunk));
    }

    while (mgr)
    {
        if (end_ix < mgr->end_ix)
            end_ix = mgr->end_ix;
        mgr = mgr->next;
    }

    if (ad_db)
    {
        cur_mgr = ad_db->mgr[ad_db->db_info.device->core_id];
        kaps_sassert(cur_mgr != NULL);
    }

    if (cur_mgr->size == 0)
    {

        cur_mgr->start_ix = end_ix + 1;
        cur_mgr->end_ix = end_ix;

        cur_mgr->end_ix += num_entries_per_sb;

        cur_mgr->size += num_entries_per_sb;

        /*
         * Initialize the ix_segments and assign the first segment 
         */
        for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
            cur_mgr->ix_segments[i] = -1;

        cur_mgr->ix_segments[0] = cur_mgr->start_ix;
        cur_mgr->ix_segments_end[0] = cur_mgr->end_ix;
        if (!is_warmboot)
        {
            expanded_free_chunk->start_ix = cur_mgr->start_ix;
            expanded_free_chunk->size = num_entries_per_sb;
        }
    }
    else
    {
        cur_mgr->end_ix = end_ix + num_entries_per_sb;
        cur_mgr->size += num_entries_per_sb;

        /*
         * Try to merge with existing segment, else create new segment 
         */
        empty_ix_segment = -1;
        is_fused = 0;   /* Indicates if we have merged the IX segments or not */
        for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
        {
            if (cur_mgr->ix_segments[i] == -1)
            {
                if (empty_ix_segment == -1)
                {
                    empty_ix_segment = i;
                }
            }
            else
            {
                if (cur_mgr->ix_segments_end[i] == end_ix)
                {
                    cur_mgr->ix_segments_end[i] = end_ix + num_entries_per_sb;
                    empty_ix_segment = -1;
                    is_fused = 1;
                    break;
                }
            }
        }

        if (empty_ix_segment == -1)
        {
            kaps_assert(is_fused, "Problem with IX expansion \n");
        }
        else
        {
            cur_mgr->ix_segments[empty_ix_segment] = end_ix + 1;
            cur_mgr->ix_segments_end[empty_ix_segment] = cur_mgr->end_ix;
        }

        if (!is_warmboot)
        {
            expanded_free_chunk->start_ix = end_ix + 1;
            expanded_free_chunk->size = num_entries_per_sb;
        }
    }

    if (!is_warmboot)
    {
        expanded_free_chunk->type = IX_FREE_CHUNK;

        kaps_ix_mgr_add_to_free_list(cur_mgr, expanded_free_chunk);
        kaps_ix_mgr_add_to_neighbor_list(cur_mgr, cur_mgr->neighbor_list_end, expanded_free_chunk);
    }

    return KAPS_OK;
}

kaps_status
kaps_kaps_ix_mgr_alloc_ad_dynamically(
    struct kaps_ix_mgr * mgr,
    struct kaps_ad_db * ad_db,
    uint32_t force_alloc,
    uint32_t is_warmboot)
{
    return KAPS_OUT_OF_AD;
}

kaps_status
kaps_kaps_ix_mgr_alloc_internal(
    struct kaps_ix_mgr * mgr,
    struct kaps_ad_db * ad_db,
    uint32_t rqt_size,
    void *lsn_ptr,
    enum kaps_ix_user_type user_type,
    struct kaps_ix_chunk ** alloc_chunk_pp)
{
    struct kaps_ix_chunk *chosen_chunk, *remainder_chunk = NULL, *remainder_chunk_2 = NULL;
    uint32_t level;
    uint32_t sb_no = -1;
    uint32_t sb_base_address;
    struct kaps_ix_mgr *cur_mgr = mgr;
    struct kaps_ad_db *cur_ad_db = ad_db;

    *alloc_chunk_pp = NULL;

    level = rqt_size - 1;
    if (rqt_size == 0 || level > mgr->max_num_chunk_levels - 1)
    {
        kaps_sassert(0);
        return KAPS_INVALID_ARGUMENT;
    }

    if (ad_db && ad_db->db_info.hw_res.ad_res->is_1_1 == 0)
        cur_ad_db = NULL;

    if (ad_db)
    {
        cur_mgr = ad_db->mgr[ad_db->db_info.device->core_id];
        kaps_sassert(cur_mgr);
    }

    /*
     * Get the free chunk 
     */
    chosen_chunk = NULL;
    if (cur_mgr->free_list[level])
    {
        if (!cur_ad_db)
            chosen_chunk = cur_mgr->free_list[level];
        else
            sb_no = kaps_ad_get_chunk(cur_mgr, ad_db, rqt_size, level, &chosen_chunk);
    }

    if (!chosen_chunk)
    {
        while (level < cur_mgr->max_num_chunk_levels)
        {
            if (cur_mgr->free_list[level] && level >= rqt_size)
            {
                if (cur_ad_db)
                {
                    sb_no = kaps_ad_get_chunk(cur_mgr, ad_db, rqt_size, level, &chosen_chunk);
                    if (sb_no != -1)
                        break;
                }
                else
                {
                    chosen_chunk = cur_mgr->free_list[level];
                    break;
                }
            }
            ++level;
        }
    }

    if (!chosen_chunk)
        return KAPS_OUT_OF_INDEX;

    if (chosen_chunk->size > rqt_size)
    {
        POOL_ALLOC(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, remainder_chunk);
        if (!remainder_chunk)
            return KAPS_OUT_OF_MEMORY;
        kaps_memset(remainder_chunk, 0, sizeof(*remainder_chunk));
        if (cur_ad_db && cur_mgr->uda_translate[sb_no] > chosen_chunk->start_ix)
        {
            POOL_ALLOC(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, remainder_chunk_2);
            if (!remainder_chunk_2)
                return KAPS_OUT_OF_MEMORY;
            kaps_memset(remainder_chunk_2, 0, sizeof(*remainder_chunk_2));
        }
    }

    /*
     * Remove the chosen chunk from the free list 
     */
    kaps_ix_mgr_remove_from_free_list(cur_mgr, chosen_chunk);

    if (remainder_chunk_2)
    {

        remainder_chunk_2->size = cur_mgr->uda_translate[sb_no] - chosen_chunk->start_ix;
        remainder_chunk_2->start_ix = chosen_chunk->start_ix;
        remainder_chunk_2->type = IX_FREE_CHUNK;

        /*
         * Can't alloc from start of chunk, break into three, and alloc the middle 
         */
        chosen_chunk->start_ix = cur_mgr->uda_translate[sb_no];

        kaps_ix_mgr_add_to_free_list(cur_mgr, remainder_chunk_2);
        kaps_ix_mgr_add_to_neighbor_list(cur_mgr, chosen_chunk->prev_neighbor, remainder_chunk_2);

        remainder_chunk->size = chosen_chunk->size - remainder_chunk_2->size - rqt_size;
        if (remainder_chunk->size)
        {
            remainder_chunk->start_ix = cur_mgr->uda_translate[sb_no] + rqt_size;
            remainder_chunk->type = IX_FREE_CHUNK;

            kaps_ix_mgr_add_to_free_list(cur_mgr, remainder_chunk);
            kaps_ix_mgr_add_to_neighbor_list(cur_mgr, chosen_chunk, remainder_chunk);
        }
        else
        {
            POOL_FREE(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, remainder_chunk);
        }

    }
    else if (remainder_chunk)
    {   /* Reinsert the remaining free chunk */
        remainder_chunk->size = chosen_chunk->size - rqt_size;
        remainder_chunk->start_ix = chosen_chunk->start_ix + rqt_size;
        remainder_chunk->type = IX_FREE_CHUNK;

        kaps_ix_mgr_add_to_free_list(cur_mgr, remainder_chunk);
        kaps_ix_mgr_add_to_neighbor_list(cur_mgr, chosen_chunk, remainder_chunk);
    }

    chosen_chunk->type = IX_ALLOCATED_CHUNK;
    chosen_chunk->next_free_chunk = NULL;
    chosen_chunk->prev_free_chunk = NULL;
    chosen_chunk->size = rqt_size;

    if (ad_db)
    {
        chosen_chunk->ad_info =
            ad_db->db_info.device->alloc->xcalloc(ad_db->db_info.device->alloc->cookie, 1,
                                                  sizeof(struct kaps_ad_chunk));
        if (!chosen_chunk->ad_info)
            return KAPS_OUT_OF_MEMORY;
        chosen_chunk->ad_info->device = ad_db->db_info.device;
        chosen_chunk->ad_info->ad_db = ad_db;
    }
    if (cur_ad_db)
    {
        sb_base_address =
            sb_no * (((KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / KAPS_BITS_IN_BYTE) / KAPS_HW_MIN_UDA_WIDTH_8);
        chosen_chunk->ad_info->uda_address =
            sb_base_address +
            ((chosen_chunk->start_ix -
              cur_mgr->uda_translate[sb_no]) * (cur_ad_db->device_bytes / KAPS_HW_MIN_UDA_WIDTH_8));
        chosen_chunk->ad_info->sb_no = sb_no;
        chosen_chunk->ad_info->offset_in_sb = chosen_chunk->start_ix - cur_mgr->uda_translate[sb_no];
    }
    else if (ad_db)
    {
        chosen_chunk->ad_info->uda_address = -1;
    }

    cur_mgr->num_allocated_ix += rqt_size;
    kaps_sassert(cur_mgr->num_allocated_ix <= cur_mgr->size);
    chosen_chunk->lsn_ptr = lsn_ptr;
    chosen_chunk->user_type = user_type;
    *alloc_chunk_pp = chosen_chunk;

    return KAPS_OK;

}

#if 0
static void
kaps_ad_get_sb_no_uda_address(
    struct kaps_ix_mgr *mgr,
    uint32_t num_entries_per_sb,
    int32_t start_loc,
    int32_t size,
    int32_t * sb_no,
    int32_t * uda_address,
    int32_t * incr_start_loc)
{
    int16_t i;
    uint32_t sb_end_loc;
    uint32_t *uda_translate = mgr->uda_translate;

    *incr_start_loc = 0;

    for (i = 0; i < KAPS_HW_MAX_UDA_SB; i++)
    {
        if (uda_translate[i] == -1)
            continue;

        sb_end_loc = uda_translate[i] + num_entries_per_sb - 1;
        if (start_loc > sb_end_loc)
            continue;

        if (start_loc >= uda_translate[i])
        {
            if ((start_loc + size - 1) <= sb_end_loc)
                break;

            if (((i + 1) < KAPS_HW_MAX_UDA_SB) && (uda_translate[i + 1] == sb_end_loc + 1))
                break;

            if ((start_loc + size) >= (sb_end_loc + 1))
            {
                while (i < KAPS_HW_MAX_UDA_SB && uda_translate[i] != sb_end_loc + 1)
                    i++;
                if (i == KAPS_HW_MAX_UDA_SB)
                    kaps_sassert(0);
                break;
            }
        }
    }
    kaps_sassert(i != KAPS_HW_MAX_UDA_SB);
    *sb_no = i;
    *uda_address = i * (((KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / KAPS_BITS_IN_BYTE) / KAPS_HW_MIN_UDA_WIDTH_8);

    if (mgr->uda_translate[i] > start_loc)
        *incr_start_loc = mgr->uda_translate[i] - start_loc;
    start_loc += *incr_start_loc;
    *uda_address += (start_loc - mgr->uda_translate[i]) * (mgr->main_ad_db->device_bytes / KAPS_HW_MIN_UDA_WIDTH_8);
}
#endif
/*#if 0*/
static void
kaps_ad_get_sb_no_uda_address(
    struct kaps_ix_mgr *mgr,
    uint32_t num_entries_per_sb,
    int32_t start_loc,
    int32_t size,
    int32_t * sb_no,
    int32_t * uda_address,
    int32_t * incr_start_loc)
{
    int16_t i, opt_sb_no;
    uint32_t sb_end_loc;
    uint32_t *uda_translate = mgr->uda_translate;

    *incr_start_loc = 0;

    opt_sb_no = mgr->ix_base_to_sb[(start_loc / num_entries_per_sb)];

    if (opt_sb_no == -1)
        opt_sb_no = 0;
    /* kaps_sassert(opt_sb_no >= 0 && opt_sb_no < KAPS_HW_MAX_UDA_SB);*/

    for (i = opt_sb_no; i < KAPS_HW_MAX_UDA_SB; i++)
    {
        if (uda_translate[i] == -1)
            continue;

        sb_end_loc = uda_translate[i] + num_entries_per_sb - 1;
        if (start_loc > sb_end_loc)
            continue;

        if (start_loc >= uda_translate[i])
        {
            break;
        }
    }

    if (i == KAPS_HW_MAX_UDA_SB)
    {
        for (i = 0; i < opt_sb_no; i++)
        {
            if (uda_translate[i] == -1)
                continue;

            sb_end_loc = uda_translate[i] + num_entries_per_sb - 1;
            if (start_loc > sb_end_loc)
                continue;

            if (start_loc >= uda_translate[i])
            {
                break;
            }
        }
        kaps_sassert(i != opt_sb_no);
    }

    kaps_sassert(start_loc >= uda_translate[i]);

    *sb_no = i;
    *uda_address = i * (((KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / KAPS_BITS_IN_BYTE) / KAPS_HW_MIN_UDA_WIDTH_8);
    *uda_address += (start_loc - mgr->uda_translate[i]) * (mgr->main_ad_db->device_bytes / KAPS_HW_MIN_UDA_WIDTH_8);
}
/*#endif*/

kaps_status
kaps_kaps_ix_mgr_compact_op(
    struct kaps_ix_mgr *mgr,
    struct kaps_ad_db *ad_db,
    int32_t * was_compaction_done,
    struct kaps_ix_compaction_chunk **playback_head_pp)
{
    struct kaps_ix_chunk *cur_chunk, *next_chunk, *prev_chunk;
    struct kaps_ix_chunk *new_free_chunk, *compaction_start_chunk;
    int32_t free_space, num_prexies = 0, num_pfx = 0;
    int32_t num_ix_in_window, num_empty_slots_in_lsn = 0;
    int32_t cb_start_ix, cb_size, incr_start_ix = 0;
    int32_t hole_start_ix, hole_size, count;
    struct kaps_ix_chunk *cb_head, *cb_tail, *tmp_chunk;
    struct kaps_ix_compaction_chunk *playback_chunk, *playback_head, *playback_tail;
    int32_t num_entries_per_sb = 0, has_reserved_slot = 0;      /* , num_entries = 0;*/
    struct kaps_device *device = NULL;

    device = ad_db->db_info.device;
    if (0 && device->main_bc_device)
        device = device->main_bc_device;

    kaps_sassert(ad_db);
    mgr = ad_db->mgr[ad_db->db_info.device->core_id];
    kaps_sassert(mgr);

    if (!mgr->cb_size || mgr->size <= KAPS_MAX_IX_INVOLVED_IN_COMPACTION)
    {
        *was_compaction_done = 0;
        return KAPS_OK;
    }

    num_entries_per_sb = (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / ad_db->db_info.width.ad_width_1;

    /*
     * Try to find a region with KAPS_MAX_IX_INVOLVED_IN_COMPACTION indexes that can be compacted
     */

    cur_chunk = mgr->neighbor_list->next_neighbor;
    free_space = 0;
    num_ix_in_window = 0;
    compaction_start_chunk = cur_chunk;

    if (0 && mgr->last_rembered_chunk)
        compaction_start_chunk = mgr->last_rembered_chunk;

    while (cur_chunk)
    {

        if (cur_chunk->next_neighbor)
        {
            int32_t sb_no1, sb_no2, incr, uda_addr;
            kaps_ad_get_sb_no_uda_address(mgr, num_entries_per_sb,
                                          cur_chunk->start_ix, cur_chunk->size, &sb_no1, &uda_addr, &incr);

            kaps_ad_get_sb_no_uda_address(mgr, num_entries_per_sb,
                                          cur_chunk->next_neighbor->start_ix,
                                          cur_chunk->next_neighbor->size, &sb_no2, &uda_addr, &incr);

            /*
             * It is possible for two neighboring chunks not be continuous since they belong to different ix_segments.
             * So for instance, if ix_segment[0] is 0 to 32K and segement[1] is 1M to 2M, then one chunk can have
             * end_ix as 32K and its neighbor can have start_ix as 1M. In this case we stop and reinitialize the
             * compaction window
             */
            if (cur_chunk->start_ix + cur_chunk->size != cur_chunk->next_neighbor->start_ix)
            {
                if (free_space >= 2 * mgr->max_alloc_chunk_size)
                {
                    /*
                     * We have already found a region that can be compacted
                     */
                    break;
                }
                else
                {
                    /*
                     * Re-initailize the compaction window by moving to the next chunk
                     */
                    cur_chunk = cur_chunk->next_neighbor;
                    free_space = 0;
                    num_ix_in_window = 0;
                    compaction_start_chunk = cur_chunk;
                    continue;
                }
            }

            /*
             * If the super blocks of the current chunk and next chunk are not the same or not continuous
             */
            if ((sb_no2 > sb_no1 + 1) || (sb_no2 < sb_no1))
            {
                if (free_space >= 2 * mgr->max_alloc_chunk_size)
                {
                    /*
                     * We have already found a region that can be compacted
                     */
                    break;
                }
                else
                {
                    /*
                     * Re-initailize the compaction window by moving to the next chunk
                     */
                    cur_chunk = cur_chunk->next_neighbor;
                    free_space = 0;
                    num_ix_in_window = 0;
                    compaction_start_chunk = cur_chunk;
                    continue;
                }
            }
        }

        /*
         * If we encounter an IX chunk for APT prefixes
         */
        if (cur_chunk->user_type == IX_USER_OVERFLOW)
        {

            if (free_space >= 2 * mgr->max_alloc_chunk_size)
            {
                break;
            }
            else
            {
                cur_chunk = cur_chunk->next_neighbor;
                free_space = 0;
                num_ix_in_window = 0;
                compaction_start_chunk = cur_chunk;
                continue;
            }
        }

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            free_space += cur_chunk->size;
        }
        else if (cur_chunk->type == IX_ALLOCATED_CHUNK)
        {
            /*
             * Try to reclaim the indexes from LSNs that have too many holes
             */
            num_pfx = kaps_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);

            if (!has_reserved_slot)
            {
                if (cur_chunk->size > num_pfx + KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION)
                    free_space += cur_chunk->size - num_pfx - KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION;
            }
        }

        num_ix_in_window += cur_chunk->size;    /* increment the total number of indexes (allocated + free) in the
                                                 * window */

        cur_chunk = cur_chunk->next_neighbor;

        if (num_ix_in_window >= KAPS_MAX_IX_INVOLVED_IN_COMPACTION)
        {

            if (free_space >= 2 * mgr->max_alloc_chunk_size)
            {
                break;
            }
            else
            {
                num_ix_in_window = 0;
                free_space = 0;
                compaction_start_chunk = cur_chunk;
            }
        }

    }

    if (free_space < 2 * mgr->max_alloc_chunk_size)
    {
        *was_compaction_done = 0;
        return KAPS_OK;
    }

    mgr->last_rembered_chunk = cur_chunk;

    tmp_chunk = compaction_start_chunk;
    if (tmp_chunk->prev_neighbor)
    {
        int32_t sb_no1, incr, uda_addr;
        kaps_ad_get_sb_no_uda_address(mgr, num_entries_per_sb,
                                      tmp_chunk->start_ix, tmp_chunk->size, &sb_no1, &uda_addr, &incr);

        if (incr)
        {
            compaction_start_chunk = tmp_chunk->next_neighbor;
        }
    }

    /*
     * Create the playback list of the chunks that need to be compacted
     */
    cur_chunk = compaction_start_chunk;
    free_space = 0;
    num_ix_in_window = 0;
    cb_start_ix = mgr->cb_start_ix;
    cb_size = mgr->cb_size;
    hole_start_ix = -1;
    hole_size = 0;
    cb_head = NULL;
    cb_tail = NULL;
    playback_head = NULL;
    playback_tail = NULL;
    count = 0;
    prev_chunk = compaction_start_chunk->prev_neighbor;
    while (cur_chunk && num_ix_in_window < KAPS_MAX_IX_INVOLVED_IN_COMPACTION)
    {

        next_chunk = cur_chunk->next_neighbor;

        if (next_chunk)
        {
            int32_t sb_no1, sb_no2, incr, uda_addr;
            kaps_ad_get_sb_no_uda_address(mgr, num_entries_per_sb,
                                          cur_chunk->start_ix, cur_chunk->size, &sb_no1, &uda_addr, &incr);

            kaps_ad_get_sb_no_uda_address(mgr, num_entries_per_sb,
                                          next_chunk->start_ix, next_chunk->size, &sb_no2, &uda_addr, &incr);

            if (cur_chunk->start_ix + cur_chunk->size != cur_chunk->next_neighbor->start_ix)
            {
                kaps_sassert(free_space >= 2 * mgr->max_alloc_chunk_size);
                break;
            }

            if ((sb_no2 > sb_no1 + 1) || (sb_no2 < sb_no1))
            {
                kaps_sassert(free_space >= 2 * mgr->max_alloc_chunk_size);
                break;
            }

        }

        num_ix_in_window += cur_chunk->size;

        /*
         * If we notice an APT IX chunk, then since we are not handling it, break out
         */
        if (cur_chunk->user_type == IX_USER_OVERFLOW)
        {
            break;
        }

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            if (hole_start_ix == -1)
            {
                hole_start_ix = cur_chunk->start_ix;
                hole_size = cur_chunk->size;
            }
            else
            {
                hole_size += cur_chunk->size;
            }

            free_space += cur_chunk->size;

            kaps_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
            kaps_ix_mgr_remove_from_free_list(mgr, cur_chunk);

            POOL_FREE(kaps_ix_chunk, &mgr->ix_chunk_pool, cur_chunk);

        }
        else
        {
            num_prexies += cur_chunk->size;
            if (hole_size < cur_chunk->size)
            {
                /*
                 * Since hole size is smaller than the chunk size, we will have to move the chunk to the compaction
                 * buffer region 
                 */
                playback_chunk =
                    mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_ix_compaction_chunk));

                if (!playback_chunk)
                    return KAPS_OUT_OF_MEMORY;

                num_pfx = kaps_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);

                num_empty_slots_in_lsn = 0;

                if (!has_reserved_slot)
                {
                    if (cur_chunk->size > num_pfx + KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION)
                    {
                        num_empty_slots_in_lsn = cur_chunk->size - num_pfx - KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION;
                    }
                }

                playback_chunk->kaps_ix_chunk = cur_chunk;
                playback_chunk->from_start_ix = cur_chunk->start_ix;
                playback_chunk->from_size = cur_chunk->size;
                playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
                playback_chunk->to_start_ix = cb_start_ix;
                playback_chunk->to_size = cur_chunk->size - num_empty_slots_in_lsn;
                kaps_ad_get_sb_no_uda_address(mgr, num_entries_per_sb,
                                              playback_chunk->to_start_ix,
                                              playback_chunk->to_size,
                                              &playback_chunk->sb_no, &playback_chunk->uda_address, &incr_start_ix);
                if (incr_start_ix)
                {
                    kaps_sassert(0);
                }
                playback_chunk->to_end_ix = cb_start_ix + playback_chunk->to_size - 1;

                kaps_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

                mgr->num_allocated_ix -= num_empty_slots_in_lsn;

                if (hole_start_ix == -1)
                    hole_start_ix = playback_chunk->from_start_ix;

                hole_size += cur_chunk->size;
                free_space += num_empty_slots_in_lsn;

                cb_start_ix += playback_chunk->to_size;
                cb_size -= playback_chunk->to_size;
                kaps_sassert(cb_size >= 0);

                cur_chunk->start_ix = playback_chunk->to_start_ix;
                cur_chunk->size = playback_chunk->to_size;

                kaps_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
                kaps_ix_mgr_add_to_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);

            }
            else
            {
                /*
                 * The hole size is big enough to accommodate the current chunk. So move the current chunk to the hole
                 * and advance the hole forward 
                 */
                playback_chunk =
                    mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_ix_compaction_chunk));
                if (!playback_chunk)
                    return KAPS_OUT_OF_MEMORY;

                num_pfx = kaps_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);

                num_empty_slots_in_lsn = 0;

                if (!has_reserved_slot)
                {
                    if (cur_chunk->size > num_pfx + KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION)
                    {
                        num_empty_slots_in_lsn = cur_chunk->size - num_pfx - KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION;
                    }
                }

                playback_chunk->kaps_ix_chunk = cur_chunk;
                playback_chunk->from_start_ix = cur_chunk->start_ix;
                playback_chunk->from_size = cur_chunk->size;
                playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
                playback_chunk->to_start_ix = hole_start_ix;
                playback_chunk->to_size = cur_chunk->size - num_empty_slots_in_lsn;
                kaps_ad_get_sb_no_uda_address(mgr, num_entries_per_sb, playback_chunk->to_start_ix,
                                              playback_chunk->to_size,
                                              &playback_chunk->sb_no, &playback_chunk->uda_address, &incr_start_ix);

                if (incr_start_ix)
                {
                    kaps_sassert(0);
                }
                playback_chunk->to_end_ix = hole_start_ix + playback_chunk->to_size - 1;

                kaps_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

                mgr->num_allocated_ix -= num_empty_slots_in_lsn;

                hole_start_ix += cur_chunk->size - num_empty_slots_in_lsn;
                hole_size += num_empty_slots_in_lsn;
                free_space += num_empty_slots_in_lsn;

                cur_chunk->start_ix = playback_chunk->to_start_ix;
                cur_chunk->size = playback_chunk->to_size;

                prev_chunk = cur_chunk;

            }
        }
        cur_chunk = next_chunk;
        count++;
    }

    /*
     * If we have moved any of chunks to the compaction buffer, then bring them back
     */
    cur_chunk = cb_head;
    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;

        num_prexies += cur_chunk->size;
        playback_chunk =
            mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_ix_compaction_chunk));
        if (!playback_chunk)
            return KAPS_OUT_OF_MEMORY;

        playback_chunk->kaps_ix_chunk = cur_chunk;
        playback_chunk->from_start_ix = cur_chunk->start_ix;
        playback_chunk->from_size = cur_chunk->size;
        playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
        playback_chunk->to_start_ix = hole_start_ix;
        playback_chunk->to_size = cur_chunk->size;
        kaps_ad_get_sb_no_uda_address(mgr, num_entries_per_sb, playback_chunk->to_start_ix,
                                      playback_chunk->to_size,
                                      &playback_chunk->sb_no, &playback_chunk->uda_address, &incr_start_ix);
        if (incr_start_ix)
        {
            kaps_sassert(0);
            playback_chunk->to_start_ix += incr_start_ix;
            hole_start_ix += incr_start_ix;
        }

        playback_chunk->to_end_ix = hole_start_ix + cur_chunk->size - 1;

        kaps_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

        hole_start_ix += cur_chunk->size;
        hole_size -= cur_chunk->size;

        cur_chunk->start_ix = playback_chunk->to_start_ix;

        kaps_ix_mgr_remove_from_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);
        kaps_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, cur_chunk);

        prev_chunk = cur_chunk;

        cur_chunk = next_chunk;
        count++;
    }

    /*
     * Add a free chunk at the end of the region corresponding to the hole that has been created
     */
    POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, new_free_chunk);
    if (!new_free_chunk)
        return KAPS_OUT_OF_MEMORY;

    kaps_memset(new_free_chunk, 0, sizeof(*new_free_chunk));

    new_free_chunk->type = IX_FREE_CHUNK;
    new_free_chunk->size = hole_size;
    new_free_chunk->start_ix = hole_start_ix;

    kaps_ix_mgr_add_to_free_list(mgr, new_free_chunk);
    kaps_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, new_free_chunk);

    *playback_head_pp = playback_head;
    *was_compaction_done = 1;

    return KAPS_OK;
}

#if 0
int32_t
kaps_lpm_db_get_num_entries(
    struct kaps_ad_db * ad_db,
    struct kaps_ix_mgr * mgr)
{
    struct kaps_ix_mgr *cur_mgr = ad_db->mgr[ad_db->db_info.device->core_id];
    struct kaps_ad_db *common_ad_db = ad_db;
    struct kaps_db *algo_db = ad_db->db_info.common_info->ad_info.db;
    struct kaps_db *common_algo_db = algo_db;
    int32_t i, num_entries;

    if (algo_db->common_ad_owner_db)
    {
        common_algo_db = algo_db->common_ad_owner_db;
        common_ad_db = (struct kaps_ad_db *) common_algo_db->common_info->ad_info.ad;
    }

    while (common_ad_db)
    {
        if (common_ad_db->mgr[common_ad_db->db_info.device->core_id] == ad_db->mgr[ad_db->db_info.device->core_id])
            break;
        common_ad_db = common_ad_db->next;
    }

    kaps_sassert(common_ad_db);

    cur_mgr = common_ad_db->mgr[common_ad_db->db_info.device->core_id];

    for (i = 0; i < common_algo_db->num_other_dbs_with_shared_ad; i++)
    {
    common_algo_db->other_dbs_with_shared_ad[i]}
}

#endif

kaps_status
kaps_ix_mgr_alloc(
    struct kaps_ix_mgr *mgr,
    struct kaps_ad_db *ad_db,
    uint32_t rqt_size,
    void *lsn_ptr,
    enum kaps_ix_user_type user_type,
    struct kaps_ix_chunk **alloc_chunk_pp)
{
    struct kaps_ad_db *cur_ad_db = ad_db;
    kaps_status status;

    if (ad_db && (ad_db->db_info.hw_res.ad_res->is_1_1 == 0))
        cur_ad_db = NULL;

    status = kaps_kaps_ix_mgr_alloc_internal(mgr, ad_db, rqt_size, lsn_ptr, user_type, alloc_chunk_pp);

    if (status == KAPS_OUT_OF_INDEX)
    {
        if (cur_ad_db)
        {

            /*
             * Before allocating try to see if we have exceeded more than 18 % wastage 
             */

            if (0)
            {
                int32_t num_entries = ad_db->ad_list.count;

                if (num_entries > 1024 * 1024)
                {
                    if ((mgr->num_allocated_ix - num_entries) > 0)
                    {
                        int32_t percent_of_free_slots =
                            ((mgr->num_allocated_ix - num_entries) * 100) / mgr->num_allocated_ix;

                        if (percent_of_free_slots > 40)
                            return KAPS_OUT_OF_AD;
                    }
                }
            }

            KAPS_STRY(kaps_kaps_ix_mgr_alloc_ad_dynamically(mgr, ad_db, 0, 0));
            status = kaps_kaps_ix_mgr_alloc_internal(mgr, ad_db, rqt_size, lsn_ptr, user_type, alloc_chunk_pp);
        }
        else if (ad_db && ((ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INPLACE)
                           || (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INDIRECTION)
                           || (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_CUSTOM)))
        {
            kaps_kaps_ix_mgr_expand_dummy_ix(mgr, ad_db, 0);
            status = kaps_kaps_ix_mgr_alloc_internal(mgr, ad_db, rqt_size, lsn_ptr, user_type, alloc_chunk_pp);
        }
    }

    return status;
}

kaps_status
kaps_ix_mgr_wb_alloc(
    struct kaps_ix_mgr * mgr,
    struct kaps_ad_db * ad_db,
    uint32_t rqt_size,
    uint32_t start_ix,
    void *lsn_ptr,
    enum kaps_ix_user_type user_type,
    struct kaps_ix_chunk ** alloc_chunk_pp)
{
    struct kaps_ix_chunk *chosen_chunk;
    struct kaps_ix_mgr *cur_mgr = mgr;
    struct kaps_ad_db *cur_ad_db = ad_db;
    int32_t level;
    uint32_t sb_no = -1;
    uint32_t sb_base_address;
    uint32_t num_entries_per_sb;

    *alloc_chunk_pp = NULL;

    level = rqt_size - 1;
    if (rqt_size == 0 || level > mgr->max_num_chunk_levels - 1)
    {
        kaps_sassert(0);
        return KAPS_INVALID_ARGUMENT;
    }

    if (ad_db && ad_db->db_info.hw_res.ad_res->is_1_1 == 0)
        cur_ad_db = NULL;

    if (ad_db)
    {
        cur_mgr = ad_db->mgr[ad_db->db_info.device->core_id];
        kaps_sassert(cur_mgr);
    }

    if (start_ix + rqt_size > cur_mgr->end_ix)
    {
        if (cur_ad_db)
        {
            KAPS_STRY(kaps_kaps_ix_mgr_alloc_ad_dynamically(mgr, ad_db, 1, 1));
        }
        else if (ad_db && ((ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INPLACE)
                           || (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INDIRECTION)
                           || (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_CUSTOM)))
        {
            if (ad_db->db_info.common_info->ad_info.db->common_info->enable_dynamic_allocation)
            {
                kaps_kaps_ix_mgr_expand_dummy_ix(mgr, ad_db, 1);
            }
        }
    }

    POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, chosen_chunk);
    if (!chosen_chunk)
        return KAPS_OUT_OF_MEMORY;
    kaps_memset(chosen_chunk, 0, sizeof(*chosen_chunk));

    chosen_chunk->type = IX_ALLOCATED_CHUNK;
    chosen_chunk->next_free_chunk = NULL;
    chosen_chunk->prev_free_chunk = NULL;
    chosen_chunk->size = rqt_size;
    chosen_chunk->start_ix = start_ix;
    chosen_chunk->next_neighbor = NULL;
    chosen_chunk->prev_neighbor = NULL;
    chosen_chunk->lsn_ptr = lsn_ptr;
    chosen_chunk->user_type = user_type;

    cur_mgr->num_allocated_ix += rqt_size;

    kaps_ix_mgr_add_to_neighbor_list(cur_mgr, NULL, chosen_chunk);

    if (ad_db)
    {
        chosen_chunk->ad_info =
            ad_db->db_info.device->alloc->xcalloc(ad_db->db_info.device->alloc->cookie, 1,
                                                  sizeof(struct kaps_ad_chunk));
        if (!chosen_chunk->ad_info)
            return KAPS_OUT_OF_MEMORY;
        chosen_chunk->ad_info->device = ad_db->db_info.device;
        chosen_chunk->ad_info->ad_db = ad_db;
    }
    if (cur_ad_db)
    {
        num_entries_per_sb = (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / cur_ad_db->db_info.width.ad_width_1;
        sb_no =
            kaps_ad_get_sb_for_chunk(cur_mgr, chosen_chunk->start_ix, chosen_chunk->size, rqt_size,
                                     cur_mgr->uda_translate, num_entries_per_sb);
        kaps_sassert(sb_no != -1);
        sb_base_address =
            sb_no * (((KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / KAPS_BITS_IN_BYTE) / KAPS_HW_MIN_UDA_WIDTH_8);
        chosen_chunk->ad_info->uda_address =
            sb_base_address +
            ((chosen_chunk->start_ix -
              cur_mgr->uda_translate[sb_no]) * (cur_ad_db->device_bytes / KAPS_HW_MIN_UDA_WIDTH_8));
        chosen_chunk->ad_info->sb_no = sb_no;
        chosen_chunk->ad_info->offset_in_sb = chosen_chunk->start_ix - cur_mgr->uda_translate[sb_no];
    }
    else if (ad_db)
    {
        chosen_chunk->ad_info->uda_address = -1;
    }

    *alloc_chunk_pp = chosen_chunk;

    return KAPS_OK;

}

kaps_status
kaps_ix_mgr_wb_finalize(
    struct kaps_ix_mgr * mgr)
{
    struct kaps_ix_chunk *iter_chunk, *alloc_chunk, *prev_chunk;
    uint32_t num_pfx = 0;
    uint32_t ix_seg_no = 0;
    uint32_t temp_start_ix, temp_end_ix;
    int32_t has_reserved_slot = 0;

    if (!mgr->neighbor_list)
    {

        struct kaps_ix_chunk *first_free_chunk = NULL;

        if (mgr->is_finalized)
            return KAPS_OK;

        if (mgr->end_ix - mgr->start_ix + 1)
        {
            POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, first_free_chunk);
            if (!first_free_chunk)
                return KAPS_OUT_OF_MEMORY;
            kaps_memset(first_free_chunk, 0, sizeof(*first_free_chunk));
            first_free_chunk->start_ix = mgr->start_ix;
            first_free_chunk->size = mgr->end_ix - mgr->start_ix + 1;
            first_free_chunk->type = IX_FREE_CHUNK;
            kaps_ix_mgr_add_to_free_list(mgr, first_free_chunk);
            mgr->neighbor_list = first_free_chunk;
            mgr->neighbor_list_end = first_free_chunk;
            mgr->is_finalized = 1;

            if (mgr->main_ad_db && mgr->main_ad_db->db_info.hw_res.ad_res->is_1_1)
                KAPS_STRY(kaps_ix_mgr_normalize_free_chunks(mgr));
        }

        if (mgr->next)
            KAPS_STRY(kaps_ix_mgr_wb_finalize(mgr->next));

        return KAPS_OK;
    }

    if (mgr->is_finalized)
        return KAPS_OK;

    mgr->neighbor_list = kaps_merge_sort(mgr->neighbor_list);

    iter_chunk = mgr->neighbor_list;
    temp_start_ix = mgr->start_ix;
    temp_end_ix = mgr->end_ix;
    prev_chunk = NULL;

    /*
     * Reconstruct the IX mgr chunks segment by segment 
     */
    while (mgr->ix_segments[ix_seg_no] != -1)
    {
        mgr->start_ix = mgr->ix_segments[ix_seg_no];
        mgr->end_ix = mgr->ix_segments_end[ix_seg_no];
        if (ix_seg_no == 0)
            mgr->start_ix += mgr->cb_size;

        if (!iter_chunk || iter_chunk->start_ix > mgr->end_ix)
        {
            POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, alloc_chunk);
            if (!alloc_chunk)
                return KAPS_OUT_OF_MEMORY;
            kaps_memset(alloc_chunk, 0, sizeof(*alloc_chunk));

            alloc_chunk->type = IX_FREE_CHUNK;
            alloc_chunk->next_free_chunk = NULL;
            alloc_chunk->prev_free_chunk = NULL;
            alloc_chunk->start_ix = mgr->start_ix;
            alloc_chunk->size = mgr->end_ix - mgr->start_ix + 1;
            alloc_chunk->next_neighbor = NULL;
            alloc_chunk->prev_neighbor = NULL;

            kaps_ix_mgr_add_to_free_list(mgr, alloc_chunk);
            kaps_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, alloc_chunk);
            prev_chunk = alloc_chunk;
            ix_seg_no++;
            continue;
        }

        if (iter_chunk->start_ix != mgr->start_ix)
        {
            POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, alloc_chunk);
            if (!alloc_chunk)
                return KAPS_OUT_OF_MEMORY;
            kaps_memset(alloc_chunk, 0, sizeof(*alloc_chunk));

            alloc_chunk->type = IX_FREE_CHUNK;
            alloc_chunk->next_free_chunk = NULL;
            alloc_chunk->prev_free_chunk = NULL;
            alloc_chunk->start_ix = mgr->start_ix;
            alloc_chunk->size = iter_chunk->start_ix - mgr->start_ix;
            alloc_chunk->next_neighbor = NULL;
            alloc_chunk->prev_neighbor = NULL;

            kaps_ix_mgr_add_to_free_list(mgr, alloc_chunk);
            kaps_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, alloc_chunk);
            prev_chunk = alloc_chunk;
        }

        while (iter_chunk && (iter_chunk->start_ix <= mgr->end_ix))
        {
            if (iter_chunk->next_neighbor && (iter_chunk->next_neighbor->start_ix <= mgr->end_ix))
            {
                if ((iter_chunk->start_ix + iter_chunk->size) < iter_chunk->next_neighbor->start_ix)
                {

                    POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, alloc_chunk);
                    if (!alloc_chunk)
                        return KAPS_OUT_OF_MEMORY;
                    kaps_memset(alloc_chunk, 0, sizeof(*alloc_chunk));

                    alloc_chunk->type = IX_FREE_CHUNK;
                    alloc_chunk->next_free_chunk = NULL;
                    alloc_chunk->prev_free_chunk = NULL;
                    alloc_chunk->start_ix = iter_chunk->start_ix + iter_chunk->size;
                    alloc_chunk->size = iter_chunk->next_neighbor->start_ix - alloc_chunk->start_ix;
                    alloc_chunk->next_neighbor = NULL;
                    alloc_chunk->prev_neighbor = NULL;

                    kaps_ix_mgr_add_to_free_list(mgr, alloc_chunk);
                    kaps_ix_mgr_add_to_neighbor_list(mgr, iter_chunk, alloc_chunk);

                }
                else if ((iter_chunk->start_ix + iter_chunk->size) > iter_chunk->next_neighbor->start_ix)
                {
                    num_pfx = kaps_ix_chunk_get_num_prefixes(iter_chunk->lsn_ptr, iter_chunk, &has_reserved_slot);
                    if ((iter_chunk->start_ix + num_pfx) > iter_chunk->next_neighbor->start_ix)
                    {
                        kaps_printf("\n Error: Overlapping Index Chunks \n");
                        return KAPS_INTERNAL_ERROR;
                    }
                    mgr->num_allocated_ix -= iter_chunk->size;
                    iter_chunk->size = iter_chunk->next_neighbor->start_ix - iter_chunk->start_ix;
                    mgr->num_allocated_ix += iter_chunk->size;
                }
            }
            else if (iter_chunk->start_ix + iter_chunk->size != (mgr->end_ix + 1))
            {

                POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, alloc_chunk);
                if (!alloc_chunk)
                    return KAPS_OUT_OF_MEMORY;
                kaps_memset(alloc_chunk, 0, sizeof(*alloc_chunk));

                alloc_chunk->type = IX_FREE_CHUNK;
                alloc_chunk->next_free_chunk = NULL;
                alloc_chunk->prev_free_chunk = NULL;
                alloc_chunk->start_ix = iter_chunk->start_ix + iter_chunk->size;
                alloc_chunk->size = mgr->end_ix - alloc_chunk->start_ix + 1;
                alloc_chunk->next_neighbor = NULL;
                alloc_chunk->prev_neighbor = NULL;

                kaps_ix_mgr_add_to_free_list(mgr, alloc_chunk);
                kaps_ix_mgr_add_to_neighbor_list(mgr, iter_chunk, alloc_chunk);

            }
            prev_chunk = iter_chunk;
            iter_chunk = iter_chunk->next_neighbor;
        }
        ix_seg_no++;
    }
    mgr->start_ix = temp_start_ix;
    mgr->end_ix = temp_end_ix;

    if (mgr->main_ad_db && mgr->main_ad_db->db_info.hw_res.ad_res->is_1_1)
        KAPS_STRY(kaps_ix_mgr_normalize_free_chunks(mgr));

    if (mgr->next)
        KAPS_STRY(kaps_ix_mgr_wb_finalize(mgr->next));

    mgr->is_finalized = 1;

    /*
     * IX Manager verify has to be called with recalc_neighbor_list_end argument set to 1 By doing this, we are now
     * correctly setting mgr->neighbor_list_end
     */
    KAPS_TRY(kaps_ix_mgr_verify(mgr, 1));

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_check_grow_up(
    struct kaps_ix_mgr * mgr,
    struct kaps_ix_chunk * cur_ix_chunk,
    uint32_t * grow_up_size_p)
{
    struct kaps_ix_chunk *prev_neighbor = cur_ix_chunk->prev_neighbor;
    uint32_t ad_grow = 1;
    struct kaps_ix_mgr *cur_mgr = mgr;
    uint32_t is_same_seg = 0;

    *grow_up_size_p = 0;

    if (cur_ix_chunk->ad_info)
    {
        cur_mgr = cur_ix_chunk->ad_info->ad_db->mgr[cur_ix_chunk->ad_info->ad_db->db_info.device->core_id];
        kaps_sassert(cur_mgr);
    }

    if (prev_neighbor && prev_neighbor->type == IX_FREE_CHUNK)
    {
        if (kaps_ix_mgr_get_seg_no(cur_mgr, cur_ix_chunk->start_ix) ==
            kaps_ix_mgr_get_seg_no(cur_mgr, prev_neighbor->start_ix))
            is_same_seg = 1;
        /*
         * If the previous chunk and the current chunk are adjacent, then we can grow up
         */
        if (cur_ix_chunk->ad_info && (cur_ix_chunk->ad_info->uda_address != -1))
        {
            ad_grow = kaps_check_ad_grow(cur_mgr, prev_neighbor->start_ix, cur_ix_chunk->start_ix,
                                         cur_mgr->uda_translate,
                                         (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) /
                                         cur_ix_chunk->ad_info->ad_db->db_info.width.ad_width_1);

        }
        if (prev_neighbor->start_ix + prev_neighbor->size == cur_ix_chunk->start_ix && ad_grow && is_same_seg)
        {
            *grow_up_size_p = prev_neighbor->size;
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_grow_up(
    struct kaps_ix_mgr * mgr,
    struct kaps_ix_chunk * cur_ix_chunk,
    uint32_t grow_up_size)
{
    struct kaps_ix_chunk *prev_neighbor = cur_ix_chunk->prev_neighbor;
    struct kaps_ix_mgr *cur_mgr = mgr;

    if (cur_ix_chunk->ad_info)
    {
        cur_mgr = cur_ix_chunk->ad_info->ad_db->mgr[cur_ix_chunk->ad_info->ad_db->db_info.device->core_id];
        kaps_sassert(cur_mgr);
    }

    if (!prev_neighbor || prev_neighbor->type != IX_FREE_CHUNK || prev_neighbor->size < grow_up_size)
    {
        kaps_sassert(0);
        return KAPS_INTERNAL_ERROR;
    }

    cur_ix_chunk->size += grow_up_size;

    cur_ix_chunk->start_ix -= grow_up_size;

    kaps_ix_mgr_remove_from_free_list(cur_mgr, prev_neighbor);
    prev_neighbor->size -= grow_up_size;

    if (prev_neighbor->size > 0)
    {
        kaps_ix_mgr_add_to_free_list(cur_mgr, prev_neighbor);
    }
    else
    {
        kaps_ix_mgr_remove_from_neighbor_list(cur_mgr, prev_neighbor);
        POOL_FREE(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, prev_neighbor);
    }

    cur_mgr->num_allocated_ix += grow_up_size;
    kaps_sassert(cur_mgr->num_allocated_ix <= cur_mgr->size);

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_check_grow_down(
    struct kaps_ix_mgr * mgr,
    struct kaps_ix_chunk * cur_ix_chunk,
    uint32_t * grow_down_size_p)
{
    struct kaps_ix_chunk *next_neighbor = cur_ix_chunk->next_neighbor;
    struct kaps_ix_mgr *cur_mgr = mgr;
    int32_t ad_grow = 1;
    uint32_t is_same_seg = 0;

    *grow_down_size_p = 0;

    if (cur_ix_chunk->ad_info)
    {
        cur_mgr = cur_ix_chunk->ad_info->ad_db->mgr[cur_ix_chunk->ad_info->ad_db->db_info.device->core_id];
        kaps_sassert(cur_mgr);
    }

    if (next_neighbor && next_neighbor->type == IX_FREE_CHUNK)
    {
        if (kaps_ix_mgr_get_seg_no(cur_mgr, cur_ix_chunk->start_ix) ==
            kaps_ix_mgr_get_seg_no(cur_mgr, next_neighbor->start_ix))
            is_same_seg = 1;
        /*
         * If the two chunks are adjacent, then we can grow down
         */
        if (cur_ix_chunk->ad_info && (cur_ix_chunk->ad_info->uda_address != -1))
        {
            ad_grow = kaps_check_ad_grow(cur_mgr, cur_ix_chunk->start_ix, next_neighbor->start_ix,
                                         cur_mgr->uda_translate,
                                         (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) /
                                         cur_ix_chunk->ad_info->ad_db->db_info.width.ad_width_1);
        }
        if (cur_ix_chunk->start_ix + cur_ix_chunk->size == next_neighbor->start_ix && ad_grow && is_same_seg)
        {
            *grow_down_size_p = next_neighbor->size;
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_grow_down(
    struct kaps_ix_mgr * mgr,
    struct kaps_ix_chunk * cur_ix_chunk,
    uint32_t grow_down_size)
{
    struct kaps_ix_chunk *next_neighbor = cur_ix_chunk->next_neighbor;
    struct kaps_ix_mgr *cur_mgr = mgr;

    if (cur_ix_chunk->ad_info)
    {
        cur_mgr = cur_ix_chunk->ad_info->ad_db->mgr[cur_ix_chunk->ad_info->ad_db->db_info.device->core_id];
        kaps_sassert(cur_mgr);
    }

    if (!next_neighbor || next_neighbor->type != IX_FREE_CHUNK || next_neighbor->size < grow_down_size)
    {
        kaps_sassert(0);
        return KAPS_INTERNAL_ERROR;
    }

    cur_ix_chunk->size += grow_down_size;

    kaps_ix_mgr_remove_from_free_list(cur_mgr, next_neighbor);
    next_neighbor->size -= grow_down_size;

    if (next_neighbor->size > 0)
    {
        next_neighbor->start_ix += grow_down_size;
        kaps_ix_mgr_add_to_free_list(cur_mgr, next_neighbor);
    }
    else
    {
        kaps_ix_mgr_remove_from_neighbor_list(cur_mgr, next_neighbor);
        POOL_FREE(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, next_neighbor);
    }

    cur_mgr->num_allocated_ix += grow_down_size;
    kaps_sassert(cur_mgr->num_allocated_ix <= cur_mgr->size);

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_free(
    struct kaps_ix_mgr * mgr,
    struct kaps_ix_chunk * released_chunk)
{
    struct kaps_ix_chunk *prev_neighbor = NULL, *next_neighbor = NULL;
    uint32_t can_merge_prev_neighbor = 0, can_merge_next_neighbor = 0;
    struct kaps_ix_mgr *cur_mgr = mgr;
    uint32_t ix_seg_no;

    if (!released_chunk)
        return KAPS_INVALID_ARGUMENT;

    if (released_chunk->ad_info)
    {
        cur_mgr = released_chunk->ad_info->ad_db->mgr[released_chunk->ad_info->ad_db->db_info.device->core_id];
        kaps_sassert(cur_mgr);
    }

    if (released_chunk->start_ix < cur_mgr->start_ix || released_chunk->size > cur_mgr->size)
    {
        kaps_sassert(0);
        return KAPS_INVALID_ARGUMENT;
    }

    released_chunk->lsn_ptr = NULL;

    prev_neighbor = released_chunk->prev_neighbor;
    next_neighbor = released_chunk->next_neighbor;

    if (prev_neighbor && prev_neighbor->type == IX_FREE_CHUNK)
    {
        ix_seg_no = kaps_ix_mgr_get_seg_no(cur_mgr, prev_neighbor->start_ix);

        kaps_sassert(ix_seg_no != KAPS_HW_MAX_UDA_SB);
        if (prev_neighbor->start_ix + prev_neighbor->size == released_chunk->start_ix)
            can_merge_prev_neighbor = 1;
        if (can_merge_prev_neighbor)
        {
            if (cur_mgr->ix_segments[ix_seg_no] <= released_chunk->start_ix)
            {
                if (cur_mgr->ix_segments_end[ix_seg_no] >= (released_chunk->start_ix + released_chunk->size - 1))
                {
                    can_merge_prev_neighbor = 1;
                }
                else
                {
                    can_merge_prev_neighbor = 0;
                }
            }
            else
            {
                can_merge_prev_neighbor = 0;
            }
        }
        if (can_merge_prev_neighbor && released_chunk->ad_info && released_chunk->ad_info->ad_db->user_width_1 != 0)
        {
            can_merge_prev_neighbor = kaps_check_ad_grow(cur_mgr, prev_neighbor->start_ix, released_chunk->start_ix,
                                                         cur_mgr->uda_translate,
                                                         (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) /
                                                         released_chunk->ad_info->ad_db->db_info.width.ad_width_1);
        }
    }

    if (next_neighbor && next_neighbor->type == IX_FREE_CHUNK)
    {
        ix_seg_no = kaps_ix_mgr_get_seg_no(cur_mgr, next_neighbor->start_ix);

        kaps_sassert(ix_seg_no != KAPS_HW_MAX_UDA_SB);

        if (next_neighbor->start_ix == released_chunk->start_ix + released_chunk->size)
            can_merge_next_neighbor = 1;

        if (can_merge_next_neighbor)
        {
            if (cur_mgr->ix_segments[ix_seg_no] <= released_chunk->start_ix)
            {
                if (cur_mgr->ix_segments_end[ix_seg_no] >= (released_chunk->start_ix + released_chunk->size - 1))
                {
                    can_merge_next_neighbor = 1;
                }
                else
                {
                    can_merge_next_neighbor = 0;
                }
            }
            else
            {
                can_merge_next_neighbor = 0;
            }
        }
        if (can_merge_next_neighbor && released_chunk->ad_info && released_chunk->ad_info->ad_db->user_width_1 != 0)
        {
            can_merge_next_neighbor = kaps_check_ad_grow(cur_mgr, released_chunk->start_ix, next_neighbor->start_ix,
                                                         cur_mgr->uda_translate,
                                                         (KAPS_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) /
                                                         released_chunk->ad_info->ad_db->db_info.width.ad_width_1);
        }
    }

    if (released_chunk->ad_info)
        released_chunk->ad_info->ad_db->db_info.device->alloc->xfree(released_chunk->ad_info->ad_db->db_info.
                                                                     device->alloc->cookie, released_chunk->ad_info);

    cur_mgr->num_allocated_ix -= released_chunk->size;

    if (can_merge_prev_neighbor && can_merge_next_neighbor)
    {
        kaps_ix_mgr_remove_from_free_list(cur_mgr, prev_neighbor);
        kaps_ix_mgr_remove_from_free_list(cur_mgr, next_neighbor);

        prev_neighbor->size += released_chunk->size + next_neighbor->size;

        kaps_ix_mgr_remove_from_neighbor_list(cur_mgr, next_neighbor);
        kaps_ix_mgr_remove_from_neighbor_list(cur_mgr, released_chunk);

        POOL_FREE(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, next_neighbor);
        POOL_FREE(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, released_chunk);

        kaps_ix_mgr_add_to_free_list(cur_mgr, prev_neighbor);

    }
    else if (can_merge_prev_neighbor)
    {
        kaps_ix_mgr_remove_from_free_list(cur_mgr, prev_neighbor);

        prev_neighbor->size += released_chunk->size;

        kaps_ix_mgr_remove_from_neighbor_list(cur_mgr, released_chunk);
        POOL_FREE(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, released_chunk);

        kaps_ix_mgr_add_to_free_list(cur_mgr, prev_neighbor);

    }
    else if (can_merge_next_neighbor)
    {
        kaps_ix_mgr_remove_from_free_list(cur_mgr, next_neighbor);

        released_chunk->size += next_neighbor->size;
        released_chunk->type = IX_FREE_CHUNK;

        kaps_ix_mgr_remove_from_neighbor_list(cur_mgr, next_neighbor);
        POOL_FREE(kaps_ix_chunk, &cur_mgr->ix_chunk_pool, next_neighbor);

        kaps_ix_mgr_add_to_free_list(cur_mgr, released_chunk);

    }
    else
    {
        released_chunk->type = IX_FREE_CHUNK;
        kaps_ix_mgr_add_to_free_list(cur_mgr, released_chunk);
    }

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_destroy(
    struct kaps_ix_mgr * mgr)
{
    struct kaps_ix_chunk *tmp, *next;
    struct kaps_allocator *alloc = NULL;

    if (!mgr)
        return KAPS_INVALID_ARGUMENT;

    if (mgr->next)
        KAPS_STRY(kaps_ix_mgr_destroy(mgr->next));

    alloc = mgr->ix_chunk_pool.allocator;

    tmp = mgr->neighbor_list;
    while (tmp)
    {
        next = tmp->next_neighbor;
        POOL_FREE(kaps_ix_chunk, &mgr->ix_chunk_pool, tmp);
        tmp = next;
    }
    POOL_FINI(kaps_ix_chunk, &mgr->ix_chunk_pool);

    if (mgr->free_list)
        alloc->xfree(alloc->cookie, mgr->free_list);

    if (mgr->num_ix_in_free_list)
        alloc->xfree(alloc->cookie, mgr->num_ix_in_free_list);

    alloc->xfree(alloc->cookie, mgr);
    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_calc_stats(
    const struct kaps_ix_mgr * mgr,
    struct kaps_ix_mgr_stats * ix_stats)
{
    const struct kaps_ix_mgr *tmp = mgr;
    struct kaps_ad_db *ad_db = tmp->main_ad_db;

    ix_stats->total_num_allocated_ix = 0;
    ix_stats->total_num_free_ix = 0;
    for (; tmp; tmp = tmp->next)
    {
        ix_stats->total_num_allocated_ix += tmp->num_allocated_ix;
        ix_stats->total_num_free_ix += (tmp->size - tmp->num_allocated_ix);
    }

    if (ad_db && ad_db->db_info.hw_res.ad_res->is_1_1)
    {
        struct kaps_ix_chunk *cur_chunk = mgr->neighbor_list;

        while (cur_chunk)
        {
            if (cur_chunk->type == IX_ALLOCATED_CHUNK)
            {
                ix_stats->num_entries_present[cur_chunk->ad_info->sb_no] += cur_chunk->size;
            }

            cur_chunk = cur_chunk->next_neighbor;
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_print_stats(
    struct kaps_ix_mgr_stats * ix_stats)
{
    uint32_t total_ix = ix_stats->total_num_allocated_ix + ix_stats->total_num_free_ix;
    float percent_allocated_ix = ix_stats->total_num_allocated_ix * 100.0F / total_ix;
    float percent_free_ix = ix_stats->total_num_free_ix * 100.0F / total_ix;

    kaps_printf("\n\nTotal number of allocated indexes = %d  (%.2f%%) \n",
                ix_stats->total_num_allocated_ix, percent_allocated_ix);

    kaps_printf("Total number of free indexes = %d (%.2f%%) \n\n", ix_stats->total_num_free_ix, percent_free_ix);

    return KAPS_OK;
}

/*
 * Compaction is done on a region of the entire IX space by moving all the allocated chunks to one side
 * and the free space to the other size in the region. The maximum size of the region that is compacted
 * is controlled using the macro KAPS_MAX_IX_INVOLVED_IN_COMPACTION.
 *
 * When compaction is done, we may also decide to shrink the IX space of an LSN if we think that the LSN has
 * too many holes. For instance, suppose an LSN has 50 holes and 70 prefixes with an IX space of 120. We will shrink
 * the IX space to 74 and release 46 indexes.
 *
 * While performing compaction, we have to maintain coherency. To ensure coherency, we maintain a compaction buffer
 * (that is abbreviated as cb). The compaction buffer is used to store indexes temporarily during compaction until we create
 * a hole that is big enough in the region being compacted so that the hole can accommodate the maximum sized LSN in it. So
 * suppose the maximum sized LSN is 1024, then we first move the allocated IX chunks to the compaction buffer until we
 * create a hole that is at least 1024 wide. From then on we can move the subsequent allocated IX chunks into the hole.
 * Once we have processed all the IX chunks in the region, we bring back the IX chunks from the compaction buffer back
 * to the hole in the region being compacted
 *
 * To decouple the IX Manager module from the LSN module, we are maintaining a playback list. All the IX chunk movements
 * are stored in a playback list and passed back to the LSN. The LSN will then process the playback list and perform the
 * index change operation on the prefixes in the LSN.
 *
*/
kaps_status
kaps_ix_mgr_compact(
    struct kaps_ix_mgr * mgr,
    int32_t * was_compaction_done,
    struct kaps_ix_compaction_chunk ** playback_head_pp)
{
    struct kaps_ix_chunk *cur_chunk, *next_chunk, *prev_chunk;
    struct kaps_ix_chunk *new_free_chunk, *compaction_start_chunk;
    int32_t free_space, num_empty_slots_in_lsn;
    int32_t num_ix_in_window, num_pfx;
    int32_t cb_start_ix, cb_size;
    int32_t hole_start_ix, hole_size, has_reserved_slot;
    struct kaps_ix_chunk *cb_head, *cb_tail;
    struct kaps_ix_compaction_chunk *playback_chunk, *playback_head, *playback_tail;

    /*
     * If the total IX space is too small, then return without performing compaction
     */
    if (mgr->size <= KAPS_MAX_IX_INVOLVED_IN_COMPACTION)
    {
        *was_compaction_done = 0;
        return KAPS_OK;
    }

    /*
     * First identify a region in the IX space which can be compacted so that we can free up at least two max LSNs
     * worth of indexes. We are checking for 2 LSNs since a split operation can produce two LSNs and we will want both
     * the LSNs to be able to successfully acquire the index space
     */
    cur_chunk = mgr->neighbor_list->next_neighbor;
    free_space = 0;
    num_ix_in_window = 0;
    compaction_start_chunk = cur_chunk;

    while (cur_chunk)
    {

        /*
         * To simplify processing, we don't try to compact chunks that are assigned to APT. So if we encounter an APT
         * chunk, we skip the chunk and start processing a new region after the APT IX chunk
         */
        if (cur_chunk->user_type == IX_USER_OVERFLOW)
        {

            if (free_space >= 2 * mgr->max_alloc_chunk_size)
            {
                break;
            }
            else
            {
                /*
                 * Skip the APT chunk and proceed to the next chunk in the next region
                 */
                cur_chunk = cur_chunk->next_neighbor;
                free_space = 0;
                num_ix_in_window = 0;
                compaction_start_chunk = cur_chunk;
                continue;
            }
        }

        /*
         * Figure out how much of free space we will be able to aggregate in the region
         */
        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            free_space += cur_chunk->size;
        }
        else if (cur_chunk->type == IX_ALLOCATED_CHUNK)
        {
            /*
             * If an LSN has holes more than KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION, then release the excess holes
             */
            num_pfx = kaps_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);
            if (cur_chunk->size > num_pfx + KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION)
                free_space += cur_chunk->size - num_pfx - KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION;
        }

        num_ix_in_window += cur_chunk->size;

        cur_chunk = cur_chunk->next_neighbor;

        /*
         * If we have processed all the IX chunks in the region and we have found sufficient free space then break out.
         * Otherwise proceed to the next region
         */
        if (num_ix_in_window >= KAPS_MAX_IX_INVOLVED_IN_COMPACTION)
        {

            if (free_space >= 2 * mgr->max_alloc_chunk_size)
            {
                break;
            }
            else
            {
                num_ix_in_window = 0;
                free_space = 0;
                compaction_start_chunk = cur_chunk;
            }
        }

    }

    if (free_space < 2 * mgr->max_alloc_chunk_size)
    {
        *was_compaction_done = 0;
        return KAPS_OK;
    }

    /*
     * We have a region where we can get sufficient free space by compacting. So perform the IX compaction on this
     * region
     */
    cur_chunk = compaction_start_chunk;
    free_space = 0;
    num_ix_in_window = 0;
    cb_start_ix = mgr->cb_start_ix;
    cb_size = mgr->cb_size;
    hole_start_ix = -1;
    hole_size = 0;
    cb_head = NULL;
    cb_tail = NULL;
    playback_head = NULL;
    playback_tail = NULL;
    prev_chunk = compaction_start_chunk->prev_neighbor;
    while (cur_chunk && num_ix_in_window < KAPS_MAX_IX_INVOLVED_IN_COMPACTION)
    {

        next_chunk = cur_chunk->next_neighbor;

        num_ix_in_window += cur_chunk->size;

        /*
         * If we notice an APT IX chunk, then since we are not handling it, break out
         */
        if (cur_chunk->user_type == IX_USER_OVERFLOW)
        {
            break;
        }

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            /*
             * We have come across a free chunk. So increase the hole size
             */
            if (hole_start_ix == -1)
            {
                hole_start_ix = cur_chunk->start_ix;
                hole_size = cur_chunk->size;
            }
            else
            {
                hole_size += cur_chunk->size;
            }

            free_space += cur_chunk->size;

            /*
             * Since all the holes will be aggregated and moved to the end of the region for which a single hole chunk
             * is sufficient, delete the current hole chunk
             */
            kaps_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
            kaps_ix_mgr_remove_from_free_list(mgr, cur_chunk);

            POOL_FREE(kaps_ix_chunk, &mgr->ix_chunk_pool, cur_chunk);

        }
        else
        {

            if (hole_size < cur_chunk->size)
            {
                /*
                 * Since hole size is smaller than the chunk size, we will have to move the chunk to the compaction
                 * buffer region 
                 */
                playback_chunk =
                    mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_ix_compaction_chunk));
                if (!playback_chunk)
                    return KAPS_OUT_OF_MEMORY;

                num_pfx = kaps_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);
                num_empty_slots_in_lsn = 0;
                if (cur_chunk->size > num_pfx + KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION)
                {
                    num_empty_slots_in_lsn = cur_chunk->size - num_pfx - KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION;
                }

                playback_chunk->kaps_ix_chunk = cur_chunk;
                playback_chunk->from_start_ix = cur_chunk->start_ix;
                playback_chunk->from_size = cur_chunk->size;
                playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
                playback_chunk->to_start_ix = cb_start_ix;
                playback_chunk->to_size = cur_chunk->size - num_empty_slots_in_lsn;
                playback_chunk->to_end_ix = cb_start_ix + playback_chunk->to_size - 1;

                kaps_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

                mgr->num_allocated_ix -= num_empty_slots_in_lsn;

                if (hole_start_ix == -1)
                    hole_start_ix = playback_chunk->from_start_ix;

                hole_size += cur_chunk->size;
                free_space += num_empty_slots_in_lsn;

                cb_start_ix += playback_chunk->to_size;
                cb_size -= playback_chunk->to_size;
                kaps_sassert(cb_size >= 0);

                cur_chunk->start_ix = playback_chunk->to_start_ix;
                cur_chunk->size = playback_chunk->to_size;

                kaps_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
                kaps_ix_mgr_add_to_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);

            }
            else
            {
                /*
                 * The hole size is big enough to accommodate the current chunk. So move the current chunk to the hole
                 * and advance the hole forward 
                 */
                playback_chunk =
                    mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_ix_compaction_chunk));

                num_pfx = kaps_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);
                num_empty_slots_in_lsn = 0;
                if (cur_chunk->size > num_pfx + KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION)
                {
                    num_empty_slots_in_lsn = cur_chunk->size - num_pfx - KAPS_MAX_HOLES_ALLOWED_IN_IX_COMPACTION;
                }

                playback_chunk->kaps_ix_chunk = cur_chunk;
                playback_chunk->from_start_ix = cur_chunk->start_ix;
                playback_chunk->from_size = cur_chunk->size;
                playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
                playback_chunk->to_start_ix = hole_start_ix;
                playback_chunk->to_size = cur_chunk->size - num_empty_slots_in_lsn;
                playback_chunk->to_end_ix = hole_start_ix + playback_chunk->to_size - 1;

                kaps_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

                mgr->num_allocated_ix -= num_empty_slots_in_lsn;

                hole_start_ix += cur_chunk->size - num_empty_slots_in_lsn;
                hole_size += num_empty_slots_in_lsn;
                free_space += num_empty_slots_in_lsn;

                cur_chunk->start_ix = playback_chunk->to_start_ix;
                cur_chunk->size = playback_chunk->to_size;

                prev_chunk = cur_chunk;
            }
        }
        cur_chunk = next_chunk;
    }

    /*
     * If we have moved any of chunks to the compaction buffer, then bring them back
     */
    cur_chunk = cb_head;
    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;

        playback_chunk =
            mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_ix_compaction_chunk));

        playback_chunk->kaps_ix_chunk = cur_chunk;
        playback_chunk->from_start_ix = cur_chunk->start_ix;
        playback_chunk->from_size = cur_chunk->size;
        playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
        playback_chunk->to_start_ix = hole_start_ix;
        playback_chunk->to_size = cur_chunk->size;
        playback_chunk->to_end_ix = hole_start_ix + cur_chunk->size - 1;

        kaps_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

        hole_start_ix += cur_chunk->size;
        hole_size -= cur_chunk->size;

        cur_chunk->start_ix = playback_chunk->to_start_ix;

        kaps_ix_mgr_remove_from_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);
        kaps_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, cur_chunk);

        prev_chunk = cur_chunk;

        cur_chunk = next_chunk;

    }

    /*
     * Add a free chunk at the end of the region corresponding to the hole that has been created
     */
    POOL_ALLOC(kaps_ix_chunk, &mgr->ix_chunk_pool, new_free_chunk);
    if (!new_free_chunk)
        return KAPS_OUT_OF_MEMORY;

    kaps_memset(new_free_chunk, 0, sizeof(*new_free_chunk));

    new_free_chunk->type = IX_FREE_CHUNK;
    new_free_chunk->size = hole_size;
    new_free_chunk->start_ix = hole_start_ix;

    kaps_ix_mgr_add_to_free_list(mgr, new_free_chunk);
    kaps_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, new_free_chunk);

    *playback_head_pp = playback_head;
    *was_compaction_done = 1;

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_verify(
    struct kaps_ix_mgr * mgr,
    uint32_t recalc_neighbor_list_end)
{
    uint32_t level = 0;
    struct kaps_ix_chunk *cur_chunk = mgr->neighbor_list, *prev_chunk = NULL;
    uint32_t total_free_ix_1, total_free_ix_2, total_alloc_ix;
    uint32_t cur_level_num_free_ix;
    uint32_t check_if_chunk_in_two_segments = 1;        /* Too time consuming, so set to 0 if needed */
    uint32_t check_if_chunk_in_non_neighbor_sb = 1;     /* Too time consuming, so set to 0 if needed */
    uint32_t check_if_segment_exists_for_chunk = 1;     /* Too time consuming, so set to 0 if needed */

    total_free_ix_1 = 0;
    total_alloc_ix = 0;
    while (cur_chunk)
    {

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            if (cur_chunk->next_free_chunk == NULL && cur_chunk->prev_free_chunk == NULL)
            {

                level = cur_chunk->size - 1;
                if (level >= mgr->max_num_chunk_levels)
                    level = mgr->max_num_chunk_levels - 1;

                if (mgr->free_list[level] != cur_chunk)
                {
                    kaps_printf("Incorrect arrangement in sorted neighbor list ");
                    kaps_handle_error();
                    return KAPS_INTERNAL_ERROR;
                }
            }
            total_free_ix_1 += cur_chunk->size;
        }

        if (cur_chunk->type == IX_ALLOCATED_CHUNK)
        {
            if (cur_chunk->next_free_chunk || cur_chunk->prev_free_chunk)
            {
                kaps_printf("Incorrect arrangement in sorted neighbor list ");
                kaps_handle_error();
                return KAPS_INTERNAL_ERROR;
            }
            total_alloc_ix += cur_chunk->size;
        }

        if (check_if_segment_exists_for_chunk)
        {
            if (kaps_ix_mgr_get_seg_no(mgr, cur_chunk->start_ix) == KAPS_HW_MAX_UDA_SB)
            {
                kaps_handle_error();
                return KAPS_INTERNAL_ERROR;
            }
        }

        if (check_if_chunk_in_two_segments)
        {
            if (kaps_ix_mgr_get_seg_no(mgr, cur_chunk->start_ix) !=
                kaps_ix_mgr_get_seg_no(mgr, cur_chunk->start_ix + cur_chunk->size - 1))
            {
                kaps_handle_error();
                return KAPS_INTERNAL_ERROR;
            }
        }

        if (check_if_chunk_in_non_neighbor_sb && mgr->main_ad_db && mgr->main_ad_db->db_info.hw_res.ad_res->is_1_1)
        {
            uint32_t start_ix_sb = kaps_ix_mgr_ix_to_sb(mgr, cur_chunk->start_ix);
            uint32_t end_ix_sb = kaps_ix_mgr_ix_to_sb(mgr, cur_chunk->start_ix + cur_chunk->size - 1);

            /*
             * An allocated chunk should either be in the same super block or in two neighboring super blocks A free
             * chunk can span over non-contiguous super blocks
             */
            if (cur_chunk->type == IX_ALLOCATED_CHUNK)
            {
                if ((start_ix_sb == end_ix_sb) || (start_ix_sb + 1 == end_ix_sb))
                {
                    /*
                     * Things are fine
                     */
                }
                else
                {
                    kaps_handle_error();
                    return KAPS_INTERNAL_ERROR;
                }
            }
            else
            {
                if (!kaps_is_chunk_in_continuous_sb(mgr, cur_chunk))
                {
                    kaps_handle_error();
                    return KAPS_INTERNAL_ERROR;
                }
            }

        }

        prev_chunk = cur_chunk;
        cur_chunk = cur_chunk->next_neighbor;
    }

    if (recalc_neighbor_list_end)
    {
        mgr->neighbor_list_end = prev_chunk;
    }
    else
    {
        if (prev_chunk != mgr->neighbor_list_end)
        {
            kaps_handle_error();
            return KAPS_INTERNAL_ERROR;
        }
    }

    if (prev_chunk)
    {
        if (prev_chunk->start_ix + prev_chunk->size != (mgr->end_ix + 1))
        {
            kaps_printf("Size in sorted neighbor list does not add up");
            kaps_handle_error();
            return KAPS_INTERNAL_ERROR;
        }
    }

    total_free_ix_2 = 0;
    for (level = 0; level < mgr->max_num_chunk_levels; ++level)
    {
        cur_level_num_free_ix = 0;
        cur_chunk = mgr->free_list[level];

        while (cur_chunk)
        {
            if (level == mgr->max_num_chunk_levels - 1)
            {
                if (cur_chunk->size - 1 < level)
                {
                    kaps_printf("Error in array of lists \n");
                    kaps_handle_error();
                    return KAPS_INTERNAL_ERROR;
                }
            }
            else
            {
                if (cur_chunk->size - 1 != level)
                {
                    kaps_printf("Error in array of lists \n");
                    kaps_handle_error();
                    return KAPS_INTERNAL_ERROR;
                }
            }

            cur_level_num_free_ix += cur_chunk->size;
            total_free_ix_2 += cur_chunk->size;
            cur_chunk = cur_chunk->next_free_chunk;
        }

        if (cur_level_num_free_ix != mgr->num_ix_in_free_list[level])
        {
            kaps_printf("Error in the number of IX in the free list\n");
            kaps_handle_error();
            return KAPS_INTERNAL_ERROR;
        }

    }

    if (total_free_ix_1 != total_free_ix_2)
    {
        kaps_printf("Error in the total free size between neighbor list and free list nodes\n");
        kaps_handle_error();
        return KAPS_INTERNAL_ERROR;
    }

    if (total_alloc_ix != mgr->num_allocated_ix)
    {
        kaps_printf("Error in the total alloc size\n");
        kaps_handle_error();
        return KAPS_INTERNAL_ERROR;
    }

    if (total_alloc_ix + total_free_ix_1 + mgr->cb_size != mgr->size)
    {
        kaps_printf("Some of the indexes in the IX Manager are missing\n");
        kaps_handle_error();
        return KAPS_INTERNAL_ERROR;
    }

    return KAPS_OK;

}

kaps_status
kaps_ix_mgr_wb_save_uda_translate(
    struct kaps_ix_mgr * mgr,
    kaps_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset)
{
    if (0 != write_fn(handle, (uint8_t *) & mgr->uda_translate, sizeof(mgr->uda_translate), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->uda_translate);

    if (0 != write_fn(handle, (uint8_t *) & mgr->ix_base_to_sb, sizeof(mgr->ix_base_to_sb), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->ix_base_to_sb);

    if (0 != write_fn(handle, (uint8_t *) & mgr->ix_segments, sizeof(mgr->ix_segments), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->ix_segments);
    if (0 != write_fn(handle, (uint8_t *) & mgr->ix_segments_end, sizeof(mgr->ix_segments_end), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->ix_segments_end);

    if (0 != write_fn(handle, (uint8_t *) & mgr->cb_start_ix, sizeof(mgr->cb_start_ix), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_start_ix);
    if (0 != write_fn(handle, (uint8_t *) & mgr->cb_end_ix, sizeof(mgr->cb_end_ix), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_end_ix);
    if (0 != write_fn(handle, (uint8_t *) & mgr->cb_size, sizeof(mgr->cb_size), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_size);

    if (0 != write_fn(handle, (uint8_t *) & mgr->start_ix, sizeof(mgr->start_ix), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->start_ix);
    if (0 != write_fn(handle, (uint8_t *) & mgr->end_ix, sizeof(mgr->end_ix), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->end_ix);
    if (0 != write_fn(handle, (uint8_t *) & mgr->size, sizeof(mgr->size), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->size);

    if (mgr->next)
        return kaps_ix_mgr_wb_save_uda_translate(mgr->next, write_fn, handle, nv_offset);

    return KAPS_OK;
}

kaps_status
kaps_ix_mgr_wb_restore_uda_translate(
    struct kaps_ix_mgr * mgr,
    kaps_device_issu_write_fn read_fn,
    void *handle,
    uint32_t * nv_offset)
{

    if (0 != read_fn(handle, (uint8_t *) & mgr->uda_translate, sizeof(mgr->uda_translate), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->uda_translate);

    if (0 != read_fn(handle, (uint8_t *) & mgr->ix_base_to_sb, sizeof(mgr->ix_base_to_sb), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->ix_base_to_sb);

    if (0 != read_fn(handle, (uint8_t *) & mgr->ix_segments, sizeof(mgr->ix_segments), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->ix_segments);
    if (0 != read_fn(handle, (uint8_t *) & mgr->ix_segments_end, sizeof(mgr->ix_segments_end), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->ix_segments_end);

    if (0 != read_fn(handle, (uint8_t *) & mgr->cb_start_ix, sizeof(mgr->cb_start_ix), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_start_ix);
    if (0 != read_fn(handle, (uint8_t *) & mgr->cb_end_ix, sizeof(mgr->cb_end_ix), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_end_ix);
    if (0 != read_fn(handle, (uint8_t *) & mgr->cb_size, sizeof(mgr->cb_size), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_size);

    if (0 != read_fn(handle, (uint8_t *) & mgr->start_ix, sizeof(mgr->start_ix), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->start_ix);
    if (0 != read_fn(handle, (uint8_t *) & mgr->end_ix, sizeof(mgr->end_ix), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->end_ix);
    if (0 != read_fn(handle, (uint8_t *) & mgr->size, sizeof(mgr->size), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->size);

    if (mgr->next)
        return kaps_ix_mgr_wb_restore_uda_translate(mgr->next, read_fn, handle, nv_offset);

    return KAPS_OK;
}
