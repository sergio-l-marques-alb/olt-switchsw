/*
 * $Id$
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
#include "sbZfC2PpAggregateHashConfigEntry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfC2PpAggregateHashConfigEntry_Pack(sbZfC2PpAggregateHashConfigEntry_t *pFrom,
                                      uint8 *pToData,
                                      uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_C2PPAGGREGATEHASHCONFIGENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uData9 */
  (pToData)[36] |= ((pFrom)->m_uData9) & 0xFF;
  (pToData)[37] |= ((pFrom)->m_uData9 >> 8) &0xFF;
  (pToData)[38] |= ((pFrom)->m_uData9 >> 16) &0xFF;
  (pToData)[39] |= ((pFrom)->m_uData9 >> 24) &0xFF;

  /* Pack Member: m_uData8 */
  (pToData)[32] |= ((pFrom)->m_uData8) & 0xFF;
  (pToData)[33] |= ((pFrom)->m_uData8 >> 8) &0xFF;
  (pToData)[34] |= ((pFrom)->m_uData8 >> 16) &0xFF;
  (pToData)[35] |= ((pFrom)->m_uData8 >> 24) &0xFF;

  /* Pack Member: m_uData7 */
  (pToData)[28] |= ((pFrom)->m_uData7) & 0xFF;
  (pToData)[29] |= ((pFrom)->m_uData7 >> 8) &0xFF;
  (pToData)[30] |= ((pFrom)->m_uData7 >> 16) &0xFF;
  (pToData)[31] |= ((pFrom)->m_uData7 >> 24) &0xFF;

  /* Pack Member: m_uData6 */
  (pToData)[24] |= ((pFrom)->m_uData6) & 0xFF;
  (pToData)[25] |= ((pFrom)->m_uData6 >> 8) &0xFF;
  (pToData)[26] |= ((pFrom)->m_uData6 >> 16) &0xFF;
  (pToData)[27] |= ((pFrom)->m_uData6 >> 24) &0xFF;

  /* Pack Member: m_uData5 */
  (pToData)[20] |= ((pFrom)->m_uData5) & 0xFF;
  (pToData)[21] |= ((pFrom)->m_uData5 >> 8) &0xFF;
  (pToData)[22] |= ((pFrom)->m_uData5 >> 16) &0xFF;
  (pToData)[23] |= ((pFrom)->m_uData5 >> 24) &0xFF;

  /* Pack Member: m_uData4 */
  (pToData)[16] |= ((pFrom)->m_uData4) & 0xFF;
  (pToData)[17] |= ((pFrom)->m_uData4 >> 8) &0xFF;
  (pToData)[18] |= ((pFrom)->m_uData4 >> 16) &0xFF;
  (pToData)[19] |= ((pFrom)->m_uData4 >> 24) &0xFF;

  /* Pack Member: m_uData3 */
  (pToData)[12] |= ((pFrom)->m_uData3) & 0xFF;
  (pToData)[13] |= ((pFrom)->m_uData3 >> 8) &0xFF;
  (pToData)[14] |= ((pFrom)->m_uData3 >> 16) &0xFF;
  (pToData)[15] |= ((pFrom)->m_uData3 >> 24) &0xFF;

  /* Pack Member: m_uData2 */
  (pToData)[8] |= ((pFrom)->m_uData2) & 0xFF;
  (pToData)[9] |= ((pFrom)->m_uData2 >> 8) &0xFF;
  (pToData)[10] |= ((pFrom)->m_uData2 >> 16) &0xFF;
  (pToData)[11] |= ((pFrom)->m_uData2 >> 24) &0xFF;

  /* Pack Member: m_uData1 */
  (pToData)[4] |= ((pFrom)->m_uData1) & 0xFF;
  (pToData)[5] |= ((pFrom)->m_uData1 >> 8) &0xFF;
  (pToData)[6] |= ((pFrom)->m_uData1 >> 16) &0xFF;
  (pToData)[7] |= ((pFrom)->m_uData1 >> 24) &0xFF;

  /* Pack Member: m_uData0 */
  (pToData)[0] |= ((pFrom)->m_uData0) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_uData0 >> 8) &0xFF;
  (pToData)[2] |= ((pFrom)->m_uData0 >> 16) &0xFF;
  (pToData)[3] |= ((pFrom)->m_uData0 >> 24) &0xFF;

  return SB_ZF_C2PPAGGREGATEHASHCONFIGENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfC2PpAggregateHashConfigEntry_Unpack(sbZfC2PpAggregateHashConfigEntry_t *pToStruct,
                                        uint8 *pFromData,
                                        uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uData9 */
  (pToStruct)->m_uData9 =  (uint32)  (pFromData)[36] ;
  (pToStruct)->m_uData9 |=  (uint32)  (pFromData)[37] << 8;
  (pToStruct)->m_uData9 |=  (uint32)  (pFromData)[38] << 16;
  (pToStruct)->m_uData9 |=  (uint32)  (pFromData)[39] << 24;

  /* Unpack Member: m_uData8 */
  (pToStruct)->m_uData8 =  (uint32)  (pFromData)[32] ;
  (pToStruct)->m_uData8 |=  (uint32)  (pFromData)[33] << 8;
  (pToStruct)->m_uData8 |=  (uint32)  (pFromData)[34] << 16;
  (pToStruct)->m_uData8 |=  (uint32)  (pFromData)[35] << 24;

  /* Unpack Member: m_uData7 */
  (pToStruct)->m_uData7 =  (uint32)  (pFromData)[28] ;
  (pToStruct)->m_uData7 |=  (uint32)  (pFromData)[29] << 8;
  (pToStruct)->m_uData7 |=  (uint32)  (pFromData)[30] << 16;
  (pToStruct)->m_uData7 |=  (uint32)  (pFromData)[31] << 24;

  /* Unpack Member: m_uData6 */
  (pToStruct)->m_uData6 =  (uint32)  (pFromData)[24] ;
  (pToStruct)->m_uData6 |=  (uint32)  (pFromData)[25] << 8;
  (pToStruct)->m_uData6 |=  (uint32)  (pFromData)[26] << 16;
  (pToStruct)->m_uData6 |=  (uint32)  (pFromData)[27] << 24;

  /* Unpack Member: m_uData5 */
  (pToStruct)->m_uData5 =  (uint32)  (pFromData)[20] ;
  (pToStruct)->m_uData5 |=  (uint32)  (pFromData)[21] << 8;
  (pToStruct)->m_uData5 |=  (uint32)  (pFromData)[22] << 16;
  (pToStruct)->m_uData5 |=  (uint32)  (pFromData)[23] << 24;

  /* Unpack Member: m_uData4 */
  (pToStruct)->m_uData4 =  (uint32)  (pFromData)[16] ;
  (pToStruct)->m_uData4 |=  (uint32)  (pFromData)[17] << 8;
  (pToStruct)->m_uData4 |=  (uint32)  (pFromData)[18] << 16;
  (pToStruct)->m_uData4 |=  (uint32)  (pFromData)[19] << 24;

  /* Unpack Member: m_uData3 */
  (pToStruct)->m_uData3 =  (uint32)  (pFromData)[12] ;
  (pToStruct)->m_uData3 |=  (uint32)  (pFromData)[13] << 8;
  (pToStruct)->m_uData3 |=  (uint32)  (pFromData)[14] << 16;
  (pToStruct)->m_uData3 |=  (uint32)  (pFromData)[15] << 24;

  /* Unpack Member: m_uData2 */
  (pToStruct)->m_uData2 =  (uint32)  (pFromData)[8] ;
  (pToStruct)->m_uData2 |=  (uint32)  (pFromData)[9] << 8;
  (pToStruct)->m_uData2 |=  (uint32)  (pFromData)[10] << 16;
  (pToStruct)->m_uData2 |=  (uint32)  (pFromData)[11] << 24;

  /* Unpack Member: m_uData1 */
  (pToStruct)->m_uData1 =  (uint32)  (pFromData)[4] ;
  (pToStruct)->m_uData1 |=  (uint32)  (pFromData)[5] << 8;
  (pToStruct)->m_uData1 |=  (uint32)  (pFromData)[6] << 16;
  (pToStruct)->m_uData1 |=  (uint32)  (pFromData)[7] << 24;

  /* Unpack Member: m_uData0 */
  (pToStruct)->m_uData0 =  (uint32)  (pFromData)[0] ;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[1] << 8;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[2] << 16;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[3] << 24;

}



/* initialize an instance of this zframe */
void
sbZfC2PpAggregateHashConfigEntry_InitInstance(sbZfC2PpAggregateHashConfigEntry_t *pFrame) {

  pFrame->m_uData9 =  (unsigned int)  0;
  pFrame->m_uData8 =  (unsigned int)  0;
  pFrame->m_uData7 =  (unsigned int)  0;
  pFrame->m_uData6 =  (unsigned int)  0;
  pFrame->m_uData5 =  (unsigned int)  0;
  pFrame->m_uData4 =  (unsigned int)  0;
  pFrame->m_uData3 =  (unsigned int)  0;
  pFrame->m_uData2 =  (unsigned int)  0;
  pFrame->m_uData1 =  (unsigned int)  0;
  pFrame->m_uData0 =  (unsigned int)  0;

}
