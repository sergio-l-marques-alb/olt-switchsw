/*
 * ! \file ctest_dnx_apt_object_ipmc_def.c
 * Contains the functions and definitions for IPMC API performance test objects.
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
#include <bcm/ipmc.h>
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

#define DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS         10
#define DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS        10000
#define DNX_APT_IPMC_TCAM_DEFAULT_NUMBER_OF_CALLS   5000

/** KAPS rate */
#define DNX_APT_IPMC_IPV4_LPM_INSERT_RATE       49000
#define DNX_APT_IPMC_IPV4_LPM_UPDATE_RATE       102000
#define DNX_APT_IPMC_IPV6_LPM_INSERT_RATE       37000
#define DNX_APT_IPMC_IPV6_LPM_UPDATE_RATE       91000

#define DNX_APT_IPMC_IPV4_LPM_INSERT_RATE_HIGHEST   58000
#define DNX_APT_IPMC_IPV4_LPM_UPDATE_RATE_HIGHEST   112000
#define DNX_APT_IPMC_IPV6_LPM_INSERT_RATE_HIGHEST   43000
#define DNX_APT_IPMC_IPV6_LPM_UPDATE_RATE_HIGHEST   102000

/** EM rate */
#define DNX_APT_IPMC_IPV4_EM_INSERT_RATE        68000
#define DNX_APT_IPMC_IPV4_EM_UPDATE_RATE        63000
#define DNX_APT_IPMC_IPV6_EM_INSERT_RATE        63000
#define DNX_APT_IPMC_IPV6_EM_UPDATE_RATE        57000

#define DNX_APT_IPMC_IPV4_EM_INSERT_RATE_JR2_A0     38000
#define DNX_APT_IPMC_IPV4_EM_UPDATE_RATE_JR2_A0     37000
#define DNX_APT_IPMC_IPV6_EM_INSERT_RATE_JR2_A0     37000
#define DNX_APT_IPMC_IPV6_EM_UPDATE_RATE_JR2_A0     35000

/** TCAM rate */
#define DNX_APT_IPMC_IPV4_TCAM_INSERT_RATE      25000
#define DNX_APT_IPMC_IPV4_TCAM_UPDATE_RATE      23000

#define DNX_APT_IPMC_IPV4_TCAM_INSERT_RATE_JR2      16000
#define DNX_APT_IPMC_IPV4_TCAM_UPDATE_RATE_JR2      18000

#define DNX_APT_IPMC_IPV4_TCAM_INSERT_RATE_J2P      16000
#define DNX_APT_IPMC_IPV4_TCAM_UPDATE_RATE_J2P      19500

/** KBP rate */
#define DNX_APT_IPMC_IPV4_TCAM_INSERT_KBP_RATE  75000
#define DNX_APT_IPMC_IPV4_TCAM_UPDATE_KBP_RATE  115000
#define DNX_APT_IPMC_IPV6_TCAM_INSERT_KBP_RATE  55000
#define DNX_APT_IPMC_IPV6_TCAM_UPDATE_KBP_RATE  90000

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */

/** LPM */
#define DNX_APT_IPMC_LPM_OPTIONAL_THRESHOLD_INFO(_info_, _ip_, _test_type_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "KAPS: %.3fus (varies to %.3fus)", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_LPM_##_test_type_##_RATE), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_LPM_##_test_type_##_RATE_HIGHEST)) \

/** EM */
#define DNX_APT_IPMC_EM_TIME_THRESHOLD(_unit_, _ip_, _test_type_) \
    dnxc_data_mgmt_is_jr2_a0(_unit_) ? \
            /** JR2_A0 */ DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_EM_##_test_type_##_RATE_JR2_A0) : \
            /** All other */ DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_EM_##_test_type_##_RATE)

#define DNX_APT_IPMC_EM_OPTIONAL_THRESHOLD_INFO(_info_, _ip_, _test_type_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "EM: %.3fus (JR2_A0: %.3fus)", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_EM_##_test_type_##_RATE), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_EM_##_test_type_##_RATE_JR2_A0))

/** TCAM */
#define DNX_APT_IPMC_TCAM_TIME_THRESHOLD(_unit_, _ip_, _test_type_) \
    dnx_kbp_device_enabled(_unit_) ? \
            /** KBP */ DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_TCAM_##_test_type_##_KBP_RATE) : \
            /** TCAM */ (dnxc_data_mgmt_is_jr2_a0(_unit_) || \
                         dnxc_data_mgmt_is_jr2_b0(_unit_) || dnxc_data_mgmt_is_jr2_b1(_unit_)) ? \
                        /** JR2 */ DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_TCAM_##_test_type_##_RATE_JR2) : \
                        /** J2P */ (dnxc_data_mgmt_is_j2p_a0(_unit_) ? DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_TCAM_##_test_type_##_RATE_J2P) :\
                        /** All other */ DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_TCAM_##_test_type_##_RATE))

#define DNX_APT_IPMC_TCAM_OPTIONAL_THRESHOLD_INFO(_info_, _ip_, _test_type_) \
    sal_snprintf(info, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "TCAM: %.3fus (JR2: %.3fus)\nKBP: %.3fus", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_TCAM_##_test_type_##_RATE), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_TCAM_##_test_type_##_RATE_JR2), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_##_ip_##_TCAM_##_test_type_##_KBP_RATE))

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the IPMC API performance tests */
typedef struct
{
    bcm_ipmc_addr_t ipmc;
    uint32 intf_insert;
    uint32 intf_update;
} dnx_apt_ipmc_info_t;

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
 * IPMC time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_ipmc_ipv4_lpm_insert_time_threshold(
    int unit)
{
    return DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_IPV4_LPM_INSERT_RATE);
}
static double
dnx_apt_ipmc_ipv4_lpm_update_time_threshold(
    int unit)
{
    return DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_IPV4_LPM_UPDATE_RATE);
}
static double
dnx_apt_ipmc_ipv6_lpm_insert_time_threshold(
    int unit)
{
    return DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_IPV6_LPM_INSERT_RATE);
}
static double
dnx_apt_ipmc_ipv6_lpm_update_time_threshold(
    int unit)
{
    return DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_IPV6_LPM_UPDATE_RATE);
}
/**********************************************************************************************************************/
static double
dnx_apt_ipmc_ipv4_em_insert_time_threshold(
    int unit)
{
    return DNX_APT_IPMC_EM_TIME_THRESHOLD(unit, IPV4, INSERT);
}
static double
dnx_apt_ipmc_ipv4_em_update_time_threshold(
    int unit)
{
    return DNX_APT_IPMC_EM_TIME_THRESHOLD(unit, IPV4, UPDATE);
}
static double
dnx_apt_ipmc_ipv6_em_insert_time_threshold(
    int unit)
{
    return DNX_APT_IPMC_EM_TIME_THRESHOLD(unit, IPV6, INSERT);
}
static double
dnx_apt_ipmc_ipv6_em_update_time_threshold(
    int unit)
{
    return DNX_APT_IPMC_EM_TIME_THRESHOLD(unit, IPV6, UPDATE);
}
/**********************************************************************************************************************/
static double
dnx_apt_ipmc_ipv4_tcam_insert_time_threshold(
    int unit)
{
    return DNX_APT_IPMC_TCAM_TIME_THRESHOLD(unit, IPV4, INSERT);
}
static double
dnx_apt_ipmc_ipv4_tcam_update_time_threshold(
    int unit)
{
    return DNX_APT_IPMC_TCAM_TIME_THRESHOLD(unit, IPV4, UPDATE);
}
static double
dnx_apt_ipmc_ipv6_tcam_insert_time_threshold(
    int unit)
{
    return DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_IPV6_TCAM_INSERT_KBP_RATE);
}
static double
dnx_apt_ipmc_ipv6_tcam_update_time_threshold(
    int unit)
{
    return DNX_APT_RATE_TO_TIME(DNX_APT_IPMC_IPV6_TCAM_UPDATE_KBP_RATE);
}

