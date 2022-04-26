/*
 * ! \file mdb_diag.c Contains all of the KBP ctest commands
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <appl/diag/diag.h>
#include <bcm_int/dnx/auto_generated/dnx_ipmc_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_l3_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_field_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
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

extern sh_sand_cmd_t dnx_kbp_multi_result_types_cmds[];
extern sh_sand_man_t dnx_kbp_multi_result_types_man;
extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];

/**
 * \brief
 * Strings used for representing ACL tests
 */
#define CTEST_DNX_KBP_ACL_MASTER_KEY    "master_key"
#define CTEST_DNX_KBP_ACL_LOOKUP        "lookup"
#define CTEST_DNX_KBP_ACL_ENTRY_SEM     "entry_sem"
#define CTEST_DNX_KBP_ACL_RANGES_SEM    "ranges_sem"
#define CTEST_DNX_KBP_ACL_OPCODE_CREATE "opcode_create"
#define CTEST_DNX_KBP_ACL_OPCODE        "opcode"

/** Strings used for ACL performance and capacity tests */
#define CTEST_DNX_KBP_ACL_KEY_SIZE      "Key"
#define CTEST_DNX_KBP_ACL_RESULT_LENGTH "Result"
#define CTEST_DNX_KBP_ACL_SKIP_CONFIG   "Skip_config"
#define CTEST_DNX_KBP_ACL_SKIP_DEINIT   "Skip_deinit"
#define CTEST_DNX_KBP_ACL_CACHE_COMMIT  "Cache_commit"

/** Strings used for FWD performance and capacity tests */
#define CTEST_DNX_KBP_TEST               "Test"
#define CTEST_DNX_KBP_FWD_HOST           "Host"
#define CTEST_DNX_KBP_FWD_ROUTE          "Route"
#define CTEST_DNX_KBP_FWD_RPF            "RPF"
#define CTEST_DNX_KBP_FWD_RPF_NO_PAYLOAD "RPF_NO_PAYLOAD"
#define CTEST_DNX_KBP_FWD_RPF_PROFILE1   "RPF_PROFILE1"
#define CTEST_DNX_KBP_FWD_IPMC           "IPMC"
#define CTEST_DNX_KBP_FWD_IPMC_LARGE_M1  "IPMC_LARGE_M1"
#define CTEST_DNX_KBP_FWD_IPMC_LARGE_M3  "IPMC_LARGE_M3"
#define CTEST_DNX_KBP_MEASURE            "Measure"
#define CTEST_DNX_KBP_PROFILE            "Profile"

/** Strings used for performance tests */
#define CTEST_DNX_KBP_PERFORMANCE_ENTRIES               "Entries"
#define CTEST_DNX_KBP_PERFORMANCE_RUNS                  "Runs"
#define CTEST_DNX_KBP_PERFORMANCE_CACHING               "Caching"
#define CTEST_DNX_KBP_PERFORMANCE_WARMBOOT              "WARMBOOT"
#define CTEST_DNX_KBP_PERFORMANCE_RANDOM                "Random"
#define CTEST_DNX_KBP_PERFORMANCE_UPDATE                "Update"
#define CTEST_DNX_KBP_PERFORMANCE_DELETE                "DeLeTe"
#define CTEST_DNX_KBP_PERFORMANCE_LAYER                 "Layer"
#define CTEST_DNX_KBP_PERFORMANCE_ALL                   "All"

#define CTEST_DNX_KBP_PERFORMANCE_LAYER_BCM             "BCM"
#define CTEST_DNX_KBP_PERFORMANCE_LAYER_DBAL            "DBAL"
#define CTEST_DNX_KBP_PERFORMANCE_LAYER_DBAL_FORCE      "DBAL_FORCE"
#define CTEST_DNX_KBP_PERFORMANCE_LAYER_ACCESS          "Access"

#define CTEST_DNX_KBP_PERFORMANCE_LAYER_ID_BCM          0
#define CTEST_DNX_KBP_PERFORMANCE_LAYER_ID_DBAL         1
#define CTEST_DNX_KBP_PERFORMANCE_LAYER_ID_DBAL_FORCE   2
#define CTEST_DNX_KBP_PERFORMANCE_LAYER_ID_ACCESS       3

/** Strings used for capacity tests */
#define CTEST_DNX_KBP_CAPACITY_DEINIT_INIT              "Deinit_Init"

#define CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH "kbp/ctest_dnx_kbp_measurements.xml"
/*
 * Sync and device lock are not required for the test, but if device is locked will cause the test to fail.
 * Store the current status and update it to INIT_DONE in order for this test to run without issues.
 * Revert it to its original state when the test finishes.
 */
#define CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_START() \
    do { \
        dbal_enum_value_field_kbp_device_status_e _stashed_kbp_mngr_status_; \
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &_stashed_kbp_mngr_status_)); \
        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE))

/** Update the KBP mngr status to its original state */
#define CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_END() \
        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, _stashed_kbp_mngr_status_)); \
    } while(0)

#define CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT(_cmd_) \
    LOG_CLI((BSL_META("%s\n"), _cmd_)); sh_process_command(unit, _cmd_)
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
#define CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK                                                       \
{                                                                                                    \
    if (!SAL_BOOT_PLISIM)                                                                              \
        {                                                                                              \
            if (!dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))            \
            {                                                                                          \
                LOG_CLI((BSL_META("test not supported when ipv4 not enabled in KBP\n")));              \
                SHR_EXIT();                                                                            \
            }                                                                                          \
        }                                                                                            \
}

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

static uint8
dnx_kbp_is_little_endian(
    void)
{
    uint32 a = 1;
    uint8 *b = (uint8 *) &a;
    return *b;
}

/** Used to retrieve the root of the measurements XML */
static shr_error_e
dnx_kbp_measurements_root_get(
    int unit,
    void **root)
{
    SHR_FUNC_INIT_VARS(unit);

    *root =
        dbx_file_get_xml_top(unit, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH, "CtestKbpMeasurementsRoot",
                             CONF_OPEN_PER_DEVICE);
    if (*root == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find KBP measurements xml file: %s\n",
                     CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }

exit:
    SHR_FUNC_EXIT;
}

/** Used to retrieve the node of the tested table */
static shr_error_e
dnx_kbp_fwd_measurements_node_get(
    int unit,
    void *root,
    uint8 is_slk,
    uint8 is_capacity,
    uint8 is_ipv6,
    char *test,
    void **node)
{
    void *test_node = NULL;
    void *cpu_or_core_node = NULL;
    void *ip_node = NULL;
    void *table_node = NULL;

    char *test_name = is_capacity ? "Capacity" : "Performance";
    char *ip_name = is_ipv6 ? "IPv6" : "IPv4";
    char *cpu_name = is_slk ? "SLK" : "Intel";
    char *core_name = DNX_KBP_OP2_IS_SINGLE_PORT_MODE(unit) ? "SingleCore" : "DualCore";
    char *table_name = test;
    char *cpu_or_core_name = is_capacity ? core_name : cpu_name;

    SHR_FUNC_INIT_VARS(unit);

    test_node = dbx_xml_child_get_first(root, test_name);
    if (test_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     test_name, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    cpu_or_core_node = dbx_xml_child_get_first(test_node, cpu_or_core_name);
    if (cpu_or_core_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     cpu_name, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    ip_node = dbx_xml_child_get_first(cpu_or_core_node, ip_name);
    if (ip_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     ip_name, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    table_node = dbx_xml_child_get_first(ip_node, table_name);
    if (table_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     table_name, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    *node = table_node;

exit:
    SHR_FUNC_EXIT;
}

/** Used to retrieve the expected capacity */
static shr_error_e
dnx_kbp_fwd_capacity_expected_measures_get(
    int unit,
    uint8 is_ipv6,
    char *test,
    int *capacity)
{
    void *root = NULL;
    void *node = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_kbp_measurements_root_get(unit, &root));
    SHR_IF_ERR_EXIT(dnx_kbp_fwd_measurements_node_get(unit, root, TRUE, TRUE, is_ipv6, test, &node));

    if (capacity)
    {
        RHDATA_GET_INT_STOP(node, "Capacity", *capacity);
    }

exit:
    dbx_xml_top_close(root);
    SHR_FUNC_EXIT;
}

/** This API parses the required test parameters from the input performance and capacity scenario */
static shr_error_e
dnx_kbp_fwd_performance_and_capacity_test_scenario_get(
    int unit,
    char **test,
    uint8 *rpf,
    uint8 *rpf_no_payload,
    uint8 *host,
    uint8 *ipmc,
    uint8 *ipmc_large)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!strncasecmp(*test, CTEST_DNX_KBP_FWD_ROUTE, 16))
    {
        *test = CTEST_DNX_KBP_FWD_ROUTE;
        *rpf = *host = *ipmc = FALSE;
        *ipmc_large = 0;
    }
    else if (!strncasecmp(*test, CTEST_DNX_KBP_FWD_HOST, 16))
    {
        *test = CTEST_DNX_KBP_FWD_HOST;
        *rpf = *ipmc = FALSE;
        *ipmc_large = 0;
        *host = TRUE;
    }
    else if (!strncasecmp(*test, CTEST_DNX_KBP_FWD_RPF, 16))
    {
        *test = CTEST_DNX_KBP_FWD_RPF;
        *ipmc = *host = FALSE;
        *ipmc_large = 0;
        *rpf = TRUE;
    }
    else if (!strncasecmp(*test, CTEST_DNX_KBP_FWD_RPF_NO_PAYLOAD, 16))
    {
        *test = CTEST_DNX_KBP_FWD_RPF_NO_PAYLOAD;
        *ipmc = *host = FALSE;
        *ipmc_large = 0;
        *rpf = *rpf_no_payload = TRUE;
    }
    else if (!strncasecmp(*test, CTEST_DNX_KBP_FWD_RPF_PROFILE1, 16))
    {
        *test = CTEST_DNX_KBP_FWD_RPF_PROFILE1;
        *ipmc = *host = FALSE;
        *ipmc_large = 0;
        *rpf = TRUE;
    }
    else if ((!strncasecmp(*test, CTEST_DNX_KBP_FWD_IPMC, 16)))
    {
        *test = CTEST_DNX_KBP_FWD_IPMC;
        *rpf = *host = FALSE;
        *ipmc_large = 0;
        *ipmc = TRUE;
    }
    else if ((!strncasecmp(*test, CTEST_DNX_KBP_FWD_IPMC_LARGE_M1, 16)))
    {
        *test = CTEST_DNX_KBP_FWD_IPMC_LARGE_M1;
        *rpf = *host = FALSE;
        *ipmc = TRUE;
        *ipmc_large = 1;
    }
    else if ((!strncasecmp(*test, CTEST_DNX_KBP_FWD_IPMC_LARGE_M3, 16)))
    {
        *test = CTEST_DNX_KBP_FWD_IPMC_LARGE_M3;
        *rpf = *host = FALSE;
        *ipmc = TRUE;
        *ipmc_large = 3;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown FWD test command \"%s\".\n", *test);
    }

exit:
    SHR_FUNC_EXIT;
}

/** This API parses the required test parameters from the input performance scenario */
static shr_error_e
dnx_kbp_performance_only_test_scenario_get(
    int unit,
    char **layer,
    uint8 *layer_id,
    uint8 *slk,
    uint8 *measure)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Determine whether the CPU is SLK or Intel-GTS depending on the endianness */
    *slk = dnx_kbp_is_little_endian()? FALSE : TRUE;

    /** Validate the layer from which the performance testing starts - BCM, DBAL, ACCESS */
    if (!strncasecmp(*layer, CTEST_DNX_KBP_PERFORMANCE_LAYER_BCM, 16))
    {
        *layer = CTEST_DNX_KBP_PERFORMANCE_LAYER_BCM;
        *layer_id = CTEST_DNX_KBP_PERFORMANCE_LAYER_ID_BCM;
    }
    else if (!strncasecmp(*layer, CTEST_DNX_KBP_PERFORMANCE_LAYER_DBAL, 16))
    {
        *layer = CTEST_DNX_KBP_PERFORMANCE_LAYER_DBAL;
        /** Do not validate measures for DBAL */
        *measure = FALSE;
        *layer_id = CTEST_DNX_KBP_PERFORMANCE_LAYER_ID_DBAL;
    }
    else if (!strncasecmp(*layer, CTEST_DNX_KBP_PERFORMANCE_LAYER_DBAL_FORCE, 16))
    {
        *layer = CTEST_DNX_KBP_PERFORMANCE_LAYER_DBAL;
        /** Do not validate measures for DBAL */
        *measure = FALSE;
        *layer_id = CTEST_DNX_KBP_PERFORMANCE_LAYER_ID_DBAL_FORCE;
    }
    else if (!strncasecmp(*layer, CTEST_DNX_KBP_PERFORMANCE_LAYER_ACCESS, 16))
    {
        *layer = CTEST_DNX_KBP_PERFORMANCE_LAYER_ACCESS;
        /** Do not validate measures for Access */
        *measure = FALSE;
        *layer_id = CTEST_DNX_KBP_PERFORMANCE_LAYER_ID_ACCESS;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown performance layer command \"%s\".\n", *layer);
    }

exit:
    SHR_FUNC_EXIT;
}

/** Used to calculate and print the performance measures */
static void
dnx_kbp_performance_detailed_measures_print(
    char *step_name,
    int *run_time,
    int nof_runs,
    int run_entries,
    int *avg_rate)
{
    int index;
    int mint = run_time[0], maxt = run_time[0], avgt = run_time[0];
    double minr, maxr, avgr;
    for (index = 1; index < nof_runs; index++)
    {
        if (mint > run_time[index])
        {
            mint = run_time[index];
        }
        if (maxt < run_time[index])
        {
            maxt = run_time[index];
        }
        avgt += run_time[index];
    }
    avgt /= nof_runs;
    minr = (double) run_entries / (double) mint;
    maxr = (double) run_entries / (double) maxt;
    avgr = (double) run_entries / (double) avgt;
    LOG_CLI((BSL_META("\n%s\n"), step_name));
    LOG_CLI((BSL_META("Time spent (us)\n\tmin - %d\n\tmax - %d\n\tavg - %d\n"), mint, maxt, avgt));
    LOG_CLI((BSL_META("Rate (Ke/s)\n\tbest  - %.2f\n\tworst - %.2f\n\tavg   - %.2f\n\n"), minr * 1000, maxr * 1000,
             avgr * 1000));

    if (avg_rate)
    {
        *avg_rate = avgr * 1000 * 1000;
    }
}

static shr_error_e
dnx_kbp_fwd_resource_profile_set(
    int unit,
    int profile,
    struct kbp_device *device_p)
{
    SHR_FUNC_INIT_VARS(unit);

    if (profile == 0)
    {
        /** profile value 0 means, no resource profile to be set */
        SHR_EXIT();
    }

