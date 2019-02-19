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
 * File:        trident2plus.h
 * Purpose:     Function declarations for Trident2+ Internal functions.
 */

#ifndef _BCM_INT_TRIDENT2PLUS_H_
#define _BCM_INT_TRIDENT2PLUS_H_

#if defined(BCM_TRIDENT2PLUS_SUPPORT)

#include <bcm/failover.h>

/* Number of protection switching groups/next hops
 * represented per entry in the PROT_SW tables
 * (INITIAL_PROT_NHI_TABLE_1, TX_INITIAL_PROT_GROUP_TABLE,
 * RX_PROT_GROUP_TABLE, EGR_TX_PROT_GROUP_TABLE and
 * EGR_L3_NEXT_HOP_1)
 */
#define BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY 128
#define BCM_TD2P_MAX_COE_MODULES 5

#define BCM_TD2P_COE_MODULE_1 1
#define BCM_TD2P_COE_MODULE_2 2
#define BCM_TD2P_COE_MODULE_3 3
#define BCM_TD2P_COE_MODULE_4 4
#define BCM_TD2P_COE_MODULE_5 5


#ifdef INCLUDE_L3
extern int bcm_td2p_vp_group_init(int unit);
extern int bcm_td2p_vp_group_detach(int unit);
extern int bcm_td2p_vp_vlan_member_set(int unit, bcm_gport_t gport,
        uint32 flags);
extern int bcm_td2p_vp_vlan_member_get(int unit, bcm_gport_t gport,
        uint32 *flags);
extern int bcm_td2p_ing_vp_group_move(int unit, int vp, bcm_vlan_t vlan,
        int add);
extern int bcm_td2p_eg_vp_group_move(int unit, int vp, bcm_vlan_t vlan, 
        int add);
extern int bcm_td2p_ing_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_td2p_eg_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_td2p_ing_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);
extern int bcm_td2p_eg_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);
extern int bcm_td2p_egr_vp_group_unmanaged_set(int unit, int flag);
extern int bcm_td2p_ing_vp_group_unmanaged_set(int unit, int flag);
extern int bcm_td2p_ing_vp_group_unmanaged_get(int unit);
extern int bcm_td2p_egr_vp_group_unmanaged_get(int unit);
extern int bcm_td2p_vlan_vp_group_set(int unit, soc_mem_t vlan_mem, int vlan, 
                                    int vp_group,  int enable);
extern int bcm_td2p_vlan_vp_group_get(int unit, soc_mem_t vlan_mem, int vlan, 
                                    int vp_group,  int *enable);
extern int bcm_td2p_vlan_vp_group_get_all(int unit, int vlan, int array_max,
             bcm_gport_t *gport_array, int *flags_array, int *port_cnt);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_td2p_vp_group_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

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

#endif /* INCLUDE_L3 */

/* VLAN VFI */
extern int bcm_td2p_vlan_table_port_get(int unit, bcm_vlan_t vid,
        pbmp_t *ing_pbmp, pbmp_t *pbmp);
extern int bcm_td2p_update_vlan_pbmp(int unit, bcm_vlan_t vlan,
        bcm_pbmp_t *pbmp);
extern int bcm_td2p_vlan_vfi_membership_init(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);
extern int bcm_td2p_ing_vlan_vfi_membership_add(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);
extern int bcm_td2p_ing_vlan_vfi_membership_delete(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);
extern int bcm_td2p_egr_vlan_vfi_membership_add(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);
extern int bcm_td2p_egr_vlan_vfi_membership_delete(int unit, bcm_vlan_t vid,
        pbmp_t pbmp);

/* Subport functions */

extern int bcm_td2plus_subport_coe_init(int unit);



#endif /* BCM_TRIDENT2PLUS_SUPPORT  */

#endif /* !_BCM_INT_TRIDENT2PLUS_H_ */
