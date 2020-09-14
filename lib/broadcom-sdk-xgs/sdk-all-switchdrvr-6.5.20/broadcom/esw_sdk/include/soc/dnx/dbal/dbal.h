/**
 *\file dbal.h
 * Main interfaces for DBAL.
 */

/*! 
 * \brief
 * The DBAL layer is used to configure the underlying device. Each table is associated with a unique name.
 * A table is composed of one or more key fields and one or more value fields.
 * 
 * At a high level, the API interacts with the following types of objects:
 * 1. Fields - two types of fields, key and values. the keys are defining where in the table the values are located, value are logical representation of part of the entry that presents in the HW.
 * 2. Handle - the SW representation of an entry
 * 3. Tables - these are the logical tables.
 * 4  Generic APIs
 *
 * A simpler mode of table operation involves the following steps:
 * a. Allocate an entry handle (SW).
 * b. Add the fields to the entry (SW).
 * c. Commit the entry (HW).
 * Nothing will be changed in the state of the system until the commit operation is performed.
 * 
 * Sequence example
 * (when rv is used some kind of error handling is required.)
 *
 * my_func()
 * {
 *      uint32 entry_handle_id;
 *      int rv;
 *
 *      DBAL_FUNC_INIT_VARS(unit);
 *
 *      rv = DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI, &entry_handle_id);
 *
 *
 *      dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_ID, 5);
 *
 *      dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STG, INST_SINGLE, 2);
 *      dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, 17);
 *
 *      rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
 *
 *      DBAL_FUNC_FREE_VARS; 
 *
 *      return rv; 
 * }
 *
 * Dynamic tables
 * dynamic tables are tables that created after DBAL init completed. the tables are created by dynamic APIs and not by XMLs (as other DBAL tables).
 * The tables attributes are limited in those cases. For the prototypes of the Dynamic tables APIs see dbal_dynamic.h
 * 
 * Error recovery
 * SDK has error recovery mechanism and utilizes the dbal as a part of its system state restoration mechanism,
 * which in case of an error restores the SW and HW state to the point prior to the beginning of current "transaction".
 * A transaction is an atomic set of sw and hw operations that either succeeds or fails as a whole (should be defined in error recoevery mechanism).
 * In order to accomplish this behavior for the HW state, the concept of a "dbal journal" is introduced, which keeps track of the dbal operations that have occurred during an error recovery transaction.
 * The dbal journal both utilizes and is utilized by the dbal APIs. In case of an error the journal is rolled-back and restores the hw state to the point prior to the beginning of the transaction.
 * 
 */

#ifndef DBAL_H_INCLUDED
#define DBAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#    error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal_external_defines.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>

/*******************
 *  DEFINES, ENUMS *
 *******************/

/** see dbal_external_defines.h */

/*************
 *  MACROS  *
 *************/

/**
 * \brief DBAL Function entry declarations.
 *
 * This macro must appear in each function using DBAL before
 * SHR_FUNC_INIT_VARS(unit), a compilation error will appear if
 * you fail to do so.
 * This macro is used to maintain a local db of dabl handles
 * used in the function. all dbal handles used in this function
 * are released at the function exit point using
 * DBAL_FUNC_FREE_VARS macro
 * This MACRO has 1 extra { at the beginning for compilation
 * reason as it defines variable after code. the corresponding }
 * is located at the DBAL_FUNC_FREE_VARS macro.
 *
 * internal variables:
 * _dbal_handles[DBAL_SW_NOF_ENTRY_HANDLES] - DB for holding
 *    dbal handles used in this function
 * _free_handle_index - loop index for free section, used also
 *    for validating the free macro exist, otherwise a variable not
 *    used warning will appear in compilation
 * _nof_local_handles - indicate the number of dbal handles used
 *    in this function
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
 *   \param [in] _unit - Value of 'unit' input parameter at entry.
 *   \param [in] _table_id - value of requested table_id
 *   \param [in] _entry_handle_id - value of current entry_id
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
if((*_entry_handle_id) < DBAL_SW_NOF_ENTRY_HANDLES)                                        \
{                                                                                       \
    do                                                                                    \
    {                                                                                     \
        int _handle_index;                                                                \
        uint32 *_entry_handle_id_ptr = _entry_handle_id;                                  \
                                                                                          \
       /** if no room for another handle in this function exit with err */                \
        if (_nof_local_handles == DBAL_FUNC_NOF_ENTRY_HANDLES)                            \
        {                                                                                 \
            dbal_entry_handle_release_internal(_unit, *_entry_handle_id_ptr);             \
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
    } while(0);                                                                          \
}

 /**
  * \brief DBAL HANDLE COPY MACRO declarations.
  *
  * This macro is used to copy a dbal handle from a given
  * valid dbal handle. The allocated handle is then stored in the
  * function's local handles DB. At the exit point of the
  * function, the handle is freed
  *
  * \par DIRECT INPUT:
  *   \param [in] _unit -
  *     Value of 'unit' input parameter at entry.
  *   \param [in] _src_handle_id - value of current entry_id, created and used before calling this function
  *   \param [in] _dst_handle_id - value of copied entry_id, allocated internally
  */
 #define DBAL_HANDLE_COPY(_unit, _src_handle_id, _dst_handle_id)                       \
     dbal_entry_handle_copy_macro(_unit, _src_handle_id, _dst_handle_id);              \
 do                                                                                    \
 {                                                                                     \
     int _handle_index;                                                                \
     uint32 *_entry_handle_id_ptr = _dst_handle_id;                                  \
                                                                                       \
    /** if no room for another handle in this function exit with err */                \
     if (_nof_local_handles == DBAL_FUNC_NOF_ENTRY_HANDLES)                            \
     {                                                                                 \
         dbal_entry_handle_release_internal(_unit, *_entry_handle_id_ptr);             \
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
    SHR_SET_CURRENT_ERR(dbal_entry_handle_release_internal(_unit, _entry_handle_id)); \
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
    if(_handle_index == DBAL_FUNC_NOF_ENTRY_HANDLES)                                  \
    {                                                                                 \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "trying to free handle that already freed\n");  \
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
                dbal_entry_handle_release_internal(_unit, _dbal_handles[_free_handle_index]);             \
                _nof_local_handles--;                                                                     \
            }                                                                                             \
        }                                                                                                 \
    }while (0);                                                                                           \
}

/****************
 *API PROTOYPES *
 ****************/

/***********************************************field APIs ***********************************************/

/**
 *\brief sets a field value in the entry, this operation is SW only. no error is returned from this function, all errors related to this field
 * will be indicated when user any HW operations such as commit/get/delete the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  field_val - the value to set, The number of bits taken from the array is according to the field definition in the table. \n
 */
void dbal_entry_key_field8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 field_val);

/**
 * \brief see dbal_entry_key_field8_set()
 */
void dbal_entry_key_field16_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 field_val);

/**
 * \brief see dbal_entry_key_field8_set()
 */
void dbal_entry_key_field32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val);

/**
 *\brief see dbal_entry_key_field8_set()
 */
void dbal_entry_key_field64_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint64 field_val);


/**
 * \brief see dbal_entry_key_field8_set()
 */
void dbal_entry_key_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val);

/**
 * \brief see dbal_entry_key_field8_set()
 */
void dbal_entry_key_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val);

/**
 *\brief sets predefine value for a key field list of predefine values can be found in dbal_field_predefine_value_type_e
 *       (such as Min/Max value), this API as the same affect as dbal_entry_key_field8_set but the value itself is
 *       predefined (not being added by the user).
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
 *\brief see dbal_entry_key_field8_set() for general Documentation. This API is used to set range of keys with the same 
 *       value. for example: min value=3 max value=6 will update entries 3,4,5,6
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
 *\brief see dbal_entry_key_field8_range_set()
 */
void dbal_entry_key_field16_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 min_val,
    uint16 max_val);

/**
 *\brief see dbal_entry_key_field8_range_set()
 */
void dbal_entry_key_field32_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 min_val,
    uint32 max_val);

/**
 *\brief see dbal_entry_key_field8_set() for general description. masked field are used for TCAM databases. for TCAM 
 *       user can use don't care bits.
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
 *\brief see dbal_entry_key_field8_masked_set()
 */
void dbal_entry_key_field16_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 field_val,
    uint16 field_mask);

/**
 *\brief see dbal_entry_key_field8_masked_set()
 */
void dbal_entry_key_field32_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val,
    uint32 field_mask);

/**
 *\brief see dbal_entry_key_field8_masked_set()
 */
void dbal_entry_key_field_arr8_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val,
    uint8 *field_mask);

/**
 *\brief see dbal_entry_key_field8_masked_set()
 */
void dbal_entry_key_field_arr32_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask);

/**
 *\brief see dbal_entry_key_field8_masked_set()
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

/**
 *\brief
 * same as dbal_entry_key_field_predefine_value_set() just for the value field.
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
 *\\brief unset a value field in the handle, it is used in rare cases that after field was set, the user want to change 
 *        the decision and ignore the operation. this is only SW operation. 
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  inst_id  - for multiple instances fields, the instance number use INST_SINGLE when no multiple
 *          instances in table. use INST_ALL to configure all fields instances \n
 */
void dbal_entry_value_field_unset(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id);


/**
 *\brief used to get field info from the entry. in this API only setting the pointer that will used to retrive the value.
 * the actual value is returned only after calling dbal_entry_get(). when using this API it should be called before
 * entry get and it signals to the access layer that the specific field is requested. No error is returned from this
 * function, all errors related to this field will be received when commit/get/delete the entry. The array size should
 * be at least the size of the requested field as defined in the table.
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
 *\brief see dbal_value_field8_request()
 */
void dbal_value_field16_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 *field_val);

/**
 *\brief see dbal_value_field8_request()
 */
void dbal_value_field32_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 *\brief see dbal_value_field8_request()
 */
void dbal_value_field_arr8_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);

/**
 *\brief see dbal_value_field8_request()
 */
void dbal_value_field_arr32_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 *\brief see dbal_value_field8_request()
 */
void dbal_value_field64_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 *field_val);


/***********************************************Entry Handle APIs ***********************************************/

/**
 *\brief returns the assosiated table ID to the entry handle. this should be called after handle was allocated, it 
 *       allows the user to identify the related table that associated to the entry handle.
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
 *\brief this API is used to get the min and max range values that were set for specific field.
 *       this API is supported only for KBP TCAM tables. prioir to this API user must call dbal_entry_get()
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  entry_handle_id - the entry handle\n
 *   \param [in]  field_id  - the field to relate \n
 *   \param [in]  min_val - the pointer that will returned the min value\n
 *   \param [in]  max_val - the pointer that will returned the max value\n
 */

shr_error_e
dbal_entry_handle_key_field_range_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *min_val,
    uint32 *max_val);

/**
 * \brief returns key field value from entry handle, this API will be used usually when using iterator over all entries 
 *        to retireve the key fields from the last iterated entry.
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
 * \brief see dbal_entry_handle_key_field_arr32_get()
 */
shr_error_e dbal_entry_handle_key_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val);

/**
 * \brief see dbal_entry_handle_key_field_arr32_get()
 */

shr_error_e dbal_entry_handle_key_field_arr8_masked_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val,
    uint8 *field_mask);

/**
 * \brief see dbal_entry_handle_key_field_arr32_get()
 */
shr_error_e dbal_entry_handle_key_field_arr32_masked_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask);

/**
 * \brief returns value fields from entry handle according to the HW state, this API should be used after performing get
 *        from the HW bu using entry_get with flag DBAL_GET_ALL_FIELDS.
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
 * \brief see dbal_entry_handle_value_field8_get()
 */
shr_error_e dbal_entry_handle_value_field16_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 *field_val);

/**
 * \brief see dbal_entry_handle_value_field8_get()
 */
shr_error_e dbal_entry_handle_value_field32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 * \brief see dbal_entry_handle_value_field8_get()
 */
shr_error_e dbal_entry_handle_value_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val);

/**
 * \brief see dbal_entry_handle_value_field8_get()
 */
shr_error_e dbal_entry_handle_value_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val);

/**
 * \brief see dbal_entry_handle_value_field8_get()
 */
shr_error_e dbal_entry_handle_value_field64_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 *field_val);

/**
 * \brief set the access ID on the entry. this is used for TCAM tables only. in TCAM tables, the entries are managed by 
 *        access ID instead of by Key. to perform an action user should set the accessID. this operation is only SW
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
 * \brief get the access ID of the entry. this is used for TCAM tables only. can by used in iterator.
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
 * \brief set the entry with an attribute value. according to table type entries can have certain type of attrubutes 
 *        such as priority/ age. in this cases user should use this API to set the attribute on the entry.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  attr_type - attribute type from DBAL_ENTRY_ATTR_XXX \n
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
 * \brief get the entry attribute value of the entry. example of usage in is within an iterator sequence, 
 * when settin an iterator rule/action for hitbit the call to dbal_entry_attribute_request is done without allocatin a 
 * pointer for the returned value. The hitbit result is kept within the handle, and this API is used to extract the 
 * value in to the allocated pointer within the iterator loop. 
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
 * \brief get the entry attribute value. this API should be called prioir to entry_get (same as value_field_request 
 *        APIs)
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
 *        dbal_value_field_XXX_reques(). returns the information of the requested fields only. when using
 *        GET_ALL_FIELDS, will return all fields related to the entry. this operation is access to HW.
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
 *  	  is SW only.
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
 * \brief set the entry to HW according to flags. this API should be called after key and value fields were set.
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
 * \brief clear the entry from HW according to flags. For direct tables the entry will be set to default values, for 
 *        non-direct tables the entry is removed from the HW.
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
 * The status of the handle after that API should be exactly like the given entry was read from HW. It supports table 
 * with result type for MDB tables only. this function doesn't handle the hitbit cmd/result 
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id - the handle to perform the action \n
 *   \param [in]  key_buffer - key value buffer (core id is not part of the buffer). If null, will be set with zero value\n
 *   \param [in]  key_mask_buffer - key mask buffer\n
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
    uint32 *key_mask_buffer,
    int core_id,
    int payload_size,
    uint32 *payload_buffer);

/**
 * \brief get the entry access ID that corresponds to the key in handle, use flag DBAL_COMMIT. When flag this operation
 *        returns the access_id according to the given Key, this is SW operation used only for TCAM tables.
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
/***********************************************iterator APIs ***********************************************/

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
 * \brief setting key rule on iterator, this rule allow to filter the entries that the iterator finds.
 *        Condition can be set per field or for complete key value (see description of field_id parameter)
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
 * \brief see dbal_iterator_key_field_arr8_rule_add() 
 */
shr_error_e dbal_iterator_key_field_arr32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 *field_val,
    uint32 *field_mask);

/**
 * \brief see dbal_iterator_key_field_arr8_rule_add() 
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
 * \brief setting value rule on iterator, this rule allow to filter the entries that the iterator finds.
 *        Condition can be set per field or for complete value of the entry (see description of field_id
 *        parameter)
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
 * \brief dbal_iterator_value_field_arr8_rule_add() 
 */
shr_error_e dbal_iterator_value_field_arr32_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_idx,
    dbal_condition_types_e condition,
    uint32 *field_val,
    uint32 *field_mask);

/**
 * \brief dbal_iterator_value_field_arr8_rule_add() 
 */
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
 * \brief setting rule onentry attribute to filter the entries that the iterator find. 
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
 * \brief setting field action on iterator, actions on the iterator are operations that the iterator will do on 
 * each entry it finds, field actions are actions which affect specific field values. Action can be set per field or for
 * entire result value (see description of field_id parameter) This API handling values which are holds as uint8 buffers
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
 * \brief see dbal_iterator_field_arr8_action_add()
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
 * \brief see dbal_iterator_field_arr8_action_add()
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

/**
 * \brief setting attribute action on iterator, the action will affect specific attribute of the entry
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


/** \brief - run the iterator after rules and actions were set. perform the set of actions for all entries that matches
 *         the rules. this function will return once all DB was scanned and all relevant entries were updated */
shr_error_e dbal_iterator_action_commit(
    int unit,
    uint32 entry_handle_id);

/**
 *\brief run the iterator after rules were set. will return the next relevant entry from the DB.
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
 *\brief destroy the iterator, called when finishing to work with the iterator. called automaticlly by handle mangement 
 *       functions.
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

/**
 * \brief - set a sub struct field value into full field value if the field is encoded the value that will be added to
 *        the struct will be the encoded value (for example in ENUM will add the HW value), if the field is subfield of
 *        the parent field will use the encoding between the subfield to the parent field
 *        For structure fields with arr prefix, it verify the min and max value of the full field value with the prefix
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
 * \brief - read a sub struct field value from full field value, the field in struct must be up to 32bit size. 
 * for bigger fields use  dbal_fields_struct_field_decode_masked()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] field_id - full field id
 *   \param [in] field_in_struct_id - sub struct field id
 *   \param [in] field_in_struct_val- sub struct field value (output buffer)
 *   \param [in] full_field_val - full field value (input buffer)
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
 * \brief - read a sub struct field value from full field and mask value 
 * For structure fields with arr prefix, return the full field value with the prefix
 *
 *   \param [in] unit - unit
 *   \param [in] field_id - The field ID of the struct
 *   \param [out] field_in_struct_id - sub struct field id
 *   \param [out] field_in_struct_val- sub struct field value (input buffer) must pointer with size of
 *          DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS
 *   \param [in] field_in_struct_mask- sub struct field mask (input buffer)
 *   \param [in] full_field_val - full field value (output buffer)
 *   \param [in] full_field_mask - full field mask (output buffer)
 *   \param [in] is_uint32 - if the expected value in field_in_struct_val is up to 32bit
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
    uint32 *full_field_mask,
    int is_uint32);

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
 * \brief Check if a given Enum val of a given field is legal value. legal value means:
 * 1. In valid range
 * 2. not disabled
 * 3. Not set as illegal
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] field_id - field of the relevant enum\n
 *   \param [in] value - logical value of the enum\n
 *   \param [out] is_illegal - Bollean indication. True means illegal\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the is_illegal pointer \n
 */
shr_error_e dbal_fields_is_illegal_value(
    int unit,
    dbal_fields_e field_id,
    uint32 value,
    uint8 *is_illegal);

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

/***********************************************logical table APIs ***********************************************/
/**
 * \brief delete all entries related to table from the HW.
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
 * \brief get table capacity. the maximum amount of entries that can be added to the table.
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
 * \brief return table field info, error if field not related to table.
 * in case that the field is sub field in a table returns the
 * parent field ID info.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested field is key or value field\n
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
 * \brief return the field offset in HW buffer e.g. field location in HW entry
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [in] is_key -  indication if the requested field is key or value field\n
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
 * \brief check if a given field is part of the table key fields
 * (directly or as child field)
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [out] is_key  -   indication if the field is one of the key fields of the table (or their child field) 
 */
shr_error_e dbal_tables_field_is_key_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_key);

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
 * \brief return the next dbal field in table, this API is used to iterate over all the table fields.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the relevant table_id.
 *   \param [in] field_id -  the current field to look for, if user want to start from the first field set to
 *          DBAL_FIELD_EMPTY\n
 *   \param [in] is_key -  type of the field key/result
 *   \param [in] result_type -  the result type to look for only valid when is_key==0.
 *   \param [out] next_field_id -  the next field ID in table , if no more fields found returns DBAL_FIELD_EMPTY \n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 *       filled the next_field_id pointer \n
 */
shr_error_e
dbal_tables_next_field_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int is_key,
    int result_type,
    dbal_fields_e * next_field_id);

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
 * \brief - Return the app db id that associates with the
 *        logical table. app db id  is valid for MDB tables and KBP tables only. otherwise an error will be return.
 *        Notice, not all MDB tables have valid app db id, if invalid U32_MAX will be set
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] app_db_id - the physical app db id associate, kbp db_id for kbp tables
 *   \param [in] app_db_size - the app db id size in bits associated with the logical table
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   See app_db_id above - the physical app db id associate
 *          with the logical table
 *   See app_db_size above
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
 * \brief - get an indication value for the logical table.
 *        if invalid an error will be prompted
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for
 *   \param [in] ind_type - the indication type
 *   \param [in] ind_val  - the indication val
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dbal_tables_indication_get(
    int unit,
    dbal_tables_e table_id,
    dbal_table_indications_e ind_type,
    uint32 *ind_val);

/**
 * \brief - API to set an indication for the logical table. not all indication are dynamic and can be updated. valid 
 *        indications to update are:DBAL_TABLE_IND_IS_LEARNING_EN, DBAL_TABLE_IND_IS_HITBIT_EN,
 *        DBAL_TABLE_IND_TYPE_NONE_DIRECT_OPTIMIZED.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for
 *   \param [in] ind_type - the indication type
 *   \param [in] ind_val  - the indication val
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dbal_tables_indication_set(
    int unit,
    dbal_tables_e table_id,
    dbal_table_indications_e ind_type,
    uint32 ind_val);

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
 * \brief - Return the payload size of a specific result type. 
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] res_type_idx - Result type index in table
 *   \param [in] p_size - payload size
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   See p_size above - the payload size in bits of the given
 *          result type
 */
shr_error_e dbal_tables_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size);

/**
 * \brief - return a printable string that can be used to dump the field value. the printable can be enum/define name or
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
 * \brief For a key field in a table, get whether the key field has the "is_ranged" attribute.
 *
 *   \param [in] unit
 *   \param [in] table_id - the table to look in
 *   \param [in] field_id - the key field to check
 *   \param [out] is_ranged - Whether the key field in the table is ranged
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_table_field_is_ranged_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 * is_ranged);

/**
 * \brief For a key field in a table, get whether the key field has the "is_packed" attribute.
 *
 *   \param [in] unit
 *   \param [in] table_id - the table to look in
 *   \param [in] field_id - the key field to check
 *   \param [out] is_packed - Whether the key field in the table is packed 
 *                            (added to the same segment as the key before it)
 *
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_table_field_is_packed_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 * is_packed);

/**
 * \brief
 * this API returns table access info according to the access layer type
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id - the table to update
 *   \param [in] access_method - access method to update DBAL_ACCESS_METHOD_XXX
 *   \param [out] access_info - a pointer to the access info requested should be one of the following structs:
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
 * \brief - return a printable string that can be used to dump the complete entry. the printable can be 
 *        enum/define name or value represented according to dbal type )ip/mac...)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id - the table to look for\n
 *   \param [in] key_buffer - an LSB aligned buffer holding the key value, not including core id, if Null will
 *          only parse the payload buffer.\n
 *   \param [in] core_id - core id (0 - nof_cores() - 1) or DBAL_CORE_ALL.
 *   \param [in] payload_size - the size of the payload buffer in bits
 *   \param [in] payload_buffer - an MSB aligned buffer (data is aligned to payload_size) of the payload. if Null will
 *          only parse the Key buffer.
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
 * \brief this API return the pre-define value of the table field, the predefine value can be concluded from the XML 
 *        inputs, HW specifications or dynamic update (rare cases).
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

/**
 * \brief Indicate if a table is active for this specific run. table can be not active for the following: 
        1. table_status is HW_ERROR / incompatible / not initialized
        2. maturity_level is low.
 *   \param [in] unit
 *   \param [in] table_id - table id \n
 *   \param [out] is_table_active - 1 if table is active in this run, 0 otherwise.
 *          indication \n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e
dbal_tables_is_table_active(
    int unit,
    dbal_tables_e table_id,
    int *is_table_active);

/**
 * \brief returns the access method of the table
 *   \param [in] unit
 *   \param [in] table_id - table id \n
 *   \param [out] access_method - the table's access method\n
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_access_method_get(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e * access_method);

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


/**
 * \brief set the bulk mode that related to the access_method and bulk module, when set to collect mode, entries will 
 *        not be set to HW. entries will be collected to SW, only once dbal_bulk_commit will be called entries will be
 *        set to HW.
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
 * \brief commit the entries that where collected by the machine. this API should be called 
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
 * \brief clear all the rules that where added to the bulk machine.
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
 * \brief indicates if collection mode is enable in DBAL. collection mode should also enabled per table by using 
 *        dbal_tables_indication_set with indication = DBAL_TABLE_IND_COLLECT_MODE. collection mode uses multithreading
 *        solution in order to improve performance. one main thread is collecting the entries, the woker thread is
 *        adding the entries in async way. in order to receive info about all entries where added to pool user ushould
 *        use dbal_collection_trigger_set()
 *
 *   \param [in] unit
 * \return
 *   Non-zero(shr_error_e) in case of an error
 */
int
dbal_is_collection_mode_enabled(
    int unit);


/** 
 *  \brief user can initiate a trigger to start processing the pool, it is called in order to signal DBAL that all
 *         entries were added, (DBAL should not expect any more entries) this trigger is differentl from the internal
 *         trigger that is called only when the pool is full.
 *  
 *  \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] trigger_type -  trigger type, to indicate the API flavor \n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT 
 */
shr_error_e
dbal_collection_trigger_set(
   int unit, 
   dbal_collection_trigger_type_e trigger_type);
/*********************************************** DBAL init/de-init and global APIs *********************************************/
/**
 * \brief
 * perform init to DBAL. this function init only the core DBAL module, user should call list of the following sub_moudle (from dnx_init_deinit_dbal_seq):
 * dbal_field_types_init, dbal_tables_init, dbal_pemla_init, dbal_sw_access_init, dbal_mdb_init, 
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_init(
    int unit);

/**
 * \brief
 * perform deinit to module.
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_deinit(
    int unit);

/**
 * \brief
 * this API is called to update dbal layer that the device init done. There are some behaviors that changes between 
 * device state, mainly regarding to error handling for bare metal. 
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
 * perform tables init update dbal DB according to the XML output from DBAL autocoder.
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_init(
    int unit);

/**
 * \brief
 * perform tables deinit run over all existing tables and release any memory allocated for those tables.
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_tables_deinit(
    int unit);

/**
 * \brief
 * perform init to the access method, for each access method define, the user can perform init prior to any call dbal.
 * action.
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] access_method the access method to perform init
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_access_method_init(
   int unit, 
   dbal_access_method_e access_method);

/**
 * \brief
 * perform de-init to the access method, for each access method define, the user can perform deinit.
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] access_method the access method to perform deinit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dbal_access_method_deinit(
   int unit, 
   dbal_access_method_e access_method);

/**
 * \brief
 * Performs DBAL Post init operations
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_post_init_operations(
    int unit);

/**
 * \brief
 * Performs DBAL Pre deinit operations
 * \par DIRECT INPUT:
 *   \param [in] unit
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e dnx_dbal_pre_deinit_operations(
    int unit);

/** LOGGER APIs, the DBAL logger helps track over DBAL operations, logger can be performed in multiple ways,
 *  according to severity DBAL layer and specific table. */
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
 *\brief sets the logger mode regular/write only
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] mode -  logger mode
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 */
shr_error_e
dbal_logger_table_mode_set(
    int unit,
    dbal_logger_mode_e mode);

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
 *\brief copy SW entry handle. this function copies a created and used entry handle
 *       to a new allocates internally entry handle
 *   should be used only by DBAL_HANDLE_COPY macro
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] src_handle_id -  the source handle id to copy from (allocated and used befor calling this function)\n
 *   \param [in] dst_handle_id -  pointer to the returned handle ID (allocated internally)\n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 */
extern shr_error_e dbal_entry_handle_copy_macro(
    int unit,
    uint32 src_handle_id,
    uint32 *dst_handle_id);

/**
 *\brief return handle to pool. must be called after taking an handle. 
 *   
 *
 * \par DIRECT INPUT:
 *   \param [in] unit -  \n
 *   \param [in] entry_handle_id -  handle ID to return\n
 *
 *   \return
 *      Non-zero(shr_error_e) in case of an error
 */
extern shr_error_e dbal_entry_handle_release_internal(
    int unit,
    uint32 entry_handle_id);

/**
 *\brief Checks if DBAL table is direct and returns TRUE or FALSE
 *
 * \par DIRECT INPUT:
 *   \param [in] unit
 *   \param [in] table_id -  the requested table to check DIRECT or not \n
 *   \param [out] is_table_direct -  return value of TRUE or FALSE \n
 * \par DIRECT OUTPUT:
 *   Non-zero(shr_error_e) in case of an error
 * \par INDIRECT OUTPUT
 */
shr_error_e
dbal_tables_table_is_direct(
    int unit,
    dbal_tables_e table_id,
    int *is_table_direct);


#endif/*_DBAL_INCLUDED__*/
