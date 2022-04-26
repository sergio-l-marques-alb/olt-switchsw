/** \file sw_state_cb.c
 *
 * A callback implementation that is used through sw_state
 * interface
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
/* ---------- */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/error.h>
#include <shared/bsl.h>

/**
* See description in .h file
*/
int
sw_state_cb_db_print(
    int unit,
    sw_state_cb_t * cb)
{

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    cli_out("Printing Sw State Callback:\n");

    cli_out("%s \n", cb->function_name);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
