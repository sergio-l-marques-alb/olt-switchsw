/** \file diag_dnx_field_entry.h
 * Semantic test for field entries in the TCAM
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_ENTRY_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_ENTRY_H_INCLUDED
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
extern sh_sand_man_t sh_dnx_field_entry_man;
/**
 * \brief
 *   Options list for 'database' shell command
 * \see
 *   appl_dnx_database_starter() in diag_dnx_field_database.c
 */
extern sh_sand_option_t dnx_field_entry_options[];
/**
 * \brief
 *   Options list for 'database' tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_database_starter() in diag_dnx_field_database.c
 */
extern sh_sand_invoke_t sh_dnx_field_entry_tests[];

/**
 * \brief - run 'database' (TCAM Look-ups) sequence in diag shell
 */
shr_error_e sh_dnx_field_entry_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
/*
 * }
 */

#endif /* DIAG_DNX_ENTRY_H_INCLUDED */
