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
 *
 */

#ifndef _PORTMOD_CLMAC_H_
#define _PORTMOD_CLMAC_H_

#include <soc/portmod/portmod.h>

#define CLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG  (1)
#define CLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN  (2)
#define CLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS      (4)
#define CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS     (1)
#define CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS      (1)

#define CLMAC_ENABLE_RX  (1)
#define CLMAC_ENABLE_TX  (2)

#define CLMAC_INIT_F_RX_STRIP_CRC               0x1
#define CLMAC_INIT_F_TX_APPEND_CRC              0x2
#define CLMAC_INIT_F_TX_REPLACE_CRC             0x4
#define CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE   0x8
#define CLMAC_INIT_F_IS_HIGIG                   0x10
#define CLMAC_INIT_F_IPG_CHECK_DISABLE          0x20

int clmac_init(int unit, soc_port_t port, uint32 init_flags);
int clmac_speed_set(int unit, soc_port_t port, int flags, int speed);
int clmac_speed_get     (int unit, soc_port_t port, int *speed);
int clmac_encap_set(int unit, soc_port_t port, int flags, portmod_encap_t encap);
int clmac_encap_get(int unit, soc_port_t port, int *flags, portmod_encap_t *encap);
int clmac_enable_set(int unit, soc_port_t port, int flags, int enable);
int clmac_enable_get(int unit, soc_port_t port, int flags, int *enable);
int clmac_duplex_set(int unit, soc_port_t port, int duplex);
int clmac_duplex_get(int unit, soc_port_t port, int *duplex);

int clmac_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t lb, int enable);
int clmac_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t lb, int *enable);
int clmac_rx_enable_set (int unit, soc_port_t port, int enable);
int clmac_rx_enable_get (int unit, soc_port_t port, int *enable);
int clmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac);
int clmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac);
int clmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac);
int clmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac);
int clmac_soft_reset_set     (int unit, soc_port_t p, int enable);
int clmac_soft_reset_get     (int unit, soc_port_t p, int *enable);
int clmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag);
int clmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag);
int clmac_rx_max_size_set(int unit, soc_port_t port, int value);
int clmac_rx_max_size_get(int unit, soc_port_t port, int *value);
int clmac_tx_average_ipg_set(int unit, soc_port_t port, int val);
int clmac_tx_average_ipg_get(int unit, soc_port_t port, int *val);
int clmac_runt_threshold_set(int unit, soc_port_t port, int value);
int clmac_runt_threshold_get(int unit, soc_port_t port, int *value);
int clmac_sw_link_status_set (int unit, soc_port_t p, int link);

int xlmac_tx_preamble_length_set    (int unit, soc_port_t p, int length);
int xlmac_sw_link_status_select_set (int unit, soc_port_t p, int enable);
int xlmac_sw_link_status_select_get (int unit, soc_port_t p, int *enable);

/******************************************************************************* 
 Remote/local Fault
********************************************************************************/
int clmac_remote_fault_control_get(int unit, soc_port_t port, portmod_remote_fault_control_t *control);
int clmac_remote_fault_control_set(int unit, soc_port_t port, const portmod_remote_fault_control_t *control);
int clmac_local_fault_control_get(int unit, soc_port_t port, portmod_local_fault_control_t *control);
int clmac_local_fault_control_set(int unit, soc_port_t port, const portmod_local_fault_control_t *control);
int clmac_local_fault_status_get  (int unit, soc_port_t port, int clear_status, int *status);
int clmac_remote_fault_status_get (int unit, soc_port_t port, int clear_status, int *status);
int clmac_clear_rx_lss_status_set (int unit, soc_port_t port, int lcl_fault, int rmt_fault);
int clmac_clear_rx_lss_status_get (int unit, soc_port_t port, int *lcl_fault, int *rmt_fault);

/******************************************************************************* 
 Flow Control
********************************************************************************/
int clmac_pfc_control_set(int unit, soc_port_t port,  const portmod_pfc_control_t *control);
int clmac_pfc_control_get(int unit, soc_port_t port, portmod_pfc_control_t *control);
int clmac_llfc_control_set(int unit, soc_port_t port,  const portmod_llfc_control_t *control);
int clmac_llfc_control_get(int unit, soc_port_t port, portmod_llfc_control_t *control);
int clmac_pause_control_set(int unit, soc_port_t port,  const portmod_pause_control_t *control);
int clmac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control);

/***************************************************************************** 
 SDK Support Functions 
******************************************************************************/
int clmac_eee_set(int u, int p, const portmod_eee_t* eee);
int clmac_eee_get(int u, int p, portmod_eee_t* eee);

int clmac_pfc_config_set (int u, int p, const portmod_pfc_config_t* pfc_cfg);
int clmac_pfc_config_get (int u, int p, const portmod_pfc_config_t* pfc_cfg);

int clmac_diag_fifo_status_get (int u, int p, const portmod_fifo_status_t* inf);

int clmac_frame_spacing_stretch_set (int u, int p, int spacing);
int clmac_frame_spacing_stretch_get (int u, int p, int *spacing);

int clmac_pass_control_frame_set(int unit, int port, int value);

int clmac_lag_failover_loopback_set(int unit, int port, int val);
int clmac_lag_failover_loopback_get(int unit, int port, int *val);

int clmac_lag_failover_en_get(int unit, int port, int *val);
int clmac_lag_failover_en_set(int unit, int port, int val);

int clmac_reset_fc_timers_on_link_dn_get (int unit, soc_port_t port, int *val);
int clmac_reset_fc_timers_on_link_dn_set (int unit, soc_port_t port, int val);

int clmac_lag_remove_failover_lpbk_get(int unit, int port, int *val);
int clmac_lag_remove_failover_lpbk_set(int unit, int port, int val);

int clmac_lag_failover_disable(int unit, int port);

int clmac_mac_ctrl_set    (int u, int p, uint64 ctrl);
int clmac_drain_cell_get  (int unit, int port, portmod_drain_cells_t *drain_cells);
int clmac_drain_cell_stop (int unit, int port, const portmod_drain_cells_t *drain_cells);
int clmac_drain_cell_start(int u, int p);

int clmac_txfifo_cell_cnt_get     (int u, int p, uint32* fval);
int clmac_egress_queue_drain_get  (int u, int p, uint64 *ctrl, int *rx);

int clmac_drain_cells_rx_enable   (int u, int p, int rx_en);
int clmac_egress_queue_drain_rx_en(int u, int p, int rx_en);
int clmac_reset_check(int u, int p, int enable, int *reset);

#endif /*_PORTMOD_CLMAC_H_*/
