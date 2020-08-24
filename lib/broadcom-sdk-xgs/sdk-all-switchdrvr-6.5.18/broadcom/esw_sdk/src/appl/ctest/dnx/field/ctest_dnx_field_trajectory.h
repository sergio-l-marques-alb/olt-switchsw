/** \file diag_dnx_field_trajectory.h
 * Trajectory iPMF use-case for DNX.
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

#ifndef DIAG_DNX_FIELD_TRAJECTORY_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_TRAJECTORY_H_INCLUDED

/*
 * }
 */
/**
 * \brief
 *   Options list for 'trajectory' shell command
 */
extern sh_sand_option_t dnx_field_trajectory_options[];

extern sh_sand_man_t sh_dnx_field_trajectory_man;
/**
 * \brief
 *   Options list for 'trajectory' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t sh_dnx_field_trajectory_tests[];

/**
 * \brief - run trajectory init sequence in diag shell 
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
shr_error_e sh_dnx_field_trajectory_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FIELD_TRAJECTORY_H_INCLUDED */
