
/** \file diag_dnx_field_compare.h
 * Compare feature testing.
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
#include <bcm_int/dnx/field/field_map.h>
/*
 * }
 */

#ifndef DIAG_DNX_FIELD_COMPARE_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_COMPARE_H_INCLUDED

/**
 * \brief
 *   Options list for 'CoMPare' shell command
 */
extern sh_sand_option_t Sh_dnx_field_compare_options[];

/**
 * \brief
 *   man for 'CoMPare' command
 */
extern sh_sand_man_t Sh_dnx_field_compare_man;
/**
 * \brief
 *   Options list for 'CoMPare' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_compare_tests[];

/**
 * \brief - run CoMPare init sequence in diag shell
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
shr_error_e sh_dnx_field_compare_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FIELD_COMPARE_H_INCLUDED */
