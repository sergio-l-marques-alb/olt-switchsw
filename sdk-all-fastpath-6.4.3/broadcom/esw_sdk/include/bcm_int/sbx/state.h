/*
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$Id: state.h,v 1.13 Broadcom SDK $
 * All Rights Reserved.$
 *
 * File:        state.h
 * Purpose:     Single point of reference for all BCM state of a unit.
 */

#ifndef	_BCM_INT_SBX_STATE_H_
#define	_BCM_INT_SBX_STATE_H_
#ifdef BCM_WARM_BOOT_SUPPORT
#include "soc/types.h"
#endif /* BCM_WARM_BOOT_SUPPORT */

typedef struct bcm_sbx_state_s {
    struct bcm_sbx_cosq_discard_state_s *discard_state;
    struct bcm_sbx_fabric_state_s *fabric_state;
    struct bcm_sbx_stack_state_s *stack_state;
    struct bcm_sbx_cosq_queue_state_s *queue_state;       /* pointer to array of queue_state_t */
    struct bcm_sbx_cosq_bw_group_state_s *bw_group_state; /* pointer to array of bw_group_state_t */
    struct bcm_sbx_cosq_sysport_state_s *sysport_state;   /* pointer to array of sysport_state_t */
    struct bcm_sbx_cosq_destport_state_s *destport_state; /* pointer to array of destport_state_t */
    struct bcm_sbx_cosq_sysport_group_state_s *sysport_group_state; /* pointer to array of sysport_group_state_t */
    struct bcm_sbx_cosq_ingress_scheduler_state_s *ingress_scheduler_state; /* pointer to array of ingress_scheduler_state_t */
    struct bcm_sbx_cosq_egress_scheduler_state_s *egress_scheduler_state; /* pointer to array of egress_scheduler_state_t */
    struct bcm_sbx_cosq_egress_group_state_s *egress_group_state; /* pointer to array of egress_group_state_t */
    struct bcm_sbx_cosq_ingress_multipath_state_s *ingress_multipath_state; /* pointer to array of ingress_multipath_state_t */
    struct bcm_sbx_cosq_egress_multipath_state_s *egress_multipath_state; /* pointer to array of egress_multipath_state_t */
    struct bcm_sbx_failover_object_s *failover_state;     /* pointer to array of failover_state_t */
    struct bcm_sbx_port_state_s *port_state;              /* pointer to array of port_state_t */
    struct bcm_sbx_cosq_fcd_state_s *fcd_state;           /* pointer to array of fcd_state_t */
#ifdef BCM_WARM_BOOT_SUPPORT
    uint8 *cosq_index;                                    /* current location in scache */
    uint32 cache_cosq; /* if set cache cosq changes (do not commit changes to hardware) */
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_EASY_RELOAD_SUPPORT
    struct bcm_sbx_cosq_bw_group_state_cache_s *bw_group_state_cache; /* pointer to array of bw_group_state_cache_t */
#endif /* BCM_EASY_RELOAD_SUPPORT */
    uint32 l2_age_delete; /* if set, l2_age will delete MAC entries */
    uint32 cache_l2; /* if set cache l2 changes (do not commit changes to hardware) */
    uint32 cache_ipmc; /* if set cache ipmc changes (do not commit changes to hardware) */
    uint32 cache_l3host; /* if set cache l3host changes (do not commit changes to hardware) */
    uint32 cache_l3route; /* if set cache l3route changes (do not commit changes to hardware) */
    uint32 cache_field; /* if set cache field changes (do not commit changes to hardware) */
} bcm_sbx_state_t;

#endif	/* _BCM_INT_SBX_STATE_H_ */
