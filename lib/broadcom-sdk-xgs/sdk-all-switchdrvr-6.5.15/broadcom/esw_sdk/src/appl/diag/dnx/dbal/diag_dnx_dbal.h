
/*! \file diag_dnx_dbal.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_DBAL_H_INCLUDED
#define DIAG_DNX_DBAL_H_INCLUDED

/*************
 * INCLUDES  *
 *************/

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/** this Macro validate that the table was initialized before performing any action on the table */
#define DIAG_DBAL_TABLE_STATUS_VALIDATE(table_id)                                                                   \
{                                                                                                                   \
    dbal_table_status_e status;                                                                                     \
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &status));                              \
    {                                                                                                               \
        if ((status == DBAL_TABLE_NOT_INITIALIZED))                                                                 \
        {                                                                                                           \
            LOG_CLI((BSL_META("\tTABLE %s not initialized \n"), dbal_logical_table_to_string(unit, table_id)));     \
            SHR_EXIT();                                                                                             \
        }                                                                                                           \
    }                                                                                                               \
}

/*************
 * GLOBALS   *
 *************/
extern sh_sand_man_t sh_dnx_dbal_man;
extern sh_sand_cmd_t sh_dnx_dbal_cmds[];
extern sal_field_type_e dbal_field_type_to_sal_field_type[];
extern sh_sand_enum_t dbal_test_enum_table[];
extern sh_sand_cmd_t diag_dbal_table_cmds[];
extern sh_sand_cmd_t diag_dbal_signal_cmds[];
extern sh_sand_cmd_t diag_dbal_entry_cmds[];
extern dbal_field_types_defs_e dbal_field_id_to_type[DBAL_NOF_FIELDS];

shr_error_e diag_dbal_table_print_names(
    int unit,
    char *table_name);

shr_error_e dbal_builds_enum_sand_table(
    int unit,
    dbal_fields_e field_id);
/**
 * \brief
 *   Prints all the entries of a table with ID table_id.
 *   Printing format is compact - each entry in a single line.
 */
shr_error_e diag_dbal_table_entries_print(
    int unit,
    dbal_tables_e table_id,
    uint8 dump_only_key_fields,
    args_t * args,
    sh_sand_control_t * sand_control);

cmd_result_t diag_dbal_table_string_to_next_table_id_get(
    int unit,
    char *substr_match,
    dbal_tables_e curr_table_id,
    dbal_tables_e * table_id,
    int *is_full_match);

shr_error_e diag_dbal_table_from_string(
    int unit,
    char *table_name,
    dbal_tables_e * table_id,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *   Prints the entry of a table
 *   the key is pre defined in the handle and dbal enty get has
 *   been called before calling this function.
 */
shr_error_e diag_dbal_entry_print_entry(
    int unit,
    uint32 entry_handle_id);

#endif /* DIAG_DNX_DBAL_H_INCLUDED */
