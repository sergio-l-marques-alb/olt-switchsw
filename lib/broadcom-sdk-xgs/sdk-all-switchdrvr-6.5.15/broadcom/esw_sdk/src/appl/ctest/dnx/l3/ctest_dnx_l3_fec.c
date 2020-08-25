/** \file ctest_dnx_l3_fec.c
 * $Id$
 *
 * Tests for L3 fecs
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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
#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
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

/*
 * }
 */

/**
 * \brief
 * Main fec performance testing command
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
    int fec_id_start = 0xB000;
    bcm_l3_egress_t fec_entry;
    uint32 nof_entries = 8000;
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

#if defined(ADAPTER_SERVER_MODE)
    nof_entries = 2000;
#endif
    sal_strncpy(group_name, "L3 fec add performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    bcm_l3_egress_t_init(&fec_entry);
    fec_entry.port = port;

    utilex_ll_timer_clear_all(timers_group);
    /** Set timer for creating the L3 fec entries */
    printf("Adding L3 fec entries...\n");

    /** Iterate over nof_entries number of entries and create that many fec entries with different ID. */
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

/* *INDENT-OFF* */
/**
 * \brief DNX L3 fec Tests
 * List of fec ctests modules.
 */
sh_sand_cmd_t dnx_l3_fec_test_cmds[] = {
    {"performance", sh_dnx_l3_fec_performance_cmd, NULL, NULL, &sh_dnx_l3_fec_performance_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE
