/*
 * $Id: vlan.h,v 1.1 2011/04/18 17:10:56 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        include/bcmx/vlan.h
 * Purpose:     BCMX Virtual LAN (VLAN) APIs
 */

#ifndef _BCMX_VLAN_H
#define _BCMX_VLAN_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/vlan.h>

typedef struct {
    bcm_vlan_t      vlan_tag;
    bcmx_lplist_t   port_bitmap;
    bcmx_lplist_t   ut_port_bitmap;
} bcmx_vlan_data_t;

typedef struct bcmx_vlan_block_s {
    bcmx_lplist_t  known_multicast;
    bcmx_lplist_t  unknown_multicast;
    bcmx_lplist_t  unknown_unicast;
    bcmx_lplist_t  broadcast;
} bcmx_vlan_block_t;

extern int bcmx_vlan_create(bcm_vlan_t vid);
extern int bcmx_vlan_destroy(bcm_vlan_t vid);
extern int bcmx_vlan_destroy_all(void);
extern int bcmx_vlan_port_add(bcm_vlan_t vid,
                              bcmx_lplist_t lplist, bcmx_lplist_t ubmp);
extern int bcmx_vlan_port_remove(bcm_vlan_t vid, bcmx_lplist_t lplist);
extern int bcmx_vlan_port_get(bcm_vlan_t vid,
                              bcmx_lplist_t *lplist, bcmx_lplist_t *ubmp);
extern int bcmx_vlan_default_get(bcm_vlan_t *vid_ptr);
extern int bcmx_vlan_default_set(bcm_vlan_t vid);
extern int bcmx_vlan_stg_get(bcm_vlan_t vid, bcm_stg_t *stg_ptr);
extern int bcmx_vlan_stg_set(bcm_vlan_t vid, bcm_stg_t stg);
extern int bcmx_vlan_stp_set(bcm_vlan_t vid, bcmx_lport_t port, int stp_state);
extern int bcmx_vlan_stp_get(bcm_vlan_t vid, bcmx_lport_t port,
                             int *stp_state);

extern int bcmx_vlan_port_default_action_set(bcmx_lport_t port,
                                             bcm_vlan_action_set_t *action);
extern int bcmx_vlan_port_default_action_get(bcmx_lport_t port,
                                             bcm_vlan_action_set_t *action);
extern int bcmx_vlan_port_default_action_delete(bcmx_lport_t port);

extern int bcmx_vlan_port_egress_default_action_set(bcmx_lport_t port,
                                                    bcm_vlan_action_set_t *action);
extern int bcmx_vlan_port_egress_default_action_get(bcmx_lport_t port,
                                                    bcm_vlan_action_set_t *action);
extern int bcmx_vlan_port_egress_default_action_delete(bcmx_lport_t port);

extern int bcmx_vlan_port_protocol_action_add(bcmx_lport_t port,
                                              bcm_port_frametype_t frame,
                                              bcm_port_ethertype_t ether,
                                              bcm_vlan_action_set_t *action);
extern int bcmx_vlan_port_protocol_action_get(bcmx_lport_t port, 
                                              bcm_port_frametype_t frame, 
                                              bcm_port_ethertype_t ether, 
                                              bcm_vlan_action_set_t *action);
extern int bcmx_vlan_port_protocol_action_delete(bcmx_lport_t port, 
                                                 bcm_port_frametype_t frame, 
                                                 bcm_port_ethertype_t ether);
extern int bcmx_vlan_port_protocol_action_delete_all(bcmx_lport_t port);

extern int bcmx_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio);
extern int bcmx_vlan_mac_delete(bcm_mac_t mac);
extern int bcmx_vlan_mac_delete_all(void);

extern int bcmx_vlan_mac_action_add(bcm_mac_t mac,
                                    bcm_vlan_action_set_t *action);
extern int bcmx_vlan_mac_action_delete(bcm_mac_t mac);
extern int bcmx_vlan_mac_action_delete_all(void);
extern int bcmx_vlan_mac_action_get(bcm_mac_t mac, 
                                    bcm_vlan_action_set_t *action);

extern int bcmx_vlan_translate_add(bcmx_lport_t port,
                                   bcm_vlan_t old_vid,
                                   bcm_vlan_t new_vid, int prio);
extern int bcmx_vlan_translate_delete(bcmx_lport_t port,
                                      bcm_vlan_t old_vid);
extern int bcmx_vlan_translate_delete_all(void);
extern int bcmx_vlan_translate_get(bcmx_lport_t port, 
                                   bcm_vlan_t old_vid,
                                   bcm_vlan_t *new_vid, int *prio);

extern int bcmx_vlan_translate_action_add(bcm_gport_t port, 
                                          bcm_vlan_translate_key_t key_type, 
                                          bcm_vlan_t outer_vlan, 
                                          bcm_vlan_t inner_vlan, 
                                          bcm_vlan_action_set_t *action);
extern int bcmx_vlan_translate_action_delete(bcm_gport_t port,
                                             bcm_vlan_translate_key_t key_type,
                                             bcm_vlan_t outer_vlan,
                                             bcm_vlan_t inner_vlan);
extern int bcmx_vlan_translate_action_get(bcm_gport_t port, 
                                          bcm_vlan_translate_key_t key_type, 
                                          bcm_vlan_t outer_vlan, 
                                          bcm_vlan_t inner_vlan, 
                                          bcm_vlan_action_set_t *action);

extern int bcmx_vlan_translate_egress_add(bcmx_lport_t port,
                                          bcm_vlan_t old_vid,
                                          bcm_vlan_t new_vid, int prio);
extern int bcmx_vlan_translate_egress_delete(bcmx_lport_t port,
                                             bcm_vlan_t old_vid);
extern int bcmx_vlan_translate_egress_delete_all(void);
extern int bcmx_vlan_translate_egress_get(bcmx_lport_t port,
                                          bcm_vlan_t old_vid, 
                                          bcm_vlan_t *new_vid, int *prio);

extern int bcmx_vlan_translate_egress_action_add(int port_class,
                                                 bcm_vlan_t outer_vlan,
                                                 bcm_vlan_t inner_vlan,
                                                 bcm_vlan_action_set_t *action);
extern int bcmx_vlan_translate_egress_action_delete(int port_class,
                                                    bcm_vlan_t outer_vlan,
                                                    bcm_vlan_t inner_vlan);
extern int bcmx_vlan_translate_egress_action_get(int port_class, 
                                                 bcm_vlan_t outer_vlan, 
                                                 bcm_vlan_t inner_vlan, 
                                                 bcm_vlan_action_set_t *action);

extern int bcmx_vlan_dtag_add(bcmx_lport_t port, bcm_vlan_t old_vid,
                              bcm_vlan_t new_vid, int prio);
extern int bcmx_vlan_dtag_delete(bcmx_lport_t port, bcm_vlan_t old_vid);
extern int bcmx_vlan_dtag_delete_all(void);
extern int bcmx_vlan_dtag_get(bcmx_lport_t port, bcm_vlan_t old_vid, 
                              bcm_vlan_t *new_vid, int *prio);

extern int bcmx_vlan_translate_range_add(bcmx_lport_t port,
                                         bcm_vlan_t old_vid_low,
                                         bcm_vlan_t old_vid_high,
                                         bcm_vlan_t new_vid, int int_prio);
extern int bcmx_vlan_translate_range_delete(bcmx_lport_t port,
                                            bcm_vlan_t old_vid_low,
                                            bcm_vlan_t old_vid_high);
extern int bcmx_vlan_translate_range_delete_all(void);
extern int bcmx_vlan_translate_range_get(bcmx_lport_t port, 
                                         bcm_vlan_t old_vid_low, 
                                         bcm_vlan_t old_vid_high, 
                                         bcm_vlan_t *new_vid, int *int_prio);

extern int bcmx_vlan_dtag_range_add(bcmx_lport_t port,
                                    bcm_vlan_t old_vid_low, 
                                    bcm_vlan_t old_vid_high,
                                    bcm_vlan_t new_vid, int int_prio);
extern int bcmx_vlan_dtag_range_delete(bcmx_lport_t port, 
                                       bcm_vlan_t old_vid_low,  
                                       bcm_vlan_t old_vid_high);
extern int bcmx_vlan_dtag_range_delete_all(void);
extern int bcmx_vlan_dtag_range_get(bcmx_lport_t port,
                                    bcm_vlan_t old_vid_low, 
                                    bcm_vlan_t old_vid_high, 
                                    bcm_vlan_t *new_vid, int *prio);

extern int bcmx_vlan_translate_action_range_add(bcm_gport_t port,
                                                bcm_vlan_t outer_vlan_low, 
                                                bcm_vlan_t outer_vlan_high, 
                                                bcm_vlan_t inner_vlan_low, 
                                                bcm_vlan_t inner_vlan_high, 
                                                bcm_vlan_action_set_t *action);
extern int bcmx_vlan_translate_action_range_delete(bcm_gport_t port,
                                                   bcm_vlan_t outer_vlan_low, 
                                                   bcm_vlan_t outer_vlan_high, 
                                                   bcm_vlan_t inner_vlan_low, 
                                                   bcm_vlan_t inner_vlan_high);
extern int bcmx_vlan_translate_action_range_delete_all(void);
extern int bcmx_vlan_translate_action_range_get(bcm_gport_t port, 
                                                bcm_vlan_t outer_vlan_low, 
                                                bcm_vlan_t outer_vlan_high, 
                                                bcm_vlan_t inner_vlan_low, 
                                                bcm_vlan_t inner_vlan_high, 
                                                bcm_vlan_action_set_t *action);

extern int bcmx_vlan_ip4_add(bcm_ip_t ipaddr, bcm_ip_t netmask,
                             bcm_vlan_t vid, int prio);
extern int bcmx_vlan_ip4_delete(bcm_ip_t ipaddr, bcm_ip_t netmask);
extern int bcmx_vlan_ip4_delete_all(void);

extern int bcmx_vlan_ip_add(bcm_vlan_ip_t *vlan_ip);
extern int bcmx_vlan_ip_delete(bcm_vlan_ip_t *vlan_ip);
extern int bcmx_vlan_ip_delete_all(void);
extern int bcmx_vlan_ip_action_add(bcm_vlan_ip_t *vlan_ip,
                                   bcm_vlan_action_set_t *action);
extern int bcmx_vlan_ip_action_get(bcm_vlan_ip_t *vlan_ip,
                                   bcm_vlan_action_set_t *action);
extern int bcmx_vlan_ip_action_delete(bcm_vlan_ip_t *vlan_ip);
extern int bcmx_vlan_ip_action_delete_all(void);

extern int bcmx_vlan_control_set(bcm_vlan_control_t type, int arg);
extern int bcmx_vlan_control_get(bcm_vlan_control_t type, int *arg);

extern int bcmx_vlan_control_port_set(bcmx_lport_t port,
                                      bcm_vlan_control_port_t type, int arg);
extern int bcmx_vlan_control_port_get(bcmx_lport_t port,
                                      bcm_vlan_control_port_t type, int *arg);

extern int bcmx_vlan_mcast_flood_set(bcm_vlan_t vlan,
                                     bcm_vlan_mcast_flood_t mode);
extern int bcmx_vlan_mcast_flood_get(bcm_vlan_t vlan,
                                     bcm_vlan_mcast_flood_t *mode);

extern int bcmx_vlan_control_vlan_set(bcm_vlan_t vlan, 
                                      bcm_vlan_control_vlan_t control);
extern int bcmx_vlan_control_vlan_get(bcm_vlan_t vlan,
                                      bcm_vlan_control_vlan_t *control);

extern int bcmx_vlan_cross_connect_add(bcm_vlan_t outer_vlan, 
                                       bcm_vlan_t inner_vlan, 
                                       bcm_gport_t port_1, 
                                       bcm_gport_t port_2);

extern int bcmx_vlan_cross_connect_delete(bcm_vlan_t outer_vlan, 
                                          bcm_vlan_t inner_vlan);

extern int bcmx_vlan_cross_connect_delete_all(void);

extern void bcmx_vlan_block_t_init(bcmx_vlan_block_t *data);

extern int bcmx_vlan_block_get(bcm_vlan_t vlan, bcmx_vlan_block_t *vlan_block);

extern int bcmx_vlan_block_set(bcm_vlan_t vlan, bcmx_vlan_block_t *vlan_block);

/*
 * VLAN statistics
 */
extern int bcmx_vlan_stat_enable_set(bcm_vlan_t vlan, int enable);
extern int bcmx_vlan_stat_get(bcm_vlan_t vlan, bcm_cos_t cos,
                              bcm_vlan_stat_t stat, uint64 *val);
extern int bcmx_vlan_stat_get32(bcm_vlan_t vlan, bcm_cos_t cos, 
                                bcm_vlan_stat_t stat, uint32 *val);
extern int bcmx_vlan_stat_set(bcm_vlan_t vlan, bcm_cos_t cos, 
                              bcm_vlan_stat_t stat, uint64 val);
extern int bcmx_vlan_stat_set32(bcm_vlan_t vlan, bcm_cos_t cos, 
                                bcm_vlan_stat_t stat, uint32 val);
extern int bcmx_vlan_stat_multi_get(bcm_vlan_t vlan, bcm_cos_t cos,
                                    int nstat, 
                                    bcm_vlan_stat_t *stat_arr,
                                    uint64 *value_arr);
extern int bcmx_vlan_stat_multi_get32(bcm_vlan_t vlan, bcm_cos_t cos,
                                      int nstat,
                                      bcm_vlan_stat_t *stat_arr, 
                                      uint32 *value_arr);
extern int bcmx_vlan_stat_multi_set(bcm_vlan_t vlan, bcm_cos_t cos, 
                                    int nstat, 
                                    bcm_vlan_stat_t *stat_arr, 
                                    uint64 *value_arr);
extern int bcmx_vlan_stat_multi_set32(bcm_vlan_t vlan, bcm_cos_t cos, 
                                      int nstat, 
                                      bcm_vlan_stat_t *stat_arr, 
                                      uint32 *value_arr);
/*
 * VLAN queue mapping
 */
extern int bcmx_vlan_queue_map_create(uint32 flags, int *qmid);
extern int bcmx_vlan_queue_map_destroy(int qmid);
extern int bcmx_vlan_queue_map_destroy_all(void);
extern int bcmx_vlan_queue_map_set(int qmid, int pkt_pri, int cfi, 
                                   int queue, int color);
extern int bcmx_vlan_queue_map_get(int qmid, int pkt_pri, int cfi, 
                                   int *queue, int *color);
extern int bcmx_vlan_queue_map_attach(bcm_vlan_t vlan, int qmid);
extern int bcmx_vlan_queue_map_attach_get(bcm_vlan_t vlan, int *qmid);
extern int bcmx_vlan_queue_map_detach(bcm_vlan_t vlan);
extern int bcmx_vlan_queue_map_detach_all(void);

/*
 * VLAN gport
 */
extern int bcmx_vlan_gport_add(bcm_vlan_t vlan, bcm_gport_t port, 
                               int is_untagged);
extern int bcmx_vlan_gport_delete(bcm_vlan_t vlan, bcm_gport_t port);
extern int bcmx_vlan_gport_delete_all(bcm_vlan_t vlan);
extern int bcmx_vlan_gport_get(bcm_vlan_t vlan, bcm_gport_t port, 
                               int *is_untagged);
extern int bcmx_vlan_gport_get_all(bcm_vlan_t vlan, 
                                   int array_max, bcm_gport_t *gport_array, 
                                   int *is_untagged, int *array_size);

#endif  /* _BCMX_VLAN_H */
