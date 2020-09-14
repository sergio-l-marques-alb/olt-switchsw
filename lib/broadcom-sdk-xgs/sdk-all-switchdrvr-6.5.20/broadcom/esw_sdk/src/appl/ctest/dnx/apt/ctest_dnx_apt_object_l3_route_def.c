/*
 * ! \file ctest_dnx_apt_object_l3_route_def.c
 * Contains the functions and definitions for L3 route API performance test objects.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_RUNS     10
#define DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_CALLS    10000

/** KAPS rates */
#define DNX_APT_L3_ROUTE_IPV4_INSERT_KAPS_RATE      56000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_KAPS_RATE      102000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_KAPS_RATE      34000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_KAPS_RATE      89000

#define DNX_APT_L3_ROUTE_IPV4_INSERT_KAPS_RATE_HIGHEST  66000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_KAPS_RATE_HIGHEST  123000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_KAPS_RATE_HIGHEST  42000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_KAPS_RATE_HIGHEST  101000

/** KBP rates */
#define DNX_APT_L3_ROUTE_IPV4_INSERT_KBP_RATE       69000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_KBP_RATE       97000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_KBP_RATE       52000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_KBP_RATE       82000

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_L3_ROUTE_TIME_THRESHOLD(_unit_, _ip_, _test_type_) \
    dnx_kbp_device_enabled(_unit_) ? \
                /** KBP */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ROUTE_##_ip_##_##_test_type_##_KBP_RATE) : \
                /** KAPS */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ROUTE_##_ip_##_##_test_type_##_KAPS_RATE)

#define DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(_info_, _ip_, _test_type_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "KAPS: %.3fus (varies to %.3fus)\nKBP: %.3fus", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L3_ROUTE_##_ip_##_##_test_type_##_KAPS_RATE), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L3_ROUTE_##_ip_##_##_test_type_##_KAPS_RATE_HIGHEST), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L3_ROUTE_##_ip_##_##_test_type_##_KBP_RATE)) \
 /* 
  * }
  */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the L3 route API performance tests */
typedef struct
{
    bcm_l3_route_t route;
    uint32 intf_insert;
    uint32 intf_update;
} dnx_apt_l3_route_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/***********************************************************************************************************************
 * L3 Route time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_l3_route_ipv4_insert_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV4, INSERT);
}

static double
dnx_apt_l3_route_ipv4_update_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV4, UPDATE);
}

static double
dnx_apt_l3_route_ipv6_insert_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV6, INSERT);
}

static double
dnx_apt_l3_route_ipv6_update_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV6, UPDATE);
}

/***********************************************************************************************************************
 * L3 Route optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_route_ipv4_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV4, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_route_ipv4_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV4, UPDATE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_route_ipv6_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV6, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_route_ipv6_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV6, UPDATE);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * KBP support functions
 **********************************************************************************************************************/
shr_error_e
dnx_apt_kbp_sync(
    int unit)
{
    int is_kbp_sync_done = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_kbp_device_enabled(unit))
    {
        SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, bcmSwitchExternalTcamSync, &is_kbp_sync_done));
        if (!is_kbp_sync_done)
        {
            SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_kbp_application_support(
    int unit,
    dnx_data_elk_application_feature_e kbp_application_feature)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_kbp_device_enabled(unit))
    {
        if (!dnx_data_elk.application.feature_get(unit, kbp_application_feature))
        {
            LOG_CLI((BSL_META("Required KBP application is not enabled.\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
        }
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_apt_kbp_application_support_ipv4(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_SET_CURRENT_ERR(dnx_apt_kbp_application_support(unit, dnx_data_elk_application_ipv4));
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_apt_kbp_application_support_ipv6(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_SET_CURRENT_ERR(dnx_apt_kbp_application_support(unit, dnx_data_elk_application_ipv6));
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 Route shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_route_alloc(
    int unit,
    int number_of_routes,
    dnx_apt_l3_route_info_t ** l3_route_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*l3_route_info, (sizeof(dnx_apt_l3_route_info_t) * number_of_routes), "dnx_apt_l3_route_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_route_generate(
    int unit,
    int number_of_routes,
    dnx_apt_l3_route_info_t * l3_route_info,
    uint8 is_ipv6)
{
    int iter = 0;
    bcm_ip6_t ipv6_mask =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00 };
    uint32 flags2 = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!is_ipv6)
    {
        SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, 16 * 1024 * 1024));
    }
    if (dnx_kbp_device_enabled(unit) && kbp_mngr_split_rpf_in_use(unit))
    {
        flags2 = BCM_L3_FLAGS2_FWD_ONLY;
    }

    dnx_apt_random_seed(0);
    for (iter = 0; iter < number_of_routes; iter++)
    {
        bcm_l3_route_t_init(&l3_route_info[iter].route);

        /** Randomized route entries */
        l3_route_info[iter].route.l3a_flags = 0;
        l3_route_info[iter].route.l3a_flags2 = flags2;

        if (is_ipv6)
        {
            int ipv6_addr_index = 16;

            l3_route_info[iter].route.l3a_flags |= BCM_L3_IP6;
            while (--ipv6_addr_index)
            {
                l3_route_info[iter].route.l3a_ip6_net[ipv6_addr_index] = dnx_apt_random_get() & 0xFF;
            }
            l3_route_info[iter].route.l3a_ip6_net[0] = 0x00;
            sal_memcpy(l3_route_info[iter].route.l3a_ip6_mask, ipv6_mask, 16);
        }
        else
        {
            l3_route_info[iter].route.l3a_subnet = dnx_apt_unique_random_get(iter) & 0x1FFFFFFF;
            l3_route_info[iter].route.l3a_ip_mask = 0xFFFFFFFF;
        }

        l3_route_info[iter].route.l3a_vrf = (dnx_apt_random_get() & 0xFE) + 1;

        l3_route_info[iter].intf_insert = (dnx_apt_random_get() & 0x3FFE) + 1;
        l3_route_info[iter].intf_update = (dnx_apt_random_get() & 0x3FFE) + 1;
        l3_route_info[iter].route.l3a_intf = l3_route_info[iter].intf_insert;
    }

    SHR_IF_ERR_EXIT(dnx_apt_kbp_sync(unit));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 Route test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_route_ipv4_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_l3_route_alloc(unit, number_of_calls, (dnx_apt_l3_route_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_l3_route_generate(unit, number_of_calls, (dnx_apt_l3_route_info_t *) * custom_data, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_route_ipv6_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_l3_route_alloc(unit, number_of_calls, (dnx_apt_l3_route_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_l3_route_generate(unit, number_of_calls, (dnx_apt_l3_route_info_t *) * custom_data, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 Route shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_route_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_l3_route_add(unit, &((dnx_apt_l3_route_info_t *) custom_data)[call_iter].route);
}

static shr_error_e
dnx_apt_l3_route_update_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    dnx_apt_l3_route_info_t *l3_route_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        l3_route_info = &((dnx_apt_l3_route_info_t *) custom_data)[iter];

        l3_route_info->route.l3a_flags &= (~BCM_L3_REPLACE);
        l3_route_info->route.l3a_intf = l3_route_info->intf_insert;

        SHR_IF_ERR_EXIT(bcm_l3_route_add(unit, &l3_route_info->route));

        l3_route_info->route.l3a_flags |= BCM_L3_REPLACE;
        l3_route_info->route.l3a_intf = l3_route_info->intf_update;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_route_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    return bcm_l3_route_delete_all(unit, NULL);
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
const dnx_apt_object_def_t dnx_apt_l3_route_ipv4_insert_object = {
    /** Test name */
    "L3_Route_IPv4_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_route_add() when inserting new IPv4 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in KAPS.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_l3_route_ipv4_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv4_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** Test init procedure */
    dnx_apt_l3_route_ipv4_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_route_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_route_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_route_ipv4_update_object = {
    /** Test name */
    "L3_Route_IPv4_Update",
    /** Test description */
    "Measures the performance of bcm_l3_route_add() when updating existing IPv4 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in KAPS.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_l3_route_ipv4_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv4_update_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** Test init procedure */
    dnx_apt_l3_route_ipv4_init,
    /** Run init procedure */
    dnx_apt_l3_route_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_route_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_route_clear_exec,
    /** Test deinit procedure */
    NULL
};

/***********************************************************************************************************************
 * IPv6
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_l3_route_ipv6_insert_object = {
    /** Test name */
    "L3_Route_IPv6_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_route_add() when inserting new IPv6 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in KAPS.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_l3_route_ipv6_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv6_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** Test init procedure */
    dnx_apt_l3_route_ipv6_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_route_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_route_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_route_ipv6_update_object = {
    /** Test name */
    "L3_Route_IPv6_Update",
    /** Test description */
    "Measures the performance of bcm_l3_route_add() when updating existing IPv6 random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in KAPS.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_l3_route_ipv6_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv6_update_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** Test init procedure */
    dnx_apt_l3_route_ipv6_init,
    /** Run init procedure */
    dnx_apt_l3_route_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_route_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_route_clear_exec,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
