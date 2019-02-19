/*
 *         
 * $Id: furia_cfg_seq.c 2014/04/02 palanivk Exp $
 * 
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 *
 */

/* 
 * Includes
 */
#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
/* Implementation of the interface defined here */
#include "falcon_furia_src/falcon_furia_internal.h"
#include "falcon_furia_src/falcon_furia_functions.h"
#include "furia_cfg_seq.h"
#include "furia_reg_access.h"
#include "furia_address_defines.h"
/*
 *  Defines
 */
#define PHYMOD_INTERFACE_SIDE_SHIFT        31

/* 
 *  Types
 */

/*
 *  Macros
 */
/*
 *  Global Variables
 */

/*
 *  Functions
 */
int _furia_get_ref_clock_freq_in_mhz(FURIA_REF_CLK_E ref_clk_freq);
static int _furia_get_pll_divider(FALCON_PLL_MODE_E pll_mode);
static uint16_t _furia_get_link_type(phymod_interface_t intf, uint32_t speed);
static uint16_t _furia_get_phy_type(phymod_interface_t intf);
static int _furia_config_clk_scaler_val(const phymod_access_t *pa, FURIA_REF_CLK_E ref_clk_freq);
static int _furia_phymod_to_furia_type_ref_clk_freq(phymod_ref_clk_t ref_clk, FURIA_REF_CLK_E *ref_clk_freq);
static int _furia_get_pll_mode(phymod_ref_clk_t ref_clk, uint32_t speed, FALCON_PLL_MODE_E *pll_mode);
static int furia_get_pkg_idx(uint32_t chip_id, int *pkg_idx);
static int _furia_config_clk_scaler_without_m0_reset(const phymod_access_t *pa, FURIA_REF_CLK_E ref_clk_freq);
int _furia_fw_enable(const phymod_access_t* pa);
/*
 * Functions for Manipulating Chip/Port Cfg Descriptors
 */

/*
 * Functions for Programming the Chip
 */

/**   Get Revision ID 
 *    This function retrieves Revision ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param rev_id            Revision ID retrieved from the chip
 */
int furia_rev_id(const phymod_access_t *pa, uint32_t *rev_id) 
{
    FUR_MISC_CTRL_CHIP_REVISION_t chip_revision;
    PHYMOD_MEMSET(&chip_revision, 0 , sizeof(FUR_MISC_CTRL_CHIP_REVISION_t));

    /* Read the chip revision from register */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_chip_revision_Adr,\
                                &chip_revision.data)); 
    *rev_id = chip_revision.fields.chip_rev;
    return PHYMOD_E_NONE;
}

/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return chip_id           Chip id retrieved from the chip
 */
uint32_t _furia_get_chip_id(const phymod_access_t *pa) 
{
    uint32_t chip_id = 0;
    FUR_MISC_CTRL_CHIP_REVISION_t chip_revision;
    FUR_MISC_CTRL_CHIP_ID_t chip_id_t;
    PHYMOD_MEMSET(&chip_revision, 0 , sizeof(FUR_MISC_CTRL_CHIP_REVISION_t));
    PHYMOD_MEMSET(&chip_id_t, 0 , sizeof(FUR_MISC_CTRL_CHIP_ID_t));

    /* Read upper bits of chip id */ 
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_chip_revision_Adr,\
                                &chip_revision.data)); 
    chip_id = chip_revision.fields.chip_id_19_16;

    /* Read lower 16 bits of chip id */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_chip_id_Adr,\
                                &chip_id_t.data)); 
    chip_id = ( (chip_id << 16) | 
                 (chip_id_t.fields.chip_id_15_0));

    return(chip_id);
}


/**   Reset Chip 
 *    This function is used to reset entire chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_chip_reset (const phymod_access_t *pa) 
{
    FUR_GEN_CNTRLS_GEN_CONTROL1_t gen_cntrl1;
    PHYMOD_MEMSET(&gen_cntrl1, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL1_t));
   
    /* Read General control1 Reg */ 
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
         FUR_GEN_CNTRLS_gen_control1_Adr,\
         &gen_cntrl1.data));

    /* update bit field for hard reset */
    gen_cntrl1.fields.resetb = 0;

    /* Write to General control1 Reg */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
         FUR_GEN_CNTRLS_gen_control1_Adr,\
         gen_cntrl1.data));
    return PHYMOD_E_NONE;
}	
	
/**   Reset Register 
 *    This function is used to perform register reset 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_register_rst (const phymod_access_t *pa) 
{
    FUR_GEN_CNTRLS_GEN_CONTROL1_t gen_cntrl1;
    PHYMOD_MEMSET(&gen_cntrl1, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL1_t));

    /* Read General control1 Reg */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control1_Adr,\
                                &gen_cntrl1.data));

    /* Update bit field for register reset */ 
    gen_cntrl1.fields.reg_rstb = 0;

    /* Write to General control1 Reg */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control1_Adr,\
                                 gen_cntrl1.data));
    return PHYMOD_E_NONE;
}

/**   Configure PRBS generator 
 *    This function is used to configure PRBS generator with user provided
 *    polynomial and invert data information 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param prbs_mode          User specified polynomial
 *                              0 - PRBS7
 *                              1 - PRBS9
 *                              2 - PRBS11    
 *                              3 - PRBS15
 *                              4 - PRBS23
 *                              5 - PRBS31
 *                              6 - PRBS58
 * 
 *    @param prbs_inv           User specified invert data config
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_config_set(const phymod_access_t *pa,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum prbs_mode,
                        uint32_t prbs_inv)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
                PHYMOD_IF_ERR_RETURN(falcon_furia_config_tx_prbs(prbs_mode, (uint8_t)prbs_inv)); 
            }
            if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                PHYMOD_IF_ERR_RETURN(falcon_furia_config_rx_prbs(prbs_mode, PRBS_INITIAL_SEED_HYSTERESIS, (uint8_t)prbs_inv)); 
            }
            if(lane_map == 0xF) {
                break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;  
}

/**   Get PRBS generator configuration 
 *    This function is used to retrieve PRBS generator configuration from 
 *    the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param prbs_mode          Configured Polynomial retrieved from chip 
 *    @param prbs_inv           Configured invert data retrieved from chip
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_config_get(const phymod_access_t *pa,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum *prbs_mode,
                        uint32_t *prbs_inv)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t chip_id = 0;
    uint32_t acc_flags = 0;
    uint8_t prbs_invert = 0;
    enum srds_prbs_checker_mode_enum prbs_checker_mode = 0; 
    *prbs_inv = 0;
    *prbs_mode = 0; 

    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);

    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            if (pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA; 
            } else {
                sys_en = pkg_ln_des->sideB;
            } 
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
                PHYMOD_IF_ERR_RETURN(falcon_furia_get_tx_prbs_config(prbs_mode, &prbs_invert));
                *prbs_inv = prbs_invert;
            }
            if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                PHYMOD_IF_ERR_RETURN(falcon_furia_get_rx_prbs_config(prbs_mode, &prbs_checker_mode, &prbs_invert));
                *prbs_inv = prbs_invert;
            }
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;  
}


/**   Get PRBS lock and error status 
 *    This function is used to retrieve PRBS lock, loss of lock and error counts
 *    from the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param lock_status        PRBS lock status denotes PRBS generator and 
 *                              checker are locked to same polynomial data
 *    @param lock_loss          Loss of lock denotes PRBS generator and checker
 *                              are not in sync   
 *    @param error_count        PRBS error count retrieved from chip
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_status_get(const phymod_access_t *pa,
                                uint32_t *lock_status,
                                uint32_t *lock_loss,
                                uint32_t *error_count)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0; 
    uint32_t acc_flags = 0;
    uint8_t chk_lock = 0;  
    uint8_t loss_of_lock = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t per_lane_err_count = 0;
    *lock_status = 1;
    *lock_loss = 1;
    *error_count = 0;
    
     

    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);

    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
         
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN(falcon_furia_prbs_chk_lock_state(&chk_lock));
            *lock_status &= chk_lock;
            PHYMOD_IF_ERR_RETURN(falcon_furia_prbs_err_count_state(&per_lane_err_count, &loss_of_lock));
            *error_count |= per_lane_err_count;
            *lock_loss &= loss_of_lock;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

/**   Enable PRBS generator and PRBS checker 
 *    This function is used to enable or disable PRBS generator and PRBS checker
 *    as requested by the user  
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param enable              Enable or disable as specified by the user
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_enable_set(const phymod_access_t *pa, uint32_t flags, uint32_t enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint8_t ena_dis = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    TLB_RX_PRBS_CHK_CONFIG_t prbs_chk_config;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t acc_flags = 0; 
    PHYMOD_MEMSET(&prbs_chk_config, 0 , sizeof(TLB_RX_PRBS_CHK_CONFIG_t));
    
    ena_dis = enable ? 1 : 0;


 
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);

    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
       
            if(lane_map == 0xf) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
           
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                if(enable) {
                    /* Read PRBS Checker Control Register */
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                TLB_RX_prbs_chk_config_Adr,\
                                                &prbs_chk_config.data));

                    /* Update the field PRBS checker clock enable. */
                    prbs_chk_config.fields.prbs_chk_clk_en_frc_on = 1;

                    /* Write to PRBS Checker Control Register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 TLB_RX_prbs_chk_config_Adr,\
                                                 prbs_chk_config.data));
                }
                PHYMOD_IF_ERR_RETURN(falcon_furia_rx_prbs_en(ena_dis));
 
                if(!enable) {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                TLB_RX_prbs_chk_config_Adr,\
                                                &prbs_chk_config.data));

                    /* Update the field PRBS checker clock enable. */
                    prbs_chk_config.fields.prbs_chk_clk_en_frc_on = 0;

                    /* Write to PRBS Checker Control Register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 TLB_RX_prbs_chk_config_Adr,\
                                                 prbs_chk_config.data));
                }
            }

            if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
                PHYMOD_IF_ERR_RETURN(falcon_furia_tx_prbs_en(ena_dis));
            }

            if(lane_map == 0xf) {
                break;
            } 
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

/**   Get Enable status of PRBS generator and PRBS checker 
 *    This function is used to retrieve the enable status of PRBS generator and
 *    PRBS checker
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param enable              Enable or disable read from chip 
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_enable_get(const phymod_access_t *pa, uint32_t flags, uint32_t *enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint8_t gen_en = 0;
    uint8_t chk_en = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    gen_en = 0;
    chk_en = 0; 
    *enable = 0; 
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);

    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
        
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;


            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
                PHYMOD_IF_ERR_RETURN(falcon_furia_get_tx_prbs_en(&gen_en));
                *enable = gen_en;
            }

            if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                PHYMOD_IF_ERR_RETURN(falcon_furia_get_rx_prbs_en(&chk_en));
                *enable = chk_en;
                break;
            }
        }
    }
    /* Set the slice register to default */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    if(flags == 0) {
        if(gen_en && chk_en) {
            *enable = 1;
        } else {
            *enable = 0;
        }
    }
   
    return PHYMOD_E_NONE;
}

/**   Get  link status of PHY 
 *    This function is used to retrieve the link status of PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param link_status        link status of the PHY 
 *                              1 - Up 
 *                              0 - Down 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_link_status(const phymod_access_t *pa,
                                   uint32_t *link_status) 
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint8_t rx_pmd_lock = 0;
    uint32_t fcpcs_lock = 1;
    uint16_t fcpcs_chkr_mode = 0;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t tmp_main_ctrl_mode;
    SYS_RX_PMA_DP_MAIN_CTRL_t sys_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_LIVE_STATUS_FC_MODE_t          line_rx_pma_dp_live_status_fc_mode;
    LINE_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t         line_rx_pma_dp_live_status_pcs_mode;
    SYS_RX_PMA_DP_LIVE_STATUS_FC_MODE_t          sys_rx_pma_dp_live_status_fc_mode;
    SYS_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t         sys_rx_pma_dp_live_status_pcs_mode;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&tmp_main_ctrl_mode, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_live_status_fc_mode, 0 , sizeof(LINE_RX_PMA_DP_LIVE_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_live_status_pcs_mode, 0 , sizeof(LINE_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_live_status_fc_mode, 0 , sizeof(SYS_RX_PMA_DP_LIVE_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_live_status_pcs_mode, 0 , sizeof(SYS_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t));
    
    *link_status = 1;
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);

    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            if (FURIA_IS_DUPLEX(chip_id)) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_pmd_lock_status(&rx_pmd_lock));
            if(sys_en == LINE) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_RX_PMA_DP_main_ctrl_Adr,\
                                            &line_rx_dp_main_ctrl.data));
                tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
            } else {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_RX_PMA_DP_main_ctrl_Adr,\
                                            &sys_rx_dp_main_ctrl.data)); 
                tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
            }
            if(!tmp_main_ctrl_mode.fields.link_mon_en) {
                *link_status &= rx_pmd_lock;
            } else {
                if(sys_en == LINE) {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                LINE_RX_PMA_DP_live_status_Adr,\
                                                &line_rx_pma_dp_live_status_pcs_mode.data));
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                LINE_RX_PMA_DP_live_status_Adr,\
                                                &line_rx_pma_dp_live_status_fc_mode.data));
                } else {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                SYS_RX_PMA_DP_live_status_Adr,\
                                                &sys_rx_pma_dp_live_status_pcs_mode.data));
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                SYS_RX_PMA_DP_live_status_Adr,\
                                                &sys_rx_pma_dp_live_status_fc_mode.data));
                }

                if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x0)                    {
                    fcpcs_chkr_mode = PCS49_1x10G;
                }
                if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x1) {
                    if(tmp_main_ctrl_mode.fields.mode_50g) {
                        fcpcs_chkr_mode = PCS82_2x25G;
                    } else if(tmp_main_ctrl_mode.fields.mode_100g) {
                        fcpcs_chkr_mode = PCS82_4x25G;
                    } else {
                        fcpcs_chkr_mode = PCS82_4x10G;
                    }
                } 
                if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x2) {
                    fcpcs_chkr_mode = FC4;
                }
                if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x3) {
                    fcpcs_chkr_mode = FC8;
                }

                switch (fcpcs_chkr_mode) {
                    case PCS49_1x10G :
                    case PCS82_4x10G :
                    case FC16 :
                    case FC32 :
                    case PCS82_2x25G :
                    case PCS82_4x25G :
                        if(sys_en == LINE) {
                            fcpcs_lock &= line_rx_pma_dp_live_status_pcs_mode.fields.pcs_status;
                        } else {
                            fcpcs_lock &= sys_rx_pma_dp_live_status_pcs_mode.fields.pcs_status;
                        }
                    break;
                    case FC4 :
                    case FC8 :
                        if(sys_en == LINE) {
                            fcpcs_lock = line_rx_pma_dp_live_status_fc_mode.fields.wsyn_link_fail;
                        } else {
                            fcpcs_lock = sys_rx_pma_dp_live_status_fc_mode.fields.wsyn_link_fail;
                        }
                    break;
                }
                *link_status &= fcpcs_lock;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

/**   Set config mode 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf               Interface specified by user 
 *    @param speed              Speed val as specified by user  
 *    @param ref_clk            Reference clock frequency to set 
 *                              the PHY into specified interface 
 *                              and speed
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_set_config_mode(const phymod_access_t *pa,
                       phymod_interface_t intf,
                       uint32_t speed,
                       phymod_ref_clk_t ref_clk)
{
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_t an_ctrl_reg;
    FUR_MISC_CTRL_UDMS_PHY_t udms_phy;
    FUR_MISC_CTRL_UDMS_LINK_t udms_link;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    SYS_RX_PMA_DP_MAIN_CTRL_t sys_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t tmp_main_ctrl_mode;
    uint16_t phy_type;
    uint16_t link_type;
    int lane_map = 0;
    int lane_map_saved = 0;
    int lane_index = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    uint32_t acc_flags = 0;
    int pkg_side = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    FURIA_REF_CLK_E ref_clk_freq = REF_CLK_INVALID;
    FALCON_PLL_MODE_E pll_mode = PLL_MODE_INVALID;
    phymod_interface_t actual_intf = 0;
    uint32_t actual_speed = 0;
    phymod_ref_clk_t actual_ref_clk;
    uint32_t actual_interface_modes = 0;
    uint32_t phy_id = 0;
    uint8_t fw_enable = 0;
    uint8_t retry_count = 5;
    phymod_access_t l_pa;
    uint32_t curr_intf[MAX_NUM_LANES] = {0,};
    uint32_t curr_speed[MAX_NUM_LANES] = {0,};
    int wr_lane = 0;
    int rd_lane = 0;
    int sys_en = 0;
    PHYMOD_MEMSET(&an_ctrl_reg, 0 , sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_t));
    PHYMOD_MEMSET(&udms_phy, 0 , sizeof(FUR_MISC_CTRL_UDMS_PHY_t));
    PHYMOD_MEMSET(&udms_link, 0 , sizeof(FUR_MISC_CTRL_UDMS_LINK_t));
    PHYMOD_MEMSET(&fw_enable_reg, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&tmp_main_ctrl_mode, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t));
    /* Get a copy of phymod access */
    PHYMOD_MEMCPY(&l_pa, pa, sizeof(phymod_access_t));
    /* Get CHIP ID */
    chip_id = _furia_get_chip_id(pa);
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    lane_map_saved = lane_map;
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    phy_id = pa->addr;

    if(pkg_side == SIDE_B) {
        return PHYMOD_E_UNAVAIL;
    }
    /* Wait for firmware to ready before config to be changed */
    PHYMOD_IF_ERR_RETURN
         (READ_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                 &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;


     while((fw_enable != 0) && (retry_count)) {
         PHYMOD_IF_ERR_RETURN
             (READ_FURIA_PMA_PMD_REG(pa,\
                                     FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                     &fw_enable_reg.data));
         fw_enable = fw_enable_reg.fields.fw_enable_val;
         PHYMOD_SLEEP(1);
         retry_count--;
     }

     if(!retry_count) {
         PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("1:config failed, micro controller is busy..")));
     }
 

    /* Read current mode configurations from registers */ 
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        l_pa.lane_mask = 0x1 << lane_index;
        PHYMOD_IF_ERR_RETURN
            (furia_get_config_mode(&l_pa, &actual_intf, &actual_speed, &actual_ref_clk, &actual_interface_modes));
        curr_intf[lane_index] = actual_intf;
        curr_speed[lane_index] = actual_speed;
    }

    if ((curr_speed[0] == SPEED_10G || curr_speed[0] == SPEED_20G) ||
        (curr_speed[1] == SPEED_10G || curr_speed[1] == SPEED_20G) ||
        (curr_speed[2] == SPEED_10G || curr_speed[2] == SPEED_20G) ||
        (curr_speed[3] == SPEED_10G || curr_speed[3] == SPEED_20G)) {
        if((speed == SPEED_25G || speed == SPEED_50G) && (lane_map != BROADCAST)) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Unsupported mode")));
        }
    }


    if ((curr_speed[0] == SPEED_25G || curr_speed[0] == SPEED_50G) ||
        (curr_speed[1] == SPEED_25G || curr_speed[1] == SPEED_50G) ||
        (curr_speed[2] == SPEED_25G || curr_speed[2] == SPEED_50G) ||
        (curr_speed[3] == SPEED_25G || curr_speed[3] == SPEED_50G)) {
        if((speed == SPEED_10G || speed == SPEED_20G) && (lane_map != BROADCAST)) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Unsupported mode")));
        }
    }

    if(((speed == SPEED_100G || speed == SPEED_40G) && 
         (intf == phymodInterfaceSR4 || intf == phymodInterfaceKR4 || intf == phymodInterfaceCR4 || intf == phymodInterfaceLR4)) &&
         (lane_map != BROADCAST)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Wrong Lanemap param")));
    }
    
    if((speed == SPEED_50G || speed == SPEED_20G || speed == SPEED_40G) &&
       (intf == phymodInterfaceKR2) && 
       (lane_map != MULTICAST01 && lane_map != MULTICAST23 && lane_map != BROADCAST)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Wrong Lanemap param")));
    } 
    
    /*perform phymod to furia enum conversion */
    _furia_phymod_to_furia_type_ref_clk_freq(ref_clk, &ref_clk_freq);
    /*Set the ref_clk freq before programming phy type and link type */ 
    _furia_config_clk_scaler_without_m0_reset(pa, ref_clk_freq);
    phy_type = _furia_get_phy_type(intf);
    link_type = _furia_get_link_type(intf, speed);


    /* Disable autonegotiation and PCS monitor on all the PHY lanes */
    lane_map = 0xF;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            /* Force AN disable since mode is configured using UDMS*/ 
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        IEEE_AN_BLK0_an_control_register_Adr,\
                                        &an_ctrl_reg.data));
           /* Update auto_negotiationenable bit field */
           an_ctrl_reg.fields.auto_negotiationenable = 0;

           /* Write to AN control register */
           PHYMOD_IF_ERR_RETURN
               (WRITE_FURIA_PMA_PMD_REG(pa,\
                                        IEEE_AN_BLK0_an_control_register_Adr,\
                                        an_ctrl_reg.data));
           if (FURIA_IS_DUPLEX(chip_id)) {
               if(pkg_side == SIDE_A || pkg_side == SIDE_B) {
                   if(sys_en == LINE) {
                       PHYMOD_IF_ERR_RETURN
                           (READ_FURIA_PMA_PMD_REG(pa,\
                                                   LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                   &line_rx_dp_main_ctrl.data));
                       tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                       tmp_main_ctrl_mode.fields.link_mon_en = 0;
                       PHYMOD_IF_ERR_RETURN
                           (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    tmp_main_ctrl_mode.data));
                   } else {
                       PHYMOD_IF_ERR_RETURN
                           (READ_FURIA_PMA_PMD_REG(pa,\
                                                   SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                   &sys_rx_dp_main_ctrl.data)); 
                       tmp_main_ctrl_mode.data = sys_rx_dp_main_ctrl.data; 
                       tmp_main_ctrl_mode.fields.link_mon_en = 0;
                       PHYMOD_IF_ERR_RETURN
                           (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    tmp_main_ctrl_mode.data)); 
                   }
               }
           } else {
                if(pkg_side == SIDE_B) {
                    if(sys_en == LINE) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    &line_rx_dp_main_ctrl.data)); 
                        tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                        tmp_main_ctrl_mode.fields.link_mon_en = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                     tmp_main_ctrl_mode.data)); 
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    &sys_rx_dp_main_ctrl.data)); 
                        tmp_main_ctrl_mode.data = sys_rx_dp_main_ctrl.data; 
                        tmp_main_ctrl_mode.fields.link_mon_en = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                     tmp_main_ctrl_mode.data)); 
                    }
                }
           }
           if(lane_map == 0xF) {
               break;
           }
        }
    }
    
    /* Program udms_en=1 */
    /* Read UDMS PHY Type Register */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_udms_phy_Adr,\
                                &udms_phy.data));
 
    /* Update the field udms enable */
    udms_phy.fields.udms_en = 1;
    /* Update udms_phy_type */ 
    udms_phy.fields.udms_phy_type = phy_type;

    /* Write to UDMS PHY Type Register */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_udms_phy_Adr,\
                                 udms_phy.data));

    /* Read the link type Register*/
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_udms_link_Adr,\
                                &udms_link.data));

    if(
       (curr_intf[0] == phymodInterfaceKR4 &&
        curr_intf[1] == phymodInterfaceKR4 &&
        curr_intf[2] == phymodInterfaceKR4 &&
        curr_intf[3] == phymodInterfaceKR4) ||
       (curr_intf[0] == phymodInterfaceCR4 &&
        curr_intf[1] == phymodInterfaceCR4 &&
        curr_intf[2] == phymodInterfaceCR4 &&
        curr_intf[3] == phymodInterfaceCR4) ||
       (curr_intf[0] == phymodInterfaceLR4 &&
        curr_intf[1] == phymodInterfaceLR4 &&
        curr_intf[2] == phymodInterfaceLR4 &&
        curr_intf[3] == phymodInterfaceLR4) ||
       (curr_intf[0] == phymodInterfaceSR4 &&
        curr_intf[1] == phymodInterfaceSR4 &&
        curr_intf[2] == phymodInterfaceSR4 &&
        curr_intf[3] == phymodInterfaceSR4)
       ) {
        lane_map = 0xF; 
    } else {
        lane_map = lane_map_saved;
    }

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Update the field udms enable */
            if (pkg_ln_des->die_lane_num == 0) {
                udms_link.fields.udms_ln0_lnk_type = link_type;
            }
            if (pkg_ln_des->die_lane_num == 1) {
                udms_link.fields.udms_ln1_lnk_type = link_type;
            }
            if (pkg_ln_des->die_lane_num == 2) {
                udms_link.fields.udms_ln2_lnk_type = link_type;
            }
            if (pkg_ln_des->die_lane_num == 3) {
                udms_link.fields.udms_ln3_lnk_type = link_type;
            }
            /* Update the software database */
            INTERFACE(phy_id, lane_index) = (uint32_t)intf;
        }
    }
    /* Program the Link Type Register with link type*/
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_udms_link_Adr,\
                                 udms_link.data));
    /* Set the PLL value based on ref clock and speed */ 
    _furia_get_pll_mode(ref_clk, speed, &pll_mode);
   

    _furia_config_pll_div(pa, pll_mode, ref_clk_freq);

#if 0
    /*Wait for fw_enable to go low  before setting fw_enable_val*/
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;

    while((fw_enable != 0) && (retry_count)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                    &fw_enable_reg.data));
        fw_enable = fw_enable_reg.fields.fw_enable_val;
        retry_count--; 
    }

    if(!retry_count) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("config failed, micro controller is busy..")));
    }
#endif
    retry_count = 5;  
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                &fw_enable_reg.data));
    fw_enable_reg.fields.fw_enable_val = 1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                 fw_enable_reg.data));
    /*Wait for fw_enable to go low */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;

    while((fw_enable != 0) && (retry_count)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                    &fw_enable_reg.data));
        fw_enable = fw_enable_reg.fields.fw_enable_val;
        PHYMOD_SLEEP(1);
        retry_count--; 
    }

    if(!retry_count) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("config failed, micro controller is busy..")));
    }

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            l_pa.lane_mask = 0x1 << lane_index;
            PHYMOD_IF_ERR_RETURN
                (furia_get_config_mode(&l_pa, &actual_intf, &actual_speed, &actual_ref_clk, &actual_interface_modes));


            if((intf == actual_intf) &&
               (speed == actual_speed) &&
               (ref_clk == actual_ref_clk)) {
                INTERFACE(phy_id, lane_index) = actual_intf;
            } else {
                /* Restore the configurations before returning from here */
                INTERFACE(phy_id, lane_index) = curr_intf[lane_index];
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Set config failed")));
            }
        }
    }
    return PHYMOD_E_NONE;
}
 
/**   Get config mode 
 *    This function is used to retrieve the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf               Interface type 
 *    @param speed              Speed val retrieved from PHY 
 *    @param ref_clk            Reference clock 
 *    @param interface_modes    Supported interface modes
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_get_config_mode(const phymod_access_t *pa,
                       phymod_interface_t *intf,
                       uint32_t *speed,
                       phymod_ref_clk_t *ref_clk,
                       uint32_t *interface_modes)
{
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_t an_ctrl_reg;
    FUR_MISC_CTRL_PHY_TYPE_STATUS_t udms_phy;
    FUR_MISC_CTRL_LINK_TYPE_STATUS_t udms_link;
    FUR_GEN_CNTRLS_GEN_CONTROL3_t gen_ctrl_3;
    furia_an_ability_t an_ability; 
    uint32_t logic_to_phy_ln0_map = 0; 
    uint16_t master_lane_num = 0;
    int lane_map = 0;
    uint16_t phy_type = 0;
    uint16_t link_type = 0;
    uint32_t phy_id = 0;
    uint16_t saved_intf = 0;
    uint16_t ref_clk_scaler_val = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    int lane_index = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    *speed = 0;
    *intf = 0;
    *ref_clk = 0; 
    PHYMOD_MEMSET(&an_ctrl_reg, 0 , sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_t));
    PHYMOD_MEMSET(&udms_phy, 0 , sizeof(FUR_MISC_CTRL_PHY_TYPE_STATUS_t));
    PHYMOD_MEMSET(&udms_link, 0 , sizeof(FUR_MISC_CTRL_LINK_TYPE_STATUS_t));
    PHYMOD_MEMSET(&gen_ctrl_3, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL3_t));
    PHYMOD_MEMSET(&an_ability, 0 , sizeof(furia_an_ability_t));

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                IEEE_AN_BLK0_an_control_register_Adr,\
                                &an_ctrl_reg.data));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    phy_id = pa->addr;

    
    /* Get the link type and phy type from udms reg if AN is not on */ 
    if (!an_ctrl_reg.fields.auto_negotiationenable) {
        /* Read UDMS PHY Type Register */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_MISC_CTRL_phy_type_status_Adr,\
                                    &udms_phy.data));
        /* Read the link type Register*/
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_MISC_CTRL_link_type_status_Adr,\
                                    &udms_link.data));
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
                PHYMOD_NULL_CHECK(pkg_ln_des);
                /* Update the field udms enable */
                if (pkg_ln_des->die_lane_num == 0) {
                    phy_type = udms_phy.fields.ln0_phy_status;
                    link_type = udms_link.fields.ln0_lnk_status;
                }
                if (pkg_ln_des->die_lane_num == 1) {
                    phy_type = udms_phy.fields.ln1_phy_status;
                    link_type = udms_link.fields.ln1_lnk_status;
                }
                if (pkg_ln_des->die_lane_num == 2) {
                    phy_type = udms_phy.fields.ln2_phy_status;
                    link_type = udms_link.fields.ln2_lnk_status;
                }
                if (pkg_ln_des->die_lane_num == 3) {
                    phy_type = udms_phy.fields.ln3_phy_status;
                    link_type = udms_link.fields.ln3_lnk_status;
                }
                /* Get params from software database */
                saved_intf = INTERFACE(phy_id, lane_index);
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN
            (_furia_autoneg_ability_get(pa, &an_ability));
       
        if((an_ability.cl73_adv.an_base_speed == FURIA_CL73_100GBASE_KR4) ||
           (an_ability.cl73_adv.an_base_speed == FURIA_CL73_100GBASE_CR4)||
           (an_ability.cl73_adv.an_base_speed == FURIA_CL73_40GBASE_KR4) ||
           (an_ability.cl73_adv.an_base_speed == FURIA_CL73_40GBASE_CR4)) {    
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa, 0x8a08,  &logic_to_phy_ln0_map));
            master_lane_num = logic_to_phy_ln0_map & 0x3;
            switch(master_lane_num) {
                case 0:
                    link_type = udms_link.fields.ln0_lnk_status;
                break;
                case 1:
                    link_type = udms_link.fields.ln1_lnk_status;
                break;
                case 2:
                    link_type = udms_link.fields.ln2_lnk_status;
                break;
                case 3:
                    link_type = udms_link.fields.ln3_lnk_status;
                break;
            }
        } else {
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    switch(pkg_ln_des->die_lane_num) {
                        case 0:
                            link_type = udms_link.fields.ln0_lnk_status;
                        break;
                        case 1:
                            link_type = udms_link.fields.ln1_lnk_status;
                        break;
                        case 2:
                            link_type = udms_link.fields.ln2_lnk_status;
                        break;
                        case 3:
                            link_type = udms_link.fields.ln3_lnk_status;
                        break;
                    }
                }
            }
        }
        /* Get params from software database */
        saved_intf = INTERFACE(phy_id, lane_index);
    }

    
    switch(phy_type) {
        case 0x0:
            switch(link_type) {
                case 0x1:
                    *speed = SPEED_1G;
                    *intf = phymodInterfaceKX;
                break;
                case 0x2:
                    *speed = SPEED_10G;
                    *intf = phymodInterfaceKR;
                break;
                case 0x3:
                    *speed = SPEED_40G;
                    *intf = phymodInterfaceKR4;
                break;
                case 0x4:
                    *speed = SPEED_100G;
                    *intf = phymodInterfaceKR4;
                break;
                case 0x5:
                     *speed = SPEED_20G;
                     *intf = phymodInterfaceKR2;
                break;
                case 0x6:
                    *speed = SPEED_20G;
                    *intf = phymodInterfaceKR;
                break;
                case 0x7:
                    *speed = SPEED_40G;
                    *intf = phymodInterfaceKR2;
                break;
                case 0x8:
                    *speed = SPEED_25G;
                    *intf = phymodInterfaceKR;
                break;
                case 0x9:
                    *speed = SPEED_50G;
                    *intf = phymodInterfaceKR2;
                break;
                default:
                break;

           }
        break;
        case 0x1:
            switch(link_type) {
                case 0x1:
                    if((saved_intf == phymodInterfaceSR) ||
                        (saved_intf == phymodInterfaceLR)) {
                        *speed = SPEED_10G;
                        *intf = saved_intf;
                    }
                break;
                case 0x2:
                    *speed = SPEED_40G;
                    *intf = phymodInterfaceCR4;
                break;
                case 0x3:
                    if((saved_intf == phymodInterfaceSR4) ||
                        (saved_intf == phymodInterfaceLR4)) {
                        *speed = SPEED_40G;
                        *intf = saved_intf;
                    }
                break;
                case 0x4:
                    *speed = SPEED_100G;
                    *intf = phymodInterfaceLR4;
                break;
                case 0x5:
                    *speed = SPEED_100G;
                    *intf = phymodInterfaceCR4;
                break;
                case 0x6:
                    *speed = SPEED_20G;
                    *intf = phymodInterfaceLR;
                break;
                case 0x7:
                    *speed = SPEED_25G;
                    *intf = phymodInterfaceLR;
                break;
                default:
                break;
            }
        break;
        default:
        break;
    } 

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl_3.data));

      if(gen_ctrl_3.fields.clock_scaler_frc) {
          ref_clk_scaler_val = gen_ctrl_3.fields.clock_scaler_frcval;
      } else {
          ref_clk_scaler_val = gen_ctrl_3.fields.clock_scaler_code;
      }

    switch(ref_clk_scaler_val) {
        case 0x0:
            *ref_clk = phymodRefClk174Mhz;
        break;
        case 0x1:
            *ref_clk = phymodRefClk161Mhz;
        break;
        case 0x2:
            *ref_clk = phymodRefClk156Mhz;
        break;
        case 0x3:
            *ref_clk = phymodRefClk125Mhz;
        break;
        case 0x4:
            *ref_clk = phymodRefClk106Mhz;
        break;
        default:
        break;
    }
    *interface_modes = 0;

    return PHYMOD_E_NONE;
}


/**   Config PLL divider 
 *    This function is used to configure PLL divider 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param pll_mode           PLL mode  
 *    @param ref_clk_freq       Ref clk freq type 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_config_pll_div(const phymod_access_t *pa, FALCON_PLL_MODE_E pll_mode, FURIA_REF_CLK_E ref_clk_freq)
{
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    int side_index = 0;
    /* For Falcon speedup */
    int vco_stime   = 5;
    int pfreq_dtime = 100;
    int retry       = 100;
    int rescal_cntr = 0;
    int vco_freq = 0;
    PLL_CAL_COM_CTL_7_t pll_ctrl_7;
    AMS_COM_PLL_CONTROL_1_t ams_pll_ctrl_1;
    PLL_CAL_COM_CTL_5_t pll_ctrl_5;
    PLL_CAL_COM_CTL_0_t pll_ctrl_0;
    PLL_CAL_COM_CTL_1_t pll_ctrl_1;
    PLL_CAL_COM_CTL_2_t pll_ctrl_2;  
    PHYMOD_MEMSET(&pll_ctrl_7, 0 , sizeof(PLL_CAL_COM_CTL_7_t));
    PHYMOD_MEMSET(&ams_pll_ctrl_1, 0 , sizeof(AMS_COM_PLL_CONTROL_1_t));
    PHYMOD_MEMSET(&pll_ctrl_5, 0 , sizeof(PLL_CAL_COM_CTL_5_t));
    PHYMOD_MEMSET(&pll_ctrl_0, 0 , sizeof(PLL_CAL_COM_CTL_0_t));
    PHYMOD_MEMSET(&pll_ctrl_1, 0 , sizeof(PLL_CAL_COM_CTL_1_t));
    PHYMOD_MEMSET(&pll_ctrl_2, 0 , sizeof(PLL_CAL_COM_CTL_2_t));

    /* Get vco_freq */
    vco_freq = _furia_get_pll_divider(pll_mode) * _furia_get_ref_clock_freq_in_mhz((FURIA_REF_CLK_E)ref_clk_freq);
    vco_freq /= 100;
    for(side_index = 0; side_index < 2; side_index ++) {
        /* Set PLL div */
        wr_lane = 1 << 0;
        rd_lane = 0;
        if(side_index == 0) {
            sys_en = SIDE_A;
        } else {
            sys_en = SIDE_B;
        } 
        /* Program the slice register */
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

        /* Program pll_mode */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    PLL_CAL_COM_CTL_7_Adr,\
                                    &pll_ctrl_7.data));
        pll_ctrl_7.fields.pll_mode = pll_mode;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     PLL_CAL_COM_CTL_7_Adr,\
                                     pll_ctrl_7.data));

        /* enable VCO2 - 15-18G */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    AMS_COM_PLL_CONTROL_1_Adr,\
                                    &ams_pll_ctrl_1.data));
        if(vco_freq < 19000) {
            ams_pll_ctrl_1.fields.ams_pll_vco2_15g = 1;
        } else {
            ams_pll_ctrl_1.fields.ams_pll_vco2_15g = 0;
        }
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     AMS_COM_PLL_CONTROL_1_Adr,\
                                     ams_pll_ctrl_1.data));

        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    PLL_CAL_COM_CTL_5_Adr,\
                                    &pll_ctrl_5.data));
        pll_ctrl_5.fields.refclk_divcnt = 6250;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     PLL_CAL_COM_CTL_5_Adr,\
                                     pll_ctrl_5.data));

        /* Added for simulations speed ups Micro Arch Document Falcon Core Startup Seq */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    PLL_CAL_COM_CTL_0_Adr,\
                                    &pll_ctrl_0.data));
        pll_ctrl_0.fields.vco_start_time = vco_stime;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     PLL_CAL_COM_CTL_0_Adr,\
                                     pll_ctrl_0.data));
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    PLL_CAL_COM_CTL_1_Adr,\
                                    &pll_ctrl_1.data));
        pll_ctrl_1.fields.pre_freq_det_time = pfreq_dtime;
        pll_ctrl_1.fields.retry_time = retry;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     PLL_CAL_COM_CTL_1_Adr,\
                                     pll_ctrl_1.data));
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    PLL_CAL_COM_CTL_2_Adr,\
                                    &pll_ctrl_2.data));
        pll_ctrl_2.fields.res_cal_cntr = rescal_cntr;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     PLL_CAL_COM_CTL_2_Adr,\
                                     pll_ctrl_2.data));
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE; 
}

/**   Set Tx Rx polarity 
 *    This function is used to set Tx Rx polarity of a single lane
 *    or multiple lanes 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param tx_polarity        Tx polarity 
 *    @param rx_polarity        Rx polarity 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_tx_rx_polarity_set (const phymod_access_t *pa, uint32_t tx_polarity, uint32_t rx_polarity)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    TLB_RX_TLB_RX_MISC_CONFIG_t rx_misc_config;
    TLB_TX_TLB_TX_MISC_CONFIG_t tx_misc_config;
    PHYMOD_MEMSET(&rx_misc_config, 0 , sizeof(TLB_RX_TLB_RX_MISC_CONFIG_t));
    PHYMOD_MEMSET(&tx_misc_config, 0 , sizeof(TLB_TX_TLB_TX_MISC_CONFIG_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
 
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            /* Read Tx misc config register */
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        TLB_TX_tlb_tx_misc_config_Adr,\
                                        &tx_misc_config.data));

            /* Update bit fields for polarity inversion */
            tx_misc_config.fields.tx_pmd_dp_invert = tx_polarity;
                
            /* Write to Tx misc config register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         TLB_TX_tlb_tx_misc_config_Adr,\
                                         tx_misc_config.data));
            /* Read Rx misc config register */
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        TLB_RX_tlb_rx_misc_config_Adr,\
                                        &rx_misc_config.data));

            /* Update bit fields for polarity inversion */
            rx_misc_config.fields.rx_pmd_dp_invert = rx_polarity;
                
            /* Write to Rx misc config register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         TLB_RX_tlb_rx_misc_config_Adr,\
                                         rx_misc_config.data));
            if(lane_map == 0xF) {
                break;
            }
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));

    return PHYMOD_E_NONE;
}

/**   Get Tx Rx polarity 
 *    This function is used to get Tx Rx polarity of a specific lane
 *    specified by user 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param tx_polarity        Tx polarity 
 *    @param rx_polarity        Rx polarity 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_tx_rx_polarity_get (const phymod_access_t *pa, uint32_t *tx_polarity, uint32_t *rx_polarity)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    TLB_RX_TLB_RX_MISC_CONFIG_t rx_misc_config;
    TLB_TX_TLB_TX_MISC_CONFIG_t tx_misc_config;
    PHYMOD_MEMSET(&rx_misc_config, 0 , sizeof(TLB_RX_TLB_RX_MISC_CONFIG_t));
    PHYMOD_MEMSET(&tx_misc_config, 0 , sizeof(TLB_TX_TLB_TX_MISC_CONFIG_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    *tx_polarity = 0;
    *rx_polarity = 0;
   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
  
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            /* Read Tx misc config register */
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        TLB_TX_tlb_tx_misc_config_Adr,\
                                        &tx_misc_config.data));

            /* Update bit fields for polarity inversion */
            *tx_polarity = tx_misc_config.fields.tx_pmd_dp_invert ;
                
            /* Read Rx misc config register */
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        TLB_RX_tlb_rx_misc_config_Adr,\
                                        &rx_misc_config.data));

            /* Update bit fields for polarity inversion */
            *rx_polarity = rx_misc_config.fields.rx_pmd_dp_invert;
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));

    return PHYMOD_E_NONE;
}

/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param rx_seq_done        RX sew done status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_pmd_lock_get(const phymod_access_t *pa, uint32_t *rx_seq_done) 
{
    PHYMOD_IF_ERR_RETURN
        (furia_link_status(pa, rx_seq_done));
    return PHYMOD_E_NONE;
}


/**  Tx Rx power get 
 *    
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param power_tx           TX Power 
 *    @param power_rx           RX Power 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_tx_rx_power_get(const phymod_access_t *pa, uint32_t *power_tx, uint32_t *power_rx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t rst_pwdn_ctrl;
    uint32_t acc_flags = 0; 
    *power_tx = 0;
    *power_rx = 0;
    PHYMOD_MEMSET(&rst_pwdn_ctrl, 0 , sizeof(CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                        &rst_pwdn_ctrl.data));
            *power_tx = rst_pwdn_ctrl.fields.ln_tx_s_pwrdn ? 0 : 1;
            *power_rx = rst_pwdn_ctrl.fields.ln_rx_s_pwrdn ? 0 : 1 ;
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


/**  Rx Power Set 
 *    
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param tx_rx              TX_RX Value
 *    @param enable             Set 1 to enable 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_tx_rx_power_set(const phymod_access_t *pa, uint8_t tx_rx, uint32_t enable) 
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t rst_pwdn_ctrl;
    uint32_t acc_flags = 0; 
    PHYMOD_MEMSET(&rst_pwdn_ctrl, 0 , sizeof(CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    enable = enable ? 0 : 1;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            /*Put the lane under the reset */
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                        &rst_pwdn_ctrl.data));
            if(tx_rx) { 
                rst_pwdn_ctrl.fields.ln_rx_s_pwrdn = enable;
            } else { 
                rst_pwdn_ctrl.fields.ln_tx_s_pwrdn = enable;
            }
 
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                         rst_pwdn_ctrl.data)); 

        }
        if(lane_map == 0xF) {
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}
int furia_loopback_set(const phymod_access_t *pa,
                       phymod_loopback_mode_t loopback,
                       uint32_t enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int other_sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t acc_flags = 0;
    int retry_count = 5;
    DSC_C_RX_PI_CONTROL_t pi_ctrl; 
    DSC_C_CDR_CONTROL_2_t cd_ctrl_2;
    TX_PI_RPTR_TX_PI_CONTROL_0_t pi_ctrl_0;
    TX_PI_RPTR_TX_PI_CONTROL_5_t pi_ctrl_5;
    TLB_RX_DIG_LPBK_CONFIG_t dig_lpbk_cfg;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable;
    PHYMOD_MEMSET(&pi_ctrl, 0 , sizeof(DSC_C_RX_PI_CONTROL_t));
    PHYMOD_MEMSET(&cd_ctrl_2, 0 , sizeof(DSC_C_CDR_CONTROL_2_t));
    PHYMOD_MEMSET(&pi_ctrl_0, 0 , sizeof(TX_PI_RPTR_TX_PI_CONTROL_0_t));
    PHYMOD_MEMSET(&pi_ctrl_5, 0 , sizeof(TX_PI_RPTR_TX_PI_CONTROL_5_t));
    PHYMOD_MEMSET(&dig_lpbk_cfg, 0 , sizeof(TLB_RX_DIG_LPBK_CONFIG_t));
    PHYMOD_MEMSET(&fw_enable, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            other_sys_en = (sys_en == SIDE_A) ? SIDE_B : SIDE_A;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            switch (loopback) {
                case phymodLoopbackGlobal:
                    return PHYMOD_E_UNAVAIL;
                break;
                case phymodLoopbackRemotePMD:
                    if(enable) {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_rmt_lpbk_from_nl());
                    } else {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_nl_from_rmt_lpbk());
                    }
                break;
                case phymodLoopbackRemotePCS:
                    return PHYMOD_E_UNAVAIL;
                break;
                case phymodLoopbackGlobalPMD:
                    if(enable) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));

                        while(fw_enable.fields.fw_enable_val != 0 && retry_count){
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                        &fw_enable.data));
                            PHYMOD_SLEEP(1);
                            retry_count --;
                        } 
                        if(!retry_count) {
                            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Digital loopback configuration failed...")));
                        }  
                        /* Enable digital loopback */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_dig_lpbk_config_Adr,\
                                                    &dig_lpbk_cfg.data));
                        dig_lpbk_cfg.fields.dig_lpbk_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_dig_lpbk_config_Adr,\
                                                     dig_lpbk_cfg.data));
                        /* Set firmware enable */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));
                        fw_enable.fields.fw_enable_val = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                     fw_enable.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));

                        while(fw_enable.fields.fw_enable_val != 0 && retry_count){
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                        &fw_enable.data));
                            PHYMOD_SLEEP(1);
                            retry_count --;
                        } 
                        if(!retry_count) {
                            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Digital loopback configuration failed...")));
                        }  
                        /* Enable digital loopback */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_dig_lpbk_config_Adr,\
                                                    &dig_lpbk_cfg.data));
                        dig_lpbk_cfg.fields.dig_lpbk_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_dig_lpbk_config_Adr,\
                                                     dig_lpbk_cfg.data));
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));
                        while(fw_enable.fields.fw_enable_val != 0 && retry_count){
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                        &fw_enable.data));

                            PHYMOD_SLEEP(1);
                            retry_count--;
                        } 
                        if(!retry_count) {
                            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Digital loopback configuration failed...")));
                        }  
                        /* Program Other Falcon (one which is not doing digital loopback) */
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, other_sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_C_cdr_control_2_Adr,\
                                                    &cd_ctrl_2.data));
                        cd_ctrl_2.fields.tx_pi_loop_timing_src_sel = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     DSC_C_cdr_control_2_Adr,\
                                                     cd_ctrl_2.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                    &pi_ctrl_5.data));
                        pi_ctrl_5.fields.tx_pi_repeater_mode_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                     pi_ctrl_5.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 0;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));
                        /* Program Falcon (one which is doing digital loopback) */
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                    &pi_ctrl_5.data));
                        pi_ctrl_5.fields.tx_pi_repeater_mode_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                     pi_ctrl_5.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 0;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_dig_lpbk_config_Adr,\
                                                    &dig_lpbk_cfg.data));
                        dig_lpbk_cfg.fields.dig_lpbk_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_dig_lpbk_config_Adr,\
                                                     dig_lpbk_cfg.data));
                        /* Set firmware enable */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));
                        fw_enable.fields.fw_enable_val = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                     fw_enable.data));
                    }
                break;
                default :
                break;
            }

        }
        if(lane_map == 0xF) {
            break;
        }
    }


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            other_sys_en = (sys_en == SIDE_A) ? SIDE_B : SIDE_A;

            switch (loopback) {
                case phymodLoopbackGlobal:
                    return PHYMOD_E_UNAVAIL;
                break;
                case phymodLoopbackRemotePMD:
                    if(enable) {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_rmt_lpbk_from_nl());
                    } else {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_nl_from_rmt_lpbk());
                    }
                break;
                case phymodLoopbackRemotePCS:
                    return PHYMOD_E_UNAVAIL;
                break;
                case phymodLoopbackGlobalPMD:
                    if(enable) {
                        /* Program Other Falcon (one which is not doing digital loopback) */
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, other_sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_C_rx_pi_control_Adr,\
                                                    &pi_ctrl.data));
                        pi_ctrl.fields.rx_pi_manual_mode = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     DSC_C_rx_pi_control_Adr,\
                                                     pi_ctrl.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_C_cdr_control_2_Adr,\
                                                    &cd_ctrl_2.data));
                        cd_ctrl_2.fields.tx_pi_loop_timing_src_sel = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     DSC_C_cdr_control_2_Adr,\
                                                     cd_ctrl_2.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 0;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));

                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                    &pi_ctrl_5.data));
                        pi_ctrl_5.fields.tx_pi_repeater_mode_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                     pi_ctrl_5.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 1;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));

                        /* Program Falcon (one which is doing digital loopback) */
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                    &pi_ctrl_5.data));
                        pi_ctrl_5.fields.tx_pi_repeater_mode_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                     pi_ctrl_5.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 1;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));

                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_dig_lpbk_config_Adr,\
                                                    &dig_lpbk_cfg.data));
                        dig_lpbk_cfg.fields.dig_lpbk_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_dig_lpbk_config_Adr,\
                                                     dig_lpbk_cfg.data));
                        PHYMOD_USLEEP(50);
                    }
                break;
                default :
                break;
            }

        }
        if(lane_map == 0xF) {
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_loopback_get(const phymod_access_t *pa,
                       phymod_loopback_mode_t loopback,
                       uint32_t* enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint8_t ena_dis = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    falcon_furia_pm_acc_set(pa);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
  
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                switch (loopback) {
                    case phymodLoopbackGlobal :
                        return PHYMOD_E_UNAVAIL;
                    break;
                    case phymodLoopbackRemotePMD :
                        PHYMOD_IF_ERR_RETURN(falcon_furia_rmt_lpbk_get(&ena_dis));
                        *enable = ena_dis;
                    break;
                    case phymodLoopbackRemotePCS :
                        return PHYMOD_E_UNAVAIL;
                    break;
                    case phymodLoopbackGlobalPMD :
                        PHYMOD_IF_ERR_RETURN(falcon_furia_dig_lpbk_get(&ena_dis));
                        *enable = ena_dis;
                    break;
                    default :
                    break;
                }
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_reset_set(const phymod_access_t *pa,
                    phymod_reset_mode_t reset_mode,
                    phymod_reset_direction_t direction)
{
    if((reset_mode == phymodResetModeSoft) &&
       (direction == phymodResetDirectionIn)) { 
        PHYMOD_IF_ERR_RETURN
            (_furia_chip_reset(pa));
    }
    return PHYMOD_E_NONE;
}

int furia_tx_set(const phymod_access_t *pa,
                 const phymod_tx_t* tx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t acc_flags = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(TX_AFE_PRE, tx->pre));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(TX_AFE_MAIN, tx->main));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(TX_AFE_POST1, tx->post));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(TX_AFE_POST2, tx->post2));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(TX_AFE_POST3, tx->post3));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(TX_AFE_AMP, tx->amp));
        }
        if(lane_map == 0xF) {
            break;
        }
    }

    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_tx_get(const phymod_access_t *pa,
                 phymod_tx_t* tx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
  
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_read_tx_afe(TX_AFE_PRE, &tx->pre));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_read_tx_afe(TX_AFE_MAIN, &tx->main));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_read_tx_afe(TX_AFE_POST1, &tx->post));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_read_tx_afe(TX_AFE_POST2, &tx->post2));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_read_tx_afe(TX_AFE_POST3, &tx->post3));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_read_tx_afe(TX_AFE_AMP, &tx->amp));
            
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}
int furia_rx_set(const phymod_access_t *pa,
                 const phymod_rx_t* rx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t acc_flags = 0; 
    uint32_t i = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            /*params check*/
            if(rx->num_of_dfe_taps > PHYMOD_NUM_DFE_TAPS){
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set")));
            }
            /*vga set*/
            if (rx->vga.enable) {
                /* first stop the rx adaption */
                PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(1));
                PHYMOD_IF_ERR_RETURN(falcon_furia_write_rx_afe(RX_AFE_VGA, rx->vga.value));
            } else {
                PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(0));
            }

            /*dfe set*/
            for (i = 0 ; i < PHYMOD_NUM_DFE_TAPS; i++){
                if(rx->dfe[i].enable){
                    PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(1));
                    PHYMOD_IF_ERR_RETURN(falcon_furia_write_rx_afe(RX_AFE_DFE1+i, rx->dfe[i].value));
                } else {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(0));
                }
            }

            /*peaking filter set*/
            if(rx->peaking_filter.enable){
                /* first stop the rx adaption */
                PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(1));
                PHYMOD_IF_ERR_RETURN(falcon_furia_write_rx_afe(RX_AFE_PF, rx->peaking_filter.value));
            } else {
                PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(0));
            }

            if(rx->low_freq_peaking_filter.enable){
                /* first stop the rx adaption */
                PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(1));
                PHYMOD_IF_ERR_RETURN(falcon_furia_write_rx_afe(RX_AFE_PF2, rx->low_freq_peaking_filter.value));
            } else {
                PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(0));
            }
            if(lane_map == 0xF) {
                break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


int furia_rx_get(const phymod_access_t *pa,
                 phymod_rx_t* rx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t i = 0;
    int8_t dfe;
    int8_t vga;
    int8_t pf;
    int8_t low_freq_pf;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
  
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

           
            rx->num_of_dfe_taps = PHYMOD_NUM_DFE_TAPS; 
            PHYMOD_IF_ERR_RETURN(falcon_furia_read_rx_afe(RX_AFE_VGA, &vga));
            rx->vga.value = vga;

            /*dfe get*/
            for (i = 0 ; i < PHYMOD_NUM_DFE_TAPS; i++){
                PHYMOD_IF_ERR_RETURN(falcon_furia_read_rx_afe((RX_AFE_DFE1+i), &dfe));
                rx->dfe[i].value = dfe;
            }

            /*peaking filter get*/
            PHYMOD_IF_ERR_RETURN(falcon_furia_read_rx_afe(RX_AFE_PF, &pf));
            rx->peaking_filter.value = pf;
            PHYMOD_IF_ERR_RETURN(falcon_furia_read_rx_afe(RX_AFE_PF2, &low_freq_pf));
            rx->low_freq_peaking_filter.value = low_freq_pf;
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


int furia_tx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_tx_lane_control_t tx_control)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    uint16_t die_lane_num = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    SYS_TX_PMA_DP_SOFT_RST_t sys_tx_dp_soft_rst;
    LINE_TX_PMA_DP_SOFT_RST_t line_tx_dp_soft_rst; 
    FUR_GEN_CNTRLS_gpreg12_t gen_ctrls_gpreg12;
    FUR_GEN_CNTRLS_gpreg11_t gen_ctrls_gpreg11;
    PHYMOD_MEMSET(&sys_tx_dp_soft_rst, 0 , sizeof(SYS_TX_PMA_DP_SOFT_RST_t));
    PHYMOD_MEMSET(&line_tx_dp_soft_rst, 0 , sizeof(LINE_TX_PMA_DP_SOFT_RST_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            die_lane_num = pkg_ln_des->die_lane_num;
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
               sys_en = pkg_ln_des->sideB;
            }
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            switch(tx_control) {
                case phymodTxReset:
                    /* Asserting datapath reset */
                    if (sys_en == LINE) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_TX_PMA_DP_soft_rst_Adr,\
                                                    &line_tx_dp_soft_rst.data));
                        line_tx_dp_soft_rst.fields.rstb_frc = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     LINE_TX_PMA_DP_soft_rst_Adr,\
                                                     line_tx_dp_soft_rst.data));
                        /* Releasing datapath reset */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_TX_PMA_DP_soft_rst_Adr,\
                                                    &line_tx_dp_soft_rst.data));
                        line_tx_dp_soft_rst.fields.rstb = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     LINE_TX_PMA_DP_soft_rst_Adr,\
                                                     line_tx_dp_soft_rst.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_TX_PMA_DP_soft_rst_Adr,\
                                                    &line_tx_dp_soft_rst.data));
                        line_tx_dp_soft_rst.fields.rstb_frc = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     LINE_TX_PMA_DP_soft_rst_Adr,\
                                                     line_tx_dp_soft_rst.data));
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_TX_PMA_DP_soft_rst_Adr,\
                                                    &sys_tx_dp_soft_rst.data));
                        sys_tx_dp_soft_rst.fields.rstb_frc = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_TX_PMA_DP_soft_rst_Adr,\
                                                     sys_tx_dp_soft_rst.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_TX_PMA_DP_soft_rst_Adr,\
                                                    &sys_tx_dp_soft_rst.data));
                        sys_tx_dp_soft_rst.fields.rstb = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_TX_PMA_DP_soft_rst_Adr,\
                                                     sys_tx_dp_soft_rst.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_TX_PMA_DP_soft_rst_Adr,\
                                                    &sys_tx_dp_soft_rst.data));
                        sys_tx_dp_soft_rst.fields.rstb_frc = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_TX_PMA_DP_soft_rst_Adr,\
                                                     sys_tx_dp_soft_rst.data));
                    }
                break;
                case phymodTxTrafficDisable:
                    return PHYMOD_E_UNAVAIL; 
                case phymodTxTrafficEnable:
                    return PHYMOD_E_UNAVAIL; 
                case phymodTxSquelchOn:
                    if(sys_en == LINE) {
                        /* Read GPReg12 */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                    &gen_ctrls_gpreg12.data));
                        gen_ctrls_gpreg12.fields.en_tx_squelch_line &= ~(1 << die_lane_num);
                        gen_ctrls_gpreg12.fields.en_tx_squelch_line |= (1 << die_lane_num);
                        /* Write to GPReg12 */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                     gen_ctrls_gpreg12.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                    &gen_ctrls_gpreg12.data));
                    } else {
                        /* Read GPReg11 */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                    &gen_ctrls_gpreg11.data));
                        gen_ctrls_gpreg11.fields.en_tx_squelch_sys &= ~(1 << die_lane_num);
                        gen_ctrls_gpreg11.fields.en_tx_squelch_sys |= (1 << die_lane_num);
                        /* Write to GPReg11 */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                     gen_ctrls_gpreg11.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                    &gen_ctrls_gpreg11.data));
                    }
                    if(lane_map == 0xF) {
                        continue;
                    }
                break;
                case phymodTxSquelchOff:
                    if(sys_en == LINE) {
                        /* Read GPReg12 */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                    &gen_ctrls_gpreg12.data));
                        gen_ctrls_gpreg12.fields.en_tx_squelch_line &= ~(1 << die_lane_num);
                        /* Write to GPReg12 */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                     gen_ctrls_gpreg12.data));
                    } else {
                        /* Read GPReg11 */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                    &gen_ctrls_gpreg11.data));
                        gen_ctrls_gpreg11.fields.en_tx_squelch_sys &= ~(1 << die_lane_num);
                        /* Write to GPReg11 */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                     gen_ctrls_gpreg11.data));
                    }
                    if(lane_map == 0xF) {
                        continue;
                    }
                break;
                default:
                break; 
            }
        }
        if(lane_map == 0xF) {
            break;
        } 
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_rx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_rx_lane_control_t rx_control)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl; 
    SYS_RX_PMA_DP_main_ctrl_t sys_rx_dp_main_ctrl;
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_main_ctrl_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
           
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
               sys_en = pkg_ln_des->sideB;
            }

            
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            switch(rx_control) {
                case phymodRxReset: 
                    if(sys_en == LINE) {
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                        /* Asserting datapath reset */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    &line_rx_dp_main_ctrl.data));
                        line_rx_dp_main_ctrl.fields.rstb_frc = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                     line_rx_dp_main_ctrl.data));
                        /* Releasing datapath reset */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    &line_rx_dp_main_ctrl.data));
                        line_rx_dp_main_ctrl.fields.rstb = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                     line_rx_dp_main_ctrl.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    &line_rx_dp_main_ctrl.data));
                        line_rx_dp_main_ctrl.fields.rstb_frc = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                     line_rx_dp_main_ctrl.data));
                    } else { 
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    &sys_rx_dp_main_ctrl.data));
                        sys_rx_dp_main_ctrl.fields.rstb_frc = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                     sys_rx_dp_main_ctrl.data));
                        /* Releasing datapath reset */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    &sys_rx_dp_main_ctrl.data));
                        sys_rx_dp_main_ctrl.fields.rstb = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                     sys_rx_dp_main_ctrl.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    &sys_rx_dp_main_ctrl.data));
                        sys_rx_dp_main_ctrl.fields.rstb_frc = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                     sys_rx_dp_main_ctrl.data));

                    }
                break;
                default:
                break;
            }
            if(lane_map == 0xF) {
                break;
            }
        }
    }

    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

/**  Get Package index 
 *    
 *    
 *
 *    @param chip_id            Chip id of furia device
 *    @param pkg_idx            Furia package
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
static int furia_get_pkg_idx(uint32_t chip_id, int *pkg_idx)
{
    int index = 0;
    for (index = 0; index < MAX_NUM_PACKAGES; index ++) {
        if(glb_package_array[index] != NULL) {
           if(glb_package_array[index]->chip_id == chip_id) {
               *pkg_idx = index;
               break;
           } 
        } 
    }
    if(index >= MAX_NUM_PACKAGES) {
        return PHYMOD_E_LIMIT; 
    }
    return PHYMOD_E_NONE; 
}

/**   Get lane descriptor 
 *    This function is used to retrieve lane descriptor from package lane 
 *    
 *    @param chip_id            Chip id of furia device
 *    @param pa                 Pointer to phymod access structure 
 *    @param pkg_lane           Package lane number
 *
 *    @return pkg_ln_des        Lane descriptor structure contains the info
 *                              about package lane and die lane mapping 
 */
const FURIA_PKG_LANE_CFG_t* _furia_pkg_ln_des(uint32_t chip_id, const phymod_access_t *pa, int pkg_lane)
{
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    int pkg_idx;
    
    if ((pa->addr & 1) == 1) {
        if(FURIA_IS_SIMPLEX(chip_id)) {
            pkg_lane = pkg_lane + 8; 
        } else {
            pkg_lane = pkg_lane + 4;
        }
    }  
    /* Get the package index */
    if (!furia_get_pkg_idx(chip_id, &pkg_idx)) {
        pkg_ln_des = glb_package_array[pkg_idx] + pkg_lane;
    }
    return pkg_ln_des; 
}  
 
/**   Get ref clock frequency
 *    This function returns ref_clk_freq from ref clock freq type 
 *
 *    @param ref_clk_freq  Ref clk type 
 * 
 *    @return freq         Ref clk freq  in Mhz based on ref clk freq type
 */
int _furia_get_ref_clock_freq_in_mhz(FURIA_REF_CLK_E ref_clk_freq) 
{
    int freq = 0; 
    if (ref_clk_freq == REF_CLK_106p25MHz) {
        freq = 10625;
    } else if (ref_clk_freq == REF_CLK_174p703125MHz) { /* 174.703125 Mhz */
        freq = 17470;
    } else if (ref_clk_freq == REF_CLK_156p25Mhz) { /* 156.25 Mhz */
        freq = 15625;
    } else if (ref_clk_freq == REF_CLK_125Mhz) { /* 125 Mhz */
        freq = 1250;
    } else if (ref_clk_freq == REF_CLK_156p637Mhz) { /* 156.637 Mhz */
        freq = 15663;
    } else if (ref_clk_freq == REF_CLK_161p1328125Mhz) { /* 161.1328125 Mhz */
        freq = 16113;
    } else if (ref_clk_freq == REF_CLK_168p04Mhz) { /* 168.04 Mhz */
        freq = 16804;
    } else if (ref_clk_freq == REF_CLK_172p64Mhz) { /* 172.64 Mhz */
        freq = 17264;
    } else {
        PHYMOD_RETURN_WITH_ERR
            (PHYMOD_E_PARAM,
            (_PHYMOD_MSG(" Ref clk is not supported by furia")));
    }
    return(freq);
}

/**   Config clk scaler  
 *    This function is used to program clk scaler 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param ref_clk_freq       Furia ref clk type 
 *  
 *    @return PHYMOD_E_NONE     successful function execution 
 */
static int _furia_config_clk_scaler_val(const phymod_access_t *pa, FURIA_REF_CLK_E ref_clk_freq) 
{
    uint16_t clock_scaler_code = 0;
    FUR_GEN_CNTRLS_GEN_CONTROL3_t gen_ctrl_3;
    PHYMOD_MEMSET(&gen_ctrl_3, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL3_t));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl_3.data));
    gen_ctrl_3.fields.clock_sclr_lock = 0x1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control3_Adr,\
                                 gen_ctrl_3.data));
    switch (ref_clk_freq) {
        case REF_CLK_106p25MHz:
            clock_scaler_code = 0x4;
        break;
        case REF_CLK_174p703125MHz:
            clock_scaler_code = 0x0; 
        break;
        case REF_CLK_156p25Mhz:
            clock_scaler_code = 0x2;
        break;
        case REF_CLK_125Mhz:
            clock_scaler_code = 0x3;
        break;
        case REF_CLK_161p1328125Mhz:
            clock_scaler_code = 0x1;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                    (PHYMOD_E_PARAM,
                    (_PHYMOD_MSG(" This Ref Clock is not supported by Furia")));

        break;   

    }
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl_3.data));

    gen_ctrl_3.fields.clock_scaler_code = clock_scaler_code;

    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control3_Adr,\
                                 gen_ctrl_3.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl_3.data));
    gen_ctrl_3.fields.clock_sclr_lock = 0x0;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control3_Adr,\
                                 gen_ctrl_3.data));
    return PHYMOD_E_NONE;
}

/**   Get PLL divider 
 *    This function is used to retrieve the pll divder value from 
 *    pll mode 
 *
 *    @param pll_mode                 PLL mode 
 *    @return divider           PLL divider value calculated using pll mode 
 */
static int _furia_get_pll_divider(FALCON_PLL_MODE_E pll_mode) 
{
    int divider = 0;
    if (pll_mode == PLL_MODE_64) {
        divider = 64;
    } else if (pll_mode == PLL_MODE_66) {
        divider = 66;
    } else if (pll_mode == PLL_MODE_80) {
        divider = 80;
    } else if (pll_mode == PLL_MODE_96) {
        divider = 96;
    } else if (pll_mode == PLL_MODE_120) {
        divider = 120;
    } else if (pll_mode == PLL_MODE_128) {
        divider = 128;
    } else if (pll_mode == PLL_MODE_132) {
        divider = 132;
    } else if (pll_mode == PLL_MODE_140) {
        divider = 140;
    } else if (pll_mode == PLL_MODE_160) {
        divider = 160;
    } else if (pll_mode == PLL_MODE_165) {
        divider = 165;
    } else if (pll_mode == PLL_MODE_168) {
        divider = 168;
    } else if (pll_mode == PLL_MODE_170) {
        divider = 170;
    } else if (pll_mode == PLL_MODE_175) {
        divider = 175;
    } else if (pll_mode == PLL_MODE_180) {
        divider = 180;
    } else if (pll_mode == PLL_MODE_184) {
        divider = 184;
    } else if (pll_mode == PLL_MODE_200) {
        divider = 200;
    } else if (pll_mode == PLL_MODE_224) {
        divider = 224;
    } else if (pll_mode == PLL_MODE_264) {
        divider = 264;
    } else {
        PHYMOD_RETURN_WITH_ERR
            (PHYMOD_E_PARAM,
            (_PHYMOD_MSG(" PLL mode is not supported by furia")));
    }
    return(divider);
}


/**   Get PHY type 
 *    This function is used to retrieve the PHY type
 *    based on the interface value. 
 *
 *    @param intf               Interface type 
 * 
 *    @return phy_type          PHY type calculated based on interface  
 */
static uint16_t _furia_get_phy_type(phymod_interface_t intf)
{
    uint16_t phy_type = 0;
    switch (intf) {
        case phymodInterfaceSR:
        case phymodInterfaceCX:
        case phymodInterfaceCX2:
        case phymodInterfaceCX4:
        case phymodInterfaceCR:
        case phymodInterfaceCR2:
        case phymodInterfaceCR4:
        case phymodInterfaceCR10:
        case phymodInterfaceLR4:
        case phymodInterfaceLR:
        case phymodInterfaceSR4:
            phy_type = 0x1;
        break;
        case phymodInterfaceKX:
        case phymodInterfaceKX4:
        case phymodInterfaceKR:
        case phymodInterfaceKR2:
        case phymodInterfaceKR4:
            phy_type = 0x0;
        break;
        default:
        break;
    }
    return phy_type;
}

/**   Get link type 
 *    This function is used to retrieve the operating mode of the PHY
 *
 *    @param intf               Phymod interface type
 *    @param speed              User specified speed 
 * 
 *    @return link_type         Link type calculated based on intf and speed 
 */
static uint16_t _furia_get_link_type(phymod_interface_t intf, uint32_t speed)
{
    uint16_t link_type = 0; 
    switch (intf) {
        case phymodInterfaceKX:
        case phymodInterfaceSR:
            link_type = 0x1;
        break;
        case phymodInterfaceKR:
            if (speed == SPEED_10G) {
                link_type = 0x2;
            } else if (speed == SPEED_20G) {
                link_type = 0x6;
            } else {
                link_type = 0x8;
            }    
        break;
        case phymodInterfaceLR:
            if (speed == SPEED_10G) {
                link_type = 0x1;
            } else if (speed == SPEED_20G) {
                link_type = 0x6;
            } else {
                link_type = 0x7;
            }
        break;
        case phymodInterfaceCR4:
            if (speed == SPEED_100G) {
                link_type = 0x5;
            } else {
                link_type = 0x2;
            }
        break;
        case phymodInterfaceKR4:
        case phymodInterfaceLR4:
        case phymodInterfaceSR4:
            if (speed == SPEED_100G) {
                link_type = 0x4;
            } else {
                link_type = 0x3;
            }
        break;
        case phymodInterfaceKR2:
            if (speed == SPEED_20G) {
                link_type = 0x5;
            } else if (speed == SPEED_40G) {
                link_type = 0x7;
            } else {
                link_type = 0x9;
            }
        break;
        default:
        break;
    }
    return link_type;
}

/**   Get furia ref clk type 
 *    This function is used to convert the phymod ref clk type to 
 *    furia ref clk type 
 *
 *    @param ref_clk            Phymod ref clk type 
 *    @param ref_clk_freq       Furia ref clk type  
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
static int _furia_phymod_to_furia_type_ref_clk_freq(phymod_ref_clk_t ref_clk, FURIA_REF_CLK_E *ref_clk_freq)
{
    switch (ref_clk) {
        case phymodRefClk156Mhz:
           *ref_clk_freq = REF_CLK_156p25Mhz;
        break;
        case phymodRefClk125Mhz:
            *ref_clk_freq = REF_CLK_125Mhz;
        break;
        case phymodRefClk106Mhz:
            *ref_clk_freq = REF_CLK_106p25MHz;
        break;
        case phymodRefClk161Mhz:
            *ref_clk_freq = REF_CLK_161p1328125Mhz;
        break;
        case phymodRefClk174Mhz:
            *ref_clk_freq = REF_CLK_174p703125MHz;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                                   (_PHYMOD_MSG(" This Ref Clock is not supported by Furia")));
        break;
    }
    return PHYMOD_E_NONE;
}

/**   Get the PLL mode 
 *    This function calculates pll mode based on the refclock & speed 
 *    @param ref_clk            PHYMOD ref clk frequency
 *    @param speed              speed specified by user
 *    @param pll_mode           pll mode calculated based on ref clk and speed 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
static int _furia_get_pll_mode(phymod_ref_clk_t ref_clk, uint32_t speed, FALCON_PLL_MODE_E *pll_mode) {
    switch (ref_clk) {
        case phymodRefClk156Mhz:
            switch(speed) {
                case SPEED_100G:
                case SPEED_50G:
                case SPEED_25G:
                    *pll_mode = PLL_MODE_165;
                break;
                case SPEED_40G:
                case SPEED_20G:
                case SPEED_10G:
                    *pll_mode = PLL_MODE_132;
                break;
                case SPEED_1G:
                    *pll_mode = PLL_MODE_132;
                break;
                default:
                break;
            } 
        break;
        case phymodRefClk125Mhz:
            switch(speed) {
                case SPEED_23G:
                    *pll_mode = PLL_MODE_184;
                break;
                case SPEED_15G:
                    *pll_mode = PLL_MODE_120;
                break;
                case SPEED_12P5G:
                    *pll_mode = PLL_MODE_200;
                break;
                case SPEED_11P5G:
                    *pll_mode = PLL_MODE_184;
                break;
                case SPEED_7P5G:
                    *pll_mode = PLL_MODE_120;
                default:
                break;
            } 
        break;
        case phymodRefClk106Mhz:
            switch(speed) {
                case SPEED_28G:
                    *pll_mode = PLL_MODE_264;
                break;
                case SPEED_14G:
                    *pll_mode = PLL_MODE_264;
                break;
                case SPEED_8P5G:
                    *pll_mode = PLL_MODE_160;
                break;
                case SPEED_4P25G:
                    *pll_mode = PLL_MODE_160;
                break;
                default:
                break;
            } 
        break;
        case phymodRefClk161Mhz:
            switch(speed) {
                case SPEED_100G:
                case SPEED_50G:
                case SPEED_25G:
                    *pll_mode = PLL_MODE_160;
                break;
                case SPEED_40G:
                case SPEED_20G:
                case SPEED_10G:
                    *pll_mode = PLL_MODE_128;
                break;
                case SPEED_1G:
                    *pll_mode = PLL_MODE_128;
                break;
                default:
                break;
            } 
        break;
        default:
        break;
    }
    return PHYMOD_E_NONE;
}


static int _furia_config_clk_scaler_without_m0_reset(const phymod_access_t *pa, FURIA_REF_CLK_E ref_clk_freq)
{
    FUR_MICRO_BOOT_BOOT_POR_t micro_boot_por;
    FUR_GEN_CNTRLS_BOOT_t gen_ctrl_boot;
    int retry_count = 5;
    PHYMOD_MEMSET(&micro_boot_por, 0 , sizeof(FUR_MICRO_BOOT_BOOT_POR_t));
    PHYMOD_MEMSET(&gen_ctrl_boot, 0 , sizeof(FUR_GEN_CNTRLS_BOOT_t));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MICRO_BOOT_boot_por_Adr,\
                                &micro_boot_por.data));
    if((micro_boot_por.fields.serboot == 1) && (micro_boot_por.fields.spi_port_used == 1)) {
        while(((micro_boot_por.fields.mst_dwld_done == 0) || (micro_boot_por.fields.slv_dwld_done == 0)) && (retry_count)) {
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MICRO_BOOT_boot_por_Adr,\
                                        &micro_boot_por.data));
            retry_count--; 
        }
        if(!retry_count) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("config failed, micro controller is busy..")));
        } 
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_boot_Adr,\
                                    &gen_ctrl_boot.data));
        retry_count = 5;
        while((gen_ctrl_boot.fields.serboot_busy == 1) && (retry_count)) {
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_GEN_CNTRLS_boot_Adr,\
                                        &gen_ctrl_boot.data));
            retry_count--;
        }
        if(!retry_count) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("config failed, micro controller is busy with firmware download..")));
        }
    } 
   _furia_config_clk_scaler_val(pa, ref_clk_freq);
    return PHYMOD_E_NONE;
}

int _furia_fw_enable(const phymod_access_t* pa)
{
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    PHYMOD_MEMSET(&fw_enable_reg, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));
 
    /* Get the lane map from phymod access */ 
    PHYMOD_IF_ERR_RETURN
           (READ_FURIA_PMA_PMD_REG(pa, 
                                   FUR_GEN_CNTRLS_firmware_enable_Adr,  
                                   &fw_enable_reg.data));
     if (!fw_enable_reg.fields.fw_enable_val) {
         fw_enable_reg.fields.fw_enable_val = 1;
         PHYMOD_IF_ERR_RETURN
             (WRITE_FURIA_PMA_PMD_REG(pa, 
                                      FUR_GEN_CNTRLS_firmware_enable_Adr,
                                      fw_enable_reg.data));
     }
     return PHYMOD_E_NONE;
}

int _furia_autoneg_set(const phymod_access_t* pa, const phymod_autoneg_control_t* an)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_t an_control;
    FUR_MISC_CTRL_UDMS_PHY_t udms_phy;
    PHYMOD_MEMSET(&an_control, 0 , sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_t));
    PHYMOD_MEMSET(&udms_phy, 0 , sizeof(FUR_MISC_CTRL_UDMS_PHY_t));
    
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Autoneg feature is not applicable for simplex packages")));
    }

    /* Program udms_en=0 */
    /* Read UDMS PHY Type Register */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_udms_phy_Adr,\
                                &udms_phy.data));
 
    /* Update the field udms disable*/
    udms_phy.fields.udms_en = 0;
    /* Write to UDMS PHY Type Register */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_udms_phy_Adr,\
                                 udms_phy.data));

    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            if (an->an_mode == phymod_AN_MODE_CL73) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_AN_REG(pa,\
                                            IEEE_AN_BLK0_an_control_register_Adr,\
                                            &an_control.data));
                an_control.fields.auto_negotiationenable = an->enable;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_AN_REG(pa,\
                                             IEEE_AN_BLK0_an_control_register_Adr,\
                                             an_control.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_AN_REG(pa,\
                                            IEEE_AN_BLK0_an_control_register_Adr,\
                                            &an_control.data));
            } else {
                return PHYMOD_E_PARAM;
            }
        }
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));

    PHYMOD_IF_ERR_RETURN(
            _furia_fw_enable(pa));

    return PHYMOD_E_NONE;
}

int _furia_autoneg_get(const phymod_access_t* pa, phymod_autoneg_control_t* an, uint32_t *an_done)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_t an_control;
    IEEE_AN_BLK0_AN_STATUS_REGISTER_t an_sts;
    PHYMOD_MEMSET(&an_control, 0 , sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_t));
    PHYMOD_MEMSET(&an_sts, 0 , sizeof(IEEE_AN_BLK0_AN_STATUS_REGISTER_t));
    
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Autoneg feature is not applicable for simplex packages")));
    }

    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_AN_REG(pa, IEEE_AN_BLK0_an_control_register_Adr,  
                                     &an_control.data));
            an->enable = an_control.fields.auto_negotiationenable;
            an->an_mode = phymod_AN_MODE_CL73;
            PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_AN_REG(pa, IEEE_AN_BLK0_an_status_register_Adr,  
                                     &an_sts.data));
            *an_done = an_sts.fields.auto_negotiationcomplete;
            return PHYMOD_E_NONE;
        }
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));

    return PHYMOD_E_NONE;
}
int _furia_autoneg_ability_get (const phymod_access_t* pa, furia_an_ability_t *an_ability)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    IEEE_AN_BLK1_an_advertisement_1_register_t adv1;
    IEEE_AN_BLK1_an_advertisement_2_register_t adv2;
    IEEE_AN_BLK1_an_advertisement_3_register_t adv3;
    PHYMOD_MEMSET(&adv1, 0 , sizeof(IEEE_AN_BLK1_an_advertisement_1_register_t));
    PHYMOD_MEMSET(&adv2, 0 , sizeof(IEEE_AN_BLK1_an_advertisement_2_register_t));
    PHYMOD_MEMSET(&adv3, 0 , sizeof(IEEE_AN_BLK1_an_advertisement_3_register_t));
    
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Autoneg feature is not applicable for simplex packages")));
    }

    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN 
                (READ_FURIA_AN_REG(pa, IEEE_AN_BLK1_an_advertisement_3_register_Adr,  
                                     &adv3.data));
            an_ability->cl73_adv.an_fec = adv3.fields.fec_requested;
            PHYMOD_IF_ERR_RETURN 
                (READ_FURIA_AN_REG(pa, IEEE_AN_BLK1_an_advertisement_2_register_Adr,  
                                     &adv2.data));
            an_ability->cl73_adv.an_base_speed = adv2.fields.techability;
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_AN_REG(pa, IEEE_AN_BLK1_an_advertisement_1_register_Adr,  
                                     &adv1.data));
            an_ability->cl73_adv.an_pause = adv1.fields.pause;
        }
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}
int _furia_autoneg_ability_set (const phymod_access_t* pa, furia_an_ability_t *an_ability)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t logic_to_phy_ln0_map = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    IEEE_AN_BLK1_an_advertisement_1_register_t adv1;
    IEEE_AN_BLK1_an_advertisement_2_register_t adv2;
    IEEE_AN_BLK1_an_advertisement_3_register_t adv3;
 
    PHYMOD_MEMSET(&adv1, 0, sizeof(IEEE_AN_BLK1_an_advertisement_1_register_t));
    PHYMOD_MEMSET(&adv2, 0, sizeof(IEEE_AN_BLK1_an_advertisement_2_register_t));
    PHYMOD_MEMSET(&adv3, 0, sizeof(IEEE_AN_BLK1_an_advertisement_3_register_t));
   
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Autoneg feature is not applicable for simplex packages")));
    }
    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);

            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            /* Selecting master lane for 40G and 100G*/
            if((an_ability->cl73_adv.an_base_speed == FURIA_CL73_40GBASE_KR4) ||
               (an_ability->cl73_adv.an_base_speed == FURIA_CL73_40GBASE_CR4) ||
               (an_ability->cl73_adv.an_base_speed == FURIA_CL73_100GBASE_KR4) ||
               (an_ability->cl73_adv.an_base_speed == FURIA_CL73_100GBASE_CR4)) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa, 0x8a08,  &logic_to_phy_ln0_map));
                logic_to_phy_ln0_map = (logic_to_phy_ln0_map & (~(0x3))) | pkg_ln_des->die_lane_num;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa, 0x8a08, logic_to_phy_ln0_map));
            }
 

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_AN_REG(pa,\
                                        IEEE_AN_BLK1_an_advertisement_3_register_Adr,\
                                        &adv3.data));
            adv3.fields.fec_requested = an_ability->cl73_adv.an_fec; 
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_REG(pa,\
                                         IEEE_AN_BLK1_an_advertisement_3_register_Adr,\
                                         adv3.data));
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_AN_REG(pa,\
                                        IEEE_AN_BLK1_an_advertisement_2_register_Adr,\
                                        &adv2.data));
            adv2.fields.techability = an_ability->cl73_adv.an_base_speed;
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_REG(pa,\
                                         IEEE_AN_BLK1_an_advertisement_2_register_Adr,\
                                         adv2.data));
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_AN_REG(pa,\
                                        IEEE_AN_BLK1_an_advertisement_1_register_Adr,\
                                        &adv1.data));
            adv1.fields.pause = an_ability->cl73_adv.an_pause;
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_REG(pa,\
                                         IEEE_AN_BLK1_an_advertisement_1_register_Adr,\
                                         adv1.data));
        }
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_display_eye_scan(const phymod_access_t *pa)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            PHYMOD_DIAG_OUT((" eyescan for lane = %d\n",     lane_index));
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            PHYMOD_IF_ERR_RETURN
                (falcon_furia_display_lane_state_hdr());
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_display_lane_state()); 
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_display_eye_scan());
        }
    }
    return PHYMOD_E_NONE;
}

int furia_pll_sequencer_restart(const phymod_access_t *pa, phymod_sequencer_operation_t operation)
{
    int pkg_side = 0;
    uint32_t acc_flags = 0; 
    uint32_t chip_id = 0;
    LINE_FALCON_IF_SOFT_MODE0_t line_falcon_soft_mode0;
    LINE_FALCON_IF_C_AND_R_CNTL_t line_falcon_c_and_r_ctrl;
    SYS_FALCON_IF_SOFT_MODE0_t    sys_falcon_soft_mode0;
    SYS_FALCON_IF_C_AND_R_CNTL_t  sys_falcon_c_and_r_ctrl;
    PHYMOD_MEMSET(&line_falcon_soft_mode0, 0 , sizeof(LINE_FALCON_IF_SOFT_MODE0_t));
    PHYMOD_MEMSET(&line_falcon_c_and_r_ctrl, 0 , sizeof(LINE_FALCON_IF_C_AND_R_CNTL_t));
    PHYMOD_MEMSET(&sys_falcon_soft_mode0, 0 , sizeof(SYS_FALCON_IF_SOFT_MODE0_t));
    PHYMOD_MEMSET(&sys_falcon_c_and_r_ctrl, 0 , sizeof(SYS_FALCON_IF_C_AND_R_CNTL_t));
   
    /* Get the lane map from phymod access */
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    switch(operation) {
        case phymodSeqOpStop:
        case phymodSeqOpStart:
            return PHYMOD_E_UNAVAIL; 
        case phymodSeqOpRestart:
            if((FURIA_IS_DUPLEX(chip_id) && pkg_side == LINE) ||
               (FURIA_IS_SIMPLEX(chip_id))) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_soft_mode0_Adr,\
                                            &line_falcon_soft_mode0.data));
                line_falcon_soft_mode0.fields.software_mode_pmd_core_dp_h_rstb = 1;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_soft_mode0_Adr,\
                                             line_falcon_soft_mode0.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                            &line_falcon_c_and_r_ctrl.data));
                line_falcon_c_and_r_ctrl.fields.pmd_core_dp_h_rstb = 0;
                line_falcon_c_and_r_ctrl.fields.pmd_core_dp_h_rstb = 1;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                             line_falcon_c_and_r_ctrl.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_soft_mode0_Adr,\
                                            &line_falcon_soft_mode0.data));
                line_falcon_soft_mode0.fields.software_mode_pmd_core_dp_h_rstb = 0;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_soft_mode0_Adr,\
                                             line_falcon_soft_mode0.data));
            } 
            if((FURIA_IS_DUPLEX(chip_id) && pkg_side == SYS) ||
                (FURIA_IS_SIMPLEX(chip_id))) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_soft_mode0_Adr,\
                                            &sys_falcon_soft_mode0.data));
                sys_falcon_soft_mode0.fields.software_mode_pmd_core_dp_h_rstb = 1;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_soft_mode0_Adr,\
                                             sys_falcon_soft_mode0.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                            &sys_falcon_c_and_r_ctrl.data));
                sys_falcon_c_and_r_ctrl.fields.pmd_core_dp_h_rstb = 0;
                sys_falcon_c_and_r_ctrl.fields.pmd_core_dp_h_rstb = 1;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                             sys_falcon_c_and_r_ctrl.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_soft_mode0_Adr,\
                                            &sys_falcon_soft_mode0.data));
                sys_falcon_soft_mode0.fields.software_mode_pmd_core_dp_h_rstb = 0;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_soft_mode0_Adr,\
                                             sys_falcon_soft_mode0.data));
            }
        break;
        default:
        break;
    }
    return PHYMOD_E_NONE;
}
int furia_fec_enable_set(const phymod_access_t *pa, uint32_t enable)
{
    FUR_MISC_CTRL_MODE_DEC_FRC_t dec_frc;
    FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t dec_frc_val;
    uint32_t chip_id = 0;
    PHYMOD_MEMSET(&dec_frc, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_t));
    PHYMOD_MEMSET(&dec_frc_val, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t));

    chip_id = _furia_get_chip_id(pa);
    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("FEC is not applicable for simplex packages")));
    }

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_mode_dec_frc_val_Adr,\
                                &dec_frc_val.data));
    dec_frc_val.fields.rg_cl91_enabled_frc_val = enable ? 1 : 0;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_mode_dec_frc_val_Adr,\
                                 dec_frc_val.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_mode_dec_frc_Adr,\
                                &dec_frc.data));
    dec_frc.fields.rg_cl91_enabled_frc = 1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_mode_dec_frc_Adr,\
                                 dec_frc.data));
    return PHYMOD_E_NONE;
}
int furia_fec_enable_get(const phymod_access_t *pa, uint32_t* enable)
{
    FUR_MISC_CTRL_MODE_DEC_FRC_t dec_frc;
    FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t dec_frc_val;
    uint32_t chip_id = 0;

    PHYMOD_MEMSET(&dec_frc, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_t));
    PHYMOD_MEMSET(&dec_frc_val, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t));

    chip_id = _furia_get_chip_id(pa);
    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("FEC is not applicable for simplex packages")));
    }

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_mode_dec_frc_val_Adr,\
                                    &dec_frc_val.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_mode_dec_frc_Adr,\
                                &dec_frc.data));
    if(dec_frc_val.fields.rg_cl91_enabled_frc_val &&
       dec_frc.fields.rg_cl91_enabled_frc) {
        *enable = 1;
    } else {
        *enable = 0;
    } 
    return PHYMOD_E_NONE;
}
int _furia_phy_status_dump(const phymod_access_t *pa)
{
    int pkg_side = 0;
    int sys_en = 0;
    int wr_lane = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    int lane_map = 0;
    uint32_t acc_flags = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_DIAG_OUT((" ******* PHY status dump for PHY ID:%d ********\n", pa->addr));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    /* Program the slice register */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, pkg_side, 1, 0));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_DIAG_OUT((" ******* PHY status dump for side:%d ********\n", pkg_side));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_IF_ERR_RETURN(falcon_furia_display_core_config());
    PHYMOD_IF_ERR_RETURN(falcon_furia_display_core_state());
    
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_display_lane_state_hdr());
            PHYMOD_IF_ERR_RETURN(falcon_furia_display_lane_state());
            PHYMOD_IF_ERR_RETURN(falcon_furia_display_lane_config());
       }
    }
    return PHYMOD_E_NONE;
}
int _furia_phy_diagnostics_get(const phymod_access_t *pa, phymod_phy_diagnostics_t* diag)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    falcon_furia_lane_state_st state;

    PHYMOD_MEMSET(&state, 0 , sizeof(falcon_furia_lane_state_st));
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    falcon_furia_pm_acc_set(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (_falcon_furia_read_lane_state(&state));
            diag->signal_detect = state.sig_det;
            diag->osr_mode = state.osr_mode.tx_rx;
            diag->rx_lock = state.rx_lock;
            diag->tx_ppm = state.tx_ppm;
            diag->clk90_offset = state.clk90;
            diag->clkp1_offset = state.clkp1;
            diag->p1_lvl = state.p1_lvl;
            diag->dfe1_dcd = state.dfe1_dcd;
            diag->dfe2_dcd = state.dfe2_dcd;
            diag->slicer_offset.offset_pe = state.pe;           
            diag->slicer_offset.offset_ze = state.ze;           
            diag->slicer_offset.offset_me = state.me;           
            diag->slicer_offset.offset_po = state.po;           
            diag->slicer_offset.offset_zo = state.zo;           
            diag->slicer_offset.offset_mo = state.mo; 
            diag->eyescan.heye_left = state.heye_left;
            diag->eyescan.heye_right = state.heye_right;
            diag->eyescan.veye_upper = state.veye_upper;
            diag->eyescan.veye_lower = state.veye_lower;
            diag->link_time = state.link_time;
            diag->pf_main = state.pf_main;
            diag->pf_hiz = state.pf_hiz;
            diag->pf_bst = state.pf_bst;
            diag->pf_low = state.pf_low;
            diag->pf2_ctrl = state.pf2_ctrl;
            diag->vga = state.vga;
            diag->dc_offset = state.dc_offset;
            diag->p1_lvl_ctrl = state.p1_lvl_ctrl;
            diag->dfe1 = state.dfe1;
            diag->dfe2 = state.dfe2;
            diag->dfe3 = state.dfe3;
            diag->dfe4 = state.dfe4;
            diag->dfe5 = state.dfe5;
            diag->dfe6 = state.dfe6;
            diag->txfir_pre = state.txfir_pre;
            diag->txfir_main = state.txfir_main;
            diag->txfir_post1 = state.txfir_post1;
            diag->txfir_post2 = state.txfir_post2;
            diag->txfir_post3 = state.txfir_post3;
            diag->tx_amp_ctrl = state.tx_amp_ctrl;
            diag->br_pd_en = state.br_pd_en;
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int _furia_core_rptr_rtmr_mode_set(const phymod_core_access_t* core, uint32_t rptr_rtmr_mode)
{
    int lane_map = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint16_t die_lane_num = 0;
    uint32_t enable = 0;
    uint8_t fw_enable = 0;
    uint8_t retry_count = 5;
    FUR_GEN_CNTRLS_gpreg12_t gen_ctrls_gpreg12;
    FUR_GEN_CNTRLS_gpreg11_t gen_ctrls_gpreg11;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    const phymod_access_t *pa = &core->access;
    PHYMOD_MEMSET(&gen_ctrls_gpreg12, 0 , sizeof(FUR_GEN_CNTRLS_gpreg12_t));
    PHYMOD_MEMSET(&gen_ctrls_gpreg11, 0 , sizeof(FUR_GEN_CNTRLS_gpreg11_t));
    PHYMOD_MEMSET(&fw_enable_reg, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));

    
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    enable = (rptr_rtmr_mode == 1) ? 1 : 0;

    /*Wait for fw_enable to go low  before setting fw_enable_val*/
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;

    while((fw_enable != 0) && (retry_count)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                    &fw_enable_reg.data));
        fw_enable = fw_enable_reg.fields.fw_enable_val;
        retry_count--;
    }

    lane_map = FURIA_IS_SIMPLEX(chip_id) ? 0xFF : 0xF;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            die_lane_num = pkg_ln_des->die_lane_num;
            if(FURIA_IS_SIMPLEX(chip_id)) {
                if(pkg_ln_des->sideA == LINE) {
                    /* Read GPReg12 */
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                &gen_ctrls_gpreg12.data));
                    if(enable) {
                        gen_ctrls_gpreg12.fields.en_tx_rptr_mode_sys2line |= (1 << die_lane_num);
                    } else {
                        gen_ctrls_gpreg12.fields.en_tx_rptr_mode_sys2line &= ~(1 << die_lane_num);
                    }
                    /* Write GPReg12 */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                gen_ctrls_gpreg12.data));
                } else {
                    /* Read GPReg11 */
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                &gen_ctrls_gpreg11.data));
                    if(enable) {
                        gen_ctrls_gpreg11.fields.en_tx_rptr_mode_line2sys |= (1 << die_lane_num);
                    } else {
                        gen_ctrls_gpreg11.fields.en_tx_rptr_mode_line2sys &= ~(1 << die_lane_num);
                    }
                    /* Write GPReg11 */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                gen_ctrls_gpreg11.data));
                }
            } else {
                /* Read GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg12_Adr,\
                                            &gen_ctrls_gpreg12.data));
                /* Read GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg11_Adr,\
                                            &gen_ctrls_gpreg11.data));
                if(enable) {
                    gen_ctrls_gpreg12.fields.en_tx_rptr_mode_sys2line |= (1 << die_lane_num);
                    gen_ctrls_gpreg11.fields.en_tx_rptr_mode_line2sys |= (1 << die_lane_num);
                } else {
                    gen_ctrls_gpreg12.fields.en_tx_rptr_mode_sys2line &= ~(1 << die_lane_num);
                    gen_ctrls_gpreg11.fields.en_tx_rptr_mode_line2sys &= ~(1 << die_lane_num);
                }
                /* Write GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             FUR_GEN_CNTRLS_gpreg12_Adr,\
                                             gen_ctrls_gpreg12.data));
                /* Write GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg11_Adr,\
                                            gen_ctrls_gpreg11.data));
            }
        }
    }
    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_furia_fw_enable(pa));
    return PHYMOD_E_NONE;
}
