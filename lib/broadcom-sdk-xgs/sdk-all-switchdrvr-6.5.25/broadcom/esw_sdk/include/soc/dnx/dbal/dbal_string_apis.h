/**
 *\file dbal_string_apis.h
 * Main interfaces for DBAL string apis. this file has PROTOTYPES for major DBAL APIs string based. those APIs are
 * exposed to cint and are used for bare metal.
 */

#ifndef DBAL_STRING_APIS_H_INCLUDED
#define DBAL_STRING_APIS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *STRUCTURES *
 *************/

/** STRINGABLE APIS exposed to CINT */

/**
 * \brief
 *   this API receives field name and its HW value, it returns a string that represent the HW value
 *   if the specific field has no relevant stringable value it will return empty string ("").
 *   this API is used in 3 major options
 *   1. when field has define value that represents one of the HW values.
 *   2. when field is an enum and each of its HW values has stringable meaning.
 *   3. when the field is a parent field (like destination) and it can be decoded according to one of its fields, in
 *   this case it will return the string as following "encoded_field_name val=%d"
 *
 * \param [in] unit - device ID.
 * \param [in] field_name - requested field .
 * \param [in] hw_value - the HW value related to the field.
 * \param [in] str -
 *     the string that represents the HW value of the field. Maximal value for
 *     This string is DBAL_MAX_STRING_LENGTH.
 * \par DIRECT OUTPUT:
 *   shr_error_e
 *
 *   EXAMPLES
 *  dbal_fields_string_form_hw_value_get(0, "DESTINATION", 0xC0007 --> prefix 24 + value 7, str); str = PORT_ID 7
 *  dbal_fields_string_form_hw_value_get(0, "OAM_INTERNAL_OPCODE", 3, str); str = INTERNAL_ETH_LBM
 *  dbal_fields_string_form_hw_value_get(0, "FIELD_DEFINE_EXAMPLE", 65535, str); str = INVALID
 */
shr_error_e dnx_dbal_fields_string_form_hw_value_get(
    int unit,
    char *field_name,
    uint32 hw_value,
    char str[DBAL_MAX_STRING_LENGTH]);

/**
 * \brief
 *    set a sub struct field value into full field value if the field is encoded the value that will be added to
 *    the struct will be the encoded value , if the field is subfield of
 *    the parent field will use the encoding between the subfield to the parent field
 * \param [in] unit - device ID.
 * \param [in] field_name - requested field .
 * \param [in] field_in_struct_name -  sub struct field id name
 * \param [in] field_in_struct_val - sub struct field value
 *          (input buffer)
 * \param [in] full_field_val - full field value (output
 *          buffer)
 *
 * \return
 *   int the logical value of the enum requested -\n
 *
 */
shr_error_e dnx_dbal_fields_struct_field_encode(
    int unit,
    char *field_name,
    char *field_in_struct_name,
    uint32 *field_in_struct_val,
    uint32 *full_field_val);

shr_error_e dnx_dbal_fields_struct_field_decode(
    int unit,
    char *field_name,
    char *field_in_struct_name,
    uint32 *field_in_struct_val,
    uint32 *full_field_val);

/**
 * \brief
 *   this API receives field name and enum value name, and returns the corresponding enum hw value
 * \param [in] unit - device ID.
 * \param [in] field_name - requested field .
 * \param [in] field_val_name - requested enum value .
 * \param [in] hw_val - returned enum value the HW value.
 *
 * \return
 *   int the logical value of the enum requested -\n
 *
 *   EXAMPLE: dnx_dbal_fields_enum_value_get(0,"LAYER_TYPE",
 *   "TRILL", &layer)
 */
int dnx_dbal_fields_enum_value_get(
    int unit,
    char *field_name,
    char *field_val_name,
    int *hw_val);

/**
 * \brief
 *   this API receives field name and enum value name, and returns the corresponding enum dbal value.
 *   In case the enum value name is not found in the field, it will return the error code SHR_E_PARAM, but won't fix any error inormation.
 * \param [in] unit - device ID.
 * \param [in] field_name - requested field .
 * \param [in] enum_val_name - requested enum value .
 * \param [in] dbal_val - returned enum value the DBAL value.
 *
 * \return
 *   int the logical value of the enum requested -\n
 */
int dnx_dbal_fields_enum_dbal_value_get_no_err(
    int unit,
    char *field_name,
    char *enum_val_name,
    int *dbal_val);

