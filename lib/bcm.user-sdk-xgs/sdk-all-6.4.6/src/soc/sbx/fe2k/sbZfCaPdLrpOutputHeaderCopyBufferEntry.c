/*
 * $Id: sbZfCaPdLrpOutputHeaderCopyBufferEntry.c,v 1.2 Broadcom SDK $
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
#include "sbZfCaPdLrpOutputHeaderCopyBufferEntry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfCaPdLrpOutputHeaderCopyBufferEntry_Pack(sbZfCaPdLrpOutputHeaderCopyBufferEntry_t *pFrom,
                                            uint8 *pToData,
                                            uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_CAPDLRPOUTPUTHEADERCOPYBUFFERENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uHeaderCopy7 */
  (pToData)[28] |= ((pFrom)->m_uHeaderCopy7) & 0xFF;
  (pToData)[29] |= ((pFrom)->m_uHeaderCopy7 >> 8) &0xFF;
  (pToData)[30] |= ((pFrom)->m_uHeaderCopy7 >> 16) &0xFF;
  (pToData)[31] |= ((pFrom)->m_uHeaderCopy7 >> 24) &0xFF;

  /* Pack Member: m_uHeaderCopy6 */
  (pToData)[24] |= ((pFrom)->m_uHeaderCopy6) & 0xFF;
  (pToData)[25] |= ((pFrom)->m_uHeaderCopy6 >> 8) &0xFF;
  (pToData)[26] |= ((pFrom)->m_uHeaderCopy6 >> 16) &0xFF;
  (pToData)[27] |= ((pFrom)->m_uHeaderCopy6 >> 24) &0xFF;

  /* Pack Member: m_uHeaderCopy5 */
  (pToData)[20] |= ((pFrom)->m_uHeaderCopy5) & 0xFF;
  (pToData)[21] |= ((pFrom)->m_uHeaderCopy5 >> 8) &0xFF;
  (pToData)[22] |= ((pFrom)->m_uHeaderCopy5 >> 16) &0xFF;
  (pToData)[23] |= ((pFrom)->m_uHeaderCopy5 >> 24) &0xFF;

  /* Pack Member: m_uHeaderCopy4 */
  (pToData)[16] |= ((pFrom)->m_uHeaderCopy4) & 0xFF;
  (pToData)[17] |= ((pFrom)->m_uHeaderCopy4 >> 8) &0xFF;
  (pToData)[18] |= ((pFrom)->m_uHeaderCopy4 >> 16) &0xFF;
  (pToData)[19] |= ((pFrom)->m_uHeaderCopy4 >> 24) &0xFF;

  /* Pack Member: m_uHeaderCopy3 */
  (pToData)[12] |= ((pFrom)->m_uHeaderCopy3) & 0xFF;
  (pToData)[13] |= ((pFrom)->m_uHeaderCopy3 >> 8) &0xFF;
  (pToData)[14] |= ((pFrom)->m_uHeaderCopy3 >> 16) &0xFF;
  (pToData)[15] |= ((pFrom)->m_uHeaderCopy3 >> 24) &0xFF;

  /* Pack Member: m_uHeaderCopy2 */
  (pToData)[8] |= ((pFrom)->m_uHeaderCopy2) & 0xFF;
  (pToData)[9] |= ((pFrom)->m_uHeaderCopy2 >> 8) &0xFF;
  (pToData)[10] |= ((pFrom)->m_uHeaderCopy2 >> 16) &0xFF;
  (pToData)[11] |= ((pFrom)->m_uHeaderCopy2 >> 24) &0xFF;

  /* Pack Member: m_uHeaderCopy1 */
  (pToData)[4] |= ((pFrom)->m_uHeaderCopy1) & 0xFF;
  (pToData)[5] |= ((pFrom)->m_uHeaderCopy1 >> 8) &0xFF;
  (pToData)[6] |= ((pFrom)->m_uHeaderCopy1 >> 16) &0xFF;
  (pToData)[7] |= ((pFrom)->m_uHeaderCopy1 >> 24) &0xFF;

  /* Pack Member: m_uHeaderCopy0 */
  (pToData)[0] |= ((pFrom)->m_uHeaderCopy0) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_uHeaderCopy0 >> 8) &0xFF;
  (pToData)[2] |= ((pFrom)->m_uHeaderCopy0 >> 16) &0xFF;
  (pToData)[3] |= ((pFrom)->m_uHeaderCopy0 >> 24) &0xFF;

  return SB_ZF_CAPDLRPOUTPUTHEADERCOPYBUFFERENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfCaPdLrpOutputHeaderCopyBufferEntry_Unpack(sbZfCaPdLrpOutputHeaderCopyBufferEntry_t *pToStruct,
                                              uint8 *pFromData,
                                              uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uHeaderCopy7 */
  (pToStruct)->m_uHeaderCopy7 =  (uint32)  (pFromData)[28] ;
  (pToStruct)->m_uHeaderCopy7 |=  (uint32)  (pFromData)[29] << 8;
  (pToStruct)->m_uHeaderCopy7 |=  (uint32)  (pFromData)[30] << 16;
  (pToStruct)->m_uHeaderCopy7 |=  (uint32)  (pFromData)[31] << 24;

  /* Unpack Member: m_uHeaderCopy6 */
  (pToStruct)->m_uHeaderCopy6 =  (uint32)  (pFromData)[24] ;
  (pToStruct)->m_uHeaderCopy6 |=  (uint32)  (pFromData)[25] << 8;
  (pToStruct)->m_uHeaderCopy6 |=  (uint32)  (pFromData)[26] << 16;
  (pToStruct)->m_uHeaderCopy6 |=  (uint32)  (pFromData)[27] << 24;

  /* Unpack Member: m_uHeaderCopy5 */
  (pToStruct)->m_uHeaderCopy5 =  (uint32)  (pFromData)[20] ;
  (pToStruct)->m_uHeaderCopy5 |=  (uint32)  (pFromData)[21] << 8;
  (pToStruct)->m_uHeaderCopy5 |=  (uint32)  (pFromData)[22] << 16;
  (pToStruct)->m_uHeaderCopy5 |=  (uint32)  (pFromData)[23] << 24;

  /* Unpack Member: m_uHeaderCopy4 */
  (pToStruct)->m_uHeaderCopy4 =  (uint32)  (pFromData)[16] ;
  (pToStruct)->m_uHeaderCopy4 |=  (uint32)  (pFromData)[17] << 8;
  (pToStruct)->m_uHeaderCopy4 |=  (uint32)  (pFromData)[18] << 16;
  (pToStruct)->m_uHeaderCopy4 |=  (uint32)  (pFromData)[19] << 24;

  /* Unpack Member: m_uHeaderCopy3 */
  (pToStruct)->m_uHeaderCopy3 =  (uint32)  (pFromData)[12] ;
  (pToStruct)->m_uHeaderCopy3 |=  (uint32)  (pFromData)[13] << 8;
  (pToStruct)->m_uHeaderCopy3 |=  (uint32)  (pFromData)[14] << 16;
  (pToStruct)->m_uHeaderCopy3 |=  (uint32)  (pFromData)[15] << 24;

  /* Unpack Member: m_uHeaderCopy2 */
  (pToStruct)->m_uHeaderCopy2 =  (uint32)  (pFromData)[8] ;
  (pToStruct)->m_uHeaderCopy2 |=  (uint32)  (pFromData)[9] << 8;
  (pToStruct)->m_uHeaderCopy2 |=  (uint32)  (pFromData)[10] << 16;
  (pToStruct)->m_uHeaderCopy2 |=  (uint32)  (pFromData)[11] << 24;

  /* Unpack Member: m_uHeaderCopy1 */
  (pToStruct)->m_uHeaderCopy1 =  (uint32)  (pFromData)[4] ;
  (pToStruct)->m_uHeaderCopy1 |=  (uint32)  (pFromData)[5] << 8;
  (pToStruct)->m_uHeaderCopy1 |=  (uint32)  (pFromData)[6] << 16;
  (pToStruct)->m_uHeaderCopy1 |=  (uint32)  (pFromData)[7] << 24;

  /* Unpack Member: m_uHeaderCopy0 */
  (pToStruct)->m_uHeaderCopy0 =  (uint32)  (pFromData)[0] ;
  (pToStruct)->m_uHeaderCopy0 |=  (uint32)  (pFromData)[1] << 8;
  (pToStruct)->m_uHeaderCopy0 |=  (uint32)  (pFromData)[2] << 16;
  (pToStruct)->m_uHeaderCopy0 |=  (uint32)  (pFromData)[3] << 24;

}



/* initialize an instance of this zframe */
void
sbZfCaPdLrpOutputHeaderCopyBufferEntry_InitInstance(sbZfCaPdLrpOutputHeaderCopyBufferEntry_t *pFrame) {

  pFrame->m_uHeaderCopy7 =  (unsigned int)  0;
  pFrame->m_uHeaderCopy6 =  (unsigned int)  0;
  pFrame->m_uHeaderCopy5 =  (unsigned int)  0;
  pFrame->m_uHeaderCopy4 =  (unsigned int)  0;
  pFrame->m_uHeaderCopy3 =  (unsigned int)  0;
  pFrame->m_uHeaderCopy2 =  (unsigned int)  0;
  pFrame->m_uHeaderCopy1 =  (unsigned int)  0;
  pFrame->m_uHeaderCopy0 =  (unsigned int)  0;

}
