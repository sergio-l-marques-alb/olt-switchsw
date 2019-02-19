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
 * some design details:
 * - transaction starts at the beginning of an API and ends at the end of an API
 *
 */

#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>

#if defined(BCM_ARAD_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)

#include <soc/hwstate/hw_log.h>
#include <shared/swstate/sw_state_journal.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/dcmn/dcmn_crash_recovery_test.h>
#include <soc/dcmn/dcmn_wb.h>
#include <setjmp.h>
#include <bcm_int/control.h>
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern int diff_two_files(int unit, char *file_name1, char *file_name2);
extern sw_state_journal_t *sw_state_journal[SOC_MAX_NUM_DEVICES];
extern soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];

/* the buffer used by cr test - for more info search BCM_CR_TRANSACTION_START */
jmp_buf dcmn_cr_test_buf[SOC_MAX_NUM_DEVICES];
_dcmn_cr_stat_t dcmn_cr_test_crash_status[SOC_MAX_NUM_DEVICES];

/*
 * this function determines the number of iterations to perform the logic
 * when testing api crash recovery
 * if crash recovery conditions hold the number of iterations determined to be 2
 * else, it determined to be 1
 * for more information read comments in the file switch.h (search BCM_CR_TRANSACTION_START)
 */
int soc_dcmn_cr_test_get_num_of_iterations(int unit) {
    /* cr test supported only on JERICHO devices */
    int test_mode;
    if (!BCM_UNIT_VALID(unit) || !SOC_IS_JERICHO(unit)) return 1;
    soc_dcmn_wb_test_mode_get(unit, &test_mode);
    if ((test_mode == _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY
          || test_mode == _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK)
           && dcmn_bcm_warm_boot_api_test_reset_conds(unit)) {
        int is_recovarable;
        _dcmn_wb_skip_decision_t decision;
        is_recovarable = dcmn_cr_info[unit]->is_recovarable;
        /* simulate api test reset decider */
        /* exponential backoff decider */
        decision = dcmn_bcm_warm_boot_api_test_reset_skip_decider(unit, NULL);
        if ((decision == _dcmnDontSkip) && is_recovarable) {
            return 2;
        }
    }
    return 1;
}

/* this function used by the macro BCM_CR_TRANSACTION_END
   only in the case of crash recovery test */

int soc_dcmn_cr_test_transaction_end(int unit, int reset_num) {
    int retVal = 0;
    int8 is_reset = 0;
    int test_mode;
#ifdef SW_STATE_DIFF_DEBUG
    int diff_rv = 0;
#endif
    /* first reset */
    if (reset_num == 1) {
        retVal = dcmn_bcm_warm_boot_api_test_reset(unit, &is_reset);
        /* We call this function after checking the test reset conditions
         * (see BCM_CR_TRANSACTION_END macro definition in the file switch.h)
         * so this invariant must holds in this point.
         * is_reset gets the value -1 only if the reset conditions dont hold
         * (see dcmn_bcm_warm_boot_api_test_reset).
         */
#if SW_STATE_DIFF_DEBUG
        cli_out("%d: Dumping sw_state Data Block to file /dev/shm/warmboot_data_after_rb.txt\n", unit);
        shr_sw_state_block_dump(unit, SW_STATE_EFFICIENT_DUMP,"/dev/shm/warmboot_data_after_rb.txt", "w");
        cli_out("%s(): Diffing files /dev/shm/warmboot_data_beginning_of_API.txt with /dev/shm/warmboot_data_after_rb.txt\n", FUNCTION_NAME());
        diff_rv = diff_two_files(unit, "/dev/shm/warmboot_data_beginning_of_API.txt", "/dev/shm/warmboot_data_after_rb.txt");
        if (diff_rv < 0) {
            cli_out("CR_DIFF FAIL: sw state has changed after roll back. to see the difference compare files.\n");
        } else {
            cli_out("CR_DIFF PASS: sw state files identical.\n");
        }
        /* check that the sw_state journal is empty */
        if (sw_state_journal[unit]->nof_log_entries != 0) {
            cli_out("CR_EMPTY_SW_JOURNAL FAIL - the sw_state_journal is not empty after roll back.\n");
        } else {
            cli_out("CR_EMPTY_SW_JOURNAL PASS - the sw_state_journal is empty after roll back.\n");
        }
        if (Hw_Log_List[unit].header->nof_elements != 0) {
            cli_out("CR_EMPTY_HW_LOG FAIL - the hw_log is not empty after roll back.\n");
        } else {
            cli_out("CR_EMPTY_HW_LOG PASS - the hw_log is empty after roll back.\n");
        }
#endif
        assert (is_reset != -1);
    } else { /* reset_num == 2 */
        assert (reset_num == 2);

        soc_dcmn_wb_test_mode_get(unit, &test_mode);
        /* The following singal is for checking the WRITE AHEAD mechanism of the crash recovery
           hence, we don't want to singal in case of ROLL BACK test */
        if (test_mode != _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK) {
            /* singal the prepare_for_commit function to perform api test reset */
            dcmn_cr_test_signal_after_prepare_for_commit_set(unit);
            /* if we have reached this point (we already did the first tr 141) we want to perform the second crash (tr 141)
               regardless the reset conditions */
            soc_dcmn_wb_enforce_once_set(unit, 1);
        }
        SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_dispatcher_commit(unit));
    }
    return retVal;
}

int soc_dcmn_cr_test_transaction_end_regular_reset(int unit) {
  int test_mode = 0, reset_rv = 0;
  int8 is_reset = 0;
  soc_dcmn_wb_test_mode_get(unit, &test_mode);
  if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY == test_mode ||
          _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK == test_mode) {
      int is_recovarable;
      is_recovarable = dcmn_cr_info[unit]->is_recovarable;
      if (!is_recovarable) return reset_rv;
      reset_rv = dcmn_bcm_warm_boot_api_test_reset(unit, &is_reset);
      /* we can reach here only if the num_of_iterations is equal to 1
       * which means that the reset conditions dont hold */
       assert (is_reset != 1);
  } else if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API == test_mode) {
      reset_rv = dcmn_bcm_warm_boot_api_test_reset(unit, NULL);
  }
  return reset_rv;
}

int dcmn_cr_test_second_crash(int unit){
    SOC_INIT_FUNC_DEFS;
    if (dcmn_cr_test_signal_after_prepare_for_commit_get(unit)) {
        int8 is_reset;
        dcmn_cr_test_signal_before_prepare_for_commit_set(unit);
        /* perform the second crash regardless of the reset conditions */
        soc_dcmn_wb_enforce_once_set(unit, 1);
        SOC_WB_API_RESET_ERR_CHECK(dcmn_bcm_warm_boot_api_test_reset(unit, &is_reset));
        assert(is_reset == 1);
    /* jump back to the API and continues from there (without repeating the API logic) */
        longjmp(dcmn_cr_test_buf[unit], 1);
    }

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/* When we simulate crash recovery, we want to crash twice.
   The first time is before raising commit flags and the second time is after raising this flag.
   (for more information read the comments above the BCM_CR_TRANSACTION_START macro definition in the file switch.h)
   After the first crash, we want to signal to the commit function that we want to perform the second reset.
   Furthermore, we want the commit function to not perform reset in any other case.
   So to do this we use the following functions */


void dcmn_cr_test_signal_before_prepare_for_commit_set(int unit) {
    dcmn_cr_test_crash_status[unit] = _dcmnCrResetBeforeCommittingMode;
}

void dcmn_cr_test_signal_after_prepare_for_commit_set(int unit) {
    dcmn_cr_test_crash_status[unit] = _dcmnCrResetAfterCommittingMode;
}

int dcmn_cr_test_signal_before_prepare_for_commit_get(int unit) {
    return (dcmn_cr_test_crash_status[unit] == _dcmnCrResetBeforeCommittingMode);
}

int dcmn_cr_test_signal_after_prepare_for_commit_get(int unit) {
    return (dcmn_cr_test_crash_status[unit] == _dcmnCrResetAfterCommittingMode);
}

#endif
