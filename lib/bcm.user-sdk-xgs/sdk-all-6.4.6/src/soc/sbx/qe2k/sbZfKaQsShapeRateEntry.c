/*
 * $Id: sbZfKaQsShapeRateEntry.c,v 1.2 Broadcom SDK $
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
#include "sbZfKaQsShapeRateEntry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfKaQsShapeRateEntry_Pack(sbZfKaQsShapeRateEntry_t *pFrom,
                            uint8 *pToData,
                            uint32 nMaxToDataIndex) {
#ifdef SAND_BIG_ENDIAN_HOST
  int i;
  int size = SB_ZF_ZFKAQSSHAPERATEENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on bigword endian */

  /* Pack Member: m_nReserved */
  (pToData)[0] |= ((pFrom)->m_nReserved) & 0xFF;

  /* Pack Member: m_nMode */
  (pToData)[1] |= ((pFrom)->m_nMode & 0x01) <<7;

  /* Pack Member: m_nShapeRate */
  (pToData)[3] |= ((pFrom)->m_nShapeRate) & 0xFF;
  (pToData)[2] |= ((pFrom)->m_nShapeRate >> 8) &0xFF;
  (pToData)[1] |= ((pFrom)->m_nShapeRate >> 16) & 0x7f;
#else
  int i;
  int size = SB_ZF_ZFKAQSSHAPERATEENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_nReserved */
  (pToData)[3] |= ((pFrom)->m_nReserved) & 0xFF;

  /* Pack Member: m_nMode */
  (pToData)[2] |= ((pFrom)->m_nMode & 0x01) <<7;

  /* Pack Member: m_nShapeRate */
  (pToData)[0] |= ((pFrom)->m_nShapeRate) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_nShapeRate >> 8) &0xFF;
  (pToData)[2] |= ((pFrom)->m_nShapeRate >> 16) & 0x7f;
#endif

  return SB_ZF_ZFKAQSSHAPERATEENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfKaQsShapeRateEntry_Unpack(sbZfKaQsShapeRateEntry_t *pToStruct,
                              uint8 *pFromData,
                              uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;

#ifdef SAND_BIG_ENDIAN_HOST

  /* Unpack operation based on bigword endian */

  /* Unpack Member: m_nReserved */
  (pToStruct)->m_nReserved =  (uint32)  (pFromData)[0] ;

  /* Unpack Member: m_nMode */
  (pToStruct)->m_nMode =  (uint32)  ((pFromData)[1] >> 7) & 0x01;

  /* Unpack Member: m_nShapeRate */
  (pToStruct)->m_nShapeRate =  (uint32)  (pFromData)[3] ;
  (pToStruct)->m_nShapeRate |=  (uint32)  (pFromData)[2] << 8;
  (pToStruct)->m_nShapeRate |=  (uint32)  ((pFromData)[1] & 0x7f) << 16;
#else

  /* Unpack operation based on little endian */

  /* Unpack Member: m_nReserved */
  (pToStruct)->m_nReserved =  (uint32)  (pFromData)[3] ;

  /* Unpack Member: m_nMode */
  (pToStruct)->m_nMode =  (uint32)  ((pFromData)[2] >> 7) & 0x01;

  /* Unpack Member: m_nShapeRate */
  (pToStruct)->m_nShapeRate =  (uint32)  (pFromData)[0] ;
  (pToStruct)->m_nShapeRate |=  (uint32)  (pFromData)[1] << 8;
  (pToStruct)->m_nShapeRate |=  (uint32)  ((pFromData)[2] & 0x7f) << 16;
#endif

}



/* initialize an instance of this zframe */
void
sbZfKaQsShapeRateEntry_InitInstance(sbZfKaQsShapeRateEntry_t *pFrame) {

  pFrame->m_nReserved =  (unsigned int)  0;
  pFrame->m_nMode =  (unsigned int)  0;
  pFrame->m_nShapeRate =  (unsigned int)  0;

}
