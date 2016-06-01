/*
 * $Id: nlmcmutility.h,v 1.2.8.2 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */


 
#ifndef INCLUDED_NLMCM_UTILITY_H
#define INCLUDED_NLMCM_UTILITY_H

#ifndef NLMPLATFORM_BCM
#include "nlmcmbasic.h"
#include "nlmcmallocator.h"
#include "nlmcmexterncstart.h"
#else
#include <soc/kbp/common/nlmcmbasic.h>
#include <soc/kbp/common/nlmcmallocator.h>
#include <soc/kbp/common/nlmcmexterncstart.h>
#endif

#define NLMCM_NUM_BYTES_PER_80_BITS 10
#define NLMCM_BITS_IN_REG           80
/*------------------FUNCTION FOR COMMON---------------------*/
extern
nlm_u32 ReadBitsInRegs(nlm_u8* RegOf80Bits,
                     nlm_u8 end, 
                     nlm_u8 start
                     );
extern                   
void WriteBitsInRegs(nlm_u8* RegOf80Bits,
                     nlm_u8 end, 
                     nlm_u8 start,
                     nlm_u32 value);
extern
void WriteBitsInRegsAndWrapAround(nlm_u8* RegOf80Bits,
                     nlm_u8 len, 
                     nlm_u8 start,
                     nlm_u32 value,
                     nlm_u8 wrapBoundary);
extern 
void PrintReginNibbles(nlm_u8* RegOf80Bits );

/*----------------------------------------------------------------*/


/*----------------------------------------------------------------*/
extern
nlm_u32 ReadBitsInArrray(
                         nlm_u8* Arr,
                         nlm_u32 ArrSize,
                         nlm_u32 EndPos,
                         nlm_u32 StartPos
                         );

extern
void WriteBitsInArray(
                      nlm_u8* Arr,
                      nlm_u32 ArrSize,
                      nlm_u32 EndPos,
                      nlm_u32 StartPos,
                      nlm_u32 Value
                      );
/*----------------------------------------------------------------*/

extern 
void NlmCm__FillOnes(
                     nlm_u8 *data, 
                     nlm_u32 datalen,
                     nlm_u32 startBit,
                     nlm_u32 endBit
                     );

extern 
void NlmCm__FillZeroes(
                       nlm_u8 *data, 
                       nlm_u32 datalen,
                       nlm_u32 startBit,
                       nlm_u32 endBit
                       );

extern 
void NlmCm__CopyData(    
            nlm_u8 *o_data,
            nlm_u8 *data, 
            nlm_u32 datalen,
            nlm_u32 startBit,
            nlm_u32 endBit
            );

void WriteBitsInByte(
        nlm_u8* targetByte_p,
        nlm_u8 sourceByte,
        nlm_u8  startBit,
        nlm_u8  endBit);


void WriteFirstNBytes(
    nlm_u8* arr,
    nlm_u32 numBytes,
    nlm_u32 Value);


nlm_u64 
LongReadBitsInArray(
            nlm_u8* Arr,
            nlm_u32 ArrSize,
            nlm_u32 EndPos,
            nlm_u32 StartPos
            );

void 
LongWriteBitsInArray(
            nlm_u8* Arr,
            nlm_u32 ArrSize,
            nlm_u32 EndPos,
            nlm_u32 StartPos,
            nlm_u64 Value
            );



void
FlipBitsInArray(
    nlm_u8 *data_p,
    nlm_u32 totalNumBytes,
    nlm_u32 endPos,
    nlm_u32 startPos);

#ifndef NLMPLATFORM_BCM
#include "nlmcmexterncend.h"
#else
#include <soc/kbp/common/nlmcmexterncend.h>
#endif

#endif /* INCLUDED_NLMCM_UTILITY_H */
/**/










