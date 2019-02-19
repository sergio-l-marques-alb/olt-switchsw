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
 * Purpose:     SDK Port Control Glue Layer
 */

#ifndef   _BCM_INT_PORTCTRL_H_
#define   _BCM_INT_PORTCTRL_H_

#include <sal/types.h>
#include <soc/types.h>
#include <soc/esw/portctrl.h>
#include <bcm/types.h>
#include <soc/higig.h>




/* Functions */
extern int bcmi_esw_portctrl_probe(int unit, bcm_gport_t port, int init_flag, int *okay);
extern int bcmi_esw_portctrl_probe_pbmp(int unit,
                                        pbmp_t pbmp, pbmp_t *okay_pbmp);

extern int bcmi_esw_portctrl_ability_get(int unit, bcm_gport_t port,
                                         bcm_port_ability_t *port_ability,
                                         bcm_port_abil_t *ability_mask);

extern int bcmi_esw_portctrl_ability_remote_get(int unit, bcm_gport_t port,
                                                bcm_port_ability_t *ability,
                                                bcm_port_abil_t *ability_mask);

extern int bcmi_esw_portctrl_enable_get(int unit,
                                        bcm_gport_t port, int *enable);
extern int bcmi_esw_portctrl_enable_set(int unit,
                                        bcm_gport_t port, int enable);
extern int bcmi_esw_portctrl_phy_enable_set(int unit,
                                            bcm_gport_t port, int enable);

extern int bcmi_esw_portctrl_egress_queue_drain(int unit, bcm_gport_t port);
extern int bcmi_esw_portctrl_update(int unit, bcm_gport_t port, int link);
    
extern int bcmi_esw_portctrl_fault_get(int unit,
                                       bcm_gport_t port, uint32 *flags);

extern int bcmi_esw_portctrl_link_fault_get(int unit, bcm_gport_t port,
                                            int *local_fault,
                                            int *remote_fault);

extern int bcmi_esw_portctrl_loopback_get(int unit,
                                          bcm_gport_t port, int *loopback);
extern int bcmi_esw_portctrl_loopback_set(int unit,
                                          bcm_gport_t port, int loopback);

extern int bcmi_esw_portctrl_speed_get(int unit, bcm_gport_t port, int *speed);
extern int bcmi_esw_portctrl_speed_max(int unit, bcm_gport_t port, int *speed);
extern int bcmi_esw_portctrl_speed_set(int unit, bcm_gport_t port, int speed);

extern int bcmi_esw_portctrl_stk_my_modid_set(int unit, bcm_gport_t port,
                                              int my_modid);

extern int bcmi_esw_portctrl_pause_get(int unit, bcm_gport_t port,
                                       int *pause_tx, int *pause_rx);
extern int bcmi_esw_portctrl_pause_set(int unit, bcm_gport_t port,
                                       int pause_tx, int pause_rx);

extern int bcmi_esw_portctrl_pause_addr_get(int unit, bcm_gport_t port,
                                            bcm_mac_t mac);
extern int bcmi_esw_portctrl_pause_addr_set(int unit, bcm_gport_t port,
                                            bcm_mac_t mac);

extern int bcmi_esw_portctrl_eee_enable_get(int unit,
                                            bcm_gport_t port, int *enable);
extern int bcmi_esw_portctrl_eee_enable_set(int unit,
                                            bcm_gport_t port, int enable);

extern int bcmi_esw_portctrl_medium_get(int unit, bcm_gport_t port,
                                        bcm_port_medium_t *medium);

extern int bcmi_esw_portctrl_master_get(int unit, bcm_gport_t port, int *ms);
extern int bcmi_esw_portctrl_master_set(int unit, bcm_gport_t port, int ms);

extern int bcmi_esw_portctrl_interface_get(int unit, bcm_gport_t port,
                                           bcm_port_if_t *intf);
extern int bcmi_esw_portctrl_interface_set(int unit, bcm_gport_t port,
                                           bcm_port_if_t intf);

extern int bcmi_esw_portctrl_mdix_get(int unit, bcm_gport_t port,
                                      bcm_port_mdix_t *mode);
extern int bcmi_esw_portctrl_mdix_set(int unit, bcm_gport_t port,
                                      bcm_port_mdix_t mode);

extern int bcmi_esw_portctrl_mdix_status_get(int unit, bcm_gport_t port,
                                             bcm_port_mdix_status_t *status);
extern int bcmi_esw_portctrl_phy_get(int unit, bcm_gport_t port, uint32 flags,
                                     uint32 phy_reg_addr, uint32 *phy_data);
extern int bcmi_esw_portctrl_phy_multi_get(int unit, bcm_gport_t port, 
                                           uint32 flags, uint32 dev_addr, 
                                           uint32 offset, int max_size, 
                                           uint8 *data, int *actual_size);
extern int bcmi_esw_portctrl_phy_modify(int unit, bcm_gport_t port, 
                                        uint32 flags, uint32 phy_reg_addr,
                                        uint32 phy_data, uint32 phy_mask);
extern int bcmi_esw_portctrl_phy_set(int unit, bcm_gport_t port, uint32 flags,
                                     uint32 phy_reg_addr, uint32 phy_data);

extern int bcmi_esw_portctrl_phy_drv_name_get(int unit, bcm_gport_t port,
                                              char *name, int len);
extern int bcmi_esw_portctrl_phy_reset(int unit, bcm_gport_t port);

extern int bcmi_esw_portctrl_ability_advert_get(int unit, bcm_gport_t port,
                                     bcm_port_ability_t *port_ability,
                                     bcm_port_abil_t *ability_mask);
extern int bcmi_esw_portctrl_ability_advert_set(int unit, bcm_gport_t port, 
                                     bcm_port_ability_t *port_ability,
                                     bcm_port_abil_t ability_mask);

extern int bcmi_esw_portctrl_autoneg_get(int unit, bcm_gport_t port,
                                         int *autoneg);
extern int bcmi_esw_portctrl_autoneg_set(int unit, bcm_gport_t port,
                                         int autoneg);

extern int bcmi_esw_portctrl_duplex_get(int unit, bcm_gport_t port,
                                        int *duplex);
extern int bcmi_esw_portctrl_duplex_set(int unit, bcm_gport_t port,
                                        int duplex);
extern int bcmi_esw_portctrl_clear_rx_lss_status_set(int unit, bcm_gport_t port,
                                                     int local_fault,
                                                     int remote_fault);
extern int bcmi_esw_portctrl_clear_rx_lss_status_get(int unit, bcm_gport_t port,
                                                     int *local_fault,
                                                     int *remote_fault);

extern int bcmi_esw_portctrl_frame_max_get(int unit,
                                           bcm_gport_t port, int *size);
extern int bcmi_esw_portctrl_frame_max_set(int unit,
                                           bcm_gport_t port, int size);

extern int bcmi_esw_portctrl_encap_get(int unit, bcm_gport_t port, int *mode);
extern int bcmi_esw_portctrl_encap_set(int unit, bcm_gport_t port,
                                       int mode, int force);

extern int bcmi_esw_portctrl_encap_xport_set(int unit, bcm_gport_t port,
                                             int mode);
extern int bcmi_esw_portctrl_encap_higig_lite_set(int unit, bcm_gport_t port);
extern int bcmi_esw_portctrl_higig_mode_set(int unit, bcm_gport_t port,
                                            int higig_mode);
extern int bcmi_esw_portctrl_higig2_mode_set(int unit, bcm_gport_t port,
                                             int higig2_mode);

extern int bcmi_esw_portctrl_link_get(int unit, bcm_gport_t port,
                                      int hw, int *up);

extern int bcmi_esw_portctrl_mode_setup(int unit, bcm_gport_t port, int enable);
extern int bcmi_esw_portctrl_detach(int unit, pbmp_t pbmp, pbmp_t *detached);
extern int bcmi_esw_portctrl_ifg_set(int unit, bcm_gport_t port, int speed, 
                                     bcm_port_duplex_t duplex, int ifg);
extern int bcmi_esw_portctrl_ifg_get(int unit, bcm_gport_t port, int speed, 
                                     bcm_port_duplex_t duplex, int *ifg);
extern int bcmi_esw_portctrl_mac_rx_control(int unit, bcm_port_t port, uint8 optype, int *enable);
extern int bcmi_esw_portctrl_llfc_get(int unit, bcm_gport_t port, bcm_port_control_t type, int *value);
extern int bcmi_esw_portctrl_llfc_set(int unit, bcm_port_t port, bcm_port_control_t type, int value);
extern int bcmi_esw_portctrl_lag_failover_disable(int unit, bcm_gport_t port);
extern int bcmi_esw_portctrl_lag_failover_loopback_get(int unit, bcm_gport_t port, int* value);
extern int bcmi_esw_portctrl_lag_failover_status_toggle(int unit, bcm_gport_t port);
extern int bcmi_esw_portctrl_lag_remove_failover_lpbk_set(int unit, bcm_gport_t port, int value);
extern int bcmi_esw_portctrl_trunk_hwfailover_set(int unit, bcm_gport_t port, int hw_count);

extern int bcmi_esw_td2_portctrl_lanes_set(int unit, bcm_port_t port_base, int lanes);

extern int bcmi_esw_portctrl_hwfailover_enable_set(int unit, bcm_gport_t port,
                                                   int enable);
extern int bcmi_esw_portctrl_hwfailover_enable_get(int unit, bcm_gport_t port,
                                                   int *enable);
extern int bcmi_esw_portctrl_hwfailover_status_get(int unit, bcm_gport_t port,
                                                   int *status);

extern int bcmi_esw_portctrl_phy_control_set(int unit, bcm_gport_t port,
                                   bcm_port_phy_control_t type, uint32 value);
extern int bcmi_esw_portctrl_phy_control_get(int unit, bcm_gport_t port,
                                   bcm_port_phy_control_t type, uint32 *value);

extern int bcmi_esw_portctrl_control_validate(int unit, bcm_port_control_t type,
                                              int *valid);
extern int bcmi_esw_portctrl_control_set(int unit, bcm_gport_t port,
                                         bcm_port_control_t type, int value);
extern int bcmi_esw_portctrl_control_get(int unit, bcm_gport_t port,
                                         bcm_port_control_t type, int *value);

extern int bcmi_esw_portctrl_mode_get(int unit, bcm_gport_t port, int *mode);

extern int bcmi_esw_portctrl_e2e_tx_enable_get(int unit, bcm_gport_t port,
                                             int *enable);
extern int bcmi_esw_portctrl_e2e_tx_enable_set(int unit, bcm_gport_t port,
                                             int enable);
extern int bcmi_esw_portctrl_e2ecc_hdr_get(int unit, bcm_gport_t port,
                                          soc_higig_e2ecc_hdr_t *e2ecc_hdr);
extern int bcmi_esw_portctrl_e2ecc_hdr_set(int unit, bcm_gport_t port,
                                          soc_higig_e2ecc_hdr_t *e2ecc_hdr);
extern int bcmi_esw_portctrl_vlan_tpid_set(int unit, bcm_gport_t port, int tpid);
#endif /* _BCM_INT_PORTCTRL_H_ */
