/*----------------------------------------------------------------------
 * $Id: tbhmod_flexe_cfg_seq.c,v 1.8 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tbhmod_flexe_cfg_seq.c
 * Description: c functions implementing Tier1s for TBHMod FlexE Serdes Driver
 *---------------------------------------------------------------------*/

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tscbh_flexe_xgxs_defs.h>
#include <tscbh/tier1/tbhmod_enum_defines.h>
#include <tscbh/tier1/tbhmod_spd_ctrl.h>
#include <tscbh/tier1/tbhmod.h>
#include <tscbh/tier1/tbhmod_sc_lkup_table.h>
#include <tscbh/tier1/tbhPCSRegEnums.h>
#include <tscbh_flexe/tier1/tbhmod_flexe.h>

#define TBHMOD_DBG_IN_FUNC_INFO(pc) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNC,pc,("-22%s: Adr:%08x Ln:%02d\n", __func__, pc->addr, pc->lane_mask))
#define TBHMOD_DBG_IN_FUNC_VIN_INFO(pc,_print_) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNCVALIN,pc,_print_)
#define TBHMOD_DBG_IN_FUNC_VOUT_INFO(pc,_print_) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNCVALOUT,pc,_print_)

/**
 * @brief   Select PMD ln dp reset override bit
 * @param   pc handle to current TSCBH context (#tbhmod_st)
 * @returns The value PHYMOD_E_NONE upon successful completion
 * @details This will set PMD ln dp rest override bit.
 * */
int tbhmod_flexe_pmd_ln_dp_reset_override_enable(PHYMOD_ST* pc, int enable)
{
  PMD_X4_OVRRr_t pmd_x4_override;

  TBHMOD_DBG_IN_FUNC_INFO(pc);

  /* set the PMD ln dp reset override bit as well */
  PMD_X4_OVRRr_CLR(pmd_x4_override);
  PMD_X4_OVRRr_LN_DP_H_RSTB_OENf_SET(pmd_x4_override, enable);
  PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_OVRRr(pc, pmd_x4_override));

  return PHYMOD_E_NONE;
}

/* this function needs to be called if pcs reset is set
only forced speed will be affected by this HW issue */

int tbhmod_flexe_pcs_reset_sw_war(const PHYMOD_ST* pc)
{
    SC_X4_CTLr_t reg_sc_ctrl;
    TXFIR_MISC_CTL0r_t txfir_misc_ctrl;
    RX_X4_PCS_LATCH_STS1r_t link_status_reg;
    int start_lane, num_lane, i;
    PHYMOD_ST phy_copy;

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));


    /*first check live status, if link status is down,
    nothing is needed */
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_PCS_LATCH_STS1r(pc, &link_status_reg));

    if (RX_X4_PCS_LATCH_STS1r_PCS_LINK_STATUS_LIVEf_GET(link_status_reg)) {
        SC_X4_CTLr_CLR(reg_sc_ctrl);
        TXFIR_MISC_CTL0r_CLR(txfir_misc_ctrl);
        /* first disable all lane tx */
        TXFIR_MISC_CTL0r_SDK_TX_DISABLEf_SET(txfir_misc_ctrl, 1);
        for (i = 0; i < num_lane; i++) {
            phy_copy.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (MODIFY_TXFIR_MISC_CTL0r(&phy_copy, txfir_misc_ctrl));
        }
        /* next  toggle speed control bit */
        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 1);
        phy_copy.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(&phy_copy, reg_sc_ctrl));
        /* then sleep for 5 micro-second */
        PHYMOD_USLEEP(5);

        SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 0);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(&phy_copy, reg_sc_ctrl));
        /* then sleep for 5 micro-second */
        PHYMOD_USLEEP(5);

        /* last, disable tx_disable */
        TXFIR_MISC_CTL0r_SDK_TX_DISABLEf_SET(txfir_misc_ctrl, 0);
        for (i = 0; i < num_lane; i++) {
            phy_copy.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (MODIFY_TXFIR_MISC_CTL0r(&phy_copy, txfir_misc_ctrl));
        }
    }

    return PHYMOD_E_NONE;
}

int _tbhmod_flexe_pcs_vco_to_ui(uint32_t vco, int os_mode, int pam4)
{
    uint32_t ui_value;
    ui_value = 0;

    /* These are ideal values and when PMD is present these values are in sync with the VCos. */
    switch (vco) {
        case TBHMOD_VCO_20G:
            if (os_mode == 1) {
                ui_value = TBHMOD_FLEXE_UI_20G_NRZ_OSX2; /* 2/20.625 = 0.09696.. make 32 bit 0.09696.. * 2^32 = 416481676 = 0x18d3018c */
            } else {
                ui_value = TBHMOD_FLEXE_UI_20G_NRZ_OSX1; /* 1/20.625 = 0.04848.. make 32 bit 0.04848.. * 2^32 = 208240838 = 0xc6980c6 */
            }
            break;
        case TBHMOD_VCO_25G:
            if (pam4) {
                ui_value = TBHMOD_FLEXE_UI_25G_PAM4;     /* 1/(2*25.78125) = 0.019393939.. make 32 bit 0.019393939.. * 2^32 = 83296335 = 0x4F7004F */

            } else {
                if (os_mode == 3) {   /* os_mode 2.5 */
                    ui_value = TBHMOD_FLEXE_UI_25G_NRZ_OSX2P5; /* 2.5/25.78125 = 0.09696.. make 32 bit 0.09696.. * 2^32 = 416481676 = 0x18d3018c */
                } else {
                    ui_value = TBHMOD_FLEXE_UI_25G_NRZ_OSX1; /* 1/25.78125 = 0.038787879.. make 32 bit 0.038787879.. * 2^32 = 166592670 = 0x09ee009e */
                }
            }
            break;
        case TBHMOD_VCO_26G:
            if (pam4) {
                ui_value = TBHMOD_FLEXE_UI_26G_PAM4;   /* 1/(2*26.56250) = 0.018823529.. make 32 bit 0.018823529.. * 2^32 = 80846443 = 0x4D19E6B */
            } else {
                ui_value = TBHMOD_FLEXE_UI_26G_NRZ;   /* 1/26.56250 = 0.037647059.. make 32 bit 0.038787879.. * 2^32 = 161692886 = 0x09a33cd6 */
            }
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return ui_value;

}

/*!
 *  @brief tbhmod_flexe_pcs_set_1588_ui per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @param clk4sync_div the fast clk divider 0 => 6 and 1 => 8
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_flexe_pcs_set_1588_ui(PHYMOD_ST* pc, uint32_t vco, uint32_t vco1, int os_mode, int clk4sync_div, int pam4)
{
    PMD_X1_FCLK_PERIODr_t PMD_X1_FCLK_PERIODr_reg;
    PMD_X4_UI_VALUE_HIr_t PMD_X4_UI_VALUE_HIr_reg;
    PMD_X4_UI_VALUE_LOr_t PMD_X4_UI_VALUE_LOr_reg;
    int fclk_period = 0, ui_value_hi, ui_value_lo, ui_value_lo_m17_to_m23;


    ui_value_hi = _tbhmod_flexe_pcs_vco_to_ui(vco, os_mode, pam4) >> 16;
    ui_value_lo = _tbhmod_flexe_pcs_vco_to_ui(vco, os_mode, pam4) & 0xffff;

    /* ui_value_lo represent ui_value m17 to m32. For register programming,
     * we only need m17 to m23 value. So right shift 9 bits. */
    ui_value_lo_m17_to_m23 = ui_value_lo >> 9;


    switch(vco1) {
      case TBHMOD_VCO_20G:
        if (clk4sync_div) {
            fclk_period = TBHMOD_FLEXE_FCLK_PERIOD_20G_DIV6;
        } else {
            fclk_period = TBHMOD_FLEXE_FCLK_PERIOD_20G_DIV8;
        }
        break;
      case TBHMOD_VCO_25G:
        fclk_period = TBHMOD_FLEXE_FCLK_PERIOD_25G;
        break;
      case TBHMOD_VCO_26G:
        fclk_period = TBHMOD_FLEXE_FCLK_PERIOD_26G;
        break;
      default:
          return PHYMOD_E_PARAM;
    }


    PMD_X1_FCLK_PERIODr_CLR(PMD_X1_FCLK_PERIODr_reg);
    PMD_X4_UI_VALUE_HIr_CLR(PMD_X4_UI_VALUE_HIr_reg);
    PMD_X4_UI_VALUE_LOr_CLR(PMD_X4_UI_VALUE_LOr_reg);

    PMD_X1_FCLK_PERIODr_FCLK_FRAC_NSf_SET(PMD_X1_FCLK_PERIODr_reg, fclk_period);
    PMD_X4_UI_VALUE_HIr_UI_FRAC_M1_TO_M16f_SET(PMD_X4_UI_VALUE_HIr_reg, ui_value_hi);
    PMD_X4_UI_VALUE_LOr_UI_FRAC_M17_TO_M23f_SET(PMD_X4_UI_VALUE_LOr_reg, ui_value_lo_m17_to_m23);

    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_FCLK_PERIODr(pc, PMD_X1_FCLK_PERIODr_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_UI_VALUE_HIr(pc, PMD_X4_UI_VALUE_HIr_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_UI_VALUE_LOr(pc, PMD_X4_UI_VALUE_LOr_reg));

    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_flexe_1588_pmd_latency per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_flexe_1588_pmd_latency(PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4)
{
#ifdef SERDES_API_FLOATING_POINT
    uint32_t tx_latency;
    uint32_t ui_hi_lo;
    float bit_ui_val, bit_ui_val1, bit_ui_val2;

    PMD_X4_TX_FIXED_LATENCYr_t PMD_X4_TX_FIXED_LATENCYr_reg;
    RXCOM_OSR_MODE_STS_MC_MASKr_t RXCOM_OSR_MODE_STS_MC_MASKr_reg;

    if (an_en) {
      PHYMOD_IF_ERR_RETURN(READ_RXCOM_OSR_MODE_STS_MC_MASKr(pc, &RXCOM_OSR_MODE_STS_MC_MASKr_reg));
      os_mode = RXCOM_OSR_MODE_STS_MC_MASKr_RX_OSR_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASKr_reg);
      pam4    = RXCOM_OSR_MODE_STS_MC_MASKr_RX_PAM4_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASKr_reg);
    }

    /* ui_val with fractional bit 32 bit from _tbhmod_gen2_pcs_vco_to_ui we need 10 fractional
     * so we need 22 bit shift.
     * First shifting 8 then 14 to improve accuracy.
     */
    ui_hi_lo = _tbhmod_flexe_pcs_vco_to_ui(vco, os_mode, pam4);
    bit_ui_val1 = ((float) ui_hi_lo / 1024);
    bit_ui_val2 = ((float) bit_ui_val1 / 1024);
    bit_ui_val = ((float) bit_ui_val2 / 4096);

    /***
     * Please refer to the Blackhawk7 user spec datapath latency table
     * For PAM4 the pmd latency is based on the middle bit ie 20th bit of the 40bit interface.
     * Hence we subtract 20. Also for Pam4 each UI == 2bits hence we multiple by 2
    *****/
    if (pam4) {
        tx_latency = ((uint32_t)((bit_ui_val * ((TBHMOD_FLEXE_PMD_TX_DP_LATENCY_PAM4 * 2) - 20)) * 1024));
    } else {
        tx_latency = ((uint32_t)((bit_ui_val * (TBHMOD_FLEXE_PMD_TX_DP_LATENCY_NRZ - 20)) * 1024));  /* delay number from Blackhawk_V1.0 User Spec.pdf */
    }

      PMD_X4_TX_FIXED_LATENCYr_CLR(PMD_X4_TX_FIXED_LATENCYr_reg);


      PMD_X4_TX_FIXED_LATENCYr_TX_PMD_LATENCY_IN_NSf_SET (PMD_X4_TX_FIXED_LATENCYr_reg, (tx_latency >> 10));
      PMD_X4_TX_FIXED_LATENCYr_TX_PMD_LATENCY_IN_FRAC_NSf_SET(PMD_X4_TX_FIXED_LATENCYr_reg, (tx_latency & 0x3ff));

      PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_TX_FIXED_LATENCYr(pc, PMD_X4_TX_FIXED_LATENCYr_reg));
#endif
    return PHYMOD_E_NONE;
}

