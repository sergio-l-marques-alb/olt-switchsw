/** \file diag_dnx_field_utils.h
 *
 * Diagnostics utilities to be used for DNX diagnostics of Field Module.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_UTILS_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_UTILS_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/**
 * Maximum size of the 'str' string in the DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(),
 * to be sure that the given input will be stored in. It is used only inside the
 * string verify macro, memory about it is being allocated and freed after calculations
 * or in case of an error.
 * The value of 5000 is chosen, because we assume that for the moment it can be the max
 * input, which can be parsed to the macro. It can be increased in the future.
 */
#define DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE               5000
/**
 * Maximum string size, to be used in field diagnostics for string buffers like:
 * CTX IDs, FG IDs, presel qualifiers (names, values, args, masks),
 * FFC Groups info (used/free IDs), Key IDs, compare modes, Entry IDs.
 */
#define DIAG_DNX_FIELD_UTILS_STR_SIZE               200
/**
 * \brief
 *  Verify that the size of the input value to buff_str[] is not bigger than
 *  the maximum allowed in the field diagnostics DIAG_DNX_FIELD_UTILS_STR_SIZE.
 *  In case yes, an error will be returned (_SHR_E_PARAM).
 *  Used also in case we want to concatenate the buff_str[] to the given
 *  '_current_str'. Then we will check if the '_current_str' length plus
 *  the input value length is not bigger then the total length of the '_current_str'.
 *
 * param [in] _current_str - In case of concatenation, the string to which we
 *  are going to concatenate the 'buff_str[]'. In other cases, like if we want
 *  to verify the 'buff_str[]' input, this parameter must be '0'.
 * param [in] _format - Format of the string, which will be stored in the buff_str[].
 *  Like "%d, %s, %X...".
 * param [in] __VA_ARGS__ - All input params, which are described in the given _format, to be
 *  stored in the buff_str[]. Like (num_one, str_one, hex_num_one...).
 *  In case of concatenation we should give the 'buff_str[]' and '_format' of "%s".
 *
 * return
 *   Error code (as per 'shr_error_e').
 */
#define DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(_current_str, _format, ...)                               \
{                                                                                                       \
    char *_str = NULL;                                                                                  \
    int *_current_str_addr = (int *) _current_str;                                                      \
    int _str_size = 0, _current_str_len = 0, _current_str_total_len = 0;                                \
    SHR_ALLOC(_str, DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE, "_str", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);    \
    sal_snprintf(_str, DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE, _format, __VA_ARGS__);                      \
    _str_size = sal_strnlen(_str, DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE);                                 \
    if (_current_str_addr != 0)                                                                         \
    {                                                                                                   \
        _current_str_total_len = sizeof(_current_str);                                                  \
        _current_str_len = sal_strnlen(_current_str, sizeof(_current_str));                             \
        if ((_current_str_len + _str_size) > _current_str_total_len)                                    \
        {                                                                                               \
            LOG_ERROR_EX(BSL_LOG_MODULE,                                                                \
                     "\r\n"                                                                             \
                     "Given string (%s) size (%d) is bigger then the current string size %d!%s\r\n",    \
                     _str, (_str_size + _current_str_len), _current_str_total_len, EMPTY);              \
            SHR_FREE(_str);                                                                             \
            SHR_EXIT();                                                                                 \
        }                                                                                               \
    }                                                                                                   \
    else if (_str_size > DIAG_DNX_FIELD_UTILS_STR_SIZE)                                                 \
    {                                                                                                   \
        LOG_ERROR_EX(BSL_LOG_MODULE,                                                                    \
                     "\r\n"                                                                             \
                     "Given string (%s) size (%d) is bigger then the maximum allowed size %d!%s\r\n",   \
                     _str, _str_size, DIAG_DNX_FIELD_UTILS_STR_SIZE, EMPTY);                            \
        SHR_FREE(_str);                                                                                 \
        SHR_EXIT();                                                                                     \
    }                                                                                                   \
    SHR_FREE(_str);                                                                                     \
}
/*
 * }
 */

/*
 * typedefs
 * {
 */
/*
 * }
 */

/**
 * \brief
 *  Converts field group type name to an appropriate string, which will be exposed in the diagnostic.
 * \param [in] unit - The unit number.
 * \param [in] fg_type          - Field group type
 * \param [out] converted_fg_type_p - The converted string of FG type
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - The field group type is invalid
 */
shr_error_e diag_dnx_field_utils_group_type_conversion(
    int unit,
    dnx_field_group_type_e fg_type,
    char *converted_fg_type_p);

/**
 * \brief
 *  Converts field key_size enum (dnx_field_key_length_type_e) to its corresponding
 *  value in bits (only for TCAM).
 * \param [in] unit                 - unit
 * \param [in] key_size          - Field's key size enum we want to convert
 * \param [out] key_size_in_bits - The converted key size in bits
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - Key size enum received is not of type TCAM
 */
shr_error_e diag_dnx_field_utils_key_size_conversion(
    int unit,
    dnx_field_key_length_type_e key_size,
    uint32 *key_size_in_bits);

/**
 * \brief
 *  Converts field action_size enum (dnx_field_action_length_type_e) to its corresponding
 *  value in bits (only for TCAM).
 * \param [in] unit                 - Device ID
 * \param [in] action_size          - Field's action size enum we want to convert
 * \param [out] action_size_in_bits - The converted action size in bits
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - Action size enum received is not of type TCAM
 */
shr_error_e diag_dnx_field_utils_action_size_conversion(
    int unit,
    dnx_field_action_length_type_e action_size,
    uint32 *action_size_in_bits);

/**
 * \brief
 *  Converts context cmp, hash, state mode names to an appropriate string, which will be exposed in the diagnostic.
 * \param [in] unit  - The unit number.
 * \param [in] mode  - Context cmp, hash, state mode to be converted.
 * \param [out] converted_mode_p - The converted string of Context cmp, hash, state mode
 * \param [in] is_hash  - Indicates if context mode is hash.
 * \param [in] is_state  - Indicates if context mode is state table.
 * \return
 *  \retval _SHR_E_NONE  - success
 */
shr_error_e diag_dnx_field_utils_cmp_hash_state_mode_conversion(
    int unit,
    int mode,
    char *converted_mode_p,
    int is_hash,
    int is_state);

/**
 * \brief
 *  Returns a string, which contains all valid context modes, based on given
 *  context_mode info.
 * \param [in] unit  - The unit number.
 * \param [in] context_mode  - Context mode info (cmp, hash, state table).
 * \param [out] mode_string_p - String, which will store all the context modes.
 *  Please make sure the given array of char is with size of DIAG_DNX_FIELD_UTILS_STR_SIZE.
 *
 * \return
 *  \retval _SHR_E_NONE  - success
 */
shr_error_e diag_dnx_field_utils_context_modes_string_get(
    int unit,
    dnx_field_context_mode_t context_mode,
    char *mode_string_p);

/**
 * \brief
 *  This functions returns information for compare, hash, state table modes for a given context.
 * \param [in] unit  - The unit number.
 * \param [in] context_id  - Context Id for which info should be retrieved.
 * \param [in] mode  - Context mode info (cmp, hash, state table).
 * \param [out] ctx_mode_key_usage_info_p - Structure to store the returned information about
 *  Context cmp, hash, state table modes.
 * \return
 *  \retval _SHR_E_NONE  - success
 */
shr_error_e diag_dnx_field_utils_cmp_hash_state_key_usage_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t mode,
    dnx_field_context_ipmf1_sw_info_t * ctx_mode_key_usage_info_p);

/**
 * \brief
 *    Retrieves the name of an FFC group type.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *  Field stage for which the FFC Groups info will be taken.
 * \param [in] ffc_group_num -
 *    The FFC group within the stage.
 * \param [out] ffc_group_name -
 *    The name of the field ffc group.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_ffc_groups_names_get(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int ffc_group_num,
    char ffc_group_name[DIAG_DNX_FIELD_UTILS_STR_SIZE]);

/**
 * \brief
 *    Retrieves an array with FGs attached to the given context
 *    on the given stage.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] context_id -
 *  Context ID for which the attached groups
 *  should be returned.
 * \param [in] field_stage -
 *  Field stage for which the info should be returned.
 * \param [out] nof_fgs_p -
 *    Number of valid FG IDs in fg_ids_p[].
 * \param [out] fg_ids_p -
 *    Array to store the attached FGs for the give context.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_groups_per_context_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_stage_e field_stage,
    uint32 *nof_fgs_p,
    dnx_field_group_t * fg_ids_p);

/**
 * \brief
 *    Retrieves the number of allocated FEM IDs for given Field Group.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fg_id -
 *  Field Group, to retrieve allocated FEMs.
 * \param [out] nof_fems_p -
 *  Number of allocated FEMs for the given FG.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_nof_alloc_fems_per_fg_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_fem_id_t * nof_fems_p);

/**
 * \brief
 *    Retrieves the last key information for a given DBAL table, core and key_id.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] table_id -
 *  The last key DBAL table.
 * \param [in] core_id -
 *  Core Id for which the key value will be returned.
 * \param [in] key_id -
 *  Key for which value will be returned.
 * \param [out] field_key_value_p -
 *  The value of the given key.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_last_key_table_info_get(
    int unit,
    dbal_tables_e table_id,
    bcm_core_t core_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 *field_key_value_p);

/**
 * \brief
 *    Retrieves the entry key information for an EXEM field group.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fg_info_p -
 *  Pointer, which contains all info about the EXEM FG.
 * \param [in] key_values_p -
 *  Values of the used Keys, by EXEM FG, taken from the signals.
 * \param [out] entry_key_p -
 *  The entry key info.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_exem_entry_key_info_get(
    int unit,
    dnx_field_group_full_info_t * fg_info_p,
    uint32 *key_values_p,
    dnx_field_entry_key_t * entry_key_p);

/**
 * \brief
 *    Retrieves the number of available DT entries per one TCAM bank.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fg_id -
 *  Direct table field group ID.
 * \param [in] core_id -
 *  Core ID to be used for entry_handle construction.
 * \param [out] num_available_entries_p -
 *  Number of available entries in one TCAM bank.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_dt_available_entries_get(
    int unit,
    dnx_field_group_t fg_id,
    bcm_core_t core_id,
    uint32 *num_available_entries_p);

/**
 * \brief
 *    Returns the signal value, of given DNX_QUAL, as a raw data.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] core_id - Core Id on from which signal values will be retrieved.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP (depends on the field_stage).
 * \param [in] field_stage - Field stage on which the qualifier was used.
 * \param [in] dnx_qual - DNX qualifier.
 * \param [out] out_qual_sig_value - Array to store that Qual signal raw value.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_qual_signal_value_raw_data_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    uint32 out_qual_sig_value[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY]);

/**
 * \brief
 *    Retrieves the value of given user define MetaData qualifier, from the PBUS, by given offset.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] core_id - Core Id on from which signal values will be retrieved.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP (depends on the field_stage).
 * \param [in] field_stage - Field stage on which the qualifier was used.
 * \param [in] dnx_qual - DNX qualifier.
 * \param [in] qual_attach_offset - User qualifier offset, which was set by the user in
 *  the attach info.
 * \param [out] out_qual_value - String with the constructed user qualifier value.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_user_qual_metadata_value_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    int qual_attach_offset,
    char * out_qual_value);

#endif /* DIAG_DNX_UTILS_H_INCLUDED */
