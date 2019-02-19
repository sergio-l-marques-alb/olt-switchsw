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
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM12X10_SUPPORT


#define PM_12x10_INFO(pm_info) ((pm_info)->pm_data.pm12x10_db)
#define PM_4X25_INFO(pm_info) (PM_12x10_INFO(pm_info)->pm4x25_info)
#define PM_4X10_INFO(pm_info, i) (PM_12x10_INFO(pm_info)->pm4x10_info[i])
#define PM_4X25_DRIVER(pm_info) PM_DRIVER(PM_4X25_INFO(pm_info))
#define PM_4X10_DRIVER(pm_info, i) PM_DRIVER(PM_4X10_INFO(pm_info, i))


#define TOP_BLK_ID_OFFSET (4)


struct pm12x10_s{
    int blk_id;
    pm_info_t pm4x25_info;
    pm_info_t pm4x10_info[3];
};


int pm12x10_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
    *is_supported = FALSE;

    if(interface == SOC_PORT_IF_CAUI){
       *is_supported = TRUE;
    }
    return SOC_E_NONE;
}

int pm12x10_pm_destroy(int unit, pm_info_t pm_info)
{        
    if(pm_info->pm_data.pm12x10_db != NULL){
        sal_free(pm_info->pm_data.pm12x10_db);
        pm_info->pm_data.pm12x10_db = NULL;
    }
    return SOC_E_NONE;
}


int pm12x10_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    pm12x10_t pm12x10_data = NULL;
    int nof_phys = 0;
    SOC_INIT_FUNC_DEFS;

    SOC_PBMP_COUNT(pm_add_info->phys, nof_phys);
    if(nof_phys != 12){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("number of phys should be 12")));
    }
    pm12x10_data = sal_alloc(sizeof(*(pm_info->pm_data.pm12x10_db)), "specific_db");
    SOC_NULL_CHECK(pm12x10_data);

    /*PM12x10 Top*/
    pm_info->type = portmodDispatchTypePm12x10;
    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.pm12x10_db = pm12x10_data;
    pm_info->pm_data.pm12x10_db->blk_id =  pm_add_info->blk_id; 
    sal_memcpy(pm_info->pm_data.pm12x10_db->pm4x10_info, pm_add_info->pm_specific_info.pm12x10.pm4x10, sizeof(pm_info->pm_data.pm12x10_db->pm4x10_info));
    pm_info->pm_data.pm12x10_db->pm4x25_info = pm_add_info->pm_specific_info.pm12x10.pm4x25;

exit:
    if(SOC_FUNC_ERROR){
        pm12x10_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN; 
}


/*int _pm12x10_lane_map_validate(int unit, int port, )*/


int pm12x10_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
        
    uint32 nof_lanes = 0;
    int i = 0;
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    SOC_PBMP_COUNT(add_info->phys, nof_lanes);
    if(nof_lanes == 10){
        
    }else if(nof_lanes != 12){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("number of lanes should be 10 or 12")));
    }

    for(i = 0 ; i < 3 ; i++)
    {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_pm_bypass_set(unit, PM_4X10_INFO(pm_info,i), 1);
        _SOC_IF_ERR_EXIT(rv);
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_attach(unit, port, PM_4X10_INFO(pm_info,i), add_info);
        _SOC_IF_ERR_EXIT(rv);
    }

    rv = WRITE_CXX_CXXPORT_MODE_REGr(unit, port, 1); 
    _SOC_IF_ERR_EXIT(rv);
    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_attach(unit, port, PM_4X25_INFO(pm_info), add_info);
    
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_detach(int unit, int port, pm_info_t pm_info)
{
    int rv = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_detach(unit, port, PM_4X25_INFO(pm_info));
    _SOC_IF_ERR_EXIT(rv);

    
    rv = WRITE_CXX_CXXPORT_MODE_REGr(unit, port, 0); 
    _SOC_IF_ERR_EXIT(rv);
    for(i = 0 ; i < 3 ; i++)
    {
        PM_4X10_DRIVER(pm_info, i)->f_portmod_pm_bypass_set(unit, PM_4X10_INFO(pm_info,i), 0);
    }
    
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    int rv = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;
    if(enable){
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_enable_set(unit, port, PM_4X25_INFO(pm_info), flags, 1);
        _SOC_IF_ERR_EXIT(rv);
        for(i = 0 ; i < 3; i++){
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_enable_set(unit, port, PM_4X10_INFO(pm_info, i), flags, 1);
            _SOC_IF_ERR_EXIT(rv);
        }
    } else{
        for(i = 0 ; i < 3; i++){
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_enable_set(unit, port, PM_4X10_INFO(pm_info, i), flags, 1);
            _SOC_IF_ERR_EXIT(rv);
        }
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_enable_set(unit, port, PM_4X25_INFO(pm_info), flags, 0);
        _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    int rv = 0;    
    SOC_INIT_FUNC_DEFS;

    *enable = 0;
    if(PORTMOD_PORT_ENABLE_PHY_GET(flags)){
        rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_enable_get(unit, port, PM_4X10_INFO(pm_info, 0), flags, enable);
    } else{
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_enable_get(unit, port, PM_4X25_INFO(pm_info), flags, enable);
    }
    _SOC_IF_ERR_EXIT(rv);
       
exit:
    SOC_FUNC_RETURN;    
}


int pm12x10_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
    int rv = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_interface_config_set(unit, port, PM_4X25_INFO(pm_info), config);
    _SOC_IF_ERR_EXIT(rv);
    for ( i = 0 ; i < 3 ; i++){
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_interface_config_set(unit, port, PM_4X10_INFO(pm_info, i), config);
        _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN; 
    
}


int pm12x10_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN;   
}


int pm12x10_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
   int rv = 0;
   SOC_INIT_FUNC_DEFS;

   switch(loopback_type){
    case portmodLoopbackMacOuter:
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_loopback_set(unit, port, PM_4X25_INFO(pm_info), loopback_type, enable);
        _SOC_IF_ERR_EXIT(rv);
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPCS: /*slide*/
    case portmodLoopbackPhyRloopPCS: /*slide*/
         _SOC_IF_ERR_EXIT(portmod_common_phy_loopback_set(unit, port, loopback_type, enable));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }   
exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
   int rv = 0;
   SOC_INIT_FUNC_DEFS;

   switch(loopback_type){
    case portmodLoopbackMacOuter:
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_loopback_get(unit, port, PM_4X25_INFO(pm_info), loopback_type, enable);
        _SOC_IF_ERR_EXIT(rv);
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPCS: /*slide*/
    case portmodLoopbackPhyRloopPCS: /*slide*/
         _SOC_IF_ERR_EXIT(portmod_common_phy_loopback_get(unit, port, loopback_type, enable));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }   
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_ability_local_get(int unit, int port, pm_info_t pm_info, const portmod_port_ability_t* ability)
{    
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */
    
    SOC_FUNC_RETURN;  
}


int pm12x10_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{      
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
}


int pm12x10_port_prbs_config_set(int unit, int port, pm_info_t pm_info, int mode, int flags, const phymod_prbs_t* config)
{   
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_config_set(unit, port, flags, config));
    }
    else{    
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    } 
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_prbs_config_get(int unit, int port, pm_info_t pm_info, int mode, int flags, phymod_prbs_t* config)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_config_set(unit, port, flags, config));
    }
    else{    
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    } 
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, int mode, int flags, int enable)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_enable_set(unit, port, flags, enable));
    }
    else{    
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    } 
exit:
    SOC_FUNC_RETURN;  
}

int pm12x10_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, int mode, int flags, int* enable)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_enable_get(unit, port, flags, enable));
    }
    else{    
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    } 
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_prbs_status_get(int unit, int port, pm_info_t pm_info, int mode, int flags, phymod_prbs_status_t* status)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(portmod_common_phy_prbs_status_get(unit, port, flags, status));
    }
    else{    
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    } 
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_firmware_mode_set(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t fw_mode)
{      
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(portmod_common_phy_firmware_mode_set(unit, port, fw_mode));
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_firmware_mode_get(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t* fw_mode)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(portmod_common_phy_firmware_mode_get(unit, port, fw_mode));
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_runt_threshold_set(int unit, int port, pm_info_t pm_info, int value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_runt_threshold_set(unit, port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
    
}

int pm12x10_port_runt_threshold_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_runt_threshold_get(unit, port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN;    
}


int pm12x10_port_max_packet_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_max_packet_size_set(unit, port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_max_packet_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_max_packet_size_get(unit, port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN;    
}


int pm12x10_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pad_size_set(unit, port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN;  
}


int pm12x10_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pad_size_get(unit, port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN;  
}


int pm12x10_port_local_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_local_fault_status_get(unit, port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_remote_fault_status_get(unit, port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_pause_control_set(int unit, int port, pm_info_t pm_info, const portmod_pause_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pause_control_set(unit, port, PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_pause_control_get(int unit, int port, pm_info_t pm_info, portmod_pause_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pause_control_get(unit, port, PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_pfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_pfc_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pfc_control_set(unit, port, PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_pfc_control_get(int unit, int port, pm_info_t pm_info, portmod_pfc_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pfc_control_get(unit, port, PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_llfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_llfc_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_llfc_control_set(unit, port, PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_llfc_control_get(int unit, int port, pm_info_t pm_info, portmod_llfc_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_llfc_control_get(unit, port, PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;  
}


int pm12x10_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores)
{
    int i = 0;
    int rv = 0;
    int tmp_nof_cores;
    SOC_INIT_FUNC_DEFS;

    if(max_cores < 3){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("3 core structures are required, %d given"), max_cores));
    }
    for( i = 0 ; i < 3 ; i++){
        rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_core_access_get(unit, port, PM_4X10_INFO(pm_info, i), phyn, 1 , &core_access_arr[i], &tmp_nof_cores);
        _SOC_IF_ERR_EXIT(rv);
    }
    *nof_cores = 3;
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* access, int* nof_phys)
{
    int i = 0;
    int rv = 0;
    int tmp_nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(max_phys < 3){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("3 phy access structures are required, %d given"), max_phys));
    }
    for( i = 0 ; i < 3 ; i++){
        rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_phy_lane_access_get(unit, port, PM_4X10_INFO(pm_info, i), params, 1 , &access[i], &tmp_nof_phys);
        _SOC_IF_ERR_EXIT(rv);
    }
    *nof_phys = 3;
exit:
    SOC_FUNC_RETURN; 
    
}


#endif /* PORTMOD_PM12X10_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
