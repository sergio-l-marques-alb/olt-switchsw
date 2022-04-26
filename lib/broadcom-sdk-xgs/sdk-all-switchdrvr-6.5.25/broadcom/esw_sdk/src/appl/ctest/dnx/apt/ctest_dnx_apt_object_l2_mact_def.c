/*
 * ! \file ctest_dnx_apt_object_l2_mact_def.c
 * Contains the functions and definitions for L2 MACT API performance test objects.
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
#include <bcm/l2.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/l2/l2_addr.h>
#include <bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_L2_MACT_DEFAULT_NUMBER_OF_RUNS      10
#define DNX_APT_L2_MACT_DEFAULT_NUMBER_OF_CALLS     50000

/** Rates */

#define DNX_APT_L2_MACT_INSERT_RATE      1000

/*
* }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_L2_MACT_TIME_THRESHOLD(_unit_, _test_type_) \
                        DNX_APT_RATE_TO_TIME(DNX_APT_L2_MACT_##_test_type_##_RATE)

#define DNX_APT_L2_MACT_OPTIONAL_THRESHOLD_INFO(_info_, _test_type_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "EM: %.3fus \n", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L2_MACT_##_test_type_##_RATE))
/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the L2 MACT API performance tests */
typedef struct
{
    bcm_l2_addr_t l2addr;
} dnx_apt_l2_mact_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/***********************************************************************************************************************
 * L2 MACT time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_l2_mact_insert_time_threshold(
    int unit)
{
    return DNX_APT_L2_MACT_TIME_THRESHOLD(unit, INSERT);
}

/***********************************************************************************************************************
 * L2 MACT optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l2_mact_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L2_MACT_OPTIONAL_THRESHOLD_INFO(info, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * L2 MACT shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l2_mac_alloc(
    int unit,
    int number_of_macs,
    dnx_apt_l2_mact_info_t ** l2_mac_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*l2_mac_info, (sizeof(dnx_apt_l2_mact_info_t) * number_of_macs), "dnx_apt_l2_mact_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L2 MACT generate random MAC entries
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l2_mac_generate(
    int unit,
    int number_of_macs,
    dnx_apt_l2_mact_info_t * l2_mac_info)
{
    int iter = 0;
    bcm_mac_t mac_addr = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    bcm_vlan_t vid = 1;
    int random_mac_address;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, 1024 * 1024));
    dnx_apt_random_seed(0);

    for (iter = 0; iter < number_of_macs; iter++)
    {
        /** Randomized mac entries */
        random_mac_address = dnx_apt_unique_random_get(iter);
        mac_addr[3] = (random_mac_address >> 16) & UTILEX_U8_MAX;
        mac_addr[4] = (random_mac_address >> 8) & UTILEX_U8_MAX;
        mac_addr[5] = (random_mac_address) & UTILEX_U8_MAX;
        sal_memcpy(l2_mac_info[iter].l2addr.mac, mac_addr, 6);

        bcm_l2_addr_t_init(&l2_mac_info[iter].l2addr, mac_addr, vid);

        l2_mac_info[iter].l2addr.port = 200;
        l2_mac_info[iter].l2addr.flags = BCM_L2_STATIC;
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L2 MACT test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l2_mact_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_l2_mac_alloc(unit, number_of_calls, (dnx_apt_l2_mact_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_l2_mac_generate(unit, number_of_calls, (dnx_apt_l2_mact_info_t *) * custom_data));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L2 MACT shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l2_mact_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{

    return bcm_l2_addr_add(unit, &((dnx_apt_l2_mact_info_t *) custom_data)[call_iter].l2addr);

}

static shr_error_e
dnx_apt_l2_mact_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    dnx_apt_l2_mact_info_t *l2_mac_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        l2_mac_info = &((dnx_apt_l2_mact_info_t *) custom_data)[iter];
        SHR_IF_ERR_EXIT(bcm_l2_addr_delete(unit, l2_mac_info->l2addr.mac, l2_mac_info->l2addr.vid));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * GLOBALs
 * {
 */

const dnx_apt_object_def_t dnx_apt_l2_mact_insert_object = {
    /** Test name */
    "L2_MACT_Insert",
    /** Test description */
    "Measures the performance of bcm_l2_addr_add() when inserting new MAC random entries.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Number of runs */
    DNX_APT_L2_MACT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L2_MACT_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM,
    /** Time threshold */
    dnx_apt_l2_mact_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l2_mact_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l2_mact_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l2_mact_add_exec,
    /** Run deinit procedure */
    dnx_apt_l2_mact_clear_exec,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
