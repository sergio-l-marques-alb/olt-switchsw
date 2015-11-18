/*
 * $Id: sbZfCaQmBufferState1Entry.c,v 1.2 Broadcom SDK $
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


#include "sbTypes.h"
#include "sbZfCaQmBufferState1Entry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfCaQmBufferState1Entry_Pack(sbZfCaQmBufferState1Entry_t *pFrom,
                               uint8 *pToData,
                               uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_CAQMBUFFERSTATE1ENTRY_SIZE_IN_BYTES;

  if (size % 4) {
    size += (4 - size %4);
  }

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uDrop */
  (pToData)[4] |= ((pFrom)->m_uDrop & 0x01);

  /* Pack Member: m_uCrcMode */
  (pToData)[3] |= ((pFrom)->m_uCrcMode & 0x03) <<6;

  /* Pack Member: m_uHeaderLength */
  (pToData)[2] |= ((pFrom)->m_uHeaderLength & 0x03) <<6;
  (pToData)[3] |= ((pFrom)->m_uHeaderLength >> 2) & 0x3f;

  /* Pack Member: m_uContinueByte */
  (pToData)[1] |= ((pFrom)->m_uContinueByte & 0x03) <<6;
  (pToData)[2] |= ((pFrom)->m_uContinueByte >> 2) & 0x3f;

  /* Pack Member: m_uPacketLength */
  (pToData)[0] |= ((pFrom)->m_uPacketLength) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_uPacketLength >> 8) & 0x3f;

  return SB_ZF_CAQMBUFFERSTATE1ENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfCaQmBufferState1Entry_Unpack(sbZfCaQmBufferState1Entry_t *pToStruct,
                                 uint8 *pFromData,
                                 uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uDrop */
  (pToStruct)->m_uDrop =  (uint32)  ((pFromData)[4] ) & 0x01;

  /* Unpack Member: m_uCrcMode */
  (pToStruct)->m_uCrcMode =  (uint32)  ((pFromData)[3] >> 6) & 0x03;

  /* Unpack Member: m_uHeaderLength */
  (pToStruct)->m_uHeaderLength =  (uint32)  ((pFromData)[2] >> 6) & 0x03;
  (pToStruct)->m_uHeaderLength |=  (uint32)  ((pFromData)[3] & 0x3f) << 2;

  /* Unpack Member: m_uContinueByte */
  (pToStruct)->m_uContinueByte =  (uint32)  ((pFromData)[1] >> 6) & 0x03;
  (pToStruct)->m_uContinueByte |=  (uint32)  ((pFromData)[2] & 0x3f) << 2;

  /* Unpack Member: m_uPacketLength */
  (pToStruct)->m_uPacketLength =  (uint32)  (pFromData)[0] ;
  (pToStruct)->m_uPacketLength |=  (uint32)  ((pFromData)[1] & 0x3f) << 8;

}



/* initialize an instance of this zframe */
void
sbZfCaQmBufferState1Entry_InitInstance(sbZfCaQmBufferState1Entry_t *pFrame) {

  pFrame->m_uDrop =  (unsigned int)  0;
  pFrame->m_uCrcMode =  (unsigned int)  0;
  pFrame->m_uHeaderLength =  (unsigned int)  0;
  pFrame->m_uContinueByte =  (unsigned int)  0;
  pFrame->m_uPacketLength =  (unsigned int)  0;

}
