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
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PMNULL_SUPPORT

int pmNull_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
        
    SOC_INIT_FUNC_DEFS;

    *is_supported =  (interface == SOC_PORT_IF_NULL)? 1 : 0;
    
    SOC_FUNC_RETURN; 
    
}


int pmNull_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    pm_info->type         = pm_add_info->type;
    pm_info->unit         = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    SOC_FUNC_RETURN; 
}


int pmNull_pm_destroy(int unit, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}


int pmNull_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}


int pmNull_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}


int pmNull_port_detach(int unit, int port, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}


int pmNull_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}

int pmNull_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
   
    *enable = 0; 

        
    SOC_FUNC_RETURN; 
    
}


int pmNull_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int all_phy)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}

int pmNull_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        

    SOC_FUNC_RETURN; 
}


int pmNull_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        

    SOC_FUNC_RETURN; 
    
}

int pmNull_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        

    SOC_FUNC_RETURN; 
    
}


int pmNull_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores, int* is_most_ext)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        

    SOC_FUNC_RETURN; 
    
}


int pmNull_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* access, int* nof_phys, int* is_most_ext)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
}

int pmNull_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    SOC_INIT_FUNC_DEFS;
    
    *link = 0; 

    SOC_FUNC_RETURN; 
}

int pmNull_port_mac_reset_check(int unit, int port, pm_info_t pm_info,
                                int enable, int* reset)
{
    SOC_INIT_FUNC_DEFS;

    *reset = 0;

    SOC_FUNC_RETURN;
}

/*!
 * pmNull_port_duplex_set
 *
 * @brief duplex set.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable        -
 */
int pmNull_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return (0);
}


/*!
 * pmNull_port_duplex_get
 *
 * @brief duplex get.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        -
 */
int pmNull_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    *enable=1;
    return (0);
}

int pmNull_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status)
{
    an_status->enabled = 0;
    return (0);
}

int pmNull_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    return (0);
}

int pmNull_port_vlan_tag_set(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    return (0);
}

int pmNull_port_vlan_tag_get(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    return (0);
}

int pmNull_port_eee_set(int unit, int port, pm_info_t pm_info,const portmod_eee_t* eee)
{
    return (0);
}

int pmNull_port_eee_get(int unit, int port, pm_info_t pm_info, portmod_eee_t* eee)
{
    return (0);
}

int pmNull_port_ref_clk_get(int unit, soc_port_t port, pm_info_t pm_info, int *ref_clk)
{
    return (SOC_E_NONE);
}

int pmNull_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (SOC_E_NONE);
}

int pmNull_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (SOC_E_NONE);
}

int pmNull_port_autoneg_get (int unit, int port, pm_info_t pm_info, 
                             phymod_autoneg_control_t* an)
{
    an->enable = 0;
    return (SOC_E_NONE);
}

int pmNull_port_autoneg_set (int unit, int port, pm_info_t pm_info, 
                             const phymod_autoneg_control_t* an)
{
    return (SOC_E_NONE);
}

int pmNull_port_ability_advert_get(int unit, int port, pm_info_t pm_info, 
                                 portmod_port_ability_t* ability)
{
    return (SOC_E_NONE);
}

int pmNull_port_ability_advert_set(int unit, int port, pm_info_t pm_info, 
                                 portmod_port_ability_t* ability)
{
    return (SOC_E_NONE);
}

int pmNull_port_pause_control_set(int unit, int port, pm_info_t pm_info, 
                                  const portmod_pause_control_t* control)
{
    return (SOC_E_NONE);
}

int pmNull_port_pause_control_get(int unit, int port, pm_info_t pm_info, 
                                  portmod_pause_control_t* control)
{
    return (SOC_E_NONE);
}

int pmNull_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (SOC_E_NONE);
}


int pmNull_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (SOC_E_NONE);
}

int pmNull_port_ability_local_get (int unit, int port, pm_info_t pm_info, 
                                   portmod_port_ability_t* ability)
{
    return (SOC_E_NONE);
}

int pmNull_port_max_packet_size_set (int unit, int port,
                                     pm_info_t pm_info, int value)
{
    return (SOC_E_NONE);
}

int pmNull_port_max_packet_size_get (int unit, int port,
                                     pm_info_t pm_info, int* value)
{
    *value = 0x3fe8; 
    return (SOC_E_NONE);
}

int pmNull_port_local_fault_status_get(int unit, int port,
                                       pm_info_t pm_info, int* value)
{
    *value=0;
    return (SOC_E_NONE);
}

int pmNull_port_remote_fault_status_get(int unit, int port,
                                        pm_info_t pm_info, int* value)
{
    *value=0;
    return (SOC_E_NONE);
}
#endif /* PORTMOD_PMNULL_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
