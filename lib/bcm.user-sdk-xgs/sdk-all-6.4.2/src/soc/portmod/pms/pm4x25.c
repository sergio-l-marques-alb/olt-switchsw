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
#include <soc/drv.h>
#include <shared/bsl.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>
        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X25_SUPPORT

#include <soc/portmod/clmac.h>
#include <soc/portmod/pm4x25.h>


#define PM4X25_LANES_PER_CORE (4)
#define MAX_PORTS_PER_PM4X25 (4)

#define PM_4x25_INFO(pm_info) ((pm_info)->pm_data.pm4x25_db)

typedef enum pm4x25_fabric_wb_vars{
    isActive,
    isBypassed, 
    ports,
    threePortsMode
}pm4x25_wb_vars_t;

struct pm4x25_s{
    soc_pbmp_t phys;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity[PM4X25_LANES_PER_CORE];
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    int blk_id;
    phymod_core_access_t core_access[1 + MAX_PHYN];
    int nof_phys /* internal + external phys */;
    uint8 in_pm12x10;
};

#define PM4x25_QUAD_MODE_IF(interface) \
    (interface == SOC_PORT_IF_CAUI || interface == SOC_PORT_IF_XLAUI)

#define PM4x25_DUAL_MODE_IF(interface) \
    (interface == SOC_PORT_IF_XLAUI2)

int pm4x25_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{ 
    SOC_INIT_FUNC_DEFS;

    switch(interface){
        case SOC_PORT_IF_CAUI:
        case SOC_PORT_IF_XLAUI2:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XFI:
            *is_supported = TRUE;
            break;
        default:
            *is_supported = FALSE;
    }

    SOC_FUNC_RETURN;     
}

STATIC 
int portmod_interface_to_phymod_interface(int unit, soc_port_if_t interface, phymod_interface_t *phymod_interface)
{
    SOC_INIT_FUNC_DEFS;

    switch(interface) {
        case SOC_PORT_IF_CAUI:
            (*phymod_interface) = phymodInterfaceCAUI;
            break;
        case SOC_PORT_IF_XLAUI2:
            (*phymod_interface) = phymodInterfaceXLAUI2;
            break;
        case SOC_PORT_IF_XLAUI:
            (*phymod_interface) = phymodInterfaceXLAUI;
            break;
        case SOC_PORT_IF_XFI:
            (*phymod_interface) = phymodInterfaceXFI;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Interface %d not supported"), interface));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC 
int portmod_phymod_interface_to_interface(int unit, phymod_interface_t phymod_interface, soc_port_if_t *interface)
{
    SOC_INIT_FUNC_DEFS;

    switch(phymod_interface) {
        case phymodInterfaceCAUI:
            (*interface) = SOC_PORT_IF_CAUI;
            break;
        case phymodInterfaceXLAUI2:
            (*interface) = SOC_PORT_IF_XLAUI2;
            break;
        case phymodInterfaceXLAUI:
            (*interface) = SOC_PORT_IF_XLAUI;
            break;
        case phymodInterfaceXFI:
            (*interface) = SOC_PORT_IF_XFI;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Phymod Interface %d not supported"), phymod_interface));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int pm4x25_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm4x25", NULL, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM4X25, VERSION(1));
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

     _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "three_ports_mode", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[threePortsMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));
    if(!SOC_WARM_BOOT(unit)){
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], -1);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_pm_destroy(int unit, pm_info_t pm_info)
{    
    SOC_INIT_FUNC_DEFS;

    if(pm_info->pm_data.pm4x25_db != NULL){
        sal_free(pm_info->pm_data.pm4x25_db);
        pm_info->pm_data.pm4x25_db = NULL;
    }

    SOC_FUNC_RETURN; 
}   

STATIC portmod_ucode_buf_t pm4x25_ucode_buf[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};

STATIC int
pm4x25_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{
    int rv;
    portmod_default_user_access_t *user_data;
    int unit = ((portmod_default_user_access_t*)core->access.user_acc)->unit;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    rv = portmod_firmware_set(unit, 
                              user_data->blk_id,
                              data, 
                              length, 
                              portmod_ucode_buf_order_reversed, 
                              &(pm4x25_ucode_buf[0]),
                              CLPORT_WC_UCMEM_DATAm, 
                              CLPORT_WC_UCMEM_CTRLr);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}


STATIC
int
pm4x25_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CLPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm4x25_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CLPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x25_default_bus = {
    "PM4x25 Bus",
    pm4x25_default_bus_read,
    pm4x25_default_bus_write,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL

};

int pm4x25_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
        
    const portmod_pm4x25_create_info_t *info = &pm_add_info->pm_specific_info.pm4x25;
    pm4x25_t pm4x25_data = NULL;
    int i, pm_is_active, rv, three_ports_mode;
    int bypass_enable;
    SOC_INIT_FUNC_DEFS;

    pm_info->type = portmodDispatchTypePm4x25;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    /* PM4x25 specific info */
    pm4x25_data = sal_alloc(sizeof(struct pm4x25_s), "specific_db");
    SOC_NULL_CHECK(pm4x25_data);
    pm_info->pm_data.pm4x25_db = pm4x25_data;

    SOC_PBMP_ASSIGN(pm4x25_data->phys, pm_add_info->phys);
    pm4x25_data->core_access[0].type = phymodDispatchTypeCount;
    
    pm4x25_data->nof_phys = 0;
    for(i = 0 ; i < MAX_PHYN + 1; i++){
        phymod_core_access_t_init(&pm4x25_data->core_access[i]);
    }

    if(!info->in_pm_12x10){
        if((info->ref_clk != phymodRefClk156Mhz) && (info->ref_clk != phymodRefClk125Mhz)){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("illegal ref clock")));
        }
        if(info->lane_map.num_of_lanes != PM4X25_LANES_PER_CORE){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("lanes map length should be 4")));
        }
        _SOC_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(info->fw_load_method));

        sal_memcpy(pm4x25_data->polarity, info->polarity, sizeof(pm4x25_data->polarity[0])*PM4X25_LANES_PER_CORE);
        sal_memcpy(&(pm4x25_data->core_access[0].access), &info->access, sizeof(phymod_access_t));
        sal_memcpy(&pm4x25_data->lane_map, &info->lane_map, sizeof(pm4x25_data->lane_map));
        pm4x25_data->ref_clk = info->ref_clk;
        pm4x25_data->fw_load_method = info->fw_load_method;
        pm4x25_data->external_fw_loader = info->external_fw_loader;

        if(info->access.bus == NULL) {
            /* if null - use default */
            pm4x25_data->core_access[0].access.bus = &pm4x25_default_bus;
        }

        if(pm4x25_data->external_fw_loader == NULL) {
            /* if null - use default */
            pm4x25_data->external_fw_loader = pm4x25_default_fw_loader;
        }

        pm4x25_data->nof_phys = 1; /* Internal Phy */

    }
    pm4x25_data->blk_id = pm_add_info->blk_id;
    pm4x25_data->in_pm12x10 = info->in_pm_12x10;
    
    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm4x25_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    pm_is_active = 0;
    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    bypass_enable = 0;
    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &bypass_enable);
    _SOC_IF_ERR_EXIT(rv);

    three_ports_mode = 0;
    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[threePortsMode], &three_ports_mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    if(SOC_FUNC_ERROR){
        pm4x25_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

STATIC 
int _pm4x25_tsc_reset(int unit, int port, int in_reset)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* Bring Internal Phy OOR */
    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, port, &reg_val));
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, reg_val));

    sal_usleep(1100);

exit:
    SOC_FUNC_RETURN;
}

typedef enum clmac_port_mode_e{
    CLMAC_QUAD        = 0,
    CLMAC_TRI_0_1_2_2 = 1,
    CLMAC_TRI_0_0_2_3 = 2,
    CLMAC_DUAL        = 3,
    CLMAC_SINGLE      = 4
} clmac_port_mode_t;

