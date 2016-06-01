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
    if (!BCM_UNIT_VALID(unit)) { \
            LOG_ERROR(SOC_E_UNIT,\
             (BSL_META_U(unit, "Unit:%d invalid"), unit)); \
                    return SOC_E_UNIT; \
    } \
    if (SOC_CR_ENABALED(unit)) {SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_transaction_start(unit));}
#define BCM_CR_TRANSACTION_END(unit)\
    if (SOC_CR_ENABALED(unit)) {SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_dispatcher_commit(unit));}

#elif defined (CRASH_RECOVERY_SUPPORT) && defined (BCM_WARM_BOOT_API_TEST)

 /* get the number of iterations
  * if the crash recovery conditions hold, i.e.
  * 1) the current thread is the main thread
  * 2) the test mode is crash recovery mode
  * 3) exponential backoff hit
  * then, the number of iterations get the value 2
  * we first want to peform "crash" (tr141) before
  * the flag DCMN_TRANSACTION_MODE_COMMITTING is being raised
  * this one is for checking the sw state journal roll back mechanism
  * and to perform the second crash after this flag
  * is being raised but before we perform commit
  * this one is for checking the hwlog write ahead mechanism
  * in any other case we don't start the crash simulation
  */
#define BCM_CR_TRANSACTION_START(unit) \
    if (!BCM_UNIT_VALID(unit)) { \
            LOG_ERROR(SOC_E_UNIT,\
             (BSL_META_U(unit, "Unit:%d invalid"), unit)); \
                    return SOC_E_UNIT; \
    } \
    do { \
        /* allocated i in the stack and not in register */ \
        auto int i = 0; \
        int num_of_iterations = soc_dcmn_cr_get_num_of_iterations(unit); \
        SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_transaction_start(unit)); \
        /* if we are in API crash recovery test mode and the abovementioned conditions hold */ \
        if (num_of_iterations == 2) { \
           /* setjmp saves a "snapshot" of the execution inside a dedicated struct \
            * and afterwards longjmp recapture this "snapshot" and the run \
            * continues from this point. \
            * In our case we save the snapshot before the logic of the api start. \
            * The snapshot is saved in the struct dcmn_cr_buf. \
            * In the next step we peform the logic of the api. Afterwards we perform the \
            * first crash and the first iteration of the while loop ends. \
            * In the second iteration, we perform the logic again, and afterwards \
            * we call the commit function, and from there we perform the second crash \
            * After the second crash we want to continue the flow of the execution \
            * from this point. \
            * So, we then call the longjmp function (actually it's a macro) from \
            * the commit function (after the crash that has been done after we \
            * raise the flag DCMN_TRANSACTION_MODE_COMMITTING) and we \
            * return to this point of execution  \
            */ \
             cli_out("%d: Dumping sw_state Data Block to file /dev/shm/warmboot_data_beginning_of_API.txt\n", unit); \
             shr_sw_state_block_dump(unit, "/dev/shm/warmboot_data_beginning_of_API.txt", "w"); \
             LOG_VERBOSE(BSL_LS_BCM_COMMON, \
                 (BSL_META_U(unit, "Unit:%d CR: setjmp dcmn_cr_buf[%d] to file: %s line: %d\n"), unit, unit, __FILE__, __LINE__)); \
             setjmp(dcmn_cr_buf[unit]); \
        } \
        while (i++ < num_of_iterations) { \
           int trans_ret_val = 0; \
           if (num_of_iterations == 2) { \
               LOG_VERBOSE(BSL_LS_BCM_COMMON, \
                   (BSL_META_U(unit, "Unit:%d CR: API iteration %d of %d\n"), unit, i, num_of_iterations)); \
           }

/* the API logic comes between BCM_CR_TRANSACTION_START and BCM_CR_TRANSACTION_END*/

#define BCM_CR_TRANSACTION_END(unit) \
           /* here we are still in the while loop */ \
           /* update num_of_iterations in case of the reset conditions have been changed */ \
           if (i == 1) num_of_iterations = soc_dcmn_cr_get_num_of_iterations(unit); \
           if (num_of_iterations == 2) { \
                /* in the first iteration soc_dcmn_cr_transaction_end calls direcly \
                   the api test reset (which calls tr 141). \
                   In the second iteration, soc_dcmn_cr_transaction_end calls \
                   the commit fuction which calls the api test reset after raising \
                   the abovementioned flag. \
                */ \
                trans_ret_val = soc_dcmn_cr_transaction_end(unit, i /* iteration number */); \
                /* the following statements are occured only in the first iteration */ \
                if (trans_ret_val == -1 /* tr 141 failed */) { \
                    LOG_ERROR(SOC_E_FAIL, \
                       (BSL_META_U(unit, "Unit:%d CR: crash recovery simulation failed (first tr 141)"), unit)); \
                    return SOC_E_FAIL; \
                 } \
            /* regular transaction - no crash recovery simulation */ \
            } else { /* num_of_iterations == 1 */ \
                SOC_WB_API_RESET_ERR_CHECK(soc_dcmn_cr_transaction_end_regular_reset(unit)); \
                SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_dispatcher_commit(unit)); \
            } \
        } \
    } \
    while(0)

#else /* !CRASH_RECOVERY_SUPPORT */
#define BCM_CR_TRANSACTION_START(unit)
#define BCM_CR_TRANSACTION_END(unit)
#endif

#endif /* _BCM_INT_SWITCH_H_ */