    /** Profile is valid only for OP2 devices */
    if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource profile is valid only for OP2 (profile=%d)\n", profile);
    }

    switch (profile)
    {
        case 1:
        {
            /** Test an invalid profile, KBP SDK should return an error */
            SHR_IF_ERR_EXIT(dnx_kbp_error_translation
                            (kbp_device_load_resource_profile
                             (Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER],
                              "{(type:meta) (ver:1)}" "{(type:dev) (dev_name:KBP_DEV_0)}"
                              "{(type:db) (db_name:KBP_IPV4_UNICAST) (xl_db:1) (core:0) (sz_ab:6) (sz_uda:120) (core:1) (sz_ab:6)}"))
                            != _SHR_E_NONE);

            /** Test a valid resource profile */
            SHR_IF_ERR_EXIT(dnx_kbp_error_translation
                            (kbp_device_load_resource_profile
                             (Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER],
                              "{(type:meta) (ver:1)}" "{(type:dev) (dev_name:KBP_DEV_0)}"
                              "{(type:db) (db_name:KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) (xl_db:1) (core:0) (sz_ab:6) (sz_uda:120) (core:1) (sz_ab:6)}"
                              "{(type:ad) (ad_name:AD_64) (indir:1) (sz_uda:50)}"
                              "{(type:db) (db_name:KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) (xl_db:1) (core:0) (sz_ab:8) (core:1) (sz_ab:8)}"
                              "{(type:ad) (ad_name:AD_64) (indir:1) (sz_uda:4)}"
                              "{(type:db) (db_name:KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) (sz_ab:22) (sz_uda:200)}"
                              "{(type:db) (db_name:KBP_IPV6_UNICAST_PRIVATE_LPM_RPF) (sz_ab:8)}"
                              "{(type:db) (db_name:KBP_IPV4_MULTICAST_TCAM_FORWARD) (algo:2) (mc:1) (sz_ab:2)}"
                              "{(type:ad) (ad_name:AD_24) (sz_uda:4)}" "{(type:ad) (ad_name:AD_64) (sz_uda:6)}"
                              "{(type:db) (db_name:KBP_IPV6_MULTICAST_TCAM_FORWARD) (algo:2) (mc:1) (sz_ab:2)}"
                              "{(type:ad) (ad_name:AD_24) (sz_uda:4)}" "{(type:ad) (ad_name:AD_64) (sz_uda:4)}")));
        }
            break;
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid resource profile %d\n", profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_fwd_capacity_host(
    int unit,
    uint8 is_ipv6,
    int *capacity)
{
    int rv = 0;
    int iter = 0;
    bcm_l3_host_t host = { 0 };

    bcm_ip6_t Ipv6Addr =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

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
        host.l3a_ip_addr = 0;
    }

    /** Add entries until failure */
    while (TRUE)
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
        iter++;

        rv = bcm_l3_host_add(unit, &host);
        if (rv != BCM_E_NONE)
        {
            if (rv != BCM_E_FULL)
            {
                SHR_ERR_EXIT(rv, "Error, bcm_l3_host_add failed - host number %d\n", iter);
            }
            else
            {
                LOG_CLI((BSL_META("Table full after %d hosts\n"), iter));
                break;
            }
        }
        /** Progression print */
        if (iter % 131072 == 0)
        {
            LOG_CLI((BSL_META("%d hosts\n"), iter));
        }
    }

    *capacity = iter;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_fwd_capacity_ipmc(
    int unit,
    uint8 is_ipv6,
    uint32 fwd_flags,
    int *capacity)
{
    int rv = 0;
    int iter = 0;
    bcm_ipmc_addr_t ipmc_addr = { 0 };

    bcm_ip6_t Ipv6Mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_ip6_t Ipv6Route =
        { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

    /** Set the common route properties */
    bcm_ipmc_addr_t_init(&ipmc_addr);

    ipmc_addr.flags = fwd_flags;
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

    /** Add entries until failure */
    while (TRUE)
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
        iter++;

        rv = bcm_ipmc_add(unit, &ipmc_addr);
        if (rv != BCM_E_NONE)
        {
            if (rv != BCM_E_FULL)
            {
                SHR_ERR_EXIT(rv, "Error, bcm_ipmc_add failed - entry number %d\n", iter);
            }
            else
            {
                LOG_CLI((BSL_META("Table full after %d entries\n"), iter));
                break;
            }
        }
        /** Progression print */
        if (iter % 16384 == 0)
        {
            LOG_CLI((BSL_META("%d entries\n"), iter));
        }
    }

    *capacity = iter;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_fwd_capacity_route(
    int unit,
    uint8 is_ipv6,
    uint32 fwd_flags2,
    int *capacity)
{
    int rv = 0;
    int iter = 0;
    bcm_l3_route_t route = { 0 };

    bcm_ip6_t Ipv6Mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_ip6_t Ipv6Route =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    SHR_FUNC_INIT_VARS(unit);

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
    }
    else
    {
        route.l3a_subnet = 0;
        route.l3a_ip_mask = 0xFFFFFFFF;
    }

    /** Add entries until failure */
    while (TRUE)
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

        if (route.l3a_flags2 & BCM_L3_FLAGS2_NO_PAYLOAD)
        {
            route.l3a_intf = 0;
        }
        else if (++route.l3a_intf == 256 * 1024)
        {
            route.l3a_intf = 1;
        }
        iter++;

        rv = bcm_l3_route_add(unit, &route);
        if (rv != BCM_E_NONE)
        {
            if (rv != BCM_E_FULL)
            {
                SHR_ERR_EXIT(rv, "Error, bcm_l3_route_add failed - route number %d\n", iter);
            }
            else
            {
                LOG_CLI((BSL_META("Table full after %d routes\n"), iter));
                break;
            }
        }
        /** Progression print */
        if (iter % 131072 == 0)
        {
            LOG_CLI((BSL_META("%d routes\n"), iter));
        }
    }

    *capacity = iter;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_fwd_capacity(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    uint8 ipv6)
{
    char *test = NULL;
    char *layer = CTEST_DNX_KBP_PERFORMANCE_LAYER_BCM;
    int capacity = 0;
    double capacity_tolerance = 0.05;
    uint8 rpf = FALSE;
    uint8 rpf_no_payload = FALSE;
    uint8 host = FALSE;
    uint8 ipmc = FALSE;
    uint8 ipmc_large = 0;
    uint8 deinit_init = FALSE;
    uint8 measure = FALSE;
    uint32 profile = 0;
    uint8 layer_id = 0;
    uint8 slk = FALSE;
    uint32 fwd_flags = 0;
    uint32 fwd_flags2 = 0;
    int start_time = 0;
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t ctest_soc_property[2] = { {0} };

    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM || !dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Test is not supported without KBP\n");
    }

    SH_SAND_GET_STR(CTEST_DNX_KBP_TEST, test);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MEASURE, measure);
    SH_SAND_GET_UINT32(CTEST_DNX_KBP_PROFILE, profile);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_CAPACITY_DEINIT_INIT, deinit_init);

    if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321)
    {
        if (profile != 0)
        {
            /** Perform testing with profile != only on OP2 */
            SHR_ERR_EXIT(_SHR_E_NONE, "Testing with profile != 0 is applicable for only OP2 device\n");
        }
        /** Perform measures validation only on OP2 */
        measure = FALSE;
    }
    if (dnx_data_elk.application.split_rpf_get(unit) == 0)
    {
        /** Skip measures validation for non-split RPF */
        measure = FALSE;
    }

    SHR_IF_ERR_EXIT(dnx_kbp_fwd_performance_and_capacity_test_scenario_get
                    (unit, &test, &rpf, &rpf_no_payload, &host, &ipmc, &ipmc_large));
    SHR_IF_ERR_EXIT(dnx_kbp_performance_only_test_scenario_get(unit, &layer, &layer_id, &slk, &measure));

    if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321 && ipmc_large)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "The testcase is applicable for only OP2 device\n");
    }

    /** Perform deinit-init for regression */
    if (deinit_init)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_init_deinit(unit, NULL));
    }

    /*
     * This test is capable of running on LPM. Perform checks and initializations for KBP only if KBP is enabled.
     * In simulation KBP is always enabled and the FWD applications need to be validated separately.
     */
    if (dnx_kbp_device_enabled(unit))
    {
        /** Set resource profile */
        SHR_IF_ERR_EXIT(dnx_kbp_fwd_resource_profile_set
                        (unit, profile, Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER]));

        if ((ipv6 && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6)) ||
            (!ipv6 && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4)))
        {
            /*
             * Setting large MC custom soc property.
             */
            if (ipmc_large)
            {
                if (ipv6)
                {
                    ctest_soc_property[0].property = "ipv6_large_mc_enable";
                }
                else
                {
                    ctest_soc_property[0].property = "ipv4_large_mc_enable";
                }
                ctest_soc_property[0].value = (ipmc_large == 1) ? "1" : "3";
                SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
            }
            /** Perform sync and use the correct flags only if the relevant application is enabled */
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));
            fwd_flags = ipmc ? BCM_IPMC_TCAM : 0;
            fwd_flags2 = dnx_data_elk.application.split_rpf_get(unit) ?
                (rpf ? BCM_L3_FLAGS2_RPF_ONLY : BCM_L3_FLAGS2_FWD_ONLY) : 0;
            if (rpf_no_payload)
            {
                fwd_flags2 |= BCM_L3_FLAGS2_NO_PAYLOAD;
            }
        }
    }

    start_time = sal_time_usecs();
    if (host)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_fwd_capacity_host(unit, ipv6, &capacity));
    }
    else if (ipmc)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_fwd_capacity_ipmc(unit, ipv6, fwd_flags, &capacity));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_kbp_fwd_capacity_route(unit, ipv6, fwd_flags2, &capacity));
    }
    LOG_CLI((BSL_META("Finished filling the table for %ds\n\n"), ((sal_time_usecs() - start_time) / (1000 * 1000))));

    /** Validate if the measured capacity meets the expected capacity */
    if (measure)
    {
        int expected_capacity = 0;

        SHR_IF_ERR_EXIT(dnx_kbp_fwd_capacity_expected_measures_get(unit, ipv6, test, &expected_capacity));
        if ((double) capacity <= ((double) expected_capacity * (1.0 + capacity_tolerance)) &&
            (double) capacity >= ((double) expected_capacity * (1.0 - capacity_tolerance)))
        {
            LOG_CLI((BSL_META("Reached the expected capacity of %d within the expected %.2f%% tolerance (%d)\n"),
                     expected_capacity, (100.0 * capacity_tolerance), capacity));
        }
        else if ((double) capacity > ((double) expected_capacity * (1.0 + capacity_tolerance)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Reached higher capacity than the expected %d with %.2f%% tolerance (%d)\n",
                         expected_capacity, (100.0 * capacity_tolerance), capacity);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not reach the expected capacity of %d with %.2f%% tolerance (%d)\n",
                         expected_capacity, (100.0 * capacity_tolerance), capacity);
        }
    }
    else
    {
        LOG_CLI((BSL_META("Measurements validation is not performed\n")));
    }

    /** Perform deinit-init for regression */
    if (deinit_init)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_init_deinit(unit, NULL));
    }

