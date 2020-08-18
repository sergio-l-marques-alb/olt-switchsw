/*
 * ! \file mdb_diag.c Contains all of the KBP ctest commands
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_KBPDNX
#include <shared/bsl.h>

 /*
  * Include files.
  * {
  */
#include <soc/dnx/dbal/dbal_string_mgmt.h>

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#include "ctest_dnx_kbp.h"
#include "ctest_dnx_kbp_stif.h"

/*
 * }
 */
/*
 * DEFINEs
 * {
 */

/**
 * \brief
 * String used for representing ACL master key test
 */
#define CTEST_DNX_KBP_ACL_MASTER_KEY    "master_key"
#define CTEST_DNX_KBP_ACL_LOOKUP        "lookup"
#define CTEST_DNX_KBP_ACL_ENTRY_SEM     "entry_sem"
#define CTEST_DNX_KBP_ACL_OPCODE_CREATE     "opcode_create"
#define CTEST_DNX_KBP_ACL_OPCODE        "opcode"

#define CTEST_DNX_KBP_FWD_IPV4_RANDOM              "ipv4_random"
#define CTEST_DNX_KBP_FWD_IPV4_RANDOM_ENTRY_NUM    "EntryNum"
#define CTEST_DNX_KBP_FWD_IPV4_RANDOM_ENTRY_NUM_PRINT_MOD   "EntryNumPrintMod"
#define CTEST_DNX_KBP_FWD_IPV4_RANDOM_CACHED_ENT_NUM    "CachedEntNum"
#define CTEST_DNX_KBP_FWD_IPV4_RANDOM_WARMBOOT     "WARMBOOT"
#define CTEST_DNX_KBP_FWD_IPV4_RANDOM_UPDATE       "Update"
#define CTEST_DNX_KBP_FWD_IPV4_RANDOM_DELETE       "DeLeTe"
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
#if defined(INCLUDE_KBP)
#define CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK											   		\
{																									\
	if (!SAL_BOOT_PLISIM)   																	   	\
		{   																					   	\
			if (!dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4)) 	   	\
			{   																				   	\
				LOG_CLI((BSL_META("test not supported when ipv4 not enabled in KBP\n")));   	   	\
				SHR_EXIT(); 																	   	\
			}   																				   	\
		}																							\
}
#else
#define CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK										\
{																						\
	LOG_CLI((BSL_META("test not supported KBP not defined\n")));   	   					\
	SHR_EXIT(); 																	   	\
}
#endif

#define CTEST_DNX_KBP_LOGGER_INIT_VARS bsl_severity_t original_severity_kbp_mngr, orignal_severity_dbal, \
    orignal_severity_fldproc

#define CTEST_DNX_KBP_LOGGER_INIT() \
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_KBP_MNGR, original_severity_kbp_mngr); \
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, orignal_severity_dbal) \
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, orignal_severity_fldproc)
#if 1
#define CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(_A_) \
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_KBP_MNGR, bslSeverityFatal); \
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal); \
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal); \
    SHR_SET_CURRENT_ERR(_A_); \
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, orignal_severity_fldproc); \
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, orignal_severity_dbal); \
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_KBP_MNGR, original_severity_kbp_mngr)
#else
#define CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(_A_) SHR_SET_CURRENT_ERR(_A_)
#endif
/*
 * }
 */

/**********************************************************************************************************************/
/** FWD */
/**********************************************************************************************************************/

/**
 * The following ctest exercises adding, updating and deleting L3 route entries.
 * Each step is being timed.
 */
shr_error_e
dnx_kbp_fwd_ipv4_random(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_entries;
    uint32 nof_entries_print_mod;
    uint32 nof_entries_cached;
    uint32 prefix_length = 32;
    unsigned int warmboot, update, delete;
    bcm_l3_route_t route_info;
    int idx, jdx, rv;
    bcm_ip_t dip;
    bcm_vrf_t vrf = 5;
    bcm_if_t fec_idx = 2000;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    uint32 timer_idx[3] = { 1, 2, 3 };
    uint32 timers_group;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(CTEST_DNX_KBP_FWD_IPV4_RANDOM_ENTRY_NUM, nof_entries);
    SH_SAND_GET_UINT32(CTEST_DNX_KBP_FWD_IPV4_RANDOM_ENTRY_NUM_PRINT_MOD, nof_entries_print_mod);
    SH_SAND_GET_UINT32(CTEST_DNX_KBP_FWD_IPV4_RANDOM_CACHED_ENT_NUM, nof_entries_cached);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_FWD_IPV4_RANDOM_WARMBOOT, warmboot);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_FWD_IPV4_RANDOM_UPDATE, update);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_FWD_IPV4_RANDOM_DELETE, delete);

    sal_strncpy(group_name, "KBP IPv4 random", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);

#if defined(INCLUDE_KBP)
    if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));
    }
#endif

    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    /** WB is currently disabled */
    if (warmboot != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Warmboot mode is not supported\n");
    }
    if (nof_entries_cached != 0)
    {
        cli_out("Warning: CachedEntNum value is different than 0 and it is being ignored.\n");
    }

    cli_out("step1: Add routes\n");
    utilex_ll_timer_clear_all(timers_group);
    /** Set timer for creating the L3 route entries */
    utilex_ll_timer_set("bcm_l3_route_add create", timers_group, timer_idx[0]);
    /** Iterate over the given number of entries to create */
    for (idx = 0; idx < (nof_entries / nof_entries_print_mod); idx++)
    {
        for (jdx = 0; jdx < nof_entries_print_mod; jdx++)
        {
            bcm_l3_route_t_init(&route_info);

            dip = (1 << (32 - prefix_length)) * ((jdx + idx * nof_entries_print_mod) / 15 + 1);
            route_info.l3a_vrf = vrf;
            route_info.l3a_intf = fec_idx;
            route_info.l3a_subnet = dip;
            route_info.l3a_ip_mask = 0xffffffff << (32 - prefix_length);
            rv = bcm_l3_route_add(unit, &route_info);
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "Error, bcm_l3_route_add Failed- %d %d, route_info.l3a_subnet=0x%x\n", idx, jdx,
                             route_info.l3a_subnet);
            }
            fec_idx++;
            prefix_length--;
            if (prefix_length <= 17)
            {
                prefix_length = 32;
                dip++;
            }
        }
        if (!dnx_kbp_device_enabled(unit))
        {
            bcm_switch_control_set(unit, bcmSwitchDescCommit, 1);
        }
    }
    /** Stop timer for creating entries */
    utilex_ll_timer_stop(timers_group, timer_idx[0]);
    if (!dnx_kbp_device_enabled(unit))
    {
        bcm_switch_control_set(unit, bcmSwitchDescCommit, 1);
    }

    /** Iterate over entries to update if update indication is ON */
    if (update)
    {
        /** Set timer to updating the entries */
        utilex_ll_timer_set("bcm_l3_route_add update", timers_group, timer_idx[1]);
        cli_out("step2: Update routes\n");
        fec_idx = 3000;
        prefix_length = 32;
        for (idx = 0; idx < (nof_entries / nof_entries_print_mod); idx++)
        {
            for (jdx = 0; jdx < nof_entries_print_mod; jdx++)
            {
                bcm_l3_route_t_init(&route_info);
                dip = (1 << (32 - prefix_length)) * ((jdx + idx * nof_entries_print_mod) / 15 + 1);
                route_info.l3a_vrf = vrf;
                route_info.l3a_intf = fec_idx;
                route_info.l3a_subnet = dip;
                route_info.l3a_ip_mask = 0xffffffff << (32 - prefix_length);
                route_info.l3a_flags |= BCM_L3_REPLACE;
                rv = bcm_l3_route_add(unit, &route_info);
                if (rv != BCM_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "Error, bcm_l3_route_add Failed, route_info.l3a_subnet=0x%x\n",
                                 route_info.l3a_subnet);
                }

                fec_idx++;
                prefix_length--;
                if (prefix_length <= 17)
                {
                    prefix_length = 32;
                }
            }
            if (!dnx_kbp_device_enabled(unit))
            {
                bcm_switch_control_set(unit, bcmSwitchDescCommit, 1);
            }
        }
        /** Stop timer for updating entries */
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
    }

    if (!dnx_kbp_device_enabled(unit))
    {
        bcm_switch_control_set(unit, bcmSwitchDescCommit, 1);
    }

    /** Iterate over all entries and delete them if delete indication is set */
    if (delete)
    {
        /** Start timer for deleting entries. */
        utilex_ll_timer_set("bcm_l3_route_delete", timers_group, timer_idx[2]);
        cli_out("step3: Delete routes\n");
        prefix_length = 32;
        for (idx = 0; idx < nof_entries / nof_entries_print_mod; idx++)
        {
            for (jdx = 0; jdx < nof_entries_print_mod; jdx++)
            {
                bcm_l3_route_t_init(&route_info);
                dip = (1 << (32 - prefix_length)) * ((jdx + idx * nof_entries_print_mod) / 15 + 1);
                route_info.l3a_vrf = vrf;
                route_info.l3a_subnet = dip;
                route_info.l3a_ip_mask = 0xffffffff << (32 - prefix_length);
                rv = bcm_l3_route_delete(unit, &route_info);
                if (rv != BCM_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "Error, bcm_l3_route_delete Failed, route_info.l3a_subnet=0x%x\n",
                                 route_info.l3a_subnet);
                }

                prefix_length--;
                if (prefix_length <= 17)
                {
                    prefix_length = 32;
                }
            }
        }
        /** Stop timer for deleting entries */
        utilex_ll_timer_stop(timers_group, timer_idx[2]);
    }

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    if (!dnx_kbp_device_enabled(unit))
    {
        bcm_switch_control_set(unit, bcmSwitchDescCommit, 1);
    }

exit:
    SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_kbp_fwd_ipv4_random_man = {
    "Create, update and delete L3 route entries",
    "Create, update and delete L3 route entries while recording how much time each step takes",
    "",
    ""
};

sh_sand_option_t dnx_kbp_fwd_ipv4_random_options[] = {
    {CTEST_DNX_KBP_FWD_IPV4_RANDOM_ENTRY_NUM, SAL_FIELD_TYPE_UINT32, "Number of entries", "4096"}
    ,
    {CTEST_DNX_KBP_FWD_IPV4_RANDOM_ENTRY_NUM_PRINT_MOD, SAL_FIELD_TYPE_UINT32, "Number of entries in print mode", "2"}
    ,
    {CTEST_DNX_KBP_FWD_IPV4_RANDOM_CACHED_ENT_NUM, SAL_FIELD_TYPE_UINT32, "Cached number of entries", "0"}
    ,
    {CTEST_DNX_KBP_FWD_IPV4_RANDOM_WARMBOOT, SAL_FIELD_TYPE_BOOL, "Warmboot indication", "NO"}
    ,
    {CTEST_DNX_KBP_FWD_IPV4_RANDOM_UPDATE, SAL_FIELD_TYPE_BOOL, "Update indication", "YES"}
    ,
    {CTEST_DNX_KBP_FWD_IPV4_RANDOM_DELETE, SAL_FIELD_TYPE_BOOL, "Delete indication", "YES"}
    ,
    {NULL}
};

sh_sand_invoke_t dnx_kbp_fwd_ipv4_random_tests[] = {
    {"KBP_test_ipv4_random", "EntryNum=4096 EntryNumPrintMod=2 CachedEntNum=0 WarmBoot=NO Update=YES Delete=YES",
     CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/*generates array of 60k ipv6 routes with full mask*/
STATIC void
generate_ipv6_routes_array_dnx(bcm_l3_route_t* routesArray, int numRoutes)
{
    bcm_ip6_t Ipv6Mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_ip6_t currRoute = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int counterA = 0, counterB = 0, totalCounter = 0;
    int startTime = 0, endTime = 0;

    printf("Started creating routes array\n...\n");
    startTime = sal_time_usecs();

    for(counterA = 0 ; counterA < 235 ; counterA ++)
    {
        for(counterB = 0 ; counterB < 256 ; counterB ++)
        {
            currRoute[(totalCounter % 15) + 1] = counterA;
            currRoute[((totalCounter + 1) % 15) + 1] = counterB;
            bcm_l3_route_t_init(&routesArray[totalCounter]);
            sal_memcpy(routesArray[totalCounter].l3a_ip6_net, currRoute, 16);
            sal_memcpy(routesArray[totalCounter].l3a_ip6_mask, Ipv6Mask, 16);
            routesArray[totalCounter].l3a_flags |= BCM_L3_IP6;
            totalCounter++;
            if(totalCounter == numRoutes)
            {
                break;
            }
        }

        if(totalCounter == numRoutes)
        {
            break;
        }
    }

    endTime = sal_time_usecs();

    printf("Finished creating routes array after %d us\n\n",endTime-startTime);
}

/**
 * The following ctest exercises adding, updating and deleting L3 IPv6 route entries.
 * Each step is being timed.
 */
shr_error_e
dnx_kbp_fwd_ipv6_performance(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_entries = 60000;
    uint32 nof_entries_print_mod = 2000;
    int idx = 0, jdx = 0, rv = 0;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    uint32 timer_idx[3] = { 1, 2, 3 };
    uint32 timers_group;
    bcm_l3_route_t *routesArray = NULL;
    int startTime = 0, endTime = 0;
    int is_active;
    char *name;
    uint32 hits[3] = { 0, 0 , 0 };
    uint32 total_time[3] = { 0, 0 , 0 };
    uint32 avgPerformance = 0;
    uint32 average_time[3] = { 500000, 300000, 500000 };

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(group_name, "KBP IPv6 performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);

    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    routesArray = (bcm_l3_route_t *)sal_alloc(sizeof(bcm_l3_route_t) * 60000, "routes_array");

    generate_ipv6_routes_array_dnx(routesArray, nof_entries);

    cli_out("step1: Add routes\n");
    utilex_ll_timer_clear_all(timers_group);
    /** Set timer for creating the L3 route entries */
    utilex_ll_timer_set("bcm_l3_route_add create", timers_group, timer_idx[0]);
    /** Iterate over the given number of entries to create */
    for (idx = 0; idx < (nof_entries / nof_entries_print_mod); idx++)
    {
        startTime = sal_time_usecs();
        for (jdx = 0; jdx < nof_entries_print_mod; jdx++)
        {
            rv = bcm_l3_route_add(unit, &routesArray[jdx + nof_entries_print_mod * idx]);
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "error occurred while adding route, index %d by API bcm_l3_route_add",
                             (jdx + nof_entries_print_mod * idx));
            }
        }
        endTime = sal_time_usecs();
        printf("    %3dst 2K routes add done in : %d us\n",(idx + 1), (endTime - startTime));
    }
    /** Stop timer for creating entries */
    utilex_ll_timer_stop(timers_group, timer_idx[0]);

    /** Receive the total time that it took to add nof_entries number of IPv6 entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[0], &is_active, &name, &hits[0], &total_time[0]));

    avgPerformance = total_time[0] / (nof_entries / nof_entries_print_mod);
    if(avgPerformance > average_time[0])
    {
        SHR_ERR_EXIT(BCM_E_FAIL, "ERROR : the average time for bcm_l3_route_add is higher than %d us for 2k entries : %d us\n\n",
                     average_time[0], avgPerformance);
    }
    else
    {
        printf("the average time for 2k bcm_l3_route_add : %d us\n\n", avgPerformance);
    }

    /** Set timer to updating the entries */
    utilex_ll_timer_set("bcm_l3_route_add update", timers_group, timer_idx[1]);
    cli_out("step2: Update routes\n");
    for (idx = 0; idx < (nof_entries / nof_entries_print_mod); idx++)
    {
        startTime = sal_time_usecs();
        for (jdx = 0; jdx < nof_entries_print_mod; jdx++)
        {
            routesArray[jdx + nof_entries_print_mod * idx].l3a_flags |= BCM_L3_REPLACE;
            rv = bcm_l3_route_add(unit, &routesArray[jdx + nof_entries_print_mod * idx]);
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "error occurred while updating route, index %d by API bcm_l3_route_add",
                             (jdx + nof_entries_print_mod * idx));
            }
            /* Clear BCM_L3_REPLACE */
            routesArray[jdx + nof_entries_print_mod * idx].l3a_flags &= ~BCM_L3_REPLACE;
        }
        endTime = sal_time_usecs();
        printf("    %3dst 2K routes update done in : %d us\n", (idx + 1), (endTime - startTime));
    }
    /** Stop timer for updating entries */
    utilex_ll_timer_stop(timers_group, timer_idx[1]);

    /** Receive the total time that it took to replace nof_entries number of IPv6 entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[1], &is_active, &name, &hits[1], &total_time[1]));

    avgPerformance = total_time[1] / (nof_entries / nof_entries_print_mod);
    if(avgPerformance > average_time[1])
    {
        SHR_ERR_EXIT(BCM_E_FAIL, "ERROR : the average time for bcm_l3_route_add (Update) is higher than %d us for 2k entries : %d us\n\n",
                     average_time[1], avgPerformance);
    }
    else
    {
        printf("the average time for 2k bcm_l3_route_add (Replace) : %d us\n\n", avgPerformance);
    }

    /** Start timer for deleting entries. */
    utilex_ll_timer_set("bcm_l3_route_delete", timers_group, timer_idx[2]);
    cli_out("step3: Delete routes\n");
    for (idx = 0; idx < (nof_entries / nof_entries_print_mod); idx++)
    {
        startTime = sal_time_usecs();
        for (jdx = 0; jdx < nof_entries_print_mod; jdx++)
        {
            rv = bcm_l3_route_delete(unit, &routesArray[jdx + nof_entries_print_mod * idx]);
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "error occurred while deleting route, index %d by API bcm_l3_route_delete",
                             (jdx + nof_entries_print_mod * idx));
            }
        }
        endTime = sal_time_usecs();
        printf("    %3dst 2K routes delete done in : %d us\n", (idx + 1), (endTime - startTime));
    }
    /** Stop timer for deleting entries */
    utilex_ll_timer_stop(timers_group, timer_idx[2]);

    /** Receive the total time that it took to delete nof_entries number of IPv6 entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[2], &is_active, &name, &hits[2], &total_time[2]));

    avgPerformance = total_time[2] / (nof_entries / nof_entries_print_mod);
    if(avgPerformance > average_time[2])
    {
        SHR_ERR_EXIT(BCM_E_FAIL, "ERROR : the average time for bcm_l3_route_delete is higher than %d us for 2k entries : %d us\n\n",
                     average_time[2], avgPerformance);
    }
    else
    {
        printf("the average time for 2k bcm_l3_route_delete : %d us\n\n", avgPerformance);
    }

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

exit:
    sal_free(routesArray);
    SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    SHR_FUNC_EXIT;
}

extern shr_error_e
diag_dbal_test_logical_table(
    int unit,
    dbal_tables_e table_id,
    uint32 flags);
/**
 * \brief run logical table test in ctest with flags = 0
 */
static shr_error_e
dnx_kbp_fwd_semantic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    uint32 flags = 0;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    shr_error_e curr_res = _SHR_E_NONE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Set the Result PRT structure */
    PRT_TITLE_SET("Logical Table Test (Flags=0x%x) Summary", flags);
    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("Test Name");
    PRT_COLUMN_ADD("Access Method");
    PRT_COLUMN_ADD("Result");

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                  (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_KBP, DBAL_PHYSICAL_TABLE_NONE,
                   DBAL_TABLE_TYPE_NONE, &table_id));

    while (table_id != DBAL_TABLE_EMPTY)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        /** only high maturity level tables */
        if (table->maturity_level == DBAL_MATURITY_HIGH)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", table->table_name);
            PRT_CELL_SET("%s", "LTT");
            PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));

            flags = 0;
            curr_res = diag_dbal_test_logical_table(unit, table_id, flags);
            if (curr_res == _SHR_E_NONE)
            {
                PRT_CELL_SET("%s", "PASS");
            }
            else
            {
                PRT_CELL_SET("%s", "FAIL");
            }

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", table->table_name);
            PRT_CELL_SET("%s", "LTT_FULL");
            PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));

            flags = 0x50;
            curr_res = diag_dbal_test_logical_table(unit, table_id, flags);
            if (curr_res == _SHR_E_NONE)
            {
                PRT_CELL_SET("%s", "PASS");
            }
            else
            {
                PRT_CELL_SET("%s", "FAIL");
            }
        }
        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                          (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_KBP, DBAL_PHYSICAL_TABLE_NONE,
                           DBAL_TABLE_TYPE_NONE, &table_id));
    }
    PRT_COMMITX;

exit:
    SHR_FUNC_EXIT;
}


static sh_sand_man_t dnx_kbp_fwd_semantic_man = {
    "KBP logical table test - a basic set/get/iterator logical test per KBP table",
};


sh_sand_man_t dnx_kbp_fwd_ipv6_performance_man = {
    "Create, update and delete L3 IPv6 route entries",
    "Create, update and delete L3 IPv6 route entries while recording how much time each step takes",
    "",
    ""
};

