/** \file diag_dnx_field_key.h
 *
 * 'key' operations (for key allocation, construction and ffc allocation)
 * testing procedures for DNX.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_KEY_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_KEY_H_INCLUDED
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
/**
 * Number of Field Stages, used as an array size to Last_key_info_tables[] array.
 */
#define DNX_FIELD_KEY_NUM_STAGES              4
/*
 * }
 */
/**
 * \brief
 *   List of shell options for 'key' shell commands (display, ...)
 * \remark
 *   * For now, just passive display of allocated keys and general keys space
 */
extern sh_sand_cmd_t Sh_dnx_field_key_cmds[];

#endif /* DIAG_DNX_KEY_H_INCLUDED */
