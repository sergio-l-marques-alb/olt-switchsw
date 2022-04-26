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

#include "kaps_ab.h"
#include "kaps_uda_mgr.h"
#include "kaps_key_internal.h"
#include "kaps_instruction_internal.h"
#include "kaps_dma_internal.h"
#include "kaps_ad_internal.h"

#define MIN_NUM_SMALL_BB_PER_ROW (4)
#define MAX_NUM_SMALL_BB_PER_ROW (12)

struct large_kaps2_stats
{
    uint32_t total_rpb_rows;
    uint32_t num_rpb_rows_used;

    uint32_t total_small_bb;
    uint32_t num_small_bb_used;

    uint32_t total_ad2_rows;
    uint32_t num_ad2_rows_used;

    uint32_t total_large_bb_rows;
    uint32_t num_large_bb_rows_used;
};

void kaps_kaps_print_detailed_stats_html(
    FILE * f,
    struct kaps_device *dev,
    struct kaps_db *db);

void kaps_ftm_large_kaps2_calc_stats(
    struct kaps_db *db,
    struct large_kaps2_stats *stats);


kaps_status
kaps_jr2_config_small_bb_for_two_level_db(
    struct kaps_db *db)
{
    struct kaps_device *device = db->device;
    uint32_t i;
    uint32_t num_lpu;
    uint32_t db_group_id = db->db_group_id;

    
    db->hw_res.db_res->start_lpu = db->hw_res.db_res->start_small_bb_nr;
    db->hw_res.db_res->end_lpu = db->hw_res.db_res->end_small_bb_nr;

    num_lpu = db->hw_res.db_res->end_lpu - db->hw_res.db_res->start_lpu + 1;

    db->hw_res.db_res->lsn_info[0].max_lsn_size = num_lpu;

    db->common_info->uda_mb = ((float) num_lpu * 
            device->hw_res->num_rows_in_small_bb[db_group_id] * KAPS_BKT_WIDTH_1) / (1024 * 1024);

    db->common_info->total_bb_size_in_bits = num_lpu * 
            device->hw_res->num_rows_in_small_bb[db_group_id] * KAPS_BKT_WIDTH_1;
    
    kaps_memset(db->hw_res.db_res->lsn_info[0].alloc_udm, 0,
                sizeof(db->hw_res.db_res->lsn_info[0].alloc_udm));
    
    for (i = db->hw_res.db_res->start_lpu; i <= db->hw_res.db_res->end_lpu; ++i)
    {
        db->hw_res.db_res->lsn_info[0].alloc_udm[0][0][i] = 1;
    }

    return KAPS_OK;
}


kaps_status
kaps_jr2_small_bb_hw_config(
    struct kaps_device * device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t bb_index;
    uint8_t register_data[4] = { 0 };
    uint32_t num_db;
    struct kaps_db *db, *db_a, *db_b;

    num_db = 0;
    db_a = NULL;
    db_b = NULL;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        db = KAPS_SSDBLIST_TO_ENTRY(el);
        if (db->type == KAPS_DB_LPM)
        {
            ++num_db;


            if (db->rpb_id == 0)
            {
                db_a = db;
            }
            else
            {
                db_b = db;
            }
        }
    }

    KapsWriteBitsInArray(register_data, sizeof(register_data), 1, 1, 0);

    if (db_a)
    {
        for (bb_index = db_a->hw_res.db_res->start_small_bb_nr; bb_index <= db_a->hw_res.db_res->end_small_bb_nr;
             bb_index++)
        {
            KAPS_STRY(kaps_dm_kaps_alg_reg_write(device, bb_index, 0x00000021, sizeof(register_data), register_data));

        }
    }

    KapsWriteBitsInArray(register_data, sizeof(register_data), 1, 1, 1);

    if (db_b)
    {
        for (bb_index = db_b->hw_res.db_res->start_small_bb_nr; bb_index <= db_b->hw_res.db_res->end_small_bb_nr;
             bb_index++)
        {
            KAPS_STRY(kaps_dm_kaps_alg_reg_write(device, bb_index, 0x00000021, sizeof(register_data), register_data));

        }
    }

    return KAPS_OK;
}





kaps_status
append_to_rpb_ab(struct kaps_db *cur_db, uint32_t i)
{
    struct kaps_device *device = cur_db->device;
    struct kaps_c_list_iter rpb_ab_it;
    struct kaps_list_node *rpb_ab_el;
    struct kaps_ab_info *ab;

    /*Link this RPB AB to the dup_ab list for the AB in the c list
    This is to handle cloned databases*/
    kaps_c_list_iter_init(&cur_db->hw_res.db_res->rpb_ab_list, &rpb_ab_it);
    while ((rpb_ab_el = kaps_c_list_iter_next(&rpb_ab_it)) != NULL)
    {
        ab = KAPS_ABLIST_TO_ABINFO(rpb_ab_el);

        while (ab->dup_ab)
            ab = ab->dup_ab;

        ab->dup_ab = &device->map->rpb_ab[i];
        device->map->rpb_ab[i].dup_ab = NULL;
        
    }

    return KAPS_OK;
}

kaps_status
kaps_assign_rpb_abs(struct kaps_db *cur_db, uint32_t i)
{
    struct kaps_device *device;
    

    if (cur_db ) 
    {
        device = cur_db->device;
            
        device->map->rpb_ab[i].db = cur_db;

        if (cur_db->hw_res.db_res->rpb_ab_list.count == 0)
        {
            /*Add the AB to the c-list*/
            kaps_c_list_add_tail(&cur_db->hw_res.db_res->rpb_ab_list, 
                                &device->map->rpb_ab[i].ab_node);
        }
        else
        {
            /*Link this RPB AB to the dup_ab list for the AB in the c list*/
            append_to_rpb_ab(cur_db, i);
        }
        
        cur_db->common_info->num_ab = 1;
    }

    return KAPS_OK;
}


static kaps_status
propagate_db_params_to_tables(
    struct kaps_db *parent_db)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_device *device;
    struct kaps_db *db_iter;

    if (parent_db) 
    {
        device = parent_db->device;
        
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            db_iter = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db_iter->type == KAPS_DB_LPM && db_iter->parent == parent_db)
            {
                db_iter->rpb_id = parent_db->rpb_id;
                db_iter->num_algo_levels_in_db = parent_db->num_algo_levels_in_db;
            }
        }
    }

    return KAPS_OK;
}


kaps_status
kaps_jr2_assign_num_algo_levels_in_big_device(
    struct kaps_device * device)
{
    uint32_t are_a_type_bbs_present = 0;
    uint32_t are_b_type_bbs_present = 0;
    struct kaps_db *db_a = NULL, *db_b = NULL, *db_iter;
    uint32_t i, bb_nr;
    enum large_bb_config_type bb_config_options;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t db_group_id = 0;


    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        db_iter = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db_iter->type == KAPS_DB_LPM && !db_iter->parent)
        {
            if (db_iter->rpb_id == 0)
            {
                db_a = db_iter;
            }
            else 
            {
                db_b = db_iter;
            }
        }
    }


    for (i = 0; i < device->hw_res->total_num_bb[0] / 2; ++i)
    {
        bb_nr = 2 * i;
        bb_config_options = device->map->bb_map[db_group_id][bb_nr].bb_config_options;

        if ( (bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B000) 
            || (bb_config_options & KAPS_LARGE_BB_WIDTH_A960_B000)
            || (bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B480) )
        {
            are_a_type_bbs_present = 1;
        }

        if ( (bb_config_options & KAPS_LARGE_BB_WIDTH_A000_B480) 
            || (bb_config_options & KAPS_LARGE_BB_WIDTH_A000_B960)
            || (bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B480) )
        {
            are_b_type_bbs_present = 1;
        }
    }
    

    if (are_a_type_bbs_present)
    {
        if (db_a)
            db_a->num_algo_levels_in_db = 3;
    }
    else 
    {
        if (db_a)
            db_a->num_algo_levels_in_db = 2;
    }

    
    if (are_b_type_bbs_present)
    {
        if (db_b)
            db_b->num_algo_levels_in_db = 3;
    }
    else
    {
        if (db_b)
            db_b->num_algo_levels_in_db = 2;
    }

    propagate_db_params_to_tables(db_a);
    propagate_db_params_to_tables(db_b);

 
    return KAPS_OK;
}


kaps_status
kaps_resource_kaps_configure_bbs(
    struct kaps_device * device,
    struct kaps_db * db)
{
    struct kaps_c_list_iter it2;
    struct kaps_list_node *el2;
    struct kaps_ab_info *ab_info;
    uint8_t register_data[4] = { 0 };
    struct kaps_ab_info *cur_dup_ab;
    int32_t bit_pos, bb_index;
    uint32_t num_bits_per_ab;
    uint32_t i, value;
    int8_t pair[2];

    num_bits_per_ab = 2;
    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID || device->id == KAPS_QUMRAN_DEVICE_ID
        || device->id == KAPS_QUX_DEVICE_ID)
        num_bits_per_ab = 1;

    kaps_c_list_iter_init(&db->hw_res.db_res->ab_list, &it2);
    while ((el2 = kaps_c_list_iter_next(&it2)) != NULL)
    {
        ab_info = KAPS_ABLIST_TO_ABINFO(el2);
        if (ab_info->db->is_clone || ab_info->db->parent)
            continue;

        cur_dup_ab = ab_info;
        bit_pos = 0;
        value = 0;
        pair[0] = -1;
        pair[1] = -1;
        for (i = 0; i < 2; ++i)
        {
            if (cur_dup_ab)
            {
                value = cur_dup_ab->ab_num;
                pair[i] = value;
                if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
                    value = cur_dup_ab->ab_num / 2;
            }

            KapsWriteBitsInArray(register_data, sizeof(register_data), bit_pos + num_bits_per_ab - 1, bit_pos, value);
            if (device->id == KAPS_QUMRAN_DEVICE_ID || device->id == KAPS_QUX_DEVICE_ID)
                break;

            if (cur_dup_ab)
                cur_dup_ab = cur_dup_ab->dup_ab;

            bit_pos += num_bits_per_ab;
        }

        if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
        {
            if (pair[0] != -1 && pair[1] != -1)
            {
                /*
                 * Store smaller value in pair[0] and larger value in pair[1]
                 */
                if (pair[0] > pair[1])
                {
                    int8_t temp = pair[0];

                    pair[0] = pair[1];
                    pair[1] = temp;
                }

                /*
                 * Pair-0 and Pair-1 indicate the TCAM Blocks that are used for searching the BBs belonging to a DB. So
                 * for instance if Pair-0 is 2 and Pair-1 is 3, then TCAMs 2 and 3 are used to search the BB. Note that
                 * because only some of the combinations are allowed, some obvious combinations such as DB-0 on TCAM-0
                 * and DB-1 on TCAM-1 are not allowed
                 */
                if (pair[0] == 0 && pair[1] == 1)
                {
                    KapsWriteBitsInArray(register_data, sizeof(register_data), 1, 0, 0);
                }
                else if (pair[0] == 2 && pair[1] == 3)
                {
                    KapsWriteBitsInArray(register_data, sizeof(register_data), 1, 0, 3);
                }
                else if (pair[0] == 0 && pair[1] == 3)
                {
                    KapsWriteBitsInArray(register_data, sizeof(register_data), 1, 0, 2);
                }
                else if (pair[0] == 1 && pair[1] == 2)
                {
                    KapsWriteBitsInArray(register_data, sizeof(register_data), 1, 0, 1);
                }
                else
                {
                    return KAPS_INVALID_BB_CONFIG;
                }
            }
        }
    }

    for (bb_index = db->hw_res.db_res->start_lpu; bb_index <= db->hw_res.db_res->end_lpu; bb_index++)
    {
        int32_t bb_blk_id = bb_index * device->hw_res->num_bb_in_one_chain;
        int32_t i = 0;

        for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
        {
            KAPS_STRY(kaps_dm_kaps_alg_reg_write(device, bb_blk_id, 0x00000021, sizeof(register_data), register_data));
            bb_blk_id++;
        }
    }
    return KAPS_OK;
}

/**
 * Try to fit the requested UDA memory
 * into the BBs available
 *
 * @param device KBP device handle
 *
 * @return KAPS_OK on success or an error code
 */
static kaps_status
kaps_fit_kaps_bbs(
    struct kaps_device *device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t num_bb, bb_index;
    float bb_size;
    uint32_t i, occupied_bb;
    uint32_t db_group_id = 0;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_LPM)
        {
            db_group_id = db->db_group_id;
            
            bb_size =
                (((float) device->hw_res->num_rows_in_bb[db_group_id] * KAPS_BKT_WIDTH_1 * device->hw_res->num_bb_in_one_chain) /
                 (1024 * 1024));
            db->common_info->uda_mb = db->hw_res.db_res->lsn_info[0].max_lsn_size * bb_size;

            if (!db->common_info->uda_mb)
                db->common_info->uda_mb = 1;
        }
    }

    if (!device->nv_ptr && device->issu_in_progress)
        return KAPS_OK;

    bb_index = 0;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
        uint32_t start_found = 0;

        if (db->type == KAPS_DB_LPM)
        {

            if (db->common_info->capacity == 0)
            {
                db->hw_res.db_res->lsn_info[0].max_lsn_size = db->device->hw_res->incr_in_bbs;
                continue;
            }

            kaps_memset(db->hw_res.db_res->lsn_info[0].alloc_udm, 0, sizeof(db->hw_res.db_res->lsn_info[0].alloc_udm));
            num_bb = db->hw_res.db_res->lsn_info[0].max_lsn_size;
            while (num_bb)
            {
                if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_chain))
                    return KAPS_RESOURCE_FIT_FAIL;

                occupied_bb = 0;
                for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
                {
                    if (device->map->bb_map[db_group_id][bb_index].row->owner)
                    {
                        if (start_found)
                            return KAPS_RESOURCE_FIT_FAIL;
                        occupied_bb = 1;
                        bb_index++;
                    }
                }

                if (occupied_bb)
                    continue;

                if (!start_found)
                {
                    db->hw_res.db_res->start_lpu = bb_index / device->hw_res->num_bb_in_one_chain;
                    start_found = 1;
                }

                db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index / device->hw_res->num_bb_in_one_chain] =
                    1;

                for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
                {
                    device->map->bb_map[db_group_id][bb_index].row->owner = db;
                    bb_index++;
                }
                num_bb--;
            }
            db->hw_res.db_res->end_lpu = (bb_index / device->hw_res->num_bb_in_one_chain) - 1;
        }
    }

    /*
     * Write the BB Config registers for LPM DBs
     */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_LPM)
        {
            KAPS_STRY(kaps_resource_kaps_configure_bbs(device, db));
        }
    }

    return KAPS_OK;
}

/**
 * Get the total bucket blocks available on the device
 */
static void
kaps_jr1_get_free_bbs_on_device(
    struct kaps_device *device,
    uint32_t * num_bb)
{
    uint32_t num_rows = 0;
    int32_t i;
    struct bb_partition_info *row;
    uint32_t num_bb_per_cascade;
    uint32_t db_group_id = 0;

    if (device->hw_res->reduced_algo_levels)
    {
        *num_bb = device->hw_res->total_small_bb[0];
        return;
    }

    num_bb_per_cascade = device->map->num_bb;
    for (i = 0; i < num_bb_per_cascade * device->hw_res->num_bb_in_one_chain; i += device->hw_res->num_bb_in_one_chain)
    {
        row = device->map->bb_map[db_group_id][i].row;
        while (row)
        {
            if (row->owner == NULL)
                num_rows += row->num_rows;
            row = row->next;
        }
    }

    *num_bb = num_rows / device->hw_res->num_rows_in_bb[0];
}

/**
 * Allocate static LPM resources based on capacity and type(v4/v6)
 */
static void
kaps_allocate_static_lpm_resource(
    struct kaps_device *device,
    uint32_t capacity,
    uint8_t is_v6,
    uint32_t * num_bb)
{
    /*
     * 16bb * num_bb_in_one_chain for for 300K v4 or 150K v6 prefixes
     */
    uint32_t max_cap = 300000;
    uint32_t nbb = 0;

    if (is_v6)
        max_cap /= 2;

    if (device->id == KAPS_QUMRAN_DEVICE_ID)
        max_cap /= 2;

    if (device->id == KAPS_QUX_DEVICE_ID)
        max_cap /= 8;

    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
    {
        if (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM4
            || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM0)
        {
            max_cap *= 4;
        }
    }

    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        if (device->hw_res->num_algo_levels == 2)
            max_cap /= 8;
        else
            max_cap *= 4;
    }

    nbb = (capacity * device->map->num_bb + max_cap - 1) / max_cap;

    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
    {
        if (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM4
            || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
        {
            /*
             * Number of Bucket Blocks should be a multiple of 4 for now
             */
            nbb = ((nbb + 3) / 4) * 4;
        }
    }

    *num_bb = nbb;
}


static void
swap_val(
    uint32_t *x,
    uint32_t *y)
{
    uint32_t temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

static kaps_status
kaps_calculate_bb_proportion(
    struct kaps_device *device,
    struct kaps_db **db_arr,
    uint32_t num_dbs,
    uint32_t bb_arr[],
    uint32_t total_bb_needed,
    uint32_t available_bb,
    uint32_t proportional_bb_arr[],
    uint32_t num_lpm_db)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    int32_t i, j;
    struct kaps_db *temp_db;


    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID &&
        (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM4
         || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4))
    {
        uint32_t avg_num_bb_per_lpm_db = available_bb / num_lpm_db;
        uint32_t num_assigned_bb, num_bbs_left, num_4bb_units_left;
        uint32_t num_bbs_still_needed_arr[4];

        /*We have to give at least 4 BBs to each db. If the average is less than 4,
        then we can't assign BBs for one or more dbs. So return an error*/
        if (avg_num_bb_per_lpm_db < 4)
            return KAPS_RESOURCE_FIT_FAIL;

        if (num_lpm_db > 4)
            return KAPS_RESOURCE_FIT_FAIL;


        /*Sort in descending order of capacity*/
        for (i = 0; i < num_lpm_db - 1; i++)
        {
           for (j = i+1; j < num_lpm_db; ++j)
           {
                if (db_arr[i]->common_info->capacity < 
                    db_arr[j]->common_info->capacity)
                {
                    temp_db = db_arr[i];
                    db_arr[i] = db_arr[j];
                    db_arr[j] = temp_db;
                    
                    swap_val(&bb_arr[i], &bb_arr[j]);
                    swap_val(&proportional_bb_arr[i], &proportional_bb_arr[j]);
                }
           }
        }
        

        /*First give 4 BBs for each DB*/
        num_assigned_bb = 0;
        for (j = 0; j < num_lpm_db; ++j)
        {
            proportional_bb_arr[j] = 4;
            num_assigned_bb += 4;

            if (bb_arr[j] > 4)
            {
                num_bbs_still_needed_arr[j] = bb_arr[j] - 4;
            }
            else
            {
                num_bbs_still_needed_arr[j] = 0;
            }
        }

        num_bbs_left = available_bb - num_assigned_bb;

        num_4bb_units_left = num_bbs_left / 4;


        /*Assign the remaining 4BB units in the descending order of BB capacity*/
        while (num_4bb_units_left)
        {
           for (j = 0; j < num_lpm_db; ++j)
           {
                if (num_bbs_still_needed_arr[j])
                {
                    proportional_bb_arr[j] += 4;

                    if (num_bbs_still_needed_arr[j] > 4)
                        num_bbs_still_needed_arr[j] -= 4;
                    else
                        num_bbs_still_needed_arr[j] = 0;

                    num_4bb_units_left--;
                }

                if (num_4bb_units_left == 0)
                    break;
           }
            
        }

        
        return KAPS_OK;

    }
   

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_LPM)
        {
            for (j = 0; j < num_dbs; j++)
            {
                if (db_arr[j] == db)
                {

                    proportional_bb_arr[j] = (bb_arr[j] * available_bb + total_bb_needed - 1) / total_bb_needed;

                    db->hw_res.db_res->lsn_info[0].max_lsn_size = 0;
                    break;
                }
            }
        }
    }

    return KAPS_OK;
}

static kaps_status kaps_allocate_dma_bbs(struct kaps_device *device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_fast_bitmap dma_tag_free_fbmp;
    struct memory_map *mem_map = resource_kaps_get_memory_map(device);
    struct kaps_db *last_dma_db = NULL;
    uint32_t total_num_bb_assigned_to_dma_dbs;
    uint32_t db_group_id = 0;

    KAPS_STRY(kaps_init_bmp(&dma_tag_free_fbmp, device->alloc, KAPS_DMA_TAG_MAX + 1, 1));

    /* Reserve the user specified dma tags */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL) {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
        struct kaps_dma_db *dma_db;

        if (db->type == KAPS_DB_DMA) {
            dma_db = (struct kaps_dma_db *) db;
            if (dma_db->dma_tag != -1) {
                if (!kaps_get_bit(&dma_tag_free_fbmp, dma_db->dma_tag))
                    return KAPS_INVALID_RESOURCE_UDA;

                kaps_reset_bit(&dma_tag_free_fbmp, dma_db->dma_tag);
            }
        }
    }

    /* Allocate dma tags to DBs not specified by user */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL) {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
        struct kaps_dma_db *dma_db;

        if (db->type == KAPS_DB_DMA) {
            dma_db = (struct kaps_dma_db *) db;
            if (dma_db->dma_tag == -1) {
                dma_db->dma_tag = kaps_find_first_bit_set(&dma_tag_free_fbmp, 0, KAPS_DMA_TAG_MAX);
                if (dma_db->dma_tag == -1)
                    return KAPS_RESOURCE_FIT_FAIL;

                kaps_reset_bit(&dma_tag_free_fbmp, dma_db->dma_tag);
                
            }
        }
    }

    total_num_bb_assigned_to_dma_dbs = 0;

    /* Allocate the user specified resources to the DMA DBs first */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL) {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
        struct kaps_dma_db *dma_db;

        if (db->type == KAPS_DB_DMA) {
            dma_db = (struct kaps_dma_db *) db;
            if (db->common_info->user_specified) {
                if (mem_map->bb_map[db_group_id][db->hw_res.db_res->start_lpu].row->owner != NULL)
                    return KAPS_INVALID_RESOURCE_UDA;

                mem_map->bb_map[db_group_id][db->hw_res.db_res->start_lpu].row->owner = db;

                kaps_set_bit(dma_db->alloc_fbmap, db->hw_res.db_res->start_lpu);

                last_dma_db = db;
                total_num_bb_assigned_to_dma_dbs++;
            }
        }
    }

    kaps_free_bmp(&dma_tag_free_fbmp, device->alloc);

    /* Allocate BBs to the rest of the DMA DBs */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL) {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
        struct kaps_dma_db *dma_db;
        int32_t bb_index;

        if (db->type == KAPS_DB_DMA) {
            dma_db = (struct kaps_dma_db *) db;
            if (!db->common_info->user_specified) {
                for (bb_index = 0; bb_index < mem_map->num_bb * device->hw_res->num_bb_in_one_chain; bb_index++) {
                    if (mem_map->bb_map[db_group_id][bb_index].row->owner)
                        continue;
                    
                    db->hw_res.db_res->start_lpu = bb_index;
                    db->hw_res.db_res->end_lpu = bb_index;
                    mem_map->bb_map[db_group_id][bb_index].row->owner = db;
                    
                    kaps_set_bit(dma_db->alloc_fbmap, bb_index);
                    total_num_bb_assigned_to_dma_dbs++;
                    break;
                }
                if (bb_index == (mem_map->num_bb * device->hw_res->num_bb_in_one_chain))
                    return KAPS_RESOURCE_FIT_FAIL;
            }
            
            last_dma_db = db;
        }
    }

    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID &&
        (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM4
         || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4))
    {
        uint32_t num_units_assigned_to_dma_dbs;
        uint32_t num_4bb_units_assigned_to_dma ;
        uint32_t num_extra_dma_bb_units_to_assign ;
        int32_t bb_index, num_extra_dma_bbs_to_mark_as_used;

        /*A unit consists of all the BBs in the cascade*/
        num_units_assigned_to_dma_dbs = (total_num_bb_assigned_to_dma_dbs + device->hw_res->num_bb_in_one_chain - 1) / device->hw_res->num_bb_in_one_chain;

        num_4bb_units_assigned_to_dma = ((num_units_assigned_to_dma_dbs + 3) / 4 ) ;

        /*See how many more units need to be assigned to make the number of units assigned a multiple of 4*/
        num_extra_dma_bb_units_to_assign = (num_4bb_units_assigned_to_dma * 4) - num_units_assigned_to_dma_dbs;

        /*Find the actual number of DMA BBs after taking into account num chained bb*/
        num_extra_dma_bbs_to_mark_as_used = num_extra_dma_bb_units_to_assign * device->hw_res->num_bb_in_one_chain; 

        /*Assign the extra dma bb units to the last_dma_db so that the number of units assigned to DMA is a multiple of 4*/
        if (num_extra_dma_bbs_to_mark_as_used)
        {
       
            for (bb_index = 0; bb_index < mem_map->num_bb * device->hw_res->num_bb_in_one_chain; bb_index++) 
            {
                if (mem_map->bb_map[db_group_id][bb_index].row->owner)
                    continue;

                mem_map->bb_map[db_group_id][bb_index].row->owner = last_dma_db;
                num_extra_dma_bbs_to_mark_as_used--;

                if (num_extra_dma_bbs_to_mark_as_used == 0)
                    break;
                
            }
        }
    }

    /* Write the BB Config registers for DMA DBs */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL) {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
        struct kaps_dma_db *dma_db;
        int32_t bb_index;
        uint32_t register_data;
        uint32_t dma_addr_start_bit = 4;

        if (db->type == KAPS_DB_DMA) {
            dma_db = (struct kaps_dma_db *) db;
            register_data = 0;
            bb_index = db->hw_res.db_res->start_lpu;

            if (device->id == KAPS_QUX_DEVICE_ID || device->id == KAPS_QUMRAN_DEVICE_ID)
                dma_addr_start_bit = 1;
            else if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
                dma_addr_start_bit = 2;

            if (device->issu_in_progress == 1)
                continue;

            KapsWriteBitsInArray((uint8_t *) &register_data, sizeof(register_data), dma_addr_start_bit + 8, dma_addr_start_bit + 8, 1);
            KapsWriteBitsInArray((uint8_t *) &register_data, sizeof(register_data), dma_addr_start_bit + 7, dma_addr_start_bit, dma_db->dma_tag);
            KAPS_STRY(kaps_dm_kaps_alg_reg_write(device, bb_index, 0x00000021, sizeof(register_data), (uint8_t *) &register_data));

            if (device->id == KAPS_DEFAULT_DEVICE_ID) {
                /* Clear the BB memory */
                KAPS_STRY(kaps_dm_kaps_alg_reg_read(device, bb_index, 0x00000023, sizeof(register_data), (uint8_t *) &register_data));
                KapsWriteBitsInArray((uint8_t *) &register_data, sizeof(register_data), 16, 16, 1);
                KAPS_STRY(kaps_dm_kaps_alg_reg_write(device, bb_index, 0x00000023, sizeof(register_data), (uint8_t *) &register_data));
                KapsWriteBitsInArray((uint8_t *) &register_data, sizeof(register_data), 16, 16, 0);
                KAPS_STRY(kaps_dm_kaps_alg_reg_write(device, bb_index, 0x00000023, sizeof(register_data), (uint8_t *) &register_data));
            }
        }
    }

    return KAPS_OK;
}


static kaps_status
kaps_jr1_allocate_bbs(
    struct kaps_device *device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_db **db_arr = NULL;
    uint32_t num_bb, available_bb, total_bb_needed = 0;
    uint32_t max_width = 0;
    uint32_t *bb_arr, *proportional_bb_arr, i = 0, j = 0;
    uint32_t num_dbs = kaps_c_list_count(&device->db_list);
    uint32_t num_lpm_db;

    KAPS_STRY(kaps_allocate_dma_bbs(device));


    if (!device->nv_ptr && device->issu_in_progress)
    {
        return KAPS_OK;
    }

    db_arr = device->alloc->xcalloc(device->alloc->cookie, num_dbs, sizeof(struct kaps_db *));
    if (!db_arr)
        return KAPS_OUT_OF_MEMORY;

    proportional_bb_arr = device->alloc->xcalloc(device->alloc->cookie, num_dbs, sizeof(uint32_t));
    if (!proportional_bb_arr)
    {
        device->alloc->xfree(device->alloc->cookie, db_arr);
        return KAPS_OUT_OF_MEMORY;
    }

    bb_arr = device->alloc->xcalloc(device->alloc->cookie, num_dbs, sizeof(uint32_t));
    if (!bb_arr)
    {
        device->alloc->xfree(device->alloc->cookie, db_arr);
        device->alloc->xfree(device->alloc->cookie, proportional_bb_arr);
        return KAPS_OUT_OF_MEMORY;
    }

    num_lpm_db = 0;
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *tab, *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_LPM)
        {
            for (tab = db; tab; tab = tab->next_tab)
            {
                if (tab->key->width_1 > max_width)
                    max_width = tab->key->width_1;
            }

            if (db->hw_res.db_res->num_bbs)
            {
                num_bb = db->hw_res.db_res->num_bbs;
            }
            else
            {
                kaps_allocate_static_lpm_resource(device, db->common_info->capacity, (max_width > 128), &num_bb);
            }

            if (num_bb == 0)
                num_bb = 1;

            db_arr[i] = db;
            bb_arr[i] = num_bb;
            total_bb_needed += bb_arr[i];
            i++;
            num_lpm_db++;
        }
    }

    kaps_jr1_get_free_bbs_on_device(device, &available_bb);

    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID &&
        (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM4
         || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4))
    {
        if (available_bb % 4 != 0)
            available_bb = (available_bb / 4) * 4;
    }

    /*
     * Allocate BBs proportionately if required BBs is more than available
     */
    if (total_bb_needed > available_bb)
    {
        uint32_t num_allocated_bb = 0;
        uint32_t done = 0;

        KAPS_STRY(kaps_calculate_bb_proportion(device, db_arr, num_dbs, bb_arr, total_bb_needed, available_bb,
                                proportional_bb_arr, num_lpm_db));

        while (!done)
        {
            /*
             * do a round robin distribution of 1 BB each to the databases that are not yet fully satisfied. 
             * this helps overcome the problem of dealing with fractional values which will be hard to distribute
             */
            kaps_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

                if (db->type == KAPS_DB_LPM)
                {
                    for (j = 0; j < i; j++)
                    {
                        if (db_arr[j] == db)
                        {
                            if (proportional_bb_arr[j])
                            {
                                if (num_allocated_bb < available_bb)
                                {
                                    db->hw_res.db_res->lsn_info[0].max_lsn_size += 1;
                                    num_allocated_bb++;
                                    proportional_bb_arr[j]--;
                                }
                                else
                                {
                                    /*
                                     * We have used up all the BBs that were available. But for one db we have not been
                                     * able to allocate even a single BB. So return a resource fit fail error
                                     */
                                    if (db->hw_res.db_res->lsn_info[0].max_lsn_size == 0)
                                    {
                                        device->alloc->xfree(device->alloc->cookie, db_arr);
                                        device->alloc->xfree(device->alloc->cookie, proportional_bb_arr);
                                        device->alloc->xfree(device->alloc->cookie, bb_arr);
                                        return KAPS_RESOURCE_FIT_FAIL;
                                    }
                                }
                            }

                            break;
                        }
                    }
                }
            }

            if (num_allocated_bb >= available_bb)
            {
                done = 1;
                break;
            }
        }
    }
    else
    {
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_DB_LPM)
            {
                for (j = 0; j < i; j++)
                {
                    if (db_arr[j] == db)
                    {
                        db->hw_res.db_res->lsn_info[0].max_lsn_size = bb_arr[j];
                        db_arr[j] = NULL;
                        break;
                    }
                }
            }
        }
    }

    device->alloc->xfree(device->alloc->cookie, db_arr);
    device->alloc->xfree(device->alloc->cookie, proportional_bb_arr);
    device->alloc->xfree(device->alloc->cookie, bb_arr);

    return KAPS_OK;
}

/**
 * Initialize a memory of the KAPS device
 * taking into account all the relevant device parameters.
 * This memory map will be used to perform the resource allocation
 *
 * @param device KBP device handle
 *
 * @return KAPS_OK on success or an error code
 */
kaps_status
kaps_initialize_kaps_mem_map(
    struct kaps_device * device)
{
    int32_t i;
    struct memory_map *mem_map;

    device->hw_res->incr_in_bbs = 4;
    mem_map = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct memory_map));
    if (!mem_map)
        return KAPS_OUT_OF_MEMORY;

    mem_map->ab_memory = device->alloc->xcalloc(device->alloc->cookie, device->num_ab[0], sizeof(struct kaps_ab_info));
    if (!mem_map->ab_memory)
    {
        device->alloc->xfree(device->alloc->cookie, mem_map);
        return KAPS_OUT_OF_MEMORY;
    }

    for (i = 0; i < device->num_ab[0]; i++)
    {
        mem_map->ab_memory[i].ab_num = i;
        mem_map->ab_memory[i].device = device;
        if (device->hw_res->num_algo_levels == 3)
        {
            mem_map->ab_memory[i].is_mapped_ab = 1;
        }
    }

    mem_map->rpb_ab = device->alloc->xcalloc(device->alloc->cookie, KAPS_HW_MAX_NUM_RPB_BLOCKS, 
                                sizeof(struct kaps_ab_info));
    
    if (!mem_map->rpb_ab)
    {
        return KAPS_OUT_OF_MEMORY;
    }

    for (i = 0; i < KAPS_HW_MAX_NUM_RPB_BLOCKS; ++i)
    {
        mem_map->rpb_ab[i].ab_num = i;
        mem_map->rpb_ab[i].device = device;
    }

    mem_map->num_bb = device->hw_res->total_num_bb[0] / device->hw_res->num_bb_in_one_chain;
    mem_map->bb_map[0] = device->alloc->xcalloc(device->alloc->cookie,
                                         mem_map->num_bb * device->hw_res->num_bb_in_one_chain, sizeof(struct bb_info));
    if (!mem_map->bb_map[0])
    {
        device->alloc->xfree(device->alloc->cookie, mem_map->ab_memory);
        device->alloc->xfree(device->alloc->cookie, mem_map);
        return KAPS_OUT_OF_MEMORY;
    }

    for (i = 0; i < mem_map->num_bb * device->hw_res->num_bb_in_one_chain; i++)
    {
        mem_map->bb_map[0][i].width_1 = KAPS_BKT_WIDTH_1;
        mem_map->bb_map[0][i].row = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct bb_partition_info));

        if (!mem_map->bb_map[0])
        {
            device->alloc->xfree(device->alloc->cookie, mem_map->ab_memory);

            for (i--; i >= 0; i--)
                device->alloc->xfree(device->alloc->cookie, mem_map->bb_map[0][i].row);

            device->alloc->xfree(device->alloc->cookie, mem_map->bb_map[0]);
            device->alloc->xfree(device->alloc->cookie, mem_map);
            return KAPS_OUT_OF_MEMORY;
        }
        mem_map->bb_map[0][i].row->start_row = 0;
        mem_map->bb_map[0][i].row->num_rows = device->hw_res->num_rows_in_bb[0];
        mem_map->bb_map[0][i].row->owner = NULL;
        mem_map->bb_map[0][i].row->next = NULL;

        mem_map->bb_map[0][i].bb_num_rows = device->hw_res->num_rows_in_bb[0];
        mem_map->bb_map[0][i].num_units_in_bb = 1;
    }

    device->map = mem_map;

    return KAPS_OK;
}


static kaps_status
kaps_jr2_resource_assign_rpbs(
    struct kaps_device *device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    int32_t i, j, count;
    struct kaps_db *cur_db;

    /*
     * Make sure that the same db is being searched at the current position in all instructions
     * device->max_num_searches indicates the maximum number of parallel searches
     */
    for (i = 0; i < device->max_num_searches; i++)
    {

        /*
         * Iterate through the instructions
         */
        cur_db = NULL;
        kaps_c_list_iter_init(&device->inst_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {

            struct kaps_instruction *instr = KAPS_INSTLIST_TO_ENTRY(el);

            if (instr->desc[i].db == NULL)
                continue;

            if (cur_db == NULL)
            {
                cur_db = kaps_db_get_main_db(instr->desc[i].db);

                if (i % 2 == 0)
                {
                    instr->desc[i].db->rpb_id = 0;
                    cur_db->rpb_id = 0;
                }
                else
                {
                    instr->desc[i].db->rpb_id = 1;
                    cur_db->rpb_id = 1;
                }
                
                
                continue;
            }

            if (cur_db != kaps_db_get_main_db(instr->desc[i].db))
            {
                return KAPS_CONFLICTING_SEARCH_PORTS;
            }

            instr->desc[i].db->rpb_id = cur_db->rpb_id;
        }

        if (cur_db)
        {
            /*
             * Assign RPT to database in the RPT map
             */
            cur_db->hw_res.db_res->rpt_map[0] |= 1ULL << i;
            device->map->rpb_info[i].rpt_depth = device->hw_res->num_rows_in_rpb[i];
            device->map->rpb_info[i].db = cur_db;

            kaps_assign_rpb_abs(cur_db, i);
        }
    }

    for (i = 0; i < device->max_num_searches; i++)
    {
        cur_db = device->map->rpb_info[i].db;
        if (!cur_db)
            continue;

        count = 1;

        for (j = 0; j < device->max_num_searches; j++)
        {
            if (!device->map->rpb_info[j].db)
                continue;

            if (i == j)
                continue;

            if (device->map->rpb_info[j].db == cur_db)
                count++;

            if (count > device->max_num_clone_parallel_lookups)
                return KAPS_EXCEEDED_MAX_PARALLEL_SEARCHES;
        }
    }

    return KAPS_OK;
}

static kaps_status
kaps_resource_assign_kaps_small_bb_abs(
    struct kaps_device *device)
{
    uint32_t i, j;
    struct kaps_db *cur_db;
    uint64_t rptMap;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t num_small_ab_per_db;
    uint32_t start_ab_nr, end_ab_nr;

    /*
     * For the first time assign the AB config information to the
     * AB structure
     */
    for (i = 0; i < device->num_ab[0]; i++)
    {
        struct kaps_ab_info *ab = &device->map->ab_memory[i];
        ab->conf = KAPS_NORMAL_160;
        /*
         * Num slots filled up here is not what is picked up. The num slots depends on granularity of entries entering
         * in. This will be decided at run time
         */
        ab->num_slots = 0;
    }

    num_small_ab_per_db = device->num_ab[0] / 2;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        cur_db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (cur_db->type != KAPS_DB_LPM)
            continue;

        rptMap = cur_db->hw_res.db_res->rpt_map[0];

        for (i = 0; i < device->hw_res->num_rpb_blks; ++i)
        {
            if (rptMap & 1ULL << i)
            {
                start_ab_nr = (i % 2) * num_small_ab_per_db;
                end_ab_nr = start_ab_nr + num_small_ab_per_db - 1;

                for (j = start_ab_nr; j <= end_ab_nr; ++j)
                {
                    kaps_c_list_add_tail(&cur_db->hw_res.db_res->ab_list, &device->map->ab_memory[j].ab_node);
                    device->map->ab_memory[j].db = cur_db;
                }
                cur_db->common_info->num_ab = num_small_ab_per_db;
                break;
            }
        }
    }

    return KAPS_OK;
}

static kaps_status
kaps_kaps2_find_start_end_lpu(
    struct kaps_device *device,
    struct kaps_db *db)
{
    uint32_t i;
    uint32_t db_group_id = db->db_group_id;

    db->hw_res.db_res->start_lpu = 0xFFFF;

    for (i = 0; i < device->hw_res->total_num_bb[db_group_id]; ++i)
    {
        if (db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][i] == 1)
        {
            if (db->hw_res.db_res->start_lpu == 0xFFFF)
            {
                db->hw_res.db_res->start_lpu = i;
            }

            db->hw_res.db_res->end_lpu = i;
        }
    }

    return KAPS_OK;
}

static kaps_status
kaps_initialize_small_bb_brick_bitmap(
    struct kaps_db *db)
{
    uint32_t i, j;
    uint32_t small_bb_bit_no;
    struct kaps_device *device = db->device;
    uint32_t db_group_id = db->db_group_id;

    db->hw_res.db_res->small_bb_brick_bitmap_num_bytes = 
            device->hw_res->total_small_bb[db_group_id] * device->hw_res->num_rows_in_small_bb[db_group_id] / KAPS_BITS_IN_BYTE;

    if (db->hw_res.db_res->small_bb_brick_bitmap_num_bytes)
    {
        db->hw_res.db_res->small_bb_brick_bitmap = device->alloc->xcalloc(device->alloc->cookie, 
                            1, db->hw_res.db_res->small_bb_brick_bitmap_num_bytes);
    }

    /*
     * First re-initialize the whole space from start_small_bb_nr to end_small_bb_nr with 1 to indicate that the whole
     * region is free
     */
    small_bb_bit_no = 0;
    for (i = 0; i < device->hw_res->num_rows_in_small_bb[db_group_id]; ++i)
    {
        for (j = db->hw_res.db_res->start_small_bb_nr; j <= db->hw_res.db_res->end_small_bb_nr; ++j)
        {
            kaps_array_set_bit(db->hw_res.db_res->small_bb_brick_bitmap, small_bb_bit_no, 1);
            small_bb_bit_no++;
        }
    }

    return KAPS_OK;
}

static kaps_status
kaps_allocate_fit_small_bbs(
    struct kaps_device *device)
{
    struct kaps_db *db_a, *db_b, *cur_db, *tab;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t num_dbs, multiplier;
    uint32_t device_normalized_capacity, normalized_capacity;
    uint32_t db_a_normalized_capacity, db_b_normalized_capacity;
    uint32_t num_two_level_db, num_three_level_db;
    uint32_t db_group_id = 0;

    num_dbs = 0;
    db_a = NULL;
    db_b = NULL;
    db_a_normalized_capacity = 0;
    db_b_normalized_capacity = 0;

    num_two_level_db = 0;
    num_three_level_db = 0;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        cur_db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (cur_db->type != KAPS_DB_LPM)
        {
            continue;
        }

        ++num_dbs;

        if (cur_db->num_algo_levels_in_db == 3) {
            num_three_level_db++;
        } else if (cur_db->num_algo_levels_in_db == 2) {
            num_two_level_db++;
        }

        tab = cur_db;
        multiplier = 1;
        while (tab)
        {
            if (tab->key->width_1 > 80)
            {
                multiplier = 2;
            }
            tab = tab->next_tab;
        }

        normalized_capacity = cur_db->common_info->capacity * multiplier;
        if (normalized_capacity == 0)
            normalized_capacity = 1;

        if (cur_db->rpb_id == 0)
        {
            db_a = cur_db;
            db_a_normalized_capacity = normalized_capacity;
            db_group_id = db_a->db_group_id;
        }
        else
        {
            db_b = cur_db;
            db_b_normalized_capacity = normalized_capacity;
            db_group_id = db_b->db_group_id;
        }
    }

    device_normalized_capacity = db_a_normalized_capacity + db_b_normalized_capacity;

    if (num_dbs == 1)
    {
        if (!db_a)
        {
            db_a = db_b;
        }

        db_a->hw_res.db_res->start_small_bb_nr = 0;
        db_a->hw_res.db_res->end_small_bb_nr = device->hw_res->total_small_bb[db_group_id] - 1;
        db_a->hw_res.db_res->num_small_bb_per_row = device->hw_res->total_small_bb[db_group_id];
        db_a->hw_res.db_res->max_num_bricks_per_ab = db_a->hw_res.db_res->num_small_bb_per_row;
        if (db_a->hw_res.db_res->max_num_bricks_per_ab > 8)
            db_a->hw_res.db_res->max_num_bricks_per_ab = 8;

        if (db_a->num_algo_levels_in_db == 2)
            kaps_jr2_config_small_bb_for_two_level_db(db_a);

        kaps_initialize_small_bb_brick_bitmap(db_a);

    }
    else
    {
        if (db_a == NULL)
        {
            return KAPS_INTERNAL_ERROR;
        }
        if (num_three_level_db == 1 && num_two_level_db == 1) {
            if (db_a->num_algo_levels_in_db == 3) {
                db_a->hw_res.db_res->num_small_bb_per_row = 12;    
            } else {
                db_a->hw_res.db_res->num_small_bb_per_row = 4;
            }
        }
        else {
            db_a->hw_res.db_res->num_small_bb_per_row =
                device->hw_res->total_small_bb[db_group_id] * 1.0 * db_a_normalized_capacity / device_normalized_capacity;
        }

        if (num_two_level_db == 2)
        {
            /*Ensure that atleast one small BB is available to each database*/
            if (db_a->hw_res.db_res->num_small_bb_per_row >= device->hw_res->total_small_bb[db_group_id] )
                db_a->hw_res.db_res->num_small_bb_per_row = device->hw_res->total_small_bb[db_group_id] - 1; 

            if (db_a->hw_res.db_res->num_small_bb_per_row == 0)
                db_a->hw_res.db_res->num_small_bb_per_row = 1;
                
        }
        else
        {
            if (db_a->hw_res.db_res->num_small_bb_per_row % 2 == 1)
                db_a->hw_res.db_res->num_small_bb_per_row++;

            if (db_a->hw_res.db_res->num_small_bb_per_row < MIN_NUM_SMALL_BB_PER_ROW)
                db_a->hw_res.db_res->num_small_bb_per_row = MIN_NUM_SMALL_BB_PER_ROW;

            if (db_a->hw_res.db_res->num_small_bb_per_row > MAX_NUM_SMALL_BB_PER_ROW)
                db_a->hw_res.db_res->num_small_bb_per_row = MAX_NUM_SMALL_BB_PER_ROW;
        }
        
        db_a->hw_res.db_res->start_small_bb_nr = 0;
        db_a->hw_res.db_res->end_small_bb_nr = db_a->hw_res.db_res->num_small_bb_per_row - 1;
        db_a->hw_res.db_res->max_num_bricks_per_ab = db_a->hw_res.db_res->num_small_bb_per_row;
        if (db_a->hw_res.db_res->max_num_bricks_per_ab > 8)
            db_a->hw_res.db_res->max_num_bricks_per_ab = 8;

        if (db_a->num_algo_levels_in_db == 2)
            kaps_jr2_config_small_bb_for_two_level_db(db_a);

        kaps_sassert(db_b != NULL);
        db_b->hw_res.db_res->start_small_bb_nr = db_a->hw_res.db_res->end_small_bb_nr + 1;
        db_b->hw_res.db_res->end_small_bb_nr = device->hw_res->total_small_bb[db_group_id] - 1;
        db_b->hw_res.db_res->num_small_bb_per_row =
            db_b->hw_res.db_res->end_small_bb_nr - db_b->hw_res.db_res->start_small_bb_nr + 1;
        db_b->hw_res.db_res->max_num_bricks_per_ab = db_b->hw_res.db_res->num_small_bb_per_row;
        if (db_b->hw_res.db_res->max_num_bricks_per_ab > 8)
            db_b->hw_res.db_res->max_num_bricks_per_ab = 8;

        if (db_b->num_algo_levels_in_db == 2)
            kaps_jr2_config_small_bb_for_two_level_db(db_b);

        kaps_initialize_small_bb_brick_bitmap(db_a);
        kaps_initialize_small_bb_brick_bitmap(db_b);
    }

    return KAPS_OK;
}

static kaps_status
kaps_kaps2_allocate_fit_large_bbs(
    struct kaps_device *device)
{
    double cur_diff, min_diff;
    double capacity_ratio, estimated_ratio;
    uint32_t cur_num_rows_for_a, cur_num_rows_for_b;
    uint32_t estimated_num_rows_for_a, estimated_num_rows_for_b;
    uint32_t i, c, bb_nr;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_db *db_a, *db_b, *cur_db, *tab;
    uint32_t num_dbs;
    enum large_bb_config_type bb_config_options;
    uint32_t num_unassigned_blocks = 0;
    uint8_t unassigned_blocks[32];
    uint32_t unassigned_ix;
    uint32_t capacity_a, capacity_b;
    uint32_t multiplier;
    uint32_t fair_cutoff;
    uint32_t found;
    uint32_t db_group_id = 0;

    cur_num_rows_for_a = 0;
    cur_num_rows_for_b = 0;

    num_dbs = 0;
    db_a = NULL;
    db_b = NULL;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        cur_db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (cur_db->type == KAPS_DB_LPM && cur_db->num_algo_levels_in_db == 3)
        {
            ++num_dbs;

            if (cur_db->rpb_id == 0)
            {
                db_a = cur_db;
            }
            else
            {
                db_b = cur_db;
            }
        }
    }

    /*
     * If db_a is present, then ensure that there is at least one large BB which can be used by this database. If not,
     * then return a FIT Error
     */
    if (db_a)
    {
        found = 0;
        db_group_id = db_a->db_group_id;
        for (i = 0; i < device->hw_res->total_num_bb[db_group_id] / 2; ++i)
        {
            bb_nr = 2 * i;
            bb_config_options = device->map->bb_map[db_group_id][bb_nr].bb_config_options;

            if (bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B000
                || bb_config_options & KAPS_LARGE_BB_WIDTH_A960_B000
                || bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B480)
            {
                found = 1;
                break;
            }
        }

        if (!found)
        {
            db_a->common_info->fit_error = FAILED_FIT_LSN;
            return KAPS_RESOURCE_FIT_FAIL;
        }
    }

    /*
     * If db_b is present, then ensure that there is at least one large BB which can be used by this database. If not
     * then return a FIT Error
     */
    if (db_b)
    {
        found = 0;
        db_group_id = db_b->db_group_id;
        for (i = 0; i < device->hw_res->total_num_bb[db_group_id] / 2; ++i)
        {
            bb_nr = 2 * i;
            bb_config_options = device->map->bb_map[db_group_id][bb_nr].bb_config_options;

            if (bb_config_options & KAPS_LARGE_BB_WIDTH_A000_B480
                || bb_config_options & KAPS_LARGE_BB_WIDTH_A000_B960
                || bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B480)
            {
                found = 1;
                break;
            }
        }

        if (!found)
        {
            db_b->common_info->fit_error = FAILED_FIT_LSN;
            return KAPS_RESOURCE_FIT_FAIL;
        }
    }

    /*
     * First go through the BBs and if we find a BB which has to always be assigned to a particular database then go
     * ahead and assign it. The remaining BBs for which we have a choice between the two databases will be initially
     * unassigned
     */
    num_unassigned_blocks = 0;
    unassigned_ix = 0;

    for (i = 0; i < device->hw_res->total_num_bb[0] / 2; ++i)
    {
        bb_nr = 2 * i;
        bb_config_options = device->map->bb_map[db_group_id][bb_nr].bb_config_options;

        if (bb_config_options == KAPS_LARGE_BB_WIDTH_A000_B000)
        {
            /*
             * do nothing since the block is disabled
             */
            device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A000_B000;
        }
        else if (bb_config_options == KAPS_LARGE_BB_WIDTH_A000_B480
                 || ((bb_config_options & KAPS_LARGE_BB_WIDTH_A000_B480) && num_dbs == 1 && db_b))
        {

            device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A000_B480;

            /*
             * assign the block to the database on RPB-B
             */
            if (db_b)
            {
                db_b->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                device->map->bb_map[db_group_id][bb_nr].row->owner = db_b;
                cur_num_rows_for_b += device->hw_res->ads2_depth[1] / 2;
                db_b->hw_res.db_res->lsn_info[0].max_lsn_size++;
            }
        }
        else if (bb_config_options == KAPS_LARGE_BB_WIDTH_A000_B960
                 || ((bb_config_options & KAPS_LARGE_BB_WIDTH_A000_B960) && num_dbs == 1 && db_b))
        {

            device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A000_B960;

            /*
             * assign the two neighboring blocks to the database on RPB-B
             */
            if (db_b)
            {
                db_b->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                db_b->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr + 1] = 1;
                device->map->bb_map[db_group_id][bb_nr].row->owner = db_b;
                device->map->bb_map[db_group_id][bb_nr + 1].row->owner = db_b;
                cur_num_rows_for_b += device->hw_res->ads2_depth[1];
                db_b->hw_res.db_res->lsn_info[0].max_lsn_size += 2;
            }
        }
        else if ((bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B480) && num_dbs == 1 && db_b)
        {
            device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A480_B480;

            /*
             * assign the block to the database on RPB-B
             */
            if (db_b)
            {
                db_b->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr + 1] = 1;
                device->map->bb_map[db_group_id][bb_nr + 1].row->owner = db_b;
                cur_num_rows_for_b += device->hw_res->ads2_depth[1] / 2;
                db_b->hw_res.db_res->lsn_info[0].max_lsn_size++;
            }
        }
        else if (bb_config_options == KAPS_LARGE_BB_WIDTH_A480_B000
                 || ((bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B000) && num_dbs == 1 && db_a))
        {

            device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A480_B000;

            if (db_a)
            {
                db_a->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                device->map->bb_map[db_group_id][bb_nr].row->owner = db_a;
                cur_num_rows_for_a += device->hw_res->ads2_depth[0] / 2;
                db_a->hw_res.db_res->lsn_info[0].max_lsn_size++;
            }
        }
        else if (bb_config_options == KAPS_LARGE_BB_WIDTH_A960_B000
                 || ((bb_config_options & KAPS_LARGE_BB_WIDTH_A960_B000) && num_dbs == 1 && db_a))
        {

            device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A960_B000;

            if (db_a)
            {
                db_a->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                db_a->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr + 1] = 1;
                device->map->bb_map[db_group_id][bb_nr].row->owner = db_a;
                device->map->bb_map[db_group_id][bb_nr + 1].row->owner = db_a;
                cur_num_rows_for_a += device->hw_res->ads2_depth[0];
                db_a->hw_res.db_res->lsn_info[0].max_lsn_size += 2;
            }
        }
        else if ((bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B480) && num_dbs == 1 && db_a)
        {
            device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A480_B480;

            /*
             * assign the block to the database on RPB-A
             */
            if (db_a)
            {
                db_a->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                device->map->bb_map[db_group_id][bb_nr].row->owner = db_a;
                cur_num_rows_for_a += device->hw_res->ads2_depth[0] / 2;
                db_a->hw_res.db_res->lsn_info[0].max_lsn_size++;
            }
        }
        else if (bb_config_options == KAPS_LARGE_BB_WIDTH_A480_B480)
        {
            /*
             * assign one block to database of RPB-A and the neighboring block to database of RPB-B
             */
            device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A480_B480;

            if (db_a)
            {
                db_a->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                device->map->bb_map[db_group_id][bb_nr].row->owner = db_a;
                cur_num_rows_for_a += device->hw_res->ads2_depth[0] / 2;
                db_a->hw_res.db_res->lsn_info[0].max_lsn_size++;
            }

            if (db_b)
            {
                db_b->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr + 1] = 1;
                device->map->bb_map[db_group_id][bb_nr + 1].row->owner = db_b;
                cur_num_rows_for_b += device->hw_res->ads2_depth[1] / 2;
                db_b->hw_res.db_res->lsn_info[0].max_lsn_size++;
            }
        }
        else
        {
            num_unassigned_blocks++;
            unassigned_blocks[unassigned_ix] = bb_nr;
            unassigned_ix++;
        }
    }

    /*
     * Now assign the remaining blocks between the two databases in a fair way according to the ratio of the capacities
     * of the two databases
     */

    if (num_dbs > 1 && num_unassigned_blocks)
    {
        /*
         * Normalize the capacities of db_a and db_b. So db_a is IPv4 and db_b is IPv6, then capacity of db_b will get
         * multiplied by 2.
         */
        tab = db_a;
        multiplier = 1;
        while (tab)
        {
            if (tab->key->width_1 > 80)
            {
                multiplier = 2;
            }
            tab = tab->next_tab;
        }

        capacity_a = db_a->common_info->capacity * multiplier;
        if (capacity_a == 0)
            capacity_a = 1;

        tab = db_b;
        multiplier = 1;
        while (tab)
        {
            if (tab->key->width_1 > 80)
            {
                multiplier = 2;
            }
            tab = tab->next_tab;
        }

        capacity_b = db_b->common_info->capacity * multiplier;
        if (capacity_b == 0)
            capacity_b = 1;

        /*
         * Find the ratio of capacity of db_a and db_b
         */
        capacity_ratio = 1.0 * capacity_a / capacity_b;

        min_diff = 9999999.0;

        fair_cutoff = 0;

        /*
         * Find the cutoff point where all the BBs to the left of the cutoff point should be assigned to db_a and all
         * BBs to the right of the cutoff point should be assigned to db_b. Then do the assignment of BBs to the dbs
         */
        for (c = 0; c <= num_unassigned_blocks; ++c)
        {

            estimated_num_rows_for_a = cur_num_rows_for_a + (c * device->hw_res->ads2_depth[0] / 2);

            estimated_num_rows_for_b =
                cur_num_rows_for_b + ((num_unassigned_blocks - c) * device->hw_res->ads2_depth[1] / 2);

            estimated_ratio = 1.0 * estimated_num_rows_for_a / estimated_num_rows_for_b;

            cur_diff = estimated_ratio - capacity_ratio;

            if (cur_diff < 0)
                cur_diff = cur_diff * -1;

            if (cur_diff < min_diff)
            {
                min_diff = cur_diff;
                fair_cutoff = c;
            }
        }

        unassigned_ix = 0;

        for (c = 1; c <= num_unassigned_blocks; ++c)
        {
            bb_nr = unassigned_blocks[unassigned_ix];
            bb_config_options = device->map->bb_map[db_group_id][bb_nr].bb_config_options;

            if (c <= fair_cutoff)
            {
                if (bb_config_options & KAPS_LARGE_BB_WIDTH_A480_B000)
                {
                    db_a->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                    device->map->bb_map[db_group_id][bb_nr].row->owner = db_a;
                    db_a->hw_res.db_res->lsn_info[0].max_lsn_size++;
                    cur_num_rows_for_a += device->hw_res->ads2_depth[0] / 2;
                    device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A480_B000;

                }
                else if (bb_config_options & KAPS_LARGE_BB_WIDTH_A960_B000)
                {
                    db_a->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                    db_a->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr + 1] = 1;

                    device->map->bb_map[db_group_id][bb_nr].row->owner = db_a;
                    device->map->bb_map[db_group_id][bb_nr + 1].row->owner = db_a;

                    cur_num_rows_for_a += device->hw_res->ads2_depth[0];

                    db_a->hw_res.db_res->lsn_info[0].max_lsn_size += 2;

                    device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A960_B000;

                }
                else
                {
                    kaps_assert(0, "Unassigned BB\n");
                }

            }
            else
            {
                if (bb_config_options & KAPS_LARGE_BB_WIDTH_A000_B480)
                {
                    db_b->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                    device->map->bb_map[db_group_id][bb_nr].row->owner = db_b;
                    cur_num_rows_for_b += device->hw_res->ads2_depth[1] / 2;
                    db_b->hw_res.db_res->lsn_info[0].max_lsn_size++;
                    device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A000_B480;

                }
                else if (bb_config_options & KAPS_LARGE_BB_WIDTH_A000_B960)
                {
                    db_b->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr] = 1;
                    db_b->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_nr + 1] = 1;
                    device->map->bb_map[db_group_id][bb_nr].row->owner = db_b;
                    device->map->bb_map[db_group_id][bb_nr + 1].row->owner = db_b;
                    cur_num_rows_for_b += device->hw_res->ads2_depth[1];
                    db_b->hw_res.db_res->lsn_info[0].max_lsn_size += 2;
                    device->map->bb_map[db_group_id][bb_nr].bb_config = KAPS_LARGE_BB_WIDTH_A000_B960;

                }
                else
                {
                    kaps_assert(0, "Unassigned BB \n");
                }
            }

            ++unassigned_ix;
        }
    }

    /*
     * Find the start lpu and end lpu for the databases
     */
    if (db_a)
    {
        kaps_kaps2_find_start_end_lpu(device, db_a);
    }

    if (db_b)
    {
        kaps_kaps2_find_start_end_lpu(device, db_b);
    }

    return KAPS_OK;
}



static kaps_status
kaps_find_uda_mb_of_three_level_dbs(
    struct kaps_device *device)
{
    uint32_t i;
    uint32_t bb_nr;
    uint32_t num_rows;
    struct kaps_db *cur_db, *db_a = NULL, *db_b = NULL;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t db_group_id = 0;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        cur_db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (cur_db->type == KAPS_DB_LPM && cur_db->num_algo_levels_in_db == 3)
        {
            db_group_id = cur_db->db_group_id;

            if (cur_db->rpb_id == 0)
            {
                db_a = cur_db;
            }
            else
            {
                db_b = cur_db;
            }
        }
    }

    device->large_bb_mb = 0;

    if (!db_a && !db_b)
        return KAPS_OK;

    if (db_a)
    {
        db_a->common_info->total_bb_rows = 0;
        db_a->common_info->uda_mb = 0;
    }

    if (db_b)
    {
        db_b->common_info->total_bb_rows = 0;
        db_b->common_info->uda_mb = 0;
    }

    for (i = 0; i < device->hw_res->total_num_bb[0] / 2; ++i)
    {
        bb_nr = 2 * i;

        device->num_rows_in_each_hb_block[bb_nr] = device->map->bb_map[db_group_id][bb_nr].bb_num_rows;
        device->num_rows_in_each_hb_block[bb_nr + 1] = device->map->bb_map[db_group_id][bb_nr + 1].bb_num_rows;

        num_rows = device->map->bb_map[db_group_id][bb_nr].bb_num_rows;
        if (device->map->bb_map[db_group_id][bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A960_B000
            || device->map->bb_map[db_group_id][bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A000_B960)
        {
            num_rows *= 2;
        }

        if (device->map->bb_map[db_group_id][bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A480_B000
            || device->map->bb_map[db_group_id][bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A960_B000)
        {
            if (db_a)
            {
                db_a->common_info->total_bb_rows += num_rows;
            }
        }

        if (device->map->bb_map[db_group_id][bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A000_B480
            || device->map->bb_map[db_group_id][bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A000_B960)
        {
            if (db_b)
            {
                db_b->common_info->total_bb_rows += num_rows;
            }
        }

        if (device->map->bb_map[db_group_id][bb_nr].bb_config == KAPS_LARGE_BB_WIDTH_A480_B480)
        {
            if (db_a)
            {
                db_a->common_info->total_bb_rows += num_rows;
            }

            if (db_b)
            {
                db_b->common_info->total_bb_rows += num_rows;
            }
        }

        device->large_bb_mb += (num_rows * KAPS_BKT_WIDTH_1);
    }

    device->large_bb_mb /= (1024 * 1024);       /* convert from bits to mb */

    if (db_a)
    {
        db_a->common_info->uda_mb = (db_a->common_info->total_bb_rows * KAPS_BKT_WIDTH_1) / (1024 * 1024);
        db_a->common_info->total_bb_size_in_bits = db_a->common_info->total_bb_rows * KAPS_BKT_WIDTH_1;
    }

    if (db_b)
    {
        db_b->common_info->uda_mb = (db_b->common_info->total_bb_rows * KAPS_BKT_WIDTH_1) / (1024 * 1024);
        db_b->common_info->total_bb_size_in_bits = db_b->common_info->total_bb_rows * KAPS_BKT_WIDTH_1;
    }

    return KAPS_OK;
}

void
kaps_kaps2_print_ads2_depth(
    struct kaps_device *device)
{
    kaps_printf("ADS for DB-0 = %d\n", device->hw_res->ads2_depth[0]);
    kaps_printf("ADS for DB-1 = %d\n", device->hw_res->ads2_depth[1]);
}


kaps_status
kaps_kaps2_print_large_bb_config_options(
    struct kaps_device * device,
    uint32_t db_group_id,
    uint32_t print_num_rows)
{
    uint32_t i;
    uint32_t options = 0;

    for (i = 0; i < device->hw_res->total_num_bb[db_group_id] / 2; ++i)
    {
        options = device->map->bb_map[db_group_id][2 * i].bb_config_options;

        if (options != KAPS_LARGE_BB_WIDTH_A000_B000)
        {
            kaps_printf("Blk %d = ", i);

            if (options & KAPS_LARGE_BB_WIDTH_A480_B000) 
            {
                kaps_printf("A480 | ");
            }

            if (options & KAPS_LARGE_BB_WIDTH_A960_B000) 
            {
                kaps_printf("A960 | ");
            }

            if (options & KAPS_LARGE_BB_WIDTH_A000_B480) 
            {
                kaps_printf("B480 | ");
            }

            if (options & KAPS_LARGE_BB_WIDTH_A000_B960) 
            {
                kaps_printf("B960 | ");
            }

            if (options & KAPS_LARGE_BB_WIDTH_A480_B480)
            {
                kaps_printf("A480_B480 | ");
            }

            if (print_num_rows)
            {
                kaps_printf("Number of rows in BB = %d\n", device->map->bb_map[db_group_id][2 * i].bb_num_rows);
            }

            kaps_printf("\n");
        }
    }

    kaps_printf("\n\n");

    return KAPS_OK;
}


kaps_status
kaps_kaps2_read_large_bb_config_options(
    struct kaps_device * device,
    uint32_t log_info)
{
    uint32_t i;
    uint32_t options = 0;
    uint32_t db_group_id = 0;

    for (i = 0; i < device->hw_res->total_num_bb[db_group_id] / 2; ++i)
    {
        kaps_dm_kaps_read_bb_config_options(device, i, &options);
        device->map->bb_map[db_group_id][2 * i].bb_config_options = options;
    }

    if (log_info) 
    {
        kaps_printf("BB options supported by device \n");
        kaps_kaps2_print_large_bb_config_options(device, db_group_id, 0);
    }

    return KAPS_OK;
}



kaps_status
kaps_kaps2_write_large_bb_config_option(
    struct kaps_device * device)
{
    uint32_t i;
    uint32_t option = 0;
    uint32_t db_group_id = 0;

    for (i = 0; i < device->hw_res->total_num_bb[db_group_id] / 2; ++i)
    {
        option = device->map->bb_map[db_group_id][2 * i].bb_config;
        kaps_dm_kaps_write_bb_config_option(device, i, option);
        device->kaps2_hb_config[2 * i] = option;
    }

    return KAPS_OK;
}

kaps_status
kaps_kaps2_read_large_bb_num_rows(
    struct kaps_device * device,
    uint32_t log_info)
{
    uint32_t i;
    uint32_t num_rows = 0;
    uint32_t db_group_id = 0;

    for (i = 0; i < device->hw_res->total_num_bb[db_group_id] / 2; ++i)
    {
        kaps_dm_kaps_read_large_bb_num_rows(device, i, &num_rows);
        device->map->bb_map[db_group_id][2 * i].bb_num_rows = num_rows;
        device->map->bb_map[db_group_id][(2 * i) + 1].bb_num_rows = num_rows;
    }

    if (log_info)
    {
        kaps_printf("Configured BB options \n");
        kaps_kaps2_print_large_bb_config_options(device, db_group_id, 1);
    }

    return KAPS_OK;
}



kaps_status
kaps_rxc_process_jr1(
    struct kaps_device *device)
{
    
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t nlpm = 0;
    uint32_t db_id = 0;
    struct kaps_db *tmp;
    uint8_t *data = NULL;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_LPM)
        {
            nlpm++;
        }
    }

    if (nlpm > device->num_ab[0])
        return KAPS_RESOURCE_FIT_FAIL;

    if (device->id == KAPS_DEFAULT_DEVICE_ID)
    {
        /*
         * Reset the KAPS Blocks
         */
        KAPS_STRY(kaps_dm_kaps_reset_blks(device, 0, 0, 0, data));
        /*
         * Enumerate the KAPS Blocks
         */
        KAPS_STRY(kaps_dm_kaps_enumerate_blks(device, 0, 0, 0, data));
    }


    KAPS_STRY(kaps_jr1_resource_assign_rpbs(device));


    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        /*
         * LTR DB ID doesn't mean anything for KAPS * Assigned only for consistency purposes
         */
        if (db->type == KAPS_DB_LPM)
        {
            for (tmp = db; tmp; tmp = tmp->next_tab)
                tmp->ltr_db_id = db_id++;
        }
    }

    KAPS_STRY(kaps_jr1_allocate_bbs(device));

    KAPS_STRY(kaps_fit_kaps_bbs(device));

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_LPM)
        {
            db->common_info->finalized_resources = 1;

            if (db->type == KAPS_DB_LPM && device->num_hb_blocks)
            {
                db->common_info->start_hb_blk = db->hw_res.db_res->start_lpu * device->hw_res->num_bb_in_one_chain;
                db->common_info->num_hb_blks =
                    db->hw_res.db_res->lsn_info[0].max_lsn_size * device->hw_res->num_bb_in_one_chain;

            }
        }
    }

    return KAPS_OK;
}


kaps_status
kaps_rxc_process_jr2(
    struct kaps_device *device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t i;
    uint32_t log_info = device->debug_level;

    if (log_info)
        kaps_kaps2_print_ads2_depth(device);

    KAPS_STRY(kaps_jr2_resource_assign_rpbs(device));

    if (device->hw_res->num_algo_levels == 3)
    {
        KAPS_STRY(kaps_kaps2_read_large_bb_config_options(device, log_info));
        KAPS_STRY(kaps_jr2_assign_num_algo_levels_in_big_device(device));
    }


    if (device->hw_res->num_algo_levels == 3)
    {
        KAPS_STRY(kaps_resource_assign_kaps_small_bb_abs(device));
    }

    KAPS_STRY(kaps_allocate_fit_small_bbs(device));
    KAPS_STRY(kaps_jr2_small_bb_hw_config(device));

    if (device->hw_res->num_algo_levels == 3)
    {
        KAPS_STRY(kaps_kaps2_allocate_fit_large_bbs(device));
        KAPS_STRY(kaps_kaps2_write_large_bb_config_option(device));
        KAPS_STRY(kaps_kaps2_read_large_bb_num_rows(device, log_info));
    }

    KAPS_STRY(kaps_find_uda_mb_of_three_level_dbs(device));


    
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_LPM )
        {
            db->common_info->finalized_resources = 1;

            if (device->num_hb_blocks)
            {
                if (db->num_algo_levels_in_db == 3)
                {
                    db->common_info->kaps2_large_hb_blk_bmp = 0;

                    for (i = 0; i < device->num_hb_blocks; ++i)
                    {
                        if (db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][i])
                        {
                            db->common_info->kaps2_large_hb_blk_bmp |= (1u << i);
                        }
                    }

                }
                else
                {
                    db->common_info->start_hb_blk = db->hw_res.db_res->start_lpu * device->hw_res->num_bb_in_one_chain;
                    db->common_info->num_hb_blks =
                        db->hw_res.db_res->lsn_info[0].max_lsn_size * device->hw_res->num_bb_in_one_chain;
                }
            }
        }
    }

    return KAPS_OK;
    
}



/**
 * Allocate resources to the databases for a KAPS device
 *
 * @param device KBP device handle
 *
 * @return KAPS_OK on success or an error code
 */

kaps_status
kaps_resource_process(
    struct kaps_device * device)
{
    if (device->id < KAPS_JERICHO_2_DEVICE_ID)
    {
        KAPS_STRY(kaps_rxc_process_jr1(device));
    } 
    else if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        KAPS_STRY(kaps_rxc_process_jr2(device));
    }
    

    return KAPS_OK;
        
}


kaps_status
kaps_resource_kaps_dynamic_uda_alloc(
    struct kaps_device * device,
    struct memory_map * mem_map,
    struct kaps_db * db,
    int8_t dt_index,
    int32_t * p_num_lpu,
    uint8_t udms[])
{
    int32_t bb_index, i, num_bb;
    uint32_t db_group_id = db->db_group_id;

    if (db->common_info->enable_dynamic_allocation == 0)
        return KAPS_OUT_OF_UDA;

    if (mem_map->bb_map[db_group_id][0].row->owner == NULL)
    {
        bb_index = 0;
        num_bb = device->hw_res->incr_in_bbs;
        while (num_bb)
        {
            if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_chain))
                return KAPS_OUT_OF_UDA;

            for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
            {
                if (device->map->bb_map[db_group_id][bb_index].row->owner)
                    return KAPS_OUT_OF_UDA;
                bb_index++;
            }
            num_bb--;
        }
        db->hw_res.db_res->start_lpu = 0;
        db->hw_res.db_res->end_lpu = (bb_index / device->hw_res->num_bb_in_one_chain) - 1;

        for (bb_index = db->hw_res.db_res->start_lpu; bb_index <= db->hw_res.db_res->end_lpu; bb_index++)
        {
            int32_t bb_blk_id = bb_index * device->hw_res->num_bb_in_one_chain;
            int32_t i = 0;

            db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index] = 1;
            for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
            {
                kaps_sassert(device->map->bb_map[db_group_id][bb_blk_id].row->owner == NULL);
                device->map->bb_map[db_group_id][bb_blk_id].row->owner = db;
                bb_blk_id++;
            }
        }

    }
    else
    {

        bb_index = (device->map->num_bb - device->hw_res->incr_in_bbs) * device->hw_res->num_bb_in_one_chain;
        num_bb = device->hw_res->incr_in_bbs;
        while (num_bb)
        {
            if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_chain))
                return KAPS_OUT_OF_UDA;

            for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
            {
                if (device->map->bb_map[db_group_id][bb_index].row->owner)
                    return KAPS_OUT_OF_UDA;
                bb_index++;
            }
            num_bb--;
        }
        db->hw_res.db_res->start_lpu = device->map->num_bb - device->hw_res->incr_in_bbs;
        db->hw_res.db_res->end_lpu = device->map->num_bb - 1;

        for (bb_index = db->hw_res.db_res->start_lpu; bb_index <= db->hw_res.db_res->end_lpu; bb_index++)
        {
            int32_t bb_blk_id = bb_index * device->hw_res->num_bb_in_one_chain;
            int32_t i = 0;

            db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index] = 1;
            for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
            {
                kaps_sassert(device->map->bb_map[db_group_id][bb_blk_id].row->owner == NULL);
                device->map->bb_map[db_group_id][bb_blk_id].row->owner = db;
                bb_blk_id++;
            }
        }
    }

    *p_num_lpu = device->hw_res->incr_in_bbs;
    for (i = 0; i < *p_num_lpu; i++)
    {
        udms[i] = (db->hw_res.db_res->start_lpu + i) * KAPS_UDM_PER_UDC;
    }

    KAPS_STRY(kaps_resource_kaps_configure_bbs(device, db));
    return KAPS_OK;
}

kaps_status
kaps_resource_kaps_expand_uda_mgr_regions(
    struct kaps_uda_mgr * mgr)
{
    struct kaps_db *db = mgr->db;
    struct kaps_device *device = db->device;
    int32_t bb_index, i, num_bb;
    uint32_t db_group_id = db->db_group_id;

    if (mgr->db->common_info->enable_dynamic_allocation == 0 || mgr->enable_compaction == 0)
        return KAPS_OUT_OF_UDA;

    /*
     * try growing towards right
     */
    bb_index = (db->hw_res.db_res->end_lpu + 1) * device->hw_res->num_bb_in_one_chain;
    num_bb = device->hw_res->incr_in_bbs;
    while (num_bb)
    {

        if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_chain))
            break;

        for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
        {
            if (device->map->bb_map[db_group_id][bb_index].row->owner)
                break;
            
            bb_index++;
        }

        if (i < device->hw_res->num_bb_in_one_chain)
            break;

        num_bb--;
    }

    if (num_bb == 0)
    {
        int32_t new_region_no, old_num_lpu;
        int32_t region_no = 0;
        int32_t i, width;

        new_region_no = mgr->no_of_regions;
        old_num_lpu = mgr->region_info[region_no].num_lpu;
        width = device->hw_res->incr_in_bbs;

        kaps_memcpy(&mgr->region_info[new_region_no], &mgr->region_info[region_no], sizeof(mgr->region_info[0]));

        for (i = 0; i < old_num_lpu; i++)
        {
            mgr->region_info[new_region_no].udms[i] = mgr->region_info[region_no].udms[i];
        }

        for (i = 0; i < width; i++)
        {
            mgr->region_info[new_region_no].udms[old_num_lpu + i] =
                (db->hw_res.db_res->end_lpu + 1 + i) * KAPS_UDM_PER_UDC;
        }

        mgr->region_info[new_region_no].num_lpu += width;

        KAPS_STRY(kaps_uda_mgr_compact(mgr, 0, new_region_no));
        mgr->uda_mgr_update_lsn_size(mgr->db);

        bb_index = (db->hw_res.db_res->end_lpu + 1) * device->hw_res->num_bb_in_one_chain;
        num_bb = device->hw_res->incr_in_bbs;
        while (num_bb)
        {

            db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index / device->hw_res->num_bb_in_one_chain] = 1;
            for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
            {
                device->map->bb_map[db_group_id][bb_index].row->owner = db;
                bb_index++;
            }

            num_bb--;
        }

        db->hw_res.db_res->end_lpu += device->hw_res->incr_in_bbs;
        db->hw_res.db_res->lsn_info[0].max_lsn_size += device->hw_res->incr_in_bbs;
        mgr->max_lpus_in_a_chunk += device->hw_res->incr_in_bbs;
        KAPS_STRY(kaps_resource_kaps_configure_bbs(device, db));
        return KAPS_OK;
    }

    /*
     * try growing towards left
     */
    bb_index = (db->hw_res.db_res->start_lpu - device->hw_res->incr_in_bbs) * device->hw_res->num_bb_in_one_chain;
    num_bb = device->hw_res->incr_in_bbs;
    while (num_bb > 0)
    {

        if (bb_index < 0)
            break;

        if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_chain))
            break;

        for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
        {
            if (device->map->bb_map[db_group_id][bb_index].row->owner)
                break;
            
            bb_index++;
        }

        if (i < device->hw_res->num_bb_in_one_chain)
            break;

        num_bb--;
    }

    if (num_bb == 0)
    {
        int32_t new_region_no, old_num_lpu;
        int32_t region_no = 0;
        int32_t i, width;

        new_region_no = mgr->no_of_regions;
        old_num_lpu = mgr->region_info[region_no].num_lpu;
        width = device->hw_res->incr_in_bbs;

        kaps_memcpy(&mgr->region_info[new_region_no], &mgr->region_info[region_no], sizeof(mgr->region_info[0]));

        for (i = 0; i < width; i++)
        {
            mgr->region_info[new_region_no].udms[i] = (db->hw_res.db_res->start_lpu - width + i) * KAPS_UDM_PER_UDC;
        }

        for (i = 0; i < old_num_lpu; i++)
        {
            kaps_sassert(width + i < MAX_NUM_CHUNK_LEVELS);
            mgr->region_info[new_region_no].udms[width + i] = mgr->region_info[region_no].udms[i];
        }

        mgr->region_info[new_region_no].num_lpu += width;

        KAPS_STRY(kaps_uda_mgr_compact(mgr, 0, new_region_no));
        mgr->uda_mgr_update_lsn_size(mgr->db);

        bb_index = (db->hw_res.db_res->start_lpu - device->hw_res->incr_in_bbs) * device->hw_res->num_bb_in_one_chain;
        num_bb = device->hw_res->incr_in_bbs;
        while (num_bb > 0)
        {
            db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index / device->hw_res->num_bb_in_one_chain] = 1;
            for (i = 0; i < device->hw_res->num_bb_in_one_chain; i++)
            {
                device->map->bb_map[db_group_id][bb_index].row->owner = db;
                bb_index++;
            }
            num_bb--;
        }

        db->hw_res.db_res->start_lpu -= device->hw_res->incr_in_bbs;
        db->hw_res.db_res->lsn_info[0].max_lsn_size += device->hw_res->incr_in_bbs;
        mgr->max_lpus_in_a_chunk += device->hw_res->incr_in_bbs;
        KAPS_STRY(kaps_resource_kaps_configure_bbs(device, db));
        return KAPS_OK;
    }

    return KAPS_OUT_OF_UDA;
}

static void
kaps_print_db_key(
    struct kaps_key *key,
    FILE * fp)
{
    struct kaps_key_field *field = key->first_field;
    uint32_t fcount = 0, count = 0, next_offset = 0;

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_KEY_FIELD_HOLE && !field->is_usr_bmr)
            continue;
        fcount++;
    }
    do
    {
        for (field = key->first_field; field; field = field->next)
        {
            if (next_offset != field->orig_offset_1)
                continue;
            count++;
            next_offset = next_offset + field->width_1;
            kaps_print_to_file(key->device, fp, "%s(%d) ", field->name, field->width_1);
            break;
        }
    }
    while (count < fcount);
}

/*
 * Device print utility function to print capacity
 */
static void
kaps_print_capacity(
    struct kaps_device *device,
    uint32_t capacity,
    FILE * f)
{
    if (capacity >= 1024 && capacity % 1024 == 0)
    {
        capacity /= 1024;
        if (capacity >= 1024 && capacity % 1024 == 0)
        {
            capacity /= 1024;
            kaps_print_to_file(device, f, "%dM", capacity);
        }
        else
        {
            kaps_print_to_file(device, f, "%dK", capacity);
        }
    }
    else
    {
        kaps_print_to_file(device, f, "%d", capacity);
    }
}

static void
kaps_print_db_name(
    struct kaps_db *db,
    FILE * f)
{
    if (db->parent)
    {
        if (db->is_clone)
        {
            if (!db->clone_of->parent)
                kaps_print_to_file(db->device, f, "%s%d Clone%d", kaps_device_db_name(db->parent), db->clone_of->tid,
                                   db->tid);
            else
                kaps_print_to_file(db->device, f, "%s%d Table%d Clone%d",
                                   kaps_device_db_name(db->clone_of->parent),
                                   db->clone_of->parent->tid, db->clone_of->tid, db->tid);
        }
        else
        {
            kaps_print_to_file(db->device, f, "%s%d Table%d", kaps_device_db_name(db->parent), db->parent->tid,
                               db->tid);
        }
    }
    else
    {
        kaps_print_to_file(db->device, f, "%s%d", kaps_device_db_name(db), db->tid);
    }
}

/*
 * Device print utility function
 * to pretty print KAPS dba allocation
 * across the Databases to an HTML output file
 */
static void
kaps_print_dba(
    struct kaps_device *device,
    FILE * f,
    struct memory_map *mem_map)
{
    float ystart;
    int32_t i, j, k, count = 0, ab_no = 0;
    float xstart = 6.5;

    kaps_print_to_file(device, f, "<text x=\"12.7cm\" y=\"1.2cm\" "
                       "font-family=\"Verdana\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" >\n");
    kaps_print_to_file(device, f, "RPBs\n");
    kaps_print_to_file(device, f, "</text>\n");

    kaps_print_to_file(device, f, "<g id=\"RPB\">\n");
    ystart = 1.5;

    for (i = 0; i < device->num_dba_sb; i++)
    {
        kaps_print_to_file(device, f, "    <g id=\"RPB%d\">\n", count * 8 + i);
        for (k = 0; k < device->num_ab_per_sb; k++)
        {
            struct kaps_db *db = device->map->ab_memory[ab_no].db;

            kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"2.50cm\" "
                               "height=\"2.50cm\" fill=\"%s\" stroke=\"black\" stroke-width=\".02cm\"/>\n",
                               xstart, ystart, db ? db->hw_res.db_res->color : "none");
            if (db && db->is_clone)
            {
                for (j = 0; j < 16; j++)
                {
                    kaps_print_to_file(device, f, "    <line x1=\"%.02fcm\" y1=\"%.02fcm\" x2=\"%.02fcm\" "
                                       "y2=\"%.02fcm\" stroke=\"black\"/>\n",
                                       xstart, ystart + j * 0.15, xstart + 2.50, ystart + j * 0.15 + 0.15);
                }
            }
            ab_no++;
        }
        xstart += 3.25;
        kaps_print_to_file(device, f, "    </g>\n");
    }

    kaps_print_to_file(device, f, "</g>\n");
}

static void
kaps_print_rpbs(
    struct kaps_device *device,
    FILE * f,
    struct memory_map *mem_map,
    float *ystart)
{
    float xstart = 6;
    int32_t i = 0;

    kaps_print_to_file(device, f, "<text x=\"11.1cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" >\n", *ystart);
    kaps_print_to_file(device, f, "Root Pivot Blocks\n");
    kaps_print_to_file(device, f, "</text>\n");

    kaps_print_to_file(device, f, "<g id=\"RPB\">\n");
    *ystart = *ystart + 0.3;

    kaps_print_to_file(device, f, "<g id=\"UDA\">\n");
    kaps_print_to_file(device, f, "    <g id=\"sbb\"/>\n");
    kaps_print_to_file(device, f, "        <rect x=\"6.0cm\" y=\"%.02fcm\" width=\"13.5cm\" "
                       "height=\"0.15cm\" fill=\"black\"/>\n", *ystart);
    kaps_print_to_file(device, f, "    </g>\n");
    *ystart = *ystart + 0.4;

    kaps_print_to_file(device, f, "<text x=\"5.0cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"black\" >\n",
                       *ystart + 0.7);
    kaps_print_to_file(device, f, "(%d)", i);
    kaps_print_to_file(device, f, "</text>\n");

    for (; i < device->max_num_searches; i++)
    {
        struct kaps_db *db = mem_map->rpb_info[i].db;
        char *border_color;

#if 0
        if (!i)
            border_color = "green";
        else
            border_color = "red";

        if (db == NULL)
#endif
            border_color = "black";

        kaps_print_to_file(device, f, "    <g id=\"RPB%d\">\n", i);

        kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"1.25cm\" "
                           "height=\"1.25cm\" fill=\"%s\" stroke=\"%s\" stroke-width=\".05cm\"/>\n",
                           xstart, *ystart, db ? db->hw_res.db_res->color : "none", border_color);

        if (db)
        {
            kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                               "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"white\" >\n",
                               xstart + 0.35, *ystart + 0.7);
            kaps_print_capacity(device, mem_map->rpb_info[i].rpt_depth, f);
            kaps_print_to_file(device, f, "</text>\n");
        }

        xstart += 1.75;
        kaps_print_to_file(device, f, "    </g>\n");
    }
    kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"black\" >\n", xstart,
                       *ystart + 0.7);
    kaps_print_to_file(device, f, "(%d)", i - 1);
    kaps_print_to_file(device, f, "</text>\n");
    *ystart = *ystart + 1.25;

    kaps_print_to_file(device, f, "</g>\n");
}

static void
kaps_print_small_bbs(
    struct kaps_device *device,
    FILE * f,
    struct memory_map *mem_map,
    float *ystart)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_db *db_a = NULL, *db_b = NULL, *db_array[32] = { 0, };
    int32_t i;
    float xstart;
    uint32_t db_group_id = 0;

    *ystart = *ystart + 1.5;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_DB_LPM)
            continue;

        if (db->rpb_id == 0)
        {
            db_a = db;
            db_group_id = db_a->db_group_id;
        }
        else
        {
            db_b = db;
            db_group_id = db_b->db_group_id;
        }
    }

    for (i = db_a->hw_res.db_res->start_small_bb_nr; i <= db_a->hw_res.db_res->end_small_bb_nr; i++)
        db_array[i] = db_a;

    if (db_b)
    {
        for (i = db_b->hw_res.db_res->start_small_bb_nr; i <= db_b->hw_res.db_res->end_small_bb_nr; i++)
            db_array[i] = db_b;
    }

    *ystart = *ystart + 0.2;

    kaps_print_to_file(device, f, "<text x=\"11.0cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" >\n", *ystart);
    kaps_print_to_file(device, f, "Small Bucket Blocks\n");
    kaps_print_to_file(device, f, "</text>\n");

    *ystart = *ystart + 0.2;

    kaps_print_to_file(device, f, "<g id=\"UDA\">\n");
    kaps_print_to_file(device, f, "    <g id=\"sbb\"/>\n");
    kaps_print_to_file(device, f, "        <rect x=\"5.0cm\" y=\"%.02fcm\" width=\"15.7cm\" "
                       "height=\"0.15cm\" fill=\"black\"/>\n", *ystart);
    kaps_print_to_file(device, f, "    </g>\n");
    xstart = 5.0;
    *ystart = *ystart + 0.4;

    i = 0;
    kaps_print_to_file(device, f, "<text x=\"4.1cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"black\" >\n",
                       *ystart + 0.45);
    kaps_print_to_file(device, f, "(%d)", i);
    kaps_print_to_file(device, f, "</text>\n");

    for (i = 0; i < device->hw_res->total_small_bb[db_group_id]; i++)
    {
        struct kaps_db *db = db_array[i];

        kaps_print_to_file(device, f, "    <g id=\"sbb%d\">\n", i);

        kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"0.7cm\" "
                           "height=\"0.7cm\" fill=\"%s\" stroke=\"black\" stroke-width=\".05cm\"/>\n",
                           xstart, *ystart, db ? db->hw_res.db_res->color : "none");
        kaps_print_to_file(device, f, "    </g>\n");

        if (db)
        {
            kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                               "font-family=\"Verdana\" font-size=\"9\" font-weight=\"bold\" fill=\"white\" >\n",
                               xstart + 0.1, *ystart + 0.4);
            kaps_print_to_file(device, f, "512");
            kaps_print_to_file(device, f, "</text>\n");
        }

        xstart += 1.0;
    }

    kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"black\" >\n", xstart,
                       *ystart + 0.45);
    kaps_print_to_file(device, f, "(%d)", i - 1);
    kaps_print_to_file(device, f, "</text>\n");

    *ystart += 1.0;
    kaps_print_to_file(device, f, "</g>\n");
}

static void
kaps_print_large_bbs(
    struct kaps_device *device,
    FILE * f,
    struct memory_map *mem_map,
    float *ystart)
{
    int32_t i;
    float xstart, rect_ystart;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_db *db_a = NULL, *db_b = NULL;
    uint32_t db_group_id = 0;

    (void) db_b;
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_DB_LPM)
            continue;

        db_group_id = db->db_group_id;

        if (db->rpb_id == 0)
            db_a = db;
        else
            db_b = db;
    }

    *ystart = *ystart + 1.5;

    kaps_print_to_file(device, f, "<text x=\"11.0cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"16\" font-weight=\"bold\" fill=\"black\" >\n", *ystart);
    kaps_print_to_file(device, f, "Large Bucket Blocks\n");
    kaps_print_to_file(device, f, "</text>\n");

    *ystart = *ystart + 0.2;

    rect_ystart = *ystart;

    xstart = 1.0;
    *ystart += 0.4;

    for (i = 0; i < device->hw_res->total_num_bb[0] / 2; ++i)
    {
        int32_t bb_nr = 2 * i;
        enum large_bb_config_type config = mem_map->bb_map[db_group_id][bb_nr].bb_config;
        struct kaps_db *db_1 = mem_map->bb_map[db_group_id][bb_nr].row[0].owner;
        struct kaps_db *db_2 = mem_map->bb_map[db_group_id][bb_nr + 1].row[0].owner;
        int32_t large_bb = 0, num_rows[2] = { 0, };
        const char *color[2] = { 0, };

        switch (config)
        {
            case KAPS_LARGE_BB_WIDTH_A000_B000:
                color[0] = "grey";
                color[1] = "grey";
                num_rows[0] = 0;
                num_rows[1] = 0;
                large_bb = 1;
                break;
            case KAPS_LARGE_BB_WIDTH_A000_B480:
                color[0] = (db_1) ? db_1->hw_res.db_res->color : "none";
                color[1] = "grey";
                num_rows[0] =
                    (db_1) ? ((db_1 == db_a) ? device->hw_res->ads2_depth[0] : device->hw_res->ads2_depth[1]) : 0;
                num_rows[1] = 0;
                large_bb = 0;
                break;
            case KAPS_LARGE_BB_WIDTH_A000_B960:
                color[0] = (db_2) ? db_2->hw_res.db_res->color : "none";
                color[1] = (db_2) ? db_2->hw_res.db_res->color : "none";
                large_bb = 1;
                num_rows[0] =
                    (db_2) ? ((db_2 == db_a) ? device->hw_res->ads2_depth[0] : device->hw_res->ads2_depth[1]) : 0;
                num_rows[1] =
                    (db_2) ? ((db_2 == db_a) ? device->hw_res->ads2_depth[0] : device->hw_res->ads2_depth[1]) : 0;
                break;
            case KAPS_LARGE_BB_WIDTH_A480_B000:
                color[0] = (db_1) ? db_1->hw_res.db_res->color : "none";
                color[1] = "grey";
                large_bb = 0;
                num_rows[0] =
                    (db_1) ? ((db_1 == db_a) ? device->hw_res->ads2_depth[0] : device->hw_res->ads2_depth[1]) : 0;
                num_rows[1] = 0;
                break;
            case KAPS_LARGE_BB_WIDTH_A480_B480:
                color[0] = (db_1) ? db_1->hw_res.db_res->color : "none";
                color[1] = (db_2) ? db_2->hw_res.db_res->color : "none";
                num_rows[0] =
                    (db_1) ? ((db_1 == db_a) ? device->hw_res->ads2_depth[0] : device->hw_res->ads2_depth[1]) : 0;
                num_rows[1] =
                    (db_2) ? ((db_2 == db_a) ? device->hw_res->ads2_depth[0] : device->hw_res->ads2_depth[1]) : 0;
                large_bb = 0;
                break;
            case KAPS_LARGE_BB_WIDTH_A960_B000:
                color[0] = (db_1) ? db_1->hw_res.db_res->color : "none";
                color[1] = (db_1) ? db_1->hw_res.db_res->color : "none";
                num_rows[0] =
                    (db_1) ? ((db_1 == db_a) ? device->hw_res->ads2_depth[0] : device->hw_res->ads2_depth[1]) : 0;
                num_rows[1] =
                    (db_1) ? ((db_1 == db_a) ? device->hw_res->ads2_depth[0] : device->hw_res->ads2_depth[1]) : 0;
                large_bb = 1;
                break;
            default:
                kaps_sassert(0);
        }

        kaps_print_to_file(device, f, "    <g id=\"lbb%d\">\n", i);

        if (large_bb)
        {
            kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"%.02fcm\" "
                               "height=\"%.02fcm\" fill=\"%s\" stroke=\"black\" stroke-width=\".05cm\"/>\n",
                               xstart, *ystart, (num_rows[0]) ? 2.0 : 1.0,
                               ((num_rows[0] / 2 == 16 * 1024) || !num_rows[0]) ? 2.0 : 1.0, color[0]);

            kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                               "font-family=\"Verdana\" font-size=\"9\" font-weight=\"bold\" fill=\"white\" >\n",
                               xstart + 0.8,
                               *ystart + ((((num_rows[0] / 2 == 16 * 1024) || !num_rows[0])) ? 1.0 : 0.6));
            if (num_rows[0] != 0)
                kaps_print_capacity(device, num_rows[0] / 2, f);
            kaps_print_to_file(device, f, "</text>\n");

            if (num_rows[0])
                xstart += 1.0;

        }
        else
        {
            kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"1.0cm\" "
                               "height=\"%.02fcm\" fill=\"%s\" stroke=\"black\" stroke-width=\".05cm\"/>\n",
                               xstart, *ystart, (num_rows[0] / 2 == 16 * 1024) ? 2.0 : 1.0, color[0]);

            kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                               "font-family=\"Verdana\" font-size=\"9\" font-weight=\"bold\" fill=\"white\" >\n",
                               xstart + 0.25, *ystart + ((num_rows[0] / 2 == 16 * 1024) ? 1.0 : 0.6));
            if (num_rows[0] != 0)
                kaps_print_capacity(device, num_rows[0] / 2, f);
            kaps_print_to_file(device, f, "</text>\n");

            if (num_rows[1] != 0)
            {
                kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"1.0cm\" "
                                   "height=\"%.02fcm\" fill=\"%s\" stroke=\"black\" stroke-width=\".05cm\"/>\n",
                                   xstart, *ystart + 1.3, (num_rows[1] / 2 == 16 * 1024) ? 2.0 : 1.0, color[1]);

                kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                                   "font-family=\"Verdana\" font-size=\"9\" font-weight=\"bold\" fill=\"white\" >\n",
                                   xstart + 0.25, *ystart + 1.0);
                kaps_print_capacity(device, num_rows[1] / 2, f);
                kaps_print_to_file(device, f, "</text>\n");
            }
        }
        kaps_print_to_file(device, f, "    </g>\n");
        xstart += 1.2;
    }
    kaps_print_to_file(device, f, "</g>\n");

    kaps_print_to_file(device, f, "<g id=\"UDA\">\n");
    kaps_print_to_file(device, f, "    <g id=\"lbb\"/>\n");
    kaps_print_to_file(device, f, "        <rect x=\"1.0cm\" y=\"%.02fcm\" width=\"%.02fcm\" "
                       "height=\"0.15cm\" fill=\"black\"/>\n", rect_ystart, xstart - 1.2);
    kaps_print_to_file(device, f, "    </g>\n");
}

static void
kaps_print_ads1(
    struct kaps_device *device,
    FILE * f,
    struct memory_map *mem_map,
    float *ystart)
{
    float xstart = 9.9;
    int32_t i = 0;

    *ystart = *ystart + 1.5;

    kaps_print_to_file(device, f, "<text x=\"12.0cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" >\n", *ystart);
    kaps_print_to_file(device, f, "ADS - 1\n");
    kaps_print_to_file(device, f, "</text>\n");

    *ystart = *ystart + 0.2;

    kaps_print_to_file(device, f, "<g id=\"ADS1\">\n");
    kaps_print_to_file(device, f, "    <g id=\"ads1\"/>\n");
    kaps_print_to_file(device, f, "        <rect x=\"9.9cm\" y=\"%.02fcm\" width=\"5.75cm\" "
                       "height=\"0.15cm\" fill=\"black\"/>\n", *ystart);
    kaps_print_to_file(device, f, "    </g>\n");

    *ystart = *ystart + 0.15;

    kaps_print_to_file(device, f, "<text x=\"9.0cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"11\" font-weight=\"bold\" fill=\"black\" >\n",
                       *ystart + 0.7);
    kaps_print_to_file(device, f, "(%d)", i);
    kaps_print_to_file(device, f, "</text>\n");

    kaps_print_to_file(device, f, "<g id=\"ADS1\">\n");

    *ystart = *ystart + 0.3;

    for (i = 0; i < device->max_num_searches; i++)
    {
        struct kaps_db *db = mem_map->rpb_info[i].db;
        char *border_color;
#if 0
        if (!i)
            border_color = "green";
        else
            border_color = "red";

        if (db == NULL)
#endif
            border_color = "black";

        kaps_print_to_file(device, f, "    <g id=\"ads%d\">\n", i);

        kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"0.5cm\" "
                           "height=\"0.5cm\" fill=\"%s\" stroke=\"%s\" stroke-width=\".05cm\"/>\n",
                           xstart, *ystart, db ? db->hw_res.db_res->color : "none", border_color);
        xstart += 0.75;
        kaps_print_to_file(device, f, "    </g>\n");
    }

    kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"11\" font-weight=\"bold\" fill=\"black\" >\n", xstart,
                       *ystart + 0.4);
    kaps_print_to_file(device, f, "(%d)", i - 1);
    kaps_print_to_file(device, f, "</text>\n");

    *ystart = *ystart + 0.5;

    kaps_print_to_file(device, f, "</g>\n");
}

static void
kaps_print_ads2(
    struct kaps_device *device,
    FILE * f,
    struct memory_map *mem_map,
    float *ystart)
{
    float xstart = 11.4;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_db *db_a = NULL, *db_b = NULL;

    *ystart = *ystart + 1.5;

    kaps_print_to_file(device, f, "<text x=\"12.0cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" >\n", *ystart);
    kaps_print_to_file(device, f, "ADS - 2\n");
    kaps_print_to_file(device, f, "</text>\n");

    *ystart = *ystart + 0.2;

    kaps_print_to_file(device, f, "<g id=\"ADS2\">\n");
    kaps_print_to_file(device, f, "    <g id=\"ads2\"/>\n");
    kaps_print_to_file(device, f, "        <rect x=\"11.4cm\" y=\"%.02fcm\" width=\"2.75cm\" "
                       "height=\"0.15cm\" fill=\"black\"/>\n", *ystart);
    kaps_print_to_file(device, f, "    </g>\n");

    *ystart = *ystart + 0.2;

    kaps_print_to_file(device, f, "<g id=\"ADS2\">\n");
    *ystart = *ystart + 0.2;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_DB_LPM)
            continue;

        if (db->rpb_id == 0)
            db_a = db;
        else
            db_b = db;
    }

    kaps_print_to_file(device, f, "<text x=\"10.4cm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"black\" >\n",
                       *ystart + 0.7);
    kaps_print_to_file(device, f, "(0)");
    kaps_print_to_file(device, f, "</text>\n");

    kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"1.25cm\" "
                       "height=\"1.25cm\" fill=\"%s\" stroke=\"black\" stroke-width=\".05cm\"/>\n",
                       xstart, *ystart, db_a ? db_a->hw_res.db_res->color : "none");

    kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"white\" >\n",
                       xstart + 0.25, *ystart + 0.7);
    kaps_print_capacity(device, device->hw_res->ads2_depth[0], f);
    kaps_print_to_file(device, f, "</text>\n");

    xstart += 1.5;

    kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"1.25cm\" "
                       "height=\"1.25cm\" fill=\"%s\" stroke=\"black\" stroke-width=\".05cm\"/>\n",
                       xstart, *ystart, db_b ? db_b->hw_res.db_res->color : "none");

    kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"white\" >\n",
                       xstart + 0.25, *ystart + 0.7);
    kaps_print_capacity(device, device->hw_res->ads2_depth[1], f);
    kaps_print_to_file(device, f, "</text>\n");
    xstart += 1.5;

    kaps_print_to_file(device, f, "<text x=\"%.02fcm\" y=\"%.02fcm\" "
                       "font-family=\"Verdana\" font-size=\"12\" font-weight=\"bold\" fill=\"black\" >\n", xstart,
                       *ystart + 0.7);
    kaps_print_to_file(device, f, "(1)");
    kaps_print_to_file(device, f, "</text>\n");

    *ystart = *ystart + 1.25;

    kaps_print_to_file(device, f, "</g>\n");
}

/*
 * Device print utility function
 * to pretty print KAPS uda (BBs) allocation
 * across the Databases to an HTML output file
 */

static void
kaps_print_bbs(
    struct kaps_device *device,
    struct memory_map *mem_map,
    FILE * f)
{
    int32_t i, j;
    float xstart, ystart;
    uint32_t db_group_id = 0;

    kaps_print_to_file(device, f, "<text x=\"12.0cm\" y=\"4.8cm\" "
                       "font-family=\"Verdana\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" >\n");
    kaps_print_to_file(device, f, "Bucket Blocks\n");
    kaps_print_to_file(device, f, "</text>\n");

    kaps_print_to_file(device, f, "<g id=\"UDA\">\n");
    kaps_print_to_file(device, f, "    <g id=\"LPU\"/>\n");
    kaps_print_to_file(device, f, "        <rect x=\"5.0cm\" y=\"5.0cm\" width=\"15.7cm\" "
                       "height=\"0.3cm\" fill=\"black\"/>\n");
    kaps_print_to_file(device, f, "    </g>\n");
    xstart = 5.0;
    ystart = 5.5;
    for (i = 0; i < mem_map->num_bb; i++)
    {
        for (j = 0; j < device->hw_res->num_bb_in_one_chain; j++)
        {
            struct kaps_db *db = device->map->bb_map[db_group_id][j + (i * device->hw_res->num_bb_in_one_chain)].row->owner;

            kaps_print_to_file(device, f, "    <g id=\"BB%d\">\n", i);

            kaps_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"0.7cm\" "
                               "height=\"%.02fcm\" fill=\"%s\" stroke=\"black\" stroke-width=\".05cm\"/>\n",
                               xstart, ystart + (j * 1.0), 0.7, db ? db->hw_res.db_res->color : "none");
            kaps_print_to_file(device, f, "    </g>\n");
        }
        xstart += 1.0;
    }
    kaps_print_to_file(device, f, "</g>\n");
}

void
kaps_print_database_key_structure(
    struct kaps_device *device,
    FILE * f)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    kaps_print_to_file(device, f, "<br>\n");
    kaps_print_to_file(device, f, "<h3>Database Key Structure</h3>\n");

    kaps_print_to_file(device, f, "<table>\n");
    kaps_print_to_file(device, f, "<tbody>\n");
    kaps_print_to_file(device, f, "<tr class =\"broadcom2\">\n");

    kaps_print_to_file(device, f, "<th>Color</th>\n");

    kaps_print_to_file(device, f, "<th>Name</th>\n");
    kaps_print_to_file(device, f, "<th>Key Width</th>\n");
    kaps_print_to_file(device, f, "<th>Achieved Capacity</th>\n");

    kaps_print_to_file(device, f, "<th>Key Structure</th>\n");
    kaps_print_to_file(device, f, "</tr>\n");

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP || db->type == KAPS_DB_HB)
            continue;

        for (; db; db = db->next_tab)
        {
            kaps_print_to_file(device, f, "<tr>\n");

            kaps_print_to_file(device, f, "  <td bgcolor=%s></td>\n", db->hw_res.db_res->color);

            kaps_fprintf(f, "<td>");
            kaps_print_db_name(db, f);
            kaps_print_to_file(device, f, "</td>");
            kaps_print_to_file(device, f, "  <td>%d</td>\n", db->width.key_width_1);

            if (db->is_clone)
                kaps_print_to_file(device, f, "  <td> - </td>\n");
            else
                kaps_print_to_file(device, f, "  <td>%d</td>\n", kaps_c_list_count(&db->db_list));

            kaps_print_to_file(device, f, "<td>");
            if (db->key)
                kaps_print_db_key(db->key, f);
            kaps_print_to_file(device, f, "</td>");
            kaps_print_to_file(device, f, "</tr>\n");
        }
    }
    kaps_print_to_file(device, f, "</tbody>\n");
    kaps_print_to_file(device, f, "</table>\n");
    kaps_print_to_file(device, f, "<br>\n");
}

void
kaps_print_kaps_resource_usage(
    struct kaps_device *device,
    FILE * f)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_db *db_a = NULL, *db_b = NULL;
    struct large_kaps2_stats stats_a, stats_b;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_DB_LPM)
            continue;

        if (db->rpb_id == 0)
        {
            db_a = db;
        }
        else
        {
            db_b = db;
        }
    }

    kaps_ftm_large_kaps2_calc_stats(db_a, &stats_a);

    if (db_b)
        kaps_ftm_large_kaps2_calc_stats(db_b, &stats_b);

    /*
     * Legend of the databases
     */
    kaps_print_to_file(device, f, "<h3>Resource Utilization</h3>\n");
    kaps_print_to_file(device, f, "<table>\n");
    kaps_print_to_file(device, f, "<tbody>\n");
    kaps_print_to_file(device, f, "<tr class=\"broadcom2\"> \n");
    kaps_print_to_file(device, f, "  <th>Resource</th>\n");
    kaps_print_to_file(device, f, "  <th>Type</th>\n");
    kaps_print_to_file(device, f, "  <th>Total</th>\n");
    kaps_print_to_file(device, f, "  <th>Used</th>\n");
    kaps_print_to_file(device, f, "  <th>%%Free</th>\n");
    kaps_print_to_file(device, f, "</tr>\n");

    kaps_print_to_file(device, f, "<tr>\n");
    kaps_print_to_file(device, f, "<td bgcolor=\"grey\" colspan=\"5\">\n");
    kaps_print_to_file(device, f, "</tr>\n");

    /*
     * RPB Usage
     */
    kaps_print_to_file(device, f, "<tr>\n");
    kaps_print_to_file(device, f, "  <td rowspan=\"2\">RPB</td>\n");
    kaps_print_to_file(device, f, "  <td bgcolor=%s><font color=\"white\">A</font></td>\n", db_a->hw_res.db_res->color);
    kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_a.total_rpb_rows);
    kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_a.num_rpb_rows_used);
    kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                       (((stats_a.total_rpb_rows - stats_a.num_rpb_rows_used) * 1.0) / stats_a.total_rpb_rows) * 100.0);
    kaps_print_to_file(device, f, "</tr>\n");

    if (db_b)
    {
        kaps_print_to_file(device, f, "<tr>\n");
        kaps_print_to_file(device, f, "  <td bgcolor=%s><font color=\"white\">B</font></td>\n",
                           db_b->hw_res.db_res->color);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_b.total_rpb_rows);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_b.num_rpb_rows_used);
        kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                           (((stats_b.total_rpb_rows -
                              stats_b.num_rpb_rows_used) * 1.0) / stats_b.total_rpb_rows) * 100.0);
        kaps_print_to_file(device, f, "</tr>\n");
    }

    kaps_print_to_file(device, f, "<tr>\n");
    kaps_print_to_file(device, f, "<td bgcolor=\"grey\" colspan=\"5\">\n");
    kaps_print_to_file(device, f, "</tr>\n");

    /*
     * Small BBs Usage
     */
    kaps_print_to_file(device, f, "<tr>\n");
    kaps_print_to_file(device, f, "  <td rowspan=\"2\">Small BBs </br>(480b rows)</td>\n");
    kaps_print_to_file(device, f, "  <td bgcolor=%s><font color=\"white\">A</font></td>\n", db_a->hw_res.db_res->color);
    kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_a.total_small_bb);
    kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_a.num_small_bb_used);
    kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                       (1.0 * (stats_a.total_small_bb - stats_a.num_small_bb_used) / stats_a.total_small_bb) * 100.0);

    kaps_print_to_file(device, f, "</tr>\n");

    if (db_b)
    {
        kaps_print_to_file(device, f, "<tr>\n");
        kaps_print_to_file(device, f, "  <td bgcolor=%s><font color=\"white\">B</font></td>\n",
                           db_b->hw_res.db_res->color);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_b.total_small_bb);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_b.num_small_bb_used);
        kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                           (1.0 * (stats_b.total_small_bb - stats_b.num_small_bb_used) / stats_b.total_small_bb) *
                           100.0);
        kaps_print_to_file(device, f, "</tr>\n");
    }

    kaps_print_to_file(device, f, "<tr>\n");
    kaps_print_to_file(device, f, "<td bgcolor=\"grey\" colspan=\"5\">\n");
    kaps_print_to_file(device, f, "</tr>\n");

    /*
     * ADS-2 Usage
     */
    kaps_print_to_file(device, f, "<tr>\n");
    kaps_print_to_file(device, f, "  <td rowspan=\"2\">AD-2</td>\n");
    kaps_print_to_file(device, f, "  <td bgcolor=%s><font color=\"white\">A</font></td>\n", db_a->hw_res.db_res->color);

    kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_a.total_ad2_rows);
    kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_a.num_ad2_rows_used);
    kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                       (((stats_a.total_ad2_rows -
                          stats_a.num_ad2_rows_used) * 1.0) / stats_a.total_ad2_rows) * 100.00);

    kaps_print_to_file(device, f, "</tr>\n");

    if (db_b)
    {
        kaps_print_to_file(device, f, "<tr>\n");
        kaps_print_to_file(device, f, "  <td bgcolor=%s><font color=\"white\">B</font></td>\n",
                           db_b->hw_res.db_res->color);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_b.total_ad2_rows);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_b.num_ad2_rows_used);
        kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                           (((stats_b.total_ad2_rows -
                              stats_b.num_ad2_rows_used) * 1.0) / stats_b.total_ad2_rows) * 100.0);
        kaps_print_to_file(device, f, "</tr>\n");
    }

    kaps_print_to_file(device, f, "<tr>\n");
    kaps_print_to_file(device, f, "<td bgcolor=\"grey\" colspan=\"5\">\n");
    kaps_print_to_file(device, f, "</tr>\n");

    /*
     * Large BBs Usage
     */
    kaps_print_to_file(device, f, "<tr>\n");
    kaps_print_to_file(device, f, "  <td rowspan=\"2\">Large BBs</br>(480b rows)</td>\n");
    kaps_print_to_file(device, f, "  <td bgcolor=%s><font color=\"white\">A</font></td>\n", db_a->hw_res.db_res->color);
    kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_a.total_large_bb_rows);
    kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_a.num_large_bb_rows_used);
    kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                       (((stats_a.total_large_bb_rows -
                          stats_a.num_large_bb_rows_used) * 1.0) / stats_a.total_large_bb_rows) * 100.0);
    kaps_print_to_file(device, f, "</tr>\n");

    if (db_b)
    {
        kaps_print_to_file(device, f, "<tr>\n");
        kaps_print_to_file(device, f, "  <td bgcolor=%s><font color=\"white\">B</font></td>\n",
                           db_b->hw_res.db_res->color);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_b.total_large_bb_rows);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", stats_b.num_large_bb_rows_used);
        kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                           (((stats_b.total_large_bb_rows -
                              stats_b.num_large_bb_rows_used) * 1.0) / stats_b.total_large_bb_rows) * 100.0);
        kaps_print_to_file(device, f, "</tr>\n");
    }

    kaps_print_to_file(device, f, "</tbody>\n");
    kaps_print_to_file(device, f, "</table>\n");

    kaps_print_to_file(device, f, "<br>\n");
    kaps_print_to_file(device, f, "<br>\n");

    kaps_kaps_print_detailed_stats_html(f, device, db_a);
    kaps_print_to_file(device, f, "<br>\n");
    kaps_print_to_file(device, f, "<br>\n");

    if (db_b)
        kaps_kaps_print_detailed_stats_html(f, device, db_b);
}

/*
 * Device print utility function
 * to pretty print KAPS resource allocation
 * across the Databases and other relevant
 * information to an HTML output file
 */
void
kaps_resource_kaps_print_html(
    struct kaps_device *device,
    FILE * f)
{
    struct memory_map *mem_map = device->map;
    int32_t i, j, num_smt;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    if (device->map_print)
    {
        device->map_print_offset = 0;
        device->hw_res->device_print_advance = 2;
    }

    /*
     * HTML headers
     */
    kaps_print_to_file(device, f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n");
    kaps_print_to_file(device, f, "\"http://www.w3.org/TR/html4/strict.dtd\">\n");
    kaps_print_to_file(device, f, "<HTML lang=\"en\">\n");
    kaps_print_to_file(device, f, "<HEAD>\n");
    kaps_print_to_file(device, f, "<STYLE type=\"text/css\">\n");
    kaps_print_to_file(device, f, "td,th {padding-bottom:4pt;padding-top:4pt;padding-left:4pt;padding-right:4pt}\n");
    kaps_print_to_file(device, f, "table {border-collapse:collapse}\n");
    kaps_print_to_file(device, f, "td {text-align:center;font-family:Courier New;font-weight:bold;font-size:100%%}\n");
    kaps_print_to_file(device, f, "table,td,th {border:2px solid #adafb2}\n");
    kaps_print_to_file(device, f, "tr.broadcom1 {background:#e31837;color:#ffffff}\n");
    kaps_print_to_file(device, f, "tr.broadcom2 {background:#005568;color:#ffffff}\n");
    kaps_print_to_file(device, f, "tr.broadcom3 {background:#ffd457;color:#000000}\n");
    kaps_print_to_file(device, f, "body {margin:20px 20px 20px 20px}\n");
    kaps_print_to_file(device, f, "</STYLE>\n");
    kaps_print_to_file(device, f, "</HEAD>\n");

    kaps_print_to_file(device, f, "<BODY>\n");
    if (device->description)
    {
        kaps_print_to_file(device, f, "<h2>%s</h2>\n", device->description);
    }
    else
    {
        kaps_print_to_file(device, f, "<h2>KBP Scenario Output</h2>\n");
    }

    {
        /*
         * Legend of the databases
         */
        kaps_print_to_file(device, f, "<h3>Databases</h3>\n");
        kaps_print_to_file(device, f, "<table>\n");
        kaps_print_to_file(device, f, "<tbody>\n");
        kaps_print_to_file(device, f, "<tr class=\"broadcom2\"> \n");
        kaps_print_to_file(device, f, "  <th>Color</th>\n");
        kaps_print_to_file(device, f, "  <th>Name</th>\n");
        kaps_print_to_file(device, f, "  <th>Description</th>\n");
        kaps_print_to_file(device, f, "  <th>Key width</th>\n");
        kaps_print_to_file(device, f, "  <th>Capacity</th>\n");

        if (device->hw_res->num_algo_levels != 3)
        {
            kaps_print_to_file(device, f, "  <th>AD<br>entries*width,Mb</th>\n");
            kaps_print_to_file(device, f, "  <th>Algorithmic</th>\n");
            kaps_print_to_file(device, f, "  <th>BB</th>\n");
            kaps_print_to_file(device, f, "  <th>DT(SRAM,DBA)</th>\n");
            kaps_print_to_file(device, f, "  <th>AB</th>\n");
        }

        kaps_print_to_file(device, f, "  <th>LSN Size</th>\n");
        kaps_print_to_file(device, f, "  <th>LSN Mb</th>\n");
        kaps_print_to_file(device, f, "  <th>Failed to fit</th>\n");
        kaps_print_to_file(device, f, "</tr>\n");

        if (device->smt)
            num_smt = 2;
        else
            num_smt = 1;
        for (i = 0; i < num_smt; i++)
        {
            struct kaps_device *dev;

            if (i == 0)
            {
                dev = device;
            }
            else
            {
                dev = device->smt;
            }

            kaps_c_list_iter_init(&dev->db_list, &it);
            while ((el = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

                if (db->type == KAPS_DB_AD || db->type == KAPS_DB_HB)
                    continue;

                kaps_print_to_file(device, f, "<tr>\n");
                kaps_print_to_file(device, f, "  <td bgcolor=%s></td>\n", db->hw_res.db_res->color);
                kaps_print_to_file(device, f, "  <td>%s%d", kaps_device_db_name(db), db->tid);

                if (device->smt)
                {
                    if (i == 0)
                        kaps_print_to_file(device, f, " (Thread 0)");
                    else
                        kaps_print_to_file(device, f, " (Thread 1)");
                }
                kaps_print_to_file(device, f, "</td>\n");
                if (db->num_algo_levels_in_db == 3)
                    kaps_print_to_file(device, f, "<td>%s</td>\n",
                                       db->description ? db->description : (db->rpb_id ? "RPB 1" : "RPB 0"));
                else
                    kaps_print_to_file(device, f, "<td>%s</td>\n", db->description ? db->description : "-");
                kaps_print_to_file(device, f, "  <td>%d</td>\n",
                                   (db->type == KAPS_DB_DMA ? KAPS_DMA_WIDTH_1 : db->key->width_1));
                kaps_print_to_file(device, f, "  <td>");
                kaps_print_capacity(device, db->common_info->capacity, f);
                kaps_print_to_file(device, f, "</td>\n");

                if (db->num_algo_levels_in_db != 3)
                {

                    if (db->common_info->ad_info.ad)
                    {
                        kaps_print_to_file(device, f, "  <td>");
                        kaps_print_capacity(device, db->common_info->ad_info.ad->common_info->capacity, f);
                        kaps_print_to_file(device, f, "*%db,%dMb</td>\n",
                                           db->common_info->ad_info.ad->width.ad_width_1,
                                           db->common_info->ad_info.ad->common_info->uda_mb);
                    }
                    else
                    {
                        kaps_print_to_file(device, f, "  <td>-</td>\n");
                    }
                    if (kaps_db_get_algorithmic(db))
                        kaps_print_to_file(device, f, "  <td>yes</td>\n");
                    else
                        kaps_print_to_file(device, f, "  <td>no</td>\n");
                    kaps_print_to_file(device, f, "  <td>%d,%d</td>\n", db->hw_res.db_res->num_dt,
                                       db->hw_res.db_res->num_dba_dt);
                    kaps_print_to_file(device, f, "  <td>%d</td>\n", db->common_info->num_ab);
                }

                kaps_print_to_file(device, f, "  <td>%d</td>\n", db->hw_res.db_res->lsn_info[0].max_lsn_size);
                kaps_print_to_file(device, f, "  <td>%.02f</td>\n",
                                   ((db->common_info->total_bb_rows * KAPS_BKT_WIDTH_1) / (1024.0 * 1024.0)));

                if (db->common_info->fit_error == 0)
                {
                    kaps_print_to_file(device, f, "  <td> - </td>\n");
                }
                else
                {
                    kaps_print_to_file(device, f, "  <td> ");
                    if (db->common_info->fit_error & FAILED_FIT_DBA)
                        kaps_print_to_file(device, f, "DBA ");
                    if (db->common_info->fit_error & FAILED_FIT_LSN)
                        kaps_print_to_file(device, f, "LSN ");
                    if (db->common_info->fit_error & FAILED_FIT_AD)
                        kaps_print_to_file(device, f, "AD ");
                    if (db->common_info->fit_error & FAILED_FIT_IT)
                        kaps_print_to_file(device, f, "IT ");
                    kaps_print_to_file(device, f, "</td>\n");
                }
                kaps_print_to_file(device, f, "</tr>\n");
            }
        }

        kaps_print_to_file(device, f, "</tbody>\n");
        kaps_print_to_file(device, f, "</table>\n");
    }

    kaps_print_database_key_structure(device, f);

    kaps_print_to_file(device, f, "<h3>Instructions</h3>\n");
    kaps_print_to_file(device, f, "<table>\n");
    kaps_print_to_file(device, f, "<tbody>\n");
    kaps_print_to_file(device, f, "<tr class =\"broadcom2\">\n");
    kaps_print_to_file(device, f, "<th>ID</th>\n");
    kaps_print_to_file(device, f, "<th>Master key width</th>\n");

    for (i = 0; i < device->max_num_searches; i++)
    {
        kaps_print_to_file(device, f, "<th>%d</th>\n", i);
    }

    kaps_print_to_file(device, f, "</tr>\n");

    for (j = 0; j < num_smt; j++)
    {
        struct kaps_device *dev;

        if (j == 0)
        {
            dev = device;
        }
        else
        {
            dev = device->smt;
        }

        kaps_c_list_iter_init(&dev->inst_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_instruction *instruction = KAPS_INSTLIST_TO_ENTRY(el);
            struct kaps_db *db[KAPS_HW_MAX_SEARCH_DB] = { 0 };

            if (instruction->is_installed == 0)
                continue;

            kaps_print_to_file(device, f, "<tr>\n");
            kaps_print_to_file(device, f, "<td>%d</td>\n", instruction->id);
            kaps_print_to_file(device, f, "<td>%d</td>\n", instruction->master_key->width_1);

            for (i = 0; i < device->max_num_searches; i++)
            {
                uint32_t s_interface;

                if (!instruction->desc[i].db)
                    continue;
                s_interface = instruction->desc[i].result_id;
                db[s_interface] = instruction->desc[i].db;
            }

            for (i = 0; i < device->max_num_searches; i++)
            {
                if (db[i])
                {
                    kaps_print_to_file(device, f, "<td>");
                    kaps_print_db_name(db[i], f);
                    kaps_print_to_file(device, f, "</td>\n");
                }
                else
                {
                    kaps_print_to_file(device, f, "<td>-</td>\n");
                }
            }

            kaps_print_to_file(device, f, "</tr>\n");
        }
    }

    kaps_print_to_file(device, f, "</tbody>\n");
    kaps_print_to_file(device, f, "</table>\n");
    kaps_print_to_file(device, f, "<br>\n");

    if (device->hw_res->num_algo_levels != 3)
    {
        /*
         * Print device related assumptions
         */
        kaps_print_to_file(device, f, "<h3>Device Configuration</h3>\n");
        kaps_print_to_file(device, f, "<table>\n");
        kaps_print_to_file(device, f, "<tbody>\n");
        kaps_print_to_file(device, f, "<tr class=\"broadcom3\"> \n");
        kaps_print_to_file(device, f, "  <th>Type</th>\n");
        kaps_print_to_file(device, f, "  <th>AB</th>\n");
        kaps_print_to_file(device, f, "  <th>BBs</th>\n");
        kaps_print_to_file(device, f, "  <th>BB Cascades</th>\n");
        kaps_print_to_file(device, f, "</tr>\n");

        kaps_print_to_file(device, f, "<tr>\n");
        kaps_print_to_file(device, f, "  <td>KAPS</td>\n");
        kaps_print_to_file(device, f, "  <td>%d</td>\n", device->num_ab[0]);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", device->hw_res->total_lpus);
        kaps_print_to_file(device, f, "  <td>%d</td>\n", device->hw_res->num_bb_in_one_chain);
        kaps_print_to_file(device, f, "</tr>\n");
        kaps_print_to_file(device, f, "</tbody>\n");
        kaps_print_to_file(device, f, "</table>\n");
        kaps_print_to_file(device, f, "<br>\n");
    }

    if (device->hw_res->device_print_advance == 2)
    {

        if (device->hw_res->num_algo_levels == 3)
        {
            kaps_print_kaps_resource_usage(device, f);
        }

        kaps_print_to_file(device, f, "<h3>Device Mapping</h3>\n");

        /*
         * Orange outline for resources
         */
        kaps_print_to_file(device, f,
                           "<svg preserveAspectRatio=\"xMinYMin meet\" width=\"26cm\" height=\"20cm\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n");
        kaps_print_to_file(device, f,
                           "<g id=\"Bounding box\"><rect x=\"0.2cm\" y=\".2cm\" width=\"25.6cm\" height=\"19.6cm\" "
                           "fill=\"none\" stroke=\"orange\" stroke-width=\".2cm\"/></g>\n");

        if (device->hw_res->num_algo_levels == 2)
        {
            /*
             * RPBs
             */
            kaps_print_dba(device, f, mem_map);

            /*
             * Bucket Blocks
             */
            kaps_print_bbs(device, mem_map, f);
        }

        if (device->hw_res->num_algo_levels == 3)
        {
            float ystart = 2.0;

            /*
             * RPBs
             */
            kaps_print_rpbs(device, f, mem_map, &ystart);

            kaps_print_ads1(device, f, mem_map, &ystart);

            /*
             * Bucket Blocks
             */
            kaps_print_small_bbs(device, f, mem_map, &ystart);

            kaps_print_ads2(device, f, mem_map, &ystart);

            kaps_print_large_bbs(device, f, mem_map, &ystart);
        }

        kaps_print_to_file(device, f, "</g>\n");

        kaps_print_to_file(device, f, "</svg>\n");
    }

    kaps_print_to_file(device, f, "</BODY>\n");
    kaps_print_to_file(device, f, "</HTML>\n");
}
