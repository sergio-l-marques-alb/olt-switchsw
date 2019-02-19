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
 * INFO: this module is the entry poit for Crash Recovery feature
 * 
 */

#ifndef _SOC_DCMN_CRASH_RECOVERY_H
#define _SOC_DCMN_CRASH_RECOVERY_H

#include <soc/types.h>
#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/error.h>
#include <soc/drv.h>
#endif

#include <shared/swstate/sw_state.h>
#ifdef CRASH_RECOVERY_SUPPORT
#if defined(__KERNEL__) || !defined (LINUX)
#error "cmpl flags err: CRASH_RECOVERY_SUPPORT cannot be used w/o LINUX or with __KERNEL__"
#endif
#ifndef BCM_WARM_BOOT_SUPPORT
#error "cmpl flags err: CRASH_RECOVERY_SUPPORT cannot be used w/o BCM_WARM_BOOT_SUPPORT"
#endif
#endif


typedef enum
{
    DCMN_TRANSACTION_MODE_INVALID = 0,
    DCMN_TRANSACTION_MODE_IDLE,
    DCMN_TRANSACTION_MODE_LOGGING,
    DCMN_TRANSACTION_MODE_COMMITTING,
    DCMN_TRANSACTION_MODE_COUNT
} DCMN_TRANSACTION_MODE;

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
    dcmn_cr_no_support_portmod
} dcmn_cr_no_support;

typedef struct soc_dcmn_cr_s {
    PARSER_HINT_ALLOW_WB_ACCESS  DCMN_TRANSACTION_MODE  transaction_mode;
    PARSER_HINT_ALLOW_WB_ACCESS  int                    is_recovarable;
    PARSER_HINT_ALLOW_WB_ACCESS  int                    transaction_status;
    PARSER_HINT_ALLOW_WB_ACCESS  dcmn_cr_no_support     not_recoverable_reason;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
    PARSER_HINT_ALLOW_WB_ACCESS  uint8                  kbp_dirty;
	PARSER_HINT_ALLOW_WB_ACCESS  uint8   			   kaps_dirty;
	PARSER_HINT_ALLOW_WB_ACCESS  uint32  			   kbp_tbl_id;
	PARSER_HINT_ALLOW_WB_ACCESS  uint32  			   kaps_tbl_id;
#endif
} soc_dcmn_cr_t;

#define DCMN_CR_EXIT_IF_NOT_MAIN_THREAD \
if (sal_thread_self() != sal_thread_main_get()) {SOC_EXIT;}

#ifdef CRASH_RECOVERY_SUPPORT

#define SOC_CR_DISP_ERR_CHECK(disp_rv)\
    if (disp_rv != SOC_E_NONE) {return SOC_E_INTERNAL;}

/* 
 * used to allocate/retrieve shared memory\ 
 * use str as a unique identifier for the memory segment 
 * use flags to indicate if the memory should be allocated or fetched 
 * returns a pointer to the shared memory. 
 * return NULL on failure.* 
 */
typedef void* (*dcmn_ha_mem_get)(unsigned int size, char *str, int flags);

/* 
 * used to free shared memory
 */
typedef void* (*dcmn_ha_mem_release)(unsigned int size, char *str);

int soc_dcmn_cr_transaction_start(int unit);
int soc_dcmn_cr_init(int unit);
int soc_dcmn_cr_commit(int unit);
int soc_dcmn_cr_dispatcher_commit(int unit);
int soc_dcmn_cr_abort(int unit);
int soc_dcmn_cr_recover(int unit);
int soc_dcmn_cr_suppress(int unit, dcmn_cr_no_support reason);
int soc_dcmn_cr_unsuppress(int unit);

#ifdef BCM_WARM_BOOT_API_TEST
void dcmn_cr_signal_before_prepare_for_commit_set(int unit);
void dcmn_cr_signal_after_prepare_for_commit_set(int unit);
int dcmn_cr_signal_before_prepare_for_commit_get(int unit);
int dcmn_cr_signal_after_prepare_for_commit_get(int unit);
int soc_dcmn_cr_transaction_end(int unit, int reset_num);
int soc_dcmn_cr_transaction_end_regular_reset(int unit);
int soc_dcmn_cr_get_num_of_iterations(int unit);
#endif /* BCM_WARM_BOOT_API_TEST */

#endif /* CRASH_RECOVERY_SUPPORT */
#endif  /* _SOC_DCMN_CRASH_RECOVERY_H */

