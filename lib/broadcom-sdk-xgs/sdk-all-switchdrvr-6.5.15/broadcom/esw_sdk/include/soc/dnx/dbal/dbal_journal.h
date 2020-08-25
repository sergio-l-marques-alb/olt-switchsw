/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by dbal access calls since the beginning of the last transaction.
 */
#ifndef _DNX_DBAL_JOURNAL_H
/* { */

#define _DNX_DBAL_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/dnxc_rollback_journal.h>

/**
 * \brief - maximum number of excluded tables per transaction
 */
#define DNX_DBAL_JOURNAL_EXCLUDED_TABLES_MAX_NOF 10

#define DNX_DBAL_JOURNAL_ROLLBACK_FLAG                     SAL_BIT(0)
#define DNX_DBAL_JOURNAL_COMPARE_FLAG                      SAL_BIT(1)

/*
 * describe error recovery journal flags
 */
typedef enum
{
    DNX_DBAL_JOURNAL_ROLLBACK = 0,
    DNX_DBAL_JOURNAL_COMPARISON,
    DNX_DBAL_JOURNAL_ALL
} dnx_err_recovery_journal_e;

/**
 * \brief
 *  Callback each time dbal journal is accessed.
 * \param [in] unit - Device Id
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef void (
    *dnx_dbal_journal_access_cb) (
    int unit);

/**
 * \brief - dbal journal entry structure
 */
typedef struct dnx_dbal_journal_entry_d
{
    dbal_actions_e action;
} dnx_dbal_journal_entry_t;

/**
 * \brief - structures used for holding state for the current dbal journal transaction
 */
typedef struct dnx_dbal_journal_transaction_d
{
    /*
     * indicates if entry has been inserted in current dbal access region
     * flag should be cleared at the beginning and end of every access region
     */
    uint8 dbal_access_region_is_entry_inserted;
    uint32 excluded_tables_count;
    dbal_tables_e excluded_tables[DNX_DBAL_JOURNAL_EXCLUDED_TABLES_MAX_NOF];
} dnx_dbal_journal_transaction_t;

/**
 * \brief - describes dbal rollback and comparison journals.
 */
typedef struct dnx_dbal_journal_d
{
    dnxc_rollback_journal_handle_t handle;
    dnx_dbal_journal_transaction_t transaction;
    dnx_dbal_journal_access_cb access_cb;
} dnx_dbal_journal_t;

/**
 * \brief - dbal journal internal state
 */
typedef struct dnx_dbal_journal_manager_d
{
    uint32 unsupported_table_bypass_counter;
    dnx_dbal_journal_t journals[2];
} dnx_dbal_journal_manager_t;

/**
 * \brief
 *  Initialize dbal journal
 * \param [in] unit - Device Id
 * \param [in] rollback_journal_cbs  - Generic journaling callbacks.
 * \param [in] access_cb - Callback dbal journal is being accessed
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_initialize(
    int unit,
    dnxc_rollback_journal_cbs_t rollback_journal_cbs,
    dnx_dbal_journal_access_cb access_cb,
    uint8 is_comparison);

/**
 * \brief
 *  Indicate if the dbal journal is currently on
 * \param [in] unit - Device Id
 * \param [in] journals - Specify if rollback, comparison and all journals should be checked if on.
 *  Should be one of:
 *  - DNX_DBAL_JOURNAL_ALL
 *  - DNX_DBAL_JOURNAL_ROLLBACK
 *  - DNX_DBAL_JOURNAL_COMPARISON
 * \return
 *   int - Error Type
* \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_dbal_journal_is_on(
    int unit,
    dnx_err_recovery_journal_e journals);

/**
 * \brief
 *  Destroy the dbal journal
 * \param [in] unit  - Device Id 
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_destroy(
    int unit,
    uint8 is_comparison);

/**
 * \brief
 *  The function will roll-back the current dbal journal transaction
 * \param [in] unit - Device Id
 * \param [in] only_head_rollback - Indicate if only the head of the journal should be rollbacked
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_roll_back(
    int unit,
    uint8 only_head_rollback,
    uint8 is_comparison);

/**
 * \brief
 *  The function will clear and deactivate the dbal journal
 * \param [in] unit - Device Id
 * \param [in] only_head_clear - clear only the top level entry of the dbal
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_clear(
    int unit,
    uint8 only_head_clear,
    uint8 is_comparison);

/**
 * \brief
 *  Log an ADD operation in the dbal journal
 * \param [in] unit - Device Id
 * \param [in] get_handle - Dbal journal get entry handle
 * \param [in] entry_handle - Dbal journal entry handle
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_log_add(
    int unit,
    dbal_entry_handle_t * get_handle,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief
 *  Log an CLEAR operation in the dbal journal
 * \param [in] unit - Device Id
 * \param [in] entry_handle - Dbal journal entry handle
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_log_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief
 *  Get sequence id of the head entry of the dbal journal
 * \param [in] unit   - Device Id
 * \param [in] is_comparison   - Sequence id to be returned
 * \param [out] seq_id   - Sequence id to be returned
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_get_head_seq_id(
    int unit,
    uint8 is_comparison,
    uint32 *seq_id);

/**
 * \brief
 *  Check if error recovery is supported for dbal table
 * \param [in] unit   - Device Id
 * \param [in] table_id   - Table id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_dbal_journal_is_unsupported_table(
    int unit,
    uint32 table_id);

/**
 * \brief
 *  Indicate the begin or end of unsupported dbal table check bypass.
 * \param [in] unit   - Unit id
 * \param [in] is_bypassed   - Indicate being or end of bypass region
 * \return
 *   int - Error Type
 * \remark
 *   * Used in cases of providing error recovery support for APIs which contain unsupported tables. Suppresses error messages on access.
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_unsupported_tables_bypass(
    int unit,
    uint8 is_bypassed);

/**
 * \brief
 *  Check if unsupported dbal tables are bypassed.
 * \param [in] unit   - Unit Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_dbal_journal_are_unsupported_tables_bypassed(
    int unit);

/**
 * \brief
 *  Exclude a table from comparison journal
 * \param [in] unit   - Unit Id
 * \param [in] stamp - dbal table stamp string
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_exclude_table_by_stamp(
    int unit,
    char *stamp,
    uint8 is_comparison);

/**
 * \brief
 *  Clear the set of excluded dbal tables
 * \param [in] unit   - Unit Id
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_exclude_table_clear(
    int unit,
    uint8 is_comparison);

/**
 * \brief
 *  Invalidates the last entry in the dbal journal.
 *  Assumes that dbal journal is on.
 * \param [in] unit - Unit Id
 * \param [in] access_type - Type of action access that will be performed
 * \param [in] access_func - action access function type of the caller
 * \param [in] error - Indicates if any errors were encountered in the dbal journal access region function
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_invalidate_last_on_err(
    int unit,
    dbal_action_access_type_e access_type,
    dbal_action_access_func_e access_func,
    int error);

/**
 * \brief
 *  Print rollback or comparison dbal journal
 * \param [in] unit   - Unit Id
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dbal_journal_print(
    int unit,
    uint8 is_comparison);

/* } */
#endif /* _DNX_DBAL_JOURNAL_H */
