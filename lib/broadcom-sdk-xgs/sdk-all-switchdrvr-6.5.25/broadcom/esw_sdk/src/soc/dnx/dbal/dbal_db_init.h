/**
 * \file dbal_db_init.h
 * $Id$
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Main functions for init the dbal fields and logical tables DB
 *
 */

#ifndef DBAL_DB_INIT_H_INCLUDED
#define DBAL_DB_INIT_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <soc/dnx/pp_stage.h>

/*************
 *  DEFINES  *
 *************/

/** \brief The nof access elements that can be set in HL access */
#define DBAL_TABLE_MAX_NUM_OF_ACCESS_FIELDS                       400

/** defines that indicates that the field in the formula (position and type) is
 *  unknown */
#define DBAL_TABLE_FIELD_POSITION_UNKNOWN                         -1
#define DBAL_TABLE_FIELD_KEY_OR_RESULT                            -1

/**
 * \brief
 * identifier for dnx data reference in the old format
 * Example: Size="DNX_DATA.mdb_app_db.fields.numeric_mdb_field___1"
 * This is kept for backward compatibility but won't be supported anymore at some point
 */
#define DB_INIT_DNX_DATA_INDICATION                       "DNX_DATA"

/**
 * \brief
 * prefix  for dnx data reference, in the new format
 * (as used in SDK)
 * Example: Size="dnx_data_mdb_app_db.fields.numeric_mdb_field___1_get(unit)"
 */
#define DB_INIT_DNX_DATA_SDK_MODULE_PREFIX                "dnx_data_"

/**
 * \brief
 * suffix for SDK dnx data get function
 */
#define DB_INIT_DNX_DATA_GET_SUFFIX                       "_get"

/**
 * \brief
 * feature
 */
#define DB_INIT_DNX_DATA_FEATURE                       "feature"

/**
 * \brief
 * identifier for Physical Table reference
 * for SW access table size
 */
#define DB_INIT_PHYDB_INDICATION                          "PHYDB"

/**
 * \brief log2 function
 * Used for nof bits
 */
#define DBAL_LOG2_FUNCTION                                  "#log2_round_up"

#define DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(_table_entry, _num_of_results_types)                                    \
    (_table_entry->nof_result_types) = (_num_of_results_types);                                                         \
    if (_table_entry->nof_result_types > DBAL_MAX_NUMBER_OF_RESULT_TYPES - 1)                                           \
    {                                                                                                                   \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more result types than max allowed.\n", _table_entry->table_name);  \
    }

/*
 * precondition: _multi_res_info.is_disabled is initialized
 */
#define DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(_table_entry, _multi_res_info, _num_of_results_fields)         \
    (_multi_res_info.nof_result_fields) = (_num_of_results_fields);                                             \
    if (_multi_res_info.nof_result_fields > DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE)                   \
    {                                                                                                           \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, result type %s has more result fields than max allowed.\n",    \
                     _table_entry->table_name, _multi_res_info.result_type_name);                               \
    }                                                                                                           \
    if (!_multi_res_info.is_disabled && _multi_res_info.nof_result_fields < 1)                                  \
    {                                                                                                           \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, result type %s has no result fields \n",                       \
                     _table_entry->table_name, _multi_res_info.result_type_name);                               \
    }

/** precondition: table name has been set */
#define DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(_table_entry, _nof_key_fields) \
        SHR_ALLOC_SET_ZERO(_table_entry->keys_info, _nof_key_fields * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", _table_entry->table_name, EMPTY, EMPTY);

#define DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(_table_entry, _multi_res_info)                         \
            SHR_ALLOC_SET_ZERO(_multi_res_info.results_info,                                                    \
                           _multi_res_info.nof_result_fields * sizeof(dbal_table_field_info_t),                 \
                           "results fields info allocation",                                                    \
                           "%s%s%s\r\n", _table_entry->table_name, EMPTY, EMPTY)

#define DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(_multi_res_info, _hw_value) \
            _multi_res_info.result_type_hw_value[_multi_res_info.result_type_nof_hw_values++] = _hw_value;

#define DBAL_DB_INIT_RESULT_TYPE_LART_INDEX_SET(_multi_res_info, _lart_index) \
        _multi_res_info.lart_index = _lart_index;

#define DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(_table_entry, _is_standard_1_table, _is_compatible_with_all_images)  \
    if (_is_standard_1_table || !_is_compatible_with_all_images)                                                                    \
    {                                                                                                                               \
        /** indicate that the table is per image */                                                                                 \
        SHR_BITSET(_table_entry->indications_bm, DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE);                                           \
    }

/** precondition: table name and nof_labels has been set, */
#define DBAL_DB_INIT_TABLE_LABELS_ALLOC(_table_entry)   \
 SHR_ALLOC_SET_ZERO(_table_entry->table_labels, _table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", _table_entry->table_name, EMPTY, EMPTY);

#define DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(_access_param, _alias) \
 _access_param->alias_reg_mem_hw_entity_id = _alias;

#define DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(_access_param, _hw_entity_group_id) \
 _access_param->hw_entity_group_id = _hw_entity_group_id;

/** precondition: nof_enum_vals has been set */
#define DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(_cur_field_type_param)  \
    sal_memset(_cur_field_type_param->enums, 0x0, _cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));

#define DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(_cur_field_type_param, _enum_index, _name) \
    sal_strncpy(_cur_field_type_param->enums[_enum_index].name_from_interface, \
                   _name, \
                   sizeof(_cur_field_type_param->enums[_enum_index].name_from_interface));

#define DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(_cur_field_type_param, _enum_index, _invalid_value) \
    _cur_field_type_param->enums[_enum_index].is_invalid_value_from_mapping = _invalid_value;

/** precondition: nof_childs has been set */
#define DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(_cur_field_type_param)  \
    sal_memset(_cur_field_type_param->childs, 0x0, cur_field_type_param->nof_childs * sizeof(dbal_db_child_field_info_struct_t));

#define DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(_cur_field_type_param, _child_index, _name)  \
    sal_strncpy(_cur_field_type_param->childs[_child_index].name, _name, sizeof(_cur_field_type_param->childs[_child_index].name));

/*************
 * TYPEDEFS  *
 *************/
/**
 * \brief
 * dnx_data parameters structure
 * Used for dnx data reference from dbal XML files
 */
typedef struct
{
    /** In not valid DNX_DATA reference, value is read as integer */
    int int_val;

    /** Formula as string */
    char formula_as_string[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    /** Formula structure */
    dbal_offset_formula_t *formula;

        /** in case 1 it means that the value is not valid (can be because the value as no mapping)  */
    int is_invalid;

} dbal_db_int_or_dnx_data_info_struct_t;

/**
 * \brief
 * parsed dnx data struct.
 * Result of the dnx_data string parsing
 */
typedef struct
{
    /** dnx data module  */
    char module[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    /** dnx data submodule  */
    char submodule[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    /** dnx data data  */
    char data[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    /** dnx data member */
    char member[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    /** dnx data key 1 */
    int key1;

    /** dnx data key 2 */
    int key2;
} dbal_parsed_db_dnx_data_struct_t;

/**
 * \brief
 * child field structure
 */
typedef struct
{
    /** child field name */
    char name[DBAL_MAX_STRING_LENGTH];

    /**  encode type  */
    dbal_value_field_encode_types_e encode_type;

    /** encoding input param as integer */
    int encode_param1;

    /**  reference field value  */
    char reference_field_value[DBAL_MAX_STRING_LENGTH];

} dbal_db_child_field_info_struct_t;

/**
 * \brief
 * sub struct field structure
 */
typedef struct
{
    /** struct field name */
    char name[DBAL_MAX_STRING_LENGTH];

    /**  start offset  */
    int start_offest;

    /** length */
    int length;

    /** arr prefix table entry value   */
    int arr_prefix_table_entry_value;

    /** arr prefix table entry width   */
    int arr_prefix_table_entry_width;

} dbal_db_struct_field_info_struct_t;

/**
 * \brief
 * enum structure - read from XML
 */
typedef struct
{
    /** enum name */
    char name_from_interface[2 * DBAL_MAX_STRING_LENGTH];

    /** enum value as integer */
    int value_from_mapping;
    int is_invalid_value_from_mapping;

} dbal_db_enum_info_struct_t;

/**
 * \brief
 * enum structure - read from XML
 */
typedef struct
{
    /** define name */
    char define_name[DBAL_MAX_STRING_LENGTH];

    /** define value as int or dnx_data */
    int define_value;

} dbal_db_defines_info_struct_t;

/**
 * \brief
 * field DB - read from XML
 */
typedef struct
{

    /** num of child fields */
    int nof_childs;

    /** child fields list */
    dbal_db_child_field_info_struct_t *childs;

    /** num of enum values */
    int nof_enum_vals;

    /** field enum values */
    dbal_db_enum_info_struct_t *enums;

    /** num of defines values */
    int nof_defined_vals;

    /** field defines values */
    dbal_db_defines_info_struct_t *define;

    /** field max value as integer */
    int max_value;

    /** field min value */
    int min_value;

    /** field const value*/
    int const_value;

    /** Number of valid illegal values */
    int nof_illegal_value;

    /** Illegal values */
    int *illegal_values;

    /** field default value as integer */
    int default_val;

    /** default value indication */
    uint8 default_val_valid;

} field_type_db_struct_t;

/**
 * \brief
 * field in table DB - read from XML
 */
typedef struct
{
    /** field validity as integer or DNX_DATA reference */
    int valid;

    /** size as integer or DNX_DATA reference */
    int size;

    /** is this field support multiple instances */
    int nof_instances;

    /** max nof instances */
    int max_nof_instances;

    /** validate nof_instance against max_nof_isntance.
     *  Should be set to true when DNX_DATA */
    uint8 is_validate_nof_instances;

    /** max value of field in table */
    int max_value;

    /** min value of field in table */
    int min_value;

    /** field const value as integer or DNX_DATA reference */
    int const_value;

    /** permission */
    dbal_field_permission_e permission;

    /** field offset in table [bits] */
    int offset;

    /** arr prefix value */
    int arr_prefix_value;

    /** arr prefix width */
    int arr_prefix_width;

} table_db_field_params_struct_t;

/**
 * \brief
 * Offset in HW structure - read from XML
 */
typedef struct
{
    /** parsed formula  by autocoder (when simple int) */
    int formula_int;
    /** formula is a auto generated callback, created by autocoder */
    dnx_dbal_formula_offset_cb formula_cb;
} table_db_offset_in_hw_struct_t;

/**
 * \brief
 * Condition structure - read from XML
 */
typedef struct
{
    dbal_condition_types_e condition_type;

    dbal_fields_e field_id;

    /** value - for condition calculation */
    int value;

    /** use enum value of the field for value indication */
    /*
     * char enum_val[DBAL_MAX_STRING_LENGTH];
     */
    uint32 enum_val;

} table_db_access_condition_struct_t;

/**
 * \brief
 * logical table access DB - read from XML
 * MDB tables only
 */
typedef struct
{
    /** table app id - MDB tables only */
    int app_db_id;

    /** table app id size in bits - MDB tables only */
    int app_db_id_size;

    /** table physical db - MDB tables only */
    char phy_db[DBAL_MAX_SHORT_STRING_LENGTH];

} table_db_mdb_access_struct_t;

/**
 * \brief
 * logical table access DB - read from XML
 * pemla tables only
 */
typedef struct
{
    /** pemla key fields ID mapping */
    uint32 pemla_key_mapping[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];

    /** pemla result fields ID mapping */
    uint32 pemla_result_mapping[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
} table_db_pemla_access_struct_t;

/** *
 *
 * \brief
 * logical table access DB - read from XML
 * hard logic tables only
 */
typedef struct
{
    /** hard logic access type */
    dbal_hard_logic_access_types_e access_type;

    /** field id for access mapping */
    dbal_fields_e access_field_id;

    /** field instance index */
    uint8 inst_idx;

    /** access type for tcam key/mask/result */
    dbal_hl_tcam_access_type_e tcam_access_type;

    /** group id. Group a set of register or memories  */
    dbal_hw_entity_group_e hw_entity_group_id;

    /** indicate how many hw entities (reg/mem) in groups there is
     *  If 0, then it means, that a hw entity was indicated
     *  instead. See hw_entity_id.
     *  Max value: DBAL_MAX_NUMBER_OF_HW_ELEMENTS */

    /** hw entity (reg/mem)  */
    int reg_mem_hw_entity_id;

    /** size in access [bits] */
    int access_size;

    /** offset in access [bits] */
    int access_offset;

    /** mapping encoding */
    /** encode type */
    dbal_value_field_encode_types_e encode_type;

    /** encoding input param as integer or DNX_DATA reference */
    int encode_param1;

    int nof_conditions;

    /** mapping condition */
    table_db_access_condition_struct_t access_condition[DBAL_FIELD_MAX_NUM_OF_CONDITIONS];

    /** array offset (array index in access) */
    table_db_offset_in_hw_struct_t array_offset;

    /** entry offset */
    table_db_offset_in_hw_struct_t entry_offset;

    /** block offset */
    table_db_offset_in_hw_struct_t block_index;

    /** data offset */
    table_db_offset_in_hw_struct_t data_offset;

    /** group offset */
    table_db_offset_in_hw_struct_t group_offset;

    /** data offset in aliased memory   */
    table_db_offset_in_hw_struct_t alias_data_offset;

    /** hw field name in access */
    int hw_field;

    /** alias hw entity, memory or register */
    int alias_reg_mem_hw_entity_id;

} table_db_access_params_struct_t;

/**
 * Indicate that access has field-packed tag. Meaning that not
 * all the mapping has been defined.
 * Part of the mapping is implicit.
 *
 */
#define DBAL_INIT_HARD_LOGIC_ACCESS_STRUCT_PACKED_FIELDS  (0x1)

/**
 * \brief
 * logical table access DB - read from XML CS tables only
 */
typedef struct
{
    /** hw entity (reg/mem)  */
    int reg_mem_hw_entity_id;

} table_db_cs_access_struct_t;

/**
 * \brief
 * logical table access DB - read from XML
 * hard logic tables only
 */
typedef struct
{
    /** number of access mapping*/
    int nof_access;

    table_db_access_params_struct_t access[DBAL_TABLE_MAX_NUM_OF_ACCESS_FIELDS];

    /** result type logical value (name) */
    char mapping_result_name[DBAL_MAX_STRING_LENGTH];

} table_db_hl_access_struct_t;

/**
 * \brief
 * logical table full DB, read from XML
 */
typedef struct
{
    /** TCAM pipe stage - MDB TCAM and TCAM CS only */
    dbal_stage_e pipe_stage;

    /** access DB */
    /**
     * If the hook is active the access to the table is done by
     * dedicated functions. Hooks are generally used to fix HW
     * issues or support specific HW behaviors
     */
    int is_hook_active;

    /** access DB - MDB tables only */
    table_db_mdb_access_struct_t *mdb_access;

    /** access DB - hard logic tables only
     *  Size of the array: DBAL_MAX_NUMBER_OF_RESULT_TYPES */
    table_db_hl_access_struct_t *hl_access;

    /** TCAM CS DB mapping */
    table_db_cs_access_struct_t *cs_access;

    /** access DB - pemla tables only */
    table_db_pemla_access_struct_t *pemla_access;
} table_db_struct_t;

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief
 * Initializes a dbal_logical_table_t structure.
 * Note that this function does not deallocate memory pointed to by fields within the structure.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_info - \n
 *      dbal_logical_table_t structure
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_logical_table_t_init(
    int unit,
    dbal_logical_table_t * table_info);

/**
 * \brief
 * Initializes a dbal_logical_table_t structure for dynamic table in this case the pointers are not set to NULL.
 * Note that this function does not deallocate memory pointed to by fields within the structure.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_info - \n
 *      dbal_logical_table_t structure
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_logical_dynamic_table_t_init(
    int unit,
    dbal_logical_table_t * table_info);

/**
 * \brief
 * The function initializes dbal tables component with default values
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_info - \n
 *      dbal tables info DB
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_table_alloc(
    int unit,
    dbal_logical_table_t * table_info);

/**
 * \brief
 * The function initializes dbal dynamic tables component with default values
 * it preallocate all needed memory to accommodate the key and result fields according to the size defined in DNX data.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_dynamic_dbal_tables_sw_state(
    int unit);

/**
 * \brief
 * The function initializes dbal dynamic tables component with default values
 * it preallocate all needed memory to accommodate the key and result fields according to the size defined in DNX data.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_dynamic_dbal_tables_memory(
    int unit);

/**
 * \brief
 * The function read the MDB tables XML file(s) and fill
 * the tables DB accordingly
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_info - \n
 *       dbal tables info DB
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_logical_tables(
    int unit,
    dbal_logical_table_t * table_info);

/**
 * \brief
 * The function run some logical validation on the tables DB
 * In addition it fills the DB with paraneters that not directly
 * read from XML
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] tables_info - \n
 *      dbal tables info DB
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_tables_post_process(
    int unit,
    dbal_logical_table_t * tables_info);

/**
 * clear section of the table_db_struct_t which is common between all types of tables.
 */
void dbal_init_table_db_struct_clear(
    table_db_struct_t * cur_table_param);

/**
 * Set the general information to the table db struct
 */
shr_error_e dbal_db_init_general_info_set(
    int unit,
    dbal_logical_table_t * table_entry,
    dbal_tables_e table_id,
    int is_valid,
    char *table_name,
    dbal_access_method_e access_method);

/**
 * \brief
 * clear access params (sub access from hl_access)
 */
void dbal_db_init_table_db_struct_hl_access_params_clear(
    table_db_access_params_struct_t * access);

/**
 * \brief
 * clear specific access section of the table_db_struct_t
 */
void dbal_db_init_table_db_struct_access_clear(
    table_db_struct_t * cur_table_param,
    dbal_logical_table_t * table_entry);

/**
 *  init field params.
 *  Field params is used to gather field properties.
 *  Field params helps to set dbal field
 */
void dbal_db_init_table_field_params_init(
    table_db_field_params_struct_t * field_db);

/**
 *  set mandatory field values to field
 */
shr_error_e dbal_db_init_table_field_set_mandatory_values(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_defs_e field_type,
    dbal_table_field_info_t * field_info);

/**
 * \brief
 * This function set an interface field using optional values.
 * Optional values are gather in field_read_data.
 */
shr_error_e dbal_db_init_table_field_set_optional_or_default_values(
    int unit,
    dbal_logical_table_t * table_entry,
    dbal_table_field_info_t * field_info,
    table_db_field_params_struct_t * field_read_data,
    uint8 is_key_fields);

/**
 * Set a arr prefix for db field to the table db struct Used for
 * keys, result, result mapping
 */
void dbal_db_init_table_set_field_arr_prefix(
    int unit,
    table_db_field_params_struct_t * field_db,
    uint32 arr_prefix_value,
    uint32 arr_prefix_width);

/**
 * set register/memory access to table db struct
 */
shr_error_e dbal_db_init_table_set_access_register_memory(
    int unit,
    table_db_access_params_struct_t * access_params,
    int reg_mem_hw_entity_id,
    int hw_field);

/**
 * set a access condition to table db struct
 */
shr_error_e dbal_db_init_table_set_access_condition(
    int unit,
    table_db_access_condition_struct_t * access_condition,
    dbal_condition_types_e type,
    dbal_fields_e field,
    int value,
    uint32 enum_value);

/**
 * hard logic sw state access
 */
void dbal_db_init_sw_state_set_indirect_table_size(
    int unit,
    dbal_logical_table_t * table_entry,
    dnx_dbal_formula_offset_cb indirect_table_size_cb,
    int indirect_table_size_int);

/**
 * No explicit mapping defined in the app to phy db mapping
 * set pemla access mapping by index
 */
void dbal_db_init_table_set_pemla_mapping_by_index(
    table_db_struct_t * table_param,
    dbal_logical_table_t * table_entry);

shr_error_e dbal_db_init_table_tcam_cs_mapping_add(
    int unit,
    table_db_struct_t * cur_table_param,
    soc_mem_t mem);

/**
 * set access mdb indo
 */
shr_error_e dbal_db_init_table_set_mdb_info(
    int unit,
    table_db_struct_t * cur_table_param,
    char *phy_db,
    int app_db_id_size,
    int app_db_id,
    dbal_stage_e stage);

/**
 * \brief
 * The function adds a logical table read from XML to the dbal
 * tables DB.
 * Main functionality is translating the string from XML to
 * corresponding values
 */
shr_error_e dbal_db_init_table_add(
    int unit,
    table_db_struct_t * table_params,
    dbal_tables_e table_id,
    dbal_logical_table_t * table_info);

/**
* \brief
* The function gets a string and check if it has
* dnx data indication.
* If it has - the indication will be parsed.
* If the string is null, a default value can be returned.
* If the field wasn't mandatory
*/
shr_error_e dbal_db_parse_dnx_data_indication(
    int unit,
    dbal_db_int_or_dnx_data_info_struct_t * int_or_dnx_data_as_formula,
    char *value_to_parse,
    uint8 is_mandatory,
    int default_val,
    char *str_child_name,
    char *str_to_read);

char *dbal_db_init_image_name_get(
    int unit);

/**
* \brief
* set general info for autogenerated field type init
*/
shr_error_e dbal_db_init_field_type_general_info_set(
    int unit,
    dbal_field_types_basic_info_t * field_types_info,
    char *name,
    int size,
    int is_valid,
    uint8 has_resource_name,
    dbal_field_print_type_e print_type,
    uint8 is_default,
    uint8 is_standard_1);

void dbal_init_field_type_db_struct_clear(
    int unit,
    field_type_db_struct_t * cur_field_type_param);

/**
 * \brief
 * The function adds a field read from XML to the dbal field DB.
 * Main functionality is translating the string from XML to
 * corresponding values
 */
shr_error_e dbal_db_init_field_type_add(
    int unit,
    field_type_db_struct_t * fld_type_prm,
    dbal_field_types_basic_info_t * field_types_info);

shr_error_e dbal_db_init_hw_element_group_info_init(
    int unit,
    hl_groups_info_t * groups_info);

shr_error_e dbal_db_init_field_type_struct_alloc(
    int unit,
    dbal_field_types_basic_info_t * fld_type_ent);

/**
 * \brief
 * This function set the general info for a group.
 * Called by autogenerated code.
 */
shr_error_e dbal_db_init_hw_entity_group_general_info_set(
    int unit,
    hl_group_info_t * group_info,
    uint32 element_counter,
    uint8 is_reg);

/**
 * \brief
 * This function set the name of a reg/mem
 * Called by autogenerated code
 */
void dbal_db_init_hw_entity_group_reg_mem_name_set(
    int unit,
    hl_group_info_t * group_info,
    int element_index,
    int reg_or_mem_id);

/**
 * \brief
 * This function calculate the key fields size
 * Called by autogenerated code
 */
void dbal_db_init_table_calculate_key_size(
    dbal_logical_table_t * table_entry);

/**
 * \brief
 * This function allocate the interface results array of the table
 * Precondition: table_name, nof_result_types, and "has result type" indication have been updated in table_entry
 */
shr_error_e dbal_db_init_table_db_interface_results_alloc(
    int unit,
    dbal_logical_table_t * table_entry);

/**
 * \brief
 * This function set the result type hw value according to a reference field
 */
shr_error_e dbal_db_init_table_add_result_type_physical_values_by_reference_field(
    int unit,
    multi_res_info_t * multi_res_info,
    dbal_field_types_defs_e reference_field);

/**
 * \brief
 * This function set the result type hw value according to the result type array index
 * Precondition: nof_result_types has been updated in table entry,
 * multi_results has been allocated in table_entry
 */
void dbal_db_init_table_add_result_type_physical_values_by_result_type_index(
    dbal_logical_table_t * table_entry);

/**
 * \brief
 * This function upate the sw state indication "incompatible image"
 * is_standard_1 and is_compatible_with_all_images are set by autocoder.
 */
shr_error_e dbal_db_init_table_set_table_incompatible_image_sw_state_indication(
    int unit,
    dbal_tables_e table_id,
    uint8 is_standard_1_table,
    uint8 is_compatible_with_all_images);

void dbal_init_field_type_default_restrict_values(
    field_type_db_struct_t * cur_field_type_param);

#endif /*_DBAL_DB_INIT_INCLUDED__*/
