/** \file diag_dnx_field_qual.h
 * Qualifiers and actions testing scenarios appl
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_QUAL_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_QUAL_H_INCLUDED
/*
* Include files.
* {
*/
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/*
 * }
 */
extern sh_sand_man_t sh_dnx_field_qual_man;
/**
 * \brief
 *   Options list for 'qual' shell command 
 */
extern sh_sand_option_t dnx_field_qual_options[];
/**
 * \brief
 *   Options list for 'qual' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t sh_dnx_field_qual_tests[];

/**
 * \brief - run 'qual' (TCAM Look-ups) sequence in diag shell
 */
extern shr_error_e sh_dnx_field_qual_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* DIAG_DNX_QUAL_H_INCLUDED */
