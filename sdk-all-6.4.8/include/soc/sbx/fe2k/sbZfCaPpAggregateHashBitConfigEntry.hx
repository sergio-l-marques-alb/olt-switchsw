/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id: sbZfCaPpAggregateHashBitConfigEntry.hx,v 1.4 Broadcom SDK $
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


#ifndef SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_H
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_H

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SIZE_IN_BYTES 40
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SIZE 40
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA9_BITS "319:288"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA8_BITS "287:256"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA7_BITS "255:224"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA6_BITS "223:192"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA5_BITS "191:160"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA4_BITS "159:128"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA3_BITS "127:96"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA2_BITS "95:64"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA1_BITS "63:32"
#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_M_UDATA0_BITS "31:0"


typedef struct _sbZfCaPpAggregateHashBitConfigEntry {
  uint32 m_uData9;
  uint32 m_uData8;
  uint32 m_uData7;
  uint32 m_uData6;
  uint32 m_uData5;
  uint32 m_uData4;
  uint32 m_uData3;
  uint32 m_uData2;
  uint32 m_uData1;
  uint32 m_uData0;
} sbZfCaPpAggregateHashBitConfigEntry_t;

uint32
sbZfCaPpAggregateHashBitConfigEntry_Pack(sbZfCaPpAggregateHashBitConfigEntry_t *pFrom,
                                         uint8 *pToData,
                                         uint32 nMaxToDataIndex);
void
sbZfCaPpAggregateHashBitConfigEntry_Unpack(sbZfCaPpAggregateHashBitConfigEntry_t *pToStruct,
                                           uint8 *pFromData,
                                           uint32 nMaxToDataIndex);
void
sbZfCaPpAggregateHashBitConfigEntry_InitInstance(sbZfCaPpAggregateHashBitConfigEntry_t *pFrame);

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA9(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[36] = ((nFromData)) & 0xFF; \
           (pToData)[37] = ((pToData)[37] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[38] = ((pToData)[38] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[39] = ((pToData)[39] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA8(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[32] = ((nFromData)) & 0xFF; \
           (pToData)[33] = ((pToData)[33] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[34] = ((pToData)[34] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[35] = ((pToData)[35] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA7(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[28] = ((nFromData)) & 0xFF; \
           (pToData)[29] = ((pToData)[29] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[30] = ((pToData)[30] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[31] = ((pToData)[31] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA6(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[24] = ((nFromData)) & 0xFF; \
           (pToData)[25] = ((pToData)[25] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[26] = ((pToData)[26] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[27] = ((pToData)[27] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA5(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[20] = ((nFromData)) & 0xFF; \
           (pToData)[21] = ((pToData)[21] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[22] = ((pToData)[22] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[23] = ((pToData)[23] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA4(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[16] = ((nFromData)) & 0xFF; \
           (pToData)[17] = ((pToData)[17] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[18] = ((pToData)[18] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[19] = ((pToData)[19] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA3(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[12] = ((nFromData)) & 0xFF; \
           (pToData)[13] = ((pToData)[13] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[14] = ((pToData)[14] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[15] = ((pToData)[15] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[8] = ((nFromData)) & 0xFF; \
           (pToData)[9] = ((pToData)[9] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[10] = ((pToData)[10] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[11] = ((pToData)[11] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((nFromData)) & 0xFF; \
           (pToData)[5] = ((pToData)[5] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[6] = ((pToData)[6] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[7] = ((pToData)[7] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_SET_DATA0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[3] = ((pToData)[3] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA9(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[36] ; \
           (nToData) |= (uint32) (pFromData)[37] << 8; \
           (nToData) |= (uint32) (pFromData)[38] << 16; \
           (nToData) |= (uint32) (pFromData)[39] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA8(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[32] ; \
           (nToData) |= (uint32) (pFromData)[33] << 8; \
           (nToData) |= (uint32) (pFromData)[34] << 16; \
           (nToData) |= (uint32) (pFromData)[35] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA7(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[28] ; \
           (nToData) |= (uint32) (pFromData)[29] << 8; \
           (nToData) |= (uint32) (pFromData)[30] << 16; \
           (nToData) |= (uint32) (pFromData)[31] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA6(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[24] ; \
           (nToData) |= (uint32) (pFromData)[25] << 8; \
           (nToData) |= (uint32) (pFromData)[26] << 16; \
           (nToData) |= (uint32) (pFromData)[27] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA5(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[20] ; \
           (nToData) |= (uint32) (pFromData)[21] << 8; \
           (nToData) |= (uint32) (pFromData)[22] << 16; \
           (nToData) |= (uint32) (pFromData)[23] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA4(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[16] ; \
           (nToData) |= (uint32) (pFromData)[17] << 8; \
           (nToData) |= (uint32) (pFromData)[18] << 16; \
           (nToData) |= (uint32) (pFromData)[19] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA3(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[12] ; \
           (nToData) |= (uint32) (pFromData)[13] << 8; \
           (nToData) |= (uint32) (pFromData)[14] << 16; \
           (nToData) |= (uint32) (pFromData)[15] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[8] ; \
           (nToData) |= (uint32) (pFromData)[9] << 8; \
           (nToData) |= (uint32) (pFromData)[10] << 16; \
           (nToData) |= (uint32) (pFromData)[11] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[4] ; \
           (nToData) |= (uint32) (pFromData)[5] << 8; \
           (nToData) |= (uint32) (pFromData)[6] << 16; \
           (nToData) |= (uint32) (pFromData)[7] << 24; \
          } while(0)

#define SB_ZF_CAPPAGGREGATEHASHBITCONFIGENTRY_GET_DATA0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) (pFromData)[1] << 8; \
           (nToData) |= (uint32) (pFromData)[2] << 16; \
           (nToData) |= (uint32) (pFromData)[3] << 24; \
          } while(0)

#endif
