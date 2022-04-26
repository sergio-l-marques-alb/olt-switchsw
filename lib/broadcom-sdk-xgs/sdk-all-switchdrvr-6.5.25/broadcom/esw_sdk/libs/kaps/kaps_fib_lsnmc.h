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

#ifndef INCLUDED_KAPS_LSNMC_H
#define INCLUDED_KAPS_LSNMC_H

#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_fib_cmn_nstblrqt.h"
#include "kaps_fib_triedata.h"
#include "kaps_ix_mgr.h"
#include "kaps_fib_poolmgr.h"
#include "kaps_device_internal.h"
#include "kaps_uda_mgr.h"
#include "kaps_hw_limits.h"
#include "kaps_device_alg.h"
#include "kaps_fib_hw.h"
#include "kaps_hb_internal.h"

#include "kaps_externcstart.h"

#define KAPS_GIVEOUT_BUF_SIZE (KAPS_MAX_TMAX_VAL + 1)

#define KAPS_MAX_NUM_STORED_LSN_INFO (3)

#define KAPS_KEEP (1)
#define KAPS_GIVE (2)

typedef struct kaps_lpm_lpu_brick
{
    kaps_pfx_bundle **m_pfxes;  /* Array of Prefix bundles in the brick */
    struct kaps_ix_chunk *m_ixInfo;     /* Ix chuck related to this brick */
    struct kaps_ad_db *ad_db;   /* AD database associated with brick */
    struct kaps_lpm_lpu_brick *m_next_p;        /* Linked list pointer */
    uint16_t m_gran;            /* Gran of the brick */
    uint16_t m_numPfx;          /* Number of prefixes currently present in the brick */
    uint32_t m_granIx:8;        /* Gran Index */
    uint32_t meta_priority:2;   /* Meta priority associated with brick */
    uint32_t m_maxCapacity:16;  /* Maximum number of prefixes that can be stored in the brick */
    uint32_t m_hasReservedSlot:1;       /* If is has reserved slot */
    uint32_t m_underAllocatedIx:1;      /* Have we under allocated the Ix */
} kaps_lpm_lpu_brick;

/* Following granularities are supported with the device */

