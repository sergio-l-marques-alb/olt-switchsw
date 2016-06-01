/*
 *
 * $Id:$
 * 
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_legacy_phy.h>


#include <soc/portmod/xlmac.h>
#include <soc/portmod/pm4x10.h>
#include <soc/portmod/pm12x10_internal.h>
#include <soc/portmod/pm12x10_xgs_internal.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X10_SUPPORT

#if 0
#define PORTMOD_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type = (_core_access)->type; \
    }while(0)
#endif

/* 10G mode at 10.3125G and 1G mode at 1.25G */
#define PM4X10_LANES_PER_CORE (4)
#define MAX_PORTS_PER_PM4X10 (4)

#define PM_4x10_INFO(pm_info) ((pm_info)->pm_data.pm4x10_db)

#define PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc) \
    do { \
        uint32 is_bypass; \
        uint8 in_pm12x10 = PM_4x10_INFO(pm_info)->in_pm12x10; \
        phy_acc = 0; \
        SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypass); \
        phy_acc = (is_bypass && !in_pm12x10 && PM_4x10_INFO(pm_info)->first_phy_port != -1) ? (PM_4x10_INFO(pm_info)->first_phy_port | SOC_REG_ADDR_PHY_ACC_MASK) : port; \
    } while (0)



/* Warm Boot Variables to indicate the boot state */
typedef enum pm4x10_fabric_wb_vars{
    isInitialized,
    isActive,
    isBypassed,
    ports,
    threePortsMode,
    phy_type,
    interfaceConfig,
    portInitConfig,
    phyInitConfig,
    phyInterfaceConfig,
    portDynamicState,
    nofPhys
}pm4x10_wb_vars_t;


typedef struct pm_4x10_port_s {
    portmod_port_interface_config_t    interface_config;
    portmod_port_init_config_t         port_init_config;
    phymod_phy_init_config_t           phy_init_config;
    phymod_phy_inf_config_t            phy_interface_config;
    uint32_t                           port_dynamic_state;    /* specifes if any of the default params in HW are updated */
} pm_4x10_port_t;

struct pm4x10_s{
    soc_pbmp_t phys;
    int first_phy;
    soc_pbmp_t phy_ports;
    int first_phy_port;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t int_core_access ; /* for internal SerDes only */
    int nof_phys[PM4X10_LANES_PER_CORE]; /* internal + External Phys for each lane*/
    uint8 in_pm12x10;
    pm_4x10_port_t  port_config[PM4X10_LANES_PER_CORE];
    uint8 default_fw_loader_is_used;
    uint8 default_bus_is_used;
    portmod_phy_external_reset_f  portmod_phy_external_reset;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    int core_num;
    int core_num_int;  /* 0, 1, 2 for TSC12 */
    portmod_xphy_lane_connection_t lane_conn[MAX_PHYN][PM4X10_LANES_PER_CORE];
};

#define PM4x10_QUAD_MODE_IF(interface) \
    (interface == SOC_PORT_IF_CAUI  || \
     interface == SOC_PORT_IF_XLAUI || \
     interface == SOC_PORT_IF_CR4   || \
     interface == SOC_PORT_IF_SR4   || \
     interface == SOC_PORT_IF_KR4   || \
     interface == SOC_PORT_IF_XGMII || \
     interface == SOC_PORT_IF_DNX_XAUI)

#define PM4x10_DUAL_MODE_IF(interface) \
    (interface == SOC_PORT_IF_RXAUI)

STATIC
int pm4x10_port_soft_reset(int unit, int port, pm_info_t pm_info, int enable);

int pm4x10_pm_interface_type_is_supported(int unit, soc_port_if_t interface, 
                                          int* is_supported)
{
    SOC_INIT_FUNC_DEFS;
    switch(interface){
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SFI:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_CR4:
        case SOC_PORT_IF_KR4:
        case SOC_PORT_IF_ER4:
        case SOC_PORT_IF_XGMII:
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

STATIC
int _pm4x10_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < MAX_PORTS_PER_PM4X10; i++) {
       rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD, 
                     pm_info->wb_vars_ids[ports], &tmp_port, i);
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
int pm4x10_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm4x10", NULL, VERSION(3), 1, SOC_WB_ENGINE_PRE_RELEASE);
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

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "three_ports_mode", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[threePortsMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_type", wb_buffer_index, sizeof(int), NULL,MAX_PORTS_PER_PM4X10,  VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phy_type] = wb_var_id;

    /* Deleting the below WB structures since most likely to be added in the future in a diffrent format */
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "interface_config", wb_buffer_index, sizeof(portmod_port_interface_config_t), NULL, 1, 1, 0xffffffff, 0xffffffff, VERSION(2), VERSION(3), NULL);
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[interfaceConfig] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_init_config", wb_buffer_index, sizeof(portmod_port_init_config_t), NULL, 1, 1, 0xffffffff, 0xffffffff, VERSION(2), VERSION(3), NULL);
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portInitConfig] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_init_config", wb_buffer_index, sizeof(phymod_phy_init_config_t), NULL, 1, 1, 0xffffffff, 0xffffffff, VERSION(2), VERSION(3), NULL);
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phyInitConfig] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_interface_config", wb_buffer_index, sizeof(phymod_phy_inf_config_t), NULL, 1, 1, 0xffffffff, 0xffffffff, VERSION(2), VERSION(3), NULL);
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phyInterfaceConfig] = wb_var_id;

    /* Keeping the below 2 WB var/arr since most likely to be used in the future */
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_dynamic_state", wb_buffer_index, sizeof(uint32_t), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portDynamicState] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "nof_phys", wb_buffer_index, sizeof(int), NULL, PM4X10_LANES_PER_CORE,  VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[nofPhys] = wb_var_id;
	
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

STATIC portmod_ucode_buf_t pm4x10_ucode_buf[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};

int
pm4x10_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
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
                              &(pm4x10_ucode_buf[unit]),
                              XLPORT_WC_UCMEM_DATAm,
                              XLPORT_WC_UCMEM_CTRLr);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
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

/*
 * XLPORT_SOFT_RESET by physical port
 * This function expects 2nd argument port is physical port.
 */
STATIC
int _pm4x10_port_soft_reset_set_by_phyport(int unit, int port, pm_info_t pm_info, int idx, int val)
{
    int phy_port;
    uint32 reg_val, raddr;
    int    block;
    uint8  at;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};

    SOC_INIT_FUNC_DEFS;

    phy_port = port;
    raddr = soc_reg_addr_get(unit, XLPORT_SOFT_RESETr, phy_port, 0,
                 SOC_REG_ADDR_OPTION_PRESERVE_PORT,
                 &block, &at);

    _SOC_IF_ERR_EXIT
      (_soc_reg32_get(unit, block, at, raddr, &reg_val));

    soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &reg_val, port_fields[idx], val);

    raddr = soc_reg_addr_get(unit, XLPORT_SOFT_RESETr, phy_port, 0,
                SOC_REG_ADDR_OPTION_WRITE | SOC_REG_ADDR_OPTION_PRESERVE_PORT,
                &block, &at);
    _SOC_IF_ERR_EXIT(_soc_reg32_set(unit, block, at, raddr, reg_val));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx, int val)
{
    int phy_acc;
    uint32 reg_val;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};

    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_XLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &reg_val, port_fields[idx],
                      val);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{                    
    int phy_acc;      
    uint32 reg_val, old_val;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};

    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
            
    _SOC_IF_ERR_EXIT(READ_XLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
    old_val = reg_val;

    soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &reg_val, port_fields[idx], 1); 

    _SOC_IF_ERR_EXIT(WRITE_XLPORT_SOFT_RESETr(unit, phy_acc, reg_val));
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_SOFT_RESETr(unit, phy_acc, old_val));

exit:
    SOC_FUNC_RETURN;        
}

phymod_bus_t pm4x10_default_bus = {
    "PM4x10 Bus",
    pm4x10_default_bus_read,
    pm4x10_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

int pm4x10_pm_init(int unit,
                   const portmod_pm_create_info_internal_t* pm_add_info,
                   int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_pm4x10_create_info_t *info;
    pm4x10_t pm4x10_data = NULL;
    int i,j, pm_is_active, rv, three_ports_mode;
    int pm_is_initialized;
    int bypass_enable;
    SOC_INIT_FUNC_DEFS;

    info = &pm_add_info->pm_specific_info.pm4x10;
    pm4x10_data = sal_alloc(sizeof(struct pm4x10_s), "pm4x10_specific_db");
    SOC_NULL_CHECK(pm4x10_data);

    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.pm4x10_db = pm4x10_data;
    SOC_PBMP_ASSIGN(pm4x10_data->phys, pm_add_info->phys);
    SOC_PBMP_ASSIGN(pm4x10_data->phy_ports, info->phy_ports);
    pm4x10_data->int_core_access.type = phymodDispatchTypeCount;
    pm4x10_data->default_bus_is_used = 0;
    pm4x10_data->default_fw_loader_is_used = 0;
    pm4x10_data->portmod_phy_external_reset = pm_add_info->pm_specific_info.pm4x10.portmod_phy_external_reset;
    pm4x10_data->portmod_mac_soft_reset = pm_add_info->pm_specific_info.pm4x10.portmod_mac_soft_reset;
    pm4x10_data->first_phy = -1;
    pm4x10_data->first_phy_port = -1;

    /* initialize num of phys for each lane */
    for(i = 0 ; i < PM4X10_LANES_PER_CORE ; i++){
        pm4x10_data->nof_phys[i] = 0;
    }
    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm4x10_data->int_core_access);

    /* initialize lane connections */
    for(i = 0 ; i < MAX_PHYN ; i++){
        for(j = 0 ; j < PM4X10_LANES_PER_CORE ; j++){
            portmod_xphy_lane_connection_t_init(unit, &pm4x10_data->lane_conn[i][j]);
        }
    }

    sal_memcpy(&pm4x10_data->polarity, &info->polarity, 
                sizeof(phymod_polarity_t));
    sal_memcpy(&(pm4x10_data->int_core_access.access), &info->access,
                sizeof(phymod_access_t));
    sal_memcpy(&pm4x10_data->lane_map, &info->lane_map,
                sizeof(pm4x10_data->lane_map));
    pm4x10_data->ref_clk = info->ref_clk;
    pm4x10_data->fw_load_method = info->fw_load_method;
    pm4x10_data->external_fw_loader = info->external_fw_loader;

    if(info->access.bus == NULL) {
        /* if null - use default */
        pm4x10_data->int_core_access.access.bus = &pm4x10_default_bus;
        pm4x10_data->default_bus_is_used = 1;
    }

    if(pm4x10_data->external_fw_loader == NULL) {
        /* if null - use default */
        pm4x10_data->external_fw_loader = pm4x10_default_fw_loader;
        pm4x10_data->default_fw_loader_is_used = 1;
    }
 
   /* initialize num of phys for each lane */
    for(i = 0 ; i < PM4X10_LANES_PER_CORE ; i++){
        pm4x10_data->nof_phys[i] = 1;
    }
 
    pm4x10_data->in_pm12x10   = info->in_pm_12x10;
    pm4x10_data->core_num     = info->core_num;
    pm4x10_data->core_num_int = info->core_num_int;

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm4x10_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if(SOC_WARM_BOOT(unit)){
        rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[phy_type], &pm4x10_data->int_core_access.type);
        _SOC_IF_ERR_EXIT(rv);
    }

    if(!SOC_WARM_BOOT(unit)){

        pm_is_active = 0;
        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD, 
                                pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_initialized = 0;
        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD, 
                                pm_info->wb_vars_ids[isInitialized], &pm_is_initialized);
        _SOC_IF_ERR_EXIT(rv);

        bypass_enable = 0;
        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD, 
                             pm_info->wb_vars_ids[isBypassed], &bypass_enable);
        _SOC_IF_ERR_EXIT(rv);

        three_ports_mode = info->three_ports_mode;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, 
                      pm_info->wb_vars_ids[threePortsMode], &three_ports_mode);
        _SOC_IF_ERR_EXIT(rv);

         rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                    pm_info->wb_vars_ids[phy_type],
                    &PM_4x10_INFO(pm_info)->int_core_access.type, 0);
         _SOC_IF_ERR_EXIT(rv);
    }

exit:
    if(SOC_FUNC_ERROR){
        pm4x10_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

int
pm4x10_default_bus_update(int unit, pm_info_t pm_info, const portmod_bus_update_t* update) 
{
    portmod_default_user_access_t* user_acc;
    soc_error_t rv = SOC_E_NONE;
    SOC_INIT_FUNC_DEFS;

    if (update == NULL) /*restore to default*/
    {
        /* if null - use default */
        PM_4x10_INFO(pm_info)->int_core_access.access.bus = &pm4x10_default_bus;
        PM_4x10_INFO(pm_info)->int_core_access.type = phymodDispatchTypeCount;
        PM_4x10_INFO(pm_info)->default_bus_is_used = 1;
    
        PM_4x10_INFO(pm_info)->external_fw_loader = pm4x10_default_fw_loader;
        PM_4x10_INFO(pm_info)->default_fw_loader_is_used = 1;
    }
    else
    {

        if(PM_4x10_INFO(pm_info)->default_fw_loader_is_used && update->external_fw_loader != NULL) {
            PM_4x10_INFO(pm_info)->external_fw_loader = update->external_fw_loader;
            user_acc = (portmod_default_user_access_t*)PM_4x10_INFO(pm_info)->int_core_access.access.user_acc;
            user_acc->blk_id = (update->blk_id == -1) ? user_acc->blk_id : update->blk_id;
            PM_4x10_INFO(pm_info)->default_fw_loader_is_used = 0;
        }

        if(PM_4x10_INFO(pm_info)->default_bus_is_used && update->default_bus != NULL) {
            PM_4x10_INFO(pm_info)->int_core_access.access.bus = update->default_bus; 
            user_acc = (portmod_default_user_access_t*)PM_4x10_INFO(pm_info)->int_core_access.access.user_acc;
            user_acc->blk_id = (update->blk_id == -1) ? user_acc->blk_id : update->blk_id;
            if (PM_4x10_INFO(pm_info)->int_core_access.type >= phymodDispatchTypeCount) {
                PM_4x10_INFO(pm_info)->int_core_access.type = phymodDispatchTypeCount;
            }
            PM_4x10_INFO(pm_info)->default_bus_is_used = 0;
        }

        if (update->user_acc != NULL) {
            PM_4x10_INFO(pm_info)->int_core_access.access.user_acc = update->user_acc;
        }

    }

    /*update warmboot engine*/
    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
        pm_info->wb_vars_ids[phy_type],
        &PM_4x10_INFO(pm_info)->int_core_access.type, 0);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_port_tricore_lane_info_get(int unit, int port, pm_info_t pm_info,
                                       int *start_lane, int* end_lane,
                                       int *line_start_lane, int* line_end_lane,
                                       uint32 *int_core_lane_mask);

STATIC
int pm4x10_port_phy_init_config_restore (int unit, int port, pm_info_t pm_info,
                                      const portmod_port_init_config_t *init_config,
                                      phymod_phy_init_config_t *phy_init_config)
{
    phymod_phy_access_t         phy_access_arr[1+MAX_PHYN];
    portmod_access_get_params_t params;
    phymod_tx_t                 phymod_tx, phymod_tx_default;
    int core_start_lane=-1, core_end_lane=-1, line_start_lane=-1, line_end_lane=-1;
    int lane;
    uint32 int_lane_mask=0xf;

    int     nof_phys, rv = 0;
    int     i, my_i, port_i, is_legacy_present;

    my_i = 0;
    for(i=0 ; i<PM4X10_LANES_PER_CORE ; i++) {

        rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[ports], &port_i, i);
        if (SOC_FAILURE(rv)) return (rv);

        if(port_i != port) {
            continue;
        }

        if(SHR_BITGET(&PM_4x10_INFO(pm_info)->polarity.tx_polarity,i)) {
            SHR_BITSET(&phy_init_config->polarity.tx_polarity, my_i);
        }

        if(SHR_BITGET(&PM_4x10_INFO(pm_info)->polarity.rx_polarity,i)) {
            SHR_BITSET(&phy_init_config->polarity.rx_polarity, my_i);
        }
        my_i++;
    }

    rv = portmod_access_get_params_t_init(unit, &params);
    if (SOC_FAILURE(rv)) return (rv);

    rv = portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access_arr ,(1+MAX_PHYN),
                                           &nof_phys);
    if (SOC_FAILURE(rv)) return (rv);

    /* Figure out the start lane if the current core belongs to a tricore. */ 
    rv = _pm4x10_port_tricore_lane_info_get(unit, port, pm_info,
                                            &core_start_lane, &core_end_lane,
                                            &line_start_lane, &line_end_lane,
                                            &int_lane_mask);
    if (SOC_FAILURE(rv)) return (rv);
    /* if current core does not belong to a tricore, set the start lane to 0 */
    if(core_start_lane==-1){
        core_start_lane = 0;
    }

    if( nof_phys > 1 ){
        is_legacy_present = ((portmod_default_user_access_t *)(phy_access_arr[nof_phys-1].access.user_acc))->is_legacy_phy_present;
        if (!is_legacy_present) {
            /* get external phy default tx params */
            rv = phymod_phy_media_type_tx_get(&phy_access_arr[nof_phys-1], phymodMediaTypeChipToChip, &phymod_tx_default);
            if (SOC_FAILURE(rv)) return (rv);
        }

        for(i=0 ; i<PM4X10_LANES_PER_CORE ; i++) {
            lane = i + core_start_lane;
            phymod_tx = phymod_tx_default;

            /* overlap external phy preemphasis and amp from config */
            if(init_config->ext_phy_tx_params_user_flag[lane] & PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG) {
                phymod_tx.pre  = init_config->ext_phy_tx_params[lane].pre ;
                phymod_tx.main = init_config->ext_phy_tx_params[lane].main ;
                phymod_tx.post = init_config->ext_phy_tx_params[lane].post ;
            }
            if(init_config->ext_phy_tx_params_user_flag[lane] & PORTMOD_USER_SET_TX_AMP_BY_CONFIG) {
                phymod_tx.amp  = init_config->ext_phy_tx_params[lane].amp ;
            }
            /* save to PM init config */
            phy_init_config->ext_phy_tx[i].pre  = phymod_tx.pre  ;
            phy_init_config->ext_phy_tx[i].main = phymod_tx.main ;
            phy_init_config->ext_phy_tx[i].post = phymod_tx.post ;
            phy_init_config->ext_phy_tx[i].post2= phymod_tx.post2;
            phy_init_config->ext_phy_tx[i].post3= phymod_tx.post3;
            phy_init_config->ext_phy_tx[i].amp  = phymod_tx.amp  ;
        }
    }

    /* get internal serdes default tx params */
    rv = phymod_phy_media_type_tx_get(&phy_access_arr[0], phymodMediaTypeChipToChip, &phymod_tx_default);
    if (SOC_FAILURE(rv)) return (rv);

    for(i=0 ; i<PM4X10_LANES_PER_CORE ; i++) {
        lane = i + core_start_lane;
        phymod_tx = phymod_tx_default;

        /* overlap serdes preemphasis and amp from config */
        if(init_config->tx_params_user_flag[lane] & PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG) {
            phymod_tx.pre  = init_config->tx_params[lane].pre ;
            phymod_tx.main = init_config->tx_params[lane].main ;
            phymod_tx.post = init_config->tx_params[lane].post ;
        }
        if(init_config->tx_params_user_flag[lane] & PORTMOD_USER_SET_TX_AMP_BY_CONFIG) {
            phymod_tx.amp  = init_config->tx_params[lane].amp ;
        }
        /* save to PM init config */
        phy_init_config->tx[i].pre  = phymod_tx.pre  ;
        phy_init_config->tx[i].main = phymod_tx.main ;
        phy_init_config->tx[i].post = phymod_tx.post ;
        phy_init_config->tx[i].post2= phymod_tx.post2;
        phy_init_config->tx[i].post3= phymod_tx.post3;
        phy_init_config->tx[i].amp  = phymod_tx.amp  ;
    }

    return (SOC_E_NONE);
}

STATIC
int _pm4x10_nof_lanes_get(int unit, int port, pm_info_t pm_info)
{
    int port_index, rv;
    uint32_t bitmap, bcnt = 0;

    rv = _pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap);
    if (rv) return (0); /* bit count is zero will flag error */

    while (bitmap) {
        if (bitmap & 0x1) bcnt++;
        bitmap >>= 1;
    }

    return (bcnt);
}

STATIC
int _pm4x10_an_mode_update (int unit, int port, pm_info_t pm_info);

int pm4x10_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info,
                         const portmod_port_interface_config_t *f_intf_config,
                         const portmod_port_init_config_t      *f_init_config,
                         const phymod_operation_mode_t phy_op_mode)
{
    pm4x10_t pm4x10_data = NULL;
    phymod_phy_access_t                 phy_access[1+MAX_PHYN];
    portmod_port_interface_config_t    *interface_config;
    portmod_port_init_config_t         *port_init_config; 
    phymod_phy_inf_config_t            *phy_intf_config;
    phymod_phy_init_config_t           *phy_init_config;

    phymod_autoneg_control_t            an; 

    int     phy, nof_phys, port_index, rv = 0, flags = 0;
    uint32  bitmap, an_done;

    rv = _pm4x10_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) return (rv);

    pm4x10_data      = pm_info->pm_data.pm4x10_db;
    interface_config = &pm4x10_data->port_config[port_index].interface_config;
    port_init_config = &pm4x10_data->port_config[port_index].port_init_config;
    phy_intf_config  = &pm4x10_data->port_config[port_index].phy_interface_config;
    phy_init_config  = &pm4x10_data->port_config[port_index].phy_init_config;

    /* clean up polarity and tx of phy_init_config */
    rv = phymod_phy_init_config_t_init(phy_init_config);
    if (SOC_FAILURE(rv)) return (rv); 


    *interface_config = *f_intf_config;
    *port_init_config = *f_init_config;
   
    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phy_ports, phy) {
        pm4x10_data->first_phy_port = phy;
        break;
    }

    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy) {
        pm4x10_data->first_phy = phy;
        break;
    }

    rv = portmod_port_chain_phy_access_get (unit, port, pm_info, phy_access,
                                           (1+MAX_PHYN), &nof_phys);
    if (SOC_FAILURE(rv)) return (rv);

    rv = portmod_port_phychain_interface_config_get(phy_access, nof_phys,
                                   0, phymodRefClk156Mhz, phy_intf_config);
    if (SOC_FAILURE(rv)) return (rv);


    interface_config->flags           = 0;
    interface_config->port_refclk_int = phymodRefClk156Mhz;
    interface_config->pll_divider_req = f_init_config->pll_divider_req; 
    interface_config->interface_modes |= phy_intf_config->interface_modes; 
    phy_intf_config->interface_modes  |= interface_config->interface_modes;

    rv = portmod_port_interface_type_get (unit, port, &interface_config->interface);
    if (SOC_FAILURE(rv)) return (rv);

    xlmac_encap_get(unit, port, &flags, &interface_config->encap_mode);
    xlmac_speed_get(unit, port, &interface_config->speed);
    
    if ((interface_config->encap_mode == SOC_ENCAP_HIGIG2) ||
        (interface_config->encap_mode == SOC_ENCAP_HIGIG)) {
        PHYMOD_INTF_MODES_HIGIG_SET(interface_config);
    }
    
    /* fix the speed */
    interface_config->speed *= interface_config->port_num_lanes;

    rv = portmod_port_phychain_autoneg_get(phy_access, nof_phys, &an, &an_done);
    if (SOC_FAILURE(rv)) return (rv);

    port_init_config->an_mode = an.an_mode;

    if(an.an_mode == phymod_AN_MODE_NONE){
        _pm4x10_an_mode_update(unit, port, pm_info);
    }
    
    phy_init_config->op_mode = phy_op_mode;
    rv = pm4x10_port_phy_init_config_restore (unit, port, pm_info, port_init_config, phy_init_config);
    if (SOC_FAILURE(rv)) return (rv);

    return (SOC_E_NONE);
}

STATIC
int _pm4x10_tsc_reset(int unit, pm_info_t pm_info, int port, int in_reset)
{
    int phy_acc;
    uint32 reg_val, phy;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy) {break;};

    if(PM_4x10_INFO(pm_info)->in_pm12x10) {
#ifdef PORTMOD_PM12X10_SUPPORT
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
        
        if (SOC_IS_APACHE(unit)) {
            _SOC_IF_ERR_EXIT(pm12x10_xgs_pm4x10_tsc_reset(unit, port, phy, in_reset));
        } else
#endif
        {
            _SOC_IF_ERR_EXIT(pm12x10_pm4x10_tsc_reset(unit, port, phy, in_reset));
        }
#endif
    } else {
        _SOC_IF_ERR_EXIT(READ_XLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));

        soc_reg_field_set (unit, XLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf,
                           in_reset ? 0 : 1);
        soc_reg_field_set (unit, XLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf,
                           in_reset ? 1 : 0);
        soc_reg_field_set (unit, XLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf,
                           in_reset ? 1 : 0);

        _SOC_IF_ERR_EXIT(WRITE_XLPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));
    }

    sal_usleep(1100);

exit:
    SOC_FUNC_RETURN;
}

static int _xlport_mode_get(int unit, int phy_acc, int bindex, 
                      portmod_core_port_mode_t *core_mode, int *cur_lanes)
{
    uint32_t rval;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_XLPORT_MODE_REGr(unit, phy_acc, &rval));

    *core_mode = soc_reg_field_get(unit, XLPORT_MODE_REGr, rval, XPORT0_CORE_PORT_MODEf);

    switch (*core_mode) {
        case portmodPortModeQuad:
            *cur_lanes = 1;
            break;

        case portmodPortModeTri012: 
            *cur_lanes = (bindex == 2 ? 2 : 1);
            break;

        case portmodPortModeTri023:
            *cur_lanes = (bindex == 0 ? 2 : 1);
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

static int _xlport_mode_set(int unit, int phy_acc, portmod_core_port_mode_t cur_mode)
{
    uint32_t rval; 
    int      mode;

    SOC_INIT_FUNC_DEFS;

    switch (cur_mode) {
        case portmodPortModeQuad:
            mode = 0;
            break;

        case portmodPortModeTri012: 
            mode = 1; 
            break;

        case portmodPortModeTri023:
            mode = 2; 
            break;

        case portmodPortModeDual: 
            mode = 3; 
            break;

        case portmodPortModeSingle:
            mode = 4; 
            break;

        default:
            return SOC_E_FAIL;
    }

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_XLPORT_MODE_REGr(unit, phy_acc, &rval));

    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf, mode);
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf, mode);

    _SOC_IF_ERR_EXIT(WRITE_XLPORT_MODE_REGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    int phy_acc;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    return (READ_XLPORT_CNTMAXSIZEr(unit, phy_acc, (uint32_t *)val));
}

int pm4x10_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    uint32 reg_val;
    int phy_acc, rv = 0;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = READ_XLPORT_CNTMAXSIZEr(unit, phy_acc, &reg_val);
    if (SOC_FAILURE(rv)) return (rv);

    soc_reg_field_set(unit, XLPORT_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, val);

    return(WRITE_XLPORT_CNTMAXSIZEr(unit, phy_acc, reg_val));
}

int pm4x10_port_mib_reset_toggle(int unit, int port, pm_info_t pm_info, int port_index)
{
    uint32 reg_val[1];
    int    rv = 0, phy_acc;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = READ_XLPORT_MIB_RESETr(unit, phy_acc, reg_val);
    if (SOC_FAILURE(rv)) return (rv);
    SHR_BITSET(reg_val, port_index);

    rv = WRITE_XLPORT_MIB_RESETr(unit, phy_acc, *reg_val);
    if (SOC_FAILURE(rv)) return (rv);
    SHR_BITCLR(reg_val, port_index);

    return(WRITE_XLPORT_MIB_RESETr(unit, phy_acc, *reg_val));
}
int pm4x10_port_modid_set (int unit, int port, pm_info_t pm_info, int value)
{
    uint32_t rval, modid;
    int      flen, phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    flen = soc_reg_field_length(unit, XLPORT_CONFIGr, MY_MODIDf)? value : 0;
    modid =  (value < (1 <<  flen))? value : 0;

    _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, MY_MODIDf, modid);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_higig2_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG2_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_higig_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_higig2_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &rval));
    *(mode) = soc_reg_field_get(unit, XLPORT_CONFIGr, rval, HIGIG2_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_higig_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &rval));
    *(mode) = soc_reg_field_get(unit, XLPORT_CONFIGr, rval, HIGIG_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_encap_set (int unit, int port, pm_info_t pm_info, 
                                int flags, portmod_encap_t encap)
{
    return (xlmac_encap_set(unit, port, flags, encap));

}

int pm4x10_port_encap_get (int unit, int port, pm_info_t pm_info, 
                            int *flags, portmod_encap_t *encap)
{
    return (xlmac_encap_get(unit, port, flags, encap));
}

int pm4x10_port_config_port_type_set (int unit, int port, pm_info_t pm_info, int type)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, PORT_TYPEf, type);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_config_port_type_get (int unit, int port, pm_info_t pm_info, int *type)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &rval));
    *(type) = soc_reg_field_get(unit, XLPORT_CONFIGr, rval, PORT_TYPEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_led_chain_config (int unit, int port, pm_info_t pm_info, 
                          int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(WRITE_XLPORT_LED_CHAIN_CONFIGr (unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

typedef enum xlmac_port_mode_e{
    XLMAC_4_LANES_SEPARATE  = 0,
    XLMAC_3_TRI_0_1_2_2     = 1,
    XLMAC_3_TRI_0_0_2_3     = 2,
    XLMAC_2_LANES_DUAL      = 3,
    XLMAC_4_LANES_TOGETHER  = 4
} xlmac_port_mode_t;

STATIC
int _pm4x10_port_tricore_lane_info_get(int unit, int port, pm_info_t pm_info,
                                       int *start_lane, int* end_lane, 
                                       int *line_start_lane, int* line_end_lane, 
                                       uint32 *int_core_lane_mask) 
{
    uint32  iphy_lane_mask;
    int port_index=0;
    int core_num_int;
    phymod_phy_inf_config_t     *phy_interface_config;
    SOC_INIT_FUNC_DEFS;

    /* get port index and lane mask */
    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get(unit, port, pm_info, &port_index,
                                            &iphy_lane_mask));

    phy_interface_config = &PM_4x10_INFO(pm_info)->port_config[port_index].phy_interface_config;

    *start_lane  = 0;
    *end_lane    = PM4X10_LANES_PER_CORE-1;
    *int_core_lane_mask = 0x0F;

    *line_start_lane = 0;
    *line_end_lane = PM4X10_LANES_PER_CORE-1;

    core_num_int = PM_4x10_INFO(pm_info)->core_num_int;
                                  
    if(PHYMOD_INTF_MODES_TRIPLE_CORE_GET(phy_interface_config)) {
        /* first check if 120G or 127G */
        if (phy_interface_config->data_rate >= 120000) {
            if(core_num_int == 0){
                *start_lane  = 0;
                *end_lane    = PM4X10_LANES_PER_CORE-1;
                *int_core_lane_mask = 0x0F;
                *line_start_lane = -1;
                *line_end_lane = -1;
             } else if (core_num_int == 1) {
                *start_lane  = 4;
                *end_lane    = 7;
                *line_start_lane = 0;
                *line_end_lane = PM4X10_LANES_PER_CORE-1;
                *int_core_lane_mask = 0x0F;
            } else if (core_num_int == 2) {
                *start_lane     = 6  ;  
                *end_lane       = 9  ;
                *int_core_lane_mask = 0x0F;
                *line_start_lane = -1;
                *line_end_lane = -1;
            }

        } else {
            if(core_num_int == 0){
                if (PHYMOD_INTF_MODES_TC_244_GET(phy_interface_config)){
                    *start_lane     = 0  ;
                    *end_lane       = 1  ;
                    *int_core_lane_mask = 0x03;
                } else if (PHYMOD_INTF_MODES_TC_343_GET(phy_interface_config)){
                    *start_lane     = 0  ;  
                    *end_lane       = 2  ;
                    *int_core_lane_mask = 0x07;
                } else if (PHYMOD_INTF_MODES_TC_442_GET(phy_interface_config)){
                    *start_lane  = 0;
                    *end_lane    = PM4X10_LANES_PER_CORE-1;
                    *int_core_lane_mask = 0x0F;
                }
                *line_start_lane = -1;
                *line_end_lane = -1;

            } else if (core_num_int == 1) {
                if (PHYMOD_INTF_MODES_TC_244_GET(phy_interface_config)){
                    *start_lane     = 2  ;  
                    *end_lane       = 5  ;
                } else if (PHYMOD_INTF_MODES_TC_343_GET(phy_interface_config)){
                    *start_lane     = 3  ;
                    *end_lane       = 6  ;
                } else if (PHYMOD_INTF_MODES_TC_442_GET(phy_interface_config)){
                    *start_lane  = 4;
                    *end_lane    = 7;
                }
                *line_start_lane = 0;
                *line_end_lane = PM4X10_LANES_PER_CORE-1;
                *int_core_lane_mask = 0x0F;
            } else if (core_num_int == 2) {
                if (PHYMOD_INTF_MODES_TC_244_GET(phy_interface_config)){
                    *start_lane     = 6  ;  
                    *end_lane       = 9  ;
                    *int_core_lane_mask = 0x0F;
                } else if (PHYMOD_INTF_MODES_TC_343_GET(phy_interface_config)){
                    *start_lane     = 7  ;
                    *end_lane       = 9  ;
                    *int_core_lane_mask = 0x07;
                } else if (PHYMOD_INTF_MODES_TC_442_GET(phy_interface_config)){
                    *start_lane  = 8;
                    *end_lane    = 9;
                    *int_core_lane_mask = 0x03;
                }
                *line_start_lane = -1;
                *line_end_lane = -1;
            }
        }
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
                                     const portmod_access_get_params_t* params,
                                     int max_phys,
                                     phymod_phy_access_t* phy_access,
                                     int* nof_phys, int* is_most_ext)
{
    int phyn = 0, rv;
    uint32  iphy_lane_mask; 
    int port_index=0;
    int serdes_lane=-1;
    int xphy_lane_mask=0;
    uint32 xphy_id = 0;
    portmod_xphy_core_info_t xphy_core_info;
    uint32 lane_mask[MAX_NUM_CORES];
    phymod_port_loc_t port_loc[MAX_NUM_CORES];
    int i;
    uint32 xphy_idx;
    int num_of_phys;
    int done;
    int sys_start_lane=0;
    int sys_end_lane=0;
    int line_start_lane=0;
    int line_end_lane=0;
    int start_lane = 0;
    int end_lane = 0;
    uint32 int_lane_mask = 0xf;


    SOC_INIT_FUNC_DEFS;

    start_lane  = 0;
    end_lane    = PM4X10_LANES_PER_CORE-1;

    if(max_phys > MAX_NUM_CORES)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("max_phys parameter exceeded the MAX value. max_phys=%d, max allowed %d."),
                       max_phys, MAX_NUM_CORES));
    }

    /* get port index and lane mask */
    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get(unit, port, pm_info, &port_index,
                                            &iphy_lane_mask));

    _SOC_IF_ERR_EXIT(_pm4x10_port_tricore_lane_info_get(unit, port, pm_info,
                                                        &sys_start_lane, &sys_end_lane,
                                                        &line_start_lane, &line_end_lane,
                                                        &int_lane_mask));

    if(!params->apply_lane_mask){
        int_lane_mask = 0xF;
    }

    if(params->phyn >= PM_4x10_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
               PM_4x10_INFO(pm_info)->nof_phys[port_index] - 1, params->phyn));
    }


    for( i = 0 ; i < max_phys; i++) {
        lane_mask[i] = 0;
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    phyn = params->phyn;

    /* if phyn is -1, it is looking for outer most phy.
       assumption is that all lane has same phy depth. */
    if(phyn < 0) {
        phyn = PM_4x10_INFO(pm_info)->nof_phys[port_index] - 1;
    }

    if( phyn == 0 ) {
        /* internal core */
        sal_memcpy (&phy_access[0], &(PM_4x10_INFO(pm_info)->int_core_access),
                    sizeof(phymod_phy_access_t));
        phy_access[0].access.lane_mask = iphy_lane_mask;
        if (params->lane != -1) {

            start_lane  = sys_start_lane;
            end_lane    = sys_end_lane;

            if((params->lane >= start_lane) && (params->lane <= end_lane)) { 
                serdes_lane = port_index + params->lane-start_lane;
                phy_access[0].access.lane_mask &= (0x1 << serdes_lane );
                *nof_phys = 1;
            } else {
                phy_access[0].access.lane_mask &= 0; 
                *nof_phys = 0;
            }
        } else {
            *nof_phys = 1;
            phy_access[0].access.lane_mask &= int_lane_mask;
        }
        phy_access[0].port_loc = phymodPortLocLine; /* only line is availabe for internal. */
        /* if it is warm boot, get probed information from wb db instead of re-probing. */
        if(SOC_WARM_BOOT(unit)) {
            rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phy_type], &(phy_access[0].type));
            _SOC_IF_ERR_EXIT(rv);
        }
    } else {

        /* external phy */
        num_of_phys = 0;

        /* xphy idx is 1 less than phyn since internal core does not count */    
        xphy_idx = phyn-1;
    
        if (params->lane != -1){ /* specific lane */

            if(params->sys_side == PORTMOD_SIDE_SYSTEM){
                start_lane = sys_start_lane;
                end_lane    = sys_end_lane;
            } else { /* PORTMOD_SIDE_LINE */
                start_lane = line_start_lane;
                end_lane    = line_end_lane;
            }

            if((params->lane >= start_lane) && (params->lane <= end_lane)) { 
                serdes_lane = port_index + params->lane-start_lane;
            } else {
                serdes_lane = -1; /* no matching lane in this core. */
                *nof_phys = 0;
            }
            if( serdes_lane != -1){    
                xphy_id = PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;

                if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                    _SOC_IF_ERR_EXIT ( portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
                    sal_memcpy (&phy_access[num_of_phys], &xphy_core_info.core_access,
                                sizeof(phymod_phy_access_t));
                    xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)? PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                  PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                    if( xphy_lane_mask != -1 ){
                        phy_access[0].access.lane_mask = xphy_lane_mask;
                    }
                    phy_access[0].port_loc = ( params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                               phymodPortLocSys : phymodPortLocLine;
                    *nof_phys = 1;
                }
            }
        } else { /* all lanes */
            /* go thru all the lanes related to this port. */
            serdes_lane = 0;

            for (serdes_lane = 0; serdes_lane < MAX_PORTS_PER_PM4X10; serdes_lane++) 
                if (iphy_lane_mask & (1U<<serdes_lane)) {
                xphy_id = PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;
                if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                    _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
                    if(num_of_phys == 0 ) {/* first one */
                        sal_memcpy (&phy_access[num_of_phys], &xphy_core_info.core_access,
                                    sizeof(phymod_phy_access_t));
                        xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                    PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                    PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                        if( xphy_lane_mask != -1) {
                            lane_mask[num_of_phys] |= xphy_lane_mask;
                        }
                            port_loc[num_of_phys] = ( params->sys_side == PORTMOD_SIDE_SYSTEM) ? 
                                                            phymodPortLocSys : phymodPortLocLine;
                            num_of_phys++;
                    } else {
                        /* if not the first one, see if this one sharing core with other 
                           lanes.  */
                        done = 0;

                        /* Skip if the lane is used by an inactive port marked with :i in the config
                         *  * property.
                         */
                        if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port+serdes_lane)){
                            continue;
                        }

                        for( i = 0 ; (i < num_of_phys) &&(!done) ; i++) {
                             if((!sal_memcmp (&phy_access[i], &xphy_core_info.core_access,
                                               sizeof(phymod_phy_access_t)))) { /* found a match */
                                /* update lane */
                                xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                            PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                            PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                                if( xphy_lane_mask != -1) {
                                    lane_mask[i] |= xphy_lane_mask;
                                }
                                done = 1;
                            }
                        }
                        if((!done) && (num_of_phys < max_phys)){
                            sal_memcpy (&phy_access[num_of_phys], &xphy_core_info.core_access,
                            sizeof(phymod_phy_access_t));
                            xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                        PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                        PM_4x10_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                            if( xphy_lane_mask != -1) {
                                lane_mask[num_of_phys] = xphy_lane_mask;
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
        if (phyn == PM_4x10_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }
 
exit:
    SOC_FUNC_RETURN;
}

STATIC 
int _pm4x10_core_probe ( int unit, int port, pm_info_t pm_info, phymod_core_access_t* core_access, int *probe, int is_probe_for_legacy) 
{
    int is_phymod_probed=0;
    uint32 is_identify=0;
    soc_error_t rv = SOC_E_NONE;
    SOC_INIT_FUNC_DEFS;

    *probe = 0;

    if (core_access->type == phymodDispatchTypeCount)
    {
        rv = phymod_core_probe( &(core_access->access), &(core_access->type), &is_phymod_probed);
        if(SOC_FAILURE(rv)) {
            /* restore the old value */
            core_access->type = phymodDispatchTypeCount;
            _SOC_IF_ERR_EXIT(rv);
        }

        if (!is_phymod_probed)
        {
            /* Reset type */
            core_access->type = phymodDispatchTypeCount;

            if (is_probe_for_legacy)
            {
                if (portmod_port_legacy_phy_probe(unit, port))
                {
                    PORTMOD_IS_LEGACY_PHY_SET(&core_access->access);
                    *probe = 1;
                }
            }
        }
        else
        {
            *probe = 1;
        }
    }

    if (core_access->type == phymodDispatchTypeQuadra28) {
        PHYMOD_ACC_F_PRECONDITION_SET(&core_access->access);
        SOC_IF_ERROR_RETURN(phymod_core_identify(core_access, 0, &is_identify));
        PHYMOD_ACC_F_PRECONDITION_CLR(&core_access->access);
    }

exit:
    /* In case that the call to phymod_core_probe hasn't failed and the returned parameter is_phymod_probed is true - 
     * We are interested in changing the value of core_access->access
     */
    /* coverity[end_of_path] */
    SOC_FUNC_RETURN;
}

int pm4x10_port_mac_rsv_mask_set(int unit, int port, pm_info_t pm_info, uint32 rsv_mask)
{
        return(WRITE_XLPORT_MAC_RSV_MASKr(unit, port, rsv_mask)); 
}

STATIC
int _pm4x10_pm_port_init(int unit, int port, int phy_acc, int internal_port,
                         const portmod_port_add_info_t* add_info, int enable, pm_info_t pm_info)
{
    int rv;
    uint32 reg_val, flags;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};
    uint32 rsv_mask;
    SOC_INIT_FUNC_DEFS;

    if(enable) {
        /* RSV Mask */
        rsv_mask = 0;
        SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
        SHR_BITSET(&rsv_mask, 4); /* CRC error */
        SHR_BITSET(&rsv_mask, 6); /* PGW_MACTruncated/Frame out of Range */
        SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/

         rv = __portmod__dispatch__[pm_info->type]->f_portmod_port_mac_rsv_mask_set(unit, port, pm_info, rsv_mask);
        _SOC_IF_ERR_EXIT(rv);


        /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_XLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &reg_val, 
                                  port_fields[internal_port], 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

        soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &reg_val, 
                                  port_fields[internal_port], 0);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

        
        /* Port enable */
        _SOC_IF_ERR_EXIT(READ_XLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &reg_val, 
                                   port_fields[internal_port], 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

        /* Init MAC */
        flags = 0;
        if(PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
            flags |= XLMAC_INIT_F_RX_STRIP_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_APPEND_CRC_GET(add_info)) {
            flags |= XLMAC_INIT_F_TX_APPEND_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_GET(add_info)) {
            flags |= XLMAC_INIT_F_TX_REPLACE_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_PASS_THROUGH_CRC_GET(add_info)) {
            flags |= XLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE;
        }

        if (IS_HG_PORT(unit,port)){
            flags |= XLMAC_INIT_F_IPG_CHECK_DISABLE;
        } else {
            flags &= ~XLMAC_INIT_F_IPG_CHECK_DISABLE;
        }

        if(PHYMOD_INTF_MODES_HIGIG_GET(&(add_info->interface_config))) {
            flags |= XLMAC_INIT_F_IS_HIGIG;
        }
        rv = xlmac_init(unit, port, flags);
        _SOC_IF_ERR_EXIT(rv);

        /* LSS */
        _SOC_IF_ERR_EXIT(READ_XLPORT_FAULT_LINK_STATUSr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &reg_val,
                          REMOTE_FAULTf, 1);
        soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &reg_val,
                          LOCAL_FAULTf, 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_FAULT_LINK_STATUSr(unit, phy_acc, reg_val)); 

        /* Counter MAX size */
        rv = __portmod__dispatch__[pm_info->type]->f_portmod_port_cntmaxsize_set(unit, port, pm_info, 1518);
       _SOC_IF_ERR_EXIT(rv);
 
        /* Reset MIB counters */
        rv = __portmod__dispatch__[pm_info->type]->f_portmod_port_mib_reset_toggle(unit, port, pm_info, internal_port);
       _SOC_IF_ERR_EXIT(rv);

    } else {
        /* Port disable */
        _SOC_IF_ERR_EXIT(READ_XLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
        soc_reg_field_set (unit, XLPORT_ENABLE_REGr, &reg_val, 
                           port_fields[internal_port], 0);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

         /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_XLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &reg_val, 
                          port_fields[internal_port], 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_SOFT_RESETr(unit, phy_acc, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int portmod_interface_pm_4x10_to_phymod_interface(int unit,
                                                  int port,
                                                  const portmod_port_interface_config_t *interface_config,
                                                  const portmod_port_init_config_t *init_config,
                                                  phymod_phy_inf_config_t *phy_interface_config) ;
STATIC
int portmod_interface_pm_4x10_xphy_to_phymod_interface(int unit,
                                                  int port,
                                                  const portmod_port_interface_config_t *interface_config,
                                                  const portmod_port_init_config_t *init_config,
                                                  phymod_phy_inf_config_t *phy_interface_config) ;

/*
*   overwrite default tx params from config file.
*   set phy_init_config in PM.
*/
STATIC
int _pm4x10_port_tx_config_init_set(int core_start_lane, int core_end_lane, 
                                    phymod_tx_t *tx_params_default, phymod_tx_t lane_tx_params_config[],
                                    int *config_flag, phymod_phy_init_config_t *phy_init_config, 
                                    int is_ext_phy)
{
    int i = 0;
    phymod_tx_t per_lane_tx_params_default[PM4X10_LANES_PER_CORE];
    int start_lane = core_start_lane, lane;
    
    /* if the current core does not belong to a tricore */
    if(core_start_lane==-1){
        start_lane = 0;
    }

    for(i=0; i<PM4X10_LANES_PER_CORE; i++){
        per_lane_tx_params_default[i].pre   = tx_params_default->pre;
        per_lane_tx_params_default[i].main  = tx_params_default->main;
        per_lane_tx_params_default[i].post  = tx_params_default->post;
        per_lane_tx_params_default[i].post2 = tx_params_default->post2;
        per_lane_tx_params_default[i].post3 = tx_params_default->post3;
        per_lane_tx_params_default[i].amp   = tx_params_default->amp;

    }

    /* overlap preemphasis and amp from config */
    for(i=0; i<PM4X10_LANES_PER_CORE; i++){
        lane = i + start_lane;
        if(config_flag[lane] & PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG) {
            per_lane_tx_params_default[i].pre  = lane_tx_params_config[lane].pre ;
            per_lane_tx_params_default[i].main = lane_tx_params_config[lane].main ;
            per_lane_tx_params_default[i].post = lane_tx_params_config[lane].post ;
        }
        if(config_flag[lane] & PORTMOD_USER_SET_TX_AMP_BY_CONFIG) {
            per_lane_tx_params_default[i].amp  = lane_tx_params_config[lane].amp ;
        }
    }

    for(i=0 ; i<PM4X10_LANES_PER_CORE ; i++) {
        lane = i + start_lane;
        if(is_ext_phy) {
            phy_init_config->ext_phy_tx_params_user_flag[i] = config_flag[lane];
            phy_init_config->ext_phy_tx[i].pre  = per_lane_tx_params_default[i].pre  ;
            phy_init_config->ext_phy_tx[i].main = per_lane_tx_params_default[i].main ;
            phy_init_config->ext_phy_tx[i].post = per_lane_tx_params_default[i].post ;
            phy_init_config->ext_phy_tx[i].post2= per_lane_tx_params_default[i].post2;
            phy_init_config->ext_phy_tx[i].post3= per_lane_tx_params_default[i].post3;
            phy_init_config->ext_phy_tx[i].amp  = per_lane_tx_params_default[i].amp  ;
        }
        else {
            phy_init_config->tx_params_user_flag[i] = config_flag[lane];
            phy_init_config->tx[i].pre  = per_lane_tx_params_default[i].pre  ;
            phy_init_config->tx[i].main = per_lane_tx_params_default[i].main ;
            phy_init_config->tx[i].post = per_lane_tx_params_default[i].post ;
            phy_init_config->tx[i].post2= per_lane_tx_params_default[i].post2;
            phy_init_config->tx[i].post3= per_lane_tx_params_default[i].post3;
            phy_init_config->tx[i].amp  = per_lane_tx_params_default[i].amp  ;
        }
    }

    return SOC_E_NONE;
}


/*
 *  clear pm_info's phy_interface_config, and set few members of phy_interface_config
 *  initialize phy_init_config tx params and set registers 
*/
int _pm4x10_port_interface_config_init_set(int unit, int port, 
                        pm_info_t pm_info, 
                        const portmod_port_interface_config_t* config)
{
    uint32 reg_val, pm_is_bypassed = 0; 
    int    nof_phys = 0, rv = 0, phy_acc, flags = 0;
    int    is_ext_phy = 0;
    int    core_start_lane=-1, core_end_lane=-1, line_start_lane=-1, line_end_lane=-1;
    uint32 int_lane_mask=0xf;
    phymod_phy_access_t phy_access[1 + MAX_PHYN];
    phymod_phy_inf_config_t     *phy_interface_config;
    phymod_tx_t                 phymod_tx;
    portmod_port_init_config_t *init_config;
    phymod_phy_init_config_t   *phy_init_config;
    int                         port_index, is_legacy_phy = 0, is_higig;
    uint32_t                    bitmap;
    soc_port_if_t               interface = SOC_PORT_IF_NULL;

    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
    init_config          = &PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config;
    phy_interface_config = &PM_4x10_INFO(pm_info)->port_config[port_index].phy_interface_config;
    phy_init_config      = &PM_4x10_INFO(pm_info)->port_config[port_index].phy_init_config;


    is_higig =  ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
                 (config->encap_mode == SOC_ENCAP_HIGIG) ||
                 PHYMOD_INTF_MODES_HIGIG_GET(config)) ? 1 : 0;

    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD, 
                                pm_info->wb_vars_ids[isBypassed], 
                                &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);
   
    if(!pm_is_bypassed){
        /* set HiG mode */
        _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &reg_val));
        soc_reg_field_set (unit, XLPORT_CONFIGr, &reg_val, HIGIG2_MODEf, is_higig);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_CONFIGr(unit, phy_acc, reg_val));
    
        /* set port speed */
        flags = XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
        rv = xlmac_speed_set(unit, port, flags, config->speed);
        _SOC_IF_ERR_EXIT(rv);

        /* set encapsulation */
        flags = XLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS;
        rv = xlmac_encap_set (unit, port, flags, is_higig ?  SOC_ENCAP_HIGIG2 : 
                                                             SOC_ENCAP_IEEE);
        _SOC_IF_ERR_EXIT(rv);

        rv = xlmac_strict_preamble_set(unit, port, (!is_higig && (config->speed >= 10000)));

        _SOC_IF_ERR_EXIT(rv);

    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
 
    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (phy_interface_config));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,  config->speed,
                interface, &phy_interface_config->interface_type));

    phy_interface_config->data_rate       = config->speed;
    phy_interface_config->interface_modes = config->interface_modes;
    phy_interface_config->pll_divider_req = config->pll_divider_req;
    phy_interface_config->ref_clock       = PM_4x10_INFO(pm_info)->ref_clk;

    /* find the start_lane and end_lane of the current core */
    _SOC_IF_ERR_EXIT(_pm4x10_port_tricore_lane_info_get(unit, port, pm_info,
                                                        &core_start_lane, &core_end_lane,
                                                        &line_start_lane, &line_end_lane,
                                                        &int_lane_mask));

    if(nof_phys > 1)
    {   /* external phy. */
        is_ext_phy = 1;
        portmod_interface_pm_4x10_xphy_to_phymod_interface(unit, port,
                                        config, init_config, phy_interface_config);

        /* get most external phy default tx params */
        is_legacy_phy = ((portmod_default_user_access_t *)(phy_access[nof_phys-1].access.user_acc))->is_legacy_phy_present;
        if (!is_legacy_phy) {
            _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access[nof_phys-1], phymodMediaTypeChipToChip, &phymod_tx));
        }

        /* overlap most external phy  preemphasis and amp from config */
        _SOC_IF_ERR_EXIT(_pm4x10_port_tx_config_init_set(core_start_lane, core_end_lane, &phymod_tx, 
                                                         init_config->ext_phy_tx_params, 
                                                         init_config->ext_phy_tx_params_user_flag, 
                                                         phy_init_config, is_ext_phy));
    }
    else
    {   /* internal phy. */
        portmod_interface_pm_4x10_to_phymod_interface(unit, port,
                                        config, init_config, phy_interface_config);
    }

    is_ext_phy = 0;
    /* get serdes default tx params */
    _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(phy_access, phymodMediaTypeChipToChip, &phymod_tx));

    /* overlap serdes preemphasis and amp from config */
    _SOC_IF_ERR_EXIT(_pm4x10_port_tx_config_init_set(core_start_lane, core_end_lane, &phymod_tx, init_config->tx_params,
                                                     init_config->tx_params_user_flag, phy_init_config, is_ext_phy));

    /* set the outmost external PHY tx. internal and intermediary phy will be modified in portmod_port_phychain_phy_init*/
    if (nof_phys > 1){
        SOC_IF_ERROR_RETURN(portmod_port_phychain_tx_set(phy_access,
                                                             nof_phys, phy_init_config->ext_phy_tx));
    }
    
    /* copy phy interface config to phy init config - used by the external phy 
    drivers like sesto during phy init */
    sal_memcpy(&phy_init_config->interface, phy_interface_config, sizeof(phymod_phy_inf_config_t));
    phy_init_config->op_mode = config->port_op_mode;

exit:
    SOC_FUNC_RETURN;
}


STATIC
int _pm4x10_pm_disable (int unit, int port, pm_info_t pm_info, int phy_acc)
{
    uint32 reg_val;
    int rv = 0;

    /* Put Serdes in reset*/
    if(PM_4x10_INFO(pm_info)->portmod_phy_external_reset) {
        rv = PM_4x10_INFO(pm_info)->portmod_phy_external_reset(unit, port, 1);
    } else {
        rv = _pm4x10_tsc_reset(unit, pm_info, port, 1);
    }
    if (rv) return (rv);

    /* put MAC in reset */
    rv = READ_XLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val);
    if (rv) return (rv);

    soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 1);
    return(WRITE_XLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
}

STATIC
int _pm4x10_pm_xlport_init (int unit, int port, pm_info_t pm_info,
                            int port_index, int phy_acc, int interface,
                            int timestamp_mode)     
{
    uint32 reg_val, port_mode;
    int    three_ports_mode, rv;
    SOC_INIT_FUNC_DEFS;

    /* Power Save */
    _SOC_IF_ERR_EXIT(READ_XLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, XLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 0); 
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_POWER_SAVEr(unit, phy_acc, reg_val));

    /* Port configuration */
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, 
               pm_info->wb_vars_ids[threePortsMode], &three_ports_mode);
    _SOC_IF_ERR_EXIT(rv);

    if(PM4x10_QUAD_MODE_IF(interface)) {
        port_mode = XLMAC_4_LANES_TOGETHER;
    } else if(PM4x10_DUAL_MODE_IF(interface)){
        port_mode = (!three_ports_mode) ?  XLMAC_2_LANES_DUAL :
                      ((port_index < 2) ?  XLMAC_3_TRI_0_0_2_3 : 
                                           XLMAC_3_TRI_0_1_2_2);
    } else { /*single mode*/
        port_mode = (!three_ports_mode) ? XLMAC_4_LANES_SEPARATE :
                     ((port_index < 2) ?  XLMAC_3_TRI_0_1_2_2 :
                                          XLMAC_3_TRI_0_0_2_3);
    }

    _SOC_IF_ERR_EXIT(READ_XLPORT_MODE_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set (unit, XLPORT_MODE_REGr, &reg_val, XPORT0_CORE_PORT_MODEf, port_mode);
    soc_reg_field_set (unit, XLPORT_MODE_REGr, &reg_val, XPORT0_PHY_PORT_MODEf,  port_mode);
    soc_reg_field_set (unit, XLPORT_MODE_REGr, &reg_val, EGR_1588_TIMESTAMPING_MODEf, timestamp_mode); 
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_MODE_REGr(unit, phy_acc, reg_val));

    /* Bring MAC OOR */
    _SOC_IF_ERR_EXIT(READ_XLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 0);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));

    /* Get Serdes OOR */
    if(PM_4x10_INFO(pm_info)->portmod_phy_external_reset) {
        if(!((interface == SOC_PORT_IF_CAUI)&&(PM_4x10_INFO(pm_info)->core_num_int))) {
            _SOC_IF_ERR_EXIT(PM_4x10_INFO(pm_info)->portmod_phy_external_reset(unit, port, 1));
            _SOC_IF_ERR_EXIT(PM_4x10_INFO(pm_info)->portmod_phy_external_reset(unit, port, 0));
        }
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_tsc_reset(unit, pm_info, port, 1));
        _SOC_IF_ERR_EXIT(_pm4x10_tsc_reset(unit, pm_info, port, 0));
    }

exit:
    SOC_FUNC_RETURN;        
}

STATIC
int _pm4x10_pm_core_probe (int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int     rv = SOC_E_NONE;
    int    temp, phy;
    soc_pbmp_t port_phys_in_pm;
    int probe =0;
    uint32 xphy_id;
    uint32 nof_ext_phys =0;
    int i;
    portmod_xphy_core_info_t xphy_core_info;
    SOC_INIT_FUNC_DEFS;

    /* probe phys (both internal and external cores. */

    /* Get the first phy related to this port */
    SOC_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    SOC_PBMP_AND(port_phys_in_pm, PM_4x10_INFO(pm_info)->phys);

    /* if internal core is not probe, probe it. Currently checking to type to see
       phymodDispatchTypeCount, later need to move to WB.  */
    _SOC_IF_ERR_EXIT(_pm4x10_core_probe(unit, port, pm_info, &PM_4x10_INFO(pm_info)->int_core_access,&probe, 0));

    /* save probed phy type to wb. */
    if(probe) {
         rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                     pm_info->wb_vars_ids[phy_type],
                     &PM_4x10_INFO(pm_info)->int_core_access.type);
         _SOC_IF_ERR_EXIT(rv);
    }

    /* probe ext core related to this logical port. */
    temp = 0;
    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy) {
        if(SOC_PBMP_MEMBER(port_phys_in_pm, phy)) {
            if( PM_4x10_INFO(pm_info)->nof_phys[temp] >= 2 ){
                nof_ext_phys = PM_4x10_INFO(pm_info)->nof_phys[temp]-1;
                for(i=0 ; i<nof_ext_phys ; i++) {
                    xphy_id = PM_4x10_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                    if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
                        if( !xphy_core_info.core_probed ){
                            probe = 0;
                            if(SOC_E_NONE != (_pm4x10_core_probe(unit,port, pm_info, &xphy_core_info.core_access, &probe, 1))) {
                                /* Remove un-probed external PHY from phy number count */
                                PM_4x10_INFO(pm_info)->nof_phys[temp] = i + 1;
                                /* Set firmware download method to phymodFirmwareLoadMethodNone to avoid image download for unprobed XPHY */
                                xphy_core_info.fw_load_method = phymodFirmwareLoadMethodNone;
                            } else {
                                if (!probe) {
                                /* case where port_phy_addr is specified and phy is not connetced */
                                /* Remove un-probed external PHY from phy number count */
                                PM_4x10_INFO(pm_info)->nof_phys[temp] = i + 1;
                                /* Set firmware download method to phymodFirmwareLoadMethodNone to avoid image download for unprobed XPHY */
                                xphy_core_info.fw_load_method = phymodFirmwareLoadMethodNone;
                                } else {
                                    xphy_core_info.core_probed = 1;
                                }
                            }
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_info_set(unit, xphy_id, &xphy_core_info));
                        }
                    }
                }
            }
        }
        temp++;
    }

   /*update warmboot engine*/
    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
        pm_info->wb_vars_ids[phy_type],
        &PM_4x10_INFO(pm_info)->int_core_access.type, 0);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_pm_serdes_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int     rv;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    const portmod_port_interface_config_t* config;
    uint32 core_is_initialized;
    uint32 init_flags, is_bypass;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    SOC_INIT_FUNC_DEFS;

    config = &(add_info->interface_config);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypass);
    _SOC_IF_ERR_EXIT(rv);
    /* set internal setting. */

    core_conf.firmware_load_method = PM_4x10_INFO(pm_info)->fw_load_method;
    core_conf.firmware_loader = PM_4x10_INFO(pm_info)->external_fw_loader;
    core_conf.lane_map = PM_4x10_INFO(pm_info)->lane_map;

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                        interface, &core_conf.interface.interface_type));
    core_conf.interface.data_rate = config->speed;
    core_conf.interface.interface_modes = config->interface_modes;
    core_conf.interface.ref_clock = PM_4x10_INFO(pm_info)->ref_clk;
    core_conf.interface.pll_divider_req = 0xA;  /* FIX THIS - IN PORTCTRL.c */

    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));

    if(PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_conf);
    }

    init_flags = PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) | PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info);
    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_conf);
    }

    if(PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_conf);
    }

    core_conf.op_datapath = add_info->phy_op_datapath;

    /* deal witn internal serdes first */
    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isInitialized],
                                &core_is_initialized);
     _SOC_IF_ERR_EXIT(rv);
    if(!core_is_initialized){
        if ((is_bypass) && (!PM_4x10_INFO(pm_info)->in_pm12x10)) {
            PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(PM_4x10_INFO(pm_info)->int_core_access.device_op_mode);
        }
         _SOC_IF_ERR_EXIT(phymod_core_init(&PM_4x10_INFO(pm_info)->int_core_access,
                                          &core_conf,
                                          &core_status));
         if(PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || init_flags == 0) {
            core_is_initialized = 1;
            rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                    pm_info->wb_vars_ids[isInitialized],
                                    &core_is_initialized);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    /*update warmboot engine*/
    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
        pm_info->wb_vars_ids[phy_type],
        &PM_4x10_INFO(pm_info)->int_core_access.type, 0);
    _SOC_IF_ERR_EXIT(rv);

    exit:
        SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_pm_ext_phy_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int    temp, phy;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    soc_pbmp_t port_phys_in_pm;
    uint32 xphy_id;
    uint32 nof_ext_phys =0;
    int i;
    portmod_xphy_core_info_t xphy_core_info;
    portmod_xphy_core_info_t xphy_primary_core_info;
    uint32 is_initialized;
    int lane;
    int init_all;
    SOC_INIT_FUNC_DEFS;

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                    !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                    !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    /* Get the first phy related to this port */
    SOC_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    SOC_PBMP_AND(port_phys_in_pm, PM_4x10_INFO(pm_info)->phys);

    /* core init (both internal and external cores. */

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    core_conf.interface.pll_divider_req = 0xA;  /* FIX THIS - IN PORTCTRL.c */

    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));


    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_conf);
    }

    if(PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_conf);
    }

    core_conf.op_datapath = add_info->phy_op_datapath;

    /* deal with ext phys now ... */
    if(PORTMOD_PORT_ADD_F_PORT_ATTACH_EXT_PHY_SKIP_GET(add_info)) {
        return(SOC_E_NONE);
    }

    /* adjust config setup for external phys. */

    /* for external phys, FW load method is default to Internal
       unless it is force to NONE. */


    core_conf.firmware_load_method = (PM_4x10_INFO(pm_info)->fw_load_method ==
                                        phymodFirmwareLoadMethodNone) ?
                                        phymodFirmwareLoadMethodNone :
                                        phymodFirmwareLoadMethodInternal;
    core_conf.firmware_loader = NULL;

    /* config "no swap" for external phys.set later using APIs*/
    for(lane=0 ; lane < (core_conf.lane_map.num_of_lanes); lane++) {
        core_conf.lane_map.lane_map_rx[lane] = lane;
        core_conf.lane_map.lane_map_tx[lane] = lane;
    }


    temp = 0;
    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy) {
        if(SOC_PBMP_MEMBER(port_phys_in_pm, phy)) {
            if( PM_4x10_INFO(pm_info)->nof_phys[temp] >= 2 ){
                nof_ext_phys = PM_4x10_INFO(pm_info)->nof_phys[temp]-1;
                for(i=0 ; i<nof_ext_phys ; i++) {
                    xphy_id = PM_4x10_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                    if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, xphy_id, &xphy_core_info));
                        /* for external phys, FW load method is default to Internal
                        unless it is force to NONE. */
                        core_conf.firmware_load_method = xphy_core_info.fw_load_method;
                        core_conf.firmware_loader = NULL;
                        if (xphy_core_info.force_fw_load == phymodFirmwareLoadForce) {
                            PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_SET(&core_conf);
                        }
                        else if (xphy_core_info.force_fw_load == phymodFirmwareLoadAuto) {
                            PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_SET(&core_conf);
                        }

                        /* config "no swap" for external phys.set later using APIs*/
                        for(lane=0 ; lane < xphy_core_info.lane_map.num_of_lanes; lane++) {
                            core_conf.lane_map.lane_map_rx[lane] = xphy_core_info.lane_map.lane_map_rx[lane];
                            core_conf.lane_map.lane_map_tx[lane] = xphy_core_info.lane_map.lane_map_tx[lane];
                        }
                        if (xphy_id == xphy_core_info.primary_core_num) {
                             is_initialized = xphy_core_info.is_initialized;
                             /* Very first time this function gets called, honor the call and 
                              * ignore the flags */
                             if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
                                 if (PM_4x10_INFO(pm_info)->first_phy == phy) {
                                     /* is_initialized &= ~PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD;*/
                                     /* is_initialized &= ~PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD;*/
                                     xphy_core_info.is_initialized = is_initialized;
                                     xphy_core_info.core_config = core_conf;
                                     _SOC_IF_ERR_EXIT(portmod_xphy_core_info_set(unit, xphy_id, 
                                                &xphy_core_info));
                                 }
                             }
                        } else {
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_info_get(unit, 
                                     xphy_core_info.primary_core_num, &xphy_primary_core_info));
                             is_initialized = xphy_primary_core_info.is_initialized;
                        } 


                        if ((PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_all) &&
                            (is_initialized & PHYMOD_CORE_INIT_F_EXECUTE_PASS1))
                            continue;

                        if ((PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || init_all) &&
                            (is_initialized & PHYMOD_CORE_INIT_F_EXECUTE_PASS2))
                            continue;

                        if (PORTMOD_IS_LEGACY_PHY_GET(&xphy_core_info.core_access.access)) {
                            /* Dont do a core init for legacy Phys.*/
                        } else {
                           _SOC_IF_ERR_EXIT(phymod_core_init(&xphy_core_info.core_access,
                                              &core_conf,
                                              &core_status));
                        }

                        if (init_all) {
                            is_initialized = (PHYMOD_CORE_INIT_F_EXECUTE_PASS2 | PHYMOD_CORE_INIT_F_EXECUTE_PASS1);
                        }
                        else if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
                            is_initialized |= PHYMOD_CORE_INIT_F_EXECUTE_PASS2;
                        } else {
                            is_initialized = PHYMOD_CORE_INIT_F_EXECUTE_PASS1;
                        }

                        xphy_core_info.is_initialized = is_initialized;
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_info_set(unit, xphy_id, &xphy_core_info));

                        if (xphy_id != xphy_core_info.primary_core_num) {
                            xphy_primary_core_info.is_initialized = is_initialized;
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_info_set(unit, xphy_core_info.primary_core_num, 
                                                &xphy_primary_core_info));
                        }
                    }
                }
            }
        }
        temp++;
    }

exit:
    SOC_FUNC_RETURN;
}



STATIC
int _pm4x10_pm_core_init (int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x10_pm_serdes_core_init(unit,pm_info,add_info));
    _SOC_IF_ERR_EXIT(_pm4x10_pm_ext_phy_core_init(unit,pm_info,add_info));

exit:
    SOC_FUNC_RETURN;
}


STATIC
int _pm4x10_port_attach_core_probe (int unit, int port, pm_info_t pm_info, 
                        const portmod_port_add_info_t* add_info)
{
    int        port_index = -1, rv = 0, phy, i, phys_count = 0;
    uint32     pm_is_active = 0;
    soc_pbmp_t port_phys_in_pm, phy_acc_ports_in_pm;
    int first_phy = -1, phy_acc, phy_port;

    SOC_INIT_FUNC_DEFS;

    SOC_PBMP_ASSIGN(phy_acc_ports_in_pm, add_info->phy_ports);
    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phy_ports, phy_port) {
        if(SOC_PBMP_MEMBER(phy_acc_ports_in_pm, phy_port)) {
            PM_4x10_INFO(pm_info)->first_phy_port = phy_port;
            break;
        }
    }

    /* Get the first phy related to this port */
    SOC_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    SOC_PBMP_AND(port_phys_in_pm, PM_4x10_INFO(pm_info)->phys);
    SOC_PBMP_COUNT(port_phys_in_pm, phys_count);

    /* if first phy is initialized and use it, default is -1 anyways */
    first_phy = PM_4x10_INFO(pm_info)->first_phy;

    i = 0;
    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy) {
        if(SOC_PBMP_MEMBER(port_phys_in_pm, phy)) {
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                             pm_info->wb_vars_ids[ports], &port, i);
            _SOC_IF_ERR_EXIT(rv);
            first_phy = (first_phy == -1) ? phy : first_phy;
            port_index = (port_index == -1 ? i : port_index);
        }
        i++;
    }
    PM_4x10_INFO(pm_info)->first_phy = first_phy;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    if(port_index<0 || port_index>=PM4X10_LANES_PER_CORE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("can't attach port %d. can't find relevant phy"), port));
    }

    /* init data for interface_config and port_init_config */
    sal_memcpy(&PM_4x10_INFO(pm_info)->port_config[port_index].interface_config,
               &add_info->interface_config,
               sizeof(portmod_port_interface_config_t));

    sal_memcpy(&PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config,
               &add_info->init_config,
               sizeof(portmod_port_init_config_t));

    if (add_info->init_config.polarity_overwrite == 1) {
        sal_memcpy(&PM_4x10_INFO(pm_info)->polarity,
            &add_info->init_config.polarity[PM_4x10_INFO(pm_info)->core_num_int],
               sizeof(phymod_polarity_t));
    }
    if (add_info->init_config.lane_map_overwrite == 1) {
        sal_memcpy(&PM_4x10_INFO(pm_info)->lane_map,
            &add_info->init_config.lane_map[PM_4x10_INFO(pm_info)->core_num_int],
               sizeof(phymod_lane_map_t));
    }
    if (add_info->init_config.ref_clk_overwrite == 1) {
        PM_4x10_INFO(pm_info)->ref_clk = add_info->init_config.ref_clk;
    }
    if (add_info->init_config.fw_load_method_overwrite == 1) {
        PM_4x10_INFO(pm_info)->fw_load_method =
            add_info->init_config.fw_load_method[PM_4x10_INFO(pm_info)->core_num_int];
    }

    /* port_index is 0, 1, 2, or 3 */
    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                            pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* if not active - initalize PM */
    if(!pm_is_active){

#ifdef PORTMOD_PM12X10_SUPPORT
        if(PM_4x10_INFO(pm_info)->in_pm12x10) {
            SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy) {break;}
            /* this will be redirected to pm12x10 */
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
            
            if (SOC_IS_APACHE(unit)) {
                rv = pm12x10_xgs_pm4x10_enable(unit, port, phy, 1);
                _SOC_IF_ERR_EXIT(rv);
            } else
#endif
            {
                rv = pm12x10_pm4x10_enable(unit, port, phy, 1);
                _SOC_IF_ERR_EXIT(rv);
            }
        }
#endif /* PORTMOD_PM12X10_SUPPORT */

        rv = _pm4x10_pm_xlport_init (unit, port, pm_info, port_index,
                 phy_acc, add_info->interface_config.interface,
                 add_info->flags & PORTMOD_PORT_ADD_F_EGR_1588_TIMESTAMP_MODE_48BIT ? 1 : 0);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_active = 1;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
             pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    /*probe the core*/
    rv = _pm4x10_pm_core_probe(unit, port, pm_info, add_info);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_port_attach_resume_fw_load (int unit, int port, pm_info_t pm_info, 
                        const portmod_port_add_info_t* add_info)
{
    int        port_index = -1, rv = 0, i, nof_phys, port_i;
    uint32     bitmap;
    int my_i, phy_acc, is_bypass;
    phymod_interface_t          phymod_serdes_interface = phymodInterfaceCount; 
    phymod_phy_init_config_t    *phy_init_config;
    phymod_phy_inf_config_t     *phy_interface_config;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_access_t phy_access_lane;
    portmod_port_ability_t port_ability;
    portmod_access_get_params_t params;

    SOC_INIT_FUNC_DEFS;
    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypass);
    _SOC_IF_ERR_EXIT(rv);
    /* initialze phys */

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
     phy_init_config      = &PM_4x10_INFO(pm_info)->port_config[port_index].phy_init_config;
     phy_interface_config = &PM_4x10_INFO(pm_info)->port_config[port_index].phy_interface_config;


    /* clean up polarity and tx of phy_init_config */
    _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(phy_init_config));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm4x10_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access_lane, &nof_phys, NULL));

    /*init the core*/
    rv = _pm4x10_pm_core_init(unit, pm_info, add_info);
    _SOC_IF_ERR_EXIT(rv);

    /* initalize port */
    rv = _pm4x10_pm_port_init(unit, port, phy_acc, port_index, add_info, 1, pm_info);
    _SOC_IF_ERR_EXIT(rv);

    my_i = 0;
    for(i=0 ; i<PM4X10_LANES_PER_CORE ; i++) {

        rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[ports], &port_i, i);
        _SOC_IF_ERR_EXIT(rv);

        if(port_i != port) {

            continue;
        }

        if(SHR_BITGET(&PM_4x10_INFO(pm_info)->polarity.tx_polarity,i)) {
            SHR_BITSET(&phy_init_config->polarity.tx_polarity, my_i);
        }

        if(SHR_BITGET(&PM_4x10_INFO(pm_info)->polarity.rx_polarity,i)) {
            SHR_BITSET(&phy_init_config->polarity.rx_polarity, my_i);
        }
        my_i++;
    }
    phy_init_config->op_mode = add_info->phy_op_mode;

    _SOC_IF_ERR_EXIT(_pm4x10_port_interface_config_init_set(unit, port,
                          pm_info, &add_info->interface_config));

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                           phy_access ,(1+MAX_PHYN),
                                                           &nof_phys));
    if (nof_phys > 1) {
        
        PORTMOD_USER_ACC_LPORT_SET(&(phy_access[nof_phys-1].access), port);
    }

    if ((is_bypass) && (!PM_4x10_INFO(pm_info)->in_pm12x10)) {
        PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(PM_4x10_INFO(pm_info)->int_core_access.device_op_mode);
    }
    _SOC_IF_ERR_EXIT(portmod_port_phychain_phy_init(phy_access, nof_phys,
                                                            phy_init_config));

    if (add_info->interface_config.serdes_interface != SOC_PORT_IF_COUNT) {
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, 
                                add_info->interface_config.speed, 
                                add_info->interface_config.serdes_interface, 
                                &phymod_serdes_interface));
    }
    _SOC_IF_ERR_EXIT(portmod_port_phychain_interface_config_set(phy_access, nof_phys,
                            PM_4x10_INFO(pm_info)->port_config[port_index].interface_config.flags ,
                            phy_interface_config, phymod_serdes_interface,
                            PM_4x10_INFO(pm_info)->ref_clk,
                            TRUE )); /* Apply to ALL PHY with same interface config */


    if(add_info->init_config.fs_cl72) {
       /* config port_init_cl72 is used to enable fs cl72 only in the serdes,
        do not enable cl72 on external phys. Set the no of phys argument to 1 to avoid setting cl72 enable 
        on ext phys */
       /* need to check if multi core and not pcs by pass mode */
       if ((phy_interface_config->data_rate >= 100000)  &&
           (phy_interface_config->interface_type != phymodInterfaceBypass)) {
            uint32_t temp_flag = PM_4x10_INFO(pm_info)->port_config[port_index].interface_config.flags;
            if (temp_flag == PHYMOD_INTF_F_SET_SPD_NO_TRIGGER) {
                _SOC_IF_ERR_EXIT(portmod_port_phychain_cl72_set(phy_access, 1, 1));
            }
       } else {
                _SOC_IF_ERR_EXIT(portmod_port_phychain_cl72_set(phy_access, 1, 1));
       }
    }
    /* set the default advert ability */
    _SOC_IF_ERR_EXIT
        (pm4x10_port_ability_local_get(unit, port, pm_info, &port_ability));
    _SOC_IF_ERR_EXIT
        (pm4x10_port_ability_advert_set(unit, port, pm_info, &port_ability));

    /* De-Assert SOFT_RESET */
        _SOC_IF_ERR_EXIT(pm4x10_port_soft_reset(unit, port, pm_info, 0));

exit:
    SOC_FUNC_RETURN;

}

STATIC
int _pm4x10_an_mode_update (int unit, int port, pm_info_t pm_info)
{
    portmod_port_init_config_t *init_config;
    uint32 bitmap;
    int    port_index;

    SOC_IF_ERROR_RETURN(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    init_config = &PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config;
    if(init_config->is_hg) {
        init_config->an_mode = phymod_AN_MODE_CL37BAM ;
        if (init_config->an_cl37 == PORTMOD_CL37_W_BAM_HG) {
            init_config->an_mode = phymod_AN_MODE_CL37BAM_10P9375G_VCO;
        }
    } else if (init_config->an_cl73) {
        init_config->an_mode = phymod_AN_MODE_CL73 ;
    } else {
        init_config->an_mode = phymod_AN_MODE_CL37;
        if(init_config->an_cl37 == PORTMOD_CL37_SGMII_COMBO){
           init_config->an_mode = phymod_AN_MODE_CL37_SGMII;
        }
    }

    return (SOC_E_NONE);
}

int pm4x10_port_attach (int unit, int port, pm_info_t pm_info, 
                        const portmod_port_add_info_t* add_info)
{
    int init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                    !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                    !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || (init_all)) {
        SOC_IF_ERROR_RETURN(_pm4x10_port_attach_core_probe (unit, port, pm_info, add_info));
    }

    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
        !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        return (SOC_E_NONE);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || (init_all)) {

        if (!PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_GET(add_info))
        {
            SOC_IF_ERROR_RETURN(_pm4x10_an_mode_update (unit, port, pm_info));
        }
        SOC_IF_ERROR_RETURN(_pm4x10_pm_core_init(unit, pm_info, add_info));
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        return (SOC_E_NONE);
    }

    SOC_IF_ERROR_RETURN(_pm4x10_port_attach_resume_fw_load (unit, port, pm_info, add_info));

    return (SOC_E_NONE);
}


int pm4x10_port_detach(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_inf_config_t phy_interface_config;

    int     enable, tmp_port, i=0, rv=0, port_index = -1, nof_phys;
    int     invalid_port = -1, is_last_one = TRUE, phy_acc;
    uint32  inactive = 0;

    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(pm4x10_port_enable_get(unit, port, pm_info, 0, &enable));
    if(enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
                  (_SOC_MSG("can't detach active port %d"), port));
    }

    /* in case of 100G disable the speed for the port */
    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,(1+MAX_PHYN), &nof_phys));
    _SOC_IF_ERR_EXIT(portmod_port_phychain_interface_config_get(phy_access, nof_phys,
                                   0, PM_4x10_INFO(pm_info)->ref_clk, &phy_interface_config));

    if ((phy_interface_config.interface_type == phymodInterfaceCR10)   ||
        (phy_interface_config.interface_type == phymodInterfaceCAUI)) {
        _SOC_IF_ERR_EXIT(portmod_port_phychain_interface_config_set(phy_access, nof_phys,
                                   PHYMOD_INTF_F_SET_SPD_DISABLE,
                                   &phy_interface_config, phy_interface_config.interface_type, 
                                   PM_4x10_INFO(pm_info)->ref_clk,
                                   PORTMOD_INIT_F_ALL_PHYS ));
    }
    if ((phy_interface_config.interface_type == phymodInterfaceCR10)   ||
        (phy_interface_config.interface_type == phymodInterfaceCAUI)) {
        _SOC_IF_ERR_EXIT(portmod_port_phychain_interface_config_set(phy_access, nof_phys,
                                   PHYMOD_INTF_F_SPEED_CONFIG_CLEAR,
                                   &phy_interface_config, phy_interface_config.interface_type,
                                   PM_4x10_INFO(pm_info)->ref_clk,
                                   PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));
    }


    /*remove from array and check if it was the last one*/
    for( i = 0 ; i < MAX_PORTS_PER_PM4X10; i++) {
       rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, 
                    pm_info->wb_vars_ids[ports], &tmp_port, i);
       _SOC_IF_ERR_EXIT(rv);

       if(tmp_port == port){
           port_index = (port_index == -1 ? i : port_index);
           rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, 
                    pm_info->wb_vars_ids[ports], &invalid_port, i);
           _SOC_IF_ERR_EXIT(rv);
       } else if (tmp_port != -1) {
           is_last_one = FALSE;
       }
    }

    if(port_index == -1) { 
        _SOC_EXIT_WITH_ERR(SOC_E_PORT,(_SOC_MSG("Port %d wasn't found"),port));
    }   

    rv = _pm4x10_pm_port_init(unit, port, phy_acc, port_index, NULL, 0, pm_info);
    _SOC_IF_ERR_EXIT(rv);

    /*deinit PM in case of last one*/
    if (is_last_one) {
       rv = _pm4x10_pm_disable(unit, port, pm_info, phy_acc);
       _SOC_IF_ERR_EXIT(rv);

#ifdef PORTMOD_PM12X10_SUPPORT
       if(PM_4x10_INFO(pm_info)->in_pm12x10) {
           uint32 phy;
           SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy) {break;};
            /* this will be redirected to pm12x10 */
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
            
            if (SOC_IS_APACHE(unit)) {
                rv = pm12x10_xgs_pm4x10_enable(unit, port, phy, 1);
                _SOC_IF_ERR_EXIT(rv);
            } else
#endif
            {
                rv = pm12x10_pm4x10_enable(unit, port, phy, 0);
                _SOC_IF_ERR_EXIT(rv);
            }
        }
#endif /* PORTMOD_PM12X10_SUPPORT */

       rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, 
                             pm_info->wb_vars_ids[isActive], &inactive);
       _SOC_IF_ERR_EXIT(rv);

       rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                                   pm_info->wb_vars_ids[isInitialized],
                                   &inactive);
       _SOC_IF_ERR_EXIT(rv);

       rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                                   pm_info->wb_vars_ids[isBypassed], &inactive);
       _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int i, tmp_port;
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    /* replace old port with new port */
    for (i = 0; i < MAX_PORTS_PER_PM4X10; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, 
                     pm_info->wb_vars_ids[ports], &tmp_port, i);
        _SOC_IF_ERR_EXIT(rv);
       
        if (tmp_port == port){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, 
                             pm_info->wb_vars_ids[ports], &new_port, i);
            _SOC_IF_ERR_EXIT(rv);                
        }
    }
        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_pm_bypass_set(int unit, pm_info_t pm_info, int bypass_enable)
{
    int     rv = 0;
    uint32  pm_is_active = 0;
    uint32 is_bypass;

    SOC_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypass);
    _SOC_IF_ERR_EXIT(rv);

    if (bypass_enable != is_bypass) {
        rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active); 
        _SOC_IF_ERR_EXIT(rv);

        if(!pm_is_active) {
            
            rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &bypass_enable);
            _SOC_IF_ERR_EXIT(rv);
        }
    }


exit:
    SOC_FUNC_RETURN;
}

int pm4x10_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    core_info->ref_clk = PM_4x10_INFO(pm_info)->ref_clk;
    return SOC_E_NONE;
}

int pm4x10_pm_phys_get(int unit, pm_info_t pm_info, soc_pbmp_t* phys)
{
    SOC_INIT_FUNC_DEFS;
    SOC_PBMP_ASSIGN(*phys, PM_4x10_INFO(pm_info)->phys);
    SOC_FUNC_RETURN;
}

int pm4x10_port_enable_set (int unit, int port, pm_info_t pm_info, 
                            int flags, int enable)
{
    uint32  is_bypassed = 0;
    int     rv = 0, rst_flags = 0;
    int     actual_flags = flags, nof_phys = 0, xlmac_flags = 0;
    uint8   in_pm12x10 = PM_4x10_INFO(pm_info)->in_pm12x10;

    phymod_phy_power_t           phy_power;
    phymod_phy_access_t phy_access[1+MAX_PHYN];

    SOC_INIT_FUNC_DEFS;

    /* If no Rx/Tx flags - set both */
    if ((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && 
        (!PORTMOD_PORT_ENABLE_RX_GET(flags)))
    {
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* If no Mac/Phy flags - set both */
    if ((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && 
        (!PORTMOD_PORT_ENABLE_MAC_GET(flags)))
    {
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

     /* if MAC is set and either RX or TX set is invalid combination */
    if( (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) ) {
        if((!PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) || (!PORTMOD_PORT_ENABLE_RX_GET(actual_flags))){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC RX and TX can't be enabled separately")));
        }
    }

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, 
                               pm_info->wb_vars_ids[isBypassed], &is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    phy_power.rx = phymodPowerNoChange;
    phy_power.tx = phymodPowerNoChange;

    if(PORTMOD_PORT_ENABLE_RX_GET(actual_flags)){
        phy_power.rx = (enable) ? phymodPowerOn : phymodPowerOff;
        xlmac_flags |= XLMAC_ENABLE_SET_FLAGS_RX_EN;
    }
    if(PORTMOD_PORT_ENABLE_TX_GET(actual_flags)){
        phy_power.tx = (enable) ? phymodPowerOn : phymodPowerOff;
        xlmac_flags |= XLMAC_ENABLE_SET_FLAGS_TX_EN;
    }

    if(enable){
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)){
              if(PM_4x10_INFO(pm_info)->portmod_mac_soft_reset) {
                rst_flags |= XLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
            }    
            rv = xlmac_enable_set(unit, port, rst_flags, 1);
            _SOC_IF_ERR_EXIT(rv);
              if(PM_4x10_INFO(pm_info)->portmod_mac_soft_reset) {
                _SOC_IF_ERR_EXIT(pm4x10_port_soft_reset(unit, port, pm_info, 0));
            }
        }

        if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags))
        {
            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));
    
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_power_set(phy_access, nof_phys, &phy_power));

            /* bypassed for ILKN */
            if (!is_bypassed || in_pm12x10) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_tx_lane_control_set(phy_access, nof_phys,
                                                               phymodTxSquelchOff));

                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_rx_lane_control_set(phy_access, nof_phys,
                                                              phymodRxSquelchOff));
            }
        }
    } else {
        if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                                  phy_access ,(1+MAX_PHYN),
                                                   &nof_phys));
            /* bypassed for ILKN */
            if (!is_bypassed || in_pm12x10) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_tx_lane_control_set(phy_access, nof_phys,
                                                               phymodTxSquelchOn));
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_rx_lane_control_set(phy_access, nof_phys,
                                                               phymodRxSquelchOn));
            }
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_power_set(phy_access, nof_phys, &phy_power));
        } 

        /* if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))  && (!is_bypassed)){ */
        /* Not checking is_bypassed as if clmac was enable before bypass was set */
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) ){
            rv = xlmac_enable_set(unit, port, xlmac_flags, 0);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_enable_get (int unit, int port, pm_info_t pm_info, int flags, 
                            int* enable)
{
    uint32  is_bypassed = 0;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    phymod_phy_tx_lane_control_t tx_control = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_control = phymodRxSquelchOn;
    int phy_enable = 0;
    int mac_enable = 0;
    
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    if (PORTMOD_PORT_ENABLE_PHY_GET(flags)||(0 == flags)) {

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                              phy_access ,(1+MAX_PHYN),
                                               &nof_phys));

            _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_tx_lane_control_get(phy_access, nof_phys,
                                                               &tx_control));
            _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_rx_lane_control_get(phy_access, nof_phys,
                                                               &rx_control));
        phy_enable = ((tx_control == phymodTxSquelchOn) &&
                     (rx_control == phymodRxSquelchOn)) ? 0 : 1;
        if(0 != flags) {
            *enable = phy_enable;
        }
    } 

    _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, 
                     pm_info->wb_vars_ids[isBypassed], &is_bypassed));

    if ((PORTMOD_PORT_ENABLE_MAC_GET(flags)||(0 == flags)) && (!is_bypassed)) {
        _SOC_IF_ERR_EXIT(xlmac_enable_get(unit, port, 0, &mac_enable));
        if(0 != flags) {
            *enable = mac_enable;
        }
    } else if (PORTMOD_PORT_ENABLE_RX_GET(flags) && (!is_bypassed)) {
        _SOC_IF_ERR_EXIT(xlmac_enable_get(unit, port, XLMAC_ENABLE_SET_FLAGS_RX_EN, enable));
    } else if (PORTMOD_PORT_ENABLE_TX_GET(flags) && (!is_bypassed)) {
        _SOC_IF_ERR_EXIT(xlmac_enable_get(unit, port, XLMAC_ENABLE_SET_FLAGS_TX_EN, enable));
    }
    if(0 == flags){
        *enable = (mac_enable || phy_enable) ;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * portmod_interface_pm_4x10_to_phymod_interface
 *
 * @brief  passing and process data structure from in to out 
 *          
 * @param[in]   interface_config
 * @param[im]   init_config
 *
 * @param[out]  phy_interface_config
 *
 */
STATIC
int portmod_interface_pm_4x10_to_phymod_interface(int unit,
                                                  int port,
                                                  const portmod_port_interface_config_t *interface_config,
                                                  const portmod_port_init_config_t *init_config,
                                                  phymod_phy_inf_config_t *phy_interface_config)
{
    int port_is_higig;
    int port_num_lanes;
    int scr_enabled;
    int fiber_pref;
    int phy_supports_dual_rate;
    soc_port_if_t interface;

    SOC_INIT_FUNC_DEFS;
        
    port_num_lanes         = interface_config->port_num_lanes;
    port_is_higig          = PHYMOD_INTF_MODES_HIGIG_GET(interface_config);
    scr_enabled            = PHYMOD_INTF_MODES_SCR_GET(interface_config);
    fiber_pref             = PHYMOD_INTF_MODES_FIBER_GET(interface_config);
    phy_supports_dual_rate = PHYMOD_INTF_MODES_OS2_GET(interface_config);

    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(interface_config, &interface));
                                                 
    if (interface == _SHR_PORT_IF_ILKN){
        phy_interface_config->interface_type = phymodInterfaceBypass;
    }else{
        switch (interface_config->speed) {
              case 10:
                phy_interface_config->interface_type = phymodInterfaceSGMII;
                break;
            case 100:
                phy_interface_config->interface_type = phymodInterfaceSGMII;
                break;
            case 1000:
                if (interface == SOC_PORT_IF_GMII) {
                    phy_interface_config->interface_type = phymodInterface1000X;
                } else if (interface == SOC_PORT_IF_SGMII) {
                    phy_interface_config->interface_type = phymodInterfaceSGMII;
                } else if (fiber_pref) {
                    phy_interface_config->interface_type = phymodInterface1000X;
                } else if(interface==SOC_PORT_IF_KX) {
                    phy_interface_config->interface_type = phymodInterfaceKX;
                } else {
                    phy_interface_config->interface_type =
                          (phy_interface_config->interface_type == phymodInterfaceKX) ?
                                phymodInterfaceKX : phymodInterfaceSGMII;
                }
                break;
            case 2500:
                if(fiber_pref) {
                    phy_interface_config->interface_type = phymodInterface1000X;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceSGMII;
                }
                break;
            case 5000:
                if (port_num_lanes == 2)
                   phy_interface_config->interface_type = phymodInterfaceRXAUI;
                else
                   phy_interface_config->interface_type = phymodInterfaceXFI;
                break;
            case 3125:
            case 5750:
                phy_interface_config->interface_type = phymodInterfaceBypass;
                break;
            case 6250:
                phy_interface_config->interface_type = phymodInterfaceBypass;
                if(phy_supports_dual_rate) { /* for dual rate OS2 is required */
                    PHYMOD_INTF_MODES_OS2_SET(phy_interface_config);
                }
                break;
            case 8125:
            case 8500:
                phy_interface_config->interface_type = phymodInterfaceBypass;
                break;
            case 10000:
                if (port_num_lanes == 4) {
                    phy_interface_config->interface_type = phymodInterfaceXGMII;
                } else if (port_num_lanes == 2) {
                    if (init_config->rxaui_mode == 0x2)  {
                        phy_interface_config->interface_type = phymodInterfaceRXAUI;
                    }
                    else if (init_config->rxaui_mode)  {
                        phy_interface_config->interface_type = phymodInterfaceX2;
                    } else {
                        phy_interface_config->interface_type = phymodInterfaceRXAUI;
                    }
                    if (scr_enabled) {
                        PHYMOD_INTF_MODES_SCR_SET(phy_interface_config);
                    }
                } else {
                    if( fiber_pref) {
                       if (interface==SOC_PORT_IF_SR) {
                           phy_interface_config->interface_type = phymodInterfaceSR ;
                       } else {
                           phy_interface_config->interface_type = phymodInterfaceSFI ;
                       }
                    } else if(interface==SOC_PORT_IF_SFI) {
                        phy_interface_config->interface_type = phymodInterfaceSFI ;
                    } else if(interface==SOC_PORT_IF_SR) {
                        phy_interface_config->interface_type = phymodInterfaceSR ;
                    } else if(interface==SOC_PORT_IF_CR) {
                        phy_interface_config->interface_type = phymodInterfaceSFPDAC;
                    } else if(interface==SOC_PORT_IF_KR) {
                        phy_interface_config->interface_type = phymodInterfaceKR;
                    } else if(interface==SOC_PORT_IF_LR) {
                        phy_interface_config->interface_type = phymodInterfaceLR;
                    } else if(interface==SOC_PORT_IF_LRM) {
                        phy_interface_config->interface_type = phymodInterfaceLRM;
                    } else if(interface==SOC_PORT_IF_CX) {
                        phy_interface_config->interface_type = phymodInterfaceCX;
                    } else {
                        phy_interface_config->interface_type = phymodInterfaceXFI;
                    }
                }
                break;
            case 10312:
                phy_interface_config->interface_type = phymodInterfaceBypass;
                break;
            case 10500:
                break ;
            case 10937:
                phy_interface_config->interface_type = phymodInterfaceBypass;
                break;
            case 11000:
                if (interface == SOC_PORT_IF_SR) {
                        phy_interface_config->interface_type = phymodInterfaceSR;
                } else if (interface == SOC_PORT_IF_KR) {
                        phy_interface_config->interface_type = phymodInterfaceKR;
                } else if (interface == SOC_PORT_IF_CR) {
                        phy_interface_config->interface_type = phymodInterfaceCR;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceXFI;
                }
                break;
            case 11250:
                phy_interface_config->interface_type = phymodInterfaceBypass;
                break;
            case 11500:
                phy_interface_config->interface_type = phymodInterfaceXFI;
                break;
            case 12000:
                if (port_num_lanes == 4) {
                    phy_interface_config->interface_type = phymodInterfaceXGMII;
                } else if (port_num_lanes == 2) {
                    phy_interface_config->interface_type = phymodInterfaceRXAUI;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceXFI;
                }
                break ;
            case 12500:
                break;
            case 12773:   /*  12733Mbps */
                phy_interface_config->interface_type = phymodInterfaceRXAUI;
                break ;
            case 13000:
                if (port_num_lanes == 4) {
                    phy_interface_config->interface_type = phymodInterfaceXGMII;
                } else if (port_num_lanes == 2) {
                    phy_interface_config->interface_type = phymodInterfaceRXAUI;
                }
                break;
            case 15000:
                phy_interface_config->interface_type = phymodInterfaceXGMII;
                break;
            case 15750:
            case 16000:  /* setting RX66_CONTROL_CC_EN/CC_DATA_SEL failed this 16000 HI_DXGXS mode */
                if (port_num_lanes == 4){
                    phy_interface_config->interface_type = phymodInterfaceXGMII;
                } else if (port_num_lanes == 2) {
                    phy_interface_config->interface_type = phymodInterfaceRXAUI;
                }
                break ;
                /*  WCMOD_SPD_21G_HI_DXGXS ?? */
            case 20000:
                if (port_num_lanes == 4) {
                    if (scr_enabled) {
                        PHYMOD_INTF_MODES_SCR_SET(phy_interface_config);
                    }
                    phy_interface_config->interface_type = phymodInterfaceXGMII;
                } else if(port_num_lanes == 2) {
                    if (port_is_higig) {
                        phy_interface_config->interface_type = phymodInterfaceRXAUI;
                    } else {
                        if( (interface == SOC_PORT_IF_KR2) ||
                            (interface == SOC_PORT_IF_KR) ) {
                            phy_interface_config->interface_type = phymodInterfaceKR2;
                        } else if( (interface == SOC_PORT_IF_CR2) ||
                                   (interface == SOC_PORT_IF_CR) ) {
                            phy_interface_config->interface_type = phymodInterfaceCR2;
                        } else {
                            phy_interface_config->interface_type = phymodInterfaceRXAUI;
                        }
                    }
                }
                break ;
            case 21000:
                if (port_num_lanes == 4) {
                    phy_interface_config->interface_type = phymodInterfaceXGMII;
                }else if(port_num_lanes == 2) {
                    phy_interface_config->interface_type = phymodInterfaceRXAUI;
                }
                break ;
            case 23000:
               if (port_num_lanes == 2) {
                  phy_interface_config->interface_type = phymodInterfaceX2;
               }
               break ;
            case 25000:
            case 25450:
            case 25455:
            case 30000:
            case 31500:
                if (port_num_lanes == 4) {
                    phy_interface_config->interface_type = phymodInterfaceKR4;
                } else if (port_num_lanes == 2) {
                    phy_interface_config->interface_type = phymodInterfaceKR2;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceKR;
                }
                break;
            case 40000:
                if(port_is_higig) {
                    if(interface == SOC_PORT_IF_KR4) {
                        phy_interface_config->interface_type = phymodInterfaceKR4;
                    } else if(interface == SOC_PORT_IF_CR4){
                        phy_interface_config->interface_type = phymodInterfaceCR4;
                    } else {
                        phy_interface_config->interface_type = phymodInterfaceXGMII;
                    }
                } else {
                    if(fiber_pref){
                        PHYMOD_INTF_MODES_FIBER_SET(phy_interface_config);
                    }
                    if(interface == SOC_PORT_IF_SR4){
                        phy_interface_config->interface_type = phymodInterfaceSR4;
                    }else if(interface == SOC_PORT_IF_LR4){
                        phy_interface_config->interface_type = phymodInterfaceLR4;
                    }else if(interface == SOC_PORT_IF_KR4){
                        phy_interface_config->interface_type = phymodInterfaceKR4;
                    }else if(interface == SOC_PORT_IF_CR4){
                        phy_interface_config->interface_type = phymodInterfaceCR4;
                    }else if(interface == SOC_PORT_IF_ER4){
                        phy_interface_config->interface_type = phymodInterfaceER4;
                    }else if(interface == SOC_PORT_IF_XLPPI){
                        phy_interface_config->interface_type = phymodInterfaceXLPPI;
                    }else{
                        phy_interface_config->interface_type = phymodInterfaceXLAUI;
                    }
                }
                break;
            case 42000:
                if (port_is_higig) {
                    if (port_num_lanes == 2) {
                        if(interface == SOC_PORT_IF_KR2) {
                            phy_interface_config->interface_type = phymodInterfaceKR2;
                        } else if (interface == SOC_PORT_IF_SR2) {
                            phy_interface_config->interface_type = phymodInterfaceSR2;
                        } else if (interface == SOC_PORT_IF_CR2) {
                            phy_interface_config->interface_type = phymodInterfaceCR2;
                        } else {
                            phy_interface_config->interface_type = phymodInterfaceXLAUI2;
                        }
                    } else {
                        if(interface == SOC_PORT_IF_KR4) {
                            phy_interface_config->interface_type = phymodInterfaceKR4;
                        } else if (interface == SOC_PORT_IF_SR4) {
                            phy_interface_config->interface_type = phymodInterfaceSR4;
                        } else if (interface == SOC_PORT_IF_CR4) {
                            phy_interface_config->interface_type = phymodInterfaceCR4;
                        } else {
                            phy_interface_config->interface_type = phymodInterfaceXLAUI;
                        }
                    }
                } else {
                    phy_interface_config->interface_type = phymodInterfaceKR4;
                }
                 break;
            case 100000:
                if(interface == SOC_PORT_IF_CR10) {
                    phy_interface_config->interface_type = phymodInterfaceCR10;
                } else if(interface == SOC_PORT_IF_KR10) {
                    phy_interface_config->interface_type = phymodInterfaceKR10;
                } else if(interface == SOC_PORT_IF_LR10) {
                    phy_interface_config->interface_type = phymodInterfaceLR10;
                } else if(interface == SOC_PORT_IF_SR10) {
                    phy_interface_config->interface_type = phymodInterfaceSR10;
                } else if(interface == SOC_PORT_IF_CAUI_C2C) {
                    phy_interface_config->interface_type = phymodInterfaceCAUI4_C2C;
                } else if(interface == SOC_PORT_IF_CAUI_C2M) {
                    phy_interface_config->interface_type = phymodInterfaceCAUI4_C2M;
                } else if(interface == SOC_PORT_IF_VSR) {
                    phy_interface_config->interface_type = phymodInterfaceVSR;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceCAUI;
                }
                break;
            case 106000:
                if(interface == SOC_PORT_IF_CR10) {
                    phy_interface_config->interface_type = phymodInterfaceCR10;
                } else if(interface == SOC_PORT_IF_KR10) {
                    phy_interface_config->interface_type = phymodInterfaceKR10;
                } else if(interface == SOC_PORT_IF_SR4) {
                    phy_interface_config->interface_type = phymodInterfaceSR4;
                } else if(interface == SOC_PORT_IF_KR4) {
                    phy_interface_config->interface_type = phymodInterfaceKR4;
                } else if(interface == SOC_PORT_IF_CR4) {
                    phy_interface_config->interface_type = phymodInterfaceCR4;
                } else if(interface == SOC_PORT_IF_VSR) {
                    phy_interface_config->interface_type = phymodInterfaceVSR;
                } else if(interface == SOC_PORT_IF_CAUI_C2C) {
                    phy_interface_config->interface_type = phymodInterfaceCAUI4_C2C;
                } else if(interface == SOC_PORT_IF_CAUI_C2M) {
                    phy_interface_config->interface_type = phymodInterfaceCAUI4_C2M;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceCAUI;
                }
                break;
            case 120000:
                phy_interface_config->interface_type = phymodInterfaceCAUI;
                break;
            case 127000:
                phy_interface_config->interface_type = phymodInterfaceCAUI;
                break;
            default:
                return SOC_E_PARAM;
        }
    }

exit:
     SOC_FUNC_RETURN;
 
}

/*!
 * portmod_interface_pm_4x10_xphy_to_phymod_interface
 *
 * @brief  passing and process data structure from in to out 
 *          
 * @param[in]   interface_config
 * @param[im]   init_config
 *
 * @param[out]  phy_interface_config
 *
 */
STATIC
int portmod_interface_pm_4x10_xphy_to_phymod_interface(int unit,
                                                  int port,
                                                  const portmod_port_interface_config_t *interface_config,
                                                  const portmod_port_init_config_t *init_config,
                                                  phymod_phy_inf_config_t *phy_interface_config)
{
    int port_is_higig;
    int port_num_lanes;
    int scr_enabled;
    int fiber_pref;
    soc_port_if_t interface;

    SOC_INIT_FUNC_DEFS;

    port_num_lanes          = interface_config->port_num_lanes;
    port_is_higig           = PHYMOD_INTF_MODES_HIGIG_GET(interface_config);
    scr_enabled             = PHYMOD_INTF_MODES_SCR_GET(interface_config);
    fiber_pref              = PHYMOD_INTF_MODES_FIBER_GET(interface_config);

    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(interface_config, &interface));

    switch (interface_config->speed) {
#ifdef PHYMOD_DINO_SUPPORT
        case 1000:
            if (interface==SOC_PORT_IF_XFI){
                phy_interface_config->interface_type = phymodInterfaceXFI;
            } else if (interface==SOC_PORT_IF_SFI){
                phy_interface_config->interface_type = phymodInterfaceSFI;
            } else if (interface==SOC_PORT_IF_CX){
                phy_interface_config->interface_type = phymodInterfaceCX;
            } else if (interface==SOC_PORT_IF_CR){
                phy_interface_config->interface_type = phymodInterfaceCR;
            } else if (interface==SOC_PORT_IF_KR){
                phy_interface_config->interface_type = phymodInterfaceKR;
            } else{
                phy_interface_config->interface_type = phymodInterfaceXFI;
            }
            break;
#endif /* PHYMOD_DINO_SUPPORT */
        case 10000:
            if (port_num_lanes == 4) {
                phy_interface_config->interface_type = phymodInterfaceXGMII;
            } else if (port_num_lanes == 2) {
                if (init_config->rxaui_mode == 0x2)  {
                    phy_interface_config->interface_type = phymodInterfaceRXAUI;
                }
                else if (init_config->rxaui_mode)  {
                    phy_interface_config->interface_type = phymodInterfaceX2;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceRXAUI;
                }
                if (scr_enabled) {
                    PHYMOD_INTF_MODES_SCR_SET(phy_interface_config);
                }
            } else {
                if(interface==SOC_PORT_IF_SFI)  {
                    phy_interface_config->interface_type = phymodInterfaceSFI ;
                } else if(interface==SOC_PORT_IF_XLPPI) {
                    phy_interface_config->interface_type = phymodInterfaceXLPPI;
                } else if(interface==SOC_PORT_IF_CR) {
                    phy_interface_config->interface_type = phymodInterfaceCR;
                } else if(interface==SOC_PORT_IF_SR) {
                    phy_interface_config->interface_type = phymodInterfaceSR;
                } else if(interface==SOC_PORT_IF_KR) {
                    phy_interface_config->interface_type = phymodInterfaceKR;
                } else if(interface==SOC_PORT_IF_LR) {
                    phy_interface_config->interface_type = phymodInterfaceLR;
                } else if(interface==SOC_PORT_IF_ZR) {
                    phy_interface_config->interface_type = phymodInterfaceZR;
                } else if(interface==SOC_PORT_IF_LRM) {
                    phy_interface_config->interface_type = phymodInterfaceLRM;
                } else if(interface==SOC_PORT_IF_ER) {
                    phy_interface_config->interface_type = phymodInterfaceER;
                } else if(interface==SOC_PORT_IF_ER2) {
                    phy_interface_config->interface_type = phymodInterfaceER2;
                } else if(interface==SOC_PORT_IF_CX) {
                    phy_interface_config->interface_type = phymodInterfaceCX;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceXFI;
                }        
            }
            break;
        case 40000:
            if(port_is_higig) {
                if(interface ==  SOC_PORT_IF_KR4){
                    phy_interface_config->interface_type = phymodInterfaceKR4;
                } else if(interface == SOC_PORT_IF_CR4) {
                    phy_interface_config->interface_type = phymodInterfaceCR4;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceXGMII;
                }
            }else if(fiber_pref){
                PHYMOD_INTF_MODES_FIBER_SET(phy_interface_config);
                phy_interface_config->interface_type = phymodInterfaceKR4;
            }else if(interface == SOC_PORT_IF_SR4){
                phy_interface_config->interface_type = phymodInterfaceSR4;
            }else if(interface == SOC_PORT_IF_ER4){
                phy_interface_config->interface_type = phymodInterfaceER4;
            }else if(interface == SOC_PORT_IF_CR4){
                phy_interface_config->interface_type = phymodInterfaceCR4;
            }else if(interface == SOC_PORT_IF_LR4){
                phy_interface_config->interface_type = phymodInterfaceLR4;
            }else if(interface == SOC_PORT_IF_KR4){
                phy_interface_config->interface_type = phymodInterfaceKR4;
            }else if(interface == SOC_PORT_IF_SR2){
                phy_interface_config->interface_type = phymodInterfaceSR2;
            }else if(interface == SOC_PORT_IF_CR2){
                phy_interface_config->interface_type = phymodInterfaceCR2;
            }else if(interface == SOC_PORT_IF_LR2){
                phy_interface_config->interface_type = phymodInterfaceLR2;
            }else if(interface == SOC_PORT_IF_KR2){
                phy_interface_config->interface_type = phymodInterfaceKR2;
            }else if(interface==SOC_PORT_IF_XLPPI) {
                phy_interface_config->interface_type = phymodInterfaceXLPPI;
            }else if(interface==SOC_PORT_IF_XLAUI2) {
                phy_interface_config->interface_type = phymodInterfaceXLAUI2;
            } else{
                phy_interface_config->interface_type = phymodInterfaceXLAUI;
            }
            break;
        case 100000:
            if(interface == SOC_PORT_IF_SR4){
                phy_interface_config->interface_type = phymodInterfaceSR4;
            } else if(interface == SOC_PORT_IF_LR4){
                phy_interface_config->interface_type = phymodInterfaceLR4;
            } else if(interface == SOC_PORT_IF_KR4){
                phy_interface_config->interface_type = phymodInterfaceKR4;
            } else if(interface == SOC_PORT_IF_CR4){
                phy_interface_config->interface_type = phymodInterfaceCR4;
            } else if(interface == SOC_PORT_IF_ER4){
                phy_interface_config->interface_type = phymodInterfaceER4;
            } else if(interface == SOC_PORT_IF_VSR){
                phy_interface_config->interface_type = phymodInterfaceVSR;
            } else if(interface == SOC_PORT_IF_CAUI_C2C) {
                phy_interface_config->interface_type = phymodInterfaceCAUI4_C2C;
            } else if(interface == SOC_PORT_IF_CAUI_C2M) {
                phy_interface_config->interface_type = phymodInterfaceCAUI4_C2M;
            } else if(interface == SOC_PORT_IF_CR10) {
                phy_interface_config->interface_type = phymodInterfaceCR10;
            } else if(interface == SOC_PORT_IF_KR10) {
                phy_interface_config->interface_type = phymodInterfaceKR10;
            } else if(interface == SOC_PORT_IF_LR10) {
                phy_interface_config->interface_type = phymodInterfaceLR10;
            } else if(interface == SOC_PORT_IF_SR10) {
                phy_interface_config->interface_type = phymodInterfaceSR10;
            } else {
                phy_interface_config->interface_type = phymodInterfaceCAUI;
            }
            break;

        default:
            return SOC_E_PARAM;
    }

exit:
    SOC_FUNC_RETURN;
 
}

int pm4x10_port_interface_config_set(int unit, int port, 
                        pm_info_t pm_info, 
                        const portmod_port_interface_config_t* config,
                        int phy_init_flags)
{
    uint32 reg_val;
    uint32 pm_is_bypassed = 0, pm_is_initialized = 0; 
    int    nof_phys = 0, rv = 0, phy_acc, flags = 0;
    phymod_phy_access_t phy_access_arr[1+MAX_PHYN];
    phymod_phy_access_t         phy_access;
    phymod_phy_inf_config_t     phy_interface_config;
    portmod_access_get_params_t params;
    portmod_port_init_config_t *init_config;
    phymod_phy_init_config_t   *phy_init_config;
    int                         port_index;
    uint32_t                    bitmap;
    portmod_port_interface_config_t *interface_config ;
    portmod_port_interface_config_t  config_temp ;
    phymod_interface_t   phymod_interface;
    phymod_interface_t   phymod_serdes_interface = phymodInterfaceCount;
    phymod_tx_t          updated_tx_params[PHYMOD_MAX_LANES_PER_CORE];
    soc_port_if_t        interface = SOC_PORT_IF_NULL;
    int is_legacy_phy = 0;
    int i;
    int is_higig;
    SOC_INIT_FUNC_DEFS;

    /* 
     * Ignore the call to interface config set if the pm is not iniialized.
     * This happens if the legacy external phy does a call back prior to initializing
     * and probing internal phys 
     */  
    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD, 
                                pm_info->wb_vars_ids[isInitialized], 
                                &pm_is_initialized);
    _SOC_IF_ERR_EXIT(rv);
    if (!pm_is_initialized) return (SOC_E_NONE);


    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
    init_config          = &PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config;
    phy_init_config      = &PM_4x10_INFO(pm_info)->port_config[port_index].phy_init_config;
    interface_config     = &PM_4x10_INFO(pm_info)->port_config[port_index].interface_config;

    is_higig =  ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
                 (config->encap_mode == SOC_ENCAP_HIGIG) ||
                 PHYMOD_INTF_MODES_HIGIG_GET(config)) ? 1 : 0;

    if (config->flags & PHYMOD_INTF_F_UPDATE_SPEED_LINKUP) {
        int mac_speed = 0, serdes_speed = 0, mac_only = 0;
        int nof_lanes = SOC_INFO(unit).port_num_lanes[port];

        _SOC_IF_ERR_EXIT(portmod_port_speed_get(unit, port, &serdes_speed));
        if (config->speed == serdes_speed) {
            xlmac_speed_get(unit, port, &mac_speed);
            mac_speed *= nof_lanes;
            if (config->speed == mac_speed) {
                SOC_FUNC_RETURN;
            }

            mac_only = 1;
        }

        flags = XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
        rv = xlmac_speed_set(unit, port, flags, config->speed);

        if (mac_only) {
            /* line side speed and SerDes speed are equal, no need update SerDes configuration */
            SOC_FUNC_RETURN;
        }

        phy_init_flags = PORTMOD_INIT_F_ALL_PHYS;
    }

    interface_config->port_refclk_int       = PM_4x10_INFO(pm_info)->ref_clk;

    if ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
        (config->encap_mode == SOC_ENCAP_HIGIG)  ||
        (PHYMOD_INTF_MODES_HIGIG_GET(config))) {
        PHYMOD_INTF_MODES_HIGIG_SET(interface_config);
    } else if ((config->encap_mode == SOC_ENCAP_IEEE)) {
        PHYMOD_INTF_MODES_HIGIG_CLR(interface_config);
    }

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    params.lane = -1;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT(pm4x10_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));

    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD, 
                                pm_info->wb_vars_ids[isBypassed], 
                                &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    /* 
     * if the interface is already set with PARAM_SET_ONLY
     * h/w interface needs to be updated during speed set. 
     * So load the interface information and clear the flag;
     */ 
    if(interface_config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
        ((portmod_port_interface_config_t*)config)->interface = interface_config->interface;
        interface_config->flags &= ~PHYMOD_INTF_F_INTF_PARAM_SET_ONLY;
    }
    interface_config->interface = config->interface;
    if(config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
        /* Verify if the interface is correct before saving it */
        _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed, interface,
                                              &phymod_interface));

        /* only flag saved by interface_config->flags */
        interface_config->flags     = PHYMOD_INTF_F_INTF_PARAM_SET_ONLY;
        SOC_FUNC_RETURN;
    }

    if (!(config->flags & (PHYMOD_INTF_F_SET_SPD_TRIGGER | PHYMOD_INTF_F_UPDATE_SPEED_LINKUP)) && (!pm_is_bypassed)){

        /* set HiG mode */
        _SOC_IF_ERR_EXIT(READ_XLPORT_CONFIGr(unit, phy_acc, &reg_val));
        soc_reg_field_set (unit, XLPORT_CONFIGr, &reg_val, HIGIG2_MODEf, 
                           PHYMOD_INTF_MODES_HIGIG_GET(interface_config));
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_CONFIGr(unit, phy_acc, reg_val));

        /* set port speed */
        flags = XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
        rv = xlmac_speed_set(unit, port, flags, config->speed);
        _SOC_IF_ERR_EXIT(rv);

        /* set encapsulation */
        flags = XLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS;
        rv = xlmac_encap_set (unit, port, flags, config->encap_mode);
        _SOC_IF_ERR_EXIT(rv);

        rv = xlmac_strict_preamble_set(unit, port, (!is_higig && (config->speed >= 10000)));
        _SOC_IF_ERR_EXIT(rv);

        /* De-Assert SOFT_RESET */
        rv = pm4x10_port_soft_reset(unit, port, pm_info, 0);
        _SOC_IF_ERR_EXIT(rv);
    }

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (&phy_interface_config));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,  config->speed,
                    config->interface, &phy_interface_config.interface_type));

    phy_interface_config.data_rate       = config->speed;
    phy_interface_config.interface_modes = config->interface_modes;
    phy_interface_config.pll_divider_req = config->pll_divider_req;
    phy_interface_config.ref_clock       = PM_4x10_INFO(pm_info)->ref_clk;

    /*
     * copy const *config to config_temp cause
     * config.interface may come from storage
     */
    _SOC_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, &config_temp));
    sal_memcpy(&config_temp, config, sizeof(portmod_port_interface_config_t));

    if(interface_config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
        /* overwrite config->interface form storage */
        config_temp.interface                = interface_config->interface ;
        interface_config->flags              = 0 ;
    }

    if(init_config->fs_cl72) {
        /* PHYMOD_INTF_F_CL72_REQUESTED_BY_CNFG config is only honoured by tsce. incase if the external phys start
        using this flag, cl72 should not be set for the external phys. Because 
        port_init_cl72 should enable cl72 only on  the serdes */
        config_temp.flags |= PHYMOD_INTF_F_CL72_REQUESTED_BY_CNFG ;
    }

    portmod_interface_pm_4x10_to_phymod_interface(unit, port, 
                                        &config_temp, init_config, &phy_interface_config);

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access_arr ,(1+MAX_PHYN),
                                           &nof_phys));

    if( nof_phys > 1 ){
        /* external phy interface translation. */
        portmod_interface_pm_4x10_xphy_to_phymod_interface(unit, port,
                                        &config_temp, init_config, &phy_interface_config);
        /* external phy preemphasis and current */
        if (!(config->flags & (PHYMOD_INTF_F_SET_SPD_TRIGGER | PHYMOD_INTF_F_UPDATE_SPEED_LINKUP))) {
            is_legacy_phy = ((portmod_default_user_access_t *)(phy_access_arr[nof_phys-1].access.user_acc))->is_legacy_phy_present;
            if (!is_legacy_phy) {
                _SOC_IF_ERR_EXIT(portmod_port_phychain_tx_set(&phy_access_arr[nof_phys-1], 1, phy_init_config->ext_phy_tx));
            }
        }
    } else {
        portmod_interface_pm_4x10_to_phymod_interface(unit, port, 
                                        &config_temp, init_config, &phy_interface_config);
    }

    /* internal phy preemphasis and current */
    if (!(config->flags & (PHYMOD_INTF_F_SET_SPD_TRIGGER | PHYMOD_INTF_F_UPDATE_SPEED_LINKUP))) {
        /* if the tx params are updated from non default value read the non dflt value from the HW and update it */
        if (PORTMOD_PORT_IS_DEFAULT_TX_PARAMS_UPDATED(PM_4x10_INFO(pm_info)->port_config[port_index].port_dynamic_state)) {
            for(i=0; i<PHYMOD_MAX_LANES_PER_CORE; i++){
                phymod_tx_t_init(&(updated_tx_params[i]));
            }
            _SOC_IF_ERR_EXIT(portmod_port_phychain_tx_get(phy_access_arr, 1, updated_tx_params));
            _SOC_IF_ERR_EXIT(portmod_port_phychain_tx_set(phy_access_arr, 1, updated_tx_params));
        } else {
            _SOC_IF_ERR_EXIT(portmod_port_phychain_tx_set(phy_access_arr, 1, phy_init_config->tx));
        }
    }

    if (config->serdes_interface != SOC_PORT_IF_COUNT) {
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, 
                        config->speed,  
                        config->serdes_interface, 
                        &phymod_serdes_interface));
    }
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_set(phy_access_arr, nof_phys,
                                                config_temp.flags,
                                                &phy_interface_config, 
                                                phymod_serdes_interface, 
                                                PM_4x10_INFO(pm_info)->ref_clk,
                                                phy_init_flags));
    interface_config->speed = config->speed;  /* update the new speed */ 

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_interface_config_get (int unit, int port, pm_info_t pm_info, 
                                      portmod_port_interface_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_inf_config_t      phy_interface_config;
    portmod_port_interface_config_t *interface_config ;
    int                          port_index;
    uint32_t                     bitmap;
    int flags=0, nof_phys = 0, rv = 0,  ref_clock ;
    phymod_interface_t           prev_phymod_if;
    
    SOC_INIT_FUNC_DEFS;
    
    ref_clock = PM_4x10_INFO(pm_info)->ref_clk;

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
    interface_config     = &PM_4x10_INFO(pm_info)->port_config[port_index].interface_config;

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_interface_config));

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    phy_interface_config.data_rate =  interface_config->max_speed;
    phy_interface_config.ref_clock =  ref_clock;

    _SOC_IF_ERR_EXIT( portmod_intf_to_phymod_intf(unit, interface_config->max_speed, 
                                                  interface_config->interface,
                                                  &prev_phymod_if));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_get(phy_access, nof_phys,
                                                    0, ref_clock,
                                                    &phy_interface_config));

    /* portmod_port_phychain_interface_config_get() cannot distinguish among 10G SFI/SR/LR.
       Following section of code classifies the 10G optical interface by reading the previously
       set PHYMOD interface. */
    if( (phy_interface_config.data_rate == 12120 || phy_interface_config.data_rate == 10000 || 
                phy_interface_config.data_rate == 11000)
             && phy_interface_config.interface_type == phymodInterfaceSFI){
        if(prev_phymod_if == phymodInterfaceSR)
            phy_interface_config.interface_type = phymodInterfaceSR;
        else if(prev_phymod_if == phymodInterfaceLR)
            phy_interface_config.interface_type = phymodInterfaceLR;
    }

    /* portmod_port_phychain_interface_config_get() cannot distinguish among 10G CR.
       Following section of code classifies the 10G copper interface by reading the previously
       set PHYMOD interface. */
    if((phy_interface_config.data_rate == 10000)
             && phy_interface_config.interface_type == phymodInterfaceXFI){
        if(prev_phymod_if == phymodInterfaceCR)
            phy_interface_config.interface_type = phymodInterfaceCR;
        else if (prev_phymod_if == phymodInterfaceKR)
            phy_interface_config.interface_type = phymodInterfaceKR;
    }

    /* portmod_port_phychain_interface_config_get() cannot distinguish among 40G SR4/LR4.
       Following section of code classifies the 40G optical interface by reading the previously
       set PHYMOD interface. */
    if ((phy_interface_config.data_rate == 40000) &&
         phy_interface_config.interface_type == phymodInterfaceXGMII){
        if(prev_phymod_if == phymodInterfaceSR4)
            phy_interface_config.interface_type = phymodInterfaceSR4;
        else if(prev_phymod_if == phymodInterfaceLR4)
            phy_interface_config.interface_type = phymodInterfaceLR4;
    }

    config->speed           = phy_interface_config.data_rate;
    config->interface_modes = phy_interface_config.interface_modes;
    config->flags           = 0;
    config->interface       = interface_config->interface;
    config->serdes_interface   = interface_config->serdes_interface;

#if 1
    rv = portmod_intf_from_phymod_intf (unit, 
             phy_interface_config.interface_type, &(config->interface));
    _SOC_IF_ERR_EXIT(rv);
#endif

    rv = xlmac_encap_get (unit, port, &flags, &config->encap_mode); 
    _SOC_IF_ERR_EXIT(rv);
                          
    config->port_num_lanes  = interface_config->port_num_lanes;
    config->max_speed       = interface_config->max_speed;
    config->pll_divider_req = interface_config->pll_divider_req;
    config->interface_modes = interface_config->interface_modes;
    config->port_op_mode    = interface_config->port_op_mode;

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_cl72_set(phy_access, nof_phys, enable));

exit:
    SOC_FUNC_RETURN;
}
int pm4x10_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_cl72_get(phy_access, nof_phys, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_cl72_status_get(int unit, int port, pm_info_t pm_info, phymod_cl72_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_cl72_status_get(phy_access, nof_phys, status));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_loopback_get(int unit, int port, pm_info_t pm_info, 
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
            _SOC_IF_ERR_EXIT(xlmac_loopback_get(unit, port, enable));
            break;

        case portmodLoopbackPhyRloopPCS:
        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPMD: /*slide*/
        case portmodLoopbackPhyGloopPCS: /*slide*/
             if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
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

int pm4x10_port_loopback_set (int unit, int port, pm_info_t pm_info, 
                  portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    phymod_loopback_mode_t phymod_lb_type;

    SOC_INIT_FUNC_DEFS;

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(xlmac_loopback_set(unit, port, enable));
        break;

    case portmodLoopbackPhyRloopPCS:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyGloopPCS: /*slide*/
        if(PM_4x10_INFO(pm_info)->nof_phys != 0) {
            if (enable) {
                _SOC_IF_ERR_EXIT(xlmac_loopback_set(unit, port, 0));
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

int pm4x10_port_rx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(xlmac_rx_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_rx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(xlmac_rx_enable_get(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_tx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(xlmac_tx_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_tx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(xlmac_tx_enable_get(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_ability_local_get (int unit, int port, pm_info_t pm_info, 
                                   portmod_port_ability_t* ability)
{
    portmod_port_interface_config_t *interface_config;
    portmod_port_ability_t legacy_phy_ability;
    int                 fiber_pref, port_index;
    int                 max_speed = 11000;
    int                 num_of_lanes = 0;
    uint32              bitmap, tmpbit ;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;

    SOC_INIT_FUNC_DEFS;

    sal_memset(ability, 0, sizeof(portmod_port_ability_t));
    sal_memset(&legacy_phy_ability, 0, sizeof(portmod_port_ability_t));

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,
                                           (1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT
        (_pm4x10_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    
    interface_config = &(PM_4x10_INFO(pm_info)->port_config[port_index].interface_config);

    fiber_pref        = PHYMOD_INTF_MODES_FIBER_GET(interface_config);
    max_speed         = interface_config->max_speed;
    tmpbit = bitmap ;
    while(tmpbit) {
        num_of_lanes += ((tmpbit&1)?1:0) ;
        tmpbit >>= 1 ;
    }

    ability->loopback  = SOC_PA_LB_PHY | SOC_PA_LB_MAC;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->fec       = SOC_PA_FEC_NONE | SOC_PA_FEC_CL74;
    ability->flags     = SOC_PA_AUTONEG;
    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->encap     = SOC_PA_ENCAP_IEEE | SOC_PA_ENCAP_HIGIG | SOC_PA_ENCAP_HIGIG2;

    if (num_of_lanes == 1) {
        if (PHYMOD_INTF_MODES_HIGIG_GET(interface_config)) {
            switch(max_speed) {
                case 10000: max_speed = 11000 ; break ;
                default:  break;
            }
            interface_config->max_speed = max_speed;
        } else {
            /* 10M, 100M, 1G, 2.5G, 5G don't have equivalent HiGig speeds
               Should only be supported on a non-HiGig port
             */
            if (!fiber_pref) {
                ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
            }
            ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;

            switch(max_speed) {
                case 11000: max_speed = 10000; break;
                default:  break;
            }
            interface_config->max_speed = max_speed;

            /* Must include all supported speeds */
            switch(max_speed) {
                case 12000:
                case 11000:
                case 10000:
                case 5000:
                    ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
                case 2500:
                    /* Need to revisit: 2500MB for copper mode */
                    ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
                default: break;
            }
        }
        switch(max_speed) {  /* Must include all the supported speeds */
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
            case 11000:
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
            case 10000:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        if (max_speed >= 10000) {
            ability->interface |= SOC_PA_INTF_XGMII;
        }
        ability->medium    = SOC_PA_MEDIUM_FIBER;

        /* single lane port */
        ability->flags = SOC_PA_AUTONEG;
    } else if (num_of_lanes == 2) {
        if(PHYMOD_INTF_MODES_HIGIG_GET(interface_config)) {
            switch(max_speed) {
                case 10000: max_speed = 11000 ; break ;
                case 20000: max_speed = 21000 ; break ;
                default:  break ;
            }
            interface_config->max_speed = max_speed;
        } else {
            switch(max_speed) {
                case 11000: max_speed = 10000 ; break ;
                case 21000: max_speed = 20000 ; break ;
                default:  break ;
            }
            interface_config->max_speed = max_speed;
        }
        switch(max_speed) {  /* must include all the supported speedss */
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
            case 16000: 
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB; 
            case 15000:
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
            case 13000:
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
            case 11000:
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
            case 10000:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->flags = 0;
    } else {
        if(PHYMOD_INTF_MODES_HIGIG_GET(interface_config)) {
            switch(max_speed) {
                case 10000: max_speed = 11000 ; break ;
                case 20000: max_speed = 21000 ; break ;
                case 40000: max_speed = 42000 ; break ;
                case 100000: max_speed = 106000 ; break ;
                case 120000: max_speed = 127000 ; break ;
                default:  break ;
            }
            interface_config->max_speed = max_speed;
        } else {
            switch(max_speed) {
                case 11000: max_speed = 10000 ; break ;
                case 21000: max_speed = 20000 ; break ;
                case 42000: max_speed = 40000 ; break ;
                case 106000: max_speed = 100000 ; break ;
                case 127000: max_speed = 120000 ; break ;
                default:  break ;
            }
            interface_config->max_speed = max_speed;
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
                break; /* PM12x10 with lanes >=10 only supports above speeds */
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
            case 16000:
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB; 
            case 15000:
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB; 
            case 13000:
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
            case 11000:
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
            case 10000:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->flags     = SOC_PA_AUTONEG;
    }

    /* If legacy external phy present retrieve the local ability from the
     * external phy as it has more variants and the ability might vary for each */
    if (nof_phys > 1) {
        /* check if the legacy phy present */
        if(PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[nof_phys-1].access))) {
            /* call portmod phy chain function to retrieve the ability */
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_local_ability_get(phy_access,
                                                         nof_phys, &legacy_phy_ability));
            /* PHY driver returns all the speeds supported by PHY
             * Advertise only the speeds supported by both PortMacro and the PHY
             */
            legacy_phy_ability.speed_full_duplex &= ability->speed_full_duplex;
            *ability = legacy_phy_ability;
        } else {
            /* We might need to consider calling portmod phy chain for
             * non legacy phys as well*/
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "%-22s:unit=%d p=%d sp=%08x bitmap=%x\n"),
              __FUNCTION__, unit, port, ability->speed_full_duplex, bitmap));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_autoneg_set (int unit, int port, pm_info_t pm_info, 
                             const phymod_autoneg_control_t* an)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32_t bitmap, an_done;
    phymod_autoneg_control_t  *pAn = (phymod_autoneg_control_t*)an;
    portmod_port_init_config_t *init_config;

    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        if (pAn->num_lane_adv == 0) {
            pAn->num_lane_adv = _pm4x10_nof_lanes_get(unit, port, pm_info);
        }
        
        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            pAn->an_mode = PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config.an_mode;
        }

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            pAn->an_mode = phymod_AN_MODE_SGMII; 
        }

        if (PORTMOD_PORT_IS_AUTONEG_MODE_UPDATED(PM_4x10_INFO(pm_info)->port_config[port_index].port_dynamic_state))  {
            pAn->an_mode = PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config.an_mode;
            PORTMOD_PORT_AUTONEG_MODE_UPDATED_CLR(PM_4x10_INFO(pm_info)->port_config[port_index].port_dynamic_state);  
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

        init_config = &PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config;
        if(init_config->an_cl37 == PORTMOD_CL37_SGMII_COMBO){
           if(pAn->enable){
              init_config->cl37_sgmii_war = 1;
           } else{
              init_config->cl37_sgmii_war = 0;
           }
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_set(phy_access, nof_phys, an));
    }
     
exit:
    SOC_FUNC_RETURN;
 
}

int pm4x10_port_autoneg_get (int unit, int port, pm_info_t pm_info, 
                             phymod_autoneg_control_t* an)
{      
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int         nof_phys;
    uint32      an_done;
    int port_index;
    uint32_t bitmap;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
    
    if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        
        if (an->num_lane_adv == 0) {
            an->num_lane_adv = _pm4x10_nof_lanes_get(unit, port, pm_info);
        }
        an->an_mode = PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config.an_mode;

        if (an->an_mode == phymod_AN_MODE_NONE) {
            an->an_mode = phymod_AN_MODE_SGMII; 
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                              phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_get(phy_access, nof_phys, an, &an_done));
    }

exit:
    SOC_FUNC_RETURN;
 
}

int pm4x10_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status)
{   
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    
    SOC_INIT_FUNC_DEFS; 

    if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Autoneg isn't supported")));
    } else {
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_status_get(phy_access, nof_phys, an_status));
    }

exit:
    SOC_FUNC_RETURN;
    
}

int pm4x10_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int sys_start_lane=-1;
    int sys_end_lane=-1;
    int line_start_lane=-1;
    int line_end_lane=-1;
    uint32 int_lane_mask=0xf;
    uint32 bitmap;
    int    port_index;
    portmod_port_init_config_t *init_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT(_pm4x10_port_tricore_lane_info_get(unit, port, pm_info,
                                                        &sys_start_lane, &sys_end_lane,
                                                        &line_start_lane, &line_end_lane,
                                                        &int_lane_mask));
    /* only look at the lanes that are used. */
    phy_access[0].access.lane_mask &= int_lane_mask;

    _SOC_IF_ERR_EXIT
            (portmod_port_phychain_link_status_get(phy_access, nof_phys, (uint32_t*) link));

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    init_config = &PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config;
    if(*link == 0){
       if(init_config->cl37_sgmii_war){
          if(++init_config->cl37_sgmii_cnt >= init_config->cl37_sgmii_RESTART_CNT){
             init_config->cl37_sgmii_cnt = 0;
             _SOC_IF_ERR_EXIT
                (portmod_port_phychain_autoneg_restart_set(phy_access,nof_phys));
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_link_latch_down_get(int unit, int port, pm_info_t pm_info, uint32 flags, int* link)
{
    int first_index, rv;
    uint32 reg32_val, bitmap;
    soc_field_t status_field, clear_field;
    SOC_INIT_FUNC_DEFS;
    
    *link = 0;
    
    rv = _pm4x10_port_index_get(unit, port, pm_info, &first_index, &bitmap);
    _SOC_IF_ERR_EXIT(rv);

     switch(first_index) {
        case 0:
            status_field = PORT0_LINKSTATUSf;
            clear_field = PORT0_LINKDOWN_CLEARf;
            break;

        case 1:
            status_field = PORT1_LINKSTATUSf;
            clear_field = PORT1_LINKDOWN_CLEARf;
            break;

        case 2:
            status_field = PORT2_LINKSTATUSf;
            clear_field = PORT2_LINKDOWN_CLEARf;
            break;

        case 3:
            status_field = PORT3_LINKSTATUSf;
            clear_field = PORT3_LINKDOWN_CLEARf;
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                     (_SOC_MSG("Port %d, failed to get port index"), port));
    }

    rv = READ_XLPORT_LINKSTATUS_DOWNr(unit, port, &reg32_val);
    _SOC_IF_ERR_EXIT(rv);

    (*link) = soc_reg_field_get(unit, XLPORT_LINKSTATUS_DOWNr, 
                                        reg32_val, status_field);

    if (PORTMOD_PORT_LINK_LATCH_DOWN_F_CLEAR & flags) {

        rv = READ_XLPORT_LINKSTATUS_DOWN_CLEARr(unit, port, &reg32_val);
        _SOC_IF_ERR_EXIT(rv);

        soc_reg_field_set(unit, XLPORT_LINKSTATUS_DOWN_CLEARr, 
                                        &reg32_val, clear_field, 1);

        rv = WRITE_XLPORT_LINKSTATUS_DOWN_CLEARr(unit, port, reg32_val);
        _SOC_IF_ERR_EXIT(rv);

        sal_usleep(10);

        soc_reg_field_set(unit, XLPORT_LINKSTATUS_DOWN_CLEARr, 
                                        &reg32_val, clear_field, 0);

        /* reset default register value */
        rv = WRITE_XLPORT_LINKSTATUS_DOWN_CLEARr(unit, port, reg32_val);
        _SOC_IF_ERR_EXIT(rv);

    }

        
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _rv = portmod_port_phychain_phy_link_up_event(phy_access, nof_phys);
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    
    _rv = portmod_port_phychain_phy_link_down_event(phy_access, nof_phys);

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_mac_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    uint32 reg_val, bitmap;
    int port_index, phy_acc;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index, 
                                             &bitmap));

    switch(port_index) {
        case 0:
            _SOC_IF_ERR_EXIT(READ_XLPORT_XGXS0_LN0_STATUS0_REGr(unit, phy_acc, 
                                        &reg_val));
            (*link) = soc_reg_field_get(unit, XLPORT_XGXS0_LN0_STATUS0_REGr, 
                                        reg_val, LINK_STATUSf);
            break;

        case 1:
            _SOC_IF_ERR_EXIT(READ_XLPORT_XGXS0_LN1_STATUS0_REGr(unit, phy_acc, 
                                         &reg_val));
            (*link) = soc_reg_field_get(unit, XLPORT_XGXS0_LN1_STATUS0_REGr, 
                                          reg_val, LINK_STATUSf);
            break;

        case 2:
            _SOC_IF_ERR_EXIT(READ_XLPORT_XGXS0_LN2_STATUS0_REGr(unit, phy_acc, 
                                          &reg_val));
            (*link) = soc_reg_field_get(unit, XLPORT_XGXS0_LN2_STATUS0_REGr, 
                                          reg_val, LINK_STATUSf);
            break;

        case 3:
            _SOC_IF_ERR_EXIT(READ_XLPORT_XGXS0_LN3_STATUS0_REGr(unit, phy_acc, 
                                           &reg_val));
            (*link) = soc_reg_field_get(unit, XLPORT_XGXS0_LN3_STATUS0_REGr, 
                                           reg_val, LINK_STATUSf);
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                     (_SOC_MSG("Port %d, failed to get port index"), port));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_prbs_config_set(int unit, int port, pm_info_t pm_info, 
                     portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }

    if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                            (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_set(phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_prbs_config_get (int unit, int port, pm_info_t pm_info, 
                                 portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }

    if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                       (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_get(phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_prbs_enable_set (int unit, int port, pm_info_t pm_info, 
                                 portmod_prbs_mode_t mode, int flags, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;
    
    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (
              _SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }
    
    if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
              _SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_set(phy_access, nof_phys, flags, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_prbs_enable_get (int unit, int port, pm_info_t pm_info, 
                                 portmod_prbs_mode_t mode, int flags, int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    uint32 is_enabled;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }

    if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                    (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_get(phy_access, nof_phys, flags, &is_enabled));

    (*enable) = (is_enabled ? 1 : 0);

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_prbs_status_get(int unit, int port, pm_info_t pm_info, 
                    portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
                (_SOC_MSG("MAC PRBS is not supported for PM4x10")));
    }

    if(PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, 
                    (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_status_get(phy_access, nof_phys, flags, status));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_nof_lanes_get(int unit, int port, pm_info_t pm_info, int *nof_lanes)
{
    int temp_nof_lanes = 0;
    SOC_INIT_FUNC_DEFS;

    temp_nof_lanes = _pm4x10_nof_lanes_get(unit, port, pm_info);
    if(0 == temp_nof_lanes){
        SOC_EXIT;
    }
    else{
        *nof_lanes = temp_nof_lanes;
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_firmware_mode_set (int unit, int port, pm_info_t pm_info,
                                   phymod_firmware_mode_t fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_firmware_mode_get (int unit, int port, pm_info_t pm_info, 
                                   phymod_firmware_mode_t* fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_runt_threshold_set (int unit, int port, 
                                    pm_info_t pm_info, int value)
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


int pm4x10_port_max_packet_size_set (int unit, int port, 
                                     pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(xlmac_rx_max_size_set(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_max_packet_size_get (int unit, int port, 
                                     pm_info_t pm_info, int* value)
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
    
    _SOC_IF_ERR_EXIT(xlmac_pad_size_set(unit, port , value));
        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    _SOC_IF_ERR_EXIT(xlmac_pad_size_get(unit, port , value));
        
exit:
    SOC_FUNC_RETURN;     
}


int pm4x10_port_tx_drop_on_local_fault_set (int unit, int port, 
                                            pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(pm4x10_port_local_fault_control_get(unit, port, pm_info, &control));

    control.drop_tx_on_fault  = enable;
    _SOC_IF_ERR_EXIT(pm4x10_port_local_fault_control_set(unit, port, pm_info, &control));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_tx_drop_on_local_fault_get (int unit, int port, 
                                            pm_info_t pm_info, int* enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(pm4x10_port_local_fault_control_get(unit, port, pm_info, &control));
    *enable = control.drop_tx_on_fault;

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_tx_drop_on_remote_fault_set(int unit, int port,
                                            pm_info_t pm_info, int enable)
{
    portmod_remote_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(pm4x10_port_remote_fault_control_get(unit, port, pm_info, &control));
    control.drop_tx_on_fault = enable;
    _SOC_IF_ERR_EXIT(pm4x10_port_remote_fault_control_set(unit, port, pm_info,  &control));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_tx_drop_on_remote_fault_get (int unit, int port, 
                                             pm_info_t pm_info, int* enable)
{
    portmod_remote_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(pm4x10_port_remote_fault_control_get(unit, port, pm_info, &control));
    *enable = control.drop_tx_on_fault;

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_local_fault_enable_set (int unit, int port, 
                                        pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(pm4x10_port_local_fault_control_get(unit, port, pm_info, &control));
    control.enable = enable;
    _SOC_IF_ERR_EXIT(pm4x10_port_local_fault_control_set(unit, port, pm_info, &control));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_local_fault_enable_get (int unit, int port, 
                                        pm_info_t pm_info, int* enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(pm4x10_port_local_fault_control_get(unit, port, pm_info, &control));
    *enable = control.enable;

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_local_fault_status_get(int unit, int port,
                                       pm_info_t pm_info, int* value)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = xlmac_local_fault_status_get(unit, port, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_local_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = xlmac_local_fault_control_set(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_local_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = xlmac_local_fault_control_get(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_remote_fault_status_get(int unit, int port,
                                        pm_info_t pm_info, int* value)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = xlmac_remote_fault_status_get(unit, port, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_remote_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = xlmac_remote_fault_control_set(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_remote_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = xlmac_remote_fault_control_get(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_pause_control_set(int unit, int port, pm_info_t pm_info, 
                                  const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_pause_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_pause_control_get(int unit, int port, pm_info_t pm_info,
                                  portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_pause_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_llfc_control_set(int unit, int port, 
           pm_info_t pm_info, const portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_llfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_llfc_control_get(int unit, int port,
            pm_info_t pm_info, portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_llfc_control_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_core_access_get (int unit, int port, 
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

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(phyn > PM_4x10_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), 
                       PM_4x10_INFO(pm_info)->nof_phys[port_index] - 1, phyn));
    }

    if(phyn < 0)
    {
        phyn = PM_4x10_INFO(pm_info)->nof_phys[port_index] - 1;
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));

    if( phyn == 0 ){
        sal_memcpy(&core_access_arr[0], &(PM_4x10_INFO(pm_info)->int_core_access),
                     sizeof(phymod_core_access_t));
        *nof_cores = 1;
    } else {
        index = 0;
        done = 0;
        for(i=0 ; (i< MAX_PORTS_PER_PM4X10); i++) {
            if(bitmap & (0x1U << i)) {
                xphy_id = PM_4x10_INFO(pm_info)->lane_conn[phyn-1][i].xphy_id;
                if(xphy_id != PORTMOD_XPHY_ID_INVALID){
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
        if (phyn == PM_4x10_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x10_port_frame_spacing_stretch_set
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int pm4x10_port_frame_spacing_stretch_set (int unit, int port, 
                                           pm_info_t pm_info,int spacing)
{
    return(xlmac_frame_spacing_stretch_set(unit, port, spacing));
}

/*! 
 * pm4x10_port_frame_spacing_stretch_get
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int pm4x10_port_frame_spacing_stretch_get (int unit, int port,
                                            pm_info_t pm_info, 
                                            const int *spacing)
{
    return (xlmac_frame_spacing_stretch_get(unit, port, (int*)spacing));
}

/*! 
 * pm4x10_port_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  diag_info       - 
 */
int pm4x10_port_diag_fifo_status_get (int unit, int port,pm_info_t pm_info, 
                                 const portmod_fifo_status_t* diag_info)
{
    return(xlmac_diag_fifo_status_get(unit, port, diag_info));
}

int _xlport_pfc_config_set(int unit, int port, uint32 value)
{
    int rv = SOC_E_UNAVAIL;
    uint32 rval32 = 0;

    if (SOC_REG_IS_VALID(unit,XLPORT_MAC_RSV_MASKr)) {
        rv = READ_XLPORT_MAC_RSV_MASKr(unit, port, &rval32);
        if (SOC_SUCCESS(rv)) {
            /* XLPORT_MAC_RSV_MASK: Bit 18 PFC frame detected
             * Enable  PFC Frame : Set 0. Go through
             * Disable PFC Frame : Set 1. Purged.
             */
            if(value) {
                rval32 &= ~(1 << 18);
            } else {
                rval32 |= (1 << 18);
            }
            rv = WRITE_XLPORT_MAC_RSV_MASKr(unit, port, rval32);
        }
    }

    return rv;
}

int _xlport_pfc_config_get(int unit, int port, uint32* value)
{
    int rv = SOC_E_NONE;
    uint32 rval32 = 0;

    if (SOC_REG_IS_VALID(unit,XLPORT_MAC_RSV_MASKr)) {
        rv = READ_XLPORT_MAC_RSV_MASKr(unit, port, &rval32);
        if (SOC_SUCCESS(rv)) {
            /* XLPORT_MAC_RSV_MASK: Bit 18 PFC frame detected
             * Enable  PFC Frame : Set 0. Go through
             * Disable PFC Frame : Set 1. Purged.
             */
            *value = ((rval32 & 0x40000) >> 18) ? 0 : 1;
        }
    }

    return rv;
}

/*! 
 * pm4x10_port_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm4x10_port_pfc_config_set (int unit, int port,pm_info_t pm_info,
                           const portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    if (pfc_cfg->classes != 8) {
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT(xlmac_pfc_config_set(unit, port, pfc_cfg));
    _SOC_IF_ERR_EXIT(_xlport_pfc_config_set(unit, port, pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN; 
}


/*! 
 * pm4x10_port_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm4x10_port_pfc_config_get (int unit, int port,pm_info_t pm_info, 
                                portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    pfc_cfg->classes = 8;
    _SOC_IF_ERR_EXIT(xlmac_pfc_config_get(unit, port, pfc_cfg));

    _SOC_IF_ERR_EXIT(_xlport_pfc_config_get(unit, port, &pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN; 
}

int pm4x10_port_pfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_pfc_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_pfc_control_set(unit, port, control));        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_pfc_control_get(int unit, int port, pm_info_t pm_info, portmod_pfc_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(xlmac_pfc_control_get(unit, port, control));     
exit:
    SOC_FUNC_RETURN; 
    
}

/*!
 * pm4x10_port_eee_set
 *
 * @brief set EEE control and timers
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x10_port_eee_set(int unit, int port, pm_info_t pm_info,const portmod_eee_t* eee)
{
    return (xlmac_eee_set(unit, port, eee));
}

/*!
 * pm4x10_port_eee_get
 *
 * @brief get EEE control and timers
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x10_port_eee_get(int unit, int port, pm_info_t pm_info, portmod_eee_t* eee)
{
    return (xlmac_eee_get(unit, port, eee));
}

/*! 
 * pm4x10_port_vlan_tag_set
 *
 * @brief vlan tag set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x10_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,const portmod_vlan_tag_t* vlan_tag)
{
    return (xlmac_rx_vlan_tag_set (unit, port, vlan_tag->outer_vlan_tag,
                                   vlan_tag->inner_vlan_tag));
}


/*! 
 * pm4x10_port_vlan_tag_get
 *
 * @brief vlan tag get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x10_port_vlan_tag_get(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    return (xlmac_rx_vlan_tag_get (unit, port, (int*)&vlan_tag->outer_vlan_tag,
                                   (int*)&vlan_tag->inner_vlan_tag));
}

/*! 
 * pm4x10_port_duplex_set
 *
 * @brief duplex set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable        - 
 */
int pm4x10_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return (xlmac_duplex_set (unit, port, enable)); 
}


/*! 
 * pm4x10_port_duplex_get
 *
 * @brief duplex get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x10_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    return (xlmac_duplex_get (unit, port, enable)); 
}

/*!
 * pm4x10_port_speed_get
 *
 * @brief duplex get.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  speed         -
 */
int pm4x10_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_inf_config_t config;
    int nof_phys, rv;

    rv = portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys);
    if (rv) return(rv);

    rv = phymod_phy_interface_config_get(&phy_access[0], 0,0,  &config);
    if (rv) return (rv);

    *speed = config.data_rate;
    return (rv);
}

int pm4x10_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int flags, int reg_addr, uint32* value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_reg_read(phy_access, nof_phys, flags, reg_addr,value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int flags, int reg_addr, uint32 value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_reg_write(phy_access, nof_phys, flags, reg_addr,value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_check_legacy_phy (int unit, int port, pm_info_t pm_info, int *legacy_phy)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    *legacy_phy = 0;

    if (PM_4x10_INFO(pm_info)->nof_phys == 0) {
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

int pm4x10_port_reset_set (int unit, int port, pm_info_t pm_info, 
                           int reset_mode, int opcode, int direction)
{
    phymod_core_access_t core_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy reset is not supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           (phymod_phy_access_t*)core_access,
                                           (1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_core_reset_set(core_access, nof_phys,
                                            (phymod_reset_mode_t)reset_mode, 
                                            (phymod_reset_direction_t)direction));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_reset_get (int unit, int port, pm_info_t pm_info, 
                           int reset_mode, int opcode, int* direction)
{
    phymod_core_access_t core_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (PM_4x10_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy reset is not supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           (phymod_phy_access_t*)core_access,
                                           (1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_core_reset_get(core_access, nof_phys,
                       (phymod_reset_mode_t)reset_mode, 
                       (phymod_reset_direction_t*)direction));

exit:
    SOC_FUNC_RETURN;
}

/*Port remote Adv get*/
int pm4x10_port_adv_remote_get (int unit, int port, pm_info_t pm_info, 
                                int* ability_mask)
{
    return (0);
}

/*get port fec enable*/
int pm4x10_port_fec_enable_get(int unit, int port, pm_info_t pm_info, 
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
int pm4x10_port_fec_enable_set(int unit, int port, pm_info_t pm_info, 
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
int pm4x10_port_ability_advert_get(int unit, int port, pm_info_t pm_info, 
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t              phy_access[1+MAX_PHYN];
    phymod_autoneg_ability_t         an_ability;
    portmod_port_interface_config_t  *interface_config;

    int     nof_phys = 0, port_index, flags;
    uint32  bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_get(phy_access, nof_phys, &an_ability));

    /* 
     * SGMII speed should be initialized to count but is initialized to 0 
     * which is treated as 10M. so for sesto mask this out.
     */
    if (nof_phys > 1) {
#ifdef PHYMOD_SESTO_SUPPORT
        if (phy_access[nof_phys-1].type == phymodDispatchTypeSesto) {
            an_ability.sgmii_speed =  phymod_CL37_SGMII_Count; 
        }
#endif /* PHYMOD_SESTO_SUPPORT */
    }
    portmod_common_phy_to_port_ability(&an_ability, ability);

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    interface_config     = &PM_4x10_INFO(pm_info)->port_config[port_index].interface_config;
    ability->interface   = interface_config->interface; 

    _SOC_IF_ERR_EXIT(xlmac_encap_get (unit, port, &flags, &ability->encap)); 

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_ability_advert_set(int unit, int port, pm_info_t pm_info, 
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0, port_index;
    uint32 bitmap;
    portmod_port_ability_t      port_ability;
    phymod_autoneg_ability_t    an_ability;
    portmod_access_get_params_t params;
    pm_4x10_port_t        *pInfo = NULL; 

    int port_num_lanes, line_interface, cx4_10g;
    int an_cl72, an_fec, hg_mode;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index, 
                                             &bitmap));
    pInfo = &(PM_4x10_INFO(pm_info)->port_config[port_index]);


    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = 0;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT
        (pm4x10_port_ability_local_get(unit, port, pm_info, &port_ability));

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
    line_interface = pInfo->interface_config.interface;
    cx4_10g        = pInfo->port_init_config.cx4_10g;
    an_cl72        = pInfo->port_init_config.an_cl72;
    an_fec         = pInfo->port_init_config.an_fec;
    hg_mode        = PHYMOD_INTF_MODES_HIGIG_GET(&pInfo->interface_config); 

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    /* 
     * if line interface is CAUI -- check it is 4x25 or 10x10
     * change the line interface for this purpose.
     *
     * Default interface for 100G is CR10. However if line interface is Falcon
     * the interface should be CR4 (not CR10). Fix the line interface here..,
     */ 
    if (nof_phys > 1) {
#ifdef PHYMOD_SESTO_SUPPORT
        if ((phy_access[nof_phys-1].type == phymodDispatchTypeSesto) &&
            (line_interface == SOC_PORT_IF_CAUI)) {
            line_interface = SOC_PORT_IF_CR4;  
        }
#endif /* PHYMOD_SESTO_SUPPORT */

#ifdef PHYMOD_DINO_SUPPORT
        if ((phy_access[nof_phys-1].type == phymodDispatchTypeDino) &&
            (line_interface == SOC_PORT_IF_CAUI)) {
            line_interface = SOC_PORT_IF_CR4;
        }
#endif /* PHYMOD_DINO_SUPPORT */
    }

    portmod_common_port_to_phy_ability(&port_ability, &an_ability,
                                       port_num_lanes, line_interface, cx4_10g,
                                       an_cl72, an_fec, hg_mode);

    an_ability.an_master_lane = PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config.an_master_lane;

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_set(phy_access, nof_phys, &an_ability));

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
int pm4x10_port_ability_remote_get(int unit, int port, pm_info_t pm_info, 
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
        (portmod_port_phychain_autoneg_remote_ability_get(phy_access, nof_phys, &an_ability));

    portmod_common_phy_to_port_ability(&an_ability, ability);

exit:
    SOC_FUNC_RETURN;
}

int _xlport_rx_control_set(int unit, int port, int value)
{
    int rv = SOC_E_UNAVAIL;
    uint32 rval32 = 0;

    if (SOC_REG_IS_VALID(unit,XLPORT_MAC_RSV_MASKr)) {
        rv = READ_XLPORT_MAC_RSV_MASKr(unit, port, &rval32);
        if (SOC_SUCCESS(rv)) {
            /* XLPORT_MAC_RSV_MASK: Bit 11 Control Frame recieved
             * Enable  Control Frame : Set 0. Packet go through
             * Disable Control Frame : Set 1. Packet is purged.
             */
            if(value) {
                rval32 &= ~(1 << 11);
            } else {
                rval32 |= (1 << 11);
            }
            rv = WRITE_XLPORT_MAC_RSV_MASKr(unit, port, rval32);
        }
    }

    return rv;
}

int pm4x10_port_rx_control_set (int unit, int port, pm_info_t pm_info,
                                 const portmod_rx_control_t* rx_ctrl)
{
    int phy_acc;
    uint32 reg_val;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_RSV_MASKr(unit, phy_acc, &reg_val));

    if(rx_ctrl->pass_control_frames){
            reg_val &= ~(1 << 11);
    } else{
            reg_val |= (1 << 11);
    }
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_RSV_MASKr(unit, phy_acc, reg_val));

    return (SOC_E_NONE);
}

int pm4x10_port_rx_control_get (int unit, int port, pm_info_t pm_info,
                                         portmod_rx_control_t* rx_ctrl)
{
    int phy_acc;
    uint32 reg_val;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_RSV_MASKr(unit, phy_acc, &reg_val));
    rx_ctrl->pass_control_frames = ((reg_val & 0x800) >> 11) ? 0 : 1;

    return (SOC_E_NONE);
}

int pm4x10_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (xlmac_tx_mac_sa_set(unit, port, mac_addr));
}


int pm4x10_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (xlmac_tx_mac_sa_get(unit, port, mac_addr));
}

int pm4x10_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (xlmac_rx_mac_sa_set(unit, port, mac_addr));
}


int pm4x10_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (xlmac_rx_mac_sa_get(unit, port, mac_addr));
}

int pm4x10_port_tx_average_ipg_set (int unit, int port, 
                                    pm_info_t pm_info, int value)
{
    return (xlmac_tx_average_ipg_set(unit, port, value));
}


int pm4x10_port_tx_average_ipg_get (int unit, int port, 
                                    pm_info_t pm_info, int* value)
{
    return (xlmac_tx_average_ipg_get(unit, port, value));
}

int pm4x10_port_update (int unit, int port, pm_info_t pm_info,
                        const portmod_port_update_control_t* update_control)
{
    int link, flags = 0;
    int duplex = 0, tx_pause = 0, rx_pause = 0;
    phymod_autoneg_status_t autoneg;
    portmod_pause_control_t pause_control;
    portmod_port_ability_t local_advert, remote_advert;
    portmod_port_interface_config_t interface_config;

    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int sys_start_lane = -1;
    int sys_end_lane = -1;
    int line_start_lane = -1;
    int line_end_lane=-1;
    uint32 int_lane_mask=0xf;
    uint32_t serdes_link_status=0;
    phymod_phy_rx_lane_control_t rx_control;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_autoneg_status_t_init(&autoneg));
    _SOC_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    sal_memset(&local_advert, 0, sizeof(portmod_port_ability_t));
    sal_memset(&remote_advert, 0, sizeof(portmod_port_ability_t));

    if(update_control->link_status == -1) {
        _SOC_IF_ERR_EXIT(pm4x10_port_link_get(unit, port, pm_info, &link)); 
    } else {
        link = update_control->link_status;
    }

    if (PORTMOD_PORT_UPDATE_F_UPDATE_SERDES_LINK_GET(update_control)){
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));

        _SOC_IF_ERR_EXIT(_pm4x10_port_tricore_lane_info_get(unit, port, pm_info,
                                              &sys_start_lane, &sys_end_lane,
                                              &line_start_lane, &line_end_lane,
                                              &int_lane_mask));
        /* only look at the lanes that are used. */
        phy_access[0].access.lane_mask &= int_lane_mask;

        if (nof_phys > 1){
            _SOC_IF_ERR_EXIT
                (phymod_phy_link_status_get(&phy_access[0],&serdes_link_status));

            if(link) {
                _SOC_IF_ERR_EXIT
                    (phymod_phy_rx_lane_control_get(&phy_access[0],&rx_control));
                if(rx_control == phymodRxSquelchOn){
                    phymod_phy_rx_lane_control_set(&phy_access[0],
                                                   phymodRxSquelchOff);
                }
            } else {
                /* if external phy and link is down, internal serdes link is up,
                   force internal SerDes link to be down. */
                if(serdes_link_status){
                    phymod_phy_rx_lane_control_set(&phy_access[0],
                                                   phymodRxSquelchOn);
                }
            }
        }
        return (SOC_E_NONE);
    }



    if(!link) {
        /* PHY is down.  Disable the MAC. */
        if (PORTMOD_PORT_UPDATE_F_DISABLE_MAC_GET(update_control)) {
            _SOC_IF_ERR_EXIT(xlmac_enable_set(unit, port, 0, 0));
        }

        /* TBD - do we need phymod_link_down_event ? */
    } else {

        /* TBD - do we need phymod_link_up_event ? */

        /* In case of AN - need to update MAC settings*/
        _SOC_IF_ERR_EXIT(pm4x10_port_autoneg_status_get(unit, port, pm_info, &autoneg));

        if(autoneg.enabled && autoneg.locked) {
            /* update MAC */
            _SOC_IF_ERR_EXIT(pm4x10_port_interface_config_get(unit, port, pm_info, &interface_config));
            _SOC_IF_ERR_EXIT(xlmac_enable_set(unit, port, 0, 0));
            flags = XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
            _SOC_IF_ERR_EXIT(xlmac_speed_set(unit, port, flags, interface_config.speed));
            _SOC_IF_ERR_EXIT(xlmac_enable_set(unit, port, 0, 1));
            /* update pause in MAC on the base of local and remote pause ability*/
            _SOC_IF_ERR_EXIT(pm4x10_port_duplex_get(unit, port, pm_info, &duplex));
            _SOC_IF_ERR_EXIT(pm4x10_port_ability_advert_get(unit, port, pm_info, &local_advert));
            _SOC_IF_ERR_EXIT(pm4x10_port_ability_remote_get(unit, port, pm_info, &remote_advert));
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
            _SOC_IF_ERR_EXIT(pm4x10_port_pause_control_get(unit, port, pm_info, &pause_control));
            if ((pause_control.rx_enable != rx_pause) || (pause_control.tx_enable != tx_pause)) {
                pause_control.rx_enable = rx_pause;
                pause_control.tx_enable = tx_pause;
                _SOC_IF_ERR_EXIT(pm4x10_port_pause_control_set(unit, port, pm_info, &pause_control));
            }
        }

        _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_drv_name_get (int unit, int port, pm_info_t pm_info, 
                              char* buf, int len)
{
    strncpy(buf, "PM4X10 Driver", len);
    return (SOC_E_NONE);
}

int pm4x10_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int lcl_fault, rmt_fault;

    SOC_IF_ERROR_RETURN (pm4x10_port_clear_rx_lss_status_get (unit, port, pm_info, &lcl_fault, &rmt_fault));
    if (lcl_fault) {
        SOC_IF_ERROR_RETURN (pm4x10_port_clear_rx_lss_status_set(unit, port, pm_info, 0, rmt_fault)); 
    }
    return (pm4x10_port_clear_rx_lss_status_set (unit, port, pm_info, 1, rmt_fault));
}

int pm4x10_port_remote_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int lcl_fault, rmt_fault;

    SOC_IF_ERROR_RETURN (pm4x10_port_clear_rx_lss_status_get (unit, port, pm_info, &lcl_fault, &rmt_fault));
    if (rmt_fault) {
        SOC_IF_ERROR_RETURN (pm4x10_port_clear_rx_lss_status_set(unit, port, pm_info, lcl_fault, 0)); 
    }
    return (pm4x10_port_clear_rx_lss_status_set (unit, port, pm_info, lcl_fault, 1));
}


int pm4x10_port_clear_rx_lss_status_set (int unit, soc_port_t port, 
                           pm_info_t pm_info, int lcl_fault, int rmt_fault)
{
    return (xlmac_clear_rx_lss_status_set (unit, port, lcl_fault, rmt_fault));
}

int pm4x10_port_clear_rx_lss_status_get (int unit, soc_port_t port, 
                           pm_info_t pm_info, int *lcl_fault, int *rmt_fault)
{
    return (xlmac_clear_rx_lss_status_get (unit, port, lcl_fault, rmt_fault));
}

int pm4x10_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    int phy_acc;
    uint32_t rval;
    int link = 0;
    soc_timeout_t to;

    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* Link status to IPIPE is updated by H/W and driven based on both the
     * rising edge of XLPORT_LAG_FAILOVER_CONFIG.LINK_STATUS_UP and
     * actual link up status from Serdes.
     * In some loopback scenarios it may take longer time to see Serdes link up status.
     */
    soc_timeout_init(&to, 10000, 0);

    do {
        _SOC_IF_ERR_EXIT(pm4x10_port_mac_link_get(unit, port, pm_info, &link));
        if (link) {
            break;
        }
    } while(!soc_timeout_check(&to));

    /* Toggle link_status_up bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_XLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, XLPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 1);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));
    soc_reg_field_set(unit, XLPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 0);
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_lag_failover_loopback_set (int unit, soc_port_t port, 
                                        pm_info_t pm_info, int value)
{
    return (xlmac_lag_failover_loopback_set(unit, port, value));
}

int pm4x10_port_lag_failover_loopback_get (int unit, soc_port_t port, 
                                        pm_info_t pm_info, int *value)
{
    return(xlmac_lag_failover_loopback_get(unit, port, value));
}

int pm4x10_port_mode_set(int unit, soc_port_t port,
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    int phy_acc;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    return (_xlport_mode_set(unit, phy_acc, mode->cur_mode));
}

int pm4x10_port_mode_get(int unit, soc_port_t port,
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    int port_index, rv, phy_acc;
    uint32 bitmap;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = _pm4x10_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (rv) return (rv);

    return(_xlport_mode_get(unit, phy_acc, port_index, &mode->cur_mode, &mode->lanes));
}

int
pm4x10_port_trunk_hwfailover_config_set(int unit, soc_port_t port, pm_info_t pm_info, int hw_count)
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

    SOC_IF_ERROR_RETURN (xlmac_lag_failover_loopback_get(unit, port, &lag_failover_lpbk));
    if (lag_failover_lpbk) return (SOC_E_NONE);


    SOC_IF_ERROR_RETURN (xlmac_lag_failover_en_get(unit, port, &old_failover_en));
    SOC_IF_ERROR_RETURN (xlmac_sw_link_status_select_get (unit, port, &old_link_status_sel));

    if (old_failover_en     != new_failover_en ||
        old_link_status_sel != new_link_status_sel) {

        SOC_IF_ERROR_RETURN (xlmac_sw_link_status_select_set (unit, port, new_link_status_sel));
        SOC_IF_ERROR_RETURN (xlmac_lag_failover_en_set(unit, port, new_failover_en));
    }

    SOC_IF_ERROR_RETURN (xlmac_reset_fc_timers_on_link_dn_get(unit,
                                       port, &old_reset_flow_control));
    if (old_reset_flow_control != new_reset_flow_control) {
        SOC_IF_ERROR_RETURN (xlmac_reset_fc_timers_on_link_dn_set(unit, 
                                        port, new_reset_flow_control));
    }

    return 0;
}

int
pm4x10_port_trunk_hwfailover_config_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                        int *enable)
{
    return (xlmac_lag_failover_en_get(unit, port, enable));
}


int
pm4x10_port_trunk_hwfailover_status_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                        int *loopback)
{
    return (xlmac_lag_failover_loopback_get(unit, port, loopback));
}

int pm4x10_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_tx_t          ln_txparam[PM4X10_LANES_PER_CORE];
    int nof_phys;
    uint32 lane_map;
    portmod_access_get_params_t params;


    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));

    if( PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_INT ) {
        params.phyn = 0 ;
    } else { 
        /* most external  PHY_DIAG_DEV_DFLT and PHY_DIAG_DEV_EXT */  
        params.phyn = -1 ;  
    }

    if(PHY_DIAG_INST_INTF(inst) == PHY_DIAG_INTF_SYS ){
        params.sys_side = PORTMOD_SIDE_SYSTEM;
    } else { /* line side is default */
        params.sys_side = PORTMOD_SIDE_LINE;
    }

    params.apply_lane_mask = 1;

    SOC_IF_ERROR_RETURN(pm4x10_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, phy_access, &nof_phys, NULL));

    lane_map = phy_access[0].access.lane_mask;

    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "pm4x10_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_DSC));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pmd_info_dump(phy_access, nof_phys,
                                                 (void*)arg));

            break;

        case PHY_DIAG_CTRL_PCS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "pm4x10_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_PCS));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pcs_info_dump(phy_access, nof_phys,
                                                    (void*)arg));
            break;

        case PHY_DIAG_CTRL_LINKMON_MODE:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "pm4x10_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_LINKMON_MODE  0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_LINKMON_MODE));

            if(phy_access[0].access.lane_mask){
                SOC_IF_ERROR_RETURN
                    (portmod_pm_phy_link_mon_enable_set(phy_access, nof_phys, PTR_TO_INT(arg)));
            }
            break;

        case PHY_DIAG_CTRL_LINKMON_STATUS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "pm4x10_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_LINKMON_STATUS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_LINKMON_STATUS));

            if(phy_access[0].access.lane_mask){
                SOC_IF_ERROR_RETURN
                    (portmod_pm_phy_link_mon_status_get(phy_access, nof_phys));
            }
            break;


        default:
            if(op_type == PHY_DIAG_CTRL_SET) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "pm4x10_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_SET 0x%x\n"),
                          unit, port, PHY_DIAG_CTRL_SET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !(phy_access->access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_set(phy_access, nof_phys, op_cmd, ln_txparam, lane_map, PTR_TO_INT(arg)));
                    }
                }
            } else if(op_type == PHY_DIAG_CTRL_GET) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "pm4x10_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_GET 0x%x\n"),
                          unit, port, PHY_DIAG_CTRL_GET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !(phy_access->access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_get(phy_access,nof_phys,op_cmd, ln_txparam, lane_map, (uint32 *)arg));
                    } else {
                        *(uint32 *)arg = 0;
                    }
                }
            } else {
                return (SOC_E_UNAVAIL);
            }
            break ;
    }
    return (SOC_E_NONE);
}

int
pm4x10_port_ref_clk_get(int unit, soc_port_t port, pm_info_t pm_info, int *ref_clk)
{
    *ref_clk = pm_info->pm_data.pm4x10_db->ref_clk;

    return (SOC_E_NONE);
}

int pm4x10_port_lag_remove_failover_lpbk_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (xlmac_lag_remove_failover_lpbk_get(unit, port, val));
}

int pm4x10_port_lag_remove_failover_lpbk_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (xlmac_lag_remove_failover_lpbk_set(unit, port, val));
}

int pm4x10_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{
    return (xlmac_lag_failover_disable(unit, port));
}

int pm4x10_port_mac_ctrl_set(int unit, int port, 
                   pm_info_t pm_info, uint64 ctrl)
{
    return(xlmac_mac_ctrl_set(unit, port, ctrl));
}

int pm4x10_port_drain_cell_get(int unit, int port, 
           pm_info_t pm_info, portmod_drain_cells_t *drain_cells)
{
    return (xlmac_drain_cell_get(unit, port, drain_cells));
}

int pm4x10_port_drain_cell_stop (int unit, int port, 
           pm_info_t pm_info, const portmod_drain_cells_t *drain_cells)
{
    return(xlmac_drain_cell_stop (unit, port, drain_cells));
}

int pm4x10_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    return (xlmac_drain_cell_start(unit, port));
}

int pm4x10_port_txfifo_cell_cnt_get(int unit, int port,
                       pm_info_t pm_info, uint32* fval)
{
    return(xlmac_txfifo_cell_cnt_get(unit, port, fval));
}

int pm4x10_port_egress_queue_drain_get(int unit, int port,
                   pm_info_t pm_info, uint64 *ctrl, int *rx)
{
    return (xlmac_egress_queue_drain_get(unit, port, ctrl, rx));
}

int pm4x10_port_drain_cells_rx_enable (int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return (xlmac_drain_cells_rx_enable(unit, port, rx_en));
}

int pm4x10_port_egress_queue_drain_rx_en(int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return (xlmac_egress_queue_drain_rx_en(unit, port, rx_en));
}

int pm4x10_port_mac_reset_set(int unit, int port,
                   pm_info_t pm_info, int val)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    /* if callback defined, go to local soft reset function */
    rv = (PM_4x10_INFO(pm_info)->portmod_mac_soft_reset) ? pm4x10_port_soft_reset(unit, port, pm_info, val) : xlmac_soft_reset_set(unit, port, val);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_mac_reset_check(int unit, int port, pm_info_t pm_info,
                                int enable, int* reset)
{
    return (xlmac_reset_check(unit, port, enable, reset));
}

int pm4x10_port_core_num_get(int unit, int port, pm_info_t pm_info,
                                int* core_num)
{
    *core_num = PM_4x10_INFO(pm_info)->core_num;
    return (SOC_E_NONE);
}

int pm4x10_port_e2ecc_hdr_set (int unit, int port, pm_info_t pm_info, 
                               const portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    return (xlmac_e2ecc_hdr_set (unit, port, (uint32_t *)e2ecc_hdr->words));
}


int pm4x10_port_e2ecc_hdr_get (int unit, int port, pm_info_t pm_info, 
                               portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    return (xlmac_e2ecc_hdr_get (unit, port, e2ecc_hdr->words));
}

int pm4x10_port_e2e_enable_set (int unit, int port, pm_info_t pm_info, int enable) 
{
    return (xlmac_e2e_enable_set(unit, port, enable));
}


int pm4x10_port_e2e_enable_get (int unit, int port, pm_info_t pm_info, int *enable)
{
    return (xlmac_e2e_enable_get(unit, port, enable));
}

int pm4x10_port_fallback_lane_get(int unit, int port, pm_info_t pm_info, int* fallback_lane)
{
    int                         port_index;
    uint32_t                    bitmap;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x10_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    *fallback_lane = PM_4x10_INFO(pm_info)->port_config[port_index].port_init_config.port_fallback_lane;

exit:
    SOC_FUNC_RETURN;

}

/* in_out: 1= in reset, 0= in and out of reset */
STATIC
int pm4x10_port_soft_reset(int unit, int port, pm_info_t pm_info, int in_out)
{
    int rv, rx_enable = 0;
    int phy_port, block, bindex, i;
    portmod_drain_cells_t drain_cells;
    uint32 cell_count;
    uint64 mac_ctrl;
    soc_timeout_t to;
    SOC_INIT_FUNC_DEFS;

    /* Callback soft reset function */
    if (PM_4x10_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_4x10_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePre);
        _SOC_IF_ERR_EXIT(rv);
    }

    if(in_out == 0) {
    
        rv = xlmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable); 
        _SOC_IF_ERR_EXIT(rv);

        /* Drain cells */
        rv = xlmac_drain_cell_get(unit, port, &drain_cells);
        _SOC_IF_ERR_EXIT(rv);

        /* Start TX FIFO draining */
        rv = xlmac_drain_cell_start(unit, port);
        _SOC_IF_ERR_EXIT(rv);

        /* De-assert SOFT_RESET to let the drain start */
        rv = xlmac_soft_reset_set(unit, port, 0);           
        _SOC_IF_ERR_EXIT(rv);

        /* Wait until TX fifo cell count is 0 */
        soc_timeout_init(&to, 250000, 0);
        for (;;) {
            rv = xlmac_txfifo_cell_cnt_get(unit, port, &cell_count);
               _SOC_IF_ERR_EXIT(rv);
               if (cell_count == 0) {
                    break;
            }
            if (soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_UP(unit, port,
                                   "ERROR: u=%d p=%d timeout draining "
                                   "TX FIFO (%d cells remain)\n"),
                               unit, port, cell_count));
                    return SOC_E_INTERNAL;
            }
        }
   
        /* Stop TX FIFO draining */
        rv = xlmac_drain_cell_stop (unit, port, &drain_cells);
        _SOC_IF_ERR_EXIT(rv);

        /* Reset port FIFO */
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
       for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                if ((SOC_BLOCK_INFO(unit, block).type == SOC_BLK_XLPORT) ||
                    (SOC_BLOCK_INFO(unit, block).type == SOC_BLK_XLPORTB0)) {
                    bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                    _SOC_IF_ERR_EXIT(portmod_port_soft_reset_toggle(unit, port, bindex));
                    break;
            }
        }
    }

    /* Put port into SOFT_RESET */
    rv = xlmac_soft_reset_set(unit, port, 1);
    _SOC_IF_ERR_EXIT(rv);
    
    if (in_out == 0) {

        /* Callback soft reset function */
       if (PM_4x10_INFO(pm_info)->portmod_mac_soft_reset) {
            rv = PM_4x10_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeDuring);
               _SOC_IF_ERR_EXIT(rv);
               soc_reg64_field32_set (unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0); /*make sure restored data wont put mac back in reset*/
        }

        /* Enable TX, set RX, de-assert SOFT_RESET */
       rv = xlmac_egress_queue_drain_rx_en(unit, port, rx_enable);
        _SOC_IF_ERR_EXIT(rv);

        /* Restore XLMAC_CTRL to original value */
        rv = xlmac_mac_ctrl_set(unit, port, mac_ctrl);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* Callback soft reset function */
    if (PM_4x10_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_4x10_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePost);
        _SOC_IF_ERR_EXIT(rv);
    }
 
exit:
    SOC_FUNC_RETURN;
}

/*Port discard set*/
int pm4x10_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    return(xlmac_discard_set(unit, port, discard));
}

/*Port soft reset set set*/
int pm4x10_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx,
                               int val, int flags)
{
    SOC_INIT_FUNC_DEFS;

    if (PORTMOD_PORT_REG_ACCESS_DIRECT == flags) {
        _SOC_IF_ERR_EXIT(_pm4x10_port_soft_reset_set_by_phyport(unit, port,
                                                        pm_info, idx, val));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_port_soft_reset_set(unit, port, pm_info, idx, val));
    }

exit:
    SOC_FUNC_RETURN;
}

/*Port tx_en=0 and softreset mac*/
int pm4x10_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(xlmac_tx_enable_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(xlmac_discard_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(xlmac_soft_reset_set(unit, port, 1));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_pgw_reconfig (int unit, int port, pm_info_t pm_info, 
                              const portmod_port_mode_info_t *pmode, int phy_port, int flags)
{
    int rv, idx;
    int phy_acc;
    uint32 reg_val;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};

    SOC_INIT_FUNC_DEFS; 

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    if (PORTMOD_PORT_PGW_MAC_RESET == flags) {
        rv = xlmac_discard_set(unit, port, 1);
        _SOC_IF_ERR_EXIT(rv);
        rv = xlmac_rx_enable_set(unit, port, 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = xlmac_tx_enable_set(unit, port, 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = xlmac_soft_reset_set(unit, port, 1);
        _SOC_IF_ERR_EXIT(rv);

        _SOC_IF_ERR_EXIT(READ_XLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));

    } else if (PORTMOD_PORT_PGW_POWER_SAVE == flags) {
         _SOC_IF_ERR_EXIT(READ_XLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
         soc_reg_field_set(unit, XLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 1);
         _SOC_IF_ERR_EXIT(WRITE_XLPORT_POWER_SAVEr(unit, phy_acc, reg_val));

    } else if (PORTMOD_PORT_PGW_MAC_UNRESET == flags) {
        _SOC_IF_ERR_EXIT(READ_XLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 0);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
    } else if (PORTMOD_PORT_PGW_CONFIGURE == flags) {
        /* 10/20/40G RECONFIGURE SEQUENCE */
        idx = pmode->port_index;

        /* 1.a st SOFT_RESET field in XLMAC_CTRL for new ports that use XLMAC */
        rv = xlmac_soft_reset_set(unit, port, 1);
        _SOC_IF_ERR_EXIT(rv);

        /* 1.a set PORT[subport] field in XLPORT_SOFT_RESET for all new ports */
        _SOC_IF_ERR_EXIT(READ_XLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &reg_val, port_fields[idx], 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

        /* Power Save */
        _SOC_IF_ERR_EXIT(READ_XLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_POWER_SAVEr(unit, phy_acc, reg_val));

        /* Port enable */
        _SOC_IF_ERR_EXIT(READ_XLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
        soc_reg_field_set (unit, XLPORT_ENABLE_REGr, &reg_val,
                               port_fields[idx], 1);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

        /* set port mode */
        _SOC_IF_ERR_EXIT(_xlport_mode_set(unit, phy_acc, pmode->cur_mode));

        /* Power Save */
        _SOC_IF_ERR_EXIT(READ_XLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 0);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_POWER_SAVEr(unit, phy_acc, reg_val));

        /* Release XLPORT SOFT RESET */
        _SOC_IF_ERR_EXIT(READ_XLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &reg_val, port_fields[idx], 0);
        _SOC_IF_ERR_EXIT(WRITE_XLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

        /*Release Mac Soft Reset */
        rv = xlmac_soft_reset_set(unit, port, 0);
        _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_notify(int unit, int port, pm_info_t pm_info, int link) {
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

int pm4x10_port_timesync_config_set(int unit, int port, pm_info_t pm_info, const portmod_phy_timesync_config_t* config)
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

int pm4x10_port_timesync_config_get(int unit, int port, pm_info_t pm_info, portmod_phy_timesync_config_t* config)
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

int pm4x10_port_timesync_enable_set(int unit, int port, pm_info_t pm_info, uint32 enable)
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

int pm4x10_port_timesync_enable_get(int unit, int port, pm_info_t pm_info, uint32* enable)
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

int pm4x10_port_timesync_nco_addend_set(int unit, int port, pm_info_t pm_info, uint32 freq_step)
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

int pm4x10_port_timesync_nco_addend_get(int unit, int port, pm_info_t pm_info, uint32* freq_step)
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

int pm4x10_port_timesync_framesync_mode_set(int unit, int port, pm_info_t pm_info, 
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

int pm4x10_port_timesync_framesync_mode_get(int unit, int port, pm_info_t pm_info, 
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

int pm4x10_port_timesync_local_time_set(int unit, int port, pm_info_t pm_info, const uint64 local_time)
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


int pm4x10_port_timesync_local_time_get(int unit, int port, pm_info_t pm_info, uint64* local_time)
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

int pm4x10_port_timesync_load_ctrl_set(int unit, int port, pm_info_t pm_info, 
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

int pm4x10_port_timesync_load_ctrl_get(int unit, int port, pm_info_t pm_info, 
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

int pm4x10_port_timesync_tx_timestamp_offset_set(int unit, int port, pm_info_t pm_info, uint32 ts_offset)
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

int pm4x10_port_timesync_tx_timestamp_offset_get(int unit, int port, pm_info_t pm_info, uint32* ts_offset)
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

int pm4x10_port_timesync_rx_timestamp_offset_set(int unit, int port, pm_info_t pm_info, uint32 ts_offset)
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

int pm4x10_port_timesync_rx_timestamp_offset_get(int unit, int port, pm_info_t pm_info, uint32* ts_offset)
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

int pm4x10_port_phy_intr_enable_set(int unit, int port, pm_info_t pm_info, uint32 intr_enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_phy_intr_enable_set(phy_access, nof_phys,
                                                                     intr_enable));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_intr_enable_get(int unit, int port, pm_info_t pm_info, uint32* intr_enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_phy_intr_enable_get(phy_access, nof_phys,
                                                                  intr_enable));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_intr_status_get(int unit, int port, pm_info_t pm_info, uint32* intr_enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_phy_intr_status_get(phy_access, nof_phys,
                                                                  intr_enable));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_intr_status_clear(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_phy_intr_status_clear(phy_access, nof_phys));

    exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_phy_timesync_do_sync(int unit, int port, pm_info_t pm_info )
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_phy_timesync_do_sync(phy_access, nof_phys ));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_timesync_capture_timestamp_get(int unit, int port, pm_info_t pm_info, uint64* cap_ts)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_capture_timestamp_get(phy_access, nof_phys,
                                                                             cap_ts));
    exit:
    SOC_FUNC_RETURN;
}


int pm4x10_port_timesync_heartbeat_timestamp_get(int unit, int port, pm_info_t pm_info, uint64* hb_ts)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_heartbeat_timestamp_get(phy_access, nof_phys,
                                                                              hb_ts));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_edc_config_set(int unit, int port, pm_info_t pm_info, 
                               const portmod_edc_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_edc_config_set(phy_access, nof_phys, config));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_edc_config_get(int unit, int port, pm_info_t pm_info, 
                               portmod_edc_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_edc_config_get(phy_access, nof_phys, config));
    exit:
    SOC_FUNC_RETURN;
}



/* get interrupt value. */
int pm4x10_port_interrupt_enable_get (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 *val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_XLPORT_INTR_ENABLEr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_ENABLEr, reg_val, MIB_TX_MEM_ERRf);
             break;
        case portmodIntrTypeMibRxMemErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_ENABLEr, reg_val, MIB_RX_MEM_ERRf);
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_ENABLEr, reg_val, MAC_TX_CDC_MEM_ERRf);
             break;
        case portmodIntrTypeMacRxCdcMemErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_ENABLEr, reg_val, MAC_RX_CDC_MEM_ERRf);
             break;
        case portmodIntrTypeTscErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_ENABLEr, reg_val, TSC_ERRf);
             break;
        case portmodIntrTypeRxFcReqFull:
             *val = soc_reg_field_get(unit, XLPORT_INTR_ENABLEr, reg_val, RX_FLOWCONTROL_REQ_FULLf);
             break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/* set interrupt value. */
int pm4x10_port_interrupt_enable_set (int unit, int port, pm_info_t pm_info, 
                                    int intr_type, uint32 val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_XLPORT_INTR_ENABLEr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             soc_reg_field_set(unit, XLPORT_INTR_ENABLEr, &reg_val, MIB_TX_MEM_ERRf, val); 
             break;
        case portmodIntrTypeMibRxMemErr:
             soc_reg_field_set(unit, XLPORT_INTR_ENABLEr, &reg_val, MIB_RX_MEM_ERRf, val); 
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             soc_reg_field_set(unit, XLPORT_INTR_ENABLEr, &reg_val, MAC_TX_CDC_MEM_ERRf, val); 
             break;
        case portmodIntrTypeMacRxCdcMemErr:
             soc_reg_field_set(unit, XLPORT_INTR_ENABLEr, &reg_val, MAC_RX_CDC_MEM_ERRf, val); 
             break;
        case portmodIntrTypeTscErr:
             soc_reg_field_set(unit, XLPORT_INTR_ENABLEr, &reg_val, TSC_ERRf, val); 
             break;
        case portmodIntrTypeRxFcReqFull:
             soc_reg_field_set(unit, XLPORT_INTR_ENABLEr, &reg_val, RX_FLOWCONTROL_REQ_FULLf, val); 
             break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_INTR_ENABLEr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/* get interrupt value. */
int pm4x10_port_interrupt_get (int unit, int port, pm_info_t pm_info, 
                               int intr_type, uint32* val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_XLPORT_INTR_STATUSr(unit, phy_acc, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, MIB_TX_MEM_ERRf); 
             break;
        case portmodIntrTypeMibRxMemErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, MIB_RX_MEM_ERRf); 
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, MAC_TX_CDC_MEM_ERRf); 
             break;
        case portmodIntrTypeMacRxCdcMemErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, MAC_RX_CDC_MEM_ERRf); 
             break;
        case portmodIntrTypeTscErr:
             *val = soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, TSC_ERRf); 
             break;
        case portmodIntrTypeRxFcReqFull:
             *val = soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, RX_FLOWCONTROL_REQ_FULLf); 
             break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_interrupts_get (int unit, int port, pm_info_t pm_info, 
                                int arr_max_size, uint32* intr_arr, uint32* size)
{
    uint32 reg_val, cnt=0;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_XLPORT_INTR_STATUSr(unit, phy_acc, &reg_val));

    if (soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, MIB_TX_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibTxMemErr;
    }

    if (soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, MIB_RX_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibRxMemErr;
    }
    if (soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, MAC_TX_CDC_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMacTxCdcMemErr;
    }
    if (soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, MAC_RX_CDC_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMacRxCdcMemErr;
    }
    if (soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, TSC_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTscErr;
    }
    if (soc_reg_field_get(unit, XLPORT_INTR_STATUSr, reg_val, RX_FLOWCONTROL_REQ_FULLf)) {
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
 * pm4x10_port_eee_clock_set
 *
 * @brief set EEE Config.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x10_port_eee_clock_set(int unit, int port, pm_info_t pm_info,
                              const portmod_eee_clock_t* eee_clk)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(WRITE_XLPORT_EEE_CLOCK_GATEr(unit, phy_acc, eee_clk->clock_gate));
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_EEE_CORE0_CLOCK_GATE_COUNTERr(unit, phy_acc, eee_clk->clock_count));
    _SOC_IF_ERR_EXIT(WRITE_XLPORT_EEE_DURATION_TIMER_PULSEr(unit, phy_acc, eee_clk->timer_pulse));

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
int pm4x10_port_eee_clock_get (int unit, int port, pm_info_t pm_info,
                               portmod_eee_clock_t* eee_clk)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_XLPORT_EEE_CLOCK_GATEr(unit, phy_acc, &eee_clk->clock_gate));
    _SOC_IF_ERR_EXIT(READ_XLPORT_EEE_CORE0_CLOCK_GATE_COUNTERr(unit, phy_acc, &eee_clk->clock_count));
    _SOC_IF_ERR_EXIT(READ_XLPORT_EEE_DURATION_TIMER_PULSEr(unit, phy_acc, &eee_clk->timer_pulse));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_port_failover_mode_set(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t failover)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (PM_4x10_INFO(pm_info)->nof_phys == 0) {
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

int pm4x10_port_failover_mode_get(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t* failover)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    if (PM_4x10_INFO(pm_info)->nof_phys == 0) {
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

int pm4x10_port_flow_control_set (int unit, int port, pm_info_t pm_info, 
                                  int merge_mode_en, int parallel_fc_en)
{
    int phy_acc, rv;
    uint32 rval;

    PM4x10_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = READ_XLPORT_FLOW_CONTROL_CONFIGr(unit, phy_acc, &rval);
    if (SOC_FAILURE(rv)) return (rv);

    soc_reg_field_set(unit, XLPORT_FLOW_CONTROL_CONFIGr, &rval, MERGE_MODE_ENf, merge_mode_en);
    soc_reg_field_set(unit, XLPORT_FLOW_CONTROL_CONFIGr, &rval, PARALLEL_FC_ENf, parallel_fc_en);

    return(WRITE_XLPORT_FLOW_CONTROL_CONFIGr(unit, phy_acc, rval));
}

int pm4x10_port_lane_map_set(int unit, int port, pm_info_t pm_info, const phymod_lane_map_t* lane_map)
{
    SOC_INIT_FUNC_DEFS;

    PM_4x10_INFO(pm_info)->lane_map = *lane_map;


    SOC_FUNC_RETURN;
}

int pm4x10_port_lane_map_get(int unit, int port, pm_info_t pm_info, phymod_lane_map_t* lane_map)
{
    SOC_INIT_FUNC_DEFS;

    *lane_map = PM_4x10_INFO(pm_info)->lane_map;


    SOC_FUNC_RETURN;
}

int pm4x10_xphy_lane_attach_to_pm (int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_conn)
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
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    candidate_lane_conn = &(PM_4x10_INFO(pm_info)->lane_conn[phyn-1][lane_index]);

    sal_memcpy(candidate_lane_conn, lane_conn, sizeof(portmod_xphy_lane_connection_t));
    
    /* set the number of phys to current iphy. If nof_phy is already counted for this
       phyn, keep the same nof_phys. */
    if (PM_4x10_INFO(pm_info)->nof_phys[lane_index] <= phyn){
        PM_4x10_INFO(pm_info)->nof_phys[lane_index] = phyn+1;
    }

exit:
   SOC_FUNC_RETURN;
}

int pm4x10_xphy_lane_detach_from_pm (int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_conn)
{

    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int pm_is_active = 0, rv = 0;
    SOC_INIT_FUNC_DEFS;

    if (MAX_PHYN <= phyn){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }
    /* Making sure the port macro is not active */
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    SOC_PBMP_ITER(PM_4x10_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    candidate_lane_conn = &(PM_4x10_INFO(pm_info)->lane_conn[phyn-1][lane_index]);

    sal_memcpy( lane_conn, candidate_lane_conn, sizeof(portmod_xphy_lane_connection_t));
    portmod_xphy_lane_connection_t_init(unit, candidate_lane_conn); 

    /* if removing phyn, the number of phys need to be adjusted.  If there is xphy
        after this phyn, those will be invalidate too as nof_phys got adjusted. */
    if(phyn > 0 ){
        PM_4x10_INFO(pm_info)->nof_phys[lane_index] = phyn;
    }

exit:
   SOC_FUNC_RETURN;
}

int pm4x10_port_control_phy_timesync_set(int unit, int port, pm_info_t pm_info, const portmod_port_control_phy_timesync_t config, uint64 value)
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

int pm4x10_port_control_phy_timesync_get(int unit, int port, pm_info_t pm_info, const portmod_port_control_phy_timesync_t config, uint64* value)
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

int pm4x10_port_update_dynamic_state(int unit, int port, pm_info_t pm_info, uint32_t port_dynamic_state) {
    
    int rv = 0;
    pm4x10_t pm4x10_data = NULL;
    int port_index;
    uint32_t bitmap;
    SOC_INIT_FUNC_DEFS;


    rv = _pm4x10_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Unable to retrieve the port index")));
    }

    pm4x10_data      = pm_info->pm_data.pm4x10_db;
    if (PORTMOD_PORT_IS_AUTONEG_MODE_UPDATED(port_dynamic_state)) {
        /*we need to change the data base An_mode based on the new value */
        switch (port_dynamic_state >> 16 & 0xffff) { 
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37:
            pm4x10_data->port_config[port_index].port_init_config.an_mode = phymod_AN_MODE_CL37;
            PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(
                pm4x10_data->port_config[port_index].port_dynamic_state);
            break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM:
            pm4x10_data->port_config[port_index].port_init_config.an_mode = phymod_AN_MODE_CL37BAM;
            PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(
                pm4x10_data->port_config[port_index].port_dynamic_state);
            break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL73:
            pm4x10_data->port_config[port_index].port_init_config.an_mode = phymod_AN_MODE_CL73;
            PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(
                pm4x10_data->port_config[port_index].port_dynamic_state);
            break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM:
            pm4x10_data->port_config[port_index].port_init_config.an_mode = phymod_AN_MODE_CL73BAM;
            PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(
                pm4x10_data->port_config[port_index].port_dynamic_state);
            break;
        default:
            break;
        }
    } else if (PORTMOD_PORT_IS_DEFAULT_TX_PARAMS_UPDATED(port_dynamic_state)) {
        PORTMOD_PORT_DEFAULT_TX_PARAMS_UPDATED_SET(
            pm4x10_data->port_config[port_index].port_dynamic_state);
    }

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_phy_op_mode_get(int unit, int port, pm_info_t pm_info, phymod_operation_mode_t* val)
{
    uint32 bitmap;
    int    port_index, rv = 0;

    rv = _pm4x10_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if SOC_FAILURE(rv) return (rv);
    
    *val =  PM_4x10_INFO(pm_info)->port_config[port_index].phy_init_config.op_mode;

    return (rv);
}

int pm4x10_port_medium_config_set(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = portmod_port_phychain_medium_config_set(unit, port, medium, config);
    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm4x10_port_medium_config_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = portmod_port_phychain_medium_config_get(unit, port, medium, config);

    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm4x10_port_medium_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t* medium)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = portmod_port_phychain_medium_get(unit, port, medium);
    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm4x10_port_multi_get(int unit, int port, pm_info_t pm_info, portmod_multi_get_t* multi_get)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_multi_get(phy_access, nof_phys, multi_get));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_port_polarity_set(int unit, int port, pm_info_t pm_info, const phymod_polarity_t* polarity)
{
    SOC_INIT_FUNC_DEFS;

    PM_4x10_INFO(pm_info)->polarity = *polarity;


    SOC_FUNC_RETURN;
}

int pm4x10_port_polarity_get(int unit, int port, pm_info_t pm_info, phymod_polarity_t* polarity)
{
    SOC_INIT_FUNC_DEFS;

    *polarity = PM_4x10_INFO(pm_info)->polarity;


    SOC_FUNC_RETURN;
}

int pm4x10_pm_is_in_pm12x10(int unit, pm_info_t pm_info, int* in_pm12x10)
{
    SOC_INIT_FUNC_DEFS;

    if (PM_4x10_INFO(pm_info)->in_pm12x10){
        *in_pm12x10 = 1;
    } else {
        *in_pm12x10 = 0;
    }

    SOC_FUNC_RETURN;
}


#endif /* PORTMOD_PM4X10_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

