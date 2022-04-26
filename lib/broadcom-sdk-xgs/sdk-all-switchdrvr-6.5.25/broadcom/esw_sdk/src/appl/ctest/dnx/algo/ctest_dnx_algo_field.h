/** \file diag_dnx_algo_field.h
 *
 * 'Action' operations (for payload and FES configuration and allocation) procedures for DNX.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_ALGO_FIELD_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_ALGO_FIELD_H_INCLUDED
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
extern sh_sand_man_t sh_dnx_algo_field_man;
/**
 * \brief
 *   Options list for 'algo' field tests to run on regression, precommit, ...
 * \see
 *   sh_dnx_action_cmd() in ctest_dnx_algo_field.c
 */
extern sh_sand_invoke_t sh_dnx_algo_field_tests[];
/**
 * \brief
 *   Options list for 'algo' field shell command
 * \see
 *   sh_dnx_action_cmd() in ctest_dnx_field_action.c
 */
extern sh_sand_option_t dnx_algo_field_options[];

/**
 * \brief - run 'algo' field sequence in diag shell
 */
shr_error_e sh_dnx_algo_field_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* DIAG_DNX_ACTION_H_INCLUDED */
