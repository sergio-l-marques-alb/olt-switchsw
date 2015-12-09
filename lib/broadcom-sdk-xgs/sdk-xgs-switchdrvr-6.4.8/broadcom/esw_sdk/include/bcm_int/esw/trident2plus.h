/*
 * $Id: $
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
 * File:        trident2plus.h
 * Purpose:     Function declarations for Trident2+ Internal functions.
 */

#ifndef _BCM_INT_TRIDENT2PLUS_H_
#define _BCM_INT_TRIDENT2PLUS_H_

#if defined(BCM_TRIDENT2PLUS_SUPPORT)

#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/l3.h>
#include <bcm/failover.h>
#include <bcm/rx.h>
#include <bcm/mpls.h>
#include <bcm/l3.h>
#include <soc/profile_mem.h>


/* Number of protection switching groups/next hops
 * represented per entry in the PROT_SW tables
 * (INITIAL_PROT_NHI_TABLE_1, TX_INITIAL_PROT_GROUP_TABLE,
 * RX_PROT_GROUP_TABLE, EGR_TX_PROT_GROUP_TABLE and
 * EGR_L3_NEXT_HOP_1)
 */
#define BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY 128

#define _BCM_EGR_PRI_CNG_MAP_CHUNK     64

#define BCM_TD2P_COE_MODULE_1 1
#define BCM_TD2P_COE_MODULE_2 2
#define BCM_TD2P_COE_MODULE_3 3
#define BCM_TD2P_COE_MODULE_4 4
#define BCM_TD2P_COE_MODULE_5 5

#define BCM_TD2P_ENTROPY_LABEL_ID_MAX          (0xfffff)
#define BCM_TD2P_ENTROPY_LABEL_ID_VALID(_label_)                        \
             (_label_ <= BCM_TD2P_ENTROPY_LABEL_ID_MAX)

#define BCM_TD2P_ENTROPY_LABEL_MASK_MAX        (0xfffff)
#define BCM_TD2P_ENTROPY_LABEL_MASK_VALID(_mask_)                       \
             ((_mask_ > 0) && (_mask_ <= BCM_TD2P_ENTROPY_LABEL_MASK_MAX))

#define BCMI_VP_GROUP_INGRESS 0
#define BCMI_VP_GROUP_EGRESS  1

#define BCMI_VP_GROUP_DELETE            0
#define BCMI_VP_GROUP_ADD               1
#define BCMI_VP_GROUP_DEFAULT_FLAGS     0x0

/* Corresponds of maximum logical number for the chip */
#define TD2P_MAX_NUM_PORTS 106

/* This enum denotes the type of access into the
   VP Grp VLAN Membership framework, we can come in
   when ports are being added to VLANs and when the
   ports are being associated with STP states */
typedef enum _bcm_vp_group_access_type_e {
	_bcmVpGrpGlp,   /* VP Group datatype gpp port. */
	_bcmVpGrpVp,   /* VP Group datatype vp port. */
	_bcmVpGrpStg   /* VP Group datatype stg. */
} _bcm_vp_group_access_type_t;


typedef enum _bcm_vp_group_port_type_e {
	_bcmVpGrpPortLocal, /* Front-panel port. */
	_bcmVpGrpPortGlp,   /* GLP port. */
	_bcmVpGrpPortVp    /* VP port. */
} _bcm_vp_group_port_type_t;

typedef struct _bcm_vp_group_stp_vp_grp_map_s {
	soc_field_t stp_vp_group_field;
} _bcm_vp_group_stp_vp_grp_map_t;

extern int bcm_td2p_vp_group_init(int unit);
extern int bcm_td2p_vp_group_detach(int unit);
extern int bcm_td2p_vp_vlan_member_set(int unit, bcm_gport_t gport,
        uint32 flags);
extern int bcm_td2p_vp_vlan_member_get(int unit, bcm_gport_t gport,
        uint32 *flags);

/* 
 
   extern int bcm_td2p_ing_vp_group_move(int unit, int vp, bcm_vlan_t vlan, 
    	int add); 
   extern int bcm_td2p_eg_vp_group_move(int unit, int vp, bcm_vlan_t vlan, 
    	int add);
*/


extern int bcm_td2p_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan);


extern int bcm_td2p_ing_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_td2p_eg_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan);

extern int bcm_td2p_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);

extern int bcm_td2p_ing_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);
extern int bcm_td2p_eg_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);

extern int bcm_td2p_egr_vp_group_unmanaged_set(int unit, int flag);
extern int bcm_td2p_ing_vp_group_unmanaged_set(int unit, int flag);
extern int bcm_td2p_ing_vp_group_unmanaged_get(int unit);
extern int bcm_td2p_egr_vp_group_unmanaged_get(int unit);
extern int bcm_td2p_vlan_vp_group_set(int unit, int vlan_vfi, int egress,
                                    int vp_group,  int enable);
extern int bcm_td2p_vlan_vp_group_get(int unit, int vlan_vfi, int egress,
                                    int vp_group,  int *enable);
extern int bcm_td2p_vlan_vp_group_get_all(int unit, int vlan, int array_max,
             bcm_gport_t *gport_array, int *flags_array, int *port_cnt);


extern int bcm_td2p_vp_group_port_move(int unit, int gport, 
                                       bcm_vlan_t vlan_vfi, 
                                       int add, int egress, int flags);
extern int bcm_td2p_vp_group_state_get(int unit, bcm_port_t gport,
                                       bcm_vlan_t vlan_vfi, int egress,
                                       int *flags);
extern int bcm_td2p_vp_group_vlan_vfi_stg_set(int unit,
                                        int vlan_vfi, int egress, int stg);
extern int bcm_td2p_vp_group_vlan_vfi_stg_get(int unit,
                                        int vlan_vfi, int egress, int *stg);



#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcmi_xgs5_subport_wb_scache_alloc(int unit, uint16 version,
                                           int *alloc_sz);
extern int bcmi_xgs5_subport_wb_sync(int unit, uint16 version,
                                   uint8 **scache_ptr);
extern int bcmi_xgs5_subport_wb_recover(int unit, uint16 version,
                                      uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP

extern void bcm_td2p_vp_group_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef INCLUDE_L3

extern int  bcm_td2p_failover_init(int unit);
extern void bcm_td2p_failover_unlock(int unit);
extern int  bcm_td2p_failover_lock(int unit);
extern int  bcm_td2p_failover_init(int unit);
extern int  bcm_td2p_failover_cleanup(int unit);
extern int  bcm_td2p_failover_create(int unit, uint32 flags,
        bcm_failover_t *failover_id);
extern int  bcm_td2p_failover_destroy(int unit, bcm_failover_t  failover_id);
extern int  bcm_td2p_failover_status_set(int unit,
        bcm_failover_element_t *failover, int value);
extern int  bcm_td2p_failover_status_get(int unit,
        bcm_failover_element_t *failover, int  *value);
extern int  bcm_td2p_failover_prot_nhi_set(int unit, uint32 flags,
        int nh_index, uint32 prot_nh_index, bcm_multicast_t  mc_group,
        bcm_failover_t failover_id);
extern int  bcm_td2p_failover_prot_nhi_get(int unit, int nh_index,
        bcm_failover_t  *failover_id, int  *prot_nh_index,
        bcm_multicast_t  *mc_group);
extern int  bcm_td2p_failover_prot_nhi_cleanup(int unit, int nh_index);
extern int  bcm_td2p_failover_prot_nhi_update(int unit, int old_nh_index,
        int new_nh_index);
extern int _bcm_td2p_failover_ingress_id_validate(int unit,
                                                  bcm_failover_t failover_id);
extern int bcm_td2p_dvp_vlan_xlate_key_set(int unit, 
        bcm_gport_t port_id, int key_type);
extern int bcm_td2p_dvp_vlan_xlate_key_get(int unit, 
        bcm_gport_t port_id, int *key_type);
extern void bcm_td2_vxlan_port_match_count_adjust(int unit, int vp, int step);
extern int bcm_td2p_share_vp_get(int unit, bcm_vpn_t vpn, SHR_BITDCL *vp_bitmap);
extern int bcm_td2p_share_vp_delete(int unit, bcm_vpn_t vpn, int vp);

/* DVP Group ID check*/
extern int bcm_td2p_dvp_group_validate(int unit, int dvp_group_id);

extern int bcm_td2p_mpls_entropy_identifier_add(int unit, uint32 options,
        bcm_mpls_entropy_identifier_t *info);
extern int bcm_td2p_mpls_entropy_identifier_delete(int unit,
        bcm_mpls_entropy_identifier_t *info);
extern int bcm_td2p_mpls_entropy_identifier_delete_all(int unit);
extern int bcm_td2p_mpls_entropy_identifier_get(int unit,
        bcm_mpls_entropy_identifier_t *info);
extern int bcm_td2p_mpls_entropy_identifier_traverse(int unit,
        bcm_mpls_entropy_identifier_traverse_cb cb, void *user_data);

extern int bcm_td2p_l3_defip_init(int unit);
extern void bcm_td2p_l3_defip_deinit(int unit);
extern int bcm_td2p_defip_mc_route_check(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_td2p_defip_ipmc_count_update(int unit);
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)

/* ECMP Single level */
#define BCMI_L3_ECMP_SINGLE_LEVEL         1

/* Overlay validity checks */
#define BCMI_L3_ECMP_IS_MULTI_LEVEL(_u) \
    (((&_bcm_l3_bk_info[unit])->l3_ecmp_levels) > BCMI_L3_ECMP_SINGLE_LEVEL)

#define BCMI_L3_ECMP_OVERLAY_ENTRIES(_u) \
    ((&_bcm_l3_bk_info[_u])->l3_ecmp_overlay_entries)

/* ECMP Macro flow overlay check */
#define BCM_MACROFLOW_IS_OVERLAY(type) \
            ((type == bcmSwitchMacroFlowHashOverlayMinOffset) || \
             (type == bcmSwitchMacroFlowHashOverlayMaxOffset) || \
             (type == bcmSwitchMacroFlowHashOverlayStrideOffset))

#endif

/******************************************************************|
|**************                                   *****************|
|**************           RIOT BLOCK              *****************|
|**************                                   *****************|
 ******************************************************************/

/* RIOT related defines */

#ifdef BCM_RIOT_SUPPORT


/* TD2+ : EGR_L3_NEXT_HOP-KEY_TYPEf */
#define BCMI_L3_EGR_NH_MCAST_ENTRY_TYPE  7
#define BCMI_L3_EGR_NH_SD_TAG_ENTRY_TYPE 3


/* L3 interface banks values */
#define BCMI_L3_INTF_ENT_PER_BANK        2048
#define BCMI_L3_INTF_BANKS_MAX           6

/* L3 next hop banks values */
#define BCMI_L3_NH_ENT_PER_BANK          4096
#define BCMI_L3_NH_BANKS_MAX             12

/* NH destination encodings, masks and macros */
#define BCMI_L3_NH_DEST_DGLP_SHIFT       15
#define BCMI_L3_NH_DEST_DGLP_MASK        0x7FFF   /* 15 bits value */
#define BCMI_L3_NH_DEST_DGLP_EN_MASK     0x7      /* 3 bits value */
#define BCMI_L3_NH_DEST_DGLP_ENCODING    0x0      /* 15 bits value */
#define BCMI_L3_NH_DEST_DVP_SHIFT        14       /* 14 bits value */
#define BCMI_L3_NH_DEST_DVP_MASK         0x3FFF   /* 14 bits value */
#define BCMI_L3_NH_DEST_DVP_EN_MASK      0xF      /* 4 bits value */
#define BCMI_L3_NH_DEST_DVP_ENCODING     0x3      /* 15 bits value */
#define BCMI_L3_NH_DEST_LAG_SHIFT        14       /* 14 bits value */
#define BCMI_L3_NH_DEST_LAG_MASK         0x3FFF   /* 14 bits value */
#define BCMI_L3_NH_DEST_LAG_EN_MASK      0xF      /* 4 bits value */
#define BCMI_L3_NH_DEST_LAG_ENCODING     0x2      /* 15 bits value */
#define BCMI_L3_NH_DEST_DROP_SHIFT       12       /* 12 bits value */
#define BCMI_L3_NH_DEST_DROP_MASK        0xFFF    /* 12 bits value */
#define BCMI_L3_NH_DEST_DROP_EN_MASK     0x3F     /* 6 bits value */
#define BCMI_L3_NH_DEST_DROP_ENCODING    0x1b     /* 15 bits value */


#define BCMI_L3_NH_DEST_DVP_GET(gport) \
            ((BCMI_L3_NH_DEST_DVP_ENCODING << BCMI_L3_NH_DEST_DVP_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_DVP_MASK))

#define BCMI_L3_NH_DEST_LAG_GET(gport) \
            ((BCMI_L3_NH_DEST_LAG_ENCODING << BCMI_L3_NH_DEST_LAG_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_LAG_MASK))

#define BCMI_L3_NH_DEST_DGLP_GET(gport) \
            ((BCMI_L3_NH_DEST_DGLP_ENCODING << BCMI_L3_NH_DEST_DGLP_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_DGLP_MASK))

#define BCMI_L3_NH_DEST_DROP_GET(gport) \
            ((BCMI_L3_NH_DEST_DROP_ENCODING << BCMI_L3_NH_DEST_DROP_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_DROP_MASK))

#define BCMI_L3_NH_DEST_IS_LAG(_dest)  \
            ((((_dest) >> BCMI_L3_NH_DEST_LAG_SHIFT) & BCMI_L3_NH_DEST_LAG_EN_MASK) == \
            BCMI_L3_NH_DEST_LAG_ENCODING)

#define BCMI_L3_NH_PORT_LAG_GET(_dest) \
            (((_dest) & BCMI_L3_NH_DEST_LAG_MASK))

#define BCMI_L3_NH_DEST_IS_DGLP(_dest)  \
            ((((_dest) >> BCMI_L3_NH_DEST_DGLP_SHIFT) & BCMI_L3_NH_DEST_DGLP_EN_MASK) == \
            BCMI_L3_NH_DEST_DGLP_ENCODING)
#define BCMI_L3_NH_PORT_DGLP_GET(_dest) \
            (((_dest) & BCMI_L3_NH_DEST_DGLP_MASK))

#define BCMI_L3_NH_DEST_IS_DVP(_dest)  \
            ((((_dest) >> BCMI_L3_NH_DEST_DVP_SHIFT) & BCMI_L3_NH_DEST_DVP_EN_MASK) == \
            BCMI_L3_NH_DEST_DVP_ENCODING)

#define BCMI_L3_NH_PORT_DVP_GET(_dest) \
            (((_dest) & BCMI_L3_NH_DEST_DVP_MASK))

/* Td2+ : EGR_L3_NEXT_HOP-NEXT_PTR field types */
#define BCMI_L3_NH_EGR_NEXT_PTR_TYPE_DVP    1
#define BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH     2

/* RioT Overlay validity checks */
#define BCMI_RIOT_IS_ENABLED(_u) \
    ((&_bcm_l3_bk_info[_u])->riot_enable)

#define BCMI_L3_NH_OVERLAY_VALID(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    (((&_bcm_l3_bk_info[_u])->l3_nh_overlay_entries) > 0))

#define BCMI_L3_INTF_OVERLAY_VALID(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    (((&_bcm_l3_bk_info[_u])->l3_intf_overlay_entries) > 0))

#define BCMI_RIOT_VPN_VFI_IS_SET(_u, _vid) \
    ((BCMI_RIOT_IS_ENABLED(_u)) && (_BCM_VPN_VFI_IS_SET((_vid))))

#define BCMI_RIOT_VPN_VFI_IS_NOT_SET(_u, _vid) \
    ((!BCMI_RIOT_IS_ENABLED(_u)) || (!(_BCM_VPN_VFI_IS_SET((_vid)))))

#define BCMI_GPORT_TYPE_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)

/*
 * The below infrastructure is done specially for nh-nh association management.
 * When overlay nh is associated with underlay nh, we will have to keep
 * backward tracing to redraw association if underlay NH is changed.
 */
typedef struct ol_nh_link_s {
    int o_nh_idx;
    struct ol_nh_link_s *next_link;
} ol_nh_link_t;

typedef struct ul_nh_link_s {
    int u_nh_idx;
    ol_nh_link_t *ol_nh_assoc;
    struct ul_nh_link_s *ul_nh_link;
} ul_nh_link_t;

/* TD2P specific routines */
extern int bcm_td2p_l3_tables_init(int unit);
extern int bcmi_l3_riot_bank_sel(int unit);
extern int bcmi_get_port_from_destination(int unit,
               uint32 dest, bcm_l3_egress_t *nh);

/* ol-ul association routines */
extern int bcmi_l3_nh_assoc_ol_ul_link_sw_add(int unit,
               int ol_nh, int ul_nh);
extern int bcmi_l3_nh_assoc_ol_ul_link_delete(int unit, int nh_idx);
extern int bcmi_l3_nh_assoc_ol_ul_link_is_required(int unit, int vp_nh_index);
extern int bcmi_l3_nh_assoc_ol_ul_link_replace(int unit,
               int old_ul, int new_ul);
extern int bcmi_l3_nh_assoc_ol_ul_link_sw_delete(int unit, int ol_nh,
               int ul_nh);
extern int bcmi_l3_nh_assoc_ol_ul_link_reinit(int unit, int ol_nh_idx);
extern int bcmi_l3_nh_dest_set(int unit,
               bcm_l3_egress_t *nh_entry, uint32 *nh_dest);

/******************************************************************|
|**************                                   *****************|
|**************    RIOT BLOCK - Ends Here         *****************|
|**************                                   *****************|
 ******************************************************************/
#endif /* BCM_RIOT_SUPPORT */

#endif /* INCLUDE_L3 */

/* VLAN VFI */
extern int bcm_td2p_vlan_table_port_get(int unit, bcm_vlan_t vid,
        pbmp_t *ing_pbmp, pbmp_t *pbmp);
extern int bcm_td2p_vlan_vfi_membership_init(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);
extern int bcm_td2p_ing_vlan_vfi_membership_add(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);
extern int bcm_td2p_ing_vlan_vfi_membership_delete(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);
extern int bcm_td2p_delete_vlan_vfi_membership(int unit, bcm_vlan_t vlan);
extern int bcm_td2p_egr_vlan_vfi_membership_add(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);
extern int bcm_td2p_egr_vlan_vfi_membership_delete(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);

extern int bcm_td2p_vp_group_stg_move(int unit, int stg, int port, int stp_state,
                           int egress);

extern int bcm_td2p_vp_group_stg_default_state_set(int unit, int stg, int egress, 
                                        int stp_state);

extern int bcm_td2p_vp_group_move(int unit, int gport, bcm_vlan_t vlan_vfi, int stg,
                       int add, int egress);

extern int _bcm_vlan_vfi_membership_profile_entry_op(int unit,
                                                     void **entries,
                                                     int entries_per_set,
                                                     int del,
                                                     int egress,
                                                     uint32 *index);

extern int _bcm_vlan_vfi_membership_profile_mem_reference(int unit,
                                                          int index,
                                                          int entries_per_set,
                                                          int egress);

extern int _bcm_td2p_vp_group_port_type_get(int unit, int port,
                                 _bcm_vp_group_port_type_t *port_type);

extern int bcm_td2p_vp_group_stg_set(int unit, int stg, int gport, int egress,
                                     int stp_state);
extern int bcm_td2p_vp_group_stg_get(int unit, int stg, int gport, int egress,
                                     int *stp_state);

extern int _bcm_vlan_vfi_mbrship_profile_ref_count(int unit, int index,
                                                   int egress, int *ref_count);

extern int _bcm_td2_vxlan_vpn_is_valid( int unit, bcm_vpn_t l2vpn);
extern int bcmi_td2p_vlan_control_vpn_get(int unit, bcm_vlan_t vid,
               uint32 valid_fields, bcm_vlan_control_vlan_t *control);
extern int bcmi_td2p_vlan_control_vpn_set(int unit, bcm_vlan_t vid,
               uint32 valid_fields, bcm_vlan_control_vlan_t *control);

/* Subport functions */
extern int bcm_td2plus_subport_coe_init(int unit);

extern int bcm_td2p_cosq_ets_mode(int unit);
extern int bcm_td2p_rx_queue_channel_set(int unit,bcm_cos_queue_t queue_id,bcm_rx_chan_t chan_id);

extern int bcm_td2p_oam_init(int unit);
extern int _bcm_td2p_oam_olp_fp_hw_index_get(
            int unit,
            bcm_field_olp_header_type_t olp_hdr_type,
            int *hwindex);
extern int _bcm_td2p_oam_olp_hw_index_olp_type_get(
            int unit,
            int hwindex,
            bcm_field_olp_header_type_t *olp_hdr_type );

extern int bcm_td2p_port_coe_e2ecc(int unit, bcm_port_t port,
        bcm_port_congestion_config_t *config);
extern int bcm_td2p_cosq_subport_flow_control_set(int unit, bcm_gport_t subport, 
                                                  bcm_gport_t sched_port);
extern int bcm_td2p_cosq_subport_flow_control_get(int unit, bcm_gport_t subport, 
                                                  bcm_gport_t * sched_port);

/*
 * Port Functions
 */
extern int
bcmi_td2p_port_fn_drv_init(int unit);
extern int
bcmi_td2p_port_lanes_get(int unit, bcm_port_t port, int *lanes);
extern int
bcmi_td2p_port_lanes_set(int unit, bcm_port_t port, int lanes);

extern int
bcm_td2p_port_vlan_priority_unmap_set(int unit, bcm_port_t port,
                                          int internal_pri, bcm_color_t color,
                                          int pkt_pri, int cfi);
extern int
bcm_td2p_port_vlan_priority_unmap_get(int unit, bcm_port_t port,
                                          int internal_pri, bcm_color_t color,
                                          int *pkt_pri, int *cfi);
extern int
bcm_td2p_port_vlan_priority_unmap_init(int unit);

/* IPMC multicast enhancement */
extern int bcm_td2p_set_repl_port_agg_map(int unit, 
                                          const bcm_port_t *local_member_array,
                                          int local_member_count,
                                          bcm_trunk_t tgid);
extern int bcm_td2p_get_free_aggregation_id(int unit,
                                            int pipe);
extern int bcm_td2p_aggregation_id_list_detach(int unit);
extern int bcm_td2p_aggid_trunk_map_detach(int unit);
extern int bcm_td2p_aggregation_id_list_init(int unit);
extern int bcm_td2p_aggid_trunk_map_init(int unit);

extern soc_profile_mem_t *egr_pri_cng_profile[BCM_MAX_NUM_UNITS];

/* Stat functions */
extern int bcmi_td2p_stat_port_attach(int unit, bcm_port_t port);
extern int bcmi_td2p_stat_port_detach(int unit, bcm_port_t port);

/* Flex port */
extern int
bcm_td2p_flexport_pbmp_update(int unit, bcm_pbmp_t *pbmp);
#endif /* BCM_TRIDENT2PLUS_SUPPORT  */

#endif /* !_BCM_INT_TRIDENT2PLUS_H_ */
