
/*! \file diag_dnx_multicast_tests.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_MULTICAST_H_INCLUDED
#  define DIAG_DNX_MULTICAST_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/sand/diag_sand_framework.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/
 
shr_error_e sh_dnx_tm_multicast_cmd(int unit, args_t * args, sh_sand_control_t * sand_control);
extern sh_sand_man_t sh_dnx_tm_multicast_man;
extern sh_sand_option_t sh_dnx_tm_multicast_options[];
extern sh_sand_invoke_t sh_dnx_tm_multicast_tests[];

#endif /*  DIAG_DNX_TX_H_INCLUDED */
