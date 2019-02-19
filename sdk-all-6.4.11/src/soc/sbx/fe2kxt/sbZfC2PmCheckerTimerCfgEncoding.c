/*
 * $Id$
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


#include "sbTypes.h"
#include "sbZfC2PmCheckerTimerCfgEncoding.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfC2PmCheckerTimerCfgEncoding_Pack(sbZfC2PmCheckerTimerCfgEncoding_t *pFrom,
                                     uint8 *pToData,
                                     uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_C2PMCHECKERTIMERCFGENCODING_SIZE_IN_BYTES;

  if (size % 4) {
    size += (4 - size %4);
  }

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uType */
  (pToData)[9] |= ((pFrom)->m_uType & 0x07);

  /* Pack Member: m_uInterrupt */
  (pToData)[2] |= ((pFrom)->m_uInterrupt & 0x01) <<7;

  /* Pack Member: m_uReset */
  (pToData)[2] |= ((pFrom)->m_uReset & 0x01) <<6;

  /* Pack Member: m_uStrict */
  (pToData)[2] |= ((pFrom)->m_uStrict & 0x01) <<5;

  /* Pack Member: m_uMode32 */
  (pToData)[2] |= ((pFrom)->m_uMode32 & 0x01) <<4;

  /* Pack Member: m_uDeadline */
  (pToData)[0] |= ((pFrom)->m_uDeadline) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_uDeadline >> 8) &0xFF;
  (pToData)[2] |= ((pFrom)->m_uDeadline >> 16) & 0x0f;

  return SB_ZF_C2PMCHECKERTIMERCFGENCODING_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfC2PmCheckerTimerCfgEncoding_Unpack(sbZfC2PmCheckerTimerCfgEncoding_t *pToStruct,
                                       uint8 *pFromData,
                                       uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uType */
  (pToStruct)->m_uType =  (uint32)  ((pFromData)[9] ) & 0x07;

  /* Unpack Member: m_uInterrupt */
  (pToStruct)->m_uInterrupt =  (uint32)  ((pFromData)[2] >> 7) & 0x01;

  /* Unpack Member: m_uReset */
  (pToStruct)->m_uReset =  (uint32)  ((pFromData)[2] >> 6) & 0x01;

  /* Unpack Member: m_uStrict */
  (pToStruct)->m_uStrict =  (uint32)  ((pFromData)[2] >> 5) & 0x01;

  /* Unpack Member: m_uMode32 */
  (pToStruct)->m_uMode32 =  (uint32)  ((pFromData)[2] >> 4) & 0x01;

  /* Unpack Member: m_uDeadline */
  (pToStruct)->m_uDeadline =  (uint32)  (pFromData)[0] ;
  (pToStruct)->m_uDeadline |=  (uint32)  (pFromData)[1] << 8;
  (pToStruct)->m_uDeadline |=  (uint32)  ((pFromData)[2] & 0x0f) << 16;

}



/* initialize an instance of this zframe */
void
sbZfC2PmCheckerTimerCfgEncoding_InitInstance(sbZfC2PmCheckerTimerCfgEncoding_t *pFrame) {

  pFrame->m_uType =  (unsigned int)  0;
  pFrame->m_uInterrupt =  (unsigned int)  0;
  pFrame->m_uReset =  (unsigned int)  0;
  pFrame->m_uStrict =  (unsigned int)  0;
  pFrame->m_uMode32 =  (unsigned int)  0;
  pFrame->m_uDeadline =  (unsigned int)  0;

}
