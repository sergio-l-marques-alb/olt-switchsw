/*
 * $Id: switch.h,v 1.12 Broadcom SDK $
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
 * Common internal definitions for BCM switch module
 */

#ifndef _BCM_INT_SWITCH_H_
#define _BCM_INT_SWITCH_H_

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/control.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/defs.h>
extern char warmboot_api_function_name[SOC_MAX_NUM_DEVICES][100];
#endif

extern int _bcm_switch_state_sync(int unit, bcm_dtype_t dtype);

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif /* CRASH_RECOVERY_SUPPORT */

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_WARM_BOOT_API_TEST)

#include <soc/dcmn/dcmn_wb.h>
#if defined(CRASH_RECOVERY_SUPPORT)
#include <shared/bsl.h>
#include <stdio.h>
#include <setjmp.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/dcmn/dcmn_crash_recovery_test.h>
#endif

#endif /* BCM_PETRA_SUPPORT && BCM_WARM_BOOT_API_TEST */

/* _BCM_DPP_WARM_BOOT_API_TEST_SYNC contains a FUNCTION_NAME() print of the called BCM API,
 * so must be defined as a macro  */
#define BCM_STATE_SYNC(_u) \
    /* next line actually call sync only in autosync mode */ \
    _bcm_switch_state_sync(_u, dtype);

#else /* !BCM_WARM_BOOT_SUPPORT */
#define BCM_STATE_SYNC(_u)
#endif /* BCM_WARM_BOOT_SUPPORT */


#if defined (CRASH_RECOVERY_SUPPORT) && !defined(BCM_WARM_BOOT_API_TEST) 
#define BCM_CR_TRANSACTION_START(unit)\
    if (BCM_UNIT_VALID(unit) && SOC_CR_ENABALED(unit) && soc_dcmn_cr_is_journaling_per_api(unit)) {SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_transaction_start(unit));}
#define BCM_CR_TRANSACTION_END(unit)\
    if (BCM_UNIT_VALID(unit) && SOC_CR_ENABALED(unit) && soc_dcmn_cr_is_journaling_per_api(unit)) {SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_dispatcher_commit(unit));}

#elif defined (CRASH_RECOVERY_SUPPORT) && defined (BCM_WARM_BOOT_API_TEST)
#define BCM_CR_TRANSACTION_START(unit)\
    BCM_CR_TEST_TRANSACTION_START(unit)

/* the API logic comes between BCM_CR_TRANSACTION_START and BCM_CR_TRANSACTION_END*/

#ifdef BCM_WARM_BOOT_API_TEST
#define BCM_CR_TRANSACTION_END(unit) \
    dcmn_wb_api_nesting_dec(unit);\
    sal_strcpy(warmboot_api_function_name[unit],__FUNCTION__); \
    BCM_CR_TEST_TRANSACTION_END(unit)
#else
#define BCM_CR_TRANSACTION_END(unit) \
    dcmn_wb_api_nesting_dec(unit);\
    BCM_CR_TEST_TRANSACTION_END(unit)
#endif

#else /* !CRASH_RECOVERY_SUPPORT */
#define BCM_CR_TRANSACTION_START(unit)
#define BCM_CR_TRANSACTION_END(unit)
#endif

#endif /* _BCM_INT_SWITCH_H_ */
