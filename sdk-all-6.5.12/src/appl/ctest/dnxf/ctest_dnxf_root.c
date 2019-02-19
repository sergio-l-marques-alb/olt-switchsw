/*
 * $Id: cmdlist.c,v 1.30 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        cmdlist.c
 * Purpose:     List of commands available in DNX mode
 * Requires:
 */

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>

#include <appl/ctest/dnxc/ctest_dnxc_system.h>

#ifdef INCLUDE_AUTOCOMPLETE
#include <sal/appl/editline/autocomplete.h>
#endif

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

extern sh_sand_cmd_t sh_dnxf_commands[];
extern sh_sand_cmd_t dnxc_framework_test_cmds[];
extern sh_sand_cmd_t dnxf_data_test_cmds[];

static sh_sand_man_t shell_exec_man = {
    "Test all shell commands",
    "Traverse shell command tree and run all commands and their invocation options",
    "",
    "",
};

static shr_error_e
shell_exec_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sh_sand_act_all(unit, args, "exec", sh_dnxf_commands, sh_sand_sys_cmds), "");

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
static sh_sand_cmd_t sh_dnxf_ctest_commands[] = {
   /* Name         | Leaf Action | Junction Array Pointer   | Option list for leaf | Man page         | Dynamic Option CB   */
    {"data",    NULL,           dnxf_data_test_cmds},
    {"exec",    shell_exec_cmd, NULL, NULL,                &shell_exec_man,     NULL,                  NULL, CTEST_POSTCOMMIT},
    {"framework",   NULL,           dnxc_framework_test_cmds},
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */

const char cmd_dnxf_ctest_usage[] = "Root command for DNXF specific tests. Use \"ct cmdname[s] usage\" for details\n";

/**
 * \brief Routine called from DNX shell on ctest
 * Entry point of any ctest command
 */
cmd_result_t
cmd_dnxf_ctest_invoke(
    int unit,
    args_t * args)
{
    shr_error_e res;
    /*
     * Start from highest point in the tree
     */
    res = sh_sand_act(unit, args, sh_dnxf_ctest_commands, sh_dnxc_sys_ctest_cmds);
    ARG_DISCARD(args);

    return diag_sand_error_get(res);
}

cmd_result_t
cmd_dnxf_ctest_init(
    int unit)
{
    cmd_result_t result;
    char command[SH_SAND_MAX_TOKEN_SIZE];

#ifdef INCLUDE_AUTOCOMPLETE
    sh_sand_cmd_autocomplete_init(unit, NULL, (sh_sand_cmd_t[2])
                                  {
                                  {
                                  "ctest", NULL, sh_dnxf_ctest_commands}
                                  ,
                                  {
                                  NULL}
                                  }
    );
#endif /* INCLUDE_AUTOCOMPLETE */

    command[0] = 0;
    result = diag_sand_error_get(sh_sand_init(unit, sh_dnxf_ctest_commands, command, SH_SAND_VERIFY_ALL));
    if (result != CMD_OK)
        goto exit;

    command[0] = 0;
    result = diag_sand_error_get(sh_sand_init(unit, sh_dnxc_sys_ctest_cmds, command, SH_SAND_VERIFY_ALL));
    if (result != CMD_OK)
        goto exit;

exit:
    return result;
}

cmd_result_t
cmd_dnxf_ctest_deinit(
    int unit)
{
    cmd_result_t result;

#ifdef INCLUDE_AUTOCOMPLETE
    sh_sand_cmd_autocomplete_deinit(unit, (sh_sand_cmd_t[2])
                                    {
                                    {
                                    "ctest", NULL, sh_dnxf_ctest_commands}
                                    ,
                                    {
                                    NULL}
                                    }
    );
#endif /* INCLUDE_AUTOCOMPLETE */

    result = diag_sand_error_get(sh_sand_deinit(unit, sh_dnxf_ctest_commands));

    if (result != CMD_OK)
        goto exit;

exit:
    return result;
}
