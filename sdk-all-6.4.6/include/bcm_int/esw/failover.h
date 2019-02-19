/*
 * $Id: failover.h,v 1.3 Broadcom SDK $
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
 * This file contains Failover module definitions internal to the BCM library.
 */
#ifndef _BCM_INT_ESW_FAILOVER_H_
#define _BCM_INT_ESW_FAILOVER_H_

#if defined(INCLUDE_L3)

#define _BCM_FAILOVER_1_PLUS_PROTECTION  0x1
#define _BCM_FAILOVER_DEFAULT_MODE  0x0
#define _BCM_FAILOVER_1_1_MC_PROTECTION_MODE  0x2
#define FAILOVER_TYPE_SHIFT 24
#define FAILOVER_TYPE_MASK  0xff000000 
#define FAILOVER_ID_MASK    0x00ffffff 

#define _BCM_ENCAP_TYPE_IN_FAILOVER_ID(failover_id, type) \
    (failover_id = ((type << FAILOVER_TYPE_SHIFT) | (failover_id)))  

#define _BCM_GET_FAILOVER_ID(failover_id) \
    failover_id = failover_id &  FAILOVER_ID_MASK  

#define _BCM_GET_FAILOVER_TYPE(failover_id, type) \
    type = ((failover_id & FAILOVER_TYPE_MASK) >> FAILOVER_TYPE_SHIFT)  



/*
 * Software book keeping for Failover related information
 */
typedef struct _bcm_failover_bookkeeping_s {
    int         initialized;        /* Set to TRUE when Failover module initialized */
    SHR_BITDCL  *prot_group_bitmap;       /* INITIAL_PROT_GROUP usage bitmap */
    SHR_BITDCL  *prot_nhi_bitmap;       /* INITIAL_PROT_NHI usage bitmap */
    SHR_BITDCL  *mmu_prot_group_bitmap;       /* MMU_PROT_GROUP usage bitmap */
    SHR_BITDCL  *egress_prot_group_bitmap;       /* EGRESS_PROT_GROUP usage bitmap */
    sal_mutex_t    failover_mutex;			  /* Protection mutex. */
} _bcm_failover_bookkeeping_t;

#define FAILOVER_INFO(_unit_)   (&_bcm_failover_bk_info[_unit_])

/*
 * INITIAL_PROT_GROUP  usage bitmap operations
 */
#define _BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_GROUP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_GROUP_MAP_IS_VALID(_u_) \
        (FAILOVER_INFO((_u_))->prot_group_bitmap != NULL)     

#define _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->mmu_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->mmu_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->mmu_prot_group_bitmap, (_map_))


/*
 * EGR_TX_PROT_GROUP  usage bitmap operations
 */
#define _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->egress_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->egress_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->egress_prot_group_bitmap, (_map_))

/*
 * INITIAL_PROT_NHI  usage bitmap operations
 */
#define _BCM_FAILOVER_PROT_NHI_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->prot_nhi_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_NHI_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->prot_nhi_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_NHI_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->prot_nhi_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_NHI_MAP_IS_VALID(_u_) \
        (FAILOVER_INFO((_u_))->prot_nhi_bitmap != NULL)        

extern _bcm_failover_bookkeeping_t  _bcm_failover_bk_info[BCM_MAX_NUM_UNITS]; 

extern int _bcm_esw_failover_id_check(int unit, bcm_failover_t failover_id);
extern int _bcm_esw_failover_egr_check(int unit, bcm_l3_egress_t  *egr);
extern int _bcm_esw_failover_prot_nhi_create(int unit, uint32 flags, int nh_index, int prot_nh_index,
                             bcm_multicast_t mc_group, bcm_failover_t failover_id);
extern int _bcm_esw_failover_prot_nhi_cleanup(int unit, int nh_index);
extern int _bcm_esw_failover_prot_nhi_get (int unit, int nh_index,
         bcm_failover_t *failover_id, int *prot_nh_index, int *multicast_group);
extern int _bcm_esw_failover_mpls_check(int unit, bcm_mpls_port_t  *mpls_port);
extern int _bcm_esw_failover_prot_nhi_update (int unit, int old_nh_index, int new_nh_index);
extern int _bcm_esw_failover_extender_check(int unit, bcm_extender_port_t *extender_port);
extern int _bcm_esw_failover_ecmp_prot_nhi_create(int unit, int ecmp, int index, 
                    int nh_index, bcm_failover_t failover_id, int prot_nh_index);
extern int _bcm_esw_failover_ecmp_prot_nhi_cleanup(int unit, int ecmp, int index, int nh_index);
extern int _bcm_esw_failover_ecmp_prot_nhi_get(int unit, int ecmp, int index, 
                    int nh_index, bcm_failover_t *failover_id, int *prot_nh_index);
extern int _bcm_esw_failover_ecmp_prot_nhi_update(int unit, int ecmp, int index, 
                    int old_nh_index, int new_nh_index);


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_tr2_failover_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
bcm_esw_failover_sync(int unit);

#endif
#endif /* INCLUDE_L3 */

#endif /* !_BCM_INT_ESW_FAILOVER_H_ */
