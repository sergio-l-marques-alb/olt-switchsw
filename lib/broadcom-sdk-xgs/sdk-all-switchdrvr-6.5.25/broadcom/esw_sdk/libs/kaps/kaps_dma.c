/*******************************************************************************
 *
 * Copyright 2014-2020 Broadcom Corporation
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

#include "kaps_dma.h"
#include "kaps_dma_internal.h"
#include "kaps_algo_hw.h"

kaps_status kaps_dma_db_init(struct kaps_device *device, uint32_t id, uint32_t capacity,
                           uint32_t width_1, struct kaps_dma_db **db)
{
    struct kaps_dma_db *dma_db;
    kaps_status status;
    uint32_t kaps_dma_width = KAPS_DMA_WIDTH_1;
    uint32_t kaps_dma_rows = KAPS_DMA_NUM_ROWS;

    KAPS_TRACE_IN("%p %u %u %u %p\n", device, id, capacity, width_1, db);
    if (device == NULL)
        return KAPS_INVALID_DEVICE_PTR;

    if (db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_DEVICE_ALREADY_LOCKED;

    /* We are using this API internally during Restore device info*/
    if (device->flags & KAPS_DEVICE_ISSU) {
        if (device->issu_status != KAPS_ISSU_INIT &&
            device->issu_status != KAPS_ISSU_RESTORE_START)
            return KAPS_ISSU_IN_PROGRESS;
    }

    if (capacity == 0)
        return KAPS_DYNAMIC_UNSUPPORTED;

    if (device->id == KAPS_QUMRAN_DEVICE_ID)
        kaps_dma_width = 127;
    else if (device->id == KAPS_QUX_DEVICE_ID || device->id == KAPS_JERICHO_PLUS_DEVICE_ID) {
        kaps_dma_width = 120;
        kaps_dma_rows = KAPS_DMA_NUM_ROWS * 4;
    }

    if (device->type == KAPS_DEVICE_KAPS) {
        if (width_1 != kaps_dma_width)
            return KAPS_UNSUPPORTED_DMA_WIDTH;
        if (capacity > kaps_dma_rows)
            return KAPS_INVALID_ARGUMENT;
    } else {
        return KAPS_UNSUPPORTED;
    }

    dma_db = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_dma_db));
    if (!dma_db) {
        return KAPS_OUT_OF_MEMORY;
    }
    status = kaps_resource_add_database(device, &dma_db->db_info, id,
                                   capacity, width_1, KAPS_DB_DMA);
    if (status != KAPS_OK) {
        device->alloc->xfree(device->alloc->cookie, dma_db);
        return status;
    }

    dma_db->user_width_1 = width_1;
    dma_db->dma_tag = -1;

    dma_db->alloc_fbmap = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_fast_bitmap));
    if (!dma_db->alloc_fbmap)
        return KAPS_OUT_OF_MEMORY;
    KAPS_TRY(kaps_init_bmp(dma_db->alloc_fbmap, device->alloc, device->hw_res->total_num_bb[0], 0));

    *db = dma_db;
    KAPS_TRACE_OUT("%p\n", *db);

    return KAPS_OK;
}

kaps_status kaps_dma_db_destroy(struct kaps_dma_db *db)
{
    KAPS_TRACE_IN("%p\n", db);
    if (!db)
        return KAPS_INVALID_ARGUMENT;

    /*
     * Check if database is still linked to DB list
     * and free it
     */
    {
        struct kaps_device *device;
        struct kaps_c_list_iter it;
        struct kaps_list_node *el;

        device = db->db_info.device;
        if (device->main_dev)
            device = device->main_dev;
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL) {
            struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(el);

            if (tmp == &db->db_info) {
                kaps_c_list_remove_node(&device->db_list, el, &it);
                break;
            }
        }
    }

    kaps_free_bmp(db->alloc_fbmap, db->db_info.device->alloc);
    db->db_info.device->alloc->xfree(db->db_info.device->alloc->cookie, db->alloc_fbmap);

    kaps_resource_free_database(db->db_info.device, &db->db_info);


    db->db_info.device->alloc->xfree(db->db_info.device->alloc->cookie, db);
    return KAPS_OK;
}

kaps_status kaps_dma_db_set_resource(struct kaps_dma_db *db, enum kaps_hw_resource resource, ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;

    if (db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->is_config_locked)
        return KAPS_DB_ACTIVE;

    if (db->db_info.device->flags & KAPS_DEVICE_ISSU) {
        if (db->db_info.device->issu_status != KAPS_ISSU_INIT &&
            db->db_info.device->issu_status != KAPS_ISSU_RESTORE_START)
            return KAPS_ISSU_IN_PROGRESS;
    }

    va_start(vl, resource);
    switch (resource) {
    case KAPS_HW_RESOURCE_UDA:
    {
        uint32_t value = va_arg(vl, uint32_t);

        KAPS_TRACE_IN("%p %d %u\n", db, resource, value);
        if (value > 0) {
            db->db_info.hw_res.db_res->start_lpu = value;
            db->db_info.hw_res.db_res->end_lpu = value;
            db->db_info.common_info->user_specified = 1;
        } else {
            status = KAPS_INVALID_ARGUMENT;
        }

        break;
    }
    default:
        status = KAPS_INVALID_ARGUMENT;
        break;
    }
    va_end(vl);
    return status;
}

