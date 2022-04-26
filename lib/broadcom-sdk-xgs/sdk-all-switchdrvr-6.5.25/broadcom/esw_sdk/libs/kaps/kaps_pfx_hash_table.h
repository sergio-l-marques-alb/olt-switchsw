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

#ifndef __KAPS_PFX_HASH_TABLE_H
#define __KAPS_PFX_HASH_TABLE_H

#include "kaps_errors.h"
#include "kaps_portable.h"
#include "kaps_allocator.h"
#include "kaps_db_internal.h"
#include "kaps_device.h"

/**
 * @file pfx_hash_table.h
 *
 * Internal details and structures used for hash table for storing prefixes
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Represents an unused slot in the hash table
 */
#define HASH_TABLE_NULL_VALUE ( (struct kaps_entry *)0)

/**
 * Represents a slot that has been deleted in the hash table
 */
#define HASH_TABLE_INVALID_VALUE ( (struct kaps_entry *)1)

/**
 * @brief Represents the hash table used for storing prefixes
 */
    struct pfx_hash_table
    {
        struct kaps_entry **m_slots_p;  /**< The actual slots for storing the entries*/
        struct kaps_allocator *m_alloc_p;
                                        /**< The allocator to use */
        struct kaps_db *m_db;           /**< Type of the database */
        uint32_t m_nCurSize;           /**< Number of occupied slots */
        uint32_t m_nMaxSize;           /**< Maximum number of slots (free + occupied)*/
        uint32_t m_nThreshSize;        /**< When the current size becomes equal to thresh size, a resize occurs*/
        uint16_t m_nThreshold;         /**< Percent of max size which triggers a resize*/
        uint16_t m_nResizeBy;          /**< Percent to grow by when a resize is triggered*/
        uint32_t m_nInvalidSlots;      /**< Num of Invalid (deleted) slots */
        uint32_t m_nMaxPfxWidth_1;     /**< Maximum width of the prefixes added to hash table*/
        uint32_t m_doffset;            /**< Data offset */
    };

/**
 * Creates the hash table for storing prefixes
 *
 * @param alloc allocator handle for memory allocation
 * @param initsz initial number of slots in the hash table. As more members get add, resize automatically occurs
 * @param thresh percent of max size of hash table which triggers a resize
 * @param resizeby percent of max size by which hash table should grow when a resize is triggered
 * @param max_pfx_width_1 maximum width of the prefix in bits that can be added
 * @param d_offset the byte offset if any to use to de-reference the data in the entry
 * @param db data base handle (whether ACL or LPM)
 * @param self_pp handle to the prefix hash table is returned here
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_pfx_hash_table_create(
    struct kaps_allocator *alloc,
    uint32_t initsz,
    uint32_t thresh,
    uint32_t resizeby,
    uint32_t max_pfx_width_1,
    uint32_t d_offset,
    struct kaps_db *db,
    struct pfx_hash_table **self_pp);
/**
 * Destroys the hash table
 *
 * @param self handle to the initialized hash table
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_pfx_hash_table_destroy(
    struct pfx_hash_table *self);

/**
 * Inserts an entry to the hash table without checking for duplicates. If the user wants to ensure that only
 * unique entries are there in the hash table the user has to first explicitly locate the entry being inserted,
 * ensure that it is not in the hash table and then insert it
 *
 * @param self handle to the initialized hash table
 * @param e handle to the entry to be inserted
 *
 * @return KAPS_OK on success or KAPS_OUT_OF_MEMORY
 */
    kaps_status kaps_pfx_hash_table_insert(
    struct pfx_hash_table *self,
    struct kaps_entry *e);

/*
  * Searches the data in the hash table and returns the handle to the entry in the hash table
  *
  * @param self handle to the initialized hash table
  * @param data the data bytes of the prefix to be searched in the hash table
  * @param len_1 length of the data in bits
  * @param ret handle to the entry in the hash table is returned here. If no match is found NULL is returned
  *
  * @return KAPS_OK on success or an error code
  */
    kaps_status kaps_pfx_hash_table_locate(
    struct pfx_hash_table *self,
    uint8_t * data,
    uint32_t len_1,
    struct kaps_entry ***ret);

/*
 * Deletes an entry from the hash table.
 *
 * @param self handle to the initialized hash table
 * @param slot handle to the entry to be deleted. This handle must be obtained by first doing a kaps_pfx_hash_table_locate
 *                  on the entry. There should be no other operation (including no new locate operation) between
 *                  the call to locate the entry and the call to delete entry.
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_pfx_hash_table_delete(
    struct pfx_hash_table *self,
    struct kaps_entry **slot);

/*
  *  Increases the size of the hash table
  *
  * @param self handle to the initialized hash table
  *
  * @return KAPS_OK on success or an error code
  */
    kaps_status kaps_pfx_hash_table_expand(
    struct pfx_hash_table *self);

/*
  *  Gets rid of the invalid slots in the hash table and keeps the max size of the hash table intact
  *
  * @param self handle to the initialized hash table
  *
  * @return KAPS_OK on success or an error code
  */
    kaps_status kaps_pfx_hash_table_reconstruct(
    struct pfx_hash_table *self);

#ifdef __cplusplus
}
#endif

#endif
