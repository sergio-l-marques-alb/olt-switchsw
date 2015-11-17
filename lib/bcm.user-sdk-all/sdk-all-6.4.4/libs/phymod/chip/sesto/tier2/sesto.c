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

#ifdef PHYMOD_SESTO_SUPPORT

#include "../tier1/sesto_address_defines.h"
#include "../tier1/sesto_reg_structs.h"
#include "../tier1/sesto_cfg_seq.h"

#define SESTO_PMD_ID0          0xAE02
#define SESTO_PMD_ID1          0x5290
#define SESTO_CHIP_ID_1        0x82764
#define SESTO_CHIP_ID_2        0x82792
#define SESTO_CHIP_ID_3        0x82790

/**   Core identify 
 *    This function reads PMD Identifiers, Set is identified if
 *    sesto identified. 
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param core_id            Core ID which is supplied by interface layer 
 *    @param is_identified      Flag variable to return device 
 *                              identification status 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    const phymod_access_t *pm_acc = &core->access;
    uint32_t id0 = 0, chip_id = 0;
    uint32_t id1 = 0;
    uint32_t rev = 0;
    
    *is_identified = 0;

    if (core_id == 0) {
        READ_SESTO_PMA_PMD_REG(pm_acc, IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_0_ADR, id0);
        READ_SESTO_PMA_PMD_REG(pm_acc, IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_1_ADR, id1);
    } else {
        id0 = (core_id >> 16) & 0xffff;
        id1 = core_id & 0xffff;
    }
    if (id0 == SESTO_PMD_ID0 && id1 == SESTO_PMD_ID1) {
        /* PHY IDs match - now check model */
        PHYMOD_IF_ERR_RETURN(sesto_get_chipid(pm_acc, &chip_id, &rev));
        if (chip_id == SESTO_CHIP_ID_1 || chip_id == SESTO_CHIP_ID_2 ||
	        chip_id == SESTO_CHIP_ID_3)  {
            *is_identified = 1;
        }
    }
    return PHYMOD_E_NONE;         
}

/**   Core information 
 *    This function gives the core version
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param info               Pointer to core version 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    info->core_version = phymodCoreVersionSestoA2; 
    
    return PHYMOD_E_NONE;
}


int sesto_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int sesto_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**   Core Reset 
 *    This function reset sesto core, it support hard and soft reset
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param reset_mode         Represent hard/soft reset to perform
 *    @param direction          Represet direction of reset. Sesto ignore this parameter
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return _sesto_core_reset_set(&core->access, reset_mode, direction);   
    
}

int sesto_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**   Get Firmware info
 *    This function get the firmware information such as master firmware version and master checksum
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param fw_info            Represent firmware version and checksum.
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    SES_GEN_CNTRLS_FIRMWARE_VERSION_TYPE_T firmware_version;
    SES_GEN_CNTRLS_MST_RUNNING_CHKSUM_TYPE_T mst_running_chksum;

    /* Read the firmware version */
    READ_SESTO_PMA_PMD_REG(&core->access, SES_GEN_CNTRLS_FIRMWARE_VERSION_ADR,
                                     firmware_version.data);
    fw_info->fw_version = firmware_version.data;

    READ_SESTO_PMA_PMD_REG(&core->access,
                                SES_GEN_CNTRLS_MST_RUNNING_CHKSUM_ADR,
                                mst_running_chksum.data);

    fw_info->fw_crc = mst_running_chksum.data; 

    
    return PHYMOD_E_NONE;
}

/**  Restart PLL Sequecer
 *   This function is used to get restart the PLL sequencer
 *
 *
 *    @param core                Pointer to phymod core access structure
 *    @param flags               Reserved for Furtuer use
 *    @param operation           Operation to perform on PLL sequencer
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return _sesto_pll_sequencer_restart(&core->access, operation);
}


int sesto_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int sesto_phy_rx_restart(const phymod_phy_access_t* phy)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**  PHY polarity set
 *   This function is used to set the lane polarity
 *
 *   @param phy          Pointer to phymod phy access structure
 *   @param polarity     Pointer to phymod_polarity_t for rx and tx
 * 
 *   @return PHYMOD_E_NONE     successful function execution
 */


