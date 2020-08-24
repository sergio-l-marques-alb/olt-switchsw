/** \file dnx_er_regression.c
 * This module is related to Error Recovery regression.
 */
/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>

#include <soc/drv.h>

#include <soc/dnx/dnx_er_regression.h>

#include <soc/dnx/dnx_err_recovery_manager_common.h>

#include <soc/dnx/dnx_er_threading.h>

#include <soc/dnx/dnx_err_recovery_manager.h>

#include <soc/dnx/dnx_state_snapshot_manager.h>

#include <soc/dnxc/dnxc_generic_state_journal.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

#include <soc/dnx/kbp/kbp_common.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

extern err_recovery_manager_t dnx_err_recovery_manager[SOC_MAX_NUM_DEVICES];

dnx_er_regress_t dnx_er_regression[SOC_MAX_NUM_DEVICES] = { {0}
};

/*
 * see .h file for description
 */
static uint8
dnx_er_regress_is_top_level_api(
    int unit)
{
    return (1 == dnx_err_recovery_manager[unit].api_counter);
}

/*
 * see .h file for description
 */
static uint8
dnx_er_regress_is_top_level_api_transaction(
    int unit)
{
    /*
     * retrieve transaction number before top level API was ran.
     */
    uint32 trans_num_before_api = dnx_err_recovery_manager[unit].api_to_trans_map[0];

    /*
     * check if the current transaction is the first transaction internal to the top level BCM API
     */
    return (dnx_err_recovery_manager[unit].transaction_counter == (trans_num_before_api + 1));
}

static uint8
dnx_er_regress_is_outside_of_transaction(
    int unit)
{
    return (0 == dnx_err_recovery_manager[unit].transaction_counter);
}

/*
 * \brief - check if all testing conditions are met
 */
static uint8
dnx_er_regress_testing_conditions_are_met(
    int unit)
{
    return (SOC_UNIT_VALID(unit)
            && SOC_IS_DONE_INIT(unit)
            && !SOC_IS_DETACHING(unit)
            && dnx_err_recovery_common_flag_mask_is_on(unit,
                                                       DNXC_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                                       DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED
                                                       | DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED
                                                       | DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED)
            && !dnxc_rollback_journal_is_any_journal_rolling_back(unit)
            && dnx_er_threading_is_main_thread(unit)
            && dnx_er_regress_is_top_level_api(unit)
            && dnx_er_regression[unit].is_appl_init_done
            && dnx_er_regression[unit].is_testing_enabled && (!dnx_er_regression[unit].is_api_excluded));
}

/*
 * \brief - Check for swstate and hw access violations outside of err recovery transaction in top level (non-nested) BCM APIs.
 *  Utilized by journal functions called during swstate and hw access.
 */
static void
dnx_er_regress_system_state_modified(
    int unit)
{
    /*
     * Violation check should be performed only for regression testing and the bypass flag has not been raised.
     * Mark swstate or hw access in a non-nested BCM API outside of transactions.
     * - Not a violation in cases where no transactions in BCM API.
     * - Not a violation in cases where BCM API is in a "test cleanup" transaction.
     */
    if (dnx_er_regress_testing_conditions_are_met(unit) && dnx_er_regress_is_outside_of_transaction(unit))
    {
        dnx_er_regression[unit].has_access_outside_trans = TRUE;

        /*
         * violation if at least one transaction was already started / ended in the current BCM API
         */
        if (0 != dnx_er_regression[unit].top_level_trans_in_api_count)
        {
            LOG_CLI((BSL_META_U
                     (unit, "error recovery regression ERROR: hw or swstate access outside of transaction\n")));
            assert(0);
        }
    }
}

/**
 * \brief - check if error recovery suppression flag is on
 */
static uint8
dnx_err_recovery_regress_is_err_type_suppressed(
    int unit,
    dnx_err_recovery_regress_error_t type)
{
    return (dnx_er_regression[unit].err_type_suppress_counters[type] > 0);
}

/*
 * check if this is the first transaction in a top level BCM API
 */
static uint8
dnx_err_recovery_is_top_level_trans_in_top_level_api(
    int unit)
{
    return (dnx_er_regress_is_top_level_api(unit) && dnx_er_regress_is_top_level_api_transaction(unit));
}

/*
 * \brief - returns true if this is the first test iteration of the BCM API that has opted in for error recovery
 */
static uint8
dnx_er_regress_is_first_test_iteration(
    int unit)
{
    return (dnx_er_regress_testing_conditions_are_met(unit)
            && dnx_err_recovery_is_top_level_trans_in_top_level_api(unit) && (!dnx_er_regression[unit].is_api_ran));
}

/*
 * \brief - returns true if this is the second test iteration of the BCM API that has opted in for error recovery
 */
static uint8
dnx_er_regress_is_second_test_iteration(
    int unit)
{
    return (dnx_er_regress_testing_conditions_are_met(unit)
            && dnx_err_recovery_is_top_level_trans_in_top_level_api(unit) && (dnx_er_regression[unit].is_api_ran));
}

/*
 * \brief - print API name and test iteration counter
 */
static void
dnx_er_regress_print_api_and_iteration(
    int unit,
    uint8 is_begin)
{
    if (is_begin)
    {
        if (dnx_er_regress_is_first_test_iteration(unit))
        {
            LOG_CLI((BSL_META_U(unit, "**** ER BCM API %s **** ERROR SIMULATION RUN ****\n"),
                     dnx_er_regression[unit].dnx_er_regress_testing_api_name));
        }
        else if (dnx_er_regress_is_second_test_iteration(unit))
        {
            LOG_CLI((BSL_META_U(unit, "**** ER BCM API %s **** NORMAL RUN ****\n"),
                     dnx_er_regression[unit].dnx_er_regress_testing_api_name));
        }
    }
    else
    {
        if (dnx_er_regress_is_second_test_iteration(unit))
        {
            LOG_CLI((BSL_META_U(unit, "**** ER BCM test finished successfully ****\n")));
        }
    }
}

static shr_error_e
dnx_er_regress_init_deinit_counters_reset(
    int unit)
{
    uint32 idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < DNX_ERR_RECOVERY_REGRESS_ERROR_NOF; idx++)
    {
        dnx_er_regression[unit].err_type_suppress_counters[idx] = 0;
    }

    dnx_er_regression[unit].is_appl_init_done = FALSE;

    dnx_er_regression[unit].is_api_excluded = FALSE;
    dnx_er_regression[unit].is_testing_enabled = FALSE;
    dnx_er_regression[unit].has_access_outside_trans = FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_init(
    int unit)
{
    const char *err_recovery_system_name = "ERROR-RECOVERY-REGRESSION";

    SHR_FUNC_INIT_VARS(unit);

    dnx_er_regress_init_deinit_counters_reset(unit);

    if ((NULL != dnx_data_device.regression.regression_parms_get(unit)->system_mode_name)
        && (sal_strncmp(dnx_data_device.regression.regression_parms_get(unit)->system_mode_name,
                        err_recovery_system_name, sal_strlen(err_recovery_system_name)) == 0))
    {
        dnx_er_regression[unit].is_testing_enabled = TRUE;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_regress_init_deinit_counters_reset(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_top_level_api_flags_reset(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_regression[unit].top_level_trans_in_api_count = 0;
    dnx_er_regression[unit].is_api_excluded = FALSE;
    dnx_er_regression[unit].is_api_ran = FALSE;
    dnx_er_regression[unit].has_access_outside_trans = FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_comparison_tmp_allow(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * comparison journal should also be suppressed during the simulation run
     * for each rollback journal suppression.
     */
    if (dnx_er_regress_is_first_test_iteration(unit))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, !is_on));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - violations check before starting a new top-level transaction
 */
static shr_error_e
dnx_er_regress_transaction_start_top_level_verify(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_regression[unit].has_access_outside_trans)
    {
        LOG_CLI((BSL_META_U
                 (unit, "#### #### #### #### ####"
                  "#### Error Recovery regression ERROR: hw or swstate access outside of transaction ####\n"
                  "#### It was detected that the current API opt in to Error Recovery but perform some HW or sw state\n"
                  "     access before it started the transaction or after it was already terminated\n"
                  "     most common cause for it is when an API does not opt in for Error Recovery but calls another\n"
                  "     API that does opt in. to fix it you should either opt in also for the outer API, or alternatively\n"
                  "     use the macro DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN/END on the outer API, more details in confluence page.\n"
                  "#### #### #### ##### ####")));
        assert(0);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - start error recovery transaction
 */
shr_error_e
dnx_er_regress_transaction_start(
    int unit,
    uint8 is_top_level_transaction)
{

    SHR_FUNC_INIT_VARS(unit);

    if (is_top_level_transaction && dnx_er_regress_testing_conditions_are_met(unit))
    {
        SHR_IF_ERR_EXIT(dnx_er_regress_transaction_start_top_level_verify(unit));
    }

    /*
     * comparison journal start to validate rollback after first iteration
     */
    if (dnx_er_regress_is_first_test_iteration(unit))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
    }

    /*
     * print API name and iteration count
     */
    dnx_er_regress_print_api_and_iteration(unit, TRUE);

    /*
     * check for sibling transaction violations, indicate that at least one transaction was started in the top level bcm api
     */
    if (dnx_err_recovery_is_top_level_trans_in_top_level_api(unit))
    {
        dnx_er_regression[unit].top_level_trans_in_api_count++;

        if (dnx_er_regress_testing_conditions_are_met(unit)
            && (dnx_er_regression[unit].top_level_trans_in_api_count > 1))
        {
            LOG_CLI((BSL_META_U
                     (unit,
                      "err recovery ERROR: Multiple top-level transactions in a top level bcm API detected. Either opt-in the API for ER or exclude it from the ER regression algorithm.\n")));
            assert(0);
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_transaction_end_should_abort(
    int unit)
{
    return dnx_er_regress_is_first_test_iteration(unit);
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_transaction_fail_should_return_error(
    int unit)
{
    return dnx_er_regress_testing_conditions_are_met(unit);
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_transaction_end(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * print end of API testing
     */
    dnx_er_regress_print_api_and_iteration(unit, FALSE);

    /*
     * comparison journal validate rollback after first iteration rollback
     */
    if (dnx_er_regress_is_first_test_iteration(unit))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_journal_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * use generic API name for regression testing prints
     */
    dnx_er_regress_testing_api_name_update(unit, "ERROR RECOVERY CLEANUP");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_appl_init_state_change(
    int unit,
    uint8 is_init)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_regression[unit].is_appl_init_done = (is_init) ? TRUE : FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
void
dnx_er_regress_testing_api_name_update(
    int unit,
    const char *func_name)
{
    if (dnx_er_regress_testing_conditions_are_met(unit))
    {
        sal_strncpy(dnx_er_regression[unit].dnx_er_regress_testing_api_name, func_name,
                    DNXC_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH - 1);
    }
}

/*
 * see .h file for description
 */
int
dnx_er_regress_test_should_loop(
    int unit)
{
    /*
     * in case of sibling trnasaction violation we don't want to run the DNX API twice
     */
    if ((dnx_er_regress_testing_conditions_are_met(unit)
         && (1 == dnx_er_regression[unit].top_level_trans_in_api_count) && (!dnx_er_regression[unit].is_api_ran)))
    {
        /*
         * mark that API has been ran and prevent sibling transactions,
         * since the DNX API will be run one more time.
         */
        dnx_er_regression[unit].is_api_ran = TRUE;
        dnx_er_regression[unit].top_level_trans_in_api_count = 0;

        return TRUE;
    }

    return FALSE;
}

/*
 * see .h file for description
 */
uint8
dnx_er_regress_testing_api_test_mode_get(
    int unit)
{
    return dnx_er_regression[unit].is_testing_enabled;
}

/*
 * see .h file for description
 */
void
dnx_er_regress_testing_api_test_mode_change(
    int unit,
    uint8 is_on)
{
    dnx_er_regression[unit].is_testing_enabled = is_on ? TRUE : FALSE;
}

/*
 * see .h file for description
 */
void
dnx_er_regress_testing_api_exclude(
    int unit)
{
    /*
     * - exclusion flag is reset at the end of the top level BCM API
     * - not valid inside a transaction
     */
    if (dnx_er_regress_testing_conditions_are_met(unit) && dnx_er_regress_is_outside_of_transaction(unit))
    {
        dnx_er_regression[unit].is_api_excluded = TRUE;
    }
}

/*
 * see .h file for description
 */
shr_error_e
dnx_er_regress_journal_var(
    int unit,
    uint32 size,
    uint8 *ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_er_regress_is_first_test_iteration(unit))
    {
        SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_memcpy(unit, size, ptr));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_regress_violate_tmp_allow(
    int unit,
    dnx_err_recovery_regress_error_t type,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_er_regress_testing_conditions_are_met(unit))
    {
        SHR_EXIT();
    }

    if (!is_on)
    {
        dnx_er_regression[unit].err_type_suppress_counters[type]++;
    }
    else
    {
        if (dnx_er_regression[unit].err_type_suppress_counters[type] == 0)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "error recovery regression ERROR: Error suppression counter cannot be less than 0.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        dnx_er_regression[unit].err_type_suppress_counters[type]--;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
void
dnx_er_regress_swstate_access_cb(
    int unit)
{
    if (dnx_er_regression[unit].is_testing_enabled
        && !dnx_err_recovery_regress_is_err_type_suppressed(unit,
                                                            DNX_ERR_RECOVERY_REGRESS_ERROR_SW_ACCESS_OUTSIDE_TRANS))
    {
        dnx_er_regress_system_state_modified(unit);
    }
}

/*
 * see .h file for description
 */
void
dnx_er_regress_dbal_access_cb(
    int unit,
    dbal_logical_table_t * table_ptr)
{
    if (dnx_er_regression[unit].is_testing_enabled
        && !dnx_err_recovery_regress_is_err_type_suppressed(unit,
                                                            DNX_ERR_RECOVERY_REGRESS_ERROR_DBAL_ACCESS_OUTSIDE_TRANS))
    {
        dnx_er_regress_system_state_modified(unit);
    }

    /*
     * Validate that no DBAL KBP entries are being inserted while caching mode is ON
     */
#if defined(INCLUDE_KBP)
    if (dnx_er_regress_testing_conditions_are_met(unit)
        && table_ptr->access_method == DBAL_ACCESS_METHOD_KBP && dnx_kbp_device_enabled(unit))
    {
        int l3_route_cache_enabled = 0;
        int field_cache_enabled = 0;

        bcm_switch_control_get(unit, bcmSwitchL3RouteCache, &l3_route_cache_enabled);
        bcm_switch_control_get(unit, bcmSwitchFieldCache, &field_cache_enabled);

        if ((l3_route_cache_enabled && DNX_KBP_IS_TABLE_USED_FOR_FWD(table_ptr->table_type))
            || (field_cache_enabled && DNX_KBP_IS_TABLE_USED_FOR_ACL(table_ptr->table_type)))
        {
            LOG_CLI((BSL_META_U
                     (unit,
                      "error recovery regression ERROR: attempted to start a new top-level transaction when KBP caching is on.\n")));
            assert(0);
        }
    }
#endif /* #ifdef INCLUDE_KBP */
}

#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

#undef _ERR_MSG_MODULE_NAME
