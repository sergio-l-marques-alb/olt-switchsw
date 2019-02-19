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


#ifndef SB_ZF_ZFC2PRTAGINFO_H
#define SB_ZF_ZFC2PRTAGINFO_H

#define SB_ZF_ZFC2PRTAGINFO_SIZE_IN_BYTES 6
#define SB_ZF_ZFC2PRTAGINFO_SIZE 6
#define SB_ZF_ZFC2PRTAGINFO_M_BACTIVE_BITS "47:47"
#define SB_ZF_ZFC2PRTAGINFO_M_BTESTPKT_BITS "46:46"
#define SB_ZF_ZFC2PRTAGINFO_M_NPORT_BITS "45:40"
#define SB_ZF_ZFC2PRTAGINFO_M_BENDOFPKT_BITS "39:39"
#define SB_ZF_ZFC2PRTAGINFO_M_NENQUEUECOUNT_BITS "38:37"
#define SB_ZF_ZFC2PRTAGINFO_M_NLINECOUNT_BITS "36:34"
#define SB_ZF_ZFC2PRTAGINFO_M_NNUMPAGES_BITS "33:27"
#define SB_ZF_ZFC2PRTAGINFO_M_NBUFFER_BITS "26:15"
#define SB_ZF_ZFC2PRTAGINFO_M_NLENGTH_BITS "14:1"
#define SB_ZF_ZFC2PRTAGINFO_M_BDROPPKT_BITS "0:0"


typedef struct _sbZfC2PrTagInfo {
  uint32 m_bActive;
  uint32 m_bTestPkt;
  uint32 m_nPort;
  uint32 m_bEndOfPkt;
  uint32 m_nEnqueueCount;
  uint32 m_nLineCount;
  uint32 m_nNumPages;
  uint32 m_nBuffer;
  uint32 m_nLength;
  uint32 m_bDropPkt;
} sbZfC2PrTagInfo_t;

uint32
sbZfC2PrTagInfo_Pack(sbZfC2PrTagInfo_t *pFrom,
                     uint8 *pToData,
                     uint32 nMaxToDataIndex);
void
sbZfC2PrTagInfo_Unpack(sbZfC2PrTagInfo_t *pToStruct,
                       uint8 *pFromData,
                       uint32 nMaxToDataIndex);
void
sbZfC2PrTagInfo_InitInstance(sbZfC2PrTagInfo_t *pFrame);

#define SB_ZF_C2PRTAGINFO_SET_ACTIVE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_TESTPKT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_PORT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~0x3f) | ((nFromData) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_EOP(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((pToData)[4] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_ENQUEUES(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((pToData)[4] & ~(0x03 << 5)) | (((nFromData) & 0x03) << 5); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_LINES(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((pToData)[4] & ~(0x07 << 2)) | (((nFromData) & 0x07) << 2); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_NPAGES(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x1f << 3)) | (((nFromData) & 0x1f) << 3); \
           (pToData)[4] = ((pToData)[4] & ~ 0x03) | (((nFromData) >> 5) & 0x03); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_BUFFER(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 1) & 0xFF); \
           (pToData)[3] = ((pToData)[3] & ~ 0x07) | (((nFromData) >> 9) & 0x07); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_LENGTH(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
           (pToData)[1] = ((pToData)[1] & ~ 0x7f) | (((nFromData) >> 7) & 0x7f); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_SET_DROP(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~0x01) | ((nFromData) & 0x01); \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_ACTIVE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[5] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_TESTPKT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[5] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_PORT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[5]) & 0x3f; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_EOP(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[4] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_ENQUEUES(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[4] >> 5) & 0x03; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_LINES(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[4] >> 2) & 0x07; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_NPAGES(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3] >> 3) & 0x1f; \
           (nToData) |= (uint32) ((pFromData)[4] & 0x03) << 5; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_BUFFER(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 7) & 0x01; \
           (nToData) |= (uint32) (pFromData)[2] << 1; \
           (nToData) |= (uint32) ((pFromData)[3] & 0x07) << 9; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_LENGTH(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 1) & 0x7f; \
           (nToData) |= (uint32) ((pFromData)[1] & 0x7f) << 7; \
          } while(0)

#define SB_ZF_C2PRTAGINFO_GET_DROP(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0]) & 0x01; \
          } while(0)

#endif
