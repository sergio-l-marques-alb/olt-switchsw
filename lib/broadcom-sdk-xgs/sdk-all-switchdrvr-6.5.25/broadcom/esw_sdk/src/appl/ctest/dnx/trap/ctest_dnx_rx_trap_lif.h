/*
 * ctest_dnx_rx_trap_lif.h
 *
 *  Created on: Jan 5, 2018
 *      Author: dp889757
 */

/*
 * ctest_dnx_rx_trap_lifantic.h
 *
 *  Created on: Dec 21, 2017
 *      Author: dp889757
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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

#ifndef SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_LIF_H_INCLUDED
#define SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_LIF_H_INCLUDED

#define CTEST_DNX_RX_TRAPS_LIF_TEST_TYPE_POSITIVE           "pos"
#define CTEST_DNX_RX_TRAPS_LIF_TEST_TYPE_NEGATIVE           "neg"
#define CTEST_DNX_RX_TRAPS_LIF_TEST_TYPE_EXHAUSTIVE         "exh"

/**
 * \brief
 * LIF trap test details
 */
extern sh_sand_man_t sh_dnx_rx_trap_lif_man;
/**
 * \brief
 *   Options list for 'lif' shell command
 */
extern sh_sand_option_t sh_dnx_rx_trap_lif_options[];
/**
 * \brief
 *   List of tests for LIF Traps shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
extern sh_sand_invoke_t sh_dnx_rx_trap_lif_tests[];

/**
 * \brief - run LIF test sequence in the BCM shell
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
shr_error_e sh_dnx_rx_trap_lif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* SRC_APPL_CTEST_DNX_RX_CTEST_DNX_RX_TRAPS_LIF_H_INCLUDED */
