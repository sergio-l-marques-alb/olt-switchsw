/** \file ctest_dnx_l3_rif.c
 * $Id$
 *
 * Tests for L3 ing intf create
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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
#include <bcm/tunnel.h>
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
extern void increment_macaddr(
    sal_mac_addr_t macaddr,
    int amount);

extern void increment_ip6addr(
    ip6_addr_t ipaddr,
    uint32 byte_pos,
    int amount);
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

static sh_sand_man_t sh_dnx_l3_intf_mymac_alloc_man = {
    "Test for MYMAC prefix profile allocation",
    "ctest l3 rif mymac_alloc",
};

static sh_sand_option_t sh_dnx_l3_ing_intf_performance_options[] = {
    {"count", SAL_FIELD_TYPE_UINT32, "Number of VSIs to add", "4000"},
    {NULL}
};
static sh_sand_option_t sh_dnx_l3_intf_mymac_alloc_options[] = {
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
    uint32 timers_group = UTILEX_LL_TIMER_NO_GROUP;
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
    sal_printf("Running the L3 intf create performance test with %d number of entries\n", nof_entries);

    sal_memset(group_name, 0, sizeof(char) * UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME);
    sal_strncpy_s(group_name, "L3 intf create perf", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    utilex_ll_timer_clear_all(timers_group);

    sal_printf("Adding %d number of entries\n", nof_entries);
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

    sal_printf("Added %d number of entries\n", entry_id);
    if (entry_id != nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not add requested number of entries. Added %u out of %u\n",
                     entry_id, nof_entries);
    }
    sal_printf("Clearing the intf table\n");
    SHR_IF_ERR_EXIT(bcm_l3_intf_delete_all(unit));

    /** Receive the total time that it took to add nof_entries number of Intf entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[0], &is_active, &name, &hits, &total_time[0]));
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timers_group, timer_idx[1], &is_active, &name, &hits, &total_time[1]));
    sal_printf("_______________________________________________________\n\n");
    sal_printf("AVERAGE time for bcm_l3_intf_create - %u us, expected approximately %u\n",
               (total_time[0] / nof_entries), average_time[0]);
    sal_printf("AVERAGE time for bcm_l3_intf_create - %u us, expected approximately %u\n",
               (total_time[1] / nof_entries), average_time[1]);
    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);
    /** Verify that the average execution time for adding a single entry is within limits. */
#if defined(ADAPTER_SERVER_MODE)
    /** Skip comparing with average expected time for adapter execution. */
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

/**
 * \brief
 * Mymac prefix/SA/SIP profile allocation testing
 * Verify that the template manager source_address_table_allocation is working as expected.
 * The template is used to allocate mymac prefix, SA and SIP profiles.
 */
static shr_error_e
sh_dnx_l3_intf_mymac_alloc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_intf_t l3if, l3if_find;
    bcm_l3_egress_t arp_entry;
    bcm_tunnel_initiator_t tunnel_info;
    int vsi = 100;
    int arp_id, lif_id;
    int lif_id_start = dnx_data_l3.rif.nof_rifs_get(unit) + 1;

    uint32 intf[33] = { 0 };
    uint32 max_ipv6_sip_profiles = dnx_data_l3.source_address.source_address_table_size_get(unit) / 4;
    uint32 sip_profile_half_table_plus_one = max_ipv6_sip_profiles / 2 + 1;
    uint32 nof_mac_resuorces_taken_by_ipv6_ip = 2;
    /*
     * The max number of my_mac_prefix and SA profiles that can be allocated without the default one. 
     */
    uint32 mymac_prefix_profiles_without_default = dnx_data_l3.source_address_ethernet.nof_mymac_prefixes_get(unit) - 1;
    uint32 entry_id = 0;
    uint32 nof_entries = dnx_data_l3.source_address_ethernet.nof_mymac_prefixes_get(unit);
    bcm_ip6_t ipv6_sip =
        { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 };
    bcm_mac_t my_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    bcm_mac_t empty_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_intf_t_init(&l3if);
    bcm_l3_egress_t_init(&arp_entry);
    bcm_tunnel_initiator_t_init(&tunnel_info);

    /*
     * Allocate the max number of mymac prefix profiles.
     * Verify bcm_l3_intf_create() will return _SHR_E_FULL error when there is not any free profile
     */
    sal_printf("Allocating the max number of mymac prefix profiles using bcm_l3_intf_create()\n\n\n");
    sal_memcpy(l3if.l3a_mac_addr, my_mac, sizeof(bcm_mac_t));
    for (entry_id = 0; entry_id <= nof_entries; entry_id++)
    {
        l3if.l3a_vid = entry_id + vsi;
        if (entry_id < mymac_prefix_profiles_without_default)
        {
            SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));
        }
        else
        {
            /*
             * No free profiles
             */
            SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_intf_create(unit, &l3if), _SHR_E_FULL);
        }
        increment_macaddr(l3if.l3a_mac_addr, 1000);
    }
    /*
     * All MyMac Prefix profiles are allocated.
     * Adding an intf with an empty MAC(default data) is possible as the default profile is used.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_vid = 200;
    sal_memcpy(l3if.l3a_mac_addr, empty_mac, sizeof(bcm_mac_t));
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));
    sal_printf("Successfully added an intf with an empty MAC, the default profile is used.\n");

    /*
     * Find an intf with MAC of all 0s
     */
    bcm_l3_intf_t_init(&l3if_find);
    l3if_find.l3a_vid = 200;
    sal_memcpy(l3if_find.l3a_mac_addr, empty_mac, sizeof(bcm_mac_t));
    SHR_IF_ERR_EXIT(bcm_l3_intf_find(unit, &l3if_find));

    /*
     * At this point all mymac_prefix/SA profiles are allocated. Verify that bcm_l3_egress_create will return an error
     * as there is not any free profile. 
     */
    sal_printf("All profiles are allocated, bcm_l3_egress_create() will return an error!\n");
    sal_memcpy(arp_entry.src_mac_addr, my_mac, sizeof(bcm_mac_t));
    arp_entry.encap_id = lif_id_start;
    arp_entry.vlan = 20;
    arp_entry.flags2 = BCM_L3_FLAGS2_SRC_MAC;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &arp_entry, &arp_id), _SHR_E_FULL);

    sal_printf("Clearing the intf table\n");
    SHR_IF_ERR_EXIT(bcm_l3_intf_delete_all(unit));

    /*
     * Allocate the max number of SA profiles using bcm_l3_egress_create()
     */
    sal_printf("Allocating the max number of SA profiles using bcm_l3_egress_create()\n");
    for (entry_id = 0; entry_id <= nof_entries; entry_id++)
    {
        lif_id = lif_id_start + entry_id;
        arp_entry.encap_id = lif_id;
        increment_macaddr(arp_entry.src_mac_addr, 1000);
        if (entry_id < mymac_prefix_profiles_without_default)
        {

            SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &arp_entry, &arp_id));
        }
        else
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &arp_entry, &arp_id), _SHR_E_FULL);
        }
    }
    /*
     * Delete the ARP entries
     */
    for (entry_id = 0; entry_id < mymac_prefix_profiles_without_default; entry_id++)
    {
        BCM_L3_ITF_SET(lif_id, BCM_L3_ITF_TYPE_LIF, lif_id_start + entry_id);
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, lif_id));
    }

    /*
     * At this point all profiles are free.
     * Allocate the TOP half of the table with IPv6 SIP profiles and one more SIP profile at the BOTTOM half.
     */
    bcm_l3_intf_t_init(&l3if);
    sal_memcpy(tunnel_info.sip6, ipv6_sip, sizeof(bcm_ip6_t));
    tunnel_info.type = bcmTunnelTypeUdp6;
    tunnel_info.udp_dst_port = 2152;
    tunnel_info.udp_src_port = 2152;
    for (entry_id = 0; entry_id < sip_profile_half_table_plus_one; entry_id++)
    {
        SHR_IF_ERR_EXIT(bcm_tunnel_initiator_create(unit, &l3if, &tunnel_info));
        increment_ip6addr(tunnel_info.sip6, 8, 10000);
        intf[entry_id] = l3if.l3a_intf_id;
    }

    /*
     * A SIP profile was allocated at the bottom half of the table(it took 2 SA profiles). It means that the max
     * SA/mymac prefix profiles that can be allocated are less by two.
     */
    for (entry_id = 0; entry_id <= nof_entries; entry_id++)
    {
        lif_id = lif_id_start + entry_id;
        arp_entry.encap_id = lif_id;
        increment_macaddr(arp_entry.src_mac_addr, 1000);
        if (entry_id < mymac_prefix_profiles_without_default - nof_mac_resuorces_taken_by_ipv6_ip)
        {

            SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &arp_entry, &arp_id));
        }
        else
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &arp_entry, &arp_id), _SHR_E_FULL);
        }
    }
    /*
     * Clean up
     */
    /*
     * Delete created ARP entries
     */
    for (entry_id = 0; entry_id < mymac_prefix_profiles_without_default - nof_mac_resuorces_taken_by_ipv6_ip;
         entry_id++)
    {
        BCM_L3_ITF_SET(lif_id, BCM_L3_ITF_TYPE_LIF, lif_id_start + entry_id);
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, lif_id));
    }
    /*
     * Delete IPv6 entries
     */
    bcm_l3_intf_t_init(&l3if);
    for (entry_id = 0; entry_id < sip_profile_half_table_plus_one; entry_id++)
    {
        l3if.l3a_intf_id = intf[entry_id];
        SHR_IF_ERR_EXIT(bcm_tunnel_initiator_clear(unit, &l3if));
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\nMYMAC prefix allocation test PASSED!\n")));
exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX L3 intf Tests
 * List of intf ctests modules.
 */
sh_sand_cmd_t dnx_l3_ing_intf_test_cmds[] = {
    {"performance", sh_dnx_l3_ing_intf_performance_cmd, NULL, sh_dnx_l3_ing_intf_performance_options, &sh_dnx_l3_ing_intf_performance_man, NULL, NULL, CTEST_POSTCOMMIT * CTEST_UM},
    {"mymac_prefix_allocation", sh_dnx_l3_intf_mymac_alloc_cmd, NULL, sh_dnx_l3_intf_mymac_alloc_options, &sh_dnx_l3_intf_mymac_alloc_man, NULL, NULL, CTEST_POSTCOMMIT * CTEST_UM},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE
