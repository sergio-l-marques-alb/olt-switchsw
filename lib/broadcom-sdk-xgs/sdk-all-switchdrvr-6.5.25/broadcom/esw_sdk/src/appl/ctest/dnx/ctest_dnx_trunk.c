/** \file ctest_dnx_trunk.c
 *
 * ctests for trunk
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

#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_trunk_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_stat_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_l2_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_vlan_dispatch.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include "appl/ctest/dnxc/ctest_dnxc_system.h"
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_l2.h>

#define PARAMS_STR_OVERKILL_SIZE 100
#define NAME_STR_OVERKILL_SIZE 100

static shr_error_e
dnx_trunk_config_test_per_pool(
    int unit,
    uint32 pool)
{
    int rv;
    uint32 max_nof_groups_in_pool = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_groups_in_pool;
    uint32 max_nof_members_in_group = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    bcm_trunk_t trunk_id;
    uint32 system_port_id = 20;
    bcm_trunk_member_t *members_p = NULL;
    bcm_gport_t sysport_gport;
    bcm_trunk_info_t trunk_info = { 0 };
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_gport_t trunk_gport;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(members_p, sizeof(*members_p) * (max_nof_members_in_group + 1),
                       "members_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** try to create trunk with invalid ID */
    BCM_TRUNK_ID_SET(trunk_id, pool, max_nof_groups_in_pool);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TRUNK, bslSeverityFatal);
    rv = bcm_dnx_trunk_create(unit, 0, &trunk_id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TRUNK, bslSeverityInfo);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, expected %d but received %d \n", _SHR_E_PARAM, rv);
    }

    /** try to create trunk with maximal valid ID */
    BCM_TRUNK_ID_SET(trunk_id, pool, max_nof_groups_in_pool - 1);
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_create(unit, 0, &trunk_id));

    /*****************************************/
    /** set trunk with max amount if members */
    /*****************************************/
    /** create members */
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, system_port_id);
    for (int member_index = 0; member_index < (max_nof_members_in_group + 1); ++member_index)
    {
        members_p[member_index].gport = sysport_gport;
    }
    /** set trunk info */
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    /** set headers type to Eth */
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    key.index = 0;
    key.type = bcmSwitchPortHeaderType;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, trunk_gport, key, value));
    /** set trunk */
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_set(unit, trunk_id, &trunk_info, max_nof_members_in_group, members_p));

    /** try to set trunk with too much members - expect failure */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TRUNK, bslSeverityFatal);
    rv = bcm_dnx_trunk_set(unit, trunk_id, &trunk_info, (max_nof_members_in_group + 1), members_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TRUNK, bslSeverityInfo);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, expected %d but received %d \n", _SHR_E_PARAM, rv);
    }

    /** try to add members to full trunk - expect failure */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TRUNK, bslSeverityFatal);
    rv = bcm_dnx_trunk_member_add(unit, trunk_id, &members_p[0]);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TRUNK, bslSeverityInfo);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, expected %d but received %d \n", _SHR_E_PARAM, rv);
    }

    /** clean-up */
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_destroy(unit, trunk_id));

exit:
    SHR_FREE(members_p);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_trunk_config_test(
    int unit)
{
    uint32 nof_trunk_pools = dnx_data_trunk.parameters.nof_pools_get(unit);
    bsl_severity_t bsl_severity;
    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TRUNK, bsl_severity);
    /** get from DNX data number of trunk groups in pool and number of members in group */
    for (int pool = 0; pool < nof_trunk_pools; ++pool)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_config_test_per_pool(unit, pool));
    }

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TRUNK, bsl_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_trunk_spa_map_test(
    int unit,
    uint32 pool,
    uint32 group,
    uint32 ports_array[],
    uint32 ports_array_size)
{
    bcm_trunk_t trunk_id;
    bcm_trunk_member_t *members_p = NULL;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    bcm_trunk_info_t trunk_info = { 0 };
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_gport_t trunk_gport;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(members_p, sizeof(*members_p) * (ports_array_size), "members_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** create trunk */
    BCM_TRUNK_ID_SET(trunk_id, pool, group);
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_create(unit, 0, &trunk_id));

    /** set trunk with members */
    /** create members */
    for (int member_index = 0; member_index < ports_array_size; ++member_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, ports_array[member_index],
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY |
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                                                        &phy_gport_info));
        BCM_GPORT_SYSTEM_PORT_ID_SET(members_p[member_index].gport, phy_gport_info.sys_port);
    }

    /** set trunk info */
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;

    /** set headers type to Eth */
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    key.index = 0;
    key.type = bcmSwitchPortHeaderType;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, trunk_gport, key, value));

    /** set trunk */
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_set(unit, trunk_id, &trunk_info, ports_array_size, members_p));

    /** check system port to SPA mapping and SPA to system port mapping */
    for (int member_index = 0; member_index < ports_array_size; ++member_index)
    {
        uint32 system_port_aggregate;
        bcm_gport_t gport;
        SHR_IF_ERR_EXIT(dnx_trunk_system_port_to_spa_map_get
                        (unit, members_p[member_index].gport, trunk_id, &system_port_aggregate));
        SHR_IF_ERR_EXIT(bcm_dnx_trunk_spa_to_system_phys_port_map_get(unit, 0, system_port_aggregate, &gport));
        if (gport != members_p[member_index].gport)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SPA to system port and system port to SPA mapping failed\n"
                         "original system port was 0x%x, after mappings got system port 0x%x\n",
                         members_p[member_index].gport, gport);
        }
    }

    /** clean-up */
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_destroy(unit, trunk_id));

exit:
    SHR_FREE(members_p);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_trunk_spa_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const int ports_array_size = 2;
    uint32 ports_array[2];
    uint32 pool;
    uint32 group;
    SHR_FUNC_INIT_VARS(unit);

    /** get params */
    SH_SAND_GET_UINT32("pool", pool);
    SH_SAND_GET_UINT32("group", group);
    SH_SAND_GET_UINT32("port_1", ports_array[0]);
    SH_SAND_GET_UINT32("port_2", ports_array[1]);

    /** The Body of the test */
    SHR_IF_ERR_EXIT(dnx_trunk_spa_map_test(unit, pool, group, ports_array, ports_array_size));
    LOG_CLI((BSL_META("Test PASS\n")));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_trunk_config_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 max_nof_members_in_group;
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t ctest_soc_property[] = { {"", ""}, {NULL} };
    char soc_val_str[PARAMS_STR_OVERKILL_SIZE] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** get params */
    SH_SAND_GET_UINT32("size", max_nof_members_in_group);
    LOG_CLI((BSL_META("Test trunk configuration with %d as max number of members in group\n"),
             max_nof_members_in_group));

    /** define soc properties to change and change them*/
    ctest_soc_property[0].property = "trunk_group_max_members";
    sal_snprintf(soc_val_str, PARAMS_STR_OVERKILL_SIZE, "%d", max_nof_members_in_group);
    ctest_soc_property[0].value = soc_val_str;
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

    /** The Body of the test */
    SHR_IF_ERR_EXIT(dnx_trunk_config_test(unit));
    LOG_CLI((BSL_META("Test PASS\n")));

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

/** CB function to generate trunk config tests. */
static shr_error_e
dnx_trunk_config_test_create(
    int unit,
    rhlist_t * test_list)
{
    const dnxc_data_property_t *property;
    SHR_FUNC_INIT_VARS(unit);

    /** get all possible values for max_nof_members_in_group from DNX Data */
    SHR_IF_ERR_EXIT(dnxc_data_utils_generic_property_get
                    (unit, "trunk", "parameters", "pool_info", "max_nof_members_in_group", &property));
    if (property->mapping == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "mapping for property was not found\n");
    }

    /** iterate over possible values and initiate a test for each */
    for (int current_mapping = 0; current_mapping < property->nof_mapping; ++current_mapping)
    {
        char params_str[PARAMS_STR_OVERKILL_SIZE] = { 0 };
        char name_str[NAME_STR_OVERKILL_SIZE] = { 0 };
        sal_snprintf(params_str, PARAMS_STR_OVERKILL_SIZE, "size=%d", property->mapping[current_mapping].val);
        sal_snprintf(name_str, NAME_STR_OVERKILL_SIZE, "trunk_group_size_%d", property->mapping[current_mapping].val);
        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, name_str, params_str, 0), "Failed to add test");
    }

exit:
    SHR_FUNC_EXIT;
}

/************************************************
 * Consistent Distribution Test functions START *
 ***********************************************/
static sh_sand_man_t dnx_consistent_distribution_man = {
    .brief = "Run a test to check the consistent distribution of packets to CLAG member ports.",
    .full = "Test to check the consistent distribution of packets to CLAG member ports. Test procedures:\n"
        "\t1. Create trunk with user-defined ID and max number of members. Set it with the Consistent Hashing PSC. Add either max members or max/2 depending on full_group_mode.\n"
        "\t2. Send traffic to the trunk - 100 packets with different Source MACs to ensure different Load Balancing Keys. Save the distribution of the packets on the member ports.\n"
        "\t3. Remove one port and send again the same traffic. Check the distribution on the ports that remained in the trunk. Check that all packets that arrived on these ports in\n"
        "\tthe previous step are still arriving on them. Skip check for packets that were arriving on the removed port. Save again the distribution of the packets"
        "\t4. Re-add the port from the previous step and send again the same traffic. Check the distribution and compare it from the one from the previous step. make sure that the expected\n"
        "\t percentage of packets is arriving on the same ports as before." "\t4. Clean-up.",
    .synopsis = NULL,
    .examples = NULL
};

sh_sand_enum_t pool_id_table[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Max Pool ID supported",
     .value = 0,
     .plugin_str = "DNX_DATA.trunk.parameters.nof_pools-1"}
    ,
    {NULL}
};

static sh_sand_option_t dnx_consistent_distribution_options[] = {
    {"lag_id", SAL_FIELD_TYPE_INT32, "Link Aggregation Group ID", "0"},
    {"pool", SAL_FIELD_TYPE_INT32, "Pool ID", "0", pool_id_table, "0-max"},
    {"max_members", SAL_FIELD_TYPE_INT32, "Max number of group members", "10"},
    {"full_group_mode", SAL_FIELD_TYPE_BOOL, "Run test with a full group", "0"},
    {NULL}
};

static shr_error_e
dnx_consistent_distribution_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int lag_id = 0;
    int pool = 0;
    int max_members = 0;
    int trunk_id = 0;
    int last_port_to_add = 0;
    int iterator = 0;
    int current_first_phy = 0;
    int ilkn_port = 0;
    int ilkn_ports_nof = 0;
    uint8 full_group_mode = FALSE;
    uint32 log2_val = 0;
    uint32 power_of_2 = 0;
    uint32 nof_members_to_set = 0;
    uint32 flags = 0;
    uint32 vid = 100;
    char soc_val_str[PARAMS_STR_OVERKILL_SIZE] = { 0 };
    char port_to_remove[PARAMS_STR_OVERKILL_SIZE] = { 0 };
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t *ctest_soc_property = NULL;
    bcm_gport_t trunk_gport = 0;
    bcm_trunk_info_t trunk_info;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_trunk_member_t *members_array = NULL;
    bcm_l2_addr_t l2addr;
    bcm_port_t src_port = -1;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_pbmp_t ilkn_ports;
    bcm_pbmp_t p, u;
    bcm_mac_t mac = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 };
    ctest_cint_argument_t cint_arguments[10];
#if !defined(ADAPTER_SERVER_MODE)
    int removed_port = 0;
    float actual_packets_percent = 0;
    float expected_packets_percent = 0;
    int current_port = 0;
    int nof_packets_to_send = 100;
    int nof_packets_on_correct_ports = 0;
    int out_port_array_state2[100];
    int out_port_array_state1[100];
    uint64 recieved_packets;
    rhhandle_t packet_h = NULL;
    char mac_str[PARAMS_STR_OVERKILL_SIZE] = { 0 };
#endif
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Get input parameters
     */
    SH_SAND_GET_INT32("lag_id", lag_id);
    SH_SAND_GET_INT32("pool", pool);
    SH_SAND_GET_INT32("max_members", max_members);
    SH_SAND_GET_BOOL("full_group_mode", full_group_mode);

    /**
     * Check the value of max members.
     * There must be at least 2 members, because during the
     * test 1 will be removed and at least one needs to remain in the trunk
     */
    if (max_members < 2)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - max_members must be 2 or above!\n");
    }

    /**
     * For the goals of the test, we need to remove all NIF ports, including ELK ILKN.
     * However, dynamic port procedures for ELK ILKN ports are not supported, so we
     * need to get the existing ones and remove them using SOC properties
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, 0, &ilkn_ports));
    BCM_PBMP_COUNT(ilkn_ports, ilkn_ports_nof);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(ctest_soc_property, sizeof(ctest_soc_property_t) * (ilkn_ports_nof + 2),
                                "ctest_soc_property", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** Remove all ILKN ports using SOC property */
    iterator = 0;
    BCM_PBMP_ITER(ilkn_ports, ilkn_port)
    {
        sal_snprintf(port_to_remove, RHNAME_MAX_SIZE, "ucode_port_%d*", ilkn_port);
        SHR_ALLOC_SET_ZERO_ERR_EXIT(ctest_soc_property[iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                                    "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
        sal_strncpy(ctest_soc_property[iterator].property, port_to_remove, RHNAME_MAX_SIZE);
        ctest_soc_property[iterator].value = NULL;
        iterator++;
    }
    /** If needed change the max number of members to allow the creation of the requested group */
    if (max_members > dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group)
    {
        /** Value of the SOC property must be a power of two */
        log2_val = utilex_log2_round_up(max_members);
        power_of_2 = (1 << log2_val);

        /** Change the max number of members to allow the creation of the requested group */
        ctest_soc_property[iterator].property = "trunk_group_max_members";
        sal_snprintf(soc_val_str, PARAMS_STR_OVERKILL_SIZE, "%d", power_of_2);
        ctest_soc_property[iterator].value = soc_val_str;
        iterator++;
    }
    /** Last member must be NULL */
    ctest_soc_property[iterator].property = NULL;
    ctest_soc_property[iterator].value = NULL;
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

    /** Check the validity of the LAG ID */
    if (lag_id >= dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_groups_in_pool)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - Lag ID %d is above the maximum allowed in the current pool %d!\n", lag_id,
                     dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_groups_in_pool);
    }

    /** Load cints needed for ports dynamic procedures */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/port/cint_dynamic_port_add_remove.c"),
                        "cint_dynamic_port_add_remove.c Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/internal/port/cint_test_dynamic_port_add.c"),
                        "cint_dynamic_port_add_remove.c Load Failed\n");
    /** Remove all NIF ports */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_dyn_port_remove_all", NULL, 0),
                        "cint_dyn_port_remove_all Run Failed\n");

    /** Set the cint arguments structure with initial values */
    for (iterator = 0; iterator < 10; iterator++)
    {
        cint_arguments[iterator].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[iterator].value.value_int32 = 0;
    }

    /**
     * Create the needed amount of ports.
     * The iteration starts at 1 because logical port 0
     * is the main CPU port and was not removed. One additional
     * port is added to use as Source Port, it will be the last one to be added.
     */
    nof_members_to_set = (full_group_mode == TRUE) ? max_members : (max_members / 2);
    last_port_to_add = nof_members_to_set + 1;
    src_port = last_port_to_add;
    for (iterator = 1; iterator <= last_port_to_add; iterator++)
    {
        if (current_first_phy >= dnx_data_nif.phys.nof_phys_get(unit))
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Cannot find free Phy to allocate port index=%d \n", iterator);
        }
        cint_arguments[0].value.value_int32 = iterator; /** port_to_add */
        cint_arguments[1].value.value_int32 = 0; /** channel */
        cint_arguments[2].value.value_int32 = iterator; /** tm_port */
        cint_arguments[3].value.value_int32 = 2; /** num_priorities */
        cint_arguments[4].value.value_int32 = current_first_phy; /** first_phy */
        cint_arguments[5].value.value_int32 = BCM_PORT_IF_NIF_ETH; /** interface */
        cint_arguments[6].value.value_int32 = 1; /** num_lanes */
        cint_arguments[7].value.value_int32 = 10000; /** speed mbps**/
        cint_arguments[8].value.value_int32 = 0; /** flags */
        cint_arguments[9].value.value_int32 = 0; /** offset */
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "dynamic_port_add", cint_arguments, 10),
                            "dynamic_port_add Run Failed\n");
        current_first_phy++;
    }

    /** Create trunk with user ID */
    BCM_TRUNK_ID_SET(trunk_id, pool, lag_id);
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_create(unit, 0, &trunk_id));

    /** Set trunk header type. Index 0 means direction BOTH */
    key.index = 0;
    key.type = bcmSwitchPortHeaderType;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, trunk_gport, key, value));

    /** Allocate the members array and fill it */
    SHR_ALLOC_SET_ZERO(members_array, sizeof(bcm_trunk_member_t) * nof_members_to_set, "members_array", "%s%s%s", EMPTY,
                       EMPTY, EMPTY);
    for (iterator = 0; iterator < nof_members_to_set; iterator++)
    {
        BCM_GPORT_SYSTEM_PORT_ID_SET(members_array[iterator].gport, (iterator + 1));
    }

    /** Set trunk info and the members to the trunk */
    trunk_info.psc = BCM_TRUNK_PSC_C_LAG;
    trunk_info.psc_info.max_nof_members_in_profile = max_members;
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_set(unit, trunk_id, &trunk_info, nof_members_to_set, members_array));

    /** Disable learning for the source port port. */
    SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, src_port, BCM_PORT_LEARN_FWD));

    /** Configure L2 entry for traffic with DA=1 and VLAN tag ID 100 to trunk */
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = trunk_gport;
    l2addr.tgid = trunk_id;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

    /** Add src_port to the VLAN */
    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(u);
    BCM_PBMP_PORT_SET(p, src_port);
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vid, p, u));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid, src_port, 0));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid, trunk_gport, 0));

    /**
     * Set up traffic with CPU Packet launcher and send 100 packets.
     * For each packet change the SA and record through which out port it exited.
     */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));
#if !defined(ADAPTER_SERVER_MODE)
    for (iterator = 0; iterator < nof_packets_to_send; iterator++)
    {
        /** Clear stats on all port that are used in the trunk */
        for (current_port = 1; current_port < last_port_to_add; current_port++)
        {
            SHR_IF_ERR_EXIT(bcm_stat_clear(unit, current_port));
        }

        /** Create packet and send traffic */
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vid, 12));
        sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "00:00:00:00:%02x:01", iterator);
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", "00:00:00:00:00:01"));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_h, SAND_PACKET_RX));

        /** Get the out port of the packet and save it in an array */
        for (current_port = 1; current_port < last_port_to_add; current_port++)
        {
            COMPILER_64_ZERO(recieved_packets);
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, current_port, snmpEtherStatsTXNoErrors, &recieved_packets));
            if (COMPILER_64_LO(recieved_packets) != 0)
            {
                out_port_array_state1[iterator] = current_port;
            }
        }

        /** Free the packet, so it can be created with different headers in the next iteration */
        diag_sand_packet_free(unit, packet_h);
    }
#endif

    /** Remove one member of the trunk, somewhere in the middle */
    SHR_IF_ERR_EXIT(bcm_trunk_member_delete(unit, trunk_id, &members_array[nof_members_to_set / 2]));

#if !defined(ADAPTER_SERVER_MODE)
    /** Repeat the above traffic and make sure the same packets are arriving on the ports that remain in the trunk */
    removed_port = BCM_GPORT_SYSTEM_PORT_ID_GET(members_array[nof_members_to_set / 2].gport);
    for (iterator = 0; iterator < nof_packets_to_send; iterator++)
    {
         /** Clear stats on all port that are used in the trunk */
        for (current_port = 1; current_port < last_port_to_add; current_port++)
        {
            SHR_IF_ERR_EXIT(bcm_stat_clear(unit, current_port));
        }

         /** Create packet and send traffic */
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vid, 12));
        sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "00:00:00:00:%02x:01", iterator);
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", "00:00:00:00:00:01"));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_h, SAND_PACKET_RX));

        /**
         * Get the out port of the packet and save it in another out ports array.
         * If the packet was not previously exiting through the removed port, compare it to what is was in the previous state
         */
        for (current_port = 1; current_port < last_port_to_add; current_port++)
        {
            COMPILER_64_ZERO(recieved_packets);
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, current_port, snmpEtherStatsTXNoErrors, &recieved_packets));
            if (COMPILER_64_LO(recieved_packets) != 0)
            {
                /** Check that the packet arrived on the same port */
                if ((out_port_array_state1[iterator] != removed_port)
                    && (out_port_array_state1[iterator] != current_port))
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "FAILURE - Expected packet with SA 00:00:00:00:%02x:01 on port %d but got it on port %d.\n",
                                 out_port_array_state1[iterator], iterator, current_port);
                }

                /** Save the out ports of the second state */
                out_port_array_state2[iterator] = current_port;
            }
        }
    }
#endif

        /** Re-add the removed port */
    SHR_IF_ERR_EXIT(bcm_trunk_member_add(unit, trunk_id, &members_array[nof_members_to_set / 2]));

#if !defined(ADAPTER_SERVER_MODE)
    /** Repeat the above traffic and make sure the amount packets that are arriving on the same ports is as expected */
    for (iterator = 0; iterator < nof_packets_to_send; iterator++)
    {
        /** Clear stats on all port that are used in the trunk */
        for (current_port = 1; current_port < last_port_to_add; current_port++)
        {
            SHR_IF_ERR_EXIT(bcm_stat_clear(unit, current_port));
        }

        /** Create packet and send traffic */
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vid, 12));
        sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "00:00:00:00:%02x:01", iterator);
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", "00:00:00:00:00:01"));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_h, SAND_PACKET_RX));

        /** Get the out port of the packet and compare it to what is was in the previous state. If yes, then count it. */
        for (current_port = 1; current_port < last_port_to_add; current_port++)
        {
            COMPILER_64_ZERO(recieved_packets);
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, current_port, snmpEtherStatsTXNoErrors, &recieved_packets));
            if ((COMPILER_64_LO(recieved_packets) != 0) && (out_port_array_state2[iterator] == current_port))
            {
                nof_packets_on_correct_ports++;
            }
        }

         /** Free the packet, so it can be created with different headers in the next iteration */
        diag_sand_packet_free(unit, packet_h);
    }

     /**
      * Check if the correct % of packets arrived on the expected ports.
      * The allowed deviation is the nominal deviation of 1.5 members.
      * Example - if we have 10 members and the deviation for 1 member is 90%,
      * the deviation for 1.5 members will be 85%.
      */
    expected_packets_percent = ((nof_members_to_set - 1.5) * 100) / (nof_members_to_set);
    actual_packets_percent = (nof_packets_on_correct_ports * 100) / nof_packets_to_send;
    if (actual_packets_percent < expected_packets_percent)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - Expected %.1f to arrive on the correct ports, but only %.1f did.\n",
                     expected_packets_percent, actual_packets_percent);
    }
#endif

    /** Reset Cints */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");

    /** Delete all members and destroy trunk */
    SHR_IF_ERR_EXIT(bcm_trunk_member_delete_all(unit, trunk_id));
    SHR_IF_ERR_EXIT(bcm_trunk_destroy(unit, trunk_id));

    /** Clear L2 entries */
    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));

    /** Destroy the VLAN */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_destroy(unit, vid));

exit:
    SHR_FREE(members_array);
    for (iterator = 0; iterator < (ilkn_ports_nof + 2); iterator++)
    {
        if (ctest_soc_property)
        {
            SHR_FREE(ctest_soc_property[iterator].property);
            SHR_FREE(ctest_soc_property[iterator].value);
        }
    }
    SHR_FREE(ctest_soc_property);
    if (ctest_soc_set_h != NULL)
    {
        ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_invoke_t dnx_consistent_distribution_tests[] = {
    {"profile_type_0_pool_0_full_grp", "lag_id=2 pool=0 max_members=10 full_group_mode=1"},
    {"profile_type_0_pool_1_full_grp", "lag_id=2 pool=1 max_members=10 full_group_mode=1"},
    {"profile_type_0_pool_0", "lag_id=2 pool=0 max_members=10 full_group_mode=0"},
    {"profile_type_0_pool_1", "lag_id=2 pool=1 max_members=10 full_group_mode=0"},
    {NULL}
};
/**********************************************
 * Consistent Distribution Test functions END *
 **********************************************/

/************************************************
 * Profiles Defragmentation Test functions START *
 ***********************************************/
static sh_sand_man_t dnx_profiles_defragmentation_man = {
    .brief = "Run a test to check the defragmentation of Consistent Hashing profiles.",
    .full =
        /** The resources for the Consistent Hashing profiles are not per pool, the test scenario can be mixed on both pools.
         *  The differentiation between pools id done for comfort reasons , to not overcomplicate the test */
        "Test to check the defragmentation of Consistent Hashing profiles for a given pool. There are 3 profile types. profiles of up to 16, 64 or 256 members.\n"
        "Each of those types consumes 1, 4, or 8 resources accordingly. There is a finite amount resources available. Test procedures:\n"
        "\t1. Create 8 trunks of type 1 profile (under 16 members), two trunks of type 2 profile (over 16 and under 64 members) and full the remaining resources with trunks of type 3 profile (over 64 and under 256 members).\n"
        "\t2. Destroy trunks 1,3,5,7(type 1) and trunk 9(type 2).\n"
        "\t3. Create one trunk with profile of type 3 (over 64 and under 256 members). This will take up 8 resources, but the free resources are fragmented, so defragmentation will have to be performed.\n"
        "\t4. Send traffic to the trunks 0,2,4,6(type 1) and trunk 8(type 2) - 1000 packets,  check that the packets are arriving to the correct destination\n"
        "\t5. Send traffic to the trunk that was added in step 4 and make sure it is arriving to all destinations.\n"
        "\t6. Clean-up.",
    .synopsis = NULL,
    .examples = NULL
};

static sh_sand_option_t dnx_profiles_defragmentation_options[] = {
    {"pool", SAL_FIELD_TYPE_INT32, "Pool ID", "0", pool_id_table, "0-max"},
    {NULL}
};

static shr_error_e
dnx_profiles_defragmentation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int lag_id_max = 0;
    int pool = 0;
    int trunk_id = 0;
    bcm_trunk_member_t members_array[70];
    int current_first_phy = 0;
    int iterator = 0;
    int member_iterator = 0;
    ctest_cint_argument_t cint_arguments[10];
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t *ctest_soc_property = NULL;
    bcm_gport_t trunk_gport = 0;
    bcm_trunk_info_t trunk_info;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 };
    uint32 vid = 100;
    bcm_port_t src_port = -1;
    char port_to_remove[RHNAME_MAX_SIZE];
    bcm_pbmp_t p, u;
    uint32 nof_members_to_set = 0;
    bcm_pbmp_t ilkn_ports;
    int ilkn_port = 0;
    int ilkn_ports_nof = 0;
#if !defined(ADAPTER_SERVER_MODE)
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags = 0;
    int packet = 0;
    int nof_packets = 10;
    uint64 recieved_packets;
    rhhandle_t packet_h = NULL;
#endif
    SHR_FUNC_INIT_VARS(unit);

    /** Get input parameters */
    SH_SAND_GET_INT32("pool", pool);

    /**
     * For the goals of the test, we need to remove all NIF ports, including ELK ILKN.
     * However, dynamic port procedures for ELK ILKN ports are not supported, so we
     * need to get the existing ones and remove them using SOC properties
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, 0, &ilkn_ports));
    BCM_PBMP_COUNT(ilkn_ports, ilkn_ports_nof);
    SHR_ALLOC_SET_ZERO(ctest_soc_property, sizeof(ctest_soc_property_t) * (ilkn_ports_nof + 2), "ctest_soc_property",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** Remove all ILKN ports using SOC property */
    iterator = 0;
    BCM_PBMP_ITER(ilkn_ports, ilkn_port)
    {
        sal_snprintf(port_to_remove, RHNAME_MAX_SIZE, "ucode_port_%d*", ilkn_port);
        SHR_ALLOC_SET_ZERO(ctest_soc_property[iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                           "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
        sal_strncpy(ctest_soc_property[iterator].property, port_to_remove, RHNAME_MAX_SIZE);
        ctest_soc_property[iterator].value = NULL;
        iterator++;
    }
    /** Change the max number of members to allow the creation of the requested groups. Set SOC property trunk_group_max_members to the maximum value */
    ctest_soc_property[iterator].property = "trunk_group_max_members";
    ctest_soc_property[iterator].value = "256";
    /** Last member must be NULL */
    ctest_soc_property[iterator + 1].property = NULL;
    ctest_soc_property[iterator + 1].value = NULL;
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

    /** Load cints needed for ports dynamic procedures */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/port/cint_dynamic_port_add_remove.c"),
                        "cint_dynamic_port_add_remove.c Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/internal/port/cint_test_dynamic_port_add.c"),
                        "cint_dynamic_port_add_remove.c Load Failed\n");
    /** Remove all NIF ports */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_dyn_port_remove_all", NULL, 0),
                        "cint_dyn_port_remove_all Run Failed\n");

    /** Set the cint arguments structure with initial values */
    for (iterator = 0; iterator < 10; iterator++)
    {
        cint_arguments[iterator].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[iterator].value.value_int32 = 0;
    }

    /**
     * Add the needed amount of ports, we need one port for each group.
     * We need to create groups to occupy the total resources.
     * Use the same loop to also create and set the groups.
     * An additional port will be added to be used as in_port.
     * The iteration starts at 1, because port 0 is the main CPU port and was not removed.
     * The amount of groups/ports that will be added is calculated based on the total resources.
     * We need to create 6 groups of type 1 profile, two groups of type 2 profile and full the remaining
     * resources with trunks of type 3 profile.Those types consumes 1, 4, or 8 resources accordingly.
     */
    lag_id_max = dnx_data_trunk.psc.consistant_hashing_nof_resources_get(unit) / 8 + 8;
    /** Since the IDs are 1-based and we need one more additional ID, so add 1 */
    lag_id_max = lag_id_max + 1;
    for (iterator = 1; iterator <= lag_id_max; iterator++)
    {
        if (current_first_phy >= dnx_data_nif.phys.nof_phys_get(unit))
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Cannot find free Phy to allocate port index=%d \n", iterator);
        }
        cint_arguments[0].value.value_int32 = iterator; /** port_to_add */
        cint_arguments[1].value.value_int32 = 0; /** channel */
        cint_arguments[2].value.value_int32 = iterator; /** tm_port */
        cint_arguments[3].value.value_int32 = 2; /** num_priorities */
        cint_arguments[4].value.value_int32 = current_first_phy; /** first_phy */
        cint_arguments[5].value.value_int32 = BCM_PORT_IF_NIF_ETH; /** interface */
        cint_arguments[6].value.value_int32 = 1; /** num_lanes */
        cint_arguments[7].value.value_int32 = 10000; /** speed mbps**/
        cint_arguments[8].value.value_int32 = 0; /** flags */
        cint_arguments[9].value.value_int32 = 0; /** offset */
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "dynamic_port_add", cint_arguments, 10),
                            "dynamic_port_add Run Failed\n");
        current_first_phy++;

        if (iterator == lag_id_max)
        {
            /** The source port will be port the last */
            src_port = iterator;

            /** Disable learning for the port. */
            SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, src_port, BCM_PORT_LEARN_FWD));

            /** Add src_port to the VLAN */
            SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vid));
            BCM_PBMP_CLEAR(u);
            BCM_PBMP_PORT_SET(p, src_port);
            SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vid, p, u));
            SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid, src_port, 0));
            continue;
        }

        /** Create trunk */
        BCM_TRUNK_ID_SET(trunk_id, pool, iterator);
        SHR_IF_ERR_EXIT(bcm_dnx_trunk_create(unit, 0, &trunk_id));

        /** Set trunk header type. Index 0 means direction BOTH */
        key.index = 0;
        key.type = bcmSwitchPortHeaderType;
        value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
        SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, trunk_gport, key, value));

        /**
         * Fill the members array with repeating members.
         * Groups 1, 2, 3, 4, 5, 6, 7, and 8 with 10 members (< 16 members, meaning profile type 0, each group taking 1 resource) - 8 resources total
         * Groups 9 and 10 with 30 members (between 16 and 64 members, meaning profile type 1, each group taking 4 resources) - 8 resources total
         * Groups 11 to the max with 70 members (between 64 and 256 members, meaning profile type 2, each group taking 8 resources) - max resources total
         */
        sal_memset(members_array, 0x0, sizeof(bcm_trunk_member_t) * 70);
        nof_members_to_set = (iterator <= 8) ? 10 : ((iterator <= 10) ? 30 : 70);
        for (member_iterator = 0; member_iterator < nof_members_to_set; member_iterator++)
        {
            BCM_GPORT_SYSTEM_PORT_ID_SET(members_array[member_iterator].gport, iterator);
        }

        /** Set trunk info */
        trunk_info.psc = BCM_TRUNK_PSC_C_LAG;
        trunk_info.psc_info.max_nof_members_in_profile = nof_members_to_set;
        SHR_IF_ERR_EXIT(bcm_dnx_trunk_set(unit, trunk_id, &trunk_info, nof_members_to_set, members_array));
    }

    /** Destroy trunks 1,3,5,7 and trunk 9 */
    for (iterator = 1; iterator <= 9; iterator++)
    {
        if (iterator % 2)
        {
            /** Delete all members and destroy trunk */
            BCM_TRUNK_ID_SET(trunk_id, pool, iterator);
            SHR_IF_ERR_EXIT(bcm_trunk_member_delete_all(unit, trunk_id));
            SHR_IF_ERR_EXIT(bcm_trunk_destroy(unit, trunk_id));
        }
    }

    /**
     * Create one trunk with profile of type 3 (over 64 and under 256 members). This will take up 8 resources, but the free resources are
     * fragmented, so defragmentation will have to be performed. Use Lag ID=42 and port 9 which was released from the deleted trunks.
     */
    BCM_TRUNK_ID_SET(trunk_id, pool, lag_id_max);
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_create(unit, 0, &trunk_id));

    /** Set trunk header type. Index 0 means direction BOTH */
    key.index = 0;
    key.type = bcmSwitchPortHeaderType;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, trunk_gport, key, value));

    /** Fill the members array with repeating members. */
    nof_members_to_set = 70;
    for (member_iterator = 0; member_iterator < nof_members_to_set; member_iterator++)
    {
        BCM_GPORT_SYSTEM_PORT_ID_SET(members_array[member_iterator].gport, 9);
    }

    /** Set trunk info */
    trunk_info.psc = BCM_TRUNK_PSC_C_LAG;
    trunk_info.psc_info.max_nof_members_in_profile = nof_members_to_set;
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_set(unit, trunk_id, &trunk_info, nof_members_to_set, members_array));

#if !defined(ADAPTER_SERVER_MODE)
    /** Create packet */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vid, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", "00:00:00:00:00:02"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", "00:00:00:00:00:01"));
#endif
    /**
     * Send traffic to the trunks 2,4,6,8(type 1) and trunk 10(type 2) - 10 packets,
     * check that the packets are arriving to the correct destination
     */
    for (iterator = 2; iterator <= 10; iterator++)
    {
        if (iterator % 2)
        {
            continue;
        }

        /** Configure L2 entry for traffic with DA=1 and VLAN tag ID 100 to trunk */
        BCM_TRUNK_ID_SET(trunk_id, pool, iterator);
        BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
        bcm_l2_addr_t_init(&l2addr, mac, vid);
        l2addr.flags = BCM_L2_STATIC;
        l2addr.port = trunk_gport;
        l2addr.tgid = trunk_id;
        SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));
#if !defined(ADAPTER_SERVER_MODE)
        /** Clear stats on all port that are used in the trunk */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, iterator));

        /** Send_traffic, 10 packets */
        for (packet = 0; packet < nof_packets; packet++)
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_h, SAND_PACKET_RX));
        }

        /** Get stats on the port */
        COMPILER_64_ZERO(recieved_packets);
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
        SHR_IF_ERR_EXIT(bcm_stat_get(unit, iterator, snmpEtherStatsTXNoErrors, &recieved_packets));

        /** All packets should be received. */
        if (COMPILER_64_LO(recieved_packets) != nof_packets)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - Expected  %d packets on port %d but got %d.\n", nof_packets, iterator,
                         COMPILER_64_LO(recieved_packets));
        }
#endif
        /** Clear L2 entries */
        SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));
    }

    /** Also send traffic to trunk that was last added
     *  Configure L2 entry for traffic with DA=1 and VLAN tag ID 100 to trunk */
    BCM_TRUNK_ID_SET(trunk_id, pool, lag_id_max);
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = trunk_gport;
    l2addr.tgid = trunk_id;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));
#if !defined(ADAPTER_SERVER_MODE)
     /** Clear stats on all port that are used in the trunk */
    SHR_IF_ERR_EXIT(bcm_stat_clear(unit, 9));

      /** Send_traffic, 10 packets */
    for (packet = 0; packet < nof_packets; packet++)
    {
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_h, SAND_PACKET_RX));
    }

     /** Get stats on the port */
    COMPILER_64_ZERO(recieved_packets);
    SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
    SHR_IF_ERR_EXIT(bcm_stat_get(unit, 9, snmpEtherStatsTXNoErrors, &recieved_packets));

     /** All packets should be received. */
    if (COMPILER_64_LO(recieved_packets) < nof_packets)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - Expected  %d packets on port %d but got %d.\n", nof_packets, 9,
                     COMPILER_64_LO(recieved_packets));
    }
#endif
     /** Clear L2 entries */
    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));

     /** Reset Cints */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");

     /** Destroy the VLAN */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_destroy(unit, vid));

exit:
#if !defined(ADAPTER_SERVER_MODE)
    if (packet_h != NULL)
    {
        diag_sand_packet_free(unit, packet_h);
    }
#endif
    if (ctest_soc_set_h != NULL)
    {
        ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_invoke_t dnx_profiles_defragmentation_tests[] = {
    {"profiles_defragmentation_pool_0", "pool=0"},
    {"profiles_defragmentation_pool_1", "pool=1"},
    {NULL}
};
/***********************************************
 * Profiles Defragmentation Test functions END *
 ***********************************************/

/***************************************************************************************************
 * This is the entry point for DNX C-Tests for Consistent Hashing Link Aggregation Groups (C-LAG). *
 * Consistent Hashing is a Port Selection Criteria for LAGs that                                   *
 * assures the alignment between LB-Keys and LAG members remains consistent.                       *
 ***************************************************************************************************/
/* *INDENT-OFF* */
static sh_sand_cmd_t dnx_clag_tests_cmds[] = {
    /** Command name             |Action                           |Child    |Options                            |Manual                            |CB   |Static invokes                     |Flags         */
    {"consistent_distribution", dnx_consistent_distribution_cmd, NULL, dnx_consistent_distribution_options,      &dnx_consistent_distribution_man,  NULL, dnx_consistent_distribution_tests,  CTEST_POSTCOMMIT},
    {"profiles_defragmentation", dnx_profiles_defragmentation_cmd, NULL, dnx_profiles_defragmentation_options,   &dnx_profiles_defragmentation_man, NULL, dnx_profiles_defragmentation_tests, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/***************************************************************************************************/

static sh_sand_man_t dnx_trunk_config_man = {
    "Test Trunk different configurations.",
    "Load init sequence with different trunk configuration to validate that at least semantically they are not causing ant unexpected failures.",
    NULL,
    "ctest trunk config size=128",
};

static sh_sand_man_t dnx_trunk_spa_map_man = {
    "Test mapping between system port to SPA.",
    "Run internal mapping function between system port and trunk to SPA.",
    NULL,
};

static sh_sand_option_t dnx_trunk_config_options[] = {
    {"size", SAL_FIELD_TYPE_UINT32, "max number of members in trunk group", "64"},
    {NULL}
};

static sh_sand_option_t dnx_trunk_spa_map_options[] = {
    {"pool", SAL_FIELD_TYPE_UINT32, "trunk pool to use", "0"},
    {"group", SAL_FIELD_TYPE_UINT32, "trunk group to use", "0"},
    {"port_1", SAL_FIELD_TYPE_UINT32, "first port member in trunk", "13"},
    {"port_2", SAL_FIELD_TYPE_UINT32, "second port member in trunk", "14"},
    {NULL}
};

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_trunk_test_cmds[] = {
    /** CMD_name |Action                 |Child               |Options                   |Manual                 |CB   |Static invokes |Flags |Invoke_CB                     */
    {"config",    dnx_trunk_config_cmd,  NULL,                dnx_trunk_config_options,  &dnx_trunk_config_man,  NULL, NULL,           0,     dnx_trunk_config_test_create},
    {"map",       dnx_trunk_spa_map_cmd, NULL,                dnx_trunk_spa_map_options, &dnx_trunk_spa_map_man, NULL, NULL,           0,     NULL},
    {"clag",      NULL,                  dnx_clag_tests_cmds},
    {NULL}
};
/* *INDENT-ON* */
