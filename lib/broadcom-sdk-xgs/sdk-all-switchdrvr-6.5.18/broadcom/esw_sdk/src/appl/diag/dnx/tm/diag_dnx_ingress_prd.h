/*! \file diag_dnx_ingress_prd.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_INGRESS_PRD_H_INCLUDED
#define DIAG_DNX_INGRESS_PRD_H_INCLUDED

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

extern sh_sand_option_t sh_dnx_ingress_port_drop_options[];
extern sh_sand_man_t sh_dnx_ingress_port_drop_man;

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - Dump the port PRD info
 */
shr_error_e sh_dnx_ingress_port_drop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_INGRESS_PRD_H_INCLUDED */
