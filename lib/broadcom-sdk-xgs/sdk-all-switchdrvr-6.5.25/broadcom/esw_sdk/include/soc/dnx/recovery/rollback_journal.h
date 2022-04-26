/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is a rollback journal,
 * saving the old values that were overridden
 * by access calls since the beginning of the last transaction.
 */

#ifndef _DNX_ROLLBACK_JOURNAL_JOURNAL_H
/* { */
#define _DNX_ROLLBACK_JOURNAL_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>

#ifdef DNX_ERR_RECOVERY_VALIDATION
/*
 * This macro enables rollback journal "assert mode",
 * rollback journal validation error will fail with assertion.
 * Assert mode reuires swstate "assert mode".
 * When turned off errors are returned instead of assertions enforced.
 */
#define DNX_ROLLBACK_JOURNAL_VALIDATION_ASSERT

#ifdef DNX_ROLLBACK_JOURNAL_VALIDATION_ASSERT
#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT(_expr)\
    DNXC_SW_STATE_IF_ERR_ASSERT(_expr)

#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)\
    DNXC_SW_STATE_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)
#else /* DNX_ROLLBACK_JOURNAL_VALIDATION_ASSERT */
#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT(_expr)\
    SHR_IF_ERR_EXIT(_expr)

#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)\
    SHR_IF_ERR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)
#endif /* DNX_ROLLBACK_JOURNAL_VALIDATION_ASSERT */

#else /* DNX_ERR_RECOVERY_VALIDATION */
#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT(_expr)
#define DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)
#endif /* DNX_ERR_RECOVERY_VALIDATION */
/**
 * \brief - limit the maximum number of journals supported
 */
#define DNX_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM 6

#define DNX_ROLLBACK_JOURNAL_MAX_STAMP_SIZE 512
#define DNX_ROLLBACK_JOURNAL_CHUNK_SIZE (20*4096)

/**
 * \brief
 *  Rollback journal rollback handler
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
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
    *dnx_rollback_journal_rollback_handler) (
    int unit,
    int er_thread_id,
    uint8 *metadata,
    uint8 *payload,
    char *stamp);

/**
 * \brief
 *  Journal print handler
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
    *dnx_rollback_journal_print_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload);

/**
 * \brief
 *  Rollback journal rollback traverse entries handler.
 *  Should be used when the rollback of an entry has an effect on
 *  a subset of the remaining entries in the journal.
 * \param [in] unit - Device Id
 * \param [in] metadata - Stored entry information
 * \param [in] payload - Entry data
 * \param [in] traversed_metadata - Traversed entry information
 * \param [in] traversed_payload - Traversed entry data
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef shr_error_e(
    *dnx_rollback_journal_rollback_traverse_handler) (
    int unit,
    uint8 *metadata,
    uint8 *payload,
    uint8 *traversed_metadata,
    uint8 *traversed_payload);

/**
 * \brief
 *  Rollback journal distinct callback.
 *  Used by the rollback journal in order to determine if two journal entries are considered to be distinct.
 * \param [in] unit - Device Id
 * \param [in] first_metadata - Metadata for first entry
 * \param [in] second_metadata - Metadata for second entry
 * \param [out] result - result if the two entries do not match
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef shr_error_e(
    *dnx_rollback_journal_is_distinct_cb) (
    int unit,
    uint8 *first_metadata,
    uint8 *first_payload,
    uint8 *second_metadata,
    uint8 *second_payload,
    uint8 *result);

/**
 * \brief
 *  Callback checks if journaling is enabled
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \return
 *   uint8 - Is journaling on
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef uint8 (
    *dnx_rollback_journal_is_on_cb) (
    int unit,
    int er_thread_id);

/**
 * \brief
 *  Callback to get new journal entry sequence id
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \return
 *   uint32 - new journal entry sequence id
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef uint32 (
    *dnx_rollback_journal_new_entry_seq_id_get_cb) (
    int unit,
    int er_thread_id);

/**
 * \brief - define all states of a rollback journal entries
 */
typedef enum dnx_rollback_journal_entry_state_d
{
    /*
     * journal performs no rollback/traverse actions on this entry, just destroys
     */
    DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID = 0,
    /*
     * journal performs actions on valid entries, destroyed after rollback,
     * entry CAN be invalidated during transaction
     */
    DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
    /*
     * same as valid,
     * entry CANNOT be invalidated during transaction
     */
    DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID
} dnx_rollback_journal_entry_state;

/**
 * \brief - Rollback journal memory chunk structure
 */
typedef struct dnx_rollback_journal_mem_chunk_d
{
    struct dnx_rollback_journal_mem_chunk_d *prev;
    struct dnx_rollback_journal_mem_chunk_d *next;
    uint32 data_size;
    int32 num_allocated_bytes;
    int32 available_space_at_end;
} dnx_rollback_journal_mem_chunk_t;

/**
 * \brief - Rollback journal memory chunk structure
 */
typedef struct dnx_rollback_journal_mem_pool_d
{
    dnx_rollback_journal_mem_chunk_t *head;
    dnx_rollback_journal_mem_chunk_t *tail;
    int32 num_chunks;
    int32 total_bytes_allocated;
} dnx_rollback_journal_mem_pool_t;

/**
 * \brief - Rollback journal entry structure
 */
typedef struct dnx_rollback_journal_entry_d
{
    dnx_rollback_journal_mem_chunk_t *mem_chunk;
    int32 split_offset;         /* The offset where the entry is splitted into two chunks, 0 means no split */
    struct dnx_rollback_journal_entry_d *next;
    struct dnx_rollback_journal_entry_d *prev;
    /*
     * invalid entries are not rolled-back, just destroyed
     */
    dnx_rollback_journal_entry_state state;
    /*
     * indicate the sequence in which the operation represented by the entry occured
     */
    uint32 seq_id;
    uint32 metadata_size;
    uint32 payload_size;
    dnx_rollback_journal_rollback_handler rollback_handler;
    dnx_rollback_journal_rollback_traverse_handler rollback_traverse_handler;
    dnx_rollback_journal_print_handler print_handler;
    /*
     * stamped string
     */
    char *stamp;
    dnx_rollback_journal_mem_chunk_t *stamp_mem_chunk;
    uint32 stamp_size;
} dnx_rollback_journal_entry_t;

/**
 * \brief - Rollback journal callbacks
 */
typedef struct dnx_rollback_journal_cbs_d
{
    dnx_rollback_journal_is_on_cb is_on;
    dnx_rollback_journal_new_entry_seq_id_get_cb seq_id_get;
} dnx_rollback_journal_cbs_t;

/**
 * \brief - Rollback journal main structure
 */
typedef struct dnx_rollback_journal_d
{
    dnx_rollback_journal_mem_pool_t mem_pool;
    dnx_rollback_journal_entry_t *head;
    dnx_rollback_journal_entry_t *tail;
    uint8 is_inverse_journaling;
    uint8 is_distinct;
    uint32 entry_counter;
    uint32 size;
    uint32 is_journaled_disabled_counter;
    uint32 current_allocated_memory;
    dnx_rollback_journal_type_e type;
    dnx_rollback_journal_subtype_e subtype;
    dnx_rollback_journal_cbs_t cbs;
} dnx_rollback_journal_t;

/**
 * \brief - Rollback journal pool
 */
typedef struct dnx_rollback_journal_pool_d
{
    uint8 is_any_journal_rolling_back;
    uint32 count;
    dnx_rollback_journal_t *entries[DNX_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM];
} dnx_rollback_journal_pool_t;

/**
 * \brief - Rollback journal handle
 */
typedef uint32 dnx_rollback_journal_handle_t;

#define DNX_ROLLBACK_JOURNAL_INVALID_HANDLE -1

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
uint8 dnx_rollback_journal_is_done_init(
    int unit);

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
shr_error_e dnx_rollback_journal_is_init_check_bypass(
    int unit,
    uint8 is_on);

/**
 * \brief
 *  Returns true if ER is bypassed.
 * \param [in] unit - Device Id
 * \return
 *   int - True or False
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_rollback_journal_is_error_recovery_bypassed(
    int unit);

/**
 * \brief
 *  Check if journaling is enabled
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] handle - Handle to journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_rollback_journal_is_on(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_handle_t handle);

/**
 * \brief
 *  Create new rollback journal
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] is_inverse_journaling - Indicate if new entries should be added to tail of the journal, instead of head.
 * \param [in] type - Indicate the type of the journal
 * \param [in] subtype -  Indicate the subtype of the journal
 * \param [in] cbs - Callbacks used by the rollback journal
 * \param [out] handle - Handle to newly created journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_new(
    int unit,
    int er_thread_id,
    uint8 is_inverse_journaling,
    dnx_rollback_journal_type_e type,
    dnx_rollback_journal_subtype_e subtype,
    dnx_rollback_journal_cbs_t cbs,
    dnx_rollback_journal_handle_t * handle);

/**
 * \brief
 *  Destroys a rollback journal
 * \param [in] unit - Device Id
 * \param [out] handle - Journal handle
 * \return
 *   int - Error Type
 * \remark
 *   * Should be called only on device deteach
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_destroy(
    int unit,
    dnx_rollback_journal_handle_t * handle);

/**
 * \brief
 *  Roll-back rollback journal
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] only_head_rollback - Indicate if only the head of the journal should be rollbacked
 * \param [in] handle - Handle to journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_rollback(
    int unit,
    int er_thread_id,
    uint8 only_head_rollback,
    dnx_rollback_journal_handle_t handle);

/**
 * \brief
 *  Clear rollback journal
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] only_head_clear - Indicate if only the head of the journal should be rollbacked
 * \param [in] handle - Handle to journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_clear(
    int unit,
    int er_thread_id,
    uint8 only_head_clear,
    dnx_rollback_journal_handle_t handle);

/**
 * \brief
 *  Add a new entry to the rollback journal
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] handle - Handle to journal
 * \param [in] metadata - Pointer to entry description structure
 * \param [in] metadata_size - Size of entry description structure
 * \param [in] payload - Pointer to entry data
 * \param [in] payload_size - Size of entry data
 * \param [in] default_state - Specify the default state of the entry
 * \param [in] rollback_handler - Entry callback used at rollback
 * \param [in] rollback_traverse_handler - Entry callback, called for each remaining entry inthe journal after the targed one is being rollbacked.
 * \param [in] print_handler - Entry callback, used for printing journal contents.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_entry_new(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_handle_t handle,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnx_rollback_journal_entry_state default_state,
    dnx_rollback_journal_rollback_handler rollback_handler,
    dnx_rollback_journal_rollback_traverse_handler rollback_traverse_handler,
    dnx_rollback_journal_print_handler print_handler);

/**
 * \brief
 *  Get sequence id of the head entry
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [out] seq_id   - Sequence id to be returned
 * \param [in] handle - Handle to journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_get_head_seq_id(
    int unit,
    int er_thread_id,
    uint32 *seq_id,
    dnx_rollback_journal_handle_t handle);

/**
 * \brief
 *  Invalidate the last entry that was inserted into a journal.
 *  Invalidation occurs only if entry is not marked as "ALWAYS VALID"
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] handle - Handle to journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_invalidate_last_inserted_entry(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_handle_t handle);

/**
 * \brief
 *  Removes all entries that are considered dublicate, based on an entry comparison callback
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] handle - Handle to journal
 * \param [in] is_distinct_cb - Determines if two entries are distinct
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_distinct(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_handle_t handle,
    dnx_rollback_journal_is_distinct_cb is_distinct_cb);

/**
 * \brief
 *  Increase or decrease the temporary disablement counter for journal.
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] is_disable - Increase the counter if TRUE
 * \param [in] handle - Handle to journal
 * \param [in] is_unsafe - Indicate if "is_on" check should be performed for the journal before suppress
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_disabled_counter_change(
    int unit,
    int er_thread_id,
    uint8 is_disable,
    dnx_rollback_journal_handle_t handle,
    uint8 is_unsafe);

/**
 * \brief
 *  Get temporary disablement counter for journal.
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] handle - Handle to journal
 * \param [out] counter - Counter retrieved
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_disabled_counter_get(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_handle_t handle,
    uint32 *counter);

/**
 * \brief
 *  Stamp with a string all unstamped entries in the journal
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \param [in] handle - Handle to journal
 * \param [in] format   - String with which all to stamp all unstamped entries in the journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_entries_stamp(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_handle_t handle,
    char *format);

/**
 * \brief
 *  Indicate if any journal is rolling back
 * \param [in] unit - Device Id
 * \param [in] er_thread_id - index given to thread by ER thread manager
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_rollback_journal_is_any_journal_rolling_back(
    int unit,
    int er_thread_id);

/**
 * \brief
 *   Prints the contents of a rollback journal
 * \param [in] unit   - Device Id
 * \param [in] handle - Handle to journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_print(
    int unit,
    dnx_rollback_journal_handle_t handle);

/**
 * \brief
 *   Toggle rollback journal logger.
 *   Each entry being inserted in the rollback journal will be printed on a continuous basis.
 * \param [in] unit   - Device Id
 * \param [in] handle - Handle to journal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_rollback_journal_logger_state_change_by_type(
    int unit,
    dnx_rollback_journal_handle_t handle);

shr_error_e dnx_rollback_journal_is_logging(
    int unit,
    dnx_rollback_journal_handle_t handle,
    uint8 *is_logging);

/**
 * \brief
 *   Returns the current amount of allocated chunks of memory (in bytes), that have not been freed yet.
 * \param [in] unit         - Device Id
 * \param [in] er_thread_id - Error recovery thread Id
 * \return
 *   uint32 - Amount of allocated bytes
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32 dnx_rollback_journal_get_current_alloc_memory(
    int unit,
    int er_thread_id);

/* } */
#endif /* _DNX_ROLLBACK_JOURNAL_JOURNAL_H */