STATIC
int _pm4x25_pm_enable(int unit, int port, pm_info_t pm_info, int port_index, const portmod_port_interface_config_t* config, int enable)
{
    uint32 reg_val, port_mode;
    uint32 rsv_mask;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    int caui_mode, three_ports_mode;
    int i, lane, rv;
    phymod_core_access_t* core_access;
    SOC_INIT_FUNC_DEFS;

    if(enable) {
        /* Power Save */
        _SOC_IF_ERR_EXIT(READ_CLPORT_POWER_SAVEr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_POWER_SAVEr(unit, port, reg_val));

        /* Port configuration */
        rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[threePortsMode], &three_ports_mode);
        _SOC_IF_ERR_EXIT(rv);

        caui_mode = (config->interface == SOC_PORT_IF_CAUI ? 1 : 0);
        if(PM4x25_QUAD_MODE_IF(config->interface)) {
            port_mode = CLMAC_QUAD;
        } else if(PM4x25_DUAL_MODE_IF(config->interface)){
            if(!three_ports_mode) {
                port_mode = CLMAC_DUAL;
            } else {
                if(port_index < 2) {
                    port_mode = CLMAC_TRI_0_0_2_3;
                } else {
                    port_mode = CLMAC_TRI_0_1_2_2;
                }
            }
        } else { /*single mode*/
            if(!three_ports_mode) {
                port_mode = CLMAC_SINGLE;
            } else {
                if(port_index < 2) {
                    port_mode = CLMAC_TRI_0_1_2_2;
                } else {
                    port_mode = CLMAC_TRI_0_0_2_3;
                }
            }
        }

        _SOC_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, FIELD_10_10f, caui_mode);
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, XPORT0_CORE_PORT_MODEf, port_mode);
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, XPORT0_PHY_PORT_MODEf,  port_mode);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_MODE_REGr(unit, port, reg_val));

        /* Bring MAC OOR */
        _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, port, reg_val));

        /* RSV Mask */
        rsv_mask = 0;
        SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
        SHR_BITSET(&rsv_mask, 4); /* CRC error */
        SHR_BITSET(&rsv_mask, 6); /* Truncated/Frame out of Range */
        SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_RSV_MASKr(unit, port, rsv_mask));
        
        /* Get Serdes OOR */
        _SOC_IF_ERR_EXIT(_pm4x25_tsc_reset(unit, port, 1));
        _SOC_IF_ERR_EXIT(_pm4x25_tsc_reset(unit, port, 0));

        /* Initialize Phys */
        for(i=0 ; i<PM_4x25_INFO(pm_info)->nof_phys ; i++) {
            core_access = &(PM_4x25_INFO(pm_info)->core_access[i]);
            _SOC_IF_ERR_EXIT(phymod_core_probe(&(core_access->access), &(core_access->type)));

            _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

            if(i==0 /*internal phy */) {
                core_conf.firmware_load_method = PM_4x25_INFO(pm_info)->fw_load_method;
                core_conf.firmware_loader = PM_4x25_INFO(pm_info)->external_fw_loader;
                core_conf.lane_map = PM_4x25_INFO(pm_info)->lane_map;
            } else {
                core_conf.firmware_load_method = 
                    (PM_4x25_INFO(pm_info)->fw_load_method == phymodFirmwareLoadMethodNone) ? phymodFirmwareLoadMethodNone : phymodFirmwareLoadMethodInternal;
                core_conf.firmware_loader = NULL;

                /* configure "no swap" for external phys. can be set later using APIs*/
                _SOC_IF_ERR_EXIT(phymod_lane_map_t_init(&core_conf.lane_map));
                core_conf.lane_map.num_of_lanes = PM4X25_LANES_PER_CORE;
                for(lane=0 ; lane<PM4X25_LANES_PER_CORE ; lane++) {
                    core_conf.lane_map.lane_map_rx[lane] = lane;
                    core_conf.lane_map.lane_map_tx[lane] = lane;
                }
            }

            _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
            _SOC_IF_ERR_EXIT(portmod_interface_to_phymod_interface(unit, config->interface, &core_conf.interface.interface_type));
            core_conf.interface.data_rate = config->speed;
            core_conf.interface.interface_modes = config->interface_modes;
            core_conf.interface.ref_clock = PM_4x25_INFO(pm_info)->ref_clk;

            _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
            core_status.pmd_active = 1;

            _SOC_IF_ERR_EXIT(phymod_core_init(core_access, &core_conf, &core_status));
        }
        

    } else { /* disable */

        /* Put Serdes in reset*/
        _SOC_IF_ERR_EXIT(_pm4x25_tsc_reset(unit, port, 1));

        /* put MAC in reset */
        _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, port, reg_val));

    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_pm_port_init(int unit, int port, int internal_port, int interface_modes, int enable)
{
    int rv;
    uint32 reg_val, flags;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};
    SOC_INIT_FUNC_DEFS;

    if(enable) {
        /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, port, reg_val));

        soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 0);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, port, reg_val));

        /* Port enable */
        _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &reg_val, port_fields[internal_port], 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, port, reg_val));

        /* Init MAC */
        flags = CLAMC_INIT_F_RX_STRIP_CRC | CLAMC_INIT_F_TX_APPEND_CRC | CLAMC_INIT_F_IPG_CHECK_DISABLE;
        if(interface_modes & PHYMOD_INTF_MODES_HIGIG) {
            flags |= CLAMC_INIT_F_IS_HIGIG;
        }
        rv = clmac_init(unit, port, flags);
        _SOC_IF_ERR_EXIT(rv);

        /* LSS */
        _SOC_IF_ERR_EXIT(READ_CLPORT_FAULT_LINK_STATUSr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &reg_val, REMOTE_FAULTf, 1);
        soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &reg_val, LOCAL_FAULTf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_FAULT_LINK_STATUSr(unit, port, reg_val)); 

        /* Counter MAX size */
        _SOC_IF_ERR_EXIT(READ_CLPORT_CNTMAXSIZEr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, 1518);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_CNTMAXSIZEr(unit, port, reg_val));
        
        /* Reset MIB counters */
        _SOC_IF_ERR_EXIT(READ_CLPORT_MIB_RESETr(unit, port, &reg_val));
        SHR_BITSET(&reg_val, internal_port);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_MIB_RESETr(unit, port, reg_val));
        SHR_BITCLR(&reg_val, internal_port); 
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_MIB_RESETr(unit, port, reg_val));
    } else {
        /* Port disable */
        _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &reg_val, port_fields[internal_port], 0);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, port, reg_val));

         /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 1);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int port_index = -1;
    int rv = 0;
    int phy, i;
    int phys_count = 0;
    uint32 pm_is_active = 0;
    uint32 pm_is_bypassed = 0;
    soc_pbmp_t port_phys_in_pm;
    SOC_INIT_FUNC_DEFS;

     rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);
    if(pm_is_bypassed){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't add port to pm when is bypassed")));
    }

    /* Get the first phy related to this port */
    SOC_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    SOC_PBMP_AND(port_phys_in_pm, PM_4x25_INFO(pm_info)->phys);
    SOC_PBMP_COUNT(port_phys_in_pm, phys_count);
    i = 0;
    SOC_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy){
        if(SOC_PBMP_MEMBER(port_phys_in_pm, phy)){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &port, i);
            _SOC_IF_ERR_EXIT(rv);
            port_index = (port_index == -1 ? i : port_index);
        }
        i++;
    }

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* if not active - initalize PM */
    if(!pm_is_active){
        /*init the PM*/
        rv = _pm4x25_pm_enable(unit, port, pm_info, port_index, &(add_info->interface_config), 1);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_active = 1;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* initalize port */
    rv = _pm4x25_pm_port_init(unit, port, port_index, add_info->interface_config.interface_modes, 1);
    _SOC_IF_ERR_EXIT(rv);
    
exit:
    SOC_FUNC_RETURN;   
}


