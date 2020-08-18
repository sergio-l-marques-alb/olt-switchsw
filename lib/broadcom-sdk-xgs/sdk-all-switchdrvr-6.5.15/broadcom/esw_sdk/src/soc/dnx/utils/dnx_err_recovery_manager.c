/** \file dnx_err_recovery_manager.c
 * This module is the Error Recovery manager.
 * It is the interface to the Error Recovery feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
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
#include <soc/dnxc/dnxc_generic_state_journal.h>
#include <soc/dnxc/dnxc_rollback_journal.h>
#include <soc/dnxc/dnxc_rollback_journal_diag.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/dnxc_rollback_journal_utils.h>
#include <soc/dnx/dnx_err_recovery_manager_utils.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
#include <soc/dnx/dnx_state_snapshot_manager.h>
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * \brief - exit of not DNX chip
 */
#define DNX_ERR_RECOVERY_EXIT_IF_NOT_DNX(unit)\
do{                                           \
    if(!SOC_IS_DNX(unit))                     \
    {                                         \
        SHR_EXIT();                           \
    }                                         \
}while(0)

/*
 * \brief - exit if the feature is not initialized
 */
#define DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit)          \
do{                                                             \
    if (TRUE != dnx_err_recovery_manager[unit].is_initialized) {\
        SHR_EXIT();                                             \
    }                                                           \
}while(0)

/*
 * \brief - exit if current thread is not in the set of allowed journaling threads.
 */
#define DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit) \
do{                                                       \
    if (dnx_err_recovery_utils_is_excluded_thread(unit)) {\
        SHR_EXIT();                                       \
    }                                                     \
}while(0)

/*
 * \brief - exit if no api has opted in for error recovery.
 */
#define DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit)           \
do {                                                             \
    if(0 == dnx_err_recovery_manager[unit].transaction_counter) {\
        SHR_EXIT();                                              \
    }                                                            \
}                                                                \
while(0)

/*
 * \brief - exit if no api has opted in for error recovery.
 */
#define DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit)              \
do {                                                            \
    if(TRUE == dnx_err_recovery_manager[unit].is_invalidated) { \
        SHR_EXIT();                                             \
    }                                                           \
}                                                               \
while(0)

/*
 * \brief - exit if in "No Support" region
 */
#define DNX_ERR_RECOVERY_EXIT_IF_NO_SUPPORT(unit)               \
do {                                                            \
    if (0 != dnx_err_recovery_manager[unit].no_support_counter){\
        SHR_EXIT();                                             \
    }                                                           \
}                                                               \
while(0)

err_recovery_manager_t dnx_err_recovery_manager[SOC_MAX_NUM_DEVICES] = { {0}
};

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
static void dnx_err_recovery_regression_swstate_access_cb(
    int unit);
static void dnx_err_recovery_regression_dbal_access_cb(
    int unit);
static void dnx_err_recovery_regression_print_api_and_iteration(
    int unit,
    uint8 is_begin);
static void dnx_err_recovery_regression_system_state_modified(
    int unit);
static uint8 dnx_err_recovery_regression_testing_conditions_are_met(
    int unit);
static uint8 dnx_err_recovery_regression_is_first_test_iteration(
    int unit);
static uint8 dnx_err_recovery_regression_is_second_test_iteration(
    int unit);
static uint8 dnx_err_recovery_is_first_transaction_in_top_level_api(
    int unit);
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

static uint8 dnx_err_recovery_is_suppression_flag_on(
    int unit,
    uint32 flag);

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

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    /*
     * clear rollback journals
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear(unit, FALSE, FALSE));
    SHR_IF_ERR_EXIT(dnx_dbal_journal_clear(unit, FALSE, FALSE));
    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_clear(unit, FALSE));

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

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    /*
     * clear journals
     */
    while (dnx_err_recovery_manager[unit].entry_counter > 0)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, FALSE, &sw_state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnxc_generic_state_journal_get_head_seq_id(unit, &state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, FALSE, &dbal_entry_seq_id));

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * check if entry sequence id is not found in either journals
         */
        if ((sw_state_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter)
            && (state_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter)
            && (dbal_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: entry sequence id not found in journals.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }
#endif

        /*
         * clear dbal or sw state journal entry
         */
        if (sw_state_entry_seq_id == dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear(unit, TRUE, FALSE));
        }
        else if (state_entry_seq_id == dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnxc_generic_state_journal_clear(unit, TRUE));
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
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "err recovery ERROR: entry count non-zero after journals cleared.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif

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
        SHR_IF_ERR_EXIT(dnxc_generic_state_journal_get_head_seq_id(unit, &state_entry_seq_id));
        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, FALSE, &dbal_entry_seq_id));

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * check if entry sequence id is not found in either journals
         */
        if ((sw_state_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter)
            && (state_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter)
            && (dbal_entry_seq_id != dnx_err_recovery_manager[unit].entry_counter))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: entry sequence id not found in journals.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }
#endif

        /*
         * revert dbal or sw state journal entry
         */
        if (sw_state_entry_seq_id == dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back(unit, TRUE, FALSE));
        }
        else if (state_entry_seq_id == dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT(dnxc_generic_state_journal_roll_back(unit, TRUE));
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

/**
 * \brief - check if error recovery suppression flag is on
 */
static uint8
dnx_err_recovery_is_suppression_flag_on(
    int unit,
    uint32 flag)
{
    return ((dnx_err_recovery_manager[unit].is_suppressed_err & flag) != 0);
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
    return dnx_err_recovery_is_on(unit) && (0 == dnx_err_recovery_manager[unit].dbal_access_region_counter);
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_initialize(
    int unit)
{
    dnxc_rollback_journal_cbs_t swstate_cbs;
    dnxc_rollback_journal_cbs_t dbal_cbs;
    dnxc_rollback_journal_cbs_t state_cbs;

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    const char *err_recovery_system_name = "ERROR-RECOVERY-REGRESSION";
    char *system_indicator_name = NULL;

    dnxc_sw_state_journal_access_cb swstate_access_cb = &dnx_err_recovery_regression_swstate_access_cb;
    dnxc_sw_state_journal_access_cb dbal_access_cb = &dnx_err_recovery_regression_dbal_access_cb;
#else
    dnxc_sw_state_journal_access_cb swstate_access_cb = NULL;
    dnxc_sw_state_journal_access_cb dbal_access_cb = NULL;
#endif

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize Error Recovery only for dnx chips
     */
    DNX_ERR_RECOVERY_EXIT_IF_NOT_DNX(unit);

    if (TRUE == dnx_err_recovery_manager[unit].is_initialized)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: already initialized .\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    /*
     * if error recovery is disabled at compile-time, don't initialize
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        SHR_EXIT();
    }

    swstate_cbs.is_on = &dnx_err_recovery_is_sw_state_journal_on;
    swstate_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_initialize(unit, swstate_cbs, swstate_access_cb, FALSE));

    dbal_cbs.is_on = &dnx_err_recovery_is_on;
    dbal_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dbal_cbs, dbal_access_cb, FALSE));

    state_cbs.is_on = &dnx_err_recovery_is_on;
    state_cbs.seq_id_get = &dnx_err_recovery_new_journal_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_initialize(unit, state_cbs));

    dnx_err_recovery_manager[unit].is_initialized = TRUE;
    dnx_err_recovery_manager[unit].is_suppressed_counter = 0;
    dnx_err_recovery_manager[unit].is_invalidated = FALSE;
    dnx_err_recovery_manager[unit].no_support_counter = 0;
    dnx_err_recovery_manager[unit].transaction_counter = 0;
    dnx_err_recovery_manager[unit].entry_counter = 0;
    dnx_err_recovery_manager[unit].dbal_access_region_counter = 0;
    dnx_err_recovery_manager[unit].is_suppressed_err = DNX_ERR_RECOVERY_ERR_SUPPRESS_NONE;
    dnx_err_recovery_manager[unit].is_appl_init_done = FALSE;

    SHR_IF_ERR_EXIT(dnx_err_recovery_utils_initialize(unit));

#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnx_err_recovery_manager[unit].api_counter = 0;
    sal_memset(dnx_err_recovery_manager[unit].api_to_trans_map, 0x0,
               sizeof(dnx_err_recovery_manager[unit].api_to_trans_map));
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    dnx_err_recovery_manager[unit].is_api_excluded = FALSE;
    dnx_err_recovery_manager[unit].is_testing_enabled = FALSE;
    dnx_err_recovery_manager[unit].has_access_outside_trans = FALSE;

    system_indicator_name = soc_property_get_str(unit, "dvapi_system_indicator");
    if ((NULL != system_indicator_name)
        && (sal_strncmp(system_indicator_name, err_recovery_system_name, sal_strlen(err_recovery_system_name)) == 0))
    {
        dnx_err_recovery_manager[unit].is_testing_enabled = TRUE;
    }
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

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
    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_destroy(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_err_recovery_utils_deinitialize(unit));

    dnx_err_recovery_manager[unit].is_initialized = FALSE;
    dnx_err_recovery_manager[unit].is_suppressed_counter = 0;
    dnx_err_recovery_manager[unit].is_invalidated = FALSE;
    dnx_err_recovery_manager[unit].no_support_counter = 0;
    dnx_err_recovery_manager[unit].transaction_counter = 0;
    dnx_err_recovery_manager[unit].entry_counter = 0;
    dnx_err_recovery_manager[unit].dbal_access_region_counter = 0;
    dnx_err_recovery_manager[unit].is_suppressed_err = DNX_ERR_RECOVERY_ERR_SUPPRESS_NONE;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnx_err_recovery_manager[unit].api_counter = 0;
    sal_memset(dnx_err_recovery_manager[unit].api_to_trans_map, 0x0,
               sizeof(dnx_err_recovery_manager[unit].api_to_trans_map));
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    dnx_err_recovery_manager[unit].is_api_excluded = FALSE;
    dnx_err_recovery_manager[unit].is_testing_enabled = FALSE;
    dnx_err_recovery_manager[unit].has_access_outside_trans = FALSE;
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION
/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_api_counter_inc(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!(SOC_UNIT_VALID(unit)))
    {
        return SOC_E_NONE;
    }

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    dnx_err_recovery_manager[unit].api_counter++;

    if (dnx_err_recovery_manager[unit].api_counter > DNX_ERR_RECOVERY_MAX_NUM_NESTED_APIS)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: API counter cannot be more than 3.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    /*
     * save which transaction is at the beginning of the API.
     * used for sanity check at the API decrement
     */
    dnx_err_recovery_manager[unit].api_to_trans_map[dnx_err_recovery_manager[unit].api_counter] =
        dnx_err_recovery_manager[unit].transaction_counter;

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    if (1 == dnx_err_recovery_manager[unit].api_counter)
    {
        dnx_err_recovery_manager[unit].non_nested_trans_in_api_count = 0;
        dnx_err_recovery_manager[unit].is_api_excluded = FALSE;
        dnx_err_recovery_manager[unit].is_api_ran = FALSE;
        dnx_err_recovery_manager[unit].has_access_outside_trans = FALSE;
    }
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_api_counter_dec(
    int unit)
{
    uint8 reset_flags = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (!(SOC_UNIT_VALID(unit)))
    {
        return SOC_E_NONE;
    }

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    if (0 == dnx_err_recovery_manager[unit].api_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "err recovery ERROR: API counter cannot be less than 0.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    reset_flags = TRUE;

    /*
     * sanity check if there is an open transaction
     */
    if (dnx_err_recovery_manager[unit].api_to_trans_map[dnx_err_recovery_manager[unit].api_counter] !=
        dnx_err_recovery_manager[unit].transaction_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "err recovery ERROR: API opens a transaction but never closes it.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    if (1 == dnx_err_recovery_manager[unit].api_counter)
    {
        dnx_err_recovery_manager[unit].non_nested_trans_in_api_count = 0;
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        dnx_err_recovery_manager[unit].is_api_excluded = FALSE;
        dnx_err_recovery_manager[unit].is_api_ran = FALSE;
        dnx_err_recovery_manager[unit].has_access_outside_trans = FALSE;
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
    }
    dnx_err_recovery_manager[unit].api_counter--;

    reset_flags = FALSE;

exit:
    if (reset_flags)
    {
        if (1 == dnx_err_recovery_manager[unit].api_counter)
        {
            dnx_err_recovery_manager[unit].non_nested_trans_in_api_count = 0;
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
            dnx_err_recovery_manager[unit].is_api_excluded = FALSE;
            dnx_err_recovery_manager[unit].is_api_ran = FALSE;
            dnx_err_recovery_manager[unit].has_access_outside_trans = FALSE;
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
        }
        dnx_err_recovery_manager[unit].api_counter--;
    }
    SHR_FUNC_EXIT;
}
#endif

/*
 * \brief - basic journaling conditions
 */
static uint8
dnx_err_recovery_is_on_basic(
    int unit)
{
    /*
     * Basic journaling conditions
     * - We are not in soc init / deinit
     * - Feature is initialized
     * - We are currently in an error recovery transaction
     * - No journal is currently rolling back
     * - The current thread is in the set of allowed journaling threads
     */
    return (SOC_UNIT_VALID(unit)
            && SOC_IS_DONE_INIT(unit)
            && !SOC_IS_DETACHING(unit)
            && (dnx_err_recovery_manager[unit].is_initialized)
            && (0 != dnx_err_recovery_manager[unit].transaction_counter)
            && (!dnxc_rollback_journal_is_any_journal_rolling_back(unit))
            && (!dnx_err_recovery_utils_is_excluded_thread(unit)));
}

/*
 * \brief - check if error recovery is invalidated or suppressed
 */
static uint8
dnx_err_recovery_is_tmp_suspended(
    int unit)
{
    return ((0 != dnx_err_recovery_manager[unit].is_suppressed_counter)
            || (dnx_err_recovery_manager[unit].is_invalidated));
}

/*
 * see .h file for description
 */
uint8
dnx_err_recovery_is_on(
    int unit)
{
    /*
     * Error recovery is considered to be on only if:
     * - Basic journaling conditions are all met
     * - Feature is not temporary suspended
     */
    return (dnx_err_recovery_is_on_basic(unit) && !dnx_err_recovery_is_tmp_suspended(unit));
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

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);

    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    if (!is_on)
    {
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        /*
         * comparison journal should also be suppressed during the simulation run
         * for each rollback journal suppression.
         */
        if (dnx_err_recovery_regression_is_first_test_iteration(unit))
        {
            SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));
        }
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

        dnx_err_recovery_manager[unit].is_suppressed_counter++;
    }
    else
    {

        if (0 == dnx_err_recovery_manager[unit].is_suppressed_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: Suppression counter cannot be less than 0.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }

        dnx_err_recovery_manager[unit].is_suppressed_counter--;

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        if (dnx_err_recovery_regression_is_first_test_iteration(unit))
        {
            SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));
        }
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

    /*
     * check if we are currently in a dbal access journal
     */
    return ((0 < dnx_err_recovery_manager[unit].dbal_access_region_counter)
            || dnx_err_recovery_is_suppression_flag_on(unit, DNX_ERR_RECOVERY_ERR_SUPPRESS_HW_ACCESS));
}
#endif

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_unsupported_dbal_unsup_tbls_bypass(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);

    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

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

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    dnx_err_recovery_manager[unit].no_support_counter++;

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

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    if (0 == dnx_err_recovery_manager[unit].no_support_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "err recovery ERROR: No Support counter cannot be less than 0.\n%s%s%s", EMPTY, EMPTY,
                                 EMPTY);
    }

    dnx_err_recovery_manager[unit].no_support_counter--;

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
    SHR_FUNC_INIT_VARS(unit);

    /*
     * change the start api only if it is first one to opt in for error recovery,
     */
    if (0 == dnx_err_recovery_manager[unit].transaction_counter)
    {

#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (dnx_err_recovery_manager[unit].is_invalidated)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: transaction cannot be invalidated before it has been started.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        if (0 != dnx_err_recovery_manager[unit].is_suppressed_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: transaction cannot be suppressed before it has been started.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        if (dnx_dbal_journal_are_unsupported_tables_bypassed(unit))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: transaction cannot be started with temporary bypass of unsupported dbal tables set.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        if (0 != dnx_err_recovery_manager[unit].entry_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: entries found prior to transaction start.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
#endif

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
        /*
         * don't allow access outside of a transaction while testing
         */
        if (dnx_err_recovery_regression_testing_conditions_are_met(unit)
            && dnx_err_recovery_manager[unit].has_access_outside_trans)
        {
            LOG_CLI((BSL_META_U
                     (unit, "error recovery regression ERROR: hw or swstate access outside of transaction\n")));
            assert(0);
        }
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    }

    dnx_err_recovery_manager[unit].transaction_counter++;

    dnx_err_recovery_transaction_bookmark_set(unit, dnx_err_recovery_manager[unit].transaction_counter,
                                              dnx_err_recovery_manager[unit].entry_counter);

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    /*
     * comparison journal start to validate rollback after first iteration
     */
    if (dnx_err_recovery_regression_is_first_test_iteration(unit))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
    }

    /*
     * print API name and iteration count
     */
    dnx_err_recovery_regression_print_api_and_iteration(unit, TRUE);
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    /*
     * check for sibling transaction violations, indicate that at least one transaction was started in the top level bcm api
     */
    if (dnx_err_recovery_is_first_transaction_in_top_level_api(unit))
    {
        dnx_err_recovery_manager[unit].non_nested_trans_in_api_count++;

        if (dnx_err_recovery_regression_testing_conditions_are_met(unit)
            && !dnx_err_recovery_is_suppression_flag_on(unit, DNX_ERR_RECOVERY_ERR_SUPPRESS_SIBLING_TRANSACTIONS)
            && (dnx_err_recovery_manager[unit].non_nested_trans_in_api_count > 1))
        {
            LOG_CLI((BSL_META_U
                     (unit, "err recovery ERROR: Multiple top-level transactions in a top level bcm API detected.\n")));
            assert(0);
        }
    }
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
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);
    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_SUPPORT(unit);

    /*
     * errors during transaction start should fail DNX APIs only testing
     */
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    if (dnx_err_recovery_regression_testing_conditions_are_met(unit))
    {
        SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start_internal(unit));
    }
    else
#endif
    {
        dnx_err_recovery_transaction_start_internal(unit);
    }

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
    uint8 reset_flags = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == dnx_err_recovery_manager[unit].transaction_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "err recovery ERROR: transaction end called outside of transaction.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    reset_flags = TRUE;

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
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: temporary suppression is not disabled at the end of the transaction.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        /*
         * check if we are ending the transaction with a non-zero 
         * unsupported dbal bypass counter
         */
        if (dnx_dbal_journal_are_unsupported_tables_bypassed(unit))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: temporary bypass of unsupported dbal table is not disabled at the end of transaction.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        /*
         * check if any unsupported features were accessed during the transaction
         */
        if (dnx_err_recovery_manager[unit].is_invalidated)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "err recovery ERROR: unsupported features were accessed during the transaction.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
#endif

        if (SHR_FAILURE(api_result)
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
            || dnx_err_recovery_regression_is_first_test_iteration(unit)
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
            || dnx_err_recovery_regression_is_first_test_iteration(unit)
#endif
            )
        {
            SHR_IF_ERR_EXIT(dnx_err_recovery_abort_unsafe(unit));
        }
    }

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    /*
     * print end of API testing
     */
    dnx_err_recovery_regression_print_api_and_iteration(unit, FALSE);

    /*
     * comparison journal validate rollback after first iteration rollback
     */
    if (dnx_err_recovery_regression_is_first_test_iteration(unit))
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
    }
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

    dnx_err_recovery_manager[unit].transaction_counter--;

    /*
     * clear flags so that we don't leave manager internals inconsistent after the transaction
     */
    if (0 == dnx_err_recovery_manager[unit].transaction_counter)
    {
        dnx_err_recovery_manager[unit].is_invalidated = FALSE;
        dnx_err_recovery_manager[unit].is_suppressed_counter = 0;
    }

    reset_flags = FALSE;
exit:
    if (reset_flags)
    {
        dnx_err_recovery_manager[unit].transaction_counter--;
        if (0 == dnx_err_recovery_manager[unit].transaction_counter)
        {
            dnx_err_recovery_manager[unit].is_invalidated = FALSE;
            dnx_err_recovery_manager[unit].is_suppressed_counter = 0;
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
    int api_result)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);
    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_SUPPORT(unit);

    /*
     * errors during transaction end should fail DNX APIs only testing
     */
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    if (dnx_err_recovery_regression_testing_conditions_are_met(unit))
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

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);
    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_SUPPORT(unit);

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    /*
     * use generic API name for regression testing prints
     */
    dnx_err_recovery_regression_testing_api_name_update(unit, "ERROR RECOVERY CLEANUP");
#endif

    SHR_IF_ERR_EXIT(dnx_err_recovery_transaction_start_internal(unit));

    SHR_EXIT();
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

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);
    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);
    DNX_ERR_RECOVERY_EXIT_IF_NO_SUPPORT(unit);

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

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);

    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    if (dnx_err_recovery_is_suppression_flag_on(unit, DNX_ERR_RECOVERY_ERR_SUPPRESS_INVALIDATE))
    {
        SHR_EXIT();
    }

    dnx_err_recovery_manager[unit].is_invalidated = TRUE;

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
dnx_err_recovery_manager_dbal_access_region_inc(
    int unit,
    dbal_action_access_type_e access_type,
    dbal_action_access_func_e access_func)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_manager[unit].dbal_access_region_counter++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_manager_dbal_access_region_dec(
    int unit,
    dbal_action_access_type_e access_type,
    dbal_action_access_func_e access_func)
{
    SHR_FUNC_INIT_VARS(unit);

    if (0 == dnx_err_recovery_manager[unit].dbal_access_region_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "err recovery manager ERROR: attempted to end dbal access region without start.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    dnx_err_recovery_manager[unit].dbal_access_region_counter--;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_suppress_errors(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    /*
     * swstate violations are suppressed at the beginning of dbal access outside of transaction
     */
    if (0 == (flags & DNX_ERR_RECOVERY_ERR_SUPPRESS_SW_ACCESS_OUTSIDE_TRANS))
    {
        DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);
    }

    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    dnx_err_recovery_manager[unit].is_suppressed_err |= flags;

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_unsuppress_errors(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_EXIT_IF_NOT_INITIALIZED(unit);

    /*
     * swstate violations are unsuppressed at the end of dbal access outside of transaction
     */
    if (0 == (flags & DNX_ERR_RECOVERY_ERR_SUPPRESS_SW_ACCESS_OUTSIDE_TRANS))
    {
        DNX_ERR_RECOVERY_EXIT_IF_NO_API_OPTED_IN(unit);
    }

    DNX_ERR_RECOVERY_EXIT_IF_INVALIDATED(unit);

    DNX_ERR_RECOVERY_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    dnx_err_recovery_manager[unit].is_suppressed_err &= ~flags;

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

    dnx_err_recovery_manager[unit].is_appl_init_done = (is_init) ? TRUE : FALSE;

#ifdef DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS
    if (is_init)
    {
        /*
         * initialize the diagnostics module even if error recovery is disabled
         */
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_diag_init(unit));
    }
    else
    {
        /*
         * deinitialize if error recovery has not been initialized
         */
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_diag_deinit(unit));
    }
#endif /* DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_rollback_journal_print_journals(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_print(unit, FALSE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_print(unit, FALSE));

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
#ifdef BCM_WARM_BOOT_API_TEST
    if (dnx_err_recovery_manager[unit].transaction_counter == 0)
    {
        dnxc_wb_no_wb_test_set(unit, 1);
    }
#endif /* BCM_WARM_BOOT_API_TEST */

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
#ifdef BCM_WARM_BOOT_API_TEST
    if (dnx_err_recovery_manager[unit].transaction_counter == 0)
    {
        dnxc_wb_no_wb_test_set(unit, 0);
    }
#endif /* BCM_WARM_BOOT_API_TEST */

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
    return (dnx_err_recovery_manager[unit].is_initialized ? 1 : 0);
}

/*
 * see .h file for description
 */
int
dnx_tcl_rollback_journal_diagnostics_test_end(
    int unit,
    char *testName)
{
#ifdef DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS
    dnxc_rollback_journal_diag_print(unit, testName);
    return dnxc_rollback_journal_diag_clean(unit);
#else
    return 0;
#endif /* DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS */
}

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
/*
 * \brief - Callback used each time swstate is being accessed
 */
static void
dnx_err_recovery_regression_swstate_access_cb(
    int unit)
{
    if (dnx_err_recovery_manager[unit].is_testing_enabled
        && !dnx_err_recovery_is_suppression_flag_on(unit, DNX_ERR_RECOVERY_ERR_SUPPRESS_SW_ACCESS_OUTSIDE_TRANS))
    {
        dnx_err_recovery_regression_system_state_modified(unit);
    }
}

/*
 * \brief - Callback used each time dbal is being accessed
 */
static void
dnx_err_recovery_regression_dbal_access_cb(
    int unit)
{
    if (dnx_err_recovery_manager[unit].is_testing_enabled
        && !dnx_err_recovery_is_suppression_flag_on(unit, DNX_ERR_RECOVERY_ERR_SUPPRESS_DBAL_ACCESS_OUTSIDE_TRANS))
    {
        dnx_err_recovery_regression_system_state_modified(unit);
    }
}

/*
 * \brief - print API name and test iteration counter
 */
static void
dnx_err_recovery_regression_print_api_and_iteration(
    int unit,
    uint8 is_begin)
{
    if (is_begin)
    {
        if (dnx_err_recovery_regression_is_first_test_iteration(unit))
        {
            LOG_CLI((BSL_META_U(unit, "**** ER BCM API %s **** ERROR SIMULATION RUN ****\n"),
                     dnx_err_recovery_manager[unit].dnx_err_recovery_regression_testing_api_name));
        }
        else if (dnx_err_recovery_regression_is_second_test_iteration(unit))
        {
            LOG_CLI((BSL_META_U(unit, "**** ER BCM API %s **** NORMAL RUN ****\n"),
                     dnx_err_recovery_manager[unit].dnx_err_recovery_regression_testing_api_name));
        }
    }
    else
    {
        if (dnx_err_recovery_regression_is_second_test_iteration(unit))
        {
            LOG_CLI((BSL_META_U(unit, "**** ER BCM test finished successfully ****\n")));
        }
    }
}

/*
 * \brief - check if all testing conditions are met 
 */
static uint8
dnx_err_recovery_regression_testing_conditions_are_met(
    int unit)
{
    return (SOC_UNIT_VALID(unit)
            && SOC_IS_DONE_INIT(unit)
            && !SOC_IS_DETACHING(unit)
            && (dnx_err_recovery_manager[unit].is_initialized)
            && !dnx_err_recovery_is_tmp_suspended(unit)
            && (!dnxc_rollback_journal_is_any_journal_rolling_back(unit))
            && (!dnx_err_recovery_utils_is_excluded_thread(unit))
            && dnx_err_recovery_manager[unit].is_appl_init_done
            && dnx_err_recovery_manager[unit].is_testing_enabled
            && (1 == dnx_err_recovery_manager[unit].api_counter) && (!dnx_err_recovery_manager[unit].is_api_excluded));
}

/*
 * check if this is the first transaction in a top level BCM API
 */
static uint8
dnx_err_recovery_is_first_transaction_in_top_level_api(
    int unit)
{
    /*
     * the api_to_trans_map marks which transaction number was at the beginning of the bcm API.
     * first transaction started in the current bcm API is + 1
     * top level bcm apis have api counter = 1
     */
    return ((1 == dnx_err_recovery_manager[unit].api_counter)
            && ((dnx_err_recovery_manager[unit].api_to_trans_map[1] + 1) ==
                dnx_err_recovery_manager[unit].transaction_counter));
}

/*
 * \brief - returns true if this is the first test iteration of the BCM API that has opted in for error recovery
 */
static uint8
dnx_err_recovery_regression_is_first_test_iteration(
    int unit)
{
    return (dnx_err_recovery_regression_testing_conditions_are_met(unit)
            && dnx_err_recovery_is_first_transaction_in_top_level_api(unit)
            && (!dnx_err_recovery_manager[unit].is_api_ran));
}

/*
 * \brief - returns true if this is the second test iteration of the BCM API that has opted in for error recovery
 */
static uint8
dnx_err_recovery_regression_is_second_test_iteration(
    int unit)
{
    return (dnx_err_recovery_regression_testing_conditions_are_met(unit)
            && dnx_err_recovery_is_first_transaction_in_top_level_api(unit)
            && (dnx_err_recovery_manager[unit].is_api_ran));
}

/*
 * \brief - Check for swstate and hw access violations outside of err recovery transaction in top level (non-nested) BCM APIs.
 *  Utilized by journal functions called during swstate and hw access.
 */
static void
dnx_err_recovery_regression_system_state_modified(
    int unit)
{
    /*
     * Violation check should be performed only for regression testing and the bypass flag has not been raised.
     * Mark swstate or hw access in a non-nested BCM API outside of transactions.
     * - Not a violation in cases where no transactions in BCM API.
     * - Not a violation in cases where BCM API is in a "test cleanup" transaction.
     */
    if (dnx_err_recovery_regression_testing_conditions_are_met(unit)
        && (0 == dnx_err_recovery_manager[unit].transaction_counter))

    {
        dnx_err_recovery_manager[unit].has_access_outside_trans = TRUE;

        /*
         * violation if at least one transaction was already started / ended in the current BCM API
         */
        if (0 != dnx_err_recovery_manager[unit].non_nested_trans_in_api_count)
        {
            LOG_CLI((BSL_META_U
                     (unit, "error recovery regression ERROR: hw or swstate access outside of transaction\n")));
            assert(0);
        }
    }
}

/*
 * see .h file for description
 */
void
dnx_err_recovery_regression_testing_api_name_update(
    int unit,
    const char *func_name)
{
    if (dnx_err_recovery_regression_testing_conditions_are_met(unit))
    {
        sal_strncpy(dnx_err_recovery_manager[unit].dnx_err_recovery_regression_testing_api_name, func_name,
                    DNXC_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH - 1);
    }
}

/*
 * see .h file for description
 */
int
dnx_err_recovery_regression_test_should_loop(
    int unit)
{
    return (dnx_err_recovery_regression_testing_conditions_are_met(unit)
            && (1 == dnx_err_recovery_manager[unit].non_nested_trans_in_api_count)
            && (!dnx_err_recovery_manager[unit].is_api_ran));
}

/*
 * see .h file for description
 */
int
dnx_err_recovery_regression_test_counter_inc(
    int unit)
{
    /*
     * in case of sibling trnasaction violation we don't want to run the DNX API twice
     */
    if (dnx_err_recovery_regression_test_should_loop(unit))
    {
        /*
         * mark that API has been ran and prevent sibling transactions,
         * since the DNX API will be run one more time.
         */
        dnx_err_recovery_manager[unit].is_api_ran = TRUE;
        dnx_err_recovery_manager[unit].non_nested_trans_in_api_count = 0;

        return TRUE;
    }

    return FALSE;
}

/*
 * see .h file for description
 */
void
dnx_err_recovery_regression_testing_api_test_mode_change(
    int unit,
    uint8 is_on)
{
    dnx_err_recovery_manager[unit].is_testing_enabled = is_on ? TRUE : FALSE;
}

/*
 * see .h file for description
 */
void
dnx_err_recovery_regression_testing_api_exclude(
    int unit,
    dnx_err_recovery_api_no_test_reason_t reason)
{
    /*
     * exclusion flag is reset at the end of the top level BCM API
     */
    if (dnx_err_recovery_regression_testing_conditions_are_met(unit))
    {
        dnx_err_recovery_manager[unit].is_api_excluded = TRUE;
    }
}

shr_error_e
dnx_err_recovery_regression_journal_var(
    int unit,
    uint32 size,
    uint8 *ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_regression_is_first_test_iteration(unit))
    {
        SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_memcpy(unit, size, ptr));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

#undef _ERR_MSG_MODULE_NAME
