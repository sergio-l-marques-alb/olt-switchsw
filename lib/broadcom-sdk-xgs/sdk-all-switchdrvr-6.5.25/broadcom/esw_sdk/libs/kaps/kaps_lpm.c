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

#include "kaps_fib_trie.h"
#include "kaps_handle.h"
#include "kaps_algo_common.h"
#include "kaps_device_wb.h"
#include "kaps_key_internal.h"
#include "kaps_cr_pool_mgr.h"
#include "kaps_errors.h"

#define LPM_MAX_WIDTH       (160)
#define LPM_MAX_AB_LIT_UP   (2)

static kaps_status kaps_lpm_db_delete_all_entries(
    struct kaps_db *in_db);

static kaps_status
kaps_lpm_db_get_data_len(
    struct kaps_db *in_db,
    struct kaps_entry *in_e,
    uint32_t * len,
    uint8_t ** data)
{
    struct kaps_lpm_entry *e = (struct kaps_lpm_entry *) in_e;

    *len = e->pfx_bundle->m_nPfxSize;
    *data = e->pfx_bundle->m_data;
    return KAPS_OK;
}

static kaps_status
kaps_map_reason_code_to_kaps_status(
    NlmReasonCode reason)
{
    kaps_status status = KAPS_OK;

    if (reason == NLMRSC_REASON_OK)
        status = KAPS_OK;
    else if (reason == NLMRSC_LOW_MEMORY)
        status = KAPS_OUT_OF_MEMORY;
    else if (reason == NLMRSC_UDA_ALLOC_FAILED)
        status = KAPS_OUT_OF_UDA;
    else if (reason == NLMRSC_DBA_ALLOC_FAILED)
        status = KAPS_OUT_OF_DBA;
    else if (reason == NLMRSC_IDX_RANGE_FULL)
        status = KAPS_OUT_OF_INDEX;
    else if (reason == NLMRSC_PCM_ALLOC_FAILED)
        status = KAPS_EXHAUSTED_PCM_RESOURCE;
    else if (reason == NLMRSC_IT_ALLOC_FAILED)
        status = KAPS_OUT_OF_UIT;
    else if (reason == NLMRSC_DUPLICATE_PREFIX)
        status = KAPS_DUPLICATE;
    else if (reason == NLMRSC_XPT_FAILED)
        status = KAPS_FATAL_TRANSPORT_ERROR;
    else if (reason == NLMRSC_OUT_OF_AD)
        status = KAPS_OUT_OF_AD;
    else if (reason == NLMRSC_RETRY_WITH_RPT_SPLIT)
        status = KAPS_OUT_OF_DBA;
    else
        status = KAPS_INTERNAL_ERROR;

    return status;
}

static void
kaps_fib_index_change_cb(
    void *client_p,
    void *fib_tbl_p,
    kaps_pfx_bundle * b,
    uint32_t old_index,
    uint32_t new_index)
{
    struct kaps_lpm_mgr *lpm_mgr = (struct kaps_lpm_mgr *) client_p;
    struct kaps_entry *user_handle = NULL, *e = NULL;

    if (lpm_mgr->fib_tbl_mgr->m_devMgr_p->issu_in_progress)
        return;

    if (lpm_mgr->callback_fn)
    {
        if (b)
        {
            e = (struct kaps_entry *) b->m_backPtr;
            KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(lpm_mgr->fib_tbl_mgr->m_devMgr_p, e, lpm_mgr->curr_db);
            if (lpm_mgr->fib_tbl_mgr->m_devMgr_p->flags & KAPS_DEVICE_ISSU)
            {
                user_handle =
                    KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, ((struct kaps_entry *) b->m_backPtr)->user_handle);
            }
            else
            {
                user_handle = (struct kaps_entry *) b->m_backPtr;
            }
        }
        if (lpm_mgr->curr_db->common_info->calc_shuffle_stats)
            lpm_mgr->curr_db->common_info->pio_stats.num_ix_cbs++;      /* number of index change calls */
        if (lpm_mgr->curr_db->common_info->callback_fn)
            lpm_mgr->curr_db->common_info->callback_fn(lpm_mgr->curr_db->common_info->callback_handle, lpm_mgr->curr_db,
                                                       user_handle, old_index, new_index);
    }

    return;
}

static kaps_status
kaps_lpm_db_destroy(
    struct kaps_db *in_db)
{
    struct kaps_device *device = NULL;
    NlmReasonCode reason;
    struct kaps_lpm_db *tmp;
    struct kaps_ad_db *ad_db;
    int32_t i, j;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;

    if (!db)
        return KAPS_OK;

    if (db->db_info.parent)
        db = (struct kaps_lpm_db *) db->db_info.parent;

    tmp = db;
    while (tmp)
    {
        struct kaps_lpm_db *next = (struct kaps_lpm_db *) tmp->db_info.next_tab;

        tmp->db_info.is_destroy_in_progress = 1;
        kaps_lpm_db_delete_all_entries((struct kaps_db *) tmp);
        tmp->db_info.is_destroy_in_progress = 0;

        tmp = next;
    }

    tmp = db;
    while (tmp)
    {
        struct kaps_lpm_db *next = (struct kaps_lpm_db *) tmp->db_info.next_tab;

        if (tmp->db_info.key)
            kaps_key_destroy_internal(tmp->db_info.key);

        if (tmp->fib_tbl)
            kaps_ftm_destroy_table(tmp->fib_tbl, &reason);

        device = tmp->db_info.device;

        /*
         * remove db information from the instructions
         */
        if (tmp->db_info.instructions)
        {
            for (i = 0; i < tmp->db_info.ninstructions; i++)
            {
                struct kaps_instruction *inst = tmp->db_info.instructions[i];
                for (j = 0; j < inst->num_searches; j++)
                {
                    if (tmp == (struct kaps_lpm_db *) inst->desc[j].db)
                        inst->desc[j].db = NULL;
                }
            }

            device->alloc->xfree(device->alloc->cookie, tmp->db_info.instructions);
            tmp->db_info.instructions = NULL;
        }

        POOL_FINI(kaps_lpm_entry, &tmp->lpm_entry_pool);

        ad_db = (struct kaps_ad_db *) tmp->db_info.common_info->ad_info.ad;
        while (ad_db)
        {
            ad_db->db_info.common_info->ad_info.db = NULL;
            ad_db = ad_db->next;
        }

        if (tmp->resolution_dbs)
        {
            kaps_sassert(tmp->num_resolution_db != 0);
            device->alloc->xfree(device->alloc->cookie, tmp->resolution_dbs);
        }

        if (tmp != db)
            device->alloc->xfree(device->alloc->cookie, tmp);
        tmp = next;
    }

    /*Destroy the IX Managers for all the AD DBs under this DB*/
    kaps_ix_mgr_destroy_for_all_ad_dbs(device, 
                (struct kaps_ad_db*) db->db_info.common_info->ad_info.ad);


    if (db->db_info.common_info->del_entry_list)
    {
        device->alloc->xfree(device->alloc->cookie, db->db_info.common_info->del_entry_list);
        db->db_info.common_info->del_entry_list = NULL;
    }

    db->db_info.is_destroyed = 1;

    return KAPS_OK;
}

static kaps_status
kaps_get_num_lpm_mgr(
    struct kaps_device *main_device,
    struct kaps_device *this_device,
    kaps_db_index_callback * cb_fn,
    void **cb_handle,
    uint32_t * num_lpm)
{
    int32_t nlpm = 0;

    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    /*
     * Collect the resource information from the first
     * database only.
     */

    kaps_c_list_iter_init(&main_device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(el);

        if (tmp->type != KAPS_DB_LPM)
            continue;

        nlpm++;

        /*
         * The user may install different callbacks for
         * the different LPM databases
         */

        *cb_fn = tmp->common_info->callback_fn;
        *cb_handle = tmp->common_info->callback_handle;

    }

    *num_lpm = nlpm;
    return KAPS_OK;
}

