/*
 *         
 * $Id: furia.c, 2014/04/03 palanivk Exp $
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

#ifdef PHYMOD_FURIA_SUPPORT

#include <phymod/chip/furia.h>

#include "../tier1/furia_regs_structs.h"
#include "../tier1/furia_reg_access.h"
#include "../tier1/furia_address_defines.h"
#include "../tier1/furia_micro_seq.h"
#include "../tier1/furia_types.h"
#include "../tier1/furia_cfg_seq.h"
#include "../tier1/furia_pkg_cfg.h"

#define FURIA_ID0       0xae02   
#define FURIA_ID1       0x5230  

#define FURIA_NOF_LANES_IN_CORE (4)
#define FURIA_PHY_ALL_LANES (0xf)
#define FURIA_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access,\
                       sizeof((_phy_access)->access));\
        (_phy_access)->type = (_core_access)->type; \
        (_phy_access)->access.lane = FURIA_PHY_ALL_LANES; \
    }while(0)

/* uController's firmware */
extern unsigned char furia_ucode_Furia[];
extern unsigned short furia_ucode_Furia_len;

int _furia_tx_rx_power_set(const phymod_access_t *pa,
                           phymod_power_t tx_rx_power,
                           uint8_t tx_rx);
/**   Core identify 
 *    This function reads device ID2 & ID3 registers and confirm whether furia
 *    device presents or not. 
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param core_id            Core ID which is supplied by interface layer 
 *    @param is_identified      Flag variable to return device 
 *                              identification status 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_core_identify(const phymod_core_access_t* core, uint32_t core_id,
                        uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_0_t id2;
    IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_1_t id3; 
    uint32_t chip_id = 0;
    chip_id =  _furia_get_chip_id(pm_acc);

    *is_identified = 0;

    if(core_id == 0){
        ioerr += 
        READ_FURIA_PMA_PMD_REG(pm_acc,
                               IEEE_PMA_PMD_BLK0_pmd_identifier_register_0_Adr,
                               &id2.data);  
        ioerr += 
        READ_FURIA_PMA_PMD_REG(pm_acc,
                               IEEE_PMA_PMD_BLK0_pmd_identifier_register_1_Adr,
                               &id3.data);  
    } else {
        id2.data = (uint16_t) ((core_id >> 16) & 0xffff);
        id3.data = (uint16_t) core_id & 0xffff;
    }

    if (((id2.data) == FURIA_ID0 &&
        (id3.data) == FURIA_ID1) && (FURIA_IS_SIMPLEX(chip_id) || FURIA_IS_DUPLEX(chip_id))) {

        /* PHY IDs match */
        *is_identified = 1;
    }
    LOG_BSL_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(0,
                             "Furia PROBE is identified:%d chip ID:%x\n"),
                  *is_identified, chip_id));

        
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;    
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
STATIC
int _furia_core_firmware_load(const phymod_core_access_t* core,
                              phymod_firmware_load_method_t load_method,
                              phymod_firmware_loader_f fw_loader)
{
    int ret_val = 0; 
    const phymod_access_t *pm_acc = &core->access;
    switch(load_method)
    {
        case phymodFirmwareLoadMethodInternal:
            PHYMOD_DIAG_OUT((" Starting Firmware download through MDIO,  it takes few seconds...\n"));
            ret_val = furia_download_prog_eeprom(pm_acc,
                                                 furia_ucode_Furia,
                                                 furia_ucode_Furia_len,
                                                 0);
            if (ret_val != furia_ucode_Furia_len) {
                PHYMOD_RETURN_WITH_ERR
                    (PHYMOD_E_CONFIG,
                    (_PHYMOD_MSG("firmware download failed")));
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
            ret_val = furia_download_prog_eeprom(pm_acc,
                                                 furia_ucode_Furia,
                                                 furia_ucode_Furia_len,
                                                 1);
            if (ret_val == PHYMOD_E_FAIL) {
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_FAIL,
                 (_PHYMOD_MSG("Flasing Firmware to EEPROM faliled failed")));
            } else if (ret_val != furia_ucode_Furia_len) {
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

/**   Set Interface config 
 *    This function sets interface configuration (interface, speed, frequency of
 *    operation). This is requried to put the furia into specific mode specified
 *    by the user.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use 
 *    @param config             Interface config structure where user specifies 
 *                              interface, speed and the frequency of operation.
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_interface_config_set(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   const phymod_phy_inf_config_t* config)
{
    return furia_set_config_mode(&phy->access,
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
int furia_phy_interface_config_get(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   phymod_ref_clk_t ref_clock,
                                   phymod_phy_inf_config_t* config)
{
    return furia_get_config_mode(&phy->access,
                       &config->interface_type,
                       &config->data_rate,
                       &config->ref_clock,
                       &config->interface_modes);
}

/**   Core Init  
 *    This function initializes the furia core and 
 *    download the firmware and setting the PHY into 
 *    default mode. 
 *
 *    @param phy                Pointer to phymod core access structure 
 *    @param init_config        Init configuration specified by user 
 *    @param core_status        Core status read from PHY chip 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_core_init(const phymod_core_access_t* core,
                    const phymod_core_init_config_t* init_config,
                    const phymod_core_status_t* core_status)
{
    uint32_t phy_id = 0;
    uint32_t rptr_rtmr_mode = 1;


    PHYMOD_IF_ERR_RETURN
        (_furia_core_firmware_load(core,
                                   init_config->firmware_load_method,
                                   init_config->firmware_loader));


    /* Configure furia as either RPTR or RETIMER based on static config specified by the user*/
    PHYMOD_IF_ERR_RETURN
        (_furia_core_rptr_rtmr_mode_set(core, rptr_rtmr_mode));
    phy_id = core->access.addr;

    /* Update the global sw database with default mode */
    glb_interface[phy_id][0] = phymodInterfaceLR4;
    glb_interface[phy_id][1] = phymodInterfaceLR4;
    glb_interface[phy_id][2] = phymodInterfaceLR4;
    glb_interface[phy_id][3] = phymodInterfaceLR4;

    return PHYMOD_E_NONE;
}


/**   PHY init
 *    This function initializes PHY 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param init_config        User specified phy init configuration
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_init(const phymod_phy_access_t* phy,
                   const phymod_phy_init_config_t* init_config)
{
/*    int pll_restart = 0;
    const phymod_access_t *pm_acc = &phy->access;
*/
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
int furia_phy_link_status_get(const phymod_phy_access_t* phy,
                              uint32_t* link_status)
{
    return furia_link_status(&phy->access, link_status);
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
int furia_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr,
                       uint32_t *val)
{
    return furia_reg_read(&phy->access,\
                        FURIA_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
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
int furia_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr,
                        uint32_t val)
{
    return furia_reg_write(&phy->access,\
                         FURIA_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
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
int furia_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{
    uint32_t chip_id;
    chip_id =  _furia_get_chip_id(&phy->access);
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionFuriaA2;
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
int furia_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return furia_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity);
}


/**  PHY polarity get
 *   This function is used to get the lane polarity
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param polarity           Pointer to phymod_polarity_t for rx and tx
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return furia_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity);
}

/**  PHY Rx PMD lock 
 *   PHY Rx PMD lock This function is used to get the rx PMD lock status 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param rx_seq_done        Pointer to rx sequence    
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done){
    return furia_pmd_lock_get(&phy->access, rx_seq_done);
}

/**  PHY power set 
 *   PHY power set This function is used to set the power
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param power              Pointer to phymod phy power structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_power_set(const phymod_phy_access_t* phy,
                        const phymod_phy_power_t* power)
{
    /* Set Tx power */
    _furia_tx_rx_power_set(&phy->access, power->tx, 0);
    /* Set Rx Power */
    _furia_tx_rx_power_set(&phy->access, power->rx, 1);
    return PHYMOD_E_NONE;
}

int _furia_tx_rx_power_set(const phymod_access_t *pa,
                           phymod_power_t tx_rx_power,
                           uint8_t tx_rx)
{
    switch(tx_rx_power) {
        case phymodPowerOff:
            PHYMOD_IF_ERR_RETURN
                (furia_tx_rx_power_set(pa, tx_rx, 0));
        break;
        case phymodPowerOn:
            PHYMOD_IF_ERR_RETURN
                (furia_tx_rx_power_set(pa, tx_rx, 1));
        break;
        case phymodPowerOffOn:
        break;
        case phymodPowerNoChange:
        break; 
        default:
        break; 
    }
    return PHYMOD_E_NONE;
}

/**  PHY power get 
 *   PHY power get This function is used to get the power 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param power              Pointer to phymod phy power structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    phymod_power_t power_tx = 0, power_rx = 0;
    PHYMOD_IF_ERR_RETURN    
        (furia_tx_rx_power_get(&phy->access, &power_tx, &power_rx));
    power->tx = power_tx;
    power->rx = power_rx; 
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
int furia_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    furia_an_ability_t value;
    PHYMOD_MEMSET(&value, 0, sizeof(value));

    value.cl73_adv.an_fec = an_ability->an_fec;

    /*check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)){
        return PHYMOD_E_PARAM;
    }

    /*next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = FURIA_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = FURIA_ASYM_PAUSE;
    } else {
        value.cl73_adv.an_pause = FURIA_NO_PAUSE;
    }

    /*check cl73 and cl73 bam ability */
    if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap)) {
        _furia_config_pll_div(&phy->access, PLL_MODE_132, REF_CLK_156p25Mhz);
        value.cl73_adv.an_base_speed = FURIA_CL73_1000BASE_KX;
    } else if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap)) {
        _furia_config_pll_div(&phy->access, PLL_MODE_132, REF_CLK_156p25Mhz);
        value.cl73_adv.an_base_speed = FURIA_CL73_10GBASE_KR;
    } else if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) {
        _furia_config_pll_div(&phy->access, PLL_MODE_132, REF_CLK_156p25Mhz);
        value.cl73_adv.an_base_speed = FURIA_CL73_40GBASE_KR4;
    } else if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) {
        _furia_config_pll_div(&phy->access, PLL_MODE_132, REF_CLK_156p25Mhz);
        value.cl73_adv.an_base_speed = FURIA_CL73_40GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_CR10_GET(an_ability->an_cap)) { 
        return PHYMOD_E_UNAVAIL;
    } else if (PHYMOD_AN_CAP_100G_CR4_GET(an_ability->an_cap)) { 
        _furia_config_pll_div(&phy->access, PLL_MODE_165, REF_CLK_156p25Mhz);
        value.cl73_adv.an_base_speed = FURIA_CL73_100GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_KR4_GET(an_ability->an_cap)) { 
        _furia_config_pll_div(&phy->access, PLL_MODE_165, REF_CLK_156p25Mhz);
        value.cl73_adv.an_base_speed = FURIA_CL73_100GBASE_KR4;
    } else {
        return PHYMOD_E_PARAM;
    } 

    PHYMOD_IF_ERR_RETURN
        (_furia_autoneg_ability_set(&phy->access, &value));
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
int furia_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    furia_an_ability_t value;

   PHYMOD_IF_ERR_RETURN(   
    _furia_autoneg_ability_get (&phy->access, &value));
   
    an_ability_get_type->an_fec = value.cl73_adv.an_fec;
    switch (value.cl73_adv.an_pause) {
        case FURIA_ASYM_PAUSE:
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
        case FURIA_SYMM_PAUSE:
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
        default:
            break;
    }

    /*first check cl73 ability*/
    switch (value.cl73_adv.an_base_speed) {
        case FURIA_CL73_100GBASE_CR10:
            PHYMOD_AN_CAP_100G_CR10_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_10GBASE_KR:
            PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_1000BASE_KX:
            PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_100GBASE_KR4:
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
int furia_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return (_furia_autoneg_set(&phy->access, an));
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
int furia_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return (_furia_autoneg_get(&phy->access, an, an_done));
}