int pm4x25_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable; 
    int invalid_port = -1;
    int tmp_port;
    int i = 0;
    int rv = 0;
    int is_last_one = TRUE;
    int port_index = -1;
    uint32 inactive = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x25_port_enable_get(unit, port, pm_info, 0, &enable));
    if(enable){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }
    
    /*remove from array and check if it was the last one*/
    for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++){
       rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &tmp_port, i);
       _SOC_IF_ERR_EXIT(rv);
       if(tmp_port == port){
           port_index = (port_index == -1 ? i : port_index);
           rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &invalid_port, i);
           _SOC_IF_ERR_EXIT(rv);
       } else if (tmp_port != -1){
           is_last_one = FALSE;
       }
   }

    if(port_index == -1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Port %d wasn't found"), port));
    }   

    rv = _pm4x25_pm_port_init(unit, port, port_index, 0, 0);
    _SOC_IF_ERR_EXIT(rv);

    /*deinit PM in case of last one*/
    if(is_last_one){
       rv = _pm4x25_pm_enable(unit, port, pm_info, port_index, NULL, 0);
       _SOC_IF_ERR_EXIT(rv);

       rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &inactive);
       _SOC_IF_ERR_EXIT(rv);
    }    
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_pm_bypass_set(int unit, pm_info_t pm_info, int bypass_enable)
{
    int rv = 0;
    uint32 pm_is_active = 0;
    SOC_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);
    if(pm_is_active){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't chenge bypass mode for active pm")));
    }

    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &bypass_enable);
    _SOC_IF_ERR_EXIT(rv);
    
