/** \file dnx_err_recovery_manager_utils.h
 * This module is the Error Recovery manager utility.
 * It is a module that is shared between Error Recovery and State Snapshot managers
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_ERR_RECOVERY_MANAGER_UTILS_H
/* { */
#define _DNX_ERR_RECOVERY_MANAGER_UTILS_H

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

#define DNX_ERR_RECOVERY_MANAGER_EXCLUDED_THREADS_MAX 20

/**
 * \brief - defines the set of fields used by the err recovery manager
 */
typedef struct dnx_err_recovery_manager_utils_d
{
    uint8 is_initialized;
    uint32 count;
    sal_mutex_t counter_mtx;
    sal_thread_t config_thread;
    sal_thread_t excluded_threads[DNX_ERR_RECOVERY_MANAGER_EXCLUDED_THREADS_MAX];
} dnx_err_recovery_manager_utils_t;

/**
 * \brief
 *  Register current thread as non-journaling thread.
 * \param [in] unit - Device Id
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_err_recovery_utils_excluded_thread_add(
    int unit);

/**
 * \brief
 *  Register current thread for all units
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_err_recovery_utils_excluded_threads_add_all_units(
    );

/**
 * \brief
 *  Unregister current thread from non-journaling thread list.
 * \param [in] unit - Device Id
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_err_recovery_utils_excluded_thread_remove(
    int unit);

/**
 * \brief
 *  Unregister current thread for all units
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_err_recovery_utils_excluded_threads_remove_all_units(
    );

/**
 * \brief
 *  Check if the current thread has been registered as a non-journaling thread.
 * \param [in] unit - Device Id
 * \return
 *   uint8 - if thread was registered as non-journaling.
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_err_recovery_utils_is_excluded_thread(
    int unit);

/**
 * \brief
 *  Initialize the Error Recovery utilities
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_utils_initialize(
    int unit);

/**
 * \brief
 *  Deinitialize the Error Recovery utilities
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_utils_deinitialize(
    int unit);

/* } */
#endif /* _DNX_ERR_RECOVERY_MANAGER_UTILS_H */
