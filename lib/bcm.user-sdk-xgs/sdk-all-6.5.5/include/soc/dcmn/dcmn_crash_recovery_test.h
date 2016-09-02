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

#ifndef _SOC_DCMN_CRASH_RECOVERY_TEST_H
#define _SOC_DCMN_CRASH_RECOVERY_TEST_H

#if defined (CRASH_RECOVERY_SUPPORT) && defined (BCM_WARM_BOOT_API_TEST)
#include <setjmp.h>
#include <soc/dcmn/dcmn_crash_recovery_utils.h>

#ifdef SW_STATE_DIFF_DEBUG
#define CR_DEBUG(x) x
#else
#define CR_DEBUG(x)
#endif

void dcmn_cr_test_signal_before_prepare_for_commit_set(int unit);
void dcmn_cr_test_signal_after_prepare_for_commit_set(int unit);
int dcmn_cr_test_signal_before_prepare_for_commit_get(int unit);
int dcmn_cr_test_signal_after_prepare_for_commit_get(int unit);
int soc_dcmn_cr_test_transaction_end(int unit, int reset_num);
int soc_dcmn_cr_test_transaction_end_regular_reset(int unit);
int soc_dcmn_cr_test_get_num_of_iterations(int unit);

extern jmp_buf dcmn_cr_test_buf[SOC_MAX_NUM_DEVICES];

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
#define BCM_CR_TEST_TRANSACTION_START(unit) \
    r_rv = BCM_E_NONE; /* initialization of the api returned value in order to aviod build error with optimization */\
    do { \
        /* allocated i in the stack and not in register */ \
        volatile int i = 0; \
        /* If there is an API inside API, then we want to relate to the inner API as if it is just a function. \
         * One of the reasons is that if such case exists, then the inner API changes the jumping buffer \
         * and when we want to jump back to the outer API we find ourself in the inner API. \
         * Hence, in this situation, we set the num_of_iterations to be 1 and we skip the transaction start \
         * and transaction end commands (only perform the API logic). \
         */ \
        volatile int inner_API = (BCM_UNIT_VALID(unit) && SOC_CR_ENABALED(unit)) ? soc_dcmn_cr_api_counter_count_get(unit) > 0 : 0; \
        volatile int num_of_iterations = soc_dcmn_cr_test_get_num_of_iterations(unit); \
        /* if we are in API crash recovery test mode and the abovementioned conditions hold */ \
        if (!inner_API && num_of_iterations == 2) { \
           /* setjmp saves a "snapshot" of the execution inside a dedicated struct \
            * and afterwards longjmp recapture this "snapshot" and the run \
            * continues from this point. \
            * In our case we save the snapshot before the logic of the api start. \
            * The snapshot is saved in the struct dcmn_cr_test_buf. \
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
             CR_DEBUG(cli_out("%d: Dumping sw_state Data Block to file /dev/shm/warmboot_data_beginning_of_API.txt\n", unit)); \
             CR_DEBUG(shr_sw_state_block_dump(unit, SW_STATE_EFFICIENT_DUMP, "/dev/shm/warmboot_data_beginning_of_API.txt", "w")); \
             LOG_VERBOSE(BSL_LS_BCM_COMMON, \
                 (BSL_META_U(unit, "Unit:%d CR: setjmp dcmn_cr_test_buf[%d] to file: %s line: %d\n"), unit, unit, __FILE__, __LINE__)); \
             setjmp(dcmn_cr_test_buf[unit]); \
        } \
        if (!BCM_UNIT_VALID(unit) || inner_API) num_of_iterations = 1; \
        while (i++ < num_of_iterations) { \
           int trans_ret_val = 0; \
           dcmn_wb_api_nesting_inc(unit);\
           if (BCM_UNIT_VALID(unit) && !inner_API) SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_transaction_start(unit)); \
           if (num_of_iterations == 2) { \
               LOG_VERBOSE(BSL_LS_BCM_COMMON, \
                   (BSL_META_U(unit, "Unit:%d CR: API iteration %d of %d\n"), unit, i, num_of_iterations)); \
           } \
/* the API logic comes between BCM_CR_TRANSACTION_START and BCM_CR_TRANSACTION_END*/

#define BCM_CR_TEST_TRANSACTION_END(unit) \
           /* here we are still in the while loop */ \
           /* update num_of_iterations in case of the reset conditions have been changed */ \
           if (BCM_UNIT_VALID(unit) && !inner_API && i == 1) num_of_iterations = soc_dcmn_cr_test_get_num_of_iterations(unit); \
           if (num_of_iterations == 2) { \
                /* in the first iteration soc_dcmn_cr_test_transaction_end calls direcly \
                   the api test reset (which calls tr 141). \
                   In the second iteration, soc_dcmn_cr_test_transaction_end calls \
                   the commit fuction which calls the api test reset after raising \
                   the abovementioned flag. \
                */ \
                trans_ret_val = soc_dcmn_cr_test_transaction_end(unit, i /* iteration number */); \
                /* the following statements are occured only in the first iteration */ \
                if (trans_ret_val == -1 /* tr 141 failed */) { \
                    LOG_ERROR(SOC_E_FAIL, \
                       (BSL_META_U(unit, "Unit:%d CR: crash recovery simulation failed (first tr 141)"), unit)); \
                    return SOC_E_FAIL; \
                 } \
            /* regular transaction - no crash recovery simulation */ \
            } else if (BCM_UNIT_VALID(unit) && !inner_API) { /* num_of_iterations == 1 */ \
                SOC_WB_API_RESET_ERR_CHECK(soc_dcmn_cr_test_transaction_end_regular_reset(unit)); \
                SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_dispatcher_commit(unit)); \
            } \
        } \
    } \
    while(0)

#endif /* CRASH_RECOVERY_SUPPORT && BCM_WARM_BOOT_API_TEST */
#endif  /* _SOC_DCMN_CRASH_RECOVERY_TEST_H */