typedef struct kaps_lsn_mc_settings
{
    kaps_nlm_allocator *m_pAlloc;       /* Allocator used with MC LSN */
    kaps_fib_tbl *m_fibTbl;

    struct kaps_uda_mgr **m_pMlpMemMgr;
    struct uda_mem_chunk *m_pTmpMlpMemInfoBuf[KAPS_MAX_NUM_STORED_LSN_INFO];

    struct uda_mem_chunk *m_oldMlpMemInfoPtr;
    struct kaps_ix_chunk *m_oldIxChunkForLsn;

    struct kaps_ix_chunk *m_oldLpuIxChunk[KAPS_HW_MAX_LPUS_PER_LPM_DB];
    kaps_lpm_lpu_brick *m_oldLpuBrick[KAPS_HW_MAX_LPUS_PER_LPM_DB];
    uint32_t m_numBackupLpuBrickResources;

    uint8_t m_isPlaced[KAPS_MAX_NUM_PFX_IN_BUFFER];

    kaps_fib_prefix_index_changed_app_cb_t m_pAppCB;    /* Index changed Application Callback */
    void *m_pAppData;           /* Application data passed with callback function */

    hb_ix_changed_callback m_pHitBitCB;
    void *m_pHitBitCBData;

    uint8_t zero_value_array[KAPS_BKT_WIDTH_8];

    uint8_t common_bits_array[KAPS_LPM_KEY_MAX_WIDTH_8];

    struct kaps_device *m_device;

    uint32_t m_cmpLookupTable[256];

    uint16_t lpm_num_gran; /* Number of granularities present */

    uint16_t m_lengthToGran[KAPS_HW_LPM_MAX_GRAN + 1];  /* An extra 1 is added because the length can be from 0 to
                                                         * max_gran */
    uint16_t m_lengthToGranIx[KAPS_HW_LPM_MAX_GRAN + 1];

    uint16_t m_middleLevelLengthToGran[KAPS_HW_LPM_MAX_GRAN + 1];       /* An extra 1 is added because the length can
                                                                         * be from 0 to max_gran */
    uint16_t m_middleLevelLengthToGranIx[KAPS_HW_LPM_MAX_GRAN + 1];

    uint16_t m_maxPfxInBrickForGranIx[KAPS_HW_MAX_NUM_LPU_GRAN];
    uint16_t m_max20bInPlacePfxInBrickForGranIx[KAPS_HW_MAX_NUM_LPU_GRAN];
    uint32_t m_maxTriggersInBrickForGranIx[KAPS_HW_MAX_NUM_LPU_GRAN];
    
    uint16_t m_granIxToGran[KAPS_HW_MAX_NUM_LPU_GRAN];
    uint16_t m_numHolesForGranIx[KAPS_HW_MAX_NUM_LPU_GRAN];

    uint16_t m_granIxToFormatValue[KAPS_MAX_NUM_GRANULARITIES];
    uint16_t m_middleLevelGranIxToFormatValue[KAPS_MAX_NUM_GRANULARITIES];

    uint16_t m_maxNumPfxInAnyBrick;
    uint16_t m_maxNumInPlacePfxInAnyBrick;

    uint32_t m_isGrowEnhEnable:1;
    uint32_t m_isPerLpuGran:1;
    uint32_t m_isAptPresent:1;
    uint32_t dynamic_alloc_enable:1;
    uint32_t m_isHardwareMappedIx:1;
    uint32_t m_isUnderAllocateLpuIx:1;
    uint32_t m_numUnderallocateLpus:4;
    uint32_t m_isShrinkEnabled:1;
    uint32_t m_isFullWidthLsn:1;
    uint32_t m_isMultiBrickAlloc:1;
    uint32_t m_areHitBitsPresent:1;
    uint32_t m_areIPTHitBitsPresent:1;
    uint32_t m_areRPTHitBitsPresent:1;
    uint32_t m_strictlyStoreLmpsofarInAds:1;

    uint32_t m_bdata_ads_size_1;

    uint32_t m_maxTriggersPerBktRow;

    
    /*
     * For OP with lsnLmpsofar, we have to expand the user prefix into a prefix copy. The prefix copy can't store the
     * length of the original user prefix. So it is not possible to resolve across different databases. Also we can't
     * store the metapriority of the user prefix in the prefix copy. So supporting multiple meta-priorities with
     * lsnLmpsofar in OP is not possible. So it is ok to keep either the user prefix or the prefix copy in the reserved 
     * slot. In OP2 however, the hardware supports prefix copy to have its own length and meta-priority. So we can
     * store the user prefix length in the prefix copy. This allows us to resolve across databases. Also different
     * meta-priorities can be used. To support this, we have to store only the prefix copy in the reserved slot 
     */
    uint32_t m_onlyPfxCopyInReservedSlot:1;
    uint32_t m_recomputeLmpOnDelete:1;
    uint32_t m_isJoinedUdc:1;
    uint32_t m_splitIfLowUda:1;
    uint32_t m_isAncestorLsnMergeEnabled:1;
    uint32_t m_isRelatedLsnMergeEnabled:1;
    uint32_t m_treat_20b_ad_as_inplace:1;
    uint32_t m_lpu_brick_width_1;
    uint32_t m_numMultiBricks;
    uint32_t m_num_extra_holes_per_lsn;
    uint32_t m_bottom_up_caution_height;        /* If we are going deeper than this height, then we will be forming
                                                 * sparse LSNs where the number of bit combinations will be less than
                                                 * the number of prefixes based on granularity */

    uint32_t m_maxLpuPerLsn;
    uint32_t m_initialMaxLpuPerLsn;     /* The LSN width at the beginning */

    uint32_t m_numJoinedSets;
    uint32_t m_numJoinedBricksInOneSet;

    uint8_t m_give_or_keep[KAPS_GIVEOUT_BUF_SIZE];
    uint32_t m_lsnid;
    uint8_t lsn_data_buffer[KAPS_HW_LPU_WIDTH_8 * KAPS_HW_MAX_LPUS_PER_LSN * 2];        /* Keeping additional space for 
                                                                                         * round off bits to byte */

    uint8_t m_isLsnInfoValid[KAPS_MAX_NUM_STORED_LSN_INFO];

    uint32_t m_maxCapacityOfStoredLsnInfo[KAPS_MAX_NUM_STORED_LSN_INFO][KAPS_HW_MAX_LPUS_PER_LPM_DB];
    uint32_t m_numBricksInStoredLsnInfo[KAPS_MAX_NUM_STORED_LSN_INFO];
    uint32_t m_startIxOfStoredLsnInfo[KAPS_MAX_NUM_STORED_LSN_INFO];
    uint32_t m_regionIdOfStoredLsnInfo[KAPS_MAX_NUM_STORED_LSN_INFO];

    int32_t m_numAllocatedBricksForAllLsns;

} kaps_lsn_mc_settings;

/* This is LSN structure */
typedef struct kaps_lsn_mc
{

    kaps_lsn_mc_settings *m_pSettings;  /* LSN settings common to all LSNs */
    struct kaps_ix_chunk *m_ixInfo;
    struct uda_mem_chunk *m_mlpMemInfo;
    struct kaps_lpm_trie *m_pTrie;      /* This MC LSN belongs to this trie */

    kaps_fib_tbl *m_pTbl;       /* Prefixes in the LSN belong to this table */

    /*
     * This is same as corresponding trienode's prefix bundle. We can get trienode pointer from the AssocData of this
     * prefix bundle. This member is needed as trie algorithms access to get the trienode pointer from it 
     */
    kaps_pfx_bundle *m_pParentHandle;

    kaps_lpm_lpu_brick *m_lpuList;

    uint8_t m_numLpuBricks;

    /*
     * Number of prefixes currently present in the LSN 
     */
    uint16_t m_nNumPrefixes;

    /*
     * Total number of prefixes that can be stored in the LSN 
     */
    uint16_t m_nLsnCapacity;

    /*
     * Base address assigned to this LSN used with index space management for the prefixes within this LSN 
     */
    uint32_t m_nAllocBase;

    /*
     * Number of indices currently allocated for this LSN. 
     */
    uint16_t m_nNumIxAlloced;

    /*
     * Number of delete operations performed on the LSN
     */
    uint16_t m_numDeletes;

    uint32_t m_lsnId;

    /*
     * Pack the following members within one uint32 to optimize the memory requirement. * m_devid : this LSN residing
     * on this device. * m_nDepth : depth of the trienode this LSN belongs to. * m_nLopoff : These many bits are lopped 
     * off by the SES entry pointing to this LSN. * * In the members below, more bits are allocated than required to
     * make exact packing in 32 bits. * For e.g. for m_devid, 2 bits are enough as at max 4 devices can be cascaded. *
     * 9 bits are enough for for m_nDepth; maximum length of SES entry is 320 bits which means * 9 bits are enough to
     * store the depth. Same is true with m_nLopoff member 
     */
    uint32_t m_devid:4;
    uint32_t m_nDepth:12;
    uint32_t m_nLopoff:12;

    /**
      * m_bIsNewLsn : This flag is set when a LSN is newly created
      *
      *
      * m_bAllocedResource : This flag is set during LSN resource allocation. Resource allocation
      *                                  includes allocation of space on the hardware, allocation of index
      *                                  space for this LSN
      */
    /*
     * Using u32 to get rid of compilation warnings 
     */
    uint32_t m_bIsNewLsn:1;

    uint32_t m_bAllocedResource:1;
    uint32_t m_bIsCompacted:1;
    uint32_t m_bDoGiveout:1;

} kaps_lsn_mc;

typedef struct kaps_flat_lsn_data
{
    kaps_prefix *m_commonPfx;
    kaps_pfx_bundle *m_pfxesInLsn[KAPS_GIVEOUT_BUF_SIZE];
    struct kaps_ad_db *ad_colors[16];
    uint16_t meta_priority_colors[16];
    uint16_t pfx_color[KAPS_GIVEOUT_BUF_SIZE];
    uint16_t num_colors;
    uint16_t m_numPfxInLsn;
    uint16_t m_maxPfxLenInBits;
    kaps_pfx_bundle *m_iitLmpsofarPfx;
    uint8_t isGiveLsn;          /* Indicates if it is a Give LSN or a Keep LSN */
    kaps_lsn_mc *lsn_cookie;
    struct uda_mem_chunk *temp_uda_chunk;
} kaps_flat_lsn_data;

#define kaps_lsn_mc_get_prefix_count(self)               ((self)->m_nNumPrefixes)

/* Functions exposed by the LSN Manager */
kaps_lsn_mc_settings *kaps_lsn_mc_settings_create(
    kaps_nlm_allocator * pAlloc,
    kaps_fib_tbl * fibTbl,
    struct kaps_device *device,
    struct kaps_uda_mgr **pMlpMemMgr,
    kaps_fib_prefix_index_changed_app_cb_t pAppCB,
    void *pAppData,
    uint32_t numLpuPerRow,
    uint32_t numJoinedSets,
    uint32_t numJoinedBricksInOneSet);

void kaps_lsn_mc_settings_destroy(
    kaps_lsn_mc_settings * self);

uint32_t kaps_lsn_mc_get_index(
    kaps_lsn_mc * self,
    uint32_t relative_index);

uint32_t kaps_lsn_mc_get_index_in_lsn(
    kaps_lsn_mc * self,
    uint32_t actual_index);

kaps_lsn_mc *kaps_lsn_mc_create(
    kaps_lsn_mc_settings * settings,
    struct kaps_lpm_trie *pTrie,
    uint32_t depth);

void kaps_lsn_mc_destroy(
    kaps_lsn_mc * self);

void kaps_lsn_mc_free_single_lpu_brick(
    kaps_lpm_lpu_brick * curLpuBrick,
    kaps_nlm_allocator * alloc);

void kaps_lsn_mc_free_lpu_bricks(
    kaps_lpm_lpu_brick * curLpuBrick,
    kaps_nlm_allocator * alloc);

void kaps_lsn_mc_free_resources(
    kaps_lsn_mc * self);

void kaps_lsn_mc_free_resources_per_lpu(
    kaps_lsn_mc * self);

NlmErrNum_t kaps_lsn_mc_undo_acquire_resources(
    kaps_lsn_mc * self);

NlmErrNum_t kaps_lsn_mc_free_old_resources(
    kaps_lsn_mc * self);

NlmErrNum_t kaps_lsn_mc_backup_resources_per_lpu(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * brick,
    struct kaps_ix_chunk *oldIxInfo);

NlmErrNum_t kaps_lsn_mc_undo_acquire_resources_per_lpu(
    kaps_lsn_mc * self);

NlmErrNum_t kaps_lsn_mc_free_old_resources_per_lpu(
    kaps_lsn_mc * self);

