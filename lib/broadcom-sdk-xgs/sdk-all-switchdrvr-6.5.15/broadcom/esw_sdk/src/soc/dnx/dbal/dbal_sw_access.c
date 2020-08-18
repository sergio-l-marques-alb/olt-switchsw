/*
 * $Id: dbal_sw_access.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <shared/bsl.h>

#include "dbal_internal.h"
#include <soc/drv.h> 
#include <soc/dnx/dbal/dbal.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
/** \brief max key size for SW direct tables in bits */
#  define DBAL_SW_DIRECT_TABLES_MAX_KEY_SIZE_BITS   30

#define DBAL_SW_FIELD_LOG( field_id, value, offset, nof_bits)                                     \
        if(dbal_logger_is_enable(unit, entry_handle->table_id))                                       \
        {                                                                                         \
            LOG_INFO_EX(BSL_LOG_MODULE, " using Field %-30s, Vlaue %d, offset %d, nof_bits %d\n", \
                    dbal_field_to_string(unit, field_id), value, offset, nof_bits);               \
        }

/**
 * actual allocation of the SW table
 */
shr_error_e
dbal_sw_access_table_alloc(
    int unit,
    dbal_logical_table_t * table,
    dbal_tables_e table_id,
    int nof_entries,
    int result_length_in_bytes)
{
    sw_state_htbl_init_info_t hash_tbl_init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&hash_tbl_init_info, 0, sizeof(hash_tbl_init_info));
    switch (table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_LPM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.set(unit, table_id, DBAL_SW_TABLE_HASH));

            hash_tbl_init_info.max_nof_elements = nof_entries;
            hash_tbl_init_info.expected_nof_elements = nof_entries;

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.create(unit, table_id, &hash_tbl_init_info));

            break;

        case DBAL_TABLE_TYPE_DIRECT:
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.set(unit, table_id, DBAL_SW_TABLE_DIRECT));

            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown logical table type, %s. table %s\n",
                         dbal_table_type_to_string(unit, table->table_type), table->table_name);
            break;
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.entries.alloc(unit, table_id, nof_entries * result_length_in_bytes));

exit:
    SHR_FUNC_EXIT;
}

/**
 * this function runs over all the tables and allocate the SW state info if needed. for access_method == SW only
 * allocate according to the size, for HL and MDB, allocate according to the relevant fields that mapped to the SW.
 *
 * @param unit
 *
 * @return shr_error_e
 */
shr_error_e
dbal_sw_access_init(
    int unit)
{
    int ii, max_result_length_bytes = 0;
    dbal_logical_table_t *table;
    uint32 num_of_entries = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_WARM_BOOT(unit))
    {
        SHR_EXIT();
    }

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        max_result_length_bytes = 0;
        num_of_entries = 0;
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, ii, &table));

        if (table->maturity_level == DBAL_MATURITY_LOW)
        {
            continue;
        }

        switch (table->access_method)
        {
            case DBAL_ACCESS_METHOD_SW_STATE:
                max_result_length_bytes = table->sw_payload_length_bytes;
                break;

            case DBAL_ACCESS_METHOD_MDB:
                if (table->result_type_mapped_to_sw)
                {
                    max_result_length_bytes = BITS2BYTES(table->multi_res_info[0].results_info[0].field_nof_bits);
                }
                break;

            case DBAL_ACCESS_METHOD_HARD_LOGIC:
                max_result_length_bytes = table->sw_payload_length_bytes;
                break;

            default:
                break;
        }

        if (!max_result_length_bytes)
        {
            continue;
        }

        if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            if (table->key_size > DBAL_SW_DIRECT_TABLES_MAX_KEY_SIZE_BITS)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "key is too long.  current key %d bits. table %s\n",
                             table->key_size, table->table_name);
            }
            num_of_entries = utilex_power_of_2(table->key_size);
        }
        else
        {
            if (table->max_capacity == 0)
            {
                if (table->access_method == DBAL_ACCESS_METHOD_MDB)
                {

                    /**in this case the size should be taken from the HW */
                    int jj;

                    for (jj = 0; jj < table->nof_physical_tables; jj++)
                    {
                        int max_capacity;
                        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, table->physical_db_id[jj], &max_capacity));
                        num_of_entries += (uint32) max_capacity;
                    }
                    if (!num_of_entries)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR num of entries == 0  %s\n", table->table_name);
                    }

                }
                else if (table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
                {
                    dbal_db_int_or_dnx_data_info_struct_t indirect_table_size_dnx_data;
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                    (unit, &indirect_table_size_dnx_data, table->table_size_str, TRUE, 0, "", ""));

                    num_of_entries =  (uint32) indirect_table_size_dnx_data.int_val;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "max capacity == 0  %s\n", table->table_name);
                }
            }
            else
            {
                num_of_entries = table->max_capacity;
            }

            if (BITS2BYTES(table->key_size) > BITS2BYTES(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "key is too long.  max key size for sw state HASH is %u bytes. table %s\n",
                             (unsigned) BITS2BYTES(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS), table->table_name);
            }
        }

        if (max_result_length_bytes > DBAL_PHYSICAL_RES_SIZE_IN_BYTES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "table %s, result length for SW table id %d bytes exceeding the max %u bytes.\n",
                         table->table_name, max_result_length_bytes, (uint32) DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
        }

        /** allocate the table entries buffers */
        table->sw_state_nof_entries = num_of_entries;
        table->sw_payload_length_bytes = max_result_length_bytes;
        if (num_of_entries > 0)
        {
            SHR_IF_ERR_EXIT(dbal_sw_access_table_alloc(unit, table, ii, num_of_entries, max_result_length_bytes));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_sw_access_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}
/**
 * \brief
 * The function print (verbose level) an entry in SW table
 * (key+payload).
 * used for both hash and direct tables
 */
static shr_error_e
dbal_sw_table_entry_dump(
    int unit,
    dbal_tables_e table_id,
    int entry_index,
    uint8 *payload,
    int res_length,
    char *action)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logger_is_enable(unit, table_id))
    {
        LOG_CLI((BSL_META_U(unit, "Entry %s \n"), action));
        LOG_CLI((BSL_META_U(unit, "Entry index : 0x%x\n"), entry_index));
        LOG_CLI((BSL_META_U(unit, "Payload (%d bytes) : 0x"), res_length));
        for (ii = res_length - 1; ii >= 0; ii--)
        {
            LOG_CLI((BSL_META("%02x"), payload[ii]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function set an entry represented by entry_handle to a
 * specific entry index in sw state table for hl access
 */
static shr_error_e
dbal_sw_table_entry_set_in_index_hl(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int *access_needed)
{
    int access_index;
    dbal_table_field_info_t *table_field_info;
    int field_pos_in_interface;
    multi_res_info_t *multi_res_info = &entry_handle->table->multi_res_info[entry_handle->cur_res_type];
    dbal_hl_l2p_info_t *l2p_hl_sw_info =
        &entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_SW];

    SHR_FUNC_INIT_VARS(unit);

    for (access_index = 0; access_index < l2p_hl_sw_info->num_of_access_fields; access_index++)
    {
        uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };

        field_pos_in_interface = l2p_hl_sw_info->l2p_fields_info[access_index].field_pos_in_interface;

        table_field_info = &(multi_res_info->results_info[field_pos_in_interface]);

        *access_needed = TRUE;

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_handle->phy_entry.payload,
                         l2p_hl_sw_info->l2p_fields_info[access_index].offset_in_interface,
                         l2p_hl_sw_info->l2p_fields_info[access_index].access_nof_bits, field_value));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (field_value, l2p_hl_sw_info->l2p_fields_info[access_index].access_offset,
                         l2p_hl_sw_info->l2p_fields_info[access_index].access_nof_bits, entry_sw_value));

        DBAL_SW_FIELD_LOG(table_field_info->field_id, field_value[0],
                          l2p_hl_sw_info->l2p_fields_info[access_index].access_offset,
                          l2p_hl_sw_info->l2p_fields_info[access_index].access_nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function set an entry represented by entry_handle to a
 * specific entry index in sw state table for mdb access.
 * Only Result Type can be mapped
 */
static shr_error_e
dbal_sw_table_entry_set_in_index_mdb(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int *access_needed)
{
    dbal_table_field_info_t *table_field_info;
    multi_res_info_t *multi_res_info = &entry_handle->table->multi_res_info[entry_handle->cur_res_type];

    SHR_FUNC_INIT_VARS(unit);

    table_field_info = &(multi_res_info->results_info[0]);

    if (entry_handle->value_field_ids[0] != DBAL_FIELD_EMPTY)
    {
        *access_needed = TRUE;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_handle->phy_entry.payload, table_field_info->bits_offset_in_buffer,
                         table_field_info->field_nof_bits, entry_sw_value));

        DBAL_SW_FIELD_LOG(table_field_info->field_id, entry_sw_value[0], 0, table_field_info->field_nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function set an entry represented by entry_handle to a
 * specific entry index in sw state table for sw only table (all
 * entry is written)
 */
static shr_error_e
dbal_sw_table_entry_set_in_index_sw_only(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int *access_needed)
{
    multi_res_info_t *multi_res_info = &entry_handle->table->multi_res_info[entry_handle->cur_res_type];

    SHR_FUNC_INIT_VARS(unit);

    *access_needed = TRUE;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (entry_handle->phy_entry.payload, 0, multi_res_info->entry_payload_size, entry_sw_value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function set an entry represented by entry_handle to a
 * specific entry index in sw state table, in sw only tables the
 * full entry is written
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_index - \n
 *    The index to insert to entry in sw state
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 *  \par INDIRECT OUTPUT:
 *  SW state dbal tables, an entry will
 *    be added to the relevant table
 */
static shr_error_e
dbal_sw_table_entry_set_in_index(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int entry_index)
{
    int bsl_severity;
    uint32 entry_sw_value[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint8 payload[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
    int access_needed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 0);
    }

    if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index_hl(unit, entry_handle, entry_sw_value, &access_needed));
    }
    else if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index_mdb(unit, entry_handle, entry_sw_value, &access_needed));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index_sw_only(unit, entry_handle, entry_sw_value, &access_needed));
    }

    if (access_needed)
    {
        SHR_IF_ERR_EXIT(utilex_U32_to_U8(entry_sw_value, entry_handle->table->sw_payload_length_bytes, payload));

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.entries.memwrite(unit,
                                                              entry_handle->table_id,
                                                              payload,
                                                              entry_index *
                                                              entry_handle->table->sw_payload_length_bytes,
                                                              entry_handle->table->sw_payload_length_bytes));
        if (bsl_severity >= bslSeverityInfo)
        {
            uint8 diag_payload[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
            int res_length_bytes = entry_handle->table->sw_payload_length_bytes;
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.entries.memread(unit, entry_handle->table_id,
                                                                 diag_payload,
                                                                 entry_index *
                                                                 entry_handle->table->sw_payload_length_bytes,
                                                                 res_length_bytes));

            SHR_IF_ERR_EXIT(dbal_sw_table_entry_dump(unit, entry_handle->table_id,
                                                     entry_index, diag_payload, res_length_bytes, "add"));

            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 0);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function return the information from specific entry index
 * in sw state table to the dbal entry handle for hl access
 */
static shr_error_e
dbal_sw_table_entry_get_in_index_hl(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int result_type_index)
{
    int access_index;
    dbal_table_field_info_t *table_field_info;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    dbal_hl_l2p_info_t l2p_sw_info =
        entry_handle->table->hl_mapping_multi_res[result_type_index].l2p_hl_info[DBAL_HL_ACCESS_SW];

    SHR_FUNC_INIT_VARS(unit);

    for (access_index = 0; access_index < l2p_sw_info.num_of_access_fields; access_index++)
    {
        int pos_in_interface = l2p_sw_info.l2p_fields_info[access_index].field_pos_in_interface;
        table_field_info = &(entry_handle->table->multi_res_info[result_type_index].results_info[pos_in_interface]);

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_sw_value, l2p_sw_info.l2p_fields_info[access_index].access_offset,
                         l2p_sw_info.l2p_fields_info[access_index].access_nof_bits, field_value));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_value,
                                                       l2p_sw_info.l2p_fields_info[access_index].offset_in_interface,
                                                       l2p_sw_info.l2p_fields_info[access_index].nof_bits_in_interface,
                                                       entry_handle->phy_entry.payload));

        entry_handle->phy_entry.payload_size += l2p_sw_info.l2p_fields_info[access_index].nof_bits_in_interface;

        DBAL_SW_FIELD_LOG(table_field_info->field_id, field_value[0], 0, table_field_info->field_nof_bits);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function return the information from specific entry index
 * in sw state table to the dbal entry handle for mdb access
 */
static shr_error_e
dbal_sw_table_entry_get_in_index_mdb(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int result_type_index)
{
    dbal_table_field_info_t *table_field_info;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    table_field_info = &(entry_handle->table->multi_res_info[result_type_index].results_info[0]);

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(entry_sw_value, 0, table_field_info->field_nof_bits, field_value));

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_value, table_field_info->bits_offset_in_buffer,
                                                   table_field_info->field_nof_bits, entry_handle->phy_entry.payload));

    DBAL_SW_FIELD_LOG(table_field_info->field_id, field_value[0], 0, table_field_info->field_nof_bits);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function return the information from specific entry index
 * in sw state table to the dbal entry handle for sw_only access
 */
static shr_error_e
dbal_sw_table_entry_get_in_index_sw_only(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int result_type_index)
{
    int field_index;
    dbal_table_field_info_t *table_field_info;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    for (field_index = 0; field_index < entry_handle->table->multi_res_info[result_type_index].nof_result_fields;
         field_index++)
    {
        table_field_info = &(entry_handle->table->multi_res_info[result_type_index].results_info[field_index]);

        if (!entry_handle->get_all_fields)
        {
            /** only if single result type per table - check which fields were requested */
            if (entry_handle->value_field_ids[field_index] == DBAL_FIELD_EMPTY)
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_sw_value, table_field_info->bits_offset_in_buffer,
                         table_field_info->field_nof_bits, field_value));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_value, table_field_info->bits_offset_in_buffer,
                                                       table_field_info->field_nof_bits,
                                                       entry_handle->phy_entry.payload));

        entry_handle->phy_entry.payload_size += table_field_info->field_nof_bits;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_sw_table_resolved_res_type_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *entry_sw_value,
    int *resolved_res_type)
{
    dbal_table_field_info_t *table_field_info;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    int result_type_size;
    int result_type_offset_in_sw_buffer;
    uint32 result_type_mask;

    SHR_FUNC_INIT_VARS(unit);

    table_field_info = &(entry_handle->table->multi_res_info[0].results_info[0]);

    result_type_size = table_field_info->field_nof_bits;

    if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
    {
        result_type_offset_in_sw_buffer = table_field_info->bits_offset_in_buffer;
    }
    else if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        result_type_offset_in_sw_buffer = 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "access_method == DBAL_ACCESS_METHOD_HARD_LOGIC, iw wrong here\n");
    }

    result_type_mask = ((1 << result_type_size) - 1);

    if (result_type_offset_in_sw_buffer)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_sw_value, result_type_offset_in_sw_buffer, table_field_info->field_nof_bits,
                         field_value));
    }
    else
    {
        /** incase this is the result type it has to be max 32 bit and offset is 0 */
        field_value[0] = entry_sw_value[0];
    }

    *resolved_res_type = entry_sw_value[0] & result_type_mask;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function return the information from specific entry index
 * in sw state table to the dbal entry handle
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_index - \n
 *    The index to get the information from in sw state
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *    \param [in] res_type_get - \n
 *      indicate if the get should consider a specific result
 *    \param [in] resolved_res_type - \n
 *      if not NULL, the API will only resolve the result type and return it on it
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 *  \par INDIRECT OUTPUT:
 *    \param [in] entry_handle - \n
 *      will be updated with the entry information
 */
static shr_error_e
dbal_sw_table_entry_get_in_index(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int entry_index,
    int res_type_get,
    int *resolved_res_type)
{
    int res_length_bytes;
    uint8 payload[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
    uint32 entry_sw_value[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    int result_type_index = 0;
    int bsl_severity;
    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    res_length_bytes = entry_handle->table->sw_payload_length_bytes;

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.entries.memread(unit,
                                                         entry_handle->table_id,
                                                         payload,
                                                         entry_index * entry_handle->table->sw_payload_length_bytes,
                                                         res_length_bytes));

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(payload, entry_handle->table->sw_payload_length_bytes, entry_sw_value));

    if (bsl_severity >= bslSeverityInfo)
    {
        DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 1);
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_dump
                        (unit, entry_handle->table_id, entry_index, payload, res_length_bytes, "get from"));
    }

    /** If not NULL, means should perform result type resolution */
    if (resolved_res_type != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_resolved_res_type_get(unit, entry_handle, entry_sw_value, resolved_res_type));
        SHR_EXIT();
    }

    if (res_type_get != DBAL_INVALID_PARAM)
    {
        result_type_index = res_type_get;
    }
    else
    {
        result_type_index = entry_handle->cur_res_type;
    }

    if (result_type_index == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "result type not initialized for entry. table %s\n",
                     entry_handle->table->table_name);
    }

    switch (entry_handle->table->access_method)
    {
        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index_sw_only
                            (unit, entry_handle, entry_sw_value, result_type_index));
            break;
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index_hl(unit, entry_handle, entry_sw_value, result_type_index));
            break;
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index_mdb
                            (unit, entry_handle, entry_sw_value, result_type_index));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Access method %s cannot involve SW fields\n",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
            break;
    }

    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 1);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * this function resolve the result type of an entry, before it gets the full entry.
 * If no error occur, the cur_res_type on entry handle will be update.
 * If cannot find match result type, _SHR_E_NOT_FOUND is returned
 */
shr_error_e
dbal_sw_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii, jj;
    dbal_sw_state_table_type_e table_type;
    int resolved_result_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    dbal_logger_internal_disable_set(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get
                            (unit, entry_handle, DBAL_INVALID_PARAM, &resolved_result_type));
            break;
        case DBAL_SW_TABLE_HASH:
            SHR_SET_CURRENT_ERR(dbal_sw_table_hash_entry_get(unit, entry_handle, &resolved_result_type));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "table type not supported %d\n", table_type);
            break;
    }

    for (ii = 0; ii < entry_handle->table->nof_result_types; ii++)
    {
        for (jj = 0; jj < entry_handle->table->multi_res_info[ii].result_type_nof_hw_values; jj++)
        {
            if (entry_handle->table->multi_res_info[ii].result_type_hw_value[jj] == resolved_result_type)
            {
                entry_handle->cur_res_type = ii;
                SHR_EXIT();
            }
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    dbal_logger_internal_disable_clear(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is used to add entry to a dbal sw state table of
 * type direct
 * It calculates the entry index in the table according the key
 * and calls dbal_sw_table_entry_set_in_index
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_direct_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int key_length, key_mask, entry_index;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    key_length = entry_handle->phy_entry.key_size;
    key_mask = ((1 << key_length) - 1);

    entry_index = key_mask & entry_handle->phy_entry.key[0];

    if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
    {
            /** if partial entry for direct return error */
        multi_res_info_t *multi_res_info = &entry_handle->table->multi_res_info[entry_handle->cur_res_type];
        if (multi_res_info->nof_result_fields != entry_handle->nof_result_fields)
        {
            if (!entry_handle->entry_merged)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial set issue %s fields in table fields %d in handle %d\n",
                             entry_handle->table->table_name, multi_res_info->nof_result_fields,
                             entry_handle->nof_result_fields);
            }
        }
    }
    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
            /** We only add the core_id to the key in case of DPC */
                entry_index &= key_mask;
                entry_index |= (core_id << key_length);
            }

            SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index(unit, entry_handle, entry_index));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is used to get and entry to a dbal sw state table
 * of type direct
 * It calculates the entry index in the table according the key
 * and calls dbal_sw_table_entry_get_in_index
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *    \param [in] res_type_get - \n
 *      indicate if the get should consider a specific result
 *      type
 *    \param [in] resolved_res_type - \n
 *     the returned result type, in case of called by res_type_resolution
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_direct_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int res_type_get,
    int *resolved_res_type)
{
    int key_length, key_mask, entry_index;

    SHR_FUNC_INIT_VARS(unit);

    key_length = entry_handle->phy_entry.key_size;
    key_mask = ((1 << key_length) - 1);

    entry_index = key_mask & entry_handle->phy_entry.key[0];

    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        if (entry_handle->core_id >= 0)
        {
            entry_index |= (entry_handle->core_id << key_length);
        }
    }

    SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index(unit, entry_handle, entry_index, res_type_get, resolved_res_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is the generic API to set a default entry (delete
 * an entry) in dbal SW state tables.
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_direct_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int key_length, key_mask, entry_index;
    uint8 default_payload[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
    int bsl_severity;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    key_length = entry_handle->phy_entry.key_size;
    key_mask = ((1 << key_length) - 1);

    entry_index = key_mask & entry_handle->phy_entry.key[0];

    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
                /** We only add the core_id to the key in case of DPC */
                entry_index &= key_mask;
                entry_index |= (core_id << key_length);
            }

            if (bsl_severity >= bslSeverityInfo)
            {
                DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 2);
                LOG_CLI((BSL_META_U(unit, "DBAL SW Access (direct). Entry clear\n")));
                LOG_CLI((BSL_META_U(unit, "Entry index=0x%08x\n"), entry_index));
                DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 2);
            }

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.entries.memwrite(unit,
                                                                  entry_handle->table_id,
                                                                  default_payload,
                                                                  entry_index *
                                                                  entry_handle->table->sw_payload_length_bytes,
                                                                  entry_handle->table->sw_payload_length_bytes));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init the iterator info structure, according to the
 *        table parameters.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - dbal table id
 *   \param [in] key_size - table key_size
 *   \param [in] dbal_iterator - sw table iterator info
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_sw_table_direct_iterator_init(
    int unit,
    dbal_tables_e table_id,
    uint32 key_size,
    dbal_iterator_info_t * dbal_iterator)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_max_key_value_get(unit, table_id, &dbal_iterator->max_num_of_iterations));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the next entry in direct table, relative to the
 *        entry that exist in iterator info
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit
 *   \param [in] entry_handle
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_sw_table_direct_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    uint8 entry_found = FALSE;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    while ((!entry_found) && (!iterator_info->is_end))
    {
        /**
         * Check if we iterated over all keys in the current core
         */
        if (iterator_info->used_first_key)
        {
            if (entry_handle->phy_entry.key[0] == iterator_info->max_num_of_iterations)
            {
                /** Check if next (lower) core is valid */
                entry_handle->core_id--;
                if (entry_handle->core_id < 0)
                {
                    /**No more entries */
                    break;
                }
                else
                {
                    entry_handle->phy_entry.key[0] = 0;
                }
            }
            else
            {
                entry_handle->phy_entry.key[0]++;
            }
        }
        iterator_info->used_first_key = TRUE;
        is_valid_entry = DBAL_KEY_IS_VALID;
        if (entry_handle->table->allocator_field_id != DBAL_FIELD_EMPTY)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_increment_by_allocator(unit, entry_handle, &is_valid_entry));
        }
        if (is_valid_entry == DBAL_KEY_IS_VALID)
        {
            SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
        }

        /**
         * Get the current entry
         */
        if (is_valid_entry == DBAL_KEY_IS_VALID)
        {
            int rv;
            entry_handle->get_all_fields = TRUE;
            sal_memset(entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);

            if (entry_handle->table->has_result_type)
            {
                entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;

                rv = dbal_sw_res_type_resolution(unit, entry_handle);
                if (rv != _SHR_E_NONE)
                {
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        continue;
                    }
                    else
                    {
                        SHR_ERR_EXIT(rv, "SW entry get");
                    }
                }
            }
            else
            {
                entry_handle->cur_res_type = 0;
            }

            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get(unit, entry_handle, DBAL_INVALID_PARAM, NULL));

            /** if the entry is not equals to the default entry it is returned */
            if (iterator_info->mode == DBAL_ITER_MODE_ALL)
            {
                entry_found = TRUE;
            }
            else if (sal_memcmp
                     (zero_buffer_to_compare, entry_handle->phy_entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_BYTES) != 0)
            {
                                /** if the entry is not equals to the default entry it is returned */
                entry_found = TRUE;
            }
        }

    }

    if (entry_found == FALSE)
    {
        iterator_info->is_end = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is used to add entry to a dbal sw state table of
 * type hash
 * It gets the entry index in the table according the hash table
 * (adds the key to hash table) and calls
 * dbal_sw_table_entry_set_in_index
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_hash_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 data_indx = UTILEX_U32_MAX;
    uint8 success = FALSE;
    uint32 key_val[BITS2WORDS(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS)] = { 0 };
    dbal_sw_state_hash_key key;
    int core_id;
    int key_length = entry_handle->table->key_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&key, 0, sizeof(key));

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(entry_handle->phy_entry.key, 0, entry_handle->phy_entry.key_size,
                                                   key_val));
    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
                /** We only add the core_id to the key in case of DPC */
                SHR_IF_ERR_EXIT(utilex_bitstream_set_field(key_val, key_length - DBAL_CORE_SIZE_IN_BITS,
                                                           DBAL_CORE_SIZE_IN_BITS, core_id));
            }

            SHR_IF_ERR_EXIT(utilex_U32_to_U8(key_val, WORDS2BYTES(BITS2WORDS(key_length)), (uint8 *) &key));

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.
                            hash_table.insert(unit, entry_handle->table_id, &key, &data_indx, &success));
            if (!success)
            {
                int nof_entries;
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, entry_handle->table_id, &nof_entries));
                if (entry_handle->table->max_capacity <= nof_entries)
                {
                    SHR_ERR_EXIT(SOC_E_FULL,
                                 "cant add entry to hash sw table %s, table is full, hash table ID %d max capacity is %d\n",
                                 entry_handle->table->table_name, entry_handle->table_id,
                                 entry_handle->table->max_capacity);

                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "cant add entry to hash sw table %s hash table ID %d\n",
                                 entry_handle->table->table_name, entry_handle->table_id);
                }
            }
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_set_in_index(unit, entry_handle, data_indx));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is used to get entry to a dbal sw state table of
 * type hash
 * It gets the entry index in the table according the hash table
 * (lookup with the key in the hash table) and calls
 * dbal_sw_table_entry_get_in_index
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *    \param [in] resolved_res_type - \n
 *      the resolved result type
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_hash_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *resolved_res_type)
{
    uint32 data_indx = { 0 };
    uint8 found = FALSE;
    uint32 key_val[BITS2WORDS(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS)] = { 0 };
    dbal_sw_state_hash_key key;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&key, 0, sizeof(key));
    entry_handle->phy_entry.key_size = entry_handle->table->key_size;

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(entry_handle->phy_entry.key, 0, entry_handle->phy_entry.key_size,
                                                   key_val));
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        int key_length = entry_handle->phy_entry.key_size - DBAL_CORE_SIZE_IN_BITS;
        if (entry_handle->core_id >= 0)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                            (key_val, key_length, DBAL_CORE_SIZE_IN_BITS, entry_handle->core_id));
        }
        SHR_IF_ERR_EXIT(utilex_U32_to_U8
                        (key_val, WORDS2BYTES(BITS2WORDS(entry_handle->phy_entry.key_size)), (uint8 *) &key));
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_U32_to_U8
                        (key_val, WORDS2BYTES(BITS2WORDS(entry_handle->phy_entry.key_size)), (uint8 *) &key));
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.find(unit, entry_handle->table_id, &key, &data_indx, &found));
    if (!found)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_in_index
                        (unit, entry_handle, data_indx, DBAL_INVALID_PARAM, resolved_res_type));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is used to delete entry to a dbal sw state table
 * of type hash
 * It remove the key from the hash table
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_hash_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 key_val[BITS2WORDS(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS)] = { 0 };
    dbal_sw_state_hash_key key;
    int bsl_severity;
    int core_id;
    int key_length = entry_handle->table->key_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&key, 0, sizeof(key));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(entry_handle->phy_entry.key, 0, key_length, key_val));

    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {
                /** We only add the core_id to the key in case of DPC */
                SHR_IF_ERR_EXIT(utilex_bitstream_set_field(key_val, key_length - DBAL_CORE_SIZE_IN_BITS,
                                                           DBAL_CORE_SIZE_IN_BITS, core_id));
            }

            SHR_IF_ERR_EXIT(utilex_U32_to_U8(key_val, WORDS2BYTES(BITS2WORDS(key_length)), (uint8 *) &key));

            SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);
            if (bsl_severity >= bslSeverityInfo)
            {
                if (dbal_logger_is_enable(unit, entry_handle->table_id))
                {
                    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 2);
                    LOG_CLI((BSL_META_U(unit, "DBAL SW Access (hash). Entry clear\n")));
                    if (key_length > 32)
                    {
                        LOG_CLI((BSL_META_U(unit, "hash key=0x%02x%02x%02x%02x%02x%02x%02x%02x\n"),
                                 ((uint8 *) &key)[7], ((uint8 *) &key)[6], ((uint8 *) &key)[5], ((uint8 *) &key)[4],
                                 ((uint8 *) &key)[3], ((uint8 *) &key)[2], ((uint8 *) &key)[1], ((uint8 *) &key)[0]));
                    }
                    else
                    {
                        LOG_CLI((BSL_META_U(unit, "hash key=0x%02x%02x%02x%02x\n"), ((uint8 *) &key)[3],
                                 ((uint8 *) &key)[2], ((uint8 *) &key)[1], ((uint8 *) &key)[0]));
                    }
                    DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_SW, 2);
                }
            }

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.delete(unit, entry_handle->table_id, &key));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init the iterator info structure, according to the
 *        table parameters.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - dbal table id
 *   \param [in] sw_iterator - sw table iterator info
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_sw_table_hash_iterator_init(
    int unit,
    dbal_tables_e table_id,
    dbal_sw_table_iterator_t * sw_iterator)
{
    SHR_FUNC_INIT_VARS(unit);

    sw_iterator->hash_table_id = table_id;
    sw_iterator->hash_entry_index = 0;

    SHR_FUNC_EXIT;
}

/**
 * \brief - get the next entry in hash table, relative to the
 *        entry that exist in iterator info
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit
 *   \param [in] entry_handle
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_sw_table_hash_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 data_indx;
    dbal_sw_state_hash_key key;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    sal_memset(&key, 0, sizeof(key));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.get_next(unit, iterator_info->sw_iterator.hash_table_id,
                                                             &iterator_info->sw_iterator.hash_entry_index, &key,
                                                             &data_indx));
    iterator_info->used_first_key = TRUE;

    if (SW_STATE_HASH_TABLE_ITER_IS_END(&iterator_info->sw_iterator.hash_entry_index))
    {
        iterator_info->is_end = TRUE;
        SHR_EXIT();
    }
    else
    {
        dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
        entry_handle->phy_entry.key_size = entry_handle->table->key_size;

        SHR_IF_ERR_EXIT(utilex_U8_to_U32((uint8 *) &key, DBAL_PHYSICAL_RES_SIZE_IN_WORDS, entry_handle->phy_entry.key));

        if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
        {
            int key_length = entry_handle->phy_entry.key_size - DBAL_CORE_SIZE_IN_BITS;
            uint32 core_id_u32 = 0;
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(entry_handle->phy_entry.key, key_length,
                                                           DBAL_CORE_SIZE_IN_BITS, &core_id_u32));
            entry_handle->core_id = core_id_u32;
        }

        SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
        if (is_valid_entry == DBAL_KEY_IS_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SW Hash iterator found invalid entry\n");
        }
        else if (is_valid_entry == DBAL_KEY_IS_OUT_OF_ITERATOR_RULE)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_get_next(unit, entry_handle));
            SHR_EXIT();
        }

        entry_handle->get_all_fields = TRUE;
        sal_memset(entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);

        if (entry_handle->table->has_result_type)
        {
            entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;
            SHR_IF_ERR_EXIT(dbal_sw_res_type_resolution(unit, entry_handle));
        }
        else
        {
            entry_handle->cur_res_type = 0;
        }
        SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_get(unit, entry_handle, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is the generic API to set an  entry in dbal SW
 * state tables.
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e table_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_set(unit, entry_handle));
            break;
        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_add(unit, entry_handle));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is the generic API to get an entry in dbal SW
 * state tables.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e table_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get(unit, entry_handle, DBAL_INVALID_PARAM, NULL));
            break;
        case DBAL_SW_TABLE_HASH:
            SHR_SET_CURRENT_ERR(dbal_sw_table_hash_entry_get(unit, entry_handle, NULL));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function is the generic API to delete an entry in dbal SW
 * state tables.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] entry_handle - \n
 *      dbal entry handle - holds the entry information
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *    Error code
 */
shr_error_e
dbal_sw_table_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e table_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_clear(unit, entry_handle));
            break;

        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_delete(unit, entry_handle));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear a sw table. direct or hash
 * clear direct table mean to set all entries to zero
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] entry_handle
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_sw_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e table_type;
    uint32 entry_index, max_entry_index, key_size;
    uint8 default_payload[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &table_type));
    switch (table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            key_size = entry_handle->table->key_size;
            max_entry_index = utilex_power_of_2(key_size);
            for (entry_index = 0; entry_index < max_entry_index; entry_index++)
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.entries.memwrite(unit,
                                                                      entry_handle->table_id,
                                                                      default_payload,
                                                                      entry_index *
                                                                      entry_handle->table->sw_payload_length_bytes,
                                                                      entry_handle->table->sw_payload_length_bytes));
            }
            break;

        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.hash_table.clear(unit, entry_handle->table_id));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - iterator init. sw tables shell.
 * calls iterator init of direct/hash according to tablt type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] entry_handle -
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_sw_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e sw_table_type;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &sw_table_type));

    switch (sw_table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_iterator_init(unit, entry_handle->table_id,
                                                               entry_handle->phy_entry.key_size, iterator_info));
            break;

        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_iterator_init(unit, entry_handle->table_id,
                                                             &iterator_info->sw_iterator));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - iterator get next. sw tables shell
 * calls iterator get next of direct/hash according to tablt
 * type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] entry_handle -
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dbal_sw_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_sw_state_table_type_e sw_table_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TABLES.table_type.get(unit, entry_handle->table_id, &sw_table_type));

    switch (sw_table_type)
    {
        case DBAL_SW_TABLE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_get_next(unit, entry_handle));
            break;

        case DBAL_SW_TABLE_HASH:
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_get_next(unit, entry_handle));
            break;

        default:
            break;
    }
exit:
    SHR_FUNC_EXIT;
}
