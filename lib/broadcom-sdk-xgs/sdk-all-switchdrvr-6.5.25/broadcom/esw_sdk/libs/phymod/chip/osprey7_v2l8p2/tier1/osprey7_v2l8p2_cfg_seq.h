/*----------------------------------------------------------------------
 * $Id: osprey_v2l8p2_cfg_seq.h,v 1.1.2.2 Broadcom SDK $
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : osprey_v2l8p2_cfg_seq.h
 * Description: c functions implementing Tier1s for Osprey Serdes Driver
 *---------------------------------------------------------------------*/
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  $Id$
*/


#ifndef OSPREY7_V2L8P2_CFG_SEQ_H
#define OSPREY7_V2L8P2_CFG_SEQ_H

#include "common/srds_api_err_code.h"
#include <phymod/phymod_diagnostics.h>

typedef struct {
  int8_t pll_pwrdn;
  int8_t tx_s_pwrdn;
  int8_t rx_s_pwrdn;
} power_status_t;

typedef struct {
  int8_t revid_model;
  int8_t revid_process;
  int8_t revid_bonding;
  int8_t revid_rev_number;
  int8_t revid_rev_letter;
} osprey7_v2l8p2_rev_id0_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp;
  int8_t revid_pir;
  int8_t revid_cl72;
  int8_t revid_micro;
  int8_t revid_mdio;
  int8_t revid_multiplicity;
} osprey7_v2l8p2_rev_id1_t;

typedef enum {
  TX = 0,
  Rx
} tx_rx_t;

typedef enum {
    OSPREY_PRBS_POLYNOMIAL_7 = 0,
    OSPREY_PRBS_POLYNOMIAL_9,
    OSPREY_PRBS_POLYNOMIAL_11,
    OSPREY_PRBS_POLYNOMIAL_15,
    OSPREY_PRBS_POLYNOMIAL_23,
    OSPREY_PRBS_POLYNOMIAL_31,
    OSPREY_PRBS_POLYNOMIAL_58,
    OSPREY_PRBS_POLYNOMIAL_TYPE_COUNT
} osprey7_v2l8p2_prbs_polynomial_type_t;

#define PATTERN_MAX_SIZE 8


extern err_code_t _osprey7_v2l8p2_pmd_mwr_reg_byte(phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val);
extern uint8_t _osprey7_v2l8p2_pmd_rde_field_byte(phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);
extern uint16_t _osprey7_v2l8p2_pmd_rde_field(phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);
err_code_t osprey7_v2l8p2_tx_pi_control_get(phymod_access_t *pa,  int16_t *value);
err_code_t osprey7_v2l8p2_tx_rx_polarity_set(phymod_access_t *pa, uint32_t tx_pol, uint32_t rx_pol);
err_code_t osprey7_v2l8p2_tx_rx_polarity_get(phymod_access_t *pa, uint32_t *tx_pol, uint32_t *rx_pol);
err_code_t osprey7_v2l8p2_uc_active_set(phymod_access_t *pa, uint32_t enable);
err_code_t osprey7_v2l8p2_uc_active_get(phymod_access_t *pa, uint32_t *enable);
err_code_t osprey7_v2l8p2_pmd_force_signal_detect(phymod_access_t *pa, uint8_t force_en, uint8_t force_val);
err_code_t osprey7_v2l8p2_pmd_force_signal_detect_get(phymod_access_t *sa__, uint8_t *force_en, uint8_t *force_val);
err_code_t osprey7_v2l8p2_dig_lpbk_get(phymod_access_t *pa, uint32_t *lpbk);
err_code_t osprey7_v2l8p2_rmt_lpbk_get(phymod_access_t *pa, uint32_t *lpbk);
err_code_t osprey7_v2l8p2_pmd_lane_map_get(phymod_access_t *pa, uint32_t *tx_lane_map, uint32_t *rx_lane_map);
err_code_t osprey7_v2l8p2_pmd_ln_h_rstb_pkill_override(phymod_access_t *pa, uint16_t val);
err_code_t osprey7_v2l8p2_lane_soft_reset(phymod_access_t *pa, uint32_t enable);   /* pmd core soft reset */
err_code_t osprey7_v2l8p2_lane_soft_reset_get(phymod_access_t *pa, uint32_t *enable);
err_code_t osprey7_v2l8p2_lane_hard_soft_reset_release(phymod_access_t *pa, uint32_t enable);
err_code_t osprey7_v2l8p2_clause72_control(phymod_access_t *pc, uint32_t cl_72_en);        /* CLAUSE_72_CONTROL */
err_code_t osprey7_v2l8p2_clause72_control_get(phymod_access_t *pc, uint32_t *cl_72_en);   /* CLAUSE_72_CONTROL */
err_code_t osprey7_v2l8p2_pmd_cl72_receiver_status(phymod_access_t *pa, uint32_t *status);
err_code_t osprey7_v2l8p2_ucode_init(phymod_access_t *pa );
err_code_t osprey7_v2l8p2_pram_firmware_enable(phymod_access_t *pa, int enable, int wait);
err_code_t osprey7_v2l8p2_signal_detect(phymod_access_t *pa, uint32_t *signal_detect);
err_code_t osprey7_v2l8p2_tx_shared_patt_gen_en_get(phymod_access_t *pa, uint8_t *enable);
err_code_t osprey7_v2l8p2_config_shared_tx_pattern_idx_get(phymod_access_t *pa, uint32_t *pattern_len, uint32_t *pattern);
err_code_t osprey7_v2l8p2_tx_disable_get(phymod_access_t *pa, uint8_t *enable);
err_code_t osprey7_v2l8p2_comclk_set(phymod_access_t *pa, phymod_ref_clk_t ref_clock);

err_code_t osprey7_v2l8p2_channel_loss_set(phymod_access_t *sa__, uint32_t loss_in_db);
err_code_t osprey7_v2l8p2_channel_loss_get(phymod_access_t *sa__, uint32_t *loss_in_db);
err_code_t osprey7_v2l8p2_tx_tap_mode_get(phymod_access_t *sa__, uint8_t *mode);
err_code_t osprey7_v2l8p2_pam4_tx_pattern_enable_get(phymod_access_t *sa__, phymod_PAM4_tx_pattern_t pattern_type, uint32_t* enable);
err_code_t osprey7_v2l8p2_signalling_mode_status_get(phymod_access_t *sa__, phymod_phy_signalling_method_t *mode);
err_code_t osprey7_v2l8p2_tx_nrz_mode_get(phymod_access_t *sa__, uint16_t *tx_nrz_mode);
err_code_t osprey7_v2l8p2_tx_pam4_precoder_enable_set(phymod_access_t *sa__, int enable);
err_code_t osprey7_v2l8p2_tx_pam4_precoder_enable_get(phymod_access_t *sa__, int *enable);

 /* Get the PLL powerdown status */
err_code_t osprey7_v2l8p2_pll_pwrdn_get(phymod_access_t *sa__, uint32_t *is_pwrdn);
err_code_t osprey7_v2l8p2_rx_ppm(phymod_access_t *sa__, int16_t *rx_ppm);
/* Set/Get clk4sync_en, clk4sync_div */
err_code_t osprey7_v2l8p2_clk4sync_enable_set(phymod_access_t *sa__, uint32_t en, uint32_t div);
err_code_t osprey7_v2l8p2_clk4sync_enable_get(phymod_access_t *sa__, uint32_t *en, uint32_t *div);
err_code_t osprey7_v2l8p2_pll_lock_get(phymod_access_t *sa__, uint32_t *pll_lock);
err_code_t osprey7_v2l8p2_lane_pll_selection_set( phymod_access_t *pa, uint32_t pll_index);
err_code_t osprey7_v2l8p2_lane_pll_selection_get( phymod_access_t *pa, uint32_t *pll_index);
err_code_t osprey7_v2l8p2_rx_protect_nrzmux_set(phymod_access_t *sa__, uint8_t mux_val);
err_code_t osprey7_v2l8p2_force_os_cdr_get(phymod_access_t *sa__, uint32_t *enabled);
err_code_t osprey7_v2l8p2_rx_pmd_lock_status_get(phymod_access_t *sa__, uint32_t *pmd_rx_locked, uint32_t *pmd_lock_changed);
err_code_t osprey7_v2l8p2_lane_dp_reset_state_get(phymod_access_t *sa__, uint32_t *reset_state);

#endif /* OSPREY7_V2L8P2_CFG_SEQ_H */
