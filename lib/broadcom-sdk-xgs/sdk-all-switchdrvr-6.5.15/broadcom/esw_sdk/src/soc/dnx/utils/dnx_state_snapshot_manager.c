/** \file dnx_state_snapshot_manager.c
 * This module is the System State snapshot manager.
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

#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/dnxc_rollback_journal.h>
#include <soc/dnxc/dnxc_rollback_journal_utils.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_err_recovery_manager_utils.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * \brief - exit if the feature is not initialized
 */
#define DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit)  \
do{                                                               \
    if (TRUE != dnx_state_snapshot_manager[unit].is_initialized) {\
        SHR_EXIT();                                               \
    }                                                             \
}while(0)

/*
 * \brief - exit if current thread is not in the set of allowed journaling threads.
 */
#define DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_THREAD_NOT_ALLOWED(unit)\
do{                                                                \
    if (dnx_err_recovery_utils_is_excluded_thread(unit)) {         \
        SHR_EXIT();                                                \
    }                                                              \
}while(0)

/*
 * \brief - holds on/off state for the snapshot manager
 */
dnx_state_snapshot_manager_t dnx_state_snapshot_manager[SOC_MAX_NUM_DEVICES] = { {0}
};

/**
 * /brief - cb get sequence id for new journal entry
 */
static uint32
dnx_state_snapshot_manager_entry_seq_id_get(
    int unit)
{
    dnx_state_snapshot_manager[unit].entry_counter++;
    return dnx_state_snapshot_manager[unit].entry_counter;
}

/**
 * \brief - check if snapshot manager is on, basic preconditions
 */
static uint8
dnx_state_snapshot_manager_is_on_basic(
    int unit)
{
    /*
     * Snapshot manager 'is on' basic conditions:
     * - We are not in soc init / deinit
     * - Feature is initialized and on
     * - No journal is currently rolling back
     * - The current thread is in the set of allowed journaling threads
     */
    return (SOC_IS_DONE_INIT(unit)
            && !SOC_IS_DETACHING(unit)
            && (dnx_state_snapshot_manager[unit].is_initialized)
            && (dnx_state_snapshot_manager[unit].is_on_counter > 0)
            && (!dnxc_rollback_journal_is_any_journal_rolling_back(unit))
            && (!dnx_err_recovery_utils_is_excluded_thread(unit)));
}

/**
 * see .h file for description
 */
uint8
dnx_state_snapshot_manager_is_on(
    int unit)
{
    /*
     * Snapshot manager is considered to be on only if:
     * - Basic conditions are met
     * - There are no active suppressions
     */
    return (dnx_state_snapshot_manager_is_on_basic(unit)
            && (dnx_state_snapshot_manager[unit].is_suppressed_counter == 0));
}

/**
 * \brief - check if snapshot swstate journal is on
 */
static uint8
dnx_state_snapshot_sw_state_journal_is_on(
    int unit)
{
    /*
     * journal only if we are currently not in dbal region access
     */
    return (dnx_state_snapshot_manager_is_on(unit)
            && (0 == dnx_state_snapshot_manager[unit].dbal_access_region_counter));
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_snapshot_manager_init(
    int unit)
{
    dnxc_rollback_journal_cbs_t swstate_cbs;
    dnxc_rollback_journal_cbs_t dbal_cbs;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * if error recovery is disabled at compile-time, don't initialize
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        SHR_EXIT();
    }

    dnx_state_snapshot_manager[unit].is_on_counter = 0;
    dnx_state_snapshot_manager[unit].entry_counter = 0;
    dnx_state_snapshot_manager[unit].is_suppressed_counter = 0;
    dnx_state_snapshot_manager[unit].dbal_access_region_counter = 0;

    swstate_cbs.is_on = &dnx_state_snapshot_sw_state_journal_is_on;
    swstate_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_initialize(unit, swstate_cbs, NULL, TRUE));

    dbal_cbs.is_on = &dnx_state_snapshot_manager_is_on;
    dbal_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dbal_cbs, NULL, TRUE));

    dnx_state_snapshot_manager[unit].is_initialized = TRUE;
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_snapshot_manager_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * do nothing if snapshot manager hasn't been initialized.
     */
    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

    dnx_state_snapshot_manager[unit].is_on_counter = 0;
    dnx_state_snapshot_manager[unit].is_suppressed_counter = 0;

    /*
     * destroy the sw state journal and the dbal journal
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_destroy(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy(unit, TRUE));

    dnx_state_snapshot_manager[unit].is_initialized = FALSE;
    dnx_state_snapshot_manager[unit].entry_counter = 0;
    dnx_state_snapshot_manager[unit].dbal_access_region_counter = 0;
exit:
    if (dnx_state_snapshot_manager[unit].is_initialized)
    {
        dnx_state_snapshot_manager[unit].is_initialized = FALSE;
    }
    if (dnx_state_snapshot_manager[unit].entry_counter != 0)
    {
        dnx_state_snapshot_manager[unit].entry_counter = 0;
    }
    if (dnx_state_snapshot_manager[unit].dbal_access_region_counter != 0)
    {
        dnx_state_snapshot_manager[unit].dbal_access_region_counter = 0;
    }
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_comparison_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);
    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    dnx_state_snapshot_manager[unit].is_on_counter++;

#ifdef BCM_WARM_BOOT_API_TEST
    /*
     * Mark the region as Non-warmbootable is we are in comparison mode.
     */
    if (dnx_state_snapshot_manager[unit].is_on_counter == 1)
    {
        dnxc_wb_no_wb_test_set(unit, 1);
    }
#endif /* BCM_WARM_BOOT_API_TEST */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_comparison_end_and_compare(
    int unit)
{
    uint32 seq_id = 0;

    uint8 should_clean = FALSE;
    uint8 differences_exist = FALSE;

    shr_error_e rollback_rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);
    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    should_clean = TRUE;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (0 == dnx_state_snapshot_manager[unit].is_on_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "snapshot manager ERROR: end called without manager being started .\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    if (0 < dnx_state_snapshot_manager[unit].is_suppressed_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "snapshot manager ERROR: end called while temporary suppressions are on.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * disable journaling
     */
    dnx_state_snapshot_manager[unit].is_on_counter--;

    if (0 == dnx_state_snapshot_manager[unit].is_on_counter)
    {
        /*
         * check all differences in the swstate snapshot journal.
         * rollback all valid entries in the swstate journal.
         */
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, TRUE, &seq_id));

        while (0 != seq_id)
        {
            rollback_rv = dnxc_sw_state_journal_roll_back(unit, TRUE, TRUE);
            if (_SHR_E_NONE != rollback_rv)
            {
                differences_exist = TRUE;
            }

            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, TRUE, &seq_id));
        }

        /*
         * check all differences in the dbal snapshot journal
         * rollback all valid entries in the dbal journal.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, TRUE, &seq_id));

        while (0 != seq_id)
        {
            rollback_rv = dnx_dbal_journal_roll_back(unit, TRUE, TRUE);
            if (_SHR_E_NONE != rollback_rv)
            {
                differences_exist = TRUE;
            }

            SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, TRUE, &seq_id));
        }

        dnx_state_snapshot_manager[unit].entry_counter = 0;

        /*
         * remove all exclusions for dbal and swstate at the end of the transaction
         */
        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_clear(unit, TRUE));
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_exclude_clear(unit, TRUE));

#ifdef BCM_WARM_BOOT_API_TEST
        /*
         * Resore the warmboot test mode to enable after the journal end.
         */
        dnxc_wb_no_wb_test_set(unit, 0);
#endif /* BCM_WARM_BOOT_API_TEST */
    }

    should_clean = FALSE;

    if (differences_exist)
    {
        SHR_IF_ERR_EXIT_NO_MSG(SOC_E_INTERNAL);
    }
exit:
    if (should_clean)
    {
        dnxc_sw_state_journal_clear(unit, FALSE, TRUE);
        dnx_dbal_journal_clear(unit, FALSE, TRUE);

        dnx_dbal_journal_exclude_table_clear(unit, TRUE);
        dnxc_sw_state_journal_exclude_clear(unit, TRUE);
    }
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_comparison_dbal_access_region_inc(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_state_snapshot_manager[unit].dbal_access_region_counter++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_comparison_dbal_access_region_dec(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (0 == dnx_state_snapshot_manager[unit].dbal_access_region_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "snapshot manager ERROR: attempted to end dbal access region without start.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    dnx_state_snapshot_manager[unit].dbal_access_region_counter--;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_comparison_suppress(
    int unit,
    uint8 is_suppressed)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);
    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_THREAD_NOT_ALLOWED(unit);

    if (is_suppressed)
    {
        dnx_state_snapshot_manager[unit].is_suppressed_counter++;
    }
    else
    {
        if (0 == dnx_state_snapshot_manager[unit].is_suppressed_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "snapshot manager ERROR: attempting to unsuppress without any suppressions done\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);

        }

        dnx_state_snapshot_manager[unit].is_suppressed_counter--;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_comparison_exclude_by_stamp(
    int unit,
    char *stamp,
    int is_swstate)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_swstate)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_exclude_by_stamp(unit, stamp, TRUE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_by_stamp(unit, stamp, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_comparison_print_journals(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_print(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_print(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_start(
    int unit)
{
    return dnx_state_comparison_start(unit);
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_end_and_compare(
    int unit)
{
    return dnx_state_comparison_end_and_compare(unit);
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_suppress(
    int unit)
{
    return dnx_state_comparison_suppress(unit, TRUE);
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_unsuppress(
    int unit)
{
    return dnx_state_comparison_suppress(unit, FALSE);
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_exclude_by_stamp(
    int unit,
    char *stamp,
    int is_swstate)
{
    return dnx_state_comparison_exclude_by_stamp(unit, stamp, is_swstate);
}

#undef _ERR_MSG_MODULE_NAME
