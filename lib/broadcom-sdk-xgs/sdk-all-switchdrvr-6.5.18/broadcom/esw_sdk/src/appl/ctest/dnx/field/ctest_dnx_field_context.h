/** \file diag_dnx_field_context.h
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
* Include files.
* {
*/
#include <appl/diag/sand/diag_sand_framework.h>
/*
 * }
 */

#ifndef DIAG_DNX_FIELD_CONTEXT_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_CONTEXT_H_INCLUDED

/** Number of PMF stages that we set variables for */
#define CTEST_DNX_FIELD_CONTEXT_NUM_OF_STAGES           4

/** Number of allocation and deallocation cycles we want to run/test */
#define CTEST_DNX_FIELD_NOF_ALLOC_CYCLES                10

/*
 * }
 */
/**
 * \brief
 *   Options list for 'CaScaDe' shell command
 */
extern sh_sand_option_t dnx_field_context_options[];

/**
 * \brief
 *   man for 'CaScaDe' command
 */
extern sh_sand_man_t sh_dnx_field_context_man;
/**
 * \brief
 *   Options list for 'Context' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t sh_dnx_field_context_tests[];

/**
 * \brief - run cascade init sequence in diag shell
 *
 * \param [in] unit - Number of hardware unit used
 *      [in] args - Pointer to args_t struct, not used
 *      [in] sand_control - Pointer to comamnd control structure used for parameter delivery
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e sh_dnx_field_context_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FIELD_CASCADE_H_INCLUDED */
