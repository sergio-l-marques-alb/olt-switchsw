/*
 * ! \file ctest_dnx_apt_object_l3_host_def.c
 * Contains the functions and definitions for L3 host API performance test objects.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
#include <shared/bsl.h>

/*
 * Include files.
 * {
 */

#include "ctest_dnx_apt.h"
#include <bcm/l3.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_RUNS      10
#define DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_CALLS     50000

/** EM rates */
#define DNX_APT_L3_HOST_IPV4_INSERT_EM_RATE_JR2_A0  39000
#define DNX_APT_L3_HOST_IPV4_UPDATE_EM_RATE_JR2_A0  37000
#define DNX_APT_L3_HOST_IPV6_INSERT_EM_RATE_JR2_A0  38000
#define DNX_APT_L3_HOST_IPV6_UPDATE_EM_RATE_JR2_A0  35000

#define DNX_APT_L3_HOST_IPV4_INSERT_EM_RATE         70000
#define DNX_APT_L3_HOST_IPV4_UPDATE_EM_RATE         62000
#define DNX_APT_L3_HOST_IPV6_INSERT_EM_RATE         64000
#define DNX_APT_L3_HOST_IPV6_UPDATE_EM_RATE         56000

/** KBP rates */
#define DNX_APT_L3_HOST_IPV4_INSERT_KBP_RATE        115000
#define DNX_APT_L3_HOST_IPV4_UPDATE_KBP_RATE        195000
#define DNX_APT_L3_HOST_IPV6_INSERT_KBP_RATE        70000
#define DNX_APT_L3_HOST_IPV6_UPDATE_KBP_RATE        140000

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_L3_HOST_TIME_THRESHOLD(_unit_, _ip_, _test_type_) \
    dnx_kbp_device_enabled(_unit_) ? \
            /** KBP */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_HOST_##_ip_##_##_test_type_##_KBP_RATE) : \
            /** LEM */ dnxc_data_mgmt_is_jr2_a0(_unit_) ? \
                        /** JR2_A0 */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_HOST_##_ip_##_##_test_type_##_EM_RATE_JR2_A0) : \
                        /** All other */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_HOST_##_ip_##_##_test_type_##_EM_RATE)

#define DNX_APT_L3_HOST_OPTIONAL_THRESHOLD_INFO(_info_, _ip_, _test_type_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "EM: %.3fus (JR2_A0: %.3fus)\nKBP: %.3fus", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L3_HOST_##_ip_##_##_test_type_##_EM_RATE), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L3_HOST_##_ip_##_##_test_type_##_EM_RATE_JR2_A0), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L3_HOST_##_ip_##_##_test_type_##_KBP_RATE))
/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the L3 host API performance tests */
typedef struct
{
    bcm_l3_host_t host;
    uint32 intf_insert;
    uint32 intf_update;
} dnx_apt_l3_host_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

extern shr_error_e dnx_apt_kbp_sync(
    int unit);
extern shr_error_e dnx_apt_kbp_application_support_ipv4(
    int unit);
extern shr_error_e dnx_apt_kbp_application_support_ipv6(
    int unit);

/***********************************************************************************************************************
 * L3 Host time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_l3_host_ipv4_insert_time_threshold(
    int unit)
{
    return DNX_APT_L3_HOST_TIME_THRESHOLD(unit, IPV4, INSERT);
}

static double
dnx_apt_l3_host_ipv4_update_time_threshold(
    int unit)
{
    return DNX_APT_L3_HOST_TIME_THRESHOLD(unit, IPV4, UPDATE);
}

static double
dnx_apt_l3_host_ipv6_insert_time_threshold(
    int unit)
{
    return DNX_APT_L3_HOST_TIME_THRESHOLD(unit, IPV6, INSERT);
}

static double
dnx_apt_l3_host_ipv6_update_time_threshold(
    int unit)
{
    return DNX_APT_L3_HOST_TIME_THRESHOLD(unit, IPV6, UPDATE);
}

/***********************************************************************************************************************
 * L3 Host optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_host_ipv4_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_HOST_OPTIONAL_THRESHOLD_INFO(info, IPV4, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_host_ipv4_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_HOST_OPTIONAL_THRESHOLD_INFO(info, IPV4, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_host_ipv6_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_HOST_OPTIONAL_THRESHOLD_INFO(info, IPV6, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_host_ipv6_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_HOST_OPTIONAL_THRESHOLD_INFO(info, IPV6, UPDATE);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * L3 Host shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_host_alloc(
    int unit,
    int number_of_hosts,
    dnx_apt_l3_host_info_t ** l3_host_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*l3_host_info, (sizeof(dnx_apt_l3_host_info_t) * number_of_hosts), "dnx_apt_l3_host_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_host_generate(
    int unit,
    int number_of_hosts,
    dnx_apt_l3_host_info_t * l3_host_info,
    uint8 is_ipv6)
{
    int iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!is_ipv6)
    {
        SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, 16 * 1024 * 1024));
    }

    dnx_apt_random_seed(0);
    for (iter = 0; iter < number_of_hosts; iter++)
    {
        bcm_l3_host_t_init(&l3_host_info[iter].host);

        /** Randomized host entries */
        l3_host_info[iter].host.l3a_flags = 0;

        if (is_ipv6)
        {
            int ipv6_addr_index = 16;

            l3_host_info[iter].host.l3a_flags |= BCM_L3_IP6;
            while (--ipv6_addr_index)
            {
                l3_host_info[iter].host.l3a_ip6_addr[ipv6_addr_index] = dnx_apt_random_get() & 0xFF;
            }
            l3_host_info[iter].host.l3a_ip6_addr[0] = 0x00;
        }
        else
        {
            l3_host_info[iter].host.l3a_ip_addr = dnx_apt_unique_random_get(iter) & 0x1FFFFFFF;
        }

        l3_host_info[iter].host.l3a_vrf = (dnx_apt_random_get() & 0xFE) + 1;

        l3_host_info[iter].intf_insert = (dnx_apt_random_get() & 0x3FFE) + 1;
        l3_host_info[iter].intf_update = (dnx_apt_random_get() & 0x3FFE) + 1;

        l3_host_info[iter].host.l3a_intf = l3_host_info[iter].intf_insert;
        l3_host_info[iter].host.encap_id = (dnx_apt_random_get() & 0x3FFE) + 1;
    }

    SHR_IF_ERR_EXIT(dnx_apt_kbp_sync(unit));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 Host test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_host_ipv4_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_l3_host_alloc(unit, number_of_calls, (dnx_apt_l3_host_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_l3_host_generate(unit, number_of_calls, (dnx_apt_l3_host_info_t *) * custom_data, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_host_ipv6_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_host_alloc(unit, number_of_calls, (dnx_apt_l3_host_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_l3_host_generate(unit, number_of_calls, (dnx_apt_l3_host_info_t *) * custom_data, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 Route shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_host_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_l3_host_add(unit, &((dnx_apt_l3_host_info_t *) custom_data)[call_iter].host);
}

static shr_error_e
dnx_apt_l3_host_update_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    dnx_apt_l3_host_info_t *l3_host_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        l3_host_info = &((dnx_apt_l3_host_info_t *) custom_data)[iter];

        l3_host_info->host.l3a_flags &= (~BCM_L3_REPLACE);
        l3_host_info->host.l3a_intf = l3_host_info->intf_insert;

        SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &l3_host_info->host));

        l3_host_info->host.l3a_flags |= BCM_L3_REPLACE;
        l3_host_info->host.l3a_intf = l3_host_info->intf_update;
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 Host DBAL exec functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_host_dbal_add(
    int unit,
    bcm_l3_host_t * l3_host)
{
    uint32 result_type = -1;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_kbp_device_enabled(unit))
    {
        if (_SHR_IS_FLAG_SET(l3_host->l3a_flags, BCM_L3_IP6))
        {
            result_type = DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD, &entry_handle_id));
            dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, l3_host->l3a_ip6_addr);
        }
        else
        {
            result_type = DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4, l3_host->l3a_ip_addr);
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(l3_host->l3a_flags, BCM_L3_IP6))
        {
            result_type = DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST, &entry_handle_id));
            dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, l3_host->l3a_ip6_addr);
        }
        else
        {
            result_type = DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4, l3_host->l3a_ip_addr);
        }
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, l3_host->l3a_vrf);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                 BCM_FORWARD_ENCAP_ID_VAL_GET(l3_host->encap_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE,
                                 BCM_L3_ITF_VAL_GET(l3_host->l3a_intf));
    SHR_IF_ERR_EXIT(dbal_entry_commit
                    (unit, entry_handle_id,
                     (_SHR_IS_FLAG_SET(l3_host->l3a_flags, BCM_L3_REPLACE) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_host_dbal_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return dnx_apt_l3_host_dbal_add(unit, &((dnx_apt_l3_host_info_t *) custom_data)[call_iter].host);
}

static shr_error_e
dnx_apt_l3_host_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    return bcm_l3_host_delete_all(unit, NULL);
}

/*
 * }
 */

/*
 * GLOBALs
 * {
 */

/***********************************************************************************************************************
 * IPv4
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_l3_host_ipv4_insert_object = {
    /** Test name */
    "L3_Host_IPv4_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_host_add() when inserting new IPv4 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in LEM.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_l3_host_ipv4_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_host_ipv4_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_host_ipv4_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_host_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_host_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_host_ipv4_insert_dbal_object = {
    /** Test name */
    "L3_Host_IPv4_Insert_DBAL",
    /** Test description */
    "Measures the performance of a custom DBAL insertion API when inserting new IPv4 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in LEM.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_l3_host_ipv4_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_host_ipv4_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_host_ipv4_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_host_dbal_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_host_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_host_ipv4_update_object = {
    /** Test name */
    "L3_Host_IPv4_Update",
    /** Test description */
    "Measures the performance of bcm_l3_host_add() when updating existing IPv4 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in LEM.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_l3_host_ipv4_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_host_ipv4_update_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_host_ipv4_init,
    /** Run init procedure */
    dnx_apt_l3_host_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_host_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_host_clear_exec,
    /** Test deinit procedure */
    NULL
};

/***********************************************************************************************************************
 * IPv6
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_l3_host_ipv6_insert_object = {
    /** Test name */
    "L3_Host_IPv6_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_host_add() when inserting new IPv6 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in LEM.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_l3_host_ipv6_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_host_ipv6_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_host_ipv6_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_host_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_host_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_host_ipv6_insert_dbal_object = {
    /** Test name */
    "L3_Host_IPv6_Insert_DBAL",
    /** Test description */
    "Measures the performance of a custom DBAL insertion API when inserting new IPv6 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in LEM.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_l3_host_ipv6_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_host_ipv6_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_host_ipv6_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_host_dbal_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_host_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_host_ipv6_update_object = {
    /** Test name */
    "L3_Host_IPv6_Update",
    /** Test description */
    "Measures the performance of bcm_l3_host_add() when updating existing IPv6 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in LEM.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_HOST_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_l3_host_ipv6_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_host_ipv6_update_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_host_ipv6_init,
    /** Run init procedure */
    dnx_apt_l3_host_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_host_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_host_clear_exec,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
