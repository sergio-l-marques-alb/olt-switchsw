/*
 * $Id: $
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
 * File: jer_nif.h
 */

#ifndef __JER_NIF_INCLUDED__

#define __JER_NIF_INCLUDED__


#define SOC_JER_NIF_PLL_TYPE_PMH_LAST_PHY_LANE    23
#define SOC_JER_NIF_PLL_TYPE_PML0_LAST_PHY_LANE   83
#define SOC_JER_NIF_PLL_TYPE_PML1_LAST_PHY_LANE   143

/* PLL types enum*/
typedef enum
{
  SOC_JER_NIF_PLL_TYPE_PMH,
  SOC_JER_NIF_PLL_TYPE_PML0,
  SOC_JER_NIF_PLL_TYPE_PML1,

  /* Total number of PLL types. */
  SOC_JER_NIF_NOF_PLL_TYPE
} SOC_JER_NIF_PLL_TYPE;


int soc_jer_portmod_init(int unit);
int soc_jer_portmod_post_init(int unit, soc_pbmp_t* ports);
int soc_jer_portmod_deinit(int unit);
int soc_jer_portmod_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
int soc_jer_portmod_port_enable_set(int unit, soc_port_t port, uint32 mac_only, int enable);
int soc_jer_portmod_port_enable_get(int unit, soc_port_t port, int* enable);
int soc_jer_portmod_port_speed_set(int unit, soc_port_t port, int speed);
int soc_jer_portmod_port_speed_get(int unit, soc_port_t port, int* speed);
int soc_jer_portmod_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf);
int soc_jer_portmod_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
int soc_jer_portmod_port_link_state_get(int unit, soc_port_t port, int clear_status, int *is_link_up, int *is_latch_down);
int soc_jer_portmod_is_supported_encap_get(int unit, int mode, int* is_supported);
int soc_jer_portmod_autoneg_set(int unit, soc_port_t port, int enable);
int soc_jer_portmod_autoneg_get(int unit, soc_port_t port, int* enable) ;
int soc_jer_portmod_pfc_refresh_set(int unit, soc_port_t port, int value);
int soc_jer_portmod_pfc_refresh_get(int unit, soc_port_t port, int* value);
int soc_jer_port_phy_reset(int unit, soc_port_t port);
int soc_jer_port_mac_sa_set(int unit, int port, sal_mac_addr_t mac_sa);
int soc_jer_port_mac_sa_get(int unit, int port, sal_mac_addr_t mac_sa);
int soc_jer_port_pll_type_get(int unit, soc_port_t port, SOC_JER_NIF_PLL_TYPE *pll_type);
int soc_jer_port_autoneg_get(int unit, soc_port_t port, int *autoneg);
int soc_jer_port_duplex_set(int unit, soc_port_t port, int duplex);
int soc_jer_port_fault_get(int unit, soc_port_t port, uint32 *flags);
int soc_jer_port_eee_enable_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_enable_get(int unit, soc_port_t port, uint32 *value);
int soc_jer_port_eee_tx_idle_time_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_tx_idle_time_get(int unit, soc_port_t port, uint32 *value);
int soc_jer_port_eee_tx_wake_time_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_tx_wake_time_get(int unit, soc_port_t port, uint32 *value);

/* Add QSGMIIs offset to phy port
   Input:  Physical lane in the range of 1-72
   Output: Physical port in the range of 1-144*/
int soc_jer_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy);
int soc_jer_qsgmii_offsets_add_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp);

/* Remove QSGMIIs offset from phy port
   Input:  Physical port in the range of 1-144
   Output: Physical lane in the range of 1-72*/
int soc_jer_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy);
int soc_jer_qsgmii_offsets_remove_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp);

int soc_jer_port_close_ilkn_path(int unit, int port); 
int soc_jer_port_open_path(int unit, soc_port_t port);


#endif /*__JER_NIF_INCLUDED__*/

