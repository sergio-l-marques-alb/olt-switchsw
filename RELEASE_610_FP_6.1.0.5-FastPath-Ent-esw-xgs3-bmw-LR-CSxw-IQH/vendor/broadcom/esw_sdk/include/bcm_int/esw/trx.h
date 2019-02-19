/*
 * $Id: trx.h,v 1.1 2011/04/18 17:10:56 mruas Exp $
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
 * File:        trx.h
 * Purpose:     Function declarations for Triumph Micro Architecture 
 *              based devices.
 */

#ifndef _BCM_INT_TRX_H_
#define _BCM_INT_TRX_H_

#if defined(BCM_TRX_SUPPORT)
#if defined(INCLUDE_L3)

#define BCM_TRX_DEFIP128_HASH_SZ            (0x6)
#define BCM_TRX_DEFIP128(_unit_)             l3_trx_defip_128[(_unit_)]
#define BCM_TRX_DEFIP128_ARR(_unit_)         BCM_TRX_DEFIP128((_unit_))->entry_array
#define BCM_TRX_DEFIP128_IDX_MAX(_unit_)     BCM_TRX_DEFIP128((_unit_))->idx_max
#define BCM_TRX_DEFIP128_USED_COUNT(_unit_)  BCM_TRX_DEFIP128((_unit_))->used_count
#define BCM_TRX_DEFIP128_TOTAL(_unit_)       BCM_TRX_DEFIP128((_unit_))->total_count
#define BCM_TRX_DEFIP128_URPF_OFFSET(_unit_) BCM_TRX_DEFIP128((_unit_))->urpf_offset
#define BCM_TRX_DEFIP128_ENTRY_SET(_unit_, _idx_, _plen_, _hash_)          \
            BCM_TRX_DEFIP128_ARR((_unit_))[(_idx_)].prefix_len = (_plen_); \
            BCM_TRX_DEFIP128_ARR((_unit_))[(_idx_)].entry_hash = (_hash_)


typedef struct _bcm_l3_defip_128_entry_s {
    uint8  prefix_len; /* Route entry  prefix length.*/
    uint8  entry_hash; /* Route entry key hash.      */
} _bcm_l3_defip_128_entry_t;

typedef struct _bcm_l3_defip_128_table_s {
    _bcm_l3_defip_128_entry_t *entry_array; /* Cam entries array.          */
    uint16 idx_max;                   /* Last cam entry index.             */
    uint16 used_count;                /* Used cam entry count.             */
    uint16 total_count;               /* Total number of available entries.*/
    uint16 urpf_offset;               /* Src lookup start offset.          */
} _bcm_l3_defip_128_table_t;


extern int _bcm_trx_defip_128_init(int unit);
extern int _bcm_trx_defip_128_deinit(int unit);
extern int _bcm_trx_defip_128_get(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                  int *nh_ecmp_idx); 
extern int _bcm_trx_defip_128_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                  int nh_ecmp_idx);
extern int _bcm_trx_defip_128_delete(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_trx_defip_128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
extern int _bcm_trx_l3_defip_mem_get(int unit, uint32 flags, 
                                     int plen, soc_mem_t *mem);
extern int _bcm_trx_tnl_type_to_hw_code(int unit, bcm_tunnel_type_t tnl_type, 
                                        int *hw_code, int *entry_type);
extern int _bcm_trx_tnl_hw_code_to_type(int unit, int hw_tnl_type, 
                                         int entry_type, bcm_tunnel_type_t
                                         *tunnel_type);
extern int _bcm_trx_tunnel_dscp_map_port_set(int unit, bcm_port_t port,
                                             bcm_tunnel_dscp_map_t *dscp_map);
extern int _bcm_trx_tunnel_dscp_map_port_get(int unit, bcm_port_t port,
                                             bcm_tunnel_dscp_map_t *dscp_map);
extern int _bcm_trx_multicast_ipmc_read(int unit, int ipmc_id, 
                                        bcm_pbmp_t *l2_pbmp,
                                        bcm_pbmp_t *l3_pbmp);
#endif /* INCLUDE_L3 */
/*
 * Vlan block profile structure.  
 *
 * Purpose  : Implement different block functionality per vlan.
 */
typedef struct _trx_vlan_block_s {
    soc_pbmp_t first_mask;
    soc_pbmp_t second_mask;
    uint8 broadcast_mask_sel;
    uint8 unknown_unicast_mask_sel;
    uint8 unknown_mulitcast_mask_sel;
    uint8 known_mulitcast_mask_sel;
} _trx_vlan_block_t;

extern int _bcm_trx_vlan_action_profile_init(int unit);
extern int _bcm_trx_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 *index);
extern void _bcm_trx_vlan_action_profile_entry_increment(int unit, uint32 index);
extern void _bcm_trx_vlan_action_profile_entry_get(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 index);
extern int _bcm_trx_vlan_action_profile_entry_delete(int unit, uint32 index);
extern int _bcm_trx_vlan_range_profile_init(int unit);
extern int _bcm_trx_vlan_action_profile_detach(int unit);

extern int _bcm_trx_egr_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 *index);
extern void _bcm_trx_egr_vlan_action_profile_entry_increment(int unit, uint32 index);
extern void _bcm_trx_egr_vlan_action_profile_entry_get(int unit,
                                          bcm_vlan_action_set_t *action,
                                          uint32 index);
extern int _bcm_trx_egr_vlan_action_profile_entry_delete(int unit, uint32 index);

extern int _bcm_trx_vlan_ip_action_add(int unit, bcm_vlan_ip_t *vlan_ip,
                                      bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_ip_action_get(int unit, bcm_vlan_ip_t *vlan_ip,
                                      bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_ip_action_traverse(int unit, 
                                            bcm_vlan_ip_action_traverse_cb cb, 
                                            void *user_data);


extern int _bcm_trx_vlan_ip_delete(int unit, bcm_vlan_ip_t *vlan_ip);
extern int _bcm_trx_vlan_ip_delete_all(int unit);
extern int _bcm_trx_vlan_mac_action_add(int unit, bcm_mac_t mac,
                                       bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_mac_delete(int unit, bcm_mac_t mac);
extern int _bcm_trx_vlan_mac_delete_all(int unit);

extern int _bcm_trx_vlan_port_prot_match_get(int unit, 
                                             bcm_port_frametype_t frame, 
                                             bcm_port_ethertype_t ether, 
                                             int *match_idx);
extern int _bcm_trx_vlan_port_prot_empty_get(int unit, int *empty_idx);

extern int _bcm_trx_vlan_translate_action_add(int unit,
                                             bcm_gport_t port,
                                             bcm_vlan_translate_key_t key_type,
                                             bcm_vlan_t outer_vlan,
                                             bcm_vlan_t inner_vlan,
                                             bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_translate_action_delete(int unit,
                                                bcm_gport_t port,
                                                bcm_vlan_translate_key_t key_type,
                                                bcm_vlan_t outer_vlan,
                                                bcm_vlan_t inner_vlan);
extern int _bcm_trx_vlan_translate_action_delete_all(int unit);
extern int _bcm_trx_vlan_translate_action_get (int unit, 
                                              bcm_gport_t port, 
                                              bcm_vlan_translate_key_t key_type, 
                                              bcm_vlan_t outer_vid,
                                              bcm_vlan_t inner_vid,
                                              bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_translate_action_range_add(int unit,
                                                   bcm_gport_t port,
                                                   bcm_vlan_t outer_vlan_low,
                                                   bcm_vlan_t outer_vlan_high,
                                                   bcm_vlan_t inner_vlan_low,
                                                   bcm_vlan_t inner_vlan_high,
                                                   bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_translate_action_range_get(int unit,
                                                   bcm_gport_t port,
                                                   bcm_vlan_t outer_vlan_low,
                                                   bcm_vlan_t outer_vlan_high,
                                                   bcm_vlan_t inner_vlan_low,
                                                   bcm_vlan_t inner_vlan_high,
                                                   bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_translate_action_range_delete(int unit,
                                                      bcm_gport_t port,
                                                      bcm_vlan_t outer_vlan_low,
                                                      bcm_vlan_t outer_vlan_high,
                                                      bcm_vlan_t inner_vlan_low,
                                                      bcm_vlan_t inner_vlan_high);
extern int _bcm_trx_vlan_translate_action_range_delete_all(int unit);
extern int _bcm_trx_vlan_translate_action_range_traverse(int unit, 
                                  _bcm_vlan_translate_traverse_t *trvs_info);

extern int _bcm_trx_vlan_translate_parse(int unit, soc_mem_t mem, uint32 *vent, 
                                  _bcm_vlan_translate_traverse_t *trvs_info);
extern int _bcm_trx_vlan_translate_egress_action_add(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_translate_egress_action_get(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_translate_egress_action_delete(int unit, int port_class,
                                            bcm_vlan_t outer_vlan,
                                            bcm_vlan_t inner_vlan);
extern int _bcm_trx_vlan_mac_action_traverse(int unit, 
                                             bcm_vlan_mac_action_traverse_cb cb, 
                                             void *user_data);
extern int _bcm_trx_vlan_mac_action_get(int unit, bcm_mac_t  mac,
                             bcm_vlan_action_set_t *action);
extern int _bcm_trx_vlan_translate_egress_action_delete_all(int unit);
extern int bcm_trx_metro_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan);
extern int bcm_trx_metro_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan);
extern int bcm_trx_metro_l2_tunnel_delete_all(int unit);
extern int _bcm_common_init(int unit);
extern int _bcm_common_cleanup(int unit);
extern int _bcm_mac_da_profile_entry_add(int unit, void **entries, 
                                         int entries_per_set, uint32 *index);
extern int _bcm_mac_da_profile_entry_delete(int unit, int index);
extern int _bcm_lport_profile_entry_add(int unit, void **entries, 
                                         int entries_per_set, uint32 *index);
extern int _bcm_lport_profile_entry_delete(int unit, int index);
extern int _bcm_ing_pri_cng_map_entry_add(int unit, void **entries, 
                                         int entries_per_set, uint32 *index);
extern int _bcm_ing_pri_cng_map_entry_delete(int unit, int index);
extern int _bcm_egr_mpls_pri_map_entry_add(int unit, void **entries, 
                                         int entries_per_set, uint32 *index);
extern int _bcm_egr_mpls_pri_map_entry_delete(int unit, int index);
extern int _bcm_egr_mpls_exp_map_entry_add(int unit, void **entries, 
                                         int entries_per_set, uint32 *index);
extern int _bcm_egr_mpls_exp_map_entry_delete(int unit, int index);
extern int _bcm_dscp_table_entry_add(int unit, void **entries, 
                                         int entries_per_set, uint32 *index);
extern int _bcm_dscp_table_entry_delete(int unit, int index);
extern int _bcm_egr_dscp_table_entry_add(int unit, void **entries, 
                                         int entries_per_set, uint32 *index);
extern int _bcm_egr_dscp_table_entry_delete(int unit, int index);
extern int _bcm_trx_egr_src_port_outer_tpid_set(int unit, int tpid_index,
                                                int enable);
extern int _bcm_trx_source_trunk_map_set(int unit, bcm_port_t port, 
                                         soc_field_t field, uint32 value);
extern int _bcm_trx_source_trunk_map_get(int unit, bcm_port_t port, 
                                         soc_field_t field, uint32 *value);
extern int _bcm_trx_port_force_vlan_set(int unit, bcm_port_t port,
                                        bcm_vlan_t vlan, int pkt_prio,
                                        uint32 flags);

extern int _bcm_trx_port_force_vlan_get(int unit, bcm_port_t port,
                                        bcm_vlan_t *vlan, int *pkt_prio, 
                                        uint32 *flags);
extern int  _bcm_trx_port_dtag_mode_set(int unit, bcm_port_t port, int mode);
extern int  _bcm_trx_port_dtag_mode_get(int unit, bcm_port_t port, int *mode);
extern int _bcm_trx_lport_tab_default_entry_add(int unit, 
                                                soc_profile_mem_t *prof);

#if defined(BCM_FIELD_SUPPORT)
#define _FP_TRX_RANGE_CHECKER_LOWER_MAX (23)
extern int _bcm_field_trx_tcp_ttl_tos_init(int unit);
extern int _bcm_trx_range_checker_selcodes_update(int unit,
                                                  _field_entry_t *f_ent);
extern int _bcm_field_trx_write_slice_map_vfp(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_trx_write_slice_map_egress(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_trx_mirror_ingress_add(int unit, soc_mem_t mem, 
                                             _field_entry_t *f_ent,
                                             _field_action_t *fa, uint32 *buf);
extern int _bcm_field_trx_mirror_egress_add(int unit, soc_mem_t mem, 
                                            _field_entry_t *f_ent,
                                            _field_action_t *fa, uint32 *buf);
extern int _bcm_field_trx_mirror_ingress_delete(int unit, soc_mem_t mem, 
                                                _field_entry_t *f_ent, 
                                                bcm_gport_t *mirror_dest, 
                                                uint32 *buf);
extern int _bcm_field_trx_mirror_egress_delete(int unit, soc_mem_t mem, 
                                               _field_entry_t *f_ent, 
                                               bcm_gport_t *mirror_dest, 
                                               uint32 *buf);
extern int _bcm_field_trx_mtp_release(int unit, _field_entry_t *f_ent,
                                      int tcam_idx);
extern int _bcm_field_trx_action_support_check(int unit, _field_entry_t *f_ent,
                                                bcm_field_action_t action, 
                                                int *result);
extern int _bcm_field_trx_action_conflict_check(int unit, _field_entry_t *f_ent,
                                                bcm_field_action_t action, 
                                                bcm_field_action_t action1);
extern int _bcm_field_trx_qualify_ip_type(int unit, _field_entry_t *f_ent, 
                                          bcm_field_IpType_t type);
extern int _bcm_field_trx_qualify_ip_type_get(int unit, _field_entry_t *f_ent, 
                                              bcm_field_IpType_t *type);
extern int _bcm_field_trx_egress_key_match_type_set(int unit, 
                                                    _field_entry_t *f_ent);
extern int _bcm_field_trx_action_delete(int unit, _field_entry_t *f_ent,
                                        _field_action_t *fa, int tcam_idx, 
                                        uint32 param0, uint32 param1);
extern int _bcm_field_trx_tcam_get(int unit, soc_mem_t mem, 
                                   _field_entry_t *f_ent, uint32 *buf);
extern int _bcm_field_trx_action_counter_update(int unit, soc_mem_t mem, 
                                                _field_entry_t *f_ent, 
                                                _field_action_t *fa,
                                                uint32 *buf);
extern int _bcm_field_trx_action_get(int unit, soc_mem_t mem,
                                     _field_entry_t *f_ent, 
                                      int tcam_idx, _field_action_t *fa, 
                                      uint32 *buf);
extern int _bcm_field_trx_range_check_set(int unit, int range, uint32 flags, 
                                          int enable, bcm_l4_port_t min, 
                                          bcm_l4_port_t max);
extern int _bcm_field_trx_udf_spec_set(int unit, bcm_field_udf_spec_t *udf_spec, 
                                       uint32 flags, uint32 offset);
extern int _bcm_field_trx_udf_spec_get(int unit, bcm_field_udf_spec_t *udf_spec, 
                                       uint32 *flags, uint32 *offset);
extern int _bcm_field_trx_udf_write(int unit, bcm_field_udf_spec_t *udf_spec, 
                                    uint32 udf_num, uint32 user_num);
extern int _bcm_field_trx_udf_read(int unit, bcm_field_udf_spec_t *udf_spec, 
                                   uint32 udf_num, uint32 user_num);
extern int _bcm_field_trx_udf_index_to_flags(int unit, uint32 tbl_idx, 
                                             int *flags);
extern int _bcm_field_trx_udf_flags_to_index (int unit, int flags, uint32
                                              *tbl_idx);
extern int _bcm_field_trx_mtp_release(int unit, _field_entry_t *f_ent, 
                                      int tcam_idx);
extern int _bcm_field_trx_redirect_profile_delete(int unit, int index);
extern int _bcm_field_trx_redirect_profile_alloc(int unit, _field_entry_t *f_ent, 
                                          _field_action_t *fa);
extern int _bcm_field_trx_redirect_profile_ref_count_get(int unit, int index,
                                                         int *ref_count);
extern int _bcm_field_trx_stat_hw_mode_to_bmap(int unit, uint8 mode, 
                                               _field_stage_id_t stage_id, 
                                               uint32 *hw_bmap,
                                               uint8 *hw_entry_count);
extern int _bcm_field_trx_stat_index_get(int unit, _field_stat_t *f_st, 
                                         bcm_field_stat_t stat,
                                         int *idx1, int *idx2, uint32 *flags);
extern int _bcm_field_trx_stat_action_set(int unit, _field_entry_t *f_ent, 
                                          soc_mem_t mem, uint32 *buf);
extern int _bcm_field_trx_selcodes_install(int unit, _field_group_t *fg, 
                                           uint8 slice_numb, bcm_pbmp_t pbmp, 
                                           int selcode_index);
extern int _bcm_field_trx_policer_install(int unit, _field_entry_t *f_ent, 
                                          _field_policer_t *f_pl);
extern int _bcm_field_trx_policer_action_set(int unit, _field_entry_t *f_ent,
                                             soc_mem_t mem, uint32 *buf);
extern int _bcm_field_trx_control_arp_set(int unit,  
                                          bcm_field_control_t control,
                                          uint32 state);
extern int _bcm_field_trx_egress_selcode_get(int unit, _field_stage_t *stage_fc, 
                               bcm_field_qset_t *qset_req,
                               _field_group_t *fg);
extern int _bcm_field_trx_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs); 
extern int _bcm_field_trx_action_params_check(int unit,_field_entry_t *f_ent, 
                                               _field_action_t *fa);
#endif /* BCM_FIELD_SUPPORT */
extern int _bcm_trx_vlan_block_set (int unit, bcm_vlan_t vid, 
                                    bcm_vlan_block_t *block);
extern int _bcm_trx_vlan_block_get (int unit, bcm_vlan_t vid, 
                                    bcm_vlan_block_t *block);
#endif /* BCM_TRX_SUPPORT */
#endif  /* !_BCM_INT_TRX_H_ */
