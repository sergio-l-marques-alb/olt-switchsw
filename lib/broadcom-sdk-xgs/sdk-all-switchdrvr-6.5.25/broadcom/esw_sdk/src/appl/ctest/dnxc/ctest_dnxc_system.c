/**
 * \file ctest_dnxc_ctest.c
 *
 * Framework add-on for ctest development
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <shared/dbx/dbx_file.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_system.h>
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#endif

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * If test is unsupported in adapter but command is full, include anyway, otherwise skip (CTEST_ADAPTER_UNSUPPORTED)
 * If general flag is to skip and command is marked as skip - skip
 * If random on command flag does not match random per test - skip
 * If run is for pre-commit only, execute only tests with CTEST_PRECOMMIT
 * If run is for weekend only, execute only tests with CTEST_WEEKEND
 * If run is for skippable, tests and test groups with CTEST_SKIPPABLE flag will be skipped
 * If run is for verify, only tests with CTEST_VERIFY flag will be included
 * Will always execute if CTEST_ALL flag is on or if CTEST_PASS is on and skippable was not provided (if skippable was provided, eligibility determined by remaining conditions)
 */
#define CTEST_ELIGIBLE(cmd_flags, test_flags)                                                                                                                                     \
                    (   !(!utilex_is_prime_flag_set(cmd_flags, CTEST_ADAPTER_UNSUPPORTED) && utilex_is_prime_flag_set(test_flags, CTEST_ADAPTER_UNSUPPORTED))               &&      \
                        (   (utilex_is_prime_flag_set(test_flags, CTEST_PASS) && !utilex_is_prime_flag_set(cmd_flags, CTEST_SKIPPABLE))                               ||          \
                            utilex_is_prime_flag_set(cmd_flags, CTEST_ALL)                                                                                            ||          \
                            (   !(utilex_is_prime_flag_set(cmd_flags, SH_CMD_SKIP_EXEC) && utilex_is_prime_flag_set(test_flags, SH_CMD_SKIP_EXEC))                &&              \
                                (utilex_is_prime_flag_set(cmd_flags, CTEST_RANDOM) == utilex_is_prime_flag_set(test_flags, CTEST_RANDOM))                         &&              \
                                (   (utilex_is_prime_flag_set(cmd_flags, CTEST_WEEKEND) && utilex_is_prime_flag_set(test_flags, CTEST_WEEKEND))               ||                  \
                                    (!utilex_is_prime_flag_set(cmd_flags, CTEST_WEEKEND) && !utilex_is_prime_flag_set(test_flags, CTEST_WEEKEND))                                 \
                                )                                                                                                                                 &&              \
                                (!utilex_is_prime_flag_set(cmd_flags, CTEST_PRECOMMIT) || utilex_is_prime_flag_set(test_flags, CTEST_PRECOMMIT))                  &&              \
                                !(utilex_is_prime_flag_set(cmd_flags, CTEST_SKIPPABLE) && utilex_is_prime_flag_set(test_flags, CTEST_SKIPPABLE))                  &&              \
                                (!utilex_is_prime_flag_set(cmd_flags, CTEST_VERIFY) || utilex_is_prime_flag_set(test_flags, CTEST_VERIFY))                                        \
                            )                                                                                                                                                     \
                        )                                                                                                                                                         \
                    )

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
    if ((test_invoke->params =
         sal_alloc(sal_strnlen(test_params, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1,
                   "ShellInvokeParams")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Error - failed to allocate memory for test params on:\"%s\"\n", test_name);
    }
    sal_strncpy(test_invoke->params, test_params,
                sal_strnlen(test_params, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
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
    if (utilex_is_prime_flag_set(sh_sand_cmd->flags, SH_CMD_SKIP_EXEC)
        && utilex_is_prime_flag_set(flags, SH_CMD_SKIP_EXEC))
    {
        SHR_EXIT();
    }

    if ((test_list = utilex_rhlist_create("ShellInvokes", sizeof(sh_sand_test_t), 1)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Failed to allocate list for test invocations on:%s\n",
                     sh_sand_cmd->keyword);
    }
#if defined(ADAPTER_SERVER_MODE)
    if (utilex_is_prime_flag_set(flags, CTEST_ADAPTER_UNSUPPORTED))
    {
        test_list->include_unsupported = TRUE;
    }
#endif
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
            int param_len = sal_strnlen(invoke->params, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
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
        if ((sh_sand_cmd->invoke_cb != NULL) && !utilex_is_prime_flag_set(sh_sand_cmd->flags, SH_CMD_CONDITIONAL))
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
             * No invokes - either NULL pointer or first entry is empty
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
        if (utilex_is_prime_flag_set(sh_sand_test->flags, CTEST_PRECOMMIT))
        {
            PRT_CELL_SET("PRECOMMIT");
        }
        else if (utilex_is_prime_flag_set(sh_sand_test->flags, CTEST_POSTCOMMIT))
        {
            PRT_CELL_SET("POSTCOMMIT");
        }
        /*
         * Or SKIPPed tests
         */
        else if (utilex_is_prime_flag_set(sh_sand_cmd->flags, SH_CMD_SKIP_EXEC)
                 || utilex_is_prime_flag_set(sh_sand_test->flags, SH_CMD_SKIP_EXEC))
        {
            PRT_CELL_SET("SKIP");
        }
        else
        {
            PRT_CELL_SET("");
        }
    }
    /*
     * Add command label if exist
     */
    if (!ISEMPTY(sh_sand_cmd->label))
    {
        PRT_CELL_SET("%s", sh_sand_cmd->label);
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
        sal_strncat_s(test_name, cmd_tokens[cmd_id], SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        sal_strncat_s(test_name, "_", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    }
    /*
     * Finally append test name, if no test name was provided use default
     */
    if (sh_sand_test != NULL)
        sal_strncat_s(test_name, RHNAME(sh_sand_test), SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    else
        sal_strncat_s(test_name, CTEST_DEFAULT, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
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
     * Verify that it's not empty test
     */
    if (sal_strncmp(test_name, "AT_Dnx_", sizeof(test_name)) != 0)
    {
        /*
         * All tests start from the standard preamble
         */
        teststr[SH_SAND_MAX_TOKEN_SIZE - 1] = 0;
        sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE - 1, "test_list_add ResetOnFail::%s\n", test_name);

        /*
         * Record full line in the file, whose handle is delivered by flex_ptr
         */
        dbx_file_write(file_in, teststr);
    }
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
    utilex_prime_flag_set(&flags, SH_CMD_SKIP_EXEC);

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
        sal_snprintf(teststr + sal_strnlen(teststr, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH),
                     SH_SAND_MAX_TOKEN_SIZE - 1, "#\\%s\n", title);
    }

    if (content != NULL)
    {
        sal_strncpy(tmp_content, content, SH_SAND_MAX_TOKEN_SIZE);

        dummy_buf = tmp_content;
        while ((token_buf = sal_strtok_r(dummy_buf, "\n", &save_ptr)) != NULL)
        {
            dummy_buf = NULL;
            sal_snprintf(teststr + sal_strnlen(teststr, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH),
                         SH_SAND_MAX_TOKEN_SIZE - 1, "# %s\n", token_buf);
        }
    }
    else
    {
        sal_snprintf(teststr + sal_strnlen(teststr, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH),
                     SH_SAND_MAX_TOKEN_SIZE - 1, "#\n");
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
    const char *tracking_id_contents = NULL;

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
        tracking_id_contents = doc->tracking_id;
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
     * tracking_id
     */
    cmd_ctest_export_tcl_add_test_doc_str(teststr, "tracking_id", tracking_id_contents);

    cmd_ctest_export_tcl_add_test_doc_str(teststr, "bug", NULL);

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
     * Verify that it's not empty test
     */
    if (sal_strncmp(test_name, "AT_Dnx_", sizeof(test_name)) != 0)
    {

        /*
         * Add documentation on top of test name
         */
        cmd_ctest_export_tcl_add_test_doc(sh_sand_cmd, teststr);

        /*
         * Procedure Name
         */
        sal_snprintf(teststr + sal_strnlen(teststr, sizeof(teststr)),
                     SH_SAND_MAX_TOKEN_SIZE - 1, "proc  ResetOnFail::%s", test_name);

        /*
         * Append line ending
         */
        sal_strncat_s(teststr, " {chip unit} {\n", sizeof(teststr));

        /*
         * Record full line in the file, whose handle is delivered by file_in
         */
        dbx_file_write(file_in, teststr);
        /*
         * Add warmboot check if needed
         */
        if (utilex_is_prime_flag_set(sh_sand_cmd->flags, SH_CMD_SKIP_TEST_WB)
            || utilex_is_prime_flag_set(sh_sand_test->flags, SH_CMD_SKIP_TEST_WB))
        {
            dbx_file_write(file_in, "\tset is_wb_test_mode [bcm_dnxc_warmboot_test_mode_get $unit]\n");
            dbx_file_write(file_in,
                           "\tNTest::AssertNEq 1 $is_wb_test_mode NO_SUPPORT \"this test is not supported in WB or CR mode\"\n");
        }
        /*
         * Create actual command line
         */
        sal_strncpy(teststr, "\tbcm shell [AT_Dnx_CTest_Seeded_Command_Get \"", SH_SAND_MAX_TOKEN_SIZE - 1);
        sal_strncpy(teststr + sal_strnlen(teststr, sizeof(teststr)), sh_sand_cmd->cmd_only,
                    SH_SAND_MAX_TOKEN_SIZE - 1 - sal_strnlen(teststr, sizeof(teststr)));
        if (sh_sand_test->params != NULL)
        {
            char escaped_params[RHSTRING_MAX_SIZE];
            sal_strncpy(teststr + sal_strnlen(teststr, sizeof(teststr)), " ",
                        SH_SAND_MAX_TOKEN_SIZE - 1 - sal_strnlen(teststr, sizeof(teststr)));
            utilex_escape_quote(sh_sand_test->params, escaped_params, RHSTRING_MAX_SIZE);
            sal_strncpy(teststr + sal_strnlen(teststr, sizeof(teststr)), escaped_params,
                        SH_SAND_MAX_TOKEN_SIZE - 1 - sal_strnlen(teststr, sizeof(teststr)));
        }
        sal_snprintf(teststr + sal_strnlen(teststr, sizeof(teststr)),
                     SH_SAND_MAX_TOKEN_SIZE - 1, "\" \"%s\"]\n", test_name);
        dbx_file_write(file_in, teststr);
        /*
         * Procedure Ending
         */
        dbx_file_write(file_in, "\treturn PASS\n");
        dbx_file_write(file_in, "}\n");
    }
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
    utilex_prime_flag_set(&flags, SH_CMD_SKIP_EXEC);

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
 *  The function returned the labels of the requested system mode from
 *  the tools/dnx/system_type_labels.xml file.
 * */
static shr_error_e
sys_ctest_find_label_for_system_mode(
    int unit,
    char *system_mode,
    char *label)
{
    char **system_mode_tokens;
    uint32 max_tokens = 100;
    uint32 tokens = 0;
    int i = 0;
    char filepath[RHFILE_MAX_SIZE];
    void *top_node, *curr_node;
    char sys_mode[RHFILE_MAX_SIZE];
    char sys_mode_label[RHFILE_MAX_SIZE];
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    if (ISEMPTY(system_mode))
    {
        goto exit;
    }
    else
    {
        system_mode_tokens = utilex_str_split(system_mode, ",", max_tokens, &tokens);
    }

    sal_memset(filepath, 0x0, RHFILE_MAX_SIZE);

    SHR_CLI_EXIT_IF_ERR(dbx_file_get_sdk_path(unit, "system_type_labels.xml", "/tools/dnx/", filepath, TRUE),
                        "could not get sdk path\n");

    if ((top_node = dbx_file_get_xml_top(unit, filepath, "entries", CONF_OPEN_CURRENT_LOC)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Failed to open file: system_type_labels.xml\n");
    }

    for (i = 0; i < tokens; i++)
    {
        found = FALSE;
        RHDATA_ITERATOR(curr_node, top_node, "entry")
        {
            sal_memset(sys_mode, 0x0, RHFILE_MAX_SIZE);
            sal_memset(sys_mode_label, 0x0, RHFILE_MAX_SIZE);
            RHDATA_GET_XSTR_STOP(curr_node, "system_mode", sys_mode, RHFILE_MAX_SIZE);
            RHDATA_GET_XSTR_STOP(curr_node, "label", sys_mode_label, RHFILE_MAX_SIZE);
            if (!(sal_strncasecmp(sys_mode, system_mode_tokens[i], sizeof(sys_mode))))
            {
                found = TRUE;
                if (ISEMPTY(label))
                {
                    sal_strncat_s(label, sys_mode_label, RHFILE_MAX_SIZE);
                }
                else
                {
                    sal_sprintf(label, "%s,%s", label, sys_mode_label);
                }
            }
        }
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "System mode: %s does not exist in file tools/dnx/system_type_labels.xml\n",
                         system_mode_tokens[i]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

void
sys_ctest_export_dev_str(
    int unit,
    char *dev_str,
    char *dev_gen_str)
{
#ifdef BCM_DNX_SUPPORT
    if (soc_is(unit, JERICHO2_B0_DEVICE) || soc_is(unit, Q2A_B0_DEVICE) || soc_is(unit, J2C_A0_DEVICE)
        || soc_is(unit, J2P_A0_DEVICE) || soc_is(unit, J2X_DEVICE))
    {
        if (dev_str != NULL)
            sal_strncat_s(dev_str, "_DNX", 5);
        sal_strncat_s(dev_gen_str, "DNX", 4);
        return;
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (soc_is(unit, DNX_RAMON))
    {
        sal_strncat_s(dev_gen_str, "DNXF", 5);
        return;
    }
#endif
    sal_strncat_s(dev_gen_str, "DNX", 4);
}

void
sys_ctest_export_insert_auto_generated_to_filepath(
    int unit,
    char *filepath,
    char *filename)
{
    char new_filepath[RHFILE_MAX_SIZE];
    int fname_len = sal_strlen(filename);
    sal_strncpy(new_filepath, filepath, sizeof(new_filepath));
    new_filepath[sal_strlen(filepath) - fname_len] = '\0';
    sal_strncat_s(new_filepath, "auto_generated/", sizeof(new_filepath));
    sal_strncat_s(new_filepath, filepath + sal_strlen(filepath) - fname_len, sizeof(new_filepath));
    filepath[0] = '\0';
    sal_strncat_s(filepath, new_filepath, sizeof(new_filepath));
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
    int flags = 0, tmp_flag;
    char *label;
    char *system_mode;
    char chip_type_str[8] = "";
    char chip_generation[6] = "";

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(filepath, 0x0, RHFILE_MAX_SIZE);

    SH_SAND_GET_STR("list", inputname);
    if (ISEMPTY(inputname))
    {
        filename = "testDvapiDNXCtest";
    }
    else
    {
        filename = inputname;
    }

    SH_SAND_GET_STR("label", label);
    utilex_str_white_spaces_remove(label);

    SH_SAND_GET_STR("system_mode", system_mode);
    utilex_str_white_spaces_remove(system_mode);
    SHR_CLI_EXIT_IF_ERR(sys_ctest_find_label_for_system_mode(unit, system_mode, label), "");

    SH_SAND_GET_BOOL("pre", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_PRECOMMIT);

    SH_SAND_GET_BOOL("skippable", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_SKIPPABLE);

    SH_SAND_GET_BOOL("weekend", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_WEEKEND);

    SH_SAND_GET_BOOL("random", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_RANDOM);

    SH_SAND_GET_BOOL("all", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_ALL);

#if defined(ADAPTER_SERVER_MODE)
    SH_SAND_GET_BOOL("full", tmp_flag);
    if (tmp_flag == TRUE)
    {
        /*
         * In 'ctest export full' the generated files are saved to a different file per device
         */
        sys_ctest_export_dev_str(unit, chip_type_str, chip_generation);
        utilex_prime_flag_set(&flags, CTEST_ALL);
        utilex_prime_flag_set(&flags, CTEST_ADAPTER_UNSUPPORTED);
    }
    else
    {
        sys_ctest_export_dev_str(unit, NULL, chip_generation);
    }
#else
    /*
     * In real devices, we do not want to exclude tests that are unsupported only in adapter (i.e. supported in real devices)
     */
    sys_ctest_export_dev_str(unit, NULL, chip_generation);
    utilex_prime_flag_set(&flags, CTEST_ADAPTER_UNSUPPORTED);
#endif

    SH_SAND_GET_BOOL("verify", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_VERIFY);

    /*
     * Generate TLIST file with list of all DVAPIs
     */

    if (dbx_file_get_sdk_path(unit, filename, "/tests/dnx/", filepath, FALSE) < 0)
    {
        SHR_CLI_EXIT_IF_ERR(dbx_file_get_sdk_path(unit, filename, "/regress/bcm/tests/dnx/", filepath, TRUE),
                            "could not get sdk path");
    }
    if (sal_strlen(chip_type_str))
    {
        /*
         * Insert 'auto_generated/' to filepath to ensure that files generated by running 'ctest export full' are in appropriately-named directory.
         */
        sys_ctest_export_insert_auto_generated_to_filepath(unit, filepath, filename);
    }
    sal_strncat_s(filepath, chip_type_str, sizeof(filepath));
    sal_strncat_s(filepath, ".tlist", sizeof(filepath));

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
                   "# DO NOT EDIT THIS FILE!                                                                    #\n");
    dbx_file_write(file,
                   "# This file is auto-generated by running \"ctest export\" from bcm shell                      #\n");
    dbx_file_write(file,
                   "# Edits to this file will be lost when it is regenerated                                    #\n");
    dbx_file_write(file,
                   "# $Id$                                                                                      #\n");
    dbx_file_write(file,
                   "# $Copyright: (c) 2015 Broadcom Corp.                                                       #\n");
    dbx_file_write(file,
                   "# All Rights Reserved.$                                                                     #\n");
    dbx_file_write(file,
                   "#############################################################################################\n");
    /*
     * If chip_type_str is not empty, the command is 'ctest export full' and the following needs to be added to prevent INFORM_UNUSED_TLIST regression failures
     */
    if (sal_strlen(chip_type_str) > 0)
    {
        dbx_file_write(file,
                       "\n# Do not remove following line, it is used to identify that the tlist is manually used by the team and will not be reported as \"tlist which is not used by regression\"\n");
        dbx_file_write(file, "# USED BY THE TEAM FOR MANUAL RUN\n\n");
    }

    if (sand_control->sh_sand_cmd_a)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse(unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_export_tlist,
                                                 (void *) file, flags, label), "ctest export to tlist failed\n");
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_CLI_EXIT_IF_ERR(cmd_ctest_export_tlist(unit, args, sand_control->sh_sand_cmd,
                                                   (void *) file, flags), "ctest export to tlist failed\n");
    }
    dbx_file_close(file);
    LOG_CLI((BSL_META("\nGenerate TLIST file with list of all DVAPIs was succeed%s\n"), EMPTY));

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

    if (dbx_file_get_sdk_path(unit, filename, "/tests/dnx/ctest/", filepath, FALSE) < 0)
    {
        SHR_CLI_EXIT_IF_ERR(dbx_file_get_sdk_path(unit, filename, "/regress/bcm/tests/dnx/ctest/", filepath, TRUE),
                            "could not get sdk path");
    }
    if (sal_strlen(chip_type_str))
    {
        /*
         * Insert 'auto_generated/' to filepath to ensure that files generated by running 'ctest export full' are in appropriately-named directory.
         */
        sys_ctest_export_insert_auto_generated_to_filepath(unit, filepath, filename);
    }
    sal_strncat_s(filepath, chip_type_str, sizeof(filepath));
    sal_strncat_s(filepath, ".tcl", sizeof(filepath));
    if ((file = dbx_file_open(filepath)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Cannot open:%s\n", filepath);
    }
    dbx_file_write(file, "##\n#\\file\n\n");
    dbx_file_write(file,
                   "#############################################################################################\n");
    dbx_file_write(file,
                   "# This file contains DVAPI tests for Jericho-II that should be run in the pre commit process#\n");
    dbx_file_write(file,
                   "#############################################################################################\n");
    dbx_file_write(file,
                   "# DO NOT EDIT THIS FILE!                                                                    #\n");
    dbx_file_write(file,
                   "# This file is auto-generated by running \"ctest export\" from bcm shell                      #\n");
    dbx_file_write(file,
                   "# Edits to this file will be lost when it is regenerated                                    #\n");
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
                                                 (void *) file, flags, label), "ctest export to tlist failed\n");
    }
    else if (sand_control->sh_sand_cmd)
    {
        SHR_CLI_EXIT_IF_ERR(cmd_ctest_export_tcl(unit, args, sand_control->sh_sand_cmd,
                                                 (void *) file, flags), "ctest export to tlist failed\n");
    }
    dbx_file_close(file);
    LOG_CLI((BSL_META("\nGenerate TCL file with ALL DVAPI was succeed%s\n"), EMPTY));

    LOG_CLI((BSL_META("\nChip Generation: %s\n"), chip_generation));

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
    int flags = 0, precommit_flag, random_flag, skippable_flag, verify_flag;
    char *label;
    char *system_mode;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("pre", precommit_flag);
    if (precommit_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_PRECOMMIT);

    SH_SAND_GET_BOOL("skippable", skippable_flag);
    if (skippable_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_SKIPPABLE);

    SH_SAND_GET_BOOL("random", random_flag);
    if (random_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_RANDOM);

    SH_SAND_GET_BOOL("verify", verify_flag);
    if (verify_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_VERIFY);

    SH_SAND_GET_STR("label", label);
    utilex_str_white_spaces_remove(label);

    SH_SAND_GET_STR("system_mode", system_mode);
    utilex_str_white_spaces_remove(system_mode);
    SHR_CLI_EXIT_IF_ERR(sys_ctest_find_label_for_system_mode(unit, system_mode, label), "");

    PRT_TITLE_SET("%s", "CTests List");
    PRT_COLUMN_ADD("Command");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Arguments");
    PRT_COLUMN_ADD("Flags");
    PRT_COLUMN_ADD("Label");
    if (sand_control->sh_sand_cmd_a)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_traverse
                            (unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_list, (void *) prt_ctr, flags, label),
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
        sal_strncpy(command_str + sal_strnlen(command_str, sizeof(command_str)), " ",
                    SH_SAND_MAX_TOKEN_SIZE - 1 - sal_strnlen(command_str, sizeof(command_str)));
        sal_strncpy(command_str + sal_strnlen(command_str, sizeof(command_str)), sh_sand_test->params,
                    SH_SAND_MAX_TOKEN_SIZE - 1 - sal_strnlen(command_str, sizeof(command_str)));
        PRT_CELL_SET("%s", sh_sand_test->params);
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

    if (utilex_is_prime_flag_set(sh_sand_cmd->flags, SH_CMD_EXEC_PER_CORE))
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

    if (utilex_is_prime_flag_set(flags, SH_CMD_LOG))
    {
        LOG_CLI((BSL_META("Test:%s\n"), command_str));
    }

    SHR_SET_CURRENT_ERR(sh_sand_option_list_process
                        (unit, core, leaf_args, sh_sand_cmd, &sand_ctr_m, sh_sand_cmd->flags));
    if (SHR_FUNC_VAL_IS(_SHR_E_NONE)
        && ((SHR_SET_CURRENT_ERR(sh_sand_cmd->action(unit, leaf_args, &sand_ctr_m))) == _SHR_E_NONE))
    {
        shell_flex_p->succeeded++;
        if (utilex_is_prime_flag_set(flags, SH_CMD_LOG))
        {
            LOG_CLI((BSL_META("\tResult(pass)\n")));
        }
        if (utilex_is_prime_flag_set(flags, SH_CMD_FAILURE_ONLY))
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
        if (utilex_is_prime_flag_set(flags, SH_CMD_LOG))
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
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Exclude SKIPPED tests
     */
    utilex_prime_flag_set(&flags, SH_CMD_SKIP_EXEC);

    SHR_CLI_EXIT_IF_ERR(cmd_ctest_list_create(unit, sh_sand_cmd, &test_list, flags),
                        "Failed to create test list for:'%s'\n", sh_sand_cmd->cmd_only);

    RHITERATOR(sh_sand_test, test_list)
    {
        if (utilex_is_prime_flag_set(sh_sand_cmd->flags, SH_CMD_EXEC_PER_CORE))
        {
            /*
             * if SH_CMD_EXEC_PER_CORE flag is set update the nof_cores.
             */
            SHR_CLI_EXIT_IF_ERR(sh_sand_nof_cores_get(unit, &nof_cores), "Failed to get the number of cores.");
        }
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            int core_id = (nof_cores == 1) ? 0 : BCM_CORE_ALL;
            DNXCMN_CORES_ITER(unit, core_id, core)
            {
                if (SOC_IS_DNX(unit)
                    && (utilex_is_prime_flag_set(sh_sand_cmd->flags, SH_CMD_ROLLBACK)
                        || utilex_is_prime_flag_set(sh_sand_test->flags, SH_CMD_ROLLBACK)))
                {
                    DNX_ROLLBACK_JOURNAL_START(unit);
                    rv = cmd_ctest_run_test(unit, sh_sand_cmd, sh_sand_test, flex_ptr, flags, &core);
                    if (utilex_is_prime_flag_set(flags, CTEST_CONT_ON_FAIL))
                    {
                        SHR_IF_ERR_CONT(rv);
                    }
                    else
                    {
                        SHR_CLI_EXIT_IF_ERR(rv, "Test failed\n");
                    }
                    DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
                }
                else
                {
                    rv = cmd_ctest_run_test(unit, sh_sand_cmd, sh_sand_test, flex_ptr, flags, &core);
                    if (utilex_is_prime_flag_set(flags, CTEST_CONT_ON_FAIL))
                    {
                        SHR_IF_ERR_CONT(rv);
                    }
                    else
                    {
                        SHR_CLI_EXIT_IF_ERR(rv, "Test failed\n");
                    }
                }
            }
        }
        else
#endif
        {
            for (core = 0; core < nof_cores; core++)
            {
#ifdef BCM_DNX_SUPPORT
                if (SOC_IS_DNX(unit)
                    && (utilex_is_prime_flag_set(sh_sand_cmd->flags, SH_CMD_ROLLBACK)
                        || utilex_is_prime_flag_set(sh_sand_test->flags, SH_CMD_ROLLBACK)))
                {
                    DNX_ROLLBACK_JOURNAL_START(unit);
                    rv = cmd_ctest_run_test(unit, sh_sand_cmd, sh_sand_test, flex_ptr, flags, &core);
                    if (utilex_is_prime_flag_set(flags, CTEST_CONT_ON_FAIL))
                    {
                        SHR_IF_ERR_CONT(rv);
                    }
                    else
                    {
                        SHR_CLI_EXIT_IF_ERR(rv, "Test failed\n");
                    }
                    DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
                }
                else
#endif
                {
                    rv = cmd_ctest_run_test(unit, sh_sand_cmd, sh_sand_test, flex_ptr, flags, &core);
                    if (utilex_is_prime_flag_set(flags, CTEST_CONT_ON_FAIL))
                    {
                        SHR_IF_ERR_CONT(rv);
                    }
                    else
                    {
                        SHR_CLI_EXIT_IF_ERR(rv, "Test failed\n");
                    }
                }
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
    int rv = 0;
    char err_msg[25] = "";

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SH_SAND_GET_BOOL("pre", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_PRECOMMIT);

    SH_SAND_GET_BOOL("skippable", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_SKIPPABLE);

    SH_SAND_GET_BOOL("random", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_RANDOM);

    SH_SAND_GET_BOOL("logger", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, SH_CMD_LOG);

    SH_SAND_GET_BOOL("failed", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, SH_CMD_FAILURE_ONLY);

    SH_SAND_GET_BOOL("continue", tmp_flag);
    if (tmp_flag == TRUE)
        utilex_prime_flag_set(&flags, CTEST_CONT_ON_FAIL);

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
        rv = sh_sand_cmd_traverse(unit, args, sand_control->sh_sand_cmd_a, cmd_ctest_run, (void *) &shell_flex, flags, NULL     /* label 
                                                                                                                                 */ );
        sal_strncat_s(err_msg, "CTest node run failure\n", 25);
    }
    else if (sand_control->sh_sand_cmd)
    {
        rv = cmd_ctest_run(unit, args, sand_control->sh_sand_cmd, (void *) &shell_flex, flags);
        sal_strncat_s(err_msg, "CTest leaf run failure\n", 25);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Summary");
    PRT_CELL_SET("Failure:%d", shell_flex.failed);
    PRT_CELL_SET("Success:%d", shell_flex.succeeded);
    PRT_CELL_SET("");   /* add empty cell in the 'core' column to align PASS/FAIL status and time columns */
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

    SHR_CLI_EXIT_IF_ERR(rv, "%s", err_msg);

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
#ifndef NO_FILEIO
static shr_error_e
sys_ctest_manual_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *introduction =
        "This document presents description of the CTests. "
        "All commands are organized in ctest tree, where leaves are actual executable tests. "
        "Manual presents description, synopsis, arguments and examples per each leaf. "
        "Each argument has certain type which defines input format, as may be see from argument description. "
        "Argument may have default and thus not required on command line, if there is no default(tagged by 'None' in usage). "
        "user must provide it. "
        "All arguments should be provided in arg=<value> paradigm with 2 exceptions: "
        "1. Boolean argument provided without value imply 'TRUE', "
        "2. Argument with 'free' tag allows omitting argument name, providing values only. Values without argument names "
        "will be associated with free argument according to the order. 1st nameless argument to the 1st free one, "
        "2nd to the second and so on "
        "Besides the arguments presented per command, there are system arguments that may be used with any leaf, "
        "There are also system commands which may be invoked after each branch or leaf, see details below ";

    return sys_manual_cmd_internal(unit, args, sand_control, introduction, MANUAL_TYPE_CTEST);
}
#endif

static sh_sand_man_t sys_ctest_export_man = {
    "Generate tlist and tcl code for ctest",
    "Command generates tlist and tcl code for all ctest starting from the last command before export\n"
#if defined(ADAPTER_SERVER_MODE)
        "$SDK/regress/bcm/tests/dnx/ctest/CTestDvapis.tcl and $SDK/regress/bcm/tests/dnx/testDvapiDNXCtest.tlist\n"
        "running with argument 'full' writes to device specific files in same directories instead",
    "ctest [commands] export [pre] [skippable] [all] [full] [rnd] [verify] [label] [system_mode]",
#else
        "$SDK/regress/bcm/tests/dnx/ctest/CTestDvapis.tcl and $SDK/regress/bcm/tests/dnx/testDvapiDNXCtest.tlist",
    "ctest [commands] export [pre] [skippable] [all] [rnd] [verify] [label] [system_mode]",
#endif
    "ct export pre\n" "ct fld export\n" "ct mdb export\n" "ct export all\n" "ct export label=kbp\n"
        "ct export system_mode=kbp\n" "ctest export skippable label=apt\n" "ctest export verify"
};

static sh_sand_man_t sys_ctest_list_man = {
    "List available ctests",
    "List ctests for all commands starting from last branch on command line",
    "ctest [commands] list [pre] [skippable] [rnd] [verify] [label] [system_mode]",
    "ct list\n" "ct tm list pre\n" "ct export label=kbp\n" "ct export system_mode=kbp\n"
        "ctest list skippable label=apt"
};

static sh_sand_man_t sys_ctest_run_man = {
    "Execute ctests",
    "Execute ctests for all commands starting from last branch on command line",
    "ctest [commands] run [pre] [skippable] [rnd] [lg] [fail] [um] [continue]",
    "ct run\n" "ct tm run pre\n" "ctest run skippable\n" "ctest run pre continue"
};

#ifndef NO_FILEIO
static sh_sand_man_t sys_ctest_manual_man = {
    .brief = "Export usage to xml file. By default it will be ctest_manual.xml placed in $PWD."
        "Argument 'file' may be used to assign any other name or/and path",
    .examples = "file=CTest_UM.xml\n"
};
#endif

static sh_sand_option_t sys_list_arguments[] = {
    {"pre", SAL_FIELD_TYPE_BOOL, "Include only tests with precommit flag", "No"},
    {"skippable", SAL_FIELD_TYPE_BOOL, "Skip tests and test groups with skippable flag", "No"},
    {"random", SAL_FIELD_TYPE_BOOL, "Include only tests with random flag", "No"},
    {"verify", SAL_FIELD_TYPE_BOOL, "Include only framework verify tests", "No"},
    {"label", SAL_FIELD_TYPE_STR, "Include tests with specific label", ""},
    {"system_mode", SAL_FIELD_TYPE_STR, "Include tests with specific system mode", ""},
    {NULL}
};

static sh_sand_option_t sys_export_arguments[] = {
    {"list", SAL_FIELD_TYPE_STR, "Export TCl/TLIST name", ""},
    {"pre", SAL_FIELD_TYPE_BOOL, "Include only tests with precommit flag", "No"},
    {"skippable", SAL_FIELD_TYPE_BOOL, "Skip tests and test groups with skippable flag", "No"},
    {"weekend", SAL_FIELD_TYPE_BOOL, "Include only tests with weekend flag", "No"},
    {"random", SAL_FIELD_TYPE_BOOL, "Include only tests with random flag", "No"},
    {"all", SAL_FIELD_TYPE_BOOL, "Include all tests, regardless device/soc properties", "No"},
#if defined(ADAPTER_SERVER_MODE)
    {"full", SAL_FIELD_TYPE_BOOL,
     "Include all tests, regardless device/soc properties and including unsupported adapter tests", "No"},
#endif
    {"verify", SAL_FIELD_TYPE_BOOL, "Include only framework verify tests", "No"},
    {"label", SAL_FIELD_TYPE_STR, "Include tests with specific label", ""},
    {"system_mode", SAL_FIELD_TYPE_STR, "Include tests with specific system label", ""},
    {NULL}
};

static sh_sand_option_t sys_run_arguments[] = {
    {"logger", SAL_FIELD_TYPE_BOOL, "Print Start/End Markers with result status", "No"},
    {"failed", SAL_FIELD_TYPE_BOOL, "Results table will show only failed tests", "No"},
    {"pre", SAL_FIELD_TYPE_BOOL, "Include only tests with precommit flag", "No"},
    {"skippable", SAL_FIELD_TYPE_BOOL, "Skip tests and test groups with skippable flag", "No"},
    {"random", SAL_FIELD_TYPE_BOOL, "Include only tests with random flag", "No"},
    {"continue", SAL_FIELD_TYPE_BOOL, "Continue running remaining tests after first failure", "No"},
    {NULL}
};

sh_sand_cmd_t sh_dnxc_sys_ctest_cmds[] = {
#if !defined(NO_FILEIO)
    {"export", sys_ctest_export_cmd, NULL, sys_export_arguments, &sys_ctest_export_man, NULL, NULL, CTEST_POSTCOMMIT},
#endif
    {"list", sys_ctest_list_cmd, NULL, sys_list_arguments, &sys_ctest_list_man, NULL, NULL, CTEST_UM},
    {"run", sys_ctest_run_cmd, NULL, sys_run_arguments, &sys_ctest_run_man, NULL, NULL, CTEST_UM},
#ifndef NO_FILEIO
    {"manual", sys_ctest_manual_cmd, NULL, sys_manual_arguments, &sys_ctest_manual_man, NULL, NULL},
#endif
    {"usage", sys_usage_cmd, NULL, sys_usage_arguments, &sys_usage_man, NULL, NULL, CTEST_UM},
    {NULL}
};
