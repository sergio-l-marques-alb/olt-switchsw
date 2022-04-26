/** \file diag_dnx_field_context_attach.h
 *  Context attach/detach usage example in DNX level.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
* Include files.
* {
*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/field/field_map.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
/*
 * }
 */

#ifndef CTEST_DNX_FIELD_CONTEXT_ATTACH_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_H_INCLUDED

/*
 * }
 */
/**
 * \brief
 *   Options list for 'ConTeXt_Attach' shell command
 */
extern sh_sand_option_t Sh_dnx_field_context_attach_options[];

/**
 * \brief
 *   man for 'ConTeXt_Attach' command
 */
extern sh_sand_man_t Sh_dnx_field_context_attach_man;
/**
 * \brief
 *   Options list for 'ConTeXt_Attach' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_context_attach_tests[];

/**
 * \brief - run ConTeXt_Attach init sequence in diag shell
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
shr_error_e sh_dnx_field_context_attach_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_DNX_FIELD_CONTEXT_ATTACH_H_INCLUDED */
