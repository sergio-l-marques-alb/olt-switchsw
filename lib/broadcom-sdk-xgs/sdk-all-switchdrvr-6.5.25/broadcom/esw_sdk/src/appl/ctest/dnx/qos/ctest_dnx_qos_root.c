/** \file ctest_dnx_qos_root.c
 *
 * Tests junction for QOS CTests
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

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include "ctest_dnx_qos_egress.h"

/** List of qos tests   */
sh_sand_cmd_t dnx_qos_test_cmds[] = {
    {"egress", NULL, dnx_qos_egress_tests}
    ,
    {NULL}
};
