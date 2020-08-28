/** \file diag_dnx_field_tcam.h
 * TCAM Tests
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_TCAM_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_TCAM_H_INCLUDED
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
#include <bcm/rx.h>
#include <bcm/error.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/*
 * }
 */
extern sh_sand_man_t sh_dnx_field_tcam_man;
/**
 * \brief
 *   Options list for 'tcam' shell command
 * \see
 *   appl_dnx_tcam_starter() in diag_dnx_field_tcam.c
 */
extern sh_sand_option_t dnx_field_tcam_options[];
/**
 * \brief
 *   Options list for 'tcam' tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_tcam_starter() in diag_dnx_field_tcam.c
 */
extern sh_sand_invoke_t sh_dnx_field_tcam_tests[];

/**
 * \brief - run 'tcam'
 */
shr_error_e sh_dnx_field_tcam_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - run 'BCM' sequence in diag field tcam shell
 */
shr_error_e diag_dnx_field_tcam_base_test(
    int unit);
/*
 * }
 */

#endif /* DIAG_DNX_TCAM_H_INCLUDED */
