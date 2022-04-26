/** \file ctest_dnx_field_hash.h
 * Hash iPMF use-case for DNX.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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

#ifndef CTEST_DNX_FIELD_HASH_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_HASH_H_INCLUDED

/*
 * }
 */
/**
 * \brief
 *   Options list for 'hash' shell command
 */
extern sh_sand_option_t dnx_field_hash_options[];

extern sh_sand_man_t sh_dnx_field_hash_man;
/**
 * \brief
 *   Options list for 'hash' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t sh_dnx_field_hash_tests[];

/**
 * \brief - run hash init sequence in diag shell 
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
shr_error_e sh_dnx_field_hash_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_DNX_FIELD_HASH_H_INCLUDED */
