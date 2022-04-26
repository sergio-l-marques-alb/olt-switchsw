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

#ifndef INCLUDED_KAPS_FIB_HW_H
#define INCLUDED_KAPS_FIB_HW_H

#include "kaps_fibmgr.h"
#include "kaps_fibmgr_xk.h"
#include "kaps_device_internal.h"
#include "kaps_device_alg.h"
#include "kaps_algo_hw.h"

#include "kaps_externcstart.h"

/* NetRoute defintions  */
/* LSB 160b is having valid data for Rev A/B. In case of Rev B, MSB 160 bit
will have 4b DBID as 0 and remaining 156b as don't cares */
typedef struct NlmDevRPTEntry_s
{
    /*
     * RPT entry is in X/Y format 
     */
    uint8_t m_data[8 * KAPS_RPB_WIDTH_8];
    uint8_t m_mask[8 * KAPS_RPB_WIDTH_8];

} kaps_dev_rpt_entry;

NlmErrNum_t kaps_kaps_iit_write(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_ab_info *start_blk,
    struct kaps_ads *write_ads,
    uint32_t addr,
    NlmReasonCode * o_reason_p);

NlmErrNum_t kaps_kaps_write_ab_data(
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
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_kaps_delete_ab_data(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_ab_info *start_blk,
    uint32_t logicalLoc,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_kaps_read_trigger_hb_in_small_bb(
    kaps_fib_tbl_mgr * fibMgr_p,
    struct kaps_ab_info *ab,
    uint32_t pfx_index,
    uint8_t * hb_value,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_kaps_write_trigger_hb_in_small_bb(
    kaps_fib_tbl_mgr * fibMgr_p,
    struct kaps_ab_info *ab,
    uint32_t pfx_index,
    uint8_t hb_value,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_kaps_flush_buffered_small_bb_writes(
    kaps_fib_tbl_mgr * fibMgr_p,
    struct kaps_ab_info *ab,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_fib_rpt_write(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    kaps_dev_rpt_entry * entry, /* in X/Y format */
    int32_t isDeleteOper,
    uint16_t addr,              /* 0 to 1023 are valid values */
    NlmReasonCode * o_reason_p);

NlmErrNum_t kaps_fib_rit_write(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_pct *entry,
    uint16_t addr,              /* 0 to 1023 are valid values */
    NlmReasonCode * o_reason_p);


void
kaps_write_first_half_of_fmap(
    uint8_t * write_buf,
    uint32_t buf_len_in_bytes,
    uint32_t cur_bit_pos,
    struct kaps_ads *write_ads);


void
kaps_write_second_half_of_fmap(
    uint8_t * write_buf,
    uint32_t buf_len_in_bytes,
    uint32_t cur_bit_pos,
    struct kaps_ads *write_ads);

#include "kaps_externcend.h"

#endif
