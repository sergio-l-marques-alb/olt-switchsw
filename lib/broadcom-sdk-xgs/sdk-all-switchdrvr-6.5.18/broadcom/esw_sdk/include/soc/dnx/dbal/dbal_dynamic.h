/**
 *\file dbal_dynamic.h
 * Main interfaces for DBAL for dynamic operations. dynamic operations are used to create/delete dbal entities after
 * DBAL init. not all accesses supports dynamic operations. working with dynamic operations required advance DBAL
 * understanding.
 */

#ifndef DBAL_DYNAMIC_H_INCLUDED
#define DBAL_DYNAMIC_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#    error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *STRUCTURES *
 *************/

/**
 *\brief returns the decoded value and sub-field according to the encoded value added.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] field_type -  the relevant field type related to the field\n
 *   \param [in] field_name -  the new field name\n
 *   \param [in] field_id -  a pointer to the requested new field ID\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the sub_field_id and sub_field_value pointers \n
 */
shr_error_e dbal_fields_field_create(
    int unit,
    dbal_field_types_defs_e field_type,
    char field_name[DBAL_MAX_STRING_LENGTH],
    dbal_fields_e * field_id);

/**
 * \brief
 * create a dynamic DBAL table, only applicable for specific access methods such as KBP and MDB. those tables are used for ACLs. Dynamic result types are not supported for dynamic tables
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] access_method - how to access to HW example: KBP/MDB
 *   \param [in] table_type - TCAM,DIRECT,LPM
 *   \param [in] core_mode - SBC,DPC
 *   \param [in] nof_key_fields - NOF key fields in the table.
 *   \param [in] keys_info - key fields, no duplications.
 *               The key fields are ordered from the MSB to the LSB (index 0 is placed at the MSB) for MDB and KBP,
 *               and from LSB to MSB for hard logic (but dynamic table for hard logic is not supported at the moment).
 *   \param [in] nof_result_fields - NOF key fields in the table.
 *   \param [in] results_info - Result fields 2d array, no duplications
 *               The result fields are ordered from the MSB to the LSB (index 0 is placed at the MSB) for MDB and KBP,
 *               and from LSB to MSB for hard logic (but dynamic table for hard logic is not supported at the moment).
 *   \param [in] table_name - table name, in case of NULL the name DYNAMIC_<table_id> is set.
 *   \param [in] table_id - pointer to the returned table ID.
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_table_create(
    int unit,
    dbal_access_method_e access_method,
    dbal_table_type_e table_type,
    dbal_core_mode_e core_mode,
    int nof_key_fields,
    dbal_table_field_input_info_t * keys_info,
    int nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    char *table_name,
    dbal_tables_e * table_id);

/**
 * \brief
 * add dynamic result type to table, only applicable for PPMC table. Function produces a generic state journal entry and
 * is rolled backable, unless error occurs during execution of the function itself. 
 *   \param [in] unit
 *   \param [in] table_id - table to which to add result type
 *   \param [in] nof_result_fields - number of result fields
 *   \param [in] results_info - set of results info
 *   \param [in] res_type_name - name of result type
 *   \param [in] result_type_size - result type size
 *   \param [in] result_type_hw_value - result type value
 *   \param [out] result_type_index - result type index added in the table object
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_result_type_add(
    int unit,
    dbal_tables_e table_id,
    int nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    char *res_type_name,
    int result_type_size,
    int result_type_hw_value,
    int *result_type_index);

/**
 * \brief
 * destroy a DBAL table, only applicable for dynamic tables
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id - the table to destroy
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_table_destroy(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * read DBAL table form sw state, only applicable for dynamic
 * tables
 *   \param [in] unit
 *   \param [in] table_id - the table to destroy
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_table_restore(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * restore all dynamic tables from sw state
 *   \param [in] unit - the relevant unit
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_restore_tables(
    int unit);

/**
 * \brief
 * this API update the access info according to the access layer type
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id - the table to update
 *   \param [in] access_method - access method to update DBAL_ACCESS_METHOD_XXX
 *                               Important: if DBAL_ACCESS_METHOD_MDB, call mdb_init_logical_table with same table_id and access_info instead.
 *   \param [in] access_info - a pointer to the access info to update should be one of the following structs:
 *          dbal_table_mdb_access_info_t \ dbal_table_sw_access_info_t
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_table_access_info_set(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e access_method,
    void *access_info);

/**
 *\brief destroys a field this API only applicable for dynamic fields that was created by dbal_fields_field_create
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] field_id -  the field ID to delete\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the sub_field_id and sub_field_value pointers \n
 */
shr_error_e dbal_fields_field_destroy(
    int unit,
    dbal_fields_e field_id);

/**
 * \brief this API update in the TABLE DB the new MIN/MAX value of the table field. this API is not Warmboot protected, 
 *        if user updated predefine value, he needs to update the value again after warmboot. 
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested fireld is key or value field\n
 *   \param [in] result_type -  valid in cases of value fields are requested. indicate the result type index\n
 *   \param [in] instance_id -  valid in cases of value fields are requested. indicate the instance idx\n
 *   \param [in] value_type - the type of the value user want to set to the field\\n
 *   \param [in] predef_value -  pointer to the requested field info\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e dbal_tables_field_predefine_value_update(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int is_key,
    int result_type,
    int instance_id,
    dbal_field_predefine_value_type_e value_type,
    uint32 predef_value);

shr_error_e dbal_tables_app_id_set(
    int unit,
    dbal_tables_e table_id,
    uint32 app_db_id);

/**
 * \brief
 * delete dynamic result type from table, only applicable for
 * PPMC table.
 * Function produces a generic state journal entry and is rolled backable,
 * unless error occurs during execution of the function itself.
 *   \param [in] unit
 *   \param [in] table_id - table to which to add result type
 *   \param [in] result_type_index - Index of result type to be deleted
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_dynamic_result_type_delete(
    int unit,
    dbal_tables_e table_id,
    int result_type_index);

shr_error_e dbal_tables_dynamic_result_type_destroy_all(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * read dynamic result type form sw state, only applicable for
 * PPMC table
 *   \param [in] unit
 *   \param [in] table_id - the table to restore
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_result_type_restore_rollback(
    int unit,
    dbal_tables_e table_id);

#endif /* DBAL_DYNAMIC_H_INCLUDED */
