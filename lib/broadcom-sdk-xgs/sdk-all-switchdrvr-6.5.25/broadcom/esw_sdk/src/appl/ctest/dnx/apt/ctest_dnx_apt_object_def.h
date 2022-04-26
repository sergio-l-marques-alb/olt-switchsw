/* \file ctest_dnx_apt_object_def.h
 *
 * Contains the structure definition of an API performance test object and the test functions prototypes.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNX_APT_OBJECT_DEF_H_INCLUDED
#define CTEST_DNX_APT_OBJECT_DEF_H_INCLUDED

/*
* Include files.
* {
*/

#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm/types.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnxc/dnxc_regression_utils.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_MAX_NAME_LENGTH             64
#define DNX_APT_MAX_DESCRIPTION_LENGTH      512

#define DNX_APT_DEFAULT_NUMBER_OF_RUNS      10
#define DNX_APT_DEFAULT_NUMBER_OF_CALLS     10000

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define DNX_APT_TIME_TO_RATE(_microseconds_) ((_microseconds_ == 0) ? 0 : (int)(1000000.0 / (_microseconds_)))
#define DNX_APT_RATE_TO_TIME(_entries_per_second_) ((_entries_per_second_ == 0) ? 0.0 : (double)(1000000.0 / ((double)(_entries_per_second_))))

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/***********************************************************************************************************************
 * Callback functions types definitions
 **********************************************************************************************************************/
/* *INDENT-OFF* */
typedef double(*DNX_APT_OBJECT_TIME_THRESHOLD_FUNCPTR) (int unit);
typedef shr_error_e(*DNX_APT_OBJECT_OPTIONAL_THRESHOLD_INFO_FUNCPTR) (int unit, char info[DNX_APT_MAX_DESCRIPTION_LENGTH]);
typedef shr_error_e(*DNX_APT_OBJECT_SUPPORT_CHECK_FUNCPTR) (int unit);
typedef dbal_tables_e(*DNX_APT_OBJECT_DBAL_TABLE_FUNCPTR) (int unit);
typedef shr_error_e(*DNX_APT_OBJECT_EXEC_FUNCPTR) (int unit, int call_iter, void *custom_data);
typedef shr_error_e(*DNX_APT_OBJECT_INIT_FUNCPTR) (int unit, int number_of_calls, void **custom_data);
typedef shr_error_e(*DNX_APT_OBJECT_DEINIT_FUNCPTR) (int unit, int number_of_calls, void *custom_data);
typedef shr_error_e(*DNX_APT_OBJECT_PRE_EXEC_FUNCPTR) (int unit, int number_of_calls, void *custom_data);
typedef shr_error_e(*DNX_APT_OBJECT_POST_EXEC_FUNCPTR) (int unit, int number_of_calls, void *custom_data);
/* *INDENT-ON* */


typedef enum
{
    /** Support flags */

    /*
     * Support flags for internal usage only.
     */
    /** Indicates that KBP device check should be skipped. */
    DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP = SAL_BIT(0),
    /** Indicates that KBP device is required when set and not required when not set. */
    DNX_APT_SUPPORT_INTERNAL_KBP_REQUIRED = SAL_BIT(1),

    /*
     * Support flags available to the user.
     */
    /** Indicates that the test is supported also with KBP. */
    DNX_APT_SUPPORT_KBP_SUPPORTED = DNX_APT_SUPPORT_INTERNAL_KBP_CHECK_SKIP,
    /** Indicates that the test is supported only with KBP. */
    DNX_APT_SUPPORT_KBP_ONLY = DNX_APT_SUPPORT_INTERNAL_KBP_REQUIRED,
    /** NOTE: If KBP_SUPPORTED or KBP_ONLY are not provided, the test will NOT be supported with KBP. */

} dnx_apt_support_e;

/***********************************************************************************************************************
 * Measurement validation methods
 **********************************************************************************************************************/
typedef enum
{
    /** Validate using the validation method from the test object (valid as ctest input, not valid for test objects) */
    DNX_APT_VALIDATION_METHOD_DEFAULT = -1,
    DNX_APT_VALIDATION_METHOD_FIRST = 0,

    /** Validate using the worst performing run */
    DNX_APT_VALIDATION_METHOD_WORST_RUN = DNX_APT_VALIDATION_METHOD_FIRST,
    /** Validate using the best performing run */
    DNX_APT_VALIDATION_METHOD_BEST_RUN,
    /** Validate using the average of all runs */
    DNX_APT_VALIDATION_METHOD_AVERAGE_ALL,
    /** Validate using the average of all runs after discarding the worst and best runs */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Validate that at least half of all runs pass the threshold validation */
    DNX_APT_VALIDATION_METHOD_AT_LEAST_HALF_RUNS_PASS,
    /** Skip validation and print average of all runs */
    DNX_APT_VALIDATION_METHOD_SKIP,

    DNX_APT_VALIDATION_METHOD_LAST,
    DNX_APT_VALIDATION_METHOD_NOF = DNX_APT_VALIDATION_METHOD_LAST
} dnx_apt_validation_method_e;

/***********************************************************************************************************************
 * Test object definition structure
 **********************************************************************************************************************/
typedef struct
{
    /** Name of the test. Needs to be unique. */
    char test_name[DNX_APT_MAX_NAME_LENGTH];

    /** Description of the test. Needs to be unique. */
    char test_description[DNX_APT_MAX_DESCRIPTION_LENGTH];

    /** Test support flags */
    dnx_apt_support_e support_flags;

    /** ctest flags */
    uint32 ctest_flags;

    /** Validation method */
    dnx_apt_validation_method_e validation_method;

    /*
     * Number of runs the test will perform.
     * The test will be executed this much number of times to get more measurements for better results.
     * All runs are identical to one another. (The user is supposed to make sure they're identical)
     */
    int number_of_runs;

    /*
     * Number of calls that will be performed with the tested API per run.
     * The tested API is called this much number of times with different input data.
     * Example: Number of entries to be inserted.
     */
    int number_of_calls;
    /*
     * The Descriptor DMA table linked to MDB DDMA testing
     */
    sbusdma_desc_module_enable_e ddma_table;

    /*********************************************** Operations *******************************************************/
    /** See ctest_dnx_apt.h for more information on the test execution flow and usage in example scenario. */
    /*
     * Time threshold retrieval procedure.
     * Returns the execution time of the tested API in us.
     * Should return different time threshold in case of different performance between devices and device devisions.
     * Measured time above the time threshold means the test fails.
     */
    DNX_APT_OBJECT_TIME_THRESHOLD_FUNCPTR time_threshold;

    /*
     * Optional information about test thresholds.
     * The provided string parameter need to be set with the optional threshold information.
     */
    DNX_APT_OBJECT_OPTIONAL_THRESHOLD_INFO_FUNCPTR optional_threshold_info;

    /*
     * Custom test support check procedure.
     * In case the test support cannot be validated using the support flags,
     * the user can implement custom support check that will be called at the beginning of the test.
     */
    DNX_APT_OBJECT_SUPPORT_CHECK_FUNCPTR custom_support_check;

    /*
     * SOC property set structure array.
     * SOC properties are set before initializing the test.
     */
    ctest_soc_property_t *soc_properties_set;

    /*
     * Test initialization procedure.
     * Executed once at the beginning of the test.
     * Used for initializing test specific configurations and custom data.
     * Example: Allocating memory to custom_data for <number_of_calls> random entries.
     * Example2: Create interfaces required for entry insertion.
     */
    DNX_APT_OBJECT_INIT_FUNCPTR init;

    /*
     * Test run initialization procedure.
     * Executed once at the beginning of each test run.
     * Example: When testing entry update performance, add all entries that will be updated.
     */
    DNX_APT_OBJECT_PRE_EXEC_FUNCPTR pre_execute;

    /*
     * Test execution procedure.
     * Executes the tested API.
     * This function will be executed <number_of_calls> times for each run.
     * Example: Call to entry add API with entry info provided from custom_data.
     */
    DNX_APT_OBJECT_EXEC_FUNCPTR execute;

    /*
     * Test run deinitialization procedure.
     * Executed once at the end of each run.
     * Example: Delete all inserted entries to start the next run clean.
     */
    DNX_APT_OBJECT_POST_EXEC_FUNCPTR post_execute;

    /*
     * Test deinitialization procedure.
     * Executed once at the end of the test.
     * Used for deinitializing all configurations performed in the test initialization procedure.
     * Automatically frees custom_data if allocated by the user.
     * Example: Destroy all interfaces created in the init step.
     */
    DNX_APT_OBJECT_DEINIT_FUNCPTR deinit;

} dnx_apt_object_def_t;

/*
 * }
 */

/*
 * GLOBALSs
 * {
 */

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/*
 * }
 */

#endif /* CTEST_DNX_APT_OBJECT_DEF_H_INCLUDED */
