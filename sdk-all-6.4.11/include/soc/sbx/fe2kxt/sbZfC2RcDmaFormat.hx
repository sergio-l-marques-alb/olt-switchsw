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


#ifndef SB_ZF_C2RCDMAFORMAT_H
#define SB_ZF_C2RCDMAFORMAT_H

#define SB_ZF_C2RCDMAFORMAT_SIZE_IN_BYTES 1024
#define SB_ZF_C2RCDMAFORMAT_SIZE 1024
#define SB_ZF_C2RCDMAFORMAT_M_UINSTRUCTION0_BITS "31:0"
#define SB_ZF_C2RCDMAFORMAT_M_UINSTRUCTION1_BITS "63:32"
#define SB_ZF_C2RCDMAFORMAT_M_UINSTRUCTION2_BITS "95:64"
#define SB_ZF_C2RCDMAFORMAT_M_UINSTRUCTION3_BITS "127:96"
#define SB_ZF_C2RCDMAFORMAT_M_UINSTRUCTION4_BITS "159:128"
#define SB_ZF_C2RCDMAFORMAT_M_UINSTRUCTION5_BITS "191:160"
#define SB_ZF_C2RCDMAFORMAT_M_UINSTRUCTION6_BITS "223:192"
#define SB_ZF_C2RCDMAFORMAT_M_UINSTRUCTION7_BITS "255:224"
#define SB_ZF_C2RCDMAFORMAT_M_URESV0_BITS "287:256"
#define SB_ZF_C2RCDMAFORMAT_M_URESV1_BITS "319:288"
#define SB_ZF_C2RCDMAFORMAT_M_URESV2_BITS "351:320"
#define SB_ZF_C2RCDMAFORMAT_M_URESV3_BITS "383:352"
#define SB_ZF_C2RCDMAFORMAT_M_URESV4_BITS "415:384"
#define SB_ZF_C2RCDMAFORMAT_M_URESV5_BITS "447:416"
#define SB_ZF_C2RCDMAFORMAT_M_URESV6_BITS "479:448"
#define SB_ZF_C2RCDMAFORMAT_M_URESV7_BITS "511:480"
#define SB_ZF_C2RCDMAFORMAT_M_URESV8_BITS "543:512"
#define SB_ZF_C2RCDMAFORMAT_M_URESV9_BITS "575:544"
#define SB_ZF_C2RCDMAFORMAT_M_URESV10_BITS "607:576"
#define SB_ZF_C2RCDMAFORMAT_M_URESV11_BITS "639:608"
#define SB_ZF_C2RCDMAFORMAT_M_URESV12_BITS "671:640"
#define SB_ZF_C2RCDMAFORMAT_M_URESV13_BITS "703:672"
#define SB_ZF_C2RCDMAFORMAT_M_URESV14_BITS "735:704"
#define SB_ZF_C2RCDMAFORMAT_M_URESV15_BITS "767:736"
#define SB_ZF_C2RCDMAFORMAT_M_URESV16_BITS "799:768"
#define SB_ZF_C2RCDMAFORMAT_M_URESV17_BITS "831:800"
#define SB_ZF_C2RCDMAFORMAT_M_URESV18_BITS "863:832"
#define SB_ZF_C2RCDMAFORMAT_M_URESV19_BITS "895:864"
#define SB_ZF_C2RCDMAFORMAT_M_URESV20_BITS "927:896"
#define SB_ZF_C2RCDMAFORMAT_M_URESV21_BITS "959:928"
#define SB_ZF_C2RCDMAFORMAT_M_URESV22_BITS "991:960"
#define SB_ZF_C2RCDMAFORMAT_M_URESV23_BITS "1023:992"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN0_31_0_BITS "1055:1024"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN0_63_32_BITS "1087:1056"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN0_95_64_BITS "1119:1088"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN0_127_96_BITS "1151:1120"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN1_31_0_BITS "1183:1152"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN1_63_32_BITS "1215:1184"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN1_95_64_BITS "1247:1216"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN1_127_96_BITS "1279:1248"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN2_31_0_BITS "1311:1280"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN2_63_32_BITS "1343:1312"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN2_95_64_BITS "1375:1344"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN2_127_96_BITS "1407:1376"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN3_31_0_BITS "1439:1408"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN3_63_32_BITS "1471:1440"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN3_95_64_BITS "1503:1472"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN3_127_96_BITS "1535:1504"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN4_31_0_BITS "1567:1536"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN4_63_32_BITS "1599:1568"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN4_95_64_BITS "1631:1600"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN4_127_96_BITS "1663:1632"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN5_31_0_BITS "1695:1664"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN5_63_32_BITS "1727:1696"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN5_95_64_BITS "1759:1728"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN5_127_96_BITS "1791:1760"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN6_31_0_BITS "1823:1792"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN6_63_32_BITS "1855:1824"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN6_95_64_BITS "1887:1856"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN6_127_96_BITS "1919:1888"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN7_31_0_BITS "1951:1920"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN7_63_32_BITS "1983:1952"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN7_95_64_BITS "2015:1984"
#define SB_ZF_C2RCDMAFORMAT_M_USB0PATTERN7_127_96_BITS "2047:2016"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN0_31_0_BITS "2079:2048"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN0_63_32_BITS "2111:2080"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN0_95_64_BITS "2143:2112"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN0_127_96_BITS "2175:2144"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN1_31_0_BITS "2207:2176"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN1_63_32_BITS "2239:2208"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN1_95_64_BITS "2271:2240"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN1_127_96_BITS "2303:2272"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN2_31_0_BITS "2335:2304"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN2_63_32_BITS "2367:2336"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN2_95_64_BITS "2399:2368"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN2_127_96_BITS "2431:2400"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN3_31_0_BITS "2463:2432"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN3_63_32_BITS "2495:2464"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN3_95_64_BITS "2527:2496"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN3_127_96_BITS "2559:2528"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN4_31_0_BITS "2591:2560"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN4_63_32_BITS "2623:2592"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN4_95_64_BITS "2655:2624"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN4_127_96_BITS "2687:2656"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN5_31_0_BITS "2719:2688"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN5_63_32_BITS "2751:2720"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN5_95_64_BITS "2783:2752"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN5_127_96_BITS "2815:2784"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN6_31_0_BITS "2847:2816"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN6_63_32_BITS "2879:2848"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN6_95_64_BITS "2911:2880"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN6_127_96_BITS "2943:2912"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN7_31_0_BITS "2975:2944"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN7_63_32_BITS "3007:2976"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN7_95_64_BITS "3039:3008"
#define SB_ZF_C2RCDMAFORMAT_M_USB1PATTERN7_127_96_BITS "3071:3040"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN0_31_0_BITS "3103:3072"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN0_63_32_BITS "3135:3104"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN0_95_64_BITS "3167:3136"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN0_127_96_BITS "3199:3168"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN1_31_0_BITS "3231:3200"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN1_63_32_BITS "3263:3232"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN1_95_64_BITS "3295:3264"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN1_127_96_BITS "3327:3296"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN2_31_0_BITS "3359:3328"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN2_63_32_BITS "3391:3360"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN2_95_64_BITS "3423:3392"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN2_127_96_BITS "3455:3424"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN3_31_0_BITS "3487:3456"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN3_63_32_BITS "3519:3488"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN3_95_64_BITS "3551:3520"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN3_127_96_BITS "3583:3552"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN4_31_0_BITS "3615:3584"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN4_63_32_BITS "3647:3616"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN4_95_64_BITS "3679:3648"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN4_127_96_BITS "3711:3680"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN5_31_0_BITS "3743:3712"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN5_63_32_BITS "3775:3744"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN5_95_64_BITS "3807:3776"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN5_127_96_BITS "3839:3808"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN6_31_0_BITS "3871:3840"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN6_63_32_BITS "3903:3872"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN6_95_64_BITS "3935:3904"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN6_127_96_BITS "3967:3936"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN7_31_0_BITS "3999:3968"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN7_63_32_BITS "4031:4000"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN7_95_64_BITS "4063:4032"
#define SB_ZF_C2RCDMAFORMAT_M_USB2PATTERN7_127_96_BITS "4095:4064"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN0_31_0_BITS "4127:4096"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN0_63_32_BITS "4159:4128"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN0_95_64_BITS "4191:4160"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN0_127_96_BITS "4223:4192"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN1_31_0_BITS "4255:4224"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN1_63_32_BITS "4287:4256"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN1_95_64_BITS "4319:4288"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN1_127_96_BITS "4351:4320"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN2_31_0_BITS "4383:4352"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN2_63_32_BITS "4415:4384"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN2_95_64_BITS "4447:4416"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN2_127_96_BITS "4479:4448"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN3_31_0_BITS "4511:4480"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN3_63_32_BITS "4543:4512"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN3_95_64_BITS "4575:4544"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN3_127_96_BITS "4607:4576"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN4_31_0_BITS "4639:4608"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN4_63_32_BITS "4671:4640"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN4_95_64_BITS "4703:4672"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN4_127_96_BITS "4735:4704"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN5_31_0_BITS "4767:4736"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN5_63_32_BITS "4799:4768"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN5_95_64_BITS "4831:4800"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN5_127_96_BITS "4863:4832"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN6_31_0_BITS "4895:4864"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN6_63_32_BITS "4927:4896"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN6_95_64_BITS "4959:4928"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN6_127_96_BITS "4991:4960"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN7_31_0_BITS "5023:4992"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN7_63_32_BITS "5055:5024"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN7_95_64_BITS "5087:5056"
#define SB_ZF_C2RCDMAFORMAT_M_USB3PATTERN7_127_96_BITS "5119:5088"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN0_31_0_BITS "5151:5120"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN0_63_32_BITS "5183:5152"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN0_95_64_BITS "5215:5184"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN0_127_96_BITS "5247:5216"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN1_31_0_BITS "5279:5248"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN1_63_32_BITS "5311:5280"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN1_95_64_BITS "5343:5312"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN1_127_96_BITS "5375:5344"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN2_31_0_BITS "5407:5376"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN2_63_32_BITS "5439:5408"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN2_95_64_BITS "5471:5440"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN2_127_96_BITS "5503:5472"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN3_31_0_BITS "5535:5504"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN3_63_32_BITS "5567:5536"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN3_95_64_BITS "5599:5568"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN3_127_96_BITS "5631:5600"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN4_31_0_BITS "5663:5632"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN4_63_32_BITS "5695:5664"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN4_95_64_BITS "5727:5696"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN4_127_96_BITS "5759:5728"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN5_31_0_BITS "5791:5760"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN5_63_32_BITS "5823:5792"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN5_95_64_BITS "5855:5824"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN5_127_96_BITS "5887:5856"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN6_31_0_BITS "5919:5888"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN6_63_32_BITS "5951:5920"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN6_95_64_BITS "5983:5952"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN6_127_96_BITS "6015:5984"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN7_31_0_BITS "6047:6016"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN7_63_32_BITS "6079:6048"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN7_95_64_BITS "6111:6080"
#define SB_ZF_C2RCDMAFORMAT_M_USB4PATTERN7_127_96_BITS "6143:6112"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN0_31_0_BITS "6175:6144"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN0_63_32_BITS "6207:6176"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN0_95_64_BITS "6239:6208"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN0_127_96_BITS "6271:6240"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN1_31_0_BITS "6303:6272"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN1_63_32_BITS "6335:6304"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN1_95_64_BITS "6367:6336"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN1_127_96_BITS "6399:6368"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN2_31_0_BITS "6431:6400"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN2_63_32_BITS "6463:6432"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN2_95_64_BITS "6495:6464"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN2_127_96_BITS "6527:6496"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN3_31_0_BITS "6559:6528"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN3_63_32_BITS "6591:6560"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN3_95_64_BITS "6623:6592"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN3_127_96_BITS "6655:6624"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN4_31_0_BITS "6687:6656"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN4_63_32_BITS "6719:6688"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN4_95_64_BITS "6751:6720"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN4_127_96_BITS "6783:6752"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN5_31_0_BITS "6815:6784"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN5_63_32_BITS "6847:6816"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN5_95_64_BITS "6879:6848"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN5_127_96_BITS "6911:6880"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN6_31_0_BITS "6943:6912"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN6_63_32_BITS "6975:6944"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN6_95_64_BITS "7007:6976"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN6_127_96_BITS "7039:7008"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN7_31_0_BITS "7071:7040"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN7_63_32_BITS "7103:7072"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN7_95_64_BITS "7135:7104"
#define SB_ZF_C2RCDMAFORMAT_M_USB5PATTERN7_127_96_BITS "7167:7136"
#define SB_ZF_C2RCDMAFORMAT_M_URESV24_BITS "7199:7168"
#define SB_ZF_C2RCDMAFORMAT_M_URESV25_BITS "7231:7200"
#define SB_ZF_C2RCDMAFORMAT_M_URESV26_BITS "7263:7232"
#define SB_ZF_C2RCDMAFORMAT_M_URESV27_BITS "7295:7264"
#define SB_ZF_C2RCDMAFORMAT_M_URESV28_BITS "7327:7296"
#define SB_ZF_C2RCDMAFORMAT_M_URESV29_BITS "7359:7328"
#define SB_ZF_C2RCDMAFORMAT_M_URESV30_BITS "7391:7360"
#define SB_ZF_C2RCDMAFORMAT_M_URESV31_BITS "7423:7392"
#define SB_ZF_C2RCDMAFORMAT_M_URESV32_BITS "7455:7424"
#define SB_ZF_C2RCDMAFORMAT_M_URESV33_BITS "7487:7456"
#define SB_ZF_C2RCDMAFORMAT_M_URESV34_BITS "7519:7488"
#define SB_ZF_C2RCDMAFORMAT_M_URESV35_BITS "7551:7520"
#define SB_ZF_C2RCDMAFORMAT_M_URESV36_BITS "7583:7552"
#define SB_ZF_C2RCDMAFORMAT_M_URESV37_BITS "7615:7584"
#define SB_ZF_C2RCDMAFORMAT_M_URESV38_BITS "7647:7616"
#define SB_ZF_C2RCDMAFORMAT_M_URESV39_BITS "7679:7648"
#define SB_ZF_C2RCDMAFORMAT_M_URESV40_BITS "7711:7680"
#define SB_ZF_C2RCDMAFORMAT_M_URESV41_BITS "7743:7712"
#define SB_ZF_C2RCDMAFORMAT_M_URESV42_BITS "7775:7744"
#define SB_ZF_C2RCDMAFORMAT_M_URESV43_BITS "7807:7776"
#define SB_ZF_C2RCDMAFORMAT_M_URESV44_BITS "7839:7808"
#define SB_ZF_C2RCDMAFORMAT_M_URESV45_BITS "7871:7840"
#define SB_ZF_C2RCDMAFORMAT_M_URESV46_BITS "7903:7872"
#define SB_ZF_C2RCDMAFORMAT_M_URESV47_BITS "7935:7904"
#define SB_ZF_C2RCDMAFORMAT_M_URESV48_BITS "7967:7936"
#define SB_ZF_C2RCDMAFORMAT_M_URESV49_BITS "7999:7968"
#define SB_ZF_C2RCDMAFORMAT_M_URESV50_BITS "8031:8000"
#define SB_ZF_C2RCDMAFORMAT_M_URESV51_BITS "8063:8032"
#define SB_ZF_C2RCDMAFORMAT_M_URESV52_BITS "8095:8064"
#define SB_ZF_C2RCDMAFORMAT_M_URESV53_BITS "8127:8096"
#define SB_ZF_C2RCDMAFORMAT_M_URESV54_BITS "8159:8128"
#define SB_ZF_C2RCDMAFORMAT_M_URESV55_BITS "8191:8160"


typedef struct _sbZfC2RcDmaFormat {
  uint32 m_uInstruction0;
  uint32 m_uInstruction1;
  uint32 m_uInstruction2;
  uint32 m_uInstruction3;
  uint32 m_uInstruction4;
  uint32 m_uInstruction5;
  uint32 m_uInstruction6;
  uint32 m_uInstruction7;
  uint32 m_uResv0;
  uint32 m_uResv1;
  uint32 m_uResv2;
  uint32 m_uResv3;
  uint32 m_uResv4;
  uint32 m_uResv5;
  uint32 m_uResv6;
  uint32 m_uResv7;
  uint32 m_uResv8;
  uint32 m_uResv9;
  uint32 m_uResv10;
  uint32 m_uResv11;
  uint32 m_uResv12;
  uint32 m_uResv13;
  uint32 m_uResv14;
  uint32 m_uResv15;
  uint32 m_uResv16;
  uint32 m_uResv17;
  uint32 m_uResv18;
  uint32 m_uResv19;
  uint32 m_uResv20;
  uint32 m_uResv21;
  uint32 m_uResv22;
  uint32 m_uResv23;
  uint32 m_uSb0Pattern0_31_0;
  uint32 m_uSb0Pattern0_63_32;
  uint32 m_uSb0Pattern0_95_64;
  uint32 m_uSb0Pattern0_127_96;
  uint32 m_uSb0Pattern1_31_0;
  uint32 m_uSb0Pattern1_63_32;
  uint32 m_uSb0Pattern1_95_64;
  uint32 m_uSb0Pattern1_127_96;
  uint32 m_uSb0Pattern2_31_0;
  uint32 m_uSb0Pattern2_63_32;
  uint32 m_uSb0Pattern2_95_64;
  uint32 m_uSb0Pattern2_127_96;
  uint32 m_uSb0Pattern3_31_0;
  uint32 m_uSb0Pattern3_63_32;
  uint32 m_uSb0Pattern3_95_64;
  uint32 m_uSb0Pattern3_127_96;
  uint32 m_uSb0Pattern4_31_0;
  uint32 m_uSb0Pattern4_63_32;
  uint32 m_uSb0Pattern4_95_64;
  uint32 m_uSb0Pattern4_127_96;
  uint32 m_uSb0Pattern5_31_0;
  uint32 m_uSb0Pattern5_63_32;
  uint32 m_uSb0Pattern5_95_64;
  uint32 m_uSb0Pattern5_127_96;
  uint32 m_uSb0Pattern6_31_0;
  uint32 m_uSb0Pattern6_63_32;
  uint32 m_uSb0Pattern6_95_64;
  uint32 m_uSb0Pattern6_127_96;
  uint32 m_uSb0Pattern7_31_0;
  uint32 m_uSb0Pattern7_63_32;
  uint32 m_uSb0Pattern7_95_64;
  uint32 m_uSb0Pattern7_127_96;
  uint32 m_uSb1Pattern0_31_0;
  uint32 m_uSb1Pattern0_63_32;
  uint32 m_uSb1Pattern0_95_64;
  uint32 m_uSb1Pattern0_127_96;
  uint32 m_uSb1Pattern1_31_0;
  uint32 m_uSb1Pattern1_63_32;
  uint32 m_uSb1Pattern1_95_64;
  uint32 m_uSb1Pattern1_127_96;
  uint32 m_uSb1Pattern2_31_0;
  uint32 m_uSb1Pattern2_63_32;
  uint32 m_uSb1Pattern2_95_64;
  uint32 m_uSb1Pattern2_127_96;
  uint32 m_uSb1Pattern3_31_0;
  uint32 m_uSb1Pattern3_63_32;
  uint32 m_uSb1Pattern3_95_64;
  uint32 m_uSb1Pattern3_127_96;
  uint32 m_uSb1Pattern4_31_0;
  uint32 m_uSb1Pattern4_63_32;
  uint32 m_uSb1Pattern4_95_64;
  uint32 m_uSb1Pattern4_127_96;
  uint32 m_uSb1Pattern5_31_0;
  uint32 m_uSb1Pattern5_63_32;
  uint32 m_uSb1Pattern5_95_64;
  uint32 m_uSb1Pattern5_127_96;
  uint32 m_uSb1Pattern6_31_0;
  uint32 m_uSb1Pattern6_63_32;
  uint32 m_uSb1Pattern6_95_64;
  uint32 m_uSb1Pattern6_127_96;
  uint32 m_uSb1Pattern7_31_0;
  uint32 m_uSb1Pattern7_63_32;
  uint32 m_uSb1Pattern7_95_64;
  uint32 m_uSb1Pattern7_127_96;
  uint32 m_uSb2Pattern0_31_0;
  uint32 m_uSb2Pattern0_63_32;
  uint32 m_uSb2Pattern0_95_64;
  uint32 m_uSb2Pattern0_127_96;
  uint32 m_uSb2Pattern1_31_0;
  uint32 m_uSb2Pattern1_63_32;
  uint32 m_uSb2Pattern1_95_64;
  uint32 m_uSb2Pattern1_127_96;
  uint32 m_uSb2Pattern2_31_0;
  uint32 m_uSb2Pattern2_63_32;
  uint32 m_uSb2Pattern2_95_64;
  uint32 m_uSb2Pattern2_127_96;
  uint32 m_uSb2Pattern3_31_0;
  uint32 m_uSb2Pattern3_63_32;
  uint32 m_uSb2Pattern3_95_64;
  uint32 m_uSb2Pattern3_127_96;
  uint32 m_uSb2Pattern4_31_0;
  uint32 m_uSb2Pattern4_63_32;
  uint32 m_uSb2Pattern4_95_64;
  uint32 m_uSb2Pattern4_127_96;
  uint32 m_uSb2Pattern5_31_0;
  uint32 m_uSb2Pattern5_63_32;
  uint32 m_uSb2Pattern5_95_64;
  uint32 m_uSb2Pattern5_127_96;
  uint32 m_uSb2Pattern6_31_0;
  uint32 m_uSb2Pattern6_63_32;
  uint32 m_uSb2Pattern6_95_64;
  uint32 m_uSb2Pattern6_127_96;
  uint32 m_uSb2Pattern7_31_0;
  uint32 m_uSb2Pattern7_63_32;
  uint32 m_uSb2Pattern7_95_64;
  uint32 m_uSb2Pattern7_127_96;
  uint32 m_uSb3Pattern0_31_0;
  uint32 m_uSb3Pattern0_63_32;
  uint32 m_uSb3Pattern0_95_64;
  uint32 m_uSb3Pattern0_127_96;
  uint32 m_uSb3Pattern1_31_0;
  uint32 m_uSb3Pattern1_63_32;
  uint32 m_uSb3Pattern1_95_64;
  uint32 m_uSb3Pattern1_127_96;
  uint32 m_uSb3Pattern2_31_0;
  uint32 m_uSb3Pattern2_63_32;
  uint32 m_uSb3Pattern2_95_64;
  uint32 m_uSb3Pattern2_127_96;
  uint32 m_uSb3Pattern3_31_0;
  uint32 m_uSb3Pattern3_63_32;
  uint32 m_uSb3Pattern3_95_64;
  uint32 m_uSb3Pattern3_127_96;
  uint32 m_uSb3Pattern4_31_0;
  uint32 m_uSb3Pattern4_63_32;
  uint32 m_uSb3Pattern4_95_64;
  uint32 m_uSb3Pattern4_127_96;
  uint32 m_uSb3Pattern5_31_0;
  uint32 m_uSb3Pattern5_63_32;
  uint32 m_uSb3Pattern5_95_64;
  uint32 m_uSb3Pattern5_127_96;
  uint32 m_uSb3Pattern6_31_0;
  uint32 m_uSb3Pattern6_63_32;
  uint32 m_uSb3Pattern6_95_64;
  uint32 m_uSb3Pattern6_127_96;
  uint32 m_uSb3Pattern7_31_0;
  uint32 m_uSb3Pattern7_63_32;
  uint32 m_uSb3Pattern7_95_64;
  uint32 m_uSb3Pattern7_127_96;
  uint32 m_uSb4Pattern0_31_0;
  uint32 m_uSb4Pattern0_63_32;
  uint32 m_uSb4Pattern0_95_64;
  uint32 m_uSb4Pattern0_127_96;
  uint32 m_uSb4Pattern1_31_0;
  uint32 m_uSb4Pattern1_63_32;
  uint32 m_uSb4Pattern1_95_64;
  uint32 m_uSb4Pattern1_127_96;
  uint32 m_uSb4Pattern2_31_0;
  uint32 m_uSb4Pattern2_63_32;
  uint32 m_uSb4Pattern2_95_64;
  uint32 m_uSb4Pattern2_127_96;
  uint32 m_uSb4Pattern3_31_0;
  uint32 m_uSb4Pattern3_63_32;
  uint32 m_uSb4Pattern3_95_64;
  uint32 m_uSb4Pattern3_127_96;
  uint32 m_uSb4Pattern4_31_0;
  uint32 m_uSb4Pattern4_63_32;
  uint32 m_uSb4Pattern4_95_64;
  uint32 m_uSb4Pattern4_127_96;
  uint32 m_uSb4Pattern5_31_0;
  uint32 m_uSb4Pattern5_63_32;
  uint32 m_uSb4Pattern5_95_64;
  uint32 m_uSb4Pattern5_127_96;
  uint32 m_uSb4Pattern6_31_0;
  uint32 m_uSb4Pattern6_63_32;
  uint32 m_uSb4Pattern6_95_64;
  uint32 m_uSb4Pattern6_127_96;
  uint32 m_uSb4Pattern7_31_0;
  uint32 m_uSb4Pattern7_63_32;
  uint32 m_uSb4Pattern7_95_64;
  uint32 m_uSb4Pattern7_127_96;
  uint32 m_uSb5Pattern0_31_0;
  uint32 m_uSb5Pattern0_63_32;
  uint32 m_uSb5Pattern0_95_64;
  uint32 m_uSb5Pattern0_127_96;
  uint32 m_uSb5Pattern1_31_0;
  uint32 m_uSb5Pattern1_63_32;
  uint32 m_uSb5Pattern1_95_64;
  uint32 m_uSb5Pattern1_127_96;
  uint32 m_uSb5Pattern2_31_0;
  uint32 m_uSb5Pattern2_63_32;
  uint32 m_uSb5Pattern2_95_64;
  uint32 m_uSb5Pattern2_127_96;
  uint32 m_uSb5Pattern3_31_0;
  uint32 m_uSb5Pattern3_63_32;
  uint32 m_uSb5Pattern3_95_64;
  uint32 m_uSb5Pattern3_127_96;
  uint32 m_uSb5Pattern4_31_0;
  uint32 m_uSb5Pattern4_63_32;
  uint32 m_uSb5Pattern4_95_64;
  uint32 m_uSb5Pattern4_127_96;
  uint32 m_uSb5Pattern5_31_0;
  uint32 m_uSb5Pattern5_63_32;
  uint32 m_uSb5Pattern5_95_64;
  uint32 m_uSb5Pattern5_127_96;
  uint32 m_uSb5Pattern6_31_0;
  uint32 m_uSb5Pattern6_63_32;
  uint32 m_uSb5Pattern6_95_64;
  uint32 m_uSb5Pattern6_127_96;
  uint32 m_uSb5Pattern7_31_0;
  uint32 m_uSb5Pattern7_63_32;
  uint32 m_uSb5Pattern7_95_64;
  uint32 m_uSb5Pattern7_127_96;
  uint32 m_uResv24;
  uint32 m_uResv25;
  uint32 m_uResv26;
  uint32 m_uResv27;
  uint32 m_uResv28;
  uint32 m_uResv29;
  uint32 m_uResv30;
  uint32 m_uResv31;
  uint32 m_uResv32;
  uint32 m_uResv33;
  uint32 m_uResv34;
  uint32 m_uResv35;
  uint32 m_uResv36;
  uint32 m_uResv37;
  uint32 m_uResv38;
  uint32 m_uResv39;
  uint32 m_uResv40;
  uint32 m_uResv41;
  uint32 m_uResv42;
  uint32 m_uResv43;
  uint32 m_uResv44;
  uint32 m_uResv45;
  uint32 m_uResv46;
  uint32 m_uResv47;
  uint32 m_uResv48;
  uint32 m_uResv49;
  uint32 m_uResv50;
  uint32 m_uResv51;
  uint32 m_uResv52;
  uint32 m_uResv53;
  uint32 m_uResv54;
  uint32 m_uResv55;
} sbZfC2RcDmaFormat_t;

uint32
sbZfC2RcDmaFormat_Pack(sbZfC2RcDmaFormat_t *pFrom,
                       uint8 *pToData,
                       uint32 nMaxToDataIndex);
void
sbZfC2RcDmaFormat_Unpack(sbZfC2RcDmaFormat_t *pToStruct,
                         uint8 *pFromData,
                         uint32 nMaxToDataIndex);
void
sbZfC2RcDmaFormat_InitInstance(sbZfC2RcDmaFormat_t *pFrame);

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[3] = ((pToData)[3] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((nFromData)) & 0xFF; \
           (pToData)[5] = ((pToData)[5] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[6] = ((pToData)[6] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[7] = ((pToData)[7] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[8] = ((nFromData)) & 0xFF; \
           (pToData)[9] = ((pToData)[9] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[10] = ((pToData)[10] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[11] = ((pToData)[11] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR3(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[12] = ((nFromData)) & 0xFF; \
           (pToData)[13] = ((pToData)[13] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[14] = ((pToData)[14] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[15] = ((pToData)[15] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR4(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[16] = ((nFromData)) & 0xFF; \
           (pToData)[17] = ((pToData)[17] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[18] = ((pToData)[18] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[19] = ((pToData)[19] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR5(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[20] = ((nFromData)) & 0xFF; \
           (pToData)[21] = ((pToData)[21] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[22] = ((pToData)[22] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[23] = ((pToData)[23] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR6(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[24] = ((nFromData)) & 0xFF; \
           (pToData)[25] = ((pToData)[25] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[26] = ((pToData)[26] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[27] = ((pToData)[27] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR7(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[28] = ((nFromData)) & 0xFF; \
           (pToData)[29] = ((pToData)[29] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[30] = ((pToData)[30] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[31] = ((pToData)[31] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[32] = ((nFromData)) & 0xFF; \
           (pToData)[33] = ((pToData)[33] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[34] = ((pToData)[34] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[35] = ((pToData)[35] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[36] = ((nFromData)) & 0xFF; \
           (pToData)[37] = ((pToData)[37] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[38] = ((pToData)[38] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[39] = ((pToData)[39] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[40] = ((nFromData)) & 0xFF; \
           (pToData)[41] = ((pToData)[41] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[42] = ((pToData)[42] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[43] = ((pToData)[43] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV3(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[44] = ((nFromData)) & 0xFF; \
           (pToData)[45] = ((pToData)[45] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[46] = ((pToData)[46] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[47] = ((pToData)[47] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV4(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[48] = ((nFromData)) & 0xFF; \
           (pToData)[49] = ((pToData)[49] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[50] = ((pToData)[50] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[51] = ((pToData)[51] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV5(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[52] = ((nFromData)) & 0xFF; \
           (pToData)[53] = ((pToData)[53] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[54] = ((pToData)[54] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[55] = ((pToData)[55] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV6(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[56] = ((nFromData)) & 0xFF; \
           (pToData)[57] = ((pToData)[57] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[58] = ((pToData)[58] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[59] = ((pToData)[59] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV7(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[60] = ((nFromData)) & 0xFF; \
           (pToData)[61] = ((pToData)[61] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[62] = ((pToData)[62] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[63] = ((pToData)[63] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV8(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[64] = ((nFromData)) & 0xFF; \
           (pToData)[65] = ((pToData)[65] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[66] = ((pToData)[66] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[67] = ((pToData)[67] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV9(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[68] = ((nFromData)) & 0xFF; \
           (pToData)[69] = ((pToData)[69] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[70] = ((pToData)[70] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[71] = ((pToData)[71] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV10(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[72] = ((nFromData)) & 0xFF; \
           (pToData)[73] = ((pToData)[73] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[74] = ((pToData)[74] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[75] = ((pToData)[75] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV11(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[76] = ((nFromData)) & 0xFF; \
           (pToData)[77] = ((pToData)[77] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[78] = ((pToData)[78] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[79] = ((pToData)[79] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV12(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[80] = ((nFromData)) & 0xFF; \
           (pToData)[81] = ((pToData)[81] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[82] = ((pToData)[82] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[83] = ((pToData)[83] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV13(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[84] = ((nFromData)) & 0xFF; \
           (pToData)[85] = ((pToData)[85] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[86] = ((pToData)[86] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[87] = ((pToData)[87] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV14(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[88] = ((nFromData)) & 0xFF; \
           (pToData)[89] = ((pToData)[89] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[90] = ((pToData)[90] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[91] = ((pToData)[91] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV15(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[92] = ((nFromData)) & 0xFF; \
           (pToData)[93] = ((pToData)[93] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[94] = ((pToData)[94] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[95] = ((pToData)[95] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV16(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[96] = ((nFromData)) & 0xFF; \
           (pToData)[97] = ((pToData)[97] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[98] = ((pToData)[98] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[99] = ((pToData)[99] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV17(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[100] = ((nFromData)) & 0xFF; \
           (pToData)[101] = ((pToData)[101] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[102] = ((pToData)[102] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[103] = ((pToData)[103] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV18(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[104] = ((nFromData)) & 0xFF; \
           (pToData)[105] = ((pToData)[105] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[106] = ((pToData)[106] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[107] = ((pToData)[107] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV19(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[108] = ((nFromData)) & 0xFF; \
           (pToData)[109] = ((pToData)[109] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[110] = ((pToData)[110] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[111] = ((pToData)[111] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV20(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[112] = ((nFromData)) & 0xFF; \
           (pToData)[113] = ((pToData)[113] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[114] = ((pToData)[114] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[115] = ((pToData)[115] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV21(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[116] = ((nFromData)) & 0xFF; \
           (pToData)[117] = ((pToData)[117] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[118] = ((pToData)[118] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[119] = ((pToData)[119] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV22(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[120] = ((nFromData)) & 0xFF; \
           (pToData)[121] = ((pToData)[121] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[122] = ((pToData)[122] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[123] = ((pToData)[123] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV23(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[124] = ((nFromData)) & 0xFF; \
           (pToData)[125] = ((pToData)[125] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[126] = ((pToData)[126] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[127] = ((pToData)[127] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I0PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[128] = ((nFromData)) & 0xFF; \
           (pToData)[129] = ((pToData)[129] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[130] = ((pToData)[130] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[131] = ((pToData)[131] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I0PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[132] = ((nFromData)) & 0xFF; \
           (pToData)[133] = ((pToData)[133] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[134] = ((pToData)[134] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[135] = ((pToData)[135] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I0PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[136] = ((nFromData)) & 0xFF; \
           (pToData)[137] = ((pToData)[137] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[138] = ((pToData)[138] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[139] = ((pToData)[139] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I0PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[140] = ((nFromData)) & 0xFF; \
           (pToData)[141] = ((pToData)[141] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[142] = ((pToData)[142] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[143] = ((pToData)[143] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I1PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[144] = ((nFromData)) & 0xFF; \
           (pToData)[145] = ((pToData)[145] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[146] = ((pToData)[146] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[147] = ((pToData)[147] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I1PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[148] = ((nFromData)) & 0xFF; \
           (pToData)[149] = ((pToData)[149] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[150] = ((pToData)[150] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[151] = ((pToData)[151] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I1PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[152] = ((nFromData)) & 0xFF; \
           (pToData)[153] = ((pToData)[153] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[154] = ((pToData)[154] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[155] = ((pToData)[155] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I1PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[156] = ((nFromData)) & 0xFF; \
           (pToData)[157] = ((pToData)[157] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[158] = ((pToData)[158] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[159] = ((pToData)[159] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I2PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[160] = ((nFromData)) & 0xFF; \
           (pToData)[161] = ((pToData)[161] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[162] = ((pToData)[162] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[163] = ((pToData)[163] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I2PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[164] = ((nFromData)) & 0xFF; \
           (pToData)[165] = ((pToData)[165] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[166] = ((pToData)[166] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[167] = ((pToData)[167] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I2PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[168] = ((nFromData)) & 0xFF; \
           (pToData)[169] = ((pToData)[169] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[170] = ((pToData)[170] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[171] = ((pToData)[171] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I2PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[172] = ((nFromData)) & 0xFF; \
           (pToData)[173] = ((pToData)[173] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[174] = ((pToData)[174] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[175] = ((pToData)[175] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I3PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[176] = ((nFromData)) & 0xFF; \
           (pToData)[177] = ((pToData)[177] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[178] = ((pToData)[178] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[179] = ((pToData)[179] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I3PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[180] = ((nFromData)) & 0xFF; \
           (pToData)[181] = ((pToData)[181] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[182] = ((pToData)[182] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[183] = ((pToData)[183] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I3PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[184] = ((nFromData)) & 0xFF; \
           (pToData)[185] = ((pToData)[185] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[186] = ((pToData)[186] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[187] = ((pToData)[187] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I3PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[188] = ((nFromData)) & 0xFF; \
           (pToData)[189] = ((pToData)[189] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[190] = ((pToData)[190] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[191] = ((pToData)[191] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I4PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[192] = ((nFromData)) & 0xFF; \
           (pToData)[193] = ((pToData)[193] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[194] = ((pToData)[194] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[195] = ((pToData)[195] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I4PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[196] = ((nFromData)) & 0xFF; \
           (pToData)[197] = ((pToData)[197] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[198] = ((pToData)[198] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[199] = ((pToData)[199] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I4PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[200] = ((nFromData)) & 0xFF; \
           (pToData)[201] = ((pToData)[201] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[202] = ((pToData)[202] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[203] = ((pToData)[203] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I4PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[204] = ((nFromData)) & 0xFF; \
           (pToData)[205] = ((pToData)[205] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[206] = ((pToData)[206] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[207] = ((pToData)[207] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I5PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[208] = ((nFromData)) & 0xFF; \
           (pToData)[209] = ((pToData)[209] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[210] = ((pToData)[210] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[211] = ((pToData)[211] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I5PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[212] = ((nFromData)) & 0xFF; \
           (pToData)[213] = ((pToData)[213] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[214] = ((pToData)[214] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[215] = ((pToData)[215] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I5PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[216] = ((nFromData)) & 0xFF; \
           (pToData)[217] = ((pToData)[217] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[218] = ((pToData)[218] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[219] = ((pToData)[219] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I5PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[220] = ((nFromData)) & 0xFF; \
           (pToData)[221] = ((pToData)[221] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[222] = ((pToData)[222] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[223] = ((pToData)[223] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I6PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[224] = ((nFromData)) & 0xFF; \
           (pToData)[225] = ((pToData)[225] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[226] = ((pToData)[226] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[227] = ((pToData)[227] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I6PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[228] = ((nFromData)) & 0xFF; \
           (pToData)[229] = ((pToData)[229] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[230] = ((pToData)[230] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[231] = ((pToData)[231] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I6PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[232] = ((nFromData)) & 0xFF; \
           (pToData)[233] = ((pToData)[233] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[234] = ((pToData)[234] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[235] = ((pToData)[235] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I6PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[236] = ((nFromData)) & 0xFF; \
           (pToData)[237] = ((pToData)[237] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[238] = ((pToData)[238] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[239] = ((pToData)[239] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I7PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[240] = ((nFromData)) & 0xFF; \
           (pToData)[241] = ((pToData)[241] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[242] = ((pToData)[242] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[243] = ((pToData)[243] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I7PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[244] = ((nFromData)) & 0xFF; \
           (pToData)[245] = ((pToData)[245] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[246] = ((pToData)[246] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[247] = ((pToData)[247] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I7PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[248] = ((nFromData)) & 0xFF; \
           (pToData)[249] = ((pToData)[249] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[250] = ((pToData)[250] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[251] = ((pToData)[251] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB0I7PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[252] = ((nFromData)) & 0xFF; \
           (pToData)[253] = ((pToData)[253] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[254] = ((pToData)[254] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[255] = ((pToData)[255] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I0PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[256] = ((nFromData)) & 0xFF; \
           (pToData)[257] = ((pToData)[257] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[258] = ((pToData)[258] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[259] = ((pToData)[259] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I0PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[260] = ((nFromData)) & 0xFF; \
           (pToData)[261] = ((pToData)[261] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[262] = ((pToData)[262] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[263] = ((pToData)[263] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I0PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[264] = ((nFromData)) & 0xFF; \
           (pToData)[265] = ((pToData)[265] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[266] = ((pToData)[266] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[267] = ((pToData)[267] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I0PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[268] = ((nFromData)) & 0xFF; \
           (pToData)[269] = ((pToData)[269] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[270] = ((pToData)[270] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[271] = ((pToData)[271] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I1PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[272] = ((nFromData)) & 0xFF; \
           (pToData)[273] = ((pToData)[273] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[274] = ((pToData)[274] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[275] = ((pToData)[275] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I1PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[276] = ((nFromData)) & 0xFF; \
           (pToData)[277] = ((pToData)[277] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[278] = ((pToData)[278] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[279] = ((pToData)[279] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I1PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[280] = ((nFromData)) & 0xFF; \
           (pToData)[281] = ((pToData)[281] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[282] = ((pToData)[282] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[283] = ((pToData)[283] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I1PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[284] = ((nFromData)) & 0xFF; \
           (pToData)[285] = ((pToData)[285] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[286] = ((pToData)[286] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[287] = ((pToData)[287] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I2PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[288] = ((nFromData)) & 0xFF; \
           (pToData)[289] = ((pToData)[289] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[290] = ((pToData)[290] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[291] = ((pToData)[291] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I2PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[292] = ((nFromData)) & 0xFF; \
           (pToData)[293] = ((pToData)[293] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[294] = ((pToData)[294] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[295] = ((pToData)[295] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I2PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[296] = ((nFromData)) & 0xFF; \
           (pToData)[297] = ((pToData)[297] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[298] = ((pToData)[298] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[299] = ((pToData)[299] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I2PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[300] = ((nFromData)) & 0xFF; \
           (pToData)[301] = ((pToData)[301] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[302] = ((pToData)[302] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[303] = ((pToData)[303] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I3PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[304] = ((nFromData)) & 0xFF; \
           (pToData)[305] = ((pToData)[305] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[306] = ((pToData)[306] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[307] = ((pToData)[307] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I3PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[308] = ((nFromData)) & 0xFF; \
           (pToData)[309] = ((pToData)[309] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[310] = ((pToData)[310] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[311] = ((pToData)[311] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I3PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[312] = ((nFromData)) & 0xFF; \
           (pToData)[313] = ((pToData)[313] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[314] = ((pToData)[314] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[315] = ((pToData)[315] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I3PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[316] = ((nFromData)) & 0xFF; \
           (pToData)[317] = ((pToData)[317] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[318] = ((pToData)[318] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[319] = ((pToData)[319] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I4PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[320] = ((nFromData)) & 0xFF; \
           (pToData)[321] = ((pToData)[321] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[322] = ((pToData)[322] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[323] = ((pToData)[323] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I4PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[324] = ((nFromData)) & 0xFF; \
           (pToData)[325] = ((pToData)[325] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[326] = ((pToData)[326] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[327] = ((pToData)[327] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I4PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[328] = ((nFromData)) & 0xFF; \
           (pToData)[329] = ((pToData)[329] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[330] = ((pToData)[330] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[331] = ((pToData)[331] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I4PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[332] = ((nFromData)) & 0xFF; \
           (pToData)[333] = ((pToData)[333] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[334] = ((pToData)[334] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[335] = ((pToData)[335] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I5PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[336] = ((nFromData)) & 0xFF; \
           (pToData)[337] = ((pToData)[337] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[338] = ((pToData)[338] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[339] = ((pToData)[339] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I5PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[340] = ((nFromData)) & 0xFF; \
           (pToData)[341] = ((pToData)[341] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[342] = ((pToData)[342] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[343] = ((pToData)[343] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I5PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[344] = ((nFromData)) & 0xFF; \
           (pToData)[345] = ((pToData)[345] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[346] = ((pToData)[346] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[347] = ((pToData)[347] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I5PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[348] = ((nFromData)) & 0xFF; \
           (pToData)[349] = ((pToData)[349] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[350] = ((pToData)[350] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[351] = ((pToData)[351] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I6PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[352] = ((nFromData)) & 0xFF; \
           (pToData)[353] = ((pToData)[353] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[354] = ((pToData)[354] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[355] = ((pToData)[355] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I6PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[356] = ((nFromData)) & 0xFF; \
           (pToData)[357] = ((pToData)[357] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[358] = ((pToData)[358] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[359] = ((pToData)[359] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I6PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[360] = ((nFromData)) & 0xFF; \
           (pToData)[361] = ((pToData)[361] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[362] = ((pToData)[362] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[363] = ((pToData)[363] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I6PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[364] = ((nFromData)) & 0xFF; \
           (pToData)[365] = ((pToData)[365] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[366] = ((pToData)[366] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[367] = ((pToData)[367] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I7PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[368] = ((nFromData)) & 0xFF; \
           (pToData)[369] = ((pToData)[369] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[370] = ((pToData)[370] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[371] = ((pToData)[371] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I7PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[372] = ((nFromData)) & 0xFF; \
           (pToData)[373] = ((pToData)[373] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[374] = ((pToData)[374] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[375] = ((pToData)[375] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I7PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[376] = ((nFromData)) & 0xFF; \
           (pToData)[377] = ((pToData)[377] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[378] = ((pToData)[378] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[379] = ((pToData)[379] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB1I7PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[380] = ((nFromData)) & 0xFF; \
           (pToData)[381] = ((pToData)[381] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[382] = ((pToData)[382] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[383] = ((pToData)[383] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I0PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[384] = ((nFromData)) & 0xFF; \
           (pToData)[385] = ((pToData)[385] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[386] = ((pToData)[386] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[387] = ((pToData)[387] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I0PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[388] = ((nFromData)) & 0xFF; \
           (pToData)[389] = ((pToData)[389] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[390] = ((pToData)[390] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[391] = ((pToData)[391] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I0PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[392] = ((nFromData)) & 0xFF; \
           (pToData)[393] = ((pToData)[393] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[394] = ((pToData)[394] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[395] = ((pToData)[395] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I0PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[396] = ((nFromData)) & 0xFF; \
           (pToData)[397] = ((pToData)[397] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[398] = ((pToData)[398] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[399] = ((pToData)[399] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I1PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[400] = ((nFromData)) & 0xFF; \
           (pToData)[401] = ((pToData)[401] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[402] = ((pToData)[402] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[403] = ((pToData)[403] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I1PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[404] = ((nFromData)) & 0xFF; \
           (pToData)[405] = ((pToData)[405] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[406] = ((pToData)[406] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[407] = ((pToData)[407] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I1PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[408] = ((nFromData)) & 0xFF; \
           (pToData)[409] = ((pToData)[409] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[410] = ((pToData)[410] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[411] = ((pToData)[411] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I1PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[412] = ((nFromData)) & 0xFF; \
           (pToData)[413] = ((pToData)[413] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[414] = ((pToData)[414] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[415] = ((pToData)[415] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I2PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[416] = ((nFromData)) & 0xFF; \
           (pToData)[417] = ((pToData)[417] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[418] = ((pToData)[418] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[419] = ((pToData)[419] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I2PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[420] = ((nFromData)) & 0xFF; \
           (pToData)[421] = ((pToData)[421] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[422] = ((pToData)[422] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[423] = ((pToData)[423] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I2PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[424] = ((nFromData)) & 0xFF; \
           (pToData)[425] = ((pToData)[425] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[426] = ((pToData)[426] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[427] = ((pToData)[427] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I2PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[428] = ((nFromData)) & 0xFF; \
           (pToData)[429] = ((pToData)[429] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[430] = ((pToData)[430] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[431] = ((pToData)[431] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I3PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[432] = ((nFromData)) & 0xFF; \
           (pToData)[433] = ((pToData)[433] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[434] = ((pToData)[434] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[435] = ((pToData)[435] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I3PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[436] = ((nFromData)) & 0xFF; \
           (pToData)[437] = ((pToData)[437] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[438] = ((pToData)[438] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[439] = ((pToData)[439] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I3PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[440] = ((nFromData)) & 0xFF; \
           (pToData)[441] = ((pToData)[441] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[442] = ((pToData)[442] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[443] = ((pToData)[443] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I3PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[444] = ((nFromData)) & 0xFF; \
           (pToData)[445] = ((pToData)[445] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[446] = ((pToData)[446] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[447] = ((pToData)[447] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I4PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[448] = ((nFromData)) & 0xFF; \
           (pToData)[449] = ((pToData)[449] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[450] = ((pToData)[450] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[451] = ((pToData)[451] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I4PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[452] = ((nFromData)) & 0xFF; \
           (pToData)[453] = ((pToData)[453] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[454] = ((pToData)[454] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[455] = ((pToData)[455] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I4PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[456] = ((nFromData)) & 0xFF; \
           (pToData)[457] = ((pToData)[457] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[458] = ((pToData)[458] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[459] = ((pToData)[459] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I4PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[460] = ((nFromData)) & 0xFF; \
           (pToData)[461] = ((pToData)[461] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[462] = ((pToData)[462] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[463] = ((pToData)[463] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I5PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[464] = ((nFromData)) & 0xFF; \
           (pToData)[465] = ((pToData)[465] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[466] = ((pToData)[466] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[467] = ((pToData)[467] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I5PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[468] = ((nFromData)) & 0xFF; \
           (pToData)[469] = ((pToData)[469] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[470] = ((pToData)[470] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[471] = ((pToData)[471] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I5PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[472] = ((nFromData)) & 0xFF; \
           (pToData)[473] = ((pToData)[473] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[474] = ((pToData)[474] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[475] = ((pToData)[475] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I5PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[476] = ((nFromData)) & 0xFF; \
           (pToData)[477] = ((pToData)[477] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[478] = ((pToData)[478] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[479] = ((pToData)[479] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I6PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[480] = ((nFromData)) & 0xFF; \
           (pToData)[481] = ((pToData)[481] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[482] = ((pToData)[482] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[483] = ((pToData)[483] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I6PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[484] = ((nFromData)) & 0xFF; \
           (pToData)[485] = ((pToData)[485] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[486] = ((pToData)[486] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[487] = ((pToData)[487] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I6PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[488] = ((nFromData)) & 0xFF; \
           (pToData)[489] = ((pToData)[489] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[490] = ((pToData)[490] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[491] = ((pToData)[491] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I6PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[492] = ((nFromData)) & 0xFF; \
           (pToData)[493] = ((pToData)[493] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[494] = ((pToData)[494] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[495] = ((pToData)[495] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I7PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[496] = ((nFromData)) & 0xFF; \
           (pToData)[497] = ((pToData)[497] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[498] = ((pToData)[498] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[499] = ((pToData)[499] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I7PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[500] = ((nFromData)) & 0xFF; \
           (pToData)[501] = ((pToData)[501] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[502] = ((pToData)[502] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[503] = ((pToData)[503] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I7PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[504] = ((nFromData)) & 0xFF; \
           (pToData)[505] = ((pToData)[505] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[506] = ((pToData)[506] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[507] = ((pToData)[507] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB2I7PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[508] = ((nFromData)) & 0xFF; \
           (pToData)[509] = ((pToData)[509] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[510] = ((pToData)[510] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[511] = ((pToData)[511] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I0PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[512] = ((nFromData)) & 0xFF; \
           (pToData)[513] = ((pToData)[513] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[514] = ((pToData)[514] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[515] = ((pToData)[515] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I0PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[516] = ((nFromData)) & 0xFF; \
           (pToData)[517] = ((pToData)[517] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[518] = ((pToData)[518] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[519] = ((pToData)[519] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I0PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[520] = ((nFromData)) & 0xFF; \
           (pToData)[521] = ((pToData)[521] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[522] = ((pToData)[522] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[523] = ((pToData)[523] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I0PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[524] = ((nFromData)) & 0xFF; \
           (pToData)[525] = ((pToData)[525] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[526] = ((pToData)[526] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[527] = ((pToData)[527] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I1PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[528] = ((nFromData)) & 0xFF; \
           (pToData)[529] = ((pToData)[529] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[530] = ((pToData)[530] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[531] = ((pToData)[531] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I1PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[532] = ((nFromData)) & 0xFF; \
           (pToData)[533] = ((pToData)[533] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[534] = ((pToData)[534] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[535] = ((pToData)[535] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I1PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[536] = ((nFromData)) & 0xFF; \
           (pToData)[537] = ((pToData)[537] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[538] = ((pToData)[538] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[539] = ((pToData)[539] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I1PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[540] = ((nFromData)) & 0xFF; \
           (pToData)[541] = ((pToData)[541] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[542] = ((pToData)[542] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[543] = ((pToData)[543] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I2PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[544] = ((nFromData)) & 0xFF; \
           (pToData)[545] = ((pToData)[545] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[546] = ((pToData)[546] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[547] = ((pToData)[547] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I2PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[548] = ((nFromData)) & 0xFF; \
           (pToData)[549] = ((pToData)[549] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[550] = ((pToData)[550] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[551] = ((pToData)[551] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I2PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[552] = ((nFromData)) & 0xFF; \
           (pToData)[553] = ((pToData)[553] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[554] = ((pToData)[554] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[555] = ((pToData)[555] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I2PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[556] = ((nFromData)) & 0xFF; \
           (pToData)[557] = ((pToData)[557] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[558] = ((pToData)[558] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[559] = ((pToData)[559] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I3PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[560] = ((nFromData)) & 0xFF; \
           (pToData)[561] = ((pToData)[561] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[562] = ((pToData)[562] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[563] = ((pToData)[563] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I3PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[564] = ((nFromData)) & 0xFF; \
           (pToData)[565] = ((pToData)[565] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[566] = ((pToData)[566] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[567] = ((pToData)[567] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I3PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[568] = ((nFromData)) & 0xFF; \
           (pToData)[569] = ((pToData)[569] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[570] = ((pToData)[570] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[571] = ((pToData)[571] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I3PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[572] = ((nFromData)) & 0xFF; \
           (pToData)[573] = ((pToData)[573] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[574] = ((pToData)[574] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[575] = ((pToData)[575] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I4PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[576] = ((nFromData)) & 0xFF; \
           (pToData)[577] = ((pToData)[577] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[578] = ((pToData)[578] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[579] = ((pToData)[579] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I4PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[580] = ((nFromData)) & 0xFF; \
           (pToData)[581] = ((pToData)[581] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[582] = ((pToData)[582] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[583] = ((pToData)[583] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I4PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[584] = ((nFromData)) & 0xFF; \
           (pToData)[585] = ((pToData)[585] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[586] = ((pToData)[586] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[587] = ((pToData)[587] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I4PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[588] = ((nFromData)) & 0xFF; \
           (pToData)[589] = ((pToData)[589] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[590] = ((pToData)[590] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[591] = ((pToData)[591] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I5PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[592] = ((nFromData)) & 0xFF; \
           (pToData)[593] = ((pToData)[593] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[594] = ((pToData)[594] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[595] = ((pToData)[595] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I5PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[596] = ((nFromData)) & 0xFF; \
           (pToData)[597] = ((pToData)[597] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[598] = ((pToData)[598] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[599] = ((pToData)[599] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I5PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[600] = ((nFromData)) & 0xFF; \
           (pToData)[601] = ((pToData)[601] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[602] = ((pToData)[602] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[603] = ((pToData)[603] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I5PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[604] = ((nFromData)) & 0xFF; \
           (pToData)[605] = ((pToData)[605] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[606] = ((pToData)[606] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[607] = ((pToData)[607] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I6PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[608] = ((nFromData)) & 0xFF; \
           (pToData)[609] = ((pToData)[609] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[610] = ((pToData)[610] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[611] = ((pToData)[611] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I6PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[612] = ((nFromData)) & 0xFF; \
           (pToData)[613] = ((pToData)[613] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[614] = ((pToData)[614] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[615] = ((pToData)[615] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I6PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[616] = ((nFromData)) & 0xFF; \
           (pToData)[617] = ((pToData)[617] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[618] = ((pToData)[618] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[619] = ((pToData)[619] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I6PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[620] = ((nFromData)) & 0xFF; \
           (pToData)[621] = ((pToData)[621] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[622] = ((pToData)[622] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[623] = ((pToData)[623] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I7PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[624] = ((nFromData)) & 0xFF; \
           (pToData)[625] = ((pToData)[625] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[626] = ((pToData)[626] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[627] = ((pToData)[627] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I7PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[628] = ((nFromData)) & 0xFF; \
           (pToData)[629] = ((pToData)[629] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[630] = ((pToData)[630] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[631] = ((pToData)[631] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I7PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[632] = ((nFromData)) & 0xFF; \
           (pToData)[633] = ((pToData)[633] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[634] = ((pToData)[634] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[635] = ((pToData)[635] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB3I7PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[636] = ((nFromData)) & 0xFF; \
           (pToData)[637] = ((pToData)[637] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[638] = ((pToData)[638] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[639] = ((pToData)[639] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I0PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[640] = ((nFromData)) & 0xFF; \
           (pToData)[641] = ((pToData)[641] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[642] = ((pToData)[642] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[643] = ((pToData)[643] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I0PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[644] = ((nFromData)) & 0xFF; \
           (pToData)[645] = ((pToData)[645] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[646] = ((pToData)[646] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[647] = ((pToData)[647] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I0PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[648] = ((nFromData)) & 0xFF; \
           (pToData)[649] = ((pToData)[649] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[650] = ((pToData)[650] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[651] = ((pToData)[651] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I0PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[652] = ((nFromData)) & 0xFF; \
           (pToData)[653] = ((pToData)[653] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[654] = ((pToData)[654] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[655] = ((pToData)[655] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I1PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[656] = ((nFromData)) & 0xFF; \
           (pToData)[657] = ((pToData)[657] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[658] = ((pToData)[658] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[659] = ((pToData)[659] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I1PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[660] = ((nFromData)) & 0xFF; \
           (pToData)[661] = ((pToData)[661] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[662] = ((pToData)[662] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[663] = ((pToData)[663] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I1PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[664] = ((nFromData)) & 0xFF; \
           (pToData)[665] = ((pToData)[665] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[666] = ((pToData)[666] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[667] = ((pToData)[667] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I1PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[668] = ((nFromData)) & 0xFF; \
           (pToData)[669] = ((pToData)[669] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[670] = ((pToData)[670] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[671] = ((pToData)[671] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I2PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[672] = ((nFromData)) & 0xFF; \
           (pToData)[673] = ((pToData)[673] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[674] = ((pToData)[674] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[675] = ((pToData)[675] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I2PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[676] = ((nFromData)) & 0xFF; \
           (pToData)[677] = ((pToData)[677] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[678] = ((pToData)[678] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[679] = ((pToData)[679] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I2PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[680] = ((nFromData)) & 0xFF; \
           (pToData)[681] = ((pToData)[681] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[682] = ((pToData)[682] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[683] = ((pToData)[683] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I2PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[684] = ((nFromData)) & 0xFF; \
           (pToData)[685] = ((pToData)[685] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[686] = ((pToData)[686] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[687] = ((pToData)[687] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I3PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[688] = ((nFromData)) & 0xFF; \
           (pToData)[689] = ((pToData)[689] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[690] = ((pToData)[690] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[691] = ((pToData)[691] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I3PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[692] = ((nFromData)) & 0xFF; \
           (pToData)[693] = ((pToData)[693] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[694] = ((pToData)[694] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[695] = ((pToData)[695] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I3PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[696] = ((nFromData)) & 0xFF; \
           (pToData)[697] = ((pToData)[697] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[698] = ((pToData)[698] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[699] = ((pToData)[699] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I3PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[700] = ((nFromData)) & 0xFF; \
           (pToData)[701] = ((pToData)[701] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[702] = ((pToData)[702] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[703] = ((pToData)[703] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I4PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[704] = ((nFromData)) & 0xFF; \
           (pToData)[705] = ((pToData)[705] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[706] = ((pToData)[706] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[707] = ((pToData)[707] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I4PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[708] = ((nFromData)) & 0xFF; \
           (pToData)[709] = ((pToData)[709] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[710] = ((pToData)[710] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[711] = ((pToData)[711] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I4PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[712] = ((nFromData)) & 0xFF; \
           (pToData)[713] = ((pToData)[713] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[714] = ((pToData)[714] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[715] = ((pToData)[715] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I4PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[716] = ((nFromData)) & 0xFF; \
           (pToData)[717] = ((pToData)[717] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[718] = ((pToData)[718] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[719] = ((pToData)[719] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I5PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[720] = ((nFromData)) & 0xFF; \
           (pToData)[721] = ((pToData)[721] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[722] = ((pToData)[722] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[723] = ((pToData)[723] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I5PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[724] = ((nFromData)) & 0xFF; \
           (pToData)[725] = ((pToData)[725] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[726] = ((pToData)[726] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[727] = ((pToData)[727] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I5PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[728] = ((nFromData)) & 0xFF; \
           (pToData)[729] = ((pToData)[729] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[730] = ((pToData)[730] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[731] = ((pToData)[731] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I5PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[732] = ((nFromData)) & 0xFF; \
           (pToData)[733] = ((pToData)[733] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[734] = ((pToData)[734] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[735] = ((pToData)[735] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I6PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[736] = ((nFromData)) & 0xFF; \
           (pToData)[737] = ((pToData)[737] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[738] = ((pToData)[738] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[739] = ((pToData)[739] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I6PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[740] = ((nFromData)) & 0xFF; \
           (pToData)[741] = ((pToData)[741] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[742] = ((pToData)[742] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[743] = ((pToData)[743] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I6PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[744] = ((nFromData)) & 0xFF; \
           (pToData)[745] = ((pToData)[745] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[746] = ((pToData)[746] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[747] = ((pToData)[747] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I6PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[748] = ((nFromData)) & 0xFF; \
           (pToData)[749] = ((pToData)[749] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[750] = ((pToData)[750] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[751] = ((pToData)[751] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I7PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[752] = ((nFromData)) & 0xFF; \
           (pToData)[753] = ((pToData)[753] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[754] = ((pToData)[754] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[755] = ((pToData)[755] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I7PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[756] = ((nFromData)) & 0xFF; \
           (pToData)[757] = ((pToData)[757] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[758] = ((pToData)[758] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[759] = ((pToData)[759] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I7PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[760] = ((nFromData)) & 0xFF; \
           (pToData)[761] = ((pToData)[761] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[762] = ((pToData)[762] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[763] = ((pToData)[763] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB4I7PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[764] = ((nFromData)) & 0xFF; \
           (pToData)[765] = ((pToData)[765] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[766] = ((pToData)[766] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[767] = ((pToData)[767] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I0PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[768] = ((nFromData)) & 0xFF; \
           (pToData)[769] = ((pToData)[769] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[770] = ((pToData)[770] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[771] = ((pToData)[771] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I0PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[772] = ((nFromData)) & 0xFF; \
           (pToData)[773] = ((pToData)[773] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[774] = ((pToData)[774] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[775] = ((pToData)[775] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I0PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[776] = ((nFromData)) & 0xFF; \
           (pToData)[777] = ((pToData)[777] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[778] = ((pToData)[778] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[779] = ((pToData)[779] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I0PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[780] = ((nFromData)) & 0xFF; \
           (pToData)[781] = ((pToData)[781] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[782] = ((pToData)[782] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[783] = ((pToData)[783] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I1PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[784] = ((nFromData)) & 0xFF; \
           (pToData)[785] = ((pToData)[785] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[786] = ((pToData)[786] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[787] = ((pToData)[787] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I1PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[788] = ((nFromData)) & 0xFF; \
           (pToData)[789] = ((pToData)[789] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[790] = ((pToData)[790] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[791] = ((pToData)[791] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I1PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[792] = ((nFromData)) & 0xFF; \
           (pToData)[793] = ((pToData)[793] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[794] = ((pToData)[794] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[795] = ((pToData)[795] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I1PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[796] = ((nFromData)) & 0xFF; \
           (pToData)[797] = ((pToData)[797] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[798] = ((pToData)[798] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[799] = ((pToData)[799] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I2PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[800] = ((nFromData)) & 0xFF; \
           (pToData)[801] = ((pToData)[801] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[802] = ((pToData)[802] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[803] = ((pToData)[803] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I2PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[804] = ((nFromData)) & 0xFF; \
           (pToData)[805] = ((pToData)[805] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[806] = ((pToData)[806] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[807] = ((pToData)[807] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I2PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[808] = ((nFromData)) & 0xFF; \
           (pToData)[809] = ((pToData)[809] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[810] = ((pToData)[810] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[811] = ((pToData)[811] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I2PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[812] = ((nFromData)) & 0xFF; \
           (pToData)[813] = ((pToData)[813] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[814] = ((pToData)[814] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[815] = ((pToData)[815] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I3PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[816] = ((nFromData)) & 0xFF; \
           (pToData)[817] = ((pToData)[817] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[818] = ((pToData)[818] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[819] = ((pToData)[819] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I3PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[820] = ((nFromData)) & 0xFF; \
           (pToData)[821] = ((pToData)[821] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[822] = ((pToData)[822] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[823] = ((pToData)[823] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I3PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[824] = ((nFromData)) & 0xFF; \
           (pToData)[825] = ((pToData)[825] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[826] = ((pToData)[826] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[827] = ((pToData)[827] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I3PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[828] = ((nFromData)) & 0xFF; \
           (pToData)[829] = ((pToData)[829] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[830] = ((pToData)[830] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[831] = ((pToData)[831] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I4PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[832] = ((nFromData)) & 0xFF; \
           (pToData)[833] = ((pToData)[833] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[834] = ((pToData)[834] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[835] = ((pToData)[835] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I4PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[836] = ((nFromData)) & 0xFF; \
           (pToData)[837] = ((pToData)[837] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[838] = ((pToData)[838] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[839] = ((pToData)[839] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I4PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[840] = ((nFromData)) & 0xFF; \
           (pToData)[841] = ((pToData)[841] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[842] = ((pToData)[842] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[843] = ((pToData)[843] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I4PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[844] = ((nFromData)) & 0xFF; \
           (pToData)[845] = ((pToData)[845] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[846] = ((pToData)[846] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[847] = ((pToData)[847] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I5PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[848] = ((nFromData)) & 0xFF; \
           (pToData)[849] = ((pToData)[849] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[850] = ((pToData)[850] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[851] = ((pToData)[851] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I5PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[852] = ((nFromData)) & 0xFF; \
           (pToData)[853] = ((pToData)[853] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[854] = ((pToData)[854] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[855] = ((pToData)[855] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I5PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[856] = ((nFromData)) & 0xFF; \
           (pToData)[857] = ((pToData)[857] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[858] = ((pToData)[858] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[859] = ((pToData)[859] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I5PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[860] = ((nFromData)) & 0xFF; \
           (pToData)[861] = ((pToData)[861] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[862] = ((pToData)[862] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[863] = ((pToData)[863] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I6PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[864] = ((nFromData)) & 0xFF; \
           (pToData)[865] = ((pToData)[865] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[866] = ((pToData)[866] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[867] = ((pToData)[867] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I6PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[868] = ((nFromData)) & 0xFF; \
           (pToData)[869] = ((pToData)[869] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[870] = ((pToData)[870] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[871] = ((pToData)[871] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I6PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[872] = ((nFromData)) & 0xFF; \
           (pToData)[873] = ((pToData)[873] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[874] = ((pToData)[874] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[875] = ((pToData)[875] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I6PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[876] = ((nFromData)) & 0xFF; \
           (pToData)[877] = ((pToData)[877] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[878] = ((pToData)[878] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[879] = ((pToData)[879] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I7PATT31TO0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[880] = ((nFromData)) & 0xFF; \
           (pToData)[881] = ((pToData)[881] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[882] = ((pToData)[882] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[883] = ((pToData)[883] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I7PATT63TO32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[884] = ((nFromData)) & 0xFF; \
           (pToData)[885] = ((pToData)[885] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[886] = ((pToData)[886] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[887] = ((pToData)[887] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I7PATT95TO64(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[888] = ((nFromData)) & 0xFF; \
           (pToData)[889] = ((pToData)[889] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[890] = ((pToData)[890] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[891] = ((pToData)[891] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_SB5I7PATT127TO96(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[892] = ((nFromData)) & 0xFF; \
           (pToData)[893] = ((pToData)[893] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[894] = ((pToData)[894] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[895] = ((pToData)[895] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV24(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[896] = ((nFromData)) & 0xFF; \
           (pToData)[897] = ((pToData)[897] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[898] = ((pToData)[898] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[899] = ((pToData)[899] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV25(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[900] = ((nFromData)) & 0xFF; \
           (pToData)[901] = ((pToData)[901] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[902] = ((pToData)[902] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[903] = ((pToData)[903] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV26(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[904] = ((nFromData)) & 0xFF; \
           (pToData)[905] = ((pToData)[905] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[906] = ((pToData)[906] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[907] = ((pToData)[907] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV27(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[908] = ((nFromData)) & 0xFF; \
           (pToData)[909] = ((pToData)[909] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[910] = ((pToData)[910] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[911] = ((pToData)[911] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV28(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[912] = ((nFromData)) & 0xFF; \
           (pToData)[913] = ((pToData)[913] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[914] = ((pToData)[914] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[915] = ((pToData)[915] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV29(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[916] = ((nFromData)) & 0xFF; \
           (pToData)[917] = ((pToData)[917] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[918] = ((pToData)[918] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[919] = ((pToData)[919] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV30(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[920] = ((nFromData)) & 0xFF; \
           (pToData)[921] = ((pToData)[921] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[922] = ((pToData)[922] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[923] = ((pToData)[923] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV31(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[924] = ((nFromData)) & 0xFF; \
           (pToData)[925] = ((pToData)[925] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[926] = ((pToData)[926] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[927] = ((pToData)[927] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV32(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[928] = ((nFromData)) & 0xFF; \
           (pToData)[929] = ((pToData)[929] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[930] = ((pToData)[930] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[931] = ((pToData)[931] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV33(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[932] = ((nFromData)) & 0xFF; \
           (pToData)[933] = ((pToData)[933] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[934] = ((pToData)[934] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[935] = ((pToData)[935] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV34(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[936] = ((nFromData)) & 0xFF; \
           (pToData)[937] = ((pToData)[937] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[938] = ((pToData)[938] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[939] = ((pToData)[939] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV35(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[940] = ((nFromData)) & 0xFF; \
           (pToData)[941] = ((pToData)[941] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[942] = ((pToData)[942] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[943] = ((pToData)[943] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV36(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[944] = ((nFromData)) & 0xFF; \
           (pToData)[945] = ((pToData)[945] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[946] = ((pToData)[946] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[947] = ((pToData)[947] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV37(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[948] = ((nFromData)) & 0xFF; \
           (pToData)[949] = ((pToData)[949] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[950] = ((pToData)[950] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[951] = ((pToData)[951] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV38(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[952] = ((nFromData)) & 0xFF; \
           (pToData)[953] = ((pToData)[953] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[954] = ((pToData)[954] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[955] = ((pToData)[955] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV39(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[956] = ((nFromData)) & 0xFF; \
           (pToData)[957] = ((pToData)[957] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[958] = ((pToData)[958] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[959] = ((pToData)[959] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV40(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[960] = ((nFromData)) & 0xFF; \
           (pToData)[961] = ((pToData)[961] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[962] = ((pToData)[962] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[963] = ((pToData)[963] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV41(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[964] = ((nFromData)) & 0xFF; \
           (pToData)[965] = ((pToData)[965] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[966] = ((pToData)[966] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[967] = ((pToData)[967] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV42(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[968] = ((nFromData)) & 0xFF; \
           (pToData)[969] = ((pToData)[969] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[970] = ((pToData)[970] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[971] = ((pToData)[971] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV43(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[972] = ((nFromData)) & 0xFF; \
           (pToData)[973] = ((pToData)[973] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[974] = ((pToData)[974] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[975] = ((pToData)[975] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV44(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[976] = ((nFromData)) & 0xFF; \
           (pToData)[977] = ((pToData)[977] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[978] = ((pToData)[978] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[979] = ((pToData)[979] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV45(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[980] = ((nFromData)) & 0xFF; \
           (pToData)[981] = ((pToData)[981] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[982] = ((pToData)[982] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[983] = ((pToData)[983] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV46(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[984] = ((nFromData)) & 0xFF; \
           (pToData)[985] = ((pToData)[985] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[986] = ((pToData)[986] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[987] = ((pToData)[987] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV47(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[988] = ((nFromData)) & 0xFF; \
           (pToData)[989] = ((pToData)[989] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[990] = ((pToData)[990] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[991] = ((pToData)[991] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV48(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[992] = ((nFromData)) & 0xFF; \
           (pToData)[993] = ((pToData)[993] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[994] = ((pToData)[994] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[995] = ((pToData)[995] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV49(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[996] = ((nFromData)) & 0xFF; \
           (pToData)[997] = ((pToData)[997] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[998] = ((pToData)[998] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[999] = ((pToData)[999] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV50(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1000] = ((nFromData)) & 0xFF; \
           (pToData)[1001] = ((pToData)[1001] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[1002] = ((pToData)[1002] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[1003] = ((pToData)[1003] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV51(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1004] = ((nFromData)) & 0xFF; \
           (pToData)[1005] = ((pToData)[1005] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[1006] = ((pToData)[1006] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[1007] = ((pToData)[1007] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV52(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1008] = ((nFromData)) & 0xFF; \
           (pToData)[1009] = ((pToData)[1009] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[1010] = ((pToData)[1010] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[1011] = ((pToData)[1011] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV53(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1012] = ((nFromData)) & 0xFF; \
           (pToData)[1013] = ((pToData)[1013] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[1014] = ((pToData)[1014] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[1015] = ((pToData)[1015] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV54(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1016] = ((nFromData)) & 0xFF; \
           (pToData)[1017] = ((pToData)[1017] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[1018] = ((pToData)[1018] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[1019] = ((pToData)[1019] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_RESV55(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1020] = ((nFromData)) & 0xFF; \
           (pToData)[1021] = ((pToData)[1021] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[1022] = ((pToData)[1022] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[1023] = ((pToData)[1023] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_INSTR0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) (pFromData)[1] << 8; \
           (nToData) |= (uint32) (pFromData)[2] << 16; \
           (nToData) |= (uint32) (pFromData)[3] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_INSTR1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[4] ; \
           (nToData) |= (uint32) (pFromData)[5] << 8; \
           (nToData) |= (uint32) (pFromData)[6] << 16; \
           (nToData) |= (uint32) (pFromData)[7] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_INSTR2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[8] ; \
           (nToData) |= (uint32) (pFromData)[9] << 8; \
           (nToData) |= (uint32) (pFromData)[10] << 16; \
           (nToData) |= (uint32) (pFromData)[11] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_INSTR3(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[12] ; \
           (nToData) |= (uint32) (pFromData)[13] << 8; \
           (nToData) |= (uint32) (pFromData)[14] << 16; \
           (nToData) |= (uint32) (pFromData)[15] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_INSTR4(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[16] ; \
           (nToData) |= (uint32) (pFromData)[17] << 8; \
           (nToData) |= (uint32) (pFromData)[18] << 16; \
           (nToData) |= (uint32) (pFromData)[19] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_INSTR5(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[20] ; \
           (nToData) |= (uint32) (pFromData)[21] << 8; \
           (nToData) |= (uint32) (pFromData)[22] << 16; \
           (nToData) |= (uint32) (pFromData)[23] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_INSTR6(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[24] ; \
           (nToData) |= (uint32) (pFromData)[25] << 8; \
           (nToData) |= (uint32) (pFromData)[26] << 16; \
           (nToData) |= (uint32) (pFromData)[27] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_INSTR7(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[28] ; \
           (nToData) |= (uint32) (pFromData)[29] << 8; \
           (nToData) |= (uint32) (pFromData)[30] << 16; \
           (nToData) |= (uint32) (pFromData)[31] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[32] ; \
           (nToData) |= (uint32) (pFromData)[33] << 8; \
           (nToData) |= (uint32) (pFromData)[34] << 16; \
           (nToData) |= (uint32) (pFromData)[35] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[36] ; \
           (nToData) |= (uint32) (pFromData)[37] << 8; \
           (nToData) |= (uint32) (pFromData)[38] << 16; \
           (nToData) |= (uint32) (pFromData)[39] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[40] ; \
           (nToData) |= (uint32) (pFromData)[41] << 8; \
           (nToData) |= (uint32) (pFromData)[42] << 16; \
           (nToData) |= (uint32) (pFromData)[43] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV3(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[44] ; \
           (nToData) |= (uint32) (pFromData)[45] << 8; \
           (nToData) |= (uint32) (pFromData)[46] << 16; \
           (nToData) |= (uint32) (pFromData)[47] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV4(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[48] ; \
           (nToData) |= (uint32) (pFromData)[49] << 8; \
           (nToData) |= (uint32) (pFromData)[50] << 16; \
           (nToData) |= (uint32) (pFromData)[51] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV5(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[52] ; \
           (nToData) |= (uint32) (pFromData)[53] << 8; \
           (nToData) |= (uint32) (pFromData)[54] << 16; \
           (nToData) |= (uint32) (pFromData)[55] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV6(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[56] ; \
           (nToData) |= (uint32) (pFromData)[57] << 8; \
           (nToData) |= (uint32) (pFromData)[58] << 16; \
           (nToData) |= (uint32) (pFromData)[59] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV7(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[60] ; \
           (nToData) |= (uint32) (pFromData)[61] << 8; \
           (nToData) |= (uint32) (pFromData)[62] << 16; \
           (nToData) |= (uint32) (pFromData)[63] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV8(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[64] ; \
           (nToData) |= (uint32) (pFromData)[65] << 8; \
           (nToData) |= (uint32) (pFromData)[66] << 16; \
           (nToData) |= (uint32) (pFromData)[67] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV9(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[68] ; \
           (nToData) |= (uint32) (pFromData)[69] << 8; \
           (nToData) |= (uint32) (pFromData)[70] << 16; \
           (nToData) |= (uint32) (pFromData)[71] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV10(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[72] ; \
           (nToData) |= (uint32) (pFromData)[73] << 8; \
           (nToData) |= (uint32) (pFromData)[74] << 16; \
           (nToData) |= (uint32) (pFromData)[75] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV11(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[76] ; \
           (nToData) |= (uint32) (pFromData)[77] << 8; \
           (nToData) |= (uint32) (pFromData)[78] << 16; \
           (nToData) |= (uint32) (pFromData)[79] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV12(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[80] ; \
           (nToData) |= (uint32) (pFromData)[81] << 8; \
           (nToData) |= (uint32) (pFromData)[82] << 16; \
           (nToData) |= (uint32) (pFromData)[83] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV13(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[84] ; \
           (nToData) |= (uint32) (pFromData)[85] << 8; \
           (nToData) |= (uint32) (pFromData)[86] << 16; \
           (nToData) |= (uint32) (pFromData)[87] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV14(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[88] ; \
           (nToData) |= (uint32) (pFromData)[89] << 8; \
           (nToData) |= (uint32) (pFromData)[90] << 16; \
           (nToData) |= (uint32) (pFromData)[91] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV15(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[92] ; \
           (nToData) |= (uint32) (pFromData)[93] << 8; \
           (nToData) |= (uint32) (pFromData)[94] << 16; \
           (nToData) |= (uint32) (pFromData)[95] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV16(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[96] ; \
           (nToData) |= (uint32) (pFromData)[97] << 8; \
           (nToData) |= (uint32) (pFromData)[98] << 16; \
           (nToData) |= (uint32) (pFromData)[99] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV17(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[100] ; \
           (nToData) |= (uint32) (pFromData)[101] << 8; \
           (nToData) |= (uint32) (pFromData)[102] << 16; \
           (nToData) |= (uint32) (pFromData)[103] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV18(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[104] ; \
           (nToData) |= (uint32) (pFromData)[105] << 8; \
           (nToData) |= (uint32) (pFromData)[106] << 16; \
           (nToData) |= (uint32) (pFromData)[107] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV19(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[108] ; \
           (nToData) |= (uint32) (pFromData)[109] << 8; \
           (nToData) |= (uint32) (pFromData)[110] << 16; \
           (nToData) |= (uint32) (pFromData)[111] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV20(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[112] ; \
           (nToData) |= (uint32) (pFromData)[113] << 8; \
           (nToData) |= (uint32) (pFromData)[114] << 16; \
           (nToData) |= (uint32) (pFromData)[115] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV21(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[116] ; \
           (nToData) |= (uint32) (pFromData)[117] << 8; \
           (nToData) |= (uint32) (pFromData)[118] << 16; \
           (nToData) |= (uint32) (pFromData)[119] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV22(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[120] ; \
           (nToData) |= (uint32) (pFromData)[121] << 8; \
           (nToData) |= (uint32) (pFromData)[122] << 16; \
           (nToData) |= (uint32) (pFromData)[123] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV23(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[124] ; \
           (nToData) |= (uint32) (pFromData)[125] << 8; \
           (nToData) |= (uint32) (pFromData)[126] << 16; \
           (nToData) |= (uint32) (pFromData)[127] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I0PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[128] ; \
           (nToData) |= (uint32) (pFromData)[129] << 8; \
           (nToData) |= (uint32) (pFromData)[130] << 16; \
           (nToData) |= (uint32) (pFromData)[131] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I0PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[132] ; \
           (nToData) |= (uint32) (pFromData)[133] << 8; \
           (nToData) |= (uint32) (pFromData)[134] << 16; \
           (nToData) |= (uint32) (pFromData)[135] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I0PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[136] ; \
           (nToData) |= (uint32) (pFromData)[137] << 8; \
           (nToData) |= (uint32) (pFromData)[138] << 16; \
           (nToData) |= (uint32) (pFromData)[139] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I0PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[140] ; \
           (nToData) |= (uint32) (pFromData)[141] << 8; \
           (nToData) |= (uint32) (pFromData)[142] << 16; \
           (nToData) |= (uint32) (pFromData)[143] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I1PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[144] ; \
           (nToData) |= (uint32) (pFromData)[145] << 8; \
           (nToData) |= (uint32) (pFromData)[146] << 16; \
           (nToData) |= (uint32) (pFromData)[147] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I1PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[148] ; \
           (nToData) |= (uint32) (pFromData)[149] << 8; \
           (nToData) |= (uint32) (pFromData)[150] << 16; \
           (nToData) |= (uint32) (pFromData)[151] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I1PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[152] ; \
           (nToData) |= (uint32) (pFromData)[153] << 8; \
           (nToData) |= (uint32) (pFromData)[154] << 16; \
           (nToData) |= (uint32) (pFromData)[155] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I1PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[156] ; \
           (nToData) |= (uint32) (pFromData)[157] << 8; \
           (nToData) |= (uint32) (pFromData)[158] << 16; \
           (nToData) |= (uint32) (pFromData)[159] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I2PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[160] ; \
           (nToData) |= (uint32) (pFromData)[161] << 8; \
           (nToData) |= (uint32) (pFromData)[162] << 16; \
           (nToData) |= (uint32) (pFromData)[163] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I2PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[164] ; \
           (nToData) |= (uint32) (pFromData)[165] << 8; \
           (nToData) |= (uint32) (pFromData)[166] << 16; \
           (nToData) |= (uint32) (pFromData)[167] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I2PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[168] ; \
           (nToData) |= (uint32) (pFromData)[169] << 8; \
           (nToData) |= (uint32) (pFromData)[170] << 16; \
           (nToData) |= (uint32) (pFromData)[171] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I2PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[172] ; \
           (nToData) |= (uint32) (pFromData)[173] << 8; \
           (nToData) |= (uint32) (pFromData)[174] << 16; \
           (nToData) |= (uint32) (pFromData)[175] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I3PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[176] ; \
           (nToData) |= (uint32) (pFromData)[177] << 8; \
           (nToData) |= (uint32) (pFromData)[178] << 16; \
           (nToData) |= (uint32) (pFromData)[179] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I3PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[180] ; \
           (nToData) |= (uint32) (pFromData)[181] << 8; \
           (nToData) |= (uint32) (pFromData)[182] << 16; \
           (nToData) |= (uint32) (pFromData)[183] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I3PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[184] ; \
           (nToData) |= (uint32) (pFromData)[185] << 8; \
           (nToData) |= (uint32) (pFromData)[186] << 16; \
           (nToData) |= (uint32) (pFromData)[187] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I3PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[188] ; \
           (nToData) |= (uint32) (pFromData)[189] << 8; \
           (nToData) |= (uint32) (pFromData)[190] << 16; \
           (nToData) |= (uint32) (pFromData)[191] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I4PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[192] ; \
           (nToData) |= (uint32) (pFromData)[193] << 8; \
           (nToData) |= (uint32) (pFromData)[194] << 16; \
           (nToData) |= (uint32) (pFromData)[195] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I4PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[196] ; \
           (nToData) |= (uint32) (pFromData)[197] << 8; \
           (nToData) |= (uint32) (pFromData)[198] << 16; \
           (nToData) |= (uint32) (pFromData)[199] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I4PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[200] ; \
           (nToData) |= (uint32) (pFromData)[201] << 8; \
           (nToData) |= (uint32) (pFromData)[202] << 16; \
           (nToData) |= (uint32) (pFromData)[203] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I4PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[204] ; \
           (nToData) |= (uint32) (pFromData)[205] << 8; \
           (nToData) |= (uint32) (pFromData)[206] << 16; \
           (nToData) |= (uint32) (pFromData)[207] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I5PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[208] ; \
           (nToData) |= (uint32) (pFromData)[209] << 8; \
           (nToData) |= (uint32) (pFromData)[210] << 16; \
           (nToData) |= (uint32) (pFromData)[211] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I5PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[212] ; \
           (nToData) |= (uint32) (pFromData)[213] << 8; \
           (nToData) |= (uint32) (pFromData)[214] << 16; \
           (nToData) |= (uint32) (pFromData)[215] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I5PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[216] ; \
           (nToData) |= (uint32) (pFromData)[217] << 8; \
           (nToData) |= (uint32) (pFromData)[218] << 16; \
           (nToData) |= (uint32) (pFromData)[219] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I5PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[220] ; \
           (nToData) |= (uint32) (pFromData)[221] << 8; \
           (nToData) |= (uint32) (pFromData)[222] << 16; \
           (nToData) |= (uint32) (pFromData)[223] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I6PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[224] ; \
           (nToData) |= (uint32) (pFromData)[225] << 8; \
           (nToData) |= (uint32) (pFromData)[226] << 16; \
           (nToData) |= (uint32) (pFromData)[227] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I6PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[228] ; \
           (nToData) |= (uint32) (pFromData)[229] << 8; \
           (nToData) |= (uint32) (pFromData)[230] << 16; \
           (nToData) |= (uint32) (pFromData)[231] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I6PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[232] ; \
           (nToData) |= (uint32) (pFromData)[233] << 8; \
           (nToData) |= (uint32) (pFromData)[234] << 16; \
           (nToData) |= (uint32) (pFromData)[235] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I6PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[236] ; \
           (nToData) |= (uint32) (pFromData)[237] << 8; \
           (nToData) |= (uint32) (pFromData)[238] << 16; \
           (nToData) |= (uint32) (pFromData)[239] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I7PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[240] ; \
           (nToData) |= (uint32) (pFromData)[241] << 8; \
           (nToData) |= (uint32) (pFromData)[242] << 16; \
           (nToData) |= (uint32) (pFromData)[243] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I7PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[244] ; \
           (nToData) |= (uint32) (pFromData)[245] << 8; \
           (nToData) |= (uint32) (pFromData)[246] << 16; \
           (nToData) |= (uint32) (pFromData)[247] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I7PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[248] ; \
           (nToData) |= (uint32) (pFromData)[249] << 8; \
           (nToData) |= (uint32) (pFromData)[250] << 16; \
           (nToData) |= (uint32) (pFromData)[251] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB0I7PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[252] ; \
           (nToData) |= (uint32) (pFromData)[253] << 8; \
           (nToData) |= (uint32) (pFromData)[254] << 16; \
           (nToData) |= (uint32) (pFromData)[255] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I0PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[256] ; \
           (nToData) |= (uint32) (pFromData)[257] << 8; \
           (nToData) |= (uint32) (pFromData)[258] << 16; \
           (nToData) |= (uint32) (pFromData)[259] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I0PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[260] ; \
           (nToData) |= (uint32) (pFromData)[261] << 8; \
           (nToData) |= (uint32) (pFromData)[262] << 16; \
           (nToData) |= (uint32) (pFromData)[263] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I0PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[264] ; \
           (nToData) |= (uint32) (pFromData)[265] << 8; \
           (nToData) |= (uint32) (pFromData)[266] << 16; \
           (nToData) |= (uint32) (pFromData)[267] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I0PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[268] ; \
           (nToData) |= (uint32) (pFromData)[269] << 8; \
           (nToData) |= (uint32) (pFromData)[270] << 16; \
           (nToData) |= (uint32) (pFromData)[271] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I1PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[272] ; \
           (nToData) |= (uint32) (pFromData)[273] << 8; \
           (nToData) |= (uint32) (pFromData)[274] << 16; \
           (nToData) |= (uint32) (pFromData)[275] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I1PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[276] ; \
           (nToData) |= (uint32) (pFromData)[277] << 8; \
           (nToData) |= (uint32) (pFromData)[278] << 16; \
           (nToData) |= (uint32) (pFromData)[279] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I1PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[280] ; \
           (nToData) |= (uint32) (pFromData)[281] << 8; \
           (nToData) |= (uint32) (pFromData)[282] << 16; \
           (nToData) |= (uint32) (pFromData)[283] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I1PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[284] ; \
           (nToData) |= (uint32) (pFromData)[285] << 8; \
           (nToData) |= (uint32) (pFromData)[286] << 16; \
           (nToData) |= (uint32) (pFromData)[287] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I2PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[288] ; \
           (nToData) |= (uint32) (pFromData)[289] << 8; \
           (nToData) |= (uint32) (pFromData)[290] << 16; \
           (nToData) |= (uint32) (pFromData)[291] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I2PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[292] ; \
           (nToData) |= (uint32) (pFromData)[293] << 8; \
           (nToData) |= (uint32) (pFromData)[294] << 16; \
           (nToData) |= (uint32) (pFromData)[295] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I2PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[296] ; \
           (nToData) |= (uint32) (pFromData)[297] << 8; \
           (nToData) |= (uint32) (pFromData)[298] << 16; \
           (nToData) |= (uint32) (pFromData)[299] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I2PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[300] ; \
           (nToData) |= (uint32) (pFromData)[301] << 8; \
           (nToData) |= (uint32) (pFromData)[302] << 16; \
           (nToData) |= (uint32) (pFromData)[303] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I3PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[304] ; \
           (nToData) |= (uint32) (pFromData)[305] << 8; \
           (nToData) |= (uint32) (pFromData)[306] << 16; \
           (nToData) |= (uint32) (pFromData)[307] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I3PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[308] ; \
           (nToData) |= (uint32) (pFromData)[309] << 8; \
           (nToData) |= (uint32) (pFromData)[310] << 16; \
           (nToData) |= (uint32) (pFromData)[311] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I3PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[312] ; \
           (nToData) |= (uint32) (pFromData)[313] << 8; \
           (nToData) |= (uint32) (pFromData)[314] << 16; \
           (nToData) |= (uint32) (pFromData)[315] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I3PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[316] ; \
           (nToData) |= (uint32) (pFromData)[317] << 8; \
           (nToData) |= (uint32) (pFromData)[318] << 16; \
           (nToData) |= (uint32) (pFromData)[319] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I4PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[320] ; \
           (nToData) |= (uint32) (pFromData)[321] << 8; \
           (nToData) |= (uint32) (pFromData)[322] << 16; \
           (nToData) |= (uint32) (pFromData)[323] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I4PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[324] ; \
           (nToData) |= (uint32) (pFromData)[325] << 8; \
           (nToData) |= (uint32) (pFromData)[326] << 16; \
           (nToData) |= (uint32) (pFromData)[327] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I4PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[328] ; \
           (nToData) |= (uint32) (pFromData)[329] << 8; \
           (nToData) |= (uint32) (pFromData)[330] << 16; \
           (nToData) |= (uint32) (pFromData)[331] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I4PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[332] ; \
           (nToData) |= (uint32) (pFromData)[333] << 8; \
           (nToData) |= (uint32) (pFromData)[334] << 16; \
           (nToData) |= (uint32) (pFromData)[335] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I5PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[336] ; \
           (nToData) |= (uint32) (pFromData)[337] << 8; \
           (nToData) |= (uint32) (pFromData)[338] << 16; \
           (nToData) |= (uint32) (pFromData)[339] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I5PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[340] ; \
           (nToData) |= (uint32) (pFromData)[341] << 8; \
           (nToData) |= (uint32) (pFromData)[342] << 16; \
           (nToData) |= (uint32) (pFromData)[343] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I5PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[344] ; \
           (nToData) |= (uint32) (pFromData)[345] << 8; \
           (nToData) |= (uint32) (pFromData)[346] << 16; \
           (nToData) |= (uint32) (pFromData)[347] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I5PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[348] ; \
           (nToData) |= (uint32) (pFromData)[349] << 8; \
           (nToData) |= (uint32) (pFromData)[350] << 16; \
           (nToData) |= (uint32) (pFromData)[351] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I6PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[352] ; \
           (nToData) |= (uint32) (pFromData)[353] << 8; \
           (nToData) |= (uint32) (pFromData)[354] << 16; \
           (nToData) |= (uint32) (pFromData)[355] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I6PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[356] ; \
           (nToData) |= (uint32) (pFromData)[357] << 8; \
           (nToData) |= (uint32) (pFromData)[358] << 16; \
           (nToData) |= (uint32) (pFromData)[359] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I6PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[360] ; \
           (nToData) |= (uint32) (pFromData)[361] << 8; \
           (nToData) |= (uint32) (pFromData)[362] << 16; \
           (nToData) |= (uint32) (pFromData)[363] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I6PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[364] ; \
           (nToData) |= (uint32) (pFromData)[365] << 8; \
           (nToData) |= (uint32) (pFromData)[366] << 16; \
           (nToData) |= (uint32) (pFromData)[367] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I7PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[368] ; \
           (nToData) |= (uint32) (pFromData)[369] << 8; \
           (nToData) |= (uint32) (pFromData)[370] << 16; \
           (nToData) |= (uint32) (pFromData)[371] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I7PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[372] ; \
           (nToData) |= (uint32) (pFromData)[373] << 8; \
           (nToData) |= (uint32) (pFromData)[374] << 16; \
           (nToData) |= (uint32) (pFromData)[375] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I7PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[376] ; \
           (nToData) |= (uint32) (pFromData)[377] << 8; \
           (nToData) |= (uint32) (pFromData)[378] << 16; \
           (nToData) |= (uint32) (pFromData)[379] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB1I7PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[380] ; \
           (nToData) |= (uint32) (pFromData)[381] << 8; \
           (nToData) |= (uint32) (pFromData)[382] << 16; \
           (nToData) |= (uint32) (pFromData)[383] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I0PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[384] ; \
           (nToData) |= (uint32) (pFromData)[385] << 8; \
           (nToData) |= (uint32) (pFromData)[386] << 16; \
           (nToData) |= (uint32) (pFromData)[387] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I0PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[388] ; \
           (nToData) |= (uint32) (pFromData)[389] << 8; \
           (nToData) |= (uint32) (pFromData)[390] << 16; \
           (nToData) |= (uint32) (pFromData)[391] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I0PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[392] ; \
           (nToData) |= (uint32) (pFromData)[393] << 8; \
           (nToData) |= (uint32) (pFromData)[394] << 16; \
           (nToData) |= (uint32) (pFromData)[395] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I0PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[396] ; \
           (nToData) |= (uint32) (pFromData)[397] << 8; \
           (nToData) |= (uint32) (pFromData)[398] << 16; \
           (nToData) |= (uint32) (pFromData)[399] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I1PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[400] ; \
           (nToData) |= (uint32) (pFromData)[401] << 8; \
           (nToData) |= (uint32) (pFromData)[402] << 16; \
           (nToData) |= (uint32) (pFromData)[403] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I1PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[404] ; \
           (nToData) |= (uint32) (pFromData)[405] << 8; \
           (nToData) |= (uint32) (pFromData)[406] << 16; \
           (nToData) |= (uint32) (pFromData)[407] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I1PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[408] ; \
           (nToData) |= (uint32) (pFromData)[409] << 8; \
           (nToData) |= (uint32) (pFromData)[410] << 16; \
           (nToData) |= (uint32) (pFromData)[411] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I1PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[412] ; \
           (nToData) |= (uint32) (pFromData)[413] << 8; \
           (nToData) |= (uint32) (pFromData)[414] << 16; \
           (nToData) |= (uint32) (pFromData)[415] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I2PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[416] ; \
           (nToData) |= (uint32) (pFromData)[417] << 8; \
           (nToData) |= (uint32) (pFromData)[418] << 16; \
           (nToData) |= (uint32) (pFromData)[419] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I2PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[420] ; \
           (nToData) |= (uint32) (pFromData)[421] << 8; \
           (nToData) |= (uint32) (pFromData)[422] << 16; \
           (nToData) |= (uint32) (pFromData)[423] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I2PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[424] ; \
           (nToData) |= (uint32) (pFromData)[425] << 8; \
           (nToData) |= (uint32) (pFromData)[426] << 16; \
           (nToData) |= (uint32) (pFromData)[427] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I2PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[428] ; \
           (nToData) |= (uint32) (pFromData)[429] << 8; \
           (nToData) |= (uint32) (pFromData)[430] << 16; \
           (nToData) |= (uint32) (pFromData)[431] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I3PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[432] ; \
           (nToData) |= (uint32) (pFromData)[433] << 8; \
           (nToData) |= (uint32) (pFromData)[434] << 16; \
           (nToData) |= (uint32) (pFromData)[435] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I3PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[436] ; \
           (nToData) |= (uint32) (pFromData)[437] << 8; \
           (nToData) |= (uint32) (pFromData)[438] << 16; \
           (nToData) |= (uint32) (pFromData)[439] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I3PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[440] ; \
           (nToData) |= (uint32) (pFromData)[441] << 8; \
           (nToData) |= (uint32) (pFromData)[442] << 16; \
           (nToData) |= (uint32) (pFromData)[443] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I3PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[444] ; \
           (nToData) |= (uint32) (pFromData)[445] << 8; \
           (nToData) |= (uint32) (pFromData)[446] << 16; \
           (nToData) |= (uint32) (pFromData)[447] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I4PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[448] ; \
           (nToData) |= (uint32) (pFromData)[449] << 8; \
           (nToData) |= (uint32) (pFromData)[450] << 16; \
           (nToData) |= (uint32) (pFromData)[451] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I4PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[452] ; \
           (nToData) |= (uint32) (pFromData)[453] << 8; \
           (nToData) |= (uint32) (pFromData)[454] << 16; \
           (nToData) |= (uint32) (pFromData)[455] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I4PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[456] ; \
           (nToData) |= (uint32) (pFromData)[457] << 8; \
           (nToData) |= (uint32) (pFromData)[458] << 16; \
           (nToData) |= (uint32) (pFromData)[459] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I4PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[460] ; \
           (nToData) |= (uint32) (pFromData)[461] << 8; \
           (nToData) |= (uint32) (pFromData)[462] << 16; \
           (nToData) |= (uint32) (pFromData)[463] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I5PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[464] ; \
           (nToData) |= (uint32) (pFromData)[465] << 8; \
           (nToData) |= (uint32) (pFromData)[466] << 16; \
           (nToData) |= (uint32) (pFromData)[467] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I5PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[468] ; \
           (nToData) |= (uint32) (pFromData)[469] << 8; \
           (nToData) |= (uint32) (pFromData)[470] << 16; \
           (nToData) |= (uint32) (pFromData)[471] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I5PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[472] ; \
           (nToData) |= (uint32) (pFromData)[473] << 8; \
           (nToData) |= (uint32) (pFromData)[474] << 16; \
           (nToData) |= (uint32) (pFromData)[475] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I5PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[476] ; \
           (nToData) |= (uint32) (pFromData)[477] << 8; \
           (nToData) |= (uint32) (pFromData)[478] << 16; \
           (nToData) |= (uint32) (pFromData)[479] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I6PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[480] ; \
           (nToData) |= (uint32) (pFromData)[481] << 8; \
           (nToData) |= (uint32) (pFromData)[482] << 16; \
           (nToData) |= (uint32) (pFromData)[483] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I6PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[484] ; \
           (nToData) |= (uint32) (pFromData)[485] << 8; \
           (nToData) |= (uint32) (pFromData)[486] << 16; \
           (nToData) |= (uint32) (pFromData)[487] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I6PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[488] ; \
           (nToData) |= (uint32) (pFromData)[489] << 8; \
           (nToData) |= (uint32) (pFromData)[490] << 16; \
           (nToData) |= (uint32) (pFromData)[491] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I6PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[492] ; \
           (nToData) |= (uint32) (pFromData)[493] << 8; \
           (nToData) |= (uint32) (pFromData)[494] << 16; \
           (nToData) |= (uint32) (pFromData)[495] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I7PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[496] ; \
           (nToData) |= (uint32) (pFromData)[497] << 8; \
           (nToData) |= (uint32) (pFromData)[498] << 16; \
           (nToData) |= (uint32) (pFromData)[499] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I7PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[500] ; \
           (nToData) |= (uint32) (pFromData)[501] << 8; \
           (nToData) |= (uint32) (pFromData)[502] << 16; \
           (nToData) |= (uint32) (pFromData)[503] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I7PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[504] ; \
           (nToData) |= (uint32) (pFromData)[505] << 8; \
           (nToData) |= (uint32) (pFromData)[506] << 16; \
           (nToData) |= (uint32) (pFromData)[507] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB2I7PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[508] ; \
           (nToData) |= (uint32) (pFromData)[509] << 8; \
           (nToData) |= (uint32) (pFromData)[510] << 16; \
           (nToData) |= (uint32) (pFromData)[511] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I0PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[512] ; \
           (nToData) |= (uint32) (pFromData)[513] << 8; \
           (nToData) |= (uint32) (pFromData)[514] << 16; \
           (nToData) |= (uint32) (pFromData)[515] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I0PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[516] ; \
           (nToData) |= (uint32) (pFromData)[517] << 8; \
           (nToData) |= (uint32) (pFromData)[518] << 16; \
           (nToData) |= (uint32) (pFromData)[519] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I0PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[520] ; \
           (nToData) |= (uint32) (pFromData)[521] << 8; \
           (nToData) |= (uint32) (pFromData)[522] << 16; \
           (nToData) |= (uint32) (pFromData)[523] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I0PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[524] ; \
           (nToData) |= (uint32) (pFromData)[525] << 8; \
           (nToData) |= (uint32) (pFromData)[526] << 16; \
           (nToData) |= (uint32) (pFromData)[527] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I1PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[528] ; \
           (nToData) |= (uint32) (pFromData)[529] << 8; \
           (nToData) |= (uint32) (pFromData)[530] << 16; \
           (nToData) |= (uint32) (pFromData)[531] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I1PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[532] ; \
           (nToData) |= (uint32) (pFromData)[533] << 8; \
           (nToData) |= (uint32) (pFromData)[534] << 16; \
           (nToData) |= (uint32) (pFromData)[535] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I1PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[536] ; \
           (nToData) |= (uint32) (pFromData)[537] << 8; \
           (nToData) |= (uint32) (pFromData)[538] << 16; \
           (nToData) |= (uint32) (pFromData)[539] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I1PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[540] ; \
           (nToData) |= (uint32) (pFromData)[541] << 8; \
           (nToData) |= (uint32) (pFromData)[542] << 16; \
           (nToData) |= (uint32) (pFromData)[543] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I2PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[544] ; \
           (nToData) |= (uint32) (pFromData)[545] << 8; \
           (nToData) |= (uint32) (pFromData)[546] << 16; \
           (nToData) |= (uint32) (pFromData)[547] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I2PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[548] ; \
           (nToData) |= (uint32) (pFromData)[549] << 8; \
           (nToData) |= (uint32) (pFromData)[550] << 16; \
           (nToData) |= (uint32) (pFromData)[551] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I2PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[552] ; \
           (nToData) |= (uint32) (pFromData)[553] << 8; \
           (nToData) |= (uint32) (pFromData)[554] << 16; \
           (nToData) |= (uint32) (pFromData)[555] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I2PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[556] ; \
           (nToData) |= (uint32) (pFromData)[557] << 8; \
           (nToData) |= (uint32) (pFromData)[558] << 16; \
           (nToData) |= (uint32) (pFromData)[559] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I3PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[560] ; \
           (nToData) |= (uint32) (pFromData)[561] << 8; \
           (nToData) |= (uint32) (pFromData)[562] << 16; \
           (nToData) |= (uint32) (pFromData)[563] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I3PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[564] ; \
           (nToData) |= (uint32) (pFromData)[565] << 8; \
           (nToData) |= (uint32) (pFromData)[566] << 16; \
           (nToData) |= (uint32) (pFromData)[567] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I3PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[568] ; \
           (nToData) |= (uint32) (pFromData)[569] << 8; \
           (nToData) |= (uint32) (pFromData)[570] << 16; \
           (nToData) |= (uint32) (pFromData)[571] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I3PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[572] ; \
           (nToData) |= (uint32) (pFromData)[573] << 8; \
           (nToData) |= (uint32) (pFromData)[574] << 16; \
           (nToData) |= (uint32) (pFromData)[575] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I4PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[576] ; \
           (nToData) |= (uint32) (pFromData)[577] << 8; \
           (nToData) |= (uint32) (pFromData)[578] << 16; \
           (nToData) |= (uint32) (pFromData)[579] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I4PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[580] ; \
           (nToData) |= (uint32) (pFromData)[581] << 8; \
           (nToData) |= (uint32) (pFromData)[582] << 16; \
           (nToData) |= (uint32) (pFromData)[583] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I4PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[584] ; \
           (nToData) |= (uint32) (pFromData)[585] << 8; \
           (nToData) |= (uint32) (pFromData)[586] << 16; \
           (nToData) |= (uint32) (pFromData)[587] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I4PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[588] ; \
           (nToData) |= (uint32) (pFromData)[589] << 8; \
           (nToData) |= (uint32) (pFromData)[590] << 16; \
           (nToData) |= (uint32) (pFromData)[591] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I5PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[592] ; \
           (nToData) |= (uint32) (pFromData)[593] << 8; \
           (nToData) |= (uint32) (pFromData)[594] << 16; \
           (nToData) |= (uint32) (pFromData)[595] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I5PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[596] ; \
           (nToData) |= (uint32) (pFromData)[597] << 8; \
           (nToData) |= (uint32) (pFromData)[598] << 16; \
           (nToData) |= (uint32) (pFromData)[599] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I5PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[600] ; \
           (nToData) |= (uint32) (pFromData)[601] << 8; \
           (nToData) |= (uint32) (pFromData)[602] << 16; \
           (nToData) |= (uint32) (pFromData)[603] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I5PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[604] ; \
           (nToData) |= (uint32) (pFromData)[605] << 8; \
           (nToData) |= (uint32) (pFromData)[606] << 16; \
           (nToData) |= (uint32) (pFromData)[607] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I6PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[608] ; \
           (nToData) |= (uint32) (pFromData)[609] << 8; \
           (nToData) |= (uint32) (pFromData)[610] << 16; \
           (nToData) |= (uint32) (pFromData)[611] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I6PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[612] ; \
           (nToData) |= (uint32) (pFromData)[613] << 8; \
           (nToData) |= (uint32) (pFromData)[614] << 16; \
           (nToData) |= (uint32) (pFromData)[615] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I6PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[616] ; \
           (nToData) |= (uint32) (pFromData)[617] << 8; \
           (nToData) |= (uint32) (pFromData)[618] << 16; \
           (nToData) |= (uint32) (pFromData)[619] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I6PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[620] ; \
           (nToData) |= (uint32) (pFromData)[621] << 8; \
           (nToData) |= (uint32) (pFromData)[622] << 16; \
           (nToData) |= (uint32) (pFromData)[623] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I7PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[624] ; \
           (nToData) |= (uint32) (pFromData)[625] << 8; \
           (nToData) |= (uint32) (pFromData)[626] << 16; \
           (nToData) |= (uint32) (pFromData)[627] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I7PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[628] ; \
           (nToData) |= (uint32) (pFromData)[629] << 8; \
           (nToData) |= (uint32) (pFromData)[630] << 16; \
           (nToData) |= (uint32) (pFromData)[631] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I7PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[632] ; \
           (nToData) |= (uint32) (pFromData)[633] << 8; \
           (nToData) |= (uint32) (pFromData)[634] << 16; \
           (nToData) |= (uint32) (pFromData)[635] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB3I7PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[636] ; \
           (nToData) |= (uint32) (pFromData)[637] << 8; \
           (nToData) |= (uint32) (pFromData)[638] << 16; \
           (nToData) |= (uint32) (pFromData)[639] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I0PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[640] ; \
           (nToData) |= (uint32) (pFromData)[641] << 8; \
           (nToData) |= (uint32) (pFromData)[642] << 16; \
           (nToData) |= (uint32) (pFromData)[643] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I0PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[644] ; \
           (nToData) |= (uint32) (pFromData)[645] << 8; \
           (nToData) |= (uint32) (pFromData)[646] << 16; \
           (nToData) |= (uint32) (pFromData)[647] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I0PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[648] ; \
           (nToData) |= (uint32) (pFromData)[649] << 8; \
           (nToData) |= (uint32) (pFromData)[650] << 16; \
           (nToData) |= (uint32) (pFromData)[651] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I0PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[652] ; \
           (nToData) |= (uint32) (pFromData)[653] << 8; \
           (nToData) |= (uint32) (pFromData)[654] << 16; \
           (nToData) |= (uint32) (pFromData)[655] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I1PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[656] ; \
           (nToData) |= (uint32) (pFromData)[657] << 8; \
           (nToData) |= (uint32) (pFromData)[658] << 16; \
           (nToData) |= (uint32) (pFromData)[659] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I1PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[660] ; \
           (nToData) |= (uint32) (pFromData)[661] << 8; \
           (nToData) |= (uint32) (pFromData)[662] << 16; \
           (nToData) |= (uint32) (pFromData)[663] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I1PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[664] ; \
           (nToData) |= (uint32) (pFromData)[665] << 8; \
           (nToData) |= (uint32) (pFromData)[666] << 16; \
           (nToData) |= (uint32) (pFromData)[667] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I1PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[668] ; \
           (nToData) |= (uint32) (pFromData)[669] << 8; \
           (nToData) |= (uint32) (pFromData)[670] << 16; \
           (nToData) |= (uint32) (pFromData)[671] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I2PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[672] ; \
           (nToData) |= (uint32) (pFromData)[673] << 8; \
           (nToData) |= (uint32) (pFromData)[674] << 16; \
           (nToData) |= (uint32) (pFromData)[675] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I2PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[676] ; \
           (nToData) |= (uint32) (pFromData)[677] << 8; \
           (nToData) |= (uint32) (pFromData)[678] << 16; \
           (nToData) |= (uint32) (pFromData)[679] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I2PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[680] ; \
           (nToData) |= (uint32) (pFromData)[681] << 8; \
           (nToData) |= (uint32) (pFromData)[682] << 16; \
           (nToData) |= (uint32) (pFromData)[683] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I2PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[684] ; \
           (nToData) |= (uint32) (pFromData)[685] << 8; \
           (nToData) |= (uint32) (pFromData)[686] << 16; \
           (nToData) |= (uint32) (pFromData)[687] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I3PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[688] ; \
           (nToData) |= (uint32) (pFromData)[689] << 8; \
           (nToData) |= (uint32) (pFromData)[690] << 16; \
           (nToData) |= (uint32) (pFromData)[691] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I3PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[692] ; \
           (nToData) |= (uint32) (pFromData)[693] << 8; \
           (nToData) |= (uint32) (pFromData)[694] << 16; \
           (nToData) |= (uint32) (pFromData)[695] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I3PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[696] ; \
           (nToData) |= (uint32) (pFromData)[697] << 8; \
           (nToData) |= (uint32) (pFromData)[698] << 16; \
           (nToData) |= (uint32) (pFromData)[699] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I3PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[700] ; \
           (nToData) |= (uint32) (pFromData)[701] << 8; \
           (nToData) |= (uint32) (pFromData)[702] << 16; \
           (nToData) |= (uint32) (pFromData)[703] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I4PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[704] ; \
           (nToData) |= (uint32) (pFromData)[705] << 8; \
           (nToData) |= (uint32) (pFromData)[706] << 16; \
           (nToData) |= (uint32) (pFromData)[707] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I4PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[708] ; \
           (nToData) |= (uint32) (pFromData)[709] << 8; \
           (nToData) |= (uint32) (pFromData)[710] << 16; \
           (nToData) |= (uint32) (pFromData)[711] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I4PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[712] ; \
           (nToData) |= (uint32) (pFromData)[713] << 8; \
           (nToData) |= (uint32) (pFromData)[714] << 16; \
           (nToData) |= (uint32) (pFromData)[715] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I4PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[716] ; \
           (nToData) |= (uint32) (pFromData)[717] << 8; \
           (nToData) |= (uint32) (pFromData)[718] << 16; \
           (nToData) |= (uint32) (pFromData)[719] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I5PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[720] ; \
           (nToData) |= (uint32) (pFromData)[721] << 8; \
           (nToData) |= (uint32) (pFromData)[722] << 16; \
           (nToData) |= (uint32) (pFromData)[723] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I5PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[724] ; \
           (nToData) |= (uint32) (pFromData)[725] << 8; \
           (nToData) |= (uint32) (pFromData)[726] << 16; \
           (nToData) |= (uint32) (pFromData)[727] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I5PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[728] ; \
           (nToData) |= (uint32) (pFromData)[729] << 8; \
           (nToData) |= (uint32) (pFromData)[730] << 16; \
           (nToData) |= (uint32) (pFromData)[731] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I5PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[732] ; \
           (nToData) |= (uint32) (pFromData)[733] << 8; \
           (nToData) |= (uint32) (pFromData)[734] << 16; \
           (nToData) |= (uint32) (pFromData)[735] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I6PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[736] ; \
           (nToData) |= (uint32) (pFromData)[737] << 8; \
           (nToData) |= (uint32) (pFromData)[738] << 16; \
           (nToData) |= (uint32) (pFromData)[739] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I6PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[740] ; \
           (nToData) |= (uint32) (pFromData)[741] << 8; \
           (nToData) |= (uint32) (pFromData)[742] << 16; \
           (nToData) |= (uint32) (pFromData)[743] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I6PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[744] ; \
           (nToData) |= (uint32) (pFromData)[745] << 8; \
           (nToData) |= (uint32) (pFromData)[746] << 16; \
           (nToData) |= (uint32) (pFromData)[747] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I6PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[748] ; \
           (nToData) |= (uint32) (pFromData)[749] << 8; \
           (nToData) |= (uint32) (pFromData)[750] << 16; \
           (nToData) |= (uint32) (pFromData)[751] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I7PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[752] ; \
           (nToData) |= (uint32) (pFromData)[753] << 8; \
           (nToData) |= (uint32) (pFromData)[754] << 16; \
           (nToData) |= (uint32) (pFromData)[755] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I7PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[756] ; \
           (nToData) |= (uint32) (pFromData)[757] << 8; \
           (nToData) |= (uint32) (pFromData)[758] << 16; \
           (nToData) |= (uint32) (pFromData)[759] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I7PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[760] ; \
           (nToData) |= (uint32) (pFromData)[761] << 8; \
           (nToData) |= (uint32) (pFromData)[762] << 16; \
           (nToData) |= (uint32) (pFromData)[763] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB4I7PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[764] ; \
           (nToData) |= (uint32) (pFromData)[765] << 8; \
           (nToData) |= (uint32) (pFromData)[766] << 16; \
           (nToData) |= (uint32) (pFromData)[767] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I0PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[768] ; \
           (nToData) |= (uint32) (pFromData)[769] << 8; \
           (nToData) |= (uint32) (pFromData)[770] << 16; \
           (nToData) |= (uint32) (pFromData)[771] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I0PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[772] ; \
           (nToData) |= (uint32) (pFromData)[773] << 8; \
           (nToData) |= (uint32) (pFromData)[774] << 16; \
           (nToData) |= (uint32) (pFromData)[775] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I0PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[776] ; \
           (nToData) |= (uint32) (pFromData)[777] << 8; \
           (nToData) |= (uint32) (pFromData)[778] << 16; \
           (nToData) |= (uint32) (pFromData)[779] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I0PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[780] ; \
           (nToData) |= (uint32) (pFromData)[781] << 8; \
           (nToData) |= (uint32) (pFromData)[782] << 16; \
           (nToData) |= (uint32) (pFromData)[783] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I1PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[784] ; \
           (nToData) |= (uint32) (pFromData)[785] << 8; \
           (nToData) |= (uint32) (pFromData)[786] << 16; \
           (nToData) |= (uint32) (pFromData)[787] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I1PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[788] ; \
           (nToData) |= (uint32) (pFromData)[789] << 8; \
           (nToData) |= (uint32) (pFromData)[790] << 16; \
           (nToData) |= (uint32) (pFromData)[791] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I1PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[792] ; \
           (nToData) |= (uint32) (pFromData)[793] << 8; \
           (nToData) |= (uint32) (pFromData)[794] << 16; \
           (nToData) |= (uint32) (pFromData)[795] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I1PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[796] ; \
           (nToData) |= (uint32) (pFromData)[797] << 8; \
           (nToData) |= (uint32) (pFromData)[798] << 16; \
           (nToData) |= (uint32) (pFromData)[799] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I2PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[800] ; \
           (nToData) |= (uint32) (pFromData)[801] << 8; \
           (nToData) |= (uint32) (pFromData)[802] << 16; \
           (nToData) |= (uint32) (pFromData)[803] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I2PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[804] ; \
           (nToData) |= (uint32) (pFromData)[805] << 8; \
           (nToData) |= (uint32) (pFromData)[806] << 16; \
           (nToData) |= (uint32) (pFromData)[807] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I2PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[808] ; \
           (nToData) |= (uint32) (pFromData)[809] << 8; \
           (nToData) |= (uint32) (pFromData)[810] << 16; \
           (nToData) |= (uint32) (pFromData)[811] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I2PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[812] ; \
           (nToData) |= (uint32) (pFromData)[813] << 8; \
           (nToData) |= (uint32) (pFromData)[814] << 16; \
           (nToData) |= (uint32) (pFromData)[815] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I3PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[816] ; \
           (nToData) |= (uint32) (pFromData)[817] << 8; \
           (nToData) |= (uint32) (pFromData)[818] << 16; \
           (nToData) |= (uint32) (pFromData)[819] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I3PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[820] ; \
           (nToData) |= (uint32) (pFromData)[821] << 8; \
           (nToData) |= (uint32) (pFromData)[822] << 16; \
           (nToData) |= (uint32) (pFromData)[823] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I3PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[824] ; \
           (nToData) |= (uint32) (pFromData)[825] << 8; \
           (nToData) |= (uint32) (pFromData)[826] << 16; \
           (nToData) |= (uint32) (pFromData)[827] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I3PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[828] ; \
           (nToData) |= (uint32) (pFromData)[829] << 8; \
           (nToData) |= (uint32) (pFromData)[830] << 16; \
           (nToData) |= (uint32) (pFromData)[831] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I4PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[832] ; \
           (nToData) |= (uint32) (pFromData)[833] << 8; \
           (nToData) |= (uint32) (pFromData)[834] << 16; \
           (nToData) |= (uint32) (pFromData)[835] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I4PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[836] ; \
           (nToData) |= (uint32) (pFromData)[837] << 8; \
           (nToData) |= (uint32) (pFromData)[838] << 16; \
           (nToData) |= (uint32) (pFromData)[839] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I4PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[840] ; \
           (nToData) |= (uint32) (pFromData)[841] << 8; \
           (nToData) |= (uint32) (pFromData)[842] << 16; \
           (nToData) |= (uint32) (pFromData)[843] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I4PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[844] ; \
           (nToData) |= (uint32) (pFromData)[845] << 8; \
           (nToData) |= (uint32) (pFromData)[846] << 16; \
           (nToData) |= (uint32) (pFromData)[847] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I5PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[848] ; \
           (nToData) |= (uint32) (pFromData)[849] << 8; \
           (nToData) |= (uint32) (pFromData)[850] << 16; \
           (nToData) |= (uint32) (pFromData)[851] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I5PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[852] ; \
           (nToData) |= (uint32) (pFromData)[853] << 8; \
           (nToData) |= (uint32) (pFromData)[854] << 16; \
           (nToData) |= (uint32) (pFromData)[855] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I5PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[856] ; \
           (nToData) |= (uint32) (pFromData)[857] << 8; \
           (nToData) |= (uint32) (pFromData)[858] << 16; \
           (nToData) |= (uint32) (pFromData)[859] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I5PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[860] ; \
           (nToData) |= (uint32) (pFromData)[861] << 8; \
           (nToData) |= (uint32) (pFromData)[862] << 16; \
           (nToData) |= (uint32) (pFromData)[863] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I6PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[864] ; \
           (nToData) |= (uint32) (pFromData)[865] << 8; \
           (nToData) |= (uint32) (pFromData)[866] << 16; \
           (nToData) |= (uint32) (pFromData)[867] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I6PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[868] ; \
           (nToData) |= (uint32) (pFromData)[869] << 8; \
           (nToData) |= (uint32) (pFromData)[870] << 16; \
           (nToData) |= (uint32) (pFromData)[871] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I6PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[872] ; \
           (nToData) |= (uint32) (pFromData)[873] << 8; \
           (nToData) |= (uint32) (pFromData)[874] << 16; \
           (nToData) |= (uint32) (pFromData)[875] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I6PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[876] ; \
           (nToData) |= (uint32) (pFromData)[877] << 8; \
           (nToData) |= (uint32) (pFromData)[878] << 16; \
           (nToData) |= (uint32) (pFromData)[879] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I7PATT31TO0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[880] ; \
           (nToData) |= (uint32) (pFromData)[881] << 8; \
           (nToData) |= (uint32) (pFromData)[882] << 16; \
           (nToData) |= (uint32) (pFromData)[883] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I7PATT63TO32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[884] ; \
           (nToData) |= (uint32) (pFromData)[885] << 8; \
           (nToData) |= (uint32) (pFromData)[886] << 16; \
           (nToData) |= (uint32) (pFromData)[887] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I7PATT95TO64(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[888] ; \
           (nToData) |= (uint32) (pFromData)[889] << 8; \
           (nToData) |= (uint32) (pFromData)[890] << 16; \
           (nToData) |= (uint32) (pFromData)[891] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_SB5I7PATT127TO96(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[892] ; \
           (nToData) |= (uint32) (pFromData)[893] << 8; \
           (nToData) |= (uint32) (pFromData)[894] << 16; \
           (nToData) |= (uint32) (pFromData)[895] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV24(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[896] ; \
           (nToData) |= (uint32) (pFromData)[897] << 8; \
           (nToData) |= (uint32) (pFromData)[898] << 16; \
           (nToData) |= (uint32) (pFromData)[899] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV25(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[900] ; \
           (nToData) |= (uint32) (pFromData)[901] << 8; \
           (nToData) |= (uint32) (pFromData)[902] << 16; \
           (nToData) |= (uint32) (pFromData)[903] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV26(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[904] ; \
           (nToData) |= (uint32) (pFromData)[905] << 8; \
           (nToData) |= (uint32) (pFromData)[906] << 16; \
           (nToData) |= (uint32) (pFromData)[907] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV27(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[908] ; \
           (nToData) |= (uint32) (pFromData)[909] << 8; \
           (nToData) |= (uint32) (pFromData)[910] << 16; \
           (nToData) |= (uint32) (pFromData)[911] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV28(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[912] ; \
           (nToData) |= (uint32) (pFromData)[913] << 8; \
           (nToData) |= (uint32) (pFromData)[914] << 16; \
           (nToData) |= (uint32) (pFromData)[915] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV29(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[916] ; \
           (nToData) |= (uint32) (pFromData)[917] << 8; \
           (nToData) |= (uint32) (pFromData)[918] << 16; \
           (nToData) |= (uint32) (pFromData)[919] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV30(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[920] ; \
           (nToData) |= (uint32) (pFromData)[921] << 8; \
           (nToData) |= (uint32) (pFromData)[922] << 16; \
           (nToData) |= (uint32) (pFromData)[923] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV31(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[924] ; \
           (nToData) |= (uint32) (pFromData)[925] << 8; \
           (nToData) |= (uint32) (pFromData)[926] << 16; \
           (nToData) |= (uint32) (pFromData)[927] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV32(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[928] ; \
           (nToData) |= (uint32) (pFromData)[929] << 8; \
           (nToData) |= (uint32) (pFromData)[930] << 16; \
           (nToData) |= (uint32) (pFromData)[931] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV33(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[932] ; \
           (nToData) |= (uint32) (pFromData)[933] << 8; \
           (nToData) |= (uint32) (pFromData)[934] << 16; \
           (nToData) |= (uint32) (pFromData)[935] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV34(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[936] ; \
           (nToData) |= (uint32) (pFromData)[937] << 8; \
           (nToData) |= (uint32) (pFromData)[938] << 16; \
           (nToData) |= (uint32) (pFromData)[939] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV35(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[940] ; \
           (nToData) |= (uint32) (pFromData)[941] << 8; \
           (nToData) |= (uint32) (pFromData)[942] << 16; \
           (nToData) |= (uint32) (pFromData)[943] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV36(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[944] ; \
           (nToData) |= (uint32) (pFromData)[945] << 8; \
           (nToData) |= (uint32) (pFromData)[946] << 16; \
           (nToData) |= (uint32) (pFromData)[947] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV37(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[948] ; \
           (nToData) |= (uint32) (pFromData)[949] << 8; \
           (nToData) |= (uint32) (pFromData)[950] << 16; \
           (nToData) |= (uint32) (pFromData)[951] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV38(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[952] ; \
           (nToData) |= (uint32) (pFromData)[953] << 8; \
           (nToData) |= (uint32) (pFromData)[954] << 16; \
           (nToData) |= (uint32) (pFromData)[955] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV39(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[956] ; \
           (nToData) |= (uint32) (pFromData)[957] << 8; \
           (nToData) |= (uint32) (pFromData)[958] << 16; \
           (nToData) |= (uint32) (pFromData)[959] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV40(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[960] ; \
           (nToData) |= (uint32) (pFromData)[961] << 8; \
           (nToData) |= (uint32) (pFromData)[962] << 16; \
           (nToData) |= (uint32) (pFromData)[963] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV41(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[964] ; \
           (nToData) |= (uint32) (pFromData)[965] << 8; \
           (nToData) |= (uint32) (pFromData)[966] << 16; \
           (nToData) |= (uint32) (pFromData)[967] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV42(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[968] ; \
           (nToData) |= (uint32) (pFromData)[969] << 8; \
           (nToData) |= (uint32) (pFromData)[970] << 16; \
           (nToData) |= (uint32) (pFromData)[971] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV43(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[972] ; \
           (nToData) |= (uint32) (pFromData)[973] << 8; \
           (nToData) |= (uint32) (pFromData)[974] << 16; \
           (nToData) |= (uint32) (pFromData)[975] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV44(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[976] ; \
           (nToData) |= (uint32) (pFromData)[977] << 8; \
           (nToData) |= (uint32) (pFromData)[978] << 16; \
           (nToData) |= (uint32) (pFromData)[979] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV45(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[980] ; \
           (nToData) |= (uint32) (pFromData)[981] << 8; \
           (nToData) |= (uint32) (pFromData)[982] << 16; \
           (nToData) |= (uint32) (pFromData)[983] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV46(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[984] ; \
           (nToData) |= (uint32) (pFromData)[985] << 8; \
           (nToData) |= (uint32) (pFromData)[986] << 16; \
           (nToData) |= (uint32) (pFromData)[987] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV47(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[988] ; \
           (nToData) |= (uint32) (pFromData)[989] << 8; \
           (nToData) |= (uint32) (pFromData)[990] << 16; \
           (nToData) |= (uint32) (pFromData)[991] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV48(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[992] ; \
           (nToData) |= (uint32) (pFromData)[993] << 8; \
           (nToData) |= (uint32) (pFromData)[994] << 16; \
           (nToData) |= (uint32) (pFromData)[995] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV49(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[996] ; \
           (nToData) |= (uint32) (pFromData)[997] << 8; \
           (nToData) |= (uint32) (pFromData)[998] << 16; \
           (nToData) |= (uint32) (pFromData)[999] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV50(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[1000] ; \
           (nToData) |= (uint32) (pFromData)[1001] << 8; \
           (nToData) |= (uint32) (pFromData)[1002] << 16; \
           (nToData) |= (uint32) (pFromData)[1003] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV51(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[1004] ; \
           (nToData) |= (uint32) (pFromData)[1005] << 8; \
           (nToData) |= (uint32) (pFromData)[1006] << 16; \
           (nToData) |= (uint32) (pFromData)[1007] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV52(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[1008] ; \
           (nToData) |= (uint32) (pFromData)[1009] << 8; \
           (nToData) |= (uint32) (pFromData)[1010] << 16; \
           (nToData) |= (uint32) (pFromData)[1011] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV53(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[1012] ; \
           (nToData) |= (uint32) (pFromData)[1013] << 8; \
           (nToData) |= (uint32) (pFromData)[1014] << 16; \
           (nToData) |= (uint32) (pFromData)[1015] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV54(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[1016] ; \
           (nToData) |= (uint32) (pFromData)[1017] << 8; \
           (nToData) |= (uint32) (pFromData)[1018] << 16; \
           (nToData) |= (uint32) (pFromData)[1019] << 24; \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_GET_RESV55(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[1020] ; \
           (nToData) |= (uint32) (pFromData)[1021] << 8; \
           (nToData) |= (uint32) (pFromData)[1022] << 16; \
           (nToData) |= (uint32) (pFromData)[1023] << 24; \
          } while(0)


#define SB_ZF_C2RCDMAFORMAT_SIZE_IN_WORDS 256

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR(uInst,nFromData,pToData) \
          do { \
           (pToData)[uInst] = ((nFromData)); \
          } while(0)

#define SB_ZF_C2RCDMAFORMAT_SET_INSTR_DMA(uInst,nFromData,pToData) \
          do { \
           (pToData)[uInst*4 + 0] = ((nFromData)) & 0xFF; \
           (pToData)[uInst*4 + 1] = ((pToData)[uInst*4 + 1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[uInst*4 + 2] = ((pToData)[uInst*4 + 2] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
           (pToData)[uInst*4 + 3] = ((pToData)[uInst*4 + 3] & ~0xFF) | (((nFromData) >> 24) & 0xFF); \
          } while(0)

void sbZfC2RcDmaFormat_SetPatternWord(uint32 *pToData, UINT uBlock, UINT uInstruction, UINT uPatternWord, UINT nFRomData);
void sbZfC2RcDmaFormat_GetPatternWord(uint32 *pToData, UINT uBlock, UINT uInstruction, UINT uPatternWord, UINT *nFromData);

#endif
