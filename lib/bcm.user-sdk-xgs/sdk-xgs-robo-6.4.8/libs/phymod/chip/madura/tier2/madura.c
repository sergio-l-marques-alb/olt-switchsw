/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
#include <phymod/phymod_reg.h>
#include <phymod/phymod_dispatch.h>

#ifdef PHYMOD_MADURA_SUPPORT


#include <phymod/chip/madura.h>
#include "../tier1/madura_cfg_seq.h"
#include "../tier1/bcmi_madura_defs.h"

/**   Core identify 
 *    This function reads PMD Identifiers, Set is identified if
 *    madura identified. 
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param core_id            Core ID which is supplied by interface layer 
 *    @param is_identified      Flag variable to return device 
 *                              identification status 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
	const phymod_access_t *pm_acc = &core->access;
	uint32_t chip_id;
	uint32_t rev_id;

	PMD_IDENTIFIER_0r_t id0;
	PMD_IDENTIFIER_1r_t id1;

	PHYMOD_MEMSET(&id0, 0, sizeof(PMD_IDENTIFIER_0r_t));
	PHYMOD_MEMSET(&id1, 0, sizeof(PMD_IDENTIFIER_1r_t));
	*is_identified = 0;

	if (core_id == 0) {
		PHYMOD_IF_ERR_RETURN (
				READ_PMD_IDENTIFIER_1r(pm_acc, &id1));
		PHYMOD_IF_ERR_RETURN (
				READ_PMD_IDENTIFIER_0r(pm_acc, &id0));
	} else {
		id0.v[0] = (core_id >> 16) & 0xffff;
		id1.v[0] = core_id & 0xffff;
		/* id1=core_id & 0xffff; */
	}
	if (id0.v[0] == MADURA_PMD_ID0 && id1.v[0] == MADURA_PMD_ID1) {
		/* PHY IDs match - now check model */
		PHYMOD_IF_ERR_RETURN(madura_get_chipid(pm_acc, &chip_id, &rev_id));
		if (chip_id == MADURA_CHIP_ID_82864) {
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
int madura_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    uint32_t chip_id;
    uint32_t rev_id;
    PMD_IDENTIFIER_0r_t id0;
    PMD_IDENTIFIER_1r_t id1;

    PHYMOD_IF_ERR_RETURN (
               madura_get_chipid(&core->access, &chip_id, &rev_id));
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionMadura;

    PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_1r(&core->access, &id1));
    info->phy_id1 = id1.v[0];
    PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_0r(&core->access, &id0));
    info->phy_id0 = id0.v[0];
    
    return PHYMOD_E_NONE;
    
}

/**   Get Firmware info
 *    This function get the firmware information such as master firmware version and master checksum
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param fw_info            Represent firmware version and checksum.
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
        
    FIRMWARE_VERSIONr_t  firmware_version;
    MST_RUNNING_CHKSUMr_t  mst_running_chksum;

    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN(
        READ_FIRMWARE_VERSIONr(&core->access, &firmware_version));

    fw_info->fw_version = FIRMWARE_VERSIONr_FIRMWARE_VERSION_VALf_GET(firmware_version);

    PHYMOD_IF_ERR_RETURN(
        READ_MST_RUNNING_CHKSUMr(&core->access, &mst_running_chksum));

    fw_info->fw_crc = MST_RUNNING_CHKSUMr_MST_RUNNING_CHKSUM_VALf_GET(mst_running_chksum);

    return PHYMOD_E_NONE;    
    
}

/**   Core Reset
 *    This function reset madura core, it support hard and soft reset
 *
 *    @param core               Pointer to phymod core access structure
 *    @param reset_mode         Represent hard/soft reset to perform
 *    @param direction          Represet direction of reset. Sesto ignore this parameter
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return _madura_core_reset_set(&core->access, reset_mode, direction);

}

int madura_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{


    /* Place your code here */


    return PHYMOD_E_UNAVAIL;

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

int madura_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return _madura_pll_sequencer_restart(core, operation);
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
int madura_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    PHYMOD_IF_ERR_RETURN(
      _madura_tx_power_set(&phy->access, power->tx));
    PHYMOD_IF_ERR_RETURN(
      _madura_rx_power_set(&phy->access, power->rx));
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

int madura_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return _madura_tx_rx_power_get(&phy->access, power);

}


/**  PHY polarity set
 *   This function is used to set the lane polarity
 *
 *   @param phy          Pointer to phymod phy access structure
 *   @param polarity     Pointer to phymod_polarity_t for rx and tx
 *
 *   @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return _madura_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity);
}


/**  PHY polarity get
 *   This function is used to get the lane polarity
 *
 *   @param phy          Pointer to phymod phy access structure
 *   @param polarity     Pointer to phymod_polarity_t for rx and tx
 *
 *   @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return _madura_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity);
}

int madura_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    /*More work is remaining !!*/
    uint16_t retry_cnt = 5;
    FIRMWARE_ENr_t fwe;

    PHYMOD_IF_ERR_RETURN (
            _madura_phy_interface_config_set(phy, flags, config));

    /* Enable FW After configuring mode */
    PHYMOD_IF_ERR_RETURN(
        _madura_fw_enable(&phy->access, MADURA_ENABLE));

    do {
        PHYMOD_USLEEP(100);

        PHYMOD_IF_ERR_RETURN(
            READ_FIRMWARE_ENr(&phy->access, &fwe));
    } while ((FIRMWARE_ENr_FW_ENABLE_VALf_GET(fwe) != 0) && (retry_cnt --));

    if (retry_cnt == 0) {
        PHYMOD_DEBUG_VERBOSE(("WARN:: FW Enable not cleared\n"));
    }

    return PHYMOD_E_NONE;

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
int madura_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
        
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    config->device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;
    if (aux_mode == NULL) {
        PHYMOD_DEBUG_VERBOSE(("AUX MODE MEM NOT ALLOC\n"));
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(aux_mode, 0, sizeof(MADURA_DEVICE_AUX_MODE_T));

    PHYMOD_IF_ERR_RETURN (
            _madura_phy_interface_config_get(&phy->access, flags, config));

    PHYMOD_FREE(config->device_aux_modes);
    return PHYMOD_E_NONE;
    
}

/**   Madura core initialization
 *    This function initialize the madura core by downlaoding the firmware.
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param init_config        Init configuration specified by user 
 *    @param core_status        PMD status read from PHY chip 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */


int madura_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
     /*uint32_t pmd_status = 0;*/
    PHYMOD_IF_ERR_RETURN(
            _madura_core_init(core, init_config));
    
    /* Retrive PMD lock status on line side for all the lanes */
    /*PHYMOD_IF_ERR_RETURN(
            _madura_rx_pmd_lock_get(&core->access, &pmd_status));
    PHYMOD_DEBUG_VERBOSE(("PMD Status: %x\n", pmd_status));*/
        
    return PHYMOD_E_NONE;
    
}


int madura_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{

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
int madura_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint32_t ena_dis = 0; 
    _madura_loopback_get(&phy->access, loopback, &ena_dis);
    if (ena_dis || enable) {
	    return _madura_loopback_set(&phy->access, loopback, enable);
    }
    return PHYMOD_E_NONE;
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
int madura_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return _madura_loopback_get(&phy->access, loopback, enable);
}


/**   Get PHY RX PMD link status 
 *    This function retrieves RX PMD lock status of madura 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param rx_pmd_locked      Rx PMD Link status retrieved from Madura
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int madura_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
        
    return (_madura_rx_pmd_lock_get(&phy->access, rx_pmd_locked));
    
}


/**   Get PCS link status
 *    This function retrieves PCS link status of madura when PCS Monitor is enabled,
 *    This function return PMD lock when PCS monitor is disabled.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param link_status        Retrives PCS Link of Madura
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int madura_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    return  _madura_get_pcs_link_status(&phy->access, link_status);

}


int madura_phy_status_dump(const phymod_phy_access_t* phy)
{
   return   _madura_phy_status_dump(&phy->access);  
}


/**   Read Register
 *    This function read user specified register based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           Madura register address
 *    @param val                Output parameter, represents the value of address specified
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    uint16_t dev_add = 0;
    dev_add = (phy->access.devad) ? phy->access.devad : MADURA_DEV_PMA_PMD;

    if (dev_add == MADURA_DEV_PMA_PMD) {
        READ_MADURA_PMA_PMD_REG(&phy->access, reg_addr, *val);
    } else {
        READ_MADURA_AN_REG(&phy->access, reg_addr, *val);
    }
        
    return PHYMOD_E_NONE;
    
}


/**   Write Register
 *    This function write user specified value to the specified address 
 *    based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           Madura register address
 *    @param val                Represents the value to be written
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
        
    uint16_t dev_add = 0;
    dev_add = (phy->access.devad) ? phy->access.devad : MADURA_DEV_PMA_PMD;
    PHYMOD_DEBUG_VERBOSE(("WRITE reg:%x data:%x\n", reg_addr, val));
    if (dev_add == MADURA_DEV_PMA_PMD) {
        WRITE_MADURA_PMA_PMD_REG(&phy->access, reg_addr, val);
    } else {
        WRITE_MADURA_AN_REG(&phy->access, reg_addr, val);
    }
        
    return PHYMOD_E_NONE;
    
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

int madura_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    return _madura_phy_reset_set(&phy->access, reset);
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
int madura_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    return _madura_phy_reset_get(&phy->access, reset);
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
int madura_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _madura_fec_enable_set(&phy->access, enable);
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
int madura_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return _madura_fec_enable_get(&phy->access, enable);
}


#endif /* PHYMOD_MADURA_SUPPORT */
