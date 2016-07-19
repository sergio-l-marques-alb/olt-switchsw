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


#ifndef SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_H
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_H

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SIZE_IN_BYTES 8
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SIZE 8
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_M_URESV0_BITS "63:57"
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_M_UWRITEACCESS_BITS "56:55"
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_M_UREADACCESS_BITS "54:53"
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_M_UDATAINTEGRITY_BITS "52:50"
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_M_UTRAPNULLPOINTER_BITS "47:47"
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_M_UWRAP_BITS "46:46"
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_M_ULIMITMASK_BITS "45:23"
#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_M_UBASEADDRESS_BITS "22:0"


typedef struct _sbZfC2MmWidePortMemorySegmentConfigData {
  uint32 m_uResv0;
  uint32 m_uWriteAccess;
  uint32 m_uReadAccess;
  uint32 m_uDataIntegrity;
  uint32 m_uTrapNullPointer;
  uint32 m_uWrap;
  uint32 m_uLimitMask;
  uint32 m_uBaseAddress;
} sbZfC2MmWidePortMemorySegmentConfigData_t;

uint32
sbZfC2MmWidePortMemorySegmentConfigData_Pack(sbZfC2MmWidePortMemorySegmentConfigData_t *pFrom,
                                             uint8 *pToData,
                                             uint32 nMaxToDataIndex);
void
sbZfC2MmWidePortMemorySegmentConfigData_Unpack(sbZfC2MmWidePortMemorySegmentConfigData_t *pToStruct,
                                               uint8 *pFromData,
                                               uint32 nMaxToDataIndex);
void
sbZfC2MmWidePortMemorySegmentConfigData_InitInstance(sbZfC2MmWidePortMemorySegmentConfigData_t *pFrame);

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SET_RESV0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[7] = ((pToData)[7] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SET_WACC(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[6] = ((pToData)[6] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[7] = ((pToData)[7] & ~ 0x01) | (((nFromData) >> 1) & 0x01); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SET_RACC(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[6] = ((pToData)[6] & ~(0x03 << 5)) | (((nFromData) & 0x03) << 5); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SET_DING(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[6] = ((pToData)[6] & ~(0x07 << 2)) | (((nFromData) & 0x07) << 2); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SET_TRAP(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SET_WRAP(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SET_LMASK(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[3] = ((pToData)[3] & ~0xFF) | (((nFromData) >> 1) & 0xFF); \
           (pToData)[4] = ((pToData)[4] & ~0xFF) | (((nFromData) >> 9) & 0xFF); \
           (pToData)[5] = ((pToData)[5] & ~ 0x3f) | (((nFromData) >> 17) & 0x3f); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_SET_BADDR(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[2] = ((pToData)[2] & ~ 0x7f) | (((nFromData) >> 16) & 0x7f); \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_GET_RESV0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[7] >> 1) & 0x7f; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_GET_WACC(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[6] >> 7) & 0x01; \
           (nToData) |= (uint32) ((pFromData)[7] & 0x01) << 1; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_GET_RACC(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[6] >> 5) & 0x03; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_GET_DING(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[6] >> 2) & 0x07; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_GET_TRAP(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[5] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_GET_WRAP(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[5] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_GET_LMASK(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 7) & 0x01; \
           (nToData) |= (uint32) (pFromData)[3] << 1; \
           (nToData) |= (uint32) (pFromData)[4] << 9; \
           (nToData) |= (uint32) ((pFromData)[5] & 0x3f) << 17; \
          } while(0)

#define SB_ZF_C2MMWIDEPORTMEMORYSEGMENTCONFIGDATA_GET_BADDR(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) (pFromData)[1] << 8; \
           (nToData) |= (uint32) ((pFromData)[2] & 0x7f) << 16; \
          } while(0)

#endif
