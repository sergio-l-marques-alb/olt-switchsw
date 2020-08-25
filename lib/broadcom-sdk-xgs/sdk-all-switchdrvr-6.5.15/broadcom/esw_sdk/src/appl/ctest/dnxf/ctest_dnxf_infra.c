/** \file diag_dnxf_infra.c
 * 
 * main file for Infrastructure tests
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxc/ctest_dnxc_infra_init.h>

/*
 * }
 */

sh_sand_cmd_t dnxf_infra_init_test_cmds[] = {
    {"PaRaLLel", sh_dnxc_infra_init_parallel_test_cmd, NULL,
     sh_dnxc_infra_init_parallel_test_options, &sh_dnxc_infra_init_parallel_test_man,
     NULL, sh_dnxc_infra_init_parallel_tests, CTEST_POSTCOMMIT}
    ,

    {NULL}
};

/**
 * \brief DNXF Infrastructure Tests
 * List of Infrastructure ctests modules.
 */
sh_sand_cmd_t dnxf_infra_test_cmds[] = {
    /*
     * NAME | Leaf action | Junction Array Pointer 
     */
    {"INIT_dnx", NULL, dnxf_infra_init_test_cmds}
    ,
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */
