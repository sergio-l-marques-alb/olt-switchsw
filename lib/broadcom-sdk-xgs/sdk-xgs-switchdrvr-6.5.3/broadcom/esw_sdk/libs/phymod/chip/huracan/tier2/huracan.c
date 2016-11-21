/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
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
 *         
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>

#ifdef PHYMOD_HURACAN_SUPPORT


#include <phymod/chip/huracan.h>

/* To be defined */
/* #include "../tier1/huracan_types.h" */
#include "../tier1/huracan_cfg_seq.h"
#include "../tier1/huracan_reg_access.h"
#include "../tier1/bcmi_huracan_defs.h"




int huracan_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
        
    uint32_t chip_id = 0;
    chip_id =  _huracan_get_chip_id(&core->access);

    *is_identified = 0;
    /* APO cretae macro for 0x82109 ../tier1/hurican_types.h*/
    if (chip_id == 0x2109 || chip_id == 0x2181)
    {
        /* PHY IDs match */
        *is_identified = 1;
    }
        
    return PHYMOD_E_NONE;
    
}


int huracan_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    uint32_t chip_id;

    chip_id =  _huracan_get_chip_id(&core->access);
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionHuracan;
    
        
    return PHYMOD_E_NONE;
    
}

int huracan_phy_interface_config_set(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   const phymod_phy_inf_config_t* config)
{
    return PHYMOD_E_NONE;
    
}

/**   Get Interface config 
 *    This function retrieves interface configuration(interface, speed and 
 *    frequency of operation) from the device. 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use 
 *    @param config             Interface config structure where  
 *                              interface, speed and the frequency of operation
 *                              will be populated from device .
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int huracan_phy_interface_config_get(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   phymod_ref_clk_t ref_clock,
                                   phymod_phy_inf_config_t* config)
{
    return PHYMOD_E_NONE;
}



int huracan_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
        
    PHYMOD_IF_ERR_RETURN (
         huracan_hard_reset(&core->access, reset_mode, direction));
        
    return PHYMOD_E_NONE;
    
}

int huracan_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_UNAVAIL;

}


int huracan_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    return _huracan_phy_power_set(&phy->access, power);
}

int huracan_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return _huracan_phy_power_get(&phy->access, power);
}


int huracan_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    return huracan_tx_lane_control_set(&phy->access, tx_control);
}

int huracan_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    int enable = 0;
    *tx_control = phymodTxSquelchOn;

    PHYMOD_IF_ERR_RETURN (
       huracan_tx_squelch_get(&phy->access, &enable));
    
    if (!enable) {
        *tx_control = phymodTxSquelchOff;
    }

    return PHYMOD_E_NONE;
    
}


int huracan_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    return huracan_rx_lane_control_set(&phy->access, rx_control);
}

int huracan_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int enable = 0;
    *rx_control = phymodRxSquelchOn;
    PHYMOD_IF_ERR_RETURN (
       huracan_rx_squelch_get(&phy->access, &enable));
    if (!enable) {
        *rx_control = phymodRxSquelchOff;
    }
    return PHYMOD_E_NONE;
    
}


int huracan_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
        
    PHYMOD_IF_ERR_RETURN (
         huracan_hard_reset(&core->access,
                            phymodResetModeHard, phymodResetDirectionInOut));

        
    return PHYMOD_E_NONE;
    
}


int huracan_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int huracan_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
        
    return PHYMOD_E_NONE;
    
}


int huracan_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
        
    return huracan_link_status(&phy->access, link_status);
    
}


int huracan_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    return huracan_reg_read(&phy->access,\
                  HURACAN_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
                  val);
    
}


int huracan_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    return huracan_reg_write(&phy->access,\
                  HURACAN_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
                  val);
    
}


int huracan_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode)
{
        
    return huracan_gpio_config_set(&phy->access, pin_no, gpio_mode);
}

int huracan_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t *gpio_mode)
{
    return huracan_gpio_config_get(&phy->access, pin_no, gpio_mode);
}


int huracan_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value)
{
    return huracan_gpio_pin_value_set(&phy->access, pin_no, value);
}

int huracan_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value)
{
    return huracan_gpio_pin_value_get(&phy->access, pin_no, value);
}


/**  PHY rev id 
 *   PHY revision id This function is used to get revision id of a chip
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param rev_id             Pointer to get revision id 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int huracan_phy_rev_id(const phymod_phy_access_t* phy, uint32_t *rev_id)
{
    return huracan_rev_id(&phy->access, rev_id);
}

int huracan_phy_status_dump(const phymod_phy_access_t* phy)
{
        return _huracan_phy_status_dump(&phy->access);
}


#endif /* PHYMOD_HURACAN_SUPPORT */
