/*
 * $Id: failover.h,v 1.12 Broadcom SDK $
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
 * Module: Failover APIs
 *
 * Purpose:
 *     Failover API for Dune Soc_petra using PPD
 */
#ifndef _BCM_INT_DPP_FAILOVER_H_
#define _BCM_INT_DPP_FAILOVER_H_

#include <bcm/types.h>
#include <bcm/failover.h>


/* Failover types to be used in order to distinguish between failover object types */
#define DPP_FAILOVER_TYPE_NONE              (0)     /* No Failover type */
#define DPP_FAILOVER_TYPE_L2_LOOKUP         (1)     /* L2 lookup (fast flush) FEC ID */
#define DPP_FAILOVER_TYPE_FEC               (2)     /* Failover ID of type FEC (1:1 UC) */
#define DPP_FAILOVER_TYPE_INGRESS           (3)     /* Failover ID of type Ingress (1+1) */
#define DPP_FAILOVER_TYPE_ENCAP             (4)     /* Failover ID of type Egress (1:1 MC) */

#define DPP_FAILOVER_TYPE_SHIFT             (29)
#define DPP_FAILOVER_VAL_SHIFT              (0)
#define DPP_FAILOVER_TYPE_MASK              (0x7)
#define DPP_FAILOVER_VAL_MASK               (0x1FFFFFFF)

#define DPP_FAILOVER_TYPE_GET(failover_type, failover_id)                               \
        ((failover_type) = (((failover_id) >> DPP_FAILOVER_TYPE_SHIFT) &                \
            DPP_FAILOVER_TYPE_MASK))                  \

#define DPP_FAILOVER_IS_L2_LOOKUP(failover_id)                                          \
        (((((failover_id) >> DPP_FAILOVER_TYPE_SHIFT) & DPP_FAILOVER_TYPE_MASK) ==      \
                DPP_FAILOVER_TYPE_L2_LOOKUP))

#define DPP_FAILOVER_L2_LOOKUP_SET(failover_id, object_id)                              \
        ((failover_id) = ((DPP_FAILOVER_TYPE_L2_LOOKUP << DPP_FAILOVER_TYPE_SHIFT) |    \
         (((object_id) & DPP_FAILOVER_VAL_MASK) << DPP_FAILOVER_VAL_SHIFT)))

#define DPP_FAILOVER_TYPE_SET(object_id, failover_id, failover_type)                    \
        ((object_id) = (((failover_type) << DPP_FAILOVER_TYPE_SHIFT) |                  \
         (((failover_id) & DPP_FAILOVER_VAL_MASK) << DPP_FAILOVER_VAL_SHIFT)))

#define DPP_FAILOVER_TYPE_RESET(object_id, failover_id)                                 \
        ((object_id) = ((DPP_FAILOVER_TYPE_NONE << DPP_FAILOVER_TYPE_SHIFT) |           \
         (((failover_id) & DPP_FAILOVER_VAL_MASK) << DPP_FAILOVER_VAL_SHIFT)))

#define DPP_FAILOVER_ID_GET(failover_id_val, failover_id)                               \
        ((failover_id_val) = (((failover_id) >> DPP_FAILOVER_VAL_SHIFT) &               \
            DPP_FAILOVER_VAL_MASK))

typedef struct bcm_dpp_failover_info_s {
    uint8 id_sequence[SOC_DPP_DEFS_MAX(NOF_FAILOVER_EGRESS_IDS)];
} bcm_dpp_failover_info_t;

int _bcm_dpp_failover_is_valid_id(
    int unit,
    int32 failover_id,
    int32 failover_type_match);

#endif /* _BCM_INT_DPP_FAILOVER_H_ */


