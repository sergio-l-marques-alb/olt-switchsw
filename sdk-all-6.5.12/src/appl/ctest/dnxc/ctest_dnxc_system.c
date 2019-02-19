/**
 * \file diag_dnxc_ctest.c
 *
 * Framework add-onn for ctest developement
 *
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include "appl/ctest/dnxc/ctest_dnxc_system.h"

#include <sal/appl/sal.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/**
* \brief
* List all defined tests for node or leaf
*****************************************************/
static shr_error_e
cmd_ctest_list(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int flags)
{
    sh_sand_test_t *sh_sand_test;
    int first_line = TRUE;
    prt_control_t *prt_ctr = (prt_control_t *) flex_ptr;

    char command_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    sh_sand_cmd_fetch(sh_sand_cmd->ctr.command_list, command_str);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", command_str);
    if (sh_sand_cmd->ctr.test_list == NULL)
    {
        if (!(flags & CTEST_PRECOMMIT) || (sh_sand_cmd->flags & CTEST_PRECOMMIT))
        {
            PRT_CELL_SET(CTEST_DEFAULT);
            PRT_CELL_SET("N/A");
            if (sh_sand_cmd->flags & CTEST_PRECOMMIT)
            {
                PRT_CELL_SET("PRECOMMIT");
            }
        }
        SHR_EXIT();
    }
    RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
    {
        if (((flags & CTEST_PRECOMMIT) == TRUE) && !(sh_sand_test->flags & CTEST_PRECOMMIT))
            continue;

        if (first_line == TRUE)
        {
            first_line = FALSE;
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(1);
        }
        if (!ISEMPTY(RHNAME(sh_sand_test)))
        {
            PRT_CELL_SET("%s", RHNAME(sh_sand_test));
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
        if (!ISEMPTY(sh_sand_test->params))
        {
            PRT_CELL_SET("%s", sh_sand_test->params);
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
        /*
         * Tests with no parameters(defaults) are automatically included in precommit
         */
        if (sh_sand_test->flags & CTEST_PRECOMMIT)
        {
            PRT_CELL_SET("PRECOMMIT");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#if !defined(NO_FILEIO)
/*
 * No export facility without file system support
 * {
 */
static void
cmd_ctest_fill_test_name(
    char *test_name,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_test_t * sh_sand_test)
{
    rhhandle_t rhhandle;

    sal_strncpy(test_name, "AT_Dnx_CTest_", SH_SAND_MAX_TOKEN_SIZE - 1);
    /*
     * Now append entire command chain
     */
    RHITERATOR(rhhandle, sh_sand_cmd->ctr.command_list)
    {
        sal_strncat(test_name, RHNAME(rhhandle), SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(test_name));
        sal_strncat(test_name, "_", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(test_name));
    }
    /*
     * Finally append test name
     */
    if (sh_sand_test != NULL)
        sal_strncat(test_name, RHNAME(sh_sand_test), SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(test_name));
    else
        sal_strncat(test_name, CTEST_DEFAULT, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(test_name));

    utilex_str_escape(test_name, '_');

    return;
}

static shr_error_e
cmd_ctest_export_tlist_add_test(
    void *file_in,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_test_t * sh_sand_test)
{
    char teststr[SH_SAND_MAX_TOKEN_SIZE];
    char test_name[SH_SAND_MAX_TOKEN_SIZE];

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Obtain test name
     */
    cmd_ctest_fill_test_name(test_name, sh_sand_cmd, sh_sand_test);
    /*
     * All tests start from the standard preamble
     */
    teststr[SH_SAND_MAX_TOKEN_SIZE - 1] = 0;
    sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE - 1, "test_list_add ResetOnFail::%s\n", test_name);

    /*
     * Record full line in the file, whose handle is delivered by flex_ptr
     */
    dbx_file_write(file_in, teststr);

    SHR_FUNC_EXIT;
}

/**
* \brief
*   Write List of tests to .tlist file
**/
static shr_error_e
cmd_ctest_export_tlist(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *file_in,
    int flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if there is list, if not we assume there only one test without parameters
     */
    if (sh_sand_cmd->ctr.test_list == NULL)
    {
        if (!(flags & CTEST_PRECOMMIT) || (sh_sand_cmd->flags & CTEST_PRECOMMIT))
        {
            cmd_ctest_export_tlist_add_test(file_in, sh_sand_cmd, NULL);
        }
    }
    else
    {
        sh_sand_test_t *sh_sand_test;

        RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
        {
            /*
             * If export is for pre-commit only, include only tests with CTEST_PRECOMMIT
             */
            if (((flags & CTEST_PRECOMMIT) == TRUE) && !(sh_sand_test->flags & CTEST_PRECOMMIT))
                continue;

            cmd_ctest_export_tlist_add_test(file_in, sh_sand_cmd, sh_sand_test);
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_ctest_export_tcl_add_test(
    void *file_in,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_test_t * sh_sand_test)
{
    char command_only[RHSTRING_MAX_SIZE];
    char teststr[SH_SAND_MAX_TOKEN_SIZE];
    char test_name[SH_SAND_MAX_TOKEN_SIZE];

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Line 1 - Create separation empty line - Test Prolog
     */
    dbx_file_write(file_in, "\n");
    /*
     * Obtain test name
     */
    cmd_ctest_fill_test_name(test_name, sh_sand_cmd, sh_sand_test);
    /*
     * Line 2 - Procedure Name
     */
    teststr[SH_SAND_MAX_TOKEN_SIZE - 1] = 0;
    sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE - 1, "proc  ResetOnFail::%s", test_name);
    /*
     * Append line ending
     */
    sal_strncat(teststr, " {chip unit} {\n", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));
    /*
     * Record full line in the file, whose handle is delivered by file_in
     */
    dbx_file_write(file_in, teststr);
    /*
     * Line 3 - Create actual command line
     */
    sal_strncpy(teststr, "\tbcm shell \"ctest ", SH_SAND_MAX_TOKEN_SIZE - 1);
    sh_sand_cmd_fetch(sh_sand_cmd->ctr.command_list, command_only);
    sal_strncpy(teststr + strlen(teststr), command_only, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));
    if (sh_sand_test != NULL)
    {
        sal_strncpy(teststr + strlen(teststr), " ", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));
        sal_strncpy(teststr + strlen(teststr), sh_sand_test->params, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));
    }
    sal_strncpy(teststr + strlen(teststr), "\"\n", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));
    dbx_file_write(file_in, teststr);
    /*
     * Line 4,5 Procedure Ending
     */
    dbx_file_write(file_in, "\treturn PASS\n");
    dbx_file_write(file_in, "}\n");

    SHR_FUNC_EXIT;
}

/**
* \brief
*   Write TCL implementing DVAPI to .tcl file
**/
static shr_error_e
cmd_ctest_export_tcl(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *file_in,
    int flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd->ctr.test_list == NULL)
    {
        if (!(flags & CTEST_PRECOMMIT) || (sh_sand_cmd->flags & CTEST_PRECOMMIT))
        {
            cmd_ctest_export_tcl_add_test(file_in, sh_sand_cmd, NULL);
        }
    }
    else
    {
        sh_sand_test_t *sh_sand_test;

        RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
        {
            /*
             * If export is for pre-commit only, include only tests with CTEST_PRECOMMIT
             */
            if (((flags & CTEST_PRECOMMIT) == TRUE) && !(sh_sand_test->flags & CTEST_PRECOMMIT))
                continue;

            cmd_ctest_export_tcl_add_test(file_in, sh_sand_cmd, sh_sand_test);
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
sys_ctest_export_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    void *file;
    char *filename;
    char filepath[RHFILE_MAX_SIZE];
    int flags = 0, precommit_flag;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("file", filename);
    if (ISEMPTY(filename))
    {
        filename = "testDvapiJer2Ctest.tlist";
    }
    SH_SAND_GET_BOOL("pre", precommit_flag);
    if (precommit_flag == TRUE)
        flags |= CTEST_PRECOMMIT;

    /*
     * Generate TLIST file with list of all DVAPIs
     */
    dbx_file_get_sdk_path(filename, "/regress/bcm/tests/dnx/", filepath);
    if ((file = dbx_file_open(filepath)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Cannot open:%s\n", filepath);
    }
    dbx_file_write(file,
                   "#############################################################################################\n");
    dbx_file_write(file,
                   "# This file contains DVAPI tests for Jericho-II that should be run in the pre commit process#\n");
    dbx_file_write(file,
                   "#############################################################################################\n");
    dbx_file_write(file,
                   "# DO not edit it, generated by running \"ctest export\" from bcm shell                        #\n");
    dbx_file_write(file,
                   "# $Id$                                                                                      #\n");
    dbx_file_write(file,
                   "# $Copyright: (c) 2015 Broadcom Corp.                                                       #\n");
    dbx_file_write(file,
                   "# All Rights Reserved.$                                                                     #\n");
    dbx_file_write(file,
                   "#############################################################################################\n");

    if (sand_control->sh_sand_cmd_a)
    {
        sh_sand_cmd_traverse(unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_export_tlist, (void *) file, flags);
    }
    else if (sand_control->sh_sand_cmd)
    {
        cmd_ctest_export_tlist(unit, args, sand_control->sh_sand_cmd, (void *) file, flags);
    }
    dbx_file_close(file);
    /*
     * Generate TCL file with ALL DVAPI
     */
    filename = "CTestDvapis.tcl";

    dbx_file_get_sdk_path(filename, "/regress/bcm/tests/dnx/ctest/", filepath);
    if ((file = dbx_file_open(filepath)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Cannot open:%s\n", filepath);
    }
    dbx_file_write(file,
                   "#############################################################################################\n");
    dbx_file_write(file,
                   "# This file contains DVAPI tests for Jericho-II that should be run in the pre commit process#\n");
    dbx_file_write(file,
                   "#############################################################################################\n");
    dbx_file_write(file,
                   "# DO not edit it, generated by running \"ctest export\" from bcm shell                        #\n");
    dbx_file_write(file,
                   "# $Id$                                                                                      #\n");
    dbx_file_write(file,
                   "# $Copyright: (c) 2015 Broadcom Corp.                                                       #\n");
    dbx_file_write(file,
                   "# All Rights Reserved.$                                                                     #\n");
    dbx_file_write(file,
                   "#############################################################################################\n");
    dbx_file_write(file, "\npackage require cints_Dvapi_utils\n");

    if (sand_control->sh_sand_cmd_a)
    {
        sh_sand_cmd_traverse(unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_export_tcl, (void *) file, flags);
    }
    else if (sand_control->sh_sand_cmd)
    {
        cmd_ctest_export_tcl(unit, args, sand_control->sh_sand_cmd, (void *) file, flags);
    }
    dbx_file_close(file);

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
#endif /* !defined NO_FILEIO */

static shr_error_e
sys_ctest_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int flags = 0, precommit_flag;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_BOOL("pre", precommit_flag);
    if (precommit_flag == TRUE)
        flags |= CTEST_PRECOMMIT;

    PRT_TITLE_SET("%s", "CTests List");
    PRT_COLUMN_ADD("Test Command");
    PRT_COLUMN_ADD("Test Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Test Parameters");
    PRT_COLUMN_ADD("Flags");
    if (sand_control->sh_sand_cmd_a)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse
                            (unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_list, (void *) prt_ctr, flags),
                            "CTest node list failure\n");
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_CLI_EXIT_IF_ERR(cmd_ctest_list(unit, args, sand_control->sh_sand_cmd, (void *) prt_ctr, flags),
                            "CTest leaf list failure\n");
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_ctest_run_test(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_test_t * sh_sand_test,
    prt_control_t * prt_ctr,
    int flags)
{
    char command_str[SH_SAND_MAX_TOKEN_SIZE];
    char command_only[RHSTRING_MAX_SIZE];
    sal_usecs_t usec;
    cmd_result_t rv;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /*
     * Create actual command line
     */
    /*
     * Start from ctest
     */
    command_str[SH_SAND_MAX_TOKEN_SIZE - 1] = 0;
    sal_strncpy(command_str, "ctest ", SH_SAND_MAX_TOKEN_SIZE - 1);
    /*
     * Add all command sequence
     */
    sh_sand_cmd_fetch(sh_sand_cmd->ctr.command_list, command_only);
    sal_strncpy(command_str + strlen(command_str), command_only, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(command_str));

    PRT_CELL_SET("%s", command_only);
    /*
     * Add options if any
     */
    if (sh_sand_test != NULL)
    {
        PRT_CELL_SET("%s", RHNAME(sh_sand_test));
        sal_strncpy(command_str + strlen(command_str), " ", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(command_str));
        sal_strncpy(command_str + strlen(command_str), sh_sand_test->params,
                    SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(command_str));
        PRT_CELL_SET("%s", sh_sand_test->params);
    }
    else
    {
        PRT_CELL_SET("%s", CTEST_DEFAULT);
        PRT_CELL_SET("%s", "N/A");
    }

    usec = sal_time_usecs();

    if (flags & CTEST_RUN_LOG)
    {
        LOG_CLI((BSL_META("Test:%s\n"), command_str));
    }

    if ((rv = sh_process_command(unit, command_str)) != CMD_OK)
    {
        PRT_CELL_SET("FAIL");
        if (flags & CTEST_RUN_LOG)
        {
            LOG_CLI((BSL_META("\tResult(fail:%d)\n"), rv));
        }
    }
    else
    {
        PRT_CELL_SET("OK");
        if (flags & CTEST_RUN_LOG)
        {
            LOG_CLI((BSL_META("\tResult(pass)\n")));
        }
    }

    usec = sal_time_usecs() - usec;
    PRT_CELL_SET("%u", usec);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Run all tests
**/
static shr_error_e
cmd_ctest_run(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    void *flex_ptr,
    int flags)
{
    sh_sand_test_t *sh_sand_test;
    prt_control_t *prt_ctr = (prt_control_t *) flex_ptr;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if there is list, if not we assume there only one test without parameters
     */
    if (sh_sand_cmd->ctr.test_list == NULL)
    {
        if (!(flags & CTEST_PRECOMMIT) || (sh_sand_cmd->flags & CTEST_PRECOMMIT))
        {
            cmd_ctest_run_test(unit, sh_sand_cmd, NULL, prt_ctr, flags);
        }
    }
    else
    {
        RHITERATOR(sh_sand_test, sh_sand_cmd->ctr.test_list)
        {
            /*
             * If run is for pre-commit only, execute only tests with CTEST_PRECOMMIT
             */
            if (((flags & CTEST_PRECOMMIT) == TRUE) && !(sh_sand_test->flags & CTEST_PRECOMMIT))
                continue;

            cmd_ctest_run_test(unit, sh_sand_cmd, sh_sand_test, prt_ctr, flags);
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
sys_ctest_run_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int flags = 0, precommit_flag, log_flag;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_BOOL("pre", precommit_flag);
    SH_SAND_GET_BOOL("logger", log_flag);
    if (precommit_flag == TRUE)
        flags |= CTEST_PRECOMMIT;

    if (log_flag == TRUE)
        flags |= CTEST_RUN_LOG;

    PRT_TITLE_SET("%s", "CTests Results");
    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Parameters");
    PRT_COLUMN_ADD("Status");
    PRT_COLUMN_ALIGN;
    PRT_COLUMN_ADD("Time(usec)");
    if (sand_control->sh_sand_cmd_a)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse
                            (unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_run, (void *) prt_ctr, flags),
                            "CTest node run failure\n");
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_CLI_EXIT_IF_ERR(cmd_ctest_run(unit, args, sand_control->sh_sand_cmd, (void *) prt_ctr, flags),
                            "CTest leaf run failure\n");
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t sys_ctest_export_man = {
    "Generate tlist and tcl code for ctest",
    "Command generates tlist and tcl code for all ctest starting from the last command before export\n"
        "$SDK/regress/bcm/tests/dnx/ctest/CTestDvapis.tcl and $SDK/regress/bcm/tests/dnx/testDvapiJer2Ctest.tlist",
    "ctest [commands] export [pre]",
    "ct export pre\n" "ct fld export\n" "ct mdb export"
};

static sh_sand_man_t sys_ctest_list_man = {
    "List available ctests",
    "List ctests for all commands starting from last branch on command line",
    "ctest [commands] list [pre]",
    "ct list\n" "ct tm list pre"
};

static sh_sand_man_t sys_ctest_run_man = {
    "Execute ctests",
    "Execute ctests for all commands starting from last branch on command line",
    "ctest [commands] run [pre]",
    "ct run\n" "ct tm run pre"
};

static sh_sand_option_t sys_ctest_options[] = {
    {"logger", SAL_FIELD_TYPE_BOOL, "Print Start/End Markers  with result status", "No"},
    {"pre", SAL_FIELD_TYPE_BOOL, "Export only tests with precommit flag", "No"},
    {NULL}
};

sh_sand_cmd_t sh_dnxc_sys_ctest_cmds[] = {
#if !defined(NO_FILEIO)
    {"export", sys_ctest_export_cmd, NULL, sys_ctest_options, &sys_ctest_export_man},
#endif
    {"list", sys_ctest_list_cmd, NULL, sys_ctest_options, &sys_ctest_list_man},
    {"run", sys_ctest_run_cmd, NULL, sys_ctest_options, &sys_ctest_run_man},
    {"usage", sys_usage_cmd, NULL, sys_usage_options, &sys_usage_man},
    {NULL}
};
