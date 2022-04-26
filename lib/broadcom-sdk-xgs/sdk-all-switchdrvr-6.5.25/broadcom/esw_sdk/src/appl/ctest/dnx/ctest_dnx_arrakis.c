/** \file ctest_dnx_arrakis.c
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * Purpose: Test Arrakis Stats API
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/utils/arrakis_stats.h>
#include <soc/dnx/utils/arrakis_stats_internal.h>
#include <shared/shrextend/shrextend_debug.h>   /* For SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC,
                                                 * SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC */
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>   /* For CTEST_POSTCOMMIT */
#include <sal/types.h>  /* For uint32 */
#include <stdlib.h>     /* For RAND_MAX */
/*
 * Sample size is limited by ARRAKIS_STATS_DIMENSION_STR_MAX_LEN defined in arrakis.stats.h
 */
#define SAMPLE_SIZE_LARGE    7
#define SAMPLE_SIZE_MEDIUM    5
#define SAMPLE_SIZE_SMALL    3
#define RAND_INT_MAX_NUM    10000
#define RAND_INT_MIN_NUM    -10000

/**
 * \brief - Generate random numbers (positive or negative). sal_srand() must be called before executing this function.
 */
static int
arrakis_stats_utils_get_random_number(
    int unit)
{
    /*
     * Statement below uses the positive number returned from sal_rand() to generate positive or negative number 
     */
    return (sal_rand() % (RAND_INT_MAX_NUM + 1 - RAND_INT_MIN_NUM)) + RAND_INT_MIN_NUM;
}

/**
 * \brief - Generate random numbers to fill float array. sal_srand() must be called before executing this function.
 */
static void
arrakis_stats_utils_float_array_random_samples_fill(
    int unit,
    float samples[],
    int samples_nof)
{
    int i;

    for (i = 0; i < samples_nof; i++)
    {
        samples[i] = (float) arrakis_stats_utils_get_random_number(unit) / arrakis_stats_utils_get_random_number(unit);
    }
}

/**
 * \brief - Generate random numbers to fill int array. sal_srand() must be called before executing this function.
 */
static void
arrakis_stats_utils_int_array_random_samples_fill(
    int unit,
    int samples[],
    int samples_nof)
{
    int i;

    for (i = 0; i < samples_nof; i++)
    {
        samples[i] = arrakis_stats_utils_get_random_number(unit);
    }
}

/**
 * \brief - Generate random numbers to fill int array. sal_srand() must be called before executing this function.
 */
static void
arrakis_stats_utils_uint32_array_random_samples_fill(
    int unit,
    uint32 samples[],
    int samples_nof)
{
    int i;

    for (i = 0; i < samples_nof; i++)
    {
        samples[i] = sal_rand();
    }
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_2d_float_samples(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples[1];

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples, 1);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_2d"));

    SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples[0]));

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_2d_int_samples(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    int samples[1];

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_int_array_random_samples_fill(unit, samples, 1);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_2d"));

    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_2", samples[0]));

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_2d_uint32_samples(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    uint32 samples[1];

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_uint32_array_random_samples_fill(unit, samples, 1);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_2d"));

    SHR_IF_ERR_EXIT(arrakis_stat_family_uint32_fill(unit, &stat_family, "Dimension_2", samples[0]));

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_float_samples(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples_dim_1[SAMPLE_SIZE_SMALL];
    float samples_dim_2[SAMPLE_SIZE_SMALL];
    float samples_dim_3[SAMPLE_SIZE_SMALL];
    int i;

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_2, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_3, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_1", samples_dim_1[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples_dim_2[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_3", samples_dim_3[i]));
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_float_samples_multiple_fill(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples[SAMPLE_SIZE_SMALL][3];
    char *dimensions[3] = { "Dimension_1", "Dimension_2", "Dimension_3" };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        arrakis_stats_utils_float_array_random_samples_fill(unit, samples[i], SAMPLE_SIZE_SMALL);
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_multiple_fill
                        (unit, &stat_family, dimensions, SAMPLE_SIZE_SMALL, samples[i]));
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_int_samples(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    int samples_dim_1[SAMPLE_SIZE_SMALL];
    int samples_dim_2[SAMPLE_SIZE_SMALL];
    int samples_dim_3[SAMPLE_SIZE_SMALL];
    int i;

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_int_array_random_samples_fill(unit, samples_dim_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_int_array_random_samples_fill(unit, samples_dim_2, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_int_array_random_samples_fill(unit, samples_dim_3, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_1", samples_dim_1[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_2", samples_dim_2[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_3", samples_dim_3[i]));
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_int_samples_multiple_fill(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    int samples[SAMPLE_SIZE_SMALL][3];
    char *dimensions[3] = { "Dimension_1", "Dimension_2", "Dimension_3" };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        arrakis_stats_utils_int_array_random_samples_fill(unit, samples[i], SAMPLE_SIZE_SMALL);
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_int_multiple_fill
                        (unit, &stat_family, dimensions, SAMPLE_SIZE_SMALL, samples[i]));
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_uint32_samples(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    uint32 samples_dim_1[SAMPLE_SIZE_SMALL];
    uint32 samples_dim_2[SAMPLE_SIZE_SMALL];
    uint32 samples_dim_3[SAMPLE_SIZE_SMALL];
    int i;

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_uint32_array_random_samples_fill(unit, samples_dim_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_uint32_array_random_samples_fill(unit, samples_dim_2, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_uint32_array_random_samples_fill(unit, samples_dim_3, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_uint32_fill(unit, &stat_family, "Dimension_1", samples_dim_1[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_uint32_fill(unit, &stat_family, "Dimension_2", samples_dim_2[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_uint32_fill(unit, &stat_family, "Dimension_3", samples_dim_3[i]));
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_uint32_samples_multiple_fill(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    uint32 samples[SAMPLE_SIZE_SMALL][3];
    char *dimensions[3] = { "Dimension_1", "Dimension_2", "Dimension_3" };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        arrakis_stats_utils_uint32_array_random_samples_fill(unit, samples[i], SAMPLE_SIZE_SMALL);
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_uint32_multiple_fill
                        (unit, &stat_family, dimensions, SAMPLE_SIZE_SMALL, samples[i]));
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_multiple_samples_types(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples_dim_1[SAMPLE_SIZE_SMALL];
    int samples_dim_2[SAMPLE_SIZE_SMALL];
    uint32 samples_dim_3[SAMPLE_SIZE_SMALL];
    int i;

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_int_array_random_samples_fill(unit, samples_dim_2, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_uint32_array_random_samples_fill(unit, samples_dim_3, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_1", samples_dim_1[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_2", samples_dim_2[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_uint32_fill(unit, &stat_family, "Dimension_3", samples_dim_3[i]));
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_mix_fill_and_multiple_fill(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    char *dimensions[3] = { "Dimension_1", "Dimension_2", "Dimension_3" };
    float samples_1[SAMPLE_SIZE_SMALL];
    float samples_2[SAMPLE_SIZE_SMALL];
    float samples_3[SAMPLE_SIZE_SMALL];

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_2, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_3, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_1", samples_1[0]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples_1[1]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_3", samples_1[2]));

    SHR_IF_ERR_EXIT(arrakis_stat_family_float_multiple_fill
                    (unit, &stat_family, dimensions, SAMPLE_SIZE_SMALL, samples_2));

    SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_1", samples_3[0]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples_3[1]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_3", samples_3[2]));

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_stat_family_example_3d_mixed_order_fill(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    int samples[SAMPLE_SIZE_SMALL * 3];

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_int_array_random_samples_fill(unit, samples, SAMPLE_SIZE_SMALL * 3);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_1", samples[0]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_1", samples[1]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_2", samples[2]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_1", samples[3]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_3", samples[4]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_2", samples[5]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_3", samples[6]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_3", samples[7]));
    SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_2", samples[8]));

    SHR_IF_ERR_EXIT(arrakis_stat_family_commit(unit, &stat_family));

exit:
    SHR_FUNC_EXIT;
}

static char *
arrakis_stats_utils_get_rand_string(
    char *str,
    size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size)
    {
        --size;
        for (size_t n = 0; n < size; n++)
        {
            int key = sal_rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

static void
arrakis_stats_utils_set_bsl_severity_for_negative_testing(
    int unit,
    bsl_severity_t * original_severity)
{
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, *original_severity);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, bslSeverityFatal);
}

static shr_error_e
ctest_dnx_arrakis_negative_test_stat_family_name_too_long(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    /*
     * Exceeds stat_family_name valid length 
     */
    char stat_family_name[ARRAKIS_STATS_FAMILY_NAME_MAX_LEN + 1];
    bsl_severity_t original_severity;

    SHR_FUNC_INIT_VARS(unit);
    sal_srand(sal_time_usecs());

    arrakis_stats_utils_get_rand_string(stat_family_name, ARRAKIS_STATS_FAMILY_NAME_MAX_LEN + 1);

    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_MEMORY != arrakis_stat_family_init(unit, &stat_family, stat_family_name))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_MEMORY error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_negative_test_exceeding_dimension_nof(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    char dimension_name[ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN];
    int sample;
    bsl_severity_t original_severity;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    sal_srand(sal_time_usecs());

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < ARRAKIS_STATS_DIMENSIONS_MAX_NOF; i++)
    {
        /*
         * dimension_name length is not important for this test
         */
        arrakis_stats_utils_get_rand_string(dimension_name, 10);
        sample = arrakis_stats_utils_get_random_number(unit);
        SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, dimension_name, sample));
    }

    arrakis_stats_utils_get_rand_string(dimension_name, 10);
    sample = arrakis_stats_utils_get_random_number(unit);
    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_RESOURCE != arrakis_stat_family_int_fill(unit, &stat_family, dimension_name, sample))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_RESOURCE error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_negative_test_samples_str_of_a_dimension_too_long(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    int sample;
    char sample_str[STATS_SAMPLE_STR_MAX_LEN];
    bsl_severity_t original_severity;

    SHR_FUNC_INIT_VARS(unit);
    sal_srand(sal_time_usecs());

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    sample = arrakis_stats_utils_get_random_number(unit);
    sal_snprintf(sample_str, sizeof(sample_str), "%s%d", STATS_DELIMITER, sample);
    while (sal_strnlen(stat_family.samples[0], sizeof(stat_family.samples[0])) +
           sal_strnlen(sample_str, sizeof(sample_str)) < ARRAKIS_STATS_SAMPLES_STR_MAX_LEN)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_1", sample));
        sample = arrakis_stats_utils_get_random_number(unit);
        sal_snprintf(sample_str, sizeof(sample_str), "%s%d", STATS_DELIMITER, sample);
    }

    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_RESOURCE != arrakis_stat_family_int_fill(unit, &stat_family, "Dimension_1", sample))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_RESOURCE error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_negative_test_dimension_name_too_long(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    char dimension[ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN + 1];
    bsl_severity_t original_severity;
    int sample;

    SHR_FUNC_INIT_VARS(unit);
    sal_srand(sal_time_usecs());

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    arrakis_stats_utils_get_rand_string(dimension, ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN + 1);
    sample = arrakis_stats_utils_get_random_number(unit);
    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_MEMORY != arrakis_stat_family_float_fill(unit, &stat_family, dimension, sample))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_MEMORY error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_negative_test_stat_family_name_not_exists(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples_dim_1[SAMPLE_SIZE_SMALL];
    float samples_dim_2[SAMPLE_SIZE_SMALL];
    float samples_dim_3[SAMPLE_SIZE_SMALL];
    bsl_severity_t original_severity;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_2, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_3, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "Bad_Stat_Family_Name"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_1", samples_dim_1[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples_dim_2[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_3", samples_dim_3[i]));
    }

    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_NOT_FOUND != arrakis_stat_family_commit(unit, &stat_family))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_NOT_FOUND error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_negative_test_dimension_nof_does_not_match_stat_family(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples_dim_1[SAMPLE_SIZE_SMALL];
    float samples_dim_2[SAMPLE_SIZE_SMALL];
    bsl_severity_t original_severity;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_2, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_1", samples_dim_1[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples_dim_2[i]));
    }

    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_INTERNAL != arrakis_stat_family_commit(unit, &stat_family))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_INTERNAL error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_negative_test_dimension_name_not_exists_in_stat_family(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples_dim_1[SAMPLE_SIZE_SMALL];
    float samples_dim_2[SAMPLE_SIZE_SMALL];
    float samples_dim_3[SAMPLE_SIZE_SMALL];
    bsl_severity_t original_severity;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_2, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_3, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_1", samples_dim_1[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples_dim_2[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Bad_Dimension_Name", samples_dim_3[i]));
    }

    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_INTERNAL != arrakis_stat_family_commit(unit, &stat_family))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_INTERNAL error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_negative_test_stat_family_has_no_data(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    bsl_severity_t original_severity;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_EMPTY != arrakis_stat_family_commit(unit, &stat_family))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_EMPTY error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}
/*static sh_sand_man_t negative_test_samples_str_of_a_dimension_too_long = {
    "If user call fill API such that the samples string of the dimension exceeds ARRAKIS_STATS_SAMPLES_STR_MAX_LEN"
};*/
static shr_error_e
ctest_dnx_arrakis_negative_test_dimension_do_not_have_the_same_number_of_samples(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples_dim_1[SAMPLE_SIZE_SMALL];
    float samples_dim_2[SAMPLE_SIZE_SMALL];
    float samples_dim_3[SAMPLE_SIZE_SMALL];
    bsl_severity_t original_severity;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_1, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_2, SAMPLE_SIZE_SMALL);
    arrakis_stats_utils_float_array_random_samples_fill(unit, samples_dim_3, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_3d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_1", samples_dim_1[i]));
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples_dim_2[i]));
    }

    SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_3", samples_dim_3[0]));

    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_INTERNAL != arrakis_stat_family_commit(unit, &stat_family))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_INTERNAL error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_arrakis_negative_test_stat_family_with_date_dimension_more_than_one_sample(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    arrakis_stat_family_t stat_family;
    float samples[SAMPLE_SIZE_SMALL];
    bsl_severity_t original_severity;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(sal_time_usecs());

    arrakis_stats_utils_float_array_random_samples_fill(unit, samples, SAMPLE_SIZE_SMALL);

    SHR_IF_ERR_EXIT(arrakis_stat_family_init(unit, &stat_family, "example_2d"));

    for (i = 0; i < SAMPLE_SIZE_SMALL; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, &stat_family, "Dimension_2", samples[i]));
    }

    arrakis_stats_utils_set_bsl_severity_for_negative_testing(unit, &original_severity);
    if (_SHR_E_INTERNAL != arrakis_stat_family_commit(unit, &stat_family))
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected to get _SHR_E_INTERNAL error in negative test, but got a different error code")}

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOC_COMMON, original_severity);
    SHR_FUNC_EXIT;
}

static sh_sand_man_t stat_family_example_2d_float_samples = {
    "Test Arrakis Stats Module API with Stat Family example_2d (has Date dimension) using random float samples"
};

static sh_sand_man_t stat_family_example_2d_int_samples = {
    "Test Arrakis Stats Module API with Stat Family example_2d (has Date dimension) using random int samples"
};

static sh_sand_man_t stat_family_example_2d_uint32_samples = {
    "Test Arrakis Stats Module API with Stat Family example_2d (has Date dimension) using random uint32 samples"
};

static sh_sand_man_t stat_family_example_3d_float_samples = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension) using random float samples"
};

static sh_sand_man_t stat_family_example_3d_float_samples_multiple_fill = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension) using random float samples. Using arrakis_stat_family_float_multiple_fill() API"
};

static sh_sand_man_t stat_family_example_3d_int_samples = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension) using random int samples"
};

static sh_sand_man_t stat_family_example_3d_int_samples_multiple_fill = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension) using random int samples. Using arrakis_stat_family_int_multiple_fill() API"
};

static sh_sand_man_t stat_family_example_3d_uint32_samples = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension) using random uint32 samples"
};

static sh_sand_man_t stat_family_example_3d_uint32_samples_multiple_fill = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension) using random uint32 samples. Using arrakis_stat_family_uint32_multiple_fill() API"
};

static sh_sand_man_t stat_family_example_3d_multiple_samples_types = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension) using all acceptable data types (i.e. float, int, uint32)"
};

static sh_sand_man_t stat_family_example_3d_mix_fill_and_multiple_fill = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension). Using float values filled with arrakis_stat_family_float_fill() and arrakis_stat_family_float_multiple_fill()"
};

static sh_sand_man_t stat_family_example_3d_mixed_order_fill = {
    "Test Arrakis Stats Module API with Stat Family example_3d (doesn't have Date dimension). Fill samples for dimensions in mixed order."
};

static sh_sand_man_t negative_test_stat_family_name_too_long = {
    "If user enters stat_family_name bigger than ARRAKIS_STATS_FAMILY_NAME_MAX_LEN return an error"
};

static sh_sand_man_t negative_test_exceeding_dimension_nof = {
    "If user enter NOF dimension names via fill API that exceeds ARRAKIS_STATS_DIMENSIONS_MAX_NOF"
};

static sh_sand_man_t negative_test_samples_str_of_a_dimension_too_long = {
    "If user call fill API such that the samples string of the dimension exceeds ARRAKIS_STATS_SAMPLES_STR_MAX_LEN"
};

static sh_sand_man_t negative_test_dimension_name_too_long = {
    "If user enters dimension_name in fill API that is exceeds ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN return an error"
};

static sh_sand_man_t negative_test_stat_family_name_not_exists = {
    "If user calls commit API with stat_family_name given in init API that is not defined in stats_syntax.xml"
};

static sh_sand_man_t negative_test_dimension_nof_does_not_match_stat_family = {
    "If user calls commit API and the number of dimensions in stat_family_t don't match the number of dimensions defined for that Stat Family in stats_syntax.xml"
};

static sh_sand_man_t negative_test_dimension_name_not_exists_in_stat_family = {
    "If user calls commit API and one of the dimension names is not defined for that Stat Family in stats_syntax.xml"
};

static sh_sand_man_t negative_test_stat_family_has_no_data = {
    "If user commit stat_family without entering any data"
};

static sh_sand_man_t negative_test_dimension_do_not_have_the_same_number_of_samples = {
    "If user calls commit API and the number of samples for all dimensions is not the same"
};

static sh_sand_man_t negative_test_stat_family_with_date_dimension_more_than_one_sample = {
    "If user fills more than one sample for dimensions in Stat Family with Date dimension"
};

/**
 * \brief List of arrakis_stats tests
 */
sh_sand_cmd_t dnx_arrakis_stats_positive_tests_cmds[] = {
    {"stat_family_example_2d_float_samples", ctest_dnx_arrakis_stat_family_example_2d_float_samples, NULL, NULL,
     &stat_family_example_2d_float_samples, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_2d_int_samples", ctest_dnx_arrakis_stat_family_example_2d_int_samples, NULL, NULL,
     &stat_family_example_2d_int_samples, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_2d_uint32_samples", ctest_dnx_arrakis_stat_family_example_2d_uint32_samples, NULL, NULL,
     &stat_family_example_2d_uint32_samples, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_float_samples", ctest_dnx_arrakis_stat_family_example_3d_float_samples, NULL, NULL,
     &stat_family_example_3d_float_samples, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_float_samples_multiple_fill",
     ctest_dnx_arrakis_stat_family_example_3d_float_samples_multiple_fill, NULL, NULL,
     &stat_family_example_3d_float_samples_multiple_fill, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_int_samples", ctest_dnx_arrakis_stat_family_example_3d_int_samples, NULL, NULL,
     &stat_family_example_3d_int_samples, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_int_samples_multiple_fill",
     ctest_dnx_arrakis_stat_family_example_3d_int_samples_multiple_fill, NULL, NULL,
     &stat_family_example_3d_int_samples_multiple_fill, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_uint32_samples", ctest_dnx_arrakis_stat_family_example_3d_uint32_samples, NULL, NULL,
     &stat_family_example_3d_uint32_samples, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_uint32_samples_multiple_fill",
     ctest_dnx_arrakis_stat_family_example_3d_uint32_samples_multiple_fill, NULL, NULL,
     &stat_family_example_3d_uint32_samples_multiple_fill, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_multiple_samples_types", ctest_dnx_arrakis_stat_family_example_3d_multiple_samples_types,
     NULL, NULL, &stat_family_example_3d_multiple_samples_types, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_mix_fill_and_multiple_fill",
     ctest_dnx_arrakis_stat_family_example_3d_mix_fill_and_multiple_fill, NULL, NULL,
     &stat_family_example_3d_mix_fill_and_multiple_fill, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_example_3d_mixed_order_fill", ctest_dnx_arrakis_stat_family_example_3d_mixed_order_fill, NULL, NULL,
     &stat_family_example_3d_mixed_order_fill, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}      /* This line should always stay as last one */
};

