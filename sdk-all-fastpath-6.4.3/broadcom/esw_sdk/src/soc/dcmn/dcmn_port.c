/*
 * $Id:$
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
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT
#include <shared/bsl.h>
#ifdef PORTMOD_SUPPORT

#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_port.h>
#include <soc/portmod/portmod.h>
#include <sal/core/sync.h>

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif /* BCM_DFE_SUPPORT */
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#endif /* BCM_DPP_SUPPORT */

#include <soc/phy/phymod_port_control.h>

/*simulator includes*/
#include <soc/phy/phymod_sim.h>


typedef struct dcmn_port_user_access_s {
    int unit; 
    int fsrd_blk_id; 
    int fsrd_internal_quad;
    sal_mutex_t mutex; 
} dcmn_port_user_access_t;


/****************************************************************************** 
 DCMN MDIO access
*******************************************************************************/

STATIC
int cl45_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    dcmn_port_user_access_t *cl45_user_data;
    uint16 val16;
    int rv;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    cl45_user_data = user_acc;

    rv = soc_dcmn_miim_cl45_read(cl45_user_data->unit, core_addr, reg_addr,  &val16);
    (*val) = val16;

    return rv;
}


STATIC
int cl45_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    dcmn_port_user_access_t *cl45_user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    cl45_user_data = user_acc;

    return soc_dcmn_miim_cl45_write(cl45_user_data->unit, core_addr, reg_addr, val);
}



STATIC
int cl22_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    uint16 val16;
    int rv;
    dcmn_port_user_access_t *cl22_user_data;

    (*val) = 0;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    cl22_user_data = user_acc;

    rv = soc_dcmn_miim_cl22_read(cl22_user_data->unit, core_addr, reg_addr,&val16);
    (*val) = val16;

    return rv;

}

STATIC
int cl22_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    dcmn_port_user_access_t *cl22_user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    cl22_user_data = user_acc;

    return soc_dcmn_miim_cl22_write(cl22_user_data->unit, core_addr, reg_addr, val);
}



STATIC
int mdio_bus_mutex_take(void* user_acc){
    dcmn_port_user_access_t *user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = (dcmn_port_user_access_t *) user_acc;

    return sal_mutex_take(user_data->mutex, sal_mutex_FOREVER); 
}

STATIC
int mdio_bus_mutex_give(void* user_acc){
    dcmn_port_user_access_t *user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = (dcmn_port_user_access_t *) user_acc;

    return sal_mutex_give(user_data->mutex); 
}

phymod_bus_t cl22_bus = {
    "dcmn_cl22_with_mutex",
    cl22_bus_read,
    cl22_bus_write,
    mdio_bus_mutex_take,
    mdio_bus_mutex_give,
    0
};


phymod_bus_t cl45_bus = {
    "dcmn_cl45_with_mutex",
    cl45_bus_read,
    cl45_bus_write,
    mdio_bus_mutex_take,
    mdio_bus_mutex_give,
    0
};


phymod_bus_t cl22_no_mutex_bus = {
    "dcmn_cl22",
    cl22_bus_read,
    cl22_bus_write,
    NULL,
    NULL,
    0
};


phymod_bus_t cl45_no_mutex_bus = {
    "dcmn_cl45",
    cl45_bus_read,
    cl45_bus_write,
    NULL,
    NULL,
    0
};


/** 
 *  @brief read SoC properties to get extenal phy chain of
 *         specified phy.
 *  @param unit-
 *  @param phy - lane number
 *  @param addresses_array_size - the maximum number of phy
 *                              addresses that allowed
 *  @param addresses - (output)core addresses
 *  @param phys_in_chain - (output) number of phys read from the
 *                       SoC properties
 *  @param is_clause45 - (output) clause 45 else clause 22.
 *  @note assume that all phys in the chain work with same MDIO
 *        type.
 *  
 */
soc_error_t
soc_dcmn_external_phy_chain_info_get(int unit, int phy, int addresses_array_size, int *addresses, int *phys_in_chain, int *is_clause45)
{
    int clause;
    SOCDNX_INIT_FUNC_DEFS;

    clause = soc_property_port_get(unit, phy, spn_PORT_PHY_CLAUSE, 22);
    if((clause != 22) && (clause != 45)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR("invalid Clause value %d"), clause));
    }
    *phys_in_chain = soc_property_port_get_csv(unit, phy, spn_PORT_PHY_ADDR, addresses_array_size, addresses);

    *is_clause45 = (clause == 45);

exit:
    SOCDNX_FUNC_RETURN; 
}



/** 
 *  @brief get phymod access structure for mdio core
 *  @param unit-
 *  @param acc_data - access structure allocated in the caller
 *  @param is_clause45 - clause 45/22
 *  @param addr - core address
 *  @param access - function output. the phymod core access
 */
STATIC soc_error_t
soc_dcmn_mdio_phy_access_get(int unit, dcmn_port_user_access_t *acc_data, int is_clause45, uint16 addr, phymod_access_t *access, int *is_sim)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(acc_data);
    SOCDNX_NULL_CHECK(access);
    phymod_access_t_init(access);

    PHYMOD_ACC_USER_ACC(access) = acc_data;
    PHYMOD_ACC_ADDR(access) = addr;
    if(!is_clause45){
        if(acc_data->mutex != NULL){
            PHYMOD_ACC_BUS(access) = &cl22_bus;
        }else{
            PHYMOD_ACC_BUS(access) = &cl22_no_mutex_bus;
        }
    } else{
        if(acc_data->mutex != NULL){
            PHYMOD_ACC_BUS(access) = &cl45_bus;    
        }else{
            PHYMOD_ACC_BUS(access) = &cl45_no_mutex_bus;
        }
         
        PHYMOD_ACC_F_CLAUSE45_SET(access);
    }

    PHYMOD_ACC_DEVAD(access) = 0 | PHYMOD_ACC_DEVAD_FORCE_MASK; 

    SOCDNX_IF_ERR_EXIT(soc_physim_check_sim(unit, phymodDispatchTypeFalcon, access, is_sim));

exit:
    SOCDNX_FUNC_RETURN;  
}

/****************************************************************************** 
 DCMN to portmod mapping
*******************************************************************************/

STATIC
soc_error_t
soc_to_phymod_ref_clk(int unit, int  ref_clk, phymod_ref_clk_t *phymod_ref_clk){
    SOCDNX_INIT_FUNC_DEFS;

    *phymod_ref_clk = phymodRefClkCount;
    switch(ref_clk){
    case soc_dcmn_init_serdes_ref_clock_125:
    case 125:
        *phymod_ref_clk = phymodRefClk125Mhz;
        break;
    case soc_dcmn_init_serdes_ref_clock_156_25:
    case 156:
        *phymod_ref_clk = phymodRefClk156Mhz;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid ref clk %d"), ref_clk));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_to_portmod_lb(int unit, soc_dcmn_loopback_mode_t dmnn_lb_mode, portmod_loopback_mode_t *portmod_lb_mode){
    SOCDNX_INIT_FUNC_DEFS;

    *portmod_lb_mode = portmodLoopbackCount;
    switch(dmnn_lb_mode){
    case soc_dcmn_loopback_mode_none:
        *portmod_lb_mode = portmodLoopbackCount;
        break;
    case soc_dcmn_loopback_mode_mac_async_fifo:
        *portmod_lb_mode = portmodLoopbackMacAsyncFifo;
        break;
    case soc_dcmn_loopback_mode_mac_outer:
        *portmod_lb_mode = portmodLoopbackMacOuter;
        break;
    case soc_dcmn_loopback_mode_mac_pcs:
        *portmod_lb_mode = portmodLoopbackMacPCS;
        break;
    case soc_dcmn_loopback_mode_phy_gloop:
        *portmod_lb_mode = portmodLoopbackPhyGloopPMD;
        break;
    case soc_dcmn_loopback_mode_phy_rloop:
        *portmod_lb_mode = portmodLoopbackPhyRloopPMD;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lb type %d"), dmnn_lb_mode));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


/****************************************************************************** 
 External Firmware loader
*******************************************************************************/
#define UC_TABLE_ENTRY_SIZE (4)


/** 
 *  @brief This function load firmware to all Fabric cores. IF
 *         firmware already loaded this function will do
 *         nothing.
 *         This is callback function that called during phymod
 *         core_init
 *  @param core- phymod core access
 *  @param fw_version - the firmware verision 
 *  @param fw_crc - the firmware crc
 *  @param length -  the firmware length
 *  @param data - the uCode
 *  @note assumes that all cores use the same firmware
 */
int
soc_dcmn_fabric_broadcast_firmware_loader(int unit,  uint32_t length, const uint8_t* data)
{
    soc_reg_above_64_val_t wr_data;
    int i=0;
    int word_index = 0;

    PHYMOD_IF_ERR_RETURN(WRITE_BRDC_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, 0x7)); /*all FSRDs*/
    for (i = 0 ; i < length ; i+= UC_TABLE_ENTRY_SIZE){
        SOC_REG_ABOVE_64_CLEAR(wr_data);
        if(i + UC_TABLE_ENTRY_SIZE < length){
            sal_memcpy((uint8 *)wr_data, data + i, UC_TABLE_ENTRY_SIZE);
        }else{ /*last time*/
            sal_memcpy((uint8 *)wr_data, data + i, length - i);
        }
        /*swap every 4 bytes in case of big endian*/
        for(word_index = 0 ; word_index < sizeof(soc_reg_above_64_val_t)/4; word_index++) {
            wr_data[word_index] = _shr_uint32_read((uint8 *)&wr_data[word_index]);
        }  
        /*we write to index 0 allways*/
        PHYMOD_IF_ERR_RETURN(WRITE_BRDC_FSRD_FSRD_WL_EXT_MEMm(unit, MEM_BLOCK_ALL , 0, wr_data));
    }
    PHYMOD_IF_ERR_RETURN(WRITE_BRDC_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, 0x0));

    return SOC_E_NONE;
}

STATIC int
soc_dcmn_fabric_firmware_loader_callback(const phymod_core_access_t* core,  uint32_t length, const uint8_t* data)
{
    dcmn_port_user_access_t *user_data;
    soc_reg_above_64_val_t wr_data;
    int i=0, unit, reg_access_blk_id;
    int word_index = 0;
    uint32 quad;

    user_data = core->access.user_acc;
    unit = user_data->unit;
    reg_access_blk_id = user_data->fsrd_blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;
    quad = (1 << user_data->fsrd_internal_quad);

    PHYMOD_IF_ERR_RETURN(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, reg_access_blk_id, quad)); 
    for (i = 0 ; i < length ; i+= UC_TABLE_ENTRY_SIZE){
        SOC_REG_ABOVE_64_CLEAR(wr_data);
        if(i + UC_TABLE_ENTRY_SIZE < length){
            sal_memcpy((uint8 *)wr_data, data + i, UC_TABLE_ENTRY_SIZE);
        }else{ /*last time*/
            sal_memcpy((uint8 *)wr_data, data + i, length - i);
        }
        /*swap every 4 bytes in case of big endian*/
        for(word_index = 0 ; word_index < sizeof(soc_reg_above_64_val_t)/4; word_index++) {
            wr_data[word_index] = _shr_uint32_read((uint8 *)&wr_data[word_index]);
        }  
        /*we write to index 0 allways*/
        PHYMOD_IF_ERR_RETURN(WRITE_FSRD_FSRD_WL_EXT_MEMm(user_data->unit, user_data->fsrd_blk_id  , 0, wr_data));
    }
    PHYMOD_IF_ERR_RETURN(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(user_data->unit, reg_access_blk_id, 0x0));

    return PHYMOD_E_NONE;
}

/****************************************************************************** 
 Fabric PMs init
*******************************************************************************/

soc_error_t
soc_dcmn_fabric_pms_add(int unit, int cores_num , int first_port, int use_mutex, int quads_in_fsrd, core_address_get_f address_get_func, void **alloced_buffer)
{
    int i = 0, fabric_idx, is_sim;
    int fmac_block_id, fsrd_block_id;
    int core_port_index = 0;
    int phy = first_port, port = first_port;
    soc_error_t rv;
    uint16 addr = 0;
    portmod_pm_create_info_t pm_info;
    soc_dcmn_init_serdes_ref_clock_t ref_clk = phymodRefClkCount;
    dcmn_port_user_access_t *user_data = NULL;
    uint32 tx_lane_map;
    int cl, is_cl45;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    user_data = sal_alloc(sizeof(dcmn_port_user_access_t)*cores_num , "user_data");
    SOCDNX_NULL_CHECK(user_data);
    for(i = 0 ; i < cores_num ; i++){
        user_data[i].unit = unit;
        if(use_mutex){
            user_data[i].mutex = sal_mutex_create("core mutex");
        }else {
            user_data[i].mutex = NULL;
        }
    }

    /*Clause*/
    cl = soc_property_suffix_num_get(unit, -1, spn_PORT_PHY_CLAUSE  , "fabric", 45);
    if (cl == 45)
    {
        is_cl45 = TRUE;
    } else if (cl == 22) {
        is_cl45 = FALSE;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid clause %d"), cl));
    }


    pm_info.type = portmodDispatchTypeDnx_fabric;
    pm_info.pm_specific_info.dnx_fabric.fw_load_method = soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, "fabric", phymodFirmwareLoadMethodExternal);
    pm_info.pm_specific_info.dnx_fabric.fw_load_method &= 0xff;
    if( pm_info.pm_specific_info.dnx_fabric.fw_load_method == phymodFirmwareLoadMethodExternal){
        pm_info.pm_specific_info.dnx_fabric.external_fw_loader =  soc_dcmn_fabric_firmware_loader_callback;
    }

    /*no lane swap for fabric rx links*/
    pm_info.pm_specific_info.dnx_fabric.lane_map.num_of_lanes = 4;
    for( i = 0 ; i < 4 ; i++){
        pm_info.pm_specific_info.dnx_fabric.lane_map.lane_map_rx[i] = i;
    }
    phy = SOC_INFO(unit).port_l2p_mapping[first_port];

    for(i = 0; i < cores_num ; i++){
        fmac_block_id = FMAC_BLOCK(unit, i);
        fsrd_block_id = FSRD_BLOCK(unit, (i/quads_in_fsrd));
        user_data[i].fsrd_blk_id = fsrd_block_id; 
        user_data[i].fsrd_internal_quad = (i % quads_in_fsrd);

        pm_info.first_blk_id = fmac_block_id;
        pm_info.pm_specific_info.dnx_fabric.fmac_schan_id = SOC_BLOCK_INFO(unit, fmac_block_id).schan;
        pm_info.pm_specific_info.dnx_fabric.fsrd_schan_id = SOC_BLOCK_INFO(unit, fsrd_block_id).schan;
        pm_info.pm_specific_info.dnx_fabric.fsrd_internal_quad = user_data[i].fsrd_internal_quad;
        SOCDNX_IF_ERR_EXIT(address_get_func(unit, i, &addr));
        rv = soc_dcmn_mdio_phy_access_get(unit, &user_data[i], is_cl45, addr, &pm_info.pm_specific_info.dnx_fabric.access, &is_sim);
        SOCDNX_IF_ERR_EXIT(rv);

        if(is_sim) {
            pm_info.pm_specific_info.dnx_fabric.fw_load_method = phymodFirmwareLoadMethodNone;
        }

        ref_clk = SOC_INFO(unit).port_refclk_int[port];
        rv = soc_to_phymod_ref_clk(unit, ref_clk, &pm_info.pm_specific_info.dnx_fabric.ref_clk);
        SOCDNX_IF_ERR_EXIT(rv);

        /*tx lane_swap*/
        if (!SOC_WARM_BOOT(unit))
        {
            tx_lane_map = soc_property_suffix_num_get(unit, i, spn_PHY_TX_LANE_MAP, "quad", SOC_DCMN_PORT_NO_LANE_SWAP);
            for(core_port_index = 0 ;  core_port_index < 4 ; core_port_index++) {
                pm_info.pm_specific_info.dnx_fabric.lane_map.lane_map_tx[core_port_index] = ((tx_lane_map >> (core_port_index * 4)) & 0xf);
            }    
        } else {
            pm_info.pm_specific_info.dnx_fabric.lane_map.lane_map_tx[core_port_index] = 0xFFFFFFFF; /*invalid*/
        }
        
        /*polarity*/
        SOC_PBMP_CLEAR(pm_info.phys);
        for(core_port_index = 0 ;  core_port_index < 4 ; core_port_index++){
            if (!SOC_WARM_BOOT(unit))
            {
                fabric_idx = port - first_port;
                pm_info.pm_specific_info.dnx_fabric.polarity[core_port_index].rx_polarity = soc_property_port_get(unit, fabric_idx, spn_PHY_RX_POLARITY_FLIP, 0xFFFFFFFF);
                pm_info.pm_specific_info.dnx_fabric.polarity[core_port_index].tx_polarity = soc_property_port_get(unit, fabric_idx, spn_PHY_TX_POLARITY_FLIP, 0xFFFFFFFF);

            } else {
                pm_info.pm_specific_info.dnx_fabric.polarity[core_port_index].rx_polarity = 0xFFFFFFFF; /*invalid*/
                pm_info.pm_specific_info.dnx_fabric.polarity[core_port_index].tx_polarity = 0xFFFFFFFF; /*invalid*/

            }
            SOC_PBMP_PORT_ADD(pm_info.phys, phy);
            phy++;
            port++;
        }
        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    }
    *alloced_buffer = user_data;

exit:
    if(_rv != SOC_E_NONE){
        if(user_data != NULL){
            for(i = 0 ; i < cores_num ; i++){
                if(user_data[i].mutex != NULL){
                    sal_mutex_destroy(user_data[i].mutex);
                }
            }
            sal_free(user_data);
        }
    }
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_fabric_pcs_parse(int unit, char *pcs_str, int pcs_def, int *pcs)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (pcs_str == NULL)
    {
        *pcs = pcs_def;
    } else if (!sal_strcmp(pcs_str, "KR_FEC") || !sal_strcmp(pcs_str, "2")) {
        *pcs = PORTMOD_PCS_64B66B_FEC;
    } else if (!sal_strcmp(pcs_str, "64_66") || !sal_strcmp(pcs_str, "4")) {
        *pcs = PORTMOD_PCS_64B66B;
    } else if (!sal_strcmp(pcs_str, "RS_FEC")) {
        *pcs = PORTMOD_PCS_64B66B_RS_FEC;
    } else if (!sal_strcmp(pcs_str, "LL_RS_FEC")) {
        *pcs = PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG_STR("invalid pcs %s"), pcs_str));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
soc_error_t
soc_dcmn_fabric_port_probe(int unit, int port, dcmn_port_init_stage_t init_stage, int fw_verify, dcmn_port_fabric_init_config_t* port_config)
{
    portmod_port_add_info_t info;
    uint32 encoding_properties = 0;
    int phy = 0;
    uint32 value, value_def;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {

        SOCDNX_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &info));

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy = SOC_INFO(unit).port_l2p_mapping[port];
        } else{
            phy = port;
        }
        SOC_PBMP_PORT_ADD(info.phys, phy);
        info.interface_config.interface = SOC_PORT_IF_SFI;
        info.interface_config.speed = port_config->speed;
        info.interface_config.interface_modes = 0;
        info.interface_config.flags = 0;
        info.link_training_en = port_config->cl72;

        if(init_stage == dcmn_port_init_until_fw_load) {
            PORTMOD_PORT_ADD_F_INIT_UNTIL_FW_LOAD_SET(&info);
        }

        if(init_stage == dcmn_port_init_resume_after_fw_load) {
            PORTMOD_PORT_ADD_F_INIT_RESUME_AFTER_FW_LOAD_SET(&info);
        }

        if(fw_verify) {
            PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(&info);
        } else {
            PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&info);
        }

        SOCDNX_IF_ERR_EXIT(portmod_port_add(unit, port, &info));

        if(init_stage == dcmn_port_init_until_fw_load) {
            SOC_EXIT;
        }

        if (port_config->pcs == PORTMOD_PCS_64B66B_FEC)
        {
            PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(encoding_properties);
        } else if (port_config->pcs == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC || port_config->pcs == PORTMOD_PCS_64B66B_RS_FEC)
        {
            PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(encoding_properties);
        }

        SOCDNX_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, encoding_properties, port_config->pcs));

        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_get(unit, port, SOC_PHY_CONTROL_DRIVER_CURRENT, &value_def));
        value = soc_property_port_get(unit, port, spn_SERDES_DRIVER_CURRENT, value_def);
        if (value != value_def)
        {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, SOC_PHY_CONTROL_DRIVER_CURRENT, value));
        }

        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_get(unit, port, SOC_PHY_CONTROL_PREEMPHASIS, &value_def));
        value = soc_property_port_get(unit, port, spn_SERDES_PREEMPHASIS, value_def);
        if (value != value_def)
        {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, SOC_PHY_CONTROL_PREEMPHASIS, value));
        }

        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_get(unit, port, SOC_PHY_CONTROL_FIRMWARE_MODE, &value_def));
        value = soc_property_port_get(unit, port, spn_SERDES_FIRMWARE_MODE, value_def);
        if (value != value_def)
        {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, SOC_PHY_CONTROL_FIRMWARE_MODE, value));
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_phy_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value)
{
    phymod_phy_access_t phys;
    int phys_returned;
    portmod_access_get_params_t params;
    int is_lane_control, rv;
    phymod_ref_clk_t ref_clk;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk);
    SOCDNX_IF_ERR_EXIT(rv);

    portmod_access_get_params_t_init(unit, &params);
    params.lane = -1;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phys, &phys_returned));
    switch (type)
    {
        /*not per lane control*/
        case SOC_PHY_CONTROL_LANE_SWAP:
            is_lane_control = 0;
            break;
        default:
            is_lane_control = 1;
    }
    SOCDNX_IF_ERR_EXIT(soc_port_control_set_wrapper(unit, ref_clk, is_lane_control, &phys, 1, type, value));

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_dcmn_port_phy_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value)
{
    phymod_phy_access_t phys;
    int phys_returned;
    portmod_access_get_params_t params; 
    int is_lane_control, rv;
    phymod_ref_clk_t ref_clk;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk);
    SOCDNX_IF_ERR_EXIT(rv);

    portmod_access_get_params_t_init(unit, &params);
    params.lane = -1;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phys, &phys_returned));
    switch (type)
    {
        /*not per lane control*/
        case SOC_PHY_CONTROL_LANE_SWAP:
            is_lane_control = 0;
            break;
        default:
            is_lane_control = 1;
    }
    SOCDNX_IF_ERR_EXIT(soc_port_control_get_wrapper(unit, ref_clk, is_lane_control, &phys, 1, type, value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_config_get(int unit, soc_port_t port, dcmn_port_fabric_init_config_t* port_config)
{
    int speed;
    char *pcs_str;
    SOCDNX_INIT_FUNC_DEFS;

    /* Update according to soc properties */
    pcs_str = soc_property_port_get_str(unit, port, spn_BACKPLANE_SERDES_ENCODING);
    SOCDNX_IF_ERR_EXIT(soc_dcmn_fabric_pcs_parse(unit, pcs_str, port_config->pcs, &(port_config->pcs)));
    speed = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, port_config->speed);
    if((speed != -1) && (speed != 0))
    {
        port_config->speed = speed;
    }
    port_config->cl72 = soc_property_port_get(unit, port, spn_PORT_INIT_CL72, port_config->cl72) ? 1 : 0;

exit:
    SOCDNX_FUNC_RETURN;
}



#endif /*PORTMOD_SUPPORT*/

#undef _ERR_MSG_MODULE_NAME

