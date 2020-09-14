/*
**************************************************************************************
Copyright 2009-2019 Broadcom Corporation

This program is the proprietary software of Broadcom Corporation and/or its licensors,
and may only be used, duplicated, modified or distributed pursuant to the terms and
conditions of a separate, written license agreement executed between you and
Broadcom (an "Authorized License").Except as set forth in an Authorized License,
Broadcom grants no license (express or implied),right to use, or waiver of any kind
with respect to the Software, and Broadcom expressly reserves all rights in and to
the Software and all intellectual property rights therein.
IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

Except as expressly set forth in the Authorized License,

1. This program, including its structure, sequence and organization, constitutes the
valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
protect the confidentiality thereof,and to use this information only in connection
with your use of Broadcom integrated circuit products.

2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
**************************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>

#include "kaps_simple_dba.h"
/*#include "kaps_utility.h"*/
#include "kaps_algo_hw.h"

#define kaps_debug_assert(expression) kaps_sassert(expression)

#define NO_ROOM 1000

static KAPS_INLINE uint32_t
kaps_first_good_after(
    struct kaps_simple_dba_range_for_pref_len *r)
{
    if (r == NULL)
        return 0;
    else
        return r->last_row + 1;
}

static KAPS_INLINE uint32_t
kaps_last_good_before(
    struct kaps_simple_dba_range_for_pref_len *r,
    struct kaps_simple_dba *m)
{
    if (r == NULL)
        return m->pool_size - 1;
    else
        return r->first_row - 1;
}

static KAPS_INLINE int32_t
kaps_count_shuffles_up(
    struct kaps_simple_dba_range_for_pref_len *r,
    struct kaps_simple_dba *m,
    struct kaps_simple_dba_range_for_pref_len **non_full)
{
    int32_t shuffles_so_far;
    for (shuffles_so_far = 1; r != NULL; r = r->next, shuffles_so_far++)
        if (kaps_count_bits(&m->free, r->first_row, kaps_last_good_before(r->next, m)))
        {
            *non_full = r;
            return shuffles_so_far;
        }
    return NO_ROOM;
}

static KAPS_INLINE int32_t
kaps_count_shuffles_down(
    struct kaps_simple_dba_range_for_pref_len *r,
    struct kaps_simple_dba *m,
    struct kaps_simple_dba_range_for_pref_len **non_full)
{
    int32_t shuffles_so_far;
    for (shuffles_so_far = 1; r != NULL; r = r->prev, shuffles_so_far++)
        if (kaps_count_bits(&m->free, kaps_first_good_after(r->prev), r->last_row))
        {
            *non_full = r;
            return shuffles_so_far;
        }
    return NO_ROOM;
}

static KAPS_INLINE int32_t
kaps_kaps_shuffle_up_one_entry(
    struct kaps_simple_dba_range_for_pref_len *r,
    int32_t group_id,
    int32_t new_pos,
    struct kaps_simple_dba *m,
    struct kaps_fast_bitmap *used)
{
    int32_t empty_pos;

    m->nodes[group_id].entry_move_callback(m, m->pfx_bundles[r->first_row], new_pos, r->prefix_len);
    m->pfx_bundles[new_pos] = m->pfx_bundles[r->first_row];
    if (new_pos > r->last_row)
        r->last_row = new_pos;
    empty_pos = r->first_row;
    r->first_row = kaps_find_first_bit_set(used, r->first_row + 1, r->last_row);
    kaps_debug_assert(r->first_row != -1);
    return empty_pos;
}

static KAPS_INLINE int32_t
kaps_kaps_shuffle_down_one_entry(
    struct kaps_simple_dba_range_for_pref_len *r,
    int32_t group_id,
    int32_t new_pos,
    struct kaps_simple_dba *m,
    struct kaps_fast_bitmap *used)
{
    int32_t empty_pos;

    m->nodes[group_id].entry_move_callback(m, m->pfx_bundles[r->last_row], new_pos, r->prefix_len);
    m->pfx_bundles[new_pos] = m->pfx_bundles[r->last_row];
    if (new_pos < r->first_row)
        r->first_row = new_pos;
    empty_pos = r->last_row;
    r->last_row = kaps_find_last_bit_set(used, r->first_row, r->last_row - 1);
    kaps_debug_assert(r->last_row != -1);
    return empty_pos;
}

static KAPS_INLINE int32_t
kaps_shuffle_up(
    struct kaps_simple_dba_range_for_pref_len *non_full,
    int32_t num_shuffles,
    int32_t group_id,
    struct kaps_simple_dba *m)
{
    int32_t new_pos, free_entries;

    free_entries = kaps_count_bits(&m->free, non_full->first_row, non_full->last_row);
    if (free_entries)
        new_pos = kaps_find_last_bit_set(&m->free, non_full->first_row, non_full->last_row);
    else
        new_pos = kaps_find_first_bit_set(&m->free, non_full->last_row + 1, kaps_last_good_before(non_full->next, m));
    kaps_debug_assert(new_pos > 0);
    kaps_reset_bit(&m->free, new_pos);
    kaps_set_bit(&m->nodes[group_id].used, new_pos);
    new_pos = kaps_kaps_shuffle_up_one_entry(non_full, group_id, new_pos, m, &m->nodes[group_id].used);
    /** Cycling through the remaining ranges. */
    for (non_full = non_full->prev, num_shuffles--; num_shuffles > 0; non_full = non_full->prev, num_shuffles--)
    {
        kaps_debug_assert(non_full != NULL);
        new_pos = kaps_kaps_shuffle_up_one_entry(non_full, group_id, new_pos, m, &m->nodes[group_id].used);
    }
    return new_pos;
}

static KAPS_INLINE int32_t
kaps_shuffle_down(
    struct kaps_simple_dba_range_for_pref_len *non_full,
    int32_t num_shuffles,
    int32_t group_id,
    struct kaps_simple_dba *m)
{
    int32_t new_pos = 0, free_entries;

    free_entries = kaps_count_bits(&m->free, non_full->first_row, non_full->last_row);
    if (free_entries)
        new_pos = kaps_find_first_bit_set(&m->free, non_full->first_row, non_full->last_row);
    else
        new_pos = kaps_find_last_bit_set(&m->free, kaps_first_good_after(non_full->prev), non_full->first_row - 1);
    kaps_debug_assert(new_pos >= 0);
    kaps_reset_bit(&m->free, new_pos);
    kaps_set_bit(&m->nodes[group_id].used, new_pos);
    new_pos = kaps_kaps_shuffle_down_one_entry(non_full, group_id, new_pos, m, &m->nodes[group_id].used);
    /** Cycling through the remaining ranges. */
    for (non_full = non_full->next, num_shuffles--; num_shuffles > 0; non_full = non_full->next, num_shuffles--)
    {
        kaps_debug_assert(non_full != NULL);
        new_pos = kaps_kaps_shuffle_down_one_entry(non_full, group_id, new_pos, m, &m->nodes[group_id].used);
    }
    return new_pos;
}

/** Makes room for one new entry by shuffling either in the up
    direction or in the down direction, whichever is easier. */
static KAPS_INLINE int32_t
kaps_make_room(
    struct kaps_simple_dba *m,
    int32_t group_id,
    struct kaps_simple_dba_range_for_pref_len *down,
    struct kaps_simple_dba_range_for_pref_len *up)
{
    int32_t up_shuffles, down_shuffles;
    struct kaps_simple_dba_range_for_pref_len *non_full_up = NULL;
    struct kaps_simple_dba_range_for_pref_len *non_full_down = NULL;

    down_shuffles = kaps_count_shuffles_down(down, m, &non_full_down);
    up_shuffles = kaps_count_shuffles_up(up, m, &non_full_up);
    if (up_shuffles < down_shuffles)
        return kaps_shuffle_up(non_full_up, up_shuffles, group_id, m);
    kaps_debug_assert(down_shuffles != NO_ROOM);

    return kaps_shuffle_down(non_full_down, down_shuffles, group_id, m);
}

static KAPS_INLINE struct kaps_simple_dba_range_for_pref_len *
kaps_create_wb_new_range(
    struct kaps_simple_dba *m,
    int32_t len,
    int32_t group_id,
    struct kaps_simple_dba_range_for_pref_len *prev,
    struct kaps_simple_dba_range_for_pref_len *next,
    uint32_t pos)
{
    struct kaps_simple_dba_range_for_pref_len *r;

    r = (struct kaps_simple_dba_range_for_pref_len *) m->alloc->xcalloc(m->alloc->cookie, 1,
                                                                        sizeof(struct
                                                                               kaps_simple_dba_range_for_pref_len));
    kaps_sassert(r != NULL);

    r->prefix_len = len;
    r->prev = prev;
    if (prev != NULL)
        prev->next = r;
    r->next = next;
    if (next != NULL)
        next->prev = r;
    /*
     * Set the range first row and last row and update the BMPs 
     */
    r->last_row = r->first_row = pos;
    kaps_reset_bit(&m->free, pos);
    kaps_set_bit(&m->nodes[group_id].used, pos);

    return r;
}

static KAPS_INLINE struct kaps_simple_dba_range_for_pref_len *
kaps_create_new_range(
    struct kaps_simple_dba *m,
    int32_t len,
    int32_t group_id,
    struct kaps_simple_dba_range_for_pref_len *prev,
    struct kaps_simple_dba_range_for_pref_len *next)
{
    int32_t free_entries, pos;
    struct kaps_simple_dba_range_for_pref_len *r;

    r = (struct kaps_simple_dba_range_for_pref_len *) m->alloc->xcalloc(m->alloc->cookie, 1,
                                                                        sizeof(struct
                                                                               kaps_simple_dba_range_for_pref_len));

    if (!r)
        return NULL;

    r->prefix_len = len;
    r->prev = prev;
    if (prev != NULL)
        prev->next = r;
    r->next = next;
    if (next != NULL)
        next->prev = r;
    free_entries = kaps_count_bits(&m->free, kaps_first_good_after(prev), kaps_last_good_before(next, m));
    if (free_entries)
    {
        pos =
            kaps_find_nth_bit(&m->free, 6 * free_entries / 7 + 1, kaps_first_good_after(prev),
                              kaps_last_good_before(next, m));
        kaps_reset_bit(&m->free, pos);
        kaps_set_bit(&m->nodes[group_id].used, pos);
    }
    else
        pos = kaps_make_room(m, group_id, prev, next);
    r->last_row = r->first_row = pos;
    return r;
}

static KAPS_INLINE kaps_status
kaps_find_place_for_entry_worker(
    struct kaps_simple_dba *m,
    int32_t group_id,
    int32_t len,
    int32_t * result)
{
    struct kaps_simple_dba_range_for_pref_len *r = m->nodes[group_id].chunks;
    int32_t free_entries, free_after, free_before, pos;

    if (!r)
    {
        m->nodes[group_id].chunks = kaps_create_new_range(m, len, group_id, NULL, NULL);
        if (!m->nodes[group_id].chunks)
            return KAPS_OUT_OF_MEMORY;

        *result = m->nodes[group_id].chunks->first_row;
        return KAPS_OK;
    }

    if (len > r->prefix_len)
    {
        m->nodes[group_id].chunks = kaps_create_new_range(m, len, group_id, NULL, r);
        if (!m->nodes[group_id].chunks)
            return KAPS_OUT_OF_MEMORY;

        *result = m->nodes[group_id].chunks->first_row;
        return KAPS_OK;
    }

    while (r->next != NULL && len <= r->next->prefix_len)
        r = r->next;
    if (len != r->prefix_len)
    {
        kaps_debug_assert(len < r->prefix_len && len > (r->next == NULL ? -1 : r->next->prefix_len));
        r = kaps_create_new_range(m, len, group_id, r, r->next);
        if (!r)
            return KAPS_OUT_OF_MEMORY;

        *result = r->first_row;
        return KAPS_OK;
    }
    /** The range for our entry already exists */
    free_entries = kaps_count_bits(&m->free, r->first_row, r->last_row);
    if (free_entries)
    {
        pos = kaps_find_nth_bit(&m->free, 2 * free_entries / 3 + 1, r->first_row, r->last_row);
        kaps_reset_bit(&m->free, pos);
        kaps_set_bit(&m->nodes[group_id].used, pos);

        *result = pos;
        return KAPS_OK;
    }
    /*
     * Trying to grow the range into the empty space before or after it. 
     */
    free_before = kaps_count_bits(&m->free, kaps_first_good_after(r->prev), r->first_row - 1);
    free_after = kaps_count_bits(&m->free, r->last_row + 1, kaps_last_good_before(r->next, m));
    if (free_before > free_after)
    {
        pos = kaps_find_last_bit_set(&m->free, kaps_first_good_after(r->prev), r->first_row - 1);
        r->first_row = pos;
        kaps_reset_bit(&m->free, pos);
        kaps_set_bit(&m->nodes[group_id].used, pos);

        *result = pos;
        return KAPS_OK;
    }
    if (free_after > 0)
    {
        pos = kaps_find_first_bit_set(&m->free, r->last_row + 1, kaps_last_good_before(r->next, m));
        r->last_row = pos;

        kaps_reset_bit(&m->free, pos);
        kaps_set_bit(&m->nodes[group_id].used, pos);

        *result = pos;
        return KAPS_OK;
    }
    /** Must shuffle and grow the range */
    pos = kaps_make_room(m, group_id, r->prev, r->next);
    if (pos > r->last_row)
        r->last_row = pos;
    else
    {
        kaps_debug_assert(pos < r->first_row);
        r->first_row = pos;
    }

    *result = pos;
    return KAPS_OK;
}

#define SET_BITS kaps_reset_bit(&m->free, pos);      \
    kaps_set_bit(&m->nodes[group_id].used, pos);

static KAPS_INLINE void
kaps_set_at_loc_entry_worker(
    struct kaps_simple_dba *m,
    int32_t group_id,
    int32_t len,
    uint32_t pos)
{
    struct kaps_simple_dba_range_for_pref_len *r = m->nodes[group_id].chunks;

    if (!r)
    {   /* If no pfxlen range chunks */
        m->nodes[group_id].chunks = kaps_create_wb_new_range(m, len, group_id, NULL, NULL, pos);
        return;
    }

    if (len > r->prefix_len)
    {   /* if entry len is more than the larges pfxlen we have a range for */
        m->nodes[group_id].chunks = kaps_create_wb_new_range(m, len, group_id, NULL, r, pos);
        return;
    }

    while (r->next != NULL && len <= r->next->prefix_len)
        r = r->next;
    if (len != r->prefix_len)
    {   /* If we dont have the exact pfxlen range as current pfxlen */
        kaps_debug_assert(len < r->prefix_len && len > (r->next == NULL ? -1 : r->next->prefix_len));
        (void) kaps_create_wb_new_range(m, len, group_id, r, r->next, pos);
        return;
    }
    /** The range for our entry already exists */
    if (pos < r->first_row)
    {   /* Grow up */
        r->first_row = pos;
    }
    else if (pos > r->last_row)
    {   /* Grow down */
        r->last_row = pos;
    }
    SET_BITS;
}

void
kaps_simple_dba_place_entry_at_loc(
    struct kaps_simple_dba *m,
    void *pfx_bundle,
    int32_t group_id,
    int32_t len,
    uint32_t pos)
{
    m->free_entries--;
    kaps_set_at_loc_entry_worker(m, group_id, len, pos);        /* Set the entry at specified pos,update bmps and
                                                                 * pfxlen ranges */
    m->pfx_bundles[pos] = pfx_bundle;
}

kaps_status
kaps_simple_dba_find_place_for_entry(
    struct kaps_simple_dba *m,
    void *pfx_bundle,
    int32_t group_id,
    int32_t len,
    int32_t * result)
{
    int32_t pos = -1;
    kaps_status status;

    if (m->nodes[group_id].we_are_in_pre_insert_mode)
    {
        kaps_debug_assert(m->nodes[group_id].pre_inserted_entries_not_placed_yet == 0);
        m->nodes[group_id].we_are_in_pre_insert_mode = 0;
    }

    if (m->free_entries == 0)
    {
        *result = -1;
        return KAPS_OK;
    }

    m->free_entries--;
    status = kaps_find_place_for_entry_worker(m, group_id, len, &pos);

    if (status != KAPS_OK)
        return status;

    kaps_debug_assert(pos != -1);

    m->pfx_bundles[pos] = pfx_bundle;
    *result = pos;

    return KAPS_OK;
}

static KAPS_INLINE struct kaps_simple_dba_range_for_pref_len *
kaps_create_pre_range(
    struct kaps_allocator *alloc,
    int32_t len,
    struct kaps_simple_dba_range_for_pref_len *prev,
    struct kaps_simple_dba_range_for_pref_len *next)
{
    struct kaps_simple_dba_range_for_pref_len *r;

    r = (struct kaps_simple_dba_range_for_pref_len *) alloc->xcalloc(alloc->cookie, 1,
                                                                     sizeof(struct kaps_simple_dba_range_for_pref_len));
    kaps_sassert(r != NULL);
    r->prefix_len = len;
    r->prev = prev;
    if (prev != NULL)
        prev->next = r;
    r->next = next;
    if (next != NULL)
        next->prev = r;
    r->first_row = 32767;
    r->last_row = 1;
    return r;
}

void
kaps_simple_dba_pre_insert_entry(
    struct kaps_simple_dba *m,
    int32_t group_id,
    int32_t len)
{
    struct kaps_simple_dba_range_for_pref_len *r = m->nodes[group_id].chunks;

    m->nodes[group_id].we_are_in_pre_insert_mode = 1;
    kaps_debug_assert(m->free_entries > m->nodes[group_id].pre_inserted_entries_not_placed_yet);
    m->nodes[group_id].pre_inserted_entries_not_placed_yet++;

    if (!r)
        m->nodes[group_id].chunks = kaps_create_pre_range(m->alloc, len, NULL, NULL);
    else if (len > r->prefix_len)
        m->nodes[group_id].chunks = kaps_create_pre_range(m->alloc, len, NULL, r);
    else
    {
        while (r->next != NULL && len <= r->next->prefix_len)
            r = r->next;
        if (len != r->prefix_len)
        {
            kaps_debug_assert(len < r->prefix_len && len > (r->next == NULL ? -1 : r->next->prefix_len));
            kaps_create_pre_range(m->alloc, len, r, r->next);
        }
        else
            r->last_row++;
    }
}

static KAPS_INLINE int32_t
kaps_measure_length(
    struct kaps_simple_dba_range_for_pref_len *r)
{
    int32_t length = 0;

    while (r != NULL)
    {
        r = r->next;
        length++;
    }
    return length;
}