exit:
    SOC_FUNC_RETURN; 
    
}

STATIC
int _pm4x25_port_index_get(int unit, int port, pm_info_t pm_info, int *first_index, uint32 *bitmap)
{
   int i;
   int rv = 0;
   int tmp_port = 0;
   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;
   for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++){
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


int pm4x25_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    uint32 is_bypassed = 0;  
    int rv = 0, phyn;
    int actual_flags = flags;
    phymod_phy_access_t phy_access;
    phymod_phy_power_t phy_power; 
    int nof_phys = 0;
    portmod_access_get_params_t params;
    SOC_INIT_FUNC_DEFS;

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

     /* if MAC is set - both RX and TX must be enabled\disabled togther*/
    if(PORTMOD_PORT_ENABLE_MAC_GET(flags)) {
        if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) || (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC RX and TX can't be enabled separately")));
        }
    }
    
    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypassed);
    _SOC_IF_ERR_EXIT(rv);
    

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = 0;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
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
            rv = clmac_enable_set(unit, port, 0, 1);
            _SOC_IF_ERR_EXIT(rv);
        }
        if((PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) && (!(PM_4x25_INFO(pm_info)->in_pm12x10))){
            /* COVERITY :
             * The variable phy_access is initialised  in  pm4x25_port_enable_set ()
             */
            /* coverity[uninit_use_in_call] */
            for(phyn = 0 ; phyn < PM_4x25_INFO(pm_info)->nof_phys ; phyn++) {
                params.phyn = phyn;
                _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys));
                _SOC_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));
            }
        }
    }
    else{
        if((PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) && (!(PM_4x25_INFO(pm_info)->in_pm12x10))){
            /* COVERITY :
             * The variable phy_access is initialised  in  pm4x25_port_enable_set ()
             */
            /* coverity[uninit_use_in_call] */
            for(phyn = 0 ; phyn < PM_4x25_INFO(pm_info)->nof_phys ; phyn++) {
                params.phyn = phyn;
                _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys));
                _SOC_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));
            }
        }  
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))  && (!is_bypassed)){
            rv = clmac_enable_set(unit, port, 0, 0);
            _SOC_IF_ERR_EXIT(rv);
        }
    }
        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    rv = clmac_enable_get(unit, port, 0, enable);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
    uint32 reg_val;
    uint32 pm_is_bypassed = 0;
    phymod_phy_inf_config_t phy_config;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys = 0;
    int rv = 0, phyn;
    SOC_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    if(!pm_is_bypassed){
        _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CLPORT_CONFIGr, &reg_val, HIGIG2_MODEf, PHYMOD_INTF_MODES_HIGIG_GET(config));
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, port, reg_val));

        rv = clmac_speed_set(unit, port, config->speed);
        _SOC_IF_ERR_EXIT(rv);

        rv = clmac_encap_set(unit, port, 0, PHYMOD_INTF_MODES_HIGIG_GET(config) ? SOC_ENCAP_HIGIG2 : SOC_ENCAP_IEEE);
        _SOC_IF_ERR_EXIT(rv);
    }

    if(!(PM_4x25_INFO(pm_info)->in_pm12x10)){
      
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.lane = -1;
        params.phyn = 0;
        params.sys_side = PORTMOD_SIDE_LINE;

        _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_config));
        _SOC_IF_ERR_EXIT(portmod_interface_to_phymod_interface(unit, config->interface, &phy_config.interface_type));
        phy_config.data_rate = config->speed;
        phy_config.interface_modes = config->interface_modes;
        phy_config.ref_clock = PM_4x25_INFO(pm_info)->ref_clk;

        for(phyn = 0 ; phyn < PM_4x25_INFO(pm_info)->nof_phys ; phyn++) {
            params.phyn = phyn;
            _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys));
            rv = phymod_phy_interface_config_set(&phy_access, config->flags, &phy_config);
            _SOC_IF_ERR_EXIT(rv);
        }
    }
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config)
{
    phymod_phy_inf_config_t phy_config;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys = 0, rv;
    SOC_INIT_FUNC_DEFS;

    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("can't get interface config for PM4X25 within PM12X10")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = 0;
    params.sys_side = PORTMOD_SIDE_LINE;
    _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys));

    rv = phymod_phy_interface_config_get(&phy_access, 0, &phy_config);
    _SOC_IF_ERR_EXIT(rv);

    config->speed = phy_config.data_rate;
    config->interface_modes = phy_config.interface_modes;
    config->flags = 0;
    rv = portmod_phymod_interface_to_interface(unit, phy_config.interface_type, &(config->interface));
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{   
    phymod_phy_access_t access;
    int nof_phys;
    phymod_loopback_mode_t phymod_lb_type;
    portmod_access_get_params_t params;
    uint32 is_enabled;
    SOC_INIT_FUNC_DEFS;

    switch(loopback_type){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(clmac_loopback_get(unit, port, loopback_type, enable));
            break;

        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPMD: /*slide*/
        case portmodLoopbackPhyGloopPCS: /*slide*/
        case portmodLoopbackPhyRloopPCS: /*slide*/
            if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
                (*enable) = 0; /* No phy --> no phy loopback*/
            } else {
                _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit, loopback_type, &phymod_lb_type));

                _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
                params.lane = -1;
                params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
                params.sys_side = PORTMOD_SIDE_LINE;
                _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));
                
                _SOC_IF_ERR_EXIT(phymod_phy_loopback_get(&access, phymod_lb_type, &is_enabled));
                (*enable) = (is_enabled ? 1 : 0);
            }
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_phy_access_t access;
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_loopback_mode_t phymod_lb_type;
    SOC_INIT_FUNC_DEFS;

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(clmac_loopback_set(unit, port, loopback_type, enable));
        break;

    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPCS: /*slide*/
    case portmodLoopbackPhyRloopPCS: /*slide*/
        if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("phy loopback isn't supported")));
        } else {
            _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit, loopback_type, &phymod_lb_type));

            _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            params.lane = -1;
            params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
            params.sys_side = PORTMOD_SIDE_LINE;
            _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));
            
            _SOC_IF_ERR_EXIT(phymod_phy_loopback_set(&access, phymod_lb_type, enable));

        }
        break;

    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }

exit:
    SOC_FUNC_RETURN; 
}


int pm4x25_port_ability_local_get(int unit, int port, pm_info_t pm_info, const portmod_port_ability_t* ability)
{
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
   
        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_autoneg_set(int unit, int port, pm_info_t pm_info, const phymod_autoneg_control_t* an)
{
    phymod_phy_access_t access;
    int nof_phys;
    portmod_access_get_params_t params;
    SOC_INIT_FUNC_DEFS;
    
    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Autoneg isn't supported")));
    } else {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.lane = -1;
        params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
        params.sys_side = PORTMOD_SIDE_LINE;
        _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));
        
        _SOC_IF_ERR_EXIT(phymod_phy_autoneg_set(&access, an));
    }
        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_autoneg_get(int unit, int port, pm_info_t pm_info, phymod_autoneg_control_t* an)
{      
    uint32 an_done;
    phymod_phy_access_t access;
    int nof_phys;
    portmod_access_get_params_t params;
    SOC_INIT_FUNC_DEFS;
    
    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Autoneg isn't supported")));
    } else {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.lane = -1;
        params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
        params.sys_side = PORTMOD_SIDE_LINE;
        _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));
        
        _SOC_IF_ERR_EXIT(phymod_phy_autoneg_get(&access, an, &an_done));
    }

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    uint32 reg_val, bitmap;
    int port_index;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    switch(port_index) {
        case 0:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN0_STATUS0_REGr(unit, port, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN0_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;
        case 1:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN1_STATUS0_REGr(unit, port, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN1_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;
        case 2:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN2_STATUS0_REGr(unit, port, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN2_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;
        case 3:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN3_STATUS0_REGr(unit, port, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN3_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("Port %d, failed to get port index"), port));
    }
        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_prbs_config_set(int unit, int port, pm_info_t pm_info, int mode, int flags, const phymod_prbs_t* config)
{
    portmod_access_get_params_t params; 
    phymod_phy_access_t access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
    params.sys_side = PORTMOD_SIDE_LINE;
    _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_set(&access, flags, config));

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_prbs_config_get(int unit, int port, pm_info_t pm_info, int mode, int flags, phymod_prbs_t* config)
{
    portmod_access_get_params_t params; 
    phymod_phy_access_t access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
    params.sys_side = PORTMOD_SIDE_LINE;
    _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_prbs_config_get(&access, flags, config));
      
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, int mode, int flags, int enable)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t access;
    int nof_phys; 
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
    params.sys_side = PORTMOD_SIDE_LINE;
    _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_set(&access, flags, enable));

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, int mode, int flags, int* enable)
{ 
    portmod_access_get_params_t params; 
    uint32 is_enabled;
    phymod_phy_access_t access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
    params.sys_side = PORTMOD_SIDE_LINE;
    _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_prbs_enable_get(&access, flags, &is_enabled));
    (*enable) = (is_enabled ? 1 : 0);

exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_prbs_status_get(int unit, int port, pm_info_t pm_info, int mode, int flags, phymod_prbs_status_t* status)
{
    portmod_access_get_params_t params; 
    phymod_phy_access_t access;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* check outer phy */
    params.sys_side = PORTMOD_SIDE_LINE;
    _SOC_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &access, &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_prbs_status_get(&access, flags, status));

exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_firmware_mode_set(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_firmware_mode_get(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t* fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_runt_threshold_set(int unit, int port, pm_info_t pm_info, int value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_runt_threshold_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_max_packet_size_set(int unit, int port, pm_info_t pm_info, int value)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_rx_max_size_set(unit, port , value));      
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_max_packet_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_rx_max_size_get(unit, port , value));
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_tx_drop_on_local_fault_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(clmac_local_fault_control_get(unit, port, &control));
    control.drop_tx_on_fault  = enable;
    _SOC_IF_ERR_EXIT(clmac_local_fault_control_set(unit, port, &control));     
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_tx_drop_on_local_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{       
    portmod_local_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(clmac_local_fault_control_get(unit, port, &control));
    *enable = control.drop_tx_on_fault;
exit:
    SOC_FUNC_RETURN; 
}


int pm4x25_port_tx_drop_on_remote_fault_set(int unit, int port, pm_info_t pm_info, int enable)
{        
    portmod_remote_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_get(unit, port, &control));
    control.drop_tx_on_fault = enable;
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_set(unit, port, &control));
exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_tx_drop_on_remote_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_remote_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_get(unit, port, &control));
    *enable = control.drop_tx_on_fault;
exit:
    SOC_FUNC_RETURN;   
}


int pm4x25_port_local_fault_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    
    _SOC_IF_ERR_EXIT(clmac_local_fault_control_get(unit, port , &control));
    control.enable = enable;
    _SOC_IF_ERR_EXIT(clmac_local_fault_control_set(unit, port , &control));       
exit:
    SOC_FUNC_RETURN;     
}


int pm4x25_port_local_fault_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    
    _SOC_IF_ERR_EXIT(clmac_local_fault_control_get(unit, port , &control));
    *enable = control.enable;
exit:
    SOC_FUNC_RETURN;      
}


int pm4x25_port_remote_fault_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_remote_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);
    
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_get(unit, port , &control));
    control.enable = enable;
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_set(unit, port , &control));       
exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_remote_fault_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_remote_fault_control_t control;   
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_get(unit, port , &control));
    *enable = control.enable;    
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_local_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv;
    SOC_INIT_FUNC_DEFS;
    
    rv = clmac_local_fault_status_get(unit, port, 0, value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}


int pm4x25_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv;        
    SOC_INIT_FUNC_DEFS;
    
    rv = clmac_remote_fault_status_get(unit, port, 0, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_pause_control_set(int unit, int port, pm_info_t pm_info, const portmod_pause_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pause_control_set(unit, port, control)); 
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_pause_control_get(int unit, int port, pm_info_t pm_info, portmod_pause_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pause_control_get(unit, port, control));      
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_pfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_pfc_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pfc_control_set(unit, port, control));        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_pfc_control_get(int unit, int port, pm_info_t pm_info, portmod_pfc_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pfc_control_get(unit, port, control));     
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_llfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_llfc_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_llfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_llfc_control_get(int unit, int port, pm_info_t pm_info, portmod_llfc_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_llfc_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* phy_access, int* nof_phys)
{   
    int phyn = 0;
    int port_index;
    SOC_INIT_FUNC_DEFS;
    
    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("phy access get cannot be called for PM4X25 which is part of PM12X10")));
    }

    if(params->phyn >= PM_4x25_INFO(pm_info)->nof_phys){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), PM_4x25_INFO(pm_info)->nof_phys-1, params->phyn));
    }

    phyn = params->phyn;
    if(phyn < 0) {
        phyn = PM_4x25_INFO(pm_info)->nof_phys - 1;
    }
    sal_memcpy(&phy_access[0], &(PM_4x25_INFO(pm_info)->core_access[phyn]), sizeof(PM_4x25_INFO(pm_info)->core_access[phyn]));

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index, &(phy_access[0].access.lane_mask)));

    *nof_phys = 1;       

exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores)
{
        
    SOC_INIT_FUNC_DEFS;
    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("core access get cannot be called for PM4X25 which is part of PM12X10")));
    }
    if(phyn >= PM_4x25_INFO(pm_info)->nof_phys){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), PM_4x25_INFO(pm_info)->nof_phys-1, phyn));
    }
    if(phyn < 0) {
        phyn = PM_4x25_INFO(pm_info)->nof_phys - 1; /* last one */
    }
    sal_memcpy(&core_access_arr[0], &(PM_4x25_INFO(pm_info)->core_access[phyn]), sizeof(PM_4x25_INFO(pm_info)->core_access[phyn]));
    *nof_cores = 1;      
     
exit:
    SOC_FUNC_RETURN; 
}


#endif /* PORTMOD_PM4X25_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
