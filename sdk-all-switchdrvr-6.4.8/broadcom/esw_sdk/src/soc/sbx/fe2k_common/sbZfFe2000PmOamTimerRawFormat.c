/*
 * $Id$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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


#include "sbTypesGlue.h"
#include "sbZfFe2000PmOamTimerRawFormat.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfFe2000PmOamTimerRawFormat_Pack(sbZfFe2000PmOamTimerRawFormat_t *pFrom,
                                   uint8 *pToData,
                                   uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_FE_2000_PM_OAMTIMERRAWFORMAT_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: uProfileId */
  (pToData)[6] |= ((pFrom)->uProfileId & 0x07) <<5;
  (pToData)[7] |= ((pFrom)->uProfileId >> 3) &0xFF;

  /* Pack Member: uStarted */
  (pToData)[6] |= ((pFrom)->uStarted & 0x01) <<4;

  /* Pack Member: uTime */
  (pToData)[4] |= ((pFrom)->uTime) & 0xFF;
  (pToData)[5] |= ((pFrom)->uTime >> 8) &0xFF;
  (pToData)[6] |= ((pFrom)->uTime >> 16) & 0x0f;

  /* Pack Member: uRsvd */
  (pToData)[0] |= ((pFrom)->uRsvd) & 0xFF;
  (pToData)[1] |= ((pFrom)->uRsvd >> 8) &0xFF;
  (pToData)[2] |= ((pFrom)->uRsvd >> 16) &0xFF;
  (pToData)[3] |= ((pFrom)->uRsvd >> 24) &0xFF;

  return SB_ZF_FE_2000_PM_OAMTIMERRAWFORMAT_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfFe2000PmOamTimerRawFormat_Unpack(sbZfFe2000PmOamTimerRawFormat_t *pToStruct,
                                     uint8 *pFromData,
                                     uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: uProfileId */
  (pToStruct)->uProfileId =  (uint32)  ((pFromData)[6] >> 5) & 0x07;
  (pToStruct)->uProfileId |=  (uint32)  (pFromData)[7] << 3;

  /* Unpack Member: uStarted */
  (pToStruct)->uStarted =  (uint32)  ((pFromData)[6] >> 4) & 0x01;

  /* Unpack Member: uTime */
  (pToStruct)->uTime =  (uint32)  (pFromData)[4] ;
  (pToStruct)->uTime |=  (uint32)  (pFromData)[5] << 8;
  (pToStruct)->uTime |=  (uint32)  ((pFromData)[6] & 0x0f) << 16;

  /* Unpack Member: uRsvd */
  (pToStruct)->uRsvd =  (uint32)  (pFromData)[0] ;
  (pToStruct)->uRsvd |=  (uint32)  (pFromData)[1] << 8;
  (pToStruct)->uRsvd |=  (uint32)  (pFromData)[2] << 16;
  (pToStruct)->uRsvd |=  (uint32)  (pFromData)[3] << 24;

}



/* initialize an instance of this zframe */
void
sbZfFe2000PmOamTimerRawFormat_InitInstance(sbZfFe2000PmOamTimerRawFormat_t *pFrame) {

  pFrame->uProfileId =  (unsigned int)  0;
  pFrame->uStarted =  (unsigned int)  0;
  pFrame->uTime =  (unsigned int)  0;
  pFrame->uRsvd =  (unsigned int)  0;

}
