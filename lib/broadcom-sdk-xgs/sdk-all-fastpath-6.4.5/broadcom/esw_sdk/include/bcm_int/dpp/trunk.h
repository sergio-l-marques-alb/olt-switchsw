/*
 * $Id: trunk.h,v 1.8 Broadcom SDK $
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

#ifndef _BCM_INT_DPP_TRUNK_H_
#define _BCM_INT_DPP_TRUNK_H_

#include <sal/types.h>
#include <sal/core/sync.h>
#include <bcm/types.h>

#include <soc/dpp/PPD/ppd_api_lag.h>

#include <bcm_int/dpp/stack.h>
#include <shared/swstate/sw_state.h>

/* Max Trunk ID */
#define BCM_DPP_MAX_TRUNKS      (SOC_PPD_LAG_MAX_ID)
#define BCM_DPP_MAX_STK_TRUNKS      (SOC_TMC_NOF_TM_DOMAIN_IN_SYSTEM)

/* Check valid Trunk ID */
#define BCM_DPP_TRUNK_VALID(tid) (((tid) >= 0) && ((tid) < BCM_DPP_MAX_TRUNKS))


typedef struct bcm_dpp_trunk_private_s {
    bcm_trunk_t     trunk_id;       /* trunk group ID */
    int             in_use;         /* whether particular trunk is in use */
    int             nof_members;    /* number of members in trunk */
    int             is_stateful;    /* indicates if member is stateful - relevant for arad+ and higher - relevant for network trunk */
    int             psc;            /* indicates the port selection criteria - relevant for network trunk */
} bcm_dpp_trunk_private_t;

typedef enum trunk_init_state_e {
    ts_none,
    ts_recovering, /* phase 1 recovered, second phase recovery required to
                    * complete init sequence */
    ts_init        /* completely initialized */
} trunk_init_state_t;

typedef struct bcm_dpp_trunk_member_s {
    bcm_gport_t         system_port;    /* destination */
    uint32              flags;          /* member flags */
    uint32              member_id;      /* member id in trunk */
    uint32              trunk_id;       /* trunk id */
} bcm_dpp_trunk_member_t;

typedef struct trunk_state_s {
    trunk_init_state_t                          init_state;     /* Initialized, recovering, or none */
    int                                         ngroups;        /* max number of trunk groups */
    int                                         stk_ngroups;    /* max number of trunk groups */
    int                                         nports;         /* max port count per trunk group */
    PARSER_HINT_ARR bcm_dpp_trunk_private_t     *t_info;
    PARSER_HINT_ARR bcm_dpp_trunk_member_t      *trunk_members; /* should contain max amount of members possible (16K) */
} trunk_state_t;

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_trunk_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
 	 

#endif /* _BCM_INT_DPP_TRUNK_H_ */