int sesto_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
   return _sesto_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity);
}

/**  PHY polarity get
 *   This function is used to get the lane polarity
 *
 *   @param phy          Pointer to phymod phy access structure
 *   @param polarity     Pointer to phymod_polarity_t for rx and tx
 *
 *   @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
   return _sesto_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity);
}

/**  Tx set
 *   This function is used to set transmitter analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    return _sesto_tx_set(&phy->access, tx);    
}

/**  Tx get
 *   This function is used to get transmitter analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    return _sesto_tx_get(&phy->access, tx);    
}


int sesto_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int sesto_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int sesto_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**  Rx set
 *   This function is used to set receiver analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    return _sesto_rx_set(&phy->access, rx);    
}

/**  Rx get
 *   This function is used to get receiver analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    return _sesto_rx_get(&phy->access, rx);    
}

/**  Tx Rx Phy Reset Set
 *   This function is used to set TX/RX Phy Reset options
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reset              Pointer to phymod phy reset structure
 *                              to set TX Reset and RX Reset
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    return _sesto_phy_reset_set(&phy->access, reset);
}
/**  Tx Rx Phy Reset Get
 *   This function is used to get TX/RX Phy Reset options
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reset              Pointer to phymod phy reset structure
 *                              to get TX Reset and RX Reset
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    return _sesto_phy_reset_get(&phy->access, reset);
}

/**  Tx Rx power Set
 *   This function is used to set TX/RX power options
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param power              Pointer to phymod phy power structure
 *                              to set TX Power and RX Power
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    PHYMOD_IF_ERR_RETURN(_sesto_tx_power_set(&phy->access, power->tx));
    PHYMOD_IF_ERR_RETURN(_sesto_rx_power_set(&phy->access, power->rx));
    return PHYMOD_E_NONE;
}

/**  Tx Rx power Get
 *   This function is used to get TX/RX power options
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param power              Pointer to phymod phy power structure
 *                              to get TX Power and RX Power
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return _sesto_tx_rx_power_get(&phy->access, power);
}

/**  Tx lane control set
 *   This function is used to set the lane control for example
 *   resetting the traffic
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *          0 - Traffic disable, currently not available
 *          1 - Traffic enable, currently not available
 *          2 - Tx Datapath reset
 *          3 - Tx Squelch on
 *          4 - Tx Squelch off
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    return _sesto_tx_lane_control_set(&phy->access, tx_control);
}

/**  Tx lane control get
 *   This function is used to get the lane control for example
 *   resetting the traffic
 *
 *  @param phy                Pointer to phymod phy access structure
 *  @param tx_control
 *          0 - Traffic disable, currently not available
 *          1 - Traffic enable, currently not available
 *          2 - Tx Datapath reset
 *          3 - Tx Squelch on
 *          4 - Tx Squelch off
 *
 *          Return value of tx_control is enable or disable of specified lane control option
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */


int sesto_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    return _sesto_tx_lane_control_get(&phy->access, tx_control);
}

/**  Rx lane control set
 *   This function is used to set the lane control for example
 *   resetting the traffic
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx_control         Rx lane control
 *          0 - Rx datapath reset
 *          1 - Rx squelch on
 *          2 - Rx squelch off
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    return _sesto_rx_lane_control_set(&phy->access, rx_control);
}
/**  Rx lane control get
 *   This function is used to set the lane control for example
 *   resetting the traffic
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx_control         Rx lane control
 *          0 - Rx datapath reset
 *          1 - Rx squelch on
 *          2 - Rx squelch off
 *
 *          Return value of tx_control is enable or disable of specified lane control option
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    return _sesto_rx_lane_control_get(&phy->access, rx_control);
}

/**   Set interface config
 *    This function sets interface configuration (interface, speed, frequency of
 *    operation, PT mode, BCM84793 etc). 
 *    This is requried to put the chip into specific mode specified
 *
 *    NOTE: Default system side interface type will be IEEE mode and speed is compatible with lane side configured speed.
 *     Ex: User configured 40G speed with interface type KR4 on lane side then system side speed is 40G and interface type is IEEE mode.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Reserved for future use
 *    @param config             Interface config structure where user specifies 
 *                                - Interface Type
 *                                - Speed 
 *                                - Frequency.
 *                                - device_aux_modes : a structure that have auxilary mode details
 *                                   -> pass_thru : To enable repeater on 10/40G mode of operation
 *                                   -> gearbox_100g_inverse_mode : Applicable only for 100G mode, 25G lanes on system side and
 *                                      10G lanes on line side
 *                                   -> BCM84793_capablity : To enable BCM84793 compatible on lanes
 *                                   -> passthru_sys_side_core : When pass through is enabled user are allowed to configure system side
 *                                      core i.e. 25G lanes or 10G lanes
 *                                   -> reserved: Only for future use
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
   uint16_t data1 = 0, retry_cnt = 5;
 
    PHYMOD_IF_ERR_RETURN (
            _sesto_phy_interface_config_set(phy, flags, config));

    /* Enable FW After configuring mode */
    PHYMOD_IF_ERR_RETURN(
        _sesto_fw_enable(&phy->access, SESTO_ENABLE)); 
    do {
        SESTO_CHIP_FIELD_READ(&phy->access, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data1); 
     
        PHYMOD_DEBUG_VERBOSE(("FW Clear:%d\n",data1));
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (retry_cnt --));
    if (retry_cnt == 0) {
        PHYMOD_DEBUG_VERBOSE(("WARN:: FW Enable not cleared\n"));
    }

    return PHYMOD_E_NONE;
}

/**   Get interface config
 *    This function gets interface configuration (interface, speed, frequency of
 *    operation, PT mode, BCM84793 etc). 
 *
 *    NOTE: Default system side interface type will be IEEE mode and speed is compatible with lane side configured speed.
 *     Ex: User configured 40G speed with interface type KR4 on lane side then system side speed is 40G and interface type is IEEE mode.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Reserved for future use
 *    @param config             Gets Interface config such as,
 *                                - Interface Type
 *                                - Speed 
 *                                - Frequency.
 *                                - device_aux_modes : a structure that have auxilary mode details
 *                                   -> pass_thru : To enable repeater on 10/40G mode of operation
 *                                   -> gearbox_100g_inverse_mode : Applicable only for 100G mode, 25G lanes on system side and
 *                                      10G lanes on line side
 *                                   -> BCM84793_capablity : To enable BCM84793 compatible on lanes
 *                                   -> passthru_sys_side_core : When pass through is enabled user are allowed to configure system side
 *                                      core i.e. 25G lanes or 10G lanes
 *                                   -> reserved: Only for future use
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t ref_clock, phymod_phy_inf_config_t* config)
{
        
    PHYMOD_IF_ERR_RETURN (
            _sesto_phy_interface_config_get(&phy->access, flags, config));
        
    return PHYMOD_E_NONE;
    
}

/**  PHY CL73 ability Set
 *   This function is used to set the CL73 ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an_ability         Pointer to the ability of cl73
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    sesto_an_ability_t value;
    PHYMOD_MEMSET(&value, 0, sizeof(value));
        
    value.cl73_adv.an_fec = an_ability->an_fec;
    
    /*check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)){
        return PHYMOD_E_PARAM;
    }

    /*next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = SESTO_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = SESTO_ASYM_PAUSE;
    } else {
        value.cl73_adv.an_pause = SESTO_NO_PAUSE;
    }
        
    /*check cl73 and cl73 bam ability */
    if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_40GBASE_KR4;
    } else if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_40GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_CR4_GET(an_ability->an_cap)) { 
        value.cl73_adv.an_base_speed = SESTO_CL73_100GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_KR4_GET(an_ability->an_cap)) { 
        value.cl73_adv.an_base_speed = SESTO_CL73_100GBASE_KR4;
    } else {
        return PHYMOD_E_PARAM;
    } 

    PHYMOD_IF_ERR_RETURN
        (_sesto_autoneg_ability_set(&phy->access, &value));
    
    return PHYMOD_E_NONE;
}

/**  PHY CL73 ability get
 *   This function is used to get the CL73 ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an_ability         Pointer to the ability of cl73
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    sesto_an_ability_t value;

    PHYMOD_IF_ERR_RETURN(
    _sesto_autoneg_ability_get (&phy->access, &value));

    an_ability_get_type->an_fec = value.cl73_adv.an_fec;
    switch (value.cl73_adv.an_pause) {
        case SESTO_ASYM_PAUSE:
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
        case SESTO_SYMM_PAUSE:
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
        default:
            break;
    }

    /*first check cl73 ability*/
    switch (value.cl73_adv.an_base_speed) {
        case SESTO_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case SESTO_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
        break;
        case SESTO_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case SESTO_CL73_100GBASE_KR4:
            PHYMOD_AN_CAP_100G_KR4_SET(an_ability_get_type->an_cap);
        break;
        default:
            break;
    }
    return PHYMOD_E_NONE;
}

/**  PHY enable/Disable CL73
 *   This function is used to enable/disable CL73 of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an                 Pointer to the AN control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return (_sesto_autoneg_set(&phy->access, an));
}

/**  Get CL73 completion state
 *   This function is used get CL73 completion state of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an                 Pointer to the AN control
 *    @param an_done            Pointer to the AN completion state
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return (_sesto_autoneg_get(&phy->access, an, an_done));
}


int sesto_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**   Sesto core initialization
 *    This function initialize the sesto core by downlaoding the firmware.
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param init_config        Init configuration specified by user 
 *    @param core_status        PMD status read from PHY chip 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    /*uint32_t pmd_status = 0;*/
    PHYMOD_IF_ERR_RETURN(
            _sesto_core_init(core, init_config));
    
    /* Retrive PMD lock status on line side for all the lanes */
    /*PHYMOD_IF_ERR_RETURN(
            _sesto_rx_pmd_lock_get(&core->access, &pmd_status));
    PHYMOD_DEBUG_VERBOSE(("PMD Status: %x\n", pmd_status));*/

    return PHYMOD_E_NONE;

}


int sesto_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

/**  Set loopback
 *   This function is used to set a particular loopback mode
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param loopback           Type of loopback
 *        0 - Global loopback
 *        1 - Global PMD loopback
 *        2 - Remote PMD loopback
 *        3 - Remote PCS loopback
 *        4 - Digital PMD loopback
 *    @enable                    Enable or disable
 *        0 - disable
 *        1 - enable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    return _sesto_loopback_set(&phy->access, loopback, enable);
}

/**  Get loopback
 *   This function is used to get the status of a particular loopback mode
 *   whether or not enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param loopback           Type of loopback
 *        0 - Global loopback
 *        1 - Global PMD loopback
 *        2 - Remote PMD loopback
 *        3 - Remote PCS loopback
 *        4 - Digital PMD loopback
 *    @enable                    Enable or disable
 *        0 - disable
 *        1 - enable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return _sesto_loopback_get(&phy->access, loopback, enable);    
}
/**   Get PHY RX PMD link status 
 *    This function retrieves RX PMD lock status of sesto 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param rx_pmd_locked      Rx PMD Link status retrieved from Sesto
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{

    return (_sesto_rx_pmd_lock_get(&phy->access, rx_pmd_locked));
}

/**   Get PCS link status
 *    This function retrieves PCS link status of sesto when PCS Monitor is enabled,
 *    This function return PMD lock when PCS monitor is disabled.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param link_status        Retrives PCS Link of Sesto
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
        
    return _sesto_phy_pcs_link_get(&phy->access, link_status);
        
}

/**   Read Register
 *    This function read user specified register based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           Sesto register address
 *    @param val                Output parameter, represents the value of address specified
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    uint16_t dev_add = 0;
    dev_add = (phy->access.devad) ? phy->access.devad : SESTO_DEV_PMA_PMD;

    if (dev_add == SESTO_DEV_PMA_PMD) {
        READ_SESTO_PMA_PMD_REG(&phy->access, reg_addr, *val);
    } else {
        READ_SESTO_AN_REG(&phy->access, reg_addr, *val);
    }
    return PHYMOD_E_NONE;
    
}

/**   Write Register
 *    This function write user specified value to the specified address 
 *    based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           Sesto register address
 *    @param val                Represents the value to be written
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    uint16_t dev_add = 0;
    dev_add = (phy->access.devad) ? phy->access.devad : SESTO_DEV_PMA_PMD;
    PHYMOD_DEBUG_VERBOSE(("WRITE reg:%x data:%x\n", reg_addr, val));
    if (dev_add == SESTO_DEV_PMA_PMD) {
        WRITE_SESTO_PMA_PMD_REG(&phy->access, reg_addr, val);
    } else {
        WRITE_SESTO_AN_REG(&phy->access, reg_addr, val);
    }
        
    return PHYMOD_E_NONE;
    
}

/**   Retrives Revision 
 *    This function retrives sesto revision ID 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param rev_id             Output parameter, Represents the revision ID of sesto.
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_phy_rev_id(const phymod_phy_access_t* phy, uint32_t *rev_id)
{
    uint32_t chip_id = 0;
    PHYMOD_IF_ERR_RETURN (
        sesto_get_chipid(&phy->access, &chip_id, rev_id));
 
    PHYMOD_DEBUG_VERBOSE(("CHIPID : 0x%x\n", chip_id));

    return PHYMOD_E_NONE;
}

/**  Set CL72
 *   This function is used to enable/disable forced Tx training
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable / disable force Tx training
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _sesto_force_tx_training_set(&phy->access, enable);
}


/**  Get CL72
 *   This function is used to get the status whether Tx training is enabled or disabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t *enable)
{
    return _sesto_force_tx_training_get(&phy->access, enable);
}

/**  Get CL72 status
 *   This function is used to get the status of Tx training
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param status             CL72 Training status
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    return _sesto_force_tx_training_status_get(&phy->access, status);
}

/**  Interrupt status get
 *   This function is used to check whether or not particualr interrupt is pending
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param intr_type          Interrupt type
 *             4 = INT_M0_SLV_MISC_INTR
 *             5 = INT_M0_MST_MISC_INTR
 *             6 = INT_M0_SLV_MSGOUT_INTR
 *             7 = INT_M0_MST_MSGOUT_INTR
 *            16 = INT_MUX_PMD_LOCK_LOST,
 *            17 = INT_MUX_PMD_LOCK_FOUND,
 *            20 = INT_PLL_10G_LOCK_LOST
 *            21 = INT_PLL_10G_LOCK_FOUND
 *            24 = INT_DEMUX_PMD_LOCK_LOST,
 *            25 = INT_DEMUX_PMD_LOCK_FOUND,
 *            28 = INT_PLL_25G_LOCK_LOST
 *            29 = INT_PLL_25G_LOCK_FOUND
 *    @param intr_status             Pending status
 *        1 - Pending
 *        0 - Serviced
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_intr_status_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* intr_status)
{
    return _sesto_ext_intr_status_get(&phy->access, intr_type, intr_status);
}
/**  Interrupt enable set
 *   This function is used to enable or disable particular interrupt
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param intr_type          Interrupt type
 *             4 = INT_M0_SLV_MISC_INTR
 *             5 = INT_M0_MST_MISC_INTR
 *             6 = INT_M0_SLV_MSGOUT_INTR
 *             7 = INT_M0_MST_MSGOUT_INTR
 *            16 = INT_MUX_PMD_LOCK_LOST,
 *            17 = INT_MUX_PMD_LOCK_FOUND,
 *            20 = INT_PLL_10G_LOCK_LOST
 *            21 = INT_PLL_10G_LOCK_FOUND
 *            24 = INT_DEMUX_PMD_LOCK_LOST,
 *            25 = INT_DEMUX_PMD_LOCK_FOUND,
 *            28 = INT_PLL_25G_LOCK_LOST
 *            29 = INT_PLL_25G_LOCK_FOUND
 *    @param enable             Enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_intr_enable_set(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t enable)
{
    return _sesto_ext_intr_enable_set(&phy->access, intr_type, enable);
}
/**  Interrupt enable get
 *   This function is used to check whether or not particular interrupt is enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param intr_type          Interrupt type
 *             4 = INT_M0_SLV_MISC_INTR
 *             5 = INT_M0_MST_MISC_INTR
 *             6 = INT_M0_SLV_MSGOUT_INTR
 *             7 = INT_M0_MST_MSGOUT_INTR
 *            16 = INT_MUX_PMD_LOCK_LOST,
 *            17 = INT_MUX_PMD_LOCK_FOUND,
 *            20 = INT_PLL_10G_LOCK_LOST
 *            21 = INT_PLL_10G_LOCK_FOUND
 *            24 = INT_DEMUX_PMD_LOCK_LOST,
 *            25 = INT_DEMUX_PMD_LOCK_FOUND,
 *            28 = INT_PLL_25G_LOCK_LOST
 *            29 = INT_PLL_25G_LOCK_FOUND
 *    @param enable             Enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_intr_enable_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* enable)
{
    return _sesto_ext_intr_enable_get(&phy->access, intr_type, enable);
}
/**  Interrupt status clear
 *   This function is used for clearing interrupt status
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param intr_type          Interrupt type
 *             4 = INT_M0_SLV_MISC_INTR
 *             5 = INT_M0_MST_MISC_INTR
 *             6 = INT_M0_SLV_MSGOUT_INTR
 *             7 = INT_M0_MST_MSGOUT_INTR
 *            16 = INT_MUX_PMD_LOCK_LOST,
 *            17 = INT_MUX_PMD_LOCK_FOUND,
 *            20 = INT_PLL_10G_LOCK_LOST
 *            21 = INT_PLL_10G_LOCK_FOUND
 *            24 = INT_DEMUX_PMD_LOCK_LOST,
 *            25 = INT_DEMUX_PMD_LOCK_FOUND,
 *            28 = INT_PLL_25G_LOCK_LOST
 *            29 = INT_PLL_25G_LOCK_FOUND
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_type)
{
    return _sesto_ext_intr_status_clear(&phy->access, intr_type);
}

/**  PHY status dump
 *   This function is used to display status dump of a core and for given lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_status_dump(const phymod_phy_access_t* phy)
{
    return _sesto_phy_status_dump(&phy->access);
}

/**  FEC enable set
 *   This function is used to enable the FEC
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _sesto_fec_enable_set(&phy->access, enable);
}
/**  FEC enable get
 *   This function is used to check whether or not FEC is enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return _sesto_fec_enable_get(&phy->access, enable);
}

/**  FC/PCS checker enable set
 *   This function is used to enable or disable FC/PCS checker
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param fcpcs_chkr_mode    FC/PCS checker mode
 *    @param enable             Enable or disable
 *        1 -  enable
 *        0 -  disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_fc_pcs_chkr_enable_set(const phymod_phy_access_t* phy, uint32_t fcpcs_chkr_mode, uint32_t enable)
{
    return _sesto_pcs_link_monitor_enable_set(&phy->access, enable);
}
/**  FC/PCS checker enable get
 *   This function is used to check whether or not FC/PCS checker enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param fcpcs_chkr_mode    FC/PCS checker mode
 *    @param enable             Enable or disable
 *        1 -  enable
 *        0 -  disable
 */
