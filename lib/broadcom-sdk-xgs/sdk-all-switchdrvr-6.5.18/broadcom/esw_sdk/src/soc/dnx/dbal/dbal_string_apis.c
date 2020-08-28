/*
 * $Id: dbal_string_apis.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <soc/sand/sand_signals.h>
#include <bcm_int/dnx/field/field_entry.h>

extern shr_error_e dbal_field_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    multi_res_info_t * result_info,
    uint8 is_full_buffer,
    dbal_fields_e parent_field,
    int size,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);

shr_error_e
dnx_dbal_fields_string_form_hw_value_get(
    int unit,
    char *field_name,
    uint32 hw_value,
    char *str)
{
    dbal_fields_e field_id;
    dbal_field_types_basic_info_t *field_info;
    int ii;
    uint32 local_hw_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    sal_strcpy(str, EMPTY);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    /** if there is a define value should return the string */
    if (field_info->nof_defines_values > 0)
    {
        for (ii = 0; ii < field_info->nof_defines_values; ii++)
        {
            if (field_info->defines_info[ii].value == hw_value)
            {
                sal_sprintf(str, "%s", field_info->defines_info[ii].name);
                SHR_EXIT();
            }
        }
    }

    /** if the field is enum return the string */
    if (field_info->nof_enum_values > 0)
    {
        for (ii = 0; ii < field_info->nof_enum_values; ii++)
        {
            if ((field_info->enum_val_info[ii].value == hw_value) && (!field_info->enum_val_info[ii].is_invalid))
            {
                sal_sprintf(str, "%s", field_info->enum_val_info[ii].name);
                SHR_EXIT();
            }
        }
        SHR_ERR_EXIT(_SHR_E_PARAM, "For enum field:%s HW value:%u does not match existing values\n",
                     field_name, hw_value);
    }

    /** if it is a parent field look for child fields encoding */
    if (field_info->nof_child_fields > 0)
    {
        dbal_fields_e sub_field_id;
        uint32 sub_field_val;
        sub_field_val = 0;
        local_hw_value[0] = hw_value;
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, field_id, local_hw_value, &sub_field_id, &sub_field_val));

        if (sub_field_id != DBAL_FIELD_EMPTY)
        {
            sal_sprintf(str, "%s %x", dbal_field_to_string(unit, sub_field_id), sub_field_val);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_fields_struct_field_encode(
    int unit,
    char *field_name,
    char *field_in_struct_name,
    uint32 *field_in_struct_val,
    uint32 *full_field_val)
{
    dbal_fields_e field_id;
    dbal_fields_e field_in_struct_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_in_struct_name, &field_in_struct_id));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, field_id, field_in_struct_id, field_in_struct_val, full_field_val));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_dbal_fields_enum_value_get(
    int unit,
    char *field_name,
    char *enum_val_name,
    int *hw_val)
{
    dbal_fields_e field_id;
    int ii;
    dbal_field_types_basic_info_t *field_info;
    char full_enum_name[128];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    sal_sprintf(full_enum_name, "%s_%s", field_name, enum_val_name);

    for (ii = 0; ii < field_info->nof_enum_values; ii++)
    {
        if ((sal_strcasecmp(full_enum_name, field_info->enum_val_info[ii].name) == 0)
            && (!field_info->enum_val_info[ii].is_invalid))
        {
            (*hw_val) = (int) field_info->enum_val_info[ii].value;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL - field %s val name %s wasn't found \n", field_name, enum_val_name);

exit:
    SHR_FUNC_EXIT;
}

int
dnx_dbal_fields_enum_dbal_value_get(
    int unit,
    char *field_name,
    char *enum_val_name,
    int *dbal_val)
{
    dbal_fields_e field_id;
    int ii;
    dbal_field_types_basic_info_t *field_info;
    char full_enum_name[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    sal_snprintf(full_enum_name, RHSTRING_MAX_SIZE, "%s_%s", field_name, enum_val_name);

    for (ii = 0; ii < field_info->nof_enum_values; ii++)
    {
        if ((sal_strcasecmp(full_enum_name, field_info->enum_val_info[ii].name) == 0)
            && (!field_info->enum_val_info[ii].is_invalid))
        {
            (*dbal_val) = ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL - field %s val name %s wasn't found \n", field_name, enum_val_name);

exit:
    SHR_FUNC_EXIT;
}

int
dnx_dbal_fields_sub_field_info_get(
    int unit,
    char *parent_field_name,
    uint32 hw_val,
    char *sub_field_name,
    int *sub_field_val)
{
    dbal_fields_e field_id;
    dbal_field_types_basic_info_t *field_info;
    uint32 hw_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    (*sub_field_val) = 0;

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, parent_field_name, &field_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    /** if it is a parent field look for child fields encoding */
    if (field_info->nof_child_fields > 0)
    {
        dbal_fields_e sub_field_id;
        uint32 sub_field_val_as_uint[1];
        hw_value[0] = hw_val;
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                        (unit, field_id, hw_value, &sub_field_id, sub_field_val_as_uint));

        (*sub_field_val) = sub_field_val_as_uint[0];

        if (sub_field_id != DBAL_FIELD_EMPTY)
        {
            sal_sprintf(sub_field_name, "%s%c", dbal_field_to_string(unit, sub_field_id), '\0');
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal input no subfields found or/ non of them match to he value\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_take(
    int unit,
    char *table_name,
    uint32 *entry_handle_id)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_take_macro(unit, table_id, entry_handle_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_clear(
    int unit,
    char *table_name,
    uint32 entry_handle_id)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_clear_macro(unit, table_id, entry_handle_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_free(
    int unit,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_handle_release_internal(unit, entry_handle_id);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 *field_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id, field_val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_arr32_masked_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 *field_val,
    uint32 *field_mask)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, field_id, field_val, field_mask);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_symbol_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    char *symbol_name)
{
    dbal_fields_e field_id;
    uint32 symbol_value[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_enum_string_to_id(unit, field_id, symbol_name, &symbol_value[0]));

    dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id, &symbol_value[0]);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 *field_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_key_field_arr8_set(unit, entry_handle_id, field_id, field_val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_key_field_arr8_masked_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 *field_val,
    uint8 *field_mask)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, field_id, field_val, field_mask);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_value_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint32 *field_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, inst_id, field_val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_value_field_symbol_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    char *symbol_name)
{
    dbal_fields_e field_id;
    uint32 symbol_value[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {
        dbal_entry_handle_t *entry_handle;

        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
        SHR_IF_ERR_EXIT(dbal_result_type_string_to_id(unit, entry_handle->table_id, symbol_name, &symbol_value[0]));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_enum_string_to_id(unit, field_id, symbol_name, &symbol_value[0]));
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, inst_id, &symbol_value[0]);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_value_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint8 *field_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, field_id, inst_id, field_val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_commit(
    int unit,
    uint32 entry_handle_id,
    char *flag_name)
{
    dbal_entry_action_flags_e action_flag;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_flag_string_to_id(unit, flag_name, &action_flag));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, action_flag));
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_dbal_entry_get(
    int unit,
    uint32 entry_handle_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
exit:
    SHR_FUNC_EXIT;

}
shr_error_e
dnx_dbal_entry_handle_access_id_set(
    int unit,
    uint32 entry_handle_id,
    uint32 entry_access_id)
{
    dbal_entry_handle_t *entry_handle;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_access_id_allocate_and_set(
    int unit,
    uint32 entry_handle_id,
    int core,
    uint32 priority,
    uint32 *entry_access_id)
{
    dbal_entry_handle_t *entry_handle;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, entry_handle->table_id, priority, entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, *entry_access_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_key_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_key_field_arr32_masked_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, entry_handle_id, field_id, field_val, field_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_key_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, field_id, field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_key_field_arr8_masked_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                    (unit, entry_handle_id, field_id, field_val, field_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_value_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, inst_id, field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_value_field_symbol_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    char *symbol_name)
{
    dbal_fields_e field_id;
    uint32 field_val[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, inst_id, &field_val[0]));

    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {
        dbal_entry_handle_t *entry_handle;

        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
        sal_sprintf(symbol_name, "%s", dbal_result_type_to_string(unit, entry_handle->table_id, field_val[0]));
    }
    else
    {
        sal_sprintf(symbol_name, "%s", dbal_enum_id_to_string(unit, field_id, field_val[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_handle_access_id_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, entry_access_id));

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_dbal_entry_handle_value_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES])
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, field_id, inst_id, field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_entry_clear(
    int unit,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_dbal_table_clear(
    int unit,
    char *table_name)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief Copies a signal_output_t structure's relevant fields to a newly allocated entry in 'list'
 */
static shr_error_e
dnx_dbal_decode_field_signal_copy(
    int unit,
    signal_output_t * sig,
    rhlist_t * list)
{
    rhhandle_t rhhandle;
    signal_output_t *new_sig;
    char name[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    sal_snprintf(name, RHNAME_MAX_SIZE, "%s", RHNAME(sig));
    /*
     * Allocate a new entry in list
     */
    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(list, name, RHID_TO_BE_GENERATED, &rhhandle));
    new_sig = rhhandle;
    /*
     * Copy relevant fields (name has already been copied)
     */
    new_sig->size = sig->size;
    SHR_BITCOPY_RANGE(new_sig->value, 0, sig->value, 0, sig->size);
    sal_strncpy(new_sig->print_value, sig->print_value, DSIG_MAX_SIZE_STR);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert the decoding of parent_field/child fields data to signal strings
 *
 * \param [in] unit - Relevant unit
 * \param [in] decoded_val - The value of the child field (without the prefix)
 * \param [in] index - Position in the decoded string
 * \param [in] encode_type - The encode type of the dbal field
 * \param [in] decoded_values_list - Contain all the splitted strings from the decoded data
 * \param [in] sub_field_id - Child field id
 * \param [in] field_info - Field type info
 * \param [in] data_size_bits - The size of dbal field in bits
 * \param [in] sig_format - Signal output structure
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_parent_field_string_convert_to_signal_string(
    int unit,
    uint32 decoded_val,
    int index,
    dbal_value_field_encode_types_e encode_type,
    char **decoded_values_list,
    dbal_fields_e sub_field_id,
    dbal_field_types_basic_info_t * field_info,
    uint32 data_size_bits,
    signal_output_t * sig_format)
{
    int ii;
    dbal_field_types_defs_e field_type;
    uint32 size = 0;
    uint32 field_size = 0;
    dbal_fields_e struct_member_field_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, sub_field_id, &field_size));
    /*
     * In case of field with child fields print type (index 0) that will correspond to childfield`s name and
     * print value (index 1) that will correspond to the value of the childfield
     * The rest indexes will correspond to the structure`s members in case that the childfield is from type STRUCT
     */
    if (index == 0)
    {
        sal_strncpy(sig_format->entry.name, "type", RHNAME_MAX_SIZE);
        /** Set childfield`s encoding to signal`s value */
        for (ii = 0; ii < field_info->nof_child_fields; ii++)
        {
            if (field_info->sub_field_info[ii].sub_field_id == sub_field_id)
            {
                /** Set subfield`s encoding to signal`s value */
                sig_format->value[0] = field_info->sub_field_info[ii].encode_info.input_param;
                /** Set the size in bits*/
                sig_format->size = data_size_bits - field_size;
                break;
            }
        }

    }
    if (index == 1)
    {
        sal_strncpy(sig_format->entry.name, "value", RHNAME_MAX_SIZE);
        /** Set childfield`s value to signal`s value */
        sig_format->value[0] = decoded_val;
        /** Set the size in bits*/
        sig_format->size = field_size;
    }

    /** In case that the index is bigger than 1, even number and the sub_field is from type struct, it is parsed accordingly */
    if ((index > 1) && (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCT))
    {
        if ((index & 1) == 0)
        {
            sal_strncpy(sig_format->entry.name, decoded_values_list[index], RHNAME_MAX_SIZE - 1);
        }
        /** Get the field_type of the child field */
        SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, sub_field_id, &field_type));
        /** Get the field_id of the struct member */
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, decoded_values_list[index], &struct_member_field_id));
        /** Get the size of the struct member */
        SHR_IF_ERR_EXIT(dbal_field_struct_field_size_get(unit, field_type, struct_member_field_id, &size));
        /** Set struct member`s value to signal`s value */
        if (!sal_strcmp(decoded_values_list[index + 1], "True"))
        {
            sig_format->value[0] = 1;
        }
        else if (!sal_strcmp(decoded_values_list[index + 1], "False"))
        {
            sig_format->value[0] = 0;
        }
        else
        {
            /** Convert the struct`s value to uint32 and set it to sig_format->value[0] */
            utilex_str_stoul(decoded_values_list[index + 1], &decoded_val);
            sig_format->value[0] = decoded_val;
        }
        /** Set the size in bits*/
        sig_format->size = size;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Convert dbal data to signals` names and values
 *
 * \param [in] unit - Relevant unit
 * \param [in] field_id - Field id
 * \param [in] data - The original data from the signal
 * \param [in] decoded_data_string - The decoded data
 * \param [in] field_info - Field type info
 * \param [in] encode_type - The encode type of the dbal field
 * \param [in] data_size_bits - The size of the dbal field in bits
 * \param [out] decoded_values - The values that are result from the splitting of decoded_data_string
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_string_convert_to_signal_string(
    int unit,
    dbal_fields_e field_id,
    uint32 *data,
    char *decoded_data_string,
    dbal_field_types_basic_info_t * field_info,
    dbal_value_field_encode_types_e encode_type,
    uint32 data_size_bits,
    rhlist_t * decoded_values)
{
    dbal_fields_e sub_field_id;
    uint32 sub_field_val = 0;
    uint32 nof_values = 0;
    uint32 decoded_val = 0;
    int index = 0;
    rhhandle_t sig_handle;
    int ii = 0;
    dbal_field_types_basic_info_t *struct_member_field_info = NULL;
    dbal_fields_e struct_member_field_id;
    signal_output_t *sig_format = NULL;
    char **decoded_values_list = NULL;
    dbal_field_types_basic_info_t *subfield_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    if (field_info->nof_child_fields > 0)
    {
        /** Get sub_field_id */
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, field_id, data, &sub_field_id, &sub_field_val));
        SHR_IF_ERR_EXIT(dbal_fields_encode_type_get(unit, sub_field_id, &encode_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_fields_encode_type_get(unit, field_id, &encode_type));
    }

    /*
     * In case of structure with members from type structure the decoded_data_string has the following format:
     * 'StructMember value_in_hex (Substruct_member_name1 value; substruct_member_name2 value; etc)
     * If the substruct member is enum - the parsing is EnumVal_Name EnumVal (value_in_hex)
     * It is needed to remove some special characters in order to split decoded_data_string properly
     */
    if (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCT)
    {
        /** Replace in decoded_data_string the occurrence of ';' with whitespace */
        utilex_str_replace(decoded_data_string, ';', ' ');
        /** Replace in decoded_data_string the occurrence of '(' with whitespace */
        utilex_str_replace(decoded_data_string, '(', ' ');
        /** Replace in decoded_data_string the occurrence of ')' with whitespace */
        utilex_str_replace(decoded_data_string, ')', ' ');
        /** Replace multiple decoded_data_string and/or tabs to 1 whitespace and remove them from the end */
        utilex_str_shrink(decoded_data_string);
        /** Split 'decoded_data_string' into decoded values and keep them in decoded_values_list */
        if ((decoded_values_list = utilex_str_split(decoded_data_string, " ", 20, &nof_values)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to parse decoded_data_string: %s\n", decoded_data_string);
        }
        if (field_info->nof_child_fields > 0)
        {
            /** Get the sub_field_id of the child field */
            SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, decoded_values_list[0], &sub_field_id));
        }
    }
    else
    {
        /** Replace in decoded_data_string the occurrence of '(' with whitespace */
        utilex_str_replace(decoded_data_string, '(', ' ');
        /** Replace in decoded_data_string the occurrence of ')' with whitespace */
        utilex_str_replace(decoded_data_string, ')', ' ');
        /** Replace multiple decoded_data_string and/or tabs to 1 whitespace and remove them from the end */
        utilex_str_shrink(decoded_data_string);
        /** If the field_type is define or enum the tokens for splitting are maximum 2 */
        if ((decoded_values_list = utilex_str_split(decoded_data_string, " ", 3, &nof_values)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to parse decoded_data_string: %s\n", decoded_data_string);
        }
    }

    if ((field_info->nof_enum_values > 0) || (field_info->nof_child_fields > 0))
    {
        /** Convert the decoded value to uint32. It will be set as signal`s value */
        utilex_str_stoul(decoded_values_list[1], &decoded_val);
    }

    if (NULL == sig_format)
    {
        if (field_info->nof_defines_values > 0)
        {
            /*
             * In case of defines it is needed to increment nof_values
             * Otherwise the for loop will be skipped
             */
            nof_values = nof_values + 1;
        }
        /** Add the decoded values to rhlist */
        for (index = 0; index < nof_values - 1; index++)
        {
            SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(decoded_values, "DBAL", RHID_TO_BE_GENERATED, &sig_handle));
            sig_format = sig_handle;
            /** Add the size of the field in bits */
            sig_format->size = data_size_bits;

            if (field_info->nof_defines_values > 0)
            {
                /** In case of define - print 'Define Name' */
                sal_strncpy(sig_format->entry.name, "Define_Name", RHNAME_MAX_SIZE);

                for (ii = 0; ii < field_info->nof_defines_values; ii++)
                {
                    if (field_info->defines_info[ii].name == decoded_values_list[0])
                    {
                        /** Set defines`s value to signal`s value */
                        sig_format->value[0] = field_info->defines_info[ii].value;
                        break;
                    }
                }
            }
            else if (field_info->nof_enum_values > 0)
            {
                /** In case of enum - print 'EnumVal' */
                sal_strncpy(sig_format->entry.name, "EnumVal", RHNAME_MAX_SIZE);
                for (ii = 0; ii < field_info->nof_enum_values; ii++)
                {
                    if (field_info->enum_val_info[ii].name == decoded_values_list[0])
                    {
                        /** Set enum`s value to signal`s value */
                        sig_format->value[0] = field_info->enum_val_info[ii].value;
                        index = index + 1;
                        break;
                    }
                }
            }
            else if (field_info->nof_child_fields > 0)
            {
                SHR_IF_ERR_EXIT(dbal_parent_field_string_convert_to_signal_string
                                (unit, decoded_val, index, encode_type, decoded_values_list,
                                 sub_field_id, field_info, data_size_bits, sig_format));
                if ((index > 1) && (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCT))
                {
                    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, decoded_values_list[index], &struct_member_field_id));
                    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, struct_member_field_id, &subfield_info));
                    index++;
                    if (subfield_info->nof_enum_values > 0)
                    {
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index]);
                        /** When the child field is enum, it is printed the EnumVal name and value. Use only the name, skip the value */
                        index++;
                    }
                    else if (!sal_strcmp(decoded_values_list[index], "True")
                             || !sal_strcmp(decoded_values_list[index], "False"))
                    {
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index]);
                    }
                    else
                    {
                        /** Convert the decoded value to uint32 */
                        utilex_str_stoul(decoded_values_list[index], &decoded_val);
                        /** Copy the decoded value to the  print_value of sig_format */
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%x", decoded_val);
                    }
                }
            }
            else
            {
                /** Increment the index because the first structure`s element has index 1 */
                index++;
                sal_strncpy(sig_format->entry.name, decoded_values_list[index], RHNAME_MAX_SIZE);
                /** Get the field_type of the child field */
                SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, decoded_values_list[index], &struct_member_field_id));
                /** Get the field_info in order to understand if the struct member is enum */
                SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                                (unit, struct_member_field_id, &struct_member_field_info));

                if (struct_member_field_info->nof_enum_values > 0)
                {
                    /*
                     * If the struct member is enum it is printed its EnumVal name and its value
                     * The name should be skipped for proper assigning of the value
                     * It will be used when assigning sig_format->print_value
                     */
                    index++;
                }
                /** Set struct member`s value to signal`s value */
                if (!sal_strcmp(decoded_values_list[index + 1], "True"))
                {
                    sig_format->value[0] = 1;
                }
                else if (!sal_strcmp(decoded_values_list[index + 1], "False"))
                {
                    sig_format->value[0] = 0;
                }
                else
                {
                    /** Convert the struct`s value to uint32 and set it to sig_format->value[0] */
                    utilex_str_stoul(decoded_values_list[index + 1], &decoded_val);
                    sig_format->value[0] = decoded_val;
                }
                /** Set the size in bits*/
                sig_format->size = struct_member_field_info->max_size;
                if ((field_info->nof_child_fields == 0) && encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCT)
                {
                    if (struct_member_field_info->nof_enum_values > 0)
                    {
                        /** Copy the decoded value to the  print_value of sig_format */
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index]);

                    }
                    else
                    {
                        /** Copy the decoded value to the  print_value of sig_format */
                        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index + 1]);
                    }
                }
            }

            if ((field_info->nof_child_fields == 0) && (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCT))
            {
                continue;
            }
            else if ((field_info->nof_child_fields != 0) && (index == 1))
            {
                /** Copy the decoded value to the  print_value of sig_format */
                sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%x", decoded_val);
            }
            else if ((field_info->nof_child_fields != 0) && (index > 1))
            {
                continue;
            }
            else
            {
                /** Copy the decoded value to the  print_value of sig_format */
                sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR, "%s", decoded_values_list[index]);
            }
        }
    }

exit:
    if (decoded_values_list != NULL)
    {
        utilex_str_split_free(decoded_values_list, nof_values);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Parse dbal table to signals
 *  Read all fields of dbal table and represent their names and values as signals.
 *  The table should have only one key - core id. No additional key fields are allowed.
 *
 * \param [in] unit - Relevant unit
 * \param [in] core - Core id
 * \param [in] dbal_table_name - DBAL table name
 * \param [in] table_id - DBAL table id
 * \param [out] decoded_values - Keep the result fields of a certain dbal table and their values
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dbal_table_parse_to_signal_string(
    int unit,
    int core,
    char *dbal_table_name,
    dbal_tables_e table_id,
    rhlist_t * decoded_values)
{

    signal_output_t *sig_format = NULL;
    rhhandle_t rhhandle;
    dbal_printable_entry_t *entry_info = NULL;
    int payload_size = 0;
    int result_field_size = 0;
    uint32 result_field = 0;
    uint32 entry_handle_id;
    dbal_entry_handle_t *entry_handle;
    dbal_entry_handle_t *get_entry_handle = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /** Get entry_handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    /** Check if the table has only 1 key and if it is CORE_ID. In case it is not such kind of table - return an error */
    if ((entry_handle->table->nof_key_fields != 1)
        && (entry_handle->table->keys_info[0].field_id != DBAL_FIELD_CORE_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The dbal table: %s has not only one key field! It can be used only dbal tables with single key - CORE_ID!\n",
                     dbal_table_name);
    }

    /** Allocate get_entry_handle - it will be used for getting the dbal entry */
    SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);
    entry_handle->core_id = core;
    /** Get the dbal entry in get_entry_handle */
    SHR_IF_ERR_EXIT(dbal_actions_access_entry_get(unit, entry_handle, get_entry_handle, 0));

    entry_info = sal_alloc(sizeof(dbal_printable_entry_t), "dbal_printable_entry");
    SHR_NULL_CHECK(entry_info, _SHR_E_MEMORY, "entry_info");
    sal_memset(entry_info, 0, sizeof(dbal_printable_entry_t));
    /** Get the size of the payload */
    SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, table_id, 0, &payload_size));
    /** Get the result fields of the dbal table and their values. Keep them in entry_info */
    SHR_IF_ERR_EXIT_NO_MSG(dbal_table_printable_entry_get
                           (unit, table_id, NULL, core, payload_size, get_entry_handle->phy_entry.payload, entry_info));

    /** Add the information for all table`s result fields to sig_format */
    for (result_field = 0; result_field < entry_info->nof_res_fields; ++result_field)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                        (decoded_values, entry_info->res_fields_info[result_field].field_name, RHID_TO_BE_GENERATED,
                         &rhhandle));
        sig_format = rhhandle;
        /** Set the print_value of the result field to sig_format */
        sal_snprintf(sig_format->print_value, DSIG_MAX_SIZE_STR - 1, "%s",
                     entry_info->res_fields_info[result_field].field_print_value);
        /** Set the field_value of the result field to sig_format */
        sig_format->value[0] = *entry_info->res_fields_info[result_field].field_value;
        /** Get the size of the result field */
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                        (unit, table_id, entry_info->res_fields_info[result_field].field_id, 0, 0, 0,
                         &result_field_size));
        /** Set the field size to sig_format */
        sig_format->size = result_field_size;
    }

exit:
    SHR_FREE(get_entry_handle);
    if (entry_info)
    {
        sal_free(entry_info);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   dbal_string_mgmt.h
 */
shr_error_e
dbal_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE])
{
    dbal_fields_e field_id;
    signal_output_t *sig = NULL;
    char decoded_data_string[DBAL_MAX_PRINTABLE_BUFFER_SIZE] = { 0 };
    rhlist_t *decoded_values = NULL;
    dbal_field_types_basic_info_t *field_info;
    dbal_value_field_encode_types_e encode_type = DBAL_VALUE_FIELD_ENCODE_NONE;
    uint8 is_dbal_field = 0;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    uint32 *data_ptr;
    uint32 data_u32;
    SHR_FUNC_INIT_VARS(unit);
    /** Input validation */
    SHR_NULL_CHECK(desc, _SHR_E_PARAM, "desc");
    SHR_NULL_CHECK(parsed_info, _SHR_E_PARAM, "parsed_info");

    /** Check if desc is dbal table or field */
    if (dbal_field_string_to_id_no_error(unit, desc, &field_id) == _SHR_E_NONE)
    {
        is_dbal_field = 1;
        /** 'data' is used only in case that 'desc' is field */
        SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    }
    else
    {
        if (dbal_logical_table_string_to_id(unit, desc, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "desc: %s is neither dbal table, nor dbal field! It should be used a valid table/field name! \n",
                         desc);
        }
    }

    if (is_dbal_field)
    {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, desc, &field_id));
        /** Get the field_info in order to understand if it is define, enum or field type with child fields */
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));
        SHR_IF_ERR_EXIT(dbal_fields_encode_type_get(unit, field_id, &encode_type));

        if ((field_info->nof_defines_values == 0) && (field_info->nof_enum_values == 0)
            && (field_info->nof_child_fields == 0) && (encode_type != DBAL_VALUE_FIELD_ENCODE_STRUCT))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Only enums, defines, structures and parent fields with child fields can be decoded! %s is not supported!",
                         desc);
        }
        data_ptr = data;
        if (encode_type == DBAL_VALUE_FIELD_ENCODE_ENUM)
        {
            int enum_idx;
            for (enum_idx = 0; enum_idx < field_info->nof_enum_values; enum_idx++)
            {
                if (!field_info->enum_val_info[enum_idx].is_invalid &&
                    field_info->enum_val_info[enum_idx].value == data[0])
                {
                    data_u32 = enum_idx;
                    data_ptr = &data_u32;
                    break;
                }
            }
        }
        /** Decode the data and keep the string in 'decoded_data_string' */
        SHR_IF_ERR_EXIT(dbal_field_printable_string_get
                        (unit, field_id, data_ptr, NULL, 0, DBAL_FIELD_EMPTY, data_size_bits, decoded_data_string));

        if (sal_strncmp(decoded_data_string, "No child field found!", sal_strlen("No child field found!")) == 0)
        {
            /** In case that decoded_data_string is 'No child field found', exit from the function */
            SHR_EXIT();
        }

    }

    /** Make sure parsed_info is allocated */
    if (NULL == *parsed_info)
    {
        if ((*parsed_info = utilex_rhlist_create("dbal information", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsed information list to parse the required buffer.\n");
        }
    }

    /** Allocate list for decoded values */
    if ((decoded_values = utilex_rhlist_create("Decoded values", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsed information list to parse the required buffer.\n");
    }

    if (is_dbal_field)
    {
        SHR_IF_ERR_EXIT(dbal_string_convert_to_signal_string
                        (unit, field_id, data, decoded_data_string, field_info, encode_type, data_size_bits,
                         decoded_values));
    }
    else
    {
        /** Parse the table and keep the result fields and their values in decoded_values */
        SHR_IF_ERR_EXIT(dbal_table_parse_to_signal_string(unit, core, desc, table_id, decoded_values));
    }

    RHITERATOR(sig, decoded_values)
    {
        /** Copy the decoded values to the signal output */
        SHR_IF_ERR_EXIT(dnx_dbal_decode_field_signal_copy(unit, sig, *parsed_info));
    }

exit:
    if (decoded_values)
    {
        sand_signal_list_free(decoded_values);
    }
    SHR_FUNC_EXIT;
}