kaps_status kaps_dma_db_get_resource(struct kaps_dma_db *db, enum kaps_hw_resource resource, ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;

    if (db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->flags & KAPS_DEVICE_ISSU) {
        if (db->db_info.device->issu_status != KAPS_ISSU_INIT)
            return KAPS_ISSU_IN_PROGRESS;
    }

    va_start(vl, resource);
    switch (resource) {
    case KAPS_HW_RESOURCE_UDA:
    {
        uint32_t *value = va_arg(vl, uint32_t *);

        KAPS_TRACE_IN("%p %d %u\n", db, resource, value);
        if (!value) {
            status = KAPS_INVALID_ARGUMENT;
            break;
        }
        *value = db->db_info.hw_res.db_res->start_lpu;
        break;
    }
    default:
        status = KAPS_INVALID_ARGUMENT;
        break;
    }
    va_end(vl);
    return status;
}

kaps_status kaps_dma_db_set_property(struct kaps_dma_db *db, enum kaps_db_properties property, ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;

    if (db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->is_config_locked)
        return KAPS_DB_ACTIVE;

    if (db->db_info.device->flags & KAPS_DEVICE_ISSU) {
        if (db->db_info.device->issu_status != KAPS_ISSU_INIT &&
            db->db_info.device->issu_status != KAPS_ISSU_RESTORE_START)
            return KAPS_ISSU_IN_PROGRESS;
    }

    va_start(vl, property);
    switch (property) {
    case KAPS_PROP_DESCRIPTION:
    {
        char *desc = va_arg(vl, char *);

        KAPS_TRACE_IN("%p %d %s\n", db, property, desc);
        if (!desc) {
            status = KAPS_INVALID_ARGUMENT;
            break;
        }
        db->db_info.description
            = db->db_info.device->alloc->xcalloc(db->db_info.device->alloc->cookie, 1, (strlen(desc) + 1));
        if (!db->db_info.description) {
            status = KAPS_OUT_OF_MEMORY;
            break;
        }
        strcpy(db->db_info.description, desc);
        break;
    }

    case KAPS_PROP_DMA_TAG:
    {
        uint32_t dma_tag = va_arg(vl, uint32_t);

        KAPS_TRACE_IN("%p %d %u\n", db, property, dma_tag);
        if (dma_tag > KAPS_DMA_TAG_MAX) {
            status = KAPS_INVALID_ARGUMENT;
            break;
        }
        db->dma_tag = dma_tag;
        break;
    }
    default:
        KAPS_TRACE_IN("%p %d\n", db, property);
        status = KAPS_INVALID_ARGUMENT;
        break;
    }
    va_end(vl);
    return status;
}

kaps_status kaps_dma_db_get_property(struct kaps_dma_db *db, enum kaps_db_properties property, ...)
{
    va_list vl;
    kaps_status status = KAPS_OK;

    if (db == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->flags & KAPS_DEVICE_ISSU) {
        if (db->db_info.device->issu_status != KAPS_ISSU_INIT)
            return KAPS_ISSU_IN_PROGRESS;
    }

    va_start(vl, property);
    switch (property) {
    case KAPS_PROP_DESCRIPTION:
    {
        char **desc = va_arg(vl, char **);

        KAPS_TRACE_IN("%p %d\n", db, property);
        if (!desc) {
            status = KAPS_INVALID_ARGUMENT;
            break;
        }
        *desc = db->db_info.description;
        KAPS_TRACE_OUT("%s\n", *desc);
        break;
    }

    case KAPS_PROP_DMA_TAG:
    {
        uint32_t* dma_tag = va_arg(vl, uint32_t*);

        KAPS_TRACE_IN("%p %d\n", db, property);
        if (!dma_tag) {
            status = KAPS_INVALID_ARGUMENT;
            break;
        }
        *dma_tag = db->dma_tag;
        KAPS_TRACE_OUT("%u\n", *dma_tag);
        break;
    }
    default:
        KAPS_TRACE_IN("%p %d\n", db, property);
        status = KAPS_INVALID_ARGUMENT;
        break;
    }
    va_end(vl);
    return status;
}

