/**************************************************************************************
 **************************************************************************************
 *  File Name     :  falcon_furia_pll_config.c                                        *
 *  Created On    :  23/12/2013                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Falcon Furia PLL Configuration API                               *
 *  Revision      :  $Id: falcon_furia_pll_config.c 692 2014-09-09 20:47:56Z kirand $ *
 *                                                                                    *
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$                                                              *
 *  No portions of this material may be reproduced in any form without                *
 *  the written permission of:                                                        *
 *      Broadcom Corporation                                                          *
 *      5300 California Avenue                                                        *
 *      Irvine, CA  92617                                                             *
 *                                                                                    *
 *  All information contained in this document is Broadcom Corporation                *
 *  company private proprietary, and trade secret.                                    *
 *                                                                                    *
 **************************************************************************************
 **************************************************************************************/


/** @file falcon_furia_pll_config.c
 * Falcon Repeater PLL Configuration
 */

#include "falcon_furia_enum.h"


err_code_t falcon_furia_configure_pll( const phymod_access_t *pa, enum falcon_furia_pll_enum pll_cfg) {

  /* Restore defaults, needed for non-register reset cases */
    /* wrc_pll_mode(0x7);         */
    /* wrc_ams_pll_vco2_15g(0x0); */


  switch (pll_cfg) {

    /******************/
    /*  Integer Mode  */
    /******************/    

    case FALCON_FURIA_pll_div_80x :
      /* pll_ mode<4:0> = 00010, VCO = 17.0G, Refclk = 212.5MHz  */
      wrc_pll_mode(0x02);
      wrc_ams_pll_vco2_15g(0x1);
      break;

    case FALCON_FURIA_pll_div_96x :
      /* pll_ mode<4:0> = 10000, VCO = 15.0G, Refclk = 156.25MHz  */
      wrc_pll_mode(0x10);
      wrc_ams_pll_vco2_15g(0x1);
      break;

    case FALCON_FURIA_pll_div_120x :
      /* pll_ mode<4:0> = 10001, VCO = 15.0G, Refclk = 125.0MHz    */
      /* pll_ mode<4:0> = 10001, VCO = 18.75G, Refclk = 156.25MHz  */
      wrc_pll_mode(0x11);
      wrc_ams_pll_vco2_15g(0x1);
      break;

    case FALCON_FURIA_pll_div_128x :
      /* pll_ mode<4:0> = 00011, VCO = 20.625G, Refclk = 161.1328125MHz  */
      wrc_pll_mode(0x03);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_132x :
      /* pll_ mode<4:0> = 00100 , VCO = 20.625G, Refclk = 156.25MHz */
      /* pll_ mode<4:0> = 00100 , VCO = 28.055G, Refclk = 212.5MHz  */
      wrc_pll_mode(0x04);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_140x :
      /* pll_ mode<4:0> = 00101 , VCO = 21.875G, Refclk = 156.25MHz */
      wrc_pll_mode(0x05);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_144x :
      /* pll_ mode<4:0> = 10010 , VCO = 22.39488G, Refclk = 155.52MHz */
      wrc_pll_mode(0x12);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_160x :
      /* pll_ mode<4:0> = 00110 , VCO = 25.0G, Refclk = 156.25MHz          */
      /* pll_ mode<4:0> = 00110 , VCO = 25.78125G, Refclk = 161.1328125MHz */
      wrc_pll_mode(0x06);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_160x_vco2 :
      /* pll_ mode<4:0> = 00110 , VCO = 17.0G, Refclk = 106.25MHz */
      wrc_pll_mode(0x06);
      wrc_ams_pll_vco2_15g(0x1);
      break;

    case FALCON_FURIA_pll_div_160x_refc174 :
      /* pll_mode<3:0> = 0110 , VCO = 27.9525G, Refclk = 174.7MHz */
      wrc_pll_mode(0x6);
      wrc_ams_pll_force_kvh_bw(1);
      wrc_ams_pll_kvh_force(0);
      wrc_ams_pll_vco_indicator(1);
      break;

    case FALCON_FURIA_pll_div_165x : 
      /* pll_ mode<4:0> = 00111, VCO = 20.625G, Refclk = 125MHz      */
      /* pll_ mode<4:0> = 00111, VCO = 25.78125G, Refclk = 156.25MHz */
      wrc_pll_mode(0x07);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_168x :
      /* pll_ mode<4:0> = 01000, VCO = 26.25G, Refclk = 156.25MHz */
      wrc_pll_mode(0x08);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_175x :
      /* pll_ mode<4:0> = 01010, VCO = 27.34375G, Refclk = 156.25MHz  */
      wrc_pll_mode(0x0A);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_180x :
      /* pll_ mode<4:0> = 01011 , VCO = 22.5G, Refclk = 125.0MHz    */
      /* pll_ mode<4:0> = 01011 , VCO = 28.125G, Refclk = 156.25MHz */
      wrc_pll_mode(0x0B);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_184x : 
      /* pll_ mode<4:0> = 01100 , VCO = 23.0G, Refclk = 125.0MHz */
      wrc_pll_mode(0x0C);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_198x : 
      /* pll_ mode<4:0> = 10011 , VCO = 24.75G, Refclk = 125.0MHz */
      wrc_pll_mode(0x13);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_200x :
      /* pll_ mode<4:0> = 01101, VCO = 25.0G, Refclk = 125.0MHz  */
      wrc_pll_mode(0x0D);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_224x :
      /* pll_ mode<4:0> = 01110 , VCO = 28.0G, Refclk = 125.0MHz */
      wrc_pll_mode(0x0E);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_264x :
      /* pll_ mode<4:0> = 01111 , VCO = 28.05G, Refclk = 106.25MHz */
      wrc_pll_mode(0x0F);
      wrc_ams_pll_vco2_15g(0x0);
      break;

    default:                     /* Invalid pll_cfg value  */
      return (_error(ERR_CODE_INVALID_PLL_CFG));
      break; 


  } /* switch (pll_cfg) */
  
  return (ERR_CODE_NONE);

} /* falcon_furia_configure_pll */
      
