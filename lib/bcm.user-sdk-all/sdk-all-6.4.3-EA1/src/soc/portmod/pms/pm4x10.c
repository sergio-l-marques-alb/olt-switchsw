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
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X10_SUPPORT


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

/*in case of PM12X10 the register access for each tsc done through diffrent memory*/
#ifdef PORTMOD_PM12X10_SUPPORT
STATIC
int
pm4x10_default_tsc4e_0_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXX_CXXPORT_WC_UCMEM_DATA_0m, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm4x10_default_tsc4e_0_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXX_CXXPORT_WC_UCMEM_DATA_0m, user_acc, core_addr, reg_addr, val);
}

int
pm4x10_default_tsc4e_1_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXX_CXXPORT_WC_UCMEM_DATA_1m, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm4x10_default_tsc4e_1_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXX_CXXPORT_WC_UCMEM_DATA_1m, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm4x10_default_tsc4e_2_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXX_CXXPORT_WC_UCMEM_DATA_2m, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm4x10_default_tsc4e_2_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXX_CXXPORT_WC_UCMEM_DATA_2m, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x10_tsc4e_0_bus = {
    "PM4X10 TSC4E_0 Bus",
    pm4x10_default_tsc4e_0_bus_read,
    pm4x10_default_tsc4e_0_bus_write,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY| PHYMOD_BUS_CAP_LANE_CTRL
};

phymod_bus_t pm4x10_tsc4e_1_bus = {
    "PM4X10 TSC4E_1 Bus",
    pm4x10_default_tsc4e_1_bus_read,
    pm4x10_default_tsc4e_1_bus_write,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

phymod_bus_t pm4x10_tsc4e_2_bus = {
    "PM4X10 TSC4E_2 Bus",
    pm4x10_default_tsc4e_2_bus_read,
    pm4x10_default_tsc4e_2_bus_write,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

#endif


int pm4x10_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
    *is_supported = FALSE;
    return SOC_E_NONE;
}


int pm4x10_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    pm_info->type = portmodDispatchTypePm4x10;
    pm_info->wb_buffer_id = wb_buffer_index;

    
    SOC_FUNC_RETURN; 
    
}


int pm4x10_pm_destroy(int unit, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_pm_bypass_set(int unit, pm_info_t pm_info, int enable){
    SOC_INIT_FUNC_DEFS;  

    SOC_FUNC_RETURN; 
}

int pm4x10_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    SOC_INIT_FUNC_DEFS;  

    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_detach(int unit, int port, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_ability_local_get(int unit, int port, pm_info_t pm_info, const portmod_port_ability_t* ability)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
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
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_prbs_config_get(int unit, int port, pm_info_t pm_info, int mode, int flags, phymod_prbs_t* config)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, int mode, int flags, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, int mode, int flags, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_prbs_status_get(int unit, int port, pm_info_t pm_info, int mode, int flags, phymod_prbs_status_t* status)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
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
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_max_packet_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
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
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_tx_drop_on_local_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_tx_drop_on_remote_fault_set(int unit, int port, pm_info_t pm_info, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_tx_drop_on_remote_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_local_fault_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_local_fault_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_local_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_fc_pfc_refresh_timer_set(int unit, int port, pm_info_t pm_info, int value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_fc_pfc_refresh_timer_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_fc_pfc_refresh_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_fc_pfc_refresh_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_fc_pfc_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_fc_pfc_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_fc_llfc_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10_port_fc_llfc_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x10_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


#endif /* PORTMOD_PM4X10_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
