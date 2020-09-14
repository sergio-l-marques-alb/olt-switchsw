/** \file dnx_err_recovery_manager.h
 * This module is the Error Recovery manager.
 * It is the interface to the Error Recovery feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
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
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/dnx_er_regression.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <bcm/switch.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
/*
 * \brief - maximum number of allowed nested APIs
 */
#define DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS 3

/**
 * \brief - increment the BCM API counter
 */
#define DNX_ERR_RECOVERY_API_COUNTER_INC(_unit)\
    dnx_err_recovery_api_counter_inc(_unit)

/**
 * \brief - decrement the BCM API counter
 */
#define DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit)\
    dnx_err_recovery_api_counter_dec(_unit, r_rv != BCM_E_NONE)
#else /* DNX_ERR_RECOVERY_VALIDATION_BASIC */
#define DNX_ERR_RECOVERY_API_COUNTER_INC(_unit)
#define DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit)
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

#ifdef DNX_ERR_RECOVERY_VALIDATION
/**
 * \brief - mark the API as Error Recovery not needed
 */
#define DNX_ERR_RECOVERY_NOT_NEEDED(_unit)\
    DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(unit);\
    dnx_err_recovery_not_needed(_unit)

/**
 * \brief - mark the API as Error Recovery not needed.
 * Skip API_NO_TESTING since each BCM API has an internal call to switch APIs
 * which breaks the ER regression algorithm.
 */
#define DNX_ERR_RECOVERY_NOT_NEEDED_SWITCH(_unit)\
    dnx_err_recovery_not_needed(_unit)

/**
 * \brief - check if sw access is valid
 */
#define DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(_unit)\
    dnx_err_recovery_hw_access_is_valid(_unit)
#else /* DNX_ERR_RECOVERY_VALIDATION */
#define DNX_ERR_RECOVERY_NOT_NEEDED(_unit) DNX_ERR_RECOVERY_REGRESSION_API_NO_TESTING(_unit)
#define DNX_ERR_RECOVERY_NOT_NEEDED_SWITCH(_unit)
#define DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(_unit)
#endif /* DNX_ERR_RECOVERY_VALIDATION */

#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS
/**
 * \brief - start space-time diagnostics for the current transaction.
 */
#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit)\
    SHR_IF_ERR_EXIT(dnx_rollback_journal_diag_start(_unit, __FUNCTION__))

/**
 * \brief - end space-time diagnostics for the current transaction.
 */
#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)\
    dnx_rollback_journal_diag_end(_unit)

#else
#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit)
#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)
#endif /* DNX_ROLLBACK_JOURNAL_DIAGNOSTICS */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
/**
 * \brief - err recovery transaction end. Assumes function return value. Fails API if error.
 */
#define DNX_ERR_RECOVERY_END_INTERNAL_ERR(_unit, _module_id)                                     \
do {                                                                                             \
    if(dnx_err_recovery_transaction_end(_unit, _module_id, SHR_GET_CURRENT_ERR()) != _SHR_E_NONE)\
    {                                                                                            \
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);                                                    \
    }                                                                                            \
} while(0)
#else
/**
 * \brief - err recovery transaction end. Does not fail API if error.
 */
#define DNX_ERR_RECOVERY_END_INTERNAL_ERR(_unit, _module_id)\
    dnx_err_recovery_transaction_end(_unit, _module_id, SHR_GET_CURRENT_ERR())
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

/*
 * \brief - maximum number of allowed nested transactions
 */
#define DNX_ERR_RECOVERY_MAX_NUM_NESTED_TRANSACTIONS 15

/**
 * \brief - err recovery transaction start. Transaction start should be able to fail API.
 */
#define DNX_ERR_RECOVERY_START_INTERNAL(_unit, _module_id)\
    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start(_unit, _module_id))

/**
 * \brief - err recovery transaction end.
 */
#define DNX_ERR_RECOVERY_END_INTERNAL(_unit, _module_id)\
    DNX_ERR_RECOVERY_END_INTERNAL_ERR(_unit, _module_id)

/**
 * \brief - supresses temporary error recovery
 *  Used for parts in an error recovery trasnaction that should not be journaled.
 */
#define DNX_ERR_RECOVERY_SUPPRESS(_unit)\
    dnx_err_recovery_tmp_allow(_unit, FALSE)

/**
 * \brief - remove temporary error recovery suppression.
 *  Used for parts in an error recovery trasnaction that should not be journaled.
 */
#define DNX_ERR_RECOVERY_UNSUPPRESS(_unit)\
    dnx_err_recovery_tmp_allow(_unit, TRUE)

