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
 *
 */

#ifndef _PORTMOD_UNIMAC_H_
#define _PORTMOD_UNIMAC_H_

#include <soc/portmod/portmod.h>

#define UNIMAC_INIT_F_AUTO_CFG               0x1

int unimac_init          (int unit, soc_port_t port, int init_flags);
int unimac_speed_set     (int unit, soc_port_t port, int speed);
int unimac_speed_get     (int unit, soc_port_t port, int *speed);
int unimac_encap_set     (int unit, soc_port_t port, portmod_encap_t encap);
int unimac_encap_get     (int unit, soc_port_t port, portmod_encap_t *encap);
int unimac_enable_set    (int unit, soc_port_t port, int enable);
int unimac_enable_get    (int unit, soc_port_t port, int *enable);
int unimac_duplex_set    (int unit, soc_port_t port, int duplex);
int unimac_duplex_get    (int unit, soc_port_t port, int *duplex);
int unimac_loopback_set  (int unit, soc_port_t port, int enable);
int unimac_loopback_get  (int unit, soc_port_t port, int *enable);
int unimac_rx_enable_set (int unit, soc_port_t port, int enable);
int unimac_rx_enable_get (int unit, soc_port_t port, int *enable);

int unimac_mac_sa_set (int unit, soc_port_t port, sal_mac_addr_t mac);
int unimac_mac_sa_get (int unit, soc_port_t port, sal_mac_addr_t mac);

int unimac_soft_reset_set     (int unit, soc_port_t port, int enable);
int unimac_soft_reset_get     (int unit, soc_port_t port, int *enable);
int unimac_rx_max_size_set    (int unit, soc_port_t port, int value);
int unimac_rx_max_size_get    (int unit, soc_port_t port, int *value);
int unimac_tx_average_ipg_set (int unit, soc_port_t port, int ipg_val);
int unimac_tx_average_ipg_get (int unit, soc_port_t port, int *ipg_val);
int unimac_rx_vlan_tag_set    (int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag);
int unimac_rx_vlan_tag_get    (int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag);

int unimac_tx_preamble_length_set    (int unit, soc_port_t port, int length);

/***************************************************************************** 
 SDK Support Functions 
******************************************************************************/
int unimac_eee_set (int unit, int port, const portmod_eee_t* eee);
int unimac_eee_get (int unit, int port, const portmod_eee_t* eee);

int unimac_pfc_config_set (int unit, int port, const portmod_pfc_config_t* pfc_cfg);
int unimac_pfc_config_get (int unit, int port, const portmod_pfc_config_t* pfc_cfg);

int unimac_pause_control_set(int unit, soc_port_t port, const portmod_pause_control_t *control);
int unimac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control);

int unimac_pfc_control_set(int unit, soc_port_t port, const portmod_pfc_control_t *control);
int unimac_pfc_control_get(int unit, soc_port_t port, portmod_pfc_control_t *control);

int unimac_diag_fifo_status_get (int unit, int port, const portmod_fifo_status_t* inf);

int unimac_pass_control_frame_set(int unit, int port, int value);

int unimac_mac_ctrl_set    (int unit, int port, uint32 ctrl);
int unimac_drain_cell_get  (int unit, int port, portmod_drain_cells_t *drain_cells);

#endif /*_PORTMOD_UNIMAC_H_*/
