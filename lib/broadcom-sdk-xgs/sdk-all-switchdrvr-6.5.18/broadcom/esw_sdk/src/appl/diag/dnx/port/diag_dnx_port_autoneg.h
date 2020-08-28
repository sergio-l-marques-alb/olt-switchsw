/** \file diag_dnx_port_autoneg.h
 * 
 * Diagnostics for port Auto-Negotiation.
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNX_PORT_AUTONEG_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_PORT_AUTONEG_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

 /*
  * LOCAL DIAG PACK:
  * {
  */
extern sh_sand_option_t sh_dnx_port_autoneg_options[];
extern sh_sand_man_t sh_dnx_port_autoneg_man;

shr_error_e sh_dnx_port_autoneg_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */
#endif /** DIAG_DNX_PORT_AUTONEG_H_INCLUDED*/
