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

/* This file contains definition of the following objects:
 *  kaps_lpm_trie:  Object to store the trie.
 */

#ifndef INCLUDED_KAPS_TRIE_H
#define INCLUDED_KAPS_TRIE_H

#include <kaps_fib_cmn_pfxbundle.h>
#include <kaps_portable.h>
#include <kaps_fib_triedata.h>
#include <kaps_fib_trienode.h>
#include <kaps_fib_lsnmc.h>
#include "kaps_fibmgr.h"
#include "kaps_fibmgr_xk.h"
#include "kaps_db.h"

#include <kaps_externcstart.h>

extern kaps_trie_global *kaps_trie_three_level_fib_module_init(
    kaps_nlm_allocator * alloc,
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t cascaded,
    NlmReasonCode * o_reason);

extern kaps_trie_global *kaps_trie_two_level_fib_module_init(
    kaps_nlm_allocator * alloc,
    kaps_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason);

extern void kaps_trie_module_destroy(
    void *trieGlobal);

extern kaps_lpm_trie *kaps_trie_create(
    void *trieGlobal,
    void *fibTbl,
    NlmReasonCode * o_reason);

extern void kaps_trie_destroy(
    kaps_lpm_trie * self);

extern NlmErrNum_t kaps_trie_submit_rqt(
    kaps_lpm_trie * self,
    NlmTblRqtCode rqtCode,
    struct kaps_lpm_entry *entry,
    NlmReasonCode * o_reason);

extern kaps_pfx_bundle *kaps_trie_locate_lpm(
    kaps_lpm_trie * self,
    uint8_t * pfxData,
    uint32_t pfxLen,
    uint32_t * o_lpmHitIndex_p,
    NlmReasonCode * o_reason);

extern uint32_t kaps_trie_locate_exact(
    kaps_lpm_trie * self,
    uint8_t * pfxData_p,
    uint16_t inUse,
    kaps_pfx_bundle ** o_pfxBundle_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_trie_get_prefix_location(
    kaps_lpm_trie * trie_p,
    struct kaps_lpm_entry *lpm_entry,
    uint32_t * abs_udc,
    uint32_t * abs_row);

void kaps_trie_get_resource_usage(
    kaps_fib_tbl * fibTbl,
    kaps_fib_resource_usage * rxcUsed_p,
    uint32_t log_info);

NlmErrNum_t kaps_trie_process_hit_bits_iit_lmpsofar(
    kaps_lpm_trie * trie_p);

/**
 * Saves trie information during warmboot/ISSU save phase. This function internally
 * saves information for RPT/RIT entries, IPT/IIT entries, APT/AIT entries and the
 * prefix remainders from the LSNs. Other auxilary information like APT index chunks,
 * RLO bits of each trienode are also saved
 *
 * @param trie_p Pointer to the trie
 * @param nv_offset The starting offset in the non-volatile memory to write to
 *
 * @return KAPS_OK on success or an error code otherwise
 */
kaps_status kaps_trie_wb_save(
    kaps_lpm_trie * trie_p,
    uint32_t * nv_offset);

/**
 * Restores trie information during warmboot/ISSU restore phase. This function internally
 * restores information for RPT/RIT entries, IPT/IIT entries, APT/AIT entries and the
 * prefixes in the LSNs. Other auxilary information like APT index chunks,
 * RLO bits of each trienode are also restored
 *
 * @param trie_p Pointer to the trie
 * @param nv_offset Starting offset in the non-volatile memory to read from
 *
 * @return KAPS_OK on success or an error code otherwise
 */
kaps_status kaps_trie_wb_restore(
    kaps_lpm_trie * trie_p,
    uint32_t * nv_offset);

/**
 * Saves pools. Pool Manager is common across all databases. This function is called
 * only once by the upper layer (i.e. Not called per each database). This function
 * internally calls PoolMgr APIs to save active IPT/APT pools
 *
 * @param trie_g Pointer to trie global instance
 * @param wb_fun WB state
 *
 * @return NLMERR_OK on success or an error code on otherwise
 */
NlmErrNum_t kaps_kaps_trie_wb_save_pools(
    kaps_trie_global * trie_g,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Restores pools. Pool Manager is common across all databases. This function is called
 * only once by the upper layer (i.e. Not called per each database). This function
 * internally calls PoolMgr APIs to restore active IPT/APT pools
 *
 * @param trie_g Pointer to trie global instance
 * @param wb_fun WB state
 *
 * @return NLMERR_OK on success or an error code on otherwise
 */
NlmErrNum_t kaps_kaps_trie_wb_restore_pools(
    kaps_trie_global * trie_g,
    struct kaps_wb_cb_functions *wb_fun);





/**
 * This is a utility function to traverse and prints trie details into a text file.
 * This function along with kaps_kaps_trie_wb_verify_trie can be used together to verify the
 * warmboot save/restore phase. One can call this function before warmboot save
 * phase to store the trie details into a file
 *
 * @param trie Pointer to the trie
 * @param fp File pointer to output text file
 *
 * @return KAPS_OK on success or an error code otherwise
 */
kaps_status kaps_trie_wb_traverse_and_print_trie(
    kaps_lpm_trie * trie,
    FILE * fp);

/**
 * This function along with trie_wb_traverse_and_print can be used together to verify
 * the warmboot save/restore phase. One can call this function after warmboot restore
 * phase. Text file created from traverse_and_print_trie is passed as input to this
 * function along with pointer to trie as another parameter. This function reads out
 * trie details from the input file, compares them against the trie
 *
 * @param trie Pointer to the trie created after warmboot restore
 * @param fp File pointer to input text file created before warmboot store
 *
 * @return KAPS_OK on success or an error code otherwise
 */
kaps_status kaps_kaps_trie_wb_verify_trie(
    kaps_lpm_trie * trie,
    FILE * fp);

void kaps_trie_calc_trie_lsn_stats(
    kaps_trie_node * node_p,
    uint32_t find_terminating_search_path, 
    struct NlmFibStats *stats_p);

void kaps_trie_calc_kaps_small_bb_stats(
    kaps_fib_tbl * fibTbl,
    struct kaps_small_bb_stats *stats,
    uint32_t doVerify);

NlmErrNum_t kaps_trie_configure_uda_mgr(
    kaps_trie_global * global,
    struct kaps_db *db,
    struct kaps_uda_mgr *uda_mem_mgr,
    NlmReasonCode * o_reason);




/**
 * Restores trie information during Crash Recovery restore phase. This function internally
 * restores information for RPT/RIT entries, IPT/IIT entries, APT/AIT entries and the
 * prefixes in the LSNs. Other auxilary information like APT index chunks,
 * RLO bits of each trienode are also restored
 *
 * @param trie_p Pointer to the trie
 * @param nv_offset Starting offset in the non-volatile memory to read from
 *
 * @return KAPS_OK on success or an error code otherwise
 */
kaps_status kaps_trie_cr_restore(
    kaps_lpm_trie * trie_p,
    uint32_t * nv_offset);

/**
 * Restores pools. Pool Manager is common across all databases. This function is called
 * only once by the upper layer (i.e. Not called per each database). This function
 * internally calls PoolMgr APIs to restore active IPT/APT pools
 *
 * @param trie Pointer to trie
 * @param wb_fun WB state
 *
 * @return NLMERR_OK on success or an error code on otherwise
 */
NlmErrNum_t kaps_kaps_trie_cr_restore_pools(
    kaps_lpm_trie * trie,
    struct kaps_wb_cb_functions *wb_fun);

void kaps_trie_cr_save_default_rpt_entry(
    kaps_trie_global * trie_g,
    kaps_lpm_trie * trie,
    struct kaps_wb_cb_functions *wb_fun);

NlmErrNum_t kaps_trie_cr_restore_default_rpt_entry(
    kaps_trie_global * trie_g,
    kaps_lpm_trie * trie,
    struct kaps_wb_cb_functions *wb_fun);

void kaps_kaps_trie_wb_restore_iit_lmpsofar(
    kaps_trie_node * node_p,
    kaps_trie_node * ancestorIptNode_p);

void kaps_trie_cr_save_pools(
    kaps_trie_global * trie_g,
    struct kaps_wb_cb_functions *wb_fun);

#include <kaps_externcend.h>

#endif /* INCLUDED_NLMNSTRIE_H */
