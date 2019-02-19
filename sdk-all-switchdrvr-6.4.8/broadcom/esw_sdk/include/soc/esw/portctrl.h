/* 
 * $Id:$
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
 * File:        portctrl.h
 * Purpose:     SDK SOC Port Control Glue Layer
 */

#ifndef   _SOC_ESW_PORTCTRL_H_
#define   _SOC_ESW_PORTCTRL_H_

#include <soc/feature.h>
#include <soc/property.h>
#include <soc/phyctrl.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/esw/port.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>
#endif

#define SOC_USE_PORTCTRL(_unit)                 \
    (soc_feature(_unit, soc_feature_portmod))

#ifdef PORTMOD_SUPPORT
#define SOC_PORTCTRL_NOT_INITIALIZED   (0)
#define SOC_PORTCTRL_INITIALIZED       (1)

/* PORT MACRO PM */
#define SOC_PM12X10_PM4X10_COUNT      (3)
#define SOC_PM4X10_NUM_LANES          (4)
#define SOC_PM4X10_LANE_MASK          (0xF)
#define SOC_PM12X10_NUM_LANES         (10)
#define SOC_PM12X10_LANE_MASK         (0x3FF)
#define SOC_PM4X25_NUM_LANES          (4)
#define SOC_PM4X25_LANE_MASK          (0xF)
#endif

/* device-specific function vectors */
extern soc_portctrl_functions_t soc_td2p_portctrl_func;
extern soc_portctrl_functions_t soc_th_portctrl_func;
extern soc_portctrl_functions_t soc_gh_portctrl_func;

/*
 * Define:
 *      PORTMOD_SUCCESS
 *      PORTMOD_FAILURE
 *      PORTMOD_IF_ERROR_RETURN
 * Purpose:
 *      Checks PortMod and PhyMod library error return code.
 *      PORTMOD_IF_ERROR_RETURN() causes routine to return on error.
 * Note:
 *      Currently, the PortMod functions return error codes are the same as
 *      the SHARED error codes, which are used by the BCM and SOC layers.
 *      Phymod functions error codes are also the same as the
 *      SHARED error codes.
 */
#define PORTMOD_SUCCESS(_rv)           _SHR_E_SUCCESS(_rv)
#define PORTMOD_FAILURE(_rv)           _SHR_E_FAILURE(_rv)
#define PORTMOD_IF_ERROR_RETURN(_op)   _SHR_E_IF_ERROR_RETURN(_op)

/* This structrure is used to pass information to Portmod API during Flexport
 * reconfiguration of PGW. NOte it is used for TD2+ only
 */
typedef struct soc_esw_portctrl_pgw_s {
    int mode;
    int lanes;
    int port_index;
    int flags;
} soc_esw_portctrl_pgw_t;

/* Port type used in PortMod functions */
typedef int portctrl_pport_t;

extern int
soc_esw_portctrl_init(int unit);
extern int
soc_esw_portctrl_deinit(int unit);

extern int
soc_portctrl_software_deinit(int unit);

extern int
soc_esw_portctrl_add(int unit, soc_port_t port, int init_flag);

extern int
soc_esw_portctrl_delete(int unit, soc_port_t port);

extern int
soc_esw_portctrl_pgw_reconfigure(int unit, soc_port_t port,
                                 int phy_port, soc_esw_portctrl_pgw_t *data);

extern int
soc_esw_portctrl_frame_max_set(int unit, soc_port_t port, int size);

extern int
soc_esw_portctrl_frame_max_get(int unit, soc_port_t port, int *size);

extern int
soc_portctrl_led_chain_config(int unit, int port, int value);

extern char *
soc_portctrl_phy_name_get(int unit, int port);

extern int
soc_portctrl_port_mode_set(int unit, int port, int mode);
extern int
soc_portctrl_port_mode_get(int unit, int port, int *mode, int *lanes);

extern int
soc_portctrl_phy_control_set(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 value);
extern int
soc_portctrl_phy_control_get(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 *value);
extern int
soc_portctrl_phy_timesync_config_set(int unit, soc_port_t port,
                                     soc_port_phy_timesync_config_t *conf);
extern int
soc_portctrl_phy_timesync_config_get(int unit, soc_port_t port,
                                     soc_port_phy_timesync_config_t *conf);
extern int
soc_portctrl_control_phy_timesync_set(int unit, soc_port_t port,
                                      soc_port_control_phy_timesync_t type,
                                      uint64 value);
extern int
soc_portctrl_control_phy_timesync_get(int unit, soc_port_t port,
                                      soc_port_control_phy_timesync_t type,
                                      uint64 *value);

extern int
soc_esw_portctrl_encap_get(int unit, soc_port_t port, int *mode);

extern int
soc_esw_portctrl_port_to_phyaddr(int unit, soc_port_t port, uint8* phy_addr);

extern int
soc_esw_portctrl_functions_register(int unit, soc_driver_t *drv);

extern int
soc_esw_portctrl_speed_get(int unit, soc_port_t port, int *speed);

#ifdef PORTMOD_SUPPORT
extern int
soc_esw_portctrl_init_check(int unit);

/* utility functions called from device specific portmod code */
extern int
soc_esw_portctrl_pm_user_access_alloc(int unit, int num,
                   portmod_default_user_access_t **user_acc);
extern void
soc_esw_portctrl_dump_txrx_lane_map(int unit, int first_port, int logical_port,
                   int core_num, uint32 txlane_map_b, uint32 rxlane_map_b,
                   uint32 txlane_map, uint32 rxlane_map);
extern int
soc_esw_portctrl_setup_ext_phy_add(int unit, soc_pbmp_t phy_ports,
                               portmod_dispatch_type_t type);
extern int soc_esw_portctrl_reset_tsc0_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc1_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc2_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc3_cb(int unit, int port, uint32 in_reset);

extern int
soc_esw_portctrl_config_get(int unit, soc_port_t port, 
                            portmod_port_interface_config_t* interface_config,
                            portmod_port_init_config_t* init_config, 
                            phymod_operation_mode_t *phy_op_mode);

#endif /* PORTMOD_SUPPORT */

#endif /* _SOC_ESW_PORTCTRL_H_ */

