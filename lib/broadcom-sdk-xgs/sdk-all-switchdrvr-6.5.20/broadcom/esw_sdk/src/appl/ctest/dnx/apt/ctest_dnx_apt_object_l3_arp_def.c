/*
 * ! \file ctest_dnx_apt_object_l3_arp_def.c
 * Contains the functions and definitions for L3 arp API performance test objects.
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
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_L3_ARP_DEFAULT_NUMBER_OF_RUNS     10
#define DNX_APT_L3_ARP_DEFAULT_NUMBER_OF_CALLS    10000

#define DNX_APT_L3_ARP_INSERT_RATE                14700
#define DNX_APT_L3_ARP_UPDATE_RATE                11400

#define DNX_APT_L3_ARP_INSERT_RATE_JR2_A0         11400
#define DNX_APT_L3_ARP_UPDATE_RATE_JR2_A0         10700

/* 
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_L3_ARP_TIME_THRESHOLD(_unit_,  _test_type_) \
        dnxc_data_mgmt_is_jr2_a0(_unit_) ? \
                /** JR2_A0 */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ARP_##_test_type_##_RATE_JR2_A0) : \
                /** All other */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ARP_##_test_type_##_RATE)

#define DNX_APT_L3_ARP_OPTIONAL_THRESHOLD_INFO(_info_, _test_type_) \
            sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus (JR2_A0: %.3fus)", \
                         DNX_APT_RATE_TO_TIME(DNX_APT_L3_ARP_##_test_type_##_RATE), \
                         DNX_APT_RATE_TO_TIME(DNX_APT_L3_ARP_##_test_type_##_RATE_JR2_A0))

/* 
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the L3 arp API performance tests */
typedef struct
{
    bcm_l3_egress_t egr;
} dnx_apt_l3_arp_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/***********************************************************************************************************************
 * L3 arp time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_l3_arp_insert_time_threshold(
    int unit)
{
    return DNX_APT_L3_ARP_TIME_THRESHOLD(unit, INSERT);
}

static double
dnx_apt_l3_arp_update_time_threshold(
    int unit)
{
    return DNX_APT_L3_ARP_TIME_THRESHOLD(unit, UPDATE);
}

/***********************************************************************************************************************
 * L3 arp optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_arp_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ARP_OPTIONAL_THRESHOLD_INFO(info, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_arp_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_ARP_OPTIONAL_THRESHOLD_INFO(info, UPDATE);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * L3 arp shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_arp_alloc(
    int unit,
    int number_of_arps,
    dnx_apt_l3_arp_info_t ** l3_arp_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*l3_arp_info, (sizeof(dnx_apt_l3_arp_info_t) * number_of_arps), "dnx_apt_l3_arp_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_arp_generate(
    int unit,
    int number_of_arps,
    dnx_apt_l3_arp_info_t * l3_arp_info)
{
    int iter = 0;
    int random_mac_address;
    bcm_mac_t arp_mac_add = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, 1024 * 1024));
    dnx_apt_random_seed(0);

    SHR_NULL_CHECK(l3_arp_info, _SHR_E_INTERNAL, "l3_arp_info");
    for (iter = 0; iter < number_of_arps; iter++)
    {
        bcm_l3_egress_t_init(&l3_arp_info[iter].egr);
        random_mac_address = dnx_apt_unique_random_get(iter);
        arp_mac_add[3] = (random_mac_address & 0xff0000) >> 16;
        arp_mac_add[4] = (random_mac_address & 0xff00) >> 8;
        arp_mac_add[5] = (random_mac_address & 0xff);
        sal_memcpy(l3_arp_info[iter].egr.mac_addr, arp_mac_add, 6);
        l3_arp_info[iter].egr.vlan = 100 + random_mac_address % 0xEFF;
    }
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 arp test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_arp_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_l3_arp_alloc(unit, number_of_calls, (dnx_apt_l3_arp_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_l3_arp_generate(unit, number_of_calls, (dnx_apt_l3_arp_info_t *) * custom_data));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 arp shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_arp_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    bcm_if_t l3egid_null;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_l3_egress_create
                    (unit, BCM_L3_EGRESS_ONLY, &((dnx_apt_l3_arp_info_t *) custom_data)[call_iter].egr, &l3egid_null));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 arp shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_arp_update_pre_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    int iter = 0;
    bcm_if_t l3egid_null;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < call_iter; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_l3_egress_create
                        (unit, BCM_L3_EGRESS_ONLY, &((dnx_apt_l3_arp_info_t *) custom_data)[iter].egr, &l3egid_null));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_arp_update_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    bcm_if_t l3egid_null;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_l3_egress_create
                    (unit, BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY, &((dnx_apt_l3_arp_info_t *) custom_data)[call_iter].egr,
                     &l3egid_null));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_arp_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    dnx_apt_l3_arp_info_t *l3_arp_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        l3_arp_info = &((dnx_apt_l3_arp_info_t *) custom_data)[iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, l3_arp_info->egr.encap_id));
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

const dnx_apt_object_def_t dnx_apt_l3_arp_insert_object = {
    /** Test name */
    "L3_Arp_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_egress_create() when inserting new l3 arp entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ARP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ARP_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_l3_arp_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_arp_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_arp_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_arp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_arp_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_arp_update_object = {
    /** Test name */
    "L3_Arp_Update",
    /** Test description */
    "Measures the performance of bcm_l3_egress_create() when updating existing arp entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ARP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ARP_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_l3_arp_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_arp_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_arp_init,
    /** Run init procedure */
    dnx_apt_l3_arp_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_arp_update_exec,
    /** Run deinit procedure */
    dnx_apt_l3_arp_clear_exec,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
