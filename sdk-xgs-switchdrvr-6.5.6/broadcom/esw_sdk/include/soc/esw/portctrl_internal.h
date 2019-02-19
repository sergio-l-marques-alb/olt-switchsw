/* 
 * $Id:$
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
 * File:        portctrl_internal.h
 * Purpose:     SDK SOC Port Control Glue Layer
 */

#ifndef   _SOC_ESW_PORTCTRL_INTERNAL_H_
#define   _SOC_ESW_PORTCTRL_INTERNAL_H_

#ifdef PORTMOD_SUPPORT

#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>

/* utility functions called from device specific portmod code */
extern int
soc_esw_portctrl_pm_user_access_alloc(int unit, int num,
                   portmod_default_user_access_t **user_acc);
extern void
soc_esw_portctrl_dump_txrx_lane_map(int unit, int first_port, int logical_port,
                   int core_num, uint32 txlane_map_b, uint32 rxlane_map_b,
                   uint32 txlane_map, uint32 rxlane_map);
extern int soc_esw_portctrl_reset_tsc0_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc1_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc2_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc3_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_qtsce_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_gphy_cb(int unit, int port, uint32 in_reset);

extern int
soc_esw_portctrl_config_get(int unit, soc_port_t port, 
                            portmod_port_interface_config_t* interface_config,
                            portmod_port_init_config_t* init_config, 
                            phymod_operation_mode_t *phy_op_mode);


#endif /* PORTMOD_SUPPORT */

#endif /* _SOC_ESW_PORTCTRL_INTERNAL_H_ */

