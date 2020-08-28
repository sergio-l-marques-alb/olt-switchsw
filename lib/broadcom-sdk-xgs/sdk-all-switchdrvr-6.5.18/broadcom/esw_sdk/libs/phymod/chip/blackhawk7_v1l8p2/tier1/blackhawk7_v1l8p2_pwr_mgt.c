/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/**************************************************************************************
 **************************************************************************************
 *  File Name     :  blackhawk7_v1l8p2_pwr_mgt.c                                             *
 *  Created On    :  04 Nov 2015                                                      *
 *  Created By    :  Brent Roberts                                                    *
 *  Description   :  Power management APIs for Serdes IPs                             *
 *  Revision      :     *
 *                                                                                    *
 **************************************************************************************
 **************************************************************************************/

/** @file blackhawk7_v1l8p2_pwr_mgt.c
 * Implementation of power management functions
 */

#include "blackhawk7_v1l8p2_pwr_mgt.h"
#include "blackhawk7_v1l8p2_common.h"
#include "blackhawk7_v1l8p2_functions.h"
#include "blackhawk7_v1l8p2_internal.h"
#include "blackhawk7_v1l8p2_internal_error.h"
#include "blackhawk7_v1l8p2_select_defns.h"
#include "blackhawk7_v1l8p2_config.h"



static err_code_t _blackhawk7_v1l8p2_ams_core_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode);
#ifndef SMALL_FOOTPRINT
static err_code_t _blackhawk7_v1l8p2_ams_lane_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode);
static err_code_t _blackhawk7_v1l8p2_afe_rx_force_pwr_on(srds_access_t *sa__, uint8_t enable);
static err_code_t _blackhawk7_v1l8p2_afe_tx_force_pwr_on(srds_access_t *sa__, uint8_t enable);

/***************************/
/*  Configure Serdes IDDQ  */
/***************************/

err_code_t blackhawk7_v1l8p2_core_config_for_iddq(srds_access_t *sa__) {
  UNUSED(sa__);

  return (ERR_CODE_NONE);
}


err_code_t blackhawk7_v1l8p2_lane_config_for_iddq(srds_access_t *sa__) {

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

#endif /* !SMALL_FOOTPRINT */
err_code_t blackhawk7_v1l8p2_core_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {

    switch(mode) {
    case PWR_ON:
#ifndef SMALL_FOOTPRINT
        EFUN(blackhawk7_v1l8p2_INTERNAL_core_clkgate(sa__, 0));
#endif
        EFUN(wrc_ams_pll_pwrdn_buf(0x0));
        EFUN(wrc_ams_pll_pwrdn_buf_2(0x0));
        EFUN(wrc_ams_pll_pwrdn(0x0));
        EFUN(wrc_afe_s_pll_pwrdn(0x0));
        EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */

        EFUN(blackhawk7_v1l8p2_core_dp_reset(sa__, 0));
        EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
        break;
    case PWRDN:
        {
          uint8_t version;
          ESTM(version = GET_AMS_VERSION(rdc_afe_ipversion_id()));
          if(version == AMS_VERSION_B0) {
            EFUN(blackhawk7_v1l8p2_core_dp_reset(sa__, 1));
            EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
          }
        }
        EFUN(_blackhawk7_v1l8p2_ams_core_pwrdn(sa__,PWRDN));
        break;
    case PWROFF_DEEP: /* PWROFF_DEEP is identical to PWRDN_DEEP for core pwrdn. FALL THROUGH */
    case PWRDN_DEEP:
        {
          uint8_t version;
          ESTM(version = GET_AMS_VERSION(rdc_afe_ipversion_id()));
          if(version == AMS_VERSION_B0) {
            EFUN(blackhawk7_v1l8p2_core_dp_reset(sa__, 1));
            EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
          }
        }
        EFUN(_blackhawk7_v1l8p2_ams_core_pwrdn(sa__,PWRDN_DEEP));
#ifndef SMALL_FOOTPRINT
        EFUN(blackhawk7_v1l8p2_INTERNAL_core_clkgate(sa__, 1));
#endif
        break;
    default:
        EFUN(blackhawk7_v1l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
    }
    return ERR_CODE_NONE;
}
#ifndef SMALL_FOOTPRINT

err_code_t blackhawk7_v1l8p2_lane_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {
    EFUN(blackhawk7_v1l8p2_INTERNAL_assert_pk_if_a0(sa__, ASSERT_ALL_A0_PKILLS));

    switch(mode) {
    case PWR_ON:
        EFUN(blackhawk7_v1l8p2_INTERNAL_lane_clkgate(sa__, 0));
        EFUN(blackhawk7_v1l8p2_lane_pwrdn(sa__, PWR_ON_RX));
        EFUN(blackhawk7_v1l8p2_lane_pwrdn(sa__, PWR_ON_TX));
        break;
    case PWR_ON_TX:
        EFUN(wr_ams_tx_test_data(0x0));
        EFUN(wr_ams_tx_elec_idle_aux(0x0));
        EFUN(wr_ln_tx_s_pwrdn(0x0));
        EFUN(_blackhawk7_v1l8p2_afe_tx_force_pwr_on(sa__, 0));
        break;
    case PWR_ON_RX:
        EFUN(_blackhawk7_v1l8p2_afe_rx_force_pwr_on(sa__, 0));
        EFUN(wr_ln_rx_s_pwrdn(0x0));
        break;
    case PWRDN:
        EFUN(blackhawk7_v1l8p2_lane_pwrdn(sa__, PWRDN_TX));
        EFUN(blackhawk7_v1l8p2_lane_pwrdn(sa__, PWRDN_RX));
        break;
    case PWROFF_DEEP: /* PWROFF_DEEP is identical to PWRDN_DEEP for lane pwrdn. FALL THROUGH */
    case PWRDN_DEEP:
        EFUN(blackhawk7_v1l8p2_lane_pwrdn(sa__, PWRDN_TX_DEEP));
        EFUN(blackhawk7_v1l8p2_lane_pwrdn(sa__, PWRDN_RX_DEEP));
        EFUN(blackhawk7_v1l8p2_INTERNAL_lane_clkgate(sa__, 1));
        EFUN(wr_ln_dp_s_rstb(0x0));
        break;
    case PWRDN_TX:
        EFUN(_blackhawk7_v1l8p2_ams_lane_pwrdn(sa__, PWRDN_TX));
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_RX:
        EFUN(_blackhawk7_v1l8p2_ams_lane_pwrdn(sa__, PWRDN_RX));
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        break;
    case PWRDN_TX_DEEP:
        EFUN(_blackhawk7_v1l8p2_ams_lane_pwrdn(sa__, PWRDN_TX_DEEP));
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_RX_DEEP:
        EFUN(_blackhawk7_v1l8p2_ams_lane_pwrdn(sa__, PWRDN_RX_DEEP));
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        break;
    default:
        return(blackhawk7_v1l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
  return (ERR_CODE_NONE);
}
err_code_t blackhawk7_v1l8p2_pwrdn_all(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {

    if(mode != PWRDN_DEEP && mode != PWRDN && mode != PWROFF_DEEP) {
        EFUN(blackhawk7_v1l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    {
        uint8_t lane_orig = blackhawk7_v1l8p2_get_lane(sa__);
        uint8_t lane;
        uint8_t lanes_per_core;
        ESTM(lanes_per_core = rdc_revid_multiplicity());

        for (lane = 0; lane < lanes_per_core; ++lane) {
            EFUN(blackhawk7_v1l8p2_set_lane(sa__,   lane));
            EFUN(blackhawk7_v1l8p2_lane_pwrdn(sa__, mode));
        }
        EFUN(blackhawk7_v1l8p2_set_lane(sa__, lane_orig));
    }
    {
        uint8_t pll_orig = blackhawk7_v1l8p2_get_pll_idx(sa__);
        uint8_t pll;
        for (pll = 0; pll < DUAL_PLL_NUM_PLLS; ++pll) {
            EFUN(blackhawk7_v1l8p2_set_pll_idx(sa__, pll));
            EFUN(blackhawk7_v1l8p2_core_pwrdn(sa__, mode));
        }
        EFUN(blackhawk7_v1l8p2_set_pll_idx(sa__, pll_orig));
    }
    return ERR_CODE_NONE;
}
static err_code_t _blackhawk7_v1l8p2_ams_lane_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {
    uint8_t version;
    ESTM(version = GET_AMS_VERSION(rdc_afe_ipversion_id()));

    switch(mode) {
    case PWRDN_TX:
        EFUN(wr_ams_tx_elec_idle_aux(0x1));
        if(version == AMS_VERSION_A0) {
            EFUN(wr_ams_tx_test_data(0x1));
            EFUN(_blackhawk7_v1l8p2_afe_tx_force_pwr_on(sa__, 1));
        }
        break;
    case PWRDN_RX:
        if(version == AMS_VERSION_A0) {
            EFUN(_blackhawk7_v1l8p2_afe_rx_force_pwr_on(sa__, 1));
        }
        break;
    case PWRDN_TX_DEEP:
        EFUN(wr_ams_tx_elec_idle_aux(0x1));
        if(version == AMS_VERSION_A0) {
            EFUN(wr_ams_tx_test_data(0x1));
            EFUN(wr_ln_dp_s_rstb(0x0));
		    EFUN(wr_tx_osr_mode_frc_val(BLACKHAWK7_V1L8P2_OSX4));
            EFUN(wr_tx_osr_mode_frc(0x1));
            EFUN(wr_ln_dp_s_rstb(0x1));
            EFUN(_blackhawk7_v1l8p2_afe_tx_force_pwr_on(sa__, 1));
        }
        break;
    case PWRDN_RX_DEEP:
        if(version == AMS_VERSION_A0) {
            EFUN(wr_ln_dp_s_rstb(0x0));
            EFUN(wr_rx_pam4_mode_frc_val(0x1));   /* PAM4 OS modes  */
            EFUN(wr_rx_pam4_mode_frc(0x1));
            EFUN(wr_rx_pam4_er_mode(0x1));        /* PAM4 ER mode   */
            EFUN(wr_rx_osr_mode_frc_val(BLACKHAWK7_V1L8P2_OSX4));
            EFUN(wr_rx_osr_mode_frc(0x1));
            EFUN(wr_ln_dp_s_rstb(0x1));
            EFUN(_blackhawk7_v1l8p2_afe_rx_force_pwr_on(sa__, 1));
        }
        break;
    default:
        return(blackhawk7_v1l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    return (ERR_CODE_NONE);
}
static err_code_t _blackhawk7_v1l8p2_afe_rx_force_pwr_on(srds_access_t *sa__, uint8_t enable) {
    EFUN(wr_afe_rx_pwrdn_frc_val(0x0));
    EFUN(wr_afe_rx_reset_frc_val(0x0));
    EFUN(wr_afe_rx_pwrdn_frc(enable));
    EFUN(wr_afe_rx_reset_frc(enable));
    return (ERR_CODE_NONE);
}

static err_code_t _blackhawk7_v1l8p2_afe_tx_force_pwr_on(srds_access_t *sa__, uint8_t enable) {
    EFUN(wr_afe_tx_pwrdn_frc_val(0x0));
    EFUN(wr_afe_tx_reset_frc_val(0x0));
    EFUN(wr_afe_tx_pwrdn_frc(enable));
    EFUN(wr_afe_tx_reset_frc(enable));
    return (ERR_CODE_NONE);

}

#endif /* SMALL_FOOTPRINT */
static err_code_t _blackhawk7_v1l8p2_ams_core_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {
    uint8_t version;
    ESTM(version = GET_AMS_VERSION(rdc_afe_ipversion_id()));
    switch(mode) {
    case PWRDN:
        if(version == AMS_VERSION_B0) {
            EFUN(wrc_afe_s_pll_pwrdn(0x1));
            EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
            EFUN(wrc_ams_pll_pwrdn(0x1));
        }
        break;
    case PWRDN_DEEP:
        if(version == AMS_VERSION_B0) {
            EFUN(wrc_afe_s_pll_pwrdn(0x1));
            EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
            EFUN(wrc_ams_pll_pwrdn(0x1));
            EFUN(wrc_ams_pll_pwrdn_buf_2(0x1));
            EFUN(wrc_ams_pll_pwrdn_buf(0x1));
        }
        break;
    default:
        return(blackhawk7_v1l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    return (ERR_CODE_NONE);
}
