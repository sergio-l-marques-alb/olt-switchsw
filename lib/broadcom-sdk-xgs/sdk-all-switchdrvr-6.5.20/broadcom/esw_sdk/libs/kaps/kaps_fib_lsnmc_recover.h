/*******************************************************************************
 *
 * Copyright 2014-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#ifndef __KAPS_LSNMC_RECOVER_H
#define __KAPS_LSNMC_RECOVER_H

#include "kaps_fib_lsnmc.h"
#include "kaps_lpm_algo.h"

#include "kaps_externcstart.h"

NlmErrNum_t kaps_lsn_mc_recover_construct_pfx_bundle(
    kaps_lsn_mc_settings * pSettings,
    uint8_t * commonBits,
    uint32_t commonLength_1,
    uint8_t * suffix,
    int32_t suffixLength_1,
    uint32_t ix,
    kaps_pfx_bundle ** pfxBundle_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_recover_parse_pfx(
    uint8_t * brickData,
    uint32_t brickLength_1,
    uint32_t pfxStartPosInBrick,
    uint32_t gran,
    uint8_t * suffix,
    int32_t * suffixLength_1,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_recover_rebuild_kaps_brick(
    kaps_lsn_mc * lsn_p,
    kaps_lpm_lpu_brick * curBrick,
    kaps_prefix * commonPfx,
    uint32_t curBrickIter,
    uint32_t curLpu,
    uint32_t curRow,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_lsn_mc_recover_rebuild_lsn(
    struct kaps_wb_lsn_info *lsn_info,
    kaps_lsn_mc * lsn_p,
    kaps_prefix * commonPfx,
    NlmReasonCode * o_reason);

#include <kaps_externcend.h>

#endif
