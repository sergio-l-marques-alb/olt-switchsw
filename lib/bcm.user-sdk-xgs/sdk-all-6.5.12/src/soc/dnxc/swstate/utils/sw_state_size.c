/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: this module implement a size functionality for the SW State infrastructure layer,
 *
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_utils.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX


uint32 dnx_sw_state_info_size_get(dnx_sw_state_diagnostic_info_t *info, const char *layout_str[], uint32 nof_entries, char *node) {
    uint32 size = 0;
    int i;
    for (i = 0; i < nof_entries; i++) {
        if (is_prefix(node, layout_str[i])) {
            size += info[i].size;
        }
    }
    return size;
}

void dnx_sw_state_size_print(uint32 size) {
    if (size < 10 * 1024) {
        cli_out("%u bytes\n", size);
    } else if (size < 10 * 1024 * 1024) {
        cli_out("%u KB\n", size / 1024);
    } else {
        cli_out("%u MB\n", size / (1024 * 1024));
    }
}

#else /* DNX_SW_STATE_DIAGNOSTIC */
typedef int make_iso_compilers_happy;
#endif
