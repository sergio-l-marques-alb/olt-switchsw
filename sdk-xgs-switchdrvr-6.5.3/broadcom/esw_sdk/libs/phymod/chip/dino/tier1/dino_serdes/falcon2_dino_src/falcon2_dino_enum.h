/******************************************************************************
 ******************************************************************************
 *  Revision      :  $Id: falcon2_dino_enum.h 1172 2015-10-06 19:34:58Z kirand $ *
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

/** @file falcon2_dino_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef FALCON2_DINO_API_ENUM_H
#define FALCON2_DINO_API_ENUM_H

#include "../common/srds_api_enum.h"













/* Note: Dino PLL Configs copied over from Furia */
/** Falcon2 Dino PLL Config Enum */
enum falcon2_dino_pll_enum {
  FALCON2_DINO_pll_div_80x,
  FALCON2_DINO_pll_div_96x,
  FALCON2_DINO_pll_div_120x,
  FALCON2_DINO_pll_div_128x,
  FALCON2_DINO_pll_div_128x_vco2,
  FALCON2_DINO_pll_div_132x,
  FALCON2_DINO_pll_div_140x,
  FALCON2_DINO_pll_div_144x,
  FALCON2_DINO_pll_div_160x,
  FALCON2_DINO_pll_div_160x_vco2,
  FALCON2_DINO_pll_div_160x_refc174,
  FALCON2_DINO_pll_div_165x,
  FALCON2_DINO_pll_div_168x,
  FALCON2_DINO_pll_div_175x,
  FALCON2_DINO_pll_div_180x,
  FALCON2_DINO_pll_div_184x,
  FALCON2_DINO_pll_div_198x,
  FALCON2_DINO_pll_div_200x,
  FALCON2_DINO_pll_div_224x,
  FALCON2_DINO_pll_div_264x,
  FALCON2_DINO_pll_div_120x_refc125,
  FALCON2_DINO_pll_div_132x_refc212,
  FALCON2_DINO_pll_div_165x_refc125,
  FALCON2_DINO_pll_div_180x_refc125
};






#endif
