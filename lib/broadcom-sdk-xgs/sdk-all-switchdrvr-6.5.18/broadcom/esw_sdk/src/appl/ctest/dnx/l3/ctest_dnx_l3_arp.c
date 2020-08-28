/** \file ctest_dnx_l3_arp.c
 * $Id$
 *
 * Tests for L3 ARP
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

/** L3 ARP test details */
static sh_sand_man_t sh_dnx_l3_arp_performance_man = {
    "Performance test for bcm_l3_egress_create EGRESS_ONLY",
    "Times the execution of the bcm_l3_egress_create API with BCM_L3_EGRESS_ONLY flag."
};

/*
 * }
 */

/**
 * \brief
 * Main ARP performance testing command
 */
static shr_error_e
sh_dnx_l3_arp_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 hits[2] = { 0, 0 };
    uint32 timer_idx[2] = { 1, 2 };
    uint32 total_time[2] = { 0, 0 };
    uint32 timers_group;
    uint32 average_time[2] = { 550, 600 };
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    char *name;
    int is_active;
    int fec_id, lif_id;
    int rv, idx;
    int lif_id_start = 0x1000;
    int nof_entries = 4000;
    bcm_l3_egress_t arp_entry;
    bcm_mac_t mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };

    SHR_FUNC_INIT_VARS(unit);
#if defined(ADAPTER_SERVER_MODE)
    nof_entries = 1000;
#endif
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 ARP TIME PERFORMANCE TEST START.\n")));
    sal_strncpy_s(group_name, "L3 ARP create perf", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    bcm_l3_egress_t_init(&arp_entry);
    utilex_ll_timer_clear_all(timers_group);
    /** Set timer for creating the L3 ARP entries */
    printf("Adding L3 ARP entries...\n");
    /** Iterate over nof_entries number of entries and create that many FEC entries with different ID. */
    for (idx = 0; idx < nof_entries; idx++)
    {
        lif_id = lif_id_start + idx;
        arp_entry.encap_id = lif_id;
        sal_memcpy(arp_entry.mac_addr, mac, sizeof(bcm_mac_t));
        arp_entry.vlan = idx + 1;
        utilex_ll_timer_set("bcm_l3_egress_create", timers_group, timer_idx[0]);
        rv = bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_EGRESS_ONLY, &arp_entry, &fec_id);
        /** Stop timer for creating ARP entries */
        utilex_ll_timer_stop(timers_group, timer_idx[0]);
    }

    printf("Clearing the L3 ARP table\n");
    bcm_l3_egress_t_init(&arp_entry);

    for (idx = 0; idx < nof_entries; idx++)
    {
        BCM_L3_ITF_SET(lif_id, BCM_L3_ITF_TYPE_LIF, lif_id_start + idx);
        utilex_ll_timer_set("bcm_l3_egress_destroy", timers_group, timer_idx[1]);
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, lif_id));
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
    }

    /** Receive the total time that it took to add nof_entries number of ARP entries*/
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
                     "ARP create performance is out of 10%% limit - value should be less than %f\n",
                     (average_time[0] * 1.1));
    }
    if ((total_time[1] / nof_entries) > (average_time[1] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ARP delete performance is out of 10%% limit - value should be less than %f\n",
                     (average_time[1] * 1.1));
    }

#endif
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 ARP TIME PERFORMANCE TEST END.\n")));

exit:
    rv = utilex_ll_timer_group_free(timers_group);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX L3 ARP Tests
 * List of ARP ctests.
 */
sh_sand_cmd_t dnx_l3_arp_test_cmds[] = {
    {"performance", sh_dnx_l3_arp_performance_cmd, NULL, NULL, &sh_dnx_l3_arp_performance_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE
