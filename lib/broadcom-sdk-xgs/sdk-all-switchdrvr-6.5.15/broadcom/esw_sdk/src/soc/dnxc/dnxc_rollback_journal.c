/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is a generic rollback journal,
 * saving the old values that were overridden
 * by access calls since the beginning of the last transaction.
 * The ADT is double-ended queue, implemented as double-linked list.
 */

#include <assert.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <soc/dnxc/dnxc_rollback_journal.h>
#include <soc/dnxc/dnxc_rollback_journal_diag.h>
#include <soc/dnxc/dnxc_rollback_journal_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/**
 * \brief - err reocvery journal pool.
 */
static dnxc_rollback_journal_pool_t dnxc_rollback_journal_pool[SOC_MAX_NUM_DEVICES] = { {0} };

#ifdef DNX_ERR_RECOVERY_VALIDATION
/**
 * \brief - validate the journal handle
 */
#define DNXC_ROLLBACK_JOURNAL_VALIDATE_HANDLE(_handle)\
do {\
    if((DNXC_ROLLBACK_JOURNAL_INVALID_HANDLE == (_handle))\
     ||((_handle) >= DNXC_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM)\
     ||(NULL == dnxc_rollback_journal_pool[unit].entries[(_handle)])) {\
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: invalid journal handle.\n%s%s%s", EMPTY, EMPTY, EMPTY);\
    }\
} while(0)
#else
#define DNXC_ROLLBACK_JOURNAL_VALIDATE_HANDLE(_handle)
#endif

/**
 * \brief - get journal reference based on a handle.
 */
#define DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal_ptr, _handle)   \
do {                                                                  \
    DNXC_ROLLBACK_JOURNAL_VALIDATE_HANDLE((_handle));                 \
    journal_ptr = dnxc_rollback_journal_pool[unit].entries[(_handle)];\
} while(0)

/**
 * \brief - get pointer to entry's metadata, given an entry
 */
#define DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata_ptr, entry_ptr)   \
do {                                                                           \
    metadata_ptr = ((uint8 *)entry_ptr) + sizeof(dnxc_rollback_journal_entry_t);\
} while(0)

/**
 * \brief - get pointer to entry's payload, given an entry
 */
#define DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload_ptr, entry_ptr)  \
do {                                                                        \
    payload_ptr = ((uint8 *)entry_ptr)                                      \
    + sizeof(dnxc_rollback_journal_entry_t) + (entry_ptr->metadata_size);    \
} while(0)

/**
 * \brief - check if entry state is 'valid' or 'always valid'
 */
#define DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(_state) \
    ((DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_VALID == (_state)) \
     || (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID == (_state)))

#ifdef DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS
#define DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size)\
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_diag_alloc(_unit, _jrnl_type, _jrnl_subtype, _size)
#else
#define DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size)
#endif

#define DNXC_ROLLBACK_JOURNAL_ALLOC(_unit, _ptr, _type, _size, _str, _jrnl_type, _jrnl_subtype)\
do {                                                                                           \
    (_ptr) = (_type)sal_alloc((_size), (_str));                                                \
    DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size);          \
} while(0)

/**
 * \brief - insert an entry to the head or tail of the dequeue
 */
static shr_error_e
dnxc_rollback_journal_deque_entry_insert(
    int unit,
    uint8 is_tail,
    dnxc_rollback_journal_t * journal,
    dnxc_rollback_journal_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);

    if (0 == journal->entry_counter)
    {
#ifdef DNX_ERR_RECOVERY_VALIDATION
        if ((NULL != journal->tail) || (NULL != journal->head))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: journal state is inconsistent on entry push.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
#endif

        entry->prev = NULL;
        entry->next = NULL;

        journal->tail = entry;
        journal->head = entry;
    }
    else
    {
        /*
         * add entry to head or tail
         */
        if (is_tail)
        {
            entry->prev = journal->tail;
            journal->tail->next = entry;
            journal->tail = entry;
        }
        else
        {
            entry->next = journal->head;
            journal->head->prev = entry;
            journal->head = entry;
        }
    }

    journal->entry_counter++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - remove an entry from the deque, frees memory associated with entry.
 */
static shr_error_e
dnxc_rollback_journal_entry_remove(
    int unit,
    uint8 is_tail,
    dnxc_rollback_journal_t * journal)
{
    dnxc_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if ((NULL == journal->tail) || (NULL == journal->head))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: no entries found in journal to pop.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * keep reference to free, move head to next entry
     */
    if (is_tail)
    {
        entry = journal->tail;

        journal->size -= (sizeof(dnxc_rollback_journal_entry_t) + entry->metadata_size + entry->payload_size);

        journal->tail = journal->tail->prev;
        if (NULL != journal->tail)
        {
            journal->tail->next = NULL;
        }
    }
    else
    {
        entry = journal->head;

        journal->size -= (sizeof(dnxc_rollback_journal_entry_t) + entry->metadata_size + entry->payload_size);

        journal->head = journal->head->next;
        if (NULL != journal->head)
        {
            journal->head->prev = NULL;
        }
    }

    /*
     * free stamp string if it exists
     */
    if (NULL != entry->stamp)
    {
        sal_free(entry->stamp);
        entry->stamp = NULL;
    }

    /*
     * free the disconnected head
     */
    sal_free(entry);
    entry = NULL;

    journal->entry_counter--;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * sanity check counter if last entry
     */
    if (((NULL == journal->head) || (NULL == journal->tail)) && (0 != journal->entry_counter))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: invalid entry found in journal.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * reset journal head and tail pointer
     */
    if (0 == journal->entry_counter)
    {
        journal->head = NULL;
        journal->tail = NULL;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - examine an entry from the head or tail the sw state journal deque.
 */
static void
dnxc_rollback_journal_entry_examine(
    int unit,
    dnxc_rollback_journal_t * journal,
    dnxc_rollback_journal_entry_t ** entry,
    uint8 is_tail)
{
    if (is_tail)
    {
        *entry = (NULL == journal->tail) ? NULL : journal->tail;
    }
    else
    {
        *entry = (NULL == journal->head) ? NULL : journal->head;
    }
}

/**
 * \brief - traverse all remaining entries down the stack, call callback for each of them if defined
 */
static shr_error_e
dnxc_rollback_journal_entry_rollback_traverse(
    int unit,
    dnxc_rollback_journal_entry_t * top)
{
    uint8 *metadata = NULL;
    uint8 *payload = NULL;
    uint8 *metadata_traversed = NULL;
    uint8 *payload_traversed = NULL;

    dnxc_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get metadata nad payload of the top entry
     */
    DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, top);
    DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload, top);

    /*
     * Start traversal from the second entry
     */
    entry = top->next;

    /*
     * traverse the rest of the entries, use set callback
     */
    while (NULL != entry)
    {
        /*
         * no need to update if any of the entries are marked invalid
         */
        if (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata_traversed, entry);
            DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload_traversed, entry);

            SHR_IF_ERR_EXIT(top->rollback_traverse_handler
                            (unit, metadata, payload, metadata_traversed, payload_traversed));
        }

        entry = entry->next;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
uint8
dnxc_rollback_journal_is_on(
    int unit,
    dnxc_rollback_journal_handle_t handle)
{
    /*
     * validate the handle and the callback
     */
    if ((handle >= DNXC_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM)
        || (NULL == dnxc_rollback_journal_pool[unit].entries[handle])
        || (NULL == dnxc_rollback_journal_pool[unit].entries[handle]->cbs.is_on))
    {
        return FALSE;
    }

    /*
     * check if the journal is temporary disabled
     */
    if (dnxc_rollback_journal_pool[unit].entries[handle]->is_journaled_disabled_counter > 0)
    {
        return FALSE;
    }

    return dnxc_rollback_journal_pool[unit].entries[handle]->cbs.is_on(unit);
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_entry_new(
    int unit,
    dnxc_rollback_journal_handle_t handle,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnxc_rollback_journal_entry_state default_state,
    dnxc_rollback_journal_rollback_handler rollback_handler,
    dnxc_rollback_journal_rollback_traverse_handler rollback_traverse_handler,
    dnxc_rollback_journal_print_handler print_handler)
{
    uint32 total_size = 0;

    dnxc_rollback_journal_t *journal = NULL;
    dnxc_rollback_journal_entry_t *entry = NULL;
    uint8 *entry_metadata = NULL;
    uint8 *entry_payload = NULL;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * validate input
     */
    if ((NULL == metadata))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: invalid parameter.\n%s%s%s", EMPTY, EMPTY,
                                 EMPTY);
    }

    if (NULL == rollback_handler)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: rollback handler cannot be null.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * retrieve journal based on handle
     */
    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    total_size = sizeof(dnxc_rollback_journal_entry_t) + metadata_size + payload_size;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * check if we are going to exceeding the maximum size allowed
     */
    if ((0 != journal->max_size) && (journal->max_size < (journal->size + total_size)))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: journal exceeds to maximum size allowed .\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif

    /*
     * allocate the entry and payload together
     */
    DNXC_ROLLBACK_JOURNAL_ALLOC(unit, entry, dnxc_rollback_journal_entry_t *, total_size, "new rollback journal entry",
                                journal->type, journal->subtype);

    if (NULL == entry)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "rollback journal ERROR: failed to alloc entry's data.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    sal_memset(entry, 0x0, total_size);

    /*
     * set fields of the new entry
     */
    entry->next = NULL;
    entry->prev = NULL;
    entry->state = default_state;
    entry->metadata_size = metadata_size;
    entry->payload_size = payload_size;
    entry->rollback_handler = rollback_handler;
    entry->rollback_traverse_handler = rollback_traverse_handler;
    entry->print_handler = print_handler;
    entry->seq_id = (NULL == journal->cbs.seq_id_get) ? 0 : journal->cbs.seq_id_get(unit);
    entry->stamp = NULL;

    /*
     * copy metadata, always exists
     */
    DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(entry_metadata, entry);
    sal_memcpy(entry_metadata, metadata, metadata_size);

    /*
     * copy payload if exists
     */
    if (payload_size > 0)
    {
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(entry_payload, entry);
        sal_memcpy(entry_payload, payload, payload_size);
    }

    /*
     * add entry to head or tail of the deque
     */
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_deque_entry_insert(unit, journal->is_inverse_journaling, journal, entry));

    /*
     * update the journal size
     */
    journal->size += total_size;

    /*
     * mark that the journal is not distinct, since we just added an entry 
     */
    journal->is_distinct = FALSE;

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_new(
    int unit,
    uint8 is_inverse_journaling,
    dnxc_rollback_journal_type_e type,
    dnxc_rollback_journal_subtype_e subtype,
    dnxc_rollback_journal_cbs_t cbs,
    dnxc_rollback_journal_handle_t * handle)
{
    dnxc_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == handle)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: invalid parameter.\n%s%s%s", EMPTY, EMPTY,
                                 EMPTY);
    }

    /*
     * Set default error value for handle
     */
    *handle = DNXC_ROLLBACK_JOURNAL_INVALID_HANDLE;

    if (dnxc_rollback_journal_pool[unit].count >= DNXC_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: cannot create new journal, maximum number of journals allowed reached.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    /*
     * Create the new journal
     */
    DNXC_ROLLBACK_JOURNAL_ALLOC(unit, dnxc_rollback_journal_pool[unit].entries[dnxc_rollback_journal_pool[unit].count],
                                dnxc_rollback_journal_t *, sizeof(dnxc_rollback_journal_t), "rollback journal alloc",
                                type, subtype);

    /*
     * Used for convenience
     */
    journal = dnxc_rollback_journal_pool[unit].entries[dnxc_rollback_journal_pool[unit].count];

    if (NULL == journal)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: cannot create new journal.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    sal_memset(journal, 0x0, sizeof(dnxc_rollback_journal_t));

    /*
     * Set default values for new journal
     */
    journal->head = NULL;
    journal->tail = NULL;
    journal->is_distinct = FALSE;
    journal->entry_counter = 0;
    journal->size = 0;
    journal->is_journaled_disabled_counter = 0;
    journal->max_size = DNXC_ROLLBACK_JOURNAL_MAX_SIZE;
    journal->cbs = cbs;
    journal->is_inverse_journaling = is_inverse_journaling;
    journal->type = type;
    journal->subtype = subtype;

    /*
     * Return to the user a handle to the newly created journal and increase the journal count
     */
    *handle = dnxc_rollback_journal_pool[unit].count++;

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_destroy(
    int unit,
    dnxc_rollback_journal_handle_t * handle)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_VALIDATE_HANDLE(*handle);

    /*
     * clear all entries in the journal
     */
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_clear(unit, FALSE, *handle));

    sal_free(dnxc_rollback_journal_pool[unit].entries[*handle]);
    dnxc_rollback_journal_pool[unit].entries[*handle] = NULL;

    dnxc_rollback_journal_pool[unit].count--;

    *handle = DNXC_ROLLBACK_JOURNAL_INVALID_HANDLE;

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_disabled_counter_change(
    int unit,
    uint8 is_disable,
    dnxc_rollback_journal_handle_t handle)
{
    dnxc_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * modify the low level journal suppression only if the journal is on
     */
    if ((NULL == dnxc_rollback_journal_pool[unit].entries[handle])
        || (NULL == dnxc_rollback_journal_pool[unit].entries[handle]->cbs.is_on)
        || (!dnxc_rollback_journal_pool[unit].entries[handle]->cbs.is_on(unit)))
    {
        SHR_EXIT();
    }

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (is_disable)
    {
        journal->is_journaled_disabled_counter++;
    }
    else
    {
        if (0 == journal->is_journaled_disabled_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: disabled counter is already zero.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
        journal->is_journaled_disabled_counter--;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_rollback(
    int unit,
    uint8 only_head_rollback,
    dnxc_rollback_journal_handle_t handle)
{
    shr_error_e rv_rollback = _SHR_E_NONE;
    dnxc_rollback_journal_t *journal = NULL;
    dnxc_rollback_journal_entry_t *entry = NULL;
    uint8 *metadata = NULL;
    uint8 *payload = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (0 != journal->is_journaled_disabled_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: journal cannot be rolled back when suppression is active.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    /*
     * stop all journaling during the current journal rollback, mark current journal as rolling back
     */
    dnxc_rollback_journal_pool[unit].is_any_journal_rolling_back = TRUE;

    while (journal->entry_counter > 0)
    {
        rv_rollback = _SHR_E_NONE;

        /*
         * examine deque' head
         */
        dnxc_rollback_journal_entry_examine(unit, journal, &entry, FALSE);

#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (NULL == entry)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
#endif

        /*
         * Calculate the metadata and payload locations
         */
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload, entry);

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * Client handles the entry rollback. Must exist.
         */
        if (NULL == entry->rollback_handler)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: entry's rollback handler cannot be null.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }
#endif

        if (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            rv_rollback = entry->rollback_handler(unit, metadata, payload, entry->stamp);
        }

        /*
         * Call traverse callback for each remaining entry if callback is set. Optinal.
         */
        if ((NULL != entry->rollback_traverse_handler)
            && (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state)) && (_SHR_E_NONE == rv_rollback))
        {
            rv_rollback = dnxc_rollback_journal_entry_rollback_traverse(unit, entry);
        }

        /*
         * Pop head from the journal stack, free entry's memory
         */
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_remove(unit, FALSE, journal));

        /*
         * remove entry in all cases, return error if it exists after that.
         */
        SHR_IF_ERR_EXIT_NO_MSG(rv_rollback);

        if (only_head_rollback)
        {
            break;
        }
    }

    dnxc_rollback_journal_pool[unit].is_any_journal_rolling_back = FALSE;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * if it is the last entry, sanity check the journal size
     */
    if ((0 == journal->entry_counter) && (0 != journal->size))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: journal entry count zero but size is not zero.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

exit:
    if (dnxc_rollback_journal_pool[unit].is_any_journal_rolling_back)
    {
        dnxc_rollback_journal_pool[unit].is_any_journal_rolling_back = FALSE;
    }
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_clear(
    int unit,
    uint8 only_head_clear,
    dnxc_rollback_journal_handle_t handle)
{
    dnxc_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (0 != journal->is_journaled_disabled_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: journal cannot be cleared when suppression is active.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * Pop journal entries starting from the head
     */
    while (journal->entry_counter > 0)
    {
        /*
         * remove head and get sequence id of the new head
         */
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_remove(unit, FALSE, journal));

        if (only_head_clear)
        {
            break;
        }
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * sanity check the size and entry counter of the journal
     * if we have cleared all entries
     */
    if (!only_head_clear && ((0 != journal->size) || (0 != journal->entry_counter)))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: journal cleared but entries or size are non zero.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_get_head_seq_id(
    int unit,
    uint32 *seq_id,
    dnxc_rollback_journal_handle_t handle)
{
    dnxc_rollback_journal_t *journal = NULL;
    dnxc_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == seq_id)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,
                                 "rollback journal ERROR: sequence id parameter cannot be null.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif

    /*
     * default invalid sequence id if no entries are in the journal
     */
    *seq_id = 0;

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (journal->entry_counter > 0)
    {
        /*
         * examine deque's head
         */
        dnxc_rollback_journal_entry_examine(unit, journal, &entry, FALSE);

#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (NULL == entry)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
#endif

        *seq_id = entry->seq_id;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_invalidate_last_inserted_entry(
    int unit,
    dnxc_rollback_journal_handle_t handle)
{
    dnxc_rollback_journal_t *journal = NULL;
    dnxc_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (journal->entry_counter == 0)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: Cannot invalidate last inserted entry. No entries found in journal.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * mark deque's head as invalid
     */
    dnxc_rollback_journal_entry_examine(unit, journal, &entry, FALSE);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == entry)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /*
     * Invalidate entry only if it not marked as "always valid"
     */
    if (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID != entry->state)
    {
        entry->state = DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_distinct(
    int unit,
    dnxc_rollback_journal_handle_t handle,
    dnxc_rollback_journal_is_distinct_cb is_distinct_cb)
{
    uint8 are_distinct = FALSE;

    dnxc_rollback_journal_t *journal = NULL;

    dnxc_rollback_journal_entry_t *entry = NULL;
    dnxc_rollback_journal_entry_t *trav_entry = NULL;

    uint8 *metadata = NULL;
    uint8 *payload = NULL;

    uint8 *trav_metadata = NULL;
    uint8 *trav_payload = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (journal->is_distinct)
    {
        SHR_EXIT();
    }

    /*
     * traverse journal from head to tail the inverse journal
     */
    entry = journal->head;
    while (entry != NULL)
    {
        /*
         * if entry has been marked already as invalid no need to distinct
         */
        if (!DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            entry = entry->next;
            continue;
        }

        /*
         * get entry's metadata
         */
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload, entry);

        /*
         * traverse the rest of the double linked list,
         * check if entry should be disconnected
         */
        trav_entry = entry->next;
        while (trav_entry != NULL)
        {
            if (!DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(trav_entry->state))
            {
                trav_entry = trav_entry->next;
                continue;
            }

            /*
             * get entry's metadata and check if the two entries are distinct.
             * mark the entry as invlid
             */
            DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(trav_metadata, trav_entry);
            DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(trav_payload, trav_entry);
            SHR_IF_ERR_EXIT(is_distinct_cb(unit, metadata, payload, trav_metadata, trav_payload, &are_distinct));

            if (!are_distinct && (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID != trav_entry->state))
            {
                trav_entry->state = DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
            }

            trav_entry = trav_entry->next;
        }

        entry = entry->next;
    }

    journal->is_distinct = TRUE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_entries_stamp(
    int unit,
    dnxc_rollback_journal_handle_t handle,
    char *str)
{
    dnxc_rollback_journal_entry_t *entry = NULL;
    dnxc_rollback_journal_t *journal = NULL;
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == str)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: stamp string cannot be null.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
#endif

    size = sal_strlen(str);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (size > DNXC_ROLLBACK_JOURNAL_MAX_STAMP_SIZE)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: stamp size exceeds the maximum allowed.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif

    entry = journal->head;
    while (entry != NULL)
    {
        if (NULL != entry->stamp)
        {
            entry = entry->next;
            continue;
        }

        /*
         * size + null termination
         */
        DNXC_ROLLBACK_JOURNAL_ALLOC(unit, entry->stamp, char *,
                                    size + 1,
                                    "snapshot journal stamp array",
                                    journal->type,
                                    journal->subtype);

        if (NULL == entry->stamp)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: cannot allocate memory for stamp.\n%s%s%s", EMPTY, EMPTY,
                                     EMPTY);
        }

        sal_memset(entry->stamp, 0x0, size + 1);

        /*
         * save the stamped string into newly allocated memory
         */
        sal_strncpy(entry->stamp, str, size);

        entry = entry->next;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

/*
 * see .h file for description
 */
uint8
dnxc_rollback_journal_is_any_journal_rolling_back(
    int unit)
{
    return dnxc_rollback_journal_pool[unit].is_any_journal_rolling_back;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_print(
    int unit,
    dnxc_rollback_journal_handle_t handle)
{

    uint32 entry_number = 0;
    dnxc_rollback_journal_t *journal = NULL;
    dnxc_rollback_journal_entry_t *entry = NULL;
    dnxc_rollback_journal_entry_t *last_entry = NULL;

    uint8 *metadata = NULL;
    uint8 *payload = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    /*
     * print head of the journal
     */
    switch (journal->type)
    {
        case DNXC_ROLLBACK_JOURNAL_TYPE_ROLLBACK:
            LOG_CLI((BSL_META("Journal type: ROLLBACK\n")));
            break;
        case DNXC_ROLLBACK_JOURNAL_TYPE_COMPARISON:
            LOG_CLI((BSL_META("Journal type: COMPARISON\n")));
            break;
        default:
            LOG_CLI((BSL_META("Journal type: UNKNOWN\n")));
    }

    switch (journal->subtype)
    {
        case DNXC_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            LOG_CLI((BSL_META("Journal subtype: SWSTATE\n")));
            break;
        case DNXC_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            LOG_CLI((BSL_META("Journal subtype: DBAL\n")));
            break;
        case DNXC_ROLLBACK_JOURNAL_SUBTYPE_GENERIC:
            LOG_CLI((BSL_META("Journal subtype: GENERIC\n")));
            break;
        default:
            LOG_CLI((BSL_META("Journal subtype: UNKNOWN\n")));
    }

    if (dnxc_rollback_journal_is_on(unit, handle))
    {
        LOG_CLI((BSL_META("State: ON\n")));
    }
    else
    {
        LOG_CLI((BSL_META("State: OFF\n")));
    }

    LOG_CLI((BSL_META("Entries count: %u\n"), journal->entry_counter));

    LOG_CLI((BSL_META("Size: %u\n"), journal->size));

    LOG_CLI((BSL_META("Max size: %u\n"), journal->max_size));

    if (!(journal->is_inverse_journaling))
    {
        LOG_CLI((BSL_META("Journaling: head to tail\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Journaling: tail to head\n")));
    }

    if (journal->is_distinct)
    {
        LOG_CLI((BSL_META("Is distinct: YES\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Is distinct: NO\n")));
    }

    LOG_CLI((BSL_META("Handle id: %u\n"), handle));

    LOG_CLI((BSL_META("\n")));

    /*
     * print journal entries, traverse from head to tail
     */
    entry = journal->head;

    while (NULL != entry)
    {
        /*
         * used for sanity checking 
         */
        last_entry = entry;

        /*
         * Calculate the metadata and payload locations
         */
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload, entry);

        LOG_CLI((BSL_META("Entry number in journal: %u\n"), entry_number));

        LOG_CLI((BSL_META("Sequence id in transaction: %u\n"), entry->seq_id));

        if (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            LOG_CLI((BSL_META("Is entry valid: YES\n")));
        }
        else
        {
            LOG_CLI((BSL_META("Is entry valid: NO\n")));
        }

        if (NULL != entry->stamp)
        {
            LOG_CLI((BSL_META("Associated stamp is: %s\n"), entry->stamp));
        }
        else
        {
            LOG_CLI((BSL_META("Associated stamp is: NONE\n")));
        }

        if (NULL != entry->print_handler)
        {
            SHR_IF_ERR_EXIT(entry->print_handler(unit, metadata, payload));
        }

        entry = entry->next;
        entry_number++;
        LOG_CLI((BSL_META("\n")));
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * sanity check
     */
    if (last_entry != journal->tail)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: last non-null entry in journal is difference than the journal tail.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

#undef _ERR_MSG_MODULE_NAME
