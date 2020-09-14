/** \file diag_dnx_field_efes.h
 *
 * Diagnostics procedures, for DNX, for 'efes' operations.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_EFES_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_EFES_H_INCLUDED
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
 *   List of shell options for 'efes' shell commands (list, info, context)
 */
extern sh_sand_cmd_t Sh_dnx_field_efes_cmds[];

#endif /* DIAG_DNX_EFES_H_INCLUDED */
