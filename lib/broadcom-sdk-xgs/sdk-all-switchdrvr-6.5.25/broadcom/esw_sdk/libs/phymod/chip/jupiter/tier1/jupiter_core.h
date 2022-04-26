#ifndef JUPITER_CORE_H
#define JUPITER_CORE_H

#ifdef NON_SDK
#include <stdint.h>
#else
#include <phymod/phymod.h>
#endif

#include "jupiter_core_csr_defines.h"
#include "jupiter_core_ip_defines.h"
#include "jupiter_dependencies.h"

#define CHECK(x) do { int err = (x); if (err) return err; } while (0)

/**
 * Forward Declare Access Struct.
 * 
 * This must be defined in the driver file to access the CSR space of the
 * Jupiter100 IP
 * 
 */ 
// typedef struct mss_access_s mss_access_t;

/**
 * typedef jupiter_err_code_t  - Jupiter MSS Error Codes Enum
 */
typedef enum jupiter_err_code_e {
    JUPITER_ERR_CODE_NONE = 0, /* function success (no error) */
    JUPITER_ERR_CODE_POLL_TIMEOUT = 1, /* function containing a poll did not successfully complete */
    JUPITER_ERR_CODE_INVALID_ARG_VALUE = 2, /* invalid function argument value */
    JUPITER_ERR_CODE_WRITE_FAILURE = 3, /* returned by pmd_write_field if unsuccessful */
    JUPITER_ERR_CODE_READ_FAILURE = 4, /* returned by pmd_read_field if unsuccessful */
    JUPITER_ERR_CODE_BAD_STATE = 5, /* if unexpected values occurred, usually within *_get functions */
    JUPITER_ERR_CODE_FUNC_FAILURE = 6, /* function did not complete successfully */
    JUPITER_ERR_CODE_CHECK_FAILURE = 7 /* function did not return the expected value */
} jupiter_err_code_t;

/*
 * Auto negotiation config struct
 *
 */
typedef struct jupiter_lt_cfg_s {
    /**
     * @lt_polynomial_sel_c92: Polynomial select for technologies using clause
     *                         92 training. Refer to databook for
     *                         eth_lt_prbs[lt_polynomial_sel_c92] mapping.
     */
    uint32_t lt_polynomial_sel_c92;
    /**
     * @lt_polynomial_sel_c136: Polynomial select for technologies using clause 136
     *                          training. Refer to databook for eth_lt_prbs[lt_polynomial_sel_c136]
     *                          mapping.
     */
    uint32_t lt_polynomial_sel_c136;
    /**
     * @lt_seed_11b: Link training frame PRBS seed, for technologies using
     *               Clause 72/92 training. Refer to databook for
     *               eth_lt_prbs[lt_seed_11b] mapping.
     */
    uint32_t lt_seed_11b;
    /**
     * @lt_seed_13b: Link training frame PRBS seed, for technologies using
     *               Clause 136 training. Refer to databook for
     *               eth_lt_prbs[lt_seed_13b] mapping.
     */
    uint32_t lt_seed_13b;
    /**
     * @lt_ms_per_ck: Number of reference clock cycles for 1ms minus 1.
     *                Ex. For refclk of 100MHz, set to 99999
     */
    uint32_t lt_ms_per_ck;
} jupiter_lt_cfg_t;

typedef struct jupiter_lt_status_s {
    uint32_t lt_running; /* 0 = not running, 1 = running */
    uint32_t lt_done; /* 0 = not done, 1 = done */
    uint32_t lt_failure; /* 0 - link training no fail, 1 - link training fail */
    uint32_t lt_rx_ready; /* 0 - local RX / remote TX not trained, 1 - local RX / remote TX trained */
} jupiter_lt_status_t;

typedef enum jupiter_refclk_term_mode_e {
    JUPITER_RC_HI_Z = 0, /* REF CLK high impedance */
    JUPITER_RC_R50_SE = 1, /* REF CLK 50 ohms per leg single ended to ground */
    JUPITER_RC_R100_DF = 2, /* REF CLK 100 ohms differential */
    /* UNUSED = 3,   here for clarity */
} jupiter_refclk_term_mode_t;

typedef enum jupiter_acc_term_mode_e {
    JUPITER_ACC_HI_Z = 0,
    JUPITER_ACC_TERM_VSS_AC = 1, /* termination to vss, onchip AC coupling */
    JUPITER_ACC_TERM_FL_AC = 2, /* floating termination, onchip AC coupling */
    /* RES1 = 3,  Reserved */
    /* RES2 = 4,  Reserved */
    JUPITER_ACC_TERM_VSS_DC = 5, /* termination to vss, DC coupled */
    JUPITER_ACC_TERM_FL_DC = 6, /* termination to vss, DC coupled */
    /* RES3 = 7,  Reserved */
} jupiter_acc_term_mode_t;

typedef enum jupiter_force_sigdet_mode_e {
    /**
     * Will set force_invalid=1 and force_valid=0 will force signal detect
     * value to 0, regardless of RX data presence
     */
    JUPITER_SIGDET_FORCE0 = 0,

    /**
     * Will setting force_valid=1 and force_invalid=0 will forces signal
     * detect value to 1, regardless of RX data presence
     */
    JUPITER_SIGDET_FORCE1 = 1,

    /**
     * setting force_valid=0 and force_invalid=0 so signal detect value
     * will not be forced. Signal detect is now dependent on RX data presence
     */
    JUPITER_SIGDET_NORM = 2,
} jupiter_force_sigdet_mode_t;

/*
enum jupiter_txfir_cfg_taps_e {
    JUPITER_CM3 = 0, // pre-cursor 3
    JUPITER_CM2 = 1, // pre-cursor 2
    JUPITER_CM1 = 2, // pre-cursor 1
    JUPITER_C0  = 3, // max elements
    JUPITER_C1  = 4, // post-cursor 1
    JUPITER_TXFIR_MAX_TAPS = 5
} jupiter_txfir_cfg_taps_t;
*/

/**
 * 'CM3': c(-3) value (pre-cursor 3), up to 4 max, 3b
 * 'CM2': c(-2) value  (pre-cursor 2), up to 7 max, 3b
 * 'CM1': c(-1) value  (pre-cursor 1), up to 24 max, 6b
 * 'C0': c(0) value  (main cursor), up to 60 max, 6b
 * 'C1': c(1) value  (post-cursor 1), up to 24 max, 6b
 */
typedef struct jupiter_txfir_config_s {
    uint32_t CM3;
    uint32_t CM2;
    uint32_t CM1;
    uint32_t C0;
    uint32_t C1;
    uint32_t main_or_max;
} jupiter_txfir_config_t;

/*
 * State Request Struct
 *
 * Same for TX and RX
 */
typedef enum jupiter_state_rate_e {
  JUPITER_NRZ_1p25_2p5 = 0, /* 1.25/2.5 Gbps NRZ Auto-Neg */
  JUPITER_NRZ_10p3125 = 1, /* 10.3125 Gbps NRZ (10G) */
  JUPITER_NRZ_25p78125 = 2, /* 25.78125 Gbps NRZ (25G) */
  JUPITER_NRZ_26p5625 = 3, /* 26.5625 Gbps NRZ (26G AUI) */
  JUPITER_PAM4_53p125 = 4, /* 53.125 Gbps PAM4 (50G) */
  JUPITER_NRZ_53p125 = 5, /* 53.125 Gbps NRZ (50G) */
  JUPITER_PAM4_106p25 = 6, /* 106.25 Gbps PAM4 (100G) */
  JUPITER_MAX_RATES = 7,
} jupiter_state_rate_t;

/*
 * Link Training Mode Struct
 *
 * Same for TX and RX
 */
typedef enum jupiter_training_mode_e {
  JUPITER_DISABLED = 0,
  JUPITER_CL72 = 1,
  JUPITER_CL92 = 2,
  JUPITER_CL136 = 3,
  JUPITER_CL162 = 4,
  JUPITER_MAX_CFG = 5,
} jupiter_training_mode_t;

/** JUPITER is reserving this struct and it's corresponding function in this
 * release. If there is a usecase for it, it will be updated in a future
 * release. If not, it will be removed.
 */
/*typedef struct lane_cfg_s {
 *    RESERVED
 * } lane_cfg_t; */

typedef enum jupiter_pll_pstatus_e {
    JUPITER_PWR_DOWN = 0, /* PLL power down */
    JUPITER_PWR_UP = 1, /* PLL power up */
    JUPITER_ST_CHNG = 2 /* PLL State Changing */
} jupiter_pll_pstatus_t;


/**
 * Return adapted FFE Tap values in the following type. Index 0
 * corresponds Tap 0, index 1 to tap 1 and so on all the way to tap 20
 */
typedef uint32_t jupiter_ffe_t[JUPITER_FFE_NUM_TAPS];

typedef struct jupiter_thresholds_s {
    /**
     * Return adapted threshold eh settings for specified branch.
     */
    uint32_t eh;
    /**
     * Return adapted threshold ez settings for specified branch.
     */
    uint32_t ez;
    /**
     * Return adapted threshold el settings for specified branch.
     */
    uint32_t el;
    /**
     * Return adapted threshold lower target adaptation settings for specified branch.
     */
    uint32_t thres_low;
    /**
     * Return adapted threshold upper target adaptation settings for specified branch.
     */
    uint32_t thres_hi;
} jupiter_thresholds_t;

/**
 * Slicer Levels Struct
 */
typedef struct jupiter_slicers_s {
    /**
     *        'slicer_eh3': Return adapted slicer eh3 settings for specified branch.
     */
    uint32_t eh3;
    /**
     *        'slicer_eh1': Return adapted slicer eh1 settings for specified branch.
     */
    uint32_t eh1;
    /**
     *        'slicer_el1': Return adapted slicer el1 settings for specified branch.
     */
    uint32_t el1;
    /**
     *        'slicer_el3': Return adapted slicer el3 settings for specified branch.
     */
    uint32_t el3;
} jupiter_slicers_t;

/* all uint32_t are signed, need to include custom precisions layout */
typedef struct jupiter_dsp_param_s {
    /* Return adapted DC offset value for specified branch */
    uint32_t dc_offset;
    /* Return adapted DFE value for specified branch. */
    uint32_t dfe;
    jupiter_ffe_t ffe;
    jupiter_thresholds_t thresholds;
    jupiter_slicers_t slicers;
} jupiter_dsp_param_t;

typedef struct jupiter_dcdiq_data_s {
    /**
     *   Returns calibrated duty cycle code for d0 clock
     */
    uint32_t d0;
    /**
    *    Returns calibrated duty cycle code for d90 clock
     */
    uint32_t d90;
    /**
    *    Returns calibrated IQ phase code between d0 (I) and d90 (Q) clock
     */
    uint32_t iq;
} jupiter_dcdiq_data_t;


