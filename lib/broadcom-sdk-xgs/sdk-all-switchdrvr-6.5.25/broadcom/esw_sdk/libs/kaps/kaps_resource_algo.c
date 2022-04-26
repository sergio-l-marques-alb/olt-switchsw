/*******************************************************************************
 *
 * Copyright 2012-2019 Broadcom Corporation
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

#include "kaps_resource_algo.h"
#include "kaps_it_mgr.h"
#include "kaps_simple_dba.h"
#include "kaps_uda_mgr.h"
#include "kaps_key_internal.h"
#include "kaps_ad_internal.h"

kaps_status
kaps_resource_init_mem_map(
    struct kaps_device *device)
{
    if (device->type == KAPS_DEVICE_KAPS)
        KAPS_STRY(kaps_initialize_kaps_mem_map(device));
    else
        return KAPS_UNSUPPORTED;

    return KAPS_OK;
}

struct kaps_db *
kaps_resource_get_res_db(
    struct kaps_db *db)
{
    struct kaps_db *res_db;

    res_db = db;

    if (res_db->parent)
        res_db = res_db->parent;

    if (res_db->type == KAPS_DB_LPM)
    {
        res_db = kaps_lpm_get_res_db(res_db);
    }

    return res_db;
}

void
kaps_resource_fini_module(
    struct kaps_device *device)
{
    int32_t i, num_smt;
    int32_t pcm_dba_mgr_idx;
    uint32_t should_release_mem_map;

    should_release_mem_map = 0;

    /*
     * The memory map is not stored in NV memory for KAPS even when crash recovery is enabled. So release the memory
     * map
     */
    if (device->map && device->type == KAPS_DEVICE_KAPS)
    {
        should_release_mem_map = 1;
    }

    if (device->map && !device->nv_ptr)
    {
        should_release_mem_map = 1;
    }

    if (should_release_mem_map)
    {
        int32_t j, i, ndevices = 0;
        struct kaps_device *tmp;

        for (tmp = device; tmp; tmp = tmp->next_dev)
            ndevices++;

        for (j = 0; j < ndevices; j++)
        {
            struct memory_map *map = &device->map[j];

            for (i = 0; i < map->num_bb * device->hw_res->num_bb_in_one_chain; i++)
            {
                device->alloc->xfree(device->alloc->cookie, map->bb_map[0][i].row);
            }
            
            if (map->bb_map[0])
                device->alloc->xfree(device->alloc->cookie, map->bb_map[0]);

            if (map->ab_memory)
                device->alloc->xfree(device->alloc->cookie, map->ab_memory);

            if (map->rpb_ab)
                device->alloc->xfree(device->alloc->cookie, map->rpb_ab);

        }

        device->alloc->xfree(device->alloc->cookie, device->map);
        device->map = NULL;
    }

    num_smt = 1;
    if (device->smt)
    {
        num_smt = 2;
    }

    for (i = 0; i < num_smt; ++i)
    {
        struct kaps_device *dev;

        if (i == 0)
            dev = device;
        else
            dev = device->smt;

        for (pcm_dba_mgr_idx = 0; pcm_dba_mgr_idx < HW_MAX_PCM_BLOCKS; pcm_dba_mgr_idx++)
        {
            if (dev->hw_res->pcm_dba_mgr[pcm_dba_mgr_idx])
            {
                kaps_simple_dba_destroy(dev->hw_res->pcm_dba_mgr[pcm_dba_mgr_idx]);
                dev->hw_res->pcm_dba_mgr[pcm_dba_mgr_idx] = NULL;
            }
        }
    }

    if (device->hw_res->it_mgr)
    {
        kaps_it_mgr_destroy((struct it_mgr *) device->hw_res->it_mgr);
    }
}

kaps_status
kaps_kaps_resource_set_algorithmic_single_device(
    struct kaps_device *device,
    struct kaps_db *db,
    int32_t value)
{
    struct kaps_db *tmp;

    if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP || db->type == KAPS_DB_HB)
        return KAPS_OK;

    if (value == 0)
    {
        /*
         * Disable Algorithmic 
         */
        db->hw_res.db_res->num_dt = 0;
        db->hw_res.db_res->num_dba_dt = 0;
        for (tmp = db; tmp; tmp = tmp->next_tab)
        {
            tmp->hw_res.db_res->algorithmic = 0;
        }

    }
    else if (value == 1)
    {
        /*
         * LPM and/or O3S light weight (only B silicon) 
         */
        if (db->type == KAPS_DB_LPM)
            db->hw_res.db_res->num_dt = 1;
        else
            db->hw_res.db_res->num_dt = 0;

        db->hw_res.db_res->num_dba_dt = 1;

        for (tmp = db; tmp; tmp = tmp->next_tab)
        {
            tmp->hw_res.db_res->algorithmic = 1;
        }
    }
    else
    {
        return KAPS_INVALID_ARGUMENT;
    }

    return KAPS_OK;
}

kaps_status
kaps_resource_set_algorithmic(
    struct kaps_device * device,
    struct kaps_db * db,
    int32_t value)
{
    struct kaps_device *main_dev, *tmp_dev;
    struct kaps_db *tmp_db;
    uint32_t bc_bmp;

    if (!db->is_bc_required || db->device->issu_in_progress)
    {
        KAPS_STRY(kaps_kaps_resource_set_algorithmic_single_device(device, db, value));
        return KAPS_OK;
    }

    bc_bmp = db->bc_bitmap;
    main_dev = device;
    if (main_dev->main_bc_device)
        main_dev = main_dev->main_bc_device;

    for (tmp_dev = main_dev; tmp_dev; tmp_dev = tmp_dev->next_bc_device)
    {
        if (bc_bmp & (1 << tmp_dev->bc_id))
        {
            tmp_db = kaps_db_get_bc_db_on_device(tmp_dev, db);
            kaps_sassert(tmp_db);
            KAPS_STRY(kaps_kaps_resource_set_algorithmic_single_device(tmp_dev, tmp_db, value));
        }
    }

    return KAPS_OK;
}

void
kaps_resource_set_user_specified_algo_mode(
    struct kaps_db *db)
{
    db->hw_res.db_res->user_specified_algo_type = 1;
}

uint8_t
kaps_resource_get_is_user_specified_algo_mode(
    struct kaps_db *db)
{
    return db->hw_res.db_res->user_specified_algo_type;
}

void
kaps_resource_print_html(
    struct kaps_device *device,
    FILE * f)
{
    /*
     * Print device related assumptions
     */
    if (device->type == KAPS_DEVICE_KAPS)
        kaps_resource_kaps_print_html(device, f);
}


kaps_status
kaps_resource_finalize(
    struct kaps_device *device)
{
    kaps_status status = KAPS_OK;
    struct kaps_device *tmp;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t has_cascaded_db = 0;

    kaps_sassert(device->main_dev == NULL);


    /*
     * All non-algorithmic databases must be switched
     * to MP, and LPM databases with AD must be switched
     * to MP also
     */

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP || db->type == KAPS_DB_DMA
            || db->type == KAPS_DB_HB)
            continue;

        if (db->common_info->is_cascaded)
            has_cascaded_db = 1;

    }

    for (tmp = device; tmp; tmp = tmp->next_dev)
    {
        struct it_mgr *res;

        if (!tmp->hw_res->it_mgr)
        {
            KAPS_STRY(kaps_it_mgr_init(tmp, device->alloc, &res));
            tmp->hw_res->it_mgr = res;

            
            if (has_cascaded_db && !tmp->other_core->hw_res->it_mgr)
            {
                KAPS_STRY(kaps_it_mgr_init(tmp->other_core, device->alloc, &res));
                tmp->other_core->hw_res->it_mgr = res;
            }
        }
    }

    if (device->type == KAPS_DEVICE_KAPS)
        status = kaps_resource_process(device);
    else
        status = KAPS_UNSUPPORTED;

    return status;
}

kaps_status
kaps_resource_dynamic_ad_alloc(
    struct kaps_db * ad,
    uint8_t * sb_bitmap,
    int32_t num_sb_needed)
{
    if (0 && !ad->common_info->ad_info.db->common_info->enable_dynamic_allocation)
    {
        return KAPS_OUT_OF_AD;
    }

    kaps_sassert(0);
    return KAPS_INTERNAL_ERROR;
}



uint32_t
kaps_resource_get_num_unused_uda_bricks(
    struct kaps_device * device,
    struct kaps_db * db)
{
    return 0;
}



uint8_t
kaps_db_get_algorithmic(
    const struct kaps_db * db)
{
    return db->hw_res.db_res->algorithmic;
}

kaps_status
kaps_resource_wb_pre_process(
    struct kaps_device * device)
{
    return KAPS_OK;
}


kaps_status
kaps_resource_expand_uda_mgr_regions(
    struct kaps_uda_mgr *mgr)
{
    if (mgr->db->device->type == KAPS_DEVICE_KAPS)
        return kaps_resource_kaps_expand_uda_mgr_regions(mgr);
    else
        return KAPS_OUT_OF_UDA;
}

kaps_status
kaps_resource_release_udm(
    struct kaps_device * device,
    struct memory_map * mem_map,
    struct kaps_db * db,
    int32_t dt_index,
    int32_t udc_no,
    int32_t udm_no)
{
    (void) mem_map;
    
    db->hw_res.db_res->lsn_info[dt_index].alloc_udm[device->core_id][udm_no][udc_no] = 0;
    kaps_sassert(db->common_info->uda_mb_currently_allocated >= KAPS_HW_MAX_UDM_SIZE_MB);
    db->common_info->uda_mb_currently_allocated -= KAPS_HW_MAX_UDM_SIZE_MB;

    return KAPS_OK;
}

kaps_status
kaps_resource_release_xor_udc(
    struct kaps_device * device,
    struct memory_map * mem_map,
    struct kaps_db * db,
    int32_t dt_index,
    int32_t udc_no)
{

    KAPS_STRY(kaps_resource_release_udm(device, mem_map, db, dt_index, udc_no, 0));

    return KAPS_OK;
}
