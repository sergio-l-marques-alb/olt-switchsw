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


#ifndef SB_ZF_C2UCODEACCESSDESCRIPTOR_H
#define SB_ZF_C2UCODEACCESSDESCRIPTOR_H

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_SIZE_IN_BYTES 7
#define SB_ZF_C2UCODEACCESSDESCRIPTOR_SIZE 7
#define SB_ZF_C2UCODEACCESSDESCRIPTOR_M_NRWN_BITS "50:50"
#define SB_ZF_C2UCODEACCESSDESCRIPTOR_M_NPORT_BITS "49:45"
#define SB_ZF_C2UCODEACCESSDESCRIPTOR_M_NADDR_BITS "41:14"
#define SB_ZF_C2UCODEACCESSDESCRIPTOR_M_NDATA_BITS "13:0"


typedef struct _sbZfC2UcodeAccessDescriptor {
  int32_t m_nRWN;
  int32_t m_nPort;
  int32_t m_nAddr;
  int32_t m_nData;
} sbZfC2UcodeAccessDescriptor_t;

uint32
sbZfC2UcodeAccessDescriptor_Pack(sbZfC2UcodeAccessDescriptor_t *pFrom,
                                 uint8 *pToData,
                                 uint32 nMaxToDataIndex);
void
sbZfC2UcodeAccessDescriptor_Unpack(sbZfC2UcodeAccessDescriptor_t *pToStruct,
                                   uint8 *pFromData,
                                   uint32 nMaxToDataIndex);
void
sbZfC2UcodeAccessDescriptor_InitInstance(sbZfC2UcodeAccessDescriptor_t *pFrame);

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_SET_RWN(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[6] = ((pToData)[6] & ~(0x01 << 2)) | (((nFromData) & 0x01) << 2); \
          } while(0)

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_SET_PORT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~(0x07 << 5)) | (((nFromData) & 0x07) << 5); \
           (pToData)[6] = ((pToData)[6] & ~ 0x03) | (((nFromData) >> 3) & 0x03); \
          } while(0)

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_SET_ADDR(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 2) & 0xFF); \
           (pToData)[3] = ((pToData)[3] & ~0xFF) | (((nFromData) >> 10) & 0xFF); \
           (pToData)[4] = ((pToData)[4] & ~0xFF) | (((nFromData) >> 18) & 0xFF); \
           (pToData)[5] = ((pToData)[5] & ~ 0x03) | (((nFromData) >> 26) & 0x03); \
          } while(0)

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_SET_DATA(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~ 0x3f) | (((nFromData) >> 8) & 0x3f); \
          } while(0)

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_GET_RWN(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (int32_t) ((pFromData)[6] >> 2) & 0x01; \
          } while(0)

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_GET_PORT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (int32_t) ((pFromData)[5] >> 5) & 0x07; \
           (nToData) |= (int32_t) ((pFromData)[6] & 0x03) << 3; \
          } while(0)

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_GET_ADDR(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (int32_t) ((pFromData)[1] >> 6) & 0x03; \
           (nToData) |= (int32_t) (pFromData)[2] << 2; \
           (nToData) |= (int32_t) (pFromData)[3] << 10; \
           (nToData) |= (int32_t) (pFromData)[4] << 18; \
           (nToData) |= (int32_t) ((pFromData)[5] & 0x03) << 26; \
          } while(0)

#define SB_ZF_C2UCODEACCESSDESCRIPTOR_GET_DATA(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (int32_t) (pFromData)[0] ; \
           (nToData) |= (int32_t) ((pFromData)[1] & 0x3f) << 8; \
          } while(0)

#endif
