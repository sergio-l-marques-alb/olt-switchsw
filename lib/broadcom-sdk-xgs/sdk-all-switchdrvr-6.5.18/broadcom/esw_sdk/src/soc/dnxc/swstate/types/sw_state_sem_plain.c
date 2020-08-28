/** \file sw_state_sem_plain.c
 * This module contains the plain implementation of the basic sw state sem functions
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
#include <soc/dnxc/swstate/types/sw_state_sem_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/dnxc_verify.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * see .h file for description
 */
int dnxc_sw_state_sem_create(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, int is_binary, int initial_count, char *desc, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

#ifdef DNX_SW_STATE_VERIFICATIONS
    if(DNXC_VERIFY_ALLOWED_GET(unit))
    {
        if(NULL == ptr_sem) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state sempahore ERROR: sempahore ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }
#endif

    ptr_sem->sem = sal_sem_create(desc, is_binary, initial_count);
    ptr_sem->is_binary = is_binary;
    ptr_sem->initial_count = initial_count;

    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_add(unit, (uint8**)ptr_sem, NULL, DNXC_SW_STATE_DLL_SEM));

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_sem_destroy(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free_by_ptr_location(unit, (uint8**)ptr_sem));

    if(NULL == ptr_sem) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state sempahore ERROR: sempahore ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_sem_destroy(ptr_sem->sem);

    ptr_sem->sem = NULL;

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_sem_take(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, int usec, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sal_sem_take(ptr_sem->sem, usec);

    
#if 0
#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_take(unit, module_id, ptr_sem, usec));
    }
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */
#endif

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_sem_give(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sal_sem_give(ptr_sem->sem);

    
#if 0
#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_give(unit, module_id, ptr_sem));
    }
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */
#endif

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