/**
 * \brief - marks the beginning of an error recovery no support region.
 *  No top-level transaction can be started in this region on the current thread.
 *  If transaction has already been started, it acts as a temporary suppression.
 */
#define DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(_unit)\
    DNX_ERR_RECOVERY_NOT_NEEDED(_unit);         \
    dnx_err_recovery_transaction_no_support_counter_inc(_unit)

/**
 * \brief - marks the end of a error recovery no support region
 */
#define DNX_ERR_RECOVERY_NO_SUPPORT_END(_unit)\
    dnx_err_recovery_transaction_no_support_counter_dec(_unit)

/**
 * \brief - mark the beginning of a transaction, used by bcm APIs
 *  API counter increment needs to occur before the api name update so that
 *  preconditions used in the name update function can be met.
 *  This means that during API counter increment the name of the API has not been
 *  updated yet.
 */
#define DNX_ERR_RECOVERY_TRANSACTION_START(_unit)              \
    DNX_ERR_RECOVERY_API_COUNTER_INC(_unit);                   \
    DNX_ERR_RECOVERY_REGRESSION_TESTING_API_NAME_UPDATE(_unit);\
    DNX_ERR_RECOVERY_REGRESSION_TESTING_BEGIN(_unit)

/**
 * \brief - mark the end of a transaction, used by bcm APIs
 */
#define DNX_ERR_RECOVERY_TRANSACTION_END(_unit)\
    DNX_ERR_RECOVERY_REGRESSION_TESTING_END(_unit);\
    DNX_ERR_RECOVERY_API_COUNTER_DEC(_unit)

/**
 * \brief - Error recovery start function for ctests.
 */
#define DNX_ROLLBACK_JOURNAL_START(_unit)\
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(_unit))

/**
 * \brief - Error recovery end function for ctests.
 */
#define DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(_unit)\
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(_unit, TRUE))

/**
 * \brief - suppress expression only, exit if error
 */
#define DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(_expr)                                                      \
do                                                                                                             \
{                                                                                                              \
    int _rv;                                                                                                   \
                                                                                                               \
    DNX_ERR_RECOVERY_SUPPRESS(unit);                                                                           \
    DNX_ERR_RECOVERY_REGRESSION_ERROR_SUPPRESS(unit, DNX_ERR_RECOVERY_REGRESS_ERROR_SW_ACCESS_OUTSIDE_TRANS);  \
    dnx_state_comparison_suppress(unit, TRUE);                                                                 \
                                                                                                               \
    _rv = _expr;                                                                                               \
                                                                                                               \
    dnx_state_comparison_suppress(unit, FALSE);                                                                \
    DNX_ERR_RECOVERY_REGRESSION_ERROR_UNSUPPRESS(unit, DNX_ERR_RECOVERY_REGRESS_ERROR_SW_ACCESS_OUTSIDE_TRANS);\
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);                                                                         \
                                                                                                               \
    if (SHR_FAILURE(_rv))                                                                                      \
    {                                                                                                          \
        LOG_ERROR_EX(BSL_LOG_MODULE,                                                                           \
            " Error '%s' indicated ; %s%s%s\r\n" ,                                                             \
            shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;                                                    \
            _func_rv = _rv ;                                                                                   \
        SHR_EXIT() ;                                                                                           \
    }                                                                                                          \
} while (0)

/**
 * \brief - err recovery transaction start.
 */
#define DNX_ERR_RECOVERY_START(_unit)                          \
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit);\
    DNX_ERR_RECOVERY_START_INTERNAL(_unit, bcmModuleCount)

/**
 * \brief - err recovery transaction start. Allow to specify module id.
 */
#define DNX_ERR_RECOVERY_START_MODULE_ID(_unit, _module_id)    \
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_START(_unit);\
    DNX_ERR_RECOVERY_START_INTERNAL(_unit, _module_id)

/**
 * \brief - err recovery transaction end.
 */
#define DNX_ERR_RECOVERY_END(_unit)                      \
    DNX_ERR_RECOVERY_END_INTERNAL(_unit, bcmModuleCount);\
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)

/**
 * \brief - err recovery transaction end. Allow to specify module id.
 */
#define DNX_ERR_RECOVERY_END_MODULE_ID(_unit, _module_id)\
    DNX_ERR_RECOVERY_END_INTERNAL(_unit, _module_id);    \
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_TRANSACTION_END(_unit)
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
    /*
     * There exist multiple sibling transaction insde the BCM API.
     *
     * By default this is a violation, however excetions are possible.
     * Testing such BCM APIs need to be skipped, since "Simulation Run"
     * will not rollback all internal transactions in the slibling DNX APIs
     * and therefore "Normal Run" will most likely crash the API.
     */
    DNX_ERR_RECOVERY_API_NO_TEST_SIBLING_TRANSACTIONS,
    /*
     * API has been marked as not needed, therefore no testing for it.
     */
    DNX_ERR_RECOVERY_API_NO_TESTING_NOT_NEEDED
} dnx_err_recovery_api_no_test_reason_t;

#ifdef DNX_ERR_RECOVERY_VALIDATION
/**
 * \brief - Indicate the API status
 */
typedef enum
{
    DNX_ERR_RECOVERY_API_STATUS_UNKNOWN = 0,
    DNX_ERR_RECOVERY_API_OPTS_IN,
    DNX_ERR_RECOVERY_API_NOT_NEEDED
} dnx_err_recovery_api_status;
#endif /* DNX_ERR_RECOVERY_VALIDATION */

/**
 * \brief - defines the set of fields used by the err recovery manager
 */
typedef struct err_recovery_manager_d
{

    /*
     * current transaction is temporary suppressed flag
     */
    uint32 is_suppressed_counter;

    /*
     * nested transaction depth
     */
    uint32 transaction_counter;

    /*
     * count entries in the current error recovery transaction
     */
    uint32 entry_counter;

    /*
     * up to 15 nested transactions, 0 is invalid
     */
    uint32 trans_bookmarks[DNX_ERR_RECOVERY_MAX_NUM_NESTED_TRANSACTIONS + 1];

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
    /*
     * runtime flag indicating if validation has been enabled
     */
    uint8 validation_enabled;

    /*
     * nested API depth
     */
    uint32 api_counter;

    /*
     * Maps BCM APIs to transaction numbers, up to 3 nested BCM APIs, 0 is invalid
     */
    uint32 api_to_trans_map[DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS + 1];
#endif                          /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * indicates if API opts in for ER or ER is not needed for API
     */
    dnx_err_recovery_api_status api_status;
    /*
     * Total number of DNX APIs that either OPT-IN or do NOT_NEEDED in a BCM api.
     *
     * Each BCM API is currently calling "bcm_dnx_switch_control_get" which additionally changes the api_status,
     * meaning that at the end of each BCM API the api_status_counter should be >=2.
     * (the api_counter is >2 if nested DNX APIs additionally OPT-IN or do NOT_NEEDED)
     */
    uint32 api_status_count;
#endif                          /* DNX_ERR_RECOVERY_VALIDATION */

} err_recovery_manager_t;

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
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
 * \param [in] bcm_api_error    - Indicate if error occured in bcm api.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_api_counter_dec(
    int unit,
    uint8 bcm_api_error);
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

#ifdef DNX_ERR_RECOVERY_VALIDATION
/**
 * \brief
 *  Mark the API as Error Recovery not needed
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_not_needed(
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
#endif /* DNX_ERR_RECOVERY_VALIDATION */

/**
 * \brief
 *  Temporary enable journaling during init.
 * \param [in] unit - Device Id
 * \param [in] is_on - Indicates if counter is incremented or decremented.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_is_init_check_bypass(
    int unit,
    uint8 is_on);

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
 *  Initialize error recovery and snapshot manager
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_err_recovery_module_init(
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
 *  Deinitialize error recovery and snapshot manager
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_err_recovery_module_deinit(
    int unit);

/**
 * \brief
 *  Test interface to check if error recovery feature is turned on.
 * \param [in] unit    - Device Id
 * \return
 *   uint8 - boolean value indicating if journaling is on
 * \remark
 *   * must be called from journaling thread
 * \see
 *   * None
 */
uint8 dnx_err_recovery_is_on_test(
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
 * \param [in] module_id    - Module id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_transaction_start(
    int unit,
    bcm_switch_module_t module_id);

/**
 * \brief
 *  End error recovery transaction
 * \param [in] unit    - Device Id
 * \param [in] module_id    - Module id
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
    bcm_switch_module_t module_id,
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
 *  Print contents of error recovery swstate and dbal journals
 * \param [in] unit - Device Id
 * \param [in] type - rollback journal to be toggled.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type);

/**
 * \brief
 *   Toggle rollback journal logger.
 *   Each entry being inserted in the journal will be printed on a continuous basis.
 * \param [in] unit   - Device Id
 * \param [in] type - rollback journal to be toggled.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_logger_state_change(
    int unit,
    dnx_rollback_journal_subtype_e type);

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

/* } */
#endif /* _DNX_ERR_RECOVERY_MANAGER_H */
