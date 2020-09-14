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

#include "kaps_handle.h"
#include "kaps_cr_pool_mgr.h"
#include "kaps_key_internal.h"

kaps_status
kaps_db_refresh_handle(
    struct kaps_device *device,
    struct kaps_db *stale_ptr,
    struct kaps_db **dbp)
{
    struct kaps_device *main_bc_device;
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;

    KAPS_TRACE_IN("%p %p %p\n", device, stale_ptr, dbp);
    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    main_bc_device = kaps_get_main_bc_device(device);

    if (main_bc_device->main_dev)
        main_bc_device = main_bc_device->main_dev;

    for (device = main_bc_device; device; device = device->next_bc_device)
    {

        if (!stale_ptr || !dbp)
            return KAPS_INVALID_ARGUMENT;

        if (!(device->flags & KAPS_DEVICE_ISSU))
            return KAPS_INVALID_FLAGS;

        if (device->issu_status == KAPS_ISSU_INIT)
            return KAPS_INVALID_ARGUMENT;

        kaps_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *tab = NULL;
            struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(e1);

            if (tmp->stale_ptr == (uintptr_t) stale_ptr)
            {
                *dbp = tmp;
                return KAPS_OK;
            }
            if (tmp->has_tables || tmp->has_clones)
            {
                for (tab = tmp->next_tab; tab; tab = tab->next_tab)
                {
                    if (tab->stale_ptr == (uintptr_t) stale_ptr)
                    {
                        *dbp = tab;
                        KAPS_TRACE_OUT("%p\n", *dbp);
                        return KAPS_OK;
                    }
                }
            }
        }
    }

    KAPS_TRACE_OUT("%p\n", 0);
    return KAPS_INVALID_ARGUMENT;
}

static kaps_status
kaps_db_wb_init(
    struct kaps_device *device,
    enum kaps_db_type type,
    uint32_t id,
    uint32_t capacity,
    int32_t max_capacity,
    uintptr_t stale_ptr,
    struct kaps_db **dbp,
    int32_t is_broadcast)
{
    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    if (!stale_ptr)
        return KAPS_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    /*
     * ARAD team is using this param to create
     * dummy databases. Capacity zero is fine
     * as when we try to add entries, we will fail
     * without resource
     */
    /*
     * if (capacity == 0) return KAPS_DYNAMIC_UNSUPPORTED; 
     */

    switch (type)
    {
        case KAPS_DB_LPM:
            KAPS_STRY(kaps_lpm_db_init(device, id, capacity, dbp));
            break;
        default:
            return KAPS_INVALID_DB_TYPE;
            break;
    }
    (*dbp)->stale_ptr = stale_ptr;

    (*dbp)->common_info->max_capacity = max_capacity;

    if (!is_broadcast)
    {
        KAPS_WB_HANDLE_TABLE_INIT((*dbp), capacity);
    }

    return KAPS_OK;
}

static kaps_status
kaps_db_wb_add_table(
    struct kaps_db *db,
    uint32_t id,
    uintptr_t stale_ptr,
    struct kaps_db **table)
{
    if (db == NULL || stale_ptr == 0)
        return KAPS_INVALID_ARGUMENT;

    /*
     * If the user inadvertently sent us a table pointer, instead
     * of the master database handle, we generate the master
     * database pointer here
     */

    if (db->parent)
        db = db->parent;

    if (db->device->is_config_locked)
        return KAPS_DB_ACTIVE;

    if (!db->fn_table || !db->fn_table->db_add_table)
        return KAPS_INTERNAL_ERROR;

    KAPS_STRY(db->fn_table->db_add_table(db, id, 0, table));
    (*table)->stale_ptr = stale_ptr;
    return KAPS_OK;
}

kaps_status
kaps_entry_set_used(
    struct kaps_db * db,
    struct kaps_entry * entry)
{
    struct kaps_entry *entry_p;

    KAPS_TRACE_IN("%p %p\n", db, entry);
    if (!db || !entry)
        return KAPS_INVALID_ARGUMENT;

    if (db->device->issu_status != KAPS_ISSU_RECONCILE_START)
        return KAPS_RECONCILE_NOT_STARTED;

    KAPS_WB_HANDLE_READ_LOC(db, (&entry_p), (uintptr_t) entry);

