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
#include "sbZfC2DiagUcodeCtl.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfC2DiagUcodeCtl_Pack(sbZfC2DiagUcodeCtl_t *pFrom,
                        uint8 *pToData,
                        uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_C2DIAGUCODECTL_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_ulStatus */
  (pToData)[0] |= ((pFrom)->m_ulStatus & 0x01);

  /* Pack Member: m_ulErrInject0 */
  (pToData)[0] |= ((pFrom)->m_ulErrInject0 & 0x01) <<1;

  /* Pack Member: m_ulErrInject1 */
  (pToData)[0] |= ((pFrom)->m_ulErrInject1 & 0x01) <<2;

  /* Pack Member: m_ulErrInd */
  (pToData)[0] |= ((pFrom)->m_ulErrInd & 0x01) <<3;

  /* Pack Member: m_ulUcLoaded */
  (pToData)[0] |= ((pFrom)->m_ulUcLoaded & 0x01) <<4;

  /* Pack Member: m_ulLrpState */
  (pToData)[0] |= ((pFrom)->m_ulLrpState & 0x03) <<5;

  /* Pack Member: m_ulMemExstMm0N0 */
  (pToData)[0] |= ((pFrom)->m_ulMemExstMm0N0 & 0x01) <<7;

  /* Pack Member: m_ulMemExstMm0N1 */
  (pToData)[1] |= ((pFrom)->m_ulMemExstMm0N1 & 0x01);

  /* Pack Member: m_ulMemExstMm0W */
  (pToData)[1] |= ((pFrom)->m_ulMemExstMm0W & 0x01) <<1;

  /* Pack Member: m_ulMemExstMm1N0 */
  (pToData)[1] |= ((pFrom)->m_ulMemExstMm1N0 & 0x01) <<2;

  /* Pack Member: m_ulMemExstMm1N1 */
  (pToData)[1] |= ((pFrom)->m_ulMemExstMm1N1 & 0x01) <<3;

  /* Pack Member: m_ulMemExstMm1W */
  (pToData)[1] |= ((pFrom)->m_ulMemExstMm1W & 0x01) <<4;

  /* Pack Member: m_ulTestOffset */
  (pToData)[1] |= ((pFrom)->m_ulTestOffset & 0x07) <<5;
  (pToData)[2] |= ((pFrom)->m_ulTestOffset >> 3) &0xFF;
  (pToData)[3] |= ((pFrom)->m_ulTestOffset >> 11) &0xFF;

  return SB_ZF_C2DIAGUCODECTL_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfC2DiagUcodeCtl_Unpack(sbZfC2DiagUcodeCtl_t *pToStruct,
                          uint8 *pFromData,
                          uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_ulStatus */
  (pToStruct)->m_ulStatus =  (uint32)  ((pFromData)[0] ) & 0x01;

  /* Unpack Member: m_ulErrInject0 */
  (pToStruct)->m_ulErrInject0 =  (uint32)  ((pFromData)[0] >> 1) & 0x01;

  /* Unpack Member: m_ulErrInject1 */
  (pToStruct)->m_ulErrInject1 =  (uint32)  ((pFromData)[0] >> 2) & 0x01;

  /* Unpack Member: m_ulErrInd */
  (pToStruct)->m_ulErrInd =  (uint32)  ((pFromData)[0] >> 3) & 0x01;

  /* Unpack Member: m_ulUcLoaded */
  (pToStruct)->m_ulUcLoaded =  (uint32)  ((pFromData)[0] >> 4) & 0x01;

  /* Unpack Member: m_ulLrpState */
  (pToStruct)->m_ulLrpState =  (uint32)  ((pFromData)[0] >> 5) & 0x03;

  /* Unpack Member: m_ulMemExstMm0N0 */
  (pToStruct)->m_ulMemExstMm0N0 =  (uint32)  ((pFromData)[0] >> 7) & 0x01;

  /* Unpack Member: m_ulMemExstMm0N1 */
  (pToStruct)->m_ulMemExstMm0N1 =  (uint32)  ((pFromData)[1] ) & 0x01;

  /* Unpack Member: m_ulMemExstMm0W */
  (pToStruct)->m_ulMemExstMm0W =  (uint32)  ((pFromData)[1] >> 1) & 0x01;

  /* Unpack Member: m_ulMemExstMm1N0 */
  (pToStruct)->m_ulMemExstMm1N0 =  (uint32)  ((pFromData)[1] >> 2) & 0x01;

  /* Unpack Member: m_ulMemExstMm1N1 */
  (pToStruct)->m_ulMemExstMm1N1 =  (uint32)  ((pFromData)[1] >> 3) & 0x01;

  /* Unpack Member: m_ulMemExstMm1W */
  (pToStruct)->m_ulMemExstMm1W =  (uint32)  ((pFromData)[1] >> 4) & 0x01;

  /* Unpack Member: m_ulTestOffset */
  (pToStruct)->m_ulTestOffset =  (uint32)  ((pFromData)[1] >> 5) & 0x07;
  (pToStruct)->m_ulTestOffset |=  (uint32)  (pFromData)[2] << 3;
  (pToStruct)->m_ulTestOffset |=  (uint32)  (pFromData)[3] << 11;

}



/* initialize an instance of this zframe */
void
sbZfC2DiagUcodeCtl_InitInstance(sbZfC2DiagUcodeCtl_t *pFrame) {

  pFrame->m_ulStatus =  (unsigned int)  0;
  pFrame->m_ulErrInject0 =  (unsigned int)  0;
  pFrame->m_ulErrInject1 =  (unsigned int)  0;
  pFrame->m_ulErrInd =  (unsigned int)  0;
  pFrame->m_ulUcLoaded =  (unsigned int)  0;
  pFrame->m_ulLrpState =  (unsigned int)  0;
  pFrame->m_ulMemExstMm0N0 =  (unsigned int)  0;
  pFrame->m_ulMemExstMm0N1 =  (unsigned int)  0;
  pFrame->m_ulMemExstMm0W =  (unsigned int)  0;
  pFrame->m_ulMemExstMm1N0 =  (unsigned int)  0;
  pFrame->m_ulMemExstMm1N1 =  (unsigned int)  0;
  pFrame->m_ulMemExstMm1W =  (unsigned int)  0;
  pFrame->m_ulTestOffset =  (unsigned int)  0;

}
