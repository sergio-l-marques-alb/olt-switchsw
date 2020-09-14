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

#ifndef INCLUDED_KAPS_UTILITY_H
#define INCLUDED_KAPS_UTILITY_H

#include <stdint.h>
#include "kaps_externcstart.h"

#define KAPS_INLINE __inline
/**
 * Min function
 */
#define KAPS_MIN(X, Y) (kaps_assert_((X)==(X), "Side effect in macro parm") kaps_assert_((Y)==(Y), "Side effect in macro parm") (X) <= (Y) ? (X) : (Y))

/**
 * Max function
 */
#define KAPS_MAX(X, Y) (kaps_assert_((X)==(X), "Side effect in macro parm") kaps_assert_((Y)==(Y), "Side effect in macro parm") (X) >= (Y) ? (X) : (Y))

#define kaps_NUM_BYTES_PER_80_BITS 10
#define kaps_BITS_IN_REG           80
/*------------------FUNCTION FOR COMMON---------------------*/

void kaps_convert_dm_to_xy(
    uint8_t * data_d,
    uint8_t * data_m,
    uint8_t * data_x,
    uint8_t * data_y,
    uint32_t datalenInBytes);

void kaps_convert_xy_to_dm(
    uint8_t * data_x,
    uint8_t * data_y,
    uint8_t * data_d,
    uint8_t * data_m,
    uint32_t dataLenInBytes);

/**
 * Performs LPT encoding
 *
 * @param x valid pointer to x data
 * @param y valid pointer to y data
 * @param length length of the data array
 *
 * @return none
 */
void kaps_lpt_encode(
    uint8_t * x,
    uint8_t * y,
    uint32_t length);

extern uint32_t KapsReadBitsInArrray(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos);

extern void KapsWriteBitsInArray(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos,
    uint32_t Value);

extern void kaps_FillOnes(
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit);

extern void kaps_FillZeroes(
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit);

extern void kaps_CopyData(
    uint8_t * o_data,
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit);

#include "kaps_externcend.h"
#endif /* INCLUDED_kaps_UTILITY_H */
 /**/