static kaps_status
kaps_finalize_fib_databases(
    struct kaps_device *main_device,
    struct kaps_device *this_device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_lpm_mgr *lpm_mgr = this_device->lpm_mgr;
    uint32_t internal_table_width_1;
    kaps_status status;

    /*
     * Create lpm tables for each lpm db 
     */
    kaps_c_list_iter_init(&main_device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        kaps_fib_tbl_index_range index_range;
        NlmReasonCode reason;
        struct kaps_db *db, *parent = KAPS_SSDBLIST_TO_ENTRY(el);

        if (parent->type != KAPS_DB_LPM)
            continue;
        if (parent->is_bc_required && (!parent->is_main_bc_db))
            continue;

        /*
         * For the KAPS device ensure that all the LPM databases have ad db associated with them
         */
        if (main_device->type == KAPS_DEVICE_KAPS && !parent->common_info->ad_info.ad)
            return KAPS_AD_DB_ABSENT;

        parent->device->db_bc_bitmap = parent->bc_bitmap;

        kaps_assert(KAPS_HW_MAX_LPUS_PER_LSN <= 8,
                    "The max LSN size passed on to kaps_ix_mgr_init may be greater than 1024");


        if (parent->common_info->is_cascaded)
            kaps_assert(parent->device->core_id == 0,
                        "Assumption is that cascaded databases has to be on device core 0");

        status = kaps_ix_mgr_init_for_all_ad_dbs(main_device,  1024,
                                  (struct kaps_ad_db *) parent->common_info->ad_info.ad,
                                  main_device->issu_status == KAPS_ISSU_INIT ? 0 : 1);

        if (status != KAPS_OK)
            return status;


        for (db = parent; db; db = db->next_tab)
        {
            if (db->is_clone)
                continue;

            if (db->common_info->index_range_max < db->common_info->index_range_min)
                return KAPS_INVALID_INDEX_RANGE;

            if (db->common_info->index_range_max == 0)
                db->common_info->index_range_max = KAPS_FIB_MAX_INDEX_RANGE;

            index_range.m_indexLowValue = db->common_info->index_range_min;
            index_range.m_indexHighValue = db->common_info->index_range_max;

            internal_table_width_1 = db->width.key_width_1;
            if (db->common_info->is_extra_byte_added)
                internal_table_width_1 += KAPS_BITS_IN_BYTE;

            lpm_mgr = parent->lpm_mgr;
            db->lpm_mgr = lpm_mgr;

            ((struct kaps_lpm_db *) db)->fib_tbl = kaps_ftm_create_table(lpm_mgr->fib_tbl_mgr,
                                                                         db->ltr_db_id,
                                                                         &index_range,
                                                                         internal_table_width_1, db, 0, &reason);

            if (((struct kaps_lpm_db *) db)->fib_tbl == NULL)
                return kaps_map_reason_code_to_kaps_status(reason);

            ((struct kaps_lpm_db *) db)->core0_fib_tbl = ((struct kaps_lpm_db *) db)->fib_tbl;

            if (parent->common_info->is_cascaded)
            {
                struct kaps_lpm_db *lpm_db = ((struct kaps_lpm_db *) db);

                lpm_db->core1_fib_tbl = kaps_ftm_create_table(lpm_mgr->fib_tbl_mgr,
                                                              db->ltr_db_id,
                                                              &index_range, internal_table_width_1, db, 1, &reason);
                if (((struct kaps_lpm_db *) db)->core1_fib_tbl == NULL)
                    return kaps_map_reason_code_to_kaps_status(reason);

                lpm_db->core1_fib_tbl->m_trie->m_hashtable_p = lpm_db->fib_tbl->m_trie->m_hashtable_p;
                /*
                 * Roll Back 
                 */
                lpm_mgr->fib_tbl_mgr->m_trieGlobal = lpm_mgr->fib_tbl_mgr->m_core0trieGlobal;
            }

            /*
             * Find out the widest table. This is required for implicit BMR configurations for IPT 
             */
            if (lpm_mgr->fib_tbl_mgr->m_maxTblWidth_8 < internal_table_width_1 / KAPS_BITS_IN_BYTE)
                lpm_mgr->fib_tbl_mgr->m_maxTblWidth_8 = internal_table_width_1 / KAPS_BITS_IN_BYTE;
        }
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_lock(
    struct kaps_device *device)
{
    NlmReasonCode reason_code;
    kaps_db_index_callback callback_fn = NULL;
    void *callback_handle = NULL;
    uint32_t num_lpm = 0, nlpm = 0;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    /*
     * Create fib tbl mgr 
     */
    if (device->type == KAPS_DEVICE_KAPS)
    {
        KAPS_STRY(kaps_get_num_lpm_mgr(device, device, &callback_fn, &callback_handle, &num_lpm));
        device->lpm_mgr = device->alloc->xcalloc(device->alloc->cookie, 1, (num_lpm * sizeof(struct kaps_lpm_mgr)));
        if (!device->lpm_mgr)
            return KAPS_OUT_OF_MEMORY;

        /* kaps_memset(device->lpm_mgr, 0, num_lpm * sizeof(struct kaps_lpm_mgr));*/

        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *tmp_db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (tmp_db->type != KAPS_DB_LPM)
                continue;

            device->lpm_mgr[nlpm].curr_db = tmp_db;
            device->lpm_mgr[nlpm].resource_db = tmp_db;
            tmp_db->lpm_mgr = &device->lpm_mgr[nlpm];

            device->lpm_mgr[nlpm].fib_tbl_mgr = kaps_ftm_kaps_init(&device->nlm_alloc,
                                                                   device, &device->lpm_mgr[nlpm],
                                                                   kaps_fib_index_change_cb, &reason_code);

            device->lpm_mgr[nlpm].callback_fn = tmp_db->common_info->callback_fn;
            device->lpm_mgr[nlpm].callback_handle = tmp_db->common_info->callback_handle;

            nlpm++;
        }

        KAPS_STRY(kaps_finalize_fib_databases(device, device));

        while (nlpm)
        {
            device->lpm_mgr[--nlpm].fib_tbl_mgr->m_IsConfigLocked = 1;
            device->lpm_mgr[nlpm].is_config_locked = 1;
        }

    }
    else
    {
        device->alloc->xfree(device->alloc->cookie, device->lpm_mgr);
        return KAPS_INVALID_ARGUMENT;
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_destroy(
    struct kaps_device * device)
{
    struct kaps_device *tmp;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    NlmErrNum_t err_num = NLMERR_OK;
    NlmReasonCode reason;

    for (tmp = device; tmp; tmp = tmp->next_dev)
    {
        kaps_c_list_iter_init(&tmp->db_list, &it);
        {
            while ((el = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_db *tmp_db = KAPS_SSDBLIST_TO_ENTRY(el);

                if (tmp_db->type != KAPS_DB_LPM)
                    continue;

                if (tmp_db->hw_res.db_res->small_bb_brick_bitmap)
                {
                    device->alloc->xfree(device->alloc->cookie,
                            tmp_db->hw_res.db_res->small_bb_brick_bitmap);
                }

                /*
                 * For KAPS and OP the DBs are not completely destroyed in the db_destroy call, do the rest here 
                 */

                if (tmp_db->lpm_mgr)
                    err_num = kaps_ftm_destroy(tmp_db->lpm_mgr->fib_tbl_mgr, &reason);
                if (err_num != NLMERR_OK)
                    return kaps_map_reason_code_to_kaps_status(reason);
                kaps_c_list_remove_node(&tmp->db_list, el, &it);
                if (tmp_db->fn_table)
                    device->alloc->xfree(device->alloc->cookie, tmp_db->fn_table);
                KAPS_WB_HANDLE_TABLE_DESTROY(tmp_db);
                kaps_resource_free_database(device, tmp_db);
                device->alloc->xfree(device->alloc->cookie, tmp_db);
            }
            if (tmp->lpm_mgr)
            {
                kaps_arena_destroy(&device->nlm_alloc);
            }
        }

        if (tmp->lpm_mgr)
        {
            device->alloc->xfree(device->alloc->cookie, tmp->lpm_mgr);
            tmp->lpm_mgr = NULL;
        }
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_add_key(
    struct kaps_db *in_db,
    struct kaps_key *key)
{
    struct kaps_key_field *f;
    uint32_t cnt_key_pfx = 0, cnt_key_tid = 0;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;

    if (db == NULL || key == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.key)
        return KAPS_DUPLICATE_KEY;

    /*
     * Validate fields in LPM key, for tables need to check the table id position
     */
    f = key->first_field;
    while (f)
    {
        if (f->type == KAPS_KEY_FIELD_TERNARY)
        {
            return KAPS_LPM_TERNARY;
        }
        else if (f->type == KAPS_KEY_FIELD_RANGE)
        {
            return KAPS_LPM_RANGE;
        }
        else if (f->type == KAPS_KEY_FIELD_PREFIX)
        {
            cnt_key_pfx++;
            if (cnt_key_pfx > 1)
                return KAPS_MULTIPLE_PREFIX_FIELDS;
        }
        else if (f->type == KAPS_KEY_FIELD_TABLE_ID)
        {
            cnt_key_tid++;
            if (cnt_key_tid > 1)
                return KAPS_MULTIPLE_TBLID_FIELDS;
            if (cnt_key_pfx)    /* prefix cannot followed by other key types */
                return KAPS_LPM_INVALID_KEY_FORMAT;
        }
        else if (f->type == KAPS_KEY_FIELD_EM)
        {
            if (cnt_key_pfx)    /* prefix cannot followed by other key types */
                return KAPS_LPM_INVALID_KEY_FORMAT;
        }
        f = f->next;
    }

    db->db_info.width.key_width_1 = key->width_1;
    return kaps_resource_db_set_key(db->db_info.device, &db->db_info, key);
}

static kaps_status
kaps_lpm_db_add_entry(
    struct kaps_db *in_db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_entry **o_entry)
{
    struct kaps_lpm_entry *entry, *dup_entry;
    NlmErrNum_t err_num;
    uint8_t add_extra_first_byte;
    struct kaps_device *device = NULL;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;

    if (!db || !prefix || !o_entry)
        return KAPS_INVALID_ARGUMENT;

    if (db->fib_tbl->m_width < length)
        return KAPS_INVALID_ARGUMENT;

    device = db->db_info.device;
    if (device->type != KAPS_DEVICE_KAPS && length < 8 && !in_db->common_info->is_extra_byte_added)
        return KAPS_INVALID_ARGUMENT;

    POOL_ALLOC(kaps_lpm_entry, &db->lpm_entry_pool, entry);
    if (!entry)
        return KAPS_OUT_OF_MEMORY;

    kaps_memset(entry, 0, sizeof(*entry));

    /*
     * Add an extra zero byte to the entry at the MS position for NL12K and Oprime This is helps us reduce the number
     * of RPT entries. Otherwise if the user adds entries where the first byte has all the 256 combinations then we
     * will be generating 256 RPT entries Also adding the first byte allows the user to add entries that are shorter
     * than 8b The side effect of this is that the max table width can only be 152b for these devices 
     */
    add_extra_first_byte = in_db->common_info->is_extra_byte_added;

    /*
     * If this is a replayed prefix no need to add an extra byte 
     */
    if (device->issu_status != KAPS_ISSU_INIT)
        add_extra_first_byte = 0;

    err_num = kaps_arena_activate_arena(db->fib_tbl->m_alloc_p, NULL);
    if (err_num != NLMERR_OK)
        return KAPS_OUT_OF_MEMORY;

    entry->pfx_bundle = kaps_pfx_bundle_create_from_string(db->fib_tbl->m_alloc_p, prefix,
                                                           length, KAPS_LSN_NEW_INDEX, 0, add_extra_first_byte);

    kaps_arena_deactivate_arena(db->fib_tbl->m_alloc_p);

    if (!entry->pfx_bundle)
    {
        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
        return KAPS_OUT_OF_MEMORY;
    }

    /*
     * check if entry is a duplicate
     */
    dup_entry = NULL;
    err_num = kaps_ftm_get_prefix_handle(db->fib_tbl, entry->pfx_bundle->m_data,
                                         entry->pfx_bundle->m_nPfxSize, &dup_entry);
    if (err_num != NLMERR_OK)
    {
        kaps_pfx_bundle_destroy(entry->pfx_bundle, &device->nlm_alloc);
        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
        return KAPS_INTERNAL_ERROR;
    }

    if (dup_entry != NULL)
    {
        kaps_pfx_bundle_destroy(entry->pfx_bundle, &device->nlm_alloc);
        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
        return KAPS_DUPLICATE;
    }

    err_num = kaps_ftm_update_hash_table(db->fib_tbl, entry, 0);
    if (err_num != NLMERR_OK)
    {
        kaps_pfx_bundle_destroy(entry->pfx_bundle, &device->nlm_alloc);
        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
        return KAPS_INTERNAL_ERROR;
    }
    entry->pfx_bundle->m_status = PENDING;

    entry->pfx_bundle->m_backPtr = entry;
    kaps_c_list_add_tail(&db->pending, &entry->db_node);
    entry->db_seq_num = db->db_info.seq_num;
    entry->meta_priority = db->db_info.common_info->meta_priority;

    *o_entry = (struct kaps_entry *) entry;
    return KAPS_OK;
}

kaps_status
kaps_lpm_db_wb_add_entry(
    struct kaps_lpm_db * db,
    uint8_t * prefix,
    uint32_t length,
    uint32_t index,
    struct kaps_lpm_entry ** entry_p,
    uint32_t meta_priority)
{
    struct kaps_lpm_entry *entry;
    struct kaps_ad_db *cur_ad_db;
    NlmErrNum_t err_num;
    NlmReasonCode reason_code;

    if (!db || !prefix)
        return KAPS_INVALID_ARGUMENT;

    if (db->fib_tbl->m_width < length)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->type != KAPS_DEVICE_KAPS && length < 8)
        return KAPS_INVALID_ARGUMENT;

    if (index == KAPS_LSN_NEW_INDEX)
    {
        return kaps_lpm_db_add_entry(&db->db_info, prefix, length, (struct kaps_entry **) entry_p);
    }

    POOL_ALLOC(kaps_lpm_entry, &db->lpm_entry_pool, entry);

    if (!entry)
        return KAPS_OUT_OF_MEMORY;

    kaps_memset(entry, 0, sizeof(*entry));

    err_num = kaps_arena_activate_arena(db->fib_tbl->m_alloc_p, &reason_code);
    if (err_num != NLMERR_OK)
        return KAPS_OUT_OF_MEMORY;

    entry->pfx_bundle = kaps_pfx_bundle_create_from_string(db->fib_tbl->m_alloc_p, prefix,
                                                           length, KAPS_LSN_NEW_INDEX, 0, 0);

    kaps_arena_deactivate_arena(db->fib_tbl->m_alloc_p);

    if (!entry->pfx_bundle)
    {
        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
        return KAPS_OUT_OF_MEMORY;
    }

    entry->meta_priority = meta_priority;
    entry->pfx_bundle->m_backPtr = entry;

    err_num = kaps_ftm_wb_add_prefix(db->fib_tbl, entry, index, &reason_code);

    if (err_num != NLMERR_OK)
        return kaps_map_reason_code_to_kaps_status(reason_code);

    db->db_info.common_info->cur_capacity++;
    if (entry->ad_handle)
    {
        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->db_info.device, entry->ad_handle, cur_ad_db);
        cur_ad_db->num_entries[entry->core]++;
    }

    entry->pfx_bundle->m_status = COMMITTED;
    entry->db_seq_num = db->db_info.seq_num;
    kaps_c_list_add_tail(&db->db_info.db_list, &entry->db_node);

    *entry_p = entry;
    return KAPS_OK;
}

kaps_status
kaps_lpm_db_wb_add_pfx_copy(
    struct kaps_lpm_db * db,
    uint8_t * prefix,
    uint32_t length,
    uint32_t index,
    struct kaps_pfx_bundle ** pfx_bundle_p)
{
    struct kaps_lpm_entry entry;
    struct kaps_ad_db *cur_ad_db;
    NlmErrNum_t err_num;
    NlmReasonCode reason_code;

    if (!db || !prefix)
        return KAPS_INVALID_ARGUMENT;

    if (db->fib_tbl->m_width < length)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->type != KAPS_DEVICE_KAPS && length < 8)
        return KAPS_INVALID_ARGUMENT;

    if (index == KAPS_LSN_NEW_INDEX)
    {
        return KAPS_INVALID_ARGUMENT;
    }

    kaps_memset(&entry, 0, sizeof(entry));

    err_num = kaps_arena_activate_arena(db->fib_tbl->m_alloc_p, &reason_code);
    if (err_num != NLMERR_OK)
        return KAPS_OUT_OF_MEMORY;

    entry.pfx_bundle = kaps_pfx_bundle_create_from_string(db->fib_tbl->m_alloc_p, prefix,
                                                          length, KAPS_LSN_NEW_INDEX, 0, 0);

    kaps_arena_deactivate_arena(db->fib_tbl->m_alloc_p);

    if (!entry.pfx_bundle)
    {
        return KAPS_OUT_OF_MEMORY;
    }

    entry.pfx_bundle->m_isPfxCopy = 1;

    err_num = kaps_ftm_wb_add_prefix(db->fib_tbl, &entry, index, &reason_code);

    if (err_num != NLMERR_OK)
        return kaps_map_reason_code_to_kaps_status(reason_code);

    db->db_info.common_info->cur_capacity++;
    if (entry.ad_handle)
    {
        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->db_info.device, entry.ad_handle, cur_ad_db);
        cur_ad_db->num_entries[entry.core]++;
    }

    if (db->db_info.common_info->is_cascaded)
        entry.core = db->db_info.device->core_id;

    *pfx_bundle_p = entry.pfx_bundle;

    return KAPS_OK;
}

#if 0
static void
kaps_remove_from_pending_list(
    struct kaps_db *db,
    struct kaps_entry *e)
{
    uint32_t i, j, found = 0, pending_count, e_offset =
        kaps_db_entry_get_nv_offset(db, (uint32_t) ((uintptr_t) e->user_handle));
    uint8_t *cur_nv_ptr, *next_nv_ptr, *nv_ptr = db->device->nv_ptr;
    struct kaps_db *main_db = db;

    if (db->parent)
        main_db = db->parent;

    nv_ptr += db->device->nv_mem_mgr->offset_device_pending_list;
    pending_count = *(uint32_t *) nv_ptr;
    nv_ptr += sizeof(uint32_t);

    for (i = 0; i < pending_count; i++)
    {
        struct kaps_lpm_cr_entry_info *pending_entry = NULL;
        struct kaps_db *cur_db;
        uint8_t *pending_nv_ptr = db->device->nv_ptr;
        uint32_t entry_offset;

        entry_offset = *(uint32_t *) nv_ptr;
        if (entry_offset == e_offset)
        {
            found = 1;

            cur_nv_ptr = db->device->nv_ptr;
            cur_nv_ptr += db->device->nv_mem_mgr->offset_device_pending_list;
            cur_nv_ptr += sizeof(uint32_t);

            cur_nv_ptr += (i * sizeof(uint32_t));
            next_nv_ptr = cur_nv_ptr + sizeof(uint32_t);

            for (j = i; j < pending_count - 1; j++)
            {

                *(uint32_t *) cur_nv_ptr = *(uint32_t *) next_nv_ptr;
                cur_nv_ptr += sizeof(uint32_t);
                next_nv_ptr += sizeof(uint32_t);
            }

            nv_ptr = db->device->nv_ptr;
            nv_ptr += db->device->nv_mem_mgr->offset_device_pending_list;
            pending_count = *(uint32_t *) nv_ptr;
            *(uint32_t *) nv_ptr = pending_count - 1;
            return;
        }
        nv_ptr += sizeof(uint32_t);

        pending_nv_ptr += entry_offset;
        pending_entry = (struct kaps_lpm_cr_entry_info *) pending_nv_ptr;

        cur_db = db->device->seq_num_to_ptr->db_ptr[pending_entry->seq_num];
        if (cur_db->parent)
            cur_db = cur_db->parent;

        /*
         * This entry not belongs to this data base, so skip it
         */
        if (cur_db != main_db)
        {
            if ((main_db->device->seq_num_to_ptr->db_ptr[pending_entry->seq_num]->type == KAPS_DB_LPM)
                && pending_entry->marked_for_delete)
            {
                i += 6;
                nv_ptr += (6 * sizeof(uint32_t));
            }
        }
    }
    kaps_sassert(found);
}
#endif

static kaps_status
kaps_lpm_db_delete_entry(
    struct kaps_db *in_db,
    struct kaps_entry *in_e)
{
    NlmErrNum_t err_num;
    NlmReasonCode reason_code;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    struct kaps_lpm_entry *entry = (struct kaps_lpm_entry *) in_e;
    struct kaps_allocator *alloc = in_db->device->alloc;
    struct kaps_ad_db *cur_ad_db = NULL;
    int core_id = 0;

    if (db == NULL || entry == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (in_db->common_info->defer_deletes_to_install)
    {
        if (entry->pfx_bundle->m_status == PENDING)
        {
            KAPS_WB_HANDLE_DELETE_LOC(KAPS_GET_DB_PARENT(in_db), (uintptr_t) in_e->user_handle);
            /*
             * We are trying to delete an entry that is still not been installed. In this case kaps_db_delete_entry has 
             * already set marked_for_delete = 1 in the kaps_lpm_cr_entry_info before calling this function. Now if we
             * allow marked for delete to continue to be 1, then when kaps_lpm_db_cr_create_index_handle_map is called,
             * we will end up reading 6 extra words from the nv pending list, which causes the pending list reading to
             * go haywire. So we are setting marked_for_delete to 0 here
             */
            if (in_db->device->nv_ptr && in_db->device->issu_status == KAPS_ISSU_INIT)
            {
                struct kaps_lpm_cr_entry_info *cr_delete_entry;
                uint8_t *nv_ptr = in_db->device->nv_ptr;
                uint32_t offset = kaps_db_entry_get_nv_offset(in_db, entry->user_handle);

                nv_ptr += offset;
                cr_delete_entry = (struct kaps_lpm_cr_entry_info *) nv_ptr;
                cr_delete_entry->marked_for_delete = 0;
            }
            /* kaps_remove_from_pending_list(in_db, in_e);*/
        }
        else
        {
            if (in_db->common_info->num_pending_del_entries >= in_db->common_info->max_pending_del_entries)
            {
                struct kaps_entry **new_ptr;

                in_db->common_info->max_pending_del_entries *= 2;
                new_ptr =
                    alloc->xcalloc(alloc->cookie, in_db->common_info->max_pending_del_entries,
                                   sizeof(struct kaps_entry *));
                if (!new_ptr)
                    return KAPS_OUT_OF_MEMORY;

                kaps_memcpy(new_ptr, in_db->common_info->del_entry_list,
                            sizeof(struct kaps_entry *) * in_db->common_info->num_pending_del_entries);

                alloc->xfree(alloc->cookie, in_db->common_info->del_entry_list);
                in_db->common_info->del_entry_list = new_ptr;
            }

            in_db->common_info->del_entry_list[in_db->common_info->num_pending_del_entries] = in_e;
            in_db->common_info->num_pending_del_entries++;

            if (in_db->common_info->defer_deletes_to_install
                && in_db->device->nv_ptr && in_db->device->issu_status == KAPS_ISSU_INIT)
            {
                uint32_t offset;
                uint8_t *nv_ptr = in_db->device->nv_ptr;
                uint8_t *pending_ptr = NULL;
                uint32_t pending_count = 0;
                int32_t start_bit = 0, end_bit;
                uint8_t pfx_data[KAPS_LPM_KEY_MAX_WIDTH_8];
                uint32_t pfx_val;

                offset = in_db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;

                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;
                nv_ptr += sizeof(uint32_t);

                if (kaps_cr_pool_mgr_is_nv_exhausted(in_db->device, sizeof(uint32_t) * 6))
                    return KAPS_EXHAUSTED_NV_MEMORY;

                nv_ptr += ((pending_count) * sizeof(uint32_t));

                *(uint32_t *) nv_ptr = entry->pfx_bundle->m_nPfxSize;
                *(uint32_t *) pending_ptr = (pending_count + 1);
                nv_ptr += sizeof(uint32_t);
                pending_count++;

                kaps_memset(pfx_data, 0, KAPS_LPM_KEY_MAX_WIDTH_8);
                kaps_memcpy(pfx_data, entry->pfx_bundle->m_data, (entry->pfx_bundle->m_nPfxSize + 7) / 8);
                for (start_bit = 0; start_bit < KAPS_LPM_KEY_MAX_WIDTH_1; start_bit += 32)
                {
                    end_bit = start_bit + 31;

                    pfx_val = KapsReadBitsInArrray(pfx_data, KAPS_LPM_KEY_MAX_WIDTH_8, end_bit, start_bit);
                    *(uint32_t *) nv_ptr = pfx_val;
                    *(uint32_t *) pending_ptr = (pending_count + 1);
                    nv_ptr += sizeof(uint32_t);
                    pending_count++;
                }
            }

            return KAPS_OK;
        }
    }

    if (entry->ad_handle)
        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->db_info.device, entry->ad_handle, cur_ad_db);

    core_id = entry->core;
    if (in_db->device->nv_ptr && in_db->device->issu_status == KAPS_ISSU_INIT)
        KAPS_STRY(in_db->fn_table->cr_delete_entry(in_db, in_e, 1));

    if (entry->pfx_bundle->m_status == PENDING)
    {
        kaps_c_list_remove_node(&db->pending, &entry->db_node, NULL);
    }
    else
    {
        err_num = kaps_ftm_delete_prefix(db->fib_tbl, entry, &reason_code);
        if (err_num != NLMERR_OK)
            return kaps_map_reason_code_to_kaps_status(reason_code);

        kaps_c_list_remove_node(&db->db_info.db_list, &entry->db_node, NULL);
    }
    err_num = kaps_ftm_update_hash_table(db->fib_tbl, entry, 1);
    if (err_num != NLMERR_OK)
    {
        return KAPS_INTERNAL_ERROR;
    }

    db->db_info.common_info->cur_capacity--;

    if (cur_ad_db)
    {
        cur_ad_db->num_entries[core_id]--;
    }

    kaps_pfx_bundle_destroy(entry->pfx_bundle, &db->db_info.device->nlm_alloc);

    if (in_db->device->nv_ptr && in_db->device->issu_status == KAPS_ISSU_INIT)
        KAPS_STRY(in_db->fn_table->cr_delete_entry(in_db, in_e, 0));

    if (entry->ad_handle)
    {
        if (entry->hb_user_handle && in_db->common_info->hb_info.hb)
        {
            struct kaps_hb *hb = NULL;
            KAPS_WB_HANDLE_READ_LOC((in_db->common_info->hb_info.hb), (&hb), (uintptr_t) entry->hb_user_handle);
            if (hb == NULL)
            {
                return KAPS_INTERNAL_ERROR;
            }
            hb->entry = NULL;
        }
        KAPS_STRY(kaps_ad_db_deassociate_entry_and_ad(in_db->device, entry->ad_handle, in_e));
    }

    POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
    return KAPS_OK;
}

static kaps_status
kaps_lpm_entry_add_ad(
    struct kaps_db *in_db,
    struct kaps_entry *in_e,
    struct kaps_ad *ad)
{
    /*
     * Associated Data can't be added for the
     * entry if the entry is already active
     * If status is not PENDING, it indicates
     * that the entry is already active
     */

    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    struct kaps_lpm_entry *entry = (struct kaps_lpm_entry *) in_e;

    if (!db || !entry || !ad)
        return KAPS_INVALID_ARGUMENT;

    if (entry->pfx_bundle->m_status != PENDING && !db->db_info.device->issu_in_progress)
    {
        return KAPS_ENTRY_ALREADY_ACTIVE;
    }

    entry->ad_handle = ad;
    KAPS_STRY(kaps_ad_db_associate_entry_and_ad(in_db->device, ad, in_e));

    return KAPS_OK;
}

static kaps_status
kaps_lpm_entry_add_hb(
    struct kaps_db *in_db,
    struct kaps_entry *in_e,
    struct kaps_hb *hb)
{
    /*
     * Hit Bit can't be added for the
     * entry if the entry is already active
     * If status is not PENDING, it indicates
     * that the entry is already active
     */

    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    struct kaps_lpm_entry *entry = (struct kaps_lpm_entry *) in_e;
    struct kaps_device *device = NULL;
    uint32_t is_hb_supported = 0;

    if (!db || !entry || !hb)
        return KAPS_INVALID_ARGUMENT;

    device = db->db_info.device;
    if (!device)
        return KAPS_INVALID_ARGUMENT;

    if (device->type == KAPS_DEVICE_KAPS && device->num_hb_blocks > 0)
    {
        is_hb_supported = 1;
    }

    if (!is_hb_supported)
        return KAPS_UNSUPPORTED;

    if (entry->pfx_bundle->m_status != PENDING && !db->db_info.device->issu_in_progress)
    {
        return KAPS_ENTRY_ALREADY_ACTIVE;
    }

    if (!entry->ad_handle)
        return KAPS_NEED_AD_FOR_HB;

    /*
     * Entries needing hitbits need to have unique ad handle. This restriction can be relaxed later with internal
     * duplication of AD or API changes later 
     */
    if (entry->ad_handle->ref_count > 1)
        return KAPS_UNSUPPORTED;

    entry->hb_user_handle = hb->user_handle;

    hb->entry = in_e;

    return KAPS_OK;
}

static kaps_status
kaps_lpm_entry_print(
    struct kaps_db *in_db,
    struct kaps_entry *in_e,
    FILE * fp)
{
    uint32_t i;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    struct kaps_lpm_entry *entry = (struct kaps_lpm_entry *) in_e;
    int32_t num_bits, num_bytes;
    struct kaps_key_field *db_f = NULL;
    uint32_t fcount = 0;

    if (!db || !entry || !fp)
        return KAPS_INVALID_ARGUMENT;

    num_bytes = (entry->pfx_bundle->m_nPfxSize + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE;
    db_f = db->db_info.key->first_field;

    for (; db_f; db_f = db_f->next)
    {
        if (db_f->type == KAPS_KEY_FIELD_HOLE)
            continue;
        fcount++;
    }

    i = 0;
    num_bits = entry->pfx_bundle->m_nPfxSize;
    if (in_db->common_info->is_extra_byte_added)
    {
        i = 1;
        num_bits -= KAPS_BITS_IN_BYTE;
    }

    fcount = 1; /* TBD: need to check some more TID/EM/PFX combinations [same need to handle in acl.c]: keeping raw*/
    /* prefix print*/

    if (fcount == 1)
    {
        /* prefix has single field*/
        for (; i < num_bytes; ++i)
        {
            if (i == num_bytes - 1)
            {
                kaps_fprintf(fp, "%d", entry->pfx_bundle->m_data[i]);
            }
            else
            {
                kaps_fprintf(fp, "%d.", entry->pfx_bundle->m_data[i]);
            }
        }
        kaps_fprintf(fp, "/%d", num_bits);
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_install_internal(
    struct kaps_db *in_db)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    NlmErrNum_t err_num = NLMERR_OK;
    NlmReasonCode reason_code = NLMRSC_REASON_OK;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    kaps_status status = KAPS_OK;

    if (!db)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.parent)
        db = (struct kaps_lpm_db *) db->db_info.parent;

    for (; db; db = (struct kaps_lpm_db *) db->db_info.next_tab)
    {
        kaps_c_list_reset(&db->tmp_list);
        if (kaps_c_list_count(&db->pending) == 0)
            continue;

        /*
         * Iterate through pending list and add each entry in to device 
         */
        kaps_c_list_iter_init(&db->pending, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_lpm_entry *entry = DBLIST_TO_LPM_ENTRY(el);
            struct kaps_ad_db *cur_ad_db;

            /*
             * Ensure there is a valid ad handle if the LPM DB has AD DB associated with it
             */
            if (db->db_info.common_info->ad_info.ad && !entry->ad_handle)
            {
                status = KAPS_AD_NOT_YET_ADDED;
                break;
            }

            err_num = kaps_ftm_add_prefix(db, entry, &reason_code);

            if (err_num != NLMERR_OK)
            {
                entry->pfx_bundle->m_nIndex = KAPS_LSN_NEW_INDEX;
                status = kaps_map_reason_code_to_kaps_status(reason_code);
                break;
            }

            entry->pfx_bundle->m_status = COMMITTED;
            db->meta_priority_map |= (1 << entry->meta_priority);

            db->db_info.common_info->cur_capacity++;
            if (entry->ad_handle)
            {
                KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->db_info.device, entry->ad_handle, cur_ad_db);
                cur_ad_db->num_entries[entry->core]++;
            }

            /*
             * Remove node from pending and add it into tmp list 
             */
            kaps_c_list_remove_node(&db->pending, el, &it);
            kaps_c_list_add_tail(&db->tmp_list, el);
        }
        if (status != KAPS_OK)
        {
            struct kaps_lpm_db *tmp_db = (struct kaps_lpm_db *) in_db;

            if (tmp_db->db_info.parent)
                tmp_db = (struct kaps_lpm_db *) tmp_db->db_info.parent;

            for (; tmp_db; tmp_db = (struct kaps_lpm_db *) tmp_db->db_info.next_tab)
            {

                /*
                 * Iterate through tmp list and delete each from the device 
                 */

                kaps_c_list_iter_init(&tmp_db->tmp_list, &it);
                while ((el = kaps_c_list_iter_next(&it)) != NULL)
                {
                    struct kaps_lpm_entry *entry = DBLIST_TO_LPM_ENTRY(el);
                    struct kaps_ad_db *cur_ad_db;

                    if (in_db->device->nv_ptr && in_db->device->issu_status == KAPS_ISSU_INIT)
                        KAPS_STRY(in_db->fn_table->cr_delete_entry((struct kaps_db *) tmp_db, (void *) entry, 1));

                    err_num = kaps_ftm_delete_prefix(tmp_db->fib_tbl, entry, &reason_code);
                    if (err_num != NLMERR_OK)
                    {
                        if (reason_code == NLMRSC_LOW_MEMORY)
                        {
                            status = kaps_map_reason_code_to_kaps_status(reason_code);
                            return status;
                        }
                        kaps_sassert(0);
                    }

                    if (entry->ad_handle)
                    {
                        if (entry->hb_user_handle && in_db->common_info->hb_info.hb)
                        {
                            struct kaps_hb *hb = NULL;

                            KAPS_WB_HANDLE_READ_LOC((in_db->common_info->hb_info.hb), (&hb),
                                                    (uintptr_t) entry->hb_user_handle);
                            if (hb == NULL)
                            {
                                return KAPS_INTERNAL_ERROR;
                            }
                            hb->entry = NULL;
                        }
                    }

                    entry->pfx_bundle->m_nIndex = KAPS_LSN_NEW_INDEX;
                    entry->pfx_bundle->m_status = PENDING;

                    /*
                     * initialize the underneath module referable params : JIRA KBPSDK-1834 
                     */
                    entry->pfx_bundle->m_isLmpsofarPfx = 0;
                    entry->pfx_bundle->m_isPfxCopy = 0;
                    entry->core = 0;

                    tmp_db->db_info.common_info->cur_capacity--;
                    if (entry->ad_handle)
                    {
                        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->db_info.device, entry->ad_handle, cur_ad_db);
                        cur_ad_db->num_entries[entry->core]--;
                    }

                    kaps_c_list_remove_node(&tmp_db->tmp_list, el, &it);
                    kaps_c_list_add_head(&tmp_db->pending, el);

                    if (in_db->device->nv_ptr && in_db->device->issu_status == KAPS_ISSU_INIT)
                        KAPS_STRY(in_db->fn_table->cr_delete_entry((struct kaps_db *) tmp_db, (void *) entry, 0));
                }
            }
            return status;
        }
    }

    db = (struct kaps_lpm_db *) in_db;
    if (db->db_info.parent)
        db = (struct kaps_lpm_db *) db->db_info.parent;

    for (; db; db = (struct kaps_lpm_db *) db->db_info.next_tab)
    {
        /*
         * Iterate through tmp list and update to db_list 
         */
        kaps_c_list_iter_init(&db->tmp_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            kaps_c_list_remove_node(&db->tmp_list, el, &it);
            kaps_c_list_add_tail(&db->db_info.db_list, el);
        }
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_install(
    struct kaps_db *in_db)
{
    if (!in_db)
        return KAPS_INVALID_ARGUMENT;

    if (in_db->common_info->defer_deletes_to_install)
    {

        KAPS_STRY(kaps_lpm_db_install_internal(in_db));

        in_db->common_info->defer_deletes_to_install = 0;

        if (in_db->common_info->num_pending_del_entries)
        {
            uint32_t i = 0;

            for (i = 0; i < in_db->common_info->num_pending_del_entries; i++)
            {
                struct kaps_entry *e = in_db->common_info->del_entry_list[i];
                struct kaps_db *cur_tab = NULL;

                KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(in_db->device, e, cur_tab);
                KAPS_WB_HANDLE_DELETE_LOC(cur_tab, (uintptr_t) e->user_handle);
                KAPS_STRY(kaps_lpm_db_delete_entry(cur_tab, e));
            }
            in_db->common_info->num_pending_del_entries = 0;
        }

        in_db->common_info->defer_deletes_to_install = 1;

    }
    else
    {
        KAPS_STRY(kaps_lpm_db_install_internal(in_db));
    }
    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_search(
    struct kaps_db *in_db,
    uint8_t * key,
    struct kaps_entry **in_e,
    int32_t * index,
    int32_t * length)
{
    NlmErrNum_t err_num = NLMERR_OK;
    NlmReasonCode reason_code;
    uint8_t temp_key[KAPS_LPM_KEY_MAX_WIDTH_8];
    uint8_t *sw_key;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    struct kaps_lpm_entry **entry_pp = (struct kaps_lpm_entry **) in_e;

    *entry_pp = NULL;   /* if hit, assign the proper handle else NULL */

    if (db->db_info.is_clone)
        db = (struct kaps_lpm_db *) db->db_info.clone_of;

    sw_key = key;

    if (in_db->common_info->is_extra_byte_added)
    {
        /*
         * Take into account extra byte at MS portion 
         */
        temp_key[0] = 0;
        kaps_memcpy(&temp_key[1], key, db->db_info.width.key_width_1 / KAPS_BITS_IN_BYTE);
        sw_key = temp_key;
    }

    err_num = kaps_ftm_locate_lpm(db->fib_tbl, sw_key, entry_pp, (uint32_t *) index, (uint32_t *) length, &reason_code);

    if (err_num != NLMERR_OK)
        return kaps_map_reason_code_to_kaps_status(reason_code);

    /*
     * For KAPS, if the index callback is not registered, then we are not interested in the index. So return -1
     */
    if (db->db_info.device->type == KAPS_DEVICE_KAPS && !in_db->common_info->callback_fn)
    {
        *index = -1;
    }

    if (in_db->common_info->is_extra_byte_added)
    {
        if (*index != -1)
            *length -= KAPS_BITS_IN_BYTE;
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_stats(
    struct kaps_db *in_db,
    struct kaps_db_stats *stats)
{
    NlmReasonCode reason;
    NlmErrNum_t err_num = NLMERR_OK;
    kaps_fib_resource_usage rxc_usage = { 0, };   /**< fib resource usage */
    struct kaps_lpm_db *tmp;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    uint32_t cur_extra_capacity, min_extra_capacity;

    if (db == NULL || stats == NULL)
        return KAPS_INVALID_ARGUMENT;

    kaps_memset(stats, 0, sizeof(*stats));
    db->num_used_ab = 0;
    db->used_uda_kb = 0;
    db->num_used_ait = 0;

    /*
     * Initialize min_extra capacity to a very high value
     */
    min_extra_capacity = (1u << 30);
    tmp = db;
    {
        if (tmp->fib_tbl == NULL)
            return KAPS_OK;

        stats->num_entries = tmp->fib_tbl->m_numPrefixes;

        if (tmp->fib_tbl)
        {
            err_num = kaps_ftm_get_resource_usage(tmp->fib_tbl, &rxc_usage, &reason);
            if (err_num != NLMERR_OK)
                return kaps_map_reason_code_to_kaps_status(reason);
        }

        if (tmp->core1_fib_tbl)
        {
            err_num = kaps_ftm_get_resource_usage(tmp->core1_fib_tbl, &rxc_usage, &reason);
            if (err_num != NLMERR_OK)
                return kaps_map_reason_code_to_kaps_status(reason);
            stats->num_entries += tmp->core1_fib_tbl->m_numPrefixes;
        }
        cur_extra_capacity = 0;
        if (rxc_usage.m_estCapacity > tmp->fib_tbl->m_numPrefixes)
            cur_extra_capacity = rxc_usage.m_estCapacity - tmp->fib_tbl->m_numPrefixes;

        if (cur_extra_capacity < min_extra_capacity)
            min_extra_capacity = cur_extra_capacity;
    }

    db->num_used_ab = rxc_usage.m_numOfABsUsed;
    db->num_released_ab = rxc_usage.m_numABsReleased;

    db->num_ab_no_compression = rxc_usage.m_num_ab_no_compression;
    db->num_ab_compression_1 = rxc_usage.m_num_ab_compression_1;
    db->num_ab_compression_2 = rxc_usage.m_num_ab_compression_2;

    /*
     * Add the smallest extra capacity for a table across all tables in the database and the current number of entries
     * in all tables of the database to get the capacity estimate
     */
    stats->capacity_estimate = min_extra_capacity + stats->num_entries;

    return KAPS_OK;
}


kaps_status
kaps_lpm_wb_save_pfx_bundle(
    struct kaps_db * db,
    struct kaps_lpm_entry * entry,
    kaps_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset)
{
    uint32_t len_8 = 0;
    struct kaps_wb_prefix_info prefix_info;
    uint32_t tmp_offset = 0;

    len_8 = (entry->pfx_bundle->m_nPfxSize + 7) / 8;

    prefix_info.pfx_len = entry->pfx_bundle->m_nPfxSize;
    prefix_info.index = entry->pfx_bundle->m_nIndex;
    prefix_info.user_handle = entry->user_handle;
    prefix_info.meta_priority = entry->meta_priority;

    tmp_offset = *nv_offset;
    *nv_offset += sizeof(prefix_info);

    if (0 != write_fn(handle, entry->pfx_bundle->m_data, len_8, *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += len_8;

    if (entry->ad_handle)
    {
        struct kaps_ad_db *ad_db;
        uint32_t ad_user_hdl = entry->ad_handle->user_handle;

        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->device, entry->ad_handle, ad_db);
        prefix_info.ad_seq_num = KAPS_GET_AD_SEQ_NUM(entry->ad_handle);
        if (0 != write_fn(handle, (uint8_t *) & (ad_user_hdl), sizeof(uint32_t), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(ad_user_hdl);

        if (0 !=
            write_fn(handle, (uint8_t *) & (entry->ad_handle->value[ad_db->byte_offset]),
                     sizeof(uint8_t) * ad_db->user_bytes, *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += (sizeof(uint8_t) * ad_db->user_bytes);
    }

    if (db->common_info->hb_info.hb)
    {
        uintptr_t hb_user_handle = 0;
        if (entry->hb_user_handle)
            hb_user_handle = entry->hb_user_handle;

        if (0 != write_fn(handle, (uint8_t *) & (hb_user_handle), sizeof(hb_user_handle), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(hb_user_handle);
    }

    if (0 != write_fn(handle, (uint8_t *) & prefix_info, sizeof(prefix_info), tmp_offset))
        return KAPS_NV_READ_WRITE_FAILED;

    return KAPS_OK;
}

kaps_status
kaps_lpm_wb_save_state(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_lpm_mgr *lpm_mgr;
    kaps_fib_tbl_mgr *tbl_mgr;
    kaps_trie_global *trie_g;
    struct kaps_device *tmp_device = device;

    lpm_mgr = device->lpm_mgr;
    tbl_mgr = lpm_mgr->fib_tbl_mgr;
    trie_g = tbl_mgr->m_trieGlobal;

    trie_g->wb_fun = wb_fun;

    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmFalse;
        if (tmp_device->smt)
            tmp_device->smt->issu_in_progress = NlmFalse;
        tmp_device = tmp_device->next_dev;
    }

    /*
     * Device writes are not allowed when ISSU is in-progress 
     */
    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmTrue;
        if (tmp_device->smt)
            tmp_device->smt->issu_in_progress = NlmTrue;
        tmp_device = tmp_device->next_dev;
    }

    if (device->type == KAPS_DEVICE_KAPS)
        KAPS_STRY(kaps_device_save_kaps_shadow(device, wb_fun));

    return KAPS_OK;
}

kaps_status
kaps_lpm_cr_save_state(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_lpm_mgr *lpm_mgr;
    kaps_fib_tbl_mgr *tbl_mgr;
    kaps_trie_global *trie_g;

    lpm_mgr = device->lpm_mgr;
    tbl_mgr = lpm_mgr->fib_tbl_mgr;
    trie_g = tbl_mgr->m_trieGlobal;

    trie_g->wb_fun = wb_fun;

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_wb_save_state_internal(
    struct kaps_db *db,
    int32_t core_id,
    struct kaps_wb_cb_functions *cb_fun)
{
    struct kaps_lpm_mgr *lpm_mgr;
    kaps_fib_tbl_mgr *tbl_mgr;
    kaps_trie_global *trie_g;
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;
    struct kaps_db *tab;
    struct kaps_lpm_db *lpm_db;
    struct kaps_fib_tbl *fib_tbl;
    uint32_t count = 0;

    if (db->is_bc_required && !db->is_main_bc_db)
        return KAPS_OK;

    lpm_db = (struct kaps_lpm_db *) db;

    fib_tbl = lpm_db->fib_tbl;

    {
        lpm_mgr = db->lpm_mgr;
        tbl_mgr = lpm_mgr->fib_tbl_mgr;
        trie_g = tbl_mgr->m_trieGlobal;

        trie_g->wb_fun = cb_fun;

        if (NLMERR_OK != kaps_kaps_trie_wb_save_pools(tbl_mgr->m_trieGlobal, cb_fun))
            return KAPS_NV_READ_WRITE_FAILED;
    }

    for (tab = db; tab; tab = tab->next_tab)
    {
        if (!tab->is_clone)
        {
            lpm_db = (struct kaps_lpm_db *) tab;

            fib_tbl = lpm_db->fib_tbl;

            /*
             * Store the algorithm info 
             */
            KAPS_STRY(kaps_ftm_wb_save(fib_tbl, cb_fun->nv_offset));

            if (db->common_info->is_cascaded && core_id == 1)
                continue;

            /*
             * Store the pending entries for this DB 
             */
            kaps_c_list_iter_init(&lpm_db->pending, &it);
            count = kaps_c_list_count(&lpm_db->pending);

            if (0 != cb_fun->write_fn(cb_fun->handle, (uint8_t *) & count, sizeof(count), *cb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
            *cb_fun->nv_offset += sizeof(count);

            while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_lpm_entry *entry = DBLIST_TO_LPM_ENTRY(e1);

                KAPS_STRY(kaps_lpm_wb_save_pfx_bundle(db, entry, cb_fun->write_fn, cb_fun->handle, cb_fun->nv_offset));
            }
        }
    }

    if (core_id == 1 && db->common_info->is_cascaded)
        return KAPS_OK;


    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_wb_save_state(
    struct kaps_db *db,
    struct kaps_wb_cb_functions *cb_fun)
{
    KAPS_STRY(kaps_lpm_db_wb_save_state_internal(db, 0, cb_fun));

    return KAPS_OK;
}

static kaps_status
kaps_db_wb_add_prefix(
    struct kaps_db *db,
    uint8_t * prefix,
    uint32_t length,
    uint32_t index,
    uintptr_t user_handle,
    uint32_t meta_priority)
{
    struct kaps_lpm_entry *entry_p = NULL;
    struct kaps_entry *entry;

    if (db == NULL || prefix == NULL || user_handle == 0)
        return KAPS_INVALID_ARGUMENT;

    if (db->is_clone)
        return KAPS_ADD_DEL_ENTRY_TO_CLONE_DB;

    if (db->type != KAPS_DB_LPM && !db->common_info->mapped_to_acl)
        return KAPS_INVALID_ARGUMENT;

    if (!db->device->is_config_locked)
        return KAPS_DEVICE_UNLOCKED;

    if (db->key == NULL)
        return KAPS_MISSING_KEY;

    if (db->common_info->mapped_to_acl)
        return KAPS_UNSUPPORTED;

    KAPS_STRY(kaps_lpm_db_wb_add_entry((struct kaps_lpm_db *) db, prefix, length, index,
                                       (struct kaps_lpm_entry **) &entry_p, meta_priority));
    if (entry_p == NULL)
    {
        return KAPS_INTERNAL_ERROR;
    }
    entry_p->meta_priority = meta_priority;
    entry = (struct kaps_entry *) entry_p;
    if (db->common_info->is_cascaded)
        entry->core = db->device->core_id;

    KAPS_WB_HANDLE_WRITE_LOC(db, entry, user_handle);

    return KAPS_OK;
}

static kaps_status
kaps_db_wb_replay_prefixes(
    struct kaps_db *db,
    int32_t core_id)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;
    struct kaps_wb_prefix_data *pfx;
    struct kaps_device *device;
    struct kaps_ad_db *ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;
    struct kaps_ad *ad_p = NULL;
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) db;
    struct kaps_fib_tbl *fib_tbl;

    device = db->device;

    fib_tbl = lpm_db->fib_tbl;

    kaps_c_list_iter_init(&fib_tbl->wb_prefixes, &it);

    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        pfx = WBPFXLIST_TO_ENTRY(e1);
        ad_p = NULL;
        KAPS_STRY(kaps_db_wb_add_prefix(db, pfx->data, pfx->info.pfx_len, pfx->info.index,
                                        pfx->info.user_handle, pfx->info.meta_priority));

        if (ad_db)
        {
            uint32_t ad_db_seq_num = pfx->info.ad_seq_num;

            kaps_sassert(ad_db_seq_num);
            kaps_sassert(ad_db_seq_num <= device->seq_num_to_ptr->num_ad_db);
            kaps_sassert(device->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num]);
            ad_db = device->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num];

            if (!(pfx->ad_user_handle > *ad_db->db_info.common_info->user_hdl_table_size))
                KAPS_WB_HANDLE_READ_LOC((&ad_db->db_info), (&ad_p), (uintptr_t) pfx->ad_user_handle);

            if (!ad_p)
            {
                if (kaps_resource_get_ad_type(&ad_db->db_info) == KAPS_AD_TYPE_INDIRECTION)
                {
                    
                }

                KAPS_STRY(kaps_ad_db_add_entry_internal(ad_db, pfx->ad, &ad_p, pfx->ad_user_handle));
            }
            KAPS_TRY(kaps_entry_add_ad(db, (struct kaps_entry *) pfx->info.user_handle,
                                       (struct kaps_ad *) ((uintptr_t) pfx->ad_user_handle)));
            device->alloc->xfree(device->alloc->cookie, pfx->ad);
            if (db->common_info->hb_info.hb)
            {
                struct kaps_hb_db *hb_db = (struct kaps_hb_db *) db->common_info->hb_info.hb;
                uintptr_t hb_usr_handle = pfx->hb_user_handle;

                if (hb_usr_handle)
                {
                    KAPS_STRY(kaps_hb_db_wb_add_entry(hb_db, pfx->hb_no, pfx->hb_user_handle));
                    KAPS_STRY(kaps_entry_add_hb
                              (db, (struct kaps_entry *) pfx->info.user_handle, (struct kaps_hb *) hb_usr_handle));
                }
            }
        }

        kaps_c_list_remove_node(&fib_tbl->wb_prefixes, e1, &it);
        device->alloc->xfree(device->alloc->cookie, pfx->data);
        device->alloc->xfree(device->alloc->cookie, pfx);
    }

    if (core_id == 1 && db->common_info->is_cascaded)
        return KAPS_OK;

    if (fib_tbl->ad_handle_zero_size_wb)
    {
        struct kaps_ad *ad_p;
        ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;
        while (ad_db)
        {
            if (!ad_db->user_width_1 && !ad_db->num_0b_entries)
                break;
            ad_db = ad_db->next;
        }
        kaps_sassert(ad_db);
        KAPS_STRY(kaps_ad_db_add_entry_internal(ad_db, NULL, &ad_p, fib_tbl->ad_handle_zero_size_wb));
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_wb_restore_prefix(
    struct kaps_device * device,
    struct kaps_wb_prefix_data * prefix,
    kaps_device_issu_read_fn read_fn,
    void *handle,
    uint32_t * nv_offset,
    struct kaps_db * db)
{
    uint32_t len_8 = 0;
    uint32_t ad_bytes = 0;
    struct kaps_ad_db *ad_db;
    uint32_t num_bytes_to_allocate;

    if (0 != read_fn(handle, (uint8_t *) & prefix->info, sizeof(prefix->info), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(prefix->info);

    len_8 = (prefix->info.pfx_len + 7) / 8;

    num_bytes_to_allocate = len_8;
    if (num_bytes_to_allocate == 0)
        num_bytes_to_allocate = 1;

    prefix->data = device->alloc->xcalloc(device->alloc->cookie, 1, num_bytes_to_allocate);
    if (!prefix->data)
        return KAPS_OUT_OF_MEMORY;

    if (0 != read_fn(handle, prefix->data, len_8, *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += len_8;

    if (db->common_info->ad_info.ad)
    {
        uint32_t ad_db_seq_num = prefix->info.ad_seq_num;

        kaps_sassert(ad_db_seq_num);
        kaps_sassert(ad_db_seq_num <= device->seq_num_to_ptr->num_ad_db);
        kaps_sassert(device->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num]);
        ad_db = device->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num];
        ad_bytes = ad_db->user_bytes;
    }

    if (db->common_info->ad_info.ad)
    {

        if (0 != read_fn(handle, (uint8_t *) & (prefix->ad_user_handle), sizeof(prefix->ad_user_handle), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(prefix->ad_user_handle);

    }

    if (ad_bytes)
    {

        prefix->ad = device->alloc->xcalloc(device->alloc->cookie, 1, ad_bytes);
        if (!prefix->ad)
            return KAPS_OUT_OF_MEMORY;

        if (0 != read_fn(handle, (uint8_t *) prefix->ad, sizeof(uint8_t) * ad_bytes, *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += (sizeof(uint8_t) * ad_bytes);
    }

    if (db->common_info->hb_info.hb)
    {
        uintptr_t hb_user_handle = 0;

        if (0 != read_fn(handle, (uint8_t *) & (hb_user_handle), sizeof(hb_user_handle), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(hb_user_handle);

        prefix->hb_user_handle = hb_user_handle;
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_wb_restore_state(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fn)
{
    struct kaps_lpm_mgr *lpm_mgr;
    kaps_fib_tbl_mgr *tbl_mgr;
    kaps_trie_global *trie_g;
    struct kaps_device *tmp_device = device;

    lpm_mgr = device->lpm_mgr;
    tbl_mgr = lpm_mgr->fib_tbl_mgr;
    trie_g = tbl_mgr->m_trieGlobal;

    /*
     * Device writes are not allowed when ISSU is in-progress 
     */
    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmTrue;
        tmp_device = tmp_device->next_dev;
    }

    trie_g->wb_fun = wb_fn;

    if (device->type == KAPS_DEVICE_KAPS)
        KAPS_STRY(kaps_device_restore_kaps_shadow(device, wb_fn));

    return KAPS_OK;
}

kaps_status
kaps_lpm_cr_restore_state(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fn)
{
    struct kaps_lpm_mgr *lpm_mgr;
    kaps_fib_tbl_mgr *tbl_mgr;
    kaps_trie_global *trie_g;
    struct kaps_device *tmp_device = device;

    lpm_mgr = device->lpm_mgr;
    tbl_mgr = lpm_mgr->fib_tbl_mgr;
    trie_g = tbl_mgr->m_trieGlobal;

    /*
     * Device writes are not allowed when ISSU is in-progress 
     */
    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmTrue;
        tmp_device = tmp_device->next_dev;
    }

    trie_g->wb_fun = wb_fn;

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_wb_restore_state_internal(
    struct kaps_db *db,
    int32_t core_id,
    struct kaps_wb_cb_functions *cb_fun)
{
    struct kaps_lpm_mgr *lpm_mgr;
    kaps_fib_tbl_mgr *tbl_mgr;
    kaps_trie_global *trie_g;
    struct kaps_db *tab;
    struct kaps_lpm_db *lpm_db;
    struct kaps_device *device = db->device;
    uint32_t pending_count;
    struct kaps_wb_prefix_data *prefix;
    struct kaps_fib_tbl *fib_tbl;

    if (db->is_bc_required && !db->is_main_bc_db)
        return KAPS_OK;

    lpm_mgr = db->lpm_mgr;
    tbl_mgr = lpm_mgr->fib_tbl_mgr;
    trie_g = tbl_mgr->m_trieGlobal;

    lpm_db = (struct kaps_lpm_db *) db;

    fib_tbl = lpm_db->fib_tbl;

    {
        lpm_mgr = db->lpm_mgr;
        tbl_mgr = lpm_mgr->fib_tbl_mgr;
        tbl_mgr->m_curFibTbl = fib_tbl;
        trie_g = tbl_mgr->m_trieGlobal;

        trie_g->wb_fun = cb_fun;

        if (NLMERR_OK != kaps_kaps_trie_wb_restore_pools(tbl_mgr->m_trieGlobal, cb_fun))
            return KAPS_NV_READ_WRITE_FAILED;
    }

    for (tab = db; tab; tab = tab->next_tab)
    {
        if (!tab->is_clone)
        {
            lpm_db = (struct kaps_lpm_db *) tab;

            fib_tbl = lpm_db->fib_tbl;

            kaps_c_list_reset(&fib_tbl->wb_prefixes);
            kaps_c_list_reset(&fib_tbl->wb_apt_prefixes);
            kaps_c_list_reset(&fib_tbl->wb_apt_pfx_copies);

            /*
             * restore algo info for the database 
             */
            KAPS_STRY(kaps_ftm_wb_restore(fib_tbl, cb_fun->nv_offset));

            if (db->common_info->is_cascaded && core_id == 1)
                continue;

            if (0 !=
                cb_fun->read_fn(cb_fun->handle, (uint8_t *) & pending_count, sizeof(pending_count), *cb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
            *cb_fun->nv_offset += sizeof(pending_count);

            while (pending_count--)
            {
                prefix = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_wb_prefix_data));
                if (!prefix)
                    return KAPS_OUT_OF_MEMORY;
                KAPS_STRY(kaps_lpm_wb_restore_prefix
                          (device, prefix, cb_fun->read_fn, cb_fun->handle, cb_fun->nv_offset, db));

                kaps_c_list_add_tail(&fib_tbl->wb_prefixes, &prefix->kaps_list_node);
            }
        }
    }

    if (device->type == KAPS_DEVICE_KAPS)
    {
        KAPS_STRY(kaps_uda_mgr_wb_finalize(device->alloc, trie_g->m_mlpmemmgr[0]));
    }

    if (core_id == 1 && db->common_info->is_cascaded)
        return KAPS_OK;

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_wb_restore_state(
    struct kaps_db *db,
    struct kaps_wb_cb_functions *cb_fun)
{
    KAPS_STRY(kaps_lpm_db_wb_restore_state_internal(db, 0, cb_fun));

    return KAPS_OK;
}

kaps_status
kaps_lpm_cr_remap_prefix(
    kaps_fib_tbl * fib_tbl,
    kaps_pfx_bundle * newPfxBundle,
    uint32_t * is_pending)
{
    struct kaps_lpm_entry *entry;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) fib_tbl->m_db;
    struct kaps_ad_db *ad_db;
    struct kaps_db *main_db;
    uint8_t *nv_ptr = NULL;
    uint32_t user_handle;
    uint32_t offset;
    struct kaps_lpm_cr_entry_info *ins_entry;

    *is_pending = 0;
    user_handle = fib_tbl->cr_user_handles[newPfxBundle->m_nIndex];
    if (!user_handle)
    {
        *is_pending = 1;
        return KAPS_OK;
    }

    POOL_ALLOC(kaps_lpm_entry, &db->lpm_entry_pool, entry);
    if (!entry)
        return KAPS_OUT_OF_MEMORY;

    kaps_memset(entry, 0, sizeof(*entry));
    entry->pfx_bundle = newPfxBundle;
    entry->pfx_bundle->m_status = COMMITTED;
    entry->pfx_bundle->m_backPtr = entry;
    entry->db_seq_num = db->db_info.seq_num;
    kaps_c_list_add_tail(&db->db_info.db_list, &entry->db_node);

    KAPS_WB_HANDLE_WRITE_LOC(fib_tbl->m_db, ((struct kaps_entry *) entry), user_handle);

    ad_db = (struct kaps_ad_db *) fib_tbl->m_db->common_info->ad_info.ad;
    if (ad_db)
    {
        main_db = fib_tbl->m_db;
        if (main_db->parent)
            main_db = main_db->parent;
        nv_ptr = main_db->device->nv_ptr;

        offset = kaps_db_entry_get_nv_offset(&db->db_info, user_handle);
        nv_ptr += offset;

        ins_entry = (struct kaps_lpm_cr_entry_info *) nv_ptr;

        KAPS_TRY(kaps_entry_add_ad(&db->db_info, KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, user_handle),
                                   (struct kaps_ad *) ((uintptr_t) ins_entry->ad_user_handle)));
        if (fib_tbl->m_db->common_info->hb_info.hb)
        {
            struct kaps_hb_db *hb_db = (struct kaps_hb_db *) fib_tbl->m_db->common_info->hb_info.hb;
            uintptr_t hb_usr_handle = ins_entry->hb_user_handle;

            if (hb_usr_handle)
            {
                KAPS_STRY(kaps_hb_db_wb_add_entry(hb_db, ins_entry->hb_bit_no, ins_entry->hb_user_handle));
                KAPS_STRY(kaps_entry_add_hb
                          (&db->db_info, KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, user_handle),
                           (struct kaps_hb *) hb_usr_handle));
            }
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_db_cr_create_index_handle_map(
    struct kaps_db * db)
{
    struct kaps_db *main_db = db, *tab;
    struct kaps_ad_db *ad_db;
    struct kaps_lpm_db *lpm_db;
    struct kaps_cr_pool_mgr *cr_mgr;
    struct kaps_cr_pool_entry_iter *iter;
    uint8_t *nv_ptr = NULL;
    uint32_t offset, num_entries = 0, pending_count, i;

    if (db->parent)
        main_db = db->parent;

    nv_ptr = main_db->device->nv_ptr;

    offset = main_db->device->nv_mem_mgr->offset_device_pending_list;
    nv_ptr += offset;
    pending_count = *(uint32_t *) nv_ptr;
    nv_ptr += sizeof(uint32_t);

    /*
     * Delete Pending entries here 
     */
    for (i = 0; i < pending_count; i++)
    {
        struct kaps_lpm_cr_entry_info *pending_entry = NULL;
        struct kaps_db *cur_db;
        uint8_t *pending_nv_ptr = main_db->device->nv_ptr;
        uint32_t is_del;
        uint32_t entry_offset;

        entry_offset = *(uint32_t *) nv_ptr;
        nv_ptr += sizeof(uint32_t);

        pending_nv_ptr += entry_offset;
        pending_entry = (struct kaps_lpm_cr_entry_info *) pending_nv_ptr;

        cur_db = main_db->device->seq_num_to_ptr->db_ptr[pending_entry->seq_num];
        if (cur_db->parent)
            cur_db = cur_db->parent;

        /*
         * This entry not belongs to this data base, so skip it
         */
        if (cur_db != main_db)
        {
            if ((main_db->device->seq_num_to_ptr->db_ptr[pending_entry->seq_num]->type == KAPS_DB_LPM)
                && pending_entry->marked_for_delete)
            {
                i += 6;
                nv_ptr += (6 * sizeof(uint32_t));
            }
            continue;
        }

        is_del = 0;

        /*
         * this is deleted entry store entry handle here
         */
        if (pending_entry->marked_for_delete)
        {
            is_del = 1;
            nv_ptr += (sizeof(uint32_t) * 6);
            i += 6;
        }

        if (is_del)
        {
            /*
             * mark deleted 
             */
            if (pending_entry->status == 2)
            {
                pending_entry->status = 0;
            }
        }
        else
        {
            pending_entry->status = 0;
        }
    }

    KAPS_CR_GET_POOL_MGR(main_db->device, cr_mgr);
    KAPS_STRY(kaps_cr_pool_mgr_iter_init(cr_mgr, main_db, &iter));

    while (1)
    {
        struct kaps_lpm_cr_entry_info *ins_entry;

        KAPS_STRY(kaps_cr_pool_mgr_iter_next(cr_mgr, main_db, iter, &nv_ptr));

        if (!nv_ptr)
            break;

        num_entries++;
        ins_entry = (struct kaps_lpm_cr_entry_info *) nv_ptr;

        /*
         * in-active entry skip here 
         */
        if (!ins_entry->status)
        {
            continue;
        }

        /*
         * crash happened at the time of delete, skip the entry here handle this entry at the time of reconcile 
         */
        if (ins_entry->status == 2)
        {
            continue;
        }

        nv_ptr += sizeof(*ins_entry);

        tab = main_db->device->seq_num_to_ptr->db_ptr[ins_entry->seq_num];
        if (tab->parent)
            kaps_sassert(tab->parent == main_db);

        kaps_sassert(tab != NULL);

        lpm_db = (struct kaps_lpm_db *) tab;
        kaps_sassert(lpm_db->fib_tbl->cr_user_handles[ins_entry->new_index] == 0);
        lpm_db->fib_tbl->cr_user_handles[ins_entry->new_index] = ins_entry->entry_user_handle;
        ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;
        if (ad_db)
        {
            uint8_t ad_data[256];
            struct kaps_ad *ad_p;

            kaps_memcpy(&ad_data, nv_ptr, ad_db->user_bytes);
            KAPS_STRY(kaps_ad_db_add_entry_internal(ad_db, ad_data, &ad_p, ins_entry->ad_user_handle));
            nv_ptr += (sizeof(uint8_t) * ad_db->user_bytes);
        }
    }

    KAPS_STRY(kaps_cr_pool_mgr_iter_destroy(cr_mgr, main_db, iter));

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_cr_save_state(
    struct kaps_db *db,
    struct kaps_wb_cb_functions *cb_fun)
{
    kaps_fib_tbl_mgr *tbl_mgr = NULL;
    struct kaps_lpm_db *lpm_db = NULL;
    struct kaps_device *device = db->device;
    struct kaps_wb_cb_functions tmp_cb_fun;
    uint32_t offset;
    uint8_t *defer_deletes;

    if (db->parent)
        db = db->parent;

    tbl_mgr = device->lpm_mgr->fib_tbl_mgr;

    lpm_db = (struct kaps_lpm_db *) db;
    kaps_sassert(tbl_mgr);

    tmp_cb_fun.handle = device->nv_handle;
    tmp_cb_fun.nv_offset = &offset;
    offset = device->nv_mem_mgr->offset_device_lpm_info;
    tmp_cb_fun.nv_ptr = (uint8_t *) device->nv_ptr + offset;
    tmp_cb_fun.read_fn = NULL;
    tmp_cb_fun.write_fn = NULL;

    KAPS_STRY(kaps_lpm_cr_save_state(device, &tmp_cb_fun));

    defer_deletes = cb_fun->nv_ptr;
    *defer_deletes = lpm_db->db_info.common_info->defer_deletes_to_install;
    cb_fun->nv_ptr += sizeof(*defer_deletes);
    *cb_fun->nv_offset += sizeof(*defer_deletes);

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_cr_restore_state(
    struct kaps_db *db,
    struct kaps_wb_cb_functions *cb_fun)
{
    struct kaps_lpm_mgr *lpm_mgr;
    kaps_fib_tbl_mgr *tbl_mgr;
    kaps_trie_global *trie_g;
    struct kaps_db *tab, *parent_db = db;
    struct kaps_lpm_db *lpm_db;
    struct kaps_device *device = db->device;
    uint8_t *defer_deletes;

    if (db->parent)
        parent_db = db->parent;

    /*
     * Create IX to User Handle Mapping 
     */
    for (tab = db; tab; tab = tab->next_tab)
    {
        if (!tab->is_clone)
        {
            lpm_db = (struct kaps_lpm_db *) tab;
            lpm_db->fib_tbl->cr_user_handles =
                device->alloc->xcalloc(device->alloc->cookie, KAPS_FIB_MAX_INDEX_RANGE, sizeof(uint32_t));
            if (!lpm_db->fib_tbl->cr_user_handles)
                return KAPS_OUT_OF_MEMORY;
        }
    }

    lpm_db = (struct kaps_lpm_db *) db;

    lpm_mgr = device->lpm_mgr;
    tbl_mgr = lpm_mgr->fib_tbl_mgr;
    trie_g = tbl_mgr->m_trieGlobal;

    {

        lpm_mgr = db->lpm_mgr;
        tbl_mgr = lpm_mgr->fib_tbl_mgr;
        trie_g = tbl_mgr->m_trieGlobal;

        if (NLMERR_OK != kaps_kaps_trie_cr_restore_pools(lpm_db->fib_tbl->m_trie, cb_fun))
            return KAPS_NV_READ_WRITE_FAILED;
    }

    KAPS_STRY(kaps_lpm_db_cr_create_index_handle_map(db));

    /*
     * restore algo info for the database 
     */
    KAPS_STRY(kaps_ftm_cr_restore(lpm_db->fib_tbl, cb_fun));

    for (tab = db->next_tab; tab; tab = tab->next_tab)
    {
        if (!tab->is_clone)
        {
            lpm_db = (struct kaps_lpm_db *) tab;

            if (NLMERR_OK != kaps_kaps_trie_cr_restore_pools(lpm_db->fib_tbl->m_trie, cb_fun))
                return KAPS_NV_READ_WRITE_FAILED;

            KAPS_STRY(kaps_ftm_cr_restore(lpm_db->fib_tbl, cb_fun));

        }
    }

    defer_deletes = cb_fun->nv_ptr;
    parent_db->common_info->defer_deletes_to_install = *defer_deletes;
    if (parent_db->common_info->defer_deletes_to_install && !parent_db->common_info->del_entry_list)
    {
        struct kaps_allocator *alloc = parent_db->device->alloc;

        parent_db->common_info->del_entry_list =
            alloc->xcalloc(alloc->cookie, parent_db->common_info->max_pending_del_entries, sizeof(struct kaps_entry *));
        if (!parent_db->common_info->del_entry_list)
            return KAPS_OUT_OF_MEMORY;
        parent_db->common_info->num_pending_del_entries = 0;
    }
    cb_fun->nv_ptr += sizeof(*defer_deletes);
    *cb_fun->nv_offset += sizeof(*defer_deletes);

    KAPS_STRY(kaps_ix_mgr_wb_finalize_for_all_ad_dbs(device, (struct kaps_ad_db*) db->common_info->ad_info.ad));

    KAPS_STRY(kaps_uda_mgr_wb_finalize(device->alloc, trie_g->m_mlpmemmgr[0]));

    /*
     * Free IX to User Handle Mapping 
     */
    for (tab = db; tab; tab = tab->next_tab)
    {
        if (!tab->is_clone)
        {
            lpm_db = (struct kaps_lpm_db *) tab;
            device->alloc->xfree(device->alloc->cookie, lpm_db->fib_tbl->cr_user_handles);
        }
    }
    return KAPS_OK;
}

kaps_status
kaps_lpm_wb_pre_processing(
    struct kaps_device * device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;
    struct kaps_db *db, *tab;
    struct kaps_lpm_db *lpm_db;

    /*
     * Process any pending RPT Splits or move before saving the state 
     */

    kaps_c_list_iter_init(&device->db_list, &it);

    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        db = KAPS_SSDBLIST_TO_ENTRY(e1);

        if (db->type != KAPS_DB_LPM)
            continue;
        if (db->is_bc_required && !db->is_main_bc_db)
            continue;

        lpm_db = (struct kaps_lpm_db *) db;
        KAPS_STRY(kaps_ftm_wb_process_rpt(lpm_db->fib_tbl));

        for (tab = db->next_tab; tab; tab = tab->next_tab)
        {
            if (!tab->is_clone)
            {
                lpm_db = (struct kaps_lpm_db *) tab;

                KAPS_STRY(kaps_ftm_wb_process_rpt(lpm_db->fib_tbl));
            }
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_wb_post_processing(
    struct kaps_device * device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;

    if (!device->lpm_mgr)
        return KAPS_OK;

    /*
     * Uda Mgr warmboot finalize is required only once as we create only one
     * instance for the entire device.
     */

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(e1);

        if (db->type != KAPS_DB_LPM)
            continue;
        if (db->is_bc_required && !db->is_main_bc_db)
            continue;

        KAPS_STRY(kaps_uda_mgr_wb_finalize(device->alloc, db->lpm_mgr->fib_tbl_mgr->m_trieGlobal->m_mlpmemmgr[0]));

        KAPS_STRY(kaps_ix_mgr_wb_finalize_for_all_ad_dbs(device, (struct kaps_ad_db*) db->common_info->ad_info.ad));

    }

    /*
     * Restore individual database information
     */
    {
        struct kaps_c_list_iter it;
        struct kaps_list_node *e1;
        struct kaps_db *tab;

        kaps_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(e1);

            if (db->type != KAPS_DB_LPM)
                continue;
            if (db->is_bc_required && !db->is_main_bc_db)
                continue;

            for (tab = db; tab; tab = tab->next_tab)
            {
                if (!tab->is_clone)
                {

                    KAPS_STRY(kaps_db_wb_replay_prefixes(tab, 0));
                }
            }
        }
    }

    return KAPS_OK;
}


kaps_status
kaps_lpm_wb_traverse_and_print(
    struct kaps_lpm_db * db,
    FILE * fp)
{
    kaps_trie_wb_traverse_and_print_trie(db->fib_tbl->m_trie, fp);

    return KAPS_OK;
}

kaps_status
kaps_lpm_wb_traverse_and_verify(
    struct kaps_lpm_db * db,
    FILE * fp)
{
    kaps_kaps_trie_wb_verify_trie(db->fib_tbl->m_trie, fp);

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_delete_all_entries(
    struct kaps_db *in_db)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    NlmErrNum_t err_num;
    NlmReasonCode reason_code;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;

    if (!db->db_info.device->lpm_mgr)
        return KAPS_OK;

    /*
     * Iterate through pending list delete entry 
     */
    kaps_c_list_iter_init(&db->pending, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_lpm_entry *entry = DBLIST_TO_LPM_ENTRY(el);

        err_num = kaps_ftm_update_hash_table(db->fib_tbl, entry, 1);
        if (err_num != NLMERR_OK)
        {
            return KAPS_INTERNAL_ERROR;
        }

        kaps_pfx_bundle_destroy(entry->pfx_bundle, &db->db_info.device->nlm_alloc);

        kaps_c_list_remove_node(&db->pending, el, &it);
        if (entry->ad_handle)
        {
            if (entry->hb_user_handle && in_db->common_info->hb_info.hb)
            {
                struct kaps_hb *hb = NULL;

                KAPS_WB_HANDLE_READ_LOC((in_db->common_info->hb_info.hb), (&hb), (uintptr_t) entry->hb_user_handle);
                if (hb == NULL)
                {
                    return KAPS_INTERNAL_ERROR;
                }
                hb->entry = NULL;
            }
            KAPS_STRY(kaps_ad_db_deassociate_entry_and_ad
                      (in_db->device, entry->ad_handle, (struct kaps_entry *) entry));
        }
        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
    }

    /*
     * Iterate through record list and delete entry from device 
     */
    kaps_c_list_iter_init(&db->db_info.db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_lpm_entry *entry = DBLIST_TO_LPM_ENTRY(el);

        err_num = kaps_ftm_delete_prefix(db->fib_tbl, entry, &reason_code);
        if (err_num != NLMERR_OK)
            return kaps_map_reason_code_to_kaps_status(reason_code);

        err_num = kaps_ftm_update_hash_table(db->fib_tbl, entry, 1);
        if (err_num != NLMERR_OK)
        {
            return KAPS_INTERNAL_ERROR;
        }

        kaps_c_list_remove_node(&db->db_info.db_list, el, &it);
        kaps_pfx_bundle_destroy(entry->pfx_bundle, &db->db_info.device->nlm_alloc);

        if (entry->ad_handle)
        {
            if (entry->hb_user_handle && in_db->common_info->hb_info.hb)
            {
                struct kaps_hb *hb = NULL;

                KAPS_WB_HANDLE_READ_LOC((in_db->common_info->hb_info.hb), (&hb), (uintptr_t) entry->hb_user_handle);
                if (hb == NULL)
                {
                    return KAPS_INTERNAL_ERROR;
                }
                hb->entry = NULL;
            }
            KAPS_STRY(kaps_ad_db_deassociate_entry_and_ad
                      (in_db->device, entry->ad_handle, (struct kaps_entry *) entry));
        }

        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
    }
    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_reconcile_end_db(
    struct kaps_db *in_db)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    NlmErrNum_t err_num;
    NlmReasonCode reason_code;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    int32_t i;

    if (!db->db_info.device->lpm_mgr)
        return KAPS_OK;

    in_db->device->db_bc_bitmap = in_db->bc_bitmap;

    db->db_info.device->lpm_mgr->curr_db = (struct kaps_db *) db;

    if (in_db->common_info->defer_deletes_to_install)
    {
        for (i = 0; i < in_db->common_info->num_pending_del_entries; i++)
        {
            struct kaps_lpm_entry *entry = (struct kaps_lpm_entry *) in_db->common_info->del_entry_list[i];

            if (entry->pfx_bundle->reconcile_used == 1)
                continue;
            in_db->common_info->num_pending_del_entries--;
            if (i == in_db->common_info->num_pending_del_entries)
                continue;
            in_db->common_info->del_entry_list[i] =
                in_db->common_info->del_entry_list[in_db->common_info->num_pending_del_entries];
            i--;
        }
    }

    /*
     * Iterate through record list and add delete entry from device 
     */
    kaps_c_list_iter_init(&db->db_info.db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_lpm_entry *entry = DBLIST_TO_LPM_ENTRY(el);

        if (entry->pfx_bundle->reconcile_used == 1)
            continue;

        err_num = kaps_ftm_delete_prefix(db->fib_tbl, entry, &reason_code);
        if (err_num != NLMERR_OK)
            return kaps_map_reason_code_to_kaps_status(reason_code);

        err_num = kaps_ftm_update_hash_table(db->fib_tbl, entry, 1);
        if (err_num != NLMERR_OK)
        {
            return KAPS_INTERNAL_ERROR;
        }

        kaps_c_list_remove_node(&db->db_info.db_list, el, &it);
        kaps_pfx_bundle_destroy(entry->pfx_bundle, &db->db_info.device->nlm_alloc);

        if (entry->ad_handle)
        {

            struct kaps_ad_db *e_ad_db = NULL;
            int32_t ref_count = 0;

            KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(in_db->device, entry->ad_handle, e_ad_db);

            if (entry->hb_user_handle && in_db->common_info->hb_info.hb)
            {
                struct kaps_hb *hb = NULL;

                KAPS_WB_HANDLE_READ_LOC((in_db->common_info->hb_info.hb), (&hb), (uintptr_t) entry->hb_user_handle);
                if (hb == NULL)
                {
                    return KAPS_INTERNAL_ERROR;
                }
                hb->entry = NULL;
            }
            KAPS_STRY(kaps_ad_db_deassociate_entry_and_ad
                      (in_db->device, entry->ad_handle, (struct kaps_entry *) entry));

            if (e_ad_db->user_width_1)
                ref_count = entry->ad_handle->ref_count;
            else
                ref_count = e_ad_db->num_0b_entries;

            if (ref_count == 0)
            {
                KAPS_STRY(kaps_ad_db_delete_entry((struct kaps_ad_db *) in_db->common_info->ad_info.ad,
                                                  KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_ad,
                                                                                  entry->ad_handle->user_handle)));
            }
        }

        KAPS_WB_HANDLE_DELETE_LOC(in_db, (uintptr_t) entry->user_handle);
        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
    }

    /*
     * Iterate through pending list and add delete entry from device 
     */
    kaps_c_list_iter_init(&db->pending, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_lpm_entry *entry = DBLIST_TO_LPM_ENTRY(el);

        if (entry->pfx_bundle->reconcile_used == 1)
            continue;

        err_num = kaps_ftm_update_hash_table(db->fib_tbl, entry, 1);
        if (err_num != NLMERR_OK)
        {
            return KAPS_INTERNAL_ERROR;
        }

        kaps_c_list_remove_node(&db->pending, el, &it);
        kaps_pfx_bundle_destroy(entry->pfx_bundle, &db->db_info.device->nlm_alloc);

        if (entry->ad_handle)
        {

            struct kaps_ad_db *e_ad_db = NULL;
            int32_t ref_count = 0;

            KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(in_db->device, entry->ad_handle, e_ad_db);

            if (entry->hb_user_handle && in_db->common_info->hb_info.hb)
            {
                struct kaps_hb *hb = NULL;

                KAPS_WB_HANDLE_READ_LOC((in_db->common_info->hb_info.hb), (&hb), (uintptr_t) entry->hb_user_handle);
                if (hb == NULL)
                {
                    return KAPS_INTERNAL_ERROR;
                }
                hb->entry = NULL;
            }
            KAPS_STRY(kaps_ad_db_deassociate_entry_and_ad
                      (in_db->device, entry->ad_handle, (struct kaps_entry *) entry));

            if (e_ad_db->user_width_1)
                ref_count = entry->ad_handle->ref_count;
            else
                ref_count = e_ad_db->num_0b_entries;

            if (ref_count == 0)
            {

                KAPS_STRY(kaps_ad_db_delete_entry((struct kaps_ad_db *) in_db->common_info->ad_info.ad,
                                                  KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_ad,
                                                                                  entry->ad_handle->user_handle)));
            }
        }

        KAPS_WB_HANDLE_DELETE_LOC(in_db, (uintptr_t) entry->user_handle);
        POOL_FREE(kaps_lpm_entry, &db->lpm_entry_pool, entry);
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_entry_set_used(
    struct kaps_entry *entry)
{
    struct kaps_lpm_entry *lpm_entry = (struct kaps_lpm_entry *) entry;

    if (!lpm_entry)
        return KAPS_INVALID_ARGUMENT;
    lpm_entry->pfx_bundle->reconcile_used = 1;
    return KAPS_OK;
}

static kaps_status
kaps_lpm_entry_reset_used(
    struct kaps_entry *entry)
{
    struct kaps_lpm_entry *lpm_entry = (struct kaps_lpm_entry *) entry;

    if (!lpm_entry)
        return KAPS_INVALID_ARGUMENT;
    lpm_entry->pfx_bundle->reconcile_used = 0;
    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_reconcile_end(
    struct kaps_db *in_db)
{
    struct kaps_db *tmp;

    for (tmp = in_db; tmp; tmp = tmp->next_tab)
    {
        if (tmp->is_clone)
            continue;
        KAPS_STRY(kaps_lpm_db_reconcile_end_db(tmp));
    }
    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_get_index(
    struct kaps_db *in_db,
    struct kaps_entry *in_e,
    int32_t * nindices,
    int32_t ** indices)
{
    struct kaps_device *device = in_db->device;
    NlmReasonCode reason;
    NlmErrNum_t err_num;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    struct kaps_lpm_entry *entry = (struct kaps_lpm_entry *) in_e;
    kaps_fib_tbl *fib_tbl;

    *nindices = 0;
    *indices = NULL;

    if (device->type == KAPS_DEVICE_KAPS)
        return KAPS_UNSUPPORTED;

    if (!entry->pfx_bundle || entry->pfx_bundle->m_nIndex == KAPS_LSN_NEW_INDEX)
        return KAPS_OK;

    if (db->db_info.is_clone)
        db = (struct kaps_lpm_db *) db->db_info.clone_of;

    fib_tbl = db->fib_tbl;

    err_num = kaps_ftm_get_index(fib_tbl, entry, nindices, indices, &reason);

    if (err_num != NLMERR_OK)
        return kaps_map_reason_code_to_kaps_status(reason);

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_get_prefix_handle(
    struct kaps_db *in_db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_entry **entry)
{
    NlmErrNum_t err_num;
    uint8_t *sw_prefix;
    uint8_t temp_prefix[KAPS_LPM_KEY_MAX_WIDTH_8];
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    uint32_t tmp = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(length);

    sw_prefix = prefix;
    if (in_db->common_info->is_extra_byte_added)
    {
        /*
         * Handle an extra byte at MS portion 
         */
        temp_prefix[0] = 0;
        kaps_memcpy(&temp_prefix[1], prefix, (length + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE);
        sw_prefix = temp_prefix;
        length += KAPS_BITS_IN_BYTE;
    }

    /*
     * make sure mask out the fraction bits 
     */
    if (prefix)
    {
        uint32_t endByte = tmp - 1;

        if (in_db->common_info->is_extra_byte_added)
        {
            endByte = tmp;
        }
        if (length & 7)
        {
            uint32_t mask = (KAPS_PREFIX_GET_BIT_MASK(length - 1) - 1);

            sw_prefix[endByte] &= ~mask;
        }
    }
    err_num = kaps_ftm_get_prefix_handle(db->fib_tbl, sw_prefix, length, (struct kaps_lpm_entry **) entry);

    if (err_num != NLMERR_OK || *entry == NULL)
    {
        return KAPS_PREFIX_NOT_FOUND;
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_db_get_fib_stats(
    struct kaps_lpm_db * lpm_db,
    struct NlmFibStats * fib_stats)
{
    if (fib_stats)
    {
        struct kaps_lpm_db *tmp;

        for (tmp = lpm_db; tmp; tmp = (struct kaps_lpm_db *) tmp->db_info.next_tab)
        {
            if (!tmp->fib_tbl)
                continue;

            kaps_ftm_calc_stats(tmp->fib_tbl);
            *fib_stats = tmp->fib_tbl->m_fibStats;
            break;
        }
    }
    return KAPS_OK;
}

static void
kaps_lpm_print_lpm_ab_details(
    struct kaps_device *device,
    struct kaps_db *db,
    struct NlmFibStats *fib_stats)
{
    uint32_t i, numABsProcessed;

    /*
     * There will be too many ABs for 3 level KAPS. So skip printing 
     */
    if (db->num_algo_levels_in_db == 3)
    {
        return;
    }

    numABsProcessed = 0;
    for (i = 0; i < KAPS_MAX_NUM_POOLS; ++i)
    {

        if (fib_stats->isABUsed[i])
        {

            kaps_printf("\t\t\t");

            kaps_printf("AB - %3d, N_Trig = %4d, N_Cols = %3d, Width = ", i, fib_stats->numTriggersInAB[i],
                        fib_stats->numColsInAB[i]);

            if (fib_stats->widthOfAB[i] == KAPS_NORMAL_80)
            {
                kaps_printf("  80b, ");
            }
            else if (fib_stats->widthOfAB[i] == KAPS_NORMAL_160)
            {
                kaps_printf(" 160b, ");
            }

            if (fib_stats->isIPTAB[i])
            {
                kaps_printf("IPT");
            }
            else
            {
                kaps_printf("APT");
            }

            numABsProcessed++;
            kaps_printf("\n");
        }
    }
    kaps_printf("\n");
}

kaps_status
kaps_lpm_db_print_fib_stats_internal(
    struct kaps_db *db,
    struct NlmFibStats *user_fib_stats,
    struct kaps_db_stats *user_db_stats)
{
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) db;
    struct kaps_device *device = db->device;
    struct kaps_ad_db *ad_db, *tmp;
    struct NlmFibStats *fib_stats_ptr = NULL;
    struct kaps_db_stats db_stats;
    kaps_status status;
    uint32_t i, gran;
    uint32_t numBitsWritten;
    uint32_t was_memory_allocated;
    uint32_t num_prefixes;
    double mbitsWritten;

    kaps_sassert(db != NULL);
    kaps_sassert(lpm_db != NULL);

    ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;

    was_memory_allocated = 0;
    if (user_fib_stats)
    {
        fib_stats_ptr = user_fib_stats;
    }
    else
    {
        fib_stats_ptr =
            lpm_db->db_info.device->alloc->xcalloc(lpm_db->db_info.device->alloc->cookie, 1,
                                                   sizeof(struct NlmFibStats));
        if (fib_stats_ptr == NULL)
            return KAPS_OUT_OF_MEMORY;

        was_memory_allocated = 1;
        (void) kaps_lpm_db_get_fib_stats(lpm_db, fib_stats_ptr);
    }

    if (user_db_stats)
    {
        db_stats = *user_db_stats;
    }
    else
    {
        status = kaps_db_stats(db, &db_stats);
        if (status != KAPS_OK)
        {
            if (was_memory_allocated)
                lpm_db->db_info.device->alloc->xfree(lpm_db->db_info.device->alloc->cookie, fib_stats_ptr);
            return status;
        }
    }

    kaps_printf("\nTable Id = %d, Table width = %d, User capacity = %d\n", db->tid, db->key->width_1,
                db->common_info->capacity);

    kaps_printf("Core-Id = %d, Number of Algo levels = %d\n", device->core_id, db->num_algo_levels_in_db);

    num_prefixes = 0;
    if (db->common_info->is_cascaded)
    {
        if (device->core_id == 0 && lpm_db->core0_fib_tbl)
        {
            num_prefixes = lpm_db->core0_fib_tbl->m_numPrefixes;
        }
        else if (device->core_id == 1 && lpm_db->core1_fib_tbl)
        {
            num_prefixes = lpm_db->core1_fib_tbl->m_numPrefixes;
        }
    }
    else
    {
        num_prefixes = lpm_db->fib_tbl->m_numPrefixes;
    }

    kaps_printf("Number of Prefixes = %d\n", num_prefixes);
    tmp = ad_db;
    while (tmp)
    {

        kaps_printf("AD ID = %d, Width = %d, Entries = %d\n", tmp->db_info.tid, tmp->user_width_1,
                    tmp->num_entries[tmp->db_info.device->core_id]);
        tmp = tmp->next;
    }

    kaps_printf("\nTRIE_STATS   : RPT = %d, IPT = %d, APT Lmpsofar = %d, APT Long Pfx = %d, Trie Nodes = %d\n",
                fib_stats_ptr->numRPTEntries, fib_stats_ptr->numIPTEntries,
                fib_stats_ptr->numAPTLmpsofarEntries, fib_stats_ptr->numAptLongPfx, fib_stats_ptr->numTrieNodes);

    kaps_printf("DBA          : RXC AB Assigned = %d, Total AB Used = %d, AB Released = %d \n",
                fib_stats_ptr->rxcTotalNumABAssigned,
                (fib_stats_ptr->numTotalIptAB + fib_stats_ptr->numTotalAptAB), lpm_db->num_released_ab);

    kaps_printf("\t\t IPT AB = %3d (80b = %3d, 160b = %3d, 80b Small 80b = %3d, Small 160b = %3d), Only Clone = %3d \n",
                fib_stats_ptr->numTotalIptAB, fib_stats_ptr->numTotal80bIptAB, fib_stats_ptr->numTotal160bIptAB,
                fib_stats_ptr->numTotal80bSmallIptAB, fib_stats_ptr->numTotal160bSmallIptAB,
                fib_stats_ptr->numDupIptAB);

    kaps_printf("\t\t APT AB = %3d (80b = %3d, 160b = %3d, 80b Small 80b = %3d, Small 160b = %3d), Only Clone = %3d \n",
                fib_stats_ptr->numTotalAptAB, fib_stats_ptr->numTotal80bAptAB, fib_stats_ptr->numTotal160bAptAB,
                fib_stats_ptr->numTotal80bSmallAptAB, fib_stats_ptr->numTotal160bSmallAptAB,
                fib_stats_ptr->numDupAptAB);

    kaps_printf("\t\t 40b Trig = %d, 80b Trig = %d, 160b Trig = %d, Avg Trig Len = %d\n",
                fib_stats_ptr->num40bTriggers, fib_stats_ptr->num80bTriggers, fib_stats_ptr->num160bTriggers,
                fib_stats_ptr->avgTrigLen);

    kaps_lpm_print_lpm_ab_details(device, db, fib_stats_ptr);

    kaps_printf
        ("UDA          : Used Bricks = %d, Used but empty bricks = %d, Partially Occupied Bricks = %d, Free Bricks = %d, Number of Regions = %d\n",
         fib_stats_ptr->numUsedBricks, fib_stats_ptr->numUsedButEmptyBricks, fib_stats_ptr->numPartiallyOccupiedBricks,
         fib_stats_ptr->numFreeBricks, fib_stats_ptr->numRegions);

    kaps_printf("DBA Compress : No Compression = %d, Compression 1 = %d, Compression 2 = %d\n",
                lpm_db->num_ab_no_compression, lpm_db->num_ab_compression_1, lpm_db->num_ab_compression_2);

    kaps_printf("UDA Big Ops  : Num UDA Compactions = %d, Num UDA Dyn Alloc = %d, Num UDA Expansions = %d\n",
                fib_stats_ptr->numUDACompactionCalls, fib_stats_ptr->numUDADynamicAllocCalls,
                fib_stats_ptr->numUDAExpansionCalls);

    kaps_printf
        ("RECURSIVE    : Recursive RPT splits = %d, Recursive IPT splits = %d, Max RPT splits per op = %d, Max IPT splits per op = %d\n",
         fib_stats_ptr->numRecursiveRptSplits, fib_stats_ptr->numRecursiveIptSplits,
         fib_stats_ptr->maxNumRptSplitsInOneOperation, fib_stats_ptr->maxNumIptSplitsInOneOperation);

    kaps_printf
        ("SRCH_INFO    : Unique Lmpsofars present = %d, Lmpsofars hit during search = %d, Number of misses = %d\n",
         fib_stats_ptr->numLmpsofarPfx, fib_stats_ptr->numLmpsofarHitInSearch, fib_stats_ptr->numMissesInSearch);

    kaps_printf("DEV_WRITES   : PIOWrites = %d, ", fib_stats_ptr->numPIOWrites);

    if (db->num_algo_levels_in_db == 2)
    {
        kaps_printf(" RPB = %9d, ADS = %9d, BKT = %9d  \n",
                    fib_stats_ptr->numIPTWrites, fib_stats_ptr->numIITWrites, fib_stats_ptr->numLSNRmw);

        kaps_printf("               HB read = %9d, HB write = %9d, HB copy = %9d \n",
                    fib_stats_ptr->numHitBitReads, fib_stats_ptr->numHitBitWrites, fib_stats_ptr->numHitBitCopy);

        kaps_printf("               Total H/W Writes = %9d, ", fib_stats_ptr->numIPTWrites + fib_stats_ptr->numIITWrites
                    + fib_stats_ptr->numLSNRmw + fib_stats_ptr->numHitBitWrites + fib_stats_ptr->numHitBitCopy);

        /*
         * Multiply the RPB width by 2 since we will have to write data and mask
         */
        numBitsWritten = (fib_stats_ptr->numIPTWrites * KAPS_RPB_WIDTH_1 * 2);
        numBitsWritten += (fib_stats_ptr->numIITWrites * KAPS_ADS_WIDTH_1);
        numBitsWritten += (fib_stats_ptr->numLSNRmw * KAPS_BKT_WIDTH_1);
        numBitsWritten += (fib_stats_ptr->numHitBitWrites * KAPS_HB_ROW_WIDTH_1);
        numBitsWritten += (fib_stats_ptr->numHitBitCopy * KAPS_HB_ROW_WIDTH_1);

        kaps_printf("Mbits: %0.2lf \n", numBitsWritten * 1.0 / (1024 * 1024));

    }
    else
    {
        kaps_printf(" RPB = %d, ADS-1 = %d,", fib_stats_ptr->numRPTWrites, fib_stats_ptr->numRITWrites);
        kaps_printf(" Small BB = %d, ADS-2 = %d,", fib_stats_ptr->numIPTWrites, fib_stats_ptr->numIITWrites);
        kaps_printf(" Large BB = %d\n", fib_stats_ptr->numLSNRmw);
        kaps_printf("               HB read = %9d, HB write = %9d, HB copy = %9d \n",
                    fib_stats_ptr->numHitBitReads, fib_stats_ptr->numHitBitWrites, fib_stats_ptr->numHitBitCopy);

        kaps_printf("               Total H/W Writes = %9d, ",
                    fib_stats_ptr->numRPTWrites + fib_stats_ptr->numRITWrites + fib_stats_ptr->numIPTWrites +
                    fib_stats_ptr->numIITWrites + fib_stats_ptr->numLSNRmw + fib_stats_ptr->numHitBitWrites +
                    fib_stats_ptr->numHitBitCopy);

        /*
         * Multiply the RPB width by 2 since we will have to write data and mask
         */
        numBitsWritten = (fib_stats_ptr->numRPTWrites * KAPS_RPB_WIDTH_1 * 2);
        numBitsWritten += (fib_stats_ptr->numRITWrites * KAPS_ADS_WIDTH_1);
        numBitsWritten += (fib_stats_ptr->numIPTWrites * KAPS_BKT_WIDTH_1);
        numBitsWritten += (fib_stats_ptr->numIITWrites * KAPS_ADS_WIDTH_1);
        numBitsWritten += (fib_stats_ptr->numLSNRmw * KAPS_BKT_WIDTH_1);
        numBitsWritten += (fib_stats_ptr->numHitBitWrites * KAPS_HB_ROW_WIDTH_1);
        numBitsWritten += (fib_stats_ptr->numHitBitCopy * KAPS_HB_ROW_WIDTH_1);

        mbitsWritten = numBitsWritten * 1.0 / (1024 * 1024);

        kaps_printf("Mbits: %0.2lf\n", mbitsWritten);



    }

    kaps_printf("\nAD_STATS     :");
    while (ad_db)
    {
        kaps_printf("\n              AD ID = %d, AD Width = %3d, Entries = %d \n",
                ad_db->db_info.tid, ad_db->user_width_1, ad_db->ad_list.count); 
        
        for (i = 0; i < device->hw_res->num_daisy_chains; ++i) 
        {
            struct kaps_ix_mgr *kaps_ix_mgr = ad_db->mgr[ad_db->db_info.device->core_id][i];

            if (kaps_ix_mgr)
            {
                kaps_printf
                    ("             \t Daisy Chain = %d, IX Allocated = %d, IX Free = %d \n",
                     kaps_ix_mgr->daisy_chain_id, kaps_ix_mgr->num_allocated_ix, 
                     kaps_ix_mgr->size - kaps_ix_mgr->num_allocated_ix);
            }
        }
        ad_db = ad_db->next;
    }
    kaps_printf("\n");

    if (db->num_algo_levels_in_db == 3)
    {
        kaps_printf
            ("\nSMALL_BB     : Total Bricks = %d, Used Bricks = %d, Free Bricks = %d, Total LSN Holes = %d, Max LSN width = %d, Average LSN Width = %.2f\n",
             fib_stats_ptr->kaps_small_bb_stats.maxBricks, fib_stats_ptr->kaps_small_bb_stats.numUsedBricks,
             fib_stats_ptr->kaps_small_bb_stats.numFreeBricks, fib_stats_ptr->kaps_small_bb_stats.totalNumHolesInLsns,
             fib_stats_ptr->kaps_small_bb_stats.maxLsnWidth,
             (float) ((1.0 * fib_stats_ptr->kaps_small_bb_stats.numUsedBricks) / fib_stats_ptr->numRPTEntries));

        for (i = 0; i < device->hw_res->lpm_num_gran; ++i)
        {
            gran = device->hw_res->lpm_gran_array[i];
            if (fib_stats_ptr->kaps_small_bb_stats.numBricksForEachGran[i])
            {
                kaps_printf("               Ix = %3d, Gran = %3d, N_Bricks = %8d, N_Pfx = %8d, N_Holes = %8d\n",
                            i, gran, fib_stats_ptr->kaps_small_bb_stats.numBricksForEachGran[i],
                            fib_stats_ptr->kaps_small_bb_stats.numPfxForEachGran[i],
                            fib_stats_ptr->kaps_small_bb_stats.numHolesForEachGran[i]);
            }
        }

        for (i = 1; i <= lpm_db->fib_tbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn; i++)
        {
            if (fib_stats_ptr->kaps_small_bb_stats.numLsnForEachSize[i - 1])
                kaps_printf("               Width = %2d, N_Lsns = %8d, N_Holes = %8d\n",
                            i, fib_stats_ptr->kaps_small_bb_stats.numLsnForEachSize[i - 1],
                            fib_stats_ptr->kaps_small_bb_stats.numHolesForEachLsnSize[i - 1]);
        }
        kaps_printf("\n");


        kaps_printf("\nADS-2        : Num ADS-2 slots used = %d, Num ADS-2 slots free = %d\n",
                        fib_stats_ptr->numUsedAds2Slots, fib_stats_ptr->numFreeAds2Slots);
    }

    kaps_printf("\nHOLE_INFO    :  Num holes in non-last bricks = %d, Num holes in last brick = %d\n",
                                fib_stats_ptr->numHolesInNonLastBricks, fib_stats_ptr->numHolesInLastBrick);

    kaps_printf("                Num empty non-last bricks = %d, Num empty last bricks = %d\n",
                                fib_stats_ptr->numNonLastBricksThatAreEmpty, fib_stats_ptr->numLastBricksThatAreEmpty);

    kaps_printf
        ("\nLSN_STATS    : GiveOuts = %d, Push = %d, Shrinks = %d, Anc Merges = %d, Rel Merges = %d, Total LSN Holes = %d ",
         fib_stats_ptr->numLsnGiveOuts, fib_stats_ptr->numLsnPush, fib_stats_ptr->numLsnShrinks,
         fib_stats_ptr->numLsnMerges, fib_stats_ptr->numRelatedLsnMerges, fib_stats_ptr->totalNumHolesInLsns);

    if (db_stats.num_entries)
    {
        uint16_t lpu_width = 1024;

        if (device->type == KAPS_DEVICE_KAPS)
            lpu_width = KAPS_BKT_WIDTH_1;

        kaps_printf(" Bits/pfx = %.02f",
                    (float) ((1.0 * (fib_stats_ptr->numUsedBricks + fib_stats_ptr->numFreeBricks) * lpu_width) /
                             db_stats.num_entries));
    }
    kaps_printf("\n");

    
    kaps_printf("             : Num LSNs with exactly matching prefix = %d, Num LSNs that can be pushed deeper = %d\n",
         fib_stats_ptr->numLsnsWithPrefixMatchingDepth, fib_stats_ptr->numLsnsThatCanbePushedDeeper);

    kaps_printf("             : Max LSN width = %d, Average LSN Width = %.2f, Initial LSN Width = %d\n\n",
                fib_stats_ptr->maxLsnWidth,
                (float) ((1.0 * fib_stats_ptr->numUsedBricks) / fib_stats_ptr->numIPTEntries),
                fib_stats_ptr->initialLsnWidth);

    for (i = 0; i < device->hw_res->lpm_num_gran; ++i)
    {
        gran = device->hw_res->lpm_gran_array[i];
        if (fib_stats_ptr->numBricksForEachGran[i])
        {
            kaps_printf
                ("               Ix = %3d, Gran = %3d, N_Bricks = %8d (Empty_Bricks = %8d), N_Pfx = %8d, N_Holes = %8d\n",
                 i, gran, fib_stats_ptr->numBricksForEachGran[i], fib_stats_ptr->numEmptyBricksForEachGran[i],
                 fib_stats_ptr->numPfxForEachGran[i], fib_stats_ptr->numHolesForEachGran[i]);
        }
    }

    for (i = 1; i <= lpm_db->fib_tbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn; i++)
    {
        if (fib_stats_ptr->numLsnForEachSize[i - 1])
            kaps_printf("               Width = %2d, N_Lsns = %8d, N_Holes = %8d\n",
                        i, fib_stats_ptr->numLsnForEachSize[i - 1], fib_stats_ptr->numHolesForEachLsnSize[i - 1]);
    }

    kaps_printf("\n");
    kaps_printf("RPT_STATS    : ");
    if (fib_stats_ptr->numRPTEntries)
    {
        kaps_printf("Splits = %d, Pushes = %d, Moves = %d Average RPT loppedoff = %.2f bits",
                    fib_stats_ptr->numRptSplits, fib_stats_ptr->numRptPushes, fib_stats_ptr->numRptMoves,
                    (float) ((1.0 * fib_stats_ptr->totalRptBytesLoppedOff * 8) / fib_stats_ptr->numRPTEntries));
    }
    kaps_printf("\n");
    for (i = 0; i <= KAPS_LPM_KEY_MAX_WIDTH_8; ++i)
    {
        if (fib_stats_ptr->rpt_lopoff_info[i])
        {
            kaps_printf("                Length = %d bits, Num entries = %d\n", i * 8,
                        fib_stats_ptr->rpt_lopoff_info[i]);
        }
    }
    kaps_printf("\n");
    if (fib_stats_ptr->maxNumRptSplitsInANode)
    {
        kaps_printf("               Max Num RPT splits in a node = %d, Depth of node = %d, ID of node = %d\n",
                    fib_stats_ptr->maxNumRptSplitsInANode, fib_stats_ptr->depthofNodeWithMaxRptSplits,
                    fib_stats_ptr->idOfNodeWithMaxRptSplits);
    }

    kaps_printf("\n\n");

    if (was_memory_allocated)
        lpm_db->db_info.device->alloc->xfree(lpm_db->db_info.device->alloc->cookie, fib_stats_ptr);

    return KAPS_OK;
}

kaps_status
kaps_lpm_db_print_fib_stats(
    struct kaps_db * db,
    struct NlmFibStats * user_fib_stats,
    struct kaps_db_stats * user_db_stats)
{

    kaps_lpm_db_print_fib_stats_internal(db, user_fib_stats, user_db_stats);

    kaps_printf("_____________________________________________________\n");

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_get_hb(
    struct kaps_db *db,
    struct kaps_entry *entry,
    struct kaps_hb **hb)
{
    struct kaps_lpm_entry *lpm_entry = (struct kaps_lpm_entry *) entry;

    *hb = NULL;
    if (lpm_entry->hb_user_handle)
    {
        *hb = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_hb, lpm_entry->hb_user_handle);
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_update_ad(
    struct kaps_db *in_db,
    struct kaps_ad *ad_handle,
    struct kaps_entry *in_e,
    uint32_t num_bytes)
{
    NlmErrNum_t errNum;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    struct kaps_lpm_entry *e = (struct kaps_lpm_entry *) in_e;
    kaps_fib_tbl *fibTbl = db->fib_tbl;

    /*
     * If the entry has not yet been installed then simply return
     */
    if (e->pfx_bundle->m_status != COMMITTED)
        return KAPS_OK;

    fibTbl = db->fib_tbl;

    errNum = kaps_ftm_update_ad(fibTbl, e, &reason);

    if (errNum != NLMERR_OK)
        return kaps_map_reason_code_to_kaps_status(reason);

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_set_property(
    struct kaps_db *in_db,
    uint32_t property,
    va_list vl)
{
    int32_t value;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    uint32_t db_group_id = in_db->db_group_id;

    if (in_db->device->is_config_locked)
        return KAPS_DB_ACTIVE;

    switch (property)
    {
        case 110:
            value = va_arg(vl, int32_t);
            KAPS_TRACE_PRINT("%d", value);
            if (value != 0 && value != 1)
            {
                return KAPS_INVALID_ARGUMENT;
            }
            db->db_info.common_info->calc_shuffle_stats = value;
            break;
        case KAPS_PROP_MAX_UDA_LSN_SIZE:
            value = va_arg(vl, int32_t);
            KAPS_TRACE_PRINT("%d", value);
            if (value > in_db->device->num_udc || value < 1)
            {
                return KAPS_INVALID_ARGUMENT;
            }
            in_db->hw_res.db_res->lsn_info[0].max_lsn_size = value;
            in_db->common_info->user_specified_lsn_width = 1;
            break;

        case KAPS_PROP_NO_OVERFLOW_LPM:
            value = va_arg(vl, int32_t);
            KAPS_TRACE_PRINT("%d", value);
            if (value != 0 && value != 1)
                return KAPS_INVALID_ARGUMENT;
            if (value)
            {
                struct kaps_ad_db *tmp;

                tmp = (struct kaps_ad_db *) (db->db_info.common_info->ad_info.ad);
                if (tmp->next)
                    return KAPS_TOO_MANY_AD;
                db->db_info.hw_res.db_res->num_dba_dt = 0;
            }
            else
            {
                db->db_info.hw_res.db_res->num_dba_dt = 1;
            }
            break;
        case KAPS_PROP_NUM_BBS:
            value = va_arg(vl, int32_t);
            KAPS_TRACE_PRINT("%d", value);
            if (in_db->device->type != KAPS_DEVICE_KAPS)
                return KAPS_INVALID_ARGUMENT;
            if (value > (in_db->device->hw_res->total_num_bb[db_group_id] / in_db->device->hw_res->num_bb_in_one_chain))
                return KAPS_INVALID_ARGUMENT;
            if (value < 0)
                return KAPS_INVALID_ARGUMENT;
            in_db->hw_res.db_res->num_bbs = value;
            break;
        default:
            return KAPS_INVALID_ARGUMENT;
    }
    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_get_property(
    struct kaps_db *in_db,
    uint32_t property,
    va_list vl)
{
    int32_t *value, core_id;
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) in_db;

    switch (property)
    {
        case KAPS_PROP_NUM_AB_ACTIVE:
        case KAPS_PROP_MAX_LITUP_ABS:
            value = va_arg(vl, int32_t *);
            *value = LPM_MAX_AB_LIT_UP; /* max AB lit up for LPM are 2 */
            break;

        case KAPS_PROP_LPM_GET_LSN_SIZE:
            core_id = va_arg(vl, int32_t);
            value = va_arg(vl, int32_t *);

            if (lpm_db->fib_tbl == NULL)
            {
                *value = in_db->hw_res.db_res->lsn_info[0].max_lsn_size;
            }
            else
            {
                if (core_id == 0)
                {
                    *value = lpm_db->fib_tbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn;
                }
                else if (core_id == 1)
                {
                    kaps_sassert(in_db->common_info->is_cascaded);
                    *value = lpm_db->core1_fib_tbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn;
                }
                else
                {
                    kaps_sassert(0);
                }
            }
            break;

        default:
            return KAPS_INVALID_ARGUMENT;
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_entry_set_property(
    struct kaps_db *in_db,
    struct kaps_entry *entry,
    uint32_t property,
    va_list vl)
{
    struct kaps_lpm_entry *lpm_entry = (struct kaps_lpm_entry *) entry;
    kaps_status status = KAPS_OK;

    /*
     * Already Committed Entry 
     */
    if (lpm_entry->pfx_bundle->m_status == COMMITTED)
        return KAPS_INVALID_ARGUMENT;

    switch (property)
    {
        default:
            status = KAPS_INVALID_ARGUMENT;
            break;
    }
    return status;
}

static kaps_status
kaps_lpm_entry_get_length(
    struct kaps_db *in_db,
    struct kaps_entry *in_e,
    uint32_t * prio_length)
{
    struct kaps_lpm_entry *e = (struct kaps_lpm_entry *) in_e;

    *prio_length = e->pfx_bundle->m_nPfxSize;

    if (in_db->common_info->is_extra_byte_added)
    {
        /*
         * We have added an extra byte at MS portion for 12K and Oprime.
         */
        *prio_length = *prio_length - KAPS_BITS_IN_BYTE;
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_get_entry_info(
    struct kaps_db *in_db,
    struct kaps_entry *in_e,
    struct kaps_entry_info *info)
{
    int32_t i, num_bytes, rem_bits;
    uint32_t user_first_byte_pos;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    struct kaps_lpm_entry *entry = (struct kaps_lpm_entry *) in_e;

    kaps_memset(info, 0, sizeof(*info));
    kaps_memset(info->mask, -1, KAPS_HW_MAX_DBA_WIDTH_8);

    info->width_8 = db->db_info.width.key_width_1 / KAPS_BITS_IN_BYTE;

    if (entry->pfx_bundle->m_status == PENDING)
    {
        info->active = 0;
        return KAPS_PENDING_ENTRY;
    }
    if (entry->pfx_bundle->m_status == COMMITTED)
        info->active = 1;

    /*
     * We have added an extra byte at MS portion for 12K and Oprime. 
     */
    info->prio_len = entry->pfx_bundle->m_nPfxSize;
    if (in_db->common_info->is_extra_byte_added)
        info->prio_len -= KAPS_BITS_IN_BYTE;

    if (db->db_info.device->flags & KAPS_DEVICE_ISSU)
    {
        if (entry->ad_handle)
        {
            info->ad_handle = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_ad, entry->ad_handle->user_handle);
            KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(in_db->device, entry->ad_handle, info->ad_db);
            if (info->ad_db->parent_ad)
                info->ad_db = info->ad_db->parent_ad;
        }
    }
    else
    {
        info->ad_handle = entry->ad_handle;
        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(in_db->device, entry->ad_handle, info->ad_db);
        if (info->ad_db->parent_ad)
            info->ad_db = info->ad_db->parent_ad;
    }

    num_bytes = (info->prio_len + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE;
    user_first_byte_pos = 0;
    if (in_db->common_info->is_extra_byte_added)
        user_first_byte_pos = 1;

    kaps_memcpy(info->data, &entry->pfx_bundle->m_data[user_first_byte_pos], num_bytes);
    kaps_memset(info->mask, 0, info->prio_len / KAPS_BITS_IN_BYTE);

    /*
     * Make the mask bits for the trailing prefix part
     * valid from dont cares
     */
    rem_bits = info->prio_len % KAPS_BITS_IN_BYTE;
    for (i = 0; i < rem_bits; i++)
    {
        info->mask[num_bytes - 1] &= ~(1 << (KAPS_BITS_IN_BYTE - i - 1));
    }

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_fix_ab_location(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t ab_location,
    struct kaps_entry **find_entry)
{
    if (device->type == KAPS_DEVICE_KAPS)
        return KAPS_UNSUPPORTED;

    return KAPS_OK;
}


static kaps_status
kaps_lpm_process_hit_bits(
    struct kaps_device *device,
    struct kaps_db *db)
{
    NlmErrNum_t errNum;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) db;

    errNum = kaps_ftm_process_hit_bits(lpm_db->fib_tbl, &reason);
    if (errNum != NLMERR_OK)
        return kaps_map_reason_code_to_kaps_status(reason);

    return KAPS_OK;
}

static kaps_status
kaps_lpm_get_algo_hit_bit_value(
    struct kaps_device *device,
    struct kaps_db *db,
    struct kaps_entry *entry,
    uint8_t clear_on_read,
    uint32_t * bit_value)
{
    NlmErrNum_t errNum;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) db;

    errNum = kaps_ftm_get_algo_hit_bit_value(lpm_db->fib_tbl, entry, clear_on_read, bit_value, &reason);

    if (errNum != NLMERR_OK)
        return kaps_map_reason_code_to_kaps_status(reason);

    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_add_table(
    struct kaps_db *in_db,
    uint32_t id,
    uint32_t is_clone,
    struct kaps_db **table)
{
    struct kaps_allocator *alloc;
    struct kaps_lpm_db *new_tab;
    struct kaps_db *tmp_db, *parent;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;

    if (is_clone)
    {
        if (db->db_info.parent)
            parent = db->db_info.parent;
        else
            parent = &db->db_info;
    }
    else
    {
        kaps_sassert(db->db_info.parent == NULL);
        parent = &db->db_info;
    }

    /*
     * KBPSDK-580: check for duplicate table IDs
     */

    for (tmp_db = parent->next_tab; tmp_db; tmp_db = tmp_db->next_tab)
    {
        if (tmp_db->tid == id)
            return KAPS_DUPLICATE_TABLE_ID;
    }

    alloc = db->db_info.device->alloc;
    new_tab = alloc->xcalloc(alloc->cookie, 1, sizeof(*db));
    if (!new_tab)
        return KAPS_OUT_OF_MEMORY;
    kaps_memcpy(new_tab, db, sizeof(*db));
    new_tab->db_info.key = NULL;
    new_tab->db_info.parent = parent;
    new_tab->db_info.next_tab = parent->next_tab;
    new_tab->db_info.tid = id;
    new_tab->db_info.has_tables = 0;
    new_tab->db_info.has_clones = 0;
    parent->next_tab = &new_tab->db_info;

    POOL_INIT(kaps_lpm_entry, &new_tab->lpm_entry_pool, alloc);

    /*
     * Mark the top level database as having clones
     */

    parent->has_clones |= is_clone;
    parent->has_tables |= !is_clone;

    /*
     * This specific database/table has a clone, setup
     * pointers for each to reach back and forth
     */

    if (is_clone)
    {
        new_tab->db_info.is_clone = is_clone;
        new_tab->db_info.clone_of = &db->db_info;
    }

    *table = (struct kaps_db *) new_tab;
    return KAPS_OK;
}

static kaps_status
kaps_lpm_db_lock(
    struct kaps_db *in_db)
{
    struct kaps_device *device = in_db->device;
    struct kaps_db *tmp = in_db;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    uint32_t is_lpm_no_overflow = 0, s = 0, i = 0;

    if (in_db->is_bc_required && !in_db->is_main_bc_db)
        return KAPS_OK;

    POOL_INIT(kaps_lpm_entry, &db->lpm_entry_pool, device->alloc);

    if (device->main_dev)
        device = device->main_dev;

    {
        if (db->db_info.hw_res.db_res->num_dba_dt == 0)
        {
            struct kaps_ad_db *ad;

            ad = (struct kaps_ad_db *) (db->db_info.common_info->ad_info.ad);
            if (ad && ad->next)
                return KAPS_TOO_MANY_AD;
        }

        for (; tmp; tmp = tmp->next_tab)
        {
            struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) tmp;
            struct kaps_db *databases[50];
            uint32_t count = 0;

            if (tmp->is_clone)
                continue;

            kaps_memset(databases, 0, sizeof(databases));

            for (i = 0; i < tmp->ninstructions; i++)
            {
                struct kaps_instruction *inst = tmp->instructions[i];
                int32_t iter = -1, result_id = -1;

                if (inst->has_resolution == 0)
                    continue;

                for (s = 0; s < inst->num_searches; s++)
                {
                    if (inst->desc[s].db == tmp)
                    {
                        result_id = inst->desc[s].result_id;
                        break;
                    }
                }

                kaps_sassert(result_id != -1);

                for (s = 0; s < inst->num_searches; s++)
                {
                    if (inst->desc[s].db != tmp
                        && inst->desc[s].result_id == result_id && inst->desc[s].db->type == KAPS_DB_LPM)
                    {
                        struct kaps_db *cur_db = inst->desc[s].db;

                        if (cur_db->parent)
                            cur_db = cur_db->parent;

                        /*
                         * Check if this array has already having cur_db 
                         */
                        for (iter = 0; iter < count; iter++)
                        {
                            if (databases[iter] == cur_db)
                                break;
                        }

                        if (iter == count)
                            databases[count++] = cur_db;
                    }
                }
            }

            if (!count)
                continue;

            if (device->hw_res->no_overflow_lpm)
                is_lpm_no_overflow = 1;

            if (lpm_db->db_info.hw_res.db_res->num_dba_dt == 0)
                is_lpm_no_overflow = 1;

            for (i = 0; i < count; i++)
            {
                if (databases[i]->hw_res.db_res->num_dba_dt == 0)
                {
                    is_lpm_no_overflow = 1;
                    break;
                }
            }

            if (is_lpm_no_overflow)
            {
                for (i = 0; i < count; i++)
                {
                    if (lpm_db->db_info.common_info->meta_priority == databases[i]->common_info->meta_priority)
                        return KAPS_RESOLUTION_NOT_POSSIBLE;
                }
            }

            if (count)
            {
                lpm_db->resolution_dbs = device->alloc->xcalloc(device->alloc->cookie, count, sizeof(struct kaps_db *));
                if (!lpm_db->resolution_dbs)
                    return KAPS_OUT_OF_MEMORY;
                lpm_db->num_resolution_db = count;
                for (s = 0; s < count; s++)
                {
                    lpm_db->resolution_dbs[s] = databases[s];
                }
            }
        }
    }

    if (device->lpm_lock_done)
        return KAPS_OK;

    device->lpm_lock_done = 1;
    return kaps_lpm_lock(device);
}

kaps_status
kaps_lpm_db_create_broadcast(
    struct kaps_db * ori_db,
    struct kaps_db * bc_db)
{
    struct kaps_db *tmp;

    bc_db->hw_res.db_res->num_dt = ori_db->hw_res.db_res->num_dt;
    bc_db->hw_res.db_res->num_dba_dt = ori_db->hw_res.db_res->num_dba_dt;
    for (tmp = bc_db; tmp; tmp = tmp->next_tab)
    {
        tmp->hw_res.db_res->algorithmic = ori_db->hw_res.db_res->algorithmic;
    }
    return KAPS_OK;
}

struct kaps_db *
kaps_lpm_get_res_db(
    struct kaps_db *db)
{
    if (db->lpm_mgr && db->lpm_mgr->resource_db)
        return db->lpm_mgr->resource_db;
    else
        return db;
}






/*** Crash Recovery Business */

uint32_t
kaps_lpm_cr_calc_entry_mem(
    struct kaps_db * db_p)
{
    uint32_t mem_required;
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) db_p;

    mem_required = 0;

    /*
     * Entry info 
     */
    mem_required += sizeof(struct kaps_lpm_cr_entry_info);

    /*
     * AD data
     */
    if (db->db_info.common_info->ad_info.ad)
        mem_required += (sizeof(uint8_t) * (((struct kaps_ad_db *) db_p->common_info->ad_info.ad)->user_bytes));

    return mem_required;
}

kaps_status
kaps_lpm_cr_entry_shuffle(
    struct kaps_db * db_p,
    void *entry,
    uint32_t is_start)
{
    uint8_t *nv_ptr = NULL;
    struct kaps_lpm_cr_entry_info *shuffle_entry = NULL;
    struct kaps_entry *entry_p = (struct kaps_entry *) entry;
    struct kaps_lpm_entry *lpm_entry = (struct kaps_lpm_entry *) entry_p;
    uint32_t offset = 0;

    nv_ptr = db_p->device->nv_ptr;

    offset = kaps_db_entry_get_nv_offset(db_p, entry_p->user_handle);

    if (nv_ptr)
    {
        nv_ptr += offset;
        shuffle_entry = (struct kaps_lpm_cr_entry_info *) nv_ptr;
        shuffle_entry->status = 1;
        shuffle_entry->new_index = lpm_entry->pfx_bundle->m_nIndex;
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_cr_store_entry(
    struct kaps_db * db_p,
    struct kaps_entry * entry_p)
{
    uint8_t *nv_ptr = db_p->device->nv_ptr;
    struct kaps_db *main_db;
    struct kaps_lpm_cr_entry_info e_info, *pending_entry = NULL;
    uint32_t offset = 0;
    uint32_t pool_id;

    kaps_cr_pool_mgr_associate_user_handle(db_p->device->nv_mem_mgr->kaps_cr_pool_mgr, db_p, entry_p->user_handle,
                                           &pool_id);

    if (pool_id == KAPS_CR_INVALID_POOL_ID)
        return KAPS_DB_FULL;

    offset = kaps_db_entry_get_nv_offset(db_p, entry_p->user_handle);

    if (nv_ptr)
        nv_ptr += offset;

    if (nv_ptr)
        pending_entry = (struct kaps_lpm_cr_entry_info *) nv_ptr;
    else
        pending_entry = &e_info;

    if (db_p->parent)
        main_db = db_p->parent;
    else
        main_db = db_p;

    pending_entry->status = 0;
    pending_entry->marked_for_delete = 0;
    pending_entry->entry_user_handle = entry_p->user_handle;
    pending_entry->seq_num = db_p->seq_num;
    pending_entry->new_index = KAPS_FIB_INVALID_INDEX;

    if (entry_p->ad_handle)
    {
        pending_entry->ad_user_handle = entry_p->ad_handle->user_handle;
    }

    if (nv_ptr)
        nv_ptr += sizeof(*pending_entry);

    if (entry_p->ad_handle)
    {
        struct kaps_ad_db *ad_db;
        uint32_t user_width;
        uint32_t byte_offset;

        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db_p->device, entry_p->ad_handle, ad_db) user_width = ad_db->user_bytes;
        byte_offset = ((ad_db->db_info.width.ad_width_1 + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE - user_width);

        if (kaps_resource_get_ad_type(main_db) == KAPS_AD_TYPE_INPLACE)
            byte_offset = 0;

        if (nv_ptr)
        {
            kaps_memcpy(nv_ptr, (uint8_t *) (uint8_t *) & entry_p->ad_handle->value[byte_offset],
                        (sizeof(uint8_t) * user_width));
            nv_ptr += (sizeof(uint8_t) * user_width);
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_cr_delete_entry(
    struct kaps_db * db_p,
    void *entry,
    uint32_t is_start)
{
    uint8_t *nv_ptr = NULL;
    struct kaps_lpm_cr_entry_info *delete_entry = NULL;
    uint32_t offset = 0;
    struct kaps_entry *entry_p = (struct kaps_entry *) entry;

    kaps_cr_pool_mgr_disassociate_user_handle(db_p->device->nv_mem_mgr->kaps_cr_pool_mgr, db_p, entry_p->user_handle);

    nv_ptr = db_p->device->nv_ptr;

    offset = kaps_db_entry_get_nv_offset(db_p, entry_p->user_handle);

    if (nv_ptr)
    {
        nv_ptr += offset;
        delete_entry = (struct kaps_lpm_cr_entry_info *) nv_ptr;

        if (is_start)
        {
            delete_entry->status = KAPS_CR_ENTRY_DELETE;
            delete_entry->new_index = KAPS_FIB_INVALID_INDEX;
        }
        else
        {
            delete_entry->status = 0;
            /* delete_entry->marked_for_delete = 0;*/
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_ad_cr_update_ad(
    struct kaps_device * device,
    struct kaps_entry * e,
    struct kaps_ad * ad_handle,
    uint8_t * value)
{
    uint8_t *nv_ptr = device->nv_ptr;
    uint32_t offset = 0;
    struct kaps_lpm_cr_entry_info *cr_entry;
    struct kaps_db *db_p;
    struct kaps_ad_db *e_ad_db;

    KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, e, db_p)
        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, ad_handle, e_ad_db)
        offset = kaps_db_entry_get_nv_offset(db_p, e->user_handle);
    nv_ptr += offset;
    cr_entry = (struct kaps_lpm_cr_entry_info *) nv_ptr;
    nv_ptr += sizeof(struct kaps_lpm_cr_entry_info);

    if (e->ad_handle)
    {
        uint32_t user_width = e_ad_db->user_bytes;

        cr_entry->ad_user_handle = e->ad_handle->user_handle;
        kaps_memcpy(nv_ptr, value, user_width);
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_cr_get_adv_ptr(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    return KAPS_OK;
}

kaps_status
kaps_lpm_cr_reconcile(
    struct kaps_db * db)
{
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) db;
    struct kaps_db *tab, *main_db = db;
    struct kaps_ad_db *ad_db;
    uint8_t *nv_ptr = NULL;
    uint32_t offset, pending_count, i;
    kaps_status status;

    if (db->device->type != KAPS_DEVICE_KAPS)
        return KAPS_OK;

    if (db->parent)
        main_db = db->parent;

    nv_ptr = db->device->nv_ptr;

    offset = db->device->nv_mem_mgr->offset_device_pending_list;
    nv_ptr += offset;
    pending_count = *(uint32_t *) nv_ptr;
    nv_ptr += sizeof(uint32_t);

    /*
     * Handle Pending Deletes here 
     */
    for (i = 0; i < pending_count; i++)
    {
        struct kaps_lpm_cr_entry_info *pending_entry = NULL;
        struct kaps_db *cur_db;
        uint8_t *pending_nv_ptr = db->device->nv_ptr;
        uint32_t entry_offset;

        entry_offset = *(uint32_t *) nv_ptr;
        nv_ptr += sizeof(uint32_t);

        pending_nv_ptr += entry_offset;
        pending_entry = (struct kaps_lpm_cr_entry_info *) pending_nv_ptr;

        cur_db = db->device->seq_num_to_ptr->db_ptr[pending_entry->seq_num];
        if (cur_db->parent)
            cur_db = cur_db->parent;

        /*
         * This entry not belongs to this data base, so skip it
         */
        if (cur_db != main_db)
        {
            if ((main_db->device->seq_num_to_ptr->db_ptr[pending_entry->seq_num]->type == KAPS_DB_LPM)
                && pending_entry->marked_for_delete)
            {
                i += 6;
                nv_ptr += (6 * sizeof(uint32_t));
            }
            continue;
        }

        /*
         * this is deleted entry store entry handle here
         */
        if (pending_entry->marked_for_delete)
        {
            int32_t start_bit = 0, end_bit;
            uint8_t pfx_data[KAPS_LPM_KEY_MAX_WIDTH_8];
            uint32_t pfx_val;
            uint32_t pfx_len = 0;

            pfx_len = *(uint32_t *) nv_ptr;
            nv_ptr += sizeof(uint32_t);
            i++;

            kaps_memset(pfx_data, 0, KAPS_LPM_KEY_MAX_WIDTH_8);
            for (start_bit = 0; start_bit < KAPS_LPM_KEY_MAX_WIDTH_1; start_bit += 32)
            {
                end_bit = start_bit + 31;

                pfx_val = *(uint32_t *) nv_ptr;
                KapsWriteBitsInArray(pfx_data, KAPS_LPM_KEY_MAX_WIDTH_8, end_bit, start_bit, pfx_val);
                nv_ptr += sizeof(uint32_t);
                i++;
            }

            /*
             * This entry was a pending entry, so delete immediately 
             */
            if (pending_entry->new_index == -1)
                continue;

            tab = db->device->seq_num_to_ptr->db_ptr[pending_entry->seq_num];

            if (tab->parent)
                kaps_sassert(tab->parent == main_db);

            kaps_sassert(tab != NULL);
            status =
                kaps_db_wb_add_prefix(tab, pfx_data, pfx_len, pending_entry->new_index,
                                      pending_entry->entry_user_handle, 0);
            if (status == KAPS_DUPLICATE)
                continue;
            if (status != KAPS_OK)
                return status;

            pending_nv_ptr = db->device->nv_ptr;
            pending_nv_ptr += entry_offset;
            pending_nv_ptr += sizeof(*pending_entry);
            lpm_db = (struct kaps_lpm_db *) tab;
            lpm_db->fib_tbl->cr_user_handles[pending_entry->new_index] = pending_entry->entry_user_handle;
            ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;
            if (ad_db)
            {
                uint8_t ad_data[256];
                struct kaps_ad *ad_p;

                kaps_memcpy(&ad_data, pending_nv_ptr, ad_db->user_bytes);
                KAPS_STRY(kaps_ad_db_add_entry_internal(ad_db, ad_data, &ad_p, pending_entry->ad_user_handle));
                pending_nv_ptr += (sizeof(uint8_t) * ad_db->user_bytes);

                KAPS_TRY(kaps_entry_add_ad
                         (db, KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, pending_entry->entry_user_handle),
                          (struct kaps_ad *) ((uintptr_t) pending_entry->ad_user_handle)));
            }
        }
        else
            continue;
    }

    for (tab = db; tab; tab = tab->next_tab)
    {
        if (tab->is_clone)
            continue;
        lpm_db = (struct kaps_lpm_db *) tab;
        kaps_kaps_trie_wb_restore_iit_lmpsofar(lpm_db->fib_tbl->m_trie->m_roots_trienode_p, NULL);
    }

    return KAPS_OK;
}


kaps_status
kaps_device_lpm_memory_stats(
    struct kaps_device * device,
    FILE * fp)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(el);

        if (tmp->type == KAPS_DB_LPM)
        {
            for (; tmp; tmp = tmp->next_tab)
            {
                struct kaps_lpm_db *db = (struct kaps_lpm_db *) tmp;

                if (tmp->is_clone)
                    continue;

                if (tmp->parent == NULL)
                    kaps_fprintf(fp, "\nLPM algo database #%d entries #%d\n", tmp->tid, db->fib_tbl->m_numPrefixes);
                else
                    kaps_fprintf(fp, "LPM table #%d entries #%d\n", tmp->tid, db->fib_tbl->m_numPrefixes);

                POOL_STATS(kaps_lpm_entry, &db->lpm_entry_pool, fp);
            }

        }
    }

    return KAPS_OK;
}

kaps_status
kaps_lpm_db_init(
    struct kaps_device * device,
    uint32_t id,
    uint32_t capacity,
    struct kaps_db ** dbp)
{
    kaps_status status;
    struct kaps_lpm_db *db;
    struct kaps_db_fn_impl *fn_impl;

    if (!dbp)
        return KAPS_INVALID_ARGUMENT;

    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    db = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(*db));
    if (!db)
        return KAPS_OUT_OF_MEMORY;

    fn_impl = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(*fn_impl));
    if (!fn_impl)
    {
        device->alloc->xfree(device->alloc->cookie, db);
        return KAPS_OUT_OF_MEMORY;
    }


    db->db_info.device = device;
   
    
    status = kaps_resource_add_database(device, &db->db_info, id, capacity, 0, KAPS_DB_LPM);
    if (status != KAPS_OK)
    {
        device->alloc->xfree(device->alloc->cookie, fn_impl);
        device->alloc->xfree(device->alloc->cookie, db);
        return status;
    }

    db->db_info.num_algo_levels_in_db = device->hw_res->num_algo_levels;


    db->db_info.hw_res.db_res->start_lpu = 1;
    db->db_info.hw_res.db_res->end_lpu = 0;

    db->db_info.common_info->meta_priority = KAPS_ENTRY_META_PRIORITY_0;        /* Default priority */


    db->fib_tbl = NULL;
    db->db_info.fn_table = fn_impl;

    fn_impl->db_destroy = kaps_lpm_db_destroy;
    fn_impl->db_set_key = kaps_lpm_db_add_key;
    fn_impl->db_set_property = kaps_lpm_db_set_property;
    fn_impl->db_get_property = kaps_lpm_db_get_property;
    fn_impl->entry_set_property = kaps_lpm_entry_set_property;
    fn_impl->db_add_ace = NULL;
    fn_impl->db_add_prefix = kaps_lpm_db_add_entry;
    fn_impl->db_add_em = NULL;
    fn_impl->db_delete_entry = kaps_lpm_db_delete_entry;
    fn_impl->entry_add_range = NULL;
    fn_impl->entry_add_ad = kaps_lpm_entry_add_ad;
    fn_impl->entry_add_hb = kaps_lpm_entry_add_hb;
    fn_impl->entry_print = kaps_lpm_entry_print;
    fn_impl->db_install = kaps_lpm_db_install;
    fn_impl->db_stats = kaps_lpm_db_stats;
    fn_impl->db_search = kaps_lpm_db_search;
    fn_impl->db_add_table = kaps_lpm_db_add_table;
    fn_impl->db_delete_all_entries = kaps_lpm_db_delete_all_entries;
    fn_impl->db_fix_ab_location = kaps_lpm_db_fix_ab_location;
    fn_impl->db_process_hit_bits = kaps_lpm_process_hit_bits;
    fn_impl->db_get_algo_hit_bit_value = kaps_lpm_get_algo_hit_bit_value;
    fn_impl->entry_get_priority = kaps_lpm_entry_get_length;
    fn_impl->entry_get_index = kaps_lpm_db_get_index;
    fn_impl->get_prefix_handle = kaps_lpm_db_get_prefix_handle;
    fn_impl->entry_get_info = kaps_lpm_db_get_entry_info;
    fn_impl->update_ad = kaps_lpm_db_update_ad;
    fn_impl->get_ad = NULL;
    fn_impl->get_hb = kaps_lpm_db_get_hb;
    fn_impl->lock_db = kaps_lpm_db_lock;
    fn_impl->get_data_len = kaps_lpm_db_get_data_len;

    fn_impl->save_state = kaps_lpm_db_wb_save_state;
    fn_impl->restore_state = kaps_lpm_db_wb_restore_state;
    fn_impl->save_cr_state = kaps_lpm_db_cr_save_state;
    fn_impl->restore_cr_state = kaps_lpm_db_cr_restore_state;
    fn_impl->cr_calc_entry_mem = kaps_lpm_cr_calc_entry_mem;
    fn_impl->cr_store_entry = kaps_lpm_cr_store_entry;
    fn_impl->cr_delete_entry = kaps_lpm_cr_delete_entry;
    fn_impl->cr_entry_shuffle = kaps_lpm_cr_entry_shuffle;
    fn_impl->cr_entry_update_ad = kaps_lpm_ad_cr_update_ad;
    fn_impl->cr_reconcile = kaps_lpm_cr_reconcile;
    fn_impl->reconcile_end = kaps_lpm_db_reconcile_end;
    fn_impl->set_used = kaps_lpm_entry_set_used;
    fn_impl->reset_used = kaps_lpm_entry_reset_used;
    fn_impl->advanced_search_and_repair = NULL;

    *dbp = (struct kaps_db *) db;

    return KAPS_OK;
}

kaps_status
kaps_db_is_reduced_algo_levels(struct kaps_db *db)
{
    struct kaps_device *device = db->device;
    uint32_t is_reduced_algo_levels = 0;
    uint32_t db_group_id = db->db_group_id;

    if (device->hw_res->total_small_bb[db_group_id] && db->num_algo_levels_in_db < 3)
    {
        is_reduced_algo_levels = 1;
    }

    return is_reduced_algo_levels;
}


void
kaps_ftm_large_kaps2_calc_stats(
    struct kaps_db *in_db,
    struct large_kaps2_stats *stats)
{
    struct kaps_lpm_db *db = (struct kaps_lpm_db *) in_db;
    kaps_fib_tbl *fibTbl = db->fib_tbl;
    struct kaps_device *device = fibTbl->m_db->device;
    struct uda_mgr_stats uda_stats;
    uint32_t ads2_num = in_db->rpb_id;

    kaps_ftm_calc_stats(fibTbl);
    /*
     * RPB 
     */
    stats->total_rpb_rows = fibTbl->m_trie->m_trie_global->m_rpm_p->kaps_simple_dba.pool_size;
    stats->num_rpb_rows_used =
        fibTbl->m_trie->m_trie_global->m_rpm_p->kaps_simple_dba.pool_size -
        kaps_rpm_get_num_free_slots(fibTbl->m_trie->m_trie_global->m_rpm_p);

    /*
     * Small BBs 
     */
    stats->total_small_bb =
        (in_db->hw_res.db_res->end_small_bb_nr - in_db->hw_res.db_res->start_small_bb_nr +
         1.0) * (stats->total_rpb_rows / 2.0);
    stats->num_small_bb_used = fibTbl->m_fibStats.kaps_small_bb_stats.numUsedBricks;

    /*
     * AD2 
     */
    
    stats->total_ad2_rows = device->hw_res->ads2_depth[ads2_num];
    stats->num_ad2_rows_used =
        device->hw_res->ads2_depth[ads2_num] - device->kaps_shadow->ads2_overlay[ads2_num].num_free_it_slots;


    /*
     * Large BBs 
     */
    kaps_uda_mgr_calc_stats(fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0], &uda_stats);
    stats->total_large_bb_rows = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0]->total_lpu_bricks;
    stats->num_large_bb_rows_used = uda_stats.total_num_allocated_lpu_bricks;
}
