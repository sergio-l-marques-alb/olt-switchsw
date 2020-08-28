/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is a generic side effect / state rollback journal,
 * saving the old values of global state or side effects that were overridden
 * by access calls since the beginning of the last transaction.
 * This journal should be used only in places where the existing DBAL or SWSTATE journal fail short.
 */

#include <assert.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <soc/dnxc/dnxc_rollback_journal.h>
#include <soc/dnxc/dnxc_rollback_journal_utils.h>
#include <soc/dnxc/dnxc_generic_state_journal.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
static dnxc_generic_state_journal_manager_t dnxc_generic_state_journal_manager[SOC_MAX_NUM_DEVICES] = { {0} };

#define DNX_DBAL_ROLLBACK_JOURNAL_NUM 0
#define DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE dnxc_generic_state_journal_manager[unit].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].handle

/**
 * \brief - generic state function definitions
 */
#define DNXC_GENERIC_STATE_JOURNAL_DEFS\
    SHR_FUNC_INIT_VARS(unit);\

/**
 * \brief - generic state function returns
 */
#define DNXC_GENERIC_STATE_FUNC_RETURN\
    SHR_EXIT();\
exit:\
    SHR_FUNC_EXIT

#define DNXC_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit)                                  \
do {                                                                          \
    if(!dnxc_rollback_journal_is_on(unit, DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE))\
    {                                                                         \
        SHR_EXIT();                                                           \
    }                                                                         \
} while(0)

/**
 * \brief - roll-back a 'memcpy' entry
 */
static shr_error_e
dnxc_generic_state_journal_rollback_memcpy_entry(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnxc_generic_state_journal_entry_t *entry = (dnxc_generic_state_journal_entry_t *) (metadata);

    DNXC_GENERIC_STATE_JOURNAL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if ((NULL == entry->ptr) || (NULL == payload))
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                     "state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                                     EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    sal_memcpy(entry->ptr, payload, entry->size);

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_entry_new(
    int unit,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnxc_generic_state_journal_rollback_handler rollback_handler)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_rollback_journal_entry_new(unit,
                                                    DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE,
                                                    metadata,
                                                    metadata_size,
                                                    payload,
                                                    payload_size,
                                                    DNXC_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
                                                    rollback_handler, NULL, NULL));
    DNXC_GENERIC_STATE_FUNC_RETURN;
}

#ifdef BCM_DNX_SUPPORT
/**
 * \brief - roll-back a 'dbal table create' entry
 */
static shr_error_e
dnxc_generic_state_journal_rollback_dbal_table_create(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnxc_generic_state_journal_dbal_table_entry_t *entry = (dnxc_generic_state_journal_dbal_table_entry_t *) (metadata);

    DNXC_GENERIC_STATE_JOURNAL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == metadata)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                     "state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                                     EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, entry->table_id));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_generic_state_journal_log_dbal_table_create_internal(
    int unit,
    dbal_tables_e table_id)
{
    dnxc_generic_state_journal_dbal_table_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.table_id = table_id;

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_entry_new(unit,
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_dbal_table_entry_t),
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_dbal_table_entry_t),
                                                         &dnxc_generic_state_journal_rollback_dbal_table_create));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_log_dbal_table_create(
    int unit,
    dbal_tables_e table_id)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    DNXC_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_dbal_table_create_internal(unit, table_id));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back a 'dbal table destroy' entry
 */
static shr_error_e
dnxc_generic_state_journal_rollback_dbal_table_info_change(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnxc_generic_state_journal_dbal_table_entry_t *entry = (dnxc_generic_state_journal_dbal_table_entry_t *) (metadata);

    DNXC_GENERIC_STATE_JOURNAL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == metadata)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                     "state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                                     EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * restore table state from swstate
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_restore(unit, entry->table_id));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_generic_state_journal_log_dbal_table_info_change_internal(
    int unit,
    dbal_tables_e table_id)
{
    dnxc_generic_state_journal_dbal_table_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Only supported for dynamic tables.
     */
    if (table_id < DBAL_NOF_TABLES)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                     "state journal ERROR: Attempted to log change to non dynamic DBAL table %d.\n%s%s",
                                                     table_id, EMPTY, EMPTY);
    }

    entry.table_id = table_id;

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_entry_new(unit,
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_dbal_table_entry_t),
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_dbal_table_entry_t),
                                                         &dnxc_generic_state_journal_rollback_dbal_table_info_change));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_log_dbal_table_info_change(
    int unit,
    dbal_tables_e table_id)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    DNXC_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_dbal_table_info_change_internal(unit, table_id));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back a 'dbal result type add' entry
 */
static shr_error_e
dnxc_generic_state_journal_rollback_result_type_add(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnxc_generic_state_journal_dbal_table_entry_t *entry = (dnxc_generic_state_journal_dbal_table_entry_t *) (metadata);

    DNXC_GENERIC_STATE_JOURNAL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == metadata)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif

    /*
     * restore table state from swstate
     */
    SHR_IF_ERR_EXIT(dbal_tables_dynamic_result_type_delete(unit, entry->table_id, entry->res_type_idx));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
shr_error_e
dnxc_generic_state_journal_log_result_type_add_internal(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx)
{
    dnxc_generic_state_journal_dbal_table_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.table_id = table_id;
    entry.res_type_idx = res_type_idx;

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_entry_new(unit,
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_dbal_table_entry_t),
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_dbal_table_entry_t),
                                                         &dnxc_generic_state_journal_rollback_result_type_add));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_generic_state_journal_log_result_type_add(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    DNXC_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_result_type_add_internal(unit, table_id, res_type_idx));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back a 'dbal result type delete' entry
 */
static shr_error_e
dnxc_generic_state_journal_rollback_result_type_delete(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnxc_generic_state_journal_dbal_table_entry_t *entry = (dnxc_generic_state_journal_dbal_table_entry_t *) (metadata);

    DNXC_GENERIC_STATE_JOURNAL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == metadata)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif

    /*
     * restore table state from swstate
     */
    SHR_IF_ERR_EXIT(dbal_tables_result_type_restore_rollback(unit, entry->table_id));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
shr_error_e
dnxc_generic_state_journal_log_result_type_delete_internal(
    int unit,
    dbal_tables_e table_id)
{
    dnxc_generic_state_journal_dbal_table_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.table_id = table_id;

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_entry_new(unit,
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_dbal_table_entry_t),
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_dbal_table_entry_t),
                                                         &dnxc_generic_state_journal_rollback_result_type_delete));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_generic_state_journal_log_result_type_delete(
    int unit,
    dbal_tables_e table_id)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    DNXC_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_result_type_delete_internal(unit, table_id));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back a 'free' entry
 */
static shr_error_e
dnxc_generic_state_journal_rollback_free_entry(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnxc_generic_state_journal_free_entry_t *entry = (dnxc_generic_state_journal_free_entry_t *) metadata;
    uint32 data_size = entry->nof_elements * entry->element_size;

    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    /*
     * allocate the data
     */
    SHR_ALLOC_SET_ZERO(*(entry->ptr_location), data_size, "error recovery free entry rollback", "%s%s%s\r\n", EMPTY,
                       EMPTY, EMPTY);

    /*
     * restore state
     */
    sal_memcpy(*(entry->ptr_location), payload, data_size);

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_generic_state_journal_log_free_internal(
    int unit,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr_location)
{
    dnxc_generic_state_journal_free_entry_t entry;
    uint32 data_size = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * validate input
     */
    if (NULL == ptr_location)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                     "state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    if (NULL == *ptr_location)
    {
        SHR_EXIT();
    }

    entry.nof_elements = nof_elements;
    entry.element_size = element_size;
    entry.location = *ptr_location;
    entry.ptr_location = ptr_location;

    data_size = entry.nof_elements * entry.element_size;

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_entry_new(unit,
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_free_entry_t),
                                                         *ptr_location,
                                                         data_size, &dnxc_generic_state_journal_rollback_free_entry));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_log_free(
    int unit,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr_location)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    DNXC_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_free_internal(unit, nof_elements, element_size, ptr_location));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

#endif

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_generic_state_journal_log_memcpy_internal(
    int unit,
    uint32 size,
    uint8 *ptr)
{
    dnxc_generic_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == ptr)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                     "state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }

    if (0 == size)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                     "state journal ERROR: data with size zero attempted to be inserted to journal.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    entry.size = size;
    entry.ptr = ptr;

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_entry_new(unit,
                                                         (uint8 *) (&entry),
                                                         sizeof(dnxc_generic_state_journal_entry_t),
                                                         ptr, size, &dnxc_generic_state_journal_rollback_memcpy_entry));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_log_memcpy(
    int unit,
    uint32 size,
    uint8 *ptr)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    DNXC_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_memcpy_internal(unit, size, ptr));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

static inline shr_error_e
dnxc_generic_state_journal_clear_internal(
    int unit,
    uint8 only_head_clear)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_rollback_journal_clear(unit, only_head_clear, DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_clear(
    int unit,
    uint8 only_head_clear)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_clear_internal(unit, only_head_clear));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_initialize(
    int unit,
    dnxc_rollback_journal_cbs_t rollback_journal_cbs)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == rollback_journal_cbs.is_on)
    {
        DNXC_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                     "state journal ERROR: journal is on callback must be set.\n%s%s%s",
                                                     EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    SHR_IF_ERR_EXIT(dnxc_rollback_journal_new(unit,
                                              FALSE,
                                              DNXC_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                              DNXC_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE,
                                              rollback_journal_cbs, &(DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE)));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_destroy(
    int unit,
    uint8 is_comparison)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_rollback_journal_destroy(unit, &(DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE)));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_get_head_seq_id(
    int unit,
    uint32 *seq_id)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_rollback_journal_get_head_seq_id(unit, seq_id, DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_all_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    /*
     * currently there is only a state rollback journal, no comparison journal avaiable
     */
    SHR_IF_ERR_EXIT(dnxc_rollback_journal_disabled_counter_change
                    (unit, is_disable, DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE, TRUE));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}

static inline shr_error_e
dnxc_generic_state_journal_roll_back_internal(
    int unit,
    uint8 only_head_rollback)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_rollback_journal_rollback
                    (unit, only_head_rollback, DNXC_GENERIC_STATE_ROLLBACK_JOURNAL_HANDLE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_generic_state_journal_roll_back(
    int unit,
    uint8 only_head_rollback)
{
    DNXC_GENERIC_STATE_JOURNAL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_roll_back_internal(unit, only_head_rollback));

    DNXC_GENERIC_STATE_FUNC_RETURN;
}
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

#undef _ERR_MSG_MODULE_NAME
