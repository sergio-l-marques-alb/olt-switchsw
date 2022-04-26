/*
 **************************************************************************************
 Copyright 2009-2019 Broadcom Corporation

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

#include "kaps_utility.h"
#include "kaps_fib_lsnmc.h"
#include "kaps_fib_lsnmc_hw.h"
#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_fib_cmn_seqgen.h"
#include "kaps_device.h"
#include "kaps_device_internal.h"
#include "kaps_fib_poolmgr.h"
#include "kaps_uda_mgr.h"
#include "kaps_xpt.h"
#include "kaps_device_alg.h"
#include "kaps_fib_hw.h"
#include "kaps_ad_internal.h"
#include "kaps_fib_hw.h"
#include "kaps_handle.h"





static struct kaps_ab_info *
kaps_kaps_get_rpb_for_lsn(
    kaps_lsn_mc * self)
{
    kaps_ipm *ipm;
    kaps_trie_node *trieNode;
    void *tmp_ptr;
    NlmReasonCode reason;
    kaps_lpm_trie *trie = self->m_pTrie;

    tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);
    kaps_memcpy(&trieNode, tmp_ptr, sizeof(kaps_trie_node *));

    if (!trieNode->m_rptParent_p)
        kaps_trie_compute_rpt_parent(trieNode, &reason);

    ipm = kaps_pool_mgr_get_ipm_for_pool(trie->m_trie_global->poolMgr, trieNode->m_poolId);
    return ipm->m_ab_info;
}

uint8_t *
kaps_kaps_get_bkt_data(
    kaps_lsn_mc * self,
    struct kaps_device * device,
    uint32_t bkt_nr,
    uint32_t row_nr)
{
    uint8_t *bkt_data = NULL;
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    struct kaps_db *db = self->m_pTbl->m_db;

    if (kaps_db_is_reduced_algo_levels(db))
    {
        bkt_data = shadow->small_bbs[bkt_nr].bkt_rows[row_nr].data;
    }
    else
    {
        bkt_data = shadow->bkt_blks[bkt_nr].bkt_rows[row_nr].data;
    }

    return bkt_data;
}

static uint32_t
kaps_check_if_hb_is_present_in_lsn(
    kaps_lsn_mc * self)
{
    kaps_lpm_lpu_brick *curLpuBrick = self->m_lpuList;
    uint32_t i;
    kaps_pfx_bundle *pfxBundle;

    while (curLpuBrick)
    {
        for (i = 0; i < curLpuBrick->m_maxCapacity; ++i)
        {
            pfxBundle = curLpuBrick->m_pfxes[i];
            if (pfxBundle)
            {
                if (pfxBundle->m_backPtr->hb_user_handle)
                    return 1;
            }
        }

        curLpuBrick = curLpuBrick->m_next_p;
    }

    return 0;
}

static uint32_t
kaps_jr2_large_convert_lpu_nr_to_format_ix(
    struct kaps_device *device,
    uint32_t cur_lpu_nr,
    uint32_t rpb_id)
{
    uint32_t format_ix = 0;

    /*
     * Make cur_lpu_nr even
     */
    if (cur_lpu_nr % 2)
        cur_lpu_nr--;

    if (device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_J2P_THREE_LEVEL)
    {
        if (rpb_id == 0)
        {
            switch (cur_lpu_nr)
            {
                case 2:
                    format_ix = 0;
                    break;
                case 4:
                    format_ix = 1;
                    break;

                case 6:
                    format_ix = 2;
                    break;

                case 8:
                    format_ix = 3;
                    break;

                case 10:
                    format_ix = 4;
                    break;

                case 12:
                    format_ix = 5;
                    break;

                case 14:
                    format_ix = 6;
                    break;

                case 16:
                    format_ix = 7;
                    break;

                case 18:
                    format_ix = 8;
                    break;

                case 20:
                    format_ix = 9;
                    break;

                case 24:
                    format_ix = 10;
                    break;

                case 26:
                    format_ix = 11;
                    break;

                default:
                    kaps_assert(0, "Incorrect LPU Nr to format ix conversion\n");
                    break;

            }
        }
        else
        {
            switch (cur_lpu_nr)
            {
                case 0:
                    format_ix = 0;
                    break;

                case 8:
                    format_ix = 1;
                    break;

                case 10:
                    format_ix = 2;
                    break;

                case 18:
                    format_ix = 3;
                    break;

                case 20:
                    format_ix = 4;
                    break;

                case 22:
                    format_ix = 5;
                    break;

                default:
                    kaps_assert(0, "Incorrect LPU Nr to format ix conversion\n");
                    break;
            }
        }
    }
    else
    {

        if (rpb_id == 0)
        {
            switch (cur_lpu_nr)
            {
                case 0:
                    format_ix = 0;
                    break;

                case 2:
                    format_ix = 1;
                    break;

                case 4:
                    format_ix = 2;
                    break;

                case 6:
                    format_ix = 3;
                    break;

                case 8:
                    format_ix = 4;
                    break;

                case 10:
                    format_ix = 5;
                    break;

                case 12:
                    format_ix = 6;
                    break;

                case 14:
                    format_ix = 7;
                    break;

                case 16:
                    format_ix = 8;
                    break;

                case 24:
                    format_ix = 9;
                    break;

                case 26:
                    format_ix = 10;
                    break;

                default:
                    kaps_assert(0, "Incorrect LPU Nr to format ix conversion\n");
                    break;
            }

        }
        else
        {
            switch (cur_lpu_nr)
            {
                case 16:
                    format_ix = 0;
                    break;

                case 18:
                    format_ix = 1;
                    break;

                case 20:
                    format_ix = 2;
                    break;

                case 22:
                    format_ix = 3;
                    break;

                case 24:
                    format_ix = 4;
                    break;

                case 26:
                    format_ix = 5;
                    break;

                default:
                    kaps_assert(0, "Incorrect LPU Nr to format ix conversion\n");
                    break;

            }
        }
    }

    return format_ix;
}


uint32_t
kaps_get_daisy_chain_id(
    struct kaps_device *device,
    struct kaps_lsn_mc *self,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum)
{

    return 0;   

}




static NlmErrNum_t
kaps_kaps_fill_format_map(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick *cur_brick,
    struct kaps_ads *kaps_it,
    uint32_t cur_lpu_nr,
    uint32_t brick_gran_ix)
{
    uint32_t format_value;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t format_ix;
    struct kaps_db *db = self->m_pTbl->m_db;
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    uint32_t db_group_id = db->db_group_id;

    (void) cur_brick;

    format_value = settings->m_granIxToFormatValue[brick_gran_ix];

    format_ix = cur_lpu_nr;
    
    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID &&
        (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM4
         || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4))
    {
        format_ix = cur_lpu_nr / 4;
    }
    else if (db->num_algo_levels_in_db == 3)
    {
        /*
         * Make cur_lpu_nr even
         */
        if (cur_lpu_nr % 2)
            cur_lpu_nr--;

        format_ix = kaps_jr2_large_convert_lpu_nr_to_format_ix(device, cur_lpu_nr, db->rpb_id);

        if (db->rpb_id == 0)
        {
            if (device->map->bb_map[db_group_id][cur_lpu_nr].bb_config != KAPS_LARGE_BB_WIDTH_A480_B000
                && device->map->bb_map[db_group_id][cur_lpu_nr].bb_config != KAPS_LARGE_BB_WIDTH_A960_B000
                && device->map->bb_map[db_group_id][cur_lpu_nr].bb_config != KAPS_LARGE_BB_WIDTH_A480_B480)
                kaps_assert(0, "Mismatching configuration while computing the format ix \n");

        }
        else
        {

            if (device->map->bb_map[db_group_id][cur_lpu_nr].bb_config != KAPS_LARGE_BB_WIDTH_A000_B480
                && device->map->bb_map[db_group_id][cur_lpu_nr].bb_config != KAPS_LARGE_BB_WIDTH_A000_B960
                && device->map->bb_map[db_group_id][cur_lpu_nr].bb_config != KAPS_LARGE_BB_WIDTH_A480_B480)
                kaps_assert(0, "Mismatching configuration while computing the format ix \n");
        }
    }


    switch (format_ix)
    {
        case 0:
            kaps_it->format_map_00 = format_value;
            break;

        case 1:
            kaps_it->format_map_01 = format_value;
            break;

        case 2:
            kaps_it->format_map_02 = format_value;
            break;

        case 3:
            kaps_it->format_map_03 = format_value;
            break;

        case 4:
            kaps_it->format_map_04 = format_value;
            break;

        case 5:
            kaps_it->format_map_05 = format_value;
            break;

        case 6:
            kaps_it->format_map_06 = format_value;
            break;

        case 7:
            kaps_it->format_map_07 = format_value;
            break;

        case 8:
            kaps_it->format_map_08 = format_value;
            break;

        case 9:
            kaps_it->format_map_09 = format_value;
            break;

        case 10:
            kaps_it->format_map_10 = format_value;
            break;

        case 11:
            kaps_it->format_map_11 = format_value;
            break;

        case 12:
            kaps_it->format_map_12 = format_value;
            break;

        case 13:
            kaps_it->format_map_13 = format_value;
            break;

        case 14:
            kaps_it->format_map_14 = format_value;
            break;

        case 15:
            kaps_it->format_map_15 = format_value;
            break;

        default:
            errNum = NLMERR_FAIL;
            kaps_assert(0, "Incorrect format value specified");
            break;
    }

    return errNum;
}



NlmErrNum_t
kaps_kaps_format_map_get_gran(
    kaps_lsn_mc_settings * settings,
    struct kaps_ads * kaps_it,
    uint32_t cur_lpu_nr,
    uint32_t * brick_gran_ix)
{
    uint32_t format_value = 0;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_device *device = settings->m_device;
    uint32_t format_ix = cur_lpu_nr;

    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID &&
        (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM4
         || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4))
    {
        format_ix = cur_lpu_nr / 4;
    }

    switch (format_ix)
    {
        case 0:
            format_value = kaps_it->format_map_00;
            break;

        case 1:
            format_value = kaps_it->format_map_01;
            break;

        case 2:
            format_value = kaps_it->format_map_02;
            break;

        case 3:
            format_value = kaps_it->format_map_03;
            break;

        case 4:
            format_value = kaps_it->format_map_04;
            break;

        case 5:
            format_value = kaps_it->format_map_05;
            break;

        case 6:
            format_value = kaps_it->format_map_06;
            break;

        case 7:
            format_value = kaps_it->format_map_07;
            break;

        case 8:
            format_value = kaps_it->format_map_08;
            break;

        case 9:
            format_value = kaps_it->format_map_09;
            break;

        case 10:
            format_value = kaps_it->format_map_10;
            break;

        case 11:
            format_value = kaps_it->format_map_11;
            break;

        case 12:
            format_value = kaps_it->format_map_12;
            break;

        case 13:
            format_value = kaps_it->format_map_13;
            break;

        case 14:
            format_value = kaps_it->format_map_14;
            break;

        case 15:
            format_value = kaps_it->format_map_15;
            break;

        default:
            errNum = NLMERR_FAIL;
            kaps_assert(0, "Incorrect format value specified");
            break;
    }

    *brick_gran_ix = format_value - 1;
    return errNum;
}



static NlmErrNum_t
kaps_kaps_iit_commit(
    kaps_lsn_mc * self,
    uint32_t index,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    uint32_t i, it_addr = 0, cur_lpu_nr, brick_no;
    struct uda_mem_chunk *mem_info = self->m_mlpMemInfo;
    NlmErrNum_t errNum = NLMERR_OK;
    void *tmp_ptr = NULL;
    kaps_trie_node *trienode = NULL;
    kaps_pool_mgr *poolMgr = self->m_pTrie->m_trie_global->poolMgr;
    kaps_lpm_lpu_brick *curLpuBrick;
    kaps_ipm *ipm_p = NULL;
    struct kaps_ads kaps_it;
    uint8_t *ad_value;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t cur_lpu, cur_row;
    struct kaps_db *db = self->m_pTbl->m_db;
    struct kaps_ad_db *ad_db = NULL;
    uint32_t ads2_num, ads2_depth;

    if (device->issu_in_progress)
        return errNum;

    ads2_num = db->rpb_id;

    kaps_memset(&kaps_it, 0, sizeof(kaps_it));

    if (index != KAPS_LSN_NEW_INDEX)
    {
        tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);
        kaps_memcpy(&trienode, tmp_ptr, sizeof(kaps_trie_node *));

        ipm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, trienode->m_poolId);

        it_addr = index + ipm_p->m_ab_info->base_addr;

        if (db->num_algo_levels_in_db == 3)
        {
            int32_t actual_it_addr;

            
            ads2_depth = device->hw_res->ads2_depth[ads2_num];

            /*
             * it_addr contains the virtual it address. Calculate the actual_it_addr by looking up the x_table
             */
            actual_it_addr = device->kaps_shadow->ads2_overlay[ads2_num].x_table[it_addr];

            /*
             * If actual_it_addr is -1, then it means that the virtual_it_address doesn't yet have a valid
             * actual_it_address So look up the bitmap it_fbmp, find a real it address and map it to the
             * virtual_it_address
             */
            if (actual_it_addr == -1)
            {
                actual_it_addr = kaps_find_first_bit_set(&device->kaps_shadow->ads2_overlay[ads2_num].it_fbmp,
                                                         0, ads2_depth - 1);

                kaps_assert(0 <= actual_it_addr && actual_it_addr < ads2_depth, "Out of KAPS ADS-2 IT\n");

                device->kaps_shadow->ads2_overlay[ads2_num].x_table[it_addr] = actual_it_addr;

                /*
                 * Make actual_it_addr in the bitmap to 0 to indicate that the location is no longer free
                 */
                kaps_reset_bit(&device->kaps_shadow->ads2_overlay[ads2_num].it_fbmp, actual_it_addr);

                if (device->kaps_shadow->ads2_overlay[ads2_num].num_free_it_slots)
                {
                    device->kaps_shadow->ads2_overlay[ads2_num].num_free_it_slots--;
                }
                else
                {
                    kaps_assert(0, "Incorrect number of IT slots in ADS-2 overlay");
                }
            }

            /*
             * it_addr will now contain the actual_it_addr
             */
            it_addr = actual_it_addr;
        }

        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), mem_info, 0, &cur_lpu, &cur_row);
        kaps_it.bkt_row = cur_row;

        if (db->num_algo_levels_in_db == 2)
        {
            kaps_it.row_offset = cur_lpu;
        }
        else
        {
            {
                kaps_it.row_offset = kaps_jr2_large_convert_lpu_nr_to_format_ix(device, cur_lpu, db->rpb_id);
            }
        }

        if (db->num_algo_levels_in_db == 3)
        {
            uint32_t max_rows_in_udm;

            max_rows_in_udm = device->hw_res->ads2_depth[ads2_num] / 2;

            /*
             * For the large KAPS BBs we should not go to the row beyond the last row. This can happen if we specify a
             * non-zero row offset for the last row. So to overcome the hardware limitation, force the row offset to 0
             * for the last row
             */
            if (kaps_it.bkt_row == max_rows_in_udm - 1)
            {
                kaps_it.row_offset = 0;
            }
        }

        kaps_it.key_shift = self->m_nLopoff;

        if (trienode->m_iitLmpsofarPfx_p)
        {
            kaps_it.bpm_len = trienode->m_iitLmpsofarPfx_p->m_nPfxSize;

            /*Get the AD DB from the prefix bundle*/
            KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, 
                trienode->m_iitLmpsofarPfx_p->m_backPtr->ad_handle, ad_db);

            {
                struct kaps_lpm_entry *entry = trienode->m_iitLmpsofarPfx_p->m_backPtr;
            
                ad_value = entry->ad_handle->value;

                for (i = 0; i < ad_db->user_width_1 / KAPS_BITS_IN_BYTE; ++i)
                {
                    kaps_it.bpm_ad = (kaps_it.bpm_ad << KAPS_BITS_IN_BYTE) | ad_value[i];
                }

                if (ad_db->user_width_1 == 20)
                    kaps_it.bpm_ad = (kaps_it.bpm_ad << 4) | ad_value[i] >> 4;

            }

        }
        else
        {
            kaps_it.bpm_len = 0;
            kaps_it.bpm_ad = 0;

        }



        curLpuBrick = self->m_lpuList;
        brick_no = 0;

        while (curLpuBrick)
        {
            cur_lpu_nr = kaps_uda_mgr_compute_brick_udc(*(settings_p->m_pMlpMemMgr), mem_info, brick_no);
            kaps_kaps_fill_format_map(self, curLpuBrick, &kaps_it, cur_lpu_nr, curLpuBrick->m_granIx);
            brick_no++;
            curLpuBrick = curLpuBrick->m_next_p;
        }

        if (device->type == KAPS_DEVICE_KAPS && device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
        {
            if (kaps_uda_mgr_compute_brick_udc(*(settings_p->m_pMlpMemMgr), mem_info, 0) != 0
                && settings_p->m_isFullWidthLsn)
            {
                kaps_kaps_fill_format_map(self, curLpuBrick, &kaps_it, 0, self->m_lpuList->m_granIx);
            }
        }

        errNum = kaps_kaps_iit_write(self->m_pTbl->m_fibTblMgr_p, (uint8_t) self->m_devid, ipm_p->m_ab_info, &kaps_it,
                                it_addr, o_reason);
    }

    return errNum;
}

static NlmErrNum_t
kaps_kaps_bb_write_wrapper(
    kaps_lsn_mc * self,
    struct kaps_device *device,
    uint32_t bb_num,
    uint32_t row_num,
    uint8_t is_final_level,
    uint32_t length,
    uint8_t * data,
    NlmReasonCode * o_reason)
{
    kaps_status status;
    struct kaps_ab_info *cur_ab;
    uint32_t ab_num, start_small_bb_num;
    struct kaps_db *db = self->m_pTbl->m_db;

    if (!kaps_db_is_reduced_algo_levels(db))
    {
        status = kaps_dm_kaps_bb_write(device, db, bb_num, row_num, is_final_level, length, data);
        if (status != KAPS_OK)
        {
            *o_reason = NLMRSC_XPT_FAILED;
            return NLMERR_FAIL;
        }
        return NLMERR_OK;
    }

    cur_ab = kaps_kaps_get_rpb_for_lsn(self);

    while (cur_ab)
    {
        ab_num = cur_ab->ab_num;

        if (ab_num == 2 || ab_num == 3 || ab_num == 6 || ab_num == 7)
        {
            cur_ab = cur_ab->dup_ab;
            continue;
        }

        start_small_bb_num = 0;
        if (ab_num >= 4)
            start_small_bb_num = 16;

        status = kaps_dm_kaps_bb_write(device, db, start_small_bb_num + bb_num, row_num, is_final_level, length, data);
        if (status != KAPS_OK)
        {
            *o_reason = NLMRSC_XPT_FAILED;
            return NLMERR_FAIL;
        }

        cur_ab = cur_ab->dup_ab;
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_kaps_commit_hitbits(
    kaps_lsn_mc * lsn_p,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings = lsn_p->m_pSettings;
    struct kaps_device *device = lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    struct NlmFibStats *stats = &settings->m_fibTbl->m_fibStats;
    uint16_t hb_word;
    uint8_t data[2 * KAPS_HB_ROW_WIDTH_8] = { 0, };
    kaps_lpm_lpu_brick *curBrick;
    uint32_t i, cur_hb_blk, cur_row;
    kaps_status status;
    int32_t brick_idx;
    uint32_t actual_num_entries_per_hb_blk, pos_in_hb_blk;
    uint32_t flush_write;
    struct kaps_db *db = lsn_p->m_pTbl->m_db;
    uint32_t db_group_id = db->db_group_id;
    uint32_t final_level_offset = kaps_device_get_final_level_offset(device, db);

    curBrick = lsn_p->m_lpuList;
    hb_word = 0;
    brick_idx = 0;

    while (curBrick)
    {
        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings->m_pMlpMemMgr), lsn_p->m_mlpMemInfo, brick_idx,
                                                   &cur_hb_blk, &cur_row);
        cur_hb_blk = cur_hb_blk * device->hw_res->num_bb_in_one_chain;

        actual_num_entries_per_hb_blk = curBrick->m_maxCapacity / device->hw_res->num_bb_in_one_chain;
        pos_in_hb_blk = 0;
        flush_write = 1;

        if (db->num_algo_levels_in_db == 3)
        {
            /*
             * Set actual_num_entries_per_hb_blk to an impossible high value so that we don't flush in the for loop
             * below and we flush if needed after the for loop
             */
            actual_num_entries_per_hb_blk = (KAPS_HB_ROW_WIDTH_1 * 2) + 1;

            if (cur_hb_blk % 2 == 0)
            {
                uint32_t next_hb_blk = 0, next_blk_row = 0;
                uint32_t is_next_hb_blk_odd = 0;

                if (curBrick->m_next_p)
                {
                    kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings->m_pMlpMemMgr), lsn_p->m_mlpMemInfo,
                                                               brick_idx + 1, &next_hb_blk, &next_blk_row);

                    if (next_hb_blk == cur_hb_blk + 1)
                    {
                        is_next_hb_blk_odd = 1;
                    }
                }

                if (is_next_hb_blk_odd)
                {
                    flush_write = 0;
                }
            }

        }

        for (i = 0; i < curBrick->m_maxCapacity; ++i)
        {
            kaps_pfx_bundle *curPfxBundle = curBrick->m_pfxes[i];

            if (curPfxBundle && curPfxBundle->m_backPtr->hb_user_handle)
            {
                struct kaps_db *db;
                struct kaps_hb *hb = NULL;

                KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, curPfxBundle->m_backPtr, db)
                    KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb),
                                            (uintptr_t) curPfxBundle->m_backPtr->hb_user_handle);
                if (hb->value)
                    hb_word |= (1u << pos_in_hb_blk);
            }

            ++pos_in_hb_blk;

            if (pos_in_hb_blk >= actual_num_entries_per_hb_blk)
            {
                KapsWriteBitsInArray(data, KAPS_HB_ROW_WIDTH_8, KAPS_HB_ROW_WIDTH_1 - 1, 0, hb_word);

                status = kaps_dm_kaps_hb_write(device, db, final_level_offset + cur_hb_blk, cur_row, data);
                if (status != KAPS_OK)
                {
                    *o_reason = NLMRSC_XPT_FAILED;
                    return NLMERR_FAIL;
                }

                stats->numHitBitWrites++;

                hb_word = 0;
                pos_in_hb_blk = 0;

                ++cur_hb_blk;
            }
        }

        if (db->num_algo_levels_in_db == 3)
        {
            uint32_t main_hb_blk = cur_hb_blk;

            if (main_hb_blk % 2 == 1)
            {
                main_hb_blk--;
            }

            if (device->map->bb_map[db_group_id][main_hb_blk].bb_config == KAPS_LARGE_BB_WIDTH_A480_B480)
            {
                status = kaps_dm_kaps_hb_read(device, db, final_level_offset + (cur_hb_blk / 2), cur_row, data);
                if (status != KAPS_OK)
                {
                    *o_reason = NLMRSC_XPT_FAILED;
                    return NLMERR_FAIL;
                }
            }

            if (cur_hb_blk % 2 == 0)
            {
                KapsWriteBitsInArray(data, KAPS_HB_ROW_WIDTH_8, KAPS_HB_ROW_WIDTH_1 - 1, 0, hb_word);
            }
            else
            {
                KapsWriteBitsInArray(&data[KAPS_HB_ROW_WIDTH_8], KAPS_HB_ROW_WIDTH_8, KAPS_HB_ROW_WIDTH_1 - 1, 0, hb_word);
            }

            if (flush_write)
            {
                status = kaps_dm_kaps_hb_write(device, db, final_level_offset + (cur_hb_blk / 2), cur_row, data);
                if (status != KAPS_OK)
                {
                    *o_reason = NLMRSC_XPT_FAILED;
                    return NLMERR_FAIL;
                }

                stats->numHitBitWrites++;

                kaps_memset(data, 0, 2 * KAPS_HB_ROW_WIDTH_8);
            }

            hb_word = 0;
        }

        curBrick = curBrick->m_next_p;
        brick_idx++;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_kaps_clear_old_hit_bits(
    kaps_lsn_mc * self,
    uint32_t lsnInfoIndex,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    uint32_t level_nr, curLpuNr, curBktNr, rowNr, numLpusProcessed, numLpusToProcess;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    struct NlmFibStats *fibStats = &settings_p->m_fibTbl->m_fibStats;
    struct uda_mem_chunk *oldMlpMemInfo_p;
    kaps_status status;
    int32_t brick_idx;
    uint8_t data[2 * KAPS_HB_ROW_WIDTH_8] = { 0 };
    struct kaps_db *db = self->m_pTbl->m_db;
    uint32_t final_level_offset = kaps_device_get_final_level_offset(device, db);

    kaps_sassert(settings_p->m_isLsnInfoValid[lsnInfoIndex]);
    oldMlpMemInfo_p = settings_p->m_pTmpMlpMemInfoBuf[lsnInfoIndex];
    numLpusToProcess = settings_p->m_numBricksInStoredLsnInfo[lsnInfoIndex];

    brick_idx = 0;

    numLpusProcessed = 0;
    while (numLpusProcessed < numLpusToProcess)
    {
        ++numLpusProcessed;

        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), oldMlpMemInfo_p, brick_idx, &curLpuNr,
                                                   &rowNr);

        curBktNr = curLpuNr * device->hw_res->num_bb_in_one_chain;

        if (db->num_algo_levels_in_db == 3)
        {
            curBktNr = curBktNr / 2;
        }

        /*
         * Process each bucket in the cascade 
         */
        for (level_nr = 0; level_nr < device->hw_res->num_bb_in_one_chain; ++level_nr)
        {

            status = kaps_dm_kaps_hb_write(device, db, final_level_offset + curBktNr, rowNr, data);
            if (status != KAPS_OK)
            {
                *o_reason = NLMRSC_XPT_FAILED;
                return NLMERR_FAIL;
            }
            fibStats->numHitBitWrites++;

            ++curBktNr;
        }

        brick_idx++;
    }

    return errNum;
}

static NlmErrNum_t
kaps_kaps_get_hit_bits_for_lsn_from_hw(
    kaps_lsn_mc * curLsn)
{
    kaps_lsn_mc_settings *settings = curLsn->m_pSettings;
    struct kaps_device *device = settings->m_device;
    struct NlmFibStats *stats = &settings->m_fibTbl->m_fibStats;
    uint32_t i, cur_hb_blk;
    uint32_t curRow;
    kaps_lpm_lpu_brick *curBrick;
    uint32_t hb_word = 0;
    uint8_t buffer[2 * KAPS_HB_ROW_WIDTH_8];
    kaps_status status;
    uint32_t isHbPresentInLsn = 0;
    int32_t brick_idx;
    uint32_t actual_num_entries_per_hb_blk, pos_in_hb_blk;
    kaps_pfx_bundle *curPfxBundle;
    struct kaps_db *db = curLsn->m_pTbl->m_db;
    struct kaps_hb *hb = NULL;
    uint32_t final_level_offset = kaps_device_get_final_level_offset(device, db);

    isHbPresentInLsn = kaps_check_if_hb_is_present_in_lsn(curLsn);
    if (!isHbPresentInLsn)
        return NLMERR_OK;

    curBrick = curLsn->m_lpuList;
    brick_idx = 0;

    kaps_memset(buffer, 0, 2 * KAPS_HB_ROW_WIDTH_8);

    while (curBrick)
    {
        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings->m_pMlpMemMgr), curLsn->m_mlpMemInfo, brick_idx,
                                                   &cur_hb_blk, &curRow);
        cur_hb_blk = cur_hb_blk * device->hw_res->num_bb_in_one_chain;

        actual_num_entries_per_hb_blk = curBrick->m_maxCapacity / device->hw_res->num_bb_in_one_chain;
        pos_in_hb_blk = 0;

        if (db->num_algo_levels_in_db == 3)
        {

            status = kaps_dm_kaps_hb_read(device, db, final_level_offset + (cur_hb_blk / 2), curRow, buffer);
            if (status != KAPS_OK)
                return NLMERR_FAIL;

            stats->numHitBitReads++;

            if (cur_hb_blk % 2 == 1)
            {
                hb_word =
                    KapsReadBitsInArrray(&buffer[KAPS_HB_ROW_WIDTH_8], KAPS_HB_ROW_WIDTH_8, KAPS_HB_ROW_WIDTH_1 - 1, 0);
            }
            else
            {
                hb_word = KapsReadBitsInArrray(buffer, KAPS_HB_ROW_WIDTH_8, KAPS_HB_ROW_WIDTH_1 - 1, 0);
            }

            for (i = 0; i < curBrick->m_maxCapacity; ++i)
            {
                curPfxBundle = curBrick->m_pfxes[i];

                if (curPfxBundle && curPfxBundle->m_backPtr->hb_user_handle)
                {

                    KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb),
                                                (uintptr_t) curPfxBundle->m_backPtr->hb_user_handle);
                    if (hb == NULL)
                    {
                        return NLMERR_FAIL;
                    }
                    if (hb_word & (1u << pos_in_hb_blk))
                        hb->value = 1;
                    else
                        hb->value = 0;
                }

                pos_in_hb_blk++;
            }

        }
        else
        {

            hb_word = 0;

            for (i = 0; i < curBrick->m_maxCapacity; ++i)
            {
                curPfxBundle = curBrick->m_pfxes[i];

                if (pos_in_hb_blk == 0)
                {
                    status = kaps_dm_kaps_hb_read(device, db, final_level_offset + cur_hb_blk, curRow, buffer);
                    if (status != KAPS_OK)
                        return NLMERR_FAIL;

                    stats->numHitBitReads++;
                    hb_word = KapsReadBitsInArrray(buffer, KAPS_HB_ROW_WIDTH_8, KAPS_HB_ROW_WIDTH_1 - 1, 0);
                }

                if (curPfxBundle && curPfxBundle->m_backPtr->hb_user_handle)
                {

                    KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb),
                                                (uintptr_t) curPfxBundle->m_backPtr->hb_user_handle);
                    if (hb == NULL)
                    {
                        return NLMERR_FAIL;
                    }
                    if (hb_word & (1u << pos_in_hb_blk))
                        hb->value = 1;
                    else
                        hb->value = 0;
                }

                pos_in_hb_blk++;

                if (pos_in_hb_blk >= actual_num_entries_per_hb_blk)
                {
                    cur_hb_blk++;
                    pos_in_hb_blk = 0;
                }
            }
        }

        curBrick = curBrick->m_next_p;
        brick_idx++;
    }

    return NLMERR_OK;
}

void
kaps_lsn_mc_clear_mlp_data_generic(
    uint8_t * result_buf,
    uint32_t result_buf_size_8,
    uint32_t result_start_bit,
    uint16_t gran)
{
    int32_t numOfBits = gran;
    uint32_t cur_result_bit = result_start_bit;

    while (numOfBits >= 32)
    {
        KapsWriteBitsInArray(result_buf, result_buf_size_8, cur_result_bit + 31, cur_result_bit, 0);

        cur_result_bit += 32;
        numOfBits -= 32;
    }

    if (numOfBits)
    {
        KapsWriteBitsInArray(result_buf, result_buf_size_8, cur_result_bit + numOfBits - 1, cur_result_bit, 0);
    }

}

/*This function should be used when granularity is not a multiple of 8*/
void
kaps_lsn_mc_prepare_mlp_data_generic(
    uint8_t * pfxData,
    uint16_t pfxLen,
    uint16_t lopoffLen,
    uint16_t gran,
    uint8_t * result_buf,
    uint32_t result_buf_size_8,
    uint32_t result_end_bit)
{
    uint16_t tmp, numOfBits, pfxArraySizeInBits, startBit, endBit;
    uint8_t pfxArraySizeInBytes;
    uint32_t value = 0;
    uint32_t cur_result_bit = result_end_bit;

    numOfBits = pfxLen - lopoffLen;

    if (numOfBits)
    {
        pfxArraySizeInBytes = (uint8_t) ((pfxLen + 7) >> 3);
        pfxArraySizeInBits = pfxArraySizeInBytes << 3;

        tmp = numOfBits;
        startBit = lopoffLen;
        endBit = lopoffLen + 31;
        while (tmp >= 32)
        {
            value = KapsReadBitsInArrray(pfxData, pfxArraySizeInBytes, pfxArraySizeInBits - startBit - 1,
                                     pfxArraySizeInBits - endBit - 1);

            KapsWriteBitsInArray(result_buf, result_buf_size_8, cur_result_bit, cur_result_bit - 31, value);

            tmp -= 32;
            startBit += 32;
            endBit += 32;

            cur_result_bit -= 32;
        }

        value = 0;
        if (tmp)
        {
            endBit = pfxLen - 1;
            value = KapsReadBitsInArrray(pfxData, pfxArraySizeInBytes, pfxArraySizeInBits - startBit - 1,
                                     pfxArraySizeInBits - endBit - 1);
        }
        /*
         * After prefix length, set next bit as 1 for MPE encoding, remaining data is already 0 
         */
        value = (value << 1) + 1;

        /*
         * It is cur_result_bit - tmp and NOT cur_result_bit - tmp + 1 since end bit also has one bit for MPE 
         */
        KapsWriteBitsInArray(result_buf, result_buf_size_8, cur_result_bit, cur_result_bit - tmp, value);
    }
    else
    {
        /*
         * Set first bit as 1 for MPE encoding, remaining data is already 0 
         */
        KapsWriteBitsInArray(result_buf, result_buf_size_8, cur_result_bit, cur_result_bit, 1);
    }

    return;
}




/*
pfxData - Prefix data including lopoff
pfxLen - Prefix length including lopoff
lopoffLen - Lopoff of corresponding IPT entry
o_data - resultant data with MPE encoding and lopoff excluded
*/
void
kaps_lsn_mc_prepare_mlp_data(
    uint8_t * pfxData,
    uint16_t pfxLen,
    uint16_t lopoffLen,
    uint16_t gran,
    uint8_t * o_data)
{
    uint16_t tmp, numOfBits, pfxArraySizeInBits, mlpDataArraySizeInBits, startBit, endBit;
    uint8_t pfxArraySizeInBytes, mlpDataArraySizeInBytes, i = 0;
    uint32_t value = 0;

    numOfBits = pfxLen - lopoffLen;

    if (numOfBits)
    {
        pfxArraySizeInBytes = (uint8_t) ((pfxLen + 7) >> 3);
        pfxArraySizeInBits = pfxArraySizeInBytes << 3;

        mlpDataArraySizeInBytes = (uint8_t) ((gran + 7) >> 3);
        mlpDataArraySizeInBits = mlpDataArraySizeInBytes << 3;

        tmp = numOfBits;
        startBit = lopoffLen;
        endBit = lopoffLen + 31;
        while (tmp >= 32)
        {
            value = KapsReadBitsInArrray(pfxData, pfxArraySizeInBytes, pfxArraySizeInBits - startBit - 1,
                                     pfxArraySizeInBits - endBit - 1);

            o_data[i++] = (uint8_t) ((value >> 24) & 0xFF);
            o_data[i++] = (uint8_t) ((value >> 16) & 0xFF);
            o_data[i++] = (uint8_t) ((value >> 8) & 0xFF);
            o_data[i++] = (uint8_t) (value & 0xFF);

            tmp -= 32;
            startBit += 32;
            endBit += 32;
        }

        value = 0;
        if (tmp)
        {
            endBit = pfxLen - 1;
            value = KapsReadBitsInArrray(pfxData, pfxArraySizeInBytes, pfxArraySizeInBits - startBit - 1,
                                     pfxArraySizeInBits - endBit - 1);
        }
        /*
         * After prefix length, set next bit as 1 for MPE encoding, remaining data is already 0 
         */
        startBit = i * 8;
        endBit = startBit + tmp;        /* end bit also has one bit for MPE */
        value = (value << 1) + 1;
        KapsWriteBitsInArray(o_data, mlpDataArraySizeInBytes, mlpDataArraySizeInBits - startBit - 1,
                         mlpDataArraySizeInBits - endBit - 1, value);
    }
    else
    {
        /*
         * Set first bit as 1 for MPE encoding, remaining data is already 0 
         */
        o_data[0] = 0x80;
    }

    return;
}



static NlmErrNum_t
kaps_kaps_del_entry_in_mlp(
    kaps_lsn_mc * self,
    uint32_t index,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_fib_tbl_mgr *fibMgr_p = self->m_pTbl->m_db->lpm_mgr->fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    uint32_t pfxIndexInLsn = 0, indexSoFar, pfxIndexInLpu, pfxIndexInBkt;
    kaps_lpm_lpu_brick *curBrick;
    uint32_t lpu_nr, bkt_nr, row_nr, numPfxPerBkt, brick_idx;
    struct uda_mem_chunk *mem_info = self->m_mlpMemInfo;
    uint8_t *bktData;
    int32_t start_array_index, start_bit_pos, num_bytes;
    NlmErrNum_t errNum;
    struct kaps_ad_db *ad_db = NULL;

    if (device->issu_status == KAPS_ISSU_SAVE_COMPLETED)
        return KAPS_OK;

    pfxIndexInLsn = kaps_lsn_mc_get_index_in_lsn(self, index);

    curBrick = self->m_lpuList;
    indexSoFar = 0;
    brick_idx = 0;

    while (curBrick)
    {
        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), mem_info, brick_idx, &lpu_nr, &row_nr);
        if (indexSoFar <= pfxIndexInLsn && (pfxIndexInLsn < indexSoFar + curBrick->m_maxCapacity))
        {
            break;
        }
        indexSoFar += curBrick->m_maxCapacity;
        curBrick = curBrick->m_next_p;
        brick_idx++;
    }

    if (!curBrick)
        return NLMERR_FAIL;

    
    ad_db = kaps_lsn_mc_get_ad_db(self, curBrick);

    numPfxPerBkt = curBrick->m_maxCapacity / device->hw_res->num_bb_in_one_chain;
    pfxIndexInLpu = pfxIndexInLsn - indexSoFar;
    pfxIndexInBkt = pfxIndexInLpu % numPfxPerBkt;
    bkt_nr = (lpu_nr * device->hw_res->num_bb_in_one_chain) + (pfxIndexInLpu / numPfxPerBkt);

    bktData = kaps_kaps_get_bkt_data(self, device, bkt_nr, row_nr);

    if (curBrick->m_gran % 8 == 0)
    {
        start_array_index = (pfxIndexInBkt * curBrick->m_gran) / KAPS_BITS_IN_BYTE;
        num_bytes = curBrick->m_gran / KAPS_BITS_IN_BYTE;
        kaps_memset(&bktData[start_array_index], 0, num_bytes);
    }
    else
    {
        kaps_lsn_mc_clear_mlp_data_generic(bktData, KAPS_BKT_WIDTH_8,
                                           KAPS_BKT_WIDTH_1 - ((pfxIndexInBkt + 1) * curBrick->m_gran),
                                           curBrick->m_gran);
    }

    if (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INPLACE
        && ad_db->user_width_1 > 0)
    {
        start_bit_pos = (pfxIndexInBkt * ad_db->db_info.width.ad_width_1);
        KapsWriteBitsInArray(bktData, KAPS_BKT_WIDTH_8, 
            start_bit_pos + ad_db->db_info.width.ad_width_1 - 1, start_bit_pos, 0);
    }

    errNum = kaps_kaps_bb_write_wrapper(self, device, bkt_nr, row_nr, 1, KAPS_BKT_WIDTH_8, bktData, o_reason);

    fibStats->numLSNRmw++;

    return errNum;
}

uint32_t
kaps_kaps_get_old_pfx_hb_index(
    kaps_lsn_mc * curLsn,
    uint32_t oldPfxIndex)
{
    kaps_lsn_mc_settings *settings = curLsn->m_pSettings;
    struct kaps_device *device = settings->m_device;
    struct kaps_db *db = curLsn->m_pTbl->m_db;
    uint32_t found;
    uint32_t curBktStartIx, curBktEndIx;
    uint32_t pfx_hb_index = 0;
    uint32_t num_entries_per_bb, which_bb_in_cascade;
    uint32_t maxCapacity = 0;
    kaps_lpm_lpu_brick *curBrick;
    uint32_t curLpuNr, rowNr, brick_idx;
    uint32_t num_active_hb_blocks = kaps_device_get_active_num_hb_blocks(device, db);

    if (oldPfxIndex == KAPS_LSN_NEW_INDEX)
        return KAPS_LSN_NEW_INDEX;

    if (settings->m_isPerLpuGran)
        curBktStartIx = curLsn->m_lpuList->m_ixInfo->start_ix;
    else
        curBktStartIx = curLsn->m_ixInfo->start_ix;

    brick_idx = 0;

    found = 0;
    curBrick = curLsn->m_lpuList;
    while (curBrick)
    {
        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings->m_pMlpMemMgr), curLsn->m_mlpMemInfo, brick_idx,
                                                   &curLpuNr, &rowNr);

        pfx_hb_index = (rowNr * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1)
            + (curLpuNr * device->hw_res->num_bb_in_one_chain * KAPS_HB_ROW_WIDTH_1);

        if (settings->m_isPerLpuGran)
            curBktStartIx = pfx_hb_index;

        maxCapacity = curBrick->m_maxCapacity;
        curBktEndIx = curBktStartIx + maxCapacity - 1;
        if (curBktStartIx <= oldPfxIndex && oldPfxIndex <= curBktEndIx)
        {
            found = 1;
            break;
        }

        if (!settings->m_isPerLpuGran)
            curBktStartIx += maxCapacity;

        curBrick = curBrick->m_next_p;
        brick_idx++;
    }

    if (!found)
    {
        kaps_assert(0, "Old prefix index not found \n");
    }

    num_entries_per_bb = maxCapacity / device->hw_res->num_bb_in_one_chain;
    which_bb_in_cascade = (oldPfxIndex - curBktStartIx) / num_entries_per_bb;

    pfx_hb_index += (which_bb_in_cascade * KAPS_HB_ROW_WIDTH_1);
    pfx_hb_index += (oldPfxIndex - curBktStartIx) - (which_bb_in_cascade * num_entries_per_bb);

    return pfx_hb_index;
}






static NlmErrNum_t
kaps_kaps_write_entry_to_mlp(
    kaps_lsn_mc * self,
    uint16_t gran,
    kaps_pfx_bundle * pfx,
    uint32_t oldix,
    uint32_t newix,
    uint32_t is_ad_update,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_fib_tbl_mgr *fibMgr_p = self->m_pTbl->m_db->lpm_mgr->fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    struct kaps_db *db = fibMgr_p->m_curFibTbl->m_db;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t pfxIndexInLsn = 0, indexSoFar, pfxIndexInLpu, pfxIndexInBkt;
    kaps_lpm_lpu_brick *curBrick;
    uint32_t lpu_nr = 0, bkt_nr, row_nr = 0, numPfxPerBkt, brick_idx;
    struct uda_mem_chunk *mem_info = self->m_mlpMemInfo;
    uint8_t *bktData;
    uint32_t array_index, start_bit_pos;
    struct kaps_ad *ad_handle = pfx->m_backPtr->ad_handle;
    uint32_t ad_value;
    uint32_t old_hb_index = KAPS_LSN_NEW_INDEX, new_hb_index = KAPS_LSN_NEW_INDEX;
    uint32_t num_active_hb_blocks = kaps_device_get_active_num_hb_blocks(device, db);
    struct kaps_ad_db *ad_db = NULL;

    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, pfx->m_backPtr->ad_handle, ad_db);

    pfxIndexInLsn = kaps_lsn_mc_get_index_in_lsn(self, pfx->m_nIndex);

    if (settings_p->m_areHitBitsPresent)
    {
        old_hb_index = kaps_kaps_get_old_pfx_hb_index(self, oldix);
    }

    curBrick = self->m_lpuList;
    brick_idx = 0;
    indexSoFar = 0;

    while (curBrick)
    {
        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), mem_info, brick_idx, &lpu_nr, &row_nr);

        if (indexSoFar <= pfxIndexInLsn && (pfxIndexInLsn < indexSoFar + curBrick->m_maxCapacity))
        {
            break;
        }
        indexSoFar += curBrick->m_maxCapacity;
        curBrick = curBrick->m_next_p;
        brick_idx++;
    }

    kaps_assert((curBrick != NULL), "Invalid MLP brick.\n");
    numPfxPerBkt = curBrick->m_maxCapacity / device->hw_res->num_bb_in_one_chain;
    pfxIndexInLpu = pfxIndexInLsn - indexSoFar;
    pfxIndexInBkt = pfxIndexInLpu % numPfxPerBkt;
    bkt_nr = (lpu_nr * device->hw_res->num_bb_in_one_chain) + (pfxIndexInLpu / numPfxPerBkt);

    bktData = kaps_kaps_get_bkt_data(self, device, bkt_nr, row_nr);

    array_index = (pfxIndexInBkt * curBrick->m_gran) / KAPS_BITS_IN_BYTE;

    

    

    /*
     * 'pfxdata' is the actual prefix. 'pfxlen' is the total prefix length. 'lopoff' bits are * lopped off by SES.
     * Following function prepares MLP data (remaining bits after lopoff) in MPE format. 
     */
    if (!is_ad_update)
    {
        if (gran % 8 == 0)
        {
            kaps_lsn_mc_prepare_mlp_data(pfx->m_data, pfx->m_nPfxSize, self->m_nLopoff, gran, &bktData[array_index]);
        }
        else
        {
            kaps_lsn_mc_prepare_mlp_data_generic(pfx->m_data, pfx->m_nPfxSize, self->m_nLopoff, gran, bktData,
                                                KAPS_BKT_WIDTH_8,
                                                KAPS_BKT_WIDTH_1 - 1 - (pfxIndexInBkt * curBrick->m_gran));
        }
    }


    if (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INPLACE 
        && ad_db->user_width_1 > 0)
    {

        ad_value = KapsReadBitsInArrray(&ad_handle->value[0], (ad_db->db_info.width.ad_width_1 + 7)/8, KAPS_AD_ARRAY_END_BIT_POS,
                                    KAPS_AD_ARRAY_START_BIT_POS);
        start_bit_pos = (pfxIndexInBkt * ad_db->db_info.width.ad_width_1);
        KapsWriteBitsInArray(bktData, KAPS_BKT_WIDTH_8, 
                start_bit_pos + ad_db->db_info.width.ad_width_1 - 1, start_bit_pos, ad_value);

    }


    

    errNum = kaps_kaps_bb_write_wrapper(self, device, bkt_nr, row_nr, 1, KAPS_BKT_WIDTH_8, bktData, o_reason);

    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    fibStats->numLSNRmw++;

    kaps_lsn_mc_update_prefix(self, pfx, oldix, newix);

    if (settings_p->m_areHitBitsPresent)
    {
        new_hb_index = (row_nr * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1) + (bkt_nr * KAPS_HB_ROW_WIDTH_1);
        new_hb_index += pfxIndexInBkt;
        if (settings_p->m_pHitBitCB)
        {
            settings_p->m_pHitBitCB(settings_p->m_pHitBitCBData, (struct kaps_entry *) pfx->m_backPtr, old_hb_index,
                                    new_hb_index);
        }
    }

    return errNum;
}

static NlmErrNum_t
kaps_kaps_sbc_commit(
    struct uda_mem_chunk *uda_chunk,
    int32_t to_region_id,
    int32_t to_offset)
{
    uint32_t level_nr, srcBktNr, destBktNr;
    uint32_t oldix, newix;
    uint32_t numSlotsInPrevLpus = 0;
    uint8_t *bktData;
    kaps_lsn_mc *curLsn = (kaps_lsn_mc *) (uda_chunk->handle);
    kaps_fib_tbl_mgr *tbl_mgr = curLsn->m_pTbl->m_fibTblMgr_p;
    struct kaps_device *device = tbl_mgr->m_devMgr_p;
    struct kaps_db *db = tbl_mgr->m_curFibTbl->m_db; 
    struct NlmFibStats *fibStats = &tbl_mgr->m_curFibTbl->m_fibStats;
    struct kaps_lsn_mc_settings *settings = tbl_mgr->m_curFibTbl->m_trie->m_lsn_settings_p;
    struct kaps_uda_mgr *mgr = *(settings->m_pMlpMemMgr);
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    uint32_t iterInBkt, iterInLpu;
    kaps_lpm_lpu_brick *curBrick;
    uint32_t isHbPresentInLsn = 0;
    kaps_status status;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode o_reason;
    uint32_t final_level_offset = kaps_device_get_final_level_offset(device, db);
    uint32_t total_num_bbs_in_final_level = kaps_device_get_num_final_level_bbs(device, db);
    uint32_t db_group_id = db->db_group_id;

    uint32_t src_row_num;
    uint32_t src_udc_num;
    uint32_t dest_row_num;
    uint32_t dest_udc_num;
    int32_t brick_idx;

    device = tbl_mgr->m_devMgr_p;

    if (settings->m_areHitBitsPresent)
    {
        isHbPresentInLsn = kaps_check_if_hb_is_present_in_lsn(curLsn);
    }

    curBrick = curLsn->m_lpuList;
    for (brick_idx = 0; brick_idx < uda_chunk->size; brick_idx++)
    {

        kaps_uda_mgr_compute_abs_brick_udc_and_row(mgr, uda_chunk, brick_idx, &src_udc_num, &src_row_num);

        kaps_kaps_uda_mgr_compute_brick_udc_and_row_from_region_id(mgr, to_region_id, to_offset, brick_idx,
                                                                   &dest_udc_num, &dest_row_num);

        srcBktNr = src_udc_num * device->hw_res->num_bb_in_one_chain;
        destBktNr = dest_udc_num * device->hw_res->num_bb_in_one_chain;
        iterInLpu = 0;

        if (settings->m_isHardwareMappedIx)
        {
            if (settings->m_isPerLpuGran)
            {
                curBrick->m_ixInfo->start_ix =
                    (dest_row_num * total_num_bbs_in_final_level * settings->m_maxNumPfxInAnyBrick) +
                    (dest_udc_num * settings->m_maxNumPfxInAnyBrick);
                curBrick->m_ixInfo->size = settings->m_maxNumPfxInAnyBrick;
            }
            else if (brick_idx == 0)
            {
                curLsn->m_ixInfo->start_ix =
                    (dest_row_num * total_num_bbs_in_final_level * settings->m_maxNumPfxInAnyBrick) +
                    (dest_udc_num * settings->m_maxNumPfxInAnyBrick);
                curLsn->m_nAllocBase = curLsn->m_ixInfo->start_ix;
            }
        }

        /*
         * Process each bucket in the cascade 
         */
        for (level_nr = 0; level_nr < device->hw_res->num_bb_in_one_chain; ++level_nr)
        {
            bktData = shadow->bkt_blks[srcBktNr].bkt_rows[src_row_num].data;

            kaps_memcpy(shadow->bkt_blks[destBktNr].bkt_rows[dest_row_num].data, bktData, KAPS_BKT_WIDTH_8);
            errNum =
                kaps_kaps_bb_write_wrapper(curLsn, device, destBktNr, dest_row_num, 1, KAPS_BKT_WIDTH_8,
                                      bktData, &o_reason);
            if (errNum != NLMERR_OK)
            {
                return errNum;
            }

            fibStats->numLSNRmw++;

            if (settings->m_areHitBitsPresent || settings->m_isHardwareMappedIx)
            {
                uint32_t maxNumPfxInBkt = curBrick->m_maxCapacity / device->hw_res->num_bb_in_one_chain;
                uint32_t src_hb_index, dest_hb_index;

                src_hb_index =
                    (src_row_num * device->hw_res->total_num_bb[db_group_id] * KAPS_HB_ROW_WIDTH_1) +
                    (srcBktNr * KAPS_HB_ROW_WIDTH_1);
                dest_hb_index =
                    (dest_row_num * device->hw_res->total_num_bb[db_group_id] * KAPS_HB_ROW_WIDTH_1) +
                    (destBktNr * KAPS_HB_ROW_WIDTH_1);

                for (iterInBkt = 0; iterInBkt < maxNumPfxInBkt; ++iterInBkt)
                {
                    /*
                     * Process the prefixes in the bucket
                     */
                    kaps_pfx_bundle *b = curBrick->m_pfxes[iterInLpu];

                    if (b)
                    {
                        oldix = KAPS_PFX_BUNDLE_GET_INDEX(b);
                        newix = kaps_lsn_mc_get_index(curLsn, numSlotsInPrevLpus + iterInLpu);

                        b->m_nIndex = newix;

                        if (settings->m_pHitBitCB)
                        {
                            settings->m_pHitBitCB(settings->m_pHitBitCBData, (struct kaps_entry *) b->m_backPtr,
                                                  src_hb_index, dest_hb_index);
                        }

                        kaps_lsn_mc_update_prefix(curLsn, b, oldix, newix);
                    }
                    ++src_hb_index;
                    ++dest_hb_index;
                    ++iterInLpu;
                }

                if (isHbPresentInLsn)
                {
                    status = kaps_dm_kaps_hb_copy(device, db, final_level_offset + srcBktNr, src_row_num,
                                                  final_level_offset + destBktNr, dest_row_num, 0xFFFF, 0, 0);
                    if (status != KAPS_OK)
                    {
                        o_reason = NLMRSC_XPT_FAILED;
                        return NLMERR_FAIL;
                    }
                    fibStats->numHitBitCopy++;
                }
            }

            ++srcBktNr;
            ++destBktNr;
        }

        numSlotsInPrevLpus += curBrick->m_maxCapacity;

        if (brick_idx < (uda_chunk->size - 1))
            curBrick = curBrick->m_next_p;
    }

    return NLMERR_OK;
}

static uint32_t
kaps_kaps_search_old_lsn_for_hit_bits(
    kaps_lsn_mc_settings * settings,
    uint32_t lsnInfoIndex,
    uint32_t oldPfxIndex,
    uint32_t * found)
{
    struct kaps_device *device = settings->m_device;
    struct kaps_db *db = settings->m_fibTbl->m_db;
    uint32_t i;
    uint32_t curBktStartIx, curBktEndIx;
    uint32_t pfx_hb_index = 0;
    uint32_t num_entries_per_bb, which_bb_in_cascade;
    uint32_t maxCapacity = 0;
    uint32_t oldLsnStartIx, oldLsnNumBricks;
    uint32_t *oldLsnMaxCapacity;
    uint32_t curLpuNr, rowNr;
    struct uda_mem_chunk *oldMlpMemInfo;
    uint32_t num_active_hb_blocks = kaps_device_get_active_num_hb_blocks(device, db);

    if (oldPfxIndex == KAPS_LSN_NEW_INDEX)
    {
        *found = 1;
        return KAPS_LSN_NEW_INDEX;
    }

    oldMlpMemInfo = settings->m_pTmpMlpMemInfoBuf[lsnInfoIndex];
    oldLsnStartIx = settings->m_startIxOfStoredLsnInfo[lsnInfoIndex];
    oldLsnNumBricks = settings->m_numBricksInStoredLsnInfo[lsnInfoIndex];
    oldLsnMaxCapacity = settings->m_maxCapacityOfStoredLsnInfo[lsnInfoIndex];

    curBktStartIx = oldLsnStartIx;
    *found = 0;

    for (i = 0; i < oldLsnNumBricks; ++i)
    {

        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings->m_pMlpMemMgr), oldMlpMemInfo, i, &curLpuNr, &rowNr);

        pfx_hb_index = (rowNr * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1)
            + (curLpuNr * device->hw_res->num_bb_in_one_chain * KAPS_HB_ROW_WIDTH_1);

        if (settings->m_isPerLpuGran)
            curBktStartIx = pfx_hb_index;

        maxCapacity = oldLsnMaxCapacity[i];
        curBktEndIx = curBktStartIx + maxCapacity - 1;
        if (curBktStartIx <= oldPfxIndex && oldPfxIndex <= curBktEndIx)
        {
            *found = 1;
            break;
        }

        if (!settings->m_isPerLpuGran)
        {
            curBktStartIx += maxCapacity;
        }
    }

    if (*found == 0)
        return KAPS_LSN_NEW_INDEX;

    num_entries_per_bb = maxCapacity / device->hw_res->num_bb_in_one_chain;
    which_bb_in_cascade = (oldPfxIndex - curBktStartIx) / num_entries_per_bb;

    pfx_hb_index += (which_bb_in_cascade * KAPS_HB_ROW_WIDTH_1);
    pfx_hb_index += (oldPfxIndex - curBktStartIx) - (which_bb_in_cascade * num_entries_per_bb);

    return pfx_hb_index;
}

static uint32_t
kaps_kaps_get_old_lsn_pfx_hb_index(
    kaps_lsn_mc_settings * settings,
    uint32_t oldPfxIndex)
{
    uint32_t found = 0;
    uint32_t pfx_hb_index;
    uint32_t i;

    pfx_hb_index = kaps_kaps_search_old_lsn_for_hit_bits(settings, 0, oldPfxIndex, &found);

    i = 1;
    while (!found && settings->m_isLsnInfoValid[i])
    {
        pfx_hb_index = kaps_kaps_search_old_lsn_for_hit_bits(settings, i, oldPfxIndex, &found);

        if (found)
            break;

        ++i;
    }

    if (!found)
    {
        kaps_assert(0, "Old prefix index not found \n");
    }

    return pfx_hb_index;
}

static NlmErrNum_t
kaps_kaps_normal_commit(
    kaps_lsn_mc * self,
    uint32_t isSbcPossible,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    uint16_t maxNumPfxInBkt;
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    kaps_lpm_lpu_brick *curLpuBrick;
    uint32_t level_nr, curLpuNr, curBktNr, rowNr, numLpusProcessed, brick_idx;
    uint32_t iterInLpu, iterInBkt;
    uint32_t oldix, newix;
    uint32_t numSlotsInPrevLpus;
    uint8_t *bktData;
    uint32_t array_index, start_bit_pos;
    struct kaps_ad *ad_handle;
    uint32_t ad_value;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_fib_tbl_mgr *fibMgr_p = self->m_pTbl->m_db->lpm_mgr->fib_tbl_mgr;
    struct kaps_db *db = fibMgr_p->m_curFibTbl->m_db;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    struct uda_mem_chunk *oldMlpMemInfo_p = settings_p->m_pTmpMlpMemInfoBuf[0];
    uint32_t srcLpuNum = 0, srcBktNum = 0, srcRowNum = 0, totalNumSrcLpus = 0;
    kaps_status status;
    uint32_t old_hb_bkt_start_index, new_hb_bkt_start_index;
    uint32_t old_pfx_hb_index = 0, new_pfx_hb_index;
    uint32_t isHbPresentInLsn = 0;
    uint32_t final_level_offset = kaps_device_get_final_level_offset(device, db);
    uint32_t num_active_hb_blocks = kaps_device_get_active_num_hb_blocks(device, db);
    struct kaps_ad_db *ad_db = NULL;

    /*
     * Make sure resources are allocated for this LSN 
     */
    kaps_assert(self->m_bAllocedResource, "Resources not allocated");

    if (settings_p->m_areHitBitsPresent)
    {
        isHbPresentInLsn = kaps_check_if_hb_is_present_in_lsn(self);
    }

    curLpuBrick = self->m_lpuList;
    brick_idx = 0;
    numSlotsInPrevLpus = 0;

    if (isSbcPossible)
    {
        kaps_sassert(settings_p->m_isLsnInfoValid[0]);
        totalNumSrcLpus = oldMlpMemInfo_p->size;
    }

    numLpusProcessed = 0;
    while (curLpuBrick)
    {

        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), self->m_mlpMemInfo, brick_idx,
                                                   &curLpuNr, &rowNr);

        if (isSbcPossible && (numLpusProcessed < totalNumSrcLpus))
        {
            kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), oldMlpMemInfo_p, brick_idx,
                                                       &srcLpuNum, &srcRowNum);
        }

        iterInLpu = 0;
        maxNumPfxInBkt = curLpuBrick->m_maxCapacity / device->hw_res->num_bb_in_one_chain;
        curBktNr = curLpuNr * device->hw_res->num_bb_in_one_chain;
        srcBktNum = srcLpuNum * device->hw_res->num_bb_in_one_chain;

        /*
         * Process each bucket in the cascade 
         */
        for (level_nr = 0; level_nr < device->hw_res->num_bb_in_one_chain; ++level_nr)
        {
            bktData = kaps_kaps_get_bkt_data(self, device, curBktNr, rowNr);
            kaps_memset(bktData, 0, KAPS_BKT_WIDTH_8);

            old_hb_bkt_start_index = (srcRowNum * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1)
                + (srcBktNum * KAPS_HB_ROW_WIDTH_1);

            new_hb_bkt_start_index = (rowNr * num_active_hb_blocks * KAPS_HB_ROW_WIDTH_1)
                + (curBktNr * KAPS_HB_ROW_WIDTH_1);

            ad_db = kaps_lsn_mc_get_ad_db(self, curLpuBrick);

            for (iterInBkt = 0; iterInBkt < maxNumPfxInBkt; ++iterInBkt)
            {
                /*
                 * Process the prefixes in the bucket
                 */
                kaps_pfx_bundle *b = curLpuBrick->m_pfxes[iterInLpu];

                if (b)
                {
                    oldix = KAPS_PFX_BUNDLE_GET_INDEX(b);
                    newix = kaps_lsn_mc_get_index(self, numSlotsInPrevLpus + iterInLpu);

                    b->m_nIndex = newix;

                    if (curLpuBrick->m_gran % 8 == 0)
                    {
                        array_index = iterInBkt * curLpuBrick->m_gran / KAPS_BITS_IN_BYTE;
                        kaps_lsn_mc_prepare_mlp_data(b->m_data, b->m_nPfxSize, self->m_nLopoff, curLpuBrick->m_gran,
                                                     &bktData[array_index]);
                    }
                    else
                    {
                        kaps_lsn_mc_prepare_mlp_data_generic(b->m_data, b->m_nPfxSize, self->m_nLopoff,
                                                            curLpuBrick->m_gran, bktData, KAPS_BKT_WIDTH_8,
                                                            KAPS_BKT_WIDTH_1 - 1 - (iterInBkt * curLpuBrick->m_gran));
                    }

                    if (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INPLACE && 
                        ad_db->user_width_1 > 0)
                    {
                        ad_handle = b->m_backPtr->ad_handle;

                        
                        ad_value = KapsReadBitsInArrray(&ad_handle->value[0], (ad_db->db_info.width.ad_width_1 + 7)/8, 
                            KAPS_AD_ARRAY_END_BIT_POS, KAPS_AD_ARRAY_START_BIT_POS);
                        
                        start_bit_pos = iterInBkt * ad_db->db_info.width.ad_width_1;
                        KapsWriteBitsInArray(bktData, KAPS_BKT_WIDTH_8, 
                            start_bit_pos + ad_db->db_info.width.ad_width_1 - 1, start_bit_pos,
                                         ad_value);
                    }

                   
                    kaps_lsn_mc_update_prefix(self, b, oldix, newix);

                    if (settings_p->m_pHitBitCB)
                    {
                        if (oldix == KAPS_LSN_NEW_INDEX)
                        {
                            old_pfx_hb_index = KAPS_LSN_NEW_INDEX;
                        }
                        else if (isSbcPossible)
                        {
                            old_pfx_hb_index = old_hb_bkt_start_index + iterInBkt;
                        }
                        else if (settings_p->m_isLsnInfoValid[0])
                        {
                            old_pfx_hb_index = kaps_kaps_get_old_lsn_pfx_hb_index(settings_p, oldix);
                        }
                        else
                        {
                            kaps_assert(0, "Unable to determine old prefix hit bit index \n");
                        }

                        new_pfx_hb_index = new_hb_bkt_start_index + iterInBkt;

                        settings_p->m_pHitBitCB(settings_p->m_pHitBitCBData, (struct kaps_entry *) b->m_backPtr,
                                                old_pfx_hb_index, new_pfx_hb_index);

                        

                    }

                }
                ++iterInLpu;

            }



            errNum =
                kaps_kaps_bb_write_wrapper(self, device, curBktNr, rowNr, 1, KAPS_BKT_WIDTH_8, bktData,
                                      o_reason);
            if (errNum != NLMERR_OK)
            {
                return errNum;
            }

            fibStats->numLSNRmw++;

            if (isSbcPossible && settings_p->m_areHitBitsPresent && isHbPresentInLsn)
            {
                if (numLpusProcessed < totalNumSrcLpus)
                {
                    status = kaps_dm_kaps_hb_copy(device, db, final_level_offset + srcBktNum, srcRowNum,
                                                  final_level_offset + curBktNr, rowNr, 0xFFFF, 0, 0);

                    if (status != KAPS_OK)
                    {
                        *o_reason = NLMRSC_XPT_FAILED;
                        return NLMERR_FAIL;
                    }
                    fibStats->numHitBitCopy++;
                }
            }

            ++curBktNr;
            ++srcBktNum;
        }

        numSlotsInPrevLpus += curLpuBrick->m_maxCapacity;

        ++numLpusProcessed;
        curLpuBrick = curLpuBrick->m_next_p;
        brick_idx++;
    }

    if (settings_p->m_areHitBitsPresent && !isSbcPossible)
    {
        if (settings_p->m_isLsnInfoValid[0])
        {
            /*
             * This is due to an LSN split
             */
            if (isHbPresentInLsn)
            {
                errNum = kaps_kaps_commit_hitbits(self, o_reason);
                if (errNum != NLMERR_OK)
                    return errNum;
            }
        }
    }

    return errNum;
}

static NlmErrNum_t
kaps_kaps_clear_lpu_for_grow(
    kaps_lsn_mc * self,
    int32_t isGrowUp,
    uint32_t numLPUsToClear,
    NlmReasonCode * o_reason)
{
    uint32_t i, destLPU = 0, destBktNr = 0;
    uint32_t destRow = 0;
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_fib_tbl_mgr *fibMgr_p = self->m_pTbl->m_db->lpm_mgr->fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    uint8_t *bktData;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t loopCnt, brickIter;

    if (isGrowUp)
    {
        kaps_assert(numLPUsToClear == 1, "Incorrect number of LPUs to clear");
        brickIter = 0;
    }
    else
    {
        kaps_assert(numLPUsToClear == 1 || numLPUsToClear == 2, "Incorrect number of LPUs to clear");

        if (numLPUsToClear == 1)
        {
            brickIter = self->m_mlpMemInfo->size - 1;
        }
        else
        {
            brickIter = self->m_mlpMemInfo->size - 2;
        }
    }

    for (loopCnt = 0; loopCnt < numLPUsToClear; ++loopCnt)
    {

        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), self->m_mlpMemInfo, brickIter, &destLPU,
                                                   &destRow);

        destBktNr = destLPU * device->hw_res->num_bb_in_one_chain;
        for (i = 0; i < device->hw_res->num_bb_in_one_chain; ++i)
        {
            bktData = kaps_kaps_get_bkt_data(self, device, destBktNr, destRow);
            kaps_memset(bktData, 0, KAPS_BKT_WIDTH_8);

            errNum =
                kaps_kaps_bb_write_wrapper(self, device, destBktNr, destRow, 1, KAPS_BKT_WIDTH_8, bktData,
                                      o_reason);

            if (errNum != NLMERR_OK)
            {
                return errNum;
            }

            fibStats->numLSNRmw++;

            ++destBktNr;
        }

        brickIter++;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_write_entry_to_hw(
    kaps_lsn_mc * self,
    uint16_t lpuGran,
    kaps_pfx_bundle * pfx,
    uint32_t oldix,
    uint32_t newix,
    uint32_t is_ad_update,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;

    errNum = kaps_kaps_write_entry_to_mlp(self, lpuGran, pfx, oldix, newix, is_ad_update, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_lsn_mc_delete_entry_in_hw(
    kaps_lsn_mc * self,
    int ix,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;

    errNum = kaps_kaps_del_entry_in_mlp(self, ix, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_lsn_mc_commit(
    kaps_lsn_mc * self,
    int32_t isSbcPossible,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;

    errNum = kaps_kaps_normal_commit(self, isSbcPossible, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_lsn_mc_clear_lpu_for_grow(
    kaps_lsn_mc * self,
    int32_t isGrowUp,
    uint32_t numLpusToClear,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;

    errNum = kaps_kaps_clear_lpu_for_grow(self, isGrowUp, numLpusToClear, o_reason);

    return errNum;

}

NlmErrNum_t
kaps_lsn_mc_commitIIT(
    kaps_lsn_mc * self,
    uint32_t index,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;

    errNum = kaps_kaps_iit_commit(self, index, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_lsn_mc_update_iit(
    kaps_lsn_mc * self,
    NlmReasonCode * o_reason)
{
    kaps_pfx_bundle *parent = self->m_pParentHandle;
    NlmErrNum_t errNum = NLMERR_OK;

    errNum = kaps_lsn_mc_commitIIT(self, parent->m_nIndex, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(
    kaps_lsn_mc * curLsn)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_lsn_mc_settings *settings = curLsn->m_pSettings;
    struct kaps_device *device = settings->m_device;

    if (!settings->m_areHitBitsPresent)
        return NLMERR_OK;

    if (device->type == KAPS_DEVICE_KAPS)
    {
        errNum = kaps_kaps_get_hit_bits_for_lsn_from_hw(curLsn);
    }

    return errNum;
}

kaps_status
kaps_fib_update_it_callback(
    struct uda_mem_chunk * uda_chunk)
{
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode o_reason;
    kaps_lsn_mc *self = (kaps_lsn_mc *) (uda_chunk->handle);

    if (!self->m_mlpMemInfo)
        kaps_sassert(0);

    errNum = kaps_lsn_mc_update_iit((kaps_lsn_mc *) self, &o_reason);
    if (errNum != NLMERR_OK)
        return KAPS_INTERNAL_ERROR;

    return KAPS_OK;
}

kaps_status
kaps_fib_uda_sub_block_copy_callback(
    struct uda_mem_chunk * uda_chunk,
    int32_t to_region_id,
    int32_t to_offset)
{
    kaps_lsn_mc *self = (kaps_lsn_mc *) (uda_chunk->handle);

    if (!self->m_mlpMemInfo)
        kaps_sassert(0);

    KAPS_STRY(kaps_kaps_sbc_commit(uda_chunk, to_region_id, to_offset));

    return KAPS_OK;
}

void
kaps_fib_kaps_update_lsn_size(
    struct kaps_db *db)
{
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) db;
    lpm_db->fib_tbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn += db->device->hw_res->incr_in_bbs;
}




