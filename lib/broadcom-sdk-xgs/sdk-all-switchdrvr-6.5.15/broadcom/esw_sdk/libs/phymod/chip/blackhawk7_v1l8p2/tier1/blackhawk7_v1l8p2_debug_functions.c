/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  blackhawk7_v1l8p2_debug_functions.c                                  *
 *  Created On    :  03 Nov 2015                                                   *
 *  Created By    :  Brent Roberts                                                 *
 *  Description   :  APIs for Serdes IPs                                           *
 *  Revision      :   *
 *                                                                                 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                           *
 *  No portions of this material may be reproduced in any form without             *
 *  the written permission of:                                                     *
 *      Broadcom Corporation                                                       *
 *      5300 California Avenue                                                     *
 *      Irvine, CA  92617                                                          *
 *                                                                                 *
 *  All information contained in this document is Broadcom Corporation             *
 *  company private proprietary, and trade secret.                                 *
 *                                                                                 *
 ***********************************************************************************
 ***********************************************************************************/

/** @file blackhawk7_v1l8p2_debug_functions.c
 * Implementation of API debug functions
 */
#include <phymod/phymod_system.h>
#include "blackhawk7_v1l8p2_debug_functions.h"
#include "blackhawk7_v1l8p2_access.h"
#include "blackhawk7_v1l8p2_common.h"
#include "blackhawk7_v1l8p2_config.h"
#include "blackhawk7_v1l8p2_functions.h"
#include "blackhawk7_v1l8p2_internal.h"
#include "blackhawk7_v1l8p2_internal_error.h"
#include "blackhawk7_v1l8p2_prbs.h"
#include "blackhawk7_v1l8p2_select_defns.h"



/*************************/
/*  Stop/Resume uC Lane  */
/*************************/

err_code_t blackhawk7_v1l8p2_stop_uc_lane(srds_access_t *sa__, uint8_t enable) {

    return(blackhawk7_v1l8p2_stop_rx_adaptation(sa__,enable));
}


err_code_t blackhawk7_v1l8p2_stop_uc_lane_status(srds_access_t *sa__, uint8_t *uc_lane_stopped) {

  if(!uc_lane_stopped) {
      return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*uc_lane_stopped = rdv_usr_sts_micro_stopped());

  return (ERR_CODE_NONE);
}

/*******************************************************************/
/*  APIs to Write Core/Lane Config and User variables into uC RAM  */
/*******************************************************************/

err_code_t blackhawk7_v1l8p2_set_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t core_event_log_level) {
  return(wrcv_usr_ctrl_core_event_log_level(core_event_log_level));
}

err_code_t blackhawk7_v1l8p2_set_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t lane_event_log_level) {
  return(wrv_usr_ctrl_lane_event_log_level(lane_event_log_level));
}

err_code_t blackhawk7_v1l8p2_set_usr_ctrl_disable_startup(srds_access_t *sa__, struct blackhawk7_v1l8p2_usr_ctrl_disable_functions_st set_val) {
  EFUN(blackhawk7_v1l8p2_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_functions_word(set_val.word));
}

err_code_t blackhawk7_v1l8p2_set_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct blackhawk7_v1l8p2_usr_ctrl_disable_dfe_functions_st set_val) {
  EFUN(blackhawk7_v1l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_dfe_functions_byte(set_val.byte));
}

err_code_t blackhawk7_v1l8p2_set_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct blackhawk7_v1l8p2_usr_ctrl_disable_functions_st set_val) {
  EFUN(blackhawk7_v1l8p2_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */
  EFUN(blackhawk7_v1l8p2_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_functions_word(set_val.word));
}

err_code_t blackhawk7_v1l8p2_set_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct blackhawk7_v1l8p2_usr_ctrl_disable_dfe_functions_st set_val) {
  EFUN(blackhawk7_v1l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(set_val.byte));
}

/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t blackhawk7_v1l8p2_get_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t *core_event_log_level) {

  if(!core_event_log_level) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*core_event_log_level = rdcv_usr_ctrl_core_event_log_level());

  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p2_get_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t *lane_event_log_level) {

  if(!lane_event_log_level) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*lane_event_log_level = rdv_usr_ctrl_lane_event_log_level());
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p2_get_usr_ctrl_disable_startup(srds_access_t *sa__, struct blackhawk7_v1l8p2_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_usr_ctrl_disable_startup_functions_word());
  EFUN(blackhawk7_v1l8p2_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p2_get_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct blackhawk7_v1l8p2_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_dfe_functions_byte());
  EFUN(blackhawk7_v1l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p2_get_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct blackhawk7_v1l8p2_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_usr_ctrl_disable_steady_state_functions_word());
  EFUN(blackhawk7_v1l8p2_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p2_get_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct blackhawk7_v1l8p2_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
  EFUN(blackhawk7_v1l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

/****************************************/
/*  Serdes Register/Variable Dump APIs  */
/****************************************/

err_code_t blackhawk7_v1l8p2_reg_dump(srds_access_t *sa__) {
  uint16_t addr, rddata;

  EFUN_PRINTF(("\n****  SERDES REGISTER DUMP    ****"));

  for (addr = 0x0; addr < 0x10; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(blackhawk7_v1l8p2_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0x90; addr < 0xA0; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(blackhawk7_v1l8p2_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0xD000; addr < 0xD1A0; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(blackhawk7_v1l8p2_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0xD1A0; addr < 0xD200; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(blackhawk7_v1l8p2_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }
  for (addr = 0xD200; addr < 0xD278; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(blackhawk7_v1l8p2_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }
  for (addr = 0xD300; addr < 0xD324; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(blackhawk7_v1l8p2_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0xD400; addr < 0xD4B0; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(blackhawk7_v1l8p2_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }
  for (addr = 0xFFD0; addr < 0xFFE0; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(blackhawk7_v1l8p2_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }
  return (ERR_CODE_NONE);
}


err_code_t blackhawk7_v1l8p2_uc_core_var_dump(srds_access_t *sa__) {
  uint8_t addr;

  EFUN_PRINTF(("\n**** SERDES UC CORE RAM VARIABLE DUMP ****"));

  for (addr = 0x0; addr < 0xFF; addr++) {
    if (!(addr % 26))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    ESTM_PRINTF(("%02x ", blackhawk7_v1l8p2_rdbc_uc_var(sa__, __ERR, addr)));
  }
  return (ERR_CODE_NONE);
}


err_code_t blackhawk7_v1l8p2_uc_lane_var_dump(srds_access_t *sa__) {
  uint8_t     rx_lock, uc_stopped = 0;
  uint16_t    addr;
  blackhawk7_v1l8p2_info_t const * const blackhawk7_v1l8p2_info_ptr = blackhawk7_v1l8p2_INTERNAL_get_blackhawk7_v1l8p2_info_ptr();
  EFUN(blackhawk7_v1l8p2_INTERNAL_verify_blackhawk7_v1l8p2_info(sa__, blackhawk7_v1l8p2_info_ptr));

  EFUN_PRINTF(("\n**** SERDES UC LANE %d RAM VARIABLE DUMP ****",blackhawk7_v1l8p2_get_lane(sa__)));

  ESTM(rx_lock = rd_pmd_rx_lock());

  {
      err_code_t err_code=ERR_CODE_NONE;
      uc_stopped = blackhawk7_v1l8p2_INTERNAL_stop_micro(sa__,rx_lock,&err_code);
      if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
  }

  for (addr = 0x0; addr < blackhawk7_v1l8p2_info_ptr->lane_var_ram_size; addr++) {
    if (!(addr % 26))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    ESTM_PRINTF(("%02x ", blackhawk7_v1l8p2_rdbl_uc_var(sa__, __ERR, addr)));
  }

  if (rx_lock == 1) {
      if (!uc_stopped) {
          EFUN(blackhawk7_v1l8p2_stop_rx_adaptation(sa__, 0));
      }
  } else {
      EFUN(blackhawk7_v1l8p2_stop_rx_adaptation(sa__, 0));
  }

  return (ERR_CODE_NONE);
}

/***************************************/
/*  API Function to Read Event Logger  */
/***************************************/

err_code_t blackhawk7_v1l8p2_read_event_log(srds_access_t *sa__) {
    blackhawk7_v1l8p2_INTERNAL_event_log_dump_state_t state;
    uint8_t micro_num = 0;
    blackhawk7_v1l8p2_info_t * blackhawk7_v1l8p2_info_ptr;
    ESTM(blackhawk7_v1l8p2_info_ptr = blackhawk7_v1l8p2_INTERNAL_get_blackhawk7_v1l8p2_info_ptr());
    for (; micro_num<blackhawk7_v1l8p2_info_ptr->micro_count; ++micro_num)
    {
        state.index = 0;
        state.line_start_index = 0;
        EFUN(blackhawk7_v1l8p2_INTERNAL_read_event_log_with_callback(sa__, micro_num, 0, &state, blackhawk7_v1l8p2_INTERNAL_event_log_dump_callback));
        EFUN(blackhawk7_v1l8p2_INTERNAL_event_log_dump_callback(sa__, &state, 0, 0));
    }
    return(ERR_CODE_NONE);
}

/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Enable/Disable Digital Loopback */
err_code_t blackhawk7_v1l8p2_dig_lpbk(srds_access_t *sa__, uint8_t enable) {
    /* setting/clearing prbs_chk_en_auto_mode while coming out of/going in to dig lpbk */
    EFUN(wr_prbs_chk_en_auto_mode(!enable));
    EFUN(wr_dig_lpbk_en(enable));                         /* 0 = disabled, 1 = enabled */

    /* Link Training enabled is an invalid mode in digital loopback */
    if(enable) {
        uint8_t link_training_enable = 0;
        ESTM(link_training_enable = rd_linktrn_ieee_training_enable());
        if(link_training_enable) {
            EFUN_PRINTF(("Warning: Core: %d, Lane %d: Link Training mode is on in digital loopback.\n", blackhawk7_v1l8p2_get_core(sa__), blackhawk7_v1l8p2_get_lane(sa__)));
        }
    }

  return (ERR_CODE_NONE);
}


/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t blackhawk7_v1l8p2_tx_pi_jitt_gen(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp) {
    /* Added a limiting for the jitter amplitude index, per freq_idx */
    uint8_t max_amp_idx_r20_os1[] = {37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37};

    /* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
    if (jit_type == TX_PI_SJ) {
        if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx]) {
            tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];
        }
    }

    EFUN(blackhawk7_v1l8p2_tx_pi_freq_override(sa__, enable, freq_override_val));

    if (enable) {
        EFUN(wr_tx_pi_jit_freq_idx(tx_pi_jit_freq_idx));
        EFUN(wr_tx_pi_jit_amp(tx_pi_jit_amp));

        if (jit_type == TX_PI_SSC_HIGH_FREQ) {
            EFUN(wr_tx_pi_jit_ssc_freq_mode(0x1));        /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
            EFUN(wr_tx_pi_ssc_gen_en(0x1));               /* SSC jitter enable:         0 = disabled,    1 = enabled */
        }
        else if (jit_type == TX_PI_SSC_LOW_FREQ) {
            EFUN(wr_tx_pi_jit_ssc_freq_mode(0x0));        /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
            EFUN(wr_tx_pi_ssc_gen_en(0x1));               /* SSC jitter enable:         0 = disabled,    1 = enabled */
        }
        else if (jit_type == TX_PI_SJ) {
            EFUN(wr_tx_pi_sj_gen_en(0x1));                /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
        }
    }
    else {
        EFUN(wr_tx_pi_ssc_gen_en(0x0));                   /* SSC jitter enable:         0 = disabled,    1 = enabled */
        EFUN(wr_tx_pi_sj_gen_en(0x0));                    /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
    }
  return (ERR_CODE_NONE);
}


/*******************************/
/*  Isolate Serdes Input Pins  */
/*******************************/

err_code_t blackhawk7_v1l8p2_isolate_ctrl_pins(srds_access_t *sa__, uint8_t enable) {
    uint8_t lane, lane_orig, num_lanes;
    blackhawk7_v1l8p2_info_t const * const blackhawk7_v1l8p2_info_ptr = blackhawk7_v1l8p2_INTERNAL_get_blackhawk7_v1l8p2_info_ptr();
    uint8_t pll, pll_orig;
    ESTM(pll_orig = blackhawk7_v1l8p2_get_pll_idx(sa__));

    for(pll = 0; pll < DUAL_PLL_NUM_PLLS; pll++) {
        EFUN(blackhawk7_v1l8p2_set_pll_idx(sa__, pll));
        EFUN(blackhawk7_v1l8p2_isolate_core_ctrl_pins(sa__, enable));
    }
    EFUN(blackhawk7_v1l8p2_set_pll_idx(sa__, pll_orig));
    
    ESTM(lane_orig = blackhawk7_v1l8p2_get_lane(sa__));
    num_lanes = blackhawk7_v1l8p2_info_ptr->lane_count * blackhawk7_v1l8p2_info_ptr->micro_count;
    for(lane = 0; lane < num_lanes; lane++) {
        EFUN(blackhawk7_v1l8p2_set_lane(sa__, lane));
        EFUN(blackhawk7_v1l8p2_isolate_lane_ctrl_pins(sa__, enable));
    }
    EFUN(blackhawk7_v1l8p2_set_lane(sa__, lane_orig));

  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p2_isolate_lane_ctrl_pins(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
    EFUN(wr_pmd_ln_tx_h_pwrdn_pkill(0x1));
    EFUN(wr_pmd_ln_rx_h_pwrdn_pkill(0x1));
    EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x1));
    EFUN(wr_pmd_ln_h_rstb_pkill(0x1));
    EFUN(wr_pmd_tx_disable_pkill(0x1));
  }
  else {
    EFUN(wr_pmd_ln_tx_h_pwrdn_pkill(0x0));
    EFUN(wr_pmd_ln_rx_h_pwrdn_pkill(0x0));
    EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x0));
    EFUN(wr_pmd_ln_h_rstb_pkill(0x0));
    EFUN(wr_pmd_tx_disable_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p2_isolate_core_ctrl_pins(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x1));
  }
  else {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p2_log_full_pmd_state_noPRBS (srds_access_t *sa__, struct blackhawk7_v1l8p2_detailed_lane_status_st *lane_st) {
    uint16_t reg_data, reg_data1, reg_data2, reg_data3, reg_data4;
    int8_t tmp;

    if(!lane_st) 
      return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    ESTM(lane_st->pmd_lock = rd_pmd_rx_lock());
    {
        err_code_t err_code=ERR_CODE_NONE;
        lane_st->stop_state = blackhawk7_v1l8p2_INTERNAL_stop_micro(sa__,lane_st->pmd_lock,&err_code);
        if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
    }
 
    ESTM(lane_st->reset_state = rd_lane_dp_reset_state());

  {
     enum blackhawk7_v1l8p2_rx_pam_mode_enum pam_mode = NRZ;
     EFUN(blackhawk7_v1l8p2_INTERNAL_get_rx_pam_mode(sa__, &pam_mode));
     lane_st->rx_pam_mode = (uint8_t)pam_mode;
  }

    ESTM(lane_st->pmd_tx_pll_select = rd_tx_pll_select());
    ESTM(lane_st->pmd_rx_pll_select = rd_rx_pll_select());

    ESTM(lane_st->restart_count = rdv_usr_sts_restart_counter());
    ESTM(lane_st->reset_count = rdv_usr_sts_reset_counter());
    ESTM(lane_st->pmd_lock_count = rdv_usr_sts_pmd_lock_counter());

    ESTM(lane_st->temp_idx = rdcv_temp_idx());
    ESTM(lane_st->ams_tx_ana_rescal = rdc_rescal_bg_mux());  
    
    ESTM(lane_st->tx_pam_mode = rd_tx_pam4_mode());
    ESTM(lane_st->tx_prec_en  = rd_pam4_precoder_en());
    ESTM(lane_st->txfir_use_pam4_range = rd_txfir_nrz_tap_range_sel() ? 0 : 1);
    {
        uint8_t tap_num;
        for (tap_num=0; tap_num<6; ++tap_num) {
            EFUN(blackhawk7_v1l8p2_INTERNAL_get_tx_tap(sa__, tap_num, &(lane_st->txfir.tap[tap_num])));
        }
        ESTM(tap_num = rd_txfir_tap_en());
        lane_st->num_txfir_taps = (tap_num == 0) ? 3 : ((tap_num == 1) ? 6 : ((tap_num == 2) ? 9 : 12));
    }
    ESTM(lane_st->sigdet = rd_signal_detect());
    ESTM(lane_st->dsc_sm[0] = rd_dsc_state_one_hot());
    ESTM(lane_st->dsc_sm[1] = rd_dsc_state_one_hot());
    ESTM(lane_st->ppm = rd_cdr_integ_reg());
    ESTM(lane_st->vga = rd_rx_vga_status());
    EFUN(blackhawk7_v1l8p2_INTERNAL_get_rx_pf_main(sa__, &tmp)); lane_st->pf = tmp;
    EFUN(blackhawk7_v1l8p2_INTERNAL_get_rx_pf2(sa__, &tmp)); lane_st->pf2 = tmp;
    EFUN(blackhawk7_v1l8p2_INTERNAL_get_rx_pf3(sa__, &tmp)); lane_st->pf3 = tmp;
    ESTM(lane_st->main_tap_est = rdv_usr_main_tap_est());
    ESTM(lane_st->data23_thresh  = rd_rx_data23_status());
    ESTM(lane_st->data14_thresh  = rd_rx_data14_status());
    ESTM(lane_st->data05_thresh  = rd_rx_data05_status());
    ESTM(lane_st->phase1_thresh  = rd_rx_phase1_status());
    ESTM(lane_st->phase02_thresh = rd_rx_phase02_status());
    EFUN(blackhawk7_v1l8p2_INTERNAL_get_lms_thresh_bin(sa__, &tmp)); lane_st->lms_thresh = tmp;
    ESTM(lane_st->clk90     = rdv_usr_sts_phase_hoffset());
    ESTM(reg_data = reg_rd_DSC_E_RX_PI_CNT_BIN_D());
    ESTM(reg_data1 = reg_rd_DSC_E_RX_PI_CNT_BIN_P());
    ESTM(reg_data2 = reg_rd_DSC_E_RX_PI_CNT_BIN_L());
    ESTM(reg_data3 = reg_rd_DSC_E_RX_PI_CNT_BIN_PD());
    ESTM(reg_data4 = reg_rd_DSC_E_RX_PI_CNT_BIN_LD());
    lane_st->ddq_hoffset = (int8_t)((reg_data&0xFF) - (((reg_data>>8)&0xFF) + (int8_t)(-128)));
    lane_st->ppq_hoffset = (int8_t)((reg_data1&0xFF) - (((reg_data1>>8)&0xFF) + (int8_t)(-128)));
    lane_st->llq_hoffset = (int8_t)((reg_data2&0xFF) - (((reg_data2>>8)&0xFF) + (int8_t)(-128)));
    lane_st->dp_hoffset = (int8_t)(((reg_data3>>8)&0xFF) - ((reg_data3&0xFF) - lane_st->clk90));
    lane_st->dl_hoffset = (int8_t)((reg_data4&0xFF) - ((reg_data4>>8)&0xFF));

    ESTM(lane_st->lms_dac4ck = rd_ams_rx_dac4ck_lms_i());
    ESTM(lane_st->phase_dac4ck = rd_ams_rx_dac4ck_phs_i());
    ESTM(lane_st->data_dac4ck     = rd_ams_rx_dac4ck_dat_i());
    if (lane_st->lms_dac4ck>7) lane_st->lms_dac4ck = -(lane_st->lms_dac4ck&0x07);
    if (lane_st->phase_dac4ck>7) lane_st->phase_dac4ck = -(lane_st->phase_dac4ck&0x07);
    if (lane_st->data_dac4ck>7) lane_st->data_dac4ck = -(lane_st->data_dac4ck&0x07);

    ESTM(lane_st->dc_offset = rd_dc_offset_bin());
    ESTM(lane_st->dfe[1][0] = rd_rxa_dfe_tap2_status());
    ESTM(lane_st->dfe[1][1] = rd_rxb_dfe_tap2_status());
    ESTM(lane_st->dfe[2][0] = rd_rxa_dfe_tap3_status());
    ESTM(lane_st->dfe[2][1] = rd_rxb_dfe_tap3_status());

    ESTM(lane_st->dfe[3][0] = rd_rxa_dfe_tap4_status());
    ESTM(lane_st->dfe[3][1] = rd_rxb_dfe_tap4_status());    
    ESTM(lane_st->dfe[4][0] = rd_rxa_dfe_tap5());
    ESTM(lane_st->dfe[4][1] = rd_rxb_dfe_tap5());

    ESTM(lane_st->dfe[5][0] = rd_rxa_dfe_tap6());
    ESTM(lane_st->dfe[5][1] = rd_rxb_dfe_tap6());

    ESTM(lane_st->dfe[6][0] = ((rd_rxa_dfe_tap7_mux()==0)?rd_rxa_dfe_tap7():0));
    ESTM(lane_st->dfe[6][1] = ((rd_rxb_dfe_tap7_mux()==0)?rd_rxb_dfe_tap7():0));

    ESTM(lane_st->dfe[7][0] = ((rd_rxa_dfe_tap8_mux()==0)?rd_rxa_dfe_tap8():0));
    ESTM(lane_st->dfe[7][1] = ((rd_rxb_dfe_tap8_mux()==0)?rd_rxb_dfe_tap8():0));

    ESTM(lane_st->dfe[8][0] = ((rd_rxa_dfe_tap9_mux()==0)?rd_rxa_dfe_tap9():0));
    ESTM(lane_st->dfe[8][1] = ((rd_rxb_dfe_tap9_mux()==0)?rd_rxb_dfe_tap9():0));
    
    ESTM(lane_st->dfe[9][0] = ((rd_rxa_dfe_tap10_mux()==0)?rd_rxa_dfe_tap10():0));
    ESTM(lane_st->dfe[9][1] = ((rd_rxb_dfe_tap10_mux()==0)?rd_rxb_dfe_tap10():0));
    
    ESTM(lane_st->dfe[10][0] = ((rd_rxa_dfe_tap7_mux()==1)?rd_rxa_dfe_tap7():(rd_rxa_dfe_tap11_mux()==0)?rd_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[10][1] = ((rd_rxb_dfe_tap7_mux()==1)?rd_rxb_dfe_tap7():(rd_rxb_dfe_tap11_mux()==0)?rd_rxb_dfe_tap11():0));
    
    ESTM(lane_st->dfe[11][0] = ((rd_rxa_dfe_tap8_mux()==1)?rd_rxa_dfe_tap8():(rd_rxa_dfe_tap12_mux()==0)?rd_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[11][1] = ((rd_rxb_dfe_tap8_mux()==1)?rd_rxb_dfe_tap8():(rd_rxb_dfe_tap12_mux()==0)?rd_rxb_dfe_tap12():0));
    
    ESTM(lane_st->dfe[12][0] = ((rd_rxa_dfe_tap9_mux()==1)?rd_rxa_dfe_tap9():(rd_rxa_dfe_tap13_mux()==0)?rd_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[12][1] = ((rd_rxb_dfe_tap9_mux()==1)?rd_rxb_dfe_tap9():(rd_rxb_dfe_tap13_mux()==0)?rd_rxb_dfe_tap13():0));
    
    ESTM(lane_st->dfe[13][0] = ((rd_rxa_dfe_tap10_mux()==1)?rd_rxa_dfe_tap10():(rd_rxa_dfe_tap14_mux()==0)?rd_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[13][1] = ((rd_rxb_dfe_tap10_mux()==1)?rd_rxb_dfe_tap10():(rd_rxb_dfe_tap14_mux()==0)?rd_rxb_dfe_tap14():0));
    
    ESTM(lane_st->dfe[14][0] = ((rd_rxa_dfe_tap7_mux()==2)?rd_rxa_dfe_tap7():(rd_rxa_dfe_tap11_mux()==1)?rd_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[14][1] = ((rd_rxb_dfe_tap7_mux()==2)?rd_rxb_dfe_tap7():(rd_rxb_dfe_tap11_mux()==1)?rd_rxb_dfe_tap11():0));

    ESTM(lane_st->dfe[15][0] = ((rd_rxa_dfe_tap8_mux()==2)?rd_rxa_dfe_tap8():(rd_rxa_dfe_tap12_mux()==1)?rd_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[15][1] = ((rd_rxb_dfe_tap8_mux()==2)?rd_rxb_dfe_tap8():(rd_rxb_dfe_tap12_mux()==1)?rd_rxb_dfe_tap12():0));

    ESTM(lane_st->dfe[16][0] = ((rd_rxa_dfe_tap9_mux()==2)?rd_rxa_dfe_tap9():(rd_rxa_dfe_tap13_mux()==1)?rd_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[16][1] = ((rd_rxb_dfe_tap9_mux()==2)?rd_rxb_dfe_tap9():(rd_rxb_dfe_tap13_mux()==1)?rd_rxb_dfe_tap13():0));

    ESTM(lane_st->dfe[17][0] = ((rd_rxa_dfe_tap10_mux()==2)?rd_rxa_dfe_tap10():(rd_rxa_dfe_tap14_mux()==1)?rd_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[17][1] = ((rd_rxb_dfe_tap10_mux()==2)?rd_rxb_dfe_tap10():(rd_rxb_dfe_tap14_mux()==1)?rd_rxb_dfe_tap14():0));

    ESTM(lane_st->dfe[18][0] = ((rd_rxa_dfe_tap7_mux()==3)?rd_rxa_dfe_tap7():(rd_rxa_dfe_tap11_mux()==2)?rd_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[18][1] = ((rd_rxb_dfe_tap7_mux()==3)?rd_rxb_dfe_tap7():(rd_rxb_dfe_tap11_mux()==2)?rd_rxb_dfe_tap11():0));

    ESTM(lane_st->dfe[19][0] = ((rd_rxa_dfe_tap8_mux()==3)?rd_rxa_dfe_tap8():(rd_rxa_dfe_tap12_mux()==2)?rd_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[19][1] = ((rd_rxb_dfe_tap8_mux()==3)?rd_rxb_dfe_tap8():(rd_rxb_dfe_tap12_mux()==2)?rd_rxb_dfe_tap12():0));

    ESTM(lane_st->dfe[20][0] = ((rd_rxa_dfe_tap9_mux()==3)?rd_rxa_dfe_tap9():(rd_rxa_dfe_tap13_mux()==2)?rd_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[20][1] = ((rd_rxb_dfe_tap9_mux()==3)?rd_rxb_dfe_tap9():(rd_rxb_dfe_tap13_mux()==2)?rd_rxb_dfe_tap13():0));

    ESTM(lane_st->dfe[21][0] = ((rd_rxa_dfe_tap10_mux()==3)?rd_rxa_dfe_tap10():(rd_rxa_dfe_tap14_mux()==2)?rd_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[21][1] = ((rd_rxb_dfe_tap10_mux()==3)?rd_rxb_dfe_tap10():(rd_rxb_dfe_tap14_mux()==2)?rd_rxb_dfe_tap14():0));

    ESTM(lane_st->dfe[22][0] = ((rd_rxa_dfe_tap11_mux()==3)?rd_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[22][1] = ((rd_rxb_dfe_tap11_mux()==3)?rd_rxb_dfe_tap11():0));

    ESTM(lane_st->dfe[23][0] = ((rd_rxa_dfe_tap12_mux()==3)?rd_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[23][1] = ((rd_rxb_dfe_tap12_mux()==3)?rd_rxb_dfe_tap12():0));

    ESTM(lane_st->dfe[24][0] = ((rd_rxa_dfe_tap13_mux()==3)?rd_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[24][1] = ((rd_rxb_dfe_tap13_mux()==3)?rd_rxb_dfe_tap13():0));

    ESTM(lane_st->dfe[25][0] = ((rd_rxa_dfe_tap14_mux()==3)?rd_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[25][1] = ((rd_rxb_dfe_tap14_mux()==3)?rd_rxb_dfe_tap14():0));

    
    ESTM(lane_st->thctrl_d0[0] = rd_rxa_slicer_offset_adj_cal_d0());
    ESTM(lane_st->thctrl_d0[1] = rd_rxb_slicer_offset_adj_cal_d0());

    ESTM(lane_st->thctrl_d1[0] = rd_rxa_slicer_offset_adj_cal_d1());
    ESTM(lane_st->thctrl_d1[1] = rd_rxb_slicer_offset_adj_cal_d1());

    ESTM(lane_st->thctrl_d2[0] = rd_rxa_slicer_offset_adj_cal_d2());
    ESTM(lane_st->thctrl_d2[1] = rd_rxb_slicer_offset_adj_cal_d2());

    ESTM(lane_st->thctrl_d3[0] = rd_rxa_slicer_offset_adj_cal_d3());
    ESTM(lane_st->thctrl_d3[1] = rd_rxb_slicer_offset_adj_cal_d3());

    ESTM(lane_st->thctrl_d4[0] = rd_rxa_slicer_offset_adj_cal_d4());
    ESTM(lane_st->thctrl_d4[1] = rd_rxb_slicer_offset_adj_cal_d4());

    ESTM(lane_st->thctrl_d5[0] = rd_rxa_slicer_offset_adj_cal_d5());
    ESTM(lane_st->thctrl_d5[1] = rd_rxb_slicer_offset_adj_cal_d5());

    ESTM(lane_st->thctrl_p0[0] = rd_rxa_slicer_offset_adj_cal_p0());
    ESTM(lane_st->thctrl_p0[1] = rd_rxb_slicer_offset_adj_cal_p0());

    ESTM(lane_st->thctrl_p1[0] = rd_rxa_slicer_offset_adj_cal_p1());
    ESTM(lane_st->thctrl_p1[1] = rd_rxb_slicer_offset_adj_cal_p1());

    ESTM(lane_st->thctrl_p2[0] = rd_rxa_slicer_offset_adj_cal_p2());
    ESTM(lane_st->thctrl_p2[1] = rd_rxb_slicer_offset_adj_cal_p2());

    ESTM(lane_st->thctrl_l[0]  = rd_rxa_slicer_offset_adj_cal_lms());
    ESTM(lane_st->thctrl_l[1]  = rd_rxb_slicer_offset_adj_cal_lms());


    ESTM(lane_st->pam4_chn_loss = rdv_usr_ctrl_pam4_chn_loss());
    ESTM(lane_st->ams_rx_vga1_deq        = rd_ams_rx_vga1_deq());
    ESTM(lane_st->ams_rx_sd_cal_pos      = rd_ams_rx_sigdet_offset_correction_pos());
    ESTM(lane_st->ams_rx_sd_cal_neg      = rd_ams_rx_sigdet_offset_correction_neg());
    ESTM(lane_st->ams_rx_vga0_rescal_mux = rd_ams_rx_status_pon_vga0());
    ESTM(lane_st->ams_rx_vga1_rescal_mux = rd_ams_rx_status_pon_vga1());
    ESTM(lane_st->ams_rx_vga_dis_ind = rd_ams_rx_vga_dis_ind());
    ESTM(lane_st->ams_rx_vga2_cm = rd_ams_rx_vga2_cm());
    ESTM(lane_st->ams_rx_eq2_hibw = rd_ams_rx_eq2_hibw());
    {
       uint8_t reg1 = 0, reg2 = 0;
       ESTM(reg1 = rd_ams_rx_eq1_deq_2());
       ESTM(reg2 = rd_ams_rx_eq1_deq());
       lane_st->EQ1_deQ = (reg1 << 2) | reg2;
       ESTM(reg1 = rd_ams_rx_eq2_deq2());
       ESTM(reg2 = rd_ams_rx_eq2_deq());
       lane_st->EQ2_deQ = (reg1 << 2) | reg2;
       ESTM(lane_st->VGA1_deQ = rd_ams_rx_vga1_deq());
       ESTM(lane_st->VGA2_deQ = rd_ams_rx_vga2_deq());
       ESTM(lane_st->vga_dis_ind = rd_ams_rx_vga_dis_ind());
    }
    ESTM(lane_st->blw_gain = rd_blw_gain());
    ESTM(lane_st->dummy4 = rdv_usr_status_eq_debug1());
    ESTM(lane_st->dummy5 = rdv_usr_status_eq_debug2());
    ESTM(lane_st->dummy6 = rdv_usr_status_eq_debug3());

    ESTM(lane_st->dummy7[0][0] = rdv_lhc_sts_0_0());
    ESTM(lane_st->dummy7[0][1] = rdv_lhc_sts_0_1());
    ESTM(lane_st->dummy7[1][0] = rdv_lhc_sts_1_0());
    ESTM(lane_st->dummy7[1][1] = rdv_lhc_sts_1_1());
    ESTM(lane_st->dummy7[2][0] = rdv_lhc_sts_2_0());
    ESTM(lane_st->dummy7[2][1] = rdv_lhc_sts_2_1());
    ESTM(lane_st->dummy7[3][0] = rdv_lhc_sts_3_0());
    ESTM(lane_st->dummy7[3][1] = rdv_lhc_sts_3_1());
    ESTM(lane_st->dummy7[4][0] = rdv_lhc_sts_4_0());
    ESTM(lane_st->dummy7[4][1] = rdv_lhc_sts_4_1());
    ESTM(lane_st->dummy7[5][0] = rdv_lhc_sts_5_0());
    ESTM(lane_st->dummy7[5][1] = rdv_lhc_sts_5_1());
    ESTM(lane_st->dummy8[0][0] = rdv_lvc_sts_0_0());
    ESTM(lane_st->dummy8[0][1] = rdv_lvc_sts_0_1());
    ESTM(lane_st->dummy8[1][0] = rdv_lvc_sts_1_0());
    ESTM(lane_st->dummy8[1][1] = rdv_lvc_sts_1_1());
    ESTM(lane_st->dummy8[2][0] = rdv_lvc_sts_2_0());
    ESTM(lane_st->dummy8[2][1] = rdv_lvc_sts_2_1());
    ESTM(lane_st->dummy8[3][0] = rdv_lvc_sts_3_0());
    ESTM(lane_st->dummy8[3][1] = rdv_lvc_sts_3_1());
    ESTM(lane_st->dummy8[4][0] = rdv_lvc_sts_4_0());
    ESTM(lane_st->dummy8[4][1] = rdv_lvc_sts_4_1());
    ESTM(lane_st->dummy8[5][0] = rdv_lvc_sts_5_0());
    ESTM(lane_st->dummy8[5][1] = rdv_lvc_sts_5_1());
    ESTM(lane_st->dummy9[0] = rdv_lthc_sts_0());
    ESTM(lane_st->dummy9[1] = rdv_lthc_sts_1());
    ESTM(lane_st->dummy9[2] = rdv_lthc_sts_2());
    ESTM(lane_st->dummy9[3] = rdv_lthc_sts_3());
    
    ESTM(lane_st->dummy10[0][0] = rdv_lc_sts_0_0_byte());
    ESTM(lane_st->dummy10[0][1] = rdv_lc_sts_0_1_byte());
    ESTM(lane_st->dummy10[1][0] = rdv_lc_sts_1_0_byte());
    ESTM(lane_st->dummy10[1][1] = rdv_lc_sts_1_1_byte());
    ESTM(lane_st->dummy10[2][0] = rdv_lc_sts_2_0_byte());
    ESTM(lane_st->dummy10[2][1] = rdv_lc_sts_2_1_byte());
    ESTM(lane_st->dummy10[3][0] = rdv_lc_sts_3_0_byte());
    ESTM(lane_st->dummy10[3][1] = rdv_lc_sts_3_1_byte());
    ESTM(lane_st->dummy10[4][0] = rdv_lc_sts_4_0_byte());
    ESTM(lane_st->dummy10[4][1] = rdv_lc_sts_4_1_byte());
    ESTM(lane_st->dummy10[5][0] = rdv_lc_sts_5_0_byte());
    ESTM(lane_st->dummy10[5][1] = rdv_lc_sts_5_1_byte());
    ESTM(lane_st->dummy11[0][0] = rdv_lhr_sts_0_0());
    ESTM(lane_st->dummy11[0][1] = rdv_lhr_sts_0_1());
    ESTM(lane_st->dummy11[1][0] = rdv_lhr_sts_1_0());
    ESTM(lane_st->dummy11[1][1] = rdv_lhr_sts_1_1());
    ESTM(lane_st->dummy11[2][0] = rdv_lhr_sts_2_0());
    ESTM(lane_st->dummy11[2][1] = rdv_lhr_sts_2_1());
    ESTM(lane_st->dummy11[3][0] = rdv_lhr_sts_3_0());
    ESTM(lane_st->dummy11[3][1] = rdv_lhr_sts_3_1());
    ESTM(lane_st->dummy11[4][0] = rdv_lhr_sts_4_0());
    ESTM(lane_st->dummy11[4][1] = rdv_lhr_sts_4_1());
    ESTM(lane_st->dummy11[5][0] = rdv_lhr_sts_5_0());
    ESTM(lane_st->dummy11[5][1] = rdv_lhr_sts_5_1());

    ESTM(lane_st->dummy12[0][0] = rdv_lvr_sts_0_0());
    ESTM(lane_st->dummy12[0][1] = rdv_lvr_sts_0_1());
    ESTM(lane_st->dummy12[1][0] = rdv_lvr_sts_1_0());
    ESTM(lane_st->dummy12[1][1] = rdv_lvr_sts_1_1());
    ESTM(lane_st->dummy12[2][0] = rdv_lvr_sts_2_0());
    ESTM(lane_st->dummy12[2][1] = rdv_lvr_sts_2_1());
    ESTM(lane_st->dummy12[3][0] = rdv_lvr_sts_3_0());
    ESTM(lane_st->dummy12[3][1] = rdv_lvr_sts_3_1());
    ESTM(lane_st->dummy12[4][0] = rdv_lvr_sts_4_0());
    ESTM(lane_st->dummy12[4][1] = rdv_lvr_sts_4_1());
    ESTM(lane_st->dummy12[5][0] = rdv_lvr_sts_5_0());
    ESTM(lane_st->dummy12[5][1] = rdv_lvr_sts_5_1());


    ESTM(lane_st->dummy13 = rdv_usr_status_eq_debug4());

    ESTM(lane_st->dummy14    = rdv_usr_status_eq_debug10());
    ESTM(lane_st->dummy15[0] = rdv_usr_status_eq_debug9());
    ESTM(lane_st->dummy15[1] = rdv_usr_status_eq_debug8());
    ESTM(lane_st->dummy16    = rdv_usr_status_eq_debug7());
    ESTM(lane_st->dummy17[0] = rdv_usr_status_eq_debug6());
    ESTM(lane_st->dummy17[1] = rdv_usr_status_eq_debug5());
    ESTM(lane_st->dummy18[0] = rdv_usr_status_tp_metric_1());
    ESTM(lane_st->dummy18[1] = rdv_usr_status_tp_metric_2());
    ESTM(lane_st->dummy18[2] = rdv_usr_status_tp_metric_3());
    ESTM(lane_st->dummy19     = rdv_usr_status_eq_debug14());
    EFUN(blackhawk7_v1l8p2_INTERNAL_get_eye_margin_est(sa__, &lane_st->heye_left, &lane_st->heye_right, &lane_st->veye_upper, &lane_st->veye_lower));
    ESTM(lane_st->link_time = (((uint32_t)rdv_usr_sts_link_time())*8)/10);

    if (lane_st->pmd_lock == 1) {
      if (!lane_st->stop_state) {
        EFUN(blackhawk7_v1l8p2_stop_rx_adaptation(sa__, 0));
      }
    } else {
        EFUN(blackhawk7_v1l8p2_stop_rx_adaptation(sa__, 0));
    }

    return(ERR_CODE_NONE);
}

    const char* blackhawk7_v1l8p2_e2s_rx_pam_mode_enum[3] = {
      "    NRZ",
      "PAM4_NR",
      "PAM4_ER"
    };

err_code_t blackhawk7_v1l8p2_log_full_pmd_state (srds_access_t *sa__, struct blackhawk7_v1l8p2_detailed_lane_status_st *lane_st) {
    enum srds_prbs_polynomial_enum prbs_poly_mode = PRBS_7;
    enum srds_prbs_checker_mode_enum prbs_checker_mode = PRBS_SELF_SYNC_HYSTERESIS;

    if(!lane_st) 
      return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    ENULL_MEMSET(lane_st, 0, sizeof(struct blackhawk7_v1l8p2_detailed_lane_status_st));
    EFUN(blackhawk7_v1l8p2_log_full_pmd_state_noPRBS(sa__, lane_st));

    ESTM(lane_st->prbs_chk_en = rd_prbs_chk_en());
    EFUN(blackhawk7_v1l8p2_get_rx_prbs_config(sa__, &prbs_poly_mode, &prbs_checker_mode, &lane_st->prbs_chk_inv));
    lane_st->prbs_chk_order = (uint8_t)prbs_poly_mode;

    EFUN(blackhawk7_v1l8p2_prbs_chk_lock_state(sa__, &lane_st->prbs_chk_lock));
    EFUN(blackhawk7_v1l8p2_prbs_err_count_ll(sa__, &lane_st->prbs_chk_errcnt));
    EFUN(blackhawk7_v1l8p2_INTERNAL_get_BER_string(sa__,100,&lane_st->ber_string[0]));

    return(ERR_CODE_NONE);
}


err_code_t blackhawk7_v1l8p2_disp_full_pmd_state (srds_access_t *sa__, struct blackhawk7_v1l8p2_detailed_lane_status_st const * const lane_st, uint8_t num_lanes) {
    const uint8_t num_bytes_each_line = 26;
    uint32_t i;
    uint32_t size_of_lane_st = 0;
    uint32_t one_u32 = 0x01000000;
    char * ptr = (char *)(&one_u32);
    const uint8_t big_endian = (ptr[0] == 1);


    if(lane_st == NULL) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if ((num_lanes != 4) && (num_lanes != 8) && (num_lanes != 1)) {
      return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));  /* Number of lanes has to be 1, 4 or 8 */
    }

    size_of_lane_st = sizeof(struct blackhawk7_v1l8p2_detailed_lane_status_st);

    EFUN_PRINTF(("\n**** SERDES EXTENDED LANE DIAG DATA DUMP ****"));
    EFUN_PRINTF(("\n0000 "));
    ESTM_PRINTF(("%02x ", num_lanes));
    ESTM_PRINTF(("%02x ", big_endian));
    /* For some customer builds, ESTM_PRINTF is an empty macro.
       In such cases, some compilers might generates an error of "unused variable" for big_endian.
       The void usage of big_endian down below - solves this compiler error */
    UNUSED(big_endian);
    for (i = 2; i < num_lanes*size_of_lane_st+2; i++) {
      if (!(i % num_bytes_each_line))  {
        EFUN_PRINTF(("\n%04x ", i));
      }
      ESTM_PRINTF(("%02x ", *(((uint8_t*)lane_st)+i-2)));
    }

    EFUN_PRINTF(("\n**** END OF DATA DUMP ****\n"));

    EFUN_PRINTF(( "\n"));

    return (ERR_CODE_NONE);
}

