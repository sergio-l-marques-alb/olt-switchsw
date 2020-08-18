/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef DBAL_H_INCLUDED
#define DBAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#    error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal_structures.h>

#include <soc/dnx/dbal/dbal_string_mgmt.h>

/**
 *\brief sets information related to a iterator rule can be called before dbal_iterator_get_next()
 *
 * \par DIRECT INPUT:
 *   \param [in]  dbal_iterator - iterator info\n
 *   \param [in]  field_id      - the field ID  related to the condition \n
 *   \param [in]  field_val     - value to be compared\n
 *   \param [in]  condition     - condition (equal_to / bigger_than...)\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
#define DBAL_ITERATOR_RULE_SET(dbal_iterator, field_id, field_val, condition)       \
do                                                                                  \
{                                                                                   \
    dbal_iterator.rule.field_id = field_id;                                         \
    dbal_iterator.rule.field_val = field_val;                                       \
    dbal_iterator.rule.condition = condition;                                       \
}                                                                                   \
while (0)

/**
 *  \brief max number of dbal function entry handles to perform
 *         table action with
 */
#define DBAL_FUNC_NOF_ENTRY_HANDLES   10

/**
 * \brief DBAL Function entry declarations.
 *
 * This macro must appear in each function using DBAL before
 * SHR_FUNC_INIT_VARS(unit), a compilation error will apear if
 * you fail to do so.
 * This macro is used to maintain a local db of dabl handles
 * used in the function. all dbal hanles used in this function
 * are relesef at the function exit point using
 * DBAL_FUNC_FREE_VARS macro
 * This MACRO has 1 extra { at the begining for compilation
 * reason as it defines variable after code. the corrosponding }
 * is located at the DBAL_FUNC_FREE_VARS macro.
 *
 * internal variables:
 * _dbal_handles[DBAL_SW_NOF_ENTRY_HANDLES] - DB for holding
 *    dbal handles used in this function
 * _free_handle_index - loop index for free section, used also
 *    for validating the free macro exist, otherwise a variable not
 *    used warning will apear in compilation
 * _nof_local_handles - indicate the number of dbal hables used
 *    in this fuction
 *
 * \par DIRECT INPUT:
 *   \param [in] _in_unit -
 *     Value of 'unit' input parameter at entry.
 * \par INDIRECT INPUT:
 * \par INDIRECT OUTPUT:
 *
 *  Example:
 *
 * \code{.c}
 * int my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *
 *   SHR_FUNC_INIT_VARS(unit);
 *   DBAL_FUNC_INIT_VARS(unit);
 *
 *   DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id);
 *
 * exit:
 *   DBAL_FUNC_FREE_VARS;
 *   SHR_FUNC_EXIT;
 * }
 * \endcode
 */
#define DBAL_FUNC_INIT_VARS(_in_unit)                                                                           \
{                                                                                                               \
  int _dbal_handles[DBAL_FUNC_NOF_ENTRY_HANDLES]=                                                               \
      {DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES, \
       DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES, \
       DBAL_SW_NOF_ENTRY_HANDLES,DBAL_SW_NOF_ENTRY_HANDLES};                                                    \
  int _free_handle_index;                                                                                       \
  int _nof_local_handles = 0;                                                                                   \
  int _unit = _in_unit

/**
 * \brief DBAL HANDLE ALLOC MACRO declarations.
 *
 * This macro is used to allocate a dbal handle for a given
 * table_id. The allocated handle is then stored in the
 * function's local handles DB. At the exit point of the
 * function, the handle is freed
 *
 * \par DIRECT INPUT:
 *   \param [in] _unit -
 *     Value of 'unit' input parameter at entry.
 *   \param [in] _table_id - value of requested table_id
 *   \param [in] _entry_handle_id - value of current entry_id
 * \par INDIRECT INPUT:
 * \par INDIRECT OUTPUT:
 *    \param [in] _entry_handle_id - value of current entry_id
 * Example:
 *
 * \code{.c}
 * int my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *   uint32 entry_handle_id;
 *
 *   SHR_FUNC_INIT_VARS(unit);
 *   DBAL_FUNC_INIT_VARS(unit);
 *
 *   DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id);
 *
 * exit:
 *   DBAL_FUNC_FREE_VARS;
 *   SHR_FUNC_EXIT;
 * }
 * \endcode
 */
#define DBAL_HANDLE_ALLOC(_unit, _table_id, _entry_handle_id)                         \
    dbal_entry_handle_take_macro(_unit, _table_id, _entry_handle_id);                 \
do                                                                                    \
{                                                                                     \
    int _handle_index;                                                                \
    uint32 *_entry_handle_id_ptr = _entry_handle_id;                                  \
                                                                                      \
   /** if no room for another handle in this function exit with err */                \
    if (_nof_local_handles == DBAL_FUNC_NOF_ENTRY_HANDLES)                            \
    {                                                                                 \
        dbal_entry_handle_release_macro(_unit, *_entry_handle_id_ptr);                \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "function dbal handle usage exceed maximum %d\n"\
                    ,DBAL_FUNC_NOF_ENTRY_HANDLES);                                    \
    }                                                                                 \
    /** search for a free position in the hadles DB */                                \
    for (_handle_index=0; _handle_index<DBAL_FUNC_NOF_ENTRY_HANDLES; _handle_index++) \
    {                                                                                 \
        if (_dbal_handles[_handle_index] == DBAL_SW_NOF_ENTRY_HANDLES)                \
        {                                                                             \
            /** store the handle id in the local DB */                                \
            _dbal_handles[_handle_index] = *_entry_handle_id_ptr;                     \
            _nof_local_handles++;                                                     \
            break;                                                                    \
        }                                                                             \
    }                                                                                 \
} while(0)

/**
 * \brief DBAL HANDLE CLEAR MACRO declarations.
 *
 * This macro is used to clear a dbal handle or to reassign
 * a dbal handle to another table. After allocating and using
 * the handle, this macro sets the handle to its initial state
 * so it can be reused either by the same table or by another
 * table.
 *
 * \par DIRECT INPUT:
 *   \param [in] _unit -
 *     Value of 'unit' input parameter at entry. If there is no
 *     relevant unit to report, use NO_UNIT.
 *   \param [in] _table_id - value of requested table_id
 *   \param [in] _entry_handle_id - value of current entry_id,
 *          this handle will be reused
 * \par INDIRECT INPUT:
 * \par INDIRECT OUTPUT:
 *
 * Example:
 *
 * \code{.c}
 * int my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *   uint32 entry_handle_id;
 *
 *   SHR_FUNC_INIT_VARS(unit);
 *   DBAL_FUNC_INIT_VARS(unit);
 *
 *   DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id);
 *
 *   ****** perform table actions and commit
 *
 *   ***** reuse the table handle / allocate it to another table
 *   DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id);
 *
 * exit:
 *   DBAL_FUNC_FREE_VARS;
 *   SHR_FUNC_EXIT;
 * }
 * \endcode
 *
 */
#define DBAL_HANDLE_CLEAR(_unit, _table_id, _entry_handle_id)                         \
    dbal_entry_handle_clear_macro(_unit, _table_id, _entry_handle_id);

/**
 * \brief DBAL HANDLE FREE MACRO declarations.
 *
 * This macro is used to free a dbal handle.
 * Calling this macro frees the dbal handle and updates the
 * local handles DB. Usage of this macro is optional since all
 * occupied handles are free at function exit.
 *
 * \par DIRECT INPUT:
 *   \param [in] _unit -
 *     Value of 'unit' input parameter at entry. If there is no
 *     relevant unit to report, use NO_UNIT.
 *   \param [in] _entry_handle_id - value of current entry_id
 * \par INDIRECT INPUT:
 * \par INDIRECT OUTPUT:
 *
 * Example:
 *
 * \code{.c}
 * int my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *   uint32 entry_handle_id;
 *
 *   SHR_FUNC_INIT_VARS(unit);
 *   DBAL_FUNC_INIT_VARS(unit);
 *
 *   DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id);
 *   DBAL_HANDLE_FREE(unit, entry_handle_id);
 *
 * exit:
 *   DBAL_FUNC_FREE_VARS;
 *   SHR_FUNC_EXIT;
 * }
 * \endcode
 *
 */
#define DBAL_HANDLE_FREE(_unit, _entry_handle_id)                                     \
    SHR_SET_CURRENT_ERR(dbal_entry_handle_release_macro(_unit, _entry_handle_id));    \
do                                                                                    \
{                                                                                     \
    int _handle_index;                                                                \
    for (_handle_index=0; _handle_index<DBAL_FUNC_NOF_ENTRY_HANDLES; _handle_index++) \
    {                                                                                 \
        if (_dbal_handles[_handle_index] == _entry_handle_id)                         \
        {                                                                             \
            _dbal_handles[_handle_index] = DBAL_SW_NOF_ENTRY_HANDLES;                 \
            _nof_local_handles--;                                                     \
            break;                                                                    \
        }                                                                             \
    }                                                                                 \
} while(0)

/**
 * \brief DBAL FREE FUNC VARS MACRO declarations.
 *
 * This macro is used to free dbal handles db allocated at the
 * enty of the function. it free all dbal handles used in the
 * function scope.
 * This MACRO has 1 extra } at the end for compilation
 * reason as variable defined in the DBAL_FUNC_INIT_VARS after
 * code. the corrosponding { is located at the beginning of
 * DBAL_FUNC_INIT_VARS macro.
 *
 * \par DIRECT INPUT:
 * \par INDIRECT INPUT:
 *    \param [in] _dbal_handles - DB for holding
 *                dbal handles used in this function
 *    \param [in] _free_handle_index - loop index for free
 *          section, used alsofor validating the free macro exist,
 *          otherwise a variable not used warning will apear in
 *          compilation
 *    \param [in] _nof_local_handles - indicate the number of dbal hables used
 *          in this fuction
 *
 * \par INDIRECT OUTPUT:
 *    \param [in] _dbal_handles - DB for holding
 *                dbal handles used in this function
 *    \param [in] _free_handle_index - loop index for free
 *          section, used alsofor validating the free macro exist,
 *          otherwise a variable not used warning will apear in
 *          compilation
 *    \param [in] _nof_local_handles - indicate the number of dbal hables used
 *          in this fuction
 *
 * Example:
 *
 * \code{.c}
 * int my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *   uint32 entry_handle_id;
 *
 *   SHR_FUNC_INIT_VARS(unit);
 *   DBAL_FUNC_INIT_VARS(unit);
 *
 *   DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id);
 *   DBAL_HANDLE_FREE(unit, entry_handle_id);
 *
 * exit:
 *   DBAL_FUNC_FREE_VARS;
 *   SHR_FUNC_EXIT;
 * }
 * \endcode
 *
 */
#define DBAL_FUNC_FREE_VARS                                                                               \
    do                                                                                                    \
    {                                                                                                     \
        for (_free_handle_index=0; _free_handle_index<DBAL_FUNC_NOF_ENTRY_HANDLES; _free_handle_index++)  \
        {                                                                                                 \
            if (_dbal_handles[_free_handle_index] != DBAL_SW_NOF_ENTRY_HANDLES)                           \
            {                                                                                             \
                dbal_entry_handle_release_macro(_unit, _dbal_handles[_free_handle_index]);                \
                _nof_local_handles--;                                                                     \
            }                                                                                             \
        }                                                                                                 \
    }while (0);                                                                                           \
}

/*********************************************** DBAL init de-init APIs *********************************************/
/**
 * \brief
 * perform init to module. set the init flags and severity levels
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_init(
    int unit);

/**
 * \brief
 * perform deinit to module. set the init flags and severity
 * levels
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_deinit(
    int unit);

/**
 * \brief
 * this API is called when the device init done.
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_device_init_done(
    int unit);

/**
 * \brief
 * perform fields init to module. read fields xmls and fill DB
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_field_types_init(
    int unit);

/**
 * \brief
 * perform fields deinit to module.
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_field_types_deinit(
    int unit);

/**
 * \brief
 * perform tables init to module. read tables xmls and fill DB
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_init(
    int unit);

/**
 * \brief
 * create a DBAL table, only applicable for dynamic tables
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] access_method - how to access to HW example: SW,HL,PEMLA...
 *   \param [in] table_type - TCAM,DIRECT,LPM
 *   \param [in] core_mode - SBC,DPC
 *   \param [in] nof_key_fields - NOF key fields in the table.
 *   \param [in] keys_info - key fields, no duplications.
 *               The key fields are ordered from the MSB to the LSB (index 0 is placed at the MSB) for MDB and KBP,
 *               and from LSB to MSB for hard logic (but dynamic table for hard logic is not supported at the moment).
 *   \param [in] nof_result_fields - NOF key fields in the table.
 *   \param [in] results_info - Result fields, no duplications
 *               The result fields are ordered from the MSB to the LSB (index 0 is placed at the MSB) for MDB and KBP,
 *               and from LSB to MSB for hard logic (but dynamic table for hard logic is not supported at the moment).
 *   \param [in] table_name - table name, in case of NULL
 *               the name DYNAMIC_<table_id> is set.
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
 * read dynamic result types form sw state, only applicable for
 * PPMC table
 *   \param [in] unit
 *   \param [in] table_id - the table to restore
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_result_type_restore(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_tables_dynamic_result_type_delete(
    int unit,
    dbal_tables_e table_id,
    int result_type_index);

shr_error_e dbal_tables_dynamic_result_type_destroy_all(
    int unit,
    dbal_tables_e table_id);

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
shr_error_e
dbal_tables_table_restore(
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
 * \brief
 * this API returns table access info according to the access layer type
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id - the table to update
 *   \param [in] access_method - access method to update DBAL_ACCESS_METHOD_XXX
 *   \param [in] access_info - a pointer to the access info requested should be one of the following structs:
 *          dbal_table_mdb_access_info_t \ dbal_table_sw_access_info_t
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_table_access_info_get(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e access_method,
    void *access_info);

/**
 * \brief
 * perform tables deinit to module.
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_deinit(
    int unit);

/**
 * \brief
 * Init the Pemla access DB
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_pemla_init(
    int unit);

/**
 * \brief
 * Deinit the Pemla access DB
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_pemla_deinit(
    int unit);

/**
 * \brief
 * Init the SW access DB
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_sw_access_init(
    int unit);

/**
 * \brief
 * Deinit the SW access DB
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_sw_access_deinit(
    int unit);

/**
 * \brief
 * Init the MDB access DB, and Physical tables
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_mdb_init(
    int unit);

/**
 * \brief
 * Deinit the MDB access DB and Physical tables
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_mdb_deinit(
    int unit);

/**************************************************DBAL APIs (from:dbal_api.c) *********************************************/
/**
 *\brief return the severity of the logger for DBAL.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] logger_info -  pointer to the logger information to return\n
 *  \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the sevirity pointer \n
 */
