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
#include <stdlib.h>
#include "kaps_handle.h"

kaps_status
kaps_db_user_handle_table_realloc(
    struct kaps_db *db)
{
    void **tmp;
    uintptr_t iter;
    uintptr_t curr_size = *db->common_info->user_hdl_table_size + 1;
    uintptr_t size;
    uintptr_t *freeptr;

    kaps_sassert(curr_size < HANDLE_MAX_TABLE_SIZE);    /* Already at max size */
    size = curr_size * 1.5;
    if (size > HANDLE_MAX_TABLE_SIZE)
        size = HANDLE_MAX_TABLE_SIZE;
    if (size < 4096)
        size = 4096;

    tmp = db->device->alloc->xcalloc(db->device->alloc->cookie, size, sizeof(void *));

    if (!tmp)
        return KAPS_OUT_OF_MEMORY;

    freeptr = (uintptr_t *) & tmp[0];

    /*
     * Copy the existing data. memcpy might be faster? 
     */
    for (iter = 0; iter < curr_size; iter++)
    {
        tmp[iter] = db->common_info->user_hdl_to_entry[iter];
    }

    /*
     * if realloc is called during between init and update freelist donot update the freelist 
     */
    if (*freeptr != 0xFFFFFFFF)
    {
        for (iter = curr_size; iter < size - 1; iter++)
        {
            tmp[iter] = (void *) (((iter + 1) << 4) | 0x1);
        }
        tmp[size - 1] = (void *) (((*freeptr) << 4) | 0x1);
        *freeptr = curr_size;
    }

    *db->common_info->user_hdl_table_size = size - 1;
    db->device->alloc->xfree(db->device->alloc->cookie, db->common_info->user_hdl_to_entry);
    db->common_info->user_hdl_to_entry = tmp;

    /*
     * In case of multiple ad db for same main db, update user_hdl_to_entry point to same table 
     */
    if (db->type == KAPS_DB_AD)
    {
        struct kaps_db *main_db = db->common_info->ad_info.db;
        struct kaps_ad_db *ad_db = (struct kaps_ad_db *) main_db->common_info->ad_info.ad;

        while (ad_db)
        {
            ad_db->db_info.common_info->user_hdl_table_size = db->common_info->user_hdl_table_size;
            ad_db->db_info.common_info->user_hdl_to_entry = db->common_info->user_hdl_to_entry;
            ad_db = ad_db->next;
        }
    }
    return KAPS_OK;
}

kaps_status
kaps_db_user_handle_table_init(
    struct kaps_allocator * alloc,
    struct kaps_db * db,
    uint32_t capacity)
{
    uint32_t size;

    kaps_sassert(capacity < HANDLE_MAX_TABLE_SIZE);     /* 4 lsb bits are used to distinguish between free handle and
                                                         * entry pointer */

    size = capacity;

    if (size > HANDLE_MAX_TABLE_SIZE)
        size = HANDLE_MAX_TABLE_SIZE;

    if (size == 0)
        size = 4096;

    db->common_info->user_hdl_table_size = alloc->xcalloc(alloc->cookie, 1, sizeof(uint32_t));
    if (!db->common_info->user_hdl_table_size)
        return KAPS_OUT_OF_MEMORY;

    db->common_info->user_hdl_to_entry = alloc->xcalloc(alloc->cookie, size, sizeof(void *));
    if (!db->common_info->user_hdl_to_entry)
    {
        alloc->xfree(alloc->cookie, db->common_info->user_hdl_table_size);
        return KAPS_OUT_OF_MEMORY;
    }

    *db->common_info->user_hdl_table_size = size - 1;   /* one space used for free ptr */
    db->common_info->user_hdl_to_entry[0] = (void *) 0xFFFFFFFF;        /* Free ptr */
    return KAPS_OK;
}

kaps_status
kaps_db_user_handle_update_freelist(
    struct kaps_db * db)
{
    void **entry_pp = db->common_info->user_hdl_to_entry;
    uint32_t i = *db->common_info->user_hdl_table_size; /* last index */
    uintptr_t *freeptr = (uintptr_t *) & entry_pp[0];

    kaps_sassert(db->common_info->user_hdl_to_entry[0] == (void *) 0xFFFFFFFF);

    *freeptr = 0;
    while (i > 0)
    {
        if (entry_pp[i] == NULL)
        {
            entry_pp[i] = (void *) ((*freeptr) << 4 | 0x1);
            *freeptr = i;
        }
        i--;
    }

    return KAPS_OK;
}

kaps_status
kaps_db_get_user_handle(
    uint32_t * user_handle,
    struct kaps_db * db)
{
    uintptr_t *freeptr = (uintptr_t *) & db->common_info->user_hdl_to_entry[0];

    if (db->common_info->insertion_mode == KAPS_DB_INSERTION_MODE_ENTRY_WITH_INDEX)
    {
        uintptr_t iter = *db->common_info->user_hdl_table_size;

        for (; iter > 0; iter--)
        {
            if (KAPS_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, iter))
                break;
        }

        if (iter != 0)
        {
            *user_handle = (uint32_t) iter;
            return KAPS_OK;
        }

        KAPS_STRY(kaps_db_user_handle_table_realloc(db));
        iter = *db->common_info->user_hdl_table_size;
        kaps_sassert(KAPS_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, iter));
        *user_handle = (uint32_t) iter;
        return KAPS_OK;
    }

    if (*freeptr == 0)
    {
        KAPS_STRY(kaps_db_user_handle_table_realloc(db));
        freeptr = (uintptr_t *) & db->common_info->user_hdl_to_entry[0];
        kaps_sassert(*freeptr != 0);
    }

    *user_handle = (uint32_t) * freeptr;
    *freeptr = ((uintptr_t) db->common_info->user_hdl_to_entry[*user_handle]) >> 4;

    return KAPS_OK;
}
