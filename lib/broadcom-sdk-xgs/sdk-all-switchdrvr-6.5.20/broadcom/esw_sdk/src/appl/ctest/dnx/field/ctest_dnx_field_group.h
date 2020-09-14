/** \file diag_dnx_field_group.h
 * Database access (incl. TCAM Look-ups) scenarios appl
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_DATABAE_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_DATABAE_H_INCLUDED
/*
* Include files.
* {
*/
/* soc */
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
/** appl */
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

/*
* Defines
* {
*/

/** Base context value */
#define CTEST_DNX_FIELD_GROUP_DEFAULT_CONTEXT                         5

/** Number of preselectors used in the text */
#define CTEST_DNX_FIELD_GROUP_NOF_PRESEL                              1

/** DEFULT is 0, for itteration and index purposes(0 imagine that 0 = 1) */
#define CTEST_DNX_FIELD_GROUP_DEFALUT_NUM_FGS                         0

/** Defines how many field groups we want to test in FG_MULTI scenario */
#define CTEST_DNX_FIELD_GROUP_MULTI_NUM_FGS                           2

/** Hard coded preselector ID */
#define CTEST_DNX_FIELD_GROUP_PRESEL_ID                               10

/** Number of PMF stages that the test is for, the value is used for defining array sizes */
#define CTEST_DNX_FIELD_GROUP_NUM_STAGES                              4

/** Number of preselector qualifiers - array size definition */
#define CTEST_DNX_FIELD_GROUP_NUM_PRESEL_QUALS                        4

/** Preselector qualifier value - legal value that will match any qualifier */
#define CTEST_DNX_FIELD_GROUP_QUAL_VALUE                              0x1

/** Preselector qualifier mask - legal value that will match any qualifier */
#define CTEST_DNX_FIELD_GROUP_QUAL_MASK                               0x3

/*
 * }
 */


extern sh_sand_man_t sh_dnx_field_group_man;
/**
 * \brief
 *   Options list for 'database' shell command
 * \see
 *   appl_dnx_database_starter() in diag_dnx_field_database.c
 */
extern sh_sand_option_t dnx_field_group_options[];
/**
 * \brief
 *   Options list for 'database' tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_database_starter() in diag_dnx_field_database.c
 */
extern sh_sand_invoke_t sh_dnx_field_group_tests[];

/**
 * \brief - run 'database' (TCAM Look-ups) sequence in diag shell
 */
shr_error_e sh_dnx_field_group_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e sh_dnx_clean_up_func(
    int unit,
    int num_fgs,
    dnx_field_stage_e field_stage);
/*
 * }
 */

#endif /* DIAG_DNX_DATABAE_H_INCLUDED */
