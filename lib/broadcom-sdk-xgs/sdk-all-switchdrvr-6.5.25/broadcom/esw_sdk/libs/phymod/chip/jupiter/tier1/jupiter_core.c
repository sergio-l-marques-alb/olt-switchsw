/**
 * Jupiter JupiterCORE100 C API functions
 * 
 * Version: 0.9.0
 */

#ifdef NON_SDK
#include <stdint.h>
#include <math.h>
#include <string.h>
#else
#include <phymod/phymod_system.h>
#include <phymod/phymod.h>
#include "jupiter_dependencies.h"
#endif

#include "jupiter_core.h"

const char jupiter_library_version[] = "0.9.0";

uint32_t jupiter_width_decoder (uint32_t width_encoded) {
    uint32_t width = 128;
    if (width_encoded == 7){
        width = 128 ;
    } else if (width_encoded == 6){
        width = 64;
    } else if (width_encoded == 5){
        width = 40;
    } else if (width_encoded == 4){
        width = 32;
    } else if (width_encoded == 3){
        width = 20;
    } else if (width_encoded == 2){
        width = 16;
    } else if (width_encoded == 1){
        width = 10;
    } else if (width_encoded == 0){
        width = 64;
    } else {
        USR_PRINTF(("ERROR: Invalid width encoding\n"));
        return 0; 
    }
    return width;
}


int jupiter_pmd_link_training_config_set(mss_access_t *mss, jupiter_lt_cfg_t lt_cfg) {
    CHECK(pmd_write_field(mss, ETH_LT_PRBS_ADDR, ETH_LT_PRBS_LT_POLYNOMIAL_SEL_C92_MASK, ETH_LT_PRBS_LT_POLYNOMIAL_SEL_C92_OFFSET, lt_cfg.lt_polynomial_sel_c92));
    CHECK(pmd_write_field(mss, ETH_LT_PRBS_ADDR, ETH_LT_PRBS_LT_POLYNOMIAL_SEL_C136_MASK, ETH_LT_PRBS_LT_POLYNOMIAL_SEL_C136_OFFSET, lt_cfg.lt_polynomial_sel_c136));
    CHECK(pmd_write_field(mss, ETH_LT_PRBS_ADDR, ETH_LT_PRBS_LT_SEED_11B_MASK, ETH_LT_PRBS_LT_SEED_11B_OFFSET, lt_cfg.lt_seed_11b));
    CHECK(pmd_write_field(mss, ETH_LT_PRBS_ADDR, ETH_LT_PRBS_LT_SEED_13B_MASK, ETH_LT_PRBS_LT_SEED_13B_OFFSET, lt_cfg.lt_seed_13b));
    CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_ANLT_MS_PER_CK_MASK, ETH_ANLT_CTRL_ANLT_MS_PER_CK_OFFSET, lt_cfg.lt_ms_per_ck));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_link_training_config_get(mss_access_t *mss, jupiter_lt_cfg_t* lt_cfg){
    CHECK(pmd_read_field(mss, ETH_LT_PRBS_ADDR, ETH_LT_PRBS_LT_POLYNOMIAL_SEL_C92_MASK, ETH_LT_PRBS_LT_POLYNOMIAL_SEL_C92_OFFSET, &(lt_cfg->lt_polynomial_sel_c92) ));
    CHECK(pmd_read_field(mss, ETH_LT_PRBS_ADDR, ETH_LT_PRBS_LT_POLYNOMIAL_SEL_C136_MASK, ETH_LT_PRBS_LT_POLYNOMIAL_SEL_C136_OFFSET, &(lt_cfg->lt_polynomial_sel_c136) ));
    CHECK(pmd_read_field(mss, ETH_LT_PRBS_ADDR, ETH_LT_PRBS_LT_SEED_11B_MASK, ETH_LT_PRBS_LT_SEED_11B_OFFSET, &(lt_cfg->lt_seed_11b) ));
    CHECK(pmd_read_field(mss, ETH_LT_PRBS_ADDR, ETH_LT_PRBS_LT_SEED_13B_MASK, ETH_LT_PRBS_LT_SEED_13B_OFFSET, &(lt_cfg->lt_seed_13b) ));
    CHECK(pmd_read_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_ANLT_MS_PER_CK_MASK, ETH_ANLT_CTRL_ANLT_MS_PER_CK_OFFSET, &(lt_cfg->lt_ms_per_ck) ));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_link_training_status_get(mss_access_t *mss, jupiter_lt_status_t* lt_status) {
    CHECK(pmd_read_field(mss, ETH_LT_STAT_ADDR, ETH_LT_STAT_LT_RUNNING_MASK, ETH_LT_STAT_LT_RUNNING_OFFSET, &(lt_status->lt_running) ));
    CHECK(pmd_read_field(mss, ETH_ANLT_STATUS_ADDR, ETH_ANLT_STATUS_LT_DONE_MASK, ETH_ANLT_STATUS_LT_DONE_OFFSET, &(lt_status->lt_done)));
    CHECK(pmd_read_field(mss, ETH_LT_STAT_ADDR, ETH_LT_STAT_LT_TRAINING_FAILURE_MASK, ETH_LT_STAT_LT_TRAINING_FAILURE_OFFSET, &(lt_status->lt_failure) ));
    CHECK(pmd_read_field(mss, ETH_LT_STAT_ADDR, ETH_LT_STAT_LT_RX_READY_MASK, ETH_LT_STAT_LT_RX_READY_OFFSET, &(lt_status->lt_rx_ready) ));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_refclk_termination_set(mss_access_t *mss, jupiter_refclk_term_mode_t lsrefbuf_term_mode){
    CHECK(pmd_write_field(mss, CMN_REFCLK_ADDR, CMN_REFCLK_LSREFBUF_TERM_MODE_NT_MASK, CMN_REFCLK_LSREFBUF_TERM_MODE_NT_OFFSET, lsrefbuf_term_mode ));
    return JUPITER_ERR_CODE_NONE;
}

/* NOTE: getter added */
int jupiter_pmd_refclk_termination_get(mss_access_t *mss, jupiter_refclk_term_mode_t *lsrefbuf_term_mode){
    uint32_t rdval;
    CHECK(pmd_read_field(mss, CMN_REFCLK_ADDR, CMN_REFCLK_LSREFBUF_TERM_MODE_NT_MASK, CMN_REFCLK_LSREFBUF_TERM_MODE_NT_OFFSET, &rdval));
    *lsrefbuf_term_mode = (jupiter_refclk_term_mode_t) rdval;
    return JUPITER_ERR_CODE_NONE;
} 

int jupiter_pmd_rx_termination_set(mss_access_t *mss, jupiter_acc_term_mode_t acc_term_mode){
    CHECK(pmd_write_field(mss, AFE_OCTERM_RX_ADDR, AFE_OCTERM_RX_ACC_TERM_MODE_NT_MASK, AFE_OCTERM_RX_ACC_TERM_MODE_NT_OFFSET, acc_term_mode ));
    return JUPITER_ERR_CODE_NONE;
}

/* NOTE: getter added */
int jupiter_pmd_rx_termination_get(mss_access_t *mss, jupiter_acc_term_mode_t *acc_term_mode){
    uint32_t rdval;
    CHECK(pmd_read_field(mss, AFE_OCTERM_RX_ADDR, AFE_OCTERM_RX_ACC_TERM_MODE_NT_MASK, AFE_OCTERM_RX_ACC_TERM_MODE_NT_OFFSET, &rdval ));
    *acc_term_mode = (jupiter_acc_term_mode_t) rdval;
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_force_signal_detect_config_set(mss_access_t *mss, jupiter_force_sigdet_mode_t sigdet_mode){
    switch (sigdet_mode) {
        case JUPITER_SIGDET_FORCE0:
            CHECK(pmd_write_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_FORCE_VALID_A_MASK, RX_SIGNAL_DETECT_REG3_FORCE_VALID_A_OFFSET, 0));
            CHECK(pmd_write_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_FORCE_INVALID_A_MASK, RX_SIGNAL_DETECT_REG3_FORCE_INVALID_A_OFFSET, 1));
            return JUPITER_ERR_CODE_NONE;
        case JUPITER_SIGDET_FORCE1:
            CHECK(pmd_write_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_FORCE_VALID_A_MASK, RX_SIGNAL_DETECT_REG3_FORCE_VALID_A_OFFSET, 1));
            CHECK(pmd_write_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_FORCE_INVALID_A_MASK, RX_SIGNAL_DETECT_REG3_FORCE_INVALID_A_OFFSET, 0));
            return JUPITER_ERR_CODE_NONE;
        case JUPITER_SIGDET_NORM:
            CHECK(pmd_write_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_FORCE_VALID_A_MASK, RX_SIGNAL_DETECT_REG3_FORCE_VALID_A_OFFSET, 0));
            CHECK(pmd_write_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_FORCE_INVALID_A_MASK, RX_SIGNAL_DETECT_REG3_FORCE_INVALID_A_OFFSET, 0));
            return JUPITER_ERR_CODE_NONE;
        default:
            return JUPITER_ERR_CODE_INVALID_ARG_VALUE;
    }
}

int jupiter_pmd_force_signal_detect_config_get(mss_access_t *mss, jupiter_force_sigdet_mode_t *sigdet_mode){
    uint32_t valid;
    uint32_t invalid;
    CHECK(pmd_read_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_FORCE_VALID_A_MASK, RX_SIGNAL_DETECT_REG3_FORCE_VALID_A_OFFSET, &valid));
    CHECK(pmd_read_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_FORCE_INVALID_A_MASK, RX_SIGNAL_DETECT_REG3_FORCE_INVALID_A_OFFSET, &invalid));
    if (valid == 0 && invalid == 1) {
        *sigdet_mode = JUPITER_SIGDET_FORCE0;
        return JUPITER_ERR_CODE_NONE;
    } else if (valid == 1 && invalid == 0) {
        *sigdet_mode = JUPITER_SIGDET_FORCE1;
        return JUPITER_ERR_CODE_NONE;
    } else if (valid == 0 && invalid == 0) {
        *sigdet_mode = JUPITER_SIGDET_NORM;
        return JUPITER_ERR_CODE_NONE;
    } else {
        return JUPITER_ERR_CODE_BAD_STATE;
    }
}

int jupiter_pmd_tx_disable_set(mss_access_t *mss, uint32_t tx_disable){
    /* 1 to set tx_disable, 0 to clear tx_disable */
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_ELECIDLE_A_MASK, TX_OVRD_REG1_ICTL_TX_ELECIDLE_A_OFFSET, tx_disable));
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_TXCHNG_PEDGE_A_MASK, TX_OVRD_REG1_TXCHNG_PEDGE_A_OFFSET, 1));
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_TXCHNG_PEDGE_A_MASK, TX_OVRD_REG1_TXCHNG_PEDGE_A_OFFSET, 0));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_disable_get(mss_access_t *mss, uint32_t *tx_disable_override, uint32_t *tx_disable){
    /* 1 = tx_disable, 0 = normal operation */
    CHECK(pmd_read_field(mss, TX_OVRD_ENABLE_ADDR, TX_OVRD_REG1_ICTL_TX_ELECIDLE_A_MASK, TX_OVRD_REG1_ICTL_TX_ELECIDLE_A_OFFSET, tx_disable_override));
    CHECK(pmd_read_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_ELECIDLE_A_MASK, TX_OVRD_REG1_ICTL_TX_ELECIDLE_A_OFFSET, tx_disable));
    return JUPITER_ERR_CODE_NONE;
}

/* NOTE: these are commented out, and not included in the API spec, but Jupiter is
 *       reserving in case there is a future usecase
 * int jupiter_pmd_rx_disable_pin_override_set(mss_access_t *mss, int override_enable){}
 * int jupiter_pmd_rx_disable_pin_override_get(mss_access_t *mss, int *override_enable){}
 * int jupiter_pmd_rx_disable_set (mss_access_t *mss, int rx_disable){}
 * int jupiter_pmd_rx_disable_get(mss_access_t *mss, int *rx_disable){}
 * ENDNOTE
 */

int jupiter_pmd_txfir_config_reset_set(mss_access_t *mss, jupiter_txfir_config_t txfir_cfg){
    /*uint32_t cm3_mask = 0x7;  //3bits */
    uint32_t cm2_mask = 0x7;  /*3bits */
    uint32_t cm1_mask = 0x3F; /*6bits */
    uint32_t c0_mask = 0x3F; /*6bits */
    uint32_t c1_mask = 0x3F;  /*6bits */

    uint32_t c1_c0_cm1_cm2 = ((txfir_cfg.C1 & c1_mask) << 15);
    c1_c0_cm1_cm2 = c1_c0_cm1_cm2 + ((txfir_cfg.C0 & c0_mask) << 9);
    c1_c0_cm1_cm2 = c1_c0_cm1_cm2 + ((txfir_cfg.CM1 & cm1_mask) << 3);
    c1_c0_cm1_cm2 = c1_c0_cm1_cm2 + ((txfir_cfg.CM2 & cm2_mask));

    CHECK(pmd_write_field(mss, TX_FIR_REG14_ADDR, TX_FIR_REG14_RESET_VAL_NT_MASK, TX_FIR_REG14_RESET_VAL_NT_OFFSET, c1_c0_cm1_cm2));
    CHECK(pmd_write_field(mss, TX_FIR_REG14_ADDR, TX_FIR_REG14_M3_RESET_VAL_NT_MASK, TX_FIR_REG14_M3_RESET_VAL_NT_OFFSET, txfir_cfg.CM3));

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_txfir_config_set(mss_access_t *mss, jupiter_txfir_config_t txfir_cfg){
    /*uint32_t cm3_mask = 0x7;  //3bits */
    int poll_result;
    uint32_t cm2_mask = 0x7;  /*3bits */
    uint32_t cm1_mask = 0x3F; /*6bits */
    uint32_t c0_mask = 0x3F; /*6bits */
    uint32_t c1_mask = 0x3F;  /*6bits */

    uint32_t c1_c0_cm1_cm2 = ((txfir_cfg.C1 & c1_mask) << 15);
    c1_c0_cm1_cm2 = c1_c0_cm1_cm2 + ((txfir_cfg.C0 & c0_mask) << 9);
    c1_c0_cm1_cm2 = c1_c0_cm1_cm2 + ((txfir_cfg.CM1 & cm1_mask) << 3);
    c1_c0_cm1_cm2 = c1_c0_cm1_cm2 + ((txfir_cfg.CM2 & cm2_mask));

    CHECK(pmd_write_field(mss, TX_OVRD_ENABLE_ADDR, TX_OVRD_ENABLE_FIR_A_MASK, TX_OVRD_ENABLE_FIR_A_OFFSET, 1));
    CHECK(pmd_write_field(mss, TX_OVRD_REG2_ADDR, TX_OVRD_REG2_ICTL_TX_FIR_A_MASK, TX_OVRD_REG2_ICTL_TX_FIR_A_OFFSET, c1_c0_cm1_cm2));
    CHECK(pmd_write_field(mss, TX_OVRD_REG3_ADDR, TX_OVRD_REG3_ICTL_TX_M3FIR_A_MASK, TX_OVRD_REG3_ICTL_TX_M3FIR_A_OFFSET, txfir_cfg.CM3));
    CHECK(pmd_write_field(mss, TX_OVRD_REG2_ADDR, TX_OVRD_REG2_ICTL_TX_FIR_REQ_A_MASK, TX_OVRD_REG2_ICTL_TX_FIR_REQ_A_OFFSET, 1));

    poll_result = pmd_poll_field(mss, TX_STAT_ADDR, TX_STAT_OCTL_TX_FIR_ACK_MASK, TX_STAT_OCTL_TX_FIR_ACK_OFFSET, 1, 5);

    if (poll_result == -1) {
        USR_PRINTF(("ERROR: polling for TXFIR ack\n"));
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        CHECK(pmd_write_field(mss, TX_OVRD_REG2_ADDR, TX_OVRD_REG2_ICTL_TX_FIR_REQ_A_MASK, TX_OVRD_REG2_ICTL_TX_FIR_REQ_A_OFFSET, 0));
        USR_PRINTF(("TXFIR ack received\n"));
    }

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_txfir_config_get(mss_access_t *mss, jupiter_txfir_config_t *txfir_cfg){
    int32_t max_ele;
    int cm3_mask = 0x00000007; /*3bits */
    int cm2_mask = 0x00000038; /*3bits */
    int cm1_mask = 0x000007c0; /*5bits */
    int max_mask = 0x0001f800; /*6bits */
    int c1_mask =  0x003e0000; /*5bits */

    uint32_t rdval;
    CHECK(pmd_read_field(mss, TX_FIR_RDREG_ADDR, TX_FIR_RDREG_AFE_VAL_NT_MASK, TX_FIR_RDREG_AFE_VAL_NT_OFFSET, &rdval));
    txfir_cfg->CM3 = (rdval & cm3_mask);
    txfir_cfg->CM2 = (rdval & cm2_mask) >> 3;
    txfir_cfg->CM1 = (rdval & cm1_mask) >> 6;
    max_ele = (rdval & max_mask) >> 11; /* max_elements */
    txfir_cfg->C1  = (rdval & c1_mask)  >> 17;
    if (txfir_cfg->main_or_max == 0){
        txfir_cfg->C0  = max_ele + 1 - txfir_cfg->C1 - txfir_cfg->CM1 - txfir_cfg->CM2 - txfir_cfg->CM3;
    } else {
        txfir_cfg->C0  = max_ele;
    }
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_tap_mode_get(uint32_t *max_rng_cm3, uint32_t *max_rng_cm2, uint32_t *max_rng_cm1, uint32_t *max_rng_c1, uint32_t *max_rng_c0){
    uint32_t cm3 = 4;
    uint32_t cm2 = 8;
    uint32_t cm1 = 24;
    uint32_t c1  = 24;
    uint32_t c0  = 60;
    *max_rng_cm3 = cm3;
    *max_rng_cm2 = cm2;
    *max_rng_cm1 = cm1;
    *max_rng_c1  = c1;
    *max_rng_c0  = c0;

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_pam4_precoder_override_set(mss_access_t *mss, uint32_t en){
    /* 1 to enable the PAM4 tx precoder, 0 to disable */
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG2_ADDR, TX_DATAPATH_REG2_PAMCODE_OVR_EN_A_MASK, TX_DATAPATH_REG2_PAMCODE_OVR_EN_A_OFFSET, en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_pam4_precoder_override_get(mss_access_t *mss, uint32_t *en){
    /* 1 = PAM4 tx precoder enabled */
    /* 0 = PAM4 tx precoder to disabled */
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG2_ADDR, TX_DATAPATH_REG2_PAMCODE_OVR_EN_A_MASK, TX_DATAPATH_REG2_PAMCODE_OVR_EN_A_OFFSET, en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_pam4_precoder_enable_set(mss_access_t *mss, uint32_t gray_en, uint32_t plusd_en){
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_GRAY_CODE_ENABLE_A_MASK, TX_DATAPATH_REG1_GRAY_CODE_ENABLE_A_OFFSET, gray_en));
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_PLUSD_ENABLE_A_MASK, TX_DATAPATH_REG1_PLUSD_ENABLE_A_OFFSET, plusd_en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_pam4_precoder_enable_get(mss_access_t *mss, uint32_t *gray_en, uint32_t *plusd_en){
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_GRAY_CODE_ENABLE_A_MASK, TX_DATAPATH_REG1_GRAY_CODE_ENABLE_A_OFFSET, gray_en));
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_PLUSD_ENABLE_A_MASK, TX_DATAPATH_REG1_PLUSD_ENABLE_A_OFFSET, plusd_en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_pam4_precoder_override_set(mss_access_t *mss, uint32_t en){
    if (en == 1) {
        CHECK(pmd_write_field(mss, RX_CNTRL_REG2_ADDR, RX_CNTRL_REG2_RX_GRAY_ENA_OVR_NT_MASK, RX_CNTRL_REG2_RX_GRAY_ENA_OVR_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, RX_CNTRL_REG2_ADDR, RX_CNTRL_REG2_RX_GRAY_ENA_OVR_NT_MASK, RX_CNTRL_REG2_RX_GRAY_ENA_OVR_NT_OFFSET, 1));
    }
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_pam4_precoder_override_get(mss_access_t *mss, uint32_t *en){
    uint32_t ret;
    CHECK(pmd_read_field(mss, RX_CNTRL_REG2_ADDR, RX_CNTRL_REG2_RX_GRAY_ENA_OVR_NT_MASK, RX_CNTRL_REG2_RX_GRAY_ENA_OVR_NT_OFFSET, &ret));
    if (ret == 1) {
        *en = 0;
    } else {
        *en = 1;
    }
    return JUPITER_ERR_CODE_NONE;
}


int jupiter_pmd_remote_loopback_set(mss_access_t *mss, uint32_t remote_loopback_enable){
    /* NOTE: this function is only able to loopback the same physical lane to itself */
    /* Enable/disable remote (far-end) loopback (RX -> TX) for the given lane. */
    CHECK(pmd_write_field(mss, TX_LOOPBACK_CNTRL_ADDR, TX_LOOPBACK_CNTRL_ENA_NT_MASK, TX_LOOPBACK_CNTRL_ENA_NT_OFFSET, 1));
    CHECK(pmd_write_field(mss, LOOPBACK_CNTRL_ADDR, LOOPBACK_CNTRL_ENA_NT_MASK, LOOPBACK_CNTRL_ENA_NT_OFFSET, 1));
    CHECK(pmd_write_field(mss, TX_LOOPBACK_CNTRL_ADDR, TX_LOOPBACK_CNTRL_POSTDIV_LOOPBACK_ENA_A_MASK, TX_LOOPBACK_CNTRL_POSTDIV_LOOPBACK_ENA_A_OFFSET, remote_loopback_enable));
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_FEP_LOOPBACK_ENABLE_A_MASK, TX_DATAPATH_REG1_FEP_LOOPBACK_ENABLE_A_OFFSET, remote_loopback_enable));
    CHECK(pmd_write_field(mss, TX_FEP_LOOPBACK_FIFO_TOP_ADDR, TX_FEP_LOOPBACK_FIFO_TOP_FIFO_ENABLE_A_MASK, TX_FEP_LOOPBACK_FIFO_TOP_FIFO_ENABLE_A_OFFSET, remote_loopback_enable));
    CHECK(pmd_write_field(mss, LOOPBACK_CNTRL_ADDR, LOOPBACK_CNTRL_TX_BITCK_LOOPBACK_ENA_NT_MASK, LOOPBACK_CNTRL_TX_BITCK_LOOPBACK_ENA_NT_OFFSET, remote_loopback_enable));
    CHECK(pmd_write_field(mss, LOOPBACK_CNTRL_ADDR, LOOPBACK_CNTRL_RX_BITCK_LOOPBACK_ENA_NT_MASK, LOOPBACK_CNTRL_RX_BITCK_LOOPBACK_ENA_NT_OFFSET, remote_loopback_enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_remote_loopback_get(mss_access_t *mss, uint32_t *remote_loopback_enable){
    /* Return if the remote (far-end) loopback is enabled/disabled. */
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_FEP_LOOPBACK_ENABLE_A_MASK, TX_DATAPATH_REG1_FEP_LOOPBACK_ENABLE_A_OFFSET, remote_loopback_enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_analog_loopback_set(mss_access_t *mss, uint32_t analog_loopback_enable){
    /* Enable/disable analog local (near-end) loopback (TX -> RX) for the given lane.
     * This loopback must be configured after rate/power state change
     * (after irst_*_ln[#] signals are asserted and the subsequent tasks
     * initiated by the assertion of the reset signals are carried out). Lane
     * remapping must not be used. */
    CHECK(pmd_write_field(mss, RX_ADAPT_VGA_OFFSET_CTLE_ADDR, RX_ADAPT_VGA_OFFSET_CTLE_ISOLATE_A_MASK, RX_ADAPT_VGA_OFFSET_CTLE_ISOLATE_A_OFFSET, analog_loopback_enable));
    CHECK(pmd_write_field(mss, LOOPBACK_CNTRL_ADDR, LOOPBACK_CNTRL_ENA_NT_MASK, LOOPBACK_CNTRL_ENA_NT_OFFSET, analog_loopback_enable));
    CHECK(pmd_write_field(mss, LOOPBACK_CNTRL_ADDR, LOOPBACK_CNTRL_TX_NES_LOOPBACK_ENA_NT_MASK, LOOPBACK_CNTRL_TX_NES_LOOPBACK_ENA_NT_OFFSET, analog_loopback_enable));
    CHECK(pmd_write_field(mss, LOOPBACK_CNTRL_ADDR, LOOPBACK_CNTRL_RX_NES_LOOPBACK_ENA_NT_MASK, LOOPBACK_CNTRL_RX_NES_LOOPBACK_ENA_NT_OFFSET, analog_loopback_enable));
    return JUPITER_ERR_CODE_NONE;
}
int jupiter_pmd_analog_loopback_get(mss_access_t *mss, uint32_t *analog_loopback_enable){
    /* Return if the analog local (near-end) loopback is enabled/disabled */
    CHECK(pmd_read_field(mss, LOOPBACK_CNTRL_ADDR, LOOPBACK_CNTRL_TX_NES_LOOPBACK_ENA_NT_MASK, LOOPBACK_CNTRL_TX_NES_LOOPBACK_ENA_NT_OFFSET, analog_loopback_enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_polarity_set(mss_access_t *mss, uint32_t tx_pol_flip){
    /* Set tx polarity for a given lane. */
    /* 1 for inverted tx_polarity */
    /* 0 for normal operation */
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG2_ADDR, TX_DATAPATH_REG2_INVERT_ENABLE_A_MASK, TX_DATAPATH_REG2_INVERT_ENABLE_A_OFFSET, tx_pol_flip));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_polarity_get(mss_access_t *mss, uint32_t *tx_pol_flip){
    /* 1 for inverted tx_polarity */
    /* 0 for normal operation */
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG2_ADDR, TX_DATAPATH_REG2_INVERT_ENABLE_A_MASK, TX_DATAPATH_REG2_INVERT_ENABLE_A_OFFSET, tx_pol_flip));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_polarity_set(mss_access_t *mss, uint32_t rx_pol_flip){
    /* 1 for inverted rx_polarity */
    /* 0 for normal operation */
    CHECK(pmd_write_field(mss, RX_DEMAPPER_ADDR, RX_DEMAPPER_INVERT_ENABLE_A_MASK, RX_DEMAPPER_INVERT_ENABLE_A_OFFSET, rx_pol_flip));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_polarity_get(mss_access_t *mss, uint32_t *rx_pol_flip){
    /* 1 for inverted rx_polarity */
    /* 0 for normal operation */
    CHECK(pmd_read_field(mss, RX_DEMAPPER_ADDR, RX_DEMAPPER_INVERT_ENABLE_A_MASK, RX_DEMAPPER_INVERT_ENABLE_A_OFFSET, rx_pol_flip));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_hbridge_set(mss_access_t *mss, uint32_t msb, uint32_t lsb){
    /* Set txhbridge, used to adjust Tx PAM4 eye linearity. */
    CHECK(pmd_write_field(mss, TX_HBRIDGE_ADDR, TX_HBRIDGE_BIAS_ADJ_NT_MASK, TX_HBRIDGE_BIAS_ADJ_NT_OFFSET, msb));
    CHECK(pmd_write_field(mss, TX_HBRIDGE_ADDR, TX_HBRIDGE_LSB_OVR_ENA_NT_MASK, TX_HBRIDGE_LSB_OVR_ENA_NT_OFFSET, 1));
    CHECK(pmd_write_field(mss, TX_HBRIDGE_ADDR, TX_HBRIDGE_LSB_OVR_NT_MASK, TX_HBRIDGE_LSB_OVR_NT_OFFSET, lsb));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_hbridge_get(mss_access_t *mss, uint32_t *msb, uint32_t *lsb){
    CHECK(pmd_read_field(mss, TX_HBRIDGE_ADDR, TX_HBRIDGE_BIAS_ADJ_NT_MASK, TX_HBRIDGE_BIAS_ADJ_NT_OFFSET, msb));
    CHECK(pmd_read_field(mss, TX_HBRIDGE_ADDR, TX_HBRIDGE_LSB_OVR_NT_MASK, TX_HBRIDGE_LSB_OVR_NT_OFFSET, lsb));
    return JUPITER_ERR_CODE_NONE;
}

/* Everything previously defined in the given lane_cfg_t struct was not
 * applicable. We are reserving these functions here incase there is a future
 * usecase, if not, they will be removed.
 * int jupiter_pmd_lane_cfg_set(mss_access_t *mss, lane_cfg_t lane_cfg){}
 * int jupiter_pmd_lane_cfg_get(mss_access_t *mss, lane_cfg_t *lane_cfg){} */

int jupiter_pmd_rx_dfe_adapt_set(mss_access_t *mss, uint32_t dfe_adapt_enable){
    CHECK(pmd_write_field(mss, RX_EQDFE_ADDR, RX_EQDFE_DFE_ENABLE_NT_MASK, RX_EQDFE_DFE_ENABLE_NT_OFFSET, dfe_adapt_enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_dfe_adapt_get(mss_access_t *mss, uint32_t *dfe_adapt_enable){
    CHECK(pmd_read_field(mss, RX_EQDFE_ADDR, RX_EQDFE_DFE_ENABLE_NT_MASK, RX_EQDFE_DFE_ENABLE_NT_OFFSET, dfe_adapt_enable));
    return JUPITER_ERR_CODE_NONE;
}

/* Enables or disables CTLE adaptation. If ctle adapt is disabled, ctle boost
 * code will default to ctle_boost_a. 
 * If ctle adapt is enabled, initial ctle boost value will default to 0. */
int jupiter_pmd_rx_ctle_adapt_set(mss_access_t *mss, uint32_t ctle_adapt_enable, uint32_t ctle_boost_a){
    uint32_t read_scratch;
    uint32_t write_scratch;
    
    uint32_t ccg_ctle;
    if (ctle_adapt_enable) {
        CHECK(pmd_write_field(mss, RX_CCG_ADDR, RX_CCG_CTLE_ADAPT_ENA_A_MASK, RX_CCG_CTLE_ADAPT_ENA_A_OFFSET, 1));
        CHECK(pmd_write_field(mss, RX_CTLE_ADAPT_BYPASS_ADDR, RX_CTLE_ADAPT_BYPASS_SETTING_A_MASK, RX_CTLE_ADAPT_BYPASS_SETTING_A_OFFSET, 0));
        CHECK(pmd_write_field(mss, RX_CTLE_ADAPT_BYPASS_ADDR, RX_CTLE_ADAPT_BYPASS_ENABLE_A_MASK, RX_CTLE_ADAPT_BYPASS_ENABLE_A_OFFSET, 0));
        CHECK(pmd_write_field(mss, RX_CTLE_ADAPT_LMS_ADDR, RX_CTLE_ADAPT_LMS_INVERT_ENA_A_MASK, RX_CTLE_ADAPT_LMS_INVERT_ENA_A_OFFSET, 0));
    } else {
        CHECK(pmd_read_field(mss, RX_CCG_ADDR, RX_CCG_CTLE_ADAPT_ENA_A_MASK, RX_CCG_CTLE_ADAPT_ENA_A_OFFSET, &ccg_ctle));
        CHECK(pmd_write_field(mss, RX_CCG_ADDR, RX_CCG_CTLE_ADAPT_ENA_A_MASK, RX_CCG_CTLE_ADAPT_ENA_A_OFFSET, 1));
        CHECK(pmd_write_field(mss, RX_CTLE_ADAPT_BYPASS_ADDR, RX_CTLE_ADAPT_BYPASS_SETTING_A_MASK, RX_CTLE_ADAPT_BYPASS_SETTING_A_OFFSET, ctle_boost_a));
        CHECK(pmd_write_field(mss, RX_CTLE_ADAPT_BYPASS_ADDR, RX_CTLE_ADAPT_BYPASS_ENABLE_A_MASK, RX_CTLE_ADAPT_BYPASS_ENABLE_A_OFFSET, 1));
        CHECK(pmd_write_field(mss, RX_CCG_ADDR, RX_CCG_CTLE_ADAPT_ENA_A_MASK, RX_CCG_CTLE_ADAPT_ENA_A_OFFSET, ccg_ctle));
    }
    CHECK(pmd_read_field(mss, RXMFSM_SCRATCH_REG7_ADDR, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_MASK, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_OFFSET, &read_scratch));
    write_scratch = (read_scratch & 0xfffeffff) | ((~ctle_adapt_enable & 0x1) << 17);
    CHECK(pmd_write_field(mss, RXMFSM_SCRATCH_REG7_ADDR, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_MASK, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_OFFSET, write_scratch));

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_ctle_adapt_get(mss_access_t *mss, uint32_t *ctle_adapt_enable, uint32_t *ctle_boost_a){
    uint32_t bypass_en;
    CHECK(pmd_read_field(mss, RX_CTLE_ADAPT_BYPASS_ADDR, RX_CTLE_ADAPT_BYPASS_SETTING_A_MASK, RX_CTLE_ADAPT_BYPASS_SETTING_A_OFFSET, ctle_boost_a));
    CHECK(pmd_read_field(mss, RX_CTLE_ADAPT_BYPASS_ADDR, RX_CTLE_ADAPT_BYPASS_ENABLE_A_MASK, RX_CTLE_ADAPT_BYPASS_ENABLE_A_OFFSET, &bypass_en));
    if (bypass_en){
        *ctle_adapt_enable = 0;
    } else {
        *ctle_adapt_enable = 1;
    }
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_background_adapt_enable_set(mss_access_t *mss, uint32_t rx_bkgrnd_adapt_enable){
    CHECK(pmd_write_field(mss, SEQ_CNTRL_ADAPT_ADDR, SEQ_CNTRL_ADAPT_BACKGROUND_SEL_ENA_A_MASK, SEQ_CNTRL_ADAPT_BACKGROUND_SEL_ENA_A_OFFSET, rx_bkgrnd_adapt_enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_background_adapt_enable_get(mss_access_t *mss, uint32_t *rx_bkgrnd_adapt_enable){
    CHECK(pmd_read_field(mss, SEQ_CNTRL_ADAPT_ADDR, SEQ_CNTRL_ADAPT_BACKGROUND_SEL_ENA_A_MASK, SEQ_CNTRL_ADAPT_BACKGROUND_SEL_ENA_A_OFFSET, rx_bkgrnd_adapt_enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_autoeq_set(mss_access_t *mss, uint32_t rx_autoeq_enable){
    CHECK(pmd_write_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_AUTOEQ_EN_NT_MASK, RX_SIGNAL_DETECT_REG3_AUTOEQ_EN_NT_OFFSET, rx_autoeq_enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_autoeq_get(mss_access_t *mss, uint32_t *rx_autoeq_enable){
    CHECK(pmd_read_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_AUTOEQ_EN_NT_MASK, RX_SIGNAL_DETECT_REG3_AUTOEQ_EN_NT_OFFSET, rx_autoeq_enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_signal_detect_en_set(mss_access_t *mss, uint32_t en){
    CHECK(pmd_write_field(mss, RX_SIGNAL_DETECT_REG3_ADDR, RX_SIGNAL_DETECT_REG3_VALID_PCS_SEL_NT_MASK, RX_SIGNAL_DETECT_REG3_VALID_PCS_SEL_NT_OFFSET, 0));
    CHECK(pmd_write_field(mss, PD_AFE_RX_ADDR, PD_AFE_RX_SIGDET_BA_MASK, PD_AFE_RX_SIGDET_BA_OFFSET, en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_vga_cap_set(mss_access_t *mss, uint32_t vga_cap) {
    CHECK(pmd_write_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_0_A_MASK, RX_VGA_REG1_CAP1_LUT_0_A_OFFSET, vga_cap));
    CHECK(pmd_write_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_1_A_MASK, RX_VGA_REG1_CAP1_LUT_1_A_OFFSET, vga_cap));
    CHECK(pmd_write_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_2_A_MASK, RX_VGA_REG1_CAP1_LUT_2_A_OFFSET, vga_cap));
    CHECK(pmd_write_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_3_A_MASK, RX_VGA_REG1_CAP1_LUT_3_A_OFFSET, vga_cap));
    CHECK(pmd_write_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_4_A_MASK, RX_VGA_REG1_CAP1_LUT_4_A_OFFSET, vga_cap));
    CHECK(pmd_write_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_5_A_MASK, RX_VGA_REG1_CAP1_LUT_5_A_OFFSET, vga_cap));
    CHECK(pmd_write_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_6_A_MASK, RX_VGA_REG1_CAP1_LUT_6_A_OFFSET, vga_cap));
    CHECK(pmd_write_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_7_A_MASK, RX_VGA_REG1_CAP1_LUT_7_A_OFFSET, vga_cap));
    return JUPITER_ERR_CODE_NONE;
}



int jupiter_pmd_rx_vga_cap_get(mss_access_t *mss, uint32_t *vga_cap) {
    CHECK(pmd_read_field(mss, RX_VGA_REG1_ADDR, RX_VGA_REG1_CAP1_LUT_0_A_MASK, RX_VGA_REG1_CAP1_LUT_0_A_OFFSET, vga_cap));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_vga_cap_adapt_set(mss_access_t *mss, uint32_t en) {
    uint32_t read_scratch;
    uint32_t write_scratch;
    
    CHECK(pmd_read_field(mss, RXMFSM_SCRATCH_REG7_ADDR, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_MASK, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_OFFSET, &read_scratch));
    write_scratch = (read_scratch & 0xfffeffff) | ((~en & 0x1) << 16);
    CHECK(pmd_write_field(mss, RXMFSM_SCRATCH_REG7_ADDR, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_MASK, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_OFFSET, write_scratch));

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_c0_adapt_set(mss_access_t *mss, uint32_t en){
    uint32_t read_scratch;
    uint32_t write_scratch;
    
    CHECK(pmd_read_field(mss, RXMFSM_SCRATCH_REG7_ADDR, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_MASK, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_OFFSET, &read_scratch));
    write_scratch = (read_scratch & 0xfffeffff) | ((~en & 0x1) << 18);
    CHECK(pmd_write_field(mss, RXMFSM_SCRATCH_REG7_ADDR, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_MASK, RXMFSM_SCRATCH_REG7_RXMFSM_SCRATCH7_OFFSET, write_scratch));

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_cdr_offset_set(mss_access_t *mss, uint32_t cdr_offset, uint32_t cdr_dir) {
    uint32_t cdr_offset_dir;
    cdr_offset_dir = (cdr_offset & 0x00ff) | (cdr_dir << 8);
    CHECK(pmd_write_field(mss, RXMFSM_SCRATCH_REG11_ADDR, RXMFSM_SCRATCH_REG11_RXMFSM_SCRATCH11_MASK, RXMFSM_SCRATCH_REG11_RXMFSM_SCRATCH11_OFFSET, cdr_offset_dir));
    return JUPITER_ERR_CODE_NONE;
}

/* Status APIs */
int jupiter_pmd_rx_signal_detect_get(mss_access_t *mss, uint32_t *signal_detect){
    CHECK(pmd_read_field(mss, RX_STAT_ADDR, RX_STAT_ODAT_RX_SIGNAL_DETECT_A_MASK, RX_STAT_ODAT_RX_SIGNAL_DETECT_A_OFFSET, signal_detect));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_signal_detect_check(mss_access_t *mss, uint32_t signal_detect_expected){
    uint32_t signal_detect;
    CHECK(pmd_read_check_field(mss, RX_STAT_ADDR, RX_STAT_ODAT_RX_SIGNAL_DETECT_A_MASK, RX_STAT_ODAT_RX_SIGNAL_DETECT_A_OFFSET, RD_EQ, &signal_detect,  signal_detect_expected, 0 /*NULL*/));
    if (signal_detect != signal_detect_expected) {
        USR_PRINTF(("ERROR: Expected signal_detect = %d, Actual signal_detect = %d\n",signal_detect_expected, signal_detect)); 
        return JUPITER_ERR_CODE_CHECK_FAILURE;
    } else {
        USR_PRINTF(("Expected signal_detect = %d, Actual signal_detect = %d\n",signal_detect_expected, signal_detect));
        return JUPITER_ERR_CODE_NONE;
    }
}


/* timing_window: Sampling measurement window to be `2**timing_window-1` clock cycles */
int jupiter_pmd_tx_ppm_get(mss_access_t *mss, uint32_t timing_window, uint32_t timeout_us, USR_DOUBLE *tx_ppm){
    /* save timing window value prior to measurement */
    uint32_t old_timing_window;
    int poll_result;
    uint32_t freq_meas_result;
    USR_DOUBLE ppm;

    CHECK(pmd_read_field(mss, TX_VCO_ADAPT_REG2_ADDR, TX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_MASK, TX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_OFFSET, &old_timing_window));
    /* assert req, check for poll error */
    CHECK(pmd_write_field(mss, TX_VCO_ADAPT_REG2_ADDR, TX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_MASK, TX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_OFFSET, timing_window));
    CHECK(pmd_write_field(mss, TX_VCO_ADAPT_REG2_ADDR, TX_VCO_ADAPT_REG2_FREQ_MEASURE_REQ_A_MASK, TX_VCO_ADAPT_REG2_FREQ_MEASURE_REQ_A_OFFSET, 1));
    poll_result = pmd_poll_field(mss, TX_VCO_ADAPT_RDREG2_ADDR, TX_VCO_ADAPT_RDREG2_FREQ_MEASURE_ACK_NT_MASK, TX_VCO_ADAPT_RDREG2_FREQ_MEASURE_ACK_NT_OFFSET, 1, timeout_us);
    if (poll_result == -1) {
        USR_PRINTF(("ERROR: polling for freq measure ack\n"));
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        USR_PRINTF(("freq measure ack received\n"));
    }
    CHECK(pmd_read_field(mss, TX_VCO_ADAPT_RDREG2_ADDR, TX_VCO_ADAPT_RDREG2_FREQ_MEASURE_RESULT_NT_MASK, TX_VCO_ADAPT_RDREG2_FREQ_MEASURE_RESULT_NT_OFFSET, &freq_meas_result));
    /* de-assert req, check for poll error */
    CHECK(pmd_write_field(mss, TX_VCO_ADAPT_REG2_ADDR, TX_VCO_ADAPT_REG2_FREQ_MEASURE_REQ_A_MASK, TX_VCO_ADAPT_REG2_FREQ_MEASURE_REQ_A_OFFSET, 0));
    poll_result = pmd_poll_field(mss, TX_VCO_ADAPT_RDREG2_ADDR, TX_VCO_ADAPT_RDREG2_FREQ_MEASURE_ACK_NT_MASK, TX_VCO_ADAPT_RDREG2_FREQ_MEASURE_ACK_NT_OFFSET, 0, timeout_us);
    if (poll_result == -1) {
        USR_PRINTF(("ERROR: polling for freq measure ack\n"));
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        USR_PRINTF(("freq measure ack received\n"));
    }

    /* restore timing window to value prior to measurement */
    CHECK(pmd_write_field(mss, TX_VCO_ADAPT_REG2_ADDR, TX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_MASK, TX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_OFFSET, old_timing_window));
    /* compute ppm. result is in S3.12 format, need to convert first. */
    if (((freq_meas_result >> 15) & 0x1) == 1) {
        freq_meas_result = 0xffff - freq_meas_result;
    }
    ppm = (1000000)*freq_meas_result/(1 << timing_window);
    *tx_ppm = ppm;
    return JUPITER_ERR_CODE_NONE;
}

/* timing_window: Sampling measurement window to be `2**timing_window-1` clock cycles */
int jupiter_pmd_rx_ppm_get(mss_access_t *mss, uint32_t timing_window, uint32_t timeout_us, USR_DOUBLE *rx_ppm){
    /* save timing window value prior to measurement */
    uint32_t old_timing_window;
    uint32_t freq_meas_result;
    int poll_result;
    USR_DOUBLE ppm;
    CHECK(pmd_read_field(mss, RX_VCO_ADAPT_REG2_ADDR, RX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_MASK, RX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_OFFSET, &old_timing_window));
    /* assert req, check for poll error */
    CHECK(pmd_write_field(mss, RX_VCO_ADAPT_REG2_ADDR, RX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_MASK, RX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_OFFSET, timing_window));
    CHECK(pmd_write_field(mss, RX_VCO_ADAPT_REG2_ADDR, RX_VCO_ADAPT_REG2_FREQ_MEASURE_REQ_A_MASK, RX_VCO_ADAPT_REG2_FREQ_MEASURE_REQ_A_OFFSET, 1));
    poll_result = pmd_poll_field(mss, RX_VCO_ADAPT_RDREG2_ADDR, RX_VCO_ADAPT_RDREG2_FREQ_MEASURE_ACK_NT_MASK, RX_VCO_ADAPT_RDREG2_FREQ_MEASURE_ACK_NT_OFFSET, 1, timeout_us);
    if (poll_result == -1) {
        USR_PRINTF(("ERROR: polling for freq measure ack\n"));
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        USR_PRINTF(("freq measure ack received\n"));
    }
    CHECK(pmd_read_field(mss, RX_VCO_ADAPT_RDREG2_ADDR, RX_VCO_ADAPT_RDREG2_FREQ_MEASURE_RESULT_NT_MASK, RX_VCO_ADAPT_RDREG2_FREQ_MEASURE_RESULT_NT_OFFSET, &freq_meas_result));
    /* de-assert req, check for poll error */
    CHECK(pmd_write_field(mss, RX_VCO_ADAPT_REG2_ADDR, RX_VCO_ADAPT_REG2_FREQ_MEASURE_REQ_A_MASK, RX_VCO_ADAPT_REG2_FREQ_MEASURE_REQ_A_OFFSET, 0));
    poll_result = pmd_poll_field(mss, RX_VCO_ADAPT_RDREG2_ADDR, RX_VCO_ADAPT_RDREG2_FREQ_MEASURE_ACK_NT_MASK, RX_VCO_ADAPT_RDREG2_FREQ_MEASURE_ACK_NT_OFFSET, 0, timeout_us);
    if (poll_result == -1) {
        USR_PRINTF(("ERROR: polling for freq measure ack\n"));
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        USR_PRINTF(("freq measure ack received\n"));
    }

    /* restore timing window to value prior to measurement */
    CHECK(pmd_write_field(mss, RX_VCO_ADAPT_REG2_ADDR, RX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_MASK, RX_VCO_ADAPT_REG2_TIMING_WINDOW_NT_OFFSET, old_timing_window));
    /* compute ppm. result is in S3.12 format, need to convert first.  */
    if (((freq_meas_result >> 15) & 0x1) == 1) {
        freq_meas_result = 0xffff - freq_meas_result;
    }
    ppm = (1000000)*freq_meas_result/(1 << timing_window);
    *rx_ppm = ppm;
    return JUPITER_ERR_CODE_NONE;
}

/* 0 - Not locked */
/* 1 - Locked */
int jupiter_pmd_rx_lock_status_get(mss_access_t *mss, uint32_t *pmd_rx_lock){
    CHECK(pmd_read_field(mss, RX_STAT_ADDR, RX_STAT_OCTL_RX_DATA_VLD_MASK, RX_STAT_OCTL_RX_DATA_VLD_OFFSET, pmd_rx_lock));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_dcdiq_get(mss_access_t *mss, jupiter_dcdiq_data_t *rx_dcdiq_data){
   CHECK(pmd_read_field(mss, RX_PHASE_ADAPT_RDREG_ADDR, RX_PHASE_ADAPT_RDREG_D0_DCD_A_MASK, RX_PHASE_ADAPT_RDREG_D0_DCD_A_OFFSET, &(rx_dcdiq_data->d0) ));
   CHECK(pmd_read_field(mss, RX_PHASE_ADAPT_RDREG_ADDR, RX_PHASE_ADAPT_RDREG_D90_DCD_A_MASK, RX_PHASE_ADAPT_RDREG_D90_DCD_A_OFFSET, &(rx_dcdiq_data->d90) ));
   CHECK(pmd_read_field(mss, RX_PHASE_ADAPT_RDREG_ADDR, RX_PHASE_ADAPT_RDREG_IQ_A_MASK, RX_PHASE_ADAPT_RDREG_IQ_A_OFFSET, &(rx_dcdiq_data->iq) ));
   return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_dcdiq_get(mss_access_t *mss, jupiter_dcdiq_data_t *tx_dcdiq_data){
   CHECK(pmd_read_field(mss, TX_PHASE_ADAPT_RDREG_ADDR, TX_PHASE_ADAPT_RDREG_D0_DCD_A_MASK, TX_PHASE_ADAPT_RDREG_D0_DCD_A_OFFSET, &(tx_dcdiq_data->d0) ));
   CHECK(pmd_read_field(mss, TX_PHASE_ADAPT_RDREG_ADDR, TX_PHASE_ADAPT_RDREG_D90_DCD_A_MASK, TX_PHASE_ADAPT_RDREG_D90_DCD_A_OFFSET, &(tx_dcdiq_data->d90) ));
   CHECK(pmd_read_field(mss, TX_PHASE_ADAPT_RDREG_ADDR, TX_PHASE_ADAPT_RDREG_IQ_A_MASK, TX_PHASE_ADAPT_RDREG_IQ_A_OFFSET, &(tx_dcdiq_data->iq) ));
   return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_afe_get(mss_access_t *mss, jupiter_afe_data_t *rx_afe_data){
    CHECK(pmd_read_field(mss, RX_CTLE_ADDR, RX_CTLE_RATE_NT_MASK, RX_CTLE_RATE_NT_OFFSET, &(rx_afe_data->ctle_rate) ));
    CHECK(pmd_read_field(mss, RX_CTLE_ADAPT_STATUS_ADDR, RX_CTLE_ADAPT_STATUS_CTLE_BOOST_A_MASK, RX_CTLE_ADAPT_STATUS_CTLE_BOOST_A_OFFSET, &(rx_afe_data->ctle_boost) ));
    CHECK(pmd_read_field(mss, RX_AGC_ADAPT_STATUS_ADDR, RX_AGC_ADAPT_STATUS_VGA_COARSE_A_MASK, RX_AGC_ADAPT_STATUS_VGA_COARSE_A_OFFSET, &(rx_afe_data->vga_coarse) ));
    CHECK(pmd_read_field(mss, RX_AGC_ADAPT_STATUS_ADDR, RX_AGC_ADAPT_STATUS_VGA_FINE_A_MASK, RX_AGC_ADAPT_STATUS_VGA_FINE_A_OFFSET, &(rx_afe_data->vga_fine) ));
    CHECK(pmd_read_field(mss, RX_ADAPT_VGA_OFFSET_STATUS_ADDR, RX_ADAPT_VGA_OFFSET_STATUS_VGA_OFFSET_A_MASK, RX_ADAPT_VGA_OFFSET_STATUS_VGA_OFFSET_A_OFFSET, &(rx_afe_data->vga_offset) ));
    return JUPITER_ERR_CODE_NONE;
}

/* BIST APIs */
int jupiter_pmd_rx_chk_config_set(mss_access_t *mss, jupiter_bist_pattern_t pattern, jupiter_bist_mode_t mode, uint64_t udp, uint32_t lock_thresh, uint32_t timer_thresh){
    uint32_t udp_31_0;
    uint32_t udp_63_32;
    uint32_t udp_en = 0;

    CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_BIST_ENABLE_A_MASK, RX_DATABIST_TOP_REG1_BIST_ENABLE_A_OFFSET, 0));
    CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_MASK, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_OFFSET, 0));
    CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_MASK, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_OFFSET, 1));
    CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_MASK, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_OFFSET, 0));

    switch (pattern){
    case JUPITER_PRBS7:
    case JUPITER_PRBS9:
    case JUPITER_PRBS11:
    case JUPITER_PRBS13:
    case JUPITER_PRBS15:
    case JUPITER_PRBS23:
    case JUPITER_PRBS31:
    case JUPITER_QPRBS13:
    case JUPITER_JP03A:
    case JUPITER_JP03B:
    case JUPITER_LINEARITY_PATTERN:
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG2_ADDR, RX_DATABIST_TOP_REG2_PATTERN_SEL_NT_MASK, RX_DATABIST_TOP_REG2_PATTERN_SEL_NT_OFFSET, pattern));
        break;
    case JUPITER_USER_DEFINED_PATTERN:
        udp_31_0 = COMPILER_64_LO(udp);
        udp_63_32 = COMPILER_64_HI(udp);
        udp_en = 1;
    case JUPITER_FULL_RATE_CLOCK:
        udp_31_0  = 0xAAAAAAAA;
        udp_63_32 = 0xAAAAAAAA;        
        udp_en = 1;
        break;
    case JUPITER_HALF_RATE_CLOCK:
        udp_31_0  = 0xCCCCCCCC;
        udp_63_32 = 0xCCCCCCCC;
        udp_en = 1;
        break;
    case JUPITER_QUARTER_RATE_CLOCK:
        udp_31_0  = 0xF0F0F0F0;
        udp_63_32 = 0xF0F0F0F0;
        udp_en = 1;
        break;
    case JUPITER_PATT_32_1S_32_0S:
        udp_63_32 = 0xFFFFFFFF;
        udp_31_0  = 0x00000000;
        udp_en = 1;
        break;
    default:
        return JUPITER_ERR_CODE_INVALID_ARG_VALUE;
    }
    if (udp_en) {
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG2_ADDR, RX_DATABIST_TOP_REG2_PATTERN_SEL_NT_MASK, RX_DATABIST_TOP_REG2_PATTERN_SEL_NT_OFFSET, JUPITER_USER_DEFINED_PATTERN));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG4_ADDR, RX_DATABIST_TOP_REG4_UDP_PATTERN_31_0_NT_MASK, RX_DATABIST_TOP_REG4_UDP_PATTERN_31_0_NT_OFFSET, udp_31_0));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG5_ADDR, RX_DATABIST_TOP_REG5_UDP_PATTERN_63_32_NT_MASK, RX_DATABIST_TOP_REG5_UDP_PATTERN_63_32_NT_OFFSET, udp_63_32));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG6_ADDR, RX_DATABIST_TOP_REG6_UDP_PATTERN_95_64_NT_MASK, RX_DATABIST_TOP_REG6_UDP_PATTERN_95_64_NT_OFFSET, udp_31_0));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG7_ADDR, RX_DATABIST_TOP_REG7_UDP_PATTERN_127_96_NT_MASK, RX_DATABIST_TOP_REG7_UDP_PATTERN_127_96_NT_OFFSET, udp_63_32));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG8_ADDR, RX_DATABIST_TOP_REG8_UDP_PATTERN_159_128_NT_MASK, RX_DATABIST_TOP_REG8_UDP_PATTERN_159_128_NT_OFFSET, udp_31_0));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG9_ADDR, RX_DATABIST_TOP_REG9_UDP_PATTERN_191_160_NT_MASK, RX_DATABIST_TOP_REG9_UDP_PATTERN_191_160_NT_OFFSET, udp_63_32));
    }

    if (mode == JUPITER_TIMER) {
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_WALL_CLOCK_ENABLE_A_MASK, RX_DATABIST_TOP_REG1_WALL_CLOCK_ENABLE_A_OFFSET, 0));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_BIST_MODE_NT_MASK, RX_DATABIST_TOP_REG1_BIST_MODE_NT_OFFSET, 0));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_LOCK_THRESHOLD_NT_MASK, RX_DATABIST_TOP_REG1_LOCK_THRESHOLD_NT_OFFSET, lock_thresh));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG2_ADDR, RX_DATABIST_TOP_REG2_TIMER_THRESHOLD_NT_MASK, RX_DATABIST_TOP_REG2_TIMER_THRESHOLD_NT_OFFSET, timer_thresh));
    } else {
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_WALL_CLOCK_ENABLE_A_MASK, RX_DATABIST_TOP_REG1_WALL_CLOCK_ENABLE_A_OFFSET, 1));
        CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_BIST_MODE_NT_MASK, RX_DATABIST_TOP_REG1_BIST_MODE_NT_OFFSET, 1));
    }
    return JUPITER_ERR_CODE_NONE;
}

/* NOTE: changed from pmd_chk_rx_config_get -> pmd_rx_chk_config_get */
int jupiter_pmd_rx_chk_config_get(mss_access_t *mss, jupiter_bist_pattern_t *pattern, jupiter_bist_mode_t *mode, uint64_t *udp, uint32_t *lock_thresh, uint32_t *timer_thresh){
    uint32_t patt;
    uint32_t udp_31_0;
    uint32_t udp_63_32;
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG2_ADDR, RX_DATABIST_TOP_REG2_PATTERN_SEL_NT_MASK, RX_DATABIST_TOP_REG2_PATTERN_SEL_NT_OFFSET, &patt)); /* 11 is UDP */
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG4_ADDR, RX_DATABIST_TOP_REG4_UDP_PATTERN_31_0_NT_MASK, RX_DATABIST_TOP_REG4_UDP_PATTERN_31_0_NT_OFFSET, &udp_31_0));
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG5_ADDR, RX_DATABIST_TOP_REG5_UDP_PATTERN_63_32_NT_MASK, RX_DATABIST_TOP_REG5_UDP_PATTERN_63_32_NT_OFFSET, &udp_63_32));

    if (patt >= JUPITER_USER_DEFINED_PATTERN){
        if (udp_31_0 == 0xAAAAAAAA){
            *pattern = JUPITER_FULL_RATE_CLOCK;
        } else if (udp_31_0 == 0xCCCCCCCC){
            *pattern = JUPITER_HALF_RATE_CLOCK;
        } else if (udp_31_0 == 0xF0F0F0F0){
            *pattern = JUPITER_QUARTER_RATE_CLOCK;
        } else if (udp_31_0 == 0x00000000 && udp_63_32 == 0xFFFFFFFF){
            *pattern = JUPITER_PATT_32_1S_32_0S;
        } else {
            *pattern = JUPITER_USER_DEFINED_PATTERN;
            COMPILER_64_SET(*udp, udp_63_32, udp_31_0);
        }
    } else {
        *pattern = (jupiter_bist_pattern_t) patt;
        COMPILER_64_SET(*udp, 0, 0); /* return JUPITER_ERR_CODE_NONE for UDP if the mode is not UDP */
    }
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_BIST_MODE_NT_MASK, RX_DATABIST_TOP_REG1_BIST_MODE_NT_OFFSET, mode));
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_PPM_LOCK_DETECT_REG1_FREQ_LOCK_THRESHOLD_NT_MASK, RX_PPM_LOCK_DETECT_REG1_FREQ_LOCK_THRESHOLD_NT_OFFSET, lock_thresh));
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG2_ADDR, RX_DATABIST_TOP_REG2_TIMER_THRESHOLD_NT_MASK, RX_DATABIST_TOP_REG2_TIMER_THRESHOLD_NT_OFFSET, timer_thresh));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_chk_en_set(mss_access_t *mss, uint32_t enable){
    /* 0 - disable RX BIST */
    /* 1 - enable RX BIST */
    CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_BIST_ENABLE_A_MASK, RX_DATABIST_TOP_REG1_BIST_ENABLE_A_OFFSET, enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_chk_en_get(mss_access_t *mss, uint32_t *enable){
    /* 0 - disable RX BIST */
    /* 1 - enable RX BIST */
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_BIST_ENABLE_A_MASK, RX_DATABIST_TOP_REG1_BIST_ENABLE_A_OFFSET, enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_chk_lock_state_get(mss_access_t *mss, uint32_t *rx_bist_lock){
    /* 0 - RX BIST not locked */
    /* 1 - RX BIST locked */
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_LOCKED_NT_MASK, RX_DATABIST_TOP_RDREG1_LOCKED_NT_OFFSET, rx_bist_lock));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_chk_err_count_state_get(mss_access_t *mss, uint64_t *err_count, uint32_t *err_count_done, uint32_t *err_count_overflown){
    uint32_t bist_mode;
    uint32_t err_cnt_55_32;
    uint32_t err_cnt_31_0;
    uint32_t err_code;
    
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_BIST_MODE_NT_MASK, RX_DATABIST_TOP_REG1_BIST_MODE_NT_OFFSET, &bist_mode));

    if (bist_mode == 1){
        uint32_t bist_enable;
        CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_BIST_ENABLE_A_MASK, RX_DATABIST_TOP_REG1_BIST_ENABLE_A_OFFSET, &bist_enable));
        if (bist_enable == 1){
            *err_count_done = 0;
            COMPILER_64_SET(*err_count, 0, 0);
            *err_count_overflown = 0;
            err_code = 1;
        } else {
            *err_count_done = 1;
            CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG3_ADDR, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_MASK, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_OFFSET, &err_cnt_55_32));
            CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG2_ADDR, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_MASK, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_OFFSET, &err_cnt_31_0));
            COMPILER_64_SET(*err_count, err_cnt_55_32, err_cnt_31_0);
            CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_ERROR_CNT_OVERFLOW_NT_MASK, RX_DATABIST_TOP_RDREG1_ERROR_CNT_OVERFLOW_NT_OFFSET, err_count_overflown));
            err_code = 0;
        }
    } else {
        CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_ERROR_CNT_DONE_NT_MASK, RX_DATABIST_TOP_RDREG1_ERROR_CNT_DONE_NT_OFFSET, err_count_done));
        if (!(*err_count_done)) {
            *err_count_done = 0;
            COMPILER_64_SET(*err_count, 0, 0);
            *err_count_overflown = 0;
            err_code = 2;
        } else {
            *err_count_done = 1;
            CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG3_ADDR, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_MASK, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_OFFSET, &err_cnt_55_32));
            CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG2_ADDR, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_MASK, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_OFFSET, &err_cnt_31_0));
            COMPILER_64_SET(*err_count, err_cnt_55_32, err_cnt_31_0);
            CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_ERROR_CNT_OVERFLOW_NT_MASK, RX_DATABIST_TOP_RDREG1_ERROR_CNT_OVERFLOW_NT_OFFSET, err_count_overflown));
            err_code = 0;
        }
    }
    if (err_code == 1 || err_code == 2 ) {
        return JUPITER_ERR_CODE_FUNC_FAILURE;
    } else {
        return JUPITER_ERR_CODE_NONE;
    }
}

int jupiter_pmd_rx_chk_err_count_state_clear(mss_access_t *mss){
    CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_MASK, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_OFFSET, 1));
    CHECK(pmd_write_field(mss, RX_DATABIST_TOP_REG1_ADDR, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_MASK, RX_DATABIST_TOP_REG1_ERROR_CNT_CLR_A_OFFSET, 0));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_gen_config_set(mss_access_t *mss, jupiter_bist_pattern_t pattern, uint64_t udp){
    uint32_t udp_31_0;
    uint32_t udp_63_32;
    uint32_t udp_en = 0;

    switch (pattern){
    case JUPITER_PRBS7:
    case JUPITER_PRBS9:
    case JUPITER_PRBS11:
    case JUPITER_PRBS13:
    case JUPITER_PRBS15:
    case JUPITER_PRBS23:
    case JUPITER_PRBS31:
    case JUPITER_QPRBS13:
    case JUPITER_JP03A:
    case JUPITER_JP03B:
    case JUPITER_LINEARITY_PATTERN:
        CHECK(pmd_write_field(mss, TX_DATAPATH_REG2_ADDR, TX_DATAPATH_REG2_PATTERN_SEL_NT_MASK, TX_DATAPATH_REG2_PATTERN_SEL_NT_OFFSET, pattern));
        break;
    case JUPITER_USER_DEFINED_PATTERN:
        udp_31_0 = COMPILER_64_LO(udp);
        udp_63_32 = COMPILER_64_HI(udp);
        udp_en = 1;
        break;
    case JUPITER_FULL_RATE_CLOCK:
        udp_31_0 = 0xAAAAAAAA;
        udp_63_32 = 0xAAAAAAAA;
        udp_en = 1;
        break;
    case JUPITER_HALF_RATE_CLOCK:
        udp_31_0 = 0xCCCCCCCC;
        udp_63_32 = 0xCCCCCCCC;
        udp_en = 1;
        break;
    case JUPITER_QUARTER_RATE_CLOCK:
        udp_31_0 = 0xF0F0F0F0;
        udp_63_32 = 0xF0F0F0F0;
        udp_en = 1;
        break;
    case JUPITER_PATT_32_1S_32_0S:
        udp_63_32 = 0xFFFFFFFF;
        udp_31_0 = 0x00000000;
        udp_en = 1;
        break;
    default:
        return JUPITER_ERR_CODE_INVALID_ARG_VALUE; /* Error */
    }
    if (udp_en) {
        CHECK(pmd_write_field(mss, TX_DATAPATH_REG2_ADDR, TX_DATAPATH_REG2_PATTERN_SEL_NT_MASK, TX_DATAPATH_REG2_PATTERN_SEL_NT_OFFSET, pattern));
        CHECK(pmd_write_field(mss, TX_DATAPATH_REG8_ADDR, TX_DATAPATH_REG8_UDP_PATTERN_63_32_NT_MASK, TX_DATAPATH_REG8_UDP_PATTERN_63_32_NT_OFFSET, udp_63_32));
        CHECK(pmd_write_field(mss, TX_DATAPATH_REG7_ADDR, TX_DATAPATH_REG7_UDP_PATTERN_31_0_NT_MASK, TX_DATAPATH_REG7_UDP_PATTERN_31_0_NT_OFFSET, udp_31_0));
    }
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_gen_config_get(mss_access_t *mss, jupiter_bist_pattern_t *pattern, uint64_t *udp){
    uint32_t patt;
    uint32_t udp_31_0;
    uint32_t udp_63_32;

    CHECK(pmd_read_field(mss, TX_DATAPATH_REG2_ADDR, TX_DATAPATH_REG2_PATTERN_SEL_NT_MASK, TX_DATAPATH_REG2_PATTERN_SEL_NT_OFFSET, &patt));
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG8_ADDR, TX_DATAPATH_REG8_UDP_PATTERN_63_32_NT_MASK, TX_DATAPATH_REG8_UDP_PATTERN_63_32_NT_OFFSET, &udp_63_32));
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG7_ADDR, TX_DATAPATH_REG7_UDP_PATTERN_31_0_NT_MASK, TX_DATAPATH_REG7_UDP_PATTERN_31_0_NT_OFFSET, &udp_31_0));

    switch (patt){
    case JUPITER_PRBS7:
    case JUPITER_PRBS9:
    case JUPITER_PRBS11:
    case JUPITER_PRBS13:
    case JUPITER_PRBS15:
    case JUPITER_PRBS23:
    case JUPITER_PRBS31:
    case JUPITER_QPRBS13:
    case JUPITER_JP03A:
    case JUPITER_JP03B:
    case JUPITER_LINEARITY_PATTERN:
        CHECK(pmd_read_field(mss, TX_DATAPATH_REG2_ADDR, TX_DATAPATH_REG2_PATTERN_SEL_NT_MASK, TX_DATAPATH_REG2_PATTERN_SEL_NT_OFFSET, &patt));
        return JUPITER_ERR_CODE_NONE;
    case JUPITER_USER_DEFINED_PATTERN:
        if (udp_31_0 == 0xAAAAAAAA && udp_63_32 == 0xAAAAAAAA) {
            *pattern = JUPITER_FULL_RATE_CLOCK;
        } else if (udp_31_0 == 0xCCCCCCCC && udp_63_32 == 0xCCCCCCCC) {
            *pattern = JUPITER_HALF_RATE_CLOCK;
        } else if (udp_31_0 == 0xF0F0F0F0 && udp_63_32 == 0xF0F0F0F0) {
            *pattern = JUPITER_QUARTER_RATE_CLOCK;
        } else if (udp_31_0 == 0x00000000 && udp_63_32 == 0xFFFFFFFF) {
            *pattern = JUPITER_PATT_32_1S_32_0S;
        } else {
            *pattern = JUPITER_USER_DEFINED_PATTERN;
            COMPILER_64_SET(*udp, udp_63_32, udp_31_0);
        }
    default:
        return JUPITER_ERR_CODE_INVALID_ARG_VALUE;
    }

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_gen_en_set(mss_access_t *mss, uint32_t enable){
    /* 0 - disable TX BIST */
    /* 1 - enable TX BIST */
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_BIST_ENABLE_A_MASK, TX_DATAPATH_REG1_BIST_ENABLE_A_OFFSET, enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_gen_en_get(mss_access_t *mss, uint32_t *enable){
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_BIST_ENABLE_A_MASK, TX_DATAPATH_REG1_BIST_ENABLE_A_OFFSET, enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_gen_err_inject_config_set(mss_access_t *mss, uint64_t err_pattern, uint32_t err_rate){
    /* 'err_pattern': 64bit error injection pattern  
     * 'err_rate':  
     *     0 - inject 1 bit error  
     *     not 0 - error injection rate */
    uint32_t err_pattern_31_0 = COMPILER_64_LO(err_pattern);
    uint32_t err_pattern_63_32 = COMPILER_64_HI(err_pattern);
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG10_ADDR, TX_DATAPATH_REG10_ERROR_PATTERN_63_32_NT_MASK, TX_DATAPATH_REG10_ERROR_PATTERN_63_32_NT_OFFSET, err_pattern_63_32));
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG9_ADDR, TX_DATAPATH_REG9_ERROR_PATTERN_31_0_NT_MASK, TX_DATAPATH_REG9_ERROR_PATTERN_31_0_NT_OFFSET, err_pattern_31_0));
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_TXDATA_ERROR_RATE_NT_MASK, TX_DATAPATH_REG1_TXDATA_ERROR_RATE_NT_OFFSET, err_rate));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_gen_err_inject_config_get(mss_access_t *mss, uint64_t *err_pattern, uint32_t *err_rate){
    uint32_t err_pattern_63_32;
    uint32_t err_pattern_31_0;
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG10_ADDR, TX_DATAPATH_REG10_ERROR_PATTERN_63_32_NT_MASK, TX_DATAPATH_REG10_ERROR_PATTERN_63_32_NT_OFFSET, &err_pattern_63_32));
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG9_ADDR, TX_DATAPATH_REG9_ERROR_PATTERN_31_0_NT_MASK, TX_DATAPATH_REG9_ERROR_PATTERN_31_0_NT_OFFSET, &err_pattern_31_0));
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_TXDATA_ERROR_RATE_NT_MASK, TX_DATAPATH_REG1_TXDATA_ERROR_RATE_NT_OFFSET, err_rate));
    COMPILER_64_SET(*err_pattern, err_pattern_63_32, err_pattern_31_0);
    return JUPITER_ERR_CODE_NONE;

}

int jupiter_pmd_tx_gen_err_inject_en_set(mss_access_t *mss, uint32_t enable){
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_TXDATA_ERROR_ENABLE_A_MASK, TX_DATAPATH_REG1_TXDATA_ERROR_ENABLE_A_OFFSET, enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_gen_err_inject_en_get(mss_access_t *mss, uint32_t *enable){
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_TXDATA_ERROR_ENABLE_A_MASK, TX_DATAPATH_REG1_TXDATA_ERROR_ENABLE_A_OFFSET, enable));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_sweep_demapper(mss_access_t *mss, uint32_t npam4_nrz, uint32_t timeout_us){
    /* 0 - PAM4 mode */
    /* 1 - NRZ mode */
    uint32_t i;
    uint32_t rx_bist_lock = 0;
    int poll_result;
    uint32_t params[24][4] = {
        {0,1,2,3},
        {0,1,3,2},
        {0,2,1,3},
        {0,2,3,1},
        {0,3,1,2},
        {0,3,2,1},
        {1,0,2,3},
        {1,0,3,2},
        {1,2,0,3},
        {1,2,3,0},
        {1,3,0,2},
        {1,3,2,0},
        {2,0,1,3},
        {2,0,3,1},
        {2,1,0,3},
        {2,1,3,0},
        {2,3,0,1},
        {2,3,1,0},
        {3,0,1,2},
        {3,0,2,1},
        {3,1,0,2},
        {3,1,2,0},
        {3,2,0,1},
        {3,2,1,0}
    };


    if (npam4_nrz){ /* nrz */
        for (i = 0; i<=1; i++) {
            CHECK(pmd_write_field(mss, RX_DEMAPPER_ADDR, RX_DEMAPPER_INVERT_ENABLE_A_MASK, RX_DEMAPPER_INVERT_ENABLE_A_OFFSET, i));

            poll_result = pmd_poll_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_LOCKED_NT_MASK, RX_DATABIST_TOP_RDREG1_LOCKED_NT_OFFSET, 1, timeout_us);

            if (poll_result == 0) {
                USR_PRINTF(("RX_DEMAPPER_INVERT_ENABLE_A_OFFSET = %d\n",i));
                rx_bist_lock = 1;
                break;
            }
        }
    } else {
        CHECK(pmd_write_field(mss, RX_CNTRL_REG2_ADDR, RX_CNTRL_REG2_RX_GRAY_ENA_OVR_NT_MASK, RX_CNTRL_REG2_RX_GRAY_ENA_OVR_NT_OFFSET, 1));
        for (i = 0; i<=23; i++){
            CHECK(pmd_write_field(mss, RX_DEMAPPER_ADDR, RX_DEMAPPER_EL3_VAL_NT_MASK, RX_DEMAPPER_EL3_VAL_NT_OFFSET, params[i][0]));
            CHECK(pmd_write_field(mss, RX_DEMAPPER_ADDR, RX_DEMAPPER_EL1_VAL_NT_MASK, RX_DEMAPPER_EL1_VAL_NT_OFFSET, params[i][1]));
            CHECK(pmd_write_field(mss, RX_DEMAPPER_ADDR, RX_DEMAPPER_EH1_VAL_NT_MASK, RX_DEMAPPER_EH1_VAL_NT_OFFSET, params[i][2]));
            CHECK(pmd_write_field(mss, RX_DEMAPPER_ADDR, RX_DEMAPPER_EH3_VAL_NT_MASK, RX_DEMAPPER_EH3_VAL_NT_OFFSET, params[i][3]));
            poll_result = pmd_poll_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_LOCKED_NT_MASK, RX_DATABIST_TOP_RDREG1_LOCKED_NT_OFFSET, 1, timeout_us);

            if (poll_result == 0) {
                USR_PRINTF(("RX_DEMAPPER_EL3 = %d ; RX_DEMAPPER_EL1 = %d ; RX_DEMAPPER_EH1 = %d ;RX_DEMAPPER_EH3 = %d ;\n",params[i][0], params[i][1], params[i][2], params[i][3]));
                rx_bist_lock = 1;
                break;
            }
        }
    }

    if (rx_bist_lock ==1 ){
        USR_PRINTF(("RX BIST locked after sweeping demapper\n"));
        return JUPITER_ERR_CODE_NONE;
    } else {
        USR_PRINTF(("ERROR: RXBIST did not lock after sweeping demapper\n"));
        return JUPITER_ERR_CODE_FUNC_FAILURE; /* no bist lock, couldn't find a mapping */
    }
}


/* UC APIs */

/**
 * Default method for loading firmware and pointers. Needs to take in an array
 * that contains both pointers and SRAM values, or a seperate structure
 * containing pointer info, and then also the arry to the SRAM instructions.
 * 
 * It will load the pointers to the CSRs via the PMI interface and the FW to
 * the SRAM via the PRAM interface.
 * 
 */
int jupiter_pmd_uc_ucode_load(mss_access_t *mss, uint32_t (*ucode_arr)[2], uint32_t ucode_len){
    uint32_t i;
    /* Always clear the CRC before loading, note: CRC only applies when loading */
    /* through the PRAM interface */
    CHECK(jupiter_pmd_uc_ucode_crc_clear(mss));
    
    USR_PRINTF(("Loading FW\n"));
    for (i = 0; i<ucode_len; i++){
        CHECK(pmd_write_addr(mss, ucode_arr[i][0], ucode_arr[i][1])); /* addr, val */
    }
    return JUPITER_ERR_CODE_NONE;
}

/*
int c_load_hexfile(mss_access_t *mss, char * fileName) {
    int pass = 0;
    
    FILE* file = fopen(fileName, "r");
    char line[256];
    uint32_t jupiter_ucode[JUPITER_SRAM_SIZE][2];
    uint32_t i = 0;
    uint32_t addr;
    uint32_t value;
    while (fgets(line, sizeof(line), file)) {
        char * token = strtok(line, ","); 
        sscanf(token, "%x", &addr); 
        token = strtok(NULL, ","); 
        sscanf(token, "%x", &value); 
        jupiter_ucode[i][0] = addr;
        jupiter_ucode[i][1] = value;
        i++;
        continue;
    }

    fclose(file);

    USR_PRINTF(("Calling jupiter_pmd_uc_ucode_load\n")); 
    pass += jupiter_pmd_uc_ucode_load(mss, jupiter_ucode, i);
    return pass;

}
*/


int jupiter_pmd_ock_pcs_clk_b_gate_set(mss_access_t *mss, uint32_t enable) {
    CHECK(pmd_write_field(mss, SWITCHCLK_DBE_CMN_ADDR, SWITCHCLK_DBE_CMN_CLKB_EN_NT_MASK, SWITCHCLK_DBE_CMN_CLKB_EN_NT_OFFSET, enable ));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_ock_pcs_clk_b_gate_get(mss_access_t *mss, uint32_t *enable) {
    CHECK(pmd_read_field(mss, SWITCHCLK_DBE_CMN_ADDR, SWITCHCLK_DBE_CMN_CLKB_EN_NT_MASK, SWITCHCLK_DBE_CMN_CLKB_EN_NT_OFFSET, enable ));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_disable_pin_override_set(mss_access_t *mss, uint32_t override_enable){
    /* 1 to enable override, 0 to disable override */
    CHECK(pmd_write_field(mss, TX_OVRD_ENABLE_ADDR, TX_OVRD_ENABLE_ELECIDLE_A_MASK, TX_OVRD_ENABLE_ELECIDLE_A_OFFSET, override_enable));
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_TXCHNG_PEDGE_A_MASK, TX_OVRD_REG1_TXCHNG_PEDGE_A_OFFSET, 1));
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_TXCHNG_PEDGE_A_MASK, TX_OVRD_REG1_TXCHNG_PEDGE_A_OFFSET, 0));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_preset_get(mss_access_t *mss, jupiter_state_rate_t *tx_rate_preset, jupiter_training_mode_t *tx_training_mode){
    uint32_t rp;
    uint32_t tm;
    CHECK(pmd_read_field(mss, TX_STAT_ADDR, TX_STAT_ICTL_TX_STATE_RATE_NT_MASK, TX_STAT_ICTL_TX_STATE_RATE_NT_OFFSET, &rp));
    CHECK(pmd_read_field(mss, TX_STAT_ADDR, TX_STAT_ICTL_TX_TRAINING_MODE_NT_MASK, TX_STAT_ICTL_TX_TRAINING_MODE_NT_OFFSET, &tm));
    *tx_rate_preset = (jupiter_state_rate_t) rp;
    *tx_training_mode = (jupiter_training_mode_t) tm;
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_preset_get(mss_access_t *mss, jupiter_state_rate_t *rx_rate_preset, jupiter_training_mode_t *rx_training_mode){
    uint32_t rp;
    uint32_t tm;
    CHECK(pmd_read_field(mss, RX_STAT_ADDR, RX_STAT_ICTL_RX_STATE_RATE_NT_MASK, RX_STAT_ICTL_RX_STATE_RATE_NT_OFFSET, &rp));
    CHECK(pmd_read_field(mss, RX_STAT_ADDR, RX_STAT_ICTL_RX_TRAINING_MODE_NT_MASK, RX_STAT_ICTL_RX_TRAINING_MODE_NT_OFFSET, &tm));
    *rx_rate_preset = (jupiter_state_rate_t) rp;
    *rx_training_mode = (jupiter_training_mode_t) tm;
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_pll_lock_get(mss_access_t *mss, uint32_t *pll_lock){
    uint32_t check_done = 0;
    uint32_t check_status = 0;
    CHECK(pmd_write_field(mss, LCPLL_CHECK_ADDR, LCPLL_CHECK_START_A_MASK, LCPLL_CHECK_START_A_OFFSET, 1));
    USR_USLEEP(10);
    CHECK(pmd_read_field(mss, LCPLL_CHECK_RDREG_ADDR, LCPLL_CHECK_RDREG_DONE_A_MASK, LCPLL_CHECK_RDREG_DONE_A_OFFSET, &check_done));
    if (check_done) {
        CHECK(pmd_read_field(mss, LCPLL_CHECK_RDREG_ADDR, LCPLL_CHECK_RDREG_STAT_NT_MASK, LCPLL_CHECK_RDREG_STAT_NT_OFFSET, &check_status));
    }

    if (!check_done || !check_status) {
        *pll_lock = 0;
    } else {
        *pll_lock = 1;
    }
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_pll_pwrdn_get(mss_access_t *mss, jupiter_pll_pstatus_t *pll_pwrdn_status){
    uint32_t pll_lock;
    uint32_t cmniffsm_state;
    jupiter_pll_pstatus_t s;
    CHECK(pmd_read_field(mss, SEQ_CNTRL_CMN_ADDR, SEQ_CNTRL_CMN_LCPLL_LOCK_A_MASK, SEQ_CNTRL_CMN_LCPLL_LOCK_A_OFFSET, &pll_lock));
    CHECK(pmd_read_field(mss, CMNIFFSM_STAT_ADDR, CMNIFFSM_STAT_CMNIFFSM_STATE_MASK, CMNIFFSM_STAT_CMNIFFSM_STATE_OFFSET, &cmniffsm_state));
    if (pll_lock == 0 && cmniffsm_state == 0) {
        s = JUPITER_PWR_DOWN;
    } else if (pll_lock == 1 && cmniffsm_state == 0) {
        s = JUPITER_PWR_UP;
    } else {
        s = JUPITER_ST_CHNG;
    }
    *pll_pwrdn_status = s;
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_lane_map_set(mss_access_t *mss, uint32_t const *tx_lane_map_arr, uint32_t const *rx_lane_map_arr){

    /* Remapping lane0 */
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE0_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE0_TX_NT_OFFSET, tx_lane_map_arr[0]));
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG3_ADDR, LOGICAL_MAPPING_REG3_LANE0_RX_NT_MASK, LOGICAL_MAPPING_REG3_LANE0_RX_NT_OFFSET, rx_lane_map_arr[0]));

    /* Remapping lane1 */
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE1_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE1_TX_NT_OFFSET, tx_lane_map_arr[1]));
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG3_ADDR, LOGICAL_MAPPING_REG3_LANE1_RX_NT_MASK, LOGICAL_MAPPING_REG3_LANE1_RX_NT_OFFSET, rx_lane_map_arr[1]));

    /* Remapping lane2 */
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE2_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE2_TX_NT_OFFSET, tx_lane_map_arr[2]));
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE2_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE2_RX_NT_OFFSET, rx_lane_map_arr[2]));

    /* Remapping lane3 */
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE3_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE3_TX_NT_OFFSET, tx_lane_map_arr[3]));
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE3_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE3_RX_NT_OFFSET, rx_lane_map_arr[3]));

    /* Remapping lane4 */
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE4_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE4_TX_NT_OFFSET, tx_lane_map_arr[4]));
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE4_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE4_RX_NT_OFFSET, rx_lane_map_arr[4]));

    /* Remapping lane5 */
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE5_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE5_TX_NT_OFFSET, tx_lane_map_arr[5]));
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE5_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE5_RX_NT_OFFSET, rx_lane_map_arr[5]));

    /* Remapping lane6 */
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG2_ADDR, LOGICAL_MAPPING_REG2_LANE6_TX_NT_MASK, LOGICAL_MAPPING_REG2_LANE6_TX_NT_OFFSET, tx_lane_map_arr[6]));
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE6_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE6_RX_NT_OFFSET, rx_lane_map_arr[6]));

    /* Remapping lane7 */
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG2_ADDR, LOGICAL_MAPPING_REG2_LANE7_TX_NT_MASK, LOGICAL_MAPPING_REG2_LANE7_TX_NT_OFFSET, tx_lane_map_arr[7]));
    CHECK(pmd_write_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE7_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE7_RX_NT_OFFSET, rx_lane_map_arr[7]));

    pmd_set_lane(mss,0);
    CHECK(pmd_write_field(mss, ETH_MAP_ADDR, ETH_MAP_RXSEL_NT_MASK, ETH_MAP_RXSEL_NT_OFFSET, rx_lane_map_arr[0]-1));
    if (tx_lane_map_arr[0] == rx_lane_map_arr[0]) {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 1));
    }

    pmd_set_lane(mss,1);
    CHECK(pmd_write_field(mss, ETH_MAP_ADDR, ETH_MAP_RXSEL_NT_MASK, ETH_MAP_RXSEL_NT_OFFSET, rx_lane_map_arr[1]-1));
    if (tx_lane_map_arr[1] == rx_lane_map_arr[1]) {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 1));
    }

    pmd_set_lane(mss,2);
    CHECK(pmd_write_field(mss, ETH_MAP_ADDR, ETH_MAP_RXSEL_NT_MASK, ETH_MAP_RXSEL_NT_OFFSET, rx_lane_map_arr[2]-1));
    if (tx_lane_map_arr[2] == rx_lane_map_arr[2]) {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 1));
    }

    pmd_set_lane(mss,3);
    CHECK(pmd_write_field(mss, ETH_MAP_ADDR, ETH_MAP_RXSEL_NT_MASK, ETH_MAP_RXSEL_NT_OFFSET, rx_lane_map_arr[3]-1));
    if (tx_lane_map_arr[3] == rx_lane_map_arr[3]) {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 1));
    }

    pmd_set_lane(mss,4);
    CHECK(pmd_write_field(mss, ETH_MAP_ADDR, ETH_MAP_RXSEL_NT_MASK, ETH_MAP_RXSEL_NT_OFFSET, rx_lane_map_arr[4]-1));
    if (tx_lane_map_arr[4] == rx_lane_map_arr[4]) {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 1));
    }

    pmd_set_lane(mss,5);
    CHECK(pmd_write_field(mss, ETH_MAP_ADDR, ETH_MAP_RXSEL_NT_MASK, ETH_MAP_RXSEL_NT_OFFSET, rx_lane_map_arr[5]-1));
    if (tx_lane_map_arr[5] == rx_lane_map_arr[5]) {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 1));
    }

    pmd_set_lane(mss,6);
    CHECK(pmd_write_field(mss, ETH_MAP_ADDR, ETH_MAP_RXSEL_NT_MASK, ETH_MAP_RXSEL_NT_OFFSET, rx_lane_map_arr[6]-1));
    if (tx_lane_map_arr[6] == rx_lane_map_arr[6]) {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 1));
    }

    pmd_set_lane(mss,7);
    CHECK(pmd_write_field(mss, ETH_MAP_ADDR, ETH_MAP_RXSEL_NT_MASK, ETH_MAP_RXSEL_NT_OFFSET, rx_lane_map_arr[7]-1));
    if (tx_lane_map_arr[7] == rx_lane_map_arr[7]) {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 0));
    } else {
        CHECK(pmd_write_field(mss, ETH_ANLT_CTRL_ADDR, ETH_ANLT_CTRL_MAP_EN_NT_MASK, ETH_ANLT_CTRL_MAP_EN_NT_OFFSET, 1));
    }

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_lane_map_get(mss_access_t *mss, uint32_t *tx_lane_map_arr, uint32_t *rx_lane_map_arr){
    /* TX LANES */
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE0_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE0_TX_NT_OFFSET, &tx_lane_map_arr[0]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE1_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE1_TX_NT_OFFSET, &tx_lane_map_arr[1]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE2_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE2_TX_NT_OFFSET, &tx_lane_map_arr[2]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE3_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE3_TX_NT_OFFSET, &tx_lane_map_arr[3]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE4_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE4_TX_NT_OFFSET, &tx_lane_map_arr[4]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG1_ADDR, LOGICAL_MAPPING_REG1_LANE5_TX_NT_MASK, LOGICAL_MAPPING_REG1_LANE5_TX_NT_OFFSET, &tx_lane_map_arr[5]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG2_ADDR, LOGICAL_MAPPING_REG2_LANE6_TX_NT_MASK, LOGICAL_MAPPING_REG2_LANE6_TX_NT_OFFSET, &tx_lane_map_arr[6]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG2_ADDR, LOGICAL_MAPPING_REG2_LANE7_TX_NT_MASK, LOGICAL_MAPPING_REG2_LANE7_TX_NT_OFFSET, &tx_lane_map_arr[7]));
    /* RX LANES */
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG3_ADDR, LOGICAL_MAPPING_REG3_LANE0_RX_NT_MASK, LOGICAL_MAPPING_REG3_LANE0_RX_NT_OFFSET, &rx_lane_map_arr[0]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG3_ADDR, LOGICAL_MAPPING_REG3_LANE1_RX_NT_MASK, LOGICAL_MAPPING_REG3_LANE1_RX_NT_OFFSET, &rx_lane_map_arr[1]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE2_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE2_RX_NT_OFFSET, &rx_lane_map_arr[2]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE3_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE3_RX_NT_OFFSET, &rx_lane_map_arr[3]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE4_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE4_RX_NT_OFFSET, &rx_lane_map_arr[4]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE5_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE5_RX_NT_OFFSET, &rx_lane_map_arr[5]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE6_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE6_RX_NT_OFFSET, &rx_lane_map_arr[6]));
    CHECK(pmd_read_field(mss, LOGICAL_MAPPING_REG4_ADDR, LOGICAL_MAPPING_REG4_LANE7_RX_NT_MASK, LOGICAL_MAPPING_REG4_LANE7_RX_NT_OFFSET, &rx_lane_map_arr[7]));
    return JUPITER_ERR_CODE_NONE;
}

/**
 * Clear the CRC
 *
 * Only needs to be run before loading the SRAM.
 */
int jupiter_pmd_uc_ucode_crc_clear(mss_access_t *mss){
    /* CRC is always on */
    CHECK(pmd_write_field(mss, PRAM_ADDR, PRAM_CRC_CLR_A_MASK, PRAM_CRC_CLR_A_OFFSET, 1));
    CHECK(pmd_write_field(mss, PRAM_ADDR, PRAM_CRC_CLR_A_MASK, PRAM_CRC_CLR_A_OFFSET, 0));
    return JUPITER_ERR_CODE_NONE;
}

/**
 *  Loads firmware via PMI interface only
 */
int jupiter_pmd_uc_ucode_pmi_load(mss_access_t *mss, uint32_t **ucode_arr, uint32_t ucode_len){
    uint32_t i;
    /* Use PMI for everything */

    for (i = 0; i<ucode_len; i++){
        CHECK(pmd_write_addr(mss, ucode_arr[i][0], ucode_arr[i][1])); /* addr, val */
    }
    return JUPITER_ERR_CODE_NONE;
}

/**
 *  Loads firmware via MDIO interface only
 */
int jupiter_pmd_uc_ucode_mdio_load(mss_access_t *mss, uint32_t **ucode_arr, uint32_t ucode_len){
    uint32_t i;
    /* CSR Pointers and SRAM get loaded through the MDIO interface */
    for (i = 0; i<ucode_len; i++){
        CHECK(pmd_write_addr(mss, ucode_arr[i][0], ucode_arr[i][1])); /* addr, val */
    }
    return JUPITER_ERR_CODE_NONE;
}

/**
 * Calculate the CRC of the SRAM, pass that "expected_crc" to this function
 * and it will read and return the value of the hardware calculated CRC.
 */
int jupiter_pmd_uc_ucode_crc_verify(mss_access_t *mss, uint32_t expected_crc){
    uint32_t rdcrc;
    CHECK(pmd_read_field(mss, PRAM_RDREG_ADDR, PRAM_RDREG_CRC_MASK, PRAM_RDREG_CRC_OFFSET, &rdcrc));
    if (expected_crc == rdcrc){
        return JUPITER_ERR_CODE_NONE;
    } else {
        return JUPITER_ERR_CODE_FUNC_FAILURE;
    }
}

/**
 * This dump function will print all the diagnostic information about 
 * the Cores Master FSM. This includes all pointer locations where all
 * the FSM routines are stored. This API uses a macro that is to be
 * user defined.
 */
int jupiter_pmd_uc_diag_reg_dump(mss_access_t *mss, jupiter_uc_diag_regs_t *uc_diag){

    /* RX */
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_RATE_CUR_MASK, RXMFSM_STAT_RXMFSM_RATE_CUR_OFFSET, &(uc_diag->rxmfsm_rate_cur) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_RATE_NEW_MASK, RXMFSM_STAT_RXMFSM_RATE_NEW_OFFSET, &(uc_diag->rxmfsm_rate_new) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_WIDTH_CUR_MASK, RXMFSM_STAT_RXMFSM_WIDTH_CUR_OFFSET, &(uc_diag->rxmfsm_width_cur) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_WIDTH_NEW_MASK, RXMFSM_STAT_RXMFSM_WIDTH_NEW_OFFSET, &(uc_diag->rxmfsm_width_new) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_RXDISABLE_MASK, RXMFSM_STAT_RXMFSM_RXDISABLE_OFFSET, &(uc_diag->rxmfsm_rxdisable) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_REQ_MASK, RXMFSM_STAT_RXMFSM_REQ_OFFSET, &(uc_diag->rxmfsm_req) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_POWER_CUR_MASK, RXMFSM_STAT_RXMFSM_POWER_CUR_OFFSET, &(uc_diag->rxmfsm_power_cur) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_POWER_NEW_MASK, RXMFSM_STAT_RXMFSM_POWER_NEW_OFFSET, &(uc_diag->rxmfsm_power_new) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_PAM_CUR_MASK, RXMFSM_STAT_RXMFSM_PAM_CUR_OFFSET, &(uc_diag->rxmfsm_pam_cur) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_PAM_NEW_MASK, RXMFSM_STAT_RXMFSM_PAM_NEW_OFFSET, &(uc_diag->rxmfsm_pam_new) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_PAM_CTRL_CUR_MASK, RXMFSM_STAT_RXMFSM_PAM_CTRL_CUR_OFFSET, &(uc_diag->rxmfsm_pam_ctrl_cur) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_PAM_CTRL_NEW_MASK, RXMFSM_STAT_RXMFSM_PAM_CTRL_NEW_OFFSET, &(uc_diag->rxmfsm_pam_ctrl_new) ));
    CHECK(pmd_read_field(mss, RXMFSM_STAT_ADDR, RXMFSM_STAT_RXMFSM_INSTR_NUM_MASK, RXMFSM_STAT_RXMFSM_INSTR_NUM_OFFSET, &(uc_diag->rxmfsm_instr_num) ));
    CHECK(pmd_read_field(mss, RXMFSM_STATE_ADDR, RXMFSM_STATE_RXMFSM_STATE_MASK, RXMFSM_STATE_RXMFSM_STATE_OFFSET, &(uc_diag->rxmfsm_state) ));
    CHECK(pmd_read_field(mss, RXIFFSM_STAT_ADDR, RXIFFSM_STAT_RXIFFSM_STATE_MASK, RXIFFSM_STAT_RXIFFSM_STATE_OFFSET, &(uc_diag->rxiffsm_state) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG1_ADDR, RXMFSM_LOG_RDREG1_LOG0_MASK, RXMFSM_LOG_RDREG1_LOG0_OFFSET, &(uc_diag->rx_log0) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG1_ADDR, RXMFSM_LOG_RDREG1_LOG1_MASK, RXMFSM_LOG_RDREG1_LOG1_OFFSET, &(uc_diag->rx_log1) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG2_ADDR, RXMFSM_LOG_RDREG2_LOG2_MASK, RXMFSM_LOG_RDREG2_LOG2_OFFSET, &(uc_diag->rx_log2) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG2_ADDR, RXMFSM_LOG_RDREG2_LOG3_MASK, RXMFSM_LOG_RDREG2_LOG3_OFFSET, &(uc_diag->rx_log3) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG3_ADDR, RXMFSM_LOG_RDREG3_LOG4_MASK, RXMFSM_LOG_RDREG3_LOG4_OFFSET, &(uc_diag->rx_log4) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG3_ADDR, RXMFSM_LOG_RDREG3_LOG5_MASK, RXMFSM_LOG_RDREG3_LOG5_OFFSET, &(uc_diag->rx_log5) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG4_ADDR, RXMFSM_LOG_RDREG4_LOG6_MASK, RXMFSM_LOG_RDREG4_LOG6_OFFSET, &(uc_diag->rx_log6) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG4_ADDR, RXMFSM_LOG_RDREG4_LOG7_MASK, RXMFSM_LOG_RDREG4_LOG7_OFFSET, &(uc_diag->rx_log7) ));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_RDREG5_ADDR, RXMFSM_LOG_RDREG5_LOG8_MASK, RXMFSM_LOG_RDREG5_LOG8_OFFSET, &(uc_diag->rx_log8) ));
    /* TX */
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_RATE_CUR_MASK, TXMFSM_STAT_TXMFSM_RATE_CUR_OFFSET, &(uc_diag->txmfsm_rate_cur) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_RATE_NEW_MASK, TXMFSM_STAT_TXMFSM_RATE_NEW_OFFSET, &(uc_diag->txmfsm_rate_new) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_WIDTH_CUR_MASK, TXMFSM_STAT_TXMFSM_WIDTH_CUR_OFFSET, &(uc_diag->txmfsm_width_cur) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_WIDTH_NEW_MASK, TXMFSM_STAT_TXMFSM_WIDTH_NEW_OFFSET, &(uc_diag->txmfsm_width_new) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_POWER_CUR_MASK, TXMFSM_STAT_TXMFSM_POWER_CUR_OFFSET, &(uc_diag->txmfsm_power_cur) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_POWER_NEW_MASK, TXMFSM_STAT_TXMFSM_POWER_NEW_OFFSET, &(uc_diag->txmfsm_power_new) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_PAM_CUR_MASK, TXMFSM_STAT_TXMFSM_PAM_CUR_OFFSET, &(uc_diag->txmfsm_pam_cur) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_PAM_NEW_MASK, TXMFSM_STAT_TXMFSM_PAM_NEW_OFFSET, &(uc_diag->txmfsm_pam_new) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_INSTR_NUM_MASK, TXMFSM_STAT_TXMFSM_INSTR_NUM_OFFSET, &(uc_diag->txmfsm_instr_num) ));
    CHECK(pmd_read_field(mss, TXMFSM_STAT_ADDR, TXMFSM_STAT_TXMFSM_REQ_MASK, TXMFSM_STAT_TXMFSM_REQ_OFFSET, &(uc_diag->txmfsm_req) ));
    CHECK(pmd_read_field(mss, TXMFSM_STATE_ADDR, TXMFSM_STATE_TXMFSM_STATE_MASK, TXMFSM_STATE_TXMFSM_STATE_OFFSET, &(uc_diag->txmfsm_state) ));
    CHECK(pmd_read_field(mss, TXIFFSM_STAT_ADDR, TXIFFSM_STAT_TXIFFSM_STATE_MASK, TXIFFSM_STAT_TXIFFSM_STATE_OFFSET, &(uc_diag->txiffsm_state) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG1_ADDR, TXMFSM_LOG_RDREG1_LOG0_MASK, TXMFSM_LOG_RDREG1_LOG0_OFFSET, &(uc_diag->tx_log0) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG1_ADDR, TXMFSM_LOG_RDREG1_LOG1_MASK, TXMFSM_LOG_RDREG1_LOG1_OFFSET, &(uc_diag->tx_log1) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG2_ADDR, TXMFSM_LOG_RDREG2_LOG2_MASK, TXMFSM_LOG_RDREG2_LOG2_OFFSET, &(uc_diag->tx_log2) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG2_ADDR, TXMFSM_LOG_RDREG2_LOG3_MASK, TXMFSM_LOG_RDREG2_LOG3_OFFSET, &(uc_diag->tx_log3) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG3_ADDR, TXMFSM_LOG_RDREG3_LOG4_MASK, TXMFSM_LOG_RDREG3_LOG4_OFFSET, &(uc_diag->tx_log4) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG3_ADDR, TXMFSM_LOG_RDREG3_LOG5_MASK, TXMFSM_LOG_RDREG3_LOG5_OFFSET, &(uc_diag->tx_log5) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG4_ADDR, TXMFSM_LOG_RDREG4_LOG6_MASK, TXMFSM_LOG_RDREG4_LOG6_OFFSET, &(uc_diag->tx_log6) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG4_ADDR, TXMFSM_LOG_RDREG4_LOG7_MASK, TXMFSM_LOG_RDREG4_LOG7_OFFSET, &(uc_diag->tx_log7) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG5_ADDR, TXMFSM_LOG_RDREG5_LOG8_MASK, TXMFSM_LOG_RDREG5_LOG8_OFFSET, &(uc_diag->tx_log8) ));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_RDREG5_ADDR, TXMFSM_LOG_RDREG5_LOG9_MASK, TXMFSM_LOG_RDREG5_LOG9_OFFSET, &(uc_diag->tx_log9) ));
    /* CMN */
    CHECK(pmd_read_field(mss, CMNMFSM_STAT_ADDR, CMNMFSM_STAT_CMNMFSM_RATE_CUR_MASK, CMNMFSM_STAT_CMNMFSM_RATE_CUR_OFFSET, &(uc_diag->cmnmfsm_rate_cur) ));
    CHECK(pmd_read_field(mss, CMNMFSM_STAT_ADDR, CMNMFSM_STAT_CMNMFSM_INSTR_NUM_MASK, CMNMFSM_STAT_CMNMFSM_INSTR_NUM_OFFSET, &(uc_diag->cmnmfsm_instr_num) ));
    CHECK(pmd_read_field(mss, CMNMFSM_STAT_ADDR, CMNMFSM_STAT_CMNMFSM_POWER_CUR_MASK, CMNMFSM_STAT_CMNMFSM_POWER_CUR_OFFSET, &(uc_diag->cmnmfsm_power_cur) ));
    CHECK(pmd_read_field(mss, CMNMFSM_STAT_ADDR, CMNMFSM_STAT_CMNMFSM_POWER_NEW_MASK, CMNMFSM_STAT_CMNMFSM_POWER_NEW_OFFSET, &(uc_diag->cmnmfsm_power_new) ));
    CHECK(pmd_read_field(mss, CMNMFSM_STAT_ADDR, CMNMFSM_STAT_CMNMFSM_RATE_NEW_MASK, CMNMFSM_STAT_CMNMFSM_RATE_NEW_OFFSET, &(uc_diag->cmnmfsm_rate_new) ));
    CHECK(pmd_read_field(mss, CMNMFSM_STAT_ADDR, CMNMFSM_STAT_CMNMFSM_REQ_MASK, CMNMFSM_STAT_CMNMFSM_REQ_OFFSET, &(uc_diag->cmnmfsm_req) ));
    CHECK(pmd_read_field(mss, CMNMFSM_STATE_ADDR, CMNMFSM_STATE_CMNMFSM_STATE_MASK, CMNMFSM_STATE_CMNMFSM_STATE_OFFSET, &(uc_diag->cmnmfsm_state) ));
    CHECK(pmd_read_field(mss, CMNIFFSM_STAT_ADDR, CMNIFFSM_STAT_CMNIFFSM_STATE_MASK, CMNIFFSM_STAT_CMNIFFSM_STATE_OFFSET, &(uc_diag->cmniffsm_state) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG1_ADDR, CMNMFSM_LOG_RDREG1_LOG0_MASK, CMNMFSM_LOG_RDREG1_LOG0_OFFSET, &(uc_diag->cmn_log0) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG1_ADDR, CMNMFSM_LOG_RDREG1_LOG1_MASK, CMNMFSM_LOG_RDREG1_LOG1_OFFSET, &(uc_diag->cmn_log1) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG2_ADDR, CMNMFSM_LOG_RDREG2_LOG2_MASK, CMNMFSM_LOG_RDREG2_LOG2_OFFSET, &(uc_diag->cmn_log2) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG2_ADDR, CMNMFSM_LOG_RDREG2_LOG3_MASK, CMNMFSM_LOG_RDREG2_LOG3_OFFSET, &(uc_diag->cmn_log3) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG3_ADDR, CMNMFSM_LOG_RDREG3_LOG4_MASK, CMNMFSM_LOG_RDREG3_LOG4_OFFSET, &(uc_diag->cmn_log4) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG3_ADDR, CMNMFSM_LOG_RDREG3_LOG5_MASK, CMNMFSM_LOG_RDREG3_LOG5_OFFSET, &(uc_diag->cmn_log5) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG4_ADDR, CMNMFSM_LOG_RDREG4_LOG6_MASK, CMNMFSM_LOG_RDREG4_LOG6_OFFSET, &(uc_diag->cmn_log6) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG4_ADDR, CMNMFSM_LOG_RDREG4_LOG7_MASK, CMNMFSM_LOG_RDREG4_LOG7_OFFSET, &(uc_diag->cmn_log7) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG5_ADDR, CMNMFSM_LOG_RDREG5_LOG8_MASK, CMNMFSM_LOG_RDREG5_LOG8_OFFSET, &(uc_diag->cmn_log8) ));
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_RDREG5_ADDR, CMNMFSM_LOG_RDREG5_LOG9_MASK, CMNMFSM_LOG_RDREG5_LOG9_OFFSET, &(uc_diag->cmn_log9) ));

    USR_PRINTF(("CMN MFSM DEBUG INFO:\n"));
    USR_PRINTF(("CMN_CMNMFSM_STAT_CMNMFSM_REQ: 0x%x", uc_diag->cmnmfsm_req));
    USR_PRINTF(("CMN_CMNMFSM_STAT_CMNMFSM_POWER_CUR: 0x%x", uc_diag->cmnmfsm_power_cur));
    USR_PRINTF(("CMN_CMNMFSM_STAT_CMNMFSM_POWER_NEW: 0x%x", uc_diag->cmnmfsm_power_new));
    USR_PRINTF(("CMN_CMNMFSM_STAT_CMNMFSM_RATE_CUR: 0x%x", uc_diag->cmnmfsm_rate_cur));
    USR_PRINTF(("CMN_CMNMFSM_STAT_CMNMFSM_RATE_NEW: 0x%x", uc_diag->cmnmfsm_rate_new));
    USR_PRINTF(("CMN_CMNMFSM_STATE_CMNMFSM_STATE: 0x%x", uc_diag->cmnmfsm_state));
    USR_PRINTF(("CMN_CMNIFFSM_STAT_CMNIFFSM_STATE: 0x%x", uc_diag->cmniffsm_state));
    USR_PRINTF(("CMN_CMNMFSM_STAT_CMNMFSM_INSTR_NUM: 0x%x", uc_diag->cmnmfsm_instr_num));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG1_LOG0: 0x%x", uc_diag->cmn_log0));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG1_LOG1: 0x%x", uc_diag->cmn_log1));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG2_LOG2: 0x%x", uc_diag->cmn_log2));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG2_LOG3: 0x%x", uc_diag->cmn_log3));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG3_LOG4: 0x%x", uc_diag->cmn_log4));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG3_LOG5: 0x%x", uc_diag->cmn_log5));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG4_LOG6: 0x%x", uc_diag->cmn_log6));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG4_LOG7: 0x%x", uc_diag->cmn_log7));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG5_LOG8: 0x%x", uc_diag->cmn_log8));
    USR_PRINTF(("CMN_CMNMFSM_LOG_RDREG5_LOG9: 0x%x", uc_diag->cmn_log9));

    USR_PRINTF(("TX MFSM DEBUG INFO:\n"));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_REQ: 0x%x", uc_diag->txmfsm_req));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_POWER_CUR: 0x%x", uc_diag->txmfsm_power_cur));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_POWER_NEW: 0x%x", uc_diag->txmfsm_power_new));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_RATE_CUR: 0x%x", uc_diag->txmfsm_rate_cur));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_RATE_NEW: 0x%x", uc_diag->txmfsm_rate_new));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_WIDTH_CUR: 0x%x", uc_diag->txmfsm_width_cur));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_WIDTH_NEW: 0x%x", uc_diag->txmfsm_width_new));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_PAM_CUR: 0x%x", uc_diag->txmfsm_pam_cur));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_PAM_NEW: 0x%x", uc_diag->txmfsm_pam_new));
    USR_PRINTF(("TXMFSM_STATE_TXMFSM_STATE: 0x%x", uc_diag->txmfsm_state));
    USR_PRINTF(("TXIFFSM_STAT_TXIFFSM_STATE: 0x%x", uc_diag->txiffsm_state));
    USR_PRINTF(("TXMFSM_STAT_TXMFSM_INSTR_NUM: 0x%x", uc_diag->txmfsm_instr_num));
    USR_PRINTF(("TXMFSM_LOG_RDREG1_LOG0: 0x%x", uc_diag->tx_log0));
    USR_PRINTF(("TXMFSM_LOG_RDREG1_LOG1: 0x%x", uc_diag->tx_log1));
    USR_PRINTF(("TXMFSM_LOG_RDREG2_LOG2: 0x%x", uc_diag->tx_log2));
    USR_PRINTF(("TXMFSM_LOG_RDREG2_LOG3: 0x%x", uc_diag->tx_log3));
    USR_PRINTF(("TXMFSM_LOG_RDREG3_LOG4: 0x%x", uc_diag->tx_log4));
    USR_PRINTF(("TXMFSM_LOG_RDREG3_LOG5: 0x%x", uc_diag->tx_log5));
    USR_PRINTF(("TXMFSM_LOG_RDREG4_LOG6: 0x%x", uc_diag->tx_log6));
    USR_PRINTF(("TXMFSM_LOG_RDREG4_LOG7: 0x%x", uc_diag->tx_log7));
    USR_PRINTF(("TXMFSM_LOG_RDREG5_LOG8: 0x%x", uc_diag->tx_log8));
    USR_PRINTF(("TXMFSM_LOG_RDREG5_LOG9: 0x%x", uc_diag->tx_log9));

    USR_PRINTF(("RX MFSM DEBUG INFO:\n"));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_REQ: 0x%x\n", uc_diag->rxmfsm_req));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_POWER_CUR: 0x%x\n", uc_diag->rxmfsm_power_cur));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_POWER_NEW: 0x%x\n", uc_diag->rxmfsm_power_new));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_RATE_CUR: 0x%x\n", uc_diag->rxmfsm_rate_cur));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_RATE_NEW: 0x%x\n", uc_diag->rxmfsm_rate_new));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_WIDTH_CUR: 0x%x\n", uc_diag->rxmfsm_width_cur));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_WIDTH_NEW: 0x%x\n", uc_diag->rxmfsm_width_new));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_PAM_CUR: 0x%x\n", uc_diag->rxmfsm_pam_cur));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_PAM_NEW: 0x%x\n", uc_diag->rxmfsm_pam_new));
    USR_PRINTF(("RXMFSM_STATE_RXMFSM_STATE: 0x%x\n", uc_diag->rxmfsm_state));
    USR_PRINTF(("RXIFFSM_STAT_RXIFFSM_STATE: 0x%x\n", uc_diag->rxiffsm_state));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_INSTR_NUM: 0x%x\n", uc_diag->rxmfsm_instr_num));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_RXDISABLE: 0x%x\n", uc_diag->rxmfsm_rxdisable));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_PAM_CTRL_CUR: 0x%x\n", uc_diag->rxmfsm_pam_ctrl_cur));
    USR_PRINTF(("RXMFSM_STAT_RXMFSM_PAM_CTRL_NEW: 0x%x\n", uc_diag->rxmfsm_pam_ctrl_new));
    USR_PRINTF(("RXMFSM_LOG_RDREG1_LOG0: 0x%x\n", uc_diag->rx_log0));
    USR_PRINTF(("RXMFSM_LOG_RDREG1_LOG1: 0x%x\n", uc_diag->rx_log1));
    USR_PRINTF(("RXMFSM_LOG_RDREG2_LOG2: 0x%x\n", uc_diag->rx_log2));
    USR_PRINTF(("RXMFSM_LOG_RDREG2_LOG3: 0x%x\n", uc_diag->rx_log3));
    USR_PRINTF(("RXMFSM_LOG_RDREG3_LOG4: 0x%x\n", uc_diag->rx_log4));
    USR_PRINTF(("RXMFSM_LOG_RDREG3_LOG5: 0x%x\n", uc_diag->rx_log5));
    USR_PRINTF(("RXMFSM_LOG_RDREG4_LOG6: 0x%x\n", uc_diag->rx_log6));
    USR_PRINTF(("RXMFSM_LOG_RDREG4_LOG7: 0x%x\n", uc_diag->rx_log7));
    USR_PRINTF(("RXMFSM_LOG_RDREG5_LOG8: 0x%x\n", uc_diag->rx_log8));

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_uc_diag_logging_en_set(mss_access_t *mss, uint32_t uc_log_cmn_en, uint32_t uc_log_tx_en, uint32_t uc_log_rx_en){
    CHECK(pmd_write_field(mss, CMNMFSM_LOG_CTRL_ADDR, CMNMFSM_LOG_CTRL_EN_MASK, CMNMFSM_LOG_CTRL_EN_OFFSET, uc_log_cmn_en));
    CHECK(pmd_write_field(mss, TXMFSM_LOG_CTRL_ADDR, TXMFSM_LOG_CTRL_EN_MASK, TXMFSM_LOG_CTRL_EN_OFFSET, uc_log_tx_en));
    CHECK(pmd_write_field(mss, RXMFSM_LOG_CTRL_ADDR, RXMFSM_LOG_CTRL_EN_MASK, RXMFSM_LOG_CTRL_EN_OFFSET, uc_log_rx_en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_uc_diag_logging_en_get(mss_access_t *mss, uint32_t *uc_log_cmn_en, uint32_t *uc_log_tx_en, uint32_t *uc_log_rx_en){
    CHECK(pmd_read_field(mss, CMNMFSM_LOG_CTRL_ADDR, CMNMFSM_LOG_CTRL_EN_MASK, CMNMFSM_LOG_CTRL_EN_OFFSET, uc_log_cmn_en));
    CHECK(pmd_read_field(mss, TXMFSM_LOG_CTRL_ADDR, TXMFSM_LOG_CTRL_EN_MASK, TXMFSM_LOG_CTRL_EN_OFFSET, uc_log_tx_en));
    CHECK(pmd_read_field(mss, RXMFSM_LOG_CTRL_ADDR, RXMFSM_LOG_CTRL_EN_MASK, RXMFSM_LOG_CTRL_EN_OFFSET, uc_log_rx_en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_cmn_pstate_set(mss_access_t *mss, uint32_t value){
    CHECK(pmd_write_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_PCLK_STATE_POWER_A_MASK, CMN_OVRD_ICTL_PCLK_STATE_POWER_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_cmn_pstate_get(mss_access_t *mss, uint32_t *value){
    CHECK(pmd_read_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_PCLK_STATE_POWER_A_MASK, CMN_OVRD_ICTL_PCLK_STATE_POWER_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}


int jupiter_pmd_iso_tx_reset_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_IRST_TX_BA_MASK, TX_OVRD_REG1_IRST_TX_BA_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_reset_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_IRST_TX_BA_MASK, TX_OVRD_REG1_IRST_TX_BA_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_reset_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_IRST_RX_BA_MASK, RX_OVRD_REG1_IRST_RX_BA_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_reset_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_IRST_RX_BA_MASK, RX_OVRD_REG1_IRST_RX_BA_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_rate_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_STATE_RATE_A_MASK, TX_OVRD_REG1_ICTL_TX_STATE_RATE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_rate_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_STATE_RATE_A_MASK, TX_OVRD_REG1_ICTL_TX_STATE_RATE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_rate_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_STATE_RATE_A_MASK, RX_OVRD_REG1_ICTL_RX_STATE_RATE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_rate_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_STATE_RATE_A_MASK, RX_OVRD_REG1_ICTL_RX_STATE_RATE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_pstate_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_STATE_POWER_A_MASK, TX_OVRD_REG1_ICTL_TX_STATE_POWER_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_pstate_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_STATE_POWER_A_MASK, TX_OVRD_REG1_ICTL_TX_STATE_POWER_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_pstate_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_STATE_POWER_A_MASK, RX_OVRD_REG1_ICTL_RX_STATE_POWER_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_pstate_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_STATE_POWER_A_MASK, RX_OVRD_REG1_ICTL_RX_STATE_POWER_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_width_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_STATE_WIDTH_A_MASK, TX_OVRD_REG1_ICTL_TX_STATE_WIDTH_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_width_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_STATE_WIDTH_A_MASK, TX_OVRD_REG1_ICTL_TX_STATE_WIDTH_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_width_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_STATE_WIDTH_A_MASK, RX_OVRD_REG1_ICTL_RX_STATE_WIDTH_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_width_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_STATE_WIDTH_A_MASK, RX_OVRD_REG1_ICTL_RX_STATE_WIDTH_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_training_mode_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_TRAINING_MODE_A_MASK, TX_OVRD_REG1_ICTL_TX_TRAINING_MODE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_tx_training_mode_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_TRAINING_MODE_A_MASK, TX_OVRD_REG1_ICTL_TX_TRAINING_MODE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_training_mode_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_TRAINING_MODE_A_MASK, RX_OVRD_REG1_ICTL_RX_TRAINING_MODE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_rx_training_mode_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_TRAINING_MODE_A_MASK, RX_OVRD_REG1_ICTL_RX_TRAINING_MODE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}


int jupiter_pmd_isolate_cmn_set(mss_access_t *mss, uint32_t en) {
    CHECK(pmd_write_field(mss, CMN_OVRD_ADDR, CMN_OVRD_EXTRA_RESET_B_MASK, CMN_OVRD_EXTRA_RESET_B_OFFSET, 0));

    /*Commenting refclk sel as braves refsel will be pin controlled */
    /*CHECK(pmd_write_field(mss, CMN_OVRD_ENABLE_ADDR, CMN_OVRD_ENABLE_REF_A_MASK, CMN_OVRD_ENABLE_REF_A_OFFSET, en)); */

    CHECK(pmd_write_field(mss, CMN_OVRD_ENABLE_ADDR, CMN_OVRD_ENABLE_STATE_A_MASK, CMN_OVRD_ENABLE_STATE_A_OFFSET, en));
    CHECK(pmd_write_field(mss, CMN_OVRD_ENABLE_ADDR, CMN_OVRD_ENABLE_BYPASS_A_MASK, CMN_OVRD_ENABLE_BYPASS_A_OFFSET, en));
    CHECK(pmd_write_field(mss, CMN_OVRD_ADDR, CMN_OVRD_EXTRA_RESET_B_MASK, CMN_OVRD_EXTRA_RESET_B_OFFSET, 1));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_isolate_cmn_get(mss_access_t *mss, uint32_t *en) {
    CHECK(pmd_read_field(mss, CMN_OVRD_ENABLE_ADDR, CMN_OVRD_ENABLE_REF_A_MASK, CMN_OVRD_ENABLE_REF_A_OFFSET, en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_isolate_lane_set(mss_access_t *mss, uint32_t en) {
    CHECK(pmd_write_field(mss, TX_OVRD_REG3_ADDR, TX_OVRD_REG3_EXTRA_RESET_B_MASK, TX_OVRD_REG3_EXTRA_RESET_B_OFFSET, 0));

    CHECK(pmd_write_field(mss, RX_OVRD_ENABLE_ADDR, RX_OVRD_ENABLE_RST_A_MASK, RX_OVRD_ENABLE_RST_A_OFFSET, en));
    CHECK(pmd_write_field(mss, RX_OVRD_ENABLE_ADDR, RX_OVRD_ENABLE_STATE_A_MASK, RX_OVRD_ENABLE_STATE_A_OFFSET, en));
    CHECK(pmd_write_field(mss, RX_OVRD_ENABLE_ADDR, RX_OVRD_ENABLE_RX_BYPASS_A_MASK, RX_OVRD_ENABLE_RX_BYPASS_A_OFFSET, en));
    CHECK(pmd_write_field(mss, RX_OVRD_ENABLE_ADDR, RX_OVRD_ENABLE_LINKEVAL_A_MASK, RX_OVRD_ENABLE_LINKEVAL_A_OFFSET, en));

    CHECK(pmd_write_field(mss, TX_OVRD_ENABLE_ADDR, TX_OVRD_ENABLE_RST_A_MASK, TX_OVRD_ENABLE_RST_A_OFFSET, en));
    CHECK(pmd_write_field(mss, TX_OVRD_ENABLE_ADDR, TX_OVRD_ENABLE_STATE_A_MASK, TX_OVRD_ENABLE_STATE_A_OFFSET, en));
    CHECK(pmd_write_field(mss, TX_OVRD_ENABLE_ADDR, TX_OVRD_ENABLE_TX_BYPASS_A_MASK, TX_OVRD_ENABLE_TX_BYPASS_A_OFFSET, en));
    CHECK(pmd_write_field(mss, TX_OVRD_ENABLE_ADDR, TX_OVRD_ENABLE_ELECIDLE_A_MASK, TX_OVRD_ENABLE_ELECIDLE_A_OFFSET, en));
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_TXSOC_CLK_A_MASK, TX_OVRD_REG1_TXSOC_CLK_A_OFFSET, en));

    CHECK(pmd_write_field(mss, TX_OVRD_REG3_ADDR, TX_OVRD_REG3_EXTRA_RESET_B_MASK, TX_OVRD_REG3_EXTRA_RESET_B_OFFSET, 1));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_isolate_lane_get(mss_access_t *mss, uint32_t *en) {
    CHECK(pmd_read_field(mss, RX_OVRD_ENABLE_ADDR, RX_OVRD_ENABLE_RST_A_MASK, RX_OVRD_ENABLE_RST_A_OFFSET, en));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_r2l_hsref_sel_set(mss_access_t *mss, uint32_t sel) {
    CHECK(pmd_write_field(mss, CMN_REFCLK_ADDR, CMN_REFCLK_R2L_HSREF_SELECT_NT_MASK,CMN_REFCLK_R2L_HSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_r2l0_lsref_sel_set(mss_access_t *mss, uint32_t sel) {
    CHECK(pmd_write_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_R2L0_LSREF_SELECT_NT_MASK,CMN_OVRD_ICTL_R2L0_LSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_r2l1_lsref_sel_set(mss_access_t *mss, uint32_t sel) {
    CHECK(pmd_write_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_R2L1_LSREF_SELECT_NT_MASK,CMN_OVRD_ICTL_R2L1_LSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_l2r_hsref_sel_set(mss_access_t *mss, uint32_t sel) {
    CHECK(pmd_write_field(mss, CMN_REFCLK_ADDR, CMN_REFCLK_L2R_HSREF_SELECT_NT_MASK,CMN_REFCLK_L2R_HSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_l2r0_lsref_sel_set(mss_access_t *mss, uint32_t sel) {
    CHECK(pmd_write_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_L2R0_LSREF_SELECT_NT_MASK,CMN_OVRD_ICTL_L2R0_LSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_l2r1_lsref_sel_set(mss_access_t *mss, uint32_t sel) {
    CHECK(pmd_write_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_L2R1_LSREF_SELECT_NT_MASK,CMN_OVRD_ICTL_L2R1_LSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_r2l_hsref_sel_get(mss_access_t *mss, uint32_t *sel) {
    CHECK(pmd_read_field(mss, CMN_REFCLK_ADDR, CMN_REFCLK_R2L_HSREF_SELECT_NT_MASK,CMN_REFCLK_R2L_HSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_r2l0_lsref_sel_get(mss_access_t *mss, uint32_t *sel) {
    CHECK(pmd_read_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_R2L0_LSREF_SELECT_NT_MASK,CMN_OVRD_ICTL_R2L0_LSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_r2l1_lsref_sel_get(mss_access_t *mss, uint32_t *sel) {
    CHECK(pmd_read_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_R2L1_LSREF_SELECT_NT_MASK,CMN_OVRD_ICTL_R2L1_LSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_l2r_hsref_sel_get(mss_access_t *mss, uint32_t *sel) {
    CHECK(pmd_read_field(mss, CMN_REFCLK_ADDR, CMN_REFCLK_L2R_HSREF_SELECT_NT_MASK,CMN_REFCLK_L2R_HSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_l2r0_lsref_sel_get(mss_access_t *mss, uint32_t *sel) {
    CHECK(pmd_read_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_L2R0_LSREF_SELECT_NT_MASK,CMN_OVRD_ICTL_L2R0_LSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_l2r1_lsref_sel_get(mss_access_t *mss, uint32_t *sel) {
    CHECK(pmd_read_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_L2R1_LSREF_SELECT_NT_MASK,CMN_OVRD_ICTL_L2R1_LSREF_SELECT_NT_OFFSET , sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_lsref_sel_set(mss_access_t *mss, uint32_t ref_sel) {
    CHECK(pmd_write_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_LSREF_SELECT_NT_MASK, CMN_OVRD_ICTL_LSREF_SELECT_NT_OFFSET, ref_sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_lsref_sel_get(mss_access_t *mss, uint32_t *ref_sel) {
    CHECK(pmd_read_field(mss, CMN_OVRD_ADDR, CMN_OVRD_ICTL_LSREF_SELECT_NT_MASK, CMN_OVRD_ICTL_LSREF_SELECT_NT_OFFSET, ref_sel));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_gen_tx_en_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_BIST_ENABLE_A_MASK, TX_DATAPATH_REG1_BIST_ENABLE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_gen_tx_en_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, TX_DATAPATH_REG1_ADDR, TX_DATAPATH_REG1_BIST_ENABLE_A_MASK, TX_DATAPATH_REG1_BIST_ENABLE_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_error_cnt_done_get(mss_access_t *mss, uint32_t *err_count_done) {
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_ERROR_CNT_DONE_NT_MASK, RX_DATABIST_TOP_RDREG1_ERROR_CNT_DONE_NT_OFFSET, err_count_done));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_iso_request_cmn_state_change(mss_access_t *mss, jupiter_cmn_pstate_t cmn_pstate, uint32_t timeout_us) {
    int poll_result = 0;
    jupiter_pmd_iso_cmn_pstate_set(mss, cmn_pstate);
    poll_result = pmd_poll_field(mss, SEQ_CNTRL_CMN_ADDR, SEQ_CNTRL_CMN_LCPLL_LOCK_A_MASK, SEQ_CNTRL_CMN_LCPLL_LOCK_A_OFFSET, 1, timeout_us);

    if (poll_result == -1) {
        USR_PRINTF(("ERROR: polling for CMN state ack\n"));
    } else {
        USR_PRINTF(("CMN state ack received\n"));
    }
    if (poll_result == -1) {
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        return JUPITER_ERR_CODE_NONE;
    }
}

int jupiter_pmd_iso_request_tx_state_change(mss_access_t *mss, jupiter_pstate_t tx_pstate, uint32_t tx_rate, uint32_t tx_width, uint32_t timeout_us) {
    int poll_result;

    if (tx_pstate == JUPITER_P0 || tx_pstate == JUPITER_PD) {
        USR_PRINTF(("Setting TX rate/width/pstate\n"));
        jupiter_pmd_iso_tx_reset_set(mss,0);
        jupiter_pmd_iso_tx_rate_set(mss,tx_rate);
        jupiter_pmd_iso_tx_width_set(mss,tx_width);
        jupiter_pmd_iso_tx_pstate_set(mss,tx_pstate);
        jupiter_pmd_iso_tx_reset_set(mss,1);

        if (tx_pstate == JUPITER_P0) {
            poll_result = pmd_poll_field(mss, SEQ_CNTRL_TX_ADDR, SEQ_CNTRL_TX_BLOCK_VALID_A_MASK, SEQ_CNTRL_TX_BLOCK_VALID_A_OFFSET, 1, timeout_us); 

            if (poll_result == -1) {
                USR_PRINTF(("ERROR: polling for TX state ack\n"));
            } else {
                USR_PRINTF(("TX state ack received\n"));
            }
        } else {
            USR_SLEEP(timeout_us);
            poll_result = 0;
        }

        if (poll_result == -1) {
            return JUPITER_ERR_CODE_POLL_TIMEOUT;
        } else {
            return JUPITER_ERR_CODE_NONE;
        }
    } else {
       USR_PRINTF(("ERROR: Invalid pstate argument.\n"));
       return JUPITER_ERR_CODE_INVALID_ARG_VALUE; 
    }
}

int jupiter_pmd_iso_request_rx_state_change(mss_access_t *mss, jupiter_pstate_t rx_pstate, uint32_t rx_rate, uint32_t rx_width, uint32_t timeout_us) {
    int poll_result;

    if (rx_pstate == JUPITER_P0 || rx_pstate == JUPITER_PD) {
        USR_PRINTF(("Setting RX rate/width/pstate\n"));
        jupiter_pmd_iso_rx_reset_set(mss,0);
        jupiter_pmd_iso_rx_rate_set(mss,rx_rate);
        jupiter_pmd_iso_rx_width_set(mss,rx_width);
        jupiter_pmd_iso_rx_pstate_set(mss,rx_pstate);
        jupiter_pmd_iso_rx_reset_set(mss,1);

        if (rx_pstate == JUPITER_P0) {
            poll_result = pmd_poll_field(mss, SEQ_CNTRL_RX_ADDR, SEQ_CNTRL_RX_BLOCK_VALID_A_MASK, SEQ_CNTRL_RX_BLOCK_VALID_A_OFFSET, 1, timeout_us);

            if (poll_result == -1) {
                USR_PRINTF(("ERROR: polling for RX state ack\n"));
            } else {
                USR_PRINTF(("RX state ack received\n"));
            }
        } else {
            USR_SLEEP(timeout_us);
            poll_result = 0;
        }

        if (poll_result == -1) {
            return JUPITER_ERR_CODE_POLL_TIMEOUT;
        } else {
            return JUPITER_ERR_CODE_NONE;
        }
    } else {
       USR_PRINTF(("ERROR: Invalid pstate argument.\n"));
       return JUPITER_ERR_CODE_INVALID_ARG_VALUE; 
    }

}

int jupiter_pmd_rx_check_cdr_lock(mss_access_t *mss, uint32_t timeout_us) {
    int poll_result;
    poll_result = pmd_poll_field(mss, RX_STAT_ADDR, RX_STAT_OCTL_RX_DATA_VLD_MASK, RX_STAT_OCTL_RX_DATA_VLD_OFFSET, 1, timeout_us);

    if (poll_result == -1) {
        USR_PRINTF(("ERROR: RX CDR timed out waiting for lock\n")); 
        return JUPITER_ERR_CODE_POLL_TIMEOUT;

    } else {
        USR_PRINTF(("RX CDR is locked\n")); 
        return JUPITER_ERR_CODE_NONE;
    }
}

int jupiter_pmd_rx_check_bist (mss_access_t *mss, jupiter_bist_mode_t bist_mode, uint32_t timer_threshold, uint32_t rx_width, uint32_t timeout_us, int32_t expected_errors) {
    uint32_t err_count_overflow, err_cnt_55_32, err_cnt_31_0;
    uint64_t err_count;
    uint32_t width;
    USR_DOUBLE ber;
    int poll_result;
    uint64_t temp_value;

    if (bist_mode == JUPITER_TIMER) {
        /* Timer mode */
        poll_result = pmd_poll_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_ERROR_CNT_DONE_NT_MASK, RX_DATABIST_TOP_RDREG1_ERROR_CNT_DONE_NT_OFFSET, 1, timeout_us);

        if (poll_result == -1) {
            USR_PRINTF(("ERROR: Timed out waiting for error_cnt_done\n"));
            return JUPITER_ERR_CODE_POLL_TIMEOUT;

        } else {
            if (expected_errors == -1) {
                CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG3_ADDR, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_MASK, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_OFFSET, &err_cnt_55_32));
                CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG2_ADDR, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_MASK, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_OFFSET, &err_cnt_31_0));
            } else {
                CHECK(pmd_read_check_field(mss, RX_DATABIST_TOP_RDREG3_ADDR, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_MASK, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_OFFSET, RD_EQ, &err_cnt_55_32, 0, 0 /*NULL*/));
                CHECK(pmd_read_check_field(mss, RX_DATABIST_TOP_RDREG2_ADDR, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_MASK, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_OFFSET, RD_EQ, &err_cnt_31_0, (uint32_t)expected_errors, 0 /*NULL*/));
            }
            /* CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG2_ADDR, RX_DATABIST_TOP_REG2_TIMER_THRESHOLD_NT_MASK, RX_DATABIST_TOP_REG2_TIMER_THRESHOLD_NT_OFFSET, &timer_threshold)); */
            /* CHECK(pmd_read_field(mss, RX_DATABIST_TOP_REG3_ADDR, RX_DATABIST_TOP_REG3_DATA_WIDTH_NT_MASK, RX_DATABIST_TOP_REG3_DATA_WIDTH_NT_OFFSET, &width_encoded)); */
            COMPILER_64_SET(err_count, err_cnt_55_32, err_cnt_31_0);
            CHECK(pmd_read_check_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_ERROR_CNT_OVERFLOW_NT_MASK, RX_DATABIST_TOP_RDREG1_ERROR_CNT_OVERFLOW_NT_OFFSET, RD_EQ, &err_count_overflow, 0, 0 /*NULL*/));
            width = jupiter_width_decoder(rx_width);
            /* ber = err_count / (width * timer_threshold); */
            COMPILER_64_COPY(temp_value, err_count);
            COMPILER_64_UDIV_32(temp_value, width);
            COMPILER_64_UDIV_32(temp_value, timer_threshold);
            ber = COMPILER_64_LO(temp_value);
            USR_PRINTF(("err_count = Upper 0x%x Lower 0x%x\n",COMPILER_64_HI(err_count), COMPILER_64_LO(err_count)));
            USR_PRINTF(("err_count_overflow = %d\n", err_count_overflow));
#ifdef SERDES_API_FLOATING_POINT
            USR_PRINTF(("ber = %e\n", ber));
#else
            USR_PRINTF(("ber = %d\n", ber));
#endif

            return JUPITER_ERR_CODE_NONE;
        }
    }
    /* Should not get here */
    return JUPITER_ERR_CODE_FUNC_FAILURE;
}

int jupiter_pmd_eqeval_type_set(mss_access_t *mss, uint32_t eq_type) {
    CHECK(pmd_write_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_LINKEVAL_TYPE_A_MASK, RX_OVRD_REG1_ICTL_RX_LINKEVAL_TYPE_A_OFFSET, eq_type));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_eqeval_req_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, RX_OVRD_REG1_ADDR, RX_OVRD_REG1_ICTL_RX_LINKEVAL_REQ_A_MASK, RX_OVRD_REG1_ICTL_RX_LINKEVAL_REQ_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_eqeval_ack_get(mss_access_t *mss, uint32_t *eqeval_ack) {
    CHECK(pmd_read_field(mss, RX_STAT_ADDR, RX_STAT_OCTL_RX_LINKEVAL_ACK_MASK, RX_STAT_OCTL_RX_LINKEVAL_ACK_OFFSET, eqeval_ack));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_eqeval_incdec_get(mss_access_t *mss, uint32_t *incdec) {
    CHECK(pmd_read_field(mss, RX_STAT_ADDR, RX_STAT_OCTL_RX_LINKEVAL_DIR_MASK, RX_STAT_OCTL_RX_LINKEVAL_DIR_OFFSET, incdec));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_equalize(mss_access_t *mss, jupiter_eq_type_t eq_type, uint32_t timeout_us) {
    int poll_result;
    uint32_t incdec;
    jupiter_pmd_eqeval_type_set(mss, eq_type);
    jupiter_pmd_eqeval_req_set(mss,1);
    poll_result = pmd_poll_field(mss, RX_STAT_ADDR, RX_STAT_OCTL_RX_LINKEVAL_ACK_MASK, RX_STAT_OCTL_RX_LINKEVAL_ACK_OFFSET, 1, timeout_us);
    jupiter_pmd_eqeval_req_set(mss,0);
    if (poll_result == -1) {
        USR_PRINTF(("ERROR: Timed out waiting for rx linkeval ack\n"));
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        USR_PRINTF(("Received RXEQ EVAL Ack\n"));
        jupiter_pmd_eqeval_incdec_get(mss,&incdec);
        USR_PRINTF(("EqEval incdec = 0x%X\n",incdec));
        return JUPITER_ERR_CODE_NONE;
    }
}

int jupiter_pmd_tx_rxdet_req_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, TX_OVRD_REG3_ADDR, TX_OVRD_REG3_ICTL_PCIE_TX_RXDET_REQ_A_MASK, TX_OVRD_REG3_ICTL_PCIE_TX_RXDET_REQ_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_rxdet(mss_access_t *mss, uint32_t rxdet_expected, uint32_t timeout_us) {
    int poll_result;
    uint32_t rxdet_result;

    jupiter_pmd_tx_rxdet_req_set(mss,1);
    poll_result = pmd_poll_field(mss, TX_STAT_ADDR, TX_STAT_OCTL_PCIE_TX_RXDET_ACK_MASK, TX_STAT_OCTL_PCIE_TX_RXDET_ACK_OFFSET, 1, timeout_us);

    jupiter_pmd_tx_rxdet_req_set(mss,0);

    if (poll_result == -1) {
        USR_PRINTF(("ERROR: Timed out waiting for tx_rxdet ack\n"));
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        USR_PRINTF(("Received TX RXDET Ack\n"));
        CHECK(pmd_read_check_field(mss, TX_STAT_ADDR, TX_STAT_OCTL_PCIE_TX_RXDET_RESULT_MASK, TX_STAT_OCTL_PCIE_TX_RXDET_RESULT_OFFSET, RD_EQ, &rxdet_result, rxdet_expected, 0 /*NULL*/));
        if (rxdet_result != rxdet_expected) {
           USR_PRINTF(("ERROR: Expected rxdet_result = %d, Actual rxdet_result = %d\n",rxdet_expected, rxdet_result));
           return JUPITER_ERR_CODE_CHECK_FAILURE;
        } else {
            USR_PRINTF(("Expected rxdet_result = %d, Actual rxdet_result = %d\n",rxdet_expected, rxdet_result));
            return JUPITER_ERR_CODE_NONE;
        }
    }
}

int jupiter_pmd_tx_beacon_en_set(mss_access_t *mss, uint32_t value) {
    CHECK(pmd_write_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_BEACON_ENA_A_MASK, TX_OVRD_REG1_ICTL_TX_BEACON_ENA_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_beacon_en_get(mss_access_t *mss, uint32_t *value) {
    CHECK(pmd_read_field(mss, TX_OVRD_REG1_ADDR, TX_OVRD_REG1_ICTL_TX_BEACON_ENA_A_MASK, TX_OVRD_REG1_ICTL_TX_BEACON_ENA_A_OFFSET, value));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_pll_fine_code_get(mss_access_t *mss, uint32_t * tx_pll_fine_code, uint32_t center_code, int tolerance) {
    CHECK(pmd_write_field(mss, TX_SSCM_DLPF_REG3_ADDR, TX_SSCM_DLPF_REG3_RO_CSR_CAPTURE_A_MASK, TX_SSCM_DLPF_REG3_RO_CSR_CAPTURE_A_OFFSET, 0));
    CHECK(pmd_write_field(mss, TX_SSCM_DLPF_REG3_ADDR, TX_SSCM_DLPF_REG3_RO_CSR_CAPTURE_A_MASK, TX_SSCM_DLPF_REG3_RO_CSR_CAPTURE_A_OFFSET, 1));
    if (tolerance == -1) {
        /* Read back fine code without check */
        CHECK(pmd_read_field(mss, TX_SSCM_DLPF_RDREG2_ADDR, TX_SSCM_DLPF_RDREG2_CTL_AFE_DS_OSC_INT_NT_MASK, TX_SSCM_DLPF_RDREG2_CTL_AFE_DS_OSC_INT_NT_OFFSET, tx_pll_fine_code));
    } else {
        /* Read back fine code with range check */
        CHECK(pmd_read_check_field(mss, TX_SSCM_DLPF_RDREG2_ADDR, TX_SSCM_DLPF_RDREG2_CTL_AFE_DS_OSC_INT_NT_MASK, TX_SSCM_DLPF_RDREG2_CTL_AFE_DS_OSC_INT_NT_OFFSET, RD_RANGE, tx_pll_fine_code, center_code-tolerance, center_code+tolerance));
    }
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_tx_pll_coarse_code_get(mss_access_t *mss, uint32_t * tx_pll_coarse_code) {
    CHECK(pmd_read_field(mss, TX_VCO_ADAPT_RDREG2_ADDR, TX_VCO_ADAPT_RDREG2_DS_OSC_CAL_MASK, TX_VCO_ADAPT_RDREG2_DS_OSC_CAL_OFFSET, tx_pll_coarse_code));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_pll_fine_code_get(mss_access_t *mss, uint32_t * rx_pll_fine_code, uint32_t center_code, int tolerance) {
    CHECK(pmd_write_field(mss, RX_SSCM_DLPF_REG3_ADDR, RX_SSCM_DLPF_REG3_RO_CSR_CAPTURE_A_MASK, RX_SSCM_DLPF_REG3_RO_CSR_CAPTURE_A_OFFSET, 0));
    CHECK(pmd_write_field(mss, RX_SSCM_DLPF_REG3_ADDR, RX_SSCM_DLPF_REG3_RO_CSR_CAPTURE_A_MASK, RX_SSCM_DLPF_REG3_RO_CSR_CAPTURE_A_OFFSET, 1));
    if (tolerance == -1) {
        /* Read back fine code without check */
        CHECK(pmd_read_field(mss, RX_SSCM_DLPF_RDREG2_ADDR, RX_SSCM_DLPF_RDREG2_CTL_AFE_DS_OSC_INT_NT_MASK, RX_SSCM_DLPF_RDREG2_CTL_AFE_DS_OSC_INT_NT_OFFSET, rx_pll_fine_code));
    } else {
        /* Read back fine code with range check */
        CHECK(pmd_read_check_field(mss, RX_SSCM_DLPF_RDREG2_ADDR, RX_SSCM_DLPF_RDREG2_CTL_AFE_DS_OSC_INT_NT_MASK, RX_SSCM_DLPF_RDREG2_CTL_AFE_DS_OSC_INT_NT_OFFSET, RD_RANGE, rx_pll_fine_code, center_code-tolerance, center_code+tolerance));
    }
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_rx_pll_coarse_code_get(mss_access_t *mss, uint32_t * rx_pll_coarse_code) {
    CHECK(pmd_read_field(mss, RX_VCO_ADAPT_RDREG2_ADDR, RX_VCO_ADAPT_RDREG2_DS_OSC_CAL_MASK, RX_VCO_ADAPT_RDREG2_DS_OSC_CAL_OFFSET, rx_pll_coarse_code));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_pll_fine_code_get(mss_access_t *mss, uint32_t * cmn_pll_fine_code, uint32_t center_code, int tolerance) {
    CHECK(pmd_write_field(mss, AFE_CMN_LCPLL_OSC_REG3_ADDR, AFE_CMN_LCPLL_OSC_REG3_RO_CSR_CAPTURE_A_MASK, AFE_CMN_LCPLL_OSC_REG3_RO_CSR_CAPTURE_A_OFFSET, 0));
    CHECK(pmd_write_field(mss, AFE_CMN_LCPLL_OSC_REG3_ADDR, AFE_CMN_LCPLL_OSC_REG3_RO_CSR_CAPTURE_A_MASK, AFE_CMN_LCPLL_OSC_REG3_RO_CSR_CAPTURE_A_OFFSET, 1));
    if (tolerance == -1) {
        /*Read back fine code without check */
        CHECK(pmd_read_field(mss, AFE_CMN_LCPLL_OSC_RDREG1_ADDR, AFE_CMN_LCPLL_OSC_RDREG1_CLKGEN_OSC_NT_MASK, AFE_CMN_LCPLL_OSC_RDREG1_CLKGEN_OSC_NT_OFFSET, cmn_pll_fine_code));
    } else {
        /*Read back fine code with range check */
        CHECK(pmd_read_check_field(mss, AFE_CMN_LCPLL_OSC_RDREG1_ADDR, AFE_CMN_LCPLL_OSC_RDREG1_CLKGEN_OSC_NT_MASK, AFE_CMN_LCPLL_OSC_RDREG1_CLKGEN_OSC_NT_OFFSET, RD_RANGE, cmn_pll_fine_code, center_code-tolerance, center_code+tolerance));
    }
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_cmn_pll_coarse_code_get(mss_access_t *mss, uint32_t * cmn_pll_coarse_code) {
    CHECK(pmd_read_field(mss, AFE_CMN_VCO_ADAPT_RDREG3_ADDR, AFE_CMN_VCO_ADAPT_RDREG3_CLKGEN_OSC_CAL_MASK, AFE_CMN_VCO_ADAPT_RDREG3_CLKGEN_OSC_CAL_OFFSET, cmn_pll_coarse_code));
    return JUPITER_ERR_CODE_NONE;
}


int jupiter_pmd_measure_pmon(mss_access_t *mss, uint32_t pmon_sel, uint32_t pvt_measure_timing_window, uint32_t timeout_us, uint32_t * pvt_measure_result) {
    int poll_result;

    CHECK(pmd_write_field(mss, PD_AFE_CMN_ADDR, PD_AFE_CMN_ATEST_PMON_BA_MASK, PD_AFE_CMN_ATEST_PMON_BA_OFFSET, 1));
    CHECK(pmd_write_field(mss, RST_AFE_CMN_ADDR, RST_AFE_CMN_ATEST_PMON_BA_MASK, RST_AFE_CMN_ATEST_PMON_BA_OFFSET, 1));

    CHECK(pmd_write_field(mss, AFE_CMN_ATEST_ADDR, AFE_CMN_ATEST_BYPASS_ENA_A_MASK, AFE_CMN_ATEST_BYPASS_ENA_A_OFFSET, 1));
    CHECK(pmd_write_field(mss, AFE_CMN_ATEST_ADDR, AFE_CMN_ATEST_PMONSEL_A_MASK, AFE_CMN_ATEST_PMONSEL_A_OFFSET, pmon_sel));

    CHECK(pmd_write_field(mss, PVT_MEASURE_ADDR, PVT_MEASURE_TIMING_WINDOW_NT_MASK,PVT_MEASURE_TIMING_WINDOW_NT_OFFSET, pvt_measure_timing_window));
    CHECK(pmd_write_field(mss, PVT_MEASURE_ADDR, PVT_MEASURE_FREQ_TARGET_NT_MASK,PVT_MEASURE_FREQ_TARGET_NT_OFFSET, 0));

    CHECK(pmd_write_field(mss, PVT_MEASURE_ADDR, PVT_MEASURE_REQ_A_MASK, PVT_MEASURE_REQ_A_OFFSET, 1));

    poll_result = pmd_poll_field(mss, PVT_MEASURE_RDREG_ADDR, PVT_MEASURE_RDREG_ACK_A_MASK, PVT_MEASURE_RDREG_ACK_A_OFFSET, 1, timeout_us);

    CHECK(pmd_write_field(mss, PVT_MEASURE_ADDR, PVT_MEASURE_REQ_A_MASK, PVT_MEASURE_REQ_A_OFFSET, 0));

    if (poll_result == -1) {
        USR_PRINTF(("ERROR: Timed out waiting for pvt_measure_ack\n"));
        return JUPITER_ERR_CODE_POLL_TIMEOUT;
    } else {
        USR_PRINTF(("Received pvt_measure_ack\n"));
        CHECK(pmd_read_field(mss, PVT_MEASURE_RDREG_ADDR, PVT_MEASURE_RDREG_RESULT_MASK, PVT_MEASURE_RDREG_RESULT_OFFSET, pvt_measure_result));
        USR_PRINTF(("pmon_sel = %d. pvt_measure_result = %d\n", pmon_sel, *pvt_measure_result));
        return JUPITER_ERR_CODE_NONE;
    }
}

int jupiter_pmd_atest_en(mss_access_t *mss, uint32_t en) {

    CHECK(pmd_write_field(mss, AFE_CMN_ATEST_ADDR, AFE_CMN_ATEST_BYPASS_ENA_A_MASK, AFE_CMN_ATEST_BYPASS_ENA_A_OFFSET, en));
    CHECK(pmd_write_field(mss, PD_AFE_CMN_ADDR, PD_AFE_CMN_ATEST_PMON_BA_MASK, PD_AFE_CMN_ATEST_PMON_BA_OFFSET, en));
    CHECK(pmd_write_field(mss, PD_AFE_CMN_ADDR, PD_AFE_CMN_ATEST_ADC_BA_MASK, PD_AFE_CMN_ATEST_ADC_BA_OFFSET, en));
    CHECK(pmd_write_field(mss, RST_AFE_CMN_ADDR, RST_AFE_CMN_ATEST_ADC_BA_MASK, RST_AFE_CMN_ATEST_ADC_BA_OFFSET, en));
    CHECK(pmd_write_field(mss, RST_AFE_CMN_ADDR, RST_AFE_CMN_ATEST_PMON_BA_MASK, RST_AFE_CMN_ATEST_PMON_BA_OFFSET, en));

    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_atest_cmn_capture(mss_access_t *mss, uint32_t atest_addr, uint32_t * atest_adc_val) {
    CHECK(pmd_write_field(mss, CMN_ATEST_ADDR, CMN_ATEST_ADDR_BIN_NT_MASK, CMN_ATEST_ADDR_BIN_NT_OFFSET, atest_addr));
    CHECK(pmd_write_field(mss, CMN_ATEST_ADDR, AFE_CMN_ATEST_CAPTURE_ENA_NT_MASK, AFE_CMN_ATEST_CAPTURE_ENA_NT_OFFSET, 1));
    USR_SLEEP(5);
    CHECK(pmd_read_field(mss, AFE_CMN_ATEST_RDREG_ADDR, AFE_CMN_ATEST_RDREG_ADC_NT_MASK, AFE_CMN_ATEST_RDREG_ADC_NT_OFFSET, atest_adc_val));
    CHECK(pmd_write_field(mss, CMN_ATEST_ADDR, AFE_CMN_ATEST_CAPTURE_ENA_NT_MASK, AFE_CMN_ATEST_CAPTURE_ENA_NT_OFFSET, 1));
    CHECK(pmd_write_field(mss, CMN_ATEST_ADDR, CMN_ATEST_ADDR_BIN_NT_MASK, CMN_ATEST_ADDR_BIN_NT_OFFSET, 0));
    return JUPITER_ERR_CODE_NONE;
}

int jupiter_pmd_read_status(mss_access_t *mss, int branch) {
    int i,j;
    uint32_t err_count_done, err_count_overflow,err_cnt_55_32,err_cnt_31_0;
    uint64_t err_count;
    jupiter_dsp_param_t dsp_info_[JUPITER_NUM_BRANCHES];
    jupiter_afe_data_t rx_afe_data_;
    jupiter_dcdiq_data_t rx_dcdiq_data_;
    jupiter_dcdiq_data_t tx_dcdiq_data_;

    uint32_t tx_pll_fine_code;
    uint32_t tx_pll_coarse_code;
    uint32_t rx_pll_fine_code;
    uint32_t rx_pll_coarse_code;
    uint32_t cmn_pll_fine_code;
    uint32_t cmn_pll_coarse_code;

    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_ERROR_CNT_DONE_NT_MASK, RX_DATABIST_TOP_RDREG1_ERROR_CNT_DONE_NT_OFFSET, &err_count_done));
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG3_ADDR, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_MASK, RX_DATABIST_TOP_RDREG3_ERROR_CNT_55T32_NT_OFFSET, &err_cnt_55_32));
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG2_ADDR, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_MASK, RX_DATABIST_TOP_RDREG2_ERROR_CNT_NT_OFFSET, &err_cnt_31_0));
    CHECK(pmd_read_field(mss, RX_DATABIST_TOP_RDREG1_ADDR, RX_DATABIST_TOP_RDREG1_ERROR_CNT_OVERFLOW_NT_MASK, RX_DATABIST_TOP_RDREG1_ERROR_CNT_OVERFLOW_NT_OFFSET, &err_count_overflow));

    COMPILER_64_SET(err_count, err_cnt_55_32, err_cnt_31_0);

    /* If branch = -1, read all branches */
    if (branch == -1) {
        for (i=0; i<JUPITER_NUM_BRANCHES; i++) {
            CHECK(jupiter_pmd_rx_dsp_get(mss, i, &dsp_info_[i]));
        }
    } else {
        CHECK(jupiter_pmd_rx_dsp_get(mss, branch, &dsp_info_[branch]));
    }
    CHECK(jupiter_pmd_rx_afe_get(mss, &rx_afe_data_));
    CHECK(jupiter_pmd_rx_dcdiq_get(mss, &rx_dcdiq_data_));
    CHECK(jupiter_pmd_tx_dcdiq_get(mss, &tx_dcdiq_data_));
    CHECK(jupiter_pmd_tx_pll_fine_code_get(mss, &tx_pll_fine_code,0,-1));
    CHECK(jupiter_pmd_tx_pll_coarse_code_get(mss, &tx_pll_coarse_code));
    CHECK(jupiter_pmd_rx_pll_fine_code_get(mss, &rx_pll_fine_code,0,-1));
    CHECK(jupiter_pmd_rx_pll_coarse_code_get(mss, &rx_pll_coarse_code));
    CHECK(jupiter_pmd_cmn_pll_fine_code_get(mss, &cmn_pll_fine_code,0,-1));
    CHECK(jupiter_pmd_cmn_pll_coarse_code_get(mss, &cmn_pll_coarse_code));
   

    USR_PRINTF(("------------------------------------------\n"));
    USR_PRINTF(("-----------  STATUS REGISTERS  -----------\n"));
    USR_PRINTF(("------------------------------------------\n\n"));        
    USR_PRINTF(("err_count_done                  = 0x%x\n",err_count_done));
    USR_PRINTF(("err_count                       = Upper 0x%x Lower 0x%x\n",COMPILER_64_HI(err_count), COMPILER_64_LO(err_count)));
    USR_PRINTF(("err_count_overflow              = 0x%x\n",err_count_overflow)); 
    if (branch == -1) {
        for (i=0; i<JUPITER_NUM_BRANCHES; i++) {
            for(j=0; j<JUPITER_FFE_NUM_TAPS; j++) {
                USR_PRINTF(("BRANCH %2d: ffe[%2d]               = 0x%x\n",i,j, dsp_info_[i].ffe[j]));
            }
            USR_PRINTF(("BRANCH %2d: dc_offset             = 0x%x\n",i, dsp_info_[i].dc_offset));
            USR_PRINTF(("BRANCH %2d: dfe                   = 0x%x\n",i, dsp_info_[i].dfe));
            USR_PRINTF(("BRANCH %2d: thresholds.eh         = 0x%x\n",i, dsp_info_[i].thresholds.eh));
            USR_PRINTF(("BRANCH %2d: thresholds.ez         = 0x%x\n",i, dsp_info_[i].thresholds.ez));
            USR_PRINTF(("BRANCH %2d: thresholds.el         = 0x%x\n",i, dsp_info_[i].thresholds.el));
            USR_PRINTF(("BRANCH %2d: thresholds.thres_low  = 0x%x\n",i, dsp_info_[i].thresholds.thres_low));
            USR_PRINTF(("BRANCH %2d: thresholds.thres_high = 0x%x\n",i, dsp_info_[i].thresholds.thres_hi));
            USR_PRINTF(("BRANCH %2d: slicers.el3           = 0x%x\n",i, dsp_info_[i].slicers.el3));
            USR_PRINTF(("BRANCH %2d: slicers.el1           = 0x%x\n",i, dsp_info_[i].slicers.el1));
            USR_PRINTF(("BRANCH %2d: slicers.eh1           = 0x%x\n",i, dsp_info_[i].slicers.eh1));
            USR_PRINTF(("BRANCH %2d: slicers.eh3           = 0x%x\n",i, dsp_info_[i].slicers.eh3));
        }
    } else {
        for(j=0; j<JUPITER_FFE_NUM_TAPS; j++) {
            USR_PRINTF(("BRANCH %2d: ffe[%2d]               = 0x%x\n", branch, j, dsp_info_[branch].ffe[j]));
        }
        USR_PRINTF(("BRANCH %2d: dc_offset             = 0x%x\n", branch, dsp_info_[branch].dc_offset));
        USR_PRINTF(("BRANCH %2d: dfe                   = 0x%x\n", branch, dsp_info_[branch].dfe));
        USR_PRINTF(("BRANCH %2d: thresholds.eh         = 0x%x\n", branch, dsp_info_[branch].thresholds.eh));
        USR_PRINTF(("BRANCH %2d: thresholds.ez         = 0x%x\n", branch, dsp_info_[branch].thresholds.ez));
        USR_PRINTF(("BRANCH %2d: thresholds.el         = 0x%x\n", branch, dsp_info_[branch].thresholds.el));
        USR_PRINTF(("BRANCH %2d: thresholds.thres_low  = 0x%x\n", branch, dsp_info_[branch].thresholds.thres_low));
        USR_PRINTF(("BRANCH %2d: thresholds.thres_high = 0x%x\n", branch, dsp_info_[branch].thresholds.thres_hi));
        USR_PRINTF(("BRANCH %2d: slicers.el3           = 0x%x\n", branch, dsp_info_[branch].slicers.el3));
        USR_PRINTF(("BRANCH %2d: slicers.el1           = 0x%x\n", branch, dsp_info_[branch].slicers.el1));
        USR_PRINTF(("BRANCH %2d: slicers.eh1           = 0x%x\n", branch, dsp_info_[branch].slicers.eh1));
        USR_PRINTF(("BRANCH %2d: slicers.eh3           = 0x%x\n", branch, dsp_info_[branch].slicers.eh3));

    }
    USR_PRINTF(("err_count_overflow               = 0x%x\n", err_count_overflow));
    USR_PRINTF(("ctle_rate                        = 0x%x\n", rx_afe_data_.ctle_rate));
    USR_PRINTF(("ctle_boost                       = 0x%x\n", rx_afe_data_.ctle_boost));
    USR_PRINTF(("vga_coarse                       = 0x%x\n", rx_afe_data_.vga_coarse));
    USR_PRINTF(("vga_fine                         = 0x%x\n", rx_afe_data_.vga_fine));
    USR_PRINTF(("vga_offset                       = 0x%x\n", rx_afe_data_.vga_offset));
    USR_PRINTF(("RX dcdiq.d0                      = 0x%x\n", rx_dcdiq_data_.d0));
    USR_PRINTF(("RX dcdiq.d90                     = 0x%x\n", rx_dcdiq_data_.d90));
    USR_PRINTF(("RX dcdiq.iq                      = 0x%x\n", rx_dcdiq_data_.iq));
    USR_PRINTF(("TX dcdiq.d0                      = 0x%x\n", tx_dcdiq_data_.d0));
    USR_PRINTF(("TX dcdiq.d90                     = 0x%x\n", tx_dcdiq_data_.d90));
    USR_PRINTF(("TX dcdiq.iq                      = 0x%x\n", tx_dcdiq_data_.iq));
    USR_PRINTF(("CMNPLL coarse code               = 0x%x\n", cmn_pll_coarse_code));
    USR_PRINTF(("CMNPLL fine code                 = 0x%x\n", cmn_pll_fine_code));
    USR_PRINTF(("TXPLL coarse code                = 0x%x\n", tx_pll_coarse_code));
    USR_PRINTF(("TXPLL fine code                  = 0x%x\n", tx_pll_fine_code));
    USR_PRINTF(("RXPLL coarse code                = 0x%x\n", rx_pll_coarse_code));
    USR_PRINTF(("RXPLL fine code                  = 0x%x\n", rx_pll_fine_code));

    return JUPITER_ERR_CODE_NONE;
}
