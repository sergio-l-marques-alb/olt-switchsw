/*
 * $Id: dbal_internal.h,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef DBAL_INTERNAL_H_INCLUDED
#  define DBAL_INTERNAL_H_INCLUDED

#  include <soc/dnx/dbal/dbal.h>
#  include <shared/shrextend/shrextend_debug.h>
#  include <shared/utilex/utilex_str.h>
#  include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#  include <soc/dnx/swstate/auto_generated/types/dbal_types.h>
#  include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#  include "dbal_db_init.h"
#  include <src/soc/dnx/flush/flush_internal.h>


/**
 *  \brief
 *  Defining the number of valid cores in device.
 *  Use for CORE_ID value validation
 *  Need to be defined in DNX data.
 */
#  define DBAL_MAX_NUM_OF_CORES    (dnx_data_device.general.nof_cores_get(unit))

/**
 *  \brief
 *  Macro that returns a struct to the result info in handle according to the result type
 */
#  define DBAL_RES_INFO            entry_handle->table->multi_res_info[entry_handle->cur_res_type]

#define GET_FIELD_TYPE(_field_id, _field_type)                                                                          \
do                                                                                                                      \
{                                                                                                                       \
    if  ((_field_id) < DBAL_NOF_FIELDS)                                                                                 \
    {                                                                                                                   \
        (*_field_type) = dbal_field_id_to_type[(_field_id)];                                                            \
    }                                                                                                                   \
    else                                                                                                                \
    {                                                                                                                   \
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.get(unit, (_field_id - DBAL_NOF_FIELDS), (_field_type)));   \
    }                                                                                                                   \
} while (0)

/**
 *  \brief enum that represents all the action validations if setting action_validation_flags on the action
 */
typedef enum
{
    /** if set, in the action validation will check that all key fields exists */
    ALL_KEY_FIELDS_MUST_EXIST = SAL_BIT(1),

    /** if set, in the action validation will check that at least on result field exists */
    RESULT_FIELD_MUST_EXIST = SAL_BIT(2),

    /** if set, in the action validation will check that no result field exists */
    RESULT_FIELD_NOT_ALLOWED = SAL_BIT(3),

    /** if not set, in the action validation will check that the core id is set to a legal value */
    CORE_COULD_BE_NOT_INITIALIZED = SAL_BIT(4),

    /** if set need to check if allocator fields */
    VALIDATE_ALLOCATOR_FIELD = SAL_BIT(5),

    /** if set need to validate result type set on lif tables */
    VALIDATE_LIF_RESULT_TYPE = SAL_BIT(6)
} dbal_action_validation_flags_e;

/**
 *  \brief
 *  Enum to indicate is a key value is valid or not during iterating a table
 */
typedef enum
{
    /** The key value is valid */
    DBAL_KEY_IS_VALID,
    /** The key value is valid, but not passing the iterator rules */
    DBAL_KEY_IS_OUT_OF_ITERATOR_RULE,
    /** The key value is invalid */
    DBAL_KEY_IS_INVALID,
} dbal_key_value_validity_e;
/**
 * \brief
 * this macro is used to dump info to the consule and also to the file, meta is the information to dump
 * log - 0/1 consule mode, file - file pointer/NULL file mode\n
 */
#define DBAL_DUMP(meta, log, file)                                \
do                                                                \
{                                                                 \
    if (log)                                                      \
    {                                                             \
        LOG_CLI( (BSL_META("%s"),meta) );                         \
    }                                                             \
    if (file != NULL)                                             \
    {                                                             \
        sal_fwrite(meta, strlen(meta), 1, file);                  \
    }                                                             \
} while (0);

#define DBAL_ITERATE_OVER_CORES(entry_handle, core_id)                              \
    int first_core = 0, nof_cores = DBAL_MAX_NUM_OF_CORES;                          \
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_SBC)                       \
    {                                                                               \
        nof_cores = 1;                                                              \
        if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)           \
        {                                                                           \
            nof_cores = DBAL_MAX_NUM_OF_CORES;                                      \
        }                                                                           \
    }                                                                               \
    else if ((entry_handle->core_id != DBAL_CORE_ALL) &&                            \
             (entry_handle->core_id != DBAL_CORE_NOT_INTIATED))                     \
    {                                                                               \
        first_core = entry_handle->core_id;                                         \
        nof_cores = 1;                                                              \
    }                                                                               \
    for(core_id = first_core; core_id < first_core + nof_cores; core_id++)

/**
 * \brief
 * used for error indication when working on fields, the error
 * saved here will be used when performing action on the entry\n
 */
#define DBAL_FIELD_ERR_HANDLE(is_error, string_to_save)                                           \
do                                                                                                \
{                                                                                                 \
    if(is_error)                                                                                  \
    {                                                                                             \
        if(entry_handle->table->has_result_type && entry_handle->cur_res_type != DBAL_RESULT_TYPE_NOT_INITIALIZED)\
        {                                                                                             \
            LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s res type %s\n\n"),                          \
            string_to_save, dbal_field_to_string(unit, field_id), entry_handle->table->table_name,                        \
                                      entry_handle->table->multi_res_info[entry_handle->cur_res_type].result_type_name)); \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s\n\n"),                  \
            string_to_save, dbal_field_to_string(unit, field_id), entry_handle->table->table_name));  \
        }                                                                                             \
        entry_handle->error_info.field_id = field_id;                                             \
        entry_handle->error_info.error_exists = 1;                                                \
        return;                                                                                   \
    }                                                                                             \
} while (0);

#define DBAL_FIELD_ERR_HANDLE_NO_RETURN(is_error, string_to_save)                                           \
do                                                                                                \
{                                                                                                 \
    if(is_error)                                                                                  \
    {                                                                                             \
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s\n\n"),                  \
        string_to_save, dbal_field_to_string(unit, field_id), entry_handle->table->table_name));  \
        entry_handle->error_info.field_id = field_id;                                             \
        entry_handle->error_info.error_exists = 1;                                                \
    }                                                                                             \
} while (0);

#define DBAL_MASK_ENTRY_VALID_TABLE_CHECK(table_type, access_method)                            \
do                                                                                              \
{                                                                                               \
    DBAL_FIELD_ERR_HANDLE((table_type != DBAL_TABLE_TYPE_TCAM) &&                               \
                          (table_type != DBAL_TABLE_TYPE_TCAM_DIRECT) &&                        \
                          (table_type != DBAL_TABLE_TYPE_LPM) &&                                \
                          (access_method != DBAL_ACCESS_METHOD_TCAM_CS),                        \
                          "Illegal use for mask APIs in table type");                           \
} while (0);

/**
 * \brief
 * used for error indication when working on fields, the error
 * saved here will be used when performing action on the entry\n
 */
#define DBAL_FIELD_ERR_RETURN_HANDLE(is_error, string_to_save)                                  \
do                                                                                                \
{                                                                                                 \
    if(is_error)                                                                                  \
    {                                                                                             \
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s\n\n"),                  \
        string_to_save, dbal_field_to_string(unit, field_id), entry_handle->table->table_name));  \
        entry_handle->error_info.field_id = field_id;                                             \
        entry_handle->error_info.error_exists = 1;                                                \
        return _SHR_E_INTERNAL;                                                                   \
    }                                                                                             \
} while (0);

#define DBAL_ACCESS_PRINTS_HL                                   ("HL-----")
#define DBAL_ACCESS_PRINTS_MDB                                  ("MDB----")
#define DBAL_ACCESS_PRINTS_SW                                   ("SW-----")
#define DBAL_ACCESS_PRINTS_PEMLA                                ("PEML---")
#define DBAL_ACCESS_PRINTS_TCAM_CS                              ("TCAM_CS")
#define DBAL_ACCESS_PRINTS_KBP                                  ("KBP----")

/** log_action 0 = commit 1 = get 2 = clear */
#define DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(is_start, sevirity, type, log_action)            \
do                                                                                      \
{                                                                                       \
    int rv = bsl_check(bslLayerSocdnx, bslSourceDbalaccessdnx, bslSeverityInfo, unit);  \
    if (rv)                                                                             \
    {                                                                                   \
        if(dbal_logger_is_enable(unit, entry_handle->table_id))                             \
        {                                                                               \
            if (is_start)                                                                   \
            {                                                                               \
                LOG_CLI((BSL_META("\n")));                                                  \
            }                                                                                                     \
            LOG_CLI((BSL_META("---------------------------------ACCESS-%s------------------------------------\n"), is_start? type:"-------"));  \
            if(is_start)                                                                    \
            {                                                                               \
                LOG_CLI((BSL_META("Action: %s\n"), (log_action==0)?"Commit":(log_action==1)?"Get":"Clear"));  \
            }                                                                               \
            if (!is_start )                                                                 \
            {                                                                               \
                LOG_CLI((BSL_META("\n")));                                                  \
            }                                                                               \
        }                                                                               \
    }                                                                                   \
}                                                                                       \
while (0)

#define DBAL_PRINT_FRAME_FOR_API_PRINTS(is_start, sevirity)                             \
do                                                                                      \
{                                                                                       \
    int rv = bsl_check(bslLayerSocdnx, bslSourceDbaldnx, bslSeverityInfo, unit);\
    if (rv)                                                                             \
    {                                                                                   \
        if (is_start)                                                                   \
        {                                                                               \
            LOG_CLI((BSL_META("\n")));                                                  \
        }                                                                               \
        LOG_CLI((BSL_META("**************************************API**************************************\n")));  \
        if (!is_start)                                                                  \
        {                                                                               \
            LOG_CLI((BSL_META("\n")));                                                  \
        }                                                                               \
    }                                                                                   \
}                                                                                       \
while (0)

/**
 * \brief
 * Path to the dbal sw tabes in sw state
 */
#define DBAL_SW_STATE_TABLES dbal_db.sw_tables

/**
 * \brief
 * Path to the dbal sw tables properties in sw state
 */
#define DBAL_SW_STATE_TBL_PROP dbal_db.tables_properties

/**
 * \brief
 * Path to the dbal dynamic tables copy in sw state */
#define DBAL_TBL_IN_SW_STATE dbal_db.dbal_dynamic_tables

/**
 * \brief
 * Path to the dbal fields properties in sw state */
#define DBAL_SW_STATE_FIELD_PROP dbal_db.dbal_fields

/**
 * \brief
 * Path to the dbal PPMC result type in sw state
 */
#define DBAL_SW_STATE_PPMC_RES_TYPE dbal_db.dbal_ppmc_table_res_types

extern uint8 dbal_is_j2c(int unit);
extern uint8 dbal_is_j2a0(int unit);
extern uint8 dbal_is_j2b0(int unit);

#  define DBAL_IS_J2C                                 (dbal_is_j2c(unit))
#  define DBAL_IS_J2A0                                (dbal_is_j2a0(unit))
#  define DBAL_IS_J2B0                                (dbal_is_j2b0(unit))

/**
 * \brief
 * Indicate field_type per field.
 *
 *
 */

/**
 *\brief
 * Define the operation type for dbal hndle take internal.
 * DBAL_HANDLE_TAKE_ALLOC allocate new handle.
 * DBAL_HANDLE_TAKE_CLEAR reuse existing handle.
 */
#define DBAL_HANDLE_TAKE_ALLOC 0
#define DBAL_HANDLE_TAKE_CLEAR 1

/**
 *  \brief struct that centrelized DBAL action information */
typedef struct
{
    /** action name */
    char name[DBAL_MAX_SHORT_STRING_LENGTH];

    /** if set to 1 when logger enabled will print before the action the log, else after the action */
    int is_pre_acces_prints;

    /** flags that indicate which validation to make to the action */
    uint32 action_validation_flags;

    /** supported flags for action dbal_entry_action_flags_e */
    uint32 supported_flags;
} dbal_action_info_t;

extern const uint32 zero_buffer_to_compare[DBAL_MAX_SIZE_FOR_GLOBAL_ZERO_BUFFER];
/************** DBAL internal functionality {APIs from: dbal_internal.c} **************/
/**
 * \brief returns 1 if dbal initiated, otherwise returns 0.
 */
uint8 dbal_is_intiated(
    int unit);

/**
 * \brief sets dbal status
 */
void dbal_status_set(
    int unit,
    dbal_status_e status);

/**
 * \brief gets dbal status
 */
shr_error_e dbal_status_get(
    int unit,
    dbal_status_e * status);

/**
 * \brief set dbal deinit preformed
 */
void dbal_initiated_reset(
    int unit);

/**
 *\brief prints the action preformed and entry information
 *  The prints are in INFO level, with respect to the DBAL API
 *  severity
 */
shr_error_e dbal_action_prints(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e action);

/**
 *  \brief
 *  checks if the action can be run for table. hw_err/device_image.
 *  returns action= DBAL_ACTION_SKIP incase that the action should be skipped.
 */
shr_error_e dbal_action_skip_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e * action);

/**
 *  \brief The function should be in the begining of any DBAL access API,
 *  it takes the handle, performs validations on the entry handle inforamtion before performing the action,
 *  updating logger inforamtion printing the API logs and updating the action (according to flags)
 */
shr_error_e dbal_action_start(
    int unit,
    uint32 handle_id,
    dbal_entry_handle_t ** entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e * action);

/**
 *  \brief The function should be called at the end of action to update logging and validations
 *         executing post action logs
 */
shr_error_e dbal_action_finish(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e action);

/**
 *\brief allocate SW entry handle from pool. this handle  is used to preform table actions
 */
shr_error_e dbal_entry_handle_take_internal(
    int unit,
    dbal_tables_e table_id,
    uint32 *entry_handle_id,
    uint8 operation);

/**
 *\brief return handle to pool. must be called after taking an handle.
 *          entry APIs (commit/get/delete) will return the handle automaticlly (see also DBAL_COMMIT_KEEP_HANDLE)
 */
shr_error_e dbal_entry_handle_release_internal(
    int unit,
    uint32 entry_handle_id);

/**
 *\brief get handle info.
 */
shr_error_e dbal_entry_handle_get_internal(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_handle_t ** entry_handle);

/**
 *\brief Update entry handle relevant result fields.
 */
void dbal_entry_handle_update_result(
    int unit,
    uint32 orig_entry_handle_id,
    uint32 new_entry_handle_id);

/**
 *\brief set a key field in the handle
 */
void dbal_entry_key_field_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type);

/**
 *\brief setting range of values per a key field in the handle
 */
void dbal_entry_key_field_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 min_val,
    uint32 max_val);

/**
 *\brief set a value field in the handle
 */
void dbal_entry_value_field_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id,
    dbal_pointer_type_e pointer_type,
    uint32 *field_val,
    uint32 *field_mask);

