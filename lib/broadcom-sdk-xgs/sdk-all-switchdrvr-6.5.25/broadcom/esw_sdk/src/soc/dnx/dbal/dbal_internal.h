/*
 * $Id: dbal_internal.h,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef DBAL_INTERNAL_H_INCLUDED
#define DBAL_INTERNAL_H_INCLUDED

#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <soc/dnx/swstate/auto_generated/types/dbal_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "dbal_db_init.h"
#include <src/soc/dnx/flush/flush_internal.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <src/soc/dnx/dbal/dbal_formula_cb.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_cb_formula.h>
#include <soc/dnxc/dnxc_verify.h>
#include <src/soc/dnx/dbal/auto_generated/dbal_ac_general_outputs.h>

uint8 dbal_is_dnx2(
    int unit);

#define DBAL_IS_DNX2(unit)                           (dbal_is_dnx2(unit))


/**
 *  \brief
 *  define that indicate that DBAL verifications are enabled, this define related to entry management
 */
#define DBAL_RUNTIME_VERIFICATIONS_ENABLED(_unit) DNXC_VERIFY_ALLOWED_GET(_unit)

/**
 *  \brief
 *  this macro indicates that the code called inside it is used for verifications only. that code can be rmoved to boost
 *  the performance.
 */
#define DBAL_VERIFICATIONS(_expr)                \
if(DBAL_RUNTIME_VERIFICATIONS_ENABLED(unit))     \
{                                                \
  SHR_IF_ERR_EXIT(_expr);                        \
}

/**
 * These constants compact the triplets 'layer,source,severity' for the two
 * combinations used by DBAL. If neither corresponding 'enable' flags is set
 * then no DBAL logging is carried out.
 * It is input to bsl_fast_check().
 * See DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_VOID
 * See DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_ERR
 */
#define BSL_SOURCE_DBALACCESS_DNX_CHK \
    (BSL_LAYER_ENC(bslLayerSocdnx) | BSL_SOURCE_ENC(bslSourceDbalaccessdnx) | BSL_SEVERITY_ENC(bslSeverityInfo))
#define BSL_SOURCE_DBAL_DNX_CHK \
    (BSL_LAYER_ENC(bslLayerSocdnx) | BSL_SOURCE_ENC(bslSourceDbaldnx) | BSL_SEVERITY_ENC(bslSeverityInfo))

dbal_field_types_defs_e dbal_field_id_to_field_type(
    int unit,
    dbal_fields_e dbal_field);

#define GET_FIELD_TYPE(_unit, _field_id, _field_type)                                                                          \
do                                                                                                                      \
{                                                                                                                       \
    if  ((_field_id) < DBAL_NOF_FIELDS)                                                                                 \
    {                                                                                                                   \
        (*_field_type) = dbal_field_id_to_field_type(_unit, _field_id);                                                 \
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

    /** if set, the action validation will check that at least one result field was set.
     * In get operation, it means at least one value field needs to be requested */
    RESULT_FIELD_MUST_BE_SET = SAL_BIT(2),

    /** if set, in the action validation will check that no result field exists */
    RESULT_FIELD_NOT_ALLOWED = SAL_BIT(3),

    /** if not set, in the action validation will check that the core id is set to a legal value */
    CORE_COULD_BE_NOT_INITIALIZED = SAL_BIT(4),

    /** if set need to check if allocator fields */
    VALIDATE_ALLOCATOR_FIELD = SAL_BIT(5),

    /** if set, check the core id is set to only one of the cores (DBAL_CORE_ALL not allowed) */
    CORE_ID_MUST_BE_SET_TO_ONE_CORE_ONLY = SAL_BIT(6)
} dbal_action_validation_flags_e;

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
        sal_fwrite(meta, sal_strnlen(meta,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH), 1, file);                  \
    }                                                             \
} while (0);

/*
* Encode the field instance index in the last 8 MSB of the field id
* The field_id itself uses 24 LSBs
*/
#define DBAL_ENCODE_FIELD_ID_WITH_INSTANCE(output, field_id, inst_idx)\
do                                                                    \
{                                                                     \
    output = field_id | (inst_idx << 24);                             \
} while(0);

/*
 * Decodes and retrieves field instance from the field_id
 * then restore field_id value
 */
