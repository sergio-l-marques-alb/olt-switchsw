/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id: sbZfCaPbCounterEntry.hx,v 1.4 Broadcom SDK $
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


#ifndef SB_ZF_CAPBCOUNTERENTRY_H
#define SB_ZF_CAPBCOUNTERENTRY_H

#define SB_ZF_CAPBCOUNTERENTRY_SIZE_IN_BYTES 96
#define SB_ZF_CAPBCOUNTERENTRY_SIZE 96
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA23_BITS "767:736"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA22_BITS "735:704"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA21_BITS "703:672"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA20_BITS "671:640"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA19_BITS "639:608"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA18_BITS "607:576"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA17_BITS "575:544"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA16_BITS "543:512"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA15_BITS "511:480"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA14_BITS "479:448"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA13_BITS "447:416"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA12_BITS "415:384"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA11_BITS "383:352"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA10_BITS "351:320"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA9_BITS "319:288"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA8_BITS "287:256"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA7_BITS "255:224"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA6_BITS "223:192"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA5_BITS "191:160"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA4_BITS "159:128"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA3_BITS "127:96"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA2_BITS "95:64"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA1_BITS "63:32"
#define SB_ZF_CAPBCOUNTERENTRY_M_UDATA0_BITS "31:0"


typedef struct _sbZfCaPbCounterEntry {
  uint32 m_uData23;
  uint32 m_uData22;
  uint32 m_uData21;
  uint32 m_uData20;
  uint32 m_uData19;
  uint32 m_uData18;
  uint32 m_uData17;
  uint32 m_uData16;
  uint32 m_uData15;
  uint32 m_uData14;
  uint32 m_uData13;
  uint32 m_uData12;
  uint32 m_uData11;
  uint32 m_uData10;
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
} sbZfCaPbCounterEntry_t;

uint32
sbZfCaPbCounterEntry_Pack(sbZfCaPbCounterEntry_t *pFrom,
                          uint8 *pToData,
                          uint32 nMaxToDataIndex);
void
sbZfCaPbCounterEntry_Unpack(sbZfCaPbCounterEntry_t *pToStruct,
                            uint8 *pFromData,
                            uint32 nMaxToDataIndex);
void
sbZfCaPbCounterEntry_InitInstance(sbZfCaPbCounterEntry_t *pFrame);

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA23(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[92] = ((nFromData)) & 0xFF; \
           (pToData)[93] = ((pToData)[93] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[94] = ((pToData)[94] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[95] = ((pToData)[95] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA22(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[88] = ((nFromData)) & 0xFF; \
           (pToData)[89] = ((pToData)[89] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[90] = ((pToData)[90] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[91] = ((pToData)[91] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA21(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[84] = ((nFromData)) & 0xFF; \
           (pToData)[85] = ((pToData)[85] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[86] = ((pToData)[86] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[87] = ((pToData)[87] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA20(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[80] = ((nFromData)) & 0xFF; \
           (pToData)[81] = ((pToData)[81] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[82] = ((pToData)[82] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[83] = ((pToData)[83] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA19(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[76] = ((nFromData)) & 0xFF; \
           (pToData)[77] = ((pToData)[77] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[78] = ((pToData)[78] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[79] = ((pToData)[79] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA18(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[72] = ((nFromData)) & 0xFF; \
           (pToData)[73] = ((pToData)[73] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[74] = ((pToData)[74] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[75] = ((pToData)[75] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA17(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[68] = ((nFromData)) & 0xFF; \
           (pToData)[69] = ((pToData)[69] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[70] = ((pToData)[70] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[71] = ((pToData)[71] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA16(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[64] = ((nFromData)) & 0xFF; \
           (pToData)[65] = ((pToData)[65] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[66] = ((pToData)[66] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[67] = ((pToData)[67] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA15(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[60] = ((nFromData)) & 0xFF; \
           (pToData)[61] = ((pToData)[61] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[62] = ((pToData)[62] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[63] = ((pToData)[63] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA14(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[56] = ((nFromData)) & 0xFF; \
           (pToData)[57] = ((pToData)[57] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[58] = ((pToData)[58] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[59] = ((pToData)[59] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA13(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[52] = ((nFromData)) & 0xFF; \
           (pToData)[53] = ((pToData)[53] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[54] = ((pToData)[54] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[55] = ((pToData)[55] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA12(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[48] = ((nFromData)) & 0xFF; \
           (pToData)[49] = ((pToData)[49] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[50] = ((pToData)[50] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[51] = ((pToData)[51] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA11(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[44] = ((nFromData)) & 0xFF; \
           (pToData)[45] = ((pToData)[45] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[46] = ((pToData)[46] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[47] = ((pToData)[47] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA10(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[40] = ((nFromData)) & 0xFF; \
           (pToData)[41] = ((pToData)[41] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[42] = ((pToData)[42] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[43] = ((pToData)[43] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA9(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[36] = ((nFromData)) & 0xFF; \
           (pToData)[37] = ((pToData)[37] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[38] = ((pToData)[38] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[39] = ((pToData)[39] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA8(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[32] = ((nFromData)) & 0xFF; \
           (pToData)[33] = ((pToData)[33] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[34] = ((pToData)[34] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[35] = ((pToData)[35] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA7(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[28] = ((nFromData)) & 0xFF; \
           (pToData)[29] = ((pToData)[29] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[30] = ((pToData)[30] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[31] = ((pToData)[31] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA6(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[24] = ((nFromData)) & 0xFF; \
           (pToData)[25] = ((pToData)[25] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[26] = ((pToData)[26] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[27] = ((pToData)[27] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA5(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[20] = ((nFromData)) & 0xFF; \
           (pToData)[21] = ((pToData)[21] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[22] = ((pToData)[22] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[23] = ((pToData)[23] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA4(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[16] = ((nFromData)) & 0xFF; \
           (pToData)[17] = ((pToData)[17] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[18] = ((pToData)[18] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[19] = ((pToData)[19] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA3(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[12] = ((nFromData)) & 0xFF; \
           (pToData)[13] = ((pToData)[13] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[14] = ((pToData)[14] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[15] = ((pToData)[15] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[8] = ((nFromData)) & 0xFF; \
           (pToData)[9] = ((pToData)[9] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[10] = ((pToData)[10] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[11] = ((pToData)[11] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((nFromData)) & 0xFF; \
           (pToData)[5] = ((pToData)[5] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[6] = ((pToData)[6] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[7] = ((pToData)[7] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_SET_DATA0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[3] = ((pToData)[3] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA23(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[92] ; \
           (nToData) |= (uint32) (pFromData)[93] << 8; \
           (nToData) |= (uint32) (pFromData)[94] << 16; \
           (nToData) |= (uint32) (pFromData)[95] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA22(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[88] ; \
           (nToData) |= (uint32) (pFromData)[89] << 8; \
           (nToData) |= (uint32) (pFromData)[90] << 16; \
           (nToData) |= (uint32) (pFromData)[91] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA21(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[84] ; \
           (nToData) |= (uint32) (pFromData)[85] << 8; \
           (nToData) |= (uint32) (pFromData)[86] << 16; \
           (nToData) |= (uint32) (pFromData)[87] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA20(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[80] ; \
           (nToData) |= (uint32) (pFromData)[81] << 8; \
           (nToData) |= (uint32) (pFromData)[82] << 16; \
           (nToData) |= (uint32) (pFromData)[83] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA19(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[76] ; \
           (nToData) |= (uint32) (pFromData)[77] << 8; \
           (nToData) |= (uint32) (pFromData)[78] << 16; \
           (nToData) |= (uint32) (pFromData)[79] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA18(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[72] ; \
           (nToData) |= (uint32) (pFromData)[73] << 8; \
           (nToData) |= (uint32) (pFromData)[74] << 16; \
           (nToData) |= (uint32) (pFromData)[75] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA17(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[68] ; \
           (nToData) |= (uint32) (pFromData)[69] << 8; \
           (nToData) |= (uint32) (pFromData)[70] << 16; \
           (nToData) |= (uint32) (pFromData)[71] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA16(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[64] ; \
           (nToData) |= (uint32) (pFromData)[65] << 8; \
           (nToData) |= (uint32) (pFromData)[66] << 16; \
           (nToData) |= (uint32) (pFromData)[67] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA15(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[60] ; \
           (nToData) |= (uint32) (pFromData)[61] << 8; \
           (nToData) |= (uint32) (pFromData)[62] << 16; \
           (nToData) |= (uint32) (pFromData)[63] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA14(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[56] ; \
           (nToData) |= (uint32) (pFromData)[57] << 8; \
           (nToData) |= (uint32) (pFromData)[58] << 16; \
           (nToData) |= (uint32) (pFromData)[59] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA13(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[52] ; \
           (nToData) |= (uint32) (pFromData)[53] << 8; \
           (nToData) |= (uint32) (pFromData)[54] << 16; \
           (nToData) |= (uint32) (pFromData)[55] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA12(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[48] ; \
           (nToData) |= (uint32) (pFromData)[49] << 8; \
           (nToData) |= (uint32) (pFromData)[50] << 16; \
           (nToData) |= (uint32) (pFromData)[51] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA11(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[44] ; \
           (nToData) |= (uint32) (pFromData)[45] << 8; \
           (nToData) |= (uint32) (pFromData)[46] << 16; \
           (nToData) |= (uint32) (pFromData)[47] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA10(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[40] ; \
           (nToData) |= (uint32) (pFromData)[41] << 8; \
           (nToData) |= (uint32) (pFromData)[42] << 16; \
           (nToData) |= (uint32) (pFromData)[43] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA9(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[36] ; \
           (nToData) |= (uint32) (pFromData)[37] << 8; \
           (nToData) |= (uint32) (pFromData)[38] << 16; \
           (nToData) |= (uint32) (pFromData)[39] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA8(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[32] ; \
           (nToData) |= (uint32) (pFromData)[33] << 8; \
           (nToData) |= (uint32) (pFromData)[34] << 16; \
           (nToData) |= (uint32) (pFromData)[35] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA7(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[28] ; \
           (nToData) |= (uint32) (pFromData)[29] << 8; \
           (nToData) |= (uint32) (pFromData)[30] << 16; \
           (nToData) |= (uint32) (pFromData)[31] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA6(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[24] ; \
           (nToData) |= (uint32) (pFromData)[25] << 8; \
           (nToData) |= (uint32) (pFromData)[26] << 16; \
           (nToData) |= (uint32) (pFromData)[27] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA5(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[20] ; \
           (nToData) |= (uint32) (pFromData)[21] << 8; \
           (nToData) |= (uint32) (pFromData)[22] << 16; \
           (nToData) |= (uint32) (pFromData)[23] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA4(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[16] ; \
           (nToData) |= (uint32) (pFromData)[17] << 8; \
           (nToData) |= (uint32) (pFromData)[18] << 16; \
           (nToData) |= (uint32) (pFromData)[19] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA3(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[12] ; \
           (nToData) |= (uint32) (pFromData)[13] << 8; \
           (nToData) |= (uint32) (pFromData)[14] << 16; \
           (nToData) |= (uint32) (pFromData)[15] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[8] ; \
           (nToData) |= (uint32) (pFromData)[9] << 8; \
           (nToData) |= (uint32) (pFromData)[10] << 16; \
           (nToData) |= (uint32) (pFromData)[11] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[4] ; \
           (nToData) |= (uint32) (pFromData)[5] << 8; \
           (nToData) |= (uint32) (pFromData)[6] << 16; \
           (nToData) |= (uint32) (pFromData)[7] << 24; \
          } while(0)

#define SB_ZF_CAPBCOUNTERENTRY_GET_DATA0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) (pFromData)[1] << 8; \
           (nToData) |= (uint32) (pFromData)[2] << 16; \
           (nToData) |= (uint32) (pFromData)[3] << 24; \
          } while(0)

#endif
