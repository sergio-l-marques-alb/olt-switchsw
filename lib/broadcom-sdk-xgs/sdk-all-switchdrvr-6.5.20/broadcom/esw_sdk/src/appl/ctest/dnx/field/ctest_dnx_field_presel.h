/** \file diag_dnx_field_presel.h
 * PMF preselection (Context Selection) application procedures for DNX
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_PRESEL_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_PRESEL_H_INCLUDED
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
/** shared */
#include <include/shared/error.h>
/*
 * }
 */
/**
 * \brief
 *   Options list for 'field presel' shell command
 * \see
 *   appl_dnx_field_presel() in diag_dnx_field_presel.c
 */
extern sh_sand_option_t dnx_field_presel_options[];

extern sh_sand_man_t sh_dnx_field_presel_man;
/**
 * \brief
 *   Options list for 'field presel' C-tests
 * \see
 *   appl_dnx_field_presel() in diag_dnx_field_presel.c
 */
extern sh_sand_invoke_t sh_dnx_field_presel_tests[];

/**
 * \brief -run PMF Preselection sequence in diag shell
 */
shr_error_e sh_dnx_field_presel_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* DIAG_DNX_PRESEL_H_INCLUDED */
