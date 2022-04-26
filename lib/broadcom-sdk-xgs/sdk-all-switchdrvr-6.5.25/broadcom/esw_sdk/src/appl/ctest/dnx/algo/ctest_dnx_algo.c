/** \file diag_dnx_algo.c
 *
 * alocation manager unit tests.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* INCLUDE FILES:
* {
*/

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>

#include "ctest_dnx_algo_resource.h"
#include "ctest_dnx_algo_template.h"
#include "ctest_dnx_algo_hashing.h"

/**
 * }
 */

sh_sand_cmd_t sh_dnx_algo_test_cmds[] = {
   /*************************************************************************************************************************************
    *   CMD_NAME    *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *               *                           * Level                   *                                *                            *
    *               *                           * CMD                     *                                *                            *
    *************************************************************************************************************************************/
    {"template", dnx_algo_template_unit_test, NULL, dnx_template_manager_test_options, &dnx_template_manager_test_man,
     NULL, NULL, CTEST_PRECOMMIT}
    ,
    {"Consistent_Hashing_Manager", dnx_chm_unit_test, NULL, dnx_chm_test_options, &dnx_chm_test_man,
     NULL, NULL, CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/*
 * }
 */