/**
 * \brief List of arrakis_stats tests
 */
sh_sand_cmd_t dnx_arrakis_stats_negative_tests_cmds[] = {
    {"stat_family_name_too_long", ctest_dnx_arrakis_negative_test_stat_family_name_too_long, NULL, NULL,
     &negative_test_stat_family_name_too_long, NULL, NULL, CTEST_POSTCOMMIT},
    {"exceeding_dimension_nof", ctest_dnx_arrakis_negative_test_exceeding_dimension_nof, NULL, NULL,
     &negative_test_exceeding_dimension_nof, NULL, NULL, CTEST_POSTCOMMIT},
    {"samples_str_of_a_dimension_too_long", ctest_dnx_arrakis_negative_test_samples_str_of_a_dimension_too_long, NULL,
     NULL, &negative_test_samples_str_of_a_dimension_too_long, NULL, NULL, CTEST_POSTCOMMIT},
    {"dimension_name_too_long", ctest_dnx_arrakis_negative_test_dimension_name_too_long, NULL, NULL,
     &negative_test_dimension_name_too_long, NULL, NULL, CTEST_POSTCOMMIT},

    {"stat_family_name_not_exists", ctest_dnx_arrakis_negative_test_stat_family_name_not_exists, NULL, NULL,
     &negative_test_stat_family_name_not_exists, NULL, NULL, CTEST_POSTCOMMIT},

    {"dimension_nof_does_not_match_stat_family",
     ctest_dnx_arrakis_negative_test_dimension_nof_does_not_match_stat_family, NULL, NULL,
     &negative_test_dimension_nof_does_not_match_stat_family, NULL, NULL, CTEST_POSTCOMMIT},

    {"dimension_name_not_exists_in_stat_family",
     ctest_dnx_arrakis_negative_test_dimension_name_not_exists_in_stat_family, NULL, NULL,
     &negative_test_dimension_name_not_exists_in_stat_family, NULL, NULL, CTEST_POSTCOMMIT},

    {"stat_family_has_no_data", ctest_dnx_arrakis_negative_test_stat_family_has_no_data, NULL, NULL,
     &negative_test_stat_family_has_no_data, NULL, NULL, CTEST_POSTCOMMIT},
    {"dimension_do_not_have_the_same_number_of_samples",
     ctest_dnx_arrakis_negative_test_dimension_do_not_have_the_same_number_of_samples, NULL, NULL,
     &negative_test_dimension_do_not_have_the_same_number_of_samples, NULL, NULL, CTEST_POSTCOMMIT},
    {"stat_family_with_date_dimension_more_than_one_sample",
     ctest_dnx_arrakis_negative_test_stat_family_with_date_dimension_more_than_one_sample, NULL, NULL,
     &negative_test_stat_family_with_date_dimension_more_than_one_sample, NULL, NULL, CTEST_POSTCOMMIT},

    {NULL}      /* This line should always stay as last one */
};

/**
 * \brief List of access tests
 */
sh_sand_cmd_t dnx_arrakis_stats_cmds[] = {
    /*
     * NAME | Leaf action | Junction Array Pointer
     */
    {"positive_tests", NULL, dnx_arrakis_stats_positive_tests_cmds},
    {"negative_tests", NULL, dnx_arrakis_stats_negative_tests_cmds},
    {NULL}      /* This line should always stay as last one */
};
