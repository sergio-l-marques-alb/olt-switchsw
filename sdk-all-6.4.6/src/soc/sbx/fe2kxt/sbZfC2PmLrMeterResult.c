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
#include "sbZfC2PmLrMeterResult.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfC2PmLrMeterResult_Pack(sbZfC2PmLrMeterResult_t *pFrom,
                           uint8 *pToData,
                           uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_C2PMLRMETERRESULT_SIZE_IN_BYTES;

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uDrop */
  (pToData)[3] |= ((pFrom)->m_uDrop & 0x01) <<7;

  /* Pack Member: m_uInvalidPolcierId */
  (pToData)[3] |= ((pFrom)->m_uInvalidPolcierId & 0x01) <<6;

  /* Pack Member: m_uMmuError */
  (pToData)[3] |= ((pFrom)->m_uMmuError & 0x01) <<5;

  /* Pack Member: m_uResv */
  (pToData)[3] |= ((pFrom)->m_uResv & 0x03) <<3;

  /* Pack Member: m_uPolicerId */
  (pToData)[0] |= ((pFrom)->m_uPolicerId & 0x0f) <<4;
  (pToData)[1] |= ((pFrom)->m_uPolicerId >> 4) &0xFF;
  (pToData)[2] |= ((pFrom)->m_uPolicerId >> 12) &0xFF;
  (pToData)[3] |= ((pFrom)->m_uPolicerId >> 20) & 0x07;

  /* Pack Member: m_uPmuDp */
  (pToData)[0] |= ((pFrom)->m_uPmuDp & 0x03) <<2;

  /* Pack Member: m_uLrpDp */
  (pToData)[0] |= ((pFrom)->m_uLrpDp & 0x03);

  return SB_ZF_C2PMLRMETERRESULT_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfC2PmLrMeterResult_Unpack(sbZfC2PmLrMeterResult_t *pToStruct,
                             uint8 *pFromData,
                             uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uDrop */
  (pToStruct)->m_uDrop =  (uint8)  ((pFromData)[3] >> 7) & 0x01;

  /* Unpack Member: m_uInvalidPolcierId */
  (pToStruct)->m_uInvalidPolcierId =  (uint32)  ((pFromData)[3] >> 6) & 0x01;

  /* Unpack Member: m_uMmuError */
  (pToStruct)->m_uMmuError =  (uint32)  ((pFromData)[3] >> 5) & 0x01;

  /* Unpack Member: m_uResv */
  (pToStruct)->m_uResv =  (uint32)  ((pFromData)[3] >> 3) & 0x03;

  /* Unpack Member: m_uPolicerId */
  (pToStruct)->m_uPolicerId =  (uint32)  ((pFromData)[0] >> 4) & 0x0f;
  (pToStruct)->m_uPolicerId |=  (uint32)  (pFromData)[1] << 4;
  (pToStruct)->m_uPolicerId |=  (uint32)  (pFromData)[2] << 12;
  (pToStruct)->m_uPolicerId |=  (uint32)  ((pFromData)[3] & 0x07) << 20;

  /* Unpack Member: m_uPmuDp */
  (pToStruct)->m_uPmuDp =  (uint8)  ((pFromData)[0] >> 2) & 0x03;

  /* Unpack Member: m_uLrpDp */
  (pToStruct)->m_uLrpDp =  (uint8)  ((pFromData)[0] ) & 0x03;

}



/* initialize an instance of this zframe */
void
sbZfC2PmLrMeterResult_InitInstance(sbZfC2PmLrMeterResult_t *pFrame) {

  pFrame->m_uDrop =  (unsigned int)  0;
  pFrame->m_uInvalidPolcierId =  (unsigned int)  0;
  pFrame->m_uMmuError =  (unsigned int)  0;
  pFrame->m_uResv =  (unsigned int)  0;
  pFrame->m_uPolicerId =  (unsigned int)  0;
  pFrame->m_uPmuDp =  (unsigned int)  0;
  pFrame->m_uLrpDp =  (unsigned int)  0;

}
