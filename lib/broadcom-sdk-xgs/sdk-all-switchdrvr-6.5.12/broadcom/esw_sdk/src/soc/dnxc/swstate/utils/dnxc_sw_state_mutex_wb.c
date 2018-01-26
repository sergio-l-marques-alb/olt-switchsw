/** \file dnxc_sw_state_mutex_wb.c
 * This module contains the wb implementation of the basic sw state mutex functions
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#include <shared/bsl.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_mutex_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * see .h file for description
 */
int dnxc_sw_state_mutex_create_wb(int unit, uint32 module_id, sal_mutex_t *ptr_mtx, char *desc, uint32 flags)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == ptr_mtx) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state mutex ERROR: mutex ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *ptr_mtx = sal_mutex_create(desc);

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_mutex_destroy_wb(int unit, uint32 module_id, sal_mutex_t *ptr_mtx, uint32 flags)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == ptr_mtx) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state mutex ERROR: mutex ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_mutex_destroy(*ptr_mtx);

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_mutex_take_wb(int unit, uint32 module_id, sal_mutex_t mtx, int usec, uint32 flags)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    sal_mutex_take(mtx, usec);

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_mutex_give_wb(int unit, uint32 module_id, sal_mutex_t mtx, uint32 flags)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    sal_mutex_give(mtx);

    DNX_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
