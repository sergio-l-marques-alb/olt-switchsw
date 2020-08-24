/** \file diag_dnx_l3.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_PP_H_INCLUDED
#define DIAG_DNX_PP_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/dbal/dbal.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/
extern sh_sand_man_t sh_dnx_l3_man;
extern sh_sand_cmd_t sh_dnx_l3_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_intf_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_route_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_host_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_egr_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_ecmp_cmds[];

/*************
 * FUNCTIONS *
 *************/

#endif /* DIAG_DNX_PP_H_INCLUDED */
