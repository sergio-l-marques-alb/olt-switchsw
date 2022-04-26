/*******************************************************************************
 *
 * Copyright 2014-2019 Broadcom Corporation
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

#ifndef __KAPS_HANDLE_H
#define __KAPS_HANDLE_H

#include <stdint.h>
#include "kaps_errors.h"
#include "kaps_legacy.h"
#include "kaps_device.h"
#include "kaps_portable.h"
#include "kaps_device_internal.h"
#include "kaps_db_internal.h"
#include "kaps_instruction_internal.h"
#include "kaps_ad_internal.h"
#ifdef __cplusplus
extern "C"
{
#endif

/** The handle table is an array of uint32_t. After KAPS_WB_HANDLE_TABLE_FREELIST_INIT is called
    the table is as follows
      idx
         _________________
      0 |    FFFFFFFF     |
        |-----------------|
      1 |       0         |
        |-----------------|
      2 |       0         |
        |-----------------|
      3 |       0         |
        |-----------------|
      4 |       0         |
        |-----------------|
      5 |       0         |
        |-----------------|
      6 |       0         |
        |-----------------|

    The slot 0 is not used to store handle. After init it hold the special value FFFFFFFF to
    indicate that free list has not been initialised.

    In case of warmboot restore index based insertions can be done at this time.
    After the restore entry insertions the table looks as below.

   idx
         _________________
      0 |    FFFFFFFF     |
        |-----------------|
      1 | <entry pointer> |
        |-----------------|
      2 |       0         |
        |-----------------|
      3 | <entry pointer> |
        |-----------------|
      4 |       0         |
        |-----------------|
      5 |       0         |
        |-----------------|
      6 | <entry pointer> |
        |-----------------|

     When KAPS_WB_HANDLE_TABLE_FREELIST_INIT is called the unfilled entries are chained as a linked list.
     Slot 0 now points to the first such entry. Index based insertions (KAPS_WB_HANDLE_WRITE_LOC) are not possible
     after this.

   idx
         _________________
      0 |       2         | <Free Pointer
        |-----------------|
      1 | <entry pointer> |
        |-----------------|
      2 |       4         |
        |-----------------|
      3 | <entry pointer> |
        |-----------------|
      4 |       5         |
        |-----------------|
      5 |       7         |
        |-----------------|
      6 | <entry pointer> |
        |-----------------|

     Entries can be inserted using KAPS_WB_HANDLE_GET_FREE_AND_WRITE. This call fills an empty slot with the
     incoming entry and updates the slot 0 to point to the next entry as below.

idx
         _________________
      0 |       4         |
        |-----------------|
      1 | <entry pointer> |
        |-----------------|
      2 | <entry pointer> |
        |-----------------|
      3 | <entry pointer> |
        |-----------------|
      4 |       5         |
        |-----------------|
      5 |       7         |
        |-----------------|
      6 | <entry pointer> |
        |-----------------|

    Entry deletions are handled by inserting the new free slot into the head of the linked list. so if
    KAPS_WB_HANDLE_DELETE_LOC is called with userhandle 6 the table becomes

idx
         _________________
      0 |       6         |
        |-----------------|
      1 | <entry pointer> |
        |-----------------|
      2 | <entry pointer> |
        |-----------------|
      3 | <entry pointer> |
        |-----------------|
      4 |       5         |
        |-----------------|
      5 |       7         |
        |-----------------|
      6 |       4         |
        |-----------------|

    When free entry indices are stored in the non-0 slots, they are left shifted by 4 and or-ed with 1.
    The LSB bits being 0001 is used as an indication that this slot is free.

 */

/**
 * Resize the user handle table to about 1.5X the current size
 *
 * @param db Valid database handle.
 */
    kaps_status kaps_db_user_handle_table_realloc(
    struct kaps_db *db);

/**
 * Initializes the user handle table
 *
 * @param alloc Valid non-NULL allocator handle for portability.
 * @param db Valid database handle.
 * @param capacity The minimum required capacity for this database.
 */
    kaps_status kaps_db_user_handle_table_init(
    struct kaps_allocator *alloc,
    struct kaps_db *db,
    uint32_t capacity);

/**
 * Locates a free handle and marks it as used
 *
 * @param user_handle.valid free.slot from the bitmap
 * @param db Valid database handle.
 */
    kaps_status kaps_db_get_user_handle(
    uint32_t * user_handle,
    struct kaps_db *db);

/**
 * Initialises the Free list in the user handle. Index based write to the table is not possible after this call.
 *
 * @param db Valid database handle.
 */
    kaps_status kaps_db_user_handle_update_freelist(
    struct kaps_db *db);

#define FREE_PTR(db) ((db)->common_info->user_hdl_to_entry[0])
#define FREE_MASK (0xF)
#define HANDLE_MAX_TABLE_SIZE (0x7FFFFFF)

#define KAPS_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, user_handle) \
    (((((uintptr_t)db->common_info->user_hdl_to_entry[user_handle]) & FREE_MASK) == 0x1) ? 1 : 0) \

#define KAPS_WB_HANDLE_WRITE_LOC(db, wb_handle, usr_hdl)                               \
    do {                                                                              \
        if (db->device->flags & KAPS_DEVICE_ISSU) {                                    \
            while (usr_hdl > (*db->common_info->user_hdl_table_size)) {               \
                KAPS_WB_HANDLE_TABLE_GROW(db);                                         \
            }                                                                         \
            if ((FREE_PTR(db) == (void *)0xFFFFFFFF))                                  \
                kaps_sassert(db->common_info->user_hdl_to_entry[usr_hdl] == NULL);     \
            else if ((((uintptr_t)db->common_info->user_hdl_to_entry[usr_hdl]) & FREE_MASK) != 0x1)  \
                return KAPS_DB_WB_LOCATION_BUSY;                                       \
            db->common_info->user_hdl_to_entry[usr_hdl] = wb_handle;                  \
            wb_handle->user_handle = usr_hdl;                                         \
        }                                                                                 \
    } while (0)

#define KAPS_WB_HANDLE_UPDATE_LOC(db, wb_handle, user_handle)                              \
    do {                                                                                  \
        if (db->device->flags & KAPS_DEVICE_ISSU) {                                        \
            kaps_sassert((((uintptr_t)db->common_info->user_hdl_to_entry[user_handle]) & FREE_MASK) != 0x1);  \
            db->common_info->user_hdl_to_entry[user_handle] = wb_handle;                  \
            wb_handle->user_handle = user_handle;                                         \
        }                                                                                 \
    } while (0)

#define KAPS_WB_HANDLE_READ_LOC(db, wb_handle, user_handle)                                \
    do {                                                                                  \
        if (db->device->flags & KAPS_DEVICE_ISSU){                                         \
            *wb_handle = db->common_info->user_hdl_to_entry[user_handle];                 \
            if (((uintptr_t)(*wb_handle)) & 0x1)                                                         \
                *wb_handle = NULL;                                                        \
        }                                                                                 \
        else                                                                              \
            *wb_handle = (void *)user_handle;                                             \
    } while (0)

#define KAPS_WB_HANDLE_DELETE_LOC(db, user_handle)                                         \
    do {                                                                                  \
        if ((db)->device->flags & KAPS_DEVICE_ISSU) {                                      \
            kaps_sassert((((uintptr_t)(db)->common_info->user_hdl_to_entry[(uint32_t)user_handle]) & FREE_MASK) != 0x1); \
            if ((FREE_PTR(db) == (void *)0xFFFFFFFF)) {                                    \
                (db)->common_info->user_hdl_to_entry[(uint32_t)user_handle] = NULL;       \
            } else {                                                                      \
                (db)->common_info->user_hdl_to_entry[(uint32_t)user_handle] =(void *) ((uintptr_t)FREE_PTR(db) << 4 | 0x1); \
                FREE_PTR(db) = (void *)user_handle;                                       \
                kaps_sassert(((uintptr_t)FREE_PTR(db)) <= (uintptr_t)(*((db)->common_info->user_hdl_table_size))); \
            }                                                                             \
        }                                                                                 \
    } while (0)

#define KAPS_WB_HANDLE_TABLE_DESTROY(db)                                                   \
    do {                                                                                  \
        if ((db)->device->flags & KAPS_DEVICE_ISSU) {                                      \
            (db)->device->alloc->xfree((db)->device->alloc->cookie, (db)->common_info->user_hdl_table_size);   \
            (db)->device->alloc->xfree((db)->device->alloc->cookie, (db)->common_info->user_hdl_to_entry);   \
        }                                                                                 \
    } while (0)                                                                            \

#define KAPS_WB_HANDLE_TABLE_GROW(db)                                                      \
    do {                                                                                  \
        if (db->device->flags & KAPS_DEVICE_ISSU)                                          \
            KAPS_STRY(kaps_db_user_handle_table_realloc(db));\
    } while (0)                                                                             \

#define KAPS_WB_HANDLE_TABLE_INIT(db, capacity)                                             \
    do {                                                                                  \
        if (db->device->flags & KAPS_DEVICE_ISSU)                                          \
            KAPS_STRY(kaps_db_user_handle_table_init(db->device->alloc, db, capacity));     \
    } while (0)                                                                             \

#define KAPS_WB_HANDLE_TABLE_FREELIST_INIT(db)                                             \
    do {                                                                                  \
        if ((db)->device->flags & KAPS_DEVICE_ISSU) {                                      \
            KAPS_STRY(kaps_db_user_handle_update_freelist(db));                             \
            kaps_sassert(((uintptr_t)FREE_PTR(db)) <= (uintptr_t)(*((db)->common_info->user_hdl_table_size))); \
        }                                                                                 \
    } while (0)

#define KAPS_WB_HANDLE_GET_FREE_AND_WRITE(db, entry)                          \
    do {                                                                                  \
        if (db->device->flags & KAPS_DEVICE_ISSU) {                                        \
            uint32_t user_handle = 0;                                                     \
            KAPS_STRY(kaps_db_get_user_handle((&user_handle), db));                          \
            entry->user_handle = user_handle;                                              \
            kaps_sassert(((uintptr_t)FREE_PTR(db)) <= (uintptr_t)(*((db)->common_info->user_hdl_table_size))); \
            db->common_info->user_hdl_to_entry[user_handle] = (void *)(entry);            \
            entry = (void *) ((uintptr_t)user_handle);                                    \
        }                                                                                 \
    } while (0)

#define KAPS_WB_HANDLE_WRITE(db, entry, user_handle)                          \
            do {                                                                                  \
                if (db->device->flags & KAPS_DEVICE_ISSU) {                                        \
                    entry->user_handle = user_handle;                                              \
                    kaps_sassert(((uintptr_t)FREE_PTR(db)) <= (uintptr_t)(*((db)->common_info->user_hdl_table_size))); \
                    db->common_info->user_hdl_to_entry[user_handle] = (void *)(entry);            \
                }                                                                                 \
            } while (0)

#define KAPS_WB_CONVERT_TO_ENTRY_HANDLE(type, wb_handle) \
            ((struct type *)((uintptr_t)(wb_handle)))

#define KAPS_WB_HANDLE_TABLE_IS_FREELIST_INIT_DONE(db) \
        ((db->common_info->user_hdl_to_entry[0] == (void *)0xFFFFFFFF) ? 0 : 1)

#ifdef __cplusplus
}
#endif

#endif                          /* __HANDLE_H */