/**
 * \brief
 *   this API receives field name and enum value name, and returns the corresponding enum dbal value
 * \param [in] unit - device ID.
 * \param [in] field_name - requested field .
 * \param [in] enum_val_name - requested enum value .
 * \param [in] dbal_val - returned enum value the DBAL value.
 *
 * \return
 *   int the logical value of the enum requested -\n
 */
int dnx_dbal_fields_enum_dbal_value_get(
    int unit,
    char *field_name,
    char *enum_val_name,
    int *dbal_val);

/**
 *\brief returns the decoded value and sub-field according to the hw value added.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] parent_field_name -  parent field Name \n
 *   \param [in] hw_val -  field HW value (example, in destination the value encoded as port/fec...)\n
 *   \param [in] sub_field_name -  the decoded subfield name\n
 *   \param [in] sub_field_val -  a pointer to the requested the value decoded\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the sub_field_id and sub_field_value pointers \n
 *
 * EXAMPLE: dnx_dbal_fields_sub_field_info_get(0, "DESTINATION",
 * 0xC0007 --> prefix 24 + value 7, str, val);
 */

int dnx_dbal_fields_sub_field_info_get(
    int unit,
    char *parent_field_name,
    uint32 hw_val,
    char *sub_field_name,
    int *sub_field_val);

/**
 *\brief
 * allocate SW entry handle from pool. this handle  is used to
 *  Perform table actions.
 *
 *   \param [in] unit
 *   \param [in] table_name -  the requested table name to
 *          allocate handle for\n
 *   \param [out] entry_handle_id -  pointer to the returned
 *          handle ID\n
 * \return
 *   Non-zero in case of an error
 */
shr_error_e dnx_dbal_entry_handle_take(
    int unit,
    char *table_name,
    uint32 *entry_handle_id);

/**
 *\brief
 * clear SW entry handle. this function reuses a SW entry handle
 * already allocated. calling this function with the same
 * table_id will reset the handle to it's initially values.
 * calling this function with different table_id, will assign
 * the handle to the new table.
 *
 *   \param [in] unit
 *   \param [in] table_name -  the requested table name to
 *          reallocate the handle for\n
 *   \param [in] entry_handle_id -  pointer to the returned
 *          handle ID\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_clear(
    int unit,
    char *table_name,
    uint32 entry_handle_id);

/**
 *\brief
 *  return handle to pool. must be called after taking an
 *  handle.
 *
 *   \param [in] unit -  \n
 *   \param [in] entry_handle_id -  handle ID to return\n
 */
shr_error_e dnx_dbal_entry_handle_free(
    int unit,
    uint32 entry_handle_id);

/**
 *\brief sets a field value in the entry.  The number of bits taken from the array is according to the
 * field definition in the table.
 *
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_name  - the field_name to relate \n
 *   \param [in]  field_val - the value to set \n
 *
 *   \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_key_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 *field_val);

/**
 *\brief sets a enum field value in the entry.
 *
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_name  - the field_name to relate \n
 *   \param [in]  symbol_name - the value name to set \n
 *
 *   \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_key_field_symbol_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    char *symbol_name);

/**
 *\brief sets a field value with mask in the entry.  The number of bits taken from the array is according to the
 * field definition in the table.
 *
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_name  - the field_name to relate \n
 *   \param [in]  field_val - the value to set \n
 *   \param [in]  field_mask - the mask to set \n
 *
 *   \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_key_field_arr32_masked_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 *field_val,
    uint32 *field_mask);

/**
 *\brief sets a field value in the entry. The number of bits
 * taken from the array is according to the field definition in
 * the table.
 *
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_name  - the field_name to relate \n
 *   \param [in]  field_val - the value to set \n
 *
 *   \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_key_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 *field_val);

/**
 *\brief sets a field value with mask in the entry.  The number of bits taken from the array is according to the
 * field definition in the table.
 *
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_name  - the field_name to relate \n
 *   \param [in]  field_val - the value to set \n
 *   \param [in]  field_mask - the mask to set \n
 *
 *   \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_key_field_arr8_masked_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 *field_val,
    uint8 *field_mask);

/**
 *\brief sets a field value in the entry. all errors related to
 * this field will be received when commit/get/delete the entry.
 *
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_name  - the field_name to relate \n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
 *   no multiple instances in table. use INST_ALL to configure
 *   all fields instances \n
 *   \param [in]  field_val - the value to set \n
 *
 *   \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_value_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint32 *field_val);

/**
 *\brief sets a field value in the entry. all errors related to
 * this field will be received when commit/get/delete the entry.
 *
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_name  - the field_name to relate \n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
 *   no multiple instances in table. use INST_ALL to configure
 *   all fields instances \n
 *   \param [in]  symbol_name - the value name to set \n
 *
 *   \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_value_field_symbol_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    char *symbol_name);

/**
 * \brief Set the access ID on the entry. this is used for TCAM tables only.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  entry_access_id - the access ID \n
 *   \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_access_id_set(
    int unit,
    uint32 entry_handle_id,
    uint32 entry_access_id);

/**
 * \brief Allocates access ID through field API, and set the access ID on the entry. this is used for TCAM tables only.
 * Access_ID is returned to used
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  core - core id (can be SOC_CORE_ALL) \n
 *   \param [in]  priority - The entry priority in TCAM \n
 *   \param [in]  entry_access_id - the access ID \n
 *   \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_access_id_allocate_and_set(
    int unit,
    uint32 entry_handle_id,
    int core,
    uint32 priority,
    uint32 *entry_access_id);

/**
 *\brief sets a field value in the entry.
 *
 * \param [in]  unit -\n
 * \param [in]  entry_handle_id - the entry handle\n
 * \param [in]  field_name  - the field_name to relate \n
 * \param [in]  inst_id  - for multiple instances fields, the
 *   instance number use INST_SINGLE when no multiple instances
 *   in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the value to set \n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_value_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint8 *field_val);

/**
 * \brief set the entry to HW according to flags.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  flag_name - requested flag Name\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_commit(
    int unit,
    uint32 entry_handle_id,
    char *flag_name);

/**
 * \brief get all entry fields from HW.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_get(
    int unit,
    uint32 entry_handle_id);

/**
 * \brief get nof result fields
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  nof_result_field_names - pointer that will return the nof result field names
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_nof_result_field_get(
    int unit,
    uint32 entry_handle_id,
    int *nof_result_field_names);

/**
 * \brief get a result field name
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in] result_field_index - index of the result, must be < nof_result_field_names
 *   \param [in]  result_field_name - field name. Already allocated by caller
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_result_field_name_get(
    int unit,
    uint32 entry_handle_id,
    int result_field_index,
    char *result_field_name);

/**
 * \brief returns key fields from entry handle this API is used when using iterator to retrieve the key fields.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_name - the field name to relate \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_key_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

/**
 * \brief returns key fields from entry handle this API is used when using iterator to retrieve the key fields with mask.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_name - the field name to relate \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 *   \param [in]  field_mask - the pointer that will be returned the mask\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_key_field_arr32_masked_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

/**
 * \brief returns key fields from entry handle this API is used when using iterator to retrieve the key fields.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_name - the field name to relate \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_key_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

/**
 * \brief returns key fields from entry handle this API is used when using iterator to retrieve the key fields with mask.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_name - the field name to relate \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 *   \param [in]  field_mask - the pointer that will be returned the mask\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_key_field_arr8_masked_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL_FIELDS.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_name - the field name to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_value_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS]);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL_FIELDS.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_name - the field name to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  symbol_name - the pointer that will be returned the value name\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_value_field_symbol_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    char *symbol_name);

/**
 * \brief get the access ID of the entry. this is used for TCAM
 *        tables only.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  entry_access_id - pointer for returning the
 *          access ID\n
 *   \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_access_id_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL_FIELDS.
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_name - the field name to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_handle_value_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    char *field_name,
    int inst_id,
    uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES]);

/**
 * \brief clear the entry from HW .
 *
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_entry_clear(
    int unit,
    uint32 entry_handle_id);

/**
 * \brief delete all entries related to table.
 *
 *   \param [in] unit
 *   \param [in] table_name -  the table to clear\n
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_table_clear(
    int unit,
    char *table_name);

/**
 * \brief
 * returns true if the table is tcam.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_name - table name
 *    \param [in] is_tcam - indicate if the table is tcam
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    None
 */
shr_error_e dnx_dbal_table_is_tcam(
    int unit,
    char *table_name,
    uint8 *is_tcam);

/**
 * \brief
 * returns true if the table is shared between cores
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_name - table name
 *    \param [in] is_shared_between_cores - indicate if the table is shared between cores
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    None
 */
shr_error_e dnx_dbal_table_is_shared_between_cores(
    int unit,
    char *table_name,
    uint8 *is_shared_between_cores);

/**
 * \brief
 * returns the access method as string.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_name - table name
 *    \param [in] access_method - access method
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    None
 */
shr_error_e dnx_dbal_table_access_method_get(
    int unit,
    char *table_name,
    char **access_method);

#endif /* DBAL_STRING_APIS_H_INCLUDED */
