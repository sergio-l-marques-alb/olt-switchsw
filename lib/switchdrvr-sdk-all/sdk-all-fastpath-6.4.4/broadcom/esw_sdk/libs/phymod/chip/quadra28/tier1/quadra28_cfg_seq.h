/*
 *         
 * $Id: quadra_cfg_seq.h 2014/01/17 aman Exp $
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
 
#ifndef __QUADRA28_CFG_SEQ_H__
#define __QUADRA28_CFG_SEQ_H__

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include "quadra28_types.h"
/*
 *  Global Variables
 */

/*
 *  Functions
 */

/*
 *  Functions for Manipulating Chip/Port Cfg Descriptors
 */

/*
 * Functions
 */

/*
 *  Functions for Manipulating Chip/Port Cfg Descriptors
 */



#define QUADRA28_IS_DUAL(devid) \
((devid) == QUADRA28_ID_82071 ||\
 (devid) == QUADRA28_ID_82315   \
)

#define QUADRA28_IS_QUAD(devid)\
((devid) == QUADRA28_ID_82071 ||\
 (devid) == QUADRA28_ID_82314 ||\
 (devid) == QUADRA28_ID_82315   \
)

#define QUADRA28_IS_OCTAL(devid)\
((devid) == QUADRA28_ID_82071 ||\
 (devid) == QUADRA28_ID_82070 ||\
 (devid) == QUADRA28_ID_82314 ||\
 (devid) == QUADRA28_ID_82315   \
)

#define QUADRA28_GET_IF_SIDE(_FLAGS, _SIDE)                             \
{                                                                  \
    if(((_FLAGS >> 31) & 0x1) == 0x1) {   \
        _SIDE = SYS;                                               \
    } else {                                                       \
        _SIDE = LINE;                                              \
    }                                                              \
}
#define QUADRA28_PMD_ID0    0xae02
#define QUADRA28_PMD_ID1    0x5250
#define QUADRA28_CHIP_ID    0x82780

#define QUADRA28_NOF_LANES_IN_CORE 4   
#define QUADRA28_PHY_ALL_LANES     0xf 
#define DELAY_1_MS_FROM_US 1000

/** PRBS Polynomial Enum */
enum srds_prbs_polynomial_enum {
  SRDS_PRBS_7  = 0,
  SRDS_PRBS_9  = 1,
  SRDS_PRBS_11 = 2,
  SRDS_PRBS_15 = 3,
  SRDS_PRBS_23 = 4,
  SRDS_PRBS_31 = 5,
  SRDS_PRBS_58 = 6
};

/** PRBS Checker Mode Enum */
enum srds_prbs_checker_mode_enum {
  PRBS_SELF_SYNC_HYSTERESIS       = 0,
  PRBS_INITIAL_SEED_HYSTERESIS    = 1,
  PRBS_INITIAL_SEED_NO_HYSTERESIS = 2
};


enum LINE_SELECTION{
    LINE = 0,
    SYS = 1
}; 

#define SIDE_SELECT(pa,r,side){                                                    \
    if(LINE == side) {                                                             \
            PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_SIDE_SELECTIONr(pa,&r));       \
            BCMI_QUADRA28_SIDE_SELECTIONr_SIDE_SELf_SET(r,LINE);                   \
            PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_WRITE_SIDE_SELECTIONr(pa,r));       \
        }                                                                          \
        else {                                                                     \
            PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_SIDE_SELECTIONr(pa,&r));       \
            BCMI_QUADRA28_SIDE_SELECTIONr_SIDE_SELf_SET(r,LINE);                   \
            PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_WRITE_SIDE_SELECTIONr(pa,r));       \
        }                                                                          \
}

#define QUADRA28_EN_BROADCAST(pa,r,enable)  {                               \
    PHYMOD_IF_ERR_RETURN(                                                   \
                 READ_MDIO_BROADCAST_CONTROLr(pa,&r));                      \
    if (1 == enable) {                                                      \
        BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MCAST_ENf_SET(r,0);      \
        BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(r, 1);     \
    } else {                                                                \
        BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(r,0);      \
    }                                                                       \
    PHYMOD_IF_ERR_RETURN(WRITE_MDIO_BROADCAST_CONTROLr(pa, r));             \
}	   

#define QUADRA28_ADDRESS_LANE(_pa,r, lane_index) {                                  \
    PHYMOD_IF_ERR_RETURN(                                                           \
                 READ_MDIO_BROADCAST_CONTROLr(pa,&r));                              \
    BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(r,0);                  \
    BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MCAST_ENf_SET(r,1);                  \
    BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MULTICAST_PHYADf_SET(r, lane_index); \
    PHYMOD_IF_ERR_RETURN(WRITE_MDIO_BROADCAST_CONTROLr(pa, r));                     \
}

#define QUADRA28_IS_PCS_ON(pa,r, status){                 \
    PHYMOD_IF_ERR_RETURN(                                 \
                 BCMI_QUADRA28_READ_PCS_STATUS1r(pa,&r)); \
    *status = BCMI_QUADRA28_PCS_STATUS1r_GET(r);          \
}

#define QUADRA28_SELECT_SIDE(pa, r, _side) {              \
    if(LINE == _side) {                                   \
        SIDE_SELECT(pa,r,LINE);                           \
    } else {                                              \
        SIDE_SELECT(pa,r,SYS);                            \
    }                                                     \
}

#define Q28_UNUSED_PARAM(x)         (void)(x)

#define Q28_GET_PMD_MODE(m_acc, pmd_mode)                                                  \
    {                                                                                      \
        phymod_interface_t m_intf;                                                         \
        phymod_ref_clk_t m_ref_clk;                                                        \
        uint32_t m_intf_modes;                                                             \
        PHYMOD_IF_ERR_RETURN (                                                             \
          quadra28_get_config_mode(m_acc, &m_intf, &speed, &m_ref_clk, &m_intf_modes));    \
        Q28_UNUSED_PARAM(m_intf);                                                          \
        Q28_UNUSED_PARAM(m_ref_clk);                                                       \
        Q28_UNUSED_PARAM(m_intf_modes);                                                    \
        if (speed == SPEED_10G) {                                                          \
            pmd_mode = Q28_MULTI_PMD;                                                      \
        } else {                                                                           \
            pmd_mode = Q28_SINGLE_PMD;                                                     \
        }                                                                                  \
    }

#define Q28_10G_PHY_ID_WORK_AROUND(speed, acc)     \
    if (speed == 10000) {                          \
        if (acc.lane_mask == 0x2) {                \
            acc.addr += 1;                         \
        }                                          \
        if (acc.lane_mask == 0x4) {                \
            acc.addr += 2;                         \
        }                                          \
        if (acc.lane_mask == 0x8) {                \
            acc.addr += 3;                         \
        }                                          \
    }

#define Q28_BIT(X) (1<<X)

int _quadra28_config_line_interface(uint32_t* reg_val, phymod_interface_t intf, uint32_t speed);
int _quadra28_config_sys_interface(uint32_t* reg_val, phymod_interface_t intf);
int _quadra28_config_port_speed(uint32_t speed, uint32_t* reg_val, uint16_t* mode_type);
int quadra28_micro_download(const phymod_access_t *pa,
                                 uint8_t *new_fw,
                                 uint32_t fw_length,
                                 uint8_t prg_eeprom);

int quadra28_firmware_info_get(const phymod_access_t *pa, phymod_core_firmware_info_t *fw_info)
;
/**   Get Revision ID 
 *    This function retrieves Revision ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return rev_id            Revision ID retrieved from the chip
 */
uint16_t _quadra28_get_rev_id(const phymod_access_t *pa);

/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return chip_id           Chip id retrieved from the chip
 */
uint32_t _quadra28_get_chip_id(const phymod_access_t *pa);

/**   Reset Chip 
 *    This function is used to reset entire chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_chip_reset(const phymod_access_t *pa);	
	
/**   Reset Register 
 *    This function is used to perform register reset 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_register_rst(const phymod_access_t *pa);


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
/*int quadra28_prbs_config_set(const phymod_access_t *pa,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum prbs_mode,
                        uint32_t prbs_inv);*/


/**   Enable PRBS generator and PRBS checker 
 *    This function is used to enable or disable PRBS generator and PRBS checker
 *    as requested by the user  
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param enable             Enable or disable as specified by the user
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_enable_set(const phymod_access_t *pa, uint32_t flags, uint32_t enable);

/**   Get Enable status of PRBS generator and PRBS checker 
 *    This function is used to retrieve the enable status of PRBS generator and
 *    PRBS checker
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param *enable            Enable or disable read from chip 
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_enable_get(const phymod_access_t *pa, uint32_t flags, uint32_t *enable);



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
int quadra28_prbs_status_get(const phymod_access_t *pa,
                                uint32_t *lock_status,
                                uint32_t *lock_loss,
                                uint32_t *error_count);

/**   Get lane descriptor 
 *    This function is used to retrieve lane descriptor from package lane 
 *
 *    @param chip_id         chip id number 
 *    @param pa             Pointer to phymod access structure 
 *    @param pkg_lane       Package lane number 
 *
 *    @return pkg_ln_des    Lane descriptor structure contains the info
 *                              about package lane and die lane mapping 
 */
/*const struct _quadra28_PKG_LANE_CFG_S* _quadra28_pkg_ln_des(uint32_t chip_id, const phymod_access_t *pa, int pkg_lane);*/

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
int quadra28_chk_phy_link_mode_status(const phymod_access_t *pa,
                                   uint32_t *link_status); 

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
                          phymod_ref_clk_t ref_clk);

/**   Get config mode 
 *    This function is used to retrieve the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf               Interface type 
 *    @param speed              Speed val retrieved from PHY 
 *    @param ref_clk            Reference clock frequency to set 
 *                              the PHY into specified interface 
 *                              and speed
 *    @param interface_modes    mode which is currently unused
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_get_config_mode(const phymod_access_t *pa,
                       phymod_interface_t *intf,
                       uint32_t *speed,
                       phymod_ref_clk_t *ref_clk,
                       uint32_t *interface_modes);

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
int quara28_prbs_config_set(const phymod_access_t *pc,
                        uint32_t flags,
                        uint32_t prbs_mode,
                        uint32_t prbs_inv);

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
#if 1
int quadra28_prbs_config_get(const phymod_access_t *pa,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum *prbs_mode,
                        uint32_t *prbs_inv);
						
#endif					
/**   Get phymod interface type from pma type 
 *    This function is used to retrieve PHYMOD interface type from pma_type
 *
 *    @param pma_type           pma type read from mode register 
 *    @param intf               PHYMOD interface type 
 *    @param speed              Speed val 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_get_phymod_interface_type(uint16_t pma_type,
                                     phymod_interface_t *intf,
                                     uint32_t *speed);


/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param rx_seq_done        RX sew done status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_pmd_lock_get(const phymod_access_t *pa, uint32_t *rx_seq_done); 									 
									 /**   Get pma type from interface and speed 
 *    This function is used to get the pma type from interface and speed. 
 *    This pma type will be programmed to mode register for different PHY modes 
 *
 *    @param intf               PHYMOD interface type 
 *    @param speed              Speed val 
 * 
 *    @return pma_typeselection pma type calculated with interface and speed 
 */
uint16_t _quadra28_get_pma_type_selection(phymod_interface_t intf, uint32_t speed);


int quadra28_tx_rx_polarity_set (const phymod_access_t *pa,
                              uint32_t tx_polarity,
                              uint32_t rx_polarity);


int quadra28_tx_rx_polarity_get (const phymod_access_t *pa,
                              uint32_t *tx_polarity,
                              uint32_t *rx_polarity);

	
int quadra28_link_status(const phymod_access_t *pa, uint32_t *link_status);

int quadra28_reg_read(const phymod_access_t *pa, uint32_t addr, uint32_t  *val);

int quadra28_reg_write(const phymod_access_t *pa, uint32_t addr, uint32_t  val);

int _quadra28_refclk_set(const phymod_access_t *pa, uint32_t ref_clk) ;

int quadra28_get_chip_id(const phymod_access_t *pa);
#endif /*__QUADRA28_CFG_SEQ_H__*/

