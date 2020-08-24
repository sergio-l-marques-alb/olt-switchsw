/*
 * ctest_dnx_rx_trap_prog.h
 *
 *  Created on: Dec 06, 2018
 *      Author: sm888907
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
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

#ifndef SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_PROG_H_INCLUDED
#define SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_PROG_H_INCLUDED

#define CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_TYPE        "type"
#define CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_POSITIVE    "pos"
#define CTEST_DNX_RX_TRAPS_PROG_OPTION_TEST_NEGATIVE    "neg"

/**
 * \brief
 * Programmable trap test details
 */
extern sh_sand_man_t sh_dnx_rx_trap_prog_man;
/**
 * \brief
 *   Options list for 'prog' shell command
 */
extern sh_sand_option_t sh_dnx_rx_trap_prog_options[];
/**
 * \brief
 *   List of tests for Ingress shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
extern sh_sand_invoke_t sh_dnx_rx_trap_prog_tests[];

/**
 * \brief - run programmable traps test sequence in the BCM shell
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
shr_error_e sh_dnx_rx_trap_prog_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_PROG_H_INCLUDED */
