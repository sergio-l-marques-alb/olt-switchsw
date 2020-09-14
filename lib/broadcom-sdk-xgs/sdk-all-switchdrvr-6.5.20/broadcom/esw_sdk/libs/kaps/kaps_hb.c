/*******************************************************************************
 *
 * Copyright 2015-2019 Broadcom Corporation
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

#include "kaps_portable.h"
#include "kaps_device_internal.h"
#include "kaps_handle.h"
#include "kaps_hb.h"









static kaps_status
kaps_hb_init(
    struct kaps_device *device,
    struct kaps_hb_db *hb_db,
    uint32_t user_capacity)
{
    /*
     * Total number of hit bits possible on the device
     */
    uint32_t capacity = KAPS_HB_ROW_WIDTH_1 * device->num_hb_blocks * device->num_rows_in_hb_block;

    if (!device->hb_buffer)
    {
        device->hb_buffer = device->alloc->xmalloc(device->alloc->cookie, capacity / KAPS_BITS_IN_BYTE);
        if (!device->hb_buffer)
        {
            device->alloc->xfree(device->alloc->cookie, hb_db);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    if (!device->aging_table)
    {
        /*
         * initialize the aging table for the whole device and not just for one database
         */
        device->aging_table = device->alloc->xcalloc(device->alloc->cookie, capacity, sizeof(struct kaps_aging_entry));

        device->aging_table_size = capacity;
    }

    if (!device->aging_table)
    {
        device->alloc->xfree(device->alloc->cookie, device->hb_buffer);
        device->alloc->xfree(device->alloc->cookie, hb_db);
        return KAPS_OUT_OF_MEMORY;
    }

    if (device->num_small_hb_blocks && !device->small_aging_table)
    {
        capacity = KAPS_HB_ROW_WIDTH_1 * device->num_small_hb_blocks * device->num_rows_in_small_hb_block; 

        device->small_aging_table = device->alloc->xcalloc(device->alloc->cookie, capacity, sizeof(struct kaps_aging_entry));

        device->small_aging_table_size = capacity;

        if (!device->small_aging_table)
        {
            device->alloc->xfree(device->alloc->cookie, device->aging_table);
            device->alloc->xfree(device->alloc->cookie, device->hb_buffer);
            device->alloc->xfree(device->alloc->cookie, hb_db);
            return KAPS_OUT_OF_MEMORY;
        }

    }
  

    POOL_INIT(kaps_hb, &hb_db->kaps_hb_pool, device->alloc);

    KAPS_WB_HANDLE_TABLE_INIT((&hb_db->db_info), user_capacity);
    if (device->flags & KAPS_DEVICE_ISSU)
    {
        if (device->issu_status == KAPS_ISSU_RESTORE_START)
            return KAPS_OK;
    }

    kaps_db_user_handle_update_freelist(&hb_db->db_info);

    return KAPS_OK;
}

kaps_status
kaps_kaps_hb_cr_init(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * cb_fun)
{
    uint32_t capacity = KAPS_HB_ROW_WIDTH_1 * device->num_hb_blocks * device->num_rows_in_hb_block;

    device->alloc->xfree(device->alloc->cookie, device->hb_buffer);
    device->alloc->xfree(device->alloc->cookie, device->aging_table);

    if (cb_fun->nv_ptr)
    {
        device->hb_buffer = (uint8_t *) cb_fun->nv_ptr;
        cb_fun->nv_ptr = (void *) ((uint8_t *) cb_fun->nv_ptr + capacity / KAPS_BITS_IN_BYTE);
        *cb_fun->nv_offset += capacity / KAPS_BITS_IN_BYTE;
    }

    if (cb_fun->nv_ptr)
    {
        device->aging_table = (struct kaps_aging_entry *) cb_fun->nv_ptr;
        cb_fun->nv_ptr = (void *) ((uint8_t *) cb_fun->nv_ptr + (capacity * sizeof(struct kaps_aging_entry)));
        *cb_fun->nv_offset += (capacity * sizeof(struct kaps_aging_entry));
    }
    return KAPS_OK;
}

kaps_status
kaps_hb_db_init(
    struct kaps_device * device,
    uint32_t id,
    uint32_t capacity,
    struct kaps_hb_db ** hb_dbp)
{
    struct kaps_hb_db *hb_db = NULL;
    kaps_status status = KAPS_OK;
    uint32_t is_supported;

    KAPS_TRACE_IN("%p %u %u %p\n", device, id, capacity, hb_dbp);
    if (!hb_dbp)
        return KAPS_INVALID_ARGUMENT;

    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    if (device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    /*
     * We are using this API internally during Restore device info
     */
    if (device->flags & KAPS_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_ISSU_INIT && device->issu_status != KAPS_ISSU_RESTORE_START)
            return KAPS_ISSU_IN_PROGRESS;
    }

    is_supported = 0;
    if (device->type == KAPS_DEVICE_KAPS && device->num_hb_blocks > 0)
    {
        is_supported = 1;
    }

    if (!is_supported)
        return KAPS_UNSUPPORTED;

    if (capacity == 0)
        return KAPS_DYNAMIC_UNSUPPORTED;

    hb_db = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_hb_db));
    if (!hb_db)
    {
        return KAPS_OUT_OF_MEMORY;
    }

    hb_db->capacity = capacity;
    hb_db->alloc = device->alloc;

    status = kaps_resource_add_database(device, &hb_db->db_info, id, capacity, 0, KAPS_DB_HB);

    if (status != KAPS_OK)
    {
        device->alloc->xfree(device->alloc->cookie, hb_db);
        return status;
    }

    if (device->type == KAPS_DEVICE_KAPS)
    {
        KAPS_STRY(kaps_hb_init(device, hb_db, capacity));
    }

    *hb_dbp = hb_db;
    KAPS_TRACE_OUT("%p\n", *hb_dbp);

    return status;
}

kaps_status
kaps_hb_db_destroy(
    struct kaps_hb_db * hb_db)
{
    KAPS_TRACE_IN("%p\n", hb_db);
    if (!hb_db)
        return KAPS_INVALID_ARGUMENT;

    /*
     * Check if database is still linked to DB list
     * and free it
     */
    {
        struct kaps_device *main_device;
        struct kaps_c_list_iter it;
        struct kaps_list_node *el;

        main_device = hb_db->db_info.device;
        if (main_device->main_dev)
            main_device = main_device->main_dev;
        kaps_c_list_iter_init(&main_device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(el);

            if (tmp == &hb_db->db_info)
            {
                kaps_c_list_remove_node(&main_device->db_list, el, &it);
                break;
            }
        }
    }

    KAPS_STRY(kaps_hb_db_delete_all_entries(hb_db));
    POOL_FINI(kaps_hb, &hb_db->kaps_hb_pool);

    KAPS_WB_HANDLE_TABLE_DESTROY((&hb_db->db_info));
    kaps_resource_free_database(hb_db->db_info.device, &hb_db->db_info);

    hb_db->alloc->xfree(hb_db->alloc->cookie, hb_db);
    return KAPS_OK;
}

kaps_status
kaps_hb_db_add_entry(
    struct kaps_hb_db * hb_db,
    struct kaps_hb ** hb)
{
    struct kaps_hb *hb_p;

    KAPS_TRACE_IN("%p %p\n", hb_db, hb);
    if (!hb_db || !hb)
        return KAPS_INVALID_ARGUMENT;

    if (hb_db->db_info.device->flags & KAPS_DEVICE_ISSU)
    {
        if (hb_db->db_info.device->issu_status != KAPS_ISSU_INIT)
            return KAPS_ISSU_IN_PROGRESS;
    }

    if (!hb_db->db_info.device->is_config_locked)
        return KAPS_DEVICE_UNLOCKED;

    POOL_ALLOC(kaps_hb, &hb_db->kaps_hb_pool, hb_p);
    if (!hb_p)
        return KAPS_OUT_OF_MEMORY;

    kaps_memset(hb_p, 0, sizeof(*hb_p));

    kaps_c_list_add_tail(&hb_db->hb_list, &hb_p->hb_node);
    hb_p->hb_db_seq_num = hb_db->db_info.seq_num;
    *hb = hb_p;

    KAPS_WB_HANDLE_GET_FREE_AND_WRITE((&(hb_db->db_info)), (*hb));
    KAPS_TRACE_OUT("%p\n", *hb);

    return KAPS_OK;
}

static kaps_status
kaps_hb_db_delete_entry_internal(
    struct kaps_hb_db *hb_db,
    struct kaps_hb *hb,
    struct kaps_c_list_iter *it)
{
    struct kaps_hb *hb_p = NULL;

    if (!hb_db || !hb)
        return KAPS_INVALID_ARGUMENT;

    KAPS_WB_HANDLE_READ_LOC((&hb_db->db_info), (&hb_p), (uintptr_t) hb);

    if (!hb_p)
        return KAPS_INVALID_ARGUMENT;

    if (hb_p->entry)
        return KAPS_INVALID_ARGUMENT;   

    if (!hb_db->db_info.device->is_config_locked)
    {
        if (hb_db->db_info.device->issu_status != KAPS_ISSU_RECONCILE_END)
            return KAPS_DEVICE_UNLOCKED;
    }

    kaps_c_list_remove_node(&hb_db->hb_list, &hb_p->hb_node, it);

    POOL_FREE(kaps_hb, &hb_db->kaps_hb_pool, hb_p);

    KAPS_WB_HANDLE_DELETE_LOC((&hb_db->db_info), (uintptr_t) hb);

    return KAPS_OK;
}

kaps_status
kaps_hb_db_delete_entry(
    struct kaps_hb_db * hb_db,
    struct kaps_hb * hb)
{
    KAPS_TRACE_IN("%p %p\n", hb_db, hb);
    return kaps_hb_db_delete_entry_internal(hb_db, hb, NULL);
}

kaps_status
kaps_hb_db_delete_all_entries(
    struct kaps_hb_db * hb_db)
{
    KAPS_TRACE_IN("%p\n", hb_db);
    if (!hb_db)
        return KAPS_INVALID_ARGUMENT;

    do
    {
        struct kaps_list_node *el;
        struct kaps_hb *e;

        el = kaps_c_list_head(&hb_db->hb_list);
        if (el == NULL)
            break;
        e = KAPS_DBLIST_TO_HB_ENTRY(el);

        if (hb_db->db_info.device->flags & KAPS_DEVICE_ISSU)
        {
            KAPS_STRY(kaps_hb_db_delete_entry(hb_db, (struct kaps_hb *) (uintptr_t) e->user_handle));
        }
        else
        {
            KAPS_STRY(kaps_hb_db_delete_entry(hb_db, e));
        }
    }
    while (1);

    return KAPS_OK;
}

kaps_status
kaps_hb_db_delete_unused_entries(
    struct kaps_hb_db * hb_db)
{
    struct kaps_list_node *el;
    struct kaps_hb *e;
    struct kaps_c_list_iter it;

    KAPS_TRACE_IN("%p\n", hb_db);
    if (!hb_db)
        return KAPS_INVALID_ARGUMENT;

