/** \file dnx_err_recovery_manager_utils.c
 * This module is the Error Recovery manager.
 * It is the interface to the Error Recovery feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/drv.h>
#include <sal/core/dpc.h>

#include <soc/dnx/dnx_err_recovery_manager_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * \brief - take counter mutex
 */
#define DNX_ERR_RECOVERY_UTILS_CNT_MTX_TAKE(_unit)                                           \
do {                                                                                         \
    if(dnx_err_recovery_manager_utils[_unit].is_initialized) {                               \
        sal_mutex_take(dnx_err_recovery_manager_utils[_unit].counter_mtx, sal_mutex_FOREVER);\
    }                                                                                        \
} while(0)

/*
 * \brief - give counter mutex
 */
#define DNX_ERR_RECOVERY_UTILS_CNT_MTX_GIVE(_unit)                        \
do {                                                                      \
    if(dnx_err_recovery_manager_utils[_unit].is_initialized) {            \
        sal_mutex_give(dnx_err_recovery_manager_utils[_unit].counter_mtx);\
    }                                                                     \
} while(0)

dnx_err_recovery_manager_utils_t dnx_err_recovery_manager_utils[SOC_MAX_NUM_DEVICES] = { {0}
};

/**
 * see .h file for description
 */
void
dnx_err_recovery_utils_excluded_thread_add(
    int unit)
{
    uint32 idx = 0;
    sal_thread_t tid = sal_thread_self();

    DNX_ERR_RECOVERY_UTILS_CNT_MTX_TAKE(unit);


    /*
     * insert the entry at the end of the array and increment the excluded thread count
     */
    idx = dnx_err_recovery_manager_utils[unit].count;
    dnx_err_recovery_manager_utils[unit].excluded_threads[idx] = tid;

    dnx_err_recovery_manager_utils[unit].count++;

    DNX_ERR_RECOVERY_UTILS_CNT_MTX_GIVE(unit);
}

/**
 * see .h file for description
 */
void
dnx_err_recovery_utils_excluded_threads_add_all_units(
    )
{
    int unit = 0;

    for (unit = 0; unit < SOC_MAX_NUM_DEVICES; unit++)
    {
        dnx_err_recovery_utils_excluded_thread_add(unit);
    }
}

/**
 * see .h file for description
 */
void
dnx_err_recovery_utils_excluded_thread_remove(
    int unit)
{
    uint8 found = 0;
    uint32 idx = 0;
    uint32 last = 0;
    sal_thread_t tid = sal_thread_self();

    DNX_ERR_RECOVERY_UTILS_CNT_MTX_TAKE(unit);


    for (idx = 0; idx < dnx_err_recovery_manager_utils[unit].count; idx++)
    {
        if (tid == dnx_err_recovery_manager_utils[unit].excluded_threads[idx])
        {
            found = TRUE;
            break;
        }
    }


    if (found)
    {
        /*
         * replace the current thread with the last one and decrement the total count of threads.
         */
        last = dnx_err_recovery_manager_utils[unit].count - 1;
        dnx_err_recovery_manager_utils[unit].excluded_threads[idx] =
            dnx_err_recovery_manager_utils[unit].excluded_threads[last];
        dnx_err_recovery_manager_utils[unit].excluded_threads[last] = SAL_THREAD_ERROR;
        dnx_err_recovery_manager_utils[unit].count--;
    }

    DNX_ERR_RECOVERY_UTILS_CNT_MTX_GIVE(unit);
}

/**
 * see .h file for description
 */
void
dnx_err_recovery_utils_excluded_threads_remove_all_units(
    )
{
    int unit;

    for (unit = 0; unit < SOC_MAX_NUM_DEVICES; unit++)
    {
        dnx_err_recovery_utils_excluded_thread_remove(unit);
    }
}

/**
 * see .h file for description
 */
uint8
dnx_err_recovery_utils_is_excluded_thread(
    int unit)
{
    uint8 is_thread_excluded = FALSE;
    uint32 idx = 0;
    sal_thread_t tid = sal_thread_self();

    for (idx = 0; idx < dnx_err_recovery_manager_utils[unit].count; idx++)
    {
        if (tid == dnx_err_recovery_manager_utils[unit].excluded_threads[idx])
        {
            is_thread_excluded = TRUE;
            break;
        }
    }


    return is_thread_excluded;
}

/**
 * \brief - callback registered to dpc thread.
 *  Function used to exclude the thread. The call should be done only once at device init.
 *  During denit->init function is ignored by dpc since request is being done while dpc is still disabled for this owner.
 */
static void
dnx_err_recovery_utils_dpc_thread_cb(
    void *owner_ptr,
    void *dont_care_0,
    void *dont_care_1,
    void *dont_care_2,
    void *dont_care_3)
{
    dnx_err_recovery_utils_excluded_thread_add(PTR_TO_INT(owner_ptr));
}

/**
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_utils_initialize(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * save the configuration thread for the current unit
     */
    dnx_err_recovery_manager_utils[unit].config_thread = sal_thread_main_get();

    /*
     * register a callback for the dpc thread that will be executed immediatedly.
     * the callback will register the thread as "excluded".
     */
    sal_dpc(dnx_err_recovery_utils_dpc_thread_cb, INT_TO_PTR(unit), 0, 0, 0, 0);

    dnx_err_recovery_manager_utils[unit].counter_mtx = sal_mutex_create("er thread cnt mtx");

    dnx_err_recovery_manager_utils[unit].is_initialized = TRUE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file for description
 */
shr_error_e
dnx_err_recovery_utils_deinitialize(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_mutex_destroy(dnx_err_recovery_manager_utils[unit].counter_mtx);

    dnx_err_recovery_manager_utils[unit].is_initialized = FALSE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
