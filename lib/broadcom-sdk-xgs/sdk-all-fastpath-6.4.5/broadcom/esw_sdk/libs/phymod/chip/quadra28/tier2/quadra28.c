/*
 *
 * $Id: quadra28.c, 2014/12/16 aman $
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
 * File: quadra28.c
 * Purpose: tier2 phymod support for Broadcom 40G Quadra28 phy
 * note:
 * Specifications:

 * Repeater, retimer operation.

 * Supports the following data speeds:
 * 1.25 Gbps line rate (1 GbE data rate, 8x oversampled over 10 Gbps line rate)
 * 10.3125 Gbps line rate (10 GbE data rate)
 * 11.5 Gpbs line rate (for backplane application, proprietary data rate)
 * 410.3125 Gbps line rate (40 GbE data rate)
 *
 * Supports the following line-side connections:
 * 1 GbE and 10 GbE SFP+ SR and LR optical modules
 * 40 GbE QSFP SR4 and LR4 optical modules
 * 1 GbE and 10 GbE SFP+ CR (CX1) copper cable
 * 40 GbE QSFP CR4 copper cable
 * 10 GbE KR, 11.5 Gbps line rate and 40 GbE KR4 backplanes
 *
 * Operates with the following reference clocks:
 * Single 156.25 MHz differential clock for 1.25 Gbps, 10.3125 Gpbs and 11.5 Gbps
 * line rates
 *
 * Supports autonegotiation as follows:
 * Clause 73 only for starting Clause 72 and requesting FEC
 * No speed resolution performed
 * No Clause 73 in 11.5 Gbps line rate, only Clause 72 supported
 * Clause 72 may be enabled standalone for close systems
 * Clause 37 is supported
     
  */

#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>

#ifdef PHYMOD_QUADRA28_SUPPORT

#include <phymod/chip/quadra28.h>

/* To be defined */
#include "../tier1/quadra28_types.h"
#include "../tier1/quadra28_cfg_seq.h"
#include "../tier1/quadra28_reg_access.h"
#include "../tier1/bcmi_quadra28_defs.h"

/* Microcontroller Firmware */
extern unsigned char quadra28_ucode_bin[];
extern uint32_t quadra28_ucode_len;
int _quadra28_core_firmware_load (const phymod_core_access_t* core,
                              phymod_firmware_load_method_t load_method,
                              phymod_firmware_loader_f fw_loader);


/*
 *    Set Interface config
 *    This function sets interface configuration (interface, speed, frequency of
 *    operation). This is requried to put the quadra28 into specific mode specified
 *    by the user.
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param flags              Currently unused and reserved for future use
 *    @param config             Interface config structure where user specifies
 *                              interface, speed and the frequency of operation.
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_interface_config_set(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   const phymod_phy_inf_config_t* config)
{
    return quadra28_set_config_mode(&phy->access,
                       config->interface_type,
                       config->data_rate,
                       config->ref_clock);
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
int quadra28_phy_interface_config_get(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   phymod_ref_clk_t ref_clock,
                                   phymod_phy_inf_config_t* config)
{
    return quadra28_get_config_mode(&phy->access,
                       &config->interface_type,
                       &config->data_rate,
                       &config->ref_clock,
                       &config->interface_modes);
}


/**   Core Init
 *    This function initializes the quadra core and
 *    downloads the firmware. It sets the PHY in
 *    default mode.
 *
 *    @param phy                Pointer to phymod core access structure
 *    @param init_config        Init configuration specified by user
 *    @param core_status        Core status read from PHY chip
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_core_init(const phymod_core_access_t *core,
                       const phymod_core_init_config_t *init_config,
                       const phymod_core_status_t *core_status) 
{
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));

    PHYMOD_IF_ERR_RETURN
        (_quadra28_core_firmware_load(core,
                                   init_config->firmware_load_method,
                                   init_config->firmware_loader));

    /* UPdate the Datapath in mode register. Which will be retrived and
     * programmed suring mode config API*/
    PHYMOD_IF_ERR_RETURN(READ_APPS_MODE_0r(&core->access,&apps_mode0_reg_val));
    if (init_config->op_datapath == phymodDatapathNormal) {
        BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val, 0);
    } else if (init_config->op_datapath == phymodDatapathUll) {
        BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val, 1);
    } else {
        BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val, 0);
    }
    PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&core->access,apps_mode0_reg_val));
    
    return PHYMOD_E_NONE;

}

/**   Core firmware download
 *    This function downloads the firmware using either internal or external
 *    method as specified by the user. Firmware loader function supplied by user
 *    will be used for external download method.
 *
 *    @param core               Pointer to phymod core access structure
 *    @param load_method        Load method will be specified by user
 *    @param fw_loader          Loader function specified by user and will be
 *                              used for external download method
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */ 
int _quadra28_core_firmware_load (const phymod_core_access_t* core,
                              phymod_firmware_load_method_t load_method,
                              phymod_firmware_loader_f fw_loader)
{
   int ret_val = 0;
    const phymod_access_t *pm_acc = &core->access;
    switch(load_method)
    {
        case phymodFirmwareLoadMethodInternal:
            /* Download the Microcode (Firmware) through MDIO. The actual download is done in tier1 */ 
            PHYMOD_DIAG_OUT((" Starting Firmware download through MDIO,  it takes few seconds...\n"));
            ret_val = quadra28_micro_download(pm_acc,
                                                 quadra28_ucode_bin,
                                                 quadra28_ucode_len,
                                                 0);
            if (ret_val != PHYMOD_E_NONE) {
                PHYMOD_RETURN_WITH_ERR
                    (PHYMOD_E_CONFIG,
                    (_PHYMOD_MSG("firmware download through MDIO failed")));
            } else {
                PHYMOD_DIAG_OUT(("Firmware download through MDIO success \n"));
            }

        break;
        case phymodFirmwareLoadMethodExternal:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodFirmwareLoadMethodNone:
        break;
        case phymodFirmwareLoadMethodProgEEPROM:
            PHYMOD_DIAG_OUT(("Firmware will be downloaded first, and flashed on to EEPROM \n"));
            PHYMOD_DIAG_OUT(("This process will take few minutes.....\n"));
            ret_val = quadra28_micro_download (pm_acc,
                                                quadra28_ucode_bin,
                                                quadra28_ucode_len,
                                                1);
            if (ret_val == PHYMOD_E_FAIL) {
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_FAIL,
                 (_PHYMOD_MSG("Flashing Firmware to EEPROM failed")));
            } else if (ret_val != quadra28_ucode_len) {
                PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_FAIL,
                 (_PHYMOD_MSG("Downloading Firmware failed")));
            } else {
                PHYMOD_DIAG_OUT(("Firmware is flashed to EEPROM successfully\n"));
            }
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_CONFIG,
                 (_PHYMOD_MSG("illegal fw load method")));
    }
    return PHYMOD_E_NONE;
}

 
/**   Get PHY link status
 *    This function retrieves PHY link status from device
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param link_status        Link status retrieved from the device
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */


int quadra28_phy_link_status_get(const phymod_phy_access_t* phy,
                              uint32_t* link_status)
{
    return quadra28_link_status(&phy->access, link_status);
}
                   
/**   PHY register read
 *    This function reads the PHY register
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reg_addr           PHY Register Address to Read
 *    @param val                PHY Register content of the specified
 *                              register Address to Read
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr,
                       uint32_t *val)
{
    return phymod_raw_iblk_read(&phy->access,\
                        QUADRA28_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
                        val);
}


/**   PHY register write
 *    This function is used to write content to PHY register
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reg_addr           PHY Register Address to Write
 *    @param val                Content/Value to Write to the PHY Register
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr,
                        uint32_t val)
{

    return phymod_raw_iblk_write(&phy->access,\
                         QUADRA28_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
                         val);
}

/**   PHY core info get
 *    This function is used to write content to PHY register
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param info               Pointer to core info structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{
    uint32_t chip_id;
    PMD_IDENTIFIER_0r_t id0;
    PMD_IDENTIFIER_1r_t id1;

    chip_id =  _quadra28_get_chip_id(&phy->access);
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionQuadra28;
    
    PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_1r(&phy->access, &id1));
    info->phy_id1 = id1.v[0];
    PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_0r(&phy->access, &id0));
    info->phy_id0 = id0.v[0];

    return PHYMOD_E_NONE;
}
/**  PHY polarity set
 *   This function is used to set the lane polarity
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param polarity           Pointer to phymod_polarity_t for rx and tx
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return quadra28_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity);
}


/**  PHY polarity get
 *   This function is used to get the lane polarity
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param polarity           Pointer to phymod_polarity_t for rx and tx
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return quadra28_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity);
}

/**  PHY Rx PMD lock
 *   PHY Rx PMD lock This function is used to get the rx PMD lock status
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx_seq_done        Pointer to rx sequence
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done){

    return quadra28_pmd_lock_get(&phy->access, rx_seq_done);
}


/**  PHY power set
 *   PHY power set This function is used to set the power
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param power              Pointer to phymod phy power structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_power_set(const phymod_phy_access_t* phy,
                        const phymod_phy_power_t* power)
{
    return _quadra28_phy_power_set(&phy->access, power);
}

/*
 *    @param flags               Reserved for Furtuer use
 *    @param operation           Operation to perform on PLL sequencer
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{

    return  _quadra28_pll_seq_restart(&core->access, flags, operation);

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
int quadra28_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _quadra28_phy_fec_enable_set(&phy->access, enable);
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
int quadra28_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return _quadra28_phy_fec_enable_get(&phy->access, enable);
}
/**  PHY status dump
 *   This function is used to display status dump of a core and for given lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_status_dump(const phymod_phy_access_t* phy)
{

    PHYMOD_IF_ERR_RETURN(
            _quadra28_phy_status_dump(&phy->access));

    return PHYMOD_E_NONE;

}

int quadra28_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    uint32_t chip_id;
    PMD_IDENTIFIER_0r_t id0;
    PMD_IDENTIFIER_1r_t id1;
    const phymod_access_t *pm_acc = &core->access;

    PHYMOD_MEMSET(&id0, 0, sizeof(PMD_IDENTIFIER_0r_t));
    PHYMOD_MEMSET(&id1, 0, sizeof(PMD_IDENTIFIER_1r_t));
    *is_identified = 0;
    if (core_id == 0) {
        PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_1r(pm_acc, &id1));
        PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_0r(pm_acc, &id0));
    } else {
        id0.v[0] = (uint16_t) ((core_id >> 16) & 0xffff);
        id1.v[0] = (uint16_t) core_id & 0xffff;
    }
    if ((id0.v[0] == QUADRA28_PMD_ID0 &&
        id1.v[0] == QUADRA28_PMD_ID1)) {
        chip_id =  _quadra28_get_chip_id(&core->access);

        if (chip_id == QUADRA28_82780_CHIP_ID) {
            /* PHY IDs match */
            *is_identified = 1;
        }
    }
    return PHYMOD_E_NONE;

}
int quadra28_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    PHYMOD_IF_ERR_RETURN (
         quadra28_soft_reset(&core->access, reset_mode, direction));
    return PHYMOD_E_NONE;

}

int quadra28_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_UNAVAIL;

}

int quadra28_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    PHYMOD_IF_ERR_RETURN( 
        quadra28_firmware_info_get(&core->access, fw_info));
    return PHYMOD_E_NONE;

}

int quadra28_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    return _quadra28_phy_tx_set(&phy->access, tx);
}

int quadra28_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    PHYMOD_IF_ERR_RETURN( 
        _quadra28_phy_tx_get(&phy->access, tx));
    return PHYMOD_E_NONE;
}

int quadra28_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
    TXFIR_CONTROL1r_t txfir_ctrl1;
    TXFIR_CONTROL2r_t txfir_ctrl2;

    PHYMOD_MEMSET(tx, 0, sizeof(phymod_tx_t));
    PHYMOD_MEMSET(&txfir_ctrl1, 0, sizeof(TXFIR_CONTROL1r_t));
    PHYMOD_MEMSET(&txfir_ctrl2, 0, sizeof(TXFIR_CONTROL2r_t));
    PHYMOD_IF_ERR_RETURN(
        READ_TXFIR_CONTROL1r(&phy->access, &txfir_ctrl1));
    PHYMOD_IF_ERR_RETURN(
        READ_TXFIR_CONTROL2r(&phy->access, &txfir_ctrl2));

    tx->pre = TXFIR_CONTROL1r_TXFIR_PRE_OVERRIDEf_GET(txfir_ctrl1);
    tx->post = TXFIR_CONTROL1r_TXFIR_POST_OVERRIDEf_GET(txfir_ctrl1);
    tx->main = TXFIR_CONTROL2r_TXFIR_MAIN_OVERRIDEf_GET(txfir_ctrl2);
    tx->post2 = TXFIR_CONTROL2r_TXFIR_POST2f_GET(txfir_ctrl2);

    return PHYMOD_E_NONE;
}
int quadra28_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    return _quadra28_phy_rx_set(&phy->access, rx);
}

int quadra28_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    return _quadra28_phy_rx_get(&phy->access, rx);
}

int quadra28_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    return _quadra28_phy_reset_set(&phy->access, reset);
}

int quadra28_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    return _quadra28_phy_reset_get(&phy->access, reset);
}

int quadra28_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    return quadra28_tx_lane_control_set(&phy->access, tx_control);
} 

int quadra28_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    int enable = 0;
    *tx_control = phymodTxSquelchOn;

    PHYMOD_IF_ERR_RETURN (
       quadra28_tx_squelch_get(&phy->access, &enable));
    
    if (!enable) {
        *tx_control = phymodTxSquelchOff;
    }
    return PHYMOD_E_NONE;

}

int quadra28_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    return quadra28_rx_lane_control_set(&phy->access, rx_control);
}

int quadra28_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int enable = 0;
    *rx_control = phymodRxSquelchOn;
    PHYMOD_IF_ERR_RETURN (
       quadra28_rx_squelch_get(&phy->access, &enable));
    if (!enable) {
        *rx_control = phymodRxSquelchOff;
    }
    return PHYMOD_E_NONE;

}

int quadra28_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{
    q28_an_ability_t an_ability;
    an_ability.fec_ability = an_ability_set_type->an_fec;

    /*next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability_set_type)) {
        an_ability.pause_ability = Q28_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability_set_type)) {
        an_ability.pause_ability = Q28_ASYM_PAUSE;
    } else {
        an_ability.pause_ability = Q28_NO_PAUSE;
    }

    /* Quadra28 FW set advertisement based on the speed, so no
     * need to set advert*/
    PHYMOD_IF_ERR_RETURN(_quadra28_phy_autoneg_ability_set(&phy->access, an_ability));

    return PHYMOD_E_NONE;
}

int quadra28_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    return _quadra28_phy_autoneg_ability_get(&phy->access, an_ability_get_type);
}

int quadra28_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    if (init_config->op_mode == phymodOperationModeRepeater) {
        /*When retimer is not enabled, it enables repeater*/
        PHYMOD_IF_ERR_RETURN(
            _quadra28_phy_retimer_enable(&phy->access, 0));
    } else {
        /*When repeater is not enabled, it enables retimer*/
        PHYMOD_IF_ERR_RETURN(
            _quadra28_phy_retimer_enable(&phy->access, 1));
    }
    return PHYMOD_E_NONE;
}

int quadra28_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    return _quadra28_loopback_set(&phy->access, loopback, enable);
}

int quadra28_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return _quadra28_loopback_get(&phy->access, loopback, enable);    
}

int quadra28_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return _quadra28_phy_power_get(&phy->access, power);
}

int quadra28_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return _quadra28_phy_autoneg_set(&phy->access, an);
}

int quadra28_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return _quadra28_phy_autoneg_get(&phy->access, an, an_done);

}

int quadra28_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    return _quadra28_phy_autoneg_remote_ability_get(&phy->access, an_ability_get_type) ;
}

int quadra28_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    return _quadra28_phy_cl72_set(&phy->access, cl72_en);
}

int quadra28_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    return _quadra28_phy_cl72_get(&phy->access, cl72_en);
}

int quadra28_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    return _quadra28_phy_cl72_status_get(&phy->access, status);
}

#endif 
