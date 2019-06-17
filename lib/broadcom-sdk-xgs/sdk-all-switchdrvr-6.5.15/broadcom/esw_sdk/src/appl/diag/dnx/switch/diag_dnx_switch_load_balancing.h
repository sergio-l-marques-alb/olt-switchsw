/**
 * \file diag_dnx_switch_load_balancing.h
 *
 * Diagnostics definitions, for DNX, for 'switch/loiad_balancing' operations.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_SWITCH_LOAD_BALANCING_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_SWITCH_LOAD_BALANCING_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/*
 * Options
 * {
 */
/**
 * \brief
 *   Keyword for the load balancing info to be displayed.
 *   Currently, only one level is allowed: "all"
 */
#define DNX_DIAG_SWITCH_LB_OPTION_DISPLAY_LEVEL           "level"
/*
 * }
 */
/**
 * \brief
 *   List of shell options for 'load_balancing' shell commands (display, ...)
 * \remark
 *   * For now, just passive display.
 */
extern sh_sand_cmd_t Sh_dnx_diag_switch_load_balancing_cmds[];
/*
 * }
 */
/*
 * }
 */
#endif /* DIAG_DNX_ACTION_H_INCLUDED */
