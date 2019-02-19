/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id: sbZfCaPpCamStateRawFormat.hx,v 1.4 Broadcom SDK $
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


#ifndef SB_ZF_CAPPCAMSTATERAWFORMAT_H
#define SB_ZF_CAPPCAMSTATERAWFORMAT_H

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SIZE_IN_BYTES 4
#define SB_ZF_CAPPCAMSTATERAWFORMAT_SIZE 4
#define SB_ZF_CAPPCAMSTATERAWFORMAT_M_URESV0_BITS "31:24"
#define SB_ZF_CAPPCAMSTATERAWFORMAT_M_UUNMODIFIEDSTATE_BITS "23:8"
#define SB_ZF_CAPPCAMSTATERAWFORMAT_M_UPROPBITS4_BITS "7:7"
#define SB_ZF_CAPPCAMSTATERAWFORMAT_M_UPROPBITS3_BITS "6:6"
#define SB_ZF_CAPPCAMSTATERAWFORMAT_M_UPROPBITS2_BITS "5:5"
#define SB_ZF_CAPPCAMSTATERAWFORMAT_M_UPROPBITS1_BITS "4:4"
#define SB_ZF_CAPPCAMSTATERAWFORMAT_M_UPROPBITS0_BITS "3:3"
#define SB_ZF_CAPPCAMSTATERAWFORMAT_M_USTREAMSTATE_BITS "2:0"


typedef struct _sbZfCaPpCamStateRawFormat {
  uint32 m_uResv0;
  uint32 m_uUnmodifiedState;
  uint32 m_uPropBits4;
  uint32 m_uPropBits3;
  uint32 m_uPropBits2;
  uint32 m_uPropBits1;
  uint32 m_uPropBits0;
  uint32 m_uStreamState;
} sbZfCaPpCamStateRawFormat_t;

uint32
sbZfCaPpCamStateRawFormat_Pack(sbZfCaPpCamStateRawFormat_t *pFrom,
                               uint8 *pToData,
                               uint32 nMaxToDataIndex);
void
sbZfCaPpCamStateRawFormat_Unpack(sbZfCaPpCamStateRawFormat_t *pToStruct,
                                 uint8 *pFromData,
                                 uint32 nMaxToDataIndex);
void
sbZfCaPpCamStateRawFormat_InitInstance(sbZfCaPpCamStateRawFormat_t *pFrame);

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SET_RESV0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((nFromData)) & 0xFF; \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SET_STATE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((nFromData)) & 0xFF; \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SET_PBITS4(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SET_PBITS3(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SET_PBITS2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x01 << 5)) | (((nFromData) & 0x01) << 5); \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SET_PBITS1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x01 << 4)) | (((nFromData) & 0x01) << 4); \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SET_PBITS0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x01 << 3)) | (((nFromData) & 0x01) << 3); \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_SET_SSL(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~0x07) | ((nFromData) & 0x07); \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_GET_RESV0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[3] ; \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_GET_STATE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[1] ; \
           (nToData) |= (uint32) (pFromData)[2] << 8; \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_GET_PBITS4(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_GET_PBITS3(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_GET_PBITS2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 5) & 0x01; \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_GET_PBITS1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 4) & 0x01; \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_GET_PBITS0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 3) & 0x01; \
          } while(0)

#define SB_ZF_CAPPCAMSTATERAWFORMAT_GET_SSL(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0]) & 0x07; \
          } while(0)

#endif
