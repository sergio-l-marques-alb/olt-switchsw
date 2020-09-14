/** \file diag_dnx_in_lif_profile.c
 *
 * in_lif_profile unit test.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
#include "ctest_dnx_inlif_wide_data.h"
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

shr_error_e ctest_dnx_local_outlif_stress_test_support_check(
    int unit,
    rhlist_t * invoke_list);

shr_error_e ctest_dnx_local_outlif_allocation_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_local_outlif_allocation_replace_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_local_outlif_allocation_phase_map_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_local_outlif_allocation_capacity_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_local_outlif_allocation_negative_ll_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_local_outlif_disable_mdb_clusters(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_dnx_local_outlif_disable_eedb_data_banks(
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

shr_error_e ctest_dnx_lif_esem_cmd_allocation_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t dnx_outlif_stress_test_man;

extern sh_sand_man_t dnx_outlif_semantic_test_man;

extern sh_sand_man_t dnx_outlif_allocation_replace_semantic_test_man;

extern sh_sand_man_t dnx_outlif_mdb_phase_map_test_man;

extern sh_sand_man_t dnx_outlif_phase_capacity_test_man;

extern sh_sand_man_t dnx_outlif_negative_ll_test_man;

extern sh_sand_man_t dnx_inlif_allocation_replace_semantic_test_man;

extern sh_sand_man_t dnx_inlif_semantic_test_man;

extern sh_sand_man_t dnx_in_lif_profile_test_man;

extern sh_sand_man_t dnx_out_lif_profile_test_man;

extern sh_sand_man_t dnx_global_lif_semantic_test_man;

extern sh_sand_man_t dnx_lif_esem_cmd_allocation_test_man;

extern sh_sand_option_t ctest_dnx_local_outlif_disable_mdb_options[];

extern sh_sand_option_t ctest_dnx_local_outlif_disable_eedb_data_banks_options[];

extern sh_sand_option_t ctest_dnx_local_outlif_allocation_semantic_test_options[];

extern sh_sand_option_t ctest_dnx_local_outlif_phase_capacity_test_options[];

extern sh_sand_option_t ctest_dnx_local_outlif_negative_ll_test_options[];

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
    {"outlif_stress", ctest_dnx_local_outlif_allocation_stress_test, NULL, NULL, &dnx_outlif_stress_test_man, NULL,
     NULL, CTEST_POSTCOMMIT | SH_CMD_CONDITIONAL, ctest_dnx_local_outlif_stress_test_support_check}
    ,
    {"outlif_semantic", ctest_dnx_local_outlif_allocation_semantic_test, NULL,
     ctest_dnx_local_outlif_allocation_semantic_test_options, &dnx_outlif_semantic_test_man, NULL,
     NULL, CTEST_POSTCOMMIT}
    ,
    {"outlif_replace", ctest_dnx_local_outlif_allocation_replace_semantic_test, NULL, NULL,
     &dnx_outlif_allocation_replace_semantic_test_man, NULL, NULL, CTEST_POSTCOMMIT}
    ,
    {"outlif_mdb_phase_map", ctest_dnx_local_outlif_allocation_phase_map_test, NULL, NULL,
     &dnx_outlif_mdb_phase_map_test_man, NULL, NULL, CTEST_POSTCOMMIT}
    ,
    {"outlif_phase_capacity", ctest_dnx_local_outlif_allocation_capacity_test, NULL,
     ctest_dnx_local_outlif_phase_capacity_test_options,
     &dnx_outlif_phase_capacity_test_man, NULL, NULL, SH_CMD_SKIP_EXEC}
    ,
    {"outlif_negative_ll_check", ctest_dnx_local_outlif_allocation_negative_ll_test, NULL,
     ctest_dnx_local_outlif_negative_ll_test_options,
     &dnx_outlif_negative_ll_test_man, NULL, NULL, CTEST_POSTCOMMIT}
    ,
    {"disable_mdb_clusters", ctest_dnx_local_outlif_disable_mdb_clusters, NULL,
     ctest_dnx_local_outlif_disable_mdb_options,
     &dnx_outlif_stress_test_man, NULL, NULL, CTEST_POSTCOMMIT}
    ,
    {"disable_eedb_data_banks", ctest_dnx_local_outlif_disable_eedb_data_banks, NULL,
     ctest_dnx_local_outlif_disable_eedb_data_banks_options,
     &dnx_outlif_stress_test_man, NULL, NULL, CTEST_POSTCOMMIT}
    ,
    {"inlif_semantic", ctest_dnx_local_inlif_semantic_test, NULL, NULL, &dnx_inlif_semantic_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,
    {"global_lif_semantic", ctest_dnx_global_lif_basic_test, NULL, NULL, &dnx_global_lif_semantic_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,
    {"Esem_cmd_allocation_mngr", ctest_dnx_lif_esem_cmd_allocation_semantic_test, NULL, NULL,
     &dnx_lif_esem_cmd_allocation_test_man,
     NULL, NULL, CTEST_POSTCOMMIT}
    ,
    {"WideData", NULL, sh_dnx_inlif_wide_data_test_cmds, NULL, &dnx_inlif_wide_data_man}

    ,
    {NULL}
};

shr_error_e dnx_in_lif_table_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e dnx_in_lif_child_field_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e dnx_out_lif_table_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e dnx_in_lif_table_replace_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e dnx_in_lif_child_field_replace_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e dnx_out_lif_table_replace_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t dnx_in_lif_table_unit_test_man;
extern sh_sand_man_t dnx_in_lif_child_field_test_man;
extern sh_sand_man_t dnx_out_lif_table_unit_test_man;
extern sh_sand_man_t dnx_in_lif_table_replace_test_man;
extern sh_sand_man_t dnx_in_lif_child_field_replace_test_man;
extern sh_sand_man_t dnx_out_lif_table_replace_test_man;

sh_sand_cmd_t dnx_lif_table_mngr_test_cmds[] = {
   /*************************************************************************************************************************************
    *   CMD_NAME    *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *               *                           * Level                   *                                *                            *
    *               *                           * CMD                     *                                *                            *
    *************************************************************************************************************************************/
    {"in_lif_table_unit", dnx_in_lif_table_unit_test, NULL, NULL, &dnx_in_lif_table_unit_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,

    {"in_lif_child_field_unit", dnx_in_lif_child_field_test, NULL, NULL, &dnx_in_lif_child_field_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,

    {"out_lif_table_unit", dnx_out_lif_table_unit_test, NULL, NULL, &dnx_out_lif_table_unit_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,

    {"in_lif_table_replace", dnx_in_lif_table_replace_test, NULL, NULL, &dnx_in_lif_table_replace_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,

    {"in_lif_child_field_replace", dnx_in_lif_child_field_replace_test, NULL, NULL,
     &dnx_in_lif_child_field_replace_test_man, NULL, NULL,
     CTEST_POSTCOMMIT}
    ,

    {"out_lif_table_replace", dnx_out_lif_table_replace_test, NULL, NULL, &dnx_out_lif_table_replace_test_man, NULL,
     NULL,
     CTEST_POSTCOMMIT}
    ,

    {NULL}
};
