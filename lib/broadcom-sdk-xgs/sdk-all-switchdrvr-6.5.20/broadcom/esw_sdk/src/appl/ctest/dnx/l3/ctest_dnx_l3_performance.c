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
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

 /*
  * Include files.
  * {
  */
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
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
#include <bcm_int/dnx/algo/l3/algo_l3.h>
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

/*
 * }
 */
/*
 * DEFINEs
 * {
 */

/**
 * \brief
 */
/** Input parameters for the performance tests */
#define CTEST_DNX_L3_PERFORMANCE_ENTRIES                "Entries"
#define CTEST_DNX_L3_PERFORMANCE_RUNS                   "Runs"
#define CTEST_DNX_L3_PERFORMANCE_UPDATE                 "Update"
#define CTEST_DNX_L3_PERFORMANCE_DELETE                 "DeLeTe"
#define CTEST_DNX_L3_PERFORMANCE_RANDOM                 "Random"
#define CTEST_DNX_L3_PERFORMANCE_VALIDATE               "Validate"
#define CTEST_DNX_L3_PERFORMANCE_UPDATE_MEASURES        "Update_measures"
#define CTEST_DNX_L3_PERFORMANCE_PUBLIC                 "Public"
#define CTEST_DNX_L3_PERFORMANCE_RPF                    "RPF"
#define CTEST_DNX_L3_PERFORMANCE_TCAM                   "TCAM"
#define CTEST_DNX_L3_PERFORMANCE_EM                     "EM"
#define CTEST_DNX_L3_PERFORMANCE_LAYER                  "Layer"
#define CTEST_DNX_L3_PERFORMANCE_ALL                    "All"

#define CTEST_DNX_L3_PERFORMANCE_LAYER_BCM              "BCM"
#define CTEST_DNX_L3_PERFORMANCE_LAYER_DBAL             "DBAL"
#define CTEST_DNX_L3_PERFORMANCE_LAYER_DBAL_FORCE       "DBAL_FORCE"
#define CTEST_DNX_L3_PERFORMANCE_LAYER_ACCESS           "Access"

#define CTEST_DNX_L3_PERFORMANCE_LAYER_ID_BCM           0
#define CTEST_DNX_L3_PERFORMANCE_LAYER_ID_DBAL          1
#define CTEST_DNX_L3_PERFORMANCE_LAYER_ID_DBAL_FORCE    2
#define CTEST_DNX_L3_PERFORMANCE_LAYER_ID_ACCESS        3

#define CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH "l3/ctest_dnx_l3_performance_measurements.xml"

#define CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT(_cmd_) \
    LOG_CLI((BSL_META("%s\n"), _cmd_)); sh_process_command(unit, _cmd_)
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/** Utility structure for populating the required L3 DBAL fields for DBAL performance measuring */
typedef struct
{
    uint32 vrf;
    uint32 destination;
    uint32 in_lif;
    uint32 out_lif;
    uint32 dip;
    uint32 sip;
    uint8 dip6[16];
    uint8 sip6[16];
} dnx_l3_performance_dbal_entry_info_t;
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * }
 */

/**********************************************************************************************************************/
/** FWD */
/**********************************************************************************************************************/

static uint8
dnx_l3_is_little_endian(
    void)
{
    uint32 a = 1;
    uint8 *b = (uint8 *) &a;
    return *b;
}

/** Utility API for rounding a number up or down to a specified granularity */
static int
dnx_l3_performance_round_to(
    int number,
    int round_to)
{
    return (((number / round_to) * round_to) + (((number % round_to) / (round_to / 2)) * round_to));
}

/** The state for the pseudo-random number generator */
static uint64 dnx_l3_performance_random_state = COMPILER_64_INIT(0, 1);

/** API for seeding the state of the pseudo-random number generator */
static void
dnx_l3_performance_random_seed(
    uint64 seed)
{
    COMPILER_64_COPY(dnx_l3_performance_random_state, seed);
}

/** APIs for fast generating of 64b pseudo-random numbers (splitmix64) */
uint64
dnx_l3_performance_random_get_64(
    void)
{
    uint64 number;
    uint64 tmp_64_a;
    uint64 tmp_64_b;

    /** number = (dnx_l3_performance_random_state += (0x9E3779B97F4A7C15)); */
    COMPILER_64_SET(number, 0x9E3779B9, 0x7F4A7C15);
    COMPILER_64_ADD_64(dnx_l3_performance_random_state, number);
    COMPILER_64_COPY(number, dnx_l3_performance_random_state);

    /** number = (number ^ (number >> 30)) * (0xBF58476D1CE4E5B9); */

    /** tmp_64_a = (number ^ (number >> 30)); */
    COMPILER_64_COPY(tmp_64_a, number);
    COMPILER_64_SHR(tmp_64_a, 30);
    COMPILER_64_XOR(tmp_64_a, number);
    /*
     * number = tmp_64_a * 0xBF58476D1CE4E5B9;
     */
    COMPILER_64_COPY(tmp_64_b, tmp_64_a);
    COMPILER_64_UMUL_32(tmp_64_a, 0xBF58476D);
    COMPILER_64_UMUL_32(tmp_64_b, 0x1CE4E5B9);
    COMPILER_64_SHL(tmp_64_a, 32);
    COMPILER_64_ADD_64(tmp_64_a, tmp_64_b);
    COMPILER_64_COPY(number, tmp_64_a);

    /** number = (number ^ (number >> 27)) * (0x94D049BB133111EB); */

    /** tmp_64_a = (number ^ (number >> 27)); */
    COMPILER_64_COPY(tmp_64_a, number);
    COMPILER_64_SHR(tmp_64_a, 27);
    COMPILER_64_XOR(tmp_64_a, number);
    /** number = tmp_64_a * 0x94D049BB133111EB; */
    COMPILER_64_COPY(tmp_64_b, tmp_64_a);
    COMPILER_64_UMUL_32(tmp_64_a, 0x94D049BB);
    COMPILER_64_UMUL_32(tmp_64_b, 0x133111EB);
    COMPILER_64_SHL(tmp_64_a, 32);
    COMPILER_64_ADD_64(tmp_64_a, tmp_64_b);
    COMPILER_64_COPY(number, tmp_64_a);

    /** number = (number ^ (number >> 31); */
    COMPILER_64_COPY(tmp_64_a, number);
    COMPILER_64_SHR(tmp_64_a, 31);
    COMPILER_64_XOR(tmp_64_a, number);
    COMPILER_64_COPY(number, tmp_64_a);

    return number;
}

/** API for for truncating the 64b pseudo-random number to 32b */
static uint32
dnx_l3_performance_random_get(
    void)
{
    uint64 number;
    uint32 ret;

    number = dnx_l3_performance_random_get_64();
    ret = COMPILER_64_LO(number);
    return (ret);
}

/*
 * Utility function for populating and array of sequential numbers (IPv4 addresses)
 * and shuffling them using them using the random function.
 * This way no IPv4 address used in the performance measurements will be duplicated.
 */
static void
dnx_l3_performance_random_ipv4_generate(
    uint32 *ipv4,
    uint32 entries)
{
    int i;

    for (i = 0; i < entries; i++)
    {
        ipv4[i] = i;
    }
    for (i = 0; i < entries; i++)
    {
        int tmp_i = dnx_l3_performance_random_get() % entries;
        uint32 tmp = ipv4[i];
        ipv4[i] = ipv4[tmp_i];
        ipv4[tmp_i] = tmp;
    }
}

/** A function to generate a non-consecutive unique sequence of given number of FEC IDs */
static shr_error_e
dnx_l3_performance_fec_ids_generate(
    int unit,
    int *fec_ids,
    uint32 nof_entries,
    int *hierarchy)
{
    int id;
    int hier = -1;
    uint32 base_fec;
    uint32 fec_range_size;
    int fec_count = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (id = 0; id < nof_entries; id++)
    {
        if (hier > 2 || hier == -1)
        {
            if (hier == -1)
            {
                hier = dnx_l3_performance_random_get() % 3;
            }
            else
            {
                hier = 0;
            }
            SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get(unit, hier, &base_fec, &fec_range_size));
        }

        fec_ids[id] = base_fec + fec_count;
        hierarchy[id] = hier;
        if (base_fec + fec_count == base_fec + fec_range_size)
        {
            hier++;
            fec_count = 0;
        }
        else
        {
            fec_count++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** Utility API for retrieving the device and revision as a string */
static char *
dnx_l3_performance_device_and_revision_get(
    int unit)
{
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        return "JR2_A0";
    }
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        return "JR2_B0";
    }
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        return "JR2_B1";
    }
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        return "Q2A_A0";
    }
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        return "Q2A_B0";
    }
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        return "Q2A_B1";
    }
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        return "J2C_A0";
    }
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        return "J2C_A1";
    }
    return NULL;
}

/** Used to retrieve the expected performance rates */
static shr_error_e
dnx_l3_performance_expected_performance_measurements_get_or_update(
    int unit,
    uint8 do_update_measures,
    uint8 is_host,
    uint8 is_ipmc,
    uint8 is_ipv6,
    uint8 is_external,
    uint8 is_rpf,
    uint8 is_tcam,
    uint8 is_em,
    uint8 is_fec,
    int *add_rate,
    int *update_rate,
    int *delete_rate)
{
    void *root = NULL;
    void *cpu_node = NULL;
    void *location_node = NULL;
    void *ip_node = NULL;
    void *table_node = NULL;

    char *cpu_name = dnx_l3_is_little_endian()? "Intel" : "SLK";
    char *location_name = is_external ? "External" : "Internal";
    char *ip_name = is_ipv6 ? "IPv6" : "IPv4";
    char *table_name = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Decide the table name from the input parameters */
    if (is_fec)
    {
        table_name = "FEC";
    }
    else if (is_host)
    {
        table_name = "Host";
    }
    else if (is_ipmc)
    {
        if (is_tcam)
        {
            table_name = "IPMC_TCAM";
        }
        else if (is_em)
        {
            table_name = "IPMC_EM";
        }
        else
        {
            table_name = "IPMC_LPM";
        }
    }
    else if (is_rpf)
    {
        table_name = "Route_RPF";
    }
    else
    {
        table_name = "Route";
    }

    root = dbx_file_get_xml_top(unit, CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH,
                                "CtestL3PerformanceMeasurementsRoot", CONF_OPEN_PER_DEVICE);
    if (root == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find L3 performance measurements xml file: %s\n",
                     CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH);
    }

    cpu_node = dbx_xml_child_get_first(root, cpu_name);
    if (cpu_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     cpu_name, CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH);
    }
    if (is_fec == 0)
    {
        location_node = dbx_xml_child_get_first(cpu_node, location_name);
        if (location_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                         location_name, CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH);
        }

        ip_node = dbx_xml_child_get_first(location_node, ip_name);
        if (ip_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                         ip_name, CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH);
        }

    }
    else
    {
        char *fec_hit_name =
            (dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit)) ? "FEC_no_hit" : "FEC_hit";
        ip_node = dbx_xml_child_get_first(cpu_node, fec_hit_name);
        if (ip_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                         fec_hit_name, CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH);
        }
    }
    table_node = dbx_xml_child_get_first(ip_node, table_name);
    if (table_node == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                     table_name, CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH);
    }

    /** EM and TCAM tables show different performance between devices and revisions */
    if ((is_host || is_em || is_tcam) && !is_external)
    {
        char *revision = dnx_l3_performance_device_and_revision_get(unit);
        if (revision == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Valid device and revision not found\n");
        }
        table_node = dbx_xml_child_get_first(table_node, revision);
        if (table_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n",
                         revision, CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH);
        }
    }

    if (do_update_measures)
    {
        int round_to = 500;
        if (add_rate)
        {
            RHDATA_MOD_INT(table_node, "Add", dnx_l3_performance_round_to(*add_rate, round_to));
        }
        if (update_rate)
        {
            RHDATA_MOD_INT(table_node, "Update", dnx_l3_performance_round_to(*update_rate, round_to));
        }
        if (delete_rate)
        {
            RHDATA_MOD_INT(table_node, "Delete", dnx_l3_performance_round_to(*delete_rate, round_to));
        }
        dbx_file_save_xml(unit, CTEST_DNX_L3_PERFORMANCE_MEASUREMENTS_XML_FILE_PATH, root, CONF_OPEN_PER_DEVICE);
    }
    else
    {
        if (add_rate)
        {
            RHDATA_GET_INT_STOP(table_node, "Add", *add_rate);
        }
        if (update_rate)
        {
            RHDATA_GET_INT_STOP(table_node, "Update", *update_rate);
        }
        if (delete_rate)
        {
            RHDATA_GET_INT_STOP(table_node, "Delete", *delete_rate);
        }
    }

exit:
    dbx_xml_top_close(root);
    SHR_FUNC_EXIT;
}

/** This API updates the validation indication to FALSE if the device ID is present in the unsupported devices list */
static shr_error_e
dnx_l3_performance_validation_check_for_device(
    int unit,
    uint8 *do_validation,
    uint8 *do_update_measures)
{
    int iter;
    uint32 device_id = dnx_data_device.general.device_id_get(unit);
    int nof_unsupported_device_ids = 1;
    uint32 unsupported_device_id_list[1] = {
        0x8821,
    };

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < nof_unsupported_device_ids; iter++)
    {
        if (device_id == unsupported_device_id_list[iter])
        {
            *do_validation = *do_update_measures = FALSE;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** This API parses the input layer parameter and update validation requirements */
static shr_error_e
dnx_l3_performance_layer_parameters_update(
    int unit,
    char *layer,
    uint8 *layer_id,
    uint8 *do_validation,
    uint8 *do_update_measures)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Validate the layer from which the performance testing starts - BCM, DBAL, ACCESS */
    if (!strncasecmp(layer, CTEST_DNX_L3_PERFORMANCE_LAYER_BCM, 16))
    {
        *layer_id = CTEST_DNX_L3_PERFORMANCE_LAYER_ID_BCM;
    }
    else if (!strncasecmp(layer, CTEST_DNX_L3_PERFORMANCE_LAYER_DBAL, 16))
    {
        /** Do not validate performance measurements for DBAL */
        *do_validation = *do_update_measures = FALSE;
        *layer_id = CTEST_DNX_L3_PERFORMANCE_LAYER_ID_DBAL;
    }
    else if (!strncasecmp(layer, CTEST_DNX_L3_PERFORMANCE_LAYER_DBAL_FORCE, 16))
    {
        /** Do not validate performance measurements for DBAL */
        *do_validation = *do_update_measures = FALSE;
        *layer_id = CTEST_DNX_L3_PERFORMANCE_LAYER_ID_DBAL_FORCE;
    }
#if 0
    else if (!strncasecmp(layer, CTEST_DNX_L3_PERFORMANCE_LAYER_ACCESS, 16))
    {
        /** Do not validate performance measurements for Access */
        *do_validation = *do_update_measures = FALSE;
        *layer_id = CTEST_DNX_L3_PERFORMANCE_LAYER_ID_ACCESS;
    }
#endif
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown performance layer command \"%s\".\n", layer);
    }

exit:
    SHR_FUNC_EXIT;
}

/** This API parses and validates the test input parameters and updates some of them according to the configuration */
static shr_error_e
dnx_l3_performance_input_parameters_validation_and_update(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    uint8 is_host,
    uint8 is_ipmc,
    uint8 is_ipv6,
    uint32 *nof_entries,
    uint32 *nof_runs,
    uint8 *update,
    uint8 *delete,
    uint8 *is_random,
    uint8 *do_validation,
    uint8 *do_update_measures,
    uint8 *is_external,
    uint8 *is_public,
    uint8 *is_rpf,
    uint8 *is_tcam,
    uint8 *is_em,
    uint8 *layer_id)
{
    char *layer = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(CTEST_DNX_L3_PERFORMANCE_ENTRIES, *nof_entries);
    SH_SAND_GET_UINT32(CTEST_DNX_L3_PERFORMANCE_RUNS, *nof_runs);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_UPDATE, *update);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_DELETE, *delete);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_RANDOM, *is_random);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_VALIDATE, *do_validation);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_UPDATE_MEASURES, *do_update_measures);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_PUBLIC, *is_public);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_RPF, *is_rpf);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_TCAM, *is_tcam);
    SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_EM, *is_em);
    SH_SAND_GET_STR(CTEST_DNX_L3_PERFORMANCE_LAYER, layer);

    if (*nof_runs < 1 || *nof_runs > 100)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of runs need to be between 1 and 100\n");
    }
    if (*nof_entries == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of entries is 0\n");
    }
    if (*is_tcam && *is_em)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid combination of parameters - " CTEST_DNX_L3_PERFORMANCE_TCAM " and "
                     CTEST_DNX_L3_PERFORMANCE_EM "\n");
    }
    if (*do_validation && *do_update_measures)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid combination of parameters - " CTEST_DNX_L3_PERFORMANCE_VALIDATE " and "
                     CTEST_DNX_L3_PERFORMANCE_UPDATE_MEASURES "\n");
    }
    if (*do_update_measures && !(*update && *delete))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Parameter " CTEST_DNX_L3_PERFORMANCE_UPDATE_MEASURES " requires " CTEST_DNX_L3_PERFORMANCE_UPDATE
                     " and " CTEST_DNX_L3_PERFORMANCE_DELETE " to be set\n");
    }

#if defined(INCLUDE_KBP)
    /** In simulation KBP is always enabled and the FWD applications need to be validated separately */
    if (dnx_kbp_device_enabled(unit))
    {
        if ((is_ipv6 && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6)) ||
            (!is_ipv6 && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4)))
        {
            /** Perform sync only if the relevant application is enabled */
            *is_external = TRUE;
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

            if (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_BCM52321)
            {
                /** Perform measurements validation or update only on OP2 */
                *do_validation = *do_update_measures = FALSE;
            }
        }
        else
        {
            *is_external = FALSE;
        }
    }
    else
#else
    {
        *is_external = FALSE;
    }
#endif

    /** Update the validation indication depending on the device ID */
    SHR_IF_ERR_EXIT(dnx_l3_performance_validation_check_for_device(unit, do_validation, do_update_measures));
    /** Only BCM and DBAL layer testing is supported at the moment */
    SHR_IF_ERR_EXIT(dnx_l3_performance_layer_parameters_update
                    (unit, layer, layer_id, do_validation, do_update_measures));

exit:
    SHR_FUNC_EXIT;
}

/** Used to calculate and print the performance measures */
static void
dnx_l3_performance_detailed_performance_measurements_print(
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
    LOG_CLI((BSL_META("\n%s\nTime spent (us) - All runs: "), step_name));
    for (index = 0; index < nof_runs; index++)
    {
        LOG_CLI((BSL_META("%d "), run_time[index]));
    }
    LOG_CLI((BSL_META("\n\tmin - %d\n\tmax - %d\n\tavg - %d\n"), mint, maxt, avgt));
    LOG_CLI((BSL_META("Rate (Ke/s) - All runs: ")));
    for (index = 0; index < nof_runs; index++)
    {
        LOG_CLI((BSL_META("%.2f "), ((double) run_entries / (double) run_time[index]) * 1000));
    }
    LOG_CLI((BSL_META("\n\tbest  - %.2f\n\tworst - %.2f\n\tavg   - %.2f\n"), minr * 1000, maxr * 1000, avgr * 1000));

    if (avg_rate)
    {
        *avg_rate = avgr * 1000 * 1000;
    }

    /*
     * The following code block ignores (about) 1/4 of the worst and (about) 1/4 of the best results
     * and calculate new more stable average
     */
    {
        /** Calculate the number of runs to ignore */
        int trim_runs = ((nof_runs / 2) - ((nof_runs / 2) % 2)) / 2;
        int order_index;
        avgt = 0;
        for (order_index = 0; order_index < nof_runs - 1; order_index++)
        {
            int swap_index = order_index;
            int tmp_run_time;
            for (index = order_index + 1; index < nof_runs; index++)
            {
                if (run_time[index] < run_time[swap_index])
                {
                    swap_index = index;
                }
            }
            if (swap_index != order_index)
            {
                tmp_run_time = run_time[swap_index];
                run_time[swap_index] = run_time[order_index];
                run_time[order_index] = tmp_run_time;
            }
        }
        for (index = trim_runs; index < nof_runs - trim_runs; index++)
        {
            avgt += run_time[index];
        }
        avgt /= (nof_runs - trim_runs * 2);
        avgr = (double) run_entries / (double) avgt;

        if (avg_rate)
        {
            *avg_rate = avgr * 1000 * 1000;
        }

        LOG_CLI((BSL_META
                 ("Removing the %d worst and best runs and using the average of the remaining results for validation (%.2f)\n\n"),
                 trim_runs, avgr * 1000));
    }
}

#if 0
/** Used to execute the performance measures for KBP Access */

