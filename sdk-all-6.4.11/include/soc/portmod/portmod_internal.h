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

#ifndef _PORTMOD_INTERNAL_H_
#define _PORTMOD_INTERNAL_H_

#include <soc/portmod/portmod.h>
#include <appl/diag/diag.h>

#define MAX_PMS_PER_PHY (3)
#define MAX_VARS_PER_BUFFER (13)
#define VERSION(_ver) (_ver)
#define PORTMOD_IS_LEGACY_PHY_GET(a) \
    ((portmod_default_user_access_t *)((a)->user_acc))->is_legacy_phy_present
#define PORTMOD_IS_LEGACY_PHY_SET(a) \
    ((portmod_default_user_access_t *)((a)->user_acc))->is_legacy_phy_present = 1
#define PORTMOD_USER_ACC_LPORT_GET(a) \
    ((portmod_default_user_access_t *)((a)->user_acc))->port
#define PORTMOD_USER_ACC_LPORT_SET(a, p) \
    ((portmod_default_user_access_t *)((a)->user_acc))->port = p
#define PORTMOD_USER_ACC_UNIT_GET(a) \
    ((portmod_default_user_access_t *)((a)->user_acc))->unit
#define PORTMOD_USER_ACC_CMD_FOR_PHY_GET(a, idx) \
    ((portmod_default_user_access_t *)((a[idx].access).user_acc))->cmd_for_phy
#define PORTMOD_USER_ACC_CMD_FOR_PHY_SET(a, idx) \
    ((portmod_default_user_access_t *)((a[idx].access).user_acc))->cmd_for_phy = 1
#define PORTMOD_USER_ACC_CMD_FOR_PHY_CLR(a, idx) \
    ((portmod_default_user_access_t *)((a[idx].access).user_acc))->cmd_for_phy = 0

#define PORTMOD_PORT_IS_DEFAULT_TX_PARAMS_UPDATED(port_dynamic_state)     (port_dynamic_state & 0x1)
#define PORTMOD_PORT_DEFAULT_TX_PARAMS_UPDATED_SET(port_dynamic_state)    (port_dynamic_state |= 0x1)
#define PORTMOD_PORT_DEFAULT_TX_PARAMS_UPDATED_CLR(port_dynamic_state)    (port_dynamic_state &= 0xfffe) 

#define PORTMOD_PORT_IS_AUTONEG_MODE_UPDATED(port_dynamic_state)          (port_dynamic_state & 0x2)
#define PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(port_dynamic_state)         (port_dynamic_state |= 0x2)
#define PORTMOD_PORT_AUTONEG_MODE_UPDATED_CLR(port_dynamic_state)         (port_dynamic_state &= 0xfffd) 

typedef struct pm4x10_s *pm4x10_t;
typedef struct pm4x10q_s *pm4x10q_t;
typedef struct pm4x25_s *pm4x25_t;
typedef struct pm12x10_s *pm12x10_t;
typedef struct dnx_fabric_s *dnx_fabric_t;
typedef struct pmOsIlkn_s *pmOsIlkn_t;

typedef union pm_db_u{
    pm4x10_t      pm4x10_db;
    pm4x10q_t      pm4x10q_db;
    dnx_fabric_t  dnx_fabric;
    pm4x25_t      pm4x25_db;
    pm12x10_t     pm12x10_db;
    pmOsIlkn_t    pmOsIlkn_db;
}pm_db_t;

struct pm_info_s{
    portmod_dispatch_type_t type;
    int unit;
    int wb_buffer_id;
    int wb_vars_ids[MAX_VARS_PER_BUFFER];
    pm_db_t pm_data;
};

typedef struct pm_info_s *pm_info_t;

typedef portmod_pm4x25_create_info_t portmod_pm4x25_create_info_internal_t;
typedef portmod_pm4x10_create_info_t portmod_pm4x10_create_info_internal_t;
typedef portmod_dnx_fabric_create_info_t portmod_dnx_fabric_create_info_internal_t;

typedef struct portmod_pm4x10q_create_info_internal_s{
    pm_info_t pm4x10;
    uint32 blk_id;
    void* qsgmii_user_acc;
    void* pm4x10_user_acc;
}portmod_pm4x10q_create_info_internal_t;

typedef struct portmod_pm12x10_create_info_internal_s{
    pm_info_t pm4x25;
    pm_info_t pm4x10[3];
    uint32 flags;
    int blk_id;
    int refclk_source;
}portmod_pm12x10_create_info_internal_t;


typedef struct portmod_os_ilkn_create_info_internal_s{
    int nof_aggregated_pms;
    pm_info_t *pms;
    int wm_high; /**< watermark high value */
    int wm_low; /**< watermark low value */
    uint8 is_over_fabric;
}portmod_os_ilkn_create_info_internal_t;

typedef union portmod_pm_specific_create_info_internal_u{
    portmod_dnx_fabric_create_info_internal_t dnx_fabric;
    portmod_pm4x25_create_info_internal_t pm4x25;
    portmod_pm4x10_create_info_internal_t pm4x10;
    portmod_pm4x10q_create_info_internal_t pm4x10q;
    portmod_pm12x10_create_info_internal_t pm12x10;
    portmod_os_ilkn_create_info_internal_t os_ilkn;
}portmod_pm_specific_create_info_internal_t;


typedef struct portmod_pm_create_info_internal_s {
    portmod_dispatch_type_t type; /**< PM type */
    soc_pbmp_t phys; /**< which PHYs belongs to the PM */
    portmod_pm_specific_create_info_internal_t pm_specific_info;
} portmod_pm_create_info_internal_t;

typedef struct portmod_bus_update_s {
    phymod_firmware_loader_f external_fw_loader;
    phymod_bus_t* default_bus;
    void* user_acc;
    int blk_id;
} portmod_bus_update_t;

typedef struct portmod_ext_phy_core_info_s
{
    phymod_core_access_t core_access; /**< core access */
    int is_initialized; /**< Phy is Initialized - need to convert to WB */
} portmod_ext_phy_core_info_t;

typedef struct portmod_xphy_core_info_s {
    phymod_core_access_t core_access; /**< core access */
    int core_initialized; /**< external phy init status */
    phymod_ref_clk_t ref_clk; /**< Phy ref clock */
    phymod_firmware_load_method_t fw_load_method;
    int  force_fw_load; /* flag to specify to force load the FW */
    phymod_polarity_t polarity; /**< Lanes Polarity */
    phymod_lane_map_t lane_map; /* lane map information for the xphy */
    int is_initialized; /**< Phy is Initialized - need to convert to WB */
    int wb_var_ids[3];
    uint8_t gearbox_enable;
    uint8_t pin_compatibility_enable;
    uint8_t phy_mode_reverse;
    phymod_core_init_config_t core_config; 
    uint16_t primary_core_num;
} portmod_xphy_core_info_t;

#define PM_DRIVER(pm_info) (__portmod__dispatch__[(pm_info)->type])

int portmod_pm_create_info_internal_t_init(int unit, portmod_pm_create_info_internal_t *create_info_internal);

int portmod_port_pm_type_get(int unit, int port, int *real_port, portmod_dispatch_type_t *type);
int portmod_pm_id_pm_type_get(int unit, int pm_id, portmod_dispatch_type_t *type);

int portmod_pm_info_get(int unit, int port, pm_info_t *pm_info);
int portmod_phy_pms_info_get(int unit, int phy, int max_pms, pm_info_t *pms_info, int *nof_pms);
int portmod_pm_info_type_get(int unit, int port, portmod_dispatch_type_t type, pm_info_t* pm_info);
int portmod_pm_info_from_pm_id_get(int unit, int pm_id, pm_info_t* pm_info);
int portmod_port_pm_id_get(int unit, int port, int *pm_id);
int portmod_port_interface_type_get(int unit, int port, soc_port_if_t *interface);
int portmod_port_main_core_access_get(int unit, int port, int phyn,
                                      phymod_core_access_t *core_access,
                                      int *nof_cores);

/*is interface type supported by PM should be supported by all PMs types*/
int portmod_pm_interface_type_is_supported(int unit, pm_info_t pm_info, soc_port_if_t interface, int* is_supported);

int portmod_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info);
int portmod_pm_destroy(int unit, pm_info_t pm_info);
int portmod_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info);
int portmod_port_attach(int unit, int port, const portmod_port_add_info_t* add_info);
int portmod_port_detach(int unit, int port);
int portmod_port_replace(int unit, int port, int new_port);

int portmod_next_wb_var_id_get(int unit, int *var_id);
int portmod_ext_phy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_lane_connection_t* lane_connection);
int portmod_ext_phy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_lane_connection_t* lane_connection);

/*!
 * portmod_port_mac_rsv_mask_set
 *
 * @brief  portmod port rsv mask set
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  rsv_mask        - rsv mask
 */
int portmod_port_mac_rsv_mask_set(int unit, int port, uint32 rsv_mask);

/*!
 * portmod_port_mib_reset_toggle
 *
 * @brief  portmod port mib reset toggle
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  port_index      - internal port
 */
int portmod_port_mib_reset_toggle(int unit, int port, int port_index);

int portmod_xphy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_connection);
int portmod_xphy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_connection);
int portmod_max_pms_get(int unit, int* max_pms);
int portmod_eyescan_diag_dispatch(int unit, soc_port_t port, args_t *a);
int portmod_phy_pm_type_get(int unit, int phy, portmod_dispatch_type_t *type);
int portmod_xphy_addr_set(int unit, int idx, int xphy_addr);
int portmod_xphy_db_addr_set(int unit, int idx, int xphy_addr);

#endif /*_PORTMOD_INTERNAL_H_*/