exit:
    /*
     * Resetting soc properties.
     */
    if (ipmc_large)
    {
        ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_fwd_capacity_ipv4_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_kbp_fwd_capacity(unit, args, sand_control, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_fwd_capacity_ipv6_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_kbp_fwd_capacity(unit, args, sand_control, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

static sh_sand_option_t dnx_kbp_fwd_capacity_cmd_options[] = {
    {CTEST_DNX_KBP_TEST, SAL_FIELD_TYPE_STR, "Test scenario", CTEST_DNX_KBP_FWD_ROUTE},
    {CTEST_DNX_KBP_MEASURE, SAL_FIELD_TYPE_BOOL, "Measure indication", "NO"},
    {CTEST_DNX_KBP_CAPACITY_DEINIT_INIT, SAL_FIELD_TYPE_BOOL, "Deinit-Init indication", "NO"},
    {CTEST_DNX_KBP_PROFILE, SAL_FIELD_TYPE_UINT32, "User Profile", "0"},
    {NULL}
};

static sh_sand_invoke_t dnx_kbp_fwd_capacity_ipv4_tests[] = {
    {CTEST_DNX_KBP_FWD_ROUTE, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_ROUTE " " CTEST_DNX_KBP_MEASURE, CTEST_POSTCOMMIT},
    {CTEST_DNX_KBP_FWD_RPF, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_RPF " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_RPF_PROFILE1, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_RPF_PROFILE1 " " CTEST_DNX_KBP_MEASURE " " CTEST_DNX_KBP_PROFILE "=1", CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_RPF_NO_PAYLOAD, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_RPF_NO_PAYLOAD " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_HOST, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_HOST " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_IPMC, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_IPMC " " CTEST_DNX_KBP_MEASURE, CTEST_POSTCOMMIT},
    {CTEST_DNX_KBP_FWD_IPMC_LARGE_M1, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=NO " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_IPMC_LARGE_M1 " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_IPMC_LARGE_M3, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=NO " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_IPMC_LARGE_M3 " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {NULL}
};

static sh_sand_invoke_t dnx_kbp_fwd_capacity_ipv6_tests[] = {
    {CTEST_DNX_KBP_FWD_ROUTE, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_ROUTE " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_RPF, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_RPF " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_RPF_NO_PAYLOAD, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_RPF_NO_PAYLOAD " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_HOST, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_HOST " " CTEST_DNX_KBP_MEASURE, CTEST_POSTCOMMIT},
    {CTEST_DNX_KBP_FWD_IPMC, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=YES " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_IPMC " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_IPMC_LARGE_M1, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=NO " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_IPMC_LARGE_M1 " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {CTEST_DNX_KBP_FWD_IPMC_LARGE_M3, CTEST_DNX_KBP_CAPACITY_DEINIT_INIT "=NO " CTEST_DNX_KBP_TEST "=" CTEST_DNX_KBP_FWD_IPMC_LARGE_M3 " " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {NULL}
};

static sh_sand_man_t dnx_kbp_fwd_capacity_ipv4_man = {
    "Add IPv4 route entries and compare against expected capacity",
    "Add IPv4 route entries and compare against expected capacity",
    "",
    ""
};

static sh_sand_man_t dnx_kbp_fwd_capacity_ipv6_man = {
    "Add IPv6 route entries and compare against expected capacity",
    "Add IPv6 route entries and compare against expected capacity",
    "",
    ""
};

static sh_sand_invoke_t dnx_kbp_fwd_capacity_tests[] = {
    {NULL}
};

static sh_sand_man_t dnx_kbp_fwd_capacity_man = {
    "Add IPv4 or IPv6 Host/Route/RPF/IPMC entries and compare against expected capacity",
    "Add IPv4 or IPv6 Host/Route/RPF/IPMC entries and compare against expected capacity",
    "",
    ""
};

static sh_sand_option_t dnx_kbp_fwd_capacity_options[] = {
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_fwd_capacity_cmds[] = {
    {"IPv4", dnx_kbp_fwd_capacity_ipv4_cmd, NULL, dnx_kbp_fwd_capacity_cmd_options, &dnx_kbp_fwd_capacity_ipv4_man, NULL, dnx_kbp_fwd_capacity_ipv4_tests, .label="kbp"},
    {"IPv6", dnx_kbp_fwd_capacity_ipv6_cmd, NULL, dnx_kbp_fwd_capacity_cmd_options, &dnx_kbp_fwd_capacity_ipv6_man, NULL, dnx_kbp_fwd_capacity_ipv6_tests, .label="kbp"},
    {NULL}
};

/* *INDENT-ON* */

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
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    shr_error_e curr_res = _SHR_E_NONE;
    dnx_dbal_ltt_input_t ltt_input = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Set the Result PRT structure */
    PRT_TITLE_SET("Logical Table Test (Flags=0x%x) Summary", ltt_input.flags);
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

            ltt_input.flags = LTT_FLAGS_DEFAULT;
            ltt_input.nof_entries = DIAG_DBAL_DEFAULT_NOF_ENTRIES;
            curr_res = ctest_dbal_table_test_run(unit, table_id, &ltt_input);
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

            ltt_input.flags = LTT_FLAGS_MEASUREMENTS;
            ltt_input.nof_entries = 3;
            curr_res = ctest_dbal_table_test_run(unit, table_id, &ltt_input);
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

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_kbp_fwd_cmds[] = {
    {"Capacity", NULL, dnx_kbp_fwd_capacity_cmds, dnx_kbp_fwd_capacity_options, &dnx_kbp_fwd_capacity_man, NULL, dnx_kbp_fwd_capacity_tests, .label="kbp"},
    {"SEMantic", dnx_kbp_fwd_semantic, NULL, NULL, &dnx_kbp_fwd_semantic_man, .label="kbp"},
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

/*
 * See dnx_kbp_acl_man for more information on the test case
 */
shr_error_e
dnx_kbp_acl_master_key_cmd(
    int unit)
{
    int index;
    int index_check;
    uint8 opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_NOP;
    uint32 nof_key_segments = 1;
    uint32 nof_key_segments_check = 0;
    uint32 nof_key_segments_stash = 0;
    kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { {0} };
    kbp_mngr_key_segment_t master_key_segments_check[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { {0} };
    kbp_mngr_key_segment_t master_key_segments_stash[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { {0} };
    CTEST_DNX_KBP_LOGGER_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_KBP_LOGGER_INIT();

    CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_START();
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Starting KBP manager master key testing\n\n")));

    for (index = 0; index < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; index++)
    {
        kbp_mngr_key_segment_t_init(unit, &master_key_segments[index]);
    }

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

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add
                                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, nof_key_segments, NULL));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add too much segments */
    opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_NOP;
    nof_key_segments = DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY + 1;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using too much segments %d with \"kbp_mngr_opcode_master_key_segments_add()\"\n"),
              nof_key_segments));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add
                                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, nof_key_segments,
                                     master_key_segments));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add invalid segment size 0 */
    master_key_segments[0].nof_bytes = 0;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid segment size %d with \"kbp_mngr_opcode_master_key_segments_add()\"\n"),
              master_key_segments[0].nof_bytes));

    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add
                                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, 1, master_key_segments));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
    }

    /** Add invalid segment size above the maximum supported */
    master_key_segments[0].nof_bytes = DNX_KBP_MAX_SEGMENT_LENGTH_IN_BYTES + 1;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Using invalid segment size %d with \"kbp_mngr_opcode_master_key_segments_add()\"\n"),
              master_key_segments[0].nof_bytes));
    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add
                                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, 1, master_key_segments));
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
    CTEST_DNX_KBP_EXEC_SUPPRESS_LOG(kbp_mngr_opcode_master_key_segments_add
                                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, 2, master_key_segments));
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

    nof_key_segments = DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - nof_key_segments_stash;
    for (index = 0; index < nof_key_segments; index++)
    {
        sal_snprintf(master_key_segments[index].name, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, "TEST_SEGMENT_%d", index);
        master_key_segments[index].nof_bytes = 1;
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, nof_key_segments, master_key_segments));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get
                    (unit, opcode_id, &nof_key_segments_check, master_key_segments_check));

    if (nof_key_segments_check != DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to get %d master key segments, but got %d\n",
                     DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY, nof_key_segments_check);
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

    CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_END();

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
    int opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_NOP;
    kbp_opcode_lookup_info_t lookup_info, lookup_info_check;
    CTEST_DNX_KBP_LOGGER_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_KBP_LOGGER_INIT();

    CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK;

    CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_START();
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Starting KBP manager lookup testing\n\n")));

    SHR_IF_ERR_EXIT(kbp_opcode_lookup_info_t_init(unit, &lookup_info));

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
    opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_IPV4_UNICAST_PRIVATE_W_UC_RPF;
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
    if (SHR_GET_CURRENT_ERR() != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to fail with %d, but result was %d\n", _SHR_E_PARAM, SHR_GET_CURRENT_ERR());
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
    opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_IPV4_UNICAST_PRIVATE_W_UC_RPF;
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

    CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_END();

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
    int opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_NOP;
    kbp_opcode_lookup_info_t lookup_info;
    uint32 nof_key_segments = 0;
    kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { {0} };
    SHR_FUNC_INIT_VARS(unit);

    CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK;

    CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_START();
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Starting KBP manager opcode testing\n\n")));

    SHR_IF_ERR_EXIT(kbp_opcode_lookup_info_t_init(unit, &lookup_info));
    for (index = 0; index < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; index++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_t_init(unit, &master_key_segments[index]));
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_key_segments, master_key_segments));

    lookup_info.result_index = 0;
    lookup_info.dbal_table_id = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
    lookup_info.result_offset = 0;
    lookup_info.nof_segments = 5;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Setting opcode %d and lookup to result %d\n"), opcode_id, lookup_info.result_index));

    for (index = 0; index < lookup_info.nof_segments; index++)
    {
        sal_snprintf(master_key_segments[index].name, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, "TEST_SEGMENT_%d", index);
        master_key_segments[index].nof_bytes = index + 1;
        lookup_info.key_segment_index[index] = index;
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, lookup_info.nof_segments, master_key_segments));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Testing opcode passed; Clearing opcode %d and lookup to result %d\n"), opcode_id,
              lookup_info.result_index));

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_result_clear(unit, opcode_id, lookup_info.result_index));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_clear(unit, opcode_id));

    CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_END();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_opcode_create(
    int unit)
{
    int opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_IPV4_UNICAST_PRIVATE_W_UC_RPF;
    uint8 acl_ctx;
    uint8 new_opcode_id;
    char *opcode_name = "test opcode";

    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_KBP_GENERIC_TEST_SUPPORT_CHECK;

    CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_START();
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Starting KBP manager opcode create testing\n\n")));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_create(unit, opcode_id, opcode_name, &acl_ctx, &new_opcode_id));
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Created new opcode %d\n"), new_opcode_id));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Testing opcode create passed; Clearing opcode %d\n"), new_opcode_id));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_clear(unit, new_opcode_id));

    CTEST_DNX_KBP_ACL_DEVICE_STATUS_DONT_CARE_END();

exit:
    SHR_FUNC_EXIT;
}

/**********************************************************************************************************************/
shr_error_e
dnx_kbp_acl_kbp_mngr_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test = NULL;
    uint32 kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("Test", test);
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

    if (ISEMPTY(test))
    {
        /** Run all tests */
        SHR_IF_ERR_EXIT(dnx_kbp_acl_master_key_cmd(unit));
        SHR_IF_ERR_EXIT(dnx_kbp_acl_lookup_cmd(unit));
        SHR_IF_ERR_EXIT(dnx_kbp_acl_opcode_cmd(unit));
        SHR_IF_ERR_EXIT(dnx_kbp_acl_opcode_create(unit));
    }
    else
    {
        if (!sal_strncmp(test, CTEST_DNX_KBP_ACL_MASTER_KEY, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_acl_master_key_cmd(unit));
        }
        else if (!sal_strncmp(test, CTEST_DNX_KBP_ACL_LOOKUP, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_acl_lookup_cmd(unit));
        }
        else if (!sal_strncmp(test, CTEST_DNX_KBP_ACL_OPCODE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_acl_opcode_cmd(unit));
        }
        else if ((!sal_strncmp(test, CTEST_DNX_KBP_ACL_OPCODE_CREATE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))
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
static sh_sand_invoke_t dnx_kbp_acl_kbp_mngr_tests[] = {
    {CTEST_DNX_KBP_ACL_MASTER_KEY,      "Test=" CTEST_DNX_KBP_ACL_MASTER_KEY,       CTEST_PRECOMMIT},
    {CTEST_DNX_KBP_ACL_LOOKUP,          "Test=" CTEST_DNX_KBP_ACL_LOOKUP,           CTEST_PRECOMMIT},
    {CTEST_DNX_KBP_ACL_OPCODE,          "Test=" CTEST_DNX_KBP_ACL_OPCODE,           CTEST_PRECOMMIT},
    {CTEST_DNX_KBP_ACL_OPCODE_CREATE,   "Test=" CTEST_DNX_KBP_ACL_OPCODE_CREATE,    CTEST_PRECOMMIT},
    {NULL}
};

static sh_sand_man_t dnx_kbp_acl_kbp_mngr_man = {
    "KBP manager ACL related tests",
    "Setting \"Test\" option tests a selected group of KBP manager APIs. Setting no option will run all tests.\n"
    "List of supported ACL tests:\n"
    CTEST_DNX_KBP_ACL_MASTER_KEY " - Test that validates the KBP manager dynamic APIs related to master key\n"
    CTEST_DNX_KBP_ACL_LOOKUP " - Test that validates the KBP manager dynamic APIs related to opcode lookups\n"
    CTEST_DNX_KBP_ACL_OPCODE " - Test that configures lookup to a non-forwarding opcode\n"
    CTEST_DNX_KBP_ACL_OPCODE_CREATE" - Test that creates dynamic opcode based on fwd opcode\n"
    "[Test=kbp_mngr_acl_test]",
    "Test=" CTEST_DNX_KBP_ACL_MASTER_KEY "\n"
    "Test=" CTEST_DNX_KBP_ACL_LOOKUP "\n"
    "Test=" CTEST_DNX_KBP_ACL_OPCODE "\n"
    "Test=" CTEST_DNX_KBP_ACL_OPCODE_CREATE
};

static sh_sand_option_t dnx_kbp_acl_kbp_mngr_cmd_options[] = {
    {"Test", SAL_FIELD_TYPE_STR, "KBP manager ACL related test", ""},
    {NULL}
};
/* *INDENT-ON* */

#define DNX_KBP_ACL_QUAL_ATTACH_INFO_SET(_attach_info_) \
_attach_info_.input_type = bcmFieldInputTypeLayerFwd; _attach_info_.input_arg = 0; _attach_info_.offset = 0

static shr_error_e
dnx_kbp_acl_quals_by_key_size_set(
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
    DNX_KBP_ACL_QUAL_ATTACH_INFO_SET(attach_info_list[0]);
    if (key_size > 80)
    {
        (*nof_quals)++;
        qual_list[1] = bcmFieldQualifyDstMac;
        DNX_KBP_ACL_QUAL_ATTACH_INFO_SET(attach_info_list[1]);
    }
    if (key_size > 160)
    {
        (*nof_quals)++;
        qual_list[2] = bcmFieldQualifySrcIp6;
        DNX_KBP_ACL_QUAL_ATTACH_INFO_SET(attach_info_list[2]);
    }
    if (key_size > 320)
    {
        (*nof_quals)++;
        qual_list[3] = bcmFieldQualifyDstIp6;
        DNX_KBP_ACL_QUAL_ATTACH_INFO_SET(attach_info_list[3]);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_uda_create(
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
dnx_kbp_acl_config_create(
    int unit,
    uint32 key_size,
    uint32 res_length,
    bcm_field_qualify_t * qual_list,
    int *nof_quals,
    bcm_field_action_t * action_list,
    int *nof_actions,
    int initial_capacity,
    bcm_field_group_t * fg_id)
{
    int index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_qualify_attach_info_t attach_info_list[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    bcm_field_context_t context_id;
    uint32 res_offset = 248 - res_length;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_list, _SHR_E_INTERNAL, "qual_list");
    SHR_NULL_CHECK(nof_quals, _SHR_E_INTERNAL, "nof_quals");
    SHR_NULL_CHECK(action_list, _SHR_E_INTERNAL, "action_list");
    SHR_NULL_CHECK(nof_actions, _SHR_E_INTERNAL, "nof_actions");
    SHR_NULL_CHECK(fg_id, _SHR_E_INTERNAL, "fg_id");

    /** Validate the key size */
    /** Do not perform testing on 360b key, because 360b key size is no longer valid */
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
    SHR_IF_ERR_EXIT(dnx_kbp_acl_quals_by_key_size_set(unit, key_size, qual_list, attach_info_list, nof_quals));
    /** Create user defined void actions according to the tested result length */
    SHR_IF_ERR_EXIT(dnx_kbp_acl_uda_create(unit, res_length, action_list, nof_actions));

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
    /** Set the initial capacity */
    if (initial_capacity != 0)
    {
        fg_info.external_info.capacity_valid = TRUE;
        fg_info.external_info.capacity = initial_capacity;
    }

    /** Create the KBP ACL field group */
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

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

    /** Call the attach API */
    context_id = (bcmFieldAppTypeTrafficManagement & 0x00ff);
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0 /** flags */ , *fg_id, context_id, &group_attach_info));

    /** Perform sync */
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

exit:
    SHR_FUNC_EXIT;
}

/** Used to retrieve the node of the tested table */
static shr_error_e
dnx_kbp_acl_measurements_node_get(
    int unit,
    void *root,
    uint8 is_capacity,
    uint8 is_slk,
    uint32 key_size,
    uint32 result_length,
    void **node)
{
    void *test_node = NULL;
    void *cpu_or_core_node = NULL;
    void *acl_node = NULL;
    void *key_node = NULL;
    void *result_node = NULL;

    char *test_name = is_capacity ? "Capacity" : "Performance";
    char *cpu_name = is_slk ? "SLK" : "Intel";
    char *core_name = DNX_KBP_OP2_IS_SINGLE_PORT_MODE(unit) ? "SingleCore" : "DualCore";
    char *cpu_or_core_name = is_capacity ? core_name : cpu_name;
    char *key_name =
        (key_size == 80) ? "Key80" : (key_size == 160) ? "Key160" : (key_size == 320) ? "Key320" : "Key360";
    char *result_name = (result_length == 32) ? "Result32" : (result_length == 64) ? "Result64" : "Result128";

    SHR_FUNC_INIT_VARS(unit);

    test_node = dbx_xml_child_get_first(root, test_name);
    if (test_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     test_name, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    cpu_or_core_node = dbx_xml_child_get_first(test_node, cpu_or_core_name);
    if (cpu_or_core_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     cpu_name, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    acl_node = dbx_xml_child_get_first(cpu_or_core_node, "ACL");
    if (acl_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     "ACL", CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    key_node = dbx_xml_child_get_first(acl_node, key_name);
    if (key_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     key_name, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    result_node = dbx_xml_child_get_first(key_node, result_name);
    if (result_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     result_name, CTEST_DNX_KBP_MEASUREMENTS_XML_FILE_PATH);
    }
    *node = result_node;

exit:
    SHR_FUNC_EXIT;
}

/** Used to retrieve the expected performance rates */
static shr_error_e
dnx_kbp_acl_performance_expected_measures_get(
    int unit,
    uint8 is_slk,
    uint32 key_size,
    uint32 result_length,
    int *add_rate,
    int *get_rate,
    int *delete_rate)
{
    void *root = NULL;
    void *node = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_kbp_measurements_root_get(unit, &root));
    SHR_IF_ERR_EXIT(dnx_kbp_acl_measurements_node_get(unit, root, FALSE, is_slk, key_size, result_length, &node));

    if (add_rate)
    {
        RHDATA_GET_INT_STOP(node, "Add", *add_rate);
    }
    if (get_rate)
    {
        RHDATA_GET_INT_STOP(node, "Get", *get_rate);
    }
    if (get_rate)
    {
        RHDATA_GET_INT_STOP(node, "Delete", *delete_rate);
    }

exit:
    dbx_xml_top_close(root);
    SHR_FUNC_EXIT;
}

/** Used to retrieve the expected capacity */
static shr_error_e
dnx_kbp_acl_capacity_expected_measures_get(
    int unit,
    uint32 key_size,
    uint32 result_length,
    int *capacity)
{
    void *root = NULL;
    void *node = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_kbp_measurements_root_get(unit, &root));
    SHR_IF_ERR_EXIT(dnx_kbp_acl_measurements_node_get(unit, root, TRUE, TRUE, key_size, result_length, &node));

    if (capacity)
    {
        RHDATA_GET_INT_STOP(node, "Capacity", *capacity);
    }

exit:
    dbx_xml_top_close(root);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_performance_entries_generate(
    int unit,
    bcm_field_entry_info_t * entry_info,
    int nof_entries,
    bcm_field_qualify_t * qual_list,
    int nof_quals,
    bcm_field_action_t * action_list,
    int nof_actions)
{
    int index;
    int entries_index;
    int key_value = 1;
    int res_value = 1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_info, _SHR_E_INTERNAL, "entry_info");

    for (entries_index = 0; entries_index < nof_entries; entries_index++)
    {
        /*
         * Initialize kbp_entry_info
         */
        bcm_field_entry_info_t_init(&entry_info[entries_index]);

        for (index = 0; index < nof_quals; index++)
        {
            entry_info[entries_index].entry_qual[index].type = qual_list[index];
            entry_info[entries_index].entry_qual[index].value[0] = key_value;
            entry_info[entries_index].entry_qual[index].mask[0] = 0xFFFFFFFF;
        }
        entry_info[entries_index].nof_entry_quals = nof_quals;
        key_value++;

        for (index = 0; index < nof_actions; index++)
        {
            entry_info[entries_index].entry_action[index].type = action_list[index];
            entry_info[entries_index].entry_action[index].value[0] = res_value;
        }
        entry_info[entries_index].nof_entry_actions = nof_actions;
        res_value++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_performance_entries(
    int unit,
    int nof_runs,
    int nof_entries,
    bcm_field_qualify_t * qual_list,
    int nof_quals,
    bcm_field_action_t * action_list,
    int nof_actions,
    bcm_field_group_t fg_id,
    unsigned int *action,
    int run_time[3][100])
{
    int rv;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    uint32 timer_idx[3] = { 1, 2, 3 };
    uint32 timers_group;
    int entry_index, run_index, array_index;
    int total_entries;
    int start_time = 0;
    bcm_field_entry_info_t *entry_info = NULL;
    bcm_field_entry_t *entry_id = NULL;

    SHR_FUNC_INIT_VARS(unit);

    total_entries = nof_entries * nof_runs;
    entry_info = (bcm_field_entry_info_t *) sal_alloc(sizeof(bcm_field_entry_info_t) * total_entries, "entry_info");
    if (entry_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d entries\n", total_entries);
    }
    entry_id = (bcm_field_entry_t *) sal_alloc(sizeof(bcm_field_entry_t) * total_entries, "entry_id");
    if (entry_id == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d entry IDs\n", total_entries);
    }
    SHR_IF_ERR_EXIT(dnx_kbp_acl_performance_entries_generate
                    (unit, entry_info, total_entries, qual_list, nof_quals, action_list, nof_actions));

    sal_strncpy(group_name, "KBP performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    array_index = 0;
    /** Iterate over entries to add */
    utilex_ll_timer_clear_all(timers_group);
    LOG_CLI((BSL_META("step1: Add entries\n")));
    utilex_ll_timer_set("bcm_field_entry_add create", timers_group, timer_idx[0]);

    for (run_index = 0; run_index < nof_runs; run_index++)
    {
        start_time = sal_time_usecs();
        for (entry_index = 0; entry_index < nof_entries; entry_index++)
        {
            rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info[array_index], &entry_id[array_index]);

            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "Error, bcm_field_entry_add failed - entry index %d\n", array_index);
            }
            array_index++;
        }
        run_time[0][run_index] = sal_time_usecs() - start_time;
    }
    utilex_ll_timer_stop(timers_group, timer_idx[0]);

    array_index = 0;
    /** Iterate over entries to update if update indication is ON */
    if (action[1])
    {
        LOG_CLI((BSL_META("step2: Update entries is currently not supported\n")));
        action[1] = FALSE;
#if 0
        array_index = 0;
        /** Update the routes payload and flags */
        for (array_index = 0; array_index < total_entries; array_index++)
        {
            entry_info[array_index].entry_action[0].value[0] += 10;
        }

        array_index = 0;
        LOG_CLI((BSL_META("step2: Update entries\n")));
        utilex_ll_timer_set("bcm_field_entry_add update", timers_group, timer_idx[1]);

        for (run_index = 0; run_index < nof_runs; run_index++)
        {
            start_time = sal_time_usecs();
            for (entry_index = 0; entry_index < nof_entries; entry_index++)
            {
                rv = bcm_field_entry_add(unit, BCM_FIELD_FLAG_WITH_ID, fg_id, &entry_info[array_index],
                                         &entry_id[array_index]);

                if (rv != BCM_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "Error, bcm_field_entry_add failed update - entry index %d\n", array_index);
                }
                array_index++;
            }
            run_time[1][run_index] = sal_time_usecs() - start_time;
        }
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
#endif
    }

    /** Iterate over all entries and delete them if delete indication is set */
    if (action[2])
    {
        array_index = 0;
        LOG_CLI((BSL_META("step3: Delete entries\n")));

        utilex_ll_timer_set("bcm_field_entry_delete", timers_group, timer_idx[2]);

        for (run_index = 0; run_index < nof_runs; run_index++)
        {
            start_time = sal_time_usecs();
            for (entry_index = 0; entry_index < nof_entries; entry_index++)
            {
                rv = bcm_field_entry_delete(unit, fg_id, entry_info[array_index].entry_qual, entry_id[array_index]);

                if (rv != BCM_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "Error, bcm_field_entry_delete failed - entry index %d\n", array_index);
                }
                array_index++;
            }
            run_time[2][run_index] = sal_time_usecs() - start_time;
        }
        utilex_ll_timer_stop(timers_group, timer_idx[2]);
    }

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

exit:
    sal_free(entry_info);
    sal_free(entry_id);
    SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_performance_run_single(
    int unit,
    uint32 key_size,
    uint32 res_length,
    uint32 nof_entries,
    uint32 nof_runs,
    unsigned int action[3],
    int rate[3],
    uint8 skip_config,
    uint8 skip_deinit)
{
    int iter;
    char action_name[3][16] = { "Add", "Update", "Delete" };
    int run_time[3][100];

    bcm_field_qualify_t qual_list[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP] = { 0 };
    int nof_quals = 0;
    bcm_field_action_t action_list[BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP] = { 0 };
    int nof_actions = 0;
    bcm_field_group_t fg_id;

    uint8 threads_disabled = FALSE;
    uint8 error_recovery_disabled = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /** Disable threads */
    LOG_CLI((BSL_META("Disable threads\n")));
    threads_disabled = TRUE;
    CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("linkscan off");
    CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("counter off");
    CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("deinit rx_los");
    CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("deinit interrupt");
    CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("crps detach");

    LOG_CLI((BSL_META("\nRunning performance test on ACL with %db key and %db result, %d runs each with %d entries\n"),
             key_size, res_length, nof_runs, nof_entries));

    /** Disable error recovery */
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    error_recovery_disabled = TRUE;

    if (!skip_config)
    {
        /** Create the KBP ACL field group */
        SHR_IF_ERR_EXIT(dnx_kbp_acl_config_create
                        (unit, key_size, res_length, qual_list, &nof_quals, action_list, &nof_actions,
                         (nof_runs * nof_entries), &fg_id));
    }

    /** Perform the entries performance test */
    SHR_IF_ERR_EXIT(dnx_kbp_acl_performance_entries
                    (unit, nof_runs, nof_entries, qual_list, nof_quals, action_list, nof_actions, fg_id, action,
                     run_time));

    /** Print the measured rates and retrieve them for validation */
    for (iter = 0; iter < 3; iter++)
    {
        if (action[iter])
        {
            dnx_kbp_performance_detailed_measures_print(action_name[iter], run_time[iter], nof_runs, nof_entries,
                                                        &rate[iter]);
        }
    }

exit:
    if (error_recovery_disabled)
    {
        /** Enable error recovery */
        DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    }
    if (threads_disabled)
    {
        /** Enable threads */
        LOG_CLI((BSL_META("Enable threads\n")));
        CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("crps attach");
        CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("init interrupt");
        CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("init rx_los");
        CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("counter on");
        CTEST_DNX_KBP_SH_CMD_EXEC_AND_PRINT("linkscan on");
    }
    if (!skip_deinit)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_init_deinit(unit, NULL));
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_performance_validation(
    int unit,
    uint32 key_size,
    uint32 res_length,
    uint8 is_slk,
    unsigned int action[3],
    int rate[3])
{
    int iter;
    int exp_rate[3] = { 0, 0, 0 };
    double performance_tolerance = 0.20;
    char action_name[3][16] = { "Add", "Update", "Delete" };
    uint8 is_sanitization_performed = TRUE;

    SHR_FUNC_INIT_VARS(unit);


    SHR_IF_ERR_EXIT(dnx_kbp_acl_performance_expected_measures_get
                    (unit, is_slk, key_size, res_length, &exp_rate[0], &exp_rate[1], &exp_rate[2]));

    for (iter = 0; iter < 3; iter++)
    {
        if (action[iter])
        {
            if ((double) rate[iter] <= ((double) exp_rate[iter] * (1.0 + performance_tolerance)) &&
                (double) rate[iter] >= ((double) exp_rate[iter] * (1.0 - performance_tolerance)))
            {
                LOG_CLI((BSL_META("Reached the expected %s rate of %d within the expected %.2f%% tolerance (%d)\n"),
                         action_name[iter], exp_rate[iter], (100.0 * performance_tolerance), rate[iter]));
            }
            else if ((double) rate[iter] > ((double) exp_rate[iter] * (1.0 + performance_tolerance)))
            {
                LOG_CLI((BSL_META
                         ("Reached higher %s rate (%d) than the expected %d with %.2f%% tolerance (%.0f)\n"),
                         action_name[iter], rate[iter], exp_rate[iter], (100.0 * performance_tolerance),
                         ((double) exp_rate[iter] * (1.0 + performance_tolerance))));
                /** Return failure when the rate is higher than expected only if sanitization is not performed */
                if (!is_sanitization_performed)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
                }
            }
            else
            {
                LOG_CLI((BSL_META("Reached lower %s rate (%d) than the expected %d with %.2f%% tolerance (%.0f)\n"),
                         action_name[iter], rate[iter], exp_rate[iter], (100.0 * performance_tolerance),
                         ((double) exp_rate[iter] * (1.0 - performance_tolerance))));
                SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_entries = 0;
    uint32 nof_runs = 1;
    uint32 key_size = 0;
    uint32 res_length = 0;
    uint8 measure = FALSE, caching = FALSE, warmboot = FALSE, random = FALSE, skip_config = FALSE, skip_deinit = FALSE;
    uint8 slk = FALSE, layer_id;
    char *layer = CTEST_DNX_KBP_PERFORMANCE_LAYER_BCM;
    int rate[3] = { 0, 0, 0 };
    int rate_all[4][3][3];
    unsigned int action[3] = { 1, 0, 0 };
    uint8 run_all = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM || !dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Test is not supported in simulation or without KBP\n");
    }

    SH_SAND_GET_UINT32(CTEST_DNX_KBP_PERFORMANCE_ENTRIES, nof_entries);
    SH_SAND_GET_UINT32(CTEST_DNX_KBP_PERFORMANCE_RUNS, nof_runs);
    SH_SAND_GET_UINT32(CTEST_DNX_KBP_ACL_KEY_SIZE, key_size);
    SH_SAND_GET_UINT32(CTEST_DNX_KBP_ACL_RESULT_LENGTH, res_length);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MEASURE, measure);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_PERFORMANCE_CACHING, caching);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_PERFORMANCE_WARMBOOT, warmboot);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_PERFORMANCE_RANDOM, random);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_PERFORMANCE_UPDATE, action[1]);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_PERFORMANCE_DELETE, action[2]);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_ACL_SKIP_CONFIG, skip_config);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_ACL_SKIP_DEINIT, skip_deinit);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_PERFORMANCE_ALL, run_all);

    if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321)
    {
        /** Perform measures validation only on OP2 */
        measure = FALSE;
    }

    SHR_IF_ERR_EXIT(dnx_kbp_performance_only_test_scenario_get(unit, &layer, &layer_id, &slk, &measure));

    /** WB is currently disabled */
    if (warmboot != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Warmboot mode is not supported\n");
    }
    /** Caching is not being tested */
    if (caching != FALSE)
    {
        LOG_CLI((BSL_META("Warning: Caching is being ignored.\n")));
    }
    /** Randomized routes are not being used */
    if (random != FALSE)
    {
        LOG_CLI((BSL_META("Warning: Random is being ignored.\n")));
    }

    if (nof_runs < 1 || nof_runs > 100)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of runs need to be between 1 and 100\n");
    }
    if (nof_entries == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of entries is 0\n");
    }

    if (run_all)
    {
        int key_size_index, res_length_index;
        int key_sizes[4] = { 80, 160, 320, 360 };
        int res_lengths[3] = { 32, 64, 128 };
        sal_memset(rate_all, 0x0, sizeof(int) * 4 * 3 * 3);

        if (skip_config)
        {
            LOG_CLI((BSL_META("Warning: ACL configuration will not be skipped.\n")));
            skip_config = FALSE;
        }
        if (skip_deinit)
        {
            LOG_CLI((BSL_META("Warning: BCM deinit-init will not be skipped.\n")));
            skip_deinit = FALSE;
        }

        /** Run all tests */
        /** Do not perform testing on 360b key (index 4), because 360b key size is no longer valid */
        for (key_size_index = 0; key_size_index < 3; key_size_index++)
        {
            for (res_length_index = 0; res_length_index < 3; res_length_index++)
            {
                SHR_IF_ERR_EXIT(dnx_kbp_acl_performance_run_single(unit, key_sizes[key_size_index],
                                                                   res_lengths[res_length_index], nof_entries, nof_runs,
                                                                   action, rate_all[key_size_index][res_length_index],
                                                                   skip_config, skip_deinit));
            }
        }

        /** Validate if the rates meet the expected rates */
        if (measure)
        {
            shr_error_e rv = _SHR_E_NONE;
            /** Do not perform testing on 360b key (index 4), because 360b key size is no longer valid */
            for (key_size_index = 0; key_size_index < 3; key_size_index++)
            {
                for (res_length_index = 0; res_length_index < 3; res_length_index++)
                {
                    LOG_CLI((BSL_META("\nValidating measurements on ACL ")));
                    LOG_CLI((BSL_META("with %db key and %db result, %d runs each with %d entries\n"),
                             key_sizes[key_size_index], res_lengths[res_length_index], nof_runs, nof_entries));

                    rv = dnx_kbp_acl_performance_validation(unit, key_sizes[key_size_index],
                                                            res_lengths[res_length_index], slk, action,
                                                            rate_all[key_size_index][res_length_index]);
                    if (rv != _SHR_E_NONE)
                    {
                        SHR_SET_CURRENT_ERR(rv);
                    }
                }
            }
        }
        else
        {
            LOG_CLI((BSL_META("Measurements validation is not performed\n")));
        }
    }
    else
    {
        /** Run single test */
        SHR_IF_ERR_EXIT(dnx_kbp_acl_performance_run_single(unit, key_size, res_length, nof_entries, nof_runs,
                                                           action, rate, skip_config, skip_deinit));
        if (measure)
        {
            SHR_SET_CURRENT_ERR(dnx_kbp_acl_performance_validation(unit, key_size, res_length, slk, action, rate));
        }
        else
        {
            LOG_CLI((BSL_META("Measurements validation is not performed\n")));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**********************************************************************************************************************/
/* *INDENT-OFF* */
static sh_sand_option_t dnx_kbp_acl_performance_options[] = {
    {CTEST_DNX_KBP_PERFORMANCE_ENTRIES, SAL_FIELD_TYPE_UINT32, "Number of entries", "1500"},
    {CTEST_DNX_KBP_PERFORMANCE_RUNS, SAL_FIELD_TYPE_UINT32, "Number of runs", "10"},
    {CTEST_DNX_KBP_ACL_KEY_SIZE, SAL_FIELD_TYPE_UINT32, "ACL key size", "80"},
    {CTEST_DNX_KBP_ACL_RESULT_LENGTH, SAL_FIELD_TYPE_UINT32, "ACL result length", "32"},
    {CTEST_DNX_KBP_MEASURE, SAL_FIELD_TYPE_BOOL, "Measure indication", "NO"},
    {CTEST_DNX_KBP_PERFORMANCE_CACHING, SAL_FIELD_TYPE_BOOL, "Caching indication", "NO"},
    {CTEST_DNX_KBP_PERFORMANCE_WARMBOOT, SAL_FIELD_TYPE_BOOL, "Warmboot indication", "NO"},
    {CTEST_DNX_KBP_PERFORMANCE_RANDOM, SAL_FIELD_TYPE_BOOL, "Random indication", "NO"},
    {CTEST_DNX_KBP_PERFORMANCE_UPDATE, SAL_FIELD_TYPE_BOOL, "Update indication", "YES"},
    {CTEST_DNX_KBP_PERFORMANCE_DELETE, SAL_FIELD_TYPE_BOOL, "Delete indication", "YES"},
    {CTEST_DNX_KBP_ACL_SKIP_CONFIG, SAL_FIELD_TYPE_BOOL, "Skip configuration", "no"},
    {CTEST_DNX_KBP_ACL_SKIP_DEINIT, SAL_FIELD_TYPE_BOOL, "Skip deinit", "no"},
    {CTEST_DNX_KBP_PERFORMANCE_ALL, SAL_FIELD_TYPE_BOOL, "Run all tests", "no"},
    {NULL}
};

static sh_sand_man_t dnx_kbp_acl_performance_man = {
    "Create, update and delete ACL entries",
    "Create, update and delete ACL entries while recording how much time each step takes",
    "",
    ""
};

static sh_sand_invoke_t dnx_kbp_acl_performance_tests[] = {
    {"ALL", CTEST_DNX_KBP_PERFORMANCE_ALL " " CTEST_DNX_KBP_MEASURE, CTEST_POSTCOMMIT},
    {NULL}
};

/* *INDENT-ON* */

static shr_error_e
dnx_kbp_acl_capacity_entries(
    int unit,
    bcm_field_qualify_t * qual_list,
    int nof_quals,
    bcm_field_action_t * action_list,
    int nof_actions,
    bcm_field_group_t fg_id,
    int *capacity,
    int nof_entries_to_cache)
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

    /** Add entries until failure */
    while (TRUE)
    {
        for (index = 0; index < nof_quals; index++)
        {
            entry_info.entry_qual[index].value[0]++;
        }

        for (index = 0; index < nof_actions; index++)
        {
            entry_info.entry_action[index].value[0]++;
        }
        iter++;

        rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id);
        if (nof_entries_to_cache > 0 && rv == BCM_E_NONE && iter == nof_entries_to_cache)
        {
            /**  The number of cached entries can't exceed real capacity. Or it will trigger error recovery in cache Install */
            LOG_CLI((BSL_META("%d entries added to cache\n"), iter));
            break;
        }
        if (rv != BCM_E_NONE)
        {
            if (rv != BCM_E_FULL)
            {
                SHR_ERR_EXIT(rv, "Error, bcm_field_entry_add failed - entry index %d\n", iter);
            }
            else
            {
                LOG_CLI((BSL_META("Table full after %d entries\n"), iter));
                break;
            }
        }
        /** Progression print */
        if (iter % 16384 == 0)
        {
            LOG_CLI((BSL_META("%d entries\n"), iter));
        }
    }

    *capacity = iter;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_acl_capacity_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_qualify_t qual_list[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP] = { 0 };
    int nof_quals = 0;
    bcm_field_action_t action_list[BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP] = { 0 };
    int nof_actions = 0;
    bcm_field_group_t fg_id;

    int capacity = 0;
    double capacity_tolerance = 0.05;
    int expected_capacity = 0;
    int cache_capacity = 0;
    uint32 key_size = 0;
    uint32 res_length = 0;
    uint8 measure = FALSE, skip_config = FALSE, skip_deinit = FALSE, cache_commit = FALSE;
    int start_time = 0;
    char *layer = CTEST_DNX_KBP_PERFORMANCE_LAYER_BCM;
    uint8 slk = FALSE, layer_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM || !dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Test is not supported without KBP\n");
    }

    SH_SAND_GET_UINT32(CTEST_DNX_KBP_ACL_KEY_SIZE, key_size);
    SH_SAND_GET_UINT32(CTEST_DNX_KBP_ACL_RESULT_LENGTH, res_length);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_MEASURE, measure);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_ACL_SKIP_CONFIG, skip_config);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_ACL_SKIP_DEINIT, skip_deinit);
    SH_SAND_GET_BOOL(CTEST_DNX_KBP_ACL_CACHE_COMMIT, cache_commit);

    if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321)
    {
        /** Perform measures validation only on OP2 */
        measure = FALSE;
    }
    if (dnx_data_elk.application.split_rpf_get(unit) == 0)
    {
        /** Skip measures validation for non-split RPF */
        measure = FALSE;
    }

    SHR_IF_ERR_EXIT(dnx_kbp_performance_only_test_scenario_get(unit, &layer, &layer_id, &slk, &measure));
    SHR_IF_ERR_EXIT(dnx_kbp_acl_capacity_expected_measures_get(unit, key_size, res_length, &expected_capacity));

    if (!skip_config)
    {
        /** Create the KBP ACL field group */
        SHR_IF_ERR_EXIT(dnx_kbp_acl_config_create
                        (unit, key_size, res_length, qual_list, &nof_quals, action_list, &nof_actions, 0, &fg_id));
    }

    start_time = sal_time_usecs();
    if (cache_commit)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_cache(unit, 0, fg_id, bcmFieldGroupCacheModeStart));
        LOG_CLI((BSL_META("CacheMode was set to Start\n")));

        /** Compute the number of entries for cache. It will fail to install if cached entries exceed KBP capacity */
        cache_capacity = (int) ((double) expected_capacity * (1.0 - capacity_tolerance)) + 1;
    }
    SHR_IF_ERR_EXIT(dnx_kbp_acl_capacity_entries
                    (unit, qual_list, nof_quals, action_list, nof_actions, fg_id, &capacity, cache_capacity));
    if (cache_commit)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_cache(unit, 0, fg_id, bcmFieldGroupCacheModeInstall));
        LOG_CLI((BSL_META("CacheMode was set to Install\n")));
    }
    LOG_CLI((BSL_META("Finished filling the table for %ds\n\n"), ((sal_time_usecs() - start_time) / (1000 * 1000))));

    /** Validate if the measured capacity meets the expected capacity */
    if (measure)
    {
        if ((double) capacity <= ((double) expected_capacity * (1.0 + capacity_tolerance)) &&
            (double) capacity >= ((double) expected_capacity * (1.0 - capacity_tolerance)))
        {
            LOG_CLI((BSL_META("Reached the expected capacity of %d within the expected %.2f%% tolerance (%d)\n"),
                     expected_capacity, (100.0 * capacity_tolerance), capacity));
        }
        else if ((double) capacity > ((double) expected_capacity * (1.0 + capacity_tolerance)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Reached higher capacity than the expected %d with %.2f%% tolerance (%d)\n",
                         expected_capacity, (100.0 * capacity_tolerance), capacity);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not reach the expected capacity of %d with %.2f%% tolerance (%d)\n",
                         expected_capacity, (100.0 * capacity_tolerance), capacity);
        }
    }
    else
    {
        LOG_CLI((BSL_META("Measurements validation is not performed\n")));
    }

    if (!skip_deinit)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_init_deinit(unit, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
static sh_sand_option_t dnx_kbp_acl_capacity_options[] = {
    {CTEST_DNX_KBP_ACL_KEY_SIZE, SAL_FIELD_TYPE_UINT32, "ACL key size", "80"},
    {CTEST_DNX_KBP_ACL_RESULT_LENGTH, SAL_FIELD_TYPE_UINT32, "ACL result length", "32"},
    {CTEST_DNX_KBP_MEASURE, SAL_FIELD_TYPE_BOOL, "Measure indication", "NO"},
    {CTEST_DNX_KBP_ACL_SKIP_CONFIG, SAL_FIELD_TYPE_BOOL, "Skip configuration", "NO"},
    {CTEST_DNX_KBP_ACL_SKIP_DEINIT, SAL_FIELD_TYPE_BOOL, "Skip deinit", "NO"},
    {CTEST_DNX_KBP_ACL_CACHE_COMMIT, SAL_FIELD_TYPE_BOOL, "Cache commit", "NO"},
    {NULL}
};

static sh_sand_man_t dnx_kbp_acl_capacity_man = {
    "Add ACL entries and compare against expected capacity",
    "Add ACL entries and compare against expected capacity",
    "",
    ""
};

static sh_sand_invoke_t dnx_kbp_acl_capacity_tests[] = {
    {"K80_R32", CTEST_DNX_KBP_ACL_KEY_SIZE "=80 " CTEST_DNX_KBP_ACL_RESULT_LENGTH "=32 " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {"K160_R32", CTEST_DNX_KBP_ACL_KEY_SIZE "=160 " CTEST_DNX_KBP_ACL_RESULT_LENGTH "=32 " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {"K320_R32", CTEST_DNX_KBP_ACL_KEY_SIZE "=320 " CTEST_DNX_KBP_ACL_RESULT_LENGTH "=32 " CTEST_DNX_KBP_MEASURE, CTEST_POSTCOMMIT},
#if 0
    /** Disable the below tests, because 360b key size is no longer valid */
    {"K360_R32", CTEST_DNX_KBP_ACL_KEY_SIZE "=360 " CTEST_DNX_KBP_ACL_RESULT_LENGTH "=32 " CTEST_DNX_KBP_MEASURE, CTEST_POSTCOMMIT},
#endif
    {"CACHE_K80_R32", CTEST_DNX_KBP_ACL_KEY_SIZE "=80 " CTEST_DNX_KBP_ACL_RESULT_LENGTH "=32 " CTEST_DNX_KBP_ACL_CACHE_COMMIT "=YES " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {"CACHE_K160_R32", CTEST_DNX_KBP_ACL_KEY_SIZE "=160 " CTEST_DNX_KBP_ACL_RESULT_LENGTH "=32 " CTEST_DNX_KBP_ACL_CACHE_COMMIT "=YES " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {"CACHE_K320_R32", CTEST_DNX_KBP_ACL_KEY_SIZE "=320 " CTEST_DNX_KBP_ACL_RESULT_LENGTH "=32 " CTEST_DNX_KBP_ACL_CACHE_COMMIT "=YES " CTEST_DNX_KBP_MEASURE, CTEST_WEEKEND},
    {NULL}
};

sh_sand_cmd_t dnx_kbp_acl_cmds[] = {
    {"TeST", dnx_kbp_acl_kbp_mngr_cmd, NULL, dnx_kbp_acl_kbp_mngr_cmd_options, &dnx_kbp_acl_kbp_mngr_man, NULL, dnx_kbp_acl_kbp_mngr_tests, .label="kbp"},
    {"PERFormance", dnx_kbp_acl_performance_cmd, NULL, dnx_kbp_acl_performance_options, &dnx_kbp_acl_performance_man, NULL, dnx_kbp_acl_performance_tests, .label="kbp"},
    {"Capacity", dnx_kbp_acl_capacity_cmd, NULL, dnx_kbp_acl_capacity_options, &dnx_kbp_acl_capacity_man, NULL, dnx_kbp_acl_capacity_tests, .label="kbp"},
    {NULL}
};
sh_sand_option_t dnx_kbp_acl_options[] = {
    {NULL}
};
sh_sand_invoke_t dnx_kbp_acl_tests[] = {
    {NULL}
};
sh_sand_man_t dnx_kbp_acl_man = {
    "KBP manager ACL related tests",
    "KBP manager ACL related tests",
    "",
    ""
};
/* *INDENT-ON* */

/**********************************************************************************************************************/
/** Combo */
/**********************************************************************************************************************/

/*
 * See dnx_kbp_combo_test_man for more information on the test case
 */
static shr_error_e
dnx_kbp_combo_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t ctest_soc_property[] = {
        {"ext_ipv4_fwd_enable", "1"},
        {"ext_ipv6_fwd_enable", "1"},
        {"ext_tcam_dev_type", "BCM52321"},
        {"custom_feature_kbp_blackhole", "1"},
        {NULL}
    };

    char *input_xml = NULL;
    char *output = NULL;
    uint8 clear = TRUE;
    uint8 configure = TRUE;
    uint8 init_soc = TRUE;
    int test_id = -1;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("Source", input_xml);
    SH_SAND_GET_STR("PATH", output);
    SH_SAND_GET_BOOL("Clear", clear);
    SH_SAND_GET_BOOL("Configure", configure);
    SH_SAND_GET_BOOL("InitSOCProperties", init_soc);
    SH_SAND_GET_INT32("Test", test_id);

    if (init_soc)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    if (configure)
    {
        uint32 kbp_mngr_status;
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "KBP device is already locked; Combo test will not run\n")));
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(dnx_kbp_combo_test(unit, input_xml, output, test_id, configure));

exit:
    if (clear)
    {
        ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    }
    SHR_FUNC_EXIT;
}

/*
 * See dnx_kbp_combo_info_man for more information on the test case
 */
static shr_error_e
dnx_kbp_combo_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *input_xml = NULL;
    char *output = NULL;
    int test_id = -1;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("Source", input_xml);
    SH_SAND_GET_STR("PATH", output);
    SH_SAND_GET_INT32("Test", test_id);

    SHR_IF_ERR_EXIT(dnx_kbp_combo_info(unit, input_xml, output, test_id));

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_man_t dnx_kbp_combo_test_man = {
    "Run the KBP combo test",
    "The KBP combo test environment will read and apply the group and opcode configurations and will run the test scenarios",
    "",
    ""
};

sh_sand_option_t dnx_kbp_combo_test_options[] = {
    {"Source", SAL_FIELD_TYPE_STR, "File path to the input XML file (leave empty to use the default test XML file)", ""},
    {"PATH", SAL_FIELD_TYPE_STR, "Output to file (stdout to print to the screen)", "stdout"},
    {"Clear", SAL_FIELD_TYPE_BOOL, "Reset device to clear all test configurations", "TRUE"},
    {"Configure", SAL_FIELD_TYPE_BOOL, "Perform groups and opcodes configuration and KBP sync", "TRUE"},
    {"InitSOCProperties", SAL_FIELD_TYPE_BOOL, "Initialize the default OP2 SOC properties", "TRUE"},
    {"Test", SAL_FIELD_TYPE_INT32, "Indicates specific test to run (-1 for all tests)", "-1"},
    {NULL}
};

sh_sand_man_t dnx_kbp_combo_info_man = {
    "Dumps information about the combo test cases",
    "The KBP combo test environment will read the test scenarios and will output information for them",
    "",
    ""
};

sh_sand_option_t dnx_kbp_combo_info_options[] = {
    {"Source", SAL_FIELD_TYPE_STR, "File path to the input XML file (leave empty to use the default test XML file)", ""},
    {"PATH", SAL_FIELD_TYPE_STR, "Output to file (stdout to print to the screen)", "stdout"},
    {"Test", SAL_FIELD_TYPE_INT32, "Dumps the information for a given test (-1 for all tests)", "-1"},
    {NULL}
};

sh_sand_cmd_t dnx_kbp_combo_cmds[] = {
    {"Test", dnx_kbp_combo_test_cmd, NULL, dnx_kbp_combo_test_options, &dnx_kbp_combo_test_man, .label="kbp"},
    {"Info", dnx_kbp_combo_info_cmd, NULL, dnx_kbp_combo_info_options, &dnx_kbp_combo_info_man, .label="kbp"},
    {NULL}
};

sh_sand_option_t dnx_kbp_combo_options[] = {
    {NULL}
};

sh_sand_invoke_t dnx_kbp_combo_tests[] = {
    {"KBP_combo_Test", "Test", CTEST_PRECOMMIT},
    {NULL}
};

sh_sand_man_t dnx_kbp_combo_man = {
    "KBP testing environment which executes tests from an XML configuration file",
    "  The KBP combo test environment aims to ease the implementation of tests and to improve on the execution time by "
    "removing the need for performing deinit-init after each test scenario because of the device lock.\n"
    "The environment reads the XML configuration and performs the testing in two stages: configuration and execution.\n"
    "  1. Configuration stage\n"
    "The environment reads the XML file for all groups and opcodes and configures them one by one. "
    "In case of failure during this stage, all groups and opcodes that are already configured are deleted. This "
    "way everything will be reverted to the previous clean state.\n"
    "The configuration stage ends with KBP sync. In case of failure the previous clean state cannot be guaranteed. "
    "After this stage is completed all required configurations for the execution stage are done.\n"
    "  2. Execution stage\n"
    "The environment reads the XML file for all test cases and executes them one by one.\n"
    "Each test case is comprised of steps which are executed in the given order. The steps are divided to two types: "
    "entry management related actions and KBP searches that simulate traffic.\n"
    "In case of failure (and also when the test scenario finishes) the environment removes all entries in order "
    "to revert to clean state.\n"
    "  The environment keeps track of the state of all test cases and prints their status when the combo test finishes.",
    "",
    ""
};
/* *INDENT-ON* */

/**********************************************************************************************************************/
/* *INDENT-OFF* */

sh_sand_man_t dnx_kbp_stif_man = {
    "KBP manager STIF related tests",
    "two options: 1. running full test, using /'test'/ command. 2. running test by several steps, using /'config'/, /'verify'/, /'destroy'/ commands",
    "",
    ""
};

sh_sand_cmd_t dnx_kbp_test_cmds[] = {
    {"ACL", NULL, dnx_kbp_acl_cmds, dnx_kbp_acl_options, &dnx_kbp_acl_man, NULL, dnx_kbp_acl_tests, .label="kbp"},
    {"CoMBo", NULL, dnx_kbp_combo_cmds, dnx_kbp_combo_options, &dnx_kbp_combo_man, NULL, dnx_kbp_combo_tests, .label="kbp"},
    {"FWD", NULL, dnx_kbp_fwd_cmds, dnx_kbp_fwd_options, &dnx_kbp_fwd_man, NULL, dnx_kbp_fwd_tests, .label="kbp"},/* must be after ACLs since ACLs performing lock*/
    {"STIF", NULL, sh_dnx_kbp_stif_test_cmds, NULL, &dnx_kbp_stif_man,.label="kbp"},
    {"MultiRstType", NULL, dnx_kbp_multi_result_types_cmds, NULL, &dnx_kbp_multi_result_types_man, .label="kbp"},
    {NULL}
};
/* *INDENT-ON* */
