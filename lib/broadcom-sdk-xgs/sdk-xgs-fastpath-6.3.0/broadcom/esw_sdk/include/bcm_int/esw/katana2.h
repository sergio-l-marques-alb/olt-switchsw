/*
 * $Id: katana2.h 1.15.2.3 Broadcom SDK $
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
 * File:        katana2.h
 * Purpose:     Function declarations for Trident  bcm functions
 */

#ifndef _BCM_INT_KATANA2_H_
#define _BCM_INT_KATANA2_H_
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <bcm/qos.h>

extern int bcm_kt2_port_cfg_init(int, bcm_port_t, bcm_vlan_data_t *);

#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_kt2_init(int unit, _field_control_t *fc);
extern int _bcm_field_kt2_qual_tcam_key_mask_get(int unit,
                                                 _field_entry_t *f_ent,
                                                 _field_tcam_t  *tcam
                                                 );
extern int _bcm_field_kt2_qual_tcam_key_mask_set(int unit,
                                                 _field_entry_t *f_ent,
                                                 unsigned       validf
                                                 );
extern int soc_katana2_port_lanes_set_post_operation(int unit, bcm_port_t port);
extern int _bcm_kt2_port_lanes_set_post_operation(int unit, bcm_port_t port);
extern int _bcm_kt2_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_kt2_port_lanes_get(int unit, bcm_port_t port, int *value);

#endif /* BCM_FIELD_SUPPORT */

extern int bcm_kt2_subport_init(int unit);
extern int bcm_kt2_subport_cleanup(int unit);
extern int bcm_kt2_subport_group_create(int unit,
                                     bcm_subport_group_config_t *config,
                                     bcm_gport_t *group);
extern int bcm_kt2_subport_group_destroy(int unit, bcm_gport_t group);
extern int bcm_kt2_subport_group_get(int unit, bcm_gport_t group,
                                     bcm_subport_group_config_t *config);
extern int bcm_kt2_subport_group_traverse(int unit, bcm_gport_t group,
                                     bcm_subport_port_traverse_cb cb,
                                     void *user_data);
extern int bcm_kt2_subport_port_add(int unit, bcm_subport_config_t *config,
                                   bcm_gport_t *port);
extern int bcm_kt2_subport_port_delete(int unit, bcm_gport_t port);
extern int bcm_kt2_subport_port_get(int unit, bcm_gport_t port,
                                   bcm_subport_config_t *config);
extern int bcm_kt2_subport_port_traverse(int unit,
                                      bcm_subport_port_traverse_cb cb,
                                        void *user_data);
extern int bcm_kt2_subport_group_linkphy_config_get(int unit, 
                          bcm_gport_t group, 
                          bcm_subport_group_linkphy_config_t *linkphy_config);
extern int bcm_kt2_subport_group_linkphy_config_set(int unit,
                          bcm_gport_t group,
                          bcm_subport_group_linkphy_config_t *linkphy_config);
extern int bcm_kt2_subport_port_stat_set(int unit, 
                          bcm_gport_t port, 
                          int stream_id, 
                          bcm_subport_stat_t stat_type, 
                          uint64 val);
extern int bcm_kt2_subport_port_stat_get(
                          int unit, 
                          bcm_gport_t port, 
                          int stream_id, 
                          bcm_subport_stat_t stat_type, 
                          uint64 *val);
extern int bcm_kt2_subport_group_resolve(int unit,
                              bcm_gport_t subport_group_gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *id);
extern int bcm_kt2_subport_port_resolve(int unit,
                              bcm_gport_t subport_port_gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *id);
extern int bcm_kt2_subport_counter_init(int unit);
extern int bcm_kt2_subport_counter_cleanup(int unit);

extern int bcm_kt2_subport_egr_subtag_dot1p_map_add(int unit,
                                        bcm_qos_map_t *map);
extern int bcm_kt2_subport_egr_subtag_dot1p_map_delete(int unit,
                                        bcm_qos_map_t *map);
extern int bcm_kt2_subport_subtag_port_tpid_set(int unit,
                                 bcm_gport_t gport, uint16 tpid);
extern int bcm_kt2_subport_subtag_port_tpid_get(int unit,
                                 bcm_gport_t gport, uint16 *tpid);
extern int bcm_kt2_subport_subtag_port_tpid_delete(int unit,
                                 bcm_gport_t gport, uint16 tpid);
extern int bcm_kt2_port_control_subtag_status_set(int unit,
                                 bcm_port_t port, int value);
extern int bcm_kt2_port_control_subtag_status_get(int unit,
                                 bcm_port_t port, int *value);
extern int bcm_kt2_oam_init(int unit);

extern int bcm_kt2_oam_detach(int unit);

extern int bcm_kt2_oam_group_create(int unit, bcm_oam_group_info_t *group_info);

extern int bcm_kt2_oam_group_get(int unit, bcm_oam_group_t group,
                                 bcm_oam_group_info_t *group_info);

extern int bcm_kt2_oam_group_destroy(int unit, bcm_oam_group_t group);

extern int bcm_kt2_oam_group_destroy_all(int unit);

extern int bcm_kt2_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb,
                                      void *user_data);

extern int bcm_kt2_oam_endpoint_create(int unit,
                                       bcm_oam_endpoint_info_t *endpoint_info);

extern int bcm_kt2_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint,
                                    bcm_oam_endpoint_info_t *endpoint_info);

extern int bcm_kt2_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint);

extern int bcm_kt2_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group);

extern int bcm_kt2_oam_endpoint_traverse(int unit, bcm_oam_group_t group,
                                         bcm_oam_endpoint_traverse_cb cb,
                                         void *user_data);

extern int bcm_kt2_oam_event_register(int unit, 
                                      bcm_oam_event_types_t event_types,
                                      bcm_oam_event_cb cb, void *user_data);

extern int bcm_kt2_oam_event_unregister(int unit, 
                                      bcm_oam_event_types_t event_types,
                                      bcm_oam_event_cb cb);

extern int bcm_kt2_oam_endpoint_action_set(int unit, bcm_oam_endpoint_t id, 
                                           bcm_oam_endpoint_action_t *action); 

extern int _bcm_kt2_port_control_oam_loopkup_with_dvp_set(int unit, 
                                                          bcm_port_t port, 
                                                          int val);
extern int _bcm_kt2_port_control_oam_loopkup_with_dvp_get(int unit, 
                                                          bcm_port_t port, 
                                                          int *val);

#ifdef BCM_WARM_BOOT_SUPPORT /* BCM_WARM_BOOT_SUPPORT */
extern int _bcm_kt2_oam_sync(int unit);
#endif /* !BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP /* BCM_WARM_BOOT_SUPPORT_SW_DUMP*/
extern void _bcm_kt2_oam_sw_dump(int unit);
#endif /* !BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* BCM_KATANA2_SUPPORT */
#endif  /* !_BCM_INT_KATANA2_H_ */

