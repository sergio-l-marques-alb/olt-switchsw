/** \file diag_dnx_field_exem_learn_flush.h
 *
 * 'EXEM' operations (for group add and entry add) procedures for DNX.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef CTEST_DNX_EXEM_LEARN_FLUSH_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_EXEM_LEARN_FLUSH_H_INCLUDED
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
extern sh_sand_man_t Sh_dnx_field_exem_learn_flush_man;
/**
 * \brief
 *   Options list for 'exem_learn_flush' tests to run on regression, precommit, ...
 * \see
 *   sh_dnx_exem_learn_flush_cmd() in ctest_dnx_field_exem_learn_flush.c
 */
extern sh_sand_invoke_t Sh_dnx_field_exem_learn_flush_tests[];
/**
 * \brief
 *   Options list for 'exem_learn_flush' shell command
 * \see
 *   sh_dnx_exem_learn_flush_cmd() in ctest_dnx_field_exem_learn_flush.c
 */
extern sh_sand_option_t Sh_dnx_field_exem_learn_flush_options[];

/**
 * \brief - run 'exem_learn_flush' sequence in diag shell
 */
shr_error_e sh_dnx_field_exem_learn_flush_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* CTEST_DNX_EXEM_LEARN_FLUSH_H_INCLUDED */