sh_sand_option_t dnx_kbp_fwd_ipv6_performance_options[] = {
    {NULL}
};

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_kbp_fwd_cmds[] = {
    {"IPV4_Random", dnx_kbp_fwd_ipv4_random, NULL, dnx_kbp_fwd_ipv4_random_options, &dnx_kbp_fwd_ipv4_random_man, NULL, dnx_kbp_fwd_ipv4_random_tests},
    {"IPV6_PERFormance", dnx_kbp_fwd_ipv6_performance, NULL, dnx_kbp_fwd_ipv6_performance_options, &dnx_kbp_fwd_ipv6_performance_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {"SEMantic", dnx_kbp_fwd_semantic, NULL, NULL, &dnx_kbp_fwd_semantic_man},
    {NULL}
};

sh_sand_option_t dnx_kbp_fwd_options[] = {
    {NULL}
};

sh_sand_invoke_t dnx_kbp_fwd_tests[] = {
    {NULL}
};

sh_sand_man_t dnx_kbp_fwd_man = {
    "KBP manager FWD related tests",
    "No forwarding tests are currently supported",
    "",
    ""
};
/* *INDENT-ON* */

/**********************************************************************************************************************/
/** ACL */
/**********************************************************************************************************************/
/* *INDENT-OFF* */
sh_sand_option_t dnx_kbp_acl_options[] = {
    {"Test", SAL_FIELD_TYPE_STR, "KBP manager ACL related test", ""},
    {NULL}
};

sh_sand_invoke_t dnx_kbp_acl_tests[] = {
    {"KBP_MNGR_ACL_" CTEST_DNX_KBP_ACL_MASTER_KEY,   "Test=" CTEST_DNX_KBP_ACL_MASTER_KEY,   CTEST_PRECOMMIT},
    {"KBP_MNGR_ACL_" CTEST_DNX_KBP_ACL_LOOKUP,       "Test=" CTEST_DNX_KBP_ACL_LOOKUP,       CTEST_PRECOMMIT},
    {"KBP_MNGR_ACL_" CTEST_DNX_KBP_ACL_OPCODE,       "Test=" CTEST_DNX_KBP_ACL_OPCODE,       CTEST_PRECOMMIT},
    {"KBP_MNGR_ACL_" CTEST_DNX_KBP_ACL_ENTRY_SEM,    "Test=" CTEST_DNX_KBP_ACL_ENTRY_SEM,    CTEST_PRECOMMIT},
    {"KBP_MNGR_ACL_" CTEST_DNX_KBP_ACL_OPCODE_CREATE,    "Test=" CTEST_DNX_KBP_ACL_OPCODE_CREATE,    CTEST_PRECOMMIT},
    {NULL}
};

sh_sand_man_t dnx_kbp_acl_man = {
    "KBP manager ACL related tests",
    "Setting \"Test\" option tests a selected group of KBP manager APIs. Setting no option will run all tests.\n"
    "List of supported ACL tests:\n"
    CTEST_DNX_KBP_ACL_MASTER_KEY " - Test that validates the KBP manager dynamic APIs related to master key\n"
    CTEST_DNX_KBP_ACL_LOOKUP " - Test that validates the KBP manager dynamic APIs related to opcode lookups\n"
    CTEST_DNX_KBP_ACL_ENTRY_SEM " - Test that creates dynamic DBAL table for KBP ACL entries and KBP DB for this DBAL table, configures a lookup for this DB and performs entry validations for the table (add, get, remove and validating all values retrieved from the entry).\n"
    CTEST_DNX_KBP_ACL_OPCODE_CREATE" - ctreate dynamic opcode based on fwd opcode.\n"
    CTEST_DNX_KBP_ACL_OPCODE " - Test that configures lookup to a non-forwarding opcode\n",
    "[Test=kbp_mngr_acl_test]",
    "Test=" CTEST_DNX_KBP_ACL_MASTER_KEY "\n"
    "Test=" CTEST_DNX_KBP_ACL_LOOKUP "\n"
    "Test=" CTEST_DNX_KBP_ACL_OPCODE "\n"
    "Test=" CTEST_DNX_KBP_ACL_OPCODE_CREATE "\n"
    "Test=" CTEST_DNX_KBP_ACL_ENTRY_SEM
};
/* *INDENT-ON* */

/*
 * See dnx_kbp_acl_man for more information on the test case
 */
shr_error_e
dnx_kbp_acl_master_key_cmd(
    int unit)
{
    int index;
    int index_check;
    uint8 opcode_id = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_NOP;
    uint32 nof_bytes = 0;
    uint32 nof_key_segments = 1;
    uint32 nof_key_segments_check = 0;
    uint32 nof_key_segments_stash = 0;
    kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { {0} };
    kbp_mngr_key_segment_t master_key_segments_check[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { {0} };
    kbp_mngr_key_segment_t master_key_segments_stash[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { {0} };
    CTEST_DNX_KBP_LOGGER_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_KBP_LOGGER_INIT();

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Starting KBP manager master key testing\n\n")));

    for (index = 0; index < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; index++)
    {
        kbp_mngr_key_segment_t_init(unit, &master_key_segments[index]);
    }

    /** Sync and device lock are not required for the test */

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Negative testing\n")));

    /** Get NULL number of key segments */
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U
              (unit, "Using NULL pointer for number of key segments with \"kbp_mngr_opcode_master_key_get()\"\n")));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_get(unit, opcode_id, NULL, master_key_segments));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Get NULL master key segments */
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U
              (unit, "Using NULL pointer for master key segments with \"kbp_mngr_opcode_master_key_get()\"\n")));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_key_segments, NULL));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add NULL master key segments */
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U
              (unit,
               "Using NULL pointer for master key segments with \"kbp_mngr_opcode_master_key_segments_add()\"\n")));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add(unit, opcode_id, nof_key_segments, NULL));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add too much segments */
    opcode_id = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_NOP;
    nof_key_segments = DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP + 1;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using too much segments %d with \"kbp_mngr_opcode_master_key_segments_add()\"\n"),
              nof_key_segments));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add
                                    (unit, opcode_id, nof_key_segments, master_key_segments));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add invalid segment size 0 */
    master_key_segments[0].nof_bytes = 0;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid segment size %d with \"kbp_mngr_opcode_master_key_segments_add()\"\n"),
              master_key_segments[0].nof_bytes));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add(unit, opcode_id, 1, master_key_segments));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add invalid segment size above the maximum supported */
    master_key_segments[0].nof_bytes = DNX_KBP_MAX_SEGMENT_LENGTH_IN_BYTES + 1;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid segment size %d with \"kbp_mngr_opcode_master_key_segments_add()\"\n"),
              master_key_segments[0].nof_bytes));
    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add(unit, opcode_id, 1, master_key_segments));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add segments with the same name */
    sal_strncpy(master_key_segments[0].name, "TEST_SEGMENT_NAME", DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES);
    master_key_segments[0].nof_bytes = 1;
    sal_strncpy(master_key_segments[1].name, "TEST_SEGMENT_NAME", DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES);
    master_key_segments[1].nof_bytes = 2;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Using two segments with the same name \"%s\" with \"kbp_mngr_opcode_master_key_segments_add()\"\n"),
              master_key_segments[0].name));
    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add(unit, opcode_id, 2, master_key_segments));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nPositive testing\n")));

    /** Set all available (free) segments and test they are added correctly */
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Setting all available (free) segments and validate them\n")));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get
                    (unit, opcode_id, &nof_key_segments_stash, master_key_segments_stash));

    nof_key_segments = DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP - nof_key_segments_stash;
    for (index = 0, nof_bytes = 1; index < nof_key_segments; index++, nof_bytes++)
    {
        sal_snprintf(master_key_segments[index].name, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, "TEST_SEGMENT_%d",
                     nof_bytes);
        master_key_segments[index].nof_bytes = nof_bytes;
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add(unit, opcode_id, nof_key_segments, master_key_segments));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get
                    (unit, opcode_id, &nof_key_segments_check, master_key_segments_check));

    if (nof_key_segments_check != DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to get %d master key segments, but got %d\n",
                     DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP, nof_key_segments_check);
    }

    /** Check only the newly added segments; offset the checked index */
    for (index = 0, index_check = nof_key_segments_stash; index < nof_key_segments; index++, index_check++)
    {
        if (sal_strncmp
            (master_key_segments[index].name, master_key_segments_check[index_check].name,
             DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Master key segment %d \"%s\" has different name than expected \"%s\"\n",
                         index_check, master_key_segments_check[index_check].name, master_key_segments[index].name);
        }
        if (master_key_segments[index].nof_bytes != master_key_segments_check[index_check].nof_bytes)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Master key segment %d \"%s\" has different size %d than the expected %d\n",
                         index, master_key_segments_check[index].name, master_key_segments_check[index].nof_bytes,
                         master_key_segments[index].nof_bytes);
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Testing master key passed; Setting opcode master key to original state\n\n")));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_set(unit, opcode_id, nof_key_segments_stash, master_key_segments_stash));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See dnx_kbp_acl_man for more information on the test case
 */
shr_error_e
dnx_kbp_acl_lookup_cmd(
    int unit)
{
    int index;
    int opcode_id = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_NOP;
    kbp_opcode_lookup_info_t lookup_info, lookup_info_check;
    CTEST_DNX_KBP_LOGGER_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_KBP_LOGGER_INIT();

    CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK;

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Starting KBP manager lookup testing\n\n")));

    SHR_IF_ERR_EXIT(kbp_opcode_lookup_info_t_init(unit, &lookup_info));

    /** Clear configuration by FIELD KBP CINT. cint_field_kbp.c */
    kbp_mngr_opcode_result_clear(unit, 0, 4);

    /** Sync and device lock are not required for the test */

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Negative testing\n")));

    /** Get NULL lookup info */
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using NULL pointer for lookup info with \"kbp_mngr_opcode_lookup_get()\"\n")));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_get(unit, opcode_id, NULL));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Get out of range result index */
    lookup_info.result_index = DNX_KBP_MAX_NOF_RESULTS;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using out of range result index %d with \"kbp_mngr_opcode_lookup_get()\"\n"),
              lookup_info.result_index));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_get(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Get invalid opcode */
    opcode_id = 50;
    lookup_info.result_index = 5;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid opcode %d with \"kbp_mngr_opcode_lookup_get()\"\n"), opcode_id));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_get(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Get lookup index that is not in use */
    opcode_id = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_UNICAST_PRIVATE_W_UC_RPF;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using result index that is not in use %d with \"kbp_mngr_opcode_lookup_get()\"\n"),
              lookup_info.result_index));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_get(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_NOT_FOUND,
                     SHR_GET_CURRENT_ERR());
    }

    /** Add NULL lookup info */
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using NULL pointer for lookup info with \"kbp_mngr_opcode_lookup_add()\"\n")));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, NULL));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add out of range lookup index */
    lookup_info.result_index = DNX_KBP_MAX_NOF_RESULTS;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using out of range result index %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.result_index));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add not byte aligned offset */
    lookup_info.result_index = 4;
    lookup_info.result_offset = 93;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using not byte aligned offset %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.result_offset));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add lookup with result in use */
    lookup_info.result_index = 0;
    lookup_info.result_offset = 96;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using result index in use %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.result_index));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add lookup with unavailable result */
    lookup_info.result_index = 7;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using result index that is not available %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.result_index));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add invalid DBAL table */
    lookup_info.result_index = 3;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid DBAL table %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.dbal_table_id));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add DBAL table that is not KBP */
    lookup_info.dbal_table_id = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid DBAL table %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.dbal_table_id));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_NOT_FOUND,
                     SHR_GET_CURRENT_ERR());
    }

    /** Add overlapping offset */
    lookup_info.dbal_table_id = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
    lookup_info.result_offset = 80;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using overlapping offset %d with kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.result_offset));
    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add too much lookup segments */
    lookup_info.result_offset = 88;
    lookup_info.nof_segments = DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP + 1;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using too much lookup segments %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.nof_segments));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add invalid key segment index */
    lookup_info.nof_segments = 1;
    lookup_info.key_segment_index[0] = DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP + 1;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid key segment index %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.key_segment_index[0]));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add repeating key segment index */
    lookup_info.nof_segments = 2;
    lookup_info.key_segment_index[0] = 0;
    lookup_info.key_segment_index[1] = 0;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid key segment index %d with \"kbp_mngr_opcode_lookup_add()\"\n"),
              lookup_info.key_segment_index[0]));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nPositive testing\n")));

    /** Set all available (free) lookup indices and test they are added correctly */
    opcode_id = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_UNICAST_PRIVATE_W_UC_RPF;
    lookup_info.result_index = 2;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Setting opcode %d result %d and validate it\n"), opcode_id, lookup_info.result_index));

    lookup_info.dbal_table_id = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
    lookup_info.result_offset = DNX_KBP_RESULT_SIZE_FWD + DNX_KBP_RESULT_SIZE_RPF;
    lookup_info.nof_segments = 3;
    for (index = 0; index < lookup_info.nof_segments; index++)
    {
        lookup_info.key_segment_index[index] = index;
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));
    lookup_info_check.result_index = lookup_info.result_index;
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_get(unit, opcode_id, &lookup_info_check));

    if (lookup_info.dbal_table_id != lookup_info_check.dbal_table_id)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to get DBAL table %d, but got %d\n",
                     lookup_info.dbal_table_id, lookup_info_check.dbal_table_id);
    }

    if (lookup_info.result_offset != lookup_info_check.result_offset)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to get result offset %d, but got %d\n",
                     lookup_info.result_offset, lookup_info_check.result_offset);
    }

    if (lookup_info.nof_segments != lookup_info_check.nof_segments)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to get %d number of segments, but got %d\n",
                     lookup_info.nof_segments, lookup_info_check.nof_segments);
    }

    for (index = 0; index < lookup_info.nof_segments; index++)
    {
        if (lookup_info.key_segment_index[index] != lookup_info_check.key_segment_index[index])
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to get key_segment_index[%d]=%d number of segments, but got %d\n",
                         index, lookup_info.nof_segments, lookup_info_check.nof_segments);
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Testing lookup passed; Setting opcode %d result %d to original state\n\n"), opcode_id,
              lookup_info.result_index));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_result_clear(unit, opcode_id, lookup_info.result_index));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See dnx_kbp_acl_man for more information on the test case
 */
static shr_error_e
dnx_kbp_acl_opcode_cmd(
    int unit)
{
    int index;
    int opcode_id = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_NOP;
    kbp_opcode_lookup_info_t lookup_info;
    uint32 nof_key_segments = 0;
    kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { {0} };
    SHR_FUNC_INIT_VARS(unit);

    CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK;

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Starting KBP manager opcode testing\n\n")));

    SHR_IF_ERR_EXIT(kbp_opcode_lookup_info_t_init(unit, &lookup_info));
    for (index = 0; index < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; index++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_t_init(unit, &master_key_segments[index]));
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_key_segments, master_key_segments));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Setting opcode %d and lookup to result %d\n"), opcode_id, lookup_info.result_index));

    lookup_info.result_index = 0;
    lookup_info.dbal_table_id = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
    lookup_info.result_offset = 0;
    lookup_info.nof_segments = 5;
    for (index = 0; index < lookup_info.nof_segments; index++)
    {
        sal_snprintf(master_key_segments[index].name, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, "TEST_SEGMENT_%d", index);
        master_key_segments[index].nof_bytes = index + 1;
        lookup_info.key_segment_index[index] = index;
        nof_key_segments++;
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add(unit, opcode_id, nof_key_segments, master_key_segments));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Testing opcode passed; Clearing opcode %d and lookup to result %d\n"), opcode_id,
              lookup_info.result_index));

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_result_clear(unit, opcode_id, lookup_info.result_index));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_clear(unit, opcode_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_opcode_create(
    int unit)
{
    int opcode_id = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_UNICAST_PRIVATE_W_UC_RPF;
    uint8 acl_ctx;
    uint8 new_opcode_id;
    char *opcode_name = "test opcode";

    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_create(unit, opcode_id, opcode_name, &acl_ctx, &new_opcode_id));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See dnx_kbp_acl_man for more information on the test case
 */
static shr_error_e
dnx_kbp_acl_entry_sem_cmd(
    int unit)
{
    int index;
    int opcode_id = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_UNICAST_PRIVATE_W_UC_RPF;
    int result_index = 2;
    uint32 entry_handle_id;
    uint32 access_id;
    kbp_opcode_lookup_info_t lookup_info;
    uint32 nof_key_segments = 0;
    kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { {0} };
    dbal_tables_e dbal_table_id;
    uint32 nof_keys = 3;
    dbal_table_field_input_info_t key_info[3] = {
        {DBAL_FIELD_QUALIFIER, 32, FALSE},
        {DBAL_FIELD_IPV4_DIP, 32, FALSE},
        {DBAL_FIELD_VRF, 16, FALSE}
    };
    uint8 is_shared_qual[3] = { FALSE, TRUE, TRUE };
    uint8 key_index[3] = { 3, 2, 0 };
    dbal_table_field_input_info_t result_info = { DBAL_FIELD_ACTION, 32, FALSE };
    char qual_name[3][DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { "QUALIFIER", "IPV4_DIP", "VRF" };
    uint32 key_val[3] = { 0x12345678, 0x9abcdef0, 0xab };
    uint32 add_val = 0x87654321;
    uint32 get_val;
    uint32 add_prio = 92;
    uint32 get_prio;
    CTEST_DNX_KBP_LOGGER_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    CTEST_DNX_KBP_LOGGER_INIT();

    CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK;

    SHR_IF_ERR_EXIT(kbp_opcode_lookup_info_t_init(unit, &lookup_info));
    for (index = 0; index < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; index++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_t_init(unit, &master_key_segments[index]));
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Create dynamic DBAL table\n")));
    SHR_IF_ERR_EXIT(dbal_tables_table_create(unit, DBAL_ACCESS_METHOD_KBP, DBAL_TABLE_TYPE_TCAM_DIRECT,
                                             DBAL_CORE_MODE_SBC, nof_keys, key_info, 1, &result_info, "KBP_TEST_TABLE",
                                             &dbal_table_id));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Create KBP DB\n")));
    SHR_IF_ERR_EXIT(kbp_mngr_db_create(unit, dbal_table_id));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Add new lookup at result %d to opcode %d\n"), result_index, opcode_id));

    for (index = 0; index < nof_keys; index++)
    {
        if (!is_shared_qual[index])
        {
            /** Update the master key with the new segment if it's not shared */
            master_key_segments[nof_key_segments].nof_bytes = BITS2BYTES(key_info[index].field_nof_bits);
            sal_strncpy(master_key_segments[nof_key_segments].name, qual_name[index],
                        DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES);
            nof_key_segments++;
        }

        /** Update the lookup info */
        lookup_info.nof_segments++;
        lookup_info.key_segment_index[index] = key_index[index];

    }
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_total_result_size_get(unit, opcode_id, &lookup_info.result_offset));
    lookup_info.dbal_table_id = dbal_table_id;
    lookup_info.result_index = result_index;

    /** Update the opcode master key and add the lookup */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add(unit, opcode_id, nof_key_segments, master_key_segments));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Perform KBP sync\n")));
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Add an entry\n")));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, add_prio));
    for (index = 0; index < nof_keys; index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key_info[index].field_id, key_val[index]);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, result_info.field_id, INST_SINGLE, add_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, &access_id));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Get the entry and compare it to the added one\n")));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, &get_prio));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (add_prio != get_prio)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "add_prio %d is not the same as get_prio %d\n", add_prio, get_prio);
    }
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Priority %d validated\n"), add_prio));

    for (index = 0; index < nof_keys; index++)
    {
        uint32 get_key = 0;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, key_info[index].field_id, &get_key));
        if (key_val[index] != get_key)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "key_val[%d] 0x%08X is not the same as get_val 0x%08X\n", index, add_val,
                         get_val);
        }
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Key %d 0x%08X validated\n"), index, key_val[index]));
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, result_info.field_id, INST_SINGLE, &get_val));
    if (add_val != get_val)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "add_val 0x%08X is not the same as get_val 0x%08X\n", add_val, get_val);
    }
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Value 0x%08X validated\n"), add_val));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Remove the entry\n")));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Get the removed entry, expect to fail\n")));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_NOT_FOUND,
                     SHR_GET_CURRENT_ERR());
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, dbal_table_id));
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "KBP semantic entry test passed\n\n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**********************************************************************************************************************/
shr_error_e
dnx_kbp_acl_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("Test", test);

    if (ISEMPTY(test))
    {
        /** Run all tests */
        SHR_IF_ERR_EXIT(dnx_kbp_acl_master_key_cmd(unit));
        SHR_IF_ERR_EXIT(dnx_kbp_acl_lookup_cmd(unit));
        SHR_IF_ERR_EXIT(dnx_kbp_acl_opcode_cmd(unit));
        SHR_IF_ERR_EXIT(dnx_kbp_acl_entry_sem_cmd(unit));
        SHR_IF_ERR_EXIT(dnx_kbp_acl_opcode_create(unit));
    }
    else
    {
        if (!strcmp(test, CTEST_DNX_KBP_ACL_MASTER_KEY))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_acl_master_key_cmd(unit));
        }
        else if (!strcmp(test, CTEST_DNX_KBP_ACL_LOOKUP))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_acl_lookup_cmd(unit));
        }
        else if (!strcmp(test, CTEST_DNX_KBP_ACL_OPCODE))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_acl_opcode_cmd(unit));
        }
        else if (!strcmp(test, CTEST_DNX_KBP_ACL_ENTRY_SEM))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_acl_entry_sem_cmd(unit));
        }
        else if ((!strcmp(test, CTEST_DNX_KBP_ACL_OPCODE_CREATE)))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_acl_opcode_create(unit));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown ACL test command \"%s\".\n", test);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**********************************************************************************************************************/
/* *INDENT-OFF* */
sh_sand_cmd_t dnx_kbp_test_cmds[] = {
	{"ACL", dnx_kbp_acl_cmd, NULL, dnx_kbp_acl_options, &dnx_kbp_acl_man, NULL, dnx_kbp_acl_tests},
	{"FWD", NULL, dnx_kbp_fwd_cmds, dnx_kbp_fwd_options, &dnx_kbp_fwd_man, NULL, dnx_kbp_fwd_tests},/* must be after ACLs since ACLs performing lock*/
	{"STIF", NULL, sh_dnx_kbp_stif_test_cmds},
    {NULL}
};
/* *INDENT-ON* */