static shr_error_e
dnx_l3_performance_access(
    int unit,
    uint8 is_ipv6,
    uint8 is_rpf,
    uint8 is_tcam,
    uint8 is_host,
    uint8 is_ipmc,
    int nof_runs,
    int nof_entries,
    unsigned int *action,
    int run_time[3][100])
{
    int rv;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    uint32 timer_idx[3] = { 1, 2, 3 };
    uint32 timers_group;
    int entry_index, run_index;
    int start_time = 0;

    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_ad_db_t_p ad_db_p_second = NULL;
    kbp_db_handles_t db_handles;

    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    uint32 key_size_in_bits;
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 payload_size_in_bits;
    uint32 prefix_length = 0;
    uint8 is_for_update = FALSE;
    uint8 hitbit = FALSE;
    uint8 is_default_entry = FALSE;
    uint8 is_cache_enabled = FALSE;
    uint8 use_optimized = FALSE;
    uint32 db_id = 0;
    int current_word_index = 0;

    dbal_tables_e select_table = NULL;
    dbal_tables_e fwd_table[2][DBAL_MAX_STRING_LENGTH] =
        { DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD };
    dbaltables_e rpf_table[2][DBAL_MAX_STRING_LENGTH] =
        { DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF };
    dbaltables_e ipmc_table[2][DBAL_MAX_STRING_LENGTH] =
        { DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD, DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * This test is capable of running on LPM. Perform checks and initializations for KBP only if KBP is enabled.
     * In simulation KBP is always enabled and the FWD applications need to be validated separately.
     */
    if (dnx_kbp_device_enabled(unit))
    {
        if ((is_ipv6 && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6)) ||
            (!is_ipv6 && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4)))
        {
            /** Perform sync and use the correct flags only if the relevant application is enabled */
            SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));
        }
    }

    if (is_ipmc)
    {
        select_table = ipmc_table[is_ipv6];
        key_size_in_bits = is_ipv6 ? 320 : 160;
        prefix_length = is_ipv6 ? 296 : 80;
        payload_size_in_bits = 24;
        use_optimized = TRUE;
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Getting KBP access measures is not supported for Multicast tables.\n");
    }
    else if (is_host)
    {
        select_table = fwd_table[is_ipv6];
        key_size_in_bits = is_ipv6 ? 144 : 48;
        prefix_length = is_ipv6 ? 144 : 48;
        payload_size_in_bits = 64;
        use_optimized = FALSE;
    }
    else if (is_rpf && kbp_mngr_split_rpf_in_use(unit))
    {
        select_table = rpf_table[is_ipv6];
        key_size_in_bits = prefix_length = is_ipv6 ? 144 : 48;
        prefix_length = is_ipv6 ? 144 : 48;
        payload_size_in_bits = 32;
        use_optimized = FALSE;
    }
    else
    {
        select_table = fwd_table[is_ipv6];
        key_size_in_bits = is_ipv6 ? 144 : 48;
        prefix_length = is_ipv6 ? 144 : 48;
        payload_size_in_bits = 24;
        use_optimized = TRUE;
    }

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, select_table, &db_id, NULL));

    SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &db_handles));
    db_p = db_handles.db_p;

    /** Check if the result is optimized or regular and use the relevant AD DB */
    if (DNX_KBP_USE_OPTIMIZED_RESULT && use_optimized)
    {
        /** Use optimized result. Update the payload size. */
        ad_db_p = db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED];
        ad_db_p_second = db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];
    }
    else
    {
        ad_db_p = db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];
        ad_db_p_second = db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED];
    }

    sal_strncpy(group_name, "KBP performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    /** Add */
    LOG_CLI((BSL_META("step1: Add entries\n")));
    utilex_ll_timer_set("dnx_kbp_entry_lpm_add create", timers_group, timer_idx[0]);
    for (run_index = 0; run_index < nof_runs; run_index++)
    {
        start_time = sal_time_usecs();
        for (entry_index = 0; entry_index < nof_entries; entry_index++)
        {

            rv = dnx_kbp_entry_lpm_add(unit, db_p, ad_db_p, NULL, key, key_size_in_bits, payload, payload_size_in_bits,
                                       prefix_length, is_for_update, hitbit, is_default_entry, is_cache_enabled,
                                       ad_db_p_second);

            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "Error, dnx_kbp_entry_lpm_add failed - run %d entry index %d\n", run_index,
                             entry_index);
            }
            current_word_index = BITS2WORDS(key_size_in_bits) - 1;
            while (++key[--current_word_index] == 0);
            current_word_index = BITS2WORDS(payload_size_in_bits) - 1;
            while (++payload[--current_word_index] == 0);
        }
        run_time[0][run_index] = sal_time_usecs() - start_time;
    }
    utilex_ll_timer_stop(timers_group, timer_idx[0]);

    /** Update */
    if (action[1])
    {
        LOG_CLI((BSL_META("step2: Update entries\n")));
        utilex_ll_timer_set("dnx_kbp_entry_lpm_add update", timers_group, timer_idx[1]);

        sal_memset(key, 0x0, sizeof(uint32) * DBAL_PHYSICAL_KEY_SIZE_IN_WORDS);
        sal_memset(payload, 0x0, sizeof(uint32) * DBAL_PHYSICAL_RES_SIZE_IN_WORDS);
        payload[BITS2WORDS(payload_size_in_bits) - 1] = 10;
        is_for_update = TRUE;

        for (run_index = 0; run_index < nof_runs; run_index++)
        {
            start_time = sal_time_usecs();
            for (entry_index = 0; entry_index < nof_entries; entry_index++)
            {
                rv = dnx_kbp_entry_lpm_add(unit, db_p, ad_db_p, NULL, key, key_size_in_bits, payload,
                                           payload_size_in_bits, prefix_length, is_for_update, hitbit, is_default_entry,
                                           is_cache_enabled, ad_db_p_second);

                if (rv != BCM_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "Error, dnx_kbp_entry_lpm_add update failed - run %d entry index %d\n", run_index,
                                 entry_index);
                }
                current_word_index = BITS2WORDS(key_size_in_bits) - 1;
                while (++key[--current_word_index] == 0);
                current_word_index = BITS2WORDS(payload_size_in_bits) - 1;
                while (++payload[--current_word_index] == 0);
            }
            run_time[1][run_index] = sal_time_usecs() - start_time;
        }
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
    }

    /** Delete */
    if (action[2])
    {
        LOG_CLI((BSL_META("step3: Delete entries\n")));
        utilex_ll_timer_set("dnx_kbp_entry_lpm_delete", timers_group, timer_idx[2]);

        sal_memset(key, 0x0, sizeof(uint32) * DBAL_PHYSICAL_KEY_SIZE_IN_WORDS);

        for (run_index = 0; run_index < nof_runs; run_index++)
        {
            start_time = sal_time_usecs();
            for (entry_index = 0; entry_index < nof_entries; entry_index++)
            {

                SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_delete(unit, db_p, ad_db_p, NULL, key, key_size_in_bits,
                                                         prefix_length, is_cache_enabled, NULL));

                if (rv != BCM_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "Error, dnx_kbp_entry_lpm_delete failed - run %d entry index %d\n", run_index,
                                 entry_index);
                }
                current_word_index = BITS2WORDS(key_size_in_bits) - 1;
                while (++key[--current_word_index] == 0);
            }
            run_time[2][run_index] = sal_time_usecs() - start_time;
        }
        utilex_ll_timer_stop(timers_group, timer_idx[2]);
    }

exit:
    SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    SHR_FUNC_EXIT;
}

/** Decide the DBAL table, the IPv4/6 field type and the result type */
static void
dnx_l3_performance_dbal_fields_get(
    uint8 is_host,
    uint8 is_ipmc,
    uint8 is_ipv6,
    uint8 is_external,
    uint8 is_public,
    uint8 is_rpf,
    uint8 is_tcam,
    uint8 is_em,
    dbal_tables_e * dbal_table_id,
    dbal_fields_e * result_type,
    dbal_fields_e * dip_field,
    dbal_fields_e * sip_field,
    dbal_fields_e * glob_in_lif_field,
    uint8 *is_kaps)
{
    if (is_external)
    {
        *dip_field = is_ipv6 ? DBAL_FIELD_IPV6 : DBAL_FIELD_IPV4;
        if (is_ipmc)
        {
            *dbal_table_id = is_ipv6 ? DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD :
                DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD;
            *result_type = is_ipv6 ? DBAL_RESULT_TYPE_KBP_IPV6_MULTICAST_TCAM_FORWARD_KBP_DEST_W_DEFAULT :
                DBAL_RESULT_TYPE_KBP_IPV4_MULTICAST_TCAM_FORWARD_KBP_DEST_W_DEFAULT;
            *sip_field = is_ipv6 ? DBAL_FIELD_IPV6_SIP : DBAL_FIELD_IPV4_SIP;
            *dip_field = is_ipv6 ? DBAL_FIELD_IPV6_DIP : DBAL_FIELD_IPV4_DIP;
        }
        else if (is_host)
        {
            *dbal_table_id = is_ipv6 ? DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD :
                DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            *result_type =
                is_ipv6 ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT :
                DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT;
        }
        else if (is_rpf)
        {
            *dbal_table_id = is_ipv6 ? DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF :
                DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF;
            *result_type = is_ipv6 ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT :
                DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT;
        }
        else
        {
            *dbal_table_id = is_ipv6 ? DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD :
                DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            *result_type = is_ipv6 ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT :
                DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT;
        }
    }
    else
    {
        *dip_field = is_ipv6 ? DBAL_FIELD_IPV6 : DBAL_FIELD_IPV4;
        if (is_ipmc)
        {
            *sip_field = is_ipv6 ? DBAL_FIELD_IPV6_SIP : DBAL_FIELD_IPV4_SIP;
            *dip_field = is_ipv6 ? DBAL_FIELD_IPV6_GROUP : DBAL_FIELD_IPV4_GROUP;
            if (is_tcam)
            {
                *dbal_table_id = is_ipv6 ? DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD :
                    DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD;
                *result_type = is_ipv6 ? DBAL_RESULT_TYPE_IPV6_MULTICAST_TCAM_FORWARD_FWD_DEST :
                    DBAL_RESULT_TYPE_IPV4_MULTICAST_TCAM_FORWARD_FWD_DEST;
            }
            else if (is_em)
            {
                *dbal_table_id = is_ipv6 ? DBAL_TABLE_IPV6_MULTICAST_EM_FORWARD : DBAL_TABLE_IPV4_MULTICAST_EM_FORWARD;
                *result_type = is_ipv6 ? DBAL_RESULT_TYPE_IPV6_MULTICAST_EM_FORWARD_FWD_DEST_STAT :
                    DBAL_RESULT_TYPE_IPV4_MULTICAST_EM_FORWARD_FWD_DEST_STAT;
            }
            else if (is_public)
            {
                *dbal_table_id = is_ipv6 ? DBAL_TABLE_IPV6_MULTICAST_PUBLIC_LPM_FORWARD :
                    DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD;
                *is_kaps = TRUE;
            }
            else
            {
                *dbal_table_id = is_ipv6 ? DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD :
                    DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD;
                *is_kaps = TRUE;
            }
        }
        else if (is_host)
        {
            *dbal_table_id = is_ipv6 ? DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST : DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST;
            *result_type = is_ipv6 ? DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF :
                DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
        }
        else if (is_public)
        {
            *dbal_table_id = is_ipv6 ? DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2 :
                DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2;
            *is_kaps = TRUE;
        }
        else
        {
            *dbal_table_id = is_ipv6 ? DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD :
                DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            *is_kaps = TRUE;
        }
    }
}

/** This function is generating random DBAL entries according to the tested table */
static shr_error_e
dnx_l3_performance_dbal_generate_entries_array(
    int unit,
    dnx_l3_performance_dbal_entry_info_t * entry_info,
    int nof_entries,
    uint8 is_host,
    uint8 is_ipmc,
    uint8 is_ipv6,
    uint8 is_external,
    uint8 is_public,
    uint8 is_rpf,
    uint8 is_tcam,
    uint8 is_em,
    uint8 is_kaps)
{
    int counter = 0;
    int start_time = 0, end_time = 0;
    uint64 work_reg_64 = COMPILER_64_INIT(0, 1);

    SHR_FUNC_INIT_VARS(unit);

    dnx_l3_performance_random_seed(work_reg_64);
    LOG_CLI((BSL_META("Started creating %d dbal entries\n...\n"), nof_entries));
    start_time = sal_time_usecs();

    for (counter = 0; counter < nof_entries; counter++)
    {
        sal_memset(&entry_info[counter], 0x0, sizeof(dnx_l3_performance_dbal_entry_info_t));

        entry_info[counter].vrf = (dnx_l3_performance_random_get() & 0xFE) + 1;
        if (is_public)
        {
            entry_info[counter].vrf = 0;
        }
        if (is_ipv6)
        {
            int current_ipv6_index = 16;
            while (--current_ipv6_index)
            {
                entry_info[counter].dip6[current_ipv6_index] = dnx_l3_performance_random_get() & 0xFF;
                entry_info[counter].sip6[current_ipv6_index] = 0x00;
            }
            if (is_ipmc)
            {
                entry_info[counter].dip6[0] = 0xFF;
            }
            else
            {
                entry_info[counter].dip6[0] = 0x00;
            }
        }
        else
        {
            if (is_ipmc)
            {
                entry_info[counter].dip = (dnx_l3_performance_random_get() & 0x0FFFFFFF);
            }
            else
            {
                entry_info[counter].dip = dnx_l3_performance_random_get() & 0x1FFFFFFF;
            }
            entry_info[counter].sip = 0x00000000;
        }
        if (is_ipmc)
        {
            entry_info[counter].in_lif = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
        }
        entry_info[counter].destination = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
        if (is_host)
        {
            entry_info[counter].out_lif = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
        }
    }

    end_time = sal_time_usecs();
    LOG_CLI((BSL_META("Finished creating dbal entries array after %d us\n\n"), end_time - start_time));

    SHR_FUNC_EXIT;
}

/** Utility macro for setting the entry fields for DBAL testing */
/* *INDENT-OFF* */
#define DNX_L3_PERFORMANCE_DBAL_FIELDS_SET(_unit_, _entry_handle_id_, _entry_info_, _is_update_, _dbal_table_id_, _is_host_, _is_ipmc_, _is_ipv6_, _is_external_, _is_public_, _is_tcam_, _is_em_, _is_kaps_, _dip_field_, _sip_field_, _glob_in_lif_field_, _result_type_) \
do { \
    if (!_is_public_) { dbal_entry_key_field32_set(_unit_, _entry_handle_id_, DBAL_FIELD_VRF, _entry_info_.vrf); } \
    if (_is_ipv6_) { dbal_entry_key_field_arr8_set(_unit_, _entry_handle_id_, _dip_field_, _entry_info_.dip6); }\
    else { dbal_entry_key_field32_set(_unit_, _entry_handle_id_, _dip_field_, _entry_info_.dip); } \
    if (_is_ipmc_) { \
        dbal_entry_key_field32_set(_unit_, _entry_handle_id_, _glob_in_lif_field_, _entry_info_.in_lif); \
        if (!_is_ipv6_ && !_is_em_) { dbal_entry_key_field32_set(_unit_, _entry_handle_id_, _sip_field_, _entry_info_.sip); } \
        if (_is_tcam_) { \
            if (_is_ipv6_) { dbal_entry_key_field_arr8_set(_unit_, _entry_handle_id_, _sip_field_, _entry_info_.sip6); } \
            if (_is_external_) { SHR_IF_ERR_EXIT(dbal_entry_attribute_set(_unit_, _entry_handle_id_, DBAL_ENTRY_ATTR_PRIORITY, 0)); } \
            else { \
                uint32 entry_access_id = 0; \
                if (_is_update_) { SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(_unit_, _entry_handle_id_, &entry_access_id, DBAL_COMMIT)); } \
                else { SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(_unit_, DBAL_CORE_ALL, _dbal_table_id_, 0, &entry_access_id)); } \
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(_unit_, _entry_handle_id_, entry_access_id)); \
            } \
        } \
    } \
    if (_is_kaps_) { dbal_entry_value_field32_set(_unit_, _entry_handle_id_, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, _entry_info_.destination); } \
    else { \
        dbal_entry_value_field32_set(_unit_, _entry_handle_id_, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, _result_type_); \
        dbal_entry_value_field32_set(_unit_, _entry_handle_id_, DBAL_FIELD_DESTINATION, INST_SINGLE, _entry_info_.destination); \
        if (_is_host_) { dbal_entry_value_field32_set(_unit_, _entry_handle_id_, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, _entry_info_.out_lif); } \
        if (_is_external_) { dbal_entry_value_field32_set(_unit_, _entry_handle_id_, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, FALSE); } \
    } \
} while(0)
/* *INDENT-ON* */

/** Used to execute the performance measures for DBAL */
static shr_error_e
dnx_l3_performance_dbal(
    int unit,
    uint8 is_random,
    uint8 is_host,
    uint8 is_ipmc,
    uint8 is_ipv6,
    uint8 is_external,
    uint8 is_public,
    uint8 is_rpf,
    uint8 is_tcam,
    uint8 is_em,
    uint8 is_force,
    int nof_runs,
    int nof_entries,
    uint8 *action,
    int run_time[3][100])
{
    int rv;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    uint32 timer_idx[3] = { 1, 2, 3 };
    uint32 timers_group;
    int entry_index, run_index, dbal_index;
    int start_time = 0;
    int total_entries;
    dnx_l3_performance_dbal_entry_info_t *entry_info = NULL;

    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;

    dbal_fields_e result_type = DBAL_FIELD_EMPTY;
    dbal_fields_e sip_field = DBAL_FIELD_EMPTY;
    dbal_fields_e dip_field = DBAL_FIELD_EMPTY;
    dbal_fields_e glob_in_lif_field = is_external ? DBAL_FIELD_KBP_GLOB_IN_LIF : DBAL_FIELD_GLOB_IN_LIF;
    uint8 is_kaps = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dnx_l3_performance_dbal_fields_get(is_host, is_ipmc, is_ipv6, is_external, is_public, is_rpf, is_tcam, is_em,
                                       &dbal_table_id, &result_type, &dip_field, &sip_field, &glob_in_lif_field,
                                       &is_kaps);

    total_entries = nof_entries * nof_runs;
    entry_info =
        (dnx_l3_performance_dbal_entry_info_t *) sal_alloc(sizeof(dnx_l3_performance_dbal_entry_info_t) * total_entries,
                                                           "host_info");
    if (entry_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d DBAL entries\n", total_entries);
    }

    /** DBAL supports only testing of randomized entries */
    {
        SHR_IF_ERR_EXIT(dnx_l3_performance_dbal_generate_entries_array
                        (unit, entry_info, total_entries, is_host, is_ipmc, is_ipv6, is_external,
                         is_public, is_rpf, is_tcam, is_em, is_kaps));
    }

    sal_strncpy(group_name, "L3 perf DBAL", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    LOG_CLI((BSL_META("Running performance test on %s, %d runs each with %d entries\n"),
             dbal_logical_table_to_string(unit, dbal_table_id), nof_runs, nof_entries));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Add */
    dbal_index = 0;
    LOG_CLI((BSL_META("step1: Add entries\n")));
    utilex_ll_timer_set("dbal_entry_commit create", timers_group, timer_idx[0]);
    for (run_index = 0; run_index < nof_runs; run_index++)
    {
        start_time = sal_time_usecs();
        for (entry_index = 0; entry_index < nof_entries; entry_index++)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            DNX_L3_PERFORMANCE_DBAL_FIELDS_SET(unit, entry_handle_id, entry_info[dbal_index], FALSE, dbal_table_id,
                                               is_host, is_ipmc, is_ipv6, is_external, is_public, is_tcam, is_em,
                                               is_kaps, dip_field, sip_field, glob_in_lif_field, result_type);
            rv = dbal_entry_commit(unit, entry_handle_id, (is_force ? DBAL_COMMIT_FORCE : DBAL_COMMIT));
            dbal_index++;

            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(rv, "Error, dbal_entry_commit DBAL_COMMIT failed - run %d entry index %d\n",
                             run_index, entry_index);
            }
        }
        run_time[0][run_index] = sal_time_usecs() - start_time;
    }
    utilex_ll_timer_stop(timers_group, timer_idx[0]);

    /** Update */
    if (action[1])
    {
        dbal_index = 0;
        /** Update the DBAL payload */
        for (dbal_index = 0; dbal_index < total_entries; dbal_index++)
        {
            entry_info[dbal_index].destination = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
        }

        dbal_index = 0;
        LOG_CLI((BSL_META("step2: Update entries\n")));
        utilex_ll_timer_set("dbal_entry_commit update", timers_group, timer_idx[1]);

        for (run_index = 0; run_index < nof_runs; run_index++)
        {
            start_time = sal_time_usecs();
            for (entry_index = 0; entry_index < nof_entries; entry_index++)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
                DNX_L3_PERFORMANCE_DBAL_FIELDS_SET(unit, entry_handle_id, entry_info[dbal_index], TRUE, dbal_table_id,
                                                   is_host, is_ipmc, is_ipv6, is_external, is_public, is_tcam, is_em,
                                                   is_kaps, dip_field, sip_field, glob_in_lif_field, result_type);
                rv = dbal_entry_commit(unit, entry_handle_id, (is_force ? DBAL_COMMIT_FORCE : DBAL_COMMIT_UPDATE));
                dbal_index++;

                if (rv != BCM_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "Error, dbal_entry_commit DBAL_COMMIT_UPDATE failed - run %d entry index %d\n",
                                 run_index, entry_index);
                }
            }
            run_time[1][run_index] = sal_time_usecs() - start_time;
        }
        utilex_ll_timer_stop(timers_group, timer_idx[1]);
    }

    /** Delete */
    if (action[2])
    {
        dbal_index = 0;
        LOG_CLI((BSL_META("step3: Delete entries\n")));
        utilex_ll_timer_set("dbal_entry_clear", timers_group, timer_idx[2]);

        for (run_index = 0; run_index < nof_runs; run_index++)
        {
            start_time = sal_time_usecs();
            for (entry_index = 0; entry_index < nof_entries; entry_index++)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
                DNX_L3_PERFORMANCE_DBAL_FIELDS_SET(unit, entry_handle_id, entry_info[dbal_index], TRUE, dbal_table_id,
                                                   is_host, is_ipmc, is_ipv6, is_external, is_public, is_tcam, is_em,
                                                   is_kaps, dip_field, sip_field, glob_in_lif_field, result_type);
                rv = dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT);
                dbal_index++;

                if (rv != BCM_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "Error, dbal_entry_clear failed - run %d entry index %d\n",
                                 run_index, entry_index);
                }
            }
            run_time[2][run_index] = sal_time_usecs() - start_time;
        }
        utilex_ll_timer_stop(timers_group, timer_idx[2]);
    }

exit:
    SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif

/** generates array of hosts with full mask */
static shr_error_e
dnx_l3_performance_host_generate_entries_array(
    int unit,
    bcm_l3_host_t * hosts,
    int nof_hosts,
    uint8 is_random,
    uint8 is_ipv6)
{
    bcm_ip6_t ipv6_addr =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int counter = 0;
    int start_time = 0, end_time = 0;
    uint32 vrf = 1;
    uint32 ipv4_addr = 0;
    uint32 encap_id = 0;
    uint32 payload = 1;
    uint32 *random_ipv4_addr = NULL;
    uint64 work_reg_64 = COMPILER_64_INIT(0, 1);

    SHR_FUNC_INIT_VARS(unit);

    dnx_l3_performance_random_seed(work_reg_64);
    LOG_CLI((BSL_META("Started creating %d %s hosts array\n...\n"), nof_hosts, (is_ipv6 ? "IPv6" : "IPv4")));
    start_time = sal_time_usecs();

    if (!is_ipv6 && is_random)
    {
        /** Generate 16M random IPv4 addresses */
        random_ipv4_addr = (uint32 *) sal_alloc(sizeof(uint32) * 16 * 1024 * 1024, "random_ipv4_array");
        if (random_ipv4_addr == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d random IPv4\n", 16 * 1024 * 1024);
        }
        dnx_l3_performance_random_ipv4_generate(random_ipv4_addr, 16 * 1024 * 1024);
    }

    for (counter = 0; counter < nof_hosts; counter++)
    {
        bcm_l3_host_t_init(&hosts[counter]);

        if (is_random)
        {
            /** Randomized host entries */
            hosts[counter].l3a_vrf = (dnx_l3_performance_random_get() & 0xFE) + 1;
            if (is_ipv6)
            {
                int current_host_index = 16;
                while (--current_host_index)
                {
                    hosts[counter].l3a_ip6_addr[current_host_index] = dnx_l3_performance_random_get() & 0xFF;
                }
                hosts[counter].l3a_ip6_addr[0] = 0x00;
                hosts[counter].l3a_flags |= BCM_L3_IP6;
            }
            else
            {
                hosts[counter].l3a_ip_addr = random_ipv4_addr[counter] & 0x1FFFFFFF;
            }
            hosts[counter].l3a_intf = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
            hosts[counter].encap_id = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
        }
        else
        {
            /** Sequential host entries */
            hosts[counter].l3a_vrf = vrf;
            if (is_ipv6)
            {
                int current_route_index = 16;
                hosts[counter].l3a_flags |= BCM_L3_IP6;
                sal_memcpy(hosts[counter].l3a_ip6_addr, ipv6_addr, 16);
                while (++ipv6_addr[--current_route_index] == 0);
            }
            else
            {
                hosts[counter].l3a_ip_addr = ipv4_addr++;
            }
            hosts[counter].l3a_intf = payload;
            hosts[counter].encap_id = encap_id;
            if (++payload == 256 * 1024)
            {
                payload = 1;
            }
            if (++encap_id == 128 * 1024)
            {
                payload = 1;
            }
        }
    }

    end_time = sal_time_usecs();
    LOG_CLI((BSL_META("Finished creating hosts array after %d us\n\n"), end_time - start_time));

exit:
    if (random_ipv4_addr != NULL)
    {
        sal_free(random_ipv4_addr);
    }
    SHR_FUNC_EXIT;
}

/** generates array of ipmc entries */
static shr_error_e
dnx_l3_performance_ipmc_generate_entries_array(
    int unit,
    bcm_ipmc_addr_t * ipmcs,
    int nof_ipmcs,
    uint8 is_random,
    uint8 is_ipv6,
    uint8 is_public,
    uint8 is_tcam,
    uint8 is_em)
{
    bcm_ip6_t ipv6_mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_ip6_t ipv6_addr =
        { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int counter = 0;
    int start_time = 0, end_time = 0;
    uint32 fwd_flags = is_tcam ? BCM_IPMC_TCAM : 0;
    uint32 ipv4_addr = 0xE0000000;
    uint32 vrf = 1;
    uint32 in_lif = 0;
    uint32 payload = 1;
    uint32 *random_ipv4_addr = NULL;
    uint64 work_reg_64 = COMPILER_64_INIT(0, 1);

    SHR_FUNC_INIT_VARS(unit);

    dnx_l3_performance_random_seed(work_reg_64);
    LOG_CLI((BSL_META("Started creating %d %s %s ipmcs array\n...\n"),
             nof_ipmcs, (is_ipv6 ? "IPv6" : "IPv4"), (is_public ? "Public" : "Private")));
    start_time = sal_time_usecs();

    if (!is_ipv6 && is_random)
    {
        /** Generate 16M random IPv4 addresses */
        random_ipv4_addr = (uint32 *) sal_alloc(sizeof(uint32) * 16 * 1024 * 1024, "random_ipv4_array");
        if (random_ipv4_addr == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d random IPv4\n", 16 * 1024 * 1024);
        }
        dnx_l3_performance_random_ipv4_generate(random_ipv4_addr, 16 * 1024 * 1024);
    }

    for (counter = 0; counter < nof_ipmcs; counter++)
    {
        bcm_ipmc_addr_t_init(&ipmcs[counter]);

        if (is_random)
        {
            /** Randomized ipmc entries */
            ipmcs[counter].flags = fwd_flags;
            ipmcs[counter].vrf = (dnx_l3_performance_random_get() & 0xFE) + 1;
            if (is_public)
            {
                ipmcs[counter].vrf = 0;
            }
            if (is_ipv6)
            {
                int current_route_index = 16;
                while (--current_route_index)
                {
                    ipmcs[counter].mc_ip6_addr[current_route_index] = dnx_l3_performance_random_get() & 0xFF;
                    ipmcs[counter].mc_ip6_mask[current_route_index] = 0xFF;
                }
                ipmcs[counter].mc_ip6_addr[0] = 0xFF;
                sal_memcpy(ipmcs[counter].mc_ip6_mask, ipv6_mask, 16);
                ipmcs[counter].flags |= BCM_IPMC_IP6;
            }
            else
            {
                ipmcs[counter].mc_ip_addr = (random_ipv4_addr[counter] & 0x0FFFFFFF) | 0xE0000000;
                ipmcs[counter].mc_ip_mask = 0xFFFFFFFF;
                ipmcs[counter].s_ip_mask = is_em ? 0x00000000 : 0xFFFFFFFF;
                ipmcs[counter].s_ip_addr = is_em ? 0x00000000 : 0xFFFFFFFF;
            }
            if (is_tcam)
            {
                ipmcs[counter].priority = 0;
            }
            if (is_em)
            {
                BCM_L3_ITF_SET(ipmcs[counter].ing_intf, BCM_L3_ITF_TYPE_LIF,
                               ((dnx_l3_performance_random_get() & 0x3FFE) + 1));
            }
            else
            {
                /** Empty random call in order to sync EM and non-EM l3a_intf values */
                dnx_l3_performance_random_get();
            }
            ipmcs[counter].l3a_intf = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
        }
        else
        {
            /** Sequential ipmc entries */
            ipmcs[counter].flags = fwd_flags;
            ipmcs[counter].vrf = is_public ? 0 : vrf;
            if (is_ipv6)
            {
                int current_route_index = 16;
                sal_memcpy(ipmcs[counter].mc_ip6_addr, ipv6_addr, 16);
                sal_memcpy(ipmcs[counter].mc_ip6_mask, ipv6_mask, 16);
                while (++ipv6_addr[--current_route_index] == 0);
                ipmcs[counter].flags |= BCM_IPMC_IP6;
            }
            else
            {
                ipmcs[counter].mc_ip_addr = ipv4_addr++;
                ipmcs[counter].mc_ip_mask = 0xFFFFFFFF;
                ipmcs[counter].s_ip_mask = is_em ? 0x00000000 : 0xFFFFFFFF;
                ipmcs[counter].s_ip_addr = is_em ? 0x00000000 : 0xFFFFFFFF;
            }
            if (is_tcam)
            {
                ipmcs[counter].priority = 0;
            }
            if (is_em)
            {
                BCM_L3_ITF_SET(ipmcs[counter].ing_intf, BCM_L3_ITF_TYPE_LIF, in_lif);
                if (++in_lif == 128 * 1024)
                {
                    in_lif = 0;
                }
            }
            ipmcs[counter].l3a_intf = payload;
            if (++payload == 256 * 1024)
            {
                payload = 1;
            }
        }
    }

    end_time = sal_time_usecs();
    LOG_CLI((BSL_META("Finished creating ipmcs array after %d us\n\n"), end_time - start_time));

exit:
    if (random_ipv4_addr != NULL)
    {
        sal_free(random_ipv4_addr);
    }
    SHR_FUNC_EXIT;
}

/** generates array of routes with full mask */
static shr_error_e
dnx_l3_performance_route_generate_entries_array(
    int unit,
    bcm_l3_route_t * routes,
    int nof_routes,
    uint8 is_random,
    uint8 is_ipv6,
    uint8 is_external,
    uint8 is_public,
    uint8 is_rpf)
{
    bcm_ip6_t ipv6_mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00 };
    bcm_ip6_t ipv6_addr =
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int counter = 0;
    int start_time = 0, end_time = 0;
    uint32 ipv4_addr = 0;
    uint32 vrf = 1;
    uint32 fwd_flags2 = 0;
    uint32 payload = 1;
    uint32 *random_ipv4_addr = NULL;
    uint64 work_reg_64 = COMPILER_64_INIT(0, 1);

    SHR_FUNC_INIT_VARS(unit);

    dnx_l3_performance_random_seed(work_reg_64);
    LOG_CLI((BSL_META("Started creating %d %s %s routes array\n...\n"),
             nof_routes, (is_ipv6 ? "IPv6" : "IPv4"), (is_public ? "Public" : "Private")));
    start_time = sal_time_usecs();

    if (!is_ipv6 && is_random)
    {
        /** Generate 16M random IPv4 addresses */
        random_ipv4_addr = (uint32 *) sal_alloc(sizeof(uint32) * 16 * 1024 * 1024, "random_ipv4_array");
        if (random_ipv4_addr == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d random IPv4\n", 16 * 1024 * 1024);
        }
        dnx_l3_performance_random_ipv4_generate(random_ipv4_addr, 16 * 1024 * 1024);
    }

    if (is_external)
    {
        fwd_flags2 = is_rpf ? BCM_L3_FLAGS2_RPF_ONLY : BCM_L3_FLAGS2_FWD_ONLY;
    }

    for (counter = 0; counter < nof_routes; counter++)
    {
        bcm_l3_route_t_init(&routes[counter]);

        if (is_random)
        {
            /** Randomized route entries */
            routes[counter].l3a_flags2 |= fwd_flags2;
            routes[counter].l3a_vrf = (dnx_l3_performance_random_get() & 0xFE) + 1;
            if (is_public)
            {
                routes[counter].l3a_vrf = 0;
            }
            if (is_ipv6)
            {
                int current_route_index = 16;
                while (--current_route_index)
                {
                    routes[counter].l3a_ip6_net[current_route_index] = dnx_l3_performance_random_get() & 0xFF;
                }
                routes[counter].l3a_ip6_net[0] = 0x00;
                sal_memcpy(routes[counter].l3a_ip6_mask, ipv6_mask, 16);
                routes[counter].l3a_flags |= BCM_L3_IP6;
            }
            else
            {
                routes[counter].l3a_subnet = random_ipv4_addr[counter] & 0x1FFFFFFF;
                routes[counter].l3a_ip_mask = 0xFFFFFFFF;
            }
            routes[counter].l3a_intf = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
        }
        else
        {
            /** Sequential route entries */
            routes[counter].l3a_flags2 |= fwd_flags2;
            routes[counter].l3a_vrf = is_public ? 0 : vrf;
            if (is_ipv6)
            {
                int current_route_index = 14;
                routes[counter].l3a_flags |= BCM_L3_IP6;
                sal_memcpy(routes[counter].l3a_ip6_net, ipv6_addr, 16);
                sal_memcpy(routes[counter].l3a_ip6_mask, ipv6_mask, 16);
                while (++ipv6_addr[--current_route_index] == 0);
            }
            else
            {
                routes[counter].l3a_subnet = ipv4_addr;
                routes[counter].l3a_ip_mask = 0xFFFFFFF0;
                ipv4_addr += 16;
            }
            routes[counter].l3a_intf = payload;
            if (++payload == 256 * 1024)
            {
                payload = 1;
            }
        }
    }

    end_time = sal_time_usecs();
    LOG_CLI((BSL_META("Finished creating routes array after %d us\n\n"), end_time - start_time));

exit:
    if (random_ipv4_addr != NULL)
    {
        sal_free(random_ipv4_addr);
    }
    SHR_FUNC_EXIT;
}

/** Used to allocate required memory for hosts, routes, ipmcs and the add and update payloads */
static shr_error_e
dnx_l3_performance_entries_generate(
    int unit,
    uint8 is_random,
    uint8 is_host,
    uint8 is_ipmc,
    uint8 is_ipv6,
    uint8 is_external,
    uint8 is_public,
    uint8 is_rpf,
    uint8 is_tcam,
    uint8 is_em,
    int nof_entries,
    void **entries_array,
    uint32 **intf_add,
    uint32 **intf_update)
{
    int entry_index;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate memory for the given number of entries according to the tests (host, route, ipmc).
     * Allocated memory will be freed by the upper function.
     */
    if (is_host)
    {
        *entries_array = (bcm_l3_host_t *) sal_alloc(sizeof(bcm_l3_host_t) * nof_entries, "host_info");
    }
    else if (is_ipmc)
    {
        *entries_array = (bcm_ipmc_addr_t *) sal_alloc(sizeof(bcm_ipmc_addr_t) * nof_entries, "ipmc_info");
    }
    else
    {
        *entries_array = (bcm_l3_route_t *) sal_alloc(sizeof(bcm_l3_route_t) * nof_entries, "route_info");
    }
    /** Validate correct allocation */
    if (*entries_array == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d entries\n", nof_entries);
    }

    /** Allocate memory for the add and update payloads */
    *intf_add = (uint32 *) sal_alloc(sizeof(uint32) * nof_entries, "intf_add");
    if (*intf_add == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d interfaces for add\n", nof_entries);
    }
    *intf_update = (uint32 *) sal_alloc(sizeof(uint32) * nof_entries, "intf_update");
    if (*intf_update == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d interfaces for update\n", nof_entries);
    }

    /** Generate entries information */
    /** Store the add payload in intf_add */
    if (is_host)
    {
        bcm_l3_host_t *hosts = *entries_array;
        SHR_IF_ERR_EXIT(dnx_l3_performance_host_generate_entries_array
                        (unit, (bcm_l3_host_t *) * entries_array, nof_entries, is_random, is_ipv6));
        for (entry_index = 0; entry_index < nof_entries; entry_index++)
        {
            (*intf_add)[entry_index] = hosts[entry_index].l3a_intf;
        }
    }
    else if (is_ipmc)
    {
        bcm_ipmc_addr_t *ipmcs = *entries_array;
        SHR_IF_ERR_EXIT(dnx_l3_performance_ipmc_generate_entries_array
                        (unit, (bcm_ipmc_addr_t *) * entries_array, nof_entries, is_random, is_ipv6,
                         is_public, is_tcam, is_em));
        for (entry_index = 0; entry_index < nof_entries; entry_index++)
        {
            (*intf_add)[entry_index] = ipmcs[entry_index].l3a_intf;
        }
    }
    else
    {
        bcm_l3_route_t *routes = *entries_array;
        SHR_IF_ERR_EXIT(dnx_l3_performance_route_generate_entries_array
                        (unit, (bcm_l3_route_t *) * entries_array, nof_entries, is_random, is_ipv6,
                         is_external, is_public, is_rpf));
        for (entry_index = 0; entry_index < nof_entries; entry_index++)
        {
            (*intf_add)[entry_index] = routes[entry_index].l3a_intf;
        }
    }

    /** Set the intf_update according to the is_random parameter */
    for (entry_index = 0; entry_index < nof_entries; entry_index++)
    {
        if (is_random)
        {
            (*intf_update)[entry_index] = (dnx_l3_performance_random_get() & 0x3FFE) + 1;
        }
        else
        {
            (*intf_update)[entry_index] = (*intf_add)[entry_index] + 10;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** The function is used to generate the data for a given number of FEC entries */
static shr_error_e
dnx_l3_performance_fec_entries_generate(
    int unit,
    int nof_entries,
    bcm_l3_egress_t * fec_entries,
    int *fec_ids_array)
{
    int counter = 0;
    int start_time = 0, end_time = 0;
    int *hierarchy;
    uint32 fwd_flags[3] = { 0, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY };
    uint64 work_reg_64 = COMPILER_64_INIT(0, 1);

    SHR_FUNC_INIT_VARS(unit);

    hierarchy = (int *) sal_alloc(sizeof(int) * nof_entries, "hierarchy int");
    /** Generate entries information */
    dnx_l3_performance_random_seed(work_reg_64);
    LOG_CLI((BSL_META("Started creating %d FEC entries array\n...\n"), nof_entries));
    start_time = sal_time_usecs();

    SHR_IF_ERR_EXIT(dnx_l3_performance_fec_ids_generate(unit, fec_ids_array, nof_entries, hierarchy));

    for (counter = 0; counter < nof_entries; counter++)
    {
        bcm_l3_egress_t_init(&fec_entries[counter]);
        /** Sequential FEC entries */
        fec_entries[counter].flags |= fwd_flags[hierarchy[counter]];
        fec_entries[counter].intf = (counter & 0xFF0) + 1;
        fec_entries[counter].port = 200;
        fec_entries[counter].encap_id = 0x1000 + counter;
    }

    end_time = sal_time_usecs();
    LOG_CLI((BSL_META("Finished creating FEC array after %d us\n\n"), end_time - start_time));

exit:
    SHR_FUNC_EXIT;
}

/** Internal macro for setting the L3 add and update payloads */
#define CTEST_DNX_L3_PERFORMANCE_RUN_L3_PREP(_entry_, _data_, _operation_, _flag_) \
do { \
    (_entry_).l3a_intf = (_data_); \
} while (0)

/** Internal macro for setting the IPMC add and update payloads and the replace flag */
#define CTEST_DNX_L3_PERFORMANCE_RUN_IPMC_PREP(_entry_, _data_, _operation_, _flag_) \
do { \
    (_entry_).l3a_intf = (_data_); \
    (_entry_).flags _operation_ (_flag_); \
} while (0)

/** Utility macro for minimizing the code footprint when executing host, route and ipmc performance testing */
#define CTEST_DNX_L3_PERFORMANCE_RUN(_entry_info_, _add_api_, _delete_api_, _module_) \
do { \
    int rv; \
    int entry_index; \
    int start_time = 0; \
\
    LOG_CLI((BSL_META("Run %d\t"), run_index)); \
\
    /** Prepare the entries payload */ \
    for (entry_index = 0; entry_index < nof_entries; entry_index++) \
    { \
        CTEST_DNX_L3_PERFORMANCE_RUN_##_module_##_PREP((_entry_info_)[entry_index], intf_add[entry_index], &=, (~BCM_IPMC_REPLACE)); \
    } \
\
    /** Iterate over entries to add */ \
    LOG_CLI((BSL_META("Add "))); \
    utilex_ll_timer_set("bcm_l3_host_add create", timers_group, 1); \
\
    start_time = sal_time_usecs(); \
    for (entry_index = 0; entry_index < nof_entries; entry_index++) \
    { \
        rv = _add_api_(unit, &((_entry_info_)[entry_index])); \
        if (rv != BCM_E_NONE) \
        { \
            SHR_ERR_EXIT(rv, "Error, "#_add_api_" failed - entry index %d\n", entry_index); \
        } \
    } \
    run_time[0][run_index] = sal_time_usecs() - start_time; \
    utilex_ll_timer_stop(timers_group, 1); \
\
    /** Iterate over entries to update if update indication is set */ \
    if (action[1]) \
    { \
        /** Update the hosts payload */ \
        for (entry_index = 0; entry_index < nof_entries; entry_index++) \
        { \
            CTEST_DNX_L3_PERFORMANCE_RUN_##_module_##_PREP((_entry_info_)[entry_index], intf_add[entry_index], |=, BCM_IPMC_REPLACE); \
        } \
\
        LOG_CLI((BSL_META("Update "))); \
        utilex_ll_timer_set("bcm_l3_host_add update", timers_group, 2); \
        start_time = sal_time_usecs(); \
        for (entry_index = 0; entry_index < nof_entries; entry_index++) \
        { \
            rv = _add_api_(unit, &((_entry_info_)[entry_index])); \
            if (rv != BCM_E_NONE) \
            { \
                SHR_ERR_EXIT(rv, "Error, "#_add_api_" failed update - entry index %d\n", entry_index); \
            } \
        } \
        run_time[1][run_index] = sal_time_usecs() - start_time; \
        utilex_ll_timer_stop(timers_group, 2); \
    } \
\
    /** Iterate over all entries and delete them if delete indication is set */ \
    if (action[2]) \
    { \
        LOG_CLI((BSL_META("Delete "))); \
        utilex_ll_timer_set("bcm_l3_host_delete update", timers_group, 3); \
        start_time = sal_time_usecs(); \
        for (entry_index = 0; entry_index < nof_entries; entry_index++) \
        { \
            rv = _delete_api_(unit, &((_entry_info_)[entry_index])); \
            if (rv != BCM_E_NONE) \
            { \
                SHR_ERR_EXIT(rv, "Error, "#_delete_api_" failed - entry index %d\n", entry_index); \
            } \
        } \
        run_time[2][run_index] = sal_time_usecs() - start_time; \
        utilex_ll_timer_stop(timers_group, 3); \
    } \
    LOG_CLI((BSL_META("\n"))); \
} while (0)

/** Used to execute the performance measures for BCM host APIs */
static shr_error_e
dnx_l3_performance_host(
    int unit,
    int nof_entries,
    bcm_l3_host_t * host_info,
    uint32 *intf_add,
    uint32 *intf_update,
    uint8 *action,
    int run_index,
    int run_time[3][100],
    uint32 timers_group)
{
    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_L3_PERFORMANCE_RUN(host_info, bcm_l3_host_add, bcm_l3_host_delete, L3);

exit:
    SHR_FUNC_EXIT;
}

/** Used to execute the performance measures for BCM IPMC APIs */
static shr_error_e
dnx_l3_performance_ipmc(
    int unit,
    int nof_entries,
    bcm_ipmc_addr_t * ipmc_info,
    uint32 *intf_add,
    uint32 *intf_update,
    uint8 *action,
    int run_index,
    int run_time[3][100],
    uint32 timers_group)
{
    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_L3_PERFORMANCE_RUN(ipmc_info, bcm_ipmc_add, bcm_ipmc_remove, IPMC);

exit:
    SHR_FUNC_EXIT;
}

/** Used to execute the performance measures for BCM route APIs (Public, Private, FWD and RPF) */
static shr_error_e
dnx_l3_performance_route(
    int unit,
    int nof_entries,
    bcm_l3_route_t * route_info,
    uint32 *intf_add,
    uint32 *intf_update,
    uint8 *action,
    int run_index,
    int run_time[3][100],
    uint32 timers_group)
{
    SHR_FUNC_INIT_VARS(unit);
    CTEST_DNX_L3_PERFORMANCE_RUN(route_info, bcm_l3_route_add, bcm_l3_route_delete, L3);

exit:
    SHR_FUNC_EXIT;
}

/**
 * The following ctest exercises adding, updating and deleting L3 route entries.
 * Each step is being timed.
 */
static shr_error_e
dnx_l3_performance(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    uint8 is_host,
    uint8 is_ipmc,
    uint8 is_ipv6)
{
    int rv = 0;
    int iter, run;
    uint32 nof_entries = 0;
    uint32 nof_runs = 1;
    uint8 is_random = FALSE,
        is_external = FALSE,
        do_validation = FALSE, do_update_measures = FALSE,
        is_public = FALSE, is_rpf = FALSE, is_tcam = FALSE, is_em = FALSE, layer_id = 0;
    int run_time[3][100];
    int rate[3] = { 0, 0, 0 };
    char action_name[3][16] = { "Add", "Update", "Delete" };
    uint8 action[3] = { 1, 0, 0 };
    /** pointer for allocating memory for hosts, routes or ipmcs */
    void *entries_array = NULL;
    /** pointer for allocating memory for add and update payloads */
    uint32 *intf_add = NULL, *intf_update = NULL;
    /** Tolerance for LPM and TCAM is 20% */
    double performance_tolerance = 0.2;
    uint8 threads_disabled = FALSE;
    uint8 error_recovery_disabled = FALSE;
    uint8 timers_allocated = FALSE;
    uint8 is_sanitization_performed = TRUE;
    int exp_rate[3] = { 0, 0, 0 };

    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    uint32 timers_group;

    SHR_FUNC_INIT_VARS(unit);


    if (SAL_BOOT_PLISIM)
    {
        LOG_CLI((BSL_META("Performance testing is not supported in simulation\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_l3_performance_input_parameters_validation_and_update(unit,
                                                                              args,
                                                                              sand_control,
                                                                              is_host,
                                                                              is_ipmc,
                                                                              is_ipv6,
                                                                              &nof_entries, &nof_runs,
                                                                              &action[1] /** Update */ ,
                                                                              &action[2] /** Delete */ ,
                                                                              &is_random,
                                                                              &do_validation, &do_update_measures,
                                                                              &is_external,
                                                                              &is_public, &is_rpf, &is_tcam, &is_em,
                                                                              &layer_id));

    /** Additional validation for cases which are not supported and should not return error */
    if (!is_ipmc && is_em)
    {
        LOG_CLI((BSL_META("EM parameter is valid only for testing Internal IPMC tables\n")));
        SHR_EXIT();
    }
    if (is_ipmc && is_external && !is_tcam)
    {
        LOG_CLI((BSL_META("External IPMC tables are only TCAM\n")));
        SHR_EXIT();
    }
    if (is_public && (is_host || (is_ipmc && (is_tcam || is_em))))
    {
        LOG_CLI((BSL_META("Public parameter is valid only for Internal LPM tables\n")));
        SHR_EXIT();
    }
    if (is_external && is_public)
    {
        LOG_CLI((BSL_META("External Route tables are only private\n")));
        SHR_EXIT();
    }
    if (is_ipmc && !is_external && is_tcam && is_ipv6)
    {
        LOG_CLI((BSL_META("Internal IPMC IPv6 TCAM table is not valid\n")));
        SHR_EXIT();
    }
#if defined(INCLUDE_KBP)
    if ((!is_host && !is_ipmc)
        && ((!is_external && is_rpf) || (is_external && !dnx_data_elk.application.split_rpf_get(unit))))
    {
        LOG_CLI((BSL_META("%s %s RPF table does not exist\n"),
                 (is_external ? "External" : "Internal"), (is_ipv6 ? "IPv6" : "IPv4")));
        SHR_EXIT();
    }
#else
    if (is_rpf)
    {
        LOG_CLI((BSL_META("RPF table does not exist\n")));
        SHR_EXIT();
    }
#endif

    /** Disable threads */
    LOG_CLI((BSL_META("Disable threads\n")));
    threads_disabled = TRUE;
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("linkscan off");
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("counter off");
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("deinit rx_los");
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("deinit interrupt");
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("crps detach");

    /** Disable error recovery */
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    error_recovery_disabled = TRUE;

    SHR_IF_ERR_EXIT(dnx_l3_performance_entries_generate
                    (unit, is_random, is_host, is_ipmc, is_ipv6, is_external, is_public, is_rpf, is_tcam, is_em,
                     nof_entries, &entries_array, &intf_add, &intf_update));

    /** Allocate timer group ID with a given name*/
    sal_strncpy(group_name, "L3 perf Host", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    timers_allocated = TRUE;
    utilex_ll_timer_clear_all(timers_group);

    /** Print information */
    LOG_CLI((BSL_META("Running performance test on %s %s%s with %d entries and %d runs\n"),
             (is_ipv6 ? "IPv6" : "IPv4"),
             (is_host ? "Host" : is_ipmc ? "IPMC" : "Route"),
             (is_public ? " Public" : is_rpf ? " RPF" : is_tcam ? " TCAM" : is_em ? " EM" : ""),
             nof_entries, nof_runs));

    /** Execute test */
    for (run = 0; run < nof_runs; run++)
    {
        switch (layer_id)
        {
            case CTEST_DNX_L3_PERFORMANCE_LAYER_ID_BCM:
            {
                if (is_host)
                {
                    SHR_IF_ERR_EXIT(dnx_l3_performance_host
                                    (unit, nof_entries, (bcm_l3_host_t *) entries_array, intf_add, intf_update,
                                     action, run, run_time, timers_group));
                }
                else if (is_ipmc)
                {
                    SHR_IF_ERR_EXIT(dnx_l3_performance_ipmc
                                    (unit, nof_entries, (bcm_ipmc_addr_t *) entries_array, intf_add, intf_update,
                                     action, run, run_time, timers_group));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_l3_performance_route
                                    (unit, nof_entries, (bcm_l3_route_t *) entries_array, intf_add, intf_update,
                                     action, run, run_time, timers_group));
                }
            }
                break;
#if 0
            case CTEST_DNX_L3_PERFORMANCE_LAYER_ID_DBAL:
            {
                SHR_IF_ERR_EXIT(dnx_l3_performance_dbal
                                (unit, is_random, is_host, is_ipmc, is_ipv6, is_external, is_public, is_rpf, is_tcam,
                                 is_em, FALSE, nof_runs, nof_entries, action, run_time));
            }
                break;
            case CTEST_DNX_L3_PERFORMANCE_LAYER_ID_DBAL_FORCE:
            {
                SHR_IF_ERR_EXIT(dnx_l3_performance_dbal
                                (unit, is_random, is_host, is_ipmc, is_ipv6, is_external, is_public, is_rpf, is_tcam,
                                 is_em, TRUE, nof_runs, nof_entries, action, run_time));
            }
                break;
            case CTEST_DNX_L3_PERFORMANCE_LAYER_ID_ACCESS:
            {
                SHR_IF_ERR_EXIT(dnx_l3_performance_access
                                (unit, is_ipv6, is_rpf, is_tcam, is_host, is_ipmc, nof_runs, nof_entries, action,
                                 run_time));
            }
                break;
#endif
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid layer selected; should not be reached\n");
            }
        }
    }

    /** Stop and print timers */
    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    /** Print measurements */
    for (iter = 0; iter < 3; iter++)
    {
        if (action[iter])
        {
            dnx_l3_performance_detailed_performance_measurements_print(action_name[iter], run_time[iter],
                                                                       nof_runs, nof_entries, &rate[iter]);
        }
    }

    /** Validate if the rates meet the expected rates */
    if (do_validation)
    {
        /** Get the expected measurements */
        SHR_IF_ERR_EXIT(dnx_l3_performance_expected_performance_measurements_get_or_update
                        (unit, FALSE, is_host, is_ipmc, is_ipv6, is_external, is_rpf, is_tcam, is_em, 0,
                         &exp_rate[0], &exp_rate[1], &exp_rate[2]));

        for (iter = 0; iter < 3; iter++)
        {
            if (action[iter])
            {
                if ((double) rate[iter] <= ((double) exp_rate[iter] * (1.0 + performance_tolerance)) &&
                    (double) rate[iter] >= ((double) exp_rate[iter] * (1.0 - performance_tolerance)))
                {
                    LOG_CLI((BSL_META
                             ("Reached %s rate of %d within %.2f%% tolerance of the expected %d\n"),
                             action_name[iter], rate[iter], (100.0 * performance_tolerance), exp_rate[iter]));
                }
                else if ((double) rate[iter] > ((double) exp_rate[iter] * (1.0 + performance_tolerance)))
                {
                    LOG_CLI((BSL_META
                             ("Reached %s rate of %d, HIGHER than the expected %d with %.2f%% tolerance (%.0f)\n"),
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
                    LOG_CLI((BSL_META
                             ("Reached %s rate of %d, LOWER than the expected %d with %.2f%% tolerance (%.0f)\n"),
                             action_name[iter], rate[iter], exp_rate[iter], (100.0 * performance_tolerance),
                             ((double) exp_rate[iter] * (1.0 - performance_tolerance))));
                    SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
                }
            }
        }
    }
    else if (do_update_measures)
    {
        SHR_IF_ERR_EXIT(dnx_l3_performance_expected_performance_measurements_get_or_update
                        (unit, TRUE, is_host, is_ipmc, is_ipv6, is_external, is_rpf, is_tcam, is_em, 0,
                         &rate[0], &rate[1], &rate[2]));

        LOG_CLI((BSL_META("Performance measures updated\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Performance validation is not performed\n")));
    }

exit:
    if (timers_allocated)
    {
        SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    }
    /** Free allocated memory */
    if (entries_array != NULL)
    {
        sal_free(entries_array);
    }
    if (intf_add != NULL)
    {
        sal_free(intf_add);
    }
    if (intf_update != NULL)
    {
        sal_free(intf_update);
    }

    if (error_recovery_disabled)
    {
        /** Enable error recovery */
        DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    }
    if (threads_disabled)
    {
        /** Enable threads */
        LOG_CLI((BSL_META("Enable threads\n")));
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("crps attach");
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("init interrupt");
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("init rx_los");
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("counter on");
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("linkscan on");
    }
    SHR_FUNC_EXIT;
}

/**
 * The following ctest exercises adding, updating and deleting L3 FEC entries.
 * Each step is being timed.
 */
static shr_error_e
dnx_l3_fec_performance(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv = 0, run, iter, layer_id = 0;
    int run_time[3][100];
    int *fec_ids = NULL;
    int rate[3] = { 0, 0, 0 };
    int exp_rate[3];
    uint8 do_validation = FALSE;
    uint8 do_update_measures = FALSE;
    uint8 threads_disabled = FALSE;
    uint8 error_recovery_disabled = FALSE;
    uint8 timers_allocated = FALSE;
    uint8 is_sanitization_performed = TRUE;
    uint32 nof_runs = 1;
    uint32 nof_entries = 0;
    uint32 timers_group;
    uint32 nof_actions = 3;
    double performance_tolerance = 0.2;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    char action_name[3][16] = { "Create", "Update", "Delete" };
    bcm_l3_egress_t *entries_array = NULL;
    bcm_l3_egress_t fec_entry;
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        LOG_CLI((BSL_META("Performance testing is not supported in simulation\n")));
        SHR_EXIT();
    }

    /** Get and validate input parameters */
    {
        SH_SAND_GET_UINT32(CTEST_DNX_L3_PERFORMANCE_ENTRIES, nof_entries);
        SH_SAND_GET_UINT32(CTEST_DNX_L3_PERFORMANCE_RUNS, nof_runs);

        SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_VALIDATE, do_validation);
        SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_UPDATE_MEASURES, do_update_measures);

        if (nof_runs < 1 || nof_runs > 100)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Number of runs need to be between 1 and 100\n");
        }
        if (nof_entries == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Number of entries is 0\n");
        }

        if (do_validation && do_update_measures)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid combination of parameters - " CTEST_DNX_L3_PERFORMANCE_VALIDATE " and "
                         CTEST_DNX_L3_PERFORMANCE_UPDATE_MEASURES "\n");
        }
        /** Update the validation indication depending on the device ID */
        SHR_IF_ERR_EXIT(dnx_l3_performance_validation_check_for_device(unit, &do_validation, &do_update_measures));

        {
            uint8 is_public, is_rpf, is_tcam, is_em, is_random;
            char *layer;
            SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_RANDOM, is_random);
            SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_PUBLIC, is_public);
            SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_RPF, is_rpf);
            SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_TCAM, is_tcam);
            SH_SAND_GET_BOOL(CTEST_DNX_L3_PERFORMANCE_EM, is_em);
            SH_SAND_GET_STR(CTEST_DNX_L3_PERFORMANCE_LAYER, layer);
            if (strncasecmp(layer, CTEST_DNX_L3_PERFORMANCE_LAYER_BCM, 16))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "FEC performance testing supports only BCM layer testing. \n");
            }
            if (is_random)
            {
                LOG_CLI((BSL_META("Irrelevant option 'random' provided to FEC performance test. Ignoring.\n")));
            }
            if (is_public)
            {
                LOG_CLI((BSL_META("Irrelevant option 'Public' provided to FEC performance test. Ignoring.\n")));
            }
            if (is_rpf)
            {
                LOG_CLI((BSL_META("Irrelevant option 'RPF' provided to FEC performance test. Ignoring.\n")));
            }
            if (is_tcam || is_em)
            {
                LOG_CLI((BSL_META("Irrelevant option 'tcam' or 'em' provided to FEC performance test. Ignoring.\n")));
            }
        }
    }

    /** Disable error recovery */
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    error_recovery_disabled = TRUE;

    entries_array = (bcm_l3_egress_t *) sal_alloc(sizeof(bcm_l3_egress_t) * nof_entries, "bcm_l3_egress_t");
    if (entries_array == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d FEC entries\n", nof_entries);
    }
    sal_memset(entries_array, 0, sizeof(bcm_l3_egress_t) * nof_entries);
    fec_ids = (int *) sal_alloc(sizeof(int) * nof_entries, "fec ids array");
    if (fec_ids == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d FEC entries\n", nof_entries);
    }

    printf("Generating %d nof entries per hierarchy \n", nof_entries);
    SHR_IF_ERR_EXIT(dnx_l3_performance_fec_entries_generate(unit, nof_entries, entries_array, fec_ids));

    /** Allocate timer group ID with a given name*/
    sal_strncpy(group_name, "L3 perf FEC", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }

    timers_allocated = TRUE;
    utilex_ll_timer_clear_all(timers_group);

    LOG_CLI((BSL_META("Running performance test on FEC with %d entries and %d runs\n"), nof_entries, nof_runs));

    LOG_CLI((BSL_META("Disable threads\n")));
    threads_disabled = TRUE;
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("linkscan off");
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("counter off");
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("deinit rx_los");
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("deinit interrupt");
    CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("crps detach");

    /** Execute test */
    for (run = 0; run < nof_runs; run++)
    {
        switch (layer_id)
        {
            case CTEST_DNX_L3_PERFORMANCE_LAYER_ID_BCM:
            {
                uint32 start_time = sal_time_usecs();
                for (iter = 0; iter < nof_entries; iter++)
                {
                    sal_memcpy(&fec_entry, &entries_array[iter], sizeof(bcm_l3_egress_t));
                    /** Set timer for action create */
                    utilex_ll_timer_set("FEC create", timers_group, 1);
                    bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &fec_entry, &fec_ids[iter]);
                    /** Stop timer for action create */
                    utilex_ll_timer_stop(timers_group, 1);
                }
                run_time[0][run] = sal_time_usecs() - start_time;

                start_time = sal_time_usecs();
                for (iter = 0; iter < nof_entries; iter++)
                {
                    sal_memcpy(&fec_entry, &entries_array[iter], sizeof(bcm_l3_egress_t));
                    /** Update FEC destination */
                    fec_entry.intf++;
                    fec_entry.encap_id++;
                    /** Set timer for action update */
                    utilex_ll_timer_set("FEC update", timers_group, 2);
                    bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY | BCM_L3_REPLACE, &fec_entry,
                                         &fec_ids[iter]);
                    /** Stop timer for action update */
                    utilex_ll_timer_stop(timers_group, 2);
                }
                run_time[1][run] = sal_time_usecs() - start_time;

                start_time = sal_time_usecs();
                for (iter = 0; iter < nof_entries; iter++)
                {
                    /** Set timer for action destroy */
                    utilex_ll_timer_set("FEC destroy", timers_group, 3);
                    bcm_l3_egress_destroy(unit, fec_ids[iter]);
                    /** Stop timer for action destroy */
                    utilex_ll_timer_stop(timers_group, 3);
                }
                run_time[2][run] = sal_time_usecs() - start_time;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid layer selected \n");
            }
        }
    }

    /** Stop and print timers */
    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    for (iter = 0; iter < nof_actions; iter++)
    {
        /** Print measurements */
        dnx_l3_performance_detailed_performance_measurements_print(action_name[iter], run_time[iter],
                                                                   nof_runs, nof_entries, &rate[iter]);

        /** Validate if the rates meet the expected rates */
        if (do_validation)
        {
            /** Get the expected measurements */
            SHR_IF_ERR_EXIT(dnx_l3_performance_expected_performance_measurements_get_or_update
                            (unit, FALSE, 0, 0, 0, 0, 0, 0, 0, 1, &exp_rate[0], &exp_rate[1], &exp_rate[2]));

            if ((double) rate[iter] <= ((double) exp_rate[iter] * (1.0 + performance_tolerance)) &&
                (double) rate[iter] >= ((double) exp_rate[iter] * (1.0 - performance_tolerance)))
            {
                LOG_CLI((BSL_META
                         ("Reached %s rate of %d within %.2f%% tolerance of the expected %d\n"),
                         action_name[iter], rate[iter], (100.0 * performance_tolerance), exp_rate[iter]));
            }
            else if ((double) rate[iter] > ((double) exp_rate[iter] * (1.0 + performance_tolerance)))
            {
                LOG_CLI((BSL_META
                         ("Reached %s rate of %d, HIGHER than the expected %d with %.2f%% tolerance (%.0f)\n"),
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
                LOG_CLI((BSL_META
                         ("Reached %s rate of %d, LOWER than the expected %d with %.2f%% tolerance (%.0f)\n"),
                         action_name[iter], rate[iter], exp_rate[iter], (100.0 * performance_tolerance),
                         ((double) exp_rate[iter] * (1.0 - performance_tolerance))));
                SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            }
        }
        else if (do_update_measures)
        {
            uint32 timer_rate = rate[iter];
            /** Get the expected measurements */
            SHR_IF_ERR_EXIT(dnx_l3_performance_expected_performance_measurements_get_or_update
                            (unit, FALSE, 0, 0, 0, 0, 0, 0, 0, 1, &exp_rate[0], &exp_rate[1], &exp_rate[2]));
            sal_memcpy(rate, exp_rate, sizeof(uint32) * 3);
            rate[iter] = timer_rate;
            SHR_IF_ERR_EXIT(dnx_l3_performance_expected_performance_measurements_get_or_update
                            (unit, TRUE, 0, 0, 0, 0, 0, 0, 0, 1, &rate[0], &rate[1], &rate[2]));
            LOG_CLI((BSL_META("Performance measures updated\n")));
        }
        else
        {
            LOG_CLI((BSL_META("Performance validation is not performed\n")));
        }
    }

exit:
    if (timers_allocated)
    {
        SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    }
    /** Free allocated memory*/
    if (entries_array != NULL)
    {
        sal_free(entries_array);
    }
    if (fec_ids != NULL)
    {
        sal_free(fec_ids);
    }

    if (error_recovery_disabled)
    {
        /** Enable error recovery */
        DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    }
    if (threads_disabled)
    {
        /** Enable threads */
        LOG_CLI((BSL_META("Enable threads\n")));
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("crps attach");
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("init interrupt");
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("init rx_los");
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("counter on");
        CTEST_DNX_L3_SH_CMD_EXEC_AND_PRINT("linkscan on");
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_performance_host_ipv4_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_l3_performance(unit, args, sand_control, TRUE /** is_host */ ,
                                       FALSE /** is_ipmc */ ,
                                       FALSE /** is_ipv6 */ ));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_performance_host_ipv6_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_l3_performance(unit, args, sand_control, TRUE /** is_host */ ,
                                       FALSE /** is_ipmc */ ,
                                       TRUE /** is_ipv6 */ ));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_performance_route_ipv4_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_l3_performance(unit, args, sand_control, FALSE /** is_host */ ,
                                       FALSE /** is_ipmc */ ,
                                       FALSE /** is_ipv6 */ ));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_performance_route_ipv6_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_l3_performance(unit, args, sand_control, FALSE /** is_host */ ,
                                       FALSE /** is_ipmc */ ,
                                       TRUE /** is_ipv6 */ ));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_performance_ipmc_ipv4_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_l3_performance(unit, args, sand_control, FALSE /** is_host */ ,
                                       TRUE /** is_ipmc */ ,
                                       FALSE /** is_ipv6 */ ));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_l3_performance_ipmc_ipv6_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_l3_performance(unit, args, sand_control, FALSE /** is_host */ ,
                                       TRUE /** is_ipmc */ ,
                                       TRUE /** is_ipv6 */ ));
exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
static sh_sand_option_t dnx_l3_performance_cmd_options[] = {
    {CTEST_DNX_L3_PERFORMANCE_ENTRIES, SAL_FIELD_TYPE_UINT32, "Number of entries", "10000"},
    {CTEST_DNX_L3_PERFORMANCE_RUNS, SAL_FIELD_TYPE_UINT32, "Number of runs", "10"},
    {CTEST_DNX_L3_PERFORMANCE_UPDATE, SAL_FIELD_TYPE_BOOL, "Indication to perform update measurements", "YES"},
    {CTEST_DNX_L3_PERFORMANCE_DELETE, SAL_FIELD_TYPE_BOOL, "Indication to perform delete measurements", "YES"},
    {CTEST_DNX_L3_PERFORMANCE_RANDOM, SAL_FIELD_TYPE_BOOL, "Indication to validate measurements with random entries", "YES"},
    {CTEST_DNX_L3_PERFORMANCE_VALIDATE, SAL_FIELD_TYPE_BOOL, "Indication to validate the measurements", "NO"},
    {CTEST_DNX_L3_PERFORMANCE_UPDATE_MEASURES, SAL_FIELD_TYPE_BOOL, "Indication to update the measurements", "NO"},
    {CTEST_DNX_L3_PERFORMANCE_PUBLIC, SAL_FIELD_TYPE_BOOL, "Indication to validate the measurements of public routes", "NO"},
    {CTEST_DNX_L3_PERFORMANCE_RPF, SAL_FIELD_TYPE_BOOL, "Indication to validate the measurements on Route RPF table", "NO"},
    {CTEST_DNX_L3_PERFORMANCE_TCAM, SAL_FIELD_TYPE_BOOL, "Indication to validate the measurements IPMC TCAM table", "NO"},
    {CTEST_DNX_L3_PERFORMANCE_EM, SAL_FIELD_TYPE_BOOL, "Indication to validate the measurements IPMC EM table", "NO"},
    {CTEST_DNX_L3_PERFORMANCE_LAYER, SAL_FIELD_TYPE_STR, "Select starting layer", CTEST_DNX_L3_PERFORMANCE_LAYER_BCM},
    {NULL}
};

static sh_sand_invoke_t dnx_l3_performance_host_tests[] = {
    {"EM", CTEST_DNX_L3_PERFORMANCE_VALIDATE, CTEST_POSTCOMMIT},
    {NULL}
};
static sh_sand_invoke_t dnx_l3_performance_route_tests[] = {
    {"LPM", CTEST_DNX_L3_PERFORMANCE_VALIDATE, CTEST_POSTCOMMIT},
    {"LPM_RPF", CTEST_DNX_L3_PERFORMANCE_VALIDATE " " CTEST_DNX_L3_PERFORMANCE_RPF, CTEST_POSTCOMMIT},
    {NULL}
};
static sh_sand_invoke_t dnx_l3_performance_ipmc_tests[] = {
    {"LPM", CTEST_DNX_L3_PERFORMANCE_VALIDATE, CTEST_POSTCOMMIT},
    {"EM", CTEST_DNX_L3_PERFORMANCE_VALIDATE " " CTEST_DNX_L3_PERFORMANCE_EM, CTEST_POSTCOMMIT},
    {"TCAM", CTEST_DNX_L3_PERFORMANCE_VALIDATE " " CTEST_DNX_L3_PERFORMANCE_TCAM " " CTEST_DNX_L3_PERFORMANCE_ENTRIES "=5000", CTEST_POSTCOMMIT},
    {NULL}
};

/*
 * Performance Host
 */
static sh_sand_man_t dnx_l3_performance_host_ipv4_man = {
    "Insert, update and delete L3 Host IPv4 entries",
    "Insert, update and delete L3 Host IPv4 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
static sh_sand_man_t dnx_l3_performance_host_ipv6_man = {
    "Insert, update and delete L3 Host IPv6 entries",
    "Insert, update and delete L3 Host IPv6 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
static sh_sand_man_t dnx_l3_performance_host_man = {
    "Insert, update and delete L3 Host entries",
    "Insert, update and delete L3 Host IPv4 or IPv6 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
static sh_sand_cmd_t dnx_l3_performance_host_cmds[] = {
    {"IPv4", dnx_l3_performance_host_ipv4_cmd, NULL, dnx_l3_performance_cmd_options, &dnx_l3_performance_host_ipv4_man, NULL, dnx_l3_performance_host_tests, CTEST_UM},
    {"IPv6", dnx_l3_performance_host_ipv6_cmd, NULL, dnx_l3_performance_cmd_options, &dnx_l3_performance_host_ipv6_man, NULL, dnx_l3_performance_host_tests, CTEST_UM},
    {NULL}
};

/*
 * Performance Route
 */
static sh_sand_man_t dnx_l3_performance_route_ipv4_man = {
    "Insert, update and delete L3 Route IPv4 entries",
    "Insert, update and delete L3 Route IPv4 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
static sh_sand_man_t dnx_l3_performance_route_ipv6_man = {
    "Insert, update and delete L3 Route IPv6 entries",
    "Insert, update and delete L3 Route IPv6 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
static sh_sand_man_t dnx_l3_performance_route_man = {
    "Insert, update and delete L3 Route entries",
    "Insert, update and delete L3 Route IPv4 or IPv6 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
static sh_sand_cmd_t dnx_l3_performance_route_cmds[] = {
    {"IPv4", dnx_l3_performance_route_ipv4_cmd, NULL, dnx_l3_performance_cmd_options, &dnx_l3_performance_route_ipv4_man, NULL, dnx_l3_performance_route_tests, CTEST_UM},
    {"IPv6", dnx_l3_performance_route_ipv6_cmd, NULL, dnx_l3_performance_cmd_options, &dnx_l3_performance_route_ipv6_man, NULL, dnx_l3_performance_route_tests, CTEST_UM},
    {NULL}
};

/*
 * Performance IPMC
 */
static sh_sand_man_t dnx_l3_performance_ipmc_ipv4_man = {
    "Insert, update and delete IPMC IPv4 entries",
    "Insert, update and delete IPMC IPv4 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
static sh_sand_man_t dnx_l3_performance_ipmc_ipv6_man = {
    "Insert, update and delete IPMC IPv6 entries",
    "Insert, update and delete IPMC IPv6 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
static sh_sand_man_t dnx_l3_performance_ipmc_man = {
    "Insert, update and delete IPMC entries",
    "Insert, update and delete IPMC IPv4 or IPv6 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};

static sh_sand_man_t dnx_l3_performance_fec_man = {
    "Insert, update and delete FEC entries",
    "Insert, update and delete FEC entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};

static sh_sand_cmd_t dnx_l3_performance_ipmc_cmds[] = {
    {"IPv4", dnx_l3_performance_ipmc_ipv4_cmd, NULL, dnx_l3_performance_cmd_options, &dnx_l3_performance_ipmc_ipv4_man, NULL, dnx_l3_performance_ipmc_tests, CTEST_UM},
    {"IPv6", dnx_l3_performance_ipmc_ipv6_cmd, NULL, dnx_l3_performance_cmd_options, &dnx_l3_performance_ipmc_ipv6_man, NULL, dnx_l3_performance_ipmc_tests, CTEST_UM},
    {NULL}
};

/*
 * Performance
 */
sh_sand_man_t sh_dnx_l3_performance_man = {
    "Insert, update and delete Host/Route/IPMC IPv4 or IPv6 entries",
    "Insert, update and delete Host/Route/IPMC IPv4 or IPv6 entries while recording how much time each step takes and outputs and/or validates the entry rates",
    "",
    ""
};
sh_sand_cmd_t dnx_l3_performance_cmds[] = {
    {"Host", NULL, dnx_l3_performance_host_cmds, NULL, &dnx_l3_performance_host_man, NULL, NULL, CTEST_UM},
    {"Route", NULL, dnx_l3_performance_route_cmds, NULL, &dnx_l3_performance_route_man, NULL, NULL, CTEST_UM},
    {"IPMC", NULL, dnx_l3_performance_ipmc_cmds, NULL, &dnx_l3_performance_ipmc_man, NULL, NULL, CTEST_UM},
    {"FEC", dnx_l3_fec_performance, NULL, dnx_l3_performance_cmd_options, &dnx_l3_performance_fec_man, NULL, NULL, CTEST_UM},
    {NULL}
};

/* *INDENT-ON* */
