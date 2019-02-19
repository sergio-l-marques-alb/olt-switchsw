/*
*
* $Id: quadra28_cfg_seq.c, 2015/01/16 aman $
*
5 *
5 * $Copyright: Copyright 2012 Broadcom Corporation.
5 * This program is the proprietary software of Broadcom Corporation
5 * and/or its licensors, and may only be used, duplicated, modified
5 * or distributed pursuant to the terms and conditions of a separate,
5 * written license agreement executed between you and Broadcom
5 * (an "Authorized License").  Except as set forth in an Authorized
5 * License, Broadcom grants no license (express or implied), right
5 * to use, or waiver of any kind with respect to the Software, and
5 * Broadcom expressly reserves all rights in and to the Software
5 * and all intellectual property rights therein.  IF YOU HAVE
5 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
5 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
5 * ALL USE OF THE SOFTWARE.  
5 *  
5 * Except as expressly set forth in the Authorized License,
5 *  
5 * 1.     This program, including its structure, sequence and organization,
5 * constitutes the valuable trade secrets of Broadcom, and you shall use
5 * all reasonable efforts to protect the confidentiality thereof,
5 * and to use this information only in connection with your use of
5 * Broadcom integrated circuit products.
5 *  
5 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
5 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
5 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
5 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
5 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
5 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
5 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
5 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
5 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
5 * 
5 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
5 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
5 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
5 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
5 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
5 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
5 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
5 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
5 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
7  *
8  * File: quadra28.c
   * Purpose: tier1 phymod microcode download support for Broadcom 40G Quadra28 
   * note
*/
/*
 * Includes
 */
#include <phymod/phymod.h>
#include "quadra28_cfg_seq.h"
#include "quadra28_reg_access.h"
#include "bcmi_quadra28_defs.h"

static
int32 _quadra28_finish_change_wait_check(const phymod_access_t *pa ,
                   int32 timeout);
static
int32 _quadra28_intf_update_wait_check(const phymod_access_t *pa, uint32_t apps_mode_val,
                   int32 timeout) ;
static 
int merlin_quadra28_get_rx_prbs_config(const phymod_access_t *pa, 
    enum srds_prbs_polynomial_enum *prbs_poly_mode, 
    enum srds_prbs_checker_mode_enum *prbs_checker_mode, uint8_t *prbs_inv);
    
static 
int merlin_quadra28_get_tx_prbs_config(const phymod_access_t *pa, 
    enum srds_prbs_polynomial_enum *prbs_poly_mode, uint8_t *prbs_inv);
    
static 
int merlin_quadra28_pmd_lock_status(const phymod_access_t *pa, uint8_t *pmd_rx_lock);

static 
int merlin_quadra28_get_tx_prbs_en(const phymod_access_t *pa, uint8_t *enable);

static 
int merlin_quadra28_get_rx_prbs_en(const phymod_access_t *pa, uint8_t *enable);

static 
int merlin_quadra28_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val);

static 
int merlin_quadra28_config_tx_prbs(const phymod_access_t *pa, uint32_t prbs_poly_mode, uint8_t prbs_inv);

static 
int merlin_quadra28_config_rx_prbs(const phymod_access_t *pa, 
    uint32_t prbs_poly_mode, uint32_t prbs_checker_mode, uint8_t prbs_inv);

static 
int merlin_quadra28_pmd_mwr_reg(const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val);

/* PRBS Generator Enable/Disable */
static 
int merlin_quadra28_tx_prbs_en(const phymod_access_t *pa, uint8_t enable);

/* PRBS Generator Enable/Disable */
static 
int merlin_quadra28_rx_prbs_en(const phymod_access_t *pa, uint8_t enable);

static 
int merlin_quadra28_prbs_chk_lock_state(const phymod_access_t* pa, uint8_t *chk_lock);

static 
int merlin_quadra28_prbs_err_count_state(const phymod_access_t* pa, uint32_t *prbs_err_cnt, uint8_t *lock_lost);

static 
int merlin_quadra28_prbs_err_count_state(const phymod_access_t* pa, uint32_t *prbs_err_cnt, uint8_t *lock_lost);

static 
int merlin_quadra28_prbs_err_count_ll(const phymod_access_t* pa, uint32_t *prbs_err_cnt);

/**   read chip ID
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
uint32_t  _quadra28_get_chip_id(const phymod_access_t *pa)
{
    CHIP_ID0r_t chipid_lsb;
    CHIP_ID1r_t chipid_msb;
    int32_t chipid = 0;

    PHYMOD_MEMSET(&chipid_lsb, 0, sizeof(CHIP_ID0r_t));
    PHYMOD_MEMSET(&chipid_msb, 0, sizeof(CHIP_ID1r_t));

    PHYMOD_IF_ERR_RETURN(
            READ_CHIP_ID0r(pa, &chipid_lsb));
    PHYMOD_IF_ERR_RETURN(
            READ_CHIP_ID1r(pa, &chipid_msb));
    if (CHIP_ID0r_GET(chipid_lsb) == 0x2780) {
        chipid = CHIP_ID0r_GET(chipid_lsb);
        if (CHIP_ID1r_GET(chipid_msb) == 0x8) {
            chipid |= (CHIP_ID1r_GET(chipid_msb)) << 16;
        }
    }
    return chipid;
} 
/**   poll finish change 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
                
 *    @timeout                  timeout in us
 *    @return PHYMOD_E_NONE     successful function execution 
 */
static
int32 _quadra28_finish_change_wait_check(const phymod_access_t *pa ,
                   int32 timeout)          /* max wait time to check */
{
    uint16_t finish_change = 1;
    uint32_t timer_counter = 0;
    GENERAL_PURPOSE_3r_t gen_pur_reg_3r_val;
    
    
    PHYMOD_MEMSET(&gen_pur_reg_3r_val, 0, sizeof(GENERAL_PURPOSE_3r_t));
    timer_counter = timeout / DELAY_1_MS_FROM_US;
    do {
        PHYMOD_USLEEP(DELAY_1_MS_FROM_US);
        PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(pa,&gen_pur_reg_3r_val));
        finish_change = gen_pur_reg_3r_val.v[0] & Q28_BIT(7);
        timer_counter--;
    } while ((timer_counter > 0) && (finish_change != 0));
    /* Do a final read */
    PHYMOD_USLEEP(DELAY_1_MS_FROM_US);
    PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(pa,&gen_pur_reg_3r_val));
    finish_change = gen_pur_reg_3r_val.v[0] & Q28_BIT(7);   
    if (finish_change != 0){
        return PHYMOD_E_FAIL;
    }
    return PHYMOD_E_NONE;
} 
/**   poll intf update 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
                
 *    @timeout                  timeout in us
 *    @return PHYMOD_E_NONE     successful function execution 
 */
static
int32 _quadra28_intf_update_wait_check(const phymod_access_t *pa, uint32_t apps_mode0_reg_val,
                   int32 timeout)          /* max wait time to check */
{
    uint32_t timer_counter = 0;
    GENERAL_PURPOSE_3r_t gen_pur_reg_3r_val;
    timer_counter = (timeout / DELAY_1_MS_FROM_US);
    PHYMOD_MEMSET(&gen_pur_reg_3r_val, 0, sizeof(GENERAL_PURPOSE_3r_t));
    do {
        /* Read C843 register & compare its value with apps_mod0_reg_val */
        PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(pa,&gen_pur_reg_3r_val));   
        if(gen_pur_reg_3r_val.v[0] == apps_mode0_reg_val){
            break;
        }
        PHYMOD_USLEEP(DELAY_1_MS_FROM_US);
        timer_counter--;
    } while(timer_counter > 0);
    if (gen_pur_reg_3r_val.v[0] != apps_mode0_reg_val) {
        return PHYMOD_E_FAIL;
    }
 
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
int quadra28_set_config_mode(const phymod_access_t *pa,
                       phymod_interface_t intf,
                       uint32_t speed,
                       phymod_ref_clk_t ref_clk)
{

    uint16_t single_pmd_mode = 0;
    uint32_t pkg_side = 0;
    uint32_t acc_flags = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;

    PHYMOD_MEMSET(&apps_mode0_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    PHYMOD_IF_ERR_RETURN(READ_APPS_MODE_0r(&acc_cpy,&apps_mode0_reg_val));
    apps_mode0_reg_val.v[0] &= 0xFF7F;
    PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));
   
    /* Spin for the FINSH change to clear in 0xC843*/
    PHYMOD_IF_ERR_RETURN(_quadra28_finish_change_wait_check (&acc_cpy, 50000));
 
    /* Configure port speed */
    PHYMOD_IF_ERR_RETURN(_quadra28_config_port_speed (speed, \
        (uint32_t*)&apps_mode0_reg_val.v[0], (uint16_t*)&single_pmd_mode));

    if(LINE == pkg_side) {
        /* Configure line side interface */
        PHYMOD_IF_ERR_RETURN(_quadra28_config_line_interface((uint32_t*)&apps_mode0_reg_val.v[0], 
            intf, speed));
    } else {
        /* configure system side interface */
        PHYMOD_IF_ERR_RETURN(_quadra28_config_sys_interface((uint32_t*)&apps_mode0_reg_val.v[0], 
            intf));  
    }            

    /* Enable 4-bit data traffic path. This has to be enabled explicitly */
    apps_mode0_reg_val.v[0] |= Q28_BIT(6);

    /* Set bit 7 in C8D8 reg to update changes */
    apps_mode0_reg_val.v[0] |= 0x80;
    PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));
    PHYMOD_IF_ERR_RETURN(_quadra28_intf_update_wait_check(&acc_cpy,apps_mode0_reg_val.v[0], 50000));
  
    /* Program FINISH Change in 0xC8D8 */
    PHYMOD_IF_ERR_RETURN(READ_APPS_MODE_0r(&acc_cpy,&apps_mode0_reg_val));
    apps_mode0_reg_val.v[0] &= 0xFF7F;
    PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));

    /* configure clock */
    if (ref_clk == phymodRefClk156Mhz) {
        PHYMOD_IF_ERR_RETURN(
                _quadra28_refclk_set(&acc_cpy, phymodRefClk156Mhz));
    } else {
        PHYMOD_IF_ERR_RETURN(
                _quadra28_refclk_set(&acc_cpy, phymodRefClk155Mhz));
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
int quadra28_get_config_mode(const phymod_access_t *pa,
                       phymod_interface_t *intf,
                       uint32_t *speed,
                       phymod_ref_clk_t *ref_clk,
                       uint32_t *interface_modes)
{
    uint16_t ref_clk_scaler_val = 0; 
    uint32_t acc_flags = 0;
    int pkg_side = 0;
    APPS_MODE_2r_t apps_mod2_reg_val;
    GENERAL_PURPOSE_3r_t gen_pur_reg_3r_val;
    uint16_t config_speed  = 0;
    phymod_access_t acc_cpy;
   
    *speed = 0;
    *intf = 0;
    *ref_clk = 0; 

    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&gen_pur_reg_3r_val, 0, sizeof(GENERAL_PURPOSE_3r_t));
    PHYMOD_MEMSET(&apps_mod2_reg_val, 0 , sizeof(BCMI_QUADRA28_APPS_MODE_2r_t));

    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    
    PHYMOD_IF_ERR_RETURN(READ_APPS_MODE_2r(&acc_cpy, &apps_mod2_reg_val));
    ref_clk_scaler_val = APPS_MODE_2r_REFERENCE_CLOCKf_GET(apps_mod2_reg_val);
    
    PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(&acc_cpy, &gen_pur_reg_3r_val));
    
    /* Extract bits for speed */
    config_speed = (gen_pur_reg_3r_val.v[0] & 0xF);
    if (0x1 == config_speed){
        *speed = SPEED_1G_OS8;
    } else if (0x3 == config_speed) {
        *speed = SPEED_1G_OS8P25;
    } else if (0x2 == config_speed) {
        *speed = SPEED_10G;
    } else if (0x8 == config_speed) {
        *speed = SPEED_11P5G;
    } else if (0xC  == config_speed) {
        *speed = SPEED_20G;
    } else if (0x4  == config_speed) {
        *speed = SPEED_40G;
    } else if (0x7  == config_speed) {
        *speed = SPEED_42G;
    }
    /* A Quad cannot be configure in both 10 and 40G simulataneously,
     * so, gettinf and reading it again on proper PHY*/
    Q28_10G_PHY_ID_WORK_AROUND(*speed, acc_cpy);
    if (acc_cpy.addr != pa->addr) {
        PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(&acc_cpy, &gen_pur_reg_3r_val));
    }
    /* should use speed info & line side supported connections for Q28*/
    if (LINE == pkg_side){
        if ((SPEED_42G == *speed || SPEED_40G == *speed) && 
            (gen_pur_reg_3r_val.v[0] & Q28_BIT(14))) {
            *intf = phymodInterfaceLR4;
        } else if ((SPEED_42G == *speed || SPEED_40G == *speed) &&
            (gen_pur_reg_3r_val.v[0] & Q28_BIT(9))) {
            *intf = phymodInterfaceCR4;
            
        } else if ((SPEED_42G == *speed || SPEED_40G == *speed) &&
            (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(8))) &&
            (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(14)))) {
            *intf = phymodInterfaceSR4;
            
        } else if((SPEED_10G == *speed || SPEED_HG11 == *speed) &&
             (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(8)))) {
           *intf = phymodInterfaceSR;
        } else if((SPEED_10G == *speed || SPEED_HG11 == *speed) &&
             (gen_pur_reg_3r_val.v[0] & Q28_BIT(9))) {  /* Check this */
             *intf = phymodInterfaceCR;
        } else if((SPEED_10G == *speed || SPEED_HG11 == *speed) &&
             (gen_pur_reg_3r_val.v[0] & Q28_BIT(14))) {  
             *intf = phymodInterfaceLR;
        } else if((SPEED_10G == *speed || SPEED_HG11 == *speed) &&
             (gen_pur_reg_3r_val.v[0] & Q28_BIT(5))) {  /* Check this: No LRM in phymod.h enum*/
             /**intf = phymodInterfaceLRM; */
        } else if((SPEED_10G == *speed || SPEED_HG11 == *speed) &&
             (gen_pur_reg_3r_val.v[0] & Q28_BIT(9))) {  /* Check this */
             *intf = phymodInterfaceCR4;
        }
    } else if (SYS == pkg_side) {
       /* If speed is 40G or 42G, system side connections: XLAUI, Forced CL72 */
       if ((SPEED_40G == *speed || SPEED_42G == *speed) &&
            (gen_pur_reg_3r_val.v[0] & (Q28_BIT(11) | Q28_BIT(15)))) {
           *intf = phymodInterfaceXLAUI;
       } else if((SPEED_10G == *speed || SPEED_HG11 == *speed) &&
            (gen_pur_reg_3r_val.v[0] & Q28_BIT(5))) {  /* Check this */
            /* If speed is 10G, HG11 or HG10 it is XFI on system side */
           *intf = phymodInterfaceXFI;
       }
    }
    
    if (phymodRefClk156Mhz == ref_clk_scaler_val) {
        *ref_clk = phymodRefClk156Mhz;
    } else {
        *ref_clk = phymodRefClk155Mhz; /* check this */
    }
   
    return PHYMOD_E_NONE;
}

/**   Configure PRBS generator 
 *    This function is used to configure PRBS generator with user provided
 *    polynomial and invert data information 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
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
int quara28_prbs_config_set(const phymod_access_t *pa,
                        uint32_t flags,
                        uint32_t prbs_mode,
                        uint32_t prbs_inv)
{

    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE; /* num of lanes in octal package core */
    int lane_index = 0, pmd_mode = 0;
    unsigned short int lane_map = 0;
    uint32_t acc_flags = 0, speed = 0;
    MDIO_BROADCAST_CONTROLr_t  mdio_broadcast_ctrl;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    phymod_access_t acc_cpy;
    
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    QUADRA28_GET_IF_SIDE(acc_flags,pkg_side);

    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    PHYMOD_MEMSET(&mdio_broadcast_ctrl, 0, sizeof(MDIO_BROADCAST_CONTROLr_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    QUADRA28_SELECT_SIDE(&acc_cpy, side_sel_reg_val, pkg_side);
    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                if(lane_map == 0xF) {
                    /* Broadcast to be enabled in MGT unit */
                    /* &acc_cpy has addr of lane0/lane4 */
                    QUADRA28_EN_BROADCAST(&acc_cpy, mdio_broadcast_ctrl, 1);
                } else {
                    QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);
                }

                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
                    PHYMOD_IF_ERR_RETURN(
                            merlin_quadra28_config_tx_prbs(&acc_cpy, prbs_mode, (uint8_t)prbs_inv)); 
                }
                /* Following code block for PRBS Checker or RX*/
                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                    PHYMOD_IF_ERR_RETURN(
                            merlin_quadra28_config_rx_prbs(&acc_cpy, prbs_mode, PRBS_INITIAL_SEED_HYSTERESIS, (uint8_t)prbs_inv)); 
                }
                if(0xF == lane_map){
                    break;
                }
            }
        }
    } else {
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
            PHYMOD_IF_ERR_RETURN(
                merlin_quadra28_config_tx_prbs(&acc_cpy, prbs_mode, (uint8_t)prbs_inv)); 
        }
        /* Following code block for PRBS Checker or RX*/
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
            PHYMOD_IF_ERR_RETURN(
                merlin_quadra28_config_rx_prbs(&acc_cpy, prbs_mode, PRBS_INITIAL_SEED_HYSTERESIS, (uint8_t)prbs_inv)); 
        }
    }
    
    /* restore LINE side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,LINE);	

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
int quadra28_prbs_config_get(const phymod_access_t *pa,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum *prbs_mode,
                        uint32_t *prbs_inv)
{

    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    uint8_t prbs_invert = 0;
    uint32_t prbs_checker_mode = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    
    MDIO_BROADCAST_CONTROLr_t  mdio_broadcast_ctrl;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    *prbs_inv = 0;
    *prbs_mode = 0; 
    
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&mdio_broadcast_ctrl, 0 , sizeof(MDIO_BROADCAST_CONTROLr_t));	
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    
    QUADRA28_SELECT_SIDE(&acc_cpy, side_sel_reg_val, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);
                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
                    PHYMOD_IF_ERR_RETURN(
                            merlin_quadra28_get_tx_prbs_config(&acc_cpy, prbs_mode, &prbs_invert));
                    *prbs_inv = prbs_invert;
                }
                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                    /* Read Serdes API */
                    PHYMOD_IF_ERR_RETURN(
                            merlin_quadra28_get_rx_prbs_config(&acc_cpy, prbs_mode, &prbs_checker_mode, &prbs_invert));
                    *prbs_inv = prbs_invert;
                }
                break;
            }
        }
    } else {
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
            PHYMOD_IF_ERR_RETURN(
                    merlin_quadra28_get_tx_prbs_config(&acc_cpy, prbs_mode, &prbs_invert));
            *prbs_inv = prbs_invert;
        }
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
           /* Read Serdes API */
            PHYMOD_IF_ERR_RETURN(
                   merlin_quadra28_get_rx_prbs_config(&acc_cpy, prbs_mode, &prbs_checker_mode, &prbs_invert));
            *prbs_inv = prbs_invert;
        }
    }

    /* restore LINE side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,LINE);	
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
int quadra28_prbs_enable_set(const phymod_access_t *pa, uint32_t flags, uint32_t enable_state)
{
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE; /* num of lanes in octal package core */
    int lane_index = 0;
    uint32_t acc_flags = 0;
    int lane_map = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    SIDE_SELECTIONr_t side_sel_reg;
    BCMI_QUADRA28_PRBS_GEN_CONFIGr_t prbs_gen_config_reg_val;
    BCMI_QUADRA28_PRBS_CHK_CONFIGr_t prbs_chk_config_reg_val;
    MDIO_BROADCAST_CONTROLr_t mdio_broadcast_ctrl;
 
    PHYMOD_MEMSET(&prbs_gen_config_reg_val, 0, sizeof(BCMI_QUADRA28_PRBS_GEN_CONFIGr_t));
    PHYMOD_MEMSET(&prbs_chk_config_reg_val, 0, sizeof(BCMI_QUADRA28_PRBS_CHK_CONFIGr_t));
    PHYMOD_MEMSET(&side_sel_reg, 0, sizeof(SIDE_SELECTIONr_t));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
   
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags,pkg_side);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg, pkg_side);
    
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                if(lane_map == 0xF) {
                    QUADRA28_EN_BROADCAST(&acc_cpy, mdio_broadcast_ctrl, 1);
                } else {
                    QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);
                }
                /* Select respective side */
                if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
                    PHYMOD_IF_ERR_RETURN(
                      merlin_quadra28_tx_prbs_en(&acc_cpy, enable_state));
                }
                /* Following code block for PRBS Checker or RX*/
                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                    PHYMOD_IF_ERR_RETURN(
                       merlin_quadra28_rx_prbs_en(&acc_cpy, enable_state));
                }
            }
        }
    } else {
       /* Select respective side */
        if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
            PHYMOD_IF_ERR_RETURN(
              merlin_quadra28_tx_prbs_en(&acc_cpy, enable_state));
        }
        /* Following code block for PRBS Checker or RX*/
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
            PHYMOD_IF_ERR_RETURN(
               merlin_quadra28_rx_prbs_en(&acc_cpy, enable_state));
        }
    }
    /* restore LINE side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg,LINE);		
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
int quadra28_prbs_enable_get(const phymod_access_t *pa, uint32_t flags, uint32_t *enable)
{
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE; /* num of lanes in octal package core */
    int lane_index = 0;
    uint32_t acc_flags = 0;
    int lane_map = 0;
    MDIO_BROADCAST_CONTROLr_t  mdio_broadcast_ctrl;
    SIDE_SELECTIONr_t side_sel_reg;
    BCMI_QUADRA28_PRBS_GEN_CONFIGr_t prbs_gen_config_reg_val;
    BCMI_QUADRA28_PRBS_CHK_CONFIGr_t prbs_chk_config_reg_val;
    uint8_t gen_en = 0;
    uint8_t chk_en = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    
    *enable = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    QUADRA28_GET_IF_SIDE(acc_flags,pkg_side);
    
    PHYMOD_MEMSET(&prbs_gen_config_reg_val, 0, sizeof(BCMI_QUADRA28_PRBS_GEN_CONFIGr_t));
    PHYMOD_MEMSET(&prbs_chk_config_reg_val, 0, sizeof(BCMI_QUADRA28_PRBS_CHK_CONFIGr_t));
    PHYMOD_MEMSET(&side_sel_reg, 0, sizeof(BCMI_QUADRA28_PRBS_CHK_CONFIGr_t));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);
                if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_get_tx_prbs_en(&acc_cpy, &gen_en));
                    *enable = gen_en;
                }
                if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_get_rx_prbs_en(&acc_cpy, &chk_en));
                    *enable = chk_en;
                }
                break;				
            }
         }
    } else {
        if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_get_tx_prbs_en(&acc_cpy, &gen_en));
            *enable = gen_en;
        }
        if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_get_rx_prbs_en(&acc_cpy, &chk_en));
            *enable = chk_en;
        }
    }
    if (flags == 0) {
        if(gen_en && chk_en) {
            *enable = 1;
        } else {
            *enable = 0;
        }
    }
     /* restore LINE side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg,LINE);	
    return PHYMOD_E_NONE;
}


/**   Get PRBS lock and error status 
 *    This function is used to retrieve PRBS lock, loss of lock and error counts
 *    from the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param lock_status        PRBS lock status denotes PRBS generator and 
 *                              checker are locked to same polynomial data
 *    @param lock_loss          Lo of lock denotes PRBS generator and checker
 *                              are not in sync   
 *    @param error_count        PRBS error count retrieved from chip
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_status_get(const phymod_access_t *pa,
                                uint32_t *lock_status,
                                uint32_t *lock_loss,
                                uint32_t *error_count)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0; 
    uint32_t acc_flags = 0;
    uint8_t loss_of_lock = 0;
    uint32_t per_lane_err_count = 0;
    uint8_t prbs_chk_lock_stat;
    MDIO_BROADCAST_CONTROLr_t  mdio_broadcast_ctrl;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;

    *lock_status = 1;
    *lock_loss = 1;
    *error_count = 0;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));

    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    
    /* Select respective side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);
                   
                PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_chk_lock_state(&acc_cpy, (uint8_t*)&prbs_chk_lock_stat));
 
                *lock_status &= prbs_chk_lock_stat;
                
                PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_err_count_state(&acc_cpy, &per_lane_err_count, &loss_of_lock));
                *error_count |= per_lane_err_count;
                *lock_loss &= loss_of_lock;

            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_chk_lock_state(&acc_cpy, (uint8_t*)&prbs_chk_lock_stat));
        *lock_status &= prbs_chk_lock_stat;
        PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_err_count_state(&acc_cpy, &per_lane_err_count, &loss_of_lock));
        *error_count |= per_lane_err_count;
        *lock_loss &= loss_of_lock;
    }
    /* restore LINE side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,LINE);	
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
int quadra28_link_status(const phymod_access_t *pa, uint32_t *link_status)
{
    uint32_t acc_flags = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_map = 0;
    /*int pcs_status = 0; */
    int lane_index = 0;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    BCMI_QUADRA28_PMD_STATUSr_t ieee_pmd_status1_reg_val;
    BCMI_QUADRA28_PCS_STATUS1r_t pcs_status1_reg_val;	
    MDIO_BROADCAST_CONTROLr_t  mdio_broadcast_ctrl;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;

 
    PHYMOD_MEMSET(&side_sel_reg_val, 0, sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    PHYMOD_MEMSET(&ieee_pmd_status1_reg_val, 0, sizeof(BCMI_QUADRA28_PMD_STATUSr_t));
    PHYMOD_MEMSET(&pcs_status1_reg_val, 0, sizeof(BCMI_QUADRA28_PCS_STATUS1r_t)); 
    PHYMOD_MEMSET(&mdio_broadcast_ctrl, 0, sizeof(MDIO_BROADCAST_CONTROLr_t)); 
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);

    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,pkg_side);
    *link_status = 1;

    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                /* select a lane */
                QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);
                /* Get PCS status if PCS is ON */
                /*QUADRA28_IS_PCS_ON(&acc_cpy,pcs_status1_reg_val,&pcs_status);
                if(1 == pcs_status)
                {
                     PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_PCS_STATUS1r(&acc_cpy, &pcs_status1_reg_val));
                    *link_status &= BCMI_QUADRA28_PCS_STATUS1r_PCS_RECEIVE_LINK_STATUSf_GET(pcs_status1_reg_val);
                }*/
                /* Get PMD status */
                PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_PMD_STATUSr(&acc_cpy, &ieee_pmd_status1_reg_val));
                *link_status &= BCMI_QUADRA28_PMD_STATUSr_LN_RX_SIGDETf_GET(ieee_pmd_status1_reg_val);
             }
         }
    } else {
       /* Get PCS status if PCS is ON */
       /*QUADRA28_IS_PCS_ON(&acc_cpy,pcs_status1_reg_val,&pcs_status);
       if(1 == pcs_status)
       {
            PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_PCS_STATUS1r(&acc_cpy, &pcs_status1_reg_val));
           *link_status &= BCMI_QUADRA28_PCS_STATUS1r_PCS_RECEIVE_LINK_STATUSf_GET(pcs_status1_reg_val);
       }*/
       /* Get PMD status */
       PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_PMD_STATUSr(&acc_cpy, &ieee_pmd_status1_reg_val));
       *link_status &= BCMI_QUADRA28_PMD_STATUSr_LN_RX_SIGDETf_GET(ieee_pmd_status1_reg_val);
    }
    /* restore LINE side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,LINE);
    return PHYMOD_E_NONE;
    
}

int _quadra28_refclk_set(const phymod_access_t *pa, uint32_t ref_clk) 
{
    BCMI_QUADRA28_APPS_MODE_2r_t apps_mod2_reg_val;
       
    PHYMOD_MEMSET(&apps_mod2_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_2r_t));
    PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_APPS_MODE_2r(pa, &apps_mod2_reg_val));
    if (phymodRefClk156Mhz == ref_clk) {
        BCMI_QUADRA28_APPS_MODE_2r_REFERENCE_CLOCKf_SET(apps_mod2_reg_val,0);
    } else {
        BCMI_QUADRA28_APPS_MODE_2r_REFERENCE_CLOCKf_SET(apps_mod2_reg_val,1);
    }

    PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_WRITE_APPS_MODE_2r(pa, apps_mod2_reg_val));
   
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
int quadra28_tx_rx_polarity_set (const phymod_access_t *pa, uint32_t tx_polarity, uint32_t rx_polarity)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    BCMI_QUADRA28_TX_CTRL_0r_t tx_ctrl0r_config;
    MDIO_BROADCAST_CONTROLr_t  mdio_broadcast_ctrl;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&tx_ctrl0r_config, 0 , sizeof(BCMI_QUADRA28_TX_CTRL_0r_t));
    PHYMOD_MEMSET(&mdio_broadcast_ctrl, 0, sizeof(MDIO_BROADCAST_CONTROLr_t));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,pkg_side);
   
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                if(lane_map == 0xF) {
                    QUADRA28_EN_BROADCAST(&acc_cpy, mdio_broadcast_ctrl, 1);
                } else {
                    QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);
                }
                 
                /* Read 4bit tx config register for both line & sys side */
                PHYMOD_IF_ERR_RETURN
                    (BCMI_QUADRA28_READ_TX_CTRL_0r(&acc_cpy, &tx_ctrl0r_config));
                                       
                /* Program 4-bit Line Side Tx Polarity Flip */
                BCMI_QUADRA28_TX_CTRL_0r_LL_POLARITY_FLIPf_SET(tx_ctrl0r_config, tx_polarity);
                    
                /* Write to Rx config register - 4-bit */
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_WRITE_TX_CTRL_0r(&acc_cpy, tx_ctrl0r_config));
                /* By defaule ULL is enabled so 
                 * RX polarity not supported */

                if (0xF == lane_map) {
                    break;
                }
            }
        } /* End for */
    } else {
         
        /* Read 4bit tx config register for both line & sys side */
        PHYMOD_IF_ERR_RETURN
            (BCMI_QUADRA28_READ_TX_CTRL_0r(&acc_cpy, &tx_ctrl0r_config));
                               
        /* Program 4-bit Line Side Tx Polarity Flip */
        BCMI_QUADRA28_TX_CTRL_0r_LL_POLARITY_FLIPf_SET(tx_ctrl0r_config, tx_polarity);
            
        /* Write to Rx config register - 4-bit */
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_TX_CTRL_0r(&acc_cpy, tx_ctrl0r_config));
    }
    /* restore LINE side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,LINE);
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
int quadra28_tx_rx_polarity_get (const phymod_access_t *pa, uint32_t *tx_polarity, uint32_t *rx_polarity)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    MDIO_BROADCAST_CONTROLr_t  mdio_broadcast_ctrl;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    BCMI_QUADRA28_TX_CTRL_0r_t tx_ctrl0r_config;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&tx_ctrl0r_config, 0 , sizeof(BCMI_QUADRA28_TX_CTRL_0r_t));	
    PHYMOD_MEMSET(&mdio_broadcast_ctrl, 0, sizeof(MDIO_BROADCAST_CONTROLr_t));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    
    num_lanes = QUADRA28_MAX_LANE; /* for octal pkg */

    *tx_polarity = 0;
    *rx_polarity = 0;
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);

                PHYMOD_IF_ERR_RETURN
                  (BCMI_QUADRA28_READ_TX_CTRL_0r(&acc_cpy, &tx_ctrl0r_config));
                               
                /* Get 4-bit  Tx Polarity Flip */
                *tx_polarity = BCMI_QUADRA28_TX_CTRL_0r_LL_POLARITY_FLIPf_GET(tx_ctrl0r_config);
                /*rx polarity not applicable for 4-bit datapath*/
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN
              (BCMI_QUADRA28_READ_TX_CTRL_0r(&acc_cpy, &tx_ctrl0r_config));
                               
        /* Get 4-bit  Tx Polarity Flip */
        *tx_polarity = BCMI_QUADRA28_TX_CTRL_0r_LL_POLARITY_FLIPf_GET(tx_ctrl0r_config);
        /*rx polarity not applicable for 4-bit datapath*/

    }
    
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, LINE);
    return PHYMOD_E_NONE;
}

/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param rx_seq_done        RX sew done status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_pmd_lock_get(const phymod_access_t *pa, uint32_t *rx_seq_done) 
{
    uint8_t rx_pmd_lock = 0;
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    MDIO_BROADCAST_CONTROLr_t mdio_broadcast_ctrl;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    
    PHYMOD_MEMSET(&mdio_broadcast_ctrl, 0 , sizeof(MDIO_BROADCAST_CONTROLr_t));	
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
  
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);
 
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                QUADRA28_ADDRESS_LANE (&acc_cpy, mdio_broadcast_ctrl, lane_index);	            
                if(NULL != rx_seq_done) {
                    PHYMOD_IF_ERR_RETURN(
                        merlin_quadra28_pmd_lock_status(&acc_cpy, &rx_pmd_lock));
                    *rx_seq_done &= rx_pmd_lock;
                }
            }
        }
    } else {
        if(NULL != rx_seq_done) {
            PHYMOD_IF_ERR_RETURN(
                merlin_quadra28_pmd_lock_status(&acc_cpy, &rx_pmd_lock));
            *rx_seq_done &= rx_pmd_lock;
        }
    }
    /* restore LINE side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, LINE);
    return PHYMOD_E_NONE;
}

/**   Set line side interface  
 *    This function is used to set the operating mode of the PHY
 *
 *    @param reg_val            Pointer to apps mode reg 
 *    @param intf               Interface specified by user 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_config_line_interface(uint32_t* reg_val, phymod_interface_t intf, uint32_t speed)
{
    uint16_t return_val = PHYMOD_E_NONE;
    uint32_t mask = 0;
    if (speed == 10000) {
        if ((intf != phymodInterfaceSR) || (intf != phymodInterfaceCR) ||
                (intf != phymodInterfaceLR) || (intf != phymodInterfaceKR)) {
            PHYMOD_DIAG_OUT(("Interface %d not supported, so setting default interface \n", intf));
            intf = phymodInterfaceSR;
        }
    } else if(speed == 40000) {
        if ((intf != phymodInterfaceSR4) || (intf != phymodInterfaceCR4) ||
                (intf != phymodInterfaceLR4) || (intf != phymodInterfaceKR4)) {
            PHYMOD_DIAG_OUT(("Interface %d not supported, so setting default interface \n", intf));
            intf = phymodInterfaceSR4;
        }
    }

    switch (intf)
    {
        case phymodInterfaceSR:  
        case phymodInterfaceSR4:
            mask = ~(Q28_BIT(8) | Q28_BIT(14));
            *reg_val &= mask;
            break;
        case phymodInterfaceKX:
        case phymodInterfaceKR:
        case phymodInterfaceKR4:   
            mask = ~(Q28_BIT(8)); /* Bit 8 is for line type - Optical, Backplane */
            *reg_val &= mask;
            *reg_val |= 0x100;            
            break;
        case phymodInterfaceCR:
        case phymodInterfaceCX:
        case phymodInterfaceCR4:
            mask = ~(Q28_BIT(9)); /* Bit 9 is for line Cu type */
            *reg_val &= mask;
            *reg_val|= 0x200;
            break;
       
        case phymodInterfaceLR:       
        case phymodInterfaceLR4:
            mask = ~Q28_BIT(14); /* Bit 14 for line side LR type */
            *reg_val &= mask;
            *reg_val |= 0x4000;
            break;
               
        default:
            return_val = PHYMOD_E_FAIL;
            break;
    } 
    return return_val;
}

/**   Set sys side interface  
 *    This function is used to set the operating mode of the PHY
 *
 *    @param reg_val            Pointer to apps mode reg 
 *    @param intf               Interface specified by user 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_config_sys_interface(uint32_t* reg_val, phymod_interface_t intf)
{
    uint32_t mask = 0;
    uint16_t return_val = PHYMOD_E_NONE;         
    /* configure the line side port mode - interface type */
    switch (intf)
    {
        case phymodInterfaceXLAUI:
            mask = ~(Q28_BIT(11) | Q28_BIT(15));
            *reg_val &= mask;
            *reg_val |= 0x8800;
            break;
        case phymodInterfaceXFI:
            mask = ~(Q28_BIT(11));
            *reg_val &= mask;
            *reg_val |= 0x800;
            break;
        default:
            return_val = PHYMOD_E_FAIL;            
            break;
    } 
    return return_val;
}

/**   Set port speed  
 *    This function is used to set the operating mode of the PHY
 *
 *    @param reg_val            Pointer to apps mode reg 
 *    @param speed              Interface specified by user 
 *    @param mode_type          port mode (single vs multiple)
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_config_port_speed(uint32_t speed, uint32_t* reg_val, uint16_t* mode_type)
{
    uint16_t return_val = PHYMOD_E_NONE; 
    *reg_val &= ~(0xF);
    *mode_type = 0;
    switch (speed)
    {
        case SPEED_1G_OS8:
            *reg_val |= 0x1;
            break;

        case SPEED_1G_OS8P25:
            *reg_val |= 0x3;
            break;

        case SPEED_10G:
            *reg_val |= 0x2;
            break;

        case SPEED_11P5G:
            *reg_val |= 0x8;
            break;

        case SPEED_20G: /* KR2 2 * 10.3125G */
            *reg_val |= 0xC;
            break;

        case SPEED_40G:
            *reg_val |= 0x4;
            *mode_type = 1;
            break;

        case SPEED_42G:
            *reg_val |= 0x7;
            *mode_type = 1;
            break;
            
        default:
            return_val = PHYMOD_E_FAIL;
            break;

    }/* End: Switch for speed */
    return return_val;
}

static int merlin_quadra28_config_tx_prbs(const phymod_access_t *pa, 
                                           uint32_t prbs_poly_mode, uint8_t 
    prbs_inv) {
    /*wr_prbs_gen_mode_sel((uint8_t)prbs_poly_mode);*/        /* PRBS Generator mode sel */
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_mwr_reg(pa, 0xD0E1, 0xE, 1, (uint16_t)(prbs_poly_mode)));
    /*wr_prbs_gen_inv(prbs_inv);*/  /* PRBS Invert Enable/Disable */
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_mwr_reg(pa, 0xD0E1, 0x10, 4, (uint16_t)(prbs_inv)));
    /* To enable PRBS Generator */
    return (PHYMOD_E_NONE);
}

static int merlin_quadra28_config_rx_prbs(const phymod_access_t *pa, 
                             uint32_t prbs_poly_mode, uint32_t prbs_checker_mode, uint8_t prbs_inv) {
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_mwr_reg(pa, 0xD0D1, 0xE, 1, (uint8_t)(prbs_poly_mode)));
    /*wr_prbs_chk_mode_sel((uint8_t)prbs_poly_mode);*/       /* PRBS Checker Polynomial mode sel  */
    PHYMOD_IF_ERR_RETURN(
         merlin_quadra28_pmd_mwr_reg(pa, 0xD0D1, 0x60, 5, 0));
    /* wr_prbs_chk_mode((uint8_t)prbs_checker_mode); */       /* PRBS Checker mode sel (PRBS LOCK state machine select) */
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_mwr_reg(pa, 0xD0D1, 0x10, 4, (uint8_t)(prbs_inv)));
  return (PHYMOD_E_NONE);
}

/* PRBS Generator Enable/Disable */
static int merlin_quadra28_tx_prbs_en(const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_mwr_reg(pa, 0xD0E1, 0x1, 0, (uint8_t)(0x1)));
  } else {
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_mwr_reg(pa, 0xD0E1, 0x1, 0, (uint8_t)(0x0)));
  }
  return (PHYMOD_E_NONE);
}

/* PRBS Checker Enable/Disable */
static int merlin_quadra28_rx_prbs_en(const phymod_access_t* pa, uint8_t enable) {
  if (enable) {
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_mwr_reg(pa, 0xD0D1, 0x1, 0, (uint8_t)(0x1)));
  } else {
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_mwr_reg(pa, 0xD0D1, 0x1, 0, (uint8_t)(0x0)));
  }
  return (PHYMOD_E_NONE);
}

/* PRBS Checker Lock State */
static int merlin_quadra28_prbs_chk_lock_state(const phymod_access_t* pa, uint8_t *chk_lock) {
    uint16_t data = 0;
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_rdt_reg(pa, 0xD0D9, &data));
    *chk_lock = (data & 0x8000) >> 15;                  /* PRBS Checker Lock Indication 1 = Locked, 0 = Out of Lock */
    return (PHYMOD_E_NONE);
}

/* PRBS Error Count State  */
static int merlin_quadra28_prbs_err_count_state(const phymod_access_t* pa, uint32_t *prbs_err_cnt, uint8_t *lock_lost) {

  PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_err_count_ll(pa, prbs_err_cnt));
  *lock_lost    = (*prbs_err_cnt >> 31);
  *prbs_err_cnt = (*prbs_err_cnt & 0x7FFFFFFF);
  return (PHYMOD_E_NONE);
}

static int merlin_quadra28_prbs_err_count_ll(const phymod_access_t* pa, uint32_t *prbs_err_cnt) {
  uint16_t rddata;
  PHYMOD_IF_ERR_RETURN(
          merlin_quadra28_pmd_rdt_reg(pa, 0xD0DA, &rddata));
  *prbs_err_cnt = ((uint32_t) rddata)<<16;
  PHYMOD_IF_ERR_RETURN(merlin_quadra28_pmd_rdt_reg(pa, 0xD0DB, &rddata));
  (*prbs_err_cnt = (*prbs_err_cnt | rddata));
  return (PHYMOD_E_NONE);
}

static int merlin_quadra28_pmd_mwr_reg(const phymod_access_t *pa, 
                               uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val)
{
    val = val << lsb;
    PHYMOD_IF_ERR_RETURN(
         quadra28_reg_modify(pa, (0x10000 | (uint32_t) addr), val, mask));
    return(PHYMOD_E_NONE);
}
static int merlin_quadra28_get_rx_prbs_config(const phymod_access_t *pa, 
                            enum srds_prbs_polynomial_enum *prbs_poly_mode, 
                            enum srds_prbs_checker_mode_enum *prbs_checker_mode, uint8_t *prbs_inv)
{
    uint8_t val;
    uint16_t mask = 0;
    uint16_t reg_data = 0;
    /* D0D1, Bits 12, 13 */
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_rdt_reg(pa, 0xD0D1, &reg_data));
    mask = Q28_BIT(1) | Q28_BIT(2) | Q28_BIT(3);
    val = (reg_data & mask) >> 1;
    
    /* PRBS Checker Polynomial mode sel  */
    *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
    
    /* PRBS Checker mode sel (PRBS LOCK state machine select) */
    mask = Q28_BIT(5) | Q28_BIT(6);
    val = (reg_data & mask) >> 5;
  
    *prbs_checker_mode = (enum srds_prbs_checker_mode_enum)val;
    
    mask = Q28_BIT(4) ;
    val = (reg_data & mask) >> 4;
    /* PRBS Invert Enable/Disable */
    *prbs_inv = val;
    return (PHYMOD_E_NONE);
}

static int merlin_quadra28_get_tx_prbs_config(const phymod_access_t *pa, 
    enum srds_prbs_polynomial_enum *prbs_poly_mode, uint8_t *prbs_inv) 
{
    /*uint8_t val;*/
    uint16_t reg_data = 0;
    uint16_t mask = 0;
    uint16_t prbs_mode = 0;

    /*d0e1 bit 12,13*/
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_rdt_reg(pa, 0xD0E1, &reg_data));
 
    /* bits 12 and 13 */
    mask = Q28_BIT(1) | Q28_BIT(2)| Q28_BIT(3);
    prbs_mode = (reg_data & mask) >> 1;
    /* PRBS Generator mode sel */
    *prbs_poly_mode = (enum srds_prbs_polynomial_enum)prbs_mode;
   
    /* D0E1, Bits 11, 15 */
    mask = Q28_BIT(4);
    /* PRBS Invert Enable/Disable */
    *prbs_inv = (reg_data & mask) >> 4;  

     return (PHYMOD_E_NONE);
}

static int merlin_quadra28_pmd_lock_status(const phymod_access_t *pa, uint8_t *pmd_rx_lock) {
    uint16_t data = 0;
   
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_rdt_reg(pa, 0xD0DC, &data));
    if(data & Q28_BIT(0)) {
        *pmd_rx_lock = 1;
    } else {
        *pmd_rx_lock = 0;
    }    
    
  return PHYMOD_E_NONE;
}

/* Get PRBS Generator Enable/Disable */
static int merlin_quadra28_get_tx_prbs_en(const phymod_access_t *pa, uint8_t *enable) {
    
    uint16_t data = 0;
   
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_rdt_reg(pa, 0xD0E1, &data));
    *enable = (data & Q28_BIT(0));

    return (PHYMOD_E_NONE);
}

/* Get PRBS checker Enable/Disable */
static int merlin_quadra28_get_rx_prbs_en(const phymod_access_t *pa, uint8_t *enable) {
    
    uint16_t data = 0;
   
    PHYMOD_IF_ERR_RETURN(
        merlin_quadra28_pmd_rdt_reg(pa, 0xD0d1, &data));
    
    *enable = (data & Q28_BIT(0));

    return (PHYMOD_E_NONE);
}


static int merlin_quadra28_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val)
{
    uint32_t data;
    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_read(pa, (0x10000 | (uint32_t) address), &data));
    *val = (uint16_t)data;
    return (PHYMOD_E_NONE);
}
