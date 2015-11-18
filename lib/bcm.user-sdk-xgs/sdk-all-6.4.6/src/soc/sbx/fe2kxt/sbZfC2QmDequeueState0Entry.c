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
#include "sbZfC2QmDequeueState0Entry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfC2QmDequeueState0Entry_Pack(sbZfC2QmDequeueState0Entry_t *pFrom,
                                uint8 *pToData,
                                uint32 nMaxToDataIndex) {
  int i;
  int size = SB_ZF_C2QMDEQUEUESTATE0ENTRY_SIZE_IN_BYTES;

  if (size % 4) {
    size += (4 - size %4);
  }

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uDrop */
  (pToData)[4] |= ((pFrom)->m_uDrop & 0x01) <<1;

  /* Pack Member: m_uRemainingPagesHi */
  (pToData)[4] |= ((pFrom)->m_uRemainingPagesHi & 0x01);

  /* Pack Member: m_uRemainingPagesLo */
  (pToData)[3] |= ((pFrom)->m_uRemainingPagesLo & 0x3f) <<2;

  /* Pack Member: m_uReplicantNumber */
  (pToData)[1] |= ((pFrom)->m_uReplicantNumber & 0x03) <<6;
  (pToData)[2] |= ((pFrom)->m_uReplicantNumber >> 2) &0xFF;
  (pToData)[3] |= ((pFrom)->m_uReplicantNumber >> 10) & 0x03;

  /* Pack Member: m_uReplicantLength */
  (pToData)[0] |= ((pFrom)->m_uReplicantLength) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_uReplicantLength >> 8) & 0x3f;

  return SB_ZF_C2QMDEQUEUESTATE0ENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfC2QmDequeueState0Entry_Unpack(sbZfC2QmDequeueState0Entry_t *pToStruct,
                                  uint8 *pFromData,
                                  uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;


  /* Unpack operation based on little endian */

  /* Unpack Member: m_uDrop */
  (pToStruct)->m_uDrop =  (uint32)  ((pFromData)[4] >> 1) & 0x01;

  /* Unpack Member: m_uRemainingPagesHi */
  (pToStruct)->m_uRemainingPagesHi =  (uint32)  ((pFromData)[4] ) & 0x01;

  /* Unpack Member: m_uRemainingPagesLo */
  (pToStruct)->m_uRemainingPagesLo =  (uint32)  ((pFromData)[3] >> 2) & 0x3f;

  /* Unpack Member: m_uReplicantNumber */
  (pToStruct)->m_uReplicantNumber =  (uint32)  ((pFromData)[1] >> 6) & 0x03;
  (pToStruct)->m_uReplicantNumber |=  (uint32)  (pFromData)[2] << 2;
  (pToStruct)->m_uReplicantNumber |=  (uint32)  ((pFromData)[3] & 0x03) << 10;

  /* Unpack Member: m_uReplicantLength */
  (pToStruct)->m_uReplicantLength =  (uint32)  (pFromData)[0] ;
  (pToStruct)->m_uReplicantLength |=  (uint32)  ((pFromData)[1] & 0x3f) << 8;

}



/* initialize an instance of this zframe */
void
sbZfC2QmDequeueState0Entry_InitInstance(sbZfC2QmDequeueState0Entry_t *pFrame) {

  pFrame->m_uDrop =  (unsigned int)  0;
  pFrame->m_uRemainingPagesHi =  (unsigned int)  0;
  pFrame->m_uRemainingPagesLo =  (unsigned int)  0;
  pFrame->m_uReplicantNumber =  (unsigned int)  0;
  pFrame->m_uReplicantLength =  (unsigned int)  0;

}
