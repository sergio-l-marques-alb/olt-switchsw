/** \file dnx_er_threading.h
 * This module is the Error Recovery manager utility.
 * It is a module that is shared between Error Recovery and State Snapshot managers
 */
/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/dnxc/dnxc_rollback_journal_utils.h>
/*
 * }
 */

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED

/*
 * Assume that the maximum number of temporary excluded threads
 */
#define DNX_ERR_RECOVERY_TMP_EXCLUDED_EXTERNAL_THREADS_NOF 20
/*
 * Assume that the maximum number of permanently excluded periodic event threads
 */
#define DNX_ERR_RECOVERY_INTERNAL_PERIODIC_EVENT_THREAD_NOF 20

typedef enum
{
    DNX_ERR_RECOVERY_INTERNAL_THREAD_DPC,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_IPROC_EVENT,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_COUNTER,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_CRPS_EVIC,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_LINKSCAN,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_PACKET_DMA,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_SBUS_DMA,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_IPOLL,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_RX,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_COMMON_RX,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_TX,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_PHY_PRBSSTAT,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_L2_FLUSH_TRAVERSE,
    DNX_ERR_RECOVERY_INTERNAL_THREAD_NOF
} dnx_err_recovery_thread_internal_ids_e;

/**
 * \brief - holds Errror Recovery state per thread
 */
typedef struct dnx_err_recovery_thread_state_d
{
    /*
     * identifier for state kept by thread
     */
    sal_thread_t tid;
    /*
     * Indicates if thread currently has disabled ER support.
     * Can be nested.
     */
    uint32 disabled_counter;
} dnx_err_recovery_thread_state_t;

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
     * Current journaling thread. Set only if there is an active rollback journal transaction.
     * Comparison journal is locked to the configuration thread, since internal feature only.
     */
    dnx_err_recovery_thread_state_t journaling_thread;

    /*
     * List of all internal threads except for the configuration thread.
     */
    dnx_err_recovery_thread_state_t internal_threads[DNX_ERR_RECOVERY_INTERNAL_THREAD_NOF];

    /*
     * Number of registered periodic event threads
     */
    uint32 internal_periodic_thread_count;

    /*
     * Periodic event handler threads, count is dynamic
     */
        dnx_err_recovery_thread_state_t
        internal_periodic_event_threads[DNX_ERR_RECOVERY_INTERNAL_PERIODIC_EVENT_THREAD_NOF];

    /*
     * Number of registered external threads
     */
    uint32 excluded_external_thread_count;

    /*
     * List of external/customer threads
     */
    dnx_err_recovery_thread_state_t excluded_external_threads[DNX_ERR_RECOVERY_TMP_EXCLUDED_EXTERNAL_THREADS_NOF];

    /*
     * Main thread, set during init, since it can be changed by testing framework.
     * Set only if the feature has been initialized.
     * Utilized for validation purposes and comparison journal.
     */
    dnx_err_recovery_thread_state_t main_thread;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    /*
     * skips threading validations
     * Used for testing purposes only.
     */
    uint8 skip_validation;

    /*
     * Thread that initialized Error Recovery feature.
     * Set only if the feature has been initialized.
     */
    dnx_err_recovery_thread_state_t config_thread;
#endif                          /* DNX_ERR_RECOVERY_VALIDATION */

} dnx_er_threading_t;

/**
 * \brief
 *  Register periodic event handler thread
 * \param [in] unit - Device Id
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_threading_perm_excluded_periodic_event_thread_add(
    int unit);

/**
 * \brief
 *  Register current thread as non-journaling thread.
 * \param [in] unit - Device Id
 * \param [in] id - Thread identifier
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_threading_perm_excluded_thread_add(
    int unit,
    dnx_err_recovery_thread_internal_ids_e id);

/**
 * \brief
 *  Register current thread for all units
 * \param [in] id - Thread identifier
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_threading_perm_excluded_threads_add_all_units(
    dnx_err_recovery_thread_internal_ids_e id);

/**
 * \brief
 *  Unregister periodic event handler thread
 * \param [in] unit - Device Id
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_threading_perm_excluded_periodic_event_thread_remove(
    int unit);

/**
 * \brief
 *  Unregister current thread from non-journaling thread list.
 * \param [in] unit - Device Id
 * \param [in] id - Thread identifier
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_threading_perm_excluded_thread_remove(
    int unit,
    dnx_err_recovery_thread_internal_ids_e id);

/**
 * \brief
 *  Unregister current thread for all units
 * \param [in] id - Thread identifier
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_threading_perm_excluded_threads_remove_all_units(
    dnx_err_recovery_thread_internal_ids_e id);

/**
 * \brief
 *  Register current thread in the temporary excluded list
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_threading_tmp_excluded_add(
    int unit);

/**
 * \brief
 *  Unregister current thread in the temporary excluded list
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_er_threading_tmp_excluded_remove(
    int unit);

/**
 * \brief
 *  Check if the current thread has been registered as a temporary or permanently excluded non-journaling thread.
 * \param [in] unit - Device Id
 * \return
 *   uint8 - if thread was registered as non-journaling.
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_er_threading_is_excluded_thread(
    int unit);

/**
 * \brief
 *  Check if the current thread is the main thread
 * \param [in] unit - Device Id
 * \return
 *   uint8 - if current thread is the configuration thread.
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_er_threading_is_main_thread(
    int unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
/*
 * \brief
 *  Temporary skip threading violation checks.
 * \param [in] unit - Device Id
 * \param [in] is_on - Indicates if beginning or end of suspension.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32 dnx_er_threading_test_skip_validation(
    int unit,
    uint8 is_on);
#endif /* DNX_ERR_RECOVERY_VALIDATION */

/**
 * \brief
 *  Check if the current thread is the journaling thread.
 * \param [in] unit - Device Id
 * \return
 *   uint8 - if thread was registered as non-journaling.
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_er_threading_is_journaling_thread(
    int unit);

/**
 * \brief
 *  Sets the current thread as the journaling thread.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_journaling_thread_set(
    int unit);

/**
 * \brief
 *  Clears the current journaling thread, sets to invalid.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_journaling_thread_clear(
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
shr_error_e dnx_er_threading_initialize(
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
shr_error_e dnx_er_threading_deinitialize(
    int unit);

/**
 * \brief
 *  Verify that no transaction exists on parallel threads
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_er_threading_no_parallel_transactions_verify(
    int unit);

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_PERIODIC_EVENT_THREAD_ADD(_unit)\
do {                                                                    \
    dnx_er_threading_perm_excluded_periodic_event_thread_add(_unit);    \
} while(0)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_PERIODIC_EVENT_THREAD_REMOVE(_unit)\
do {                                                                       \
    dnx_er_threading_perm_excluded_periodic_event_thread_remove(_unit);    \
} while(0)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(_unit, _thread_id)\
do {                                                                 \
    dnx_er_threading_perm_excluded_thread_add(_unit, _thread_id);    \
} while(0)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(_unit, _thread_id)\
do {                                                                    \
    dnx_er_threading_perm_excluded_thread_remove(_unit, _thread_id);    \
} while(0)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREADS_ADD_ALL_UNITS(_thread_id)\
    dnx_er_threading_perm_excluded_threads_add_all_units(_thread_id)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREADS_REMOVE_ALL_UNITS(_thread_id)\
    dnx_er_threading_perm_excluded_threads_remove_all_units(_thread_id)

/*
 * \brief - exit if the current thread is no the main thread.
 */
#define DNX_ER_THREADING_EXIT_IF_NOT_MAIN_THREAD(_unit)\
do{                                                    \
    if (!dnx_er_threading_is_main_thread(_unit)) {     \
        SHR_EXIT();                                    \
    }                                                  \
}while(0)

/*
 * \brief - exit if current thread is not the journaling thread.
 */
#define DNX_ER_THREADING_EXIT_IF_NOT_JOURNALING_THREAD(_unit)\
do{                                                          \
    if (!dnx_er_threading_is_journaling_thread(_unit)) {     \
        SHR_EXIT();                                          \
    }                                                        \
}while(0)

/*
 * \brief - exit if current thread has been registered as temporary
 *  or permanently exlucded non-journaling thread.
 */
#define DNX_ER_THREADING_EXIT_IF_IS_EXCLUDED_THREAD(_unit)\
do{                                                       \
    if (dnx_er_threading_is_excluded_thread(_unit)) {     \
        SHR_EXIT();                                       \
    }                                                     \
}while(0)

#else /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */
#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_PERIODIC_EVENT_THREAD_ADD(_unit)
#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_PERIODIC_EVENT_THREAD_REMOVE(_unit)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(_unit, _thread_id)
#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREADS_ADD_ALL_UNITS(_thread_id)

#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(_unit, _thread_id)
#define DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREADS_REMOVE_ALL_UNITS(_thread_id)

#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

/* } */
#endif /* _DNX_ERR_RECOVERY_MANAGER_UTILS_H */
