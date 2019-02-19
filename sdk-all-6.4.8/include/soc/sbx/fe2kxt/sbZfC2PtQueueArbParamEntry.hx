/*  -*-  Mode:C; c-basic-offset:4 -*- */
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


#ifndef SB_ZF_C2PTQUEUEARBPARAMENTRY_H
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_H

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SIZE_IN_BYTES 8
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SIZE 8
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_UPPEENQUEUE_BITS "63:63"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_USPARE0_BITS "62:55"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_UWEIGHT_BITS "54:48"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_URXPORT_BITS "47:40"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_UQUEUE_BITS "39:32"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_UPBSELECT_BITS "31:31"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_USPARE1_BITS "30:20"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_UDSTINTERFACE_BITS "19:16"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_USPACE2_BITS "15:14"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_UDSTPORT_BITS "13:8"
#define SB_ZF_C2PTQUEUEARBPARAMENTRY_M_UDSTQUEUE_BITS "7:0"


typedef struct _sbZfC2PtQueueArbParamEntry {
  uint32 m_uPpeEnqueue;
  uint32 m_uSpare0;
  uint32 m_uWeight;
  uint32 m_uRxPort;
  uint32 m_uQueue;
  uint32 m_uPbSelect;
  uint32 m_uSpare1;
  uint32 m_uDstInterface;
  uint32 m_uSpace2;
  uint32 m_uDstPort;
  uint32 m_uDstQueue;
} sbZfC2PtQueueArbParamEntry_t;

uint32
sbZfC2PtQueueArbParamEntry_Pack(sbZfC2PtQueueArbParamEntry_t *pFrom,
                                uint8 *pToData,
                                uint32 nMaxToDataIndex);
void
sbZfC2PtQueueArbParamEntry_Unpack(sbZfC2PtQueueArbParamEntry_t *pToStruct,
                                  uint8 *pFromData,
                                  uint32 nMaxToDataIndex);
void
sbZfC2PtQueueArbParamEntry_InitInstance(sbZfC2PtQueueArbParamEntry_t *pFrame);

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_PPEENQUEUE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[7] = ((pToData)[7] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_SPARE0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[6] = ((pToData)[6] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[7] = ((pToData)[7] & ~ 0x7f) | (((nFromData) >> 1) & 0x7f); \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_WEIGHT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[6] = ((pToData)[6] & ~0x7f) | ((nFromData) & 0x7f); \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_RXPORT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((nFromData)) & 0xFF; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_QUEUE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((nFromData)) & 0xFF; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_PBSELECT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_SPARE1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x0f << 4)) | (((nFromData) & 0x0f) << 4); \
           (pToData)[3] = ((pToData)[3] & ~ 0x7f) | (((nFromData) >> 4) & 0x7f); \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_DSTINTERFACE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~0x0f) | ((nFromData) & 0x0f); \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_SPACE2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_DSTPORT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~0x3f) | ((nFromData) & 0x3f); \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_SET_DSTQUEUE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_PPEENQUEUE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[7] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_SPARE0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[6] >> 7) & 0x01; \
           (nToData) |= (uint32) ((pFromData)[7] & 0x7f) << 1; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_WEIGHT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[6]) & 0x7f; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_RXPORT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[5] ; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_QUEUE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[4] ; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_PBSELECT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_SPARE1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 4) & 0x0f; \
           (nToData) |= (uint32) ((pFromData)[3] & 0x7f) << 4; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_DSTINTERFACE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2]) & 0x0f; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_SPACE2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 6) & 0x03; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_DSTPORT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1]) & 0x3f; \
          } while(0)

#define SB_ZF_C2PTQUEUEARBPARAMENTRY_GET_DSTQUEUE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
          } while(0)

#endif
