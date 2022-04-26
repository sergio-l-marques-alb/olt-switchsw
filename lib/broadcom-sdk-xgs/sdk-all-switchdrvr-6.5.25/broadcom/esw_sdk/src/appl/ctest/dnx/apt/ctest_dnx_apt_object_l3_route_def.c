/*
 * ! \file ctest_dnx_apt_object_l3_route_def.c
 * Contains the functions and definitions for L3 route API performance test objects.
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
#define DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_CALLS    30000

/** KAPS rates */
#define DNX_APT_L3_ROUTE_IPV4_INSERT_KAPS_RATE      56000
#define DNX_APT_L3_ROUTE_IPV4_INSERT_PERF_KAPS_RATE 82000
#define DNX_APT_L3_ROUTE_IPV4_INSERT_DBAL_KAPS_RATE 71000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_KAPS_RATE      102000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_DBAL_KAPS_RATE 146000
#define DNX_APT_L3_ROUTE_IPV4_DELETE_KAPS_RATE      70000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_KAPS_RATE      34000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_PERF_KAPS_RATE 70000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_DBAL_KAPS_RATE 42000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_KAPS_RATE      89000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_DBAL_KAPS_RATE 110000
#define DNX_APT_L3_ROUTE_IPV6_DELETE_KAPS_RATE      45000

#define DNX_APT_L3_ROUTE_IPV4_INSERT_KAPS_RATE_HIGHEST       66000
#define DNX_APT_L3_ROUTE_IPV4_INSERT_PERF_KAPS_RATE_HIGHEST  91000
#define DNX_APT_L3_ROUTE_IPV4_INSERT_DBAL_KAPS_RATE_HIGHEST  81000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_KAPS_RATE_HIGHEST       123000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_DBAL_KAPS_RATE_HIGHEST  163000
#define DNX_APT_L3_ROUTE_IPV4_DELETE_KAPS_RATE_HIGHEST       76000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_KAPS_RATE_HIGHEST       42000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_PERF_KAPS_RATE_HIGHEST  66000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_DBAL_KAPS_RATE_HIGHEST  48000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_KAPS_RATE_HIGHEST       101000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_DBAL_KAPS_RATE_HIGHEST  124000
#define DNX_APT_L3_ROUTE_IPV6_DELETE_KAPS_RATE_HIGHEST       52000

/** KBP rates */
#define DNX_APT_L3_ROUTE_IPV4_INSERT_KBP_RATE       110000
#define DNX_APT_L3_ROUTE_IPV4_INSERT_PERF_KBP_RATE  69000
#define DNX_APT_L3_ROUTE_IPV4_INSERT_DBAL_KBP_RATE  130000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_KBP_RATE       200000
#define DNX_APT_L3_ROUTE_IPV4_UPDATE_DBAL_KBP_RATE  170000
#define DNX_APT_L3_ROUTE_IPV4_DELETE_KBP_RATE       100000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_KBP_RATE       69000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_PERF_KBP_RATE  52000
#define DNX_APT_L3_ROUTE_IPV6_INSERT_DBAL_KBP_RATE  79000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_KBP_RATE       147000
#define DNX_APT_L3_ROUTE_IPV6_UPDATE_DBAL_KBP_RATE  130000
#define DNX_APT_L3_ROUTE_IPV6_DELETE_KBP_RATE       80000

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
dnx_apt_l3_route_ipv4_insert_perf_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV4, INSERT_PERF);
}

static double
dnx_apt_l3_route_ipv4_insert_dbal_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV4, INSERT_DBAL);
}

static double
dnx_apt_l3_route_ipv4_update_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV4, UPDATE);
}

static double
dnx_apt_l3_route_ipv4_update_dbal_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV4, UPDATE_DBAL);
}

static double
dnx_apt_l3_route_ipv4_delete_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV4, DELETE);
}

static double
dnx_apt_l3_route_ipv6_insert_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV6, INSERT);
}

static double
dnx_apt_l3_route_ipv6_insert_perf_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV6, INSERT_PERF);
}

static double
dnx_apt_l3_route_ipv6_insert_dbal_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV6, INSERT_DBAL);
}

static double
dnx_apt_l3_route_ipv6_update_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV6, UPDATE);
}

static double
dnx_apt_l3_route_ipv6_update_dbal_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV6, UPDATE_DBAL);
}

static double
dnx_apt_l3_route_ipv6_delete_time_threshold(
    int unit)
{
    return DNX_APT_L3_ROUTE_TIME_THRESHOLD(unit, IPV6, DELETE);
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
dnx_apt_l3_route_ipv4_insert_perf_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV4, INSERT_PERF);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_route_ipv4_insert_dbal_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV4, INSERT_DBAL);
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
dnx_apt_l3_route_ipv4_update_dbal_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV4, UPDATE_DBAL);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_route_ipv4_delete_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV4, DELETE);
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
dnx_apt_l3_route_ipv6_insert_perf_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV6, INSERT_PERF);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_route_ipv6_insert_dbal_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV6, INSERT_DBAL);
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

static shr_error_e
dnx_apt_l3_route_ipv6_update_dbal_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV6, UPDATE_DBAL);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_route_ipv6_delete_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ROUTE_OPTIONAL_THRESHOLD_INFO(info, IPV6, DELETE);
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

/**
 * \brief The function generates a list of consecutive entries for IPv4 route performance testing.
 */
void
dnx_apt_l3_route_ipv4_generate_perf(
    int unit,
    int number_of_routes,
    dnx_apt_l3_route_info_t * l3_route_info)
{
    int iter, idx = 0, count = 0;
    uint32 ip_addr[5] = { 0x11010101, 0x02020202, 0x29020202, 0x61020202, 0x8C010101 };
    uint32 masks[5] = { 0xFFFFFFFF, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00, 0xFFFFF000 };
    uint32 vrf = 1;
    int nof_setups = 5;
    for (idx = 0; idx < nof_setups; idx++)
    {
        for (iter = 0; iter < number_of_routes / nof_setups; iter++, count++)
        {
            bcm_l3_route_t_init(&l3_route_info[count].route);
            l3_route_info[count].route.l3a_subnet = ip_addr[idx] + (~(masks[idx]) + 1) * iter;
            l3_route_info[count].route.l3a_ip_mask = masks[idx];
            l3_route_info[count].route.l3a_vrf = vrf;

            l3_route_info[count].intf_insert = 0x2000B000;
            l3_route_info[count].intf_update = 0x2000B001;
            l3_route_info[count].route.l3a_intf = l3_route_info[count].intf_insert;
        }
    }
}

/**
 * \brief The function generates a list of consecutive entries for IPv6 route performance testing.
 */
void
dnx_apt_l3_route_ipv6_generate_perf(
    int unit,
    int number_of_routes,
    dnx_apt_l3_route_info_t * l3_route_info)
{
    uint32 iter, idx = 0, count = 0, set_id;
    bcm_ip6_t masks[5] =
        { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00}
    };
    bcm_ip6_t ip_addr[5] =
        { {0x70, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04},
    {0x70, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x70, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00},
    {0x02, 0x00, 0x01, 0x00, 0x01, 0x70, 0x02, 0x00, 0x01, 0x00, 0x01, 0x70, 0x02, 0x00, 0x00, 0x00},
    {0x00, 0x01, 0x70, 0x02, 0x00, 0x00, 0x00, 0x01, 0x70, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x01, 0x70, 0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0x70, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    };
    uint32 vrf = 1;
    uint32 nof_setups = 5;
    uint32 masks_nof_bytes[5] = { 16, 14, 13, 11, 10 };

    for (set_id = 0; set_id < nof_setups; set_id++)
    {
        for (iter = 0; iter < number_of_routes / nof_setups; iter++, count++)
        {
            bcm_l3_route_t_init(&l3_route_info[count].route);
            sal_memcpy(l3_route_info[count].route.l3a_ip6_net, ip_addr[set_id], sizeof(bcm_ip6_t));
            sal_memcpy(l3_route_info[count].route.l3a_ip6_mask, masks[set_id], sizeof(bcm_ip6_t));

            l3_route_info[count].route.l3a_vrf = vrf;
            l3_route_info[count].route.l3a_flags = BCM_L3_IP6;

            l3_route_info[count].intf_insert = 0x2000B000;
            l3_route_info[count].intf_update = 0x2000B001;
            l3_route_info[count].route.l3a_intf = l3_route_info[count].intf_insert;

            /** Increment IPv6 address */
            for (idx = (masks_nof_bytes[set_id] - 1); idx > 0; idx--)
            {
                if (ip_addr[set_id][idx] == 0xFF)
                {
                    ip_addr[set_id][idx] = 0;
                }
                else
                {
                    ip_addr[set_id][idx]++;
                    break;
                }
            }
        }
    }
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
            sal_memcpy(l3_route_info[iter].route.l3a_ip6_mask, ipv6_mask, sizeof(bcm_ip6_t));
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
dnx_apt_l3_route_ipv4_init_perf(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_l3_route_alloc(unit, number_of_calls, (dnx_apt_l3_route_info_t **) custom_data));
    dnx_apt_l3_route_ipv4_generate_perf(unit, number_of_calls, (dnx_apt_l3_route_info_t *) * custom_data);

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

static shr_error_e
dnx_apt_l3_route_ipv6_perf_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_l3_route_alloc(unit, number_of_calls, (dnx_apt_l3_route_info_t **) custom_data));
    dnx_apt_l3_route_ipv6_generate_perf(unit, number_of_calls, (dnx_apt_l3_route_info_t *) * custom_data);

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
dnx_apt_l3_route_delete_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_l3_route_delete(unit, &((dnx_apt_l3_route_info_t *) custom_data)[call_iter].route);
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

/***********************************************************************************************************************
 * L3 Route DBAL exec functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_route_dbal_add(
    int unit,
    bcm_l3_route_t * l3_route)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(l3_route->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY))
    {
        uint32 result_type = -1;
        if (_SHR_IS_FLAG_SET(l3_route->l3a_flags, BCM_L3_IP6))
        {
            result_type = DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD, &entry_handle_id));
            dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, l3_route->l3a_ip6_net,
                                                 l3_route->l3a_ip6_mask);
        }
        else
        {
            result_type = DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD, &entry_handle_id));
            dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4, l3_route->l3a_subnet,
                                              l3_route->l3a_ip_mask);
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, l3_route->l3a_vrf);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE,
                                     BCM_L3_ITF_VAL_GET(l3_route->l3a_intf));
        SHR_IF_ERR_EXIT(dbal_entry_commit
                        (unit, entry_handle_id,
                         (_SHR_IS_FLAG_SET(l3_route->l3a_flags, BCM_L3_REPLACE) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT)));
    }
    else
    {
        if (_SHR_IS_FLAG_SET(l3_route->l3a_flags, BCM_L3_IP6))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD, &entry_handle_id));
            dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, l3_route->l3a_ip6_net,
                                                 l3_route->l3a_ip6_mask);
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD, &entry_handle_id));
            dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4, l3_route->l3a_subnet,
                                              l3_route->l3a_ip_mask);
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, l3_route->l3a_vrf);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE,
                                     BCM_L3_ITF_VAL_GET(l3_route->l3a_intf));
        SHR_IF_ERR_EXIT(dbal_entry_commit
                        (unit, entry_handle_id,
                         (_SHR_IS_FLAG_SET(l3_route->l3a_flags, BCM_L3_REPLACE) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT)));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_route_dbal_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return dnx_apt_l3_route_dbal_add(unit, &((dnx_apt_l3_route_info_t *) custom_data)[call_iter].route);
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv4_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv4_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
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

const dnx_apt_object_def_t dnx_apt_l3_route_ipv4_insert_perf_object = {
    /** Test name */
    "L3_Route_IPv4_Insert_perf",
    /** Test description */
    "Measures the performance of bcm_l3_route_add() when inserting IPv4 consecutive entries. "
        "The entries will be managed in KAPS.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv4_insert_perf_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv4_insert_perf_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_route_ipv4_init_perf,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_route_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_route_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_route_ipv4_insert_dbal_object = {
    /** Test name */
    "L3_Route_IPv4_Insert_DBAL",
    /** Test description */
    "Measures the performance of a custom DBAL insertion API when inserting new IPv4 random entries. "
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv4_insert_dbal_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv4_insert_dbal_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_route_ipv4_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_route_dbal_add_exec,
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv4_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv4_update_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
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

const dnx_apt_object_def_t dnx_apt_l3_route_ipv4_update_dbal_object = {
    /** Test name */
    "L3_Route_IPv4_Update_DBAL",
    /** Test description */
    "Measures the performance of a custom DBAL insertion API when updating existing IPv4 random entries. "
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv4_update_dbal_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv4_update_dbal_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_route_ipv4_init,
    /** Run init procedure */
    dnx_apt_l3_route_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_route_dbal_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_route_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_route_ipv4_delete_object = {
    /** Test name */
    "L3_Route_IPv4_Delete",
    /** Test description */
    "Measures the performance of bcm_l3_route_delete() when deleting existing IPv4 random entries. "
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv4_delete_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv4_delete_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_route_ipv4_init,
    /** Run init procedure */
    dnx_apt_l3_route_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_route_delete_exec,
    /** Run deinit procedure */
    NULL,
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv6_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv6_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
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

const dnx_apt_object_def_t dnx_apt_l3_route_ipv6_insert_perf_object = {
    /** Test name */
    "L3_Route_IPv6_Insert_perf",
    /** Test description */
    "Measures the performance of bcm_l3_route_add() when inserting new IPv6 consecutive entries. "
        "The entries will be managed in KAPS.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ROUTE_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv6_insert_perf_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv6_insert_perf_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_route_ipv6_perf_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_route_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_route_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_route_ipv6_insert_dbal_object = {
    /** Test name */
    "L3_Route_IPv6_Insert_DBAL",
    /** Test description */
    "Measures the performance of a custom DBAL insertion API when inserting new IPv6 random entries. "
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv6_insert_dbal_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv6_insert_dbal_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_route_ipv6_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_route_dbal_add_exec,
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv6_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv6_update_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
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

const dnx_apt_object_def_t dnx_apt_l3_route_ipv6_update_dbal_object = {
    /** Test name */
    "L3_Route_IPv6_Update_DBAL",
    /** Test description */
    "Measures the performance of a custom DBAL insertion API when updating existing IPv6 random entries. "
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv6_update_dbal_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv6_update_dbal_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_route_ipv6_init,
    /** Run init procedure */
    dnx_apt_l3_route_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_route_dbal_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_route_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_route_ipv6_delete_object = {
    /** Test name */
    "L3_Route_IPv6_Delete",
    /** Test description */
    "Measures the performance of bcm_l3_route_delete() when deleting existing IPv6 random entries. "
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
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_l3_route_ipv6_delete_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_route_ipv6_delete_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_route_ipv6_init,
    /** Run init procedure */
    dnx_apt_l3_route_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_route_delete_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