int sesto_phy_fc_pcs_chkr_enable_get(const phymod_phy_access_t* phy, uint32_t fcpcs_chkr_mode, uint32_t *enable)
{
    return _sesto_pcs_link_monitor_enable_get(&phy->access, enable);
}
/**  FC/PCS checker status get
 *   This function is used to lock, loss of lock, error count status of the FC/PCS checker
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param lock_status        Live lock status
 *        1 - Locked
 *        0 - Not locked
 *    @param lock_lost_lh       Loss of lock
 *        1 -  Loss of lock happened
 *        0 -  No Loss of lock happended
 *    @param error_count        Error count
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_fc_pcs_chkr_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    return _sesto_get_pcs_link_status(&phy->access, lock_status);
}

/** Repeater mode set
 *   This function is used to enable or diable the repeater or retimer mode
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        0 - retimer mode
 *        1 - repeater mode
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_repeater_mode_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _sesto_phy_repeater_mode_set(&phy->access, enable);
}

/** Repeater mode get
 *   This function is used get the enable or diable the repeater or retimer mode
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        0 - retimer mode
 *        1 - repeater mode
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_repeater_mode_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return _sesto_phy_repeater_mode_get(&phy->access, enable);
}
#if 0

/*! \brief Config set for GPIO pins
 * 
 *  This funtion is used to set the GPIO pins pull up or pull down
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param gpio_ctrl          Pin and Configuration direction
 *                                 gpio_pin_number;
 *                                 cfg_direction;
 *                                 cfg_pull;
 *                                 pin_value;
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_cfg_gpio_pin_set(const phymod_phy_access_t* phy, phymod_gpio_ctrl_args_t *gpio_ctrl)
{
    return _sesto_phy_cfg_gpio_pin_set(&phy->access, gpio_ctrl);
}

/*! \brief Config get for GPIO pins
 *
 *  This funtion is used to get the GPIO pins pull up or pull down
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param gpio_ctrl          Pin and Configuration direction
 *                                 gpio_pin_number;
 *                                 cfg_direction;
 *                                 cfg_pull;
 *                                 pin_value;
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_cfg_gpio_pin_get(const phymod_phy_access_t* phy, phymod_gpio_ctrl_args_t *gpio_ctrl)
{
    return _sesto_phy_cfg_gpio_pin_get(&phy->access, gpio_ctrl);
}
/*! \brief Config set for QSFP28/QSFP+ modules
 *
 *  This funtion is used to set the configuration of the module controller of line card
 *  This function will configure IO in In/Out & Its Pu/PD mode based on Module need
 *  And it configures value of the pin for Outputs
 *
 *  @param mdcrtl_pins     Module control pins parameters
 *                         To configure lpmod value:
 *                              lpmod->enable = 1; lpmod->value = <1/0>;
 *                         To configure resetl value:
 *                               resetl->enable = 1; resetl->value = <1/0>;
 *                         To configure intl value:
 *                               intl->enable = 1; intl->value = <1/0>;
 *                         To configure mod_sell value:
 *                               mod_sell->enable = 1; mod_sell->value = <1/0>;
 *                         To configure mod_prsl value:
 *                                mod_prsl->enable = 1; mod_prsl->value = <1/0>;
 *                         We can configure one or more parameters by calling
 *                         enable = 1 with particular parameter value<0/1>.
 *                         Pin value is ignored for the input pins
 *
 *                         If a Module pin is not supported in a Package or Chip
 *                         and user try to enable func throws error message
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_modctrl_cfg_qsfp_linecard_set(const phymod_phy_access_t* phy, phymod_modctrl_qsfp_io_pins_t *qsfp_io_pins)
{
    return _sesto_phy_modctrl_cfg_qsfp_linecard_set(&phy->access, qsfp_io_pins);
}
/*! \brief Config get for QSFP28/QSFP+ modules
 *
 *  This funtion is used to get the configuration of the module controller of line card
 *  This function gets configured IO in In/Out & Its Pu/PD mode based on Module config
 *
 *  @param mdcrtl_pins     Module control pins parameters
 *                         To configure lpmod value:
 *                              lpmod->enable = 1; lpmod->value = <1/0>;
 *                         To configure resetl value:
 *                               resetl->enable = 1; resetl->value = <1/0>;
 *                         To configure intl value:
 *                               intl->enable = 1; intl->value = <1/0>;
 *                         To configure mod_sell value:
 *                               mod_sell->enable = 1; mod_sell->value = <1/0>;
 *                         To configure mod_prsl value:
 *                                mod_prsl->enable = 1; mod_prsl->value = <1/0>;
 *                         We can configure one or more parameters by calling
 *                         enable = 1 with particular parameter value<0/1>.
 *                         Pin value is ignored for the input pins
 *
 *                         If a Module pin is not supported in a Package or Chip
 *                         and user try to enable func throws error message
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_modctrl_cfg_qsfp_linecard_get(const phymod_phy_access_t* phy, phymod_modctrl_qsfp_io_pins_t *qsfp_io_pins)
{
    return _sesto_phy_modctrl_cfg_qsfp_linecard_get(&phy->access, qsfp_io_pins);
}

/*! \brief Config set for CFP2/CFP4 modules
 *
 *  This funtion is used to set the configuration of the module controller IO of line card
 *  This function will configure IO in In/Out & Its Pu/PD mode based on Module need
 *  And it configures value of the pin for Outputs
 *
 *  @param phy             Pointer to phymod phy access structure
 *  @param cfp_io_pins     Module control pins parameters
 *                         To configure tx_dis value:
 *                            tx_dis->enable = 1; tx_dis->value = <1/0>;
 *                         To configure rx_los value:
 *                            rx_los->enable = 1;
 *                         To configure mod_lopwr value:
 *                            mod_lopwr->enable = 1; mod_lopwr->value = <1/0>;
 *                         To configure mod_abs value:
 *                            mod_abs->enable = 1;
 *                         To configure glb_alrmn value:
 *                            glb_alrmn->enable = 1;
 *                         To configure mod_rstn value:
 *                            mod_rstn->enable = 1; mod_rstn->value = <1/0>
 *                         We can configure one or more parameters by calling
 *                         enable = 1 with particular parameter value <0/1>.
 *                         Pin value is ignored for the input pins
 *
 *                         If a Module pin is not supported in a Package or Chip
 *                         and user try to enable func throws error message
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_modctrl_cfg_cfp_linecard_set(const phymod_phy_access_t* phy, phymod_modctrl_cfp_io_pins_t *cfp_io_pins)
{
    return _sesto_phy_modctrl_cfg_cfp_linecard_set(&phy->access, cfp_io_pins);
}

/*! \brief Config get for CFP2/CFP4 modules
 *
 *  This funtion is used to get the configuration of the module controller IO of line card
 *  This function gets configured IO in In/Out & Its Pu/PD mode based on Module config
 *
 *  @param phy             Pointer to phymod phy access structure
 *  @param cfp_io_pins     Module control pins parameters
 *                         To configure tx_dis value:
 *                            tx_dis->enable = 1; tx_dis->value = <1/0>;
 *                         To configure rx_los value:
 *                            rx_los->enable = 1;
 *                         To configure mod_lopwr value:
 *                            mod_lopwr->enable = 1; mod_lopwr->value = <1/0>;
 *                         To configure mod_abs value:
 *                            mod_abs->enable = 1;
 *                         To configure glb_alrmn value:
 *                            glb_alrmn->enable = 1;
 *                         To configure mod_rstn value:
 *                            mod_rstn->enable = 1; mod_rstn->value = <1/0>
 *                         We can configure one or more parameters by calling
 *                         enable = 1 with particular parameter value <0/1>.
 *                         Pin value is ignored for the input pins
 *
 *                         If a Module pin is not supported in a Package or Chip
 *                         and user try to enable func throws error message
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_modctrl_cfg_cfp_linecard_get(const phymod_phy_access_t* phy, phymod_modctrl_cfp_io_pins_t *cfp_io_pins)
{
    return _sesto_phy_modctrl_cfg_cfp_linecard_get(&phy->access, cfp_io_pins);
}

/*! \brief Module Write
 *   This Funtion is used write the data with I2C command on selected module
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param slv_dev_addr       Module slave address (for supported Module types)
 *    @param start_addr         Start address of i2c Slave to be accessed
 *    @param no_of_bytes        No of bytes to be read.
 *    @param write_data         Contains array of bytes to be written to the module
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */


int sesto_phy_module_write(const phymod_phy_access_t* phy, uint32_t slv_dev_addr, uint32_t start_addr, uint32_t no_of_bytes, uint8_t
*write_data)
{
    return _sesto_phy_module_write(&phy->access, slv_dev_addr, start_addr, no_of_bytes, write_data);
}

/*! \brief Module Read
 *   This Funtion is used read the data with I2C command on selected module
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param slv_dev_addr       Module slave address (for supported Module types)
 *    @param start_addr         Start address of i2c Slave to be accessed
 *    @param no_of_bytes        No of bytes to be read.
 *    @param read_data          Contains array of bytes to be read to the module
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_module_read(const phymod_phy_access_t* phy, uint32_t slv_dev_addr, uint32_t start_addr, uint32_t no_of_bytes, uint8_t
*read_data)
{
    return _sesto_phy_module_read(&phy->access, slv_dev_addr, start_addr, no_of_bytes, read_data);
}
#endif

#endif /* PHYMOD_SESTO_SUPPORT */
