/** \file ctest_dnx_l3_rif.c
 * $Id$
 *
 * Tests for L3 ing intf create
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
#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm/l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>
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

/** L3 intf test details */
static sh_sand_man_t sh_dnx_l3_ing_intf_performance_man = {
    "Performance test for RIF",
    "Times the execution of the bcm_l3_intf_create and bcm_l3_ingress_create.",
    "ctest l3 rif performance [count=<Nof_intfs>]",
    "count=3000",
};

static sh_sand_option_t sh_dnx_l3_ing_intf_performance_options[] = {
    {"count", SAL_FIELD_TYPE_UINT32, "Number of VSIs to add", "4000"},
    {NULL}
};

/*
 * }
 */

/**
 * \brief
 * Main intf performance testing command
 */
static shr_error_e
sh_dnx_l3_ing_intf_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int entry_id;
    int rv = BCM_E_NONE;
    int is_active;
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t l3_ing_if;
    uint32 nof_entries;
    uint32 hits = 0;
    uint32 timer_idx[2] = { 1, 2 };
    uint32 total_time[2];
    uint32 timers_group;
    uint32 average_time[2] = { 1000, 100 };
    uint32 vsi = 1;
    bcm_mac_t my_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    char *name;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 intf CREATE TIME PERFORMANCE TEST START.\n")));

    SH_SAND_GET_UINT32("count", nof_entries);
#if defined(ADAPTER_SERVER_MODE)
    nof_entries = 1000;
#endif

    if (nof_entries == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot run the intf performance test with 0 number of entries\n");
    }

    if (nof_entries > dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The provided number of entries is invalid, modify rif_id_max soc property\n");
    }
    printf("Running the L3 intf create performance test with %d number of entries\n", nof_entries);

    sal_memset(group_name, 0, sizeof(char) * UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME);
    sal_strncpy_s(group_name, "L3 intf create perf", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    utilex_ll_timer_clear_all(timers_group);

    printf("Adding %d number of entries\n", nof_entries);
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, my_mac, sizeof(bcm_mac_t));
    l3if.dscp_qos.qos_map_id = 0;

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ing_if.vrf = 1;
    l3_ing_if.qos_map_id = 0;

    /** Set timer for creating the L3 intf entries */
    /** Iterate over nof_entries number of entries and create that many intf entries with different VSI value. */
    for (entry_id = 0; entry_id < nof_entries; entry_id++)
    {

        int intf_id = vsi + entry_id;
        l3if.l3a_intf_id = intf_id;
        l3if.l3a_vid = intf_id;
        utilex_ll_timer_set("bcm_l3_intf_create create", timers_group, timer_idx[0]);
        rv = bcm_l3_intf_create(unit, &l3if);
        utilex_ll_timer_stop(timers_group, timer_idx[0]);
        if (rv != BCM_E_NONE)
        {
            break;
        }
        utilex_ll_timer_set("bcm_l3_ingress_create create", timers_group, timer_idx[1]);
        rv = bcm_l3_ingress_create(unit, &l3_ing_if, &intf_id);
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
        if (rv != BCM_E_NONE)
        {
            break;
        }
    }
    /** Stop timer for creating entries */

    printf("Added %d number of entries\n", entry_id);
    if (entry_id != nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not add requested number of entries. Added %u out of %u\n",
                     entry_id, nof_entries);
    }
    printf("Clearing the intf table\n");
    SHR_IF_ERR_EXIT(bcm_l3_intf_delete_all(unit));

    /** Receive the total time that it took to add nof_entries number of Intf entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[0], &is_active, &name, &hits, &total_time[0]));
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[1], &is_active, &name, &hits, &total_time[1]));
    printf("_______________________________________________________\n\n");
    printf("AVERAGE time for bcm_l3_intf_create - %u us, expected approximately %u\n",
           (total_time[0] / nof_entries), average_time[0]);
    printf("AVERAGE time for bcm_l3_intf_create - %u us, expected approximately %u\n",
           (total_time[1] / nof_entries), average_time[1]);
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
                     "Performance of bcm_l3_intf_create is out of limits - value should be less than %f\n",
                     (average_time[0] * 1.1));
    }
    if ((total_time[1] / nof_entries) > (average_time[1] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Performance of bcm_l3_ingress_create is out of limits - value should be less than %f\n",
                     (average_time[1] * 1.1));
    }
#endif

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "L3 intf CREATE TIME PERFORMANCE TEST END.\n")));

exit:
    rv = utilex_ll_timer_group_free(timers_group);
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX L3 intf Tests
 * List of intf ctests modules.
 */
sh_sand_cmd_t dnx_l3_ing_intf_test_cmds[] = {
    {"performance", sh_dnx_l3_ing_intf_performance_cmd, NULL, sh_dnx_l3_ing_intf_performance_options, &sh_dnx_l3_ing_intf_performance_man, NULL, NULL, CTEST_POSTCOMMIT},

    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE
