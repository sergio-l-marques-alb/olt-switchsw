/**
 * \file ctest_dnxf_root.c
 *
 * Root for ctests for DNXF devices
 *
 */

#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxf/ctest_dnxf_root.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

extern sh_sand_cmd_t dnxf_data_test_cmds[];
extern sh_sand_cmd_t dnxf_infra_test_cmds[];
extern sh_sand_cmd_t dnxc_framework_test_cmds[];

/* *INDENT-OFF* */
sh_sand_man_t sh_dnxf_ctest_man = {
    .brief = "Run unit tests",
    .full  = "Root command for DNXF specific tests. Use \"ct cmdname[s] usage\" for details\n"
};

sh_sand_cmd_t sh_dnxf_ctest_commands[] = {
   /* Name                 | Leaf Action | Junction Array Pointer   | Option list for leaf | Man page         | Dynamic Option CB   */
    {"data",               NULL,           dnxf_data_test_cmds},
    {"framework",          NULL,           dnxc_framework_test_cmds},
    {"INFRAstructure",     NULL,           dnxf_infra_test_cmds},
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */
