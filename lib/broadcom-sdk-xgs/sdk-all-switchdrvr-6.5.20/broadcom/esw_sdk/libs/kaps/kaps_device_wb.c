/*
**************************************************************************************
Copyright 2012-2019 Broadcom Corporation

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

#include "kaps_ab.h"
#include "kaps_device_wb.h"
#include "kaps_key_internal.h"
#include "kaps_instruction_internal.h"
#include "kaps_device_internal.h"
#include "kaps_db_internal.h"
#include "kaps_hw_limits.h"
#include "kaps_resource.h"
#include "kaps_ad_internal.h"
#include "kaps_device.h"
#include "kaps_utility.h"
#include "kaps_handle.h"
#include "kaps_math.h"
#include "kaps_cr_pool_mgr.h"
#include "kaps_errors.h"

/*#define KAPS_GET_DB_PARENT(db) ((db->parent)?(db->parent):(db))*/

kaps_status
kaps_wb_refresh_db_handle_in_map(
    struct kaps_device *main_device)
{
    struct memory_map *map;
    uint32_t i, j;
    struct kaps_db *fresh_db_ptr = NULL;
    struct kaps_device *cascade;

    cascade = main_device;

    while (cascade)
    {

        map = resource_kaps_get_memory_map(cascade);

        for (i = 0; i < cascade->num_dba_sb; i++)
        {
            struct dba_sb_info *sb = &map->sb_memory[i];
            uint32_t start = sb->sb_no * cascade->num_ab_per_sb;

            for (j = 0; j < cascade->num_ab_per_sb; ++j)
            {
                if (sb->owners[j])
                {
                    KAPS_STRY(kaps_db_refresh_handle(main_device, sb->owners[j], &fresh_db_ptr));
                    sb->owners[j] = fresh_db_ptr;
                }
            }

            for (j = 0; j < cascade->num_ab_per_sb; ++j)
            {
                struct kaps_ab_info *ab = &map->ab_memory[start + j];

                if (ab->db && ab->db != MAGIC_DB)
                {
                    KAPS_STRY(kaps_db_refresh_handle(main_device, ab->db, &fresh_db_ptr));
                    ab->db = fresh_db_ptr;
                }
            }
        }

        if (main_device->nv_ptr)
        {
            for (i = 0; i < map->num_udc * map->num_udm_per_udc * KAPS_HW_NUM_SB_PER_UDM; i++)
            {
                int32_t udc_no, udm_no;
                struct udm_info *udm;
                struct kaps_db *db_p, *fresh_db;

                udc_no = i / (map->num_udm_per_udc * KAPS_HW_NUM_SB_PER_UDM);
                udm_no = (i % (map->num_udm_per_udc * KAPS_HW_NUM_SB_PER_UDM)) / KAPS_HW_NUM_SB_PER_UDM;

                udm = &map->udc[udc_no].udm[udm_no];
                db_p = udm->db[i % KAPS_HW_NUM_SB_PER_UDM];
                if (!db_p || db_p == MAGIC_DB)
                    continue;
                KAPS_STRY(kaps_db_refresh_handle(main_device, db_p, &fresh_db));
                udm->db[i % KAPS_HW_NUM_SB_PER_UDM] = (struct kaps_db *) fresh_db;
            }
        }

        cascade = cascade->next_dev;
    }

    return KAPS_OK;
}

static kaps_status
kaps_device_nv_memory_manager_init_internal(
    struct kaps_device *device,
    uint32_t db_count)
{
    device->nv_mem_mgr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_nv_memory_manager));
    if (!device->nv_mem_mgr)
        return KAPS_OUT_OF_MEMORY;

    if (device->smt)
        device->smt->nv_mem_mgr = device->nv_mem_mgr;

    device->nv_mem_mgr->num_db = db_count;
    device->nv_mem_mgr->offset_device_info_start = 0;
    device->nv_mem_mgr->offset_device_pending_list = 0;
    device->nv_mem_mgr->offset_device_lpm_info = 0;
    device->nv_mem_mgr->offset_device_lpm_shadow_info = 0;
    device->nv_mem_mgr->offset_device_hb_info = 0;

    device->nv_mem_mgr->offset_db_trie_has_been_built =
        device->alloc->xcalloc(device->alloc->cookie, db_count, sizeof(uint32_t));
    if (!device->nv_mem_mgr->offset_db_trie_has_been_built)
    {
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr);
        return KAPS_OUT_OF_MEMORY;
    }

    device->nv_mem_mgr->offset_db_trie_sb_bitmap =
        device->alloc->xcalloc(device->alloc->cookie, db_count, sizeof(uint32_t));
    if (!device->nv_mem_mgr->offset_db_trie_sb_bitmap)
    {
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_has_been_built);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr);
        return KAPS_OUT_OF_MEMORY;
    }

    device->nv_mem_mgr->offset_db_info_start =
        device->alloc->xcalloc(device->alloc->cookie, db_count, sizeof(uint32_t));
    if (!device->nv_mem_mgr->offset_db_info_start)
    {
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_has_been_built);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_sb_bitmap);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr);
        return KAPS_OUT_OF_MEMORY;
    }

    device->nv_mem_mgr->offset_db_info_end = device->alloc->xcalloc(device->alloc->cookie, db_count, sizeof(uint32_t));
    if (!device->nv_mem_mgr->offset_db_info_end)
    {
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_has_been_built);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_sb_bitmap);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_info_start);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr);
        return KAPS_OUT_OF_MEMORY;
    }

    return KAPS_OK;
}

kaps_status
kaps_device_nv_memory_manager_init(
    struct kaps_device * device)
{
    uint32_t db_count = 0;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_DB_AD && db->type != KAPS_DB_COUNTER && db->type != KAPS_DB_TAP && db->type != KAPS_DB_HB)
            db_count++;
    }
    return kaps_device_nv_memory_manager_init_internal(device, db_count);
}

kaps_status
kaps_device_nv_memory_manager_cr_init(
    struct kaps_device * device,
    uint32_t db_count)
{
    return kaps_device_nv_memory_manager_init_internal(device, db_count);
}

struct kaps_device_res_wb_info
{
    uint16_t num_db;
    uint16_t num_instructions;
    uint16_t num_dba_sb;
    uint16_t num_ab;
    uint16_t num_uda;
    uint16_t num_uda_per_sb;
    uint32_t uda_power_budget;              /**< maximum no of UDMS allowed to turn on at any cycle */
    uint32_t dba_power_budget;              /**< maximum no of ABs allowed to turn on at any cycle by non-mp DBs */
    uint32_t num_user_threads;
    uint32_t instruction_cascading_present;  /**< OP2 specific, 1 indicates instruction cascading present */
    uint32_t op2_clock_rate;                /**< 0 means full rate, 1 means half rate */

    struct
    {
        struct dba_sb_info sb;
        struct kaps_ab_wb_info ab_info[8];
    } sb_info[2 * KAPS_HW_MAX_DBA_SB];
};

static kaps_status
kaps_device_recover_state(
    struct kaps_device *device)
{
    uint32_t nv_offset = 0;
    uint32_t num_db;
    uint32_t db_iter;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_wb_cb_functions cb_fun;
    uint32_t pending_count;
    struct kaps_device *tmp_device;
    uint8_t *tmp_ptr = NULL;

    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    if (!device->nv_ptr)
        return KAPS_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    cb_fun.read_fn = device->nv_read_fn;
    cb_fun.write_fn = device->nv_write_fn;
    cb_fun.handle = device->nv_handle;
    cb_fun.nv_offset = &nv_offset;
    cb_fun.nv_ptr = device->nv_ptr;

    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmTrue;
        if (tmp_device->smt)
            tmp_device->smt->issu_in_progress = NlmTrue;
        tmp_device = tmp_device->next_dev;
    }

    for (tmp_device = device; tmp_device; tmp_device = tmp_device->next_dev)
    {
        tmp_device->issu_status = KAPS_ISSU_RESTORE_START;
        if (tmp_device->smt)
        {
            tmp_device->smt->issu_status = KAPS_ISSU_RESTORE_START;
        }
    }

    tmp_ptr = cb_fun.nv_ptr;

    num_db = *(uint32_t *) cb_fun.nv_ptr;
    tmp_ptr += sizeof(uint32_t);

    KAPS_STRY(kaps_device_nv_memory_manager_cr_init(device, num_db));
    KAPS_CR_GET_POOL_MGR(device, device->nv_mem_mgr->kaps_cr_pool_mgr);

    device->nv_mem_mgr->num_db = num_db;

    device->nv_mem_mgr->offset_device_lpm_shadow_info = *(uint32_t *) tmp_ptr;
    tmp_ptr += sizeof(uint32_t);

    device->nv_mem_mgr->offset_device_info_start = *(uint32_t *) tmp_ptr;
    tmp_ptr += sizeof(uint32_t);

    device->nv_mem_mgr->offset_device_pending_list = *(uint32_t *) tmp_ptr;
    pending_count = *(uint32_t *) ((uint8_t *) device->nv_ptr + device->nv_mem_mgr->offset_device_pending_list);
    tmp_ptr += sizeof(uint32_t);

    if (pending_count)
    {
        *device->cr_status = KAPS_RESTORE_CHANGES_ABORTED;
    }
    else if (*device->cr_status == KAPS_RESTORE_CHANGES_ABORTED)
    {
        *device->cr_status = KAPS_RESTORE_CHANGES_COMMITTED;
    }
    else
    {
        *device->cr_status = KAPS_RESTORE_NO_CHANGE;
    }

    device->nv_mem_mgr->offset_device_lpm_info = *(uint32_t *) tmp_ptr;
    tmp_ptr += sizeof(uint32_t);

    device->nv_mem_mgr->offset_device_hb_info = *(uint32_t *) tmp_ptr;
    tmp_ptr += sizeof(uint32_t);

    for (db_iter = 0; db_iter < device->nv_mem_mgr->num_db; db_iter++)
    {
        device->nv_mem_mgr->offset_db_trie_has_been_built[db_iter] = *(uint32_t *) tmp_ptr;
        tmp_ptr += sizeof(uint32_t);
        device->nv_mem_mgr->offset_db_trie_sb_bitmap[db_iter] = *(uint32_t *) tmp_ptr;
        tmp_ptr += sizeof(uint32_t);
        device->nv_mem_mgr->offset_db_info_start[db_iter] = *(uint32_t *) tmp_ptr;
        tmp_ptr += sizeof(uint32_t);
        device->nv_mem_mgr->offset_db_info_end[db_iter] = *(uint32_t *) tmp_ptr;
        tmp_ptr += sizeof(uint32_t);
    }

    cb_fun.nv_ptr = cb_fun.nv_ptr + device->nv_mem_mgr->offset_device_info_start;

    /*
     * Restore the DB and Instructions Layout 
     */
    KAPS_STRY(kaps_wb_restore_device_info(device, &cb_fun));

    KAPS_STRY(kaps_device_lock(device));

    if (device->lpm_mgr)
    {
        KAPS_STRY(kaps_lpm_cr_restore_state(device, &cb_fun));
        cb_fun.nv_ptr = (uint8_t *) device->nv_ptr + device->nv_mem_mgr->offset_device_lpm_shadow_info;
        *cb_fun.nv_offset = device->nv_mem_mgr->offset_device_lpm_shadow_info;
        kaps_lpm_cr_get_adv_ptr(device, &cb_fun);
    }
    if (device->aging_table && device->type == KAPS_DEVICE_KAPS)
    {
        cb_fun.nv_ptr = (uint8_t *) device->nv_ptr + device->nv_mem_mgr->offset_device_hb_info;
        *cb_fun.nv_offset = device->nv_mem_mgr->offset_device_hb_info;
        KAPS_STRY(kaps_kaps_hb_cr_init(device, &cb_fun));
    }

    KAPS_STRY(kaps_cr_pool_mgr_refresh_db_handles(device));

    db_iter = 0;
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
        {
            db->common_info->entry_size = db->fn_table->cr_calc_entry_mem(db);
            db->common_info->nv_db_iter = db_iter;
            db_iter++;
        }
    }

    for (tmp_device = device; tmp_device; tmp_device = tmp_device->next_dev)
    {
        tmp_device->issu_status = KAPS_ISSU_RESTORE_END;
        if (tmp_device->smt)
        {
            tmp_device->smt->issu_status = KAPS_ISSU_RESTORE_END;
        }
    }

    db_iter = 0;

    /*
     * Restore the common info for ACL DBs 
     */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
        {
            nv_offset = device->nv_mem_mgr->offset_db_info_start[db_iter];
            cb_fun.nv_ptr = (uint8_t *) device->nv_ptr + nv_offset;
            KAPS_STRY(db->fn_table->restore_cr_state(db, &cb_fun));
            db_iter++;
        }
    }

    KAPS_STRY(kaps_lpm_wb_post_processing(device));

    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmFalse;
        if (tmp_device->smt)
            tmp_device->smt->issu_in_progress = NlmFalse;
        tmp_device = tmp_device->next_dev;
    }

    /*
     * Making to zero, we will make it to one when user call the device_lock, so that we can block the APIs which
     * should be called after the device lock 
     */
    device->is_config_locked = 0;

    return KAPS_OK;
}

kaps_status
kaps_wb_save_device_info(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;
    uint32_t i, j;
    struct kaps_device *cascade;

    /*
     * Store the resource map 
     */
    cascade = device;
    while (cascade != NULL)
    {
        uint16_t count;
        struct kaps_device_res_wb_info *res_ptr = NULL;
        struct memory_map *map;

        if (wb_fun->write_fn)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) (&cascade->prop),
                                      sizeof(struct kaps_device_property), *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        else
        {
            kaps_memcpy(wb_fun->nv_ptr, (uint8_t *) (&cascade->prop), sizeof(struct kaps_device_property));
            wb_fun->nv_ptr += sizeof(struct kaps_device_property);
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(struct kaps_device_property);

        if (wb_fun->nv_ptr == NULL)
        {
            res_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_device_res_wb_info));
            if (res_ptr == NULL)
                return KAPS_OUT_OF_MEMORY;
        }
        else if (device->type == KAPS_DEVICE_KAPS)
        {
            res_ptr = (struct kaps_device_res_wb_info *) wb_fun->nv_ptr;
            wb_fun->nv_ptr += sizeof(*res_ptr);
        }

        count = 0;
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(e1);

            if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP
                || db->type == KAPS_DB_DMA || db->type == KAPS_DB_HB)
                continue;

            if (db->type != KAPS_DB_LPM && db->type != KAPS_DB_ACL)
                return KAPS_UNSUPPORTED;

            /*
             * Saving the tables and clones info are taken care of internally 
             */
            if (db->is_clone || db->parent)
                continue;
            count++;
        }
        if (wb_fun->nv_ptr == NULL || device->type == KAPS_DEVICE_KAPS)
        {
            res_ptr->num_db = count;
        }
        else
        {
            *(uint16_t *) wb_fun->nv_ptr = count;
            wb_fun->nv_ptr += sizeof(count);
            *wb_fun->nv_offset += sizeof(count);
        }

        count = 0;
        kaps_c_list_iter_init(&device->inst_list, &it);
        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_instruction *instruction = KAPS_INSTLIST_TO_ENTRY(e1);

            /*
             * Skip the instruction if it is a cmp3 pair 
             */
            if (instruction->is_cmp3_pair)
                continue;

            /*
             * Skip the instruction if it is a cascade duplicate 
             */
            if (instruction->device->device_no != 0)
                continue;
            count++;
        }
        if (wb_fun->nv_ptr == NULL || device->type == KAPS_DEVICE_KAPS)
        {
            res_ptr->num_instructions = count;
        }
        else
        {
            *(uint16_t *) wb_fun->nv_ptr = count;
            wb_fun->nv_ptr += sizeof(count);
            *wb_fun->nv_offset += sizeof(count);
            break;
        }

        KAPS_STRY(kaps_resource_wb_pre_process(cascade));
        map = resource_kaps_get_memory_map(cascade);

        res_ptr->num_dba_sb = cascade->num_dba_sb;
        res_ptr->num_ab = cascade->num_ab;
        res_ptr->num_uda = cascade->num_udc;
        res_ptr->num_uda_per_sb = cascade->num_uda_per_sb;
        res_ptr->uda_power_budget = cascade->uda_power_budget;
        res_ptr->dba_power_budget = cascade->dba_power_budget;
        res_ptr->num_user_threads = cascade->num_user_threads;
        res_ptr->instruction_cascading_present = cascade->instruction_cascading_present;
        res_ptr->op2_clock_rate = cascade->op2_clock_rate;

        for (i = 0; i < cascade->num_dba_sb; i++)
        {
            uint32_t start = map->sb_memory[i].sb_no * cascade->num_ab_per_sb;

            res_ptr->sb_info[i].sb = map->sb_memory[i];

            for (j = 0; j < cascade->num_ab_per_sb; ++j)
            {
                struct kaps_ab_info *ab = &map->ab_memory[start + j];

                res_ptr->sb_info[i].ab_info[j].ab_num = ab->ab_num;
                res_ptr->sb_info[i].ab_info[j].num_slots = ab->num_slots;
                res_ptr->sb_info[i].ab_info[j].base_addr = ab->base_addr;
                res_ptr->sb_info[i].ab_info[j].base_idx = ab->base_idx;
                res_ptr->sb_info[i].ab_info[j].blk_cleared = ab->blk_cleared;
                res_ptr->sb_info[i].ab_info[j].conf = ab->conf;
                res_ptr->sb_info[i].ab_info[j].device_no = ab->device->device_no;
                res_ptr->sb_info[i].ab_info[j].translate = ab->translate;
                res_ptr->sb_info[i].ab_info[j].db = ab->db;
                res_ptr->sb_info[i].ab_info[j].is_dup_ab = ab->is_dup_ab;

                if (ab->dup_ab)
                    res_ptr->sb_info[i].ab_info[j].paired_ab_num = ab->dup_ab->ab_num;
                else
                    res_ptr->sb_info[i].ab_info[j].paired_ab_num = INVALID_PAIRED_AB_NUM;
            }
        }

        if (wb_fun->write_fn != NULL)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) res_ptr, sizeof(*res_ptr), *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*res_ptr);

        if (wb_fun->nv_ptr == NULL)
            device->alloc->xfree(device->alloc->cookie, res_ptr);
        cascade = cascade->next_dev;
    }

    /*
     * Save each DB info and the number of DBs present
     */

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(e1);

        if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP || db->type == KAPS_DB_DMA
            || db->type == KAPS_DB_HB)
            continue;

        if (db->type != KAPS_DB_LPM && db->type != KAPS_DB_ACL)
            return KAPS_UNSUPPORTED;

        /*
         * Saving the tables and clones info are taken care of internally 
         */
        if (db->is_clone || db->parent)
            continue;

        KAPS_STRY(kaps_db_wb_save_info(db, wb_fun));
    }

    KAPS_STRY(kaps_device_save_two_level_bb_info(device, wb_fun));

    /*
     * Save each Instruction info and the number of Instructions present
     */

    kaps_c_list_iter_init(&device->inst_list, &it);
    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_instruction *instruction = KAPS_INSTLIST_TO_ENTRY(e1);

        /*
         * Skip the instruction if it is a cmp3 pair 
         */
        if (instruction->is_cmp3_pair)
            continue;

        /*
         * Skip the instruction if it is a cascade duplicate 
         */
        if (instruction->device->device_no != 0)
            continue;
        KAPS_STRY(kaps_instruction_wb_save(instruction, wb_fun));
    }

    return KAPS_OK;
}


kaps_status
kaps_wb_restore_device_info(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    uint32_t i, j, num_db = 0, num_instructions = 0;
    struct kaps_device *cascade;

    /*
     * Restore the resource map 
     */
    cascade = device;
    while (cascade)
    {
        struct kaps_device_res_wb_info *res_ptr;
        struct memory_map *map;

        if (wb_fun->read_fn)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & cascade->prop,
                                     sizeof(struct kaps_device_property), *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        else
        {
            kaps_memcpy((uint8_t *) (&cascade->prop), wb_fun->nv_ptr, sizeof(struct kaps_device_property));
            wb_fun->nv_ptr += sizeof(struct kaps_device_property);
            if (cascade->smt)
                kaps_memcpy((uint8_t *) (&cascade->smt->prop), (uint8_t *) (&cascade->prop),
                            sizeof(struct kaps_device_property));
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(struct kaps_device_property);

        if (wb_fun->nv_ptr == NULL)
        {
            res_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_device_res_wb_info));
            if (res_ptr == NULL)
                return KAPS_OUT_OF_MEMORY;
        }
        else if (device->type == KAPS_DEVICE_KAPS)
        {
            res_ptr = (struct kaps_device_res_wb_info *) wb_fun->nv_ptr;
            wb_fun->nv_ptr += sizeof(*res_ptr);
        }
        else
        {
            num_db = *(uint16_t *) wb_fun->nv_ptr;
            wb_fun->nv_ptr += sizeof(uint16_t);
            num_instructions = *(uint16_t *) wb_fun->nv_ptr;
            wb_fun->nv_ptr += sizeof(uint16_t);
            break;
        }
        if (wb_fun->read_fn != NULL)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) res_ptr, sizeof(*res_ptr), *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*res_ptr);

        cascade->num_dba_sb = res_ptr->num_dba_sb;
        cascade->num_ab = res_ptr->num_ab;
        cascade->num_udc = res_ptr->num_uda;
        cascade->num_uda_per_sb = res_ptr->num_uda_per_sb;
        cascade->uda_power_budget = res_ptr->uda_power_budget;
        cascade->dba_power_budget = res_ptr->dba_power_budget;
        cascade->num_user_threads = res_ptr->num_user_threads;
        cascade->instruction_cascading_present = res_ptr->instruction_cascading_present;
        cascade->op2_clock_rate = res_ptr->op2_clock_rate;

        map = resource_kaps_get_memory_map(cascade);

        for (i = 0; i < cascade->num_dba_sb; i++)
        {
            map->sb_memory[i] = res_ptr->sb_info[i].sb;
            for (j = 0; j < cascade->num_ab_per_sb; ++j)
            {
                uint32_t ab_num = (map->sb_memory[i].sb_no * cascade->num_ab_per_sb) + j;
                struct kaps_ab_info *ab = &map->ab_memory[ab_num];

                kaps_sassert(res_ptr->sb_info[i].ab_info[j].ab_num == ab_num);
                kaps_sassert(res_ptr->sb_info[i].ab_info[j].device_no == cascade->device_no);

                ab->num_slots = res_ptr->sb_info[i].ab_info[j].num_slots;
                ab->conf = res_ptr->sb_info[i].ab_info[j].conf;
                ab->blk_cleared = res_ptr->sb_info[i].ab_info[j].blk_cleared;
                ab->translate = res_ptr->sb_info[i].ab_info[j].translate;

                if (res_ptr->sb_info[i].ab_info[j].paired_ab_num != INVALID_PAIRED_AB_NUM)
                    ab->dup_ab = &map->ab_memory[res_ptr->sb_info[i].ab_info[j].paired_ab_num];

                ab->base_addr = res_ptr->sb_info[i].ab_info[j].base_addr;
                ab->base_idx = res_ptr->sb_info[i].ab_info[j].base_idx;
                ab->db = res_ptr->sb_info[i].ab_info[j].db;
                ab->is_dup_ab = res_ptr->sb_info[i].ab_info[j].is_dup_ab;

            }
        }

        if (num_db == 0)
            num_db = res_ptr->num_db;
        if (num_instructions == 0)
            num_instructions = res_ptr->num_instructions;

        if (wb_fun->nv_ptr == NULL)
            device->alloc->xfree(device->alloc->cookie, res_ptr);

        cascade = cascade->next_dev;
    }

    /*
     * Restore the databases
     */
    for (i = 0; i < num_db; i++)
    {
        KAPS_STRY(kaps_db_wb_restore_info(device, NULL, wb_fun));
    }

    /*
     * Restore the device advanced state
     */

    KAPS_STRY(kaps_device_restore_two_level_bb_info(device, wb_fun));

    /*
     * Restore the instructions
     */
    for (i = 0; i < num_instructions; i++)
    {
        KAPS_STRY(kaps_instruction_wb_restore(device, wb_fun));
    }

    return KAPS_OK;
}

kaps_status
kaps_device_save_state(
    struct kaps_device * bc_device,
    kaps_device_issu_read_fn read_fn,
    kaps_device_issu_write_fn write_fn,
    void *handle)
{
    uint32_t nv_start_offset = 0;

    KAPS_TRACE_IN("%p %p %p %p\n", bc_device, read_fn, write_fn, handle);
    if (!bc_device || !write_fn || !read_fn)
        return KAPS_INVALID_ARGUMENT;

    if (bc_device->main_bc_device)
        bc_device = bc_device->main_bc_device;

    KAPS_ASSERT_OR_ERROR((bc_device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         bc_device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_device *device = bc_device;
        uint32_t nv_offset = nv_start_offset;
        uint32_t nv_size = 0, nv_size_crc = 0, nv_footer = KAPS_NV_MAGIC_NUMBER;
        struct kaps_c_list_iter it;
        struct kaps_list_node *el;
        struct kaps_wb_cb_functions cb_fun;
        struct kaps_device *tmp_device = device;

        if (device->is_generic_bc_device)
        {
            device->issu_in_progress = NlmTrue;
            if (device->smt)
                device->smt->issu_in_progress = NlmTrue;
            continue;
        }

        if (!(device->flags & KAPS_DEVICE_ISSU))
            return KAPS_INVALID_FLAGS;

        if (device->type != KAPS_DEVICE_KAPS)
            return KAPS_UNSUPPORTED;

        if (!device->is_config_locked)
            return KAPS_DEVICE_UNLOCKED;

        /*
         * Perform Warmboot Pre Processing 
         */
        KAPS_STRY(kaps_lpm_wb_pre_processing(device));

        tmp_device = device;
        while (tmp_device)
        {
            tmp_device->issu_in_progress = NlmTrue;
            if (tmp_device->smt)
                tmp_device->smt->issu_in_progress = NlmTrue;
            tmp_device = tmp_device->next_dev;
        }

        nv_offset += sizeof(nv_size);
        nv_offset += sizeof(nv_size_crc);

        cb_fun.read_fn = read_fn;
        cb_fun.write_fn = write_fn;
        cb_fun.handle = handle;
        cb_fun.nv_offset = &nv_offset;
        cb_fun.nv_ptr = NULL;

        /*
         * Save the DB and Instructions info 
         */
        KAPS_STRY(kaps_wb_save_device_info(device, &cb_fun));

        /*
         * Saves the common lpm information
         */
        if (device->lpm_mgr)
        {
            KAPS_STRY(kaps_lpm_wb_save_state(device, &cb_fun));
        }

        /*
         * Saves the HB information
         */
        KAPS_STRY(kaps_hb_wb_save_state(device, &cb_fun));

        /*
         * Save the per database information
         */

        cb_fun.read_fn = read_fn;
        cb_fun.write_fn = write_fn;
        cb_fun.handle = handle;
        cb_fun.nv_offset = &nv_offset;

        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP
                || db->type == KAPS_DB_DMA || db->type == KAPS_DB_HB)
                continue;
            KAPS_STRY(db->fn_table->save_state(db, &cb_fun));

            if (db->common_info->defer_deletes_to_install)
            {
                int32_t i;

                if (0 != cb_fun.write_fn(cb_fun.handle, (uint8_t *) & db->common_info->num_pending_del_entries,
                                         sizeof(db->common_info->num_pending_del_entries), *cb_fun.nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;
                *cb_fun.nv_offset += sizeof(db->common_info->num_pending_del_entries);

                if (0 != cb_fun.write_fn(cb_fun.handle, (uint8_t *) & db->common_info->max_pending_del_entries,
                                         sizeof(db->common_info->max_pending_del_entries), *cb_fun.nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;
                *cb_fun.nv_offset += sizeof(db->common_info->max_pending_del_entries);

                for (i = 0; i < db->common_info->num_pending_del_entries; i++)
                {
                    uint32_t user_handle;

                    user_handle = db->common_info->del_entry_list[i]->user_handle;
                    if (0 != cb_fun.write_fn(cb_fun.handle, (uint8_t *) & user_handle,
                                             sizeof(user_handle), *cb_fun.nv_offset))
                        return KAPS_NV_READ_WRITE_FAILED;
                    *cb_fun.nv_offset += sizeof(user_handle);
                }
            }
        }

        /*
         * Store the nv size and CRC of the size at the head of the file 
         */
        nv_size = nv_offset - nv_start_offset;
        nv_size_crc = kaps_crc32(nv_size_crc, (uint8_t *) & nv_size, sizeof(nv_size));
        if (0 != write_fn(handle, (uint8_t *) & nv_size, sizeof(nv_size), nv_start_offset))
            return KAPS_NV_READ_WRITE_FAILED;

        if (0 != write_fn(handle, (uint8_t *) & nv_size_crc, sizeof(nv_size_crc), nv_start_offset + sizeof(nv_size)))
            return KAPS_NV_READ_WRITE_FAILED;

        if (0 != write_fn(handle, (uint8_t *) & nv_footer, sizeof(nv_footer), nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
        nv_offset += sizeof(nv_footer);

        device->issu_status = KAPS_ISSU_SAVE_COMPLETED;
        nv_start_offset = nv_offset;
    }

    return KAPS_OK;
}

kaps_status
kaps_device_save_state_and_continue(
    struct kaps_device * bc_device,
    kaps_device_issu_read_fn read_fn,
    kaps_device_issu_write_fn write_fn,
    void *handle)
{
    KAPS_TRACE_IN("%p %p %p %p\n", bc_device, read_fn, write_fn, handle);
    KAPS_STRY(kaps_device_save_state(bc_device, read_fn, write_fn, handle));

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_device *device = bc_device;
        struct kaps_device *tmp_device = device;

        device->is_wb_continue = 1;
        device->issu_status = KAPS_ISSU_INIT;
        tmp_device = device;
        while (tmp_device)
        {
            tmp_device->issu_in_progress = NlmFalse;
            if (tmp_device->smt)
                tmp_device->smt->issu_in_progress = NlmFalse;
            tmp_device = tmp_device->next_dev;
        }
    }
    return KAPS_OK;
}

kaps_status
kaps_device_restore_state_partial(
    struct kaps_device * device,
    kaps_device_issu_read_fn read_fn,
    kaps_device_issu_write_fn write_fn,
    void *handle,
    struct kaps_wb_cb_functions * cb_fun,
    uint32_t * nv_start_offset)
{
    uint32_t nv_offset = *nv_start_offset;
    uint32_t nv_size = 0, nv_size_crc = 0, nv_footer = 0, check_crc = 0;
    struct kaps_device *tmp_device = device;

    if (!(device->flags & KAPS_DEVICE_ISSU))
        return KAPS_INVALID_FLAGS;

    if (device->type != KAPS_DEVICE_KAPS)
        return KAPS_UNSUPPORTED;

    if (device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    if (device->issu_status != KAPS_ISSU_INIT)
        return KAPS_ISSU_RESTORE_DONE;

    if (!(device->flags & KAPS_DEVICE_SKIP_INIT))
        return KAPS_DEVICE_NO_SKIP_INIT_FLAG;

    if (kaps_c_list_count(&device->inst_list) != 0)
        return KAPS_DEVICE_NOT_EMPTY;

    if (kaps_c_list_count(&device->db_list) != 0)
        return KAPS_DEVICE_NOT_EMPTY;

    for (tmp_device = device; tmp_device; tmp_device = tmp_device->next_dev)
    {
        tmp_device->issu_status = KAPS_ISSU_RESTORE_START;
        if (tmp_device->smt)
        {
            tmp_device->smt->issu_status = KAPS_ISSU_RESTORE_START;
        }
    }

    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmTrue;
        if (tmp_device->smt)
            tmp_device->smt->issu_in_progress = NlmTrue;
        tmp_device = tmp_device->next_dev;
    }

    /*
     * Read the file size and the CRC value of the file size 
     */

    if (0 != read_fn(handle, (uint8_t *) & nv_size, sizeof(nv_size), nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    nv_offset += sizeof(nv_size);

    /*
     * Recalculate the CRC and match with the data from NV 
     */
    if (0 != read_fn(handle, (uint8_t *) & nv_size_crc, sizeof(nv_size_crc), nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    nv_offset += sizeof(nv_size_crc);

    check_crc = kaps_crc32(check_crc, (uint8_t *) & nv_size, sizeof(nv_size));
    if (check_crc != nv_size_crc)
        return KAPS_NV_DATA_CORRUPT;

    if (0 != read_fn(handle, (uint8_t *) & nv_footer, sizeof(nv_footer), *nv_start_offset + nv_size))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_start_offset = *nv_start_offset + nv_size + sizeof(nv_footer);

    if (KAPS_NV_MAGIC_NUMBER != nv_footer)
        return KAPS_NV_DATA_CORRUPT;

    cb_fun->read_fn = read_fn;
    cb_fun->write_fn = write_fn;
    cb_fun->handle = handle;
    cb_fun->nv_ptr = device->nv_ptr;
    *cb_fun->nv_offset = nv_offset;

    KAPS_STRY(kaps_wb_restore_device_info(device, cb_fun));

    return KAPS_OK;
}

kaps_status
kaps_device_restore_cascaded_inst_ptr(
    struct kaps_device * device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;

    kaps_c_list_iter_init(&device->inst_list, &it);
    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_instruction *instr = KAPS_INSTLIST_TO_ENTRY(e1);

        if (instr->cascaded_inst_stale_ptr)
        {
            KAPS_STRY(kaps_instruction_refresh_handle
                      (device->other_core, (struct kaps_instruction *) instr->cascaded_inst_stale_ptr,
                       &instr->cascaded_inst));
        }
    }

    kaps_c_list_iter_init(&device->other_core->inst_list, &it);
    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_instruction *instr = KAPS_INSTLIST_TO_ENTRY(e1);

        if (instr->cascaded_inst_stale_ptr)
        {
            KAPS_STRY(kaps_instruction_refresh_handle
                      (device, (struct kaps_instruction *) instr->cascaded_inst_stale_ptr, &instr->cascaded_inst));
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_device_restore_state(
    struct kaps_device * bc_device,
    kaps_device_issu_read_fn read_fn,
    kaps_device_issu_write_fn write_fn,
    void *handle)
{
    struct kaps_device *device;
    struct kaps_device *tmp_device;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_wb_cb_functions cb_fun[KAPS_MAX_BROADCAST_DEVICES];
    uint32_t nv_offset[KAPS_MAX_BROADCAST_DEVICES];
    uint32_t nv_start_offset = 0;

    KAPS_TRACE_IN("%p %p %p %p\n", bc_device, read_fn, write_fn, handle);

    if (!bc_device)
        return KAPS_INVALID_ARGUMENT;

    if (bc_device->nv_ptr)
        return kaps_device_recover_state(bc_device);
    else if (!read_fn || !write_fn)
        return KAPS_INVALID_ARGUMENT;

    if (bc_device->main_bc_device && bc_device != bc_device->main_bc_device)
        return KAPS_OK;

    device = bc_device;
    if (bc_device->main_bc_device)
        device = bc_device->main_bc_device;

    kaps_memset (cb_fun, 0, sizeof(cb_fun));

    for (; device; device = device->next_bc_device)
    {
        if (device->is_generic_bc_device)
        {
            continue;
        }
        cb_fun[device->bc_id].nv_offset = &nv_offset[device->bc_id];
        KAPS_STRY(kaps_device_restore_state_partial(device, read_fn, write_fn, handle,
                                                    &cb_fun[device->bc_id], &nv_start_offset));
    }

    KAPS_STRY(kaps_device_lock(bc_device));

    device = bc_device;
    if (bc_device->main_bc_device)
        device = bc_device->main_bc_device;

    for (; device; device = device->next_bc_device)
    {
        for (tmp_device = device; tmp_device; tmp_device = tmp_device->next_dev)
        {
            tmp_device->issu_status = KAPS_ISSU_RESTORE_END;
            if (tmp_device->smt)
            {
                tmp_device->smt->issu_status = KAPS_ISSU_RESTORE_END;
            }
        }
    }

    device = bc_device;
    if (bc_device->main_bc_device)
        device = bc_device->main_bc_device;

    for (; device; device = device->next_bc_device)
    {
        if (device->is_generic_bc_device)
        {
            device->issu_in_progress = NlmFalse;
            if (device->smt)
                device->smt->issu_in_progress = NlmFalse;
            continue;
        }
        if (device->lpm_mgr)
        {
            cb_fun[device->bc_id].read_fn = read_fn;
            cb_fun[device->bc_id].write_fn = write_fn;
            cb_fun[device->bc_id].handle = handle;
            cb_fun[device->bc_id].nv_offset = &nv_offset[device->bc_id];
            cb_fun[device->bc_id].nv_ptr = device->nv_ptr;
            KAPS_STRY(kaps_lpm_wb_restore_state(device, &cb_fun[device->bc_id]));
        }

        /*
         * Restore the HB information
         */

        KAPS_STRY(kaps_hb_wb_restore_state(device, &cb_fun[device->bc_id]));

        /*
         * Restore the databases
         */
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP
                || db->type == KAPS_DB_DMA || db->type == KAPS_DB_HB)
                continue;
            db->device->db_bc_bitmap = db->bc_bitmap;
            KAPS_STRY(db->fn_table->restore_state(db, &cb_fun[device->bc_id]));
            if (db->common_info->defer_deletes_to_install)
            {
                int32_t i;

                if (0 !=
                    cb_fun[device->bc_id].read_fn(cb_fun[device->bc_id].handle,
                                                  (uint8_t *) & db->common_info->num_pending_del_entries,
                                                  sizeof(db->common_info->num_pending_del_entries),
                                                  *cb_fun[device->bc_id].nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;
                *cb_fun[device->bc_id].nv_offset += sizeof(db->common_info->num_pending_del_entries);

                if (0 !=
                    cb_fun[device->bc_id].read_fn(cb_fun[device->bc_id].handle,
                                                  (uint8_t *) & db->common_info->max_pending_del_entries,
                                                  sizeof(db->common_info->max_pending_del_entries),
                                                  *cb_fun[device->bc_id].nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;
                *cb_fun[device->bc_id].nv_offset += sizeof(db->common_info->max_pending_del_entries);

                for (i = 0; i < db->common_info->num_pending_del_entries; i++)
                {
                    uint32_t user_handle;

                    if (0 != cb_fun[device->bc_id].read_fn(cb_fun[device->bc_id].handle, (uint8_t *) & user_handle,
                                                           sizeof(user_handle), *cb_fun[device->bc_id].nv_offset))
                        return KAPS_NV_READ_WRITE_FAILED;
                    *cb_fun[device->bc_id].nv_offset += sizeof(user_handle);
                    if (db->type == KAPS_DB_LPM)
                        db->common_info->del_entry_list[i] = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, user_handle);
                    else
                        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), &db->common_info->del_entry_list[i],
                                                (uintptr_t) user_handle);
                }
            }
        }

        KAPS_STRY(kaps_lpm_wb_post_processing(device));

        /*
         * LPM Pending Restore after Post Processing 
         */
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db->type != KAPS_DB_LPM)
                continue;
            if (db->common_info->defer_deletes_to_install)
            {
                int32_t i;

                for (i = 0; i < db->common_info->num_pending_del_entries; i++)
                {
                    KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), &db->common_info->del_entry_list[i],
                                            (uintptr_t) db->common_info->del_entry_list[i]);
                }
            }
        }

        /*
         * Making to zero, we will make it to one when user call the device_lock, so that we can block the APIs which
         * should be called after the device lock 
         */
        device->is_config_locked = 0;
    }

    device = bc_device;
    if (bc_device->main_bc_device)
        device = bc_device->main_bc_device;

    for (; device; device = device->next_bc_device)
    {
        struct kaps_device *tmp_device;

        tmp_device = device;
        while (tmp_device)
        {
            tmp_device->issu_in_progress = NlmFalse;
            if (tmp_device->smt)
                tmp_device->smt->issu_in_progress = NlmFalse;

            tmp_device = tmp_device->next_dev;
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_device_reconcile_start(
    struct kaps_device * bc_device)
{
    KAPS_TRACE_IN("%p\n", bc_device);
    if (!bc_device)
        return KAPS_INVALID_DEVICE_PTR;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_device *device = bc_device;
        struct kaps_c_list_iter it;
        struct kaps_list_node *e1;
        struct kaps_device *cascade;

        if (device->is_generic_bc_device)
        {
            continue;
        }

        if (!(device->flags & KAPS_DEVICE_ISSU))
            return KAPS_INVALID_FLAGS;

        if (device->issu_status != KAPS_ISSU_RESTORE_END)
            return KAPS_ISSU_RESTORE_REQUIRED;

        kaps_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(e1);

            KAPS_TRY(kaps_db_reconcile_start(tmp));
        }
        for (cascade = device; cascade; cascade = cascade->next_dev)
        {
            cascade->issu_status = KAPS_ISSU_RECONCILE_START;
            if (cascade->smt)
                cascade->smt->issu_status = KAPS_ISSU_RECONCILE_START;
        }
    }
    return KAPS_OK;
}

kaps_status
kaps_device_reconcile_end(
    struct kaps_device * bc_device)
{
    KAPS_TRACE_IN("%p\n", bc_device);
    if (!bc_device)
        return KAPS_INVALID_DEVICE_PTR;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_device *device = bc_device;
        struct kaps_c_list_iter it;
        struct kaps_list_node *e1;
        struct kaps_device *cascade;

        if (device->is_generic_bc_device)
        {
            continue;
        }

        if (!(device->flags & KAPS_DEVICE_ISSU))
            return KAPS_INVALID_FLAGS;

        if (device->issu_status != KAPS_ISSU_RECONCILE_START)
            return KAPS_ISSU_RECONCILE_REQUIRED;

        for (cascade = device; cascade; cascade = cascade->next_dev)
        {
            cascade->issu_status = KAPS_ISSU_RECONCILE_END;
            if (cascade->smt)
                cascade->smt->issu_status = KAPS_ISSU_RECONCILE_END;
        }

        kaps_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(e1);

            if (tmp->type == KAPS_DB_AD || tmp->type == KAPS_DB_COUNTER || tmp->type == KAPS_DB_TAP
                || tmp->type == KAPS_DB_DMA || tmp->type == KAPS_DB_HB)
                continue;
            KAPS_TRY(kaps_db_reconcile_end(tmp));
        }
    }
    return KAPS_OK;
}

int32_t
kaps_device_wb_file_read(
    FILE * fp,
    uint8_t * buffer,
    uint32_t size,
    uint32_t offset)
{
    uint32_t i;
    int32_t retval;

    /*
     * position the file pointer to the offset from beginning of the file 
     */
    (void) kaps_fseek(fp, offset, SEEK_SET);

    for (i = 0; i < size; i++)
    {
        retval = fgetc(fp);
        if (retval == EOF)
            return -1;
        else
            buffer[i] = (uint8_t) retval;
    }

    return 0;
}
