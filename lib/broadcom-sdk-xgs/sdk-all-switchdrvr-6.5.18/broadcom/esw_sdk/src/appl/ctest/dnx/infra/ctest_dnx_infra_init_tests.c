/** \file ctest_dnx_infra_seq_tests.c
 * 
 * Tests for utilex sequence mechanism
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/diag.h>
#include <appl/ctest/dnxc/ctest_dnxc_infra_init.h>

sh_sand_cmd_t dnx_infra_init_test_cmds[] = {
    {"ERRor_RECovery", diag_dnxc_infra_init_error_recovery_test_cmd, NULL,
     diag_dnxc_infra_init_test_options, &diag_dnxc_infra_init_error_recovery_test_man,
     NULL, NULL, CTEST_POSTCOMMIT, diag_dnxc_infra_init_error_recovery_tests_creation}
    ,

#ifndef ADAPTER_SERVER_MODE
    {"TIME", diag_dnxc_infra_init_time_test_cmd, NULL,
     diag_dnxc_infra_init_test_options, &diag_dnxc_infra_init_time_test_man,
     NULL, NULL, CTEST_POSTCOMMIT, diag_dnxc_infra_init_time_tests_creation}
    ,
#endif

    {"PaRaLLel", sh_dnxc_infra_init_parallel_test_cmd, NULL,
     sh_dnxc_infra_init_parallel_test_options, &sh_dnxc_infra_init_parallel_test_man,
     NULL, sh_dnxc_infra_init_parallel_tests, CTEST_POSTCOMMIT}
    ,

    {NULL}
};
