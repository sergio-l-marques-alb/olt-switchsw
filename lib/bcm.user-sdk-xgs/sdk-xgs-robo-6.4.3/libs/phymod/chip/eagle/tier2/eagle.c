/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
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
 */

#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/phymod_util.h>
#include <phymod/chip/bcmi_eagle_xgxs_defs.h>

#include "../../eagle/tier1/eagle_cfg_seq.h"
#include "../../eagle/tier1/eagle_tsc_enum.h"
#include "../../eagle/tier1/eagle_tsc_common.h"
#include "../../eagle/tier1/eagle_tsc_interface.h"
#include "../../eagle/tier1/eagle_tsc_dependencies.h"

#define EAGLE_ID0        0
#define EAGLE_ID1        0
#define EAGLE_MODEL    0x1b
#define EAGLE_REV_MASK   0x0
#define TSCE_PHY_ALL_LANES 0xf
 
#define TSCE_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type = (_core_access)->type; \
        (_phy_access)->access.lane_mask = TSCF_PHY_ALL_LANES; \
    }while(0)

#define TSCE_NOF_DFES 9
#define TSCE_NOF_LANES_IN_CORE 4
extern unsigned char  tsce_ucode[];
extern unsigned short tsce_ucode_len;


#ifdef PHYMOD_EAGLE_SUPPORT

int eagle_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
        
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    uint32_t id2 = 0;
    uint32_t id3 = 0;
    eagle_rev_id0_t rev_id0; 
    eagle_rev_id1_t rev_id1; 

    *is_identified = 0;

    if(core_id == 0){
        ioerr += PHYMOD_BUS_READ(pm_acc, 2, &id2);
        ioerr += PHYMOD_BUS_READ(pm_acc, 3, &id3);
    }
    else{
        id2 = (core_id >> 16) & 0xffff;
        id3 = core_id & 0xffff;
    }

    if (id2 == EAGLE_ID0 &&
        id3 == EAGLE_ID1) {
        /* PHY IDs match - now check model */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_identify(pm_acc, &rev_id0, &rev_id1));
        if (rev_id0.revid_model == EAGLE_MODEL)  {
                *is_identified = 1;
        }
    }
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
}

int eagle_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_firmware_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_mode)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_firmware_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_mode)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_rx_restart(const phymod_phy_access_t* phy)
{
    return PHYMOD_E_NONE;
}


int eagle_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_tx_disable_set(const phymod_phy_access_t* phy, uint32_t tx_disable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_tx_disable_get(const phymod_phy_access_t* phy, uint32_t* tx_disable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_tx_lane_control_set(const phymod_phy_access_t* phy, uint32_t enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_tx_lane_control_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

/*Rx control*/
int eagle_phy_rx_lane_control_set(const phymod_phy_access_t* phy, uint32_t enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_rx_lane_control_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_interface_config_get(const phymod_phy_access_t* phy, 
                                   uint32_t flags, 
                                   phymod_ref_clk_t ref_clock, 
                                   phymod_phy_inf_config_t* config)
{
        
    
    /* Place your code here */
    config->ref_clock = ref_clock;

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


#endif /* PHYMOD_EAGLE_SUPPORT */
