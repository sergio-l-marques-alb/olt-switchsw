/*
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 *  $Id$
 */

/*
 *
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


/*
 * Includes
 */
#include <phymod/phymod.h>
#include <phymod/phymod_reg.h>
#include <phymod/phymod_diagnostics.h>
#include "huracan_cfg_seq.h"
#include "huracan_reg_access.h"
#include "bcmi_huracan_defs.h"


#ifndef PHYMOD_DIAG_OUT
#define PHYMOD_DIAG_OUT(a)  printf a
#endif


/* Take care of Register Address mapping */
#undef  PHYMOD_BUS_READ
#undef  PHYMOD_BUS_WRITE

#define hmr(__a) \
((__a) & 0x1ff00 ) == 0x1b000 ? (((__a) & ~(0xf000)) | 0x8000) : (__a) 

#define PHYMOD_BUS_READ(pa_,r_,v_) phymod_bus_read(pa_,hmr(r_),v_)
#define PHYMOD_BUS_WRITE(pa_,r_,v_) phymod_bus_write(pa_,hmr(r_),v_)


/*
 *  Defines
 */

/**   Get Revision ID
 *    This function retrieves Revision ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure
 *
 *    @param rev_id            Revision ID retrieved from the chip
 */
int huracan_rev_id(const phymod_access_t *pa, uint32_t *rev_id)
{

    BCMI_HURACAN_CHIP_REVISIONr_t chip_revision;
    PHYMOD_MEMSET(&chip_revision, 0 , sizeof(BCMI_HURACAN_CHIP_REVISIONr_t));

    /* Read the chip revision from register */
    PHYMOD_IF_ERR_RETURN(
            READ_CHIP_REVISIONr(pa, &chip_revision));
    *rev_id = BCMI_HURACAN_CHIP_REVISIONr_CHIP_REVf_GET(chip_revision);
    return PHYMOD_E_NONE;
}




int huracan_channel_select (const phymod_access_t *pa, uint16_t lane)
{
    return PHYMOD_E_NONE;
}

/**   read chip ID
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
uint32_t  _huracan_get_chip_id(const phymod_access_t *pa)
{
    int32_t chipid = 0;
    CHIP_IDr_t chipid_lsb;

    PHYMOD_MEMSET(&chipid_lsb, 0, sizeof(CHIP_IDr_t));


    PHYMOD_IF_ERR_RETURN(
            READ_CHIP_IDr(pa, &chipid_lsb));

    if (CHIP_IDr_GET(chipid_lsb) == 0x2109) {
        chipid = CHIP_IDr_GET(chipid_lsb);
    }

    return chipid;
} 

/**   poll intf update 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
                
 *    @timeout                  timeout in us
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _huracan_intf_update_wait_check(const phymod_access_t *pa, uint32_t apps_mode0_reg_val,
                   int32_t timeout)          /* max wait time to check */
{

    return PHYMOD_E_FAIL;
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
int huracan_set_config_mode(const phymod_access_t *pa,
                       phymod_interface_t intf,
                       uint32_t speed,
                       phymod_ref_clk_t ref_clk)
{
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
int huracan_get_config_mode(const phymod_access_t *pa,
                       phymod_interface_t *intf,
                       uint32_t *speed,
                       phymod_ref_clk_t *ref_clk,
                       uint32_t *interface_modes)
{
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
int huracan_link_status(const phymod_access_t *pa, uint32_t *link_status)
{

    BCMI_HURACAN_ANA_LANE_0_QUAD_A_STATUS0r_t ieee_pmd_status1_reg_val;
    int lane_map = 0;
    int num_lanes = 0;
    /*int pcs_status = 0; */
    int lane_index = 0;
    uint32_t reg_addr = 0;
    uint8_t lane_bitmap = 0;
    uint8_t quad = 0;

 
    PHYMOD_MEMSET(&ieee_pmd_status1_reg_val, 0, sizeof(BCMI_HURACAN_ANA_LANE_0_QUAD_A_STATUS0r_t));

    *link_status = 1;
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    num_lanes = 8  ; /* FURIA_IS_SIMPLEX(chip_id) ? 8 : 4; */

    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {

            quad        = lane_index  >> 2 ;
            lane_bitmap = (0x1  <<  (lane_index & 0x3));
            reg_addr = BCMI_HURACAN_ANA_LANE_0_QUAD_A_STATUS0r &  ~((0xF) << 8) ;
            reg_addr |= ((quad & 0x7) << 12) | ((lane_bitmap & 0xF) << 8) ;

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_READ(pa,
                       reg_addr,
                       &ieee_pmd_status1_reg_val._ana_lane_0_quad_a_status0));

            *link_status &= BCMI_HURACAN_ANA_LANE_0_QUAD_A_STATUS0r_DG_RX_SIGDETf_GET(ieee_pmd_status1_reg_val);

        }
    }

    return PHYMOD_E_NONE;
    
}

int _huracan_refclk_set(const phymod_access_t *pa, uint32_t ref_clk) 
{
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
int huracan_tx_rx_polarity_set (const phymod_access_t *pa, uint32_t tx_polarity, uint32_t rx_polarity)
{
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
int huracan_tx_rx_polarity_get (const phymod_access_t *pa, uint32_t *tx_polarity, uint32_t *rx_polarity)
{
    return PHYMOD_E_NONE;
}

/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param rx_seq_done        RX sew done status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int huracan_pmd_lock_get(const phymod_access_t *pa, uint32_t *rx_seq_done) 
{
    return PHYMOD_E_NONE;
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
int _huracan_config_port_speed(uint32_t speed, uint32_t* reg_val, uint16_t* mode_type)
{
    return PHYMOD_E_NONE; 
}

int huracan_tx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_tx_lane_control_t tx_control)
{

    BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_t ieee_pmd_cmd1_reg_val;
    int lane_map = 0;
    int num_lanes = 0;
    /*int pcs_status = 0; */
    int lane_index = 0;
    uint32_t reg_addr = 0;
    uint8_t lane_bitmap = 0;
    uint8_t quad = 0;
    uint8_t squelch = 0;

    PHYMOD_MEMSET(&ieee_pmd_cmd1_reg_val, 0, sizeof(BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_t));


    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    num_lanes = 8  ; /* FURIA_IS_SIMPLEX(chip_id) ? 8 : 4; */

    switch(tx_control) {
        case phymodTxSquelchOn:
            squelch = 1;
            break;
        case phymodTxSquelchOff:
        default:
            squelch = 0;
            break;
    }

    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {

            quad        = lane_index  >> 2 ;
            lane_bitmap = (0x1  <<  (lane_index & 0x3));
            reg_addr = BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r &  ~((0xF) << 8) ;
            reg_addr |= ((quad & 0x7) << 12) | ((lane_bitmap & 0xF) << 8) ;

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_READ(pa,
                       reg_addr,
                       &ieee_pmd_cmd1_reg_val._ana_lane_0_quad_a_ctrl14));


            BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_TX_IDLE_FRCf_SET(
                       ieee_pmd_cmd1_reg_val, 1  );

            BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_TX_IDLE_FRC_VALf_SET(
                       ieee_pmd_cmd1_reg_val,
                       squelch ?  1  : 0 );

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_WRITE(pa,
                       reg_addr,
                       ieee_pmd_cmd1_reg_val._ana_lane_0_quad_a_ctrl14));

            if  (squelch) {
                continue;
            }

            BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_TX_IDLE_FRCf_SET(
                       ieee_pmd_cmd1_reg_val, 0  );

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_WRITE(pa,
                       reg_addr,
                       ieee_pmd_cmd1_reg_val._ana_lane_0_quad_a_ctrl14));
        }
    }
    return PHYMOD_E_NONE;    
}

int huracan_tx_squelch_get(const phymod_access_t *pa, int *tx_squelch)
{

    BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_t ieee_pmd_cmd1_reg_val;
    int lane_map = 0;
    int num_lanes = 0;
    /*int pcs_status = 0; */
    int lane_index = 0;
    uint32_t reg_addr = 0;
    uint8_t lane_bitmap = 0;
    uint8_t quad = 0;

    PHYMOD_MEMSET(&ieee_pmd_cmd1_reg_val, 0, sizeof(BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_t));

    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    num_lanes = 8  ; /* FURIA_IS_SIMPLEX(chip_id) ? 8 : 4; */

    *tx_squelch = 0;

    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {

            quad        = lane_index  >> 2 ;
            lane_bitmap = (0x1  <<  (lane_index & 0x3));
            reg_addr = BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r &  ~((0xF) << 8) ;
            reg_addr |= ((quad & 0x7) << 12) | ((lane_bitmap & 0xF) << 8) ;

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_READ(pa,
                       reg_addr,
                       &ieee_pmd_cmd1_reg_val._ana_lane_0_quad_a_ctrl14));


            if (
                BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_TX_IDLE_FRCf_GET(
                       ieee_pmd_cmd1_reg_val) &&

                BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r_TX_IDLE_FRC_VALf_GET(
                       ieee_pmd_cmd1_reg_val) ) {

                *tx_squelch = 1;
                break;

            }


        }
    }
    return PHYMOD_E_NONE;    
}


int huracan_rx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_rx_lane_control_t rx_control)
{
    return PHYMOD_E_UNAVAIL;
}

int huracan_rx_squelch_get(const phymod_access_t *pa, int *rx_squelch)
{
    return PHYMOD_E_UNAVAIL;
}


int _huracan_phy_diagnostics_get(const phymod_access_t *pa, phymod_phy_diagnostics_t* diag)
{
    return PHYMOD_E_NONE;
}

int huracan_hard_reset(const phymod_access_t* pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    BCMI_HURACAN_GEN_CONTROL1r_t gen_control_reg_val;
    /*int pcs_status = 0; */
    uint32_t reg_addr = 0;
    uint8_t reset = 0;
 
    PHYMOD_MEMSET(&gen_control_reg_val, 0, sizeof(BCMI_HURACAN_GEN_CONTROL1r_t));

    if (reset_mode == phymodResetModeHard) {
        switch (direction) {
            case phymodResetDirectionIn:
                reset = 1;
                break;
            case phymodResetDirectionInOut:
                reset = 2;
                break;
            default:
                reset = 0;
                break;
        }
    } else {
        return PHYMOD_E_UNAVAIL;
    }


    reg_addr = BCMI_HURACAN_GEN_CONTROL1r ;

    PHYMOD_IF_ERR_RETURN(
        PHYMOD_BUS_READ(pa,
               reg_addr,
               &gen_control_reg_val._gen_control1));


    BCMI_HURACAN_GEN_CONTROL1r_RESETBf_SET(
               gen_control_reg_val,
               reset ?  0  : 1 );


    PHYMOD_IF_ERR_RETURN(
        PHYMOD_BUS_WRITE(pa,
               reg_addr,
               gen_control_reg_val._gen_control1));

    return PHYMOD_E_NONE;
}

int _huracan_phy_reset_set(const phymod_access_t *pa,  const phymod_phy_reset_t* reset)
{
    return PHYMOD_E_NONE;
}


int _huracan_phy_reset_get( const phymod_access_t *pa,  phymod_phy_reset_t* reset)
{
    return PHYMOD_E_NONE;
}


int _huracan_loopback_set(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t enable)
{
    return PHYMOD_E_NONE;
}

int _huracan_loopback_get(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return PHYMOD_E_NONE;
}

int _huracan_phy_power_set(const phymod_access_t* pa, const phymod_phy_power_t* power)
{

    BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_t ieee_pmd_cmd1_reg_val;
    int lane_map = 0;
    int num_lanes = 0;
    /*int pcs_status = 0; */
    uint32_t acc_flags = 0; 
    int pkg_side = 0;
    int lane_index = 0;
    uint32_t reg_addr = 0;
    uint8_t lane_bitmap = 0;
    uint8_t quad = 0;
    uint8_t pwrdn = 0;

    PHYMOD_MEMSET(&ieee_pmd_cmd1_reg_val, 0, sizeof(BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_t));

    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    HURACAN_GET_IF_SIDE(acc_flags, pkg_side);

    num_lanes = 8  ; /* FURIA_IS_SIMPLEX(chip_id) ? 8 : 4; */

    if(pkg_side == SIDE_A) {
        if (power->tx == phymodPowerOff) {
            pwrdn = 1;
        } else if (power->tx == phymodPowerOffOn) {
            pwrdn = 2;
        } else {
            pwrdn = 0;
        }
    } else {
        if (power->rx == phymodPowerOff) {
            pwrdn = 1;
        } else if (power->rx == phymodPowerOffOn) {
            pwrdn = 2;
        } else {
            pwrdn = 0;
        }
    }


    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {

            quad        = lane_index  >> 2 ;
            lane_bitmap = (0x1  <<  (lane_index & 0x3));
            reg_addr = BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r &  ~((0xF) << 8) ;
            reg_addr |= ((quad & 0x7) << 12) | ((lane_bitmap & 0xF) << 8) ;

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_READ(pa,
                       reg_addr,
                       &ieee_pmd_cmd1_reg_val._ana_lane_0_quad_a_ctrl0));


            if(pkg_side == SIDE_A) {
                BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_RXTX_PWRDNf_SET(
                       ieee_pmd_cmd1_reg_val, pwrdn ? 1 : 0);
            } else {
                BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_RX_SIGDET_PWRDNf_SET(
                       ieee_pmd_cmd1_reg_val, pwrdn ? 1 : 0);
            }

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_WRITE(pa,
                       reg_addr,
                       ieee_pmd_cmd1_reg_val._ana_lane_0_quad_a_ctrl0));


            if (pwrdn <  2) {
                continue;
            }


            /* Sleep */
            PHYMOD_USLEEP(500);

            if(pkg_side == SIDE_A) {
                BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_RXTX_PWRDNf_SET(
                       ieee_pmd_cmd1_reg_val, 0);
            } else {
                BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_RX_SIGDET_PWRDNf_SET(
                       ieee_pmd_cmd1_reg_val, 0);
            }

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_WRITE(pa,
                       reg_addr,
                       ieee_pmd_cmd1_reg_val._ana_lane_0_quad_a_ctrl0));
        }
    }

    return PHYMOD_E_NONE;
    
}


int _huracan_phy_power_get(const phymod_access_t* pa, phymod_phy_power_t* power)
{

    BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_t ieee_pmd_cmd1_reg_val;
    int lane_map = 0;
    int num_lanes = 0;
    /*int pcs_status = 0; */
    int lane_index = 0;
    uint32_t reg_addr = 0;
    uint8_t lane_bitmap = 0;
    uint8_t quad = 0;

    PHYMOD_MEMSET(&ieee_pmd_cmd1_reg_val, 0, sizeof(BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_t));

    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    num_lanes = 8  ; /* FURIA_IS_SIMPLEX(chip_id) ? 8 : 4; */

    power->rx = phymodPowerOn;
    power->tx = phymodPowerOn;

    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {

            quad        = lane_index  >> 2 ;
            lane_bitmap = (0x1  <<  (lane_index & 0x3));
            reg_addr = BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r &  ~((0xF) << 8) ;
            reg_addr |= ((quad & 0x7) << 12) | ((lane_bitmap & 0xF) << 8) ;

            PHYMOD_IF_ERR_RETURN(
                PHYMOD_BUS_READ(pa,
                       reg_addr,
                       &ieee_pmd_cmd1_reg_val._ana_lane_0_quad_a_ctrl0));

            if (power->rx != phymodPowerOff) {
                power->rx =
                BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_RX_SIGDET_PWRDNf_GET(
                        ieee_pmd_cmd1_reg_val) != 0 ?
                         phymodPowerOff : phymodPowerOn ;
            }
            if (power->tx != phymodPowerOff) {
                power->tx =  
                BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r_RXTX_PWRDNf_GET(
                        ieee_pmd_cmd1_reg_val) != 0 ?
                         phymodPowerOff : phymodPowerOn ;
            }

        }
    }

    return PHYMOD_E_NONE;

}


int huracan_gpio_config_set(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t gpio_mode)
{
    BCMI_HURACAN_GPIO_0_CONTROLr_t pad_ctrl_gpio_ctrl;

    if (pin_no > 1) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Huracan has only 2 GPIOs (0 - 1)")));
    }

    PHYMOD_IF_ERR_RETURN
        (PHYMOD_BUS_READ(pa,\
                         (pin_no*2) + BCMI_HURACAN_GPIO_0_CONTROLr,\
                         &pad_ctrl_gpio_ctrl._gpio_0_control));
    switch (gpio_mode) {
      case phymodGpioModeDisabled :
          return PHYMOD_E_UNAVAIL;
      case phymodGpioModeOutput :
          BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_OEBf_SET(pad_ctrl_gpio_ctrl, 0);
      break;     
      case phymodGpioModeInput :
          BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_OEBf_SET(pad_ctrl_gpio_ctrl, 1);
      break;    
      default:
          return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN
        (PHYMOD_BUS_WRITE(pa,\
                          (pin_no*2) + BCMI_HURACAN_GPIO_0_CONTROLr,\
                          pad_ctrl_gpio_ctrl._gpio_0_control));
    return PHYMOD_E_NONE;
}

int huracan_gpio_config_get(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
    BCMI_HURACAN_GPIO_0_CONTROLr_t pad_ctrl_gpio_ctrl;
  
    *gpio_mode = 0;
    if (pin_no > 1) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Huracan has only 2 GPIOs (0 - 1)")));
    }

    PHYMOD_IF_ERR_RETURN
        (PHYMOD_BUS_READ(pa,\
                         (pin_no*2) + BCMI_HURACAN_GPIO_0_CONTROLr,\
                         &pad_ctrl_gpio_ctrl._gpio_0_control));
    
    switch(BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_OEBf_GET(pad_ctrl_gpio_ctrl)) {
        case 0:
            *gpio_mode = phymodGpioModeOutput; 
        break;
        default:
            *gpio_mode = phymodGpioModeInput;
        break;
    }

    return PHYMOD_E_NONE;
}

int huracan_gpio_pin_value_set(const phymod_access_t *pa, int pin_no, int value)
{
    BCMI_HURACAN_GPIO_0_CONTROLr_t pad_ctrl_gpio_ctrl;

    if (pin_no > 1) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Huracan has only 2 GPIOs (0 - 1)")));
    }
    PHYMOD_IF_ERR_RETURN
        (PHYMOD_BUS_READ(pa,\
                         (pin_no*2) + BCMI_HURACAN_GPIO_0_CONTROLr,\
                         &pad_ctrl_gpio_ctrl._gpio_0_control));

    BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_IBOFf_SET( pad_ctrl_gpio_ctrl, 1);

    BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_OUT_FRCVALf_SET(
            pad_ctrl_gpio_ctrl, 
            (value & 1));

    switch(value >> 1) {
        case 0:
         BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_PUPf_SET( pad_ctrl_gpio_ctrl, 0);
         BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_PDNf_SET( pad_ctrl_gpio_ctrl, 1);
            break;
        case 1:
         BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_PUPf_SET( pad_ctrl_gpio_ctrl, 1);
         BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_PDNf_SET( pad_ctrl_gpio_ctrl, 0);
            break;
        case 2:
         BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_PUPf_SET( pad_ctrl_gpio_ctrl, 0);
         BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_PDNf_SET( pad_ctrl_gpio_ctrl, 0);
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN
        (PHYMOD_BUS_WRITE(pa,\
                          (pin_no*2) + BCMI_HURACAN_GPIO_0_CONTROLr,\
                          pad_ctrl_gpio_ctrl._gpio_0_control));
    return PHYMOD_E_NONE;
}

int huracan_gpio_pin_value_get(const phymod_access_t *pa, int pin_no, int* value)
{
    BCMI_HURACAN_GPIO_0_STATUSr_t pad_sts_gpio;
    *value = 0;
   
    if (pin_no > 1) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Huracan has only 2 GPIOs (0 - 1)")));
    }

    PHYMOD_IF_ERR_RETURN
        (PHYMOD_BUS_READ(pa,\
                         (pin_no*2) + BCMI_HURACAN_GPIO_0_STATUSr,\
                         &pad_sts_gpio._gpio_0_status));

    *value = BCMI_HURACAN_GPIO_0_STATUSr_GPIO_0_DINf_GET(pad_sts_gpio);

    {
        BCMI_HURACAN_GPIO_0_CONTROLr_t pad_ctrl_gpio_ctrl;
        PHYMOD_IF_ERR_RETURN
            (PHYMOD_BUS_READ(pa,\
                             (pin_no*2) + BCMI_HURACAN_GPIO_0_CONTROLr,\
                             &pad_ctrl_gpio_ctrl._gpio_0_control));

        if (BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_PUPf_GET(pad_ctrl_gpio_ctrl)){
            *value |= (1<<1);
        } else 
        if (!BCMI_HURACAN_GPIO_0_CONTROLr_GPIO_0_PDNf_GET(pad_ctrl_gpio_ctrl)){
            *value |= (2<<1);
        }
    }
    return PHYMOD_E_NONE;
}


int _huracan_pll_seq_restart(const phymod_access_t *pa, uint32_t flag, phymod_sequencer_operation_t operation)
{
    return PHYMOD_E_NONE;
}

int _huracan_phy_tx_set(const phymod_access_t* pa, const phymod_tx_t* tx)
{
    return PHYMOD_E_NONE;
}

int _huracan_phy_tx_get(const phymod_access_t* pa, phymod_tx_t* tx) 
{
    return PHYMOD_E_NONE;
}

int _huracan_phy_rx_set(const phymod_access_t* pa, const phymod_rx_t* rx)
{
    return PHYMOD_E_NONE;
}

int _huracan_phy_rx_get(const phymod_access_t* pa, phymod_rx_t* rx)
{
    return PHYMOD_E_NONE;
}


int _huracan_phy_status_dump(const phymod_access_t *pa)
{
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    uint32_t reg_addr = 0;
    uint32_t value = 0;
    int num_lanes = 0;
    int lane_index = 0;
    int lane_map = 0;
    uint8_t lane_bitmap = 0;
    uint8_t quad = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _huracan_get_chip_id(pa);
    huracan_rev_id(pa , &rev_id);
    num_lanes = 8 ; /*FURIA_IS_SIMPLEX(chip_id) ? 8 : 4; */

    PHYMOD_DIAG_OUT(("**********************************************\n"));
    PHYMOD_DIAG_OUT(("******* PHY status dump for PHY ID:%d ********\n",pa->addr));
    PHYMOD_DIAG_OUT(("**********************************************\n"));
    /* Program the slice register */
    PHYMOD_DIAG_OUT(("*******************************************\n"));
    PHYMOD_DIAG_OUT(("****** PHY status dump for chip/rev: %X  %X *********\n",
            chip_id, rev_id));
    PHYMOD_DIAG_OUT(("***********************************************\n"));

    reg_addr = BCMI_HURACAN_GPIO_0_CONTROLr ;
    PHYMOD_IF_ERR_RETURN( PHYMOD_BUS_READ(pa, reg_addr, &value));
    PHYMOD_DIAG_OUT(("GPIO_0_CONTROLr       :reg%08x: value=%04x\n",
                     hmr(reg_addr), value));
    
    reg_addr = BCMI_HURACAN_GPIO_0_STATUSr ;
    PHYMOD_IF_ERR_RETURN( PHYMOD_BUS_READ(pa, reg_addr, &value));
    PHYMOD_DIAG_OUT(("GPIO_0_STATUSr        :reg%08x: value=%04x\n",
                     hmr(reg_addr), value));
    
    reg_addr = BCMI_HURACAN_GPIO_1_CONTROLr ;
    PHYMOD_IF_ERR_RETURN( PHYMOD_BUS_READ(pa, reg_addr, &value));
    PHYMOD_DIAG_OUT(("GPIO_1_CONTROLr       :reg%08x: value=%04x\n",
                     hmr(reg_addr), value));
    
    reg_addr = BCMI_HURACAN_GPIO_1_STATUSr ;
    PHYMOD_IF_ERR_RETURN( PHYMOD_BUS_READ(pa, reg_addr, &value));
    PHYMOD_DIAG_OUT(("GPIO_1_STATUSr        :reg%08x: value=%04x\n",
                     hmr(reg_addr), value));
    
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {

            quad        = lane_index  >> 2 ;
            lane_bitmap = (0x1  <<  (lane_index & 0x3));

            /* Power */
            reg_addr = BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL0r &  ~((0xF) << 8);
            reg_addr |= ((quad & 0x7) << 12) | ((lane_bitmap & 0xF) << 8) ;

            PHYMOD_IF_ERR_RETURN( PHYMOD_BUS_READ(pa, reg_addr, &value));

            PHYMOD_DIAG_OUT(("ln%d:qd%d:reg%08x: value=%04x\n",
                    lane_index, quad, hmr(reg_addr), value));

            /* Link */
            reg_addr = BCMI_HURACAN_ANA_LANE_0_QUAD_A_STATUS0r &  ~((0xF) << 8);
            reg_addr |= ((quad & 0x7) << 12) | ((lane_bitmap & 0xF) << 8) ;

            PHYMOD_IF_ERR_RETURN( PHYMOD_BUS_READ(pa, reg_addr, &value));

            PHYMOD_DIAG_OUT(("ln%d:qd%d:reg%08x: value=%04x\n",
                    lane_index, quad, hmr(reg_addr), value));

            /* Squelch */
            reg_addr = BCMI_HURACAN_ANA_LANE_0_QUAD_A_CTRL14r &  ~((0xF) << 8) ;
            reg_addr |= ((quad & 0x7) << 12) | ((lane_bitmap & 0xF) << 8) ;

            PHYMOD_IF_ERR_RETURN( PHYMOD_BUS_READ(pa, reg_addr, &value));

            PHYMOD_DIAG_OUT(("ln%d:qd%d:reg%08x: value=%04x\n",
                    lane_index, quad, hmr(reg_addr), value));
        }
    }
    return PHYMOD_E_NONE;
}

