/** \file diag_dnx_fabric.c
 *
 * main file for fabric ctests
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

/** local */
#include "ctest_dnx_fabric_port_tests.h"
/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief DNX Fabric Tests
 * List of fabric ctests commands.
 */
sh_sand_cmd_t dnx_fabric_test_cmds[] = {
    /*keyword,     action,      command,                             options,  man */
    {"port",       NULL,        ctest_dnx_fabric_port_test_cmds,     NULL,     NULL},
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */
