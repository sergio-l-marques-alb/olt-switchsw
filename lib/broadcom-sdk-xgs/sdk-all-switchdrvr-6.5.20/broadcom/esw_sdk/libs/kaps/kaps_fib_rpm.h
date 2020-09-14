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

#ifndef INCLUDED_KAPS_RPM_H
#define INCLUDED_KAPS_RPM_H

#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_bitmap.h"
#include "kaps_fibmgr.h"
#include "kaps_fibmgr_xk.h"
#include "kaps_fib_dba.h"
#include "kaps_fib_hw.h"
#include "kaps_fib_trienode.h"
#include "kaps_algo_hw.h"

#ifdef __cplusplus
extern "C"
{
#endif

    kaps_rpm *kaps_rpm_init(
    kaps_fib_tbl_mgr * fibTblMgr_p,
    uint32_t is_cascaded);

    NlmErrNum_t kaps_rpm_init_db(
    kaps_rpm * rpm_p,
    uint8_t dbId,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_rpm_construct_rit(
    kaps_trie_node * rptNode_p,
    struct kaps_pct *pct);

    NlmErrNum_t kaps_rpm_add_entry(
    kaps_rpm * rpm,
    kaps_pfx_bundle * pfxBundle,
    uint8_t db_id,
    uint8_t is_extra_byte_added,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_rpm_write_entry_to_hw(
    kaps_rpm * rpm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint8_t dbId,
    uint8_t is_extra_byte_added);

    NlmErrNum_t kaps_rpm_remove_entry(
    kaps_rpm * rpm,
    kaps_pfx_bundle * pfxBundle,
    uint8_t db_id);

    NlmErrNum_t kaps_rpm_delete_entry_in_hw(
    kaps_rpm * rpm_p,
    kaps_pfx_bundle * pfxBundle_p);

    NlmErrNum_t kaps_rpm_remove_default_entry(
    kaps_rpm * rpm_p,
    uint8_t dbId,
    kaps_pfx_bundle * pfxBundle_p);

    NlmErrNum_t kaps_rpm_free_db(
    kaps_rpm * rpm_p,
    uint8_t dbId);

    uint32_t kaps_rpm_get_num_free_slots(
    kaps_rpm * rpm_p);

    void kaps_rpm_destroy(
    kaps_rpm * rpm,
    struct kaps_device *device);

    void kaps_rpm_verify(
    kaps_rpm * rpm_p,
    uint8_t dbId);

/**
 * Saves RPT entry to non-volatile memory during warmboot save phase
 *
 * @param rpm_p RPT Manager handle
 * @param pfxBundle_p RPT entry prefix bundle
 * @param length RPT entry length in bits
 * @param pos RPT entry location in the PCM
 * @param dbId This RPT entry belongs to this database id
 *
 * @return NLMERR_OK on success or an error code otherwise
 */
    NlmErrNum_t kaps_rpm_wb_add_entry(
    kaps_rpm * rpm_p,
    kaps_pfx_bundle * pfxBundle_p,
    uint32_t length,
    uint32_t pos,
    uint8_t dbId);

#ifdef __cplusplus
}
#endif

#endif
