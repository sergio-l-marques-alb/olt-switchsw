/*
 * ! \file ctest_dnx_apt_object_vlan_port_def.c
 * Contains the functions and definitions for Vlan port create API performance test objects.
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

#define DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_RUNS     10
#define DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_CALLS    1000

#define DNX_APT_VLAN_PORT_EGRESS_RATE                     13800
#define DNX_APT_VLAN_PORT_INGRESS_RATE                    8650
#define DNX_APT_VLAN_PORT_SYMMETRIC_RATE                  4850
#define DNX_APT_VLAN_PORT_ESEM_RATE                       10600
#define DNX_APT_VLAN_PORT_REPLACE_RATE                    3740
#define DNX_APT_VLAN_PORT_NATIVE_RATE                     6190

#define DNX_APT_VLAN_PORT_EGRESS_RATE_JR2_A0              10260
#define DNX_APT_VLAN_PORT_INGRESS_RATE_JR2_A0             7700
#define DNX_APT_VLAN_PORT_SYMMETRIC_RATE_JR2_A0           4170
#define DNX_APT_VLAN_PORT_ESEM_RATE_JR2_A0                9350
#define DNX_APT_VLAN_PORT_REPLACE_RATE_JR2_A0             3700
#define DNX_APT_VLAN_PORT_NATIVE_RATE_JR2_A0              5300

/* 
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_VLAN_PORT_TIME_THRESHOLD(_unit_,  _test_type_) \
        dnxc_data_mgmt_is_jr2_a0(_unit_) ? \
                /** JR2_A0 */ DNX_APT_RATE_TO_TIME(DNX_APT_VLAN_PORT_##_test_type_##_RATE_JR2_A0) : \
                /** All other */ DNX_APT_RATE_TO_TIME(DNX_APT_VLAN_PORT_##_test_type_##_RATE)

#define DNX_APT_VLAN_PORT_OPTIONAL_THRESHOLD_INFO(_info_, _test_type_) \
            sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus (JR2_A0: %.3fus)", \
                         DNX_APT_RATE_TO_TIME(DNX_APT_VLAN_PORT_##_test_type_##_RATE), \
                         DNX_APT_RATE_TO_TIME(DNX_APT_VLAN_PORT_##_test_type_##_RATE_JR2_A0))

/* 
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the Vlan port create API performance tests */
typedef struct
{
    bcm_vlan_port_t vlan_port;
} dnx_apt_vlan_port_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/***********************************************************************************************************************
 * Vlan port create time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_vlan_port_create_egress_time_threshold(
    int unit)
{
    return DNX_APT_VLAN_PORT_TIME_THRESHOLD(unit, EGRESS);
}
static double
dnx_apt_vlan_port_create_ingress_time_threshold(
    int unit)
{
    return DNX_APT_VLAN_PORT_TIME_THRESHOLD(unit, INGRESS);
}
static double
dnx_apt_vlan_port_create_symmetric_time_threshold(
    int unit)
{
    return DNX_APT_VLAN_PORT_TIME_THRESHOLD(unit, SYMMETRIC);
}
static double
dnx_apt_vlan_port_create_native_time_threshold(
    int unit)
{
    return DNX_APT_VLAN_PORT_TIME_THRESHOLD(unit, NATIVE);
}
static double
dnx_apt_vlan_port_create_replace_time_threshold(
    int unit)
{
    return DNX_APT_VLAN_PORT_TIME_THRESHOLD(unit, REPLACE);
}
static double
dnx_apt_vlan_port_create_esem_time_threshold(
    int unit)
{
    return DNX_APT_VLAN_PORT_TIME_THRESHOLD(unit, ESEM);
}

/***********************************************************************************************************************
 * Vlan port create optional threshold info functions
 **********************************************************************************************************************/

static shr_error_e
dnx_apt_vlan_port_create_egress_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_VLAN_PORT_OPTIONAL_THRESHOLD_INFO(info, EGRESS);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_ingress_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_VLAN_PORT_OPTIONAL_THRESHOLD_INFO(info, INGRESS);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_symmetric_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_VLAN_PORT_OPTIONAL_THRESHOLD_INFO(info, SYMMETRIC);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_native_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_VLAN_PORT_OPTIONAL_THRESHOLD_INFO(info, NATIVE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_replace_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_VLAN_PORT_OPTIONAL_THRESHOLD_INFO(info, REPLACE);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_esem_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_VLAN_PORT_OPTIONAL_THRESHOLD_INFO(info, ESEM);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * Vlan port create shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_vlan_port_alloc(
    int unit,
    int number_of_vlan_ports,
    dnx_apt_vlan_port_info_t ** vlan_port_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*vlan_port_info, (sizeof(dnx_apt_vlan_port_info_t) * number_of_vlan_ports), "dnx_apt_vlan_port_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Vlan port create test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_vlan_port_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    int iter;
    bcm_vlan_port_t *vlan_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_vlan_port_alloc(unit, number_of_calls, (dnx_apt_vlan_port_info_t **) custom_data));
    for (iter = 0; iter < number_of_calls; iter++)
    {
        vlan_port = &((dnx_apt_vlan_port_info_t *) * custom_data)[iter].vlan_port;
        bcm_vlan_port_t_init(vlan_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * vlan_port create test pre init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_vlan_port_create_egress_pre_init(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_vlan_port_t *vlan_port;
    for (iter = 0; iter < number_of_calls; iter++)
    {
        vlan_port = &((dnx_apt_vlan_port_info_t *) custom_data)[iter].vlan_port;
        vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
    }
    return BCM_E_NONE;
}

/***********************************************************************************************************************
 * vlan_port create test pre init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_vlan_port_create_ingress_pre_init(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    int match_vlan, match_inner_vlan;
    bcm_vlan_port_t *vlan_port;

    for (match_vlan = 1; match_vlan < BCM_VLAN_MAX; match_vlan++)
    {
        for (match_inner_vlan = 1; match_inner_vlan < BCM_VLAN_MAX; match_inner_vlan++)
        {
            vlan_port = &((dnx_apt_vlan_port_info_t *) custom_data)[iter].vlan_port;
            vlan_port->flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port->match_vlan = match_vlan;
            vlan_port->match_inner_vlan = match_inner_vlan;
            iter++;
            if (iter >= number_of_calls)
            {
                return BCM_E_NONE;
            }
        }
    }
    return BCM_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_esem_pre_init(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    int vsi, match_vlan;
    bcm_vlan_port_t *vlan_port;

    for (vsi = 1; vsi < BCM_VLAN_MAX; vsi++)
    {
        for (match_vlan = 1; match_vlan < BCM_VLAN_MAX; match_vlan++)
        {
            vlan_port = &((dnx_apt_vlan_port_info_t *) custom_data)[iter].vlan_port;
            vlan_port->flags = BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port->vsi = vsi;
            vlan_port->match_vlan = match_vlan;
            iter++;
            if (iter >= number_of_calls)
            {
                return BCM_E_NONE;
            }
        }
    }
    return BCM_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_symmetric_pre_init(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    int match_vlan, match_inner_vlan;
    bcm_vlan_port_t *vlan_port;

    for (match_vlan = 1; match_vlan < BCM_VLAN_MAX; match_vlan++)
    {
        for (match_inner_vlan = 1; match_inner_vlan < BCM_VLAN_MAX; match_inner_vlan++)
        {
            vlan_port = &((dnx_apt_vlan_port_info_t *) custom_data)[iter].vlan_port;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port->match_vlan = match_vlan;
            vlan_port->match_inner_vlan = match_inner_vlan;
            iter++;
            if (iter >= number_of_calls)
            {
                return BCM_E_NONE;
            }
        }
    }
    return BCM_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_native_pre_init(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    bcm_vlan_port_t *vlan_port;

    while (iter < number_of_calls)
    {
        vlan_port = &((dnx_apt_vlan_port_info_t *) custom_data)[iter].vlan_port;
        vlan_port->flags = BCM_VLAN_PORT_NATIVE;
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
        iter++;
    }
    return BCM_E_NONE;
}

static shr_error_e
dnx_apt_vlan_port_create_replace_pre_init(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    int match_vlan, match_inner_vlan;
    bcm_vlan_port_t *vlan_port;
    SHR_FUNC_INIT_VARS(unit);

    for (match_vlan = 1; match_vlan < BCM_VLAN_MAX; match_vlan++)
    {
        for (match_inner_vlan = 1; match_inner_vlan < BCM_VLAN_MAX; match_inner_vlan++)
        {
            vlan_port = &((dnx_apt_vlan_port_info_t *) custom_data)[iter].vlan_port;
            bcm_vlan_port_t_init(vlan_port);
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port->match_vlan = match_vlan;
            vlan_port->match_inner_vlan = match_inner_vlan;
            SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, vlan_port));
            vlan_port->flags = BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
            iter++;
            if (iter >= number_of_calls)
            {
                return BCM_E_NONE;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Vlan port create shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_vlan_port_create_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &((dnx_apt_vlan_port_info_t *) custom_data)[call_iter].vlan_port));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * Vlan port create shared execution functions
 **********************************************************************************************************************/

static shr_error_e
dnx_apt_vlan_port_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    bcm_vlan_port_t *vlan_port;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        vlan_port = &((dnx_apt_vlan_port_info_t *) custom_data)[iter].vlan_port;
        SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, vlan_port->vlan_port_id));
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

const dnx_apt_object_def_t dnx_apt_vlan_port_create_egress_object = {
    /** Test name */
    "Vlan_Port_Create_Egress",
    /** Test description */
    "Measures the egress performance of bcm_vlan_port_create(). ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_vlan_port_create_egress_time_threshold,
    /** Optional threshold info */
    dnx_apt_vlan_port_create_egress_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_vlan_port_init,
    /** Run init procedure */
    dnx_apt_vlan_port_create_egress_pre_init,
    /** Test execution procedure */
    dnx_apt_vlan_port_create_exec,
    /** Run deinit procedure */
    dnx_apt_vlan_port_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_vlan_port_create_ingress_object = {
    /** Test name */
    "Vlan_Port_Create_Ingress",
    /** Test description */
    "Measures the ingress performance of bcm_vlan_port_create(). ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_vlan_port_create_ingress_time_threshold,
    /** Optional threshold info */
    dnx_apt_vlan_port_create_ingress_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_vlan_port_init,
    /** Run init procedure */
    dnx_apt_vlan_port_create_ingress_pre_init,
    /** Test execution procedure */
    dnx_apt_vlan_port_create_exec,
    /** Run deinit procedure */
    dnx_apt_vlan_port_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_vlan_port_create_symmetric_object = {
    /** Test name */
    "Vlan_Port_Create_Symmetric",
    /** Test description */
    "Measures the symmetric performance of bcm_vlan_port_create(). ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_vlan_port_create_symmetric_time_threshold,
    /** Optional threshold info */
    dnx_apt_vlan_port_create_symmetric_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_vlan_port_init,
    /** Run init procedure */
    dnx_apt_vlan_port_create_symmetric_pre_init,
    /** Test execution procedure */
    dnx_apt_vlan_port_create_exec,
    /** Run deinit procedure */
    dnx_apt_vlan_port_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_vlan_port_create_esem_object = {
    /** Test name */
    "Vlan_Port_Create_Esem",
    /** Test description */
    "Measures the esem object performance of bcm_vlan_port_create().",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_vlan_port_create_esem_time_threshold,
    /** Optional threshold info */
    dnx_apt_vlan_port_create_esem_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_vlan_port_init,
    /** Run init procedure */
    dnx_apt_vlan_port_create_esem_pre_init,
    /** Test execution procedure */
    dnx_apt_vlan_port_create_exec,
    /** Run deinit procedure */
    dnx_apt_vlan_port_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_vlan_port_create_native_object = {
    /** Test name */
    "Vlan_Port_Create_Native",
    /** Test description */
    "Measures the native performance of bcm_vlan_port_create(). ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_vlan_port_create_native_time_threshold,
    /** Optional threshold info */
    dnx_apt_vlan_port_create_native_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_vlan_port_init,
    /** Run init procedure */
    dnx_apt_vlan_port_create_native_pre_init,
    /** Test execution procedure */
    dnx_apt_vlan_port_create_exec,
    /** Run deinit procedure */
    dnx_apt_vlan_port_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_vlan_port_create_replace_object = {
    /** Test name */
    "Vlan_Port_Create_Replace",
    /** Test description */
    "Measures the replace performance of bcm_vlan_port_create(). ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_VLAN_PORT_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_vlan_port_create_replace_time_threshold,
    /** Optional threshold info */
    dnx_apt_vlan_port_create_replace_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_vlan_port_init,
    /** Run init procedure */
    dnx_apt_vlan_port_create_replace_pre_init,
    /** Test execution procedure */
    dnx_apt_vlan_port_create_exec,
    /** Run deinit procedure */
    dnx_apt_vlan_port_clear_exec,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
