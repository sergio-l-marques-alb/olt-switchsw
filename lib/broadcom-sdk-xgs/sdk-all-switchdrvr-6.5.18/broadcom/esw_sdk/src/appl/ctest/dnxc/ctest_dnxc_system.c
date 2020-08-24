/**
 * \file ctest_dnxc_ctest.c
 *
 * Framework add-on for ctest development
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc_is_xxx*/
#include "appl/ctest/dnxc/ctest_dnxc_system.h"

#include <sal/appl/sal.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_system.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager.h>
#endif

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * If general flag is to skip and command is marked as skip - skip
 * If random on command flag does not match random per test - skip
 * If run is for pre-commit only, execute only tests with CTEST_PRECOMMIT
 */
#define CTEST_ELIGIBLE(cmd_flags, test_flags) \
                    (( !((cmd_flags & SH_CMD_SKIP_EXEC) && (test_flags & SH_CMD_SKIP_EXEC)) &&           \
                      ((cmd_flags & CTEST_RANDOM) == (test_flags & CTEST_RANDOM))          &&           \
                      (!(cmd_flags & CTEST_PRECOMMIT) || (test_flags & CTEST_PRECOMMIT)) ) || (test_flags & CTEST_PASS))

shr_error_e
sh_sand_test_add(
    int unit,
    rhlist_t * test_list,
    char *test_name,
    char *test_params,
    int flags)
{
    sh_sand_test_t *test_invoke;
    rhhandle_t void_test;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(test_list, _SHR_E_PARAM, "test_list");

    if (utilex_rhlist_entry_add_tail(test_list, test_name, RHID_TO_BE_GENERATED, &void_test) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add test:\"%s\" to the test list\n", test_name);
    }
    test_invoke = void_test;
    if ((test_invoke->params = sal_alloc(sal_strlen(test_params) + 1, "ShellInvokeParams")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Error - failed to allocate memory for test params on:\"%s\"\n", test_name);
    }
    sal_strncpy(test_invoke->params, test_params, sal_strlen(test_params) + 1);
    test_invoke->flags = flags;

exit:
    SHR_FUNC_EXIT;
}

static void
cmd_ctest_list_free(
    int unit,
    rhlist_t * test_list)
{
    sh_sand_test_t *test_invoke;
    if (test_list == NULL)
    {
        /*
         * List was never created or freed already
         */
        return;
    }
    /*
     * Go through each element and free allocations
     */
    RHITERATOR(test_invoke, test_list)
    {
        if (test_invoke->params != NULL)
        {
            sal_free(test_invoke->params);
        }
    }
    utilex_rhlist_free_all(test_list);
}

static shr_error_e
cmd_ctest_list_create(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd,
    rhlist_t ** test_list_p,
    int flags)
{
    sh_sand_invoke_t *invoke;
    rhlist_t *test_list = NULL;
    rhhandle_t void_test;
    sh_sand_test_t *test_instance_p;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sh_sand_cmd, _SHR_E_PARAM, "sh_sand_cmd");
    SHR_NULL_CHECK(test_list_p, _SHR_E_PARAM, "test_list_p");

    /*
     * Do not deal with commands marked by SKIP in case flags have this flag
     */
    if ((sh_sand_cmd->flags & SH_CMD_SKIP_EXEC) && (flags & SH_CMD_SKIP_EXEC))
    {
        SHR_EXIT();
    }

    if ((test_list = utilex_rhlist_create("ShellInvokes", sizeof(sh_sand_test_t), 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Failed to allocate list for test invocations on:%s\n",
                     sh_sand_cmd->keyword);
    }
    /*
     * Add all tests from static invocation array attached to the command, No such array - no tests added to the list
     */
    for (invoke = sh_sand_cmd->invokes; (invoke != NULL) && (invoke->name != NULL); invoke++)
    {
        if (!CTEST_ELIGIBLE(flags, invoke->flags))
        {
            continue;
        }

        if (utilex_rhlist_entry_add_tail(test_list, invoke->name, RHID_TO_BE_GENERATED, &void_test) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add test:'%s' for command:'%s'\n"
                         "Test Name should be unique per command\n", invoke->name, sh_sand_cmd->cmd_only);
        }
        test_instance_p = void_test;
        test_instance_p->flags = invoke->flags;
        if (!ISEMPTY(invoke->params))
        {
            int param_len = sal_strlen(invoke->params);
            if ((test_instance_p->params = sal_alloc(param_len + 1, "ShellInvokeParams")) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "Error - failed to allocate memory for test params on:\"%s\" to the list\n",
                             invoke->name);
            }
            sal_strncpy(test_instance_p->params, invoke->params, param_len);
            test_instance_p->params[param_len] = 0;
        }
    }
    /*
     * Before callback and default check command eligibility
     */
    if (CTEST_ELIGIBLE(flags, sh_sand_cmd->flags))
    {
        /*
         * Run invoke callback if exists and it is not accompanied by SH_CMD_CONDITIONAL, which marks that this
         * callback is for command validation purpose rather than list creation
         */
        if ((sh_sand_cmd->invoke_cb != NULL) && !(sh_sand_cmd->flags & SH_CMD_CONDITIONAL))
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_cmd->invoke_cb(unit, test_list), "");
            {
                sh_sand_test_t *sh_sand_test;
                RHSAFE_ITERATOR(sh_sand_test, test_list)
                {
                    /*
                     * Remove all tests no matching command flags
                     */
                    if (!CTEST_ELIGIBLE(flags, sh_sand_test->flags))
                    {
                        utilex_rhlist_entry_del_free(test_list, sh_sand_test);
                    }
                }
            }
        }
        else
        {
            /*
             * If there were neither invokes nor callback - create single default one with no options
             * No invokes - eoither NULL pointer or first entry is empty
             */
            if ((sh_sand_cmd->invokes == NULL) || ISEMPTY(sh_sand_cmd->invokes->name))
            {
                if (utilex_rhlist_entry_add_tail(test_list, CTEST_DEFAULT, RHID_TO_BE_GENERATED, &void_test) !=
                    _SHR_E_NONE)
                {
                    SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add test:\"%s\"\n"
                                 "Verify that Test Name is unique per command\n", CTEST_DEFAULT);
                }
                test_instance_p = void_test;
                test_instance_p->flags = sh_sand_cmd->flags;
            }
        }
    }

    *test_list_p = test_list;

exit:
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        cmd_ctest_list_free(unit, test_list);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * List all defined tests for node or leaf
 * \param [in] unit         - device id
 * \param [in] args         - pointer to standard bcm shell argument structure, used by parse.h MACROS and routines
 * \param [in] sh_sand_cmd  - pointer to command for which tests will be listed
 * \param [in] flex_ptr     - free pinter in traverse routine, here is used as prt_ctr for tabular printing
 * \param [in] flags        - flags from command invocation
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval other errors as per shr_error_e
 * \remark
 *  Command usually used, but not limited to, as traverse callback
 */
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
    rhlist_t *test_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Include SKIPPED tests
     */
    SHR_CLI_EXIT_IF_ERR(cmd_ctest_list_create(unit, sh_sand_cmd, &test_list, flags),
                        "Failed to create test list for:'%s'\n", sh_sand_cmd->cmd_only);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", sh_sand_cmd->cmd_only);
    RHITERATOR(sh_sand_test, test_list)
    {
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
         * Assign status for precommit
         */
        if (sh_sand_test->flags & CTEST_PRECOMMIT)
        {
            PRT_CELL_SET("PRECOMMIT");
        }
        else if (sh_sand_test->flags & CTEST_POSTCOMMIT)
        {
            PRT_CELL_SET("POSTCOMMIT");
        }
        /*
         * Or SKIPPed tests
         */
        else if ((sh_sand_cmd->flags & SH_CMD_SKIP_EXEC) || (sh_sand_test->flags & SH_CMD_SKIP_EXEC))
        {
            PRT_CELL_SET("SKIP");
        }
    }

exit:
    cmd_ctest_list_free(unit, test_list);
    SHR_FUNC_EXIT;
}

#if !defined(NO_FILEIO)
/*
 * No export facility without file system support
 * {
 */
/**
 * \brief
 * List all defined tests for node or leaf
 * \param [in] test_name    - pointer to allocated space where resulting test name will be formed
 * \param [in] sh_sand_cmd  - pointer to command for which tests will be invoked
 * \param [in] sh_sand_test - specific test from the list of the ones attached to the command
 *
 * \remark
 *     Routine does not return the failure - if something went wrong test will fail
 */
static void
cmd_ctest_fill_test_name(
    char *test_name,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_test_t * sh_sand_test)
{
    char **cmd_tokens = NULL;
    uint32 cmd_num = 0;
    int cmd_id;
    /*
     * Currently all tests wil start from Dnx - may be we'll need to tune it to Dnxf/Dnx
     */
    sal_strncpy(test_name, "AT_Dnx_", SH_SAND_MAX_TOKEN_SIZE - 1);
    /*
     * Each command contain space separated command line, so we turn it into test name
     * Now append entire command chain
     */
    if ((cmd_tokens = utilex_str_split(sh_sand_cmd->cmd_only, " ", SH_SAND_MAX_COMMAND_DEPTH, &cmd_num)) == NULL)
    {
        return;
    }
    /*
     * Add all tokens following by undersacore
     */
    for (cmd_id = 0; cmd_id < cmd_num; cmd_id++)
    {
        sal_strncat(test_name, cmd_tokens[cmd_id], SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(test_name));
        sal_strncat(test_name, "_", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(test_name));
    }
    /*
     * Finally append test name, if no test name was provided use default
     */
    if (sh_sand_test != NULL)
        sal_strncat(test_name, RHNAME(sh_sand_test), SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(test_name));
    else
        sal_strncat(test_name, CTEST_DEFAULT, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(test_name));
    /*
     * Replace all non-compliant characters in the test name by underscore
     */
    utilex_str_escape(test_name, '_');

    return;
}

/**
* \brief
*       Add 1 test to .tlist file
 * \param [in] fiel_in - Tlist file handle, no additional verification is performed due to static character of call
 * \param [in] sh_sand_cmd  - pointer to command for which tests will be invoked
 * \param [in] sh_sand_test - specific test from the list of the ones attached to the command
 *
 * \remark
 *     Actually routine does not return the failure - if something went wrong test will fail
 */
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
    sh_sand_test_t *sh_sand_test;
    rhlist_t *test_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Exclude SKIPPED tests
     */
    flags |= SH_CMD_SKIP_EXEC;
    SHR_CLI_EXIT_IF_ERR(cmd_ctest_list_create(unit, sh_sand_cmd, &test_list, flags),
                        "Failed to create test list for:'%s'\n", sh_sand_cmd->cmd_only);

    RHITERATOR(sh_sand_test, test_list)
    {
        cmd_ctest_export_tlist_add_test(file_in, sh_sand_cmd, sh_sand_test);
    }

exit:
    cmd_ctest_list_free(unit, test_list);
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_ctest_export_tcl_add_test_doc_str(
    char *teststr,
    const char *title,
    const char *content)
{
    /*
     * buffer to hold each line of sub_topic.
     */
    char *token_buf = NULL;

    /*
     * dummy buffer used by strktok
     */
    char *dummy_buf = NULL;

    /*
     * saveptr used by reentrant strtok
     */
    char *save_ptr = NULL;

    /*
     * need to copy the contents, since strtok_r can modify contents
     */
    char tmp_content[SH_SAND_MAX_TOKEN_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(NO_UNIT);

    if (title != NULL)
    {
        sal_snprintf(teststr + strlen(teststr), SH_SAND_MAX_TOKEN_SIZE - 1, "#\\%s\n", title);
    }

    if (content != NULL)
    {
        sal_strncpy(tmp_content, content, SH_SAND_MAX_TOKEN_SIZE);

        dummy_buf = tmp_content;
        while ((token_buf = sal_strtok_r(dummy_buf, "\n", &save_ptr)) != NULL)
        {
            dummy_buf = NULL;
            sal_snprintf(teststr + strlen(teststr), SH_SAND_MAX_TOKEN_SIZE - 1, "# %s\n", token_buf);
        }
    }
    else
    {
        sal_snprintf(teststr + strlen(teststr), SH_SAND_MAX_TOKEN_SIZE - 1, "#\n");
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - add test documentation
 */
static shr_error_e
cmd_ctest_export_tcl_add_test_doc(
    sh_sand_cmd_t * sh_sand_cmd,
    char *teststr)
{
    char begin_doc[] = "##\n";

    const char *sub_feature_contents = NULL;
    const char *proc_contents = NULL;
    const char *result_contents = NULL;
    const char *note_contents = NULL;
    const char *type_contents = NULL;
    const char *priority_contents = NULL;
    const char *brief_contents = NULL;
    const char *bug_contents = NULL;

    sh_sand_ctest_doc_t *doc = sh_sand_cmd->doc;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Try to retrieve data if specified
     */
    if (doc != NULL)
    {
        sub_feature_contents = doc->sub_feature;
        proc_contents = doc->procedures;
        result_contents = doc->result;
        note_contents = doc->note;
        bug_contents = doc->bug;
        brief_contents = doc->brief;

        switch (doc->type)
        {
            case SHR_SAND_CTEST_TYPE_SEMANTIC:
                type_contents = "SEMANTIC";
                break;
            case SHR_SAND_CTEST_TYPE_FUNCTIONAL:
                type_contents = "FUNCTIONAL";
                break;
            default:
                type_contents = NULL;
        }

        switch (doc->priority)
        {
            case 1:
                priority_contents = "1";
                break;
            case 2:
                priority_contents = "2";
                break;
            default:
                priority_contents = NULL;
        }
    }

    /*
     * Try to retrieve data from man, if exists
     */
    if (brief_contents == NULL && sh_sand_cmd->man != NULL)
    {
        brief_contents = sh_sand_cmd->man->brief;
    }

    /*
     * Begin comment
     */
    sal_strncpy(teststr, begin_doc, SH_SAND_MAX_TOKEN_SIZE - 1);

    /*
     * Sub_topic
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "Sub_topic", sub_feature_contents);

    /*
     * Test_type
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "Test_type", type_contents);

    /*
     * Priority
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "Priority", priority_contents);

    /*
     * brief
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "brief", brief_contents);

    /*
     * bug
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "bug", bug_contents);

    /*
     * Test_Procedures
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "Test_Procedures", proc_contents);

    /*
     * result
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "result", result_contents);

    /*
     * note
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "note", note_contents);

    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_ctest_export_tcl_add_test(
    void *file_in,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_test_t * sh_sand_test)
{
    char teststr[SH_SAND_MAX_TOKEN_SIZE] = { 0 };
    char test_name[SH_SAND_MAX_TOKEN_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(NO_UNIT);

    /*
     * Create separation empty line - Test Prolog
     */
    dbx_file_write(file_in, "\n");

    /*
     * Obtain test name
     */
    cmd_ctest_fill_test_name(test_name, sh_sand_cmd, sh_sand_test);

    /*
     * Add documentation on top of test name
     */
    cmd_ctest_export_tcl_add_test_doc(sh_sand_cmd, teststr);

    /*
     * Procedure Name
     */
    sal_snprintf(teststr + sal_strlen(teststr), SH_SAND_MAX_TOKEN_SIZE - 1, "proc  ResetOnFail::%s", test_name);

    /*
     * Append line ending
     */
    sal_strncat(teststr, " {chip unit} {\n", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));

    /*
     * Record full line in the file, whose handle is delivered by file_in
     */
    dbx_file_write(file_in, teststr);
    /*
     * Add warmboot check if needed
     */
    if ((sh_sand_cmd->flags & SH_CMD_SKIP_TEST_WB) || (sh_sand_test->flags & SH_CMD_SKIP_TEST_WB))
    {
        dbx_file_write(file_in, "\tset is_wb_test_mode [bcm_dnxc_warmboot_test_mode_get $unit]\n");
        dbx_file_write(file_in,
                       "\tNTest::AssertNEq 1 $is_wb_test_mode NO_SUPPORT \"this test is not supported in WB or CR mode\"\n");
    }
    /*
     * Create actual command line
     */
    sal_strncpy(teststr, "\tbcm shell [AT_Dnx_CTest_Seeded_Command_Get \"", SH_SAND_MAX_TOKEN_SIZE - 1);
    sal_strncpy(teststr + strlen(teststr), sh_sand_cmd->cmd_only, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));
    if (sh_sand_test->params != NULL)
    {
        char escaped_params[RHSTRING_MAX_SIZE];
        sal_strncpy(teststr + strlen(teststr), " ", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));
        utilex_escape_quote(sh_sand_test->params, escaped_params, RHSTRING_MAX_SIZE);
        sal_strncpy(teststr + strlen(teststr), escaped_params, SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(teststr));
    }
    sal_snprintf(teststr + strlen(teststr), SH_SAND_MAX_TOKEN_SIZE - 1, "\" \"%s\"]\n", test_name);
    dbx_file_write(file_in, teststr);
    /*
     * Procedure Ending
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
    sh_sand_test_t *sh_sand_test;
    rhlist_t *test_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Exclude SKIPPED tests
     */
    flags |= SH_CMD_SKIP_EXEC;
    SHR_CLI_EXIT_IF_ERR(cmd_ctest_list_create(unit, sh_sand_cmd, &test_list, flags),
                        "Failed to create test list for:'%s'\n", sh_sand_cmd->cmd_only);

    RHITERATOR(sh_sand_test, test_list)
    {
        cmd_ctest_export_tcl_add_test(file_in, sh_sand_cmd, sh_sand_test);
    }

exit:
    cmd_ctest_list_free(unit, test_list);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Callback for ctest export command
 * \param [in] unit unit id
 * \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 * \param [in] ctr pointer to list of options processed by sand framework to be used with SH_SAND MACROS
 * \return
 *  \retval _SHR_E_NONE for success
 *  \retval _SHR_E_PARAM problem with input parameters, usage should be printed
 *  \retval other errors for other failure type
 * \remark
 *
 */
static shr_error_e
sys_ctest_export_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    void *file;
    char *filename;
    char *inputname;
    char filepath[RHFILE_MAX_SIZE];
    int flags = 0, precommit_flag, random_flag;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(filepath, 0x0, RHFILE_MAX_SIZE);

    SH_SAND_GET_STR("list", inputname);
    if (ISEMPTY(inputname))
    {
        filename = "testDvapiJer2Ctest";
    }
    else
    {
        filename = inputname;
    }
    SH_SAND_GET_BOOL("pre", precommit_flag);
    if (precommit_flag == TRUE)
        flags |= CTEST_PRECOMMIT;

    SH_SAND_GET_BOOL("random", random_flag);
    if (random_flag == TRUE)
        flags |= CTEST_RANDOM;

    /*
     * Generate TLIST file with list of all DVAPIs
     */
    dbx_file_get_sdk_path(filename, "/regress/bcm/tests/dnx/", filepath);
    sal_strncat(filepath, ".tlist", RHFILE_MAX_SIZE - sal_strlen(filepath) - 1);

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
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse(unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_export_tlist,
                                                 (void *) file, flags), "ctest export to tlist failed\n");
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_CLI_EXIT_IF_ERR(cmd_ctest_export_tlist(unit, args, sand_control->sh_sand_cmd,
                                                   (void *) file, flags), "ctest export to tlist failed\n");
    }
    dbx_file_close(file);
    /*
     * Generate TCL file with ALL DVAPI
     */
    if (ISEMPTY(inputname))
    {
        filename = "CTestDvapis";
    }
    else
    {
        filename = inputname;
    }

    dbx_file_get_sdk_path(filename, "/regress/bcm/tests/dnx/ctest/", filepath);
    sal_strncat(filepath, ".tcl", RHFILE_MAX_SIZE - sal_strlen(filepath) - 1);
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

    dbx_file_write(file, "\n# Get test seed from TCL framework and append to ctest command\n");
    dbx_file_write(file, "proc AT_Dnx_CTest_Seeded_Command_Get {ctest_cmd testName} {\n");
    dbx_file_write(file, "\tset seed [NTestSeedGet $testName]\n");
    dbx_file_write(file, "\tappend result $ctest_cmd \" seed=\" [format %s $seed]\n");
    dbx_file_write(file, "\treturn $result\n");
    dbx_file_write(file, "}\n");

    if (sand_control->sh_sand_cmd_a)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse(unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_export_tcl,
                                                 (void *) file, flags), "ctest export to tlist failed\n");
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_CLI_EXIT_IF_ERR(cmd_ctest_export_tcl(unit, args, sand_control->sh_sand_cmd,
                                                 (void *) file, flags), "ctest export to tlist failed\n");
    }
    dbx_file_close(file);

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
#endif /* !defined NO_FILEIO */

/**
 * \brief
 *      Callback for ctest list command
 * \param [in] unit unit id
 * \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 * \param [in] ctr pointer to list of options processed by sand framework to be used with SH_SAND MACROS
 * \return
 *  \retval _SHR_E_NONE for success
 *  \retval _SHR_E_PARAM problem with input parameters, usage should be printed
 *  \retval other errors for other failure type
 * \remark
 *
 */
static shr_error_e
sys_ctest_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int flags = 0, precommit_flag, random_flag;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("pre", precommit_flag);
    if (precommit_flag == TRUE)
        flags |= CTEST_PRECOMMIT;

    SH_SAND_GET_BOOL("random", random_flag);
    if (random_flag == TRUE)
        flags |= CTEST_RANDOM;

    PRT_TITLE_SET("%s", "CTests List");
    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Arguments");
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
    void *flex_ptr,
    int flags,
    int *core)
{
    args_t *leaf_args = NULL;
    char command_str[SH_SAND_MAX_TOKEN_SIZE];
    sal_usecs_t usec;
    char *str_next;
    shell_flex_t *shell_flex_p = (shell_flex_t *) flex_ptr;
    prt_control_t *prt_ctr;
    sh_sand_control_t sand_ctr_m;

    SHR_FUNC_INIT_VARS(unit);

    if ((shell_flex_p == NULL) || ((prt_ctr = shell_flex_p->prt_ctr) == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Memory Error");
    }

    sal_memset(&sand_ctr_m, 0, sizeof(sh_sand_control_t));

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /*
     * Create actual command line
     */
    /*
     * Add all command sequence
     */
    sal_strncpy(command_str, sh_sand_cmd->cmd_only, SH_SAND_MAX_TOKEN_SIZE - 1);

    PRT_CELL_SET("%s", sh_sand_cmd->cmd_only);
    PRT_CELL_SET("%s", RHNAME(sh_sand_test));
    /*
     * Add options if any
     */
    if (sh_sand_test->params != NULL)
    {
        sal_strncpy(command_str + strlen(command_str), " ", SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(command_str));
        sal_strncpy(command_str + strlen(command_str), sh_sand_test->params,
                    SH_SAND_MAX_TOKEN_SIZE - 1 - strlen(command_str));
        PRT_CELL_SET("%s", sh_sand_test->params);
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

    if (_SHR_IS_FLAG_SET(sh_sand_cmd->flags, SH_CMD_EXEC_PER_CORE))
    {
        PRT_CELL_SET("%d", *core);
    }
    else
    {
        PRT_CELL_SET("%s", "default");
    }

    if ((leaf_args = sal_alloc(sizeof(args_t), "leafs")) == NULL)
    {
        shell_flex_p->failed++;
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        PRT_CELL_SET("Memory Error");
        SHR_EXIT();
    }
    sal_memset(leaf_args, 0, sizeof(args_t));

    if (sh_sand_test->params != NULL)
    {
        if (diag_parse_args(sh_sand_test->params, &str_next, leaf_args))
        {       /* Parses up to ; or EOL */
            shell_flex_p->failed++;
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            PRT_CELL_SET("Shell Error");
        }
    }

    usec = sal_time_usecs();

    if (flags & SH_CMD_LOG)
    {
        LOG_CLI((BSL_META("Test:%s\n"), command_str));
    }

    SHR_SET_CURRENT_ERR(sh_sand_option_list_process
                        (unit, core, leaf_args, sh_sand_cmd, &sand_ctr_m, sh_sand_cmd->flags));
    if (SHR_FUNC_VAL_IS(_SHR_E_NONE)
        && ((SHR_SET_CURRENT_ERR(sh_sand_cmd->action(unit, leaf_args, &sand_ctr_m))) == _SHR_E_NONE))
    {
        shell_flex_p->succeeded++;
        if (flags & SH_CMD_LOG)
        {
            LOG_CLI((BSL_META("\tResult(pass)\n")));
        }
        if (flags & SH_CMD_FAILURE_ONLY)
        {
            /*
             * Do not record succeeded tests - only failed ones
             */
            PRT_ROW_DELETE();
            SHR_EXIT();
        }

        PRT_CELL_SET("OK");
    }
    else
    {
        shell_flex_p->failed++;
        PRT_CELL_SET("%s", shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
        if (flags & SH_CMD_LOG)
        {
            LOG_CLI((BSL_META("\tResult(fail) - %s\n"), shrextend_errmsg_get(SHR_GET_CURRENT_ERR())));
        }
    }

    usec = sal_time_usecs() - usec;
    PRT_CELL_SET("%u", usec);

exit:
    /*
     * Clean allocations from sand_ctr
     */
    sh_sand_option_list_clean(unit, &sand_ctr_m);
    /*
     * Free memory used for argument parsing
     */
    if (leaf_args != NULL)
    {
        sal_free(leaf_args);
    }
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
    rhlist_t *test_list = NULL;
    int core = 0;
    int nof_cores = 1;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Exclude SKIPPED tests
     */
    flags |= SH_CMD_SKIP_EXEC;
    SHR_CLI_EXIT_IF_ERR(cmd_ctest_list_create(unit, sh_sand_cmd, &test_list, flags),
                        "Failed to create test list for:'%s'\n", sh_sand_cmd->cmd_only);

    RHITERATOR(sh_sand_test, test_list)
    {

        if (_SHR_IS_FLAG_SET(sh_sand_cmd->flags, SH_CMD_EXEC_PER_CORE))
        {
            /*
             * if SH_CMD_EXEC_PER_CORE flag is set update the nof_cores.
             */
            SHR_CLI_EXIT_IF_ERR(sh_sand_nof_cores_get(unit, &nof_cores), "Failed to get the number of cores.");
        }

        for (core = 0; core < nof_cores; core++)
        {
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) && ((sh_sand_cmd->flags & SH_CMD_ROLLBACK) || (sh_sand_test->flags & SH_CMD_ROLLBACK)))
            {
                DNX_ROLLBACK_JOURNAL_START(unit);
                cmd_ctest_run_test(unit, sh_sand_cmd, sh_sand_test, flex_ptr, flags, &core);
                DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
            }
            else
#endif
            {
                cmd_ctest_run_test(unit, sh_sand_cmd, sh_sand_test, flex_ptr, flags, &core);
            }
        }
    }

exit:
    cmd_ctest_list_free(unit, test_list);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Callback for ctest run command
 * \param [in] unit unit id
 * \param [in] args pointer standard bcm shell argument structure, used by parse.h MACROS and routines
 * \param [in] ctr pointer to list of options processed by sand framework to be used with SH_SAND MACROS
 * \return
 *  \retval _SHR_E_NONE for success
 *  \retval _SHR_E_PARAM problem with input parameters, usage should be printed
 *  \retval other errors for other failure type
 * \remark
 *
 */
static shr_error_e
sys_ctest_run_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int flags = 0, tmp_flag;
    shell_flex_t shell_flex;
    sal_usecs_t usec;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_BOOL("pre", tmp_flag);
    if (tmp_flag == TRUE)
        flags |= CTEST_PRECOMMIT;

    SH_SAND_GET_BOOL("random", tmp_flag);
    if (tmp_flag == TRUE)
        flags |= CTEST_RANDOM;

    SH_SAND_GET_BOOL("logger", tmp_flag);
    if (tmp_flag == TRUE)
        flags |= SH_CMD_LOG;

    SH_SAND_GET_BOOL("failed", tmp_flag);
    if (tmp_flag == TRUE)
        flags |= SH_CMD_FAILURE_ONLY;

    PRT_TITLE_SET("%s", "CTests Results");
    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Arguments");
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Status");
    PRT_COLUMN_ADD("Time(usec)");
    PRT_COLUMN_ALIGN;

    shell_flex.failed = 0;
    shell_flex.succeeded = 0;
    shell_flex.prt_ctr = prt_ctr;

    usec = sal_time_usecs();

    if (sand_control->sh_sand_cmd_a)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse
                            (unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_run, (void *) &shell_flex, flags),
                            "CTest node run failure\n");
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_CLI_EXIT_IF_ERR(cmd_ctest_run(unit, args, sand_control->sh_sand_cmd, (void *) &shell_flex, flags),
                            "CTest leaf run failure\n");
    }
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Summary");
    PRT_CELL_SET("Failure:%d", shell_flex.failed);
    PRT_CELL_SET("Success:%d", shell_flex.succeeded);
    if (shell_flex.failed == 0)
    {
        PRT_CELL_SET("PASS");
    }
    else
    {
        PRT_CELL_SET("FAIL");
    }
    usec = sal_time_usecs() - usec;
    PRT_CELL_SET("%u", usec);

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
    "ctest [commands] run [pre] [lg] [fail]",
    "ct run\n" "ct tm run pre"
};

static sh_sand_option_t sys_list_arguments[] = {
    {"pre", SAL_FIELD_TYPE_BOOL, "Include only tests with precommit flag", "No"},
    {"random", SAL_FIELD_TYPE_BOOL, "Include only tests with random flag", "No"},
    {NULL}
};

static sh_sand_option_t sys_export_arguments[] = {
    {"list", SAL_FIELD_TYPE_STR, "Export TCl/TLIST name", ""},
    {"pre", SAL_FIELD_TYPE_BOOL, "Include only tests with precommit flag", "No"},
    {"random", SAL_FIELD_TYPE_BOOL, "Include only tests with random flag", "No"},
    {NULL}
};

static sh_sand_option_t sys_run_arguments[] = {
    {"logger", SAL_FIELD_TYPE_BOOL, "Print Start/End Markers with result status", "No"},
    {"failed", SAL_FIELD_TYPE_BOOL, "Results table will show only failed tests", "No"},
    {"pre", SAL_FIELD_TYPE_BOOL, "Include only tests with precommit flag", "No"},
    {"random", SAL_FIELD_TYPE_BOOL, "Include only tests with random flag", "No"},
    {NULL}
};

sh_sand_cmd_t sh_dnxc_sys_ctest_cmds[] = {
#if !defined(NO_FILEIO)
    {"export", sys_ctest_export_cmd, NULL, sys_export_arguments, &sys_ctest_export_man},
#endif
    {"list", sys_ctest_list_cmd, NULL, sys_list_arguments, &sys_ctest_list_man},
    {"run", sys_ctest_run_cmd, NULL, sys_run_arguments, &sys_ctest_run_man},
    {"usage", sys_usage_cmd, NULL, sys_usage_arguments, &sys_usage_man},
    {NULL}
};
