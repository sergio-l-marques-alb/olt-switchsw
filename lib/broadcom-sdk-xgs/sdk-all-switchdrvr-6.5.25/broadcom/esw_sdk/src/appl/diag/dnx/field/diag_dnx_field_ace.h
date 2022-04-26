/** \file diag_dnx_field_ace.h
 *
 * Diagnostics procedures, for DNX, for 'ACE' operations.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_ACE_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_ACE_H_INCLUDED
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
 * }
 */
/**
 * \brief
 *   List of shell options for 'ace' shell commands (list, info)
 */
extern sh_sand_cmd_t Sh_dnx_field_ace_cmds[];

#endif /* DIAG_DNX_ACE_H_INCLUDED */
