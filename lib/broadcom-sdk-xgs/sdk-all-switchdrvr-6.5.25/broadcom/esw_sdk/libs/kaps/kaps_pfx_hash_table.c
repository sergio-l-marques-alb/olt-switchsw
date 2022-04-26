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

#include "kaps_pfx_hash_table.h"
#include "kaps_math.h"

/**
 * @cond INTERNAL
 *
 * @addtogroup PFX_HASH_TABLE
 * @{
 *
 * The max size of the hash table should always be a prime number
 *
 * The hash function used is CRC32 (kaps_crc32)
 *
 * Mostly it is mandatory for the data bits after the prefix length to be set to 0. For instance
 * if the prefix length is 36 bits, then bits after the prefix length till the nearest byte boundary should be 0
 * CURRENTLY ONLY
 *
 * A NULL pointer is used to mark an unused slot. An INVALID pointer is used to mark a deleted slot
 *
 * When an entry is added to the hash table, the hash value is generated using CRC32 on the prefix data.
 * The slot index is given by slot index = hash value % max size of the hash table (max size is a prime number)
 * If there is a NULL pointer or an INVALID pointer at the slot , then we can store the entry at this location.
 * If some other entry is already present at this location, then we increment the hash value with a predetermined value
 * to get the next slot index. This procedure continues till we get a usable slot in the hash table
 *
 * To search the user will specify the data and the length of the search prefix. Again the hash value is generated
 * using CRC32 on the search data. The slot index is given by  slot index = hash value % max size of the hash table
 * If we have a valid entry pointer at the slot then we compare the data of the entry at the hash slot with the user data
 * If they match then we have found the entry. If there is no match, then we generate the next slot index by adding a
 * predetermined value to the current slot index. This continues till we get a match or we get a NULL pointer. If a NULL
 * pointer is found in the slot, then we return indicating the prefix was not found in the hash table.
 * When we get an invalid pointer (which indicates a deleted prefix), we skip this slot and go to the next slot. This is
 * because there can be further entries in the hash table that still need to be compared with the search prefix
 *
 * TO DELETE AN ENTRY THE USER HAS TO PASS THE HANDLE PASSED BACK FROM THE SEARCH OPERATION. The
 * deleted slot is marked with an invalid value (NOT a NULL value).
 *
 * CURRENTLY WE DEREFERENCE THE ENTRY HANDLE INSIDE THE HASH TABLE TO GET THE PREFIX DATA. WE ARE ASSUMING
 * THAT THE ENTRY HANDLE HAS THE TYPE kaps_acl_entry. SO THIS HASH TABLE WILL NOT WORK IF SOME OTHER ENTRY
 * TYPE IS PASSED
 *
 * <hr>
 * @}
 *
 * @endcond
 */

static kaps_status
kaps_pfx_hash_table_ctor(
    struct pfx_hash_table *self,
    struct kaps_allocator *alloc,
    uint32_t initsz,
    uint32_t thresh,
    uint32_t resizeby,
    uint32_t max_pfx_width_1,
    uint32_t d_offset,
    struct kaps_db *db)
{
    /*
     * Look for reasonable initial sizes 
     */
    kaps_sassert(initsz >= 8);
    kaps_sassert(initsz < (1 << 30));

    /*
     * Thresholds must be reasonable as well 
     */
    kaps_sassert(thresh > 25);
    kaps_sassert(thresh < 90);

    kaps_sassert(resizeby > thresh);
    kaps_sassert(resizeby < 1001);

    initsz = (initsz * 100) / thresh;
    initsz++;

    self->m_nThreshold = (uint16_t) thresh;
    self->m_alloc_p = alloc;

    self->m_nMaxPfxWidth_1 = max_pfx_width_1;

    self->m_nMaxSize = kaps_find_next_prime(initsz);

    self->m_nResizeBy = resizeby;

    self->m_nThreshSize = (self->m_nMaxSize * thresh) / 100;

    self->m_db = db;

    self->m_slots_p = alloc->xcalloc(alloc->cookie, self->m_nMaxSize, sizeof(struct kaps_entry *));

    if (!self->m_slots_p)
        return KAPS_OUT_OF_MEMORY;

    self->m_doffset = d_offset;

    return KAPS_OK;
}

kaps_status
kaps_pfx_hash_table_create(
    struct kaps_allocator * alloc,
    uint32_t initsz,
    uint32_t thresh,
    uint32_t resizeby,
    uint32_t max_pfx_width_1,
    uint32_t d_offset,
    struct kaps_db * db,
    struct pfx_hash_table ** self_pp)
{
    struct pfx_hash_table *ht;
    kaps_status status;

    *self_pp = NULL;

    if (db->type != KAPS_DB_ACL && db->type != KAPS_DB_LPM)
        return KAPS_UNSUPPORTED;

