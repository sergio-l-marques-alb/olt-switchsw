/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is a rollback journal,
 * saving the old values that were overridden
 * by access calls since the beginning of the last transaction.
 */

#ifndef _DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_JOURNAL_H
/* { */
#define _DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnxc/dnxc_rollback_journal.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/dbal_structures.h>

/**
 * \brief - state journal 'dbal table' entry structure
 */
typedef struct dnxc_generic_state_journal_dbal_table_entry_d
{
    dbal_tables_e table_id;
} dnxc_generic_state_journal_dbal_table_entry_t;
#endif

/**
 * \brief - state journal 'memcpy' entry structure
 */
typedef struct dnxc_generic_state_journal_memcpy_entry_d
{
    uint8 *ptr;
    uint32 size;
} dnxc_generic_state_journal_entry_t;

/**
 * \brief - state journal 'free' entry structure
 */
typedef struct dnxc_generic_state_journal_free_entry_d
{
    uint32 nof_elements;
    uint32 element_size;
    uint8 *location;
    uint8 **ptr_location;
} dnxc_generic_state_journal_free_entry_t;

/**
 * \brief - describes state rollback
 */
typedef struct dnxc_generic_state_journal_d
{
    dnxc_rollback_journal_handle_t handle;
} dnxc_generic_state_journal_t;

/**
 * \brief - state journal internals
 */
typedef struct dnxc_generic_state_journal_manager_d
{
    int dnxc_generic_state_journal_dummy;
    dnxc_generic_state_journal_t journals[2];
} dnxc_generic_state_journal_manager_t;

/**
 * \brief
 *  State rollback journal rollback handler
 * \param [in] unit - Device Id
 * \param [in] metadata - Stored entry information
 * \param [in] payload - Entry data
 * \param [in] stamp - Entry stamp
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef shr_error_e(
    *dnxc_generic_state_journal_rollback_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp);

/**
 * \brief
 *  State journal print handler
 * \param [in] unit - Device Id
 * \param [in] metadata - Stored entry information
 * \param [in] payload - Entry data
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef shr_error_e(
    *dnxc_generic_state_journal_print_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload);

/**
 * \brief
 *  Initialize state journal
 * \param [in] unit  - Device Id
 * \param [in] rollback_journal_cbs  - Generic journaling callbacks.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_initialize(
    int unit,
    dnxc_rollback_journal_cbs_t rollback_journal_cbs);

/**
 * \brief
 *  Destroy the state journal
 * \param [in] unit  - Device Id
 * \param [in] is_comparison - Indicate if comparison or error recovery journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_destroy(
    int unit,
    uint8 is_comparison);

/**
 * \brief
 *  The function will log a 'memcpy' entry in the state journal
 * \param [in] unit - Device Id
 * \param [in] size - The size of the of the data to be inserted into the journal
 * \param [in] ptr - The pointer to the data to be inserted into the journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_log_memcpy(
    int unit,
    uint32 size,
    uint8 *ptr);

/**
 * \brief
 *  The function will log a 'free' entry in the state journal
 * \param [in] unit - Device Id
 * \param [in] nof_elements - Number of elements that will be freed
 * \param [in] element_size - Size of each element
 * \param [in] ptr_location - Pointer to location that will be freed
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_log_free(
    int unit,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr_location);

#ifdef BCM_DNX_SUPPORT
/**
 * \brief
 *  This function will back dynamic dbal table creation
 * \param [in] unit - Device Id
 * \param [in] table_id - Table id to be journaled
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_log_dbal_table_create(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 *  This function will back dynamic dbal table destroy
 * \param [in] unit - Device Id
 * \param [in] table_id - Table id to be journaled
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_log_dbal_table_destroy(
    int unit,
    dbal_tables_e table_id);
#endif

/**
 * \brief
 *  Add a new entry to the rollback journal
 * \param [in] unit   - Device Id
 * \param [in] metadata - Pointer to entry description structure
 * \param [in] metadata_size - Size of entry description structure
 * \param [in] payload - Pointer to entry data
 * \param [in] payload_size - Size of entry data
 * \param [in] rollback_handler - Entry callback used at rollback
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_entry_new(
    int unit,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnxc_generic_state_journal_rollback_handler rollback_handler);

/**
 * \brief
 *  Clear and deactivate the state journal
 * \param [in] unit - Device Id
 * \param [in] only_head_clear - Indicate if only the head of the journal should be rollbacked
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_clear(
    int unit,
    uint8 only_head_clear);

/**
 * \brief
 *  Get sequence id of the head entry of the state journal
 * \param [in] unit   - Device Id
 * \param [out] seq_id   - Sequence id to be returned
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_get_head_seq_id(
    int unit,
    uint32 *seq_id);

/**
 * \brief
 *  The function will roll-back the current state journal transaction
 * \param [in] unit - Device Id
 * \param [in] only_head_rollback - Indicate if only the head of the journal should be rollbacked
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_generic_state_journal_roll_back(
    int unit,
    uint8 only_head_rollback);

/* } */
#endif /* _DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_JOURNAL_H */
