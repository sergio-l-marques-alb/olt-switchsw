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


#ifndef SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_H
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_H

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SIZE_IN_BYTES 54
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SIZE 54
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASK0_31_BITS "429:398"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASK32_63_BITS "397:366"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASK64_95_BITS "365:334"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASK96_127_BITS "333:302"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASK128_159_BITS "301:270"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASK160_191_BITS "269:238"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASK192_199_BITS "237:230"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASKRESERVED_BITS "229:221"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UMASKPORT_BITS "220:216"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UPACKET0_31_BITS "215:184"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UPACKET32_63_BITS "183:152"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UPACKET64_95_BITS "151:120"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UPACKET96_127_BITS "119:88"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UPACKET128_159_BITS "87:56"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UPACKET160_191_BITS "55:24"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UPACKET192_199_BITS "23:16"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_URESERVED_BITS "15:7"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UPORT_BITS "6:2"
#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_M_UVALID_BITS "1:0"


typedef struct _sbZfC2PrCcTcamConfigEntryFirst {
  uint32 m_uMask0_31;
  uint32 m_uMask32_63;
  uint32 m_uMask64_95;
  uint32 m_uMask96_127;
  uint32 m_uMask128_159;
  uint32 m_uMask160_191;
  uint32 m_uMask192_199;
  uint32 m_uMaskReserved;
  uint32 m_uMaskPort;
  uint32 m_uPacket0_31;
  uint32 m_uPacket32_63;
  uint32 m_uPacket64_95;
  uint32 m_uPacket96_127;
  uint32 m_uPacket128_159;
  uint32 m_uPacket160_191;
  uint32 m_uPacket192_199;
  uint32 m_uReserved;
  uint32 m_uPort;
  uint32 m_uValid;
} sbZfC2PrCcTcamConfigEntryFirst_t;

uint32
sbZfC2PrCcTcamConfigEntryFirst_Pack(sbZfC2PrCcTcamConfigEntryFirst_t *pFrom,
                                    uint8 *pToData,
                                    uint32 nMaxToDataIndex);
void
sbZfC2PrCcTcamConfigEntryFirst_Unpack(sbZfC2PrCcTcamConfigEntryFirst_t *pToStruct,
                                      uint8 *pFromData,
                                      uint32 nMaxToDataIndex);
void
sbZfC2PrCcTcamConfigEntryFirst_InitInstance(sbZfC2PrCcTcamConfigEntryFirst_t *pFrame);

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASK0_31(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[49] = ((pToData)[49] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[50] = ((pToData)[50] & ~0xFF) | (((nFromData) >> 2) & 0xFF); \
           (pToData)[51] = ((pToData)[51] & ~0xFF) | (((nFromData) >> 10) & 0xFF); \
           (pToData)[52] = ((pToData)[52] & ~0xFF) | (((nFromData) >> 18) & 0xFF); \
           (pToData)[53] = ((pToData)[53] & ~ 0x3f) | (((nFromData) >> 26) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASK32_63(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[45] = ((pToData)[45] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[46] = ((pToData)[46] & ~0xFF) | (((nFromData) >> 2) & 0xFF); \
           (pToData)[47] = ((pToData)[47] & ~0xFF) | (((nFromData) >> 10) & 0xFF); \
           (pToData)[48] = ((pToData)[48] & ~0xFF) | (((nFromData) >> 18) & 0xFF); \
           (pToData)[49] = ((pToData)[49] & ~ 0x3f) | (((nFromData) >> 26) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASK64_95(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[41] = ((pToData)[41] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[42] = ((pToData)[42] & ~0xFF) | (((nFromData) >> 2) & 0xFF); \
           (pToData)[43] = ((pToData)[43] & ~0xFF) | (((nFromData) >> 10) & 0xFF); \
           (pToData)[44] = ((pToData)[44] & ~0xFF) | (((nFromData) >> 18) & 0xFF); \
           (pToData)[45] = ((pToData)[45] & ~ 0x3f) | (((nFromData) >> 26) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASK96_127(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[37] = ((pToData)[37] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[38] = ((pToData)[38] & ~0xFF) | (((nFromData) >> 2) & 0xFF); \
           (pToData)[39] = ((pToData)[39] & ~0xFF) | (((nFromData) >> 10) & 0xFF); \
           (pToData)[40] = ((pToData)[40] & ~0xFF) | (((nFromData) >> 18) & 0xFF); \
           (pToData)[41] = ((pToData)[41] & ~ 0x3f) | (((nFromData) >> 26) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASK128_159(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[33] = ((pToData)[33] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[34] = ((pToData)[34] & ~0xFF) | (((nFromData) >> 2) & 0xFF); \
           (pToData)[35] = ((pToData)[35] & ~0xFF) | (((nFromData) >> 10) & 0xFF); \
           (pToData)[36] = ((pToData)[36] & ~0xFF) | (((nFromData) >> 18) & 0xFF); \
           (pToData)[37] = ((pToData)[37] & ~ 0x3f) | (((nFromData) >> 26) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASK160_191(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[29] = ((pToData)[29] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[30] = ((pToData)[30] & ~0xFF) | (((nFromData) >> 2) & 0xFF); \
           (pToData)[31] = ((pToData)[31] & ~0xFF) | (((nFromData) >> 10) & 0xFF); \
           (pToData)[32] = ((pToData)[32] & ~0xFF) | (((nFromData) >> 18) & 0xFF); \
           (pToData)[33] = ((pToData)[33] & ~ 0x3f) | (((nFromData) >> 26) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASK192_199(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[28] = ((pToData)[28] & ~(0x03 << 6)) | (((nFromData) & 0x03) << 6); \
           (pToData)[29] = ((pToData)[29] & ~ 0x3f) | (((nFromData) >> 2) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASKRESERVED(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[27] = ((pToData)[27] & ~(0x07 << 5)) | (((nFromData) & 0x07) << 5); \
           (pToData)[28] = ((pToData)[28] & ~ 0x3f) | (((nFromData) >> 3) & 0x3f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_MASKPORT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[27] = ((pToData)[27] & ~0x1f) | ((nFromData) & 0x1f); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_PACKET0_31(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[23] = ((nFromData)) & 0xFF; \
           (pToData)[24] = ((pToData)[24] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[25] = ((pToData)[25] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[26] = ((pToData)[26] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_PACKET32_63(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[19] = ((nFromData)) & 0xFF; \
           (pToData)[20] = ((pToData)[20] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[21] = ((pToData)[21] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[22] = ((pToData)[22] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_PACKET64_95(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[15] = ((nFromData)) & 0xFF; \
           (pToData)[16] = ((pToData)[16] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[17] = ((pToData)[17] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[18] = ((pToData)[18] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_PACKET96_127(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[11] = ((nFromData)) & 0xFF; \
           (pToData)[12] = ((pToData)[12] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[13] = ((pToData)[13] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[14] = ((pToData)[14] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_PACKET128_159(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[7] = ((nFromData)) & 0xFF; \
           (pToData)[8] = ((pToData)[8] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[9] = ((pToData)[9] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[10] = ((pToData)[10] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_PACKET160_191(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((nFromData)) & 0xFF; \
           (pToData)[4] = ((pToData)[4] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[5] = ((pToData)[5] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[6] = ((pToData)[6] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_PACKET192_199(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((nFromData)) & 0xFF; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_RESERVED(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 1) & 0xFF); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_PORT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x1f << 2)) | (((nFromData) & 0x1f) << 2); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_SET_VALID(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~0x03) | ((nFromData) & 0x03); \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASK0_31(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[49] >> 6) & 0x03; \
           (nToData) |= (uint32) (pFromData)[50] << 2; \
           (nToData) |= (uint32) (pFromData)[51] << 10; \
           (nToData) |= (uint32) (pFromData)[52] << 18; \
           (nToData) |= (uint32) ((pFromData)[53] & 0x3f) << 26; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASK32_63(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[45] >> 6) & 0x03; \
           (nToData) |= (uint32) (pFromData)[46] << 2; \
           (nToData) |= (uint32) (pFromData)[47] << 10; \
           (nToData) |= (uint32) (pFromData)[48] << 18; \
           (nToData) |= (uint32) ((pFromData)[49] & 0x3f) << 26; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASK64_95(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[41] >> 6) & 0x03; \
           (nToData) |= (uint32) (pFromData)[42] << 2; \
           (nToData) |= (uint32) (pFromData)[43] << 10; \
           (nToData) |= (uint32) (pFromData)[44] << 18; \
           (nToData) |= (uint32) ((pFromData)[45] & 0x3f) << 26; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASK96_127(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[37] >> 6) & 0x03; \
           (nToData) |= (uint32) (pFromData)[38] << 2; \
           (nToData) |= (uint32) (pFromData)[39] << 10; \
           (nToData) |= (uint32) (pFromData)[40] << 18; \
           (nToData) |= (uint32) ((pFromData)[41] & 0x3f) << 26; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASK128_159(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[33] >> 6) & 0x03; \
           (nToData) |= (uint32) (pFromData)[34] << 2; \
           (nToData) |= (uint32) (pFromData)[35] << 10; \
           (nToData) |= (uint32) (pFromData)[36] << 18; \
           (nToData) |= (uint32) ((pFromData)[37] & 0x3f) << 26; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASK160_191(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[29] >> 6) & 0x03; \
           (nToData) |= (uint32) (pFromData)[30] << 2; \
           (nToData) |= (uint32) (pFromData)[31] << 10; \
           (nToData) |= (uint32) (pFromData)[32] << 18; \
           (nToData) |= (uint32) ((pFromData)[33] & 0x3f) << 26; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASK192_199(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[28] >> 6) & 0x03; \
           (nToData) |= (uint32) ((pFromData)[29] & 0x3f) << 2; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASKRESERVED(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[27] >> 5) & 0x07; \
           (nToData) |= (uint32) ((pFromData)[28] & 0x3f) << 3; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_MASKPORT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[27]) & 0x1f; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_PACKET0_31(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[23] ; \
           (nToData) |= (uint32) (pFromData)[24] << 8; \
           (nToData) |= (uint32) (pFromData)[25] << 16; \
           (nToData) |= (uint32) (pFromData)[26] << 24; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_PACKET32_63(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[19] ; \
           (nToData) |= (uint32) (pFromData)[20] << 8; \
           (nToData) |= (uint32) (pFromData)[21] << 16; \
           (nToData) |= (uint32) (pFromData)[22] << 24; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_PACKET64_95(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[15] ; \
           (nToData) |= (uint32) (pFromData)[16] << 8; \
           (nToData) |= (uint32) (pFromData)[17] << 16; \
           (nToData) |= (uint32) (pFromData)[18] << 24; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_PACKET96_127(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[11] ; \
           (nToData) |= (uint32) (pFromData)[12] << 8; \
           (nToData) |= (uint32) (pFromData)[13] << 16; \
           (nToData) |= (uint32) (pFromData)[14] << 24; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_PACKET128_159(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[7] ; \
           (nToData) |= (uint32) (pFromData)[8] << 8; \
           (nToData) |= (uint32) (pFromData)[9] << 16; \
           (nToData) |= (uint32) (pFromData)[10] << 24; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_PACKET160_191(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[3] ; \
           (nToData) |= (uint32) (pFromData)[4] << 8; \
           (nToData) |= (uint32) (pFromData)[5] << 16; \
           (nToData) |= (uint32) (pFromData)[6] << 24; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_PACKET192_199(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[2] ; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_RESERVED(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 7) & 0x01; \
           (nToData) |= (uint32) (pFromData)[1] << 1; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_PORT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0] >> 2) & 0x1f; \
          } while(0)

#define SB_ZF_C2PRCCTCAMCONFIGENTRYFIRST_GET_VALID(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0]) & 0x03; \
          } while(0)

#endif