/***********************************************************************************************************************
 * IPMC optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_ipmc_ipv4_lpm_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_LPM_OPTIONAL_THRESHOLD_INFO(info, IPV4, INSERT);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_ipmc_ipv4_lpm_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_LPM_OPTIONAL_THRESHOLD_INFO(info, IPV4, UPDATE);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_ipmc_ipv6_lpm_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_LPM_OPTIONAL_THRESHOLD_INFO(info, IPV6, INSERT);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_ipmc_ipv6_lpm_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_LPM_OPTIONAL_THRESHOLD_INFO(info, IPV6, UPDATE);
    return _SHR_E_NONE;
}
/**********************************************************************************************************************/
static shr_error_e
dnx_apt_ipmc_ipv4_em_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_EM_OPTIONAL_THRESHOLD_INFO(info, IPV4, INSERT);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_ipmc_ipv4_em_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_EM_OPTIONAL_THRESHOLD_INFO(info, IPV4, UPDATE);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_ipmc_ipv6_em_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_EM_OPTIONAL_THRESHOLD_INFO(info, IPV6, INSERT);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_ipmc_ipv6_em_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_EM_OPTIONAL_THRESHOLD_INFO(info, IPV6, UPDATE);
    return _SHR_E_NONE;
}
/**********************************************************************************************************************/
static shr_error_e
dnx_apt_ipmc_ipv4_tcam_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_TCAM_OPTIONAL_THRESHOLD_INFO(info, IPV4, INSERT);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_ipmc_ipv4_tcam_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_IPMC_TCAM_OPTIONAL_THRESHOLD_INFO(info, IPV4, UPDATE);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * IPMC shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_ipmc_alloc(
    int unit,
    int number_of_routes,
    dnx_apt_ipmc_info_t ** ipmc_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*ipmc_info, (sizeof(dnx_apt_ipmc_info_t) * number_of_routes), "dnx_apt_ipmc_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_ipmc_generate(
    int unit,
    int number_of_ipmcs,
    dnx_apt_ipmc_info_t * ipmc_info,
    uint8 is_ipv6,
    uint8 is_em,
    uint8 is_tcam)
{
    int iter = 0;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!is_ipv6)
    {
        SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, 16 * 1024 * 1024));
    }
    if (is_tcam)
    {
        flags |= BCM_IPMC_TCAM;
    }

    dnx_apt_random_seed(0);
    for (iter = 0; iter < number_of_ipmcs; iter++)
    {
        bcm_ipmc_addr_t_init(&ipmc_info[iter].ipmc);

        /** Randomized ipmc entries */
        ipmc_info[iter].ipmc.flags = flags;

        if (is_ipv6)
        {
            int ipv6_addr_index = 16;

            ipmc_info[iter].ipmc.flags |= BCM_IPMC_IP6;
            while (--ipv6_addr_index)
            {
                ipmc_info[iter].ipmc.mc_ip6_addr[ipv6_addr_index] = dnx_apt_random_get() & 0xFF;
                ipmc_info[iter].ipmc.mc_ip6_mask[ipv6_addr_index] = 0xFF;
            }
            ipmc_info[iter].ipmc.mc_ip6_addr[0] = 0xFF;
        }
        else
        {
            ipmc_info[iter].ipmc.mc_ip_addr = (dnx_apt_unique_random_get(iter) & 0x0FFFFFFF) | 0xE0000000;
            ipmc_info[iter].ipmc.mc_ip_mask = 0xFFFFFFFF;
            ipmc_info[iter].ipmc.s_ip_addr = is_em ? 0x00000000 : 0xFFFFFFFF;
            ipmc_info[iter].ipmc.s_ip_mask = is_em ? 0x00000000 : 0xFFFFFFFF;
        }
        if (is_tcam)
        {
            ipmc_info[iter].ipmc.priority = 0;
        }

        ipmc_info[iter].ipmc.vrf = (dnx_apt_random_get() & 0xFE) + 1;

        if (is_em)
        {
            BCM_L3_ITF_SET(ipmc_info[iter].ipmc.ing_intf, BCM_L3_ITF_TYPE_LIF, ((dnx_apt_random_get() & 0x3FFE) + 1));
        }
        else
        {
            /** Empty random call in order to sync EM and non-EM l3a_intf values */
            dnx_apt_random_get();
        }
        ipmc_info[iter].intf_insert = (dnx_apt_random_get() & 0x3FFE) + 1;
        ipmc_info[iter].intf_update = (dnx_apt_random_get() & 0x3FFE) + 1;
        ipmc_info[iter].ipmc.l3a_intf = ipmc_info[iter].intf_insert;
    }

    SHR_IF_ERR_EXIT(dnx_apt_kbp_sync(unit));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * IPMC test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_ipmc_ipv4_lpm_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_ipmc_alloc(unit, number_of_calls, (dnx_apt_ipmc_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_ipmc_generate
                    (unit, number_of_calls, (dnx_apt_ipmc_info_t *) * custom_data, FALSE, FALSE, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_ipmc_ipv6_lpm_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_ipmc_alloc(unit, number_of_calls, (dnx_apt_ipmc_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_ipmc_generate
                    (unit, number_of_calls, (dnx_apt_ipmc_info_t *) * custom_data, TRUE, FALSE, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_ipmc_ipv4_em_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_ipmc_alloc(unit, number_of_calls, (dnx_apt_ipmc_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_ipmc_generate
                    (unit, number_of_calls, (dnx_apt_ipmc_info_t *) * custom_data, FALSE, TRUE, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_ipmc_ipv6_em_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_ipmc_alloc(unit, number_of_calls, (dnx_apt_ipmc_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_ipmc_generate
                    (unit, number_of_calls, (dnx_apt_ipmc_info_t *) * custom_data, TRUE, TRUE, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_ipmc_ipv4_tcam_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_ipmc_alloc(unit, number_of_calls, (dnx_apt_ipmc_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_ipmc_generate
                    (unit, number_of_calls, (dnx_apt_ipmc_info_t *) * custom_data, FALSE, FALSE, TRUE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_ipmc_ipv6_tcam_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_ipmc_alloc(unit, number_of_calls, (dnx_apt_ipmc_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_ipmc_generate
                    (unit, number_of_calls, (dnx_apt_ipmc_info_t *) * custom_data, TRUE, FALSE, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 Route shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_ipmc_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_ipmc_add(unit, &((dnx_apt_ipmc_info_t *) custom_data)[call_iter].ipmc);
}

static shr_error_e
dnx_apt_ipmc_update_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    dnx_apt_ipmc_info_t *ipmc_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        ipmc_info = &((dnx_apt_ipmc_info_t *) custom_data)[iter];

        ipmc_info->ipmc.flags &= (~BCM_IPMC_REPLACE);
        ipmc_info->ipmc.l3a_intf = ipmc_info->intf_insert;

        SHR_IF_ERR_EXIT(bcm_ipmc_add(unit, &ipmc_info->ipmc));

        ipmc_info->ipmc.flags |= BCM_IPMC_REPLACE;
        ipmc_info->ipmc.l3a_intf = ipmc_info->intf_update;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_ipmc_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    return bcm_ipmc_remove_all(unit);
}

/*
 * }
 */

/*
 * GLOBALs
 * {
 */

/***********************************************************************************************************************
 * IPMC IPv4 LPM
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_ipmc_ipv4_lpm_insert_object = {
    /** Test name */
    "IPMC_IPv4_LPM_Insert",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when inserting new IPv4 LPM random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_ipmc_ipv4_lpm_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv4_lpm_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv4_lpm_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_ipmc_ipv4_lpm_update_object = {
    /** Test name */
    "IPMC_IPv4_LPM_Update",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when updating existing IPv4 LPM random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_ipmc_ipv4_lpm_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv4_lpm_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv4_lpm_init,
    /** Run init procedure */
    dnx_apt_ipmc_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

/***********************************************************************************************************************
 * IPMC IPv6 LPM
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_ipmc_ipv6_lpm_insert_object = {
    /** Test name */
    "IPMC_IPv6_LPM_Insert",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when inserting new IPv6 LPM random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_ipmc_ipv6_lpm_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv6_lpm_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv6_lpm_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_ipmc_ipv6_lpm_update_object = {
    /** Test name */
    "IPMC_IPv6_LPM_Update",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when updating existing IPv6 LPM random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM,
    /** Time threshold */
    dnx_apt_ipmc_ipv6_lpm_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv6_lpm_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv6_lpm_init,
    /** Run init procedure */
    dnx_apt_ipmc_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

/***********************************************************************************************************************
 * IPMC IPv4 EM
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_ipmc_ipv4_em_insert_object = {
    /** Test name */
    "IPMC_IPv4_EM_Insert",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when inserting new IPv4 EM random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_ipmc_ipv4_em_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv4_em_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv4_em_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_ipmc_ipv4_em_update_object = {
    /** Test name */
    "IPMC_IPv4_EM_Update",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when updating existing IPv4 EM random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_ipmc_ipv4_em_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv4_em_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv4_em_init,
    /** Run init procedure */
    dnx_apt_ipmc_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

/***********************************************************************************************************************
 * IPMC IPv6 EM
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_ipmc_ipv6_em_insert_object = {
    /** Test name */
    "IPMC_IPv6_EM_Insert",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when inserting new IPv6 EM random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_ipmc_ipv6_em_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv6_em_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv6_em_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_ipmc_ipv6_em_update_object = {
    /** Test name */
    "IPMC_IPv6_EM_Update",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when updating existing IPv6 EM random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_ipmc_ipv6_em_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv6_em_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv6_em_init,
    /** Run init procedure */
    dnx_apt_ipmc_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

/***********************************************************************************************************************
 * IPMC IPv4 TCAM
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_ipmc_ipv4_tcam_insert_object = {
    /** Test name */
    "IPMC_IPv4_TCAM_Insert",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when inserting new IPv4 TCAM random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in internal TCAM.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_TCAM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_ipmc_ipv4_tcam_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv4_tcam_insert_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv4_tcam_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_ipmc_ipv4_tcam_update_object = {
    /** Test name */
    "IPMC_IPv4_TCAM_Update",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when updating existing IPv4 TCAM random entries. "
        "When KBP is enabled entries will be managed in KBP. Otherwise they will be managed in internal TCAM.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_SUPPORTED,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_TCAM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_ipmc_ipv4_tcam_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_ipmc_ipv4_tcam_update_optional_threshold_info,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv4,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv4_tcam_init,
    /** Run init procedure */
    dnx_apt_ipmc_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

/***********************************************************************************************************************
 * IPMC IPv6 TCAM
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_ipmc_ipv6_tcam_insert_object = {
    /** Test name */
    "IPMC_IPv6_TCAM_Insert",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when inserting new IPv6 TCAM random entries. "
        "The entries will be managed in KBP.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_ONLY,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_TCAM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_ipmc_ipv6_tcam_insert_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv6_tcam_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_ipmc_ipv6_tcam_update_object = {
    /** Test name */
    "IPMC_IPv6_TCAM_Update",
    /** Test description */
    "Measures the performance of bcm_ipmc_add() when updating existing IPv6 TCAM random entries. "
        "The entries will be managed in KBP.",
    /** Support flags */
    DNX_APT_SUPPORT_KBP_ONLY,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_IPMC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_IPMC_TCAM_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table*/
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_ipmc_ipv6_tcam_update_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    dnx_apt_kbp_application_support_ipv6,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_ipmc_ipv6_tcam_init,
    /** Run init procedure */
    dnx_apt_ipmc_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_ipmc_add_exec,
    /** Run deinit procedure */
    dnx_apt_ipmc_clear_exec,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
