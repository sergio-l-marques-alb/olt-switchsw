/*! \file diag_dnx_armor.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_ARMOR_H_INCLUDED
#define DIAG_DNX_ARMOR_H_INCLUDED

/*************
 * INCLUDES  *
 *************/

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/
extern sh_sand_man_t sh_dnx_armor_man;
extern sh_sand_cmd_t sh_dnx_armor_cmds[];

/*************
 * FUNCTIONS *
 *************/
shr_error_e diag_armor_image_check(
    int unit,
    rhlist_t * list);

#endif /* DIAG_DNX_ARMOR_H_INCLUDED */
