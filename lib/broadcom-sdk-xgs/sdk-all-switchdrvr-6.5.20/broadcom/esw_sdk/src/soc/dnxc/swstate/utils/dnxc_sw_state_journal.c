/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by sw_state access calls since the beginning of the last transaction.
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#endif /* BCM_DNX_SUPPORT */
#include <soc/drv.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * "0xFF ", chars per element for print + null termination
 */
#define DNXC_SW_STATE_JOURNAL_BYTE_HEX_LEN 6

/*
 * "<last element repeats 0000 times>", 34 chars per element for print + null termination
 */
#define DNXC_SW_STATE_JOURNAL_LAST_ELEMENT_REPEAT_LEN 34

/**
 * \brief - sw state function definitions
 */
#define DNXC_SW_STATE_JRNL_DEFS\
    SHR_FUNC_INIT_VARS(unit);\

/**
 * \brief - sw state function returns
 */
#define DNXC_SW_STATE_JRNL_FUNC_RETURN\
    SHR_EXIT();\
exit:\
    SHR_FUNC_EXIT

/**
 * \brief
 *  Recalcuate entry single-pointer addresses, affected by 'free' entry roll-back.
 * \remark
 *  Update address sequence:
 *  - Check if pointer is in range [old_address ;old_address + size] \n
 *  - Update the address to the new location \n
 *  Valid only for 'memcpy' entry type
 */
#define DNXC_SW_STATE_JOURNAL_UPDATE_ENTRY_ADDRESS(ptr, old_address, new_address, size) \
    do{ \
        if(((ptr) - (old_address) >= 0) && ((old_address) + (size) - (ptr) > 0)) { \
            (ptr) = (new_address) + ((ptr) - (old_address)); \
        } \
    } \
    while(0)

/**
 * \brief
 *  Recalculate entry double-pointer addresses, if affected by 'free' entry roll-back.
 * \remark
 *  Update address sequence:
 *  - Check if double-pointer is in range [old_address ;old_address + size] \n
 *  - Update the address to the new location \n
 *  Double pointer is cast to a single pointer because of pointer arithmetics. \n
 *  Resulting new address after arithmetics is cast to double pointer. \n
 *  Valid only for 'alloc' and 'free' entry types
 */
#define DNXC_SW_STATE_JOURNAL_UPDATE_ENTRY_PTR_TO_ADDRESS(ptr, old_address, new_address, size) \
    do{ \
        if(((uint8 *)(ptr) - (uint8 *)(old_address) >= 0) \
        && ((uint8 *)(old_address) + (size) - (uint8 *)(ptr) > 0)) { \
            (ptr) = (uint8 **)((uint8 *)(new_address) + ((uint8 *)(ptr) - (uint8 *)(old_address))); \
        } \
    } \
    while(0)

#define DNXC_SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY(mutex, old_mutex, new_mutex)\
    do{ \
        if(mutex == old_mutex) {\
            mutex = new_mutex;\
        }\
    } \
    while(0)

#define DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(_unit, _is_er, _is_cmp)                          \
do {                                                                                      \
    _is_er = dnx_rollback_journal_is_on(_unit, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE);   \
    _is_cmp = dnx_rollback_journal_is_on(_unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE);\
    if((!_is_er) && (!_is_cmp))                                                           \
    {                                                                                     \
        SHR_EXIT();                                                                       \
    }                                                                                     \
} while(0)

/*
 * \brief - shorten the pointer calc function calls
 */
#define DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, offset)\
    dnxc_sw_state_dispatcher[unit][((dnxc_sw_state_journal_entry_t *)(entry))->module_id].calc_pointer(unit, ((dnxc_sw_state_journal_entry_t *)(entry))->offset)

/*
 * \brief - shorten the double pointer calc function calls
 */
#define DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, entry, offset)\
    dnxc_sw_state_dispatcher[unit][((dnxc_sw_state_journal_entry_t *)(entry))->module_id].calc_dpointer(unit, ((dnxc_sw_state_journal_entry_t *)(entry))->offset)
/*
 * \brief - shorten the offset calc function calls
 */
#define DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, entry, pointer)\
    dnxc_sw_state_dispatcher[unit][entry->module_id].calc_offset(unit, pointer)

/*
 * \brief - shorten the offset calc from double pointer function calls
 */
#define DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET_FROM_DPTR(unit, entry, pointer)\
    dnxc_sw_state_dispatcher[unit][entry->module_id].calc_offset_from_dptr(unit, pointer)

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
/*
 * \brief - notify the managers when memcpy / htbl entry is attempted to be inserted in journal.
 */
#define DNXC_SWSTATE_JOURNAL_ACCESS_NOTIFY(_unit)                                                        \
    do {                                                                                                 \
        if(NULL != dnxc_sw_state_manager[_unit].journals[DNX_SWSTATE_COMPARISON_JOURNAL_NUM].access_cb) {\
            dnxc_sw_state_manager[_unit].journals[DNX_SWSTATE_COMPARISON_JOURNAL_NUM].access_cb(_unit);  \
        }                                                                                                \
        if(NULL != dnxc_sw_state_manager[_unit].journals[DNX_SWSTATE_ROLLBACK_JOURNAL_NUM].access_cb) {  \
            dnxc_sw_state_manager[_unit].journals[DNX_SWSTATE_ROLLBACK_JOURNAL_NUM].access_cb(_unit);    \
        }                                                                                                \
    } while(0)
#else
#define DNXC_SWSTATE_JOURNAL_ACCESS_NOTIFY(_unit)
#endif

#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS
#define DNXC_SWSTATE_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size)\
    SHR_IF_ERR_EXIT(dnx_rollback_journal_diag_alloc(_unit, _jrnl_type, _jrnl_subtype, _size))
#else
#define DNXC_SWSTATE_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size)
#endif

#define DNXC_SWSTATE_JOURNAL_ALLOC(_unit, _ptr, _type, _size, _str, _jrnl_type, _jrnl_subtype)\
do {                                                                                          \
    (_ptr) = (_type)sal_alloc((_size), (_str));                                               \
    DNXC_SWSTATE_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size);          \
} while(0)

#define DNX_SW_STATE_JOURNAL_MUTEX_DEFAULT_USEC sal_mutex_FOREVER
#define DNX_SW_STATE_JOURNAL_SEM_DEFAULT_USER sal_sem_FOREVER

static dnxc_sw_state_journal_manager_t dnxc_sw_state_manager[SOC_MAX_NUM_DEVICES] = {{0}};

#define DNX_SWSTATE_ROLLBACK_JOURNAL_NUM 0
#define DNX_SWSTATE_COMPARISON_JOURNAL_NUM 1

#define DNXC_SW_STATE_ROLLBACK_JOURNAL_TRANSACTION dnxc_sw_state_manager[unit].journals[DNX_SWSTATE_ROLLBACK_JOURNAL_NUM].transaction
#define DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION dnxc_sw_state_manager[unit].journals[DNX_SWSTATE_COMPARISON_JOURNAL_NUM].transaction

#define DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE dnxc_sw_state_manager[unit].journals[DNX_SWSTATE_ROLLBACK_JOURNAL_NUM].handle
#define DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE dnxc_sw_state_manager[unit].journals[DNX_SWSTATE_COMPARISON_JOURNAL_NUM].handle

/*
 * \brief - check if two MEMCPY entries are distinct
 */
static shr_error_e
dnxc_sw_state_journal_MEMCPY_entries_are_distinct(
    int unit,
    uint8 *first_metadata,
    uint8 *first_payload,
    uint8 *second_metadata,
    uint8 *second_payload,
    uint8 *result)
{
    uint8 *ptr1;
    uint8 *ptr2;
    /*
     * retrieve entry address and size from generic pointers
     */

    uint32 size1 = ((dnxc_sw_state_journal_entry_t *)(first_metadata))->nof_elements * ((dnxc_sw_state_journal_entry_t *)(first_metadata))->element_size;
    uint32 size2 = ((dnxc_sw_state_journal_entry_t *)(second_metadata))->nof_elements * ((dnxc_sw_state_journal_entry_t *)(second_metadata))->element_size;

    DNXC_SW_STATE_JRNL_DEFS;

    ptr1 = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, first_metadata, data.memcpy_data.offset);
    ptr2 = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, second_metadata, data.memcpy_data.offset);
    /*
     * distinct if the bounds of the first does not entirely contain or the bounds of second entry
     * does not invalidate entries with NO OVERLAP
     */
    *result = ((ptr1 - ptr2 <= 0) && (ptr1 + size1 - (ptr2 + size2)) >= 0) ? FALSE : TRUE;

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * \brief - check if two HTBL entries are distinct
 * Two HTBL entries are distinct if the keys don't match
 */
static shr_error_e
dnxc_sw_state_journal_HTBL_entries_are_distinct(
    int unit,
    uint8 *first_metadata,
    uint8 *first_payload,
    uint8 *second_metadata,
    uint8 *second_payload,
    uint8 *result)
{
    uint8 *hash_table_ptr;
    DNXC_SW_STATE_JRNL_DEFS;

    hash_table_ptr = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, first_metadata, data.htbl_data.hash_table_offset);

    /*
     * check if it is the same hashtable, pointer comparison
     */
    if(DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, first_metadata, data.htbl_data.hash_table_offset)!= DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, second_metadata, data.htbl_data.hash_table_offset))
    {
        *result = TRUE;
        SHR_EXIT();
    }

    /*
     * compare keys, keys start at the beginning of payload buffers and have 'key_size' length
     */


    if(sal_memcmp(first_payload, second_payload, ((sw_state_htbl_t)hash_table_ptr)->key_size) != 0)
    {
        *result = TRUE;
        SHR_EXIT();
    }

    *result = FALSE;

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * \brief - check if two comparison journal entries are distinct
 */
static shr_error_e
dnxc_sw_state_journal_entries_are_distinct(
    int unit,
    uint8 *first_metadata,
    uint8 *first_payload,
    uint8 *second_metadata,
    uint8 *second_payload,
    uint8 *result)
{ 

    dnxc_sw_state_journal_entry_t *first = (dnxc_sw_state_journal_entry_t *)(first_metadata);
    dnxc_sw_state_journal_entry_t *second = (dnxc_sw_state_journal_entry_t *)(second_metadata);

    DNXC_SW_STATE_JRNL_DEFS;

    /*
     * different types entries are always distinct
     */
    if(first->entry_type != second->entry_type)
    {
        *result = TRUE;
        SHR_EXIT();
    }

    if(SW_JOURNAL_ENTRY_MEMCPY == first->entry_type)
    {
        dnxc_sw_state_journal_MEMCPY_entries_are_distinct(unit,
                                                          first_metadata,
                                                          first_payload,
                                                          second_metadata,
                                                          second_payload,
                                                          result);
    }
    else /* HTBL entry */
    {
        dnxc_sw_state_journal_HTBL_entries_are_distinct(unit,
                                                        first_metadata,
                                                        first_payload,
                                                        second_metadata,
                                                        second_payload,
                                                        result);
    }

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}


/**
 * \brief - print swstate memory string
 */
static shr_error_e
dnxc_sw_state_comparison_journal_print_difference(
    int unit,
    int size,
    uint8 *src,
    char *start_msg)
{
    int idx = 0;
    int string_size = 0;
    int start_msg_length = 0;

    int last_element_repeat_cnt = 0;

    int print_upper_bound = size;

    char *dest = NULL;

    char str_hex_value[DNXC_SW_STATE_JOURNAL_BYTE_HEX_LEN] = { 0 };
    char str_repeats[DNXC_SW_STATE_JOURNAL_LAST_ELEMENT_REPEAT_LEN] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /* print size + null terminator */
    string_size = (DNXC_SW_STATE_JOURNAL_BYTE_HEX_LEN * size) + 1;

    /*
     * consider text message for size allocation
     */
    if (NULL != start_msg)
    {
        start_msg_length = sal_strlen(start_msg);
        string_size += start_msg_length;
    }

    /*
     * consider the "last element repeats" string in teh size
     */
    string_size += DNXC_SW_STATE_JOURNAL_LAST_ELEMENT_REPEAT_LEN;

    DNXC_SWSTATE_JOURNAL_ALLOC(unit, dest, char *, string_size, "comparison journal str alloc", DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON, DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE);

    if (NULL == dest)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "swstate journal ERROR: could not allocate memory for string.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
    sal_memset(dest, 0x0, string_size);

    /*
     * append the message at the beginning of the line, if it does exist
     */
    if (NULL != start_msg)
    {
        sal_strncat(dest, start_msg, start_msg_length);
    }

    /*
     * check if we have a repeating last element
     */
    for (idx = size - 1; idx >= 0; idx--)
    {
        if (src[size - 1] != src[idx])
        {
            break;
        }

        last_element_repeat_cnt++;

        /*
         * print shorted msg raw data dump if we have more than 4 occurances of last byte
         */
        if (last_element_repeat_cnt > 1)
        {
            print_upper_bound = idx + 1;
        }
    }

    for (idx = 0; idx < print_upper_bound; idx++)
    {
        sal_snprintf(str_hex_value, DNXC_SW_STATE_JOURNAL_BYTE_HEX_LEN, "0x%02x ", src[idx]);
        sal_strncat(dest, str_hex_value, strlen(str_hex_value));
    }

    if (print_upper_bound != size)
    {
        sal_snprintf(str_repeats, DNXC_SW_STATE_JOURNAL_LAST_ELEMENT_REPEAT_LEN, "<last element repeats %04d times>", last_element_repeat_cnt);
        sal_strncat(dest, str_repeats, strlen(str_repeats));
    }

    LOG_CLI((BSL_META("%s\n"), dest));

    sal_free(dest);
    dest = NULL;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print swstate difference and the associated key, if any
 */
static shr_error_e
dnxc_sw_state_comparison_journal_print_differences(
    int unit, 
    uint8* key,
    uint32 key_size,
    uint8* data_before,
    uint8* data_after,
    uint32 data_size,
    char *stamp)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("\n################################################################################\n")));
    LOG_CLI((BSL_META("Difference found in swstate, associated stamp is: %s\n%s%s"), stamp, EMPTY, EMPTY));

    /*
     * print entry key
     */
    if((NULL != key) && (0 != key_size))
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_comparison_journal_print_difference(unit,
                                                                        key_size,
                                                                        key,
                                                                        "Key: "));
    }

    /*
     * print data stored and retrieved
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_comparison_journal_print_difference(unit,
                                                                    data_size,
                                                                    data_before,
                                                                    "Data before: "));

    SHR_IF_ERR_EXIT(dnxc_sw_state_comparison_journal_print_difference(unit,
                                                                    data_size,
                                                                    data_after,
                                                                    "Data after: "));

    LOG_CLI((BSL_META("################################################################################\n")));
    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief check if the size of the stamp exceeds the maximum allowed size
 */
static uint32
dnxc_sw_state_journal_get_stamp_size(
    char *stamp)
{
    uint32 len = sal_strlen(stamp);

    if(len > DNX_ROLLBACK_JOURNAL_MAX_STAMP_SIZE)
    {
        return DNX_ROLLBACK_JOURNAL_MAX_STAMP_SIZE;
    }

    return len;
}

/**
 * \brief check if swstate etnry should be excluded
 */
static uint8
dnxc_sw_state_journal_is_stamp_excluded(
    int unit,
    char *stamp,
    uint8 is_comparison)
{
    uint32 stamp_length = 0;
    uint32 idx_length = 0;

    int ii = 0;
    int nof_tables = 0;

    /*
     * list of permanently excluded stamps for comparison journal
     */
    char comparison_perm_excluded[5][100] = {{0}};

    /*
     * list of permanently excluded stamps for rollback journal
     */
    char rollback_perm_excluded[5][100] = {{0}};

    sal_snprintf(comparison_perm_excluded[0], sizeof(comparison_perm_excluded[0]), "example[%d]->rollback_comparison_excluded", unit);
    sal_snprintf(rollback_perm_excluded[0], sizeof(rollback_perm_excluded[0]), "example[%d]->rollback_comparison_excluded", unit);

    if (NULL == stamp)
    {
        return FALSE;
    }

    stamp_length = dnxc_sw_state_journal_get_stamp_size(stamp);

    if(!is_comparison)
    {
        /*
         * check dynamically excluded table ids
         */
        for (ii = 0; ii < DNXC_SW_STATE_ROLLBACK_JOURNAL_TRANSACTION.excluded_stamps_count; ii++)
        {
            idx_length = dnxc_sw_state_journal_get_stamp_size(DNXC_SW_STATE_ROLLBACK_JOURNAL_TRANSACTION.excluded_stamps[ii]);

            if((stamp_length == idx_length)
            && (sal_strncmp(DNXC_SW_STATE_ROLLBACK_JOURNAL_TRANSACTION.excluded_stamps[ii], stamp, stamp_length) == 0))
            {
                return TRUE;
            }
        }

        /*
         * check permenantly excluded table ids
         */
        nof_tables = sizeof(rollback_perm_excluded) / sizeof(rollback_perm_excluded[0]);

        for (ii = 0; ii < nof_tables; ii++)
        {
            idx_length = dnxc_sw_state_journal_get_stamp_size(rollback_perm_excluded[ii]);

            if((stamp_length == idx_length)
            && (sal_strncmp(rollback_perm_excluded[ii], stamp, stamp_length) == 0))
            {
                return TRUE;
            }
        }
    }
    else
    {
        /*
         * check dynamically excluded table ids
         */
        for (ii = 0; ii < DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION.excluded_stamps_count; ii++)
        {
            idx_length = dnxc_sw_state_journal_get_stamp_size(DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION.excluded_stamps[ii]);
            if((stamp_length == idx_length)
            && (sal_strncmp(DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION.excluded_stamps[ii], stamp, stamp_length) == 0))
            {
                return TRUE;
            }
        }

        /*
         * check permenantly excluded table ids
         */
        nof_tables = sizeof(comparison_perm_excluded) / sizeof(comparison_perm_excluded[0]);

        for (ii = 0; ii < nof_tables; ii++)
        {
            idx_length = dnxc_sw_state_journal_get_stamp_size(comparison_perm_excluded[ii]);

            if((stamp_length == idx_length)
            && (sal_strncmp(comparison_perm_excluded[ii], stamp, stamp_length) == 0))
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/**
 * \brief - read value and compare MEMCPY entries
 */
static shr_error_e
dnxc_sw_state_comparison_journal_roll_back_MEMCPY_entry_cb(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    int compare_result = 0;
    uint8 *current_value_buffer = NULL;

    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);

    uint32 payload_size = entry->nof_elements * entry->element_size;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.memcpy_data.offset) || NULL == payload)

    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state comparison journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, TRUE))
    {
        SHR_EXIT();
    }

    /*
     * allocate a buffer to read the current value
     */
    DNXC_SWSTATE_JOURNAL_ALLOC(unit, current_value_buffer, uint8 *, payload_size, "sw state comparison cur val buf", DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON, DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE);

    if(NULL == current_value_buffer)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "sw state comparison journal ERROR: could not allocate buffer.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    sal_memset(current_value_buffer, 0x0, payload_size);

    /*
     * read sw state memory
     */
    DNX_SW_STATE_MEMREAD(unit,
            current_value_buffer,
            DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.memcpy_data.offset),
            0,
            payload_size,
            DNXC_SW_STATE_JOURNAL_ROLLING_BACK,
            "sw state comparison cur val buf memread");


    /*
     * compare to payload, which is the initial state of the memory
     */
    DNX_SW_STATE_MEMCMP(unit,
            current_value_buffer,
            payload,
            0,
            payload_size,
            &compare_result,
            DNXC_SW_STATE_JOURNAL_ROLLING_BACK,
            "sw state comparison cur val buf memcmp");

    if(compare_result != 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnxc_sw_state_comparison_journal_print_differences(unit,
                                                                         NULL,
                                                                         0,
                                                                         payload,
                                                                         current_value_buffer,
                                                                         payload_size,
                                                                         stamp));
    }

    sal_free(current_value_buffer);
    current_value_buffer = NULL;
exit:
    if(NULL != current_value_buffer)
    {
        sal_free(current_value_buffer);
        current_value_buffer = NULL;
    }
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT
/**
 * \brief - read value and compare HTBL entries
 */
static shr_error_e
dnxc_sw_state_comparison_journal_roll_back_HTBL_entry_cb(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    sw_state_htbl_t hash_table = NULL;
    dnxc_sw_state_journal_entry_t *entry = NULL;

    uint8* key_section = NULL;
    uint8* data_section = NULL;

    uint8* data_buffer = NULL;

    uint8 found = 0;
    uint32 data_idx = 0;
    uint8 compare_result = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, TRUE))
    {
        SHR_EXIT();
    }

    entry = (dnxc_sw_state_journal_entry_t *)(entry_raw);
    hash_table = (sw_state_htbl_t)DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.htbl_data.hash_table_offset);

    /*
     * calculate where the key and data should be stored in the buffer
     */
    key_section = payload;
    data_section = payload + hash_table->key_size;

    DNXC_SWSTATE_JOURNAL_ALLOC(unit, data_buffer, uint8 *, hash_table->data_size, "sw state comparison cur val buf", DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON, DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE);

    if(NULL == data_buffer)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: Could not allocate memory for htbl data buffer.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_memset(data_buffer, 0x0, hash_table->data_size);

    /*
     * get the stored data 
     */
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_lookup(unit,
                                                     entry->module_id,
                                                     hash_table,
                                                     key_section,
                                                     data_buffer,
                                                     &data_idx,
                                                     &found));

    /*
     * compare the state of data before and after the comparison journal transaction.
     */
    compare_result = (sal_memcmp(data_buffer, data_section, hash_table->data_size) == 0) ? TRUE : FALSE;

    if(!compare_result)
    {

        SHR_IF_ERR_EXIT_NO_MSG(dnxc_sw_state_comparison_journal_print_differences(unit,
                                                                         key_section,
                                                                         hash_table->key_size,
                                                                         data_section,
                                                                         data_buffer,
                                                                         hash_table->data_size,
                                                                         stamp));
    }

    sal_free(data_buffer);
    data_buffer = NULL;
exit:
    if(NULL != data_buffer)
    {
        sal_free(data_buffer);
        data_buffer = NULL;
    }
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief - Update payload contents of all the valid entries in the journal that have overlapping buffer bounds.
 * Both entries are considered to be valid - partial or no overlap
 * Valid for MEMCPY entries only. 
 */
static shr_error_e
dnxc_sw_state_comparison_journal_roll_back_entry_update_buffers_cb(
    int unit,
    uint8 *entry_raw,
    uint8 *entry_payload_raw,
    uint8 *traversed_raw,
    uint8 *traversed_payload_raw)
{
    int    addr_diff = 0;
    uint32 entry_offset = 0;
    uint32 traversed_offset = 0;
    uint32 size = 0;

    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    dnxc_sw_state_journal_entry_t *traversed = (dnxc_sw_state_journal_entry_t *) (traversed_raw);

    uint32 entry_payload_size = entry->nof_elements * entry->element_size;
    uint32 traversed_payload_size = traversed->nof_elements * traversed->element_size;

    DNXC_SW_STATE_JRNL_DEFS;

    /*
     * don't update payload if NO OVERLAP between entries
     */
    if((DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.memcpy_data.offset) - (DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit,traversed, data.memcpy_data.offset) + traversed_payload_size) >= 0)
        || (DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, traversed, data.memcpy_data.offset) - (DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit,entry, data.memcpy_data.offset) + entry_payload_size)) >= 0)
    {
        return _SHR_E_NONE;
    }

    /*
     * entries are valid and are overlapping, need to copy parts of 'entry' payload to 'traversed' payload.
     * 1. Calculate the address difference between the two regions. Based on the addr diff:
     * - positive - the offset is in the 'traversed' entry, zero offset for 'entry'
     * - negative - the offset is in the 'entry', zero offset for 'traversed'
     * - zero - no offset in both entries
     * 2. Calculate the size of the memory copy
     */
    addr_diff = entry->data.memcpy_data.offset - traversed->data.memcpy_data.offset;

    if(addr_diff > 0)
    {
        traversed_offset = addr_diff;

        size = traversed_payload_size - traversed_offset;
        if(size > entry_payload_size)
        {
            size = entry_payload_size;
        }
    }
    else
    {
        /*
         * offset in the entry is always positive
         * it is exactly the same as the positive address difference
         */
        entry_offset = -(addr_diff);

        /*
         * no need to correct size in this case
         */
        size = entry_payload_size - entry_offset;
    }

    /*
     * do payload correction
     */
    sal_memcpy(traversed_payload_raw + traversed_offset,
              entry_payload_raw + entry_offset,
              size);

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - callback used for printing swstate journal entries of type "free"
 */
static shr_error_e
dnxc_sw_state_journal_free_entry_print_cb(
    int unit,
    uint8 *metadata,
    uint8 *payload)
{
    DNXC_SW_STATE_JRNL_DEFS;

    if (NULL == metadata)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: Invalid data to print.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    LOG_CLI((BSL_META("******************************* SW STATE Journal *******************************\n")));
    LOG_CLI((BSL_META("Type: FREE\n")));
    LOG_CLI((BSL_META("********************************************************************************\n\n")));

    /* print free data */

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - callback used for printing swstate journal entries of type "alloc"
 */
static shr_error_e
dnxc_sw_state_journal_alloc_entry_print_cb(
    int unit,
    uint8 *metadata,
    uint8 *payload)
{
    DNXC_SW_STATE_JRNL_DEFS;

    if (NULL == metadata)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: Invalid data to print.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    LOG_CLI((BSL_META("******************************* SW STATE Journal *******************************\n")));
    LOG_CLI((BSL_META("Type: ALLOC\n")));
    LOG_CLI((BSL_META("********************************************************************************\n\n")));


    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - callback used for printing swstate journal entries of type "alloc"
 */
static shr_error_e
dnxc_sw_state_journal_memcpy_entry_print_cb(
    int unit,
    uint8 *entry_raw,
    uint8 *payload)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);

    uint32 payload_size = entry->nof_elements * entry->element_size;

    DNXC_SW_STATE_JRNL_DEFS;

    LOG_CLI((BSL_META("******************************* SW STATE Journal *******************************\n")));

    LOG_CLI((BSL_META("Type: MEMCPY\n")));

    if (NULL == payload)
    {
        LOG_CLI((BSL_META("Data: NULL\n")));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_comparison_journal_print_difference(unit,
                                                                    payload_size,
                                                                    payload,
                                                                    "Data: "));
    }

    LOG_CLI((BSL_META("********************************************************************************\n\n")));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

#ifdef BCM_DNX_SUPPORT
/**
 * \brief - callback used for printing comparison journal hashtable entry
 */
static shr_error_e
dnxc_sw_state_journal_comparison_htbl_entry_print_cb(
    int unit,
    uint8 *entry_raw,
    uint8 *payload)
{
    DNXC_SW_STATE_JRNL_DEFS;

    LOG_CLI((BSL_META("******************************* SW STATE Journal *******************************\n")));
    LOG_CLI((BSL_META("Type: HTBL\n")));
    LOG_CLI((BSL_META("********************************************************************************\n\n")));

    /* print data */

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}
#endif

/**
 * \brief - callback used for printing mutex entries 
 */
static shr_error_e
dnxc_sw_state_journal_comparison_mutex_entry_print_cb(
    int unit,
    uint8 *entry_raw,
    uint8 *payload)
{
    dnxc_sw_state_journal_entry_t entry;
    DNXC_SW_STATE_JRNL_DEFS;

    entry = *(dnxc_sw_state_journal_entry_t*) entry_raw;

    LOG_CLI((BSL_META("******************************* SW STATE Journal *******************************\n")));
    LOG_CLI((BSL_META("Type: MUTEX\n")));
    LOG_CLI((BSL_META("Type of operation: %d\n"), entry.entry_type));
    LOG_CLI((BSL_META("********************************************************************************\n\n")));
    /* print data */

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - callback used for printing mutex entries
 */
static shr_error_e
dnxc_sw_state_journal_comparison_sem_entry_print_cb(
    int unit,
    uint8 *entry_raw,
    uint8 *payload)
{
    dnxc_sw_state_journal_entry_t entry;
    DNXC_SW_STATE_JRNL_DEFS;

    entry = *(dnxc_sw_state_journal_entry_t*) entry_raw;

    LOG_CLI((BSL_META("******************************* SW STATE Journal *******************************\n")));
    LOG_CLI((BSL_META("Type: SEMAPHORE\n")));
    LOG_CLI((BSL_META("Type of operation: %d\n"), entry.entry_type));
    LOG_CLI((BSL_META("********************************************************************************\n\n")));
    /* print data */

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}


/**
 * \brief - insert entry to sw state journal
 */
static shr_error_e
dnxc_sw_state_journal_insert(
    int unit,
    dnxc_sw_state_journal_entry_t entry,
    uint8 *payload,
    dnx_rollback_journal_rollback_handler rollback_handler,
    dnx_rollback_journal_rollback_traverse_handler rollback_traverse_handler,
    dnx_rollback_journal_print_handler print_handler,
    uint8 is_er,
    uint8 is_cmp)
{
    DNXC_SW_STATE_JRNL_DEFS;

    /*
     * journal all entry types except for hash table
     * hash table entry types are utilized only by the comparison journal
     */
    if(is_er && (SW_JOURNAL_ENTRY_HTBL_ENTRY != entry.entry_type))
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(unit,
                                                       DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE,
                                                       (uint8 *) (&entry),
                                                       sizeof(dnxc_sw_state_journal_entry_t),
                                                       payload,
                                                       entry.nof_elements * entry.element_size,
                                                       DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
                                                       rollback_handler, rollback_traverse_handler, print_handler));
    }


    /*
     * store only MEMCPY and HTBL entries in the comparison journal
     */
    if(is_cmp)
    {
        if (SW_JOURNAL_ENTRY_MEMCPY == entry.entry_type)
        {
            SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(
                        unit,
                        DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE,
                        (uint8 *) (&entry),
                        sizeof(dnxc_sw_state_journal_entry_t),
                        payload,
                        entry.nof_elements * entry.element_size,
                        DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
                        &dnxc_sw_state_comparison_journal_roll_back_MEMCPY_entry_cb,
                        &dnxc_sw_state_comparison_journal_roll_back_entry_update_buffers_cb, print_handler));
        }
