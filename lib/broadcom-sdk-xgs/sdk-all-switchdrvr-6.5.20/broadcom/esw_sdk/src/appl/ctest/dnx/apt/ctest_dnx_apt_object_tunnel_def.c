/*
 * ! \file ctest_dnx_apt_object_tunnel_initiator_def.c
 * Contains the functions and definitions for Tunnel initiator create API performance test objects.
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
#include <bcm/tunnel.h>
#include <bcm/l3.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_TUNNEL_INITIATOR_DEFAULT_NUMBER_OF_RUNS     10
#define DNX_APT_TUNNEL_INITIATOR_DEFAULT_NUMBER_OF_CALLS    1000

#define DNX_APT_TUNNEL_INITIATOR_IPV4_RATE                  5400
#define DNX_APT_TUNNEL_INITIATOR_IPV6_RATE                  3800

#define DNX_APT_TUNNEL_INITIATOR_IPV4_RATE_JR2_A0           4600
#define DNX_APT_TUNNEL_INITIATOR_IPV6_RATE_JR2_A0           3400
/* 
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_TUNNEL_INITIATOR_TIME_THRESHOLD(_unit_,  _test_type_) \
        dnxc_data_mgmt_is_jr2_a0(_unit_) ? \
                /** JR2_A0 */ DNX_APT_RATE_TO_TIME(DNX_APT_TUNNEL_INITIATOR_##_test_type_##_RATE_JR2_A0) : \
                /** All other */ DNX_APT_RATE_TO_TIME(DNX_APT_TUNNEL_INITIATOR_##_test_type_##_RATE)

#define DNX_APT_TUNNEL_INITIATOR_OPTIONAL_THRESHOLD_INFO(_info_, _test_type_) \
            sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus (JR2_A0: %.3fus)", \
                         DNX_APT_RATE_TO_TIME(DNX_APT_TUNNEL_INITIATOR_##_test_type_##_RATE), \
                         DNX_APT_RATE_TO_TIME(DNX_APT_TUNNEL_INITIATOR_##_test_type_##_RATE_JR2_A0))

/* 
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the Tunnel initiator create API performance tests */
typedef struct
{
    bcm_tunnel_initiator_t tunnel_initiator;
    bcm_l3_intf_t l3_intf;
} dnx_apt_tunnel_initiator_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/***********************************************************************************************************************
 * Tunnel initiator create time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_tunnel_initiator_create_ipv6_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_INITIATOR_TIME_THRESHOLD(unit, IPV6);
}

static shr_error_e
dnx_apt_tunnel_initiator_create_ipv6_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_INITIATOR_OPTIONAL_THRESHOLD_INFO(info, IPV6);
    return _SHR_E_NONE;
}

static double
dnx_apt_tunnel_initiator_create_ipv4_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_INITIATOR_TIME_THRESHOLD(unit, IPV4);
}

static shr_error_e
dnx_apt_tunnel_initiator_create_ipv4_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_INITIATOR_OPTIONAL_THRESHOLD_INFO(info, IPV4);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel initiator shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_initiator_alloc(
    int unit,
    int number_of_tunnel_initiators,
    dnx_apt_tunnel_initiator_info_t ** tunnel_initiator_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*tunnel_initiator_info, (sizeof(dnx_apt_tunnel_initiator_info_t) * number_of_tunnel_initiators),
              "dnx_apt_tunnel_initiator_info_t", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel initiator test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_initiator_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    int iter;
    bcm_tunnel_initiator_t *tunnel_initiator;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_tunnel_initiator_alloc
                    (unit, number_of_calls, (dnx_apt_tunnel_initiator_info_t **) custom_data));
    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_initiator = &((dnx_apt_tunnel_initiator_info_t *) * custom_data)[iter].tunnel_initiator;
        bcm_tunnel_initiator_t_init(tunnel_initiator);
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel initiator test pre init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_initiator_create_ipv4_pre_init(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_initiator_t *tunnel_initiator;
    int dip, sip;

    sip = 0x01010101;
    dip = 0x01010102;
    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_initiator = &((dnx_apt_tunnel_initiator_info_t *) custom_data)[iter].tunnel_initiator;
        tunnel_initiator->type = bcmTunnelTypeGreAnyIn4;
        tunnel_initiator->dip = dip++;
        tunnel_initiator->sip = sip;
        tunnel_initiator->flags = 0;
    }
    return BCM_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel initiator test pre init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_initiator_create_ipv6_pre_init(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_initiator_t *tunnel_initiator;
    bcm_ip6_t dip6 = { 0x00, 0xd1, 0x00, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    bcm_ip6_t sip6 = { 0x00, 0x51, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_initiator = &((dnx_apt_tunnel_initiator_info_t *) custom_data)[iter].tunnel_initiator;
        tunnel_initiator->type = bcmTunnelTypeGreAnyIn6;
        sal_memcpy(&(tunnel_initiator->sip6), sip6, 16);
        sal_memcpy(&(tunnel_initiator->dip6), dip6, 16);
        dip6[15] = iter & 0xFF;
        dip6[14] = (iter & 0xFF00) >> 8;
        dip6[13] = (iter & 0xFF0000) >> 16;
        dip6[12] = (iter & 0xFF000000) >> 24;
        tunnel_initiator->flags = 0;
    }
    return BCM_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel initiator shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_initiator_create_exec(
    int unit,
    int call_iter,
    void *custom_data)
{

    bcm_l3_intf_t l3_intf;

    SHR_FUNC_INIT_VARS(unit);
    bcm_l3_intf_t_init(&l3_intf);
    SHR_IF_ERR_EXIT(bcm_tunnel_initiator_create
                    (unit, &((dnx_apt_tunnel_initiator_info_t *) custom_data)[call_iter].l3_intf,
                     &((dnx_apt_tunnel_initiator_info_t *) custom_data)[call_iter].tunnel_initiator));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel initiator shared execution functions
 **********************************************************************************************************************/

static shr_error_e
dnx_apt_tunnel_initiator_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_l3_intf_t *l3_intf;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        l3_intf = &((dnx_apt_tunnel_initiator_info_t *) custom_data)[iter].l3_intf;
        SHR_IF_ERR_EXIT(bcm_tunnel_initiator_clear(unit, l3_intf));
    }
exit:
    SHR_FUNC_EXIT;

}

const dnx_apt_object_def_t dnx_apt_tunnel_initiator_create_ipv4_object = {
    /** Test name */
    "Tunnel_initiator_Create_Ipv4",
    /** Test description */
    "Measures the ingress performance of bcm_tunnel_initiator_create() with IPv4. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_INITIATOR_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_INITIATOR_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_tunnel_initiator_create_ipv4_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_initiator_create_ipv4_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_initiator_init,
    /** Run init procedure */
    dnx_apt_tunnel_initiator_create_ipv4_pre_init,
    /** Test execution procedure */
    dnx_apt_tunnel_initiator_create_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_initiator_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_tunnel_initiator_create_ipv6_object = {
    /** Test name */
    "Tunnel_initiator_Create_Ipv6",
    /** Test description */
    "Measures the ingress performance of bcm_tunnel_initiator_create() with IPv6. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_INITIATOR_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_INITIATOR_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_tunnel_initiator_create_ipv6_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_initiator_create_ipv6_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_initiator_init,
    /** Run init procedure */
    dnx_apt_tunnel_initiator_create_ipv6_pre_init,
    /** Test execution procedure */
    dnx_apt_tunnel_initiator_create_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_initiator_clear_exec,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
