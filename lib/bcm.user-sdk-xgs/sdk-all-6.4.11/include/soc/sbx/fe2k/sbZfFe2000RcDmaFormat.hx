/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id: sbZfFe2000RcDmaFormat.hx,v 1.4 Broadcom SDK $
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


#ifndef SB_ZF_FE2000RCDMAFORMAT_H
#define SB_ZF_FE2000RCDMAFORMAT_H

#define SB_ZF_FE2000RCDMAFORMAT_SIZE_IN_BYTES 256
#define SB_ZF_FE2000RCDMAFORMAT_SIZE 256
#define SB_ZF_FE2000RCDMAFORMAT_UINSTRUCTION0_BITS "31:0"
#define SB_ZF_FE2000RCDMAFORMAT_UINSTRUCTION1_BITS "63:32"
#define SB_ZF_FE2000RCDMAFORMAT_UINSTRUCTION2_BITS "95:64"
#define SB_ZF_FE2000RCDMAFORMAT_UINSTRUCTION3_BITS "127:96"
#define SB_ZF_FE2000RCDMAFORMAT_URESV0_BITS "159:128"
#define SB_ZF_FE2000RCDMAFORMAT_URESV1_BITS "191:160"
#define SB_ZF_FE2000RCDMAFORMAT_URESV2_BITS "223:192"
#define SB_ZF_FE2000RCDMAFORMAT_URESV3_BITS "255:224"
#define SB_ZF_FE2000RCDMAFORMAT_URESV4_BITS "287:256"
#define SB_ZF_FE2000RCDMAFORMAT_URESV5_BITS "319:288"
#define SB_ZF_FE2000RCDMAFORMAT_URESV6_BITS "351:320"
#define SB_ZF_FE2000RCDMAFORMAT_URESV7_BITS "383:352"
#define SB_ZF_FE2000RCDMAFORMAT_URESV8_BITS "415:384"
#define SB_ZF_FE2000RCDMAFORMAT_URESV9_BITS "447:416"
#define SB_ZF_FE2000RCDMAFORMAT_URESV10_BITS "479:448"
#define SB_ZF_FE2000RCDMAFORMAT_URESV11_BITS "511:480"
#define SB_ZF_FE2000RCDMAFORMAT_URESV12_BITS "543:512"
#define SB_ZF_FE2000RCDMAFORMAT_URESV13_BITS "575:544"
#define SB_ZF_FE2000RCDMAFORMAT_URESV14_BITS "607:576"
#define SB_ZF_FE2000RCDMAFORMAT_URESV15_BITS "639:608"
#define SB_ZF_FE2000RCDMAFORMAT_URESV16_BITS "671:640"
#define SB_ZF_FE2000RCDMAFORMAT_URESV17_BITS "703:672"
#define SB_ZF_FE2000RCDMAFORMAT_URESV18_BITS "735:704"
#define SB_ZF_FE2000RCDMAFORMAT_URESV19_BITS "767:736"
#define SB_ZF_FE2000RCDMAFORMAT_URESV20_BITS "799:768"
#define SB_ZF_FE2000RCDMAFORMAT_URESV21_BITS "831:800"
#define SB_ZF_FE2000RCDMAFORMAT_URESV22_BITS "863:832"
#define SB_ZF_FE2000RCDMAFORMAT_URESV23_BITS "895:864"
#define SB_ZF_FE2000RCDMAFORMAT_URESV24_BITS "927:896"
#define SB_ZF_FE2000RCDMAFORMAT_URESV25_BITS "959:928"
#define SB_ZF_FE2000RCDMAFORMAT_URESV26_BITS "991:960"
#define SB_ZF_FE2000RCDMAFORMAT_URESV27_BITS "1023:992"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN0_31_0_BITS "1055:1024"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN0_63_32_BITS "1087:1056"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN0_95_64_BITS "1119:1088"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN0_127_96_BITS "1151:1120"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN1_31_0_BITS "1183:1152"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN1_63_32_BITS "1215:1184"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN1_95_64_BITS "1247:1216"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN1_127_96_BITS "1279:1248"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN2_31_0_BITS "1311:1280"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN2_63_32_BITS "1343:1312"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN2_95_64_BITS "1375:1344"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN2_127_96_BITS "1407:1376"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN3_31_0_BITS "1439:1408"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN3_63_32_BITS "1471:1440"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN3_95_64_BITS "1503:1472"
#define SB_ZF_FE2000RCDMAFORMAT_USB0PATTERN3_127_96_BITS "1535:1504"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN0_31_0_BITS "1567:1536"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN0_63_32_BITS "1599:1568"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN0_95_64_BITS "1631:1600"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN0_127_96_BITS "1663:1632"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN1_31_0_BITS "1695:1664"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN1_63_32_BITS "1727:1696"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN1_95_64_BITS "1759:1728"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN1_127_96_BITS "1791:1760"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN2_31_0_BITS "1823:1792"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN2_63_32_BITS "1855:1824"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN2_95_64_BITS "1887:1856"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN2_127_96_BITS "1919:1888"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN3_31_0_BITS "1951:1920"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN3_63_32_BITS "1983:1952"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN3_95_64_BITS "2015:1984"
#define SB_ZF_FE2000RCDMAFORMAT_USB1PATTERN3_127_96_BITS "2047:2016"



#define SB_FE2000RCDMAFORMAT_SIZE_IN_WORDS  (64)

typedef struct 
{
  uint32    uInstruction[4];
  uint32    uPattern[32];
} sbFe2000RcDmaBlock_t;

typedef struct 
{
  uint32 *pInstruction[4];
  uint32 *pPattern[32];
} sbFe2000RcDmaFormatAlias_t;

typedef struct _sbZfFe2000RcDmaFormat {
  uint32 uInstruction0;
  uint32 uInstruction1;
  uint32 uInstruction2;
  uint32 uInstruction3;
  uint32 uResv0;
  uint32 uResv1;
  uint32 uResv2;
  uint32 uResv3;
  uint32 uResv4;
  uint32 uResv5;
  uint32 uResv6;
  uint32 uResv7;
  uint32 uResv8;
  uint32 uResv9;
  uint32 uResv10;
  uint32 uResv11;
  uint32 uResv12;
  uint32 uResv13;
  uint32 uResv14;
  uint32 uResv15;
  uint32 uResv16;
  uint32 uResv17;
  uint32 uResv18;
  uint32 uResv19;
  uint32 uResv20;
  uint32 uResv21;
  uint32 uResv22;
  uint32 uResv23;
  uint32 uResv24;
  uint32 uResv25;
  uint32 uResv26;
  uint32 uResv27;
  uint32 uSb0Pattern0_31_0;
  uint32 uSb0Pattern0_63_32;
  uint32 uSb0Pattern0_95_64;
  uint32 uSb0Pattern0_127_96;
  uint32 uSb0Pattern1_31_0;
  uint32 uSb0Pattern1_63_32;
  uint32 uSb0Pattern1_95_64;
  uint32 uSb0Pattern1_127_96;
  uint32 uSb0Pattern2_31_0;
  uint32 uSb0Pattern2_63_32;
  uint32 uSb0Pattern2_95_64;
  uint32 uSb0Pattern2_127_96;
  uint32 uSb0Pattern3_31_0;
  uint32 uSb0Pattern3_63_32;
  uint32 uSb0Pattern3_95_64;
  uint32 uSb0Pattern3_127_96;
  uint32 uSb1Pattern0_31_0;
  uint32 uSb1Pattern0_63_32;
  uint32 uSb1Pattern0_95_64;
  uint32 uSb1Pattern0_127_96;
  uint32 uSb1Pattern1_31_0;
  uint32 uSb1Pattern1_63_32;
  uint32 uSb1Pattern1_95_64;
  uint32 uSb1Pattern1_127_96;
  uint32 uSb1Pattern2_31_0;
  uint32 uSb1Pattern2_63_32;
  uint32 uSb1Pattern2_95_64;
  uint32 uSb1Pattern2_127_96;
  uint32 uSb1Pattern3_31_0;
  uint32 uSb1Pattern3_63_32;
  uint32 uSb1Pattern3_95_64;
  uint32 uSb1Pattern3_127_96;
} sbZfFe2000RcDmaFormat_t;

uint32
sbZfFe2000RcDmaFormat_Pack(sbZfFe2000RcDmaFormat_t *pFrom,
                           uint8 *pToData,
                           uint32 nMaxToDataIndex);
void
sbZfFe2000RcDmaFormat_Unpack(sbZfFe2000RcDmaFormat_t *pToStruct,
                             uint8 *pFromData,
                             uint32 nMaxToDataIndex);
void
sbZfFe2000RcDmaFormat_InitInstance(sbZfFe2000RcDmaFormat_t *pFrame);

#define SB_ZF_FE2000RCDMAFORMAT_SET_INSTR0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[3] = ((pToData)[3] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_INSTR1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((nFromData)) & 0xFF; \
           (pToData)[5] = ((pToData)[5] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[6] = ((pToData)[6] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[7] = ((pToData)[7] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_INSTR2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[8] = ((nFromData)) & 0xFF; \
           (pToData)[9] = ((pToData)[9] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[10] = ((pToData)[10] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[11] = ((pToData)[11] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_INSTR3(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[12] = ((nFromData)) & 0xFF; \
           (pToData)[13] = ((pToData)[13] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[14] = ((pToData)[14] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[15] = ((pToData)[15] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[16] = ((nFromData)) & 0xFF; \
           (pToData)[17] = ((pToData)[17] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[18] = ((pToData)[18] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[19] = ((pToData)[19] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[20] = ((nFromData)) & 0xFF; \
           (pToData)[21] = ((pToData)[21] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[22] = ((pToData)[22] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[23] = ((pToData)[23] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[24] = ((nFromData)) & 0xFF; \
           (pToData)[25] = ((pToData)[25] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[26] = ((pToData)[26] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[27] = ((pToData)[27] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV3(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[28] = ((nFromData)) & 0xFF; \
           (pToData)[29] = ((pToData)[29] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[30] = ((pToData)[30] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[31] = ((pToData)[31] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV4(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[32] = ((nFromData)) & 0xFF; \
           (pToData)[33] = ((pToData)[33] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[34] = ((pToData)[34] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[35] = ((pToData)[35] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV5(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[36] = ((nFromData)) & 0xFF; \
           (pToData)[37] = ((pToData)[37] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[38] = ((pToData)[38] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[39] = ((pToData)[39] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV6(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[40] = ((nFromData)) & 0xFF; \
           (pToData)[41] = ((pToData)[41] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[42] = ((pToData)[42] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[43] = ((pToData)[43] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV7(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[44] = ((nFromData)) & 0xFF; \
           (pToData)[45] = ((pToData)[45] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[46] = ((pToData)[46] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[47] = ((pToData)[47] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV8(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[48] = ((nFromData)) & 0xFF; \
           (pToData)[49] = ((pToData)[49] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[50] = ((pToData)[50] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[51] = ((pToData)[51] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV9(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[52] = ((nFromData)) & 0xFF; \
           (pToData)[53] = ((pToData)[53] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[54] = ((pToData)[54] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[55] = ((pToData)[55] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV10(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[56] = ((nFromData)) & 0xFF; \
           (pToData)[57] = ((pToData)[57] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[58] = ((pToData)[58] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[59] = ((pToData)[59] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV11(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[60] = ((nFromData)) & 0xFF; \
           (pToData)[61] = ((pToData)[61] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[62] = ((pToData)[62] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[63] = ((pToData)[63] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV12(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[64] = ((nFromData)) & 0xFF; \
           (pToData)[65] = ((pToData)[65] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[66] = ((pToData)[66] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[67] = ((pToData)[67] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV13(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[68] = ((nFromData)) & 0xFF; \
           (pToData)[69] = ((pToData)[69] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[70] = ((pToData)[70] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[71] = ((pToData)[71] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV14(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[72] = ((nFromData)) & 0xFF; \
           (pToData)[73] = ((pToData)[73] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[74] = ((pToData)[74] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[75] = ((pToData)[75] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV15(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[76] = ((nFromData)) & 0xFF; \
           (pToData)[77] = ((pToData)[77] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[78] = ((pToData)[78] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[79] = ((pToData)[79] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV16(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[80] = ((nFromData)) & 0xFF; \
           (pToData)[81] = ((pToData)[81] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[82] = ((pToData)[82] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[83] = ((pToData)[83] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV17(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[84] = ((nFromData)) & 0xFF; \
           (pToData)[85] = ((pToData)[85] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[86] = ((pToData)[86] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[87] = ((pToData)[87] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV18(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[88] = ((nFromData)) & 0xFF; \
           (pToData)[89] = ((pToData)[89] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[90] = ((pToData)[90] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[91] = ((pToData)[91] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV19(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[92] = ((nFromData)) & 0xFF; \
           (pToData)[93] = ((pToData)[93] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[94] = ((pToData)[94] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[95] = ((pToData)[95] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV20(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[96] = ((nFromData)) & 0xFF; \
           (pToData)[97] = ((pToData)[97] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[98] = ((pToData)[98] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[99] = ((pToData)[99] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV21(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[100] = ((nFromData)) & 0xFF; \
           (pToData)[101] = ((pToData)[101] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[102] = ((pToData)[102] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[103] = ((pToData)[103] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV22(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[104] = ((nFromData)) & 0xFF; \
           (pToData)[105] = ((pToData)[105] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[106] = ((pToData)[106] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[107] = ((pToData)[107] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV23(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[108] = ((nFromData)) & 0xFF; \
           (pToData)[109] = ((pToData)[109] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[110] = ((pToData)[110] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[111] = ((pToData)[111] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV24(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[112] = ((nFromData)) & 0xFF; \
           (pToData)[113] = ((pToData)[113] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[114] = ((pToData)[114] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[115] = ((pToData)[115] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV25(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[116] = ((nFromData)) & 0xFF; \
           (pToData)[117] = ((pToData)[117] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[118] = ((pToData)[118] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[119] = ((pToData)[119] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV26(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[120] = ((nFromData)) & 0xFF; \
           (pToData)[121] = ((pToData)[121] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[122] = ((pToData)[122] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[123] = ((pToData)[123] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_RESV27(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[124] = ((nFromData)) & 0xFF; \
           (pToData)[125] = ((pToData)[125] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[126] = ((pToData)[126] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[127] = ((pToData)[127] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB00PATT310(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[128] = ((nFromData)) & 0xFF; \
           (pToData)[129] = ((pToData)[129] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[130] = ((pToData)[130] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[131] = ((pToData)[131] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB00PATT6332(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[132] = ((nFromData)) & 0xFF; \
           (pToData)[133] = ((pToData)[133] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[134] = ((pToData)[134] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[135] = ((pToData)[135] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB00PATT6495(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[136] = ((nFromData)) & 0xFF; \
           (pToData)[137] = ((pToData)[137] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[138] = ((pToData)[138] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[139] = ((pToData)[139] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB00PATT12796(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[140] = ((nFromData)) & 0xFF; \
           (pToData)[141] = ((pToData)[141] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[142] = ((pToData)[142] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[143] = ((pToData)[143] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB01PATT310(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[144] = ((nFromData)) & 0xFF; \
           (pToData)[145] = ((pToData)[145] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[146] = ((pToData)[146] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[147] = ((pToData)[147] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB01PATT6332(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[148] = ((nFromData)) & 0xFF; \
           (pToData)[149] = ((pToData)[149] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[150] = ((pToData)[150] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[151] = ((pToData)[151] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB01PATT6495(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[152] = ((nFromData)) & 0xFF; \
           (pToData)[153] = ((pToData)[153] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[154] = ((pToData)[154] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[155] = ((pToData)[155] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB01PATT12796(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[156] = ((nFromData)) & 0xFF; \
           (pToData)[157] = ((pToData)[157] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[158] = ((pToData)[158] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[159] = ((pToData)[159] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB02PATT310(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[160] = ((nFromData)) & 0xFF; \
           (pToData)[161] = ((pToData)[161] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[162] = ((pToData)[162] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[163] = ((pToData)[163] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB02PATT6332(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[164] = ((nFromData)) & 0xFF; \
           (pToData)[165] = ((pToData)[165] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[166] = ((pToData)[166] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[167] = ((pToData)[167] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB02PATT6495(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[168] = ((nFromData)) & 0xFF; \
           (pToData)[169] = ((pToData)[169] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[170] = ((pToData)[170] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[171] = ((pToData)[171] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB02PATT12796(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[172] = ((nFromData)) & 0xFF; \
           (pToData)[173] = ((pToData)[173] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[174] = ((pToData)[174] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[175] = ((pToData)[175] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB03PATT310(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[176] = ((nFromData)) & 0xFF; \
           (pToData)[177] = ((pToData)[177] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[178] = ((pToData)[178] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[179] = ((pToData)[179] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB03PATT6332(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[180] = ((nFromData)) & 0xFF; \
           (pToData)[181] = ((pToData)[181] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[182] = ((pToData)[182] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[183] = ((pToData)[183] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB03PATT6495(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[184] = ((nFromData)) & 0xFF; \
           (pToData)[185] = ((pToData)[185] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[186] = ((pToData)[186] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[187] = ((pToData)[187] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB03PATT12796(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[188] = ((nFromData)) & 0xFF; \
           (pToData)[189] = ((pToData)[189] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[190] = ((pToData)[190] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[191] = ((pToData)[191] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB10PATT310(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[192] = ((nFromData)) & 0xFF; \
           (pToData)[193] = ((pToData)[193] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[194] = ((pToData)[194] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[195] = ((pToData)[195] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB10PATT6332(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[196] = ((nFromData)) & 0xFF; \
           (pToData)[197] = ((pToData)[197] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[198] = ((pToData)[198] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[199] = ((pToData)[199] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB10PATT6495(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[200] = ((nFromData)) & 0xFF; \
           (pToData)[201] = ((pToData)[201] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[202] = ((pToData)[202] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[203] = ((pToData)[203] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB10PATT12796(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[204] = ((nFromData)) & 0xFF; \
           (pToData)[205] = ((pToData)[205] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[206] = ((pToData)[206] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[207] = ((pToData)[207] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB11PATT310(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[208] = ((nFromData)) & 0xFF; \
           (pToData)[209] = ((pToData)[209] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[210] = ((pToData)[210] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[211] = ((pToData)[211] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB11PATT6332(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[212] = ((nFromData)) & 0xFF; \
           (pToData)[213] = ((pToData)[213] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[214] = ((pToData)[214] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[215] = ((pToData)[215] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB11PATT6495(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[216] = ((nFromData)) & 0xFF; \
           (pToData)[217] = ((pToData)[217] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[218] = ((pToData)[218] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[219] = ((pToData)[219] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB11PATT12796(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[220] = ((nFromData)) & 0xFF; \
           (pToData)[221] = ((pToData)[221] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[222] = ((pToData)[222] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[223] = ((pToData)[223] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB12PATT310(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[224] = ((nFromData)) & 0xFF; \
           (pToData)[225] = ((pToData)[225] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[226] = ((pToData)[226] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[227] = ((pToData)[227] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB12PATT6332(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[228] = ((nFromData)) & 0xFF; \
           (pToData)[229] = ((pToData)[229] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[230] = ((pToData)[230] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[231] = ((pToData)[231] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB12PATT6495(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[232] = ((nFromData)) & 0xFF; \
           (pToData)[233] = ((pToData)[233] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[234] = ((pToData)[234] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[235] = ((pToData)[235] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB12PATT12796(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[236] = ((nFromData)) & 0xFF; \
           (pToData)[237] = ((pToData)[237] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[238] = ((pToData)[238] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[239] = ((pToData)[239] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB13PATT310(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[240] = ((nFromData)) & 0xFF; \
           (pToData)[241] = ((pToData)[241] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[242] = ((pToData)[242] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[243] = ((pToData)[243] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB13PATT6332(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[244] = ((nFromData)) & 0xFF; \
           (pToData)[245] = ((pToData)[245] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[246] = ((pToData)[246] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[247] = ((pToData)[247] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB13PATT6495(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[248] = ((nFromData)) & 0xFF; \
           (pToData)[249] = ((pToData)[249] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[250] = ((pToData)[250] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[251] = ((pToData)[251] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_SET_SB13PATT12796(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[252] = ((nFromData)) & 0xFF; \
           (pToData)[253] = ((pToData)[253] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[254] = ((pToData)[254] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[255] = ((pToData)[255] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_INSTR0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) (pFromData)[1] << 8; \
           (nToData) |= (uint32) (pFromData)[2] << 16; \
           (nToData) |= (uint32) (pFromData)[3] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_INSTR1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[4] ; \
           (nToData) |= (uint32) (pFromData)[5] << 8; \
           (nToData) |= (uint32) (pFromData)[6] << 16; \
           (nToData) |= (uint32) (pFromData)[7] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_INSTR2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[8] ; \
           (nToData) |= (uint32) (pFromData)[9] << 8; \
           (nToData) |= (uint32) (pFromData)[10] << 16; \
           (nToData) |= (uint32) (pFromData)[11] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_INSTR3(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[12] ; \
           (nToData) |= (uint32) (pFromData)[13] << 8; \
           (nToData) |= (uint32) (pFromData)[14] << 16; \
           (nToData) |= (uint32) (pFromData)[15] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[16] ; \
           (nToData) |= (uint32) (pFromData)[17] << 8; \
           (nToData) |= (uint32) (pFromData)[18] << 16; \
           (nToData) |= (uint32) (pFromData)[19] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[20] ; \
           (nToData) |= (uint32) (pFromData)[21] << 8; \
           (nToData) |= (uint32) (pFromData)[22] << 16; \
           (nToData) |= (uint32) (pFromData)[23] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[24] ; \
           (nToData) |= (uint32) (pFromData)[25] << 8; \
           (nToData) |= (uint32) (pFromData)[26] << 16; \
           (nToData) |= (uint32) (pFromData)[27] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV3(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[28] ; \
           (nToData) |= (uint32) (pFromData)[29] << 8; \
           (nToData) |= (uint32) (pFromData)[30] << 16; \
           (nToData) |= (uint32) (pFromData)[31] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV4(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[32] ; \
           (nToData) |= (uint32) (pFromData)[33] << 8; \
           (nToData) |= (uint32) (pFromData)[34] << 16; \
           (nToData) |= (uint32) (pFromData)[35] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV5(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[36] ; \
           (nToData) |= (uint32) (pFromData)[37] << 8; \
           (nToData) |= (uint32) (pFromData)[38] << 16; \
           (nToData) |= (uint32) (pFromData)[39] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV6(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[40] ; \
           (nToData) |= (uint32) (pFromData)[41] << 8; \
           (nToData) |= (uint32) (pFromData)[42] << 16; \
           (nToData) |= (uint32) (pFromData)[43] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV7(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[44] ; \
           (nToData) |= (uint32) (pFromData)[45] << 8; \
           (nToData) |= (uint32) (pFromData)[46] << 16; \
           (nToData) |= (uint32) (pFromData)[47] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV8(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[48] ; \
           (nToData) |= (uint32) (pFromData)[49] << 8; \
           (nToData) |= (uint32) (pFromData)[50] << 16; \
           (nToData) |= (uint32) (pFromData)[51] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV9(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[52] ; \
           (nToData) |= (uint32) (pFromData)[53] << 8; \
           (nToData) |= (uint32) (pFromData)[54] << 16; \
           (nToData) |= (uint32) (pFromData)[55] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV10(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[56] ; \
           (nToData) |= (uint32) (pFromData)[57] << 8; \
           (nToData) |= (uint32) (pFromData)[58] << 16; \
           (nToData) |= (uint32) (pFromData)[59] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV11(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[60] ; \
           (nToData) |= (uint32) (pFromData)[61] << 8; \
           (nToData) |= (uint32) (pFromData)[62] << 16; \
           (nToData) |= (uint32) (pFromData)[63] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV12(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[64] ; \
           (nToData) |= (uint32) (pFromData)[65] << 8; \
           (nToData) |= (uint32) (pFromData)[66] << 16; \
           (nToData) |= (uint32) (pFromData)[67] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV13(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[68] ; \
           (nToData) |= (uint32) (pFromData)[69] << 8; \
           (nToData) |= (uint32) (pFromData)[70] << 16; \
           (nToData) |= (uint32) (pFromData)[71] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV14(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[72] ; \
           (nToData) |= (uint32) (pFromData)[73] << 8; \
           (nToData) |= (uint32) (pFromData)[74] << 16; \
           (nToData) |= (uint32) (pFromData)[75] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV15(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[76] ; \
           (nToData) |= (uint32) (pFromData)[77] << 8; \
           (nToData) |= (uint32) (pFromData)[78] << 16; \
           (nToData) |= (uint32) (pFromData)[79] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV16(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[80] ; \
           (nToData) |= (uint32) (pFromData)[81] << 8; \
           (nToData) |= (uint32) (pFromData)[82] << 16; \
           (nToData) |= (uint32) (pFromData)[83] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV17(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[84] ; \
           (nToData) |= (uint32) (pFromData)[85] << 8; \
           (nToData) |= (uint32) (pFromData)[86] << 16; \
           (nToData) |= (uint32) (pFromData)[87] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV18(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[88] ; \
           (nToData) |= (uint32) (pFromData)[89] << 8; \
           (nToData) |= (uint32) (pFromData)[90] << 16; \
           (nToData) |= (uint32) (pFromData)[91] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV19(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[92] ; \
           (nToData) |= (uint32) (pFromData)[93] << 8; \
           (nToData) |= (uint32) (pFromData)[94] << 16; \
           (nToData) |= (uint32) (pFromData)[95] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV20(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[96] ; \
           (nToData) |= (uint32) (pFromData)[97] << 8; \
           (nToData) |= (uint32) (pFromData)[98] << 16; \
           (nToData) |= (uint32) (pFromData)[99] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV21(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[100] ; \
           (nToData) |= (uint32) (pFromData)[101] << 8; \
           (nToData) |= (uint32) (pFromData)[102] << 16; \
           (nToData) |= (uint32) (pFromData)[103] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV22(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[104] ; \
           (nToData) |= (uint32) (pFromData)[105] << 8; \
           (nToData) |= (uint32) (pFromData)[106] << 16; \
           (nToData) |= (uint32) (pFromData)[107] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV23(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[108] ; \
           (nToData) |= (uint32) (pFromData)[109] << 8; \
           (nToData) |= (uint32) (pFromData)[110] << 16; \
           (nToData) |= (uint32) (pFromData)[111] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV24(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[112] ; \
           (nToData) |= (uint32) (pFromData)[113] << 8; \
           (nToData) |= (uint32) (pFromData)[114] << 16; \
           (nToData) |= (uint32) (pFromData)[115] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV25(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[116] ; \
           (nToData) |= (uint32) (pFromData)[117] << 8; \
           (nToData) |= (uint32) (pFromData)[118] << 16; \
           (nToData) |= (uint32) (pFromData)[119] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV26(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[120] ; \
           (nToData) |= (uint32) (pFromData)[121] << 8; \
           (nToData) |= (uint32) (pFromData)[122] << 16; \
           (nToData) |= (uint32) (pFromData)[123] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_RESV27(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[124] ; \
           (nToData) |= (uint32) (pFromData)[125] << 8; \
           (nToData) |= (uint32) (pFromData)[126] << 16; \
           (nToData) |= (uint32) (pFromData)[127] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB00PATT310(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[128] ; \
           (nToData) |= (uint32) (pFromData)[129] << 8; \
           (nToData) |= (uint32) (pFromData)[130] << 16; \
           (nToData) |= (uint32) (pFromData)[131] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB00PATT6332(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[132] ; \
           (nToData) |= (uint32) (pFromData)[133] << 8; \
           (nToData) |= (uint32) (pFromData)[134] << 16; \
           (nToData) |= (uint32) (pFromData)[135] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB00PATT6495(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[136] ; \
           (nToData) |= (uint32) (pFromData)[137] << 8; \
           (nToData) |= (uint32) (pFromData)[138] << 16; \
           (nToData) |= (uint32) (pFromData)[139] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB00PATT12796(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[140] ; \
           (nToData) |= (uint32) (pFromData)[141] << 8; \
           (nToData) |= (uint32) (pFromData)[142] << 16; \
           (nToData) |= (uint32) (pFromData)[143] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB01PATT310(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[144] ; \
           (nToData) |= (uint32) (pFromData)[145] << 8; \
           (nToData) |= (uint32) (pFromData)[146] << 16; \
           (nToData) |= (uint32) (pFromData)[147] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB01PATT6332(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[148] ; \
           (nToData) |= (uint32) (pFromData)[149] << 8; \
           (nToData) |= (uint32) (pFromData)[150] << 16; \
           (nToData) |= (uint32) (pFromData)[151] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB01PATT6495(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[152] ; \
           (nToData) |= (uint32) (pFromData)[153] << 8; \
           (nToData) |= (uint32) (pFromData)[154] << 16; \
           (nToData) |= (uint32) (pFromData)[155] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB01PATT12796(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[156] ; \
           (nToData) |= (uint32) (pFromData)[157] << 8; \
           (nToData) |= (uint32) (pFromData)[158] << 16; \
           (nToData) |= (uint32) (pFromData)[159] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB02PATT310(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[160] ; \
           (nToData) |= (uint32) (pFromData)[161] << 8; \
           (nToData) |= (uint32) (pFromData)[162] << 16; \
           (nToData) |= (uint32) (pFromData)[163] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB02PATT6332(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[164] ; \
           (nToData) |= (uint32) (pFromData)[165] << 8; \
           (nToData) |= (uint32) (pFromData)[166] << 16; \
           (nToData) |= (uint32) (pFromData)[167] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB02PATT6495(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[168] ; \
           (nToData) |= (uint32) (pFromData)[169] << 8; \
           (nToData) |= (uint32) (pFromData)[170] << 16; \
           (nToData) |= (uint32) (pFromData)[171] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB02PATT12796(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[172] ; \
           (nToData) |= (uint32) (pFromData)[173] << 8; \
           (nToData) |= (uint32) (pFromData)[174] << 16; \
           (nToData) |= (uint32) (pFromData)[175] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB03PATT310(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[176] ; \
           (nToData) |= (uint32) (pFromData)[177] << 8; \
           (nToData) |= (uint32) (pFromData)[178] << 16; \
           (nToData) |= (uint32) (pFromData)[179] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB03PATT6332(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[180] ; \
           (nToData) |= (uint32) (pFromData)[181] << 8; \
           (nToData) |= (uint32) (pFromData)[182] << 16; \
           (nToData) |= (uint32) (pFromData)[183] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB03PATT6495(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[184] ; \
           (nToData) |= (uint32) (pFromData)[185] << 8; \
           (nToData) |= (uint32) (pFromData)[186] << 16; \
           (nToData) |= (uint32) (pFromData)[187] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB03PATT12796(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[188] ; \
           (nToData) |= (uint32) (pFromData)[189] << 8; \
           (nToData) |= (uint32) (pFromData)[190] << 16; \
           (nToData) |= (uint32) (pFromData)[191] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB10PATT310(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[192] ; \
           (nToData) |= (uint32) (pFromData)[193] << 8; \
           (nToData) |= (uint32) (pFromData)[194] << 16; \
           (nToData) |= (uint32) (pFromData)[195] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB10PATT6332(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[196] ; \
           (nToData) |= (uint32) (pFromData)[197] << 8; \
           (nToData) |= (uint32) (pFromData)[198] << 16; \
           (nToData) |= (uint32) (pFromData)[199] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB10PATT6495(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[200] ; \
           (nToData) |= (uint32) (pFromData)[201] << 8; \
           (nToData) |= (uint32) (pFromData)[202] << 16; \
           (nToData) |= (uint32) (pFromData)[203] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB10PATT12796(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[204] ; \
           (nToData) |= (uint32) (pFromData)[205] << 8; \
           (nToData) |= (uint32) (pFromData)[206] << 16; \
           (nToData) |= (uint32) (pFromData)[207] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB11PATT310(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[208] ; \
           (nToData) |= (uint32) (pFromData)[209] << 8; \
           (nToData) |= (uint32) (pFromData)[210] << 16; \
           (nToData) |= (uint32) (pFromData)[211] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB11PATT6332(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[212] ; \
           (nToData) |= (uint32) (pFromData)[213] << 8; \
           (nToData) |= (uint32) (pFromData)[214] << 16; \
           (nToData) |= (uint32) (pFromData)[215] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB11PATT6495(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[216] ; \
           (nToData) |= (uint32) (pFromData)[217] << 8; \
           (nToData) |= (uint32) (pFromData)[218] << 16; \
           (nToData) |= (uint32) (pFromData)[219] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB11PATT12796(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[220] ; \
           (nToData) |= (uint32) (pFromData)[221] << 8; \
           (nToData) |= (uint32) (pFromData)[222] << 16; \
           (nToData) |= (uint32) (pFromData)[223] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB12PATT310(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[224] ; \
           (nToData) |= (uint32) (pFromData)[225] << 8; \
           (nToData) |= (uint32) (pFromData)[226] << 16; \
           (nToData) |= (uint32) (pFromData)[227] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB12PATT6332(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[228] ; \
           (nToData) |= (uint32) (pFromData)[229] << 8; \
           (nToData) |= (uint32) (pFromData)[230] << 16; \
           (nToData) |= (uint32) (pFromData)[231] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB12PATT6495(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[232] ; \
           (nToData) |= (uint32) (pFromData)[233] << 8; \
           (nToData) |= (uint32) (pFromData)[234] << 16; \
           (nToData) |= (uint32) (pFromData)[235] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB12PATT12796(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[236] ; \
           (nToData) |= (uint32) (pFromData)[237] << 8; \
           (nToData) |= (uint32) (pFromData)[238] << 16; \
           (nToData) |= (uint32) (pFromData)[239] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB13PATT310(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[240] ; \
           (nToData) |= (uint32) (pFromData)[241] << 8; \
           (nToData) |= (uint32) (pFromData)[242] << 16; \
           (nToData) |= (uint32) (pFromData)[243] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB13PATT6332(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[244] ; \
           (nToData) |= (uint32) (pFromData)[245] << 8; \
           (nToData) |= (uint32) (pFromData)[246] << 16; \
           (nToData) |= (uint32) (pFromData)[247] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB13PATT6495(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[248] ; \
           (nToData) |= (uint32) (pFromData)[249] << 8; \
           (nToData) |= (uint32) (pFromData)[250] << 16; \
           (nToData) |= (uint32) (pFromData)[251] << 24; \
          } while(0)

#define SB_ZF_FE2000RCDMAFORMAT_GET_SB13PATT12796(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[252] ; \
           (nToData) |= (uint32) (pFromData)[253] << 8; \
           (nToData) |= (uint32) (pFromData)[254] << 16; \
           (nToData) |= (uint32) (pFromData)[255] << 24; \
          } while(0)


uint32 sbZfFe2000RcDmaFormat_Pack32(sbZfFe2000RcDmaFormat_t *pFrom,
                                      uint32 *pToData, 
                                      uint32 uMaxDataLength);

void sbZfFe2000RcDmaBlock2DmaFormat(sbFe2000RcDmaBlock_t *pFrom,
                                    sbZfFe2000RcDmaFormat_t *pTo);

void sbZfFe2000RcDmaFormatAlias(sbZfFe2000RcDmaFormat_t *pFrom,
                                sbFe2000RcDmaFormatAlias_t *pTo);
#endif
