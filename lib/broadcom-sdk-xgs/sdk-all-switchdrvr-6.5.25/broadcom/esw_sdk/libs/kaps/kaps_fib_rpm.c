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

#include "kaps_fib_rpm.h"
#include "kaps_fib_cmn_pfxbundle.h"

#include "kaps_fib_trienode.h"
#include "kaps_fib_lsnmc.h"
#include "kaps_fib_hw.h"
#include "kaps_ad_internal.h"
#include "kaps_fib_lsnmc_hw.h"

static uint8_t
kaps_get_correct_id(
    kaps_rpm * rpm_p,
    uint8_t dbId)
{
    uint8_t correct_id = dbId;

    return correct_id;
}

static void
kaps_fill_ads1_format(
    struct kaps_pct *rit,
    uint32_t i,
    uint32_t format_value)
{
    switch (i)
    {
        case 0:
            rit->u.kaps.format_map_00 = format_value;
            break;

        case 1:
            rit->u.kaps.format_map_01 = format_value;
            break;

        case 2:
            rit->u.kaps.format_map_02 = format_value;
            break;

        case 3:
            rit->u.kaps.format_map_03 = format_value;
            break;

        case 4:
            rit->u.kaps.format_map_04 = format_value;
            break;

        case 5:
            rit->u.kaps.format_map_05 = format_value;
            break;

        case 6:
            rit->u.kaps.format_map_06 = format_value;
            break;

        case 7:
            rit->u.kaps.format_map_07 = format_value;
            break;

        case 8:
            rit->u.kaps.format_map_08 = format_value;
            break;

        case 9:
            rit->u.kaps.format_map_09 = format_value;
            break;

        case 10:
            rit->u.kaps.format_map_10 = format_value;
            break;

        case 11:
            rit->u.kaps.format_map_11 = format_value;
            break;

        case 12:
            rit->u.kaps.format_map_12 = format_value;
            break;

        case 13:
            rit->u.kaps.format_map_13 = format_value;
            break;

        case 14:
            rit->u.kaps.format_map_14 = format_value;
            break;

        case 15:
            rit->u.kaps.format_map_15 = format_value;
            break;

        default:
            kaps_assert(0, "Incorrect format map\n");
            break;
    }
}

static NlmErrNum_t
kaps_fill_rit(
    kaps_trie_node * rptNode_p,
    struct kaps_pct *rit)
{
    uint32_t i;
    kaps_pool_mgr *poolMgr = rptNode_p->m_trie_p->m_trie_global->poolMgr;
    struct kaps_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    kaps_ipm *ipm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, rptNode_p->m_poolId);
    kaps_lsn_mc_settings *settings = rptNode_p->m_trie_p->m_lsn_settings_p;
    struct kaps_db *db = rptNode_p->m_trie_p->m_tbl_ptr->m_db;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_ads *iitForPct;
    struct kaps_ab_info *ab = ipm_p->m_ab_info;
    uint32_t cur_small_bb_num, cur_row_num;
    uint32_t gran;
    uint32_t ads2_num, ads2_depth;
    uint32_t gran_ix, format_value;
    uint32_t db_group_id = db->db_group_id;

    (void) cur_row_num;
    (void) ad_db;
        
    if (db->parent)
        db = db->parent;

    ads2_num = db->rpb_id;

    ads2_depth = device->hw_res->ads2_depth[ads2_num];
    

    kaps_memset(&rit->u.kaps, 0, sizeof(rit->u.kaps));

    iitForPct = &rit[1].u.kaps;
    kaps_memset(iitForPct, 0, sizeof(*iitForPct));

    if (rptNode_p->m_iitLmpsofarPfx_p)
    {
        struct kaps_lpm_entry *entry = rptNode_p->m_iitLmpsofarPfx_p->m_backPtr;
        uint8_t *ad_value = entry->ad_handle->value;
        int32_t virtual_it_addr, actual_it_addr;

        iitForPct->bpm_len = rptNode_p->m_iitLmpsofarPfx_p->m_nPfxSize;

        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device,
            rptNode_p->m_iitLmpsofarPfx_p->m_backPtr->ad_handle, ad_db);

        
        {
            for (i = 0; i < ad_db->user_width_1 / KAPS_BITS_IN_BYTE; ++i)
            {
                iitForPct->bpm_ad = (iitForPct->bpm_ad << KAPS_BITS_IN_BYTE) | ad_value[i];
            }

            if (ad_db->user_width_1 == 20)
                iitForPct->bpm_ad = (iitForPct->bpm_ad << 4) | (ad_value[i] >> 4);


            
        }

        rit->u.kaps.bpm_len = rptNode_p->m_iitLmpsofarPfx_p->m_nPfxSize;

        /*
         * For the RIT (ADS-1) BPM AD, we have to program the location in the UIT (ADS-2) where the RPT Lmpsofar is
         * stored. 
         */
        virtual_it_addr = device->hw_res->ads2_rpt_lmpsofar_virtual_ix_start + rptNode_p->m_rpt_prefix_p->m_nIndex;

        actual_it_addr = device->kaps_shadow->ads2_overlay[ads2_num].x_table[virtual_it_addr];

        if (actual_it_addr == -1)
        {
            actual_it_addr = kaps_find_first_bit_set(&device->kaps_shadow->ads2_overlay[ads2_num].it_fbmp,
                                                     0, ads2_depth - 1);

            kaps_assert(0 <= actual_it_addr && actual_it_addr < ads2_depth, "Out of KAPS ADS-2 IT\n");

            device->kaps_shadow->ads2_overlay[ads2_num].x_table[virtual_it_addr] = actual_it_addr;

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

        rit->u.kaps.bpm_ad = actual_it_addr;
    }

    rit->u.kaps.bkt_row = device->kaps_shadow->ab_to_small_bb[ipm_p->m_ab_info->ab_num].sub_ab_bricks[0].row_num;
    rit->u.kaps.key_shift = kaps_trie_get_num_rpt_bits_lopped_off(device, rptNode_p->m_depth);

    rit->u.kaps.row_offset =
        device->kaps_shadow->ab_to_small_bb[ipm_p->m_ab_info->ab_num].sub_ab_bricks[0].small_bb_num;
    if (rit->u.kaps.bkt_row == device->hw_res->num_rows_in_small_bb[db_group_id] - 1)
    {
        rit->u.kaps.row_offset = 0;
    }

    i = 0;
    while (i < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks)
    {
        cur_small_bb_num = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[i].small_bb_num;
        cur_row_num = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[i].row_num;

        gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[i].sub_ab_gran;

        gran_ix = settings->m_middleLevelLengthToGranIx[gran];

        format_value = settings->m_middleLevelGranIxToFormatValue[gran_ix];

        kaps_fill_ads1_format(rit, cur_small_bb_num, format_value);

        ++i;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_rpm_construct_rit(
    kaps_trie_node * rptNode_p,
    struct kaps_pct * rit)
{
    kaps_trie_global *trieGlobal_p = rptNode_p->m_trie_p->m_trie_global;
    kaps_fib_tbl_mgr *fibTblMgr_p = trieGlobal_p->fibtblmgr;
    struct kaps_device *device = fibTblMgr_p->m_devMgr_p;

    if (device->issu_in_progress)
        return NLMERR_OK;

    if (device->type == KAPS_DEVICE_KAPS)
    {
        kaps_fill_rit(rptNode_p, rit);
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_rpm_pvt_recalc_info(
    kaps_rpm * rpm_p,
    uint32_t row_nr)
{
    uint32_t i, found;
    int32_t group_id = -1;
    kaps_fib_tbl_mgr *fibTblMgr_p = rpm_p->m_fib_tbl_mgr;
    kaps_fib_tbl_list *head, *node;
    kaps_fib_tbl *fibTbl;

    for (i = 0; i < MAX_NUM_RPT_ENTRIES_IN_POOL; ++i)
    {
        if (!rpm_p->kaps_simple_dba.nodes[i].chunks)
        {
            continue;
        }

        if (kaps_get_bit(&rpm_p->kaps_simple_dba.nodes[i].used, row_nr))
        {
            group_id = i;
            break;
        }
    }

    if (group_id == -1)
        return NLMERR_FAIL;

    rpm_p->m_curDbId = group_id;

    head = fibTblMgr_p->m_fibTblList_p;
    node = head->m_next_p;
    found = 0;
    while (node != head)
    {
        fibTbl = node->m_fibTbl_p;

        if (fibTbl->m_tblId == group_id)
        {
            found = 1;
            break;
        }

        node = node->m_next_p;
    }

    if (!found)
        return NLMERR_FAIL;

    return NLMERR_OK;
}

void
kaps_rpm_rpt_shuffle_callback(
    void *rpm_p,
    void *bundle_p,
    uint32_t newRptLocation,
    uint32_t length)
{
    kaps_pfx_bundle *pfxBundle_p = (kaps_pfx_bundle *) bundle_p;
    uint32_t oldRptLocation = pfxBundle_p->m_nIndex;
    NlmReasonCode reason;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_rpm *self_p = (kaps_rpm *) rpm_p;
    struct kaps_pct ritEntry[MAX_COPIES_PER_LPM_DB];
    uint8_t *tempPtr;
    kaps_trie_node *rptNode_p = NULL;
    kaps_fib_tbl_mgr *fibTblMgr = self_p->m_fib_tbl_mgr;
    int32_t isDirShiftUp;
    struct kaps_device *device = fibTblMgr->m_devMgr_p;
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;
    kaps_lsn_mc_settings *settings;
    kaps_status status;

    (void) length;

    if (oldRptLocation == newRptLocation)
    {
        /*
         * This can happen when we need to rewrite an RPT location due to a parity error returned from hardware The
         * dbId and whether extra byte has been added need to be determined correctly. 
         */
        errNum = kaps_rpm_pvt_recalc_info(rpm_p, newRptLocation);

        if (errNum != NLMERR_OK)
            return;
    }

    isDirShiftUp = 1;
    if (oldRptLocation < newRptLocation)
    {
        isDirShiftUp = 0;
    }

    pfxBundle_p->m_nIndex = newRptLocation;

    {

        tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle_p);
        kaps_memcpy(&rptNode_p, tempPtr, sizeof(kaps_trie_node *));

        errNum = kaps_rpm_construct_rit(rptNode_p, ritEntry);
        if (errNum != NLMERR_OK)
            kaps_assert(0, "Error while constructing RIT in RPT callback \n");

        if (isDirShiftUp)
        {
            errNum = kaps_fib_rit_write(fibTblMgr, 0, ritEntry, pfxBundle_p->m_nIndex, &reason);
            if (errNum != NLMERR_OK)
                kaps_assert(0, "Error writing RIT entry in RIT callback \n");
        }

        errNum = kaps_rpm_write_entry_to_hw(self_p, pfxBundle_p, self_p->m_curDbId);
        if (errNum != NLMERR_OK)
            kaps_assert(0, "Error while writing entry to hardware in RPT callback \n");

        if (!isDirShiftUp)
        {
            errNum = kaps_fib_rit_write(fibTblMgr, 0, ritEntry, pfxBundle_p->m_nIndex, &reason);
            if (errNum != NLMERR_OK)
                kaps_assert(0, "Error writing RIT entry in RIT callback \n");
        }
    }

    /*
     * delete the old location
     */
    if (oldRptLocation != newRptLocation)
    {
        errNum = kaps_fib_rpt_write(fibTblMgr, 0, NULL, 1, oldRptLocation, &reason);

        if (errNum != NLMERR_OK)
            kaps_assert(0, "Error while deleting the old RPT location during a shuffle\n");
    }


    {
        settings = rptNode_p->m_trie_p->m_lsn_settings_p;

        if (settings->m_areRPTHitBitsPresent && rptNode_p->m_iitLmpsofarPfx_p
            && rptNode_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
        {

            struct kaps_lpm_mgr *lpm_mgr = fibTblMgr->m_lpm_mgr;
            uint64_t rptMap;
            uint32_t rptNr;
            uint8_t hb_data[KAPS_HB_ROW_WIDTH_8];

            rptMap = lpm_mgr->resource_db->hw_res.db_res->rpt_map[0];

            for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
            {
                if (rptMap & (1ULL << rptNr))
                {

                    status = kaps_dm_kaps_hb_read(device, db, device->dba_offset + rptNr, oldRptLocation, hb_data);
                    if (status != KAPS_OK)
                    	return;

                    status = kaps_dm_kaps_hb_write(device, db, device->dba_offset + rptNr, newRptLocation, hb_data);
                    if (status != KAPS_OK)
                    	return;
                }
            }
        }
    }
}


kaps_rpm *
kaps_rpm_init(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t is_cascaded)
{
    struct kaps_device *device = fibTblMgr->m_devMgr_p;
    kaps_rpm *result = NULL;
    struct kaps_lpm_mgr *lpm_mgr = fibTblMgr->m_lpm_mgr;
    struct kaps_db *res_db;
    uint64_t rptMap;

    if (is_cascaded)
    {
        device = device->other_core;
    }

    {
        res_db = lpm_mgr->resource_db;
        rptMap = res_db->hw_res.db_res->rpt_map[is_cascaded];
    }

    {
        int32_t pcm_num;

        for (pcm_num = 0; pcm_num < HW_MAX_PCM_BLOCKS; pcm_num++)
        {
            if (rptMap & (1ULL << pcm_num))
            {
                if (device->hw_res->pcm_dba_mgr[pcm_num] == NULL)
                    device->hw_res->pcm_dba_mgr[pcm_num]
                        = kaps_simple_dba_init(device, 0, device->map->rpb_info[pcm_num].rpt_depth, pcm_num, 0);
                if (!result)
                    result = (kaps_rpm *) device->hw_res->pcm_dba_mgr[pcm_num];
            }
        }
    }

    if (result == NULL)
        return result;

    result->m_fib_tbl_mgr = fibTblMgr;

    return result;
}

NlmErrNum_t
kaps_rpm_add_entry(
    kaps_rpm * rpm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint8_t db_id,
    NlmReasonCode * o_reason)
{
    kaps_status status;

    rpm_p->m_curDbId = kaps_get_correct_id(rpm_p, db_id);

    /*
     * RPM internally stores the entries from location 0 so we need to map it to device RPT entry by adding the RPT
     * Start Location/Offset 
     */
    status = kaps_simple_dba_find_place_for_entry(&rpm_p->kaps_simple_dba, pfxBundle_p,
                                                  rpm_p->m_curDbId,
                                                  pfxBundle_p->m_nPfxSize, (int32_t *) & pfxBundle_p->m_nIndex);

    if (status != KAPS_OK)
    {
        if (status == KAPS_OUT_OF_MEMORY)
            *o_reason = NLMRSC_LOW_MEMORY;

        return NLMERR_FAIL;
    }

    if (pfxBundle_p->m_nIndex == KAPS_LSN_NEW_INDEX)
    {
        kaps_assert(0, "Unable to insert RPT entry in the pool \n");
        return NLMERR_FAIL;
    }
    /*
     * Add the start loc to get the final index in RPT 
     */
    pfxBundle_p->m_nIndex += rpm_p->kaps_simple_dba.pool_start_loc;
    return NLMERR_OK;
}

NlmErrNum_t
kaps_rpm_wb_add_entry(
    kaps_rpm * rpm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint32_t length,
    uint32_t pos,
    uint8_t dbId)
{
    pfxBundle_p->m_nIndex = pos;

    rpm_p->m_curDbId = kaps_get_correct_id(rpm_p, dbId);

    kaps_simple_dba_place_entry_at_loc(&rpm_p->kaps_simple_dba, pfxBundle_p, rpm_p->m_curDbId,
                                       length, pos - rpm_p->kaps_simple_dba.pool_start_loc);
    /*
     * Place entry at the specified position 
     */

    return NLMERR_OK;
}

NlmErrNum_t
kaps_rpm_write_entry_to_hw(
    kaps_rpm * rpm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint8_t dbId)
{
    kaps_fib_tbl_mgr *fibTblMgr_p = rpm_p->m_fib_tbl_mgr;
    NlmReasonCode reason;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_dev_rpt_entry rptEntry;
    uint32_t numBytes = pfxBundle_p->m_nPfxSize / 8;
    uint32_t numBitsInLastByte = pfxBundle_p->m_nPfxSize % 8;
    uint32_t rptStartPos, i;
    uint8_t *pfxData, lastByteMask = 0;

    pfxData = pfxBundle_p->m_data;

    kaps_memset(rptEntry.m_data, 0, sizeof(rptEntry.m_data));
    kaps_memset(rptEntry.m_mask, 0xFF, sizeof(rptEntry.m_mask));

    rptStartPos = 0;


    rpm_p->m_curDbId = kaps_get_correct_id(rpm_p, dbId);

    /*
     * Directly copy the prefix data into the RPT entry from bit 159 onwards 
     */
    if (numBytes)
        kaps_memcpy(&rptEntry.m_data[rptStartPos], pfxData, numBytes);

    for (i = 0; i < numBytes; ++i)
    {
        rptEntry.m_mask[rptStartPos + i] = 0;
    }

    if (numBytes <  KAPS_RPB_WIDTH_8 && numBitsInLastByte > 0)
    {
        lastByteMask = (uint8_t) (0xFF >> numBitsInLastByte);
        /*
         * coverity[overrun-local] 
         */
        rptEntry.m_data[rptStartPos + numBytes] = pfxData[numBytes] & ~lastByteMask;
        rptEntry.m_mask[rptStartPos + numBytes] = lastByteMask;
    }

    errNum = kaps_fib_rpt_write(fibTblMgr_p, 0, &rptEntry, 0, (uint16_t) pfxBundle_p->m_nIndex, &reason);

    if (errNum != NLMERR_OK)
        kaps_assert(0, "Error while writing RPT entry\n");

    return errNum;
}

NlmErrNum_t
kaps_rpm_remove_entry(
    kaps_rpm * rpm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint8_t dbId)
{
    /*
     * RPM internally considers start of pool from 0 so pass the actual position by subtracting offset/start loc 
     */
    uint32_t pos_in_rpm = pfxBundle_p->m_nIndex - rpm_p->kaps_simple_dba.pool_start_loc;

    rpm_p->m_curDbId = kaps_get_correct_id(rpm_p, dbId);

    kaps_simple_dba_free_entry(&rpm_p->kaps_simple_dba, rpm_p->m_curDbId, pfxBundle_p->m_nPfxSize, pos_in_rpm);

    pfxBundle_p->m_nIndex = KAPS_LSN_NEW_INDEX;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_rpm_delete_entry_in_hw(
    kaps_rpm * rpm_p,
    kaps_pfx_bundle * pfxBundle_p)
{
    NlmReasonCode reason;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_fib_tbl_mgr *fibTblMgr_p = rpm_p->m_fib_tbl_mgr;

    errNum = kaps_fib_rpt_write(fibTblMgr_p, 0, NULL, 1, (uint16_t) pfxBundle_p->m_nIndex, &reason);

    if (errNum != NLMERR_OK)
        kaps_assert(0, "Error while writing RPT entry\n");

    return errNum;
}

NlmErrNum_t
kaps_rpm_remove_default_entry(
    kaps_rpm * rpm_p,
    uint8_t dbId,
    kaps_pfx_bundle * pfxBundle_p)
{
    NlmReasonCode reason;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t pos_in_rpm;
    kaps_fib_tbl_mgr *fibTblMgr_p = rpm_p->m_fib_tbl_mgr;

    /*
     * RPM internally considers start of pool from 0 so pass the actual position by subtracting offset/start loc 
     */
    pos_in_rpm = pfxBundle_p->m_nIndex - rpm_p->kaps_simple_dba.pool_start_loc;

    rpm_p->m_curDbId = kaps_get_correct_id(rpm_p, dbId);

    kaps_simple_dba_free_entry(&rpm_p->kaps_simple_dba, rpm_p->m_curDbId, pfxBundle_p->m_nPfxSize, pos_in_rpm);

    errNum = kaps_fib_rpt_write(fibTblMgr_p, 0, NULL, 1, (uint16_t) pfxBundle_p->m_nIndex, &reason);

    kaps_nlm_allocator_free(fibTblMgr_p->m_alloc_p, pfxBundle_p);

    return errNum;
}

NlmErrNum_t
kaps_rpm_init_db(
    kaps_rpm * rpm_p,
    uint8_t dbId,
    NlmReasonCode * o_reason)
{
    kaps_status status;

    dbId = kaps_get_correct_id(rpm_p, dbId);

    status = kaps_kaps_simple_dba_init_node(&rpm_p->kaps_simple_dba, dbId, kaps_rpm_rpt_shuffle_callback);

    if (status != KAPS_OK)
    {
        if (status == KAPS_OUT_OF_MEMORY)
            *o_reason = NLMRSC_LOW_MEMORY;

        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_rpm_free_db(
    kaps_rpm * rpm_p,
    uint8_t dbId)
{
    dbId = kaps_get_correct_id(rpm_p, dbId);

    kaps_simple_dba_free_node(&rpm_p->kaps_simple_dba, dbId);

    return NLMERR_OK;
}

void
kaps_rpm_verify(
    kaps_rpm * rpm_p,
    uint8_t db_id)
{
    kaps_simple_dba_verify(&rpm_p->kaps_simple_dba, db_id, kaps_get_pfx_bundle_prio_length);
}

uint32_t
kaps_rpm_get_num_free_slots(
    kaps_rpm * rpm_p)
{
    return rpm_p->kaps_simple_dba.free_entries;
}
