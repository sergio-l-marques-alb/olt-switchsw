/** \file kbp_mngr_internal.c
 *
 *  kbp utility procedures for DNX that allows to configure the KBP dynamiclly.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_KBP_MNGR

/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <shared/util.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dbal/dbal_string_apis.h>

#include "kbp_mngr_internal.h"
/*
 * }
 */
#if defined(INCLUDE_KBP)
/*
 * DEFINEs
 * {
 */

/*
 * }
 */
/*
 * GLOBALs
 * {
 */

/*
 * }
 */
/*
 * FUNCTIONs
 * {
 */

extern shr_error_e dbal_field_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    multi_res_info_t * result_info,
    uint8 is_full_buffer,
    dbal_fields_e parent_field,
    int size,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);

/**
 * See kbp_mngr_db_id_by_name_get() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_db_id_by_name_get_internal(
    int unit,
    char *name,
    uint8 *db_id)
{
    uint32 entry_handle_id;
    int is_end;
    int db_index = -1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        char get_name[DBAL_MAX_STRING_LENGTH];

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_DB_NAME, INST_SINGLE, (uint8 *) get_name));
        if (sal_strcmp(name, get_name) == 0)
        {
            uint8 rec_db_id;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_DB_ID, &rec_db_id));
            db_index = rec_db_id;
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (db_index == -1)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    *db_id = db_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See kbp_mngr_db_capacity_get() in kbp_mngr.h
*/
shr_error_e
kbp_mngr_db_capacity_get_internal(
    int unit,
    uint8 db_id,
    uint32 *nof_entries,
    uint32 *estimated_capacity)
{
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);
    KBP_COMPILATION_CHECK;

    if ((nof_entries == NULL) && (estimated_capacity == NULL))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status))
    {
        kbp_db_handles_t db_handles;
        struct kbp_db_stats db_stats = { 0 };

        SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &db_handles));
        DNX_KBP_TRY(kbp_db_stats(db_handles.db_p, &db_stats));

        if (nof_entries)
        {
            *nof_entries = db_stats.num_entries;
        }
        if (estimated_capacity)
        {
            *estimated_capacity = db_stats.capacity_estimate;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_opcode_total_result_size_get() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_opcode_total_result_size_get_internal(
    int unit,
    uint8 opcode_id,
    uint32 *result_size)
{
    int index;
    uint32 entry_handle_id;
    uint32 res_entry_handle_id;
    uint8 is_valid;
    uint32 result_bmp = 0;
    uint32 last_ad_width = 0;
    uint32 last_result_offset = 0;
    uint8 last_valid_result = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(result_size, _SHR_E_PARAM, "result_size");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

    if (!is_valid)
    {
        *result_size = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));

    for (index = 0; index < DNX_KBP_MAX_NOF_LOOKUPS; index++)
    {
        if (utilex_bitstream_test_bit(&result_bmp, index))
        {
            last_valid_result = index;
        }
    }

    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, last_valid_result);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &last_result_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &last_ad_width));

    *result_size = last_result_offset + last_ad_width;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_opcode_lookup_add() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_opcode_lookup_add_internal(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info)
{
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 res_entry_handle_id;
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    uint32 opcode_result_size = 0;
    uint32 valid_db_result_size = 0;
    uint32 valid_segments = 0;
    int index;
    uint8 lookup_id;
    uint8 prev_res_size = 0;
    uint8 prev_res_offset = 0;
    uint8 next_res_offset = DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS - DNX_KBP_MAX_NOF_RESULTS;
    uint8 kbp_db_id = 0;
    uint8 nof_public_lookups;
    const dbal_logical_table_t *table;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED();

    SHR_NULL_CHECK(lookup_info, _SHR_E_PARAM, "lookup_info");

    /** Validate result index */
    if (lookup_info->result_index >= DNX_KBP_MAX_NOF_RESULTS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_id %d is out of range\n", lookup_info->result_index);
    }

    /** Validate offset is byte aligned */
    if ((lookup_info->result_offset % 8) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result offset %d is not byte aligned\n", lookup_info->result_offset);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_PUBLIC_LOOKUPS, INST_SINGLE, &nof_public_lookups));

    /** Validate result availability */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));
    if (utilex_bitstream_test_bit(&result_bmp, lookup_info->result_index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_id %d is already set\n", lookup_info->result_index);
    }
    /** Validate lookup index */
    lookup_id = lookup_info->result_index + nof_public_lookups;
    if (lookup_id >= DNX_KBP_MAX_NOF_LOOKUPS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_id %d is not available due to existing public lookups\n",
                     lookup_info->result_index);
    }
    /** Update lookup bitmap */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));
    utilex_bitstream_set_bit(&lookup_bmp, lookup_id);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, lookup_info->dbal_table_id, &table));
    /** get the KBP DB ID using the DBAL table name */
    SHR_IF_ERR_EXIT(kbp_mngr_db_id_by_name_get(unit, (char *) table->table_name, &kbp_db_id));
    /*
     * If the result size  is 0, get the result size from the DBAL table.
     * Otherwise, use the result size.
     */
    if (lookup_info->result_size == 0)
    {
        opcode_result_size = table->max_payload_size;
    }
    else
    {
        opcode_result_size = lookup_info->result_size;
    }
    /** Use dnx_kbp_valid_result_width to verify that the size is within range.*/
    SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, opcode_result_size, &valid_db_result_size));

    /** Get previous result size and offset */
    if (lookup_info->result_index > 0)
    {
        for (index = (lookup_info->result_index - 1); index >= 0; index--)
        {
            if (utilex_bitstream_test_bit(&result_bmp, index))
            {
                dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, (uint8) index);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &prev_res_size));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &prev_res_offset));
                break;
            }
        }
    }

    /** Get next result offset */
    for (index = (lookup_info->result_index + 1); index < DNX_KBP_MAX_NOF_RESULTS; index++)
    {
        if (utilex_bitstream_test_bit(&result_bmp, index))
        {
            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, (uint8) index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &next_res_offset));
            break;
        }
    }

    /** Validate offset and result placement */
    if (lookup_info->result_offset < (prev_res_offset + prev_res_size)
        || lookup_info->result_offset + opcode_result_size > next_res_offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Result overlaps with other results or is out of the buffer bounds; "
                     "Result placement [%d:%d]; Available placement [%d:%d]\n",
                     lookup_info->result_offset, lookup_info->result_offset + opcode_result_size - 1,
                     (prev_res_offset + prev_res_size), (next_res_offset - 1));
    }
    /** Update result bitmap */
    utilex_bitstream_set_bit(&result_bmp, lookup_info->result_index);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &valid_segments));

    if (lookup_info->nof_segments > valid_segments)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nof_segments %d exceeds the available number of segments %d in the opcode (%d)\n",
                     lookup_info->nof_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP, opcode_id);
    }

    /** Update lookup info */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_set
                    (unit, opcode_id, lookup_id, DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_ACL, kbp_db_id,
                     lookup_info->result_index, lookup_info->nof_segments, lookup_info->key_segment_index));

    /** Update result info */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_result_set
                    (unit, opcode_id, lookup_info->result_index, lookup_info->result_offset, opcode_result_size));

    /** Update opcode info */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, lookup_bmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, result_bmp);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * If the device is locked, update status to indicate that the SW has been changed after device lock
     * and that sync need to be redone in order to apply the updated configurations.
     * Currently it's not allowed to perform sync and lock more than once
     * and trying to apply new configurations after device lock will return error.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_opcode_lookup_get() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_opcode_lookup_get_internal(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info)
{
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 res_entry_handle_id;
    uint32 db_entry_handle_id;
    uint8 valid = 0;
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    int index;
    uint8 lookup_id;
    uint8 kbp_db_id = 0;
    uint32 dbal_table_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lookup_info, _SHR_E_PARAM, "lookup_info");

    if (lookup_info->result_index >= DNX_KBP_MAX_NOF_RESULTS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_id %d is out of range\n", lookup_info->result_index);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &valid));

    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "opcode %d is not valid\n", opcode_id);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));

    if (!utilex_bitstream_test_bit(&result_bmp, lookup_info->result_index))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "result_id %d is not configured\n", lookup_info->result_index);
    }

    /** Get the lookup_id for this result */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));
    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    for (lookup_id = 0; lookup_id < DNX_KBP_MAX_NOF_LOOKUPS; lookup_id++)
    {
        if (utilex_bitstream_test_bit(&lookup_bmp, lookup_id))
        {
            uint8 get_result_id;

            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, &get_result_id));

            if (lookup_info->result_index == get_result_id)
            {
                break;
            }
        }
    }

    if (lookup_id >= DNX_KBP_MAX_NOF_LOOKUPS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No valid lookup was pointing to result_id %d\n", lookup_info->result_index);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &kbp_db_id));

    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, kbp_db_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** In case of dummy lookup there is no DBAL table info to be returned. */
    if (kbp_db_id != DNX_KBP_DUMMY_HOLE_DB_ID)
    {
        dbal_access_method_e access_method;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table_id));

        SHR_IF_ERR_EXIT(dbal_tables_access_method_get(unit, dbal_table_id, &access_method));

        if (access_method != DBAL_ACCESS_METHOD_KBP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal table %d retrieved using KBP DB %d is not KBP\n", dbal_table_id,
                         kbp_db_id);
        }
        /** Retrieve the lookup info */
        lookup_info->dbal_table_id = dbal_table_id;
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, lkp_entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE,
                     &lookup_info->nof_segments));

    for (index = 0; index < lookup_info->nof_segments; index++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, index,
                         &lookup_info->key_segment_index[index]));
    }

    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, lookup_info->result_index);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &lookup_info->result_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &lookup_info->result_size));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_opcode_result_clear() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_opcode_result_clear_internal(
    int unit,
    uint8 opcode_id,
    uint8 result_id)
{
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 res_entry_handle_id;
    uint32 valid = 0;
    uint8 lookup_id;
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED();

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));

    if (result_id >= DNX_KBP_MAX_NOF_RESULTS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_id %d is out of range\n", result_id);
    }

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &valid));

    if (valid)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));

        /** Update the lookup if result is set */
        if (utilex_bitstream_test_bit(&result_bmp, result_id))
        {
            /** Find and clear the lookup entries that are pointing to this result */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));
            dbal_entry_key_field32_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            for (lookup_id = 0; lookup_id < DNX_KBP_MAX_NOF_LOOKUPS; lookup_id++)
            {
                if (utilex_bitstream_test_bit(&lookup_bmp, lookup_id))
                {
                    uint8 get_result_id;

                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_id);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, &get_result_id));

                    if (result_id == get_result_id)
                    {
                        utilex_bitstream_reset_bit(&lookup_bmp, lookup_id);
                        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, lkp_entry_handle_id, DBAL_COMMIT));
                    }
                }
            }

            /** Update the opcode result and lookup bitmaps */
            utilex_bitstream_reset_bit(&result_bmp, result_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, result_bmp);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, lookup_bmp);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Clear the result entry */
            dbal_entry_key_field32_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field32_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, result_id);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, res_entry_handle_id, DBAL_COMMIT));

            /*
             * If the device is locked, update status to indicate that the SW has been changed after device lock
             * and that sync need to be redone in order to apply the updated configurations.
             * Currently it's not allowed to perform sync and lock more than once
             * and trying to apply new configurations after device lock will return error.
             */
            SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
            if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
            {
                SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Validate the names and sizes of the provided master key segments.
*   \param [in] unit          - Relevant unit.
*   \param [in] nof_segments  - The number of valid segments.
*   \param [in] key_segments  - Array of master key segments to be validated.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
static shr_error_e
kbp_mngr_opcode_master_key_validate(
    int unit,
    uint32 nof_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    int index;
    int index_2;
    int master_key_nof_bytes = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate the segments sizes */
    for (index = 0; index < nof_segments; index++)
    {
        if (key_segments[index].nof_bytes < 1 || key_segments[index].nof_bytes > DNX_KBP_MAX_SEGMENT_LENGTH_IN_BYTES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "New key segment (%d) \"%s\" has invalid size %d. Size must be between 1 and %d\n", index,
                         key_segments[index].name, key_segments[index].nof_bytes, DNX_KBP_MAX_SEGMENT_LENGTH_IN_BYTES);
        }
        master_key_nof_bytes += key_segments[index].nof_bytes;
    }

    /** Validate the master key does not exceed the maximum allowed size */
    if (master_key_nof_bytes > BITS2BYTES(DNX_KBP_MAX_KEY_LENGTH_IN_BITS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "New master key (%d bits) exceeds the maximum allowed size of %d bits\n",
                     BYTES2BITS(master_key_nof_bytes), DNX_KBP_MAX_KEY_LENGTH_IN_BITS);
    }
    /** If the key size exceeds 360 bits, write a debug indicating this.*/
    if (master_key_nof_bytes > BITS2BYTES(DNX_KBP_MAX_KEY_LENGTH_IN_BITS_LOSSLESS))
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "New master key (%d bits) exceeds the maximum lossless size of %d bits%s%s\n",
                     BYTES2BITS(master_key_nof_bytes), DNX_KBP_MAX_KEY_LENGTH_IN_BITS_LOSSLESS, EMPTY, EMPTY);
    }

    /** Validate master key segment names do not repeat */
    for (index = 0; index < nof_segments - 1; index++)
    {
        for (index_2 = index + 1; index_2 < nof_segments; index_2++)
        {
            if (!sal_strncmp
                (key_segments[index].name, key_segments[index_2].name, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Master key segments %d and %d have the same name \"%s\"\n", index, index_2,
                             key_segments[index].name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_opcode_master_key_set() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_opcode_master_key_set_internal(
    int unit,
    uint8 opcode_id,
    uint32 nof_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    uint32 entry_handle_id;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED();

    SHR_NULL_CHECK(key_segments, _SHR_E_PARAM, "key_segments");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (nof_segments > DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to set %d master key segments, which is more than the available %d\n",
                     nof_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_validate(unit, nof_segments, key_segments));

    /** Set the new valid master key segments */
    for (index = 0; index < nof_segments; index++)
    {
        uint32 field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM;

        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_set
                        (unit, opcode_id, index, &field_type, &(key_segments[index].nof_bytes),
                         key_segments[index].name, &(key_segments[index].overlay_offset_bytes)));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, nof_segments);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_opcode_master_key_get() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_opcode_master_key_get_internal(
    int unit,
    uint32 opcode_id,
    uint32 *nof_key_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    uint32 entry_handle_id;
    uint32 nof_valid_segments = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_key_segments, _SHR_E_PARAM, "nof_key_segments");
    SHR_NULL_CHECK(key_segments, _SHR_E_PARAM, "key_segments");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_valid_segments));

    (*nof_key_segments) = 0;
    /** Running over all the valid segments and updating the key_segments array */
    for (index = 0; index < nof_valid_segments; index++)
    {
        uint32 field_nof_bytes;
        uint32 overlay_offset;

        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_get
                        (unit, opcode_id, index, NULL, &field_nof_bytes, key_segments[(*nof_key_segments)].name,
                         &overlay_offset));

        key_segments[(*nof_key_segments)].nof_bytes = field_nof_bytes;
        key_segments[(*nof_key_segments)].overlay_offset_bytes = overlay_offset;

        (*nof_key_segments)++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_opcode_master_key_segments_add() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_opcode_master_key_segments_add_internal(
    int unit,
    uint32 opcode_id,
    uint32 nof_new_segments,
    kbp_mngr_key_segment_t * key_segments)
{
    uint32 entry_handle_id;
    uint32 nof_valid_segments = 0;
    int index;
    kbp_mngr_key_segment_t new_master_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED();

    /** No need to do anything if we don't add new segments */
    if (nof_new_segments == 0)
    {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(key_segments, _SHR_E_PARAM, "key_segments");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_valid_segments));

    if (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - nof_valid_segments < nof_new_segments)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Not enough available segments in master key %d. New number of segments to add %d\n",
                     DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - nof_valid_segments, nof_new_segments);
    }

    /** Build the entire new master key for validation */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_valid_segments, new_master_key));
    sal_memcpy(&new_master_key[nof_valid_segments], key_segments, sizeof(kbp_mngr_key_segment_t) * nof_new_segments);

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_validate(unit, nof_valid_segments + nof_new_segments, new_master_key));

    /** Running over all the new input segments and updating the master key */
    for (index = 0; index < nof_new_segments; index++)
    {
        uint32 field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM;
        if (key_segments[index].is_overlay_field == FALSE)
        {
            key_segments[index].overlay_offset_bytes = DNX_KBP_OVERLAY_FIELD_INVALID;
        }

        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_set
                        (unit, opcode_id, nof_valid_segments + index, &field_type, &(key_segments[index].nof_bytes),
                         key_segments[index].name, &(key_segments[index].overlay_offset_bytes)));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE,
                                 nof_valid_segments + nof_new_segments);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * If the device is locked, update status to indicate that the SW has been changed after device lock
     * and that sync need to be redone in order to apply the updated configurations.
     * Currently it's not allowed to perform sync and lock more than once
     * and trying to apply new configurations after device lock will return error.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Function for parsing the opcode key to printable entry
 */
shr_error_e
kbp_mngr_opcode_printable_entry_result_parsing(
    int unit,
    uint32 flags,
    uint8 opcode_id,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *kbp_entry_print_num,
    kbp_printable_entry_t * kbp_entry_print_info)
{
    int lookup_index, hitbit_index;
    uint32 lkp_entry_handle_id;
    uint32 res_entry_handle_id;
    uint32 db_entry_handle_id;
    uint32 hit_indication;
    uint32 res_buffer[BYTES2WORDS(DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BYTES)] = { 0 };
    int res_sig_size_in_words = BITS2WORDS(res_sig_size);
    uint8 nof_lookups = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Hit indication from buffer is inverted */
    hit_indication = utilex_bits_reverse_char(UTILEX_GET_BYTE_0(res_sig_value[res_sig_size_in_words - 1]));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));

    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);

    if (hit_indication == 0)
    {
        *kbp_entry_print_num = 0;
        SHR_EXIT();
    }

    /** Parsing results */
    hitbit_index = 0;
    for (lookup_index = 0; lookup_index < DNX_KBP_MAX_NOF_LOOKUPS; lookup_index++)
    {
        uint8 result_id;
        uint8 lookup_db;
        dbal_enum_value_field_kbp_lookup_type_e lookup_type;
        uint32 dbal_table_id;
        int offset = 0;
        uint32 result_offset = 0;
        uint32 result_size = 0;
        sal_memset(res_buffer, 0, sizeof(res_buffer));

        dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) lookup_index);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, &result_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &lookup_db));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, (uint32 *) &lookup_type));

        if ((lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC) ||
            (lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC))
        {
            continue;
        }

        if (lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE)
        {
            hitbit_index++;
            continue;
        }

        if ((flags == DNX_KBP_ENTRY_PARSE_FWD_ONLY) &&
            ((lookup_type != DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD)
             || (lookup_type != DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF)))
        {
            /*
             * In case ENTRY_PARSE_FWD_ONLY flag is set and we reached the first ACL we finished our scan 
             */
            break;
        }
        if (utilex_bitstream_test_bit(&hit_indication, hitbit_index))
        {
            kbp_printable_entry_t *current_kbp_entry_print_info = kbp_entry_print_info + nof_lookups;
            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, result_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &result_offset));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &result_size));

            dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, lookup_db);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table_id));

            /** take the result buffer from the correct place using also result_offset. */
            offset = res_sig_size - DNX_KBP_HIT_INDICATION_SIZE_IN_BITS - result_offset - result_size;

            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(res_sig_value, offset, result_size, res_buffer));

            SHR_IF_ERR_EXIT(dbal_table_printable_entry_get
                            (unit, dbal_table_id, NULL, DBAL_CORE_ALL, result_size, res_buffer,
                             &current_kbp_entry_print_info->entry_print_info));

            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                            (unit, "LOOKUP_TYPE", lookup_type, current_kbp_entry_print_info->lookup_type_str));
            current_kbp_entry_print_info->table_id = dbal_table_id;
            current_kbp_entry_print_info->lookup_id = lookup_index;
            nof_lookups++;
        }
        hitbit_index++;
    }
    *kbp_entry_print_num = nof_lookups;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Function for parsing the opcode key to printable entry
 */

shr_error_e
kbp_mngr_opcode_printable_entry_key_parsing(
    int unit,
    uint8 opcode_id,
    uint32 *key_sig_value,
    int key_sig_size,
    uint8 *nof_print_info,
    kbp_printable_entry_t * kbp_entry_print_info)
{
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 db_info_entry_handle_id;
    uint32 nof_master_key_segments;
    uint32 current_key_offset_bytes;
    uint32 key_size[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
    uint8 key_buffer[DNX_KBP_MAX_KEY_LENGTH_IN_BYTES] = { 0 };
    dbal_printable_field_t *printable_field;
    dbal_printable_entry_t *entry_print_info;
    uint32 lookup_bitmap = 0;
    uint8 lookup_id;
    dbal_tables_e table_id;
    const dbal_logical_table_t *table;
    uint8 seg_index;
    uint8 nof_lookups = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    utilex_U32_to_U8(key_sig_value, BITS2BYTES(key_sig_size), key_buffer);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_info_entry_handle_id));

    /** Get Opcode info */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_master_key_segments));

    /** Get Opcode lookup bitmap */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bitmap));

    for (lookup_id = 0; lookup_id < DNX_KBP_MAX_NOF_LOOKUPS; lookup_id++)
    {
        if (utilex_bitstream_test_bit(&lookup_bitmap, lookup_id))
        {
            uint8 lkp_db_id;
            uint8 lkp_nof_seg;
            uint8 lkp_seg_id, lkp_seg_index;

            entry_print_info = &kbp_entry_print_info[nof_lookups].entry_print_info;
            kbp_entry_print_info[nof_lookups].lookup_id = lookup_id;

            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &lkp_db_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &lkp_nof_seg));

           /** Get db Info*/
            dbal_entry_key_field8_set(unit, db_info_entry_handle_id, DBAL_FIELD_DB_ID, lkp_db_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_info_entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, db_info_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &table_id));
            kbp_entry_print_info[nof_lookups].table_id = table_id;
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

           /** Read all segments values */
            for (seg_index = 0; seg_index <= nof_master_key_segments; seg_index++)
            {
                SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_get
                                (unit, opcode_id, seg_index, NULL, &key_size[seg_index], NULL, NULL));

            }

            for (lkp_seg_index = 0; lkp_seg_index < lkp_nof_seg; lkp_seg_index++)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, lkp_seg_index, &lkp_seg_id));

                current_key_offset_bytes = BITS2BYTES(key_sig_size);
                for (seg_index = 0; seg_index <= lkp_seg_id; seg_index++)
                {
                    if (current_key_offset_bytes < key_size[seg_index])
                    {
                       /** Key parsing failed exit and print what we all ready parsed*/
                        SHR_EXIT();
                    }

                    current_key_offset_bytes -= key_size[seg_index];
                }

                printable_field = &entry_print_info->key_fields_info[entry_print_info->nof_key_fields];

                printable_field->field_id = table->keys_info[lkp_seg_index].field_id;
                sal_strncpy(printable_field->field_name, dbal_field_to_string(unit, printable_field->field_id),
                            DBAL_MAX_LONG_STRING_LENGTH);
                SHR_IF_ERR_EXIT(utilex_U8_to_U32
                                (&key_buffer[current_key_offset_bytes], key_size[seg_index],
                                 printable_field->field_value));
                SHR_IF_ERR_EXIT(dbal_field_printable_string_get
                                (unit, printable_field->field_id, printable_field->field_value, NULL, FALSE,
                                 DBAL_FIELD_EMPTY, BYTES2BITS(key_size[seg_index]),
                                 printable_field->field_print_value));

                entry_print_info->nof_key_fields++;
            }
            nof_lookups++;
        }
        *nof_print_info = nof_lookups;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#else
/* *INDENT-OFF* */
/** dummy prototype to return error if KBP not compiled */
shr_error_e kbp_mngr_opcode_printable_entry_result_parsing(int unit, uint32 flags, uint8 opcode_id, uint32 *res_sig_value, int res_sig_size, uint8 *kbp_entry_print_num, kbp_printable_entry_t * kbp_entry_print_info){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_printable_entry_result_parsing")}
shr_error_e kbp_mngr_opcode_printable_entry_key_parsing(int unit,uint8 opcode_id,uint32 *key_sig_value,int key_sig_size, uint8 *nof_print_info, kbp_printable_entry_t * kbp_entry_print_info){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_printable_entry_key_parsing")}
shr_error_e kbp_mngr_db_id_by_name_get_internal(int unit, char *name, uint8 *db_id){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_id_by_name_get_internal")}
shr_error_e kbp_mngr_db_capacity_get_internal(int unit, uint8 db_id, uint32 * nof_entries, uint32 * estimated_capacity){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_capacity_get_internal")}
shr_error_e kbp_mngr_opcode_total_result_size_get_internal(int unit, uint8 opcode_id, uint32 *result_size){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_total_result_size_get_internal")}

shr_error_e kbp_mngr_opcode_lookup_add_internal(int unit, uint8 opcode_id, kbp_opcode_lookup_info_t * lookup_info){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_lookup_add_internal")}
shr_error_e kbp_mngr_opcode_lookup_get_internal(int unit, uint8 opcode_id, kbp_opcode_lookup_info_t * lookup_info){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_lookup_get_internal")}

shr_error_e kbp_mngr_opcode_result_clear_internal(int unit, uint8 opcode_id, uint8 lookup_idx){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_result_clear_internal")}

shr_error_e kbp_mngr_opcode_master_key_set_internal(int unit, uint8 opcode_id, uint32 nof_segments, kbp_mngr_key_segment_t * key_segments){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_master_key_set_internal")}
shr_error_e kbp_mngr_opcode_master_key_get_internal(int unit, uint32 opcode_id, uint32 *nof_key_segments, kbp_mngr_key_segment_t * key_segments){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_master_key_get_internal")}
shr_error_e kbp_mngr_opcode_master_key_segments_add_internal(int unit, uint32 opcode_id, uint32 nof_new_segments, kbp_mngr_key_segment_t * key_segments){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_opcode_master_key_segments_add_internal")}

/* *INDENT-ON* */
#endif
