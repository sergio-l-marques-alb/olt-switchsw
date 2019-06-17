/**
 * \file dbal_db_init.h
 * $Id$
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Main functions for init the dbal fields and logical tables DB
 *
 */

#ifndef DBAL_DB_INIT_H_INCLUDED
#  define DBAL_DB_INIT_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#  include <soc/dnx/dbal/dbal_structures.h>
#  include <shared/dbx/dbx_xml.h>
#  include <shared/dbx/dbx_file.h>
#  include <soc/dnx/pp_stage.h>

/*************
 *  DEFINES  *
 *************/

/**
 * \brief
 * No indication - regular init procedure
 */
#  define DBAL_INIT_FLAGS_NONE        0x0

/**
 * \brief
 * identifier for dnx data reference
 */
#  define DB_INIT_DNX_DATA_INDICATION                     "DNX_DATA"

/**
 * \brief
 * identifier for Physical Table reference
 * for SW access table size
 */
#  define DB_INIT_PHYDB_INDICATION                        "PHYDB"

/**
 * \brief
 * path to fields xml file(s) (relative to DB path folder)
 */
#  define DB_INIT_DIR_PATH_FIELDS                         "dbal/field_types/"

/**
 * \brief
 * path to tables xml validation file(s) (relative to DB path
 * folder)
 */
#  define DB_INIT_TABLES_PATH          "dbal/tables"

/**
 * \brief
 * path to mdb tables xml validation file(s) (relative to DB
 * path folder)
 */
#  define DB_INIT_VALIDATION_TEST_PATH_FIELDS             "dbal/validation_test/fields"

/** 
 * \brief jericho 2 device name
 */
#define DBAL_DEVICE_NAME                (SOC_IS_J2C(unit)? "J2C": (SOC_IS_JERICHO_2_B0(unit)? "J2B0": "JR2"))

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
    char encode_type[DBAL_MAX_SHORT_STRING_LENGTH];

    /** encoding input param as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t encode_param1_dnx_data;

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
    dbal_db_int_or_dnx_data_info_struct_t start_offest_dnx_data;

    /** length */
    dbal_db_int_or_dnx_data_info_struct_t length_dnx_data;

} dbal_db_struct_field_info_struct_t;

/**
 * \brief
 * enum structure - read from XML
 */
typedef struct
{
    /** enum name */
    char name_from_interface[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    /** enum value as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t value_from_mapping_dnx_data;

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
    dbal_db_int_or_dnx_data_info_struct_t define_value;

} dbal_db_defines_info_struct_t;

/**
 * \brief
 * field DB - read from XML
 */
typedef struct
{
    /** field name */
    char name[DBAL_MAX_STRING_LENGTH];

    /** size as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t max_size_dnx_data;

    /** field type */
    char print_type[DBAL_MAX_SHORT_STRING_LENGTH];

    /** allocatorName */
    char resource_name[DBAL_MAX_STRING_LENGTH];

    /** num of child fields */
    int nof_childs;

    /** child fields list */
    dbal_db_child_field_info_struct_t *childs;

    /** num of enum values */
    int nof_enum_vals;

    /** field enum values */
    dbal_db_enum_info_struct_t *enums;

    /** num of struct fields */
    int nof_structs;

    /** struct fields list */
    dbal_db_struct_field_info_struct_t *structs;

    /** num of defines values */
    int nof_defined_vals;

    /** field defines values */
    dbal_db_defines_info_struct_t *define;

    /** field max value as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t max_value_dnx_data;

    /** field min value as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t min_value_dnx_data;

    /** field const value as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t const_value_dnx_data;

    /** Number of valid illegal values */
    int nof_illegal_value;

    /** Illegal values as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t *illegal_values_dnx_data;

    /** field default value as integer or DNX_DATA reference   */
    dbal_db_int_or_dnx_data_info_struct_t default_val_dnx_data;

    /** field default value for enum fields */
    char default_val_for_enum[DBAL_MAX_STRING_LENGTH];

    /** default value indication */
    uint8 default_val_valid;

    /** encode type */
    char encode_type[DBAL_MAX_SHORT_STRING_LENGTH];

    /** encoding input param as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t encode_param1_dnx_data;

    /** RefernceField that use to encode parent fields */
    char refernce_field_name[DBAL_MAX_STRING_LENGTH];

} field_type_db_struct_t;

/**
 * \brief
 * field in table DB - read from XML
 */
typedef struct
{
    /** field name */
    char name[DBAL_MAX_STRING_LENGTH];

    /** field validity as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t valid_dnx_data;

    /** size as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t size_dnx_data;

    /** is this field support multiple instances */
    dbal_db_int_or_dnx_data_info_struct_t nof_instances_dnx_data;

    /** max value of field in table */
    dbal_db_int_or_dnx_data_info_struct_t max_value_dnx_data;

    /** min value of field in table */
    dbal_db_int_or_dnx_data_info_struct_t min_value_dnx_data;

    /** field const value as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t const_value_dnx_data;

    /** permission */
    dbal_field_permission_e permission;

    /** indication for reverse order of instances  */
    uint8 is_reverse_order;

    /** field offset in table [bits] */
    dbal_db_int_or_dnx_data_info_struct_t offset_dnx_data;

    /** field type */
    dbal_field_types_defs_e field_type;

    /** arr prefix value */
    dbal_db_int_or_dnx_data_info_struct_t arr_prefix_value_dnx_data;

    /** arr prefix width */
    dbal_db_int_or_dnx_data_info_struct_t arr_prefix_width_dnx_data;

} table_db_field_params_struct_t;

/**
 * \brief
 * Offset in HW structure - read from XML
 */
typedef struct
{
    /** encoding formula  */
    char formula[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    /** parsed formula  by autocoder (when simple int) */
    int formula_int; 
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
    dbal_db_int_or_dnx_data_info_struct_t value_dnx_data;

    /** use enum value of the field for value indication */
    char enum_val[DBAL_MAX_STRING_LENGTH];

} table_db_access_condition_struct_t;

/**
 * \brief
 * logical table access DB - read from XML
 * MDB tables only
 */
typedef struct
{
    /** table app id - MDB tables only */
    dbal_db_int_or_dnx_data_info_struct_t app_db_id_dnx_data;

    /** table app id size in bits - MDB tables only */
    dbal_db_int_or_dnx_data_info_struct_t app_db_id_size_dnx_data;

    /** table physical db - MDB tables only */
    char phy_db[DBAL_MAX_SHORT_STRING_LENGTH];

    /** Indicate if table is part of Standard_1 tables */
    uint8 is_standard_1_table;

    /** Indicate if supported image is standard_1*/
    uint8 is_standard_1_image;

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

    /** access type for tcam key/mask/result */
    dbal_hl_tcam_access_type_e tcam_access_type;

    /** access name, field or register */
    /* Deprecated  
      char access_name[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
      */

    /** access name, field or register */
    char access_group_name[DBAL_MAX_STRING_LENGTH];

    /** indicate how many hw entities (reg/mem) in groups there is
     *  If 0, then it means, that a hw entity was indicated
     *  instead. See hw_entity_id.
     *  Max value: DBAL_MAX_NUMBER_OF_HW_ELEMENTS */
/*
    int nof_hw_entity_ids_in_group;
    int hw_entity_ids_in_group[DBAL_MAX_NUMBER_OF_HW_ELEMENTS];
    */
    /** hw entity (reg/mem)  */
    int reg_mem_hw_entity_id; 

    /** size in access [bits] */
    int access_size;

    /** offset in access [bits] */
    int access_offset;

    /** mapping encoding */
    /** encode type */
    char encode_type[DBAL_MAX_SHORT_STRING_LENGTH];

    /** encoding input param as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t encode_param1_dnx_data;

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

    /** alias access name, memory or register */
    char alias_name[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

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
 * logical table access DB - read from XML
 * hard logic tables only
 */
typedef struct
{
    /** indicate that the fields mapping is done using packed
     *  fields */
    uint8 is_packed_fields;

    /** number of access mapping*/
    int nof_access;

    table_db_access_params_struct_t access[DBAL_TABLE_MAX_NUM_OF_ACCESS_FIELDS];

    /** result type logical value (name) */
    char mapping_result_name[DBAL_MAX_STRING_LENGTH];

} table_db_hl_access_struct_t;

/**
 * \brief
 * Results set in table.
 * Union is implemented with multiple result sets
 */
typedef struct
{
    /** indication for result type is found in mapping */
    uint8 result_is_mapped;

    /** indication that result type is enabled.
     *  Default: enabled.
     *  Used for result type disabled for specific device. */
    uint8 is_disabled;

    /** result type logical value (name) */
    char result_type_name[DBAL_MAX_STRING_LENGTH];

    /** result_type field value */
    int nof_physical_values_for_result_type;
    int result_type_physical_value[DBAL_MAX_NOF_HW_VALUES_TO_RESULT_TYPE];

    /** number of result fields in table */
    int nof_res_fields;

    /** result fields array */
    table_db_field_params_struct_t result_fields[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
} table_db_results_field_set_struct_t;

/**
 * \brief
 * logical table full DB, read from XML
 */
typedef struct
{
    /** table name */
    char name[DBAL_MAX_STRING_LENGTH];

    /** access method */
    dbal_access_method_e access_method;

    /** maturity level */
    dbal_maturity_level_e maturity_level;

    /** table type */
    char type[DBAL_MAX_SHORT_STRING_LENGTH];

    /** validity as integer or DNX_DATA reference */
    dbal_db_int_or_dnx_data_info_struct_t valid_dnx_data;

    /** number of key fields in table */
    int nof_key_fields;

    /** key fields array */
    table_db_field_params_struct_t key_fields[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];

    /** indicates if table has the result_type field */
    uint8 result_type_exists;

    /** number of results fields sets - union indication */
    int num_of_results_sets;

    /** Indication for MDB tables that the RESULT_TYPE value is mapped to sw state */
    dbal_db_int_or_dnx_data_info_struct_t result_type_in_sw_dnx_data;

    char reference_field_name[DBAL_MAX_STRING_LENGTH];

    /** results fields, support union Size of the array: DBAL_MAX_NUMBER_OF_RESULT_TYPES */
    table_db_results_field_set_struct_t *results_set;

    /** labels array */
    char labels[DBAL_MAX_NOF_ENTITY_LABEL_TYPES * DBAL_MAX_SHORT_STRING_LENGTH];

    /** core mode  */
    char core_mode[DBAL_MAX_SHORT_STRING_LENGTH];

    /** TCAM pipe stage - MDB TCAM and TCAM CS only */
    char pipe_stage[DBAL_MAX_SHORT_STRING_LENGTH];

    /** table size - SW(EM), HL(TCAM) tables only */
    dbal_db_int_or_dnx_data_info_struct_t indirect_table_size_dnx_data;

    /** table size - SW(MDB size reference), tables only */
    dbal_physical_tables_e physical_db_id;
    char table_size_str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    /** access DB */
    dbal_db_int_or_dnx_data_info_struct_t is_hook_active_dnx_data; /** incase that the hook is active the access to the table is done by dedicated functions. hooks are generally used to fix HW issues or support specific HW behaviors */

    /** access DB - MDB tables only */
    table_db_mdb_access_struct_t *mdb_access;

    /** access DB - hard logic tables only
     *  Size of the array: DBAL_MAX_NUMBER_OF_RESULT_TYPES */
    table_db_hl_access_struct_t *hl_access;

    /** pemla DB mapping ID */
    dbal_db_int_or_dnx_data_info_struct_t pemla_db_id_dnx_data;

    /** access DB - pemla tables only */
    table_db_pemla_access_struct_t *pemla_access;
} table_db_struct_t;

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief
 * Initializes a dbal_field_types_basic_info_t structure.
 * Note that this function does not deallocate memory pointed to by fields within the structure.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] field_type_info - \n
 *      dbal_field_types_basic_info_t structure
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_field_types_basic_info_t_init(
    int unit,
    dbal_field_types_basic_info_t * field_type_info);

/**
 * \brief
 * The function initializes dbal field types component with default values
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] field_types_info - \n
 *      dbal tables info DB
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_field_types_set_default(
    int unit,
    dbal_field_types_basic_info_t * field_types_info);

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
 * The function read the fields XML file(s) and fill the fields DB accordingly
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] flags
 *    \param [in] field_info - \n
 *      dbal tables info DB
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_field_types(
    int unit,
    int flags,
    dbal_field_types_basic_info_t * field_info);

/**
 * \brief
 * The function read the MDB tables XML file(s) and fill
 * the tables DB accordingly
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] flags
 *    \param [in] table_info - \n
 *       dbal tables info DB
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_logical_tables(
    int unit,
    int flags,
    dbal_logical_table_t * table_info);

/**
 * \brief
 * The function run some logical validation on the fields DB
 * In addition it fills the DB with paraneters that not directly
 * read from XML
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] field_types_info - \n
 *      dbal tables info DB
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */
shr_error_e dbal_db_init_field_types_logical_validation(
    int unit,
    dbal_field_types_basic_info_t * field_types_info);

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
shr_error_e dbal_db_init_tables_logical_validation(
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
    table_db_struct_t * cur_table_param,
    char *dbal_table_name,
    char *valid,
    dbal_access_method_e access_method);

/** 
 * Set the maturity level per device 
 */
shr_error_e dbal_db_init_maturity_level_set(
    int unit,
    table_db_struct_t * cur_table_param,
    dbal_maturity_level_e maturity_level);

/**
 * \brief
 * clear specific access section of the table_db_struct_t
 */
void dbal_db_init_table_db_struct_access_clear(
    table_db_struct_t * table_db);

/** 
 *  Set the interface informations to the table db struct.
 *  Note: doens't include keys and results. It's just the global
 *  info
 */
shr_error_e dbal_db_init_app_db_set_interface(
    int unit,
    table_db_struct_t * cur_table_param,
    char *type,
    char *labels);

/** 
 * Set a db field to the table db struct. 
 * Used for keys, result, result mapping 
 */
shr_error_e dbal_db_init_table_set_field(
    int unit,
    table_db_field_params_struct_t * field_db,
    char *field_name,
    char *valid,
    char *size,
    char *offset,
    char *max_value,
    char *min_value,
    char *const_value,
    char *nof_instances,
    dbal_field_permission_e permission,
    uint8 is_reverse_order,
    char *field_type);

/** 
 * Set a arr prefix for db field to the table db struct Used for
 * keys, result, result mapping 
 */
shr_error_e dbal_db_init_table_set_field_arr_prefix(
    int unit,
    table_db_field_params_struct_t * field_db,
    char *arr_prefix_value,
    char *arr_prefix_width);

/** 
 * Set general information for app to phy db 
 */
shr_error_e dbal_db_init_table_set_app_to_phy_db(
    int unit,
    table_db_struct_t * cur_table_param,
    char *core_mode,
    char *reference_field_name);

/**
 *  indicate if result types are mapped with sw state values
 */
shr_error_e dbal_db_init_table_set_multiple_result_mapping_is_sw_state(
    int unit,
    table_db_struct_t * cur_table_param,
    char *sw_state);

/** 
 * set result type with hw values  
 */
shr_error_e dbal_db_init_table_set_result_type_physical_values(
    int unit,
    table_db_struct_t * cur_table_param,
    char *logical_value,
    char *result_type_hw_value,
    char *result_type_hw_value1,
    char *result_type_hw_value2,
    char *result_type_hw_value3,
    char *result_type_hw_value4,
    char *result_type_hw_value5,
    char *result_type_hw_value6,
    char *result_type_hw_value7);

/** 
 * set register/memory access to table db struct
 */
shr_error_e
dbal_db_init_table_set_access_register_memory(
    int unit,
    table_db_access_params_struct_t * access_params,
    char *group_name,
    int reg_mem_hw_entity_id, 
    char *group_offset_formula,
    int  group_offset, 
    char *array_offset_formula,
    int  array_offset, 
    char *entry_offset_formula,
    int  entry_offset, 
    char *block_index_formula,
    int block_index, 
    char *data_offset_formula,
    int data_offset, 
    int hw_field,
    char *alias_name,
    char *alias_data_offset_formula, 
    int alias_data_offset);

/** 
 * set a access condition to table db struct  
 */
shr_error_e dbal_db_init_table_set_access_condition(
    int unit,
    table_db_access_condition_struct_t * access_condition,
    char *type,
    char *field,
    char *value,
    char *enum_value);

/** 
 * set access field to tagble db struct. 
 * Note it doesn't include the register/memory/sw state itself 
 * and conditions, just general info about the access field  
 */
shr_error_e dbal_db_init_table_set_access_field(
    int unit,
    table_db_access_params_struct_t * access_params,
    char orig_field_name[DBAL_MAX_STRING_LENGTH],
    int instanceIndex,
    char *field_size,
    char *field_offset,
    uint8 is_tcam,
    char *encode_type,
    char *encode_value);

/** 
 * set access field to table db struct, for packed field
 * Note it doesn't include the register/memory/sw state itself 
 * and conditions, just general info about the access field  
 */
shr_error_e dbal_db_init_table_set_access_packed_field(
    int unit,
    table_db_access_params_struct_t * access_params,
    char *field_size,
    char *encode_type,
    char *encode_value);

/** 
 * hard logic direct access types 
 */
shr_error_e dbal_db_init_hard_logic_direct_set_access_type(
    int unit,
    table_db_access_params_struct_t * access_params,
    uint8 is_register,
    uint8 is_memory);

/**
 * hard logic sw state access 
 */
shr_error_e dbal_db_init_sw_state_set_indirect_table_size(
    int unit,
    table_db_struct_t * cur_table_param,
    char *indirect_table_size);

/** 
 * No explicit mapping defined in the app to phy db mapping 
 * set pemla access mapping by index
 */
void dbal_db_init_table_set_pemla_mapping_by_index(
    table_db_struct_t * table_param);

/** 
 * set access mdb indo  
 */
shr_error_e dbal_db_init_table_set_mdb_info(
    int unit,
    table_db_struct_t * cur_table_param,
    char *phy_db,
    char *app_db_id_size,
    char *app_db_id,
    char *stage);

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

void dbal_db_init_mdb_image_name_is_std_1(
    int unit,
    uint8 *mdb_image_is_std_1);

/**
* \brief 
* set general info for autogenerated field type init
*/
shr_error_e dbal_db_init_field_type_general_info_set(
    int unit,
    field_type_db_struct_t * cur_field_type_param,
    char *name,
    char *size,
    char *resource,
    char *print_type);

/**
* \brief 
* set general info for autogenerated field type init
*/
shr_error_e dbal_db_init_field_type_mapping_restriction_set(
    int unit,
    field_type_db_struct_t * cur_field_type_param,
    char *max_value,
    char *min_value,
    char *const_value);

/** 
 * \brief 
 * for field types, for phy mapping, the function set the 
 * defaultValue
 */
shr_error_e dbal_db_init_field_types_phy_mapping_default_value_set(
    int unit,
    field_type_db_struct_t * cur_field_type_param,
    uint8 is_enum,
    char *default_val_string);

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

/** 
 * \brief 
 * This function set the general info for a group. 
 * Called by autogenerated code.  
 */
shr_error_e dbal_db_init_hw_entity_group_general_info_set(
    int unit,
    hl_group_info_t * group_info,
    char *name,
    uint32 element_counter,
    uint8 is_reg);

/**
 * \brief 
 * This function set the name of a reg/mem 
 * Called by autogenerated code 
 */
shr_error_e 
dbal_db_init_hw_entity_group_reg_mem_name_set(
    int unit,
    hl_group_info_t * group_info,
    int element_index,
    char *reg_or_mem_name);


/** 
 * \brief 
 * This function set the name of a reg, and its mapped value for 
 * a specific device 
 * Called by autogenerated code 
 */ 
shr_error_e
dbal_db_init_hw_entity_porting_reg_mapping_set(
    int unit, 
    hl_porting_info_t * hl_porting_info, 
    char * name, 
    char * map_to_name);

/** 
 * \brief 
 * This function set the name of a reg, and its mapped value for 
 * a specific device 
 * Called by autogenerated code 
 */ 
shr_error_e
dbal_db_init_hw_entity_porting_mem_mapping_set(
    int unit, 
    hl_porting_info_t * hl_porting_info, 
    char * name, 
    char * map_to_name);



#endif /*_DBAL_DB_INIT_INCLUDED__*/
