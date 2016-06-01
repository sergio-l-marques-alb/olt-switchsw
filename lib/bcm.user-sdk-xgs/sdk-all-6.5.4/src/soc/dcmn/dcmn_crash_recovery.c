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


#ifdef CRASH_RECOVERY_SUPPORT

#include <soc/hwstate/hw_log.h>
#include <shared/swstate/sw_state_journal.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/dcmn/dcmn_wb.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/dcmn/dcmn_wb.h>
#include <setjmp.h>
#endif

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

#define DCMN_CR_COULD_NOT_GUARANTEE 0
#define DCMN_CR_COMMITTED           1
#define DCMN_CR_ABORTED             2


#define DCMN_CR_UNIT_CHECK(unit) ((unit) >= 0 && (unit) < SOC_MAX_NUM_DEVICES)

#define CR_ACCESS sw_state_access[unit].dcmn.crash_recovery

#define DCMN_CR_DEFAULT_JOURNAL_SIZE (20000000) /*20MB*/

uint8 dcmn_cr_suspect_crash[SOC_MAX_NUM_DEVICES];

#ifdef BCM_WARM_BOOT_API_TEST
/* the buffer used by cr test - for more info search BCM_CR_TRANSACTION_START */
jmp_buf dcmn_cr_buf[SOC_MAX_NUM_DEVICES];
_dcmn_cr_stat_t dcmn_cr_crash_status[SOC_MAX_NUM_DEVICES];
#endif

extern int diff_two_files(char *file_name1, char *file_name2);
extern sw_state_journal_t *sw_state_journal[BCM_MAX_NUM_UNITS];

int soc_dcmn_cr_init(int unit){

    uint32 hw_journal_size;
    uint32 sw_journal_size;

    SOC_INIT_FUNC_DEFS;



    /* mark crash recovery as enabled */
    (SOC_CONTROL(unit))->crash_recovery = 1;

    /* init HW Log */
    hw_journal_size = soc_property_get(unit, spn_HA_HW_JOURNAL_SIZE, 0);
    _SOC_IF_ERR_EXIT(soc_hw_log_init(unit, hw_journal_size));

    /* init SW State Journal */
    sw_journal_size = soc_property_get(unit, spn_HA_SW_JOURNAL_SIZE, 0);
    _SOC_IF_ERR_EXIT(sw_state_journal_init(unit, sw_journal_size));

    /* Mark transaction as idle */
    if (!SOC_WARM_BOOT(unit)) {
        _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_mode.set(unit, DCMN_TRANSACTION_MODE_IDLE));
    } else {
        /* suspect a crash */
        dcmn_cr_suspect_crash[unit] = TRUE;
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_transaction_start(int unit) {
    int is_immediate = 0;
    
    SOC_INIT_FUNC_DEFS;

    /* exit without doing anything if not the main thread */
    DCMN_CR_EXIT_IF_NOT_MAIN_THREAD;

    soc_hw_set_immediate_hw_access(unit, &is_immediate);
    soc_hw_restore_immediate_hw_access(unit, is_immediate);
    
    if (is_immediate) {
        SOC_EXIT;
    }
    
    if (SOC_CR_ENABALED(unit)) {
        /* Mark transaction as started */
        _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_mode.set(unit,DCMN_TRANSACTION_MODE_LOGGING));

        /* inform hw_log to start logging */
        _SOC_IF_ERR_EXIT(soc_hw_log_transaction_start(unit));

        /* inform sw_state_journal to start logging */
        _SOC_IF_ERR_EXIT(sw_state_journal_transaction_start(unit));
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int 
soc_dcmn_cr_prepare_for_commit(int unit){
    SOC_INIT_FUNC_DEFS;

    /* inform hw_log to stop logging */
    _SOC_IF_ERR_EXIT(soc_hw_log_transaction_end(unit));

    /* inform sw_state_journal to stop logging */
    _SOC_IF_ERR_EXIT(sw_state_journal_transaction_end(unit));

    /* raise ready for commit flag */
    _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_mode.set(unit,DCMN_TRANSACTION_MODE_COMMITTING));

/* The following is not part of the logic of the commit.
 * It compiles only for CRASH RECOVERY api test.
 */
#if defined(BCM_ARAD_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    /* if this signal is on, it means that we have to perform the second crash */
    if (dcmn_cr_signal_after_prepare_for_commit_get(unit)) {
        int8 is_reset;
        dcmn_cr_signal_before_prepare_for_commit_set(unit);
        /* perform the second crash regardless of the reset conditions */
        soc_dcmn_wb_enforce_once_set(unit, 1);
        SOC_WB_API_RESET_ERR_CHECK(dcmn_bcm_warm_boot_api_test_reset(unit, &is_reset));
        assert(is_reset == 1);
        SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_commit(unit));
    /* jump back to the API and continues from there (without repeating the API logic) */
        longjmp(dcmn_cr_buf[unit], 1);
    }
#endif

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_commit(int unit){

#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
    uint8                   kbp_dirty,kaps_dirty;
    uint32                  kbp_tbl_id,kaps_tbl_id;
#endif
    DCMN_TRANSACTION_MODE  transaction_mode;
    int is_immediate = 0;
    
    SOC_INIT_FUNC_DEFS;

    DCMN_CR_EXIT_IF_NOT_MAIN_THREAD;
    
    soc_hw_set_immediate_hw_access(unit, &is_immediate);
    soc_hw_restore_immediate_hw_access(unit, is_immediate);
    
    if (is_immediate) {
      SOC_EXIT;
    }
    /* supported only for jericho */
    if (!SOC_IS_JERICHO(unit)) SOC_EXIT;

    /* sanity */
    if (!SOC_CR_ENABALED(unit)) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                       (BSL_META_U(unit,
                          "unit:%d Crash Recovery ERROR: trying to commit while Crash Recovery feature is disabled\n"), unit));
    }

    /* perform some steps to insure all data is synced to NV memory */
    /* skip it if you are "re-committing after recovering from a crash */
    _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_mode.get(unit, &transaction_mode));
    if (transaction_mode != DCMN_TRANSACTION_MODE_COMMITTING) {
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_prepare_for_commit(unit));
    }

    /* call kbp install if needed */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (ARAD_KBP_IS_CR_MODE && JER_KAPS_IS_CR_MODE) {
        _SOC_IF_ERR_EXIT(CR_ACCESS.kbp_dirty.get(unit, &kbp_dirty));
        if (kbp_dirty) {
            _SOC_IF_ERR_EXIT(CR_ACCESS.kbp_tbl_id.get(unit,&kbp_tbl_id));
            if (arad_kbp_cr_db_commit(unit,kbp_tbl_id)) {
                _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "unit:%d Crash Recovery ERROR: transaction commit failed to install KBP\n"), unit));
            }
            _SOC_IF_ERR_EXIT(arad_kbp_cr_transaction_cmd(unit,FALSE));
        }

        /* call kaps install if needed */
        _SOC_IF_ERR_EXIT(CR_ACCESS.kaps_dirty.get(unit,&kaps_dirty));
        if (kaps_dirty) {
            _SOC_IF_ERR_EXIT(CR_ACCESS.kaps_tbl_id.get(unit,&kaps_tbl_id));
            if (jer_kaps_cr_db_commit(unit,kaps_tbl_id)) {
                _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "unit:%d Crash Recovery ERROR: transaction commit failed to install KAPS\n"), unit));
            }
            _SOC_IF_ERR_EXIT(jer_kaps_cr_transaction_cmd(unit,FALSE));
        }

        /* clean KBP and KAPS dirty bit */
        _SOC_IF_ERR_EXIT(CR_ACCESS.kbp_dirty.set(unit,0));
        _SOC_IF_ERR_EXIT(CR_ACCESS.kaps_dirty.set(unit,0));
    }
#endif

    /* apply hw log */
    _SOC_IF_ERR_EXIT(soc_hw_log_commit(unit));

    /* lower ready for commit flag */
    _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_mode.set(unit,DCMN_TRANSACTION_MODE_IDLE));

    /* dismiss SW State roll back journal */
    _SOC_IF_ERR_EXIT(sw_state_journal_clear(unit));

    /* dismiss HW Log */
    soc_hw_log_purge(unit);

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_dispatcher_commit(int unit){

    int is_immediate = 0;
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    /* exit without doing anything if not the main thread */
    DCMN_CR_EXIT_IF_NOT_MAIN_THREAD;

    soc_hw_set_immediate_hw_access(unit, &is_immediate);
    soc_hw_restore_immediate_hw_access(unit, is_immediate);
    
    if (is_immediate) {
      SOC_EXIT;
    }
    
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));

    /* if logging was suppressed, unsupress it after commit */
    _SOC_IF_ERR_EXIT(soc_dcmn_cr_unsuppress(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_abort(int unit){
    SOC_INIT_FUNC_DEFS;

    DCMN_CR_EXIT_IF_NOT_MAIN_THREAD;

    /* apply roll back log */
    _SOC_IF_ERR_EXIT(sw_state_journal_roll_back(unit));

    /* dismiss SW State roll back journal */
    _SOC_IF_ERR_EXIT(sw_state_journal_clear(unit));

    /* clear kaps/kbp last transaction status */
#if defined(INCLUDE_KBP) && !defined(BCM_88030) 
    if (ARAD_KBP_IS_CR_MODE && JER_KAPS_IS_CR_MODE) {
        _SOC_IF_ERR_EXIT(arad_kbp_cr_clear_restore_status(unit));
        _SOC_IF_ERR_EXIT(jer_kaps_cr_clear_restore_status(unit));
    }
#endif

    /* dismiss HW Log */
    _SOC_IF_ERR_EXIT(soc_hw_log_purge(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_recover(int unit){
    DCMN_TRANSACTION_MODE commit_status;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
    uint8 kaps_commited = 0,kbp_commited = 0;
#endif
    int is_recovarable;
    SOC_INIT_FUNC_DEFS;

    /* if not suspecting a crash exit (e.g. we are in cold reboot) */
    if (!dcmn_cr_suspect_crash[unit]) SOC_EXIT;

    /* check if ready for commit flag is raised */
    _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_mode.get(unit,&commit_status));
    if (commit_status == DCMN_TRANSACTION_MODE_COMMITTING) {
        /* check if KBP/KAPS committed */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)        
        if (ARAD_KBP_IS_CR_MODE && JER_KAPS_IS_CR_MODE) {
            kbp_commited = arad_kbp_cr_query_restore_status(unit);
            kaps_commited = jer_kaps_cr_query_restore_status(unit);
        }
        if (!kaps_commited || !kbp_commited) {
            _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
            _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_status.set(unit, DCMN_CR_ABORTED));
        }
        else
#endif
        {
            _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));
            _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_status.set(unit, DCMN_CR_COMMITTED));
        }
    } else if (commit_status == DCMN_TRANSACTION_MODE_LOGGING) {
        _SOC_IF_ERR_EXIT(soc_dcmn_cr_abort(unit));
        _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_status.set(unit, DCMN_CR_ABORTED));
    } else if (commit_status == DCMN_TRANSACTION_MODE_IDLE) {
        _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_status.set(unit, DCMN_CR_COULD_NOT_GUARANTEE));
        _SOC_IF_ERR_EXIT(CR_ACCESS.not_recoverable_reason.set(unit, dcmn_cr_no_support_not_in_api));
    }

    CR_ACCESS.is_recovarable.get(unit, &is_recovarable);
    if (!is_recovarable) {
        _SOC_IF_ERR_EXIT(CR_ACCESS.transaction_status.set(unit, DCMN_CR_COULD_NOT_GUARANTEE));
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_suppress(int unit, dcmn_cr_no_support reason){

    SOC_INIT_FUNC_DEFS;

    DCMN_CR_EXIT_IF_NOT_MAIN_THREAD;

    /* no point in suppressing if not enabled */
    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    /* Inform that state is not protected by Crash Recovery and may not be consistent after a crash */
    _SOC_IF_ERR_EXIT(CR_ACCESS.is_recovarable.set(unit, FALSE));
    _SOC_IF_ERR_EXIT(CR_ACCESS.not_recoverable_reason.set(unit, reason));


    _SOC_IF_ERR_EXIT(soc_dcmn_cr_commit(unit));
    _SOC_IF_ERR_EXIT(soc_hw_log_suppress(unit));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_unsuppress(int unit){
    SOC_INIT_FUNC_DEFS;

    /* no point in unsuppressing if not enabled */
    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    _SOC_IF_ERR_EXIT(soc_hw_log_unsuppress(unit));

    /* Inform that state is consistent and protected by Crash Recovery again */
    _SOC_IF_ERR_EXIT(CR_ACCESS.is_recovarable.set(unit,TRUE));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}


#if defined(BCM_ARAD_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)

/*
 * this function determines the number of iterations to perform the logic
 * when testing api crash recovery
 * if crash recovery conditions hold the number of iterations determined to be 2
 * else, it determined to be 1
 * for more information read comments in the file switch.h (search BCM_CR_TRANSACTION_START)
 */
int soc_dcmn_cr_get_num_of_iterations(int unit) {
    /* cr test supported only on JERICHO devices */  
    if (!SOC_IS_JERICHO(unit)) return 1;
    if (dcmn_bcm_warm_boot_api_test_reset_conds(unit)) {
        int test_mode, is_recovarable;
        _dcmn_wb_skip_decision_t decision;
        soc_dcmn_wb_test_mode_get(unit, &test_mode);
        CR_ACCESS.is_recovarable.get(unit, &is_recovarable);
        /* simulate api test reset decider */
        /* exponential backoff decider */
        decision = dcmn_bcm_warm_boot_api_test_reset_skip_decider(unit);
        if ((decision == _dcmnDontSkip) &&
                (test_mode == _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY
                    || test_mode == _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK)
                        && is_recovarable) {
			return 2;
        }
    }
    return 1;
}

/* this function used by the macro BCM_CR_TRANSACTION_END
   only in the case of crash recovery test */

int soc_dcmn_cr_transaction_end(int unit, int reset_num) {
    int retVal = 0;
    int8 is_reset = 0;
    int test_mode;
#ifdef BCM_WARM_BOOT_API_TEST
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
#ifdef BCM_WARM_BOOT_API_TEST
        cli_out("%d: Dumping sw_state Data Block to file /dev/shm/warmboot_data_after_rb.txt\n", unit);
        shr_sw_state_block_dump(unit, "/dev/shm/warmboot_data_after_rb.txt", "w");
        cli_out("%s(): Diffing files /dev/shm/warmboot_data_beginning_of_API.txt with /dev/shm/warmboot_data_after_rb.txt\n", FUNCTION_NAME());
        diff_rv = diff_two_files("/dev/shm/warmboot_data_beginning_of_API.txt", "/dev/shm/warmboot_data_after_rb.txt");
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
            dcmn_cr_signal_after_prepare_for_commit_set(unit);
            /* if we have reached this point (we already did the first tr 141) we want to perform the second crash (tr 141)
               regardless the reset conditions */
            soc_dcmn_wb_enforce_once_set(unit, 1);
        }
        SOC_CR_DISP_ERR_CHECK(soc_dcmn_cr_dispatcher_commit(unit));
    }
    return retVal;
}

int soc_dcmn_cr_transaction_end_regular_reset(int unit) {
  int test_mode = 0, reset_rv = 0;
  int8 is_reset = 0;
  soc_dcmn_wb_test_mode_get(unit, &test_mode);
  if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY == test_mode ||
          _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK == test_mode) {
      int is_recovarable;
      CR_ACCESS.is_recovarable.get(unit, &is_recovarable);
      reset_rv = dcmn_bcm_warm_boot_api_test_reset(unit, &is_reset);
      /* assert reset has not been done in case of is_recovarable
       * we can reach here only if the num_of_iterations is equal to 1
       * which means that the reset conditions dont hold or !is_recovarable
       * in case !is_recovarable we want to reset for maintain consistency with wb regression */
      assert (is_reset != 1 || !is_recovarable);
  } else if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API == test_mode) {
      reset_rv = dcmn_bcm_warm_boot_api_test_reset(unit, NULL);
  }
  return reset_rv;
}




/* When we simulate crash recovery, we want to crash twice.
   The first time is before raising commit flags and the second time is after raising this flag.
   (for more information read the comments above the BCM_CR_TRANSACTION_START macro definition in the file switch.h)
   After the first crash, we want to signal to the commit function that we want to perform the second reset.
   Furthermore, we want the commit function to not perform reset in any other case.
   So to do this we use the following functions */


void dcmn_cr_signal_before_prepare_for_commit_set(int unit) {
    dcmn_cr_crash_status[unit] = _dcmnCrResetBeforeCommittingMode;
}

void dcmn_cr_signal_after_prepare_for_commit_set(int unit) {
    dcmn_cr_crash_status[unit] = _dcmnCrResetAfterCommittingMode;
}

int dcmn_cr_signal_before_prepare_for_commit_get(int unit) {
    return (dcmn_cr_crash_status[unit] == _dcmnCrResetBeforeCommittingMode);
}

int dcmn_cr_signal_after_prepare_for_commit_get(int unit) {
    return (dcmn_cr_crash_status[unit] == _dcmnCrResetAfterCommittingMode);
}

#endif

#undef CR_ACCESS

#endif
