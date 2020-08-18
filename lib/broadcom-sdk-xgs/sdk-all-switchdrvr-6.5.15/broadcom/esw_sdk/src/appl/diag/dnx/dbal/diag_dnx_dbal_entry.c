/** \file diag_dnx_dbal_entry.c
 *
 * Main diagnostics for dbal applications All CLI commands, that
 * are related to DBAL entry, are gathered in this file.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDBALDNX

/*************
 * INCLUDES  *
 *************/
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include "diag_dnx_dbal_internal.h"
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/sand/sand_aux_access.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/sand/sand_signals.h>

#include "diag_dnx_dbal.h"

/*************
 * TYPEDEFS  *
 *************/

extern shr_error_e dnx_algo_res_dbal_out_lif_is_allocated(
    int unit,
    int core_id,
    int out_lif,
    dbal_tables_e table_id,
    int *result_type,
    int is_alloc_table_any,
    uint8 *is_allocated);
extern shr_error_e dnx_algo_res_dbal_out_rif_is_allocated(
    int unit,
    int core_id,
    int out_lif,
    dbal_tables_e table_id,
    int *result_type,
    int is_alloc_table_any,
    uint8 *is_allocated);
extern shr_error_e dnx_algo_res_dbal_lif_is_allocated(
    int unit,
    int core_id,
    int in_lif,
    int sub_resource_index,
    dbal_tables_e table_id,
    int *result_type,
    int is_aloc_table_any,
    uint8 *is_allocated);

/*************
 * FUNCTIONS *
 *************/

/*
 * helper macros for performing the dbal action according to the field type & prameters
 */
#define DBAL_ENTRY_COMMIT_KEY_FIELD_WITH_MASK(unit, entry_handle_id, field_id, sand_arg)      \
do                                                                                  \
{                                                                                   \
    dbal_field_types_defs_e field_type;                                             \
    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));       \
                                                                                    \
    if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)                                   \
    {                                                                               \
      dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, field_id,         \
                                          SH_SAND_ARG_MAC_DATA(sand_arg),           \
                                          SH_SAND_ARG_MAC_MASK(sand_arg));          \
    }                                                                               \
    else if ((field_type == DBAL_FIELD_TYPE_DEF_IPV6) |                             \
             (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP))                        \
    {                                                                               \
      dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, field_id,         \
                                          SH_SAND_ARG_IP6_DATA(sand_arg),           \
                                          SH_SAND_ARG_IP6_MASK(sand_arg));          \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, field_id,        \
                                          SH_SAND_ARG_ARRAY_DATA(sand_arg),         \
                                          SH_SAND_ARG_ARRAY_MASK(sand_arg));        \
    }                                                                               \
}while(0)

#define DBAL_ENTRY_COMMIT_KEY_FIELD(unit, entry_handle_id, field_id, sand_arg)      \
do                                                                                  \
{                                                                                   \
    dbal_field_types_defs_e field_type;                                             \
    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));       \
                                                                                    \
    if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)                                   \
    {                                                                               \
      dbal_entry_key_field_arr8_set(unit, entry_handle_id, field_id,                \
                                                   SH_SAND_ARG_MAC_DATA(sand_arg)); \
    }                                                                               \
    else if ((field_type == DBAL_FIELD_TYPE_DEF_IPV6) |                             \
             (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP))                        \
    {                                                                               \
      dbal_entry_key_field_arr8_set(unit, entry_handle_id, field_id,                \
                                                   SH_SAND_ARG_IP6_DATA(sand_arg)); \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id,               \
                                                 SH_SAND_ARG_ARRAY_DATA(sand_arg)); \
    }                                                                               \
}while(0)

#define DBAL_ENTRY_COMMIT_VALUE_FIELD(unit, entry_handle_id, field_id, sand_arg)    \
do                                                                                  \
{                                                                                   \
    dbal_field_types_defs_e field_type;                                             \
    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));       \
                                                                                    \
    if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)                                   \
    {                                                                               \
      dbal_entry_value_field_arr8_set(unit, entry_handle_id, field_id, INST_SINGLE, \
                                                   SH_SAND_ARG_MAC_DATA(sand_arg)); \
    }                                                                               \
    else if ((field_type == DBAL_FIELD_TYPE_DEF_IPV6) |                             \
             (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP))                        \
    {                                                                               \
      dbal_entry_value_field_arr8_set(unit, entry_handle_id, field_id, INST_SINGLE, \
                                                   SH_SAND_ARG_IP6_DATA(sand_arg)); \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      if(field_id == DBAL_FIELD_RESULT_TYPE)                                        \
      {                                                                             \
          uint32 res_type;                                                          \
          int rv;                                                                   \
          rv = dbal_result_type_string_to_id(unit, table_id, SH_SAND_ARG_STR(sand_arg), &res_type);\
          if(rv)                                                                    \
          {                                                                         \
              LOG_CLI((BSL_META("result type name not valid %s, please use the string represent the result type value"), SH_SAND_ARG_STR(sand_arg)));\
              diag_dbal_entry_print_help_line(unit, table_name, cmd, FALSE);\
              SHR_EXIT();                                                           \
          }                                                                         \
          dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE,\
                                                 res_type);                         \
      }                                                                             \
      else                                                                          \
      {                                                                             \
          dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,\
                                                 SH_SAND_ARG_ARRAY_DATA(sand_arg)); \
      }                                                                             \
    }                                                                               \
}while(0)

/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  ENTRY SUB MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. ENTRY SUB MENU cmd table
 **********************************************************************************************************************/
/**
* \brief
* Print usgae example specific to requested table
*****************************************************/
static shr_error_e
diag_dbal_entry_print_help_line(
    int unit,
    char *table_name,
    char *cmd,
    uint8 key_field_only)
{
    CONST dbal_logical_table_t *table;
    dbal_field_types_basic_info_t *field_info;
    dbal_tables_e table_id;
    uint8 field_index;
    uint8 enum_val_index;
    uint8 result_index;
    uint8 enum_field_exist;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_print_names(unit, table_name));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    DIAG_DBAL_HEADER_DUMP("USAGE EXAMPLE: ", "\0");
    for (result_index = 0; result_index < table->nof_result_types; result_index++)
    {
        enum_field_exist = FALSE;

        if (!table->has_result_type || (key_field_only == TRUE))
        {
            LOG_CLI((BSL_META("dbal ENTry %s TaBLe=%s "), cmd, table_name));
        }
        else
        {
            LOG_CLI((BSL_META("For result type: %s: \n"), table->multi_res_info[result_index].result_type_name));
            LOG_CLI((BSL_META("    dbal ENTry %s TaBLe=%s "), cmd, table_name));
        }

        if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
        {
            if (table->access_method == DBAL_ACCESS_METHOD_MDB)
            {
                LOG_CLI((BSL_META("Priority=<priority_uint32> ")));
            }
            else
            {
                LOG_CLI((BSL_META("AccessId=<access_id_uint32> ")));
                if (key_field_only == TRUE)
                {
                    LOG_CLI((BSL_META("\n")));
                    SHR_EXIT();
                }
            }
        }

        /*
         * Print Key fields
         */
        for (field_index = 0; field_index < table->nof_key_fields; field_index++)
        {
            if ((key_field_only == FALSE)
                && ((table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                    || (table->table_type == DBAL_TABLE_TYPE_LPM)))
            {
                LOG_CLI((BSL_META("%s=<val,mask> "),
                         dbal_field_to_string(unit, table->keys_info[field_index].field_id)));
            }
            else
            {
                LOG_CLI((BSL_META("%s=<val> "), dbal_field_to_string(unit, table->keys_info[field_index].field_id)));
            }
        }
        if (key_field_only == FALSE)
        {
            /*
             * Print Result fields
             */
            for (field_index = 0; field_index < table->multi_res_info[result_index].nof_result_fields; field_index++)
            {
                if (table->multi_res_info[result_index].results_info[field_index].field_id == DBAL_FIELD_RESULT_TYPE)
                {
                    LOG_CLI((BSL_META("%s=%s "),
                             dbal_field_to_string(unit,
                                                  table->multi_res_info[result_index].
                                                  results_info[field_index].field_id),
                             table->multi_res_info[result_index].result_type_name));
                }
                else
                {
                    LOG_CLI((BSL_META("%s=<val> "),
                             dbal_field_to_string(unit,
                                                  table->multi_res_info[result_index].
                                                  results_info[field_index].field_id)));
                }
            }
        }

        LOG_CLI((BSL_META("\n\n")));

        /*
         * print enum fields values if exist
         */
        PRT_TITLE_SET("ENUM Fields");
        PRT_COLUMN_ADD("Field Name");
        PRT_COLUMN_ADD("ENUM Value");

        for (field_index = 0; field_index < table->nof_key_fields; field_index++)
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                            (unit, table->keys_info[field_index].field_id, &field_info));
            if (field_info->nof_enum_values > 0 && field_info->nof_enum_values < 32)/** not printing help line for enums with more then 32 values*/
            {
                enum_field_exist = TRUE;
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", dbal_field_to_string(unit, table->keys_info[field_index].field_id));
                for (enum_val_index = 0; enum_val_index < field_info->nof_enum_values; enum_val_index++)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(PRT_COLUMN_NUM - 1);
                    PRT_CELL_SET("%s", field_info->enum_val_info[enum_val_index].name);
                }
            }
        }

        for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                            (unit, table->multi_res_info[0].results_info[field_index].field_id, &field_info));
            if (field_info->nof_enum_values > 0 && field_info->nof_enum_values < 32)/** not printing help line for enums with more then 32 values*/
            {
                enum_field_exist = TRUE;
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s",
                             dbal_field_to_string(unit, table->multi_res_info[0].results_info[field_index].field_id));
                for (enum_val_index = 0; enum_val_index < field_info->nof_enum_values; enum_val_index++)
                {
                    char *enum_name;
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(PRT_COLUMN_NUM - 1);
                    enum_name = &field_info->enum_val_info[enum_val_index].name[sal_strlen(field_info->name) + 1];
                    PRT_CELL_SET("%s", enum_name);
                }
            }
        }
        if (enum_field_exist)
        {
            PRT_COMMIT;
        }
        else
        {
            PRT_FREE;
        }
        if (key_field_only)
        {
            break;
        }
    }
    LOG_CLI((BSL_META("\n")));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dbal_entry_cmd_field_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    dbal_fields_e field_id;
    dbal_field_print_type_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, keyword, &field_id));
    SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_type));

    dbal_builds_enum_sand_table(unit, field_id);

    if (type_p != NULL)
    {
        /** incase of result type recieve the value as string and than transform it to ID */
        if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            *type_p = SAL_FIELD_TYPE_STR;
        }
        else
        {
            *type_p = dbal_field_type_to_sal_field_type[field_type];
        }
    }
    if (id_p != NULL)
    {
        *id_p = field_id;
    }
    if (ext_ptr_p != NULL)
    {   /* Fill here you pointer to sh_sand_enum_t */
        *ext_ptr_p = &dbal_test_enum_table[0];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * utility function for diag_dbal_entry_commit &
 * diag_dbal_entry_update.
 * commit a DBAL table entry acccordig to the commit_flag param.
 *
 *    \param [in] unit - Relevant unit.
 *    \param [in] sand_control - recieved sand control.
 *    \param [in] commit_flags - DBAL_COMMIT - perform an
 *                entry commit. DBAL_COMMIT_UPDATE_ENTRY - perform an entry update.
 *    \param[in] cmd - string describing the performed action
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
diag_dbal_entry_commit_flag(
    int unit,
    sh_sand_control_t * sand_control,
    dbal_entry_action_flags_e commit_flags,
    char *cmd)
{
    CONST dbal_logical_table_t *table;
    uint32 entry_handle_id;
    int key_field_index = 0;
    int value_field_index = 0;
    uint32 access_id;
    uint32 priority;
    char *table_name;
    dbal_tables_e table_id;
    sh_sand_arg_t *sand_arg;
    dbal_fields_e field_id;
    uint8 is_key, action_validated;
    int core = SOC_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    action_validated = 0;
    /*
     * Get parameters
     */
    SH_SAND_GET_STR("table", table_name);

    if (ISEMPTY(table_name))
    {
        if (commit_flags == DBAL_COMMIT)
        {
            DIAG_DBAL_HEADER_DUMP("Example: dbal Entry CoMmit TaBLe=<table_name>", "\0");
        }
        else
        {
            DIAG_DBAL_HEADER_DUMP("Example: dbal Entry Update TaBLe=<table_name>", "\0");
        }
        SHR_EXIT();
    }

    if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_print_names(unit, table_name));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    DIAG_DBAL_TABLE_STATUS_VALIDATE(table_id);

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        if (table->access_method == DBAL_ACCESS_METHOD_MDB)
        {
            /*
             * Get priority , default priority is 0
             */
            SH_SAND_GET_UINT32("Priority", priority);

            SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, table_id, priority, &access_id));
        }
        else
        {
            /*
             * Get access id
             */
            SH_SAND_GET_UINT32("AccessId", access_id);
            commit_flags |= DBAL_COMMIT_OVERRIDE_DEFAULT;
        }

        if (access_id == 0xffffffff)
        {
            /*
             * Access id not received or not legal
             */
            diag_dbal_entry_print_help_line(unit, table_name, cmd, FALSE);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
        }
    }

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        int mask_present = SH_SAND_HAS_MASK(sand_arg);
        field_id = SH_SAND_GET_ID(sand_arg);

        SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, table_id, field_id, &is_key));
        if (is_key == TRUE)
        {
            if (mask_present
                && ((table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                    || (table->table_type == DBAL_TABLE_TYPE_LPM)))
            {
                /*
                 * Get Key fields Params with mask
                 */
                DBAL_ENTRY_COMMIT_KEY_FIELD_WITH_MASK(unit, entry_handle_id, field_id, sand_arg);
            }
            else
            {
                /*
                 * Get Key fields Params
                 */
                DBAL_ENTRY_COMMIT_KEY_FIELD(unit, entry_handle_id, field_id, sand_arg);
            }
            key_field_index++;
        }
        else
        {
            /*
             * Get Value fields Params
             */
            DBAL_ENTRY_COMMIT_VALUE_FIELD(unit, entry_handle_id, field_id, sand_arg);
            value_field_index++;
        }
    }

    if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        if (key_field_index > 0 || value_field_index > 0)
        {
            action_validated = 1;
        }
    }
    else
    {
        if ((key_field_index == table->nof_key_fields) && value_field_index > 0)
        {
            action_validated = 1;
        }
    }

    if (!action_validated)
    {
        if (key_field_index == table->nof_key_fields)
        {
            LOG_CLI((BSL_META("All Key fields are mandatory find available fields below:\n")));
        }
        else
        {
            LOG_CLI((BSL_META("at least one result field should be set, find available fields below:\n")));
        }

        if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_print_names(unit, table_name));
            SHR_EXIT();
        }

        diag_dbal_entry_print_help_line(unit, table_name, cmd, FALSE);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flags | DBAL_COMMIT_IGNORE_ALLOC_ERROR));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - commit a DBAL table entry
 */
