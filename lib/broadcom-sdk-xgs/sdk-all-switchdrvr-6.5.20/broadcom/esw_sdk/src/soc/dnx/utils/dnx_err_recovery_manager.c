/** \file dnx_err_recovery_manager.c
 * This module is the Error Recovery manager.
 * It is the interface to the Error Recovery feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <soc/drv.h>

#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

/*
 * \brief - exit of not DNX chip
 */
#define DNX_ERR_RECOVERY_EXIT_IF_NOT_DNX(_unit)\
do{                                            \
    if(!SOC_IS_DNX(_unit))                     \
    {                                          \
        SHR_EXIT();                            \
    }                                          \
}while(0)

/*
 * \brief - exit if the feature is not initialized
 */
#define DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(_unit)\
do{                                                          \
    if (!dnx_err_recovery_are_prerequisites_met(_unit)) {    \
        SHR_EXIT();                                          \
    }                                                        \
}while(0)

/*
 * \brief - exit if no api has opted in for error recovery.
 */
#define DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(_unit)           \
do {                                                              \
    if(0 == dnx_err_recovery_manager[_unit].transaction_counter) {\
        SHR_EXIT();                                               \
    }                                                             \
}                                                                 \
while(0)

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
/*
 * \brief - exit if the rutime validation flag hasn't been enabled.
 * Assumes there are no API calls during init.
 */
#define DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(_unit)         \
do {                                                          \
    if (!dnx_err_recovery_manager[_unit].validation_enabled) {\
        SHR_EXIT();                                           \
    }                                                         \
} while(0)
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

/*
 * \brief - exit if no api has opted in for error recovery.
 */
#define DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(_unit)                                                                                        \
do {                                                                                                                                       \
    if(!dnx_err_recovery_common_flag_is_on(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED)){\
        SHR_EXIT();                                                                                                                        \
    }                                                                                                                                      \
}                                                                                                                                          \
while(0)

err_recovery_manager_t dnx_err_recovery_manager[SOC_MAX_NUM_DEVICES] = { {0}
};

static uint8 dnx_err_recovery_are_prerequisites_met(
    int unit);

static uint8 dnx_err_recovery_is_on(
    int unit);

/*
 * \brief - bookmark where the transaction starts, maps transaction number with the entry counter.
 *          on rollback the transaction should be rollbacked up to, but not including, this point
 */
static void
dnx_err_recovery_transaction_bookmark_set(
    int unit,
    uint32 idx,
    uint32 seq_id)
{
    dnx_err_recovery_manager[unit].trans_bookmarks[idx] = seq_id;
}

/*
 * \brief - return the bookmark value for an index.
 */
static uint32
dnx_err_recovery_transaction_bookmark_get(
    int unit,
    uint32 idx)
{
    return dnx_err_recovery_manager[unit].trans_bookmarks[idx];
}

static shr_error_e
dnx_err_recovery_transaction_clear(
    int unit)
{
    uint32 idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * clear rollback journals
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear(unit, FALSE, FALSE));
    SHR_IF_ERR_EXIT(dnx_dbal_journal_clear(unit, FALSE, FALSE));
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_clear(unit, FALSE));

    /*
     * clear all bookmarks and set entry count to zero
     */
    for (idx = 0; idx < dnx_err_recovery_manager[unit].transaction_counter; idx++)
    {
        dnx_err_recovery_transaction_bookmark_set(unit, idx, 0);
    }

    dnx_err_recovery_manager[unit].entry_counter = 0;

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Perform commit operations on roll-back journals for the current transation.
 */
static shr_error_e
dnx_err_recovery_commit_unsafe(
    int unit)
{
    uint32 state_entry_seq_id = 0;
    uint32 sw_state_entry_seq_id = 0;
    uint32 dbal_entry_seq_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * clear journals
     */
    while (dnx_err_recovery_manager[unit].entry_counter > 0)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, FALSE, &sw_state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_get_head_seq_id(unit, &state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, FALSE, &dbal_entry_seq_id));

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * check if entry sequence id is not found in either journals
         */
        if ((sw_state_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter)
            && (state_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter)
            && (dbal_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter))
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: entry sequence id not found in journals.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

        /*
         * clear dbal or sw state journal entry
         */
        if (sw_state_entry_seq_id == dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear(unit, TRUE, FALSE));
        }
        else if (state_entry_seq_id == dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnx_generic_state_journal_clear(unit, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_clear(unit, TRUE, FALSE));
        }

        dnx_err_recovery_manager[unit].entry_counter--;
    }

    dnx_err_recovery_transaction_bookmark_set(unit, dnx_err_recovery_manager[unit].transaction_counter, 0);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (dnx_err_recovery_manager[unit].entry_counter != 0)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: entry count non-zero after journals cleared.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Perform abort operations on roll-back journals for the current transaction.
 */
static shr_error_e
dnx_err_recovery_abort_unsafe(
    int unit)
{
    uint32 state_entry_seq_id = 0;
    uint32 sw_state_entry_seq_id = 0;
    uint32 dbal_entry_seq_id = 0;

    uint32 rollback_entry_bound = 0;
    uint8 rollback_error = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * rollback journals only up to the specified bookmark
     */
    rollback_entry_bound =
        dnx_err_recovery_transaction_bookmark_get(unit, dnx_err_recovery_manager[unit].transaction_counter);

    while (dnx_err_recovery_manager[unit].entry_counter > rollback_entry_bound)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, FALSE, &sw_state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_get_head_seq_id(unit, &state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, FALSE, &dbal_entry_seq_id));

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * check if entry sequence id is not found in either journals
         */
        if ((sw_state_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter)
            && (state_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter)
            && (dbal_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter))
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: entry sequence id not found in journals.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

        /*
         * revert dbal or sw state journal entry
         */
        if (sw_state_entry_seq_id == dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back(unit, TRUE, FALSE));
        }
        else if (state_entry_seq_id == dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnx_generic_state_journal_roll_back(unit, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_roll_back(unit, TRUE, FALSE));
        }

        dnx_err_recovery_manager[unit].entry_counter--;
    }

    /*
     * delete the bookmark for current nested transaction
     */
    dnx_err_recovery_transaction_bookmark_set(unit, dnx_err_recovery_manager[unit].transaction_counter, 0);

    rollback_error = FALSE;

exit:
    if (rollback_error)
    {
        dnx_err_recovery_transaction_clear(unit);
    }
    SHR_FUNC_EXIT;
}

/**
 * /brief - cb get sequence id for new journal entry
 */
static uint32
dnx_err_recovery_new_journal_entry_seq_id_get(
    int unit)
{
    dnx_err_recovery_manager[unit].entry_counter++;
    return dnx_err_recovery_manager[unit].entry_counter;
}

/*
 * \brief - Callback used for swstate journal access.
 * Takes under consideration if DBAL access region counter
 */
static uint8
dnx_err_recovery_is_sw_state_journal_on(
    int unit)
{
    /*
     * journal only if we are currently not in dbal region access.
     */
    return (dnx_err_recovery_is_on(unit) && !dnx_err_recovery_common_is_dbal_access_region(unit));
}

static inline void
dnx_err_recovery_transaction_counters_reset(
    int unit)
{
    dnx_err_recovery_manager[unit].transaction_counter = 0;
    dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_err_recovery_manager[unit].is_suppressed_counter = 0;
    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);
}

static inline void
dnx_err_recovery_init_counters_reset(
    int unit)
{
    dnx_err_recovery_manager[unit].entry_counter = 0;

    dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

    dnx_err_recovery_transaction_counters_reset(unit);
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_is_init_check_bypass(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, is_on));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_initialize(
    int unit)
{
    dnx_rollback_journal_cbs_t swstate_cbs;
    dnx_rollback_journal_cbs_t dbal_cbs;
    dnx_rollback_journal_cbs_t state_cbs;

    dnxc_sw_state_journal_access_cb swstate_access_cb = NULL;
    dnx_dbal_journal_access_cb dbal_access_cb = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize Error Recovery only for dnx chips
     */
    DNX_ERR_RECOVERY_EXIT_IF_NOT_DNX(unit);

    if (dnx_err_recovery_common_flag_is_on
        (unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: already initialized .\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    /*
     * reset the value of the flag
     */
#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnx_err_recovery_manager[unit].validation_enabled = TRUE;
#else
    dnx_err_recovery_manager[unit].validation_enabled = FALSE;
#endif /* DNX_ERR_RECOVERY_VALIDATION */
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    /*
     * initialize the regression module, set callbacks.
     */
    SHR_IF_ERR_EXIT(dnx_er_regress_init(unit));
    if (dnx_er_regress_testing_api_test_mode_get(unit))
    {
        swstate_access_cb = dnx_er_regress_swstate_access_cb;
        dbal_access_cb = dnx_er_regress_dbal_access_cb;
        dnx_err_recovery_manager[unit].validation_enabled = TRUE;
    }
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    /*
     * Callbacks are kept and utilized by the lower level journals in order to avoid hierarchy violations.
     */
    swstate_cbs.is_on = &dnx_err_recovery_is_sw_state_journal_on;
    swstate_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_initialize(unit, swstate_cbs, swstate_access_cb, FALSE));

    dbal_cbs.is_on = &dnx_err_recovery_is_on;
    dbal_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dbal_cbs, dbal_access_cb, FALSE));

    state_cbs.is_on = &dnx_err_recovery_is_on;
    state_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_initialize(unit, state_cbs));

    dnx_err_recovery_init_counters_reset(unit);

    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

    SHR_IF_ERR_EXIT(dnx_er_threading_initialize(unit));

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
    dnx_err_recovery_manager[unit].api_counter = 0;
    sal_memset(dnx_err_recovery_manager[unit].api_to_trans_map, 0x0,
               sizeof(dnx_err_recovery_manager[unit].api_to_trans_map));
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_deinitialize(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * do nothing if err recovery hasn't been initialized.
     * it is possible that the function is called more than once in a row
     */
    if (!dnx_err_recovery_common_flag_is_on
        (unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED))
    {
        SHR_EXIT();
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnx_err_recovery_manager[unit].validation_enabled = TRUE;
#else
    dnx_err_recovery_manager[unit].validation_enabled = FALSE;
#endif /* DNX_ERR_RECOVERY_VALIDATION */
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    /*
     * Deinitialize the regression module
     */
    SHR_IF_ERR_EXIT(dnx_er_regress_deinit(unit));

    if (dnx_er_regress_testing_api_test_mode_get(unit))
    {
        dnx_err_recovery_manager[unit].validation_enabled = TRUE;
    }
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_er_threading_deinitialize(unit));

    dnx_err_recovery_init_counters_reset(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
    dnx_err_recovery_manager[unit].api_counter = 0;

    sal_memset(dnx_err_recovery_manager[unit].api_to_trans_map, 0x0,
               sizeof(dnx_err_recovery_manager[unit].api_to_trans_map));
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int
dnx_err_recovery_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_err_recovery_common_init(unit));
    SHR_IF_ERR_EXIT(dnx_err_recovery_initialize(unit));
    SHR_IF_ERR_EXIT(dnx_state_snapshot_manager_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int
dnx_err_recovery_module_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_state_snapshot_manager_deinit(unit));
    SHR_IF_ERR_EXIT(dnx_err_recovery_deinitialize(unit));
exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_api_counter_inc(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(unit);

    DNX_ER_THREADING_EXIT_IF_NOT_MAIN_THREAD(unit);

    if (dnx_err_recovery_manager[unit].api_counter + 1 > DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API counter cannot be more than 3.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    /*
     * save transaction number associated before the beginning of the current BCM API.
     * used for sanity check at the API decrement.
     */
    dnx_err_recovery_manager[unit].api_to_trans_map[dnx_err_recovery_manager[unit].api_counter] =
        dnx_err_recovery_manager[unit].transaction_counter;

    dnx_err_recovery_manager[unit].api_counter++;

    if (1 == dnx_err_recovery_manager[unit].api_counter)
    {
#ifdef DNX_ERR_RECOVERY_VALIDATION
        dnx_err_recovery_manager[unit].api_status = DNX_ERR_RECOVERY_API_STATUS_UNKNOWN;
        dnx_err_recovery_manager[unit].api_status_count = 0;
#endif /* DNX_ERR_RECOVERY_VALIDATION */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        SHR_IF_ERR_EXIT(dnx_er_regress_top_level_api_flags_reset(unit));
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION
/*
 * \brief - validate api status
 */
static uint8
dnx_err_recovery_api_status_is_valid(
    int unit)
{
    return (dnx_err_recovery_manager[unit].api_status != DNX_ERR_RECOVERY_API_STATUS_UNKNOWN
#ifdef BCM_WARM_BOOT_SUPPORT
            /*
             * In each BCM API the api_counter should be modified at least twice:
             * - The associated DNX API
             * - "bcm_dnx_switch_control_get" called from within each BCM API.
             *   ("bcm_switch_control_get" calls "bcm_dnx_switch_control_get" twice)
             */
            && dnx_err_recovery_manager[unit].api_status_count >= 2
#else /* !BCM_WARM_BOOT_SUPPORT */
            /*
             * In each BCM API the api_counter should be modified at least once:
             * - The associated DNX API
             */
            && dnx_err_recovery_manager[unit].api_status_count >= 1
#endif /* BCM_WARM_BOOT_SUPPORT */
        );
}
#endif /* DNX_ERR_RECOVERY_VALIDATION */

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_api_counter_dec(
    int unit,
    uint8 bcm_api_error)
{
    uint8 reset_flags = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NO_VALIDATION(unit);

    DNX_ER_THREADING_EXIT_IF_NOT_MAIN_THREAD(unit);

    if (0 == dnx_err_recovery_manager[unit].api_counter)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API counter cannot be less than 0.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    reset_flags = TRUE;

    if (1 == dnx_err_recovery_manager[unit].api_counter)
    {

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * Enforce violation in the case where:
         * - The internal DNX API does none of the folling: START-END / NOT_NEEDED / NO_SUPPORT
         * - The DNX API did not fail. This takes under consideration the cases where the API is possible to FAIL before addressing error recovery.
         * - We are currently not in a NO_SUPPORT region outside of transaction. In these cases a false positive is trigger for the violation, since the DNX APIs
         *   in such regions cannot opt-in and therefore the api_status cannot be changed internally.
         *   The simplest way to check if we in such a situation is to check if the 'main thread' has been 'excluded'. Previous in this function
         *   we have already discriminated against all threads that are not the main thread, all we need to do is check if the current thread is excluded.
         */
        if (!dnx_err_recovery_api_status_is_valid(unit) && !bcm_api_error && !dnx_er_threading_is_excluded_thread(unit))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: DNX does not use DNX_ERR_RECOVERY_START or DNX_ERR_RECOVERY_NOT_NEEDED.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
        dnx_err_recovery_manager[unit].api_status = DNX_ERR_RECOVERY_API_STATUS_UNKNOWN;
        dnx_err_recovery_manager[unit].api_status_count = 0;
#endif /* DNX_ERR_RECOVERY_VALIDATION */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        SHR_IF_ERR_EXIT(dnx_er_regress_top_level_api_flags_reset(unit));
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
    }

    dnx_err_recovery_manager[unit].api_counter--;

    /*
     * sanity check if there is a transaction that has been left open after the counter has been decremented.
     */
    if (dnx_err_recovery_manager[unit].api_to_trans_map[dnx_err_recovery_manager[unit].api_counter] !=
        dnx_err_recovery_manager[unit].transaction_counter)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "err recovery ERROR: API opens a transaction but never closes it.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    reset_flags = FALSE;

exit:
    if (reset_flags)
    {
        if (1 == dnx_err_recovery_manager[unit].api_counter)
        {
#ifdef DNX_ERR_RECOVERY_VALIDATION
            dnx_err_recovery_manager[unit].api_status = DNX_ERR_RECOVERY_API_STATUS_UNKNOWN;
            dnx_err_recovery_manager[unit].api_status_count = 0;
#endif /* DNX_ERR_RECOVERY_VALIDATION */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
            dnx_er_regress_top_level_api_flags_reset(unit);
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
        }
        dnx_err_recovery_manager[unit].api_counter--;
    }
    SHR_FUNC_EXIT;
}
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */

#ifdef DNX_ERR_RECOVERY_VALIDATION
/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_not_needed(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    /*
     * api cannot be marked as excluded started from threads that are members of the temporary or permanent thread exclusion lists
     */
    DNX_ER_THREADING_EXIT_IF_NOT_MAIN_THREAD(unit);

    /*
     * no need to error check it since api_status can already be set in cases of nested APIs
     */
    if (dnx_err_recovery_manager[unit].api_status == DNX_ERR_RECOVERY_API_STATUS_UNKNOWN)
    {
        dnx_err_recovery_manager[unit].api_status = DNX_ERR_RECOVERY_API_NOT_NEEDED;
    }

    /*
     * In all cases we need to increment the counter
     */
    dnx_err_recovery_manager[unit].api_status_count++;

exit:
    SHR_FUNC_EXIT;
}

#endif /* DNX_ERR_RECOVERY_VALIDATION */

/*
 * \brief - basic journaling prerequisites
 */
static uint8
dnx_err_recovery_are_prerequisites_met(
    int unit)
{
    return (SOC_UNIT_VALID(unit) && dnx_err_recovery_common_is_enabled(unit)
            && dnx_rollback_journal_is_done_init(unit) && !SOC_IS_DETACHING(unit)
            && dnx_err_recovery_common_flag_is_on(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                                  DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED));
}

static uint8
dnx_err_recovery_is_on(
    int unit)
{
    return dnx_err_recovery_common_is_on(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK)
        && dnx_er_threading_is_journaling_thread(unit);
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_is_on_test(
    int unit)
{
    return dnx_err_recovery_is_on(unit);
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_tmp_allow(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);
    DNX_ER_THREADING_EXIT_IF_NOT_JOURNALING_THREAD(unit);
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit);

    if (!is_on)
    {
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        SHR_IF_ERR_EXIT(dnx_er_regress_comparison_tmp_allow(unit, is_on));
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

        dnx_err_recovery_manager[unit].is_suppressed_counter++;
        dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
    }
    else
    {

        if (0 == dnx_err_recovery_manager[unit].is_suppressed_counter)
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: Suppression counter cannot be less than 0.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }

        dnx_err_recovery_manager[unit].is_suppressed_counter--;
        if (0 == dnx_err_recovery_manager[unit].is_suppressed_counter)
        {
            dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                             DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
        }

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        SHR_IF_ERR_EXIT(dnx_er_regress_comparison_tmp_allow(unit, is_on));
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION
/*
 * see .h file for description
 */
uint8
dnx_err_recovery_hw_access_is_valid(
    int unit)
{
    /*
     * if error recovery is off, all hw access is valid
     */
    if (!dnx_err_recovery_is_on(unit))
    {
        return TRUE;
    }

    if (dnx_dbal_journal_is_tmp_suppressed_unsafe(unit, FALSE))
    {
        return TRUE;
    }

    /*
     * check if we are currently in a dbal access journal
     */
    return dnx_err_recovery_common_is_dbal_access_region(unit);
}
#endif /* DNX_ERR_RECOVERY_VALIDATION */

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_unsupported_dbal_unsup_tbls_bypass(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);
    DNX_ER_THREADING_EXIT_IF_NOT_JOURNALING_THREAD(unit);
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_journal_unsupported_tables_bypass(unit, is_on));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_transaction_no_support_counter_inc(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    /*
     * - suppress if active transaction on the current thread
     * - mark that no new transaction can started otherwise
     */
    if (dnx_err_recovery_manager[unit].transaction_counter != 0 && dnx_er_threading_is_journaling_thread(unit))
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_all_tmp_suppress_unsafe(unit, TRUE));
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_all_tmp_suppress_unsafe(unit, TRUE));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_all_tmp_suppress_unsafe(unit, TRUE));
    }
    else
    {
        dnx_er_threading_tmp_excluded_add(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_transaction_no_support_counter_dec(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    /*
     * - unsuppress if active transaction on the current thread
     * - remove transaction restriction on the current thread otherwise
     */
    if (dnx_err_recovery_manager[unit].transaction_counter != 0 && dnx_er_threading_is_journaling_thread(unit))
    {
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_all_tmp_suppress_unsafe(unit, FALSE));
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_all_tmp_suppress_unsafe(unit, FALSE));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_all_tmp_suppress_unsafe(unit, FALSE));
    }
    else
    {
        dnx_er_threading_tmp_excluded_remove(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - start error recovery transaction
 */
static shr_error_e
dnx_err_recovery_transaction_start_internal(
    int unit)
{
    uint8 is_top_level_transaction = (0 == dnx_err_recovery_manager[unit].transaction_counter);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * change the start api only if it is first one to opt in for error recovery,
     */
    if (is_top_level_transaction)
    {
#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (!dnx_err_recovery_common_flag_is_on
            (unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED))
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: transaction cannot be invalidated before it has been started.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (0 != dnx_err_recovery_manager[unit].is_suppressed_counter)
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: transaction cannot be suppressed before it has been started.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (dnx_dbal_journal_are_unsupported_tables_bypassed(unit))
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: transaction cannot be started with temporary bypass of unsupported dbal tables set.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (0 != dnx_err_recovery_manager[unit].entry_counter)
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: entries found prior to transaction start.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

#endif /* DNX_ERR_RECOVERY_VALIDATION */

        /*
         * lock the transaction to a single thread.
         */
        SHR_IF_ERR_EXIT(dnx_er_threading_journaling_thread_set(unit));
    }

    dnx_err_recovery_manager[unit].transaction_counter++;
    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_err_recovery_transaction_bookmark_set(unit, dnx_err_recovery_manager[unit].transaction_counter,
                                              dnx_err_recovery_manager[unit].entry_counter);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (dnx_err_recovery_manager[unit].api_status == DNX_ERR_RECOVERY_API_STATUS_UNKNOWN)
    {
        dnx_err_recovery_manager[unit].api_status = DNX_ERR_RECOVERY_API_OPTS_IN;
    }

    /*
     * In all cases we need to increment the counter.
     * Currently we allow internal DNX APIs are allowed to change the counter.
     */
    dnx_err_recovery_manager[unit].api_status_count++;
#endif /* DNX_ERR_RECOVERY_VALIDATION */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    SHR_IF_ERR_EXIT(dnx_er_regress_transaction_start(unit, is_top_level_transaction));
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_transaction_start(
    int unit,
    bcm_switch_module_t module_id)
{
    uint8 enabled = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    /*
     * transaction cannot be started from threads that are members of the temporary or permanent thread exclusion lists
     */
    DNX_ER_THREADING_EXIT_IF_IS_EXCLUDED_THREAD(unit);

    /*
     * check if another transaction is active on other threads.
     */
    SHR_IF_ERR_EXIT(dnx_er_threading_no_parallel_transactions_verify(unit));

    /*
     * check if ER is disabled for module
     */
    if (bcmModuleCount != module_id)
    {
        switch_db.module_error_recovery.get(unit, module_id, &enabled);
        if (!enabled)
        {
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start_internal(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
static shr_error_e
dnx_err_recovery_transaction_end_internal(
    int unit,
    int api_result)
{
    uint8 reset_flags = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * only for the outermost level transaction do sanity checks and commit
     */
    if (1 == (dnx_err_recovery_manager[unit].transaction_counter))
    {

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * check if suppression hasn't been turned off prior to the end of the function
         */
        if (0 != dnx_err_recovery_manager[unit].is_suppressed_counter)
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: temporary suppression is not disabled at the end of the transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        /*
         * check if we are ending the transaction with a non-zero
         * unsupported dbal bypass counter
         */
        if (dnx_dbal_journal_are_unsupported_tables_bypassed(unit))
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: temporary bypass of unsupported dbal table is not disabled at the end of transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        /*
         * check if any unsupported features were accessed during the transaction
         */
        if (!dnx_err_recovery_common_flag_is_on
            (unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED))
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: unsupported features were accessed during the transaction.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

        if (SHR_FAILURE(api_result)
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
            || dnx_er_regress_transaction_end_should_abort(unit)
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
            )
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_abort_unsafe(unit));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_commit_unsafe(unit));
        }
    }
    else
    {
        /*
         * rollback nested transaction
         */
        if (SHR_FAILURE(api_result)
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
            || dnx_er_regress_transaction_end_should_abort(unit)
#endif
            )
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_abort_unsafe(unit));
        }
    }

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    SHR_IF_ERR_EXIT(dnx_er_regress_transaction_end(unit));
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    dnx_err_recovery_manager[unit].transaction_counter--;

    /*
     * clear flags so that we don't leave manager internals inconsistent after the transaction
     */
    if (0 == dnx_err_recovery_manager[unit].transaction_counter)
    {
        dnx_err_recovery_transaction_counters_reset(unit);
        SHR_IF_ERR_EXIT(dnx_er_threading_journaling_thread_clear(unit));
    }

    reset_flags = FALSE;
exit:
    if (reset_flags)
    {
        dnx_err_recovery_manager[unit].transaction_counter--;
        if (0 == dnx_err_recovery_manager[unit].transaction_counter)
        {
            dnx_err_recovery_transaction_counters_reset(unit);
            dnx_er_threading_journaling_thread_clear(unit);
        }
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_transaction_end(
    int unit,
    bcm_switch_module_t module_id,
    int api_result)
{
    uint8 enabled = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);
    DNX_ER_THREADING_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    /*
     * check if ER is disabled for module
     */
    if (bcmModuleCount != module_id)
    {
        switch_db.module_error_recovery.get(unit, module_id, &enabled);
        if (!enabled)
        {
            SHR_EXIT();
        }
    }

    /*
     * errors during transaction end should fail DNX APIs only testing
     */
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    if (dnx_er_regress_transaction_fail_should_return_error(unit))
    {
        SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_end_internal(unit, api_result));
    }
    else
#endif
    {
        dnx_err_recovery_transaction_end_internal(unit, api_result);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_rollback_journal_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);

    /*
     * transaction cannot be started from threads that are members of the temporary or permanent thread exclusion lists
     */
    DNX_ER_THREADING_EXIT_IF_IS_EXCLUDED_THREAD(unit);

    /*
     * check if another transaction is active on other threads.
     */
    SHR_IF_ERR_EXIT(dnx_er_threading_no_parallel_transactions_verify(unit));

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    SHR_IF_ERR_EXIT(dnx_er_regress_journal_start(unit));
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start_internal(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_rollback_journal_end(
    int unit,
    uint8 rollback_journal)
{
    shr_error_e error_type = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);
    DNX_ER_THREADING_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    if (rollback_journal)
    {
        error_type = _SHR_E_INTERNAL;
    }

    /*
     * pretend api failed, rollback journals
     */
    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_end_internal(unit, error_type));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_transaction_invalidate(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_PREREQUESITES_NOT_MET(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);
    DNX_ER_THREADING_EXIT_IF_NOT_JOURNALING_THREAD(unit);
    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit);

    dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);

    /*
     * clear the transaction and return an unsupported error
     */
    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_clear(unit));

    SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: feature not supported.\n%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_rollback_journal_appl_init_state_change(
    int unit,
    uint8 is_init)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    SHR_IF_ERR_EXIT(dnx_er_regress_appl_init_state_change(unit, is_init));
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_rollback_journal_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_print(unit, FALSE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_print(unit, FALSE));
            break;
        default:
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: unrecognized journal type.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_rollback_journal_logger_state_change(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_logger_state_change(unit, FALSE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_logger_state_change(unit, FALSE));
            break;
        default:
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "err recovery ERROR: unrecognized journal type.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int
dnx_tcl_rollback_journal_start(
    int unit)
{
    return dnx_rollback_journal_start(unit);
}

/*
 * see .h file for description
 */
int
dnx_tcl_rollback_journal_end(
    int unit,
    uint8 rollback_journal)
{
    return dnx_rollback_journal_end(unit, rollback_journal);

}

/*
 * see .h file for description
 */
int
dnx_tcl_rollback_journal_suppress(
    int unit)
{
    return dnx_err_recovery_tmp_allow(unit, FALSE);

}

/*
 * see .h file for description
 */
int
dnx_tcl_rollback_journal_unsuppress(
    int unit)
{
    return dnx_err_recovery_tmp_allow(unit, TRUE);
}

/*
 * see .h file for description
 */
int
dnx_tcl_rollback_journal_is_initialized(
    int unit)
{
    return dnx_err_recovery_common_flag_is_on(unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                              DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);
}

/*
 * see .h file for description
 */
int
dnx_tcl_rollback_journal_diagnostics_test_end(
    int unit,
    char *testName)
{
#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS
    dnx_rollback_journal_diag_print(unit, testName);
    return dnx_rollback_journal_diag_clean(unit);
#else
    return 0;
#endif /* DNX_ROLLBACK_JOURNAL_DIAGNOSTICS */
}

#undef _ERR_MSG_MODULE_NAME
