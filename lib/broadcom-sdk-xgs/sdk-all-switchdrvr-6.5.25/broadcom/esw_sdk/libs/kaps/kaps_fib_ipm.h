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

#ifndef INCLUDED_KAPS_IPM_H
#define INCLUDED_KAPS_IPM_H

#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_bitmap.h"
#include "kaps_fibmgr.h"
#include "kaps_fibmgr_xk.h"
#include "kaps_fib_dba.h"

#ifdef __cplusplus
extern "C"
{
#endif

    kaps_ipm *kaps_ipm_init(
    kaps_fib_tbl_mgr * fibTblMgr_p,
    uint32_t poolIndex,
    uint32_t pool_size,
    struct kaps_ab_info *startBlk,
    uint32_t blkWidthInBits,
    enum kaps_dba_trigger_compression_mode mode,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_ipm_add_entry(
    kaps_ipm * ipm,
    kaps_pfx_bundle * pfxBundle,
    uint32_t rptId,
    uint32_t length,
    uint32_t numRptBitsLoppedOff,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_ipm_write_entry_to_hw(
    kaps_ipm * ipm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t numRptBitsLoppedOff,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_ipm_remove_entry(
    kaps_ipm * ipm,
    uint32_t pfxIndex,
    uint32_t rptId,
    uint32_t length,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_ipm_delete_entry_in_hw(
    kaps_ipm * ipm_p,
    uint32_t pfxIndex,
    uint32_t rptId,
    uint32_t length,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_ipm_fix_entry(
    kaps_ipm * ipm_p,
    uint32_t ab_num,
    uint32_t row_nr,
    NlmReasonCode * reason);

    NlmErrNum_t kaps_ipm_create_entry_data(
    kaps_ipm * ipm_p,
    uint32_t ab_num,
    uint32_t row_nr,
    uint8_t * data,
    uint8_t * mask,
    uint8_t is_xy,
    NlmReasonCode * reason);

    uint32_t kaps_ipm_recompute_len_in_simple_dba(
    kaps_ipm * ipm,
    uint32_t length);

    NlmErrNum_t kaps_ipm_init_rpt(
    kaps_ipm * ipm_p,
    uint32_t rptId,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_ipm_free_rpt(
    kaps_ipm * ipm_p,
    uint32_t rptId);

    void kaps_ipm_destroy(
    kaps_nlm_allocator * alloc_p,
    kaps_ipm * ipm);

    uint32_t kaps_ipm_verify(
    kaps_ipm * ipm_p,
    uint32_t rptId);

/**
 * Adds IPT entry to a pool during warmboot restore phase. Pool is identified by ipm_p, rptId
 * identifies to which RPT entry this entry belongs to. This function adds the IPT entry at a
 * location within the pool
 *
 * @param ipm_p IPT Manager handle
 * @param pfxBundle_p IPT prefix bundle
 * @param rptId RPT id of RPT entry this IPT entry belongs to
 * @param length IPT length in bits
 * @param pos Location within the pool this entry should be added
 *
 * @return NLMERR_OK on success or an error code otherwise
 */
    NlmErrNum_t kaps_ipm_wb_add_entry(
    kaps_ipm * ipm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t pos);

/**
 * Stores IPT entries of a specific RPT entry to non-volatile memory. RPT entry is identified by
 * the rpt id which is one of the input parameters. An IPT pool contains entries that belong
 * to different RPT entries. Only entries that belong to the RPT entry identified by rpt id are
 * stored.
 *
 * @param ipm_p IPT Manager handle
 * @param rpt_id RPT id of an RPT entry whose IPT entries should be stored
 * @param nv_offset Starting offset in the non-volatile memory to write data to
 * @param write_fn Callback write function to write data to non-volatile memory
 * @param handle User handle passed back through read/write callback functions
 *
 * @return number of entries stored on success or -1 on failure
 *
 */
    int32_t kaps_ipm_wb_store_ipt_entries(
    kaps_ipm * ipm_p,
    uint32_t rpt_id,
    uint32_t * nv_offset,
    kaps_device_issu_write_fn write_fn,
    void *handle);

#ifdef __cplusplus
}
#endif

#endif
