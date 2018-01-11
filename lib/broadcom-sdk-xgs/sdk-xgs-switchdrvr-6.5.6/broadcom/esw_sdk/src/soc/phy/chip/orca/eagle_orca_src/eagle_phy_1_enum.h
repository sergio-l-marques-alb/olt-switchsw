/******************************************************************************
 ******************************************************************************
 *  Revision      :  $Id: eagle_phy_1_enum.h 1490 2016-08-11 22:26:06Z cvazquez $ *
 *                                                                            *
 *  Description   :  Enum types used by Serdes API functions                  *
 *                                                                            *
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$                                                      *
 *  No portions of this material may be reproduced in any form without        *
 *  the written permission of:                                                *
 *      Broadcom Corporation                                                  *
 *      5300 California Avenue                                                *
 *      Irvine, CA  92617                                                     *
 *                                                                            *
 *  All information contained in this document is Broadcom Corporation        *
 *  company private proprietary, and trade secret.                            *
 *                                                                            *
 ******************************************************************************
 ******************************************************************************/

/** @file eagle_phy_1_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef EAGLE_PHY_1_API_ENUM_H
#define EAGLE_PHY_1_API_ENUM_H

#include "common/srds_api_enum.h"



/** Eagle PLL Config Enum */
enum eagle_phy_1_pll_enum {
  EAGLE_PHY_1_pll_div_40x,
  EAGLE_PHY_1_pll_div_42x,
  EAGLE_PHY_1_pll_div_46x,
  EAGLE_PHY_1_pll_div_50x,
  EAGLE_PHY_1_pll_div_52x,
  EAGLE_PHY_1_pll_div_60x,
  EAGLE_PHY_1_pll_div_64x_refc161,
  EAGLE_PHY_1_pll_div_64x_refc156,
  EAGLE_PHY_1_pll_div_64x,
  EAGLE_PHY_1_pll_div_66x,
  EAGLE_PHY_1_pll_div_68x,
  EAGLE_PHY_1_pll_div_70x,
  EAGLE_PHY_1_pll_div_72x,
  EAGLE_PHY_1_pll_div_73p6x,
  EAGLE_PHY_1_pll_div_80x_refc125,
  EAGLE_PHY_1_pll_div_80x_refc106,
  EAGLE_PHY_1_pll_div_80x,
  EAGLE_PHY_1_pll_div_80x_refc156,
  EAGLE_PHY_1_pll_div_82p5x,
  EAGLE_PHY_1_pll_div_87p5x,
  EAGLE_PHY_1_pll_div_92x,
  EAGLE_PHY_1_pll_div_100x,
  EAGLE_PHY_1_pll_div_199p04x,
  EAGLE_PHY_1_pll_div_36p8x,
  EAGLE_PHY_1_pll_div_206p25x,
  EAGLE_PHY_1_pll_div_200p00x,
  EAGLE_PHY_1_pll_div_250p00x
};



















#endif
