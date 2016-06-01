/*
 * $Id: dfe_port.c,v 1.13 Broadcom SDK $
 *
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
 * SOC DCMN WB
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>


#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT)

#include <soc/dcmn/dcmn_wb.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/hwstate/hw_log.h>
#ifdef BCM_WARM_BOOT_API_TEST
#include <setjmp.h>
#endif
#include <shared/bsl.h>
#include <appl/diag/shell.h>

/* general flag, indication whether we are in WB test mode or not */
int    _soc_dcmn_wb_warmboot_test_mode[SOC_MAX_NUM_DEVICES];
/* temporary disabling WB test mode */
int    _soc_dcmn_wb_override_wb_test[SOC_MAX_NUM_DEVICES];
/* temporary disabling WB test mode due to temporarily field unstability */
int    _soc_dcmn_wb_field_test_mode_stop[SOC_MAX_NUM_DEVICES][_dcmnWbFieldFlagNOF];
/* disabling WB test mode for one BCM API call */
int    _soc_dcmn_wb_disable_once_wb_test[SOC_MAX_NUM_DEVICES];
/* enforcing WB test mode for one BCM API call */
int    _soc_dcmn_wb_enforce_once_wb_test[SOC_MAX_NUM_DEVICES];
/* counter, counting the number of warmboot test performed */
int    _soc_dcmn_wb_warmboot_test_counter[SOC_MAX_NUM_DEVICES];
/* number of warmboot tests to be skipped (not including the "disable once" tests */
int    _soc_dcmn_wb_warmboot_test_nof_tests_to_skip[SOC_MAX_NUM_DEVICES];




/* check conditions for running api test reset */

int dcmn_bcm_warm_boot_api_test_reset_conds (int unit) {
    int warmboot_test_mode_enable;
    int no_wb_test, field_wb_stop;
    int disable_once;
    int enforce_once = 0;
    if (sal_thread_self() == sal_thread_main_get()) {
        soc_dcmn_wb_enforce_once_get(unit, &enforce_once);
        if (enforce_once) {
          return 1;
        }
        soc_dcmn_wb_test_mode_get(unit, &warmboot_test_mode_enable);
        soc_dcmn_wb_field_test_mode_get(unit, _dcmnWbFieldFlagDirectExtraction, &field_wb_stop);
        if (!field_wb_stop){
            soc_dcmn_wb_field_test_mode_get(unit, _dcmnWbFieldFlagDataQual, &field_wb_stop);
        }
        soc_dcmn_wb_no_wb_test_get(unit, &no_wb_test);
        soc_dcmn_wb_disable_once_get(unit, &disable_once);
        if (((_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API == warmboot_test_mode_enable)
              || (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY == warmboot_test_mode_enable)
                  || (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK == warmboot_test_mode_enable))
                && ((0 == no_wb_test) && (0 == disable_once) && (0 == field_wb_stop))) {
                return 1;
        }
    }
    return 0;
}

/** decide if to skip the current reset or not
  * there are two possible reasons for skipping the current test:
  * 1) the skip counter is greater that zero (designed for handling recursive calls to the reset function)
  * 2) skipping warm reboot test for faster regression (by using exponential backoff algorithm)
  * retval - _dcmnWbSkipCounter - for reason #1
  * retval - _dcmnWbSkipFasterRegression - for reason #2
  * retval - _dcmnDontSkip */

_dcmn_wb_skip_decision_t dcmn_bcm_warm_boot_api_test_reset_skip_decider(int unit) {
    int test_counter, skip_count, enforce_once;
    soc_dcmn_wb_enforce_once_get(unit, &enforce_once);
    if (enforce_once) {
        return _dcmnDontSkip;
    }
    soc_dcmn_wb_test_skip_multiple_wb_tests_get(unit, &skip_count);
    if (skip_count>0)  {
        return _dcmnWbSkipCounter;
    } else {
          soc_dcmn_wb_test_counter_get(unit, &test_counter);
          if (!dcmn_wb_exp_backoff(test_counter)) {
              return _dcmnWbSkipFasterRegression;
          }
    }
    return _dcmnDontSkip;
}

/* in case WB sequence was skipped once, */
/* setting the flag to '0' in order that next time WB sequence will occure  */
void dcmn_bcm_warm_boot_api_test_enable_if_disabled_once(int unit) {
    int disable_once;
    soc_dcmn_wb_disable_once_get(unit, &disable_once);
    if (1 == disable_once) {
        soc_dcmn_wb_disable_once_set(unit, 0);
    }
    return;
}

/* in case WB sequence was enforced once, */
/* setting the flag to '0' in order that next time WB sequence will occure (regularly)  */
void dcmn_bcm_warm_boot_api_test_disable_if_enforced_once(int unit) {
    int enforce_once;
    soc_dcmn_wb_enforce_once_get(unit, &enforce_once);
    if (1 == enforce_once) {
        soc_dcmn_wb_enforce_once_set(unit, 0);
    }
    return;
}


int dcmn_bcm_warm_boot_api_test_reset(int unit, int8* is_reset) {

    _dcmn_wb_skip_decision_t skip_decision;
    int tr141_finish_successfully = 0;
    int skip_count, test_counter, enforce_once;
    int warmboot_test_mode = 0, is_cr = 0;
#ifdef CRASH_RECOVERY_SUPPORT
    int stored_val = 0;
#endif
    if (is_reset) *is_reset = 0;

#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_set_immediate_hw_access(unit, &stored_val);
#endif
    if (dcmn_bcm_warm_boot_api_test_reset_conds(unit)) {
        soc_dcmn_wb_enforce_once_get(unit, &enforce_once);
        /* if enforce_once we want the counter to remain the same
           for maintain consistency between wb test and cr test */

        skip_decision = dcmn_bcm_warm_boot_api_test_reset_skip_decider(unit);

        /* count even if skipped */
        if(!enforce_once){
            soc_dcmn_wb_test_counter_plus_plus(unit);
        }

        soc_dcmn_wb_test_counter_get(unit, &test_counter);
        LOG_WARN(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                      "**** WB BCM API %s **** (test counter: %d) ****\n"),
                      FUNCTION_NAME(), test_counter));

        switch (skip_decision) {
            case  _dcmnWbSkipCounter:
                soc_dcmn_wb_test_skip_counter_minus_minus(unit);
                soc_dcmn_wb_test_skip_multiple_wb_tests_get(unit, &skip_count);
                LOG_WARN(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "  --> skipping warmboot test (%d more tests to skip)\n"), skip_count));
                break;
            case _dcmnWbSkipFasterRegression:
                LOG_WARN(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "  --> skipping warm reboot test for faster regression\n")));
                break;
            case _dcmnDontSkip:
            default:

                LOG_DEBUG(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "Unit:%d Starting warm boot test\n"), unit));

                /*waiting for warmboot test to finish to avoid recursive calling to TR_141_COMMAND */
                soc_dcmn_wb_no_wb_test_set(unit,1);

                dcmn_bcm_warm_boot_api_test_disable_if_enforced_once(unit);
                soc_dcmn_wb_test_mode_get(unit, &warmboot_test_mode);
                is_cr = (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY == warmboot_test_mode ||
                            _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK == warmboot_test_mode) ? 1 : 0;
                if (0 != sh_process_command(unit, TR_141_COMMAND(is_cr))) {
                    LOG_DEBUG(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit, "Unit:%d Warm boot test failed\n"), unit));
                    return -1;
                }

                LOG_DEBUG(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit, "Unit:%d Warm boot test finish successfully\n"), unit));
                tr141_finish_successfully = 1;

                /*enable warmboot test*/
                soc_dcmn_wb_no_wb_test_set(unit,0);

        }
    } else {
        if (is_reset) *is_reset = -1;
    }

    dcmn_bcm_warm_boot_api_test_enable_if_disabled_once(unit);
    if (tr141_finish_successfully == 1) {
        if (is_reset) *is_reset = 1;
    }
#ifdef CRASH_RECOVERY_SUPPORT
    soc_hw_restore_immediate_hw_access(unit, stored_val);
#endif
    return 0;
}


int dcmn_bcm_warm_boot_api_test_sync (int unit) {
    int rv = BCM_E_NONE, warmboot_test_mode;
    if (!SOC_UNIT_VALID(unit)) return BCM_E_UNIT;
    soc_dcmn_wb_test_mode_get(unit, &warmboot_test_mode);
    if (!(_DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY == warmboot_test_mode ||
        _DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY_ROLL_BACK == warmboot_test_mode)) {
            rv = dcmn_bcm_warm_boot_api_test_reset(unit, NULL);
    }
    return rv;
}


/* 
 * allowed values for warmboot_test_mode_set/get: 
 * ---------------------------------------------- 
 * 0:WB_TEST_MODE_NONE
 * 1:WB_TEST_MODE_AFTER_EVERY_API
 * 2:WB_TEST_MODE_END_OF_DVAPIS 
 * 3:WB_TEST_MODE_CRASH_RECOVERY
 */

void
soc_dcmn_wb_test_mode_set(int unit, int enable)
{
    _soc_dcmn_wb_warmboot_test_mode[unit] = enable;
}

void
soc_dcmn_wb_test_mode_get(int unit, int *enable)
{
    *enable =  _soc_dcmn_wb_warmboot_test_mode[unit];
}

void
soc_dcmn_wb_test_counter_set(int unit, int counter)
{
    _soc_dcmn_wb_warmboot_test_counter[unit] = counter;
}

void
soc_dcmn_wb_test_counter_get(int unit, int *counter)
{
    *counter =  _soc_dcmn_wb_warmboot_test_counter[unit];
}

void
soc_dcmn_wb_test_counter_plus_plus(int unit)
{
    _soc_dcmn_wb_warmboot_test_counter[unit]++;
}

void
soc_dcmn_wb_test_counter_reset(int unit)
{
    _soc_dcmn_wb_warmboot_test_counter[unit] = 0;
}

void
soc_dcmn_wb_test_skip_multiple_wb_tests_set(int unit, int nof_tests_to_skip)
{
    _soc_dcmn_wb_warmboot_test_nof_tests_to_skip[unit] = nof_tests_to_skip;
}

void
soc_dcmn_wb_test_skip_counter_minus_minus(int unit)
{
    if (_soc_dcmn_wb_warmboot_test_nof_tests_to_skip[unit] == 0) {
        return;
    }
    else {
        _soc_dcmn_wb_warmboot_test_nof_tests_to_skip[unit]--;
    }
}

void
soc_dcmn_wb_test_skip_multiple_wb_tests_get(int unit, int *nof_tests_to_skip)
{
    *nof_tests_to_skip = _soc_dcmn_wb_warmboot_test_nof_tests_to_skip[unit];
}



/* the two functions below set\get a flag that override the wb test mode
 * i.e if test mode is on (perform warm rebooot at the end of APIs) turning this
 * flag on will instruct the driver to not perform the reboots  */ 
void
soc_dcmn_wb_no_wb_test_set(int unit, int wb_flag)
{
    if (wb_flag == 0) 
    {
        _soc_dcmn_wb_override_wb_test[unit]--;
    }
    else
    {
        _soc_dcmn_wb_override_wb_test[unit]++;
    }
}

void
soc_dcmn_wb_no_wb_test_get(int unit, int *wb_flag)
{
    *wb_flag =  _soc_dcmn_wb_override_wb_test[unit];
}


void soc_dcmn_wb_field_test_mode_set(int unit, _dcmn_wb_field_falgs_e type, int wb_flag)
{
    _soc_dcmn_wb_field_test_mode_stop[unit][type] = wb_flag;
}  
void soc_dcmn_wb_field_test_mode_get(int unit, _dcmn_wb_field_falgs_e type, int *wb_flag)
{
    *wb_flag =  _soc_dcmn_wb_field_test_mode_stop[unit][type];
} 

/* the two functions below set\get a flag that disable the wb test mode
 * i.e if test mode is on (perform warm rebooot at the end of APIs) turning this
 * flag on will instruct the driver to not perform the when the BCM API finish to run, 
 * should be used by APIs that
 * create a mismatch btween SW state and HW state, for example: field instructions saved to SW
 * but not yet commited to HW should turn on this flag. */ 
void
soc_dcmn_wb_disable_once_set(int unit, int wb_flag)
{
    _soc_dcmn_wb_disable_once_wb_test[unit] = wb_flag;
}

void
soc_dcmn_wb_disable_once_get(int unit, int *wb_flag)
{
    *wb_flag = _soc_dcmn_wb_disable_once_wb_test[unit];
}

void
soc_dcmn_wb_enforce_once_set(int unit, int wb_flag)
{
    _soc_dcmn_wb_enforce_once_wb_test[unit] = wb_flag;
}

void
soc_dcmn_wb_enforce_once_get(int unit, int *wb_flag)
{
    *wb_flag = _soc_dcmn_wb_enforce_once_wb_test[unit];
}

int dcmn_bcm_warm_boot_api_test_mode_skip_wb_sequence(int unit) {
    int rv = BCM_E_NONE;
    if (sal_thread_self() == sal_thread_main_get()) {
        /* perform only for main thread */
        soc_dcmn_wb_disable_once_set(unit, 1);
    }
    return rv;
}

int dcmn_wb_exp_backoff(int test_counter) {
    int i=DCMN_WARM_EXP_PARAM;
    while (i < test_counter) {
        i *= DCMN_WARM_EXP_PARAM;
    }
    return (test_counter % (i/DCMN_WARM_EXP_PARAM) == 0 || test_counter == 0);
}

#endif /*(defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT)*/
