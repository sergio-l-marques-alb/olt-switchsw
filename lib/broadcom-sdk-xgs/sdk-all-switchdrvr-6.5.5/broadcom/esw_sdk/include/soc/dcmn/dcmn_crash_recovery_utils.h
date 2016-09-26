/*
 * $Id: $
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
 * INFO: this is a utility module that is shared between the other Crash Recovery modules.
 * 
 */

#ifndef _SOC_DCMN_CRASH_RECOVERY_UTILITY_H
#define _SOC_DCMN_CRASH_RECOVERY_UTILITY_H

#include <soc/types.h>

/*#define DRCM_CR_MULTIPLE_THREAD_JOURNALING 1*/

#ifdef CRASH_RECOVERY_SUPPORT 

#define DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit) \
if (!soc_dcmn_cr_utils_is_journaling_thread(unit)) {SOC_EXIT;}

#define DCMN_CR_EXIT_IF_NOT_MAIN_THREAD \
if (sal_thread_self() != sal_thread_main_get()) {SOC_EXIT;}

typedef enum
{
    dcmn_cr_no_support_invalid = 0,
    dcmn_cr_no_support_unknown,
    dcmn_cr_no_support_not_in_api,
    dcmn_cr_no_support_legacy_wb,
    dcmn_cr_no_support_em_tables,
    dcmn_cr_no_support_tcam_tables,
    dcmn_cr_no_support_dma,
    dcmn_cr_no_support_kaps_kbp,
    dcmn_cr_no_support_count,
    dcmn_cr_no_support_wide_mem,
    dcmn_cr_no_support_portmod,
    dcmn_cr_no_support_sw_state_alloc
} dcmn_cr_no_support;

typedef enum
{
    DCMN_TRANSACTION_MODE_INVALID = 0,
    DCMN_TRANSACTION_MODE_IDLE,
    DCMN_TRANSACTION_MODE_LOGGING,
    DCMN_TRANSACTION_MODE_COMMITTING,
    DCMN_TRANSACTION_MODE_COUNT
} DCMN_TRANSACTION_MODE;

typedef struct soc_dcmn_cr_s {
    DCMN_TRANSACTION_MODE  transaction_mode;
    int                    is_recovarable;
    int                    transaction_status;
    dcmn_cr_no_support     not_recoverable_reason;
    uint32                 nested_api_cnt; /* nested api counter */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
    uint8                  kbp_dirty;
    uint8                  kaps_dirty;
    uint32                 kbp_tbl_id;
    uint32                 kaps_tbl_id;
#endif
	
    uint8                  perform_commit;
	
} soc_dcmn_cr_t;

typedef struct soc_dcmn_cr_utils_s {
    sal_thread_t tid;        /* transaction thread id */
    uint8        is_logging; /* is journaling flag */
} soc_dcmmn_cr_utils_t;

/* define one cr utility structure instance per unit */
soc_dcmmn_cr_utils_t dcmn_cr_utils[SOC_MAX_NUM_DEVICES];

extern uint8 soc_dcmn_cr_utils_is_journaling_thread(int unit);
extern int soc_dcmn_cr_utils_journaling_thread_set(int unit);

extern uint8 soc_dcmn_cr_utils_is_logging(int unit);
extern int soc_dcmn_cr_utils_logging_start(int unit);
extern int soc_dcmn_cr_utils_logging_stop(int unit);

extern int soc_dcmn_cr_api_counter_increase(int unit);
extern int soc_dcmn_cr_api_counter_decrease(int unit);
extern int soc_dcmn_cr_api_counter_count_get(int unit);
extern int soc_dcmn_cr_api_counter_reset(int unit);
extern uint8 soc_dcmn_cr_api_is_top_level(int unit);

#endif /*CRASH_RECOVERY_SUPPORT*/
#endif  /* _SOC_DCMN_CRASH_RECOVERY_UTILITY_H */
