/*
 * ctest_dnx_rx_trap_erpp.h
 *
 *  Created on: Jan 31, 2018
 *      Author: dp889757
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

#ifndef SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_ERPP_H_INCLDUED
#define SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_ERPP_H_INCLDUED

#define CTEST_DNX_RX_TRAP_ERPP_TEST_TYPE_POSITIVE      "pos"
#define CTEST_DNX_RX_TRAP_ERPP_TEST_TYPE_NEGATIVE      "neg"

#define CTEST_DNX_RX_TRAP_ERPP_TEST_MODE_ACTIONS       "act"
#define CTEST_DNX_RX_TRAP_ERPP_TEST_MODE_APPLICATION   "appl"

/**
 * \brief
 * ERPP traps test details
 */
extern sh_sand_man_t sh_dnx_rx_trap_erpp_man;
/**
 * \brief
 *   List of tests for ERPP shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
extern sh_sand_invoke_t sh_dnx_rx_trap_erpp_tests[];

extern sh_sand_option_t sh_dnx_rx_trap_erpp_options[];

/**
 * \brief - Fill erpp trap configuration
 *
 * \param [in] unit         - Number of hardware unit used
 * \param [in] trap_config_p - trap configuration
 *\return
 *   \retval None
 *\see
 *   None
 */
void ctest_dnx_rx_trap_erpp_fill(
    int unit,
    bcm_rx_trap_config_t * trap_config_p);

/**
 * \brief - run ERPP semantic test sequence in the BCM shell
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
shr_error_e sh_dnx_rx_trap_erpp_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_ERPP_H_INCLDUED */
