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

 /*
  * Revision: SDK 1.0a on 23 July,2012 
  */

#ifndef INCLUDED_KAPS_FIBTBLMGR_H
#define INCLUDED_KAPS_FIBTBLMGR_H

#include "kaps_device.h"
#include "kaps_fib_cmn_prefix.h"
#include "kaps_arena.h"
#include "kaps_db.h"
#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_lpm_algo.h"

#include "kaps_externcstart.h"

#define KAPS_FIB_INVALID_INDEX               (0xFFFFFFFF)

typedef struct kaps_fib_resource_usage
{
    uint16_t m_numOfABsUsed;    /* num of AB used */
    uint16_t m_uitUsedInMb;     /* UIT used in Mb */
    uint32_t m_udaUsedInKb;     /* UDA used in Kb */
    uint32_t m_estCapacity;     /* Estimated capacity */
    uint32_t m_numABsReleased;  /* Number of ABs that have been used and then freed up completely */

    uint32_t m_num_ab_no_compression;   /* Number of ABs not using compression */
    uint32_t m_num_ab_compression_1;    /* Number of ABs using compression scheme 1 */
    uint32_t m_num_ab_compression_2;    /* Number of ABs using compression scheme 2 */
} kaps_fib_resource_usage;

struct kaps_fib_tbl;

/* Declaration for Index Change Call back Function */
typedef void (
    *kaps_fib_prefix_index_changed_app_cb_t) (
    void *client_p,
    void *fibTbl_p,             /* ptr to table to which the prefix which is shuffled belongs */
    kaps_pfx_bundle * b,
    uint32_t oldIndex,          /* old index of the Prefix that was shuffled */
    uint32_t newIndex           /* new index of the Prefix that was shuffled */
    );

/* Fib Tbl Mgr Structure */
typedef struct kaps_fib_tbl_mgr
{
    kaps_nlm_allocator *m_alloc_p;      /* Memory allocator */

    struct kaps_device *m_devMgr_p;

    void *m_shadowDevice_p;

    struct kaps_fib_tbl_list *m_fibTblList_p;   /* List of fib Tbls associated with FibTblMgr */

    struct kaps_fib_tbl *m_curFibTbl;

    uint8_t m_numOfDevices;     /* number of devices in cascade */

    uint16_t m_fibTblCount;     /* Number of fib tables associated with FibTblMgr */

    int32_t m_IsConfigLocked;   /* Indicates whether fibtblmgr is locked */

    struct kaps_trie_global *m_trieGlobal;

    struct kaps_trie_global *m_core0trieGlobal;

    struct kaps_trie_global *m_core1trieGlobal;

    kaps_fib_prefix_index_changed_app_cb_t m_indexChangeCallBackFn;     /* User defined call back to inform the change
                                                                         * in the index of the prefix to the
                                                                         * application */

    struct kaps_lpm_mgr *m_lpm_mgr;           /* Client Ptr which may be used by the call back function */

    struct kaps_fib_tbl_mgr_callback_fn_ptrs *m_devWriteCallBackFns;

    uint32_t m_arenaSizeInBytes;

    uint32_t m_maxTblWidth_8;   /* Width of widest table in the FTM system */

    uint32_t m_bufferKapsABWrites;      /* Do not write to the KAPS AB immediately if this is set to 1 */

    kaps_fib_resource_usage m_resourceUsage;    /* Resource Used by FTM */

    uint8_t m_trigDataBuffer[KAPS_HW_LPM_MAX_GRAN / 8];
} kaps_fib_tbl_mgr;

typedef struct kaps_fib_tbl_index_range
{
    uint32_t m_indexLowValue;   /* Represents low value of the tbl index range */
    uint32_t m_indexHighValue;  /* Represents high value of the tbl0 index range */
} kaps_fib_tbl_index_range;     /* If the user does not want to specify the index range, both the values should be 0 */

typedef struct kaps_fib_tbl
{
    kaps_fib_tbl_mgr *m_fibTblMgr_p;    /* kaps_fib_tbl_mgr to which the table belongs to */

    kaps_nlm_allocator *m_alloc_p;      /* Generic allocator pointer */

    uint8_t m_tblId;            /* Table id assigned to the table */

    uint8_t m_coreId;           /* Table id assigned to the table */

    uint16_t m_width;           /* Table width; Represents maximum length of prefix that can be added to the tbl */

    uint32_t m_numPrefixes;     /* Number of prefixes currently present in the table */

    uint32_t m_cascaded_fibtbl;

    kaps_fib_tbl_index_range m_tblIndexRange;   /* Represents the index range for the tbl; Complimentary for user If
                                                 * specified Device will return the hit addresses within this range for 
                                                 * * the tbl */
    kaps_device_issu_read_fn wb_read_fn;
    kaps_device_issu_write_fn wb_write_fn;

    void *m_dependentTbls_p;
    /*
     * Represents list of dependent tbls 
     */

    struct kaps_lpm_trie *m_trie;

    uint16_t m_startBit;
    uint16_t m_endBit;
    struct NlmFibStats m_fibStats;      /* Statistics of the Fib Table */
    struct kaps_db *m_db;

    struct kaps_c_list wb_prefixes;     /* Warmboot prefixes to replayed for this table */
    struct kaps_c_list wb_apt_prefixes; /* APT user prefixes internally replayed during warmboot */
    struct kaps_c_list wb_apt_pfx_copies;       /* APT prefix copies internally replayed during warmboot */
    uint32_t ad_handle_zero_size_wb;

    uint32_t *cr_user_handles;  /* Temp IX to User handle mapping for Crash Recovery */

} kaps_fib_tbl;

typedef struct kaps_fib_tbl_list
{
#include "kaps_dbllinklistdata.h"
    kaps_fib_tbl *m_fibTbl_p;
} kaps_fib_tbl_list;

extern kaps_fib_tbl_mgr *kaps_ftm_kaps_init(
    kaps_nlm_allocator * alloc_p,
    struct kaps_device *devMgr_p,
    void *client_p,
    kaps_fib_prefix_index_changed_app_cb_t indexChangedAppCb,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_destroy(
    kaps_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason);

extern kaps_fib_tbl *kaps_ftm_create_table(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t fibTblId,           /* indicating Fib Tbl Id */
    kaps_fib_tbl_index_range * fibTblIndexRange,
    uint16_t fibMaxPrefixLength,
    struct kaps_db *db,
    uint32_t is_cascaded,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_destroy_table(
    kaps_fib_tbl * fibTbl,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_add_prefix(
    struct kaps_lpm_db *,       /* FibTbl to which prefix is added */
    struct kaps_lpm_entry *entry,       /* represents prefix being added */
    NlmReasonCode * o_reason);
extern NlmErrNum_t kaps_ftm_wb_add_prefix(
    kaps_fib_tbl * fibTbl,      /* FibTbl to which prefix is added */
    struct kaps_lpm_entry *entry,       /* represents prefix being added */
    uint32_t index,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_wb_add_apt_prefix(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry *entry,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_update_ad(
    kaps_fib_tbl * fibTbl,      /* FibTbl to which prefix is added */
    struct kaps_lpm_entry *entry,       /* represents prefix being added */
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_update_ad_address(
    kaps_fib_tbl * fibTbl,      /* FibTbl to which prefix is added */
    struct kaps_lpm_entry *entry,       /* represents prefix being added */
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_delete_prefix(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry *entry,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_get_index(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry *entry,
    int32_t * nindices,
    int32_t ** indices,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_locate_lpm(
    kaps_fib_tbl * fibTbl,
    uint8_t * key,
    struct kaps_lpm_entry **entry_pp,
    uint32_t * index,
    uint32_t * length,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_ftm_get_resource_usage(
    kaps_fib_tbl * fibTbl,
    kaps_fib_resource_usage * rxcUsage,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_ftm_get_prefix_handle(
    kaps_fib_tbl * fibTbl,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_lpm_entry **entry);

NlmErrNum_t kaps_ftm_update_hash_table(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry *entry,
    uint32_t is_del);

kaps_status kaps_ftm_wb_process_rpt(
    kaps_fib_tbl * fib_tbl);
kaps_status kaps_ftm_wb_save(
    kaps_fib_tbl * fib_tbl,
    uint32_t * nv_offset);
kaps_status kaps_ftm_wb_restore(
    kaps_fib_tbl * fib_tbl,
    uint32_t * nv_offset);

kaps_status kaps_ftm_cr_restore(
    kaps_fib_tbl * fib_tbl,
    struct kaps_wb_cb_functions *cb_fun);

NlmErrNum_t kaps_ftm_process_hit_bits(
    kaps_fib_tbl * fibTbl,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_ftm_calc_stats(
    kaps_fib_tbl * fibTbl);

NlmErrNum_t kaps_ftm_get_prefix_location(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry *lpm_entry,
    uint32_t * abs_udc,
    uint32_t * abs_row,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_ftm_get_algo_hit_bit_value(
    kaps_fib_tbl * fibTbl,
    struct kaps_entry *entry,
    uint8_t clear_on_read,
    uint32_t * bit_value,
    NlmReasonCode * o_reason);

#include "kaps_externcend.h"

#endif
