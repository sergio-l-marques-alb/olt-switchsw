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


#include "sbTypes.h"
#include "sbZfC2PpCamConfigurationInstance0Entry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfC2PpCamConfigurationInstance0Entry_Pack(sbZfC2PpCamConfigurationInstance0Entry_t *pFrom,
                                            uint8 *pToData,
                                            uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_C2PPCAMCONFIGURATIONINSTANCE0ENTRY_SIZE_IN_BYTES;

  if (size % 4) {
    size += (4 - size %4);
  }

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uMask6 */
  (pToData)[50] |= ((pFrom)->m_uMask6 & 0x03) <<6;
  (pToData)[51] |= ((pFrom)->m_uMask6 >> 2) &0xFF;
  (pToData)[52] |= ((pFrom)->m_uMask6 >> 10) &0xFF;
  (pToData)[53] |= ((pFrom)->m_uMask6 >> 18) & 0x3f;

  /* Pack Member: m_uMask5 */
  (pToData)[46] |= ((pFrom)->m_uMask5 & 0x03) <<6;
  (pToData)[47] |= ((pFrom)->m_uMask5 >> 2) &0xFF;
  (pToData)[48] |= ((pFrom)->m_uMask5 >> 10) &0xFF;
  (pToData)[49] |= ((pFrom)->m_uMask5 >> 18) &0xFF;
  (pToData)[50] |= ((pFrom)->m_uMask5 >> 26) & 0x3f;

  /* Pack Member: m_uMask4 */
  (pToData)[42] |= ((pFrom)->m_uMask4 & 0x03) <<6;
  (pToData)[43] |= ((pFrom)->m_uMask4 >> 2) &0xFF;
  (pToData)[44] |= ((pFrom)->m_uMask4 >> 10) &0xFF;
  (pToData)[45] |= ((pFrom)->m_uMask4 >> 18) &0xFF;
  (pToData)[46] |= ((pFrom)->m_uMask4 >> 26) & 0x3f;

  /* Pack Member: m_uMask3 */
  (pToData)[38] |= ((pFrom)->m_uMask3 & 0x03) <<6;
  (pToData)[39] |= ((pFrom)->m_uMask3 >> 2) &0xFF;
  (pToData)[40] |= ((pFrom)->m_uMask3 >> 10) &0xFF;
  (pToData)[41] |= ((pFrom)->m_uMask3 >> 18) &0xFF;
  (pToData)[42] |= ((pFrom)->m_uMask3 >> 26) & 0x3f;

  /* Pack Member: m_uMask2 */
  (pToData)[34] |= ((pFrom)->m_uMask2 & 0x03) <<6;
  (pToData)[35] |= ((pFrom)->m_uMask2 >> 2) &0xFF;
  (pToData)[36] |= ((pFrom)->m_uMask2 >> 10) &0xFF;
  (pToData)[37] |= ((pFrom)->m_uMask2 >> 18) &0xFF;
  (pToData)[38] |= ((pFrom)->m_uMask2 >> 26) & 0x3f;

  /* Pack Member: m_uMask1 */
  (pToData)[30] |= ((pFrom)->m_uMask1 & 0x03) <<6;
  (pToData)[31] |= ((pFrom)->m_uMask1 >> 2) &0xFF;
  (pToData)[32] |= ((pFrom)->m_uMask1 >> 10) &0xFF;
  (pToData)[33] |= ((pFrom)->m_uMask1 >> 18) &0xFF;
  (pToData)[34] |= ((pFrom)->m_uMask1 >> 26) & 0x3f;

  /* Pack Member: m_uMask0 */
  (pToData)[27] |= ((pFrom)->m_uMask0) & 0xFF;
  (pToData)[28] |= ((pFrom)->m_uMask0 >> 8) &0xFF;
  (pToData)[29] |= ((pFrom)->m_uMask0 >> 16) &0xFF;
  (pToData)[30] |= ((pFrom)->m_uMask0 >> 24) & 0x3f;

  /* Pack Member: m_uData6 */
  (pToData)[24] |= ((pFrom)->m_uData6) & 0xFF;
  (pToData)[25] |= ((pFrom)->m_uData6 >> 8) &0xFF;
  (pToData)[26] |= ((pFrom)->m_uData6 >> 16) &0xFF;

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
  (pToData)[0] |= ((pFrom)->m_uData0 & 0x3f) <<2;
  (pToData)[1] |= ((pFrom)->m_uData0 >> 6) &0xFF;
  (pToData)[2] |= ((pFrom)->m_uData0 >> 14) &0xFF;
  (pToData)[3] |= ((pFrom)->m_uData0 >> 22) &0xFF;

  /* Pack Member: m_uValid */
  (pToData)[0] |= ((pFrom)->m_uValid & 0x03);

  return SB_ZF_C2PPCAMCONFIGURATIONINSTANCE0ENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfC2PpCamConfigurationInstance0Entry_Unpack(sbZfC2PpCamConfigurationInstance0Entry_t *pToStruct,
                                              uint8 *pFromData,
                                              uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uMask6 */
  (pToStruct)->m_uMask6 =  (uint32)  ((pFromData)[50] >> 6) & 0x03;
  (pToStruct)->m_uMask6 |=  (uint32)  (pFromData)[51] << 2;
  (pToStruct)->m_uMask6 |=  (uint32)  (pFromData)[52] << 10;
  (pToStruct)->m_uMask6 |=  (uint32)  ((pFromData)[53] & 0x3f) << 18;

  /* Unpack Member: m_uMask5 */
  (pToStruct)->m_uMask5 =  (uint32)  ((pFromData)[46] >> 6) & 0x03;
  (pToStruct)->m_uMask5 |=  (uint32)  (pFromData)[47] << 2;
  (pToStruct)->m_uMask5 |=  (uint32)  (pFromData)[48] << 10;
  (pToStruct)->m_uMask5 |=  (uint32)  (pFromData)[49] << 18;
  (pToStruct)->m_uMask5 |=  (uint32)  ((pFromData)[50] & 0x3f) << 26;

  /* Unpack Member: m_uMask4 */
  (pToStruct)->m_uMask4 =  (uint32)  ((pFromData)[42] >> 6) & 0x03;
  (pToStruct)->m_uMask4 |=  (uint32)  (pFromData)[43] << 2;
  (pToStruct)->m_uMask4 |=  (uint32)  (pFromData)[44] << 10;
  (pToStruct)->m_uMask4 |=  (uint32)  (pFromData)[45] << 18;
  (pToStruct)->m_uMask4 |=  (uint32)  ((pFromData)[46] & 0x3f) << 26;

  /* Unpack Member: m_uMask3 */
  (pToStruct)->m_uMask3 =  (uint32)  ((pFromData)[38] >> 6) & 0x03;
  (pToStruct)->m_uMask3 |=  (uint32)  (pFromData)[39] << 2;
  (pToStruct)->m_uMask3 |=  (uint32)  (pFromData)[40] << 10;
  (pToStruct)->m_uMask3 |=  (uint32)  (pFromData)[41] << 18;
  (pToStruct)->m_uMask3 |=  (uint32)  ((pFromData)[42] & 0x3f) << 26;

  /* Unpack Member: m_uMask2 */
  (pToStruct)->m_uMask2 =  (uint32)  ((pFromData)[34] >> 6) & 0x03;
  (pToStruct)->m_uMask2 |=  (uint32)  (pFromData)[35] << 2;
  (pToStruct)->m_uMask2 |=  (uint32)  (pFromData)[36] << 10;
  (pToStruct)->m_uMask2 |=  (uint32)  (pFromData)[37] << 18;
  (pToStruct)->m_uMask2 |=  (uint32)  ((pFromData)[38] & 0x3f) << 26;

  /* Unpack Member: m_uMask1 */
  (pToStruct)->m_uMask1 =  (uint32)  ((pFromData)[30] >> 6) & 0x03;
  (pToStruct)->m_uMask1 |=  (uint32)  (pFromData)[31] << 2;
  (pToStruct)->m_uMask1 |=  (uint32)  (pFromData)[32] << 10;
  (pToStruct)->m_uMask1 |=  (uint32)  (pFromData)[33] << 18;
  (pToStruct)->m_uMask1 |=  (uint32)  ((pFromData)[34] & 0x3f) << 26;

  /* Unpack Member: m_uMask0 */
  (pToStruct)->m_uMask0 =  (uint32)  (pFromData)[27] ;
  (pToStruct)->m_uMask0 |=  (uint32)  (pFromData)[28] << 8;
  (pToStruct)->m_uMask0 |=  (uint32)  (pFromData)[29] << 16;
  (pToStruct)->m_uMask0 |=  (uint32)  ((pFromData)[30] & 0x3f) << 24;

  /* Unpack Member: m_uData6 */
  (pToStruct)->m_uData6 =  (uint32)  (pFromData)[24] ;
  (pToStruct)->m_uData6 |=  (uint32)  (pFromData)[25] << 8;
  (pToStruct)->m_uData6 |=  (uint32)  (pFromData)[26] << 16;

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
  (pToStruct)->m_uData0 =  (uint32)  ((pFromData)[0] >> 2) & 0x3f;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[1] << 6;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[2] << 14;
  (pToStruct)->m_uData0 |=  (uint32)  (pFromData)[3] << 22;

  /* Unpack Member: m_uValid */
  (pToStruct)->m_uValid =  (uint32)  ((pFromData)[0] ) & 0x03;

}



/* initialize an instance of this zframe */
void
sbZfC2PpCamConfigurationInstance0Entry_InitInstance(sbZfC2PpCamConfigurationInstance0Entry_t *pFrame) {

  pFrame->m_uMask6 =  (unsigned int)  0;
  pFrame->m_uMask5 =  (unsigned int)  0;
  pFrame->m_uMask4 =  (unsigned int)  0;
  pFrame->m_uMask3 =  (unsigned int)  0;
  pFrame->m_uMask2 =  (unsigned int)  0;
  pFrame->m_uMask1 =  (unsigned int)  0;
  pFrame->m_uMask0 =  (unsigned int)  0;
  pFrame->m_uData6 =  (unsigned int)  0;
  pFrame->m_uData5 =  (unsigned int)  0;
  pFrame->m_uData4 =  (unsigned int)  0;
  pFrame->m_uData3 =  (unsigned int)  0;
  pFrame->m_uData2 =  (unsigned int)  0;
  pFrame->m_uData1 =  (unsigned int)  0;
  pFrame->m_uData0 =  (unsigned int)  0;
  pFrame->m_uValid =  (unsigned int)  0;

}
