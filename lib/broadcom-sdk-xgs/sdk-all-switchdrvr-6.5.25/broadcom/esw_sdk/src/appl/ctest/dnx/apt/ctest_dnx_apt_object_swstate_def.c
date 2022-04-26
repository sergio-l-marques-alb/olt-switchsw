/*
 * ! \file ctest_dnx_apt_object_mpls_def.c
 * Contains the functions and definitions for MPLS API performance test objects.
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
#include <soc/dnx/swstate/auto_generated/access/example_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_RUNS     4
#define DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_CALLS    1000

#define DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS     dnx_data_module_testing.example_tests.large_nof_elements_get(unit)
#define DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS     1100

/** SWSTATE rate */
#define DNX_APT_SWSTATE_HTB_SMALL_INSERT_RATE                400000
#define DNX_APT_SWSTATE_HTB_SMALL_DELETE_RATE                400000
#define DNX_APT_SWSTATE_HTB_SMALL_FIND_RATE                  400000
#define DNX_APT_SWSTATE_HTB_LARGE_INSERT_RATE                280000
#define DNX_APT_SWSTATE_HTB_LARGE_DELETE_RATE                280000
#define DNX_APT_SWSTATE_HTB_LARGE_FIND_RATE                  280000

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_SWSTATE_TIME_THRESHOLD(_unit_, _test_, _test_type_) \
    DNX_APT_RATE_TO_TIME(DNX_APT_SWSTATE_##_test_##_##_test_type_##_RATE)

#define DNX_APT_SWSTATE_OPTIONAL_THRESHOLD_INFO(_info_, _test_, _test_type_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_SWSTATE_##_test_##_##_test_type_##_RATE))

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/***********************************************************************************************************************
 * SWSTATE time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_swstate_htb_small_insert_time_threshold(
    int unit)
{
    return DNX_APT_SWSTATE_TIME_THRESHOLD(unit, HTB_SMALL, INSERT);
}

static double
dnx_apt_swstate_htb_small_find_time_threshold(
    int unit)
{
    return DNX_APT_SWSTATE_TIME_THRESHOLD(unit, HTB_SMALL, FIND);
}

static double
dnx_apt_swstate_htb_small_delete_time_threshold(
    int unit)
{
    return DNX_APT_SWSTATE_TIME_THRESHOLD(unit, HTB_SMALL, DELETE);
}

static double
dnx_apt_swstate_htb_large_insert_time_threshold(
    int unit)
{
    return DNX_APT_SWSTATE_TIME_THRESHOLD(unit, HTB_LARGE, INSERT);
}

static double
dnx_apt_swstate_htb_large_find_time_threshold(
    int unit)
{
    return DNX_APT_SWSTATE_TIME_THRESHOLD(unit, HTB_LARGE, FIND);
}

static double
dnx_apt_swstate_htb_large_delete_time_threshold(
    int unit)
{
    return DNX_APT_SWSTATE_TIME_THRESHOLD(unit, HTB_LARGE, DELETE);
}

/***********************************************************************************************************************
 * SWSTATE optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_swstate_htb_small_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    return DNX_APT_SWSTATE_OPTIONAL_THRESHOLD_INFO(info, HTB_SMALL, INSERT);
}

static shr_error_e
dnx_apt_swstate_htb_small_find_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    return DNX_APT_SWSTATE_OPTIONAL_THRESHOLD_INFO(info, HTB_SMALL, FIND);
}

static shr_error_e
dnx_apt_swstate_htb_small_delete_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    return DNX_APT_SWSTATE_OPTIONAL_THRESHOLD_INFO(info, HTB_SMALL, DELETE);
}

static shr_error_e
dnx_apt_swstate_htb_large_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    return DNX_APT_SWSTATE_OPTIONAL_THRESHOLD_INFO(info, HTB_LARGE, INSERT);
}

static shr_error_e
dnx_apt_swstate_htb_large_find_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    return DNX_APT_SWSTATE_OPTIONAL_THRESHOLD_INFO(info, HTB_LARGE, FIND);
}

static shr_error_e
dnx_apt_swstate_htb_large_delete_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    return DNX_APT_SWSTATE_OPTIONAL_THRESHOLD_INFO(info, HTB_LARGE, DELETE);
}

static ctest_soc_property_t dnx_apt_swstate_soc_properties_set[] = {
    {"custom_feature_example_init", "1"},
    {NULL}
};

/***********************************************************************************************************************
 * SWSTATE init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_swstate_htb_small_insert_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    uint8 success;
    int ind;
    int *arr;

    SHR_FUNC_INIT_VARS(unit);

    (*custom_data) = sal_alloc(sizeof(int) * DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS, "arr");
    arr = (int *) (*custom_data);
    /*
     * Initialize assist array 
     */
    for (ind = 0; ind < DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS; ind++)
    {
        arr[ind] = ind;
    }

    for (ind = 0; ind < DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS - number_of_calls; ind++)
    {
        SHR_IF_ERR_EXIT(example.htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_small_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    uint8 success;
    int *arr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_swstate_htb_small_insert_init(unit, number_of_calls, custom_data));
    arr = (int *) (*custom_data);

    for (int ind = DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS - number_of_calls;
         ind < DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS; ind++)
    {
        SHR_IF_ERR_EXIT(example.htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_large_insert_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    uint8 success;
    int ind;
    int *arr;

    SHR_FUNC_INIT_VARS(unit);

    (*custom_data) = sal_alloc(sizeof(int) * DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS, "arr");
    arr = (int *) (*custom_data);

    /*
     * Initialize assist array 
     */
    for (ind = 0; ind < DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS; ind++)
    {
        arr[ind] = ind;
        SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));
    }

    for (ind = 0; ind < DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS - number_of_calls; ind++)
    {
        SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_large_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    uint8 success;
    int *arr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_swstate_htb_large_insert_init(unit, number_of_calls, custom_data));
    arr = (int *) (*custom_data);

    for (int ind = DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS - number_of_calls;
         ind < DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS; ind++)
    {
        SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * SWSTATE execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_swstate_htb_small_insret_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    uint8 success;
    int ind;
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    ind = DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS - call_iter;
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_large_insret_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    uint8 success;
    int ind;
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    ind = DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS - call_iter;
    SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_small_find_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    uint8 found;
    int ind;
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    ind = DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS - call_iter;
    SHR_IF_ERR_EXIT(example.htb.find(unit, 1, &(arr[ind]), &(arr[ind]), &found));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_large_find_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    uint8 found;
    int ind;
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    ind = DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS - call_iter;
    SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.find(unit, 1, &(arr[ind]), &(arr[ind]), &found));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_small_delete_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    int ind;
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    ind = DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS - call_iter;
    SHR_IF_ERR_EXIT(example.htb.delete(unit, 1, &(arr[ind])));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_large_delete_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    int ind;
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    ind = DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS - call_iter;
    SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.delete(unit, 1, &(arr[ind])));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * SWSTATE post execution functions
 **********************************************************************************************************************/

static shr_error_e
dnx_apt_swstate_htb_small_insret_post_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    for (int ind = DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS - number_of_calls;
         ind < DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS; ind++)
    {
        SHR_IF_ERR_EXIT(example.htb.delete(unit, 1, &(arr[ind])));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_large_insret_post_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    for (int ind = DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS - number_of_calls;
         ind < DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS; ind++)
    {
        SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.delete(unit, 1, &(arr[ind])));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_small_delete_post_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    uint8 success;
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    for (int ind = DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS - number_of_calls;
         ind < DNX_APT_SWSTATE_HTB_SMALL_NOF_ELEMENTS; ind++)
    {
        SHR_IF_ERR_EXIT(example.htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_large_delete_post_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    uint8 success;
    int *arr = (int *) (custom_data);

    SHR_FUNC_INIT_VARS(unit);

    for (int ind = DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS - number_of_calls;
         ind < DNX_APT_SWSTATE_HTB_LARGE_NOF_ELEMENTS; ind++)
    {
        SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.insert(unit, 1, &(arr[ind]), &(arr[ind]), &success));
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * SWSTATE deinit functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_swstate_htb_small_deinit(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(example.htb.delete_all(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_swstate_htb_large_deinit(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.delete_all(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * SWSTATE
 **********************************************************************************************************************/

const dnx_apt_object_def_t dnx_apt_swstate_htb_small_insert_object = {
    /** Test name */
    "SWSTATE_HTB_Small_insert",
    /** Test description */
    "Measures the performance of insertion to small(1100 elements) hash table",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_swstate_htb_small_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_swstate_htb_small_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    dnx_apt_swstate_soc_properties_set,
    /** Test init procedure */
    dnx_apt_swstate_htb_small_insert_init,
    /** Run init procedure (before each run)*/
    NULL,
    /** Test execution procedure */
    dnx_apt_swstate_htb_small_insret_exec,
    /** Run deinit procedure (post_execute of each run)*/
    dnx_apt_swstate_htb_small_insret_post_exec,
    /** Test deinit procedure */
    dnx_apt_swstate_htb_small_deinit
};

const dnx_apt_object_def_t dnx_apt_swstate_htb_large_insert_object = {
    /** Test name */
    "SWSTATE_HTB_large_insert",
    /** Test description */
    "Measures the performance of insertion to large(3M elements) hash table",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_swstate_htb_large_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_swstate_htb_large_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    dnx_apt_swstate_soc_properties_set,
    /** Test init procedure */
    dnx_apt_swstate_htb_large_insert_init,
    /** Run init procedure (before each run)*/
    NULL,
    /** Test execution procedure */
    dnx_apt_swstate_htb_large_insret_exec,
    /** Run deinit procedure (post_execute of each run)*/
    dnx_apt_swstate_htb_large_insret_post_exec,
    /** Test deinit procedure */
    dnx_apt_swstate_htb_large_deinit
};

const dnx_apt_object_def_t dnx_apt_swstate_htb_small_find_object = {
    /** Test name */
    "SWSTATE_HTB_Small_find",
    /** Test description */
    "Measures the performance of finding to small(1100 elements) hash table",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_swstate_htb_small_find_time_threshold,
    /** Optional threshold info */
    dnx_apt_swstate_htb_small_find_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    dnx_apt_swstate_soc_properties_set,
    /** Test init procedure */
    dnx_apt_swstate_htb_small_init,
    /** Run init procedure (before each run)*/
    NULL,
    /** Test execution procedure */
    dnx_apt_swstate_htb_small_find_exec,
    /** Run deinit procedure (post_execute of each run)*/
    NULL,
    /** Test deinit procedure */
    dnx_apt_swstate_htb_small_deinit
};

const dnx_apt_object_def_t dnx_apt_swstate_htb_large_find_object = {
    /** Test name */
    "SWSTATE_HTB_Large_find",
    /** Test description */
    "Measures the performance of finding to large(3M elements) hash table",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_swstate_htb_large_find_time_threshold,
    /** Optional threshold info */
    dnx_apt_swstate_htb_large_find_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    dnx_apt_swstate_soc_properties_set,
    /** Test init procedure */
    dnx_apt_swstate_htb_large_init,
    /** Run init procedure (before each run)*/
    NULL,
    /** Test execution procedure */
    dnx_apt_swstate_htb_large_find_exec,
    /** Run deinit procedure (post_execute of each run)*/
    NULL,
    /** Test deinit procedure */
    dnx_apt_swstate_htb_large_deinit
};

const dnx_apt_object_def_t dnx_apt_swstate_htb_small_delete_object = {
    /** Test name */
    "SWSTATE_HTB_Small_delete",
    /** Test description */
    "Measures the performance of deletion to small(1100 elements) hash table",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_swstate_htb_small_delete_time_threshold,
    /** Optional threshold info */
    dnx_apt_swstate_htb_small_delete_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    dnx_apt_swstate_soc_properties_set,
    /** Test init procedure */
    dnx_apt_swstate_htb_small_init,
    /** Run init procedure (before each run)*/
    NULL,
    /** Test execution procedure */
    dnx_apt_swstate_htb_small_delete_exec,
    /** Run deinit procedure (post_execute of each run)*/
    dnx_apt_swstate_htb_small_delete_post_exec,
    /** Test deinit procedure */
    dnx_apt_swstate_htb_small_deinit
};

const dnx_apt_object_def_t dnx_apt_swstate_htb_large_delete_object = {
    /** Test name */
    "SWSTATE_HTB_Large_delete",
    /** Test description */
    "Measures the performance of deletion to large(3M elements) hash table",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_SWSTATE_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_swstate_htb_large_delete_time_threshold,
    /** Optional threshold info */
    dnx_apt_swstate_htb_large_delete_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    dnx_apt_swstate_soc_properties_set,
    /** Test init procedure */
    dnx_apt_swstate_htb_large_init,
    /** Run init procedure (before each run)*/
    NULL,
    /** Test execution procedure */
    dnx_apt_swstate_htb_large_delete_exec,
    /** Run deinit procedure (post_execute of each run)*/
    dnx_apt_swstate_htb_large_delete_post_exec,
    /** Test deinit procedure */
    dnx_apt_swstate_htb_large_deinit
};

/*
 * }
 */
