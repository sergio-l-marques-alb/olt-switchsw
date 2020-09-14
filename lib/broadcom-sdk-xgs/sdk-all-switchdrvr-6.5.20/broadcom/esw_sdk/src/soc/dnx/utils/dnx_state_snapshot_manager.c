/** \file dnx_state_snapshot_manager.c
 * This module is the System State snapshot manager.
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

#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

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
    if (!dnx_er_threading_is_main_thread(unit)) {                  \
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
uint8
dnx_state_snapshot_manager_is_on(
    int unit)
{
    return dnx_err_recovery_common_is_on(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON)
        && dnx_er_threading_is_main_thread(unit);
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
    return (dnx_state_snapshot_manager_is_on(unit) && !dnx_err_recovery_common_is_dbal_access_region(unit));
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_snapshot_manager_init(
    int unit)
{
    dnx_rollback_journal_cbs_t swstate_cbs;
    dnx_rollback_journal_cbs_t dbal_cbs;

    SHR_FUNC_INIT_VARS(unit);

    dnx_state_snapshot_manager[unit].is_on_counter = 0;
    dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_state_snapshot_manager[unit].entry_counter = 0;
    dnx_state_snapshot_manager[unit].is_suppressed_counter = 0;
    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);

    /*
     * Callbacks are kept and utilized by the lower level journals in order to avoid hierarchy violations.
     */
    swstate_cbs.is_on = &dnx_state_snapshot_sw_state_journal_is_on;
    swstate_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_initialize(unit, swstate_cbs, NULL, TRUE));

    dbal_cbs.is_on = &dnx_state_snapshot_manager_is_on;
    dbal_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dbal_cbs, NULL, TRUE));

    dnx_state_snapshot_manager[unit].is_initialized = TRUE;
    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);
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
    dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

    dnx_state_snapshot_manager[unit].is_suppressed_counter = 0;
    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);

    /*
     * destroy the sw state journal and the dbal journal
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_destroy(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy(unit, TRUE));

    dnx_state_snapshot_manager[unit].is_initialized = FALSE;
    dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                       DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

    dnx_state_snapshot_manager[unit].entry_counter = 0;
exit:
    if (dnx_state_snapshot_manager[unit].is_initialized)
    {
        dnx_state_snapshot_manager[unit].is_initialized = FALSE;
        dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);
    }
    if (dnx_state_snapshot_manager[unit].entry_counter != 0)
    {
        dnx_state_snapshot_manager[unit].entry_counter = 0;
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
    dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

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
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "snapshot manager ERROR: end called without manager being started .\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * disable journaling
     */
    dnx_state_snapshot_manager[unit].is_on_counter--;

    if (0 == dnx_state_snapshot_manager[unit].is_on_counter)
    {

#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (0 < dnx_state_snapshot_manager[unit].is_suppressed_counter)
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "snapshot manager ERROR: end called while temporary suppressions are on.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif

        dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);
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

        dnx_err_recovery_common_flag_clear(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
    }
    else
    {
        if (0 == dnx_state_snapshot_manager[unit].is_suppressed_counter)
        {
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "snapshot manager ERROR: attempting to unsuppress without any suppressions done\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);

        }

        dnx_state_snapshot_manager[unit].is_suppressed_counter--;

        if (0 == dnx_state_snapshot_manager[unit].is_suppressed_counter)
        {
            dnx_err_recovery_common_flag_set(unit, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                             DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
        }

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
dnx_state_comparison_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_print(unit, TRUE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_print(unit, TRUE));
            break;
        default:
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "snapshot manager ERROR:: unrecognized journal type.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnx_state_comparison_journal_logger_state_change(
    int unit,
    dnx_rollback_journal_subtype_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_logger_state_change(unit, TRUE));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            SHR_IF_ERR_EXIT(dnx_dbal_journal_logger_state_change(unit, TRUE));
            break;
        default:
            DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "snapshot manager ERROR:: unrecognized journal type.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
    }

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
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_API_TEST
    int wb_enable;
    dnxc_wb_test_mode_get(unit, &wb_enable);
    if (wb_enable)
    {
        return 0;
    }
#endif /* BCM_WARM_BOOT_API_TEST */

    return dnx_state_comparison_start(unit);
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_end_and_compare(
    int unit)
{
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_API_TEST
    int wb_enable;
    dnxc_wb_test_mode_get(unit, &wb_enable);
    if (wb_enable)
    {
        return 0;
    }
#endif /* BCM_WARM_BOOT_API_TEST */

    return dnx_state_comparison_end_and_compare(unit);
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_suppress(
    int unit)
{
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_API_TEST
    int wb_enable;
    dnxc_wb_test_mode_get(unit, &wb_enable);
    if (wb_enable)
    {
        return 0;
    }
#endif /* BCM_WARM_BOOT_API_TEST */

    return dnx_state_comparison_suppress(unit, TRUE);
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_unsuppress(
    int unit)
{
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_API_TEST
    int wb_enable;
    dnxc_wb_test_mode_get(unit, &wb_enable);
    if (wb_enable)
    {
        return 0;
    }
#endif /* BCM_WARM_BOOT_API_TEST */

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
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_API_TEST
    int wb_enable;
    dnxc_wb_test_mode_get(unit, &wb_enable);
    if (wb_enable)
    {
        return 0;
    }
#endif /* BCM_WARM_BOOT_API_TEST */

    return dnx_state_comparison_exclude_by_stamp(unit, stamp, is_swstate);
}

#undef _ERR_MSG_MODULE_NAME
