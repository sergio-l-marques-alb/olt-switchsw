/*
 * $Id: sbZfKaEgMemFifoParamEntry.c,v 1.2 Broadcom SDK $
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
#include "sbZfKaEgMemShapingEntry.hx"
#include "sbZfKaEgMemFifoParamEntry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfKaEgMemFifoParamEntry_Pack(sbZfKaEgMemFifoParamEntry_t *pFrom,
                               uint8 *pToData,
                               uint32 nMaxToDataIndex) {
#ifdef SAND_BIG_ENDIAN_HOST
  int i;
  int size = SB_ZF_ZFKAEGMEMFIFOPARAMENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on bigword endian */

  /* Pack Member: m_nThreshHi */
  (pToData)[0] |= ((pFrom)->m_nThreshHi & 0x3f) <<2;
  (pToData)[7] |= ((pFrom)->m_nThreshHi >> 6) & 0x0f;

  /* Pack Member: m_nThreshLo */
  (pToData)[1] |= ((pFrom)->m_nThreshLo) & 0xFF;
  (pToData)[0] |= ((pFrom)->m_nThreshLo >> 8) & 0x03;

  /* Pack Member: m_nShaper1 */
  (pToData)[2] |= ((pFrom)->m_nShaper1) & 0xFF;

  /* Pack Member: m_nShaper0 */
  (pToData)[3] |= ((pFrom)->m_nShaper0) & 0xFF;
#else
  int i;
  int size = SB_ZF_ZFKAEGMEMFIFOPARAMENTRY_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_nThreshHi */
  (pToData)[3] |= ((pFrom)->m_nThreshHi & 0x3f) <<2;
  (pToData)[4] |= ((pFrom)->m_nThreshHi >> 6) & 0x0f;

  /* Pack Member: m_nThreshLo */
  (pToData)[2] |= ((pFrom)->m_nThreshLo) & 0xFF;
  (pToData)[3] |= ((pFrom)->m_nThreshLo >> 8) & 0x03;

  /* Pack Member: m_nShaper1 */
  (pToData)[1] |= ((pFrom)->m_nShaper1) & 0xFF;

  /* Pack Member: m_nShaper0 */
  (pToData)[0] |= ((pFrom)->m_nShaper0) & 0xFF;
#endif

  return SB_ZF_ZFKAEGMEMFIFOPARAMENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfKaEgMemFifoParamEntry_Unpack(sbZfKaEgMemFifoParamEntry_t *pToStruct,
                                 uint8 *pFromData,
                                 uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;

#ifdef SAND_BIG_ENDIAN_HOST

  /* Unpack operation based on bigword endian */

  /* Unpack Member: m_nThreshHi */
  (pToStruct)->m_nThreshHi =  (uint32)  ((pFromData)[0] >> 2) & 0x3f;
  (pToStruct)->m_nThreshHi |=  (uint32)  ((pFromData)[7] & 0x0f) << 6;

  /* Unpack Member: m_nThreshLo */
  (pToStruct)->m_nThreshLo =  (uint32)  (pFromData)[1] ;
  (pToStruct)->m_nThreshLo |=  (uint32)  ((pFromData)[0] & 0x03) << 8;

  /* Unpack Member: m_nShaper1 */
  (pToStruct)->m_nShaper1 =  (uint32)  (pFromData)[2] ;

  /* Unpack Member: m_nShaper0 */
  (pToStruct)->m_nShaper0 =  (uint32)  (pFromData)[3] ;
#else

  /* Unpack operation based on little endian */

  /* Unpack Member: m_nThreshHi */
  (pToStruct)->m_nThreshHi =  (uint32)  ((pFromData)[3] >> 2) & 0x3f;
  (pToStruct)->m_nThreshHi |=  (uint32)  ((pFromData)[4] & 0x0f) << 6;

  /* Unpack Member: m_nThreshLo */
  (pToStruct)->m_nThreshLo =  (uint32)  (pFromData)[2] ;
  (pToStruct)->m_nThreshLo |=  (uint32)  ((pFromData)[3] & 0x03) << 8;

  /* Unpack Member: m_nShaper1 */
  (pToStruct)->m_nShaper1 =  (uint32)  (pFromData)[1] ;

  /* Unpack Member: m_nShaper0 */
  (pToStruct)->m_nShaper0 =  (uint32)  (pFromData)[0] ;
#endif

}



/* initialize an instance of this zframe */
void
sbZfKaEgMemFifoParamEntry_InitInstance(sbZfKaEgMemFifoParamEntry_t *pFrame) {

  pFrame->m_nThreshHi =  (unsigned int)  0;
  pFrame->m_nThreshLo =  (unsigned int)  0;
  pFrame->m_nShaper1 =  (unsigned int)  0;
  pFrame->m_nShaper0 =  (unsigned int)  0;

}
