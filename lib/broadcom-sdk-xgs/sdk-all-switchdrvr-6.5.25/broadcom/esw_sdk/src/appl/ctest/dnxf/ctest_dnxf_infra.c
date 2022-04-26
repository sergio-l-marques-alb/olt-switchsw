/** \file diag_dnxf_infra.c
 * 
 * main file for Infrastructure tests
 * 
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxc/ctest_dnxc_infra_init.h>
#include <bcm/init.h>

/*
 * }
 */

/**
 * \brief DNXF Infrastructure Tests
 * List of Infrastructure ctests modules.
 */
sh_sand_cmd_t dnxf_infra_test_cmds[] = {
    /** NAME            | Leaf action                                   | Junction Array Pointer */
    {"INIT_dnx", NULL, dnxc_infra_init_test_cmds}
    ,
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */
