/** \file diag_dnx_in_lif_profile.c
 *
 * in_lif_profile unit test.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>

/**
 * }
 */

/*
 * structs to support bcm shell command
 */
/*
 * {
 */

shr_error_e ctest_dnx_local_outlif_allocation_stress_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_local_outlif_disable_mdb_clusters(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_local_inlif_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_global_lif_basic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e dnx_in_lif_profile_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e dnx_out_lif_profile_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t dnx_outlif_allocation_test_man;

extern sh_sand_man_t dnx_inlif_semantic_test_man;

extern sh_sand_man_t dnx_in_lif_profile_test_man;

extern sh_sand_man_t dnx_out_lif_profile_test_man;

extern sh_sand_man_t dnx_global_lif_semantic_test_man;

extern sh_sand_option_t ctest_dnx_local_outlif_disable_mdb_options[];

sh_sand_cmd_t dnx_lif_test_cmds[] = {
   /*************************************************************************************************************************************
    *   CMD_NAME    *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *               *                           * Level                   *                                *                            *
    *               *                           * CMD                     *                                *                            *
    *************************************************************************************************************************************/
    {"inlifprofile", dnx_in_lif_profile_unit_test, NULL, NULL, &dnx_in_lif_profile_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,
    {"outlifprofile", dnx_out_lif_profile_unit_test, NULL, NULL, &dnx_out_lif_profile_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,
    {"outlif_stress", ctest_dnx_local_outlif_allocation_stress_test, NULL, NULL, &dnx_outlif_allocation_test_man, NULL,
     NULL, CTEST_POSTCOMMIT}
    ,
    {"disable_mdb_clusters", ctest_dnx_local_outlif_disable_mdb_clusters, NULL,
     ctest_dnx_local_outlif_disable_mdb_options,
     &dnx_outlif_allocation_test_man, NULL, NULL, CTEST_POSTCOMMIT}
    ,
    {"inlif_semantic", ctest_dnx_local_inlif_semantic_test, NULL, NULL, &dnx_inlif_semantic_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,
    {"global_lif_semantic", ctest_dnx_global_lif_basic_test, NULL, NULL, &dnx_global_lif_semantic_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,

    {NULL}
};
