/*
 * $Id: sbZfKaQmWredCurvesTableEntry.c,v 1.2 Broadcom SDK $
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
#include "sbZfKaQmWredCurvesTableEntry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfKaQmWredCurvesTableEntry_Pack(sbZfKaQmWredCurvesTableEntry_t *pFrom,
                                  uint8 *pToData,
                                  uint32 nMaxToDataIndex) {
#ifdef SAND_BIG_ENDIAN_HOST
  int i;
  int size = SB_ZF_ZFKAQMWREDCURVESTABLEENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on bigword endian */

  /* Pack Member: m_nTmin */
  (pToData)[5] |= ((pFrom)->m_nTmin) & 0xFF;
  (pToData)[4] |= ((pFrom)->m_nTmin >> 8) &0xFF;

  /* Pack Member: m_nTmax */
  (pToData)[7] |= ((pFrom)->m_nTmax) & 0xFF;
  (pToData)[6] |= ((pFrom)->m_nTmax >> 8) &0xFF;

  /* Pack Member: m_nTecn */
  (pToData)[1] |= ((pFrom)->m_nTecn) & 0xFF;
  (pToData)[0] |= ((pFrom)->m_nTecn >> 8) &0xFF;

  /* Pack Member: m_nScale */
  (pToData)[2] |= ((pFrom)->m_nScale & 0x0f) <<4;

  /* Pack Member: m_nSlope */
  (pToData)[3] |= ((pFrom)->m_nSlope) & 0xFF;
  (pToData)[2] |= ((pFrom)->m_nSlope >> 8) & 0x0f;
#else
  int i;
  int size = SB_ZF_ZFKAQMWREDCURVESTABLEENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_nTmin */
  (pToData)[6] |= ((pFrom)->m_nTmin) & 0xFF;
  (pToData)[7] |= ((pFrom)->m_nTmin >> 8) &0xFF;

  /* Pack Member: m_nTmax */
  (pToData)[4] |= ((pFrom)->m_nTmax) & 0xFF;
  (pToData)[5] |= ((pFrom)->m_nTmax >> 8) &0xFF;

  /* Pack Member: m_nTecn */
  (pToData)[2] |= ((pFrom)->m_nTecn) & 0xFF;
  (pToData)[3] |= ((pFrom)->m_nTecn >> 8) &0xFF;

  /* Pack Member: m_nScale */
  (pToData)[1] |= ((pFrom)->m_nScale & 0x0f) <<4;

  /* Pack Member: m_nSlope */
  (pToData)[0] |= ((pFrom)->m_nSlope) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_nSlope >> 8) & 0x0f;
#endif

  return SB_ZF_ZFKAQMWREDCURVESTABLEENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfKaQmWredCurvesTableEntry_Unpack(sbZfKaQmWredCurvesTableEntry_t *pToStruct,
                                    uint8 *pFromData,
                                    uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;

#ifdef SAND_BIG_ENDIAN_HOST

  /* Unpack operation based on bigword endian */

  /* Unpack Member: m_nTmin */
  (pToStruct)->m_nTmin =  (uint32)  (pFromData)[5] ;
  (pToStruct)->m_nTmin |=  (uint32)  (pFromData)[4] << 8;

  /* Unpack Member: m_nTmax */
  (pToStruct)->m_nTmax =  (uint32)  (pFromData)[7] ;
  (pToStruct)->m_nTmax |=  (uint32)  (pFromData)[6] << 8;

  /* Unpack Member: m_nTecn */
  (pToStruct)->m_nTecn =  (uint32)  (pFromData)[1] ;
  (pToStruct)->m_nTecn |=  (uint32)  (pFromData)[0] << 8;

  /* Unpack Member: m_nScale */
  (pToStruct)->m_nScale =  (uint32)  ((pFromData)[2] >> 4) & 0x0f;

  /* Unpack Member: m_nSlope */
  (pToStruct)->m_nSlope =  (uint32)  (pFromData)[3] ;
  (pToStruct)->m_nSlope |=  (uint32)  ((pFromData)[2] & 0x0f) << 8;
#else

  /* Unpack operation based on little endian */

  /* Unpack Member: m_nTmin */
  (pToStruct)->m_nTmin =  (uint32)  (pFromData)[6] ;
  (pToStruct)->m_nTmin |=  (uint32)  (pFromData)[7] << 8;

  /* Unpack Member: m_nTmax */
  (pToStruct)->m_nTmax =  (uint32)  (pFromData)[4] ;
  (pToStruct)->m_nTmax |=  (uint32)  (pFromData)[5] << 8;

  /* Unpack Member: m_nTecn */
  (pToStruct)->m_nTecn =  (uint32)  (pFromData)[2] ;
  (pToStruct)->m_nTecn |=  (uint32)  (pFromData)[3] << 8;

  /* Unpack Member: m_nScale */
  (pToStruct)->m_nScale =  (uint32)  ((pFromData)[1] >> 4) & 0x0f;

  /* Unpack Member: m_nSlope */
  (pToStruct)->m_nSlope =  (uint32)  (pFromData)[0] ;
  (pToStruct)->m_nSlope |=  (uint32)  ((pFromData)[1] & 0x0f) << 8;
#endif

}



/* initialize an instance of this zframe */
void
sbZfKaQmWredCurvesTableEntry_InitInstance(sbZfKaQmWredCurvesTableEntry_t *pFrame) {

  pFrame->m_nTmin =  (unsigned int)  0;
  pFrame->m_nTmax =  (unsigned int)  0;
  pFrame->m_nTecn =  (unsigned int)  0;
  pFrame->m_nScale =  (unsigned int)  0;
  pFrame->m_nSlope =  (unsigned int)  0;

}
