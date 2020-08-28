/*
 * $Id: dnxc_wb_test_utils.c $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXC
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOCDNX_SWSTATE

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)

#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <shared/bsl.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#endif /* BCM_DNX_SUPPORT */

#define DNXC_TR_141_COMMAND "tr 141 w=1"

/* in the first DNXC_WARM_BOOT_EXP_PARAM calls, don't skip the reset sequence */
#define DNXC_WARM_BOOT_EXP_PARAM 10

/* general flag, indication whether we are in WB test mode or not */
int dnxc_wb_warmboot_test_mode[SOC_MAX_NUM_DEVICES];
/* temporary disabling WB test mode */
int dnxc_wb_override_wb_test[SOC_MAX_NUM_DEVICES];
/* disabling WB test mode for one BCM API call */
int dnxc_wb_disable_once_wb_test[SOC_MAX_NUM_DEVICES];
/* counter, counting the number of warmboot test performed */
int dnxc_wb_warmboot_test_counter[SOC_MAX_NUM_DEVICES];
/* counter, counting the number of APIs in current stack */
int dnxc_wb_nested_api_counter[SOC_MAX_NUM_DEVICES];
/* filter to run test only when counter is filter[0] < counter < filter[1] */
int dnxc_wb_counter_filter[SOC_MAX_NUM_DEVICES][2];
/* Mark the deinit-init when is run durint Wb test */
int dnxc_wb_deinit_init_during_wb_test[SOC_MAX_NUM_DEVICES];
/* Saves the main thread.*/
sal_thread_t dnxc_wb_main_thread[SOC_MAX_NUM_DEVICES];

/* Saved callback function, which invoke after wb done successfully */
dnxc_wb_test_callback_t dnxc_wb_test_callback[SOC_MAX_NUM_DEVICES];

int (
    *dnxc_wb_api_test_tr_141_command_function_ptr[SOC_MAX_NUM_DEVICES]) (
    int unit);

char dnxc_warmboot_api_function_name[SOC_MAX_NUM_DEVICES][DNXC_WARMBOOT_API_FUNCTION_NAME_MAX_LEN];

static int dnxc_wb_api_test_counter_is_top_level(
    int unit);
static int dnxc_wb_api_test_counter_get(
    int unit);
static int dnxc_wb_all_reset_test_utils_preconditions(
    int unit);
static void dnxc_wb_test_counter_get(
    int unit,
    int *counter);
static void dnxc_wb_test_counter_plus_plus(
    int unit);
static void dnxc_wb_disable_once_set(
    int unit,
    int wb_flag);
static void dnxc_wb_disable_once_get(
    int unit,
    int *wb_flag);
static int dnxc_wb_test_exp_backoff(
    int test_counter,
    int *nof_skip);
/*
 * Sets WB test main thread.
 */
static void dnxc_wb_test_main_thread_set(
    int unit);

/* 
 * this is intended to be used by all utilities that participate in wb
 * reset test, checking sanity stuff like unit=0, API in API, main thread
 * not during warmboot or detach
 */
int
dnxc_wb_all_reset_test_utils_preconditions(
    int unit)
{
    int warmboot_test_mode_enable = DNXC_WARM_BOOT_API_TEST_MODE_NONE;

    /*
     * some basic sanity conditions 
     */
    if (!SOC_UNIT_VALID(unit))
    {
        return 0;
    }
    if (!SOC_IS_DONE_INIT(unit))
    {
        return 0;
    }
    if (SOC_WARM_BOOT(unit))
    {
        return 0;
    }
    if (SOC_IS_DETACHING(unit))
    {
        return 0;
    }

    /*
     * wb test mode should be enabled 
     */
    dnxc_wb_test_mode_get(unit, &warmboot_test_mode_enable);
    if (DNXC_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API != warmboot_test_mode_enable)
    {
        return 0;
    }

    /*
     * must be main thread 
     */
    if (sal_thread_self() != dnxc_wb_main_thread[unit])
    {
        return 0;
    }

    /*
     * check that we are not in nested API (API in API) 
     */
    if (!dnxc_wb_api_test_counter_is_top_level(unit))
    {
        return 0;
    }

    return 1;
}
/* 
 * check pre-conditions that apply for WB regression
 * 0 - don't hold
 * 1 - preconditions hold
 */
int
dnxc_wb_common_api_preconditions(
    int unit)
{
    int disable_once;

    if (!dnxc_wb_all_reset_test_utils_preconditions(unit))
    {
        return 0;
    }

    /*
     * check if the test is disabled 
     */
    if (dnxc_wb_override_wb_test[unit] > 0)
    {
        return 0;
    }

    /*
     * check if we were signaled not to perform any resets 
     */
    dnxc_wb_disable_once_get(unit, &disable_once);
    if (disable_once)
    {
        return 0;
    }

    /*
     * all good! 
     */
    return 1;
}

/* set filters to perform wb api test only on a range of APIs */
void
dnxc_wb_api_test_filter_set(
    int unit,
    uint8 low_high,
    uint32 value)
{
    if (low_high > 1)
    {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit, "Unit:%d wrong value (%d) for wb api test filter\n"), unit, low_high));
    }

    dnxc_wb_counter_filter[unit][low_high] = value;
}

/* in case WB sequence was skipped once, */
/* setting the flag to '0' in order that next time WB sequence will occur  */
void
dnxc_wb_api_test_enable_if_disabled_once(
    int unit)
{
    int disable_once;

    assert(dnxc_wb_all_reset_test_utils_preconditions(unit));

    dnxc_wb_disable_once_get(unit, &disable_once);
    if (1 == disable_once)
    {
        dnxc_wb_disable_once_set(unit, 0);
    }
    return;
}

/*!
 * \brief dnxc_warm_boot_api_test_reset
 *
 * This function simulate warm boot via calling tr 141
 * only at the end of API when running in warm boot mode regression.
 *
 * \param [in] unit
 *
 * \retval 0  - tr 141 finished successfully
 * \retval -1  - Warm boot test failed
 *
 */
int
dnxc_warm_boot_api_test_reset(
    int unit)
{

    int test_counter;
    int warmboot_test_mode_enable;
    int nof_skip = 0;

    /*
     * if not wb test mode, just exit without error 
     */
    dnxc_wb_test_mode_get(unit, &warmboot_test_mode_enable);
    if (DNXC_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API != warmboot_test_mode_enable)
    {
        return 0;
    }

    if (!dnxc_wb_all_reset_test_utils_preconditions(unit))
    {
        return 0;
    }

    /*
     * if preconditions for reset don't hold just ack and exit 
     */
    if (!dnxc_wb_common_api_preconditions(unit))
    {
        dnxc_wb_api_test_enable_if_disabled_once(unit);
        return 0;
    }

    /*
     * increase reset test counter 
     */
    /*
     * note that counter is being increased only if preconditions hold 
     */
    dnxc_wb_test_counter_plus_plus(unit);

    /*
     * do exponential backoff, decrease frequency of reset tests 
     */
    /*
     * don't back off for ramon 
     */
    dnxc_wb_test_counter_get(unit, &test_counter);
    if (!dnxc_wb_test_exp_backoff(test_counter, &nof_skip))
    {
        if (nof_skip)
        {
            LOG_WARN(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                                 "  --> *** WB BCM API %s *** skipping %d warm reboot tests for faster regression\n"),
                      dnxc_warmboot_api_function_name[unit], nof_skip));
        }
        return 0;
    }

    if (dnxc_wb_counter_filter[unit][0] > test_counter)
    {
        LOG_WARN(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit,
                             "  --> *** WB BCM API %s *** skipping warm reboot test#%d because of user filter (%d<%d)\n"),
                  dnxc_warmboot_api_function_name[unit], test_counter, test_counter, dnxc_wb_counter_filter[unit][0]));
        return 0;
    }

    if (dnxc_wb_counter_filter[unit][1] > 0 && dnxc_wb_counter_filter[unit][1] < test_counter)
    {
        LOG_WARN(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit,
                             "  --> *** WB BCM API %s *** skipping warm reboot test#%d because of user filter (%d>%d)\n"),
                  dnxc_warmboot_api_function_name[unit], test_counter, test_counter, dnxc_wb_counter_filter[unit][0]));
        return 0;
    }

    LOG_WARN(BSL_LS_BCM_COMMON,
             (BSL_META_U(unit,
                         "**** WB BCM API %s **** (test counter: %d) ****\n"),
              dnxc_warmboot_api_function_name[unit], test_counter));

    LOG_DEBUG(BSL_LS_BCM_COMMON, (BSL_META_U(unit, "Unit:%d Starting warm reboot test\n"), unit));

#ifdef BCM_DNX_SUPPORT
    /*
     * In order to mantain satble state suppress the Err Recovery and State Comparison during the warmboot.
     */
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    DNX_STATE_COMPARISON_SUPPRESS(unit, TRUE);
#endif /* BCM_DNX_SUPPORT */

    /*
     * perform the reset test 
     */
    if (dnxc_wb_api_test_tr_141_command_function_ptr[unit] == NULL ||
        (dnxc_wb_api_test_tr_141_command_function_ptr[unit]) (unit) != 0)
    {

        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit, "Unit:%d Warm reboot test failed\n"), unit));
#ifdef BCM_DNX_SUPPORT
        /*
         * Remove the suppress of the Err Recovery and State Comparison after the warmboot.
         */
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        DNX_STATE_COMPARISON_SUPPRESS(unit, FALSE);
#endif /* BCM_DNX_SUPPORT */
        return -1;
    }
    else
    {
        LOG_WARN(BSL_LS_BCM_COMMON, (BSL_META_U(unit, "Unit:%d Warm reboot test finish successfully\n"), unit));
        if (dnxc_wb_test_callback[unit].dnxc_wb_test_callback_func != NULL)
        {
            if (dnxc_wb_test_callback[unit].dnxc_wb_test_callback_func
                (unit, dnxc_wb_test_callback[unit].dnxc_wb_test_callback_data) != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit, "Unit:%d Warm reboot test, callback function failed\n"), unit));
                return -1;
            }
        }
    }
#ifdef BCM_DNX_SUPPORT
    /*
     * Remove the suppress of the Err Recovery and State Comparison after the warmboot.
     */
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    DNX_STATE_COMPARISON_SUPPRESS(unit, FALSE);
#endif /* BCM_DNX_SUPPORT */

    return 0;
}

void
dnxc_warmboot_test_tr_141_command_set(
    int unit,
    int (*function_ptr) (int unit))
{
    dnxc_wb_api_test_tr_141_command_function_ptr[unit] = function_ptr;
}

/* 
 * allowed values for warmboot_test_mode_set/get: 
 * ---------------------------------------------- 
 * 0:WB_TEST_MODE_NONE
 * 1:WB_TEST_MODE_AFTER_EVERY_API
 */

void
dnxc_wb_test_mode_set(
    int unit,
    int enable)
{
    /*
     * Set WB test main thread.
     */
    dnxc_wb_test_main_thread_set(unit);

    dnxc_wb_warmboot_test_mode[unit] = enable;
}

void
dnxc_wb_test_mode_get(
    int unit,
    int *enable)
{
    *enable = dnxc_wb_warmboot_test_mode[unit];
}

void
dnxc_wb_test_counter_get(
    int unit,
    int *counter)
{
    assert(dnxc_wb_all_reset_test_utils_preconditions(unit));
    *counter = dnxc_wb_warmboot_test_counter[unit];
}

void
dnxc_wb_test_counter_plus_plus(
    int unit)
{
    assert(dnxc_wb_all_reset_test_utils_preconditions(unit));
    dnxc_wb_warmboot_test_counter[unit]++;
}

/* the function below set a flag that override the wb test mode
 * i.e if test mode is on (perform warm rebooot at the end of APIs) turning this
 * flag on will instruct the driver to not perform the reboots  */
void
dnxc_wb_no_wb_test_set(
    int unit,
    int wb_flag)
{
    if (wb_flag == 0)
    {
        dnxc_wb_override_wb_test[unit]--;
    }
    else
    {
        dnxc_wb_override_wb_test[unit]++;
    }
}

void
dnxc_wb_no_wb_test_get(
    int unit,
    int *override_counter)
{
    *override_counter = dnxc_wb_override_wb_test[unit];
}

/*
 * The function below set a flag that mark the deinit-init sequence if it was run during WB test.
 */
void
dnxc_wb_deinit_init_during_wb_test_set(
    int unit,
    int wb_flag)
{
    if (wb_flag == TRUE)
    {
        dnxc_wb_deinit_init_during_wb_test[unit]++;
    }
    else
    {
        dnxc_wb_deinit_init_during_wb_test[unit]--;
    }
}

/*
 * Gets if the warmboot test mode was overridden.
 */
int
dnxc_wb_deinit_init_during_wb_test_get(
    int unit)
{
    return dnxc_wb_deinit_init_during_wb_test[unit];
}

/* the two functions below set\get a flag that disable the wb test mode
 * i.e if test mode is on (perform warm rebooot at the end of APIs) turning this
 * flag on will instruct the driver to not perform the when the BCM API finish to run, 
 * should be used by APIs that
 * create a mismatch between SW state and HW state, for example: field instructions saved to SW
 * but not yet commited to HW should turn on this flag. */

void
dnxc_wb_disable_once_set(
    int unit,
    int wb_flag)
{
    assert(dnxc_wb_all_reset_test_utils_preconditions(unit));
    dnxc_wb_disable_once_wb_test[unit] = wb_flag;
}

void
dnxc_wb_disable_once_get(
    int unit,
    int *wb_flag)
{
    assert(dnxc_wb_all_reset_test_utils_preconditions(unit));
    *wb_flag = dnxc_wb_disable_once_wb_test[unit];
}

int
dnxc_wb_test_mode_skip_wb_sequence(
    int unit)
{
    int rv = 0;

    if (dnxc_wb_all_reset_test_utils_preconditions(unit))
    {
        dnxc_wb_disable_once_set(unit, 1);
    }
    return rv;
}

/* return 1 if test should be performed or 0 if test should be masked */
/* return in nof_skip the number of tests we are going to skip (if it's the first of them) */
int
dnxc_wb_test_exp_backoff(
    int test_counter,
    int *nof_skip)
{
    int i = DNXC_WARM_BOOT_EXP_PARAM;
    int skip = 0;

    while (i < test_counter)
    {
        i *= DNXC_WARM_BOOT_EXP_PARAM;
    }

    /*
     * Temporarily skip half of the tests in the base of the exponent, this is to allow more tests to run until we have 
     * more devices in regression 
     */
    if ((test_counter < DNXC_WARM_BOOT_EXP_PARAM) && (test_counter >= DNXC_WARM_BOOT_EXP_PARAM / 2))
    {
        if (nof_skip != NULL)
        {
            *nof_skip = 0;
            if (test_counter == DNXC_WARM_BOOT_EXP_PARAM / 2)
            {
                *nof_skip = DNXC_WARM_BOOT_EXP_PARAM / 2;
            }
        }
        return 0;
    }

    if (test_counter % (i / DNXC_WARM_BOOT_EXP_PARAM) == 0 || test_counter == 0)
    {
        return 1;
    }

    skip = i / DNXC_WARM_BOOT_EXP_PARAM - test_counter % (i / DNXC_WARM_BOOT_EXP_PARAM);
    if (nof_skip != NULL && skip + 1 == i / DNXC_WARM_BOOT_EXP_PARAM)
        *nof_skip = skip;
    else if (nof_skip != NULL)
        *nof_skip = 0;
    return 0;
}

int
dnxc_wb_api_test_counter_increase(
    int unit)
{
    int mode = 0;

    if (!(SOC_UNIT_VALID(unit) && SOC_IS_DONE_INIT(unit) && (!SOC_WARM_BOOT(unit)) && (!SOC_IS_DETACHING(unit))))
    {
        return 0;
    }

    /*
     * do only for main thread 
     */
    if (sal_thread_self() != dnxc_wb_main_thread[unit])
    {
        return 0;
    }

    /*
     * don't update counter if not in warmboot regression 
     */
    dnxc_wb_test_mode_get(unit, &mode);
    if (mode != DNXC_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API)
    {
        return 0;
    }

    dnxc_wb_nested_api_counter[unit]++;

    assert(dnxc_wb_nested_api_counter[unit] >= 0);
    assert(dnxc_wb_nested_api_counter[unit] < 4);

    return 0;
}

int
dnxc_wb_api_test_counter_decrease(
    int unit)
{
    int mode = 0;

    if (!(SOC_UNIT_VALID(unit) && SOC_IS_DONE_INIT(unit) && (!SOC_WARM_BOOT(unit)) && (!SOC_IS_DETACHING(unit))))
    {
        return 0;
    }

    /*
     * do only for main thread 
     */
    if (sal_thread_self() != dnxc_wb_main_thread[unit])
    {
        return 0;
    }

    /*
     * don't update counter if not in warmboot regression 
     */
    dnxc_wb_test_mode_get(unit, &mode);
    if (mode != DNXC_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API)
    {
        return 0;
    }

    /*
     * return an error if outside a transaction 
     */
    assert(dnxc_wb_nested_api_counter[unit] > 0);

    dnxc_wb_nested_api_counter[unit]--;

    assert(dnxc_wb_nested_api_counter[unit] >= 0);
    assert(dnxc_wb_nested_api_counter[unit] < 4);

    return 0;
}

int
dnxc_wb_api_test_counter_get(
    int unit)
{
    if (!(SOC_UNIT_VALID(unit) && SOC_IS_DONE_INIT(unit) && (!SOC_WARM_BOOT(unit)) && (!SOC_IS_DETACHING(unit))))
    {
        return 1;
    }

    if (sal_thread_self() != dnxc_wb_main_thread[unit])
    {
        return 1;
    }

    assert(dnxc_wb_nested_api_counter[unit] >= 0);
    assert(dnxc_wb_nested_api_counter[unit] < 4);

    return dnxc_wb_nested_api_counter[unit];
}

int
dnxc_wb_api_test_counter_is_top_level(
    int unit)
{
    /*
     * we don't expect counter to be zero - should always be at least inside one API 
     */
    if (dnxc_wb_api_test_counter_get(unit) == 0)
    {
        return 0;
    }

    return (dnxc_wb_api_test_counter_get(unit) == 1);
}

void
dnxc_wb_test_main_thread_set(
    int unit)
{
    dnxc_wb_main_thread[unit] = sal_thread_main_get();
    return;
}

void
dnxc_wb_test_callback_register(
    int unit,
    uint32 flags,
    dnxc_wb_test_callback_func_t callback,
    void *userdata)
{
    if (dnxc_wb_test_callback[unit].dnxc_wb_test_callback_func != NULL)
    {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit, "Unit:%d Warm reboot test, callback function already exist\n"), unit));
        assert(0);
    }
    dnxc_wb_test_callback[unit].dnxc_wb_test_callback_func = callback;
    dnxc_wb_test_callback[unit].dnxc_wb_test_callback_data = userdata;
}

void
dnxc_wb_test_callback_unregister(
    int unit)
{
    dnxc_wb_test_callback[unit].dnxc_wb_test_callback_func = NULL;
    dnxc_wb_test_callback[unit].dnxc_wb_test_callback_data = NULL;
}

#endif /* BCM_WARM_BOOT_SUPPORT && BCM_WARM_BOOT_API_TEST */
