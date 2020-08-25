/*
 * $Id: dbal_fields.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include <stdlib.h>
#include <sal/core/libc.h>
#include <soc/drv.h> 
#include "dbal_internal.h"
#include <shared/bsl.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
static dbal_field_types_basic_info_t field_types_basic_info[BCM_MAX_NUM_UNITS][DBAL_NOF_FIELD_TYPES_DEF];

/** extern from the auto generated file dbal_field_to_field_types.c don't use it in the code, use FIELD_TYPE */
extern dbal_field_types_defs_e dbal_field_id_to_type[DBAL_NOF_FIELDS];

#define DBAL_FIELD_ID_VALIDATE( field_id)                                                         \
        if (field_id >= DBAL_NOF_FIELDS + DBAL_NOF_DYNAMIC_FIELDS)                                \
        {                                                                                         \
            SHR_ERR_EXIT(_SHR_E_PARAM,"Illegal field_id %d, max=%d\n", field_id, DBAL_NOF_FIELDS);\
        }

#define DBAL_FIELD_TYPE_ID_VALIDATE( field_type)                                                                \
        if (field_type >= DBAL_NOF_FIELD_TYPES_DEF || field_type == DBAL_FIELD_TYPE_DEF_EMPTY)                  \
        {                                                                                                       \
            SHR_ERR_EXIT(_SHR_E_PARAM,"Illegal field_type %d, max=%d\n", field_type, DBAL_NOF_FIELD_TYPES_DEF); \
        }

#define FIELD_TYPE(_field_id)  ((_field_id < DBAL_NOF_FIELDS)? dbal_field_id_to_type[_field_id] : dbal_dynamic_field_id_to_type[_field_id - DBAL_NOF_FIELDS])

#define FIELD_TYPE_INFO(_field_type) field_types_basic_info[unit][(_field_type)]

/*********************************************** field Types APIs ***************************************************/
shr_error_e
dbal_field_types_init(
    int unit)
{
    int field_id;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        for (field_id = 0; field_id < DBAL_NOF_DYNAMIC_FIELDS; field_id++)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.set(unit, field_id, DBAL_FIELD_TYPE_DEF_EMPTY));
        }
    }

    SHR_IF_ERR_EXIT(dbal_db_init_field_types_set_default(unit, field_types_basic_info[unit]));
    SHR_IF_ERR_EXIT(dbal_db_init_field_types(unit, DBAL_INIT_FLAGS_NONE, field_types_basic_info[unit]));
    SHR_IF_ERR_EXIT(dbal_db_init_field_types_logical_validation(unit, field_types_basic_info[unit]));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_deinit(
    int unit)
{
    int ii;
    dbal_field_types_basic_info_t *field_type_entry;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_FIELD_TYPES_DEF; ii++)
    {
        field_type_entry = &field_types_basic_info[unit][ii];
        if (field_type_entry->nof_child_fields > 0)
        {
            SHR_FREE(field_type_entry->sub_field_info);
        }
        if (field_type_entry->nof_enum_values > 0)
        {
            SHR_FREE(field_type_entry->enum_val_info);
        }
        if (field_type_entry->nof_defines_values > 0)
        {
            SHR_FREE(field_type_entry->defines_info);
        }
        if (field_type_entry->nof_struct_fields > 0)
        {
            SHR_FREE(field_type_entry->struct_field_info);
        }
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_info_get(
    int unit,
    dbal_field_types_defs_e field_type,
    CONST dbal_field_types_basic_info_t ** field_type_info)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_TYPE_ID_VALIDATE(field_type);

    (*field_type_info) = &field_types_basic_info[unit][field_type];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_struct_field_size_get(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_fields_e field_id,
    uint32 *field_size_p)
{
    dbal_field_types_basic_info_t *field_type_info;
    dbal_sub_struct_field_info_t *struct_field_info;
    int i_f;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_TYPE_ID_VALIDATE(field_type);
    SHR_NULL_CHECK(field_size_p, _SHR_E_INTERNAL, "field_size_p");

    field_type_info = &field_types_basic_info[unit][field_type];

    if ((struct_field_info = field_type_info->struct_field_info) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field Type:%s has no structure\n", field_types_basic_info[unit][field_type].name);
    }

    for (i_f = 0; i_f < field_type_info->nof_struct_fields; i_f++)
    {
        if (struct_field_info[i_f].struct_field_id == field_id)
        {
            *field_size_p = struct_field_info[i_f].length;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Structured Field Type:%s has no field:%s\n",
                 field_type_info->name, dbal_field_to_string(unit, field_id));

exit:
    SHR_FUNC_EXIT;
}

/** this API is used only in init to reveive the hw value from the string of the logical value. */
shr_error_e
dbal_field_types_enum_name_to_hw_value_get(
    int unit,
    dbal_field_types_defs_e field_type,
    char enum_value_name[DBAL_MAX_STRING_LENGTH],
    uint32 *hw_value)
{
    int ii;
    int nof_enum_vals;
    int type_name_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_TYPE_ID_VALIDATE(field_type);

    nof_enum_vals = field_types_basic_info[unit][field_type].nof_enum_values;

    type_name_size = sal_strlen(field_types_basic_info[unit][field_type].name) + 1;
    if (!nof_enum_vals)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " field type is not enum %s\n", field_types_basic_info[unit][field_type].name);
    }

    for (ii = 0; ii < nof_enum_vals; ii++)
    {

        if (!sal_strcmp
            (enum_value_name, &(field_types_basic_info[unit][field_type].enum_val_info[ii].name[type_name_size])))
        {
            (*hw_value) = field_types_basic_info[unit][field_type].enum_val_info[ii].value;
            break;
        }
    }
    if (ii == nof_enum_vals)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Decoding failed for type %s HW name %s has no logical value\n",
                     field_types_basic_info[unit][field_type].name, enum_value_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function encodes a enum field. encode enum means
 * translate its SW value to HW value
 */
static shr_error_e
dbal_field_types_encode_enum(
    int unit,
    dbal_field_types_defs_e field_type,
    uint32 sw_field_val,
    uint32 *hw_field_val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sw_field_val >= field_types_basic_info[unit][field_type].nof_enum_values)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input value %d for enum field. max value %d\n",
                     sw_field_val, field_types_basic_info[unit][field_type].nof_enum_values);
    }
    *hw_field_val = field_types_basic_info[unit][field_type].enum_val_info[sw_field_val].value;
    if (field_types_basic_info[unit][field_type].enum_val_info[sw_field_val].is_invalid)
    {
                /** checks if the value is invalid for the specific device */
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * This function decodes a enum field. decode enum means
 * translate its HW value to SW value
 */
static shr_error_e
dbal_field_types_decode_enum(
    int unit,
    dbal_field_types_defs_e field_type,
    uint32 hw_field_val,
    uint32 *sw_field_val)
{
    int ii;
    int nof_enum_vals = field_types_basic_info[unit][field_type].nof_enum_values;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < nof_enum_vals; ii++)
    {
        if (hw_field_val == field_types_basic_info[unit][field_type].enum_val_info[ii].value)
        {
            (*sw_field_val) = ii;
            break;
        }
    }
    if (ii == nof_enum_vals)
    {
        (*sw_field_val) = UTILEX_U32_MAX;
    }
    SHR_FUNC_EXIT;

}

/*********************************************** fields APIs ***************************************************/

shr_error_e
dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    (*field_type_info) = &field_types_basic_info[unit][field_type];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_defs_e * field_type)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, field_type);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_nof_occupied_dynamic_fields_get(
    int unit,
    int *nof_occupied_fields)
{
    dbal_fields_e field_id;
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    (*nof_occupied_fields) = 0;

    for (field_id = 0; field_id < DBAL_NOF_DYNAMIC_FIELDS; field_id++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.get(unit, field_id, &field_type));
        if (field_type != DBAL_FIELD_TYPE_DEF_EMPTY)
        {
            (*nof_occupied_fields)++;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_available_field_id_get(
    int unit,
    dbal_fields_e * field_id)
{
    dbal_fields_e field_index;
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    for (field_index = 0; field_index < DBAL_NOF_DYNAMIC_FIELDS; field_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.get(unit, field_index, &field_type));
        if (field_type == DBAL_FIELD_TYPE_DEF_EMPTY)
        {
            (*field_id) = field_index;
            break;
        }
    }

    if (field_index == DBAL_NOF_DYNAMIC_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No available field IDs\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_is_basic_type(
    int unit,
    dbal_fields_e field_id,
    uint8 *is_basic_type)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    /** check if both of the same type */
    if ((field_type == DBAL_FIELD_TYPE_DEF_UINT) || (field_type == DBAL_FIELD_TYPE_DEF_UINT_INV) ||
        (field_type == DBAL_FIELD_TYPE_DEF_HEXA) || (field_type == DBAL_FIELD_TYPE_DEF_BOOL) ||
        (field_type == DBAL_FIELD_TYPE_DEF_BOOL_INV) || (field_type == DBAL_FIELD_TYPE_DEF_ARRAY32) ||
        (field_type == DBAL_FIELD_TYPE_DEF_ARRAY8) || (field_type == DBAL_FIELD_TYPE_DEF_BITMAP))

    {
        *is_basic_type = TRUE;
    }
    else
    {
        *is_basic_type = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_create(
    int unit,
    dbal_field_types_defs_e field_type,
    char field_name[DBAL_MAX_STRING_LENGTH],
    dbal_fields_e * field_id)
{
    int char_index;
    char eos = '\0';
    int field_name_length;
    char field_name_edited[DBAL_MAX_STRING_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_TYPE_ID_VALIDATE(field_type);

    SHR_IF_ERR_EXIT(dbal_fields_available_field_id_get(unit, field_id));

    /*
     * Get the new field's name. If given by the user verify that the name is unique. Otherwise generate a name.
     */
    if (field_name[0] == '\0')
    {
        sal_snprintf(field_name_edited, DBAL_MAX_STRING_LENGTH, "DYNAMIC_%d", (int) (*field_id));
    }
    else
    {
        dbal_fields_e existing_field_id;
        _shr_error_t rv;

        rv = dbal_field_string_to_id_no_error(unit, field_name, &existing_field_id);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "A field with the name \"%.*s\" already exists: %d. \n",
                         DBAL_MAX_STRING_LENGTH, field_name, existing_field_id);
        }
        sal_strncpy_s(field_name_edited, field_name, DBAL_MAX_STRING_LENGTH);
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.set(unit, (*field_id), field_type));
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_id.set(unit, (*field_id), (*field_id) + DBAL_NOF_FIELDS));

    field_name_length = sal_strnlen(field_name_edited, DBAL_MAX_STRING_LENGTH);
    for (char_index = 0; char_index < field_name_length; char_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.
                        field_name.set(unit, (*field_id), char_index, &field_name_edited[char_index]));
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_name.set(unit, (*field_id), char_index, &eos));
    (*field_id) += DBAL_NOF_FIELDS;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_destroy(
    int unit,
    dbal_fields_e field_id)
{
    int sw_state_field_index = field_id - DBAL_NOF_FIELDS;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);
    if (field_id < DBAL_NOF_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot destroy static fields\n");
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.set(unit, sw_state_field_index, DBAL_FIELD_TYPE_DEF_EMPTY));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function returns true if the field is encoded, it will return false in case of struct because struct fields has
 * no encoding to be done (the logical and physical values are the same)
 */
uint8
dbal_fields_is_field_encoded(
    int unit,
    dbal_fields_e field_id)
{
    uint8 is_encoded = FALSE;
    int rv;
    dbal_field_types_defs_e field_type;

    rv = dbal_fields_field_type_get(unit, field_id, &field_type);

    if (rv)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "illegal field ID %d\n"), field_id));
    }
    switch (field_types_basic_info[unit][field_type].encode_info.encode_mode)
    {
        case DBAL_VALUE_FIELD_ENCODE_NONE:
        case DBAL_VALUE_FIELD_ENCODE_STRUCT:
            is_encoded = FALSE;
            break;

        default:
            is_encoded = TRUE;
            break;
    }
    return is_encoded;
}

