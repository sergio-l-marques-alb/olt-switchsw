/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/drv.h>
#include <soc/dnxc/dnxc_rollback_journal.h>
#include <soc/dnxc/dnxc_rollback_journal_diag.h>
#include <soc/dnxc/dnxc_rollback_journal_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED

#define DNXC_ROLLBACK_JOURNAL_OPT_CODE 1

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
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: invalid journal handle.\n%s%s%s", EMPTY, EMPTY, EMPTY);\
    }\
} while(0)
#else
#define DNXC_ROLLBACK_JOURNAL_VALIDATE_HANDLE(_handle)
#endif /* DNX_ERR_RECOVERY_VALIDATION */

/**
 * \brief - get journal reference based on a handle.
 */
#define DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal_ptr, _handle)   \
do {                                                                  \
    DNXC_ROLLBACK_JOURNAL_VALIDATE_HANDLE((_handle));                 \
    journal_ptr = dnxc_rollback_journal_pool[unit].entries[(_handle)];\
} while(0)

/**
 * \brief - get the size of dnxc_rollback_journal_mem_chunk_t struct
 */
#define DNXC_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE sizeof(dnxc_rollback_journal_mem_chunk_t)

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

#define DNXC_ROLLBACK_JOURNAL_ENTRY_GET_CHUNK_PTR(chunk_ptr, entry_ptr)   \
do {                                                                           \
    chunk_ptr = entry_ptr->mem_chunk; \
} while(0)

/**
 * \brief - get chunk's current allocation pointer
 */
#define DNXC_ROLLBACK_JOURNAL_GET_MEM_CHUNK_DATA_PTR(_type, _data_ptr, _mem_chunk)   \
do {                                                                           \
    _data_ptr = (_type)((uint8 *)_mem_chunk \
    + DNXC_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE); \
} while(0)

/**
 * \brief - get chunk's current allocation pointer
 */
#define DNXC_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(_type, _data_ptr, _mem_chunk)   \
do {                                                                           \
    _data_ptr = (_type)((uint8 *)_mem_chunk \
    + DNXC_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE \
    + _mem_chunk->data_size \
    - _mem_chunk->available_space_at_end); \
} while(0)

/**
 * \brief - get entry's 2nd chunk's current allocation pointer
 */
#define DNXC_ROLLBACK_JOURNAL_ENTRY_GET_NEXT_CHUNK_DATA_PTR(_type, _data_ptr, _entry_ptr)   \
do {                                                     \
    DNXC_ROLLBACK_JOURNAL_GET_MEM_CHUNK_DATA_PTR(   \
        _type, _data_ptr, _entry_ptr->mem_chunk->next); \
} while(0)

/**
 * \brief - check if entry state is 'valid' or 'always valid'
 */
#define DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(_state) \
    ((DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_VALID == (_state)) \
     || (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID == (_state)))

#ifdef DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS
#define DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size)\
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_diag_alloc(_unit, _jrnl_type, _jrnl_subtype, _size))
#else
#define DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size)
#endif

#define DNXC_ROLLBACK_JOURNAL_ENTRY_ALLOC(_unit, _ptr, _type, _size, _metadata_size, _str, _jrnl, _jrnl_type, _jrnl_subtype)\
do {                                                                                           \
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_alloc(_unit, _jrnl, _size, _metadata_size, _ptr));         \
} while(0)

#define DNXC_ROLLBACK_JOURNAL_ALLOC(_unit, _ptr, _type, _size, _str, _jrnl_type, _jrnl_subtype)\
do {                                                                                           \
    (_ptr) = (_type)sal_alloc((_size), (_str));                                                \
    DNXC_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size);          \
} while(0)

/**
 * \brief - insert a mem chunk to the head or tail of the journal mem pool
 */
static shr_error_e
dnxc_rollback_journal_mempool_insert(
    int unit,
    dnxc_rollback_journal_t * journal,
    dnxc_rollback_journal_mem_chunk_t * new_chunk,
    uint32 insert_at_head)
{
    dnxc_rollback_journal_mem_chunk_t *pool_tail = journal->mem_pool.tail;

    SHR_FUNC_INIT_VARS(unit);

    if (!pool_tail)
    {
        journal->mem_pool.head = new_chunk;
        journal->mem_pool.tail = new_chunk;
    }
    else
    {
        if (insert_at_head)
        {
            journal->mem_pool.head->prev = new_chunk;
            new_chunk->next = journal->mem_pool.head;
            journal->mem_pool.head = new_chunk;
        }
        else
        {
            journal->mem_pool.tail->next = new_chunk;
            new_chunk->prev = journal->mem_pool.tail;
            journal->mem_pool.tail = new_chunk;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocates a new mem chunk for journaling
 */
static shr_error_e
dnxc_rollback_journal_mem_chunk_alloc(
    int unit,
    dnxc_rollback_journal_t * journal,
    uint32 required_chunk_size,
    dnxc_rollback_journal_mem_chunk_t ** new_chunk)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((0 != journal->max_size) && (journal->max_size < journal->mem_pool.total_bytes_allocated + required_chunk_size))
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: journal exceeds to maximum size allowed .\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }

    DNXC_ROLLBACK_JOURNAL_ALLOC(unit, *new_chunk, dnxc_rollback_journal_mem_chunk_t *,
                                required_chunk_size, "new rollback journal mem chunk", journal->type, journal->subtype);
    if (NULL == *new_chunk)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "rollback journal ERROR: failed to alloc chunk memory.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    (*new_chunk)->next = NULL;
    (*new_chunk)->prev = NULL;
    (*new_chunk)->data_size = required_chunk_size - DNXC_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE;
    (*new_chunk)->num_allocated_bytes = 0;
    (*new_chunk)->available_space_at_end = required_chunk_size - DNXC_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE;
    journal->mem_pool.total_bytes_allocated += required_chunk_size;
    journal->mem_pool.num_chunks++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -frees a mem_chunk
 */
static shr_error_e
dnxc_rollback_journal_mem_chunk_free(
    int unit,
    dnxc_rollback_journal_t * journal,
    dnxc_rollback_journal_mem_chunk_t * mem_chunk)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Remove this node from the chunk_list
     */

    if (journal->mem_pool.head == mem_chunk)
        journal->mem_pool.head = mem_chunk->next;

    if (journal->mem_pool.tail == mem_chunk)
        journal->mem_pool.tail = mem_chunk->prev;

    if (mem_chunk->prev)
        mem_chunk->prev->next = mem_chunk->next;
    if (mem_chunk->next)
        mem_chunk->next->prev = mem_chunk->prev;

    journal->mem_pool.total_bytes_allocated -= mem_chunk->data_size + DNXC_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE;
    journal->mem_pool.num_chunks--;

    sal_free(mem_chunk);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocates space for an entry from mem chunk.
 *  An entry can spread across two mem chunk.
 */
shr_error_e
dnxc_rollback_journal_entry_alloc(
    int unit,
    dnxc_rollback_journal_t * journal,
    uint32 entry_total_size,
    uint32 entry_metadata_size,
    dnxc_rollback_journal_entry_t ** entry)
{
    dnxc_rollback_journal_mem_chunk_t *pool_tail = NULL;
    dnxc_rollback_journal_mem_chunk_t *new_chunk = NULL;
    uint32 required_chunk_size = DNXC_ROLLBACK_JOURNAL_CHUNK_SIZE;
    uint32 use_a_whole_new_chunk = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set default error value for entry
     */
    *entry = NULL;

    pool_tail = journal->mem_pool.tail;

    if (pool_tail && pool_tail->available_space_at_end >= entry_total_size)
    {
        DNXC_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(dnxc_rollback_journal_entry_t *, *entry, pool_tail);
        (*entry)->mem_chunk = pool_tail;
        (*entry)->split_offset = 0;
        pool_tail->num_allocated_bytes += entry_total_size;
        pool_tail->available_space_at_end -= entry_total_size;
    }
    else
    {
        if (entry_total_size >= DNXC_ROLLBACK_JOURNAL_CHUNK_SIZE - DNXC_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE)
        {
            required_chunk_size = entry_total_size + DNXC_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE;
            use_a_whole_new_chunk = 1;
        }

        SHR_IF_ERR_EXIT(dnxc_rollback_journal_mem_chunk_alloc(unit, journal, required_chunk_size, &new_chunk));

        if (use_a_whole_new_chunk)
        {
            DNXC_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(dnxc_rollback_journal_entry_t *, *entry, new_chunk);
            (*entry)->mem_chunk = new_chunk;
            (*entry)->split_offset = 0;
            new_chunk->num_allocated_bytes += entry_total_size;
            new_chunk->available_space_at_end -= entry_total_size;
            /*
             * Put this chunk at head as we don't have any remaining space in this chunk, so no point putting it in the
             * tail
             */
            SHR_IF_ERR_EXIT(dnxc_rollback_journal_mempool_insert(unit, journal, new_chunk, TRUE));
        }
        else
        {
            /*
             * adjust fields of the pool_tail & set the fields of new_chunk
             */
            if (pool_tail && (sizeof(dnxc_rollback_journal_entry_t) <= pool_tail->available_space_at_end))
            {
                DNXC_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(dnxc_rollback_journal_entry_t *, *entry, pool_tail);
                (*entry)->mem_chunk = pool_tail;
                (*entry)->split_offset = pool_tail->available_space_at_end;
                pool_tail->num_allocated_bytes += pool_tail->available_space_at_end;
                pool_tail->available_space_at_end = 0;
                new_chunk->num_allocated_bytes += entry_total_size - (*entry)->split_offset;
                new_chunk->available_space_at_end -= entry_total_size - (*entry)->split_offset;
            }
            else
            {
                DNXC_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(dnxc_rollback_journal_entry_t *, *entry, new_chunk);
                (*entry)->mem_chunk = new_chunk;
                (*entry)->split_offset = 0;
                new_chunk->num_allocated_bytes += entry_total_size;
                new_chunk->available_space_at_end -= entry_total_size;
            }

            SHR_IF_ERR_EXIT(dnxc_rollback_journal_mempool_insert(unit, journal, new_chunk, FALSE));
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocates space for stamp string from mem chunk.
 */
shr_error_e
dnxc_rollback_journal_stamp_alloc(
    int unit,
    dnxc_rollback_journal_t * journal,
    char *stamp_str,
    uint32 stamp_size)
{
    dnxc_rollback_journal_mem_chunk_t *pool_tail = NULL;
    dnxc_rollback_journal_mem_chunk_t *new_chunk = NULL;
    uint32 required_chunk_size = DNXC_ROLLBACK_JOURNAL_CHUNK_SIZE;
    dnxc_rollback_journal_entry_t *entry;
    uint32 stamp_size_plus_one = stamp_size + 1;
    char *data_ptr;

    SHR_FUNC_INIT_VARS(unit);

    pool_tail = journal->mem_pool.tail;
    if (!(pool_tail && stamp_size_plus_one < pool_tail->available_space_at_end))
    {
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_mem_chunk_alloc(unit, journal, required_chunk_size, &new_chunk));

        SHR_IF_ERR_EXIT(dnxc_rollback_journal_mempool_insert(unit, journal, new_chunk, FALSE));
        pool_tail = journal->mem_pool.tail;
    }

    DNXC_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(char *,
                                                      data_ptr,
                                                      pool_tail);

    sal_memcpy(data_ptr, stamp_str, stamp_size);
    data_ptr[stamp_size] = 0;
    pool_tail->available_space_at_end -= stamp_size_plus_one;
    pool_tail->num_allocated_bytes += stamp_size_plus_one;

    entry = journal->tail;
    while (entry != NULL && entry->stamp == NULL)
    {
        entry->stamp = data_ptr;
        entry->stamp_mem_chunk = pool_tail;
        entry->stamp_size = 0;
        entry = entry->prev;
    }
    journal->tail->stamp_size = stamp_size_plus_one;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

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
            DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                         "rollback journal ERROR: journal state is inconsistent on entry push.\n%s%s%s",
                                                         EMPTY, EMPTY, EMPTY);
        }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

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
 * \brief - frees the space allocated for a stamp string
 */
shr_error_e
dnxc_rollback_journal_stamp_str_free(
    int unit,
    dnxc_rollback_journal_t * journal,
    char *stamp,
    dnxc_rollback_journal_mem_chunk_t * mem_chunk,
    uint32 stamp_size)
{
    SHR_FUNC_INIT_VARS(unit);

    mem_chunk->num_allocated_bytes -= stamp_size;

    if (mem_chunk->num_allocated_bytes < 0)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: mem_chunk allocation size became negetive.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }

    if (mem_chunk->num_allocated_bytes == 0)
    {
        /*
         * Remove this node from the chunk_list
         */
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_mem_chunk_free(unit, journal, mem_chunk));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - frees the space allocated for an entry
 */
shr_error_e
dnxc_rollback_journal_entry_free(
    int unit,
    dnxc_rollback_journal_t * journal,
    dnxc_rollback_journal_entry_t * entry)
{
    uint32 total_entry_size;
    dnxc_rollback_journal_mem_chunk_t *mem_chunk = NULL;
    dnxc_rollback_journal_mem_chunk_t *mem_chunk_2nd = NULL;

    SHR_FUNC_INIT_VARS(unit);

    total_entry_size = sizeof(dnxc_rollback_journal_entry_t) + entry->metadata_size + entry->payload_size;

    DNXC_ROLLBACK_JOURNAL_ENTRY_GET_CHUNK_PTR(mem_chunk, entry);
    if (!entry->split_offset)
    {
        mem_chunk->num_allocated_bytes -= total_entry_size;
    }
    else
    {
        mem_chunk_2nd = mem_chunk->next;
        mem_chunk->num_allocated_bytes -= entry->split_offset;
        mem_chunk_2nd->num_allocated_bytes -= total_entry_size - entry->split_offset;

        if (mem_chunk_2nd->num_allocated_bytes < 0)
        {
            DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                         "rollback journal ERROR: mem_chunk_2nd allocation size became negetive.\n%s%s%s",
                                                         EMPTY, EMPTY, EMPTY);
        }

        if (mem_chunk_2nd->num_allocated_bytes == 0)
        {
            /*
             * Remove this node from the chunk_list
             */
            SHR_IF_ERR_EXIT(dnxc_rollback_journal_mem_chunk_free(unit, journal, mem_chunk_2nd));
        }
    }
    if (mem_chunk->num_allocated_bytes < 0)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: mem_chunk allocation size became negetive.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
    if (mem_chunk->num_allocated_bytes == 0)
    {
        /*
         * Remove this node from the chunk_list
         */
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_mem_chunk_free(unit, journal, mem_chunk));
    }

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
#ifdef DNXC_ROLLBACK_JOURNAL_OPT_CODE
        if (entry->stamp_size)
        {
            SHR_IF_ERR_EXIT(dnxc_rollback_journal_stamp_str_free(unit,
                                                                 journal, entry->stamp, entry->stamp_mem_chunk,
                                                                 entry->stamp_size));
        }
#else
        sal_free(entry->stamp);
#endif
        entry->stamp = NULL;
    }

    /*
     * free the disconnected head
     */
#ifdef DNXC_ROLLBACK_JOURNAL_OPT_CODE
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_free(unit, journal, entry));
#else
    sal_free(entry);
#endif

    entry = NULL;

    journal->entry_counter--;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * sanity check counter if last entry
     */
    if (((NULL == journal->head) || (NULL == journal->tail)) && (0 != journal->entry_counter))
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "sw state journal ERROR: invalid entry found in journal.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * reset journal head and tail pointer
     */
    if (0 == journal->entry_counter)
    {
        journal->head = NULL;
        journal->tail = NULL;

        if (journal->mem_pool.total_bytes_allocated != 0 ||
            journal->mem_pool.num_chunks != 0 || journal->mem_pool.head || journal->mem_pool.tail)
        {
            DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                         "rollback journal ERROR: all journal entries freed but the mem_pool is not free yet.\n%s%s%s",
                                                         EMPTY, EMPTY, EMPTY);
        }
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

static shr_error_e
dnxc_rollback_journal_entry_get_merged_metadata_payload_ptr(
    int unit,
    dnxc_rollback_journal_t * journal,
    dnxc_rollback_journal_entry_t * entry,
    uint8 **metadata_ptr,
    uint8 **payload_ptr,
    uint32 *should_release_metadata,
    uint32 *should_release_payload)
{
    uint8 *metadata = NULL;
    uint8 *payload = NULL;
    uint8 *metadata_new = NULL;
    uint8 *payload_new = NULL;
    uint8 *next_chunk_data_ptr = NULL;
    uint32 part1_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    *should_release_metadata = 0;
    *should_release_payload = 0;

    if (!entry->split_offset)
    {
        /*
         * entry not splitted into two chunks
         */
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload, entry);
    }
    else
    {
        /*
         * entry splitted into two chunks
         */
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_NEXT_CHUNK_DATA_PTR(uint8 *,
                                                            next_chunk_data_ptr,
                                                            entry);
        if (entry->split_offset < sizeof(dnxc_rollback_journal_entry_t) + entry->metadata_size)
        {
            /*
             * metadata split case
             */

            if (entry->split_offset == sizeof(dnxc_rollback_journal_entry_t))
            {
                /*
                 * metadata is entirely into 2nd chunk
                 */
                metadata = next_chunk_data_ptr;
                payload = next_chunk_data_ptr + entry->metadata_size;
            }
            else
            {
                /*
                 * metadata is in both chunk chunk
                 */
                part1_size = entry->split_offset - sizeof(dnxc_rollback_journal_entry_t);
                DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
                DNXC_ROLLBACK_JOURNAL_ALLOC(unit, metadata_new, uint8 *,
                                            entry->metadata_size,
                                            "space to merge metadata",
                                            journal->type,
                                            journal->subtype);
                if (NULL == metadata_new)
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                             "rollback journal ERROR: cannot allocate memory to merge meatadata.\n%s%s%s",
                                             EMPTY, EMPTY, EMPTY);
                }
                sal_memcpy(metadata_new, metadata, part1_size);
                sal_memcpy(metadata_new + part1_size, next_chunk_data_ptr, entry->metadata_size - part1_size);
            }
            if (entry->payload_size > 0)
            {
                payload = next_chunk_data_ptr + entry->metadata_size - part1_size;
            }
        }
        else
        {
            /*
             * payload split case
             */
            DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
            if (entry->split_offset == (sizeof(dnxc_rollback_journal_entry_t) + entry->metadata_size))
            {
                /*
                 * payload is entirely into 2nd chunk
                 */
                payload = next_chunk_data_ptr;
            }
            else
            {
                /*
                 * payload is in both chunk chunk
                 */
                part1_size = entry->split_offset - sizeof(dnxc_rollback_journal_entry_t) - entry->metadata_size;
                DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload, entry);
                DNXC_ROLLBACK_JOURNAL_ALLOC(unit, payload_new, uint8 *,
                                            entry->payload_size,
                                            "space to merge payload",
                                            journal->type,
                                            journal->subtype);
                if (NULL == payload_new)
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                             "rollback journal ERROR: cannot allocate memory to merge payload.\n%s%s%s",
                                             EMPTY, EMPTY, EMPTY);
                }
                sal_memcpy(payload_new, payload, part1_size);
                sal_memcpy(payload_new + part1_size, next_chunk_data_ptr, entry->payload_size - part1_size);
            }
        }
    }

    if (metadata_new)
    {
        *metadata_ptr = metadata_new;
        *should_release_metadata = 1;
    }
    else
    {
        *metadata_ptr = metadata;
        *should_release_metadata = 0;
    }

    if (payload_new)
    {
        *payload_ptr = payload_new;
        *should_release_payload = 1;
    }
    else
    {
        *payload_ptr = payload;
        *should_release_payload = 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - traverse all remaining entries down the stack, call callback for each of them if defined
 */
static shr_error_e
dnxc_rollback_journal_entry_rollback_traverse(
    int unit,
    dnxc_rollback_journal_t * journal,
    dnxc_rollback_journal_entry_t * top)
{
    uint8 *metadata = NULL;
    uint8 *payload = NULL;
    uint8 *metadata_traversed = NULL;
    uint8 *payload_traversed = NULL;

    uint32 should_release_metadata = 0;
    uint32 should_release_payload = 0;
    uint32 should_release_metadata_traversed = 0;
    uint32 should_release_payload_traversed = 0;

    dnxc_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get metadata nad payload of the top entry
     */

    SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, top,
                                                                                &metadata, &payload,
                                                                                &should_release_metadata,
                                                                                &should_release_payload));

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
            SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                        &metadata_traversed,
                                                                                        &payload_traversed,
                                                                                        &should_release_metadata_traversed,
                                                                                        &should_release_payload_traversed));

            SHR_IF_ERR_EXIT(top->rollback_traverse_handler
                            (unit, metadata, payload, metadata_traversed, payload_traversed));
            if (should_release_metadata_traversed)
                sal_free(metadata_traversed);
            if (should_release_payload_traversed)
                sal_free(payload_traversed);
        }

        entry = entry->next;
    }

    if (should_release_metadata)
        sal_free(metadata);
    if (should_release_payload)
        sal_free(payload);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
uint8
dnxc_rollback_journal_is_done_init(
    int unit)
{
    return (SOC_IS_DONE_INIT(unit) || (dnxc_rollback_journal_pool[unit].is_init_restrict_bypassed_counter > 0));
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_is_init_check_bypass(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_on)
    {
        dnxc_rollback_journal_pool[unit].is_init_restrict_bypassed_counter++;
    }
    else
    {
        if (0 == dnxc_rollback_journal_pool[unit].is_init_restrict_bypassed_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: Init check bypass counter cannot be less than 0.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        dnxc_rollback_journal_pool[unit].is_init_restrict_bypassed_counter--;
    }

    SHR_EXIT();
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
    uint8 *next_chunk_data_ptr = NULL;
    uint32 part1_size = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * validate input
     */
    if ((NULL == metadata))
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: invalid parameter.\n%s%s%s", EMPTY, EMPTY,
                                                     EMPTY);
    }

    if (NULL == rollback_handler)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: rollback handler cannot be null.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

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
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: journal exceeds to maximum size allowed .\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * allocate the entry and payload together
     */

#ifdef DNXC_ROLLBACK_JOURNAL_OPT_CODE
    DNXC_ROLLBACK_JOURNAL_ENTRY_ALLOC(unit, &entry, dnxc_rollback_journal_entry_t *, total_size, metadata_size,
                                      "new rollback journal entry", journal, journal->type, journal->subtype);
#else
    DNXC_ROLLBACK_JOURNAL_ALLOC(unit, entry, dnxc_rollback_journal_entry_t *, total_size, "new rollback journal entry",
                                journal->type, journal->subtype);
#endif

    if (NULL == entry)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "rollback journal ERROR: failed to alloc entry's data.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

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
    entry->stamp_size = 0;
#ifndef DNXC_ROLLBACK_JOURNAL_OPT_CODE
    /*
     * set the below field to zero only for old code,
     * dont set it for new code
     */
    entry->split_offset = 0;
#endif

    DNXC_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(entry_metadata, entry);
    DNXC_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(entry_payload, entry);

    if (!entry->split_offset)
    {
        sal_memcpy(entry_metadata, metadata, metadata_size);
        if (payload_size > 0)
        {
            sal_memcpy(entry_payload, payload, payload_size);
        }
    }
    else
    {
        DNXC_ROLLBACK_JOURNAL_ENTRY_GET_NEXT_CHUNK_DATA_PTR(uint8 *,
                                                            next_chunk_data_ptr,
                                                            entry);
        if (entry->split_offset < sizeof(dnxc_rollback_journal_entry_t) + entry->metadata_size)
        {
            /*
             * metadata split case
             */
            if (entry->split_offset > sizeof(dnxc_rollback_journal_entry_t))
            {
                /*
                 * metadata to be put into 1st chunk. copy metadata part1
                 */
                part1_size = entry->split_offset - sizeof(dnxc_rollback_journal_entry_t);
                sal_memcpy(entry_metadata, metadata, part1_size);
            }
            /*
             * copy metadata part2 into 2nd chunk
             */
            sal_memcpy(next_chunk_data_ptr, metadata + part1_size, metadata_size - part1_size);
            if (payload_size > 0)
            {
                sal_memcpy(next_chunk_data_ptr + metadata_size - part1_size, payload, payload_size);
            }
        }
        else
        {
            /*
             * payload split case
             */
            sal_memcpy(entry_metadata, metadata, metadata_size);

            if (entry->split_offset > (sizeof(dnxc_rollback_journal_entry_t) + metadata_size))
            {
                /*
                 * payload to be put into 1st chunk. copy the payload part1
                 */
                part1_size = entry->split_offset - sizeof(dnxc_rollback_journal_entry_t) - metadata_size;
                sal_memcpy(entry_payload, payload, part1_size);
            }
            /*
             * copy payload part2 into 2nd chunk
             */
            sal_memcpy(next_chunk_data_ptr, payload + part1_size, payload_size - part1_size);
        }
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

    /*
     * print entry if the logger is on
     */
    if (journal->is_logging && print_handler != NULL)
    {
        SHR_IF_ERR_EXIT(print_handler(unit, metadata, payload));
    }

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
    journal->is_logging = FALSE;
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
    dnxc_rollback_journal_handle_t handle,
    uint8 is_unsafe)
{
    dnxc_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * modify the low level journal suppression only if the journal is on
     */
    if (NULL == dnxc_rollback_journal_pool[unit].entries[handle])
    {
        SHR_EXIT();
    }

    if (!is_unsafe
        && (NULL == dnxc_rollback_journal_pool[unit].entries[handle]->cbs.is_on
            || !dnxc_rollback_journal_pool[unit].entries[handle]->cbs.is_on(unit)))
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
dnxc_rollback_journal_disabled_counter_get(
    int unit,
    dnxc_rollback_journal_handle_t handle,
    uint32 *counter)
{
    dnxc_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * modify the low level journal suppression only if the journal is on
     */
    if (NULL == dnxc_rollback_journal_pool[unit].entries[handle])
    {
        SHR_EXIT();
    }

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    *counter = journal->is_journaled_disabled_counter;

    SHR_EXIT();
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
    uint32 should_release_metadata = 0;
    uint32 should_release_payload = 0;

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
            DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                         "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                                         EMPTY, EMPTY, EMPTY);
        }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

        /*
         * Calculate the metadata and payload locations
         */
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                    &metadata, &payload,
                                                                                    &should_release_metadata,
                                                                                    &should_release_payload));

#ifdef DNX_ERR_RECOVERY_VALIDATION
        /*
         * Client handles the entry rollback. Must exist.
         */
        if (NULL == entry->rollback_handler)
        {
            DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                         "rollback journal ERROR: entry's rollback handler cannot be null.\n%s%s%s",
                                                         EMPTY, EMPTY, EMPTY);
        }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

        if (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            rv_rollback = entry->rollback_handler(unit, metadata, payload, entry->stamp);
        }

        if (should_release_metadata)
            sal_free(metadata);
        if (should_release_payload)
            sal_free(payload);

        /*
         * Call traverse callback for each remaining entry if callback is set. Optinal.
         */
        if ((NULL != entry->rollback_traverse_handler)
            && (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state)) && (_SHR_E_NONE == rv_rollback))
        {
            rv_rollback = dnxc_rollback_journal_entry_rollback_traverse(unit, journal, entry);
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
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: journal entry count zero but size is not zero.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

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
#ifdef DNXC_ROLLBACK_JOURNAL_OPT_CODE
    dnxc_rollback_journal_mem_chunk_t *mem_chunk = NULL;
    dnxc_rollback_journal_mem_chunk_t *next_mem_chunk = NULL;
#endif
    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (0 != journal->is_journaled_disabled_counter)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: journal cannot be cleared when suppression is active.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

#ifdef DNXC_ROLLBACK_JOURNAL_OPT_CODE
    if (only_head_clear)
    {
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_remove(unit, FALSE, journal));
    }
    else
    {
        /*
         * There is no need to free all the entries of the journal queue. Just freeing the mem pool is enough
         */

        /*
         * traverse the mem_chunk list and free them all
         */
        mem_chunk = journal->mem_pool.head;
        while (mem_chunk)
        {
            next_mem_chunk = mem_chunk->next;
            sal_free(mem_chunk);
            mem_chunk = next_mem_chunk;
        }
    }
#else
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
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: journal cleared but entries or size are non zero.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */
#endif /* DNXC_ROLLBACK_JOURNAL_OPT_CODE */
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
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_PARAM,
                                                     "rollback journal ERROR: sequence id parameter cannot be null.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

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
            DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                         "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                                         EMPTY, EMPTY, EMPTY);
        }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

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
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: Cannot invalidate last inserted entry. No entries found in journal.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * mark deque's head as invalid
     */
    dnxc_rollback_journal_entry_examine(unit, journal, &entry, FALSE);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == entry)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

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

    uint32 should_release_metadata = 0;
    uint32 should_release_payload = 0;
    uint32 should_release_trav_metadata = 0;
    uint32 should_release_trav_payload = 0;

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
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                    &metadata, &payload,
                                                                                    &should_release_metadata,
                                                                                    &should_release_payload));

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
            SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                        &trav_metadata, &trav_payload,
                                                                                        &should_release_trav_metadata,
                                                                                        &should_release_trav_payload));
            SHR_IF_ERR_EXIT(is_distinct_cb(unit, metadata, payload, trav_metadata, trav_payload, &are_distinct));
            if (should_release_trav_metadata)
                sal_free(trav_metadata);
            if (should_release_trav_payload)
                sal_free(trav_payload);

            if (!are_distinct && (DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID != trav_entry->state))
            {
                trav_entry->state = DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
            }

            trav_entry = trav_entry->next;
        }

        if (should_release_metadata)
            sal_free(metadata);
        if (should_release_payload)
            sal_free(payload);

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
    dnxc_rollback_journal_t *journal = NULL;
    uint32 size = 0;
#ifndef DNXC_ROLLBACK_JOURNAL_OPT_CODE
    dnxc_rollback_journal_entry_t *entry = NULL;
#endif

    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (!journal->tail || journal->tail->stamp)
    {
        SHR_EXIT();
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == str)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: stamp string cannot be null.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    size = sal_strlen(str);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (size > DNXC_ROLLBACK_JOURNAL_MAX_STAMP_SIZE)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: stamp size exceeds the maximum allowed.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

#ifdef DNXC_ROLLBACK_JOURNAL_OPT_CODE
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_stamp_alloc(unit, journal, str, size));
#else
    entry = journal->tail;
    while (entry != NULL && entry->stamp == NULL)
    {
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

        entry = entry->prev;
    }
#endif

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
#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnxc_rollback_journal_entry_t *last_entry = NULL;
#endif

    uint8 *metadata = NULL;
    uint8 *payload = NULL;
    uint32 should_release_metadata = 0;
    uint32 should_release_payload = 0;

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
#ifdef DNX_ERR_RECOVERY_VALIDATION
        last_entry = entry;
#endif

        /*
         * Calculate the metadata and payload locations
         */
        SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                    &metadata, &payload,
                                                                                    &should_release_metadata,
                                                                                    &should_release_payload));

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

        if (should_release_metadata)
            sal_free(metadata);
        if (should_release_payload)
            sal_free(payload);

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
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                     "rollback journal ERROR: last non-null entry in journal is difference than the journal tail.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

/*
 * see .h file for description
 */
shr_error_e
dnxc_rollback_journal_logger_state_change(
    int unit,
    dnxc_rollback_journal_handle_t handle)
{
    dnxc_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    journal->is_logging = !(journal->is_logging);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

#undef _ERR_MSG_MODULE_NAME
