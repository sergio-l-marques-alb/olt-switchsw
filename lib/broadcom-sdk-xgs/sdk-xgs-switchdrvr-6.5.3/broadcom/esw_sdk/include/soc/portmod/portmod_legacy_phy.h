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

#ifndef PORTMOD_LEGACY_PHY_H
#define PORTMOD_LEGACY_PHY_H

int 
portmod_port_legacy_phy_probe(int unit, int port);
int
portmod_port_legacy_phy_init(int unit, int port);
int
portmod_port_legacy_phy_addr_get(int unit, int port);
int 
portmod_port_legacy_loopback_set(int unit, int port, phymod_loopback_mode_t lb, uint32_t enable);
int
portmod_port_legacy_loopback_get(int unit, int port, phymod_loopback_mode_t lb, uint32_t *enable);
int 
portmod_port_legacy_interface_config_set(int unit, int port, const phymod_phy_inf_config_t *config);
int 
portmod_port_legacy_interface_config_get(int unit, int port, phymod_phy_inf_config_t *config);
int 
portmod_port_legacy_power_set(int unit, int port, const phymod_phy_power_t *power);
int 
portmod_port_legacy_power_get(int unit, int port, phymod_phy_power_t *power); 
int 
portmod_port_legacy_tx_ln_ctrl_get(int unit, int port, phymod_phy_tx_lane_control_t* control); 
int 
portmod_port_legacy_tx_ln_ctrl_set(int unit, int port, const phymod_phy_tx_lane_control_t* control);
int 
portmod_port_legacy_rx_ln_ctrl_get(int unit, int port, phymod_phy_rx_lane_control_t* control);
int 
portmod_port_legacy_rx_ln_ctrl_set(int unit, int port, const phymod_phy_rx_lane_control_t* control);
int 
portmod_port_legacy_phy_control_set(int unit, int port, int phyn, int phy_lane, int sys_side,
                                    soc_phy_control_t control, uint32_t value);
int 
portmod_port_legacy_phy_control_get(int unit, int port, int phyn, int phy_lane, int sys_side,
                                        soc_phy_control_t control, uint32_t *value); 
int 
portmod_port_legacy_phy_link_get(int unit, int port, uint32_t *link);
int 
portmod_port_legacy_cl72_set(int unit, int port, uint32_t cl72_en);
int 
portmod_port_legacy_cl72_get(int unit, int port, uint32_t *cl72_en);
int 
portmod_port_legacy_fec_get(int unit, int port, uint32_t *fec_en);
int 
portmod_port_legacy_fec_set(int unit, int port, uint32_t fec_en);
int 
portmod_port_legacy_eee_set(int unit, int port, uint32_t eee_en);
int 
portmod_port_legacy_eee_get(int unit, int port, uint32_t *eee_en);
int 
portmod_port_legacy_advert_set(int unit, int port, const phymod_autoneg_ability_t* an_ability);
int 
portmod_port_legacy_advert_get(int unit, int port, phymod_autoneg_ability_t* an_ability);
int
portmod_port_legacy_ability_remote_get(int unit, int port, phymod_autoneg_ability_t* an_ability); 
int 
portmod_port_legacy_ability_local_get(int unit, int port, portmod_port_ability_t *portmod_ability);
int
portmod_port_legacy_an_get(int unit, int port, phymod_autoneg_control_t *an_config, uint32_t *an_done);
int
portmod_port_legacy_an_status_get(int unit, int port, phymod_autoneg_status_t *an_status);
int
portmod_port_legacy_an_set(int unit, int port,  const phymod_autoneg_control_t *an_config);
int portmod_port_legacy_tx_set(const phymod_phy_access_t* phy_access, const int chain_length, const phymod_tx_t* tx);
int
portmod_port_legacy_phy_mdix_set(int unit, int port, soc_port_mdix_t mode);
int
portmod_port_legacy_phy_mdix_get(int unit, int port, soc_port_mdix_t *mode);
int
portmod_port_legacy_phy_mdix_status_get(int unit, soc_port_t port,
                                             soc_port_mdix_status_t *status);
int
portmod_legacy_ext_phy_init(int unit, pbmp_t pbmp);
int
portmod_port_legacy_phy_reset_set(int unit, int port);

int portmod_port_legacy_timesync_config_set(int unit, int port,
                                            const portmod_phy_timesync_config_t* config);
int portmod_port_legacy_timesync_config_get(int unit, int port,
                                            portmod_phy_timesync_config_t* config);
int portmod_port_legacy_control_phy_timesync_set( int unit, int port,
                                                  const soc_port_control_phy_timesync_t type,
                                                  const uint64 value);

int portmod_port_legacy_control_phy_timesync_get( int unit, int port,
                                                  const soc_port_control_phy_timesync_t type,
                                                  const uint64* value);
int portmod_port_legacy_phy_link_up_event(int unit, int port);
int portmod_port_legacy_phy_link_down_event(int unit, int port);

int portmod_port_legacy_medium_config_set(int unit, int port, soc_port_medium_t medium,
                                          soc_phy_config_t *cfg);
int portmod_port_legacy_medium_config_get(int unit, int port, soc_port_medium_t medium,
                                          soc_phy_config_t *cfg);
int portmod_port_legacy_medium_get(int unit, int port, soc_port_medium_t* medium);
#endif /* PORTMOD_LEGACY_PHY_H*/
