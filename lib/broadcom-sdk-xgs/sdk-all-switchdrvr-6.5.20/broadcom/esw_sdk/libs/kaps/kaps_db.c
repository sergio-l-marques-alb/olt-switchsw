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
kaps_db_init_internal(
    struct kaps_device *device,
    enum kaps_db_type type,
    uint32_t id,
    uint32_t capacity,
    struct kaps_db **dbp,
    int32_t is_broadcast)
{
    struct kaps_device *main_device;
    kaps_status status = KAPS_OK;

    if (!dbp)
        status = KAPS_INVALID_ARGUMENT;

    else if (!device)
        status = KAPS_INVALID_DEVICE_PTR;

    else if (device->is_config_locked)
        status = KAPS_DEVICE_ALREADY_LOCKED;

    else if ((device->flags & KAPS_DEVICE_ISSU) && (device->issu_status != KAPS_ISSU_INIT))
        status = KAPS_ISSU_IN_PROGRESS;


    if (status == KAPS_OK)
    {
        if ((device->type == KAPS_DEVICE_KAPS) && (!is_broadcast))
        {
            if (device->main_dev)
            {
                device = device->main_dev;
            }
        }

        switch (type)
        {
            case KAPS_DB_LPM:
                status = kaps_lpm_db_init(device, id, capacity, dbp);
                break;
            default:
                status = KAPS_INVALID_ARGUMENT;
                break;
        }
    }

    if (status == KAPS_OK)
    {
        if (device->type == KAPS_DEVICE_KAPS && capacity > 256 * 1024)
            capacity = 256 * 1024;

        (*dbp)->common_info->max_capacity = -1;
        (*dbp)->highest_wide_ab_index = -1;
        (*dbp)->lowest_narrow_ab_index = (*dbp)->device->num_ab;

        (*dbp)->common_info->enable_dynamic_allocation = 1;


        main_device = device;
        if (main_device->main_dev)
            main_device = main_device->main_dev;

        if (main_device->next_dev || (main_device->type == KAPS_DEVICE_KAPS))
            (*dbp)->common_info->enable_dynamic_allocation = 0;

        if (!is_broadcast)
        {
            KAPS_WB_HANDLE_TABLE_INIT((*dbp), capacity);
            KAPS_WB_HANDLE_TABLE_FREELIST_INIT(*dbp);
        }

        (*dbp)->bc_bitmap = 1 << device->bc_id;
    }

    return status;
}

kaps_status
kaps_db_init(
    struct kaps_device * device,
    enum kaps_db_type type,
    uint32_t id,
    uint32_t capacity,
    struct kaps_db ** dbp)
{
    kaps_status status = KAPS_OK;
    KAPS_TRACE_IN("%p %u %u %u %p\n", device, type, id, capacity, dbp);
    status = kaps_db_init_internal(device, type, id, capacity, dbp, 0);
    KAPS_TRACE_OUT("%p\n", (dbp ? *dbp : 0));
    return status;
}

struct kaps_db *
kaps_db_get_bc_db_on_device(
    struct kaps_device *device,
    struct kaps_db *inp_db)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e;
    struct kaps_db *ret_db = NULL;

/*  KAPS_TRACE_IN("%p %p\n", device, inp_db);*/
    if (device == inp_db->device || inp_db->common_info->is_cascaded)
    {
        ret_db = inp_db;
    }
    else
    {

        kaps_c_list_iter_init(&device->db_list, &it);
        while ((e = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(e);

            if (db->type != KAPS_DB_EM && db->type != KAPS_DB_ACL && db->type != KAPS_DB_LPM)
                continue;

            if ((inp_db->tid == db->tid) &&
                (inp_db->type == db->type) &&
                (!db->parent) && (inp_db->common_info->mapped_to_acl == db->common_info->mapped_to_acl))
            {
                ret_db = db;
                break;
            }
        }
    }

/*  KAPS_TRACE_OUT("%p\n", ret_db);*/
    return ret_db;
}

kaps_status
kaps_db_create_broadcast(
    struct kaps_device * device,
    struct kaps_db * ori_db,
    struct kaps_db ** bc_db)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p %p\n", device, ori_db, bc_db);

    if (device->bc_id <= ori_db->device->bc_id)
    {
        status = KAPS_DB_BROADCAST_AT_LOWER_INDEX_DEV;
    }

    if (status == KAPS_OK)
    {
        /*
         * Create Broadcast DB on broadcast device 
         */
        status = kaps_db_init_internal(device, ori_db->type, ori_db->tid, ori_db->common_info->capacity, bc_db, 1);
    }

    if (status == KAPS_OK)
    {
        /*
         * Create BC bitmap on main DB 
         */
        ori_db->bc_bitmap |= (1 << device->bc_id);
        ori_db->is_bc_required = 1;
        (*bc_db)->is_bc_required = 1;
        (*bc_db)->main_bc_db = ori_db;
        ori_db->is_main_bc_db = 1;

        switch (ori_db->type)
        {
            case KAPS_DB_LPM:
                status = kaps_lpm_db_create_broadcast(ori_db, *bc_db);
                break;
            default:
                status = KAPS_INVALID_ARGUMENT;
                break;
        }
    }

    KAPS_TRACE_OUT("%p\n", (bc_db ? (*bc_db) : 0));

    return status;
}

kaps_status
kaps_db_destroy_single_device(
    struct kaps_db * db)
{
    kaps_status status = KAPS_OK;

/*  KAPS_TRACE_IN("%p\n", db);*/

    if (db == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT)
                status = KAPS_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        if (!db->fn_table || !db->fn_table->db_destroy)
            status = KAPS_INTERNAL_ERROR;
    }

    if (status == KAPS_OK)
    {
        if (db->device && db->device->is_wb_continue)
        {
            struct kaps_device *tmp_device = db->device;

            db->device->issu_status = KAPS_ISSU_INIT;
            tmp_device = db->device;
            while (tmp_device)
            {
                tmp_device->issu_in_progress = NlmTrue;
                if (tmp_device->smt)
                    tmp_device->smt->issu_in_progress = NlmTrue;
                tmp_device = tmp_device->next_dev;
            }
        }

        if (db->description && db->device)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, db->description);
            db->description = NULL;
        }

        status = db->fn_table->db_destroy(db);
    }

    return status;
}

kaps_status
kaps_db_destroy(
    struct kaps_db * db)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p\n", db);

    if (db == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        if (db->is_replicated_internally)
        {
            struct kaps_db *tmp_db;
            tmp_db = kaps_db_get_bc_db_on_device(db->device->other_core, db);
            status = kaps_db_destroy_single_device(tmp_db);
        }
    }

    if (status == KAPS_OK)
    {
        if (db->common_info->is_cascaded && 0)
        {
            struct kaps_db *tmp_db;
            tmp_db = kaps_db_get_bc_db_on_device(db->device->other_core, db);
            status = kaps_db_destroy_single_device(tmp_db);
        }
    }

    if (status == KAPS_OK)
        status = kaps_db_destroy_single_device(db);

    return status;
}