    return db->fn_table->set_used(entry_p);
}

static kaps_status
kaps_db_wb_clone(
    struct kaps_db *db,
    uint32_t id,
    uintptr_t stale_ptr,
    struct kaps_db **clone)
{
    if (!db || !stale_ptr)
        return KAPS_INVALID_ARGUMENT;

    if (db->device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    if (!db->fn_table || !db->fn_table->db_add_table)
        return KAPS_INTERNAL_ERROR;

    KAPS_STRY(db->fn_table->db_add_table(db, id, 1, clone));
    (*clone)->stale_ptr = stale_ptr;
    return KAPS_OK;
}

kaps_status
kaps_db_wb_save_info(
    struct kaps_db * db,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_db_wb_info *wb_info_ptr, *tmp = NULL;
    struct kaps_db *tmp_db;
    struct kaps_ad_db *ad_db;
    int32_t total_size;

    total_size = sizeof(*wb_info_ptr);
    if (db->description)
        total_size += strlen(db->description) + 1;

    if (wb_fun->nv_ptr == NULL)
    {
        wb_info_ptr = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, total_size);
        if (wb_info_ptr == NULL)
            return KAPS_OUT_OF_MEMORY;
    }
    else
    {
        tmp = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, total_size);
        if (tmp == NULL)
            return KAPS_OUT_OF_MEMORY;
        wb_info_ptr = tmp;
    }

    /* kaps_memset(wb_info_ptr, 0, total_size);*/

    ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;
    while (ad_db)
    {
        if (!ad_db->db_info.is_internal)
            wb_info_ptr->num_ad_databases++;
        ad_db = ad_db->next;
    }
    if (db->common_info->hb_info.hb)
    {
        struct kaps_hb_db *hb_db = (struct kaps_hb_db *) db->common_info->hb_info.hb;

        wb_info_ptr->has_hb = 1;
        wb_info_ptr->hb_id = hb_db->db_info.tid;
        wb_info_ptr->hb_capacity = hb_db->capacity;
        wb_info_ptr->hb_age_count = hb_db->age_count;
        wb_info_ptr->hb_stale_ptr = (uintptr_t) hb_db;
    }

    /*
     * Store the DB id, capacity and type
     */

    wb_info_ptr->tid = db->tid;
    wb_info_ptr->capacity = db->common_info->capacity;
    wb_info_ptr->max_capacity = db->common_info->max_capacity;
    wb_info_ptr->type = db->type;
    wb_info_ptr->smt_no = db->device->smt_no;
    wb_info_ptr->enable_dynamic_allocation = db->common_info->enable_dynamic_allocation;
    wb_info_ptr->enable_db_compaction = db->common_info->enable_db_compaction;
    wb_info_ptr->calc_shuffle_stats = db->common_info->calc_shuffle_stats;
    wb_info_ptr->has_dup_ad = db->has_dup_ad;

    wb_info_ptr->is_main_bc_db = db->is_main_bc_db;
    wb_info_ptr->is_replicated_internally = db->is_replicated_internally;
    wb_info_ptr->is_bc_required = db->is_bc_required;
    wb_info_ptr->bc_bitmap = db->bc_bitmap;
    wb_info_ptr->mapped_to_acl = db->common_info->mapped_to_acl;
    wb_info_ptr->is_em = db->common_info->is_em;

    wb_info_ptr->is_algo = db->common_info->is_algo;
    wb_info_ptr->has_dup_ad = db->has_dup_ad;
    wb_info_ptr->defer_deletes = db->common_info->defer_deletes_to_install;
    wb_info_ptr->meta_priority = db->common_info->meta_priority;
    wb_info_ptr->db_cascade_alternate_addition = db->common_info->db_cascade_alternate_addition;

    wb_info_ptr->is_algorithmic = kaps_db_get_algorithmic(db);
    wb_info_ptr->is_user_specified_algo_type = kaps_resource_get_is_user_specified_algo_mode(db);
    wb_info_ptr->highest_wide_ab_index = db->highest_wide_ab_index;
    wb_info_ptr->lowest_narrow_ab_index = db->lowest_narrow_ab_index;

    wb_info_ptr->insertion_mode = db->common_info->insertion_mode;
    if (db->relative_entry)
        wb_info_ptr->relative_entry_wb_handle = db->relative_entry->user_handle;
    else
        wb_info_ptr->relative_entry_wb_handle = 0;
    wb_info_ptr->relative_entry_position = db->relative_entry_position;

    wb_info_ptr->is_cascaded = db->common_info->is_cascaded;
    wb_info_ptr->locality = db->common_info->locality;

    wb_info_ptr->alg_type = 0;
    wb_info_ptr->num_algo_levels_in_db = db->num_algo_levels_in_db;

    wb_info_ptr->stale_ptr = (uintptr_t) db;

    if (db->place_on_which_half == KAPS_DB_PLACE_ON_RIGHT_HALF)
    {
        kaps_sassert(db->paired_db);

        wb_info_ptr->stale_left_db_ptr = (uintptr_t) db->paired_db;
    }
    else
    {
        wb_info_ptr->stale_left_db_ptr = (uintptr_t) NULL;
    }

    wb_info_ptr->desc = 0;
    wb_info_ptr->is_clone = 0;
    wb_info_ptr->is_table = 0;
    wb_info_ptr->clone_of = 0;

    if (db->description)
    {
        wb_info_ptr->desc = strlen(db->description) + 1;
        /*
         * copy the description string including terminating zero 
         */
        kaps_memcpy(wb_info_ptr->description, db->description, wb_info_ptr->desc);
    }

    /*
     * Store clone info if present
     */

    wb_info_ptr->has_tables = db->has_tables;
    wb_info_ptr->has_clones = db->has_clones;
    if (db->is_clone)
    {
        wb_info_ptr->clone_of = (uintptr_t) db->clone_of;
        wb_info_ptr->is_clone = db->is_clone;
    }

    if (!db->is_clone && db->parent)
        wb_info_ptr->is_table = 1;

    /*
     * Store the DB properties
     */

    wb_info_ptr->user_specified_range = 0;
    if (db->type == KAPS_DB_ACL)
    {
        wb_info_ptr->user_specified_range = db->common_info->user_specified_range;
        wb_info_ptr->prev_entry_priority = db->common_info->prev_entry_priority;
        wb_info_ptr->priority_pattern = db->common_info->priority_pattern;
    }
    wb_info_ptr->user_specified = db->common_info->user_specified;
    wb_info_ptr->cascade_override = db->common_info->cascade_override;
    wb_info_ptr->cascade_device = db->common_info->cascade_device;
    if (kaps_resource_get_ad_type(db) == KAPS_AD_TYPE_CUSTOM)
        wb_info_ptr->custom_ix_range = 1;
    wb_info_ptr->callback_data = db->common_info->callback_fn || db->common_info->callback_handle;

    if (wb_info_ptr->calc_shuffle_stats)
    {
        wb_info_ptr->pio_stats.num_blk_ops = db->common_info->pio_stats.num_blk_ops;
        wb_info_ptr->pio_stats.num_ix_cbs = db->common_info->pio_stats.num_ix_cbs;
        wb_info_ptr->pio_stats.num_of_piords = db->common_info->pio_stats.num_of_piords;
        wb_info_ptr->pio_stats.num_of_piowrs = db->common_info->pio_stats.num_of_piowrs;
        wb_info_ptr->pio_stats.num_of_worst_case_shuffles = db->common_info->pio_stats.num_of_worst_case_shuffles;
        wb_info_ptr->pio_stats.num_of_shuffles = db->common_info->pio_stats.num_of_shuffles;
        wb_info_ptr->pio_stats.num_ad_writes = db->common_info->pio_stats.num_ad_writes;
    }

    if (wb_info_ptr->user_specified)
    {
        wb_info_ptr->user_num_ab = db->common_info->user_num_ab;
        wb_info_ptr->user_uda_mb = db->common_info->user_uda_mb;
    }

    if (wb_info_ptr->user_specified_range)
    {
        wb_info_ptr->range_units = db->common_info->range_units;
    }

    if (wb_info_ptr->custom_ix_range)
    {
        wb_info_ptr->index_range_min = db->common_info->index_range_min;
        wb_info_ptr->index_range_max = db->common_info->index_range_max;
    }

    if (wb_info_ptr->callback_data)
    {
        wb_info_ptr->callback_fn = db->common_info->callback_fn;
        wb_info_ptr->callback_handle = db->common_info->callback_handle;
    }

    if (db->is_clone || db->parent)
    {
        if (db->next_tab)
            KAPS_STRY(kaps_db_wb_save_info(db->next_tab, wb_fun));
    }

    /*
     * Write all the data related to DB into the NV 
     */
    if (wb_fun->write_fn)
    {
        if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, total_size, *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
    }
    *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
    if (wb_fun->nv_ptr)
    {
        wb_info_ptr = (struct kaps_db_wb_info *) wb_fun->nv_ptr;
        kaps_memcpy(wb_info_ptr, tmp, total_size);
        wb_fun->nv_ptr += total_size;
    }

    if (!db->is_clone && !db->parent)
    {
        /*
         * Write all the data related to each AD database associated to this DB 
         */
        struct kaps_ad_db_wb_info ad_wb_info, *ad_wb_info_ptr;

        ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;
        while (ad_db)
        {
            if (ad_db->db_info.is_internal)
            {
                ad_db = ad_db->next;
                continue;
            }
            if (wb_fun->nv_ptr == NULL)
            {
                ad_wb_info_ptr = &ad_wb_info;
            }
            else
            {
                ad_wb_info_ptr = (struct kaps_ad_db_wb_info *) wb_fun->nv_ptr;
            }

            kaps_memset(&ad_wb_info, 0, sizeof(struct kaps_ad_db_wb_info));
            ad_wb_info_ptr->id = ad_db->db_info.tid;
            ad_wb_info_ptr->ad_width = ad_db->user_width_1;
            ad_wb_info_ptr->capacity = ad_db->db_info.common_info->capacity;
            ad_wb_info_ptr->stale_ptr = (uintptr_t) ad_db;
            ad_wb_info_ptr->hb_ad_stale_ptr = (uintptr_t) ad_db->hb_ad;
            ad_wb_info_ptr->user_specified = ad_db->db_info.common_info->user_specified;
            ad_wb_info_ptr->uda_mb = ad_db->db_info.common_info->uda_mb;
            if (wb_info_ptr->enable_dynamic_allocation)
            {
                kaps_memcpy(&ad_wb_info_ptr->ad_hw_res, ad_db->db_info.hw_res.ad_res,
                            sizeof(struct kaps_ad_db_hw_resource));
                if (ad_db->hb_ad)
                {
                    kaps_memcpy(&ad_wb_info_ptr->ad_hw_res_internal, ad_db->hb_ad->db_info.hw_res.ad_res,
                                sizeof(struct kaps_ad_db_hw_resource));
                }
                if (wb_fun->nv_ptr)
                {
                    if (!(db->device->flags & KAPS_DEVICE_SKIP_INIT))
                    {
                        db->device->alloc->xfree(db->device->alloc->cookie, ad_db->db_info.hw_res.ad_res);
                    }
                    ad_db->db_info.hw_res.ad_res = (struct kaps_ad_db_hw_resource *) &ad_wb_info_ptr->ad_hw_res;
                }
            }

            if (wb_fun->write_fn)
            {
                if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) ad_wb_info_ptr,
                                          sizeof(*ad_wb_info_ptr), *wb_fun->nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;
            }

            *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*ad_wb_info_ptr);
            if (wb_fun->nv_ptr)
                wb_fun->nv_ptr += sizeof(struct kaps_ad_db_wb_info);

            ad_db = ad_db->next;
        }
    }

    /*
     * Store the DB key
     */

    if (!db->is_clone && !db->parent && db->type == KAPS_DB_ACL)
        db->fn_table->save_property(db, wb_fun);

    KAPS_STRY(kaps_key_wb_save(db->key, wb_fun));

    if (db->is_clone || db->parent)
    {
        if (wb_fun->nv_ptr == NULL)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, wb_info_ptr);
        }
        return KAPS_OK;
    }
    /*
     * Recursively store the tables and clones in this DB
     */

    if (wb_info_ptr->has_tables || wb_info_ptr->has_clones)
    {
        uint32_t num_tables = 0;

        for (tmp_db = db->next_tab; tmp_db; tmp_db = tmp_db->next_tab)
        {
            num_tables++;
        }
        if (wb_fun->write_fn)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & num_tables, sizeof(num_tables), *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        else
        {
            *((uint32_t *) wb_fun->nv_ptr) = num_tables;
        }

        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_tables);
        if (wb_fun->nv_ptr)
            wb_fun->nv_ptr += sizeof(num_tables);

        if (db->next_tab)
            KAPS_STRY(kaps_db_wb_save_info(db->next_tab, wb_fun));
    }

    if (wb_fun->nv_ptr == NULL)
    {
        db->device->alloc->xfree(db->device->alloc->cookie, wb_info_ptr);
    }
    else
    {
        db->device->alloc->xfree(db->device->alloc->cookie, tmp);
    }
    return KAPS_OK;
}

kaps_status
kaps_db_wb_restore_info(
    struct kaps_device * device,
    struct kaps_db * db_p,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_db_wb_info *wb_info_ptr;
    struct kaps_db *db, *tmp;
    struct kaps_ad_db *ad_db;
    struct kaps_key *key;
    uint32_t tbl_no, num_tables = 0;
    struct kaps_device *target_device;
    int32_t total_size;
    struct kaps_hb_bank_list *head_hb_bank;

    if (wb_fun->nv_ptr == NULL)
    {
        struct kaps_db_wb_info wb_info;

        wb_info_ptr = &wb_info;
        /*
         * Need to read the WB info to determine the size
         * of description field
         */
        kaps_sassert(wb_fun->read_fn);
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
        total_size = sizeof(*wb_info_ptr) + wb_info_ptr->desc;
        wb_info_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, total_size);
        if (wb_info_ptr == NULL)
            return KAPS_OUT_OF_MEMORY;
    }
    else
    {
        wb_info_ptr = (struct kaps_db_wb_info *) wb_fun->nv_ptr;
        total_size = sizeof(*wb_info_ptr) + wb_info_ptr->desc;
    }

    if (wb_fun->read_fn != NULL)
    {
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, total_size, *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
    if (wb_fun->nv_ptr != NULL)
        wb_fun->nv_ptr += total_size;

    if (wb_info_ptr->smt_no == device->smt_no)
        target_device = device;
    else
        target_device = device->smt;

    if (wb_info_ptr->is_table)
        KAPS_STRY(kaps_db_wb_add_table(db_p, wb_info_ptr->tid, wb_info_ptr->stale_ptr, &db));
    else if (wb_info_ptr->is_clone)
    {
        KAPS_TRY(kaps_db_refresh_handle(device, (struct kaps_db *) wb_info_ptr->clone_of, &tmp));
        KAPS_STRY(kaps_db_wb_clone(tmp, wb_info_ptr->tid, wb_info_ptr->stale_ptr, &db));
    }
    else
        KAPS_STRY(kaps_db_wb_init(target_device, wb_info_ptr->type,
                                  wb_info_ptr->tid,
                                  wb_info_ptr->capacity,
                                  wb_info_ptr->max_capacity,
                                  wb_info_ptr->stale_ptr, &db,
                                  wb_info_ptr->is_bc_required && (!wb_info_ptr->is_main_bc_db)));

    db->stale_left_db_ptr = wb_info_ptr->stale_left_db_ptr;
    db->is_main_bc_db = wb_info_ptr->is_main_bc_db;
    db->is_replicated_internally = wb_info_ptr->is_replicated_internally;
    db->is_bc_required = wb_info_ptr->is_bc_required;
    db->bc_bitmap = wb_info_ptr->bc_bitmap;
    db->common_info->mapped_to_acl = wb_info_ptr->mapped_to_acl;
    db->common_info->is_em = wb_info_ptr->is_em;
    db->common_info->is_cascaded = wb_info_ptr->is_cascaded;
    db->common_info->locality = wb_info_ptr->locality;
    db->common_info->db_cascade_alternate_addition = wb_info_ptr->db_cascade_alternate_addition;
    db->num_algo_levels_in_db = wb_info_ptr->num_algo_levels_in_db;

    if (wb_info_ptr->num_ad_databases && !wb_info_ptr->is_table && !wb_info_ptr->is_clone)
    {
        uint32_t ad_iter;
        struct kaps_ad_db **ad_db_array = NULL;
        struct kaps_hb_bank_list **hb_head_bank_array = NULL;

        /*
         * the ad_db_array is an array that will hold pointers to the ad databases created inside this code block
         */
        ad_db_array = device->alloc->xcalloc(device->alloc->cookie,
                                             wb_info_ptr->num_ad_databases, sizeof(struct kaps_ad_db *));
        if (!ad_db_array)
            return KAPS_OUT_OF_MEMORY;

        /*
         * the hb_head_bank_array is an array that will hold pointers of the hb bank heads created inside this code
         * block. There will be a 1-1 mapping between the ad_db in ad_db_array and its corresponding hb bank head in
         * the hb_head_bank_array. We are having these arrays because we can restore the hb bank heads only after hb
         * database has been initialized
         */
        hb_head_bank_array = device->alloc->xcalloc(device->alloc->cookie,
                                                    wb_info_ptr->num_ad_databases, sizeof(struct kaps_hb_bank_list *));

        if (!hb_head_bank_array)
            return KAPS_OUT_OF_MEMORY;

        ad_iter = 0;
        while (ad_iter < wb_info_ptr->num_ad_databases)
        {
            struct kaps_ad_db_wb_info ad_wb_info, *ad_wb_info_ptr;

            if (wb_fun->nv_ptr == NULL)
            {
                ad_wb_info_ptr = &ad_wb_info;
            }
            else
            {
                ad_wb_info_ptr = (struct kaps_ad_db_wb_info *) wb_fun->nv_ptr;
            }

            if (wb_fun->read_fn != NULL)
            {
                if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) ad_wb_info_ptr,
                                         sizeof(*ad_wb_info_ptr), *wb_fun->nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;
            }
            *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*ad_wb_info_ptr);
            if (wb_fun->nv_ptr != NULL)
                wb_fun->nv_ptr += sizeof(*ad_wb_info_ptr);

            KAPS_STRY(kaps_ad_db_init(target_device, ad_wb_info_ptr->id,
                                      ad_wb_info_ptr->capacity, ad_wb_info_ptr->ad_width, &ad_db));

            ad_db->db_info.stale_ptr = (uintptr_t) ad_wb_info_ptr->stale_ptr;
            ad_db->db_info.hb_ad_stale_ptr = (uintptr_t) ad_wb_info_ptr->hb_ad_stale_ptr;

            if (ad_wb_info_ptr->user_specified)
                KAPS_STRY(kaps_db_set_resource(&ad_db->db_info, KAPS_HW_RESOURCE_UDA, ad_wb_info_ptr->uda_mb));

            if (wb_info_ptr->enable_dynamic_allocation)
            {
                kaps_memcpy(ad_db->db_info.hw_res.ad_res, &ad_wb_info_ptr->ad_hw_res,
                            sizeof(struct kaps_ad_db_hw_resource));
                kaps_memcpy(&ad_db->db_info.ad_res_internal, &ad_wb_info_ptr->ad_hw_res_internal,
                            sizeof(struct kaps_ad_db_hw_resource));
            }
            KAPS_STRY(kaps_db_set_ad(db, ad_db));

            head_hb_bank = NULL;

            /*
             * Store the ad_db pointer and head of hb bank. 
             */
            ad_db_array[ad_iter] = ad_db;
            hb_head_bank_array[ad_iter] = head_hb_bank;

            ad_iter++;
        }

        if (wb_info_ptr->has_hb)
        {
            struct kaps_hb_db *hb_db;

            KAPS_STRY(kaps_hb_db_init(target_device, wb_info_ptr->hb_id, wb_info_ptr->hb_capacity, &hb_db));
            hb_db->db_info.stale_ptr = (uintptr_t) wb_info_ptr->hb_stale_ptr;
            KAPS_STRY(kaps_db_set_hb(db, hb_db));
            hb_db->age_count = wb_info_ptr->hb_age_count;
        }

        if (ad_db_array)
            device->alloc->xfree(device->alloc->cookie, ad_db_array);

        if (hb_head_bank_array)
            device->alloc->xfree(device->alloc->cookie, hb_head_bank_array);
    }

    if (wb_info_ptr->desc)
    {
        KAPS_STRY(kaps_db_set_property(db, KAPS_PROP_DESCRIPTION, wb_info_ptr->description));
    }

    if (wb_info_ptr->defer_deletes)
    {
        KAPS_STRY(kaps_db_set_property(db, KAPS_PROP_DEFER_DELETES, wb_info_ptr->defer_deletes));
    }

    if (db->is_bc_required)
    {
        if (db->is_main_bc_db)
            db->main_bc_db = db;
        else
        {
            struct kaps_device *tmpdev;
            struct kaps_db *tmp_db;

            tmpdev = db->device->main_bc_device;
            while (tmpdev)
            {
                tmp_db = kaps_db_get_bc_db_on_device(tmpdev, db);
                tmpdev = tmpdev->next_bc_device;
                db->main_bc_db = tmp_db;
                if (tmp_db)
                    break;
            }
        }
    }

    KAPS_STRY(kaps_db_set_property(db, KAPS_PROP_ALGORITHMIC, wb_info_ptr->is_algorithmic));
    if (db->type == KAPS_DB_ACL && kaps_db_get_algorithmic(db))
    {
        KAPS_STRY(kaps_db_set_property(db, KAPS_PROP_ALGORITHMIC, wb_info_ptr->alg_type));
    }

    if (wb_info_ptr->is_user_specified_algo_type)
        kaps_resource_set_user_specified_algo_mode(db);

    if (!wb_info_ptr->is_table && !wb_info_ptr->is_clone && db->type == KAPS_DB_ACL)
        db->fn_table->restore_property(db, wb_fun);

    KAPS_STRY(kaps_key_init(target_device, &key));
    KAPS_STRY(kaps_key_wb_restore(key, wb_fun));
    KAPS_STRY(kaps_db_set_key(db, key));

    if (wb_info_ptr->is_table || wb_info_ptr->is_clone)
    {
        if (wb_fun->nv_ptr == NULL)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, wb_info_ptr);
        }
        return KAPS_OK;
    }

    KAPS_STRY(kaps_db_set_property(db, KAPS_PROP_ALGORITHMIC, wb_info_ptr->is_algorithmic));

    db->highest_wide_ab_index = wb_info_ptr->highest_wide_ab_index;
    db->lowest_narrow_ab_index = wb_info_ptr->lowest_narrow_ab_index;

    if (db->type == KAPS_DB_ACL && kaps_db_get_algorithmic(db))
    {
        KAPS_STRY(kaps_db_set_property(db, KAPS_PROP_ALGORITHMIC, wb_info_ptr->alg_type));
    }

    if (db->common_info->multicast_db)
    {
        KAPS_STRY(kaps_db_set_property(db, 95, 1));
        KAPS_STRY(kaps_db_set_property(db, 90, 1));
    }

    if (wb_info_ptr->has_tables || wb_info_ptr->has_clones)
    {
        if (wb_fun->read_fn)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & num_tables, sizeof(num_tables), *wb_fun->nv_offset))
                return KAPS_NV_READ_WRITE_FAILED;
        }
        else
        {
            kaps_sassert(wb_fun->nv_ptr != NULL);
            num_tables = *((uint32_t *) wb_fun->nv_ptr);
        }

        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_tables);
        if (wb_fun->nv_ptr)
            wb_fun->nv_ptr += sizeof(num_tables);

        for (tbl_no = 0; tbl_no < num_tables; tbl_no++)
        {
            KAPS_STRY(kaps_db_wb_restore_info(target_device, db, wb_fun));
        }
    }

    if (wb_info_ptr->user_specified)
    {
        KAPS_TRY(kaps_db_set_resource(db, KAPS_HW_RESOURCE_DBA, wb_info_ptr->user_num_ab));
        KAPS_TRY(kaps_db_set_resource(db, KAPS_HW_RESOURCE_UDA, wb_info_ptr->user_uda_mb));
    }

    if (wb_info_ptr->user_specified_range && db->type == KAPS_DB_ACL)
    {
        KAPS_TRY(kaps_db_set_resource(db, KAPS_HW_RESOURCE_RANGE_UNITS, wb_info_ptr->range_units));
    }

    if (wb_info_ptr->custom_ix_range)
    {
        KAPS_TRY(kaps_db_set_property
                 (db, KAPS_PROP_INDEX_RANGE, wb_info_ptr->index_range_min, wb_info_ptr->index_range_max));
    }

    if (wb_info_ptr->callback_data)
    {
        KAPS_TRY(kaps_db_set_property
                 (db, KAPS_PROP_INDEX_CALLBACK, wb_info_ptr->callback_fn, wb_info_ptr->callback_handle));
    }

    if (wb_info_ptr->cascade_override)
    {
        KAPS_TRY(kaps_db_set_property(db, KAPS_PROP_CASCADE_DEVICE, wb_info_ptr->cascade_device));
    }

    if (wb_info_ptr->enable_dynamic_allocation)
    {
        db->common_info->enable_dynamic_allocation = 1;
    }

    if (wb_info_ptr->enable_db_compaction)
    {
        db->common_info->enable_db_compaction = 1;
    }

    db->common_info->is_algo = wb_info_ptr->is_algo;
    db->common_info->calc_shuffle_stats = wb_info_ptr->calc_shuffle_stats;
    db->common_info->meta_priority = wb_info_ptr->meta_priority;
    if (db->type == KAPS_DB_ACL)
    {
        db->common_info->priority_pattern = wb_info_ptr->priority_pattern;
        db->common_info->prev_entry_priority = wb_info_ptr->prev_entry_priority;
    }

    if (wb_info_ptr->calc_shuffle_stats)
    {
        db->common_info->pio_stats.num_blk_ops = wb_info_ptr->pio_stats.num_blk_ops;
        db->common_info->pio_stats.num_ix_cbs = wb_info_ptr->pio_stats.num_ix_cbs;
        db->common_info->pio_stats.num_of_piords = wb_info_ptr->pio_stats.num_of_piords;
        db->common_info->pio_stats.num_of_piowrs = wb_info_ptr->pio_stats.num_of_piowrs;
        db->common_info->pio_stats.num_of_worst_case_shuffles = wb_info_ptr->pio_stats.num_of_worst_case_shuffles;
        db->common_info->pio_stats.num_of_shuffles = wb_info_ptr->pio_stats.num_of_shuffles;
        db->common_info->pio_stats.num_ad_writes = wb_info_ptr->pio_stats.num_ad_writes;
    }

    db->has_dup_ad = wb_info_ptr->has_dup_ad;

    db->common_info->insertion_mode = wb_info_ptr->insertion_mode;
    db->relative_entry = (struct kaps_entry *) ((uintptr_t) wb_info_ptr->relative_entry_wb_handle);
    db->relative_entry_position = wb_info_ptr->relative_entry_position;

    if (wb_fun->nv_ptr == NULL)
    {
        db->device->alloc->xfree(db->device->alloc->cookie, wb_info_ptr);
    }

    return KAPS_OK;
}

