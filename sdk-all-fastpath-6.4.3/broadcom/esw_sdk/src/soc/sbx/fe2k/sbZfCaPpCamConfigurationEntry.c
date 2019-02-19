/*
 * $Id: sbZfCaPpCamConfigurationEntry.c,v 1.2 Broadcom SDK $
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
#include "sbZfCaPpCamConfigurationEntry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfCaPpCamConfigurationEntry_Pack(sbZfCaPpCamConfigurationEntry_t *pFrom,
                                   uint8 *pToData,
                                   uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_CAPPCAMCONFIGURATIONENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uSpare0 */
  (pToData)[47] |= ((pFrom)->m_uSpare0) & 0xFF;

  /* Pack Member: m_uMask5 */
  (pToData)[44] |= ((pFrom)->m_uMask5) & 0xFF;
  (pToData)[45] |= ((pFrom)->m_uMask5 >> 8) &0xFF;
  (pToData)[46] |= ((pFrom)->m_uMask5 >> 16) &0xFF;

  /* Pack Member: m_uMask4 */
  (pToData)[40] |= ((pFrom)->m_uMask4) & 0xFF;
  (pToData)[41] |= ((pFrom)->m_uMask4 >> 8) &0xFF;
  (pToData)[42] |= ((pFrom)->m_uMask4 >> 16) &0xFF;
  (pToData)[43] |= ((pFrom)->m_uMask4 >> 24) &0xFF;

  /* Pack Member: m_uMask3 */
  (pToData)[36] |= ((pFrom)->m_uMask3) & 0xFF;
  (pToData)[37] |= ((pFrom)->m_uMask3 >> 8) &0xFF;
  (pToData)[38] |= ((pFrom)->m_uMask3 >> 16) &0xFF;
  (pToData)[39] |= ((pFrom)->m_uMask3 >> 24) &0xFF;

  /* Pack Member: m_uMask2 */
  (pToData)[32] |= ((pFrom)->m_uMask2) & 0xFF;
  (pToData)[33] |= ((pFrom)->m_uMask2 >> 8) &0xFF;
  (pToData)[34] |= ((pFrom)->m_uMask2 >> 16) &0xFF;
  (pToData)[35] |= ((pFrom)->m_uMask2 >> 24) &0xFF;

  /* Pack Member: m_uMask1 */
  (pToData)[28] |= ((pFrom)->m_uMask1) & 0xFF;
  (pToData)[29] |= ((pFrom)->m_uMask1 >> 8) &0xFF;
  (pToData)[30] |= ((pFrom)->m_uMask1 >> 16) &0xFF;
  (pToData)[31] |= ((pFrom)->m_uMask1 >> 24) &0xFF;

  /* Pack Member: m_uMask0 */
  (pToData)[24] |= ((pFrom)->m_uMask0) & 0xFF;
  (pToData)[25] |= ((pFrom)->m_uMask0 >> 8) &0xFF;
  (pToData)[26] |= ((pFrom)->m_uMask0 >> 16) &0xFF;
  (pToData)[27] |= ((pFrom)->m_uMask0 >> 24) &0xFF;

  /* Pack Member: m_uSpare1 */
  (pToData)[23] |= ((pFrom)->m_uSpare1 & 0x3f) <<2;

  /* Pack Member: m_uValid */
  (pToData)[23] |= ((pFrom)->m_uValid & 0x03);

  /* Pack Member: m_uData5 */
  (pToData)[20] |= ((pFrom)->m_uData5) & 0xFF;
  (pToData)[21] |= ((pFrom)->m_uData5 >> 8) &0xFF;
  (pToData)[22] |= ((pFrom)->m_uData5 >> 16) &0xFF;

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

  return SB_ZF_CAPPCAMCONFIGURATIONENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfCaPpCamConfigurationEntry_Unpack(sbZfCaPpCamConfigurationEntry_t *pToStruct,
                                     uint8 *pFromData,
                                     uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uSpare0 */
  (pToStruct)->m_uSpare0 =  (uint32)  (pFromData)[47] ;

  /* Unpack Member: m_uMask5 */
  (pToStruct)->m_uMask5 =  (uint32)  (pFromData)[44] ;
  (pToStruct)->m_uMask5 |=  (uint32)  (pFromData)[45] << 8;
  (pToStruct)->m_uMask5 |=  (uint32)  (pFromData)[46] << 16;

  /* Unpack Member: m_uMask4 */
  (pToStruct)->m_uMask4 =  (uint32)  (pFromData)[40] ;
  (pToStruct)->m_uMask4 |=  (uint32)  (pFromData)[41] << 8;
  (pToStruct)->m_uMask4 |=  (uint32)  (pFromData)[42] << 16;
  (pToStruct)->m_uMask4 |=  (uint32)  (pFromData)[43] << 24;

  /* Unpack Member: m_uMask3 */
  (pToStruct)->m_uMask3 =  (uint32)  (pFromData)[36] ;
  (pToStruct)->m_uMask3 |=  (uint32)  (pFromData)[37] << 8;
  (pToStruct)->m_uMask3 |=  (uint32)  (pFromData)[38] << 16;
  (pToStruct)->m_uMask3 |=  (uint32)  (pFromData)[39] << 24;

  /* Unpack Member: m_uMask2 */
  (pToStruct)->m_uMask2 =  (uint32)  (pFromData)[32] ;
  (pToStruct)->m_uMask2 |=  (uint32)  (pFromData)[33] << 8;
  (pToStruct)->m_uMask2 |=  (uint32)  (pFromData)[34] << 16;
  (pToStruct)->m_uMask2 |=  (uint32)  (pFromData)[35] << 24;

  /* Unpack Member: m_uMask1 */
  (pToStruct)->m_uMask1 =  (uint32)  (pFromData)[28] ;
  (pToStruct)->m_uMask1 |=  (uint32)  (pFromData)[29] << 8;
  (pToStruct)->m_uMask1 |=  (uint32)  (pFromData)[30] << 16;
  (pToStruct)->m_uMask1 |=  (uint32)  (pFromData)[31] << 24;

  /* Unpack Member: m_uMask0 */
  (pToStruct)->m_uMask0 =  (uint32)  (pFromData)[24] ;
  (pToStruct)->m_uMask0 |=  (uint32)  (pFromData)[25] << 8;
  (pToStruct)->m_uMask0 |=  (uint32)  (pFromData)[26] << 16;
  (pToStruct)->m_uMask0 |=  (uint32)  (pFromData)[27] << 24;

  /* Unpack Member: m_uSpare1 */
  (pToStruct)->m_uSpare1 =  (uint32)  ((pFromData)[23] >> 2) & 0x3f;

  /* Unpack Member: m_uValid */
  (pToStruct)->m_uValid =  (uint32)  ((pFromData)[23] ) & 0x03;

  /* Unpack Member: m_uData5 */
  (pToStruct)->m_uData5 =  (uint32)  (pFromData)[20] ;
  (pToStruct)->m_uData5 |=  (uint32)  (pFromData)[21] << 8;
  (pToStruct)->m_uData5 |=  (uint32)  (pFromData)[22] << 16;

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
sbZfCaPpCamConfigurationEntry_InitInstance(sbZfCaPpCamConfigurationEntry_t *pFrame) {

  pFrame->m_uSpare0 =  (unsigned int)  0;
  pFrame->m_uMask5 =  (unsigned int)  0;
  pFrame->m_uMask4 =  (unsigned int)  0;
  pFrame->m_uMask3 =  (unsigned int)  0;
  pFrame->m_uMask2 =  (unsigned int)  0;
  pFrame->m_uMask1 =  (unsigned int)  0;
  pFrame->m_uMask0 =  (unsigned int)  0;
  pFrame->m_uSpare1 =  (unsigned int)  0;
  pFrame->m_uValid =  (unsigned int)  0;
  pFrame->m_uData5 =  (unsigned int)  0;
  pFrame->m_uData4 =  (unsigned int)  0;
  pFrame->m_uData3 =  (unsigned int)  0;
  pFrame->m_uData2 =  (unsigned int)  0;
  pFrame->m_uData1 =  (unsigned int)  0;
  pFrame->m_uData0 =  (unsigned int)  0;

}