kaps_status
kaps_db_set_resource(
    struct kaps_db * db,
    enum kaps_hw_resource resource,
    ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;
    uint32_t value, limit, core_id;
    struct kaps_db *algo_db = db;

    if (db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        if (db->device->is_config_locked)
            status = KAPS_DB_ACTIVE;
    }

    if (db->type == KAPS_DB_AD)
        algo_db = db->common_info->ad_info.db;

    /*
     * We are using this API internally to redo the set_resource during restore, so we are allowing during restore
     * state 
     */
    if (status == KAPS_OK)
    {
        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT && db->device->issu_status != KAPS_ISSU_RESTORE_START)
                status = KAPS_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_OK)
    {
        va_start(vl, resource);
        value = va_arg(vl, uint32_t);
        KAPS_TRACE_IN("%p %u %u\n", db, resource, value);
        switch (resource)
        {
            case KAPS_HW_RESOURCE_DBA:
                if (value > db->device->num_ab)
                {
                    status = KAPS_INVALID_RESOURCE_DBA;
                    break;
                }
                db->common_info->user_num_ab = value;
                db->common_info->num_ab = value;
                db->common_info->user_specified = 1;
                break;

            case KAPS_HW_RESOURCE_DBA_CORE:
                if (value > db->device->num_ab)
                {
                    status = KAPS_INVALID_RESOURCE_DBA;
                    break;
                }
                if (!algo_db->common_info->is_xl_db)
                {
                    status = KAPS_UNSUPPORTED;
                    break;
                }

                core_id = va_arg(vl, uint32_t);
                if (core_id != 0 && core_id != 1)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }

                KAPS_TRACE_IN("%p %u %u %u\n", db, resource, value, core_id);
                db->common_info->num_ab_core[core_id] = value;
                db->common_info->num_ab = db->common_info->num_ab_core[0] + db->common_info->num_ab_core[1];
                db->common_info->user_num_ab = db->common_info->num_ab;
                db->common_info->user_specified = 1;
                break;

            case KAPS_HW_RESOURCE_UDA:
                limit = 15;

                if (value > limit)
                {
                    status = KAPS_INVALID_RESOURCE_UDA;
                    break;
                }
                db->common_info->user_uda_mb = value;
                db->common_info->uda_mb = value;
                db->common_info->user_specified = 1;
                break;

            case KAPS_HW_RESOURCE_UDA_CORE:
                if (value > 512)
                {
                    status = KAPS_INVALID_RESOURCE_UDA;
                    break;
                }
                if (!algo_db->common_info->is_xl_db)
                {
                    status = KAPS_UNSUPPORTED;
                    break;
                }

                core_id = va_arg(vl, uint32_t);
                if (core_id != 0 && core_id != 1)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }

                KAPS_TRACE_IN("%p %u %u %u\n", db, resource, value, core_id);

                db->common_info->uda_mb_core[core_id] = value;
                db->common_info->uda_mb = db->common_info->uda_mb_core[0] + db->common_info->uda_mb_core[1];
                db->common_info->user_uda_mb = db->common_info->uda_mb;
                db->common_info->user_specified = 1;
                break;

            case KAPS_HW_RESOURCE_RANGE_UNITS:
                if (value > db->device->num_mcor)
                {
                    status = KAPS_INVALID_RESOURCE_RANGE_UNITS;
                    break;
                }
                db->common_info->range_units = value;
                db->common_info->user_specified_range = 1;
                break;

            default:
                status = KAPS_INVALID_ARGUMENT;
                break;
        }

        va_end(vl);
    }

    return status;
}

kaps_status
kaps_db_get_resource(
    struct kaps_db * db,
    enum kaps_hw_resource resource,
    ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;
    uint32_t *value;

    if (db == NULL)
        return KAPS_INVALID_ARGUMENT;

    KAPS_TX_ERROR_CHECK(db->device, 0);

    KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

    if (db->device->flags & KAPS_DEVICE_ISSU)
    {
        if (db->device->issu_status != KAPS_ISSU_INIT)
            status = KAPS_ISSU_IN_PROGRESS;
    }

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        va_start(vl, resource);
        value = va_arg(vl, uint32_t *);
        KAPS_TRACE_IN("%p %u %p\n", db, resource, value);
        if (!value)
        {
            va_end(vl);
            status = KAPS_INVALID_ARGUMENT;
        }
    }

    if (status == KAPS_OK)
    {
        switch (resource)
        {
            case KAPS_HW_RESOURCE_DBA:
            {
                uint32_t num_ab = 0;
                struct kaps_device *cascade;
                struct kaps_db *res_db;

                res_db = kaps_resource_get_res_db(db);

                cascade = db->device;
                if (cascade->main_dev)
                    cascade = cascade->main_dev;

                while (cascade)
                {
                    num_ab += kaps_resource_get_ab_usage(cascade, res_db);
                    cascade = cascade->next_dev;
                }

                *value = num_ab;
                break;
            }

            case KAPS_HW_RESOURCE_UDA:
                *value = db->common_info->uda_mb;
                break;

            default:
                status = KAPS_INVALID_ARGUMENT;
                break;
        }

        va_end(vl);
    }

    KAPS_TX_ERROR_CHECK(db->device, status);

    return status;
}

kaps_status
kaps_db_set_key(
    struct kaps_db * db,
    struct kaps_key * key)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p\n", db, key);
    if (db == NULL || key == NULL)
        status = KAPS_INVALID_ARGUMENT;

    else if (db->device->is_config_locked)
        status = KAPS_DB_ACTIVE;

    else if (db->key)
        status = KAPS_DUPLICATE_KEY;

    else
    {
        if (db->device->type == KAPS_DEVICE_KAPS)
        {
            if (key->width_1 > 160)
                status = KAPS_INVALID_KEY_WIDTH;
        }
        else
        {
            if (key->width_1 > KAPS_HW_MAX_DBA_WIDTH_1)
                status = KAPS_INVALID_KEY_WIDTH;
        }
    }

    if (status == KAPS_OK)
    {
        if (key->first_field == NULL)
            status = KAPS_MISSING_KEY_FIELD;

        else if (key->has_dummy_fill_field)
            status = KAPS_DUMMY_FILL_FIELD_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->db_set_key)
            status = KAPS_INTERNAL_ERROR;

        else
        {       /* nothing */
        }
    }

    if (status == KAPS_OK)
    {
        if (key->ref_count != 0)
        {
            /*
             * Another database or instruction is pointing to the
             * same key, we need to create a copy of it
             */
            struct kaps_key *new_key;
            status = kaps_key_clone(db->device, key, &new_key);
            if (status == KAPS_OK)
                key = new_key;
        }
    }

    if (status == KAPS_OK)
    {
        key->ref_count++;

        status = kaps_key_validate_internal(db, key);
    }

    if (status == KAPS_OK)
    {
        status = db->fn_table->db_set_key(db, key);
    }

    return status;
}

kaps_status
kaps_db_pair_with_db(
    struct kaps_db * left_db,
    struct kaps_db * right_db)
{
    struct kaps_device *main_dev, *tmp_dev;
    struct kaps_db *tmp_left_db, *tmp_right_db;
    uint32_t bc_bmp;
    kaps_status status = KAPS_OK;

/*  KAPS_TRACE_IN("%p %p\n", left_db, right_db);*/

    /*
     * 1M+1M and 2M feature can not be set on the same database 
     */
    if (left_db->place_on_which_half != KAPS_DB_PLACE_ON_BOTH_HALVES
        || right_db->place_on_which_half != KAPS_DB_PLACE_ON_BOTH_HALVES)
    {
        status = KAPS_DB_PAIRING_SCALE_UP_NOT_POSSIBLE;
    }

    if (status == KAPS_OK)
    {

        left_db->paired_db = right_db;
        right_db->paired_db = left_db;
        right_db->bc_bitmap = left_db->bc_bitmap;
        left_db->place_on_which_half = KAPS_DB_PLACE_ON_LEFT_HALF;
        right_db->place_on_which_half = KAPS_DB_PLACE_ON_RIGHT_HALF;

        if (left_db->is_bc_required && (!left_db->device->issu_in_progress))
        {
            bc_bmp = left_db->bc_bitmap;
            main_dev = left_db->device;
            if (main_dev->main_bc_device)
                main_dev = main_dev->main_bc_device;

            for (tmp_dev = main_dev; tmp_dev; tmp_dev = tmp_dev->next_bc_device)
            {
                if (bc_bmp & (1 << tmp_dev->bc_id))
                {
                    tmp_left_db = kaps_db_get_bc_db_on_device(tmp_dev, left_db);
                    tmp_right_db = kaps_db_get_bc_db_on_device(tmp_dev, right_db);
                    kaps_sassert(tmp_left_db);
                    kaps_sassert(tmp_right_db);

                    tmp_left_db->paired_db = tmp_right_db;
                    tmp_right_db->paired_db = tmp_left_db;
                    tmp_right_db->bc_bitmap = tmp_left_db->bc_bitmap;
                    tmp_left_db->place_on_which_half = KAPS_DB_PLACE_ON_LEFT_HALF;
                    tmp_right_db->place_on_which_half = KAPS_DB_PLACE_ON_RIGHT_HALF;
                }
            }
        }
    }

    return status;
}

kaps_status
kaps_db_set_property(
    struct kaps_db * db,
    enum kaps_db_properties property,
    ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;
    int32_t value;

    if (db == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        va_start(vl, property);

        /*
         * We are using this API internally to redo the set_property during restore and also we are allowing only
         * KAPS_PROP_INDEX_CALLBACK property after restore 
         */
        if (db->device->flags & KAPS_DEVICE_ISSU && (property != KAPS_PROP_INDEX_CALLBACK))
        {

            if (db->device->issu_status != KAPS_ISSU_INIT && db->device->issu_status != KAPS_ISSU_RESTORE_START)
            {
                va_end(vl);
                status = KAPS_ISSU_IN_PROGRESS;
            }
        }
    }

    if (status == KAPS_OK)
    {
        switch (property)
        {
            case KAPS_PROP_INDEX_RANGE:
            {
                if (db->device->is_config_locked)
                {
                    status = KAPS_DB_ACTIVE;
                    break;
                }
                if (db->device->type == KAPS_DEVICE_KAPS)
                    status = KAPS_INVALID_ARGUMENT;
                else
                {
                    int32_t min, max;
                    enum kaps_ad_type ad_type;

                    min = va_arg(vl, int32_t);
                    max = va_arg(vl, int32_t);
                    KAPS_TRACE_IN("%p %u %d %d\n", db, property, min, max);
                    if ((min < 0 || max < 0) || (max == 0) || (min > max))
                    {
                        status = KAPS_INVALID_ARGUMENT;
                        break;
                    }

                    ad_type = kaps_resource_get_ad_type(db);
                    if (ad_type != KAPS_AD_TYPE_NONE)
                    {
                        /*
                         * cannot support AD with custom index range
                         */
                        status = KAPS_CUSTOM_RANGE_WITH_AD;
                        break;
                    }

                    kaps_resource_set_ad_type(db, KAPS_AD_TYPE_CUSTOM);
                    db->common_info->index_range_min = min;
                    db->common_info->index_range_max = max;
                    db->common_info->custom_index = 1;
                }
                break;
            }

            case KAPS_PROP_DESCRIPTION:
            {
                if (db->device->is_config_locked)
                    status = KAPS_DB_ACTIVE;
                else
                {
                    char *desc = va_arg(vl, char *);
                    KAPS_TRACE_IN("%p %u %s\n", db, property, desc);
                    if (!desc)
                    {
                        status = KAPS_INVALID_DEVICE_DESC_PTR;
                        break;
                    }

                    if (!db->description) 
                    {
                        db->description = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, (strlen(desc) + 1));
                        if (!db->description)
                        {
                            status = KAPS_OUT_OF_MEMORY;
                            break;
                        }
                        strcpy(db->description, desc);
                    }
                }
                break;
            }

            case KAPS_PROP_INDEX_CALLBACK:
            {
                if (db->device->is_config_locked)
                {
                    status = KAPS_DB_ACTIVE;
                    break;
                }

                db->common_info->callback_fn = va_arg(vl, kaps_db_index_callback);
                db->common_info->callback_handle = va_arg(vl, void *);
                KAPS_TRACE_IN("%p %u %p %p\n", db, property, db->common_info->callback_fn,
                              db->common_info->callback_handle);
                if (!db->common_info->callback_fn)
                    status = KAPS_INVALID_ARGUMENT;

                break;
            }

            case KAPS_PROP_MIN_PRIORITY:
            {
                if (db->device->is_config_locked)
                    status = KAPS_DB_ACTIVE;
                else
                {
                    value = va_arg(vl, int32_t);
                    KAPS_TRACE_IN("%p %u %d\n", db, property, value);
                    if (db->type != KAPS_DB_ACL)
                    {
                        status = KAPS_INVALID_ARGUMENT;
                        break;
                    }

                    if (value > KAPS_HW_MINIMUM_PRIORITY)
                    {
                        status = KAPS_INVALID_ARGUMENT;
                        break;
                    }
                    if (db->common_info->estimated_max_priority && value <= db->common_info->estimated_max_priority)
                    {
                        status = KAPS_INVALID_ARGUMENT;
                        break;
                    }
                    db->common_info->estimated_min_priority = value;
                    db->common_info->estimated_min_priority_user_specified_value = value;
                }
                break;
            }

            case KAPS_PROP_MAX_PRIORITY:
            {
                if (db->device->is_config_locked)
                    status = KAPS_DB_ACTIVE;
                else
                {
                    value = va_arg(vl, int32_t);
                    KAPS_TRACE_IN("%p %u %d\n", db, property, value);
                    if (db->type != KAPS_DB_ACL)
                    {
                        status = KAPS_INVALID_ARGUMENT;
                        break;
                    }

                    if (value > KAPS_HW_MINIMUM_PRIORITY || value < 0)
                    {
                        status = KAPS_INVALID_ARGUMENT;
                        break;
                    }
                    db->common_info->estimated_max_priority = value;
                    db->common_info->estimated_max_priority_user_specified_value = value;
                }
                break;
            }

            case KAPS_PROP_ALGORITHMIC:
            {
                if (db->device->is_config_locked)
                    status = KAPS_DB_ACTIVE;
                else
                {
                    value = va_arg(vl, int32_t);
                    KAPS_TRACE_IN("%p %u %d\n", db, property, value);
                    if (db->device->is_config_locked || db->ninstructions)
                    {
                        status = KAPS_INVALID_ARGUMENT;
                        break;
                    }
                    status = kaps_resource_set_algorithmic(db->device, db, value);
                    db->common_info->is_algo = 0;
                    if (!db->device->issu_in_progress)
                        kaps_resource_set_user_specified_algo_mode(db);
                    if (!db->device->issu_in_progress && status == KAPS_OK && value)
                    {
                        db->common_info->is_algo = 1;
                    }
                }
                break;
            }

            case KAPS_PROP_ENABLE_DB_COMPACTION:
                value = va_arg(vl, int32_t);
                KAPS_TRACE_IN("%p %u %d\n", db, property, value);
                if (value != 0 && value != 1)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                else
                    db->common_info->enable_db_compaction = value;
                break;

            case KAPS_PROP_MAX_CAPACITY:
                value = va_arg(vl, int32_t);
                KAPS_TRACE_IN("%p %u %d\n", db, property, value);
                db->common_info->max_capacity = value;
                break;

            case KAPS_PROP_DEFER_DELETES:
            {
                if (db->device->is_config_locked)
                    status = KAPS_DB_ACTIVE;
                else
                {
                    value = va_arg(vl, uint32_t);
                    KAPS_TRACE_IN("%p %u %u\n", db, property, value);
                    if (value < 0 || value > 1)
                        status = KAPS_INVALID_ARGUMENT;
                    else
                    {
                        struct kaps_allocator *alloc = db->device->alloc;

                        if (db->common_info->defer_deletes_to_install && value)
                            break;

                        db->common_info->defer_deletes_to_install = value;
                        if (value)
                        {
                            if (db->common_info->capacity)
                            {
                                db->common_info->max_pending_del_entries = db->common_info->capacity;
                            }
                            else
                            {
                                db->common_info->max_pending_del_entries = 4 * 1024;
                            }

                            if (db->common_info->del_entry_list == NULL)
                            {
                                db->common_info->del_entry_list = alloc->xcalloc(alloc->cookie,
                                                                                 db->
                                                                                 common_info->max_pending_del_entries,
                                                                                 sizeof(struct kaps_entry *));
                                if (!db->common_info->del_entry_list)
                                {
                                    status = KAPS_OUT_OF_MEMORY;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            if (db->common_info->del_entry_list)
                            {
                                alloc->xfree(alloc->cookie, db->common_info->del_entry_list);
                                db->common_info->del_entry_list = NULL;
                            }
                        }
                        db->common_info->num_pending_del_entries = 0;
                    }
                }
                break;
            }

            case KAPS_PROP_LOG:
            {
                FILE *fp = va_arg(vl, FILE *);
                KAPS_TRACE_IN("%p %u %p\n", db, property, fp);

                /*
                 * We don't check for is_config_locked and fp for NULL so that logging can be enabled/disabled
                 * dynamically. 
                 */
                db->common_info->fp = fp;
                break;
            }

            case KAPS_PROP_ENABLE_DYNAMIC_ALLOCATION:
            {
                value = va_arg(vl, int32_t);
                KAPS_TRACE_IN("%p %u %d\n", db, property, value);
                if (db->device->is_config_locked || (value != 0 && value != 1))
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                else
                    db->common_info->enable_dynamic_allocation = value;

                break;
            }
            case KAPS_PROP_UTILISE_UDA_HOLES:
            {
                value = va_arg(vl, int32_t);
                KAPS_TRACE_IN("%p %u %d\n", db, property, value);
                if (db->device->is_config_locked || (value != 0 && value != 1))
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                else
                    db->common_info->utilise_uda_holes = value;

                break;
            }
            default:
                /*
                 * Properties that need to be handled specifically
                 * by the individual database
                 */
                KAPS_TRACE_IN("%p %u\n", db, property);
                if (!db->fn_table)
                    status = KAPS_INTERNAL_ERROR;
                if (status == KAPS_OK && !db->fn_table->db_set_property)
                    status = KAPS_INVALID_ARGUMENT;
                if (status == KAPS_OK)
                    status = db->fn_table->db_set_property(db, property, vl);
                break;
        }

        va_end(vl);
    }

    KAPS_TRACE_PRINT("%s\n", " ");
    return status;
}

kaps_status
kaps_db_get_property(
    struct kaps_db * db,
    enum kaps_db_properties property,
    ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;
    int32_t *out, *out1;
    char **desc;

    KAPS_TRACE_IN("%p %u\n", db, property);

    if (db == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT)
                status = KAPS_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        va_start(vl, property);

        switch (property)
        {
            case KAPS_PROP_MIN_PRIORITY:
                if (db->type != KAPS_DB_ACL)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }

                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                *out = db->common_info->estimated_min_priority;
                break;

            case KAPS_PROP_ALGORITHMIC:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }

                *out = kaps_db_get_algorithmic(db);

                break;

            case KAPS_PROP_INDEX_RANGE:
                
                if (db->device->type == KAPS_DEVICE_KAPS)
                    status = KAPS_INVALID_ARGUMENT;
                else
                {
                    out = va_arg(vl, int32_t *);
                    out1 = va_arg(vl, int32_t *);
                    if (!out || !out1)
                    {
                        status = KAPS_INVALID_ARGUMENT;
                        break;
                    }
                    *out = db->common_info->index_range_min;
                    *out1 = db->common_info->index_range_max;
                }
                break;

            case KAPS_PROP_DESCRIPTION:
                desc = va_arg(vl, char **);
                if (!desc)
                {
                    status = KAPS_INVALID_DEVICE_DESC_PTR;
                    break;
                }
                *desc = db->description;
                break;

            case KAPS_PROP_INDEX_CALLBACK:
            {
                kaps_db_index_callback *callback;
                void **handle;

                callback = va_arg(vl, kaps_db_index_callback *);
                handle = va_arg(vl, void **);
                if (!callback || !handle)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                *callback = db->common_info->callback_fn;
                *handle = db->common_info->callback_handle;
                break;
            }

            case KAPS_PROP_DEFER_DELETES:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }

                *out = db->common_info->defer_deletes_to_install;
                break;

            case KAPS_PROP_CASCADE_DEVICE:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                *out = db->device->device_no;
                break;

            case KAPS_PROP_ENTRY_META_PRIORITY:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                *out = db->common_info->meta_priority;
                break;
            case KAPS_PROP_MC_DB:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                *out = db->common_info->multicast_db;
                break;

             case KAPS_PROP_NUM_LEVELS_IN_DB:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_INVALID_ARGUMENT;
                    break;
                }
                *out = db->num_algo_levels_in_db;
                break;

            default:
                /*
                 * Properties that need to be handled specifically
                 * by the individual database
                 */

                if (!db->fn_table)
                    status = KAPS_INTERNAL_ERROR;
                if (status == KAPS_OK && !db->fn_table->db_get_property)
                    status = KAPS_INVALID_ARGUMENT;
                if (status == KAPS_OK)
                    status = db->fn_table->db_get_property(db, property, vl);
                break;
        }

        va_end(vl);

        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

kaps_status
kaps_db_print(
    struct kaps_db * db,
    FILE * fp)
{
    kaps_status status = KAPS_OK;
    struct kaps_db *next_tab;
    struct kaps_db_stats stats;

    KAPS_TRACE_IN("%p %p\n", db, fp);

    if (!db || !fp)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->parent)
            db = db->parent;

        kaps_fprintf(fp, "----------------------------------------------------------\n");

        kaps_fprintf(fp, "DB ID:%d TYPE:%s Capacity:%d BMR NO: ",
                     db->tid, kaps_device_db_name(db), db->common_info->capacity);

        if (db->is_bmr_assigned)
            kaps_fprintf(fp, "%d\n", db->bmr_no);
        else
            kaps_fprintf(fp, "none\n");

        if (db->key)
        {
            status = kaps_key_print(db->key, fp);
            KAPS_TX_ERROR_CHECK(db->device, status);
        }
    }

    if (status == KAPS_OK)
    {
        for (next_tab = db->next_tab; next_tab; next_tab = next_tab->next_tab)
        {
            struct kaps_db *tmp;

            if (next_tab->is_clone)
                continue;

            kaps_fprintf(fp, "|  \n|-- Table ID:%d TYPE:%s BMR NO: ", next_tab->tid, kaps_device_db_name(next_tab));

            if (next_tab->is_bmr_assigned)
                kaps_fprintf(fp, "%d\n", next_tab->bmr_no);
            else
                kaps_fprintf(fp, "none\n");

            if (next_tab->key)
            {
                kaps_fprintf(fp, "|   ");
                status = kaps_key_print(next_tab->key, fp);
                KAPS_TX_ERROR_CHECK(db->device, status);
                if (status != KAPS_OK)
                    break;
            }

            for (tmp = db->next_tab; tmp; tmp = tmp->next_tab)
            {
                if (tmp->is_clone && tmp->clone_of == next_tab)
                {
                    kaps_fprintf(fp, "|  |   \n|  |-- Clone ID:%d TYPE:%s BMR NO: ", tmp->tid,
                                 kaps_device_db_name(tmp));
                    if (tmp->is_bmr_assigned)
                        kaps_fprintf(fp, "%d\n", tmp->bmr_no);
                    else
                        kaps_fprintf(fp, "none\n");

                    if (tmp->key)
                    {
                        kaps_fprintf(fp, "|  |   ");
                        status = kaps_key_print(tmp->key, fp);
                        if (status != KAPS_OK)
                            break;
                    }
                }
            }
        }
    }

    if (status == KAPS_OK)
    {
        for (next_tab = db->next_tab; next_tab; next_tab = next_tab->next_tab)
        {
            if (next_tab->is_clone && next_tab->clone_of == db)
            {
                kaps_fprintf(fp, "|  \n|-- Clone ID:%d TYPE:%s BMR NO: ", next_tab->tid, kaps_device_db_name(next_tab));
                if (next_tab->is_bmr_assigned)
                    kaps_fprintf(fp, "%d\n", next_tab->bmr_no);
                else
                    kaps_fprintf(fp, "none\n");

                if (next_tab->key)
                {
                    kaps_fprintf(fp, "|   ");
                    status = kaps_key_print(next_tab->key, fp);
                    KAPS_TX_ERROR_CHECK(db->device, status);
                    if (status != KAPS_OK)
                        break;
                }
            }
        }
    }

    if (status == KAPS_OK)
    {
        /*
         * Print the AD information 
         */
        if (db->common_info->ad_info.ad)
        {
            struct kaps_ad_db *ad_db = (struct kaps_ad_db *) (db->common_info->ad_info.ad);

            kaps_fprintf(fp, "\n---- AD Information ----\n");
            while (ad_db)
            {
                kaps_fprintf(fp, "AD DB ID:%d Width:%d Capacity:%d\n", ad_db->db_info.tid, ad_db->user_width_1,
                             ad_db->db_info.common_info->capacity);
                ad_db = ad_db->next;
            }
        }

        /*
         * Print the Instruction Information 
         */
        kaps_fprintf(fp, "\n");
        for (next_tab = db; next_tab; next_tab = next_tab->next_tab)
        {
            uint32_t i = 0;

            if (!next_tab->ninstructions)
                continue;

            if (next_tab == db)
            {
                kaps_fprintf(fp, "\n---- Instructions for %s ID:%d ninstructions:%d ----\n",
                             kaps_device_db_name(next_tab), next_tab->tid, next_tab->ninstructions);
            }
            else
            {
                kaps_fprintf(fp, "\n---- Instructions for %s ID:%d ninstructions:%d ----\n",
                             next_tab->is_clone ? "Clone" : "Table", next_tab->tid, next_tab->ninstructions);
            }

            for (i = 0; i < next_tab->ninstructions; i++)
            {
                kaps_instruction_print(next_tab->instructions[i], fp);
            }
            kaps_fprintf(fp, "\n");
        }
    }

    if (status == KAPS_OK)
    {
        if (!db->device->is_config_locked)
        {
            kaps_fprintf(fp, "----------------------------------------------------------\n");
        }
        else
        {

            /*
             * Print the stats 
             */
            kaps_fprintf(fp, "---- STATS ----\n");
            status = kaps_db_stats(db, &stats);
            KAPS_TX_ERROR_CHECK(db->device, status);
            if (status != KAPS_OK)
                return status;

            kaps_fprintf(fp, "capacity estimate: %d nentries: %d range expansion: %d\n",
                         stats.capacity_estimate, stats.num_entries, stats.range_expansion);

            kaps_fprintf(fp, "----------------------------------------------------------\n");
        }
    }
    return status;
}