kaps_status
kaps_db_reconcile_start(
    struct kaps_db * db)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;
    struct kaps_entry *entry;
    struct kaps_db *tmp;

    if (!db)
        return KAPS_INVALID_ARGUMENT;

    for (tmp = db; tmp; tmp = tmp->next_tab)
    {
        if (tmp->is_clone)
            continue;
        kaps_c_list_iter_init(&tmp->db_list, &it);

        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            entry = KAPS_DBLIST_TO_KAPS_ENTRY(e1);
            KAPS_STRY(db->fn_table->reset_used(entry));
        }
    }
    return KAPS_OK;
}

kaps_status
kaps_db_reconcile_end(
    struct kaps_db * db)
{
    if (db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (db->is_clone)
        return KAPS_INVALID_ARGUMENT;

    return db->fn_table->reconcile_end(db);
}

uint32_t
kaps_db_entry_get_nv_offset(
    struct kaps_db * db,
    uint32_t user_handle)
{
    uint8_t found;
    uint8_t *entry_nv_ptr;

    kaps_cr_pool_mgr_get_entry_nv_ptr(db->device->nv_mem_mgr->kaps_cr_pool_mgr, db, user_handle, &found, &entry_nv_ptr);
    kaps_sassert(found);
    kaps_sassert(entry_nv_ptr);

    return (entry_nv_ptr - ((uint8_t *) db->device->nv_ptr));
}
