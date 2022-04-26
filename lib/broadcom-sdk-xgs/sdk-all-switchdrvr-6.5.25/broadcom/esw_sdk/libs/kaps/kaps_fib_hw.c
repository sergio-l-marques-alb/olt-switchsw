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

#include "kaps_fib_hw.h"
#include "kaps_xpt.h"
#include "kaps_utility.h"
#include "kaps_ab.h"
#include "kaps_algo_hw.h"
#include "kaps_fib_lsnmc.h"
#include "kaps_fib_lsnmc_hw.h"
#include "kaps_fib_triedata.h"

void
kaps_write_first_half_of_fmap(
    uint8_t * write_buf,
    uint32_t buf_len_in_bytes,
    uint32_t cur_bit_pos,
    struct kaps_ads *write_ads)
{
    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_00);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_01);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_02);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_03);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_04);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_05);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_06);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_07);
/*  cur_bit_pos += 4;*/
}

void
kaps_write_second_half_of_fmap(
    uint8_t * write_buf,
    uint32_t buf_len_in_bytes,
    uint32_t cur_bit_pos,
    struct kaps_ads *write_ads)
{
    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_08);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_09);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_10);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_11);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_12);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_13);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_14);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_15);
/*  cur_bit_pos += 4;*/
}

static NlmErrNum_t
kaps_jericho_iit_write(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_ab_info *start_ab,
    struct kaps_ads *write_ads,
    uint32_t it_addr,
    NlmReasonCode * o_reason)
{
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_status status = KAPS_OK;
    uint8_t write_buf[16] = { 0 };
    struct kaps_device *device = fibTblMgr->m_devMgr_p;
    uint32_t cur_bit_pos, num_bytes;
    struct kaps_ab_info *cur_dup_ab;
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;

    num_bytes = device->hw_res->ads_width_8;

    KapsWriteBitsInArray(write_buf, num_bytes, 19, 0, write_ads->bpm_ad);
    KapsWriteBitsInArray(write_buf, num_bytes, 27, 20, write_ads->bpm_len);
    KapsWriteBitsInArray(write_buf, num_bytes, 31, 28, write_ads->row_offset);

    cur_bit_pos = 32;

    kaps_write_first_half_of_fmap(write_buf, num_bytes, cur_bit_pos, write_ads);
    cur_bit_pos += 32;

    kaps_write_second_half_of_fmap(write_buf, num_bytes, cur_bit_pos, write_ads);
    cur_bit_pos += 32;

    if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
    {
        KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 11, cur_bit_pos, write_ads->bkt_row);
        cur_bit_pos += 14;
    }
    else if (device->id == KAPS_QUX_DEVICE_ID)
    {
        KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 7, cur_bit_pos, write_ads->bkt_row);
        cur_bit_pos += 8;
    }
    else
    {
        KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 9, cur_bit_pos, write_ads->bkt_row);
        cur_bit_pos += 11;
    }

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 4, cur_bit_pos, write_ads->reserved);
    cur_bit_pos += 5;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 7, cur_bit_pos, write_ads->key_shift);
    cur_bit_pos += 8;

    if (device->id != KAPS_JERICHO_PLUS_DEVICE_ID)
    {
        KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 7, cur_bit_pos, write_ads->ecc);
        cur_bit_pos += 8;
    }

    /*
     * XOR Writes 
     */
    cur_dup_ab = start_ab;
    while (cur_dup_ab)
    {

        status = kaps_dm_kaps_kaps_iit_write(device, db, cur_dup_ab->ab_num, it_addr, num_bytes, write_buf);
        fibStats->numIITWrites++;

        if (status != KAPS_OK)
        {
            *o_reason = NLMRSC_XPT_FAILED;
            return NLMERR_FAIL;
        }

        cur_dup_ab = cur_dup_ab->dup_ab;
    }

    return NLMERR_OK;
}



static NlmErrNum_t
kaps_jericho2_iit_write(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_ab_info *start_ab,
    struct kaps_ads *write_ads,
    uint32_t it_addr,
    NlmReasonCode * o_reason)
{
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_status status = KAPS_OK;
    uint8_t write_buf[16] = { 0 };
    struct kaps_device *device = fibTblMgr->m_devMgr_p;
    uint32_t cur_bit_pos, num_bytes;
    struct kaps_ab_info *cur_dup_ab;
    uint32_t ads_block_num, ads2_block_num;
    struct kaps_lpm_mgr *lpm_mgr = fibTblMgr->m_lpm_mgr;
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;

    if (db->num_algo_levels_in_db == 2)
    {
        num_bytes = device->hw_res->ads_width_8;

        KapsWriteBitsInArray(write_buf, num_bytes, 19, 0, write_ads->bpm_ad);
        KapsWriteBitsInArray(write_buf, num_bytes, 27, 20, write_ads->bpm_len);
        KapsWriteBitsInArray(write_buf, num_bytes, 31, 28, write_ads->row_offset);

        cur_bit_pos = 32;
        kaps_write_first_half_of_fmap(write_buf, num_bytes, cur_bit_pos, write_ads);
        cur_bit_pos += 32;

        kaps_write_second_half_of_fmap(write_buf, num_bytes, cur_bit_pos, write_ads);

        KapsWriteBitsInArray(write_buf, num_bytes, 103, 96, write_ads->key_shift);
        KapsWriteBitsInArray(write_buf, num_bytes, 112, 104, write_ads->bkt_row);

    }
    else
    {
        num_bytes = device->hw_res->ads_width_8;

        KapsWriteBitsInArray(write_buf, num_bytes, 19, 0, write_ads->bpm_ad);
        KapsWriteBitsInArray(write_buf, num_bytes, 27, 20, write_ads->bpm_len);
        KapsWriteBitsInArray(write_buf, num_bytes, 31, 28, write_ads->row_offset);

        cur_bit_pos = 32;
        kaps_write_first_half_of_fmap(write_buf, num_bytes, cur_bit_pos, write_ads);
        cur_bit_pos += 32;

        kaps_write_second_half_of_fmap(write_buf, num_bytes, cur_bit_pos, write_ads);
        cur_bit_pos += 32;

        KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 7, cur_bit_pos, write_ads->key_shift);
        cur_bit_pos += 8;

        KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 13, cur_bit_pos, write_ads->bkt_row);
    }

    if (db->num_algo_levels_in_db == 2)
    {
        /*
         * XOR Writes 
         */
        cur_dup_ab = start_ab;
        while (cur_dup_ab)
        {

            if ((cur_dup_ab->ab_num % 4 == 2) || (cur_dup_ab->ab_num % 4 == 3))
            {
                cur_dup_ab = cur_dup_ab->dup_ab;
                continue;
            }

            ads_block_num = cur_dup_ab->ab_num % 2;
            ads_block_num += 2 * (cur_dup_ab->ab_num / 4);

            status = kaps_dm_kaps_kaps_iit_write(device, db, ads_block_num, it_addr, num_bytes, write_buf);
            fibStats->numIITWrites++;

            if (status != KAPS_OK)
            {
                *o_reason = NLMRSC_XPT_FAILED;
                return NLMERR_FAIL;
            }

            cur_dup_ab = cur_dup_ab->dup_ab;
        }
    }
    else
    {
        uint64_t rptMap = lpm_mgr->resource_db->hw_res.db_res->rpt_map[0];
        uint32_t rptNr;

        for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
        {

            if (rptMap & (1ULL << rptNr))
            {

                ads2_block_num = rptNr % 2;

                status = kaps_dm_kaps_kaps_iit_write(device, db, ads2_block_num, it_addr, num_bytes, write_buf);
                if (status != KAPS_OK)
                {
                    *o_reason = NLMRSC_XPT_FAILED;
                    return NLMERR_FAIL;
                }
                fibStats->numIITWrites++;
                break;
            }
        }
    }

    return NLMERR_OK;
}




NlmErrNum_t
kaps_kaps_iit_write(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_ab_info * start_ab,
    struct kaps_ads * write_ads,
    uint32_t it_addr,
    NlmReasonCode * o_reason)
{
    struct kaps_device *device = fibTblMgr->m_devMgr_p;
    NlmErrNum_t errNum = NLMERR_OK;
    
    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        errNum = kaps_jericho2_iit_write(fibTblMgr, dev_num, start_ab, write_ads, it_addr, o_reason);
    }
    else
    {
        errNum = kaps_jericho_iit_write(fibTblMgr, dev_num, start_ab, write_ads, it_addr, o_reason);
    }

    return errNum;
}

static NlmErrNum_t
kaps_write_trigger_in_small_bb(
    kaps_fib_tbl_mgr * fibMgr_p,
    struct kaps_ab_info *ab,
    uint8_t * pfx_data,
    uint32_t pfx_len_1,
    uint32_t pfx_index,
    NlmReasonCode * o_reason)
{
    struct kaps_device *device = fibMgr_p->m_devMgr_p;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    struct kaps_lpm_mgr *lpm_mgr = fibMgr_p->m_lpm_mgr;
    struct kaps_db *db = fibMgr_p->m_curFibTbl->m_db;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t pfxIndexInBkt, bkt_index;
    uint32_t bkt_nr, row_nr;
    uint32_t start_bkt_for_rpt;
    uint8_t *bktData;
    uint32_t array_index, start_bit_pos;
    uint32_t ad_value;
    kaps_status status;
    uint16_t gran;
    uint32_t rptNr;
    uint32_t small_bb_nr;
    uint64_t rptMap;
    uint32_t virtual_it_addr, actual_it_addr;
    uint32_t found;
    uint32_t issue_write;

    found = 0;
    for (bkt_index = 0; bkt_index < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++bkt_index)
    {
        if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].lower_index <= pfx_index &&
            pfx_index <= device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].upper_index)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_assert(0, "Unable to find the prefix index while writing trigger in small BB\n");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    pfxIndexInBkt = pfx_index - device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].lower_index;

    gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].sub_ab_gran;

    if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].small_bb_num == -1)
    {
        kaps_assert(0, "Writing to invalid Small BB \n");
    }

    bkt_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].small_bb_num;
    row_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].row_num;

    rptMap = lpm_mgr->resource_db->hw_res.db_res->rpt_map[0];

    for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
    {
        if (rptMap & (1ULL << rptNr))
        {

            if (rptNr % 4 == 2 || rptNr % 4 == 3)
                continue;

            start_bkt_for_rpt = 0;
            if (rptNr >= 4)
            {
                start_bkt_for_rpt += 16;
            }

            small_bb_nr = start_bkt_for_rpt + bkt_nr;

            bktData = shadow->small_bbs[small_bb_nr].bkt_rows[row_nr].data;

            array_index = (pfxIndexInBkt * gran) / KAPS_BITS_IN_BYTE;

            kaps_lsn_mc_prepare_mlp_data(pfx_data, pfx_len_1, 0, gran, &bktData[array_index]);

            virtual_it_addr = ab->base_addr + pfx_index;
            actual_it_addr = device->kaps_shadow->ads2_overlay[rptNr % 2].x_table[virtual_it_addr];
            ad_value = actual_it_addr;

            start_bit_pos = (pfxIndexInBkt * KAPS2_BPM_ADS2_LOCATION_SIZE_1);
            KapsWriteBitsInArray(bktData, KAPS_BKT_WIDTH_8, 
                start_bit_pos + KAPS2_BPM_ADS2_LOCATION_SIZE_1 - 1, start_bit_pos, ad_value);


            issue_write = 1;

            if (fibMgr_p->m_bufferKapsABWrites)
            {
                issue_write = 0;
            }

            if (issue_write)
            {
                status = kaps_dm_kaps_bb_write(device, db, small_bb_nr, row_nr, 0, KAPS_BKT_WIDTH_8, bktData);

                if (status != KAPS_OK)
                {
                    *o_reason = NLMRSC_XPT_FAILED;
                    return NLMERR_FAIL;
                }

                fibStats->numIPTWrites++;
            }

        }
    }

    return errNum;
}

static NlmErrNum_t
kaps_delete_trigger_in_small_bb(
    kaps_fib_tbl_mgr * fibMgr_p,
    struct kaps_ab_info *ab,
    uint32_t pfx_index,
    NlmReasonCode * o_reason)
{
    struct kaps_device *device = fibMgr_p->m_devMgr_p;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    struct kaps_lpm_mgr *lpm_mgr = fibMgr_p->m_lpm_mgr;
    struct kaps_db *db = fibMgr_p->m_curFibTbl->m_db;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t pfxIndexInBkt, bkt_index;
    uint32_t bkt_nr, row_nr;
    uint32_t start_bkt_for_rpt;
    uint8_t *bktData;
    uint32_t array_index, num_bytes, start_bit_pos;
    kaps_status status;
    uint16_t gran;
    uint32_t rptNr;
    uint32_t small_bb_nr;
    uint64_t rptMap;
    uint32_t found, issue_write;

    found = 0;
    for (bkt_index = 0; bkt_index < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++bkt_index)
    {
        if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].lower_index <= pfx_index &&
            pfx_index <= device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].upper_index)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_assert(0, "Unable to find the prefix index while deleting trigger in small BB\n");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    pfxIndexInBkt = pfx_index - device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].lower_index;

    gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].sub_ab_gran;

    if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].small_bb_num == -1)
    {
        kaps_assert(0, "Writing to invalid Small BB \n");
    }

    bkt_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].small_bb_num;
    row_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].row_num;

    rptMap = lpm_mgr->resource_db->hw_res.db_res->rpt_map[0];

    for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
    {
        if (rptMap & (1ULL << rptNr))
        {

            if (rptNr % 4 == 2 || rptNr % 4 == 3)
                continue;

            start_bkt_for_rpt = 0;
            if (rptNr >= 4)
            {
                start_bkt_for_rpt += 16;
            }

            small_bb_nr = start_bkt_for_rpt + bkt_nr;

            bktData = shadow->small_bbs[small_bb_nr].bkt_rows[row_nr].data;

            array_index = (pfxIndexInBkt * gran) / KAPS_BITS_IN_BYTE;
            num_bytes = gran / KAPS_BITS_IN_BYTE;
            kaps_memset(&bktData[array_index], 0, num_bytes);

            start_bit_pos = (pfxIndexInBkt * KAPS2_BPM_ADS2_LOCATION_SIZE_1);
            KapsWriteBitsInArray(bktData, KAPS_BKT_WIDTH_8, 
                start_bit_pos + KAPS2_BPM_ADS2_LOCATION_SIZE_1 - 1, start_bit_pos, 0);

            issue_write = 1;

            if (fibMgr_p->m_bufferKapsABWrites)
            {
                issue_write = 0;
            }

            if (issue_write)
            {
                status = kaps_dm_kaps_bb_write(device, db, small_bb_nr, row_nr, 0, KAPS_BKT_WIDTH_8, bktData);

                if (status != KAPS_OK)
                {
                    *o_reason = NLMRSC_XPT_FAILED;
                    return NLMERR_FAIL;
                }

                fibStats->numIPTWrites++;
            }

        }
    }

    return errNum;
}

NlmErrNum_t
kaps_kaps_flush_buffered_small_bb_writes(
    kaps_fib_tbl_mgr * fibMgr_p,
    struct kaps_ab_info * ab,
    NlmReasonCode * o_reason)
{
    struct kaps_device *device = fibMgr_p->m_devMgr_p;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    struct kaps_lpm_mgr *lpm_mgr = fibMgr_p->m_lpm_mgr;
    struct kaps_db *db = fibMgr_p->m_curFibTbl->m_db;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t bkt_index;
    uint32_t bkt_nr, row_nr;
    uint32_t start_bkt_for_rpt;
    uint8_t *bktData;
    kaps_status status;
    uint32_t rptNr;
    uint32_t small_bb_nr;
    uint64_t rptMap;

    for (bkt_index = 0; bkt_index < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++bkt_index)
    {

        bkt_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].small_bb_num;
        row_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].row_num;

        rptMap = lpm_mgr->resource_db->hw_res.db_res->rpt_map[0];

        for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
        {
            if (rptMap & (1ULL << rptNr))
            {

                if (rptNr % 4 == 2 || rptNr % 4 == 3)
                    continue;

                start_bkt_for_rpt = 0;
                if (rptNr >= 4)
                {
                    start_bkt_for_rpt += 16;
                }

                small_bb_nr = start_bkt_for_rpt + bkt_nr;

                bktData = shadow->small_bbs[small_bb_nr].bkt_rows[row_nr].data;

                status = kaps_dm_kaps_bb_write(device, db, small_bb_nr, row_nr, 0, KAPS_BKT_WIDTH_8, bktData);

                if (status != KAPS_OK)
                {
                    *o_reason = NLMRSC_XPT_FAILED;
                    return NLMERR_FAIL;
                }

                fibStats->numIPTWrites++;

            }
        }
    }

    return errNum;
}

static NlmErrNum_t
kaps_write_to_tcam(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_ab_info *start_blk,
    uint32_t logicalLoc,
    uint8_t rptId,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t byte_offset,
    NlmReasonCode * o_reason)
{
    kaps_status status = KAPS_OK;
    uint8_t ab_data[KAPS_RPB_WIDTH_8];
    uint8_t ab_mask[KAPS_RPB_WIDTH_8];
    uint32_t num_bytes;
    struct kaps_ab_info *cur_dup_ab;
    struct kaps_device *device = fibTblMgr->m_devMgr_p;

    kaps_sassert(blkWidthInBits == KAPS_RPB_WIDTH_1);

    num_bytes = (dataLen + 7) / 8;
    kaps_memcpy(ab_data, data, num_bytes);

    if (num_bytes < KAPS_RPB_WIDTH_8)
        kaps_memset(&ab_data[num_bytes], 0, KAPS_RPB_WIDTH_8 - num_bytes);

    kaps_memset(ab_mask, 0xFF, KAPS_RPB_WIDTH_8);
    if (dataLen > 0)
        kaps_FillZeroes(ab_mask, KAPS_RPB_WIDTH_8, startBit, startBit - dataLen + 1);

    if (device->kaps_shadow && device->nv_ptr)
    {
        device->kaps_shadow->rpb_blks[start_blk->ab_num].rpb_rows[logicalLoc].rpb_tbl_id =
            fibTblMgr->m_curFibTbl->m_tblId;
    }

    cur_dup_ab = start_blk;
    while (cur_dup_ab)
    {
        if (device->kaps_shadow && device->nv_ptr)
        {
            device->kaps_shadow->rpb_blks[cur_dup_ab->ab_num].rpb_rows[logicalLoc].rpb_tbl_id =
                fibTblMgr->m_curFibTbl->m_tblId;
        }
        
        status = kaps_dm_kaps_rpb_write(device, cur_dup_ab->ab_num, logicalLoc, ab_data, ab_mask, 3);

        if (status != KAPS_OK)
        {
            *o_reason = NLMRSC_XPT_FAILED;
            return NLMERR_FAIL;
        }

        cur_dup_ab = cur_dup_ab->dup_ab;
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_delete_in_tcam(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_ab_info *start_blk,
    uint32_t logicalLoc,
    NlmReasonCode * o_reason)
{
    kaps_status status = KAPS_OK;
    struct kaps_dba_entry entry;
    struct kaps_ab_info *cur_dup_ab;

    kaps_sassert(blkWidthInBits == KAPS_RPB_WIDTH_1);

    /*
     * XOR Writes 
     */
    cur_dup_ab = start_blk;
    while (cur_dup_ab)
    {

        status =
            kaps_dm_kaps_rpb_write(fibTblMgr->m_devMgr_p, cur_dup_ab->ab_num, logicalLoc, (uint8_t *) & entry,
                                   (uint8_t *) & entry, 0);

        if (status != KAPS_OK)
        {
            *o_reason = NLMRSC_XPT_FAILED;
            return NLMERR_FAIL;
        }

        cur_dup_ab = cur_dup_ab->dup_ab;
    }
    return NLMERR_OK;
}

NlmErrNum_t
kaps_kaps_read_trigger_hb_in_small_bb(
    kaps_fib_tbl_mgr * fibMgr_p,
    struct kaps_ab_info * ab,
    uint32_t pfx_index,
    uint8_t * hb_value,
    NlmReasonCode * o_reason)
{
    struct kaps_device *device = fibMgr_p->m_devMgr_p;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    struct kaps_lpm_mgr *lpm_mgr = fibMgr_p->m_lpm_mgr;
    struct kaps_db *db = fibMgr_p->m_curFibTbl->m_db;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t pfxIndexInBkt, bkt_index;
    uint32_t bkt_nr, row_nr;
    kaps_status status;
    uint16_t gran;
    uint32_t small_bb_nr;
    uint8_t data[KAPS_HB_ROW_WIDTH_8] = { 0 };
    uint32_t found;

    (void) lpm_mgr;
    (void) gran;
    *hb_value = 0;

    found = 0;
    for (bkt_index = 0; bkt_index < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++bkt_index)
    {
        if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].lower_index <= pfx_index &&
            pfx_index <= device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].upper_index)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_assert(0, "Unable to find the prefix index while reading trigger hit bits in small BB\n");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    pfxIndexInBkt = pfx_index - device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].lower_index;

    gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].sub_ab_gran;

    bkt_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].small_bb_num;
    row_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].row_num;

    small_bb_nr = device->small_bb_offset + bkt_nr;

    /*
     * The function kaps_dm_kaps_hb_read will take care of reading from cloned small bb's. 
     */
    status = kaps_dm_kaps_hb_read(device, db, small_bb_nr, row_nr, data);

    if (status != KAPS_OK)
    {
        *o_reason = NLMRSC_XPT_FAILED;
        return NLMERR_FAIL;
    }

    fibStats->numHitBitReads++;

    *hb_value = KapsReadBitsInArrray(data, KAPS_HB_ROW_WIDTH_8, pfxIndexInBkt, pfxIndexInBkt);

    return errNum;
}

NlmErrNum_t
kaps_kaps_write_trigger_hb_in_small_bb(
    kaps_fib_tbl_mgr * fibMgr_p,
    struct kaps_ab_info * ab,
    uint32_t pfx_index,
    uint8_t hb_value,
    NlmReasonCode * o_reason)
{
    struct kaps_device *device = fibMgr_p->m_devMgr_p;
    struct NlmFibStats *fibStats = &fibMgr_p->m_curFibTbl->m_fibStats;
    struct kaps_lpm_mgr *lpm_mgr = fibMgr_p->m_lpm_mgr;
    struct kaps_db *db = fibMgr_p->m_curFibTbl->m_db;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t pfxIndexInBkt, bkt_index;
    uint32_t bkt_nr, row_nr;
    kaps_status status;
    uint16_t gran;
    uint32_t small_bb_nr;
    uint8_t data[KAPS_HB_ROW_WIDTH_8] = { 0 };
    uint32_t found;

    (void) lpm_mgr;
    (void) gran;
    found = 0;
    for (bkt_index = 0; bkt_index < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++bkt_index)
    {
        if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].lower_index <= pfx_index &&
            pfx_index <= device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].upper_index)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_assert(0, "Unable to find the prefix index while writing trigger hit bits in small BB\n");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    pfxIndexInBkt = pfx_index - device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].lower_index;

    gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].sub_ab_gran;

    bkt_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].small_bb_num;
    row_nr = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[bkt_index].row_num;

    small_bb_nr = device->small_bb_offset + bkt_nr;

    /*
     * The function kaps_dm_kaps_hb_read will take care of reading from cloned small bb's. 
     */
    status = kaps_dm_kaps_hb_read(device, db, small_bb_nr, row_nr, data);

    if (status != KAPS_OK)
    {
        *o_reason = NLMRSC_XPT_FAILED;
        return NLMERR_FAIL;
    }

    fibStats->numHitBitReads++;

    KapsWriteBitsInArray(data, KAPS_HB_ROW_WIDTH_8, pfxIndexInBkt, pfxIndexInBkt, hb_value);

    /*
     * The function kaps_dm_kaps_hb_write will take care of writing to cloned small bb's. 
     */
    status = kaps_dm_kaps_hb_write(device, db, small_bb_nr, row_nr, data);

    if (status != KAPS_OK)
    {
        *o_reason = NLMRSC_XPT_FAILED;
        return NLMERR_FAIL;
    }

    return errNum;
}

NlmErrNum_t
kaps_kaps_write_ab_data(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_ab_info * start_blk,
    uint32_t logicalLoc,
    uint8_t rptId,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t byte_offset,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;

    if (db->num_algo_levels_in_db == 3)
    {
        errNum = kaps_write_trigger_in_small_bb(fibTblMgr, start_blk, data, dataLen, logicalLoc, o_reason);
    }
    else
    {
        errNum = kaps_write_to_tcam(fibTblMgr, devNum, blkWidthInBits, start_blk, logicalLoc, rptId, startBit,
                                    endBit, data, dataLen, byte_offset, o_reason);
    }

    return errNum;
}

NlmErrNum_t
kaps_kaps_delete_ab_data(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_ab_info * start_blk,
    uint32_t logicalLoc,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;

    if (db->num_algo_levels_in_db == 3)
    {
        errNum = kaps_delete_trigger_in_small_bb(fibTblMgr, start_blk, logicalLoc, o_reason);
    }
    else
    {
        errNum = kaps_delete_in_tcam(fibTblMgr, devNum, blkWidthInBits, start_blk, logicalLoc, o_reason);
    }

    return errNum;
}

NlmErrNum_t
kaps_fib_rpt_write(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    kaps_dev_rpt_entry * entry, /* in X/Y format */
    int32_t isDeleteOper,
    uint16_t addr,              /* 0 to 1023 are valid values */
    NlmReasonCode * o_reason_p)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_status status;
    struct kaps_device *device = fibTblMgr->m_devMgr_p;
    struct NlmFibStats *fibStats = NULL;
    struct kaps_lpm_mgr *lpm_mgr = fibTblMgr->m_lpm_mgr;
    struct kaps_db *res_db = NULL;
    uint64_t rptMap;
    uint32_t rptNr;

    {
        res_db = lpm_mgr->resource_db;
        rptMap = res_db->hw_res.db_res->rpt_map[0];
        if (res_db->common_info->is_cascaded)
        {
            rptMap = res_db->hw_res.db_res->rpt_map[device->core_id];
        }
    }

    if (isDeleteOper == 1)
    {
        for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
        {
            if (rptMap & (1ULL << rptNr))
            {
                status = kaps_algo_hw_delete_rpt_data(fibTblMgr->m_devMgr_p, rptNr, addr);

                if (status != KAPS_OK)
                {
                    *o_reason_p = NLMRSC_INTERNAL_ERROR;
                    return NLMERR_FAIL;
                }
            }
        }

        return NLMERR_OK;
    }

    for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
    {
        if (rptMap & (1ULL << rptNr))
        {
            status = kaps_algo_hw_write_rpt_data(device, rptNr, entry->m_data, entry->m_mask, addr );

            if (status != KAPS_OK)
            {
                *o_reason_p = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }
        }
    }

    if (fibTblMgr->m_curFibTbl)
    {
        fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
        fibStats->numRPTWrites++;
        if (device->type != KAPS_DEVICE_KAPS)
        {
            fibStats->numPIOWrites++;
        }
    }

    return errNum;
}

NlmErrNum_t
kaps_fib_rit_write(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_pct * entry,
    uint16_t addr,              /* 0 to 1023 are valid values */
    NlmReasonCode * o_reason_p)
{
    kaps_status status;
    struct NlmFibStats *fibStats = NULL;
    struct kaps_lpm_mgr *lpm_mgr = fibTblMgr->m_lpm_mgr;
    struct kaps_db *res_db = NULL;
    uint64_t rptMap;
    uint32_t rptNr, i, pct_block_num;
    struct kaps_device *device = fibTblMgr->m_devMgr_p;
    NlmErrNum_t errNum = NLMERR_OK;

    if (device->issu_in_progress)
        return NLMERR_OK;

    {
        res_db = lpm_mgr->resource_db;
        rptMap = res_db->hw_res.db_res->rpt_map[0];
        if (res_db->common_info->is_cascaded)
        {
            rptMap = res_db->hw_res.db_res->rpt_map[device->core_id];
        }
    }

    i = 0;

    for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
    {
        if (rptMap & (1ULL << rptNr))
        {

            if (device->type == KAPS_DEVICE_KAPS && (rptNr % 4 == 2 || rptNr % 4 == 3))
                continue;

            pct_block_num = rptNr;
            if (device->type == KAPS_DEVICE_KAPS)
            {
                pct_block_num = (rptNr % 2);
                pct_block_num += 2 * (rptNr / 4);
            }

            status = kaps_algo_hw_write_pct(fibTblMgr->m_devMgr_p, &entry[i], pct_block_num, addr);

            if (status != KAPS_OK)
            {
                *o_reason_p = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }

            if (device->type != KAPS_DEVICE_KAPS)
                ++i;
        }
    }

    fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    fibStats->numRITWrites++;
    if (device->type != KAPS_DEVICE_KAPS)
    {
        fibStats->numPIOWrites++;
    }

    if (device->type == KAPS_DEVICE_KAPS)
    {
        if (entry[0].u.kaps.bpm_len)
        {
            errNum = kaps_kaps_iit_write(fibTblMgr, 0, NULL, &entry[1].u.kaps, entry[0].u.kaps.bpm_ad, o_reason_p);
        }

    }

    return errNum;
}
