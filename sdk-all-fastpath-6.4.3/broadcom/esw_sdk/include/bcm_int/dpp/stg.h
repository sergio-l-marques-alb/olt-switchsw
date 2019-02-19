/*
 * $Id: stg.h,v 1.6 Broadcom SDK $
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
 * IPMC Internal header
 */

#ifndef _BCM_INT_DPP_STG_H_
#define _BCM_INT_DPP_STG_H_

#include <sal/types.h>
#include <bcm/types.h>

/*
 * The STG info structure is protected by STG_DB_LOCK. The hardware PTABLE and
 * hardware STG table are protected by memory locks in the lower level.
 */
typedef struct bcm_stg_info_s {
    int         init;       /* TRUE if STG module has been initialized */
    sal_mutex_t lock;       /* Database lock */
    bcm_stg_t   stg_min;    /* STG table min index */
    bcm_stg_t   stg_max;    /* STG table max index */
    bcm_stg_t   stg_defl;   /* Default STG */
    SHR_BITDCL *stg_bitmap; /* Bitmap of allocated STGs */
    bcm_pbmp_t *stg_enable; /* array of port bitmaps indicating whether the
                               port+stg has STP enabled */
    bcm_pbmp_t *stg_state_h;/* array of port bitmaps indicating STP state for the */
    bcm_pbmp_t *stg_state_l;/* port+stg combo. Only valid if stg_enable = TRUE */
    int         stg_count;  /* Number STGs allocated */
    /* STG reverse map - keep a linked list of VLANs in each STG */
    bcm_vlan_t *vlan_first; /* Indexed by STG (also links free list) */
    bcm_vlan_t *vlan_next;  /* Indexed by VLAN ID */
} bcm_stg_info_t;

/* Convert STG to PPD stg topology id */
#define BCM_DPP_STG_TO_TOPOLOGY_ID(stg) \
            (stg - 1)

/* Convert PPD stg topology id to STG */
#define BCM_DPP_STG_FROM_TOPOLOGY_ID(topology_id) \
            (topology_id + 1)

extern bcm_stg_info_t  stg_info[BCM_MAX_NUM_UNITS];

extern int _bcm_petra_stg_vlan_destroy(int unit, bcm_stg_t stg, bcm_vlan_t vid);

extern int bcm_petra_stg_detach(int unit);

#endif /* _BCM_INT_DPP_IPMC_H_ */

