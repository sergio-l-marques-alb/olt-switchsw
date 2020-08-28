/** \file diag_dnx_field_vw.h
 *
 * Virtual wire operations, such as name to Id mapping
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_VW_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_VW_H_INCLUDED
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
extern sh_sand_man_t Sh_dnx_field_vw_man;
/**
 * \brief
 *   Options list for 'VW' tests to run on regression, precommit, ...
 * \see
 *   sh_dnx_vw_cmd() in ctest_dnx_field_vw.c
 */
extern sh_sand_invoke_t Sh_dnx_field_vw_tests[];
/**
 * \brief
 *   Options list for 'vw' shell command
 * \see
 *   sh_dnx_vw_cmd() in ctest_dnx_field_vw.c
 */
extern sh_sand_option_t Sh_dnx_field_vw_options[];

/**
 * \brief - run 'vw' sequence in diag shell
 */
shr_error_e sh_dnx_field_vw_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* DIAG_DNX_VW_H_INCLUDED */