shr_error_e dbal_logger_info_get(
    int unit,
    CONST dbal_logger_info_t ** logger_info);

/**
 *\brief sets the table of the logger for DBAL. only one table can be set, when setting the table the
 * severity of API and access automaticly updated to verbose, to unlock the table set table_id=DBAL_NOF_TABLES
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the relevant table to log for
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_logger_table_lock(
    int unit,
    dbal_tables_e table_id);

/**
 *\brief allocate SW entry handle from pool. this handle  is used to perform table actions.
 *  should be used only by DBAL_HANDLE_ALLOC macro
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the requested table to allocate handle for\n
 *   \param [in] entry_handle_id -  pointer to the returned handle ID\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the handle pointer with the ID allocated\n
 */
extern shr_error_e dbal_entry_handle_take_macro(
    int unit,
    dbal_tables_e table_id,
    uint32 *entry_handle_id);

/**
 *\brief clear SW entry handle. this function reuses a SW entry
 *       handle allready allocated. calling this function with
 *       the same table_id will reset the handle to it's
 *       initialy values. calling this function with different
 *       table_id, will assign the handle to the new table.
 *   should be used only by DBAL_HANDLE_CLEAR macro
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the requested table to reallocate
 *          the handle for\n
 *   \param [in] entry_handle_id -  pointer to the returned handle ID\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 */
extern shr_error_e dbal_entry_handle_clear_macro(
    int unit,
    dbal_tables_e table_id,
    uint32 entry_handle_id);

/**
 *\brief return handle to pool. must be called after taking an handle.
 *          entry APIs (commit/get/delete) will return the handle automaticlly (see also DBAL_COMMIT_KEEP_HANDLE)
 *   should be used only by DBAL_HANDLE_FREE macro
 *
 * \par DIRECT INPUT:
 *   \param [in] unit -  \n
 *   \param [in] entry_handle_id -  handle ID to return\n
 *
 *   \return
 *      Non-zero(shr_error_e) in case of an error
 */
extern shr_error_e dbal_entry_handle_release_macro(
    int unit,
    uint32 entry_handle_id);

/***********SET APIs ***********/

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_key_field8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 field_val);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_key_field16_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 field_val);

/**
 *\brief sets a field value in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_key_field32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val);

/**
 *\brief sets a field value in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry. The number of bits taken from the array is according to the
 * field definition in the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_key_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val);

/**
 *\brief sets a field value in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry. The number of bits taken from the array is according to the
 * field definition in the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_key_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val);

/**
 *\brief
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  value_type - the type of the value user want to set to the field\n
 */
void dbal_entry_key_field_predefine_value_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_field_predefine_value_type_e value_type);

/**
 *\brief
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  inst_id - the istance ID, when no instances use INST_SINGLE\n
 *   \param [in]  value_type - the type of the value user want to set to the field\n
 */
void dbal_entry_value_field_predefine_value_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    dbal_field_predefine_value_type_e value_type);

/**
 *\brief sets a key field in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry. this API is used to set range of keys with the same value.
 * (min value=3 max value=6 will update entries 3,4,5,6)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  min_val - the min value to set for the field, for all the range of the field use DBAL_RANGE_ALL\n
 *   \param [in]  max_val - the last value to set for the field, to set the max field value set DBAL_RANGE_ALL\n
 */
void dbal_entry_key_field8_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 min_val,
    uint8 max_val);

/**
 *\brief sets a key field in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry. this API is used to set range of keys with the same value.
 * (min value=3 max value=6 will update entries 3,4,5,6)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  min_val - the min value to set for the field, for all the range of the field use DBAL_RANGE_ALL\n
 *   \param [in]  max_val - the last value to set for the field, to set the max field value set DBAL_RANGE_ALL\n
 */
void dbal_entry_key_field16_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 min_val,
    uint16 max_val);

/**
 *\brief sets a key field in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry. this API is used to set range of keys with the same value.
 * (min value=3 max value=6 will update entries 3,4,5,6)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  min_val - the min value to set for the field, for all the range of the field use DBAL_RANGE_ALL\n
 *   \param [in]  max_val - the last value to set for the field, to set the max field value set DBAL_RANGE_ALL\n
 */
void dbal_entry_key_field32_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 min_val,
    uint32 max_val);

/**
 *\brief sets a field value in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry. The number of bits taken from the array is according to the
 * field definition in the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_key_field64_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint64 field_val);

/*
 * masked
 */
/**
 *\brief sets a field value in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry. The number of bits taken from the field is according to the
 * field size definitions in the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 *   \param [in]  field_mask - the mask to set \n
 */
void dbal_entry_key_field8_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 field_val,
    uint8 field_mask);

/**
 *\brief sets a field value in the entry. No error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 *   \param [in]  field_mask - the mask to set \n
 */
void dbal_entry_key_field16_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 field_val,
    uint16 field_mask);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 *   \param [in]  field_mask - the mask to set \n
 */
void dbal_entry_key_field32_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val,
    uint32 field_mask);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 *   \param [in]  field_mask - the mask to set \n
 */
void dbal_entry_key_field_arr8_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val,
    uint8 *field_mask);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 *   \param [in]  field_mask - the mask to set \n
 */
void dbal_entry_key_field_arr32_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set \n
 *   \param [in]  field_mask - the mask to set \n
 */
void dbal_entry_key_field64_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint64 field_val,
    uint64 field_mask);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  inst_id   - for multiple instances fields,
 *  the instance number use INST_SINGLE when no multiple
 *  instances in table. use INST_ALL to configure all fields
 *  instances \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_value_field8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 field_val);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_value_field16_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 field_val);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_value_field32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 field_val);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_value_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_value_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 *\brief sets a field value in the entry. no error is returned from this function, all errors related to this field
 * will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the value to set \n
 */
void dbal_entry_value_field64_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 field_val);

/** value field request APIs */

/**
 *\brief used to get field info from the entry. in this API only setting the pointer that will used to retrive the value.
 * the actual value is returned only after calling dbal_entry_get()
 * no error is returned from this function, all errors related to this field will be received when commit/get/delete the entry.
 * The array size should be at least the size of the requested field as defined in the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate\n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in] field_val - the pointer that will be returned the value of the requested field \n
 */
void dbal_value_field8_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);

/**
 *\brief used to get field info from the entry. in this API only setting the pointer that will used to retrive the value.
 * the actual value is returned only after calling dbal_entry_get()
 * no error is returned from this function, all errors related to this field will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate\n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in] field_val - the pointer that will be returned the value of the requested field \n
 */
void dbal_value_field16_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 *field_val);

/**
 *\brief used to get field info from the entry. in this API only setting the pointer that will used to retrive the value.
 * the actual value is returned only after calling dbal_entry_get()
 * no error is returned from this function, all errors related to this field will be received when commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate\n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in] field_val - the pointer that will be returned the value of the requested field \n
 */
void dbal_value_field32_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 *\brief used to get field info from the entry. in this API only setting the pointer that will used to retrive the value.
 * the actual value is returned only after calling dbal_entry_get()
 * no error is returned from this function, all errors related to this field will be received when commit/get/delete the entry.
 * The array size should be at least the size of the requested field as defined in the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate\n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in] field_val - the pointer that will be returned the value of the requested field \n
 */
void dbal_value_field_arr8_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);

/**
 *\brief used to get field info from the entry. in this API only setting the pointer that will used to retrive the value.
 * the actual value is returned only after calling dbal_entry_get()
 * no error is returned from this function, all errors related to this field will be received when commit/get/delete the entry.
 * The array size should be at least the size of the requested field as defined in the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the requested field to relate\n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in] field_val - the pointer that will be returned the value of the requested field \n
 */
void dbal_value_field_arr32_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 *\brief used to get field info from the entry. in this API only setting the pointer that will used to retrive the value.
 * the actual value is returned only after calling dbal_entry_get()
 * no error is returned from this function, all errors related to this field will be received when commit/get/delete the entry.
 * The array size should be at least the size of the requested field as defined in the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the requested field to relate\n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in] field_val - the pointer that will be returned the value of the requested field\n
 */
void dbal_value_field64_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 *field_val);

/**
 *\brief returns the assosiated table ID to the entry handle.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [out] table_id  - pointer to the table ID related to the handle\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e
dbal_entry_handle_related_table_get(
    int unit,
    uint32 entry_handle_id,
	dbal_tables_e *table_id);

/**
 * \brief returns key fields from entry handle this API is used when using iterator to retireve the key fields.
 *
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_key_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val);

/**
 * \brief returns key fields from entry handle this API is used when using iterator to retireve the key fields.
 *
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_key_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val);

/**
 * \brief returns key fields and its mask from entry handle this
 *        API is used when using iterator to retireve the key
 *        fields.
 *
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 *   \param [in]  field_mask - the pointer that will be returned
 *          the mask\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */

shr_error_e dbal_entry_handle_key_field_arr8_masked_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val,
    uint8 *field_mask);

/**
 * \brief returns key fields and its mask from entry handle this
 *        API is used when using iterator to retireve the key
 *        fields.
 *
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 *   \param [in]  field_mask - the pointer that will be returned
 *          the mask\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_key_field_arr32_masked_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL_FIELDS.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \par DIRECT OUTPUT:
 *   _SHR_E_NOT_FOUND in case the value field is not in used by the entry (if it has valid bit, and it is set to zero).
 *   Other non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_value_field8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL_FIELDS.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \par DIRECT OUTPUT:
 *   _SHR_E_NOT_FOUND in case the value field is not in used by the entry (if it has valid bit, and it is set to zero).
 *   Other non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_value_field16_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 *field_val);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \par DIRECT OUTPUT:
 *   _SHR_E_NOT_FOUND in case the value field is not in used by the entry (if it has valid bit, and it is set to zero).
 *   Other non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_value_field32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \par DIRECT OUTPUT:
 *   _SHR_E_NOT_FOUND in case the value field is not in used by the entry (if it has valid bit, and it is set to zero).
 *   Other non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_value_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \par DIRECT OUTPUT:
 *   _SHR_E_NOT_FOUND in case the value field is not in used by the entry (if it has valid bit, and it is set to zero).
 *   Other non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_value_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 * \brief returns value fields from entry handle when using entry_get with flag DBAL_GET_ALL.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - the field to relate \n
 *   \param [in]  inst_id - for multiple instances fields, the instance number use INST_SINGLE when
    no multiple instances in table. use INST_ALL to configure all fields instances \n
 *   \param [in]  field_val - the pointer that will be returned the value\n
 * \par DIRECT OUTPUT:
 *   _SHR_E_NOT_FOUND in case the value field is not in used by the entry (if it has valid bit, and it is set to zero).
 *   Other non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_value_field64_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 *field_val);

/**
 * \brief set the access ID on the entry. this is used for TCAM tables only. this operation is only SW
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  entry_access_id - the access ID \n
 *   \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_access_id_set(
    int unit,
    uint32 entry_handle_id,
    uint32 entry_access_id);

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
shr_error_e dbal_entry_handle_access_id_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id);

/**
 * \brief set the entry attribute value.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  attr_type - attribute type \n
 *   \param [in]  attr_val - attribute value to set\n
 *   \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_attribute_set(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 attr_val);

/**
 * \brief get the entry attribute value.
 * This function is used within an iterator, when settin an
 * iterator rule/action for hitbit the call to
 * dbal_entry_attribute_request is done without allocatin a
 * pointer for the returned value.
 * The hitbit result is kept within the handle, and this API is
 * used to extract the value in to the allocated pointer within
 * the iterator loop.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  attr_type - attribute type \n
 *   \param [in]  attr_val - ptr to attribute value\n
 *   \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_attribute_get(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 *attr_val);

/**
 * \brief get the entry attribute value.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]   attr_type - attribute type \n
 *   \param [out]  attr_val - pointer for returning the attribute value\n
 *   \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_attribute_request(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 *attr_val);

/**
 * \brief get entry from HW according to flags, update the pointers that was set to the handle by
 *        dbal_value_field_XXX_reques(). returns the information of the requested fields only.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  flags - requested flag, default value:DBAL_COMMIT\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_get(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags);

/**
 * \brief returns an entry that equal to the default entry, from this entry user can retrive each entry value. this API
 *  	  is not calling the HW.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  result_type - the relevant result type, if no result type use 0. \n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */

shr_error_e dbal_entry_default_get(
    int unit,
    uint32 entry_handle_id,
	int result_type);

/**
 * \brief set the entry to HW according to flags.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  flags - requested flag, default value: DBAL_COMMIT\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_commit(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags);
/**
 * \brief clear the entry from HW according to flags.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  flags - requested flag, default value: DBAL_COMMIT\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_clear(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags);

/**
 * \brief This API gets a key an payload buffer and update an allocated handle with those values.
 * The status of the handle after that API should be exactly like the given entry was read from HW.
 * It supports table with result type for MDB tables only.this
 * function doesn't handle the hitbit cmd/result
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  key_buffer - key value buffer (core id is not part of the buffer). If null, will be set with zero value\n
 *   \param [in]  core_id - core id, will be used in DPC tables only. If no specific core should be set, use DBAL_CORE_ALL\n
 *   \param [in]  payload_size - size of given payload buffer in bits\n
 *   \param [in]  payload_buffer - assuming valid info is MSB aligned, to bit indicate by payload size, if null will be set with zero value (the size won't be used)\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_handle_key_payload_update(
    int unit,
    uint32 entry_handle_id,
    uint32 *key_buffer,
    int core_id,
    int payload_size,
    uint32 *payload_buffer);

/**
 * \brief get the entry access ID that corresponds to the key in handle, use flag DBAL_COMMIT. When flag this operation
 *        returns the access_id according to the given Key, it can be HW or SW operation used mostly for TCAM tables.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  entry_access_id - entry HW ID to the related to key added\n
 *   \param [in]  flags - requested flag, default value: DBAL_COMMIT\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_access_id_by_key_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id,
    dbal_entry_action_flags_e flags);

/**
 * \brief delete all entries related to table.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to clear\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_table_clear(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief return number of result types related to a table.
 *
 *   \param [in] unit
 *   \param [in] table_id - value of requested table_id
 *   \param [out] nof_res_type - number of result types of the given table
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_table_nof_res_type_get(
    int unit,
    dbal_tables_e table_id,
    int *nof_res_type);

/**
 * \brief get table capacity.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table id\n
 *   \param [out] max_capacity - aximum capacity of the table\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_capacity_get(
    int unit,
    dbal_tables_e table_id,
    int *max_capacity);

/**
 *\brief init the iterator, must be called first. the iterator allow to run over all entries related to a specific table
 *
 *IMPORTANT NOTE: when using the iterator no other operation allowed on the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit          -\n
 *   \param [in]  entry_handle_id   - value of current entry_id
 *   \param [in]  mode          - which entries are valid for the iterator\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the iterator_info pointer \n
 */
shr_error_e dbal_iterator_init(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_mode_e mode);

/**
 * \brief setting key condition on iterator, to filter the entries that the iterator find.
 *        Condition can be set per field or for entire key value (see description of field_id parameter)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - Field ID related to condition, in case OF DBAL_FIELD_EMPTY, condition is on entire key value\n
 *   \param [in]  condition - the condition type\n
 *   \param [in]  field_val - condition value, irrelevant for conditions of type IS_ODD, IS_EVEN\n*
 *   \param [in]  field_mask - relevant for conditions IS_EQUAL, IS_NOT_EQUAL\n*
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_key_field_arr8_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint8 *field_val,
    uint8 *field_mask);

/**
 * \brief setting key condition on iterator, to filter the entries that the iterator find.
 *        Condition can be set per field or for entire key value (see description of field_id parameter)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - Field ID related to condition, in case OF DBAL_FIELD_EMPTY, condition is on entire key value\n
 *   \param [in]  condition - the condition type\n
 *   \param [in]  field_val - condition value, irrelevant for conditions of type IS_ODD, IS_EVEN\n*
 *   \param [in]  field_mask - relevant for conditions IS_EQUAL, IS_NOT_EQUAL, if NULL ignored\n*
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_key_field_arr32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 *field_val,
    uint32 *field_mask);

/**
 * \brief setting key condition on iterator, to filter the entries that the iterator find.
 *        Condition can be set per field or for entire key value (see description of field_id parameter)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - Field ID related to condition, in case OF DBAL_FIELD_EMPTY, condition is on entire key value\n
 *   \param [in]  condition - the condition type\n
 *   \param [in]  field_val - condition value, irrelevant for conditions of type IS_ODD, IS_EVEN\n*
 *   \param [in]  field_mask - relevant for conditions IS_EQUAL, IS_NOT_EQUAL if equal = 0 ignored \n*
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e
dbal_iterator_key_field32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 field_val,
    uint32 field_mask);
/**
 * \brief setting result condition on iterator, to filter the entries that the iterator find.
 *        Condition can be set per field or for entire result value (see description of field_id parameter)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - Field ID related to condition, in case OF DBAL_FIELD_EMPTY, condition is on entire key value\n
 *   \param [in]  inst_idx - Instance id for field id\n
 *   \param [in]  condition - the condition type\n
 *   \param [in]  field_val - condition value, irrelevant for conditions of type IS_ODD, IS_EVEN\n*
 *   \param [in]  field_mask - relevant for conditions IS_EQUAL, IS_NOT_EQUAL\n*
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_value_field_arr8_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint8 *field_val,
    uint8 *field_mask);

/**
 * \brief setting result condition on iterator, to filter the entries that the iterator find.
 *        Condition can be set per field or for entire result value (see description of field_id parameter)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  field_id - Field ID related to condition, in case OF DBAL_FIELD_EMPTY, condition is on entire key value\n
 *   \param [in]  inst_idx - Instance id for field id\n
 *   \param [in]  condition - the condition type\n
 *   \param [in]  field_val - condition value, irrelevant for conditions of type IS_ODD, IS_EVEN\n*
 *   \param [in]  field_mask - relevant for conditions IS_EQUAL, IS_NOT_EQUAL\n*
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_value_field_arr32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint32 *field_val,
    uint32 *field_mask);

shr_error_e
dbal_iterator_value_field32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint32 field_val,
    uint32 field_mask);

/**
 * \brief setting result condition on iterator, to filter the entries that the iterator find.
 *        Condition can be set per attribute type
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  attr_type - Attribute type\n
 *   \param [in]  attr_val - attribute value
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_attribute_rule_add(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 attr_val);

/**
 * \brief setting attribute action on iterator, attribute
 * actions are actions which affect specific attribute
 * operation
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  attr_type - the attribute type\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_attribute_action_add(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type);

/**
 * \brief setting field action on iterator, field actions are actions which affect specific field values.
 * Action can be set per field or for entire result value (see description of field_id parameter)
 * This API handling values which are holds as uint8 buffers
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  action_type - the action type\n
 *   \param [in]  field_id - Field ID related to action, in case OF DBAL_FIELD_EMPTY, action is on entire result value\n
 *   \param [in]  inst_idx - Instance id for field id\n
 *   \param [in]  field_val - action value, \n*
 *   \param [in]  field_mask - action_mask\n*
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_field_arr8_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type,
    dbal_fields_e field_id,
    int inst_idx,
    uint8 *field_val,
    uint8 *field_mask);

/**
 * \brief setting field action on iterator, field actions are actions which affect specific field values.
 * Action can be set per field or for entire result value (see description of field_id parameter)
 * This API handling values which are holds as uint32 buffers
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  action_type - the action type\n
 *   \param [in]  field_id - Field ID related to action, in case OF DBAL_FIELD_EMPTY, action is on entire result value\n
 *   \param [in]  inst_idx - Instance id for field id\n
 *   \param [in]  field_val - action value, \n*
 *   \param [in]  field_mask - action_mask\n*
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_field_arr32_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type,
    dbal_fields_e field_id,
    int inst_idx,
    uint32 *field_val,
    uint32 *field_mask);

/**
 * \brief setting entry action on iterator, entry actions are actions that do not use values to update.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  action_type - the action type\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_entry_action_add(
    int unit,
    uint32 entry_handle_id,
    dbal_iterator_action_types_e action_type);

/** \brief - Description will be added with implementation */
shr_error_e dbal_iterator_action_commit(
    int unit,
    uint32 entry_handle_id);

/**
 *\brief return the next entry of the table.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in] entry_handle_id - handle that used by the iterator
 *   \param [in] is_end - indicates if the iterator reached the end
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the iterator_info pointer with the updated info (next entry found)\n
 */
shr_error_e dbal_iterator_get_next(
    int unit,
    uint32 entry_handle_id,
    int *is_end);

/**
 *\brief destroy the iterator, must be called when finishing to work with the iterator.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - handle that used by the iterator \n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_destroy_macro(
    int unit,
    uint32 entry_handle_id);

/**
 *\brief Return the entries count of a valid iterator which associated with entry_handle_id
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - handle that used by the iterator \n
 *   \param [out]  entries_counter - entries count of hte iterator \n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_iterator_entries_count_get(
    int unit,
    uint32 entry_handle_id,
    int *entries_counter);

/*************************Fields operations (from: dbal fields.c) *************************/

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

shr_error_e dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info);

/**
 *\brief returns the decoded value and sub-field according to the parent field and the encoded value received.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] field_id -  parent field ID\n
 *   \param [in] orig_val -  field HW value (example, in destination the value encoded as port/fec...)\n
 *   \param [in] sub_field_id -  a pointer to the requested sub field ID that fits to the encoding, if no field was
 *          found it will return DBAL_FIELD_EMPTY\n
 *   \param [in] sub_field_val -  a pointer to the requested the value decoded, the value will be updated according to
 *          the size of the field. the value of the pointer has to be zeroed (or any other value that the user
 *          implicitly decided to put in the buffer)\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error, incase that the field_id is not parent fields will return _SHR_E_PARAM
 * \par INDIRECT OUTPUT
 *       filled the sub_field_id and sub_field_value pointers \n
 */