/**  Get CL73 status 
 *   This function is used get CL73 resolved status
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param status             Pointer to AN resolved status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}

/**  Set Core reset 
 *   This function is used to reset the core
 *
 *    @param core                Pointer to phymod core access structure
 *    @param reset_mode          Type of reset
 *        0 - Hard reset 
 *        1 - Soft reset
 *    @param direction           Reset direction
 *        0 - In
 *        1 - Out
 *        2 - In Out (toggle)
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_core_reset_set(const phymod_core_access_t* core,
                         phymod_reset_mode_t reset_mode,
                         phymod_reset_direction_t direction)
{
    return furia_reset_set(&core->access, reset_mode, direction); 
}
/**  Get Core reset 
 *   This function is used to get the status of core reset
 *
 *    @param core                Pointer to phymod core access structure
 *    @param reset_mode          Type of reset
 *        0 - Hard reset 
 *        1 - Soft reset
 *    @param direction           Reset direction
 *        0 - In
 *        1 - Out
 *        2 - In Out (toggle)
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_core_reset_get(const phymod_core_access_t* core,
                         phymod_reset_mode_t reset_mode,
                         phymod_reset_direction_t* direction)
{
    return PHYMOD_E_UNAVAIL;
}

/**  Tx set 
 *   This function is used to set transmitter analog parameters 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    return furia_tx_set(&phy->access, tx);
}

/**  Tx get 
 *   This function is used to get transmitter analog parameters 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    return furia_tx_get(&phy->access, tx);
}

/**  Rx set 
 *   This function is used to set receiver analog parameters 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    return furia_rx_set(&phy->access, rx);
}

/**  Rx get 
 *   This function is used to get receiver analog parameters 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    return furia_rx_get(&phy->access, rx);
}

/**  Set PHY reset 
 *   This function is used to perform PHY reset 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reset              Pointer to phy reset structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_reset_set(const phymod_phy_access_t* phy,
                        const phymod_phy_reset_t* reset)
{
    return PHYMOD_E_NONE;
}

/**  Get PHY reset 
 *   This function is used to get status of PHY reset 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reset              Pointer to phy reset structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_reset_get(const phymod_phy_access_t* phy,
                        phymod_phy_reset_t* reset)
{
    return PHYMOD_E_NONE;
}

/**  Tx lane control set 
 *   This function is used to set the lane control for example
 *   resetting the traffic 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_tx_lane_control_set(const phymod_phy_access_t* phy,
                                  phymod_phy_tx_lane_control_t tx_control)
{
     return furia_tx_lane_control_set(&phy->access, tx_control);
}

/**  Tx lane control get 
 *   This function is used to get the lane control status for 
 *   a particular lane control
 *    
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_tx_lane_control_get(const phymod_phy_access_t* phy,
                                  phymod_phy_tx_lane_control_t* tx_control)
{
    return PHYMOD_E_UNAVAIL;
}


/**  Rx lane control set 
 *   This function is used to set the lane control for example
 *   resetting the traffic 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_lane_control_set(const phymod_phy_access_t* phy,
                                  phymod_phy_rx_lane_control_t rx_control)
{
    return furia_rx_lane_control_set(&phy->access, rx_control);
}


/**  Rx lane control get 
 *   This function is used to get the lane control status for 
 *   a particular lane control
 *    
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_lane_control_get(const phymod_phy_access_t* phy,
                                  phymod_phy_rx_lane_control_t* rx_control)
{
    return PHYMOD_E_UNAVAIL;
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
int furia_phy_loopback_set(const phymod_phy_access_t* phy,
                           phymod_loopback_mode_t loopback,
                           uint32_t enable)
{
    return furia_loopback_set(&phy->access, loopback, enable);
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
int furia_phy_loopback_get(const phymod_phy_access_t* phy,
                           phymod_loopback_mode_t loopback,
                           uint32_t* enable)
{
    return furia_loopback_get(&phy->access, loopback, enable);
}

/**  Get core firmware info 
 *   This function is used to get core firmware version and CRC 
 *    
 *
 *    @param core                Pointer to phymod core access structure
 *    @param fw_info             Firmware information such as version and CRC
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    return furia_firmware_info_get(&core->access, fw_info);
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
int furia_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return furia_pll_sequencer_restart(&core->access, operation);
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
int furia_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return furia_fec_enable_set(&phy->access, enable);
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
int furia_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return furia_fec_enable_get(&phy->access, enable);
}
/**  PHY status dump 
 *   This function is used to display status dump of a core and for given lane 
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_status_dump(const phymod_phy_access_t* phy)
{
    return _furia_phy_status_dump(&phy->access);
}
#endif /* PHYMOD_FURIA_SUPPORT */
