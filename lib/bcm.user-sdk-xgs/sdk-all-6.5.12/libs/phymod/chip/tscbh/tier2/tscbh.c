/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/chip/bcmi_tscbh_xgxs_defs.h>
#include <phymod/chip/tscbh.h>
#include "blackhawk/tier1/blackhawk_cfg_seq.h"
#include "blackhawk/tier1/blackhawk_tsc_enum.h"
#include "blackhawk/tier1/blackhawk_tsc_common.h"
#include "blackhawk/tier1/blackhawk_tsc_interface.h"
#include "blackhawk/tier1/blackhawk_tsc_dependencies.h"
#include "blackhawk/tier1/blackhawk_tsc_internal.h"
#include "blackhawk/tier1/public/blackhawk_api_uc_vars_rdwr_defns_public.h"
#include "blackhawk/tier1/blackhawk_tsc_access.h"




extern unsigned char blackhawk_ucode[];
extern unsigned short blackhawk_ucode_len;
extern unsigned short blackhawk_ucode_crc;


#define TSCBH_MODEL               0x26
#define TSCBH_NOF_LANES_IN_CORE   0x8
#define TSCBH_PHY_ALL_LANES       0xff
#define TSCBH_TX_TAP_NUM          12


#define TSCBH_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = TSCBH_PHY_ALL_LANES; \
    }while(0)


int tscbh_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    return PHYMOD_E_NONE;

}


int tscbh_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    return PHYMOD_E_NONE;
    
}


int tscbh_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
     return PHYMOD_E_NONE;
}


int tscbh_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return PHYMOD_E_NONE;
    
}

int tscbh_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_NONE;
    
}



int tscbh_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
  return PHYMOD_E_NONE; 
}


int tscbh_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t *tx_control)
{

    return PHYMOD_E_NONE;
}

/*Rx control*/
int tscbh_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{

    return PHYMOD_E_NONE;
}

int tscbh_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    return PHYMOD_E_NONE;
    
}

int tscbh_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{
    return PHYMOD_E_NONE;
    
}

int tscbh_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    return PHYMOD_E_NONE;
}

#if 0
/* load tscf fw. the fw_loader parameter is valid just for external fw load*/

STATIC
int _tscbh_core_firmware_load(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config)
{
    return PHYMOD_E_NONE;
}

#endif

int tscbh_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{

    return PHYMOD_E_NONE; 
}


int tscbh_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    return PHYMOD_E_NONE; 
}

int tscbh_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{

    return PHYMOD_E_NONE;
}

int tscbh_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
        
    return PHYMOD_E_NONE;
        
}

/* 
 * set lane swapping for core 
 */

int tscbh_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{

    return PHYMOD_E_NONE;
}


#if 0

STATIC
int _tscbh_speed_config_get(uint32_t speed, uint32_t *pll_multiplier, uint32_t *is_pam4, uint32_t *osr_mode)
{
    return PHYMOD_E_NONE;
}

#endif

int _tscbh_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{

    return PHYMOD_E_NONE;
}


/* reset rx sequencer 
 * flags - unused parameter
 */
int tscbh_phy_rx_restart(const phymod_phy_access_t* phy)
{

    return PHYMOD_E_NONE;
}


int tscbh_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{

    return PHYMOD_E_NONE;
}



int tscbh_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{

    return PHYMOD_E_NONE;
}

int tscbh_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
        
    return PHYMOD_E_UNAVAIL;
}


int tscbh_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    
    return PHYMOD_E_UNAVAIL;
    
}


int tscbh_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    return PHYMOD_E_UNAVAIL;
}

int tscbh_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return PHYMOD_E_UNAVAIL;
}

int tscbh_phy_speed_config_set(const phymod_phy_access_t* phy,
                                   const phymod_phy_speed_config_t* speed_config,
                                   const phymod_phy_pll_state_t* old_pll_state,
                                   phymod_phy_pll_state_t* new_pll_state)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_speed_config_get(const phymod_phy_access_t* phy, phymod_phy_speed_config_t* speed_config)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return PHYMOD_E_NONE;
}

int tscbh_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
        
    return PHYMOD_E_NONE;
}

int tscbh_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{

    return PHYMOD_E_NONE;
    
}


/* this function gives the PMD_RX_LOCK_STATUS */
int tscbh_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* pmd_lock)
{
    return PHYMOD_E_NONE;
    
}

/* this function gives the PMD_RX_LOCK_STATUS */
int tscbh_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* signal_detect)
{
    return PHYMOD_E_NONE;
    
}

int tscbh_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    return PHYMOD_E_NONE;
}


int tscbh_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    return PHYMOD_E_NONE;  
}





