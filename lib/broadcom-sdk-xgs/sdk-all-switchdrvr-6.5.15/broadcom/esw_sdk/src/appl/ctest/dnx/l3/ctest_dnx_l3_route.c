/** \file ctest_dnx_l3_route.c
 * $Id$
 *
 * Tests for L3 routes
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
#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm/l3.h>
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

/** L3 route test details */
static sh_sand_man_t sh_dnx_l3_route_performance_man = {
    "Performance test for bcm_l3_route_add",
    "Times the execution of the bcm_l3_route_add.",
    "ctest l3 route performance [count=<Nof_routes>]",
    "count=5000",
};

static sh_sand_option_t sh_dnx_l3_route_performance_options[] = {
    {"count", SAL_FIELD_TYPE_UINT32, "Number of routes to add", "6000"},
    {NULL}
};

/*
 * }
 */

/**
 * \brief
 * Main route performance testing command
 */
static shr_error_e
sh_dnx_l3_route_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int entry_id;
    int idx;
    int rv = BCM_E_NONE;
    int is_active;
    bcm_l3_route_t route;
    uint32 dip = 0x00120000;
    uint32 dip_v4_mask = 0xFFFFFFFF;
    uint32 nof_entries;
    uint32 dest = 0x2000A000;
    uint32 vrf = 5;
    uint32 prefix_length = 32;
    const uint32 max_prefix_length = 32;
    const uint32 nof_ipv6_addr_bytes = 16;
    uint32 hits = 0;
    uint32 timer_idx[2] = { 1, 2 };
    uint32 total_time[2];
    uint32 timers_group;
    uint32 average_time[2] = { 150, 150 };
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    char *name;

    SHR_FUNC_INIT_VARS(unit);
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 route ADD TIME PERFORMANCE TEST START.\n")));

    SH_SAND_GET_UINT32("count", nof_entries);

    printf("Adding %d number of entries for both IPv4 and IPv6\n", nof_entries);
    sal_strncpy(group_name, "L3 route add performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    bcm_l3_route_t_init(&route);
    route.l3a_intf = dest;
    route.l3a_vrf = vrf;
    route.l3a_ip_mask = dip_v4_mask;

    utilex_ll_timer_clear_all(timers_group);
    printf("Adding IPv4 L3 route entries...\n");

    /** Iterate over nof_entries number of entries and create that many IPv4 route entries with different IP. */
    for (entry_id = 0; entry_id < nof_entries; entry_id++)
    {
        dip = (1 << (max_prefix_length - prefix_length)) * (entry_id / 15 + 1);
        route.l3a_ip_mask = dip_v4_mask << (max_prefix_length - prefix_length);
        route.l3a_subnet = dip;
        /** Set timer for creating the L3 Ipv4 route entries */
        utilex_ll_timer_set("bcm_l3_route_add IPv4 create", timers_group, timer_idx[0]);
        rv = bcm_l3_route_add(unit, &route);
        /** Stop timer for creating Ipv4 entries */
        utilex_ll_timer_stop(timers_group, timer_idx[0]);
        if (rv != BCM_E_NONE)
        {
            break;
        }
        prefix_length--;
        if (prefix_length <= 17)
        {
            prefix_length = max_prefix_length;
        }
    }

    if (entry_id == nof_entries)
    {
        printf("Added %d number of entries\n", entry_id);
    }
    else
    {
        printf("Added %d number of entries\n", entry_id + 1);
    }
    printf("Clearing the L3 route table\n");
    bcm_l3_route_t_init(&route);
    SHR_IF_ERR_EXIT(bcm_l3_route_delete_all(unit, &route));

    route.l3a_intf = dest;
    route.l3a_vrf = vrf;
    route.l3a_flags = BCM_L3_IP6;
    printf("Adding IPv6 L3 route entries...\n");
    prefix_length = max_prefix_length;

    /** Iterate over nof_entries number of entries and create that many IPv6 route entries with different IP. */
    for (entry_id = 0; entry_id < nof_entries; entry_id++)
    {
        dip = (1 << (max_prefix_length - prefix_length)) * (entry_id / 15 + 1);
        for (idx = 1; idx < nof_ipv6_addr_bytes; idx++)
        {
            route.l3a_ip6_net[idx] = (dip >> ((idx % 4) * 8)) & 0xFF;
        }
        bcm_ip6_mask_create(route.l3a_ip6_mask, prefix_length);
        utilex_ll_timer_set("bcm_l3_route_add IPv6 create", timers_group, timer_idx[1]);
        rv = bcm_l3_route_add(unit, &route);
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
        if (rv != BCM_E_NONE)
        {
            break;
        }
        prefix_length--;
        if ((prefix_length <= 18))
        {
            prefix_length = max_prefix_length;
        }
    }

    if (entry_id == nof_entries)
    {
        printf("Added %d number of entries\n", entry_id);
    }
    else
    {
        printf("Added %d number of entries\n", entry_id + 1);
    }

    printf("Clearing the L3 route table\n");
    bcm_l3_route_t_init(&route);
    SHR_IF_ERR_EXIT(bcm_l3_route_delete_all(unit, &route));

    /** Receive the total time that it took to add nof_entries number of IP entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[0], &is_active, &name, &hits, &total_time[0]));
    printf("_______________________________________________________\n\n");
    printf("AVERAGE time for IPV4 bcm_l3_route_add - %u us, expected approximately %u\n", (total_time[0] / hits),
           average_time[0]);
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[1], &is_active, &name, &hits, &total_time[1]));
    printf("AVERAGE time for IPV6 bcm_l3_route_add - %u us, expected approximately %u\n", (total_time[1] / hits),
           average_time[1]);
    printf("_______________________________________________________\n");

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    /** Verify that the average execution time for adding a single entry is within limits. */
#if defined(ADAPTER_SERVER_MODE)
    /** Skip comparing with average expected time for cmodel execution. */
#else
    if ((total_time[0] / nof_entries) > (average_time[0] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "IPv4 route add performance is out of limits - value should be less than %f\n",
                     (average_time[0] * 1.1));
    }
    if ((total_time[1] / nof_entries) > (average_time[1] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "IPv6 route add performance is out of limits - value should be less than %f\n",
                     (average_time[1] * 1.1));
    }
#endif
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 route ADD TIME PERFORMANCE TEST END.\n")));

exit:
    rv = utilex_ll_timer_group_free(timers_group);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX L3 route Tests
 * List of route ctests modules.
 */
sh_sand_cmd_t dnx_l3_route_test_cmds[] = {
    {"performance", sh_dnx_l3_route_performance_cmd, NULL, sh_dnx_l3_route_performance_options, &sh_dnx_l3_route_performance_man, NULL, NULL, CTEST_POSTCOMMIT},

    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE
