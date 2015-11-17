/******************************************************************************/
/******************************************************************************/
/*  Revision      :  $Id: falcon_api_enum.h 633 2014-07-21 19:21:12Z jgaither $ */
/*                                                                            */
/*  Description   :  Enum types used by Serdes API functions                  */
/*                                                                            */
/*  All Rights Reserved                                                       */
/*  No portions of this material may be reproduced in any form without        */
/*  the written permission of:                                                */
/*      Broadcom Corporation                                                  */
/*      5300 California Avenue                                                */
/*      Irvine, CA  92617                                                     */
/*                                                                            */
/*  All information contained in this document is Broadcom Corporation        */
/*  company private proprietary, and trade secret.                            */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*
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

/** @file falcon_api_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef SERDES_API_ENUM_H
#define SERDES_API_ENUM_H
#define FALCON_TSC
#define FALCON

#include "falcon_tsc_common_enum.h"

#ifdef MERLIN
/** Merlin PLL Config Enum */ 
enum falcon_pll_enum {
  SERDES_ENUM_pll_8p5GHz_106p25MHz,
  SERDES_ENUM_pll_10GHz_125MHz,
  SERDES_ENUM_pll_10GHz_156p25MHz,
  SERDES_ENUM_pll_10p3125GHz_156p25MHz,
  SERDES_ENUM_pll_9p375GHz_156p25MHz,
  SERDES_ENUM_pll_9p375GHz_50MHz,
  SERDES_ENUM_pll_10p3125GHz_125MHz,
  SERDES_ENUM_pll_10p709GHz_125MHz,
  SERDES_ENUM_pll_10p7545GHz_125MHz,
  SERDES_ENUM_pll_10p9375GHz_125MHz,
  SERDES_ENUM_pll_11p049GHz_125MHz,
  SERDES_ENUM_pll_11p095GHz_125MHz,
  SERDES_ENUM_pll_11p14273GHz_125MHz,
  SERDES_ENUM_pll_11p181GHz_125MHz,
  SERDES_ENUM_pll_11p25GHz_125MHz,
  SERDES_ENUM_pll_11p45863GHz_125MHz,
  SERDES_ENUM_pll_8p5GHz_125MHz,
  SERDES_ENUM_pll_9p375GHz_125MHz,
  SERDES_ENUM_pll_10p709GHz_156p25MHz,
  SERDES_ENUM_pll_10p7545GHz_156p25MHz,
  SERDES_ENUM_pll_10p9375GHz_156p25MHz,
  SERDES_ENUM_pll_11p049GHz_156p25MHz,
  SERDES_ENUM_pll_11p095GHz_156p25MHz,
  SERDES_ENUM_pll_11p14273GHz_156p25MHz,
  SERDES_ENUM_pll_11p181GHz_156p25MHz,
  SERDES_ENUM_pll_11p25GHz_156p25MHz,
  SERDES_ENUM_pll_11p45863GHz_156p25MHz,
  SERDES_ENUM_pll_11p5GHz_156p25MHz,
  SERDES_ENUM_pll_8p5GHz_156p25MHz,
  SERDES_ENUM_pll_10GHz_161p132MHz,
  SERDES_ENUM_pll_10p3125GHz_161p132MHz,
  SERDES_ENUM_pll_10p709GHz_161p132MHz,
  SERDES_ENUM_pll_10p7545GHz_161p132MHz,
  SERDES_ENUM_pll_10p9375GHz_161p132MHz,
  SERDES_ENUM_pll_11p049GHz_161p132MHz,
  SERDES_ENUM_pll_11p095GHz_161p132MHz,
  SERDES_ENUM_pll_11p14273GHz_161p132MHz,
  SERDES_ENUM_pll_11p181GHz_161p132MHz,
  SERDES_ENUM_pll_11p25GHz_161p132MHz,
  SERDES_ENUM_pll_11p45863GHz_161p132MHz,
  SERDES_ENUM_pll_8p5GHz_161p132MHz,
  SERDES_ENUM_pll_9p375GHz_161p132MHz,
  SERDES_ENUM_pll_10GHz_50MHz,
  SERDES_ENUM_pll_10p3125GHz_50MHz
};
#endif


#ifdef EAGLE
/** Eagle PLL Config Enum */ 
enum falcon_pll_enum {
  SERDES_ENUM_pll_div_40x,
  SERDES_ENUM_pll_div_42x,
  SERDES_ENUM_pll_div_46x,
  SERDES_ENUM_pll_div_50x,
  SERDES_ENUM_pll_div_52x,
  SERDES_ENUM_pll_div_60x,
  SERDES_ENUM_pll_div_64x,
  SERDES_ENUM_pll_div_66x,
  SERDES_ENUM_pll_div_68x,
  SERDES_ENUM_pll_div_70x,
  SERDES_ENUM_pll_div_72x,
  SERDES_ENUM_pll_div_73p6x,
  SERDES_ENUM_pll_div_80x,
  SERDES_ENUM_pll_div_82p5x,
  SERDES_ENUM_pll_div_87p5x,
  SERDES_ENUM_pll_div_92x,
  SERDES_ENUM_pll_div_100x,
  SERDES_ENUM_pll_div_199p04x
};
#endif


#ifdef FALCON_TSC
/** Falcon TSC PLL Config Enum */
enum falcon_pll_enum {
  SERDES_ENUM_pll_div_128x,
  SERDES_ENUM_pll_div_132x,
  SERDES_ENUM_pll_div_140x,
  SERDES_ENUM_pll_div_160x,
  SERDES_ENUM_pll_div_165x,
  SERDES_ENUM_pll_div_168x,
  SERDES_ENUM_pll_div_175x,
  SERDES_ENUM_pll_div_180x,
  SERDES_ENUM_pll_div_184x,
  SERDES_ENUM_pll_div_200x,
  SERDES_ENUM_pll_div_224x,
  SERDES_ENUM_pll_div_264x
};
#endif


#ifdef FALCON_FURIA
/** Falcon Furia PLL Config Enum */
enum falcon_pll_enum {
  SERDES_ENUM_pll_div_80x,
  SERDES_ENUM_pll_div_96x,
  SERDES_ENUM_pll_div_120x,
  SERDES_ENUM_pll_div_128x,
  SERDES_ENUM_pll_div_132x,
  SERDES_ENUM_pll_div_140x,
  SERDES_ENUM_pll_div_144x,
  SERDES_ENUM_pll_div_160x,
  SERDES_ENUM_pll_div_160x_vco2,
  SERDES_ENUM_pll_div_165x,
  SERDES_ENUM_pll_div_168x,
  SERDES_ENUM_pll_div_175x,
  SERDES_ENUM_pll_div_180x,
  SERDES_ENUM_pll_div_184x,
  SERDES_ENUM_pll_div_198x,
  SERDES_ENUM_pll_div_200x,
  SERDES_ENUM_pll_div_224x,
  SERDES_ENUM_pll_div_264x
};
#endif

#endif
