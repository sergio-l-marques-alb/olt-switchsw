/** \file dnx_state_snapshot_manager.c
 * This module is the System State snapshot manager.
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
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
#include <soc/dnx/recovery/generic_state_journal.h>

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
 * \brief - holds on/off state for the snapshot manager
 */
dnx_state_snapshot_manager_t dnx_state_snapshot_manager[SOC_MAX_NUM_DEVICES] = { {0}
};

/**
 * /brief - cb get sequence id for new journal entry
 */
static uint32
dnx_state_snapshot_manager_entry_seq_id_get(
    int unit,
    int er_thread_id)
{
    dnx_state_snapshot_manager[unit].entry_counter[er_thread_id]++;
    return dnx_state_snapshot_manager[unit].entry_counter[er_thread_id];
}

/**
 * \brief - check if snapshot manager is on, basic preconditions
 */
static uint8
dnx_state_snapshot_manager_is_on(
    int unit,
    int er_thread_id)
{
    return dnx_err_recovery_common_is_on(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON);
}

/**
 * \brief - check if snapshot swstate journal is on
 */
static uint8
dnx_state_snapshot_sw_state_journal_is_on(
    int unit,
    int er_thread_id)
{
    /*
     * journal only if we are currently not in dbal region access
     */
    return (dnx_state_snapshot_manager_is_on(unit, er_thread_id)
            && !dnx_err_recovery_is_dbal_access_region(unit, er_thread_id));
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
    dnx_rollback_journal_cbs_t state_cbs;
    int idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_state_snapshot_manager[unit].is_on_counter[idx] = 0;
        dnx_err_recovery_common_flag_clear(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

        dnx_state_snapshot_manager[unit].entry_counter[idx] = 0;
        dnx_state_snapshot_manager[unit].is_suppressed_counter[idx] = 0;
        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);
    }
    /*
     * Callbacks are kept and utilized by the lower level journals in order to avoid hierarchy violations.
     */
    swstate_cbs.is_on = &dnx_state_snapshot_sw_state_journal_is_on;
    swstate_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_initialize(unit, swstate_cbs, NULL, TRUE));

    dbal_cbs.is_on = &dnx_state_snapshot_manager_is_on;
    dbal_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_dbal_journal_initialize(unit, dbal_cbs, NULL, TRUE));

    state_cbs.is_on = &dnx_state_snapshot_manager_is_on;
    state_cbs.seq_id_get = &dnx_state_snapshot_manager_entry_seq_id_get;
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_initialize(unit, state_cbs, TRUE));

    dnx_state_snapshot_manager[unit].is_initialized = TRUE;

    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);
    }
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
    int idx = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * do nothing if snapshot manager hasn't been initialized.
     */
    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_state_snapshot_manager[unit].is_on_counter[idx] = 0;
        dnx_err_recovery_common_flag_clear(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

        dnx_state_snapshot_manager[unit].is_suppressed_counter[idx] = 0;
        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);

        dnx_err_recovery_common_flag_set(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                         DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED);
    }

    /*
     * destroy the sw state journal and the dbal journal
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_destroy(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_destroy(unit, TRUE));

    dnx_state_snapshot_manager[unit].is_initialized = FALSE;
    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_err_recovery_common_flag_clear(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);

        dnx_state_snapshot_manager[unit].entry_counter[idx] = 0;
    }
exit:
    
    if (dnx_state_snapshot_manager[unit].is_initialized)
    {
        dnx_state_snapshot_manager[unit].is_initialized = FALSE;
        for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
        {
            dnx_err_recovery_common_flag_clear(unit, idx, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                               DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED);
        }
    }
    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        if (dnx_state_snapshot_manager[unit].entry_counter[idx] != 0)
        {
            dnx_state_snapshot_manager[unit].entry_counter[idx] = 0;
        }
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
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

    /*
     * in case this thread does not have an ER index, go and get one and mark the transaction
     * as top_level (first transaction in a reentrant transaction stack for this thread)
     */

    SHR_IF_ERR_EXIT(dnx_er_threading_transaction_register(unit));
    er_thread_id = dnx_er_threading_transaction_get(unit);
    assert(er_thread_id != -1);

    dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id]++;
    dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                     DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

#ifdef BCM_WARM_BOOT_SUPPORT
    /*
     * Mark the region as Non-warmbootable is we are in comparison mode.
     */
    if (dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id] == 1)
    {
        dnxc_wb_no_wb_test_set(unit, 1);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

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
    uint8 should_clean = TRUE;
    uint8 differences_exist = FALSE;
    shr_error_e rollback_rv = _SHR_E_NONE;
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    /*
     * sequence id of the head of each journal 
     */
    uint32 state_entry_seq_id = 0;
    uint32 sw_state_entry_seq_id = 0;
    uint32 dbal_entry_seq_id = 0;
    /*
     * internal iterator, used to traverse the journal entries in sequential order 
     */
    uint32 seq_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * must exist by the time we get to comparison_end
     */
    assert(er_thread_id != -1);

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (0 == dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id])
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "snapshot manager ERROR: end called without manager being started .\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * disable journaling
     */
    dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id]--;

    if (0 == dnx_state_snapshot_manager[unit].is_on_counter[er_thread_id])
    {

#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (0 < dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id])
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "snapshot manager ERROR: end called while temporary suppressions are on.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif

        dnx_err_recovery_common_flag_clear(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION);

        /*
         *Iterate over the comparison entries in sequential order
         */
        while (seq_id <= dnx_state_snapshot_manager[unit].entry_counter[er_thread_id])
        {
            /*
             *get the sequence id of the head entry of each journal
             */
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id(unit, er_thread_id, TRUE, &sw_state_entry_seq_id));
            SHR_IF_ERR_EXIT(dnx_generic_state_journal_get_head_seq_id(unit, er_thread_id, TRUE, &state_entry_seq_id));
            SHR_IF_ERR_EXIT(dnx_dbal_journal_get_head_seq_id(unit, er_thread_id, TRUE, &dbal_entry_seq_id));

            /*
             *compare the journal entries in sequential order
             */
            if (sw_state_entry_seq_id == seq_id)
            {
                rollback_rv = dnxc_sw_state_journal_roll_back(unit, er_thread_id, TRUE, TRUE);
                if (_SHR_E_NONE != rollback_rv)
                {
                    differences_exist = TRUE;
                }
            }
            else if (state_entry_seq_id == seq_id)
            {
                rollback_rv = dnx_generic_state_journal_roll_back(unit, er_thread_id, TRUE, TRUE);
                if (_SHR_E_NONE != rollback_rv)
                {
                    differences_exist = TRUE;
                }
            }
            else
            {
                rollback_rv = dnx_dbal_journal_roll_back(unit, er_thread_id, TRUE, TRUE);
                if (_SHR_E_NONE != rollback_rv)
                {
                    differences_exist = TRUE;
                }

            }
            seq_id++;
        }
        /*
         *reset the entry counter after all entries have been traversed
         */
        dnx_state_snapshot_manager[unit].entry_counter[er_thread_id] = 0;

        /*
         * remove all exclusions for dbal and swstate at the end of the transaction
         */
        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_clear(unit, er_thread_id, TRUE));
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_exclude_clear(unit, er_thread_id, TRUE));

#ifdef BCM_WARM_BOOT_SUPPORT
        /*
         * Resore the warmboot test mode to enable after the journal end.
         */
        dnxc_wb_no_wb_test_set(unit, 0);
#endif /* BCM_WARM_BOOT_SUPPORT */

    }

    should_clean = FALSE;

    if (differences_exist)
    {
        SHR_IF_ERR_EXIT_NO_MSG(SOC_E_INTERNAL);
    }
exit:
    if (should_clean)
    {
        dnxc_sw_state_journal_clear(unit, er_thread_id, FALSE, TRUE);
        dnx_generic_state_journal_clear(unit, er_thread_id, FALSE, TRUE);
        dnx_dbal_journal_clear(unit, er_thread_id, FALSE, TRUE);

        dnx_dbal_journal_exclude_table_clear(unit, er_thread_id, TRUE);
        dnxc_sw_state_journal_exclude_clear(unit, er_thread_id, TRUE);
    }
    SHR_IF_ERR_EXIT(dnx_er_threading_transaction_unregister(unit));
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
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Relevant only if thread is ER active
     */
    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    DNX_STATE_SNAPSHOT_MANAGER_EXIT_IF_NOT_INITIALIZED(unit);

    if (is_suppressed)
    {
        dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id]++;

        dnx_err_recovery_common_flag_clear(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                           DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
    }
    else
    {
        if (0 == dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id])
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "snapshot manager ERROR: attempting to unsuppress without any suppressions done\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);

        }

        dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id]--;

        if (0 == dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id])
        {
            dnx_err_recovery_common_flag_set(unit, er_thread_id, DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                             DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
uint8
dnx_state_comparison_is_suppressed(
    int unit,
    int er_thread_id)
{
    if (dnx_state_snapshot_manager[unit].is_suppressed_counter[er_thread_id] != 0)
    {
        return TRUE;
    }
    return FALSE;
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
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * it is assumed exclude_by_stamp is not called when journal is off 
     */
    assert(er_thread_id != -1);

    if (is_swstate)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_exclude_by_stamp(unit, er_thread_id, stamp, TRUE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_by_stamp(unit, er_thread_id, stamp, TRUE));
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
    SHR_FUNC_INIT_VARS(unit);
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_end_and_compare(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_suppress(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
int
dnx_tcl_state_comparison_unsuppress(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
/* 
 * we skip the comparison journaling for tcl tests in wb test mode as the journal will not survive
 * reboot.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
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
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_WARM_BOOT_SUPPORT
    {
        int wb_enable;
        dnxc_wb_test_mode_get(unit, &wb_enable);
        if (wb_enable)
        {
            return 0;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 1));
    SHR_IF_ERR_EXIT(dnx_state_comparison_exclude_by_stamp(unit, stamp, is_swstate));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_is_init_check_bypass(unit, 0));
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
