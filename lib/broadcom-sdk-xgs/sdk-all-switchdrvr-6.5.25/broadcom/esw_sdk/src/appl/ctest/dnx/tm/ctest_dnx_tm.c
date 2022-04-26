/** \file diag_dnx_tm.c
 * 
 * main file fot tm diagnostics
 * 
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include "ctest_dnx_ipq_tests.h"
#include "ctest_dnx_iqs_tests.h"

/** local */
#include "ctest_dnx_multicast_tests.h"
#include "ctest_dnx_dram_tests.h"
#include "ctest_dnx_nif_tests.h"
#include "ctest_dnx_portmod_pe_tests.h"
#include "ctest_dnx_priority_propagation_tests.h"
#include "ctest_dnx_fc_tests.h"
#include "ctest_dnx_scheduling_elements_tests.h"
#include "ctest_dnx_recycle_tests.h"

/*
 * }
 */

/**
 * \brief DNX TM Ingress Tests
 * List of ingress ctests modules.
 */
sh_sand_cmd_t dnx_tm_ingress_test_cmds[] = {
    /*
     * NAME | Leaf action | Junction Array Pointer 
     */
    {"queue", NULL, sh_dnx_tm_ipq_test_cmds}
    ,
    {"scheduler", NULL, sh_dnx_tm_iqs_test_cmds}
    ,
    {NULL}      /* This line should always stay as last one */
};

sh_sand_cmd_t sh_dnx_tm_scheduler_test_cmds[] = {
    /*
     * NAME | Leaf action | Junction Array Pointer
     */
    {"priority_propagation", NULL, sh_dnx_tm_priority_propagation_cmds}
    ,
    {"scheduling_elements", NULL, sh_dnx_tm_scheduling_elements_cmds}
    ,
    {NULL}      /* This line should always stay as last one */
};
/**
 * \brief DNX TM Tests
 * List of TM ctests modules.
 */
sh_sand_cmd_t dnx_tm_test_cmds[] = {
    /*
     * NAME | Leaf action | Junction Array Pointer 
     */
    {"ingress", NULL, dnx_tm_ingress_test_cmds}
    ,
    {"multicast", sh_dnx_tm_multicast_cmd, NULL, sh_dnx_tm_multicast_options, &sh_dnx_tm_multicast_man, NULL,
     sh_dnx_tm_multicast_tests}
    ,
    {"dram", NULL, sh_dnx_tm_dram_test_cmds}
    ,
    {"nif", NULL, sh_dnx_tm_nif_test_cmds}
    ,
    {"portmodPE", NULL, dnx_infra_portmod_shr_thread_manager_test_cmds}
    ,
    {"scheduler", NULL, sh_dnx_tm_scheduler_test_cmds}
    ,
    {"fc", NULL, dnx_fc_test_cmds}
    ,
    {"recycle", NULL, sh_dnx_recycle_test_cmds}
    ,
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */
