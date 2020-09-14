/** \file dnx_er_regression.h
 *
 * This module cotains the Error Recovery regression algorithm for running tests in
 * the "ERROR-RECOVERY-REGRESSION" system mode.
 * Logic defined in the module is utilized by the Error Recovery manager in order to invoke
 * special behavior for APIs that opt-in for Error Recovery.
 *
 * Algorithm overview:
 * Each DNX API in the test that opts-in for Error Recovery is being ran twice in a sequence,
 * where the first run of the BCM API is the "simulation run" and the second is the "normal run".
 *
 * - simulation run for a BCM API is to prove that all SWSTATE and DBAL operations done during the execution
 *   of the currently tested API can be successfully rolled back. In order to do this rollback is performed at the end of the simulation run.
 *   State comparison is done between the start and the end of the run in order to have basic confidence in the rollback success.
 *
 * - normal run, only if the simulation run has been successful. Normal flow of the API is being followed.
 *   Ensures that the execution of the test can continue, as well as it provides an additional validation after the simulation run.
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_ER_REGRESSION_H
/* { */
#define _DNX_ER_REGRESSION_H

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
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
/*
 * }
 */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
/*
 * \brief - update the name of the regressed api
 */
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_API_NAME_UPDATE(_unit)\
    dnx_er_regress_testing_api_name_update(_unit, __func__)

/*
 * \brief - error recovery regression testing header
 */
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_BEGIN(_unit)                     \
    r_rv = BCM_E_NONE;                                                       \
    do {

/*
 * \brief - error recovery regression testing footer
 */
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_END(_unit)          \
    } while(dnx_er_regress_test_should_loop(_unit) && (BCM_E_NONE == r_rv))

/*
 * \brief - indicates the regression testing should be
 *  skipped for the current BCM API.
 */
#define DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(_unit)\
    dnx_er_regress_testing_api_exclude(_unit)

/**
 * \brief - restore pointed data state at the end of SIMULATION run during ER regression.
 */
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(_unit, _size, _var_ptr)     \
do {                                                                                   \
    if(NULL != (_var_ptr))                                                             \
    {                                                                                  \
        SHR_IF_ERR_EXIT(dnx_er_regress_journal_var(_unit, _size, (uint8 *)(_var_ptr)));\
    }                                                                                  \
} while(0)

/**
 * \brief - restore variable state at the end of SIMULATION run during ER regression.
 */
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(_unit, _var)\
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(_unit, sizeof(_var), &(_var))

/**
 * \brief - restore array state at the end of SIMULATION run durin ER regression.
 */
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_ARRAY(_unit, _array, _count) \
do {                                                                            \
    for(int er_idx = 0; er_idx < (_count); er_idx++) {                          \
        DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(_unit, (_array)[er_idx]);\
    }                                                                           \
} while(0)

/**
 * \brief - suppress violations specific to ER regression
 */
#define DNX_ERR_RECOVERY_REGRESSION_ERROR_SUPPRESS(_unit, _type) dnx_err_recovery_regress_violate_tmp_allow(_unit, _type, FALSE)

/**
 * \brief - unsuppress violations specific to ER regression
 */
#define DNX_ERR_RECOVERY_REGRESSION_ERROR_UNSUPPRESS(_unit, _type) dnx_err_recovery_regress_violate_tmp_allow(_unit, _type, TRUE)

/**
 * \brief - describes each er regression error type
 */
typedef enum
{
    DNX_ERR_RECOVERY_REGRESS_ERROR_SW_ACCESS_OUTSIDE_TRANS = 0,
    DNX_ERR_RECOVERY_REGRESS_ERROR_DBAL_ACCESS_OUTSIDE_TRANS,
    DNX_ERR_RECOVERY_REGRESS_ERROR_NOF
} dnx_err_recovery_regress_error_t;

/*
 * \brief - callback used by the regression infrastructure to validation functions in the manager
 */
typedef uint8 (
    *dnx_er_regression_cb) (
    int unit);

/**
 * \brief - defines the set of fields used by the err recovery regression
 */