/**
 *\brief set a pointer to a value that will be updated after entry_get
 */
void dbal_entry_value_field_request(
    int unit,
    dbal_pointer_type_e pointer_type,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id,
    void *field_val,
    void *field_mask);

/**
 *\brief copy the key field value from entry buffer to given pointer.
 *       Returns _SHR_E_NOT_FOUND if field is not found in the entry
 *       (if the field has a valid bit and it is set to zero).
 */
shr_error_e dbal_entry_key_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type);

/**
 *\brief copy the field value from entry buffer to given pointer
 */
shr_error_e dbal_entry_value_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    int in_inst_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type);

/**
 *\brief setting a rule based on dbal condition_types_e and a value to a key field in iterator.
 * Iterating a table using the iterator will return only the entries that were found by key fields that match with the
 * rule
 */
shr_error_e dbal_iterator_key_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e key_condition,
    uint32 *field_value,
    uint32 *field_mask);

/**
 *\brief setting a rule based on dbal condition_types_e
 * and a value to a result field in iterator. Iterating a table using the iterator will return only the entries that
 * were found by result fields that match with the rule
 */
shr_error_e dbal_iterator_value_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e key_condition,
    uint32 *field_value,
    uint32 *field_mask);

/**
 *\brief common simple validations for iterator, incase that the action should be skipped because of HW issue will 
 *  	 return action_skip = 1;
 */
shr_error_e dbal_iterator_common_validation(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *action_skip);

/**
 *\brief setting an action based on dbaliterator_action_types_e
 * and a value to a result field in iterator.
 * Iterating a table using the iterator will return only the entries
 * that were found by result fields that match with the rule
 */
shr_error_e dbal_iterator_action_add(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    dbal_iterator_action_types_e action_type,
    uint32 *field_value,
    uint32 *field_mask);

/**
* \brief this function increments the key buffer for direct tables to the next valid value accorind to the allocator
*        field, if no valid found will increment the phy_entry by the size of the allocator.
*   \param [in] unit - Relevant unit.
*   \param [in] entry_handle - Relevant core. If resource is per core, it must be a valid core id, otherwise, must be.
*   \param [out] is_valid_entry - return if the resource ID is allocated
*  \return
*    \retval Zero if no error found See \ref shr_error_e
*  \remark
*    None
*****************************************************/
shr_error_e dbal_iterator_increment_by_allocator(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_key_value_validity_e * is_valid_entry);

/**
 *\brief this function copy all the existing key and result fields to the entry handle from the table
 */
shr_error_e dbal_iterator_init_handle_info(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief prints entry handle buffer.
 * entry print severity is INFO level, buffer print severity
 * DEBUG level, prints all the key and value fields that was
 * added to the handle. 
 * incase format == 1 will dump the info regrdless to the severity level
 */
shr_error_e dbal_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 format);

/**
 * \brief
 * returns tru if looger lock on the correct table.
 */
int dbal_logger_is_enable(
    int unit,
    dbal_tables_e table_id);

/**
 *\brief checks if the key buffer created is legal according to the fields legal values used in iterator, when the key
 *       buffer is increased by one, it is needed to check that the new key buffer created is legal
 */
shr_error_e dbal_key_buffer_validate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_key_value_validity_e * is_valid);

/**
 * \brief checks if the current HW result type belongs to the table
 */
shr_error_e dbal_verify_result_type_is_from_table(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 *is_from_table);
/**
 *\brief update values for all the requested payload fields from the handle
 *(sets entry_handle->user_output_info[iter].returned_pointer with the parsed value).
 */
shr_error_e dbal_entry_handle_value_fields_parse(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *\brief update values for all the requested attributes from the
 * handle (sets entry_handle->attribute_info. returned_pointer
 * with the parsed value).
 */
shr_error_e dbal_entry_handle_attribute_parse(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *  \brief returning dbal status
 */
dbal_status_e dbal_mngr_status_get(
    int unit);

/**
 *\brief get field value from buffer, handles encoding and sub field. the value received id the logical value.
 */
shr_error_e dbal_field_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value);

/**
 *\brief get field physical value from buffer.
 */
shr_error_e dbal_field_physical_value_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value);

/**
 *\brief get field value from buffer, handles encoding and sub field. the value received id the logical value.
 */
shr_error_e dbal_field_mask_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value);

/**
 *\brief
 *Inserts a string represantation of input field's name and
 *value into buffer_to_print. Field should be part of the input
 *table.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] field_id       - The desired field
 *   \param [in] table_id       - desired table
 *   \param [in] field_val      - Value of the field (input buffer)
 *   \param [in] field_mask     - A mask for the field_val
 *   \param [in] result_type_idx- In case that the field is a
 *          result type, the result index is needed
 *   \param [in] is_key         - Is the field a key
 *   \param [in] is_full_buffer -
 *       1. If the field value is passed as full dbal handle
 *       buffer or just a single field that already read from
 *       buffer
 *       2. If it is full buffer, Enum values are HW values, in
 *       case single field value, enum already paresed to SW
 *       values
 *   \param [in] buffer_to_print- A buffer to contain the output
 *
 */
shr_error_e dbal_field_in_table_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    dbal_tables_e table_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int result_type_idx,
    uint8 is_key,
    uint8 is_full_buffer,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);

shr_error_e dbal_calculate_offset_from_formula(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_tables_e table_id,
    dbal_fields_e current_mapped_field_id,
    dbal_offset_formula_t * formula,
    uint32 *result);

void dbal_tables_formulas_free(
    dbal_offset_formula_t * formula,
    int formula_index_in_array);

shr_error_e dbal_lpm_prefix_length_to_mask(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_lpm_mask_to_prefix_length(
    int unit,
    dbal_entry_handle_t * entry_handle);

/************** Field internal APIs (from: dbal_fields.c) **************/

shr_error_e dbal_fields_nof_occupied_dynamic_fields_get(
    int unit,
    int *nof_occupied_fields);

shr_error_e dbal_field_types_enum_name_to_hw_value_get(
    int unit,
    dbal_field_types_defs_e field_type,
    char enum_value_name[DBAL_MAX_STRING_LENGTH],
    uint32 *hw_value);

shr_error_e dbal_fields_encode_decode_bitwise_not(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out);

uint8 dbal_fields_is_field_encoded(
    int unit,
    dbal_fields_e field_id);

/** this API helps to handle the rv from dbal_fields_field_encode incase that rv is _SHR_E_UNAVAIL there is a special
 *  handling */
shr_error_e dbal_field_encode_handle_rv(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int is_key,
    dbal_fields_e field_id,
    shr_error_e ret_val);

/** encode the field according to the field encoding incase of parent field, no need to pass the field size. returns
 *  _SHR_E_UNAVAIL if the field encoding id not supported in the specific device */
shr_error_e dbal_fields_field_encode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    int field_size,
    uint32 *field_val,
    uint32 *field_value_out);

shr_error_e dbal_fields_encode_valid_ind(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out);

shr_error_e dbal_fields_decode_valid_ind(
    int unit,
    uint32 *field_val,
    uint32 curr_nof_bits,
    uint32 *field_val_out);

shr_error_e dbal_fields_field_decode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    int field_size,
    uint32 *field_val,
    uint32 *field_value_out);

shr_error_e dbal_fields_is_basic_type(
    int unit,
    dbal_fields_e field_id,
    uint8 *is_basic_type);

shr_error_e dbal_fields_of_same_instance(
    int unit,
    dbal_fields_e field_id_1,
    dbal_fields_e field_id_2,
    uint8 *is_same_instance);

shr_error_e dbal_fields_sub_field_info_get_internal(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    dbal_sub_field_info_t * sub_field_info);

shr_error_e dbal_fields_encode_prefix(
    int unit,
    uint32 *field_val,
    uint32 prefix_val,
    uint32 value_length,
    uint32 total_field_len,
    uint32 *field_val_out);

shr_error_e dbal_fields_decode_prefix(
    int unit,
    uint32 *field_val,
    uint32 value_length,
    uint32 *field_val_out);

shr_error_e dbal_fields_encode_suffix(
    int unit,
    uint32 *field_val,
    uint32 suffix_val,
    uint32 field_length,
    uint32 total_field_len,
    uint32 *field_val_out);

/**
 *\brief checks if a field is a sub field of an other field
 */
shr_error_e dbal_fields_sub_field_match(
    int unit,
    dbal_fields_e parent_field_id,
    dbal_fields_e sub_field_id,
    uint8 *is_found);

shr_error_e dbal_fields_max_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *max_value);

shr_error_e dbal_fields_min_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *min_value);

shr_error_e dbal_fields_const_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *const_value,
    uint8 *const_value_valid);

shr_error_e dbal_fields_has_illegal_values(
    int unit,
    dbal_fields_e field_id,
    uint8 *has_illegals);

shr_error_e dbal_fields_illegal_values_get(
    int unit,
    dbal_fields_e field_id,
    int *nof_illegal_values,
    uint32 illegal_values[DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES]);

shr_error_e dbal_fields_is_illegal_value(
    int unit,
    dbal_fields_e field_id,
    uint32 value,
    uint8 *is_illegal);