/**
 * \brief
 * This function encodes a general field value with a given prefix
 */
shr_error_e
dbal_fields_encode_prefix(
    int unit,
    uint32 *field_val,
    uint32 prefix_val,
    uint32 value_length,
    uint32 total_field_len,
    uint32 *field_val_out)
{
    int code_word_length;
    uint32 field_val_internal[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 prefix_val_as_arr[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (prefix_val == 0)
    {
        code_word_length = 0;
    }
    else
    {
        code_word_length = utilex_msb_bit_on(prefix_val) + 1;
    }
    if (code_word_length + value_length > total_field_len)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Sub field does not match to the parent field size in table. total_l = %d, sub_field_l = %d, prefix_val = %d\n",
                     total_field_len, value_length, prefix_val);
    }

    prefix_val_as_arr[0] = prefix_val;

    sal_memcpy(field_val_internal, field_val, WORDS2BYTES(BITS2WORDS(total_field_len)));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val_internal, 0, value_length, field_val_out));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(prefix_val_as_arr, value_length, code_word_length, field_val_out));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function encodes a general field value with a given
 * suffix
 */
shr_error_e
dbal_fields_encode_suffix(
    int unit,
    uint32 *field_val,
    uint32 suffix_val,
    uint32 field_length,
    uint32 total_field_len,
    uint32 *field_val_out)
{
    int suffix_length;
    SHR_FUNC_INIT_VARS(unit);

    if (suffix_val == 0)
    {
        suffix_length = 0;
    }
    else
    {
        suffix_length = utilex_msb_bit_on(suffix_val) + 1;
    }
    if (suffix_length + field_length > total_field_len)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Sub field does not match to the parent field size in table. total_l = %d, sub_field_l = %d, suffix_val = %d\n",
                     total_field_len, field_length, suffix_val);
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (field_val, total_field_len - field_length, field_length, field_val_out));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(&suffix_val, 0, suffix_length, field_val_out));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function decodes a general field value with a given
 * prefix
 */
shr_error_e
dbal_fields_decode_prefix(
    int unit,
    uint32 *field_val,
    uint32 value_length,
    uint32 *field_val_out)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val, 0, value_length, field_val_out));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function decodes a general field value with a given
 * suffix
 */
static shr_error_e
dbal_fields_decode_suffix(
    int unit,
    uint32 *field_val,
    uint32 total_field_len,
    uint32 field_length,
    uint32 *field_val_out)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (field_val, total_field_len - field_length, field_length, field_val_out));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function encodes/decodes a general field value by
 * operate a bitsize not for each valid bit.
 */
shr_error_e
dbal_fields_encode_decode_bitwise_not(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out)
{
    int ii;
    int nof_words, residual_bits;
    uint32 last_word_mask;

    SHR_FUNC_INIT_VARS(unit);

    nof_words = BITS2WORDS(max_nof_bits);
    residual_bits = max_nof_bits % 32;
    last_word_mask = (residual_bits == 0) ? UTILEX_U32_MAX : UTILEX_BITS_MASK(residual_bits - 1, 0);

    for (ii = 0; ii < nof_words - 1; ii++)
    {
        field_val_out[ii] = ~field_val[ii];
    }

    field_val_out[ii] = ~field_val[ii];
    field_val_out[ii] &= last_word_mask;

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function encodes a general field value by adding a valid
 * indication bit (shifting left 1b and adding 1 as lsb).
 * Used only for dynamic encoding selection, and support values
 * where bit 31 is set.
 */
shr_error_e
dbal_fields_encode_valid_ind(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out)
{
    int msb_index, bits_to_copy, ii;

    SHR_FUNC_INIT_VARS(unit);

    msb_index = max_nof_bits - 1;

    /*
     * Verify that the MSB is zero
     */
    if (utilex_bitstream_test_bit(field_val, msb_index) && max_nof_bits <= 32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MSB cannot be set");
    }

    /*
     * Set the first bit (Valid indication)
     */
    utilex_bitstream_set_bit(field_val_out, 0);

    /*
     * Copy the value (Shifted by 1)
     */
    bits_to_copy = max_nof_bits - 1;
    ii = 0;
    while (bits_to_copy > 0)
    {
        int curr_bits_to_copy = bits_to_copy > 32 ? 32 : bits_to_copy;

        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(field_val_out, 1 + 32 * ii, curr_bits_to_copy, field_val[ii]));
        bits_to_copy -= curr_bits_to_copy;
        ii++;

    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * This function decodes a general field value by removing 1b
 * (shifting right 1b).
 * Used only for dynamic encoding selection.
 * curr_nof_bits - curr_nof_bits of the value before decoding
 */
shr_error_e
dbal_fields_decode_valid_ind(
    int unit,
    uint32 *field_val,
    uint32 curr_nof_bits,
    uint32 *field_val_out)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Copy the value (Shifted by 1)
     */
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(field_val, 1, curr_nof_bits - 1, field_val_out));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_struct_field_encode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *full_field_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                    (unit, field_id, field_in_struct_id, field_in_struct_val, NULL, full_field_val, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_arr_prefix_decode(
    int unit,
    dbal_table_field_info_t * table_field,
    uint32 field_val,
    uint32 *ret_field_val)
{
    uint32 field_min_val = 0;
    uint32 field_max_val = 0;
    uint32 tmp_field_value = 0;
    uint32 field_val_as_arr[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    field_val_as_arr[0] = field_val;

    /** validate the field value in range, the min value is the arr_prefix moved */

    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field->arr_prefix,
                                              table_field->field_nof_bits,
                                              table_field->field_nof_bits + table_field->arr_prefix_size,
                                              &field_min_val));

    tmp_field_value = ((1 << table_field->field_nof_bits) - 1);
    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field->arr_prefix,
                                              table_field->field_nof_bits,
                                              table_field->field_nof_bits + table_field->arr_prefix_size,
                                              &field_max_val));

    if (ret_field_val[0] > field_max_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field value %d Max value %d\n", ret_field_val[0], field_max_val);
    }

    if (ret_field_val[0] < field_min_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field value %d Min value %d\n", ret_field_val[0], field_min_val);
    }

    ret_field_val[0] = 0;
    /**arr prefix support */
    SHR_IF_ERR_EXIT(dbal_fields_decode_prefix(unit, field_val_as_arr, table_field->field_nof_bits, ret_field_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_struct_field_encode_masked(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *field_in_struct_mask,
    uint32 *full_field_val,
    uint32 *full_field_mask)
{
    int ii;
    int offset = 0;
    int length = 0;
    int nof_struct_fields;
    uint8 is_sub_field_found = 0;
    dbal_field_types_defs_e field_type;
    dbal_sub_struct_field_info_t *struct_field_info = NULL;
    uint32 *field_val_to_add = field_in_struct_val;
    uint32 field_mask_to_add[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;
    uint32 encoded_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    nof_struct_fields = field_types_basic_info[unit][field_type].nof_struct_fields;
    for (ii = 0; ii < nof_struct_fields; ii++)
    {
        struct_field_info = &field_types_basic_info[unit][field_type].struct_field_info[ii];
        if (struct_field_info->struct_field_id == field_in_struct_id)
        {
            break;
        }
        /**checks if the field requested is a sub field of one of the fields in the struct */
        SHR_IF_ERR_EXIT(dbal_fields_sub_field_match
                        (unit, struct_field_info->struct_field_id, field_in_struct_id, &is_sub_field_found));
        if (is_sub_field_found)
        {
            parent_field_id = struct_field_info->struct_field_id;
            break;
        }
    }

    if (ii == nof_struct_fields)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_id %s %d is not a field in field_id %s structure\n",
                     dbal_field_to_string(unit, field_id), field_id, dbal_field_to_string(unit, field_in_struct_id));
    }

    offset = field_types_basic_info[unit][field_type].struct_field_info[ii].offset;
    length = field_types_basic_info[unit][field_type].struct_field_info[ii].length;

    if (is_sub_field_found || dbal_fields_is_field_encoded(unit, field_in_struct_id))
    {
        int ret_val;
        ret_val = dbal_fields_field_encode(unit, field_in_struct_id, parent_field_id, struct_field_info->length,
                                           field_in_struct_val, encoded_field_value);
        ret_val = dbal_field_encode_handle_rv(unit, NULL, 0, field_in_struct_id, ret_val);
        SHR_IF_ERR_EXIT(ret_val);

        field_val_to_add = encoded_field_value;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit, "\n***** ENCODED structure %s before = %d after = %d field_in_struct_id = %s ****\n"),
                     dbal_field_to_string(unit, field_id), field_in_struct_val[0], encoded_field_value[0],
                     dbal_field_to_string(unit, field_in_struct_id)));
    }

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val_to_add, offset, length, full_field_val));

    if (field_in_struct_mask)
    {
        dbal_field_types_basic_info_t *field_type_info;

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_in_struct_mask, 0, length, field_mask_to_add));
        if (parent_field_id != DBAL_FIELD_EMPTY)
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_in_struct_id, &field_type_info));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(field_mask_to_add, field_type_info->max_size, (length - 1)));
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_mask_to_add, offset, length, full_field_mask));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_struct_field_decode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *full_field_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode_masked
                    (unit, field_id, field_in_struct_id, field_in_struct_val, NULL, full_field_val, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_struct_field_decode_masked(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *field_in_struct_mask,
    uint32 *full_field_val,
    uint32 *full_field_mask)
{
    int ii;
    int offset = 0;
    int length = 0;
    int nof_struct_fields;
    uint8 is_sub_field_found = 0;
    dbal_field_types_defs_e field_type;
    dbal_sub_struct_field_info_t *struct_field_info;
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;
    uint32 decoded_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if ((field_id < DBAL_NOF_FIELDS) && (field_in_struct_id < DBAL_NOF_FIELDS))
    {
        SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));
        field_in_struct_val[0] = 0;
        nof_struct_fields = field_types_basic_info[unit][field_type].nof_struct_fields;
        for (ii = 0; ii < nof_struct_fields; ii++)
        {
            struct_field_info = &field_types_basic_info[unit][field_type].struct_field_info[ii];
            if (field_types_basic_info[unit][field_type].struct_field_info[ii].struct_field_id == field_in_struct_id)
            {
                break;
            }
            /**checks if the field requested is a sub field of one of the fields in the struct */
            SHR_IF_ERR_EXIT(dbal_fields_sub_field_match
                            (unit, struct_field_info->struct_field_id, field_in_struct_id, &is_sub_field_found));
            if (is_sub_field_found)
            {
                parent_field_id = struct_field_info->struct_field_id;
                break;
            }
        }
        if (ii == nof_struct_fields)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_id %d is not a struct field of field_id %d\n", field_id,
                         field_in_struct_id);
        }
        offset = field_types_basic_info[unit][field_type].struct_field_info[ii].offset;
        length = field_types_basic_info[unit][field_type].struct_field_info[ii].length;

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(full_field_val, offset, length, field_in_struct_val));

        if (is_sub_field_found || dbal_fields_is_field_encoded(unit, field_in_struct_id))
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_decode(unit, field_in_struct_id, parent_field_id, 0,
                                                     field_in_struct_val, decoded_field_value));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit, "\n***** DECODED structure %s before = %d after = %d field_in_struct_id = %s ****\n"),
                         dbal_field_to_string(unit, field_id), field_in_struct_val[0], decoded_field_value[0],
                         dbal_field_to_string(unit, field_in_struct_id)));
            field_in_struct_val[0] = 0;
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (decoded_field_value, 0, BYTES2BITS(BITS2BYTES(length)), field_in_struct_val));
        }
        if (field_in_struct_mask)
        {
            dbal_field_types_basic_info_t *field_type_info;

            if (parent_field_id != DBAL_FIELD_EMPTY)
            {
                SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_in_struct_id, &field_type_info));
                length = field_type_info->max_size;
            }
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(full_field_mask, offset, length, field_in_struct_mask));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_id %d or %d, DBAL_NOF_FIELD_TYPES_DEF=%d\n", field_id,
                     field_in_struct_id, DBAL_NOF_FIELDS);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * this function returns the allocator resource name if the field is allocator (connected to resource allocation */

shr_error_e
dbal_fields_is_allocator_get(
    int unit,
    dbal_fields_e field_id,
    uint8 *is_allocator)
{
    dbal_field_types_defs_e field_type;
    dbal_field_types_basic_info_t *field_type_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    field_type_info = &field_types_basic_info[unit][field_type];

    (*is_allocator) = field_type_info->is_allocator;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_encode_handle_rv(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int is_key,
    dbal_fields_e field_id,
    shr_error_e ret_val)
{
    dbal_status_e dbal_status;

    SHR_FUNC_INIT_VARS(unit);

    if (ret_val == _SHR_E_UNAVAIL)
    {
                /** in this case the value is not valid for this device during init ignoring this error. after init printing an error
		 *  for key fields also need to set the handle status to ignore the action. */

        SHR_IF_ERR_EXIT(dbal_status_get(unit, &dbal_status));

        if (dbal_status == DBAL_STATUS_DEVICE_INIT_DONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U
                       (unit, "%s field %s value used 0 since no mapping in image/device for value added\n\n"),
                       is_key ? "key" : "result", dbal_field_to_string(unit, field_id)));
        }
        else
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "%s field %s value used 0 since no mapping in image/device for value added\n\n"),
                      is_key ? "key" : "result", dbal_field_to_string(unit, field_id)));
        }

        if (entry_handle && is_key)
        {
            /** key fields  required additional handling, the total operation need to be skipped since the it can overide
             *  the entry in the HW, in this case the hadnle is updated to new status that is handled when perfoming the action
             *  TBD add support */
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(ret_val);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_encode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    int field_size,
    uint32 *field_val,
    uint32 *field_value_out)
{
    dbal_field_types_defs_e field_type;
    dbal_value_field_encode_types_e encode_mode;
    uint32 in_param;
    int sub_field_size = field_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    encode_mode = field_types_basic_info[unit][field_type].encode_info.encode_mode;
    in_param = field_types_basic_info[unit][field_type].encode_info.input_param;

    field_value_out[0] = 0;
    /** using parent field encoding */
    if (parent_field_id != DBAL_FIELD_EMPTY)
    {
        dbal_sub_field_info_t sub_field_info;
        int rv;

        rv = dbal_fields_sub_field_info_get_internal(unit, field_id, parent_field_id, &sub_field_info);
        if (rv)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot find child field. child %s, parent %s\n",
                         dbal_field_to_string(unit, field_id), dbal_field_to_string(unit, parent_field_id));
        }
        encode_mode = sub_field_info.encode_info.encode_mode;
        in_param = sub_field_info.encode_info.input_param;
        sub_field_size = field_types_basic_info[unit][field_type].max_size;
    }

    switch (encode_mode)
    {
        case DBAL_VALUE_FIELD_ENCODE_PREFIX:
            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix
                            (unit, field_val, in_param, sub_field_size, field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_VALID_IND:
            SHR_IF_ERR_EXIT(dbal_fields_encode_valid_ind(unit, field_val, field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT:
            SHR_IF_ERR_EXIT(dbal_fields_encode_decode_bitwise_not(unit, field_val, field_size, field_value_out));
            break;

        case DBAL_VALUE_FIELD_ENCODE_ENUM:
            SHR_IF_ERR_EXIT_NO_MSG(dbal_field_types_encode_enum(unit, field_type, *field_val, field_value_out));
            break;

        case DBAL_VALUE_FIELD_ENCODE_NONE:
        case DBAL_VALUE_FIELD_ENCODE_STRUCT:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Code should not get here. encode mode %s, field_id %s\n",
                         dbal_field_encode_type_to_string(unit, encode_mode), dbal_field_to_string(unit, field_id));
            break;

        case DBAL_VALUE_FIELD_ENCODE_SUFFIX:
            SHR_IF_ERR_EXIT(dbal_fields_encode_suffix
                            (unit, field_val, in_param, sub_field_size, field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_ADD:
        case DBAL_VALUE_FIELD_ENCODE_SUBTRACT:
        case DBAL_VALUE_FIELD_ENCODE_MULTIPLE:
        case DBAL_VALUE_FIELD_ENCODE_HARD_VALUE:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Encode type %s is not supported yet\n",
                         dbal_field_encode_type_to_string(unit, encode_mode));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encode type (%d) for field %s\n",
                         encode_mode, dbal_field_to_string(unit, field_id));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_decode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    int field_size,
    uint32 *field_val,
    uint32 *field_value_out)
{
    dbal_field_types_defs_e field_type;
    dbal_value_field_encode_types_e encode_mode;
    int sub_field_size = field_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    encode_mode = field_types_basic_info[unit][field_type].encode_info.encode_mode;
    field_value_out[0] = 0;

    /** using parent  field encoding */
    if (parent_field_id != DBAL_FIELD_EMPTY)
    {
        dbal_sub_field_info_t sub_field_info;
        int rv;
        rv = dbal_fields_sub_field_info_get_internal(unit, field_id, parent_field_id, &sub_field_info);
        if (rv)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot find child field. child %s, parent %s\n",
                         dbal_field_to_string(unit, field_id), dbal_field_to_string(unit, parent_field_id));
        }
        encode_mode = sub_field_info.encode_info.encode_mode;
        sub_field_size = field_types_basic_info[unit][field_type].max_size;
    }

    switch (encode_mode)
    {
        case DBAL_VALUE_FIELD_ENCODE_PREFIX:
            SHR_IF_ERR_EXIT(dbal_fields_decode_prefix(unit, field_val, sub_field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_VALID_IND:
            SHR_IF_ERR_EXIT(dbal_fields_decode_valid_ind(unit, field_val, sub_field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT:
            SHR_IF_ERR_EXIT(dbal_fields_encode_decode_bitwise_not(unit, field_val, sub_field_size, field_value_out));
            break;

        case DBAL_VALUE_FIELD_ENCODE_ENUM:
            SHR_IF_ERR_EXIT(dbal_field_types_decode_enum(unit, field_type, *field_val, field_value_out));
            break;

        case DBAL_VALUE_FIELD_ENCODE_NONE:
        case DBAL_VALUE_FIELD_ENCODE_STRUCT:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Code should not get here. encode mode %s, field_id %s\n",
                         dbal_field_encode_type_to_string(unit, encode_mode), dbal_field_to_string(unit, field_id));
            break;

        case DBAL_VALUE_FIELD_ENCODE_SUFFIX:
            SHR_IF_ERR_EXIT(dbal_fields_decode_suffix(unit, field_val, field_size, sub_field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_ADD:
        case DBAL_VALUE_FIELD_ENCODE_SUBTRACT:
        case DBAL_VALUE_FIELD_ENCODE_MULTIPLE:
        case DBAL_VALUE_FIELD_ENCODE_HARD_VALUE:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Encode type %s is not supported yet\n",
                         dbal_field_encode_type_to_string(unit, encode_mode));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encode type (%d) for field %s\n",
                         encode_mode, dbal_field_to_string(unit, field_id));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_of_same_instance(
    int unit,
    dbal_fields_e field_id_1,
    dbal_fields_e field_id_2,
    uint8 *is_same_instance)
{
    dbal_field_types_defs_e field_type_1;
    dbal_field_types_defs_e field_type_2;

    SHR_FUNC_INIT_VARS(unit);

    *is_same_instance = FALSE;

    DBAL_FIELD_ID_VALIDATE(field_id_1);
    DBAL_FIELD_ID_VALIDATE(field_id_2);

    GET_FIELD_TYPE(field_id_1, &field_type_1);
    GET_FIELD_TYPE(field_id_2, &field_type_2);

    if (field_id_1 == field_id_2)
    {
        *is_same_instance = TRUE;
    }

    /** check if both of the same type */
    if (field_type_1 == field_type_2)
    {
        CONST char *tmp_string;
        int field_ids_diff;
        int field_prefix_diff;
        int field_1_strlen = 0, field_2_strlen = 0;
        int field_1_prefix_num = 0, field_2_prefix_num = 0;
        uint8 field_1_after = field_id_1 > field_id_2 ? TRUE : FALSE;

        tmp_string = dbal_field_to_string(unit, field_id_1);
        field_1_strlen = sal_strlen(tmp_string);
        if (field_1_strlen < 3)
        {
            /** size of the field is smaller then 3 it means that it cant have _X in the name */
            SHR_EXIT();
        }
        field_1_prefix_num = sal_atoi(&tmp_string[field_1_strlen - 3]);

        tmp_string = dbal_field_to_string(unit, field_id_2);
        field_2_strlen = sal_strlen(tmp_string);
        if (field_2_strlen < 3)
        {
            /** size of the field is smaller then 3 it means that it cant have _X in the name */
            SHR_EXIT();
        }
        field_2_prefix_num = sal_atoi(&tmp_string[field_2_strlen - 3]);

        field_ids_diff = (field_1_after) ? (field_id_1 - field_id_2) : (field_id_2 - field_id_1);
        field_prefix_diff = (field_1_after) ? (field_1_prefix_num - field_2_prefix_num) :
            (field_2_prefix_num - field_1_prefix_num);

        if (field_prefix_diff == field_ids_diff)
        {
            *is_same_instance = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_predefine_value_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int is_key,
    dbal_field_predefine_value_type_e value_type,
    uint32 *predef_value)
{
    dbal_table_field_info_t table_field_info;
    dbal_entry_handle_t *default_handle = NULL;
    dbal_field_types_basic_info_t *field_type_info;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_type_info));

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id, field_id, is_key,
                                               entry_handle->cur_res_type, 0, &table_field_info));

    switch (value_type)
    {
        case DBAL_PREDEF_VAL_MIN_VALUE:
            (*predef_value) = table_field_info.min_value;
            if (table_field_info.arr_prefix_size != 0)
            {
                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                          table_field_info.field_nof_bits,
                                                          table_field_info.field_nof_bits +
                                                          table_field_info.arr_prefix_size, predef_value));

                if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Min value of Enum fields with ARR prefix cannot be resolved. field: %s",
                                 dbal_field_to_string(unit, table_field_info.field_id));
                }
            }
            break;

        case DBAL_PREDEF_VAL_MAX_VALUE:
            (*predef_value) = table_field_info.max_value;
            if (table_field_info.arr_prefix_size != 0)
            {
                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                          table_field_info.field_nof_bits,
                                                          table_field_info.field_nof_bits +
                                                          table_field_info.arr_prefix_size, predef_value));

                if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Max value of Enum fields with ARR prefix cannot be resolved. field: %s",
                                 dbal_field_to_string(unit, table_field_info.field_id));
                }
            }
            break;

        case DBAL_PREDEF_VAL_DEFAULT_VALUE:
            if (field_type_info->is_default_value_valid)
            {
                (*predef_value) = field_type_info->default_value;
            }
            else if (!is_key && (entry_handle->table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
                     && entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].is_default_non_standard)
            {
                if (entry_handle->nof_key_fields != entry_handle->table->nof_key_fields)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot set the default value of the entry in case not all key fields"
                                 " were set, table %s field %s\n", entry_handle->table->table_name,
                                 dbal_field_to_string(unit, field_id));
                }

                SHR_ALLOC_SET_ZERO(default_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY,
                                   EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(dbal_hl_entry_default_entry_build(unit, entry_handle, default_handle));

                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table_field_info, field_id,
                                                           default_handle->phy_entry.payload, predef_value));
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "non default value set field %s %d \n"),
                                             dbal_field_to_string(unit, field_id), predef_value[0]));
            }
            else
            {
                if (field_type_info->max_size <= 32)
                {
                    if(table_field_info.const_value_valid)
                    {
                        *predef_value = table_field_info.const_value;
                    }
                    else
                    {
                        (*predef_value) = 0;
                        if (table_field_info.arr_prefix_size != 0)
                        {
                            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                                      table_field_info.field_nof_bits,
                                                                      table_field_info.field_nof_bits +
                                                                      table_field_info.arr_prefix_size, predef_value));

                            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                            {
                                int ii;
                                for (ii = 0; ii < field_type_info->nof_enum_values; ii++)
                                {
                                    if ((*predef_value) == field_type_info->enum_val_info[ii].value)
                                    {
                                        (*predef_value) = ii;
                                        break;
                                    }
                                }

                                if(ii == field_type_info->nof_enum_values)
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Enum with ARR prefix, default value couldn't be resolved. field: %s",
                                            dbal_field_to_string(unit, table_field_info.field_id));
                                }
                            }
                        }
                    }
                }
                else
                {
                    sal_memcpy(predef_value, zero_buffer_to_compare, BITS2BYTES(field_type_info->max_size));
                }
            }
            break;

        case DBAL_PREDEF_VAL_RESET_VALUE:
            
            (*predef_value) = 0;
            break;

        default:
            /*
             * set error in handle and exit
             */
            break;
    }

exit:
    SHR_FREE(default_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_sub_field_match(
    int unit,
    dbal_fields_e parent_field_id,
    dbal_fields_e sub_field_id,
    uint8 *is_found)
{
    dbal_sub_field_info_t sub_field_info;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    (*is_found) = 0;

    rv = dbal_fields_sub_field_info_get_internal(unit, sub_field_id, parent_field_id, &sub_field_info);
    if (!rv)
    {
        (*is_found) = 1;
    }
    else
    {
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(rv);
        }
    }

    SHR_FUNC_EXIT;
}

/** returns the field size in bits */
shr_error_e
dbal_fields_max_size_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *field_size)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*field_size) = FIELD_TYPE_INFO(field_type).max_size;

exit:
    SHR_FUNC_EXIT;
}

/** returns the max legal value of the field */
shr_error_e
dbal_fields_max_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *max_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*max_value) = FIELD_TYPE_INFO(field_type).max_value;

exit:
    SHR_FUNC_EXIT;
}

/** returns the min legal value of the field */
shr_error_e
dbal_fields_min_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *min_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*min_value) = FIELD_TYPE_INFO(field_type).min_value;

exit:
    SHR_FUNC_EXIT;
}

/** returns the const value of the field and validity flag*/
shr_error_e
dbal_fields_const_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *const_value,
    uint8 *const_value_valid)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);

    *const_value_valid = FIELD_TYPE_INFO(field_type).const_value_valid;
    *const_value = FIELD_TYPE_INFO(field_type).const_value;

