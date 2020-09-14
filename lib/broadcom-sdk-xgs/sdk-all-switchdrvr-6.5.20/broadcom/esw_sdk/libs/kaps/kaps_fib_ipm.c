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

#include "kaps_fib_ipm.h"
#include "kaps_fib_cmn_pfxbundle.h"

#include "kaps_fib_trienode.h"
#include "kaps_fib_lsnmc.h"
#include "kaps_fib_lsnmc_hw.h"
#include "kaps_fib_hw.h"

#include "kaps_device_internal.h"

static uint32_t
kaps_ab_get_blk_width(
    struct kaps_ab_info *ab)
{
    uint32_t blk_width_1 = 80;

    switch (ab->conf)
    {
        case KAPS_NORMAL_80:
            blk_width_1 = 80;
            break;
        case KAPS_NORMAL_160:
            blk_width_1 = 160;
            break;
        default:
            kaps_sassert(0);
            break;
    }

    return blk_width_1;
}

int32_t
kaps_get_pfx_bundle_prio_length(
    void *pfxBundle)
{
    kaps_pfx_bundle *pfxBundle_p = (kaps_pfx_bundle *) pfxBundle;
    return pfxBundle_p->m_nPfxSize;
}

uint32_t
kaps_ipm_recompute_len_in_simple_dba(
    kaps_ipm * ipm,
    uint32_t length)
{
    kaps_fib_tbl_mgr *fibTblMgr = ipm->m_fib_tbl_mgr;
    uint32_t recomputed_len = length;
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;

    /*
     * For KAPS-2 Large, the middle level is mapped to Small BBs. Here the Small BBs are mapped into ABs. In small BBs, 
     * there is no need to maintain entries in the sorted order of their lengths. So there is no need of shuffling of
     * entries needed within the small BBs. To achieve this, we are always passing the length of the entries being
     * passed to kaps_simple_dba as 168 for KAPS-2. So from the point of view of kaps_simple_dba, all entries are of
     * the same length. So kaps_simple_dba will not shuffle any entries
     */
    if (db->num_algo_levels_in_db == 3)
    {
        recomputed_len = 168;
    }

    return recomputed_len;
}

void
kaps_ipm_ipt_shuffle_callback(
    void *ipm_p,
    void *bundle_p,
    uint32_t newIptLocation,
    uint32_t length)
{
    kaps_pfx_bundle *pfxBundle_p = (kaps_pfx_bundle *) bundle_p;
    uint32_t oldIptLocation = pfxBundle_p->m_nIndex;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_ipm *self_p = (kaps_ipm *) ipm_p;
    kaps_fib_tbl_mgr *fibTblMgr = self_p->m_fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    int32_t isDirShiftUp;
    uint8_t *tempPtr;
    kaps_trie_node *iptNode_p, *rptNode_p;
    uint32_t numRptBitsLoppedOff;
    kaps_lsn_mc_settings *settings;
    struct kaps_device *device;
    uint8_t hb_data[KAPS_HB_ROW_WIDTH_8];
    kaps_status status;
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;

    (void) length;

    pfxBundle_p->m_nIndex = newIptLocation;

    isDirShiftUp = 1;
    if (oldIptLocation < newIptLocation)
    {
        /*
         * The direction is shift down. So make isDirShiftUp 0 
         */
        isDirShiftUp = 0;
    }

    tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle_p);
    kaps_memcpy(&iptNode_p, tempPtr, sizeof(kaps_trie_node *));

    settings = iptNode_p->m_lsn_p->m_pSettings;
    device = fibTblMgr->m_devMgr_p;

    rptNode_p = iptNode_p->m_rptParent_p;

    if (isDirShiftUp)
    {
        errNum = kaps_lsn_mc_update_iit(iptNode_p->m_lsn_p, &reason);

        if (errNum != NLMERR_OK)
        {
            self_p->cb_result = errNum;
            self_p->cb_reason = reason;
            return;
        }

    }

    numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(device, rptNode_p->m_depth);

    errNum = kaps_ipm_delete_entry_in_hw(self_p, pfxBundle_p->m_nIndex, rptNode_p->m_rptId,
                                         iptNode_p->m_depth - numRptBitsLoppedOff, &reason);

    if (errNum != NLMERR_OK)
    {
        self_p->cb_result = errNum;
        self_p->cb_reason = reason;
        return;
    }

    errNum = kaps_ipm_write_entry_to_hw(self_p, pfxBundle_p, rptNode_p->m_rptId,
                                        iptNode_p->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, &reason);
    if (errNum != NLMERR_OK)
    {
        self_p->cb_result = errNum;
        self_p->cb_reason = reason;
        return;
    }

    if (!isDirShiftUp)
    {
        errNum = kaps_lsn_mc_update_iit(iptNode_p->m_lsn_p, &reason);

        if (errNum != NLMERR_OK)
        {
            self_p->cb_result = errNum;
            self_p->cb_reason = reason;
            return;
        }
    }

    if (settings->m_areIPTHitBitsPresent)
    {
        if (iptNode_p->m_iitLmpsofarPfx_p && iptNode_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
        {
            struct kaps_ab_info *ab = self_p->m_ab_info;
            uint8_t hb_value = 0;

            if (db->num_algo_levels_in_db == 3)
            {
                status = kaps_kaps_read_trigger_hb_in_small_bb(fibTblMgr, ab, oldIptLocation, &hb_value, &reason);

                if (status != KAPS_OK)
                {
                    self_p->cb_result = NLMERR_FAIL;
                    self_p->cb_reason = NLMRSC_XPT_FAILED;
                }

                status = kaps_kaps_write_trigger_hb_in_small_bb(fibTblMgr, ab, newIptLocation, hb_value, &reason);

                if (status != KAPS_OK)
                {
                    self_p->cb_result = NLMERR_FAIL;
                    self_p->cb_reason = NLMRSC_XPT_FAILED;
                }

            }
            else
            {

                while (ab)
                {
                    /*
                     * Read the old hit bit
                     */
                    status = kaps_dm_kaps_hb_read(device, db, device->dba_offset + ab->ab_num, oldIptLocation, hb_data);
                    if (status != KAPS_OK)
                    {
                        self_p->cb_result = NLMERR_FAIL;
                        self_p->cb_reason = NLMRSC_XPT_FAILED;
                    }

                    /*
                     * copy the hit bit into the new location
                     */
                    status = kaps_dm_kaps_hb_write(device, db, device->dba_offset + ab->ab_num, newIptLocation, hb_data);
                    if (status != KAPS_OK)
                    {
                        self_p->cb_result = NLMERR_FAIL;
                        self_p->cb_reason = NLMRSC_XPT_FAILED;
                    }

                    ab = ab->dup_ab;
                }
            }
        }
    }

    fibStats->numOfIPTShuffle++;

}

static void
kaps_ipm_pvt_fill_ab_info(
    kaps_fib_tbl_mgr * fibTblMgr_p,
    struct kaps_ab_info *ab,
    uint32_t blkWidthInBits)
{
    struct kaps_db *db = fibTblMgr_p->m_curFibTbl->m_db;

    if (db->parent)
        db = db->parent;

    if (db->num_algo_levels_in_db > 2)
    {
        uint32_t numEntriesPerBrick = (KAPS_BKT_WIDTH_1 / (blkWidthInBits + KAPS_AD_WIDTH_1));

        if (numEntriesPerBrick > KAPS_MAX_PFX_PER_BKT_ROW)
            numEntriesPerBrick = KAPS_MAX_PFX_PER_BKT_ROW;

        ab->conf = KAPS_NORMAL_160;
    }
    else
    {
        if (blkWidthInBits <= 80)
        {
            ab->conf = KAPS_NORMAL_80;
            ab->num_slots = fibTblMgr_p->m_devMgr_p->num_80b_entries_ab;
        }
        else if (blkWidthInBits <= 160)
        {
            ab->conf = KAPS_NORMAL_160;
            ab->num_slots = fibTblMgr_p->m_devMgr_p->num_80b_entries_ab / 2;
        }
    }
}

kaps_ipm *
kaps_ipm_init(
    kaps_fib_tbl_mgr * fibTblMgr_p,
    uint32_t poolIndex,
    uint32_t pool_size,
    struct kaps_ab_info *startBlk,
    uint32_t blkWidthInBits,
    enum kaps_dba_trigger_compression_mode mode,
    NlmReasonCode * o_reason)
{
    kaps_ipm *ipm_p = NULL;
    uint32_t i;
    struct kaps_ab_info *cur_dup_ab;
    struct kaps_db *db = fibTblMgr_p->m_curFibTbl->m_db;

    if (db->num_algo_levels_in_db == 3)
    {
        /*
         * We are requesting a bigger size for KAPS-2 Large. The main reason for this is to avoid resizing the bitmap
         * or slots in the pfx_bundles array. Further down in this function, we are making sure that only up to the
         * pool size, the bits in the bitmap are set to indicate that the slots are available. For now we are not
         * making use of the oversizing feature but we might need it later during optimizations
         */
        ipm_p = (kaps_ipm *) kaps_simple_dba_init(fibTblMgr_p->m_devMgr_p, 0, KAPS2_MAX_SLOTS_IN_AB, 0, 0);
    }
    else
    {
        ipm_p = (kaps_ipm *) kaps_simple_dba_init(fibTblMgr_p->m_devMgr_p, 0, pool_size, 0, 0);
    }

    if (ipm_p == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    if (db->num_algo_levels_in_db == 3)
    {
        /*
         * Since we have requested more slots than needed for KAPS-2 Large, we now reset the bits beyond the pool size
         * to 0, so that only the bits from 0 to pool_size - 1 are set to 1.
         */
        for (i = pool_size; i < KAPS2_MAX_SLOTS_IN_AB; ++i)
        {
            kaps_reset_bit(&ipm_p->kaps_simple_dba.free, i);
            ipm_p->kaps_simple_dba.free_entries--;
            ipm_p->kaps_simple_dba.pool_size--;
        }
    }

    ipm_p->pool_index = poolIndex;
    ipm_p->m_fib_tbl_mgr = fibTblMgr_p;
    ipm_p->m_ab_info = startBlk;

    if (startBlk->ab_init_done)
        return ipm_p;

    kaps_ipm_pvt_fill_ab_info(fibTblMgr_p, startBlk, blkWidthInBits);

    cur_dup_ab = startBlk->dup_ab;
    while (cur_dup_ab)
    {
        kaps_ipm_pvt_fill_ab_info(fibTblMgr_p, cur_dup_ab, blkWidthInBits);
        cur_dup_ab = cur_dup_ab->dup_ab;
    }

    return ipm_p;
}

NlmErrNum_t
kaps_ipm_add_entry_to_brick(
    kaps_ipm * ipm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint32_t length,
    NlmReasonCode * o_reason)
{
    struct kaps_device *device = ipm_p->kaps_simple_dba.device;
    uint32_t cur_brick_nr;
    struct kaps_ab_info *ab = ipm_p->m_ab_info;
    uint32_t i, gran, numPfxInBkt, found = 0;
    struct kaps_simple_dba_range_for_pref_len *r;

    /*
     * This function is used for KAPS-2 Big where the triggers are stored in bricks. We need to look up the
     * free_slot_bmp to figure out the location of the free slot within the bricks
     */

    /*
     * Iterate through all the bricks
     */
    for (cur_brick_nr = 0; cur_brick_nr < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++cur_brick_nr)
    {
        gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].sub_ab_gran;

        if (length < gran)
        {
            /*
             * We have found a brick where the trigger will fit. Now search if there is a free slot in the
             * free_slot_bmp
             */
            numPfxInBkt = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].upper_index
                - device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].lower_index + 1;

            found = 0;
            for (i = 0; i < numPfxInBkt; ++i)
            {
                if (kaps_array_check_bit
                    (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].free_slot_bmp, i))
                {
                    /*
                     * We have found a free slot in the bitmap 
                     */
                    pfxBundle_p->m_nIndex =
                        device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].lower_index + i;

                    /*
                     * Make the bit in the bitmap 0 to indicate it is no longer free
                     */
                    kaps_array_set_bit(device->kaps_shadow->ab_to_small_bb[ab->ab_num].
                                       sub_ab_bricks[cur_brick_nr].free_slot_bmp, i, 0);

                    /*
                     * Update the information in the kaps_simple_dba also so that we are always consistent
                     */
                    ipm_p->kaps_simple_dba.pfx_bundles[pfxBundle_p->m_nIndex] = pfxBundle_p;

                    r = ipm_p->kaps_simple_dba.nodes[0].chunks;

                    if (!r)
                    {
                        ipm_p->kaps_simple_dba.nodes[0].chunks =
                            (struct kaps_simple_dba_range_for_pref_len *) device->alloc->xcalloc(device->alloc->cookie,
                                                                                                 1,
                                                                                                 sizeof(struct
                                                                                                        kaps_simple_dba_range_for_pref_len));
                        if (!ipm_p->kaps_simple_dba.nodes[0].chunks)
                            return KAPS_OUT_OF_MEMORY;

                        r = ipm_p->kaps_simple_dba.nodes[0].chunks;
                        r->prefix_len = kaps_ipm_recompute_len_in_simple_dba(ipm_p, length);

                        r->first_row = pfxBundle_p->m_nIndex;
                    }

                    if (r->first_row > pfxBundle_p->m_nIndex)
                        r->first_row = pfxBundle_p->m_nIndex;

                    if (r->last_row < pfxBundle_p->m_nIndex)
                        r->last_row = pfxBundle_p->m_nIndex;

                    kaps_reset_bit(&ipm_p->kaps_simple_dba.free, pfxBundle_p->m_nIndex);
                    kaps_set_bit(&ipm_p->kaps_simple_dba.nodes[0].used, pfxBundle_p->m_nIndex);

                    ipm_p->kaps_simple_dba.free_entries--;

                    found = 1;

                    break;
                }
            }

            if (found)
                break;
        }
    }

    /*
     * When this function is called we should always find a free slot. If we haven't found a free slot, then report an
     * error
     */
    if (!found)
    {
        kaps_assert(0, "Unable to insert IPT entry in the pool \n");
        *o_reason = NLMRSC_DBA_ALLOC_FAILED;
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ipm_add_entry(
    kaps_ipm * ipm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t numRptBitsLoppedOff,
    NlmReasonCode * o_reason)
{
    kaps_status status;
    NlmErrNum_t errNum;
    struct kaps_db *db = ipm_p->m_fib_tbl_mgr->m_curFibTbl->m_db;

    (void) numRptBitsLoppedOff;

    ipm_p->cb_result = NLMERR_OK;
    ipm_p->cb_reason = NLMRSC_REASON_OK;

    /*
     * We should not use kaps_simple_dba for 3 level KAPS. We should use ab_to_small_bb
     */
    if (db->num_algo_levels_in_db == 3)
    {
        errNum = kaps_ipm_add_entry_to_brick(ipm_p, pfxBundle_p, length, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;

    }
    else
    {
        status = kaps_simple_dba_find_place_for_entry(&ipm_p->kaps_simple_dba, pfxBundle_p, rptId,
                                                      length, (int32_t *) & pfxBundle_p->m_nIndex);

        if (status != KAPS_OK)
        {
            if (status == KAPS_OUT_OF_MEMORY)
                *o_reason = NLMRSC_LOW_MEMORY;

            return NLMERR_FAIL;
        }
    }

    if (pfxBundle_p->m_nIndex == KAPS_LSN_NEW_INDEX)
    {
        kaps_assert(0, "Unable to insert IPT entry in the pool \n");
        return NLMERR_FAIL;
    }

    *o_reason = ipm_p->cb_reason;

    return ipm_p->cb_result;

}

NlmErrNum_t
kaps_ipm_mark_slot_in_brick_bmp(
    kaps_ipm * ipm_p,
    uint32_t pfxIndex,
    uint32_t is_free)
{
    uint32_t cur_brick_nr, found;
    struct kaps_device *device = ipm_p->kaps_simple_dba.device;
    struct kaps_ab_info *ab = ipm_p->m_ab_info;
    struct kaps_db *db = ipm_p->m_fib_tbl_mgr->m_curFibTbl->m_db;
    
    uint32_t pos_in_brick;

    if (db->num_algo_levels_in_db == 2)
        return NLMERR_OK;

    /*
     * For KAPS 3 level, we should reset/set the free bit to 0/1 in the bitmap inside the brick to indicate that it is
     * occupied/free
     */
    found = 0;
    for (cur_brick_nr = 0; cur_brick_nr < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++cur_brick_nr)
    {
        if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].lower_index <= pfxIndex &&
            pfxIndex <= device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].upper_index)
        {
            found = 1;
            pos_in_brick =
                pfxIndex - device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].lower_index;

            if (is_free)
            {
                kaps_array_set_bit(device->kaps_shadow->ab_to_small_bb[ab->ab_num].
                                   sub_ab_bricks[cur_brick_nr].free_slot_bmp, pos_in_brick, 1);
            }
            else
            {
                kaps_array_set_bit(device->kaps_shadow->ab_to_small_bb[ab->ab_num].
                                   sub_ab_bricks[cur_brick_nr].free_slot_bmp, pos_in_brick, 0);
            }

            break;
        }
    }

    if (!found)
        kaps_assert(0, "Unable to find the prefix index while marking slot in small BB\n");

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ipm_wb_add_entry(
    kaps_ipm * ipm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t pos)
{
    uint32_t recomputed_len;

    pfxBundle_p->m_nIndex = pos;
    recomputed_len = kaps_ipm_recompute_len_in_simple_dba(ipm_p, length);
    kaps_simple_dba_place_entry_at_loc(&ipm_p->kaps_simple_dba, pfxBundle_p, rptId, recomputed_len, pos);

    /*
     * Pass 0 to indicate that the slot is now occupied
     */
    kaps_ipm_mark_slot_in_brick_bmp(ipm_p, pos, 0);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ipm_write_entry_to_hw(
    kaps_ipm * ipm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t numRptBitsLoppedOff,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_fib_tbl_mgr_callback_fn_ptrs *fnptrs = (kaps_fib_tbl_mgr_callback_fn_ptrs *) fibTblMgr->m_devWriteCallBackFns;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t blockWidthInBits;
    uint8_t *trig_data;
    int32_t numBitsRemaining, endBitPos, startBitPos;
    uint32_t i, readData, pfxSizeInBytes, curNumBitsToProcess;
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;

    if (!fnptrs->m_writeABData)
        return errNum;

    blockWidthInBits = kaps_ab_get_blk_width(ipm_p->m_ab_info);

    ipm_p->m_is_cur_active = 1;

    trig_data = &pfxBundle_p->m_data[numRptBitsLoppedOff / 8];

    if (db->num_algo_levels_in_db == 3 && numRptBitsLoppedOff % 8 != 0)
    {

        numBitsRemaining = length;
        trig_data = fibTblMgr->m_trigDataBuffer;
        pfxSizeInBytes = (pfxBundle_p->m_nPfxSize + 7) / 8;
        endBitPos = (pfxSizeInBytes * 8) - 1 - numRptBitsLoppedOff;

        kaps_memset(trig_data, 0, KAPS_HW_LPM_MAX_GRAN / 8);
        i = 0;
        while (numBitsRemaining > 0)
        {
            curNumBitsToProcess = 8;
            if (curNumBitsToProcess > numBitsRemaining)
            {
                curNumBitsToProcess = numBitsRemaining;
            }

            startBitPos = endBitPos - curNumBitsToProcess + 1;
            if (startBitPos < 0)
                startBitPos = 0;

            readData = KapsReadBitsInArrray(pfxBundle_p->m_data, pfxSizeInBytes, endBitPos, startBitPos);

            if (numBitsRemaining >= 8)
            {
                trig_data[i] = readData & 0xFF;
            }
            else
            {
                trig_data[i] = (readData & 0xFF) << (8 - numBitsRemaining);
            }

            ++i;
            numBitsRemaining -= 8;
            endBitPos -= 8;
        }
    }

    errNum = fnptrs->m_writeABData(fibTblMgr, 0, blockWidthInBits, ipm_p->m_ab_info,
                                   pfxBundle_p->m_nIndex, (uint8_t) rptId,
                                   (uint16_t) (blockWidthInBits - 1), 0,
                                   trig_data, (uint16_t) length, ipm_p->m_start_offset_1 / KAPS_BITS_IN_BYTE, o_reason);

    ipm_p->m_is_cur_active = 0;

    if (db->num_algo_levels_in_db == 3)
    {
        /*
         * Do nothing. Count the number of writes in fib_kaps_hw.c due to buffering
         */
    }
    else
    {
        fibStats->numIPTWrites++;
    }

    return errNum;

}

NlmErrNum_t
kaps_ipm_remove_entry(
    kaps_ipm * ipm_p,
    uint32_t pfxIndex,
    uint32_t rptId,
    uint32_t length,
    NlmReasonCode * o_reason)
{
    uint32_t recomputed_len;
    (void) o_reason;

    recomputed_len = kaps_ipm_recompute_len_in_simple_dba(ipm_p, length);
    kaps_simple_dba_free_entry(&ipm_p->kaps_simple_dba, rptId, recomputed_len, pfxIndex);

    /*
     * Pass 1 to indicate that the slot is now free
     */
    kaps_ipm_mark_slot_in_brick_bmp(ipm_p, pfxIndex, 1);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ipm_delete_entry_in_hw(
    kaps_ipm * ipm_p,
    uint32_t pfxIndex,
    uint32_t rptId,
    uint32_t length,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_fib_tbl_mgr_callback_fn_ptrs *fnptrs = (kaps_fib_tbl_mgr_callback_fn_ptrs *) fibTblMgr->m_devWriteCallBackFns;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t blockWidthInBits;
    uint8_t invalid_data[KAPS_LPM_KEY_MAX_WIDTH_1 / KAPS_BITS_IN_BYTE];
    struct kaps_db *db = fibTblMgr->m_curFibTbl->m_db;

    (void) rptId;
    (void) length;

    if (!fnptrs->m_deleteABData)
        return errNum;

    blockWidthInBits = kaps_ab_get_blk_width(ipm_p->m_ab_info);

    ipm_p->m_is_cur_active = 1;

    if (ipm_p->mode != DBA_NO_TRIGGER_COMPRESSION)
    {
        ipm_p->m_is_delete_op = 1;

        kaps_memset(invalid_data, 0, KAPS_LPM_KEY_MAX_WIDTH_1 / KAPS_BITS_IN_BYTE);

        errNum = fnptrs->m_writeABData(fibTblMgr, 0, blockWidthInBits, ipm_p->m_ab_info,
                                       pfxIndex, (uint8_t) rptId,
                                       (uint16_t) (blockWidthInBits - 1), 0,
                                       invalid_data, (uint16_t) length, ipm_p->m_start_offset_1 / KAPS_BITS_IN_BYTE,
                                       o_reason);

        ipm_p->m_is_delete_op = 0;
    }
    else
    {

        errNum = fnptrs->m_deleteABData(fibTblMgr, 0, blockWidthInBits, ipm_p->m_ab_info, pfxIndex, o_reason);

    }
    ipm_p->m_is_cur_active = 0;

    if (db->num_algo_levels_in_db == 3)
    {
        /*
         * Do nothing. Count the number of writes in fib_kaps_hw.c due to buffering
         */
    }
    else
    {
        fibStats->numIPTWrites++;
    }

    return errNum;

}

void
kaps_ipm_delete_entry_at_location(
    void *self,
    uint32_t entry_nr)
{
    kaps_ipm *ipm_p = (kaps_ipm *) self;
    kaps_fib_tbl_mgr *fibTblMgr = (kaps_fib_tbl_mgr *) ipm_p->m_fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_fib_tbl_mgr_callback_fn_ptrs *fnptrs = (kaps_fib_tbl_mgr_callback_fn_ptrs *) fibTblMgr->m_devWriteCallBackFns;
    NlmReasonCode reason;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t blkWidthInBits;

    if (ipm_p->mode == DBA_NO_TRIGGER_COMPRESSION)
    {

        blkWidthInBits = kaps_ab_get_blk_width(ipm_p->m_ab_info);

        ipm_p->m_is_cur_active = 1;

        errNum = fnptrs->m_deleteABData(fibTblMgr, 0, blkWidthInBits, ipm_p->m_ab_info, entry_nr, &reason);

        ipm_p->m_is_cur_active = 0;

        fibStats->numAPTWrites++;

        if (errNum != NLMERR_OK)
            kaps_assert(0, "Error in DeleteEntryAtLocation while deleting IPT entry\n");
    }

}

NlmErrNum_t
kaps_ipm_fix_entry(
    kaps_ipm * ipm_p,
    uint32_t ab_num,
    uint32_t row_nr,
    NlmReasonCode * reason)
{
    kaps_pfx_bundle *pfxBundle_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    uint8_t *tempPtr;
    kaps_trie_node *iptNode_p, *rptNode_p;
    uint32_t numRptBitsLoppedOff;
    uint32_t location, blockWidthInBits;
    kaps_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;

    if (ab_num != ipm_p->m_ab_info->ab_num)
    {
        *reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    blockWidthInBits = kaps_ab_get_blk_width(ipm_p->m_ab_info);
    location = row_nr / (blockWidthInBits / KAPS_HW_MIN_DBA_WIDTH_1);
    pfxBundle_p = kaps_simple_dba_get_entry(&ipm_p->kaps_simple_dba, location);

    if (!pfxBundle_p)
    {
        kaps_ipm_delete_entry_at_location(ipm_p, location);
        return NLMERR_OK;
    }

    tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle_p);
    kaps_memcpy(&iptNode_p, tempPtr, sizeof(kaps_trie_node *));

    rptNode_p = iptNode_p->m_rptParent_p;

    numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(fibTblMgr->m_devMgr_p, rptNode_p->m_depth);

    errNum = kaps_ipm_write_entry_to_hw(ipm_p, pfxBundle_p, rptNode_p->m_rptId,
                                        iptNode_p->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, reason);

    return errNum;
}

NlmErrNum_t
kaps_ipm_create_entry_data(
    kaps_ipm * ipm_p,
    uint32_t ab_num,
    uint32_t row_nr,
    uint8_t * data,
    uint8_t * mask,
    uint8_t is_xy,
    NlmReasonCode * reason)
{
    kaps_pfx_bundle *pfxBundle_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    uint8_t *tempPtr;
    kaps_trie_node *iptNode_p, *rptNode_p;
    uint32_t numRptBitsLoppedOff;
    uint32_t location, blockWidthInBits;
    uint32_t byte_offset, start_bit;
    uint8_t num_bytes;
    uint32_t data_len;
    kaps_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;
    uint32_t rptId;

    if (ab_num != ipm_p->m_ab_info->ab_num)
    {
        *reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    blockWidthInBits = kaps_ab_get_blk_width(ipm_p->m_ab_info);
    location = row_nr / (blockWidthInBits / KAPS_HW_MIN_DBA_WIDTH_1);
    pfxBundle_p = kaps_simple_dba_get_entry(&ipm_p->kaps_simple_dba, location);

    if ((!pfxBundle_p && !ipm_p->m_is_cur_active) || ipm_p->m_is_delete_op)
    {
        if (is_xy)
        {
            byte_offset = ipm_p->m_start_offset_1 / KAPS_BITS_IN_BYTE;
            kaps_memset(&data[byte_offset], 0xFF, ipm_p->m_num_bytes);
            kaps_memset(&mask[byte_offset], 0xFF, ipm_p->m_num_bytes);
        }
        return NLMERR_OK;
    }

    if (!ipm_p->m_is_cur_active)
    {

        tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle_p);
        kaps_memcpy(&iptNode_p, tempPtr, sizeof(kaps_trie_node *));

        rptNode_p = iptNode_p->m_rptParent_p;
        rptId = rptNode_p->m_rptId;

        /*
         * During an RPT split, when we have trigger compression, we can end up with the following scenario: The AB can 
         * have two 40b columns - Col A and Col B. The old RPT has triggers in Col A and the new RPT triggers will have 
         * to be written to Col B. There is a trigger for old RPT in a row, say row 4000. This trigger should be moved
         * to new RPT. By the time we reach this function, the RPT ID of the new RPT node has been changed and the IPT
         * trie node points to the new RPT node. So when trying to write Col B of row 4000, we have to also get Col A
         * of row 4000. But since Col A still has the old RPT entries, we should get the old RPT entry instead of the
         * new RPT entry. To achieve this we are doing the following. This has to be done only when
         * CopyIptAndAptToNewPool is going on. Once we finish the CopyIptAndAptToNewPool, we will have to delete the
         * entries of the old RPT entry in Col A that have been moved to Col B. At this time, when we will call
         * DeleteIptAndAptInOldPool. In this case while trying to delete entries in Col A, we will have to fetch the
         * corresponding entry in Col B. Now we should get the new RPT node and not the old RPT node. If m_isRptNode is 
         * not yet set to 1, then this is a new RPT node during Split and Push Or RPT node being moved during RPT move
         */
        if (rptNode_p->m_trie_p->m_isCopyIptAndAptToNewPoolInProgress && !rptNode_p->m_isRptNode)
        {

            if (rptNode_p->m_trie_p->m_trie_global->m_rptOp == KAPS_RPT_MOVE)
            {
                rptId = rptNode_p->m_trie_p->m_trie_global->m_oldRptId;
            }
            else
            {

                /*
                 * It is Split or Push Operation
                 */
                rptNode_p = rptNode_p->m_parent_p;
                rptId = rptNode_p->m_rptId;
            }
        }

        numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(fibTblMgr->m_devMgr_p, rptNode_p->m_depth);

        byte_offset = ipm_p->m_start_offset_1 / KAPS_BITS_IN_BYTE;
        data_len = iptNode_p->m_depth - numRptBitsLoppedOff;
        num_bytes = (uint8_t) ((data_len + 7) >> 3);

        data[byte_offset] = rptId;
        kaps_memcpy(&data[byte_offset + 1], &pfxBundle_p->m_data[numRptBitsLoppedOff / 8], num_bytes);
        start_bit = blockWidthInBits - 1 - ipm_p->m_start_offset_1;

        kaps_FillZeroes(mask, blockWidthInBits / KAPS_BITS_IN_BYTE, start_bit, (start_bit - (data_len - 1) - 8));
    }

    if (is_xy)
    {
        uint8_t data_d[KAPS_LPM_KEY_MAX_WIDTH_1 / KAPS_BITS_IN_BYTE];
        uint8_t data_m[KAPS_LPM_KEY_MAX_WIDTH_1 / KAPS_BITS_IN_BYTE];

        byte_offset = ipm_p->m_start_offset_1 / KAPS_BITS_IN_BYTE;

        kaps_memcpy(data_d, &data[byte_offset], ipm_p->m_num_bytes);
        kaps_memcpy(data_m, &mask[byte_offset], ipm_p->m_num_bytes);

        kaps_convert_dm_to_xy(data_d, data_m, &data[byte_offset], &mask[byte_offset], ipm_p->m_num_bytes);
    }
    return errNum;
}

NlmErrNum_t
kaps_ipm_init_rpt(
    kaps_ipm * ipm_p,
    uint32_t rptId,
    NlmReasonCode * o_reason)
{
    kaps_status status = kaps_kaps_simple_dba_init_node(&ipm_p->kaps_simple_dba, rptId,
                                                        kaps_ipm_ipt_shuffle_callback);

    if (status != KAPS_OK)
    {
        if (status == KAPS_OUT_OF_MEMORY)
            *o_reason = NLMRSC_LOW_MEMORY;

        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ipm_free_rpt(
    kaps_ipm * ipm_p,
    uint32_t rptId)
{
    kaps_simple_dba_free_node(&ipm_p->kaps_simple_dba, rptId);

    return NLMERR_OK;
}

void
kaps_ipm_destroy(
    kaps_nlm_allocator * alloc_p,
    kaps_ipm * ipm)
{
    kaps_simple_dba_destroy(&ipm->kaps_simple_dba);
}

uint32_t
kaps_ipm_verify(
    kaps_ipm * ipm_p,
    uint32_t rptId)
{
    uint32_t num_entries;

    num_entries = kaps_simple_dba_verify(&ipm_p->kaps_simple_dba, rptId, kaps_get_pfx_bundle_prio_length);

    return num_entries;
}

/* warmboot functions start here */

int32_t
kaps_ipm_wb_store_ipt_entries(
    kaps_ipm * ipm_p,
    uint32_t rpt_id,
    uint32_t * nv_offset,
    kaps_device_issu_write_fn write_fn,
    void *handle)
{
    struct kaps_simple_dba_range_for_pref_len *r;
    struct kaps_wb_ipt_entry_info ipt_entry;
    kaps_pfx_bundle *pfx_bundle;
    uint32_t len_in_bytes = 0, pos;
    uint8_t *pfx_data;
    void *tempPtr;
    kaps_trie_node *trienode;
    int32_t num_ipt_entries = 0;
    uint32_t rpt_lopoff_1;
    kaps_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;

    r = ipm_p->kaps_simple_dba.nodes[rpt_id].chunks;

    kaps_ipm_verify(ipm_p, rpt_id);

    while (r)
    {
        for (pos = r->first_row; pos <= r->last_row; pos++)
        {
            pfx_bundle = ipm_p->kaps_simple_dba.pfx_bundles[pos];
            if (pfx_bundle == NULL)     /* no entry at this position */
                continue;

            tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfx_bundle);
            kaps_memcpy(&trienode, tempPtr, sizeof(kaps_trie_node *));

            if (trienode->m_rptParent_p->m_rptId != rpt_id)
                continue;

            pfx_data = KAPS_PFX_BUNDLE_GET_PFX_DATA(pfx_bundle);
            len_in_bytes = KAPS_PFX_BUNDLE_GET_PFX_SIZE(pfx_bundle);
            len_in_bytes = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(len_in_bytes);
            kaps_memcpy(ipt_entry.ipt_entry, pfx_data, len_in_bytes);

            rpt_lopoff_1 =
                kaps_trie_get_num_rpt_bits_lopped_off(fibTblMgr->m_devMgr_p, trienode->m_rptParent_p->m_depth);
            ipt_entry.ipt_entry_len_1 = pfx_bundle->m_nPfxSize - rpt_lopoff_1;
            ipt_entry.is_reserved_160b_trig = trienode->m_isReserved160bTrig;

            ipt_entry.blk_num = ipm_p->m_ab_info->ab_num;
            ipt_entry.addr = pos;

            if (NLMERR_OK != kaps_lsn_mc_wb_prepare_lsn_info(trienode->m_lsn_p, &ipt_entry.lsn_info))
                return -1;

            len_in_bytes = sizeof(ipt_entry);
            if (0 != write_fn(handle, (uint8_t *) & ipt_entry, len_in_bytes, *nv_offset))
                return -1;
            num_ipt_entries++;

            *nv_offset += len_in_bytes;
            if (NLMERR_OK != kaps_lsn_mc_wb_store_prefixes(trienode->m_lsn_p, nv_offset, write_fn, handle))
                return -1;

        }

        r = r->next;    /* Next range */
    }

    return num_ipt_entries;
}
