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
#include <bcm_int/esw/ipmc.h>

#define BCM_L3_ECMP_MAX_2K_GROUPS 2048

/* ECMP mode encodings match hardware values */
#define ECMP_MODE_SINGLE 0x1
#define ECMP_MODE_SINGLE_LOW_LATENCY 0x0
#define ECMP_MODE_HIERARCHICAL 0x3

#define RH_OPT_MAX_PATHS 16384
#define RH_OPT_MIN_PATHS 64

/* ECMP LB mode encodings match hardware values */
#define BCM_ECMP_LB_MODE_RH 0x01
#define BCM_ECMP_LB_MODE_RANDOM 0x2
#define BCM_ECMP_LB_MODE_RR 0x3

#ifdef BCM_WARM_BOOT_SUPPORT
/* ECMP member table size */
#define RH_ECMP_MEMBER_TBL_MAX    16384
#endif /* BCM_WARM_BOOT_SUPPORT */

#define L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE 0
#define VNTAG_ACTIONS_NIV   1
#define VNTAG_ACTIONS_ETAG  2

#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_th_group_lt_prio_update(int unit, _field_group_t *fg,
                                              int priority);
extern int _bcm_field_th_group_status_calc(int unit, _field_group_t *fg);
extern int _bcm_field_th_group_enable_set(int unit, bcm_field_group_t group, uint32 enable);
extern int _bcm_field_th_group_free_unused_slices(int unit,
                                                  _field_stage_t *stage_fc,
                                                  _field_group_t *fg);
extern int _bcm_field_th_group_add_slice_validate(int unit,
                                                  _field_stage_t *stage_fc,
                                                  _field_group_t *fg,
                                                  int slice_id);
extern int _bcm_field_th_entry_part_tcam_idx_get(int unit,
                                                 _field_entry_t *f_ent,
                                                 uint32 idx_pri,
                                                 uint8 entry_part,
                                                 int *idx_out);
extern int _bcm_field_th_slice_offset_to_tcam_idx(int unit,
                                       _field_stage_t *stage_fc,
                                       int instance, int slice, int slice_idx,
                                       int *tcam_idx);
extern int _bcm_field_th_tcam_idx_to_slice_offset(int unit, 
                                       _field_stage_t *stage_fc,
                                       _field_entry_t *f_ent, int tcam_idx,
                                       int *slice, int *slice_idx);
extern int _bcm_field_th_qual_sec_info_insert(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_qual_sec_info_t *ptr);
extern int _bcm_field_th_tcam_part_to_slice_number(int entry_part,
                                                   uint32 group_flags,
                                                   uint8 *slice_number);
extern int _bcm_field_th_tcam_part_to_entry_flags(int unit, int entry_part,
                                                  uint32 group_flags,
                                                  uint32 *entry_flags);
extern int _bcm_field_th_entry_flags_to_tcam_part (int unit, uint32 entry_flags,
                                                   uint32 group_flags, 
                                                   uint8 *entry_part);
extern int _bcm_field_th_lt_tcam_idx_to_slice_offset(int unit,
                                                     _field_stage_t *stage_fc,
                                                     int instance, int tcam_idx,
                                                     int *slice,
                                                     int *slice_idx);
extern int _bcm_field_th_lt_slice_offset_to_tcam_index(int unit,
                                                       _field_stage_t *stage_fc,
                                                       int instance, int slice,
                                                       int slice_idx,
                                                       int *tcam_idx);
extern int _bcm_field_th_entry_tcam_parts_count(int unit, uint32 group_flags,
                                                int *part_count);
extern int _bcm_field_th_ext_config_insert(int unit, _field_stage_t *stage_fc,
                                           _field_ext_conf_mode_t emode,
                                           int level,
                                           _field_ext_cfg_t *ext_cfg);
extern int _bcm_field_th_stage_ext_destroy(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_th_stage_ext_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_th_group_deinit(int unit, _field_group_t *fg);
extern int _bcm_field_th_group_add(int unit, bcm_field_group_config_t *gc);
extern int _bcm_field_th_stages_delete(int unit, _field_control_t *fc);
extern int _bcm_field_th_stages_add(int unit, _field_control_t *fc);
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
extern int _bcm_field_th_action_set(int unit, soc_mem_t mem, _field_entry_t *f_ent,
                                    int tcam_idx, _field_action_t *fa, uint32 *entbuf);
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

extern int _bcm_field_th_group_oper_mode_set(int unit,
                                             bcm_field_qualify_t stage,
                                             bcm_field_group_oper_mode_t mode);

extern int _bcm_field_th_group_oper_mode_get(int unit,
                                             bcm_field_qualify_t stage,
                                             bcm_field_group_oper_mode_t *mode);

extern int soc_th_field_mem_mode_get(int unit,
                                     soc_mem_t mem,
                                     int *mode);

extern int _bcm_field_th_entry_install(int unit, _field_entry_t *f_ent,
                                       int tcam_idx);

extern int _bcm_field_th_entry_reinstall(int unit, _field_entry_t *f_ent,
                                         int tcam_idx);

extern int _bcm_field_th_qual_tcam_key_mask_get(int unit,
                                                _field_entry_t *f_ent,
                                                _field_tcam_t *tcam);

extern int
_bcm_field_th_entry_remove(int unit, _field_entry_t *f_ent, int tcam_idx);

extern int
_bcm_field_th_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new);
extern int
_bcm_field_th_entry_enable_set(int unit, _field_entry_t *f_ent,
                               int enable_flag);
extern int
_bcm_field_th_slice_enable_set(int unit, _field_group_t *fg,
                               _field_slice_t *fs, uint8 enable);
extern int _bcm_field_th_ingress_stat_hw_alloc(int unit,
                                    _field_entry_t *f_ent);
extern int _bcm_field_th_stat_action_set(int unit, _field_entry_t *f_ent,
                                         soc_mem_t mem, int tcam_idx,
                                         uint32 *buf);
extern int
_bcm_field_th_forwardingType_set(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 bcm_field_ForwardingType_t type,
                                 uint32 *data, uint32 *mask);
extern int 
_bcm_field_th_forwardingType_get(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 uint32 data,  uint32 mask,
                                 bcm_field_ForwardingType_t *type);
extern int
_bcm_field_th_qualify_class(int unit,
                            bcm_field_entry_t entry,
                            bcm_field_qualify_t qual,
                            uint32 *data,
                            uint32 *mask);
extern int 
_bcm_field_th_qualify_svp(int unit, bcm_field_entry_t entry, 
                          bcm_field_qualify_t qual, 
                          uint32 data, uint32 mask,
                          int svp_valid);
extern int
_bcm_field_th_qualify_dvp(int unit, bcm_field_entry_t entry, 
                           bcm_field_qualify_t qual, 
                           uint32 data, uint32 mask,
                           int dvp_valid);

extern int
_bcm_field_th_qualify_trunk(int unit,
                            bcm_field_entry_t entry,
                            bcm_field_qualify_t qual,
                            bcm_trunk_t data, bcm_trunk_t mask);

int
_bcm_field_th_qualify_trunk_get(int unit,
                                bcm_field_entry_t entry,
                                bcm_field_qualify_t qual,
                                bcm_trunk_t *data, bcm_trunk_t *mask);
extern int
_bcm_field_th_qualify_set(int unit, bcm_field_entry_t entry,
                          int qual, uint32 *data, uint32 *mask);
extern int
_bcm_field_th_qual_value_set(int unit, _bcm_field_qual_offset_t *q_offset, 
                             _field_entry_t *f_ent, 
                             uint32 *p_data, uint32 *p_mask);
extern int
_bcm_field_th_find_empty_slice(int unit, _field_group_t *fg,
                               _field_slice_t **fs_ptr);
extern int
_bcm_field_th_qual_offset_get(int unit, _field_entry_t *f_ent, int qid, 
                             int entry_part, _bcm_field_qual_offset_t **offset);
extern int
_bcm_field_th_entry_qualifier_key_get(int unit, 
                                      bcm_field_entry_t entry, 
                                      int qual_id, 
                                      _bcm_field_qual_data_t q_data,
                                      _bcm_field_qual_data_t q_mask);
extern int 
_bcm_field_th_qualifier_delete(int unit, bcm_field_entry_t entry, 
                               bcm_field_qualify_t qual_id);
extern int
_bcm_field_th_tcam_policy_mem_get(int unit, 
                                  _field_entry_t *f_ent,
                                  soc_mem_t *tcam_mem,
                                  soc_mem_t *policy_mem);
extern int _bcm_field_th_qualify_InPorts(int unit,
                                         bcm_field_entry_t entry,
                                         bcm_field_qualify_t qual,
                                         bcm_pbmp_t data, 
                                         bcm_pbmp_t mask,
                                         bcm_pbmp_t valid_data);
extern int
_bcm_field_th_class_stage_add(int unit, _field_control_t *fc,
                              _field_stage_t *stage_fc);
extern int
_bcm_field_th_class_stage_status_init(int unit, _field_stage_t *stage_fc);

extern int
_bcm_field_th_class_stage_status_deinit(int unit, _field_stage_t *stage_fc);

extern int
_bcm_field_th_class_group_add(int unit, _field_group_add_fsm_t *fsm_ptr,
                              bcm_field_aset_t aset);
extern int
_bcm_field_th_data_qualifier_hw_alloc(int unit,
                                      _field_stage_t *stage_fc,
                                      _field_data_qualifier_t *f_dq);
extern int 
_bcm_field_th_field_qset_data_qualifier_add(int unit,
                                            bcm_field_qset_t *qset,
                                            int qual_id);
extern int 
_bcm_field_th_field_qset_data_qualifier_delete(int unit, bcm_field_qset_t *qset,
                                                int qual_id);
extern int 
_bcm_field_th_field_qset_data_qualifier_get(int idx, 
                                       bcm_field_qset_t qset,
                                       bcm_field_qualify_t *qid); 
extern int 
_bcm_field_th_field_qualify_data_elem(int unit, bcm_field_entry_t eid,
                                      _field_data_qualifier_t *f_dq,
                                      uint8 qual_elem_idx,
                                      uint32 data, uint32 mask);
extern int 
_bcm_field_th_field_qualify_data_get(int unit, bcm_field_entry_t eid, int qual_id,
        uint16 length_max,  uint8 *data, uint8 *mask,
        uint16 *length);

extern int
_bcm_field_group_flex_ctr_pool_bmp_update(int unit,
                                         bcm_field_group_t group,
                                         int pool_number);
extern int
_bcm_field_group_flex_ctr_info_get(int unit,
                               bcm_field_group_t group, int *instance,
                               _field_counter_pool_bmp_t *counter_pool_bmp);
extern int
_bcm_field_th_ingress_slice_clear(int unit,
                                  _field_group_t *fg,
                                  _field_slice_t *fs);
extern int
_bcm_field_th_qualify_class_get(int unit,
                                 bcm_field_entry_t entry,
                                 bcm_field_qualify_t qual,
                                 uint32 *data,
                                 uint32 *mask);
#ifdef BROADCOM_DEBUG
extern void
_bcm_field_th_class_stage_dump(int unit, _field_stage_t *stage_fc);

extern int
_bcm_field_th_class_group_dump(int unit, _field_group_t *class_fg);

extern int
_bcm_field_th_class_group_brief_dump(int unit, _field_group_t *class_fg);

extern int
_bcm_field_th_class_entry_dump(int unit, _field_entry_t *class_ent);
#endif

extern int
_bcm_field_th_class_entry_create(int unit, bcm_field_group_t group,
                                 bcm_field_entry_t entry);

extern int
_bcm_field_th_class_entry_destroy(int unit, _field_entry_t *entry);

extern int
_bcm_field_th_class_qualify_set(int unit, bcm_field_entry_t entry,
                                int qual, uint32 *data, uint32 *mask);
extern int
_bcm_field_th_class_qualify_clear(int unit, bcm_field_entry_t entry);

extern int
_bcm_field_th_class_entry_qualifier_key_get(int unit,
                                            bcm_field_entry_t entry,
                                            int qual_id,
                                            _bcm_field_qual_data_t q_data,
                                            _bcm_field_qual_data_t q_mask);
extern int
_bcm_field_th_class_action_set(int unit, bcm_field_entry_t entry,
                               bcm_field_class_info_t *class_info);
extern int
_bcm_field_th_class_action_delete(int unit, bcm_field_entry_t entry,
                                  bcm_field_action_t action);
extern int
_bcm_field_th_class_action_get(int unit, bcm_field_entry_t entry,
                               bcm_field_class_info_t *class_info);
extern int
_bcm_field_th_qual_class_size_get(int unit , bcm_field_qualify_t qual,
                                  uint16 *class_size);
extern int
_bcm_field_th_class_entry_install(int unit, bcm_field_entry_t entry);

extern int
_bcm_field_th_class_entry_hwinstall(int unit, _field_entry_t *entry,
                                    _field_class_type_t ctype);
extern int
_bcm_field_th_class_entry_hwread(int unit, _field_entry_t *entry,
                                 _field_class_type_t ctype,
                                 uint32 *cdata);
extern int
_bcm_field_th_class_entry_remove(int unit, bcm_field_entry_t entry);

#if defined(BROADCOM_DEBUG)
extern int
_bcm_field_th_entry_phys_dump(int unit, _field_entry_t *f_ent);
#endif /* BROADCOM_DEBUG */
#endif /* !BCM_FIELD_SUPPORT */

#if defined(INCLUDE_L3)

extern bcm_error_t 
_bcm_niv_gport_get(int unit, int is_trunk, int modid,
                   int port, int dst_vif, bcm_gport_t *niv_gport);
extern bcm_error_t 
_bcm_extender_gport_get(int unit, int is_trunk, int modid,
                        int port, int etag_vid, bcm_gport_t *ext_gport);
extern void
_bcm_th_glp_resolve_embedded_nh(int unit, uint32 gport,
                                bcm_module_t *modid, bcm_port_t *port,
                                int *is_trunk);
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
extern int _bcm_th_ipmc_l3_intf_next_hop_get(int unit, int intf, int *nh_index);
extern int _bcm_th_ipmc_l3_intf_next_hop_l3_egress_set(int unit, int intf);
extern bcm_error_t _th_ipmc_info_get(int unit, int ipmc_index, bcm_ipmc_addr_t *ipmc, 
                                     ipmc_entry_t *entry,
                                     _bcm_esw_ipmc_l3entry_t *use_ipmc_l3entry);
extern bcm_error_t _th_ipmc_glp_set(int unit, bcm_ipmc_addr_t *ipmc,
                                    _bcm_l3_cfg_t *l3cfg);
extern bcm_error_t _th_ipmc_glp_get(int unit, bcm_ipmc_addr_t *ipmc,
                                    _bcm_l3_cfg_t *l3cfg);

extern int _bcm_th_ipmc_src_port_compare(int unit, int ipmc_index,
                                         bcm_ipmc_addr_t *ipmc, int *match);
extern bcm_error_t _bcm_th_macda_oui_profile_init(int unit);
extern void _bcm_th_macda_oui_profile_deinit(int unit);
extern bcm_error_t _bcm_th_vntag_etag_profile_init(int unit);
extern void _bcm_th_vntag_etag_profile_deinit(int unit);
extern bcm_error_t _bcm_th_l3_vp_entry_add(int unit, soc_mem_t mem,
                                           _bcm_l3_cfg_t *l3cfg, uint32 *buf_p);
extern bcm_error_t _bcm_th_l3_vp_ent_parse(int unit, soc_mem_t mem,
                                           _bcm_l3_cfg_t *l3cfg, void *l3x_entry);
extern bcm_error_t
_bcm_th_l3_vp_entry_del(int unit, _bcm_l3_cfg_t *l3cfg,
                        int macda_oui_profile_index ,int vntag_etag_profile_index);
extern bcm_error_t
_bcm_th_get_extended_profile_index(int unit, soc_mem_t mem_ext,
                                   _bcm_l3_cfg_t *l3cfg,
                                   int *macda_oui_profile_index,
                                   int *vntag_etag_profile_index,
                                   int *ref_count);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_th_ipmc_repl_l3_intf_scache_size_get(int unit, uint32 *size);
extern int _bcm_th_ipmc_repl_l3_intf_sync(int unit, uint8 **scache_ptr);
extern int _bcm_th_ipmc_repl_l3_intf_scache_recover(int unit, uint8 **scache_ptr);
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
                                       int group_size,
                                       int max_paths);
extern int bcm_th_ecmp_rh_get_multipath(int unit, int ecmp_group_idx, int intf_size,
                                        bcm_if_t *intf_array, int *intf_count,
                                        int *max_paths);
extern int bcm_th_l3_egress_rh_ecmp_find(int unit, int intf_count,
                                         bcm_if_t *intf_array, bcm_if_t *mpintf);
extern int bcm_th_ecmp_lb_mode_set(int unit,
                                   int ecmp_group_idx,
                                   uint8 lb_mode);
extern int bcm_th_ecmp_lb_mode_reset(int unit,
                                     bcm_if_t mpintf);
extern int bcm_th_ecmp_group_is_rh(int unit,
                                   int ecmp_group_idx);
extern int bcm_th_l3_egress_ecmp_rh_shared_copy(int unit, int old_ecmp_group,
                                                int new_ecmp_group);

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
extern int bcm_th_port_drain_cells(int unit, int port);

extern int bcm_th_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_th_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);

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
extern int
bcm_th_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);

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

extern int
bcm_th_cosq_field_classifier_get(
    int unit, int classifier_id, bcm_cosq_classifier_t *classifier);

extern int
bcm_th_cosq_field_classifier_id_create(
    int unit, bcm_cosq_classifier_t *classifier, int *classifier_id);

extern int
bcm_th_cosq_field_classifier_id_destroy(
    int unit, int classifier_id);

extern int
bcm_th_cosq_field_classifier_map_set(
    int unit, int classifier_id, int count,
    bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_th_cosq_field_classifier_map_get(
    int unit, int classifier_id,
    int array_max, bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_th_cosq_field_classifier_map_clear(
    int unit, int classifier_id);

extern int bcm_th_cosq_init(int unit);

extern int bcm_th_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid);

extern int
_bcm_th_port_enqueue_set(int unit, bcm_port_t gport, int enable);

extern int
_bcm_th_port_enqueue_get(int unit, bcm_port_t gport, int *enable);

extern void bcm_th_cosq_sw_dump(int unit);

typedef enum {
    _BCM_TH_COSQ_INDEX_STYLE_WRED_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_TH_COSQ_INDEX_STYLE_WRED_DEVICE,
    _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_TH_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_TH_COSQ_INDEX_STYLE_COS,
    _BCM_TH_COSQ_INDEX_STYLE_COUNT
} _bcm_th_cosq_index_style_t;

extern int _bcm_th_cosq_index_resolve(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, _bcm_th_cosq_index_style_t style,
                                      bcm_port_t *local_port, int *index, int *count);

extern int _bcm_th_cosq_port_cos_resolve(int unit, bcm_port_t port, bcm_cos_t cos,
                                         _bcm_th_cosq_index_style_t style,
                                         bcm_gport_t *gport);

/* OOB FC Tx side global configuration API */
extern int bcm_th_oob_fc_tx_config_set(
    int unit,
    bcm_oob_fc_tx_config_t *config);

/* OOB FC Tx side global configuration get API */
extern int bcm_th_oob_fc_tx_config_get(
    int unit,
    bcm_oob_fc_tx_config_t *config);

extern int
_bcm_th_oob_fc_tx_port_control_set(int unit, bcm_port_t port,
                                   int status, int dir);
extern int
_bcm_th_oob_fc_tx_port_control_get(int unit, bcm_port_t port,
                                   int *status, int dir);

/* OOB FC Tx side global info get API */
extern int bcm_th_oob_fc_tx_info_get(
    int unit,
    bcm_oob_fc_tx_info_t *info);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Set
 * API
 */
extern int bcm_th_oob_fc_rx_port_tc_mapping_multi_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_count,
    uint32 *tc,
    uint32 *pri_bmp);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Get
 * API
 */
extern int bcm_th_oob_fc_rx_port_tc_mapping_multi_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_max,
    uint32 *tc,
    uint32 *pri_bmp,
    int *array_count);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping set API */
extern int bcm_th_oob_fc_rx_port_tc_mapping_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 pri_bmp);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping get API */
extern int bcm_th_oob_fc_rx_port_tc_mapping_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 *pri_bmp);

/* OOB FC Rx Interface side Configuration set API */
extern int bcm_th_oob_fc_rx_config_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_count,
    bcm_gport_t *gport_array);

/* OOB FC Rx Interface side Configuration get API */
extern int bcm_th_oob_fc_rx_config_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_max,
    bcm_gport_t *gport_array,
    int *array_count);

/* OOB FC Rx Interface side Channel offset value for a port */
extern int bcm_th_oob_fc_rx_port_offset_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 *offset);

/* OOB STATS global configuration set API */
extern int bcm_th_oob_stats_config_set(
    int unit,
    bcm_oob_stats_config_t *config);

/* OOB STATS global configuration get API */
extern int bcm_th_oob_stats_config_get(
    int unit,
    bcm_oob_stats_config_t *config);


/* OOB STATS configuration of service pool list multi set API */
extern int bcm_th_oob_stats_pool_mapping_multi_set(
    int unit,
    int array_count,
    int *offset_array,
    uint8 *dir_array,
    bcm_service_pool_id_t *pool_array);

/* OOB STATS configuration of service pool list multi get API */
extern int bcm_th_oob_stats_pool_mapping_multi_get(
    int unit,
    int array_max,
    int *offset_array,
    uint8 *dir_array,
    bcm_service_pool_id_t *pool_array,
    int *array_count);

/* OOB STATS configuration of service pool list set API */
extern int bcm_th_oob_stats_pool_mapping_set(
    int unit,
    int offset,
    uint8 dir,
    bcm_service_pool_id_t pool);

/* OOB STATS configuration of service pool list get API */
extern int bcm_th_oob_stats_pool_mapping_get(
    int unit,
    int offset,
    uint8 *dir,
    bcm_service_pool_id_t *pool);

/* OOB STATS configuration of Queue list set API */
extern int bcm_th_oob_stats_queue_mapping_multi_set(
    int unit,
    int array_count,
    int *offset_array,
    bcm_gport_t *gport_array);

/* OOB STATS configuration of Queue list get API */
extern int bcm_th_oob_stats_queue_mapping_multi_get(
    int unit,
    int array_max,
    int *offset_array,
    bcm_gport_t *gport_array,
    int *array_count);

/* OOB STATS configuration of Queue list set API */
extern int bcm_th_oob_stats_queue_mapping_set(
    int unit,
    int offset,
    bcm_gport_t gport);

/* OOB STATS configuration of Queue list get API */
extern int bcm_th_oob_stats_queue_mapping_get(
    int unit,
    int offset,
    bcm_gport_t *gport);

#ifdef  ALPM_ENABLE
extern int _bcm_th_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
extern int _bcm_th_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                              int *nh_ecmp_idx);
#endif

#endif /* BCM_TOMAHAWK_SUPPORT  */

#endif /* !_BCM_INT_TOMAHAWK_H_ */
