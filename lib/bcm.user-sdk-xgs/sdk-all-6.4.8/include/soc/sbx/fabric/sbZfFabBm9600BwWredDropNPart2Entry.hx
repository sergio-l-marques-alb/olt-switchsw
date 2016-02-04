/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id: sbZfFabBm9600BwWredDropNPart2Entry.hx,v 1.2 Broadcom SDK $
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


#ifndef SB_ZF_FAB_BM9600_BWWREDDROPNPART2ENTRY_H
#define SB_ZF_FAB_BM9600_BWWREDDROPNPART2ENTRY_H

#define SB_ZF_FAB_BM9600_BWWREDDROPNPART2ENTRY_SIZE_IN_BYTES 2
#define SB_ZF_FAB_BM9600_BWWREDDROPNPART2ENTRY_SIZE 2
#define SB_ZF_FAB_BM9600_BWWREDDROPNPART2ENTRY_M_UDP2_BITS "15:15"
#define SB_ZF_FAB_BM9600_BWWREDDROPNPART2ENTRY_M_UECN2_BITS "14:14"
#define SB_ZF_FAB_BM9600_BWWREDDROPNPART2ENTRY_M_URESERVED2_BITS "13:10"
#define SB_ZF_FAB_BM9600_BWWREDDROPNPART2ENTRY_M_UPBDE2_BITS "9:0"


typedef struct _sbZfFabBm9600BwWredDropNPart2Entry {
  uint32 m_uDp2;
  uint32 m_uEcn2;
  uint32 m_uReserved2;
  uint32 m_uPbDe2;
} sbZfFabBm9600BwWredDropNPart2Entry_t;

uint32
sbZfFabBm9600BwWredDropNPart2Entry_Pack(sbZfFabBm9600BwWredDropNPart2Entry_t *pFrom,
                                        uint8 *pToData,
                                        uint32 nMaxToDataIndex);
void
sbZfFabBm9600BwWredDropNPart2Entry_Unpack(sbZfFabBm9600BwWredDropNPart2Entry_t *pToStruct,
                                          uint8 *pFromData,
                                          uint32 nMaxToDataIndex);
void
sbZfFabBm9600BwWredDropNPart2Entry_InitInstance(sbZfFabBm9600BwWredDropNPart2Entry_t *pFrame);

#ifdef SAND_BIG_ENDIAN_HOST
#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_DP2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_ECN2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_RESERVED2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x0f << 2)) | (((nFromData) & 0x0f) << 2); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_PBDE2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((nFromData)) & 0xFF; \
           (pToData)[2] = ((pToData)[2] & ~ 0x03) | (((nFromData) >> 8) & 0x03); \
          } while(0)

#else
#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_DP2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_ECN2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_RESERVED2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x0f << 2)) | (((nFromData) & 0x0f) << 2); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_PBDE2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~ 0x03) | (((nFromData) >> 8) & 0x03); \
          } while(0)

#endif
#ifdef SAND_BIG_ENDIAN_HOST
#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_DP2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_ECN2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_RESERVED2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x0f << 2)) | (((nFromData) & 0x0f) << 2); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_PBDE2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((nFromData)) & 0xFF; \
           (pToData)[2] = ((pToData)[2] & ~ 0x03) | (((nFromData) >> 8) & 0x03); \
          } while(0)

#else
#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_DP2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_ECN2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_RESERVED2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x0f << 2)) | (((nFromData) & 0x0f) << 2); \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_SET_PBDE2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~ 0x03) | (((nFromData) >> 8) & 0x03); \
          } while(0)

#endif
#ifdef SAND_BIG_ENDIAN_HOST
#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_DP2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_ECN2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_RESERVED2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 2) & 0x0f; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_PBDE2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[3] ; \
           (nToData) |= (uint32) ((pFromData)[2] & 0x03) << 8; \
          } while(0)

#else
#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_DP2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_ECN2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_RESERVED2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 2) & 0x0f; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_PBDE2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) ((pFromData)[1] & 0x03) << 8; \
          } while(0)

#endif
#ifdef SAND_BIG_ENDIAN_HOST
#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_DP2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_ECN2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_RESERVED2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 2) & 0x0f; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_PBDE2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[3] ; \
           (nToData) |= (uint32) ((pFromData)[2] & 0x03) << 8; \
          } while(0)

#else
#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_DP2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_ECN2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_RESERVED2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 2) & 0x0f; \
          } while(0)

#define SB_ZF_FABBM9600BWWREDDROPNPART2ENTRY_GET_PBDE2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) ((pFromData)[1] & 0x03) << 8; \
          } while(0)

#endif
#endif
