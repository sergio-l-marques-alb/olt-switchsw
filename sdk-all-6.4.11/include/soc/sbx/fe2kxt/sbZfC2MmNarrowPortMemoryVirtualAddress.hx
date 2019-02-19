/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id$
 * $Copyright: Copyright 2016 Broadcom Corporation.
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


#ifndef SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_H
#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_H

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_SIZE_IN_BYTES 4
#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_SIZE 4
#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_M_URESV0_BITS "31:29"
#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_M_UPHYSICALADDRESS_BITS "27:27"
#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_M_USEGMENT_BITS "26:23"
#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_M_UADDRESS_BITS "22:0"


typedef struct _sbZfC2MmNarrowPortMemoryVirtualAddress {
  uint32 m_uResv0;
  uint32 m_uPhysicalAddress;
  uint32 m_uSegment;
  uint32 m_uAddress;
} sbZfC2MmNarrowPortMemoryVirtualAddress_t;

uint32
sbZfC2MmNarrowPortMemoryVirtualAddress_Pack(sbZfC2MmNarrowPortMemoryVirtualAddress_t *pFrom,
                                            uint8 *pToData,
                                            uint32 nMaxToDataIndex);
void
sbZfC2MmNarrowPortMemoryVirtualAddress_Unpack(sbZfC2MmNarrowPortMemoryVirtualAddress_t *pToStruct,
                                              uint8 *pFromData,
                                              uint32 nMaxToDataIndex);
void
sbZfC2MmNarrowPortMemoryVirtualAddress_InitInstance(sbZfC2MmNarrowPortMemoryVirtualAddress_t *pFrame);

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_SET_RESV0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x07 << 5)) | (((nFromData) & 0x07) << 5); \
          } while(0)

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_SET_PADDR(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x01 << 3)) | (((nFromData) & 0x01) << 3); \
          } while(0)

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_SET_SEGMENT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[3] = ((pToData)[3] & ~ 0x07) | (((nFromData) >> 1) & 0x07); \
          } while(0)

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_SET_ADDR(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[2] = ((pToData)[2] & ~ 0x7f) | (((nFromData) >> 16) & 0x7f); \
          } while(0)

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_GET_RESV0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3] >> 5) & 0x07; \
          } while(0)

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_GET_PADDR(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3] >> 3) & 0x01; \
          } while(0)

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_GET_SEGMENT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 7) & 0x01; \
           (nToData) |= (uint32) ((pFromData)[3] & 0x07) << 1; \
          } while(0)

#define SB_ZF_C2MMNARROWPORTMEMORYVIRTUALADDRESS_GET_ADDR(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) (pFromData)[1] << 8; \
           (nToData) |= (uint32) ((pFromData)[2] & 0x7f) << 16; \
          } while(0)

#endif
