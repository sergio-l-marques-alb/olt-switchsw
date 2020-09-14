/*
 * ! \file ctest_dnx_apt_random.c
 * Contains all of the random APIs used with the API performance test framework
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

/*
 * }
 */

/*
 * GLOBALs
 * {
 */

/** The state for the pseudo-random number generator */
static uint64 dnx_apt_random_state = COMPILER_64_INIT(0, 1);

/** The pointer for holding the unique random numbers array */
static uint32 *dnx_apt_unique_random = NULL;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/** API for seeding the state of the pseudo-random number generator */
void
dnx_apt_random_seed(
    uint64 seed)
{
    COMPILER_64_COPY(dnx_apt_random_state, seed);
}

/** APIs for fast generating of 64b pseudo-random numbers (splitmix64) */
uint64
dnx_apt_random_get_64(
    void)
{
    uint64 number;
    uint64 tmp_64_a;
    uint64 tmp_64_b;

    /** number = (dnx_l3_performance_random_state += (0x9E3779B97F4A7C15)); */
    COMPILER_64_SET(number, 0x9E3779B9, 0x7F4A7C15);
    COMPILER_64_ADD_64(dnx_apt_random_state, number);
    COMPILER_64_COPY(number, dnx_apt_random_state);

    /** number = (number ^ (number >> 30)) * (0xBF58476D1CE4E5B9); */

    /** tmp_64_a = (number ^ (number >> 30)); */
    COMPILER_64_COPY(tmp_64_a, number);
    COMPILER_64_SHR(tmp_64_a, 30);
    COMPILER_64_XOR(tmp_64_a, number);
    /*
     * number = tmp_64_a * 0xBF58476D1CE4E5B9;
     */
    COMPILER_64_COPY(tmp_64_b, tmp_64_a);
    COMPILER_64_UMUL_32(tmp_64_a, 0xBF58476D);
    COMPILER_64_UMUL_32(tmp_64_b, 0x1CE4E5B9);
    COMPILER_64_SHL(tmp_64_a, 32);
    COMPILER_64_ADD_64(tmp_64_a, tmp_64_b);
    COMPILER_64_COPY(number, tmp_64_a);

    /** number = (number ^ (number >> 27)) * (0x94D049BB133111EB); */

    /** tmp_64_a = (number ^ (number >> 27)); */
    COMPILER_64_COPY(tmp_64_a, number);
    COMPILER_64_SHR(tmp_64_a, 27);
    COMPILER_64_XOR(tmp_64_a, number);
    /** number = tmp_64_a * 0x94D049BB133111EB; */
    COMPILER_64_COPY(tmp_64_b, tmp_64_a);
    COMPILER_64_UMUL_32(tmp_64_a, 0x94D049BB);
    COMPILER_64_UMUL_32(tmp_64_b, 0x133111EB);
    COMPILER_64_SHL(tmp_64_a, 32);
    COMPILER_64_ADD_64(tmp_64_a, tmp_64_b);
    COMPILER_64_COPY(number, tmp_64_a);

    /** number = (number ^ (number >> 31); */
    COMPILER_64_COPY(tmp_64_a, number);
    COMPILER_64_SHR(tmp_64_a, 31);
    COMPILER_64_XOR(tmp_64_a, number);
    COMPILER_64_COPY(number, tmp_64_a);

    return number;
}

/** API for truncating the 64b pseudo-random number to 32b */
uint32
dnx_apt_random_get(
    void)
{
    uint64 number;
    uint32 ret;

    number = dnx_apt_random_get_64();
    ret = COMPILER_64_LO(number);
    return (ret);
}

/*
 * API for generating an array of unique random numbers.
 * The array will contain randomly shuffled numbers in the range from 0 to <numbers - 1>.
 */
shr_error_e
dnx_apt_unique_random_generate(
    int unit,
    uint32 numbers)
{
    int iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(dnx_apt_unique_random, (sizeof(uint32) * numbers), "dnx_apt_unique_random", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);

    for (iter = 0; iter < numbers; iter++)
    {
        dnx_apt_unique_random[iter] = iter;
    }
    for (iter = 0; iter < numbers; iter++)
    {
        int tmp_i = dnx_apt_random_get() % numbers;
        uint32 tmp = dnx_apt_unique_random[iter];
        dnx_apt_unique_random[iter] = dnx_apt_unique_random[tmp_i];
        dnx_apt_unique_random[tmp_i] = tmp;
    }

exit:
    SHR_FUNC_EXIT;
}

/** API for retrieving unique random number from pre-generated array. */
uint32
dnx_apt_unique_random_get(
    int number_index)
{
    return dnx_apt_unique_random[number_index];
}

/** API for freeing the array of unique random numbers. */
void
dnx_apt_unique_random_free(
    void)
{
    SHR_FREE(dnx_apt_unique_random);
}

/*
 * }
 */
