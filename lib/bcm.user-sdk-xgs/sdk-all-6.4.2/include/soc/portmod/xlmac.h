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

#ifndef _PORTMOD_XLMAC_H_
#define _PORTMOD_XLMAC_H_

#include <soc/portmod/portmod.h>

#define XLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG   (1)
#define XLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN  (2)

#define XLMAC_ENABLE_RX  (1)
#define XLMAC_ENABLE_TX  (2)

#define XLMAC_INIT_F_RX_STRIP_CRC       0x1
#define XLMAC_INIT_F_TX_APPEND_CRC      0x2
#define XLMAC_INIT_F_TX_REPLACE_CRC     0x4
#define XLMAC_INIT_F_IS_HIGIG           0x8
#define XLMAC_INIT_F_IPG_CHECK_DISABLE  0x10

int xlmac_init          (int unit, soc_port_t p, uint32_t f);
int xlmac_speed_set     (int unit, soc_port_t p, int speed);
int xlmac_encap_set     (int unit, soc_port_t p, int flags, portmod_encap_t encap);
int xlmac_encap_get     (int unit, soc_port_t p, portmod_encap_t *encap);
int xlmac_enable_set    (int unit, soc_port_t p, int flags, int enable);
int xlmac_enable_get    (int unit, soc_port_t p, int flags, int *enable);
int xlmac_loopback_set  (int unit, soc_port_t p, portmod_loopback_mode_t lb, int enable);
int xlmac_loopback_get  (int unit, soc_port_t p, portmod_loopback_mode_t lb, int *enable);
int xlmac_tx_mac_sa_set (int unit, soc_port_t p, sal_mac_addr_t mac);
int xlmac_rx_mac_sa_set (int unit, soc_port_t p, sal_mac_addr_t mac);

int xlmac_soft_reset_set     (int unit, soc_port_t p, int enable);
int xlmac_soft_reset_get     (int unit, soc_port_t p, int *enable);
int xlmac_rx_vlan_tag_set    (int unit, soc_port_t p, int outer_vlan_tag, int inner_vlan_tag);
int xlmac_rx_vlan_tag_get    (int unit, soc_port_t p, int *outer_vlan_tag, int *inner_vlan_tag);
int xlmac_rx_max_size_set    (int unit, soc_port_t p, int value);
int xlmac_rx_max_size_get    (int unit, soc_port_t p, int *value);
int xlmac_tx_averge_ipg_set  (int unit, soc_port_t p, int val);
int xlmac_runt_threshold_set (int unit, soc_port_t p, int value);
int xlmac_runt_threshold_get (int unit, soc_port_t p, int *value);
int xlmac_sw_link_status_set (int unit, soc_port_t p, int link);

int xlmac_tx_preamble_length_set    (int unit, soc_port_t p, int length);
int xlmac_sw_link_status_enable_set (int unit, soc_port_t p, int enable);
int xlmac_sw_link_status_enable_get (int unit, soc_port_t p, int *enable);

/******************************************************************************* 
 Remote/local Fault
********************************************************************************/
int xlmac_local_fault_status_get  (int unit, soc_port_t p, int *status);
int xlmac_remote_fault_status_get (int unit, soc_port_t p, int *status);
int xlmac_local_fault_control_set (int unit, soc_port_t p, const portmod_local_fault_control_t *control);
int xlmac_local_fault_control_get (int unit, soc_port_t p, portmod_local_fault_control_t *control);
int xlmac_remote_fault_control_get(int unit, soc_port_t p, portmod_remote_fault_control_t *control);
int xlmac_remote_fault_control_set(int unit, soc_port_t p, const portmod_remote_fault_control_t *control);

/******************************************************************************* 
 Flow Control
********************************************************************************/
int xlmac_pfc_control_set   (int unit, soc_port_t p, const portmod_pfc_control_t *control);
int xlmac_pfc_control_get   (int unit, soc_port_t p, portmod_pfc_control_t *control);
int xlmac_llfc_control_set  (int unit, soc_port_t p, const portmod_llfc_control_t *control);
int xlmac_llfc_control_get  (int unit, soc_port_t p, portmod_llfc_control_t *control);
int xlmac_pause_control_set (int unit, soc_port_t p, const portmod_pause_control_t *control);
int xlmac_pause_control_get (int unit, soc_port_t p, portmod_pause_control_t *control);

#endif /*_PORTMOD_XLMAC_H_*/
