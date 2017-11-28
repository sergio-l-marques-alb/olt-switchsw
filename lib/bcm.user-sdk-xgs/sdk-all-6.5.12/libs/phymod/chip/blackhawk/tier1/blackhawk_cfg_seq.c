/*
* $Copyright: (c) 2017 Broadcom.
* Broadcom Proprietary and Confidential. All rights reserved.$
*  $Id$
*/

/*
 *         
 * $Id: falcon.c,v 1.2.2.26 Broadcom SDK $
 * 
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *         
 *     
 */


#include <phymod/phymod.h>
#include <phymod/chip/bcmi_blackhawk_xgxs_defs.h>
#include <phymod/phymod_util.h>
#include "blackhawk_cfg_seq.h" 
#include "blackhawk_tsc_fields.h"
#include "blackhawk_tsc_field_access.h"
#include "blackhawk_tsc_dependencies.h"
#include "blackhawk_tsc_interface.h"
#include "blackhawk_tsc_functions.h"
#include "public/blackhawk_api_uc_vars_rdwr_defns_public.h"


err_code_t blackhawk_tx_rx_polarity_set( phymod_access_t *sa__, uint32_t tx_pol, uint32_t rx_pol)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err = (uint32_t) wr_tx_pmd_dp_invert(tx_pol);
  if(__err) return(__err);
  __err = (uint32_t) wr_rx_pmd_dp_invert(rx_pol);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t blackhawk_lane_pll_selection_set( phymod_access_t *sa__, uint32_t pll_index)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err = (uint32_t) wr_pll_select(pll_index);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t blackhawk_lane_pll_selection_get( phymod_access_t *sa__, uint32_t *pll_index)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;
    *pll_index = (uint32_t) rd_pll_select();
    if(__err) return(__err);

    return ERR_CODE_NONE;
}


err_code_t blackhawk_tx_rx_polarity_get( phymod_access_t *sa__, uint32_t *tx_pol, uint32_t *rx_pol)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *tx_pol = (uint32_t) rd_tx_pmd_dp_invert();
  if(__err) return(__err);
  *rx_pol = (uint32_t) rd_rx_pmd_dp_invert();
  if(__err) return(__err);
  return ERR_CODE_NONE;

}

err_code_t blackhawk_uc_active_set( phymod_access_t *sa__, uint32_t enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wrc_uc_active(enable);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t blackhawk_uc_active_get( phymod_access_t *sa__, uint32_t *enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = (uint32_t) rdc_uc_active();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}


/*
err_code_t blackhawk_uc_reset( phymod_access_t *sa__, uint32_t enable)
{
  return ERR_CODE_NONE;
}
*/

err_code_t blackhawk_force_tx_set_rst ( phymod_access_t *sa__, uint32_t rst)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_force_tx_get_rst ( phymod_access_t *sa__, uint32_t *rst)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_force_rx_set_rst ( phymod_access_t *sa__, uint32_t rst)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_force_rx_get_rst ( phymod_access_t *sa__, uint32_t *rst)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_prbs_tx_inv_data_get( phymod_access_t *sa__, uint32_t *inv_data)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_prbs_rx_inv_data_get( phymod_access_t *sa__, uint32_t *inv_data)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_prbs_tx_poly_get( phymod_access_t *sa__, blackhawk_prbs_polynomial_type_t *prbs_poly)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_prbs_rx_poly_get( phymod_access_t *sa__, blackhawk_prbs_polynomial_type_t *prbs_poly)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_prbs_tx_enable_get( phymod_access_t *sa__, uint32_t *enable)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_prbs_rx_enable_get( phymod_access_t *sa__, uint32_t *enable)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_force_signal_detect( phymod_access_t *sa__, uint8_t force_en, uint8_t force_val)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;
    __err = wr_signal_detect_frc(force_en);
    if(__err) return(__err);
    __err = wr_signal_detect_frc_val(force_val);
    if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_force_signal_detect_get( phymod_access_t *sa__, uint8_t *force_en, uint8_t *force_val)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;
    *force_en = rd_signal_detect_frc();
    if(__err) return(__err);
    *force_val = rd_signal_detect_frc_val();
    if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t blackhawk_pll_mode_set( phymod_access_t *sa__, int pll_mode)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_pll_mode_get( phymod_access_t *sa__, uint32_t *pll_mode)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_afe_pll_reg_set( phymod_access_t *sa__, const phymod_afe_pll_t *afe_pll)
{

    AMS_PLL_PLL_CTL2r_t reg;

    AMS_PLL_PLL_CTL2r_CLR(reg);

    if(afe_pll->afe_pll_change_default) {
        AMS_PLL_PLL_CTL2r_AMS_PLL_IQPf_SET(reg, afe_pll->ams_pll_iqp);
        MODIFY_AMS_PLL_PLL_CTL2r(sa__, reg);
    }     

    return ERR_CODE_NONE;
}

err_code_t blackhawk_afe_pll_reg_get( phymod_access_t *sa__, phymod_afe_pll_t *afe_pll)
{
  return ERR_CODE_NONE;
}


err_code_t blackhawk_osr_mode_set( phymod_access_t *sa__, int osr_mode)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_osr_mode_frc_val(osr_mode);
  if(__err) return(__err);
  __err=wr_osr_mode_frc(1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t blackhawk_osr_mode_get( phymod_access_t *sa__, int *osr_mode)
{
  int osr_forced;
  err_code_t __err;
  __err=ERR_CODE_NONE;
  osr_forced = rd_osr_mode_frc();
  if(osr_forced) {
    *osr_mode = rd_osr_mode_frc_val();
    if(__err) return(__err);
  } else {
    *osr_mode = rd_osr_mode_pin();
    if(__err) return(__err);
  }
  return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_dig_lpbk_get( phymod_access_t *sa__, uint32_t *lpbk)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;
    *lpbk = rd_dig_lpbk_en();
    if(__err) return(__err);
    return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_rmt_lpbk_get( phymod_access_t *sa__, uint32_t *lpbk)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;
    *lpbk = rd_rmt_lpbk_en();
    if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t blackhawk_core_soft_reset( phymod_access_t *sa__)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_core_soft_reset_release( phymod_access_t *sa__, uint32_t enable)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_core_soft_reset_read( phymod_access_t *sa__, uint32_t *enable)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_lane_soft_reset_read( phymod_access_t *sa__, uint32_t *enable)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_tx_disable_pin_dis_set( phymod_access_t *sa__, uint32_t enable)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_tx_disable_pin_dis_get( phymod_access_t *sa__, uint32_t *enable)
{
  return ERR_CODE_NONE;
}

/* set powerdown for tx or rx */
/* tx_rx == 1 => disable (enable) power for Tx */
/* tx_rx != 0 => disable (enable) power for Rx */
/* pwrdn == 0 => enable power */
/* pwrdn == 1 => disable power */
err_code_t blackhawk_tsc_pwrdn_set( phymod_access_t *sa__, int tx_rx, int pwrdn)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_pwrdn_get( phymod_access_t *sa__, power_status_t *pwrdn)
{

  return ERR_CODE_NONE;
}

err_code_t blackhawk_pcs_lane_swap_tx( phymod_access_t *sa__, uint32_t tx_lane_map)
{

  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_loopback_get( phymod_access_t *sa__, uint32_t *enable)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_cl72_enable_get( phymod_access_t *sa__, uint32_t *enable)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_cl72_receiver_status( phymod_access_t *sa__, uint32_t *status)
{
    err_code_t __err;
    __err = ERR_CODE_NONE;
    *status = rd_linktrn_ieee_receiver_status(); if(__err) return(__err);
    return ERR_CODE_NONE;
}

err_code_t blackhawk_pram_firmware_enable( phymod_access_t *sa__, int enable, int wait)   /* release the pmd core soft reset */
{
    err_code_t __err;
    uint8_t micro_orig, num_micros, micro_idx;

    __err = ERR_CODE_NONE;
    if (enable == 1) {
        __err = wrc_micro_pramif_ahb_wraddr_msw(0); if(__err) return(__err);
        __err = wrc_micro_pramif_ahb_wraddr_lsw(0); if(__err) return(__err);

        __err = wrc_micro_pram_if_rstb(1); if(__err) return(__err);
        __err = wrc_micro_pramif_en(1);    if(__err) return(__err);

        EFUN(wrc_micro_cr_crc_prtsel(0));
        EFUN(wrc_micro_cr_crc_init(1));                       /* Initialize the HW CRC calculation */
        EFUN(wrc_micro_cr_crc_init(0));
        EFUN(wrc_micro_cr_crc_calc_en(1));

        if (wait) {
          PHYMOD_USLEEP(500);
        }
    } else {
        /* block writing to program RAM */
        __err = wrc_micro_cr_ignore_micro_code_writes(1);  if(__err) return(__err);
        __err = wrc_micro_pramif_en(0); if(__err) return(__err);

        EFUN(wrc_micro_cr_crc_calc_en(0));

        EFUN(wrc_micro_core_stack_size(0x7F4));     /* Set micro stack size to 0x1FD0 for cores with 8 lanes */
        EFUN(wrc_micro_core_stack_en(1));

        micro_orig = blackhawk_tsc_get_micro_idx(sa__); 
        num_micros = rdc_micro_num_uc_cores();
        if(__err) return(__err);
	    for (micro_idx = 0; micro_idx < num_micros; micro_idx++) {
            __err = blackhawk_tsc_set_micro_idx(sa__, micro_idx);
            if(__err) return(__err);
            __err = wrc_micro_core_clk_en(1);
            if(__err) return(__err);
        }
        __err = blackhawk_tsc_set_micro_idx(sa__, micro_orig);
        if(__err) return(__err);


    }
    return ERR_CODE_NONE;
}


err_code_t blackhawk_pmd_lane_swap ( phymod_access_t *sa__, uint32_t lane_map) {

  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_lane_map_get ( phymod_access_t *sa__, uint32_t *tx_lane_map, uint32_t *rx_lane_map) {

    err_code_t __err;
    uint32_t tmp_tx_lane = 0;
    uint32_t tmp_rx_lane = 0;
    *tx_lane_map = 0;
    *rx_lane_map = 0;

    __err = ERR_CODE_NONE;
    tmp_tx_lane = rdc_tx_lane_addr_0( );
    if(__err) return(__err);
    *tx_lane_map |= tmp_tx_lane ;
    tmp_tx_lane = rdc_tx_lane_addr_1( );
    if(__err) return(__err);
    *tx_lane_map |= (tmp_tx_lane & 0xf) << (4 * 1);
    tmp_tx_lane = rdc_tx_lane_addr_2( );
    if(__err) return(__err);
    *tx_lane_map |= (tmp_tx_lane & 0xf) << (4 * 2);
    tmp_tx_lane = rdc_tx_lane_addr_3( );
    if(__err) return(__err);
    *tx_lane_map |= (tmp_tx_lane & 0xf) << (4 * 3);
    tmp_tx_lane = rdc_tx_lane_addr_4( );
    if(__err) return(__err);
    *tx_lane_map |= (tmp_tx_lane & 0xf) << (4 * 4);
    tmp_tx_lane = rdc_tx_lane_addr_5( );
    if(__err) return(__err);
    *tx_lane_map |= (tmp_tx_lane  & 0xf) << (4 * 5);
    tmp_tx_lane = rdc_tx_lane_addr_6( );
    if(__err) return(__err);
    *tx_lane_map |= (tmp_tx_lane  & 0xf) << (4 * 6);
    tmp_tx_lane = rdc_tx_lane_addr_7( );
    if(__err) return(__err);
    *tx_lane_map |= (tmp_tx_lane  & 0xf) << (4 * 7);

    tmp_rx_lane = rdc_rx_lane_addr_0( );
    if(__err) return(__err);
    *rx_lane_map |= (tmp_rx_lane & 0xf);
    tmp_rx_lane = rdc_rx_lane_addr_1( );
    if(__err) return(__err);
    *rx_lane_map |= (tmp_rx_lane & 0xf) << (4 * 1);
    tmp_rx_lane = rdc_rx_lane_addr_2( );
    if(__err) return(__err);
    *rx_lane_map |= (tmp_rx_lane & 0xf) << (4 * 2);
    tmp_rx_lane = rdc_rx_lane_addr_3( );
    if(__err) return(__err);
    *rx_lane_map |= (tmp_rx_lane & 0xf) << (4 * 3);
    tmp_rx_lane = rdc_rx_lane_addr_4( );
    if(__err) return(__err);
    *rx_lane_map |= (tmp_rx_lane & 0xf) << (4 * 4);
    tmp_rx_lane = rdc_rx_lane_addr_5( );
    if(__err) return(__err);
    *rx_lane_map |= (tmp_rx_lane  & 0xf) << (4 * 5);
    tmp_rx_lane = rdc_rx_lane_addr_6( );
    if(__err) return(__err);
    *rx_lane_map |= (tmp_rx_lane  & 0xf) << (4 * 6);
    tmp_rx_lane = rdc_rx_lane_addr_7( );
    if(__err) return(__err);
    *rx_lane_map |= (tmp_rx_lane  & 0xf) << (4 * 7);

    __err = ERR_CODE_NONE;

  return ERR_CODE_NONE;
}

err_code_t blackhawk_tx_pi_control_get( phymod_access_t *sa__, int16_t* value)
{
    err_code_t __err;
    uint8_t override_enable;

    __err = ERR_CODE_NONE;

    override_enable = rd_tx_pi_freq_override_en();
    if(__err) return(__err);
    if (override_enable) {
        *value = rd_tx_pi_freq_override_val();
        if(__err) return(__err);
    } else {
        *value = 0;
    }
    return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_identify( phymod_access_t *sa__, blackhawk_rev_id0_t *rev_id0, blackhawk_rev_id1_t *rev_id1)
{
  err_code_t __err;

  rev_id0->revid_rev_letter =0;
  rev_id0->revid_rev_number =0;
  rev_id0->revid_bonding    =0;
  rev_id0->revid_process    =0;
  rev_id0->revid_model      =0;

  rev_id1->revid_multiplicity =0;
  rev_id1->revid_mdio         =0;
  rev_id1->revid_micro        =0;
  rev_id1->revid_cl72         =0;
  rev_id1->revid_pir          =0;
  rev_id1->revid_llp          =0;
  rev_id1->revid_eee          =0;

  __err=ERR_CODE_NONE;

  rev_id0->revid_rev_letter =rdc_revid_rev_letter(); if(__err) return(__err);
  rev_id0->revid_rev_number =rdc_revid_rev_number(); if(__err) return(__err);
  rev_id0->revid_bonding    =rdc_revid_bonding();    if(__err) return(__err);
  rev_id0->revid_process    =rdc_revid_process();    if(__err) return(__err);
  rev_id0->revid_model      =rdc_revid_model();      if(__err) return(__err);

  rev_id1->revid_multiplicity =rdc_revid_multiplicity(); if(__err) return(__err);
  rev_id1->revid_mdio         =rdc_revid_mdio();         if(__err) return(__err);
  rev_id1->revid_micro        =rdc_revid_micro();        if(__err) return(__err);
  rev_id1->revid_cl72         =rdc_revid_cl72();         if(__err) return(__err);
  rev_id1->revid_pir          =rdc_revid_pir();          if(__err) return(__err);
  rev_id1->revid_llp          =rdc_revid_llp();          if(__err) return(__err);
  rev_id1->revid_eee          =rdc_revid_eee();          if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t blackhawk_pmd_ln_h_rstb_pkill_override(  phymod_access_t *sa__, uint16_t val)
{
    err_code_t __err;
    /* 
    * Work around per Magesh/Justin
    * override input from PCS to allow uc_dsc_ready_for_cmd 
    * reg get written by UC
    */ 
    __err = ERR_CODE_NONE;
    __err = wr_pmd_ln_h_rstb_pkill(val); 
    if(__err) return(__err);
    return ERR_CODE_NONE;
}

err_code_t blackhawk_lane_soft_reset( phymod_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
    int i, start_lane, num_lane;
    uint32_t reset_enable;
    phymod_access_t phy_access_copy;
    RXTXCOM_LN_CLK_RST_N_PWRDWN_CTLr_t reg;

    PHYMOD_MEMCPY(&phy_access_copy, sa__, sizeof(phy_access_copy));
    RXTXCOM_LN_CLK_RST_N_PWRDWN_CTLr_CLR(reg);

    if (enable) {
        reset_enable = 0;
    } else {
        reset_enable = 1;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(sa__, &start_lane, &num_lane));
    for (i = 0; i < num_lane; i++) {
        phy_access_copy.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(sa__->lane_mask, start_lane + i)) {
            continue;
        }
        RXTXCOM_LN_CLK_RST_N_PWRDWN_CTLr_LN_DP_S_RSTBf_SET(reg, reset_enable);
        MODIFY_RXTXCOM_LN_CLK_RST_N_PWRDWN_CTLr(&phy_access_copy, reg);

    }
    return ERR_CODE_NONE;
}

err_code_t blackhawk_lane_soft_reset_get( phymod_access_t *sa__, uint32_t *enable)   /* release the pmd core soft reset */
{
    err_code_t __err;
    uint32_t data;

    __err = ERR_CODE_NONE;

    data = rd_ln_dp_s_rstb();
    if(__err) return(__err);
    if (data) {
        *enable = 0;
    } else {
        *enable = 1;
    }
    return ERR_CODE_NONE;
}


err_code_t blackhawk_lane_hard_soft_reset_release( phymod_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
    err_code_t __err;
    __err = ERR_CODE_NONE;
    __err = wr_ln_s_rstb(enable);
    if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t blackhawk_clause72_control( phymod_access_t *sa__, uint32_t cl_72_en)                /* CLAUSE_72_CONTROL */
{
    err_code_t __err;

    __err = ERR_CODE_NONE;

    if (cl_72_en) {
        __err = wr_linktrn_ieee_training_enable(1);
        if(__err) return(__err);
    } else {
        __err = wr_linktrn_ieee_training_enable(0);
        if(__err) return(__err);
    }

    return ERR_CODE_NONE;
}

err_code_t blackhawk_clause72_control_get( phymod_access_t *sa__, uint32_t *cl_72_en)                /* CLAUSE_72_CONTROL */
{
    err_code_t __err;
    __err = ERR_CODE_NONE;
    *cl_72_en = rd_linktrn_ieee_training_enable();
    if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t blackhawk_channel_loss_set( phymod_access_t *sa__, uint32_t loss_in_db)
{
    err_code_t __err;
    __err = ERR_CODE_NONE;
     __err = wrv_blackhawk_tsc_usr_ctrl_pam4_chn_loss(sa__, loss_in_db);
    if(__err) return(__err);
    return ERR_CODE_NONE;
}

err_code_t blackhawk_channel_loss_get( phymod_access_t *sa__, uint32_t *loss_in_db)
{
    err_code_t __err;
    __err = ERR_CODE_NONE;
    *loss_in_db = rdv_blackhawk_tsc_usr_ctrl_pam4_chn_loss(sa__);
    if(__err) return(__err);
    return ERR_CODE_NONE;
}



err_code_t blackhawk_electrical_idle_set( phymod_access_t *sa__, uint8_t en)
{
    AMS_TX_TX_CTL2r_t reg;

    AMS_TX_TX_CTL2r_CLR(reg);
    AMS_TX_TX_CTL2r_AMS_TX_ELEC_IDLE_AUXf_SET(reg, en);
    MODIFY_AMS_TX_TX_CTL2r(sa__, reg);

    return ERR_CODE_NONE;
}

err_code_t blackhawk_electrical_idle_get( phymod_access_t *sa__, uint8_t *en)
{
    AMS_TX_TX_CTL2r_t reg;

    READ_AMS_TX_TX_CTL2r(sa__, &reg);
    *en = AMS_TX_TX_CTL2r_AMS_TX_ELEC_IDLE_AUXf_GET(reg);

    return ERR_CODE_NONE;
}


/***********************************************/
/*  Microcode Init into Program RAM Functions  */
/***********************************************/

/* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t blackhawk_tsc_ucode_init(  phymod_access_t *sa__ )
{
    err_code_t __err;
    uint8_t result;
    __err = ERR_CODE_NONE;

    __err = wrc_micro_master_clk_en(0x1); if(__err) return(__err); /* Enable clock to microcontroller subsystem */
    __err = wrc_micro_master_rstb(0x1); if(__err) return(__err); /* De-assert reset to microcontroller sybsystem */
    __err = wrc_micro_master_rstb(0x0); if(__err) return(__err); /* Assert reset to microcontroller sybsystem - Toggling reset*/
    __err = wrc_micro_master_rstb(0x1); if(__err) return(__err); /* De-assert reset to microcontroller sybsystem */
    __err = wrc_micro_cr_access_en(1); if(__err) return(__err); /* allow access to Code RAM */

    __err = wrc_micro_ra_init(0x1); if(__err) return(__err); /* Set initialization command to initialize code RAM */

    PHYMOD_USLEEP(1000);

    result =  rdc_micro_ra_initdone();  /* Poll for micro_ra_initdone = 1 to indicate initialization done */
    if (__err) return(__err); 
    if (!result) {                                         /* Check if initialization done within 500us time interval */
        PHYMOD_DEBUG_ERROR(("ERR_CODE_MICRO_INIT_NOT_DONE\n"));
        return (ERR_CODE_MICRO_INIT_NOT_DONE);    /* Else issue error code */
    }

    __err = wrc_micro_ra_init(0x0); if(__err) return(__err); /* Clear initialization command */

    __err = wrc_micro_cr_crc_prtsel(0); if(__err) return(__err);
    __err = wrc_micro_cr_prif_prtsel(0); if(__err) return(__err);
    __err = wrc_micro_cr_crc_init(2); if(__err) return(__err);    /* initialize the HW CRC calculation */
    __err = wrc_micro_cr_crc_init(1); if(__err) return(__err);
    __err = wrc_micro_cr_ignore_micro_code_writes(0); if(__err) return(__err);  /* allow writing to program RAM */


    return (ERR_CODE_NONE);
}

/**
@brief   Init the PMD
@param   pmd_touched If the PMD is already initialized
@returns The value ERR_CODE_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
err_code_t blackhawk_pmd_reset_seq( phymod_access_t *sa__, int pmd_touched)
{
    err_code_t __err;
    __err = ERR_CODE_NONE;

   if (pmd_touched == 0) {
         __err = wrc_core_s_rstb(1); if(__err) return(__err);
  }
    return (ERR_CODE_NONE);
}


/**
@brief   Enable the pll reset bit
@param   enable Controls whether to reset PLL
@returns The value ERR_CODE_NONE upon successful completion
@details
Resets the PLL
*/
err_code_t blackhawk_pll_reset_enable_set ( phymod_access_t *sa__, int enable)
{
    return (ERR_CODE_NONE);
}

/**
@brief   Read PLL range
*/
err_code_t blackhawk_tsc_read_pll_range ( phymod_access_t *sa__, uint32_t *pll_range)
{
  return (ERR_CODE_NONE);
}


/**
@brief   Reag signal detect
*/
err_code_t blackhawk_tsc_signal_detect( phymod_access_t *sa__, uint32_t *signal_detect)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;
    *signal_detect = rd_signal_detect();
    if(__err) return(__err);
    return (ERR_CODE_NONE);
}


err_code_t blackhawk_tsc_ladder_setting_to_mV( phymod_access_t *sa__, int8_t y, int16_t* level)
{

  return(ERR_CODE_NONE);
}


err_code_t blackhawk_tsc_get_vco ( phymod_phy_inf_config_t* config, uint32_t *vco_rate, uint32_t *new_pll_div, int16_t *new_os_mode)
{
  return(ERR_CODE_NONE);
}

/* Get Enable/Disable Shared TX pattern generator */
err_code_t blackhawk_tsc_tx_shared_patt_gen_en_get(  phymod_access_t *sa__, uint8_t *enable) {
    err_code_t __err;
    __err=ERR_CODE_NONE;
    *enable = rd_patt_gen_en();
    if(__err) return(__err);
    return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_config_shared_tx_pattern_idx_set(  phymod_access_t *sa__,  uint32_t *sa__ttern_len)
{
  return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_config_shared_tx_pattern_idx_get(  phymod_access_t *sa__, uint32_t *sa__ttern_len, uint32_t *sa__ttern) {
  return ERR_CODE_NONE;
}

/**********************************/
/*  Serdes TX disable/RX Restart  */
/**********************************/

err_code_t blackhawk_tsc_tx_disable_get ( phymod_access_t *sa__, uint8_t *enable)
{
    err_code_t __err;

    __err=ERR_CODE_NONE;
    *enable = (uint8_t) rd_sdk_tx_disable();
    if(__err) return(__err);
	return ERR_CODE_NONE;
}

err_code_t blackhawk_refclk_set( phymod_access_t *sa__, phymod_ref_clk_t ref_clock)
{
    DIG_TOP_USER_CTL0r_t dig_top_user_ctrl_reg;
    DIG_TOP_USER_CTL0r_CLR(dig_top_user_ctrl_reg);

    switch (ref_clock) {
        case phymodRefClk156Mhz:
            DIG_TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_ctrl_reg, 0x271);
            break;
        case phymodRefClk125Mhz:
            DIG_TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_ctrl_reg, 0x1f4);
            break;
        case phymodRefClk312Mhz:
            DIG_TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_ctrl_reg, 0x271);
            break;
        default:
            DIG_TOP_USER_CTL0r_HEARTBEAT_COUNT_1USf_SET(dig_top_user_ctrl_reg, 0x271);
            break;
    }
    MODIFY_DIG_TOP_USER_CTL0r(sa__, dig_top_user_ctrl_reg);

	return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_tx_nrz_mode_get(phymod_access_t *sa__, uint16_t *tx_nrz_mode)
{
    TXFIR_TAP_CTL0r_t reg;

    READ_TXFIR_TAP_CTL0r(sa__, &reg);
    *tx_nrz_mode = TXFIR_TAP_CTL0r_TXFIR_NRZ_TAP_RANGE_SELf_GET(reg);
	return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_signalling_mode_status_get( phymod_access_t *sa__, phymod_phy_signalling_method_t *mode)
{
    RXTXCOM_OSR_MODE_STS_MC_MASKr_t reg;
    uint16_t temp_data = 0;

    READ_RXTXCOM_OSR_MODE_STS_MC_MASKr(sa__, &reg);
    temp_data = RXTXCOM_OSR_MODE_STS_MC_MASKr_PAM4_MODEf_GET(reg);
    if (temp_data) {
        *mode = phymodSignallingMethodPAM4;
    } else {
        *mode = phymodSignallingMethodNRZ;
    }
	return ERR_CODE_NONE;
}


err_code_t blackhawk_tsc_tx_tap_mode_get( phymod_access_t *sa__, uint8_t *mode)
{
    err_code_t __err;

    __err=ERR_CODE_NONE;
    *mode = (uint8_t) rd_txfir_tap_en();
    if(__err) return(__err);
	return ERR_CODE_NONE;
}


err_code_t blackhawk_tsc_pam4_tx_pattern_enable_get( phymod_access_t *sa__, phymod_PAM4_tx_pattern_t pattern_type, uint32_t* enable)
{
    err_code_t __err;

    __err=ERR_CODE_NONE;
    *enable = (uint8_t) rd_patt_gen_en();
    if(__err) return(__err);
    if (*enable) {
        switch (pattern_type) {
        case phymod_PAM4TxPattern_JP03B:
            *enable = rd_pam4_tx_jp03b_patt_en();
             if(__err) return(__err);
             break;
        case phymod_PAM4TxPattern_Linear:
            *enable = rd_pam4_tx_linearity_patt_en();
             if(__err) return(__err);
             break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported PAM4 tx pattern  %u"), pattern_type));
        }
    }
	return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_tx_pam4_precoder_enable_set(phymod_access_t *sa__, int enable)
{
    err_code_t __err;

    __err = ERR_CODE_NONE;

    __err = wr_pam4_precoder_en(enable);
    if(__err) return(__err);
    return ERR_CODE_NONE;
}

err_code_t blackhawk_tsc_tx_pam4_precoder_enable_get(phymod_access_t *sa__, int *enable)
{
    err_code_t __err;
    __err = ERR_CODE_NONE;

    *enable = rd_pam4_precoder_en();
    if(__err) return(__err);
    return ERR_CODE_NONE;
}

