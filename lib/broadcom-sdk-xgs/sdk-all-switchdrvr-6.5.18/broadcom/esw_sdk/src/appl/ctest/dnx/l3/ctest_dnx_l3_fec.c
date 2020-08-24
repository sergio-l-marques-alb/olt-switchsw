/** \file ctest_dnx_l3_fec.c
 * $Id$
 *
 * Tests for L3 fecs
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

 /*
  * Include files.
  * {
  */
#include <bcm/l3.h>
#include <bcm/failover.h>
#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#ifdef BSL_LOG_MODULE
#undef BSL_LOG_MODULE
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

/*
 * }
 */

/*
 * Globals.
 * {
 */

/** L3 fec test details */
static sh_sand_man_t sh_dnx_l3_fec_performance_man = {
    "Performance test for bcm_l3_egress_create INGRESS_ONLY",
    "Times the execution of the bcm_l3_egress_create API with BCM_L3_INGRESS_ONLY flag."
};
static sh_sand_man_t sh_dnx_l3_fec_allocation_man = {
    "Allocation test for bcm_l3_egress_create INGRESS_ONLY",
    "Verifies the FEC allocation with and without protection and creating ECMP groups."
};

/*
 * }
 */

/**
 * \brief
 * Main FEC performance testing command
 */
static shr_error_e
sh_dnx_l3_fec_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int idx;
    int rv = BCM_E_NONE;
    int is_active;
    int fec_id;
    uint32 fec_id_start;
    bcm_l3_egress_t fec_entry;
    uint32 nof_entries;
    uint32 port = 200;
    uint32 hits[2] = { 0, 0 };
    uint32 timer_idx[2] = { 1, 2 };
    uint32 total_time[2] = { 0, 0 };
    uint32 timers_group;
    uint32 average_time[2] = { 300, 180 };
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    char *name;

    SHR_FUNC_INIT_VARS(unit);
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 FEC TIME PERFORMANCE TEST START.\n")));

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get
                    (unit, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1, &fec_id_start, &nof_entries));

#if defined(ADAPTER_SERVER_MODE)
    nof_entries = 2000;
#endif
    sal_strncpy_s(group_name, "L3 fec add performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    bcm_l3_egress_t_init(&fec_entry);
    BCM_GPORT_SYSTEM_PORT_ID_SET(fec_entry.port, port);

    utilex_ll_timer_clear_all(timers_group);
    /** Set timer for creating the L3 FEC entries */
    printf("Adding L3 fec entries...\n");

    /** Iterate over nof_entries number of entries and create that many FEC entries with different ID. */
    for (idx = 0; idx < nof_entries; idx++)
    {
        fec_id = fec_id_start + idx;
        utilex_ll_timer_set("bcm_l3_egress_create", timers_group, timer_idx[0]);
        rv = bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY, &fec_entry, &fec_id);
        /** Stop timer for creating FEC entries */
        utilex_ll_timer_stop(timers_group, timer_idx[0]);
    }

    printf("Clearing the L3 fec table\n");
    bcm_l3_egress_t_init(&fec_entry);

    for (idx = 0; idx < nof_entries; idx++)
    {
        BCM_L3_ITF_SET(fec_id, BCM_L3_ITF_TYPE_FEC, fec_id_start + idx);
        utilex_ll_timer_set("bcm_l3_egress_destroy", timers_group, timer_idx[1]);
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fec_id));
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
    }

    /** Receive the total time that it took to add nof_entries number of FEC entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[0], &is_active, &name, &hits[0], &total_time[0]));
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[1], &is_active, &name, &hits[1], &total_time[1]));

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    printf("_______________________________________________________\n\n");
    printf("AVERAGE time for bcm_l3_egress_create - %u us, expected approximately %d us\n",
           (total_time[0] / nof_entries), average_time[0]);
    printf("AVERAGE time for bcm_l3_egress_destroy - %u us, expected approximately %d us\n",
           (total_time[1] / nof_entries), average_time[1]);
    printf("_______________________________________________________\n");

    /** Verify that the average execution time for adding a single entry is within limits. */
#if defined(ADAPTER_SERVER_MODE)
    /** Skip comparing with average expected time for cmodel execution. */
#else
    if ((total_time[0] / nof_entries) > (average_time[0] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "FEC add performance is out of 10%% limit - value should be less than %f\n",
                     (average_time[0] * 1.1));
    }
    if ((total_time[1] / nof_entries) > (average_time[1] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "FEC delete performance is out of 10%% limit - value should be less than %f\n",
                     (average_time[1] * 1.1));
    }

#endif
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 FEC TIME PERFORMANCE TEST END.\n")));

exit:
    rv = utilex_ll_timer_group_free(timers_group);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Main fec allocation testing command
 */
static shr_error_e
sh_dnx_l3_fec_allocation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int idx;
    const int nof_fecs = 100;
    const int intf_count = 10;
    bcm_l3_egress_ecmp_t *ecmp_list;
    bcm_l3_egress_t *primary_fec_list;
    bcm_l3_egress_t *failover_fec_list;
    bcm_l3_egress_t *fec_list;

    int *failover_fec_id_p = NULL;
    int *primary_fec_id_p = NULL;
    int *fec_id_p = NULL;

    bcm_failover_t fail_id = 200;
    bcm_if_t fecs_buffer[100];

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(failover_fec_id_p, sizeof(*failover_fec_id_p) * nof_fecs,
              "failover_fec_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(primary_fec_id_p, sizeof(*primary_fec_id_p) * nof_fecs,
              "primary_fec_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(fec_id_p, sizeof(*fec_id_p) * nof_fecs, "fec_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "FEC allocation test start\n")));
    /** allocate dynamic arrays */
    primary_fec_list = sal_alloc(sizeof(bcm_l3_egress_t) * nof_fecs, "fec_allocation_main.primary_fec_list");
    failover_fec_list = sal_alloc(sizeof(bcm_l3_egress_t) * nof_fecs, "fec_allocation_main.failover_fec_list");
    fec_list = sal_alloc(sizeof(bcm_l3_egress_t) * nof_fecs, "fec_allocation_main.fec_list");
    ecmp_list = sal_alloc(sizeof(bcm_l3_egress_ecmp_t) * nof_fecs, "fec_allocation_main.ecmp_list");

    /** Creating the Failover ID */
    SHR_IF_ERR_EXIT(bcm_failover_create(unit, BCM_FAILOVER_FEC, &fail_id));

    /** STAGE 1: Create FECs with and without protection */
    /** Create FEC pairs with protection - primary and failover */
    for (idx = 0; idx < nof_fecs; idx++)
    {
        bcm_l3_egress_t_init(&failover_fec_list[idx]);
        failover_fec_list[idx].failover_id = fail_id;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create
                        (unit, BCM_L3_INGRESS_ONLY, &failover_fec_list[idx], &failover_fec_id_p[idx]));

        bcm_l3_egress_t_init(&primary_fec_list[idx]);
        primary_fec_list[idx].failover_id = fail_id;
        primary_fec_list[idx].failover_if_id = failover_fec_id_p[idx];
        SHR_IF_ERR_EXIT(bcm_l3_egress_create
                        (unit, BCM_L3_INGRESS_ONLY, &primary_fec_list[idx], &primary_fec_id_p[idx]));
    }

    /** Create FEC list without protection */
    for (idx = 0; idx < nof_fecs; idx++)
    {
        bcm_l3_egress_t_init(&fec_list[idx]);
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &fec_list[idx], &fec_id_p[idx]));
    }

    for (idx = 0; idx < nof_fecs; idx++)
    {
        uint8 is_allocated = FALSE;
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(failover_fec_id_p[idx]), &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FEC %d is not found but it should be allocated\n", failover_fec_id_p[idx]);
        }
        is_allocated = FALSE;
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(primary_fec_id_p[idx]), &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FEC %d is not found but it should be allocated\n", primary_fec_id_p[idx]);
        }
        is_allocated = FALSE;
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(fec_id_p[idx]), &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FEC %d is not found but it should be allocated\n", fec_id_p[idx]);
        }
    }

    /** STAGE 2: Create ECMP groups */
    /** Case 1: Valid input - FECs without protection */
    bcm_l3_egress_ecmp_t_init(&ecmp_list[0]);
    ecmp_list[0].max_paths = intf_count;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_list[0], intf_count, fec_id_p));

    /** Case 2: valid input - FECs with protection (primary)*/
    bcm_l3_egress_ecmp_t_init(&ecmp_list[1]);
    ecmp_list[1].max_paths = intf_count;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_list[1], intf_count, primary_fec_id_p));

    /** Case 3: invalid input - regular FECs unordered - size doesn't match lowest and biggest element */
    fecs_buffer[0] = fec_id_p[31];
    fecs_buffer[1] = fec_id_p[14];
    fecs_buffer[2] = fec_id_p[15];
    fecs_buffer[3] = fec_id_p[12];
    fecs_buffer[4] = fec_id_p[16];
    bcm_l3_egress_ecmp_t_init(&ecmp_list[2]);
    ecmp_list[2].max_paths = intf_count / 2;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_create(unit, &ecmp_list[2], intf_count / 2, fecs_buffer),
                              _SHR_E_PARAM);

    /** Case 4: invalid input - different protection state. */
    fecs_buffer[0] = fec_id_p[13];
    fecs_buffer[1] = primary_fec_id_p[11];
    fecs_buffer[2] = fec_id_p[17];
    fecs_buffer[3] = primary_fec_id_p[12];
    fecs_buffer[4] = fec_id_p[18];
    bcm_l3_egress_ecmp_t_init(&ecmp_list[3]);
    ecmp_list[3].max_paths = intf_count / 2;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_create(unit, &ecmp_list[3], intf_count / 2, fecs_buffer),
                              _SHR_E_PARAM);

    /** Case 5: invalid input - different protection state */
    fecs_buffer[0] = fec_id_p[19];
    fecs_buffer[1] = failover_fec_id_p[13];
    fecs_buffer[2] = fec_id_p[2];
    fecs_buffer[3] = failover_fec_id_p[12];
    fecs_buffer[4] = fec_id_p[3];
    bcm_l3_egress_ecmp_t_init(&ecmp_list[8]);
    ecmp_list[8].max_paths = intf_count / 2;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_create(unit, &ecmp_list[4], intf_count / 2, fecs_buffer),
                              _SHR_E_PARAM);

    /** Case 6: invalid input - failover FECs */
    bcm_l3_egress_ecmp_t_init(&ecmp_list[4]);
    ecmp_list[4].max_paths = intf_count / 2;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_create(unit, &ecmp_list[5], intf_count / 2, &failover_fec_id_p[60]),
                              _SHR_E_PARAM);

    /** Case 7: valid input - FECs with no protection */
    bcm_l3_egress_ecmp_t_init(&ecmp_list[6]);
    ecmp_list[6].max_paths = intf_count;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_list[6], intf_count, &fec_id_p[60]));

    /** Case 8: valid input - FECs with protection */
    bcm_l3_egress_ecmp_t_init(&ecmp_list[7]);
    ecmp_list[7].max_paths = intf_count;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_list[7], intf_count, &primary_fec_id_p[60]));

    /** STAGE 3: adding FECs to existing ECMP groups */
    /** Case 1: adding a FEC without protection to an ECMP which has no protection - correct index*/
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp_list[0], fec_id_p[10]));

    /** Case 2: adding a FEC with protection to an ECMP without protection */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[0], primary_fec_id_p[39]), _SHR_E_PARAM);

    /** Case 3: adding a FEC without protection to an ECMP with protection */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[1], fec_id_p[40]), _SHR_E_PARAM);

    /** Case 4: adding a FEC with protection to ECMP with protection - correct index */
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp_list[1], primary_fec_id_p[10]));

    /** Case 5: adding a FEC without protection to an ECMP without protection - index is out of range. */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[6], fec_id_p[2]), _SHR_E_PARAM);

    /** Case 6: adding a FEC with protection to ECMP without protection. */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[6], primary_fec_id_p[45]), _SHR_E_PARAM);

    /** Case 7: adding a FEC to a group that doesn't exist. */
    ecmp_list[8].ecmp_intf = 8;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[8], fec_id_p[50]), _SHR_E_NOT_FOUND);

    /** STAGE 4: Removing FECs from ECMP groups */
    /** Case 1: removing a FEC without protection from an ECMP without protection - from the beginning */
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[6], fec_id_p[60]));

    /** Case 2: removing a FEC without protection from an ECMP without protection - from the middle */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[6], fec_id_p[65]), _SHR_E_PARAM);

    /** Case 3: removing a FEC with protection from an ECMP with protection - from the middle */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[1], primary_fec_id_p[5]), _SHR_E_PARAM);

    /** Case 3: removing a FEC with protection from an ECMP with protection - from the middle */
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[1], primary_fec_id_p[10]));

    /** Case 4: removing a FEC without protection from an ECMP group to which it doesn't belong. */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[1], fec_id_p[0]), _SHR_E_PARAM);

    /** STAGE 5: Clean up */
    /** destroying ECMP groups */
    for (idx = 0; idx < 8; idx++)
    {
        SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_destroy(unit, &ecmp_list[idx]), _SHR_E_PARAM);
    }

    /** Destroying FECs */
    for (idx = 0; idx < nof_fecs; idx++)
    {
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fec_id_p[idx]));
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, primary_fec_id_p[idx]));
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, failover_fec_id_p[idx]));
    }

    /** Destroying failover ID */
    SHR_IF_ERR_EXIT(bcm_failover_destroy(unit, fail_id));

    LOG_INFO(BSL_LS_BCM_FCOE, (BSL_META_U(unit, "FEC allocation test end. PASS\n")));
exit:
    SHR_FREE(failover_fec_id_p);
    SHR_FREE(primary_fec_id_p);
    SHR_FREE(fec_id_p);
    sal_free(primary_fec_list);
    sal_free(failover_fec_list);
    sal_free(fec_list);
    sal_free(ecmp_list);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX L3 fec Tests
 * List of fec ctests modules.
 */
sh_sand_cmd_t dnx_l3_fec_test_cmds[] = {
    {"performance", sh_dnx_l3_fec_performance_cmd, NULL, NULL, &sh_dnx_l3_fec_performance_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"allocation", sh_dnx_l3_fec_allocation_cmd, NULL, NULL, &sh_dnx_l3_fec_allocation_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE
