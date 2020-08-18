/** \file dnx_err_recovery_manager.h
 * This module is the Error Recovery manager.
 * It is the interface to the Error Recovery feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_ERR_RECOVERY_MANAGER_H
/* { */
#define _DNX_ERR_RECOVERY_MANAGER_H

/*
 * Include files
 * {
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/dnxc_rollback_journal_utils.h>
#include <soc/dnxc/dnxc_rollback_journal_diag.h>
/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * describes error recovery error suppression types
 */
#define DNX_ERR_RECOVERY_ERR_SUPPRESS_ALL                       (-1)
#define DNX_ERR_RECOVERY_ERR_SUPPRESS_NONE                      SAL_BIT(0)
#define DNX_ERR_RECOVERY_ERR_SUPPRESS_HW_ACCESS                 SAL_BIT(1)
#define DNX_ERR_RECOVERY_ERR_SUPPRESS_INVALIDATE                SAL_BIT(2)
#define DNX_ERR_RECOVERY_ERR_SUPPRESS_SIBLING_TRANSACTIONS      SAL_BIT(3)
/*
 * Relevant for ER regression test mode only,
 * suppressing events that occur OUTSIDE of transactions.
 */
#define DNX_ERR_RECOVERY_ERR_SUPPRESS_SW_ACCESS_OUTSIDE_TRANS   SAL_BIT(4)
#define DNX_ERR_RECOVERY_ERR_SUPPRESS_DBAL_ACCESS_OUTSIDE_TRANS SAL_BIT(5)

/*
 * }
 */

/*
 * MACROs
 * {
 */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

#define DNXC_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH 100

/*
 * \brief - error recovery regression testing header
 */
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_BEGIN(_unit)                     \
    r_rv = BCM_E_NONE;                                                       \
    dnx_err_recovery_regression_testing_api_name_update(_unit, __FUNCTION__);\
    do {

/*
 * \brief - error recovery regression testing footer
 */
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_END(_unit)          \
    } while(dnx_err_recovery_regression_test_counter_inc(_unit) && (BCM_E_NONE == r_rv))

/*
 * \brief - indicates the regression testing should be
 *  skipped for the current BCM API.
 */
#define DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(_unit, _reason) \
    dnx_err_recovery_regression_testing_api_exclude(_unit, _reason)

#ifdef DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS
#define DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit)\
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_diag_start(_unit, __FUNCTION__))

#define DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)\
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_diag_end(_unit))
#else
#define DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit)
#define DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)
#endif

/**
 * \brief - err recovery transaction start. Fails API if error.
 */
#define DNX_ERR_RECOVERY_START(_unit) \
    DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit); \
    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start(_unit))

/**
 * \brief - err recovery transaction end. Assumes function return value. Fails API if error.
 */
#define DNX_ERR_RECOVERY_END(_unit)                                                  \
    if(dnx_err_recovery_transaction_end(_unit, SHR_GET_CURRENT_ERR()) != _SHR_E_NONE)\
    {                                                                                \
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);                                        \
    }                                                                                \
    DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)

#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(_unit, _size, _var_ptr)                  \
do {                                                                                                \
    if(NULL != (_var_ptr))                                                                          \
    {                                                                                               \
        SHR_IF_ERR_EXIT(dnx_err_recovery_regression_journal_var(_unit, _size, (uint8 *)(_var_ptr)));\
    }                                                                                               \
} while(0)

/**
 * \brief - restore variable state at the end of SIMULATION run during ER recovery regression.
 */
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(_unit, _var)\
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(_unit, sizeof(_var), &(_var))

#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_ARRAY(_unit, _array, _count) \
do {                                                                            \
    for(int er_idx = 0; er_idx < (_count); er_idx++) {                       \
        DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(_unit, (_array)[er_idx]);\
    }                                                                           \
} while(0)

#else
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_BEGIN(_unit)
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_END(_unit)
#define DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(_unit, _reason)
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(_unit, _size, _var_ptr)
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(_unit, _var)
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_ARRAY(_unit, _array, _count)

/**
 * \brief - err recovery transaction start. Does not fail API on error.
 */
#define DNX_ERR_RECOVERY_START(_unit)\
    dnx_err_recovery_transaction_start(_unit)

/**
 * \brief - err recovery transaction end. Does not fail API if error.
 */
#define DNX_ERR_RECOVERY_END(_unit)\
    dnx_err_recovery_transaction_end(_unit, SHR_GET_CURRENT_ERR())

#endif

/*
 * \brief - enable or disable error recovery initialization at compile time
 */
#define DNX_ERR_RECOVERY_IS_DISABLED FALSE

/*
 * \brief - maximum number of allowed nested APIs
 */
#define DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS 3

/*
 * \brief - maximum number of allowed nested transactions
 */
#define DNX_ERR_RECOVERY_MAX_NUM_NESTED_TRANSACTIONS 15

#ifdef DNX_ERR_RECOVERY_VALIDATION
#define DNX_ERR_RECOVERY_API_COUNTER_INC(_unit) dnx_err_recovery_api_counter_inc(_unit)
#define DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit) dnx_err_recovery_api_counter_dec(_unit)
#define DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(_unit) dnx_err_recovery_hw_access_is_valid(_unit)
#else
#define DNX_ERR_RECOVERY_API_COUNTER_INC(_unit)
#define DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit)
#define DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(_unit)
#endif

#define DNX_ERR_RECOVERY_TRANSACTION_START(_unit)\
    DNX_ERR_RECOVERY_API_COUNTER_INC(_unit);\
    DNX_ERR_RECOVERY_REGRESSION_TESTING_BEGIN(_unit)

#define DNX_ERR_RECOVERY_TRANSACTION_END(_unit)\
    DNX_ERR_RECOVERY_REGRESSION_TESTING_END(_unit);\
    DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit)

/**
 * \brief - marks the beginning of an error recovery no support region
 */
#define DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(_unit)\
    dnx_err_recovery_transaction_no_support_counter_inc(_unit)

/**
 * \brief - marks the end of a error recovery no support region
 */
#define DNX_ERR_RECOVERY_NO_SUPPORT_END(_unit)\
    dnx_err_recovery_transaction_no_support_counter_dec(_unit)

/**
 * \brief - err recovery transaction start.
 *  Don't test API if regression testing is on.
 */
#define DNX_ERR_RECOVERY_START_NO_TESTING(_unit, _reason)\
    DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(_unit, _reason);\
    DNX_ERR_RECOVERY_START(_unit)

/**
 * \brief - supresses temporary error recovery
 */
#define DNX_ERR_RECOVERY_SUPPRESS(_unit)\
    dnx_err_recovery_tmp_allow(_unit, FALSE)

/**
 * \brief - remove temporary error recovery suppression
 */
#define DNX_ERR_RECOVERY_UNSUPPRESS(_unit)\
    dnx_err_recovery_tmp_allow(_unit, TRUE)

/*
 * }
 */

/**
 * \brief - set of reasons not to run regression testing algorithm for the current API
 */
typedef enum
{
    DNX_ERR_RECOVERY_API_NO_TEST_UNKNOWN = 0,
    /*
     * API cointains in/out parameters.
     */
    DNX_ERR_RECOVERY_API_NO_TEST_IN_OUT_PARAM,
    /*
     * API is creating/destroying dynamically dbal tables.
     */
    DNX_ERR_RECOVERY_API_NO_TEST_DBAL_TBL_CREATE_DESTROY,
} dnx_err_recovery_api_no_test_reason_t;

/**
 * \brief - defines the set of fields used by the err recovery manager
 */
