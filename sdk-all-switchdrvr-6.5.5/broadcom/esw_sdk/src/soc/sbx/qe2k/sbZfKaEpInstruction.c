/*
 * $Id: sbZfKaEpInstruction.c,v 1.2 Broadcom SDK $
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
#include "sbZfKaEpInstruction.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfKaEpInstruction_Pack(sbZfKaEpInstruction_t *pFrom,
                         uint8 *pToData,
                         uint32 nMaxToDataIndex) {
#ifdef SAND_BIG_ENDIAN_HOST
  int i;
  int size = SB_ZF_ZFKAEPINSTRUCTION_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on bigword endian */

  /* Pack Member: m_nValid */
  (pToData)[0] |= ((pFrom)->m_nValid & 0x01) <<7;

  /* Pack Member: m_nOpcode */
  (pToData)[0] |= ((pFrom)->m_nOpcode & 0x3f) <<1;

  /* Pack Member: m_nOpAVariable */
  (pToData)[0] |= ((pFrom)->m_nOpAVariable & 0x01);

  /* Pack Member: m_nOperandA */
  (pToData)[1] |= ((pFrom)->m_nOperandA & 0x3f) <<2;

  /* Pack Member: m_nOpBVariable */
  (pToData)[1] |= ((pFrom)->m_nOpBVariable & 0x01) <<1;

  /* Pack Member: m_nOperandB */
  (pToData)[2] |= ((pFrom)->m_nOperandB & 0x1f) <<3;
  (pToData)[1] |= ((pFrom)->m_nOperandB >> 5) & 0x01;

  /* Pack Member: m_nOpCVariable */
  (pToData)[2] |= ((pFrom)->m_nOpCVariable & 0x01) <<2;

  /* Pack Member: m_nOperandC */
  (pToData)[3] |= ((pFrom)->m_nOperandC) & 0xFF;
  (pToData)[2] |= ((pFrom)->m_nOperandC >> 8) & 0x03;
#else
  int i;
  int size = SB_ZF_ZFKAEPINSTRUCTION_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_nValid */
  (pToData)[3] |= ((pFrom)->m_nValid & 0x01) <<7;

  /* Pack Member: m_nOpcode */
  (pToData)[3] |= ((pFrom)->m_nOpcode & 0x3f) <<1;

  /* Pack Member: m_nOpAVariable */
  (pToData)[3] |= ((pFrom)->m_nOpAVariable & 0x01);

  /* Pack Member: m_nOperandA */
  (pToData)[2] |= ((pFrom)->m_nOperandA & 0x3f) <<2;

  /* Pack Member: m_nOpBVariable */
  (pToData)[2] |= ((pFrom)->m_nOpBVariable & 0x01) <<1;

  /* Pack Member: m_nOperandB */
  (pToData)[1] |= ((pFrom)->m_nOperandB & 0x1f) <<3;
  (pToData)[2] |= ((pFrom)->m_nOperandB >> 5) & 0x01;

  /* Pack Member: m_nOpCVariable */
  (pToData)[1] |= ((pFrom)->m_nOpCVariable & 0x01) <<2;

  /* Pack Member: m_nOperandC */
  (pToData)[0] |= ((pFrom)->m_nOperandC) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_nOperandC >> 8) & 0x03;
#endif

  return SB_ZF_ZFKAEPINSTRUCTION_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfKaEpInstruction_Unpack(sbZfKaEpInstruction_t *pToStruct,
                           uint8 *pFromData,
                           uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;

#ifdef SAND_BIG_ENDIAN_HOST

  /* Unpack operation based on bigword endian */

  /* Unpack Member: m_nValid */
  (pToStruct)->m_nValid =  (int32)  ((pFromData)[0] >> 7) & 0x01;

  /* Unpack Member: m_nOpcode */
  (pToStruct)->m_nOpcode =  (int32)  ((pFromData)[0] >> 1) & 0x3f;

  /* Unpack Member: m_nOpAVariable */
  (pToStruct)->m_nOpAVariable =  (int32)  ((pFromData)[0] ) & 0x01;

  /* Unpack Member: m_nOperandA */
  (pToStruct)->m_nOperandA =  (int32)  ((pFromData)[1] >> 2) & 0x3f;

  /* Unpack Member: m_nOpBVariable */
  (pToStruct)->m_nOpBVariable =  (int32)  ((pFromData)[1] >> 1) & 0x01;

  /* Unpack Member: m_nOperandB */
  (pToStruct)->m_nOperandB =  (int32)  ((pFromData)[2] >> 3) & 0x1f;
  (pToStruct)->m_nOperandB |=  (int32)  ((pFromData)[1] & 0x01) << 5;

  /* Unpack Member: m_nOpCVariable */
  (pToStruct)->m_nOpCVariable =  (int32)  ((pFromData)[2] >> 2) & 0x01;

  /* Unpack Member: m_nOperandC */
  (pToStruct)->m_nOperandC =  (int32)  (pFromData)[3] ;
  (pToStruct)->m_nOperandC |=  (int32)  ((pFromData)[2] & 0x03) << 8;
#else

  /* Unpack operation based on little endian */

  /* Unpack Member: m_nValid */
  (pToStruct)->m_nValid =  (int32)  ((pFromData)[3] >> 7) & 0x01;

  /* Unpack Member: m_nOpcode */
  (pToStruct)->m_nOpcode =  (int32)  ((pFromData)[3] >> 1) & 0x3f;

  /* Unpack Member: m_nOpAVariable */
  (pToStruct)->m_nOpAVariable =  (int32)  ((pFromData)[3] ) & 0x01;

  /* Unpack Member: m_nOperandA */
  (pToStruct)->m_nOperandA =  (int32)  ((pFromData)[2] >> 2) & 0x3f;

  /* Unpack Member: m_nOpBVariable */
  (pToStruct)->m_nOpBVariable =  (int32)  ((pFromData)[2] >> 1) & 0x01;

  /* Unpack Member: m_nOperandB */
  (pToStruct)->m_nOperandB =  (int32)  ((pFromData)[1] >> 3) & 0x1f;
  (pToStruct)->m_nOperandB |=  (int32)  ((pFromData)[2] & 0x01) << 5;

  /* Unpack Member: m_nOpCVariable */
  (pToStruct)->m_nOpCVariable =  (int32)  ((pFromData)[1] >> 2) & 0x01;

  /* Unpack Member: m_nOperandC */
  (pToStruct)->m_nOperandC =  (int32)  (pFromData)[0] ;
  (pToStruct)->m_nOperandC |=  (int32)  ((pFromData)[1] & 0x03) << 8;
#endif

}



/* initialize an instance of this zframe */
void
sbZfKaEpInstruction_InitInstance(sbZfKaEpInstruction_t *pFrame) {

  pFrame->m_nValid =  (unsigned int)  0;
  pFrame->m_nOpcode =  (unsigned int)  0;
  pFrame->m_nOpAVariable =  (unsigned int)  0;
  pFrame->m_nOperandA =  (unsigned int)  0;
  pFrame->m_nOpBVariable =  (unsigned int)  0;
  pFrame->m_nOperandB =  (unsigned int)  0;
  pFrame->m_nOpCVariable =  (unsigned int)  0;
  pFrame->m_nOperandC =  (unsigned int)  0;

}
