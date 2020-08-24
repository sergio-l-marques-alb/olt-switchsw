/**
 * \file dbal_string_mgmt.h
 * $Id$
 *
 * Main functions for dbal strings (names) conversion
 * basically string_to_id and to_string functions
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DBAL_STRING_MGMT_H_INCLUDED
#define DBAL_STRING_MGMT_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <shared/utilex/utilex_rhlist.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <soc/dnx/dbal/dbal_external_defines.h>

/*************
 * FUNCTIONS *
 *************/
/**
 * \brief
 * translate a string to field ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] field_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] field_id - \n
 */
shr_error_e dbal_field_string_to_id(
    int unit,
    char *str,
    dbal_fields_e * field_id);

/**
 * \brief
 * translate a string to field ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] flag_name - \n
 *      string to translate
 *    \param [in] action_flag - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] action_flag - \n
 */
shr_error_e dbal_flag_string_to_id(
    int unit,
    char *flag_name,
    dbal_entry_action_flags_e *action_flag);

/**
 * \brief
 * translate a string to dbal_hl_tcam_access_type_e ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] access_type - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] access_type - \n
 */
shr_error_e dbal_hl_tcam_access_type_string_to_id(
    int unit,
    char *str,
    dbal_hl_tcam_access_type_e * access_type);

/**
 * \brief translate a string to field ID if not found, doest print error!
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] field_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] field_id - \n
 */
shr_error_e dbal_field_string_to_id_no_error(
    int unit,
    char *str,
    dbal_fields_e * field_id);

/**
 * \brief
 * translate a string to field type def ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] field_types_def_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] field_types_def_id - \n
 */
shr_error_e dbal_field_types_def_string_to_id(
    int unit,
    char *str,
    dbal_field_types_defs_e * field_types_def_id);

/**
 * \brief
 * Translates a string to logical table ID. If no string found, doesn't print an error.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] log_table_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    log_table_id - \n
 */
shr_error_e dbal_logical_table_string_to_id_no_error(
    int unit,
    char *str,
    dbal_tables_e * log_table_id);

/**
 * \brief
 * translate a string to logical table ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] log_table_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    log_table_id - \n
 */
shr_error_e dbal_logical_table_string_to_id(
    int unit,
    char *str,
    dbal_tables_e * log_table_id);

/**
 * \brief
 * translate a string to HW entity ID only for memory or register.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_id - the realted table
 *    \param [in] res_type_str - the result type string
 *    \param [out] res_type - the result type ID (equal to the enum value of the result type in the table)
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    log_table_id - \n
 */
shr_error_e
dbal_result_type_string_to_id(int unit, dbal_tables_e table_id, char* res_type_str, uint32* res_type);

shr_error_e
dbal_result_type_string_to_id_no_err(int unit, dbal_tables_e table_id, char* res_type_str, uint32* res_type);

shr_error_e
dbal_enum_type_string_to_id(int unit, dbal_field_types_defs_e field_type, char *enum_value_string, uint32 *enum_value);

shr_error_e
dbal_enum_type_string_to_id_no_err(int unit, dbal_field_types_defs_e field_type, char *enum_value_string, uint32 *enum_value);

CONST char * dbal_enum_id_type_to_string(int unit,dbal_field_types_defs_e field_type,uint32 enum_value);

/**
 * \brief
 * translate a string to HW entity ID only for memory or register.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] field_id - the related field
 *    \param [in] enum_value_string - the enum type string
 *    \param [out] enum_value - the enum value
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    log_table_id - \n
 */
shr_error_e
dbal_enum_string_to_id(int unit, dbal_fields_e field_id, char *enum_value_string, uint32 *enum_value);

/**
 * \brief
 * translate a string to HW entity ID only for memory or register.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] field_id - the related field
 *    \param [in] enum_value - the enum value
 *
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    log_table_id - \n
 */
CONST char *
dbal_enum_id_to_string(int unit, dbal_fields_e field_id, uint32 enum_value);

/**
 * \brief
 * translate a string to HW entity ID only for memory or register.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - string to translate
 *    \param [in] entity_type - pointer to output id
 *    \param [in] hw_entity_id - pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    log_table_id - \n
 */
shr_error_e dbal_hw_entity_string_to_id(
    int unit,
    char *str,
    dbal_hl_entity_type_e entity_type,
    int *hw_entity_id);

shr_error_e dbal_hw_field_form_reg_string_to_id(
    int unit,
    char *str,
    soc_reg_t reg,
    int *hw_entity_id);

shr_error_e dbal_hw_field_form_mem_string_to_id(
    int unit,
    char *str,
    soc_mem_t mem,
    int *hw_entity_id);

/**
 * \brief
 * translate a string to physical table ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] phy_table_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] phy_table_id - \n
 */
shr_error_e dbal_physical_table_string_to_id(
    int unit,
    char *str,
    dbal_physical_tables_e * phy_table_id);

/**
 * \brief
 * translate a string to label ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] label_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] label_id - \n
 */
shr_error_e dbal_label_string_to_id(
    int unit,
    char *str,
    dbal_labels_e * label_id);

/**
 * \brief
 * translate a string to core mode ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] core_mode_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] core_mode_id - \n
 */
shr_error_e dbal_core_mode_string_to_id(
    int unit,
    char *str,
    dbal_core_mode_e * core_mode_id);

/**
 * \brief
 * translate a string to condition type ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] condition_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] condition_id - \n
 */
shr_error_e dbal_condition_string_to_id(
    int unit,
    char *str,
    dbal_condition_types_e * condition_id);

/**
 * \brief
 * translate a string to field printing type ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] field_print_type_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] field_print_type_id - \n
 */
shr_error_e dbal_field_print_type_string_to_id(
    int unit,
    char *str,
    dbal_field_print_type_e * field_print_type_id);

/**
 * \brief
 * translate a string to encode type ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] encode_type_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    \param [out] encode_type_id - \n
 */
shr_error_e dbal_field_encode_type_string_to_id(
    int unit,
    char *str,
    dbal_value_field_encode_types_e * encode_type_id);

/**
 * \brief
 * translate a string to memory table type ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] table_type_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    table_type_id -
 */
shr_error_e dbal_logical_table_type_string_to_id(
    int unit,
    char *str,
    dbal_table_type_e * table_type_id);

/**
 * \brief
 * translate a string to access method ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] access_method_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    table_type_id -
 */
shr_error_e dbal_access_method_string_to_id(
    int unit,
    char *str,
    dbal_access_method_e * access_method_id);

/**
 * \brief
 * translate a string to maturity level ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] maturity_level_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    maturity_level_id -
 */
shr_error_e dbal_maturity_level_string_to_id(
    int unit,
    char *str,
    dbal_maturity_level_e * maturity_level_id);

/**
 * \brief
 * translate a string to tcam cs db ID.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] dbal_stage - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    dbal_stage -
 */
shr_error_e dbal_stage_string_to_id(
    int unit,
    char *str,
    dbal_stage_e * dbal_stage);

/**
 * \brief
 * translate a string to table status.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - \n
 *      string to translate
 *    \param [in] table_status_id - \n
 *      pointer to output id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    table_status_id -
 */
shr_error_e dbal_table_status_string_to_id(
    int unit,
    char *str,
    dbal_table_status_e * table_status_id);
/**
 * \brief
 * translate field ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] field_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 */
CONST char *dbal_field_to_string(
    int unit,
    dbal_fields_e field_id);

/**
 * \brief
 * translate hl_tcam_access_type to string. In case of unknown
 * id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] hl_tcam_access_type - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 */
char *dbal_hl_tcam_access_type_to_string(
    int unit,
    dbal_hl_tcam_access_type_e hl_tcam_access_type);

/**
 * \brief
 * translate field type def. ID to string. In case of unknown
 * id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] field_types_def_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 */
char *dbal_field_types_def_to_string(
    int unit,
    dbal_field_types_defs_e field_types_def_id);


/**
 * \brief
 * translate logical table ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_id - table ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_logical_table_to_string(
    int unit,
    dbal_tables_e table_id);


/**
 * \brief
 * translate logical table ID and result type to result type string.
 * In case of unknown table id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_id - table ID to translate
 *    \param [in] result_type - result type to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
CONST char *dbal_result_type_to_string(
    int unit,
    dbal_tables_e table_id,
    int result_type);

/**
 * \brief
 * translate logger type to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] dbal_logger_type - logger type to to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_logger_type_to_string(
    int unit,
    dnx_dbal_logger_type_e dbal_logger_type);

/**
 * \brief
 * translate physical table ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] phy_table_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_physical_table_to_string(
    int unit,
    dbal_physical_tables_e phy_table_id);

/**
 * \brief
 * translate label ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] label_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_label_to_string(
    int unit,
    dbal_labels_e label_id);

/**
 * \brief
 * translate hitbit val to string. In case of unknown hitbit
 * val, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] physical_db_id - the physical db used for hitbit
 *    \param [in] hitbit - \n
 *      hitbit to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *
dbal_mdb_hitbit_to_string(
    int unit,
    dbal_physical_tables_e physical_db_id,
    uint8 hitbit);

/**
 * \brief
 * translate attribute val to string. In case of unknown
 * attribute val, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] attr_type - attribute to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *
dbal_entry_attr_to_string(
    int unit,
    uint32 attr_type);

/**
 * \brief
 * translate HW entity ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] hw_entity_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 */
char *dbal_hw_entity_to_string(
    int unit,
    int hw_entity_id);

/**
 * \brief
 * translate core mode ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] core_mode_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_core_mode_to_string(
    int unit,
    dbal_core_mode_e core_mode_id);

/**
 * \brief
 * translate conrition type ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] condition_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_condition_to_string(
    int unit,
    dbal_condition_types_e condition_id);

/**
 * \brief
 * translate field print type ID to string.
 * In case of unknown id, a default unknown string is returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] field_print_type_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_field_print_type_to_string(
    int unit,
    dbal_field_print_type_e field_print_type_id);

/**
 * \brief
 * translate encode type ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] encode_type_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_field_encode_type_to_string(
    int unit,
    dbal_value_field_encode_types_e encode_type_id);

/**
 * \brief
 * translate action flag ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] action_flag_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_action_flags_to_string(
    int unit,
    dbal_entry_action_flags_e action_flag_id);

/**
 * \brief
 * translate access method ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] access_method_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_access_method_to_string(
    int unit,
    dbal_access_method_e access_method_id);

/**
 * \brief
 * translate table type ID to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_type_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_table_type_to_string(
    int unit,
    dbal_table_type_e table_type_id);

/**
 * \brief
 * translate table status to string. In case of unknown id, a default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_status_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_table_status_to_string(
    int unit,
    dbal_table_status_e table_status_id);

/**
 * \brief
 * translate maturity level to string. In case of unknown id, a
 * default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] maturity_level_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_maturity_level_to_string(
    int unit,
    dbal_maturity_level_e maturity_level_id);

/**
 * \brief
 * translate tcam_cs db to string. In case of unknown id, a
 * default unknown string will be returned
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] dbal_stage - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *dbal_stage_to_string(
    int unit,
    dbal_stage_e dbal_stage);

/** 
 * \brief 
 * translate dbal field permission to string. In case of unknown id, a
 * default unknown string will be returned
 *  
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] permission_id - \n
 *      ID to translate
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */ 
char * 
dbal_field_permission_to_string(
    int unit, 
    dbal_field_permission_e permission_id);

/**
 * \brief
 * returns all the stringable paramters of the requested table.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] table_id - requested table ID
 *    \param [in] table_string - pointer to struct that represents the table string
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
shr_error_e dbal_full_table_to_string(
    int unit,
    dbal_tables_e table_id,
    dbal_table_string_t * table_string);

/**
 * \brief - Decode signal`s value according to dbal parent field, dbal enum, structure or define
 *          It supports multuple level of decoding
 *          It case that desc is dbal table name - data and data_size_bits are NULL
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] desc - DBAL field name or table name
 * \param [in] data -  Pointer to uint32 data array for this field
 * \param [in] data_size_bits - Size of the field in bits
 * \param [in] parsed_info - List of sub-fields with their values based on field value
 * \param [in] from_n - from which stage the callback has been called for
 * \param [in] to_n - to which stage the callback has been called for
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE]);

/**
 * \brief
 * translate a hw entity string to hw entity group id.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] str - string to translate
 *    \param [in] group_id - pointer to output groupid
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 *  \par INDIRECT OUTPUT:
 *    group_id -
 */
shr_error_e
dbal_hw_entity_group_string_to_id(
    int unit,
    char *str,
    dbal_hw_entity_group_e * group_id);

/**
 * \brief
 * returns hw entity group name of the requested group id.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] hw_entity_group_id - requested group id
 *  \par INDIRECT INPUT:
 *    None
 *  \par DIRECT OUTPUT:
 *    char * - \n
 *      output string
 *  \par INDIRECT OUTPUT:
 *    None
 */
char *
dbal_hw_entity_group_to_string(
    int unit,
    dbal_hw_entity_group_e hw_entity_group_id);

#endif
