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

#ifndef INCLUDED_KAPS_LSNMC_HW_H
#define INCLUDED_KAPS_LSNMC_HW_H

#include "kaps_fib_lsnmc.h"

#include "kaps_externcstart.h"

NlmErrNum_t kaps_lsn_mc_write_entry_to_hw(
    kaps_lsn_mc * self,
    uint16_t lpuGran,
    kaps_pfx_bundle * pfx,
    uint32_t oldix,
    uint32_t newix,
    uint32_t is_ad_update,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_delete_entry_in_hw(
    kaps_lsn_mc * self,
    int ix,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_commit(
    kaps_lsn_mc * self,
    int32_t isSbcPossible,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_clear_lpu_for_grow(
    kaps_lsn_mc * self,
    int32_t isGrowUp,
    uint32_t numLpusToClear,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_update_iit(
    kaps_lsn_mc * self,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_commitIIT(
    kaps_lsn_mc * self,
    uint32_t index,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(
    kaps_lsn_mc * curLsn);

void kaps_lsn_mc_prepare_mlp_data(
    uint8_t * pfxData,
    uint16_t pfxLen,
    uint16_t lopoffLen,
    uint16_t gran,
    uint8_t * o_data);

NlmErrNum_t kaps_kaps_format_map_get_gran(
    kaps_lsn_mc_settings * settings,
    struct kaps_ads *kaps_it,
    uint32_t cur_lpu_nr,
    uint32_t * brick_gran_ix);

uint8_t *kaps_kaps_get_bkt_data(
    kaps_lsn_mc * self,
    struct kaps_device *device,
    uint32_t bkt_nr,
    uint32_t row_nr);

uint32_t kaps_kaps_get_old_pfx_hb_index(
    kaps_lsn_mc * curLsn,
    uint32_t oldPfxIndex);

kaps_status kaps_fib_update_it_callback(
    struct uda_mem_chunk *uda_chunk);

kaps_status kaps_fib_uda_sub_block_copy_callback(
    struct uda_mem_chunk *uda_chunk,
    int32_t to_region_id,
    int32_t to_offset);

void kaps_fib_kaps_update_lsn_size(
    struct kaps_db *db);


uint32_t
kaps_get_daisy_chain_id(
    struct kaps_device *device,
    struct kaps_lsn_mc *self,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum);


#include <kaps_externcend.h>

#endif
