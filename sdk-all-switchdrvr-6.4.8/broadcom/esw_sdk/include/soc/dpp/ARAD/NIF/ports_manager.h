/*
 * $Id: ports_manager.h,v 1.11 Broadcom SDK $
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
 * SOC NIF PORTS MANAGER H
 */
 
#ifndef _SOC_NIF_PORTS_MANAGER_H_
#define _SOC_NIF_PORTS_MANAGER_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

/*init*/
soc_error_t soc_pm_init(int unit, soc_pbmp_t all_phy_pbmp, uint32 first_direct_port);
soc_error_t soc_pm_mac_reset_set(int unit, soc_port_t port, uint32 is_in_reset);

/*ports managment*/
soc_error_t soc_pm_port_remove(int unit, soc_port_t port);
soc_error_t soc_pm_speed_set(int unit, soc_port_t port, int speed); 
soc_error_t soc_pm_higig_set(int unit, soc_port_t port, uint32 is_higig);
soc_error_t soc_pm_ports_init(int unit, soc_pbmp_t pbmp, soc_pbmp_t* okay_ports);
soc_error_t soc_pm_port_deinit(int unit, soc_port_t port);
soc_error_t soc_pm_enable_set(int unit, soc_port_t port, uint32 enable);
soc_error_t soc_pm_mac_loopback_set(int unit, soc_port_t port, uint32 lb);
soc_error_t soc_pm_port_clp_reset(int unit, soc_port_t port, int full_reset);
soc_error_t soc_pm_port_recover(int unit, soc_pbmp_t *okay_nif_ports);
soc_error_t soc_pm_frame_max_set(int unit, soc_port_t port, int size);
soc_error_t soc_pm_ilkn_dynamic_num_of_lanes_set(int unit, soc_port_t port, int lanes_number);
soc_error_t soc_pm_mac_enable_set(int unit, soc_port_t port, uint32 enable);
soc_error_t soc_pm_mac_speed_set(int unit, soc_port_t port, uint32 speed);
soc_error_t soc_pm_tx_remote_fault_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_pm_fault_remote_enable_set(int unit, soc_port_t port, uint32 value);
soc_error_t soc_pm_fault_local_enable_set(int unit, soc_port_t port, uint32 value);

/*getters*/
soc_error_t soc_pm_ilkn_id_get(int unit,soc_port_t port, uint32* ilkn_id);
soc_error_t soc_pm_default_speed_get(int unit, soc_port_t port, int* speed);
soc_error_t soc_pm_speed_get(int unit, soc_port_t port, int* speed);
soc_error_t soc_pm_enable_get(int unit, soc_port_t port, uint32* enable);
soc_error_t soc_pm_master_get(int unit, soc_port_t port, soc_port_t* master_port);
soc_error_t soc_pm_mac_loopback_get(int unit, soc_port_t port, uint32* lb);
soc_error_t soc_pm_frame_max_get(int unit, soc_port_t port, int* size);
soc_error_t soc_pm_local_fault_get(int unit, soc_port_t port, int* local_fault);
soc_error_t soc_pm_remote_fault_get(int unit, soc_port_t port, int* remote_fault);
soc_error_t soc_pm_local_fault_clear(int unit, soc_port_t port);
soc_error_t soc_pm_remote_fault_clear(int unit, soc_port_t port);
soc_error_t soc_pm_pad_size_set(int unit, soc_port_t port, int value);
soc_error_t soc_pm_pad_size_get(int unit, soc_port_t port, int *value);
soc_error_t soc_pm_mac_ability_get(int unit, soc_port_t port, soc_port_ability_t *mac_ability);
soc_error_t soc_pm_ilkn_dynamic_num_of_lanes_get(int unit, soc_port_t port, uint32 *lanes_number);
soc_error_t soc_pm_tx_remote_fault_enable_get(int unit, soc_port_t port, int *enable);
soc_error_t soc_pm_serdes_quads_in_use_get(int unit, soc_port_t port, soc_pbmp_t* quads_in_use);
soc_error_t soc_pm_pfc_refresh_set(int unit, soc_port_t port, int value);
soc_error_t soc_pm_pfc_refresh_get(int unit, soc_port_t port, int *value);
soc_error_t soc_pm_mac_sa_set(int unit, int port, sal_mac_addr_t mac_sa);
soc_error_t soc_pm_mac_sa_get(int unit, int port, sal_mac_addr_t mac_sa);
soc_error_t soc_pm_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx);
soc_error_t soc_pm_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx);
soc_error_t soc_pm_pfc_set(int unit, soc_port_t port, int is_rx /*1 for rx, 0 tx*/, int pfc);
soc_error_t soc_pm_pfc_get(int unit, soc_port_t port, int is_rx /*1 for rx, 0 tx*/, int *pfc);
soc_error_t soc_pm_llfc_set(int unit, soc_port_t port, int is_rx /*1 for rx, 0 tx*/, int llfc);
soc_error_t soc_pm_llfc_get(int unit, soc_port_t port, int is_rx /*1 for rx, 0 tx*/, int *llfc);
soc_error_t soc_pm_fault_remote_enable_get(int unit, soc_port_t port, uint32 *value);
soc_error_t soc_pm_fault_local_enable_get(int unit, soc_port_t port, uint32 *value);

/*eee*/
soc_error_t soc_pm_eee_enable_get(int unit, soc_port_t port, uint32 *value);
soc_error_t soc_pm_eee_enable_set(int unit, soc_port_t port, uint32 value);
soc_error_t soc_pm_eee_tx_idle_time_get(int unit, soc_port_t port, uint32 *value);
soc_error_t soc_pm_eee_tx_idle_time_set(int unit, soc_port_t port, uint32 value);
soc_error_t soc_pm_eee_tx_wake_time_get(int unit, soc_port_t port, uint32 *value);
soc_error_t soc_pm_eee_tx_wake_time_set(int unit, soc_port_t port, uint32 value);
soc_error_t soc_pm_eee_link_active_duration_get(int unit, soc_port_t port, uint32* value);
soc_error_t soc_pm_eee_link_active_duration_set(int unit, soc_port_t port, uint32 value);

/*status*/
soc_error_t soc_pm_link_status_get(int unit, soc_port_t port, int *is_link_up, int *is_latch_down);
soc_error_t soc_pm_link_status_clear(int unit, soc_port_t port);


#endif /*_SOC_NIF_PORTS_MANAGER_H_*/