shr_error_e dbal_fields_parent_field_decode(
    int unit,
    dbal_fields_e field_id,
    uint32 *orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val);

/** Same as above without returning err */
shr_error_e dbal_fields_parent_field_decode_no_err(
    int unit,
    dbal_fields_e field_id,
    uint32 *orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val);

/**
 *\brief returns the decoded value and sub-field according to the encoded value added. this is used for fields that are
 *       not bigger than 32 bits.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] field_id -  parent field ID\n
 *   \param [in] orig_val -  field HW value (example, in destination the value encoded as port/fec...)\n
 *   \param [in] sub_field_id -  a pointer to the requested sub field ID that feets to the encoding\n
 *   \param [in] sub_field_val -  a pointer to the requested the value decoded\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error  incase that the field_id is not parent fields will return _SHR_E_PARAM
 * \par INDIRECT OUTPUT
 *       filled the sub_field_id and sub_field_value pointers \n
 */
shr_error_e dbal_fields_uint32_sub_field_info_get(
    int unit,
    dbal_fields_e field_id,
    uint32 orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val);

/**
 * \brief - Return the max size in bits of the field type
 *        assiciated with the field_id
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] field_id -
 *   \param [in] field_size - field size in bits
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
shr_error_e dbal_fields_max_size_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *field_size);

/**
 * \brief - set a sub struct field value into full field value if the field is encoded the value that will be added to
 *        the struct will be the encoded value (for example in ENUM will add the HW value), if the field is subfield of
 *        the parent field will use the encoding between the subfield to the parent field
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] field_id - The field ID of the struct
 *   \param [in] field_in_struct_id - sub struct field id
 *   \param [in] field_in_struct_val- sub struct field value
 *          (input buffer)
 *   \param [in] full_field_val - full field value (output
 *          buffer)
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
shr_error_e dbal_fields_struct_field_encode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *full_field_val);

shr_error_e dbal_field_arr_prefix_decode(
    int unit,
    dbal_table_field_info_t * table_field,
    uint32 field_val,
    uint32 *ret_field_val);

/**
 * \brief - set a sub struct field value into full field value if the field is encoded the value that will be added to
 *        the struct will be the encoded value (for example in ENUM will add the HW value), if the field is subfield of
 *        the parent field will use the encoding between the subfield to the parent field
 *
 *   \param [in] unit - unit
 *   \param [in] field_id - The field ID of the struct
 *   \param [in] field_in_struct_id - sub struct field id
 *   \param [in] field_in_struct_val- sub struct field value
 *          (input buffer)
 *   \param [in] field_in_struct_mask- sub struct field mask
 *          (input buffer)
 *   \param [in] full_field_val - full field value (output
 *          buffer)
 *   \param [in] full_field_mask - full field mask (output
 *          buffer)
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_fields_struct_field_encode_masked(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *field_in_struct_mask,
    uint32 *full_field_val,
    uint32 *full_field_mask);
/**
 * \brief - read a sub struct field value from full field value
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] field_id - full field id
 *   \param [in] field_in_struct_id - sub struct field id
 *   \param [in] field_in_struct_val- sub struct field value
 *          (output buffer)
 *   \param [in] full_field_val - full field value (input
 *          buffer)
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
shr_error_e dbal_fields_struct_field_decode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *full_field_val);

/**
 * \brief - read a sub struct field value from full field and
 *        mask value
 *
 *   \param [in] unit - unit
 *   \param [in] field_id - The field ID of the struct
 *   \param [in] field_in_struct_id - sub struct field id
 *   \param [in] field_in_struct_val- sub struct field value
 *          (input buffer)
 *   \param [in] field_in_struct_mask- sub struct field mask
 *          (input buffer)
 *   \param [in] full_field_val - full field value (output
 *          buffer)
 *   \param [in] full_field_mask - full field mask (output
 *          buffer)
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_fields_struct_field_decode_masked(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *field_in_struct_mask,
    uint32 *full_field_val,
    uint32 *full_field_mask);

/**
 * \brief Return encoded parent field value, given child field.
 *        Encoding is applicable to field size up to 32 bit.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] parent_field_id - field id to be encoded\n
 *   \param [in] sub_field_id -  sub field id\n
 *   \param [in] sub_field_val - sub field value to be encoded
 *          and encapsulated into parent_field_val\n
 *   \param [in] parent_field_val - pointer to the parent field
 *          id to be filled with encoded value\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e dbal_fields_parent_field32_value_set(
    int unit,
    dbal_fields_e parent_field_id,
    dbal_fields_e sub_field_id,
    uint32 *sub_field_val,
    uint32 *parent_field_val);

/**
 * \brief Return the HW value of the logical value of the field enum.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] field_id - field of the relevant enum\n
 *   \param [in] enum_value -  logical value of the enum\n
 *   \param [in] hw_value - pointer to the HW value that will be updated
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the hw_value pointer \n
 */
shr_error_e dbal_fields_enum_hw_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 enum_value,
    uint32 *hw_value);

/**
 * \brief Return the logical value of the HW value of the field enum.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] field_id - field of the relevant enum\n
 *   \param [in] hw_value -  HW value of the enum\n
 *   \param [in] enum_value - pointer to the logical value that
 *          will be updated (sw value)
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the hw_value pointer \n
 */
shr_error_e dbal_fields_enum_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 hw_value,
    uint32 *enum_value);

/**
* \brief
*   Retuns is_allocator == 1 if the field is allocator.
*   \param [in] unit - Relevant unit.
*   \param [in] field_id - Relevant dbal field.
*   \param [out] is_allocator -
*       true if the field is allocator.
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

shr_error_e dbal_fields_is_allocator_get(
    int unit,
    dbal_fields_e field_id,
    uint8 *is_allocator);

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
/**
* \brief
*   Retuns the struct field size
*   \param [in] unit - Relevant unit.
*   \param [in] field_type - Structured dbal field type.
*   \param [in] field_id - Field inside field type.
*   \param [out] field_size_p - pointer to field size
*
*  \return
*    \retval Zero if no error occured
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dbal_fields_max_size_get
*****************************************************/
shr_error_e dbal_field_struct_field_size_get(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_fields_e field_id,
    uint32 *field_size_p);

/**
* \brief
*   Retuns the field type associated with the field_id.
*   \param [in] unit - Relevant unit.
*   \param [in] field_id - Relevant dbal field.
*   \param [out] field_type - the associated field type.
*  \return
*    \retval Zero if no error occured
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
*****************************************************/
shr_error_e dbal_fields_field_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_defs_e * field_type);

/************************* Logical Table operations (from: dbal_tables.c) *************************/
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
 * \brief return table field info, error if field not related to table.
 * in case that the field is sub field in a table returns the
 * parent field ID info.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested fireld is key or value field\n
 *   \param [in] result_type_idx -  valid in cases of value fields are requested. indicate the result type index\n
 *   \param [in] inst_idx -  valid in cases of value fields are
 *          requested. indicate the instance idx\n
 *   \param [in] field_info -  pointer to the requested field info\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e dbal_tables_field_info_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t * field_info);

