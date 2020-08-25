/** \file diag_dnx_dbal_table.c
 *
 * Main diagnostics for dbal applications All CLI commands, that
 * are related to DBAL tables, are gathered in this file.
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

/*************
 * FUNCTIONS *
 *************/

/*************
* FUNCTIONS *
*************/

 /*
  * LOCAL DIAG PACK:
  * {
  */
/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  STRUCTURE:
 *  MAIN MENU CMD: define under sh_dnx_dbal_cmds
 *     TABLE - SUB MENU defined under sh_dnx_dbal_table_cmds
 *     ENTRY - SUB MENU defined under sh_dnx_dbal_entry_cmds
 *     TYPES - SUB MENU defined under sh_dnx_dbal_field_types_cmds
 *     LABELSDUMP
 *     LOGSEVERITY
 *     HANDLESSTATUS
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  TABLE SUB MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. TABLE SUB MENU cmd table
 **********************************************************************************************************************/
static shr_error_e
diag_dbal_table_list_dump(
    int unit,
    dbal_labels_e label,
    dbal_maturity_level_e maturity_level,
    dbal_table_status_e status,
    dbal_access_method_e access_method,
    int min_num_operations,
    sh_sand_control_t * sand_control)
{
    int table_id, jj, nof_tables = 0;
    int char_count = 0;
    uint8 to_print;
    CONST dbal_logical_table_t *table;
    char *str;
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    str = buffer_to_print;

    char_count = sal_sprintf(str, "%s", "LOGICAL TABLES");
    str += char_count;

    if (label != DBAL_LABEL_NONE)
    {
        char_count = sal_sprintf(str, ", with %s %s", "LABEL:", dbal_label_to_string(unit, label));
        str += char_count;
    }
    if (maturity_level != DBAL_NOF_MATURITY_LEVELS)
    {
        char_count = sal_sprintf(str, ", with %s %s", "MATURITY:", dbal_maturity_level_to_string(unit, maturity_level));
        str += char_count;
    }
    if (status != DBAL_NOF_TABLE_STATUS)
    {
        char_count = sal_sprintf(str, ", with %s %s", "STATUS:", dbal_table_status_to_string(unit, status));
        str += char_count;
    }

    if (access_method != DBAL_NOF_ACCESS_METHODS)
    {
        char_count = sal_sprintf(str, ", with %s %s", "ACCESS:", dbal_access_method_to_string(unit, access_method));
        str += char_count;
    }
    PRT_TITLE_SET("%s", buffer_to_print);

    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("Access Type");
    PRT_COLUMN_ADD("Maturity");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Status");
    PRT_COLUMN_ADD("Set operations");
    PRT_COLUMN_ADD("Get operations");

    for (table_id = 0; table_id < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); table_id++)
    {
        dbal_table_status_e table_status;
        to_print = TRUE;

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        if ((table_status == DBAL_TABLE_NOT_INITIALIZED))
        {
            continue;
        }

        if (label != DBAL_LABEL_NONE)
        {
            for (jj = 0; jj < table->nof_labels; jj++)
            {
                if ((table->table_labels[jj] == label))
                {
                    break;
                }
            }
            if (jj == table->nof_labels)
            {
                to_print = FALSE;
            }
        }

        if (min_num_operations != 0)
        {
            int nof_get_operations = 0, nof_set_operations = 0;
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_set_operations.get(unit, table_id, &nof_set_operations));
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_get_operations.get(unit, table_id, &nof_get_operations));
            if ((nof_set_operations + nof_get_operations) < min_num_operations)
            {
                to_print = FALSE;
            }
        }

        if ((maturity_level != DBAL_NOF_MATURITY_LEVELS) && (table->maturity_level != maturity_level))
        {
            to_print = FALSE;
        }

        if ((access_method != DBAL_NOF_ACCESS_METHODS) && (table->access_method != access_method))
        {
            to_print = FALSE;
        }

        if ((status != DBAL_NOF_TABLE_STATUS) && (table_status != status))
        {
            to_print = FALSE;
        }

        if (to_print)
        {
            int nof_get_operations, nof_set_operations;

            nof_tables++;

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_set_operations.get(unit, table_id, &nof_set_operations));
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_get_operations.get(unit, table_id, &nof_get_operations));
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", table->table_name);
            PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));
            if (table->maturity_level == DBAL_MATURITY_LOW)
            {
                PRT_CELL_SET("Low");
            }
            else if (table->maturity_level == DBAL_MATURITY_PARTIALLY_FUNCTIONAL)
            {
                PRT_CELL_SET("Medium");
            }
            else
            {
                PRT_CELL_SET("High");
            }
            PRT_CELL_SET("%s", dbal_table_type_to_string(unit, table->table_type));
            PRT_CELL_SET("%s", dbal_table_status_to_string(unit, table_status));
            PRT_CELL_SET("%d", nof_set_operations);
            PRT_CELL_SET("%d", nof_get_operations);
        }
    }

    PRT_INFO_ADD("Total Tables: %d", nof_tables);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Return the next table id that matches to the input string, 
