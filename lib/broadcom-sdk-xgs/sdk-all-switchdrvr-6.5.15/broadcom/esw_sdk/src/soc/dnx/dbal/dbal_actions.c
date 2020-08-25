/** \file dbal_actions.c
 * $Id$
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * System wide Logical Table Manager internal functions.
 *
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal_internal.h"
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <shared/utilex/utilex_bitstream.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dbal/dbal_journal.h>

/** like SHR_IF_ERR_EXIT but wrap the expression with Journal
 *  suppressions **/
#define DBAL_IF_ERR_EXIT_WITH_ER_SUPPRESS(_expr)      \
do                      \
{                       \
    int _rv;\
\
    DNX_ERR_RECOVERY_SUPPRESS(unit);\
    dnx_err_recovery_suppress_errors(unit, DNX_ERR_RECOVERY_ERR_SUPPRESS_SW_ACCESS_OUTSIDE_TRANS);\
    dnx_state_comparison_suppress(unit, TRUE);\
\
    _rv = _expr;      \
\
    dnx_state_comparison_suppress(unit, FALSE);\
    dnx_err_recovery_unsuppress_errors(unit, DNX_ERR_RECOVERY_ERR_SUPPRESS_SW_ACCESS_OUTSIDE_TRANS);\
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);\
\
    if (SHR_FAILURE(_rv)) \
    {                     \
        LOG_ERROR_EX(BSL_LOG_MODULE,                       \
            " Error '%s' indicated ; %s%s%s\r\n" ,           \
            shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;  \
            _func_rv = _rv ;  \
        SHR_EXIT() ;        \
    }                     \
} while (0)

/* *INDENT-OFF* */
/**
 * DBAL actions information, actions are DBAL apis that performs access.
 */
#define COMMON_LEGAL_FLAGS  (DBAL_COMMIT_DISABLE_ACTION_PRINTS | DBAL_COMMIT| DBAL_COMMIT_IGNORE_ALLOC_ERROR )
#define PRE_ACCESS_PRINTS_ENABLED   1
#define POST_ACCESS_PRINTS_ENABLED  0

CONST dbal_action_info_t dbal_actions_info[DBAL_NOF_ACTIONS] =
{
/** ACTION NAME        | PRINTS                  | VALIDATIONS                                        | LEGAL FLAGS  */
    /** DBAL_ACTION_ENTRY_COMMIT */
    {"COMMIT",         PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD | VALIDATE_LIF_RESULT_TYPE,  COMMON_LEGAL_FLAGS | DBAL_COMMIT_UPDATE | DBAL_COMMIT_FORCE | DBAL_COMMIT_OVERRIDE_DEFAULT | DBAL_COMMIT_OVERRUN},

    /** DBAL_ACTION_ENTRY_COMMIT_UPDATE */
    {"COMMIT UPDATE",  PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD | VALIDATE_LIF_RESULT_TYPE,  COMMON_LEGAL_FLAGS},

    /** DBAL_ACTION_ENTRY_COMMIT_FORCE */
    {"COMMIT FORCE",   PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD | VALIDATE_LIF_RESULT_TYPE,  COMMON_LEGAL_FLAGS},

    /** DBAL_ACTION_ENTRY_COMMIT_RANGE */
    {"COMMIT RANGE",   PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD | VALIDATE_LIF_RESULT_TYPE,  COMMON_LEGAL_FLAGS},

    /** DBAL_ACTION_ENTRY_GET */
    {"GET",            POST_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | RESULT_FIELD_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD | VALIDATE_LIF_RESULT_TYPE, COMMON_LEGAL_FLAGS | DBAL_GET_ALL_FIELDS | DBAL_COMMIT_VALIDATE_OTHER_CORE},

    /** DBAL_ACTION_ENTRY_GET_ALL */
    {"GET ALL FIELDS", POST_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST| VALIDATE_ALLOCATOR_FIELD | VALIDATE_LIF_RESULT_TYPE,          COMMON_LEGAL_FLAGS},

    /** DBAL_ACTION_ENTRY_GET_ACCESS_ID */
    {"GET ACCESS ID BY KEY", POST_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | VALIDATE_ALLOCATOR_FIELD,                      COMMON_LEGAL_FLAGS},

    /** DBAL_ACTION_ENTRY_CLEAR */
    {"ENTRY CLEAR",    PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | VALIDATE_LIF_RESULT_TYPE,                            COMMON_LEGAL_FLAGS},

    /** DBAL_ACTION_ENTRY_CLEAR_RANGE */
    {"ENTRY CLEAR RANGE", PRE_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST | VALIDATE_LIF_RESULT_TYPE,                         COMMON_LEGAL_FLAGS},

	/** DBAL_ACTION_ITERATOR_GET */
	{"ITERATOR GET", POST_ACCESS_PRINTS_ENABLED, ALL_KEY_FIELDS_MUST_EXIST| VALIDATE_ALLOCATOR_FIELD | VALIDATE_LIF_RESULT_TYPE,          COMMON_LEGAL_FLAGS},

    /** DBAL_ACTION_TABLE_CLEAR */
    {"TABLE CLEAR",    PRE_ACCESS_PRINTS_ENABLED, CORE_COULD_BE_NOT_INITIALIZED,                        COMMON_LEGAL_FLAGS}

};


static shr_error_e dbal_actions_access_commit_consecutive_range(int unit,dbal_entry_handle_t * entry_handle,int num_of_entries);

static shr_error_e dbal_actions_access_clear_consecutive_range(int unit, dbal_entry_handle_t * entry_handle, int num_of_entries);


/* *INDENT-ON* */

/**
 * \brief
 * Copy results from secondary_entry to prim_entry, where the results in prim_entry were not set. At the end of the
 * merging the prim_entry will include all the result fields and their values.
 *
 * this function updates the prim_entry payload with the existing entry payload, it means that the payload of the entry
 * will be dirty (for the dbal user) and if he will re-commit an entry need to merge the prime enrty again with
 * the current HW value.
 */
shr_error_e
dbal_merge_entries(
    int unit,
    dbal_entry_handle_t * prim_entry,
    dbal_entry_handle_t * secondary_entry)
{
    int payload_num_of_uint32;
    uint32 converted_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 converted_payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    dbal_logical_table_t *table = prim_entry->table;

    SHR_FUNC_INIT_VARS(unit);

    /** If all result fields were already set - no need to merge */
    if (prim_entry->nof_result_fields != table->multi_res_info[prim_entry->cur_res_type].nof_result_fields)
    {
        if (prim_entry->cur_res_type != secondary_entry->cur_res_type)
        {
            int padding_size = table->multi_res_info[secondary_entry->cur_res_type].zero_padding;

            /**
              * If all payload is zeros (means that the entry is empty), do not raise an error and continue to trivial
              * merge This is the case when writing to an empty entry - so writing any result type is valid
              */
            if (utilex_bitstream_have_one_in_range(secondary_entry->phy_entry.payload, padding_size,
                                                   table->multi_res_info[secondary_entry->
                                                                         cur_res_type].entry_payload_size +
                                                   padding_size))
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Cannot merge entries of different result types. Primary = %d, Secondary = %d\n",
                             prim_entry->cur_res_type, secondary_entry->cur_res_type);
            }
        }
        prim_entry->entry_merged = 1;

        payload_num_of_uint32 = (table->max_payload_size + 31) / 32;

        /**Update the values buffer - Make sure that primary payload has zeros where not set */
        SHR_IF_ERR_EXIT(utilex_bitstream_and
                        (prim_entry->phy_entry.payload, prim_entry->phy_entry.p_mask, payload_num_of_uint32));

        /** Convert primary mask and use it to get relevant bits from secondary payload */
        sal_memcpy(converted_mask, prim_entry->phy_entry.p_mask, sizeof(converted_mask));
        sal_memcpy(converted_payload, secondary_entry->phy_entry.payload, sizeof(converted_payload));
        SHR_IF_ERR_EXIT(utilex_bitstream_not(converted_mask, payload_num_of_uint32));
        SHR_IF_ERR_EXIT(utilex_bitstream_and(converted_payload, converted_mask, payload_num_of_uint32));

        /** Merge payloads using OR */
        SHR_IF_ERR_EXIT(utilex_bitstream_or(prim_entry->phy_entry.payload, converted_payload, payload_num_of_uint32));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_get_handle_update_value_field(
    int unit,
    dbal_entry_handle_t * handle)
{
    int iter = 0;
    int cur_res_type = handle->cur_res_type;
    dbal_logical_table_t *table = handle->table;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < table->multi_res_info[cur_res_type].nof_result_fields; iter++)
    {
        handle->value_field_ids[iter] = table->multi_res_info[cur_res_type].results_info[iter].field_id;
    }

    /*
     * make sure that are no leftover non-empty fields
     */
    for (; iter < DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE; iter++)
    {
        if (handle->value_field_ids[iter] == DBAL_FIELD_EMPTY)
        {
            break;
        }

        handle->value_field_ids[iter] = DBAL_FIELD_EMPTY;
    }

    /*
     * some handle fields need to be updated
     */
    handle->nof_result_fields = table->multi_res_info[cur_res_type].nof_result_fields;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  performing clear \ commit to multiple entries that are in a range one by one accoding to the action.
 */
shr_error_e
dbal_entry_non_consecutive_range_action_perform(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries,
    uint32 *key_field_min_values,
    dbal_actions_e action)
{
    int ii, jj;
    uint32 key_field_counter[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    uint32 key_field_orig_values[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    int orig_nof_ranged_fields = 0;

    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n***** (non-consecutive) min key value %d nof entries %d ****\n"),
                                 entry_handle->phy_entry.key[0], num_of_entries));

    sal_memcpy(key_field_counter, key_field_min_values, sizeof(uint32) * DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS);

    orig_nof_ranged_fields = entry_handle->nof_ranged_fields;
    entry_handle->nof_ranged_fields = 0;

    for (ii = 0; ii < entry_handle->nof_key_fields; ii++)
    {
        key_field_orig_values[ii] = entry_handle->key_field_ranges[ii];
    }

    /**non-consecutive mode, updating the handle with the next key field and performing normal commit.. */
    for (ii = 0; ii < num_of_entries; ii++)
    {
        if (action == DBAL_ACTION_ENTRY_COMMIT_RANGE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit_normal(unit, entry_handle));
        }
        else if (action == DBAL_ACTION_ENTRY_CLEAR_RANGE)
        {
            SHR_IF_ERR_EXIT(dbal_actions_access_entry_clear(unit, entry_handle));
        }

        for (jj = 0; jj < entry_handle->nof_key_fields; jj++)
        {
            if (key_field_orig_values[jj] != 0)
            {
                uint32 max_key_field_value = (key_field_min_values[jj] - 1) + key_field_orig_values[jj];
                if (key_field_counter[jj] <= max_key_field_value)
                {
                    if (key_field_counter[jj] == max_key_field_value)
                    {
                        key_field_counter[jj] = key_field_min_values[jj];
                    }
                    else
                    {
                        uint8 is_illegal_value_for_field = TRUE;
                        while (is_illegal_value_for_field)
                        {
                            key_field_counter[jj]++;
                            SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, entry_handle->key_field_ids[jj],
                                                                         key_field_counter[jj],
                                                                         &is_illegal_value_for_field));
                        }
                        break;
                    }
                }
            }
        }

        for (jj = 0; jj < entry_handle->nof_key_fields; jj++)
        {
            if (key_field_orig_values[jj] > 0)
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "setting value %d to field %s, "),
                                             key_field_counter[jj], dbal_field_to_string(unit,
                                                                                         entry_handle->key_field_ids
                                                                                         [jj])));

                /** this part setting the range to 0 because it is normal field set */
                dbal_entry_key_field32_set(unit, entry_handle->handle_id, entry_handle->key_field_ids[jj],
                                           key_field_counter[jj]);
            }
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        if (entry_handle->error_info.error_exists)
        {
            break;
        }
    }

    /** setting back the ranges on the handle incase the user will want to use another range action on the table.
     *  (retreiving the handle to the original state) */
    for (ii = 0; ii < entry_handle->nof_key_fields; ii++)
    {
        entry_handle->key_field_ranges[ii] = key_field_orig_values[ii];
    }
    entry_handle->nof_ranged_fields = orig_nof_ranged_fields;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_direct_is_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify that the table is direct.*/
    if (dbal_tables_is_non_direct(unit, entry_handle->table_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table is indirect: %s.\n",
                     dbal_logical_table_to_string(unit, entry_handle->table_id));
    }

    (*is_default) = 0;
    if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
    {
        SHR_IF_ERR_EXIT(dbal_hl_is_entry_default(unit, entry_handle, is_default));
    }
    else
    {
        if (sal_memcmp(zero_buffer_to_compare, entry_handle->phy_entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_BYTES) == 0)
        {
            (*is_default) = 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit_normal(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;
    dbal_entry_handle_t *get_entry_handle = NULL;
    dbal_entry_handle_t *get_entry_copy = NULL; /** holds the get entry found or NULL otherwise*/
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    entry_handle->is_entry_update = FALSE;

    if (dbal_tables_is_non_direct(unit, entry_handle->table_id))
    {
        /** need to perform get and validate that the entry no exists. */
        rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

        /** In case of non direct table - verify entry does not exist yet. if exists need to use commit update/force. */
        if (table->table_type == DBAL_TABLE_TYPE_TCAM && table->access_method == DBAL_ACCESS_METHOD_MDB)
        {
            
        }
        else
        {
            if (rv == _SHR_E_NONE)
            {
                /** Entry exists */
                SHR_ERR_EXIT(_SHR_E_EXISTS, "Cannot commit entry - already exists in table %s\n", table->table_name);
            }

            if (entry_handle->entry_merged)
            {
                /**
                 *  In case of non-direct and the entry was already merged before, merging the entry with the default entry, to clear
                 *  unneeded values from previous merging on the handle. (In case the entry was merged before with different values)
                 */
                SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));
            }
        }
    }
    else
    {
        if (dbal_tables_is_merge_entries_supported(unit, entry_handle->table_id) || dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_ALL))
        {
			rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle);
			SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
			/** For direct tables, we need to save the preexisting entry if it is different from the default entry.*/
			if (rv != _SHR_E_NOT_FOUND)
			{
				int is_default = 0;

				/** get_entry_handle is the entry that we received from HW to use in the error recovery journal. incase that the
				 *  entry is default no need to keep entry */
				SHR_IF_ERR_EXIT(dbal_entry_direct_is_default(unit, get_entry_handle, &is_default));
				if (!is_default)
				{
					get_entry_copy = get_entry_handle;
				}
			}

            /** if needed merging the entry with the exsiting entry in the HW, only for non-direct it is possible that an entry
             *  exists */
            if (dbal_tables_is_merge_entries_supported(unit, entry_handle->table_id))
			{
				if (entry_handle->nof_result_fields != table->multi_res_info[entry_handle->cur_res_type].nof_result_fields)
				{
					entry_handle->is_entry_update = TRUE;
				
					if (rv == _SHR_E_NONE)
					{
						/** Entry exists - merge new values with the existing values in HW */
						SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));
					}
				}
			}
        }
    }

    /** Add the entry both direct and non-direct */
    SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, entry_handle, get_entry_copy));

exit:
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit_update(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;
    dbal_entry_handle_t *get_entry_handle = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    entry_handle->is_entry_update = TRUE;

    /** Get the entry if exists */
    rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NOT_FOUND)
    {
        /** Entry exists */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot update entry - entry does not exist in table %s\n",
                     entry_handle->table->table_name);
    }

    /** Entry exists - merge new values with the existing values in HW */
    SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));

    SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, entry_handle, get_entry_handle));

exit:
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit_force(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_entry_handle_t *get_entry_handle = NULL;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle);

    if (rv == _SHR_E_NONE)
    {
        /** Entry exists need to update the entry */
        entry_handle->is_entry_update = TRUE;

        /** Merge new values with the existing values in HW */
        SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));

    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        if (entry_handle->entry_merged)
        {
            /**
             *  incase of non-direct merging the entry with the default entry, to clear unneeded info from previus merging on the
             *  entry handle. (incase the entry was merged before with different values)
             */
            SHR_IF_ERR_EXIT(dbal_merge_entries(unit, entry_handle, get_entry_handle));
        }

        /** Entry does not exist, need to add the entry */
        entry_handle->is_entry_update = FALSE;
        SHR_FREE(get_entry_handle);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, entry_handle, get_entry_handle));

exit:
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_range_action_process(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e action)
{
    dbal_logical_table_t *table;
    int num_of_entries = 1, ii, jj;
    uint32 max_key_handle_id;
    dbal_entry_handle_t *max_key_handle;
    uint32 key_field_min_values[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    /** updates num_of_entries and first entry  */
    for (ii = 0; ii < entry_handle->nof_key_fields; ii++)
    {
        if (entry_handle->key_field_ranges[ii] != 0)
        {
            num_of_entries = num_of_entries * entry_handle->key_field_ranges[ii];
            SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table->keys_info[ii], entry_handle->key_field_ids[ii],
                                                       entry_handle->phy_entry.key, &key_field_min_values[ii]));
        }
    }

    if (table->range_set_supported)
    {
        /**
         *  if table supports range, calculate the last entry to check if the range is consecutive
         *  (by setting all fields max values in a new handle)
         */
        uint8 is_illegal_value_exists = FALSE;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &max_key_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, max_key_handle_id, &max_key_handle));

        max_key_handle->phy_entry.key[0] = entry_handle->phy_entry.key[0];
        for (jj = 0; jj < entry_handle->nof_key_fields; jj++)
        {
            uint8 curr_field_illegal_value = FALSE;
            SHR_IF_ERR_EXIT(dbal_fields_has_illegal_values
                            (unit, entry_handle->key_field_ids[jj], &curr_field_illegal_value));
            is_illegal_value_exists |= curr_field_illegal_value;
            if (entry_handle->key_field_ranges[jj] > 0)
            {
                uint32 max_key_field_value = (key_field_min_values[ii] - 1) + entry_handle->key_field_ranges[jj];
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nSetting max value %d for field %s \n"),
                                             key_field_min_values[ii] + entry_handle->key_field_ranges[jj],
                                             dbal_field_to_string(unit, entry_handle->key_field_ids[jj])));
                dbal_entry_key_field32_set(unit, max_key_handle_id, entry_handle->key_field_ids[jj],
                                           max_key_field_value);
                if (max_key_handle->error_info.error_exists)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, " field %s out of range min val = %d range = %d \n",
                                 dbal_field_to_string(unit, max_key_handle->error_info.field_id),
                                 key_field_min_values[ii], entry_handle->key_field_ranges[jj]);
                }
            }
        }

        /** if consecutive we can use DMA mode otherwise, need to update each entry by itself */
        if ((((entry_handle->phy_entry.key[0] - 1) + num_of_entries) == max_key_handle->phy_entry.key[0])
            && (!is_illegal_value_exists))
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit, "\n***** (consecutive) min entry offset %d max entry offset %d nof entries %d ****\n"),
                         entry_handle->phy_entry.key[0], max_key_handle->phy_entry.key[0], num_of_entries));

            if (action == DBAL_ACTION_ENTRY_COMMIT_RANGE)
            {
                SHR_IF_ERR_EXIT(dbal_actions_access_commit_consecutive_range(unit, entry_handle, num_of_entries));
                SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, num_of_entries, TRUE));
                SHR_EXIT();
            }
            else if (action == DBAL_ACTION_ENTRY_CLEAR_RANGE)
            {
                SHR_IF_ERR_EXIT(dbal_actions_access_clear_consecutive_range(unit, entry_handle, num_of_entries));
                SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, num_of_entries, FALSE));
                SHR_EXIT();
            }
        }
    }

    /** in case we reached here consecutive mode was'nt executed need to add entry by entry */
    SHR_IF_ERR_EXIT(dbal_entry_non_consecutive_range_action_perform(unit, entry_handle, num_of_entries,
                                                                    key_field_min_values, action));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_action_finish(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e action)
{
    CONST dbal_action_info_t *actions_info = &dbal_actions_info[(action)];

    SHR_FUNC_INIT_VARS(unit);

    if (actions_info->is_pre_acces_prints == POST_ACCESS_PRINTS_ENABLED)
    {
        SHR_IF_ERR_EXIT(dbal_action_prints(unit, entry_handle, flags, action));
    }

    SHR_IF_ERR_EXIT(dbal_logger_table_internal_unlock(unit, entry_handle->table_id));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_action_validate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e action,
    dbal_entry_action_flags_e flags)
{
    dbal_logical_table_t *table = entry_handle->table;
    CONST dbal_action_info_t *actions_info = &dbal_actions_info[(action)];

    SHR_FUNC_INIT_VARS(unit);
    /** General validations - for all actions  */
    if (entry_handle->handle_status < DBAL_HANDLE_STATUS_IN_USE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Handle status is invalid, cannot perform action. handle_id=%d\n",
                     entry_handle->handle_id);
    }

    if (entry_handle->error_info.error_exists)
    {
        int res_type = 0;

        if (entry_handle->cur_res_type > 0)
        {
            res_type = entry_handle->cur_res_type;
        }
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot perform action. there is an issue with field %s table %s res type %s\n",
                     dbal_field_to_string(unit, entry_handle->error_info.field_id), table->table_name,
                     table->multi_res_info[res_type].result_type_name);
    }

    if (entry_handle->table->has_result_type && entry_handle->cur_res_type == entry_handle->table->nof_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "when setting superset result type value cannot perform action. handle_id=%d table %s\n",
                     entry_handle->handle_id, table->table_name);
    }

    if (!(actions_info->action_validation_flags & CORE_COULD_BE_NOT_INITIALIZED))
    {
        if ((table->core_mode == DBAL_CORE_MODE_SBC) && (entry_handle->core_id != DBAL_CORE_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s of type SBC, but core in handle is not CORE_ALL. core=%d\n",
                         table->table_name, entry_handle->core_id);
        }

        if ((table->core_mode == DBAL_CORE_MODE_DPC) && (entry_handle->core_id == DBAL_CORE_NOT_INTIATED))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s of type DPC, but core in handle is not initialized\n",
                         table->table_name);
        }
    }
    if (actions_info->action_validation_flags & ALL_KEY_FIELDS_MUST_EXIST)
    {
        /** for tcam/LPM tables it is not mandatory to set all key fields */
        if ((table->table_type != DBAL_TABLE_TYPE_TCAM) && (table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT) &&
            (table->table_type != DBAL_TABLE_TYPE_LPM))
        {
            if (entry_handle->nof_key_fields != table->nof_key_fields)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "ilegal number of key fields for action %s in table %s, requested %d set %d\n",
                             actions_info->name, table->table_name, table->nof_key_fields,
                             entry_handle->nof_key_fields);
            }
        }
    }
/** validations of the allocator is done only in the internal code */

    /** for TCAM tables need to check that the access ID was set */
    if ((table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
        && (action != DBAL_ACTION_TABLE_CLEAR)
        && (action != DBAL_ACTION_ENTRY_GET_ACCESS_ID) && (table->access_method != DBAL_ACCESS_METHOD_KBP))
    {
        if (entry_handle->access_id_set == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tcam Tables must set HW_ACCESS_ID table %s\n", table->table_name);
        }
    }

    if (actions_info->action_validation_flags & RESULT_FIELD_MUST_EXIST)
    {
        if (entry_handle->nof_result_fields == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No value fields where set, cannot perform the action %s on table %s\n",
                         actions_info->name, table->table_name);
        }
    }

    if (actions_info->action_validation_flags & VALIDATE_LIF_RESULT_TYPE)
    {
        if ((dbal_physical_table_is_in_lif(entry_handle->table->physical_db_id[0])) ||
            (dbal_logical_table_is_out_rif_allocator(entry_handle->table)) ||
            (dbal_logical_table_is_out_lif_allocator_eedb(entry_handle->table)))
        {
                        /**
			 * In case of inlif, outlif tables, validate that result type is set prior to action
			 * Validate the result type value against lif mapping sw state table
			 */
            uint8 is_allocated = FALSE;

            if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "result type field was not set. for lif tables it should be set for every action. table %s\n",
                             entry_handle->table->table_name);
            }

            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_is_allocated
                            (unit, entry_handle->core_id, 0, entry_handle->phy_entry.key[0], entry_handle->table_id,
                             entry_handle->cur_res_type, entry_handle->table->keys_info[0].field_id, 1, &is_allocated));
            if (!is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "%s-lif is not allocated for table %s with result type %s\n",
                             entry_handle->table->keys_info[0].field_id == DBAL_FIELD_OUT_LIF ? "Out" : "In",
                             entry_handle->table->table_name, DBAL_RES_INFO.result_type_name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_action_prints(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e action)
{
    int log_enabled;
    CONST dbal_logger_info_t *logger_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logger_info_get(unit, &logger_info));

    if (!(flags & DBAL_COMMIT_DISABLE_ACTION_PRINTS))
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            log_enabled = bsl_check(bslLayerSocdnx, bslSourceDbaldnx, bslSeverityInfo, unit);
            if (log_enabled || (logger_info->dbal_file != NULL))
            {
                char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE] = { 0 };
                DBAL_PRINT_FRAME_FOR_API_PRINTS(TRUE, bslSeverityInfo);
                /** header for file dump */
                DBAL_DUMP("\n", 0, logger_info->dbal_file);

                if (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM ||
                    entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                {
                    sal_sprintf(buffer, "Action %s: table %s, acces_id %d (handle ID %d)\n",
                                dbal_actions_info[action].name, entry_handle->table->table_name,
                                entry_handle->phy_entry.entry_hw_id, entry_handle->handle_id);
                }
                else
                {
                    sal_sprintf(buffer, "Action %s: table %s, (handle ID %d)\n", dbal_actions_info[action].name,
                                entry_handle->table->table_name, entry_handle->handle_id);
                }

                DBAL_DUMP(buffer, log_enabled, logger_info->dbal_file);

                if (action != DBAL_ACTION_TABLE_CLEAR)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_print(unit, entry_handle, 0));
                }

                DBAL_DUMP("\n", 0, logger_info->dbal_file);
                DBAL_PRINT_FRAME_FOR_API_PRINTS(FALSE, bslSeverityInfo);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief Updates the action and the entry_handle According to
 *  the input
 */
static shr_error_e
dbal_action_entry_commit_parameters_update(
    int unit,
    dbal_entry_handle_t ** entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e * action)
{
    dbal_logical_table_t *table;
    dbal_entry_action_flags_e unique_action_flag_set = 0;

    SHR_FUNC_INIT_VARS(unit);

    table = (*entry_handle)->table;

    if ((*entry_handle)->nof_ranged_fields > 0)
    {
        (*action) = DBAL_ACTION_ENTRY_COMMIT_RANGE;
    }

    /*
     * Find given flag and handle accordingly
     */
    if (flags & DBAL_COMMIT)
    {
        unique_action_flag_set |= DBAL_COMMIT;
    }
    if (flags & DBAL_COMMIT_UPDATE)
    {
        if (dbal_tables_is_non_direct(unit, (*entry_handle)->table_id) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "COMMIT_UPDATE flag is not valid for direct tables. Table %s\n",
                         table->table_name);
        }
        (*action) = DBAL_ACTION_ENTRY_COMMIT_UPDATE;
        unique_action_flag_set |= DBAL_COMMIT_UPDATE;
    }
    if (flags & DBAL_COMMIT_FORCE)
    {
        if (dbal_tables_is_non_direct(unit, (*entry_handle)->table_id) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "COMMIT_FORCE flag is not valid for direct tables. Table %s\n",
                         table->table_name);
        }
        (*action) = DBAL_ACTION_ENTRY_COMMIT_FORCE;
        unique_action_flag_set |= DBAL_COMMIT_FORCE;
    }
    /*
     * From this point begin the flags that can be added to commit flags.
     */
    if (flags & DBAL_COMMIT_OVERRIDE_DEFAULT)
    {
        if ((table->table_type != DBAL_TABLE_TYPE_TCAM) && (table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT) &&
            (table->access_method != DBAL_ACCESS_METHOD_MDB))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "COMMIT_OVERRIDE_DEFAULT_ENRTY flag is only valid for MDB or TCAM. Table %s\n",
                         table->table_name);
        }
        SHR_IF_ERR_EXIT(dbal_entry_default_values_add(unit, *entry_handle));
    }
    if (flags & DBAL_COMMIT_OVERRUN)
    {
        dbal_status_e status;
        SHR_IF_ERR_EXIT(dbal_status_get(unit, &status));
        if (status != DBAL_STATUS_DEVICE_INIT_DONE)
        {
            (*entry_handle)->overrun_entry = 1;
        }
    }

    /*
     * TCAM must receive a full payload
     */
    if ((table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT) &&
        !(flags & DBAL_COMMIT_OVERRIDE_DEFAULT))
    {
        if ((*entry_handle)->nof_result_fields !=
            table->multi_res_info[(*entry_handle)->cur_res_type].nof_result_fields)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Some result fields were not set. Use flag DBAL_COMMIT_OVERRIDE_DEFAULT. table: %s\n",
                         table->table_name);
        }
    }

    /*
     * Expect only one of the unique commit flags that affect the action.
     */
    if (!(flags & DBAL_COMMIT_IGNORE_ALLOC_ERROR))
    {
        uint32 nof_unique_flags = utilex_nof_on_bits_in_long(unique_action_flag_set);
        if (nof_unique_flags <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Entry commit expects at least one of the following flags:"
                         "COMMIT, COMMIT_UPDATE_ENTRY or COMMIT_FORCE_ENTRY. Flags combination is %u.\n", flags);
        }
        if (nof_unique_flags > 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Entry commit expects no more thant one of the following flags:"
                         "COMMIT, COMMIT_UPDATE_ENTRY or COMMIT_FORCE_ENTRY. Flags combination is %u.\n", flags);
        }
    }

    
    if (((*entry_handle)->nof_ranged_fields > 0) && ((*action) != DBAL_ACTION_ENTRY_COMMIT_RANGE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Ranged commit with UPDATE or FORCE flags currently not supported. "
                     "Flag combination %u. Offending flag %u.\n", flags, unique_action_flag_set);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_action_skip_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e * action)
{
    dbal_status_e status;
    dbal_table_status_e table_status;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_status_get(unit, &status));

    table = entry_handle->table;

    /** Skip in case of incompatible device image */
    if ((table->access_method == DBAL_ACCESS_METHOD_MDB) || (table->access_method == DBAL_ACCESS_METHOD_KBP))
    {
        if (table->mdb_image_type == DBAL_MDB_IMG_STD_1_NOT_ACTIVE)
        {
            if (status == DBAL_STATUS_DEVICE_INIT_DONE)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Operation skipped table %s is not active in image\n\n"),
                           table->table_name));
            }
            (*action) = DBAL_ACTION_SKIP;
            SHR_EXIT();
        }
    }
    /** Skip in case of HW Err*/
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, entry_handle->table_id, &table_status));
    if (table_status == DBAL_TABLE_HW_ERROR)
    {
        if (status == DBAL_STATUS_DEVICE_INIT_DONE)
        {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Operation skipped table %s due to HW issues\n\n"), table->table_name));
        }
        (*action) = DBAL_ACTION_SKIP;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_action_start(
    int unit,
    uint32 handle_id,
    dbal_entry_handle_t ** entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e * action)
{
    dbal_logical_table_t *table;
    CONST dbal_action_info_t *actions_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, handle_id, entry_handle));
    table = (*entry_handle)->table;

    SHR_IF_ERR_EXIT(dbal_action_skip_check(unit, (*entry_handle), action));
    if ((*action) == DBAL_ACTION_SKIP)
    {
        SHR_EXIT();
    }

    /*
     * Changes to swstate in this function propose difficulties and are insignificant for rollback and comparison journals.
     * Should also not trigger "sw access outside of transaction violtion" for the ER regression algorithm for the same reason.
     */
    DBAL_IF_ERR_EXIT_WITH_ER_SUPPRESS(dbal_logger_table_internal_lock(unit, (*entry_handle)->table_id));

    if (((*action) == DBAL_ACTION_ENTRY_GET) || ((*action)== DBAL_ACTION_ENTRY_GET_ALL) || ((*action)== DBAL_ACTION_ENTRY_GET_ACCESS_ID) || 
        ((*action) == DBAL_ACTION_ITERATOR_GET))
    {
        DBAL_IF_ERR_EXIT_WITH_ER_SUPPRESS(DBAL_SW_STATE_TBL_PROP.
                                          nof_get_operations.inc(unit, (*entry_handle)->table_id, 1));
    }
    else if ((*action) != DBAL_ACTION_SKIP)
    {
        DBAL_IF_ERR_EXIT_WITH_ER_SUPPRESS(DBAL_SW_STATE_TBL_PROP.
                                          nof_set_operations.inc(unit, (*entry_handle)->table_id, 1));
    }
    
    actions_info = &dbal_actions_info[(*action)];

    if ((flags) && (flags & (~(actions_info->supported_flags))))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "flags combination %u is not supported for action %s\n",
                     flags, actions_info->name);
    }

    if ((*action != DBAL_ACTION_TABLE_CLEAR) && (*action != DBAL_ACTION_ENTRY_GET_ACCESS_ID))
    {
                /** Expect at least one flag */
        if (flags == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "No flags were set. At least one flag must be set.\n");
        }

                /** Update action & entry_handle According to the flags and other parameters */
        switch (*action)
        {
            case (DBAL_ACTION_ENTRY_GET):
                if (flags & DBAL_GET_ALL_FIELDS)
                {
                    (*entry_handle)->get_all_fields = TRUE;
                    (*action) = DBAL_ACTION_ENTRY_GET_ALL;
                }
                break;

            case (DBAL_ACTION_ENTRY_CLEAR):
                if (flags & DBAL_COMMIT)
                {
                    if ((*entry_handle)->nof_ranged_fields > 0)
                    {
                        (*action) = DBAL_ACTION_ENTRY_CLEAR_RANGE;
                    }
                }
                break;

            case (DBAL_ACTION_ENTRY_COMMIT):
                SHR_IF_ERR_EXIT(dbal_action_entry_commit_parameters_update(unit, entry_handle, flags, action));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL received unexpected action: %u\n", *action);
                break;
        }
    }

    actions_info = &dbal_actions_info[(*action)];

    if (actions_info->is_pre_acces_prints == PRE_ACCESS_PRINTS_ENABLED)
    {
        SHR_IF_ERR_EXIT(dbal_action_prints(unit, (*entry_handle), flags, (*action)));
    }

    SHR_IF_ERR_EXIT(dbal_action_validate(unit, (*entry_handle), *action, flags));

    /** for LPM tables need to check that the mask format is 1....10....0 */
    if ((table->table_type == DBAL_TABLE_TYPE_LPM) && (*action != DBAL_ACTION_TABLE_CLEAR))
    {
        SHR_IF_ERR_EXIT(dbal_lpm_mask_to_prefix_length(unit, (*entry_handle)));
    }

    /** setting the core_id in  */
    if (((*action) == DBAL_ACTION_ENTRY_GET) && (flags & DBAL_COMMIT_VALIDATE_OTHER_CORE))
    {
        if (table->core_mode == DBAL_CORE_MODE_DPC)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot use COMMIT_VALIDATE_OTHER_CORE in DPC table %s\n", table->table_name);
        }
        (*entry_handle)->core_id = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_actions_access_commit_consecutive_range(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_WRITE, DBAL_ACTION_ACCESS_FUNC_COMMIT_CONSEC));

    table = entry_handle->table;

    
    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            /** SHR_IF_ERR_EXIT(dbal_mdb_table_range_entry_add(unit, entry_handle, num_of_entries));*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM CS consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_range_entry_set(unit, entry_handle, num_of_entries));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            /** SHR_IF_ERR_EXIT(dbal_mdb_table_range_entry_add(unit, entry_handle, num_of_entries));*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SW consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            /** SHR_IF_ERR_EXIT(dbal_pemla_table_range_entry_add(unit, entry_handle, num_of_entries));*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "pemla consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_KBP:
            /** SHR_IF_ERR_EXIT(dbal_kbp_range_entry_add(unit, entry_handle, num_of_entries));*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP consecutive mode not supported yet \n");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, table->access_method));
            break;
    }

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                   DBAL_ACTION_ACCESS_FUNC_COMMIT_CONSEC, SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_actions_access_clear_consecutive_range(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_WRITE, DBAL_ACTION_ACCESS_FUNC_CLEAR_CONSEC));

    table = entry_handle->table;


    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            /** SHR_IF_ERR_EXIT(dbal_mdb_table_range_entry_add(unit, entry_handle, num_of_entries));*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM CS consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_range_entry_clear(unit, entry_handle, num_of_entries));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            /** SHR_IF_ERR_EXIT(dbal_mdb_table_range_entry_clear(unit, entry_handle, num_of_entries));*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SW consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            /** SHR_IF_ERR_EXIT(dbal_pemla_table_range_entry_clear(unit, entry_handle, num_of_entries));*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "pemla consecutive mode not supported yet \n");
            break;

        case DBAL_ACCESS_METHOD_KBP:
            /** SHR_IF_ERR_EXIT(dbal_kbp_range_entry_clear(unit, entry_handle, num_of_entries));*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP consecutive mode not supported yet \n");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, table->access_method));
            break;
    }

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                   DBAL_ACTION_ACCESS_FUNC_CLEAR_CONSEC, SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_result_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_READ, DBAL_ACTION_ACCESS_FUNC_RESULT_TYPE_RES));

    if (!entry_handle->table->has_result_type)
    {
        entry_handle->cur_res_type = 0;
        SHR_EXIT();
    }

    if (entry_handle->cur_res_type > DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        /**
    	 * Already set, no need to resolve it
    	 * This flow can happen when setting the result type that we expect to get and still call 'get_all_fields'
    	 */
        SHR_EXIT();
    }

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            if (entry_handle->table->result_type_mapped_to_sw)
            {
                SHR_SET_CURRENT_ERR(dbal_mdb_res_type_resolution(unit, entry_handle, TRUE));
            }
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_SET_CURRENT_ERR(dbal_hl_res_type_resolution(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_SET_CURRENT_ERR(dbal_sw_res_type_resolution(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type resolution is not supported for access method %s\n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;
    }

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_READ,
                                   DBAL_ACTION_ACCESS_FUNC_RESULT_TYPE_RES, SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_entry_commit(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_handle)
{
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, get_handle, DBAL_ACTION_ACCESS_TYPE_WRITE,
                     DBAL_ACTION_ACCESS_FUNC_ENTRY_COMMIT));

    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_add(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_add(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_entry_set(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_set(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_entry_set(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_entry_add(unit, entry_handle));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, table->access_method));
            break;
    }

    if (entry_handle->is_entry_update == FALSE)
    {
        /** Update entry counter */
        SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, 1, TRUE));
    }

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                   DBAL_ACTION_ACCESS_FUNC_ENTRY_COMMIT, SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle)
{
    int is_get_on_new_entry = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (get_entry_handle != NULL)
    {
        is_get_on_new_entry = 1;
        /** Create A copy of the handle */
        sal_memcpy(get_entry_handle, entry_handle, sizeof(dbal_entry_handle_t));
        /** deleting the result info from the payload buffer to receive new info from the get operation */
        sal_memset(&get_entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
        sal_memset(&get_entry_handle->phy_entry.p_mask, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
        if (entry_handle->table->has_result_type)
        {
            /** incase we are getting an entry for most of the cases we need to resolve the result type the usual way. because the
             *  result type not must to be equal to the added entry result type. for LIF MDB tables we don't allow to change the
             *  result type, so it means that the resoult is equal to the added entry or the entry doesn't exists. it is only
             *  important to resolve the result type for MDB tables that mapped to sw because otherwise the entry get will be done
             *  according to the result type of the entry and it will indicate that the entry exists also in cases that it is
             *  not.for other tables there is resource allocation or MDB knows to handle the case */
            if (entry_handle->table->access_method != DBAL_ACCESS_METHOD_MDB
                || entry_handle->table->result_type_mapped_to_sw)
            {
                get_entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;
            }
        }
        get_entry_handle->get_all_fields = 1;
    }
    else
    {
        /** in this case we want to perform get on the main handle */
        get_entry_handle = entry_handle;
    }

    SHR_SET_CURRENT_ERR(dbal_actions_access_result_type_resolution(unit, get_entry_handle));

    /*
     * possible mismatch between result type and value field ids for tables with multiple result types.
     * repopulate the proper value ids and the handle fields.
     */
    if (is_get_on_new_entry && get_entry_handle->cur_res_type != DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_SET_CURRENT_ERR(dbal_entry_get_handle_update_value_field(unit, get_entry_handle));
    }

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, get_entry_handle, DBAL_ACTION_ACCESS_TYPE_READ,
                     DBAL_ACTION_ACCESS_FUNC_ENTRY_GET));

    if (!SHR_FUNC_ERR())
    {
        /** Check if entry exists */
        switch (entry_handle->table->access_method)
        {
            case DBAL_ACCESS_METHOD_MDB:
                SHR_SET_CURRENT_ERR(dbal_mdb_table_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_TCAM_CS:
                SHR_SET_CURRENT_ERR(dbal_tcam_cs_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_HARD_LOGIC:
                SHR_SET_CURRENT_ERR(dbal_hl_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_SW_STATE:
                SHR_SET_CURRENT_ERR(dbal_sw_table_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_PEMLA:
                SHR_SET_CURRENT_ERR(dbal_pemla_table_entry_get(unit, get_entry_handle));
                break;

            case DBAL_ACCESS_METHOD_KBP:
                SHR_SET_CURRENT_ERR(dbal_kbp_entry_get(unit, get_entry_handle));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                             dbal_access_method_to_string(unit, entry_handle->table->access_method));
                break;
        }
    }

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_READ, DBAL_ACTION_ACCESS_FUNC_ENTRY_GET,
                                   SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_entry_handle_t *get_entry_handle = NULL;
    int rv;

    uint8 post_access_required = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    table = entry_handle->table;

    /** Clear hit bit from the handle in clear operation */
    entry_handle->phy_entry.hitbit = 0;

    rv = dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle);
    if (dbal_tables_is_non_direct(unit, entry_handle->table_id))
    {
        /**  Table is non-direct - Get the entry to make sure it exists If an error occurred while getting the entry - cannot
         *   clear - the entry not exists */
        SHR_IF_ERR_EXIT(rv);
    }

    /** Update the result type in the original entry_handle for further actions */
    entry_handle->cur_res_type = get_entry_handle->cur_res_type;

    post_access_required = TRUE;
    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, get_entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE,
                     DBAL_ACTION_ACCESS_FUNC_ENTRY_CLEAR));

    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_delete(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_delete(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_entry_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_entry_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_entry_delete(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, table->access_method));
            break;
    }

    /** Update entry counter */
    SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, 1, FALSE));

exit:
    if (post_access_required)
    {
        dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                       DBAL_ACTION_ACCESS_FUNC_ENTRY_CLEAR, SHR_GET_CURRENT_ERR());
    }
    SHR_FREE(get_entry_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_WRITE, DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR));

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_table_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_table_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_clear(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_table_clear(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %s \n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;
    }

    /** Update entry counter */
    SHR_IF_ERR_EXIT(dbal_tables_entry_counter_update(unit, entry_handle, -1, FALSE));

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_WRITE,
                                   DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR, SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_SW_ONLY, DBAL_ACTION_ACCESS_FUNC_ITERATOR_INIT));

    /**  set additional parameters per table type  */
    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_table_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_iterator_init(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_table_iterator_init(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %d \n", entry_handle->table->access_method);
            break;
    }

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_SW_ONLY,
                                   DBAL_ACTION_ACCESS_FUNC_ITERATOR_INIT, SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_iterator_destroy(
    int unit,
    dbal_entry_handle_t * entry_handle)
{

    SHR_FUNC_INIT_VARS(unit);

    /** in all current cases interator destroy will not read the HW it is just SW action */
    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_SW_ONLY,
                     DBAL_ACTION_ACCESS_FUNC_ITERATOR_DESTROY));

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_deinit(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_iterator_deinit(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        case DBAL_ACCESS_METHOD_SW_STATE:
            /** Nothing to do, NOT missing implementation */
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_iterator_deinit(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_table_iterator_deinit(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %d \n", entry_handle->table->access_method);
            break;
    }

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_SW_ONLY,
                                   DBAL_ACTION_ACCESS_FUNC_ITERATOR_DESTROY, SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}

/**
 *  \brief Get next internal is the API which read the entry from each access type (except DMA)
 *  It is called from get_next and action commit which also can perform some actions in the enrty
 */
shr_error_e
dbal_actions_access_iterator_get_next(
    int unit,
    uint32 entry_handle_id,
    int *is_end,
    uint8 *continue_iterating)
{
    dbal_entry_handle_t *entry_handle = NULL;
    uint8 post_access_required = FALSE;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    (*is_end) = 0;

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (!iterator_info->is_init)
    {
        SHR_ERR_EXIT(_SHR_E_EMPTY, "Trying to get an entry with uninitialized iterator\n");
    }

    post_access_required = TRUE;
    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_READ,
                     DBAL_ACTION_ACCESS_FUNC_ITERATOR_GET_NEXT));

    DBAL_IF_ERR_EXIT_WITH_ER_SUPPRESS(DBAL_SW_STATE_TBL_PROP.nof_get_operations.inc(unit, (entry_handle)->table_id, 1));

    /*
     * Setting the payload size to max size -
     * we already do it iterator init, but when working with actions, it might be changed.
     */
    entry_handle->phy_entry.payload_size = entry_handle->table->max_payload_size;
    entry_handle->phy_entry.hitbit = 0;

    /** in case hit_bit_rule is valid, set the action hit bit get only for non_flush machine cases */
    if(iterator_info->hit_bit_rule_valid)
    {
        /* FWD_MACT and FWD_MACT_IVL are running through flush machine, so setting the get hit rule only when flush machine is ended */
        if((entry_handle->table_id != DBAL_TABLE_FWD_MACT) && (entry_handle->table_id != DBAL_TABLE_FWD_MACT_IVL))
        {
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, iterator_info->hit_bit_rule_type, NULL));
        }
        else
        {
            if(iterator_info->mdb_iterator.start_non_flush_iteration)
            {
                SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, iterator_info->hit_bit_rule_type, NULL));
            }
        }
    }

    /** in case hit_bit_rule is valid, set the action hit bit get only for non_flush machine cases */
    if(iterator_info->hit_bit_action_get)
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, iterator_info->hit_bit_action_get_type, NULL));
    }

    /** in case hit_bit_rule is valid, set the action hit bit get only for non_flush machine cases */
    if(iterator_info->hit_bit_action_clear)
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, iterator_info->hit_bit_action_clear_type, NULL));
    }

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_tcam_cs_entry_get_next(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_hl_entry_get_next(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_next(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_table_iterator_get_next(unit, entry_handle));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            SHR_IF_ERR_EXIT(dbal_kbp_table_entry_get_next(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %d \n", entry_handle->table->access_method);
            break;
    }

    if (!iterator_info->is_end)
    {
        uint8 is_value_rules_pass = TRUE;
        entry_handle->handle_status = DBAL_HANDLE_STATUS_ACTION_PREFORMED;
        if (entry_handle->table->has_result_type)
        {
            int ii;
            /** setting all the entry handle value fields according to the result type   */
            for (ii = 0; ii < DBAL_RES_INFO.nof_result_fields; ii++)
            {
                entry_handle->value_field_ids[ii] = DBAL_RES_INFO.results_info[ii].field_id;
            }
            entry_handle->nof_result_fields = DBAL_RES_INFO.nof_result_fields;
        }
        if (((iterator_info->hit_bit_rule_valid) || (iterator_info->nof_val_rules > 0)) &&
            (entry_handle->phy_entry.mdb_action_apply != DBAL_MDB_ACTION_APPLY_SW_SHADOW) &&
            (entry_handle->phy_entry.mdb_action_apply != DBAL_MDB_ACTION_APPLY_NONE))
        {
            SHR_IF_ERR_EXIT(dbal_key_or_value_condition_check(unit, entry_handle, FALSE,
                                                              iterator_info->val_rules_info,
                                                              iterator_info->nof_val_rules, &is_value_rules_pass));
        }

        if (!is_value_rules_pass)
        {
            /** Entry is not satisfying the rules, indicate we need to get the next entry */
            *continue_iterating = TRUE;
            SHR_EXIT();
        }

        /** In case of LPM table, convert the prefix_length to mask */
        if (entry_handle->table->table_type == DBAL_TABLE_TYPE_LPM)
        {
            SHR_IF_ERR_EXIT(dbal_lpm_prefix_length_to_mask(unit, entry_handle));
        }
        iterator_info->entries_counter++;
    }
    else
    {
        (*is_end) = 1;
        SHR_EXIT();
    }

exit:
    if (post_access_required)
    {
        dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_READ,
                                       DBAL_ACTION_ACCESS_FUNC_ITERATOR_GET_NEXT, SHR_GET_CURRENT_ERR());
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_actions_access_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_access_pre_access
                    (unit, entry_handle, NULL, DBAL_ACTION_ACCESS_TYPE_READ,
                     DBAL_ACTION_ACCESS_FUNC_ITERATOR_ACCESS_ID_GET));

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            rv = dbal_mdb_table_access_id_by_key_get(unit, entry_handle);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT_NO_MSG(rv);
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            break;

        case DBAL_ACCESS_METHOD_KBP:
            rv = dbal_kbp_access_id_by_key_get(unit, entry_handle);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT_NO_MSG(rv);
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            break;
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        case DBAL_ACCESS_METHOD_SW_STATE:
        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "API not support for access type %s \n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown access method %d \n", entry_handle->table->access_method);
            break;
    }

exit:
    dbal_action_access_post_access(unit, entry_handle, DBAL_ACTION_ACCESS_TYPE_READ,
                                   DBAL_ACTION_ACCESS_FUNC_ITERATOR_ACCESS_ID_GET, SHR_GET_CURRENT_ERR());
    SHR_FUNC_EXIT;
}