typedef struct err_recovery_manager_d
{
    uint8 is_initialized;

    /*
     * journal only after APPL Init is done 
     */
    uint8 is_appl_init_done;

    /*
     * current transaction is temporary suppressed flag
     */
    uint32 is_suppressed_counter;

    /*
     * current transaction is invalidated flag.
     * used for APIs that utilize unsupported features
     */
    uint8 is_invalidated;

    /*
     * Indicates the level of nesting of "no support" regions.
     * No error recovery transaction can start if this counter is non-zero.
     */
    uint32 no_support_counter;

    /*
     * nested transaction depth
     */
    uint32 transaction_counter;

    /*
     * count entries in the current error recovery transaction
     */
    uint32 entry_counter;

    /*
     * indicates if currently in dbal access region
     */
    uint32 dbal_access_region_counter;

    /*
     * which error recovery related errors are suppressed for the current transaction
     */
    uint32 is_suppressed_err;

    /*
     * up to 15 nested transactions, 0 is invalid
     */
    uint32 trans_bookmarks[DNX_ERR_RECOVERY_MAX_NUM_NESTED_TRANSACTIONS + 1];

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * nested API depth
     */
    uint32 api_counter;

    /*
     * Maps BCM APIs to transaction numbers, up to 3 nested BCM APIs, 0 is invalid
     */
    uint32 api_to_trans_map[DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS + 1];
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    /*
     * flag indicating if regression testing is enabled
     */
    uint8 is_testing_enabled;

    /*
     * indicate that regression testing algorithm should not be performed
     * for the the current BCM API.
     */
    uint8 is_api_excluded;

    /*
     * indicates that the first (rollback) iteration for DNX API has happened. Limits the total number of iterations to two.
     */
    uint8 is_api_ran;

    /*
     * indicats at least one swstate or hw access outside of a transaction in the current non-nested BCM API
     * flag is zeroes out at the beginning and at the end of each non-nested BCM API
     */
    uint8 has_access_outside_trans;

    /*
     * track the number of non-nested transactions in the current BCM API. Not valid for nested BCM APIs.
     */
    uint32 non_nested_trans_in_api_count;

    /*
     * name of current API being tested
     */
    char dnx_err_recovery_regression_testing_api_name[DNXC_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH];
#endif                          /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

} err_recovery_manager_t;

#ifdef DNX_ERR_RECOVERY_VALIDATION
/**
 * \brief
 *  Increases API counter
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_api_counter_inc(
    int unit);

/**
 * \brief
 *  Decreases API counter
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_api_counter_dec(
    int unit);


/**
 * \brief
 *  Check if the current hw access is err recovery
 * \param [in] unit    - Device Id
 * \return
 *   uint8 - boolean result if access is err recovery
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_err_recovery_hw_access_is_valid(
    int unit);
#endif

/**
 * \brief
 *  Initialize error recovery
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_initialize(
    int unit);

/**
 * \brief
 *  Deinitialize error recovery
 *  Can be used to disable error recovery feature at runtime.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_deinitialize(
    int unit);

/**
 * \brief
 *  Check if error recovery feature is turned on.
 * \param [in] unit    - Device Id
 * \return
 *   uint8 - boolean value indicating if journaling is on
 * \remark
 *   * must be called from journaling thread
 * \see
 *   * None
 */
uint8 dnx_err_recovery_is_on(
    int unit);

/**
 * \brief
 *  Temprary turn on or off error recovery feature
 * \param [in] unit    - Device Id
 * \param [in] is_on   - Boolean on/off value
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_tmp_allow(
    int unit,
    uint8 is_on);

/**
 * \brief
 *  Turn on temporary error recovery related errors. Journaling is not impacted.
 * \param [in] unit    - Device Id
 * \param [in] flags   - Error types to suppress
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_suppress_errors(
    int unit,
    uint32 flags);

/**
 * \brief
 *  Turn off temporary error recovery related errors. Journaling is not impacted.
 * \param [in] unit    - Device Id
 * \param [in] flags   - Error types to suppress
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_unsuppress_errors(
    int unit,
    uint32 flags);

/**
 * \brief
 *  Error recovery start function for ctests.
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * Used in ctests only
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_start(
    int unit);

/**
 * \brief
 *  Error recovery end function for ctests.
 * \param [in] unit    - Device Id
 * \param [in] rollback_journal - indicate if the journal should be rolled-back or cleared at the end of the transaction
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_end(
    int unit,
    uint8 rollback_journal);

/**
 * \brief
 *  Start or stop marking hw access as error recovery access
 * \param [in] unit    - Device Id
 * \param [in] is_on   - Boolean on/off value
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_hw_access(
    int unit,
    uint8 is_on);

/**
 * \brief
 *   Increments the "no support" counter for error recovery.
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_transaction_no_support_counter_inc(
    int unit);

/**
 * \brief
 *   Decrements the "no support" counter for error recovery.
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_transaction_no_support_counter_dec(
    int unit);

/**
 * \brief
 *  Start error recovery transaction
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_transaction_start(
    int unit);

/**
 * \brief
 *  End error recovery transaction
 * \param [in] unit    - Device Id
 * \param [in] api_result - Error Type returned from API
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_transaction_end(
    int unit,
    int api_result);

/**
 * \brief
 *  Invalidate the current error recovery transaction.
 *  Utilized by features that are in conflict with error recovery.
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_transaction_invalidate(
    int unit);

/**
 * \brief
 *  Indicate the begin or end of unsupported dbal table check bypass.
 * \param [in] unit   - Unit id
 * \param [in] is_on   - Indicate being or end of bypass region
 * \return
 *   int - Error Type
 * \remark
 *   * Used in cases of providing error recovery support for APIs which contain unsupported tables.
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_unsupported_dbal_unsup_tbls_bypass(
    int unit,
    uint8 is_on);

/**
 * \brief
 *  Increment the dbal access region counter for rollack journal
 *  Assumes that dbal rollback journal is on.
 * \param [in] unit - Unit id
 * \param [in] access_type - Type of action access that will be performed
 * \param [in] access_func - action access function type of the caller
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_manager_dbal_access_region_inc(
    int unit,
    dbal_action_access_type_e access_type,
    dbal_action_access_func_e access_func);

/**
 * \brief
 *  Decrements the dbal access region counter for rollack journal
 *  Assumes that dbal rollback journal is on.
 * \param [in] unit - Unit id
 * \param [in] access_type - Type of action access that will be performed
 * \param [in] access_func - action access function type of the caller
 * \return
 *   int - Error Type
 * \remark
 *   * Used to indicate the begin or end dbal journal access region.
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_manager_dbal_access_region_dec(
    int unit,
    dbal_action_access_type_e access_type,
    dbal_action_access_func_e access_func);

/**
 * \brief
 *  Print contents of error recovery swstate and dbal journals
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_print_journals(
    int unit);

/**
 * \brief
 *  Indicate that init/deinit is done and journaling can begin/end
 * \param [in] unit - Device Id
 * \param [in] is_init - Indicate if APPL is done init or deinit
 * \return
 *   int - Error Type
 * \remark
 *   * We don't support journaling during init / deinit stages
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_appl_init_state_change(
    int unit,
    uint8 is_init);

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
/**
 * \brief
 *  Update the testing API name
 * \param [in] unit - Device Id
 * \param [in] func_name - Name of function
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_err_recovery_regression_testing_api_name_update(
    int unit,
    const char *func_name);

/**
 * \brief
 *  Check if API should be ran again
 * \param [in] unit - Device Id
 * \return
 *   int - boolean result indicating if API should be ran again
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_err_recovery_regression_test_should_loop(
    int unit);

/**
 * \brief
 *  Increment test counter and check if API should be ran again
 * \param [in] unit - Device Id
 * \return
 *   int - boolean result indicating if API should be ran again
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_err_recovery_regression_test_counter_inc(
    int unit);

/**
 * \brief
 *  Change rollback journal test mode
 * \param [in] unit - Device Id
 * \param [in] is_on - Indicate if the testmode should be turned on or off
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_err_recovery_regression_testing_api_test_mode_change(
    int unit,
    uint8 is_on);

/**
 * \brief
 *  Exclude API from error recovery API testing
 * \param [in] unit - Device Id
 * \param [in] reason - Reason current API should be excluded from testing.
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_err_recovery_regression_testing_api_exclude(
    int unit,
    dnx_err_recovery_api_no_test_reason_t reason);

/**
 * \brief
 *  Journal state of variable that will be restored at the end of simulation run during testing.
 * \param [in] unit - Device Id
 * \param [in] size - Size of data
 * \param [in] ptr - Pointer to variable data that will be journal.
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_regression_journal_var(
    int unit,
    uint32 size,
    uint8 *ptr);

#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

/* } */
#endif /* _DNX_ERR_RECOVERY_MANAGER_H */
