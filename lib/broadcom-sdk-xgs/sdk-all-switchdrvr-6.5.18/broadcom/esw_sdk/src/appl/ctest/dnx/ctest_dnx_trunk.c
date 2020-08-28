/** \file ctest_dnx_trunk.c
 *
 * ctests for trunk
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

#include <bcm_int/dnx_dispatch.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include "appl/ctest/dnxc/ctest_dnxc_system.h"

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
/*  sal_memset(members, 0, sizeof(members)); */

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

static sh_sand_man_t dnx_trunk_config_man = {
    "Test Trunk different configurations.",
    "Load init sequence with different trunk configuration to validate that at least semantically they are not causing ant unexpected failures.",
    NULL,
    "ctest trunk config size=128",
};

static sh_sand_option_t dnx_trunk_config_options[] = {
    {"size", SAL_FIELD_TYPE_UINT32, "max number of members in trunk group", "64"},
    {NULL}
};

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_trunk_test_cmds[] = {
    /** CMD_name |Action               |Child |Options                  |Manual                |CB   |Static invokes |Flags |Invoke_CB                     */
    {"config",    dnx_trunk_config_cmd, NULL,  dnx_trunk_config_options, &dnx_trunk_config_man, NULL, NULL,           0,     dnx_trunk_config_test_create},
    {NULL}
};
/* *INDENT-ON* */
