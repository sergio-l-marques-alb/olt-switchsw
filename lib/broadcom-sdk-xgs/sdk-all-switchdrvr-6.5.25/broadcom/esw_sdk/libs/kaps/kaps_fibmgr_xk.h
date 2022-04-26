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

#ifndef INCLUDED_KAPS_XK_FIBTBLMGR_H
#define INCLUDED_KAPS_XK_FIBTBLMGR_H

#include "kaps_ab.h"
#include "kaps_simple_dba.h"

#include "kaps_externcstart.h"

/* kaps_fib_tbl_mgr_write_ab_data_cb_t
startBit - Indicates start bit of the column which will contain the prefix data being added to the AB.
           Other portion of the AB entry will remain unaffected
endBit   - Indicates end bit of the column which  will contain the prefix data being added to the AB
dataLen  - Indicates Length of valid data, other bits in the column will be masked using local mask
*/
typedef NlmErrNum_t(
    *kaps_fib_tbl_mgr_write_ab_data_cb_t) (
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_ab_info * startBlkNum,
    uint32_t logicalLoc,
    uint8_t rptId,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t byte_offset,
    NlmReasonCode * o_reason);

typedef NlmErrNum_t(
    *kaps_fib_tbl_mgr_delete_ab_data_cb_t) (
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_ab_info * startBlkNum,
    uint32_t logicalLoc,
    NlmReasonCode * o_reason);

/* kaps_fib_tbl_mgr_write_uda_data_cb_t
    startBit - Indicates start bit of the LPU/row which will contain the prefix data being added to the MLP. Other portion of
               the MLP will remain unaffected
    endBit   - Indicates end bit of the LPU/row which will contain the prefix data being added to the MLP
    dataLen  - Indicates Length of data, Should be equal to (endBit - startBit +1)
    udaDataType - 0 - MLP, 1 - SIT, 2 - AIT
*/
typedef NlmErrNum_t(
    *kaps_fib_tbl_mgr_write_uda_data_cb_t) (
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint8_t sbNum,
    uint16_t address,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t udaDataType,
    NlmReasonCode * o_reason);

typedef NlmErrNum_t(
    *kaps_fib_tbl_mgr_copy_uda_data_cb_t) (
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t srcDevNum,
    uint8_t srcSBNum,
    uint16_t srcAddress,
    uint8_t numOfSB,            /* Number of SB to move */
    uint8_t dstDevNum,
    uint8_t dstSBNum,
    uint16_t dstAddress,
    NlmReasonCode * o_reason);

typedef NlmErrNum_t(
    *kaps_fib_tbl_mgr_uda_sbc_cb_t) (
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t dev_num,
    uint16_t src_rowNum,
    uint8_t src_LpuNum,
    uint16_t dst_rowNum,
    uint8_t dst_LpuNum,
    uint8_t numLpus_copy,
    uint8_t numLpus_clear,
    int8_t start_lpu,
    int8_t end_lpu,
    NlmReasonCode * o_reason_p);

typedef NlmErrNum_t(
    *kaps_fib_tbl_mgr_lsn_clear_cb_t) (
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t dev_num,
    uint16_t row_num,
    uint8_t lpu_num,
    uint8_t num_lpus,
    int8_t start_lpu,
    int8_t end_lpu,
    NlmReasonCode * o_reason_p);

typedef struct kaps_fib_tbl_mgr_callback_fn_ptrs
{
    kaps_fib_tbl_mgr_write_ab_data_cb_t m_writeABData;
    kaps_fib_tbl_mgr_delete_ab_data_cb_t m_deleteABData;
    kaps_fib_tbl_mgr_uda_sbc_cb_t m_UdaSBC;
    kaps_fib_tbl_mgr_lsn_clear_cb_t m_LsnClear;
} kaps_fib_tbl_mgr_callback_fn_ptrs;

#include "kaps_externcend.h"
#endif /* INCLUDED_NLMXKTBLMGR_H */
