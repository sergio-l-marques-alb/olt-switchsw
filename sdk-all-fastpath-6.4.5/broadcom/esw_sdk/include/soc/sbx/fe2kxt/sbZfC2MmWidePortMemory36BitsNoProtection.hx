/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id$
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


#ifndef SB_ZF_ZFC2MMWIDEPORTMEMORY36BITSNOPROTECTION_H
#define SB_ZF_ZFC2MMWIDEPORTMEMORY36BITSNOPROTECTION_H

#define SB_ZF_ZFC2MMWIDEPORTMEMORY36BITSNOPROTECTION_SIZE_IN_BYTES 9
#define SB_ZF_ZFC2MMWIDEPORTMEMORY36BITSNOPROTECTION_SIZE 9
#define SB_ZF_ZFC2MMWIDEPORTMEMORY36BITSNOPROTECTION_M_UDATA1PROTECTIONBITS_BITS "71:68"
#define SB_ZF_ZFC2MMWIDEPORTMEMORY36BITSNOPROTECTION_M_UDATA1_BITS "67:36"
#define SB_ZF_ZFC2MMWIDEPORTMEMORY36BITSNOPROTECTION_M_UDATA0PROTECTIONBITS_BITS "35:32"
#define SB_ZF_ZFC2MMWIDEPORTMEMORY36BITSNOPROTECTION_M_UDATA0_BITS "31:0"


typedef struct _sbZfC2MmWidePortMemory36BitsNoProtection {
  uint32 m_uData1ProtectionBits;
  uint32 m_uData1;
  uint32 m_uData0ProtectionBits;
  uint32 m_uData0;
} sbZfC2MmWidePortMemory36BitsNoProtection_t;

uint32
sbZfC2MmWidePortMemory36BitsNoProtection_Pack(sbZfC2MmWidePortMemory36BitsNoProtection_t *pFrom,
                                              uint8 *pToData,
                                              uint32 nMaxToDataIndex);
void
sbZfC2MmWidePortMemory36BitsNoProtection_Unpack(sbZfC2MmWidePortMemory36BitsNoProtection_t *pToStruct,
                                                uint8 *pFromData,
                                                uint32 nMaxToDataIndex);
void
sbZfC2MmWidePortMemory36BitsNoProtection_InitInstance(sbZfC2MmWidePortMemory36BitsNoProtection_t *pFrame);

#define SB_ZF_C2MMWIDEPORTMEMORY36BITSNOPROTECTION_SET_D1PRT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[8] = ((pToData)[8] & ~(0x0f << 4)) | (((nFromData) & 0x0f) << 4); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORY36BITSNOPROTECTION_SET_DATA1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((pToData)[4] & ~(0x0f << 4)) | (((nFromData) & 0x0f) << 4); \
           (pToData)[5] = ((pToData)[5] & ~0xFF) | (((nFromData) >> 4) & 0xFF); \
           (pToData)[6] = ((pToData)[6] & ~0xFF) | (((nFromData) >> 12) & 0xFF); \
           (pToData)[7] = ((pToData)[7] & ~0xFF) | (((nFromData) >> 20) & 0xFF); \
           (pToData)[8] = ((pToData)[8] & ~ 0x0f) | (((nFromData) >> 28) & 0x0f); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORY36BITSNOPROTECTION_SET_D0PRT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((pToData)[4] & ~0x0f) | ((nFromData) & 0x0f); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORY36BITSNOPROTECTION_SET_DATA0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[3] = ((pToData)[3] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORY36BITSNOPROTECTION_GET_D1PRT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[8] >> 4) & 0x0f; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORY36BITSNOPROTECTION_GET_DATA1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[4] >> 4) & 0x0f; \
           (nToData) |= (uint32) (pFromData)[5] << 4; \
           (nToData) |= (uint32) (pFromData)[6] << 12; \
           (nToData) |= (uint32) (pFromData)[7] << 20; \
           (nToData) |= (uint32) ((pFromData)[8] & 0x0f) << 28; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORY36BITSNOPROTECTION_GET_D0PRT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[4]) & 0x0f; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORY36BITSNOPROTECTION_GET_DATA0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) (pFromData)[1] << 8; \
           (nToData) |= (uint32) (pFromData)[2] << 16; \
           (nToData) |= (uint32) (pFromData)[3] << 24; \
          } while(0)

#endif
