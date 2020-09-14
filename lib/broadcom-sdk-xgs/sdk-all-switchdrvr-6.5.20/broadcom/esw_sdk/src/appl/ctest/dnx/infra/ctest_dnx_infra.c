/** \file diag_dnx_infra.c
 * 
 * main file for Infrastructure tests
 * 
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxc/ctest_dnxc_infra_init.h>

/** local */
#include "ctest_dnx_infra_pe_tests.h"
#include "ctest_dnx_infra_init_tests.h"

/*
 * }
 */

/**
 * \brief DNX Infrastructure Tests
 * List of Infrastructure ctests modules.
 */
sh_sand_cmd_t dnx_infra_test_cmds[] = {
    /*
     * NAME | Leaf action | Junction Array Pointer 
     */
    {"periodic_event", NULL, dnx_infra_periodic_event_test_cmds}
    ,
    {"INIT_dnx", NULL, dnxc_infra_init_test_cmds}
    ,
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */
