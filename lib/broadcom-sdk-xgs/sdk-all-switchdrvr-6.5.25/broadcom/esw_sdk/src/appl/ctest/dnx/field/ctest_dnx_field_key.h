/** \file diag_dnx_field_key.h
 *
 * 'key' operations (for key allocation, construction and ffc allocation)
 * testing procedures for DNX.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef CTEST_DNX_FIELD_KEY_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_KEY_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/field.h>
#include <bcm/types.h>
#include <bcm/error.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/*
 * Identifying numbers for the various 'key' ctests
 * These are the various values available for 'test_type' input.
 */
#define CTEST_DNX_FIELD_KEY_TEST_TYPE_KEY                       1
#define CTEST_DNX_FIELD_KEY_TEST_TYPE_FFC                       2
#define CTEST_DNX_FIELD_KEY_TEST_TYPE_FFC_PER_MULTIPLE_PROGRAMS 3
#define CTEST_DNX_FIELD_KEY_TEST_TYPE_BIT_RANGE                 4
#define CTEST_DNX_FIELD_KEY_TEST_TYPE_SHARED_FFC                5

#define DNX_DIAG_KEY_CONTEXT_NOF (2)
/*
 * Context to use on 'ctest field key type=bit-range ...' when no context is specified
 * on the command line
 */
#define CTEST_DNX_FIELD_KEY_DEFAULT_CONTEXT_FOR_BIT_RANGE       4
/*
 * Number of parameter-sets to reserve for this
 * test (i.e., number of allocations and deallocations in one group that composes
 * one sub-test).
 */
#define CTEST_DNX_FIELD_KEY_NUM_ALLOCATIONS_IN_KEY_TEST         4
/*
 * }
 */
extern sh_sand_man_t Sh_dnx_field_key_test_man;
/**
 * \brief
 *   Options list for 'key' ctest (shell) command
 * \see
 *   appl_dnx_key_starter() in diag_dnx_field_key.c
 */
extern sh_sand_option_t Dnx_field_key_test_options[];

/**
 * \brief
 *   Options list for 'key' tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_key_starter() in diag_dnx_field_key.c
 */
extern sh_sand_invoke_t Sh_dnx_field_key_tests[];

/**
 * \brief - run 'ctest field key' sequence in diag shell
 */
shr_error_e sh_dnx_field_key_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
/*
 * }
 */

#endif /* CTEST_DNX_FIELD_KEY_H_INCLUDED */