/**
 * \brief return table field size in bits, error if field not
 * related to table. in case that the field is sub field in a
 * table returns the parent field ID info.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested fireld is key or value field\n
 *   \param [in] result_type_idx -  valid in cases of value fields are requested. indicate the result type index\n
 *   \param [in] inst_idx -  valid in cases of value fields are
 *          requested. indicate the instance idx\n
 *   \param [in] field_size -  pointer to the requested size\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e dbal_tables_field_size_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_size);

/**
 * \brief return table field num of instances
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested fireld is key or value field\n
 *   \param [in] res_type -  indicate the result type\n
 *   \param [in] nof_inst -  pointer to the requested number\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */

shr_error_e dbal_tables_field_nof_instance_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type,
    int *nof_inst);

/**
 * \brief return table field max value, error if field not
 * related to table. in case that the field is sub field in a
 * table returns the parent field ID info.
 *
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested fireld is key or value field\n
 *   \param [in] result_type_idx -  valid in cases of value fields are requested. indicate the result type index\n
 *   \param [in] inst_idx -  valid in cases of value fields are
 *          requested. indicate the instance idx\n
 *   \param [in] max_value -  pointer to the requested size\n
 * \return
 *   Non-zero(shr_error_e) in case of an error, if field not found it will return error not_found without printing
 *   incase the field size is bigger than 32 bit it will return E_PARAM with print
 *   any other error it will return E_INTERNAL
 *
 */
shr_error_e dbal_tables_field_max_value_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *max_value);

/**
 * \brief Returns the index of the result type that uses a specific HW value.
 *        Assumes no two result types have the same HW value.
 *
 *   \param [in] unit
 *   \param [in] table_id -  the table to look in\n
 *   \param [in] hw_value -  The result type hw value to look for\n
 *   \param [in] result_type_idx -  Pointer to the index of the result type to be updated if a result type was found.\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error, specifically _SHR_E_NOT_FOUND if no result type was found.
 * \par INDIRECT OUTPUT
 *   Fills the result_type_idx pointer with the result type index, if a result type was found.\n
 */
shr_error_e dbal_tables_hw_value_result_type_get(
    int unit,
    dbal_tables_e table_id,
    uint32 hw_value,
    int *result_type_idx);

/**
 * \brief Returns the index of the result type that is defined with a given name.
 *        Assumes no two result types have the same name.
 *
 *   \param [in] unit
 *   \param [in] table_id -  the table to look in\n
 *   \param [in] result_type_name -  The result type name to look for\n
 *   \param [in] result_type_idx -  Pointer to the index of the result type to be updated if a result type was found.\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error, specifically _SHR_E_NOT_FOUND if no result type was found.
 * \par INDIRECT OUTPUT
 *   Fills the result_type_idx pointer with the result type index, if a result type was found.\n
 */
shr_error_e dbal_tables_result_type_by_name_get(
    int unit,
    dbal_tables_e table_id,
    char *result_type_name,
    int *result_type_idx);

/**
 * \brief this API gets the MIN/MAX/DEFAULT value of a field.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] entry_handle_id - value of current entry_id\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested field is key or value field\n
 *   \param [in] value_type - the type of the value user want to set to the field\\n
 *   \param [in] predef_value -  pointer to the requested field info, incase the field is bigger then 32 bit need to
 *  		send pointer according to the field length\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the predef_value pointer \n
 */
shr_error_e dbal_fields_predefine_value_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int is_key,
    dbal_field_predefine_value_type_e value_type,
    uint32 *predef_value);

/**
 * \brief return the field offset in HW buffer e.g. field location in HW entry
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested fireld is key or value field\n
 *   \param [in] result_type_idx -  valid in cases of value fields are requested. indicate the result type index\n
 *   \param [in] inst_idx -  valid in cases of value fields are
 *          requested. indicate the instance idx\n
 *   \param [in] field_offset -  offset in bits of field id in hw buffer (no zero padding)\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e dbal_tables_field_offset_in_hw_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_offset);

/**
 * \brief return the next dbal table that match all filters iterate over only tables with maturity level bigger then low
 *  	  and status is OK.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the first table to look for, if user want to start from the first table should set to
 *  		DBAL_TABLE_EMPTY\n
 *   \param [in] label -  the label to look for, if label is don't care use DBAL_LABEL_NONE
 *   \param [in] access_method -  the access method to look for, if it is don't care use DBAL_NOF_ACCESS_METHODS
 *   \param [in] mdb_physical_db -  the physical db to look for, valid for MDB only. if it is don't care use DBAL_PHYSICAL_TABLE_NONE
 *   \param [in] table_type -  the table type to look for, if all table types are required use DBAL_TABLE_TYPE_NONE
 *   \param [out] next_table_id -  the next table ID, if no table found returns DBAL_TABLE_EMPTY\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e
dbal_tables_next_table_get(
    int unit,
	dbal_tables_e table_id,
    dbal_labels_e label,
    dbal_access_method_e access_method,
    dbal_physical_tables_e mdb_physical_db,
	dbal_table_type_e table_type,
	dbal_tables_e 	*next_table_id);


/**
 * \brief Function return the field info according to input parameters.
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
 *   \param [in] field_info -  Field info structure pointer\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e dbal_tables_field_info_get_no_err(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t * field_info);

/**
 * \brief this API update in the TABLE DB the new MIN/MAX value of the table field.
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
 * \brief - Return the app db id that associates with the
 *        logical table. app db id  is valid for MDB tables and KBP tables only. otherwise an error will be return.
 *        Notice, not all MDB tables have valid app db id, if invalid U32_MAX will be set
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] app_db_id - the physical app db id associate
 *          with the logical table
 *   \param [in] app_db_size - the app db id size in bits associated with the logical table
 *          If the size is smaller than DBAL_APP_ID_FULL_SIZE, it means that there are masked bits in it.
 *          Masked bits are in MSB
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] app_db_id - the physical app db id associate
 *          with the logical table
 *   \param [out] app_db_size
 */
shr_error_e dbal_tables_app_db_id_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *app_db_id,
    int *app_db_size);

/**
 * \brief - Return the core mode that associates with the
 *        logical table.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [out] core_mode - the core mode associated with the
 *          logical table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_core_mode_get(
    int unit,
    dbal_tables_e table_id,
    dbal_core_mode_e * core_mode);

/**
 * \brief - Return the hw value realated to result type index in table id
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] result_type_idx - Enumeration index of the result type
 *   \param [out] hw_value - HW value associated with the result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_result_type_hw_value_get(
    int unit,
    dbal_tables_e table_id,
    int result_type_idx,
    uint32 *hw_value);

/**
 * \brief - gets the tcam_handler that associates with the
 *        logical table. if table is not MDB table an error will
 *        be return.
 *        Notice, not all MDB tables have valid tcam handler id,
 *        if invalid an error will be prompted
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] tcam_handler_id - the tcam handler id
 *          associate  with the logical table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_tcam_handler_id_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *tcam_handler_id);

/**
 * \brief - sets the tcam_handler that associates with the
 *        logical table. if table is not MDB table an error will
 *        be return.
 *        Notice, not all MDB tables have valid tcam handler id,
 *        if invalid an error will be prompted
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] tcam_handler_id - the tcam handler id
 *          associate  with the logical table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_tcam_handler_id_set(
    int unit,
    dbal_tables_e table_id,
    uint32 tcam_handler_id);

/**
 * \brief - Return the app db id that associates with the
 *        logical table. if table is not MDB table an error will
 *        be return.
 *        Notice, not all MDB tables have valid app db id, if
 *        invalid U32_MAX will be set
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] physical_tbl_index - physical table index\n
 *   \param [in] physical_table_id - the physical table id
 *          associate with the logical table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] physical_table_id - the physical table id
 *          associate with the logical table
 */
shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/**
 * \brief - Return the table type that is associated with the logical table.
 *
 * \param [in] unit -
 * \param [in] table_id - the table to look for
 * \param [out] table_type - the table type associated with the logical table
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_tables_table_type_get(
    int unit,
    dbal_tables_e table_id,
    dbal_table_type_e * table_type);

/**
 * \brief - Return the app db id that associates with the
 *        logical table. if table is not MDB table an error will
 *        be return.
 *        Notice, not all MDB tables have valid app db id, if
 *        invalid U32_MAX will be set
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] res_type_idx - Result type index in table
 *   \param [in] p_size - payload size
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] p_size - the payload size in bits of the given
 *          result type
 */
shr_error_e dbal_tables_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size);

/**
 * \brief - returna printable string that can be used to dump the field value. the printable can be enum/define name or
 *        value represented according to dbal type )ip/mac...)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] field_id - the required field ID\n
 *   \param [in] buffer_val - the field value that will be used to transform the string.
 *   \param [in] buffer_mask - optional, in case no need pul NULL
 *   \param [in] result_type_idx - Result type index in table of the relevant field.
 *   \param [in] is_key - is the field is a key field/ result field
 *   \param [out] buffer_to_print - the transformed string
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_table_field_printable_string_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 buffer_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 buffer_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int result_type_idx,
    uint8 is_key,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);

/**
 * \brief - returna printable string that can be used to dump the field value. the printable can be enum/define name or
 *        value represented according to dbal type )ip/mac...)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] key_buffer - an LSB aligned buffer holding the key value, not including core id\n
 *   \param [in] core_id - core id (0 - nof_cores() - 1) or DBAL_CORE_ALL.
 *   \param [in] payload_size - the size of the payload buffer in bits
 *   \param [in] payload_buffer - an MSB aligned buffer (data is aligned to payload_size) of the payload.
 *   \param [out] entry_print_info - structure which hold the printable information
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_table_printable_entry_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *key_buffer,
    int core_id,
    int payload_size,
    uint32 *payload_buffer,
    dbal_printable_entry_t * entry_print_info);

/**
 * \brief - Return the HW payload size of MDB tables.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] res_type_idx - Result type index in table
 *   \param [in] p_size - payload size
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_mdb_hw_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size);

/**
 * \brief - Return indication if EEDB table+result type contain a LL fields.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] res_type_idx - Result type index in table
 *   \param [in] has_link_list - returned indication
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] has_link_list - returned indication
 */
shr_error_e dbal_tables_eedb_link_list_indication_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    uint8 *has_link_list);

/**
 * \brief - For EEDB tables, given an ETPS format (result type HW value) return the payload data size.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] etps_format -result type HW value (ETPS_FORMAT)\n
 *   \param [in] data_payload_size - zise in bits of the data payload part (up to 120b)
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_mdb_eedb_payload_data_size_etps_format_get(
    int unit,
    uint32 etps_format,
    int *data_payload_size);

/**
 * \brief - For EEDB/ESEM tables, given an ETPS format (result type HW value) return the table_is and result type (index)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] etps_format -result type HW value (ETPS_FORMAT)\n
 *   \param [out] table_id - The table id which has that ETPS format (if none, will be set to DBAL_NOF_TABLES)
 *   \param [out] result_type - The result type index which is the ETPS format
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_mdb_table_id_and_result_type_from_etps_format_get(
    int unit,
    uint32 etps_format,
    dbal_tables_e * table_id,
    int *result_type);
/**
 * \brief - Find the dbal logical table according to phyDB and
 * AppDbId. Return the key and max payload sizes
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] phy_db_id - the physical DB ID to look for\n
 *   \param [in] app_db_id - the APP DB ID to look for\n
 *   \param [in] key_size - key size of logical table\n
 *   \param [in] max_pld_size - max payload size of result in
 *          the logical table\n
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_sizes_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    int *key_size,
    int *max_pld_size);

/**
 * \brief - Find the dbal logical table according to phyDB and
 * AppDbId.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] phy_db_id - the physcial DB ID to look for\n
 *   \param [in] app_db_id - the APP DB ID to look for\n
 *   \param [in] table_id - Logical table ID relate to phy
 *          parameters\n
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id);

/**
 * \brief this API return the MIN/MAX value of the table field.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested field is
 *          key or value field\n
 *   \param [in] result_type_idx -  valid in cases of value fields are requested. indicate the result type index\n
 *   \param [in] inst_idx -  valid in cases of value fields are requested. indicate the instance idx\n
 *   \param [in] value_type - the type of the value user want to set to the field\\n
 *   \param [in] predef_value -  pointer to the requested field info\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the field_info pointer \n
 */
shr_error_e dbal_tables_field_predefine_value_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_field_predefine_value_type_e value_type,
    uint32 *predef_value);

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
 * \param [in] str - the string that represents the HW value of the field.
 *
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
    char *str);

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
shr_error_e
dnx_dbal_fields_struct_field_encode(
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
    uint32* entry_access_id);

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
    char* flag_name);

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
    uint32* entry_access_id);

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
 * \brief set the bulk mode that related to the access_method and bulk module
 *
 *   \param [in] unit
 *   \param [in] access_method -  related access method\n
 *   \param [in] bulk_module -  the related bulk_module\n
 *   \param [in] collect_mode -  1 if it is collect mode (collecting entries)\n
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */

shr_error_e dbal_bulk_mode_set(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module,
    int collect_mode);

/**
 * \brief get the bulk mode that related to the access_method and bulk module
 *
 *   \param [in] unit
 *   \param [in] access_method -  related access method\n
 *   \param [in] bulk_module -  the related bulk_module\n
 *   \param [out] collect_mode -  1 if it is collect mode (collecting entries)\n
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_bulk_mode_get(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module,
    int *collect_mode);

/**
 * \brief commit the entries that where collected by the machine.
 *
 *   \param [in] unit
 *   \param [in] access_method -  related access method\n
 *   \param [in] bulk_module -  the related bulk_module\n
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_bulk_commit(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module);

/**
 * \brief clear all the entries that where added to the bulk machine.
 *
 *   \param [in] unit
 *   \param [in] access_method -  related access method\n
 *   \param [in] bulk_module -  the related bulk_module\n
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_bulk_clear(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module);

/**
 * \brief update entry's result type.\n
 * This API should be used only for multiple result type tables,\n
 * for changing the result type on the given handle.\n
 * The API sets all of the new result type entries:\n
 * For a field which exists in the current result type - take its value\n
 * For a new field - take the field's default value\n
 * For the rest of the cases - see dbal_result_type_update_mode_e documentation\n
 *
 * NOTE: TCAM tables are not supported\n
 * IMPORTANT NOTE: The API is not accessing the HW for fields values. User should get all fields on the\n
 * current res type for the given handle in order to update the correct set and default value
 *
 *   \param [in] unit
 *   \param [in] entry_handle_id - value of current entry_id\n
 *   \param [in] new_result_type - new result type to be applied to the entry\n
 *   \param [in] mode - entry's update mode policy (see dbal_result_type_update_mode_e definition)\n
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_entry_result_type_update(
    int unit,
    uint32 entry_handle_id,
    int new_result_type,
    dbal_result_type_update_mode_e mode);

#endif/*_DBAL_INCLUDED__*/
