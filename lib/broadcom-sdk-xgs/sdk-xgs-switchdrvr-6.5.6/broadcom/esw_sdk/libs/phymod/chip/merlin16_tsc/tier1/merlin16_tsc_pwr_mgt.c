/**************************************************************************************
 **************************************************************************************
 *  File Name     :  merlin16_tsc_pwr_mgt.c                                             *
 *  Created On    :  04 Nov 2015                                                      *
 *  Created By    :  Brent Roberts                                                    *
 *  Description   :  APIs for Serdes IPs                                              *
 *  Revision      :      *
 *                                                                                    *
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

/** @file merlin16_tsc_pwr_mgt.c
 * Implementation of API power management functions
 */

#include <phymod/phymod.h>
#include "merlin16_tsc_pwr_mgt.h"
#include "merlin16_tsc_common.h"
#include "merlin16_tsc_functions.h"
#include "merlin16_tsc_internal.h"
#include "merlin16_tsc_internal_error.h"
#include "merlin16_tsc_select_defns.h"
#include "merlin16_tsc_config.h"

/***************************/
/*  Configure Serdes IDDQ  */
/***************************/

err_code_t merlin16_tsc_core_config_for_iddq(srds_access_t *sa__) {

  EFUN(wrc_ams_pll_pwrdn(0x1));
  return (ERR_CODE_NONE);
}


err_code_t merlin16_tsc_lane_config_for_iddq(srds_access_t *sa__) {

  /* Use frc/frc_val to force all RX and TX clk_vld signals to 0 */
  EFUN(wr_pmd_rx_clk_vld_frc_val(0x0));
  EFUN(wr_pmd_rx_clk_vld_frc(0x1));
      EFUN(wr_pmd_tx_clk_vld_frc_val(0x0));
      EFUN(wr_pmd_tx_clk_vld_frc(0x1));

  /* Use frc/frc_val to force all pmd_rx_lock signals to 0 */
  EFUN(wr_rx_dsc_lock_frc_val(0x0));
  EFUN(wr_rx_dsc_lock_frc(0x1));

  /* Switch all the lane clocks to comclk by writing to RX/TX comclk_sel registers */
  EFUN(wr_ln_rx_s_comclk_sel(0x1));
    EFUN(wr_ln_tx_s_comclk_sel(0x1));

  /* Assert all the AFE pwrdn/reset pins using frc/frc_val to make sure AFE is in lowest possible power mode */
  EFUN(wr_afe_tx_pwrdn_frc_val(0x1));
  EFUN(wr_afe_tx_pwrdn_frc(0x1));
  EFUN(wr_afe_rx_pwrdn_frc_val(0x1));
  EFUN(wr_afe_rx_pwrdn_frc(0x1));
  EFUN(wr_afe_tx_reset_frc_val(0x1));
  EFUN(wr_afe_tx_reset_frc(0x1));
  EFUN(wr_afe_rx_reset_frc_val(0x1));
  EFUN(wr_afe_rx_reset_frc(0x1));

  /* Set pmd_iddq pin to enable IDDQ */
  return (ERR_CODE_NONE);
}

/****************************************************/
/*  Serdes Powerdown, ClockGate and Deep_Powerdown  */
/****************************************************/

err_code_t merlin16_tsc_core_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {
    switch(mode) {
    case PWR_ON:
        EFUN(merlin16_tsc_INTERNAL_core_clkgate(sa__, 0));
        EFUN(wrc_afe_s_pll_pwrdn(0x0));
        EFUN(merlin16_tsc_core_dp_reset(sa__, 0x0));
        break;
    case PWRDN:
        EFUN(merlin16_tsc_core_dp_reset(sa__, 1));
        USR_DELAY_NS(500); /* wait >50 comclk cycles  */
        EFUN(wrc_afe_s_pll_pwrdn(0x1));
        break;
    case PWRDN_DEEP:
        EFUN(merlin16_tsc_core_dp_reset(sa__, 1));
        USR_DELAY_NS(500); /* wait >50 comclk cycles  */
                EFUN(wrc_afe_s_pll_pwrdn(0x1));
        EFUN(merlin16_tsc_INTERNAL_core_clkgate(sa__, 1));
        break;
    default:
        EFUN(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
    }
    return ERR_CODE_NONE;
}

err_code_t merlin16_tsc_lane_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {

    switch(mode) {
    case PWR_ON:
        EFUN(wr_ln_tx_s_pwrdn(0x0));
        EFUN(wr_ln_rx_s_pwrdn(0x0));
        EFUN(merlin16_tsc_INTERNAL_lane_clkgate(sa__, 0));
        break;
    case PWRDN:
        /* do the RX first, since that is what is most users care about */
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_DEEP:
        /* do the RX first, since that is what is most users care about */
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        EFUN(merlin16_tsc_INTERNAL_lane_clkgate(sa__, 1));
        EFUN(wr_ln_dp_s_rstb(0x0));
        break;
    case PWRDN_TX:
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_RX:
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        break;
    default :
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
    }
  return (ERR_CODE_NONE);
}