void
kaps_simple_dba_convert_ranges_from_pre_insert(
    struct kaps_simple_dba *m,
    int32_t group_id)
{
    struct kaps_simple_dba_range_for_pref_len *r = m->nodes[group_id].chunks;
    int32_t free_entries = m->free_entries - m->nodes[group_id].pre_inserted_entries_not_placed_yet;
    int32_t ranges = kaps_measure_length(r);
    /** Each range gets a share of the free entries that will be right
        after it. The start of the pool gets a bigger share as new
        ranges are expected. Share sizes decay exponentially. */
    int32_t share = free_entries - (ranges > 1 ? free_entries / 3 : free_entries / 4);

    free_entries -= share;
    kaps_debug_assert(m->nodes[group_id].we_are_in_pre_insert_mode);
    r->first_row = kaps_find_nth_bit(&m->free, share, 0, m->pool_size - 1);
    kaps_debug_assert(r->first_row != -1);
    for (; r->next != NULL; r = r->next, ranges--)
    {
        kaps_debug_assert(ranges > 0);
        share = free_entries - free_entries * 2 / 3;
        /*
         * r->last_row has (future) number of entries in r 
         */
        r->next->first_row = kaps_find_nth_bit(&m->free, r->last_row + share, r->first_row + 1, m->pool_size - 1);
        kaps_debug_assert(r->next->first_row != -1);
        r->last_row = r->first_row - 1;
        free_entries -= share;
    }
    r->last_row = r->first_row - 1;
    kaps_debug_assert(ranges == 1);
}

int32_t
kaps_simple_dba_find_place_for_pre_inserted_entry(
    struct kaps_simple_dba *m,
    void *pfx_bundle,
    int32_t group_id,
    int32_t len)
{
    struct kaps_simple_dba_range_for_pref_len *r = m->nodes[group_id].chunks;

    kaps_debug_assert(m->nodes[group_id].pre_inserted_entries_not_placed_yet > 0);
    kaps_debug_assert(m->nodes[group_id].we_are_in_pre_insert_mode);
    m->nodes[group_id].pre_inserted_entries_not_placed_yet--;
    m->free_entries--;
    while (r->prefix_len != len)
        r = r->next;
    r->last_row = kaps_find_first_bit_set(&m->free, r->last_row + 1, m->pool_size - 1);
    kaps_debug_assert(r->last_row <= (int32_t) kaps_last_good_before(r->next, m));
    kaps_reset_bit(&m->free, r->last_row);
    kaps_set_bit(&m->nodes[group_id].used, r->last_row);
    m->pfx_bundles[r->last_row] = pfx_bundle;
    return r->last_row;
}

void
kaps_simple_dba_free_entry(
    struct kaps_simple_dba *m,
    int32_t group_id,
    int32_t len,
    uint32_t pos)
{
    struct kaps_simple_dba_range_for_pref_len *r = m->nodes[group_id].chunks;

    if (m->nodes[group_id].we_are_in_pre_insert_mode)
    {
        kaps_debug_assert(m->nodes[group_id].pre_inserted_entries_not_placed_yet == 0);
        m->nodes[group_id].we_are_in_pre_insert_mode = 0;
    }
    m->free_entries++;
    kaps_debug_assert(kaps_get_bit(&m->nodes[group_id].used, pos));
    kaps_set_bit(&m->free, pos);
    kaps_reset_bit(&m->nodes[group_id].used, pos);
    m->pfx_bundles[pos] = NULL;
    while (r != NULL && r->prefix_len != len)
        r = r->next;
    kaps_debug_assert(r != NULL);
    if (r->first_row == r->last_row)
    {
        kaps_debug_assert(r->first_row == (int32_t) pos);
        if (r->prev != NULL)
            r->prev->next = r->next;
        else
            m->nodes[group_id].chunks = r->next;
        if (r->next != NULL)
            r->next->prev = r->prev;
        m->alloc->xfree(m->alloc->cookie, r);
    }
    else if (r->first_row == (int32_t) pos)
        r->first_row = kaps_find_first_bit_set(&m->nodes[group_id].used, r->first_row + 1, r->last_row);
    else if (r->last_row == (int32_t) pos)
        r->last_row = kaps_find_last_bit_set(&m->nodes[group_id].used, r->first_row, r->last_row - 1);
}

void
kaps_simple_dba_replace_entry(
    struct kaps_simple_dba *m,
    void *pfx_bundle,
    int32_t group_id,
    uint32_t pos)
{
    kaps_debug_assert(kaps_get_bit(&m->nodes[group_id].used, pos));
    m->pfx_bundles[pos] = pfx_bundle;
}

void *
kaps_simple_dba_get_entry(
    struct kaps_simple_dba *m,
    uint32_t pos)
{
    return m->pfx_bundles[pos];
}

struct kaps_simple_dba *
kaps_simple_dba_init(
    struct kaps_device *device,
    uint32_t pool_start,
    uint32_t pool_size,
    uint32_t dba_no,
    uint32_t is_clone)
{
    kaps_status status;
    struct kaps_simple_dba *m = (struct kaps_simple_dba *) device->alloc->xcalloc(device->alloc->cookie, 1,
                                                                                  sizeof(struct kaps_fib_dba));

    if (m == NULL)
        return NULL;

    status = kaps_init_bmp(&m->free, device->alloc, pool_size, 1);

    if (status != KAPS_OK)
    {
        device->alloc->xfree(device->alloc->cookie, m);
        return NULL;
    }

    m->pfx_bundles = (void **) device->alloc->xcalloc(device->alloc->cookie, pool_size, sizeof(void *));
    if (m->pfx_bundles == NULL)
    {
        kaps_free_bmp(&m->free, device->alloc);
        device->alloc->xfree(device->alloc->cookie, m);
        return NULL;
    }

    m->pool_start_loc = pool_start;
    m->pool_size = pool_size;
    m->free_entries = pool_size;
    m->device = device;
    m->alloc = device->alloc;
    m->dba_no = dba_no;
    m->is_clone = is_clone;
    return m;
}

void
kaps_simple_dba_destroy(
    struct kaps_simple_dba *m)
{
    int32_t i;

    for (i = 0; i < MAX_NUM_RPT_ENTRIES_IN_POOL; i++)
    {
        if (m->nodes[i].alloced)
            kaps_simple_dba_free_node(m, i);
    }
    kaps_free_bmp(&m->free, m->alloc);
    m->alloc->xfree(m->alloc->cookie, m->pfx_bundles);
    m->alloc->xfree(m->alloc->cookie, m);
}

kaps_status
kaps_kaps_simple_dba_init_node(
    struct kaps_simple_dba *m,
    int32_t group_id,
    void (*entry_move_callback) (void *,
                                 void *,
                                 uint32_t,
                                 uint32_t))
{
    if (m->nodes[group_id].alloced)
        return KAPS_OK;

    KAPS_STRY(kaps_init_bmp(&m->nodes[group_id].used, m->alloc, m->pool_size, 0));
    m->nodes[group_id].chunks = NULL;
    m->nodes[group_id].we_are_in_pre_insert_mode = 0;
    m->nodes[group_id].pre_inserted_entries_not_placed_yet = 0;
    m->nodes[group_id].alloced = 1;
    m->nodes[group_id].entry_move_callback = entry_move_callback;

    return KAPS_OK;
}

void
kaps_simple_dba_free_node(
    struct kaps_simple_dba *m,
    int32_t group_id)
{
    struct kaps_simple_dba_range_for_pref_len *curChunk_p = m->nodes[group_id].chunks, *delChunk_p = NULL;

    kaps_free_bmp(&m->nodes[group_id].used, m->alloc);

    while (curChunk_p)
    {
        delChunk_p = curChunk_p;
        curChunk_p = curChunk_p->next;
        m->alloc->xfree(m->alloc->cookie, delChunk_p);
    }
    m->nodes[group_id].chunks = NULL;
    m->nodes[group_id].alloced = 0;
}

