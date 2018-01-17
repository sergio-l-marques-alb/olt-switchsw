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


#ifndef _DNX_FABRIC_SHARED_H__H_
#define _DNX_FABRIC_SHARED_H__H_

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT



#define DNX_FABRIC_LANES_PER_CORE (4)
#define MAX_PORTS_PER_DNX_FABRIC_PM (4)

#include <soc/dcmn/error.h>
#include <soc/portmod/portmod_chain.h>

/* WB defines */
#define DNX_FABRIC_IS_BYPASSED_SET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_bypassed], &is_bypass)
#define DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_bypassed], is_bypass)

#define DNX_FABRIC_LANE2PORT_SET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], &port, lane)
#define DNX_FABRIC_LANE2PORT_GET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], port, lane)

#ifdef PORTMOD_DNX_FABRIC_SUPPORT

int dnx_fabric_lane_phychain_core_access_get(int unit, int lane_index, int max_cores, pm_info_t pm_info, 
                                phymod_core_access_t* core_access_arr);

int dnx_fabric_port_disable_init(int unit, int port, pm_info_t pm_info);

int dnx_fabric_interface_config_validate(int unit, pm_info_t pm_info, const portmod_port_interface_config_t* config);

int dnx_fabric_port_phy_index_get(int unit, int port, pm_info_t pm_info, uint32 *phy_index, uint32* bitmap);

/*wb vars in dnx_fabric buffer. 
  new value must append to the end.
  Remove values is dissallowed*/
typedef enum dnx_fabric_wb_vars {
    wb_is_probed = 0,
    wb_is_initialized = 1,
    wb_ports = 2,
    wb_polarity_rx = 3,
    wb_polarity_tx = 4,
    wb_lane_map = 5,
    wb_is_bypassed = 6,
    wb_lane2port_map = 7
}dnx_fabric_wb_vars_t;

typedef struct dnx_fabric_internal_s{
    pm_info_t *pms;
} dnx_fabric_internal_t;

struct dnx_fabric_s{
    soc_pbmp_t phys; 
    phymod_ref_clk_t ref_clk;
    phymod_access_t access;
    phymod_dispatch_type_t core_type;
    portmod_lane_connection_t phy_chain[MAX_PHYN][DNX_FABRIC_LANES_PER_CORE];
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f fw_loader;
    int nof_phys /* internal + external phys */;
    uint32 fmac_schan_id;
    uint32 fsrd_schan_id;
    uint32 fsrd_internal_quad;
    int first_phy_offset;
    int core_index;
    int is_over_nif;
    dnx_fabric_internal_t fabric_o_nif_data;
};

#endif 

#endif /*_DNX_FABRIC_SHARED_H_*/



