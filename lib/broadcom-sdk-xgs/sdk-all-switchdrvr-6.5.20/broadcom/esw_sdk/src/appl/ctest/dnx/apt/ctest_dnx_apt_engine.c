/*
 * ! \file ctest_dnx_apt_engine.c
 * Contains all of the API performance infrastructure engine functions.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
#include <shared/bsl.h>

/*
 * Include files.
 * {
 */

#include <sal/core/boot.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnxc/dnxc_regression_utils.h>
#include <soc/dnx/kbp/kbp_common.h>
#include "ctest_dnx_apt.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define DNX_APT_SH_CMD_EXEC_AND_PRINT(_cmd_) \
    LOG_CLI((BSL_META("%s\n"), _cmd_)); sh_process_command(unit, _cmd_)

/** Utility macro for checking and executing test object callbacks and printing custom error message and exit */
#define DNX_APT_OBJECT_CB_IF_ERR_EXIT(_cb_, _cb_params_, _format_, ...) \
    if(_cb_) { \
        SHR_SET_CURRENT_ERR((*_cb_)_cb_params_); \
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE) { \
            LOG_CLI((BSL_META(_format_), ##__VA_ARGS__)); \
            SHR_EXIT(); \
        } \
    }

/** Utility macro for checking and executing test object callbacks and printing custom error message and no exit */
#define DNX_APT_OBJECT_CB_IF_ERR_NO_EXIT(_cb_, _cb_params_, _format_, ...) \
    if(_cb_) { \
        shr_error_e rv = (*_cb_)_cb_params_; \
        if (rv != _SHR_E_NONE) { \
            SHR_SET_CURRENT_ERR(rv); \
            LOG_CLI((BSL_META(_format_), ##__VA_ARGS__)); \
        } \
    }

/*
 * }
 */

/*
 * GLOBALs
 * {
 */

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/*
 * API indicating whether the compilation is supported for the API performance tests.
 * Returns TRUE if Intel-GTS-OPT.
 * Returns FALSE otherwise.
 */
uint8
dnx_apt_is_compilation_supported(
    void)
{
#if defined(__DUNE_GTS_BCM_CPU__) && defined(__OPTIMIZE__)
    return TRUE;
#else
    return FALSE;
#endif
}

/*
 * Initialize the APT engine:
 *      Disable threads
 *      Disable error recovery
 *      Initialize required data structures (currently only "execution_time")
 */
static shr_error_e
dnx_apt_engine_init(
    int unit,
    int **execution_time,
    const dnx_apt_object_def_t * test_object)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Disable any prints in the following block to reduce unnecessary output */
    {
        bsl_severity_t severity;

        SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityError);

        LOG_CLI((BSL_META("Disable threads\n")));
        DNX_APT_SH_CMD_EXEC_AND_PRINT("linkscan off");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("counter off");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("deinit rx_los");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("deinit interrupt");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("ctrp detach");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("crps detach");

        LOG_CLI((BSL_META("Disable error recovery\n")));
        DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);
    }

    SHR_ALLOC_SET_ZERO(*execution_time, (sizeof(int) * test_object->number_of_runs), "execution_time", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Denitialize the APT engine:
 *      Free required data structures (currently only "execution_time")
 *      Free custom_data (if initialized by the user)
 *      Free unique random (if initialized by the user)
 *      Enable error recovery
 *      Enable threads
 */
static void
dnx_apt_engine_deinit(
    int unit,
    int *execution_time,
    void *custom_data)
{
    SHR_FREE(execution_time);
    SHR_FREE(custom_data);
    dnx_apt_unique_random_free();

    /** Disable any prints in the following block to reduce unnecessary output */
    {
        bsl_severity_t severity;

        SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityError);

        LOG_CLI((BSL_META("Enable error recovery\n")));
        DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);

        LOG_CLI((BSL_META("Enable threads\n")));
        DNX_APT_SH_CMD_EXEC_AND_PRINT("crps attach");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("ctrp attach");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("init interrupt");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("init rx_los");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("counter on");
        DNX_APT_SH_CMD_EXEC_AND_PRINT("linkscan on");

        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, severity);
    }
}

/*
 * Validates whether the test is suppoprted on this device configuration based on the test support flags.
 * Returns _SHR_E_UNAVAIL if not supported.
 * Returns _SHR_E_NONE if supported.
 * All other errors indicate actual error.
 */
static shr_error_e
dnx_apt_engine_test_support_flags_check(
    int unit,
    dnx_apt_support_e support_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        LOG_CLI((BSL_META("Test is not supported on adapter\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
        SHR_EXIT();
    }
    if (!_SHR_IS_FLAG_SET(support_flags, DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP))
    {
        if (_SHR_IS_FLAG_SET(support_flags, DNX_APT_SUPPORT_INTERNAL_KBP_REQUIRED) && !dnx_kbp_device_enabled(unit))
        {
            LOG_CLI((BSL_META("Test requires KBP\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            SHR_EXIT();
        }
        else if (!_SHR_IS_FLAG_SET(support_flags, DNX_APT_SUPPORT_INTERNAL_KBP_REQUIRED) &&
                 dnx_kbp_device_enabled(unit))
        {
            LOG_CLI((BSL_META("Test requires that KBP is not enabled\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Validates whether the test is supported using the support flags and the custom_support_check object function.
 * Returns _SHR_E_UNAVAIL if not supported.
 * Returns _SHR_E_NONE if supported.
 * All other errors indicate actual error.
 */
shr_error_e
dnx_apt_test_support_check(
    int unit,
    const dnx_apt_object_def_t * test_object)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_apt_engine_test_support_flags_check(unit, test_object->support_flags));
    DNX_APT_OBJECT_CB_IF_ERR_EXIT(test_object->custom_support_check, (unit), "Test %s\n",
                                  SHR_GET_CURRENT_ERR() != _SHR_E_UNAVAIL ?
                                  "custom_support_check failed" : "is not supported on current device configuration");
exit:
    SHR_FUNC_EXIT;
}

/** API for returning validation method string */
const char *
dnx_apt_validation_method_string(
    dnx_apt_validation_method_e validation_method)
{
    switch (validation_method)
    {
        case DNX_APT_VALIDATION_METHOD_WORST_RUN:
        {
            return "Worst run";
        }
        case DNX_APT_VALIDATION_METHOD_BEST_RUN:
        {
            return "Best run";
        }
        case DNX_APT_VALIDATION_METHOD_AVERAGE_ALL:
        {
            return "Average all runs";
        }
        case DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST:
        {
            return "Stable average";
        }
        case DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS:
        {
            /** Calculates average, but checks passing runs */
            return "At least half runs pass";
        }
        case DNX_APT_VALIDATION_METHOD_SKIP:
        {
            /** Calculates average, but skips validation. */
            return "Average all runs (skip)";
        }
        default:
        {
            return "unknown validation method";
        }
    }
}

/*
 * API for validating the performance results and deciding whether the test pass of fail based on these results.
 * Validation of the results is performed based on test_object->validation_method.
 */
static shr_error_e
dnx_apt_engine_test_performance(
    int unit,
    int *execution_time,
    const dnx_apt_object_def_t * test_object)
{
    int run = 0;
    double validation_time = 0;
    int passing_runs = 0;
    int half_runs = (test_object->number_of_runs / 2) + (test_object->number_of_runs % 2);
    int fail_validation = TRUE;
    int fail_compare_sign = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    /** Calculate passing runs */
    for (run = 0; run < test_object->number_of_runs; run++)
    {
        if (((double) execution_time[run] / (double) test_object->number_of_calls) <= test_object->time_threshold(unit))
        {
            passing_runs++;
        }
    }

    /** Calculate the validation time */
    switch (test_object->validation_method)
    {
        case DNX_APT_VALIDATION_METHOD_WORST_RUN:
        {
            /** Taking only the worst result */
            int worst_run = 0;
            for (run = 0; run < test_object->number_of_runs; run++)
            {
                if (execution_time[worst_run] > execution_time[run])
                {
                    worst_run = run;
                }
            }
            validation_time = (double) execution_time[worst_run] / (double) test_object->number_of_calls;
            break;
        }
        case DNX_APT_VALIDATION_METHOD_BEST_RUN:
        {
            /** Taking only the best result */
            int best_run = 0;
            for (run = 0; run < test_object->number_of_runs; run++)
            {
                if (execution_time[best_run] < execution_time[run])
                {
                    best_run = run;
                }
            }
            validation_time = (double) execution_time[best_run] / (double) test_object->number_of_calls;
            break;
        }
        case DNX_APT_VALIDATION_METHOD_AVERAGE_ALL:
        {
            /** Taking the average of all runs */
            for (run = 0; run < test_object->number_of_runs; run++)
            {
                validation_time += execution_time[run];
            }
            validation_time /= (double) (test_object->number_of_runs * test_object->number_of_calls);
            break;
        }
        case DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST:
        {
            /** Trims (about) 1/4 of the worst and (about) 1/4 of the best results and calculate more stable average */

            /** Calculate the number of runs to ignore */
            int trim_runs = ((test_object->number_of_runs / 2) - ((test_object->number_of_runs / 2) % 2)) / 2;

            /** Order the execution time per run in accending order */
            int order_index;
            for (order_index = 0; order_index < test_object->number_of_runs - 1; order_index++)
            {
                int swap_index = order_index;
                for (run = order_index + 1; run < test_object->number_of_runs; run++)
                {
                    if (execution_time[run] < execution_time[swap_index])
                    {
                        swap_index = run;
                    }
                }
                if (swap_index != order_index)
                {
                    int swap_execution_time = execution_time[swap_index];
                    execution_time[swap_index] = execution_time[order_index];
                    execution_time[order_index] = swap_execution_time;
                }
            }
            for (run = trim_runs; run < test_object->number_of_runs - trim_runs; run++)
            {
                validation_time += execution_time[run];
            }
            validation_time /= (double) ((test_object->number_of_runs - trim_runs * 2) * test_object->number_of_calls);
            break;
        }
        case DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS:
        {
            /** Taking the average of all runs */
            for (run = 0; run < test_object->number_of_runs; run++)
            {
                validation_time += execution_time[run];
            }
            validation_time /= (double) (test_object->number_of_runs * test_object->number_of_calls);
            break;
        }
        case DNX_APT_VALIDATION_METHOD_SKIP:
        {
            /** Taking the average of all runs */
            for (run = 0; run < test_object->number_of_runs; run++)
            {
                validation_time += execution_time[run];
            }
            validation_time /= (double) (test_object->number_of_runs * test_object->number_of_calls);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported validation method was provided (%d).\n",
                         test_object->validation_method);
            break;
        }
    }

    /** Preform validation only on the supported compilation (Intel-GTS_OPT). */
    if (dnx_apt_is_compilation_supported() && (test_object->validation_method != DNX_APT_VALIDATION_METHOD_SKIP))
    {
        fail_compare_sign = (validation_time <= test_object->time_threshold(unit) ? FALSE : TRUE);

        if (test_object->validation_method == DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS)
        {
            fail_validation = (passing_runs < half_runs ? TRUE : FALSE);
        }
        else
        {
            fail_validation = fail_compare_sign;
        }

        /** Print results */
        LOG_CLI((BSL_META("Test \"%s\" results:\n"
                          "\tValidating: %s\n"
                          "\tExecution time %.3fus %s %.3fus time threshold.\n"
                          "\tExecution rate %d %s %d calls per second\n"
                          "\tPassing runs %d/%d\n"
                          "\tResult: (%s).\n"),
                 test_object->test_name, dnx_apt_validation_method_string(test_object->validation_method),
                 validation_time, (fail_compare_sign ? ">" : "<="), test_object->time_threshold(unit),
                 DNX_APT_TIME_TO_RATE(validation_time), (fail_compare_sign ? "<" : ">="),
                 DNX_APT_TIME_TO_RATE(test_object->time_threshold(unit)), passing_runs, test_object->number_of_runs,
                 fail_validation ? "FAIL" : "PASS"));

        /** Print time to the regression DB */
        dnxc_regression_utils_advanced_print("APT_RESULT", "D %.3f", validation_time);
    }
    /** Validation is skipped. */
    else
    {
        fail_validation = FALSE;

        /** Print results */
        LOG_CLI((BSL_META
                 ("Test \"%s\" results:\n\tAverage execution time %.3fus / rate %d calls per second (Not validated).\n"),
                 test_object->test_name, validation_time, DNX_APT_TIME_TO_RATE(validation_time)));
    }

    SHR_SET_CURRENT_ERR(fail_validation ? _SHR_E_FAIL : _SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/*
 * API for executing single API performance test from a test object.
 * Flow:
 *      1. Test support check
 *      2. Engine and test initializations
 *      3. Test execution X number of runs
 *          3.1. Initialization of the run (prepare everything required by the tested API)
 *          3.2. Start measurement
 *          3.3. Run tested API Y times
 *          3.4. Stop measurement and store the execution time
 *          3.5. Deinitialization of the run (clean-up everything done in this run)
 *      3. Results validation
 *      4. Engine and test deinitializations
 */
shr_error_e
dnx_apt_engine_test_run(
    int unit,
    const dnx_apt_object_def_t * test_object)
{
    int run = 0;
    int call = 0;
    int start_time = 0;
    int is_engine_init_executed = FALSE;
    int is_test_init_executed = FALSE;
    int *execution_time = NULL;
    void *custom_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("\nTest \"%s\" Start. %d runs %d calls.\n"), test_object->test_name, test_object->number_of_runs,
             test_object->number_of_calls));

    /** Test support validation */
    SHR_IF_ERR_EXIT_NO_MSG(dnx_apt_test_support_check(unit, test_object));

    /** Test engine initialization */
    SHR_IF_ERR_EXIT(dnx_apt_engine_init(unit, &execution_time, test_object));
    is_engine_init_executed = TRUE;

    /** Test initialization */
    DNX_APT_OBJECT_CB_IF_ERR_EXIT(test_object->init, (unit, test_object->number_of_calls, &custom_data), "%s\n",
                                  "Test init failed");
    is_test_init_executed = TRUE;

    /** Test run */
    for (run = 0; run < test_object->number_of_runs; run++)
    {
        /** Test run initialization */
        DNX_APT_OBJECT_CB_IF_ERR_EXIT(test_object->pre_execute, (unit, test_object->number_of_calls, custom_data),
                                      "Test pre execute failed on run %d\n.", run);

        /** Test execution and time measurement */
        start_time = sal_time_usecs();
        for (call = 0; call < test_object->number_of_calls; call++)
        {
            DNX_APT_OBJECT_CB_IF_ERR_EXIT(test_object->execute, (unit, call, custom_data),
                                          "Test execute failed on run %d call %d.\n", run, call);
        }
        execution_time[run] = sal_time_usecs() - start_time;

        /** Prints to indicate that something is actually happening */
        LOG_CLI((BSL_META(".")));
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U
                   (unit, "DEBUG: Run %d measured average execution time %.3fus / rate %d calls per second\n"), run,
                   ((double) execution_time[run] / (double) test_object->number_of_calls),
                   DNX_APT_TIME_TO_RATE(((double) execution_time[run] / (double) test_object->number_of_calls))));

        /** Test run deinitialization */
        DNX_APT_OBJECT_CB_IF_ERR_EXIT(test_object->post_execute, (unit, test_object->number_of_calls, custom_data),
                                      "Test post execute failed on run %d\n.", run);

    }
    LOG_CLI((BSL_META("\n")));

    SHR_SET_CURRENT_ERR(dnx_apt_engine_test_performance(unit, execution_time, test_object));

exit:
    /** Test deinitialization */
    if (is_test_init_executed)
    {
        DNX_APT_OBJECT_CB_IF_ERR_NO_EXIT(test_object->deinit, (unit, test_object->number_of_calls, custom_data),
                                         "%s\n", "Test deinit failed");
    }
    /** Test engine deinitialization */
    if (is_engine_init_executed)
    {
        dnx_apt_engine_deinit(unit, execution_time, custom_data);
    }
    LOG_CLI((BSL_META("Test \"%s\" Finished. (%s)\n"), test_object->test_name,
             (SHR_GET_CURRENT_ERR() == _SHR_E_NONE ? "PASS" : SHR_GET_CURRENT_ERR() ==
              _SHR_E_UNAVAIL ? "NO_SUPPORT" : "FAIL")));
    SHR_FUNC_EXIT;
}

/*
 * }
 */
