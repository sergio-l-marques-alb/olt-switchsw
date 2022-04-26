/*! \file diag_dnx_nif_lane_map.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_NIF_LANE_MAP_H_INCLUDED
#define DIAG_DNX_NIF_LANE_MAP_H_INCLUDED

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

extern sh_sand_man_t sh_dnx_nif_lane_map_man;
extern sh_sand_man_t sh_dnx_nif_ilkn_lane_map_man;

extern sh_sand_option_t sh_dnx_nif_lane_map_options[];
extern sh_sand_option_t sh_dnx_nif_ilkn_lane_map_options[];

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - Dump the NIF mapped lane info
 */
shr_error_e sh_dnx_nif_lane_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Dump the ILKN lane map info
 */
shr_error_e sh_dnx_nif_ilkn_lane_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_NIF_LANE_MAP_H_INCLUDED */
