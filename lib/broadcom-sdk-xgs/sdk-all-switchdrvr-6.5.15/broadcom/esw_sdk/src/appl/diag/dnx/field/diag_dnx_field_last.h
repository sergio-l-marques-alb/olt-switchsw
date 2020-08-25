/** \file diag_dnx_field_last.h
 *
 * Diagnostics procedures, for DNX, for 'last' (last packet) operations.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_LAST_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_LAST_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include "diag_dnx_field_utils.h"
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

typedef struct
{
    /** Array to store the KEY IDs, used by FGs. */
    dbal_enum_value_field_field_key_e key_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX];
    /** Array to store the values of the used KEYs, per FG. */
    uint32 key_values[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX][DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    /** Stores the Entry, which was hit, per FG. */
    uint32 entry_id;
} diag_dnx_field_last_fg_info_t;

/**
 * \brief
 *   List of shell options for 'last' shell commands (list)
 * \remark
 */
extern sh_sand_cmd_t Sh_dnx_field_last_cmds[];

#endif /* DIAG_DNX_LAST_H_INCLUDED */