    ht = alloc->xcalloc(alloc->cookie, 1, sizeof(struct pfx_hash_table));
    if (!ht)
        return KAPS_OUT_OF_MEMORY;
    status = kaps_pfx_hash_table_ctor(ht, alloc, initsz, thresh, resizeby, max_pfx_width_1, d_offset, db);

    if (status != KAPS_OK)
    {
        alloc->xfree(alloc->cookie, ht);
        return status;
    }

    *self_pp = ht;
    return KAPS_OK;
}

kaps_status
kaps_pfx_hash_table_destroy(
    struct pfx_hash_table * self)
{
    if (self)
    {
        struct kaps_allocator *alloc = self->m_alloc_p;

        if (self->m_slots_p)
            alloc->xfree(alloc->cookie, self->m_slots_p);

        alloc->xfree(alloc->cookie, self);
    }

    return KAPS_OK;
}

kaps_status
kaps_pfx_hash_table_insert(
    struct pfx_hash_table * self,
    struct kaps_entry * entry_to_insert)
{
    uint32_t h1, h2, ix, bound;
    int32_t is_inserted = 0;
    kaps_status status;
    uint32_t len_1;
    uint8_t *data;

    if (self->m_nCurSize >= self->m_nThreshSize)
    {
        status = kaps_pfx_hash_table_expand(self);
        if (status != KAPS_OK)
            return status;
    }

    kaps_sassert (self->m_nMaxSize);

    KAPS_STRY(self->m_db->fn_table->get_data_len(self->m_db, entry_to_insert, &len_1, &data));

    kaps_sassert(entry_to_insert != HASH_TABLE_NULL_VALUE && entry_to_insert != HASH_TABLE_INVALID_VALUE);

    h1 = kaps_crc32(len_1, data, (len_1 + 7) / 8);

    bound = self->m_nMaxSize;
    h1 %= bound;
    h2 = (h1 + bound) >> 1;

    for (ix = h1;; ix += h2)
    {
        struct kaps_entry *slot;

        if (ix >= bound)
            ix -= bound;

        slot = self->m_slots_p[ix];
        if (slot == HASH_TABLE_NULL_VALUE || slot == HASH_TABLE_INVALID_VALUE)
        {
            if (slot == HASH_TABLE_INVALID_VALUE)
                self->m_nInvalidSlots--;

            self->m_slots_p[ix] = entry_to_insert;
            self->m_nCurSize++;
            is_inserted = 1;
            break;
        }
    }

    kaps_sassert(is_inserted == 1);
    return KAPS_OK;
}

kaps_status
kaps_pfx_hash_table_locate_internal(
    struct pfx_hash_table * self,
    uint8_t * data,
    uint32_t len_1,
    int32_t * is_loop_cnt_exceeded,
    struct kaps_entry *** ret)
{
    uint32_t h1, h2, ix, bound;
    uint32_t nBytes = (len_1 + 7) / 8;
    int32_t i = 0;
    uint32_t loop_cnt;
    uint32_t e_len_1;
    uint8_t *e_data;

    kaps_sassert(ret != 0);

    *ret = 0;
    *is_loop_cnt_exceeded = 0;

    if (self->m_db->type == KAPS_DB_ACL)
        data += self->m_doffset;

    h1 = kaps_crc32(len_1, data, (len_1 + 7) / 8);
    bound = self->m_nMaxSize;
    h1 %= bound;
    h2 = (h1 + bound) >> 1;
    loop_cnt = 0;

    for (ix = h1;; ix += h2)
    {
        struct kaps_entry *slot;

        ++loop_cnt;

        /*
         * We have observed in some cases that this loop can become an infinite loop. So detect and return if we get
         * into an infinite loop
         */
        if (loop_cnt > self->m_nMaxSize)
        {
            *is_loop_cnt_exceeded = 1;
            break;
        }

        if (ix >= bound)
            ix -= bound;
        slot = self->m_slots_p[ix];

        if (slot == HASH_TABLE_NULL_VALUE)
        {
            break;
        }
        else if (slot == HASH_TABLE_INVALID_VALUE)
        {
            continue;
        }
        else
        {

            KAPS_STRY(self->m_db->fn_table->get_data_len(self->m_db, slot, &e_len_1, &e_data));

            if (len_1 == e_len_1)
            {
                int32_t isPfxFound = 1;

                for (i = nBytes - 1; i >= 0; --i)
                {
                    if (data[i] != e_data[i])
                    {
                        isPfxFound = 0;
                        break;
                    }
                }

                if (isPfxFound)
                {
                    *ret = self->m_slots_p + ix;
                    return KAPS_OK;
                }
            }
        }
    }

    return KAPS_OK;

}

kaps_status
kaps_pfx_hash_table_locate(
    struct pfx_hash_table * self,
    uint8_t * data,
    uint32_t len_1,
    struct kaps_entry *** ret)
{
    int32_t is_loop_cnt_exceeded = 0;