uint32_t
kaps_simple_dba_verify(
    struct kaps_simple_dba *simple_dba_p,
    int32_t group_id,
    int32_t(*get_prio_length) (void *))
{
    uint32_t pos = 0;
    void *pfxBundle_p = NULL;
    struct kaps_simple_dba_range_for_pref_len *range_chunk_p = simple_dba_p->nodes[group_id].chunks;
    uint32_t num_simple_dba_entries = 0;

    for (pos = 0; pos < simple_dba_p->pool_size; ++pos)
    {
        if (kaps_get_bit(&(simple_dba_p->nodes[group_id].used), pos))
        {
            pfxBundle_p = simple_dba_p->pfx_bundles[pos];
            get_prio_length(pfxBundle_p);

            if (kaps_get_bit(&(simple_dba_p->free), pos))
                kaps_assert(0, "Free bits and used bits are inconsistent \n");

            ++num_simple_dba_entries;
        }
    }

    while (range_chunk_p)
    {
        kaps_assert(range_chunk_p->first_row <= range_chunk_p->last_row,
                    "Empty range chunk may be due to inconsistent pre-insert\n");

        if (kaps_count_bits
            (&(simple_dba_p->nodes[group_id].used), kaps_first_good_after(range_chunk_p->prev),
             range_chunk_p->first_row - 1))
            kaps_assert(0, "");

        if (!kaps_get_bit(&(simple_dba_p->nodes[group_id].used), range_chunk_p->first_row))
            kaps_assert(0, "Range chunks are inconsistent \n");

        if (!kaps_get_bit(&(simple_dba_p->nodes[group_id].used), range_chunk_p->last_row))
            kaps_assert(0, "Range chunks are inconsistent \n");

        if (range_chunk_p->next == NULL)
            if (kaps_count_bits
                (&(simple_dba_p->nodes[group_id].used), range_chunk_p->last_row + 1, simple_dba_p->pool_size - 1))
                kaps_assert(0, "");

        range_chunk_p = range_chunk_p->next;
    }

    return num_simple_dba_entries;
}

void *
kaps_simple_dba_search(
    struct kaps_simple_dba *m,
    int32_t group_id,
    struct kaps_db *db,
    uint8_t * key,
    int32_t(*compare_fn) (struct kaps_db * db,
                          uint8_t * key,
                          void *user_data))
{
#if 0
    int32_t pos;
    for (pos = 0; pos < m->pool_size; pos++)
    {
        if (kaps_get_bit(&(m->nodes[group_id].used), pos))
        {
            if (compare_fn(db, key, m->pfx_bundles[pos]) == 0)
                return m->pfx_bundles[pos];
        }
    }
    return NULL;
#endif
    int32_t last_pos = 0;
    do
    {
        last_pos = kaps_find_first_bit_set(&m->nodes[group_id].used, last_pos, m->pool_size - 1);
        if (last_pos == -1)
            return NULL;
        if (compare_fn(db, key, m->pfx_bundles[last_pos]) == 0)
            return m->pfx_bundles[last_pos];
        last_pos++;
    }
    while (last_pos < m->pool_size);

    return NULL;
}

void
kaps_simple_dba_repair_entry(
    struct kaps_simple_dba *m,
    uint32_t row_nr,
    uint32_t * found)
{
    uint32_t i;
    int32_t group_id = -1;
    struct kaps_simple_dba_range_for_pref_len *r;

    *found = 0;

    for (i = 0; i < MAX_NUM_RPT_ENTRIES_IN_POOL; ++i)
    {
        if (!m->nodes[i].chunks)
        {
            continue;
        }

        if (kaps_get_bit(&m->nodes[i].used, row_nr))
        {
            group_id = i;
            break;
        }
    }

    if (group_id == -1)
    {
        return;
    }

    r = m->nodes[group_id].chunks;
    while (r)
    {
        if (r->first_row <= row_nr && row_nr <= r->last_row)
            break;
        r = r->next;
    }

    if (!r)
    {
        return;
    }

    *found = 1;

    m->nodes[group_id].entry_move_callback(m, m->pfx_bundles[row_nr], row_nr, r->prefix_len);
}