    kaps_c_list_iter_init(&hb_db->hb_list, &it);

    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        e = KAPS_DBLIST_TO_HB_ENTRY(el);

        if (!e->entry)
        {
            if (hb_db->db_info.device->flags & KAPS_DEVICE_ISSU)
            {
                KAPS_STRY(kaps_hb_db_delete_entry_internal(hb_db, (struct kaps_hb *) (uintptr_t) e->user_handle, &it));
            }
            else
            {
                KAPS_STRY(kaps_hb_db_delete_entry_internal(hb_db, e, &it));
            }
        }
    }

    return KAPS_OK;
}

static uint32_t
kaps2_find_num_sub_blocks_dumped(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t main_bb_nr)
{
    uint32_t num_sub_blocks_dumped = 0;

    if (db->is_type_a)
    {
        if (device->kaps2_hb_config[main_bb_nr] == KAPS2_HB_CONFIG_A480_B000)
        {
            num_sub_blocks_dumped = 1;
        }

        if (device->kaps2_hb_config[main_bb_nr] == KAPS2_HB_CONFIG_A960_B000
            || device->kaps2_hb_config[main_bb_nr] == KAPS2_HB_CONFIG_A480_B480)
        {
            num_sub_blocks_dumped = 2;
        }
    }
    else
    {
        /*
         * Type B database
         */
        if (device->kaps2_hb_config[main_bb_nr] == KAPS2_HB_CONFIG_A000_B480)
        {
            num_sub_blocks_dumped = 1;
        }

        if (device->kaps2_hb_config[main_bb_nr] == KAPS2_HB_CONFIG_A000_B960
            || device->kaps2_hb_config[main_bb_nr] == KAPS2_HB_CONFIG_A480_B480)
        {
            num_sub_blocks_dumped = 2;
        }
    }

    return num_sub_blocks_dumped;
}

kaps_status
kaps_hb_db_timer(
    struct kaps_hb_db * hb_db)
{
    struct kaps_db *db = hb_db->db_info.common_info->hb_info.db;
    struct kaps_device *device = hb_db->db_info.device;
    uint32_t start_hb_blk, end_hb_blk, num_hb_blks;
    uint32_t cur_blk, j, cur_row, offset, cur_hb_nr;
    uint16_t hb_word;
    uint32_t loop_cnt, max_loop_cnt;
    uint32_t main_bb_nr;
    uint32_t start_offset_for_odd_hb_blk = 0;
    uint32_t num_sub_blocks_dumped;
    uint32_t offset_till_previous_blk = 0;
    uint32_t is_hb_blk_enabled_for_db;
    uint32_t final_level_offset = kaps_device_get_final_level_offset(device, db);
    struct kaps_aging_entry *active_aging_table = kaps_device_get_active_aging_table(device, db);
    uint32_t num_active_hb_blocks = kaps_device_get_active_num_hb_blocks(device, db);
    uint32_t num_active_rows_in_hb_block = kaps_device_get_active_num_hb_rows(device, db);

    KAPS_TRACE_IN("%p\n", hb_db);

    if (device->id == KAPS_JERICHO_2_DEVICE_ID && db->num_algo_levels_in_db == 3)
    {
        offset = 0;
        for (main_bb_nr = 0; main_bb_nr < num_active_hb_blocks; main_bb_nr += 2)
        {
            num_sub_blocks_dumped = kaps2_find_num_sub_blocks_dumped(device, db, main_bb_nr);

            if (num_sub_blocks_dumped)
            {
                cur_blk = main_bb_nr / 2;
                KAPS_STRY(kaps_dm_kaps_hb_dump(device, db, final_level_offset + cur_blk, 0,
                                               final_level_offset + cur_blk,
                                               device->num_rows_in_each_hb_block[main_bb_nr] - 1,
                                               1, &device->hb_buffer[offset]));

                offset += (KAPS_HB_ROW_WIDTH_8 * device->num_rows_in_each_hb_block[main_bb_nr]);

                if (num_sub_blocks_dumped == 2)
                {
                    offset += (KAPS_HB_ROW_WIDTH_8 * device->num_rows_in_each_hb_block[main_bb_nr]);
                }
            }
        }

        offset = 0;
        for (main_bb_nr = 0; main_bb_nr < num_active_hb_blocks; main_bb_nr += 2)
        {

            num_sub_blocks_dumped = kaps2_find_num_sub_blocks_dumped(device, db, main_bb_nr);

            if (num_sub_blocks_dumped == 0)
            {
                continue;
            }

            offset_till_previous_blk = offset;

            /*
             * If loop_cnt is 0, we are in an even numbered hit bit block. Save the offset for the odd numbered hit bit 
             * block in case we need to use it 
             */
            start_offset_for_odd_hb_blk = offset + KAPS_HB_ROW_WIDTH_8;

            for (loop_cnt = 0; loop_cnt < 2; ++loop_cnt)
            {
                cur_blk = main_bb_nr + loop_cnt;

                is_hb_blk_enabled_for_db = 1;
                if (!(db->common_info->kaps2_large_hb_blk_bmp & (1u << cur_blk)))
                {
                    is_hb_blk_enabled_for_db = 0;
                    if (device->kaps2_hb_config[main_bb_nr] == KAPS2_HB_CONFIG_A480_B480)
                    {
                        /*
                         * If we have A480_B480 configuration we have to read the entries in the BB for the other db as 
                         * well since after the hit bit dump, we will be clearing the entire BB (both the A480 and the
                         * B480. So don't continue. Instead go ahead further
                         */
                    }
                    else
                    {
                        continue;
                    }
                }

                if (loop_cnt == 1)
                {
                    /*
                     * If loop_cnt is 1, then we are in an odd numbered hit bit block. Load the offset from the offset
                     * we have saved for the hit bit block
                     */
                    offset = start_offset_for_odd_hb_blk;
                }

                for (cur_row = 0; cur_row < device->num_rows_in_each_hb_block[main_bb_nr]; ++cur_row)
                {
                    cur_hb_nr =
                        (cur_row * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1) + (cur_blk * KAPS_HB_ROW_WIDTH_1);

                    hb_word =
                        KapsReadBitsInArrray(&device->hb_buffer[offset], KAPS_HB_ROW_WIDTH_8, KAPS_HB_ROW_WIDTH_1 - 1, 0);

                    for (j = 0; j < KAPS_HB_ROW_WIDTH_1; ++j)
                    {
                        if (hb_word & (1u << j))
                        {
                            if (is_hb_blk_enabled_for_db)
                            {
                                active_aging_table[cur_hb_nr].num_idles = 0;
                            }
                            else
                            {
                                /*
                                 * Suppose we have A480_B480, then for the block of the other DB, we have to store the
                                 * hardware hit bit value during the hb_dump in the ageing table
                                 */
                                active_aging_table[cur_hb_nr].intermediate_hb_val = 1;
                            }
                        }
                        else
                        {
                            if (is_hb_blk_enabled_for_db)
                            {
                                /*
                                 * If the intermediate_hb_val has already been set for the hit bit in the ageing table, 
                                 * then we actually had a hit for this entry while dumping the entries of a different
                                 * database. So make the number of idles as 0
                                 */
                                if (active_aging_table[cur_hb_nr].intermediate_hb_val)
                                {
                                    active_aging_table[cur_hb_nr].num_idles = 0;
                                    active_aging_table[cur_hb_nr].intermediate_hb_val = 0;
                                }
                                else
                                {
                                    active_aging_table[cur_hb_nr].num_idles++;
                                }
                            }
                        }

                        cur_hb_nr++;
                    }

                    if (num_sub_blocks_dumped == 2)
                    {
                        /*
                         * The hit bits for the even and hit bit blocks are next to each other. So we need to jump by 2 
                         * * KAPS_HB_ROW_WIDTH_8
                         */
                        offset += (2 * KAPS_HB_ROW_WIDTH_8);

                    }
                    else
                    {
                        offset += KAPS_HB_ROW_WIDTH_8;
                    }
                }
            }

            offset =
                offset_till_previous_blk +
                (device->num_rows_in_each_hb_block[main_bb_nr] * num_sub_blocks_dumped * KAPS_HB_ROW_WIDTH_8);

        }

    }
    else
    {

        start_hb_blk = db->common_info->start_hb_blk;
        num_hb_blks = db->common_info->num_hb_blks;
        end_hb_blk = start_hb_blk + num_hb_blks - 1;

        max_loop_cnt = 1;
        if (device->id == KAPS_JERICHO_2_DEVICE_ID && db->has_clones)
        {
            max_loop_cnt = 2;
        }

        for (loop_cnt = 0; loop_cnt < max_loop_cnt; ++loop_cnt)
        {

            if (device->id != KAPS_QUMRAN_DEVICE_ID)
            {
                /*
                 * QAX has no hit bit dump machine
                 */
                KAPS_STRY(kaps_dm_kaps_hb_dump(device, db, final_level_offset + start_hb_blk, 0,
                                               final_level_offset + end_hb_blk, num_active_rows_in_hb_block - 1, 1,
                                               device->hb_buffer));
            }

            offset = 0;

            for (cur_blk = start_hb_blk; cur_blk <= end_hb_blk; ++cur_blk)
            {

                for (cur_row = 0; cur_row < num_active_rows_in_hb_block; ++cur_row)
                {

                    if (device->id == KAPS_QUMRAN_DEVICE_ID)
                    {
                        KAPS_STRY(kaps_dm_kaps_hb_read
                                  (device, db, final_level_offset + cur_blk, cur_row, &device->hb_buffer[offset]));
                    }

                    hb_word =
                        KapsReadBitsInArrray(&device->hb_buffer[offset], KAPS_HB_ROW_WIDTH_8, KAPS_HB_ROW_WIDTH_1 - 1, 0);

                    cur_hb_nr =
                        (cur_row * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1) + (cur_blk * KAPS_HB_ROW_WIDTH_1);

                    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
                    {
                        cur_hb_nr =
                            (cur_row * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1) +
                            ((cur_blk % num_active_hb_blocks) * KAPS_HB_ROW_WIDTH_1);
                    }

                    if (loop_cnt == 0)
                    {
                        for (j = 0; j < KAPS_HB_ROW_WIDTH_1; ++j)
                        {
                            if (hb_word & (1u << j))
                                active_aging_table[cur_hb_nr].num_idles = 0;
                            else
                                active_aging_table[cur_hb_nr].num_idles++;

                            cur_hb_nr++;
                        }
                    }
                    else
                    {
                        /*
                         * In the second loop, we should OR the data we gathered from the first loop. So if entry was
                         * searched here, we make num_idles 0. If the entry was not searched in second loop, it might
                         * have been searched in first loop. So if entry was not searched in second loop, we leave the
                         * num_idles unchanged
                         */
                        for (j = 0; j < KAPS_HB_ROW_WIDTH_1; ++j)
                        {
                            if (hb_word & (1u << j))
                                active_aging_table[cur_hb_nr].num_idles = 0;

                            cur_hb_nr++;
                        }
                    }

                    offset += KAPS_HB_ROW_WIDTH_8;
                }
            }

            if (device->id == KAPS_JERICHO_2_DEVICE_ID )
            {
                start_hb_blk += num_active_hb_blocks;
                end_hb_blk += num_active_hb_blocks;
            }
        }
    }

    if (db->fn_table->db_process_hit_bits)
        db->fn_table->db_process_hit_bits(device, db);

    return KAPS_OK;
}

static kaps_status
kaps_get_aged_entries(
    struct kaps_hb_db *hb_db,
    uint32_t buf_size,
    uint32_t * num_entries,
    struct kaps_entry **entries)
{
    struct kaps_db *db = hb_db->db_info.common_info->hb_info.db;
    struct kaps_device *device = hb_db->db_info.device;
    uint32_t start_hb_blk, end_hb_blk, cur_hb_blk, cur_row;
    uint32_t i, j, cur_hb_nr;
    uint32_t main_bb_nr, loop_cnt, cur_blk;
    struct kaps_aging_entry *active_aging_table = kaps_device_get_active_aging_table(device, db);
    uint32_t num_active_hb_blocks = kaps_device_get_active_num_hb_blocks(device, db);
    uint32_t num_active_rows_in_hb_block = kaps_device_get_active_num_hb_rows(device, db);

    if (device->id == KAPS_JERICHO_2_DEVICE_ID && db->num_algo_levels_in_db == 3)
    {
        i = 0;
        for (main_bb_nr = 0; main_bb_nr < num_active_hb_blocks; main_bb_nr += 2)
        {
            for (loop_cnt = 0; loop_cnt < 2; ++loop_cnt)
            {
                cur_blk = main_bb_nr + loop_cnt;

                if (!(db->common_info->kaps2_large_hb_blk_bmp & (1u << cur_blk)))
                {
                    continue;
                }

                for (cur_row = 0; cur_row < device->num_rows_in_each_hb_block[main_bb_nr]; ++cur_row)
                {

                    cur_hb_nr =
                        (cur_row * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1) + (cur_blk * KAPS_HB_ROW_WIDTH_1);

                    for (j = 0; j < KAPS_HB_ROW_WIDTH_1; ++j)
                    {
                        if (active_aging_table[cur_hb_nr].entry
                            && active_aging_table[cur_hb_nr].num_idles >= hb_db->age_count)
                        {
                            if (i < buf_size)
                            {
                                entries[i] = active_aging_table[cur_hb_nr].entry;
                                ++i;
                            }
                        }
                        ++cur_hb_nr;
                    }
                }
            }

            if (i >= buf_size)
                break;
        }

    }
    else
    {

        start_hb_blk = db->common_info->start_hb_blk;
        end_hb_blk = start_hb_blk + db->common_info->num_hb_blks - 1;

        i = 0;
        for (cur_row = 0; cur_row < num_active_rows_in_hb_block; ++cur_row)
        {
            cur_hb_nr = (cur_row * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1) + (start_hb_blk * KAPS_HB_ROW_WIDTH_1);

            for (cur_hb_blk = start_hb_blk; cur_hb_blk <= end_hb_blk; ++cur_hb_blk)
            {
                for (j = 0; j < KAPS_HB_ROW_WIDTH_1; ++j)
                {
                    if (active_aging_table[cur_hb_nr].entry
                        && active_aging_table[cur_hb_nr].num_idles >= hb_db->age_count)
                    {
                        if (i < buf_size)
                        {
                            entries[i] = active_aging_table[cur_hb_nr].entry;
                            ++i;
                        }
                    }
                    ++cur_hb_nr;
                }
            }

            if (i >= buf_size)
                break;
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_db_get_aged_entries(
    struct kaps_hb_db * hb_db,
    uint32_t buf_size,
    uint32_t * num_entries,
    struct kaps_entry ** entries)
{
    KAPS_TRACE_IN("%p %u %p %p\n", hb_db, buf_size, num_entries, entries);
    if ((hb_db == NULL) || (buf_size == 0) || (num_entries == NULL) || (entries == NULL))
        return KAPS_INVALID_ARGUMENT;

    if (hb_db->db_info.device->type == KAPS_DEVICE_KAPS)
    {
        KAPS_STRY(kaps_get_aged_entries(hb_db, buf_size, num_entries, entries));
    }

    KAPS_TRACE_OUT("%u %p\n", *num_entries, *entries);
    return KAPS_OK;
}

kaps_status
kaps_hb_db_aged_entry_iter_init(
    struct kaps_hb_db * hb_db,
    struct kaps_aged_entry_iter ** iter)
{
    struct kaps_c_list_iter *it;
    struct kaps_device *device;
    struct kaps_db *parent_db;

    KAPS_TRACE_IN("%p %p\n", hb_db, iter);
    if (!hb_db || !iter)
        return KAPS_INVALID_ARGUMENT;

    parent_db = KAPS_GET_DB_PARENT(hb_db->db_info.common_info->hb_info.db);
    device = hb_db->db_info.device;

    if (!device->is_config_locked)
        return KAPS_DEVICE_UNLOCKED;

    if (device->flags & KAPS_DEVICE_ISSU)
    {
        if (device->issu_status == KAPS_ISSU_SAVE_COMPLETED)
        {
            return KAPS_UNSUPPORTED;
        }
    }

    it = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(*it));
    if (!it)
        return KAPS_OUT_OF_MEMORY;

    kaps_c_list_iter_init(&parent_db->db_list, it);
    *iter = (struct kaps_aged_entry_iter *) it;

    KAPS_TRACE_OUT("%p\n", *iter);
    return KAPS_OK;
}

kaps_status
kaps_hb_db_aged_entry_iter_next(
    struct kaps_hb_db * hb_db,
    struct kaps_aged_entry_iter * iter,
    struct kaps_entry ** entry)
{
    struct kaps_device *device;
    struct kaps_db *parent_db;
    struct kaps_c_list_iter *it;
    struct kaps_list_node *el;
    struct kaps_entry *actual_entry;
    struct kaps_hb *hb, *hb_entry;
    struct kaps_aging_entry *active_aging_table; 

    KAPS_TRACE_IN("%p %p %p\n", hb_db, iter, entry);
    if (!hb_db || !iter || !entry)
        return KAPS_INVALID_ARGUMENT;

    parent_db = KAPS_GET_DB_PARENT(hb_db->db_info.common_info->hb_info.db);
    device = hb_db->db_info.device;

    active_aging_table = kaps_device_get_active_aging_table(device, parent_db);

    if (!device->is_config_locked)
        return KAPS_DEVICE_UNLOCKED;

    if (device->flags & KAPS_DEVICE_ISSU)
    {
        if (device->issu_status == KAPS_ISSU_SAVE_COMPLETED)
        {
            return KAPS_UNSUPPORTED;
        }
    }

    it = (struct kaps_c_list_iter *) iter;

    if (it)
    {
        while ((el = kaps_c_list_iter_next(it)) != NULL)
        {
            actual_entry = KAPS_DBLIST_TO_KAPS_ENTRY(el);

            parent_db->fn_table->get_hb(parent_db, actual_entry, &hb);

            if (hb)
            {
                KAPS_WB_HANDLE_READ_LOC((&hb_db->db_info), &hb_entry, (uintptr_t) hb);

                if (active_aging_table[hb_entry->bit_no].num_idles >= hb_db->age_count)
                {
                    *entry = active_aging_table[hb_entry->bit_no].entry;
                    KAPS_TRACE_OUT("%p\n", *entry);
                    return KAPS_OK;
                }
            }
        }
    }

    *entry = NULL;
    KAPS_TRACE_OUT("%p\n", *entry);
    return KAPS_OK;
}

kaps_status
kaps_hb_db_aged_entry_iter_destroy(
    struct kaps_hb_db * hb_db,
    struct kaps_aged_entry_iter * iter)
{
    struct kaps_device *device;

    KAPS_TRACE_IN("%p %p\n", hb_db, iter);
    if (!hb_db || !iter)
        return KAPS_INVALID_ARGUMENT;

    device = hb_db->db_info.device;

    device->alloc->xfree(device->alloc->cookie, iter);
    return KAPS_OK;
}

kaps_status
kaps_hb_entry_get_idle_count(
    struct kaps_hb_db * hb_db,
    struct kaps_hb * hb,
    uint32_t * idle_count)
{
    struct kaps_hb *actual_hb;
    struct kaps_db *db; 
    struct kaps_aging_entry *active_aging_table;

    KAPS_TRACE_IN("%p %p %p\n", hb_db, hb, idle_count);
    if (!hb_db || !hb || !idle_count)
        return KAPS_INVALID_ARGUMENT;

    db = hb_db->db_info.common_info->hb_info.db;
    active_aging_table = kaps_device_get_active_aging_table(db->device, db);

    *idle_count = 0;

    KAPS_WB_HANDLE_READ_LOC((&hb_db->db_info), (&actual_hb), (uintptr_t) hb);

    *idle_count = active_aging_table[actual_hb->bit_no].num_idles;
    KAPS_TRACE_OUT("%u\n", *idle_count);

    return KAPS_OK;
}

kaps_status
kaps_hb_entry_set_idle_count(
    struct kaps_hb_db * hb_db,
    struct kaps_hb * hb,
    uint32_t idle_count)
{
    struct kaps_hb *actual_hb;
    struct kaps_db *db; 
    struct kaps_aging_entry *active_aging_table;

    KAPS_TRACE_IN("%p %p %u\n", hb_db, hb, idle_count);
    if (!hb_db || !hb)
        return KAPS_INVALID_ARGUMENT;

    db = hb_db->db_info.common_info->hb_info.db;
    active_aging_table = kaps_device_get_active_aging_table(db->device, db);

    KAPS_WB_HANDLE_READ_LOC((&hb_db->db_info), (&actual_hb), (uintptr_t) hb);

    active_aging_table[actual_hb->bit_no].num_idles = idle_count;

    return KAPS_OK;
}

kaps_status
kaps_hb_entry_get_bit_value(
    struct kaps_hb_db * hb_db,
    struct kaps_hb * hb_handle,
    uint32_t * bit_value,
    uint8_t clear_on_read)
{
    struct kaps_hb *hb = NULL;
    struct kaps_device *device = NULL;
    struct kaps_db *db = NULL;
    uint32_t final_level_offset;
    uint32_t num_active_hb_blocks;

    KAPS_TRACE_IN("%p %p %p %u\n", hb_db, hb_handle, bit_value, clear_on_read);
    if (!hb_db || !hb_handle || !bit_value)
        return KAPS_INVALID_ARGUMENT;

    device = hb_db->db_info.device;
    db = hb_db->db_info.common_info->hb_info.db;

    final_level_offset = kaps_device_get_final_level_offset(device, db);
    num_active_hb_blocks = kaps_device_get_active_num_hb_blocks(device, db);

    KAPS_WB_HANDLE_READ_LOC((&hb_db->db_info), (&hb), (uintptr_t) hb_handle);

    
    {
        uint8_t kaps_hb_data[2 * KAPS_HB_ROW_WIDTH_8];
        uint32_t block_num, row_num, bit_pos;
        uint32_t is_odd_large_bb;
        uint32_t buf_len_8;
       

        kaps_memset(kaps_hb_data, 0, 2 * KAPS_HB_ROW_WIDTH_8);

        block_num = hb->bit_no / KAPS_HB_ROW_WIDTH_1;
        block_num = block_num % num_active_hb_blocks;

        is_odd_large_bb = 0;

        if (device->id == KAPS_JERICHO_2_DEVICE_ID && db->num_algo_levels_in_db == 3)
        {
            uint32_t main_bb_nr = block_num;

            if (main_bb_nr % 2 == 1)
            {
                is_odd_large_bb = 1;
                main_bb_nr--;
            }

            block_num = block_num / 2;
        }

        row_num = hb->bit_no / (KAPS_HB_ROW_WIDTH_1 * num_active_hb_blocks);

        KAPS_STRY(kaps_dm_kaps_hb_read(device, db, final_level_offset + block_num, row_num, kaps_hb_data));

        buf_len_8 = KAPS_HB_ROW_WIDTH_8;
        bit_pos = hb->bit_no % KAPS_HB_ROW_WIDTH_1;

        if (is_odd_large_bb)
        {
            *bit_value = KapsReadBitsInArrray(&kaps_hb_data[KAPS_HB_ROW_WIDTH_8], buf_len_8, bit_pos, bit_pos);
        }
        else
        {
            *bit_value = KapsReadBitsInArrray(kaps_hb_data, buf_len_8, bit_pos, bit_pos);
        }

        if (clear_on_read)
        {
            if (*bit_value)
            {
                if (is_odd_large_bb)
                {
                    KapsWriteBitsInArray(&kaps_hb_data[KAPS_HB_ROW_WIDTH_8], buf_len_8, bit_pos, bit_pos, 0);
                }
                else
                {
                    KapsWriteBitsInArray(kaps_hb_data, buf_len_8, bit_pos, bit_pos, 0);
                }

                kaps_dm_kaps_hb_write(device, db, final_level_offset + block_num, row_num, kaps_hb_data);
            }
        }

        /* If bit_value is 0 or if clear on read is set, then we should set the bit value*/
        if (*bit_value == 0 || clear_on_read)
        {
            if (db->fn_table->db_get_algo_hit_bit_value && hb->entry)
                db->fn_table->db_get_algo_hit_bit_value(device, db, hb->entry, clear_on_read, bit_value);
        }
    }

    KAPS_TRACE_OUT("%u\n", *bit_value);
    return KAPS_OK;
}

kaps_status
kaps_hb_db_set_property(
    struct kaps_hb_db * hb_db,
    enum kaps_db_properties property,
    ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;

    if (hb_db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (hb_db->db_info.device->is_config_locked)
    {
        if (property != KAPS_PROP_AGE_COUNT)
            return KAPS_DB_ACTIVE;
    }

    if (hb_db->db_info.device->flags & KAPS_DEVICE_ISSU)
    {
        if (hb_db->db_info.device->issu_status != KAPS_ISSU_INIT)
            return KAPS_ISSU_IN_PROGRESS;
    }

    va_start(vl, property);
    switch (property)
    {
        case KAPS_PROP_DESCRIPTION:
        {
            char *desc = va_arg(vl, char *);
            KAPS_TRACE_IN("%p %d %s\n", hb_db, property, desc);
            if (!desc)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            hb_db->db_info.description
                = hb_db->db_info.device->alloc->xcalloc(hb_db->db_info.device->alloc->cookie, 1, (strlen(desc) + 1));
            if (!hb_db->db_info.description)
            {
                status = KAPS_OUT_OF_MEMORY;
                break;
            }
            strcpy(hb_db->db_info.description, desc);
            break;
        }
        case KAPS_PROP_AGE_COUNT:
        {
            uint32_t age_count = va_arg(vl, uint32_t);

            KAPS_TRACE_IN("%p %d %u\n", hb_db, property, age_count);
            if (age_count == 0)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            hb_db->age_count = age_count;
            break;
        }
        default:
            KAPS_TRACE_IN("%p %d\n", hb_db, property);
            status = KAPS_INVALID_ARGUMENT;
            break;
    }
    va_end(vl);
    return status;
}

kaps_status
kaps_hb_db_get_property(
    struct kaps_hb_db * hb_db,
    enum kaps_db_properties property,
    ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %d\n", hb_db, property);
    if (hb_db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (hb_db->db_info.device->flags & KAPS_DEVICE_ISSU)
    {
        if (hb_db->db_info.device->issu_status != KAPS_ISSU_INIT)
            return KAPS_ISSU_IN_PROGRESS;
    }

    va_start(vl, property);
    switch (property)
    {
        case KAPS_PROP_DESCRIPTION:
        {
            char **desc = va_arg(vl, char **);
            if (!desc)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            *desc = hb_db->db_info.description;
            break;
        }
        case KAPS_PROP_AGE_COUNT:
        {
            uint32_t *age_count = va_arg(vl, uint32_t *);
            if (!age_count)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            *age_count = hb_db->age_count;
            break;
        }
        default:
            status = KAPS_INVALID_ARGUMENT;
            break;
    }
    va_end(vl);
    return status;
}

kaps_status
kaps_hb_wb_save_state(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    uint32_t capacity;

    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    if (!wb_fun)
        return KAPS_INVALID_ARGUMENT;

    capacity = KAPS_HB_ROW_WIDTH_1 * device->num_hb_blocks * device->num_rows_in_hb_block;

    if (!device->hb_buffer || !device->aging_table)
        return KAPS_OK;

    if (0 !=
        wb_fun->write_fn(wb_fun->handle, (uint8_t *) device->hb_buffer, capacity / KAPS_BITS_IN_BYTE,
                         *wb_fun->nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *wb_fun->nv_offset += capacity / KAPS_BITS_IN_BYTE;

    if (0 !=
        wb_fun->write_fn(wb_fun->handle, (uint8_t *) device->aging_table, (capacity * sizeof(struct kaps_aging_entry)),
                         *wb_fun->nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *wb_fun->nv_offset += (capacity * sizeof(struct kaps_aging_entry));


    if (device->num_small_hb_blocks)
    {
        capacity = KAPS_HB_ROW_WIDTH_1 * device->num_small_hb_blocks * device->num_rows_in_small_hb_block;
        if (0 !=
        wb_fun->write_fn(wb_fun->handle, (uint8_t *) device->small_aging_table, (capacity * sizeof(struct kaps_aging_entry)),
                         *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
        
        *wb_fun->nv_offset += (capacity * sizeof(struct kaps_aging_entry));
    }

    return KAPS_OK;
}

kaps_status
kaps_hb_wb_restore_state(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    uint32_t capacity;

    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    if (!wb_fun)
        return KAPS_INVALID_ARGUMENT;

    capacity = KAPS_HB_ROW_WIDTH_1 * device->num_hb_blocks * device->num_rows_in_hb_block;

    if (!device->hb_buffer || !device->aging_table)
        return KAPS_OK;

    if (0 !=
        wb_fun->read_fn(wb_fun->handle, (uint8_t *) device->hb_buffer, capacity / KAPS_BITS_IN_BYTE,
                        *wb_fun->nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *wb_fun->nv_offset += capacity / KAPS_BITS_IN_BYTE;

    if (0 !=
        wb_fun->read_fn(wb_fun->handle, (uint8_t *) device->aging_table, (capacity * sizeof(struct kaps_aging_entry)),
                        *wb_fun->nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    
    *wb_fun->nv_offset += (capacity * sizeof(struct kaps_aging_entry));

    if (device->num_small_hb_blocks)
    {
        capacity = KAPS_HB_ROW_WIDTH_1 * device->num_small_hb_blocks * device->num_rows_in_small_hb_block;

        if (0 !=
            wb_fun->read_fn(wb_fun->handle, (uint8_t *) device->small_aging_table, (capacity * sizeof(struct kaps_aging_entry)),
                            *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
        
        *wb_fun->nv_offset += (capacity * sizeof(struct kaps_aging_entry));    
    }

    return KAPS_OK;
}

kaps_status
kaps_hb_db_wb_add_entry(
    struct kaps_hb_db * hb_db,
    uint32_t bit_no,
    uintptr_t user_handle)
{
    struct kaps_hb *hb_p;

    if (!hb_db)
        return KAPS_INVALID_ARGUMENT;

    if (hb_db->db_info.device->flags & KAPS_DEVICE_ISSU)
    {
        if (hb_db->db_info.device->issu_status != KAPS_ISSU_INIT &&
            hb_db->db_info.device->issu_status != KAPS_ISSU_RESTORE_END)
            return KAPS_ISSU_IN_PROGRESS;
    }

    if (!hb_db->db_info.device->is_config_locked)
        return KAPS_DEVICE_UNLOCKED;

    POOL_ALLOC(kaps_hb, &hb_db->kaps_hb_pool, hb_p);
    if (!hb_p)
        return KAPS_OUT_OF_MEMORY;

    kaps_memset(hb_p, 0, sizeof(*hb_p));

    kaps_c_list_add_tail(&hb_db->hb_list, &hb_p->hb_node);
    hb_p->hb_db_seq_num = hb_db->db_info.seq_num;
    hb_p->bit_no = bit_no;

    KAPS_WB_HANDLE_WRITE_LOC(((struct kaps_db *) hb_db), hb_p, user_handle);
    return KAPS_OK;
}

kaps_status
kaps_hb_db_refresh_handle(
    struct kaps_device * device,
    struct kaps_hb_db * stale_ptr,
    struct kaps_hb_db ** dbp)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;
    struct kaps_hb_db *db;

    KAPS_TRACE_IN("%p %p %p\n", device, stale_ptr, dbp);
    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    if (device->main_dev)
        device = device->main_dev;

    if (!(device->flags & KAPS_DEVICE_ISSU))
        return KAPS_INVALID_FLAGS;

    if (device->issu_status == KAPS_ISSU_INIT)
        return KAPS_INVALID_ARGUMENT;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(e1);

        if (tmp->type == KAPS_DB_HB)
        {
            db = (struct kaps_hb_db *) tmp;
            if (db->db_info.stale_ptr == (uintptr_t) stale_ptr)
            {
                *dbp = db;
                KAPS_TRACE_OUT("%p\n", *dbp);
                return KAPS_OK;
            }
        }
    }

    if (device->other_core)
    {
        kaps_c_list_iter_init(&device->other_core->db_list, &it);
        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(e1);

            if (tmp->type == KAPS_DB_HB)
            {
                db = (struct kaps_hb_db *) tmp;
                if (db->db_info.stale_ptr == (uintptr_t) stale_ptr)
                {
                    *dbp = db;
                    KAPS_TRACE_OUT("%p\n", *dbp);
                    return KAPS_OK;
                }
            }
        }
    }

    KAPS_TRACE_OUT("%p\n", 0);
    return KAPS_INVALID_ARGUMENT;
}