typedef struct jupiter_afe_data_s {
    /**
     * Automatically set based on baudrate. 1 - For 106/112Gbps datarates. 0 - All other datarates.
     */
    uint32_t ctle_rate;
    /**
     * Nyquist boost setting ranging from codes 0 to 12.
     */
    uint32_t ctle_boost;
    /**
     * Coarse VGA code, automatically adapted during foreground adapt to ensure signal going into ADC is within 80-90% full scale. Range is from 0-31.
     */
    uint32_t vga_coarse;
    /**
     * Fine VGA code, automatically adapted during background due to voltage and temperature fluctuations and ensures signal going into ADC is within 80-90% full scale. Range is from 0-63.
     */
    uint32_t vga_fine;
    /**
     * VGA offset code, automatically adapted during foreground, correcting for residual offset at the output of the VGA
     */
    uint32_t vga_offset;
} jupiter_afe_data_t;

/**
 * Master FSM Diagnostics Struct
 *
 * NOTE: this struct may change
 */
typedef struct jupiter_uc_diag_regs_s {
    uint32_t rxmfsm_rate_cur;
    uint32_t rxmfsm_rate_new;
    uint32_t rxmfsm_width_cur;
    uint32_t rxmfsm_width_new;
    uint32_t rxmfsm_rxdisable;
    uint32_t rxmfsm_req;
    uint32_t rxmfsm_power_cur;
    uint32_t rxmfsm_power_new;
    uint32_t rxmfsm_pam_cur;
    uint32_t rxmfsm_pam_new;
    uint32_t rxmfsm_pam_ctrl_cur;
    uint32_t rxmfsm_pam_ctrl_new;
    uint32_t rxmfsm_instr_num;
    uint32_t rxmfsm_state;
    uint32_t rxiffsm_state;
    uint32_t rx_log0;
    uint32_t rx_log1;
    uint32_t rx_log2;
    uint32_t rx_log3;
    uint32_t rx_log4;
    uint32_t rx_log5;
    uint32_t rx_log6;
    uint32_t rx_log7;
    uint32_t rx_log8;
    uint32_t txmfsm_rate_cur;
    uint32_t txmfsm_rate_new;
    uint32_t txmfsm_width_cur;
    uint32_t txmfsm_width_new;
    uint32_t txmfsm_power_cur;
    uint32_t txmfsm_power_new;
    uint32_t txmfsm_pam_cur;
    uint32_t txmfsm_pam_new;
    uint32_t txmfsm_instr_num;
    uint32_t txmfsm_req;
    uint32_t txmfsm_state;
    uint32_t txiffsm_state;
    uint32_t tx_log0;
    uint32_t tx_log1;
    uint32_t tx_log2;
    uint32_t tx_log3;
    uint32_t tx_log4;
    uint32_t tx_log5;
    uint32_t tx_log6;
    uint32_t tx_log7;
    uint32_t tx_log8;
    uint32_t tx_log9;
    uint32_t cmnmfsm_rate_cur;
    uint32_t cmnmfsm_instr_num;
    uint32_t cmnmfsm_power_cur;
    uint32_t cmnmfsm_power_new;
    uint32_t cmnmfsm_rate_new;
    uint32_t cmnmfsm_req;
    uint32_t cmnmfsm_state;
    uint32_t cmniffsm_state;
    uint32_t cmn_log0;
    uint32_t cmn_log1;
    uint32_t cmn_log2;
    uint32_t cmn_log3;
    uint32_t cmn_log4;
    uint32_t cmn_log5;
    uint32_t cmn_log6;
    uint32_t cmn_log7;
    uint32_t cmn_log8;
    uint32_t cmn_log9;
} jupiter_uc_diag_regs_t;

typedef enum jupiter_bist_pattern_e {
    JUPITER_PRBS7 = 0,
    JUPITER_PRBS9 = 1,
    JUPITER_PRBS11 = 2,
    JUPITER_PRBS13 = 3,
    JUPITER_PRBS15 = 4,
    JUPITER_PRBS23 = 5,
    JUPITER_PRBS31 = 6,
    JUPITER_QPRBS13 = 7,
    JUPITER_JP03A = 8,
    JUPITER_JP03B = 9,
    JUPITER_LINEARITY_PATTERN = 10,
    JUPITER_USER_DEFINED_PATTERN = 11,
    JUPITER_FULL_RATE_CLOCK = 12,
    JUPITER_HALF_RATE_CLOCK = 13,
    JUPITER_QUARTER_RATE_CLOCK = 14,
    JUPITER_PATT_32_1S_32_0S = 15, /* 32 1s and 32 0s repeating */
    JUPITER_BIST_PATTERN_MAX = 16 /* Always at the end */
} jupiter_bist_pattern_t;

typedef enum jupiter_bist_mode_e {
    JUPITER_TIMER = 0, /* will run for a number of defined cycles */
    JUPITER_DWELL = 1, /* wall clock mode, doing a test longer than 80ms, this needs to be used */
} jupiter_bist_mode_t;

typedef enum jupiter_eq_type_e {
    JUPITER_EQ_FULL_DIR = 0, /* Full EQ, Directional */
    JUPITER_EQ_EVAL_DIR = 1, /* Eval Only, Directional */
    JUPITER_EQ_INIT_EVAL = 2, /* Init Eval */
    JUPITER_EQ_CLEAR_EVAL = 3, /* Clear Eval */
    JUPITER_EQ_FULL_FOM = 4, /* Full EQ, FOM */
    JUPITER_EQ_EVAL_FOM = 5 /* Eval Only, FOM */
} jupiter_eq_type_t;

typedef enum jupiter_pstate_e {
    JUPITER_P0   = 0,
    JUPITER_P0S  = 1,
    JUPITER_P1   = 2,
    JUPITER_P2   = 3,
    JUPITER_PD   = 4,
    JUPITER_L1_0 = 5,
    JUPITER_L1_1 = 6,
    JUPITER_L1_2 = 7
} jupiter_pstate_t;

typedef enum jupiter_cmn_pstate_e {
    JUPITER_CMN_PD   = 0,
    JUPITER_CMN_P0   = 1
} jupiter_cmn_pstate_t;

/**
 *
 * Used to decode data width value
 *
 */  
uint32_t jupiter_width_decoder (uint32_t width_encoded);

//ANLT APIs

/**
 * jupiter_pmd_link_training_config_set() - Configure PMD Link Training
 * @mss: serdes access struct
 * @lt_cfg: Link training config struct
 *
 * Set various ethernet-related link training parameters in the ANLT block.
 *
 * Constraints:
 *   Power State:  Don't care.
 *   Rate State:   After desired rate change.
 *   EqEval State: Before eqeval.
 *   Effect:       Takes effect during ANLT.
 *
 * Typical Application Usage:
 *     - Used to re-configure polynomial and seed prior to starting ANLT.
 *
 * Notes:
 *     - Seed ordering is as in ethernet specification, MSB is transmitted
 *       first
 *     - Link training is controlled by ports, there is no register control
 *     - There is only configuration setup which is defined by this function
 *     - Firmware is not capable of figuring out what clause to run, as that
 *       is determined by ports
 */
int jupiter_pmd_link_training_config_set(mss_access_t *mss, jupiter_lt_cfg_t lt_cfg);

/**
 * jupiter_pmd_link_training_config_get() - Get PMD Link Training Config
 * @lt_cfg: Return pointer of link training config struct
 *
 * Return various ethernet-related link training parameters in the ANLT block.
 *
 * Constraints:
 *      - Can be read anytime
 *
 * Typical Application Usage:
 *      - Get current polynomial and seed state after setting.
 */
int jupiter_pmd_link_training_config_get(mss_access_t *mss, jupiter_lt_cfg_t* lt_cfg);

/**
 * jupiter_pmd_link_training_status_get() - Return link training state
 * @lt_status: Pointer to return result of link training status
 *
 * Return:
 *    `lt_status`: Pointer to jupiter_lt_status_t Struct
 *        'lt_running':
 *            0 - link training not running
 *            1 - link training running
 *        'lt_training_failure':
 *            0 - link training not failed
 *            1 - link training failed
 *        'lt_rx_ready':
 *            0 - local RX / remote TX not trained
 *            1 - local RX / remote TX trained
 *
 * Constraints:
 *    Can be read anytime.
 *
 * Typical Application Usage:
 *    Get current ANLT link training status.
 */
int jupiter_pmd_link_training_status_get(mss_access_t *mss, jupiter_lt_status_t *lt_status);


/**Get the termination mode of the reference clock input pads, depending on if the macro is at the end of the reference clock trace.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *    'lsrefbuf_term_mode': Enum type for jupiter_refclk_term_mode_t
 *
 * Constraints:
 *    Power State:  During PD.
 *    Rate State:   Before desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    In the scenario where multiple macros are sharing the same on-board refclk, set the termination to ground if the PHY macro that is physically furthest away from on-board refclk source. The other PHY macros should be set to high impedance.
 */
int jupiter_pmd_refclk_termination_set(mss_access_t *mss, jupiter_refclk_term_mode_t lsrefbuf_term_mode);

/**Set the termination mode of the reference clock input pads, depending on if the macro is at the end of the reference clock trace.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'lsrefbuf_term_mode': Enum type for jupiter_refclk_term_mode_t
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During PD.
 *    Rate State:   Before desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    In the scenario where multiple macros are sharing the same on-board refclk, set the termination to ground if the PHY macro that is physically furthest away from on-board refclk source. The other PHY macros should be set to high impedance.
 */
int jupiter_pmd_refclk_termination_get(mss_access_t *mss, jupiter_refclk_term_mode_t *lsrefbuf_term_mode);

/**Set the RX termination mode, depending on the channels used.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'acc_term_mode': jupiter_acc_term_mode_t enum setting the AFE On-Chip RX Termination.
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During PD.
 *    Rate State:   Before desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    RX termination setting changes depending on prescence of on-board coupling capacitor.
 */
int jupiter_pmd_rx_termination_set(mss_access_t *mss, jupiter_acc_term_mode_t acc_term_mode);

/**Set the RX termination mode, depending on the channels used.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *    'acc_term_mode': Return pointer enum for AFE On-Chip RX Termination value.
 *
 * Constraints:
 *    Power State:  During PD.
 *    Rate State:   Before desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    RX termination setting changes depending on prescence of on-board coupling capacitor.
 */
int jupiter_pmd_rx_termination_get(mss_access_t *mss, jupiter_acc_term_mode_t *acc_term_mode);

/**Force signal detect to user-specified value. If forced valid, CDR will attempt to lock to data, regardless of presence of RX Data. If forced invalid, CDR will lock to the reference clock.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'sigdet_mode': jupiter_force_sigdet_mode_t enum type
 *
 * Return:
 *    JUPITER Error Indicator Enum
 * 
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 * 
 * Typical Application Usage:
 *    Force signal detect is used in scenarios where electrically idle RX data is very noisy, typical in optical systems. Excessive noise during RX electrical idle can falsely trigger signal detect to enable the CDR during P0.
 */
int jupiter_pmd_force_signal_detect_config_set(mss_access_t *mss, jupiter_force_sigdet_mode_t sigdet_mode);

/**Return forced signal detect value.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    `lane`: lane number
 *
 * Return:
 *    'sigdet_mode': Return pointer to jupiter_force_sigdet_mode_t enum type
 *
 * Constraints:
 *    Can be used anytime.
 * 
 * Typical Application Usage:
 *    Used to confirm current signal detect state.
 */
int jupiter_pmd_force_signal_detect_config_get(mss_access_t *mss, jupiter_force_sigdet_mode_t *sigdet_mode);

/**Set TX disable when TX electric idle is asserted.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'tx_disable':
 *        0 - enable TX
 *        1 - disable TX
 *
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Used to disable TX output, when the corresponding override is enabled.
 */
int jupiter_pmd_tx_disable_set(mss_access_t *mss, uint32_t tx_disable);

/**Return if TX disabled.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'tx_disable_override':
 *        0 - TX disable override disabled
 *        1 - TX disable override enabled
 *    'tx_disable':
 *        0 - TX enabled
 *        1 - TX disabled
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current TX disable value.
 */
int jupiter_pmd_tx_disable_get(mss_access_t *mss, uint32_t *tx_disable_override, uint32_t *tx_disable);

// NOTE: these are commented out, and not included in the API spec, but JUPITER is
//       reserving in case there is a future usecase
// int jupiter_pmd_rx_disable_pin_override_set(mss_access_t *mss, uint32_t override_enable);
// int jupiter_pmd_rx_disable_pin_override_get(mss_access_t *mss, uint32_t *override_enable);
// int jupiter_pmd_rx_disable_set(mss_access_t *mss, uint32_t rx_disable);
// int jupiter_pmd_rx_disable_get(mss_access_t *mss, uint32_t *rx_disable);
// ENDNOTE

/**Set value for each TX FIR tap settings upon TX lane reset. Sum of all the cursor values should be 60 at max.  
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'txfir_cfg': Struct containing all the TX FIR configuration settings
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Should be called before de-asserting TX lane reset.
 *
 * Typical Application Usage:
 *    When using PHY as a TX, call this function to adjust TX FIR tap weights. This is only applicable where link training is not performed.
 */
int jupiter_pmd_txfir_config_reset_set(mss_access_t *mss, jupiter_txfir_config_t txfir_cfg);


/**Set value for each TX FIR tap settings to override adapted value. Sum of all the cursor values should be 60 at max.  
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'txfir_cfg': Struct containing all the TX FIR configuration settings
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Cannot be called while link training is running. Should only be called when TX lane reset is not asserted.
 *
 * Typical Application Usage:
 *    When using PHY as a TX, call this function to adjust TX FIR tap weights.
 */
int jupiter_pmd_txfir_config_set(mss_access_t *mss, jupiter_txfir_config_t txfir_cfg);

/**Return value for each TX tap setting written by txfir_config_set function.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'txfir_cfg': Return pointer to TX FIR config struct
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current FIR settings.
 */
int jupiter_pmd_txfir_config_get(mss_access_t *mss, jupiter_txfir_config_t *txfir_cfg);

/**Return mode (NRZ or PAM4), max tap range for each TX FIR tap, and the number of tap drivers that are on
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'max_tap_range_cm3': max tap range for pre cursor 3
 *    'max_tap_range_cm2': max tap range for pre cursor 2
 *    'max_tap_range_cm1': max tap range for pre cursor 1
 *    'max_tap_range_c0': max tap range for main cursor
 *    'max_tap_range_c1': max tap range for post cursor 1
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get return current TX FIR settings and its' limits.
 */
int jupiter_pmd_tx_tap_mode_get(uint32_t *max_rng_cm3, uint32_t *max_rng_cm2, uint32_t *max_rng_cm1, uint32_t *max_rng_c1, uint32_t *max_rng_c0);

/**Enable/disable PAM4 precoder for the given lane during ANLT.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'en': Set Precoder enabled or disabled
 *        0 - disable precoder
 *        1 - enable precoder
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    For links with high insertion loss, enabling precoder may help to lower BER.
 */
int jupiter_pmd_tx_pam4_precoder_override_set(mss_access_t *mss, uint32_t en);

/**Return if the PAM4 precoder is enabled/disabled for the given lane.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'en': Return pointer to if the TX precoder is enabled
 *        0 - PAM4 precoder disabled
 *        1 - PAM4 precoder enabled
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current TX precoder setting used during ANLT.
 */
int jupiter_pmd_tx_pam4_precoder_override_get(mss_access_t *mss, uint32_t *en);

/**Enable/disable TX PAM4 precoder for the given lane during ANLT.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'gray_en':  
 *        0 - gray encoding disable  
 *        1 - gray encoding enable  
 *    'plusd_en':  
 *        0 - plusd encoding disable  
 *        1 - plusd encoding enable  
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    For links with high insertion loss, enabling precoder may help to lower BER.
 */
int jupiter_pmd_tx_pam4_precoder_enable_set(mss_access_t *mss, uint32_t gray_en, uint32_t plusd_en);

/**Return if the TX PAM4 gray/plusd encoding is enabled/disabled for the given lane. 
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'gray_en':  
 *        0 - gray encoding disabled  
 *        1 - gray encoding enabled  
 *    'plusd_en':  
 *        0 - plusd encoding disabled   
 *        1 - plusd encoding enabled   
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current Tx precoder setting.
 */
int jupiter_pmd_tx_pam4_precoder_enable_get(mss_access_t *mss, uint32_t *gray_en, uint32_t *plusd_en);

/**Enable/disable RX PAM4 gray encoding override for the given lane.  
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'en':  
 *        0 - rx_demapper register will control the rx gray encoding mapping  
 *        1 - rx_cntrl_reg2[rx_gray_ena_nt] register will control the rx gray encoding mapping
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    For links with high insertion loss, enabling precoder may help to lower BER.
 */
int jupiter_pmd_rx_pam4_precoder_override_set(mss_access_t *mss, uint32_t en);

/**Return if the RX PAM4 gray encoding override is enabled/disabled for the given lane.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'en':  
 *        0 - rx_demapper register will control the rx gray encoding mapping  
 *        1 - rx_cntrl_reg2[rx_gray_ena_nt] register will control the rx gray encoding mapping
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current Rx precoder setting.
 */
int jupiter_pmd_rx_pam4_precoder_override_get(mss_access_t *mss, uint32_t *en);


/**Enable/disable remote far-end parallel (FEP) loopback (RX -> TX) for the given lane.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'remote_loopback_enable': Set the remote loopback enabled or disabled
 *        0 - remote loopback disabled
 *        1 - remote loopback enabled
 *
 * Return:
 *    'err':
 *        0 - Success, physical lane mapping matches logical lane mapping
 *        1 - Error, physical lane mapping does not match logical lane mapping
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: After EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Used to enable far-end parallel loopback for looping back RX data to TX.
 */
int jupiter_pmd_remote_loopback_set(mss_access_t *mss, uint32_t remote_loopback_enable);

/**Return if the remote far-end parallel (FEP) loopback is enabled/disabled.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 * 
 * Return:
 *    'remote_loopback_enable': Return pointer to remote loopback enable value
 *        0 - remote loopback disabled
 *        1 - remote loopback enabled
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current FEP setting.
 */
int jupiter_pmd_remote_loopback_get(mss_access_t *mss, uint32_t *remote_loopback_enable);

/**Enable/disable analog local near-end serial (NES) loopback (TX -> RX) for the given lane.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'analog_loopback_enable':
 *        0 - analog loopback disabled
 *        1 - analog loopback enabled
 *
 * Return:
 *    'err':
 *        0 - Success, physical lane mapping matches logical lane mapping
 *        1 - Error, physical lane mapping does not match logical lane mapping
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Used to enable on-die TX to RX loopback, typically used during ATE.
 */
int jupiter_pmd_analog_loopback_set(mss_access_t *mss, uint32_t analog_loopback_enable);

/**Return if the analog local near-end serial (NES) loopback is enabled/disabled.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'analog_loopback_enable': Return pointer to value of analog loopback value
 *        -1 - error
 *        0 - analog loopback disabled
 *        1 - analog loopback enabled
 * 
 *    'err': returns err = 1 if any logical lane number != physical lane number
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current near-end serial loopback setting.
 */
int jupiter_pmd_analog_loopback_get(mss_access_t *mss, uint32_t *analog_loopback_enable);

/**Set TX polarity for a given lane.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'tx_pol_flip': TX Polarity value
 *        0 - normal polarity
 *        1 - inverted polarity
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During PD.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    If the polarity of the TX is physically inverted in package or board, this function can be used to invert.
 */
int jupiter_pmd_tx_polarity_set(mss_access_t *mss, uint32_t tx_pol_flip);

/**Return TX polarity for a given lane.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 * 
 * Return:
 *    'tx_pol': Return pointer to value of tx polarity flip value
 *        0 - normal TX polarity
 *        1 - inverted TX polarity
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current TX polarity settings.
 */
int jupiter_pmd_tx_polarity_get(mss_access_t *mss, uint32_t *tx_pol_flip);

/**Set RX polarity for a given lane.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'rx_pol': Value for RX Polarity Flip
 *        0 - normal polarity
 *        1 - inverted polarity
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During PD.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    If the polarity of the RX is physically inverted in package or board, this function can be used to invert.
 */
int jupiter_pmd_rx_polarity_set(mss_access_t *mss, uint32_t rx_pol_flip);

/**Return RX polarity for a given lane.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'rx_pol': Return pointer for value of RX Polarity Flip Value
 *        0 - normal RX polarity
 *        1 - inverted RX polarity
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current RX polarity settings.
 */
int jupiter_pmd_rx_polarity_get(mss_access_t *mss, uint32_t *rx_pol_flip);

/**Set TX hbridge, used to adjust TX PAM4 eye linearity. Set msb and lsb to 30 first. Increase lsb to make inner eye smaller. Decrease lsb to make inner eye larger.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'msb': adjust swing for TX output
 *    'lsb': adjust inner eye size
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Certain applications such as optical drivers require PAM4 eye linearity to be adjusted.
 */
int jupiter_pmd_tx_hbridge_set(mss_access_t *mss, uint32_t msb, uint32_t lsb);

/**Returns TX hbridge settings, used to adjust TX PAM4 eye linearity.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'msb': Return pointer for value of most significant bitswing for TX output
 *    'lsb': Return pointer for value of least significant bit inner eye size
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current hbridge settings.
 */
int jupiter_pmd_tx_hbridge_get(mss_access_t *mss, uint32_t *msb, uint32_t *lsb);

/* -----------------------------------------------------------------------------
 * int jupiter_pmd_lane_cfg_set(mss_access_t *mss, lane_cfg_t lane_cfg);  RESERVED
 * int jupiter_pmd_lane_cfg_get(mss_access_t *mss, lane_cfg_t *lane_cfg); RESERVED
 * ----------------------------------------------------------------------------- */

/**Enables or disables DFE adaptation. If DFE adapt is disabled, DFE coefficient will default to 0.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'dfe_adapt_enable':
 *        0 - DFE adapt disabled
 *        1 - DFE adapt enabled
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    When the channel has low loss (ie. short), DFE can be disabled for power savings and slight speed up to running equalization.
 */
int jupiter_pmd_rx_dfe_adapt_set(mss_access_t *mss, uint32_t dfe_adapt_enable);

/**Returns DFE enable setting.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'en': Return pointer for DFE enable setting
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get the dfe enable setting.
 */
int jupiter_pmd_rx_dfe_adapt_get(mss_access_t *mss, uint32_t *dfe_adapt_enable);

/**Enables or disables CTLE adaptation. If ctle adapt is disabled, ctle boost code will default to opts['ctle_boost_a'].
 *   If ctle adapt is enabled, initial ctle boost value will default to 0.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'ctle_adapt_enable':
 *        0 - CTLE adapt disabled
 *        1 - CTLE adapt enabled
 *    'ctle_boost_a': Forced ctle boost code when 'en' is 0. Ranges from 0-12.
 *
 * Return:
 *    Null
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    When the channel used is short and low loss, CTLE adaptation can be disabled.
 */
int jupiter_pmd_rx_ctle_adapt_set(mss_access_t *mss, uint32_t ctle_adapt_enable, uint32_t ctle_boost_a);

/**Returns CTLE adapt enable setting.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'ctle_adapt_enable': Return pointer for CTLE adapt enable
 *    'ctle_boost_a': Return pointer for adapted or forced CTLE boost code
 *
 * Constraints:
 *    Can be used anytime.
 */
int jupiter_pmd_rx_ctle_adapt_get(mss_access_t *mss, uint32_t *ctle_adapt_enable, uint32_t *ctle_boost_a);

/**Enables or disables background adapt. When enabled, VGA fine, FFE, DFE, DC offset, slicers and thresholds are continuously adapted.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'rx_bkgrnd_adapt_enable':
 *        0 - Background adapt disable
 *        1 - Background adapt enable
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Don't care.
 *    Rate State:   After desired rate change.
 *    EqEval State: Must be run when foreground adapts are complete
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Background is typically enabled to continuously adapt RX coefficients non-destructively due to temperature and/or voltage fluctuations.
 */
int jupiter_pmd_rx_background_adapt_enable_set(mss_access_t *mss, uint32_t rx_bkgrnd_adapt_enable);

/**Returns background enable adapt setting.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'rx_bkgrnd_adapt_enable': Return pointer for background adapt enable
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get the background adapt enable setting.
 */
int jupiter_pmd_rx_background_adapt_enable_get(mss_access_t *mss, uint32_t *rx_bkgrnd_adapt_enable);

/**Enables or disables auto equalization. When enabled, receiver will automatically adapt when RX signal is present.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'rx_autoeq_enable':
 *        0 - Auto equalization enable
 *        1 - Auto equalization disable
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    It is recommended to enable this setting if a minimum BER is required prior to link training.
 *    This setting is typically enabled in PCIe applications where BER must be <1e-4 prior to running EqEval.
 */
int jupiter_pmd_rx_autoeq_set(mss_access_t *mss, uint32_t rx_autoeq_enable);

/**Returns Auto equalization enable adapt setting.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    `lane`: lane number
 *
 * Return:
 *    'rx_autoeq_enable': Return pointer for Auto Eq enable
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get the autoeq enable setting.
 */
int jupiter_pmd_rx_autoeq_get(mss_access_t *mss, uint32_t *rx_autoeq_enable);

/**Set VGA Load cap value
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'vga_cap': VGA Load cap value for all VGA coarse values
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *   When the channel used is short and low loss, vga_cap should be set to max value. To avoid adapting this value, disable CTLE adapt. 
 */
int jupiter_pmd_rx_vga_cap_set(mss_access_t *mss, uint32_t vga_cap);

/**Set VGA Load cap value
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'vga_cap': VGA Load cap value for all VGA coarse values
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *   When the channel used is short and low loss, vga_cap should be set to max value. To avoid adapting this value, disable CTLE adapt. 
 */
int jupiter_pmd_rx_vga_cap_adapt_set(mss_access_t *mss, uint32_t en);

/**Sets the enable for VGA cap adaption
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'en':
 *        0 - vga_cap adapt disabled
 *        1 - vga_cap adapt enabled
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Don't care
 *    Rate State:   Don't care
 *    EqEval State: Before EqEval/link training
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *   vga_cap adaptation is usually enabled in link training.
 */
int jupiter_pmd_rx_c0_adapt_set(mss_access_t *mss, uint32_t en);

/**Sets the enable for TX FIR c0 adaptation
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'en':
 *        0 - c0 adapt disabled
 *        1 - c0 adapt enabled
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Don't care
 *    Rate State:   Don't care
 *    EqEval State: Before EqEval/link training
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    TX FIR c0 adaptation is usually enabled in link training.
 */
int jupiter_pmd_rx_vga_cap_get(mss_access_t *mss, uint32_t *vga_cap); 

/**Sets CDR offset and direction
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'cdr_offset' : Sets CDR offset value (8bits)
 *    'cdr_dir' : Sets CDR direction
 *          0 - Negative direction
 *          1 - Positive direction
 *
 * Return:
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *   Should be used to improve post-MLSD BER, and to a lesser degree raw BER. This is very channel dependent and needs to be adapted/swept to find optimal results.
 */
int jupiter_pmd_rx_cdr_offset_set(mss_access_t *mss, uint32_t cdr_offset, uint32_t cdr_dir);

/**Set RX signal detect state.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'en' : Enable bit
 *
 * Return:
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before or after EqEval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Use this function to enable the digital signal detect block.
 */
int jupiter_pmd_rx_signal_detect_en_set(mss_access_t *mss, uint32_t en);

/**Get state of RX signal detect.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'signal_detect':
 *        0 - signal detect low
 *        1 - signal detect high
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Use this function to report if there is data coming into RX and being detected.
 */
int jupiter_pmd_rx_signal_detect_get(mss_access_t *mss, uint32_t *signal_detect);

/**Check state of RX signal detect.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'signal_detect_expected' : expectef value for signal_detect
 *
 * Return:
 *    Will return non-zero if check fails
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Use this function to report if there is data coming into RX and being detected.
 */
int jupiter_pmd_rx_signal_detect_check(mss_access_t *mss, uint32_t signal_detect_expected);

/**Read TX clk PPM offset for a given lane. Desired PPM accuracy is 1/2**timing_window-1.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'timing_window': Sampling measurement window to be `2**timing_window-1` clock cycles
 *    'timeout_us': polling iteration limit in us
 *
 * Return:
 *    'err':
 *        0 - Polling successful, ppm value is valid
 *        1 - Polling unsuccesful, ppm value is invalid
 *    'tx_ppm': Return TX clock ppm offset value
 *
 * Constraints:
 *    Used when PLL is locked, in P1 or P0 state.
 *
 * Typical Application Usage:
 *    Use this function to report the PPM offset between reference clock and VCO clock.
 */
int jupiter_pmd_tx_ppm_get(mss_access_t *mss, uint32_t timing_window, uint32_t timeout_us, USR_DOUBLE *tx_ppm);


/**Read RX clk PPM offset for a given lane. Desired PPM accuracy is 1/2**timing_window-1.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'timing_window': Sampling measurement window to be `2**timing_window-1` clock cycles
 *    'timeout_us': polling iteration limit in us
 *
 * Return:
 *    'err':
 *        0 - Polling successful, ppm value is valid
 *        1 - Polling unsuccesful, ppm value is invalid
 *    'rx_ppm': Return RX clock ppm offset value
 *
 * Constraints:
 *    Used when PLL is locked, in P1 or P0 state.
 *
 * Typical Application Usage:
 *    Use this function to report the PPM offset between reference clock and VCO clock.
 */
int jupiter_pmd_rx_ppm_get(mss_access_t *mss, uint32_t timing_window, uint32_t timeout_us, USR_DOUBLE *rx_ppm);

/**Return if RX CDR is locked onto incoming data.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'pmd_rx_lock': Return pointer for RX CDR Lock value
 *        0 - RX CDR not locked
 *        1 - RX CDR locked
 *
 * Constraints:
 *    Used when PLL is locked, in P1 or P0 state.
 *
 * Typical Application Usage:
 *    Use this function to determine if CDR is locked to incoming data.
 */
int jupiter_pmd_rx_lock_status_get(mss_access_t *mss, uint32_t *pmd_rx_lock);

/**Reports the adapted receiver digital signal processing (DSP) parameters, used for debugging signal integrity issues involving the lane receiver.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'branch':
 *       0-63 - Report statistics for specified branch value
 *
 * Return:
 *    'dsp_info': Return pointer to DSP parameter struct
 *
 *
 * Constraints:
 *    After EqEval is finished (after octl_rx_linkeval_ack_ln[#] returns high).
 *
 * Typical Application Usage:
 *    When experiencing signal integrity issues such as high BER, run this function to dump all DSP related parameters after running EqEval.
 */
int jupiter_pmd_rx_dsp_get(mss_access_t *mss, uint32_t branch, jupiter_dsp_param_t *dsp_info);

/**Return adapted codes for duty cycle distortion (DCD) and IQ phase on RX.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'rx_dcdiq_data': Return pointer to DCD and IQ data struct
 *
 *
 * Constraints:
 *    Get when RX is in P0 state after desired rate change.
 */
int jupiter_pmd_rx_dcdiq_get (mss_access_t *mss, jupiter_dcdiq_data_t *rx_dcdiq_data);

/**Return adapted codes for duty cycle distortion (DCD) and IQ phase on TX.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'tx_dcdiq_data': Return pointer to DCD and IQ data struct
 *
 * Constraints:
 *    Get when RX is in P0 state after desired rate change.
 */
int jupiter_pmd_tx_dcdiq_get (mss_access_t *mss, jupiter_dcdiq_data_t *tx_dcdiq_data);

/**Return adapted receiver analog front-end (AFE) parameters, used for signal integrity analysis.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'rx_afe_data': Return pointer to AFE data struct
 *
 *
 * Constraints:
 *    After EqEval is finished (after octl_rx_linkeval_ack_ln[#] returns high).
 *
 * Typical Application Usage:
 *    When experiencing signal integrity issues such as high BER, run this function to dump all DSP related parameters.
 */
int jupiter_pmd_rx_afe_get (mss_access_t *mss, jupiter_afe_data_t *rx_afe_data);

/* BIST APIs */
/* ----------------------------------------------------------------------------- */

/**Configure RX BIST pattern.
 *
 *Will disable RX BIST and clear BIST error counter first.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'pattern': Set the pattern to one defined by jupiter_bist_pattern_t type
 *    'UDP': 64bit user defined pattern
 *    'mode': jupiter_bist_mode_t enum
 *        0 - timer mode
 *        1 - wall clock mode 
 *    'lock_thresh': the number of cycles before entering lock state
 *    'timer_thresh': number of cycles for timer mode
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: After eqeval.
 *    Effect:       Takes effect immediately.
 *    Data:         RX is receiving PRBS data.
 *
 * Typical Application Usage:
 *    Configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0.
 */
int jupiter_pmd_rx_chk_config_set(mss_access_t *mss, jupiter_bist_pattern_t pattern, jupiter_bist_mode_t mode, uint64_t udp, uint32_t lock_thresh, uint32_t timer_thresh);

/**Return RX BIST configuration.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'pattern': Return pointer to jupiter_bist_pattern_t type
 *    'UDP': Return pointer to 64bit user defined pattern if in UDP mode
 *    'mode': Return enum of the bist mode
 *        0 - timer mode
 *        1 - wall clock mode
 *    'lock_threshold': Return pointer to the number of cycles before entering lock state value
 *    'timer_thres': Return pointer to the number of cycles for timer mode value
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Get current RX BIST configuration settings.
 */
int jupiter_pmd_rx_chk_config_get(mss_access_t *mss, jupiter_bist_pattern_t *pattern, jupiter_bist_mode_t *mode, uint64_t *udp, uint32_t *lock_thresh, uint32_t *timer_thresh);

/**Enable/disable RX BIST.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'enable':
 *        0 - disable RX BIST
 *        1 - enable RX BIST
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: After eqeval.
 *    Effect:       Takes effect immediately.
 *    Data:         RX is receiving PRBS data.
 *
 * Typical Application Usage:
 *    Configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0.
 */
int jupiter_pmd_rx_chk_en_set(mss_access_t *mss, uint32_t enable);

/**Return the RX BIST enable/disable.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'enable': Return pointer to RX BIST enable value
 *        0 - RX BIST disabled
 *        1 - RX BIST enabled
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Get current RX BIST configuration settings.
 */
int jupiter_pmd_rx_chk_en_get(mss_access_t *mss, uint32_t *enable);

/**Return the RX BIST lock state.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'rx_bist_lock': Return pointer to check if RX BIST is locked
 *        0 - RX BIST not locked
 *        1 - RX BIST locked
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: After eqeval.
 *    Effect:       Takes effect immediately.
 *    Data:         RX is receiving PRBS data.
 *
 * Typical Application Usage:
 *    Configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0.
 */
int jupiter_pmd_rx_chk_lock_state_get(mss_access_t *mss, uint32_t *rx_bist_lock);

/**Read RX BIST error counts, if the error count is done, and if the error counter is overflown.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'err_count': Return pointer to error count
 *    'err_count_done': Return pointer to err count done
 *        0 - error counting not done
 *        1 - error counting done
 *    'err_count_overflown': Return pointer to error count overflow
 *        0 - error counter not overflown
 *        1 - error counter overflown
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: After eqeval.
 *    Effect:       Takes effect immediately.
 *    Data:         RX is receiving PRBS data.
 *
 * Typical Application Usage:
 *    Configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0.
 */
int jupiter_pmd_rx_chk_err_count_state_get(mss_access_t *mss, uint64_t *err_count, uint32_t *err_count_done, uint32_t *err_count_overflown);

/**Clear RX BIST error counter.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Clear the error counter after it overflows, or to reset the counting.
 */
int jupiter_pmd_rx_chk_err_count_state_clear(mss_access_t *mss);

/**Configure TX BIST pattern.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'pattern': Set the TX BIST Pattern
 *    'user_defined_pattern': Set TX 64bit user defined pattern
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0.
 */
int jupiter_pmd_tx_gen_config_set(mss_access_t *mss, jupiter_bist_pattern_t pattern, uint64_t user_defined_pattern);

/**Return TX BIST pattern configuration.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'pattern': Return pointer to TX BIST pattern enum
 *    'user_defined_pattern': Return pointer to 64bit user defined pattern value
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Get current TX BIST setting.
 */
int jupiter_pmd_tx_gen_config_get(mss_access_t *mss, jupiter_bist_pattern_t *pattern, uint64_t *user_defined_pattern);

/**Enable/disable TX BIST.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'enable': Enable value for TX BIST
 *        0 - disable TX BIST
 *        1 - enable TX BIST
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0.
 */
int jupiter_pmd_tx_gen_en_set(mss_access_t *mss, uint32_t enable);

/**Return TX BIST enable/disable status.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 * 
 * Return:
 *    'enable': Return pointer to TX BIST enable value
 *        0 - TX BIST disabled
 *        1 - TX BIST enabled
 *
 * Constraints:
 *    Can be used anytime.
 */
int jupiter_pmd_tx_gen_en_get(mss_access_t *mss, uint32_t *enable);

/**Configure TX error injection pattern and rate.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'err_pattern': 64bit error injection pattern
 *    'err_rate':
 *        0 - inject 1 bit error
 *        not 0 - error injection rate
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *    Data:         When TX is outputting PRBS pattern.
 *
 * Typical Application Usage:
 *    In a very low BER NRZ system (<1e-12), configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0. An error is injected and error count should increment by 1.
 *
 *    Error injection is not typically useful in PAM4 system because  BER is typically >1e-10 and the error counter has already started accumulating errors.
 */
int jupiter_pmd_tx_gen_err_inject_config_set(mss_access_t *mss, uint64_t err_pattern, uint32_t err_rate);

/**Return TX error injection pattern and rate.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'err_pattern': Return pointer to 64bit error injection pattern
 *    'err_rate': Return pointer to error rate value
 *        0 - inject 1 bit error
 *        not 0 - error injection rate
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to confirm error injection rate.
 */
int jupiter_pmd_tx_gen_err_inject_config_get(mss_access_t *mss, uint64_t *err_pattern, uint32_t *err_rate);

/**Enable/disable TX error injection.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'enable':
 *        0 - disable error injection
 *        1 - enable error injection
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *    Data:         When TX is outputting PRBS pattern.
 *
 * Typical Application Usage:
 *    Configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0.
 *}
 */
int jupiter_pmd_tx_gen_err_inject_en_set(mss_access_t *mss, uint32_t enable);

/**Return TX error injection enable/disable.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'enable': Return pointer to TX Error inject value
 *        0 - TX error injection disabled
 *        1 - TX error injection enabled
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to confirm error injection rate.
 */
int jupiter_pmd_tx_gen_err_inject_en_get(mss_access_t *mss, uint32_t *enable);

/**Sets the RX demapper settings to automatically account for custom gray code mapping (PAM4 only) and polarity inversion (PAM4 or NRZ) using BIST. BIST needs to be configured prior to calling this function.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'npam4_nrz':
 *        0 - PAM4 mode
 *        1 - NRZ mode
 *    'timeout_us': iteration limits to poll for RX BIST lock for each invert/gray encoding setting. Recommended to set to 5.
 *
 * Return:
 *    'locked':
 *        0 - Unable to find appropriate demapper/inversion settings. Please check PRBS setting or equalization setting.
 *        1 - RX BIST is locked and found appropriate demapper settings.
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: After EqEval
 *    Effect:       Takes effect immediately.
 *    Data:         When RX receiving PRBS pattern.
 *
 * Typical Application Usage:
 *    When using the PHY as a local RX and the remote TX PAM4 gray code mapping is unknown, run this function to automatically sweep for the correct gray code mapping to match that of the remote TX.
 */
int jupiter_pmd_rx_sweep_demapper(mss_access_t *mss, uint32_t npam4_nrz, uint32_t timeout_us);

/* NOTE: reserving this function here incase it proves to be useful. */
/* int jupiter_pmd_rx_demapper_get(mss_access_t *mss, uint32_t rx_demapper); */

/* uC APIs */
/* ----------------------------------------------------------------------------- */
/** Default method for loading firmware and pointers.
 * 
 * Will load firmware and pointers via the PMI and PRAM interface.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'ucode_arr': Pointer to a 2d array containing address,value information
 *    'size': Size of the array
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: After EqEval
 *    Effect:       Takes effect immediately.
 *    Data:         When RX receiving PRBS pattern.
 *
 * Typical Application Usage:
 *    Used for loading the sequencing configuration for the PMD while in reset. 
 */
int jupiter_pmd_uc_ucode_load(mss_access_t *mss, uint32_t (*ucode_arr)[2], uint32_t size);

int c_load_hexfile(mss_access_t *mss, char * fileName);

/**Set pcs_clk_b gate enable.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'enable': Enable for pcs_clk_b_gate
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Set when CMN is in PD state.
 *
 * Typical Application Usage:
 *    This function is used to enable or disable pcs_clk_b for SoC use.
 */
int jupiter_pmd_ock_pcs_clk_b_gate_set(mss_access_t *mss, uint32_t enable);

/**Return value for pcs_clk_b gate enable.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'enable': Pointer to return enable result for pcs_clk_b_gate
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Get current setting for enabling pcs_clk_b used by SoC.
 */
int jupiter_pmd_ock_pcs_clk_b_gate_get(mss_access_t *mss, uint32_t *enable);

/**Set the override to allow the PCS or the software to override TX disable.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'override_enable':
 *        0 - disable override for TX disable
 *        1 - enable override for TX disable
 *
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    Power State:  Before or during P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Used to set the override for disabling TX output in P0.
 */
int jupiter_pmd_tx_disable_pin_override_set(mss_access_t *mss, uint32_t override_enable);

/* Return preset configuration of RX/TX rate and training mode for given lane
 * (returns the values of the interface input pins related to rate and training
 * mode selection) */

/**Return preset configuration of TX rate and training mode for given lane. (returns the values of the interface input pins related to rate and training mode selection)
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'tx_state_rate': rate state struct for TX rate preset
 *    'tx_training_mode': TX training mode preset struct
 *
 ***Constraints:
 *    Can be used anytime. 
 *
 ***Typical Application Usage:
 *    Used to get current rate preset and training mode.
 */
int jupiter_pmd_tx_preset_get(mss_access_t *mss, jupiter_state_rate_t *tx_rate_preset, jupiter_training_mode_t *tx_training_mode);

/**Return preset configuration of RX rate and training mode for given lane. (returns the values of the interface input pins related to rate and training mode selection)
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'rx_state_rate': rate state struct for RX rate preset
 *    'rx_training_mode': RX training mode preset struct
 *
 * Constraints:
 *    Can be used anytime. 
 *
 * Typical Application Usage:
 *    Used to get current rate preset and training mode.
 */
int jupiter_pmd_rx_preset_get(mss_access_t *mss, jupiter_state_rate_t *rx_rate_preset, jupiter_training_mode_t *rx_training_mode);

/**Return LCPLL lock status.
 *
 * Note: This is a register set by the MFSM when it has brought up the LCPLL, and is a one shot. Does not indicate if PLL has somehow lost lock.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 * 
 * Return:
 *    'pll_lock': Return LCPLL lock return value
 *        0 - PLL not locked
 *        1 - PLL locked
 *
 * Constraints:
 *    Used when PLL is locked, in P1 or P0 state.
 *
 * Typical Application Usage:
 *    Use this function to determine if CMN PLL is locked to reference clock.
 */
int jupiter_pmd_pll_lock_get(mss_access_t *mss, uint32_t *pll_lock);

/**Return PLL powerdown status.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'pd': Return pointer to value of pll_pwrdn_status enum
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Use this function to report current state of the PLL.
 */
int jupiter_pmd_pll_pwrdn_get(mss_access_t *mss, jupiter_pll_pstatus_t *pll_pwrdn_status);

/**Set the physical to logical lane mapping.
 *
 * The registers correspond to the logical lanes.
 * The values to write to the registers correspond to the physical lanes.
 * For the physical lane values, lane0 = 1, lane1 = 2, ...
 * 
 * As an example, tx_lane_map_arr[0] = 1 means logical lane 0 is mapped to
 * physical lane 1. Max array size should be no larger than the number of
 * lanes in the IP. The number of lanes is defined in the macro JUPITER_NUM_LANES.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *    'tx_lane_map_arr': Pointer to array containing the Physical to Logical
 *                       lane mapping
 *    'rx_lane_map_arr': Pointer to array containing the Physical to Logical
 *                       lane mapping
 *
 * Return:
 *    JUPITER Error Indicator Enum
 * 
 * Constraints:
 *    Power State:  During PD.
 *    Rate State:   Before desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Used to re-adjust physical to logical lane mapping.
 */
int jupiter_pmd_lane_map_set(mss_access_t *mss, uint32_t const *tx_lane_map_arr, uint32_t const *rx_lane_map_arr);

/**Return the physical to logical lane mapping.
 *
 * Uses the same array mapping as defined in the jupiter_pmd_lane_map_set function.
 *
 * Args:
 *    `mss`: Object containing driver methods.
 *
 * Return:
 *    'tx_lane_map_arr': Return pointer to array containing the Physical to
 *                       logical lane mapping
 *    'rx_lane_map_arr': Return pointer to array containing the Physical to
 *                       logical lane mapping
 *
 * Constraints:
 *    Can be used anytime.
 *
 * Typical Application Usage:
 *    Used to get current physical to logical lane mapping.
 */
int jupiter_pmd_lane_map_get(mss_access_t *mss, uint32_t *tx_lane_map_arr, uint32_t *rx_lane_map_arr);

/**
 * Clear the CRC calculator
 * 
 * Typical Application Usage:
 *    If using the above loading methods, this clear function is called
 *    before doing any loading so it should not be needed.
 */
int jupiter_pmd_uc_ucode_crc_clear(mss_access_t *mss);

/* Same as above, but loads firmware via PMI interface only */
int jupiter_pmd_uc_ucode_pmi_load(mss_access_t *mss, uint32_t **ucode_arr, uint32_t size);

/**Load FW memory using mdio interface. This is used for ATE access and does not require PCS to be powered up.
 * 
 * Same as above function
 *
 * Return:
 *    JUPITER Error Indicator Enum
 *
 * Constraints:
 *    In place of the normal firmware loading.
 */
int jupiter_pmd_uc_ucode_mdio_load(mss_access_t *mss, uint32_t **ucode_arr, uint32_t ucode_len);

/**
 * Compares the Hardware CRC 32 value with one calculated by software
 * 
 * Args:
 *    'expected_crc': The calculated CRC in software. This is be used to compare with the hardware CRC.
 * 
 * Typical Application Usage:
 *    Used to check that firmware is getting loaded correctly.
 */
int jupiter_pmd_uc_ucode_crc_verify(mss_access_t *mss, uint32_t expected_crc);

/**
 * Dump PHY Sequencing Configuration Debug information
 * 
 * Return:
 *    'uc_diag': Pointer to a struct containing all the values of all the debug registers for PHY FW and sequencing
 * 
 * Typical Application Usage:
 *    Used for debug.
 */
int jupiter_pmd_uc_diag_reg_dump(mss_access_t *mss, jupiter_uc_diag_regs_t *uc_diag);

/**
 * Enable diagnostic logging for the different lane sequencers
 * 
 * Args:
 *    'uc_log_cmn_en': Enable logging for the common sequencer
 *    'uc_log_tx_en':  Enable logging for the TX sequencer
 *    'uc_log_rx_en':  Enable logging for the RX sequencer
 * 
 * Return:
 * 
 * Typical Application Usage:
 *    Used for debug.
 */
int jupiter_pmd_uc_diag_logging_en_set(mss_access_t *mss, uint32_t uc_log_cmn_en, uint32_t uc_log_tx_en, uint32_t uc_log_rx_en);

/**
 * Get the enable for diagnostic logging for the different lane sequencers
 * 
 * Args:
 * 
 * Return:
 *    'uc_log_cmn_en': Return pointer to the enable logging for the common sequencer
 *    'uc_log_tx_en':  Return pointer to the enable logging for the TX sequencer
 *    'uc_log_rx_en':  Return pointer to the enable logging for the RX sequencer
 * 
 * Typical Application Usage:
 *    Used for debug.
 */
int jupiter_pmd_uc_diag_logging_en_get(mss_access_t *mss, uint32_t *uc_log_cmn_en, uint32_t *uc_log_tx_en, uint32_t *uc_log_rx_en);

/**
 * Override CMN power state settings during isolation mode.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set pstate value for CMN lane. 
 *              0 - PD state
 *              1 - P0 state
 *
 * Return:
 *
 * Constraints:
 *    Must be run after CMN isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_cmn_pstate_set(mss_access_t *mss, uint32_t value);

/**
 * Return override CMN power state
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Set pstate value for CMN lane. 
 *              0 - PD state
 *              1 - P0 state 
 *
 * Constraints:
 *    Must be run after CMN isolation mode is configured.
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_cmn_pstate_get(mss_access_t *mss, uint32_t *value);


/**
 * Override TX reset settings during isolation mode.  
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set value for Tx lane reset. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured.  
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_reset_set(mss_access_t *mss, uint32_t value);

/**
 * Return TX reset isolation mode setting.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Return value for Tx lane reset.
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured.  
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_reset_get(mss_access_t *mss, uint32_t *value);

/**
 * Override RX reset settings during isolation mode.  
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set value for Rx lane reset. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured.
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_reset_set(mss_access_t *mss, uint32_t value);

/**
 * Return RX reset isolation mode setting.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Return value for Rx lane reset. 
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured.
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_reset_get(mss_access_t *mss, uint32_t *value);

/**
 * Override TX rate settings during isolation mode. 
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set value for TX lane rate. Refer to databook for mapping. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured.
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_rate_set(mss_access_t *mss, uint32_t value); 

/**
 * Return TX rate isolation mode setting.  
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Return value for TX lane rate. Refer to databook for mapping. 
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured.
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_rate_get(mss_access_t *mss, uint32_t *value);

/**
 * Override RX rate settings during isolation mode. 
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set value for RX lane rate. Refer to databook for mapping. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured.
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_rate_set(mss_access_t *mss, uint32_t value);

/**
 * Return RX rate isolation mode setting. 
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Return value for RX lane rate. Refer to databook for mapping. 
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured.
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_rate_get(mss_access_t *mss, uint32_t *value);

/**
 * Override TX power state settings during isolation mode.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set pstate value for Tx lane.
 *          000 - P0 (Fully powered up) 
 *          001 - P0s 
 *          010 - P1 
 *          011 - P2 
 *          100 - PD (full power down) 
 *          101 - PCIE L1.0 Substate 
 *          110 - PCIE L1.1 Substate 
 *          111 - PCIE L1.2 Substate  
 *
 * Return:
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_pstate_set(mss_access_t *mss, uint32_t value);

/**
 *  Return TX power state isolation mode setting.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Return pstate value for Tx lane.
 *          000 - P0 (Fully powered up) 
 *          001 - P0s 
 *          010 - P1 
 *          011 - P2 
 *          100 - PD (full power down) 
 *          101 - PCIE L1.0 Substate 
 *          110 - PCIE L1.1 Substate 
 *          111 - PCIE L1.2 Substate  
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_pstate_get(mss_access_t *mss, uint32_t *value);

/**
 * Override RX power state settings during isolation mode.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set pstate value for Rx lane. 
 *          000 - P0 (Fully powered up) 
 *          001 - P0s 
 *          010 - P1 
 *          011 - P2 
 *          100 - PD (full power down) 
 *          101 - PCIE L1.0 Substate 
 *          110 - PCIE L1.1 Substate 
 *          111 - PCIE L1.2 Substate  
 *
 * Return:
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_pstate_set(mss_access_t *mss, uint32_t value);

/**
 *  Return RX power state isolation mode setting.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Return pstate value for Rx lane.
 *          000 - P0 (Fully powered up) 
 *          001 - P0s 
 *          010 - P1 
 *          011 - P2 
 *          100 - PD (full power down) 
 *          101 - PCIE L1.0 Substate 
 *          110 - PCIE L1.1 Substate 
 *          111 - PCIE L1.2 Substate  
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_pstate_get(mss_access_t *mss, uint32_t *value);

/**
 * Override TX width settings during isolation mode.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set value for Tx lane width. 
 *          1000 -> 1111 - unused 
 *          0111 - 128 width 
 *          0110 - 64 width 
 *          0101 - 40 width 
 *          0100 - 32 width 
 *          0011 - 20 width 
 *          0010 - 16 width 
 *          0000 -> 0001 reserved
 *
 * Return:
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_width_set(mss_access_t *mss, uint32_t value);

/**
 * Return TX width isolation mode setting.  
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Set value for Tx lane width. 
 *          1000 -> 1111 - unused 
 *          0111 - 128 width 
 *          0110 - 64 width 
 *          0101 - 40 width 
 *          0100 - 32 width 
 *          0011 - 20 width 
 *          0010 - 16 width 
 *          0000 -> 0001 reserved
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_width_get(mss_access_t *mss, uint32_t *value);

/**
 * Override RX width settings during isolation mode.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set value for Rx lane width. 
 *          1000 -> 1111 - unused 
 *          0111 - 128 width 
 *          0110 - 64 width 
 *          0101 - 40 width 
 *          0100 - 32 width 
 *          0011 - 20 width 
 *          0010 - 16 width 
 *          0000 -> 0001 reserved 
 *
 * Return:
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_width_set(mss_access_t *mss, uint32_t value);

/**
 * Return RX width isolation mode setting.  
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Set value for Rx lane width. 
 *          1000 -> 1111 - unused 
 *          0111 - 128 width 
 *          0110 - 64 width 
 *          0101 - 40 width 
 *          0100 - 32 width 
 *          0011 - 20 width 
 *          0010 - 16 width 
 *          0000 -> 0001 reserved
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_width_get(mss_access_t *mss, uint32_t *value);

/**
 * Override TX training mode settings during isolation mode.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set value for Tx lane training mode. 
 *          101 -> 111 - Reserved
 *          100        - CL162
 *          011        - CL136
 *          010        - CL92
 *          001        - CL72
 *          000        - Training Disabled
 *
 * Return:
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_training_mode_set(mss_access_t *mss, uint32_t value);

/**
 * Return TX training mode isolation mode setting.  
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Set value for Tx lane training mode. 
 *          101 -> 111 - Reserved
 *          100        - CL162
 *          011        - CL136
 *          010        - CL92
 *          001        - CL72
 *          000        - Training Disabled
 *
 * Constraints:
 *    Must be run after TX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_tx_training_mode_get(mss_access_t *mss, uint32_t *value);

/**
 * Override RX training mode settings during isolation mode.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value': Set value for Rx lane training mode. 
 *          101 -> 111 - Reserved
 *          100        - CL162
 *          011        - CL136
 *          010        - CL92
 *          001        - CL72
 *          000        - Training Disabled
 *
 * Return:
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_training_mode_set(mss_access_t *mss, uint32_t value);

/**
 * Return RX training mode isolation mode setting.  
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value': Set value for Rx lane training mode. 
 *          101 -> 111 - Reserved
 *          100        - CL162
 *          011        - CL136
 *          010        - CL92
 *          001        - CL72
 *          000        - Training Disabled
 *
 * Constraints:
 *    Must be run after RX isolation mode is configured. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_iso_rx_training_mode_get(mss_access_t *mss, uint32_t *value);


/**
 * Override CMN pin interface to enable standalone PHY isolation mode. 
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'en' : 0 - Use CMN pins from pin interface 
 *           1 - Override CMN pin interface
 *
 * Return:
 *
 * Constraints:
 *    Must be run after firmware is loaded, and before asserting TX/RX resets irst_*_ln[#].  
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_isolate_cmn_set(mss_access_t *mss, uint32_t en);

/**
 * Return CMN isolation mode enable status.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'en' : Returns isolation enable mode.
 *
 * Constraints:
 *    Can be used anytime. 
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_isolate_cmn_get(mss_access_t *mss, uint32_t *en);

/**
 * Override lane pin interface to enable standalone PHY isolation mode. 
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'en' : 0 - Use lane pins from pin interface 
 *           1 - Override lane pin interface
 *
 * Return:
 *
 * Constraints:
 *    Must be run after firmware is loaded, and before asserting TX/RX resets irst_*_ln[#].  
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_isolate_lane_set(mss_access_t *mss, uint32_t en);

/**
 * Return lane isolation mode enable status. 
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'en' : Returns isolation enable mode.  
 *
 * Constraints:
 *    Can be used anytime.  
 *
 * Typical Application Usage:
 *    Used to isolate the PHY from the SoC for deep debug or electrical characterization.
 */
int jupiter_pmd_isolate_lane_get(mss_access_t *mss, uint32_t *en);

/**
 * Sets the selection of the right to left high-speed reference clock buffer.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'sel' : 11 - Unused
 *            10 - Transmit local hsref clock on left to right cml. 
 *            01 - Buffer left to right cml clock
 *            00 - Power down left to right hsref buffer. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run at beginning of configuration 
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level
 *     
 */
int jupiter_pmd_cmn_r2l_hsref_sel_set(mss_access_t *mss, uint32_t sel);

/**
 * Sets the selection of the right to left low-speed reference clock buffer number 0.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'sel' : 11 - Transmit reference clock from local bu_ref_p, bu_ref_n bumps. 
 *            10 - Transmit looptiming clock derived from a local rx lane.  
 *            01 - Buffer right to left cml clock
 *            00 - Power down right to left lsref buffer. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run at beginning of configuration.
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level.
 *     
 */
int jupiter_pmd_cmn_r2l0_lsref_sel_set(mss_access_t *mss, uint32_t sel);

/**
 * Sets the selection of the right to left low-speed reference clock buffer number 1.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'sel' : 11 - Transmit reference clock from local bu_ref_p, bu_ref_n bumps. 
 *            10 - Transmit looptiming clock derived from a local rx lane.  
 *            01 - Buffer right to left cml clock
 *            00 - Power down right to left lsref buffer.
 *
 * Return:
 *
 * Constraints:
 *    Must be run at beginning of configuration.
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level.
 *     
 */
int jupiter_pmd_cmn_r2l1_lsref_sel_set(mss_access_t *mss, uint32_t sel);

/**
 * Sets the selection of the left to right high-speed reference clock buffer.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'sel' : 11 - Unused
 *            10 - Transmit local hsref clock on left to right cml. 
 *            01 - Buffer left to right cml clock
 *            00 - Power down left to right hsref buffer. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run at beginning of configuration 
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level
 *     
 */
int jupiter_pmd_cmn_l2r_hsref_sel_set(mss_access_t *mss, uint32_t sel);

/**
 * Sets the selection of the left to right low-speed reference clock buffer number 0.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'sel' : 11 - Transmit reference clock from local bu_ref_p, bu_ref_n bumps. 
 *            10 - Transmit looptiming clock derived from a local rx lane.  
 *            01 - Buffer left to right cml clock
 *            00 - Power down left to right lsref buffer. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run at beginning of configuration.
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level.
 *     
 */
int jupiter_pmd_cmn_l2r0_lsref_sel_set(mss_access_t *mss, uint32_t sel);

/**
 * Sets the selection of the left to right low-speed reference clock buffer number 1.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'sel' : 11 - Transmit reference clock from local bu_ref_p, bu_ref_n bumps. 
 *            10 - Transmit looptiming clock derived from a local rx lane.  
 *            01 - Buffer left to right cml clock
 *            00 - Power down left to right lsref buffer. 
 *
 * Return:
 *
 * Constraints:
 *    Must be run at beginning of configuration.
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level.
 *     
 */
int jupiter_pmd_cmn_l2r1_lsref_sel_set(mss_access_t *mss, uint32_t sel);

/**
 * Returns the selection of the right to left high-speed reference clock buffer.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'sel' : 11 - Unused
 *            10 - Transmit local hsref clock on left to right cml. 
 *            01 - Buffer right to left cml clock
 *            00 - Power down right to left hsref buffer. 
 *
 * Constraints:
 *    Must be run at beginning of configuration 
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level
 *     
 */
int jupiter_pmd_cmn_r2l_hsref_sel_get(mss_access_t *mss, uint32_t *sel);

/**
 * Return the selection of the right to left low-speed reference clock buffer number 0.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'sel' : 11 - Transmit reference clock from local bu_ref_p, bu_ref_n bumps. 
 *            10 - Transmit looptiming clock derived from a local rx lane.  
 *            01 - Buffer right to left cml clock
 *            00 - Power down right to left lsref buffer. 
 *
 * Constraints:
 *    Must be run at beginning of configuration.
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level.
 *     
 */
int jupiter_pmd_cmn_r2l0_lsref_sel_get(mss_access_t *mss, uint32_t *sel);

/**
 * Return the selection of the right to left low-speed reference clock buffer number 1.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'sel' : 11 - Transmit reference clock from local bu_ref_p, bu_ref_n bumps. 
 *            10 - Transmit looptiming clock derived from a local rx lane.  
 *            01 - Buffer right to left cml clock
 *            00 - Power down right to left lsref buffer. 
 *
 * Constraints:
 *    Must be run at beginning of configuration.
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level.
 *     
 */
int jupiter_pmd_cmn_r2l1_lsref_sel_get(mss_access_t *mss, uint32_t *sel);

/**
 * Returns the selection of the left to right high-speed reference clock buffer.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'sel' : 11 - Unused
 *            10 - Transmit local hsref clock on left to right cml. 
 *            01 - Buffer left to right cml clock
 *            00 - Power down left to right hsref buffer. 
 *
 * Constraints:
 *    Must be run at beginning of configuration 
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level
 *     
 */
int jupiter_pmd_cmn_l2r_hsref_sel_get(mss_access_t *mss, uint32_t *sel);

/**
 * Return the selection of the left to right low-speed reference clock buffer number 0.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'sel' : 11 - Transmit reference clock from local bu_ref_p, bu_ref_n bumps. 
 *            10 - Transmit looptiming clock derived from a local rx lane.  
 *            01 - Buffer left to right cml clock
 *            00 - Power down left to right lsref buffer. 
 *
 * Constraints:
 *    Must be run at beginning of configuration.
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level.
 *     
 */
int jupiter_pmd_cmn_l2r0_lsref_sel_get(mss_access_t *mss, uint32_t *sel);

/**
 * Return the selection of the left to right low-speed reference clock buffer number 1.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'sel' : 11 - Transmit reference clock from local bu_ref_p, bu_ref_n bumps. 
 *            10 - Transmit looptiming clock derived from a local rx lane.  
 *            01 - Buffer left to right cml clock
 *            00 - Power down left to right lsref buffer. 
 *
 * Constraints:
 *    Must be run at beginning of configuration.
 *
 * Typical Application Usage:
 *    Use to route reference clock to SOC level.
 *     
 */
int jupiter_pmd_cmn_l2r1_lsref_sel_get(mss_access_t *mss, uint32_t *sel);

/**
 * Override the lsref reference clock source select
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'ref_sel' : Selection of which reference clock is used for the local LC synthesizer
 *            111/110 - Unused
 *            110 - Use lsref_soc for the LC synth
 *            101 - Use rx_ref for the LC synth
 *            100 - Use local ref for the LC synth
 *            011 - Use r2l0 reference clock for LC synth
 *            010 - use r2l1 reference clock for the LC synth
 *            001 - use l2r0 reference clock for the LC synth
 *            000 - use l2r1 reference clock for the LC synth 
 *
 * Return:
 *
 * Constraints:
 *    Must be run after before asserting TX/RX resets irst_*_ln[#].
 *
 * Typical Application Usage:
 *    Use to select the lsref reference clock source during configuration.
 */
int jupiter_pmd_cmn_lsref_sel_set(mss_access_t *mss, uint32_t ref_sel);

/**
 * Return the lsref reference clock source select
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'ref_sel' : Selection of which reference clock is used for the local LC synthesizer
 *            111/110 - Unused
 *            110 - Use lsref_soc for the LC synth
 *            101 - Use rx_ref for the LC synth
 *            100 - Use local ref for the LC synth
 *            011 - Use r2l0 reference clock for LC synth
 *            010 - use r2l1 reference clock for the LC synth
 *            001 - use l2r0 reference clock for the LC synth
 *            000 - use l2r1 reference clock for the LC synth 
 *
 * Constraints:
 *    Can be run at anytime.
 *
 * Typical Application Usage:
 *    Use to return the lsref reference clock source select
 */
int jupiter_pmd_cmn_lsref_sel_get(mss_access_t *mss, uint32_t *ref_sel);

/**
 * Enable/disable TX BIST. 
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value' : 0 - disable TX BIST 
 *              1 - enable TX BIST 
 *
 * Return:
 *
 * Constraints:
 *    Power State:  During P0.
 *    Rate State:   After desired rate change.
 *    EqEval State: Don't care.
 *    Effect:       Takes effect immediately. 
 *
 * Typical Application Usage:
 *    Configure TX and RX BIST to PRBS31 pattern, enable external loopback, and enable TX and RX BIST. RX BIST should lock to the pattern generated by TX BIST with error = 0. 
 */
int jupiter_pmd_gen_tx_en_set(mss_access_t *mss, uint32_t value);

/**
 * Return TX BIST enable/disable status.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'value' : 0 - TX BIST disabled
 *              1 - TX BIST enabled
 *
 * Constraints:
 *    Can be run anytime.
 *
 * Typical Application Usage:
 *    Used to return TX BIST enable/disable status.
 **/
int jupiter_pmd_gen_tx_en_get(mss_access_t *mss, uint32_t *value);

/**
 * Read error count done bit. Indicating rxbist burst capture is complete
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return:
 *    'err_count_done' : 1 indicates rxbist burst capture is complete
 *
 * Constraints:
 *    Must to run while rxbist is enabled
 *
 * Typical Application Usage:
 *    Used for debug.
 *
 */
int jupiter_pmd_rx_error_cnt_done_get(mss_access_t *mss, uint32_t *err_count_done);

/**
 * Initiate cmn power state change.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'cmn_pstate' : Desired pstate
 *    'timeout_us' : Timeout limit in us
 *
 * Return:
 *
 * Constraints:
 *    CMN must be in isolation mode.
 *
 * Typical Application Usage:
 *    Used for debug.
 */
int jupiter_pmd_iso_request_cmn_state_change(mss_access_t *mss, jupiter_cmn_pstate_t cmn_pstate, uint32_t timeout_us);

/**
 * Initiate TX state change.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'tx_pstate' : Desired TX pstate; See datasheet for mapping 
 *    'tx_rate'   : Desired TX rate; See datasheet for mapping 
 *    'tx_width'  : Desired TX width; See datasheet for mapping
 *    'timeout_us' : Timeout limit in us
 *
 * Return:
 *    Return value will non-zero if timeout
 *
 * Constraints:
 *    TX must be in isolation mode.
 *
 * Typical Application Usage:
 *    Used for debug.
 */
int jupiter_pmd_iso_request_tx_state_change(mss_access_t *mss, jupiter_pstate_t tx_pstate, uint32_t tx_rate, uint32_t tx_width, uint32_t timeout_us);

/**
 * Initiate RX state change.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'rx_pstate' : Desired RX pstate; See datasheet for mapping 
 *    'rx_rate'   : Desired RX rate; See datasheet for mapping 
 *    'rx_width'  : Desired RX width; See datasheet for mapping 
 *    'timeout_us' : Timeout limit in us
 *
 * Return:
 *    Return value will non-zero if timeout
 *
 * Constraints:
 *    RX must be in isolation mode.
 *
 * Typical Application Usage:
 *    Used for debug.
 */
int jupiter_pmd_iso_request_rx_state_change(mss_access_t *mss, jupiter_pstate_t rx_pstate, uint32_t rx_rate, uint32_t rx_width, uint32_t timeout_us);

/**
 * Poll for RX CDR Lock.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'timeout_us' : Timeout limit in us
 *
 * Return:
 *    Return value will non-zero if timeout
 *
 * Constraints:
 *    Must be run after RX is powered up to P0/P1.
 *
 * Typical Application Usage:
 *    Used for debug.
 */
int jupiter_pmd_rx_check_cdr_lock(mss_access_t *mss, uint32_t timeout_us);

/**
 * Wrapper function for jupiter_pmd_rx_chk_err_count_state_get with timeout
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'rx_width' : Data width used in rx data bist (See datasheet for encoding)
 *    'timer_threshold' : number of cycles for timer mode 
 *    'timeout_us' : Timeout limit in us
 *    'expected_errors' : Expected amount of rxbist errors. If -1, perform read instead of check.
 *
 * Return:
 *    Return value will non-zero if timeout
 *
 * Constraints:
 *    Must be run after RX BIST is enabled.
 *
 * Typical Application Usage:
 *    Meant to be run to read rxbist error count and error count overflow bit.
 */
int jupiter_pmd_rx_check_bist (mss_access_t *mss, jupiter_bist_mode_t bist_mode, uint32_t timer_threshold, uint32_t rx_width, uint32_t timeout_us, int32_t expected_errors);

/**
 * Run eqeval with desired type during isolation mode. Returns incdec coefficients.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'eq_type' : Sets eqeval type.
 *            0 - Full EQ, Directional 
 *            1 - Eval Only, Directional 
 *            2 - Init Eval 
 *            3 - Clear Eval 
 *            4 - Full EQ, FOM 
 *            5 - Eval Only, FOM 
 *            6+ Reserved
 *
 * Return:
 *
 * Constraints:
 *    Must be called when RX is in P0 power state.  
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_eqeval_type_set(mss_access_t *mss, uint32_t eq_type);

/**
 * Sets request for initiating EqEval in isolation mode.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'req' : Sets eqeval request.
 *
 * Return:
 *
 * Constraints:
 *    Must be called when RX is in P0 power state.  
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_eqeval_req_set(mss_access_t *mss, uint32_t value);

/**
 * Read acknowledge for EqEval in isolation mode. 
 * Typically this function needs to be called repeatedly until ack returns a value of 1, indicating EqEval is done.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    
 *
 * Return:
 *    'eqeval_ack' : Return acknowledge value
 *
 * Constraints:
 *    Must be called after ictl_rx_linkeval_req_ln[#] has been raised. 
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_eqeval_ack_get(mss_access_t *mss, uint32_t *eqeval_ack );

/**
 * Returns eqeval incdec coefficient update in isolation mode. 
 * Incdec tells the remote transmitter to increase or decrease adaptation values. 
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    
 *
 * Return:
 *    'incdec' : [7:6] C+1, [5:4] C0, [3:2] C-1, [1:0] C-2. 00 = hold 01 = increment 10 = decrement
 *
 * Constraints:
 *    Must be called after octl_rx_linkeval_ack_ln[#] has returned value of 1.  
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_eqeval_incdec_get(mss_access_t *mss, uint32_t * incdec);

/**
 * Requests rx_linkeval and reads back incdec coefficient.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'eq_type' : Sets eqeval type. 
 *            0 - Full EQ, Directional 
 *            1 - Eval Only, Directional 
 *            2 - Init Eval 
 *            3 - Clear Eval 
 *            4 - Full EQ, FOM 
 *            5 - Eval Only, FOM 
 *            6+ Reserved
 *    'timeout_us' : Timeout limit in us
 *    
 *
 * Return:
 *    Return value will non-zero if timeout 
 *
 * Constraints:
 *    Must be called when RX is in P0 power state. 
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_rx_equalize(mss_access_t *mss, uint32_t eq_type, uint32_t timeout_us );

/**
 * Sets receiver detect request
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value' : tx_rxdet req
 *
 * Return: 
 *
 * Constraints:
 *    Power State:  During P0/P1.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_tx_rxdet_req_set(mss_access_t *mss, uint32_t value);

/**
 * TX detect RX test.
 * 1) Set tx_rxdet req=1
 * 2) Poll for ack
 * 3) Check for expected tx_rxdet_result
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'rxdet_expected' : Expected tx_rxdet_result
 *                       0 - Receiver not detected
 *                       1 - Receiver detected
 *    'timeout_us' : Timeout limit in us
 *
 * Return: 
 *    Return value will non-zero if timeout or if check fails.
 *
 * Constraints:
 *    Power State:  During P0/P1.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_tx_rxdet(mss_access_t *mss, uint32_t rxdet_expected, uint32_t timeout_us);

/**
 * Enable TX Beacon signal transmission
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'value' : enable for tx beacon
 *
 * Return: 
 *
 * Constraints:
 *    Power State:  During P2.
 *    Rate State:   After desired rate change.
 *    EqEval State: Before eqeval.
 *    Effect:       Takes effect immediately.
 *
 * Typical Application Usage:
 *    Used to verify TX Beacon requirements for PCIE spec. If external loopback is enabled, can be used to test rx signal detect.
 **/
int jupiter_pmd_tx_beacon_en_set(mss_access_t *mss, uint32_t value);

/**
 * Return TX Beacon enable status.
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return: 
 *    'value' : Return value for tx beacon enable status
 *
 * Constraints:
 *    Can be run anytime.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_tx_beacon_en_get(mss_access_t *mss, uint32_t *value);

/**
 * Capture TX PLL osc fine code
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'center_code' : expected osc code
 *    'tolerance'   : Tolerence for center_code check. If -1, read instead of check.
 *
 * Return: 
 *    'tx_pll_fine_code' : TX PLL osc fine code
 *
 * Constraints:
 *    TX PLL must be running.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_tx_pll_fine_code_get(mss_access_t *mss, uint32_t * tx_pll_fine_code, uint32_t center_code, int tolerance);

/**
 * Capture TX PLL osc coarse code
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return: 
 *    'tx_pll_coarse_code' : TX PLL osc coarse code
 *
 * Constraints:
 *    TX PLL must be running.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_tx_pll_coarse_code_get(mss_access_t *mss, uint32_t * tx_pll_coarse_code); 

/**
 * Read RX PLL osc fine code
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'center_code' : expected osc code
 *    'tolerance'   : Tolerence for center_code check. If -1, read instead of check.
 *
 * Return: 
 *    'rx_pll_fine_code' : RX PLL osc fine code
 *
 * Constraints:
 *    RX PLL must be running.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_rx_pll_fine_code_get(mss_access_t *mss, uint32_t * rx_pll_fine_code, uint32_t center_code, int tolerance);


/**
 * Capture RX PLL osc coarse code
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return: 
 *    'rx_pll_coarse_code' : RX PLL osc coarse code
 *
 * Constraints:
 *    RX PLL must be running.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_rx_pll_coarse_code_get(mss_access_t *mss, uint32_t * rx_pll_coarse_code);

/**
 * Capture LC PLL osc fine code
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'center_code' : expected osc code
 *    'tolerance'   : Tolerence for center_code check. If -1, read instead of check.
 *
 * Return: 
 *    'cmn_pll_fine_code' : LC PLL osc fine code
 *
 * Constraints:
 *    LC PLL must be running.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_cmn_pll_fine_code_get(mss_access_t *mss, uint32_t * cmn_pll_fine_code, uint32_t center_code, int tolerance);

/**
 * Capture LC PLL osc coarse code
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *
 * Return: 
 *    'cmn_pll_coarse_code' : LC PLL osc coarse code
 *
 * Constraints:
 *    LC PLL must be running.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_cmn_pll_coarse_code_get(mss_access_t *mss, uint32_t * cmn_pll_coarse_code);

/**
 * Returns process monitor results based on frequency measurements of ring oscillators
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'pmon_sel' : select for target pmon ring osc cell . Please see datasheet for mapping
 *    'pvt_measure_timing_window' : timing window for frequency measurement (in power of 2 refclk cycles)
 *    'timeout_us' : Timeout limit in us
 *
 *
 * Return: 
 *    'pvt_measure_result' : Return value from pmon measurement
 *
 * Constraints:
 *    CMN must be in P0.
 *
 * Typical Application Usage:
 *    Used for debug.
 **/
int jupiter_pmd_measure_pmon(mss_access_t *mss, uint32_t pmon_sel, uint32_t pvt_measure_timing_window, uint32_t timeout_us, uint32_t * pvt_measure_result);


int jupiter_pmd_atest_en(mss_access_t *mss, uint32_t en);


int jupiter_pmd_atest_cmn_capture(mss_access_t *mss, uint32_t atest_addr, uint32_t * atest_adc_val);

/**
 * Wrapper function to call the following status APIs:
 * jupiter_pmd_rx_dsp_get
 * jupiter_pmd_rx_afe_get
 * jupiter_pmd_rx_dcdiq_get
 * jupiter_pmd_tx_dcdiq_get
 * jupiter_pmd_tx_pll_fine_code_get
 * jupiter_pmd_tx_pll_coarse_code_get
 * jupiter_pmd_rx_pll_fine_code_get
 * jupiter_pmd_rx_pll_coarse_code_get
 * jupiter_pmd_cmn_pll_fine_code_get
 * jupiter_pmd_cmn_pll_coarse_code_get
 * 
 * Args:
 *    'mss': Object containing driver methods.
 *    'branch' : Select for which branch to read FFE coefficients. If -1, read all branches
 *
 * Return: 
 *
 * Constraints:
 *    Can be read at any time.
 *
 * Typical Application Usage:
 *    Meant to be run after loopback test for debug.
 **/
int jupiter_pmd_read_status(mss_access_t *mss, int branch);

#endif // JUPITER_CORE_H
