/** \file dnx_er_threading.h
 * This module is the Error Recovery manager utility.
 * It is a module that is shared between Error Recovery and State Snapshot managers
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
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
#include <sal/core/sync.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
/*
 * }
 */


#define DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION 20

/**
 * \brief - describes the set of fields used by the err recovery manager
 */
typedef struct dnx_er_threading_d
{
    /*
     * Indicate if the utilities manager has been initialized
     */
    uint8 is_initialized;

    /*
     * Used for synchronization of tmp / perm thread list access.
     */
    sal_mutex_t mtx;

    /*
     * Map transaction id (index) to thread.
     */
    volatile sal_thread_t transaction_to_thread[DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION];

    /*
     * Thread transaction is reentrant, count the number of times it was being registered
     * So it can be freed when 0
     */
    volatile int counter[DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION];

    /*
     * The algorithm allow to exclude a thread from Error Recovery, this counter keeps
     * reference for how many times a thread got excluded, when counter is 0 thread is no longer excluded 
     */
    volatile int tmp_excluded_counter[DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION];

} dnx_er_threading_t;

/**
 * \brief
 *  Initialize the Error Recovery thread manager utilities
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_initialize(
    int unit);

/**
 * \brief
 *  Deinitialize the Error Recovery thread manager utilities
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_deinitialize(
    int unit);

/**
 * \brief
 *  Register the current thread and assign an error recovery thread index to it
 *  if already assigned, increase its refrence counter
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_transaction_register(
    int unit);

/**
 * \brief
 *  Unregister the current thread and free its error recovery thread index
 *  only release if the registration counter is 0
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_transaction_unregister(
    int unit);

/**
 * \brief
 *  get the error recovery thread index assigned to the current thread
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_er_threading_transaction_get(
    int unit);

/**
 * \brief
 *  add the current thread to the threads that are excluded from ER
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_tmp_excluded_add(
    int unit);

/**
 * \brief
 *  remove the current thread from the threads that are excluded from ER
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_tmp_excluded_remove(
    int unit);

/**
 * \brief
 *  query if current thread is excluded from ER
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_er_threading_is_tmp_excluded(
    int unit);

/**
 * \brief
 *  verify that no ER thread ids are acive
 * \param [in] unit - Device Id
 * \return
 *   int - 1:true - there is an id leak; 0:false - everything is fine
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_er_threading_id_leak_verification(
    int unit);

/* } */
#endif /* _DNX_ERR_RECOVERY_MANAGER_UTILS_H */
