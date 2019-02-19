/*
 *         
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
 *         
 *     
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>


#include <soc/portmod/xlmac.h>
#include <soc/portmod/pm4x10.h>
        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X10_SUPPORT

/* 10G mode at 10.3125G and 1G mode at 1.25G */
#define PM4X10_LANES_PER_CORE (4)
#define MAX_PORTS_PER_PM4X10 (4)

#define PM_4x10_INFO(pm_info) ((pm_info)->pm_data.pm4x10_db)

/* Warm Boot Variables to indicate the boot state */
typedef enum pm4x10_fabric_wb_vars{
    isInitialized,
    isActive,
    isBypassed,
    ports
}pm4x10_wb_vars_t;

struct pm4x10_s{
    soc_pbmp_t phys;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity[PM4X10_LANES_PER_CORE];
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    int blk_id;
    phymod_core_access_t core_access[1 + MAX_PHYN];
    int nof_ext_phys;
    uint8 in_pm12x10;
};

int pm4x10_pm_interface_type_is_supported (int unit, soc_port_if_t interface, 
                                           int* is_supported)
{
    SOC_INIT_FUNC_DEFS;
    switch(interface){
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_XFI:
             *is_supported = TRUE;
             break;

        /* these modes pm4x10 should be bypass */
        case SOC_PORT_IF_QSGMII:
        case SOC_PORT_IF_CAUI:
             *is_supported = TRUE;
             break;
        default:
             *is_supported = FALSE;
             break;
    }
    SOC_FUNC_RETURN;
}
/*
 * Initialize the buffer to support warmboot
 * The state of warmboot is store in the variables like
 * isInitialized, isActive, isBypassed, ports.. etc.,
 * All of these variables need to be added to warmboot
 * any variables added to save the state of warmboot should be 
 * included here.  
 */
STATIC
int pm4x10_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;
    
    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm4x10", NULL, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));

    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInitialized] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));

    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM4X10, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ports] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;
    
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_bypassed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isBypassed] = wb_var_id;
    
    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));
    if(!SOC_WARM_BOOT(unit)){
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], -1);
        _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_pm_destroy(int unit, pm_info_t pm_info)
{
    if(pm_info->pm_data.pm4x10_db != NULL) {
        sal_free(pm_info->pm_data.pm4x10_db);
        pm_info->pm_data.pm4x10_db = NULL;
    }
    return SOC_E_NONE;
}

STATIC int
pm4x10_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{
    return SOC_E_NONE;
}

STATIC
int
pm4x10_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(XLPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm4x10_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(XLPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}


phymod_bus_t pm4x10_default_bus = {
    "PM4x10 Bus",
    pm4x10_default_bus_read,
    pm4x10_default_bus_write,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};


int pm4x10_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_pm4x10_create_info_t *info = &pm_add_info->pm_specific_info.pm4x10;
    pm4x10_t pm4x10_data = NULL;
    int i;
    SOC_INIT_FUNC_DEFS;

    pm4x10_data = sal_alloc(sizeof(struct pm4x10_s), "pm4x10_specific_db");
    SOC_NULL_CHECK(pm4x10_data);

    pm_info->type = portmodDispatchTypePm4x10;
    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.pm4x10_db = pm4x10_data;

    SOC_PBMP_ASSIGN(pm4x10_data->phys, pm_add_info->phys);
    pm4x10_data->core_access[0].type = phymodDispatchTypeCount;

    pm4x10_data->nof_ext_phys = 0;
    for(i = 0 ; i < MAX_PHYN + 1; i++){
        phymod_core_access_t_init(&pm4x10_data->core_access[i]);
    }

    if(!info->in_pm_12x10){
        if(info->ref_clk != phymodRefClk156Mhz) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("illegal ref clock")));
        }
        if(info->lane_map.num_of_lanes != PM4X10_LANES_PER_CORE){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("lanes map length should be 4")));
        }
        _SOC_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(info->fw_load_method));

        sal_memcpy(pm4x10_data->polarity, info->polarity, sizeof(pm4x10_data->polarity[0])*PM4X10_LANES_PER_CORE);
        sal_memcpy(&(pm4x10_data->core_access[0].access), &info->access, sizeof(phymod_access_t));
        sal_memcpy(&pm4x10_data->lane_map, &info->lane_map, sizeof(pm4x10_data->lane_map));

        pm4x10_data->ref_clk = info->ref_clk;
        pm4x10_data->fw_load_method = info->fw_load_method;
        pm4x10_data->external_fw_loader = info->external_fw_loader;

        if(info->access.bus == NULL) {
            /* if null - use default */
            pm4x10_data->core_access[0].access.bus = &pm4x10_default_bus;
        }

        if(pm4x10_data->external_fw_loader == NULL) {
            /* if null - use default */
            pm4x10_data->external_fw_loader = pm4x10_default_fw_loader;
        }

    }

    pm4x10_data->blk_id = pm_add_info->blk_id;
    pm4x10_data->in_pm12x10 = info->in_pm_12x10;

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm4x10_wb_buffer_init(unit, wb_buffer_index,  pm_info));

exit:
    if(SOC_FUNC_ERROR){
        pm4x10_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_pm_enable(int unit, int port, pm_info_t pm_info, int port_mode)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLPORT_MODE_REGr(unit, port, &reg_val));

    /* Set port core port mode and phy port mode appropriately */ 
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &reg_val, XPORT0_CORE_PORT_MODEf, port_mode);
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &reg_val, XPORT0_PHY_PORT_MODEf,  port_mode);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_MODE_REGr(unit, port, reg_val));

    /* Bring XLMAC out of reset. */  
    _SOC_IF_ERR_EXIT(READ_XLPORT_MAC_CONTROLr(unit, port, &reg_val));
    soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 0);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_MAC_CONTROLr(unit, port, reg_val));

#ifdef _CHECK_WHICH_REG_

 
    /* Bring XLMAC out of power down Mode */
    _SOC_IF_ERR_EXIT(READ_XLPORT_XGXS0_CTRL_REGr(unit, port, &reg_val));
    soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 0);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, port, reg_val));

#endif

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_calc_port_mode(int unit, int port, pm_info_t pm_info, int port_index, int phys_count, int *port_mode)
{
    uint32 reg_val = 0;
    uint32 current_port_mode = 0;
    uint32 first_couple_mode = 0;
    uint32 second_couple_mode = 0; /*if zero means that the couple used as single lanes*/

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLPORT_MODE_REGr(unit, port, &reg_val));
    current_port_mode = soc_reg_field_get(unit, XLPORT_MODE_REGr, reg_val, XPORT0_CORE_PORT_MODEf);

    first_couple_mode = ((current_port_mode == 2) || (current_port_mode == 3) || (current_port_mode == 4));
    second_couple_mode = ((current_port_mode == 1) || (current_port_mode == 3) || (current_port_mode == 4));

    if(phys_count == 1){
        if(port_index < 2){
            first_couple_mode = 0;
        }
        else{
            second_couple_mode = 0;
        }
    }else if(phys_count == 2){
        if(port_index == 0){
            first_couple_mode = 1;
        } else{
            second_couple_mode = 1;
        }
    }else if(phys_count == 4){
        *port_mode = 4;
        SOC_EXIT;
    }

    *port_mode = second_couple_mode + first_couple_mode*2;

exit:
    SOC_FUNC_RETURN;

}

STATIC
int pm4x10_phy_init(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
    phymod_core_status_t       status;
    phymod_core_init_config_t  core_config;

    SOC_INIT_FUNC_DEFS;

    phymod_core_status_t_init      (&status);
    phymod_core_init_config_t_init (&core_config);

    status.pmd_active = FALSE;

    sal_memcpy(&core_config.lane_map, &(PM_4x10_INFO(pm_info)->lane_map), sizeof(core_config.lane_map));
    core_config.firmware_load_method = PM_4x10_INFO(pm_info)->fw_load_method;

    if(PM_4x10_INFO(pm_info)->fw_load_method == phymodFirmwareLoadMethodExternal){
        /* Fast firmware load */
        core_config.firmware_loader = PM_4x10_INFO(pm_info)->external_fw_loader;
    } else {
        core_config.firmware_loader = NULL;
    }

    /*
    core_config.firmware_lane_config; 
    core_config.interface.data_rate = config->speed;
    core_config.interface.interface_modes = config->interface_modes;
    core_config.interface.interface_type = ;
    core_config.interface.ref_clock = PM_4x10_INFO(pm_info)->ref_clk;*/
   
    /*phymod_core_init(&core_access)*/
    SOC_FUNC_RETURN;
}

STATIC 
int _pm4x10_port_index_get(int unit, int port, pm_info_t pm_info, int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;
   for( i = 0 ; i < MAX_PORTS_PER_PM4X10; i++){
       rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &tmp_port, i);
       _SOC_IF_ERR_EXIT(rv);
       if(tmp_port == port){
           *first_index = (*first_index == -1 ? i : *first_index);
           SHR_BITSET(bitmap, i);
       }
   }

   if(*first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("port was not found in internal DB %d"), port));
   }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, 
                                    const portmod_access_get_params_t* params, 
                                    int max_phys, phymod_phy_access_t* phy_access, int* nof_phys)
{
    int phyn = 0;
    int port_index;
    SOC_INIT_FUNC_DEFS;

    if(params->phyn > PM_4x10_INFO(pm_info)->nof_ext_phys){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), PM_4x10_INFO(pm_info)->nof_ext_phys, params->phyn));
    }

    phyn = params->phyn;
    if(phyn < 0) {
        phyn = PM_4x10_INFO(pm_info)->nof_ext_phys;
    }
    sal_memcpy(&phy_access[0], &(PM_4x10_INFO(pm_info)->core_access[phyn]), sizeof(PM_4x10_INFO(pm_info)->core_access[phyn]));

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get(unit, port, pm_info, &port_index, &(phy_access[0].access.lane_mask)));

    *nof_phys = 1;    

exit:
    SOC_FUNC_RETURN;

}


int pm4x10_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
    uint32 reg_val, pm_is_bypassed = 0; 
    int    nof_phys = 0, rv = 0, is_core_initialized = 0;

    phymod_phy_access_t         phy;
    phymod_phy_inf_config_t     phy_config;
    portmod_access_get_params_t params;

    SOC_INIT_FUNC_DEFS;
    
    phymod_phy_inf_config_t_init     (&phy_config);
    portmod_access_get_params_t_init (unit, &params);

    params.lane     = -1;
    params.phyn     =  0;
    params.sys_side =  PORTMOD_SIDE_LINE;

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &pm_is_bypassed);  
    _SOC_IF_ERR_EXIT(rv);
    
    if(!pm_is_bypassed){
        _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, port, &reg_val));
        soc_reg_field_set(unit, XLPORT_CONFIGr, &reg_val, HIGIG2_MODEf, PHYMOD_INTF_MODES_HIGIG_GET(config));
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_CONFIGr(unit, port, reg_val));
    
        rv = xlmac_speed_set(unit, port, config->speed);
        _SOC_IF_ERR_EXIT(rv);
        rv = xlmac_encap_set(unit, port, 0, PHYMOD_INTF_MODES_HIGIG_GET(config) ? SOC_ENCAP_HIGIG2 : SOC_ENCAP_IEEE);
        _SOC_IF_ERR_EXIT(rv);
    }

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], &is_core_initialized);
    _SOC_IF_ERR_EXIT(rv);

    if(PM_4x10_INFO(pm_info)->core_access[0].type == phymodDispatchTypeCount){
        _SOC_IF_ERR_EXIT(phymod_core_probe(&PM_4x10_INFO(pm_info)->core_access[0].access, &(PM_4x10_INFO(pm_info)->core_access[0].type)));
    }

    if(!is_core_initialized){
        _SOC_IF_ERR_EXIT(pm4x10_phy_init(unit, port, pm_info , config));
        is_core_initialized = TRUE;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], &is_core_initialized);
        _SOC_IF_ERR_EXIT(rv);
    }

   if(!(PM_4x10_INFO(pm_info)->in_pm12x10)){
        _SOC_IF_ERR_EXIT(pm4x10_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy, &nof_phys));
        phy_config.data_rate = config->speed;
        phy_config.interface_modes = config->interface_modes;
        phy_config.ref_clock = PM_4x10_INFO(pm_info)->ref_clk;

        /*phy_config.interface_type = config->*/
        /*rv = phymod_phy_interface_config_set());*/
        /*_SOC_IF_ERR_EXIT(rv);*/
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config)
{
    phymod_phy_access_t          phy;
    phymod_phy_inf_config_t      phy_config;
    portmod_access_get_params_t  params;

    int nof_phys = 0;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_config));

    portmod_access_get_params_t_init(unit, &params);

    params.lane     = -1;
    params.phyn     =  0;
    params.sys_side =  PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT(pm4x10_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy, &nof_phys));

    if(PM_4x10_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("can't get interface config for PM4x10 within PM12X10")));
    }
    phymod_phy_interface_config_get(&phy, 0, &phy_config);
    config->speed = phy_config.data_rate;
    config->interface_modes = phy_config.interface_modes;
    config->flags = 0;
exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int         phy, port_mode=0, rv=0, port_index = -1, phys_count=0, i=0;
    uint32      pm_is_active = 0, pm_is_bypassed = 0, reg_val = 0;
    soc_pbmp_t  port_phys_in_pm;
    SOC_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);


    /* 
     * add port to ports array according to the first PHY 
     * in the PM that is part of the port
     * (will be used for PM registers) like XLPORT_ENABLE_REG
     */
    SOC_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    SOC_PBMP_AND(port_phys_in_pm, PM_4x10_INFO(pm_info)->phys);
    SOC_PBMP_COUNT(port_phys_in_pm, phys_count);
    i = 0;
    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy){
        if(SOC_PBMP_MEMBER(port_phys_in_pm, phy)){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &port, i);
            _SOC_IF_ERR_EXIT(rv);
            port_index = (port_index == -1 ? i : port_index);
        }
        i++;
    }

    
    _pm4x10_calc_port_mode(unit, port, pm_info, port_index, phys_count, &port_mode);
    

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    if(!pm_is_active){
        /*init the PM*/
        rv = _pm4x10_pm_enable(unit, port, pm_info, port_mode);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_active = 1;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    if(!pm_is_bypassed) {
        rv = xlmac_init(unit, port, 0);
        _SOC_IF_ERR_EXIT(rv);

        _SOC_IF_ERR_EXIT(READ_XLPORT_ENABLE_REGr(unit, port, &reg_val));
        SHR_BITSET(&reg_val, port_index);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_ENABLE_REGr(unit, port, reg_val));  

        _SOC_IF_ERR_EXIT(READ_XLPORT_FAULT_LINK_STATUSr(unit, port, &reg_val));
        soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &reg_val, REMOTE_FAULTf, 1);
        soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &reg_val, LOCAL_FAULTf, 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_FAULT_LINK_STATUSr(unit, port, reg_val));  
    }

    _SOC_IF_ERR_EXIT(pm4x10_port_interface_config_set(unit, port, pm_info, &add_info->interface_config));
    
exit:
    SOC_FUNC_RETURN;   
}

int pm4x10_pm_bypass_set(int unit, pm_info_t pm_info, int bypass_enable)
{
    int     rv = 0;
/*    uint32  reg_val = 0; */
    uint32  pm_is_active = 0;

    SOC_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    if(pm_is_active){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't chenge bypass mode for active pm")));
    }

#ifdef _CHECK_WHICH_REG_
    _SOC_IF_ERR_EXIT(READ_XLPORT_XGXS0_CTRL_REGr(unit, PM_4x10_INFO(pm_info)->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, &reg_val));
    soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, bypass_enable ? 0 : 1);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, PM_4x10_INFO(pm_info)->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, reg_val));
#endif

    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &bypass_enable);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_detach(int unit, int port, pm_info_t pm_info)
{
    int     enable, tmp_port, i=0, rv=0;
    int     invalid_port = -1, is_last_one = TRUE;
    uint32  inactive = 0, reg_val = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x10_port_enable_get(unit, port, pm_info, 0, &enable));
    if(enable){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }

    /*remove from array and check if it was the last one*/
    for( i = 0 ; i < MAX_PORTS_PER_PM4X10; i++){
       rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &tmp_port, i);
       _SOC_IF_ERR_EXIT(rv);
       if(tmp_port == port){
           rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &invalid_port, i);
           _SOC_IF_ERR_EXIT(rv);
           _SOC_IF_ERR_EXIT(READ_XLPORT_ENABLE_REGr(unit, port, &reg_val));
           SHR_BITCLR(&reg_val, i);
           _SOC_IF_ERR_EXIT(WRITE_XLPORT_ENABLE_REGr(unit, port, reg_val));
       } else if (tmp_port != -1){
           is_last_one = FALSE;
       }
   }

    /*deinit PM in case of last one*/
    if(is_last_one){
        _SOC_IF_ERR_EXIT(READ_XLPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &reg_val, XMAC_0_RESETf, 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_MAC_CONTROLr(unit, port, reg_val));

#ifdef _CHECK_WHICH_REG_
        _SOC_IF_ERR_EXIT(READ_XLPORT_XGXS0_CTRL_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, port, reg_val));
#endif

       rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &inactive);
       _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    uint32  is_bypassed = 0, reg_val = 0, orig_reg_val, bitmap;
    int     rv = 0, port_index = 0, mac_flags = 0;
    int     actual_flags = flags, nof_phys = 0;

    phymod_phy_power_t           phy_power;
    phymod_phy_access_t          phy_access;
    portmod_access_get_params_t  params;

    SOC_INIT_FUNC_DEFS;

    phymod_phy_power_t_init(&phy_power);
    portmod_access_get_params_t_init(unit, &params);
    params.lane = -1;
    params.phyn = 0;
    params.sys_side = PORTMOD_SIDE_LINE;

    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    if((is_bypassed) && (!PORTMOD_PORT_ENABLE_PHY_GET(actual_flags))){
        SOC_EXIT;
    }
    if((PM_4x10_INFO(pm_info)->in_pm12x10) && (!PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))){
        SOC_EXIT;
    }
    mac_flags  |= PORTMOD_PORT_ENABLE_RX_GET(flags) ? XLMAC_ENABLE_RX : 0;
    mac_flags  |= PORTMOD_PORT_ENABLE_TX_GET(flags) ? XLMAC_ENABLE_TX : 0;

    phy_power.rx = phymodPowerNoChange;
    phy_power.tx = phymodPowerNoChange;
    if(PORTMOD_PORT_ENABLE_RX_GET(actual_flags)){
        phy_power.rx = (enable) ? phymodPowerOn : phymodPowerOff;
    }
    if(PORTMOD_PORT_ENABLE_TX_GET(actual_flags)){
        phy_power.tx = (enable) ? phymodPowerOn : phymodPowerOff;
    }

    if(enable){
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)){
            _SOC_IF_ERR_EXIT(_pm4x10_port_index_get(unit, port, pm_info, &port_index, &bitmap));
            /*reset MIB counter*/
            _SOC_IF_ERR_EXIT(READ_XLPORT_MIB_RESETr(unit, port, &reg_val));
            orig_reg_val = reg_val;
            SHR_BITSET(&reg_val, port_index);
            _SOC_IF_ERR_EXIT(WRITE_XLPORT_MIB_RESETr(unit, port, reg_val));
            _SOC_IF_ERR_EXIT(WRITE_XLPORT_MIB_RESETr(unit, port, orig_reg_val));
            rv = xlmac_enable_set(unit, port, mac_flags, 1);
            _SOC_IF_ERR_EXIT(rv);
        }
        if((PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) && (!(PM_4x10_INFO(pm_info)->in_pm12x10))){
            _SOC_IF_ERR_EXIT(pm4x10_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys));
            _SOC_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));
        }
    }
    else{
        if((PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) && (!(PM_4x10_INFO(pm_info)->in_pm12x10))){
            _SOC_IF_ERR_EXIT(pm4x10_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys));
            _SOC_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));
        }
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))  && (!is_bypassed)){
            rv = xlmac_enable_set(unit, port, mac_flags, 0);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    rv = xlmac_enable_get(unit, port, 0, enable);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}



int pm4x10_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int lb_enable = 0;
    SOC_INIT_FUNC_DEFS;
    
    /*check if not already defined*/
    _SOC_IF_ERR_EXIT(pm4x10_port_loopback_get(unit, port, pm_info, loopback_type, &lb_enable));
    if(enable == lb_enable){
        SOC_EXIT;
    }

    /* loopback type validation*/
    switch(loopback_type){
        case portmodLoopbackMacOuter:
             _SOC_IF_ERR_EXIT(xlmac_loopback_set(unit, port, loopback_type, enable));
             break;

        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPMD: /*slide*/
        case portmodLoopbackPhyGloopPCS: /*slide*/
        case portmodLoopbackPhyRloopPCS: /*slide*/
             _SOC_IF_ERR_EXIT(portmod_common_phy_loopback_set(unit, port, loopback_type, enable));
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
            break;
    }

exit:
    SOC_FUNC_RETURN; 
}

int pm4x10_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    switch(loopback_type){
        case portmodLoopbackMacOuter:
             _SOC_IF_ERR_EXIT(xlmac_loopback_get(unit, port, loopback_type, enable));
             break;

        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPMD: /*slide*/
        case portmodLoopbackPhyGloopPCS: /*slide*/
        case portmodLoopbackPhyRloopPCS: /*slide*/
             _SOC_IF_ERR_EXIT(portmod_common_phy_loopback_get(unit, port, loopback_type, enable));
             break;

        default:
             _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
             break;
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_ability_local_get(int unit, int port, pm_info_t pm_info, const portmod_port_ability_t* ability)
{
    portmod_port_interface_config_t interface_config;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
   
    pm4x10_port_interface_config_get(unit, port, pm_info, &interface_config);

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_prbs_config_set(int unit, int port, pm_info_t pm_info, int mode, int flags, const phymod_prbs_t* config)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_config_set(unit, port, flags, config));
    }
    else{
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_prbs_config_get(int unit, int port, pm_info_t pm_info, int mode, int flags, phymod_prbs_t* config)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_config_get(unit, port, flags, config));
    }
    else{
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    } 
exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, int mode, int flags, int enable)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_enable_set(unit, port, flags, enable));
    }
    else{
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, int mode, int flags, int* enable)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_enable_get(unit, port, flags, enable));
    } else{
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }
exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_prbs_status_get(int unit, int port, pm_info_t pm_info, int mode, int flags, phymod_prbs_status_t* status)
{
    SOC_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_status_get(unit, port, flags, status));
    }
    else{
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }
exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_firmware_mode_set(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_firmware_mode_get(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t* fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_runt_threshold_set(int unit, int port, pm_info_t pm_info, int value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_runt_threshold_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_max_packet_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_rx_max_size_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_max_packet_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_rx_max_size_get(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_tx_drop_on_local_fault_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(xlmac_local_fault_control_get(unit, port, &control));

    control.drop_tx_on_fault  = enable;
    _SOC_IF_ERR_EXIT(xlmac_local_fault_control_set(unit, port, &control));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_tx_drop_on_local_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(xlmac_local_fault_control_get(unit, port, &control));
    *enable = control.drop_tx_on_fault;

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_tx_drop_on_remote_fault_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_remote_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(xlmac_remote_fault_control_get(unit, port, &control));
    control.drop_tx_on_fault = enable;
    _SOC_IF_ERR_EXIT(xlmac_remote_fault_control_set(unit, port, &control));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_tx_drop_on_remote_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_remote_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(xlmac_remote_fault_control_get(unit, port, &control));
    *enable = control.drop_tx_on_fault;

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_local_fault_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(xlmac_local_fault_control_get(unit, port , &control));
    control.enable = enable;
    _SOC_IF_ERR_EXIT(xlmac_local_fault_control_set(unit, port , &control));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_local_fault_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(xlmac_local_fault_control_get(unit, port , &control));
    *enable = control.enable;

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_local_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv;
    SOC_INIT_FUNC_DEFS;
   
    rv = xlmac_local_fault_status_get(unit, port, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv;     
    SOC_INIT_FUNC_DEFS;

    rv = xlmac_remote_fault_status_get(unit, port, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_pause_control_set(int unit, int port, pm_info_t pm_info, const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_pause_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_pause_control_get(int unit, int port, pm_info_t pm_info, portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_pause_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_llfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_llfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_llfc_control_get(int unit, int port, pm_info_t pm_info, portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_llfc_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores)
{
    SOC_INIT_FUNC_DEFS;
    if(PM_4x10_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("core access get cannot be called for PM4X10 which is part of PM12X10")));
    }
    if(phyn > PM_4x10_INFO(pm_info)->nof_ext_phys){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), PM_4x10_INFO(pm_info)->nof_ext_phys, phyn));
    }
    if(phyn < 0)
    {
        phyn = PM_4x10_INFO(pm_info)->nof_ext_phys;
    }
    sal_memcpy(&core_access_arr[0], &(PM_4x10_INFO(pm_info)->core_access[phyn]), sizeof(PM_4x10_INFO(pm_info)->core_access[phyn]));
    *nof_cores = 1;

exit:
    SOC_FUNC_RETURN;
}


#endif /* PORTMOD_PM4X10_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