NlmErrNum_t kaps_lsn_mc_add_extra_brick_for_joined_udcs(
    kaps_lsn_mc * self,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_submit_rqt(
    kaps_lsn_mc * self,
    NlmTblRqtCode rqtCode,
    struct kaps_lpm_entry *entry,
    struct kaps_lpm_entry **entrySlotInHash,
    NlmReasonCode * o_reason);

uint8_t *kaps_lsn_mc_calc_common_bits(
    kaps_lsn_mc_settings * settings,
    kaps_flat_lsn_data * flatLsnData_p,
    uint32_t * retlen);

uint16_t kaps_lsn_mc_compute_gran(
    kaps_lsn_mc_settings * settings,
    uint32_t lengthAfterLopoff,
    uint16_t * o_granIx);

uint16_t kaps_lsn_mc_get_next_max_gran(
    kaps_lsn_mc_settings * settings,
    uint16_t gran,
    uint16_t next);

uint16_t kaps_lsn_mc_calc_max_pfx_in_lpu_brick(
    kaps_lsn_mc_settings * settings,
    struct kaps_ad_db *ad_db,
    uint32_t has_reservedSlot,
    uint16_t meta_priority,
    uint16_t gran);

uint32_t
kaps_lsn_mc_calc_max_triggers_in_brick(
    kaps_lsn_mc_settings * settings,
    uint16_t gran);


kaps_pfx_bundle *kaps_lsn_mc_locate_exact(
    kaps_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    uint32_t * match_brick_num);

kaps_pfx_bundle *kaps_lsn_mc_locate_lpm(
    kaps_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    struct kaps_lpm_entry *skipEntry,
    uint32_t * matchBrickIter,
    uint32_t * matchPos);

void kaps_lsn_mc_free_prefixes_safe(
    kaps_lsn_mc * self);

void kaps_lsn_mc_print(
    kaps_lsn_mc * self,
    FILE * fp);

void kaps_lsn_mc_printPrefix(
    kaps_lsn_mc * self,
    FILE * fp);

uint16_t kaps_lsn_mc_get_first_lpu_granIx(
    kaps_lsn_mc * self);

uint16_t kaps_lsn_mc_get_first_lpu_gran(
    kaps_lsn_mc * self);

void kaps_lsn_mc_update_prefix(
    kaps_lsn_mc * self,
    kaps_pfx_bundle * b,
    uint32_t oldix,
    uint32_t nextix);

NlmErrNum_t kaps_lsn_mc_giveout(
    kaps_lsn_mc * originalLsn,
    kaps_lsn_mc_settings * settings,
    kaps_fib_tbl *fibTbl,
    kaps_flat_lsn_data * curGiveoutData_p,
    kaps_flat_lsn_data * keepData_p,
    kaps_flat_lsn_data * giveData_p,
    kaps_pfx_bundle * pfxBundleToInsert,
    kaps_nlm_allocator * alloc_p,
    uint32_t tryOnlyLsnPush,
    NlmReasonCode * o_reason);

kaps_flat_lsn_data *kaps_lsn_mc_create_flat_lsn_data(
    kaps_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_destroy_flat_lsn_data(
    kaps_nlm_allocator * alloc_p,
    kaps_flat_lsn_data * flatLsnData_p);

NlmErrNum_t kaps_lsn_mc_convert_lsn_to_flat_data(
    kaps_lsn_mc * self,
    kaps_pfx_bundle * extraPfxArray[],
    uint32_t numExtraPfx,
    kaps_flat_lsn_data * flatLsnData_p,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_convert_flat_data_to_lsn(
    kaps_flat_lsn_data * flatLsnData_p,
    kaps_lsn_mc * self,
    uint32_t * doesLsnFit_p,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_acquire_resources(
    kaps_lsn_mc * self,
    uint32_t ixRqtSize,
    uint8_t numLpuBricks,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_acquire_resources_per_lpu(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * brick,
    uint32_t brickIter,
    uint32_t ixRqtSize,
    struct kaps_ad_db *ad_db,
    NlmReasonCode * o_reason);

void kaps_lsn_mc_assign_flat_data_colors(
    kaps_lsn_mc_settings * settings,
    kaps_flat_lsn_data * flatLsnData_p);

NlmErrNum_t kaps_lsn_mc_assign_hw_mapped_ix_per_lpu(
    kaps_lsn_mc * self,
    kaps_lpm_lpu_brick * brick,
    uint32_t brickIter,
    struct kaps_ad_db *ad_db,
    struct kaps_ix_chunk **ixInfo_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_assign_hw_mapped_ix_per_lsn(
    kaps_lsn_mc * self,
    struct uda_mem_chunk *mlpMemInfo,
    struct kaps_ad_db *ad_db,
    struct kaps_ix_chunk **ixInfo_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_add_empty_bricks(
    kaps_lsn_mc * self,
    uint32_t pfxGran,
    uint32_t pfxGranIx,
    uint32_t totalNumBricks,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_store_old_lsn_info(
    kaps_lsn_mc * curLsn,
    uint32_t lsnInfoIndex);

NlmErrNum_t kaps_lsn_mc_sort_and_resize_per_lpu_gran(
    kaps_lsn_mc * self,
    struct kaps_lpm_entry *entryToInsert,
    kaps_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_rearrange_prefixes_for_joined_udcs(
    kaps_lsn_mc * curLsn,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_shrink_per_lsn_gran(
    kaps_lsn_mc * self,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_shrink_per_lpu_gran(
    kaps_lsn_mc * self,
    NlmReasonCode * o_reason);

uint32_t kaps_lsn_mc_find_longest_prefix_length(
    kaps_lsn_mc * curLsn);

NlmErrNum_t kaps_lsn_mc_find_prefixes_in_path(
    kaps_lsn_mc * curLsn,
    uint8_t * pathDataToCheck,
    uint32_t pathLengthToCheck,
    uint32_t excludeExactLengthMatch,
    kaps_pfx_bundle * pfxArray[],
    uint32_t pfxLocationsToMove_p[],
    uint32_t * numPfxInPath,
    uint32_t *isExactMatchLenPfxPresent);

uint32_t
kaps_lsn_mc_find_if_next_brick_is_joined(
    kaps_lsn_mc * curLsn,
    uint32_t brick_iter);


struct kaps_ad_db *
kaps_lsn_mc_get_ad_db(
    kaps_lsn_mc *curLsn,
    kaps_lpm_lpu_brick *curBrick);


struct kaps_ix_mgr*
kaps_lsn_mc_get_ix_mgr_for_lsn_pfx(
    kaps_lsn_mc * self,
    struct kaps_ad_db *ad_db,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum);


struct kaps_ix_mgr*
kaps_lsn_mc_get_ix_mgr_for_lmpsofar_pfx(
    struct kaps_ad_db *ad_db);




NlmErrNum_t kaps_lsn_mc_verify(
    kaps_lsn_mc * self);

/**
 * Creates LSN during warmboot restore phase. LSN software structure members are initialized with suitable values
 * using the IIT entry that is one parameter to this function.
 *
 * @param lsn_p Pointer to the LSN that is being recreated
 * @param lsn_info LSN information corresponding to this LSN
 * @param o_reason Reason code that contains details in case of any failure
 *
 * @return NLMERR_OK on success or an error code otherwise
 */
uint32_t kaps_lsn_mc_wb_create_lsn(
    kaps_lsn_mc * lsn_p,
    struct kaps_wb_lsn_info *lsn_info,
    NlmReasonCode * o_reason);

/**
 * Adds prefix to an LSN during warmboot restore phase. insert_index is used to determine the location
 * within LSN where the prefix should be added
 *
 * @param lsn_p Pointer to LSN
 * @param insert_index user index of the lpm entry that is being added
 * @param entry LPM entry that is being added to the LSN
 *
 * @return NLMERR_OK on success or error code otherwise
 */
NlmErrNum_t kaps_lsn_mc_wb_insert_pfx(
    kaps_lsn_mc * lsn_p,
    uint32_t insert_index,
    struct kaps_lpm_entry *entry);

/**
 * Stores LSN prefixes to non-volatile memory. Prefix length, remainder of the prefix after (RPT + IPT) lopoff,
 * relative index of this prefix within the LSN and the user handle of prefix are stored. It is sufficient to
 * store remainder of the prefix as the common portion for this LSN is available during the warmboot restore phase.
 * Relative index is sufficient as base address assigned to this LSN is available in the IIT entry. User handle
 * assigned to each prefix must be stored as user handle must be same through out the life of the run
 *
 * @param lsn_p Pointer to LSN whose prefixes are stored during warmboot store phase
 * @param nv_offset Starting offset in the non-volatile memory to write to
 * @param write_fn Callback function to write data to non-volatile memory
 * @param handle User handle which will be passed back through read and write functions
 *
 * @return NLMERR_OK on success or error code otherwise
 */
NlmErrNum_t kaps_lsn_mc_wb_store_prefixes(
    kaps_lsn_mc * lsn_p,
    uint32_t * nv_offset,
    kaps_device_issu_write_fn write_fn,
    void *handle);

/**
 * Restores prefixes during warmboot restore phase. Prefixes re-created are not instantly added back to the LSN but
 * postponed to later stage of the warmboot restore phase. Re-created prefixes are added to a list. This list is
 * maintained per database. These restored prefixes are internally played at the end of the warmboot restore phase.
 *
 * @param lsn_p LSN pointer
 * @param nv_offset Starting offset in the non-volatile memory to read from.
 * @param read_fn Callback function to read data from non-volatile memory
 * @param handle User handle which will be passed back through read and write functions
 *
 * @return NLMERR_OK on success or error code otherwise
 */
NlmErrNum_t kaps_lsn_mc_wb_restore_pfx_data(
    kaps_lsn_mc * lsn_p,
    uint32_t * nv_offset,
    kaps_device_issu_read_fn read_fn,
    void *handle);



/**
 * Prepares the lsn_info data from an LSN to be stored during warmboot.
 *
 * @param lsn_p Pointer to LSN whose information is to be prepared
 * @param lsn_info Pointer to lsn_info structure where data is to be written
 *
 * @return NLMERR_OK on success or an error code otherwise
 */

NlmErrNum_t kaps_lsn_mc_wb_prepare_lsn_info(
    kaps_lsn_mc * lsn_p,
    struct kaps_wb_lsn_info *lsn_info);

#include <kaps_externcend.h>

#endif /* #ifndef INCLUDED_NLMNSLSNMC_H */