exit:
    SHR_FUNC_EXIT;
}

/** returns the illegal values of the field */
shr_error_e
dbal_fields_illegal_values_get(
    int unit,
    dbal_fields_e field_id,
    int *nof_illegal_values,
    uint32 illegal_values[DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES])
{
    dbal_field_types_defs_e field_type;
    int ii;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*nof_illegal_values) = FIELD_TYPE_INFO(field_type).nof_illegal_values;
    for (ii = 0; ii < *nof_illegal_values; ii++)
    {
        illegal_values[ii] = FIELD_TYPE_INFO(field_type).illegal_values[ii];
    }

exit:
    SHR_FUNC_EXIT;
}

/** returns the illegal values of the field */
shr_error_e
dbal_fields_has_illegal_values(
    int unit,
    dbal_fields_e field_id,
    uint8 *has_illegals)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);
    GET_FIELD_TYPE(field_id, &field_type);
    if (FIELD_TYPE_INFO(field_type).nof_illegal_values > 0)
    {
        *has_illegals = TRUE;
    }
    else
    {
        *has_illegals = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** check if a given value is in the illegal value list of the field */
shr_error_e
dbal_fields_is_illegal_value(
    int unit,
    dbal_fields_e field_id,
    uint32 value,
    uint8 *is_illegal)
{
    dbal_field_types_defs_e field_type;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    *is_illegal = FALSE;

    GET_FIELD_TYPE(field_id, &field_type);
    for (ii = 0; ii < FIELD_TYPE_INFO(field_type).nof_illegal_values; ii++)
    {
        if (value == FIELD_TYPE_INFO(field_type).illegal_values[ii])
        {
            *is_illegal = TRUE;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_print_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_print_type_e * field_print_type)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*field_print_type) = FIELD_TYPE_INFO(field_type).print_type;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_encode_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_value_field_encode_types_e * encode_type)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*encode_type) = FIELD_TYPE_INFO(field_type).encode_info.encode_mode;

exit:
    SHR_FUNC_EXIT;
}

/**************************************************General usage APIs (outside of dbal *************************************************/

shr_error_e
dbal_fields_uint32_sub_field_info_get(
    int unit,
    dbal_fields_e field_id,
    uint32 orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val)
{
    dbal_field_types_defs_e field_type;
    uint32 orig_value_arr[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 sub_field_val_arr[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);

    if (field_types_basic_info[unit][field_type].max_size > 32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, this API support only 32 bit parent fields, field %s is %d bits \n",
                     dbal_field_to_string(unit, (field_id)), field_types_basic_info[unit][field_type].max_size);
    }

    orig_value_arr[0] = orig_val;

    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, field_id, orig_value_arr, sub_field_id, sub_field_val_arr));

    (*sub_field_val) = sub_field_val_arr[0];

exit:
    SHR_FUNC_EXIT;
}

/** returns the sub_field_info for a sepcific field, if not found returns e_not_found */
shr_error_e
dbal_fields_sub_field_info_get_internal(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    dbal_sub_field_info_t * sub_field_info)
{
    int iter;
    dbal_field_types_defs_e parent_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, parent_field_id, &parent_type));

    for (iter = 0; iter < field_types_basic_info[unit][parent_type].nof_child_fields; iter++)
    {
        dbal_fields_e sub_field_id = field_types_basic_info[unit][parent_type].sub_field_info[iter].sub_field_id;
        if (sub_field_id == field_id)
        {
            (*sub_field_info) = (field_types_basic_info[unit][parent_type].sub_field_info[iter]);
            break;
        }
    }

    if (iter == field_types_basic_info[unit][parent_type].nof_child_fields)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_parent_field_decode(
    int unit,
    dbal_fields_e field_id,
    uint32 *orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val)
{
    int iter;
    uint32 decoded_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 re_encoded_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_field_types_defs_e field_type, sub_field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);

    (*sub_field_id) = DBAL_FIELD_EMPTY;
    (*sub_field_val) = 0;

    if (field_types_basic_info[unit][field_type].nof_child_fields == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "field %s has no child fields\n", dbal_field_to_string(unit, field_id));
    }

    /** looking for subfield that fits to the HW value, meaning that decoding and ecoding will return the same value.. */
    for (iter = 0; iter < field_types_basic_info[unit][field_type].nof_child_fields; iter++)
    {
        dbal_fields_e curr_sub_field_id = field_types_basic_info[unit][field_type].sub_field_info[iter].sub_field_id;
        if (curr_sub_field_id != DBAL_FIELD_EMPTY)
        {
            sal_memset(decoded_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            sal_memset(re_encoded_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            SHR_IF_ERR_EXIT(dbal_fields_field_decode(unit, curr_sub_field_id, field_id,
                                                     field_types_basic_info[unit][field_type].max_size, orig_val,
                                                     decoded_value));
            SHR_IF_ERR_EXIT(dbal_fields_field_encode
                            (unit, curr_sub_field_id, field_id, field_types_basic_info[unit][field_type].max_size,
                             decoded_value, re_encoded_value));
            if (sal_memcmp
                (re_encoded_value, orig_val,
                 WORDS2BYTES(BITS2WORDS(field_types_basic_info[unit][field_type].max_size))) == 0)
            {
                int size_to_copy;
                GET_FIELD_TYPE(curr_sub_field_id, &sub_field_type);
                (*sub_field_id) = curr_sub_field_id;
                size_to_copy = BYTES2BITS(BITS2BYTES(field_types_basic_info[unit][sub_field_type].max_size));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(decoded_value, 0, size_to_copy, sub_field_val));
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_parent_field_decode_no_err(
    int unit,
    dbal_fields_e field_id,
    uint32 orig_val[],
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val)
{
    int iter;
    uint32 decoded_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 re_encoded_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_field_types_defs_e field_type, sub_field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);

    (*sub_field_id) = DBAL_FIELD_EMPTY;

    if (field_types_basic_info[unit][field_type].nof_child_fields == 0)
    {
        SHR_EXIT();
    }

    /** looking for subfield that fits to the HW value, meaning that decoding and ecoding will return the same value.. */
    for (iter = 0; iter < field_types_basic_info[unit][field_type].nof_child_fields; iter++)
    {
        dbal_fields_e curr_sub_field_id = field_types_basic_info[unit][field_type].sub_field_info[iter].sub_field_id;
        if (curr_sub_field_id != DBAL_FIELD_EMPTY)
        {
            sal_memset(decoded_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            sal_memset(re_encoded_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            SHR_IF_ERR_EXIT(dbal_fields_field_decode(unit, curr_sub_field_id, field_id,
                                                     field_types_basic_info[unit][field_type].max_size, orig_val,
                                                     decoded_value));
            SHR_IF_ERR_EXIT(dbal_fields_field_encode
                            (unit, curr_sub_field_id, field_id, field_types_basic_info[unit][field_type].max_size,
                             decoded_value, re_encoded_value));
            if (sal_memcmp
                (re_encoded_value, orig_val,
                 WORDS2BYTES(BITS2WORDS(field_types_basic_info[unit][field_type].max_size))) == 0)
            {
                int size_to_copy;
                GET_FIELD_TYPE(curr_sub_field_id, &sub_field_type);
                (*sub_field_id) = curr_sub_field_id;
                size_to_copy = BYTES2BITS(BITS2BYTES(field_types_basic_info[unit][sub_field_type].max_size));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(decoded_value, 0, size_to_copy, sub_field_val));
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

uint32
dbal_fields_instance_idx_get(
    int unit,
    dbal_fields_e field_id)
{
    char field_name[DBAL_MAX_STRING_LENGTH];
    int name_len;
    int inst_idx;

    sal_strncpy(field_name, dbal_field_to_string(unit, field_id), DBAL_MAX_STRING_LENGTH - 1);
    field_name[DBAL_MAX_STRING_LENGTH - 1] = '\0';
    name_len = sal_strlen(field_name);

    if ((field_name[name_len - 4] == '_') && (field_name[name_len - 3] >= '0') && (field_name[name_len - 3] <= '9') &&
        (field_name[name_len - 2] >= '0') && (field_name[name_len - 2] <= '9') && (field_name[name_len - 1] >= '0') &&
        (field_name[name_len - 1] <= '9'))
    {
        inst_idx =
            (field_name[name_len - 3] - '0') * 100 + (field_name[name_len - 2] - '0') * 10 + (field_name[name_len - 1] -
                                                                                              '0');
    }
    else
    {
        inst_idx = 0;
    }

    return inst_idx;
}

shr_error_e
dbal_fields_parent_field32_value_set(
    int unit,
    dbal_fields_e parent_field_id,
    dbal_fields_e sub_field_id,
    uint32 *sub_field_val,
    uint32 *parent_field_val)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(sub_field_id);

    GET_FIELD_TYPE(parent_field_id, &field_type);
    SHR_IF_ERR_EXIT(dbal_fields_field_encode(unit, sub_field_id, parent_field_id,
                                             FIELD_TYPE_INFO(field_type).max_size, sub_field_val, parent_field_val));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_enum_hw_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 enum_value,
    uint32 *hw_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    if (enum_value >= FIELD_TYPE_INFO(field_type).nof_enum_values)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input value for enum field %s. max value %d\n",
                     FIELD_TYPE_INFO(field_type).name, FIELD_TYPE_INFO(field_type).nof_enum_values);
    }

    *hw_value = FIELD_TYPE_INFO(field_type).enum_val_info[enum_value].value;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_enum_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 hw_value,
    uint32 *enum_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    SHR_IF_ERR_EXIT(dbal_field_types_decode_enum(unit, field_type, hw_value, enum_value));

exit:
    SHR_FUNC_EXIT;
}
