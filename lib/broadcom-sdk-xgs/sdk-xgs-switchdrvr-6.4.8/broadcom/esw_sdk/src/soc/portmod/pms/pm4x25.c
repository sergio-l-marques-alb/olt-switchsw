/*
 *
 * $Id:$
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

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>
#include <soc/portmod/portmod_legacy_phy.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X25_SUPPORT

#include <soc/portmod/clmac.h>
#include <soc/portmod/pm4x25.h>
#include <soc/portmod/pm4x25_shared.h>

#ifdef PORTMOD_PM4X25TD_SUPPORT
#include <soc/portmod/pm4x25td_int.h>
#endif /* PORTMOD_PM4X25TD_SUPPORT */


#define PM_4x25_INFO(pm_info) ((pm_info)->pm_data.pm4x25_db)

/* WB defines */
#define PM4x25_IS_ACTIVE_SET(unit, pm_info, is_active) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &is_active)
#define PM4x25_IS_ACTIVE_GET(unit, pm_info, is_active) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], is_active)

#define PM4x25_IS_INITIALIZED_SET(unit, pm_info, is_initialized) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], &is_initialized)
#define PM4x25_IS_INITIALIZED_GET(unit, pm_info, is_initialized) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], is_initialized)

#define PM4x25_IS_BYPASSED_SET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypass)
#define PM4x25_IS_BYPASSED_GET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], is_bypass)

#define PM4x25_LANE2PORT_SET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[lane2portMap], &port, lane)
#define PM4x25_LANE2PORT_GET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[lane2portMap], port, lane)

#define PM4x25_THREE_PORTS_MODE_SET(unit, pm_info, three_ports_mode) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[threePortsMode], &three_ports_mode)
#define PM4x25_THREE_PORTS_MODE_GET(unit, pm_info, three_ports_mode) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[threePortsMode], three_ports_mode);


typedef enum pm4x25_fabric_wb_vars{
    isActive = 0,
    isBypassed, 
    isInitialized,
    lane2portMap,
    threePortsMode,
    phy_type
}pm4x25_wb_vars_t;

#define PM4x25_QUAD_MODE_IF(interface) \
    (interface == SOC_PORT_IF_CAUI || interface == SOC_PORT_IF_XLAUI)

#define PM4x25_DUAL_MODE_IF(interface) \
    (interface == SOC_PORT_IF_XLAUI2)

#define PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc) \
    do { \
        uint32 is_bypass; \
        phy_acc = 0; \
        PM4x25_IS_BYPASSED_GET(unit, pm_info, &is_bypass); \
        phy_acc = (is_bypass && PM_4x25_INFO(pm_info)->first_phy != -1) ? (PM_4x25_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK) : port ; \
    } while (0)

STATIC
int pm4x25_port_soft_reset(int unit, int port, pm_info_t pm_info, int enable);


int pm4x25_pm_interface_type_is_supported(int unit, soc_port_if_t interface,
                                          int* is_supported)
{
    SOC_INIT_FUNC_DEFS;

    switch(interface){
        case SOC_PORT_IF_CAUI:
        case SOC_PORT_IF_XLAUI2:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SFI:
        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_SGMII:
            *is_supported = TRUE;
            break;
        default:
            *is_supported = FALSE;
            break;
    }
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++){
       rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
       _SOC_IF_ERR_EXIT(rv);

       if(tmp_port == port){
           *first_index = (*first_index == -1 ? i : *first_index);
           SHR_BITSET(bitmap, i);
       }
   }

   if(*first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("port was not found in internal DB %d"), port));
   }

exit:
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
int pm4x25_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm4x25", NULL, VERSION(2), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2portMap", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM4X25, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2portMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_bypassed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isBypassed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "three_ports_mode", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[threePortsMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_type", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM4X25, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phy_type] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));
    if(!SOC_WARM_BOOT(unit)){
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[lane2portMap], -1);
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
    portmod_ucode_buf_order_t load_order;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    if (PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_GET(user_data)) {
        load_order = portmod_ucode_buf_order_reversed;
    } else {
        load_order = portmod_ucode_buf_order_straight;
    }

    rv = portmod_firmware_set(unit,
                              user_data->blk_id,
                              data,
                              length,
                              load_order,
                              &(pm4x25_ucode_buf[unit]),
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

STATIC
int _pm4x25_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx, int val)
{
    int phy_acc;
    uint32 reg_val;

    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));

    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT0f, val);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT1f, val);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT2f, val);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT3f, val);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN; 
}
int pm4x25_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{
    int phy_acc;
    uint32 reg_val, old_val;

    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
    old_val = reg_val;

    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT0f, 1);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT1f, 1);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT2f, 1);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT3f, 1);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, old_val));

exit:
    SOC_FUNC_RETURN; 
}

phymod_bus_t pm4x25_default_bus = {
    "PM4x25 Bus",
    pm4x25_default_bus_read,
    pm4x25_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

int pm4x25_pm_init(int unit,
                   const portmod_pm_create_info_internal_t* pm_add_info,
                   int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_pm4x25_create_info_t *info = &pm_add_info->pm_specific_info.pm4x25;
    pm4x25_t pm4x25_data = NULL;
    int i,j, pm_is_active, rv, three_ports_mode;
    int bypass_enable;
    int pm_is_initialized;
    SOC_INIT_FUNC_DEFS;

    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    /* PM4x25 specific info */
    pm4x25_data = sal_alloc(sizeof(struct pm4x25_s), "specific_db");
    SOC_NULL_CHECK(pm4x25_data);
    pm_info->pm_data.pm4x25_db = pm4x25_data;

    SOC_PBMP_ASSIGN(pm4x25_data->phys, pm_add_info->phys);
    pm4x25_data->int_core_access.type = phymodDispatchTypeCount;
    pm4x25_data->portmod_mac_soft_reset = pm_add_info->pm_specific_info.pm4x25.portmod_mac_soft_reset;
	pm4x25_data->first_phy = -1;
    
    for(i = 0 ; i < PM4X25_LANES_PER_CORE; i++){
        pm4x25_data->nof_phys[i] = 0;
    }

    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm4x25_data->int_core_access);

    /* initialize lane connections */
    for(i = 0 ; i < MAX_PHYN ; i++){
        for(j = 0 ; j < PM4X25_LANES_PER_CORE ; j++){
            portmod_xphy_lane_connection_t_init(unit, &pm4x25_data->lane_conn[i][j]);
        }
    }

    if(!info->in_pm_12x10){
        if((info->ref_clk != phymodRefClk156Mhz) && (info->ref_clk != phymodRefClk125Mhz)){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("illegal ref clock")));
        }
        if(info->lane_map.num_of_lanes != PM4X25_LANES_PER_CORE){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                         (_SOC_MSG("lanes map length should be 4")));
        }
        _SOC_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(
                                             info->fw_load_method));

        sal_memcpy(&pm4x25_data->polarity, &info->polarity, sizeof(phymod_polarity_t));
        sal_memcpy(&(pm4x25_data->int_core_access.access), &info->access,
                    sizeof(phymod_access_t));

       sal_memcpy(&pm4x25_data->lane_map, &info->lane_map,
                    sizeof(pm4x25_data->lane_map));
        pm4x25_data->ref_clk = info->ref_clk;
        pm4x25_data->fw_load_method = info->fw_load_method;
        pm4x25_data->external_fw_loader = info->external_fw_loader;

        if(info->access.bus == NULL) {
            /* if null - use default */
            pm4x25_data->int_core_access.access.bus = &pm4x25_default_bus;
        }

        if(pm4x25_data->external_fw_loader == NULL) {
            /* if null - use default */
            pm4x25_data->external_fw_loader = pm4x25_default_fw_loader;
        }

        for(i = 0 ; i < PM4X25_LANES_PER_CORE; i++){
            pm4x25_data->nof_phys[i] = 1; /* Internal Phy */
        }
    }
    pm4x25_data->in_pm12x10 = info->in_pm_12x10;
    pm4x25_data->core_num   = info->core_num;
    pm4x25_data->afe_pll = info->afe_pll;
    
    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm4x25_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if(SOC_WARM_BOOT(unit)){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[phy_type], &pm4x25_data->int_core_access.type, 0);
        _SOC_IF_ERR_EXIT(rv);
    }

    if(!SOC_WARM_BOOT(unit)){
        pm_is_active = 0;
        rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_initialized = 0;
        rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, pm_is_initialized);
        _SOC_IF_ERR_EXIT(rv);

        bypass_enable = 0;
        rv = PM4x25_IS_BYPASSED_SET(unit, pm_info, bypass_enable);
        _SOC_IF_ERR_EXIT(rv);

        three_ports_mode = 0;
        rv = PM4x25_THREE_PORTS_MODE_SET(unit, pm_info, three_ports_mode);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    if(SOC_FUNC_ERROR){
        pm4x25_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

int pm4x25_port_tsc_reset_set(int unit, int phy_acc, int in_reset)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* Bring Internal Phy OOR */
    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));

    sal_usleep(1100);

exit:
    SOC_FUNC_RETURN;
}

static int _clport_mode_get(int unit, int phy_acc, int bindex, 
                      portmod_core_port_mode_t *core_mode, int *cur_lanes)
{
    uint32_t rval;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, phy_acc, &rval));

    *core_mode = soc_reg_field_get(unit, CLPORT_MODE_REGr, rval, XPORT0_CORE_PORT_MODEf);

    switch (*core_mode) {
        case portmodPortModeQuad:
            *cur_lanes = 1;
            break;

        case portmodPortModeTri012: 
            *cur_lanes = bindex == 2 ? 2 : 1;
            break;

        case portmodPortModeTri023:
            *cur_lanes = bindex == 0 ? 2 : 1;
            break;

        case portmodPortModeDual: 
            *cur_lanes = 2;
            break;

        case portmodPortModeSingle:
            *cur_lanes = 4;
            break;

        default:
            return SOC_E_FAIL;
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CNTMAXSIZEr(unit, phy_acc, (uint32_t *)val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CNTMAXSIZEr(unit, phy_acc, &reg_val));

    soc_reg_field_set(unit, CLPORT_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, val);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CNTMAXSIZEr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_modid_set (int unit, int port, pm_info_t pm_info, int value)
{
    uint32_t rval, modid;
    int      flen, phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    flen = soc_reg_field_length(unit, CLPORT_CONFIGr, MY_MODIDf)? value : 0;
    modid =  (value < (1 <<  flen))? value : 0;

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, MY_MODIDf, modid);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_higig2_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, HIGIG2_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_higig2_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    *(mode) = soc_reg_field_get(unit, CLPORT_CONFIGr, rval, HIGIG2_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_higig_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, HIGIG_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, phy_acc, rval));
   
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_higig_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    *(mode) = soc_reg_field_get(unit, CLPORT_CONFIGr, rval, HIGIG_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_encap_set (int unit, int port, pm_info_t pm_info, 
                                int flags, portmod_encap_t encap)
{
    return (clmac_encap_set(unit, port, flags, encap));

}

int pm4x25_port_encap_get (int unit, int port, pm_info_t pm_info, 
                            int *flags, portmod_encap_t *encap)
{
    return (clmac_encap_get(unit, port, flags, encap));
}

int pm4x25_port_config_port_type_set (int unit, int port, pm_info_t pm_info, int type)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, PORT_TYPEf, type);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_config_port_type_get (int unit, int port, pm_info_t pm_info, int *type)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    *(type) = soc_reg_field_get(unit, CLPORT_CONFIGr, rval, PORT_TYPEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_led_chain_config (int unit, int port, pm_info_t pm, int value)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm, phy_acc);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_LED_CHAIN_CONFIGr (unit, phy_acc, value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
                                    const portmod_access_get_params_t* params,
                                    int max_phys,
                                    phymod_phy_access_t* phy_access,
                                    int* nof_phys,
                                    int* is_most_ext)
{
    int phyn = 0, rv;
    uint32  iphy_lane_mask;
    int port_index=0;
    int serdes_lane=0;
    int xphy_lane=0;
    uint32 xphy_id = 0;
    portmod_xphy_core_info_t xphy_core_info;
    uint32 lane_mask[MAX_NUM_CORES];
    phymod_port_loc_t port_loc[MAX_NUM_CORES];
    int i;
    uint32 xphy_idx;
    int num_of_phys;
    int done;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( max_phys > MAX_NUM_CORES);
    
    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("phy access get cannot be called for PM4X25 which is part of PM12X10")));
    }

    /* get port index and lane mask */
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index,
                                            &iphy_lane_mask));


    if(params->phyn >= PM_4x25_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
               PM_4x25_INFO(pm_info)->nof_phys[port_index] - 1, params->phyn));
    }

    for( i = 0 ; i < max_phys; i++) {
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    phyn = params->phyn;

    /* if phyn is -1, it is looking for outer most phy.
       assumption is that all lane has same phy depth. */
    if(phyn < 0) {
        phyn = PM_4x25_INFO(pm_info)->nof_phys[port_index] - 1;
    }


    if( phyn == 0 ) {
        /* internal core */
        sal_memcpy (&phy_access[0], &(PM_4x25_INFO(pm_info)->int_core_access),
                    sizeof(phymod_phy_access_t));
        phy_access[0].access.lane_mask = iphy_lane_mask;
        if (params->lane != -1) {
            serdes_lane = port_index + params->lane;
            phy_access[0].access.lane_mask &= (0x1 << serdes_lane );
        }
        phy_access[0].port_loc = phymodPortLocLine; /* only line is availabe for internal. */
        /* if it is warm boot, get probed information from wb db instead of re-probing. */
        if(SOC_WARM_BOOT(unit)) {
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phy_type], &(phy_access[0].type), 0);
            _SOC_IF_ERR_EXIT(rv);
        }
        *nof_phys = 1;
    } else {

        /* external phy */
        num_of_phys = 0;

        /* xphy idx is 1 less than phyn since internal core does not count */    
        xphy_idx = phyn-1;
    
        if (params->lane != -1){ /* specific lane */
            serdes_lane = port_index + params->lane;
            xphy_id = PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;
            _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
            sal_memcpy (&phy_access[num_of_phys], &xphy_core_info.core_access,
                        sizeof(phymod_phy_access_t));
            xphy_lane = ( params->sys_side == PORTMOD_SIDE_SYSTEM)? PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane:
                          PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane;
            if( xphy_lane != -1 ){
                phy_access[0].access.lane_mask |= 0x1U <<xphy_lane;
            }
            phy_access[0].port_loc = ( params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                       phymodPortLocSys : phymodPortLocLine;
            *nof_phys = 1;
        } else { /* all lanes */
            /* go thru all the lanes related to this port. */
            serdes_lane = 0;

            for (serdes_lane = 0; serdes_lane < MAX_PORTS_PER_PM4X25; serdes_lane++) 
                if (iphy_lane_mask & (1U<<serdes_lane)) {
                xphy_id = PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;
                if( (xphy_id != PORTMOD_XPHY_ID_INVALID) && ( xphy_id < PORTMOD_MAX_NUM_XPHY_SUPPORTED)){
                    _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
                    if(num_of_phys == 0 ) {/* first one */
                        sal_memcpy (&phy_access[num_of_phys], &xphy_core_info.core_access,
                                    sizeof(phymod_phy_access_t));
                        xphy_lane = ( params->sys_side == PORTMOD_SIDE_SYSTEM)? 
                                    PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane:
                                    PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane;
                        if( xphy_lane != -1) {
                            lane_mask[num_of_phys] |= (0x1U <<xphy_lane);
                        }
                        port_loc[num_of_phys] = (params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                                  phymodPortLocSys : phymodPortLocLine;
                        
                        num_of_phys++;
                    } else {
                        /* if not the first one, see if this one sharing core with other 
                           lanes.  */
                        done = 0;
                        for( i = 0 ; (i < num_of_phys) &&(!done) ; i++) {
                             if((!sal_memcmp (&phy_access[i], &xphy_core_info.core_access,
                                               sizeof(phymod_phy_access_t)))) { /* found a match */
                                /* update lane */
                                xphy_lane = ( params->sys_side == PORTMOD_SIDE_SYSTEM)? 
                                            PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane:
                                            PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane;
                                if( xphy_lane != -1) {
                                    lane_mask[num_of_phys] |= (0x1U <<xphy_lane);
                                }
                                done = 1;
                            }
                        }
                        if((!done) && (num_of_phys < max_phys)){
                            sal_memcpy (&phy_access[num_of_phys], &xphy_core_info.core_access,
                            sizeof(phymod_phy_access_t));
                            xphy_lane = ( params->sys_side == PORTMOD_SIDE_SYSTEM)? 
                                        PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane:
                                        PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane;
                            if( xphy_lane != -1) {
                                lane_mask[num_of_phys] |= (0x1U <<xphy_lane);
                            }
                            port_loc[num_of_phys] = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                                                phymodPortLocSys : phymodPortLocLine ;
                            num_of_phys++;
                        } 
                    }
                }
            }
            *nof_phys = num_of_phys;
            for( i = 0 ; (i < *nof_phys) ; i++) {
                phy_access[i].access.lane_mask = lane_mask[i];
                phy_access[i].port_loc = port_loc[i];
            }
        }
    } /* ext phys */ 
       
    /* assumption is that all logical port have same phy depths, that 
       will not true when simplex introduced , until then we can keep 
       this code. */
    if (is_most_ext) {
        if (phyn == PM_4x25_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }
exit:
    SOC_FUNC_RETURN;

}

typedef enum clmac_port_mode_e{
    CLMAC_4_LANES_SEPARATE  = 0,
    CLMAC_3_TRI_0_1_2_2     = 1,
    CLMAC_3_TRI_0_0_2_3     = 2,
    CLMAC_2_LANES_DUAL      = 3,
    CLMAC_4_LANES_TOGETHER  = 4
} clmac_port_mode_t;

STATIC
int _pm4x25_core_probe ( int unit, int port, pm_info_t pm_info, phymod_core_access_t* core_access, int *probe, int is_probe_for_legacy )
{
    int phy_addr = 0xFF;
    SOC_INIT_FUNC_DEFS;
    *probe = 0;

    /* if not probed yet, probe it. */
    if (is_probe_for_legacy) {

        if (portmod_port_legacy_phy_probe(unit, port)) {
            PORTMOD_IS_LEGACY_PHY_SET(&core_access->access);
            /* Update the phy address in core access struct -- used by phy info */
            phy_addr = portmod_port_legacy_phy_addr_get(unit, port);
            core_access->access.addr = phy_addr;
            *probe = 1;
        } else {
            if(core_access->type == phymodDispatchTypeCount ){
                _SOC_IF_ERR_EXIT(phymod_core_probe( &(core_access->access), &(core_access->type)));
                *probe = 1;
            }
        }
    } else if(core_access->type == phymodDispatchTypeCount ){
        _SOC_IF_ERR_EXIT(phymod_core_probe( &(core_access->access), &(core_access->type)));
        *probe = 1;
    }
exit:
    SOC_FUNC_RETURN;

}

STATIC
int _pm4x25_pm_phys_enable (int unit, int port, pm_info_t pm_info, int port_index, 
                             const portmod_port_add_info_t* add_info, int enable)
{   
    int     rv;
    int    temp, phy;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    const portmod_port_interface_config_t* config;
    soc_pbmp_t port_phys_in_pm;
    int probe =0;
    uint32 xphy_id;
    uint32 nof_ext_phys =0;
    int i;
    portmod_xphy_core_info_t xphy_core_info;
    uint32 core_is_initialized;
    int lane; 
    SOC_INIT_FUNC_DEFS;


    config = &(add_info->interface_config);

    if(enable) {

        /* probe phys (both internal and external cores. */

        /* Get the first phy related to this port */
        SOC_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
        SOC_PBMP_AND(port_phys_in_pm, PM_4x25_INFO(pm_info)->phys);


        /* if internal core is not probe, probe it. Currently checking to type to see
           phymodDispatchTypeCount, later need to move to WB.  */
        _SOC_IF_ERR_EXIT(_pm4x25_core_probe(unit, port, pm_info, &PM_4x25_INFO(pm_info)->int_core_access,&probe, 0));

        /* save probed phy type to wb. */
        if(probe) {
              rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[phy_type], 
                         &PM_4x25_INFO(pm_info)->int_core_access.type, 0);
              _SOC_IF_ERR_EXIT(rv);
        }
        
        /* probe ext core related to this logical port. */
        temp = 0;
        SOC_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy) {
            if(SOC_PBMP_MEMBER(port_phys_in_pm, phy)) {
                if( PM_4x25_INFO(pm_info)->nof_phys[temp] >= 2 ){
                    nof_ext_phys = PM_4x25_INFO(pm_info)->nof_phys[temp]-1;
                    for(i=0 ; i<nof_ext_phys ; i++) {
                        xphy_id = PM_4x25_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                        if( (xphy_id != PORTMOD_XPHY_ID_INVALID) && ( xphy_id < PORTMOD_MAX_NUM_XPHY_SUPPORTED)){
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
                            _SOC_IF_ERR_EXIT(_pm4x25_core_probe(unit,port, pm_info, &xphy_core_info.core_access, &probe, 1));
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_info_set(unit, xphy_id, &xphy_core_info));
                        }
                    }
                }
            }
            temp++;
        }

        /* core init (both internal and external cores. */

        _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

        /* set internal setting. */
        core_conf.firmware_load_method = PM_4x25_INFO(pm_info)->fw_load_method;
        core_conf.firmware_loader = PM_4x25_INFO(pm_info)->external_fw_loader;
        core_conf.lane_map = PM_4x25_INFO(pm_info)->lane_map;

        _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                   config->interface, &core_conf.interface.interface_type));
        core_conf.interface.data_rate = config->speed;
        core_conf.interface.interface_modes = config->interface_modes;
        core_conf.interface.ref_clock = PM_4x25_INFO(pm_info)->ref_clk;

        _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
        core_status.pmd_active = 0;

        if(PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
            PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_conf);
        } else {
            PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_CLR(&core_conf);
        }

        if(PORTMOD_PORT_ADD_F_INIT_UNTIL_FW_LOAD_GET(add_info)) {
            PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_SET(&core_conf);
        }

        if(PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(add_info)) {
            PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(&core_conf);
        }

        core_conf.op_datapath = add_info->phy_op_datapath;

        /* deal witn internal serdes first */
        rv = PM4x25_IS_INITIALIZED_GET(unit, pm_info, &core_is_initialized);
        _SOC_IF_ERR_EXIT(rv);

        if(!core_is_initialized){

            _SOC_IF_ERR_EXIT(phymod_core_init(&PM_4x25_INFO(pm_info)->int_core_access,
                                              &core_conf,
                                              &core_status));
            core_is_initialized = 1;
            rv = PM4x25_IS_INITIALIZED_SET(unit, pm_info, core_is_initialized);
            _SOC_IF_ERR_EXIT(rv);

        }
        /* deal with ext phys now ... */

        /* adjust config setup for external phys. */
    
        /* for external phys, FW load method is default to Internal
           unless it is force to NONE. */
        
        
        core_conf.firmware_load_method = (PM_4x25_INFO(pm_info)->fw_load_method ==
                                            phymodFirmwareLoadMethodNone) ?
                                            phymodFirmwareLoadMethodNone :
                                            phymodFirmwareLoadMethodInternal;
        core_conf.firmware_loader = NULL;

        /* config "no swap" for external phys.set later using APIs*/
        for(lane=0 ; lane < PM4X25_LANES_PER_CORE; lane++) {
            core_conf.lane_map.lane_map_rx[lane] = lane;
            core_conf.lane_map.lane_map_tx[lane] = lane;
        }


        temp = 0;
        SOC_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy) {
            if(SOC_PBMP_MEMBER(port_phys_in_pm, phy)) {
                if( PM_4x25_INFO(pm_info)->nof_phys[temp] >= 2 ){
                    nof_ext_phys = PM_4x25_INFO(pm_info)->nof_phys[temp]-1;
                    for(i=0 ; i<nof_ext_phys ; i++) {
                        xphy_id = PM_4x25_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                        if( (xphy_id != PORTMOD_XPHY_ID_INVALID) && ( xphy_id < PORTMOD_MAX_NUM_XPHY_SUPPORTED)){
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
                            if( !xphy_core_info.is_initialized ){

                                _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
                                core_status.pmd_active = 0;

                                _SOC_IF_ERR_EXIT(phymod_core_init(&xphy_core_info.core_access,
                                                 &core_conf,
                                                 &core_status));
                                xphy_core_info.is_initialized = 1; 
                                _SOC_IF_ERR_EXIT(portmod_xphy_core_info_set(unit, xphy_id, &xphy_core_info));
                            }
                        }
                    }
                }
            }
            temp++;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_pm_enable(int unit, int port, pm_info_t pm_info, int port_index, const portmod_port_add_info_t* add_info, int enable)
{
    uint32 reg_val, port_mode;
    int caui_mode, three_ports_mode;
    int  rv, phy_acc;

    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

#ifdef PORTMOD_PM4X25TD_SUPPORT
    if (pm_info->type == portmodDispatchTypePm4x25td) {
        rv = _pm4x25td_pm_enable(unit, port, pm_info, port_index, add_info, enable);
        _SOC_IF_ERR_EXIT(rv);
    } else 
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    {

        if(enable) {
            /* Power Save */
            _SOC_IF_ERR_EXIT(READ_CLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 0);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_POWER_SAVEr(unit, phy_acc, reg_val));

            /* Port configuration */
            rv = PM4x25_THREE_PORTS_MODE_GET(unit, pm_info, &three_ports_mode);
            _SOC_IF_ERR_EXIT(rv);

            caui_mode = (add_info->interface_config.interface == SOC_PORT_IF_CAUI ? 1 : 0);
            /* if(PM4x25_QUAD_MODE_IF(add_info->interface_config.interface)) */
            if(add_info->interface_config.port_num_lanes == 4) {
                port_mode = CLMAC_4_LANES_TOGETHER;
            /*} else if(PM4x25_DUAL_MODE_IF(add_info->interface_config.interface)){ */
            } else if(add_info->interface_config.port_num_lanes == 2){
                if(!three_ports_mode) {
                    port_mode = CLMAC_2_LANES_DUAL;
                } else {
                    if(port_index < 2) {
                        port_mode = CLMAC_3_TRI_0_0_2_3;
                    } else {
                        port_mode = CLMAC_3_TRI_0_1_2_2;
                    }
                }
            } else { /*single mode*/
                if(!three_ports_mode) {
                    port_mode = CLMAC_4_LANES_SEPARATE;
                } else {
                    if(port_index < 2) {
                        port_mode = CLMAC_3_TRI_0_1_2_2;
                    } else {
                        port_mode = CLMAC_3_TRI_0_0_2_3;
                    }
                }
            }

            _SOC_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, FIELD_10_10f, caui_mode);
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, XPORT0_CORE_PORT_MODEf, port_mode);
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, XPORT0_PHY_PORT_MODEf,  port_mode);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MODE_REGr(unit, phy_acc, reg_val));

            /* Bring MAC OOR */
            _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 0);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
            
            /* Get Serdes OOR */
            _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 1));
            _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 0));

            if (PM_4x25_INFO(pm_info)->nof_phys[0] > 0) { 
                /* Get Serdes OOR */
                _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 1));
                _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 0));
            }
        } else { /* disable */

            /* Put Serdes in reset*/
            _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 1));

            /* put MAC in reset */
            _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));

            /* Turn on Power Save */
            _SOC_IF_ERR_EXIT(READ_CLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_POWER_SAVEr(unit, phy_acc, reg_val));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_pm_port_init(int unit, int port, pm_info_t pm_info, int internal_port,
                         const portmod_port_add_info_t* add_info, int enable)
{
    int rv, phy_acc;
    uint32 reg_val, flags;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};
    uint32 rsv_mask;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

#ifdef PORTMOD_PM4X25TD_SUPPORT
    if (pm_info->type == portmodDispatchTypePm4x25td) {
        rv = _pm4x25td_pm_port_init(unit, port, internal_port, add_info, enable);
        _SOC_IF_ERR_EXIT(rv);
    } else 
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    {
        if (internal_port == -1)  {
            _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Invalid internal Port %d"), 
                                    internal_port));
        }   
        if(enable) {

            /* RSV Mask */
            rsv_mask = 0;
            SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
            SHR_BITSET(&rsv_mask, 4); /* CRC error */
            SHR_BITSET(&rsv_mask, 6); /* Truncated/Frame out of Range */
            SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_RSV_MASKr(unit, phy_acc, rsv_mask));

            /* Soft reset */
            _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

            soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 0);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

            /* Port enable */
            _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &reg_val, port_fields[internal_port], 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

            /* Init MAC */
            flags = 0;
            if(PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
                flags |= CLMAC_INIT_F_RX_STRIP_CRC;
            }

            if(PORTMOD_PORT_ADD_F_TX_APPEND_CRC_GET(add_info)) {
                flags |= CLMAC_INIT_F_TX_APPEND_CRC;
            }

            if(PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_GET(add_info)) {
                flags |= CLMAC_INIT_F_TX_REPLACE_CRC;
            }

            if(PORTMOD_PORT_ADD_F_TX_PASS_THROUGH_CRC_GET(add_info)) {
                flags |= CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE;
            }

            flags |= CLMAC_INIT_F_IPG_CHECK_DISABLE;

            if(PHYMOD_INTF_MODES_HIGIG_GET(&(add_info->interface_config))) {
                flags |= CLMAC_INIT_F_IS_HIGIG;
            }

            rv = clmac_init(unit, port, flags);
            _SOC_IF_ERR_EXIT(rv);

            /* LSS */
            _SOC_IF_ERR_EXIT(READ_CLPORT_FAULT_LINK_STATUSr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &reg_val, REMOTE_FAULTf, 1);
            soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &reg_val, LOCAL_FAULTf, 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_FAULT_LINK_STATUSr(unit, phy_acc, reg_val)); 

            /* Counter MAX size */ 
            _SOC_IF_ERR_EXIT(READ_CLPORT_CNTMAXSIZEr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, 1518);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_CNTMAXSIZEr(unit, phy_acc, reg_val));
            
            /* Reset MIB counters */ 
            _SOC_IF_ERR_EXIT(READ_CLPORT_MIB_RESETr(unit, phy_acc, &reg_val));
            /* coverity[ptr_arith:FALSE] */
            SHR_BITSET(&reg_val, internal_port);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MIB_RESETr(unit, phy_acc, reg_val));
            SHR_BITCLR(&reg_val, internal_port);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MIB_RESETr(unit, phy_acc, reg_val));
        } else {
            /* Port disable */
            _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &reg_val, port_fields[internal_port], 0);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

             /* Soft reset */
            _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int port_index = -1;
    int rv = 0, port_i, my_i;
    int phy, ii, nof_phys;
    int phys_count = 0, first_phy = -1;
    uint32 pm_is_active = 0;
    uint32 pm_is_bypassed = 0;
    soc_pbmp_t port_phys_in_pm;
    phymod_phy_init_config_t init_config;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_access_t phy_access_lane;
    portmod_port_ability_t port_ability;

    SOC_INIT_FUNC_DEFS;

    /* Coverity fix */
    pm_is_bypassed = pm_is_bypassed;    
    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);
    
    /* Get the first phy related to this port */
    if (PM_4x25_INFO(pm_info)->in_pm12x10) {
        port_index = 0;
        SOC_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, PM_4x25_INFO(pm_info)->first_phy){break;} /*just get first phy*/
    } else {
        SOC_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
        SOC_PBMP_AND(port_phys_in_pm, PM_4x25_INFO(pm_info)->phys);
        SOC_PBMP_COUNT(port_phys_in_pm, phys_count);
        
        ii = 0;
        SOC_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy){
            if(SOC_PBMP_MEMBER(port_phys_in_pm, phy)){
                rv = PM4x25_LANE2PORT_SET(unit, pm_info, ii, port);
                _SOC_IF_ERR_EXIT(rv);
                first_phy = (first_phy == -1) ? phy : first_phy;
                port_index = (port_index == -1 ? ii : port_index);
            }
            ii++;
        }
        PM_4x25_INFO(pm_info)->first_phy = first_phy;
    }

    /* init data for port configuration */
    sal_memcpy(&PM_4x25_INFO(pm_info)->port_config[port_index].port_init_config, \
        &add_info->init_config, sizeof(portmod_port_init_config_t));
    sal_memcpy(&PM_4x25_INFO(pm_info)->port_config[port_index].interface_config, \
        &add_info->interface_config, sizeof(portmod_port_interface_config_t));

    rv = PM4x25_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* if not active - initalize PM */
    if(!pm_is_active){
        if (!PORTMOD_PORT_ADD_F_INIT_RESUME_AFTER_FW_LOAD_GET(add_info)) {
            /*init the PM*/
            rv = _pm4x25_pm_enable(unit, port, pm_info, port_index, add_info, 1);
            _SOC_IF_ERR_EXIT(rv);
        }

        pm_is_active = 1;
        rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* probe core and core config internal serdes and ext phys if there is any. */
    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] > 0 ){
        rv = _pm4x25_pm_phys_enable(unit, port, pm_info, port_index, add_info, 1);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (PORTMOD_PORT_ADD_F_INIT_UNTIL_FW_LOAD_GET(add_info)) {
        return (rv);
    }

    /* initalize port */
    rv = _pm4x25_pm_port_init(unit, port, pm_info, port_index, add_info, 1);
    _SOC_IF_ERR_EXIT(rv);

    /* initialze phys */
    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] > 0) {

        _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

        params.phyn = 0;
        _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access_lane, &nof_phys, NULL));

        init_config.an_en = add_info->autoneg_en;
        init_config.cl72_en = add_info->link_training_en;
        my_i = 0;
        for(ii=0 ; ii<PM4X25_LANES_PER_CORE ; ii++) {
            rv = PM4x25_LANE2PORT_GET(unit, pm_info, ii, &port_i);
            _SOC_IF_ERR_EXIT(rv);

            if(port_i != port) {
                continue;
            }

            if(SHR_BITGET(&(PM_4x25_INFO(pm_info)->polarity.tx_polarity),ii)) {
                SHR_BITSET(&init_config.polarity.tx_polarity, my_i);
            }

            if(SHR_BITGET(&(PM_4x25_INFO(pm_info)->polarity.rx_polarity),ii)) {
                SHR_BITSET(&init_config.polarity.rx_polarity, my_i);
            }

            _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access_lane, phymodMediaTypeChipToChip, &init_config.tx[my_i]));

            my_i++;
            
        }
        _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                           phy_access ,(1+MAX_PHYN),
                                                           &nof_phys));

        _SOC_IF_ERR_EXIT(portmod_port_phychain_phy_init(phy_access, nof_phys,
                                                            &init_config));

        if(add_info->init_config.fs_cl72) {
            _SOC_IF_ERR_EXIT(portmod_port_phychain_cl72_set(phy_access, nof_phys,1));
        }
    }

    if(!pm_is_bypassed){
        const portmod_port_interface_config_t *config = &add_info->interface_config;

        rv = pm4x25_port_interface_config_set(unit, port, pm_info, config, TRUE);
        _SOC_IF_ERR_EXIT(rv);

        if(!(PM_4x25_INFO(pm_info)->in_pm12x10)) {

            /* set the default advert ability */
            _SOC_IF_ERR_EXIT
                (pm4x25_port_ability_local_get(unit, port, pm_info, &port_ability));
            _SOC_IF_ERR_EXIT
                (pm4x25_port_ability_advert_set(unit, port, pm_info, &port_ability));
        }
        /* De-Assert SOFT_RESET */
 	_SOC_IF_ERR_EXIT(pm4x25_port_soft_reset(unit, port, pm_info, 0));
    }
    
exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable; 
    int invalid_port = -1;
    int tmp_port;
    int i = 0, flags = 0;
    int rv = 0;
    int is_last_one = TRUE;
    int port_index = -1;
    int phytype = 0;
    uint32 core_is_initialized = 0;
    uint32 inactive = 0;
    SOC_INIT_FUNC_DEFS;

    if (PM_4x25_INFO(pm_info)->nof_phys[0] == 0) {
        /* There are no phys.., so only mac needs to be checked */
        PORTMOD_PORT_ENABLE_MAC_SET(flags);
    } 

    _SOC_IF_ERR_EXIT(pm4x25_port_enable_get(unit, port, pm_info, flags, &enable));
    if(enable){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }
    
    /*remove from array and check if it was the last one*/
    for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++){
        rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if(tmp_port == port){
            port_index = (port_index == -1 ? i : port_index);
            rv = PM4x25_LANE2PORT_SET(unit, pm_info, i, invalid_port);
            _SOC_IF_ERR_EXIT(rv);
        } else if (tmp_port != -1){
            is_last_one = FALSE;
        }
    }

    if ((PM_4x25_INFO(pm_info)->nof_phys[0]) && (port_index == -1))  {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Port %d wasn't found"), port));
    }   

    rv = _pm4x25_pm_port_init(unit, port, pm_info, port_index, NULL, 0);
    _SOC_IF_ERR_EXIT(rv);

    /*deinit PM in case of last one*/
    if(is_last_one){
       rv = _pm4x25_pm_enable(unit, port, pm_info, port_index, NULL, 0);
       _SOC_IF_ERR_EXIT(rv);

       rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, inactive);
       _SOC_IF_ERR_EXIT(rv);

       phytype = phymodDispatchTypeCount;
       rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phy_type], &phytype, 0);
       _SOC_IF_ERR_EXIT(rv);

       core_is_initialized = 0;
       rv = PM4x25_IS_INITIALIZED_SET(unit, pm_info, core_is_initialized);
       _SOC_IF_ERR_EXIT(rv);

    }    

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int i, tmp_port;
    int rv = 0;       
    SOC_INIT_FUNC_DEFS;
    

    /* replace old port with new port */
    for(i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = PM4x25_LANE2PORT_SET(unit, pm_info, i, new_port);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_pm_bypass_set(int unit, pm_info_t pm_info, int bypass_enable)
{
    int rv = 0, bypass_enable_get;
    uint32 pm_is_active = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &bypass_enable_get);
    _SOC_IF_ERR_EXIT(rv);

    if (bypass_enable_get != bypass_enable) {
        rv = PM4x25_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
        if(pm_is_active){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't chenge bypass mode for active pm")));
        }

        rv = PM4x25_IS_BYPASSED_SET(unit, pm_info, bypass_enable);
        _SOC_IF_ERR_EXIT(rv);
    }
    
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{   
    SOC_INIT_FUNC_DEFS;
    
    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("core access get cannot be called for PM4X25 which is part of PM12X10")));
    }

    if(phyn >= PM_4x25_INFO(pm_info)->nof_phys[0]){ 
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), PM_4x25_INFO(pm_info)->nof_phys[0]-1, phyn));
    }
    if(phyn < 0) {
        phyn = PM_4x25_INFO(pm_info)->nof_phys[0] - 1; /* last one */
    }
    core_info->ref_clk = PM_4x25_INFO(pm_info)->ref_clk;
    sal_memcpy(&core_info->lane_map, &(PM_4x25_INFO(pm_info)->lane_map), sizeof(phymod_lane_map_t));

exit:
    SOC_FUNC_RETURN; 
}

int pm4x25_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{

    uint32 is_bypassed = 0;  
    int rv = 0, rst_flags = 0;
    int actual_flags = flags;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
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

     /* if MAC is set and either RX or TX set is invalid combination */
    if( (PORTMOD_PORT_ENABLE_MAC_GET(flags)) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags)) ) {
        if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) || (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC RX and TX can't be enabled separately")));
        }
    }
    
    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &is_bypassed);
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
            if(PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
		rst_flags |= CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
	    }
	    rv = clmac_enable_set(unit, port, rst_flags, 1);
            _SOC_IF_ERR_EXIT(rv);
	    if(PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
		_SOC_IF_ERR_EXIT(pm4x25_port_soft_reset(unit, port, pm_info, 0));
	    }
        }
        if((PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) && (!(PM_4x25_INFO(pm_info)->in_pm12x10))){
            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

            _SOC_IF_ERR_EXIT (portmod_port_phychain_power_set(phy_access, nof_phys, &phy_power));

            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_tx_lane_control_set(phy_access, nof_phys,
                                                           phymodTxSquelchOff));

            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_rx_lane_control_set(phy_access, nof_phys,
                                                          phymodRxSquelchOff));

        }
    }
    else{
        if((PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) && (!(PM_4x25_INFO(pm_info)->in_pm12x10))){

            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_tx_lane_control_set(phy_access, nof_phys,
                                                           phymodTxSquelchOff));

            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_rx_lane_control_set(phy_access, nof_phys,
                                                          phymodRxSquelchOff));

            _SOC_IF_ERR_EXIT (portmod_port_phychain_power_set(phy_access, nof_phys, &phy_power));
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
    phymod_phy_access_t phy_access;
    int nof_phys = 0;
    phymod_phy_tx_lane_control_t tx_control = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_control = phymodRxSquelchOn;
    int phy_enable = 0;
    int mac_enable = 0;
    portmod_access_get_params_t params;
    uint32 is_bypassed = 0;
    int rv;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    if (PORTMOD_PORT_ENABLE_PHY_GET(flags)||(0 == flags)) {

        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.lane = -1;
        params.phyn = 0;
        params.sys_side = PORTMOD_SIDE_LINE;
        _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

        phymod_phy_tx_lane_control_get(&phy_access, &tx_control);
        phymod_phy_rx_lane_control_get(&phy_access, &rx_control);

        phy_enable = ((tx_control == phymodTxSquelchOn) &&
                     (rx_control == phymodRxSquelchOn)) ? 0 : 1;
        if(0 != flags) {
            *enable = phy_enable;
        }
    }

    if ((PORTMOD_PORT_ENABLE_MAC_GET(flags)||(0 == flags)) && (!is_bypassed)) {
        _SOC_IF_ERR_EXIT(clmac_enable_get(unit, port, 0, &mac_enable));
        if(0 != flags) {
            *enable = mac_enable;
        }
    }
    if(0 == flags){
        *enable = (mac_enable || phy_enable) ;
    }
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_interface_config_set(int unit, int port, pm_info_t pm_info, 
                                     const portmod_port_interface_config_t* config,
                                     int all_phy)
{
    uint32 pm_is_bypassed = 0;
    phymod_phy_inf_config_t phy_config;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    int rv = 0, flags = 0;
    int port_index=0;
    uint32 bitmap;
    portmod_port_interface_config_t *interface_config;

    SOC_INIT_FUNC_DEFS;

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    if(!pm_is_bypassed){
        rv =  portmod_port_higig2_mode_set(unit, port, PHYMOD_INTF_MODES_HIGIG_GET(config));
        _SOC_IF_ERR_EXIT(rv);

        flags = CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
        rv = clmac_speed_set(unit, port, flags, config->speed);
        _SOC_IF_ERR_EXIT(rv);

        flags = CLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS;
        rv = clmac_encap_set(unit, port, 0, PHYMOD_INTF_MODES_HIGIG_GET(config) ? SOC_ENCAP_HIGIG2 : SOC_ENCAP_IEEE);
        _SOC_IF_ERR_EXIT(rv);

        /* De-Assert SOFT_RESET */
	 	rv = pm4x25_port_soft_reset(unit, port, pm_info, 0);
        _SOC_IF_ERR_EXIT(rv);
    }

    if(!(PM_4x25_INFO(pm_info)->in_pm12x10)){
        _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

        interface_config = &PM_4x25_INFO(pm_info)->port_config[port_index].interface_config;

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access,(1+MAX_PHYN),
                                           &nof_phys));

        _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_config));
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed, 
                           config->interface, &phy_config.interface_type));

        phy_config.data_rate = config->speed;
        phy_config.interface_modes = config->interface_modes;
        phy_config.ref_clock = PM_4x25_INFO(pm_info)->ref_clk;

       _SOC_IF_ERR_EXIT
           (portmod_port_phychain_interface_config_set(phy_access, nof_phys,
                                                    config->flags,
                                                    &phy_config,
                                                    PM_4x25_INFO(pm_info)->ref_clk,
                                                    all_phy));

       interface_config->speed = config->speed;  /* update the new speed */
    }

exit:
    SOC_FUNC_RETURN;    
}

int pm4x25_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    portmod_port_interface_config_t *interface_config;
    phymod_phy_inf_config_t phy_config;
    int           flags = 0, nof_phys = 0, rv, port_index;
    uint32        bitmap;
    SOC_INIT_FUNC_DEFS;

    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("can't get interface config for PM4X25 within PM12X10")));
    }

    if (_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap))
        return (SOC_E_PARAM);
   
    interface_config  = &PM_4x25_INFO(pm_info)->port_config[port_index].interface_config;


    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_config));

    phy_config.data_rate =  interface_config->max_speed;

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_get(phy_access, nof_phys,
                                                    0, PM_4x25_INFO(pm_info)->ref_clk,
                                                    &phy_config));

    config->speed = phy_config.data_rate;
    config->interface_modes = phy_config.interface_modes;
    config->flags = 0;

    rv = portmod_intf_from_phymod_intf(unit, phy_config.interface_type, &(config->interface));
    _SOC_IF_ERR_EXIT(rv);

    rv = clmac_encap_get (unit, port, &flags, &config->encap_mode);
    _SOC_IF_ERR_EXIT(rv);

    config->port_num_lanes  = interface_config->port_num_lanes;
    config->max_speed       = interface_config->max_speed;
    config->pll_divider_req = interface_config->pll_divider_req;
    config->interface_modes = interface_config->interface_modes;

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_cl72_set(phy_access, nof_phys, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_cl72_get(phy_access, nof_phys, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cl72_status_get(int unit, int port, pm_info_t pm_info, phymod_cl72_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_cl72_status_get(phy_access, nof_phys, status));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_loopback_get(int unit, int port, pm_info_t pm_info,
                 portmod_loopback_mode_t loopback_type, int* enable)
{   
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    uint32_t tmp_enable=0;
    phymod_loopback_mode_t phymod_lb_type;
    int rv = PHYMOD_E_NONE;

    SOC_INIT_FUNC_DEFS;

    switch(loopback_type){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(clmac_loopback_get(unit, port, loopback_type, enable));
            break;

        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPMD: /*slide*/
        case portmodLoopbackPhyGloopPCS: /*slide*/
             if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
                (*enable) = 0; /* No phy --> no phy loopback*/
             } else {
                _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                                                loopback_type, &phymod_lb_type));
                _SOC_IF_ERR_EXIT
                    (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                      phy_access ,(1+MAX_PHYN),
                                                      &nof_phys));

                rv = portmod_port_phychain_loopback_get(phy_access,
                                                    nof_phys,
                                                    phymod_lb_type,
                                                    &tmp_enable);
                if (rv == PHYMOD_E_UNAVAIL) {
                    rv = PHYMOD_E_NONE;
                    tmp_enable = 0;
                }

                _SOC_IF_ERR_EXIT(rv);
                *enable = tmp_enable;

             }
             break;
        default:
            (*enable) = 0; /* not supported --> no loopback */
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    phymod_loopback_mode_t phymod_lb_type;

    SOC_INIT_FUNC_DEFS;

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(clmac_loopback_set(unit, port, loopback_type, enable));
        break;

    case portmodLoopbackPhyRloopPCS:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyGloopPCS: /*slide*/
        if(PM_4x25_INFO(pm_info)->nof_phys != 0) {
            if (enable) {
                _SOC_IF_ERR_EXIT(clmac_loopback_set(unit, port, portmodLoopbackMacOuter, 0));
            }

            _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                                                loopback_type, &phymod_lb_type));

            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

            _SOC_IF_ERR_EXIT(portmod_port_phychain_loopback_set(phy_access,
                                                                nof_phys,
                                                                phymod_lb_type,
                                                                enable));
         }
         break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
                  _SOC_MSG("unsupported loopback type %d"), loopback_type));
            break;
    }
    if (enable)
        _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit, port, pm_info));

exit:
    SOC_FUNC_RETURN; 
}

int pm4x25_port_rx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(clmac_rx_enable_set(unit, port, enable));
exit:
    SOC_FUNC_RETURN; 
}

int pm4x25_port_rx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(clmac_rx_enable_get(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_ability_local_get(int unit, int port, pm_info_t pm_info, portmod_port_ability_t* ability)
{

/*    portmod_port_interface_config_t *interface_config; */
    int                 fiber_pref, port_index;
    int                 max_speed = 100000;
    int                 is_dual_lane_port = 0;
    int                 loopback = SOC_PA_LB_PHY;
    uint32        bitmap;
    portmod_port_mode_info_t mode;  
    int rv, phy_acc;

    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    if (_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap))
        return (SOC_E_PARAM);
   
    /* interface_config = &(PM_4x25_INFO(pm_info)->port_config[port_index].interface_config); */

    /* fiber_pref        = PHYMOD_INTF_MODES_FIBER_GET(interface_config);*/
    /* max_speed         = interface_config->max_speed; */
    /* is_dual_lane_port = PM4x25_DUAL_MODE_IF (interface_config->interface) ; */ 
 
    
     
     fiber_pref        = 1; 
     max_speed         = SOC_INFO(unit).port_speed_max[port]; 

    rv = _clport_mode_get(unit, phy_acc, port_index, &mode.cur_mode, &mode.lanes);
    if (rv) return (rv);

    is_dual_lane_port = ( mode.cur_mode == portmodPortModeDual)? 1 : 0;     


    sal_memset(ability, 0, sizeof(*ability));

    if (SAL_BOOT_SIMULATION) {
        loopback = SOC_PA_LB_MAC;
    }
    ability->loopback  = loopback;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->fec       = SOC_PA_FEC | SOC_PA_FEC_REQUEST;
    ability->flags     = SOC_PA_AUTONEG;

    if (is_dual_lane_port) {
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        if (fiber_pref) {
            ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB |
                                           SOC_PA_SPEED_10GB;
                ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
                ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
        } else {
            ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB;
        } 
        switch(max_speed) {  /* must include all the supported speedss */
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
                /* fall through */
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
                /* fall through */
            case 16000: 
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB; 
                /* fall through */
                /* not supported speed
                  case 15750:
                  ability->speed_full_duplex |= SOC_PA_SPEED_15P75GB; */
                /* fall through */
            case 15000:
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
                /* fall through */
                /* not supported speed
                   case 12730:
                   ability->speed_full_duplex |= SOC_PA_SPEED_12P73GB;  */
                /* fall through */
            case 13000:
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
                /* fall through */
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
                /* fall through */
            case 11000:
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
                /* not supported speed 
                   case 10500: 
                   ability->speed_full_duplex |= SOC_PA_SPEED_10P5GB; */
                /* fall through */
            case 10000:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
                /* fall through */
            case 5000:
                ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
                /* fall through */
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        if (max_speed >= 10000) {
            ability->interface |= SOC_PA_INTF_XGMII;
        } 
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = loopback;

        /* if dual lane port */
        /*
         * Coverity
         * This is defencive statement.
         */
        /* coverity[dead_error_line] */
        ability->flags = is_dual_lane_port? 0 : SOC_PA_AUTONEG;
    } else {
        ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        if (fiber_pref)   {
            ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
            ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
        } else {
            ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                      SOC_PA_SPEED_100MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                       SOC_PA_SPEED_100MB;
        }
        switch(max_speed) {
            case 127000:
                ability->speed_full_duplex |= SOC_PA_SPEED_127GB;
            case 120000:
                ability->speed_full_duplex |= SOC_PA_SPEED_120GB;
            case 106000:
                ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
            case 100000:
                ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
            case 50000:
                ability->speed_full_duplex |= SOC_PA_SPEED_50GB;
            case 42000:
                ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
            case 40000:
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
            case 30000:
                ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
            case 25000:
                ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
                /* fall through */
            case 16000:
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB; 
                /* fall through */
            case 15000:
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB; 
                /* fall through */
            case 13000:
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
                /* fall through */
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
                /* fall through */
            default:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
                break; 
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = loopback;
        ability->flags     = SOC_PA_AUTONEG;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_tsce_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));

    return (SOC_E_NONE);






exit:
    SOC_FUNC_RETURN;

}

static int _pm4x25_nof_lanes_get(int unit, int port, pm_info_t pm_info)
{
    int port_index, rv;
    uint32_t bitmap, bcnt = 0;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap);
    if (rv) return (0); /* bit count is zero will flag error */

    while (bitmap) {
        if (bitmap & 0x1) bcnt++;
        bitmap >>= 1;
    }
    return (bcnt);
}

int pm4x25_port_autoneg_set(int unit, int port, pm_info_t pm_info,
                            const phymod_autoneg_control_t* an)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32_t bitmap, an_done;
    phymod_autoneg_control_t  *pAn = (phymod_autoneg_control_t*)an;
   
    SOC_INIT_FUNC_DEFS;

    /* autoneg set is not currently supported with physim. */
    if (SAL_BOOT_SIMULATION) return (SOC_E_NONE);

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        if (pAn->num_lane_adv == 0) {
            pAn->num_lane_adv = _pm4x25_nof_lanes_get(unit, port, pm_info);
        }

        pAn->an_mode = PM_4x25_INFO(pm_info)->port_config[port_index].port_init_config.an_mode;

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            pAn->an_mode = phymod_AN_MODE_SGMII;
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

        /* In case of warm boot, get autoneg informaton from hardware */
        if(SOC_WARM_BOOT(unit)) {
            if (pAn->an_mode >= phymod_AN_MODE_Count) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_autoneg_get(phy_access, nof_phys, pAn, &an_done));
            }
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_set(phy_access, nof_phys, an));
    }
    
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_autoneg_get(int unit, int port, pm_info_t pm_info, phymod_autoneg_control_t* an)
{      
    int port_index;
    uint32_t bitmap;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {

        if (an->num_lane_adv == 0) {
            an->num_lane_adv = _pm4x25_nof_lanes_get(unit, port, pm_info);
        }
        an->an_mode = PM_4x25_INFO(pm_info)->port_config[port_index].port_init_config.an_mode;

        if (an->an_mode == phymod_AN_MODE_NONE) {
            an->an_mode = phymod_AN_MODE_SGMII;
        }

        return(portmod_phy_port_autoneg_get(unit, port, pm_info, an));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_autoneg_status_get (int unit, int port, pm_info_t pm_info, 
                                    phymod_autoneg_status_t* an_status)
{
    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        return(SOC_E_UNAVAIL);
    }

    return (portmod_phy_port_autoneg_status_get (unit, port, pm_info, an_status));
}

int pm4x25_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    return (portmod_phy_port_link_get(unit, port, pm_info, link));
}

int pm4x25_port_mac_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    uint32 reg_val, bitmap;
    int port_index, phy_acc;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
   
    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* coverity[callee_ptr_arith:FALSE] */
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index, (uint32*)(&bitmap)));

    switch(port_index) {
        case 0:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN0_STATUS0_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN0_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;

        case 1:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN1_STATUS0_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN1_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;

        case 2:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN2_STATUS0_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN2_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;

        case 3:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN3_STATUS0_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN3_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                     (_SOC_MSG("Port %d, failed to get port index"), port));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_prbs_config_set(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags, 
                                const phymod_prbs_t* config)
{
    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        return(SOC_E_UNAVAIL);
    }
    return (portmod_phy_port_prbs_config_set(unit, port, pm_info, mode, flags, config));
}

int pm4x25_port_prbs_config_get(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags, 
                                phymod_prbs_t* config)
{
    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        return(SOC_E_UNAVAIL);
    }

    return (portmod_phy_port_prbs_config_get(unit, port, pm_info, mode, flags, config));
}


int pm4x25_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags, int enable)
{
    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        return(SOC_E_UNAVAIL);
    }
    return (portmod_phy_port_prbs_enable_set(unit, port, pm_info, mode, flags, enable));
}

int pm4x25_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags, int* enable)
{ 
    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        return(SOC_E_UNAVAIL);
    }
    return (portmod_phy_port_prbs_enable_get(unit, port, pm_info, mode, flags, enable));
}


int pm4x25_port_prbs_status_get(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags, 
                                phymod_prbs_status_t* status)
{
    if(PM_4x25_INFO(pm_info)->nof_phys == 0) {
        return(SOC_E_UNAVAIL);
    }
    return (portmod_phy_port_prbs_status_get(unit, port, pm_info, mode, flags, status));
}


int pm4x25_port_nof_lanes_get(int unit, int port, pm_info_t pm_info, int *nof_lanes)
{
    int temp_nof_lanes = 0;
    SOC_INIT_FUNC_DEFS;

    temp_nof_lanes = _pm4x25_nof_lanes_get(unit, port, pm_info);
    if(0 == temp_nof_lanes){
        SOC_EXIT;
    }
    else{
        *nof_lanes = temp_nof_lanes;
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_firmware_mode_set(int unit, int port, pm_info_t pm_info,
                                  phymod_firmware_mode_t fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */
     
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
    
    _SOC_IF_ERR_EXIT(clmac_pad_size_set(unit, port , value));
    
exit:
    SOC_FUNC_RETURN;     
}

int pm4x25_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    _SOC_IF_ERR_EXIT(clmac_pad_size_get(unit, port , value));
        
exit:
    SOC_FUNC_RETURN;     
}


int pm4x25_port_tx_drop_on_local_fault_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_get(unit, port, pm_info, &control));
    control.drop_tx_on_fault  = enable;
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_set(unit, port, pm_info, &control));
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_tx_drop_on_local_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{       
    portmod_local_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_get(unit, port, pm_info, &control));
    *enable = control.drop_tx_on_fault;
exit:
    SOC_FUNC_RETURN; 
}


int pm4x25_port_tx_drop_on_remote_fault_set(int unit, int port,
                                            pm_info_t pm_info, int enable)
{        
    portmod_remote_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(pm4x25_port_remote_fault_control_get(unit, port, pm_info, &control));
    control.drop_tx_on_fault = enable;
    _SOC_IF_ERR_EXIT(pm4x25_port_remote_fault_control_set(unit, port, pm_info, &control));
exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_tx_drop_on_remote_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_remote_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(pm4x25_port_remote_fault_control_get(unit, port, pm_info, &control));
    *enable = control.drop_tx_on_fault;
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

int pm4x25_port_local_fault_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_get(unit, port, pm_info, &control));
    control.enable = enable;
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_set(unit, port, pm_info, &control));
exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_local_fault_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_get(unit, port, pm_info, &control));
    *enable = control.enable;

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_local_fault_status_get(int unit, int port,
                                       pm_info_t pm_info, int* value)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_local_fault_status_get(unit, port, 0, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_local_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_local_fault_control_set(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_local_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_local_fault_control_get(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_remote_fault_status_get(int unit, int port,
                                        pm_info_t pm_info, int* value)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_remote_fault_status_get(unit, port, 0, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_remote_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_remote_fault_control_set(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_remote_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_remote_fault_control_get(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pause_control_set(int unit, int port, pm_info_t pm_info,
                                  const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pause_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pause_control_get(int unit, int port, pm_info_t pm_info,
                                  portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pause_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pfc_control_set(int unit, int port,
           pm_info_t pm_info, const portmod_pfc_control_t* control)
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


int pm4x25_port_llfc_control_set(int unit, int port,
            pm_info_t pm_info, const portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_llfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_llfc_control_get(int unit, int port,
            pm_info_t pm_info, portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_llfc_control_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_core_access_get (int unit, int port,
                                 pm_info_t pm_info, int phyn, int max_cores,
                                 phymod_core_access_t* core_access_arr,
                                 int* nof_cores,
                                 int* is_most_ext)
{
    int port_index;
    uint32 bitmap;
    int index;
    int done=0;
    int i,j;
    uint32 xphy_id;
    portmod_xphy_core_info_t xphy_core_info;

    SOC_INIT_FUNC_DEFS;

    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("core access get cannot be called for PM4X25 which is part of PM12X10")));
    }

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(phyn > PM_4x25_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), 
                       PM_4x25_INFO(pm_info)->nof_phys[port_index] - 1, phyn));
    }

    if(phyn < 0)
    {
        phyn = PM_4x25_INFO(pm_info)->nof_phys[port_index] - 1;
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));

    if( phyn == 0 ){
        sal_memcpy(&core_access_arr[0], &(PM_4x25_INFO(pm_info)->int_core_access),
                     sizeof(phymod_core_access_t));
        *nof_cores = 1;
    } else {
        index = 0;
        done = 0;
        for(i=0 ; (i< MAX_PORTS_PER_PM4X25); i++) {
            if(bitmap & (0x1U << i)) {
                xphy_id = PM_4x25_INFO(pm_info)->lane_conn[phyn-1][i].xphy_id;
                if( (xphy_id != PORTMOD_XPHY_ID_INVALID) && ( xphy_id < PORTMOD_MAX_NUM_XPHY_SUPPORTED)){
                    if(index == 0){
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
                        sal_memcpy(&core_access_arr[index], &(xphy_core_info.core_access),
                                   sizeof(phymod_core_access_t));
                        index++;
                    } else {
                        for( j = 0 ; (j < index) &&(!done) ; j++) {
                             if((!sal_memcmp (&core_access_arr[j], &xphy_core_info.core_access,
                                               sizeof(phymod_core_access_t)))) { /* found a match */
                                done = 1;
                            }
                        }
                        if((!done) && (index < max_cores)){                        
                            sal_memcpy(&core_access_arr[index], &(xphy_core_info.core_access),
                                       sizeof(phymod_core_access_t));
                            index++;
                        }
                    }
                }
            }
        }
        *nof_cores = index;
    }
    if (is_most_ext) {
        if (phyn == PM_4x25_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x25_port_frame_spacing_stretch_set
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int pm4x25_port_frame_spacing_stretch_set (int unit, int port, 
                                           pm_info_t pm_info,int spacing)
{
    return(clmac_frame_spacing_stretch_set(unit, port, spacing));
}

/*! 
 * pm4x25_port_frame_spacing_stretch_get
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int pm4x25_port_frame_spacing_stretch_get (int unit, int port,
                                            pm_info_t pm_info, 
                                            const int *spacing)
{
    return (0); 
}

/*! 
 * pm4x25_port_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  diag_info       - 
 */
int pm4x25_port_diag_fifo_status_get (int unit, int port,pm_info_t pm_info, 
                                 const portmod_fifo_status_t* diag_info)
{
    return (0);
}

/*! 
 * pm4x25_port_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm4x25_port_pfc_config_set (int unit, int port,pm_info_t pm_info,
                                const portmod_pfc_config_t* pfc_cfg)
{
    return ((pfc_cfg->classes != 8)? SOC_E_PARAM: clmac_pfc_config_set(unit, port, pfc_cfg));
}

/*! 
 * pm4x25_port_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm4x25_port_pfc_config_get (int unit, int port,pm_info_t pm_info, 
                                portmod_pfc_config_t* pfc_cfg)
{
    pfc_cfg->classes = 8;
    return (clmac_pfc_config_get (unit, port, pfc_cfg));
}

/*!
 * pm4x25_port_eee_set
 *
 * @brief set EEE control and timers
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x25_port_eee_set(int unit, int port, pm_info_t pm_info,const portmod_eee_t* eee)
{
    return (clmac_eee_set(unit, port, eee));
}

/*!
 * pm4x25_port_eee_get
 *
 * @brief get EEE control and timers
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x25_port_eee_get(int unit, int port, pm_info_t pm_info,portmod_eee_t* eee)
{
    return (clmac_eee_get(unit, port, eee));
}

/*! 
 * pm4x25_port_vlan_tag_set
 *
 * @brief vlan tag set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x25_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,const portmod_vlan_tag_t* vlan_tag)
{
    return (clmac_rx_vlan_tag_set (unit, port, vlan_tag->outer_vlan_tag,
                                   vlan_tag->inner_vlan_tag));
}


/*! 
 * pm4x25_port_vlan_tag_get
 *
 * @brief vlan tag get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x25_port_vlan_tag_get(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    return (clmac_rx_vlan_tag_get (unit, port, (int*)&vlan_tag->outer_vlan_tag,
                                   (int*)&vlan_tag->inner_vlan_tag));
}

/*! 
 * pm4x25_port_duplex_set
 *
 * @brief duplex set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable        - 
 */
int pm4x25_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return (clmac_duplex_set (unit, port, enable)); 
}


/*! 
 * pm4x25_port_duplex_get
 *
 * @brief duplex get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x25_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    return (clmac_duplex_get (unit, port, enable)); 
}

/*!
 * pm4x25_port_speed_get
 *
 * @brief duplex get.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  speed         -
 */
int pm4x25_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    return (clmac_speed_get (unit, port, speed));
}

int pm4x25_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int flags, int reg_addr, uint32* value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_reg_read(phy_access,
                                        nof_phys, reg_addr,value));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int flags, int reg_addr, uint32 value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_reg_write(phy_access,
                                         nof_phys, reg_addr,value));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_reset_set (int unit, int port, pm_info_t pm_info, 
                           int reset_mode, int opcode, int direction)
{
    if (PM_4x25_INFO(pm_info)->nof_phys == 0) {
        return(SOC_E_UNAVAIL);
    } 
    return(portmod_phy_port_reset_set (unit, port, pm_info, reset_mode, opcode, direction));
}

int pm4x25_port_reset_get (int unit, int port, pm_info_t pm_info, 
                           int reset_mode, int opcode, int* direction)
{
    if (PM_4x25_INFO(pm_info)->nof_phys == 0) {
        return(SOC_E_UNAVAIL);
    } 
    return(portmod_phy_port_reset_get (unit, port, pm_info, reset_mode, opcode, direction));
}

/*Port remote Adv get*/
int pm4x25_port_adv_remote_get (int unit, int port, pm_info_t pm_info, 
                                int* ability_mask)
{
    return (0);
}

/*get port fec enable*/
int pm4x25_port_fec_enable_get(int unit, int port, pm_info_t pm_info, 
                                 uint32_t* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_fec_enable_get(phy_access, nof_phys, enable));

exit:
    SOC_FUNC_RETURN;
}

/*set port fec enable*/
int pm4x25_port_fec_enable_set(int unit, int port, pm_info_t pm_info, 
                                 uint32_t enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_fec_enable_set(phy_access, nof_phys, enable));

exit:
    SOC_FUNC_RETURN;
}

/*get port auto negotiation local ability*/
int pm4x25_port_ability_advert_get(int unit, int port, pm_info_t pm_info, 
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;
    phymod_autoneg_ability_t    an_ability;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_get(phy_access,
                                                       nof_phys, &an_ability));
    portmod_common_phy_to_port_ability(&an_ability, ability);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_ability_advert_set(int unit, int port, pm_info_t pm_info, 
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0, port_index = 0;    
    int port_num_lanes, line_interface, cx4_10g;
    int an_cl72, an_fec, hg_mode;    
    portmod_port_ability_t      port_ability;
    phymod_autoneg_ability_t    an_ability;
    portmod_port_interface_config_t interface_cfg;
    pm_4x25_port_t *pInfo = NULL; 
    uint32_t bitmap = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap));   
    pInfo = &(PM_4x25_INFO(pm_info)->port_config[port_index]);
    _SOC_IF_ERR_EXIT
        (pm4x25_port_interface_config_get(unit, port, pm_info, &interface_cfg));

    _SOC_IF_ERR_EXIT
        (pm4x25_port_ability_local_get(unit, port, pm_info, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.pause             &= ability->pause;
    port_ability.interface         &= ability->interface;
    port_ability.medium            &= ability->medium;
    port_ability.eee               &= ability->eee;
    port_ability.loopback          &= ability->loopback;
    port_ability.flags             &= ability->flags;
    port_ability.speed_half_duplex &= ability->speed_half_duplex;
    port_ability.speed_full_duplex &= ability->speed_full_duplex;

    port_num_lanes = pInfo->interface_config.port_num_lanes;
    cx4_10g        = pInfo->port_init_config.cx4_10g;
    an_cl72        = pInfo->port_init_config.an_cl72;
    an_fec         = pInfo->port_init_config.an_fec;
    hg_mode        = PHYMOD_INTF_MODES_HIGIG_GET(&pInfo->interface_config); 
    line_interface = interface_cfg.interface;
    
    portmod_common_port_to_phy_ability(&port_ability, &an_ability,
                                       port_num_lanes, line_interface, cx4_10g,
                                       an_cl72, an_fec, hg_mode);

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    an_ability.an_master_lane = PM_4x25_INFO(pm_info)->port_config[port_index].port_init_config.an_master_lane;

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_set(phy_access,
                                          nof_phys, &an_ability));
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                     "Speed(HD=0x%08x, FD=0x%08x) Pause=0x%08x orig(HD=0x%08x, FD=0x%08x) \n"
                     "Interface=0x%08x Medium=0x%08x Loopback=0x%08x Flags=0x%08x\n"),
                     port_ability.speed_half_duplex,
                     port_ability.speed_full_duplex,
                     port_ability.pause,
                     ability->speed_half_duplex,
                     ability->speed_full_duplex,
                     port_ability.interface,
                     port_ability.medium, port_ability.loopback,
                     port_ability.flags));
exit:
    SOC_FUNC_RETURN;
}

/*Port ability remote Adv get*/
int pm4x25_port_ability_remote_get(int unit, int port, pm_info_t pm_info,
                                       portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                         nof_phys;
    portmod_port_ability_t      port_ability;
    phymod_autoneg_ability_t    an_ability;

    SOC_INIT_FUNC_DEFS;

    sal_memset(&port_ability, 0, sizeof(portmod_port_ability_t));

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_remote_ability_get(
                    phy_access, nof_phys, &an_ability));
    portmod_common_phy_to_port_ability(&an_ability, ability);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_rx_control_set (int unit, int port, pm_info_t pm_info,
                                const portmod_rx_control_t* rx_ctrl)
{
    return (0);
}

int pm4x25_port_tx_average_ipg_set (int unit, int port, pm_info_t pm_info, int value)
{
    return (clmac_tx_average_ipg_set(unit, port, value));
}

int pm4x25_port_tx_average_ipg_get (int unit, int port, pm_info_t pm_info, int* value)
{
    return (clmac_tx_average_ipg_get(unit, port, value));
}

int pm4x25_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    int link, flags = 0;
    int duplex = 0, tx_pause = 0, rx_pause = 0;
    phymod_autoneg_status_t autoneg;
    portmod_pause_control_t pause_control;
    portmod_port_ability_t local_advert, remote_advert;
    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT(phymod_autoneg_status_t_init(&autoneg));
    _SOC_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    sal_memset(&local_advert, 0, sizeof(portmod_port_ability_t));
    sal_memset(&remote_advert, 0, sizeof(portmod_port_ability_t));
    
    if(update_control->link_status == -1) {
        _SOC_IF_ERR_EXIT(pm4x25_port_link_get(unit, port, pm_info, &link));
    } else {
        link = update_control->link_status;
    }

    if(!link) {
        /* PHY is down.  Disable the MAC. */

        /* TBD - do we need phymod_link_down_event ? */
    } else {

        _SOC_IF_ERR_EXIT(clmac_enable_set(unit, port, 0, 1));
        /* TBD - do we need phymod_link_up_event ? */

        /* In case of AN - need to update MAC settings*/
        _SOC_IF_ERR_EXIT(pm4x25_port_autoneg_status_get(unit, port, pm_info, &autoneg));
        if(autoneg.enabled && autoneg.locked) {
            /* update MAC */
            _SOC_IF_ERR_EXIT(clmac_enable_set(unit, port, 0, 0));
            flags = CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
            _SOC_IF_ERR_EXIT(clmac_speed_set(unit, port, flags, autoneg.data_rate));
            _SOC_IF_ERR_EXIT(clmac_enable_set(unit, port, 0, 1));
            /* update pause in MAC on the base of local and remote pause ability*/
            _SOC_IF_ERR_EXIT(pm4x25_port_duplex_get(unit, port, pm_info, &duplex));
            _SOC_IF_ERR_EXIT(pm4x25_port_ability_advert_get(unit, port, pm_info, &local_advert));
            _SOC_IF_ERR_EXIT(pm4x25_port_ability_remote_get(unit, port, pm_info, &remote_advert));
            /* IEEE 802.3 Flow Control Resolution.
                   * Please see $SDK/doc/pause-resolution.txt for more information. */
            if (duplex) {
                tx_pause = \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    (local_advert.pause & SOC_PA_PAUSE_RX)) || \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    !(remote_advert.pause & SOC_PA_PAUSE_TX) && \
                    (local_advert.pause & SOC_PA_PAUSE_TX)); 
                rx_pause = \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    (local_advert.pause & SOC_PA_PAUSE_RX)) || \
                    ((local_advert.pause & SOC_PA_PAUSE_RX) && \
                    (remote_advert.pause & SOC_PA_PAUSE_TX) && \
                    !(local_advert.pause & SOC_PA_PAUSE_TX));
            }
            else {
                rx_pause = tx_pause = 0;
            }
            _SOC_IF_ERR_EXIT(pm4x25_port_pause_control_get(unit, port, pm_info, &pause_control));
            if ((pause_control.rx_enable != rx_pause) || (pause_control.tx_enable != tx_pause)) {
                pause_control.rx_enable = rx_pause;
                pause_control.tx_enable = tx_pause;
                _SOC_IF_ERR_EXIT(pm4x25_port_pause_control_set(unit, port, pm_info, &pause_control));
            }
        }
        _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_drv_name_get (int unit, int port, pm_info_t pm_info, 
                              char* buf, int len)
{
    strncpy(buf, "PM4X25 Driver", len);
    return (SOC_E_NONE);
}

int pm4x25_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int lcl_fault, rmt_fault;

    SOC_IF_ERROR_RETURN (pm4x25_port_clear_rx_lss_status_get (unit, port, pm_info, &lcl_fault, &rmt_fault));
    if (lcl_fault) {
        SOC_IF_ERROR_RETURN (pm4x25_port_clear_rx_lss_status_set(unit, port, pm_info, 0, rmt_fault)); 
    }
    return (pm4x25_port_clear_rx_lss_status_set (unit, port, pm_info, 1, rmt_fault));

}

int pm4x25_port_remote_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int lcl_fault, rmt_fault;

    SOC_IF_ERROR_RETURN (pm4x25_port_clear_rx_lss_status_get (unit, port, pm_info, &lcl_fault, &rmt_fault));
    if (rmt_fault) {
        SOC_IF_ERROR_RETURN (pm4x25_port_clear_rx_lss_status_set(unit, port, pm_info, lcl_fault, 0)); 
    }
    return (pm4x25_port_clear_rx_lss_status_set (unit, port, pm_info, lcl_fault, 1));
}

int pm4x25_port_clear_rx_lss_status_set (int unit, soc_port_t port, 
                           pm_info_t pm_info, int lcl_fault, int rmt_fault)
{
    return (clmac_clear_rx_lss_status_set (unit, port, lcl_fault, rmt_fault));
}

int pm4x25_port_clear_rx_lss_status_get (int unit, soc_port_t port, 
                           pm_info_t pm_info, int *lcl_fault, int *rmt_fault)
{
    return (clmac_clear_rx_lss_status_get (unit, port, lcl_fault, rmt_fault));
}

int pm4x25_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));
    soc_reg_field_set(unit, CLPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_lag_failover_loopback_set (int unit, soc_port_t port,
                                        pm_info_t pm_info, int value)
{
    return (clmac_lag_failover_loopback_set(unit, port, value));
}

int pm4x25_port_lag_failover_loopback_get (int unit, soc_port_t port,
                                        pm_info_t pm_info, int *value)
{
    return(clmac_lag_failover_loopback_get(unit, port, value));
}

int pm4x25_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (clmac_tx_mac_sa_set(unit, port, mac_addr));
}

int pm4x25_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (clmac_tx_mac_sa_get(unit, port, mac_addr));
}

int pm4x25_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (clmac_rx_mac_sa_set(unit, port, mac_addr));
}

int pm4x25_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (clmac_rx_mac_sa_get(unit, port, mac_addr));
}

int pm4x25_port_mode_get(int unit, soc_port_t port,
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    int port_index, rv, phy_acc;
    uint32 bitmap;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* coverity[callee_ptr_arith:FALSE] */
    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (rv) return (rv);

    return (_clport_mode_get(unit, phy_acc, port_index, &mode->cur_mode, &mode->lanes));
}

int pm4x25_port_mode_set(int unit, soc_port_t port,
                     pm_info_t pm_info, portmod_port_mode_info_t* mode)
{
    return SOC_E_UNAVAIL;
}

int
pm4x25_port_trunk_hwfailover_config_set(int unit, soc_port_t port, pm_info_t pm_info, int hw_count)
{
    int    old_failover_en=0, new_failover_en = 0;
    int    old_link_status_sel=0, new_link_status_sel=0;
    int    old_reset_flow_control=0, new_reset_flow_control = 0;
    int    lag_failover_lpbk;

    if (hw_count) {
        new_failover_en        = 1;
        new_link_status_sel    = 1;
        new_reset_flow_control = 1;
    }

    SOC_IF_ERROR_RETURN (clmac_lag_failover_loopback_get(unit, port, &lag_failover_lpbk));
    if (lag_failover_lpbk) return (SOC_E_NONE);


    SOC_IF_ERROR_RETURN (clmac_lag_failover_en_get(unit, port, &old_failover_en));
    SOC_IF_ERROR_RETURN (clmac_sw_link_status_select_get (unit, port, &old_link_status_sel));

    if (old_failover_en     != new_failover_en ||
        old_link_status_sel != new_link_status_sel) {

        SOC_IF_ERROR_RETURN (clmac_sw_link_status_select_set (unit, port, new_link_status_sel));
        SOC_IF_ERROR_RETURN (clmac_lag_failover_en_set(unit, port, new_failover_en));
    }

    SOC_IF_ERROR_RETURN (clmac_reset_fc_timers_on_link_dn_get(unit,
                                       port, &old_reset_flow_control));
    if (old_reset_flow_control != new_reset_flow_control) {
        SOC_IF_ERROR_RETURN (clmac_reset_fc_timers_on_link_dn_set(unit,
                                        port, new_reset_flow_control));
    }

    return 0;
}

int
pm4x25_port_trunk_hwfailover_config_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                        int *enable)
{
    return (clmac_lag_failover_en_get(unit, port, enable));
}


int
pm4x25_port_trunk_hwfailover_status_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                        int *loopback)
{
    return (clmac_lag_failover_loopback_get(unit, port, loopback));
}

int pm4x25_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    portmod_access_get_params_t params;


    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));

    if( PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_INT ) {
        params.phyn = 0 ;
    } else { /* most external is default */  
        params.phyn = -1 ;  
    } 
    
    SOC_IF_ERROR_RETURN(pm4x25_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, phy_access, &nof_phys, NULL));

    return (portmod_phy_port_diag_ctrl(unit, port, phy_access, nof_phys, inst, op_type, op_cmd, arg));
}

int pm4x25_port_ifg_set(int unit, int port, pm_info_t pm_info, int speed, soc_port_duplex_t duplex, int ifg,  int *real_ifg)
{
    int         cur_speed;
    int         cur_duplex;
    portmod_port_ability_t ability;
    uint32      pa_flag;

    SOC_INIT_FUNC_DEFS;

    *real_ifg = 0;
    pa_flag = SOC_PA_SPEED(speed);
    sal_memset(&ability, 0, sizeof(portmod_port_ability_t));

    _SOC_IF_ERR_EXIT
        (pm4x25_port_ability_local_get(unit, port, pm_info, &ability));

    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 31 bytes (i.e. multiple of 8 bits) */
    *real_ifg = ifg < 64 ? 64 : (ifg + 7) & (0x1f << 3);

    _SOC_IF_ERR_EXIT(pm4x25_port_duplex_get(unit, port, pm_info,&cur_duplex));
    _SOC_IF_ERR_EXIT(pm4x25_port_speed_get(unit, port,pm_info, &cur_speed));

    /* XLMAC_MODE supports only 4 speeds with 4 being max as LINK_10G_PLUS */
    if ((speed > 100000) && (cur_speed == 100000)) {
        cur_speed = speed;
    }

    if (cur_speed == speed &&
        cur_duplex == (duplex == SOC_PORT_DUPLEX_FULL ? TRUE : FALSE)) {

            pm4x25_port_tx_average_ipg_set (unit, port, pm_info, *real_ifg);

    }


exit:
    SOC_FUNC_RETURN;

    return (SOC_E_NONE);
}

int pm4x25_port_ifg_get(int unit, int port, pm_info_t pm_info, int speed, soc_port_duplex_t duplex, int* ifg)
{


    portmod_port_ability_t ability;
    uint32      pa_flag;

    SOC_INIT_FUNC_DEFS;

    if (!duplex) {
        return SOC_E_PARAM;
    }

    pa_flag = SOC_PA_SPEED(speed); 
    sal_memset(&ability, 0, sizeof(portmod_port_ability_t));

    _SOC_IF_ERR_EXIT
        (pm4x25_port_ability_local_get(unit, port, pm_info, &ability));

    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

exit:
    SOC_FUNC_RETURN;

}

int
pm4x25_port_ref_clk_get(int unit, soc_port_t port, pm_info_t pm_info, int *ref_clk)
{
    *ref_clk = pm_info->pm_data.pm4x25_db->ref_clk;

    return (SOC_E_NONE);
}

int pm4x25_port_lag_remove_failover_lpbk_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (clmac_lag_remove_failover_lpbk_get(unit, port, val));
}

int pm4x25_port_lag_remove_failover_lpbk_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (clmac_lag_remove_failover_lpbk_set(unit, port, val));
}

int pm4x25_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{
    return (clmac_lag_failover_disable(unit, port));
}

int pm4x25_port_mac_ctrl_set(int unit, int port,
                   pm_info_t pm_info, uint64 ctrl)
{
    return (clmac_mac_ctrl_set(unit, port, ctrl));
}

int pm4x25_port_drain_cell_get(int unit, int port, 
           pm_info_t pm_info, portmod_drain_cells_t *drain_cells)
{
    return (clmac_drain_cell_get(unit, port, drain_cells));
}

int pm4x25_port_drain_cell_stop (int unit, int port, 
           pm_info_t pm_info, const portmod_drain_cells_t *drain_cells)
{
    return (clmac_drain_cell_stop (unit, port, drain_cells));
               
}

int pm4x25_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    return (clmac_drain_cell_start(unit, port));
}

int pm4x25_port_txfifo_cell_cnt_get(int unit, int port,
                       pm_info_t pm_info, uint32* fval)
{
    return (clmac_txfifo_cell_cnt_get(unit, port, fval));
}

int pm4x25_port_egress_queue_drain_get(int unit, int port,
                   pm_info_t pm_info, uint64 *ctrl, int *rx)
{
    return (clmac_egress_queue_drain_get(unit, port, ctrl, rx));
}

int pm4x25_port_drain_cells_rx_enable(int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return (clmac_drain_cells_rx_enable(unit, port, rx_en));
}

int pm4x25_port_egress_queue_drain_rx_en(int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return (clmac_egress_queue_drain_rx_en(unit, port, rx_en));
}

int pm4x25_port_mac_reset_set(int unit, int port,
                   pm_info_t pm_info, int val)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    /* if callback defined, go to local soft reset function */
    rv = (PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) ? pm4x25_port_soft_reset(unit, port, pm_info, val) : clmac_soft_reset_set(unit, port, val);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_mac_reset_check(int unit, int port, pm_info_t pm_info,
                                int enable, int* reset)
{
    return (clmac_reset_check(unit, port, enable, reset));
}

int pm4x25_port_core_num_get(int unit, int port, pm_info_t pm_info,
                                int* core_num)
{
    *core_num = PM_4x25_INFO(pm_info)->core_num;
    return (SOC_E_NONE);
}

/* in_out: 1= in reset, 0= in and out of reset */
STATIC
int pm4x25_port_soft_reset(int unit, int port, pm_info_t pm_info, int in_out)
{
    int rv, rx_enable = 0;
    int phy_port, block, bindex, i;
    portmod_drain_cells_t drain_cells;
    uint32 cell_count;
    uint64 mac_ctrl;
    soc_timeout_t to;
    SOC_INIT_FUNC_DEFS;

    /* Callback soft reset function */
    if (PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_4x25_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePre);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (in_out == 0) {
	    rv = clmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable); 
	    _SOC_IF_ERR_EXIT(rv);

	    /* Drain cells */
	    rv = clmac_drain_cell_get(unit, port, &drain_cells);
	    _SOC_IF_ERR_EXIT(rv);

	    /* Start TX FIFO draining */
	    rv = clmac_drain_cell_start(unit, port);
	    _SOC_IF_ERR_EXIT(rv);

	    /* De-assert SOFT_RESET to let the drain start */
	    rv = clmac_soft_reset_set(unit, port, 0);           
	    _SOC_IF_ERR_EXIT(rv);

	    /* Wait until TX fifo cell count is 0 */
	    soc_timeout_init(&to, 250000, 0);
	    for (;;) {
	        rv = clmac_txfifo_cell_cnt_get(unit, port, &cell_count);
	        _SOC_IF_ERR_EXIT(rv);
	        if (cell_count == 0) {
	            break;
        	}
	        if (soc_timeout_check(&to)) {
	            LOG_ERROR(BSL_LS_SOC_PORT,(BSL_META_UP(unit, port, "ERROR: u=%d p=%d timeout draining TX FIFO (%d cells remain)\n"), unit, port, cell_count));
        	    return SOC_E_INTERNAL;
	        }
    	    }
   
	    /* Stop TX FIFO draining */
	    rv = clmac_drain_cell_stop(unit, port, &drain_cells);
	    _SOC_IF_ERR_EXIT(rv);

	    /* Reset port FIFO */
	    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
	    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
	        block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
        	if (SOC_BLOCK_INFO(unit, block).type == SOC_BLK_CLPORT) {
	            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
        	    _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_soft_reset_toggle(unit, port, pm_info, bindex));
	            break;
        	}
    	    }
    }

    /* Put port into SOFT_RESET */
    rv = clmac_soft_reset_set(unit, port, 1);
    _SOC_IF_ERR_EXIT(rv);

    if(in_out == 0) {

	    /* Callback soft reset function */
    	    if (PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
        	rv = PM_4x25_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeDuring);
	        	_SOC_IF_ERR_EXIT(rv);
	        	soc_reg64_field32_set (unit, CLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0); /*make sure restored data wont put mac back in reset*/
    	    }

	    /* Enable TX, restore RX, de-assert SOFT_RESET */
	    rv = clmac_egress_queue_drain_rx_en(unit, port, rx_enable);
	    _SOC_IF_ERR_EXIT(rv);

	    /* Restore clmac_CTRL to original value */
	    rv = clmac_mac_ctrl_set(unit, port, mac_ctrl);
	    _SOC_IF_ERR_EXIT(rv);
    }

    /* Callback soft reset function */
    if (PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_4x25_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePost);
        _SOC_IF_ERR_EXIT(rv);
    }
 
exit:
    SOC_FUNC_RETURN;
}

/*Port discard set*/
int pm4x25_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    return(clmac_discard_set(unit, port, discard));
}

/*Port soft reset set set*/
int pm4x25_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx,
                               int val, int flags)
{
    return (_pm4x25_port_soft_reset_set(unit, port, pm_info, idx, val));
}

/*Port tx_en=0 and softreset mac*/
int pm4x25_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(clmac_tx_enable_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(clmac_discard_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(clmac_soft_reset_set(unit, port, 1));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pgw_reconfig (int unit, int port, pm_info_t pm_info, 
                              const portmod_port_mode_info_t *pmode,
                              int phy_port, int flags)
{
    int rv, idx=0, physical_port, i;
    int phy_acc, block;
    uint32 reg_val;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};

    SOC_INIT_FUNC_DEFS; 

    physical_port = SOC_INFO(unit).port_l2p_mapping[port];
    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
    	block = SOC_PORT_IDX_BLOCK(unit, physical_port, i);
   	if (SOC_BLOCK_INFO(unit, block).type == SOC_BLK_CLPORT) {
        	idx = SOC_PORT_IDX_BINDEX(unit, physical_port, i);
            	break;
        }
    }
    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* 1.a st SOFT_RESET field in CLMAC_CTRL for new ports that use XLMAC */
    rv = clmac_soft_reset_set(unit, port, 1);
    _SOC_IF_ERR_EXIT(rv);

    /* 1.a set PORT[subport] field in CLPORT_SOFT_RESET for all new ports */
    _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[idx], 1);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

    /* Power Save */
    _SOC_IF_ERR_EXIT(READ_CLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 1);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_POWER_SAVEr(unit, phy_acc, reg_val));

    /* Port enable */
    _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set (unit, CLPORT_ENABLE_REGr, &reg_val,
                           port_fields[idx], 1);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

 
    /* set port mode */
    /* _SOC_IF_ERR_EXIT(_clport_mode_set(unit, phy_acc, pmode->cur_mode)); */

    /* Power Save */
    _SOC_IF_ERR_EXIT(READ_CLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_POWER_SAVEr(unit, phy_acc, reg_val));

    /* Release XLPORT SOFT RESET */
    _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[idx], 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

    /*Release Mac Soft Reset */
    rv = clmac_soft_reset_set(unit, port, 0);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

/* get interrupt value. */
int pm4x25_port_interrupt_enable_get (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 *val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_CLPORT_INTR_ENABLEr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MIB_TX_MEM_ERRf);
             break;
        case portmodIntrTypeMibRxMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MIB_RX_MEM_ERRf);
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MAC_TX_CDC_MEM_ERRf);
             break;
        case portmodIntrTypeMacRxCdcMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MAC_RX_CDC_MEM_ERRf);
             break;
        case portmodIntrTypeMacRxTsCdcMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MAC_RX_TS_CDC_MEM_ERRf);
             break;
        case portmodIntrTypeTscErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, TSC_ERRf);
             break;
        case portmodIntrTypePmdErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, PMD_ERRf);
             break;
        case portmodIntrTypeRxFcReqFull:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, RX_FLOWCONTROL_REQ_FULLf);
             break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/* set interrupt value. */
int pm4x25_port_interrupt_enable_set (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_CLPORT_INTR_ENABLEr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MIB_TX_MEM_ERRf, val);
             break;
        case portmodIntrTypeMibRxMemErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MIB_RX_MEM_ERRf, val);
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MAC_TX_CDC_MEM_ERRf, val);
             break;
        case portmodIntrTypeMacRxCdcMemErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MAC_RX_CDC_MEM_ERRf, val);
             break;
        case portmodIntrTypeMacRxTsCdcMemErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MAC_RX_TS_CDC_MEM_ERRf, val);
             break;
        case portmodIntrTypeTscErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, TSC_ERRf, val);
             break;
        case portmodIntrTypePmdErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, PMD_ERRf, val);
             break;
        case portmodIntrTypeRxFcReqFull:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, RX_FLOWCONTROL_REQ_FULLf, val);
             break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_INTR_ENABLEr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/* get interrupt value. */
int pm4x25_port_interrupt_get (int unit, int port, pm_info_t pm_info,
                               int intr_type, uint32* val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_CLPORT_INTR_STATUSr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MIB_TX_MEM_ERRf);
             break;
        case portmodIntrTypeMibRxMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MIB_RX_MEM_ERRf);
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_TX_CDC_MEM_ERRf);
             break;
        case portmodIntrTypeMacRxCdcMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_RX_CDC_MEM_ERRf);
             break;
        case portmodIntrTypeMacRxTsCdcMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_RX_TS_CDC_MEM_ERRf);
             break;
        case portmodIntrTypeTscErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, TSC_ERRf);
             break;
        case portmodIntrTypePmdErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, PMD_ERRf);
             break;
        case portmodIntrTypeRxFcReqFull:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, RX_FLOWCONTROL_REQ_FULLf);
             break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_interrupts_get (int unit, int port, pm_info_t pm_info,
                                int arr_max_size, uint32* intr_arr, uint32* size)
{
    uint32 reg_val, cnt=0;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;
    
    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_CLPORT_INTR_STATUSr(unit, phy_acc, &reg_val));

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MIB_TX_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibTxMemErr;
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MIB_RX_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibRxMemErr;
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_TX_CDC_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMacTxCdcMemErr;
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_RX_CDC_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMacRxCdcMemErr;
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_RX_TS_CDC_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMacRxTsCdcMemErr;
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, TSC_ERRf)) {
        intr_arr[cnt++] = portmodIntrTypeTscErr;
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, RX_FLOWCONTROL_REQ_FULLf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxFcReqFull;
    }

    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x25_port_eee_clock_set
 *
 * @brief set EEE Config.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x25_port_eee_clock_set(int unit, int port, pm_info_t pm_info,
                              const portmod_eee_clock_t* eee_clk)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_EEE_CLOCK_GATEr(unit, phy_acc, eee_clk->clock_gate));
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_EEE_CORE0_CLOCK_GATE_COUNTERr(unit, phy_acc, eee_clk->clock_count));
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_EEE_DURATION_TIMER_PULSEr(unit, phy_acc, eee_clk->timer_pulse));

exit:
    SOC_FUNC_RETURN;
}


/*!
 * portmod_port_eee_clock_get
 *
 * @brief set EEE Config.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  eee             -
 */
int pm4x25_port_eee_clock_get (int unit, int port, pm_info_t pm_info,
                               portmod_eee_clock_t* eee_clk)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_EEE_CLOCK_GATEr(unit, phy_acc, &eee_clk->clock_gate));
    _SOC_IF_ERR_EXIT(READ_CLPORT_EEE_CORE0_CLOCK_GATE_COUNTERr(unit, phy_acc, &eee_clk->clock_count));
    _SOC_IF_ERR_EXIT(READ_CLPORT_EEE_DURATION_TIMER_PULSEr(unit, phy_acc, &eee_clk->timer_pulse));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_check_legacy_phy (int unit, int port, pm_info_t pm_info, int *legacy_phy)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    *legacy_phy = 0;

    if (PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy reset is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
     while (nof_phys > 1) {
         nof_phys--;

         if (PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[nof_phys].access))) {
             *legacy_phy = 1;
             break;
         }
     }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_failover_mode_set(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t failover)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy failover is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_failover_mode_set(phy_access, nof_phys, failover));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_failover_mode_get(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t* failover)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (PM_4x25_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy failover is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_failover_mode_get(phy_access, nof_phys, failover));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_xphy_lane_attach_to_pm (int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_conn)
{   
    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int pm_is_active = 0, rv = 0;
    SOC_INIT_FUNC_DEFS;

    /* Validate parameters */
    _SOC_IF_ERR_EXIT(portmod_xphy_lane_connection_t_validate(unit,lane_conn));

    if (MAX_PHYN <= phyn){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }
    /* Making sure the port macro is not active */
    rv = PM4x25_IS_ACTIVE_GET(unit, pm_info,&pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    if (pm_is_active) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("External Phy cannot be attached to a phy chain of an active port macro")));
    }
    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    SOC_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    candidate_lane_conn = &(PM_4x25_INFO(pm_info)->lane_conn[phyn-1][lane_index]);

    sal_memcpy(candidate_lane_conn, lane_conn, sizeof(portmod_xphy_lane_connection_t));
    
    /* increment the nof_phys for that lane */
    PM_4x25_INFO(pm_info)->nof_phys[lane_index] += 1;

exit:
   SOC_FUNC_RETURN;
}

int pm4x25_xphy_lane_detach_from_pm (int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_conn)
{

    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int pm_is_active = 0, rv = 0;
    SOC_INIT_FUNC_DEFS;

    if (MAX_PHYN <= phyn){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }
    /* Making sure the port macro is not active */
    rv = PM4x25_IS_ACTIVE_GET(unit, pm_info,&pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    if (pm_is_active) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("External Phy cannot be attached to a phy chain of an active port macro")));
    }
    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    SOC_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    candidate_lane_conn = &(PM_4x25_INFO(pm_info)->lane_conn[phyn-1][lane_index]);

    sal_memcpy( lane_conn, candidate_lane_conn, sizeof(portmod_xphy_lane_connection_t));
    portmod_xphy_lane_connection_t_init(unit, candidate_lane_conn); 

    /* decrement the nof_phys for that lane */
    PM_4x25_INFO(pm_info)->nof_phys[lane_index] -= 1;

exit:
   SOC_FUNC_RETURN;
}

int pm4x25_port_flow_control_set (int unit, int port, pm_info_t pm_info,
                                  int merge_mode_en, int parallel_fc_en)
{
    int phy_acc, rv;
    uint32 rval;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = READ_CLPORT_FLOW_CONTROL_CONFIGr(unit, phy_acc, &rval);
    if (SOC_FAILURE(rv)) return (rv);

    soc_reg_field_set(unit, CLPORT_FLOW_CONTROL_CONFIGr, &rval, MERGE_MODE_ENf, merge_mode_en);
    soc_reg_field_set(unit, CLPORT_FLOW_CONTROL_CONFIGr, &rval, PARALLEL_FC_ENf, parallel_fc_en);

    return(WRITE_CLPORT_FLOW_CONTROL_CONFIGr(unit, phy_acc, rval));
}

int pm4x25_port_e2ecc_hdr_set (int unit, int port, pm_info_t pm_info,
                               const portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    return (clmac_e2ecc_hdr_set (unit, port, (uint32_t *)e2ecc_hdr->words));
}


int pm4x25_port_e2ecc_hdr_get (int unit, int port, pm_info_t pm_info,
                               portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    return (clmac_e2ecc_hdr_get (unit, port, e2ecc_hdr->words));
}

int pm4x25_port_e2e_enable_set (int unit, int port, pm_info_t pm_info, int enable)
{
    return (clmac_e2e_enable_set(unit, port, enable));
}


int pm4x25_port_e2e_enable_get (int unit, int port, pm_info_t pm_info, int *enable)
{
    return (clmac_e2e_enable_get(unit, port, enable));
}

int pm4x25_port_notify(int unit, int port, pm_info_t pm_info, int link) {
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    portmod_port_interface_config_t interface_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    /* If nof phys > 1 change the internal phy speed based on the external
    link speed if the link is up*/
    if (nof_phys > 1) {
        if (link) {
            PORTMOD_USER_ACC_CMD_FOR_PHY_SET(phy_access, 0);
            _SOC_IF_ERR_EXIT(portmod_port_interface_config_get(unit,
                                                               port,
                                                               &interface_config));
            /* Set teh interface config to internal phy */
            _SOC_IF_ERR_EXIT(portmod_port_interface_config_set(unit,
                                                               port,
                                                               &interface_config, FALSE));
            PORTMOD_USER_ACC_CMD_FOR_PHY_CLR(phy_access, 0);
        } else {
            /*TBD*/
        }
    }
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_mac_rsv_mask_set(int unit, int port, pm_info_t pm_info, uint32 rsv_mask)
{
        return(WRITE_CLPORT_MAC_RSV_MASKr(unit, port, rsv_mask));
}

int pm4x25_port_mib_reset_toggle(int unit, int port, pm_info_t pm_info, int port_index)
{
    uint32 reg_val;
    int    rv = 0, phy_acc;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = READ_CLPORT_MIB_RESETr(unit, phy_acc, &reg_val);
    if (SOC_FAILURE(rv)) return (rv);
    SHR_BITSET(&reg_val, port_index);

    rv = WRITE_CLPORT_MIB_RESETr(unit, phy_acc, reg_val);
    if (SOC_FAILURE(rv)) return (rv);
    SHR_BITCLR(&reg_val, port_index);

    return(WRITE_CLPORT_MIB_RESETr(unit, phy_acc, reg_val));
}

int pm4x25_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info,
                         const portmod_port_interface_config_t *f_intf_config,
                         const portmod_port_init_config_t      *f_init_config,
                         const phymod_operation_mode_t phy_op_mode)
{
    pm4x25_t pm4x25_data = NULL;
    phymod_phy_access_t                 phy_access[1+MAX_PHYN];
    portmod_port_interface_config_t    *interface_config;
    portmod_port_init_config_t         *port_init_config;
    phymod_phy_inf_config_t             phy_intf_config;

    phymod_autoneg_control_t            an;

    int     phy, nof_phys, port_index, rv = 0, flags = 0;
    uint32  bitmap, an_done;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) return (rv);

    pm4x25_data      = pm_info->pm_data.pm4x25_db;
    interface_config = &pm4x25_data->port_config[port_index].interface_config;
    port_init_config = &pm4x25_data->port_config[port_index].port_init_config;

    *interface_config = *f_intf_config;
    *port_init_config = *f_init_config;

    SOC_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy) {
        pm4x25_data->first_phy = phy;
        break;
    }

    rv = portmod_port_chain_phy_access_get (unit, port, pm_info, phy_access,
                                           (1+MAX_PHYN), &nof_phys);
    if (SOC_FAILURE(rv)) return (rv);

    rv = portmod_port_phychain_interface_config_get(phy_access, nof_phys,
                                   0, phymodRefClk156Mhz, &phy_intf_config);
    if (SOC_FAILURE(rv)) return (rv);


    interface_config->flags           = 0;
    interface_config->port_refclk_int = phymodRefClk156Mhz;
    interface_config->pll_divider_req = f_init_config->pll_divider_req;
    interface_config->interface_modes = phy_intf_config.interface_modes;

    rv = portmod_port_interface_type_get (unit, port, &interface_config->interface);
    if (SOC_FAILURE(rv)) return (rv);

    clmac_encap_get(unit, port, &flags, &interface_config->encap_mode);
    clmac_speed_get(unit, port, &interface_config->speed);

    if ((interface_config->encap_mode == SOC_ENCAP_HIGIG2) ||
        (interface_config->encap_mode == SOC_ENCAP_HIGIG)) {
        PHYMOD_INTF_MODES_HIGIG_SET(interface_config);
    }

    /* fix the speed */
    interface_config->speed *= interface_config->port_num_lanes;

    rv = portmod_port_phychain_autoneg_get(phy_access, nof_phys, &an, &an_done);
    if (SOC_FAILURE(rv)) return (rv);

    port_init_config->an_mode = an.an_mode;


    return (SOC_E_NONE);
}

int pm4x25_port_timesync_config_set(int unit, int port, pm_info_t pm_info, const portmod_phy_timesync_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_config_set(phy_access,
                                                                  nof_phys,config));
    exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_timesync_config_get(int unit, int port, pm_info_t pm_info, portmod_phy_timesync_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_config_get(phy_access,
                                                                  nof_phys,config));
    exit:
    SOC_FUNC_RETURN;  
}

int pm4x25_port_timesync_enable_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_enable_set(phy_access,
                                                                  nof_phys,enable));
    exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_timesync_enable_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_enable_get(phy_access,
                                                                  nof_phys,enable));

    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_nco_addend_set(int unit, int port, pm_info_t pm_info, uint32 freq_step)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_nco_addend_set(phy_access,
                                                                      nof_phys,freq_step));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_nco_addend_get(int unit, int port, pm_info_t pm_info, uint32* freq_step)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_nco_addend_get(phy_access,
                                                                      nof_phys,freq_step));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_framesync_mode_set(int unit, int port, pm_info_t pm_info, 
                                            const portmod_timesync_framesync_t* framesync)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_framesync_mode_set(phy_access,
                                                                          nof_phys,framesync));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_framesync_mode_get(int unit, int port, pm_info_t pm_info, 
                                            portmod_timesync_framesync_t* framesync)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_framesync_mode_get(phy_access,
                                                                      nof_phys,framesync));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_local_time_set(int unit, int port, pm_info_t pm_info, const uint64 local_time)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_local_time_set(phy_access,
                                                                      nof_phys,local_time));
    exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_timesync_local_time_get(int unit, int port, pm_info_t pm_info, uint64* local_time)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_local_time_get(phy_access, nof_phys,
                                                                      local_time));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_load_ctrl_set(int unit, int port, pm_info_t pm_info, 
                                       uint32 load_once, uint32 load_always)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_load_ctrl_set(phy_access, nof_phys,
                                                                     load_once, load_always));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_load_ctrl_get(int unit, int port, pm_info_t pm_info, 
                                       uint32* load_once, uint32* load_always)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_load_ctrl_get(phy_access, nof_phys,
                                                                     load_once, load_always));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_tx_timestamp_offset_set(int unit, int port, pm_info_t pm_info, uint32 ts_offset)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_tx_timestamp_offset_set(phy_access, nof_phys,
                                                                               ts_offset));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_tx_timestamp_offset_get(int unit, int port, pm_info_t pm_info, uint32* ts_offset)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_tx_timestamp_offset_get(phy_access, nof_phys,
                                                                               ts_offset));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_rx_timestamp_offset_set(int unit, int port, pm_info_t pm_info, uint32 ts_offset)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_rx_timestamp_offset_set(phy_access, nof_phys,
                                                                               ts_offset));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_rx_timestamp_offset_get(int unit, int port, pm_info_t pm_info, uint32* ts_offset)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_rx_timestamp_offset_get(phy_access, nof_phys,
                                                                               ts_offset));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_control_phy_timesync_set(int unit, int port, pm_info_t pm_info, const portmod_port_control_phy_timesync_t config, uint64 value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_control_phy_timesync_set(phy_access, nof_phys,
                                                                       config, value));

    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_control_phy_timesync_get(int unit, int port, pm_info_t pm_info, const portmod_port_control_phy_timesync_t config, uint64* value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_control_phy_timesync_get(phy_access, nof_phys,
                                                                       config, value));

    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_update_dynamic_state(int unit, int port, pm_info_t pm_info, uint32_t port_dynamic_state) {
   
  /* this function will be implemented once DB schema is defined for pm4x25 */
   return SOC_E_NONE;
}

int pm4x25_port_phy_op_mode_get(int unit, int port, pm_info_t pm_info, phymod_operation_mode_t* val)
{
    return (SOC_E_UNAVAIL);
}

#endif /* PORTMOD_PM4X25_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
