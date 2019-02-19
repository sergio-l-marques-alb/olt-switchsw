/**
 * \file diag_sand_system.c
 *
 * System commands for dnx/dnxf shells
 *
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

extern shr_error_e sh_sand_option_list_process(
    int unit,
    args_t * args,
    rhlist_t * args_list,
    int flags,
    sh_sand_cmd_t * sh_sand_cmd);

static void
sh_sand_print(
    char *string,
    int left_margin,
    int right_margin,
    int term_width)
{
    int token_size;
    int str_shift;
    int str_offset = 0;
    int str_size = strlen(string);

    token_size = term_width - left_margin - right_margin;

    do
    {
        /*
         * Print left margin
         */
        diag_sand_prt_char(left_margin, ' ');
        /*
         * Get shift where last white space in the token or first new line are situated
         */
        str_shift = utilex_str_get_shift(string + str_offset, token_size);
        /*
         * Print string up to this delimiter, print only specified number of characters(str_shift) from the string
         */
        LOG_CLI((BSL_META("%.*s\n"), str_shift, string + str_offset));
        /*
         * If delimiter is new line print one more new line
         */
        if (*(string + str_offset) == '\n')
        {
            LOG_CLI((BSL_META("\n")));
        }
        /*
         * Update current string offset taking into account delimiter
         */
        str_offset += str_shift;
    }
    while (str_offset < str_size);      /* once current offset exceeds string length stop */
}

static shr_error_e
sh_sand_usage_leaf_tabular(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t * sand_control)
{
    sh_sand_option_t *option;
    int item_col_id, option_col_id;
    int flag;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((sh_sand_cmd == NULL) || (sh_sand_cmd->man == NULL))
    {   /* On this stage there is no need to print message, all errors should be rectified by verify */
        SHR_EXIT();
    }

    PRT_TITLE_SET("%s", "Usage");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &item_col_id, "Item");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, item_col_id, &option_col_id, "Option");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, option_col_id, NULL, "Type");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, option_col_id, NULL, "Default");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_ASCII, PRT_XML_ATTRIBUTE, option_col_id, NULL, "Description");

    if (sh_sand_cmd->man->synopsis != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("SYNOPSYS");
        PRT_CELL_SKIP(3);
        PRT_CELL_SET("%s", sh_sand_cmd->man->synopsis);
    }

    if (sh_sand_cmd->man->full != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("DESCRIPTION");
        PRT_CELL_SKIP(3);
        PRT_CELL_SET("%s", sh_sand_cmd->man->full);
    }

    if ((sh_sand_cmd->options != NULL) && (sh_sand_cmd->options->keyword != NULL))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("OPTIONS");
        for (option = sh_sand_cmd->options; option->keyword != NULL; option++)
        {
            char *def_str;
            if (option->def == NULL)
            {
                def_str = "NONE";
            }
            else if (ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }

            /*
             * For the first option skip 1 cell, for all others allocate new row 
             */
            if (option != sh_sand_cmd->options)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%s", option->keyword);
            PRT_CELL_SET("%s", sal_field_type_str(option->type));
            PRT_CELL_SET("%s", def_str);
            PRT_CELL_SET("%s", option->desc);
        }
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
    }

    /*
     * Show system options only when all option used
     */
    SH_SAND_GET_BOOL("all", flag);
    if (flag == TRUE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SYSTEM OPTIONS");
        for (option = sh_sand_sys_options; option->keyword != NULL; option++)
        {
            char *def_str;
            if (option->def == NULL)
            {
                def_str = "NONE";
            }
            else if (ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }

            /*
             * For the first option skip 1 cell, for all others allocate new row 
             */
            if (option != sh_sand_sys_options)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            PRT_CELL_SET("%s", option->keyword);
            PRT_CELL_SET("%s", sal_field_type_str(option->type));
            PRT_CELL_SET("%s", def_str);
            PRT_CELL_SET("%s", option->desc);
        }
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
    }

    if (sh_sand_cmd->man->examples != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("EXAMPLES");
        PRT_CELL_SKIP(3);
        PRT_CELL_SET("%s", sh_sand_cmd->man->examples);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_usage_leaf(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t * sand_control)
{
    int columns, margin;
    int flag;
    sh_sand_option_t *option;

    SHR_FUNC_INIT_VARS(unit);

    if ((sh_sand_cmd == NULL) || (sh_sand_cmd->man == NULL))
    {   /* On this stage there is no need to print message, all errors should be rectified by verify */
        SHR_EXIT();
    }

    SH_SAND_GET_BOOL("tabular", flag);
    if (flag == TRUE)
    {
        sh_sand_usage_leaf_tabular(unit, sh_sand_cmd, sand_control);
        goto exit;
    }

    SH_SAND_GET_INT32("column", columns);
    SH_SAND_GET_INT32("margin", margin);

    if (sh_sand_cmd->man->synopsis != NULL)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "SYNOPSYS"));
        sh_sand_print(sh_sand_cmd->man->synopsis, margin, margin, columns);
    }

    if (sh_sand_cmd->man->full != NULL)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "DESCRIPTION"));
        sh_sand_print(sh_sand_cmd->man->full, margin, margin, columns);
    }

    if ((sh_sand_cmd->options != NULL) && (sh_sand_cmd->options->keyword != NULL))
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "OPTIONS"));
        diag_sand_prt_char(margin, ' ');
        LOG_CLI((BSL_META("\033[1m%s\033[0m\n\n"), "option (type:default)"));
        for (option = sh_sand_cmd->options; option->keyword != NULL; option++)
        {
            char *def_str;

            if (option->def == NULL)
            {
                def_str = "NONE";
            }
            else if (ISEMPTY(option->def))
            {
                def_str = "EMPTY";
            }
            else
            {
                def_str = option->def;
            }

            diag_sand_prt_char(margin, ' ');
            LOG_CLI((BSL_META("%s (%s:%s)\n"), option->keyword, sal_field_type_str(option->type), def_str));
            if (option->desc)
            {
                sh_sand_print(option->desc, 2 * margin, margin, columns);
            }
        }
    }

    SH_SAND_GET_BOOL("all", flag);
    if (flag == TRUE)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "SYSTEM OPTIONS"));
        for (option = sh_sand_sys_options; option->keyword != NULL; option++)
        {
            diag_sand_prt_char(margin, ' ');
            LOG_CLI((BSL_META("%s (%s:%s)\n"), option->keyword, sal_field_type_str(option->type),
                     ((option->def == NULL) ? "NA" : option->def)));
            if (option->desc)
            {
                sh_sand_print(option->desc, 2 * margin, margin, columns);
            }
        }
    }

    if (sh_sand_cmd->man->examples != NULL)
    {
        LOG_CLI((BSL_META("\n\033[1m%s\033[0m\n"), "EXAMPLES"));
        sh_sand_print(sh_sand_cmd->man->examples, margin, margin, columns);
    }
    LOG_CLI((BSL_META("\n")));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_usage_branch(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_control_t * sand_control)
{
    sh_sand_cmd_t *sh_sand_cmd;
    sh_sand_cmd_t *sh_sys_cmds_a = sand_control->sh_sys_cmd_a;
    int all_flag;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", "Supported commands");

    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    for (sh_sand_cmd = sh_sand_cmd_a; sh_sand_cmd && (sh_sand_cmd->keyword != NULL); sh_sand_cmd++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", sh_sand_cmd->full_key);
        if (sh_sand_cmd->man && !ISEMPTY(sh_sand_cmd->man->brief))
        {
            PRT_CELL_SET("%s", sh_sand_cmd->man->brief);
        }
    }
    SH_SAND_GET_BOOL("all", all_flag);
    if ((all_flag == TRUE) && (sh_sys_cmds_a != NULL))
    {
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "System Commands");
        for (sh_sand_cmd = sh_sys_cmds_a; sh_sand_cmd && (sh_sand_cmd->keyword != NULL); sh_sand_cmd++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", sh_sand_cmd->full_key);
            if (sh_sand_cmd->man && !ISEMPTY(sh_sand_cmd->man->brief))
            {
                PRT_CELL_SET("%s", sh_sand_cmd->man->brief);
            }
        }
    }
    PRT_COMMIT;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t sys_usage_man = {
    "Print list of commands available",
    "Print list of commands available for the command",
    "command usage",
    "command usg"
};

sh_sand_option_t sys_usage_options[] = {
    {"column", SAL_FIELD_TYPE_INT32, "Maximum columns number for output", "80"}
    ,
    {"margin", SAL_FIELD_TYPE_INT32, "Left&Right margin of defined display width", "7"}
    ,
    {"tabular", SAL_FIELD_TYPE_BOOL, "Print usage in tabular view", "No"}
    ,
    {"all", SAL_FIELD_TYPE_BOOL, "Show all including system ones", "No"}
    ,
    {NULL}
};

shr_error_e
sys_usage_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sand_control->sh_sand_cmd_a)
    {
        sh_sand_usage_branch(unit, sand_control->sh_sand_cmd_a, sand_control);
    }
    else if (sand_control->sh_sand_cmd)
    {
        sh_sand_usage_leaf(unit, sand_control->sh_sand_cmd, sand_control);
    }

    SHR_FUNC_EXIT;
}

static sh_sand_man_t sys_exec_man = {
    "Perform all commands under the node",
    "Perform all commands under the node, excluding the ones that have options without defaults",
    "[dnx] command [command stack] exec",
    "fld exec"
};

sh_sand_option_t sys_exec_options[] = {
    {"all", SAL_FIELD_TYPE_BOOL, "Execute all shell commands ignoring errors", "No"},
    {NULL}
};

static shr_error_e
cmd_shell_exec_single(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_test_t * sh_sand_test,
    prt_control_t * prt_ctr)
{
    char command_str[SH_SAND_MAX_TOKEN_SIZE];
    char command_only[RHSTRING_MAX_SIZE];
    sal_usecs_t usec;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /*
     * Create actual command line
     */
    /*
     * Start from dnx
     */
    command_str[SH_SAND_MAX_TOKEN_SIZE - 1] = 0;
    sal_strncpy(command_str, "dnx ", SH_SAND_MAX_TOKEN_SIZE - 1);
    /*
     * Add all command sequence
     */
    sh_sand_cmd_fetch(sh_sand_cmd->ctr.command_list, command_only);
    sal_strncpy(command_str + strlen(command_str), command_only, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(command_str));

    PRT_CELL_SET("%s", command_only);
    /*
     * Add options if any
     */
    usec = sal_time_usecs();

    if (sh_sand_test != NULL)
    {
        PRT_CELL_SET("%s", RHNAME(sh_sand_test));
        sal_strncpy(command_str + strlen(command_str), " ", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(command_str));
        sal_strncpy(command_str + strlen(command_str), sh_sand_test->params,
                    SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(command_str));
        PRT_CELL_SET("%s", sh_sand_test->params);
        if (sh_process_command(unit, command_str) != CMD_OK)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            PRT_CELL_SET("FAIL");
        }
        else
        {
            PRT_CELL_SET("OK");
        }
    }
    else
    {
        shr_error_e shr_err;
        PRT_CELL_SET("%s", CTEST_DEFAULT);
        PRT_CELL_SET("%s", "N/A");

        if (sh_sand_cmd->flags & SH_CMD_SKIP_EXEC)
        {
            PRT_CELL_SET("%s", "SKIP");
        }
        else if (SHR_FAILURE
                 (sh_sand_option_list_process
                  (unit, args, sh_sand_cmd->ctr.stat_args_list, sh_sand_cmd->flags, sh_sand_cmd)))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            PRT_CELL_SET("%s", "Options Error");
        }
        else
        {
            /*
             * Even if there is an error, we are not exiting with error to give a chance to all commands to be executed
             * At least there is an option to complete entire tree
             */
            shr_err = sh_sand_cmd->action(unit, args, &sh_sand_cmd->ctr);
            SHR_SET_CURRENT_ERR(shr_err);
            PRT_CELL_SET("%s", shrextend_errmsg_get(shr_err));
        }
    }

    usec = sal_time_usecs() - usec;
    PRT_CELL_SET("%u", usec);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_shell_exec(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int ignore_error)
{
    sh_sand_test_t *sh_sand_test;
    prt_control_t *prt_ctr = (prt_control_t *) flex_ptr;
    shr_error_e cur_error;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if there is list, if not we assume there only one test without parameters
     */
    if (sh_sand_cmd->ctr.test_list == NULL)
    {
        cur_error = cmd_shell_exec_single(unit, args, sh_sand_cmd, NULL, prt_ctr);
        if (ignore_error == FALSE)
        {
            SHR_CLI_EXIT_IF_ERR(cur_error, "");
        }
    }
    else
    {
        RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
        {
            cur_error = cmd_shell_exec_single(unit, args, sh_sand_cmd, sh_sand_test, prt_ctr);
            if (ignore_error == FALSE)
            {
                SHR_CLI_EXIT_IF_ERR(cur_error, "");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sys_exec_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ignore_error;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_BOOL("all", ignore_error);

    PRT_TITLE_SET("%s", "Shell Command Results");
    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Parameters");
    PRT_COLUMN_ADD("Status");
    PRT_COLUMN_ALIGN;
    PRT_COLUMN_ADD("Time(usec)");
    if (sand_control->sh_sand_cmd_a)
    {
        SHR_SET_CURRENT_ERR(sh_sand_cmd_traverse
                            (unit, NULL, sand_control->sh_sand_cmd_a, cmd_shell_exec, (void *) prt_ctr, ignore_error));
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_SET_CURRENT_ERR(cmd_shell_exec(unit, NULL, sand_control->sh_sand_cmd, (void *) prt_ctr, ignore_error));
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_sand_sys_cmds[] = {
    {"exec", sys_exec_cmd, NULL, sys_exec_options, &sys_exec_man},
    {"usage", sys_usage_cmd, NULL, sys_usage_options, &sys_usage_man},
    {"help", sys_usage_cmd, NULL, sys_usage_options, &sys_usage_man},
    {NULL}
};