    KAPS_TRY(kaps_pfx_hash_table_locate_internal(self, data, len_1, &is_loop_cnt_exceeded, ret));

    /*
     * In some very rare observed cases, the hash table may go into an infinite loop. To overcome the problem first
     * reconstruct the hash table by removing INVALID entries and repeat the search
     */
    if (is_loop_cnt_exceeded)
    {
        KAPS_STRY(kaps_pfx_hash_table_reconstruct(self));
        KAPS_TRY(kaps_pfx_hash_table_locate_internal(self, data, len_1, &is_loop_cnt_exceeded, ret));
    }

    /*
     * In the infinite loop problem still persists, expand the hash table and repeat the search
     */
    if (is_loop_cnt_exceeded)
    {
        KAPS_STRY(kaps_pfx_hash_table_expand(self));
        KAPS_TRY(kaps_pfx_hash_table_locate_internal(self, data, len_1, &is_loop_cnt_exceeded, ret));
    }

    kaps_sassert(is_loop_cnt_exceeded == 0);

    return KAPS_OK;
}

kaps_status
kaps_pfx_hash_table_delete(
    struct pfx_hash_table * self,
    struct kaps_entry ** slot)
{
    kaps_sassert(slot >= self->m_slots_p);
    kaps_sassert(slot < (self->m_slots_p + self->m_nMaxSize));

    if (*slot == HASH_TABLE_NULL_VALUE || *slot == HASH_TABLE_INVALID_VALUE)
        return KAPS_INTERNAL_ERROR;

    *slot = HASH_TABLE_INVALID_VALUE;
    self->m_nCurSize--;
    self->m_nInvalidSlots++;

    /*
     * If there are too many invalid slots, then get rid of the invalid slots by reconstructing the hash table
     */
    if (self->m_nInvalidSlots > (self->m_nMaxSize / 2))
        kaps_pfx_hash_table_reconstruct(self);

    return KAPS_OK;
}

kaps_status
kaps_pfx_hash_table_expand(
    struct pfx_hash_table * self)
{
    struct kaps_entry **newslots;
    struct kaps_entry **oldslots = self->m_slots_p, **slot;
    uint32_t ix, oldsize = self->m_nMaxSize;
    uint32_t newsize = (oldsize * (100 + self->m_nResizeBy)) / 100;
    uint32_t num_valid_slots = self->m_nCurSize;
    struct kaps_allocator *alloc = self->m_alloc_p;

    newsize = kaps_find_next_prime(newsize);

    newslots = alloc->xcalloc(alloc->cookie, newsize, sizeof(struct kaps_entry *));
    if (!newslots)
        return KAPS_OUT_OF_MEMORY;

    self->m_nMaxSize = newsize;
    self->m_nThreshSize = (newsize * self->m_nThreshold) / 100;
    self->m_slots_p = newslots;
    self->m_nCurSize = 0;
    self->m_nInvalidSlots = 0;

    slot = oldslots;
    for (ix = 0; ix < oldsize; ix++, slot++)
    {
        if (*slot != HASH_TABLE_NULL_VALUE && *slot != HASH_TABLE_INVALID_VALUE)
        {
            kaps_pfx_hash_table_insert(self, *slot);
        }
    }

    (void) num_valid_slots;
    kaps_sassert(num_valid_slots == self->m_nCurSize);

    alloc->xfree(alloc->cookie, (void *) oldslots);

    return KAPS_OK;
}

kaps_status
kaps_pfx_hash_table_reconstruct(
    struct pfx_hash_table * self)
{
    struct kaps_entry **newslots;
    struct kaps_entry **oldslots = self->m_slots_p, **slot;
    uint32_t ix, original_size = self->m_nMaxSize;
    uint32_t num_valid_slots = self->m_nCurSize;
    struct kaps_allocator *alloc = self->m_alloc_p;

    newslots = alloc->xcalloc(alloc->cookie, original_size, sizeof(struct kaps_entry *));
    if (!newslots)
        return KAPS_OUT_OF_MEMORY;

    /*
     * Maxsize and threshold size will not change
     */
    self->m_slots_p = newslots;
    self->m_nCurSize = 0;
    self->m_nInvalidSlots = 0;

    slot = oldslots;
    for (ix = 0; ix < original_size; ix++, slot++)
    {
        if (*slot != HASH_TABLE_NULL_VALUE && *slot != HASH_TABLE_INVALID_VALUE)
        {
            kaps_pfx_hash_table_insert(self, *slot);
        }
    }

    (void) num_valid_slots;
    kaps_sassert(num_valid_slots == self->m_nCurSize);

    alloc->xfree(alloc->cookie, (void *) oldslots);

    return KAPS_OK;
}