shr_error_e dbal_fields_print_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_print_type_e * field_print_type);

shr_error_e dbal_fields_encode_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_value_field_encode_types_e * encode_type);

shr_error_e dbal_fields_transform_arr32_to_arr8(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type_index,
    int inst_id,
    uint32 *field_val_u32,
    uint8 *field_val,
    uint32 *field_mask_u32,
    uint8 *field_mask);

shr_error_e dbal_fields_transform_arr8_to_arr32(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type_index,
    int inst_id,
    uint8 *field_val,
    uint32 *field_val_u32,
    uint8 *field_mask,
    uint32 *field_mask_u32);

uint32 dbal_fields_instance_idx_get(
    int unit,
    dbal_fields_e field_id);

shr_error_e dbal_field_string_update(
    int unit,
    dbal_fields_e field_id,
    char field_name[DBAL_MAX_STRING_LENGTH]);

/************** Logical table APIs  (from: dbal_tables.c) **************/
shr_error_e dbal_tables_table_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t ** table);

shr_error_e dbal_fields_nof_occupied_dynamic_tables_get(
    int unit,
    int *nof_occupied_tables);

/**
 * \brief check if a given field is part of the table key fields
 * (directly or as child field)
 */
shr_error_e dbal_tables_field_is_key_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_key);

/**
 * \brief - update the entry counter of a table. In case of MDB
 *        in direct table, update the counter of the phyysical
 *        table. Do nothing if table is not counter supported
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] entry_handle - entry handle
 *   \param [in] num_of_entries_to_update - if set to a
 *          non-negative number, this number will be used to
 *          change the nof_entries. Else (if it is a negative
 *          number) nof_entries will be set to zero.
 *   \param [in] is_add_operation - TRUE for adding nof_entries,
 *          FALSE for subtracting nof_entries
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
shr_error_e dbal_tables_entry_counter_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries_to_update,
    uint8 is_add_operation);

shr_error_e dbal_tables_max_key_value_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *key_buffer);

shr_error_e dbal_tables_field_info_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t ** field_info,
    int *field_index_in_table,
    dbal_fields_e * parent_field_id);

shr_error_e dbal_tables_update_hw_error(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_tables_nof_hw_error_tables_get(
    int unit,
    int *not_tables);

shr_error_e dbal_tables_porting_info_get(
    int unit,
    hl_porting_info_t ** hl_porting_info);

shr_error_e dbal_tables_groups_info_get(
    int unit,
    hl_groups_info_t ** hl_groups_info);

/**
 *  \brief
 *  this function is relevant for direct tables, return true if for this access + table type needed merging the entry
 *  with the exsiting entry in the HW, there are access methods that are not supporting partial access and it is needed
 *  to merge the entries. (merging require to perform get before set)
 */
uint8 dbal_tables_is_merge_entries_supported(
    int unit,
    dbal_tables_e table_id);

/**
 *\brief returns TRUE if the table should be acted as non-direct table, for non-direct tables counters are supported.
 *       This means that the nof_entries is changed upon actions, and also means that COMMIT_UPDATE option is supported.
 *       For the other kind of tables - nof_entries is not used (always 0) - all entries exist upon init.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id
 * \par DIRECT OUTPUT:
 *   Boolean(uint8)
 * \par INDIRECT OUTPUT
 *       None
 */
uint8 dbal_tables_is_non_direct(
    int unit,
    dbal_tables_e table_id);
/************** Physical table APIs **************/
shr_error_e dbal_mdb_table_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 resolve_from_sw);
uint8 dbal_physical_table_is_in_lif(
    dbal_physical_tables_e physical_table_id);
uint8 dbal_physical_table_is_eedb(
    dbal_physical_tables_e physical_table_id);
uint8 dbal_logical_table_is_out_lif_allocator_eedb(
    CONST dbal_logical_table_t * table);
uint8 dbal_logical_table_is_out_rif_allocator(
    CONST dbal_logical_table_t * table);
uint8 dbal_physical_table_is_fec(
    dbal_physical_tables_e physical_table_id);
/************** Direct access APIs **************/
shr_error_e dbal_hl_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_range_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries);

shr_error_e dbal_hl_range_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries);

/**
 *  \brief
 *  build default entry that include all the key fields and all the result fields, when all those values are set to
 *  the default value */
shr_error_e dbal_hl_entry_default_entry_build(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * default_handle);

shr_error_e dbal_key_or_value_condition_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 is_key,
    dbal_access_condition_info_t * access_condition,
    int nof_conditions,
    uint8 *is_passed);

shr_error_e dbal_hl_offset_calculate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e mapped_field_id,
    uint32 default_val,
    dbal_offset_encode_info_t * encode_info,
    uint32 *calc_index);

shr_error_e dbal_hl_register_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get);

shr_error_e dbal_hl_memory_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get);

shr_error_e dbal_hl_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_hl_iterator_init(
    int unit,
    dbal_tables_e table_id,
    uint32 key_size,
    dbal_sw_table_iterator_t * sw_iterator);
shr_error_e dbal_hl_reg_default_mode_get(
    int unit,
    soc_reg_t reg,
    int *is_standard);
shr_error_e dbal_hl_reg_default_get(
    int unit,
    soc_reg_t reg,
    int copyno,
    int index,
    uint32 *entry);
shr_error_e dbal_hl_is_entry_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default);

/************** sw state tables/fields APIs **************/
shr_error_e dbal_sw_table_direct_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_direct_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int res_type_get,
    int *resolved_res_type);
shr_error_e dbal_sw_table_direct_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_direct_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_hash_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_hash_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *resolved_res_type);
shr_error_e dbal_sw_table_hash_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_hash_iterator_init(
    int unit,
    dbal_tables_e table_id,
    dbal_sw_table_iterator_t * sw_iterator);
shr_error_e dbal_sw_table_hash_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle);
/************** tcam cs tables/fields APIs **************/
shr_error_e dbal_tcam_cs_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_stage_info_get(
    int unit,
    dbal_stage_e dbal_stage,
    dbal_tcam_cs_stage_info_t * stage_info);
/************** Pemla access APIs **************/
shr_error_e dbal_pemla_table_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_iterator_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);

/************** KBP access APIs **************/
shr_error_e dbal_kbp_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *\brief lock the logger on a specific table according to user decision (will execute only prints that relevant to this table).
 */
shr_error_e dbal_logger_table_user_lock(
    int unit,
    dbal_tables_e table_id);

/**
 *\brief lock the logger on a specific table according to dbal internal decision (will execute only prints that relevant to this table).
 */
shr_error_e dbal_logger_table_internal_lock(
    int unit,
    dbal_tables_e table_id);

/**
 *\brief Internal APIs to disable/open logger
 */
void dbal_logger_internal_disable_set(
    int unit);
void dbal_logger_internal_disable_clear(
    int unit);

/**
 *\brief unlock the logger from a specific table according to dbal internal decision (will execute only prints that relevant to this table).
 */
shr_error_e dbal_logger_table_internal_unlock(
    int unit,
    dbal_tables_e table_id);

/**
 *\brief return CONST ptr to the logger info structure
 */
void dbal_logger_info_get_internal(
    int unit,
    CONST dbal_logger_info_t ** logger_info);

shr_error_e dbal_mutexes_init(
    int unit);
/**
 *\brief set logger parameters, unlocking the log and opening file if needed.
 */
shr_error_e dbal_logger_init(
    int unit);

/**
 *\brief deinit for logger, actualy just closing the file if needed.
 */
shr_error_e dbal_logger_deinit(
    int unit);

/**
 *\brief setting all handles to be available.
 */
shr_error_e dbal_handles_init(
    int unit);

/**
 *\brief validate that all handles were released.
 */
shr_error_e dbal_handles_deinit(
    int unit);

/**
 *\brief opening file for log with the requested file name.
 */
shr_error_e dbal_logger_file_open(
    int unit,
    char *file_name);

/**
 *\brief file get.
 */
shr_error_e dbal_logger_file_get(
    int unit,
    FILE ** dbal_file);
/**
 *\brief closing the out file of the logger.
 */
shr_error_e dbal_logger_file_close(
    int unit);

/**
 *\brief returns the locked table, if table == DBAL_NOF_TABLES no table is locked.
 */
shr_error_e dbal_logger_user_locked_table_get(
    int unit,
    dbal_tables_e * table_id);

/**
 *\brief Set the default value for each result field which was
 * not set.
 */
shr_error_e dbal_entry_default_values_add(
    int unit,
    dbal_entry_handle_t * entry_handle);

/************** dbal access actions APIs - APIs that access the HW**************/

shr_error_e dbal_actions_access_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_actions_access_iterator_destroy(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_actions_access_iterator_get_next(
    int unit,
    uint32 entry_handle_id,
    int *is_end,
    uint8 *continue_iterating);

shr_error_e dbal_actions_access_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief - Pre dbal access function. Utilized by features in need to execute before access is done. 
 * This function is called before any HW access. The action is called before the DBAL dispath the action to the access 
 * layers. Those API should be used to perform actions that are not part of the main flow of the DBAL and can be 
 * enabled/disabled without affecting the main flow. Each process should create a seperate function to handle the flow.
 * \param [in] unit - Device Id
 * \param [in] entry_handle - Pointer to entry handle structure
 * \param [in] get_entry_handle - Pointer to get entry handle structure
 * \param [in] action_access_type - Type of action access that will be performed
 * \param [in] action_access_func - action access function type of the caller
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_action_access_pre_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
    dbal_action_access_type_e action_access_type,
    dbal_action_access_func_e action_access_func);

/**
 * \brief - Indicate that an unsupported feature has been accessed during the current Error Recovery transaction
 * Returns error if called during an Error Recovery transaction. No error otherwise.
 * \param [in] unit - Device Id
 * \param [in] action_access_func - action access function type of the caller
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_action_access_error_recovery_invalidate(
    int unit,
    dbal_action_access_func_e action_access_func);

/**
 * \brief - Post dbal access function. Utilized by features in need to execute after access is done. 
 * This function is called after any HW access. The action is called after the DBAL dispath the action to the access 
 * layers and just after the exit label (this is the reason that it has no return value). Those API should be used to 
 * perform actions that are not part of the main flow of the DBAL and can be enabled/disabled without affecting the main 
 * flow. Each process should create a seperate function to handle the flow. 
 * \param [in] unit - Device Id
 * \param [in] entry_handle - Pointer to entry handle sturcture
 * \param [in] action_access_type - Type of action access performed
 * \param [in] action_access_func - action access function type of the caller
 * \param [in] rv - Result of executed action access function
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
void dbal_action_access_post_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_action_access_type_e action_access_type,
    dbal_action_access_func_e action_access_func,
    int rv);

/**
 *\brief Commit an entry
 *For non-direct - only new entry
 *Otherwise - any entry
 */
shr_error_e dbal_entry_commit_normal(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *\brief Supported for non-direct tables only.
 *Update an existing entry.
 */
shr_error_e dbal_entry_commit_update(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *\brief Supported for non-direct tables only.
 *If entry exists - will be updated.
 *Otherwise - will e created
 */
shr_error_e dbal_entry_commit_force(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 *\brief setting range of entries according to key ranges in the entry handle.
 * working in two modes, consecutive/non-consecutive. in consecutive mode passess to the access layer the first key and
 * the number of entries the access layer can use best way to update full range of entries (DMA). in the non-consecutive
 * mode (incase the range is not complete 0-4,66-201), update each entry. PERFORMANCE_IMPROVMENT: to find sub ranges
 */
shr_error_e dbal_entry_range_action_process(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e action);

/**
 *\brief clear entry from table, for dorect tables sets the default value in the entry, for non direct tables delete the
 *       entry.
 */
shr_error_e dbal_actions_access_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief
 *  Get entry according to keys of entry_handle.
 *
 *  If get_entry_handle is not NULL: put the result in
 *  get_entry_handle. All result fields will be returned.
 *  entry_handle will remain unchanged. get_entry_handle should
 *  be allocated outside.
 *
 *  Else: put the results in entry_handle.
 *
*/
shr_error_e dbal_actions_access_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle);

/**
 * \brief
 *  dispatch the entry to the access layer commit an entry to HW (Add or Update) According to
 *  entry_hanlde->is_for_update.
 */
shr_error_e dbal_actions_access_entry_commit(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_handle);

/**
 * \brief
 *  returns the access_id by the Key, only for TCAM tables
 */
shr_error_e dbal_actions_access_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_actions_access_result_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief
 * The function validate the table interface structure
 * The interface vaidation is mutual to all tables types
 * In addition to validation, it computes the offset of fields
 * in the entry buffer and some other general properties of the
 * table
 */
shr_error_e dbal_db_init_table_mutual_interface_validation(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table);

/**
 * \brief
 * The function add some validations and general properties
 * calculations for MDB tables
 */
shr_error_e dbal_db_init_table_mdb_validation(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table);

/**
 * \brief - Check if dbal entry is default, for direct table only.
 *
 * \param [in] unit - unit id
 * \param [in] entry_handle - dbal entry handle
 * \param [out] is_default - dbal entry is default
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_entry_direct_is_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default);

 /**
 * \brief
 * init the flush machine of the LEM
 */
shr_error_e dbal_flush_init(
    int unit);

/**
 * \brief
 * setting the mode of the flush to start collect rules
 */
shr_error_e dbal_flush_mode_collect(
    int unit);

/**
 * \brief
 * getting the mode of the flush
 */
shr_error_e dbal_flush_mode_get(
    int unit,
    int *collect_mode);

/**
 * \brief
 * returns the iterator info related to specific handle
 */
shr_error_e dbal_iterator_info_get(
    int unit,
    int entry_handle_id,
    dbal_iterator_info_t ** iterator_info);

/**
 * \brief
 * setting the flush mode to normal it means that not collecting any rules.
 */
shr_error_e dbal_flush_mode_normal(
    int unit);

/**
 * \brief
 * copying the results from secondary_entry to prim_entry
 */
shr_error_e dbal_merge_entries(
    int unit,
    dbal_entry_handle_t * prim_entry,
    dbal_entry_handle_t * secondary_entry);
#endif/*_DBAL_INCLUDED__*/
