/** \file ctest_dnx_l3_host.c
 * $Id$
 *
 * Tests for L3 hosts
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
 * Enumeration
 * {
 */
/*
 * }
 */

/*
 * Structures
 * {
 */
/*
 * }
 */

/*
 * Externs
 * {
 */

/*
 * }
 */
/*
 * Globals.
 * {
 */

/** L3 host test details */
static sh_sand_man_t sh_dnx_l3_host_performance_man = {
    "Performance test for bcm_l3_host_add",
    "Times the execution of the bcm_l3_host_add."
};

/*
 * }
 */

/**
 * \brief
 * Main Host performance testing command
 */
static shr_error_e
sh_dnx_l3_host_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int idx;
    int rv = BCM_E_NONE;
    int is_active;
    bcm_l3_host_t host;
    uint32 dip_v4 = 0x00120000;
    uint32 nof_entries = 8000;
    uint32 dest = 0x2000A000;
    uint32 vrf = 5;
    uint32 hits = 0;
    uint32 timer_idx[2] = { 1, 2 };
    uint32 total_time[2];
    uint32 timers_group;
    uint32 average_time[2] = { 765, 510 };
    bcm_ip6_t dip_v6 = { 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0, 0, 0, 0, 0, 0, 0, 0 };
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    char *name;

    SHR_FUNC_INIT_VARS(unit);
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 HOST ADD TIME PERFORMANCE TEST START.\n")));

#if defined(ADAPTER_SERVER_MODE)
    nof_entries = 2000;
    average_time[0] = 1550;
    average_time[1] = 1250;
#endif
    sal_strncpy(group_name, "L3 Host add performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    bcm_l3_host_t_init(&host);
    host.l3a_intf = dest;
    host.l3a_vrf = vrf;

    utilex_ll_timer_clear_all(timers_group);
    /** Set timer for creating the L3 Ipv4 host entries */
    printf("Adding IPv4 L3 host entries...\n");
    utilex_ll_timer_set("bcm_l3_host_add IPv4 create", timers_group, timer_idx[0]);
    /** Iterate over nof_entries number of entries and create that many IPv4 host entries with different IP. */
    for (idx = 0; idx < nof_entries; idx++)
    {
        host.l3a_ip_addr = dip_v4;
        rv = bcm_l3_host_add(unit, &host);
        dip_v4++;
    }
    /** Stop timer for creating Ipv4 entries */
    utilex_ll_timer_stop(timers_group, timer_idx[0]);
    printf("Clearing the L3 host table\n");
    bcm_l3_host_t_init(&host);
    SHR_IF_ERR_EXIT(bcm_l3_host_delete_all(unit, &host));

    host.l3a_intf = dest;
    host.l3a_vrf = vrf;
    host.l3a_flags = BCM_L3_IP6;
    printf("Adding IPv6 L3 host entries...\n");
    utilex_ll_timer_set("bcm_l3_host_add IPv6 create", timers_group, timer_idx[1]);
    /** Iterate over nof_entries number of entries and create that many IPv6 host entries with different IP. */
    for (idx = 0; idx < nof_entries; idx++)
    {
        sal_memcpy(host.l3a_ip6_addr, dip_v6, sizeof(bcm_ip6_t));
        rv = bcm_l3_host_add(unit, &host);
        dip_v6[15]++;
    }
    utilex_ll_timer_stop(timers_group, timer_idx[1]);
    printf("Clearing the L3 host table\n");
    bcm_l3_host_t_init(&host);
    SHR_IF_ERR_EXIT(bcm_l3_host_delete_all(unit, &host));

    /** Receive the total time that it took to add nof_entries number of IP entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[0], &is_active, &name, &hits, &total_time[0]));
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[1], &is_active, &name, &hits, &total_time[1]));

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    printf("_______________________________________________________\n\n");
    printf("AVERAGE time for IPV4 bcm_l3_host_add - %u us, expected approximately %u\n", (total_time[0] / nof_entries),
           average_time[0]);
    printf("AVERAGE time for IPV6 bcm_l3_host_add - %u us, expected approximately %u\n", (total_time[1] / nof_entries),
           average_time[1]);
    printf("_______________________________________________________\n");

    /** Verify that the average execution time for adding a single entry is within limits. */
#if defined(ADAPTER_SERVER_MODE)
    /** Skip comparing with average expected time for cmodel execution. */
#else
    if ((total_time[0] / nof_entries) > (average_time[0] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "IPv4 host add performance is out of limits - value should be less than %f\n",
                     (average_time[0] * 1.1));
    }
    if ((total_time[1] / nof_entries) > (average_time[1] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "IPv6 host add performance is out of limits - value should be less than %f\n",
                     (average_time[1] * 1.1));
    }
#endif
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 HOST ADD TIME PERFORMANCE TEST END.\n")));

exit:
    rv = utilex_ll_timer_group_free(timers_group);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX L3 Host Tests
 * List of host ctests modules.
 */
sh_sand_cmd_t dnx_l3_host_test_cmds[] = {
    {"performance", sh_dnx_l3_host_performance_cmd, NULL, NULL, &sh_dnx_l3_host_performance_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE
