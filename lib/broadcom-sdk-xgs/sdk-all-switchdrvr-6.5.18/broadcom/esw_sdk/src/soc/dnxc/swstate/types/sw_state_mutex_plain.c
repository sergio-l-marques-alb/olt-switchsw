/** \file sw_state_mutex_plain.c
 * This module contains the plain implementation of the basic sw state mutex functions
 */
/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_mutex_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/dnxc_verify.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * see .h file for description
 */
int dnxc_sw_state_mutex_create(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, char *desc, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

#ifdef DNX_SW_STATE_VERIFICATIONS
    if(DNXC_VERIFY_ALLOWED_GET(unit))
    {
        if(NULL == ptr_mtx) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state mutex ERROR: mutex ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }
#endif

    ptr_mtx->mtx = sal_mutex_create(desc);

    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_add(unit, (uint8**)ptr_mtx, NULL, DNXC_SW_STATE_DLL_MUTEX));

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_mutex_destroy(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free_by_ptr_location(unit, (uint8**)ptr_mtx));
    
    if(NULL == ptr_mtx) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state mutex ERROR: mutex ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_mutex_destroy(ptr_mtx->mtx);

    ptr_mtx->mtx = NULL;

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_mutex_take(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, int usec, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sal_mutex_take(ptr_mtx->mtx, usec);

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_take(unit, module_id, ptr_mtx, usec));
    }
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_mutex_give(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sal_mutex_give(ptr_mtx->mtx);

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_give(unit, module_id, ptr_mtx));
    }
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
