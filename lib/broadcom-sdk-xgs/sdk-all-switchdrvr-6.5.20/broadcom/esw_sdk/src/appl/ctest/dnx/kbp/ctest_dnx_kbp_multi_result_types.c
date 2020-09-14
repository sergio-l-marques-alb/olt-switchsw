/*
 * ! \file mdb_diag.c Contains all of the KBP ctest commands
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
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
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#include "src/soc/dnx/dbal/dbal_internal.h"
#include "src/appl/diag/dnx/dbal/diag_dnx_dbal_internal.h"

#include "ctest_dnx_kbp.h"
#include "ctest_dnx_kbp_stif.h"
#include "ctest_dnx_kbp_combo.h"

/*
 * }
 */
/*
 * DEFINEs
 * {
 */

#if defined(INCLUDE_KBP)
extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];
#endif

/** Strings used for test options */
#define CTEST_DNX_KBP_SCENARIO                          "scenario"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_MEASURE        "Measure"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_READD              "Readd"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_PROFILE            "Profile"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_DEINIT_INIT        "Deinit_Init"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_UPDATE             "Update"

/** Strings used for KBP result types */
#define CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD         "Route_IPv4_FWD"
#define CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF         "Route_IPv4_RPF"
#define CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD         "Route_IPv6_FWD"
#define CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF         "Route_IPv6_RPF"
#define CTEST_DNX_KBP_RESULT_TYPE_HOST_IP4              "Host_IPv4"
#define CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6              "Host_IPv6"
#define CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP4              "IPMC_IPv4"
#define CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6              "IPMC_IPv6"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT32       "ACL_KEY_80_ACT_32"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT64       "ACL_KEY_80_ACT_64"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT128      "ACL_KEY_80_ACT_128"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT32      "ACL_KEY_160_ACT_32"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT64      "ACL_KEY_160_ACT_64"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT128     "ACL_KEY_160_ACT_128"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT32      "ACL_KEY_320_ACT_32"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT64      "ACL_KEY_320_ACT_64"
#define CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT128     "ACL_KEY_320_ACT_128"

/** Test cases for FWD combinations */
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S0    "Fwd_Scenario_0"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S1    "Fwd_Scenario_1"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S2    "Fwd_Scenario_2"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S3    "Fwd_Scenario_3"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S4    "Fwd_Scenario_4"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S5    "Fwd_Scenario_5"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S6    "Fwd_Scenario_6"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S7    "Fwd_Scenario_7"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S8    "Fwd_Scenario_8"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S9    "Fwd_Scenario_9"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S10   "Fwd_Scenario_10"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S11   "Fwd_Scenario_11"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S12   "Fwd_Scenario_12"

/** Test cases for ACL combinations */
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S0    "Acl_Scenario_0"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S1    "Acl_Scenario_1"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S2    "Acl_Scenario_2"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S3    "Acl_Scenario_3"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S4    "Acl_Scenario_4"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S5    "Acl_Scenario_5"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S6    "Acl_Scenario_6"

/** Test cases for FwdACL combinations */
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FwdACL_S0 "FwdAcl_Scenario_0"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FwdACL_S1 "FwdAcl_Scenario_1"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FwdACL_S2 "FwdAcl_Scenario_2"
#define CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FwdACL_S3 "FwdAcl_Scenario_3"

#define CTEST_DNX_KBP_MULTI_RESULT_TYPES_FILE_PATH      "kbp/ctest_dnx_kbp_multi_result_types.xml"

#define DNX_KBP_MULTI_RST_TYPE_ACL_QUAL_ATTACH_INFO_SET(_attach_info_) \
_attach_info_.input_type = bcmFieldInputTypeLayerFwd; _attach_info_.input_arg = 0; _attach_info_.offset = 0

/** Used to retrieve the root of the multi_result_types XML */
static shr_error_e
dnx_kbp_multi_result_types_root_get(
    int unit,
    void **root)
{
    SHR_FUNC_INIT_VARS(unit);

    *root =
        dbx_file_get_xml_top(unit, CTEST_DNX_KBP_MULTI_RESULT_TYPES_FILE_PATH, "CtestKbpMultiRstTypesRoot",
                             CONF_OPEN_PER_DEVICE);
    if (*root == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find KBP multiple result types xml file: %s\n",
                     CTEST_DNX_KBP_MULTI_RESULT_TYPES_FILE_PATH);
    }

exit:
    SHR_FUNC_EXIT;
}

/** Used to retrieve the node of the result type table */
static shr_error_e
dnx_kbp_multi_result_types_node_get(
    int unit,
    void *root,
    char *app,
    char *scenario,
    char *result_type,
    void **node)
{
    void *test_node = NULL;
    void *core_node = NULL;
    void *app_node = NULL;
    void *scenario_node = NULL;
    void *table_node = NULL;

    char *test_name = "Capacity";
    char *core_name = DNX_KBP_OP2_IS_SINGLE_PORT_MODE(unit) ? "SingleCore" : "DualCore";
    char *app_name = app;
    char *scenario_name = scenario;
    char *table_name = result_type;

    SHR_FUNC_INIT_VARS(unit);

    test_node = dbx_xml_child_get_first(root, test_name);
    if (test_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     test_name, CTEST_DNX_KBP_MULTI_RESULT_TYPES_FILE_PATH);
    }
    core_node = dbx_xml_child_get_first(test_node, core_name);
    if (core_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     core_name, CTEST_DNX_KBP_MULTI_RESULT_TYPES_FILE_PATH);
    }
    app_node = dbx_xml_child_get_first(core_node, app_name);
    if (app_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     app_name, CTEST_DNX_KBP_MULTI_RESULT_TYPES_FILE_PATH);
    }
    scenario_node = dbx_xml_child_get_first(app_node, scenario_name);
    if (scenario_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     scenario_name, CTEST_DNX_KBP_MULTI_RESULT_TYPES_FILE_PATH);
    }
    table_node = dbx_xml_child_get_first(scenario_node, table_name);
    if (table_node == NULL)
    {
        LOG_CLI((BSL_META("Test type %s doesn't exist in %s\n"), table_name, scenario));
    }
    *node = table_node;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_multi_result_types_value_get(
    int unit,
    char *app,
    char *scenario,
    char *type,
    char *item,
    int *value)
{
    void *root = NULL;
    void *node = NULL;
    int invalid_data = -1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_root_get(unit, &root));
    SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_node_get(unit, root, app, scenario, type, &node));

    if (node != NULL)
    {
        RHDATA_GET_INT_STOP(node, item, *value);
    }
    else
    {
        /*
         * This test type doesn't exist
         */
        value = &invalid_data;
    }

exit:
    dbx_xml_top_close(root);
    SHR_FUNC_EXIT;
}

/**********************************************************************************************************************/
/** Capacity with KBP FWD entries */
/**********************************************************************************************************************/

static shr_error_e
dnx_kbp_multi_result_types_capacity_fwd_ipmc(
    int unit,
    char *result_type,
    int nof_ent_to_add,
    int *nof_ent_added)
{
    int rv = 0;
    int iter = 0;
    int is_ipv6 = 0;
    bcm_ipmc_addr_t ipmc_addr = { 0 };

    bcm_ip6_t Ipv6Mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_ip6_t Ipv6Route =
        { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6))
    {
        is_ipv6 = 1;
    }

    /** Set the common route properties */
    bcm_ipmc_addr_t_init(&ipmc_addr);

    ipmc_addr.flags = BCM_IPMC_TCAM;
    ipmc_addr.priority = 0;
    ipmc_addr.vrf = 1;
    ipmc_addr.group = 0;
    if (is_ipv6)
    {
        ipmc_addr.flags |= BCM_IPMC_IP6;
        sal_memcpy(ipmc_addr.mc_ip6_addr, Ipv6Route, 16);
        sal_memcpy(ipmc_addr.mc_ip6_mask, Ipv6Mask, 16);
    }
    else
    {
        ipmc_addr.mc_ip_addr = 0xE0000000;
        ipmc_addr.mc_ip_mask = 0xFFFFFFFF;
    }

    /** Add specific number of entries */
    for (iter = 0; iter < nof_ent_to_add; iter++)
    {
        if (is_ipv6)
        {
            int current_route_index = 16;
            while (++ipmc_addr.mc_ip6_addr[--current_route_index] == 0);
        }
        else
        {
            ipmc_addr.mc_ip_addr++;
        }

        if (++ipmc_addr.group == 256 * 1024)
        {
            ipmc_addr.group = 1;
        }
        ipmc_addr.priority++;

        rv = bcm_ipmc_add(unit, &ipmc_addr);
        if (rv != BCM_E_NONE)
        {
            if (rv == BCM_E_FULL && nof_ent_to_add == 0x7FFFFFFF)
            {
                /** nof_ent_to_add 0x7FFFFFFF means adding KBP entries until table full */
                LOG_CLI((BSL_META("Table full after %d IPMC entries\n"), iter));
                break;
            }
            else
            {
                SHR_ERR_EXIT(rv, "Error, bcm_ipmc_add failed - IPMC number %d\n", iter);
            }
        }

        /** Progression print */
        if (iter % 16384 == 0)
        {
            LOG_CLI((BSL_META("%d IPMC entries\n"), iter));
        }
    }

    *nof_ent_added = iter;
    LOG_CLI((BSL_META("Done, total %d IPMC entries added\n"), iter));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Use to get/update/remove/re-add IPMC entries
 * update_flag: 0-Get, 1-Modify, 2-Delete, 3-readd
 */
static shr_error_e
dnx_kbp_multi_result_types_capacity_fwd_ipmc_update(
    int unit,
    char *result_type,
    int nof_ent_added,
    int update_flag)
{
    int rv = 0;
    int iter = 0;
    int is_ipv6 = 0;
    int update_start = 1000;
    int nof_ent_update = 1000;
    int result_exp = 0;
    int grp_value = 0;
    bcm_ipmc_addr_t ipmc_addr = { 0 };
    bcm_ipmc_addr_t ipmc_addr_get = { 0 };

    bcm_ip6_t Ipv6Mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_ip6_t Ipv6Route =
        { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

    /** Print entry operation */
    if (update_flag == 0)
    {
        LOG_CLI((BSL_META("Getting IPMC entries for %s ...\n"), result_type));
    }
    else if (update_flag == 1)
    {
        LOG_CLI((BSL_META("Modifying IPMC entries for %s ...\n"), result_type));
    }
    else if (update_flag == 2)
    {
        LOG_CLI((BSL_META("Removing IPMC entries for %s ...\n"), result_type));
    }
    else
    {
        LOG_CLI((BSL_META("Re-adding IPMC entries for %s ...\n"), result_type));
    }

    /*
     * Check updating range
     */
    if (update_start + nof_ent_update > nof_ent_added)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error, the added IPMC entries should be no less than %d\n",
                     update_start + nof_ent_update);
    }

    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6))
    {
        is_ipv6 = 1;
    }

    /** IPMC properties for setting */
    bcm_ipmc_addr_t_init(&ipmc_addr);
    ipmc_addr.flags = BCM_IPMC_TCAM;
    ipmc_addr.priority = 0;
    ipmc_addr.vrf = 1;
    ipmc_addr.group = 0;
    if (is_ipv6)
    {
        ipmc_addr.flags |= BCM_IPMC_IP6;
        sal_memcpy(ipmc_addr.mc_ip6_addr, Ipv6Route, 16);
        sal_memcpy(ipmc_addr.mc_ip6_mask, Ipv6Mask, 16);
    }
    else
    {
        ipmc_addr.mc_ip_addr = 0xE0000000;
        ipmc_addr.mc_ip_mask = 0xFFFFFFFF;
    }

    /** IPMC properties for getting */
    bcm_ipmc_addr_t_init(&ipmc_addr_get);
    ipmc_addr_get.flags = BCM_IPMC_TCAM;
    ipmc_addr_get.priority = 0;
    ipmc_addr_get.vrf = 1;
    ipmc_addr_get.group = 0;
    if (is_ipv6)
    {
        ipmc_addr_get.flags |= BCM_IPMC_IP6;
        sal_memcpy(ipmc_addr_get.mc_ip6_mask, Ipv6Mask, 16);
    }
    else
    {
        ipmc_addr_get.mc_ip_mask = 0xFFFFFFFF;
    }

    /** Update each entry and check */
    for (iter = 0; iter < update_start + nof_ent_update; iter++)
    {
        if (is_ipv6)
        {
            int current_route_index = 16;
            while (++ipmc_addr.mc_ip6_addr[--current_route_index] == 0);
        }
        else
        {
            ipmc_addr.mc_ip_addr++;
        }
        ipmc_addr.priority++;

        /** Compute the expected result */
        if (++grp_value == 256 * 1024)
        {
            grp_value = 1;
        }
        result_exp = grp_value;

        if (iter < update_start || update_flag == 1)
        {
            /** Not support IPMC entry updating */
            continue;
        }

        /** Remove the entry */
        if (update_flag == 2)
        {
            SHR_IF_ERR_EXIT(bcm_ipmc_remove(unit, &ipmc_addr));
        }
        if (update_flag == 2 && iter != update_start && iter != update_start + nof_ent_update - 1)
        {
           /** When testing kbp entry removal, only perform entry getting for the 1st entry and the 1ast entry */
            continue;
        }

        /** Re-add the entry */
        if (update_flag == 3)
        {
            ipmc_addr.group = grp_value;
            SHR_IF_ERR_EXIT(bcm_ipmc_add(unit, &ipmc_addr));
        }

        /** Get the entry and check */
        if (is_ipv6)
        {
            sal_memcpy(ipmc_addr_get.mc_ip6_addr, ipmc_addr.mc_ip6_addr, 16);
        }
        else
        {
            ipmc_addr_get.mc_ip_addr = ipmc_addr.mc_ip_addr;
        }
        rv = bcm_ipmc_find(unit, &ipmc_addr_get);
        if (update_flag == 2)
        {
            /*
             * Expect error as the entry has been removed
             */
            if (rv == BCM_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR: no error while getting removed entry %d\n", iter);
            }
        }
        else
        {
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "ERROR: fail in bcm_ipmc_find. Index %d\n", iter);
            }
            if (ipmc_addr_get.group != result_exp)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR: returned result %d doesn't equal to %d. Index %d\n",
                             ipmc_addr_get.group, result_exp, iter);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_multi_result_types_capacity_fwd_host(
    int unit,
    char *result_type,
    int nof_ent_to_add,
    int *nof_ent_added)
{
    int rv = 0;
    int iter = 0;
    int is_ipv6 = 0;
    bcm_l3_host_t host = { 0 };

    bcm_ip6_t Ipv6Addr =
        { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6))
    {
        is_ipv6 = 1;
    }

    /** Set the common host properties */
    bcm_l3_host_t_init(&host);
    host.l3a_vrf = 1;
    host.encap_id = 1;
    host.l3a_intf = 0;
    if (is_ipv6)
    {
        host.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(host.l3a_ip6_addr, Ipv6Addr, 16);
    }
    else
    {
        host.l3a_ip_addr = 0xB0000000;
    }

    /** Add specific number of entries */
    for (iter = 0; iter < nof_ent_to_add; iter++)
    {
        if (is_ipv6)
        {
            int current_host_index = 16;
            while (++host.l3a_ip6_addr[--current_host_index] == 0);
        }
        else
        {
            host.l3a_ip_addr++;
        }

        if (++host.l3a_intf == 256 * 1024)
        {
            host.l3a_intf = 1;
        }

        rv = bcm_l3_host_add(unit, &host);
        if (rv != BCM_E_NONE)
        {
            if (rv == BCM_E_FULL && nof_ent_to_add == 0x7FFFFFFF)
            {
                /** nof_ent_to_add 0x7FFFFFFF means adding KBP entries until table full */
                LOG_CLI((BSL_META("Table full after %d hosts\n"), iter));
                break;
            }
            else
            {
                SHR_ERR_EXIT(rv, "Error, bcm_l3_host_add failed - host number %d\n", iter);
            }
        }

        /** Progression print */
        if (iter % 131072 == 0)
        {
            LOG_CLI((BSL_META("%d hosts\n"), iter));
        }
    }

    *nof_ent_added = iter;
    LOG_CLI((BSL_META("Done, total %d hosts added\n"), iter));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Use to get/update/remove/re-add host entries
 * update_flag: 0-Get, 1-Modify, 2-Delete, 3-readd
 */
static shr_error_e
dnx_kbp_multi_result_types_capacity_fwd_host_update(
    int unit,
    char *result_type,
    int nof_ent_added,
    int update_flag)
{
    int rv = 0;
    int iter = 0;
    int is_ipv6 = 0;
    int update_start = 1000;
    int nof_ent_update = 1000;
    int result_exp = 0;
    int intf_value = 0;
    int intf_prefix = 0x20000000;
    bcm_l3_host_t host = { 0 };
    bcm_l3_host_t host_get = { 0 };

    bcm_ip6_t Ipv6Addr =
        { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

    /** Print entry operation */
    if (update_flag == 0)
    {
        LOG_CLI((BSL_META("Getting host entries for %s ...\n"), result_type));
    }
    else if (update_flag == 1)
    {
        LOG_CLI((BSL_META("Modifying host entries for %s ...\n"), result_type));
    }
    else if (update_flag == 2)
    {
        LOG_CLI((BSL_META("Removing host entries for %s ...\n"), result_type));
    }
    else
    {
        LOG_CLI((BSL_META("Re-adding host entries for %s ...\n"), result_type));
    }

    /** Check updating range */
    if (update_start + nof_ent_update > nof_ent_added)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error, the added host entries should be no less than %d\n",
                     update_start + nof_ent_update);
    }

    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6))
    {
        is_ipv6 = 1;
    }

    /** Host properties for setting */
    bcm_l3_host_t_init(&host);
    host.l3a_vrf = 1;
    host.encap_id = 1;
    host.l3a_intf = 0;
    if (is_ipv6)
    {
        host.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(host.l3a_ip6_addr, Ipv6Addr, 16);
    }
    else
    {
        host.l3a_ip_addr = 0xB0000000;
    }

    /** Host properties for getting */
    bcm_l3_host_t_init(&host_get);
    host_get.l3a_vrf = 1;
    host_get.encap_id = 1;
    host_get.l3a_intf = 0;
    if (is_ipv6)
    {
        host_get.l3a_flags |= BCM_L3_IP6;
    }

    /** Update each entry and check */
    for (iter = 0; iter < update_start + nof_ent_update; iter++)
    {
        if (is_ipv6)
        {
            int current_host_index = 16;
            while (++host.l3a_ip6_addr[--current_host_index] == 0);
        }
        else
        {
            host.l3a_ip_addr++;
        }

        /** Compute the expected result */
        if (++intf_value == 256 * 1024)
        {
            intf_value = 1;
        }
        result_exp = intf_value + intf_prefix;

        if (iter < update_start)
        {
            continue;
        }

        /** Modify the entry */
        if (update_flag == 1)
        {
            /** Increase the result by 1 */
            host.l3a_intf = intf_value + 1;
            if (host.l3a_intf == 256 * 1024)
            {
                host.l3a_intf = 1;
            }
            SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host));
            result_exp = host.l3a_intf + intf_prefix;
        }

        /** Remove the entry */
        if (update_flag == 2)
        {
            SHR_IF_ERR_EXIT(bcm_l3_host_delete(unit, &host));
        }
        if (update_flag == 2 && iter != update_start && iter != update_start + nof_ent_update - 1)
        {
           /** When testing kbp entry removal, only perform entry getting for the 1st entry and the 1ast entry */
            continue;
        }

        /** Re-add the entry */
        if (update_flag == 3)
        {
            host.l3a_intf = intf_value;
            SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host));
        }

        /** Get the host entry and check */
        if (is_ipv6)
        {
            sal_memcpy(host_get.l3a_ip6_addr, host.l3a_ip6_addr, 16);
        }
        else
        {
            host_get.l3a_ip_addr = host.l3a_ip_addr;
        }
        rv = bcm_l3_host_find(unit, &host_get);
        if (update_flag == 2)
        {
            /*
             * Expect error as the entry has been removed
             */
            if (rv == BCM_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR: no error while getting removed entry %d\n", iter);
            }
        }
        else
        {
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "ERROR: fail in bcm_l3_host_find. Index %d\n", iter);
            }
            if (host_get.l3a_intf != result_exp)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR: returned result %d doesn't equal to %d. Index %d\n",
                             host_get.l3a_intf, result_exp, iter);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_multi_result_types_capacity_fwd_route(
    int unit,
    char *result_type,
    int nof_ent_to_add,
    int *nof_ent_added)
{
    int rv = 0;
    int iter = 0;
    int split_rpf = 0;
    int is_ipv6 = 0;
    uint32 fwd_flags2 = 0;
    bcm_l3_route_t route = { 0 };

    bcm_ip6_t Ipv6Mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_ip6_t Ipv6Route =
        { 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

    /** Compute flags2 according to result type */
    split_rpf = dnx_data_elk.application.split_rpf_get(unit);
    if ((!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF)
         || !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF)) && !split_rpf)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "RPF entry is not supported when split_rpf disabled\n");
    }
    if ((!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF)
         || !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF)) && split_rpf)
    {
        fwd_flags2 = BCM_L3_FLAGS2_RPF_ONLY;
    }
    if ((!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD)
         || !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD)) && split_rpf)
    {
        fwd_flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }

    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD)
        || !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF))
    {
        is_ipv6 = 1;
    }

    /** Set the common route properties */
    bcm_l3_route_t_init(&route);
    route.l3a_flags2 |= fwd_flags2;
    route.l3a_vrf = 1;
    route.l3a_intf = 0;
    if (is_ipv6)
    {
        route.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(route.l3a_ip6_net, Ipv6Route, 16);
        sal_memcpy(route.l3a_ip6_mask, Ipv6Mask, 16);
        if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF))
        {
            /*
             * Use different prefix for RPF entries
             */
            route.l3a_ip6_net[0] = 0x40;
        }
    }
    else
    {
        route.l3a_subnet = 0;
        if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF))
        {
            /*
             * Use different prefix for RPF entries
             */
            route.l3a_subnet = 0xA0000000;
        }
        route.l3a_ip_mask = 0xFFFFFFFF;
    }

    /** Add specific number of entries */
    for (iter = 0; iter < nof_ent_to_add; iter++)
    {
        if (is_ipv6)
        {
            int current_route_index = 16;
            while (++route.l3a_ip6_net[--current_route_index] == 0);
        }
        else
        {
            route.l3a_subnet++;
        }

        if (++route.l3a_intf == 256 * 1024)
        {
            route.l3a_intf = 1;
        }

        rv = bcm_l3_route_add(unit, &route);
        if (rv != BCM_E_NONE)
        {
            if (rv == BCM_E_FULL && nof_ent_to_add == 0x7FFFFFFF)
            {
                /** nof_ent_to_add 0x7FFFFFFF means adding KBP entries until table full */
                LOG_CLI((BSL_META("Table full after %d routes\n"), iter));
                break;
            }
            else
            {
                SHR_ERR_EXIT(rv, "Error, bcm_l3_route_add failed - route number %d\n", iter);
            }
        }

        /** Progression print */
        if (iter % 131072 == 0)
        {
            LOG_CLI((BSL_META("%d routes\n"), iter));
        }
    }

    *nof_ent_added = iter;
    LOG_CLI((BSL_META("Done, total %d route entries added\n"), iter));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Use to get/update/remove/re-add route entries
 * update_flag: 0-Get, 1-Modify, 2-Delete, 3-readd
 */
static shr_error_e
dnx_kbp_multi_result_types_capacity_fwd_route_update(
    int unit,
    char *result_type,
    int nof_ent_added,
    int update_flag)
{
    int rv = 0;
    int iter = 0;
    int split_rpf = 0;
    int is_ipv6 = 0;
    uint32 fwd_flags2 = 0;
    int update_start = 1000;
    int nof_ent_update = 1000;
    int result_exp = 0;
    int intf_value = 0;
    int intf_prefix = 0x20000000;
    bcm_l3_route_t route = { 0 };
    bcm_l3_route_t route_get = { 0 };

    bcm_ip6_t Ipv6Mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_ip6_t Ipv6Route =
        { 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

    /** Print entry operation */
    if (update_flag == 0)
    {
        LOG_CLI((BSL_META("Getting route entries for %s ...\n"), result_type));
    }
    else if (update_flag == 1)
    {
        LOG_CLI((BSL_META("Modifying route entries for %s ...\n"), result_type));
    }
    else if (update_flag == 2)
    {
        LOG_CLI((BSL_META("Removing route entries for %s ...\n"), result_type));
    }
    else
    {
        LOG_CLI((BSL_META("Re-adding route entries for %s ...\n"), result_type));
    }

    /*
     * Check updating range
     */
    if (update_start + nof_ent_update > nof_ent_added)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error, the added route entries should be no less than %d\n",
                     update_start + nof_ent_update);
    }

    /** Compute flags2 according to result type */
    split_rpf = dnx_data_elk.application.split_rpf_get(unit);
    if ((!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF)
         || !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF)) && !split_rpf)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "RPF entry is not supported when split_rpf disabled\n");
    }
    if ((!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF)
         || !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF)) && split_rpf)
    {
        fwd_flags2 = BCM_L3_FLAGS2_RPF_ONLY;
    }
    if ((!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD)
         || !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD)) && split_rpf)
    {
        fwd_flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }

    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD)
        || !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF))
    {
        is_ipv6 = 1;
    }

    /** Route properties for setting */
    bcm_l3_route_t_init(&route);
    route.l3a_flags2 |= fwd_flags2;
    route.l3a_vrf = 1;
    route.l3a_intf = 0;
    if (is_ipv6)
    {
        route.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(route.l3a_ip6_net, Ipv6Route, 16);
        sal_memcpy(route.l3a_ip6_mask, Ipv6Mask, 16);
        if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF))
        {
            /*
             * Use different prefix for RPF entries
             */
            route.l3a_ip6_net[0] = 0x40;
        }
    }
    else
    {
        route.l3a_subnet = 0;
        if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF))
        {
            /*
             * Use different prefix for RPF entries
             */
            route.l3a_subnet = 0xA0000000;
        }
        route.l3a_ip_mask = 0xFFFFFFFF;
    }

    /** Route properties for getting */
    bcm_l3_route_t_init(&route_get);
    route_get.l3a_flags2 |= fwd_flags2;
    route_get.l3a_vrf = 1;
    route_get.l3a_intf = 0;
    if (is_ipv6)
    {
        route_get.l3a_flags |= BCM_L3_IP6;
        sal_memcpy(route_get.l3a_ip6_mask, Ipv6Mask, 16);
    }
    else
    {
        route_get.l3a_ip_mask = 0xFFFFFFFF;
    }

    /** Update each entry and check */
    for (iter = 0; iter < update_start + nof_ent_update; iter++)
    {
        if (is_ipv6)
        {
            int current_route_index = 16;
            while (++route.l3a_ip6_net[--current_route_index] == 0);
        }
        else
        {
            route.l3a_subnet++;
        }

        /** Compute the expected result */
        if (++intf_value == 256 * 1024)
        {
            intf_value = 1;
        }
        result_exp = intf_value + intf_prefix;

        if (iter < update_start)
        {
            continue;
        }

        /** Modify the entry */
        if (update_flag == 1)
        {
            /** Increase the result by 1 */
            route.l3a_intf = intf_value + 1;
            if (route.l3a_intf == 256 * 1024)
            {
                route.l3a_intf = 1;
            }
            SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &route));
            result_exp = route.l3a_intf + intf_prefix;
        }

        /** Remove the entry */
        if (update_flag == 2)
        {
            SHR_IF_ERR_EXIT(bcm_l3_route_delete(unit, &route));
        }
        if (update_flag == 2 && iter != update_start && iter != update_start + nof_ent_update - 1)
        {
           /** When testing kbp entry removal, only perform entry getting for the 1st entry and the 1ast entry */
            continue;
        }

        /** Re-add the entry */
        if (update_flag == 3)
        {
            route.l3a_intf = intf_value;
            SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &route));
        }

        /** Get the entry and check */
        if (is_ipv6)
        {
            sal_memcpy(route_get.l3a_ip6_net, route.l3a_ip6_net, 16);
        }
        else
        {
            route_get.l3a_subnet = route.l3a_subnet;
        }
        rv = bcm_l3_route_get(unit, &route_get);
        if (update_flag == 2)
        {
            /*
             * Expect error as the entry has been removed
             */
            if (rv == BCM_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR: no error while getting removed entry %d\n", iter);
            }
        }
        else
        {
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "ERROR: fail in bcm_l3_route_get. Index %d\n", iter);
            }
            if (route_get.l3a_intf != result_exp)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR: returned result %d doesn't equal to %d. Index %d\n",
                             route_get.l3a_intf, result_exp, iter);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_multi_result_types_capacity_fwd_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int invalid_data = -1;
    int index = 0;
    int val_get = 0;
    int seq_id = 0;
    int nof_ent_added = 0;
    int ent_opcode = 0;
    uint8 measure = FALSE;
    uint8 update_flag = FALSE;
    uint8 readd_flag = FALSE;
    uint8 deinit_init = FALSE;
    double capacity_tolerance = 0.05;

    char *app = "Forward";
    char *scenario = NULL;
    char *cfg_seqence = "Seq";
    char *field_add = "Add";
    char *field_cap = "Capacity";

    int max_result_types = 8;
    int nof_ent_to_add[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int ent_num_added[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int capacity_exp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    char *result_type = NULL;
    char *result_types[8] = {
        CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD,
        CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF,
        CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD,
        CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF,
        CTEST_DNX_KBP_RESULT_TYPE_HOST_IP4,
        CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6,
        CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP4,
        CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6
    };
    char *cfg_sequence[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

    SHR_FUNC_INIT_VARS(unit);

    /** Device check */
    if (SAL_BOOT_PLISIM || !dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Test is not supported without KBP\n");
    }
    if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "The testcase is applicable for only OP2 device\n");
    }

    /** Perform deinit-init for regression */
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_DEINIT_INIT, deinit_init);
    if (deinit_init)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_init_deinit(unit, NULL));
    }

    /** Perform device sync */
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    /** Get test scenario */
    SH_SAND_GET_STR(CTEST_DNX_KBP_SCENARIO, scenario);
    LOG_CLI((BSL_META("**********************************************************************\n")));
    LOG_CLI((BSL_META("* Testing KBP capacity with FWD entries. test_scenario %s\n"), scenario));
    LOG_CLI((BSL_META("**********************************************************************\n\n")));

    /** Get result types and their adding sequences from XML file */
    for (index = 0; index < max_result_types; index++)
    {
        /*
         * Get adding sequence of the result type
         */
        seq_id = invalid_data;
        SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get
                        (unit, app, scenario, result_types[index], cfg_seqence, &seq_id));

        /*
         * Get configuration of the result type. Includes:
         * 1. Number of entries to add (if it is 0x7FFFFFFF, means adding entries until table full)
         * 2. The expected number of added entries
         */
        if (seq_id != invalid_data)
        {
            /*
             * Store the adding sequence in array
             */
            cfg_sequence[seq_id] = result_types[index];

            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get
                            (unit, app, scenario, result_types[index], field_add, &val_get));
            nof_ent_to_add[seq_id] = val_get;

            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get
                            (unit, app, scenario, result_types[index], field_cap, &val_get));
            capacity_exp[seq_id] = val_get;
        }
    }

    /** Start to add entries for each result type according to its adding sequence */
    LOG_CLI((BSL_META("\n********** Stage I: Adding KBP FWD entries **********\n")));
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_MEASURE, measure);
    for (index = 0; index < max_result_types; index++)
    {
        result_type = cfg_sequence[index];
        if (cfg_sequence[index] == NULL)
        {
            continue;
        }
        LOG_CLI((BSL_META("\n%s: ------------------------------\n"), result_type));
        LOG_CLI((BSL_META("Adding %d FWD entries...\n"), nof_ent_to_add[index]));

        /*
         * Adding IPv4/IPv6 Route/RPF entries
         */
        if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD) ||
            !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF) ||
            !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD) ||
            !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_route
                            (unit, result_type, nof_ent_to_add[index], &nof_ent_added));
        }

        /*
         * Adding IPv4/IPv6 Host entries
         */
        if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP4) ||
            !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_host
                            (unit, result_type, nof_ent_to_add[index], &nof_ent_added));
        }

        /*
         * Adding IPMCv4/IPMCv6 entries
         */
        if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP4) ||
            !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_ipmc
                            (unit, result_type, nof_ent_to_add[index], &nof_ent_added));
        }

        ent_num_added[index] = nof_ent_added;

        /*
         * Compare the number of added entries against expected capacity
         */
        if (measure)
        {
            LOG_CLI((BSL_META("%d FWD entries added. Expected capacity is %d\n"), nof_ent_added, capacity_exp[index]));
            if ((double) nof_ent_added <= ((double) capacity_exp[index] * (1.0 + capacity_tolerance)) &&
                (double) nof_ent_added >= ((double) capacity_exp[index] * (1.0 - capacity_tolerance)))
            {
                LOG_CLI((BSL_META("Reached the expected capacity of %d within the expected %.2f%% tolerance (%d)\n\n"),
                         capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added));
            }
            else if ((double) nof_ent_added > ((double) capacity_exp[index] * (1.0 + capacity_tolerance)))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Reached higher capacity than the expected %d with %.2f%% tolerance (%d)\n",
                             capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Could not reach the expected capacity of %d with %.2f%% tolerance (%d)\n",
                             capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added);
            }
        }
        else
        {
            LOG_CLI((BSL_META("Measurements validation is not performed\n")));
        }
    }

    /** Start to get/modify/delete/re-add entries for each result type */
    LOG_CLI((BSL_META("\n********** Stage II: Updating KBP FWD entries **********\n")));
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_UPDATE, update_flag);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_READD, readd_flag);
    for (index = 0; index < max_result_types; index++)
    {
        result_type = cfg_sequence[index];
        if (cfg_sequence[index] == NULL || !update_flag)
        {
            continue;
        }
        LOG_CLI((BSL_META("\n%s: ------------------------------\n"), result_type));

        /*
         * Update IPv4/IPv6 Route/RPF/IPMC entries. ent_opcode:
         * 0. Get entries and check
         * 1. Modify entries and check
         * 2. Remove entries and check
         * 3. Rer-add entries and check
         */
        for (ent_opcode = 0; ent_opcode < 4; ent_opcode++)
        {
            /*
             * Don't add deleted KBP entries back if readd_flag unset 
             */
            if (!readd_flag && ent_opcode == 3)
            {
                continue;
            }

            /*
             * Updating IPv4/IPv6 route entries
             */
            if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF))
            {
                SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_route_update
                                (unit, result_type, ent_num_added[index], ent_opcode));
            }

            /*
             * Updating IPv4/IPv6 Host entries
             */
            if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP4) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6))
            {
                SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_host_update
                                (unit, result_type, ent_num_added[index], ent_opcode));
            }

            /*
             * Updating IPMCv4/IPMCv6 entries
             */
            if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP4) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6))
            {
                SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_ipmc_update
                                (unit, result_type, ent_num_added[index], ent_opcode));
            }
        }
    }
    if (!update_flag)
    {
        LOG_CLI((BSL_META("Entry updating is not performed\n\n")));
    }

    LOG_CLI((BSL_META("**********************************************************************\n")));
    LOG_CLI((BSL_META("* FWD capacity test done!\n")));
    LOG_CLI((BSL_META("**********************************************************************\n\n")));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_kbp_multi_result_types_capacity_fwd_options[] = {
    {CTEST_DNX_KBP_SCENARIO, SAL_FIELD_TYPE_STR, "Test scenario", "Scenario0"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_MEASURE, SAL_FIELD_TYPE_BOOL, "Measure indication", "YES"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_READD, SAL_FIELD_TYPE_BOOL, "Readd indication", "NO"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_DEINIT_INIT, SAL_FIELD_TYPE_BOOL, "Deinit-Init indication", "YES"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_UPDATE, SAL_FIELD_TYPE_BOOL, "Update indication", "YES"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_PROFILE, SAL_FIELD_TYPE_UINT32, "User Profile", "0"}
    ,
    {NULL}
};

sh_sand_invoke_t dnx_kbp_multi_result_types_capacity_fwd_tests[] = {
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S0, CTEST_DNX_KBP_SCENARIO "=Scenario0"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S1, CTEST_DNX_KBP_SCENARIO "=Scenario1"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S2, CTEST_DNX_KBP_SCENARIO "=Scenario2"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S3, CTEST_DNX_KBP_SCENARIO "=Scenario3"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S4, CTEST_DNX_KBP_SCENARIO "=Scenario4"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S5, CTEST_DNX_KBP_SCENARIO "=Scenario5"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S6, CTEST_DNX_KBP_SCENARIO "=Scenario6"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S7, CTEST_DNX_KBP_SCENARIO "=Scenario7"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S8, CTEST_DNX_KBP_SCENARIO "=Scenario8"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S9, CTEST_DNX_KBP_SCENARIO "=Scenario9"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S10, CTEST_DNX_KBP_SCENARIO "=Scenario10"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FWD_S11, CTEST_DNX_KBP_SCENARIO "=Scenario11"}
    ,
    
    {NULL}
};

sh_sand_man_t dnx_kbp_multi_result_types_capacity_fwd_man = {
    "Add IPv4/IPv6/IPv4MC/IPv6MC entries(FWD/RPF) and compare against expected capacity",
    "Add IPv4/IPv6/IPv4MC/IPv6MC entries(FWD/RPF) and compare against expected capacity",
    "",
    ""
};

/**********************************************************************************************************************/
/** Capacity with KBP ACL entries */
/**********************************************************************************************************************/

static shr_error_e
dnx_kbp_multi_result_types_capacity_acl_user_opcode_create(
    int unit,
    bcm_field_context_t * user_opcode)
{
    bcm_field_context_info_t context_info;
    bcm_field_AppType_t src_opcode = bcmFieldAppTypeTrafficManagement;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);
    context_info.cascaded_from = src_opcode;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageExternal, &context_info, user_opcode));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_multi_result_types_capacity_acl_quals_by_key_size_set(
    int unit,
    uint32 key_size,
    bcm_field_qualify_t * qual_list,
    bcm_field_qualify_attach_info_t * attach_info_list,
    int *nof_quals)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_list, _SHR_E_INTERNAL, "qual_list");
    SHR_NULL_CHECK(attach_info_list, _SHR_E_INTERNAL, "attach_info_list");
    SHR_NULL_CHECK(nof_quals, _SHR_E_INTERNAL, "nof_quals");

    /*
     * Build qualifier info according to key size. Add additional qualifiers to reach the key size
     */
    *nof_quals = 1;
    qual_list[0] = bcmFieldQualifySrcMac;
    DNX_KBP_MULTI_RST_TYPE_ACL_QUAL_ATTACH_INFO_SET(attach_info_list[0]);
    if (key_size > 80)
    {
        (*nof_quals)++;
        qual_list[1] = bcmFieldQualifyDstMac;
        DNX_KBP_MULTI_RST_TYPE_ACL_QUAL_ATTACH_INFO_SET(attach_info_list[1]);
    }
    if (key_size > 160)
    {
        (*nof_quals)++;
        qual_list[2] = bcmFieldQualifySrcIp6;
        DNX_KBP_MULTI_RST_TYPE_ACL_QUAL_ATTACH_INFO_SET(attach_info_list[2]);
    }
    if (key_size > 320)
    {
        (*nof_quals)++;
        qual_list[3] = bcmFieldQualifyDstIp6;
        DNX_KBP_MULTI_RST_TYPE_ACL_QUAL_ATTACH_INFO_SET(attach_info_list[3]);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_multi_result_types_capacity_acl_uda_create(
    int unit,
    uint32 res_length,
    bcm_field_action_t * action_list,
    int *nof_actions)
{
    int action_index = 0;
    int size_count = res_length;
    int action_size = 32;
    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_list, _SHR_E_INTERNAL, "action_list");
    SHR_NULL_CHECK(nof_actions, _SHR_E_INTERNAL, "nof_actions");

    *nof_actions = 0;
    while (size_count - action_size >= 0)
    {
        size_count -= action_size;
        bcm_field_action_info_t_init(&action_info);
        action_info.stage = bcmFieldStageExternal;
        action_info.action_type = bcmFieldActionVoid;
        action_info.size = action_size;
        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0 /** flags */ , &action_info, &action_list[action_index++]));
        (*nof_actions)++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_muti_result_types_capacity_acl_config(
    int unit,
    char *result_type,
    bcm_field_qualify_t * qual_list,
    int *nof_quals,
    bcm_field_action_t * action_list,
    int *nof_actions,
    bcm_field_group_t * fg_id)
{
    uint32 key_size = 0;
    uint32 res_length = 0;
    int index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_qualify_attach_info_t attach_info_list[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    bcm_field_context_t context_id;
    uint32 res_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_list, _SHR_E_INTERNAL, "qual_list");
    SHR_NULL_CHECK(nof_quals, _SHR_E_INTERNAL, "nof_quals");
    SHR_NULL_CHECK(action_list, _SHR_E_INTERNAL, "action_list");
    SHR_NULL_CHECK(nof_actions, _SHR_E_INTERNAL, "nof_actions");
    SHR_NULL_CHECK(fg_id, _SHR_E_INTERNAL, "fg_id");

    /** Get key size according to result type */
    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT32) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT64) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT128))
    {
        key_size = 80;
    }
    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT32) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT64) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT128))
    {
        key_size = 160;
    }
    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT32) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT64) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT128))
    {
        key_size = 320;
    }

    /** Get action size according to result type */
    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT128) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT128) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT128))
    {
        res_length = 128;
    }
    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT64) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT64) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT64))
    {
        res_length = 64;
    }
    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT32) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT32) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT32))
    {
        res_length = 32;
    }
    res_offset = 248 - res_length;

    LOG_CLI((BSL_META("%s: Creating KBP ACL FG - Key %d, Action %d\n"), result_type, key_size, res_length));

    /** Validate the key size */
    
    if ((key_size != 80) && (key_size != 160) && (key_size != 320))
    {
        LOG_CLI((BSL_META("Invalid key size %d. Should be 80/160/320\n"), key_size));
        return BCM_E_FAIL;
    }
    /** Validate the key size */
    if ((res_length != 32) && (res_length != 64) && (res_length != 128))
    {
        LOG_CLI((BSL_META("Invalid result length %d. Should be 32/64/128\n"), res_length));
        return BCM_E_FAIL;
    }

    /** Set the qualifiers based on the tested key size */
    SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_acl_quals_by_key_size_set
                    (unit, key_size, qual_list, attach_info_list, nof_quals));
    /** Create user defined void actions according to the tested result length */
    SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_acl_uda_create(unit, res_length, action_list, nof_actions));

    /*
     * Configure the field group
     */
    bcm_field_group_info_t_init(&fg_info);
    sal_snprintf((char *) fg_info.name, BCM_FIELD_MAX_SHORT_NAME_LEN - 1, "EXT_K%d_R%d", key_size, res_length);
    fg_info.stage = bcmFieldStageExternal;
    fg_info.fg_type = bcmFieldGroupTypeExternalTcam;
    fg_info.nof_quals = *nof_quals;
    fg_info.nof_actions = *nof_actions;
    /*
     * Add qualifiers to the field group
     */
    for (index = 0; index < *nof_quals; index++)
    {
        fg_info.qual_types[index] = qual_list[index];
    }
    /*
     * Add actions to the field group
     */
    for (index = 0; index < *nof_actions; index++)
    {
        fg_info.action_types[index] = action_list[index];
        fg_info.action_with_valid_bit[index] = FALSE;
    }

    /** Create the KBP ACL field group */
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, fg_id));

    /*
     * Building FG attach information structure
     */
    bcm_field_group_attach_info_t_init(&group_attach_info);
    group_attach_info.key_info.nof_quals = *nof_quals;
    group_attach_info.payload_info.nof_actions = *nof_actions;
    group_attach_info.payload_info.payload_id = 0;
    group_attach_info.payload_info.payload_offset = res_offset;

    /*
     * Add qualifiers and relevant attach info into the FG attach information structure
     */
    for (index = 0; index < *nof_quals; index++)
    {
        group_attach_info.key_info.qual_types[index] = qual_list[index];
        group_attach_info.key_info.qual_info[index].input_type = attach_info_list[index].input_type;
        group_attach_info.key_info.qual_info[index].input_arg = attach_info_list[index].input_arg;
        group_attach_info.key_info.qual_info[index].offset = attach_info_list[index].offset;
    }

    /*
     * Add actions into the FG attach information structure
     */
    for (index = 0; index < *nof_actions; index++)
    {
        group_attach_info.payload_info.action_types[index] = action_list[index];
    }

    /** Create user opcode */
    SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_acl_user_opcode_create(unit, &context_id));

    /** Call the attach API */
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0 /** flags */ , *fg_id, context_id, &group_attach_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_muti_result_types_capacity_acl_config_get(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_qualify_t * qual_list,
    int *nof_quals,
    bcm_field_action_t * action_list,
    int *nof_actions)
{

    int index;
    bcm_field_group_info_t fg_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Get field group info */
    bcm_field_group_info_t_init(&fg_info);
    SHR_IF_ERR_EXIT(bcm_field_group_info_get(unit, fg_id, &fg_info));

    /** return the number of qualifiers and actions */
    *nof_quals = fg_info.nof_quals;
    *nof_actions = fg_info.nof_actions;

    /** Return qualifier list */
    for (index = 0; index < *nof_quals; index++)
    {
        qual_list[index] = fg_info.qual_types[index];
    }

    /** Return action list */
    for (index = 0; index < *nof_actions; index++)
    {
        action_list[index] = fg_info.action_types[index];
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_muti_result_types_capacity_acl_entries(
    int unit,
    bcm_field_qualify_t * qual_list,
    int nof_quals,
    bcm_field_action_t * action_list,
    int nof_actions,
    bcm_field_group_t fg_id,
    int nof_ent_to_add,
    int *nof_ent_added,
    bcm_field_entry_t * entry_id_start)
{
    int rv = 0;
    int iter = 0;
    int index = 0;

    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize kbp_entry_info and set common properties
     */
    bcm_field_entry_info_t_init(&entry_info);
    for (index = 0; index < nof_quals; index++)
    {
        entry_info.entry_qual[index].type = qual_list[index];
        entry_info.entry_qual[index].value[0] = 0;
        entry_info.entry_qual[index].mask[0] = 0xFFFFFFFF;
    }
    entry_info.nof_entry_quals = nof_quals;
    for (index = 0; index < nof_actions; index++)
    {
        entry_info.entry_action[index].type = action_list[index];
        entry_info.entry_action[index].value[0] = 0;
    }
    entry_info.nof_entry_actions = nof_actions;

    /** Add specific number of entries */
    for (iter = 0; iter < nof_ent_to_add; iter++)
    {
        for (index = 0; index < nof_quals; index++)
        {
            entry_info.entry_qual[index].value[0]++;
        }

        for (index = 0; index < nof_actions; index++)
        {
            entry_info.entry_action[index].value[0]++;
        }

        rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id);
        if (rv != BCM_E_NONE)
        {
            if (rv == BCM_E_FULL && nof_ent_to_add == 0x7FFFFFFF)
            {
                /** nof_ent_to_add 0x7FFFFFFF means adding KBP entries until table full */
                LOG_CLI((BSL_META("Table full after %d ACL entries\n"), iter));
                break;
            }
            else
            {
                SHR_ERR_EXIT(rv, "Error, bcm_field_entry_add failed - ACL entry number %d\n", iter);
            }
        }

        /** Return the 1st entry id */
        if (iter == 0)
        {
            *entry_id_start = entry_id;
        }

        /** Progression print */
        if (iter % 16384 == 0)
        {
            LOG_CLI((BSL_META("%d ACL entries\n"), iter));
        }
    }
    *nof_ent_added = iter;
    LOG_CLI((BSL_META("Done, total %d ACL entries added\n"), iter));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Use to get/update/remove/re-add ACL entries
 * update_flag: 0-Get, 1-Modify, 2-Delete, 3-readd
 */
static shr_error_e
dnx_kbp_muti_result_types_capacity_acl_entry_update(
    int unit,
    bcm_field_qualify_t * qual_list,
    int nof_quals,
    bcm_field_action_t * action_list,
    int nof_actions,
    bcm_field_group_t fg_id,
    bcm_field_entry_t entry_id_start,
    int nof_ent_added,
    int update_flag)
{
    int rv = 0;
    int iter = 0;
    int index = 0;
    int update_start = 1000;
    int nof_ent_update = 1000;
    int act_exp = 0;
    int act_value = 0;
    bcm_field_entry_info_t entry_info_get;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Print entry operation */
    if (update_flag == 0)
    {
        LOG_CLI((BSL_META("Getting ACL entries in FG %d ...\n"), fg_id));
    }
    else if (update_flag == 1)
    {
        LOG_CLI((BSL_META("Modifying ACL entries in FG %d ...\n"), fg_id));
    }
    else if (update_flag == 2)
    {
        LOG_CLI((BSL_META("Removing ACL entries in FG %d ...\n"), fg_id));
    }
    else
    {
        LOG_CLI((BSL_META("Re-adding ACL entries in FG %d ...\n"), fg_id));
    }

    /** Check updating range */
    if (update_start + nof_ent_update > nof_ent_added)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error, the added ACL entries should be no less than %d\n",
                     update_start + nof_ent_update);
    }

    /** Initialize kbp_entry_info for setting */
    bcm_field_entry_info_t_init(&entry_info);
    for (index = 0; index < nof_quals; index++)
    {
        entry_info.entry_qual[index].type = qual_list[index];
        entry_info.entry_qual[index].value[0] = 0;
        entry_info.entry_qual[index].mask[0] = 0xFFFFFFFF;
    }
    entry_info.nof_entry_quals = nof_quals;
    for (index = 0; index < nof_actions; index++)
    {
        entry_info.entry_action[index].type = action_list[index];
        entry_info.entry_action[index].value[0] = 0;
    }
    entry_info.nof_entry_actions = nof_actions;

    /** Update each entry and check */
    for (iter = 0; iter < update_start + nof_ent_update; iter++)
    {
        entry_id = entry_id_start + iter;

        for (index = 0; index < nof_quals; index++)
        {
            entry_info.entry_qual[index].value[0]++;
        }

        /** Compute the expected action value */
        act_exp = ++act_value;

        if (iter < update_start)
        {
            continue;
        }

        /** Modify the entry */
        if (update_flag == 1)
        {
            /** Increase the result by 1 */
            for (index = 0; index < nof_actions; index++)
            {
                entry_info.entry_action[index].value[0] = act_value + 1;
            }
            SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, BCM_FIELD_FLAG_UPDATE, fg_id, &entry_info, &entry_id));
            act_exp += 1;
        }

        /** Remove the entry */
        if (update_flag == 2)
        {
            SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, fg_id, NULL, entry_id));
        }
        if (update_flag == 2 && iter != update_start && iter != update_start + nof_ent_update - 1)
        {
           /** When testing kbp entry removal, only perform entry getting for the 1st entry and the 1ast entry */
            continue;
        }

        /** Re-add the entry */
        if (update_flag == 3)
        {
            for (index = 0; index < nof_actions; index++)
            {
                entry_info.entry_action[index].value[0] = act_value;
            }
            SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id));
        }

        /** Get entry and check */
        rv = bcm_field_entry_info_get(unit, fg_id, entry_id, &entry_info_get);
        if (update_flag == 2)
        {
            /*
             * Expect error as the entry has been removed
             */
            if (rv == BCM_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR: no error while getting removed entry %d\n", iter);
            }
        }
        else
        {
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "ERROR: fail in bcm_field_entry_info_get. Index %d\n", iter);
            }
            for (index = 0; index < nof_quals; index++)
            {
                if (entry_info_get.entry_qual[index].value[0] != entry_info.entry_qual[index].value[0])
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Entry %d error: Qualifier(%d) value %d doesn't equal to %d\n", iter,
                                 index, entry_info_get.entry_qual[index].value[0],
                                 entry_info.entry_qual[index].value[0]);
                }
            }
            for (index = 0; index < nof_actions; index++)
            {
                if (entry_info_get.entry_action[index].value[0] != act_exp)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Entry %d error: Action(%d) value %d doesn't equal to %d\n", iter, index,
                                 entry_info_get.entry_action[index].value[0], act_exp);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_multi_result_types_capacity_acl_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int invalid_data = -1;
    int index = 0;
    int val_get = 0;
    int seq_id = 0;
    int nof_ent_added = 0;
    int ent_opcode = 0;
    uint8 measure = FALSE;
    uint8 update_flag = FALSE;
    uint8 readd_flag = FALSE;
    uint8 deinit_init = FALSE;
    double capacity_tolerance = 0.05;

    char *app = "ACL";
    char *scenario = NULL;
    char *cfg_seqence = "Seq";
    char *field_add = "Add";
    char *field_cap = "Capacity";
    int max_result_types = 9;
    int nof_ent_to_add[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int ent_num_added[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int capacity_exp[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    char *result_type = NULL;
    char *result_types[9] = {
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT32,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT64,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT128,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT32,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT64,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT128,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT32,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT64,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT128
    };
    char *cfg_sequence[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    bcm_field_entry_t entry_id_first;
    bcm_field_entry_t entry_id_start[9];
    bcm_field_group_t fg_ids[9] = { 0 };
    bcm_field_group_t fg_id;
    bcm_field_qualify_t qual_list[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP] = { 0 };
    int nof_quals = 0;
    bcm_field_action_t action_list[BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP] = { 0 };
    int nof_actions = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Device check */
    if (SAL_BOOT_PLISIM || !dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Test is not supported without KBP\n");
    }
    if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "The testcase is applicable for only OP2 device\n");
    }

    /** Perform deinit-init for regression */
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_DEINIT_INIT, deinit_init);
    if (deinit_init)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_init_deinit(unit, NULL));
    }

    /** Get test scenario */
    SH_SAND_GET_STR(CTEST_DNX_KBP_SCENARIO, scenario);
    LOG_CLI((BSL_META("**********************************************************************\n")));
    LOG_CLI((BSL_META("* Testing KBP capacity with ACL entries. test_scenario %s\n"), scenario));
    LOG_CLI((BSL_META("**********************************************************************\n\n")));

    /** Get result types and their adding sequences from XML file */
    for (index = 0; index < max_result_types; index++)
    {
        /** Get adding sequence of the result type */
        seq_id = invalid_data;
        SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get
                        (unit, app, scenario, result_types[index], cfg_seqence, &seq_id));

        /*
         * Get configuration of the result type. Includes:
         * 1. Number of entries to add (if it is 0x7FFFFFFF, means adding entries until table full)
         * 2. The expected number of added entries
         */
        if (seq_id != invalid_data)
        {
            /** Store the adding sequence in array */
            cfg_sequence[seq_id] = result_types[index];

            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get
                            (unit, app, scenario, result_types[index], field_add, &val_get));
            nof_ent_to_add[seq_id] = val_get;

            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get
                            (unit, app, scenario, result_types[index], field_cap, &val_get));
            capacity_exp[seq_id] = val_get;
        }
    }

    /** Start to create KBP ACL FGs according to adding sequence */
    LOG_CLI((BSL_META("********** Stage I: Creating ELK ACL FGs **********\n")));
    for (index = 0; index < max_result_types; index++)
    {
        result_type = cfg_sequence[index];
        if (result_type == NULL)
        {
            continue;
        }
        /*
         * Create KBP ACL FG
         */
        SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_config
                        (unit, result_type, qual_list, &nof_quals, action_list, &nof_actions, &fg_id));
        fg_ids[index] = fg_id;
        LOG_CLI((BSL_META("%s: FG creation done. ID = %d\n"), result_type, fg_ids[index]));
    }
    /** Perform sync */
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    /** Start to add entries for each result type */
    LOG_CLI((BSL_META("\n********** Stage II: Adding ELK ACL entries **********\n")));
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_MEASURE, measure);
    for (index = 0; index < max_result_types; index++)
    {
        result_type = cfg_sequence[index];
        fg_id = fg_ids[index];
        if (result_type == NULL)
        {
            continue;
        }
        LOG_CLI((BSL_META("\n%s: ------------------------------\n"), result_type));
        LOG_CLI((BSL_META("Adding %d ACL entries to FG %d ...\n"), nof_ent_to_add[index], fg_id));

        /** Get qualifier info and action info from the FG */
        SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_config_get
                        (unit, fg_id, qual_list, &nof_quals, action_list, &nof_actions));

        /** Add specific number of ACL entries */
        SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_entries
                        (unit, qual_list, nof_quals, action_list, nof_actions, fg_id, nof_ent_to_add[index],
                         &nof_ent_added, &entry_id_first));
        ent_num_added[index] = nof_ent_added;
        entry_id_start[index] = entry_id_first;

        if (measure)
        {
            LOG_CLI((BSL_META("%d ACL entries added. Expected capacity is %d\n"), nof_ent_added, capacity_exp[index]));
            if ((double) nof_ent_added <= ((double) capacity_exp[index] * (1.0 + capacity_tolerance)) &&
                (double) nof_ent_added >= ((double) capacity_exp[index] * (1.0 - capacity_tolerance)))
            {
                LOG_CLI((BSL_META("Reached the expected capacity of %d within the expected %.2f%% tolerance (%d)\n"),
                         capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added));
            }
            else if ((double) nof_ent_added > ((double) capacity_exp[index] * (1.0 + capacity_tolerance)))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Reached higher capacity than the expected %d with %.2f%% tolerance (%d)\n",
                             capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Could not reach the expected capacity of %d with %.2f%% tolerance (%d)\n",
                             capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added);
            }
        }
        else
        {
            LOG_CLI((BSL_META("Measurements validation is not performed\n")));
        }
    }

    /** Start to update entries for each result type */
    LOG_CLI((BSL_META("\n********** Stage III: Updating ACL entries **********\n")));
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_UPDATE, update_flag);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_READD, readd_flag);
    for (index = 0; index < max_result_types; index++)
    {
        result_type = cfg_sequence[index];
        fg_id = fg_ids[index];
        if (result_type == NULL || !update_flag)
        {
            continue;
        }
        LOG_CLI((BSL_META("\n%s: ------------------------------\n"), result_type));

        /** Get qualifier info and action info from the FG */
        SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_config_get
                        (unit, fg_id, qual_list, &nof_quals, action_list, &nof_actions));

        /*
         * Update ACL entries. ent_opcode:
         * 0 - Get entries and check
         * 1 - Modify entries and check
         * 2 - Remove entries and check
         * 3 - Re-add entries and check
         */
        for (ent_opcode = 0; ent_opcode < 4; ent_opcode++)
        {
            /*
             * Don't add deleted KBP entries back if readd_flag unset 
             */
            if (!readd_flag && ent_opcode == 3)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_entry_update
                            (unit, qual_list, nof_quals, action_list, nof_actions, fg_id, entry_id_start[index],
                             ent_num_added[index], ent_opcode));
        }
    }
    if (!update_flag)
    {
        LOG_CLI((BSL_META("Entry updating is not performed\n\n")));
    }

    LOG_CLI((BSL_META("**********************************************************************\n")));
    LOG_CLI((BSL_META("* ACL capacity test done!\n")));
    LOG_CLI((BSL_META("**********************************************************************\n\n")));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_kbp_multi_result_types_capacity_acl_options[] = {
    {CTEST_DNX_KBP_SCENARIO, SAL_FIELD_TYPE_STR, "Test scenario", "Scenario0"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_MEASURE, SAL_FIELD_TYPE_BOOL, "Measure indication", "YES"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_READD, SAL_FIELD_TYPE_BOOL, "Readd indication", "NO"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_UPDATE, SAL_FIELD_TYPE_BOOL, "Update indication", "YES"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_DEINIT_INIT, SAL_FIELD_TYPE_BOOL, "Deinit-Init indication", "YES"}
    ,
    {NULL}
};

sh_sand_invoke_t dnx_kbp_multi_result_types_capacity_acl_tests[] = {
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S0, CTEST_DNX_KBP_SCENARIO "=Scenario0"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S1, CTEST_DNX_KBP_SCENARIO "=Scenario1"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S2, CTEST_DNX_KBP_SCENARIO "=Scenario2"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S3, CTEST_DNX_KBP_SCENARIO "=Scenario3"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S4, CTEST_DNX_KBP_SCENARIO "=Scenario4"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S5, CTEST_DNX_KBP_SCENARIO "=Scenario5"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_ACL_S6, CTEST_DNX_KBP_SCENARIO "=Scenario6"}
    ,
    {NULL}
};

sh_sand_man_t dnx_kbp_multi_result_types_capacity_acl_man = {
    "Add KBP ACL entries with different key&action sizes and compare against expected capacity",
    "Add KBP ACL entries with different key&action sizes and compare against expected capacity",
    "",
    ""
};

/**********************************************************************************************************************/
/** Capacity with KBP FWD and ACL entries */
/**********************************************************************************************************************/

/** Check if the specific result type belongs to KBP ACL or not */
static shr_error_e
dnx_kbp_muti_result_types_capacity_app_type_check(
    int unit,
    char *result_type,
    int *is_kbp_acl)
{
    int kbp_acl_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(result_type, _SHR_E_INTERNAL, "result_type");

    if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT32) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT64) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT128) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT32) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT64) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT128) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT32) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT64) ||
        !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT128))
    {
        kbp_acl_flag = 1;
    }

    *is_kbp_acl = kbp_acl_flag;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_multi_result_types_capacity_fwd_acl_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int invalid_data = -1;
    int index = 0;
    int val_get = 0;
    int seq_id = 0;
    int nof_ent_added = 0;
    int ent_opcode = 0;
    int is_kbp_acl = 0;
    uint8 measure = FALSE;
    uint8 update_flag = FALSE;
    uint8 readd_flag = FALSE;
    uint8 deinit_init = FALSE;
    double capacity_tolerance = 0.05;

    char *app = "FwdACL";
    char *scenario = NULL;
    char *cfg_seqence = "Seq";
    char *field_add = "Add";
    char *field_cap = "Capacity";
    int max_result_types = 17;
    int nof_ent_to_add[17] = { 0 };
    int ent_num_added[17] = { 0 };
    int capacity_exp[17] = { 0 };
    char *result_type = NULL;
    char *result_types[17] = {
        CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD,
        CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF,
        CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD,
        CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF,
        CTEST_DNX_KBP_RESULT_TYPE_HOST_IP4,
        CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6,
        CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP4,
        CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT32,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT64,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY80_ACT128,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT32,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT64,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY160_ACT128,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT32,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT64,
        CTEST_DNX_KBP_RESULT_TYPE_ACL_KEY320_ACT128
    };
    char *cfg_sequence[17] = { NULL };
    bcm_field_entry_t entry_id_first;
    bcm_field_entry_t entry_id_start[17];
    bcm_field_group_t fg_ids[17] = { 0 };
    bcm_field_group_t fg_id;
    bcm_field_qualify_t qual_list[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP] = { 0 };
    int nof_quals = 0;
    bcm_field_action_t action_list[BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP] = { 0 };
    int nof_actions = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Device check */
    if (SAL_BOOT_PLISIM || !dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Test is not supported without KBP\n");
    }
    if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "The testcase is applicable for only OP2 device\n");
    }

    /** Perform deinit-init for regression */
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_DEINIT_INIT, deinit_init);
    if (deinit_init)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_init_deinit(unit, NULL));
    }

    /** Get test scenario */
    SH_SAND_GET_STR(CTEST_DNX_KBP_SCENARIO, scenario);
    LOG_CLI((BSL_META("**********************************************************************\n")));
    LOG_CLI((BSL_META("* Testing KBP capacity with ACL&FWD entries. test_scenario %s\n"), scenario));
    LOG_CLI((BSL_META("**********************************************************************\n\n")));

    /** Get result types and their adding sequences from XML file */
    for (index = 0; index < max_result_types; index++)
    {
        result_type = result_types[index];

        /** Get adding sequence of the result type */
        seq_id = invalid_data;
        SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get(unit, app, scenario, result_type, cfg_seqence, &seq_id));

        /*
         * Get configuration of the result type. Includes:
         * 1. Number of entries to add (if it is 0x7FFFFFFF, means adding entries until table full)
         * 2. The expected number of added entries
         */
        if (seq_id != invalid_data)
        {
            /** Store the adding sequence in array */
            cfg_sequence[seq_id] = result_type;

            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get
                            (unit, app, scenario, result_type, field_add, &val_get));
            nof_ent_to_add[seq_id] = val_get;

            SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_value_get
                            (unit, app, scenario, result_type, field_cap, &val_get));
            capacity_exp[seq_id] = val_get;
        }
    }

    /** Start to create KBP ACL FGs according to adding sequence */
    LOG_CLI((BSL_META("********** Stage I: Creating ELK ACL FGs **********\n")));
    for (index = 0; index < max_result_types; index++)
    {
        result_type = cfg_sequence[index];
        if (result_type == NULL)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_app_type_check(unit, result_type, &is_kbp_acl));
        if (!is_kbp_acl)
        {
            continue;
        }

        /** Create KBP ACL FG */
        SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_config
                        (unit, result_type, qual_list, &nof_quals, action_list, &nof_actions, &fg_id));
        fg_ids[index] = fg_id;
        LOG_CLI((BSL_META("%s: FG creation done. ID = %d\n"), result_type, fg_ids[index]));
    }
    /** Perform sync */
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    /** Start to add entries for each result type */
    LOG_CLI((BSL_META("\n********** Stage II: Adding ELK FWD&ACL entries **********\n")));
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_MEASURE, measure);
    for (index = 0; index < max_result_types; index++)
    {
        result_type = cfg_sequence[index];
        if (result_type == NULL)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_app_type_check(unit, result_type, &is_kbp_acl));
        LOG_CLI((BSL_META("\n%s: ------------------------------\n"), result_type));

        if (is_kbp_acl)
        {
            fg_id = fg_ids[index];
            LOG_CLI((BSL_META("Adding %d ACL entries to FG %d ...\n"), nof_ent_to_add[index], fg_id));

            /** Get qualifier info and action info from the FG */
            SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_config_get
                            (unit, fg_id, qual_list, &nof_quals, action_list, &nof_actions));

            /** Add specific number of ACL entries */
            SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_entries
                            (unit, qual_list, nof_quals, action_list, nof_actions, fg_id, nof_ent_to_add[index],
                             &nof_ent_added, &entry_id_first));
            entry_id_start[index] = entry_id_first;
        }
        else
        {
            LOG_CLI((BSL_META("Adding %d FWD entries\n"), nof_ent_to_add[index]));

            /** Adding IPv4/IPv6 Route/RPF entries */
            if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF))
            {
                SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_route
                                (unit, result_type, nof_ent_to_add[index], &nof_ent_added));
            }

            /** Adding IPv4/IPv6 Host entries */
            if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP4) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6))
            {
                SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_host
                                (unit, result_type, nof_ent_to_add[index], &nof_ent_added));
            }

            /** Adding IPMCv4/IPMCv6 entries */
            if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP4) ||
                !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6))
            {
                SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_ipmc
                                (unit, result_type, nof_ent_to_add[index], &nof_ent_added));
            }
        }
        ent_num_added[index] = nof_ent_added;

        if (measure)
        {
            LOG_CLI((BSL_META("%d KBP entries added. Expected capacity is %d\n"), nof_ent_added, capacity_exp[index]));
            if ((double) nof_ent_added <= ((double) capacity_exp[index] * (1.0 + capacity_tolerance)) &&
                (double) nof_ent_added >= ((double) capacity_exp[index] * (1.0 - capacity_tolerance)))
            {
                LOG_CLI((BSL_META("Reached the expected capacity of %d within the expected %.2f%% tolerance (%d)\n"),
                         capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added));
            }
            else if ((double) nof_ent_added > ((double) capacity_exp[index] * (1.0 + capacity_tolerance)))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Reached higher capacity than the expected %d with %.2f%% tolerance (%d)\n",
                             capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Could not reach the expected capacity of %d with %.2f%% tolerance (%d)\n",
                             capacity_exp[index], (100.0 * capacity_tolerance), nof_ent_added);
            }
        }
        else
        {
            LOG_CLI((BSL_META("Measurements validation is not performed\n")));
        }
    }

    /** Start to get/modify/delete/re-add entries for each result type */
    LOG_CLI((BSL_META("\n********** Stage III: Updating ACL and FWD entries **********\n")));
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_UPDATE, update_flag);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MULTI_RST_TYPE_READD, readd_flag);
    for (index = 0; index < max_result_types; index++)
    {
        result_type = cfg_sequence[index];
        if (cfg_sequence[index] == NULL || !update_flag)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_app_type_check(unit, result_type, &is_kbp_acl));
        LOG_CLI((BSL_META("\n%s: ------------------------------\n"), result_type));

        if (is_kbp_acl)
        {
            /** Get qualifier info and action info from the FG */
            fg_id = fg_ids[index];
            SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_config_get
                            (unit, fg_id, qual_list, &nof_quals, action_list, &nof_actions));

            /*
             * Update KBP entries. ent_opcode:
             * 0. Get entries and check
             * 1. Modify entries and check
             * 2. Remove entries and check
             * 3. Rer-add entries and check
             */
            for (ent_opcode = 0; ent_opcode < 4; ent_opcode++)
            {
                /*
                 * Don't add deleted KBP entries back if readd_flag unset 
                 */
                if (!readd_flag && ent_opcode == 3)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(dnx_kbp_muti_result_types_capacity_acl_entry_update
                                (unit, qual_list, nof_quals, action_list, nof_actions, fg_id, entry_id_start[index],
                                 ent_num_added[index], ent_opcode));
            }
        }
        else
        {
            for (ent_opcode = 0; ent_opcode < 4; ent_opcode++)
            {
                /*
                 * Don't add deleted KBP entries back if readd_flag unset 
                 */
                if (!readd_flag && ent_opcode == 3)
                {
                    continue;
                }

                /** Updating IPv4/IPv6 route entries */
                if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_FWD) ||
                    !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP4_RPF) ||
                    !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_FWD) ||
                    !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_ROUTE_IP6_RPF))
                {
                    SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_route_update
                                    (unit, result_type, ent_num_added[index], ent_opcode));
                }

                /** Updating IPv4/IPv6 Host entries */
                if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP4) ||
                    !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_HOST_IP6))
                {
                    SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_host_update
                                    (unit, result_type, ent_num_added[index], ent_opcode));
                }

                /** Updating IPMCv4/IPMCv6 entries */
                if (!strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP4) ||
                    !strcmp(result_type, CTEST_DNX_KBP_RESULT_TYPE_IPMC_IP6))
                {
                    SHR_IF_ERR_EXIT(dnx_kbp_multi_result_types_capacity_fwd_ipmc_update
                                    (unit, result_type, ent_num_added[index], ent_opcode));
                }
            }
        }
    }
    if (!update_flag)
    {
        LOG_CLI((BSL_META("Entry updating is not performed\n\n")));
    }

    LOG_CLI((BSL_META("**********************************************************************\n")));
    LOG_CLI((BSL_META("* FwdACL capacity test done!\n")));
    LOG_CLI((BSL_META("**********************************************************************\n\n")));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_kbp_multi_result_types_capacity_fwd_acl_options[] = {
    {CTEST_DNX_KBP_SCENARIO, SAL_FIELD_TYPE_STR, "Test scenario", "Scenario0"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_MEASURE, SAL_FIELD_TYPE_BOOL, "Measure indication", "YES"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_READD, SAL_FIELD_TYPE_BOOL, "Readd indication", "NO"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_DEINIT_INIT, SAL_FIELD_TYPE_BOOL, "Deinit-Init indication", "YES"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_UPDATE, SAL_FIELD_TYPE_BOOL, "Update indication", "YES"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_PROFILE, SAL_FIELD_TYPE_UINT32, "User Profile", "0"}
    ,
    {NULL}
};

sh_sand_invoke_t dnx_kbp_multi_result_types_capacity_fwd_acl_tests[] = {
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FwdACL_S0, CTEST_DNX_KBP_SCENARIO "=Scenario0"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FwdACL_S1, CTEST_DNX_KBP_SCENARIO "=Scenario1"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FwdACL_S2, CTEST_DNX_KBP_SCENARIO "=Scenario2"}
    ,
    {CTEST_DNX_KBP_MULTI_RST_TYPE_CAP_TEST_FwdACL_S3, CTEST_DNX_KBP_SCENARIO "=Scenario3"}
    ,
    {NULL}
};

sh_sand_man_t dnx_kbp_multi_result_types_capacity_fwd_acl_man = {
    "Add KBP ACL&FWD entries and compare against expected capacity",
    "Add KBP ACL&FWD entries and compare against expected capacity",
    "",
    ""
};

sh_sand_cmd_t dnx_kbp_multi_result_types_capacity_cmds[] = {
    {"ForWarD", dnx_kbp_multi_result_types_capacity_fwd_cmd, NULL, dnx_kbp_multi_result_types_capacity_fwd_options,
     &dnx_kbp_multi_result_types_capacity_fwd_man, NULL, dnx_kbp_multi_result_types_capacity_fwd_tests,.label = "kbp"}
    ,
    {"ACL", dnx_kbp_multi_result_types_capacity_acl_cmd, NULL, dnx_kbp_multi_result_types_capacity_acl_options,
     &dnx_kbp_multi_result_types_capacity_acl_man, NULL, dnx_kbp_multi_result_types_capacity_acl_tests, 0x0,.label =
     "kbp"}
    ,
    {"FwdACL", dnx_kbp_multi_result_types_capacity_fwd_acl_cmd, NULL,
     dnx_kbp_multi_result_types_capacity_fwd_acl_options, &dnx_kbp_multi_result_types_capacity_fwd_acl_man, NULL,
     dnx_kbp_multi_result_types_capacity_fwd_acl_tests,.label = "kbp"}
    ,
    {NULL}
};

sh_sand_man_t dnx_kbp_multi_result_types_capacity_man = {
    "Add IPv4/IPv6/IPMC/ACL entries and compare against expected capacity",
    "Add IPv4/IPv6/IPMC/ACL entries and compare against expected capacity",
    "",
    ""
};

sh_sand_cmd_t dnx_kbp_multi_result_types_cmds[] = {
    {"Capacity", NULL, dnx_kbp_multi_result_types_capacity_cmds, NULL, &dnx_kbp_multi_result_types_capacity_man,.label =
     "kbp"}
    ,
    {NULL}
};

sh_sand_man_t dnx_kbp_multi_result_types_man = {
    "Test KBP entries with different result types",
    "Test different combinations with IPv4/IPv6/IPMC and ACL entries in KBP",
    "",
    ""
};
