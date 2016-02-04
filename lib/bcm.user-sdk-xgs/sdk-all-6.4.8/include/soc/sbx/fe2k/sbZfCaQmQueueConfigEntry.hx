/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id: sbZfCaQmQueueConfigEntry.hx,v 1.4 Broadcom SDK $
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


#ifndef SB_ZF_CAQMQUEUECONFIGENTRY_H
#define SB_ZF_CAQMQUEUECONFIGENTRY_H

#define SB_ZF_CAQMQUEUECONFIGENTRY_SIZE_IN_BYTES 10
#define SB_ZF_CAQMQUEUECONFIGENTRY_SIZE 10
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UENFORCEPOLICEMARKINGS_BITS "78:78"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UDROPTHRESH2_BITS "77:67"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UDROPTHRESH1_BITS "66:56"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UFLOWCONTROLTHRESH2_BITS "55:45"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UFLOWCONTROLTHRESH1_BITS "44:34"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UFLOWCONTROLENABLE_BITS "33:33"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UFLOWCONTROLTARGET_BITS "32:32"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UFLOWCONTROLPORT_BITS "31:23"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UMAXPAGES_BITS "22:12"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UMINPAGES_BITS "11:1"
#define SB_ZF_CAQMQUEUECONFIGENTRY_M_UENABLE_BITS "0:0"


typedef struct _sbZfCaQmQueueConfigEntry {
  uint32 m_uEnforcePoliceMarkings;
  uint32 m_uDropThresh2;
  uint32 m_uDropThresh1;
  uint32 m_uFlowControlThresh2;
  uint32 m_uFlowControlThresh1;
  uint32 m_uFlowControlEnable;
  uint32 m_uFlowControlTarget;
  uint32 m_uFlowControlPort;
  uint32 m_uMaxPages;
  uint32 m_uMinPages;
  uint32 m_uEnable;
} sbZfCaQmQueueConfigEntry_t;

uint32
sbZfCaQmQueueConfigEntry_Pack(sbZfCaQmQueueConfigEntry_t *pFrom,
                              uint8 *pToData,
                              uint32 nMaxToDataIndex);
void
sbZfCaQmQueueConfigEntry_Unpack(sbZfCaQmQueueConfigEntry_t *pToStruct,
                                uint8 *pFromData,
                                uint32 nMaxToDataIndex);
void
sbZfCaQmQueueConfigEntry_InitInstance(sbZfCaQmQueueConfigEntry_t *pFrame);

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_ENFORCEPOLICEMARKINGS(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[9] = ((pToData)[9] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_DROPTHRESH2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[8] = ((pToData)[8] & ~(0x1f << 3)) | (((nFromData) & 0x1f) << 3); \
           (pToData)[9] = ((pToData)[9] & ~ 0x3f) | (((nFromData) >> 5) & 0x3f); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_DROPTHRESH1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[7] = ((nFromData)) & 0xFF; \
           (pToData)[8] = ((pToData)[8] & ~ 0x07) | (((nFromData) >> 8) & 0x07); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_FLOWCONTROLTHRESH2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~(0x07 << 5)) | (((nFromData) & 0x07) << 5); \
           (pToData)[6] = ((pToData)[6] & ~0xFF) | (((nFromData) >> 3) & 0xFF); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_FLOWCONTROLTHRESH1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((pToData)[4] & ~(0x3f << 2)) | (((nFromData) & 0x3f) << 2); \
           (pToData)[5] = ((pToData)[5] & ~ 0x1f) | (((nFromData) >> 6) & 0x1f); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_FLOWCONTROLENABLE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((pToData)[4] & ~(0x01 << 1)) | (((nFromData) & 0x01) << 1); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_FLOWCONTROLTARGET(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((pToData)[4] & ~0x01) | ((nFromData) & 0x01); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_FLOWCONTROLPORT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[3] = ((pToData)[3] & ~0xFF) | (((nFromData) >> 1) & 0xFF); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_MAXPAGES(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x0f << 4)) | (((nFromData) & 0x0f) << 4); \
           (pToData)[2] = ((pToData)[2] & ~ 0x7f) | (((nFromData) >> 4) & 0x7f); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_MINPAGES(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
           (pToData)[1] = ((pToData)[1] & ~ 0x0f) | (((nFromData) >> 7) & 0x0f); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_SET_ENABLE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~0x01) | ((nFromData) & 0x01); \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_ENFORCEPOLICEMARKINGS(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[9] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_DROPTHRESH2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[8] >> 3) & 0x1f; \
           (nToData) |= (uint32) ((pFromData)[9] & 0x3f) << 5; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_DROPTHRESH1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[7] ; \
           (nToData) |= (uint32) ((pFromData)[8] & 0x07) << 8; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_FLOWCONTROLTHRESH2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[5] >> 5) & 0x07; \
           (nToData) |= (uint32) (pFromData)[6] << 3; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_FLOWCONTROLTHRESH1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[4] >> 2) & 0x3f; \
           (nToData) |= (uint32) ((pFromData)[5] & 0x1f) << 6; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_FLOWCONTROLENABLE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[4] >> 1) & 0x01; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_FLOWCONTROLTARGET(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[4]) & 0x01; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_FLOWCONTROLPORT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 7) & 0x01; \
           (nToData) |= (uint32) (pFromData)[3] << 1; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_MAXPAGES(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 4) & 0x0f; \
           (nToData) |= (uint32) ((pFromData)[2] & 0x7f) << 4; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_MINPAGES(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 1) & 0x7f; \
           (nToData) |= (uint32) ((pFromData)[1] & 0x0f) << 7; \
          } while(0)

#define SB_ZF_CAQMQUEUECONFIGENTRY_GET_ENABLE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0]) & 0x01; \
          } while(0)

#endif
