/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :   *
 *                                                                                       *
 *  Description   :  Common types used by Serdes API functions                           *
 *                                                                                       *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                                 *
 *  No portions of this material may be reproduced in any form without                   *
 *  the written permission of:                                                           *
 *      Broadcom Corporation                                                             *
 *      5300 California Avenue                                                           *
 *      Irvine, CA  92617                                                                *
 *                                                                                       *
 *  All information contained in this document is Broadcom Corporation                   *
 *  company private proprietary, and trade secret.                                       *
 *                                                                                       *
 *****************************************************************************************
 *****************************************************************************************/

/** @file blackhawk_tsc_types.h
 * Common types used by Serdes API functions
 */

#ifndef BLACKHAWK_TSC_API_TYPES_H
#define BLACKHAWK_TSC_API_TYPES_H

#include "blackhawk_tsc_ipconfig.h"
#include "blackhawk_tsc_usr_includes.h"

/** TX FIR tap values */
typedef struct txfir {
  int16_t tap[12];
} blackhawk_tsc_txfir_st;

/*----------------------------------------*/
/*  Lane/Core structs (without bitfields) */
/*----------------------------------------*/

/** Lane Config Variable Structure in Microcode */
struct blackhawk_tsc_uc_lane_config_field_st {
  uint8_t  lane_cfg_from_pcs ;
  uint8_t  an_enabled        ;
  uint8_t  dfe_on            ;
  uint8_t  dfe_lp_mode       ;
  uint8_t  force_brdfe_on    ;
  uint8_t  media_type        ;
  uint8_t  unreliable_los    ;
  uint8_t  scrambling_dis    ;
  uint8_t  cl72_auto_polarity_en ;
  uint8_t  cl72_restart_timeout_en;
  uint8_t  force_es          ;
  uint8_t  force_ns          ;
  uint8_t  lp_has_prec_en    ;
  uint8_t  force_pam4_mode    ; /* used to override the pam4mode pin */
  uint8_t  force_nrz_mode     ; /* used to override the pam4mode pin */
};

/** Core Config Variable Structure in Microcode */
struct blackhawk_tsc_uc_core_config_field_st {
  uint8_t  vco_rate          ;
  uint8_t  core_cfg_from_pcs ;
  uint8_t  reserved          ;
};

/** Lane Config Struct */
struct  blackhawk_tsc_uc_lane_config_st {
  struct blackhawk_tsc_uc_lane_config_field_st field;
  uint16_t word;
};

/** Core Config Struct */
struct  blackhawk_tsc_uc_core_config_st {
  struct blackhawk_tsc_uc_core_config_field_st field;
  uint16_t word;
  int vco_rate_in_Mhz; /* if >0 then will get converted and replace field.vco_rate when update is called */
};

/** Lane User Control Function Structure in Microcode */
struct blackhawk_tsc_usr_ctrl_func_st {

  /** Main Peaking Filter Adaptation */
  uint16_t pf_adaptation           ;
  /** Low Frequency Peaking Filter adaptation */
  uint16_t pf2_adaptation          ;
  /** Low Frequency Peaking Filter adaptation */
  uint16_t pf3_adaptation          ;    
  /** DC Offset Adaptation */
  uint16_t dc_adaptation           ;
  /** Variable Gain Amplifier adaptation */
  uint16_t vga_adaptation          ;
  /** Slicer Vertical Offset tuning */
  uint16_t slicer_voffset_tuning   ;
  /** Slicer Horizontal Offset tuning */
  uint16_t slicer_hoffset_tuning   ;
  /** Phase Offset adaptation */
  uint16_t phase_offset_adaptation ;
  /** Eye adaptation */
  uint16_t eye_adaptation          ;
  /** All adaptation */
  uint16_t all_adaptation          ;
  uint16_t reserved                ;
};

/** Lane User DFE Control Function Structure in Microcode */
struct blackhawk_tsc_usr_ctrl_dfe_func_st {
  
  /** DFE tap 1 adaptation */
  uint8_t dfe_tap1_adaptation      ;
  /** DFE Fixed taps adaptation */
  uint8_t dfe_fx_taps_adaptation   ;
  /** DFE Floating taps adaptation */
  uint8_t dfe_fl_taps_adaptation   ;
  /** DFE tap Duty Cycle Distortion */
  uint8_t dfe_dcd_adaptation       ;
};

/** Lane User Control Disable Function Struct */
struct blackhawk_tsc_usr_ctrl_disable_functions_st {
  struct blackhawk_tsc_usr_ctrl_func_st field;
  uint16_t word;
};

/** Lane User Control Disable DFE Function Struct */
struct blackhawk_tsc_usr_ctrl_disable_dfe_functions_st {
  struct blackhawk_tsc_usr_ctrl_dfe_func_st field;
  uint8_t  byte;
};
struct ber_data_st {
    uint64_t num_errs;
    uint64_t num_bits;
    uint8_t lcklost;
    uint8_t prbs_chk_en;
};

/** Blackhawk Lane status Struct */
/* Please ensure that decreasing order of size is maintained while adding new elements to this structure */
/* If any 16/32/64 bits variable is added to the following struct, add that to the diag_data_logfile_decoder source file as well */
struct blackhawk_tsc_detailed_lane_status_st {
    /* =========== 32-bit variables start =========== */
    /** PRBS Error Count */
    uint32_t prbs_chk_errcnt;
    /* =========== 32-bit variables end ============= */
    /* =========== 16-bit variables start =========== */
    /** One Hot DSC State */
    uint16_t dsc_sm[2];
    /** Horizontal left eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16_t heye_left;
    /** Horizontal right eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16_t heye_right;
    /** Vertical upper eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16_t veye_upper;
    /** Vertical lower eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16_t veye_lower;
    /** Link time in milliseconds */
    uint16_t link_time;
    /** LMS Slicer Threshold */
    int16_t  lms_thresh;
    /** blw gain */
    uint16_t blw_gain;
    /** Frequency offset of local reference clock with respect to RX data in ppm */
    int16_t ppm;
    /** Estimate for Main tap */
    int16_t main_tap_est;
    /* TX equalization FIR tap weights */
    blackhawk_tsc_txfir_st txfir;
    /* =========== 16-bit variables end ============ */
    /* =========== 8-bit variables start =========== */
    /** RX PAM Mode */
    uint8_t rx_pam_mode;
    /** Order of PRBS Checker */
    uint8_t prbs_chk_order;
    /** Temperature Index */
    uint8_t  temp_idx;
    /** TX driver to disable high voltage control */
    uint8_t  ams_tx_drv_hv_disable;
    /** Analog Resistor Calibration value */
    uint8_t  ams_tx_ana_rescal;
    /** TX PAM Mode */
    uint8_t tx_pam_mode;    
    /** TX Precoder Enable */
    uint8_t tx_prec_en;    
    /* Are the TXFIR taps using the PAM4 range */
    uint8_t txfir_use_pam4_range;
    /* No of TXFIR Taps Enabled */
    uint8_t num_txfir_taps;
    /** Signal Detect */
    uint8_t  sigdet;
    /** PMD RX Lock */
    uint8_t  pmd_lock;
    /** TX PLL Select */
    uint8_t  pmd_tx_pll_select;
    /** RX PLL Select */
    uint8_t  pmd_rx_pll_select;
    /** Restart Count */
    uint8_t restart_count;
    /** Reset Count */
    uint8_t reset_count;
    /**  PMD Lock Count */
    uint8_t pmd_lock_count;
    /** Variable Gain Amplifier settings */
    uint8_t  vga;
    /** Main Peaking Filter setting */
    uint8_t  pf;
    /** Low Frequency Peaking Filter settings */
    uint8_t  pf2;
    /** High Frequency Peaking Filter settings */
    uint8_t  pf3;
    /** Data23 Slicer Threshold */
    int8_t   data23_thresh;
    /** Data14 Slicer Threshold */
    int8_t   data14_thresh;
    /** Data05 Slicer Threshold */
    int8_t   data05_thresh;
    /** Phase1 Slicer Threshold */
    int8_t   phase1_thresh;
    /** Phase02 Slicer Threshold */
    int8_t   phase02_thresh;
    /** Delay of zero crossing slicer, m1, wrt to data in PI codes */
    int8_t clk90;
    /** Horizontal offset between data and dataq */
    int8_t  ddq_hoffset;
    /** Horizontal offset between phase and phaseq */
    int8_t  ppq_hoffset;
    /** Horizontal offset between lms and lmsq */
    int8_t  llq_hoffset;
    /** Horizontal offset between data and phase */
    int8_t  dp_hoffset;
    /** Horizontal offset between data and lms */
    int8_t  dl_hoffset;
    int8_t  llq_hoffset_cal[2];
    int8_t  ppq_hoffset_cal[2];
    /** DFE tap values */
    int8_t  dfe[26][4];
    /** Dummy 1 */
    uint8_t dummy1;
    /** Dummy 2 */
    int8_t dummy2[4];
    /** Dummy 3 */
    int8_t dummy3[4];
    /** DC offset DAC control value */
    int8_t   dc_offset;
    /** Slicer calibration control codes (data0) */
    int8_t   thctrl_d0[4];
    /** Slicer calibration control codes (data1) */
    int8_t   thctrl_d1[4];
    /** Slicer calibration control codes (data2) */
    int8_t   thctrl_d2[4];
    /** Slicer calibration control codes (data3) */
    int8_t   thctrl_d3[4];
    /** Slicer calibration control codes (data4) */
    int8_t   thctrl_d4[4];
    /** Slicer calibration control codes (data5) */
    int8_t   thctrl_d5[4];
    /** Slicer calibration control codes (phase0) */
    int8_t   thctrl_p0[4];
    /** Slicer calibration control codes (phase1) */
    int8_t   thctrl_p1[4];
    /** Slicer calibration control codes (phase2) */
    int8_t   thctrl_p2[4];
    /** Slicer calibration control codes (lms) */
    int8_t   thctrl_l[4];
    /** PRBS Checker Enable */
    uint8_t  prbs_chk_en;
    /** PRBS Checker Invert */
    uint8_t  prbs_chk_inv;
    /** PRBS Checker Lock */
    uint8_t  prbs_chk_lock;
    /** lane_reset_state **/
    uint8_t reset_state;
    /** uC stopped state **/
    uint8_t stop_state;
    /** BER Error Rate string **/
    char ber_string[10];
    /** signal detect offset calibration pos */
    uint8_t ams_rx_sd_cal_pos;
    /** signal detect offset calibration neg */
    uint8_t ams_rx_sd_cal_neg;
    /** channel loss usr var */
    uint8_t pam4_chn_loss;
    /** vga bw control capacitor */
    uint8_t ams_rx_vga_bw_ctrl_cap;
    /** vga bw control inductor */
    uint8_t ams_rx_vga_bw_ctrl_ind;
    /** vga3 inductor*/
    uint8_t ams_rx_vga3_ind;
    /** pf3 max bw */
    uint8_t ams_rx_pf3_max_bw;
    /** pf max bw */
    uint8_t ams_rx_pf_max_bw;
    /** vga0 rescal mux */
    uint8_t ams_rx_vga0_rescal_mux;
    /** vga1 rescal mux */
    uint8_t ams_rx_vga1_rescal_mux;
    /** vga2 rescal mux */
    uint8_t ams_rx_vga2_rescal_mux;
    /** Dummy 4 */
    int8_t dummy4;
    /** Dummy 5 */
    int8_t dummy5;
    /** Dummy 6 */
    uint8_t dummy6;
    /** Dummy 7 */
    int8_t dummy7[6][4];
    /** Dummy 8 */
    int8_t dummy8[6][4];
    /** Dummy 9 */
    int8_t dummy9[6];
    /** Dummy 10 */
    uint8_t dummy10[6][4];
    /** Dummy 11 */
    int8_t dummy11[6][4];
    /** Dummy 12 */
    int8_t dummy12[6][4];
    /** Dummy 13 */
    int8_t dummy13;
    /* =========== 8-bit variables end ============== */
};

/****************************************************************************
 * @name Diagnostic Sampling
 *
 ****************************************************************************/
/**@{*/

#ifdef STANDALONE_EVENT
#define DIAG_MAX_SAMPLES (64)
#else

/**
 * Diagnostic sample set size, BLACKHAWK variant.
 * Applies to collections of BER measurements, eye margins, etc.
 */
#define DIAG_MAX_SAMPLES (64)
#endif

/**@}*/

#endif
