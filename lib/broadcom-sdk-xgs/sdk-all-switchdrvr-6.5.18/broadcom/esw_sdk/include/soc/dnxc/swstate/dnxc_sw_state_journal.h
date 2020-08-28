/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by sw_state access calls since the beginning of the last transaction.
 */

#ifndef _DNX_SW_STATE_JOURNAL_H
/* { */
#define _DNX_SW_STATE_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/dnxc/dnxc_rollback_journal.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
#define DNXC_SW_STATE_JOURNAL_EXCLUDED_STAMPS_MAX_NOF 10

/**
 * \brief
 *  Callback each time swstate journal is accessed.
 * \param [in] unit - Device Id
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef void (
    *dnxc_sw_state_journal_access_cb) (
    int unit);

/**
 * \brief - structures used for holding state for the current swstate journal transaction
 */
typedef struct dnxc_sw_state_journal_transaction_d
{
    uint32 excluded_stamps_count;
    char *excluded_stamps[DNXC_SW_STATE_JOURNAL_EXCLUDED_STAMPS_MAX_NOF];
} dnxc_sw_state_journal_transaction_t;

/**
 * \brief - describes swstate rollback and comparison journals.
 */
typedef struct dnxc_sw_state_journal_d
{
    dnxc_rollback_journal_handle_t handle;
    dnxc_sw_state_journal_transaction_t transaction;
    dnxc_sw_state_journal_access_cb access_cb;
} dnxc_sw_state_journal_t;

/**
 * \brief - swstate journal internal state
 */
typedef struct dnxc_sw_state_journal_manager_d
{
    int dnxc_sw_state_journal_dummy;
    dnxc_sw_state_journal_t journals[2];
} dnxc_sw_state_journal_manager_t;
/**
 * \brief - describes differene sw state journal entry trypes
 */
typedef enum dnxc_sw_state_journal_entry_e
{
    SW_JOURNAL_ENTRY_MEMCPY = 0,
    SW_JOURNAL_ENTRY_ALLOC,
    SW_JOURNAL_ENTRY_FREE,
    SW_JOURNAL_ENTRY_MUTEX_TAKE,
    SW_JOURNAL_ENTRY_MUTEX_GIVE,
    SW_JOURNAL_ENTRY_HTBL_ENTRY,
    SW_JOURNAL_ENTRY_TYPE_COUNT
} dnxc_sw_state_journal_entry_type_t;

/**
 * \brief - sw state journal 'memcpy' entry structure
 */
typedef struct dnxc_sw_state_journal_memcpy_entry_d
{
    uint8 *ptr;
} dnxc_sw_state_journal_memcpy_entry_t;

/**
 * \brief - sw state journal 'alloc' entry structure
 */
typedef struct dnxc_sw_state_journal_alloc_entry_d
{
    uint8 *location;
    uint8 **ptr_location;
} dnxc_sw_state_journal_alloc_entry_t;

/**
 * \brief - sw state journal 'free' entry structure
 */
typedef struct dnxc_sw_state_journal_free_entry_d
{
    uint8 *location;
    uint8 **ptr_location;
} dnxc_sw_state_journal_free_entry_t;

/**
 * \brief - sw state journal 'mutex_take' entry structure
 */
typedef struct dnxc_sw_state_journal_mutex_take_entry_d
{
    sw_state_mutex_t *ptr_mtx;
    uint32 usec;
} dnxc_sw_state_journal_mutex_take_entry_t;

/**
 * \brief - sw state journal 'mutex_take' entry structure
 */
typedef struct dnxc_sw_state_journal_mutex_give_entry_d
{
    sw_state_mutex_t *ptr_mtx;
} dnxc_sw_state_journal_mutex_give_entry_t;

/**
 * \brief - sw state journal 'htbl entry add' entry structure
 */
typedef struct dnxc_sw_state_journal_htbl_entry_d
{
    /*
     * ptr to hash table structure
     */
    sw_state_htbl_t hash_table;
} dnxc_sw_state_journal_htbl_entry_t;

/**
 * \brief - sw state journal entry data is union of all entry types
 */
typedef union dnxc_sw_state_journal_entry_data_d
{
    dnxc_sw_state_journal_memcpy_entry_t memcpy_data;
    dnxc_sw_state_journal_alloc_entry_t alloc_data;
    dnxc_sw_state_journal_free_entry_t free_data;
    dnxc_sw_state_journal_mutex_take_entry_t mutex_take;
    dnxc_sw_state_journal_mutex_give_entry_t mutex_give;
    dnxc_sw_state_journal_htbl_entry_t htbl_data;
} dnxc_sw_state_journal_entry_data_t;

/**
 * \brief - sw state journal entry structure
 */
typedef struct dnxc_sw_state_journal_entry_d
{
    dnxc_sw_state_journal_entry_type_t entry_type;
    uint32 nof_elements;
    uint32 element_size;
    uint32 module_id;
    dnxc_sw_state_journal_entry_data_t data;
} dnxc_sw_state_journal_entry_t;

/**
 * \brief
 *  Check if init has been done as part of journaling preconditions.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnxc_sw_state_journal_is_done_init(
    int unit);

/**
 * \brief
 *  Initialize sw state journal
 * \param [in] unit  - Device Id 
 * \param [in] rollback_journal_cbs  - Generic journaling callbacks.
 * \param [in] access_cb - Callback swstate journal is being accessed
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_initialize(
    int unit,
    dnxc_rollback_journal_cbs_t rollback_journal_cbs,
    dnxc_sw_state_journal_access_cb access_cb,
    uint8 is_comparison);

/**
 * \brief
 *  Destroy the sw state journal
 * \param [in] unit  - Device Id 
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_destroy(
    int unit,
    uint8 is_comparison);

/**
 * \brief
 *  The function will roll-back the current sw state journal transaction
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
shr_error_e dnxc_sw_state_journal_roll_back(
    int unit,
    uint8 only_head_rollback,
    uint8 is_comparison);

/**
 * \brief
 *  The function will clear and deactivate the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] only_head_clear - Indicate if only the head of the journal should be rollbacked
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_clear(
    int unit,
    uint8 only_head_clear,
    uint8 is_comparison);

/**
 * \brief
 *  The function will log a 'free' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - module id
 * \param [in] nof_elements - Number of elements
 * \param [in] element_size - Element size in bytes
 * \param [in] ptr - The pointer to the location that holds the pointer to the data to be freed.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_log_free(
    int unit,
    uint32 module_id,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr);

/**
 * \brief
 *  The function will log a 'alloc' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - module id
 * \param [in] ptr - The pointer to the location that holds the pointer to the data that was allocated
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_log_alloc(
    int unit,
    uint32 module_id,
    uint8 **ptr);

/**
 * \brief
 *  The function will log a 'alloc' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - module id
 * \param [in] size - The size of the of the data to be inserted into the log
 * \param [in] ptr - The pointer to the data to be inserted into the log
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_log_memcpy(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *ptr);

#ifdef BCM_DNX_SUPPORT
/**
 * \brief
 *  The function will log a 'htbl' entry in the sw state journal.
 *  Htbl journaling is currently supported only for Comparison Journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - module id
 * \param [in] hash_table - Pointer to hash table structure
 * \param [in] key - Pointer to key
 * \return
 *   int - Error Type
 * \remark
 *   * Assumes that the comparison journal is already suppressed
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_log_htbl(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key);
#endif

/**
 * \brief
 *  The function will log a 'mutex_take' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - Module id
 * \param [in] ptr_mtx - Pointer to swstate mutex
 * \param [in] usec - Mutex usec
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_mutex_take(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx,
    uint32 usec);

/**
 * \brief
 *  The function will log a 'mutex_give' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - Module id
 * \param [in] ptr_mtx - Pointer to swstate mutex
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_mutex_give(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx);

/**
 * \brief
 *  Get sequence id of the head entry of the sw state journal
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
shr_error_e dnxc_sw_state_journal_get_head_seq_id(
    int unit,
    uint8 is_comparison,
    uint32 *seq_id);

/**
 * \brief
 *  Stamp with a string all unstamped entries in the comparison and rollback journal
 * \param [in] unit   - Device Id
 * \param [in] format   - String with which all to stamp all unstamped entries in the journal
 * \param [in] args   - Associated variable list for format string parameter
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_entries_stamp(
    int unit,
    char *format,
    va_list args);

/**
 * \brief
 *  Check if swstate stamping should be performend
 * \param [in] unit   - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnxc_sw_state_journal_should_stamp(
    int unit);

/**
 * \brief
 *  Temporary disable all swstate journals
 * \param [in] unit   - Device Id
 * \param [in] is_disabled   - Indicate if the suppression counter should be incremented or decremented.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_all_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable);

/**
 * \brief
 *  Temporary disable the comparison journal
 * \param [in] unit   - Device Id
 * \param [in] is_disabled   - Indicate if the suppression counter should be incremented or decremented.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_comparison_journal_tmp_suppress(
    int unit,
    uint8 is_disabled);

/**
 * \brief
 *  Exclude a swstate access by stamp
 * \param [in] unit   - Unit Id
 * \param [in] stamp - sw_state table stamp string
 * \param [in] is_comparison - Indicate if comparison or rollback journal
 */
shr_error_e dnxc_sw_state_journal_exclude_by_stamp(
    int unit,
    char *stamp,
    uint8 is_comparison);

/**
 * \brief
 *  Clear the set of excluded sw_state tables
 * \param [in] unit   - Unit Id
 * \param [in] is_comparison - Indicate if comparison or rollback journal
 */
shr_error_e dnxc_sw_state_journal_exclude_clear(
    int unit,
    uint8 is_comparison);

/**
 * \brief
 *  Print rollback or comparison swstate journal
 * \param [in] unit   - Unit Id
 * \param [in] is_comparison - Indicate if comparison or rollback journal
 */
shr_error_e dnxc_sw_state_journal_print(
    int unit,
    uint8 is_comparison);

/**
 * \brief
 *   Toggle swstate journal logger.
 *   Each entry being inserted in the journal will be printed on a continuous basis.
 * \param [in] unit   - Device Id
 * \param [in] handle - Handle to journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_sw_state_journal_logger_state_change(
    int unit,
    uint8 is_comparison);

/**
 * \brief - Temporary disable the comparison journal
 */
#define DNXC_SW_STATE_COMPARISON_SUPPRESS(_unit)\
    dnxc_sw_state_comparison_journal_tmp_suppress(_unit, TRUE)

/**
 * \brief - Enable Temporary disabled the comparison journal
 */
#define DNXC_SW_STATE_COMPARISON_UNSUPPRESS(_unit)\
    dnxc_sw_state_comparison_journal_tmp_suppress(_unit, FALSE)

#else
#define DNXC_SW_STATE_COMPARISON_SUPPRESS(_unit)
#define DNXC_SW_STATE_COMPARISON_UNSUPPRESS(_unit)
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

/* } */
#endif /* _DNX_SW_STATE_JOURNAL_H */
