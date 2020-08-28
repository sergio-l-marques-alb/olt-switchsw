/** \file sw_state_sem_wb.c
 * This module contains the wb implementation of the basic sw state semaphore functions
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

#if defined(BCM_WARM_BOOT_SUPPORT)

#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#include <shared/bsl.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/dnxc_verify.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

extern dnxc_sw_state_data_block_header_t           *dnxc_sw_state_data_block_header[SOC_MAX_NUM_DEVICES];

#ifdef DNX_SW_STATE_VERIFICATIONS
#ifdef DNXC_VERIFICATION
/*
 * Internal sw state sem create verify.
 */
STATIC int dnxc_sw_state_sem_create_wb_internal_verify(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, char *desc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ptr_sem != NULL);

    /* return error if not initialized */
    if (dnxc_sw_state_data_block_header_is_init(unit) == FALSE) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INIT,"unit:%d trying to create sw state semaphore w/o initializing the SW state\n%s%s", unit, EMPTY, EMPTY);
    }

    /* return error if semaphore is already created */
    if (ptr_sem->sem != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,"unit:%d sw state semaphore can only be created once \n%s%s", unit, EMPTY, EMPTY);
    }

    /* return error if ptr_sem is not in range*/
    if (dnxc_sw_state_data_block_is_in_range(unit, (uint8*)ptr_sem) == FALSE) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to create a semaphore and save it outside of the sw_state scope\n%s%s", unit, EMPTY, EMPTY);
    }

    /* if not enough space exit with error
     * semaphore doesn't consume memory in the data block except of the pointers stack allocation element
     */
    if (dnxc_sw_state_data_block_is_enough_space(unit, 0) == FALSE) {
        unsigned long size_left ;
        unsigned long total_buffer_size ;

        size_left = (unsigned long)(dnxc_sw_state_data_block_header[unit]->size_left) ;
        total_buffer_size = (unsigned long)(dnxc_sw_state_data_block_header[unit]->total_buffer_size) ;
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY,"dnxc_sw_state_sem_create_wb: not enough memory for allocation. total %lu size_left %lu alloc_size %lu\n",
                                                    total_buffer_size, size_left, (unsigned long)(sizeof(dnxc_sw_state_alloc_element_t)));
    }

    if (sal_strnlen(desc, SW_STATE_SEM_DESC_LENGTH) > (SW_STATE_SEM_DESC_LENGTH -1)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to create a semaphore with description string larger than SW_STATE_SEM_DESC_LENGTH chars\n%s%s", unit, EMPTY, EMPTY);
    }

    SHR_EXIT();

    DNXC_SW_STATE_FUNC_RETURN;
}
#endif /* DNXC_VERIFICATION */
#endif /* DNX_SW_STATE_VERIFICATIONS */


/*
 * see .h file for description
 */

int dnxc_sw_state_sem_create_wb(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, int is_binary, int initial_count, char *desc, uint32 flags) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    /* create a semaphore and save the semaphore off set in the pointers stack */

#ifdef DNX_SW_STATE_VERIFICATIONS

    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(dnxc_sw_state_sem_create_wb_internal_verify(unit, module_id, ptr_sem, desc)));

#endif /* DNX_SW_STATE_VERIFICATIONS */

    /* create the semaphore */
    ptr_sem->sem = sal_sem_create(desc, is_binary, initial_count);
    ptr_sem->is_binary = is_binary;
    ptr_sem->initial_count = initial_count;
    sal_strncpy(ptr_sem->description, desc, SW_STATE_SEM_DESC_LENGTH-1);

    /* save the semaphore in the pointers stack */
    dnxc_sw_state_data_block_pointers_stack_push(unit, (uint8**)ptr_sem, SW_STATE_ALLOC_ELEMENT_SEM);

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_sem_destroy_wb(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

#ifdef DNX_SW_STATE_VERIFICATIONS
    if(DNXC_VERIFY_ALLOWED_GET(unit))
    {
        if(NULL == ptr_sem->sem) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state semaphore ERROR: semaphore ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }
#endif /* DNX_SW_STATE_VERIFICATIONS */

    /* the free function will be later translated to a sem_destroy call */
    SHR_IF_ERR_EXIT_WITH_LOG(dnxc_sw_state_free_wb(unit, module_id, (uint8**)ptr_sem, flags,"sw_state_sem_destroy"),
        "unit:%d sw_state semaphore destroy failed\n%s%s", unit, EMPTY, EMPTY);

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_sem_take_wb(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, int usec, uint32 flags)
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
int dnxc_sw_state_sem_give_wb(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, uint32 flags)
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

#else

typedef int make_iso_compilers_happy;

#endif /* BCM_WARM_BOOT_SUPPORT */