#define DBAL_DECODE_FIELD_ID_WITH_INSTANCE(field_id, inst_idx) \
do                                                             \
{                                                              \
    inst_idx = (field_id & 0xFF000000) >> 24;                  \
    field_id &= 0x00FFFFFF;                                    \
} while(0);

#define DBAL_ITERATE_OVER_CORES(entry_handle, core_id)                              \
    int first_core = 0, nof_cores = DBAL_MAX_NUM_OF_CORES;                          \
    if(DBAL_MAX_NUM_OF_CORES > 1)                                                         \
    {                                                                               \
        if (entry_handle->table->core_mode == DBAL_CORE_MODE_SBC)                   \
        {                                                                           \
            nof_cores = 1;                                                          \
            if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)       \
            {                                                                       \
                nof_cores = DBAL_MAX_NUM_OF_CORES;                                  \
            }                                                                       \
        }                                                                           \
        else if ((entry_handle->core_id != DBAL_CORE_ALL) &&                        \
                 (entry_handle->core_id != DBAL_CORE_NOT_INITIALIZED))                 \
        {                                                                           \
            first_core = entry_handle->core_id;                                     \
            nof_cores = 1;                                                          \
        }                                                                           \
    }                                                                               \
    else if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)          \
    {                                                                               \
        first_core = 0;                                                             \
        nof_cores = entry_handle->table->nof_physical_tables;                       \
    }                                                                               \
    for(core_id = first_core; core_id < first_core + nof_cores; core_id++)

/**
 * \brief
 * used for error indication when working on fields, the error is
 * saved here and will be checked when performing action on the entry\n
 * inst_id argument is relevant only for value fields. 0 needs to be
 * passed for key fields.
 */
#define DBAL_FIELD_ERR_HANDLE(is_error, string_to_save, inst_id)                                           \
do                                                                                                \
{                                                                                                 \
    if(is_error)                                                                                  \
    {                                                                                             \
        char field_string[DBAL_MAX_STRING_LENGTH];                                                                     \
        if(SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE) &&  \
           entry_handle->cur_res_type != DBAL_RESULT_TYPE_NOT_INITIALIZED)                        \
        {                                                                                         \
            LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s res type %s\n\n"),\
            string_to_save, dbal_field_with_inst_to_string(unit, field_id, inst_id, field_string), entry_handle->table->table_name,                        \
                                      entry_handle->table->multi_res_info[entry_handle->cur_res_type].result_type_name)); \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s\n\n"),                \
            string_to_save, dbal_field_with_inst_to_string(unit, field_id, inst_id, field_string), entry_handle->table->table_name));  \
        }                                                                                             \
        entry_handle->error_info.field_id = field_id;                                                 \
        entry_handle->error_info.inst_idx = inst_id;                                                  \
        entry_handle->error_info.error_exists = _SHR_E_INTERNAL;                                      \
        if(!dbal_image_name_is_std_1(unit))                                                           \
        {                                                                                             \
            entry_handle->error_info.error_exists = _SHR_E_UNAVAIL;                                   \
        }                                                                                             \
        return;                                                                                       \
    }                                                                                                 \
} while (0);

#define DBAL_FIELD_ERR_HANDLE_NO_RETURN(error_code, string_to_save)                               \
do                                                                                                \
{                                                                                                 \
    if(error_code)                                                                                \
    {                                                                                             \
        char field_string[DBAL_MAX_STRING_LENGTH];                                                                     \
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit, "%s - field %s table %s\n\n"),               \
        string_to_save,                                                                           \
        dbal_field_with_inst_to_string(unit, field_id, table_field->inst_idx, field_string),      \
        entry_handle->table->table_name));                                                        \
        entry_handle->error_info.field_id = field_id;                                             \
        entry_handle->error_info.inst_idx = table_field->inst_idx;                                \
        entry_handle->error_info.error_exists = error_code;                                       \
    }                                                                                             \
} while (0);

#define  DBAL_MASKED_TABLE_TYPE(table_type)                                                     \
                          ((table_type != DBAL_TABLE_TYPE_TCAM) &&                              \
                          (table_type != DBAL_TABLE_TYPE_TCAM_DIRECT) &&                        \
                          (table_type != DBAL_TABLE_TYPE_TCAM_BY_ID) &&                        \
                          (table_type != DBAL_TABLE_TYPE_LPM))

/** supported table types for masked APIs */
#define DBAL_MASK_ENTRY_VALID_TABLE_CHECK(table_type, access_method)                            \
do                                                                                              \
{                                                                                               \
    DBAL_FIELD_ERR_HANDLE(DBAL_MASKED_TABLE_TYPE(table_type),                                   \
                          "Illegal use for mask APIs in table type", 0);                           \
} while (0);

/**
 * \brief
 * used for error indication when working on fields, the error
 * saved here will be checked when performing action on the entry\n
 */
#define DBAL_FIELD_ERR_RETURN_HANDLE(is_error, string_to_save)                                             \
do                                                                                                         \
{                                                                                                          \
    if(is_error)                                                                                           \
    {                                                                                                      \
        char field_str[DBAL_MAX_STRING_LENGTH] = "";                                                                            \                                                                                              \
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit, "%s - field %s table %s\n\n"),                          \
                string_to_save,                                                                            \
                dbal_field_with_inst_to_string(unit, field_id, table_field->inst_idx, field_str),          \
                entry_handle->table->table_name));                                                         \
                entry_handle->error_info.field_id = field_id;                                              \
                entry_handle->error_info.inst_idx = table_field->inst_idx;                                 \
        entry_handle->error_info.error_exists = _SHR_E_INTERNAL;                                           \
        return _SHR_E_INTERNAL;                                                                            \
    }                                                                                                      \
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

/*
 * Utility macro for allocating memory for user_output_info.
 * Memory is allocated only if not already allocated.
 */
#define DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC(_entry_handle) \
do \
{ \
    if ((_entry_handle).user_output_info == NULL) \
    { \
        DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC_FORCE((_entry_handle)); \
    } \
} while (0)

/*
 * Utility macro for allocating memory for user_output_info.
 * This macro is used only with DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC and DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_COPY.
 * It allocates memory according to the table max_nof_result_fields.
 */
#define DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC_FORCE(_entry_handle) \
do \
{ \
    (_entry_handle).user_output_info = \
        sal_alloc(sizeof(dbal_user_output_info_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE, \
                  (_entry_handle).table->table_name); \
        sal_memset((_entry_handle).user_output_info, 0x0, \
                   sizeof(dbal_user_output_info_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE); \
} while (0)

/*
 * Utility macro for copying user_output_info from existing entry handle.
 * Memory is allocated and copied only if the existing entry handle already has user_output_info allocated.
 */
#define DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_COPY(_src_entry_handle, _dst_entry_handle) \
do \
{ \
    if ((_src_entry_handle).user_output_info != NULL) \
    { \
        if ((_dst_entry_handle).user_output_info == NULL) \
        { \
            DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC_FORCE((_dst_entry_handle)); \
        } \
        sal_memcpy((_dst_entry_handle).user_output_info, (_src_entry_handle).user_output_info, \
                   sizeof(dbal_user_output_info_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE); \
    } \
} while (0)

/*
 * Utility macro for freeing the allocated memory for user_output_info.
 * The memory is freed only if already allocated.
 * It is used when clearing or releasing the entry handle.
 */
#define DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_FREE(_entry_handle) \
do \
{ \
    if ((_entry_handle).user_output_info != NULL) \
    { \
        sal_free((_entry_handle).user_output_info); \
        (_entry_handle).user_output_info = NULL; \
    } \
} while (0)

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
 * Path to the dbal sw tables properties in sw state
 */
#define DBAL_SW_STATE_MDB_PHY_TBL dbal_db.mdb_phy_tables

/**
 * \brief
 * Path to the dbal dynamic tables copy in sw state
 * Tables order: 1)static tables 2)dynamic xml tables (if exists) 3)dynamic tables
 * In case there're no dynamic xml tables, the dynamic tables will be placed right after the static tables */
#define DBAL_DYNAMIC_TBL_IN_SW_STATE dbal_db.dbal_dynamic_tables

/**
 * \brief
 * Path to the dbal dynamic xml tables copy in sw state
 * Tables order: 1)static tables 2)dynamic xml tables (if exists) 3)dynamic tables.
 * In case there're no dynamic xml tables, the dynamic tables will be placed right after the static tables */
#define DBAL_DYNAMIC_XML_TBL_IN_SW_STATE dbal_db.dbal_dynamic_xml_tables

/**
 * \brief
 * Path to the dbal fields properties in sw state */
#define DBAL_SW_STATE_FIELD_PROP dbal_db.dbal_fields

/**
 * \brief
 * Path to the dbal PPMC result type in sw state
 */
#define DBAL_SW_STATE_PPMC_RES_TYPE dbal_db.dbal_ppmc_table_res_types

/**
 * \brief
 * Are dynamic xml tables supported per image
 */
#define DBAL_DYNAMIC_XML_TABLES_ENABLED !dbal_image_name_is_std_1(unit)

/**
 * \brief - Calculate the arr prefix size for the field.
 * arr prefix: value that will be used to complete the value of
 * the field.
 *
 * \param [in] max_field_size - field type size
 * \param [in] field_size_in_table - field size in table
 */
#define DBAL_FIELD_ARR_PREFIX_SIZE_CALCULATE(max_field_size, field_size_in_table) \
   (max_field_size - field_size_in_table)

/**
 * \brief - Calculate the arr prefix value for the field.
 * arr prefix: value that will be used to complete the value of
 * the field.
 *
 * \param [in] arr_prefix_table_entry_width - ARR prefix table
 *      entry width.
 *      Per PP block are defined an ARR prefix table.
 *      The ARR prefix table has a fixed width.
 * \param [in] arr_prefix_table_entry_value - ARR prefix table
 *      entry value
 *      Per PP block are defined an ARR prefix table.
 *      A field is optionally associated with an ARR prefix
 *      table entry.
 * \param [in] arr_prefix_size - arr prefix size for the field
 */
#define DBAL_FIELD_ARR_PREFIX_VALUE_CALCULATE(arr_prefix_table_entry_width, arr_prefix_table_entry_value, arr_prefix_size) \
    (arr_prefix_table_entry_value >> (arr_prefix_table_entry_width - arr_prefix_size))

extern const uint32 zero_buffer_to_compare[DBAL_MAX_SIZE_FOR_GLOBAL_ZERO_BUFFER];

#if defined(ADAPTER_SERVER_MODE)
#define DBAL_CONVERT_HIGH_SKIP_ADAPTER_TO_BASIC_MATURITY_LEVEL() (DBAL_MATURITY_PARTIALLY_FUNCTIONAL)
#define DBAL_CONVERT_HIGH_SKIP_DEVICE_TO_BASIC_MATURITY_LEVEL()  (DBAL_MATURITY_HIGH)
#else
#define DBAL_CONVERT_HIGH_SKIP_ADAPTER_TO_BASIC_MATURITY_LEVEL() (DBAL_MATURITY_HIGH)
#define DBAL_CONVERT_HIGH_SKIP_DEVICE_TO_BASIC_MATURITY_LEVEL()  (DBAL_MATURITY_PARTIALLY_FUNCTIONAL)
#endif

/************** DBAL internal functionality {APIs from: dbal_internal.c} **************/
/**
 * \brief returns 1 if dbal initiated, otherwise returns 0.
 */
uint8 dbal_is_initiated(
    int unit);
/**
 * \brief internal DBAL init
 */
shr_error_e dbal_internal_init(
    int unit);

/**
 * \brief internal DBAL init
 */
shr_error_e dbal_internal_deinit(
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

uint8 dbal_image_name_is_std_1(
    int unit);

/**
 * \brief sets pemla status
 */
void dbal_pemla_status_set(
    int unit,
    dbal_status_e status);

/**
 * \brief gets pemla status
 */
shr_error_e dbal_pemla_status_get(
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
 * \brief set the status of the handle to DBAL_HANDLE_STATUS_ACTION_PREFORMED
 */
void dbal_entry_handle_status_performed_set(
    dbal_entry_handle_t * entry_handle);

/**
 *\brief allocate SW entry handle from pool. this handle  is used to preform table actions
 */
shr_error_e dbal_entry_handle_take_internal(
    int unit,
    dbal_tables_e table_id,
    uint32 *entry_handle_id,
    uint8 operation);

/**
*\brief copy SW entry handle from pool. this handle  is used to preform table actions
*/
shr_error_e dbal_entry_handle_copy_internal(
    int unit,
    uint32 src_handle_id,
    uint32 *dst_handle_id);

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
 *\brief unset a value field in the handle
 */
void dbal_entry_value_field_unset_field(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id);

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
 *\brief get the enum hw value if the field is an enum.
 *       Otherwise, return dbal_entry_key_field_get
 */
shr_error_e dbal_entry_enum_hw_value_or_key_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type);

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
 *\brief
 * API that is issued to request attribute, set a pointer to a value that will be updated after entry_get
 */
shr_error_e dbal_entry_attribute_request_internal(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 attr_type,
    uint32 *attr_val);
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
 *       return action_skip = 1;
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
 *\brief used to align the phy_entry payload. when the table has multiple result, it happens that entry get align the
 *       value to the max entry payload. after the result type was resolved, align the payload to the actual payload
 *       size.
 */
shr_error_e dbal_entry_handle_phy_entry_align(
    int unit,
    uint32 payload_offset,
    uint8 shift_left,
    dbal_entry_handle_t * entry_handle);

/** see .c file */
shr_error_e dbal_entry_handle_is_field_set(
    int unit,
    int entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id);

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
void dbal_entry_handle_attribute_parse(
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
 *   \param [in] inst_idx       - Instance field index
 *   \param [in] table_id       - desired table
 *   \param [in] field_val      - Value of the field (input buffer)
 *   \param [in] field_mask     -
 *       A mask for the field_val. To ignore, may be set to NULL. Maximal size
 *       of this input, if not NULL, is DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS
 *   \param [in] result_type_idx- In case that the field is a
 *          result type, the result index is needed
 *   \param [in] is_key         - Is the field a key
 *   \param [in] is_full_buffer -
 *       1. If the field value is passed as full dbal handle
 *       buffer or just a single field that already was read from
 *       buffer
 *       2. If it is full buffer, Enum values are HW values, in
 *       case single field value, enum already parsed to SW
 *       values
 *   \param [in] buffer_to_print- A buffer to contain the output
 */
shr_error_e dbal_field_in_table_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    uint8 inst_idx,
    dbal_tables_e table_id,
    uint32 *field_val,
    uint32 *field_mask,
    int result_type_idx,
    uint8 is_key,
    uint8 is_full_buffer,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);

shr_error_e dbal_field_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    multi_res_info_t * result_info,
    uint8 is_full_buffer,
    dbal_fields_e parent_field,
    int size,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);

shr_error_e dbal_lpm_prefix_length_to_mask(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_lpm_mask_to_prefix_length(
    int unit,
    dbal_entry_handle_t * entry_handle);

/************** Field internal APIs (from: dbal_fields.c) **************/

shr_error_e dbal_field_types_init_illegal_value_cb_assign(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_field_types_basic_info_t * field_types_info);

shr_error_e dbal_fields_nof_occupied_dynamic_fields_get(
    int unit,
    int *nof_occupied_fields);

shr_error_e dbal_field_types_enum_name_to_hw_value_get(
    int unit,
    dbal_field_types_defs_e field_type,
    char enum_value_name[DBAL_MAX_STRING_LENGTH],
    uint32 *hw_value);

/**
* \brief
*   Returns total capacity of the table, according to physical tables
*   If table is SBC, the returned value is the minimum capacity between
*     all the physical tables
*   If table is DPC, the returned value is the summed capacity of all the
*     physical tables
*   \param [in] unit - Relevant unit
*   \param [in] table - Relevant dbal field type.
*   \param [out] total_capacity - returned total table capacity
*  \return
*    \retval Zero if no error occurred
*****************************************************/
shr_error_e dbal_mdb_table_total_capacity_get(
    int unit,
    dbal_logical_table_t * table,
    uint32 *total_capacity);

/**
* \brief
*   Retuns the field type info
*   \param [in] unit - Relevant unit.
*   \param [in] field_type - Relevant dbal field type.
*   \param [out] field_type_info - field type info
*
*  \return
*    \retval Zero if no error occured
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
*****************************************************/
shr_error_e dbal_field_types_info_get(
    int unit,
    dbal_field_types_defs_e field_type,
    CONST dbal_field_types_basic_info_t ** field_type_info);

shr_error_e dbal_field_arr_prefix_decode(
    int unit,
    dbal_table_field_info_t * table_field,
    uint32 field_val,
    uint32 *ret_field_val,
    uint8 verbose);

shr_error_e dbal_fields_encode_decode_bitwise_not(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out);

shr_error_e dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info);

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

/** for TCAM tables we need to mask the encoding bits in the parent field
  * the function adds the encoding bits to the child field given mask value
  * and set the mask for the additional encoding bits.*/
shr_error_e dbal_fields_field_parent_encode_custom_val(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    uint32 encode_mask_val,
    uint32 *field_mask_val,
    uint32 *parent_field_full_mask);

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

/** returns if the field has child fields */
int dbal_fields_is_parent_field(
    int unit,
    dbal_fields_e field_id);

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

shr_error_e dbal_field_string_update(
    int unit,
    dbal_fields_e field_id,
    char field_name[DBAL_MAX_STRING_LENGTH]);

shr_error_e dbal_field_string_to_indexes_init(
    int unit);

/************** Logical table APIs  (from: dbal_tables.c) **************/
shr_error_e dbal_tables_table_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t ** table);

/**
 * \brief Function return the field index in table (the fields array) according to input parameters.
 * If not found, it will return _SHR_E_NOT_FOUND without printing any error massage.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested field is key or value field\n
 *   \param [in] result_type_idx -  valid in cases of value fields are requested. indicate the result type index\n
 *   \param [in] inst_idx -  valid in cases of value fields are
 *          requested. indicate the instance index\n
 *   \param [out] field_index_in_table -  index in the table\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e dbal_tables_field_index_in_table_get_no_err(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_index_in_table);
/**
 * \brief
 * Dynamically load tables from xmls, currently supports only MDB tables.
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] parent_dir
 *   \param [in] tables_info
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_table_create_dynamic(
    int unit,
    char *parent_dir,
    dbal_logical_table_t * tables_info);

shr_error_e dbal_fields_nof_occupied_dynamic_tables_get(
    int unit,
    int *nof_occupied_tables);

/**
 * \brief return the logical table info pointer, error if table not found.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] table -  a pointer to the requested table\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       return a pointer to the table\n
 */
shr_error_e dbal_tables_table_get(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t ** table);

/**
 * \brief - update the entry counter of a table. In case of MDB
 *        in direct table, update the counter of the physical
 *        table. Do nothing if table is not counter supported
 *        In case core_id is ALL, logical counter is updated as
 *        number of new entries x number of cores.
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

shr_error_e dbal_tables_groups_info_get(
    int unit,
    hl_groups_info_t ** hl_groups_info);

shr_error_e dbal_tables_group_info_get(
    int unit,
    dbal_hw_entity_group_e hw_entity_group_id,
    hl_group_info_t ** group_info);

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

/**
 *\brief returns TRUE if the table is a tcam table.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id
 * \par DIRECT OUTPUT:
 *   Boolean(uint8)
 * \par INDIRECT OUTPUT
 *       None
 */ uint8
    dbal_tables_is_tcam(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief API returns the key size and the max payload size (in bits) used in the table. valus are HW values so if table
 *  has result type that mapped to SW or core_id field for DPC tables those bits are not included in the size   */
shr_error_e dbal_tables_sizes_get(
    int unit,
    dbal_tables_e table_id,
    int *key_size,
    int *max_pld_size);

/**
 * \brief dbal commit mode for non direct tables affect the performance of the entry commit.
 *
 *  for non-direct tables when adding new entries there is a different operation for adding new entry and updating
 *  existing entry.
 *
 *  In standard mode, DBAL performs get prior to the commit, and validate that the entry existence is appropriate.
 *  In optimized mode, DBAL skip the get operations, and trust the user that he called to the correct action
 *  (commit/update), in-case that user didn't call the correct action an unexpected error can be returned.
 *
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] mode - 0 disable, 1 enable
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_optimize_commit_mode_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);

/**
 * \brief dbal dirty mode indicate if the table was touched during activity.
 * touch means entry commit or entry clear. (table clear is not part of it).
 *
 *  this API indication is a debug tool for users to see for example if during test case what was the table that was
 *  touched.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] mode - 0 clean, 1 dirty
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_dirty_mode_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);

/**
 * \brief dbal table protection mode indicate if the table is thread safe, once enabled a mutex is allocated for the
 * table. the mutex will be used once user perform handle alloc and released once user release the handle.
 *
 * more info can be found in DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] mode - 0 clean, 1 dirty
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_protection_enable_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);

/**
 * \brief
 * Allocates memory for field types info
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_field_types_alloc(
    int unit);

/**
 * \brief
 * Deallocates memory for field types info
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_field_types_dealloc(
    int unit);

/** reserve a table mutex see: DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED */
shr_error_e dbal_per_table_mutex_reserve(
    int unit,
    dbal_tables_e table_id,
    uint8 *mutex_id);

/** release a table mutex see: DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED */
shr_error_e dbal_per_table_mutex_release(
    int unit,
    dbal_tables_e table_id,
    uint8 mutex_id);

shr_error_e dbal_per_table_mutex_info_get(
    int unit,
    uint8 mutex_id,
    CONST dbal_specific_table_protection_info_t ** protect_info);
/**
 * \brief - sets learning functionality to a logical table.
 * Tables that have this indication set cannot use the sw shadow and must access the hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] hitbit - the hitbit value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_learning_enable(
    int unit,
    dbal_tables_e table_id,
    uint32 hitbit);

/**
 * \brief - sets the hitbit to a logical table.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] hitbit - the hitbit value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_hitbit_enable(
    int unit,
    dbal_tables_e table_id,
    uint32 hitbit);

/**
 * \brief - Enables/Disables Table validation to improve performance
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] mode - 0 no validation, 1 enable validation
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_validation_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);

/**
 * \brief
 * read dynamic result types form sw state, only applicable for PPMC table
 *   \param [in] unit
 *   \param [in] table_id - the table to restore
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_result_type_restore(
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
shr_error_e dbal_mdb_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 resolve_from_sw);
uint8 dbal_table_is_in_lif(
    CONST dbal_logical_table_t * table);
uint8 dbal_table_is_out_lif(
    CONST dbal_logical_table_t * table);
uint8 dbal_table_is_esem(
    CONST dbal_logical_table_t * table);
uint8 dbal_logical_table_is_mact(
    int unit,
    dbal_tables_e table_id);
shr_error_e dbal_physical_table_app_id_is_mact(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    uint32 *is_mact);
uint8 dbal_logical_table_is_out_rif_allocator(
    CONST dbal_logical_table_t * table);
uint8 dbal_logical_table_is_fec(
    int unit,
    CONST dbal_logical_table_t * table);

/*
 * \brief
 * Check physical table id is valid
 */
shr_error_e dbal_mdb_phy_table_is_valid(
    int unit,
    dbal_physical_tables_e physical_table_id);

/*
 * \brief
 * Returns core mode for given physical table id
 */
dbal_core_mode_e dbal_mdb_phy_table_core_mode_get(
    int unit,
    dbal_physical_tables_e physical_table_id);

/*
 * \brief
 * Returns a struct containing set of functions
 * for physical table operations
 */
shr_error_e dbal_mdb_phy_table_actions_get(
    int unit,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_table_actions_t ** physical_table_actions);
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

int dbal_hl_tcam_is_table_supports_half_entry(
    int unit,
    dbal_tables_e table_id);
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
    dbal_hl_l2p_field_info_t * curr_l2p_info,
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

shr_error_e dbal_sw_table_nof_entries_calculate(
    int unit,
    dbal_logical_table_t * table,
    uint32 *num_of_entries);

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
shr_error_e dbal_kbp_fwd_caching_enabled_get(
    int unit,
    int *enabled);
shr_error_e dbal_kbp_fwd_caching_enabled_set(
    int unit,
    int enabled);
shr_error_e dbal_kbp_fwd_cache_commit(
    int unit);
shr_error_e dbal_kbp_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 payload_size);


/**
 *\brief lock the logger on a specific table according to user decision (will execute only prints that relevant to this table).
 */
shr_error_e dbal_logger_table_user_lock(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_logger_table_mode_set_internal(
    int unit,
    dbal_logger_mode_e mode);

/**
 * \brief overrides logger print type to one of the options in DBAL_LOGGER_PRINT_TYPE_XX
 */
shr_error_e dbal_logger_print_type_set_internal(
    int unit,
    dbal_logger_print_type_e print_type);

shr_error_e dbal_logger_print_type_get_internal(
    int unit,
    dbal_logger_print_type_e * print_type);

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
 * This function is called after any HW access. The action is called after the DBAL dispatch the action to the access
 * layers and just after the exit label (this is the reason that it has no return value). Those API should be used to
 * perform actions that are not part of the main flow of the DBAL and can be enabled/disabled without affecting the main
 * flow. Each process should create a separate function to handle the flow.
 * \param [in] unit - Device Id
 * \param [in] entry_handle - Pointer to entry handle structure
 * \param [in] get_entry_handle - Pointer to get entry handle structure
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
shr_error_e dbal_action_access_post_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
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
 * working in two modes, consecutive/non-consecutive. In consecutive mode passes to the access layer the first key and
 * the number of entries the access layer can use best way to update full range of entries (DMA). in the non-consecutive
 * mode (in case the range is not complete 0-4,66-201), update each entry. PERFORMANCE_IMPROVMENT: to find sub ranges
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
 * The function validates the table interface, The interface vaidation is mutual to all tables types. In addition to
 * validation, it computes the offset of fields in the entry buffer and some other general properties of the table and
 * updates the superset result type.
 */
shr_error_e dbal_db_init_table_interface_post_process(
    int unit,
    dbal_tables_e table_id,
    uint32 result_lsb_padding,
    dbal_logical_table_t * table);

/**
 * \brief
 * The function add some validations and general properties
 * calculations for MDB tables
 */
shr_error_e dbal_db_init_table_mdb_post_process(
    int unit,
    dbal_tables_e table_id,
    uint32 result_lsb_padding,
    dbal_logical_table_t * table);

/**
 * \brief
 * The function add some validations and general properties
 * calculations for KBP tables
 */
shr_error_e dbal_db_init_table_kbp_post_process(
    int unit,
    dbal_tables_e table_id,
    uint32 result_lsb_padding,
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

/**Function pointer - used as callback to dbal_dynamic_updates_dispatcher()*/
typedef shr_error_e(
    *dbal_dynamic_xml_add_f) (
    int unit,
    xml_node top_node);

/**
 * \brief
 * Add new table dynamically (at load time)
 */
shr_error_e dnx_dynamic_xml_tables_update(
    int unit,
    xml_node top_node);

/**
 * \brief
 * Iterate a directory, search for relevant (i.e - dynamically loaded) xmls, first parse and dispatch to relevant cb
 */
shr_error_e dbal_dynamic_updates_dispatcher(
    int unit,
    char *parent_dir,
    dbal_dynamic_xml_add_f dbal_dynamic_xml_add,
    char *top_node_name);

/** API per access method. */

shr_error_e dbal_pemla_init(
    int unit);

shr_error_e dbal_pemla_deinit(
    int unit);

shr_error_e dbal_sw_access_init(
    int unit);

shr_error_e dbal_sw_access_deinit(
    int unit);

shr_error_e dbal_mdb_init(
    int unit);

shr_error_e dbal_mdb_deinit(
    int unit);

/**
 * \brief - Evaluate a single field and value for acceptance in
 * a specific result type. The function is called by the LIF
 * Table manager for every user supplied field that is on the
 * DBAL handle prior to calling the LIF table manager API to
 * decide the appropriate result type. The function is called
 * for each result type that is considered by the decision
 * mechanism. Only fields up to 32 bits are supported.
 *
 * \param [in] unit - unit id
 * \param [in] field_val - pointer to a field value
 * \param [in] table_field_info - A pointer to the static field
 *        information in the context of the specific result
 *        type.
 * \param [out] is_valid_value - The evaluation result - The
 *        field is either valid for the result type or not.
 * \return DIRECT OUTPUT
 *   shr_error_e
 */
shr_error_e dbal_value_field32_validate(
    int unit,
    uint32 *field_val,
    dbal_table_field_info_t * table_field_info,
    uint8 *is_valid_value);

/**
 * \brief
 * function doesn't access to HW
 * only supported for direct tables that DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED.
 * if table is not supported in this API returns _SHR_E_CONFIG.
 *
 * return is_entry_exists true if entry was set in the HW.
 */
shr_error_e dbal_actions_optimized_is_exists(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_entry_exists);

/** \brief
 * there are 3 basic maturity levels: LOW/PARTIALLY_FUNCTIONAL/HIGH.
 * we've added additional maturity levels which are those 3 basics maturity levels with conditions.
 * Ex: DBAL_MATURITY_HIGH_SKIP_ADAPTER, is HIGH only when running on adapter. Otherwise, it's partially_functional.
 * So we'll convert those maturity levels to basic levels
 * */
dbal_maturity_level_e dbal_convert_to_basic_maturity_level(
    dbal_maturity_level_e maturity_level);

/** \brief
 * return true if the maturity level is HIGH
 * or can be is a maturity level which is not a basic maturity level
 */
uint8 dbal_maturity_level_is_high(
    dbal_maturity_level_e maturity_level);

#endif/*_DBAL_INCLUDED__*/