#ifdef BCM_DNX_SUPPORT
        else if(SW_JOURNAL_ENTRY_HTBL_ENTRY == entry.entry_type)
        {
            SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(
                        unit,
                        DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE,
                        (uint8 *) (&entry),
                        sizeof(dnxc_sw_state_journal_entry_t),
                        payload,
                        entry.nof_elements * entry.element_size,
                        DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
                        &dnxc_sw_state_comparison_journal_roll_back_HTBL_entry_cb,
                        NULL, print_handler));
        }
#endif
    }

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - update all entry locations after 'free' entry has been roll-backed
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_update_entry_locations(
    int unit,
    uint8 *entry_raw,
    uint8 *entry_payload_raw,
    uint8 *traversed_raw,
    uint8 *traversed_payload_raw)
{
    uint32 size = 0;
    uint8 *old_location = NULL;
    uint8 *new_location = NULL;
    uint8 *calculated_pointer = NULL;
    uint8 *calculated_alloc_data_location = NULL;
    uint8 **calculated_alloc_data_new_location = NULL;
    uint8 **calculated_free_data_new_location = NULL;

    dnxc_sw_state_journal_entry_t *entry_free = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    dnxc_sw_state_journal_entry_t *traversed = (dnxc_sw_state_journal_entry_t *) (traversed_raw);

    DNXC_SW_STATE_JRNL_DEFS;

    /*
     * get new and old locations of data
     */
    old_location = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry_free, data.free_data.location_offset);
    new_location = *(DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, entry_free, data.free_data.ptr_location_offset));

    /*
     * calculate payload size
     */
    size = entry_free->element_size * entry_free->nof_elements;

    switch (traversed->entry_type)
    {
        case SW_JOURNAL_ENTRY_MEMCPY:
            calculated_pointer = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit,traversed, data.memcpy_data.offset);
            DNXC_SW_STATE_JOURNAL_UPDATE_ENTRY_ADDRESS(calculated_pointer, old_location, new_location, size);
            traversed->data.memcpy_data.offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, traversed, calculated_pointer);
            break;
        case SW_JOURNAL_ENTRY_ALLOC:
            calculated_alloc_data_location = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, traversed, data.alloc_data.location_offset);
            calculated_alloc_data_new_location = DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, traversed, data.alloc_data.ptr_location_offset);
            DNXC_SW_STATE_JOURNAL_UPDATE_ENTRY_ADDRESS(calculated_alloc_data_location, old_location, new_location,
                                                  size);
            DNXC_SW_STATE_JOURNAL_UPDATE_ENTRY_PTR_TO_ADDRESS(calculated_alloc_data_new_location, old_location,
                                                         new_location, size);
            traversed->data.alloc_data.location_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, traversed, calculated_alloc_data_location);
            traversed->data.alloc_data.ptr_location_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET_FROM_DPTR(unit, traversed, calculated_alloc_data_new_location);
            break;
        case SW_JOURNAL_ENTRY_FREE:
            calculated_free_data_new_location = DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, traversed, data.free_data.ptr_location_offset);
            DNXC_SW_STATE_JOURNAL_UPDATE_ENTRY_PTR_TO_ADDRESS(calculated_free_data_new_location, old_location,
                                                         new_location, size);
            traversed->data.alloc_data.ptr_location_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET_FROM_DPTR(unit, traversed, calculated_free_data_new_location);
            break;
        case SW_JOURNAL_ENTRY_MUTEX_CREATE:
            break;
        case SW_JOURNAL_ENTRY_MUTEX_DESTROY:
            break;
        case SW_JOURNAL_ENTRY_MUTEX_TAKE:
            break;
        case SW_JOURNAL_ENTRY_MUTEX_GIVE:
            break;
        case SW_JOURNAL_ENTRY_SEMAPHORE_CREATE:
            break;
        case SW_JOURNAL_ENTRY_SEMAPHORE_DESTROY:
            break;
        case SW_JOURNAL_ENTRY_SEMAPHORE_TAKE:
            break;
        case SW_JOURNAL_ENTRY_SEMAPHORE_GIVE:
            break;
        default:
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: Unknown entry type detected.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back a 'free' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_FREE_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if ((NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.free_data.location_offset)) || (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, entry, data.free_data.ptr_location_offset)))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    /*
     * allocate freed memory and copy the stored payload
     */
    DNX_SW_STATE_ALLOC_BASIC(unit,
                             entry->module_id,
                             DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, entry, data.free_data.ptr_location_offset),
                             entry->element_size,
                             entry->nof_elements,
                             DNXC_SW_STATE_JOURNAL_ROLLING_BACK, "sw state journal roll-back free entry");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              entry->module_id,
                              *(DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, entry, data.free_data.ptr_location_offset)),
                              payload,
                              entry->element_size * entry->nof_elements,
                              DNXC_SW_STATE_JOURNAL_ROLLING_BACK, "sw state roll back free entry");

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'alloc' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_ALLOC_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if ((NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.alloc_data.location_offset)) || (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, entry ,data.alloc_data.ptr_location_offset)))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    /*
     * FREE data and set pointer to NULL. Disable journaling 
     */
    DNX_SW_STATE_FREE_BASIC(unit,
                            entry->module_id,
                            DNXC_SWSTATE_JOURNAL_CALCULATE_DPOINTER(unit, entry, data.alloc_data.ptr_location_offset),
                            DNXC_SW_STATE_JOURNAL_ROLLING_BACK, "journal rollback alloc entry");

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back a 'memcpy' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_MEMCPY_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);

    uint32 payload_size = entry->nof_elements * entry->element_size;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.memcpy_data.offset) || NULL == payload)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }
    DNX_SW_STATE_MEMCPY_BASIC(unit, entry->module_id, DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.memcpy_data.offset), payload, payload_size,
                              DNXC_SW_STATE_JOURNAL_ROLLING_BACK, "sw state journal roll back memcpy");

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_create' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_MUTEX_CREATE_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    uint32 module_id = entry->module_id;
    uint8 *mtx_ptr = NULL;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.mutex_create.mtx_offset))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    mtx_ptr = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.mutex_create.mtx_offset);
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_destroy
                    (unit, module_id, (sw_state_mutex_t *)mtx_ptr, DNXC_SW_STATE_JOURNAL_ROLLING_BACK));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_destroy' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_MUTEX_DESTROY_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    uint32 module_id = entry->module_id;
    uint8 *mtx_ptr = NULL;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.mutex_destroy.mtx_offset))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    mtx_ptr = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.mutex_destroy.mtx_offset);
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_create
                    (unit, module_id, (sw_state_mutex_t *)mtx_ptr,"roll back created mutex", DNXC_SW_STATE_JOURNAL_ROLLING_BACK));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_take' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_MUTEX_TAKE_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    uint32 module_id = entry->module_id;
    uint8 *mtx_ptr = NULL;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit,entry, data.mutex_take.mtx_offset))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    mtx_ptr = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.mutex_take.mtx_offset);
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_give
                    (unit, module_id, (sw_state_mutex_t *)mtx_ptr, DNXC_SW_STATE_JOURNAL_ROLLING_BACK));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_give' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_MUTEX_GIVE_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    uint32 usec = DNX_SW_STATE_JOURNAL_MUTEX_DEFAULT_USEC;
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    uint32 module_id = entry->module_id;
    sw_state_mutex_t *mtx_ptr = NULL;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.mutex_give.mtx_offset))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    mtx_ptr = (sw_state_mutex_t *)(DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.mutex_give.mtx_offset));

    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_take
                    (unit, module_id, (sw_state_mutex_t *)mtx_ptr, usec, DNXC_SW_STATE_JOURNAL_ROLLING_BACK));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

static shr_error_e
dnxc_sw_state_journal_roll_back_SEM_CREATE_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    uint32 module_id = entry->module_id;
    uint8 *sem_ptr = NULL;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.sem_create.sem_offset))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    sem_ptr = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.sem_create.sem_offset);
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].sem_destroy
                    (unit, module_id, (sw_state_sem_t *)sem_ptr, DNXC_SW_STATE_JOURNAL_ROLLING_BACK));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

static shr_error_e
dnxc_sw_state_journal_roll_back_SEM_DESTROY_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    uint32 module_id = entry->module_id;
    uint8 *sem_ptr = NULL;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.sem_destroy.sem_offset))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }
    sem_ptr = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.sem_destroy.sem_offset);
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].sem_create
                    (unit, module_id, (sw_state_sem_t *)sem_ptr, entry->data.sem_destroy.is_binary, entry->data.sem_destroy.initial_count, "roll back created semaphore", DNXC_SW_STATE_JOURNAL_ROLLING_BACK));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'sem_take' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_SEM_TAKE_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    uint32 module_id = entry->module_id;
    uint8 *sem_ptr = NULL;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit,entry, data.sem_take.sem_offset))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    sem_ptr = DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.sem_take.sem_offset);
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].sem_give
                    (unit, module_id, (sw_state_sem_t *)sem_ptr, DNXC_SW_STATE_JOURNAL_ROLLING_BACK));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_give' entry
 */
static shr_error_e
dnxc_sw_state_journal_roll_back_SEM_GIVE_entry(
    int unit,
    uint8 *entry_raw,
    uint8 *payload,
    char *stamp)
{
    uint32 usec = DNX_SW_STATE_JOURNAL_SEM_DEFAULT_USER;
    dnxc_sw_state_journal_entry_t *entry = (dnxc_sw_state_journal_entry_t *) (entry_raw);
    uint32 module_id = entry->module_id;
    sw_state_sem_t *sem_ptr = NULL;

    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.sem_give.sem_offset))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * compare swstate only if stamp hasn't been explicitly excluded
     */
    if(dnxc_sw_state_journal_is_stamp_excluded(unit, stamp, FALSE))
    {
        SHR_EXIT();
    }

    sem_ptr = (sw_state_sem_t *)(DNXC_SWSTATE_JOURNAL_CALCULATE_POINTER(unit, entry, data.sem_give.sem_offset));

    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].sem_take
                    (unit, module_id, (sw_state_sem_t *)sem_ptr, usec, DNXC_SW_STATE_JOURNAL_ROLLING_BACK));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
uint8
dnxc_sw_state_journal_is_done_init(
    int unit)
{
    return dnx_rollback_journal_is_done_init(unit);
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_initialize(
    int unit,
    dnx_rollback_journal_cbs_t rollback_journal_cbs,
    dnxc_sw_state_journal_access_cb access_cb,
	uint8 is_comparison)
{
    DNXC_SW_STATE_JRNL_DEFS;

    if(NULL == rollback_journal_cbs.is_on)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "sw state journal ERROR: journal is on callback must be set.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if(is_comparison)
    {
        /*
         * comparison journal is 'inverse journal', entries are added to tail
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_new(unit,
                                                  TRUE,
                                                  DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                                  DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE,
                                                  rollback_journal_cbs,
                                                  &(DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE)));

        dnxc_sw_state_manager[unit].journals[DNX_SWSTATE_COMPARISON_JOURNAL_NUM].access_cb = access_cb;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_new(unit,
                                                  FALSE,
                                                  DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                                  DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE,
                                                  rollback_journal_cbs,
                                                  &(DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE)));

        dnxc_sw_state_manager[unit].journals[DNX_SWSTATE_ROLLBACK_JOURNAL_NUM].access_cb = access_cb;
    }

    /*
     * clear all exclusions from previous transaction
     */
    if(is_comparison)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_exclude_clear(unit, is_comparison));
    }

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_destroy(
    int unit,
    uint8 is_comparison)
{
    DNXC_SW_STATE_JRNL_DEFS;

    if(is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_destroy(unit, &DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_destroy(unit, &DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE));
    }

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

static inline shr_error_e
dnxc_sw_state_journal_roll_back_internal(
    int unit,
    uint8 only_head_rollback,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if(is_comparison)
    {
        /*
         * distinct and rollback the entire journal from tail to head
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_distinct(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE, &dnxc_sw_state_journal_entries_are_distinct));
        SHR_IF_ERR_EXIT_NO_MSG(dnx_rollback_journal_rollback(unit, FALSE, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_rollback(unit, only_head_rollback, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE));
    }

exit:
    SHR_FUNC_EXIT;
}


/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_roll_back(
    int unit,
    uint8 only_head_rollback,
    uint8 is_comparison)
{
    DNXC_SW_STATE_JRNL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_internal(unit, only_head_rollback, is_comparison));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

static inline shr_error_e
dnxc_sw_state_journal_clear_internal(
    int unit,
    uint8 only_head_clear,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if(is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, only_head_clear, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, only_head_clear, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_clear(
    int unit,
    uint8 only_head_clear,
    uint8 is_comparison)
{
    DNXC_SW_STATE_JRNL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_clear_internal(unit, only_head_clear, is_comparison));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_log_free_internal(
    int unit,
    uint32 module_id,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr_location,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * validate input 
     */
    if (NULL == ptr_location || NULL == *ptr_location)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                 "sw state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * fill entry with data, insert to journal
     */
    entry.entry_type = SW_JOURNAL_ENTRY_FREE;
    entry.nof_elements = nof_elements;
    entry.element_size = element_size;
    entry.module_id = module_id;
    entry.data.free_data.location_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit,(&entry), *ptr_location);
    entry.data.free_data.ptr_location_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET_FROM_DPTR(unit, (&entry), ptr_location);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 *ptr_location,
                                                 &dnxc_sw_state_journal_roll_back_FREE_entry,
                                                 &dnxc_sw_state_journal_roll_back_update_entry_locations,
                                                 &dnxc_sw_state_journal_free_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}


/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_log_free(
    int unit,
    uint32 module_id,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr_location)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_ACCESS_NOTIFY(unit);

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_free_internal(unit, module_id, nof_elements, element_size, ptr_location, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_log_alloc_internal(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint8 is_er,
    uint8 is_cmp)
{
    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * validate input 
     */
    if ((NULL == ptr_location) || (NULL == *ptr_location))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                 "sw state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * fill new non-null ALLOC entry with data 
     */
    entry.entry_type = SW_JOURNAL_ENTRY_ALLOC;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.alloc_data.location_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), *ptr_location);
    entry.data.alloc_data.ptr_location_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET_FROM_DPTR(unit, (&entry), ptr_location);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 *ptr_location,
                                                 &dnxc_sw_state_journal_roll_back_ALLOC_entry,
                                                 NULL,
                                                 dnxc_sw_state_journal_alloc_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_log_alloc(
    int unit,
    uint32 module_id,
    uint8 **ptr_location)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    if (!(SOC_UNIT_VALID(unit)))
    {
        return _SHR_E_NONE;
    }

    DNXC_SWSTATE_JOURNAL_ACCESS_NOTIFY(unit);

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_alloc_internal(unit, module_id, ptr_location, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_log_memcpy_internal(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *ptr,
    uint8 is_er,
    uint8 is_cmp)
{
    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * validate input 
     */
    if (NULL == ptr)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                 "sw state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (0 == size)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                 "sw state journal ERROR: data with size zero attempted to be inserted to journal.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * fill new non-null MEMCPY entry with data 
     */
    entry.entry_type = SW_JOURNAL_ENTRY_MEMCPY;
    entry.nof_elements = 1;
    entry.element_size = size;
    entry.module_id = module_id;
    entry.data.memcpy_data.offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), ptr);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 ptr,
                                                 &dnxc_sw_state_journal_roll_back_MEMCPY_entry,
                                                 NULL,
                                                 dnxc_sw_state_journal_memcpy_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_log_memcpy(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *ptr)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_ACCESS_NOTIFY(unit);

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_memcpy_internal(unit, module_id, size, ptr, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

#ifdef BCM_DNX_SUPPORT
/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_log_htbl_internal(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key)
{
    dnxc_sw_state_journal_entry_t entry;

    uint8* tmp_buff = NULL;
    uint32 tmp_buff_size = 0;

    /*
     * section locations in temporary buffer
     */
    uint8* key_section = NULL;
    uint8* data_section = NULL;

    /*
     * htbl specific parameters
     */
    uint8 found = 0;
    uint32 data_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if(NULL == hash_table)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: Invalid hash table specified for journal entry.\n%s%s%s", EMPTY, EMPTY, EMPTY);

    }

    if(NULL == key)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: Invalid hash table key specified for journal entry.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    /*
     * allocate temporary buffer, holding key and data size for journaling
     */
    tmp_buff_size = hash_table->key_size + hash_table->data_size;

    DNXC_SWSTATE_JOURNAL_ALLOC(unit, tmp_buff, uint8 *, tmp_buff_size, "tmp_buff hash table swstate journal", DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON, DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE);

    if(NULL == tmp_buff)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "sw state journal ERROR: Could not allocate memory for htbl entry buffer.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_memset(tmp_buff, 0x0, tmp_buff_size);

    /*
     * calculate where the key and data should be stored in the buffer
     */
    key_section = tmp_buff;
    data_section = tmp_buff + hash_table->key_size;

    /*
     * fill memory sections
     */
    sal_memcpy(key_section, key, hash_table->key_size);

    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_lookup(unit, module_id, hash_table, key, data_section, &data_idx, &found));

    /*
     * fill metadata
     */
    entry.entry_type = SW_JOURNAL_ENTRY_HTBL_ENTRY;
    entry.nof_elements = 1;
    entry.element_size = tmp_buff_size;
    entry.module_id = module_id;
    entry.data.htbl_data.hash_table_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)hash_table);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                    entry,
                    tmp_buff,
                    NULL,
                    NULL,
                    &dnxc_sw_state_journal_comparison_htbl_entry_print_cb,
                    FALSE,
                    TRUE));

    sal_free(tmp_buff);
    tmp_buff = NULL;

exit:
    if(NULL != tmp_buff)
    {
        sal_free(tmp_buff);
    }
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e dnxc_sw_state_journal_log_htbl(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * cancels the suppression that is assumed to exist prior to the call to the function
     */
    dnxc_sw_state_comparison_journal_tmp_suppress(unit, FALSE);

    DNXC_SWSTATE_JOURNAL_ACCESS_NOTIFY(unit);

    /*
     * journaling valid only for Comparison Journal
     */
    if(!dnx_rollback_journal_is_on(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_htbl_internal(unit, module_id, hash_table, key));

exit:
    /*
     * restore the suppression that was cancelled
     */
    dnxc_sw_state_comparison_journal_tmp_suppress(unit, TRUE);
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_mutex_create_internal(
    int unit,
    uint32 module_id,
    sw_state_mutex_t *ptr_mtx,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.entry_type = SW_JOURNAL_ENTRY_MUTEX_CREATE;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.mutex_create.mtx_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)ptr_mtx);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 (uint8 *) (ptr_mtx),
                                                 &dnxc_sw_state_journal_roll_back_MUTEX_CREATE_entry,
                                                 NULL,
                                                 &dnxc_sw_state_journal_comparison_mutex_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_mutex_destroy_internal(
    int unit,
    uint32 module_id,
    sw_state_mutex_t *ptr_mtx,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.entry_type = SW_JOURNAL_ENTRY_MUTEX_DESTROY;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.mutex_destroy.mtx_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)ptr_mtx);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 (uint8 *) (ptr_mtx),
                                                 &dnxc_sw_state_journal_roll_back_MUTEX_DESTROY_entry,
                                                 NULL,
                                                 &dnxc_sw_state_journal_comparison_mutex_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_mutex_take_internal(
    int unit,
    uint32 module_id,
    sw_state_mutex_t *ptr_mtx,
    uint32 usec,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.entry_type = SW_JOURNAL_ENTRY_MUTEX_TAKE;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.mutex_take.mtx_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)ptr_mtx);
    entry.data.mutex_take.usec = usec;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 (uint8 *) (ptr_mtx),
                                                 &dnxc_sw_state_journal_roll_back_MUTEX_TAKE_entry,
                                                 NULL,
                                                 &dnxc_sw_state_journal_comparison_mutex_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_sem_create_internal(
    int unit,
    uint32 module_id,
    sw_state_sem_t *ptr_sem,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.entry_type = SW_JOURNAL_ENTRY_SEMAPHORE_CREATE;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.sem_create.sem_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)ptr_sem);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 (uint8 *) (ptr_sem),
                                                 &dnxc_sw_state_journal_roll_back_SEM_CREATE_entry,
                                                 NULL,
                                                 &dnxc_sw_state_journal_comparison_sem_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_sem_destroy_internal(
    int unit,
    uint32 module_id,
    sw_state_sem_t *ptr_sem,
    int is_binary,
    int initial_count,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.entry_type = SW_JOURNAL_ENTRY_SEMAPHORE_DESTROY;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.sem_destroy.sem_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)ptr_sem);
    entry.data.sem_destroy.is_binary = is_binary;
    entry.data.sem_destroy.initial_count = initial_count;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 (uint8 *) (ptr_sem),
                                                 &dnxc_sw_state_journal_roll_back_SEM_DESTROY_entry,
                                                 NULL,
                                                 &dnxc_sw_state_journal_comparison_sem_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
dnxc_sw_state_journal_sem_take_internal(
    int unit,
    uint32 module_id,
    sw_state_sem_t *ptr_sem,
    uint32 usec,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.entry_type = SW_JOURNAL_ENTRY_SEMAPHORE_TAKE;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.sem_take.sem_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)ptr_sem);
    entry.data.sem_take.usec = usec;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 (uint8 *) (ptr_sem),
                                                 &dnxc_sw_state_journal_roll_back_SEM_TAKE_entry,
                                                 NULL,
                                                 &dnxc_sw_state_journal_comparison_sem_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_mutex_create(
    int unit,
    uint32 module_id,
    sw_state_mutex_t *ptr_mtx)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_create_internal(unit, module_id, ptr_mtx, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_mutex_destroy(
    int unit,
    uint32 module_id,
    sw_state_mutex_t *ptr_mtx)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_destroy_internal(unit, module_id, ptr_mtx, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_mutex_take(
    int unit,
    uint32 module_id,
    sw_state_mutex_t *ptr_mtx,
    uint32 usec)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_take_internal(unit, module_id, ptr_mtx, usec, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_sem_create(
    int unit,
    uint32 module_id,
    sw_state_sem_t *ptr_sem)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_create_internal(unit, module_id, ptr_sem, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_sem_destroy(
    int unit,
    uint32 module_id,
    sw_state_sem_t *ptr_sem,
    int is_binary,
    int initial_count)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_destroy_internal(unit, module_id, ptr_sem, is_binary, initial_count ,is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_sem_take(
    int unit,
    uint32 module_id,
    sw_state_sem_t *ptr_sem,
    uint32 usec)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_take_internal(unit, module_id, ptr_sem, usec, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_mutex_give_internal(
    int unit,
    uint32 module_id,
    sw_state_mutex_t *ptr_mtx,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.entry_type = SW_JOURNAL_ENTRY_MUTEX_GIVE;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.mutex_give.mtx_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)ptr_mtx);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 (uint8 *) (ptr_mtx),
                                                 &dnxc_sw_state_journal_roll_back_MUTEX_GIVE_entry,
                                                 NULL,
                                                 &dnxc_sw_state_journal_comparison_mutex_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_mutex_give(
    int unit,
    uint32 module_id,
    sw_state_mutex_t *ptr_mtx)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_give_internal(unit, module_id, ptr_mtx, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/**
 * \brief - assumes that journal is on, thread unsafe.
 */
static inline shr_error_e
dnxc_sw_state_journal_sem_give_internal(
    int unit,
    uint32 module_id,
    sw_state_sem_t *ptr_sem,
    uint8 is_er,
    uint8 is_cmp)
{

    dnxc_sw_state_journal_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    entry.entry_type = SW_JOURNAL_ENTRY_SEMAPHORE_GIVE;
    entry.nof_elements = 0;
    entry.element_size = 0;
    entry.module_id = module_id;
    entry.data.sem_give.sem_offset = DNXC_SWSTATE_JOURNAL_CALCULATE_OFFSET(unit, (&entry), (uint8*)ptr_sem);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_insert(unit,
                                                 entry,
                                                 (uint8 *) (ptr_sem),
                                                 &dnxc_sw_state_journal_roll_back_SEM_GIVE_entry,
                                                 NULL,
                                                 &dnxc_sw_state_journal_comparison_sem_entry_print_cb,
                                                 is_er,
                                                 is_cmp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_sem_give(
    int unit,
    uint32 module_id,
    sw_state_sem_t *ptr_sem)
{
    uint8 is_er = FALSE;
    uint8 is_cmp = FALSE;

    DNXC_SW_STATE_JRNL_DEFS;

    DNXC_SWSTATE_JOURNAL_EXIT_IF_OFF(unit, is_er, is_cmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_give_internal(unit, module_id, ptr_sem, is_er, is_cmp));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

static inline shr_error_e
dnxc_sw_state_journal_get_head_seq_id_internal(
    int unit,
    uint8 is_comparison,
    uint32 *seq_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if(is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_get_head_seq_id(unit, seq_id, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_get_head_seq_id(unit, seq_id, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_get_head_seq_id(
    int unit,
    uint8 is_comparison,
    uint32 *seq_id)
{
    DNXC_SW_STATE_JRNL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_head_seq_id_internal(unit, is_comparison, seq_id));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
static inline shr_error_e
dnxc_sw_state_journal_entries_stamp_internal(
    int unit,
    char *format,
    va_list args)
{
    char str_to_print[DNX_ROLLBACK_JOURNAL_MAX_STAMP_SIZE + 1] = { 0 };
    int size = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * check if the size of the format exceeds the maximum allowed size
     */
    size = dnxc_sw_state_journal_get_stamp_size(format);

    sal_vsnprintf(str_to_print, size, format, args);

    if(dnx_rollback_journal_is_on(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE))
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entries_stamp(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE, str_to_print));
    }

    if(dnx_rollback_journal_is_on(unit, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE))
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entries_stamp(unit, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE, str_to_print));
    }

exit:
    SHR_FUNC_EXIT;
}


/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_entries_stamp(
    int unit,
    char *format,
    va_list args)
{
    DNXC_SW_STATE_JRNL_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_entries_stamp_internal(unit, format, args));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
uint8
dnxc_sw_state_journal_should_stamp(
    int unit)
{
#ifdef DNX_ROLLBACK_JOURNAL_DEBUG
    return dnx_rollback_journal_is_on(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE)
        || dnx_rollback_journal_is_on(unit, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE);
#else
    return dnx_rollback_journal_is_on(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE);
#endif
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_journal_all_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable)
{
    DNXC_SW_STATE_JRNL_DEFS;

    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change(unit, is_disable, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE, TRUE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change(unit, is_disable, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE, TRUE));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e
dnxc_sw_state_comparison_journal_tmp_suppress(
    int unit,
    uint8 is_disable)
{
    DNXC_SW_STATE_JRNL_DEFS;

    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change(unit, is_disable, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE, FALSE));

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e dnxc_sw_state_journal_exclude_by_stamp(
    int unit,
    char *stamp,
    uint8 is_comparison)
{
    DNXC_SW_STATE_JRNL_DEFS;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if(!is_comparison)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                "sw state journal ERROR: rollback journal exclusion is not supported.\n%s%s%s",
                EMPTY, EMPTY, EMPTY);
    }

    if(!dnx_rollback_journal_is_on(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE))
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                 "sw state journal ERROR: comparison journal exclusions are done only if the comparison journal is on.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if(DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION.excluded_stamps_count + 1 >= DNXC_SW_STATE_JOURNAL_EXCLUDED_STAMPS_MAX_NOF)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                 "sw state journal ERROR: comparison journal table exclusions exceed the maximum number of allowed.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION.excluded_stamps[DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION.excluded_stamps_count] = stamp;
    DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION.excluded_stamps_count++;

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e dnxc_sw_state_journal_exclude_clear(
    int unit,
    uint8 is_comparison)
{
    DNXC_SW_STATE_JRNL_DEFS;

    if(!is_comparison)
    {
        DNX_ER_THREADING_ERROR_LOG_IF_NOT_JOURNALING_THREAD(unit);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                "sw state journal ERROR: rollback journal exclusion clear is not supported.\n%s%s%s",
                EMPTY, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_COMPARISON_JOURNAL_TRANSACTION.excluded_stamps_count = 0;

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e dnxc_sw_state_journal_print(
    int unit,
    uint8 is_comparison)
{

    DNXC_SW_STATE_JRNL_DEFS;

    if(!is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_print(unit, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_print(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE));
    }

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

/*
 * see .h file for description
 */
shr_error_e dnxc_sw_state_journal_logger_state_change(
    int unit,
    uint8 is_comparison)
{
    DNXC_SW_STATE_JRNL_DEFS;

    if(!is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change_by_type(unit, DNXC_SW_STATE_ROLLBACK_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change_by_type(unit, DNXC_SW_STATE_COMPARISON_JOURNAL_HANDLE));
    }

    DNXC_SW_STATE_JRNL_FUNC_RETURN;
}

#endif /* BCM_DNX_SUPPORT */
#undef _ERR_MSG_MODULE_NAME
