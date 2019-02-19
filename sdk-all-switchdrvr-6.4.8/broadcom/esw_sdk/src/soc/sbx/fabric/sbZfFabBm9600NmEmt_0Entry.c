/*
 * $Id: sbZfFabBm9600NmEmt_0Entry.c,v 1.3 Broadcom SDK $
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
#include "sbZfFabBm9600NmEmt_0Entry.hx"
#include "sbWrappers.h"
#include <sal/types.h>



/* Pack from struct into array of bytes */
uint32
sbZfFabBm9600NmEmt_0Entry_Pack(sbZfFabBm9600NmEmt_0Entry_t *pFrom,
                               uint8 *pToData,
                               uint32 nMaxToDataIndex) {
#ifdef SAND_BIG_ENDIAN_HOST
  int i;
  int size = SB_ZF_FAB_BM9600_NMEMT_0ENTRY_SIZE_IN_BYTES;

  if (size % 4) {
    size += (4 - size %4);
  }

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on bigword endian */

  /* Pack Member: m_uMfem */
  (pToData)[10] |= ((pFrom)->m_uMfem & 0x01);

  /* Pack Member: m_uEsetMember1 */
  (pToData)[11] |= ((pFrom)->m_uEsetMember1) & 0xFF;

  /* Pack Member: m_uuEsetMember0 */
  (pToData)[3] |= (COMPILER_64_LO((pFrom)->m_uuEsetMember0)) & 0xFF;
  (pToData)[2] |= (COMPILER_64_LO((pFrom)->m_uuEsetMember0) >> 8) &0xFF;
  (pToData)[1] |= (COMPILER_64_LO((pFrom)->m_uuEsetMember0) >> 16) &0xFF;
  (pToData)[0] |= (COMPILER_64_LO((pFrom)->m_uuEsetMember0) >> 24) &0xFF;
  (pToData)[7] |= (COMPILER_64_HI((pFrom)->m_uuEsetMember0)) & 0xFF;
  (pToData)[6] |= (COMPILER_64_HI((pFrom)->m_uuEsetMember0) >> 8) &0xFF;
  (pToData)[5] |= (COMPILER_64_HI((pFrom)->m_uuEsetMember0) >> 16) &0xFF;
  (pToData)[4] |= (COMPILER_64_HI((pFrom)->m_uuEsetMember0) >> 24) &0xFF;
#else
  int i;
  int size = SB_ZF_FAB_BM9600_NMEMT_0ENTRY_SIZE_IN_BYTES;

  if (size % 4) {
    size += (4 - size %4);
  }

  for ( i=0; i<size; i++ ) {
    (pToData)[i] = 0;
  }
  i = 0;

  /* Pack operation based on little endian */

  /* Pack Member: m_uMfem */
  (pToData)[9] |= ((pFrom)->m_uMfem & 0x01);

  /* Pack Member: m_uEsetMember1 */
  (pToData)[8] |= ((pFrom)->m_uEsetMember1) & 0xFF;

  /* Pack Member: m_uuEsetMember0 */
  (pToData)[0] |= ((pFrom)->m_uuEsetMember0) & 0xFF;
  (pToData)[1] |= ((pFrom)->m_uuEsetMember0 >> 8) &0xFF;
  (pToData)[2] |= ((pFrom)->m_uuEsetMember0 >> 16) &0xFF;
  (pToData)[3] |= ((pFrom)->m_uuEsetMember0 >> 24) &0xFF;
  (pToData)[4] |= ((pFrom)->m_uuEsetMember0 >> 32) &0xFF;
  (pToData)[5] |= ((pFrom)->m_uuEsetMember0 >> 40) &0xFF;
  (pToData)[6] |= ((pFrom)->m_uuEsetMember0 >> 48) &0xFF;
  (pToData)[7] |= ((pFrom)->m_uuEsetMember0 >> 56) &0xFF;
#endif

  return SB_ZF_FAB_BM9600_NMEMT_0ENTRY_SIZE_IN_BYTES;
}




/* Unpack from array of bytes into struct */
void
sbZfFabBm9600NmEmt_0Entry_Unpack(sbZfFabBm9600NmEmt_0Entry_t *pToStruct,
                                 uint8 *pFromData,
                                 uint32 nMaxToDataIndex) {
  COMPILER_UINT64 tmp;

  (void) tmp;

#ifdef SAND_BIG_ENDIAN_HOST

  /* Unpack operation based on bigword endian */

  /* Unpack Member: m_uMfem */
  (pToStruct)->m_uMfem =  (uint32)  ((pFromData)[10] ) & 0x01;

  /* Unpack Member: m_uEsetMember1 */
  (pToStruct)->m_uEsetMember1 =  (uint32)  (pFromData)[11] ;

  /* Unpack Member: m_uuEsetMember0 */
  COMPILER_64_SET((pToStruct)->m_uuEsetMember0, 0,  (unsigned int) (pFromData)[3]);
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[2]);
    COMPILER_64_SHL(tmp, 8);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[1]);
    COMPILER_64_SHL(tmp, 16);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[0]);
    COMPILER_64_SHL(tmp, 24);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[7]);
    COMPILER_64_SHL(tmp, 32);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[6]);
    COMPILER_64_SHL(tmp, 40);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[5]);
    COMPILER_64_SHL(tmp, 48);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[4]);
    COMPILER_64_SHL(tmp, 56);
    COMPILER_64_OR(*tmp0, tmp);
  };
#else

  /* Unpack operation based on little endian */

  /* Unpack Member: m_uMfem */
  (pToStruct)->m_uMfem =  (uint32)  ((pFromData)[9] ) & 0x01;

  /* Unpack Member: m_uEsetMember1 */
  (pToStruct)->m_uEsetMember1 =  (uint32)  (pFromData)[8] ;

  /* Unpack Member: m_uuEsetMember0 */
  COMPILER_64_SET((pToStruct)->m_uuEsetMember0, 0,  (unsigned int) (pFromData)[0]);
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[1]);
    COMPILER_64_SHL(tmp, 8);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[2]);
    COMPILER_64_SHL(tmp, 16);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[3]);
    COMPILER_64_SHL(tmp, 24);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[4]);
    COMPILER_64_SHL(tmp, 32);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[5]);
    COMPILER_64_SHL(tmp, 40);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[6]);
    COMPILER_64_SHL(tmp, 48);
    COMPILER_64_OR(*tmp0, tmp);
  };
  {
    volatile uint64 *tmp0 = &(pToStruct)->m_uuEsetMember0;
    COMPILER_64_SET(tmp, 0,  (unsigned int) (pFromData)[7]);
    COMPILER_64_SHL(tmp, 56);
    COMPILER_64_OR(*tmp0, tmp);
  };
#endif

}



/* initialize an instance of this zframe */
void
sbZfFabBm9600NmEmt_0Entry_InitInstance(sbZfFabBm9600NmEmt_0Entry_t *pFrame) {

  pFrame->m_uMfem =  (unsigned int)  0;
  pFrame->m_uEsetMember1 =  (unsigned int)  0;
  COMPILER_64_ZERO(pFrame->m_uuEsetMember0);

}
