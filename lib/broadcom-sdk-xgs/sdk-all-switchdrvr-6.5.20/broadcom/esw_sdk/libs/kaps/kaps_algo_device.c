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

#include <stdio.h>

#include "kaps_simple_dba.h"
#include "kaps_algo_common.h"
#include "kaps_uda_mgr.h"
#include "kaps_key_internal.h"

struct kaps_db_bb_info
{
    int8_t start_lpu;
    int8_t end_lpu;
    int8_t lsn_size;
};

static kaps_status
kaps_fill_lpm_granularities(
    struct kaps_device *device)
{
    /*
     * KAPS granularities
     */
    uint16_t lsn_grans_kaps[] = { 8, 16, 24, 32, 40, 48, 56, 72, 96, 136, 168 };

    uint16_t lsn_grans_kaps_j2p[] = { 8, 12, 16, 20, 24, 28, 32, 40, 48, 56, 60, 72, 96, 136, 168 };

    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        if (device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_J2P_THREE_LEVEL)
        {
            /*
             * Copy the LSN granularities for J2C+
             */
            device->hw_res->lpm_num_gran = 15;
            kaps_memcpy(device->hw_res->lpm_gran_array, lsn_grans_kaps_j2p, sizeof(lsn_grans_kaps_j2p));

            /*
             * Copy the middle level granularities for J2C+
             */
            device->hw_res->lpm_middle_level_num_gran = 11;
            kaps_memcpy(device->hw_res->lpm_middle_level_gran_array, lsn_grans_kaps, sizeof(lsn_grans_kaps));

        }
        else
        {
            /*
             * Copy the LSN granularities for other J2 KAPS devices
             */
            device->hw_res->lpm_num_gran = 11;
            kaps_memcpy(device->hw_res->lpm_gran_array, lsn_grans_kaps, sizeof(lsn_grans_kaps));

            /*
             * Copy the middle level granularities for other J2 KAPS devices
             */
            device->hw_res->lpm_middle_level_num_gran = 11;
            kaps_memcpy(device->hw_res->lpm_middle_level_gran_array, lsn_grans_kaps, sizeof(lsn_grans_kaps));

        }
    }
    else
    {
        /*
         * Copy the LSN granularities for JR KAPS devices
         */
        device->hw_res->lpm_num_gran = 11;
        kaps_memcpy(device->hw_res->lpm_gran_array, lsn_grans_kaps, sizeof(lsn_grans_kaps));

        device->hw_res->lpm_middle_level_num_gran = 0;
    }

    return NLMERR_OK;
}

kaps_status
kaps_device_check_lpm_constraints_single_device(
    struct kaps_device * device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;
    struct kaps_db *parent, *db;
    uint32_t can_add_extra_byte;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
    {
        parent = KAPS_SSDBLIST_TO_ENTRY(e1);

        if (parent->type != KAPS_DB_LPM)
            continue;

        /*
         * In LPM without overflow number of DBA DT is zero 
         */
        if (device->hw_res->no_overflow_lpm)
        {
            parent->hw_res.db_res->num_dba_dt = 0;
        }

        can_add_extra_byte = 1;
        for (db = parent; db; db = db->next_tab)
        {
            /*
             * Extra byte can be added only if all the databases are <= 152b
             */
            if (db->key->width_1 > KAPS_LPM_KEY_MAX_WIDTH_1 - KAPS_BITS_IN_BYTE)
                can_add_extra_byte = 0;

        }

        if (device->type == KAPS_DEVICE_KAPS)
            can_add_extra_byte = 0;

        if (can_add_extra_byte)
            parent->common_info->is_extra_byte_added = 1;
    }

    return KAPS_OK;
}

kaps_status
kaps_device_check_lpm_constraints(
    struct kaps_device * device)
{
    while (device)
    {
        KAPS_STRY(kaps_device_check_lpm_constraints_single_device(device));
        device = device->next_bc_device;
    }
    return KAPS_OK;
}

static kaps_status
kaps_destroy_kaps_shadow(
    struct kaps_device *device)
{
    uint32_t i;
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    uint32_t ads_depth;

    if (device->nv_ptr)
    {
        device->alloc->xfree(device->alloc->cookie, shadow);
        return KAPS_OK;
    }

    if (shadow)
    {
        if (shadow->rpb_blks)
        {
            for (i = 0; i < device->hw_res->num_rpt_blocks; ++i)
            {
                device->alloc->xfree(device->alloc->cookie, shadow->rpb_blks[i].rpb_rows);
            }

            device->alloc->xfree(device->alloc->cookie, shadow->rpb_blks);
        }

        if (shadow->ads_blks)
        {
            for (i = 0; i < device->hw_res->num_rpt_blocks; ++i)
            {
                device->alloc->xfree(device->alloc->cookie, shadow->ads_blks[i].ads_rows);
            }

            device->alloc->xfree(device->alloc->cookie, shadow->ads_blks);
        }

        if (shadow->bkt_blks)
        {
            for (i = 0; i < device->hw_res->total_num_bb; ++i)
            {
                if (shadow->bkt_blks[i].bkt_rows)
                {
                    device->alloc->xfree(device->alloc->cookie, shadow->bkt_blks[i].bkt_rows);
                }
            }
            device->alloc->xfree(device->alloc->cookie, shadow->bkt_blks);
        }

        if (shadow->small_bbs)
        {
            for (i = 0; i < device->hw_res->total_small_bb; ++i)
            {
                if (shadow->small_bbs[i].bkt_rows)
                {
                    device->alloc->xfree(device->alloc->cookie, shadow->small_bbs[i].bkt_rows);
                }
            }
            device->alloc->xfree(device->alloc->cookie, shadow->small_bbs);
        }

        if (shadow->ads2_blks)
        {
            for (i = 0; i < KAPS_NUM_ADS2_BLOCKS; ++i)
            {
                if (shadow->ads2_blks[i].ads_rows)
                {
                    device->alloc->xfree(device->alloc->cookie, shadow->ads2_blks[i].ads_rows);
                }
            }

            device->alloc->xfree(device->alloc->cookie, shadow->ads2_blks);
        }


        if (shadow->ads2_overlay)
        {
            for (i = 0; i < KAPS_NUM_ADS2_BLOCKS; ++i)
            {
                ads_depth = device->hw_res->ads2_depth_a;
                if (i > 0)
                    ads_depth = device->hw_res->ads2_depth_b;

                if (ads_depth)
                    kaps_free_bmp(&shadow->ads2_overlay[i].it_fbmp, device->alloc);
            }

            device->alloc->xfree(device->alloc->cookie, shadow->ads2_overlay);
        }

        if (shadow->ab_to_small_bb)
        {
            device->alloc->xfree(device->alloc->cookie, shadow->ab_to_small_bb);
        }


        device->alloc->xfree(device->alloc->cookie, shadow);
    }
    return KAPS_OK;
}

static kaps_status
kaps_initialize_kaps_shadow(
    struct kaps_device *device)
{
    uint32_t i, j;
    kaps_status status;

    struct kaps_shadow_device *shadow = device->alloc->xcalloc(device->alloc->cookie, 1,
                                                               sizeof(struct kaps_shadow_device));

    if (!shadow)
        return KAPS_OUT_OF_MEMORY;

    shadow->rpb_blks 
        = device->alloc->xcalloc(device->alloc->cookie, device->hw_res->num_rpt_blocks, sizeof(struct kaps_shadow_rpb));

    if (!shadow->rpb_blks)
    {
    	kaps_destroy_kaps_shadow(device);
    	return KAPS_OUT_OF_MEMORY;
    }

    for (i = 0; i < device->hw_res->num_rpt_blocks; ++i)
    {
        shadow->rpb_blks[i].rpb_rows = device->alloc->xcalloc(device->alloc->cookie, device->num_80b_entries_ab / 2,
                                        sizeof(struct kaps_rpb_entry));

        if (!shadow->rpb_blks[i].rpb_rows)
        {
            kaps_destroy_kaps_shadow(device);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    shadow->ads_blks
        =  device->alloc->xcalloc(device->alloc->cookie, device->hw_res->num_rpt_blocks, sizeof(struct kaps_shadow_ads));

    if (!shadow->ads_blks)
    {
        kaps_destroy_kaps_shadow(device);
        return KAPS_OUT_OF_MEMORY;
    }

    for (i = 0; i < device->hw_res->num_rpt_blocks; ++i)
    {
        shadow->ads_blks[i].ads_rows
            = device->alloc->xcalloc(device->alloc->cookie, device->num_80b_entries_ab / 2,
                                sizeof(struct kaps_ads));

        if (!shadow->ads_blks[i].ads_rows)
        {
            kaps_destroy_kaps_shadow(device);
            return KAPS_OUT_OF_MEMORY;
        }
    }


    shadow->bkt_blks
        = device->alloc->xcalloc(device->alloc->cookie, device->hw_res->total_num_bb, sizeof(struct kaps_shadow_bkt));

    if (!shadow->bkt_blks)
    {
        kaps_destroy_kaps_shadow(device);
        return KAPS_OUT_OF_MEMORY;
    }

    for (i = 0; i < device->hw_res->total_num_bb; ++i)
    {
        shadow->bkt_blks[i].bkt_rows = device->alloc->xcalloc(device->alloc->cookie,
                                                              device->hw_res->rows_in_lpu,
                                                              sizeof(struct kaps_shadow_bkt_row));

        if (!shadow->bkt_blks[i].bkt_rows)
        {
            kaps_destroy_kaps_shadow(device);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    if (device->hw_res->total_small_bb)
    {
        shadow->small_bbs = device->alloc->xcalloc(device->alloc->cookie, device->hw_res->total_small_bb,
                                                   sizeof(struct kaps_shadow_bkt));
        if (!shadow->small_bbs)
        {
            kaps_destroy_kaps_shadow(device);
            return KAPS_OUT_OF_MEMORY;
        }

        for (i = 0; i < device->hw_res->total_small_bb; ++i)
        {
            shadow->small_bbs[i].bkt_rows = device->alloc->xcalloc(device->alloc->cookie,
                                                                   device->hw_res->num_rows_in_small_bb,
                                                                   sizeof(struct kaps_shadow_bkt_row));
            if (!shadow->small_bbs[i].bkt_rows)
            {
                kaps_destroy_kaps_shadow(device);
                return KAPS_OUT_OF_MEMORY;
            }
        }
    }

    if (device->id == KAPS_JERICHO_2_DEVICE_ID && device->hw_res->num_algo_levels == 3)
    {
        shadow->ads2_blks = device->alloc->xcalloc(device->alloc->cookie, KAPS_NUM_ADS2_BLOCKS,
                            sizeof(struct kaps_shadow_ads2));

        if (!shadow->ads2_blks)
        {
            kaps_destroy_kaps_shadow(device);
            return KAPS_OUT_OF_MEMORY;
        }

        for (i = 0; i < KAPS_NUM_ADS2_BLOCKS; ++i)
        {
            shadow->ads2_blks[i].ads_rows = device->alloc->xcalloc(device->alloc->cookie, KAPS_ADS2_MAX_NUM_ROWS,
                                sizeof(struct kaps_ads));

            if (!shadow->ads2_blks[i].ads_rows)
            {
                kaps_destroy_kaps_shadow(device);
                return KAPS_OUT_OF_MEMORY;
            }

        }

        /*
         * The size of the ADS2 translation table will be greater than the ADS2 depth
         */
        shadow->ads2_overlay = device->alloc->xcalloc(device->alloc->cookie,
                                                      KAPS_NUM_ADS2_BLOCKS, sizeof(struct kaps_shadow_ads2_overlay));

        if (!shadow->ads2_overlay)
        {
            kaps_destroy_kaps_shadow(device);
            return KAPS_OUT_OF_MEMORY;
        }

        for (i = 0; i < KAPS_NUM_ADS2_BLOCKS; ++i)
        {
            /*
             * The size of the bitmap should be equal to the ADS2 depth
             */
            uint32_t ads_depth = device->hw_res->ads2_depth_a;

            if (i > 0)
                ads_depth = device->hw_res->ads2_depth_b;


            if (ads_depth) 
            {
                status = kaps_init_bmp(&shadow->ads2_overlay[i].it_fbmp, device->alloc, ads_depth, 1);

                if (status != KAPS_OK)
                {
                    kaps_destroy_kaps_shadow(device);
                    return KAPS_OUT_OF_MEMORY;
                }
            }

            shadow->ads2_overlay[i].num_free_it_slots = ads_depth;

            /*
             * Initialize the translation table to indicate that initial state
             */
            for (j = 0; j < KAPS_ADS2_X_TABLE_SIZE; ++j)
            {
                shadow->ads2_overlay[i].x_table[j] = -1;
            }

        }

        shadow->ab_to_small_bb = device->alloc->xcalloc(device->alloc->cookie, KAPS_MAX_NUM_POOLS,
                                                        sizeof(struct kaps_shadow_ab_to_small_bb));

        if (!shadow->ab_to_small_bb)
        {
            kaps_destroy_kaps_shadow(device);
            return KAPS_OUT_OF_MEMORY;
        }

    }

    device->kaps_shadow = shadow;
    return KAPS_OK;
}

kaps_status
kaps_initialize_advanced_shadow(
    struct kaps_device * device)
{
    uint32_t i, j;
    uint32_t num_160b_rpb_rows;
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    kaps_status status;

    if (device->nv_ptr)
    {
        shadow->rpb_blks = (struct kaps_shadow_rpb *) device->nv_ptr;
        device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr + (device->num_ab * sizeof(struct kaps_shadow_rpb)));

        num_160b_rpb_rows = device->num_80b_entries_ab / 2;

        for (i = 0; i < device->num_ab; ++i)
        {
            shadow->rpb_blks[i].rpb_rows = (struct kaps_rpb_entry *) device->nv_ptr;
            device->nv_ptr =
                (void *) ((uint8_t *) device->nv_ptr + (num_160b_rpb_rows * sizeof(struct kaps_rpb_entry)));
        }

        shadow->ads_blks = (struct kaps_shadow_ads *) device->nv_ptr;
        device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr + (device->num_ab * sizeof(struct kaps_shadow_ads)));

        for (i = 0; i < device->num_ab; ++i)
        {
            shadow->ads_blks[i].ads_rows = (struct kaps_ads *) device->nv_ptr;
            device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr + (num_160b_rpb_rows * sizeof(struct kaps_ads)));
        }

        if (shadow->bkt_blks)
        {
            for (i = 0; i < device->hw_res->total_num_bb; ++i)
            {
                if (shadow->bkt_blks[i].bkt_rows)
                {
                    device->alloc->xfree(device->alloc->cookie, shadow->bkt_blks[i].bkt_rows);
                }
            }
            device->alloc->xfree(device->alloc->cookie, shadow->bkt_blks);
        }

        shadow->bkt_blks = (struct kaps_shadow_bkt *) device->nv_ptr;
        device->nv_ptr =
            (void *) ((uint8_t *) device->nv_ptr + (device->hw_res->total_num_bb * sizeof(struct kaps_shadow_bkt)));

        for (i = 0; i < device->hw_res->total_num_bb; ++i)
        {
            shadow->bkt_blks[i].bkt_rows = (struct kaps_shadow_bkt_row *) device->nv_ptr;
            device->nv_ptr =
                (void *) ((uint8_t *) device->nv_ptr +
                          (device->hw_res->rows_in_lpu * sizeof(struct kaps_shadow_bkt_row)));
        }

        if (device->id == KAPS_JERICHO_2_DEVICE_ID && device->hw_res->num_algo_levels == 3)
        {

            /*
             * Release the old allocated memory for the ADS2 overlay
             */
            if (shadow->ads2_overlay)
            {
                for (i = 0; i < KAPS_NUM_ADS2_BLOCKS; ++i)
                {
                    kaps_free_bmp(&shadow->ads2_overlay->it_fbmp, device->alloc);
                }

                device->alloc->xfree(device->alloc->cookie, shadow->ads2_overlay);
            }

            shadow->ads2_overlay = (struct kaps_shadow_ads2_overlay *) device->nv_ptr;
            device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr +
                                       (KAPS_NUM_ADS2_BLOCKS * sizeof(struct kaps_shadow_ads2_overlay)));

            for (i = 0; i < KAPS_NUM_ADS2_BLOCKS; ++i)
            {
                /*
                 * The size of the bitmap should be equal to the ADS2 depth
                 */
                uint32_t ads_depth = device->hw_res->ads2_depth_a;

                if (i > 0)
                    ads_depth = device->hw_res->ads2_depth_b;

                if (ads_depth)
                {
                    status = kaps_init_bmp(&shadow->ads2_overlay[i].it_fbmp, device->alloc, ads_depth, 1);

                    if (status != KAPS_OK)
                    {
                        return KAPS_OUT_OF_MEMORY;
                    }
                }

                shadow->ads2_overlay[i].num_free_it_slots = ads_depth;

                /*
                 * Initialize the translation table with -1
                 */
                for (j = 0; j < KAPS_ADS2_X_TABLE_SIZE; ++j)
                {
                    shadow->ads2_overlay[i].x_table[j] = -1;
                }
            }

            /*
             * Release the memory allocated for ab_to_small_bb
             */
            device->alloc->xfree(device->alloc->cookie, shadow->ab_to_small_bb);

            /*
             * Acquire memory for ab_to_small_bb mapping
             */
            shadow->ab_to_small_bb = (struct kaps_shadow_ab_to_small_bb *) device->nv_ptr;
            device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr +
                                       (KAPS_MAX_NUM_POOLS * sizeof(struct kaps_shadow_ab_to_small_bb)));
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_device_save_kaps_shadow(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    int32_t i;
    int32_t size_of_bb;

    kaps_assert(shadow->bkt_blks, "Shadow NULL!!\n");

    if (wb_fun->write_fn)
    {
        if (device->id == KAPS_JERICHO_2_DEVICE_ID)
        {
            size_of_bb = device->hw_res->num_rows_in_small_bb * sizeof(struct kaps_shadow_bkt_row);

            for (i = 0; i < device->hw_res->total_small_bb; ++i)
            {
                if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) shadow->small_bbs[i].bkt_rows,
                                          size_of_bb, *wb_fun->nv_offset))
                {
                    return KAPS_NV_READ_WRITE_FAILED;
                }
                *wb_fun->nv_offset = *wb_fun->nv_offset + size_of_bb;
            }

            if (device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL)
            {
                return KAPS_OK;
            }
        }

        size_of_bb = device->hw_res->rows_in_lpu * sizeof(struct kaps_shadow_bkt_row);

        for (i = 0; i < device->hw_res->total_num_bb; ++i)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) shadow->bkt_blks[i].bkt_rows,
                                      size_of_bb, *wb_fun->nv_offset))
            {
                return KAPS_NV_READ_WRITE_FAILED;
            }
            *wb_fun->nv_offset = *wb_fun->nv_offset + size_of_bb;
        }

        if (device->id == KAPS_JERICHO_2_DEVICE_ID && device->hw_res->num_algo_levels == 3)
        {

            /*
             * Save the ADS-2 translation table for Large KAPS
             */
            for (i = 0; i < KAPS_NUM_ADS2_BLOCKS; ++i)
            {
                if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) device->kaps_shadow->ads2_overlay[i].x_table,
                                          sizeof(int32_t) * KAPS_ADS2_X_TABLE_SIZE, *wb_fun->nv_offset))
                {
                    return KAPS_NV_READ_WRITE_FAILED;
                }

                *wb_fun->nv_offset += sizeof(int32_t) * KAPS_ADS2_X_TABLE_SIZE;
            }

            /*
             * Save the AB to small BB mapping
             */
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) device->kaps_shadow->ab_to_small_bb,
                                      sizeof(struct kaps_shadow_ab_to_small_bb) * KAPS_MAX_NUM_POOLS,
                                      *wb_fun->nv_offset))
            {
                return KAPS_NV_READ_WRITE_FAILED;
            }

            *wb_fun->nv_offset += sizeof(struct kaps_shadow_ab_to_small_bb) * KAPS_MAX_NUM_POOLS;

        }
    }

    return KAPS_OK;
}

kaps_status
kaps_device_restore_kaps_shadow(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    int32_t i, j;
    int32_t size_of_bb;

    kaps_assert(shadow->bkt_blks, "Shadow NULL!!\n");

    if (wb_fun->read_fn)
    {
        if (device->id == KAPS_JERICHO_2_DEVICE_ID)
        {
            size_of_bb = device->hw_res->num_rows_in_small_bb * sizeof(struct kaps_shadow_bkt_row);

            for (i = 0; i < device->hw_res->total_small_bb; ++i)
            {
                if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) shadow->small_bbs[i].bkt_rows,
                                         size_of_bb, *wb_fun->nv_offset))
                {
                    return KAPS_NV_READ_WRITE_FAILED;
                }
                *wb_fun->nv_offset = *wb_fun->nv_offset + size_of_bb;
            }

            if (device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL)
            {
                return KAPS_OK;
            }
        }

        size_of_bb = device->hw_res->rows_in_lpu * sizeof(struct kaps_shadow_bkt_row);

        for (i = 0; i < device->hw_res->total_num_bb; ++i)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) shadow->bkt_blks[i].bkt_rows,
                                     size_of_bb, *wb_fun->nv_offset))
            {
                return KAPS_NV_READ_WRITE_FAILED;
            }
            *wb_fun->nv_offset = *wb_fun->nv_offset + size_of_bb;
        }

        if (device->id == KAPS_JERICHO_2_DEVICE_ID && device->hw_res->num_algo_levels == 3)
        {

            /*
             * Restore the ADS-2 translation table for Large KAPS
             */
            for (i = 0; i < KAPS_NUM_ADS2_BLOCKS; ++i)
            {
                if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) device->kaps_shadow->ads2_overlay[i].x_table,
                                         sizeof(int32_t) * KAPS_ADS2_X_TABLE_SIZE, *wb_fun->nv_offset))
                {
                    return KAPS_NV_READ_WRITE_FAILED;
                }

                *wb_fun->nv_offset += sizeof(int32_t) * KAPS_ADS2_X_TABLE_SIZE;

                /*
                 * Recompute the IT bitmap bits
                 */
                for (j = 0; j < KAPS_ADS2_X_TABLE_SIZE; ++j)
                {
                    int32_t actual_it_pos = device->kaps_shadow->ads2_overlay[i].x_table[j];

                    if (actual_it_pos != -1)
                    {
                        /*
                         * actual_bit_pos is in use. So reset it in the bitmap
                         */
                        kaps_reset_bit(&device->kaps_shadow->ads2_overlay[i].it_fbmp, actual_it_pos);

                        if (device->kaps_shadow->ads2_overlay[i].num_free_it_slots)
                        {
                            device->kaps_shadow->ads2_overlay[i].num_free_it_slots--;
                        }
                        else
                        {
                            kaps_assert(0, "Incorrect number of IT slots in ADS-2 overlay");
                        }
                    }
                }
            }

            /*
             * Restore the AB to small BB mapping
             */
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) device->kaps_shadow->ab_to_small_bb,
                                     sizeof(struct kaps_shadow_ab_to_small_bb) * KAPS_MAX_NUM_POOLS,
                                     *wb_fun->nv_offset))
            {
                return KAPS_NV_READ_WRITE_FAILED;
            }

            *wb_fun->nv_offset += sizeof(struct kaps_shadow_ab_to_small_bb) * KAPS_MAX_NUM_POOLS;

        }
    }

    return KAPS_OK;
}

kaps_status
kaps_device_advanced_init(
    struct kaps_device * device)
{
    struct kaps_device *tmp;
    struct kaps_device_resource *hw_res = NULL;

    device->num_ab_available = -1;
    device->num_uda_available = -1;

    switch (device->type)
    {

        case KAPS_DEVICE_KAPS:
            hw_res = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(*hw_res));
            if (hw_res == NULL)
                return KAPS_OUT_OF_MEMORY;
            device->hw_res = hw_res;
            hw_res->num_rpt_blocks = 4;
            hw_res->total_lpus = 16;
            hw_res->max_lpu_per_db = 16;
            hw_res->indirection_width_1 = 20;
            hw_res->rows_in_lpu = 1024;

            hw_res->total_num_bb = 32;
            hw_res->num_chained_bb = 2;

            hw_res->num_algo_levels = 2;

            device->uda_offset = 5;

            if (device->id == KAPS_QUMRAN_DEVICE_ID)
            {
                hw_res->total_num_bb = 16;
                hw_res->num_chained_bb = 1;
            }

            if (device->id == KAPS_QUX_DEVICE_ID)
            {
                hw_res->total_num_bb = 16;
                hw_res->num_chained_bb = 1;
                hw_res->rows_in_lpu = 256;
            }

            if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
            {
                /*
                 * Set the number of rows in the LPU
                 */
                hw_res->rows_in_lpu = 4 * 1024;

                if (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0)
                {
                    hw_res->rows_in_lpu = 2 * 1024;
                }
                else if (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
                {
                    hw_res->rows_in_lpu = (2 * 1024) + 256;
                }

                hw_res->max_lpu_per_db = 16;
            }

            if (device->id == KAPS_JERICHO_2_DEVICE_ID)
            {
                hw_res->num_rpt_blocks = 8;
                
                hw_res->rows_in_lpu = 16 * 1024;
                hw_res->total_num_bb = 16;
                hw_res->num_chained_bb = 1;

                hw_res->total_small_bb = 8 * 4;
                hw_res->num_rows_in_small_bb = 512;

                hw_res->num_algo_levels = 2;

                device->small_bb_offset = 10;
                device->uda_offset = 44;

                if (device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_THREE_LEVEL
                    || device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_Q2A_THREE_LEVEL
                    || device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_J2P_THREE_LEVEL)
                {
                    hw_res->num_algo_levels = 3;

                    hw_res->pc_width_8 = 20;
                    hw_res->pc_depth = 1024;

                    device->num_ab = 2 * 1024;

                    hw_res->total_lpus = 28;
                    hw_res->total_num_bb = 28;

                    /*
                     * Read the ADS-2 depths. For some profiles which have only 1 database, ADS2-1 may not exist. In
                     * this case, DNX will pass back the number of rows as 0 */
                    KAPS_STRY(kaps_dm_kaps_read_ads_depth(device, 0, &device->hw_res->ads2_depth_a));
                    
                    KAPS_STRY(kaps_dm_kaps_read_ads_depth(device, 1, &device->hw_res->ads2_depth_b));
                    
                }
            }

            hw_res->sram_dt_per_db = 1;
            hw_res->bb_width_1 = KAPS_BKT_WIDTH_1;
            hw_res->has_algorithmic_lpm = 1;
            hw_res->no_dba_compression = 1;
            kaps_initialize_kaps_shadow(device);
            break;

        default:
            return KAPS_INVALID_DEVICE_TYPE;
    }

    for (tmp = device; tmp; tmp = tmp->next_dev)
        kaps_fill_lpm_granularities(tmp);

    /*
     * Ensure the limits in the hw_limits.h header
     * file are greater than or equal to that
     * discovered on the device
     */
    kaps_sassert((hw_res->sram_dt_per_db + hw_res->dba_dt_per_db) <= KAPS_HW_MAX_DT_PER_DB);
    kaps_sassert(hw_res->max_lpu_per_db <= KAPS_HW_MAX_LPUS_PER_DB);
    return KAPS_OK;
}

kaps_status
kaps_device_advanced_destroy(
    struct kaps_device * device)
{
    if (device->type == KAPS_DEVICE_KAPS)
        kaps_destroy_kaps_shadow(device);

    if (device->hw_res)
        device->alloc->xfree(device->alloc->cookie, device->hw_res);
    return KAPS_OK;
}

kaps_status
kaps_device_advanced_set_property(
    struct kaps_device * device,
    uint32_t property,
    va_list a_list)
{
    kaps_status status = KAPS_OK;
    uint32_t value;

    switch (property)
    {

        case KAPS_DEVICE_PROP_RESOURCE_FN:
            device->hw_res->process_fn = va_arg(a_list, resource_process_fn);
            KAPS_TRACE_PRINT(" %p", device->hw_res->process_fn);
            device->hw_res->res_hdl = va_arg(a_list, void *);
            KAPS_TRACE_PRINT(" %p", device->hw_res->res_hdl);

            if (!device->hw_res->process_fn)
            {
                status = KAPS_INVALID_ARGUMENT;
            }
            break;

        case KAPS_DEVICE_PROP_TOTAL_NUM_BB:
            if (device->type == KAPS_DEVICE_KAPS)
            {
                value = va_arg(a_list, uint32_t);
                KAPS_TRACE_PRINT(" %d", val);

                if (device->id == KAPS_JERICHO_2_DEVICE_ID)
                {
                    if (device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL)
                    {
                        device->hw_res->total_small_bb = value;
                    }
                    else
                    {
                        device->hw_res->total_num_bb = 2 * value;
                    }
                    device->hw_res->num_chained_bb = 1;
                }
                else
                {
                    if (value % KAPS_DEFAULT_MIN_NUM_BB == 0
                        && value >= KAPS_DEFAULT_MIN_NUM_BB && value <= KAPS_MAX_NUM_BB)
                    {
                        device->hw_res->total_num_bb = value;
                        device->hw_res->num_chained_bb = value / KAPS_DEFAULT_MIN_NUM_BB;
                    }
                    else
                    {
                        status = KAPS_INVALID_ARGUMENT;
                    }
                }
            }
            else
                status = KAPS_INVALID_ARGUMENT;
            break;

        case KAPS_DEVICE_PROP_ADV_PRINT:
        {
            struct kaps_device *tmp_dev = NULL;

            value = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT(" %d", value);

            tmp_dev = device;
            if (device->main_bc_device)
                tmp_dev = device->main_bc_device;

            while (tmp_dev)
            {
                tmp_dev->hw_res->device_print_advance = value;
                tmp_dev = tmp_dev->next_bc_device;
            }
            break;
        }

        default:
            status = KAPS_INVALID_ARGUMENT;
            break;
    }

    return status;
}

kaps_status
kaps_device_advanced_get_property(
    struct kaps_device * device,
    uint32_t property,
    va_list a_list)
{
    kaps_status status = KAPS_OK;
    resource_process_fn *fn;
    uint32_t *value;

    switch (property)
    {

        case KAPS_DEVICE_PROP_RESOURCE_FN:
            fn = va_arg(a_list, resource_process_fn *);
            if (!fn)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            *fn = device->hw_res->process_fn;
            break;

        case KAPS_DEVICE_PROP_TOTAL_NUM_BB:
            if (device->type == KAPS_DEVICE_KAPS)
            {
                value = va_arg(a_list, uint32_t *);
                if (value)
                {
                    *value = device->hw_res->total_num_bb;
                }
                else
                    status = KAPS_INVALID_ARGUMENT;
            }
            else
                status = KAPS_INVALID_ARGUMENT;
            break;

        default:
            status = KAPS_INVALID_ARGUMENT;
            break;
    }

    return status;
}

kaps_status
kaps_device_save_two_level_bb_info(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;

    if (wb_fun->nv_ptr == NULL)
    {
        kaps_sassert (wb_fun->write_fn);
        kaps_c_list_iter_init(&device->db_list, &it);

        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(e1);
            struct kaps_db_bb_info db_bb_info;

            if (db->type == KAPS_DB_LPM)
            {
                if (db->num_algo_levels_in_db == 3)
                    continue;

                db_bb_info.start_lpu = db->hw_res.db_res->start_lpu;
                db_bb_info.end_lpu = db->hw_res.db_res->end_lpu;
                db_bb_info.lsn_size = db->hw_res.db_res->lsn_info[0].max_lsn_size;

                if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & db_bb_info,
                                          sizeof(db_bb_info), *wb_fun->nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;

                *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(struct kaps_db_bb_info);
            }
        }
    }


    return KAPS_OK;
}

kaps_status
kaps_device_restore_two_level_bb_info(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * wb_fun)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *e1;


    if (wb_fun->nv_ptr == NULL)
    {
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(e1);
            struct kaps_db_bb_info db_bb_info;
            int32_t bb_index;

            if (db->type == KAPS_DB_LPM && wb_fun->read_fn)
            {
                if (db->num_algo_levels_in_db == 3)
                    continue;

                if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & db_bb_info,
                                         sizeof(db_bb_info), *wb_fun->nv_offset))
                    return KAPS_NV_READ_WRITE_FAILED;

                db->hw_res.db_res->start_lpu = db_bb_info.start_lpu;
                db->hw_res.db_res->end_lpu = db_bb_info.end_lpu;
                db->hw_res.db_res->lsn_info[0].max_lsn_size = db_bb_info.lsn_size;

                *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(struct kaps_db_bb_info);

                for (bb_index = db->hw_res.db_res->start_lpu; bb_index <= db->hw_res.db_res->end_lpu; bb_index++)
                {
                    int32_t bb_blk_id = bb_index * device->hw_res->num_chained_bb;
                    int32_t i = 0;

                    db->hw_res.db_res->lsn_info[0].alloc_udm[db->device->core_id][0][bb_index] = 1;
                    for (i = 0; i < device->hw_res->num_chained_bb; i++)
                    {
                        kaps_sassert(device->map->bb[bb_blk_id].row->owner == NULL);
                        device->map->bb[bb_blk_id].row->owner = db;
                        bb_blk_id++;
                    }
                }
            }
        }
    }


    return KAPS_OK;
}