kaps_status
kaps_db_set_ad(
    struct kaps_db * db,
    struct kaps_ad_db * ad_db)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p\n", db, ad_db);

    if (!db || !ad_db)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        if (db->type == KAPS_DB_ACL && ad_db->user_width_1 == 0)
            status = KAPS_DB_0B_AD_FOR_ONLY_LPM;

        if (db->device->is_config_locked)
            status = KAPS_DB_ACTIVE;

        /*
         * We should not allow to set ad for database, after it is added to an instruction 
         */
        else if (db->ninstructions)
            status = KAPS_DB_ALREADY_ADDED_TO_INSTRUCTION;

        /*
         * We are using this API internally during Restore device info 
         */
        else if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT && db->device->issu_status != KAPS_ISSU_RESTORE_START)
                status = KAPS_ISSU_IN_PROGRESS;
        }
        else
        {
            /*
             * Safety else 
             */
        }
    }

    if (status == KAPS_OK)
        status = kaps_resource_db_add_ad(db->device, db, &ad_db->db_info);

    return status;
}

kaps_status
kaps_db_set_hb(
    struct kaps_db * db,
    struct kaps_hb_db * hb_db)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p\n", db, hb_db);

    if (!db || !hb_db)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        if (db->device->is_config_locked)
            status = KAPS_DB_ACTIVE;

        /*
         * We should not allow to set hb for database, after it is added to an instruction 
         */
        else if (db->ninstructions)
            status = KAPS_DB_ALREADY_ADDED_TO_INSTRUCTION;

        /*
         * We are using this API internally during Restore device info 
         */
        else if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT && db->device->issu_status != KAPS_ISSU_RESTORE_START)
                status = KAPS_ISSU_IN_PROGRESS;
        }
        else
        {
            /*
             * safety else 
             */
        }
    }

    if (status == KAPS_OK)
        status = kaps_resource_db_add_hb(db->device, db, &hb_db->db_info);

    return status;
}

kaps_status
kaps_db_add_prefix(
    struct kaps_db * db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_entry ** entry)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *tmp = NULL;
    struct kaps_db *tab = db;

    KAPS_TRACE_IN("%p %p %u %p\n", db, prefix, length, entry);

    if (db == NULL || prefix == NULL || entry == NULL)
        status = KAPS_INVALID_ARGUMENT;
    else
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_OK)
    {
        if (db->is_clone)
            status = KAPS_ADD_DEL_ENTRY_TO_CLONE_DB;

        else if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (db->key == NULL)
            status = KAPS_MISSING_KEY;

        else if (!db->fn_table)
            status = KAPS_INTERNAL_ERROR;

        else if (!db->fn_table->db_add_prefix)
            status = KAPS_INVALID_ARGUMENT;
        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        if (db->common_info->insertion_mode == KAPS_DB_INSERTION_MODE_NONE)
        {
            db->common_info->insertion_mode = KAPS_DB_INSERTION_MODE_NORMAL;
        }

        status = db->fn_table->db_add_prefix(db, prefix, length, entry);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        tmp = *entry;
        KAPS_WB_HANDLE_GET_FREE_AND_WRITE(db, (*entry));

    }

    if (status == KAPS_OK)
    {
        if (db->device->nv_ptr)
        {
            uint32_t entry_offset;
            uint32_t offset;
            uint32_t pending_count = 0;
            uint8_t *nv_ptr = tab->device->nv_ptr;
            uint8_t *pending_ptr = NULL;

            status = db->fn_table->cr_store_entry(tab, tmp);
            if (status == KAPS_OK)
            {
                entry_offset = kaps_db_entry_get_nv_offset(db, tmp->user_handle);

                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;

                if (kaps_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                    status = KAPS_EXHAUSTED_NV_MEMORY;
            }

            if (status == KAPS_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;

                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    KAPS_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_status
kaps_db_add_prefix_with_index(
    struct kaps_db * db,
    uint8_t * prefix,
    uint32_t length,
    uint32_t user_handle)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry = NULL;
    struct kaps_db *tab = db;

    KAPS_TRACE_IN("%p %p %u %u\n", db, prefix, length, user_handle);

    if (db == NULL || prefix == NULL || (user_handle >> 27) || !user_handle)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_OK)
    {

        if (db->is_clone)
            status = KAPS_ADD_DEL_ENTRY_TO_CLONE_DB;

        else if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (db->key == NULL)
            status = KAPS_MISSING_KEY;

        else if (!db->fn_table)
            status = KAPS_INTERNAL_ERROR;

        else if (!db->fn_table->db_add_prefix)
            status = KAPS_INVALID_ARGUMENT;
        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        while (user_handle > (*db->common_info->user_hdl_table_size))
        {
            KAPS_WB_HANDLE_TABLE_GROW(db);
        }

        if (KAPS_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, user_handle) == 0)
            status = KAPS_DB_WB_LOCATION_BUSY;
    }

    if (status == KAPS_OK)
    {
        db->common_info->insertion_mode = KAPS_DB_INSERTION_MODE_ENTRY_WITH_INDEX;

        status = db->fn_table->db_add_prefix(db, prefix, length, &entry);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        KAPS_WB_HANDLE_WRITE_LOC(db, entry, user_handle);

    }

    if (status == KAPS_OK)
    {
        if (db->device->nv_ptr)
        {
            uint32_t entry_offset;
            uint32_t offset;
            uint32_t pending_count = 0;
            uint8_t *nv_ptr = tab->device->nv_ptr;
            uint8_t *pending_ptr = NULL;

            status = db->fn_table->cr_store_entry(tab, entry);

            if (status == KAPS_OK)
            {
                entry_offset = kaps_db_entry_get_nv_offset(db, entry->user_handle);

                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;

                if (kaps_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                    status = KAPS_EXHAUSTED_NV_MEMORY;
            }

            if (status == KAPS_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;

                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    return status;
}

kaps_status
kaps_db_add_em(
    struct kaps_db * db,
    uint8_t * data,
    struct kaps_entry ** entry)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *tmp = NULL;
    struct kaps_db *tab = db;

    KAPS_TRACE_IN("%p %p %p\n", db, data, entry);

    if (db == NULL || data == NULL || entry == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_OK)
    {
        if (db->is_clone)
            status = KAPS_INVALID_ARGUMENT;

        else if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (!db->fn_table)
            status = KAPS_INTERNAL_ERROR;

        else if (!db->fn_table->db_add_em)
            status = KAPS_INVALID_ARGUMENT;
        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        if (db->common_info->insertion_mode == KAPS_DB_INSERTION_MODE_NONE)
            db->common_info->insertion_mode = KAPS_DB_INSERTION_MODE_NORMAL;

        status = db->fn_table->db_add_em(db, data, entry);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        tmp = *entry;
        KAPS_WB_HANDLE_GET_FREE_AND_WRITE(db, (*entry));
    }

    if (status == KAPS_OK)
    {
        if (db->device->nv_ptr)
        {
            /*
             * place user_handle in this order: (31:31:add/delete),(30:24:table_id),(23:0:user_handle) 0. add, 1.
             * delete 
             */
            uint32_t offset;
            uint32_t pending_count = 0;
            uint8_t *nv_ptr = tab->device->nv_ptr;
            uint32_t entry_offset;
            uint8_t *pending_ptr = NULL;

            status = db->fn_table->cr_store_entry(tab, tmp);
            if (status == KAPS_OK)
            {
                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;

                if (kaps_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                    status = KAPS_EXHAUSTED_NV_MEMORY;
            }

            if (status == KAPS_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;
                entry_offset = kaps_db_entry_get_nv_offset(db, tmp->user_handle);
                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    KAPS_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_status
kaps_db_add_em_with_index(
    struct kaps_db * db,
    uint8_t * data,
    uint32_t user_handle)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry = NULL;
    struct kaps_db *tab = db;

    KAPS_TRACE_IN("%p %p %u\n", db, data, user_handle);

    if (db == NULL || data == NULL || (user_handle >> 27) || !user_handle)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_OK)
    {
        if (db->is_clone)
            status = KAPS_INVALID_ARGUMENT;

        else if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (!db->fn_table)
            status = KAPS_INTERNAL_ERROR;

        else if (!db->fn_table->db_add_em)
            status = KAPS_INVALID_ARGUMENT;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        while (user_handle > (*db->common_info->user_hdl_table_size))
        {
            KAPS_WB_HANDLE_TABLE_GROW(db);
        }

        if (KAPS_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, user_handle) == 0)
            status = KAPS_DB_WB_LOCATION_BUSY;
    }

    if (status == KAPS_OK)
    {
        db->common_info->insertion_mode = KAPS_DB_INSERTION_MODE_ENTRY_WITH_INDEX;

        status = db->fn_table->db_add_em(db, data, &entry);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        KAPS_WB_HANDLE_WRITE_LOC(db, entry, user_handle);

        if (db->device->nv_ptr)
        {
            /*
             * place user_handle in this order: (31:31:add/delete),(30:24:table_id),(23:0:user_handle) 0. add, 1.
             * delete 
             */
            uint32_t offset;
            uint32_t pending_count = 0;
            uint8_t *nv_ptr = tab->device->nv_ptr;
            uint32_t entry_offset;
            uint8_t *pending_ptr = NULL;

            status = db->fn_table->cr_store_entry(tab, entry);

            if (status == KAPS_OK)
            {
                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;

                if (kaps_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                    status = KAPS_EXHAUSTED_NV_MEMORY;
            }

            if (status == KAPS_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;
                entry_offset = kaps_db_entry_get_nv_offset(db, entry->user_handle);
                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    return status;
}

kaps_status
kaps_db_delete_entry(
    struct kaps_db * db,
    struct kaps_entry * entry)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p = NULL;

    KAPS_TRACE_IN("%p %p\n", db, entry);

    if (db == NULL || entry == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status != KAPS_ISSU_INIT))
            status = KAPS_UNSUPPORTED;

        else if (db->is_clone)
            status = KAPS_ADD_DEL_ENTRY_TO_CLONE_DB;

        else if (!db->fn_table || !db->fn_table->db_delete_entry)
            status = KAPS_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        if (!entry_p)
            status = KAPS_INVALID_HANDLE;
    }

    if (status == KAPS_OK)
    {

        /*
         * PIO stats 
         */
        db->device->num_of_piowrs = 0;
        db->device->num_of_piords = 0;
        db->device->num_blk_ops = 0;

        if (db->device->nv_ptr && db->device->issu_status == KAPS_ISSU_INIT)
        {

            uint32_t entry_offset = kaps_db_entry_get_nv_offset(db, entry_p->user_handle);
            uint32_t offset;
            uint8_t *nv_ptr = db->device->nv_ptr;
            uint8_t *pending_ptr = NULL;
            uint32_t pending_count = 0;

            nv_ptr += entry_offset;

            /*
             * mark for delete 
             */
            *(uint8_t *) nv_ptr = 1;

            nv_ptr = db->device->nv_ptr;

            offset = db->device->nv_mem_mgr->offset_device_pending_list;
            nv_ptr += offset;
            pending_ptr = nv_ptr;
            pending_count = *(uint32_t *) nv_ptr;

            if (kaps_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                status = KAPS_EXHAUSTED_NV_MEMORY;
            if (status == KAPS_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;
                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    if (status == KAPS_OK)
    {
        db->device->db_bc_bitmap = db->bc_bitmap;

        status = db->fn_table->db_delete_entry(db, entry_p);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_OK)
    {
        db->common_info->pio_stats.num_of_piords += db->device->num_of_piords;
        db->common_info->pio_stats.num_of_piowrs += db->device->num_of_piowrs;
        db->common_info->pio_stats.num_blk_ops += db->device->num_blk_ops;

        if (!db->common_info->defer_deletes_to_install)
        {
            KAPS_WB_HANDLE_DELETE_LOC(KAPS_GET_DB_PARENT(db), (uintptr_t) entry);

            if (db->device->nv_ptr && db->device->issu_status == KAPS_ISSU_INIT)
            {
                uint32_t entry_offset = kaps_db_entry_get_nv_offset(db, (uint32_t) ((uintptr_t) entry));
                uint32_t offset;
                uint8_t *nv_ptr = db->device->nv_ptr;
                uint32_t pending_count = 0;

                nv_ptr += entry_offset;
                *(uint8_t *) nv_ptr = 0;

                nv_ptr = db->device->nv_ptr;
                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_count = *(uint32_t *) nv_ptr;
                kaps_sassert(pending_count != 0);
                *(uint32_t *) nv_ptr = pending_count - 1;
            }
        }
    }

    if (status == KAPS_OK)
    {
        if (db->type == KAPS_DB_ACL && db->device->nv_ptr && db->device->issu_status == KAPS_ISSU_INIT)
        {
            uint32_t offset;
            struct kaps_wb_cb_functions cb_fun;
            uint32_t nv_db_iter = db->common_info->nv_db_iter;

            cb_fun.handle = db->device->nv_handle;
            cb_fun.read_fn = db->device->nv_read_fn;
            cb_fun.write_fn = db->device->nv_write_fn;
            cb_fun.nv_offset = &offset;
            cb_fun.nv_ptr = db->device->nv_ptr;

            offset = db->device->nv_mem_mgr->offset_db_info_start[nv_db_iter];
            cb_fun.nv_ptr = (uint8_t *) db->device->nv_ptr + offset;

            status = db->fn_table->save_cr_state(db, &cb_fun);
            KAPS_TX_ERROR_CHECK(db->device, status);
        }
    }

    return status;
}

kaps_status
kaps_entry_set_property(
    struct kaps_db * db,
    struct kaps_entry * entry,
    enum kaps_entry_properties property,
    ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p = NULL;

    KAPS_TRACE_IN("%p %p %u  TO_BE_FILLED\n", db, entry, property);

    if (entry == NULL)
        status = KAPS_INVALID_ARGUMENT;
    else
    {

        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);
        KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(db->device, entry_p, db);
        va_start(vl, property);

        if (!db->fn_table)
            status = KAPS_INTERNAL_ERROR;
        if (status == KAPS_OK && !db->fn_table->entry_set_property)
            status = KAPS_UNSUPPORTED;
        if (status == KAPS_OK)
            status = db->fn_table->entry_set_property(db, entry_p, property, vl);

        va_end(vl);
    }
    return status;
}

kaps_status
kaps_entry_add_range(
    struct kaps_db * db,
    struct kaps_entry * entry,
    uint16_t lo,
    uint16_t hi,
    int32_t range_no)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p = NULL;

    KAPS_TRACE_IN("%p %p %u %u %d\n", db, entry, lo, hi, range_no);

    if (db == NULL || entry == NULL)
        status = KAPS_INVALID_ARGUMENT;
    else
    {

        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_OK)
    {
        if (db->type != KAPS_DB_ACL)
            status = KAPS_INVALID_DB_TYPE;

        else if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->entry_add_range)
            status = KAPS_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);
        status = db->fn_table->entry_add_range(db, entry_p, lo, hi, range_no);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

kaps_status
kaps_entry_add_ad(
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_ad * ad)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p = NULL;
    struct kaps_ad *ad_p = NULL;
    struct kaps_ad_db *e_ad_db = NULL;

    KAPS_TRACE_IN("%p %p %p\n", db, entry, ad);

    if (db == NULL || ad == NULL || entry == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;
    }

    /*
     * If the database is not expecting AD, do not allow it
     */
    if (status == KAPS_OK)
    {
        if (db->common_info->ad_info.ad == NULL)
            status = KAPS_AD_DB_ABSENT;

        else if (!db->fn_table || !db->fn_table->entry_add_ad)
            status = KAPS_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT && db->device->issu_status != KAPS_ISSU_RESTORE_END)
                status = KAPS_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), &entry_p, (uintptr_t) entry);
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db)->common_info->ad_info.ad, (&ad_p), (uintptr_t) ad);

        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->device, ad_p, e_ad_db)
            if (db->device->nv_ptr && db->type == KAPS_DB_ACL && ad_p->is_dangling_ad)
        {
            if (db->common_info->has_ranges == 0)
            {
                kaps_sassert(ad_p->entries);
                KAPS_WB_HANDLE_DELETE_LOC(KAPS_GET_DB_PARENT(db), (uintptr_t) ad_p->entries);
            }
            status = db->fn_table->cr_delete_entry((struct kaps_db *) e_ad_db, ad_p, 0);
            KAPS_TX_ERROR_CHECK(db->device, status);
            if (status == KAPS_OK)
            {
                ad_p->entries = NULL;
                ad_p->is_dangling_ad = 0;
            }
        }
    }

    if (status == KAPS_OK)
    {
        status = db->fn_table->entry_add_ad(db, entry_p, ad_p);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_OK)
    {
        if (!db->common_info->has_ranges && db->device->nv_ptr && db->device->issu_status == KAPS_ISSU_INIT)
        {
            uint32_t user_width = e_ad_db->user_bytes;
            uint32_t byte_offset =
                (e_ad_db->db_info.width.ad_width_1 + KAPS_BITS_IN_BYTE - 1) / KAPS_BITS_IN_BYTE - user_width;

            if (kaps_resource_get_ad_type(db) == KAPS_AD_TYPE_INPLACE)
                byte_offset = 0;

            status =
                db->fn_table->cr_entry_update_ad(db->device, entry_p, entry_p->ad_handle,
                                                 &entry_p->ad_handle->value[byte_offset]);
            KAPS_TX_ERROR_CHECK(db->device, status);
        }
    }
    return status;
}

kaps_status
kaps_entry_add_hb(
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_hb * hb)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p = NULL;
    struct kaps_hb *hb_p = NULL;

    KAPS_TRACE_IN("%p %p %p\n", db, entry, hb);

    if (db == NULL || hb == NULL || entry == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;
    }

    if (status == KAPS_OK)
    {
        /*
         * If the database is not expecting HB, do not allow it
         */
        if (db->common_info->hb_info.hb == NULL)
            status = KAPS_HB_DB_ABSENT;

        else if (!db->fn_table)
            status = KAPS_INTERNAL_ERROR;

        else if (!db->fn_table->entry_add_hb)
            status = KAPS_DB_HAS_NO_HB;

        else if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT && db->device->issu_status != KAPS_ISSU_RESTORE_END)
                status = KAPS_ISSU_IN_PROGRESS;
        }
        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), &entry_p, (uintptr_t) entry);
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db)->common_info->hb_info.hb, (&hb_p), (uintptr_t) hb);

        kaps_sassert(entry_p->ad_handle);

        status = db->fn_table->entry_add_hb(db, entry_p, hb_p);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

kaps_status
kaps_entry_print(
    struct kaps_db * db,
    struct kaps_entry * entry,
    FILE * fp)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p = NULL;

    KAPS_TRACE_IN("%p %p %p\n", db, entry, fp);

    if (db == NULL || fp == NULL || entry == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->entry_print)
            status = KAPS_INTERNAL_ERROR;

        else if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT)
                status = KAPS_ISSU_IN_PROGRESS;
        }
        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);
        status = db->fn_table->entry_print(db, entry_p, fp);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

#define KAPS_STRY_NEW(A)                                                 \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_OK)                                     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    }

kaps_status
kaps_db_install(
    struct kaps_db * db)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p\n", db);

    if (db == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->db_install)
            status = KAPS_INTERNAL_ERROR;
        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        /*
         * PIO stats 
         */
        db->device->num_of_piowrs = 0;
        db->device->num_of_piords = 0;
        db->device->num_blk_ops = 0;

        db->device->db_bc_bitmap = db->bc_bitmap;

        status = db->fn_table->db_install(db);
        KAPS_TX_ERROR_CHECK(db->device, status);
        if (status != KAPS_OK)
        {

        }
        else
        {

            db->common_info->pio_stats.num_of_piords += db->device->num_of_piords;
            db->common_info->pio_stats.num_of_piowrs += db->device->num_of_piowrs;
            db->common_info->pio_stats.num_blk_ops += db->device->num_blk_ops;

            if (db->device->nv_ptr)
            {
                uint32_t offset;
                struct kaps_wb_cb_functions cb_fun;
                uint32_t nv_db_iter = db->common_info->nv_db_iter;

                cb_fun.handle = db->device->nv_handle;
                cb_fun.read_fn = db->device->nv_read_fn;
                cb_fun.write_fn = db->device->nv_write_fn;
                cb_fun.nv_offset = &offset;
                cb_fun.nv_ptr = db->device->nv_ptr;

                offset = db->device->nv_mem_mgr->offset_db_info_start[nv_db_iter];
                cb_fun.nv_ptr = (uint8_t *) db->device->nv_ptr + offset;

                status = db->fn_table->save_cr_state(db, &cb_fun);
            }
        }
    }

    return status;
}

kaps_status
kaps_db_stats(
    struct kaps_db * db,
    struct kaps_db_stats * stats)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p\n", db, stats);

    if (db == NULL || stats == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->db_stats)
            status = KAPS_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        status = db->fn_table->db_stats(db, stats);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

/* Performs a software search in the database. It searches for the highest matching ACL entry for ACLs,
   and the longest prefix match for LPM searches. This API can be used to validate the SW structures of
   the database by comparing the returned hit-index/AD with expected. The expected value must be
   calculated taking into account the fact that the returned Index/AD depends on the priority of stored
   entries / longest matched prefix. */

kaps_status
kaps_db_search(
    struct kaps_db * db,
    uint8_t * key,
    struct kaps_entry ** entry,
    int32_t * index,
    int32_t * prio_len)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p %p %p %p\n", db, key, entry, index, prio_len);

    if (db == NULL || key == NULL || entry == NULL || index == NULL || prio_len == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->db_search)
            status = KAPS_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        if (db->type != KAPS_DB_ACL)
        {
            if (db->is_clone)
                db = db->clone_of;
        }

        status = db->fn_table->db_search(db, key, entry, index, prio_len);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_OK)
    {
        if (db->device->flags & KAPS_DEVICE_ISSU && *entry)
            *entry = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, (*entry)->user_handle);
    }

    KAPS_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_status
kaps_db_add_table(
    struct kaps_db * db,
    uint32_t id,
    struct kaps_db ** table)
{
    struct kaps_db *tab;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %u %p\n", db, id, table);

    if (db == NULL || table == NULL)
        status = KAPS_INVALID_ARGUMENT;

    /*
     * If the user inadvertently sent us a table pointer, instead
     * of the master database handle, we generate the master
     * database pointer here
     */
    if (status == KAPS_OK)
    {
        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT)
                return KAPS_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_OK)
    {
        if (db->parent)
            db = db->parent;

        if (db->device->is_config_locked)
            status = KAPS_DB_ACTIVE;

        else if (!db->fn_table || !db->fn_table->db_add_table)
            status = KAPS_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        tab = db;
        while (tab)
        {
            if (tab->ninstructions)
                return KAPS_TABLES_CREATION_NOT_ALLOWED;
            tab = tab->next_tab;
        }

        status = db->fn_table->db_add_table(db, id, 0, table);
    }

    KAPS_TRACE_OUT("%p\n", *table);
    return status;
}

kaps_status
kaps_db_entry_iter_init(
    struct kaps_db * db,
    struct kaps_entry_iter ** iter)
{
    struct kaps_c_list_iter *it;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p\n", db, iter);

    if (!db || !iter)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;
    }

    if (status == KAPS_OK)
    {
        if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED))
            return KAPS_UNSUPPORTED;
    }

    if (status == KAPS_OK)
    {
        it = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, sizeof(*it));
        if (!it)
            status = KAPS_OUT_OF_MEMORY;
    }

    if (status == KAPS_OK)
    {
        kaps_c_list_iter_init(&db->db_list, it);
        *iter = (struct kaps_entry_iter *) it;
    }

    KAPS_TRACE_OUT("%p\n", *iter);
    return status;
}

kaps_status
kaps_db_entry_iter_next(
    struct kaps_db * db,
    struct kaps_entry_iter * iter,
    struct kaps_entry ** entry)
{
    struct kaps_entry *entry_p;
    struct kaps_c_list_iter *it;
    struct kaps_list_node *el;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p %p\n", db, iter, entry);

    if (!db || !iter || !entry)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;
    }

    if (status == KAPS_OK)
    {
        if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED))
            status = KAPS_UNSUPPORTED;
    }

    if (status == KAPS_OK)
    {
        it = (struct kaps_c_list_iter *) iter;
        el = kaps_c_list_iter_next(it);
        if (el == NULL)
        {
            *entry = NULL;

        }
        else
        {

            entry_p = KAPS_DBLIST_TO_KAPS_ENTRY(el);

            if (db->device->flags & KAPS_DEVICE_ISSU)
            {
                *entry = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, entry_p->user_handle);
            }
            else
                *entry = entry_p;
        }
    }
    KAPS_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_status
kaps_db_entry_iter_destroy(
    struct kaps_db * db,
    struct kaps_entry_iter * iter)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p\n", db, iter);

    if (!db || !iter)
        status = KAPS_INVALID_ARGUMENT;
    else
    {

        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        db->device->alloc->xfree(db->device->alloc->cookie, iter);
    }

    return status;
}

kaps_status
kaps_db_delete_all_entries(
    struct kaps_db * db)
{
    uint32_t defer_deletes = 0;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p\n", db);

    if (db == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_OK)
    {
        if (db->is_clone)
            status = KAPS_ADD_DEL_ENTRY_TO_CLONE_DB;

        else if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED))
            status = KAPS_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->db_delete_all_entries)
            status = KAPS_INTERNAL_ERROR;
        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        /*
         * PIO stats 
         */
        db->device->num_of_piowrs = 0;
        db->device->num_of_piords = 0;
        db->device->num_blk_ops = 0;

        db->device->db_bc_bitmap = db->bc_bitmap;

        if (db->common_info->defer_deletes_to_install)
        {
            defer_deletes = 1;
            db->common_info->defer_deletes_to_install = 0;
        }

        status = db->fn_table->db_delete_all_entries(db);
    }

    if (status == KAPS_OK)
    {
        db->common_info->defer_deletes_to_install = defer_deletes;

        db->common_info->pio_stats.num_of_piords += db->device->num_of_piords;
        db->common_info->pio_stats.num_of_piowrs += db->device->num_of_piowrs;
        db->common_info->pio_stats.num_blk_ops += db->device->num_blk_ops;

        if (db->type == KAPS_DB_ACL && db->device->nv_ptr && db->device->issu_status == KAPS_ISSU_INIT)
        {
            uint32_t offset;
            struct kaps_wb_cb_functions cb_fun;
            uint32_t nv_db_iter = db->common_info->nv_db_iter;

            cb_fun.handle = db->device->nv_handle;
            cb_fun.read_fn = db->device->nv_read_fn;
            cb_fun.write_fn = db->device->nv_write_fn;
            cb_fun.nv_offset = &offset;
            cb_fun.nv_ptr = db->device->nv_ptr;

            if (db->device->nv_mem_mgr)
            {
                offset = db->device->nv_mem_mgr->offset_db_info_start[nv_db_iter];
                cb_fun.nv_ptr = (uint8_t *) db->device->nv_ptr + offset;

                status = db->fn_table->save_cr_state(db, &cb_fun);
            }
        }
    }
    return status;
}

kaps_status
kaps_db_delete_all_pending_entries(
    struct kaps_db * db)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p\n", db);

    KAPS_TX_ERROR_CHECK(db->device, 0);

    KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

    status = db->fn_table->db_delete_all_pending_entries(db);

    return status;
}

kaps_status
kaps_entry_get_priority(
    struct kaps_db * db,
    struct kaps_entry * entry,
    uint32_t * prio_length)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p;

    KAPS_TRACE_IN("%p %p %p\n", db, entry, prio_length);

    if (!db || !entry || !prio_length)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED))
            status = KAPS_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->entry_get_priority)
            status = KAPS_INTERNAL_ERROR;
        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        if (!entry_p)
            status = KAPS_INVALID_ARGUMENT;
    }

    if (status == KAPS_OK)
    {
        status = db->fn_table->entry_get_priority(db, entry_p, prio_length);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

kaps_status
kaps_entry_get_ad(
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_ad ** ad)
{
    struct kaps_entry *entry_p = NULL;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p %p\n", db, entry, ad);

    if (!db || !entry || !ad)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT)
                status = KAPS_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        if (!entry_p)
            status = KAPS_INVALID_HANDLE;

        else if (!db->common_info->ad_info.ad)
            status = KAPS_AD_DB_ABSENT;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        *ad = entry_p->ad_handle;
        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            *ad = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_ad, entry_p->ad_handle->user_handle);
        }
    }

    KAPS_TRACE_OUT("%p\n", *ad);
    return status;
}

kaps_status
kaps_entry_get_hb(
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_hb ** hb)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p = NULL;

    KAPS_TRACE_IN("%p %p %p\n", db, entry, hb);

    if (!db || !entry || !hb)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_ISSU_INIT)
                status = KAPS_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        if (!entry_p)
            status = KAPS_INVALID_HANDLE;

        else if (!db->common_info->hb_info.hb)
            status = KAPS_HB_DB_ABSENT;

        else if (!db->fn_table || !db->fn_table->get_hb)
            status = KAPS_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_OK)
    {
        status = db->fn_table->get_hb(db, entry_p, hb);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    KAPS_TRACE_OUT("%p\n", *hb);
    return status;
}

kaps_status
kaps_entry_get_index(
    struct kaps_db * db,
    struct kaps_entry * entry,
    int32_t * nindices,
    int32_t ** indices)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p;

    KAPS_TRACE_IN("%p %p %p %p\n", db, entry, nindices, indices);

    if (!db || !entry || !nindices || !indices)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED))
            status = KAPS_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->entry_get_index)
            status = KAPS_INTERNAL_ERROR;

        else
        {

        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        status = db->fn_table->entry_get_index(db, entry_p, nindices, indices);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    KAPS_TRACE_OUT("%d\n", *nindices);
    return status;
}

kaps_status
kaps_entry_free_index_array(
    struct kaps_db * db,
    int32_t * indices)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p\n", db, indices);

    if (!db || !indices)
        status = KAPS_INVALID_ARGUMENT;
    else
    {

        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        db->device->alloc->xfree(db->device->alloc->cookie, indices);
    }

    return status;
}

kaps_status
kaps_db_get_prefix_handle(
    struct kaps_db * db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_entry ** entry)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p %u %p\n", db, prefix, length, entry);

    if (!db || !prefix || entry == NULL)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_OK)
    {
        if (db->key == NULL)
            status = KAPS_PREFIX_NOT_FOUND;

        else if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED))
            status = KAPS_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->get_prefix_handle)
            status = KAPS_INTERNAL_ERROR;

        else
        {

        }
    }

    if (status == KAPS_OK)
    {
        status = db->fn_table->get_prefix_handle(db, prefix, length, entry);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_OK)
    {
        if (db->device->flags & KAPS_DEVICE_ISSU && *entry)
        {
            *entry = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, (*entry)->user_handle);
        }
    }

    KAPS_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_status
kaps_db_clone(
    struct kaps_db * db,
    uint32_t id,
    struct kaps_db ** clone)
{
    struct kaps_db *tab;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %u %p\n", db, id, clone);

    if (!db || !clone)
        status = KAPS_INVALID_ARGUMENT;

    else if (db->device->is_config_locked)
        status = KAPS_DEVICE_ALREADY_LOCKED;

    else if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status != KAPS_ISSU_INIT))
        status = KAPS_ISSU_IN_PROGRESS;

    else if (!db->fn_table || !db->fn_table->db_add_table)
        status = KAPS_INTERNAL_ERROR;

    else
    {

    }

    if (status == KAPS_OK)
    {
        tab = db;
        while (tab)
        {
            if (tab->ninstructions)
            {
                status = KAPS_TABLES_CREATION_NOT_ALLOWED;
                break;
            }
            tab = tab->next_tab;
        }
    }

    if (status == KAPS_OK)
    {
        status = db->fn_table->db_add_table(db, id, 1, clone);
    }

    KAPS_TRACE_OUT("%p\n", *clone);
    return status;
}

kaps_status
kaps_entry_get_info(
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_entry_info * info)
{
    kaps_status status = KAPS_OK;
    struct kaps_entry *entry_p;

    KAPS_TRACE_IN("%p %p %p\n", db, entry, info);

    if (!db || !entry || !info)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_DEVICE_ISSU) && (db->device->issu_status == KAPS_ISSU_SAVE_COMPLETED))
            status = KAPS_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->entry_get_info)
            status = KAPS_INTERNAL_ERROR;

        else
        {

        }
    }

    if (status == KAPS_OK)
    {
        if (db->device->flags & KAPS_DEVICE_ISSU)
        {
            if ((uintptr_t) entry > (*db->common_info->user_hdl_table_size))
                status = KAPS_INVALID_HANDLE;
            if (KAPS_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, ((uintptr_t) entry)))
                status = KAPS_INVALID_HANDLE;
        }
    }

    if (status == KAPS_OK)
    {
        KAPS_WB_HANDLE_READ_LOC(KAPS_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        status = db->fn_table->entry_get_info(db, entry_p, info);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

kaps_status
kaps_db_fix_ab_location(
    struct kaps_device * device,
    struct kaps_db * db,
    uint32_t hw_location)
{
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %p %u\n", device, db, hw_location);

    if (!device || !db || hw_location == -1)
        status = KAPS_INVALID_ARGUMENT;

    if (status == KAPS_OK)
    {
        KAPS_TX_ERROR_CHECK(db->device, 0);

        KAPS_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_FATAL_TRANSPORT_ERROR);

        device->db_bc_bitmap = db->bc_bitmap;

        status = db->fn_table->db_fix_ab_location(device, db, hw_location, NULL);
        KAPS_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

struct kaps_db *
kaps_db_get_main_db(
    struct kaps_db *db)
{
    struct kaps_db *main_db = db;
    struct kaps_db *ret = NULL;

    KAPS_TRACE_IN("%p\n", db);

    if (main_db)
    {

        if (main_db->clone_of)
            main_db = main_db->clone_of;

        if (main_db->parent)
            main_db = main_db->parent;

        ret = main_db;
    }
    return ret;
}
