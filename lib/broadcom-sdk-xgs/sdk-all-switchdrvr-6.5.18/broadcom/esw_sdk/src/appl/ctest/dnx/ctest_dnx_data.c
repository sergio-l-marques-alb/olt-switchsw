/** \file diag_dnxc_data_c
 * 
 * DEVICE DATA TEST - Testing DNX DATA module 
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h' 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

/*
 * INCLUDE FILES:
 * {
 */
/*soc*/
/*sal*/
#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

/**
 * \brief - Test Supported/Unsupported Data types 
 * used to make sure that supported data will return the expected values
 */
static shr_error_e
dnx_data_test_support(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int feature_val;
    uint32 define_val;
    const uint32 *generic_val;
    const dnx_data_module_testing_unsupported_supported_table_t *sup_table_val;
    int res = 0;                /* test result - assume pass */

    SHR_FUNC_INIT_VARS(unit);

    /*
     * features
     */
    feature_val = dnx_data_module_testing.unsupported.feature_get
        (unit, dnx_data_module_testing_unsupported_supported_feature);
    if (feature_val != 1)
    {

        /*
         * required data wasn't found throw print an error
         */
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that supported feature will return 1\n")));
        res = 1;
    }

    feature_val = dnx_data_module_testing.unsupported.feature_get
        (unit, dnx_data_module_testing_unsupported_unsupported_feature);
    if (feature_val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that unsupported feature will return 0\n")));
        res = 1;
    }

    /*
     * defines
     */
    define_val = dnx_data_module_testing.unsupported.supported_def_get(unit);
    if (define_val != 0x12345678)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that supported define will return correct value, got %x\n"),
                   define_val));
        res = 1;
    }

    /*
     * tables
     */
    sup_table_val = dnx_data_module_testing.unsupported.supported_table_get(unit);
    if (sup_table_val == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that supported table will return correct value\n")));
        res = 1;
    }
    else if (sup_table_val->num != 0x87654321)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that supported table will return correct value, got %x\n"),
                   sup_table_val->num));
        res = 1;
    }

    /*
     * Test generic Access API
     */
    /*
     * features
     */
    generic_val = dnxc_data_utils_generic_data_get(unit, "module_testing", "unsupported", "supported_feature", NULL);
    if (generic_val == NULL || *generic_val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that supported feature will return 1 (generic API)\n")));
        res = 1;
    }
    generic_val = dnxc_data_utils_generic_data_get(unit, "module_testing", "unsupported", "unsupported_feature", NULL);
    if (generic_val == NULL || *generic_val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that unsupported feature will return 0 (generic API)\n")));
        res = 1;
    }

    if (res != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
    }
    SHR_FUNC_EXIT;
}
/**
 * brief - Test different data types (of members in tables)
 */
static shr_error_e
dnx_data_test_types(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_data_module_testing_types_all_t *types_table_val;
    int res = 0;                /* test result - assume pass */

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get table struct
     */
    types_table_val = dnx_data_module_testing.types.all_get(unit);
    if (types_table_val == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that supported table will not return NULL\n")));
        res = 1;
    }
    else
    {
        /*
         * check values
         */
        if (types_table_val->intval != -1)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting intval=-1, got %d\n"), types_table_val->intval));
            res = 1;
        }
        if (types_table_val->uint8val != 0x12)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint8val=-1, got %d\n"), types_table_val->uint8val));
            res = 1;
        }
        if (types_table_val->charval != '1')
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting charval='c', got %d\n"), types_table_val->charval));
            res = 1;
        }
        if (types_table_val->uint16val != 0x1234)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint16val=0x1234, got %x\n"), types_table_val->uint16val));
            res = 1;
        }
        if (types_table_val->enumval != bcmFabricDeviceTypeFE2)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint16val=bcmFabricDeviceTypeFE2, got %d\n"),
                       types_table_val->enumval));
            res = 1;
        }
        if (types_table_val->arrval[0] != 1 ||
            types_table_val->arrval[1] != 2 || types_table_val->arrval[2] != 3 || types_table_val->arrval[3] != 4)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting arrval={1, 2, 3, 4, 5, 6, 7, 8, 9, 10}\n")));
            res = 1;
        }
        if (!BCM_PBMP_MEMBER(types_table_val->pbmpval, 33))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting port 33 to be in pbmpval\n")));
            res = 1;
        }

        if (sal_strncasecmp(types_table_val->strval, "dnx data", strlen("dnx data")))
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting strval=\"dnx data\", got %s\n"), types_table_val->strval));
            res = 1;
        }
        if (types_table_val->bufferval[1] != 'b')
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting bufferval[1]='b', got %d\n"),
                       types_table_val->bufferval[1]));
            res = 1;
        }
    }

    if (res != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Test different property methods
 */
static shr_error_e
dnx_data_test_property(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 numeric;
    int feature;
    const dnx_data_module_testing_property_methods_enable_t *enable;
    const dnx_data_module_testing_property_methods_port_enable_t *port_enable;
    const dnx_data_module_testing_property_methods_suffix_enable_t *suffix_enable;
    const dnx_data_module_testing_property_methods_range_t *range;
    const dnx_data_module_testing_property_methods_port_range_t *port_range;
    const dnx_data_module_testing_property_methods_suffix_range_t *suffix_range;
    const dnx_data_module_testing_property_methods_direct_map_t *direct_map;
    const dnx_data_module_testing_property_methods_port_direct_map_t *port_direct_map;
    const dnx_data_module_testing_property_methods_suffix_direct_map_t *suffix_direct_map;
    const dnx_data_module_testing_property_methods_custom_t *custom;
    const dnx_data_module_testing_property_methods_pbmp_t *pbmp;
    const dnx_data_module_testing_property_methods_port_pbmp_t *port_pbmp;
    const dnx_data_module_testing_property_methods_suffix_pbmp_t *suffix_pbmp;
    const dnxc_data_table_info_t *table_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Add relevant SoC properties
     */
    if (sh_process_command(unit, "config add dnx_data_feature_enable=1") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_numeric_range=2") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_enable=1") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_port_enable_0=0") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_suffix_enable_link4=0") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_range=5") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_port_range_1=3") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_suffix_range_link2=4") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_direct_map=HIGH") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_port_direct_map_0=LOW") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_suffix_direct_map_link3=NORMAL") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_custom_link2=1") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    /*
     * pbmp
     */
    if (sh_process_command(unit, "config add dnx_data_pbmp=0x8000") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_port_pbmp_2=0x80000") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    if (sh_process_command(unit, "config add dnx_data_suffix_pbmp_index3=0x200000") != CMD_OK)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error -  config add command failed\n");
    }
    /*
     * Run Deinit-Init DNX DATA - the SoC properties loaded upon init
     */
    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141"), "Error - init-deinit failed\n");
    /*
     * Get and Compare
     */
    /*
     * feature enable
     */
    feature = dnx_data_module_testing.property_methods.feature_get(unit,
                                                                   dnx_data_module_testing_property_methods_feature_enable);
    if (feature != 1)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting feature enable to be 1, got %d\n", feature);
    }
    /*
     * feature disable
     */
    feature = dnx_data_module_testing.property_methods.feature_get(unit,
                                                                   dnx_data_module_testing_property_methods_feature_disable);
    if (feature != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting feature disable to be 0, got %d\n", feature);
    }
    /*
     * numeric range 
     */
    numeric = dnx_data_module_testing.property_methods.numeric_range_get(unit);
    if (numeric != 2)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting numeric range to be 2, got %d\n", feature);
    }
    /*
     * tables
     */
    /*
     * enable
     */
    enable = dnx_data_module_testing.property_methods.enable_get(unit);
    if (enable->val != 1)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting enable to be 1, got %d\n", enable->val);
    }

    port_enable = dnx_data_module_testing.property_methods.port_enable_get(unit, 0);
    if (port_enable->val != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting port enable to be 0, got %d\n", port_enable->val);
    }

    suffix_enable = dnx_data_module_testing.property_methods.suffix_enable_get(unit, 4);
    if (suffix_enable->val != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting suffix enable to be 0, got %d\n", suffix_enable->val);
    }
    /*
     * range
     */
    range = dnx_data_module_testing.property_methods.range_get(unit);
    if (range->val != 5)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting range to be 5, got %d\n", range->val);
    }

    port_range = dnx_data_module_testing.property_methods.port_range_get(unit, 1);
    if (port_range->val != 3)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting port range to be 3, got %d\n", port_range->val);
    }

    suffix_range = dnx_data_module_testing.property_methods.suffix_range_get(unit, 2);
    if (suffix_range->val != 4)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting suffix range to be 4, got %d\n", suffix_range->val);
    }
    /*
     * direct map
     */
    direct_map = dnx_data_module_testing.property_methods.direct_map_get(unit);
    if (direct_map->val != 1)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting direct_map to be 1, got %d\n", direct_map->val);
    }

    port_direct_map = dnx_data_module_testing.property_methods.port_direct_map_get(unit, 0);
    if (port_direct_map->val != 2)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting port direct_map to be 2, got %d\n", port_direct_map->val);
    }

    suffix_direct_map = dnx_data_module_testing.property_methods.suffix_direct_map_get(unit, 3);
    if (suffix_direct_map->val != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting x direct_map to be 0, got %d\n", suffix_direct_map->val);
    }
    /*
     * custom
     */
    custom = dnx_data_module_testing.property_methods.custom_get(unit, 2, 1);
    if (custom->val != 1)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting custom to be 1, got %d\n", custom->val);
    }
    /*
     * check key sizes
     */
    table_info = dnx_data_module_testing.property_methods.custom_info_get(unit);
    if (table_info->key_size[0] != 5 || table_info->key_size[1] != 2)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting Expecting different custom key size\n");
    }
    /*
     * pbmp
     */
    pbmp = dnx_data_module_testing.property_methods.pbmp_get(unit);
    if (!BCM_PBMP_MEMBER(pbmp->val, 15))
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting - pbmp port 15 to be set\n");
    }

    port_pbmp = dnx_data_module_testing.property_methods.port_pbmp_get(unit, 2);
    if (!BCM_PBMP_MEMBER(port_pbmp->val, 19))
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting - port_pbmp port 19 to be set\n");
    }

    suffix_pbmp = dnx_data_module_testing.property_methods.suffix_pbmp_get(unit, 3);
    if (!BCM_PBMP_MEMBER(suffix_pbmp->val, 21))
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - Expecting - suffix_pbmp port 21 to be set\n");
    }

    SHR_CLI_EXIT_IF_ERR(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE),
                        "Error - Expecting - dnx data init done flag set.\n");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_data_test_support_man = {
    "Test DNX Data behavior on unsupported features"
};

static sh_sand_man_t dnx_data_test_types_man = {
    "Test DNX Data different types support"
};

static sh_sand_man_t dnx_data_test_property_man = {
    "Test DNX Data behavior on different properties"
};

sh_sand_cmd_t dnx_data_test_cmds[] = {
    {"input", dnx_data_test_support, NULL, NULL, &dnx_data_test_support_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"type", dnx_data_test_types, NULL, NULL, &dnx_data_test_types_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"property", dnx_data_test_property, NULL, NULL, &dnx_data_test_property_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
