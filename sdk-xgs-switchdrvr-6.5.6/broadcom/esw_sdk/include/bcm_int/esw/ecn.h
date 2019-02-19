/*
 * $Id: ecn.h,v 1.6 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * ECN - Broadcom StrataSwitch ECN API internal definitions 
 */

#ifndef _BCM_INT_ECN_H
#define _BCM_INT_ECN_H

/* ECN max value */
#define _BCM_ECN_VALUE_MAX               (3)

/* Max Internal Congestion value */
#define _BCM_ECN_INT_CN_VALUE_MAX        (3)

/* Action dequeue memory index */
#define _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET          (0x5)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CE_GREEN_OFFSET        (0x4)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CE_YELLOW_OFFSET       (0x3)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CE_RED_OFFSET          (0x2)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET             (0x0)

/* Action egress memory index*/
#define _BCM_ECN_EGRESS_MEM_INDEX_INT_CN_OFFSET           (0x2)
#define _BCM_ECN_EGRESS_MEM_INDEX_ECN_OFFSET              (0x0)

/* Internal Congestion Action Values */
#define _BCM_ECN_INT_CN_RESPONSIVE_DROP                   (0x0)
#define _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP               (0x1)
#define _BCM_ECN_INT_CN_NON_CONGESTION_MARK_ECN           (0x2)
#define _BCM_ECN_INT_CN_CONGESTION_MARK_ECN               (0x3)

/* ECN */
extern int
_bcm_esw_ecn_init(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_esw_ecn_sw_dump(int unit);
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
extern int
bcmi_xgs5_ecn_scache_size_get(int unit, uint32 *scache_size);
 
#ifdef BCM_WARM_BOOT_SUPPORT
extern int 
bcmi_xgs5_ecn_sync(int unit, uint8 **scache_ptr);
extern int
bcmi_xgs5_ecn_reinit(int unit, uint8 **scache_ptr); 
#endif /* BCM_WARM_BOOT_SUPPORT */

#define _BCM_XGS5_MPLS_ECN_MAP_TYPE_MASK         0xC00
#define _BCM_XGS5_MPLS_ECN_MAP_TYPE_EXP2ECN      0x400
#define _BCM_XGS5_MPLS_ECN_MAP_TYPE_ECN2EXP      0x800
#define _BCM_XGS5_MPLS_ECN_MAP_TYPE_INTCN2EXP    0xC00
#define _BCM_XGS5_MPLS_ECN_MAP_NUM_MASK          0x0ff

#define _BCM_ECN_MAX_ENTRIES_PER_MAP             0x20

#define _BCM_XGS5_ECN_MAP_TYPE_MASK               0xC000
#define _BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM        0x4000
#define _BCM_XGS5_ECN_MAP_NUM_MASK                0x0ff
#define _BCM_ECN_MAX_ENTRIES_PER_TUNNEL_TERM_MAP  0x10

typedef enum _bcm_ecn_map_type_s {
    _bcmEcnmapTypeExp2Ecn,      /* Ingress_EXP_TO_ECN map */
    _bcmEcnmapTypeIpEcn2Exp,    /* Egress_IP_ECN_TO_EXP map */
    _bcmEcnmapTypeIntcn2Exp,    /* Egress_INT_CN_TO_EXP map */
    _bcmEcnmapTypeTunnelTerm   /* Ingress Tunnel termination ecn map */
} _bcm_ecn_map_type_e;
extern int
bcmi_xgs5_ecn_map_used_get(int unit, int map_index, _bcm_ecn_map_type_e type);
extern int
bcmi_ecn_map_id2hw_idx (int unit,int ecn_map_id,int *hw_idx);
extern int
bcmi_ecn_map_hw_idx2id (int unit, int hw_idx, int type, int *ecn_map_id);
extern void bcmi_ecn_map_clean_up(int unit);
#endif

#endif /* _BCM_INT_ECN_H */