static shr_error_e
diag_dbal_entry_commit(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dbal_entry_commit_flag(unit, sand_control, DBAL_COMMIT, "CoMmit"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update a DBAL table entry
 */
static shr_error_e
diag_dbal_entry_update(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dbal_entry_commit_flag(unit, sand_control, DBAL_COMMIT_UPDATE, "Update"));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dbal_entry_lif_res_type_set(
    int unit,
    uint32 entry_handle_id,
    int lif_val)
{
    uint8 is_allocated;
    int result_type = -1;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (dbal_logical_table_is_out_lif_allocator_eedb(entry_handle->table))
    {
        SHR_IF_ERR_EXIT(dnx_algo_res_dbal_out_lif_is_allocated
                        (unit, entry_handle->core_id, lif_val, entry_handle->table_id, &result_type, 0, &is_allocated));
    }
    else if (dbal_logical_table_is_out_rif_allocator(entry_handle->table))
    {
        SHR_IF_ERR_EXIT(dnx_algo_res_dbal_out_rif_is_allocated
                        (unit, entry_handle->core_id, lif_val, entry_handle->table_id, &result_type, 0, &is_allocated));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_res_dbal_lif_is_allocated
                        (unit, entry_handle->core_id, lif_val, 0, entry_handle->table_id, &result_type, 0,
                         &is_allocated));
    }

    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "LIF %d not allocated\n", lif_val);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_result_type_set(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    sh_sand_arg_t * sand_arg,
    char *cmd)
{
    uint32 res_type;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_result_type_string_to_id(unit, table_id, SH_SAND_ARG_STR(sand_arg), &res_type);
    if (rv)
    {
        LOG_CLI((BSL_META
                 ("result type name not valid %s, please use the string represent the result type value"),
                 SH_SAND_ARG_STR(sand_arg)));
        diag_dbal_entry_print_help_line(unit, dbal_logical_table_to_string(unit, table_id), cmd, FALSE);
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_entry_print_entry(
    int unit,
    uint32 entry_handle_id)
{
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE] = { 0 };
    dbal_field_data_t *key_fields_array = NULL;
    dbal_field_data_t *val_fields_array = NULL;
    CONST dbal_logical_table_t *table;
    dbal_entry_handle_t *entry_handle;
    int first_res_index = 0;
    int key_field_index = 0;
    int field_index = 0;
    dbal_fields_e field_id;
    dbal_tables_e table_id;
    int result_type_idx = 0;
    uint32 access_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    table = entry_handle->table;

    if (entry_handle->access_id_set)
    {
        access_id = entry_handle->phy_entry.entry_hw_id;
    }

    key_fields_array = sal_alloc(sizeof(dbal_field_data_t) * DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS, "Key fields");
    if (NULL == key_fields_array)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "alloc failed Key fields");
    }

    val_fields_array =
        sal_alloc(sizeof(dbal_field_data_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE, "Result fields");
    if (NULL == val_fields_array)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "alloc failed Result fields");
    }

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, (char *) (table->table_name), &table_id));

    LOG_CLI((BSL_META("\n")));
    if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        LOG_CLI((BSL_META("AccessId=%d \n"), access_id));
    }

    if (table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        uint32 attr_val;

        SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle_id,
                                                 (DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY |
                                                  DBAL_ENTRY_ATTR_HIT_GET), &attr_val));
        LOG_CLI((BSL_META("HitBit=%s \n"), dbal_mdb_hitbit_to_string(unit, table->physical_db_id[0], attr_val)));

        
    }

    /*
     * Print Key fields
     */
    LOG_CLI((BSL_META("Key: ")));
    for (key_field_index = 0; key_field_index < table->nof_key_fields; key_field_index++)
    {
        
        if ((table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
            && (table->access_method != DBAL_ACCESS_METHOD_MDB))
        {
            /*
             * Read all keys from table
             * In TCAM keys are not given as input for get - need to read them
             */
            key_fields_array[key_field_index].field_id = table->keys_info[key_field_index].field_id;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                            (unit, entry_handle_id, key_fields_array[key_field_index].field_id,
                             key_fields_array[key_field_index].field_val,
                             key_fields_array[key_field_index].field_mask));
            dbal_field_in_table_printable_string_get(unit, key_fields_array[key_field_index].field_id, table_id,
                                                     key_fields_array[key_field_index].field_val,
                                                     key_fields_array[key_field_index].field_mask, result_type_idx,
                                                     TRUE, FALSE, buffer);
        }
        else
        {
            /*
             * Read all keys from table
             * In TCAM keys are not given as input for get - need to read them
             */
            key_fields_array[key_field_index].field_id = table->keys_info[key_field_index].field_id;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, key_fields_array[key_field_index].field_id,
                             key_fields_array[key_field_index].field_val));
            dbal_field_in_table_printable_string_get(unit, key_fields_array[key_field_index].field_id, table_id,
                                                     key_fields_array[key_field_index].field_val, NULL,
                                                     result_type_idx, TRUE, FALSE, buffer);

        }
        LOG_CLI((BSL_META("%s %s "), dbal_field_to_string(unit, key_fields_array[key_field_index].field_id), buffer));
    }
    /*
     * Read Result fields
     */
    if (table->has_result_type)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, val_fields_array[0].field_val));
        val_fields_array[0].field_id = DBAL_FIELD_RESULT_TYPE;
        result_type_idx = val_fields_array[0].field_val[0];

        first_res_index = 1;
    }

    for (field_index = first_res_index; field_index < table->multi_res_info[result_type_idx].nof_result_fields;
         field_index++)
    {
        field_id = table->multi_res_info[result_type_idx].results_info[field_index].field_id;
        val_fields_array[field_index].field_id = field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id,
                                                                INST_SINGLE, val_fields_array[field_index].field_val));
    }

    /** Print Result fields */
    LOG_CLI((BSL_META("\nResult:\n")));
    for (field_index = 0; field_index < table->multi_res_info[result_type_idx].nof_result_fields; field_index++)
    {
        SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get
                        (unit, val_fields_array[field_index].field_id, table_id,
                         val_fields_array[field_index].field_val, NULL, result_type_idx, FALSE, FALSE, buffer));
        LOG_CLI((BSL_META("  %-40s %s\n"), dbal_field_to_string(unit, val_fields_array[field_index].field_id), buffer));
    }
    LOG_CLI((BSL_META("\n")));

exit:
    if (key_fields_array != NULL)
    {
        sal_free(key_fields_array);
    }

    if (val_fields_array != NULL)
    {
        sal_free(val_fields_array);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - get a DBAL table entry
 */
static shr_error_e
diag_dbal_entry_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    uint32 entry_handle_id;
    int key_field_index = 0;
    uint32 access_id;
    int lif_val = 0, result_type_set = 0;

    char *table_name;
    dbal_tables_e table_id;
    char *cmd = "Get";
    sh_sand_arg_t *sand_arg;
    dbal_fields_e field_id;
    uint8 is_key;
    uint8 action_validated = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get parameters
     */
    SH_SAND_GET_STR("table", table_name);

    if (ISEMPTY(table_name))
    {
        /**dbal_tables_list_dump(unit, DBAL_LABEL_NONE, sand_control);*/
        DIAG_DBAL_HEADER_DUMP("Example: dbal ENTry Get TaBLe=<table_name>", "\0");
        SHR_EXIT();
    }

    if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_print_names(unit, table_name));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    DIAG_DBAL_TABLE_STATUS_VALIDATE(table_id);

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        if (table->access_method != DBAL_ACCESS_METHOD_MDB)
        {
            /** Get access id */
            SH_SAND_GET_UINT32("AccessId", access_id);

            if (access_id == 0xffffffff)
            {
                /*
                 * Access id not received or not legal
                 */
                diag_dbal_entry_print_help_line(unit, table_name, cmd, TRUE);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
                action_validated = 1;
            }
        }
    }

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        int mask_present = SH_SAND_HAS_MASK(sand_arg);
        field_id = SH_SAND_GET_ID(sand_arg);

        SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, table_id, field_id, &is_key));
        if (is_key == TRUE)
        {
            if (mask_present
                && ((table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                    || (table->table_type == DBAL_TABLE_TYPE_LPM)))
            {
                /*
                 * Get Key fields Params with mask
                 */
                DBAL_ENTRY_COMMIT_KEY_FIELD_WITH_MASK(unit, entry_handle_id, field_id, sand_arg);
            }
            else
            {
                /*
                 * Get Key fields Params
                 */
                DBAL_ENTRY_COMMIT_KEY_FIELD(unit, entry_handle_id, field_id, sand_arg);
                if (field_id == DBAL_FIELD_OUT_LIF || field_id == DBAL_FIELD_IN_LIF)
                {
                    lif_val = SH_SAND_ARG_ARRAY_DATA(sand_arg)[0];
                }
            }
            key_field_index++;
        }
        else if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            SHR_IF_ERR_EXIT(diag_dbal_result_type_set(unit, entry_handle_id, table_id, sand_arg, cmd));
            result_type_set = 1;
        }
    }
    if ((((table->table_type != DBAL_TABLE_TYPE_TCAM) && (table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT)) &&
         (key_field_index != table->nof_key_fields)) ||
        ((key_field_index == 0) && !action_validated &&
         ((table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT))))
    {
        LOG_CLI((BSL_META("Key fields are mandatory please find available fields below:\n")));
        if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_print_names(unit, table_name));
            SHR_EXIT();
        }
        diag_dbal_entry_print_help_line(unit, table_name, cmd, TRUE);
        SHR_EXIT();
    }
    else
    {
        
        if (((table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT))
            && (table->access_method == DBAL_ACCESS_METHOD_MDB))
        {
            SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &access_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
        }

        /** reading the result type incase of LIF tables (LIF tables require the result type when geting/clearing */
        if ((result_type_set == 0) &&
            ((dbal_physical_table_is_in_lif(table->physical_db_id[0])) ||
             (dbal_logical_table_is_out_rif_allocator(table)) || (dbal_logical_table_is_out_lif_allocator_eedb(table))))
        {
            SHR_IF_ERR_EXIT(diag_dbal_entry_lif_res_type_set(unit, entry_handle_id, lif_val));
        }

        if (table->access_method == DBAL_ACCESS_METHOD_MDB)
        {
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id,
                                                         (DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY |
                                                          DBAL_ENTRY_ATTR_HIT_GET), NULL));
            
        }

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS | DBAL_COMMIT_IGNORE_ALLOC_ERROR));

        SHR_IF_ERR_EXIT(diag_dbal_entry_print_entry(unit, entry_handle_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear a DBAL table entry
 */
static shr_error_e
diag_dbal_entry_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    uint32 entry_handle_id;
    int key_field_index = 0;
    uint32 access_id;

    char *table_name;
    dbal_tables_e table_id;
    char *cmd = "CLear";
    sh_sand_arg_t *sand_arg;
    dbal_fields_e field_id;
    uint8 is_key;
    int lif_val = 0, result_type_set = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get parameters
     */
    SH_SAND_GET_STR("table", table_name);

    if (ISEMPTY(table_name))
    {
        /**dbal_tables_list_dump(unit, DBAL_LABEL_NONE, sand_control);*/
        DIAG_DBAL_HEADER_DUMP("Example: dbal ENTry CLear TaBLe=<table_name>", "\0");
        SHR_EXIT();
    }

    if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_print_names(unit, table_name));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    DIAG_DBAL_TABLE_STATUS_VALIDATE(table_id);

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    
    if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        if (table->access_method != DBAL_ACCESS_METHOD_MDB)
        {
            /*
             * Get access id
             */
            SH_SAND_GET_INT32("AccessId", access_id);

            if (access_id == 0xffffffff)
            {
                /*
                 * Access id not received or not legal
                 */
                diag_dbal_entry_print_help_line(unit, table_name, cmd, TRUE);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
            }
        }
    }

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        field_id = SH_SAND_GET_ID(sand_arg);
        SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, table_id, field_id, &is_key));
        if (is_key == TRUE)
        {
            /*
             * Get Key fields Params
             */
            DBAL_ENTRY_COMMIT_KEY_FIELD(unit, entry_handle_id, field_id, sand_arg);
            key_field_index++;

            if (field_id == DBAL_FIELD_OUT_LIF || field_id == DBAL_FIELD_IN_LIF)
            {
                lif_val = SH_SAND_ARG_ARRAY_DATA(sand_arg)[0];
            }
        }
        else if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            SHR_IF_ERR_EXIT(diag_dbal_result_type_set(unit, entry_handle_id, table_id, sand_arg, cmd));
            result_type_set = 1;
        }

    }

    /** reading the result type incase of LIF tables (LIF tables require the result type when geting/clearing */
    if ((result_type_set == 0) &&
        ((dbal_physical_table_is_in_lif(table->physical_db_id[0])) ||
         (dbal_logical_table_is_out_rif_allocator(table)) || (dbal_logical_table_is_out_lif_allocator_eedb(table))))
    {
        SHR_IF_ERR_EXIT(diag_dbal_entry_lif_res_type_set(unit, entry_handle_id, lif_val));
    }

    if ((key_field_index != table->nof_key_fields) &&
        (((table->table_type != DBAL_TABLE_TYPE_TCAM) && (table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT)) ||
         ((table->access_method == DBAL_ACCESS_METHOD_MDB))))
    {
        LOG_CLI((BSL_META("All Key fields are mandatory please find available fields below:\n")));
        if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_print_names(unit, table_name));
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(diag_dbal_logical_table_dump(unit, table_id, 0, sand_control));
        diag_dbal_entry_print_help_line(unit, table_name, cmd, TRUE);
        SHR_EXIT();
    }
    else
    {
        
        if ((table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
            && (table->access_method == DBAL_ACCESS_METHOD_MDB))
        {
            SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &access_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
        }

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t diag_dbal_entry_commit_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"AccessId", SAL_FIELD_TYPE_UINT32, "dbal entry access id", "0xffffffff"},
    {"Priority", SAL_FIELD_TYPE_UINT32, "dbal entry priority", "0"},
    {NULL}
};

static sh_sand_man_t diag_dbal_entry_commit_man = {
    .brief = "Set and commit a table entry",
    .full = NULL,
    .synopsis = "Table=<table_name>",
    .examples =
        "table=IN_AC_C_VLAN_DB Priority=1 VLAN_DOMAIN=1 C_VID=2 IN_LIF=3\n"
        "table=PRT_VIRTUAL_PORT_TCAM AccessId=1 PRT_KBR_INFO_HALF=2"
};

static sh_sand_option_t diag_dbal_entry_update_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {NULL}
};

static sh_sand_man_t diag_dbal_entry_update_man = {
    .brief = "Set and update a table entry",
    .full = NULL,
    .synopsis = "Table=<table_name>",
    .examples = "table=IN_AC_C_VLAN_DB VLAN_DOMAIN=1 C_VID=2 IN_LIF=4"
};

static sh_sand_option_t diag_dbal_entry_get_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"AccessId", SAL_FIELD_TYPE_UINT32, "dbal entry access id", "0xffffffff"},
    {NULL}
};

static sh_sand_man_t diag_dbal_entry_get_man = {
    .brief = "Get a table Entry",
    .full = NULL,
    .synopsis = "Table=<table_name>",
    .examples = "table=IN_AC_C_VLAN_DB VLAN_DOMAIN=1 C_VID=2 IN_LIF=4\n" "table=PRT_VIRTUAL_PORT_TCAM AccessId=1"
};

static sh_sand_option_t diag_dbal_entry_clear_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"AccessId", SAL_FIELD_TYPE_INT32, "dbal entry access id", "0xffffffff"},
    {NULL}
};

static sh_sand_man_t diag_dbal_entry_clear_man = {
    .brief = "Clear a table entry",
    .full = NULL,
    .synopsis = "Table=<table_name>",
    .examples = "table=IN_AC_C_VLAN_DB VLAN_DOMAIN=1 C_VID=2 IN_LIF=4\n"
        "table=EGRESS_ETPP_TERM_CONTEXT_PROPERTIES AccessId=1\n" "table=PRT_VIRTUAL_PORT_TCAM AccessId=1"
};

sh_sand_cmd_t diag_dbal_entry_cmds[] = {
   /******************************************************************************************************************
    * CMD    *     CMD_ACTION           * Next *        Options               *            MAN             * CB      *
    * NAME   *                          * Level*                              *                            *         *
    *        *                          * CMD  *                              *                            *         *
    ******************************************************************************************************************/
    {"CoMmit", diag_dbal_entry_commit, NULL, diag_dbal_entry_commit_options, &diag_dbal_entry_commit_man,
     diag_dbal_entry_cmd_field_get},
    {"Update", diag_dbal_entry_update, NULL, diag_dbal_entry_update_options, &diag_dbal_entry_update_man,
     diag_dbal_entry_cmd_field_get},
    {"Get", diag_dbal_entry_get, NULL, diag_dbal_entry_get_options, &diag_dbal_entry_get_man,
     diag_dbal_entry_cmd_field_get},
    {"CLear", diag_dbal_entry_clear, NULL, diag_dbal_entry_clear_options, &diag_dbal_entry_clear_man,
     diag_dbal_entry_cmd_field_get},
    {NULL}
};
