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
#include "sbZfC2MmInternalMemory32BitsLayout.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfC2MmInternalMemory32BitsLayout_Pack(sbZfC2MmInternalMemory32BitsLayout_t *pFrom,
                                        uint8 *pToData,
                                        uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_C2MMINTERNALMEMORY32BITSLAYOUT_SIZE_IN_BYTES;

  if (size % 4) {
    size += (4 - size %4);
  }

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uProtectionBits0 */
  (pToData)[8] |= ((pFrom)->m_uProtectionBits0 & 0x0f) <<4;

  /* Pack Member: m_uData0 */
  (pToData)[4] |= ((pFrom)->m_uData0 & 0x0f) <<4;
  (pToData)[5] |= ((pFrom)->m_uData0 >> 4) &0xFF;
  (pToData)[6] |= ((pFrom)->m_uData0 >> 12) &0xFF;
  (pToData)[7] |= ((pFrom)->m_uData0 >> 20) &0xFF;
  (pToData)[8] |= ((pFrom)->m_uData0 >> 28) & 0x0f;

  /* Pack Member: m_uProtectionBits1 */
  (pToData)[4] |= ((pFrom)->m_uProtectionBits1 & 0x0f);

  /* Pack Member: m_uData1 */
  (pToData)[0] |= ((pFrom)->m_uData1) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_uData1 >> 8) &0xFF;
  (pToData)[2] |= ((pFrom)->m_uData1 >> 16) &0xFF;
  (pToData)[3] |= ((pFrom)->m_uData1 >> 24) &0xFF;

  return SB_ZF_C2MMINTERNALMEMORY32BITSLAYOUT_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfC2MmInternalMemory32BitsLayout_Unpack(sbZfC2MmInternalMemory32BitsLayout_t *pToStruct,
                                          uint8 *pFromData,
                                          uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uProtectionBits0 */
  (pToStruct)->m_uProtectionBits0 =  (uint32)  ((pFromData)[8] >> 4) & 0x0f;

  /* Unpack Member: m_uData0 */
  (pToStruct)->m_uData0 =  (uint32)  ((pFromData)[4] >> 4) & 0x0f;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[5] << 4;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[6] << 12;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[7] << 20;
  (pToStruct)->m_uData0 |=  (uint32)  ((pFromData)[8] & 0x0f) << 28;

  /* Unpack Member: m_uProtectionBits1 */
  (pToStruct)->m_uProtectionBits1 =  (uint32)  ((pFromData)[4] ) & 0x0f;

  /* Unpack Member: m_uData1 */
  (pToStruct)->m_uData1 =  (uint32)  (pFromData)[0] ;
  (pToStruct)->m_uData1 |=  (uint32)  (pFromData)[1] << 8;
  (pToStruct)->m_uData1 |=  (uint32)  (pFromData)[2] << 16;
  (pToStruct)->m_uData1 |=  (uint32)  (pFromData)[3] << 24;

}



/* initialize an instance of this zframe */
void
sbZfC2MmInternalMemory32BitsLayout_InitInstance(sbZfC2MmInternalMemory32BitsLayout_t *pFrame) {

  pFrame->m_uProtectionBits0 =  (unsigned int)  0;
  pFrame->m_uData0 =  (unsigned int)  0;
  pFrame->m_uProtectionBits1 =  (unsigned int)  0;
  pFrame->m_uData1 =  (unsigned int)  0;

}
