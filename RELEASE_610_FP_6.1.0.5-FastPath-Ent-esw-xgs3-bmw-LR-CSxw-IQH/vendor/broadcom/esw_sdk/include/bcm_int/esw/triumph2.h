/*
 * $Id: triumph2.h,v 1.1 2011/04/18 17:10:56 mruas Exp $
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
 * File:        triumph2.h
 * Purpose:     Function declarations for Triumph 2 bcm functions
 */

#ifndef _BCM_INT_TRIUMPH2_H_
#define _BCM_INT_TRIUMPH2_H_
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <soc/tnl_term.h>
#include <bcm/qos.h>

/****************************************************************
 *
 * Chariot functions
 *
 ****************************************************************/
#if defined(INCLUDE_L3)
extern int _bcm_tr2_l3_tnl_term_add(int unit, uint32 *entry_ptr, 
                                    bcm_tunnel_terminator_t *tnl_info);
extern int _bcm_tr2_l3_tnl_term_entry_init(int unit, 
                                           bcm_tunnel_terminator_t *tnl_info,
                                           soc_tunnel_term_t *entry);
extern int _bcm_tr2_l3_tnl_term_entry_parse(int unit, soc_tunnel_term_t *entry,
                                  bcm_tunnel_terminator_t *tnl_info);
extern int _bcm_tr2_multicast_ipmc_write(int unit, int ipmc_id, bcm_pbmp_t l2_pbmp,
                              bcm_pbmp_t l3_pbmp, int valid);
extern int _bcm_tr2_multicast_ipmc_read(int unit, int ipmc_id, bcm_pbmp_t *l2_pbmp,
                             bcm_pbmp_t *l3_pbmp);
extern int bcm_tr2_ipmc_repl_init(int unit);
extern int bcm_tr2_ipmc_repl_detach(int unit);
extern int bcm_tr2_ipmc_repl_get(int unit, int index, bcm_port_t port,
                         bcm_vlan_vector_t vlan_vec);
extern int bcm_tr2_ipmc_repl_add(int unit, int index, bcm_port_t port,
                         bcm_vlan_t vlan);
extern int bcm_tr2_ipmc_repl_delete(int unit, int index, bcm_port_t port,
                            bcm_vlan_t vlan);
extern int bcm_tr2_ipmc_repl_delete_all(int unit, int index,
                                bcm_port_t port);
extern int bcm_tr2_ipmc_egress_intf_add(int unit, int index, bcm_port_t port,
                                bcm_l3_intf_t *l3_intf);
extern int bcm_tr2_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port,
                                   bcm_l3_intf_t *l3_intf);
extern int bcm_tr2_ipmc_egress_intf_set(int unit, int mc_index,
                                       bcm_port_t port, int if_count,
                                       bcm_if_t *if_array);
extern int bcm_tr2_ipmc_egress_intf_get(int unit, int mc_index,
                                       bcm_port_t port,
                                       int if_max, bcm_if_t *if_array,
                                       int *if_count);
extern int _bcm_tr2_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                                        int nh_index);
extern int _bcm_tr2_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                           int if_max, int nh_index);
extern int bcm_tr2_multicast_create(int unit, uint32 flags,
                                   bcm_multicast_t *group);
extern int bcm_tr2_multicast_destroy(int unit, bcm_multicast_t group);
extern int bcm_tr2_multicast_vpls_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                           bcm_gport_t mpls_port_id, bcm_if_t *encap_id);
extern int bcm_tr2_multicast_subport_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                              bcm_gport_t subport, bcm_if_t *encap_id);
extern int bcm_tr2_multicast_mim_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                          bcm_gport_t mim_port_id, bcm_if_t *encap_id);
extern int bcm_tr2_multicast_wlan_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                           bcm_gport_t wlan_port_id, bcm_if_t *encap_id);
extern int bcm_tr2_multicast_egress_add(int unit, bcm_multicast_t group,
                                       bcm_gport_t port, bcm_if_t encap_id);
extern int bcm_tr2_multicast_egress_delete(int unit, bcm_multicast_t group,
                                          bcm_gport_t port, bcm_if_t encap_id);
extern int bcm_tr2_multicast_egress_delete_all(int unit, bcm_multicast_t group);
extern int bcm_tr2_multicast_egress_set(int unit, bcm_multicast_t group,
                                       int port_count, bcm_gport_t *port_array,
                                       bcm_if_t *encap_array);
extern int bcm_tr2_multicast_egress_get(int unit, bcm_multicast_t group,
                                       int port_max, bcm_gport_t *port_array,
                                       bcm_if_t *encap_array, int *port_count);
extern int bcm_tr2_mim_init(int unit);
extern int bcm_tr2_mim_detach(int unit);
extern int bcm_tr2_mim_vpn_create(int unit, bcm_mim_vpn_config_t *info);
extern int bcm_tr2_mim_vpn_destroy(int unit, bcm_mim_vpn_t vpn);
extern int bcm_tr2_mim_vpn_destroy_all(int unit);
extern int bcm_tr2_mim_vpn_get(int unit, bcm_mim_vpn_t vpn, 
                              bcm_mim_vpn_config_t *info);
extern int bcm_tr2_mim_vpn_traverse(int unit, bcm_mim_vpn_traverse_cb cb, 
                                    void *user_data);
extern int bcm_tr2_mim_port_add(int unit, bcm_mim_vpn_t vpn, bcm_mim_port_t *mim_port);
extern int bcm_tr2_mim_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mim_port_id);
extern int bcm_tr2_mim_port_delete_all(int unit, bcm_vpn_t vpn);
extern int bcm_tr2_mim_port_get(int unit, bcm_vpn_t vpn, bcm_mim_port_t *mim_port);
extern int bcm_tr2_mim_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                        bcm_mim_port_t *port_array, int *port_count);
extern int _bcm_tr2_mim_port_resolve(int unit, bcm_gport_t mim_port,
                                    bcm_module_t *modid, bcm_port_t *port,
                                    bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr2_mim_port_learn_set(int unit, bcm_gport_t port, uint32 flags);
extern int bcm_tr2_mim_port_learn_get(int unit, bcm_gport_t port, uint32 *flags);
extern int _bcm_tr2_mim_svp_field_set(int unit, bcm_gport_t vp, 
                                      soc_field_t field, int value);
extern int bcm_tr2_wlan_init(int unit);
extern int bcm_tr2_wlan_detach(int unit);
extern int bcm_tr2_wlan_client_add(int unit, bcm_wlan_client_t *info);
extern int bcm_tr2_wlan_client_delete(int unit, bcm_mac_t mac);
extern int bcm_tr2_wlan_client_delete_all(int unit);
extern int bcm_tr2_wlan_client_traverse(int unit,
                                        bcm_wlan_client_traverse_cb cb,
                                        void *user_data);
extern int bcm_tr2_wlan_client_get(int unit, bcm_mac_t mac, 
                                   bcm_wlan_client_t *info);
extern int _bcm_tr2_wlan_port_resolve(int unit, bcm_gport_t wlan_port,
                                    bcm_module_t *modid, bcm_port_t *port,
                                    bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr2_wlan_port_learn_set(int unit, bcm_gport_t port, uint32 flags);
extern int bcm_tr2_wlan_port_learn_get(int unit, bcm_gport_t port, uint32 *flags);
extern int bcm_tr2_wlan_port_add(int unit, bcm_wlan_port_t *wlan_port);
extern int bcm_tr2_wlan_port_delete(int unit, bcm_gport_t wlan_port_id);
extern int bcm_tr2_wlan_port_delete_all(int unit);
extern int bcm_tr2_wlan_port_get(int unit, bcm_gport_t wlan_port_id, 
                                 bcm_wlan_port_t *wlan_port);
extern int bcm_tr2_wlan_port_traverse(int unit,
                                      bcm_wlan_port_traverse_cb cb,
                                      void *user_data);
extern int bcm_tr2_wlan_tunnel_initiator_create(int unit, 
                                                bcm_tunnel_initiator_t *info);
extern int bcm_tr2_wlan_tunnel_initiator_destroy(int unit, 
                                                 bcm_gport_t wlan_tunnel_id);
extern int bcm_tr2_wlan_tunnel_initiator_get(int unit, 
                                             bcm_tunnel_initiator_t *info);
extern int bcm_tr2_tunnel_terminator_vlan_set(int unit, bcm_gport_t tunnel, 
                                              bcm_vlan_vector_t vlan_vec);
extern int bcm_tr2_tunnel_terminator_vlan_get(int unit, bcm_gport_t tunnel, 
                                              bcm_vlan_vector_t *vlan_vec);
extern int _bcm_tr2_lport_field_set(int unit, bcm_gport_t wlan_port_id, 
                                    soc_field_t field, int value);
extern int _bcm_tr2_lport_field_get(int unit, bcm_gport_t wlan_port_id, 
                                    soc_field_t field, int *value);
extern int _bcm_tr2_wlan_port_set(int unit, bcm_gport_t wlan_port_id, 
                                  soc_field_t field, uint32 value);
extern int bcm_tr2_wlan_port_untagged_vlan_get(int unit, bcm_gport_t port, 
                                               bcm_vlan_t *vid_ptr);
extern int bcm_tr2_wlan_port_untagged_vlan_set(int unit, bcm_gport_t port, 
                                               bcm_vlan_t vid);
extern int bcm_tr2_wlan_port_untagged_prio_get(int unit, bcm_gport_t port, 
                                               int *prio_ptr);
extern int bcm_tr2_wlan_port_untagged_prio_set(int unit, bcm_gport_t port, 
                                               int prio);
extern int _bcm_tr2_svp_field_set(int unit, bcm_gport_t port_id, 
                                  soc_field_t field, int value);
extern int _bcm_tr2_svp_field_get(int unit, bcm_gport_t port_id, 
                                  soc_field_t field, int *value);
extern int _bcm_tr2_mpls_svp_field_set(int unit, bcm_gport_t vp, 
                                       soc_field_t field, int value);
#endif /* INCLUDE_L3 */

extern int bcm_tr2_cosq_init(int unit);
extern int bcm_tr2_cosq_detach(int unit, int software_state_only);
extern int bcm_tr2_cosq_config_set(int unit, int numq);
extern int bcm_tr2_cosq_config_get(int unit, int *numq);
extern int bcm_tr2_cosq_mapping_set(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_tr2_cosq_mapping_get(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int bcm_tr2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 min_quantum,
                                          uint32 max_quantum,
                                          uint32 burst_quantum,
                                          uint32 flags);
extern int bcm_tr2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *min_quantum,
                                          uint32 *max_quantum,
                                          uint32 *burst_quantum,
                                          uint32 *flags);
extern int bcm_tr2_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_tr2_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);
extern int bcm_tr2_cosq_port_burst_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int burst);
extern int bcm_tr2_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *burst);
extern int bcm_tr2_cosq_discard_set(int unit, uint32 flags);
extern int bcm_tr2_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_tr2_cosq_discard_port_set(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int drop_start, int drop_slope,
                                        int average_time);
extern int bcm_tr2_cosq_discard_port_get(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, int *drop_slope,
                                        int *average_time);
extern int bcm_tr2_cosq_gport_discard_set(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_tr2_cosq_gport_discard_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_tr2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int weights[], int delay);
extern int bcm_tr2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int weights[], int *delay);
extern int bcm_tr2_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                 uint32 flags, bcm_gport_t *gport);
extern int bcm_tr2_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_tr2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                      void *user_data);

extern int bcm_tr2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq, 
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max,
                                            uint32 flags);
extern int bcm_tr2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq, 
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                            uint32 *flags);
extern int bcm_tr2_cosq_gport_bandwidth_burst_set(int unit,
                                                  bcm_gport_t gport,
                                                  bcm_cos_queue_t cosq, 
                                                  uint32 kbits_burst);
extern int bcm_tr2_cosq_gport_bandwidth_burst_get(int unit,
                                                  bcm_gport_t gport,
                                                  bcm_cos_queue_t cosq, 
                                                  uint32 *kbits_burst);
extern int bcm_tr2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_tr2_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                                       bcm_cos_queue_t cosq, int *mode,
                                       int *weight);
extern int bcm_tr2_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                                    bcm_cos_queue_t cosq, bcm_gport_t input_gport);
extern int bcm_tr2_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                                    bcm_cos_queue_t cosq, bcm_gport_t input_gport);
extern int bcm_tr2_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                                        bcm_cos_queue_t *cosq, bcm_gport_t *input_gport);
extern int _bcm_tr2_cosq_port_resolve(int unit, bcm_gport_t gport,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id);
extern int _bcm_tr2_port_vlan_priority_map_set(int unit, bcm_port_t port, 
                                               int pkt_pri, int cfi,
                                               int internal_pri, 
                                               bcm_color_t color);
extern int _bcm_tr2_port_vlan_priority_map_get(int unit, bcm_port_t port, 
                                               int pkt_pri, int cfi,
                                               int *internal_pri, 
                                               bcm_color_t *color);

extern void bcm_tr2_failover_unlock(int unit);
extern int  bcm_tr2_failover_lock(int unit);
extern int  bcm_tr2_failover_init(int unit);
extern int  bcm_tr2_failover_cleanup(int unit);
extern int  bcm_tr2_failover_create(int unit, uint32 flags, bcm_failover_t *failover_id);
extern int  bcm_tr2_failover_destroy(int unit, bcm_failover_t  failover_id);
extern int bcm_tr2_failover_set(int unit, bcm_failover_t failover_id, int value);
extern int bcm_tr2_failover_get(int unit, bcm_failover_t failover_id, int  *value);
extern int bcm_tr2_failover_prot_nhi_create (int unit, int nh_index);
extern int bcm_tr2_failover_prot_nhi_set(int unit, int nh_index, uint32 prot_nh_index, bcm_failover_t failover_id);
extern int bcm_tr2_failover_prot_nhi_get(int unit, int nh_index, bcm_failover_t  *failover_id, int *prot_nh_index);
extern int bcm_tr2_failover_prot_nhi_cleanup  (int unit, int nh_index);

extern int bcm_tr2x_oam_init(int unit);
extern int bcm_tr2x_oam_detach(int unit);
extern int bcm_tr2x_oam_lock(int unit);
extern int bcm_tr2x_oam_unlock(int unit);
extern int bcm_tr2x_oam_group_create(int unit,
    bcm_oam_group_info_t *group_info);
extern int bcm_tr2x_oam_group_get(int unit, bcm_oam_group_t group, 
    bcm_oam_group_info_t *group_info);
extern int bcm_tr2x_oam_group_destroy(int unit, bcm_oam_group_t group);
extern int bcm_tr2x_oam_group_destroy_all(int unit);
extern int bcm_tr2x_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb, 
    void *user_data);
extern int bcm_tr2x_oam_endpoint_create(int unit, 
    bcm_oam_endpoint_info_t *endpoint_info);
extern int bcm_tr2x_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint, 
    bcm_oam_endpoint_info_t *endpoint_info);
extern int bcm_tr2x_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint);
extern int bcm_tr2x_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group);
extern int bcm_tr2x_oam_endpoint_traverse(int unit, bcm_oam_group_t group, 
    bcm_oam_endpoint_traverse_cb cb, void *user_data);
extern int bcm_tr2x_oam_event_register(int unit, 
    bcm_oam_event_types_t event_types, bcm_oam_event_cb cb, 
    void *user_data);
extern int bcm_tr2x_oam_event_unregister(int unit, 
    bcm_oam_event_types_t event_types, bcm_oam_event_cb cb);

extern int bcm_tr2_subport_init(int unit);
extern int bcm_tr2_subport_cleanup(int unit);
extern int bcm_tr2_subport_group_create(int unit, bcm_subport_group_config_t *config,
                            bcm_gport_t *group);
extern int bcm_tr2_subport_group_destroy(int unit, bcm_gport_t group);
extern int bcm_tr2_subport_group_get(int unit, bcm_gport_t group,
                            bcm_subport_group_config_t *config);
extern int bcm_tr2_subport_port_add(int unit, bcm_subport_config_t *config,
                            bcm_gport_t *port);
extern int bcm_tr2_subport_port_delete(int unit, bcm_gport_t port);
extern int bcm_tr2_subport_port_get(int unit, bcm_gport_t port,
                            bcm_subport_config_t *config);
extern int bcm_tr2_subport_port_traverse(int unit, bcm_subport_port_traverse_cb cb,
                             void *user_data);
extern int _bcm_tr2_subport_group_resolve(int unit, bcm_gport_t gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *id);
extern int _bcm_tr2_subport_port_resolve(int unit, bcm_gport_t gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr2_subport_learn_set(int unit, bcm_gport_t port, uint32 flags);
extern int bcm_tr2_subport_learn_get(int unit, bcm_gport_t port, uint32 *flags);
extern int _bcm_tr2_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_tr2_port_lanes_get(int unit, bcm_port_t port, int *value);
extern int _bcm_port_probe(int unit, bcm_port_t p, int *okay);
extern int _bcm_tr2_ing_pri_cng_map_default_entry_add(int unit, 
                                                      soc_profile_mem_t *prof);
extern int _bcm_tr2_port_vpd_bitmap_alloc(int unit);
extern void _bcm_tr2_port_vpd_bitmap_free(int unit);
extern int _bcm_port_vlan_prot_index_alloc(int unit, int *prof_ptr);
/* Flexible counters support */

typedef enum _bcm_flex_stat_type_s {
    _bcmFlexStatTypeService,
    _bcmFlexStatTypeGport,
    _bcmFlexStatTypeFp,
    _bcmFlexStatTypeVrf,
    _bcmFlexStatTypeNum,
    _bcmFlexStatTypeHwNum = _bcmFlexStatTypeFp
} _bcm_flex_stat_type_t;

typedef struct _bcm_flex_stat_handle_to_index_s {
    uint32 handle;
    uint32 index;
    _bcm_flex_stat_type_t type;
} _bcm_flex_stat_handle_to_index_t;

/* Structure for tracking flexible stat info */
typedef struct _bcm_flex_stat_info_s {
    int stat_mem_size;
    SHR_BITDCL  *stats;
    int handle_list_size;
    _bcm_flex_stat_handle_to_index_t *handle_list;
} _bcm_flex_stat_info_t;

typedef enum _bcm_flex_stat_e {
    _bcmFlexStatIngressPackets,  /* Packets that ingress on the class */
    _bcmFlexStatIngressBytes,    /* Bytes that ingress on the class */
    _bcmFlexStatEgressPackets,   /* Packets that egress on the class */
    _bcmFlexStatEgressBytes,     /* Bytes that egress on the class */
    _bcmFlexStatNum
} _bcm_flex_stat_t;

typedef int (*_bcm_flex_stat_src_index_set_f)(int unit, uint32 handle,
                                              int fs_idx, void *cookie);

extern int _bcm_esw_flex_stat_detach(int unit, _bcm_flex_stat_type_t type);
extern int _bcm_esw_flex_stat_init(int unit, _bcm_flex_stat_type_t type);

extern void _bcm_esw_flex_stat_reinit_add(int unit,
                                          _bcm_flex_stat_type_t type,
                                          int fs_idx, uint32 handle);
extern void _bcm_esw_flex_stat_release_handles(int unit,
                                               _bcm_flex_stat_type_t type);
extern int _bcm_esw_flex_stat_stat_set(int unit,
                                       _bcm_flex_stat_type_t type,
                                       int fs_idx,
                                       bcm_vlan_stat_t vl_stat,
                                       uint64 val);
extern int _bcm_esw_flex_stat_stat_get(int unit,
                                       _bcm_flex_stat_type_t type,
                                       int fs_idx,
                                       bcm_vlan_stat_t vl_stat,
                                       uint64 *val);
extern void _bcm_esw_flex_stat_handle_free(int unit,
                                           _bcm_flex_stat_type_t type,
                                           uint32 handle);
extern int _bcm_esw_vlan_flex_stat_hw_index_set(int unit, uint32 handle,
                                                int fs_idx, void *cookie);
extern int _bcm_esw_port_flex_stat_hw_index_set(int unit, uint32 handle,
                                                int fs_idx, void *cookie);

extern int _bcm_esw_flex_stat_enable_set(int unit,
                                         _bcm_flex_stat_type_t type,
                         _bcm_flex_stat_src_index_set_f fs_set_cb,
                                         void *cookie,
                                         uint32 handle, 
                                         int enable);
extern int _bcm_esw_flex_stat_get(int unit, 
                                  _bcm_flex_stat_type_t type,
                                  uint32 handle, 
                                  _bcm_flex_stat_t stat, 
                                  uint64 *val);
extern int _bcm_esw_flex_stat_get32(int unit, 
                                    _bcm_flex_stat_type_t type,
                                    uint32 handle, 
                                    _bcm_flex_stat_t stat, 
                                    uint32 *val);
extern int _bcm_esw_flex_stat_set(int unit, 
                                  _bcm_flex_stat_type_t type,
                                  uint32 handle, 
                                  _bcm_flex_stat_t stat, 
                                  uint64 val);
extern int _bcm_esw_flex_stat_set32(int unit, 
                                    _bcm_flex_stat_type_t type,
                                    uint32 handle, 
                                    _bcm_flex_stat_t stat, 
                                    uint32 val);
extern int _bcm_esw_flex_stat_multi_get(int unit,
                                        _bcm_flex_stat_type_t type,
                                        uint32 handle,
                                        int nstat,
                                        _bcm_flex_stat_t *stat_arr,
                                        uint64 *value_arr);
extern int _bcm_esw_flex_stat_multi_get32(int unit, 
                                          _bcm_flex_stat_type_t type,
                                          uint32 handle, 
                                          int nstat, 
                                          _bcm_flex_stat_t *stat_arr, 
                                          uint32 *value_arr);
extern int _bcm_esw_flex_stat_multi_set(int unit, 
                                        _bcm_flex_stat_type_t type,
                                        uint32 handle, 
                                        int nstat, 
                                        _bcm_flex_stat_t *stat_arr, 
                                        uint64 *value_arr);
extern int _bcm_esw_flex_stat_multi_set32(int unit, 
                                          _bcm_flex_stat_type_t type,
                                          uint32 handle, 
                                          int nstat, 
                                          _bcm_flex_stat_t *stat_arr, 
                                          uint32 *value_arr);

extern int _bcm_esw_port_flex_stat_index_set(int unit, bcm_gport_t port,
                                             int fs_idx);
extern int _bcm_esw_mpls_flex_stat_index_set(int unit, bcm_gport_t port,
                                             int fs_idx);
extern int _bcm_esw_mim_flex_stat_index_set(int unit, bcm_gport_t port,
                                            int fs_idx);
extern int _bcm_esw_subport_flex_stat_index_set(int unit, bcm_gport_t port,
                                                int fs_idx);
extern int _bcm_esw_wlan_flex_stat_index_set(int unit, bcm_gport_t port,
                                             int fs_idx);
extern int _bcm_esw_vlan_flex_stat_index_set(int unit, bcm_vlan_t vlan,
                                             int fs_idx);
extern int _bcm_tr_subport_gport_used(int unit, bcm_gport_t port);
#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_tr2_init(int unit, _field_control_t *fc);
extern int _bcm_field_tr2_data_qualifier_ethertype_delete(int unit, int qual_id,
                                 bcm_field_data_ethertype_t *etype);
extern int _bcm_field_tr2_data_qualifier_ethertype_add(int unit, int qual_id,
                                 bcm_field_data_ethertype_t *etype);
extern int _bcm_field_tr2_data_qualifier_ip_protocol_add(int unit,  int qual_id,
                          bcm_field_data_ip_protocol_t *ip_protocol);
extern int _bcm_field_tr2_data_qualifier_ip_protocol_delete(int unit,  int qual_id,
                          bcm_field_data_ip_protocol_t *ip_protocol);
extern int _bcm_field_tr2_data_qualifier_packet_format_add(int unit,int qual_id,
                                bcm_field_data_packet_format_t *packet_format);
extern int _bcm_field_tr2_data_qualifier_packet_format_delete(int unit,int qual_id,
                                bcm_field_data_packet_format_t *packet_format);
extern int _bcm_field_tr2_udf_delete(int unit, uint32 udf_num, 
                                     uint32 user_num);
extern int _bcm_field_tr2_l2_actions_hw_free(int unit, _field_entry_t *f_ent,
                                             uint32 flags);
extern int _bcm_field_tr2_l2_actions_hw_alloc(int unit, _field_entry_t *f_ent);
#endif /* BCM_FIELD_SUPPORT */
extern int _bcm_tr2_vlan_qmid_set(int unit, bcm_vlan_t vlan, int qm_ptr,
                                  int use_inner_tag);
extern int _bcm_tr2_vlan_qmid_get(int unit, bcm_vlan_t vlan, int *qm_ptr,
                                  int *use_inner_tag);
extern int bcm_tr2_qos_init(int unit);
extern int bcm_tr2_qos_detach(int unit);
extern int bcm_tr2_qos_map_create(int unit, uint32 flags, int *map_id);
extern int bcm_tr2_qos_map_destroy(int unit, int map_id);
extern int bcm_tr2_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, 
                               int map_id);
extern int bcm_tr2_qos_map_delete(int unit, uint32 flags, 
                                  bcm_qos_map_t *map, int map_id);
extern int bcm_tr2_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, 
                                    int egr_map);
extern int _bcm_tr2_port_tab_set(int unit, bcm_port_t port, 
                                 soc_field_t field, int value);
#endif /* BCM_TRIUMPH2_SUPPORT */
#endif  /* !_BCM_INT_TRIUMPH2_H_ */
