/** \file ctest_dnx_field_ace.h
 *
 * 'ACE' operations (for group and entry add and for FES configuration) procedures for DNX.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_ACE_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_ACE_H_INCLUDED
/*
* Include files.
* {
*/
/** appl */
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
extern sh_sand_man_t Sh_dnx_field_ace_man;
/**
 * \brief
 *   Options list for 'ace' tests to run on regression, precommit, ...
 * \see
 *   ctest_dnx_ace_starter() in ctest_dnx_field_ace.c
 */
extern sh_sand_invoke_t Sh_dnx_field_ace_tests[];
/**
 * \brief
 *   Options list for 'ace' shell command
 * \see
 *   ctest_dnx_ace_starter() in ctest_dnx_field_ace.c
 */
extern sh_sand_option_t Sh_dnx_field_ace_options[];

/**
 * \brief - run 'ace' sequence in diag shell
 */
shr_error_e sh_dnx_field_ace_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* DIAG_DNX_ACE_H_INCLUDED */
