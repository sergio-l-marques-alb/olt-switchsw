/*
 * $Id$
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
 * File:        tomahawk.h
 * Purpose:     Function declarations for Tomahawk Internal functions.
 */

#ifndef _BCM_INT_TOMAHAWK_H_
#define _BCM_INT_TOMAHAWK_H_

#include <soc/defs.h>

#if defined(BCM_TOMAHAWK_SUPPORT)

#include <bcm/types.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/cosq.h>

#define RH_OPT_MAX_PATHS 16384
#define RH_OPT_MIN_PATHS 64
#define BCM_ECMP_LB_MODE_RH 0x01
#define BCM_ECMP_LB_MODE_RANDOM 0x2
#define BCM_ECMP_LB_MODE_RR 0x3

#ifdef BCM_WARM_BOOT_SUPPORT
/* ECMP member table size */
#define RH_ECMP_MEMBER_TBL_MAX    16384
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_th_stage_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_th_init(int unit, _field_control_t *fc);
extern int _bcm_field_th_multi_pipe_counter_mem_get(int unit,
                                         _field_stage_t *stage_fc,
                                         soc_mem_t *counter_mem);
extern int _bcm_field_th_counter_multi_pipe_collect_init(int unit,
                                                      _field_control_t *fc,
                                                      _field_stage_t *stage_fc);
extern int _bcm_field_th_action_params_check(int unit, 
                                             _field_entry_t  *f_ent,
                                             _field_action_t *fa);
extern int _bcm_field_th_action_support_check(int                unit,
                                              _field_entry_t     *f_ent,
                                              bcm_field_action_t action,
                                              int                *result);
extern int _bcm_field_th_selcodes_install(int unit,
                                          _field_group_t *fg,
                                          uint8 slice_numb,
                                          bcm_pbmp_t pbmp,
                                          int selcode_index);
extern int _bcm_field_th_scache_sync(int unit,
                                     _field_control_t *fc,
                                     _field_stage_t   *stage_fc);

extern int _bcm_field_th_policer_meter_hw_free(int unit, 
                                               _field_entry_t *f_ent,
                                              _field_entry_policer_t *f_ent_pl);
#endif

#if defined(INCLUDE_L3)

extern int bcm_th_ipmc_repl_init(int unit);
extern int bcm_th_ipmc_repl_detach(int unit);
extern int bcm_th_ipmc_repl_set(int unit, int ipmc_id, bcm_port_t port,
                                bcm_vlan_vector_t vlan_vec);
extern int bcm_th_ipmc_repl_get(int unit, int index, bcm_port_t port,
                                bcm_vlan_vector_t vlan_vec);
extern int bcm_th_ipmc_repl_add(int unit, int index, bcm_port_t port,
                                bcm_vlan_t vlan);
extern int bcm_th_ipmc_repl_delete(int unit, int index, bcm_port_t port,
                                bcm_vlan_t vlan);
extern int bcm_th_ipmc_repl_delete_all(int unit, int index,
                                bcm_port_t port);
extern int bcm_th_ipmc_egress_intf_add(int unit, int index, bcm_port_t port,
                                       bcm_l3_intf_t *l3_intf);
extern int bcm_th_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port,
                                       bcm_l3_intf_t *l3_intf);
extern int bcm_th_ipmc_egress_intf_set(int unit, int mc_index,
                                       bcm_port_t port, int if_count,
                                       bcm_if_t *if_array, int is_l3,
                                       int check_port);
extern int bcm_th_ipmc_egress_intf_get(int unit, int mc_index,
                                       bcm_port_t port,
                                       int if_max, bcm_if_t *if_array,
                                       int *if_count);
extern int _bcm_th_ipmc_egress_intf_add(int unit, int index, bcm_port_t port,
                                       int encap_id, int is_l3);
extern int _bcm_th_ipmc_egress_intf_delete(int unit, int index,
                                       bcm_port_t port, int if_max,
                                       int encap_id, int is_l3);
extern int bcm_th_ipmc_trill_mac_update(int unit, uint32 mac_field,
                                        uint8 flag);
extern int bcm_th_ipmc_l3_intf_next_hop_free(int unit, int intf);
extern int _bcm_th_ipmc_src_port_compare(int unit, bcm_ipmc_addr_t *ipmc,
                                         int *match);


#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_th_ipmc_repl_reload(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_th_ipmc_repl_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/* Resilient Hashing funcitons */
extern int bcm_th_ecmp_rh_init(int unit);
extern int bcm_th_ecmp_rh_deinit(int unit);
extern int bcm_th_l3_egress_ecmp_rh_create(int unit,
                                           bcm_l3_egress_ecmp_t *ecmp,
                                           int intf_count,
                                           bcm_if_t *intf_array,
                                           int op, int count,
                                           bcm_if_t *intf,
                                           bcm_if_t *new_intf_array);
extern int bcm_th_l3_egress_ecmp_rh_destroy(int unit, bcm_if_t mpintf);
extern int bcm_th_l3_egress_ecmp_lb_get(int unit, bcm_l3_egress_ecmp_t *ecmp);
extern int bcm_th_l3_egress_ecmp_rh_shared_copy(int unit, int old_ecmp_group,
                                                int new_ecmp_group);
extern int bcm_th_ecmp_rh_set_intf_arr(int unit, int intf_count,
                                       bcm_if_t *intf_array,
                                       int ecmp_group_idx,
                                       int group_size);
extern int bcm_th_ecmp_rh_get_multipath(int unit, int ecmp_group_idx,
                                        bcm_if_t *intf_array, int *intf_count);
extern int bcm_th_l3_egress_rh_ecmp_find(int unit, int intf_count,
                                         bcm_if_t *intf_array, bcm_if_t *mpintf);
extern int bcm_th_ecmp_lb_mode_set(int unit,
                                   int ecmp_group_idx,
                                   uint8 lb_mode);
extern int bcm_th_ecmp_lb_mode_reset(int unit,
                                     bcm_if_t mpintf);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_th_l3_ecmp_rh_member_sync(int unit, uint8 **scache_ptr);
extern int bcm_th_l3_ecmp_rh_member_recover(int unit, uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_th_ecmp_rh_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* INCLUDE_L3 */

/* Cosq */
extern int _bcm_th_cosq_port_resolve(int unit, bcm_gport_t gport, 
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id, int *qnum);
extern int bcm_th_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                                 int *numq, uint32 *flags);
extern int bcm_th_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 min_quantum, uint32 max_quantum,
                                          uint32 burst_quantum, uint32 flags);
extern int bcm_th_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *min_quantum,
                                          uint32 *max_quantum,
                                          uint32 *burst_quantum,
                                          uint32 *flags);

extern int bcm_th_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 kbits_burst_min, 
                                                 uint32 kbits_burst_max);
extern int bcm_th_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 *kbits_burst_min,
                                                 uint32 *kbits_burst_max);

extern int bcm_th_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int *mode,
                                       int *weight);
extern int bcm_th_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);

extern int bcm_th_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                      void *user_data);

extern int bcm_th_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max,
                                           uint32 flags);
extern int bcm_th_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                           uint32 *flags);
extern int bcm_th_cosq_port_burst_set(int unit, bcm_port_t port, 
                                      bcm_cos_queue_t cosq, int burst);
extern int bcm_th_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq,
                                      int *burst);

extern int bcm_th_port_rate_egress_set(int unit, bcm_port_t port,
                                       uint32 bandwidth, uint32 burst,
                                       uint32 mode);
extern int bcm_th_port_rate_egress_get(int unit, bcm_port_t port,
                                       uint32 *bandwidth, uint32 *burst,
                                       uint32 *mode);

extern int bcm_th_cosq_discard_set(int unit, uint32 flags);
extern int bcm_th_cosq_discard_get(int unit, uint32 *flags);

extern int bcm_th_cosq_discard_port_get(int unit, bcm_port_t port, 
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, 
                                        int *drop_slope, int *average_time);
extern int bcm_th_cosq_discard_port_set(int unit, bcm_port_t port, 
                                        bcm_cos_queue_t cosq,
                                        uint32 color, int drop_start,
                                        int drop_slope,
                                        int average_time);

extern int bcm_th_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_th_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);

extern int bcm_th_cosq_port_pfc_op(int unit, bcm_port_t port,
                                   bcm_switch_control_t sctype,
                                   _bcm_cosq_op_t op,
                                   bcm_gport_t *gport,
                                   int gport_count);
extern int bcm_th_cosq_port_pfc_get(int unit, bcm_port_t port,
                                    bcm_switch_control_t sctype,
                                    bcm_gport_t *gport,
                                    int gport_count,
                                    int *actual_gport_count);

extern int bcm_th_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int *weights, int delay);
extern int bcm_th_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int *weights, int *delay);

extern int bcm_th_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                 uint32 flags, bcm_gport_t *gport);
extern int bcm_th_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_th_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                                    bcm_gport_t parent_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_th_cosq_gport_detach(int unit, bcm_gport_t input_gport,
                                    bcm_gport_t parent_gport,
                                    bcm_cos_queue_t cosq);

extern int bcm_th_cosq_mapping_set(int unit, bcm_port_t gport, 
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_th_cosq_mapping_get(int unit, bcm_port_t gport,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);

extern int bcm_th_cosq_control_set(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_control_t type, int arg);
extern int bcm_th_cosq_control_get(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_control_t type, int *arg);

extern int bcm_th_cosq_stat_set(int unit, bcm_gport_t port,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_stat_t stat, uint64 value);
extern int bcm_th_cosq_stat_get(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_stat_t stat, int sync_mode,
                                uint64 *value);

extern int bcm_th_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                                         bcm_cos_t int_pri, uint32 flags,
                                         bcm_gport_t gport,
                                         bcm_cos_queue_t cosq);
extern int bcm_th_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                                         bcm_cos_t int_pri, uint32 flags,
                                         bcm_gport_t *gport,
                                         bcm_cos_queue_t *cosq);

extern int bcm_th_cosq_config_set(int unit, int numq);
extern int bcm_th_cosq_config_get(int unit, int *numq);

extern int bcm_th_cosq_init(int unit);

extern int bcm_th_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid);

extern void bcm_th_cosq_sw_dump(int unit);

typedef enum {
    _BCM_TH_COSQ_INDEX_STYLE_WRED_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_TH_COSQ_INDEX_STYLE_WRED_DEVICE,
    _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_TH_COSQ_INDEX_STYLE_COUNT
} _bcm_th_cosq_index_style_t;

#ifdef  ALPM_ENABLE
extern int _bcm_th_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
extern int _bcm_th_alpm_128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
#endif

#endif /* BCM_TOMAHAWK_SUPPORT  */

#endif /* !_BCM_INT_TOMAHAWK_H_ */