typedef struct dnx_er_regress_d
{
    /*
     * journal only after APPL Init is done
     */
    uint8 is_appl_init_done;

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
    uint32 top_level_trans_in_api_count;

    /*
     * set of counters, each indicating if a particular type of error has been suppressed
     */
    uint32 err_type_suppress_counters[DNX_ERR_RECOVERY_REGRESS_ERROR_NOF];

    /*
     * name of current API being tested. Used for diagnostic purposes.
     * printed as for each "simulation" and "normal" API run.
     */
    char dnx_er_regress_testing_api_name[DNX_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH];

} dnx_er_regress_t;

/**
 * \brief
 *  Initialize the regression module.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_init(
    int unit);

/**
 * \brief
 *  Deinitialize the regression module.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_deinit(
    int unit);

/**
 * \brief
 *  Reset regression flags related to API testing.
 *  Called at the beginning and end of each top-level BCM API.
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_top_level_api_flags_reset(
    int unit);

/**
 * \brief
 *  Temporary suspends the comparison journal used during the simulation run
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_comparison_tmp_allow(
    int unit,
    uint8 is_on);

/**
 * \brief
 *  Validation done at transaction start
 * \param [in] unit - Device Id
 * \param [in] is_top_level_transaction - Indicates if top level transaction
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_transaction_start(
    int unit,
    uint8 is_top_level_transaction);

/**
 * \brief
 *  Indicate if the transaction shoud be committed or aborted
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_transaction_end_should_abort(
    int unit);

/**
 * \brief
 *  Indicate transaction function failure should return errors.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_transaction_fail_should_return_error(
    int unit);

/**
 * \brief
 *  Print api name and compare at end of transaction
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_transaction_end(
    int unit);

/**
 * \brief
 *  Regression rollback journal start
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_journal_start(
    int unit);

/**
 * \brief
 *  Regression application init state change
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_regress_appl_init_state_change(
    int unit,
    uint8 is_init);

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
void dnx_er_regress_testing_api_name_update(
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
int dnx_er_regress_test_should_loop(
    int unit);

/**
 * \brief
 *  Change rollback journal test mode
 * \param [in] unit - Device Id
 * \return
 *   * uint8 - Indicates if api testing is enabled or disabled
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_er_regress_testing_api_test_mode_get(
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
void dnx_er_regress_testing_api_test_mode_change(
    int unit,
    uint8 is_on);

/**
 * \brief
 *  Exclude API from error recovery API testing
 * \param [in] unit - Device Id
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_regress_testing_api_exclude(
    int unit);

/**
 * \brief
 *  Journal state of variable that will be restored at the end of simulation run during testing.
 *  Used for API in/out parameters.
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
shr_error_e dnx_er_regress_journal_var(
    int unit,
    uint32 size,
    uint8 *ptr);

/**
 * \brief
 *  Turn on temporary suppression for related regression violation type.
 *  Journaling is not impacted.
 * \param [in] unit    - Device Id
 * \param [in] flags   - Error types to suppress
 * \param [in] is_on   - Boolean on/off value
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_regress_violate_tmp_allow(
    int unit,
    dnx_err_recovery_regress_error_t type,
    uint8 is_on);

/**
 * \brief
 *  Callback used each time swstate is being accessed
 * \param [in] unit    - Device Id
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_regress_swstate_access_cb(
    int unit);

/**
 * \brief
 *  Callback used each time dbal is being accessed
 * \param [in] unit - Device Id
 * \param [in] table_ptr - Pointer to logical table being accessed.
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_regress_dbal_access_cb(
    int unit,
    dbal_logical_table_t * table_ptr);

#else /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_API_NAME_UPDATE(_unit)
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_BEGIN(_unit)
#define DNX_ERR_RECOVERY_REGRESSION_TESTING_END(_unit)
#define DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(_unit)
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(_unit, _size, _var_ptr)
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(_unit, _var)
#define DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_ARRAY(_unit, _array, _count)
#define DNX_ERR_RECOVERY_REGRESSION_ERROR_SUPPRESS(_unit, _type)
#define DNX_ERR_RECOVERY_REGRESSION_ERROR_UNSUPPRESS(_unit, _type)

#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

/* } */
#endif /* _DNX_ER_REGRESSION_H */
