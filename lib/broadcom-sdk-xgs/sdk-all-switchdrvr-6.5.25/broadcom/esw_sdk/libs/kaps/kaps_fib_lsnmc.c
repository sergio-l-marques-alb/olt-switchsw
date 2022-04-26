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
#include "kaps_fib_poolmgr.h"
#include "kaps_uda_mgr.h"
#include "kaps_algo_hw.h"
#include "kaps_ad_internal.h"
#include "kaps_ix_mgr.h"
#include "kaps_handle.h"
#include "kaps_fib_trie.h"

kaps_status
kaps_fib_hb_cb(
    void *client,
    struct kaps_entry *entry,
    uint32_t old_hb_index,
    uint32_t new_hb_index)
{
    struct kaps_hb_db *hb_db = client;
    struct kaps_device *device = hb_db->db_info.device;
    struct kaps_lpm_entry *lpm_entry = (struct kaps_lpm_entry *) entry;
    struct kaps_hb *hb = NULL;
    uint8_t *nv_ptr;
    struct kaps_lpm_cr_entry_info *shuffle_entry = NULL;
    struct kaps_db *db;
    struct kaps_aging_entry *active_aging_table;

    KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, entry, db);

    active_aging_table = kaps_device_get_active_aging_table(device, db);

    nv_ptr = device->nv_ptr;

    if (nv_ptr)
    {
        uint32_t offset;
        
        offset = kaps_db_entry_get_nv_offset(db, entry->user_handle);

        nv_ptr += offset;
        shuffle_entry = (struct kaps_lpm_cr_entry_info *) nv_ptr;
        shuffle_entry->hb_bit_no = new_hb_index;
        if (lpm_entry->hb_user_handle)
            shuffle_entry->hb_user_handle = lpm_entry->hb_user_handle;
        else
            shuffle_entry->hb_user_handle = 0;
    }

    if (!lpm_entry->hb_user_handle)
    {
        if (old_hb_index != KAPS_LSN_NEW_INDEX)
            kaps_sassert(active_aging_table[old_hb_index].entry == NULL);

        if (new_hb_index != KAPS_LSN_NEW_INDEX)
            kaps_sassert(active_aging_table[new_hb_index].entry == NULL);

        return KAPS_OK;
    }

    KAPS_WB_HANDLE_READ_LOC((&hb_db->db_info), (&hb), (uintptr_t) lpm_entry->hb_user_handle);

    if (old_hb_index != KAPS_LSN_NEW_INDEX)
        kaps_sassert(hb->bit_no == old_hb_index);

    if (old_hb_index == new_hb_index)
    {
        kaps_sassert(old_hb_index != KAPS_LSN_NEW_INDEX);
        kaps_sassert(active_aging_table[old_hb_index].entry ==
                     KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, entry->user_handle));
        return KAPS_OK;
    }

    kaps_sassert(active_aging_table[new_hb_index].entry == NULL);

    if (old_hb_index != KAPS_LSN_NEW_INDEX)
        kaps_sassert(active_aging_table[old_hb_index].entry ==
                     KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, entry->user_handle));

    active_aging_table[new_hb_index].entry = KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_entry, entry->user_handle);
    hb->bit_no = new_hb_index;

    if (old_hb_index != KAPS_LSN_NEW_INDEX)
    {
        active_aging_table[new_hb_index].num_idles = active_aging_table[old_hb_index].num_idles;
        active_aging_table[new_hb_index].intermediate_hb_val = active_aging_table[old_hb_index].intermediate_hb_val;

        active_aging_table[old_hb_index].entry = NULL;
        active_aging_table[old_hb_index].num_idles = 0;
        active_aging_table[old_hb_index].intermediate_hb_val = 0;
    }
    else
    {
        active_aging_table[new_hb_index].num_idles = 0;
    }

    return KAPS_OK;
}


struct kaps_ad_db *
kaps_lsn_mc_get_ad_db(
    kaps_lsn_mc *curLsn,
    kaps_lpm_lpu_brick *curBrick)
{
    struct kaps_ad_db *ad_db = NULL;
    
    if (curLsn->m_pSettings->m_isPerLpuGran) {
        ad_db = curBrick->m_ixInfo->ad_info->ad_db;
    } else {
        ad_db = curLsn->m_ixInfo->ad_info->ad_db;
    }

    return ad_db;
}



struct kaps_ix_mgr*
kaps_lsn_mc_get_ix_mgr_for_lsn_pfx(
    kaps_lsn_mc * self,
    struct kaps_ad_db *ad_db,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum)
{
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    struct kaps_ix_mgr *ix_mgr;
    uint32_t daisy_chain_id;
    
    daisy_chain_id = kaps_get_daisy_chain_id(device, self, uda_chunk, brickNum);

    ix_mgr = ad_db->mgr[device->core_id][daisy_chain_id];

    return ix_mgr;
}


struct kaps_ix_mgr*
kaps_lsn_mc_get_ix_mgr_for_lmpsofar_pfx(
    struct kaps_ad_db *ad_db)
{
    struct kaps_db *db = ad_db->db_info.common_info->ad_info.db;
    struct kaps_device *device = db->device;
    struct kaps_ix_mgr *ix_mgr;
    uint32_t daisy_chain_id = 0;



    
    ix_mgr = ad_db->mgr[device->core_id][daisy_chain_id];

    return ix_mgr;
}



void
kaps_lsn_mc_incr_num_bricks_allocated_for_all_lsns(
    kaps_lsn_mc_settings * settings,
    uint32_t size)
{
    settings->m_numAllocatedBricksForAllLsns += size;
}

void
kaps_lsn_mc_decr_num_bricks_allocated_for_all_lsns(
    kaps_lsn_mc_settings * settings,
    uint32_t size)
{
    if (settings->m_numAllocatedBricksForAllLsns > size)
    {
        settings->m_numAllocatedBricksForAllLsns -= size;
    }
    else
    {
        settings->m_numAllocatedBricksForAllLsns = 0;
    }
}


NlmErrNum_t
kaps_lsn_mc_check_joined_udc_alloc(
    kaps_lsn_mc * self,
    uint32_t rqtNumUdaBricks,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    if (self->m_mlpMemInfo->size > rqtNumUdaBricks)
    {

        if (self->m_mlpMemInfo->size > settings->m_maxLpuPerLsn)
        {
            kaps_assert(0, "UDA chunk size is greater than Max LPU per LSN");
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }

        if (self->m_mlpMemInfo->size != rqtNumUdaBricks + 1)
        {
            kaps_assert(0, "Recheck if we are getting the correct UDA chunk");
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }
    }

    return NLMERR_OK;
}

static kaps_lsn_mc_settings *
kaps_lsn_mc_settings_pvt_ctor(
    kaps_lsn_mc_settings * self,
    kaps_nlm_allocator * pAlloc,
    struct kaps_uda_mgr **pMlpMemMgr)
{
    uint32_t i;

    self->m_pAlloc = pAlloc;

    self->m_pMlpMemMgr = pMlpMemMgr;

    for (i = 0; i < KAPS_MAX_NUM_STORED_LSN_INFO; ++i)
    {
        self->m_pTmpMlpMemInfoBuf[i] =
            (struct uda_mem_chunk *) kaps_nlm_allocator_calloc(pAlloc, 1, sizeof(struct uda_mem_chunk));
    }

    return self;
}

static NlmErrNum_t
kaps_lsn_mc_settings_pvt_fill_cmp_lookup_table(
    kaps_lsn_mc_settings * self)
{
    uint32_t cmpLookupTable[] = {
        0, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, /* 0x0f */
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x1f */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 0x2f */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 0x3f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x4f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x5f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x6f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x7f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x8f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x9f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xaf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xbf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xcf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xdf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xef */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0xff */
    };

    kaps_memcpy(self->m_cmpLookupTable, cmpLookupTable, sizeof(cmpLookupTable));

    return NLMERR_OK;
}


kaps_lsn_mc_settings *
kaps_lsn_mc_settings_create(
    kaps_nlm_allocator * pAlloc,
    kaps_fib_tbl * fibTbl,
    struct kaps_device * device,
    struct kaps_uda_mgr ** pMlpMemMgr,
    kaps_fib_prefix_index_changed_app_cb_t pAppCB,
    void *pAppData,
    uint32_t numLpuPerRow,
    uint32_t numJoinedSets,
    uint32_t numJoinedBricksInOneSet)
{
    kaps_lsn_mc_settings *ret;
    uint16_t curGranIx, length;
    uint32_t i;
    uint32_t ad_width_per_pfx_1;
    uint32_t max_encoded_pfx_len;
    struct kaps_db *db = fibTbl->m_db;
    struct kaps_ad_db *ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;
    uint32_t num_bit_combinations = 1;
    uint32_t num_pfx_based_on_gran, num_ad_dbs = 0, alloc_max_region;
    uint32_t usable_bkt_width_1;
    uint32_t maxPfxForCurGranIx;
    uint32_t usable_bkt_width_for_in_place_1;
    uint32_t choose_inplace_for_caution_height;
    uint32_t num_20b_ad_dbs;
    uint32_t are_all_20b_ad_dbs_inplace;
    uint32_t max_num_pfx_in_480b;
    uint16_t *lpm_gran_array = NULL;


    ret = (kaps_lsn_mc_settings *) kaps_nlm_allocator_calloc(pAlloc, 1, sizeof(kaps_lsn_mc_settings));

    kaps_lsn_mc_settings_pvt_ctor(ret, pAlloc, pMlpMemMgr);

    ret->m_lsnid = 0;

    ret->m_maxLpuPerLsn = numLpuPerRow;
    if (ret->m_maxLpuPerLsn > device->hw_res->max_lpu_per_db)
    {
        ret->m_maxLpuPerLsn = device->hw_res->max_lpu_per_db;
    }

    alloc_max_region = kaps_uda_mgr_max_region_width(ret->m_pMlpMemMgr[0]);

    if (alloc_max_region != 0)
    {
        if (alloc_max_region < ret->m_maxLpuPerLsn)
            ret->m_maxLpuPerLsn = alloc_max_region;
    }

    ret->m_initialMaxLpuPerLsn = ret->m_maxLpuPerLsn;

    ret->m_pAppCB = pAppCB;
    ret->m_pAppData = pAppData;

    ret->m_device = device;
    ret->m_fibTbl = fibTbl;

    ret->m_numJoinedSets = numJoinedSets;
    ret->m_numJoinedBricksInOneSet = numJoinedBricksInOneSet;

    if (db->common_info->hb_info.hb)
    {
        if (device->type == KAPS_DEVICE_KAPS)
        {
            if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID
                || device->id == KAPS_QUX_DEVICE_ID
                || device->id == KAPS_QUMRAN_DEVICE_ID || device->id == KAPS_JERICHO_2_DEVICE_ID)
            {
                ret->m_pHitBitCB = kaps_fib_hb_cb;
                ret->m_pHitBitCBData = db->common_info->hb_info.hb;
                ret->m_areHitBitsPresent = 1;

                ret->m_areIPTHitBitsPresent = 1;
                if (device->id == KAPS_QUMRAN_DEVICE_ID)
                {
                    ret->m_areIPTHitBitsPresent = 0;
                }

                if (db->num_algo_levels_in_db == 3)
                {
                    ret->m_areRPTHitBitsPresent = 1;
                }
            }
        }
    }

    num_20b_ad_dbs = 0;
    are_all_20b_ad_dbs_inplace = 1;
    while (ad_db)
    {
        num_ad_dbs++;
        if (ad_db->user_width_1 == 20)
        {
            num_20b_ad_dbs++;
            if (ad_db->db_info.hw_res.ad_res->ad_type != KAPS_AD_TYPE_INPLACE)
            {
                are_all_20b_ad_dbs_inplace = 0;
            }
        }
        
        ad_db = ad_db->next;
    }

    /*
     * Fill up the compare look up table
     */
    kaps_lsn_mc_settings_pvt_fill_cmp_lookup_table(ret);

    max_encoded_pfx_len = KAPS_LPM_KEY_MAX_WIDTH_1;
    if (device->type == KAPS_DEVICE_KAPS)
    {
        max_encoded_pfx_len = KAPS_HW_LPM_MAX_GRAN;
    }

    if (db->num_algo_levels_in_db == 2)
    {
        ret->lpm_num_gran = device->hw_res->lpm_middle_level_num_gran;
        lpm_gran_array = device->hw_res->lpm_middle_level_gran_array;
    }
    else
    {
        ret->lpm_num_gran = device->hw_res->lpm_num_gran;
        lpm_gran_array = device->hw_res->lpm_gran_array;
    }
        

    /*
     * Compute the length to gran for the final stage
     */
    curGranIx = 0;
    for (length = 0; length <= max_encoded_pfx_len; ++length)
    {
        /*
         * There can be multiple repeated granularities as in RevA 
         */
        while (length > lpm_gran_array[curGranIx])
            curGranIx++;
        ret->m_lengthToGran[length] = lpm_gran_array[curGranIx];
        ret->m_lengthToGranIx[length] = curGranIx;
    }

    /*
     * Compute the length to gran for the middle level of KAPS
     */
    curGranIx = 0;
    if (device->hw_res->lpm_middle_level_num_gran)
    {
        for (length = 0; length <= max_encoded_pfx_len; ++length)
        {
            if (length > device->hw_res->lpm_middle_level_gran_array[curGranIx])
                curGranIx++;

            ret->m_middleLevelLengthToGran[length] = device->hw_res->lpm_middle_level_gran_array[curGranIx];
            ret->m_middleLevelLengthToGranIx[length] = curGranIx;
        }

    }

    if (device->type == KAPS_DEVICE_KAPS)
    {
        ret->m_isPerLpuGran = 1;
        ret->m_isAptPresent = 0;
        ret->m_lpu_brick_width_1 = KAPS_BKT_WIDTH_1 * device->hw_res->num_bb_in_one_chain;
        ret->m_bdata_ads_size_1 = 20;

        ret->m_isHardwareMappedIx = 1;
        ret->m_isFullWidthLsn = 0;
        ret->m_isShrinkEnabled = 1;
        ret->m_isAncestorLsnMergeEnabled = 1;
        ret->m_treat_20b_ad_as_inplace = 1;

        if (!are_all_20b_ad_dbs_inplace)
            ret->m_treat_20b_ad_as_inplace = 0;

        ret->m_isMultiBrickAlloc = 0;

        if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
        {
            ret->m_isPerLpuGran = 0;
            ret->m_isHardwareMappedIx = 1;
            ret->m_isAncestorLsnMergeEnabled = 1;

            if (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM0
                || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0)
            {
                ret->m_isFullWidthLsn = 1;
                ret->m_isShrinkEnabled = 0;
            }
            else
            {
                ret->m_isMultiBrickAlloc = 1;
                ret->m_numMultiBricks = 4;
                ret->m_isShrinkEnabled = 1;
            }
        }

        if (device->id == KAPS_JERICHO_2_DEVICE_ID)
        {
            if (db->num_algo_levels_in_db == 3)
            {
                ret->m_isPerLpuGran = 1;
                ret->m_isJoinedUdc = 1;
                ret->m_splitIfLowUda = 1;
                ret->m_isRelatedLsnMergeEnabled = 1;
            }
        }
        

    }

    if (device->hw_res->no_overflow_lpm || fibTbl->m_db->hw_res.db_res->num_dba_dt == 0)
    {
        ret->m_isAptPresent = 0;
        ret->m_isUnderAllocateLpuIx = 0;
    }

    ad_width_per_pfx_1 = 0;
    if (fibTbl->m_db->common_info->ad_info.ad)
        ad_width_per_pfx_1 = device->hw_res->inplace_width_1;

    /*Calculate the max number of prefixes for different granularities*/
    for (i = 0; i < ret->lpm_num_gran; ++i)
    {
        max_num_pfx_in_480b = 16;
        
        usable_bkt_width_1 = KAPS_BKT_WIDTH_1;
        

        ret->m_maxPfxInBrickForGranIx[i] = usable_bkt_width_1 
            / (lpm_gran_array[i] + ad_width_per_pfx_1);

        
        if (ret->m_maxPfxInBrickForGranIx[i] > max_num_pfx_in_480b)
            ret->m_maxPfxInBrickForGranIx[i] = max_num_pfx_in_480b;
        
        ret->m_maxPfxInBrickForGranIx[i] *= device->hw_res->num_bb_in_one_chain;

        ret->m_numHolesForGranIx[i] = 0;
        ret->m_granIxToGran[i] = lpm_gran_array[i];
        
    }

    ret->m_maxNumPfxInAnyBrick = ret->m_maxPfxInBrickForGranIx[0];


    /*Calculate the max number of IN-PLACE prefixes for different granularities*/
    for (i = 0; i < ret->lpm_num_gran; ++i)
    {
        max_num_pfx_in_480b = 16;
        
        usable_bkt_width_for_in_place_1 = KAPS_BKT_WIDTH_1;

        ret->m_max20bInPlacePfxInBrickForGranIx[i] = usable_bkt_width_for_in_place_1 
                 / (lpm_gran_array[i] + device->hw_res->inplace_width_1);

        if (ret->m_max20bInPlacePfxInBrickForGranIx[i] > max_num_pfx_in_480b)
            ret->m_max20bInPlacePfxInBrickForGranIx[i] = max_num_pfx_in_480b;
        
        ret->m_max20bInPlacePfxInBrickForGranIx[i] *= device->hw_res->num_bb_in_one_chain;
    }
    ret->m_maxNumInPlacePfxInAnyBrick = ret->m_max20bInPlacePfxInBrickForGranIx[0];

    
    /*Calculate the maximum number of TRIGGERS for 3-level device*/
    if (device->hw_res->num_algo_levels > 2) 
    {
        ret->m_maxTriggersPerBktRow = 16;
        
        for (i = 0; i < device->hw_res->lpm_middle_level_num_gran; ++i) 
        {
            ret->m_maxTriggersInBrickForGranIx[i] = (KAPS_BKT_WIDTH_1)
                / (device->hw_res->lpm_middle_level_gran_array[i] + KAPS2_BPM_ADS2_LOCATION_SIZE_1);

            if (ret->m_maxTriggersInBrickForGranIx[i] > ret->m_maxTriggersPerBktRow)
                ret->m_maxTriggersInBrickForGranIx[i] = ret->m_maxTriggersPerBktRow;
        }
    }

    choose_inplace_for_caution_height = 1;
    
    if (!ret->m_treat_20b_ad_as_inplace)
        choose_inplace_for_caution_height = 0;
            

    /*
     * If we go too deep, then the number of bit combinations will be less and the LSN can contain very few entries. So 
     * find out the height beyond which we have to proceed cautiously
     */
    ret->m_bottom_up_caution_height = lpm_gran_array[0];
    for (length = 0; length < lpm_gran_array[1]; ++length)
    {
        num_bit_combinations = 1 << length;
        curGranIx = ret->m_lengthToGranIx[length + 1];  /* Add 1 bit for MPE */

        if (choose_inplace_for_caution_height)
        {
            maxPfxForCurGranIx = ret->m_max20bInPlacePfxInBrickForGranIx[curGranIx];
        }
        else
        {
            maxPfxForCurGranIx = ret->m_maxPfxInBrickForGranIx[curGranIx];
        }
        
        num_pfx_based_on_gran = maxPfxForCurGranIx * ret->m_maxLpuPerLsn;

        if (num_bit_combinations >= num_pfx_based_on_gran)
        {
            ret->m_bottom_up_caution_height = length;
            break;
        }
    }

    /*
     * The J2P HW and SDK Gran IX Mapping is shown below
     */
    /*
     * 0, 1, 12, 2, 13, 3, 14, 4, 5, 6, 7, 15, 8, 9, 10, 11 (HW Gran IX)
     */
    /*
     * 0, 8, 12, 16, 20, 24, 28, 32, 40, 48, 56, 60, 72, 96, 136, 160 (Gran) 
     */
    /*
     * 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 (SDK Brick Gran IX)
     */

    if (device->type == KAPS_DEVICE_KAPS)
    {


        if (device->id == KAPS_JERICHO_2_DEVICE_ID &&
            device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_J2P_THREE_LEVEL)
        {

            ret->m_granIxToFormatValue[0] = 1;
            ret->m_granIxToFormatValue[1] = 12;
            ret->m_granIxToFormatValue[2] = 2;
            ret->m_granIxToFormatValue[3] = 13;
            ret->m_granIxToFormatValue[4] = 3;
            ret->m_granIxToFormatValue[5] = 14;
            ret->m_granIxToFormatValue[6] = 4;
            ret->m_granIxToFormatValue[7] = 5;
            ret->m_granIxToFormatValue[8] = 6;
            ret->m_granIxToFormatValue[9] = 7;
            ret->m_granIxToFormatValue[10] = 15;
            ret->m_granIxToFormatValue[11] = 8;
            ret->m_granIxToFormatValue[12] = 9;
            ret->m_granIxToFormatValue[13] = 10;
            ret->m_granIxToFormatValue[14] = 11;

        }
        else
        {
            ret->m_granIxToFormatValue[0] = 1;
            ret->m_granIxToFormatValue[1] = 2;
            ret->m_granIxToFormatValue[2] = 3;
            ret->m_granIxToFormatValue[3] = 4;
            ret->m_granIxToFormatValue[4] = 5;
            ret->m_granIxToFormatValue[5] = 6;
            ret->m_granIxToFormatValue[6] = 7;
            ret->m_granIxToFormatValue[7] = 8;
            ret->m_granIxToFormatValue[8] = 9;
            ret->m_granIxToFormatValue[9] = 10;
            ret->m_granIxToFormatValue[10] = 11;
        }
    }

    if (device->hw_res->lpm_middle_level_num_gran)
    {
        ret->m_middleLevelGranIxToFormatValue[0] = 1;
        ret->m_middleLevelGranIxToFormatValue[1] = 2;
        ret->m_middleLevelGranIxToFormatValue[2] = 3;
        ret->m_middleLevelGranIxToFormatValue[3] = 4;
        ret->m_middleLevelGranIxToFormatValue[4] = 5;
        ret->m_middleLevelGranIxToFormatValue[5] = 6;
        ret->m_middleLevelGranIxToFormatValue[6] = 7;
        ret->m_middleLevelGranIxToFormatValue[7] = 8;
        ret->m_middleLevelGranIxToFormatValue[8] = 9;
        ret->m_middleLevelGranIxToFormatValue[9] = 10;
        ret->m_middleLevelGranIxToFormatValue[10] = 11;
    }

    /*
     * Currently we are supporting full width LSN only with per LSN Gran
     */
    if (ret->m_isFullWidthLsn)
        kaps_sassert(ret->m_isPerLpuGran == 0);

    /*Zero value brick will have all the contents in it set to 0*/
    kaps_memset(ret->zero_value_array, 0, KAPS_BKT_WIDTH_8);

    return ret;
}

static void
kaps_lsn_mc_settings_pvt_dtor(
    kaps_lsn_mc_settings * self)
{
    uint32_t i;
    for (i = 0; i < KAPS_MAX_NUM_STORED_LSN_INFO; ++i)
    {
        if (self->m_pTmpMlpMemInfoBuf[i])
        {
            kaps_nlm_allocator_free(self->m_pAlloc, self->m_pTmpMlpMemInfoBuf[i]);
        }
    }
}

void
kaps_lsn_mc_settings_destroy(
    kaps_lsn_mc_settings * self)
{
    if (self)
    {
        kaps_lsn_mc_settings_pvt_dtor(self);
        kaps_nlm_allocator_free(self->m_pAlloc, (void *) self);
    }
}

NlmErrNum_t
kaps_lsn_mc_add_extra_brick_for_joined_udcs(
    kaps_lsn_mc * self,
    NlmReasonCode * o_reason)
{
    uint32_t numPfxs = 0;
    kaps_lpm_lpu_brick *newBrick = NULL, *curBrick, *lastBrick;
    uint32_t brickIter;
    NlmErrNum_t errNum;
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    kaps_nlm_allocator *alloc_p = settings->m_pAlloc;
    struct kaps_ad_db *ad_db = NULL;

    if (!settings->m_isJoinedUdc)
        return NLMERR_OK;

    if (self->m_mlpMemInfo->size <= self->m_numLpuBricks)
        return NLMERR_OK;

    kaps_assert(self->m_mlpMemInfo->size == self->m_numLpuBricks + 1, "UDA chunk has more than 1 extra brick");

    /*
     * Suppose there is a failure in adding the extra brick, we simply return success without throwing an error. In
     * this case we will simply be using 1 less brick than possible
     */

    curBrick = self->m_lpuList;
    lastBrick = NULL;
    while (curBrick)
    {
        lastBrick = curBrick;
        curBrick = curBrick->m_next_p;
    }

    newBrick = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_lpu_brick));
    if (!newBrick)
    {
        return NLMERR_OK;
    }

    /*
     * Copy the last brick details into the new brick, since the last brick and new brick are joined
     */
    newBrick->m_gran = lastBrick->m_gran;
    newBrick->m_granIx = lastBrick->m_granIx;
    ad_db = lastBrick->ad_db;

    newBrick->m_maxCapacity =
        kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, lastBrick->meta_priority, lastBrick->m_gran);
    numPfxs = newBrick->m_maxCapacity;

    newBrick->m_pfxes = kaps_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_pfx_bundle *));
    if (!newBrick->m_pfxes)
    {
        kaps_nlm_allocator_free(alloc_p, newBrick);
        return NLMERR_OK;
    }

    brickIter = self->m_numLpuBricks;
    if (self->m_pSettings->m_isPerLpuGran)
    {
        errNum = kaps_lsn_mc_acquire_resources_per_lpu(self, newBrick, brickIter, numPfxs, ad_db, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_nlm_allocator_free(alloc_p, newBrick->m_pfxes);
            kaps_nlm_allocator_free(alloc_p, newBrick);
            return NLMERR_OK;
        }
    }

    newBrick->ad_db = ad_db;
    newBrick->meta_priority = lastBrick->meta_priority;
    newBrick->m_next_p = NULL;

    /*
     * Insert the new brick at the end 
     */
    lastBrick->m_next_p = newBrick;

    self->m_numLpuBricks++;

    kaps_lsn_mc_incr_num_bricks_allocated_for_all_lsns(settings, 1);

    self->m_nLsnCapacity += newBrick->m_maxCapacity;

    return NLMERR_OK;
}



void
kaps_lsn_mc_check_and_free_ix(
    kaps_lsn_mc * self,
    struct kaps_ix_chunk *ixChunk,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum)
{
    struct kaps_ix_mgr *ix_mgr;
    
    if (self->m_pSettings->m_isHardwareMappedIx)
    {
        if (ixChunk->ad_info)
        {
            kaps_nlm_allocator_free(self->m_pSettings->m_pAlloc, ixChunk->ad_info);
        }
        kaps_nlm_allocator_free(self->m_pSettings->m_pAlloc, ixChunk);
        return;
    }

    ix_mgr = kaps_lsn_mc_get_ix_mgr_for_lsn_pfx(self, ixChunk->ad_info->ad_db, uda_chunk, brickNum);
    
    kaps_ix_mgr_free(ix_mgr, ixChunk);
}

uint16_t
kaps_lsn_mc_get_next_max_gran(
    kaps_lsn_mc_settings * settings,
    uint16_t gran,
    uint16_t next)
{
    uint16_t granIx = settings->m_lengthToGranIx[gran];

    if (gran > 128)
        return 0;

    granIx += next;

    if (granIx >= settings->lpm_num_gran)
        return 0;

    return settings->m_granIxToGran[granIx];
}

uint16_t
kaps_lsn_mc_calc_max_pfx_in_lpu_brick(
    kaps_lsn_mc_settings * settings,
    struct kaps_ad_db * ad_db,
    uint32_t has_reservedSlot,
    uint16_t meta_priority,
    uint16_t gran)
{
    uint16_t granIx = settings->m_lengthToGranIx[gran];
    uint16_t maxNumPfxInBrickForGran = settings->m_maxPfxInBrickForGranIx[granIx];


    return maxNumPfxInBrickForGran;
}


uint32_t
kaps_lsn_mc_calc_max_triggers_in_brick(
    kaps_lsn_mc_settings * settings,
    uint16_t gran)
{
    uint16_t granIx = settings->m_middleLevelLengthToGranIx[gran];

    return settings->m_maxTriggersInBrickForGranIx[granIx];
}


uint32_t
kaps_lsn_mc_get_index(
    kaps_lsn_mc * self,
    uint32_t relative_index)
{
    kaps_lpm_lpu_brick *curLpuBrick;
    uint32_t actual_index = 0;
    if (self->m_pSettings->m_isPerLpuGran)
    {
        curLpuBrick = self->m_lpuList;
        while (curLpuBrick)
        {
            if (relative_index < curLpuBrick->m_maxCapacity)
            {
                actual_index = curLpuBrick->m_ixInfo->start_ix + relative_index;
                break;
            }
            relative_index -= curLpuBrick->m_maxCapacity;
            curLpuBrick = curLpuBrick->m_next_p;
        }
        if (curLpuBrick)
            return actual_index;
        else
        {
            kaps_sassert(0);
            return 0;
        }
    }
    else
    {
        actual_index = self->m_nAllocBase + relative_index;
        return actual_index;
    }
}

uint32_t
kaps_lsn_mc_get_index_in_lsn(
    kaps_lsn_mc * self,
    uint32_t actual_index)
{
    kaps_lpm_lpu_brick *curLpuBrick;
    uint32_t relative_index = 0;
    if (self->m_pSettings->m_isPerLpuGran)
    {
        curLpuBrick = self->m_lpuList;
        while (curLpuBrick)
        {
            uint32_t maxPfxInLpu = curLpuBrick->m_maxCapacity;
            if (curLpuBrick->m_underAllocatedIx)
                maxPfxInLpu = curLpuBrick->m_ixInfo->size;
            if (actual_index >= curLpuBrick->m_ixInfo->start_ix
                && actual_index < curLpuBrick->m_ixInfo->start_ix + maxPfxInLpu)
            {
                relative_index += actual_index - curLpuBrick->m_ixInfo->start_ix;
                break;
            }
            relative_index += curLpuBrick->m_maxCapacity;
            curLpuBrick = curLpuBrick->m_next_p;
        }
        if (curLpuBrick)
            return relative_index;
        else
        {
            kaps_sassert(0);
            return 0;
        }
    }
    else
    {
        relative_index = actual_index - self->m_nAllocBase;
        return relative_index;
    }
}

void
kaps_lsn_mc_free_single_lpu_brick(
    kaps_lpm_lpu_brick * curLpuBrick,
    kaps_nlm_allocator * alloc)
{
    kaps_nlm_allocator_free(alloc, curLpuBrick->m_pfxes);
    kaps_nlm_allocator_free(alloc, curLpuBrick);
}

void
kaps_lsn_mc_free_lpu_bricks(
    kaps_lpm_lpu_brick * curLpuBrick,
    kaps_nlm_allocator * alloc)
{
    kaps_lpm_lpu_brick *temp;

    while (curLpuBrick)
    {
        temp = curLpuBrick->m_next_p;

        kaps_lsn_mc_free_single_lpu_brick(curLpuBrick, alloc);

        curLpuBrick = temp;
    }
}

static kaps_lsn_mc *
kaps_lsn_mc_pvt_ctor(
    kaps_lsn_mc * self,
    kaps_lsn_mc_settings * settings)
{
    self->m_pSettings = settings;
    self->m_bIsNewLsn = 1;

    return self;
}

kaps_lsn_mc *
kaps_lsn_mc_create(
    kaps_lsn_mc_settings * settings,
    struct kaps_lpm_trie * pTrie,
    uint32_t depth)
{
    kaps_lsn_mc *ret = (kaps_lsn_mc *) kaps_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_lsn_mc));

    kaps_lsn_mc_pvt_ctor(ret, settings);

    ret->m_pTrie = pTrie;
    ret->m_pTbl = pTrie->m_tbl_ptr;

    ret->m_nDepth = depth;
    ret->m_nLopoff = depth;

    ret->m_lsnId = settings->m_lsnid++;

    return ret;
}

NlmErrNum_t
kaps_lsn_mc_pvt_allocate_mlp(
    kaps_lsn_mc * self,
    uint8_t deviceNum,
    uint8_t rqtNumLpu,
    struct uda_mem_chunk ** mlpMemInfo_pp,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_uda_mgr *pMlpMemMgr = self->m_pSettings->m_pMlpMemMgr[self->m_devid];
    kaps_status status;

    status = kaps_uda_mgr_alloc(pMlpMemMgr, rqtNumLpu, self->m_pTbl->m_fibTblMgr_p->m_devMgr_p, self,
                                mlpMemInfo_pp, self->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db);

    errNum = kaps_trie_convert_kbp_status_to_err_num(status, o_reason);

    if (errNum == NLMERR_OK)
    {
        kaps_lsn_mc_incr_num_bricks_allocated_for_all_lsns(self->m_pSettings, rqtNumLpu);
    }

    return errNum;
}

void
kaps_lsn_mc_free_resources(
    kaps_lsn_mc * self)
{
    if (self->m_bAllocedResource)
    {
        if (self->m_pSettings->m_isPerLpuGran)
            kaps_lsn_mc_free_resources_per_lpu(self);
        else
        {
            kaps_lsn_mc_check_and_free_ix(self, self->m_ixInfo, self->m_mlpMemInfo, 0);
            self->m_ixInfo = NULL;
        }

        kaps_lsn_mc_decr_num_bricks_allocated_for_all_lsns(self->m_pSettings, self->m_mlpMemInfo->size);
        kaps_uda_mgr_free(self->m_pSettings->m_pMlpMemMgr[self->m_devid], self->m_mlpMemInfo);
        self->m_mlpMemInfo = NULL;

        self->m_bAllocedResource = 0;
        self->m_nAllocBase = 0;
    }
}

void
kaps_lsn_mc_free_resources_per_lpu(
    kaps_lsn_mc * self)
{
    kaps_lpm_lpu_brick *cur_lpu_brick = self->m_lpuList;
    uint32_t brickNum;

    if (self->m_bAllocedResource)
    {
        brickNum = 0;
        while (cur_lpu_brick)
        {
            if (cur_lpu_brick->m_ixInfo)
            {
                kaps_lsn_mc_check_and_free_ix(self, cur_lpu_brick->m_ixInfo, self->m_mlpMemInfo, brickNum);
                cur_lpu_brick->m_ixInfo = NULL;
            }

            brickNum++;
            cur_lpu_brick = cur_lpu_brick->m_next_p;
        }

        self->m_nAllocBase = 0;
    }
}

void
kaps_lsn_mc_free_mlp_resources(
    kaps_lsn_mc * self,
    struct uda_mem_chunk *mlpMemInfo_p,
    NlmReasonCode * o_reason)
{
    if (self->m_bAllocedResource)
    {
        kaps_lsn_mc_decr_num_bricks_allocated_for_all_lsns(self->m_pSettings, mlpMemInfo_p->size);
        kaps_uda_mgr_free(self->m_pSettings->m_pMlpMemMgr[self->m_devid], mlpMemInfo_p);
    }
}

static void
kaps_lsn_mc_pvt_dtor(
    kaps_lsn_mc * self)
{
    kaps_lsn_mc_free_lpu_bricks(self->m_lpuList, self->m_pSettings->m_pAlloc);
    self->m_lpuList = NULL;
    self->m_nNumPrefixes = 0;
}

void
kaps_lsn_mc_destroy(
    kaps_lsn_mc * self)
{
    if (self)
    {
        kaps_nlm_allocator *alloc = self->m_pSettings->m_pAlloc;

        /*
         * Free resources allocated to this LSN like memory on the chip, index range 
         */
        kaps_lsn_mc_free_resources(self);

        kaps_lsn_mc_pvt_dtor(self);
        kaps_nlm_allocator_free(alloc, (void *) self);
    }
}

void
kaps_lsn_mc_update_prefix(
    kaps_lsn_mc * self,
    kaps_pfx_bundle * b,
    uint32_t oldix,
    uint32_t nextix)
{
    kaps_fib_tbl *tbl;
    kaps_fib_prefix_index_changed_app_cb_t appCB;
    void *appData;
    struct kaps_db *db = self->m_pTrie->m_tbl_ptr->m_db;

    tbl = self->m_pTbl;

    /*
     * User provided index changed callback function 
     */
    appCB = self->m_pSettings->m_pAppCB;
    appData = self->m_pSettings->m_pAppData;

    b->m_nIndex = nextix;
    if (oldix == KAPS_LSN_NEW_INDEX)
    {
        /*
         * Call the user Index changed CB here (PHMD) 
         */
        if (oldix != nextix)
        {

            if (appCB)
                appCB(appData, tbl, b, oldix, nextix);

            if (db->device->nv_ptr && db->device->issu_status == KAPS_ISSU_INIT && nextix != KAPS_LSN_NEW_INDEX)
                db->fn_table->cr_entry_shuffle(db, (struct kaps_entry *) b->m_backPtr, 1);
        }

    }
    else
    {
        /*
         * Call the user index changed CB here (PHMD) 
         */
        if (oldix != nextix)
        {

            if (appCB)
                appCB(appData, tbl, b, oldix, nextix);

            if (db->device->nv_ptr && db->device->issu_status == KAPS_ISSU_INIT && nextix != KAPS_LSN_NEW_INDEX)
                db->fn_table->cr_entry_shuffle(db, (struct kaps_entry *) b->m_backPtr, 1);
        }
    }
}

uint16_t
kaps_lsn_mc_get_first_lpu_granIx(
    kaps_lsn_mc * self)
{
    kaps_lpm_lpu_brick *curLpuBrick = self->m_lpuList;

    return curLpuBrick->m_granIx;
}

uint16_t
kaps_lsn_mc_get_first_lpu_gran(
    kaps_lsn_mc * self)
{
    kaps_lpm_lpu_brick *curLpuBrick = self->m_lpuList;

    return curLpuBrick->m_gran;
}

uint16_t
kaps_lsn_mc_compute_gran(
    kaps_lsn_mc_settings * settings,
    uint32_t lengthAfterLopoff,
    uint16_t * o_granIx)
{
    uint16_t gran;

    /*
     * Add 1 bit for MPE encoding 
     */
    ++lengthAfterLopoff;

    gran = settings->m_lengthToGran[lengthAfterLopoff];
    if (o_granIx)
    {
        *o_granIx = settings->m_lengthToGranIx[lengthAfterLopoff];
    }

    return gran;
}

void
kaps_lsn_mc_free_prefixes_safe(
    kaps_lsn_mc * self)
{
    kaps_nlm_allocator *alloc = self->m_pSettings->m_pAlloc;
    kaps_lpm_lpu_brick *curLpuBrick, *temp;

    kaps_lsn_mc_free_resources(self);

    curLpuBrick = self->m_lpuList;
    while (curLpuBrick)
    {
        temp = curLpuBrick->m_next_p;
        kaps_nlm_allocator_free(alloc, curLpuBrick->m_pfxes);
        kaps_nlm_allocator_free(alloc, curLpuBrick);

        curLpuBrick = temp;
    }
    self->m_lpuList = NULL;
    self->m_nNumPrefixes = 0;
}

static NlmErrNum_t
kaps_lsn_mc_update_indexes(
    kaps_lsn_mc * self)
{
    uint32_t i;
    uint32_t oldix, newix;
    kaps_lpm_lpu_brick *curLpuBrick;
    uint32_t maxNumPfxPerLpuBrick;

    if (self->m_pTbl->m_fibTblMgr_p->m_devMgr_p->type == KAPS_DEVICE_KAPS)
    {
        /*
         * We are asserting here since we don't support hit bits in this function for KAPS. In case in the future, we
         * call this function for KAPS, then we should add support for hit bits in this function
         */
        kaps_assert(0, "Update Indexes should not be called for this device type\n");
    }

    /*
     * Walk through the entire LSN, take each prefix, call user IndexChangeCB 
     */
    curLpuBrick = self->m_lpuList;
    newix = self->m_nAllocBase;

    while (curLpuBrick)
    {
        maxNumPfxPerLpuBrick = curLpuBrick->m_maxCapacity;
        if (self->m_pSettings->m_isPerLpuGran)
            newix = curLpuBrick->m_ixInfo->start_ix;

        if (curLpuBrick->m_numPfx == 0)
        {
            newix += maxNumPfxPerLpuBrick;
            curLpuBrick = curLpuBrick->m_next_p;
            continue;
        }

        for (i = 0; i < maxNumPfxPerLpuBrick; i++)
        {
            kaps_pfx_bundle *b = curLpuBrick->m_pfxes[i];

            if (b)
            {
                oldix = KAPS_PFX_BUNDLE_GET_INDEX(b);
                b->m_nIndex = newix;
                kaps_lsn_mc_update_prefix(self, b, oldix, newix);
            }
            ++newix;
        }

        curLpuBrick = curLpuBrick->m_next_p;
    }
    return NLMERR_OK;
}

static void
kaps_lsn_mc_remove_entry_helper(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry **entrySlotInHash)
{
    void *tmp_ptr;
    kaps_trie_node *trienode = NULL;
    uint32_t canFreeLsnResource;

    tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);
    kaps_memcpy(&trienode, tmp_ptr, sizeof(kaps_trie_node *));

    canFreeLsnResource = 1;

    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds)
    {
        /*If we have prefix that exactly matches the LSN depth, then we
        will be storing this prefix in trienode->m_iitLmpsofarPfx_p and not in the LSN. 
        We have to store this prefix in the ADS. So we need the trigger to be present.
        So we can't release this LSN*/
        if (trienode->m_iitLmpsofarPfx_p &&
            trienode->m_iitLmpsofarPfx_p->m_nPfxSize == trienode->m_depth)
        {
            canFreeLsnResource = 0;
        }
    }

    if (!self->m_nNumPrefixes && canFreeLsnResource)
    {
        kaps_lsn_mc_free_resources(self);
        self->m_pTbl = 0;
    }
}

static NlmErrNum_t
kaps_lsn_mc_pvt_remove_entry_normal(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * curBrick,
    kaps_pfx_bundle * found,
    uint32_t foundix,
    struct kaps_lpm_entry **entrySlotInHash,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl *tbl;
    kaps_fib_prefix_index_changed_app_cb_t appCB;
    kaps_pfx_bundle *pfx;
    void *appData;

    uint32_t oldIndex = 0;

    tbl = self->m_pTbl;

    /*
     * User provided index changed callback function 
     */
    appCB = self->m_pSettings->m_pAppCB;
    appData = self->m_pSettings->m_pAppData;

    pfx = curBrick->m_pfxes[foundix];

    curBrick->m_pfxes[foundix] = NULL;

    curBrick->m_numPfx--;

    self->m_nNumPrefixes--;

    oldIndex = KAPS_PFX_BUNDLE_GET_INDEX(found);

    if (appCB)
    {
        appCB(appData, tbl, pfx, oldIndex, KAPS_LSN_NEW_INDEX);
    }

    NLM_STRY(kaps_lsn_mc_delete_entry_in_hw(self, KAPS_PFX_BUNDLE_GET_INDEX(found), o_reason));

    kaps_lsn_mc_remove_entry_helper(self, entrySlotInHash);

    return NLMERR_OK;
}

struct uda_mem_chunk *
kaps_lsn_mc_get_uda_chunk(
    void *self)
{
    kaps_lsn_mc *lsn = (kaps_lsn_mc *) self;

    return lsn->m_mlpMemInfo;
}

static NlmErrNum_t
kaps_lsn_mc_try_fast_insert(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_lpm_lpu_brick *curLpuBrick;
    int32_t found;
    kaps_pfx_bundle *newPfxBundle;
    uint32_t indexInLsn, whereInLpu;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device;
    kaps_pfx_bundle *oldPfxBundle;
    kaps_fib_prefix_index_changed_app_cb_t appCB = self->m_pSettings->m_pAppCB;
    void *appData = self->m_pSettings->m_pAppData;
    NlmErrNum_t errNum;
    uint32_t doSearch = 0;
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    (void) settings;

    *newPfx_pp = NULL;
    
    device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;

    ad_db = NULL;
    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds 
        && self->m_nDepth == entryToInsert->pfx_bundle->m_nPfxSize)
    {
        kaps_trie_node *trienode = NULL;
        
        void *tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);
        kaps_memcpy(&trienode, tmp_ptr, sizeof(kaps_trie_node *));

        entryToInsert->pfx_bundle->m_nIndex = KAPS_STRICT_LMPSOFAR_INDEX;


        entryToInsert->pfx_bundle->m_isLmpsofarPfx = 1;

        /*Store the prefix in the trienode->m_iitLmpsofarPfx_p and not in the LSN*/
        trienode->m_iitLmpsofarPfx_p = entryToInsert->pfx_bundle;

        /*We have to update the ADS so that the Lmpsofar is stored in the ADS*/
        errNum = kaps_lsn_mc_update_iit(self, o_reason);

        *newPfx_pp = entryToInsert->pfx_bundle;

        return errNum;
    }

    /*
     * Check if the prefix can fit into one of the existing LPUs 
     */
    curLpuBrick = self->m_lpuList;
    found = 0;
    indexInLsn = 0;


    while (curLpuBrick)
    {
        /*
         * Add 1 to prefix length to account for MPE encoding 
         */
        if ((curLpuBrick->m_gran + self->m_nLopoff) >= (uint32_t) (entryToInsert->pfx_bundle->m_nPfxSize + 1))
        {
            if (curLpuBrick->m_numPfx < curLpuBrick->m_maxCapacity)
            {

                if (curLpuBrick->meta_priority == entryToInsert->meta_priority)
                {

                    if (entryToInsert->ad_handle && curLpuBrick->ad_db
                        && curLpuBrick->ad_db->db_info.seq_num != KAPS_GET_AD_SEQ_NUM(entryToInsert->ad_handle))
                    {
                        indexInLsn += curLpuBrick->m_maxCapacity;
                        curLpuBrick = curLpuBrick->m_next_p;
                        continue;
                    }

                    if (curLpuBrick->m_hasReservedSlot)
                    {
                        /*
                         * If only last location is free in the entire brick and the brick has a reserved slot then 
                         * we can't fit the prefix in the brick, since we are reserving the reserved slot for the
                         * lmpsofar prefix 
                         */
                        if (curLpuBrick->m_pfxes[curLpuBrick->m_maxCapacity - 1] == NULL
                            && (curLpuBrick->m_numPfx + 1 == curLpuBrick->m_maxCapacity))
                        {
                            indexInLsn += curLpuBrick->m_maxCapacity;
                            curLpuBrick = curLpuBrick->m_next_p;
                            continue;
                        }
                    }

                    found = 1;
                    break;
                }
            }
        }

        indexInLsn += curLpuBrick->m_maxCapacity;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (!found)
    {
        return NLMERR_OK;
    }


    whereInLpu = 0;
    doSearch = 1;
    if (curLpuBrick->m_hasReservedSlot)
    {
        if (entryToInsert->pfx_bundle->m_nPfxSize == self->m_nDepth && !self->m_pSettings->m_onlyPfxCopyInReservedSlot)
        {
            whereInLpu = curLpuBrick->m_maxCapacity - 1;
            doSearch = 0;
            oldPfxBundle = curLpuBrick->m_pfxes[whereInLpu];

            if (oldPfxBundle)
            {

                if (appCB)
                    appCB(appData, self->m_pTbl, oldPfxBundle, oldPfxBundle->m_nIndex, KAPS_LSN_NEW_INDEX);

                /*
                 * Destroy the old prefix bundle here
                 */
                kaps_pfx_bundle_destroy(oldPfxBundle, self->m_pSettings->m_pAlloc);
                curLpuBrick->m_pfxes[whereInLpu] = NULL;
                curLpuBrick->m_numPfx--;
                self->m_nNumPrefixes--;
            }
        }
    }

    if (doSearch)
    {
        for (; whereInLpu < curLpuBrick->m_maxCapacity; whereInLpu++)
        {
            if (!curLpuBrick->m_pfxes[whereInLpu])
            {
                break;
            }
        }
    }

    if (curLpuBrick->m_hasReservedSlot && whereInLpu == curLpuBrick->m_maxCapacity - 1)
    {
        if (self->m_pSettings->m_onlyPfxCopyInReservedSlot)
        {
            kaps_assert(0, "Incorrect prefix bundle in the reserved slot\n");
        }
        else
        {
            kaps_assert(entryToInsert->pfx_bundle->m_nPfxSize == self->m_nDepth,
                        "Incorrect prefix bundle in the reserved slot\n");
        }
    }

    if (curLpuBrick->m_pfxes[whereInLpu])
    {
        kaps_assert(0, "LPU Brick should be having a hole, but unable to find it\n");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    newPfxBundle->m_nIndex = kaps_lsn_mc_get_index(self, indexInLsn + whereInLpu);

    curLpuBrick->m_pfxes[whereInLpu] = newPfxBundle;

    curLpuBrick->m_numPfx++;

    self->m_nNumPrefixes++;

    
    if (!curLpuBrick->ad_db)
        curLpuBrick->ad_db = ad_db;


    kaps_sassert(curLpuBrick->ad_db == ad_db);
    kaps_sassert(curLpuBrick->meta_priority == entryToInsert->meta_priority);


    errNum = kaps_lsn_mc_write_entry_to_hw(self, curLpuBrick->m_gran, newPfxBundle, KAPS_LSN_NEW_INDEX,
                                           KAPS_PFX_BUNDLE_GET_INDEX(newPfxBundle), 0, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static int32_t
kaps_lsn_mc_pvt_grow(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    uint16_t newGran,
    int32_t * isGrowUp,
    int32_t * isNewIndexRange_p)
{
    struct kaps_uda_mgr *pMlpMemMgr = self->m_pSettings->m_pMlpMemMgr[self->m_devid];
    struct uda_mem_chunk *mlpMemInfo_p = self->m_mlpMemInfo;
    struct kaps_ix_mgr *pIxMgr;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_lsn_mc_settings *lsn_settings = self->m_pSettings;
    uint32_t ixGrowDownSize = 0, ixGrowUpSize = 0;
    uint32_t mlpLpuGrowDownSize = 0, mlpLpuGrowUpSize = 0;
    uint16_t maxNumPfxInNewLpuBrick = 0;
    int32_t isGrowComplete = 0, isGrowUpAllowed;
    uint8_t ix_chunk_available = 0;
    struct kaps_ad_db *ad_db = NULL;
    kaps_status status = KAPS_OK;
    uint32_t actualMlpGrowSize, actualIxGrowSize;
    uint32_t can_ix_grow, can_mlp_grow;
    uint32_t udc_no;
    struct kaps_db *db = self->m_pTbl->m_db;

    (void) db;

    *isNewIndexRange_p = 0;
    
    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);


    maxNumPfxInNewLpuBrick =
        kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority, newGran);

    if (self->m_pSettings->m_isUnderAllocateLpuIx)
    {
        uint32_t nextHighGran = kaps_lsn_mc_get_next_max_gran(lsn_settings, newGran, 1);
        if (nextHighGran
            && (self->m_numLpuBricks + 1 >= lsn_settings->m_maxLpuPerLsn - lsn_settings->m_numUnderallocateLpus))
            maxNumPfxInNewLpuBrick =
                kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority,
                                                      nextHighGran);
    }

    pIxMgr = kaps_lsn_mc_get_ix_mgr_for_lsn_pfx(self, ad_db, self->m_mlpMemInfo, 0);

    if (!self->m_pSettings->m_isPerLpuGran)
        kaps_ix_mgr_check_grow_down(pIxMgr, self->m_ixInfo, &ixGrowDownSize);

    kaps_uda_mgr_check_grow_down(pMlpMemMgr, mlpMemInfo_p, &mlpLpuGrowDownSize);

    can_ix_grow = 0;
    actualIxGrowSize = maxNumPfxInNewLpuBrick;
    if (ixGrowDownSize >= maxNumPfxInNewLpuBrick || ix_chunk_available)
    {
        can_ix_grow = 1;
    }

    can_mlp_grow = 0;
    actualMlpGrowSize = 1;
    if (mlpLpuGrowDownSize >= 1)
    {
        can_mlp_grow = 1;
    }

    if (lsn_settings->m_isJoinedUdc && mlpLpuGrowDownSize >= 2)
    {
        udc_no = kaps_uda_mgr_compute_brick_udc(pMlpMemMgr, mlpMemInfo_p, mlpMemInfo_p->size);

        if (pMlpMemMgr->config.joined_udcs[udc_no + 1])
        {
            if (ixGrowDownSize >= 2 * maxNumPfxInNewLpuBrick)
            {
                can_ix_grow = 1;
                actualIxGrowSize = 2 * maxNumPfxInNewLpuBrick;
                actualMlpGrowSize = 2;
            }
            else
            {
                can_ix_grow = 0;
            }
        }
    }

    /*
     * Try to grow the IX space and MLP space down by 1 LPU 
     */
    if (can_ix_grow && can_mlp_grow)
    {
        if (!self->m_pSettings->m_isPerLpuGran)
            status = kaps_ix_mgr_grow_down(pIxMgr, self->m_ixInfo, actualIxGrowSize);

        if (status != KAPS_OK)
        {
            kaps_assert(0, "Ix Grow Down Failed \n");
            return 0;
        }

        /*
         * Try to grow the MLP memory down by 1 LPU 
         */
        status = kaps_kaps_uda_mgr_grow_down(pMlpMemMgr, mlpMemInfo_p, actualMlpGrowSize);
        if (status != KAPS_OK)
            return 0;

        *isGrowUp = 0;
        isGrowComplete = 1;
    }

    isGrowUpAllowed = 1;

    if (lsn_settings->m_isJoinedUdc)
        isGrowUpAllowed = 0;

    /*
     * Try to grow the IX space and MLP space Up by 1 LPU 
     */
    if (!isGrowComplete && isGrowUpAllowed)
    {
        if (!self->m_pSettings->m_isPerLpuGran)
            kaps_ix_mgr_check_grow_up(pIxMgr, self->m_ixInfo, &ixGrowUpSize);

        kaps_uda_mgr_check_grow_up(pMlpMemMgr, mlpMemInfo_p, &mlpLpuGrowUpSize);

        if ((ixGrowUpSize >= maxNumPfxInNewLpuBrick || ix_chunk_available) && mlpLpuGrowUpSize >= 1)
        {
            if (!self->m_pSettings->m_isPerLpuGran)
                status = kaps_ix_mgr_grow_up(pIxMgr, self->m_ixInfo, maxNumPfxInNewLpuBrick);

            if (status != KAPS_OK)
            {
                kaps_assert(0, "Ix Grow Up Failed \n");
                return 0;
            }

            /*
             * Try to grow the MLP memory up by 1 LPU
             */
            status = kaps_kaps_uda_mgr_grow_up(pMlpMemMgr, mlpMemInfo_p, 1);

            if (status != KAPS_OK)
                return 0;

            *isGrowUp = 1;
            isGrowComplete = 1;
        }
    }

    if (!isGrowComplete)
    {
        return 0;
    }

    /*
     * Fill the allocBase and numIxAllocated 
     */
    if (!self->m_pSettings->m_isPerLpuGran)
    {
        self->m_nAllocBase = self->m_ixInfo->start_ix;
        self->m_nNumIxAlloced = (uint16_t) self->m_ixInfo->size;
    }

    self->m_bAllocedResource = 1;

    return 1;
}

static NlmErrNum_t
kaps_lsn_mc_pvt_commit_grow(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    uint16_t newPfxGran,
    uint16_t newPfxGranIx,
    int32_t isGrowUp,
    int32_t isNewIndexRange,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_lpm_lpu_brick *newBrick;
    kaps_lpm_lpu_brick *iterLpuBrick;
    kaps_pfx_bundle *newPfxBundle;
    kaps_lsn_mc_settings *lsn_settings = self->m_pSettings;
    uint32_t indexInLsn, numPfxs = 0;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_status status = KAPS_OK;
    NlmErrNum_t errNum;
    uint32_t oldNumBricks, newNumBricks;

    (void) numPfxs;
    (void) status;

    *newPfx_pp = NULL;

    newBrick = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_lpu_brick));
    if (!newBrick)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    newBrick->m_gran = newPfxGran;
    newBrick->m_granIx = newPfxGranIx;
    newBrick->meta_priority = entryToInsert->meta_priority;
    newBrick->m_maxCapacity = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings,
                                                                    ad_db, 0, entryToInsert->meta_priority, newPfxGran);
    numPfxs = newBrick->m_maxCapacity;
    newBrick->m_pfxes = kaps_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_pfx_bundle *));
    if (!newBrick->m_pfxes)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (self->m_pSettings->m_isUnderAllocateLpuIx)
    {
        uint32_t nextHighGran = kaps_lsn_mc_get_next_max_gran(lsn_settings, newPfxGran, 1);
        if (nextHighGran
            && (self->m_numLpuBricks + 1 >= lsn_settings->m_maxLpuPerLsn - lsn_settings->m_numUnderallocateLpus))
        {
            numPfxs =
                kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority,
                                                      nextHighGran);
            newBrick->m_underAllocatedIx = 1;
        }
    }

    newBrick->m_numPfx = 1;


    newBrick->ad_db = ad_db;

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    indexInLsn = 0;
    if (isGrowUp)
    {
        /*
         * Insert the LPU at the beginning 
         */
        newBrick->m_next_p = self->m_lpuList;
        self->m_lpuList = newBrick;
    }
    else
    {
        /*
         * Insert the LPU at the end 
         */
        iterLpuBrick = self->m_lpuList;

        while (iterLpuBrick->m_next_p)
        {
            indexInLsn += iterLpuBrick->m_maxCapacity;
            iterLpuBrick = iterLpuBrick->m_next_p;
        }

        indexInLsn += iterLpuBrick->m_maxCapacity;
        iterLpuBrick->m_next_p = newBrick;
        newBrick->m_next_p = NULL;
    }

    oldNumBricks = self->m_numLpuBricks;
    self->m_numLpuBricks++;

    newPfxBundle->m_nIndex = kaps_lsn_mc_get_index(self, indexInLsn);

    newBrick->m_pfxes[0] = newPfxBundle;
    self->m_nNumPrefixes++;
    self->m_nLsnCapacity += newBrick->m_maxCapacity;

    kaps_lsn_mc_add_extra_brick_for_joined_udcs(self, o_reason);

    newNumBricks = self->m_numLpuBricks;

    NLM_STRY(kaps_lsn_mc_clear_lpu_for_grow(self, isGrowUp, newNumBricks - oldNumBricks, o_reason));

    /*
     * First write the entry to HW and then write to the IIT for coherency reasons 
     */
    errNum = kaps_lsn_mc_write_entry_to_hw(self, newBrick->m_gran, newPfxBundle, KAPS_LSN_NEW_INDEX,
                                           KAPS_PFX_BUNDLE_GET_INDEX(newPfxBundle), 0, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    if (isNewIndexRange)
    {
        /*
         * Inform the user about the new indexes for the prefixes 
         */
        kaps_lsn_mc_update_indexes(self);
    }

    /*
     * Update the SIT with the correct base address since index range has changed To ensure coherency is maintained,
     * first call UpdateIndexes and then call UpdateSIT 
     */
    NLM_STRY(kaps_lsn_mc_update_iit(self, o_reason));

    *newPfx_pp = newPfxBundle;

    (self->m_pSettings->m_numAllocatedBricksForAllLsns)++;

    return NLMERR_OK;
}

static int32_t
kaps_lsn_mc_pvt_realloc_per_lsn_gran(
    kaps_lsn_mc * self,
    uint16_t gran,
    uint32_t * numLpuBricksRequired_p,
    NlmReasonCode * o_reason)
{
    uint32_t maxPfxPerLpuBrick, totalNumPfx, numLpuBricksRequired;
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    uint32_t granIx, numHoles;
    kaps_lpm_lpu_brick *iterBrick;
    struct kaps_ad_db * ad_db = (struct kaps_ad_db *) settings->m_fibTbl->m_db->common_info->ad_info.ad;

    maxPfxPerLpuBrick = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, 0xF, gran);
    granIx = settings->m_lengthToGranIx[gran];

    numHoles = settings->m_numHolesForGranIx[granIx];
    if (numHoles)
        totalNumPfx = self->m_nNumPrefixes + numHoles;
    else
        totalNumPfx = self->m_nNumPrefixes + 1;

    /*
     * Suppose we have a reserved slot in the brick and there is no prefix in the reserved slot, then we need to take
     * the reserved slot also into account while counting the totalNumPfx since it determines the resulting new number
     * of bricks. So increment totalNumPfx by 1 
     */
    iterBrick = self->m_lpuList;
    while (iterBrick)
    {
        if (iterBrick->m_hasReservedSlot)
        {
            if (!iterBrick->m_pfxes[iterBrick->m_maxCapacity - 1])
            {
                totalNumPfx++;
            }
            break;
        }
        iterBrick = iterBrick->m_next_p;
    }

    numLpuBricksRequired = (totalNumPfx + maxPfxPerLpuBrick - 1) / maxPfxPerLpuBrick;
    if (numLpuBricksRequired > settings->m_maxLpuPerLsn)
    {
        totalNumPfx = self->m_nNumPrefixes + 1;
        numLpuBricksRequired = (totalNumPfx + maxPfxPerLpuBrick - 1) / maxPfxPerLpuBrick;
        if (numLpuBricksRequired > settings->m_maxLpuPerLsn)
        {
            return 0;
        }
    }

    if (self->m_pSettings->dynamic_alloc_enable)
    {
        if (numLpuBricksRequired <= settings->m_maxLpuPerLsn / 2)
            numLpuBricksRequired = settings->m_maxLpuPerLsn / 2;
        else
            numLpuBricksRequired = settings->m_maxLpuPerLsn;
    }

    if (self->m_pSettings->m_isFullWidthLsn)
        numLpuBricksRequired = settings->m_maxLpuPerLsn;

    if (settings->m_isMultiBrickAlloc)
    {
        numLpuBricksRequired = (numLpuBricksRequired + settings->m_numMultiBricks - 1) / settings->m_numMultiBricks;
        numLpuBricksRequired = numLpuBricksRequired * settings->m_numMultiBricks;
        if (numLpuBricksRequired > settings->m_maxLpuPerLsn)
            numLpuBricksRequired = settings->m_maxLpuPerLsn;
    }

    *numLpuBricksRequired_p = numLpuBricksRequired;
    return 1;
}

NlmErrNum_t
kaps_lsn_mc_undo_realloc(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * newBrickList,
    uint32_t startBrickNum)
{
    kaps_lpm_lpu_brick *curBrick, *nextBrick;
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    kaps_nlm_allocator *alloc = settings->m_pAlloc;
    uint32_t curBrickNum = startBrickNum;

    curBrick = newBrickList;
    while (curBrick)
    {
        nextBrick = curBrick->m_next_p;

        if (settings->m_isPerLpuGran && curBrick->m_ixInfo)
        {
            kaps_lsn_mc_check_and_free_ix(self, curBrick->m_ixInfo, 
                                    self->m_mlpMemInfo, curBrickNum);
        }

        if (curBrick->m_pfxes)
            kaps_nlm_allocator_free(alloc, curBrick->m_pfxes);

        kaps_nlm_allocator_free(alloc, curBrick);

        curBrickNum++;
        curBrick = nextBrick;
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_pvt_commit_realloc_normal(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    uint16_t curGran,
    uint16_t curGranIx,
    uint32_t numLpuBricksRequired,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_lpm_lpu_brick *newBrick = NULL, *newListHead;
    kaps_lpm_lpu_brick *iterLpuBrick, *lastLpuBrick;
    kaps_pfx_bundle *newPfxBundle;
    kaps_lsn_mc_settings *lsn_settings = self->m_pSettings;
    uint32_t i, numAdditionalLpuBricks;
    uint16_t additionalLsnCapacity;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t brickIter;
    NlmErrNum_t errNum;
    uint32_t ixRqtSize, maxPfxPerLpuBrick;
    struct kaps_ix_chunk *oldIxChunk;
    uint32_t isSbcPossible = 1;
    struct kaps_db *db = self->m_pTbl->m_db;
    uint32_t originalNumBricks;

    *newPfx_pp = NULL;

    if (numLpuBricksRequired <= self->m_numLpuBricks)
    {
        kaps_assert(0, "reallocing to a smaller size which is incorrect");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }


    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    if (db->num_algo_levels_in_db == 3)
    {
        /*
         * For 3 level KAPS, the UDA is heterogeneous. So we can't simply copy the hit bits from source to destination.
         */
        kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(self);
    }

    maxPfxPerLpuBrick =
        kaps_lsn_mc_calc_max_pfx_in_lpu_brick(lsn_settings, ad_db, 0, entryToInsert->meta_priority, curGran);
    ixRqtSize = numLpuBricksRequired * maxPfxPerLpuBrick;

    errNum = kaps_lsn_mc_acquire_resources(self, ixRqtSize, (uint8_t) numLpuBricksRequired, o_reason);
    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    if (self->m_mlpMemInfo->size > numLpuBricksRequired)
    {
        /*
         * we have got more number of bricks from the UDA than we requested
         */
        numLpuBricksRequired = self->m_mlpMemInfo->size;
    }

    numAdditionalLpuBricks = numLpuBricksRequired - self->m_numLpuBricks;
    newListHead = NULL;
    additionalLsnCapacity = 0;
    brickIter = self->m_numLpuBricks;
    originalNumBricks = self->m_numLpuBricks; 
    for (i = 0; i < numAdditionalLpuBricks; ++i)
    {
        uint32_t numPfxs = 0;

        newBrick = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newBrick)
        {
            kaps_lsn_mc_undo_realloc(self, newListHead, originalNumBricks);
            kaps_lsn_mc_undo_acquire_resources(self);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_next_p = newListHead;
        newListHead = newBrick;

        newBrick->m_gran = curGran;
        newBrick->m_granIx = curGranIx;
        newBrick->m_maxCapacity =
            kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority, curGran);
        numPfxs = newBrick->m_maxCapacity;

        newBrick->m_pfxes = kaps_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            kaps_lsn_mc_undo_realloc(self, newListHead, originalNumBricks);
            kaps_lsn_mc_undo_acquire_resources(self);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        if (self->m_pSettings->m_isUnderAllocateLpuIx 
            && !newBrick->m_hasReservedSlot)
        {
            uint32_t nextHighGran = kaps_lsn_mc_get_next_max_gran(lsn_settings, curGran, 1);
            if (nextHighGran
                && (self->m_numLpuBricks + i >= lsn_settings->m_maxLpuPerLsn - lsn_settings->m_numUnderallocateLpus))
            {
                numPfxs =
                    kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority,
                                                          nextHighGran);
                newBrick->m_underAllocatedIx = 1;
            }
        }

        additionalLsnCapacity += newBrick->m_maxCapacity;

        if (self->m_pSettings->m_isPerLpuGran)
        {
            errNum = kaps_lsn_mc_acquire_resources_per_lpu(self, newBrick, brickIter, numPfxs, ad_db, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_lsn_mc_undo_realloc(self, newListHead, originalNumBricks);
                kaps_lsn_mc_undo_acquire_resources(self);
                return errNum;
            }
        }

        newBrick->ad_db = ad_db;
        newBrick->meta_priority = entryToInsert->meta_priority;
        brickIter++;
    }

    newBrick->m_numPfx = 1;

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    /*
     * If we have hardware mapped IX, if the LSN initially had a size of 4, then these 4 bricks will be reallocated to
     * a new region in the UDA which in turn directly corresponds to a change in their IX. So we have to call
     * kaps_lsn_mc_acquire_resources_per_lpu We also have to find the last brick in all configurations
     */
    iterLpuBrick = self->m_lpuList;
    brickIter = 0;
    lastLpuBrick = NULL;
    while (iterLpuBrick)
    {
        if (self->m_pSettings->m_isPerLpuGran && self->m_pSettings->m_isHardwareMappedIx)
        {
            oldIxChunk = iterLpuBrick->m_ixInfo;
            errNum = kaps_lsn_mc_acquire_resources_per_lpu(self, iterLpuBrick, brickIter, iterLpuBrick->m_maxCapacity,
                                                           iterLpuBrick->ad_db, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_lsn_mc_undo_acquire_resources_per_lpu(self);
                kaps_lsn_mc_undo_realloc(self, newListHead, originalNumBricks);
                kaps_lsn_mc_undo_acquire_resources(self);
                return errNum;
            }

            kaps_lsn_mc_backup_resources_per_lpu(self, iterLpuBrick, oldIxChunk);
        }

        brickIter++;
        lastLpuBrick = iterLpuBrick;
        iterLpuBrick = iterLpuBrick->m_next_p;
    }

    if (self->m_pSettings->m_isPerLpuGran && self->m_pSettings->m_isHardwareMappedIx)
    {
        kaps_lsn_mc_free_old_resources_per_lpu(self);
    }

    kaps_lsn_mc_free_old_resources(self);

    /*
     * Insert the new bricks at the end 
     */
    lastLpuBrick->m_next_p = newListHead;

    self->m_numLpuBricks = (uint8_t) numLpuBricksRequired;

    newBrick->m_pfxes[0] = newPfxBundle;
    self->m_nNumPrefixes++;
    self->m_nLsnCapacity += additionalLsnCapacity;

    isSbcPossible = 1;
    if (db->num_algo_levels_in_db == 3)
    {
        /*
         * For 3 level KAPS, the UDA is heterogeneous. So we can't simply copy the hit bits from source to destination.
         * So we are setting isSbcPossible to 0
         */
        isSbcPossible = 0;
    }

    NLM_STRY(kaps_lsn_mc_commit(self, isSbcPossible, o_reason));

    NLM_STRY(kaps_lsn_mc_update_iit(self, o_reason));

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_pvt_commit_realloc_with_gran_change(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    uint16_t newPfxGran,
    uint16_t newPfxGranIx,
    uint32_t numLpuBricksRequired,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    kaps_nlm_allocator *alloc_p = settings->m_pAlloc;
    kaps_lpm_lpu_brick *newBrick, *headBrick, *oldBrick, *iterBrick;
    kaps_pfx_bundle *newPfxBundle;
    uint32_t lpuIter;
    uint32_t oldLoc, newLoc;
    uint16_t originalNumPfx, newLsnCapacity;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t ixRqtSize, maxPfxPerLpuBrick;
    NlmErrNum_t errNum;

    *newPfx_pp = NULL;


    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    maxPfxPerLpuBrick =
        kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, entryToInsert->meta_priority, newPfxGran);
    ixRqtSize = numLpuBricksRequired * maxPfxPerLpuBrick;

    errNum = kaps_lsn_mc_acquire_resources(self, ixRqtSize, (uint8_t) numLpuBricksRequired, o_reason);
    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    if (self->m_mlpMemInfo->size > numLpuBricksRequired)
    {
        /*
         * we have got more number of bricks from the UDA than we requested
         */
        numLpuBricksRequired = self->m_mlpMemInfo->size;
    }

    headBrick = NULL;
    newLsnCapacity = 0;
    for (lpuIter = 0; lpuIter < numLpuBricksRequired; ++lpuIter)
    {
        newBrick = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newBrick)
        {
            kaps_lsn_mc_undo_realloc(self, headBrick, 0);
            kaps_lsn_mc_undo_acquire_resources(self);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_next_p = headBrick;
        headBrick = newBrick;

        newBrick->m_gran = newPfxGran;
        newBrick->m_granIx = newPfxGranIx;
        newBrick->m_maxCapacity =
            kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority,
                                                  newPfxGran);
        newLsnCapacity += newBrick->m_maxCapacity;

        newBrick->m_pfxes = kaps_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            kaps_lsn_mc_undo_realloc(self, headBrick, 0);
            kaps_lsn_mc_undo_acquire_resources(self);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }
        newBrick->ad_db = ad_db;
    }

    kaps_lsn_mc_free_old_resources(self);

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    /*
     * Insert the old prefixes 
     */
    oldLoc = 0;
    newLoc = 0;
    oldBrick = self->m_lpuList;
    newBrick = headBrick;

    /*
     * If there is a reserved slot in the old bricks, then first copy the prefix bundle in the reserved slot into the
     * first new brick
     */
    iterBrick = oldBrick;
    while (iterBrick)
    {
        if (iterBrick->m_hasReservedSlot)
        {
            newBrick->m_hasReservedSlot = 1;

            newBrick->m_pfxes[newBrick->m_maxCapacity - 1] = iterBrick->m_pfxes[iterBrick->m_maxCapacity - 1];

            if (newBrick->m_pfxes[newBrick->m_maxCapacity - 1])
                newBrick->m_numPfx++;

            break;
        }

        iterBrick = iterBrick->m_next_p;
    }

    while (oldBrick && newBrick)
    {

        /*
         * Since we have already processed the reserved slot, skip it here
         */
        if (oldBrick->m_hasReservedSlot && oldLoc == oldBrick->m_maxCapacity - 1)
        {
            oldLoc = 0;
            oldBrick = oldBrick->m_next_p;
            continue;
        }

        if (newBrick->m_hasReservedSlot && newLoc == newBrick->m_maxCapacity - 1)
        {
            newLoc = 0;
            newBrick = newBrick->m_next_p;
            continue;
        }

        if (oldBrick->m_pfxes[oldLoc])
        {
            newBrick->m_pfxes[newLoc] = oldBrick->m_pfxes[oldLoc];
            newBrick->m_numPfx++;
            newLoc++;
        }

        oldLoc++;
        if (oldLoc >= oldBrick->m_maxCapacity)
        {
            oldLoc = 0;
            oldBrick = oldBrick->m_next_p;
        }

        if (newLoc >= newBrick->m_maxCapacity)
        {
            newLoc = 0;
            newBrick = newBrick->m_next_p;
        }
    }

    /*
     * Insert the new prefix 
     */
    if (newBrick)
    {
        newBrick->m_pfxes[newLoc] = newPfxBundle;
        newBrick->m_numPfx++;
    }

    /*
     * replace the old LPU bricks with the new set of LPU bricks 
     */
    originalNumPfx = self->m_nNumPrefixes;
    kaps_lsn_mc_free_lpu_bricks(self->m_lpuList, alloc_p);

    self->m_lpuList = headBrick;
    self->m_numLpuBricks = (uint8_t) numLpuBricksRequired;
    self->m_nNumPrefixes = originalNumPfx + 1;
    self->m_nLsnCapacity = newLsnCapacity;

    NLM_STRY(kaps_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_lsn_mc_update_iit(self, o_reason));

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_resize_per_lsn_gran(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    uint16_t newPfxGran, curGran = 0, newPfxGranIx = 0, curGranIx;
    kaps_lpm_lpu_brick *curLpuBrick;
    int32_t doGrow, doRealloc, isGrowUp, isNewIndexRange;
    uint32_t lengthAfterLopoff;
    uint32_t numLpuBricksRequired = 0;
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    NlmErrNum_t errNum;
    struct kaps_ad_db *ad_db = (struct kaps_ad_db *) settings->m_fibTbl->m_db->common_info->ad_info.ad;

    *newPfx_pp = NULL;

    lengthAfterLopoff = entryToInsert->pfx_bundle->m_nPfxSize - self->m_nLopoff;
    newPfxGran = kaps_lsn_mc_compute_gran(settings, lengthAfterLopoff, &newPfxGranIx);

    curLpuBrick = self->m_lpuList;
    curGran = curLpuBrick->m_gran;
    curGranIx = curLpuBrick->m_granIx;

    if (newPfxGran <= curGran)
    {
        if (self->m_numLpuBricks >= settings->m_maxLpuPerLsn)
            return NLMERR_OK;

        doGrow = kaps_lsn_mc_pvt_grow(self, entryToInsert, curGran, &isGrowUp, &isNewIndexRange);
        if (doGrow)
        {
            errNum = kaps_lsn_mc_pvt_commit_grow(self, entryToInsert, curGran, curGranIx,
                                                 isGrowUp, isNewIndexRange, newPfx_pp, o_reason);
            return errNum;
        }
        /*
         * Grow was unsuccessful. So try to do a complete realloc 
         */
        doRealloc = kaps_lsn_mc_pvt_realloc_per_lsn_gran(self, curGran, &numLpuBricksRequired, o_reason);
        if (doRealloc)
        {
            NLM_STRY(kaps_lsn_mc_pvt_commit_realloc_normal(self, entryToInsert, curGran, curGranIx,
                                                           numLpuBricksRequired, newPfx_pp, o_reason));
        }
    }
    else
    {

        uint32_t maxNumHighGranPfxInLsn = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, 0xF, newPfxGran)
            * settings->m_maxLpuPerLsn;
        if (maxNumHighGranPfxInLsn < (uint32_t) (self->m_nNumPrefixes + 1))
        {
            return NLMERR_OK;
        }

        kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(self);

        doRealloc = kaps_lsn_mc_pvt_realloc_per_lsn_gran(self, newPfxGran, &numLpuBricksRequired, o_reason);
        if (doRealloc)
        {
            NLM_STRY(kaps_lsn_mc_pvt_commit_realloc_with_gran_change(self, entryToInsert, newPfxGran,
                                                                     newPfxGranIx, numLpuBricksRequired, newPfx_pp,
                                                                     o_reason));
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_pvt_realloc_modifying_chosen_lpu(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * chosenLpuBrick,
    uint32_t chosenBrickIter,
    uint16_t maxGran,
    uint16_t maxGranIx,
    struct kaps_lpm_entry *entryToInsert,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;
    kaps_pfx_bundle *newPfxBundle;
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_pfx_bundle **newPfxesInLpu, **oldPfxesInLpu;
    uint32_t ixRqtSize;
    uint16_t newMaxLpuCapacity, i, j;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_lpm_lpu_brick *curBrick;
    uint32_t curBrickCapacity;
    struct kaps_ix_chunk *oldIxChunk;

    *newPfx_pp = NULL;

    newMaxLpuCapacity = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, chosenLpuBrick->ad_db,
                                                              chosenLpuBrick->m_hasReservedSlot,
                                                              chosenLpuBrick->meta_priority, maxGran);
    newPfxesInLpu = kaps_nlm_allocator_calloc(alloc_p, newMaxLpuCapacity, sizeof(kaps_pfx_bundle *));

    if (!newPfxesInLpu)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (entryToInsert->ad_handle
        && chosenLpuBrick->ad_db->db_info.seq_num != KAPS_GET_AD_SEQ_NUM(entryToInsert->ad_handle))
    {
        kaps_nlm_allocator_free(alloc_p, newPfxesInLpu);
        return NLMERR_OK;
    }

    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(self);
    kaps_lsn_mc_store_old_lsn_info(self, 0);

    ixRqtSize = self->m_nLsnCapacity - chosenLpuBrick->m_maxCapacity + newMaxLpuCapacity;
    errNum = kaps_lsn_mc_acquire_resources(self, ixRqtSize, self->m_numLpuBricks, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_nlm_allocator_free(alloc_p, newPfxesInLpu);
        return errNum;
    }

    if (self->m_pSettings->m_isPerLpuGran)
    {

        if (self->m_pSettings->m_isHardwareMappedIx)
        {
            i = 0;
            curBrick = self->m_lpuList;
            while (curBrick)
            {
                curBrickCapacity = curBrick->m_maxCapacity;
                if (i == chosenBrickIter)
                    curBrickCapacity = newMaxLpuCapacity;

                oldIxChunk = curBrick->m_ixInfo;

                errNum = kaps_lsn_mc_acquire_resources_per_lpu(self, curBrick, i, curBrickCapacity,
                                                               curBrick->ad_db, o_reason);
                if (errNum != NLMERR_OK)
                {
                    kaps_lsn_mc_undo_acquire_resources_per_lpu(self);
                    kaps_lsn_mc_undo_acquire_resources(self);
                    kaps_nlm_allocator_free(alloc_p, newPfxesInLpu);
                    return errNum;
                }

                kaps_lsn_mc_backup_resources_per_lpu(self, curBrick, oldIxChunk);

                ++i;
                curBrick = curBrick->m_next_p;
            }

        }
        else
        {
            KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

            oldIxChunk = chosenLpuBrick->m_ixInfo;

            errNum = kaps_lsn_mc_acquire_resources_per_lpu(self, chosenLpuBrick, chosenBrickIter,
                                                           newMaxLpuCapacity, ad_db, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_lsn_mc_undo_acquire_resources(self);
                kaps_nlm_allocator_free(alloc_p, newPfxesInLpu);
                return errNum;
            }

            kaps_lsn_mc_backup_resources_per_lpu(self, chosenLpuBrick, oldIxChunk);
        }

        self->m_nNumIxAlloced = ixRqtSize;
    }

    kaps_lsn_mc_free_old_resources_per_lpu(self);
    kaps_lsn_mc_free_old_resources(self);

    oldPfxesInLpu = chosenLpuBrick->m_pfxes;

    j = 0;
    for (i = 0; i < chosenLpuBrick->m_maxCapacity; ++i)
    {
        if (oldPfxesInLpu[i])
        {
            if (chosenLpuBrick->m_hasReservedSlot && i == chosenLpuBrick->m_maxCapacity - 1)
            {
                newPfxesInLpu[newMaxLpuCapacity - 1] = oldPfxesInLpu[i];
            }
            else
            {
                newPfxesInLpu[j] = oldPfxesInLpu[i];
                ++j;
            }
        }
    }

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    newPfxesInLpu[j] = newPfxBundle;

    chosenLpuBrick->m_gran = maxGran;
    chosenLpuBrick->m_granIx = maxGranIx;
    chosenLpuBrick->m_maxCapacity = newMaxLpuCapacity;
    chosenLpuBrick->m_pfxes = newPfxesInLpu;
    chosenLpuBrick->m_numPfx++;


    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);


    chosenLpuBrick->ad_db = ad_db;

    kaps_sassert(chosenLpuBrick->meta_priority == entryToInsert->meta_priority);

    self->m_nNumPrefixes++;
    self->m_nLsnCapacity = (uint16_t) ixRqtSize;

    kaps_nlm_allocator_free(alloc_p, oldPfxesInLpu);

    NLM_STRY(kaps_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_lsn_mc_update_iit(self, o_reason));

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static int32_t
kaps_lsn_mc_pvt_realloc_per_lpu_gran(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    uint16_t gran,
    uint32_t * numLpuBricksRequired_p,
    NlmReasonCode * o_reason)
{
    uint32_t maxPfxPerLpuBrick, numAdditionalLpuBricks, numLpuBricksRequired;
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    uint32_t granIx, numHoles;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;

    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    maxPfxPerLpuBrick = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, entryToInsert->meta_priority, gran);
    granIx = settings->m_lengthToGranIx[gran];

    numHoles = settings->m_numHolesForGranIx[granIx];
    if (numHoles == 0)
        numHoles = 1;

    numAdditionalLpuBricks = (numHoles + maxPfxPerLpuBrick - 1) / maxPfxPerLpuBrick;

    if (self->m_numLpuBricks + numAdditionalLpuBricks > settings->m_maxLpuPerLsn)
    {
        numAdditionalLpuBricks = 1;
        if (self->m_numLpuBricks + numAdditionalLpuBricks > settings->m_maxLpuPerLsn)
        {
            return 0;
        }
    }

    numLpuBricksRequired = self->m_numLpuBricks + numAdditionalLpuBricks;

    *numLpuBricksRequired_p = numLpuBricksRequired;
    return 1;
}

static NlmErrNum_t
kaps_lsn_mc_resize_per_lpu_gran(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    uint16_t newPfxGran, maxGran = 0, newPfxGranIx = 0, maxGranIx;
    kaps_lpm_lpu_brick *curLpuBrick, *chosenLpuBrick;
    int32_t doGrow, isGrowUp, isNewIndexRange, doRealloc;
    uint32_t lengthAfterLopoff, numLpuBricksRequired;
    int32_t isLsnFull;
    uint16_t chosenGran = 0, localMaxGran = 0;
    kaps_lsn_mc_settings *lsnSettings = self->m_pSettings;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t targetNumPfx;
    uint32_t curBrickIter, chosenBrickIter;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t sort_and_resize = 0;

    if (device->type == KAPS_DEVICE_KAPS)
        sort_and_resize = 1;

    *newPfx_pp = NULL;

    lengthAfterLopoff = entryToInsert->pfx_bundle->m_nPfxSize - self->m_nLopoff;
    newPfxGran = kaps_lsn_mc_compute_gran(lsnSettings, lengthAfterLopoff, &newPfxGranIx);

    isLsnFull = 1;
    if (self->m_nLsnCapacity > self->m_nNumPrefixes)
    {
        isLsnFull = 0;
    }

    curLpuBrick = self->m_lpuList;
    chosenLpuBrick = NULL;
    curBrickIter = 0;
    chosenBrickIter = 0;
    maxGran = newPfxGran;
    maxGranIx = newPfxGranIx;
    while (curLpuBrick)
    {

        if (curLpuBrick->m_numPfx == 0)
        {
            chosenLpuBrick = curLpuBrick;
            chosenBrickIter = curBrickIter;
            chosenGran = newPfxGran;
            break;
        }

        if (maxGran < curLpuBrick->m_gran)
        {
            maxGran = curLpuBrick->m_gran;
            maxGranIx = curLpuBrick->m_granIx;
        }
        localMaxGran = newPfxGran > curLpuBrick->m_gran ? newPfxGran : curLpuBrick->m_gran;

        if (entryToInsert->ad_handle
            && curLpuBrick->ad_db
            && curLpuBrick->ad_db->db_info.seq_num != KAPS_GET_AD_SEQ_NUM(entryToInsert->ad_handle))
        {
            curLpuBrick = curLpuBrick->m_next_p;
            curBrickIter++;
            continue;
        }

        if (entryToInsert->meta_priority != curLpuBrick->meta_priority)
        {
            curLpuBrick = curLpuBrick->m_next_p;
            curBrickIter++;
            continue;
        }

        /*
         * Add one to accommodate the incoming prefix
         */
        targetNumPfx = curLpuBrick->m_numPfx + 1;

        /*
         * If the reserved slot is not occupied, add one to the targetNumPfx
         */
        if (curLpuBrick->m_hasReservedSlot && !curLpuBrick->m_pfxes[curLpuBrick->m_maxCapacity - 1])
            targetNumPfx++;

        if (kaps_lsn_mc_calc_max_pfx_in_lpu_brick
            (lsnSettings, curLpuBrick->ad_db, curLpuBrick->m_hasReservedSlot, curLpuBrick->meta_priority,
             localMaxGran) >= targetNumPfx)
        {
            if (!chosenLpuBrick)
            {
                chosenLpuBrick = curLpuBrick;
                chosenBrickIter = curBrickIter;
                chosenGran = localMaxGran;
                break;
            }
        }
        ++curBrickIter;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (chosenLpuBrick)
    {
        if (lsnSettings->m_isJoinedUdc)
        {
            errNum = kaps_lsn_mc_sort_and_resize_per_lpu_gran(self, entryToInsert, newPfx_pp, o_reason);
            return errNum;
        }
        else
        {
            struct kaps_ad_db *old_ad_db = chosenLpuBrick->ad_db;
            uint32_t old_meta_priority = chosenLpuBrick->meta_priority;
            uint32_t changed_ad_priority = 0;

            if (chosenLpuBrick->m_numPfx == 0)
            {
                changed_ad_priority = 1;

                KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, chosenLpuBrick->ad_db);

                chosenLpuBrick->meta_priority = entryToInsert->meta_priority;
            }

            errNum = kaps_lsn_mc_pvt_realloc_modifying_chosen_lpu(self, chosenLpuBrick, chosenBrickIter, chosenGran,
                                                                  lsnSettings->m_lengthToGranIx[chosenGran],
                                                                  entryToInsert, newPfx_pp, o_reason);

            if (errNum != NLMERR_OK && changed_ad_priority)
            {
                chosenLpuBrick->ad_db = old_ad_db;
                chosenLpuBrick->meta_priority = old_meta_priority;
            }

            return errNum;
        }
    }

    if (self->m_numLpuBricks >= lsnSettings->m_maxLpuPerLsn)
        return NLMERR_OK;

    if (sort_and_resize)
    {
        errNum = kaps_lsn_mc_sort_and_resize_per_lpu_gran(self, entryToInsert, newPfx_pp, o_reason);
    }
    else
    {

        if (isLsnFull)
        {
            doGrow = kaps_lsn_mc_pvt_grow(self, entryToInsert, maxGran, &isGrowUp, &isNewIndexRange);
            if (doGrow)
            {
                errNum = kaps_lsn_mc_pvt_commit_grow(self, entryToInsert, maxGran, maxGranIx,
                                                     isGrowUp, isNewIndexRange, newPfx_pp, o_reason);
                return errNum;
            }
        }

        doRealloc = kaps_lsn_mc_pvt_realloc_per_lpu_gran(self, entryToInsert, maxGran, &numLpuBricksRequired, o_reason);
        if (doRealloc)
        {
            errNum = kaps_lsn_mc_pvt_commit_realloc_normal(self, entryToInsert, maxGran,
                                                           maxGranIx, numLpuBricksRequired, newPfx_pp, o_reason);
        }
    }

    return errNum;
}

void
kaps_lsn_mc_undo_sort_and_resize(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * oldBrickList,
    uint32_t oldNumLpuBricks,
    uint32_t oldNumPrefix,
    uint32_t oldLsnCapacity,
    struct uda_mem_chunk *oldMlpMemInfo)
{
    kaps_lpm_lpu_brick *newBrickList, *iterLpuBrick, *nextBrick;
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    uint32_t i;

    newBrickList = self->m_lpuList;

    self->m_lpuList = oldBrickList;
    self->m_numLpuBricks = oldNumLpuBricks;
    self->m_nNumPrefixes = oldNumPrefix;
    self->m_nLsnCapacity = oldLsnCapacity;
    self->m_mlpMemInfo = oldMlpMemInfo;

    iterLpuBrick = newBrickList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        kaps_nlm_allocator_free(alloc_p, iterLpuBrick->m_pfxes);
        kaps_nlm_allocator_free(alloc_p, iterLpuBrick);
        ++i;
        iterLpuBrick = nextBrick;
    }

    self->m_bAllocedResource = 1;
}

NlmErrNum_t
kaps_lsn_mc_sort_and_resize_per_lpu_gran(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    kaps_flat_lsn_data *flatData = NULL;
    kaps_lpm_lpu_brick *oldBrickList, *oldLpuBrick, *cur_lpu_brick, *nextBrick;
    uint32_t oldNumLpuBricks, oldNumPrefix, oldLsnCapacity;
    uint32_t i;
    struct uda_mem_chunk *oldMlpMemInfo;
    struct uda_mem_chunk *newMlpMemInfo;
    uint32_t doesLsnFit;

    /*
     * Construct a new LSN
     */
    flatData = kaps_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, o_reason);
    if (!flatData)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    errNum = kaps_lsn_mc_convert_lsn_to_flat_data(self, &(entryToInsert->pfx_bundle), 1, flatData, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(self);
    kaps_lsn_mc_store_old_lsn_info(self, 0);

    oldBrickList = self->m_lpuList;
    oldNumLpuBricks = self->m_numLpuBricks;
    oldNumPrefix = self->m_nNumPrefixes;
    oldLsnCapacity = self->m_nLsnCapacity;
    oldMlpMemInfo = self->m_mlpMemInfo;

    doesLsnFit = 0;
    kaps_lsn_mc_convert_flat_data_to_lsn(flatData, self, &doesLsnFit, o_reason);

    kaps_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, flatData);

    if (!doesLsnFit)
    {
        kaps_lsn_mc_undo_sort_and_resize(self, oldBrickList, oldNumLpuBricks, oldNumPrefix,
                                         oldLsnCapacity, self->m_mlpMemInfo);
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    errNum =
        kaps_lsn_mc_pvt_allocate_mlp(self, (uint8_t) self->m_devid, self->m_numLpuBricks, &newMlpMemInfo, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_lsn_mc_undo_sort_and_resize(self, oldBrickList, oldNumLpuBricks, oldNumPrefix,
                                         oldLsnCapacity, self->m_mlpMemInfo);
        return errNum;
    }

    self->m_mlpMemInfo = newMlpMemInfo;

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_lsn_mc_rearrange_prefixes_for_joined_udcs(self, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    cur_lpu_brick = self->m_lpuList;
    i = 0;
    while (cur_lpu_brick)
    {
        errNum =
            kaps_lsn_mc_acquire_resources_per_lpu(self, cur_lpu_brick, i, cur_lpu_brick->m_maxCapacity,
                                                  cur_lpu_brick->ad_db, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_lsn_mc_free_resources(self);
            kaps_lsn_mc_undo_sort_and_resize(self, oldBrickList, oldNumLpuBricks, oldNumPrefix,
                                             oldLsnCapacity, oldMlpMemInfo);

            return errNum;

        }

        ++i;
        cur_lpu_brick = cur_lpu_brick->m_next_p;
    }

    self->m_nNumIxAlloced = self->m_nLsnCapacity;

    kaps_lsn_mc_add_extra_brick_for_joined_udcs(self, o_reason);

    


    oldLpuBrick = oldBrickList;
    i = 0;
    while (oldLpuBrick)
    {
        nextBrick = oldLpuBrick->m_next_p;
        if (oldLpuBrick->m_ixInfo)
            kaps_lsn_mc_check_and_free_ix(self, oldLpuBrick->m_ixInfo, oldMlpMemInfo, i);

        kaps_nlm_allocator_free(settings_p->m_pAlloc, oldLpuBrick->m_pfxes);
        kaps_nlm_allocator_free(settings_p->m_pAlloc, oldLpuBrick);
        ++i;
        oldLpuBrick = nextBrick;
    }

    kaps_lsn_mc_free_mlp_resources(self, oldMlpMemInfo, o_reason);

    NLM_STRY(kaps_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_lsn_mc_update_iit(self, o_reason));

    kaps_seq_num_gen_set_current_pfx_seq_nr(entryToInsert->pfx_bundle);

    *newPfx_pp = entryToInsert->pfx_bundle;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_move_holes_lsn_gran_to_one_side(
    kaps_lsn_mc * self,
    uint32_t perform_writes,
    NlmReasonCode * o_reason)
{
    kaps_lpm_lpu_brick *brickList[KAPS_HW_MAX_LPUS_PER_LPM_DB];
    kaps_lpm_lpu_brick *curBrick;
    int32_t holeBrick, pfxBrick, holePos, pfxPos, holeIx, pfxIx;
    int32_t i, totalNumBricks, foundHole, foundPrefix, done;
    kaps_pfx_bundle *pfxBundle;

    curBrick = self->m_lpuList;
    i = 0;
    while (curBrick)
    {
        brickList[i] = curBrick;
        ++i;
        curBrick = curBrick->m_next_p;
    }
    totalNumBricks = i;

    holeBrick = 0;
    holePos = 0;
    holeIx = self->m_nAllocBase;

    pfxBrick = totalNumBricks - 1;
    pfxPos = brickList[pfxBrick]->m_maxCapacity - 1;
    pfxIx = self->m_nAllocBase + self->m_nLsnCapacity - 1;

    done = 0;
    while (!done)
    {
        /*
         * Find the first hole from the beginning
         */
        foundHole = 0;
        while (!foundHole && holeBrick < totalNumBricks)
        {
            if (brickList[holeBrick]->m_pfxes[holePos] == NULL)
            {

                /*
                 * Do not move a prefix into the reserved slot even if reserved slot is empty
                 */
                if (brickList[holeBrick]->m_hasReservedSlot && holePos == brickList[holeBrick]->m_maxCapacity - 1)
                {
                    foundHole = 0;
                }
                else
                {
                    foundHole = 1;
                    break;
                }
            }

            holePos++;
            if (holePos >= brickList[holeBrick]->m_maxCapacity)
            {
                holeBrick++;
                holePos = 0;
            }
            holeIx++;
        }

        if (!foundHole)
        {
            done = 1;
            break;
        }

        /*
         * Find the first prefix from the end
         */
        foundPrefix = 0;
        while (!foundPrefix && pfxBrick >= 0)
        {
            if (brickList[pfxBrick]->m_pfxes[pfxPos])
            {
                /*
                 * Do not move the prefix in the reserved slot to a different location
                 */
                if (brickList[pfxBrick]->m_hasReservedSlot && pfxPos == brickList[pfxBrick]->m_maxCapacity - 1)
                {
                    foundPrefix = 0;
                }
                else
                {
                    foundPrefix = 1;
                    break;
                }
            }

            --pfxPos;
            if (pfxPos < 0)
            {
                pfxBrick--;
                if (pfxBrick >= 0)
                    pfxPos = brickList[pfxBrick]->m_maxCapacity - 1;
            }
            --pfxIx;
        }

        if (!foundPrefix)
        {
            done = 1;
            break;
        }

        if ((holeBrick < pfxBrick) || (holeBrick == pfxBrick && holePos < pfxPos))
        {
            /*
             * Move the prefix to the hole
             */
            pfxBundle = brickList[pfxBrick]->m_pfxes[pfxPos];
            brickList[holeBrick]->m_pfxes[holePos] = pfxBundle;
            brickList[pfxBrick]->m_pfxes[pfxPos] = NULL;
            brickList[holeBrick]->m_numPfx++;
            brickList[pfxBrick]->m_numPfx--;

            if (perform_writes)
            {
                uint32_t oldix = pfxIx, newix = holeIx;

                NLM_STRY(kaps_lsn_mc_write_entry_to_hw(self, brickList[holeBrick]->m_gran,
                                                       pfxBundle, oldix, newix, 0, o_reason));

                NLM_STRY(kaps_lsn_mc_delete_entry_in_hw(self, oldix, o_reason));
            }

            holePos++;
            if (holePos >= brickList[holeBrick]->m_maxCapacity)
            {
                holeBrick++;
                holePos = 0;
            }
            ++holeIx;

            --pfxPos;
            if (pfxPos < 0)
            {
                pfxBrick--;
                if (pfxBrick >= 0)
                    pfxPos = brickList[pfxBrick]->m_maxCapacity - 1;
            }
            --pfxIx;

        }
        else
        {
            done = 1;
            break;
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_pvt_commit_shrink(
    kaps_lsn_mc * self,
    int32_t newNumBricks,
    NlmReasonCode * o_reason)
{
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_lpm_lpu_brick *iterLpuBrick, *nextBrick;
    int32_t i;

    /*
     * Insert the LPU at the end 
     */
    iterLpuBrick = self->m_lpuList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        if (i + 1 == newNumBricks)
        {
            iterLpuBrick->m_next_p = NULL;
        }
        else if (i + 1 > newNumBricks)
        {
            kaps_nlm_allocator_free(alloc_p, iterLpuBrick->m_pfxes);
            kaps_nlm_allocator_free(alloc_p, iterLpuBrick);
        }
        ++i;
        iterLpuBrick = nextBrick;
    }

    self->m_numLpuBricks = (uint8_t) newNumBricks;

    self->m_nLsnCapacity = newNumBricks * self->m_lpuList->m_maxCapacity;

    NLM_STRY(kaps_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_lsn_mc_update_iit(self, o_reason));

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_shrink_per_lsn_gran(
    kaps_lsn_mc * self,
    NlmReasonCode * o_reason)
{
    uint16_t numHolesPresent = 0;
    uint16_t totalNumBricks = 0, newNumBricks, numCanFreeBricks;
    NlmErrNum_t errNum;
    kaps_lpm_lpu_brick *curBrick;
    uint32_t numMultiBricksBeforeShrink, numMultiBricksAfterShrink;
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    curBrick = self->m_lpuList;
    while (curBrick)
    {
        numHolesPresent += (curBrick->m_maxCapacity - curBrick->m_numPfx);

        /*
         * Don't count the reserved slot while computing the number of holes present since the number of holes is used
         * to determine how many bricks we can free
         */
        if (curBrick->m_hasReservedSlot && curBrick->m_pfxes[curBrick->m_maxCapacity - 1] == NULL)
        {
            numHolesPresent--;
        }

        totalNumBricks++;
        curBrick = curBrick->m_next_p;
    }

    curBrick = self->m_lpuList;

    numCanFreeBricks = numHolesPresent / curBrick->m_maxCapacity;

    if (numCanFreeBricks < 1)
        return NLMERR_OK;

    if (self->m_pSettings->m_isJoinedUdc && numCanFreeBricks < 2)
        return NLMERR_OK;

    newNumBricks = totalNumBricks - numCanFreeBricks;

    if (newNumBricks == 0)
        return NLMERR_OK;

    if (self->m_pSettings->m_isMultiBrickAlloc) {
        numMultiBricksBeforeShrink = (totalNumBricks + settings->m_numMultiBricks - 1)  / settings->m_numMultiBricks;
        numMultiBricksAfterShrink = (newNumBricks + settings->m_numMultiBricks - 1)  / settings->m_numMultiBricks;

        if (numMultiBricksAfterShrink >= numMultiBricksBeforeShrink) {
            return NLMERR_OK;
        }

        newNumBricks = numMultiBricksAfterShrink * settings->m_numMultiBricks;
    }

    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(self);

    errNum = kaps_lsn_mc_acquire_resources(self, newNumBricks * curBrick->m_maxCapacity, newNumBricks, o_reason);

    if (errNum != NLMERR_OK)
    {
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    errNum = kaps_lsn_mc_move_holes_lsn_gran_to_one_side(self, 0, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    if (self->m_mlpMemInfo->size > newNumBricks)
    {
        newNumBricks = self->m_mlpMemInfo->size;
    }

    kaps_lsn_mc_free_old_resources(self);

    errNum = kaps_lsn_mc_pvt_commit_shrink(self, newNumBricks, o_reason);

    self->m_pTrie->m_tbl_ptr->m_fibStats.numLsnShrinks++;

    return errNum;
}

void
kaps_lsn_mc_undo_shrink(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * oldBrickList,
    uint32_t oldNumLpuBricks,
    uint32_t oldLsnCapacity,
    struct uda_mem_chunk *oldMlpMemInfo)
{
    kaps_lpm_lpu_brick *newBrickList, *iterLpuBrick, *nextBrick;
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    uint32_t i;

    newBrickList = self->m_lpuList;

    self->m_lpuList = oldBrickList;
    self->m_numLpuBricks = oldNumLpuBricks;
    self->m_nLsnCapacity = oldLsnCapacity;
    self->m_mlpMemInfo = oldMlpMemInfo;

    iterLpuBrick = newBrickList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        kaps_nlm_allocator_free(alloc_p, iterLpuBrick->m_pfxes);
        kaps_nlm_allocator_free(alloc_p, iterLpuBrick);
        ++i;
        iterLpuBrick = nextBrick;
    }

    self->m_bAllocedResource = 1;
}

NlmErrNum_t
kaps_lsn_mc_shrink_per_lpu_gran(
    kaps_lsn_mc * self,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    kaps_flat_lsn_data *flatData = NULL;
    kaps_lpm_lpu_brick *oldBrickList, *iterLpuBrick, *cur_lpu_brick, *nextBrick;
    uint32_t oldNumLpuBricks, oldLsnCapacity;
    uint32_t i, shouldTryShrink;
    struct uda_mem_chunk *oldMlpMemInfo;
    struct uda_mem_chunk *newMlpMemInfo;
    uint32_t doesLsnFit, undoShrink;

    if (!self->m_lpuList || !self->m_lpuList->m_next_p || self->m_nNumPrefixes == 0)
        return NLMERR_OK;

    shouldTryShrink = 0;

    iterLpuBrick = self->m_lpuList;
    while (iterLpuBrick)
    {
        if (iterLpuBrick->m_numPfx == 0)
        {
            shouldTryShrink = 1;
            break;
        }
        iterLpuBrick = iterLpuBrick->m_next_p;
    }

    /*
     * We don't want to try shrink too often, as further operations such as ConvertLsnToFlatData are expensive So
     * restrict to 1 in 10 deletes
     */
    if (self->m_numDeletes % 10 == 0)
        shouldTryShrink = 1;

    if (!shouldTryShrink)
        return NLMERR_OK;

    /*
     * Construct a new LSN
     */
    flatData = kaps_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, o_reason);
    if (!flatData)
    {
        return NLMERR_OK;
    }

    errNum = kaps_lsn_mc_convert_lsn_to_flat_data(self, NULL, 0, flatData, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(self);
    kaps_lsn_mc_store_old_lsn_info(self, 0);

    oldBrickList = self->m_lpuList;
    oldNumLpuBricks = self->m_numLpuBricks;
    oldLsnCapacity = self->m_nLsnCapacity;
    oldMlpMemInfo = self->m_mlpMemInfo;

    doesLsnFit = 0;
    kaps_lsn_mc_convert_flat_data_to_lsn(flatData, self, &doesLsnFit, o_reason);

    kaps_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, flatData);

    undoShrink = 0;

    if (!doesLsnFit)
        undoShrink = 1;

    if (settings_p->m_isJoinedUdc)
    {
        if (self->m_numLpuBricks + 1 >= oldNumLpuBricks)
        {
            undoShrink = 1;
        }
    }
    else
    {
        if (self->m_numLpuBricks >= oldNumLpuBricks)
        {
            undoShrink = 1;
        }
    }

    if (undoShrink)
    {
        kaps_lsn_mc_undo_shrink(self, oldBrickList, oldNumLpuBricks, oldLsnCapacity, self->m_mlpMemInfo);
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    errNum =
        kaps_lsn_mc_pvt_allocate_mlp(self, (uint8_t) self->m_devid, self->m_numLpuBricks, &newMlpMemInfo, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_lsn_mc_undo_shrink(self, oldBrickList, oldNumLpuBricks, oldLsnCapacity, self->m_mlpMemInfo);
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    self->m_mlpMemInfo = newMlpMemInfo;

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_lsn_mc_rearrange_prefixes_for_joined_udcs(self, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    cur_lpu_brick = self->m_lpuList;
    i = 0;
    while (cur_lpu_brick)
    {
        errNum =
            kaps_lsn_mc_acquire_resources_per_lpu(self, cur_lpu_brick, i, cur_lpu_brick->m_maxCapacity,
                                                  cur_lpu_brick->ad_db, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_lsn_mc_free_resources(self);
            kaps_lsn_mc_undo_shrink(self, oldBrickList, oldNumLpuBricks, oldLsnCapacity, oldMlpMemInfo);

            *o_reason = NLMRSC_REASON_OK;
            return NLMERR_OK;

        }

        ++i;
        cur_lpu_brick = cur_lpu_brick->m_next_p;
    }

    self->m_nNumIxAlloced = self->m_nLsnCapacity;

    kaps_lsn_mc_add_extra_brick_for_joined_udcs(self, o_reason);

    
    iterLpuBrick = oldBrickList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        if (iterLpuBrick->m_ixInfo)
            kaps_lsn_mc_check_and_free_ix(self, iterLpuBrick->m_ixInfo, oldMlpMemInfo, i);

        kaps_nlm_allocator_free(settings_p->m_pAlloc, iterLpuBrick->m_pfxes);
        kaps_nlm_allocator_free(settings_p->m_pAlloc, iterLpuBrick);
        ++i;
        iterLpuBrick = nextBrick;
    }

    kaps_lsn_mc_free_mlp_resources(self, oldMlpMemInfo, o_reason);

    NLM_STRY(kaps_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_lsn_mc_update_iit(self, o_reason));

    self->m_pTrie->m_tbl_ptr->m_fibStats.numLsnShrinks++;

    return NLMERR_OK;
}


NlmErrNum_t
kaps_lsn_mc_store_old_lsn_info(
    kaps_lsn_mc * curLsn,
    uint32_t lsnInfoIndex)
{
    kaps_lsn_mc_settings *settings = curLsn->m_pSettings;
    kaps_lpm_lpu_brick *curBrick;
    uint32_t i;
    uint32_t *oldLsnStartIx, *oldLsnNumBricks;
    uint32_t *oldLsnMaxCapacity;

    kaps_memcpy(settings->m_pTmpMlpMemInfoBuf[lsnInfoIndex], curLsn->m_mlpMemInfo, sizeof(*curLsn->m_mlpMemInfo));
    oldLsnStartIx = &settings->m_startIxOfStoredLsnInfo[lsnInfoIndex];
    oldLsnNumBricks = &settings->m_numBricksInStoredLsnInfo[lsnInfoIndex];
    oldLsnMaxCapacity = settings->m_maxCapacityOfStoredLsnInfo[lsnInfoIndex];
    settings->m_isLsnInfoValid[lsnInfoIndex] = 1;

    if (settings->m_isPerLpuGran)
    {
        *oldLsnStartIx = curLsn->m_lpuList->m_ixInfo->start_ix;
    }
    else
    {
        *oldLsnStartIx = curLsn->m_ixInfo->start_ix;
    }

    *oldLsnNumBricks = curLsn->m_numLpuBricks;

    i = 0;
    curBrick = curLsn->m_lpuList;
    while (curBrick)
    {
        oldLsnMaxCapacity[i] = curBrick->m_maxCapacity;
        curBrick = curBrick->m_next_p;
        ++i;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_acquire_resources(
    kaps_lsn_mc * self,
    uint32_t ixRqtSize,
    uint8_t rqtNumLpuBricks,
    NlmReasonCode * o_reason)
{
    struct uda_mem_chunk *mlpMemInfo_p = NULL;
    struct kaps_ix_chunk *ixInfo_p = NULL;
    kaps_status status;
    NlmErrNum_t errNum = NLMERR_OK;

    errNum = kaps_lsn_mc_pvt_allocate_mlp(self, (uint8_t) self->m_devid, rqtNumLpuBricks, &mlpMemInfo_p, o_reason);

    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    /*
     * Suppose the UDA gives more bricks than requested
     */
    if (mlpMemInfo_p->size > rqtNumLpuBricks)
    {
        ixRqtSize = (ixRqtSize / rqtNumLpuBricks) * mlpMemInfo_p->size;
    }

    if (!self->m_pSettings->m_isPerLpuGran)
    {
        if (self->m_pSettings->m_isHardwareMappedIx)
        {
            struct kaps_ad_db *ad_db = (struct kaps_ad_db *) self->m_pTrie->m_tbl_ptr->m_db->common_info->ad_info.ad;

            errNum = kaps_lsn_mc_assign_hw_mapped_ix_per_lsn(self, mlpMemInfo_p, ad_db, &ixInfo_p, o_reason);
            if (errNum != NLMERR_OK)
            {
                kaps_lsn_mc_decr_num_bricks_allocated_for_all_lsns(self->m_pSettings, mlpMemInfo_p->size);
                kaps_uda_mgr_free(self->m_pSettings->m_pMlpMemMgr[self->m_devid], mlpMemInfo_p);
                return errNum;
            }

        }
        else
        {
            struct kaps_ad_db *ad_db = kaps_lsn_mc_get_ad_db(self, self->m_lpuList);
            struct kaps_ix_mgr *pIxMgr = kaps_lsn_mc_get_ix_mgr_for_lsn_pfx(self, ad_db, 
                                                                    mlpMemInfo_p, 0);
            
            status = kaps_ix_mgr_alloc(pIxMgr, NULL, ixRqtSize, self, IX_USER_LSN, &ixInfo_p);
            if (status != KAPS_OK)
            {
                kaps_lsn_mc_decr_num_bricks_allocated_for_all_lsns(self->m_pSettings, mlpMemInfo_p->size);
                kaps_uda_mgr_free(self->m_pSettings->m_pMlpMemMgr[self->m_devid], mlpMemInfo_p);
                errNum = kaps_trie_convert_kbp_status_to_err_num(status, o_reason);
                return errNum;
            }
        }
    }

    if (self->m_bAllocedResource)
    {
        kaps_lsn_mc_store_old_lsn_info(self, 0);

        self->m_pSettings->m_oldMlpMemInfoPtr = self->m_mlpMemInfo;
        self->m_pSettings->m_oldIxChunkForLsn = self->m_ixInfo;
    }

    self->m_mlpMemInfo = mlpMemInfo_p;

    if (!self->m_pSettings->m_isPerLpuGran)
    {
        self->m_ixInfo = ixInfo_p;

        self->m_nAllocBase = ixInfo_p->start_ix;
        self->m_nNumIxAlloced = (uint16_t) ixInfo_p->size;
    }

    self->m_bAllocedResource = 1;

    kaps_lsn_mc_check_joined_udc_alloc(self, rqtNumLpuBricks, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_lsn_mc_undo_acquire_resources(
    kaps_lsn_mc * self)
{
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    if (self->m_ixInfo)
    {
        kaps_lsn_mc_check_and_free_ix(self, self->m_ixInfo, self->m_mlpMemInfo, 0);
    }

    if (self->m_mlpMemInfo)
    {
        kaps_lsn_mc_decr_num_bricks_allocated_for_all_lsns(settings, self->m_mlpMemInfo->size);
        kaps_uda_mgr_free(settings->m_pMlpMemMgr[self->m_devid], self->m_mlpMemInfo);
    }
    

    self->m_mlpMemInfo = settings->m_oldMlpMemInfoPtr;
    self->m_ixInfo = settings->m_oldIxChunkForLsn;

    if (self->m_ixInfo)
    {
        self->m_nAllocBase = self->m_ixInfo->start_ix;
        self->m_nNumIxAlloced = self->m_ixInfo->size;
    }

    settings->m_oldMlpMemInfoPtr = NULL;
    settings->m_oldIxChunkForLsn = NULL;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_free_old_resources(
    kaps_lsn_mc * self)
{
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    if (settings->m_oldIxChunkForLsn)
    {
        kaps_lsn_mc_check_and_free_ix(self, settings->m_oldIxChunkForLsn, 
                        settings->m_oldMlpMemInfoPtr, 0);
    }

    if (settings->m_oldMlpMemInfoPtr)
    {
        kaps_lsn_mc_decr_num_bricks_allocated_for_all_lsns(settings, settings->m_oldMlpMemInfoPtr->size);
        kaps_uda_mgr_free(settings->m_pMlpMemMgr[self->m_devid], settings->m_oldMlpMemInfoPtr);
    }

    settings->m_oldMlpMemInfoPtr = NULL;
    settings->m_oldIxChunkForLsn = NULL;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_assign_hw_mapped_ix_per_lpu(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * brick,
    uint32_t brickIter,
    struct kaps_ad_db * ad_db,
    struct kaps_ix_chunk ** ixInfo_pp,
    NlmReasonCode * o_reason)
{
    struct kaps_ix_chunk *ixInfo_p = NULL;
    uint32_t cur_lpu, cur_row;
    uint32_t maxNumPfxInAnyBrick;
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t total_num_bbs_in_final_level = kaps_device_get_num_final_level_bbs(device, self->m_pTbl->m_db);

    kaps_sassert(self->m_pSettings->m_isHardwareMappedIx);

    ixInfo_p = kaps_nlm_allocator_calloc(self->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_ix_chunk));
    if (!ixInfo_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    ixInfo_p->ad_info = kaps_nlm_allocator_calloc(self->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_ad_chunk));
    if (!ixInfo_p->ad_info)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    maxNumPfxInAnyBrick = settings_p->m_maxNumPfxInAnyBrick;

    kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), self->m_mlpMemInfo, brickIter, &cur_lpu,
                                               &cur_row);

    ixInfo_p->start_ix = (cur_row * total_num_bbs_in_final_level  * maxNumPfxInAnyBrick) + (cur_lpu * maxNumPfxInAnyBrick);

    ixInfo_p->size = maxNumPfxInAnyBrick;
    ixInfo_p->type = IX_ALLOCATED_CHUNK;
    ixInfo_p->user_type = IX_USER_LSN;
    ixInfo_p->lsn_ptr = self;
    ixInfo_p->ad_info->ad_db = ad_db;
    ixInfo_p->ad_info->device = device;

    *ixInfo_pp = ixInfo_p;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_assign_hw_mapped_ix_per_lsn(
    kaps_lsn_mc * self,
    struct uda_mem_chunk * mlpMemInfo,
    struct kaps_ad_db * ad_db,
    struct kaps_ix_chunk ** ixInfo_pp,
    NlmReasonCode * o_reason)
{
    struct kaps_ix_chunk *ixInfo_p = NULL;
    uint32_t cur_lpu, cur_row;
    uint32_t maxNumPfxInAnyBrick;
    kaps_lsn_mc_settings *settings_p = self->m_pSettings;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t total_num_bbs_in_final_level = kaps_device_get_num_final_level_bbs(device, self->m_pTbl->m_db);

    kaps_sassert(self->m_pSettings->m_isHardwareMappedIx);

    ixInfo_p = kaps_nlm_allocator_calloc(self->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_ix_chunk));
    if (!ixInfo_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    ixInfo_p->ad_info = kaps_nlm_allocator_calloc(self->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_ad_chunk));
    if (!ixInfo_p->ad_info)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), mlpMemInfo, 0, &cur_lpu, &cur_row);

    maxNumPfxInAnyBrick = settings_p->m_maxNumPfxInAnyBrick;

    ixInfo_p->start_ix = (cur_row * total_num_bbs_in_final_level * maxNumPfxInAnyBrick) + (cur_lpu * maxNumPfxInAnyBrick);

    ixInfo_p->size = maxNumPfxInAnyBrick * mlpMemInfo->size;
    ixInfo_p->type = IX_ALLOCATED_CHUNK;
    ixInfo_p->user_type = IX_USER_LSN;
    ixInfo_p->lsn_ptr = self;
    ixInfo_p->ad_info->ad_db = ad_db;
    ixInfo_p->ad_info->device = device;

    *ixInfo_pp = ixInfo_p;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_acquire_resources_per_lpu(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * brick,
    uint32_t brickIter,
    uint32_t ixRqtSize,
    struct kaps_ad_db * ad_db,
    NlmReasonCode * o_reason)
{
    struct kaps_ix_chunk *ixInfo_p = NULL;
    struct kaps_ix_mgr *pIxMgr;
    kaps_status status;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    if (settings->m_isHardwareMappedIx)
    {
        errNum = kaps_lsn_mc_assign_hw_mapped_ix_per_lpu(self, brick, brickIter, ad_db, &ixInfo_p, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;

    }
    else
    {
        pIxMgr = kaps_lsn_mc_get_ix_mgr_for_lsn_pfx(self, ad_db, self->m_mlpMemInfo, brickIter);
        status = kaps_ix_mgr_alloc(pIxMgr, ad_db, ixRqtSize, self, IX_USER_LSN, &ixInfo_p);
        if (status != KAPS_OK)
        {
            errNum = kaps_trie_convert_kbp_status_to_err_num(status, o_reason);
            return errNum;
        }
    }

    brick->m_ixInfo = ixInfo_p;

    return errNum;
}

NlmErrNum_t
kaps_lsn_mc_backup_resources_per_lpu(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * brick,
    struct kaps_ix_chunk * oldIxInfo)
{
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    if (settings->m_numBackupLpuBrickResources < KAPS_HW_MAX_LPUS_PER_LPM_DB)
    {
        settings->m_oldLpuBrick[settings->m_numBackupLpuBrickResources] = brick;
        settings->m_oldLpuIxChunk[settings->m_numBackupLpuBrickResources] = oldIxInfo;
        settings->m_numBackupLpuBrickResources++;
    }
    else
    {
        kaps_assert(0, "Too many brick resources to back up");
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_undo_acquire_resources_per_lpu(
    kaps_lsn_mc * self)
{
    uint32_t i;
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    kaps_lpm_lpu_brick *curBrick;

    for (i = 0; i < settings->m_numBackupLpuBrickResources; ++i)
    {
        curBrick = settings->m_oldLpuBrick[i];

        if (curBrick->m_ixInfo != settings->m_oldLpuIxChunk[i])
        {
            kaps_lsn_mc_check_and_free_ix(self, curBrick->m_ixInfo, 
                                    self->m_mlpMemInfo, i);
        }

        curBrick->m_ixInfo = settings->m_oldLpuIxChunk[i];

        settings->m_oldLpuBrick[i] = NULL;
        settings->m_oldLpuIxChunk[i] = NULL;
    }

    settings->m_numBackupLpuBrickResources = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_free_old_resources_per_lpu(
    kaps_lsn_mc * self)
{
    uint32_t i;
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    for (i = 0; i < settings->m_numBackupLpuBrickResources; ++i)
    {
        if (settings->m_oldLpuIxChunk[i])
        {
            kaps_lsn_mc_check_and_free_ix(self, settings->m_oldLpuIxChunk[i], 
                                    settings->m_oldMlpMemInfoPtr, i);
            
            settings->m_oldLpuIxChunk[i] = NULL;
        }
        settings->m_oldLpuBrick[i] = NULL;
    }

    settings->m_numBackupLpuBrickResources = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_add_pfx_to_new_lsn(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry * entryToInsert,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_pfx_bundle *newPfxBundle;
    kaps_lsn_mc_settings *settings = self->m_pSettings;
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_lpm_lpu_brick *newBrick, *prevBrick, *curBrick, *nextBrick;
    uint16_t newPfxGran, newPfxGranIx, lengthAfterLopoff;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t freeSlotLoc;
    uint32_t i, numBricks;

    *newPfx_pp = NULL;

    lengthAfterLopoff = (uint16_t) (entryToInsert->pfx_bundle->m_nPfxSize - self->m_nDepth);
    newPfxGran = kaps_lsn_mc_compute_gran(self->m_pSettings, lengthAfterLopoff, &newPfxGranIx);

    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    numBricks = 1;
    if (settings->m_isFullWidthLsn)
    {
        numBricks = settings->m_maxLpuPerLsn;
    }

    if (settings->m_isMultiBrickAlloc)
    {
        numBricks = settings->m_numMultiBricks;
    }

    prevBrick = NULL;
    newBrick = NULL;
    for (i = 0; i < numBricks; ++i)
    {
        newBrick = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }


        newBrick->ad_db = ad_db;
        newBrick->m_gran = newPfxGran;
        newBrick->m_granIx = newPfxGranIx;
        newBrick->meta_priority = entryToInsert->meta_priority;
        newBrick->m_maxCapacity = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, newBrick->ad_db,
                                                                        newBrick->m_hasReservedSlot,
                                                                        newBrick->meta_priority, newPfxGran);

        newBrick->m_pfxes = kaps_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            kaps_nlm_allocator_free(alloc_p, newBrick);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_next_p = prevBrick;
        prevBrick = newBrick;
    }

    if (newBrick == NULL)
    {
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }
    newBrick->m_numPfx = 1;

    errNum = kaps_lsn_mc_acquire_resources(self, newBrick->m_maxCapacity * numBricks, numBricks, o_reason);

    if (errNum != NLMERR_OK)
    {
        curBrick = newBrick;
        while (curBrick)
        {
            nextBrick = curBrick->m_next_p;
            kaps_nlm_allocator_free(alloc_p, curBrick->m_pfxes);
            kaps_nlm_allocator_free(alloc_p, curBrick);
            curBrick = nextBrick;
        }
        return errNum;
    }

    if (self->m_pSettings->m_isPerLpuGran)
    {
        errNum = kaps_lsn_mc_acquire_resources_per_lpu(self, newBrick, 0, newBrick->m_maxCapacity, ad_db, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_lsn_mc_undo_acquire_resources(self);
            curBrick = newBrick;
            while (curBrick)
            {
                nextBrick = curBrick->m_next_p;
                kaps_nlm_allocator_free(alloc_p, curBrick->m_pfxes);
                kaps_nlm_allocator_free(alloc_p, curBrick);
                curBrick = nextBrick;
            }
            self->m_mlpMemInfo = NULL;
            self->m_bAllocedResource = 0;
            return errNum;
        }

        self->m_nNumIxAlloced = newBrick->m_maxCapacity;
    }

    kaps_lsn_mc_free_old_resources(self);

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    freeSlotLoc = 0;

    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds
        && self->m_nDepth == newPfxBundle->m_nPfxSize)
    {
        kaps_trie_node *trienode;
        void *tmp_ptr;

        tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);
        kaps_memcpy(&trienode, tmp_ptr, sizeof(kaps_trie_node *));

        newPfxBundle->m_nIndex = KAPS_STRICT_LMPSOFAR_INDEX;

        
        trienode->m_iitLmpsofarPfx_p = newPfxBundle;

        newPfxBundle->m_isLmpsofarPfx = 1;

        
    }
    else 
    {
        newBrick->m_pfxes[freeSlotLoc] = newPfxBundle;
        self->m_nNumPrefixes++;
    }
        

    self->m_lpuList = newBrick;
    self->m_numLpuBricks = numBricks;

    self->m_nLsnCapacity = newBrick->m_maxCapacity * numBricks;

    kaps_lsn_mc_add_extra_brick_for_joined_udcs(self, o_reason);

    NLM_STRY(kaps_lsn_mc_commit(self, 0, o_reason));

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_insert_pfx(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entry,
    NlmReasonCode * o_reason)
{
    kaps_pfx_bundle *newpfx = NULL;
    struct kaps_db *db = self->m_pTbl->m_db;
    uint32_t rpb_id = db->rpb_id;
    struct kaps_device *device = db->device;

    kaps_assert(entry->pfx_bundle->m_nPfxSize >= self->m_nDepth, "prefix length is < LSN depth");

    if (self->m_bIsNewLsn)
    {
        if (db->num_algo_levels_in_db == 2)
        {
            kaps_pool_mgr *poolMgr = self->m_pTrie->m_trie_global->poolMgr;
            if (kaps_pool_mgr_get_num_entries(poolMgr, 0, KAPS_IPT_POOL) >= 
                device->hw_res->num_rows_in_rpb[rpb_id])
            {
                *o_reason = NLMRSC_DBA_ALLOC_FAILED;
                return NLMERR_FAIL;
            }
        }
        else
        {
            if (!self->m_pTrie->m_trie_global->m_areAllPoolsHealthy)
            {
                *o_reason = NLMRSC_DBA_ALLOC_FAILED;
                return NLMERR_FAIL;
            }
        }

        NLM_STRY(kaps_lsn_mc_add_pfx_to_new_lsn(self, entry, &newpfx, o_reason));

    }
    else
    {

        /*
         * First try to do fast insert. Fast insert tries to insert new prefix without allocating any new resources and 
         * with using existing resources. If it fails, then reallocInsert is called. 
         */
        NLM_STRY(kaps_lsn_mc_try_fast_insert(self, entry, &newpfx, o_reason));

        if (!newpfx)
        {
            if (self->m_pSettings->m_isPerLpuGran)
            {
                kaps_lsn_mc_resize_per_lpu_gran(self, entry, &newpfx, o_reason);
            }
            else
            {
                kaps_lsn_mc_resize_per_lsn_gran(self, entry, &newpfx, o_reason);
            }
        }

        if (!newpfx)
        {
            if (*o_reason == NLMRSC_REASON_OK || *o_reason == NLMRSC_UDA_ALLOC_FAILED)
            {
                /*
                 * If all the pools are not healthy then don't try to do an IPT split and return a failure 
                 */
                if (!self->m_pTrie->m_trie_global->m_areAllPoolsHealthy)
                {
                    *o_reason = NLMRSC_DBA_ALLOC_FAILED;
                    return NLMERR_FAIL;
                }

                /*
                 * Split the LSN if the size of LSN has reached Tmax or if UDA allocation fails 
                 */
                self->m_bDoGiveout = 1;
                return NLMERR_OK;

            }
            else
            {
                return NLMERR_FAIL;
            }
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_delete_pfx(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry **entrySlotInHash,
    NlmReasonCode * o_reason)
{
    int32_t found_in_lsn = 0;
    kaps_pfx_bundle *pfxBundleInHash = NULL;
    kaps_lpm_lpu_brick *curLpuBrick;
    struct kaps_db *db;
    struct kaps_lpm_entry *e = NULL;

    kaps_sassert(entrySlotInHash != NULL);

    e = (*entrySlotInHash);
    KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(self->m_pSettings->m_device, e, db);

    self->m_numDeletes++;
    pfxBundleInHash = (*entrySlotInHash)->pfx_bundle;


    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds 
        && self->m_nDepth == pfxBundleInHash->m_nPfxSize)
    {
        /*The prefix will be stored in trienode->m_iitLmpsofarPfx_p and not in the LSN.
        So handle this and immediately return from here without going through the LSN.*/
        kaps_trie_node *trienode;
        void *tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);

        kaps_memcpy(&trienode, tmp_ptr, sizeof(kaps_trie_node *));

        trienode->m_iitLmpsofarPfx_p = NULL;

        return NLMERR_OK;
    }
         
 

    {
        uint32_t curIx = self->m_nAllocBase;
        uint32_t indexInLpu;

        curLpuBrick = self->m_lpuList;

        while (curLpuBrick)
        {
            kaps_pfx_bundle *item;
            uint32_t maxNumPfxInLpuBrick = curLpuBrick->m_maxCapacity;
            if (self->m_pSettings->m_isPerLpuGran)
            {
                curIx = curLpuBrick->m_ixInfo->start_ix;
                maxNumPfxInLpuBrick = curLpuBrick->m_ixInfo->size;
                if (maxNumPfxInLpuBrick < curLpuBrick->m_ixInfo->size)
                    kaps_sassert(curLpuBrick->m_underAllocatedIx == 1);
            }

            if (curIx <= pfxBundleInHash->m_nIndex && pfxBundleInHash->m_nIndex < curIx + maxNumPfxInLpuBrick)
            {

                indexInLpu = pfxBundleInHash->m_nIndex - curIx;
                item = curLpuBrick->m_pfxes[indexInLpu];
                if (item)
                {
                    if (self->m_pTrie->m_hashtable_p && pfxBundleInHash == item)
                    {
                        found_in_lsn = 1;

                        NLM_STRY(kaps_lsn_mc_pvt_remove_entry_normal
                                 (self, curLpuBrick, item, indexInLpu, entrySlotInHash, o_reason));
                    }
                }
                break;
            }

            curIx += maxNumPfxInLpuBrick;
            curLpuBrick = curLpuBrick->m_next_p;
        }
    }

    if (!found_in_lsn)
    {
        kaps_assert(0, "Inconsistency between hash table and lsn bucket ");
        *o_reason = NLMRSC_PREFIX_NOT_FOUND;
        return NLMERR_FAIL;
    }

    {

        if (e->hb_user_handle)
        {
            struct kaps_hb *hb = NULL;
            struct kaps_hb_db *hb_db;
            struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) db;
            uint32_t hit_bit_value = 0;
            uint8_t clear_on_read;
            struct kaps_aging_entry *active_aging_table;

            (void) hit_bit_value;
            (void) clear_on_read;

            KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb), (uintptr_t) e->hb_user_handle);

            /*
             * Clear the hit bit in the hardware
             */
            clear_on_read = 1;
            hb_db = (struct kaps_hb_db *) db->common_info->hb_info.hb;
            lpm_db->is_entry_delete_in_progress = 1;
            kaps_hb_entry_get_bit_value(hb_db, KAPS_WB_CONVERT_TO_ENTRY_HANDLE(kaps_hb, e->hb_user_handle),
                                        &hit_bit_value, clear_on_read);
            lpm_db->is_entry_delete_in_progress = 0;

            active_aging_table = kaps_device_get_active_aging_table(db->device, db);

            active_aging_table[hb->bit_no].entry = NULL;
            active_aging_table[hb->bit_no].num_idles = 0;
        }
    }

    if (self->m_pSettings->m_isShrinkEnabled && !db->is_destroy_in_progress)
    {
        if (self->m_pSettings->m_isPerLpuGran)
        {
            NLM_STRY(kaps_lsn_mc_shrink_per_lpu_gran(self, o_reason));
        }
        else
        {
            NLM_STRY(kaps_lsn_mc_shrink_per_lsn_gran(self, o_reason));
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_lsn_mc_update_ad(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entry,
    NlmReasonCode * o_reason,
    uint32_t update_type)
{
    kaps_pfx_bundle *curPfxBundle = entry->pfx_bundle;
    kaps_lpm_lpu_brick *curLpuBrick = self->m_lpuList;
    uint32_t curBrickStartIx = 0;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    NlmErrNum_t errNum;

    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds 
        && self->m_nDepth == entry->pfx_bundle->m_nPfxSize)
    {
        /*If the prefix exactly matches the LSN depth, then we will not be storing the
        prefix in the LSN. So simply return from here. The updating of the AD in the ADS will be
        handled when processing iitLmpsofar in submit rqt*/
        return NLMERR_OK;    
    }

    if (!self->m_pSettings->m_isPerLpuGran)
        curBrickStartIx = self->m_ixInfo->start_ix;

    while (curLpuBrick)
    {
        uint32_t maxPfxInLpu = curLpuBrick->m_maxCapacity;
        if (self->m_pSettings->m_isPerLpuGran)
        {
            curBrickStartIx = curLpuBrick->m_ixInfo->start_ix;
            if (curLpuBrick->m_underAllocatedIx)
                maxPfxInLpu = curLpuBrick->m_ixInfo->size;
        }
        if (curBrickStartIx <= curPfxBundle->m_nIndex && curPfxBundle->m_nIndex < (curBrickStartIx + maxPfxInLpu))
            break;
        curBrickStartIx += curLpuBrick->m_maxCapacity;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (!curLpuBrick)
    {
        kaps_assert(0, "Unable to find the prefix in the LSN\n");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    
    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, ad_db);

    if (ad_db && ad_db->db_info.hw_res.ad_res->ad_type == KAPS_AD_TYPE_INDIRECTION_WITH_DUPLICATION)
    {
        uint32_t whereInLpu = 0;

        for (whereInLpu = 0; whereInLpu < curLpuBrick->m_maxCapacity; whereInLpu++)
        {
            if (curLpuBrick->m_pfxes[whereInLpu] == curPfxBundle)
                break;
        }
        kaps_sassert(whereInLpu < curLpuBrick->m_maxCapacity);

        if (update_type == 2)
            return NLMERR_OK;
    }


    errNum = kaps_lsn_mc_write_entry_to_hw(self, curLpuBrick->m_gran, curPfxBundle, curPfxBundle->m_nIndex,
                                           curPfxBundle->m_nIndex, update_type, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_lsn_mc_submit_rqt(
    kaps_lsn_mc * self,
    NlmTblRqtCode rqtCode,
    struct kaps_lpm_entry * entry,
    struct kaps_lpm_entry ** entrySlotInHash,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t retVal = 0;
    kaps_lsn_mc_settings *settings = self->m_pSettings;

    if (rqtCode == NLM_FIB_PREFIX_INSERT)
    {
        retVal = kaps_lsn_mc_insert_pfx(self, entry, o_reason);

    }
    else if (rqtCode == NLM_FIB_PREFIX_INSERT_WITH_INDEX)
    {
        retVal = kaps_lsn_mc_wb_insert_pfx(self, entry->pfx_bundle->m_nIndex, entry);

    }
    else if (rqtCode == NLM_FIB_PREFIX_DELETE)
    {
        retVal = kaps_lsn_mc_delete_pfx(self, entrySlotInHash, o_reason);

    }
    else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD)
    {
        retVal = kaps_lsn_mc_update_ad(self, entry, o_reason, 1);

    }
    else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD_ADDRESS)
    {
        retVal = kaps_lsn_mc_update_ad(self, entry, o_reason, 2);
    }
    else
    {
        kaps_assert(0, "We only handle inserts, deletes and update AD");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        retVal = NLMERR_FAIL;
    }

    kaps_assert(settings->m_oldMlpMemInfoPtr == NULL, "old UDA resources have to be cleared");
    kaps_assert(settings->m_oldIxChunkForLsn == NULL, "old IX resources have to be cleared");
    kaps_assert(settings->m_numBackupLpuBrickResources == 0, "old Lpu Brick Resources have to be cleared");

    return retVal;
}

static kaps_pfx_bundle *
kaps_lsn_mc_pvt__locate_pfx_exact(
    kaps_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    uint32_t * retix,
    uint32_t * match_brick_num)
{
    kaps_lpm_lpu_brick *curLpuBrick;
    uint32_t cur_brick_num = 0;

    curLpuBrick = self->m_lpuList;
    while (curLpuBrick)
    {
        uint32_t i;
        uint32_t len = (pfxlen + 7) >> 3;

        /*Using int32_t for lenN1 since len can be 0 and lenN1 can take value of -1*/
        int32_t lenN1 = len - 1; 
        uint32_t ch1 = 0;

        if (lenN1 >= 0)
            ch1 = pfxdata[lenN1];

        for (i = 0; i < curLpuBrick->m_maxCapacity; i++)
        {
            kaps_pfx_bundle *item = curLpuBrick->m_pfxes[i];

            if (item)
            {
                uint8_t *data2 ;
                const uint8_t *pfxp;

                /*
                 * All prefixes are stored in a single bucket. Since we are searching for exact
                 match, if the prefix lengths don't match, continue to the next prefix
                 */
                if (item->m_nPfxSize != pfxlen)
                    continue;

                if (item->m_nPfxSize == 0 && pfxlen == 0)
                {
                    /*If prefix lengths are 0, then handle here and immediately return*/
                    *retix = item->m_nIndex;
                    *match_brick_num = cur_brick_num;
                    return item;
                }
                    

                data2 = KAPS_PFX_BUNDLE_GET_PFX_DATA(item) + lenN1;

                /*
                 * Skip the prefix copy when doing a locate exact
                 */
                if (item->m_isPfxCopy)
                    continue;

                /*
                 * Instead of using kaps_memcmp, we iterate backwards because it is actually faster than kaps_memcmp
                 * because more often than not, the very first byte mismatches if we iterate backwards uint8_t * data2 
                 * = KAPS_PFX_BUNDLE_GET_PFX_DATA(item); uint32_t ix = len; for (ix = len ; ix-- ; ) if (data2[ix] !=
                 * pfxdata[ix]) goto NEXT; optimized a little better (MD). made static (__pvt__) for inline
                 * optimization got rid of goto's 
                 */
                if (ch1 != *data2)
                    continue;

                pfxp = pfxdata + lenN1;
                for (;;)
                {
                    if (pfxp-- == pfxdata)
                    {
                        *retix = item->m_nIndex;
                        *match_brick_num = cur_brick_num;
                        return item;
                    }
                    data2--;
                    if (*pfxp != *data2)
                        break;
                }
            }
        }
        curLpuBrick = curLpuBrick->m_next_p;
        cur_brick_num++;
    }

    return 0;
}

kaps_pfx_bundle *
kaps_lsn_mc_locate_exact(
    kaps_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    uint32_t * match_brick_num)
{
    uint32_t ix;

    return kaps_lsn_mc_pvt__locate_pfx_exact(self, pfxdata, pfxlen, &ix, match_brick_num);
}

/*
 * There is a lot more code than what is needed to get the job
 * done because the performance of this routine is critical to
 * the entire table management. It is called many many times
 * during insert/deletes. So, we try to strike a balance between
 * readability and performance.
 */
static kaps_pfx_bundle *
kaps_lsn_mc_pvt_do_lpm(
    kaps_pfx_bundle ** bundles,
    uint32_t nBundles,
    uint32_t len,
    const uint8_t * pfxdata)
{
    uint32_t ch1, memcmplen;

    kaps_assert(len > 0, "prefix length is 0");

    memcmplen = len >> 3;

    len &= 0x7;
    if (len)
    {   /* mask off unused of the trailing bits (is this really needed */
        ch1 = pfxdata[memcmplen];       /* or is memcmplen=(len+7)>>3 sufficient?) */
        ch1 &= 0xff << (8 - len);
    }
    else
    {
        ch1 = pfxdata[--memcmplen];
    }

    while (nBundles-- > 0)
    {
        kaps_pfx_bundle *item = *bundles++;
        if (item)
        {
            uint8_t *data2 = KAPS_PFX_BUNDLE_GET_PFX_DATA(item) + memcmplen;
            const uint8_t *pfxp;
            /*
             * It is faster to use a loop backwards than to do a kaps_memcmp because 1. A mismatch is very very likely 
             * to be found quickly if we are are searching backwards. 2. most implementations of kaps_memcmp (even
             * intrinsic ones) have a substantial overhead when the compare length is not a multiple of 4 and sometimes 
             * they dont terminate early when a mismatch is found. 3. If this is IPv4 or MPLS, we are probably
             * iterating a couple of times max Perhaps for very large compares, we should use kaps_memcmp. Not sure
             * where kaps_memcmp wins over the following. Optimized a little better, got rid of goto's (MD). 
             */

            if (ch1 != *data2)
                continue;       /* Last byte check failed */

            if (!memcmplen)
                return item;
            data2--;
            pfxp = pfxdata + memcmplen - 1;
            for (;;)
            {
                if (*pfxp != *data2)
                    break;
                if (pfxp == pfxdata)
                    return item;
                pfxp--;
                data2--;
            }
        }
    }
    return 0;
}

static kaps_pfx_bundle *
kaps_lsn_mc_pvt_do_lpm_zero_len(
    kaps_pfx_bundle ** bundles,
    uint32_t nBundles)
{
    while (nBundles-- > 0)
    {
        kaps_pfx_bundle *item = *bundles++;
        if (item)
            return item;
    }

    return 0;
}

kaps_pfx_bundle *
kaps_lsn_mc_locate_lpm(
    kaps_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    struct kaps_lpm_entry * skipEntry,
    uint32_t * matchBrickIter,
    uint32_t * matchPosInBrick)
{
    kaps_pfx_bundle *item, *found, *lpmMatch = NULL;
    uint32_t len, i, brickIter;
    kaps_lpm_lpu_brick *curLpuBrick;

    kaps_assert(pfxlen < 0x10000, "Invalid prefix length");

    curLpuBrick = self->m_lpuList;
    brickIter = 0;
    while (curLpuBrick)
    {
        for (i = 0; i < curLpuBrick->m_maxCapacity; i++)
        {
            item = curLpuBrick->m_pfxes[i];
            if (!item)
                continue;

            len = KAPS_PFX_BUNDLE_GET_PFX_SIZE(item);

            if (len > pfxlen)
                continue;

            if (len)
            {
                found = kaps_lsn_mc_pvt_do_lpm(&item, 1, len, pfxdata);
            }
            else
            {
                /*
                 * Re-visit Later NLMNS_NO_SHUFFLES 
                 */
                found = kaps_lsn_mc_pvt_do_lpm_zero_len(&item, 1);
            }

            if (found && skipEntry)
            {
                if (found && found->m_backPtr == skipEntry)
                {
                    found = NULL;
                }
            }

            /*
             * Check the length of the match and set LPM accordingly (NKG) 
             */
            if (found)
            {
                if (lpmMatch == NULL)
                {
                    lpmMatch = found;

                    if (matchBrickIter)
                        *matchBrickIter = brickIter;

                    if (matchPosInBrick)
                        *matchPosInBrick = i;
                }
                else
                {
                    if (lpmMatch->m_backPtr->meta_priority > found->m_backPtr->meta_priority
                        || (lpmMatch->m_backPtr->meta_priority == found->m_backPtr->meta_priority
                            && lpmMatch->m_nPfxSize < found->m_nPfxSize)
                        || (lpmMatch->m_backPtr->meta_priority == found->m_backPtr->meta_priority
                            && lpmMatch->m_nPfxSize == found->m_nPfxSize && lpmMatch->m_isPfxCopy))
                    {
                        lpmMatch = found;

                        if (matchBrickIter)
                            *matchBrickIter = brickIter;

                        if (matchPosInBrick)
                            *matchPosInBrick = i;
                    }
                }
            }
        }
        ++brickIter;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    return lpmMatch;
}

kaps_flat_lsn_data *
kaps_lsn_mc_create_flat_lsn_data(
    kaps_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason)
{
    kaps_flat_lsn_data *ret = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_flat_lsn_data));

    if (!ret)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
    }

    return ret;
}

NlmErrNum_t
kaps_lsn_mc_destroy_flat_lsn_data(
    kaps_nlm_allocator * alloc_p,
    kaps_flat_lsn_data * flatLsnData_p)
{
    kaps_prefix_destroy(flatLsnData_p->m_commonPfx, alloc_p);
    kaps_nlm_allocator_free(alloc_p, flatLsnData_p);
    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_pvt_compute_pfx_sort_data(
    kaps_lsn_mc * self,
    kaps_pfx_bundle * extraPfxArray[],
    uint32_t numExtraPfx,
    uint16_t numPfxForEachLength[],
    uint16_t whereForEachLength[])
{
    uint16_t i, numPfxInLsn, curLoc;
    kaps_lpm_lpu_brick *curLpuBrick = self->m_lpuList;
    uint32_t length;
    kaps_pfx_bundle *newPfxBundle;

    for (i = 0; i <= KAPS_LPM_KEY_MAX_WIDTH_1; ++i)
    {
        numPfxForEachLength[i] = 0;
    }

    numPfxInLsn = 0;

    while (curLpuBrick)
    {
        if (curLpuBrick->m_numPfx == 0)
        {
            curLpuBrick = curLpuBrick->m_next_p;
            continue;
        }

        for (i = 0; i < curLpuBrick->m_maxCapacity; ++i)
        {
            kaps_pfx_bundle *bundle = curLpuBrick->m_pfxes[i];
            if (bundle)
            {
                length = bundle->m_nPfxSize;
                numPfxForEachLength[length]++;
                ++numPfxInLsn;
            }
        }

        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (extraPfxArray)
    {
        for (i = 0; i < numExtraPfx; ++i)
        {
            newPfxBundle = extraPfxArray[i];

            length = newPfxBundle->m_nPfxSize;
            numPfxForEachLength[length]++;
            ++numPfxInLsn;
        }
    }

    curLoc = 0;
    for (i = 0; i <= KAPS_LPM_KEY_MAX_WIDTH_1; ++i)
    {
        whereForEachLength[i] = curLoc;
        curLoc += numPfxForEachLength[i];
    }

    return NLMERR_OK;
}

void
kaps_lsn_mc_assign_flat_data_colors(
    kaps_lsn_mc_settings * settings,
    kaps_flat_lsn_data * flatLsnData_p)
{
    {
        flatLsnData_p->num_colors = 1;
        return;
    }

    
}

NlmErrNum_t
kaps_lsn_mc_convert_lsn_to_flat_data(
    kaps_lsn_mc * self,
    kaps_pfx_bundle * extraPfxArray[],
    uint32_t numExtraPfx,
    kaps_flat_lsn_data * flatLsnData_p,
    NlmReasonCode * o_reason)
{
    kaps_lpm_lpu_brick *curLpuBrick = self->m_lpuList;
    uint32_t i;
    uint16_t numPfxInLsn;
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_pfx_bundle *newPfxBundle = NULL;
    uint16_t numPfxForEachLength[KAPS_LPM_KEY_MAX_WIDTH_1 + 1], whereForEachLength[KAPS_LPM_KEY_MAX_WIDTH_1 + 1];
    uint32_t length;
    uint16_t curLoc;
    uint32_t sortData;

    flatLsnData_p->m_commonPfx = kaps_prefix_create(alloc_p, KAPS_LPM_KEY_MAX_WIDTH_1, self->m_nDepth,
                                                    self->m_pParentHandle->m_data);

    if (!flatLsnData_p->m_commonPfx)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    /*
     * If lsnLmpsofar is enabled, then it is easier if the prefix that matches the LSN is present in the first location 
     * of the flatLsnData since we can make the first brick have the reserved slot and store the prefix in the reserved 
     * slot. So we are sorting the prefixes
     */
    sortData = 0;
    if (self->m_pSettings->m_isPerLpuGran)
        sortData = 1;

    if (sortData)
    {
        kaps_lsn_mc_pvt_compute_pfx_sort_data(self, extraPfxArray, numExtraPfx, numPfxForEachLength,
                                              whereForEachLength);
    }

    flatLsnData_p->m_maxPfxLenInBits = 0;
    numPfxInLsn = 0;
    curLoc = 0;
    while (curLpuBrick)
    {
        if (curLpuBrick->m_numPfx == 0)
        {
            curLpuBrick = curLpuBrick->m_next_p;
            continue;
        }

        for (i = 0; i < curLpuBrick->m_maxCapacity; ++i)
        {
            kaps_pfx_bundle *bundle = curLpuBrick->m_pfxes[i];
            if (bundle)
            {
                if (sortData)
                {
                    length = bundle->m_nPfxSize;
                    curLoc = whereForEachLength[length];
                    whereForEachLength[length]++;
                }

                flatLsnData_p->m_pfxesInLsn[curLoc] = bundle;

                if (!sortData)
                {
                    ++curLoc;
                }

                ++numPfxInLsn;
                if (flatLsnData_p->m_maxPfxLenInBits < bundle->m_nPfxSize)
                {
                    flatLsnData_p->m_maxPfxLenInBits = bundle->m_nPfxSize;
                }
            }
        }

        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (extraPfxArray)
    {
        for (i = 0; i < numExtraPfx; ++i)
        {
            newPfxBundle = extraPfxArray[i];

            if (sortData)
            {
                length = newPfxBundle->m_nPfxSize;
                curLoc = whereForEachLength[length];
                whereForEachLength[length]++;
            }

            flatLsnData_p->m_pfxesInLsn[curLoc] = newPfxBundle;

            if (!sortData)
            {
                ++curLoc;
            }

            ++numPfxInLsn;
            if (flatLsnData_p->m_maxPfxLenInBits < newPfxBundle->m_nPfxSize)
            {
                flatLsnData_p->m_maxPfxLenInBits = newPfxBundle->m_nPfxSize;
            }
        }
    }

    flatLsnData_p->m_numPfxInLsn = numPfxInLsn;

    kaps_lsn_mc_assign_flat_data_colors(self->m_pSettings, flatLsnData_p);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_add_empty_bricks(
    kaps_lsn_mc * self,
    uint32_t pfxGran,
    uint32_t pfxGranIx,
    uint32_t totalNumBricks,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *lsnSettings = self->m_pSettings;
    kaps_lpm_lpu_brick *curBrick = self->m_lpuList;
    kaps_lpm_lpu_brick *newLpuBrick, *lastBrick = NULL;
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    struct kaps_db *db = lsnSettings->m_fibTbl->m_db;
    struct kaps_ad_db *ad_db = (struct kaps_ad_db*) db->common_info->ad_info.ad;

    while (curBrick)
    {
        lastBrick = curBrick;
        curBrick = curBrick->m_next_p;
    }

    while (self->m_numLpuBricks < totalNumBricks)
    {
        /*
         * Allocate a Brick 
         */
        newLpuBrick = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newLpuBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        /*
         * Allocate for maximum num prefixes in the brick 
         */
        newLpuBrick->m_pfxes =
            kaps_nlm_allocator_calloc(alloc_p, lsnSettings->m_maxNumPfxInAnyBrick, sizeof(kaps_pfx_bundle *));
        if (!newLpuBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        self->m_numLpuBricks++;

        newLpuBrick->m_maxCapacity = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(lsnSettings, ad_db, 0, 0xf, pfxGran);
        newLpuBrick->m_granIx = pfxGranIx;
        newLpuBrick->m_gran = pfxGran;
        newLpuBrick->ad_db = ad_db;
        
        self->m_nLsnCapacity += newLpuBrick->m_maxCapacity;

        if (lastBrick)
        {
            lastBrick->m_next_p = newLpuBrick;
        }
        else
        {
            self->m_lpuList = newLpuBrick;
        }

        lastBrick = newLpuBrick;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_fill_up_brick_helper(
    kaps_lsn_mc * curLsn,
    kaps_lpm_lpu_brick * newBrick,
    kaps_lpm_lpu_brick ** lastBrick_pp,
    uint32_t * brick_iter_p,
    kaps_pfx_bundle ** tempBrickArray,
    uint32_t startPosInTempBrick,
    uint32_t numPfxToFill,
    uint32_t gran,
    struct kaps_ad_db * ad_db)
{
    kaps_lsn_mc_settings *settings_p = curLsn->m_pSettings;
    uint32_t i, j;

    newBrick->m_numPfx = numPfxToFill;
    newBrick->m_gran = gran;
    newBrick->m_granIx = settings_p->m_lengthToGranIx[gran];
    newBrick->m_maxCapacity = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings_p, ad_db, 0, 0, gran);
    newBrick->ad_db = ad_db;

    j = startPosInTempBrick;
    for (i = 0; i < numPfxToFill; ++i)
    {
        newBrick->m_pfxes[i] = tempBrickArray[j];
        ++j;
    }

    if (*lastBrick_pp)
    {
        (*lastBrick_pp)->m_next_p = newBrick;
    }
    else
    {
        curLsn->m_lpuList = newBrick;
    }

    *lastBrick_pp = newBrick;
    (*brick_iter_p)++;

    return NLMERR_OK;

}

NlmErrNum_t
kaps_lsn_mc_copy_from_temp_brick_array_to_current_brick(
    kaps_lsn_mc * curLsn,
    kaps_lpm_lpu_brick ** lastBrick_pp,
    uint32_t * brick_iter_p,
    kaps_pfx_bundle ** tempBrickArray,
    uint32_t numPfxInTempBrickArray,
    uint32_t gran,
    uint32_t areBricksJoined,
    uint32_t * lsnCapacity,
    struct kaps_ad_db * ad_db,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings = curLsn->m_pSettings;
    kaps_lpm_lpu_brick *newBrick;

    /*
     * The brick is full. The prefix we are examining can't be added into this brick. So transfer the entries from
     * temp_brick_array into the current brick
     */
    if (areBricksJoined)
    {
        uint32_t numPfxInFirstBrick, numPfxInSecondBrick;

        numPfxInFirstBrick = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, 0, gran);
        if (numPfxInFirstBrick > numPfxInTempBrickArray)
        {
            numPfxInFirstBrick = numPfxInTempBrickArray;
        }

        numPfxInSecondBrick = numPfxInTempBrickArray - numPfxInFirstBrick;

        /*
         * Fill up the details in the current brick
         */
        newBrick = kaps_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_pfxes = kaps_nlm_allocator_calloc(settings->m_pAlloc, settings->m_maxNumPfxInAnyBrick,
                                                      sizeof(kaps_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        kaps_lsn_mc_fill_up_brick_helper(curLsn, newBrick, lastBrick_pp, brick_iter_p,
                                         tempBrickArray, 0, numPfxInFirstBrick, gran, ad_db);

        *lsnCapacity += newBrick->m_maxCapacity;

        /*
         * Fill up the details in the joined brick
         */
        newBrick = kaps_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_pfxes = kaps_nlm_allocator_calloc(settings->m_pAlloc, settings->m_maxNumPfxInAnyBrick,
                                                      sizeof(kaps_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        kaps_lsn_mc_fill_up_brick_helper(curLsn, newBrick, lastBrick_pp, brick_iter_p,
                                         tempBrickArray, numPfxInFirstBrick, numPfxInSecondBrick, gran, ad_db);

        *lsnCapacity += newBrick->m_maxCapacity;

    }
    else
    {
        newBrick = kaps_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_pfxes = kaps_nlm_allocator_calloc(settings->m_pAlloc, settings->m_maxNumPfxInAnyBrick,
                                                      sizeof(kaps_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        kaps_lsn_mc_fill_up_brick_helper(curLsn, newBrick, lastBrick_pp, brick_iter_p,
                                         tempBrickArray, 0, numPfxInTempBrickArray, gran, ad_db);

        *lsnCapacity += newBrick->m_maxCapacity;

    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_convert_flat_data_to_lsn_simple_for_per_brick_gran(
    kaps_flat_lsn_data * flatLsnData_p,
    kaps_lsn_mc * curLsn,
    uint32_t * doesLsnFit_p,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings_p = curLsn->m_pSettings;
    uint32_t brickIter;
    kaps_pfx_bundle *pfxBundle;
    uint32_t buffer_loc;
    uint32_t lengthAfterLopoff, maxPossibleEntriesInBrick;
    uint16_t curPfxGran, curPfxGranIx;
    uint32_t prevGran;
    uint32_t numJoinedSetsProcessed;
    uint32_t numPfxInTempBrickArray;
    kaps_pfx_bundle **tempBrickArray = curLsn->m_pTrie->m_trie_global->m_tempBrickArray;
    kaps_lpm_lpu_brick *lastBrick, *newLpuBrick;
    NlmErrNum_t errNum = NLMCMOK;
    uint32_t lsnCapacity;
    struct kaps_ad_db *ad_db = NULL;

    buffer_loc = 0;
    prevGran = 0;
    numJoinedSetsProcessed = 0;
    numPfxInTempBrickArray = 0;
    brickIter = 0;
    lastBrick = NULL;
    lsnCapacity = 0;

    while (buffer_loc < flatLsnData_p->m_numPfxInLsn)
    {
        pfxBundle = flatLsnData_p->m_pfxesInLsn[buffer_loc];


        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(settings_p->m_device, pfxBundle->m_backPtr->ad_handle, ad_db);

        lengthAfterLopoff = pfxBundle->m_nPfxSize - curLsn->m_nDepth;
        curPfxGran = kaps_lsn_mc_compute_gran(settings_p, lengthAfterLopoff, &curPfxGranIx);
        maxPossibleEntriesInBrick =
            kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings_p, ad_db, 0, pfxBundle->m_backPtr->meta_priority, curPfxGran);

        if (numJoinedSetsProcessed < settings_p->m_numJoinedSets)
        {
            maxPossibleEntriesInBrick *= settings_p->m_numJoinedBricksInOneSet;
        }

        if (numPfxInTempBrickArray + 1 > maxPossibleEntriesInBrick)
        {

            errNum =
                kaps_lsn_mc_copy_from_temp_brick_array_to_current_brick(curLsn, &lastBrick, &brickIter, tempBrickArray,
                                                                        numPfxInTempBrickArray, prevGran,
                                                                        numJoinedSetsProcessed <
                                                                        settings_p->m_numJoinedSets, &lsnCapacity,
                                                                        ad_db, o_reason);

            if (errNum != NLMERR_OK)
                return errNum;

            numPfxInTempBrickArray = 0;

            if (numJoinedSetsProcessed < settings_p->m_numJoinedSets)
            {
                numJoinedSetsProcessed++;
            }

            continue;
        }

        prevGran = curPfxGran;

        tempBrickArray[numPfxInTempBrickArray] = pfxBundle;

        buffer_loc++;
        numPfxInTempBrickArray++;

        if (buffer_loc == flatLsnData_p->m_numPfxInLsn)
        {
            errNum =
                kaps_lsn_mc_copy_from_temp_brick_array_to_current_brick(curLsn, &lastBrick, &brickIter, tempBrickArray,
                                                                        numPfxInTempBrickArray, prevGran,
                                                                        numJoinedSetsProcessed <
                                                                        settings_p->m_numJoinedSets, &lsnCapacity,
                                                                        ad_db, o_reason);
            break;
        }

    }

    /*If there are no prefixes in the flat data, then create a single empty brick*/
    if (flatLsnData_p->m_numPfxInLsn == 0)
    {
        newLpuBrick = kaps_nlm_allocator_calloc(settings_p->m_pAlloc, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newLpuBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newLpuBrick->m_pfxes =
            kaps_nlm_allocator_calloc(settings_p->m_pAlloc, settings_p->m_maxNumPfxInAnyBrick, sizeof(kaps_pfx_bundle *));
        if (!newLpuBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newLpuBrick->m_gran = settings_p->m_lengthToGran[0];
        newLpuBrick->m_granIx = 0;
        
        /*Assign the first ad_db to the brick*/
        newLpuBrick->ad_db = (struct kaps_ad_db*) curLsn->m_pTrie->m_tbl_ptr->m_db->common_info->ad_info.ad;

        newLpuBrick->m_maxCapacity = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings_p, newLpuBrick->ad_db, 
                                                0, 0, newLpuBrick->m_gran);

        curLsn->m_lpuList = newLpuBrick;

        lsnCapacity = newLpuBrick->m_maxCapacity;
        brickIter = 1;
    }

    /*
     * We should not exceed the maxBricksPerLsn
     */
    if (buffer_loc == flatLsnData_p->m_numPfxInLsn && brickIter <= settings_p->m_maxLpuPerLsn)
    {
        *doesLsnFit_p = 1;
    }

    curLsn->m_nNumPrefixes = buffer_loc;
    curLsn->m_nLsnCapacity = lsnCapacity;

    curLsn->m_numLpuBricks = brickIter;

    return errNum;

}

NlmErrNum_t
kaps_lsn_mc_convert_flat_data_to_lsn(
    kaps_flat_lsn_data * flatLsnData_p,
    kaps_lsn_mc * self,
    uint32_t * doesLsnFit_p,
    NlmReasonCode * o_reason)
{
    uint16_t longestPfxGran, longestPfxGranIx;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_lpm_lpu_brick *newLpuBrick, *prevLpuBrick;
    uint16_t numPfxProcessedInLsn;
    kaps_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    uint32_t lengthAfterLopoff;
    uint16_t numPfxInLpu, curGran, curGranIx, prevGran, lpuLoc;
    int32_t isPerLpuGran;
    uint16_t numHolesInserted, lsnCapacity;
    kaps_lsn_mc_settings *lsnSettings = self->m_pSettings;
    struct kaps_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint8_t *placed = lsnSettings->m_isPlaced;
    uint32_t next_entry = 0;
    uint8_t meta_priority;
    uint16_t color = 0;
    int32_t brickIter;
    kaps_pfx_bundle *bundle;
    uint32_t flatDataStartLocForFirstBrick = 0;
    uint32_t use_simple_conversion;
    struct kaps_ad_db *tmp_ad_db = NULL;
    struct kaps_db *db = self->m_pTbl->m_db;

    (void) db;

    *doesLsnFit_p = 0;

    use_simple_conversion = 1;


    isPerLpuGran = self->m_pSettings->m_isPerLpuGran;
    if (device->type == KAPS_DEVICE_KAPS && isPerLpuGran && use_simple_conversion)
    {
        errNum =
            kaps_lsn_mc_convert_flat_data_to_lsn_simple_for_per_brick_gran(flatLsnData_p, self, doesLsnFit_p, o_reason);
        return errNum;
    }

    if (flatLsnData_p->m_numPfxInLsn == 0)
    {
        errNum = kaps_lsn_mc_add_empty_bricks(self, 
                        lsnSettings->m_lengthToGran[0], 0, 1, o_reason);

        *doesLsnFit_p = 1;
        return errNum;
    }
    

    /*
     * placed array indicates if an entry has already been placed in the output LSN brick or not
     */
    kaps_memset(placed, 0, flatLsnData_p->m_numPfxInLsn);

    lengthAfterLopoff = flatLsnData_p->m_maxPfxLenInBits - flatLsnData_p->m_commonPfx->m_inuse;
    longestPfxGran = kaps_lsn_mc_compute_gran(lsnSettings, lengthAfterLopoff, &longestPfxGranIx);
    self->m_numLpuBricks = 0;
    self->m_lpuList = NULL;
    prevLpuBrick = NULL;
    numPfxProcessedInLsn = 0;
    numHolesInserted = 0;
    lsnCapacity = 0;
    brickIter = -1;

    while (self->m_numLpuBricks < lsnSettings->m_maxLpuPerLsn)
    {

        meta_priority = 0xFF;

        /*
         * Allocate a new Brick 
         */
        newLpuBrick = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_lpu_brick));
        if (!newLpuBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        ++brickIter;


        /*
         * Allocate for maximum num prefixes in the brick 
         */
        newLpuBrick->m_pfxes =
            kaps_nlm_allocator_calloc(alloc_p, lsnSettings->m_maxNumPfxInAnyBrick, sizeof(kaps_pfx_bundle *));
        if (!newLpuBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        /*
         * Link the new brick to the lpulist 
         */
        if (!prevLpuBrick)
        {
            self->m_lpuList = newLpuBrick;
        }
        else
        {
            prevLpuBrick->m_next_p = newLpuBrick;
        }
        prevLpuBrick = newLpuBrick;

        self->m_numLpuBricks++;

        if (numPfxProcessedInLsn >= flatLsnData_p->m_numPfxInLsn)
        {
            /*
             * All entries are already placed in the LSN. The current brick is only to satisfy the number of Hole
             * requirement in the LSN. Do not fill ad_db and meta_priority fields as there are no entries in the brick
             * yet 
             */
            newLpuBrick->m_maxCapacity =
                kaps_lsn_mc_calc_max_pfx_in_lpu_brick(lsnSettings, tmp_ad_db, newLpuBrick->m_hasReservedSlot, 0xf,
                                                      longestPfxGran);
            newLpuBrick->m_granIx = longestPfxGranIx;
            newLpuBrick->m_gran = longestPfxGran;
            lsnCapacity += newLpuBrick->m_maxCapacity;

            numHolesInserted += newLpuBrick->m_maxCapacity;
            if (numHolesInserted >= lsnSettings->m_numHolesForGranIx[newLpuBrick->m_granIx])
            {
                break;
            }
            else
            {
                continue;
            }
        }

        numPfxInLpu = 0;
        lpuLoc = 0;
        curGran = longestPfxGran;
        prevGran = 0;
        color = 0xFFFF;

        
        while (numPfxProcessedInLsn < flatLsnData_p->m_numPfxInLsn)
        {
            /*
             * Find next unplaced entry 
             */
            next_entry = 0;
            if (brickIter == 0 && lpuLoc == 1)
            {
                next_entry = flatDataStartLocForFirstBrick;
            }

            while (next_entry < flatLsnData_p->m_numPfxInLsn)
            {
                if (placed[next_entry])
                {
                    next_entry++;
                    continue;
                }

                if (color == 0xFFFF)
                {
                    color = flatLsnData_p->pfx_color[next_entry];
                    break;
                }

                if (color != flatLsnData_p->pfx_color[next_entry])
                {
                    /*
                     * Entry's color doesn't match with the already existing entries in the brick. So skip it now. 
                     */
                    next_entry++;
                    continue;
                }

                /*
                 * We found an entry to place in the brick. So break
                 */
                break;
            }

            if (next_entry >= flatLsnData_p->m_numPfxInLsn)
                break;  /* No more entries to place in current brick */

            bundle = flatLsnData_p->m_pfxesInLsn[next_entry];

            if (isPerLpuGran)
            {
                lengthAfterLopoff = bundle->m_nPfxSize - self->m_nDepth;
                curGran = kaps_lsn_mc_compute_gran(lsnSettings, lengthAfterLopoff, &curGranIx);
            }


            KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, bundle->m_backPtr->ad_handle, tmp_ad_db);

            if (lpuLoc + 1 >
                kaps_lsn_mc_calc_max_pfx_in_lpu_brick(lsnSettings, tmp_ad_db, newLpuBrick->m_hasReservedSlot,
                                                      bundle->m_backPtr->meta_priority, curGran))
            {
                curGran = prevGran;
                break;
            }

            prevGran = curGran;

            newLpuBrick->m_pfxes[lpuLoc] = bundle;

            if (!newLpuBrick->ad_db )
                KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, bundle->m_backPtr->ad_handle, newLpuBrick->ad_db);

            if (meta_priority == 0xFF)
                meta_priority = bundle->m_backPtr->meta_priority;

            placed[next_entry] = 1;
            ++numPfxInLpu;
            ++numPfxProcessedInLsn;

            ++lpuLoc;
        }

        
        /*
         * No more entries in the flat lsn can be placed in the brick. Fill the meta data and continue 
         */
        newLpuBrick->m_numPfx = numPfxInLpu;
        newLpuBrick->m_gran = curGran;
        newLpuBrick->m_granIx = lsnSettings->m_lengthToGranIx[curGran];
        newLpuBrick->meta_priority = meta_priority;
        newLpuBrick->m_maxCapacity = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(lsnSettings, newLpuBrick->ad_db,
                                                                           newLpuBrick->m_hasReservedSlot,
                                                                           newLpuBrick->meta_priority, curGran);

        lsnCapacity += newLpuBrick->m_maxCapacity;

        
        numHolesInserted += newLpuBrick->m_maxCapacity - newLpuBrick->m_numPfx;
        if (numHolesInserted >= lsnSettings->m_numHolesForGranIx[newLpuBrick->m_granIx]
            && numPfxProcessedInLsn >= flatLsnData_p->m_numPfxInLsn)
        {
            break;
        }
    }

    if (numPfxProcessedInLsn == flatLsnData_p->m_numPfxInLsn)
    {
        *doesLsnFit_p = 1;
    }

    self->m_nNumPrefixes = flatLsnData_p->m_numPfxInLsn;
    self->m_nLsnCapacity = lsnCapacity;

    if (lsnSettings->m_isFullWidthLsn)
    {
        errNum =
            kaps_lsn_mc_add_empty_bricks(self, longestPfxGran, longestPfxGranIx, lsnSettings->m_maxLpuPerLsn, o_reason);
    }

    if (lsnSettings->m_isMultiBrickAlloc)
    {
        uint32_t totalNumBricks =
            ((self->m_numLpuBricks + lsnSettings->m_numMultiBricks - 1) / lsnSettings->m_numMultiBricks);
        totalNumBricks = totalNumBricks * lsnSettings->m_numMultiBricks;

        errNum = kaps_lsn_mc_add_empty_bricks(self, longestPfxGran, longestPfxGranIx, totalNumBricks, o_reason);
    }

    return errNum;
}



uint32_t
kaps_lsn_mc_find_if_next_brick_is_joined(
    kaps_lsn_mc * curLsn,
    uint32_t brick_iter)
{
    uint32_t isNextBrickJoined = 0;
    uint32_t lpu_nr_1 = 0, row_nr_1 = 0;
    uint32_t lpu_nr_2 = 0, row_nr_2 = 0;
    kaps_lsn_mc_settings *settings_p = curLsn->m_pSettings;

    kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), curLsn->m_mlpMemInfo, brick_iter, &lpu_nr_1,
                                               &row_nr_1);

    if (brick_iter + 1 < curLsn->m_numLpuBricks)
    {
        kaps_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), curLsn->m_mlpMemInfo, brick_iter + 1,
                                                   &lpu_nr_2, &row_nr_2);

        if ((lpu_nr_1 % 2 == 0) && (lpu_nr_1 + 1 == lpu_nr_2) && (row_nr_1 == row_nr_2))
        {
            isNextBrickJoined = 1;
        }
    }

    return isNextBrickJoined;
}

uint32_t
kaps_lsn_mc_find_longest_prefix_length(
    kaps_lsn_mc * curLsn)
{
    kaps_lpm_lpu_brick *curBrick;
    uint32_t maxPfxLength;
    kaps_pfx_bundle *pfxBundle;
    uint32_t i;

    maxPfxLength = 0;
    curBrick = curLsn->m_lpuList;
    while (curBrick)
    {
        for (i = 0; i < curBrick->m_maxCapacity; ++i)
        {
            pfxBundle = curBrick->m_pfxes[i];

            if (pfxBundle && pfxBundle->m_nPfxSize > maxPfxLength)
                maxPfxLength = pfxBundle->m_nPfxSize;
        }

        curBrick = curBrick->m_next_p;
    }

    return maxPfxLength;
}

NlmErrNum_t
kaps_lsn_mc_find_prefixes_in_path(
    kaps_lsn_mc * curLsn,
    uint8_t * pathDataToCheck,
    uint32_t pathLengthToCheck,
    uint32_t excludeExactPathLengthMatch,
    kaps_pfx_bundle * pfxArray[],
    uint32_t pfxLocationsToMove_p[],
    uint32_t * numPfxInPath,
    uint32_t *isExactMatchLenPfxPresent)
{
    kaps_lpm_lpu_brick *curBrick;
    kaps_pfx_bundle *pfxBundle;
    uint32_t i, indexInLsn;

    *numPfxInPath = 0;
    indexInLsn = 0;
    *isExactMatchLenPfxPresent = 0;
    curBrick = curLsn->m_lpuList;
    while (curBrick)
    {
        for (i = 0; i < curBrick->m_maxCapacity; ++i)
        {
            pfxBundle = curBrick->m_pfxes[i];

            if (pfxBundle)
            {

                if (kaps_prefix_pvt_is_more_specific_equal(pfxBundle->m_data, pfxBundle->m_nPfxSize,
                                                           pathDataToCheck, pathLengthToCheck))
                {

                    if (pfxBundle->m_nPfxSize == pathLengthToCheck)
                        *isExactMatchLenPfxPresent = 1;
                    
                    if (excludeExactPathLengthMatch && pfxBundle->m_nPfxSize == pathLengthToCheck)
                    {
                        /*If the length of the prefix bundle is exactly equal to the path length being checked
                        and the calling function has requested to exclude this prefix, then don't do anything*/
                    }
                    else 
                    {
                        pfxArray[*numPfxInPath] = pfxBundle;
                        pfxLocationsToMove_p[*numPfxInPath] = indexInLsn;
                        (*numPfxInPath)++;
                    }

                }
            }

            ++indexInLsn;
        }

        curBrick = curBrick->m_next_p;
    }

    return NLMERR_OK;
}

void
kaps_lsn_mc_append_to_brick_list(
    kaps_lsn_mc * curLsn,
    kaps_lpm_lpu_brick ** lastBrick_pp,
    kaps_lpm_lpu_brick * newBrick)
{
    if (*lastBrick_pp)
    {
        (*lastBrick_pp)->m_next_p = newBrick;
    }
    else
    {
        curLsn->m_lpuList = newBrick;
    }

    *lastBrick_pp = newBrick;
}

NlmErrNum_t
kaps_lsn_mc_rearrange_prefixes_for_joined_udcs(
    kaps_lsn_mc * curLsn,
    NlmReasonCode * o_reason)
{
    kaps_lpm_lpu_brick *initialArrayOfBricks[KAPS_HW_MAX_LPUS_PER_DB];
    kaps_lpm_lpu_brick *finalArrayOfBricks[KAPS_HW_MAX_LPUS_PER_DB];
    kaps_lpm_lpu_brick *curBrick, *nextBrick, *prevBrick;
    uint32_t i, brickIter, found;

    kaps_memset(initialArrayOfBricks, 0, KAPS_HW_MAX_LPUS_PER_DB * sizeof(kaps_lpm_lpu_brick *));
    kaps_memset(finalArrayOfBricks, 0, KAPS_HW_MAX_LPUS_PER_DB * sizeof(kaps_lpm_lpu_brick *));

    curBrick = curLsn->m_lpuList;
    i = 0;

    while (curBrick)
    {
        nextBrick = curBrick->m_next_p;
        initialArrayOfBricks[i] = curBrick;
        curBrick->m_next_p = NULL;
        curBrick = nextBrick;
        ++i;
    }

    kaps_assert(i == curLsn->m_numLpuBricks, "Incorrect number of bricks in the LSN \n");

    /*
     * First process the joined udcs
     */
    brickIter = 0;
    while (brickIter < curLsn->m_numLpuBricks)
    {
        if (kaps_lsn_mc_find_if_next_brick_is_joined(curLsn, brickIter))
        {

            found = 0;
            
            /*We are checking i < KAPS_HW_MAX_LPUS_PER_DB - 1 in the while loop
            because we are inspecting i+1 array member in the loop below and we don't want to
            cross the bounds of the array */
            for (i = 0; i < KAPS_HW_MAX_LPUS_PER_DB - 1; i++)
            {
                /*Look for any pair of  bricks with the same granularity. We are
                looking at consecutive bricks since the bricks are sorted in increasing order of granularity
                and so same granularity bricks will be next to each other*/
                curBrick = initialArrayOfBricks[i];
                nextBrick = initialArrayOfBricks[i + 1];
                if (curBrick && nextBrick && curBrick->m_gran == nextBrick->m_gran)
                {
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                kaps_assert(0, "Should have found joined brick pair, but couldn't find\n");
                *o_reason = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }

            finalArrayOfBricks[brickIter] = curBrick;
            finalArrayOfBricks[brickIter + 1] = nextBrick;

            initialArrayOfBricks[i] = NULL;
            initialArrayOfBricks[i + 1] = NULL;

            brickIter += 2;

        }
        else
        {
            brickIter++;
        }

    }

    /*
     * Then process the normal udcs
     */
    brickIter = 0;
    while (brickIter < curLsn->m_numLpuBricks)
    {

        if (kaps_lsn_mc_find_if_next_brick_is_joined(curLsn, brickIter))
        {
            brickIter += 2;
        }
        else
        {

            found = 0;
            for (i = 0; i < KAPS_HW_MAX_LPUS_PER_DB; i++)
            {
                curBrick = initialArrayOfBricks[i];

                if (curBrick)
                {
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                kaps_assert(0, "Should have found brick, but couldn't find\n");
                *o_reason = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }

            finalArrayOfBricks[brickIter] = curBrick;

            initialArrayOfBricks[i] = NULL;

            brickIter++;
        }
    }

    /*
     * Now connect the bricks and form the linked list
     */
    brickIter = 0;
    prevBrick = NULL;
    while (brickIter < curLsn->m_numLpuBricks)
    {
        if (brickIter == 0)
        {
            curLsn->m_lpuList = finalArrayOfBricks[brickIter];
        }
        else
        {
            prevBrick->m_next_p = finalArrayOfBricks[brickIter];
        }

        prevBrick = finalArrayOfBricks[brickIter];
        brickIter++;
    }

    return NLMERR_OK;
}

uint8_t *
kaps_lsn_mc_calc_common_bits(
    kaps_lsn_mc_settings * settings,
    kaps_flat_lsn_data * flatLsnData_p,
    uint32_t * retlen)
{
    uint32_t i, j, startbyte;
    uint32_t maxmatch = 0, maxbit = 0;
    uint8_t *tmpData = 0;

    /*
     * Bits till LopOff are common in prefixes. Start the common bit search skipping the common bits bytes. 
     */
    startbyte = flatLsnData_p->m_commonPfx->m_inuse / 8;

    /*
     * Go through prefixes already in hardware and determine the max-common bits 
     */

    maxbit = KAPS_LPM_KEY_MAX_WIDTH_1;
    maxmatch = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(maxbit);

    for (i = 0; i < flatLsnData_p->m_numPfxInLsn; i++)
    {
        kaps_pfx_bundle *bundle = flatLsnData_p->m_pfxesInLsn[i];

        if (bundle)
        {       /* Not a hole? */
            uint8_t *data = KAPS_PFX_BUNDLE_GET_PFX_DATA(bundle);

            if (maxbit > bundle->m_nPfxSize)
            {
                maxbit = bundle->m_nPfxSize;
                maxmatch = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(maxbit);
            }

            if (!tmpData)
            {
                tmpData = data;
                continue;
            }

            for (j = startbyte; j < maxmatch; j++)
            {
                uint32_t cmp = tmpData[j] ^ data[j];
                if (cmp)
                {
                    uint32_t tmp = (j << 3) + settings->m_cmpLookupTable[cmp];

                    if (maxbit > tmp)
                    {
                        maxbit = tmp;
                        maxmatch = j + 1;
                    }
                    break;
                }
            }
        }
    }

    *retlen = maxbit;
    return tmpData;
}

static NlmErrNum_t
kaps_lsn_mc_pct_find_give_out_prefix(
    kaps_lsn_mc_settings * settings,
    kaps_flat_lsn_data * flatLsnData_p,
    kaps_prefix * giveout_prefix,
    uint8_t give_or_keep[],
    uint16_t tbl_width)
{
    uint32_t norig, ngive_upper_thresh, ngive_lower_thresh, found_giveout, ngive = 0, nkeep = 0, giveix, keepix;
    uint32_t flipped = 0;
    uint8_t zero_byte = 0;
    uint32_t offset_8, commonLength;
    uint32_t i = 0;
    uint8_t is_root_being_split;
    uint32_t longest_give_pfx_len;
    uint32_t caution_height = settings->m_bottom_up_caution_height;

    is_root_being_split = 0;
    if (giveout_prefix->m_inuse == 0)
    {
        is_root_being_split = 1;
    }

    norig = flatLsnData_p->m_numPfxInLsn;
    ngive_upper_thresh = (2 * norig) / 3;
    ngive_lower_thresh = norig / 3;

    if (ngive_upper_thresh == 0)
        ngive_upper_thresh = 1;

    if (ngive_lower_thresh == 0)
        ngive_lower_thresh = 1;

    offset_8 = giveout_prefix->m_inuse / 8;
    commonLength = offset_8 * 8;

    found_giveout = 0;

    while (!found_giveout)
    {
        if (flipped == 0)
        {
            kaps_prefix_append(giveout_prefix, 1, &zero_byte);
        }

        flipped = 0;

        giveix = 0;
        keepix = 0;

        longest_give_pfx_len = 0;

        for (i = 0; i < flatLsnData_p->m_numPfxInLsn; i++)
        {
            /*
             * Go over prefixes, count the number of prefixes eligible to go into giveOut LSN and Keep LSN. 
             */
            kaps_pfx_bundle *bundle = flatLsnData_p->m_pfxesInLsn[i];
            if (KAPS_PFX_BUNDLE_GET_PFX_SIZE(bundle) < giveout_prefix->m_inuse)
            {
                /*
                 * Mark that this prefix should go into Keep LSN. 
                 */
                give_or_keep[i] = KAPS_KEEP;
                keepix++;
                continue;
            }

            if (kaps_prefix_pvt_is_more_specific_equal(&bundle->m_data[offset_8],
                                                       bundle->m_nPfxSize - commonLength,
                                                       &giveout_prefix->m_data[offset_8],
                                                       giveout_prefix->m_inuse - commonLength))
            {
                /*
                 * Mark that this prefix should go into giveOut LSN. 
                 */
                give_or_keep[i] = KAPS_GIVE;
                giveix++;

                if (bundle->m_nPfxSize >= longest_give_pfx_len)
                    longest_give_pfx_len = bundle->m_nPfxSize;

            }
            else
            {
                give_or_keep[i] = KAPS_KEEP;
                keepix++;
            }
        }

        ngive = giveix;
        nkeep = keepix;

        if ((ngive <= ngive_upper_thresh) && (ngive >= ngive_lower_thresh))
        {
            found_giveout = 1;
        }
        else if ((longest_give_pfx_len - giveout_prefix->m_inuse) == caution_height && ngive >= ngive_upper_thresh
                 && nkeep)
        {
            /*
             * When doing a giveout, the give LSN should not be so deep that it becomes sparse. For instance if the
             * table width is 144 and depth of the give LSN is 139, then we can store only 2 ^ 5 = 32 prefixes in it.
             * However the index space occupied by the LSN will be for the entire 85 prefixes of 12b granularity that
             * can fit into the brick. If the LSN already has all combinations of the last 5 bits in it, the LSN will
             * be sparse and index range will be wasted. To avoid this problem, we stop when depth becomes greater than 
             * 144, even though the give LSN has more than 2/3rd prefixes in it. Note that we may have a local maximum. 
             * So the table width can be 160, but for a particular path, the user may be adding only prefixes with a
             * length of 52. So even in this case we have to take care of sparse LSNs. To handle this we are making use 
             * of longest_give_pfx_len instead of the table width
             */
            found_giveout = 1;
        }
        else if (is_root_being_split && giveout_prefix->m_inuse >= 8 && ngive >= 1)
        {
            /*
             * If the root of the trie is one of the nodes involved in the giveout, then we have noticed a problem. Let 
             * us say that the user added the default entry. Then the root of the trie becomes an LSN. Then when the
             * root LSN becomes full we do a giveout and retain some entries in the root LSN and pushing some entries
             * to a deeper LSN. After the split, the next entry added will most likely end up in the root LSN. So what
             * happens is that root LSN always ends up becoming up and so we keep splitting the root LSN. This results
             * in a huge number of IPT entries being created and we may report a table full even if a lot of UDA is
             * still available. To solve this problem, we try to retain only /0 to /7 entries in the root and any entry 
             * which is of length 8 or more is forcefully pushed into an LSN at depth 8. This avoids the repeatedly
             * splitting the root and we significantly reduce the number of triggers 
             */
            found_giveout = 1;
        }
        else if (ngive < ngive_lower_thresh)
        {
            /*
             * coverity[overrun-local] 
             */
            KAPS_PREFIX_SET_BIT(giveout_prefix, giveout_prefix->m_inuse - 1, 1);
            flipped = 1;
        }

        /*
         * Suppose we the LSN at depth 48 has a prefix copy x/48 with meta priority = 0 and the user has added a prefix 
         * x/48 with meta priority = 1 The LSN width = 1. The user prefix can't be added to the brick since the brick
         * already has a meta priority of 0. In this case, when we attempt the giveout, both the user prefix and the
         * prefix copy will go to the keep LSN. Then we try to make the giveout prefix longer to depth of 49. But then
         * we run into an infinite loop. So prevent the infinite loop we break out if giveout_prefix length exceeds the 
         * max prefix length in the flat LSN
         */
        if (giveout_prefix->m_inuse > flatLsnData_p->m_maxPfxLenInBits)
        {
            found_giveout = 1;
            break;
        }
    }

    return NLMERR_OK;
}

void
kaps_lsn_mc_flat_data_update_color(
    kaps_lsn_mc_settings * settings,
    kaps_flat_lsn_data * flatData_p)
{
    uint32_t i, j;
    uint16_t color = 0;
    uint16_t old_color;
    const uint16_t flag_bit = (1 << 15);        /* use msb bit in color to indicate if new color has been written */
    const uint16_t flag_mask = (1 << 15) - 1;
    struct kaps_device *device = settings->m_fibTbl->m_fibTblMgr_p->m_devMgr_p;

    for (i = 0; i < flatData_p->m_numPfxInLsn; i++)
    {
        old_color = flatData_p->pfx_color[i];
        if (old_color & flag_bit)
        {
            flatData_p->pfx_color[i] &= flag_mask;
            continue;
        }
        flatData_p->pfx_color[i] = color;
        for (j = i + 1; j < flatData_p->m_numPfxInLsn; j++)
        {
            if (flatData_p->pfx_color[j] == old_color)
            {
                flatData_p->pfx_color[j] = color | flag_bit;
                if (flatData_p->m_pfxesInLsn[j]->m_backPtr->ad_handle)
                {
                    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, flatData_p->m_pfxesInLsn[j]->m_backPtr->ad_handle,
                                                      flatData_p->ad_colors[color]);
                }
            }
        }
        color++;
    }

    flatData_p->num_colors = color;
}

NlmErrNum_t
kaps_lsn_mc_undo_lsn_push(
    kaps_lsn_mc_settings * settings,
    kaps_flat_lsn_data * giveData_p,
    kaps_flat_lsn_data * keepData_p,
    struct kaps_lsn_mc *newLsn)
{
    kaps_pfx_bundle *iitLmpsofar;

    if (giveData_p->m_commonPfx)
    {
        kaps_prefix_destroy(giveData_p->m_commonPfx, settings->m_pAlloc);
        giveData_p->m_commonPfx = NULL;

    }

    if (keepData_p->m_commonPfx)
    {
        kaps_prefix_destroy(keepData_p->m_commonPfx, settings->m_pAlloc);
        keepData_p->m_commonPfx = NULL;
    }

    /*
     * Reset the giveData_p since it may be reused for LSN giveout. But the iitLmpsofarPfx in giveData should be
     * restored back
     */
    iitLmpsofar = giveData_p->m_iitLmpsofarPfx;
    kaps_memset(giveData_p, 0, sizeof(kaps_flat_lsn_data));
    giveData_p->m_iitLmpsofarPfx = iitLmpsofar;

    if (newLsn)
    {
        kaps_lsn_mc_destroy(newLsn);
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_try_in_place_lsn_push(
    kaps_lsn_mc * originalLsn,
    kaps_lsn_mc_settings * settings,
    uint8_t * commonBits,
    uint32_t commonBitsLen,
    kaps_flat_lsn_data * curGiveoutData_p,
    kaps_flat_lsn_data * giveData_p,
    kaps_flat_lsn_data * keepData_p,
    kaps_pfx_bundle * pfxBundleToInsert,
    uint32_t * isInPlacePushSuccessful,
    NlmReasonCode * o_reason)
{
    uint32_t originalLsnDepth = curGiveoutData_p->m_commonPfx->m_inuse;
    uint32_t maxPfxLen = curGiveoutData_p->m_maxPfxLenInBits;
    uint32_t i, j;
    struct kaps_lsn_mc *newLsn = NULL;
    uint32_t isLsnFitPossible;
    NlmErrNum_t errNum;
    struct kaps_uda_mgr *pMlpMemMgr = settings->m_pMlpMemMgr[0];
    struct uda_mem_chunk *uda_chunk = NULL;
    kaps_status status;
    uint32_t chosenGiveSlot = 0;
    struct kaps_ad_db *ad_db = NULL;
    struct kaps_device *device = settings->m_device;

    (void) chosenGiveSlot;
    (void) ad_db;
    (void) device;

    /*
     * Simple check to see if we have sufficient common bits. This is a heuristic. If we try LSN pushing each time, it
     * may be expensive. So this heuristic has been added to prevent attempting LSN push each time. Remove the
     * heuristic if it is not working properly
     */
    if (commonBitsLen <= originalLsnDepth)
    {
        *isInPlacePushSuccessful = 0;
        return NLMERR_OK;
    }

    if (settings->m_strictlyStoreLmpsofarInAds)
    {
        kaps_trie_node *originalLsnTrieNode;

        void *tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(originalLsn->m_pParentHandle);

        kaps_memcpy(&originalLsnTrieNode, tmp_ptr, sizeof(kaps_trie_node *));
        
        /*If the prefix that exactly matches the LSN is present, then we can't push the LSN any further
        So simply return*/
        if (originalLsnTrieNode->m_iitLmpsofarPfx_p && 
            originalLsnTrieNode->m_iitLmpsofarPfx_p->m_nPfxSize == originalLsnTrieNode->m_depth)
        {
            *isInPlacePushSuccessful = 0;
            return NLMERR_OK;
        }
    }
        

    /*
     * Don't go too deep, otherwise we will end up with sparse LSNs
     */
    if (maxPfxLen - commonBitsLen < settings->m_bottom_up_caution_height)
        commonBitsLen = maxPfxLen - settings->m_bottom_up_caution_height;

    /*
     * Once again check if commonBitsLen is greater than originalLsnDepth
     */
    if (commonBitsLen <= originalLsnDepth)
    {
        *isInPlacePushSuccessful = 0;
        return NLMERR_OK;
    }

    giveData_p->m_commonPfx =
        kaps_prefix_create(settings->m_pAlloc, KAPS_LPM_KEY_MAX_WIDTH_1, commonBitsLen, commonBits);

    j = 0;
    for (i = 0; i < curGiveoutData_p->m_numPfxInLsn; ++i)
    {
        kaps_pfx_bundle *bundle = curGiveoutData_p->m_pfxesInLsn[i];

        if (settings->m_strictlyStoreLmpsofarInAds 
                && giveData_p->m_commonPfx->m_inuse == bundle->m_nPfxSize)
        {
            /*If the prefix length exactly matches the LSN and we have 
            m_strictlyStoreLmpsofarInAds = 1, then this prefix will not go into the
            resulting LSN. So store in the giveData_p->m_iitLmpsofarPfx instead.*/
            giveData_p->m_iitLmpsofarPfx = bundle;
            chosenGiveSlot = i;
        }
        else 
        {
            giveData_p->m_pfxesInLsn[j] = bundle;
            giveData_p->pfx_color[j] = curGiveoutData_p->pfx_color[i];
            if (giveData_p->m_maxPfxLenInBits < bundle->m_nPfxSize)
            {
                giveData_p->m_maxPfxLenInBits = bundle->m_nPfxSize;
            }
            ++j;
        }
    }

    giveData_p->m_numPfxInLsn = j;
    giveData_p->num_colors = curGiveoutData_p->num_colors;
    
    if (giveData_p->m_iitLmpsofarPfx && !giveData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx)
    {
    

        giveData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx = 1;
    }

    
    newLsn = kaps_lsn_mc_create(settings, settings->m_fibTbl->m_trie, giveData_p->m_commonPfx->m_inuse);

    if (!newLsn)
    {
        kaps_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    isLsnFitPossible = 0;
    errNum = kaps_lsn_mc_convert_flat_data_to_lsn(giveData_p, newLsn, &isLsnFitPossible, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        return errNum;
    }

    if (!isLsnFitPossible)
    {
        kaps_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        return NLMERR_OK;
    }

    keepData_p->m_commonPfx = kaps_prefix_create(settings->m_pAlloc, KAPS_LPM_KEY_MAX_WIDTH_1,
                                                 curGiveoutData_p->m_commonPfx->m_inuse,
                                                 curGiveoutData_p->m_commonPfx->m_data);

    if (!keepData_p->m_commonPfx)
    {
        kaps_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    status = kaps_uda_mgr_alloc(pMlpMemMgr, newLsn->m_numLpuBricks, newLsn->m_pTbl->m_fibTblMgr_p->m_devMgr_p, newLsn,
                                &uda_chunk, newLsn->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db);

    if (status != KAPS_OK || !uda_chunk)
    {
        kaps_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        return NLMERR_OK;
    }

    kaps_uda_mgr_free(pMlpMemMgr, uda_chunk);

    /*
     * We will create the LSN once we come out of this function and return to the trie layer. So destroy the LSN
     */
    if (newLsn)
    {
        kaps_lsn_mc_destroy(newLsn);
    }

    kaps_lsn_mc_flat_data_update_color(settings, giveData_p);

    *isInPlacePushSuccessful = 1;
    settings->m_fibTbl->m_fibStats.numLsnPush++;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_giveout(
    kaps_lsn_mc * originalLsn,
    kaps_lsn_mc_settings * settings,
    kaps_fib_tbl *fibTbl,
    kaps_flat_lsn_data * curGiveoutData_p,
    kaps_flat_lsn_data * keepData_p,
    kaps_flat_lsn_data * giveData_p,
    kaps_pfx_bundle * pfxBundleToInsert,
    kaps_nlm_allocator * alloc_p,
    uint32_t forceOnlyLsnPush,
    NlmReasonCode * o_reason)
{
    uint8_t *commonBits = NULL;
    uint32_t commonBitsLen;
    uint16_t lsnDepth = curGiveoutData_p->m_commonPfx->m_inuse;
    kaps_prefix *giveout_prefix = NULL;
    uint16_t giveix, keepix;
    uint32_t byteOffset, commonLength, i;
    uint16_t maxDepth = curGiveoutData_p->m_maxPfxLenInBits;
    uint32_t caution_height = settings->m_bottom_up_caution_height;
    uint32_t isCurPfxBundleLmpsofarOfGiveLsn;
    uint32_t isInPlacePushSuccessful;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t tryInPlacePush = 0;
    struct kaps_db *db = fibTbl->m_db;
    struct kaps_ad_db *ad_db = NULL;
    uint32_t areGiveAndKeepLmpsofarIdentical = 0;

    (void) ad_db;
    (void) areGiveAndKeepLmpsofarIdentical;

    if (db->num_algo_levels_in_db == 3)
    {
        tryInPlacePush = 1;
    }

    /*
     * Find the common bits in the current giveout LSN 
     */
    commonBits = kaps_lsn_mc_calc_common_bits(settings, curGiveoutData_p, &commonBitsLen);

    kaps_memset(settings->common_bits_array, 0, KAPS_LPM_KEY_MAX_WIDTH_8);
    kaps_memcpy(settings->common_bits_array, commonBits, (commonBitsLen + 7) / 8);

    if (settings->m_strictlyStoreLmpsofarInAds)
    {
        /*If we have a prefix whose depth is the same as the curGiveoutData_p, and we are using 
        m_strictlyStoreLmpsofarInAds, then the prefix will not be stored in the curGiveoutData_p->m_pfxesInLsn
        and hence will not be used to calculate the common bits. 
        In this case, force  the common bits length to be equal to the depth of the curGiveoutData_p*/
        
        if (curGiveoutData_p->m_iitLmpsofarPfx &&
            curGiveoutData_p->m_iitLmpsofarPfx->m_nPfxSize == curGiveoutData_p->m_commonPfx->m_inuse)
        {
            commonBitsLen = curGiveoutData_p->m_commonPfx->m_inuse;
        }
    }

  

    /*
     * First try to do an in-place push. If that fails, then try giveout
     */
    isInPlacePushSuccessful = 0;
    if (tryInPlacePush || forceOnlyLsnPush)
    {
        errNum =
            kaps_lsn_mc_try_in_place_lsn_push(originalLsn, settings, settings->common_bits_array, commonBitsLen, curGiveoutData_p, giveData_p,
                                              keepData_p, pfxBundleToInsert, &isInPlacePushSuccessful, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;

        if (isInPlacePushSuccessful)
        {
            return NLMERR_OK;
        }

        if (forceOnlyLsnPush && !isInPlacePushSuccessful)
        {
            return NLMERR_FAIL;
        }
    }

    /*
     * We now allow splitting an LSN which has not yet reached Tmax if the UDA is full, so that we can squeeze out
     * space from the UDA. There is no use splitting an LSN if it has too few prefixes. Also giveout can proceed only
     * if there are at least 3 prefixes in the LSN 
     */
    if (curGiveoutData_p->num_colors == 1)
    {   /* Skip the logic for LSNs with multiple colors */
        if (curGiveoutData_p->m_numPfxInLsn < 3)
        {
            return NLMERR_FAIL;
        }
    }

    /*
     * If we go too deep, then LSNs will be sparse. So avoid this 
     */
    if (curGiveoutData_p->num_colors <= settings->m_maxLpuPerLsn)
    {
        if (maxDepth >= caution_height)
        {
            /*in any case we should not go above the curGiveoutData depth*/
            if (commonBitsLen > (uint32_t) (maxDepth - caution_height)
                && (uint32_t) (maxDepth - caution_height) > lsnDepth)
            {
                commonBitsLen = maxDepth - caution_height;
            }
        }
    }

    giveout_prefix = kaps_prefix_create(alloc_p, KAPS_LPM_KEY_MAX_WIDTH_1, commonBitsLen, settings->common_bits_array);

    kaps_lsn_mc_pct_find_give_out_prefix(settings, curGiveoutData_p, giveout_prefix, settings->m_give_or_keep,
                                         settings->m_fibTbl->m_width);

    
    keepData_p->m_commonPfx = kaps_prefix_create(alloc_p, KAPS_LPM_KEY_MAX_WIDTH_1, commonBitsLen, settings->common_bits_array);
    
    
    giveData_p->m_commonPfx = giveout_prefix;

    byteOffset = commonBitsLen / 8;
    commonLength = byteOffset * 8;

    keepix = 0;
    giveix = 0;
    for (i = 0; i < curGiveoutData_p->m_numPfxInLsn; ++i)
    {
        kaps_pfx_bundle *bundle = curGiveoutData_p->m_pfxesInLsn[i];

        if (settings->m_give_or_keep[i] == KAPS_KEEP)
        {
            /*
             * Find if the current prefix is shorter than giveout prefix and lies on the same trie path of giveout
             * prefix
             */
            int32_t isInGiveoutPath = kaps_prefix_pvt_is_more_specific_equal(&giveout_prefix->m_data[byteOffset],
                                                                             giveout_prefix->m_inuse - commonLength,
                                                                             &bundle->m_data[byteOffset],
                                                                             bundle->m_nPfxSize - commonLength);
            /*Find the longest keep prefix that lies in the same path as the giveout_prefix*/
            isCurPfxBundleLmpsofarOfGiveLsn = 0;
            
            if (isInGiveoutPath)
            {

                if (!giveData_p->m_iitLmpsofarPfx)
                {
                    isCurPfxBundleLmpsofarOfGiveLsn = 1;
                }
                else
                {
                    if (bundle->m_nPfxSize > giveData_p->m_iitLmpsofarPfx->m_nPfxSize)
                    {
                        isCurPfxBundleLmpsofarOfGiveLsn = 1;
                    }
                    else if (bundle->m_nPfxSize == giveData_p->m_iitLmpsofarPfx->m_nPfxSize && !bundle->m_isPfxCopy)
                    {
                        isCurPfxBundleLmpsofarOfGiveLsn = 1;
                    }
                }

                if (isCurPfxBundleLmpsofarOfGiveLsn)
                {
                    giveData_p->m_iitLmpsofarPfx = bundle;
                }

            }

            if (settings->m_strictlyStoreLmpsofarInAds 
                && keepData_p->m_commonPfx->m_inuse == bundle->m_nPfxSize)
            {
                /*Let us say that we are splitting 10/8 into 10.0/12 and 10.0/16. Suppose user has already
                added 10.0/12, then 10.0/12 will become the lmpsofar of the KEEP LSN*/
                keepData_p->m_iitLmpsofarPfx = bundle;
            }
            else 
            {
           
                keepData_p->m_pfxesInLsn[keepix] = bundle;
                keepData_p->pfx_color[keepix] = curGiveoutData_p->pfx_color[i];
                if (keepData_p->m_maxPfxLenInBits < bundle->m_nPfxSize)
                    keepData_p->m_maxPfxLenInBits = bundle->m_nPfxSize;

                ++keepix;
            }



        }
        else if (settings->m_give_or_keep[i] == KAPS_GIVE)
        {
            if (settings->m_strictlyStoreLmpsofarInAds 
                && giveData_p->m_commonPfx->m_inuse == bundle->m_nPfxSize)
            {
                /*If the prefix length is exactly equal to the GIVE LSN depth, then 
                don't store the prefix in the Give data since we don't want to store it in the LSN.
                Store it in the m_iitLmpsofarPfx instead*/
                giveData_p->m_iitLmpsofarPfx = bundle;

            }
            else 
            {
                giveData_p->m_pfxesInLsn[giveix] = bundle;
                giveData_p->pfx_color[giveix] = curGiveoutData_p->pfx_color[i];
                if (giveData_p->m_maxPfxLenInBits < bundle->m_nPfxSize)
                {
                    giveData_p->m_maxPfxLenInBits = bundle->m_nPfxSize;
                }
                ++giveix;
            }

        }
        else
        {
            kaps_assert(0, "Incorrect value in give_or_keep array\n");
        }
    }

    areGiveAndKeepLmpsofarIdentical = 0;
    if (giveData_p->m_iitLmpsofarPfx == keepData_p->m_iitLmpsofarPfx)
        areGiveAndKeepLmpsofarIdentical = 1;
    
    if (giveData_p->m_iitLmpsofarPfx && !giveData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx)
    {
    

                            
        giveData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx = 1;
    }


    
    if (keepData_p->m_iitLmpsofarPfx && !keepData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx)
    {
    
    
                                
        keepData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx = 1;
    }

    
    giveData_p->m_numPfxInLsn = giveix;
    keepData_p->m_numPfxInLsn = keepix;

    kaps_lsn_mc_flat_data_update_color(settings, giveData_p);
    kaps_lsn_mc_flat_data_update_color(settings, keepData_p);

    return NLMERR_OK;
}

int32_t
kaps_lsn_mc_get_num_prefixes(
    void *lsn,
    struct kaps_ix_chunk * cur_chunk)
{
    kaps_lsn_mc *self = (kaps_lsn_mc *) lsn;
    kaps_lpm_lpu_brick *brickWithReservedSlot = NULL;
    (void) brickWithReservedSlot;

    if (self->m_pSettings->m_isPerLpuGran)
    {
        kaps_lpm_lpu_brick *ixBrick = NULL;

        kaps_assert(self->m_ixInfo == NULL, "Invalid Ix info in the LSN ");

        ixBrick = self->m_lpuList;
        while (ixBrick)
        {
            if (ixBrick->m_hasReservedSlot)
                brickWithReservedSlot = ixBrick;

            if (ixBrick->m_ixInfo == cur_chunk)
                break;
            ixBrick = ixBrick->m_next_p;
        }


        kaps_sassert(ixBrick);
        return ixBrick->m_numPfx;
    }
    else
    {
        return self->m_nNumPrefixes;
    }
}

NlmErrNum_t
kaps_lsn_mc_verify(
    kaps_lsn_mc * self)
{
    kaps_lpm_lpu_brick *curLpuBrick_p;
    uint16_t numPfxInLsn, numPfxInLpu;
    uint32_t i = 0;
    kaps_pfx_bundle *pfx;
    struct kaps_device *device = self->m_pTbl->m_db->device;
    struct kaps_ad_db *ad_db = NULL;

    curLpuBrick_p = self->m_lpuList;
    numPfxInLsn = 0;
    while (curLpuBrick_p)
    {
        /*Check if AD DB pointer is set in the brick*/
        if (curLpuBrick_p->m_numPfx && !curLpuBrick_p->ad_db)
        {
            kaps_assert(0, "AD DB is not set in the Brick\n");
            return NLMERR_FAIL;
        }

        /*Count the number of prefixes in the brick*/
        numPfxInLpu = 0;
        for (i = 0; i < curLpuBrick_p->m_maxCapacity; ++i)
        {
            pfx = curLpuBrick_p->m_pfxes[i];
            if (pfx)
            {
                ++numPfxInLpu;

                KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, pfx->m_backPtr->ad_handle, ad_db);

                /*Check the AD DB of Brick and Prefix */
                if (ad_db != curLpuBrick_p->ad_db)
                {
                    kaps_assert(0, " AD DB of Brick and Prefix don't match \n");
                    return NLMERR_FAIL;
                }
            }
        }

        /*Check if the number of prefixes in the brick is correct*/
        if (numPfxInLpu != curLpuBrick_p->m_numPfx)
        {
            kaps_assert(0, "Incorrect number of pfxes per LPU \n");
            return NLMERR_FAIL;
        }

        
        
        numPfxInLsn += numPfxInLpu;
        curLpuBrick_p = curLpuBrick_p->m_next_p;
    }

    if (numPfxInLsn != self->m_nNumPrefixes)
    {
        kaps_assert(0, "Incorrect number of pfxes per LSN \n");
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

void
kaps_lsn_mc_print(
    kaps_lsn_mc * self,
    FILE * fp)
{
    (void) self;
    (void) fp;
}

void
kaps_lsn_mc_printPrefix(
    kaps_lsn_mc * self,
    FILE * fp)
{
    (void) self;
    (void) fp;
}

/*  Warmboot - LSN recreation functions */
NlmErrNum_t
kaps_lsn_mc_wb_create_lsn(
    kaps_lsn_mc * lsn_p,
    struct kaps_wb_lsn_info *lsn_info,
    NlmReasonCode * o_reason)
{
    kaps_nlm_allocator *alloc_p = NULL;
    kaps_lpm_lpu_brick *new_brick = NULL, *head = NULL, *tail = NULL;
    kaps_lsn_mc_settings *lsn_settings = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    uint8_t alloc_udm[KAPS_MAX_NUM_CORES][KAPS_UDM_PER_UDC][KAPS_ALLOC_UDM_MAX_COLS];
    uint8_t max_lpus_in_a_chunk;
    uint8_t enable_over_allocation;
    struct kaps_db *db;
    struct kaps_ix_mgr *ix_mgr;

    uint32_t iter = 0;

    if (lsn_p == NULL)
    {
        *o_reason = NLMRSC_INVALID_POINTER;
        return NLMERR_FAIL;
    }

    if (lsn_p->m_pSettings == NULL || lsn_p->m_pTbl->m_alloc_p == NULL ||
         lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0] == NULL)
    {
        *o_reason = NLMRSC_INVALID_POINTER;
        return NLMERR_FAIL;
    }

    alloc_p = lsn_p->m_pTbl->m_alloc_p;
    lsn_settings = lsn_p->m_pSettings;

    lsn_p->m_bAllocedResource = 1;
    lsn_p->m_bDoGiveout = 0;
    lsn_p->m_bIsCompacted = 0;
    lsn_p->m_bIsNewLsn = 0;

    /*
     * get the index range need for the IxMgr from the IIT entry 
     */
    lsn_p->m_nAllocBase = lsn_info->base_address;
    lsn_p->m_nNumIxAlloced = lsn_info->ix_allocated;

    lsn_p->m_nNumPrefixes = 0;  /* counted on each unique prefix addition */
    lsn_p->m_lsnId = lsn_settings->m_lsnid++;
    lsn_p->m_nLsnCapacity = 0;  /* calculated based on the lpu-size and gran */

    /*
     * Get the memory for the LPU from the udaMgr 
     */
    if (!lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->chunk_map[0]
        && lsn_p->m_pTbl->m_db->device->type == KAPS_DEVICE_KAPS)
    {
        uint16_t no_of_regions;
        struct region region_info[MAX_UDA_REGIONS];

        kaps_memcpy(alloc_udm, lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->alloc_udm, sizeof(alloc_udm));
        db = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db;
        max_lpus_in_a_chunk = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->max_lpus_in_a_chunk;
        enable_over_allocation = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->enable_over_allocation;
        no_of_regions = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->no_of_regions;
        kaps_memcpy(region_info, lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->region_info, sizeof(region_info));

        kaps_uda_mgr_destroy(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]);
        kaps_uda_mgr_init(db->device, db, 0, lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p->alloc,
                          alloc_udm,
                          &lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0], max_lpus_in_a_chunk,
                          enable_over_allocation, 1, UDM_BOTH_HALF,
                          kaps_fib_uda_sub_block_copy_callback, kaps_fib_update_it_callback,
                          kaps_fib_kaps_update_lsn_size, 0);

        lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->no_of_regions = no_of_regions;
        kaps_memcpy(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->region_info, region_info, sizeof(region_info));
        KAPS_STRY(kaps_uda_mgr_wb_restore_regions
                  (lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0], lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p->alloc,
                   no_of_regions, region_info));
        kaps_trie_configure_uda_mgr(lsn_p->m_pTrie->m_trie_global, db, lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0],
                                    o_reason);
    }

    if (lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->chunk_map[0] == NULL)
        KAPS_STRY(kaps_uda_mgr_wb_restore_regions(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0],
                                                  lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p->alloc,
                                                  lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->no_of_regions,
                                                  lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->region_info));

    errNum = kaps_uda_mgr_wb_alloc(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0], lsn_p, (lsn_info->num_lpus),
                                   lsn_info->region_id, lsn_info->offset, lsn_p->m_lsnId, &lsn_p->m_mlpMemInfo);
    if (errNum != NLMERR_OK)
        return errNum;
    lsn_p->m_mlpMemInfo->db = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db;

    if (!lsn_p->m_pSettings->m_isPerLpuGran)
    {
        struct kaps_ad_db *ad_db = (struct kaps_ad_db *) lsn_p->m_pTrie->m_tbl_ptr->m_db->common_info->ad_info.ad;

        ix_mgr = kaps_lsn_mc_get_ix_mgr_for_lsn_pfx(lsn_p, ad_db, lsn_p->m_mlpMemInfo, 0);
        
        if (lsn_p->m_pSettings->m_isHardwareMappedIx)
        {
            errNum =
                kaps_lsn_mc_assign_hw_mapped_ix_per_lsn(lsn_p, lsn_p->m_mlpMemInfo, ad_db, &lsn_p->m_ixInfo, o_reason);
            lsn_p->m_nAllocBase = lsn_p->m_ixInfo->start_ix;
            lsn_p->m_nNumIxAlloced = (uint16_t) lsn_p->m_ixInfo->size;
        }
        else
        {
            errNum = kaps_ix_mgr_wb_alloc(ix_mgr, ad_db, lsn_p->m_nNumIxAlloced, lsn_p->m_nAllocBase,
                                          lsn_p, IX_USER_LSN, &lsn_p->m_ixInfo);
        }
    }
    if (errNum != NLMERR_OK)
        return errNum;


    /*
     * allocate memory for each brick in the LSN
     */
    for (iter = 0; iter < lsn_info->num_lpus; iter++)
    {
        new_brick = (kaps_lpm_lpu_brick *) kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_lpu_brick));
        if (!new_brick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        if (lsn_info->brick_info[iter].ad_db)
        {
            KAPS_STRY(kaps_ad_db_refresh_handle
                      (lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p, lsn_info->brick_info[iter].ad_db, &new_brick->ad_db));
        }

        new_brick->m_next_p = NULL;
        new_brick->m_gran = lsn_info->brick_info[iter].gran;
        new_brick->m_granIx = lsn_info->brick_info[iter].gran_ix;
        new_brick->meta_priority = lsn_info->brick_info[iter].meta_priority;
        new_brick->m_hasReservedSlot = lsn_info->brick_info[iter].m_hasReservedSlot;
        new_brick->m_underAllocatedIx = lsn_info->brick_info[iter].m_underAllocatedIx;
        new_brick->m_maxCapacity = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(lsn_p->m_pSettings, new_brick->ad_db,
                                                                         new_brick->m_hasReservedSlot,
                                                                         new_brick->meta_priority,
                                                                         lsn_info->brick_info[iter].gran);
        new_brick->m_numPfx = 0;

        lsn_p->m_nLsnCapacity += new_brick->m_maxCapacity;      /* cumulative prefixes */

        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            if (lsn_p->m_pSettings->m_isHardwareMappedIx)
            {
                kaps_lsn_mc_assign_hw_mapped_ix_per_lpu(lsn_p, new_brick, iter, new_brick->ad_db, &new_brick->m_ixInfo,
                                                        o_reason);
            }
            else
            {
                ix_mgr = kaps_lsn_mc_get_ix_mgr_for_lsn_pfx(lsn_p, new_brick->ad_db, 
                                                lsn_p->m_mlpMemInfo, iter);
                    
                errNum =
                    kaps_ix_mgr_wb_alloc(ix_mgr, new_brick->ad_db,
                                         lsn_info->brick_info[iter].brick_ix_allocated,
                                         lsn_info->brick_info[iter].base_address, lsn_p, IX_USER_LSN,
                                         &new_brick->m_ixInfo);
            }
        }

        new_brick->m_pfxes = (kaps_pfx_bundle **) kaps_nlm_allocator_calloc(alloc_p, new_brick->m_maxCapacity,
                                                                            sizeof(kaps_pfx_bundle *));
        if (!new_brick->m_pfxes)
        {
            kaps_nlm_allocator_free(alloc_p, new_brick);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        if (iter)
        {
            tail->m_next_p = new_brick;
            tail = new_brick;
        }
        else
        {
            head = new_brick;
            tail = new_brick;
        }
    }

    lsn_p->m_lpuList = head;
    lsn_p->m_numLpuBricks = lsn_info->num_lpus;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_wb_insert_pfx(
    kaps_lsn_mc * lsn_p,
    uint32_t insert_index,
    struct kaps_lpm_entry * entry)
{
    uint16_t max_pfx_in_lpu_brk = 0;
    uint32_t /* cur_lpu_num = 0, row_num = 0, */ which_brick = 0;
    uint32_t end_ix;

    /*
     * kaps_lsn_mc_settings *settings_p = lsn_p->m_pSettings;
     */
    kaps_lpm_lpu_brick *cur_lpu_brk;
    int32_t i = 0;
    uint32_t newix;
    uint32_t lpu_offset = 0;

    /*
     * Make sure resources are allocated for this LSN 
     */
    kaps_assert((lsn_p != NULL), "Not valid lsn ptr");
    kaps_assert(lsn_p->m_bAllocedResource, "Resources not allocated");

    kaps_assert((lsn_p->m_lpuList != NULL), " LSN LPU list is NULL");
    kaps_assert((lsn_p->m_mlpMemInfo != NULL), " LSN MLP info is NULL");
    if (!lsn_p->m_pSettings->m_isPerLpuGran)
    {
        kaps_assert((insert_index >= lsn_p->m_nAllocBase), " insert_index < base");
        kaps_assert((insert_index >= lsn_p->m_nAllocBase), " Index Location is out of range");
        kaps_assert((insert_index < lsn_p->m_nAllocBase + lsn_p->m_nNumIxAlloced), " Index Location is out of range");
    }

    cur_lpu_brk = lsn_p->m_lpuList;
    /*
     * cur_lpu_num = lsn_p->m_mlpMemInfo->start_lpu; row_num = lsn_p->m_mlpMemInfo->start_row;
     */

    /*
     * Find which brick the prefix goes into
     */
    kaps_assert(cur_lpu_brk, "Can't Insert Prefix at Location!!\n");
    newix = lsn_p->m_nAllocBase;
    end_ix = newix + cur_lpu_brk->m_maxCapacity - 1;
    if (lsn_p->m_pSettings->m_isPerLpuGran)
    {
        newix = cur_lpu_brk->m_ixInfo->start_ix;
        end_ix = newix + cur_lpu_brk->m_ixInfo->size - 1;
    }

    while (insert_index < newix || insert_index > end_ix)
    {
        cur_lpu_brk = cur_lpu_brk->m_next_p;
        kaps_assert(cur_lpu_brk, "Can't Insert Prefix at Location!!\n");
        newix += cur_lpu_brk->m_maxCapacity;
        end_ix += cur_lpu_brk->m_maxCapacity;
        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            newix = cur_lpu_brk->m_ixInfo->start_ix;
            end_ix = newix + cur_lpu_brk->m_ixInfo->size - 1;
        }
        which_brick++;
    }
    cur_lpu_brk = lsn_p->m_lpuList;

    while (which_brick)
    {
        if (!cur_lpu_brk)
        {
            kaps_assert(0, "Can't Insert Prefix at Location!!\n");
            return NLMERR_FAIL;
        }
        lpu_offset += cur_lpu_brk->m_maxCapacity;
        cur_lpu_brk = cur_lpu_brk->m_next_p;
        kaps_assert((cur_lpu_brk != NULL), "Not valid lbrick ");
        which_brick--;
    }

    if (cur_lpu_brk)
    {
        max_pfx_in_lpu_brk = cur_lpu_brk->m_maxCapacity;
        if (lsn_p->m_pSettings->m_isPerLpuGran)
            i = insert_index - cur_lpu_brk->m_ixInfo->start_ix;
        else
            i = (insert_index - lsn_p->m_nAllocBase - lpu_offset) % max_pfx_in_lpu_brk;

        if (lsn_p->m_pSettings->m_isPerLpuGran)
            newix = cur_lpu_brk->m_ixInfo->start_ix + i;
        else
            newix = lsn_p->m_nAllocBase + lpu_offset + i;
        if ((insert_index == newix) && (cur_lpu_brk->m_pfxes[i] == NULL))
        {
            cur_lpu_brk->m_pfxes[i] = entry->pfx_bundle;
            entry->pfx_bundle->m_nIndex = newix;
            entry->pfx_bundle->m_isLmpsofarPfx = 0;

            /*
             * Add the prefix bundle to the hash table
             */
            if (lsn_p->m_pTrie->m_hashtable_p && !entry->pfx_bundle->m_isPfxCopy)
                kaps_pfx_hash_table_insert(lsn_p->m_pTrie->m_hashtable_p, (struct kaps_entry *) entry);

        }
        else
        {
            kaps_assert(0, "Prefix already inserted!!\n");
            return NLMERR_FAIL;
        }
        cur_lpu_brk->m_numPfx++;        /* prefix count within lpu */
    }
    else
    {
        kaps_assert(0, "Can't Insert Prefix at Location!!\n");
        return NLMERR_FAIL;
    }

    lsn_p->m_nNumPrefixes++;    /* prefix count within lsn */

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_wb_prepare_lsn_info(
    kaps_lsn_mc * lsn_p,
    struct kaps_wb_lsn_info * lsn_info)
{
    uint8_t i;
    kaps_lpm_lpu_brick *brick = NULL;

    lsn_info->lsn_id = lsn_p->m_lsnId;
    lsn_info->num_lpus = lsn_p->m_numLpuBricks;
    lsn_info->region_id = lsn_p->m_mlpMemInfo->region_id;
    lsn_info->offset = lsn_p->m_mlpMemInfo->offset;
    lsn_info->base_address = lsn_p->m_nAllocBase;
    lsn_info->ix_allocated = lsn_p->m_nNumIxAlloced;

    brick = lsn_p->m_lpuList;
    i = 0;

    while (brick)
    {
        lsn_info->brick_info[i].gran = brick->m_gran;
        lsn_info->brick_info[i].gran_ix = brick->m_granIx;
        lsn_info->brick_info[i].meta_priority = brick->meta_priority;
        lsn_info->brick_info[i].m_hasReservedSlot = brick->m_hasReservedSlot;


        lsn_info->brick_info[i].m_underAllocatedIx = brick->m_underAllocatedIx;
        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            lsn_info->brick_info[i].base_address = brick->m_ixInfo->start_ix;
            lsn_info->brick_info[i].ad_db = brick->ad_db;
            lsn_info->brick_info[i].brick_ix_allocated = brick->m_ixInfo->size;
        }
        else
        {
            lsn_info->brick_info[i].ad_db = brick->ad_db;
        }
        
        i++;
        brick = brick->m_next_p;
    }
    lsn_info->special_entry_info.brick_ix_allocated = 0;


    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_wb_store_prefixes(
    kaps_lsn_mc * lsn_p,
    uint32_t * nv_offset,
    kaps_device_issu_write_fn write_fn,
    void *handle)
{
    kaps_lpm_lpu_brick *cur_lpu_brk;
    struct kaps_entry *entry;
    struct kaps_device *device;
    uint16_t iter = 0;
    uint32_t lopoff_8;
    uint16_t entries_in_brick = 0;
    uint32_t temp_offset = 0;
    uint32_t total_lsn_data_length_8 = 0;
    uint32_t offset = 0;
    uint32_t max_prefixes_in_brick = 0;
    struct kaps_wb_prefix_info prefix_info;
    uint32_t total_num_prefixes = 0;

    if (0 != write_fn(handle, (uint8_t *) & (lsn_p->m_nNumPrefixes), sizeof(lsn_p->m_nNumPrefixes), *nv_offset))
        return NLMERR_FAIL;
    *nv_offset += sizeof(lsn_p->m_nNumPrefixes);

    if (!lsn_p->m_nNumPrefixes)
        return NLMERR_OK;

    /*
     * This includes (RPT + IPT) lopoff 
     */
    lopoff_8 = lsn_p->m_nLopoff / 8;

    device = lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p;

    cur_lpu_brk = lsn_p->m_lpuList;
    while (cur_lpu_brk)
    {
        max_prefixes_in_brick = cur_lpu_brk->m_maxCapacity;
        entries_in_brick = 0;

        /*
         * For Per LPU Gran we have to store number of entries in each Brick 
         */
        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            if (cur_lpu_brk->m_underAllocatedIx)
                max_prefixes_in_brick = cur_lpu_brk->m_ixInfo->size;
            temp_offset = *nv_offset;
            *nv_offset += sizeof(entries_in_brick);
        }

        for (iter = 0; iter < max_prefixes_in_brick; iter++)
        {
            kaps_pfx_bundle *b = cur_lpu_brk->m_pfxes[iter];
            uint32_t index, len_8;
            uint16_t pfx_len, relative_index;

            if (b)
            {
                pfx_len = KAPS_PFX_BUNDLE_GET_PFX_SIZE(b);
                prefix_info.pfx_len = pfx_len;

                index = KAPS_PFX_BUNDLE_GET_INDEX(b);
                if (lsn_p->m_pSettings->m_isPerLpuGran)
                    relative_index = index - cur_lpu_brk->m_ixInfo->start_ix;
                else
                    relative_index = index - lsn_p->m_nAllocBase;

                prefix_info.index = relative_index;
                prefix_info.meta_priority = b->m_backPtr->meta_priority;
                prefix_info.pfx_copy = b->m_isPfxCopy;

                entry = (struct kaps_entry *) b->m_backPtr;

                prefix_info.user_handle = entry->user_handle;

                if (entry->ad_handle)
                {
                    prefix_info.ad_seq_num = KAPS_GET_AD_SEQ_NUM(entry->ad_handle);
                }

                if (0 != write_fn(handle, (uint8_t *) & (prefix_info), sizeof(prefix_info), *nv_offset))
                    return NLMERR_FAIL;
                *nv_offset += sizeof(prefix_info);

                len_8 = (pfx_len + 7) / 8;
                len_8 -= lopoff_8;      /* Store only the remainder of the prefix */

                total_lsn_data_length_8 += len_8;

                if (entry->ad_handle)
                {
                    struct kaps_ad_db *ad_db;
                    uint32_t ad_user_hdl = entry->ad_handle->user_handle;

                    KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, ad_db)
                        if (0 != write_fn(handle, (uint8_t *) & (ad_user_hdl), sizeof(ad_user_hdl), *nv_offset))
                        return NLMERR_FAIL;
                    *nv_offset += sizeof(ad_user_hdl);


                    if (0 !=
                        write_fn(handle, (uint8_t *) & (entry->ad_handle->value[ad_db->byte_offset]),
                                 sizeof(uint8_t) * ad_db->user_bytes, *nv_offset))
                        return NLMERR_FAIL;
                    *nv_offset += (sizeof(uint8_t) * ad_db->user_bytes);

                    {
                        uintptr_t hb_user_handle = 0;
                        if (b->m_backPtr->hb_user_handle)
                            hb_user_handle = b->m_backPtr->hb_user_handle;

                        if (0 != write_fn(handle, (uint8_t *) & (hb_user_handle), sizeof(hb_user_handle), *nv_offset))
                            return NLMERR_FAIL;
                        *nv_offset += sizeof(hb_user_handle);
                    }
                }
                entries_in_brick++;
            }
        }

        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            if (0 != write_fn(handle, (uint8_t *) & (entries_in_brick), sizeof(entries_in_brick), temp_offset))
                return NLMERR_FAIL;
        }
        total_num_prefixes += entries_in_brick;
        cur_lpu_brk = cur_lpu_brk->m_next_p;
    }

    cur_lpu_brk = lsn_p->m_lpuList;
    while (cur_lpu_brk)
    {
        max_prefixes_in_brick = cur_lpu_brk->m_maxCapacity;
        if (lsn_p->m_pSettings->m_isPerLpuGran && cur_lpu_brk->m_underAllocatedIx)
            max_prefixes_in_brick = cur_lpu_brk->m_ixInfo->size;

        for (iter = 0; iter < max_prefixes_in_brick; iter++)
        {
            kaps_pfx_bundle *b = cur_lpu_brk->m_pfxes[iter];
            uint8_t *pfx_data = NULL;
            uint32_t len_8;
            uint16_t pfx_len;

            if (b)
            {
                pfx_len = KAPS_PFX_BUNDLE_GET_PFX_SIZE(b);

                len_8 = (pfx_len + 7) / 8;
                len_8 -= lopoff_8;      /* Store only the remainder of the prefix */

                pfx_data = KAPS_PFX_BUNDLE_GET_PFX_DATA(b);
                kaps_memcpy(&lsn_p->m_pSettings->lsn_data_buffer[offset], &pfx_data[lopoff_8], len_8);
                offset += len_8;
            }
        }
        cur_lpu_brk = cur_lpu_brk->m_next_p;
    }

    /*
     * Store the LSN Data
     */
    if (0 != write_fn(handle, (uint8_t *) & (lsn_p->m_pSettings->lsn_data_buffer), total_lsn_data_length_8, *nv_offset))
        return NLMERR_FAIL;
    *nv_offset += total_lsn_data_length_8;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_lsn_mc_wb_restore_pfx_data(
    kaps_lsn_mc * lsn_p,
    uint32_t * nv_offset,
    kaps_device_issu_read_fn read_fn,
    void *handle)
{
    struct kaps_wb_prefix_data **prefix = NULL;
    kaps_lpm_lpu_brick *cur_lpu_brk;
    kaps_pfx_bundle *pfx_bundle = NULL;
    uint16_t i, iter, num_prefixes = 0;
    uint32_t lopoff_8, len_8;
    uint16_t entries_in_brick = 0;
    uint32_t total_lsn_data_length_8 = 0;
    uint32_t offset = 0;
    struct kaps_ad_db *ad_db;
    uintptr_t hb_user_handle;
    uint32_t num_bytes_to_alloc;

    if (0 != read_fn(handle, (uint8_t *) & (num_prefixes), sizeof(num_prefixes), *nv_offset))
        return NLMERR_FAIL;
    *nv_offset += sizeof(num_prefixes);

    if (!num_prefixes)
        return NLMERR_OK;

    /*
     * This includes (RPT + IPT) lopoff 
     */
    lopoff_8 = lsn_p->m_nLopoff / 8;
    pfx_bundle = lsn_p->m_pParentHandle;

    cur_lpu_brk = lsn_p->m_lpuList;
    if (lsn_p->m_pSettings->m_isPerLpuGran && num_prefixes)
    {
        if (0 != read_fn(handle, (uint8_t *) & (entries_in_brick), sizeof(entries_in_brick), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(entries_in_brick);
    }

    prefix =
        kaps_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, num_prefixes, sizeof(struct kaps_wb_prefix_data *));
    if (!prefix)
        return NLMERR_FAIL;

    for (iter = 0; iter < num_prefixes; iter++)
    {
        prefix[iter] = kaps_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_wb_prefix_data));

        if (!prefix[iter])
        {
            for (i = 0; i < iter; ++i)
            {
                kaps_nlm_allocator_free(lsn_p->m_pSettings->m_pAlloc, prefix[i]);
            }

            kaps_nlm_allocator_free(lsn_p->m_pSettings->m_pAlloc, prefix);
            return NLMERR_FAIL;
        }
    }

    for (iter = 0; iter < num_prefixes; iter++)
    {
        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            while (!entries_in_brick)
            {
                if (0 != read_fn(handle, (uint8_t *) & (entries_in_brick), sizeof(entries_in_brick), *nv_offset))
                    return NLMERR_FAIL;
                *nv_offset += sizeof(entries_in_brick);
                cur_lpu_brk = cur_lpu_brk->m_next_p;
            }
        }

        if (0 != read_fn(handle, (uint8_t *) & prefix[iter]->info, sizeof(prefix[iter]->info), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(prefix[iter]->info);

        len_8 = (prefix[iter]->info.pfx_len + 7) / 8;

        num_bytes_to_alloc = len_8;
        if (num_bytes_to_alloc == 0)
            num_bytes_to_alloc = 1;

        prefix[iter]->data = kaps_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, num_bytes_to_alloc,
                                                       sizeof(uint8_t));

        if (!prefix[iter]->data)
            return NLMERR_FAIL;

        /*
         * Re-create the prefix 
         */
        len_8 -= lopoff_8;      /* prefix remainder length */
        total_lsn_data_length_8 += len_8;
        kaps_memcpy(prefix[iter]->data, pfx_bundle->m_data, lopoff_8);  /* Common portion */

        if (lsn_p->m_pSettings->m_isPerLpuGran)
            prefix[iter]->info.index += cur_lpu_brk->m_ixInfo->start_ix;
        else
            prefix[iter]->info.index += lsn_p->m_nAllocBase;

        if (lsn_p->m_pSettings->m_isPerLpuGran)
            ad_db = cur_lpu_brk->ad_db;
        else
            ad_db = (struct kaps_ad_db *) lsn_p->m_pTbl->m_db->common_info->ad_info.ad;

        if (ad_db)
        {
            uint32_t ad_bytes = ad_db->user_bytes;
            uint32_t ad_db_seq_num = prefix[iter]->info.ad_seq_num;
            struct kaps_device *device = lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p;

            kaps_sassert(ad_db_seq_num);
            kaps_sassert(ad_db_seq_num <= device->seq_num_to_ptr->num_ad_db);
            kaps_sassert(device->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num]);
            ad_db = device->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num];
            ad_bytes = ad_db->user_bytes;

            if (0 !=
                read_fn(handle, (uint8_t *) & (prefix[iter]->ad_user_handle), sizeof(prefix[iter]->ad_user_handle),
                        *nv_offset))
                return NLMERR_FAIL;
            *nv_offset += sizeof(prefix[iter]->ad_user_handle);


            if (ad_bytes)
            {
                prefix[iter]->ad = kaps_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, ad_bytes, sizeof(uint8_t));

                if (!prefix[iter]->ad)
                    return NLMERR_FAIL;

                if (0 != read_fn(handle, (uint8_t *) prefix[iter]->ad, sizeof(uint8_t) * ad_bytes, *nv_offset))
                    return NLMERR_FAIL;
                *nv_offset += (sizeof(uint8_t) * ad_bytes);
            }

            if (0 != read_fn(handle, (uint8_t *) & (hb_user_handle), sizeof(hb_user_handle), *nv_offset))
                return NLMERR_FAIL;
            *nv_offset += sizeof(hb_user_handle);

            if (device->type == KAPS_DEVICE_KAPS && lsn_p->m_pSettings->m_areHitBitsPresent)
            {

                prefix[iter]->hb_user_handle = hb_user_handle;

                prefix[iter]->hb_no = kaps_kaps_get_old_pfx_hb_index(lsn_p, prefix[iter]->info.index);
            }

        }

        if (!prefix[iter]->info.pfx_copy)
            kaps_c_list_add_tail(&lsn_p->m_pTbl->wb_prefixes, &prefix[iter]->kaps_list_node);
        else
            kaps_c_list_add_tail(&lsn_p->m_pTbl->wb_apt_pfx_copies, &prefix[iter]->kaps_list_node);
        if (lsn_p->m_pSettings->m_isPerLpuGran)
            entries_in_brick--;
    }

    if (lsn_p->m_pSettings->m_isPerLpuGran && num_prefixes)
    {
        while (cur_lpu_brk && cur_lpu_brk->m_next_p)
        {
            if (0 != read_fn(handle, (uint8_t *) & (entries_in_brick), sizeof(entries_in_brick), *nv_offset))
                return NLMERR_FAIL;
            *nv_offset += sizeof(entries_in_brick);
            kaps_sassert(entries_in_brick == 0);
            cur_lpu_brk = cur_lpu_brk->m_next_p;
        }
    }
    /*
     * Restore the LSN Data
     */
    if (0 != read_fn(handle, (uint8_t *) & (lsn_p->m_pSettings->lsn_data_buffer), total_lsn_data_length_8, *nv_offset))
        return NLMERR_FAIL;
    *nv_offset += total_lsn_data_length_8;

    for (iter = 0; iter < num_prefixes; iter++)
    {
        len_8 = (prefix[iter]->info.pfx_len + 7) / 8;
        len_8 -= lopoff_8;

        /*
         * Copy the remainder here 
         */
        kaps_memcpy(&prefix[iter]->data[lopoff_8], &lsn_p->m_pSettings->lsn_data_buffer[offset], len_8);
        offset += len_8;
    }

    kaps_nlm_allocator_free(lsn_p->m_pSettings->m_pAlloc, prefix);

    return NLMERR_OK;
}