kaps_status kaps_dma_db_add_entry(struct kaps_dma_db *db, uint8_t *value, uint32_t offset)
{
    int32_t bb_no = -1;
    uint32_t kaps_dma_rows = KAPS_DMA_NUM_ROWS;

    KAPS_TRACE_IN("%p %p %u\n", db, value, offset);
    if (!db || !value)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->flags & KAPS_DEVICE_ISSU) {
        if (db->db_info.device->issu_status != KAPS_ISSU_INIT)
            return KAPS_ISSU_IN_PROGRESS;
    }

    if (offset >= db->db_info.common_info->capacity)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
        kaps_dma_rows = KAPS_DMA_NUM_ROWS * 4;

    while (offset >= kaps_dma_rows) {
        offset -= kaps_dma_rows;
        bb_no = kaps_find_first_bit_set(db->alloc_fbmap, bb_no + 1, db->db_info.device->hw_res->total_num_bb[0] - 1);
    }
    bb_no = kaps_find_first_bit_set(db->alloc_fbmap, bb_no + 1, db->db_info.device->hw_res->total_num_bb[0] - 1);

    KAPS_STRY(kaps_dm_kaps_dma_bb_write(db->db_info.device, bb_no, offset, (db->user_width_1 + 7)/ KAPS_BITS_IN_BYTE, value));

    return KAPS_OK;
}

kaps_status kaps_dma_db_delete_entry(struct kaps_dma_db *db, uint32_t offset)
{
    int32_t bb_no = -1;
    uint8_t data[KAPS_DMA_WIDTH_1 / KAPS_BITS_IN_BYTE];
    uint32_t kaps_dma_rows = KAPS_DMA_NUM_ROWS;

    KAPS_TRACE_IN("%p %u\n", db, offset);
    if (!db)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->flags & KAPS_DEVICE_ISSU) {
        if (db->db_info.device->issu_status != KAPS_ISSU_INIT)
            return KAPS_ISSU_IN_PROGRESS;
    }

    if (offset >= db->db_info.common_info->capacity)
        return KAPS_INVALID_ARGUMENT;


    kaps_memset(&data, 0, KAPS_DMA_WIDTH_1 / KAPS_BITS_IN_BYTE);

    if (db->db_info.device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
        kaps_dma_rows = KAPS_DMA_NUM_ROWS * 4;

    while (offset >= kaps_dma_rows) {
        offset -= kaps_dma_rows;
        bb_no = kaps_find_first_bit_set(db->alloc_fbmap, bb_no + 1, db->db_info.device->hw_res->total_num_bb[0] - 1);
    }
    bb_no = kaps_find_first_bit_set(db->alloc_fbmap, bb_no + 1, db->db_info.device->hw_res->total_num_bb[0] - 1);

    KAPS_STRY(kaps_dm_kaps_dma_bb_write(db->db_info.device, bb_no, offset, (db->user_width_1 + 7)/ KAPS_BITS_IN_BYTE, data));

    return KAPS_OK;
}

kaps_status kaps_dma_db_read_entry(struct kaps_dma_db *db, uint32_t offset, uint8_t * value)
{
    int32_t bb_no = -1;
    uint32_t kaps_dma_rows = KAPS_DMA_NUM_ROWS;

    KAPS_TRACE_IN("%p %u %p\n", db, offset, value);
    if (!db || !value)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->flags & KAPS_DEVICE_ISSU) {
        if (db->db_info.device->issu_status != KAPS_ISSU_INIT)
            return KAPS_ISSU_IN_PROGRESS;
    }

    if (offset >= db->db_info.common_info->capacity)
        return KAPS_INVALID_ARGUMENT;

    if (db->db_info.device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
        kaps_dma_rows = KAPS_DMA_NUM_ROWS * 4;

    while (offset >= kaps_dma_rows) {
        offset -= kaps_dma_rows;
        bb_no = kaps_find_first_bit_set(db->alloc_fbmap, bb_no + 1, db->db_info.device->hw_res->total_num_bb[0] - 1);
    }
    bb_no = kaps_find_first_bit_set(db->alloc_fbmap, bb_no + 1, db->db_info.device->hw_res->total_num_bb[0] - 1);

    KAPS_STRY(kaps_dm_kaps_dma_bb_read(db->db_info.device, bb_no, offset, (db->user_width_1 + 7)/ KAPS_BITS_IN_BYTE, value));

    return KAPS_OK;
}

kaps_status kaps_dma_db_refresh_handle(struct kaps_device *device, struct kaps_dma_db *stale_ptr, struct kaps_dma_db **dbp)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;
    struct kaps_dma_db *db;

    KAPS_TRACE_IN("%p %p %p\n", device, stale_ptr, dbp);
    if (!(device->flags & KAPS_DEVICE_ISSU))
        return KAPS_INVALID_FLAGS;

    if (device->issu_status == KAPS_ISSU_INIT)
        return KAPS_INVALID_ARGUMENT;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_c_list_iter_next(&it)) != NULL) {
        struct kaps_db *tmp = KAPS_SSDBLIST_TO_ENTRY(e1);

        if (tmp->type == KAPS_DB_DMA) {
            db = (struct kaps_dma_db*)tmp;
            if (db->db_info.stale_ptr == (uintptr_t) stale_ptr) {
                *dbp = db;
                KAPS_TRACE_OUT("%p\n", *dbp);
                return KAPS_OK;
            }
        }
    }
    KAPS_TRACE_OUT("%p\n", 0);
    return KAPS_INVALID_ARGUMENT;
}

kaps_status kaps_dma_db_wb_save_info(struct kaps_db *db,
                                   struct kaps_wb_cb_functions *wb_fun)
{
    struct kaps_dma_db *dma_db = (struct kaps_dma_db *) db;
    struct kaps_dma_db_wb_info *wb_info_ptr;
    int32_t total_size;

    total_size = sizeof(*wb_info_ptr);
    if (db->description)
        total_size += strlen(db->description) + 1;

    if (wb_fun->nv_ptr == NULL) {
        wb_info_ptr = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, total_size);
        if (!wb_info_ptr)
            return KAPS_OUT_OF_MEMORY;

    } else {
        wb_info_ptr = (struct kaps_dma_db_wb_info *) wb_fun->nv_ptr;
        kaps_memset(wb_info_ptr, 0, total_size);
    }

    wb_info_ptr->stale_ptr = (uintptr_t) db;
    wb_info_ptr->bb_alloc = -1;
    wb_info_ptr->capacity = db->common_info->capacity;
    if (db->description) {
        wb_info_ptr->desc = strlen(db->description) + 1;
        kaps_memcpy(wb_info_ptr->description,
                   db->description, strlen(db->description));
    }

    wb_info_ptr->dma_tag = dma_db->dma_tag;
    wb_info_ptr->tid = db->tid;
    wb_info_ptr->user_width_1 = dma_db->user_width_1;

    if (db->common_info->user_specified)
        wb_info_ptr->bb_alloc = db->hw_res.db_res->start_lpu;

    if (wb_fun->write_fn) {
        if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t*) wb_info_ptr,
                                  total_size, *wb_fun->nv_offset)) {
            db->device->alloc->xfree(db->device->alloc->cookie,
                                     wb_info_ptr);
            return KAPS_NV_READ_WRITE_FAILED;
        }
        db->device->alloc->xfree(db->device->alloc->cookie,
                                 wb_info_ptr);
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
    if (wb_fun->nv_ptr)
        wb_fun->nv_ptr += total_size;

    return KAPS_OK;
}

kaps_status kaps_dma_db_wb_restore_info(struct kaps_device *device,
                                      struct kaps_wb_cb_functions *wb_fun)
{
    struct kaps_dma_db *dma_db;
    struct kaps_dma_db_wb_info wb_info, *wb_info_ptr;
    int32_t total_size;

    if (wb_fun->read_fn) {
        /* Read to identify the size of description if it exists */
        wb_info_ptr = &wb_info;
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t*) wb_info_ptr,
                                 sizeof(*wb_info_ptr), *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;

        total_size = sizeof(*wb_info_ptr) + wb_info_ptr->desc;
        wb_info_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, total_size);
        if (!wb_info_ptr)
            return KAPS_OUT_OF_MEMORY;

        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t*) wb_info_ptr,
                                 sizeof(*wb_info_ptr), *wb_fun->nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
    } else {
        wb_info_ptr = (struct kaps_dma_db_wb_info *) wb_fun->nv_ptr;
    }

    total_size = sizeof(*wb_info_ptr) + wb_info_ptr->desc;
    KAPS_STRY(kaps_dma_db_init(device, wb_info_ptr->tid,
                             wb_info_ptr->capacity,
                             wb_info_ptr->user_width_1,
                             &dma_db));
    dma_db->db_info.stale_ptr = wb_info_ptr->stale_ptr;

    KAPS_STRY(kaps_dma_db_set_property(dma_db, KAPS_PROP_DMA_TAG,
                                     wb_info_ptr->dma_tag));

    if (wb_info_ptr->bb_alloc != -1) {
        KAPS_STRY(kaps_dma_db_set_resource(dma_db, KAPS_HW_RESOURCE_UDA,
                                         wb_info_ptr->bb_alloc));
    }

    /*
     * Restore the DB description
     */

    if (wb_info_ptr->desc) {
        KAPS_STRY(kaps_dma_db_set_property(dma_db, KAPS_PROP_DESCRIPTION,
                                         wb_info_ptr->description));
    }

    if (wb_fun->read_fn) {
        device->alloc->xfree(device->alloc->cookie, wb_info_ptr);
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
    if (wb_fun->nv_ptr)
        wb_fun->nv_ptr += total_size;

    return KAPS_OK;
}