* (next refers to the curr_table_id) if the subsstring is fully 
* match to the table name  than is_full_match=1 use 
* curr_table_id = -1 to start form the first existing table 
*/
cmd_result_t
diag_dbal_table_string_to_next_table_id_get(
    int unit,
    char *substr_match,
    dbal_tables_e curr_table_id,
    dbal_tables_e * table_id,
    int *is_full_match)
{
    int iter;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    *table_id = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);
    *is_full_match = 0;

    curr_table_id++;

    if ((substr_match == NULL) || (curr_table_id > dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input substr_match not exists or table_id too big\n");
    }

    for (iter = curr_table_id; iter < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); iter++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, iter, &table));

        if (!sal_strcasecmp(substr_match, table->table_name))
        {
            *is_full_match = 1;
            *table_id = iter;
            break;
        }

        if (strcaseindex(table->table_name, substr_match))
        {
            *table_id = iter;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Print table names fit current substring
*/
shr_error_e
diag_dbal_table_print_names(
    int unit,
    char *table_name)
{
    CONST dbal_logical_table_t *table;
    int is_full_match = 0;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    diag_dbal_table_string_to_next_table_id_get(unit, table_name, -1, &table_id, &is_full_match);

    while (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        LOG_CLI((BSL_META("\t%s \n"), table->table_name));

        diag_dbal_table_string_to_next_table_id_get(unit, table_name, table_id, &table_id, &is_full_match);
    }
exit:
    SHR_FUNC_EXIT;
}

/** returns the table ID that matches to the string, if there is more than one table ID that matches to the string
 *  dump all table IDs that related if not unqie table was found return table_id = DBAL_NOF_DYNAMIC_AND_STATIC_TABLES */
shr_error_e
diag_dbal_table_from_string(
    int unit,
    char *table_name,
    dbal_tables_e * table_id,
    sh_sand_control_t * sand_control)
{
    int is_full_match = 0;
    dbal_tables_e first_table_id;
    int nof_tables_found = 0;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Possible tables");

    PRT_COLUMN_ADD("Table Name");

    diag_dbal_table_string_to_next_table_id_get(unit, table_name, -1, table_id, &is_full_match);
    if ((*table_id) == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        LOG_CLI((BSL_META("No matching tables found\n\n")));
        SHR_EXIT();
    }

    first_table_id = (*table_id);

    while ((*table_id) != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        nof_tables_found++;
                /** full match - use this table */
        if (is_full_match)
        {
            SHR_EXIT();
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, (*table_id)));
        diag_dbal_table_string_to_next_table_id_get(unit, table_name, (*table_id), table_id, &is_full_match);
    }

        /** only one table found use it */
    if (nof_tables_found == 1)
    {
        (*table_id) = first_table_id;
        SHR_EXIT();
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
* \brief
* dump logical tables.
* input arg should be valid label_name / table_name.
* dump all logical tables names that related to specific label_id,
* if label_id = DBAL_LABEL_NONE dump all tables.
* if table_name match to more than one table name dump brief information of all matching tables
* if full match valid table_name exist dump extended table information
*****************************************************/
static shr_error_e
diag_dbal_table_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    int is_full_match = 0, min_num_operations = 0;
    dbal_tables_e table_id;
    dbal_tables_e first_table_id = -1;
    dbal_labels_e label;
    dbal_maturity_level_e maturity_level;
    dbal_access_method_e access_method;
    dbal_table_status_e table_status;
    char *table_name = NULL, *label_name = NULL, *maturity_str = NULL, *table_status_str = NULL, *access_method_str =
        NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("label", label_name);
    SH_SAND_GET_STR("maturity", maturity_str);
    SH_SAND_GET_STR("status", table_status_str);
    SH_SAND_GET_STR("access", access_method_str);
    SH_SAND_GET_INT32("operations", min_num_operations);

    if (ISEMPTY(table_name))
    {
        if (ISEMPTY(label_name))
        {
            label = DBAL_LABEL_NONE;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_name, &label));
        }

        if (ISEMPTY(maturity_str))
        {
            maturity_level = DBAL_NOF_MATURITY_LEVELS;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_maturity_level_string_to_id(unit, maturity_str, &maturity_level));
        }

        if (ISEMPTY(access_method_str))
        {
            access_method = DBAL_NOF_ACCESS_METHODS;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_access_method_string_to_id(unit, access_method_str, &access_method));
        }

        if (ISEMPTY(table_status_str))
        {
            table_status = DBAL_NOF_TABLE_STATUS;
        }
        else
        {
            if (sal_strcasecmp(table_status_str, "untouched") == 0)
            {
                table_status = DBAL_TABLE_INITIALIZED;
            }
            else if (sal_strcasecmp(table_status_str, "hwerr") == 0)
            {
                table_status = DBAL_TABLE_HW_ERROR;
            }
            else if (sal_strcasecmp(table_status_str, "NotInit") == 0)
            {
                table_status = DBAL_TABLE_NOT_INITIALIZED;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown table status %s options untouched, NotInit, Init, api, hwerr\n",
                             table_status_str);
            }
        }

        SHR_IF_ERR_EXIT(diag_dbal_table_list_dump
                        (unit, label, maturity_level, table_status, access_method, min_num_operations, sand_control));
    }
    else
    {
        diag_dbal_table_string_to_next_table_id_get(unit, table_name, -1, &table_id, &is_full_match);
        if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
        {
            LOG_CLI((BSL_META("no matching tables found\n\n")));
        }

        if (is_full_match)
        {       /* coverity[stack_use_overflow : FALSE] */
            SHR_IF_ERR_EXIT(diag_dbal_logical_table_dump(unit, table_id, 1, sand_control));
            SHR_EXIT();
        }

        first_table_id = table_id;
        diag_dbal_table_string_to_next_table_id_get(unit, table_name, table_id, &table_id, &is_full_match);
        if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
        {
            /*
             * only one table that match to string - print full table info
             */
            SHR_IF_ERR_EXIT(diag_dbal_logical_table_dump(unit, first_table_id, 1, sand_control));
            SHR_EXIT();
        }
        else
        {
            /** more than one table found, print the first table name */
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, first_table_id, &table));
            LOG_CLI((BSL_META("\t%s \n"), table->table_name));
        }

        while (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
        {
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
            LOG_CLI((BSL_META("\t%s \n"), table->table_name));

            diag_dbal_table_string_to_next_table_id_get(unit, table_name, table_id, &table_id, &is_full_match);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static void
diag_dbal_table_iterator_rule_string_parse(
    char *rule_str,
    dbal_condition_types_e * cond,
    char **rule_val_str)
{
    if (rule_str[0] == '=')
    {
        *cond = DBAL_CONDITION_EQUAL_TO;
        *rule_val_str = rule_str + 1;
    }
    else if ((rule_str[0] == '!') && (rule_str[1] == '='))
    {
        *cond = DBAL_CONDITION_NOT_EQUAL_TO;
        *rule_val_str = rule_str + 2;
    }
    else if (rule_str[0] == '>')
    {
        *cond = DBAL_CONDITION_BIGGER_THAN;
        *rule_val_str = rule_str + 1;
    }
    else if (rule_str[0] == '<')
    {
        *cond = DBAL_CONDITION_LOWER_THAN;
        *rule_val_str = rule_str + 1;
    }
    else if (sal_strcmp(rule_str, "odd") == 0)
    {
        *cond = DBAL_CONDITION_IS_ODD;
    }
    else if (sal_strcmp(rule_str, "even") == 0)
    {
        *cond = DBAL_CONDITION_IS_EVEN;
    }
    else
    {
        *cond = DBAL_CONDITION_NONE;
        *rule_val_str = NULL;
    }
}

static shr_error_e
diag_dbal_table_cmd_iterator_rule_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    dbal_fields_e field_id;
    shr_error_e rc;

    SHR_FUNC_INIT_VARS(unit);

    rc = dbal_field_string_to_id_no_error(unit, keyword, &field_id);

    if (type_p != NULL)
    {
        *type_p = SAL_FIELD_TYPE_STR;
    }
    if (id_p != NULL)
    {
        if (rc != _SHR_E_NOT_FOUND)
        {
            *id_p = field_id;
        }
        else
        {
            *id_p = DBAL_FIELD_EMPTY;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dbal_table_iterator_rules_read_and_set(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    int *nof_rules,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    uint8 is_key = TRUE, hit_bit_rule = FALSE;
    int num_of_rules = 0;
    char *rule_str;
    char *rule_val_str = "0";
    char *rule_name_str;
    dbal_condition_types_e condition = DBAL_CONDITION_NONE;
    dbal_fields_e field_id;
    dbal_field_print_type_e field_print_type;
    sh_sand_param_u parsed_value;
    sh_sand_arg_t *sand_arg;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** Add iterator Rules */
    if (sand_control)
    {
        SH_SAND_GET_ITERATOR(sand_arg)
        {
            field_id = SH_SAND_GET_ID(sand_arg);
            rule_str = SH_SAND_ARG_STR(sand_arg);
            rule_name_str = SH_SAND_GET_NAME(sand_arg);

                        /**Parse rule as string, convert rule condition to DBAL condition ENUM*/
            diag_dbal_table_iterator_rule_string_parse(rule_str, &condition, &rule_val_str);
            if (condition == DBAL_CONDITION_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot parse rule for field %s. rule %s\n",
                             dbal_field_to_string(unit, field_id), rule_str);
            }

            /*
             * handle attribute rule
             */
            if (field_id == DBAL_FIELD_EMPTY)
            {
                if (sal_strncasecmp(rule_name_str, "HITBIT", sal_strlen(rule_name_str)) == 0)
                {
                    uint32 attr_val;
                    uint32 attr_type =
                        (DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY);

                    SHR_IF_ERR_EXIT(utilex_str_stoul(rule_val_str, &attr_val));

                    dbal_iterator_attribute_rule_add(unit, entry_handle_id, attr_type, attr_val);
                    num_of_rules++;
                    hit_bit_rule = TRUE;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot parse rule for attribute %s. Supportes addtibute: HITBIT\n",
                                 rule_name_str);
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_print_type));
                dbal_builds_enum_sand_table(unit, field_id);

                                /** Parse rule value, from string to any sal_type */
                SHR_IF_ERR_EXIT(sh_sand_value_get(unit, dbal_field_type_to_sal_field_type[field_print_type],
                                                  rule_val_str, &parsed_value, &dbal_test_enum_table[0]));

                SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, table_id, field_id, &is_key));
                                /** Add rule */
                if (field_print_type == DBAL_FIELD_PRINT_TYPE_MAC)
                {
                    if (is_key == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr8_rule_add
                                        (unit, entry_handle_id, field_id, condition, parsed_value.mac_addr, NULL));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                                        (unit, entry_handle_id, field_id, 0, condition, parsed_value.mac_addr, NULL));
                    }
                }
                else if (field_print_type == DBAL_FIELD_PRINT_TYPE_IPV6)
                {
                    if (is_key == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr8_rule_add
                                        (unit, entry_handle_id, field_id, condition, parsed_value.ip6_addr, NULL));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                                        (unit, entry_handle_id, field_id, 0, condition, parsed_value.ip6_addr, NULL));
                    }
                }
                else
                {
                    if (is_key == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                                        (unit, entry_handle_id, field_id, condition, parsed_value.array_uint32, NULL));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                        (unit, entry_handle_id, field_id, 0, condition, parsed_value.array_uint32,
                                         NULL));
                    }
                }
                num_of_rules++;
            }
        }
    }

    if (!hit_bit_rule)
    {
        /*
         * For MACT, the iterator runs with flush machine.
         * flush machine is not supporting retrieving the hit bits.
         */
        if ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table_id != DBAL_TABLE_FWD_MACT)
            && (table_id != DBAL_TABLE_FWD_MACT_IVL) && (table->table_type != DBAL_TABLE_TYPE_TCAM))
        {
            SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add
                            (unit, entry_handle_id,
                             (DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY | DBAL_ENTRY_ATTR_HIT_GET)));
        }
    }

    *nof_rules = num_of_rules;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear a full DBAL table
 */
static shr_error_e
diag_dbal_table_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id;
    char *table_name = NULL;
    uint32 entry_handle_id;
    int nof_rules = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);

    if (ISEMPTY(table_name))
    {
        /**dbal_tables_list_dump(unit, DBAL_LABEL_NONE, sand_control);*/
        DIAG_DBAL_HEADER_DUMP("Example: dbal table Clear Table=<table_name>", "\0");
        SHR_EXIT();
    }

    if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_print_names(unit, table_name));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(diag_dbal_table_iterator_rules_read_and_set
                    (unit, entry_handle_id, table_id, &nof_rules, args, sand_control));

    if (nof_rules > 0)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, entry_handle_id, DBAL_ITER_ACTION_DELETE));
        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   dump all the entries of a logical table in a xml friendly format
 */
static shr_error_e
diag_dbal_table_detailed_dump(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    sh_sand_control_t * sand_control)
{
    dbal_entry_handle_t *entry_handle;
    int entry_col_id, key_col_id, result_col_id;
    int iter;
    uint32 entry_handle_id;
    int is_end, entry_counter = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", table->table_name);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &entry_col_id, "entry");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, entry_col_id, &key_col_id, "key_field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, key_col_id, NULL, "key_value");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, entry_col_id, &result_col_id, "result_field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, result_col_id, NULL, "result_value");

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        /*
         * Entry
         */
        PRT_CELL_SET("%d", entry_counter);
        for (iter = 0; (iter < entry_handle->nof_key_fields) || (iter < entry_handle->nof_result_fields); iter++)
        {
            char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
            if (iter > 0)
            {
                /*
                 * Same entry, new line
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("");
            }
            if (iter < entry_handle->nof_key_fields)
            {
                char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
                uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
                const char *field_name = dbal_field_to_string(unit, table->keys_info[iter].field_id);

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id,
                                                                      table->keys_info[iter].field_id, field_val));

                SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, table->keys_info[iter].field_id,
                                                                         table_id, field_val, NULL, 0, TRUE, FALSE,
                                                                         buffer));

                PRT_CELL_SET("%s", field_name);
                PRT_CELL_SET("%s", buffer);
            }
            else
            {
                /*
                 * Out of keys.
                 */
                PRT_CELL_SKIP(2);
            }

            if (iter < entry_handle->nof_result_fields)
            {
                const char *field_name = dbal_field_to_string(unit, entry_handle->value_field_ids[iter]);
                dbal_field_data_t field_data;

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                        entry_handle->value_field_ids[iter],
                                                                        INST_SINGLE, field_data.field_val));

                SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, entry_handle->value_field_ids[iter],
                                                                         table_id, field_data.field_val,
                                                                         NULL, entry_handle->cur_res_type, FALSE,
                                                                         FALSE, buffer));
                PRT_CELL_SET("%s", field_name);
                PRT_CELL_SET("%s", buffer);
            }
            else
            {
                /*
                 * Out of results.
                 */
                PRT_CELL_SKIP(2);
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        entry_counter++;
    }

    if (entry_counter > 0)
    {
        /*
         * Only print the table if any entry was added to it.
         */
        PRT_INFO_ADD("Total Entries: %d", entry_counter);
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * dump all the non-default entries of all the logical tables
 */
static shr_error_e
diag_dbal_table_print_all_tables(
    int unit,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id;
    dbal_table_status_e table_status;

    /*
     * If file name is given,  the printing is expected to  go to a file.
     * In that case, print some status to the console since the iteration
     * may take a long time.
     */
    char *filename;
    uint8 export_to_file;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("file", filename);
    export_to_file = (!ISEMPTY(filename));

    if (export_to_file)
    {
        LOG_CLI((BSL_META("DBAL dumps all tables to %s - this can take some time...\n."), filename));
    }
    for (table_id = 0; table_id < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); table_id++)
    {
        CONST dbal_logical_table_t *table;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

        if ((table->maturity_level <= DBAL_MATURITY_PARTIALLY_FUNCTIONAL) || (table_status != DBAL_TABLE_INITIALIZED))
        {
            continue;
        }
        if (export_to_file)
        {
            LOG_CLI((BSL_META(".")));
        }
        SHR_IF_ERR_CONT(diag_dbal_table_detailed_dump(unit, table_id, table, sand_control));
    }
    if (export_to_file)
    {
        LOG_CLI((BSL_META("\nDBAL dump - Done.\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Prints all the entries of a table with ID table_id.
 *   Printing format is compact - each entry in a single line.
 */
shr_error_e
diag_dbal_table_entries_print(
    int unit,
    dbal_tables_e table_id,
    uint8 dump_only_key_fields,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    int iter, is_end, res_type_index, first_res_index = 0;
    uint32 entry_handle_id;
    int nof_rules = 0;
    dbal_entry_handle_t *entry_handle;
    int entry_counter[DBAL_MAX_NUMBER_OF_RESULT_TYPES] = { 0 };

    /*
     * Up to DBAL_MAX_NUMBER_OF_RESULT_TYPES sub-tables
     */
    PRT_INIT_VARS_PL(DBAL_MAX_NUMBER_OF_RESULT_TYPES);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    DIAG_DBAL_TABLE_STATUS_VALIDATE(table_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(diag_dbal_table_iterator_rules_read_and_set
                    (unit, entry_handle_id, table_id, &nof_rules, args, sand_control));
    /*
     * Skip the RESULT_TYPE as a column
     */
    if (table->has_result_type)
    {
        first_res_index = 1;
    }

    /*
     * Print title and columns for each sub-table
     */
    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {

        PRT_TITLE_SET_PL(res_type_index, "Entries dump table %s", table->table_name);

        if (table->has_result_type)
        {
            PRT_INFO_ADD_PL(res_type_index, "Result type  %s", table->multi_res_info[res_type_index].result_type_name);
        }

        PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "#");

        /*
         * For TCAM tables - add the access id
         */
        if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
        {
            PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "ACCESS_ID");
        }

        /*
         * Key columns
         */
        for (iter = 0; iter < table->nof_key_fields; iter++)
        {
            PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "%s",
                                   dbal_field_to_string(unit, table->keys_info[iter].field_id));
        }

        if (!dump_only_key_fields)
        {
            PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "");
            /*
             * Result columns
             */
            for (iter = first_res_index; iter < table->multi_res_info[res_type_index].nof_result_fields; iter++)
            {
                if (table->multi_res_info[res_type_index].results_info[iter].is_valid_indication_needed)
                {
                    PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "%s (include valid bit)",
                                           dbal_field_to_string(unit,
                                                                table->multi_res_info[res_type_index].
                                                                results_info[iter].field_id));
                }
                else
                {
                    PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "%s",
                                           dbal_field_to_string(unit,
                                                                table->multi_res_info[res_type_index].
                                                                results_info[iter].field_id));
                }
            }
        }
        if ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table->table_type != DBAL_TABLE_TYPE_TCAM))
        {
            PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "HITBIT");
        }
    }

    /*
     * Use iterator to print all entries
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        /*
         * Read the result type index of this entry
         */
        uint32 curr_res_type_index = 0;
        if (table->has_result_type)
        {
            dbal_field_data_t field_data;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                    DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                                    field_data.field_val));
            curr_res_type_index = field_data.field_val[0];
        }

        /*
         * New row for the entry
         */
        PRT_ROW_ADD_PL(curr_res_type_index, PRT_ROW_SEP_NONE);
        PRT_CELL_SET_PL(curr_res_type_index, "%d", entry_counter[curr_res_type_index]);

        /*
         * For TCAM tables - add the access id
         */
        if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
        {
            uint32 entry_hw_id;
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, &entry_hw_id));
            PRT_CELL_SET_PL(curr_res_type_index, "%d", entry_hw_id);
        }

        /*
         * Add keys for the entry
         */
        for (iter = 0; iter < entry_handle->nof_key_fields; iter++)
        {
            char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
            uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
            uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

            if ((table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                || (table->table_type == DBAL_TABLE_TYPE_LPM))
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit, entry_handle_id,
                                                                             table->keys_info[iter].field_id,
                                                                             field_val, field_mask));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id,
                                                                      table->keys_info[iter].field_id, field_val));
            }

            if (entry_handle->key_field_ids[iter] == DBAL_FIELD_CORE_ID)
            {
                    /** in this case the core ID will be used as a separate parameter and not part of the key */
                if ((entry_handle->core_id == DBAL_CORE_ALL) || (table->core_mode == DBAL_CORE_MODE_SBC))
                {
                    sal_sprintf(buffer, "ALL");
                }
                else
                {
                    sal_sprintf(buffer, " %d ", entry_handle->core_id);
                }
            }
            else
            {
                if ((table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                    || (table->table_type == DBAL_TABLE_TYPE_LPM))
                {
                    SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, table->keys_info[iter].field_id,
                                                                             table_id, field_val, field_mask, 0,
                                                                             TRUE, FALSE, buffer));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, table->keys_info[iter].field_id,
                                                                             table_id, field_val, NULL, 0, TRUE,
                                                                             FALSE, buffer));
                }
            }
            PRT_CELL_SET_PL(curr_res_type_index, "%s", buffer);
        }

        if (!dump_only_key_fields)
        {
            PRT_CELL_SET_PL(curr_res_type_index, "");
            /*
             * Add results for the entry
             */
            for (iter = first_res_index; iter < entry_handle->nof_result_fields; iter++)
            {
                dbal_field_data_t field_data;
                char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
                int rv;

                rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                             entry_handle->value_field_ids[iter],
                                                             INST_SINGLE, field_data.field_val);

                /** when the field has valid indication it can return not_found, in this case no need to dump info*/
                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, entry_handle->value_field_ids[iter],
                                                                             table_id, field_data.field_val, NULL,
                                                                             entry_handle->cur_res_type, FALSE, FALSE,
                                                                             buffer));
                    PRT_CELL_SET_PL(curr_res_type_index, "%s", buffer);
                }
                else
                {
                    PRT_CELL_SET_PL(curr_res_type_index, "----");
                }
            }
        }
        /*
         * For MDB tables - add the hitbit
         */
        if ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table->table_type != DBAL_TABLE_TYPE_TCAM))
        {
            if (!SAL_BOOT_PLISIM)
            {
                uint32 hitbit;
                SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle_id, DBAL_ENTRY_ATTR_HIT_GET, &hitbit));
                PRT_CELL_SET_PL(curr_res_type_index, "%s",
                                dbal_mdb_hitbit_to_string(unit, table->physical_db_id[0], hitbit));
            }
            else
            {
                PRT_CELL_SET_PL(curr_res_type_index, "N/A");
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        entry_counter[curr_res_type_index]++;
    }

    /*
     * Add Total entries per table
     */
    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {
        PRT_INFO_ADD_PL(res_type_index, "Total Entries: %d", entry_counter[res_type_index]);
        PRT_COMMITX_PL(res_type_index);
    }

exit:
    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {
        PRT_FREE_PL(res_type_index);
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_entries_print_by_label(
    int unit,
    dbal_labels_e label)
{
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, label, DBAL_NOF_ACCESS_METHODS, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_id));

    while (table_id != DBAL_TABLE_EMPTY)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_entries_print(unit, table_id, 0, NULL, NULL));
        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, label, DBAL_NOF_ACCESS_METHODS, DBAL_PHYSICAL_TABLE_NONE, DBAL_TABLE_TYPE_NONE,
                         &table_id));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
* dump all entries that related to a specific table.
* this function is using iterator to dump all the entries.
* input parameter "table name"
*******************/
static shr_error_e
diag_dbal_table_entries_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id = 0;
    char *table_name;
    char *label_str;
    int dump_key_only;
    dbal_labels_e label = DBAL_LABEL_NONE;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_BOOL("key_only", dump_key_only);
    SH_SAND_GET_STR("label", label_str);

    if (ISEMPTY(label_str))
    {
        if (ISEMPTY(table_name))
        {
            DIAG_DBAL_HEADER_DUMP("table name must exists: dbal table dump Table=<table_name>", "\0");
            SHR_EXIT();
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_str, &label));
    }

    if (label != DBAL_LABEL_NONE)
    {
        SHR_IF_ERR_EXIT(diag_dbal_entries_print_by_label(unit, label));
        SHR_EXIT();
    }

    if (sal_strncmp(table_name, "ALL", sal_strlen(table_name)) == 0)
    {
        /** All tables dump requested */
        SHR_IF_ERR_EXIT(diag_dbal_table_print_all_tables(unit, sand_control));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_entries_print(unit, table_id, dump_key_only, args, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t diag_dbal_table_list_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"LaBeL", SAL_FIELD_TYPE_STR, "dbal label name", ""},
    {"MaTuRity", SAL_FIELD_TYPE_STR, "dbal maturity level", ""},
    {"StaTuS", SAL_FIELD_TYPE_STR, "dbal table status", ""},
    {"access", SAL_FIELD_TYPE_STR, "dbal table access", ""},
    {"operations", SAL_FIELD_TYPE_INT32, "min operations", "0"},
    {NULL}
};

static sh_sand_man_t diag_dbal_table_list_man = {
    .brief = "dump all list of tables info related to specific table name/label/maturity/status/access",
    .full = "dump all list of tables info related to specific table name/label/maturity/status,"
        "label can be from the list of labels, maturity level can be Medium/High status can be NotInit/Init/Untouched/API/hwerr",
    .synopsis = "[MaTuRity=<maturity level>] [LaBeL=<label name>] [status=<status>] [access=<access_method>]"
        "[TaBLe=<tabel name>]",
    .examples =
        "LaBeL=L3\n" "TaBLe=INGRESS_PP_PORT\n" "access=HARD_LOGIC\n" "MaTuRity=HIGH\n"
        "status=untouched operations=1000"
};

static sh_sand_option_t diag_dbal_table_clear_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {NULL}
};

static sh_sand_man_t diag_dbal_table_clear_man = {
    .brief = "delete all entries from the table",
    .full = NULL,
    .synopsis = "TaBLe=<tabel name>",
    .examples = "table=INGRESS_PP_PORT"
};

static sh_sand_option_t diag_dbal_table_dump_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"Key_Only", SAL_FIELD_TYPE_BOOL, "Dump only key fields", "false"},
    {"Label", SAL_FIELD_TYPE_STR, "Dump tables entries that related to label", ""},
    {NULL}
};

static sh_sand_man_t diag_dbal_table_dump_man = {
    .brief = "dump all table entries or dump partial entries by using rules",
    .full = NULL,
    .synopsis = "TaBLe=<tabel name>",
    .examples =
        "table=INGRESS_PP_PORT\n"
        "TaBLe=INGRESS_PP_PORT Key_Only\n"
        "label=DRAM_PHY_CONTROL\n"
        "TaBLe=QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES QOS_INT_MAP_ID=\"=5\"\n"
        "TaBLe=IN_AC_INFO_DB HITBIT=\"=0\"\n" "TaBLe=IN_AC_INFO_DB HITBIT=\"=1\""
};

sh_sand_cmd_t diag_dbal_table_cmds[] = {
  /**********************************************************************************************************
   * CMD_NAME *     CMD_ACTION              * Next *        Options                   *       MAN           *
   *          *                             * Level*                                  *                     *
   *          *                             * CMD  *                                  *                     *
   **********************************************************************************************************/
    {"Info", diag_dbal_table_info, NULL, diag_dbal_table_list_options, &diag_dbal_table_list_man,},
    {"CLear", diag_dbal_table_clear, NULL, diag_dbal_table_clear_options, &diag_dbal_table_clear_man,
     diag_dbal_table_cmd_iterator_rule_get},
    {"DuMP", diag_dbal_table_entries_dump, NULL, diag_dbal_table_dump_options, &diag_dbal_table_dump_man,
     diag_dbal_table_cmd_iterator_rule_get},
    {NULL}
};
