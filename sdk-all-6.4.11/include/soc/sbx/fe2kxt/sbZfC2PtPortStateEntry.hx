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


#ifndef SB_ZF_C2PTPORTSTATEENTRY_H
#define SB_ZF_C2PTPORTSTATEENTRY_H

#define SB_ZF_C2PTPORTSTATEENTRY_SIZE_IN_BYTES 12
#define SB_ZF_C2PTPORTSTATEENTRY_SIZE 12
#define SB_ZF_C2PTPORTSTATEENTRY_M_UPKTINPROGRESS_BITS "94:94"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UMIRROR_BITS "93:93"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UMIRRORSRCDROP_BITS "92:92"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UDOUBLEDEQUEUED_BITS "91:91"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UEOPEXTRABURST_BITS "90:90"
#define SB_ZF_C2PTPORTSTATEENTRY_M_URESIDUEBYTE_BITS "89:86"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UPKTREQUESTSTATE_BITS "85:82"
#define SB_ZF_C2PTPORTSTATEENTRY_M_ULINE_BITS "81:81"
#define SB_ZF_C2PTPORTSTATEENTRY_M_ULASTPAGE_BITS "80:80"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UNEXTPAGESTATE_BITS "79:78"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UNEXTPAGE_BITS "77:65"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UNEXTLENGTH_BITS "64:57"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UPAGE_BITS "56:44"
#define SB_ZF_C2PTPORTSTATEENTRY_M_USAVEDBYTES_BITS "43:40"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UREMAININGBYTES_BITS "39:32"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UEOP_BITS "31:31"
#define SB_ZF_C2PTPORTSTATEENTRY_M_USOP_BITS "30:30"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UPKTLENGTH_BITS "29:16"
#define SB_ZF_C2PTPORTSTATEENTRY_M_UCONTINUEBYTE_BITS "15:8"
#define SB_ZF_C2PTPORTSTATEENTRY_M_USOURCEQUEUE_BITS "7:0"


typedef struct _sbZfC2PtPortStateEntry {
  uint32 m_uPktInProgress;
  uint32 m_uMirror;
  uint32 m_uMirrorSrcDrop;
  uint32 m_uDoubleDequeued;
  uint32 m_uEopExtraBurst;
  uint32 m_uResidueByte;
  uint32 m_uPktRequestState;
  uint32 m_uLine;
  uint32 m_uLastPage;
  uint32 m_uNextPageState;
  uint32 m_uNextPage;
  uint32 m_uNextLength;
  uint32 m_uPage;
  uint32 m_uSavedBytes;
  uint32 m_uRemainingBytes;
  uint32 m_uEop;
  uint32 m_uSop;
  uint32 m_uPktLength;
  uint32 m_uContinueByte;
  uint32 m_uSourceQueue;
} sbZfC2PtPortStateEntry_t;

uint32
sbZfC2PtPortStateEntry_Pack(sbZfC2PtPortStateEntry_t *pFrom,
                            uint8 *pToData,
                            uint32 nMaxToDataIndex);
void
sbZfC2PtPortStateEntry_Unpack(sbZfC2PtPortStateEntry_t *pToStruct,
                              uint8 *pFromData,
                              uint32 nMaxToDataIndex);
void
sbZfC2PtPortStateEntry_InitInstance(sbZfC2PtPortStateEntry_t *pFrame);

#define SB_ZF_C2PTPORTSTATEENTRY_SET_PKTINPROGRESS(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[11] = ((pToData)[11] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_MIRROR(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[11] = ((pToData)[11] & ~(0x01 << 5)) | (((nFromData) & 0x01) << 5); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_MIRRORSRCDROP(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[11] = ((pToData)[11] & ~(0x01 << 4)) | (((nFromData) & 0x01) << 4); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_DOUBLEDEQUEUED(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[11] = ((pToData)[11] & ~(0x01 << 3)) | (((nFromData) & 0x01) << 3); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_EOPEXTRABURST(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[11] = ((pToData)[11] & ~(0x01 << 2)) | (((nFromData) & 0x01) << 2); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_RESIDUEBYTE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[10] = ((pToData)[10] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[11] = ((pToData)[11] & ~ 0x03) | (((nFromData) >> 2) & 0x03); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_PKTREQUESTSTATE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[10] = ((pToData)[10] & ~(0x0f << 2)) | (((nFromData) & 0x0f) << 2); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_LINE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[10] = ((pToData)[10] & ~(0x01 << 1)) | (((nFromData) & 0x01) << 1); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_LASTPAGE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[10] = ((pToData)[10] & ~0x01) | ((nFromData) & 0x01); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_NEXTPAGESTATE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[9] = ((pToData)[9] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_NEXTPAGE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[8] = ((pToData)[8] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
           (pToData)[9] = ((pToData)[9] & ~ 0x3f) | (((nFromData) >> 7) & 0x3f); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_NEXTLENGTH(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[7] = ((pToData)[7] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
           (pToData)[8] = ((pToData)[8] & ~ 0x01) | (((nFromData) >> 7) & 0x01); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_PAGE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~(0x0f << 4)) | (((nFromData) & 0x0f) << 4); \
           (pToData)[6] = ((pToData)[6] & ~0xFF) | (((nFromData) >> 4) & 0xFF); \
           (pToData)[7] = ((pToData)[7] & ~ 0x01) | (((nFromData) >> 12) & 0x01); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_SAVEDBYTES(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[5] = ((pToData)[5] & ~0x0f) | ((nFromData) & 0x0f); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_REMAININGBYTES(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((nFromData)) & 0xFF; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_EOP(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_SOP(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_PKTLENGTH(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((nFromData)) & 0xFF; \
           (pToData)[3] = ((pToData)[3] & ~ 0x3f) | (((nFromData) >> 8) & 0x3f); \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_CONTINUEBYTE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((nFromData)) & 0xFF; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_SET_SOURCEQUEUE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_PKTINPROGRESS(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[11] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_MIRROR(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[11] >> 5) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_MIRRORSRCDROP(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[11] >> 4) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_DOUBLEDEQUEUED(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[11] >> 3) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_EOPEXTRABURST(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[11] >> 2) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_RESIDUEBYTE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[10] >> 6) & 0x03; \
           (nToData) |= (uint32) ((pFromData)[11] & 0x03) << 2; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_PKTREQUESTSTATE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[10] >> 2) & 0x0f; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_LINE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[10] >> 1) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_LASTPAGE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[10]) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_NEXTPAGESTATE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[9] >> 6) & 0x03; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_NEXTPAGE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[8] >> 1) & 0x7f; \
           (nToData) |= (uint32) ((pFromData)[9] & 0x3f) << 7; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_NEXTLENGTH(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[7] >> 1) & 0x7f; \
           (nToData) |= (uint32) ((pFromData)[8] & 0x01) << 7; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_PAGE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[5] >> 4) & 0x0f; \
           (nToData) |= (uint32) (pFromData)[6] << 4; \
           (nToData) |= (uint32) ((pFromData)[7] & 0x01) << 12; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_SAVEDBYTES(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[5]) & 0x0f; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_REMAININGBYTES(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[4] ; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_EOP(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_SOP(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_PKTLENGTH(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[2] ; \
           (nToData) |= (uint32) ((pFromData)[3] & 0x3f) << 8; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_CONTINUEBYTE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[1] ; \
          } while(0)

#define SB_ZF_C2PTPORTSTATEENTRY_GET_SOURCEQUEUE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
          } while(0)

#endif
