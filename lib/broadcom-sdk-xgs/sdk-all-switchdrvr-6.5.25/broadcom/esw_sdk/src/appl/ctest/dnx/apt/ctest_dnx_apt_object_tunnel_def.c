/*
 * ! \file ctest_dnx_apt_object_tunnel_initiator_def.c
 * Contains the functions and definitions for Tunnel initiator create API performance test objects.
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
#include <bcm/tunnel.h>
#include <bcm/l3.h>
#include <bcm/vxlan.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

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

#define DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_RUNS          10
#define DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_CALLS         1000

#define DNX_APT_TUNNEL_TERM_IPV4_P2P_INSERT_RATE            5821
#define DNX_APT_TUNNEL_TERM_IPV4_MP_INSERT_RATE             5757
#define DNX_APT_TUNNEL_TERM_IPV4_TCAM_INSERT_RATE           4426
#define DNX_APT_TUNNEL_TERM_IPV6_P2P_INSERT_RATE            4994
#define DNX_APT_TUNNEL_TERM_IPV6_MP_INSERT_RATE             4487
#define DNX_APT_TUNNEL_TERM_TXSCI_INSERT_RATE               5933

#define DNX_APT_TUNNEL_VXLAN_PORT_ADD_DEFAULT_NUMBER_OF_RUNS  10
#define DNX_APT_TUNNEL_VXLAN_PORT_ADD_DEFAULT_NUMBER_OF_CALLS 1000
#define DNX_APT_TUNNEL_VXLAN_PORT_ADD_RATE                 9431
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

#define DNX_APT_TUNNEL_TERM_TIME_THRESHOLD(_unit_, _test_type_, _op_) \
                        DNX_APT_RATE_TO_TIME(DNX_APT_TUNNEL_TERM_##_test_type_##_##_op_##_RATE)

#define DNX_APT_TUNNEL_TERM_OPTIONAL_THRESHOLD_INFO(_info_, _test_type_, _op_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus \n", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_TUNNEL_TERM_##_test_type_##_##_op_##_RATE))

#define DNX_APT_TUNNEL_VXLAN_PORT_TIME_THRESHOLD(_unit_, _op_) \
                        DNX_APT_RATE_TO_TIME(DNX_APT_TUNNEL_VXLAN_PORT_##_op_##_RATE)

#define DNX_APT_TUNNEL_VXLAN_PORT_OPTIONAL_THRESHOLD_INFO(_info_, _op_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus \n", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_TUNNEL_VXLAN_PORT_##_op_##_RATE))

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

/** Utility structure for Tunnel termintor create API performance tests */
typedef struct
{
    bcm_tunnel_terminator_t tunnel_terminator;
} dnx_apt_tunnel_term_info_t;

/** Utility structure for Tunnel Vxlan Port Add */
typedef struct
{
    bcm_tunnel_terminator_t tunnel_terminator;
    bcm_l3_intf_t l3_intf_init;
    int arp_id;
    int fec_id;
    bcm_vpn_t l2vpn;
    bcm_vxlan_port_t vxlan_port;
} dnx_apt_tunnel_vxlan_port_add_info_t;

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
    return _SHR_E_NONE;
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
    return (_SHR_E_NONE);
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

/***********************************************************************************************************************
 * Tunnel terminator test: IPv4 P2P Insert time threshold
 **********************************************************************************************************************/
static double
dnx_apt_tunnel_term_ipv4_p2p_insert_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_TERM_TIME_THRESHOLD(unit, IPV4_P2P, INSERT);
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv4 P2P Insert time threshold info
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv4_p2p_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_TERM_OPTIONAL_THRESHOLD_INFO(info, IPV4_P2P, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv4 MP Insert time threshold
 **********************************************************************************************************************/
static double
dnx_apt_tunnel_term_ipv4_mp_insert_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_TERM_TIME_THRESHOLD(unit, IPV4_MP, INSERT);
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv4 MP Insert time threshold info
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv4_mp_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_TERM_OPTIONAL_THRESHOLD_INFO(info, IPV4_MP, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv4 TCAM Insert time threshold
 **********************************************************************************************************************/
static double
dnx_apt_tunnel_term_ipv4_tcam_insert_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_TERM_TIME_THRESHOLD(unit, IPV4_TCAM, INSERT);
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv4 TCAM Insert time threshold info
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv4_tcam_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_TERM_OPTIONAL_THRESHOLD_INFO(info, IPV4_TCAM, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv6 P2P Insert time threshold
 **********************************************************************************************************************/
static double
dnx_apt_tunnel_term_ipv6_p2p_insert_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_TERM_TIME_THRESHOLD(unit, IPV6_P2P, INSERT);
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv6 P2P Insert time threshold info
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv6_p2p_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_TERM_OPTIONAL_THRESHOLD_INFO(info, IPV6_P2P, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv6 MP Insert time threshold
 **********************************************************************************************************************/
static double
dnx_apt_tunnel_term_ipv6_mp_insert_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_TERM_TIME_THRESHOLD(unit, IPV6_MP, INSERT);
}

/***********************************************************************************************************************
 * Tunnel terminator test: IPv6 MP Insert time threshold info
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv6_mp_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_TERM_OPTIONAL_THRESHOLD_INFO(info, IPV6_MP, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel terminator test: TXSCI Insert time threshold
 **********************************************************************************************************************/
static double
dnx_apt_tunnel_term_txsci_insert_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_TERM_TIME_THRESHOLD(unit, TXSCI, INSERT);
}

/***********************************************************************************************************************
 * Tunnel terminator test: TXSCI Insert time threshold info
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_txsci_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_TERM_OPTIONAL_THRESHOLD_INFO(info, TXSCI, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel terminator test: TXSCI Insert requires SVTAG is supported
 **********************************************************************************************************************/
shr_error_e
dnx_apt_tunnel_term_custom_support_check_txsci(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_switch.svtag.supported_get(unit))
    {
        LOG_CLI((BSL_META("Test is only for devices that suppport SVTAG.\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }

    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator test shared alloc functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_alloc(
    int unit,
    int number_of_tunnel_terms,
    dnx_apt_tunnel_term_info_t ** tunnel_term_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*tunnel_term_info, (sizeof(dnx_apt_tunnel_term_info_t) * number_of_tunnel_terms),
              "dnx_apt_tunnel_term_info_t", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator test shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_tunnel_term_alloc(unit, number_of_calls, (dnx_apt_tunnel_term_info_t **) custom_data));
    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_term_info_t *) * custom_data)[iter].tunnel_terminator;
        bcm_tunnel_terminator_t_init(tunnel_terminator);
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator test pre exec functions: IPV4 P2P Insert
 * Create tunnel terminator with
 * 1. type = bcmTunnelTypeIpAnyIn4
 * 2. dip_mask = DNX_TUNNEL_TERM_EM_IP_MASK(0xFFFFFFFF)
 * 3. sip_mask = DNX_TUNNEL_TERM_EM_IP_MASK(0xFFFFFFFF)
 * DBAL table accessed during performance test
 * 1. DBAL_TABLE_IPV4_TT_P2P_EM_BASIC
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv4_p2p_insert_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;

    SHR_FUNC_INIT_VARS(unit);

    /** Generate array of unique random number */
    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, dnx_data_l3.vrf.nof_vrf_get(unit)));

    if (number_of_calls >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        LOG_CLI((BSL_META("Number of runs(%d) exceeds number of random VRFs(%d)\n"), number_of_calls,
                 dnx_data_l3.vrf.nof_vrf_get(unit)));
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_term_info_t *) custom_data)[iter].tunnel_terminator;
        tunnel_terminator->type = bcmTunnelTypeIpAnyIn4;
        tunnel_terminator->dip = dnx_apt_random_get() & 0xFFFFFFFF;
        tunnel_terminator->dip_mask = 0xFFFFFFFF;
        tunnel_terminator->sip = dnx_apt_random_get() & 0xFFFFFFFF;
        tunnel_terminator->sip_mask = 0xFFFFFFFF;
        tunnel_terminator->vrf = dnx_apt_unique_random_get(iter);
        tunnel_terminator->flags = 0;
    }

    /** Free array of unique random number */
    dnx_apt_unique_random_free();

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator test pre exec functions: IPV4 MP Insert
 * Create tunnel terminator with
 * 1. type = bcmTunnelTypeIpAnyIn4
 * 2. dip_mask = DNX_TUNNEL_TERM_EM_IP_MASK(0xFFFFFFFF)
 * 3. sip_mask = 0
 * DBAL table accessed during performance test
 * 1. DBAL_TABLE_IPV4_TT_MP_EM_BASIC
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv4_mp_insert_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;

    SHR_FUNC_INIT_VARS(unit);

    /** Generate array of unique random number */
    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, dnx_data_l3.vrf.nof_vrf_get(unit)));

    if (number_of_calls >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        LOG_CLI((BSL_META("Number of runs(%d) exceeds number of random VRFs(%d)\n"), number_of_calls,
                 dnx_data_l3.vrf.nof_vrf_get(unit)));
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_term_info_t *) custom_data)[iter].tunnel_terminator;
        tunnel_terminator->type = bcmTunnelTypeIpAnyIn4;
        tunnel_terminator->dip = dnx_apt_random_get() & 0xFFFFFFFF;
        tunnel_terminator->dip_mask = 0xFFFFFFFF;
        tunnel_terminator->sip = dnx_apt_random_get() & 0xFFFFFFFF;
        tunnel_terminator->sip_mask = 0;
        tunnel_terminator->vrf = dnx_apt_unique_random_get(iter);
        tunnel_terminator->flags = 0;
    }

    /** Free array of unique random number */
    dnx_apt_unique_random_free();

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator test pre exec functions: IPV4 TCAM Insert
 * Create tunnel terminator with
 * 1. type = bcmTunnelTypeIpAnyIn4
 * 2. dip_mask = 0
 * 3. sip_mask = 0
 * DBAL table accessed during performance test
 * 1. DBAL_TABLE_IPV4_TT_TCAM_BASIC
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv4_tcam_insert_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;

    SHR_FUNC_INIT_VARS(unit);

    /** Generate array of unique random number */
    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, dnx_data_l3.vrf.nof_vrf_get(unit)));

    if (number_of_calls >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        LOG_CLI((BSL_META("Number of runs(%d) exceeds number of random VRFs(%d)\n"), number_of_calls,
                 dnx_data_l3.vrf.nof_vrf_get(unit)));
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_term_info_t *) custom_data)[iter].tunnel_terminator;
        tunnel_terminator->type = bcmTunnelTypeIpAnyIn4;
        tunnel_terminator->dip = dnx_apt_random_get() & 0xFFFFFFFF;
        tunnel_terminator->dip_mask = 0xFFFFFF00;
        tunnel_terminator->sip = dnx_apt_random_get() & 0xFFFFFFFF;
        tunnel_terminator->sip_mask = 0;
        tunnel_terminator->vrf = dnx_apt_unique_random_get(iter);
        tunnel_terminator->flags = 0;
    }
    /** Free array of unique random number */
    dnx_apt_unique_random_free();
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator test pre exec functions: IPV6 P2P Insert
 * Create tunnel terminator with
 * 1. type = bcmTunnelTypeIpAnyIn6
 * 2. flags = BCM_TUNNEL_TERM_USE_TUNNEL_CLASS
 * DBAL table accessed during performance test
 * 1. DBAL_TABLE_IPV6_TT_P2P_EM_BASIC
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv6_p2p_insert_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;
    int ipv6_addr_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Generate array of unique random number */
    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, dnx_data_l3.vrf.nof_vrf_get(unit)));

    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_term_info_t *) custom_data)[iter].tunnel_terminator;
        tunnel_terminator->type = bcmTunnelTypeIpAnyIn6;
        for (ipv6_addr_index = 0; ipv6_addr_index < 16; ipv6_addr_index++)
        {
            tunnel_terminator->sip6[ipv6_addr_index] = dnx_apt_random_get() & 0xFF;
            tunnel_terminator->sip6_mask[ipv6_addr_index] = 0xFF;
            tunnel_terminator->dip6[ipv6_addr_index] = dnx_apt_random_get() & 0xFF;
            tunnel_terminator->dip6_mask[ipv6_addr_index] = 0xFF;
        }
        tunnel_terminator->vrf = dnx_apt_unique_random_get(iter);
        tunnel_terminator->flags = BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    }
    /** Free array of unique random number */
    dnx_apt_unique_random_free();
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator test pre exec functions: IPV6 MP Insert
 * Create tunnel terminator with
 * 1. type = bcmTunnelTypeIpAnyIn6
 * 2. flags = BCM_TUNNEL_TERM_USE_TUNNEL_CLASS
 * DBAL table accessed during performance test
 * 1. DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_ipv6_mp_insert_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;
    int ipv6_addr_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Generate array of unique random number */
    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, dnx_data_l3.vrf.nof_vrf_get(unit)));

    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_term_info_t *) custom_data)[iter].tunnel_terminator;
        tunnel_terminator->type = bcmTunnelTypeIpAnyIn6;
        for (ipv6_addr_index = 0; ipv6_addr_index < 16; ipv6_addr_index++)
        {
            tunnel_terminator->sip6[ipv6_addr_index] = dnx_apt_random_get() & 0xFF;
            tunnel_terminator->sip6_mask[ipv6_addr_index] = 0xFF;
            tunnel_terminator->dip6[ipv6_addr_index] = dnx_apt_random_get() & 0xFF;
            tunnel_terminator->dip6_mask[ipv6_addr_index] = 0xFF;
        }
        tunnel_terminator->vrf = dnx_apt_unique_random_get(iter);
        tunnel_terminator->flags = 0;
    }

    /** Free array of unique random number */
    dnx_apt_unique_random_free();
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator test pre exec functions: TXSCI Insert
 * Create tunnel terminator with
 * 1. type = bcmTunnelTypeEsp
 * 2. flags = BCM_TUNNEL_TERM_USE_TUNNEL_CLASS
 * DBAL table accessed during performance test
 * 1. DBAL_TABLE_IPVX_TT_IPSEC
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_txsci_insert_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    int nof_txsci_values;
    bcm_tunnel_terminator_t *tunnel_terminator;

    SHR_FUNC_INIT_VARS(unit);

    nof_txsci_values = (1 << dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit));

    /** Generate array of unique random number */
    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, nof_txsci_values));

    if (number_of_calls >= nof_txsci_values)
    {
        LOG_CLI((BSL_META("Number of runs(%d) exceeds number of TXSCI value(%d)\n"), number_of_calls,
                 nof_txsci_values));
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_term_info_t *) custom_data)[iter].tunnel_terminator;
        tunnel_terminator->type = bcmTunnelTypeEsp;
        tunnel_terminator->sci = dnx_apt_unique_random_get(iter);
        tunnel_terminator->flags = 0;
    }
    /** Free array of unique random number */
    dnx_apt_unique_random_free();
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator insert execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_term_insert_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_tunnel_terminator_create
                    (unit, &((dnx_apt_tunnel_term_info_t *) custom_data)[call_iter].tunnel_terminator));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator shared clear functions
 **********************************************************************************************************************/

static shr_error_e
dnx_apt_tunnel_term_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_term_info_t *) custom_data)[iter].tunnel_terminator;
        SHR_IF_ERR_EXIT(bcm_tunnel_terminator_delete(unit, tunnel_terminator));
    }
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel Vxlan test: Create time threshold
 **********************************************************************************************************************/
static double
dnx_apt_tunnel_vxlan_port_add_time_threshold(
    int unit)
{
    return DNX_APT_TUNNEL_VXLAN_PORT_TIME_THRESHOLD(unit, ADD);
}

/***********************************************************************************************************************
 * Tunnel Vxlan test: Create time threshold info
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_vxlan_port_add_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_TUNNEL_VXLAN_PORT_OPTIONAL_THRESHOLD_INFO(info, ADD);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Tunnel Vxlan Port Add shared alloc functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_vxlan_port_add_alloc(
    int unit,
    int number_of_tunnel_terms,
    dnx_apt_tunnel_vxlan_port_add_info_t ** tunnel_term_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*tunnel_term_info, (sizeof(dnx_apt_tunnel_vxlan_port_add_info_t) * number_of_tunnel_terms),
              "dnx_apt_tunnel_vxlan_port_add_info_t", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel Vxlan Port Add test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_vxlan_port_add_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;
    bcm_vxlan_port_t *vxlan_port;
    bcm_l3_intf_t *l3_intf_init;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_tunnel_vxlan_port_add_alloc
                    (unit, number_of_calls, (dnx_apt_tunnel_vxlan_port_add_info_t **) custom_data));
    for (iter = 0; iter < number_of_calls; iter++)
    {
        tunnel_terminator = &((dnx_apt_tunnel_vxlan_port_add_info_t *) * custom_data)[iter].tunnel_terminator;
        bcm_tunnel_terminator_t_init(tunnel_terminator);

        l3_intf_init = &((dnx_apt_tunnel_vxlan_port_add_info_t *) * custom_data)[iter].l3_intf_init;
        bcm_l3_intf_t_init(l3_intf_init);

        vxlan_port = &((dnx_apt_tunnel_vxlan_port_add_info_t *) * custom_data)[iter].vxlan_port;
        bcm_vxlan_port_t_init(vxlan_port);

        ((dnx_apt_tunnel_vxlan_port_add_info_t *) * custom_data)[iter].l2vpn = 15;
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel Vxlan Port Add test pre exec configurations, including:
 * 1. VPN
 * 2. ARP
 * 3. VXLAN initiator
 * 4. FEC
 * 5. VXLAN terminator
 * 6. Initialize Vxlan Port
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_vxlan_port_add_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    int random_mac_address;
    bcm_mac_t arp_mac_add = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
    int vlan = 500;
    int port = 200;
    int gport;
    bcm_l3_egress_t l3eg_arp;
    bcm_l3_egress_t l3eg_fec;
    bcm_if_t l3egid_null;
    bcm_if_t fec_id;
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_tunnel_terminator_t *tunnel_terminator;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    bcm_l3_intf_t *l3_intf_init;
    bcm_vxlan_port_t *vxlan_port;
    int match_port = 201;
    int vpn = 15;
    int vnid = 5000;
    bcm_if_t egress_if;

    SHR_FUNC_INIT_VARS(unit);

    /** 1. create VPN */
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.broadcast_group = vpn;
    vpn_config.vpn = vpn;
    vpn_config.vnid = vnid;
    vpn_config.unknown_unicast_group = vpn;
    vpn_config.unknown_multicast_group = vpn;
    SHR_IF_ERR_EXIT(bcm_vxlan_vpn_create(unit, &vpn_config));

    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, dnx_data_l3.vrf.nof_vrf_get(unit)));

    if (number_of_calls >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        LOG_CLI((BSL_META("Number of runs(%d) exceeds number of random VRFs(%d)\n"), number_of_calls,
                 dnx_data_l3.vrf.nof_vrf_get(unit)));
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    for (iter = 0; iter < number_of_calls; iter++)
    {
        /** 2. Create ARP entry */
        bcm_l3_egress_t_init(&l3eg_arp);
        l3eg_arp.flags = 0;
        l3eg_arp.flags2 = 0;
        random_mac_address = dnx_apt_unique_random_get(iter);
        arp_mac_add[3] = (random_mac_address & 0xff0000) >> 16;
        arp_mac_add[4] = (random_mac_address & 0xff00) >> 8;
        arp_mac_add[5] = (random_mac_address & 0xff);
        sal_memcpy(l3eg_arp.mac_addr, arp_mac_add, 6);
        l3eg_arp.vlan = vlan;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg_arp, &l3egid_null));
        /** Record ARP id */
        ((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].arp_id = l3eg_arp.encap_id;

        /** 3. Create VXLAN initiator */
        bcm_l3_intf_t_init(&l3_intf);
        bcm_tunnel_initiator_t_init(&tunnel_init);
        tunnel_init.type = bcmTunnelTypeVxlanGpe;
        tunnel_init.dip = 0xAB000002;
        tunnel_init.sip = 0xAC000002;
        tunnel_init.ttl = 64;
        tunnel_init.l3_intf_id = l3eg_arp.encap_id;
        tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        SHR_IF_ERR_EXIT(bcm_tunnel_initiator_create(unit, &l3_intf, &tunnel_init));
        /** Record VXLAN initiator id */
        l3_intf_init = &((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].l3_intf_init;
        l3_intf_init->l3a_intf_id = l3_intf.l3a_intf_id;

        /** 4. Create FEC entry */
        BCM_GPORT_LOCAL_SET(gport, port);
        bcm_l3_egress_t_init(&l3eg_fec);
        l3eg_fec.intf = tunnel_init.tunnel_id;
        l3eg_fec.encap_id = 0;
        l3eg_fec.port = gport;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg_fec, &fec_id));
        /** Record FEC id */
        ((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].fec_id = fec_id;

        /** 5. Create VXLAN terminator */
        tunnel_terminator = &((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].tunnel_terminator;
        bcm_tunnel_terminator_t_init(tunnel_terminator);
        tunnel_terminator->type = bcmTunnelTypeVxlanGpe;
        tunnel_terminator->dip = dnx_apt_random_get() & 0xFFFFFFFF;
        tunnel_terminator->dip_mask = 0xFFFFFFFF;
        tunnel_terminator->sip = dnx_apt_random_get() & 0xFFFFFFFF;
        tunnel_terminator->sip_mask = 0xFFFFFFFF;
        tunnel_terminator->vrf = dnx_apt_unique_random_get(iter);
        tunnel_terminator->flags = 0;
        SHR_IF_ERR_EXIT(bcm_tunnel_terminator_create(unit, tunnel_terminator));

        /** 6. Initialize Vxlan Port */
        vxlan_port = &((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].vxlan_port;

        BCM_L3_ITF_SET(egress_if, BCM_L3_ITF_TYPE_FEC, fec_id);
        bcm_vxlan_port_t_init(vxlan_port);
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
        vxlan_port->match_port = match_port;
        vxlan_port->match_tunnel_id = tunnel_terminator->tunnel_id;
        vxlan_port->flags = BCM_VXLAN_PORT_EGRESS_TUNNEL;
        vxlan_port->egress_if = egress_if;
        vxlan_port->default_vpn = 1023;
    }

    /** Free array of unique random number */
    dnx_apt_unique_random_free();

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator insert execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_tunnel_vxlan_port_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    bcm_vxlan_port_t *vxlan_port;
    bcm_vpn_t l2vpn;
    SHR_FUNC_INIT_VARS(unit);

    l2vpn = ((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[call_iter].l2vpn;
    vxlan_port = &((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[call_iter].vxlan_port;

    SHR_IF_ERR_EXIT(bcm_vxlan_port_add(unit, l2vpn, vxlan_port));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Tunnel terminator shared clear functions
 **********************************************************************************************************************/

static shr_error_e
dnx_apt_tunnel_vxlan_port_add_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_tunnel_terminator_t *tunnel_terminator;
    bcm_l3_intf_t *l3_intf_init;
    bcm_vxlan_port_t *vxlan_port;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        vxlan_port = &((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].vxlan_port;
        /** Delete gport forward information from gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, vxlan_port->vxlan_port_id));
        /** Delete Tunnel Vxlan terminator */
        tunnel_terminator = &((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].tunnel_terminator;
        SHR_IF_ERR_EXIT(bcm_tunnel_terminator_delete(unit, tunnel_terminator));
        /** Clear FEC */
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy
                        (unit, ((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].fec_id));
        /** Clear Tunnel Vxlan initiator */
        l3_intf_init = &((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].l3_intf_init;
        SHR_IF_ERR_EXIT(bcm_tunnel_initiator_clear(unit, l3_intf_init));
        /** Clear ARP */
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy
                        (unit, ((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].arp_id));
    }

    iter = 0;
    /** Delete VPN */
    SHR_IF_ERR_EXIT(bcm_vxlan_vpn_destroy(unit, ((dnx_apt_tunnel_vxlan_port_add_info_t *) custom_data)[iter].l2vpn));
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
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_initiator_create_ipv4_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_initiator_create_ipv4_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
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
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_initiator_create_ipv6_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_initiator_create_ipv6_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
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

const dnx_apt_object_def_t dnx_apt_tunnel_term_ipv4_p2p_insert_object = {
    /** Test name */
    "Tunnel_Term_IPv4_P2P_Insert",
    /** Test description */
    "Measures the performance of bcm_tunnel_terminator_create(): Insert DBAL_TABLE_IPV4_TT_P2P_EM_BASIC.",
    /** Support flags */
    DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_term_ipv4_p2p_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_term_ipv4_p2p_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_term_init,
    /** Run init procedure */
    dnx_apt_tunnel_term_ipv4_p2p_insert_pre_exec,
    /** Test execution procedure */
    dnx_apt_tunnel_term_insert_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_term_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_tunnel_term_ipv4_mp_insert_object = {
    /** Test name */
    "Tunnel_Term_IPv4_MP_Insert",
    /** Test description */
    "Measures the performance of bcm_tunnel_terminator_create(): Insert DBAL_TABLE_IPV4_TT_MP_EM_BASIC.",
    /** Support flags */
    DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_term_ipv4_mp_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_term_ipv4_mp_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_term_init,
    /** Run init procedure */
    dnx_apt_tunnel_term_ipv4_mp_insert_pre_exec,
    /** Test execution procedure */
    dnx_apt_tunnel_term_insert_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_term_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_tunnel_term_ipv4_tcam_insert_object = {
    /** Test name */
    "Tunnel_Term_IPv4_TCAM_Insert",
    /** Test description */
    "Measures the performance of bcm_tunnel_terminator_create(): Insert DBAL_TABLE_IPV4_TT_TCAM_BASIC.",
    /** Support flags */
    DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_term_ipv4_tcam_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_term_ipv4_tcam_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_term_init,
    /** Run init procedure */
    dnx_apt_tunnel_term_ipv4_tcam_insert_pre_exec,
    /** Test execution procedure */
    dnx_apt_tunnel_term_insert_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_term_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_tunnel_term_ipv6_p2p_insert_object = {
    /** Test name */
    "Tunnel_Term_IPv6_P2P_Insert",
    /** Test description */
    "Measures the performance of bcm_tunnel_terminator_create(): Insert DBAL_TABLE_IPV6_TT_P2P_EM_BASIC.",
    /** Support flags */
    DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_term_ipv6_p2p_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_term_ipv6_p2p_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_term_init,
    /** Run init procedure */
    dnx_apt_tunnel_term_ipv6_p2p_insert_pre_exec,
    /** Test execution procedure */
    dnx_apt_tunnel_term_insert_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_term_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_tunnel_term_ipv6_mp_insert_object = {
    /** Test name */
    "Tunnel_Term_IPv6_MP_Insert",
    /** Test description */
    "Measures the performance of bcm_tunnel_terminator_create(): Insert DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC.",
    /** Support flags */
    DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_term_ipv6_mp_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_term_ipv6_mp_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_term_init,
    /** Run init procedure */
    dnx_apt_tunnel_term_ipv6_mp_insert_pre_exec,
    /** Test execution procedure */
    dnx_apt_tunnel_term_insert_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_term_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_tunnel_term_txsci_insert_object = {
    /** Test name */
    "Tunnel_Term_TXSCI_Insert",
    /** Test description */
    "Measures the performance of bcm_tunnel_terminator_create(): Insert DBAL_TABLE_IPVX_TT_IPSEC.",
    /** Support flags */
    DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_TERM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_term_txsci_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_term_txsci_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_tunnel_term_custom_support_check_txsci,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_term_init,
    /** Run init procedure */
    dnx_apt_tunnel_term_txsci_insert_pre_exec,
    /** Test execution procedure */
    dnx_apt_tunnel_term_insert_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_term_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_tunnel_vxlan_port_add_object = {
    /** Test name */
    "Tunnel_Vxlan_Port_Add",
    /** Test description */
    "Measures the performance of bcm_vxlan_port_add(): Update inlif/outlif.",
    /** Support flags */
    DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_TUNNEL_VXLAN_PORT_ADD_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_TUNNEL_VXLAN_PORT_ADD_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_tunnel_vxlan_port_add_time_threshold,
    /** Optional threshold info */
    dnx_apt_tunnel_vxlan_port_add_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_tunnel_vxlan_port_add_init,
    /** Run init procedure */
    dnx_apt_tunnel_vxlan_port_add_pre_exec,
    /** Test execution procedure */
    dnx_apt_tunnel_vxlan_port_add_exec,
    /** Run deinit procedure */
    dnx_apt_tunnel_vxlan_port_add_clear_exec,
    /** Test deinit procedure */
    NULL
};
/*
 * }
 */
