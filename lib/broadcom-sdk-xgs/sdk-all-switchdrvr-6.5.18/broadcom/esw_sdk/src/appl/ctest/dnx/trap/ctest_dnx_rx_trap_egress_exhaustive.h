/*
 * ctest_dnx_rx_trap_egress_exhaustive.h
 *
 *  Created on: April 25, 2018
 *      Author: sm888907
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
* Include files.
* {
*/
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*
 * }
 */

#ifndef SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_EGRESS_EXHAUSTIVE_H_INCLDUED
#define SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_EGRESS_EXHAUSTIVE_H_INCLDUED

/**
 * \brief
 * EGRESS_EXHAUSTIVE trap test details
 */
extern sh_sand_man_t sh_dnx_rx_trap_egress_exhaustive_man;
/**
 * \brief
 *   List of tests for EGRESS_EXHAUSTIVE shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
extern sh_sand_invoke_t sh_dnx_rx_trap_egress_exhaustive_tests[];

/**
 * \brief - run EGRESS_EXHAUSTIVE semantic test sequence in the BCM shell
 *
 * \param [in] unit         - Number of hardware unit used
 * \param [in] args         - Pointer to args_t struct, not used
 * \param [in] sand_control - Pointer to comamnd control structure used for parameter delivery
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e sh_dnx_rx_trap_egress_exhaustive_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_EGRESS_EXHAUSTIVE_H_INCLDUED */
