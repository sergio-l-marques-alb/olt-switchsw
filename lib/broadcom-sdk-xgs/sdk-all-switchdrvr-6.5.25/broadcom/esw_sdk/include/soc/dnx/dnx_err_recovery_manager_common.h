/** \file dnx_err_recovery_manager_common.h
 * This module is a common module between
 * the Error Recovery manager and
 * the State Snapshot manager.
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_ERR_RECOVERY_COMMON_MANAGER_H
/* { */
#define _DNX_ERR_RECOVERY_COMMON_MANAGER_H

/*
 * Include files
 * {
 */
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
/*
 * }
 */

#define DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED          SAL_BIT(0)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION       SAL_BIT(1)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED       SAL_BIT(2)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED      SAL_BIT(3)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_ALL                 (SAL_BIT(4) - 1)

/**
 * \brief - defines the set of fields used by the err recovery and snapshot managers
 */
typedef struct dnx_err_recovery_common_manager_d
{
    /*
     * bitmap representing the multiple conditions in the Error Recovery manager.
     */
    uint32 bitmap;
} dnx_err_recovery_common_manager_t;

typedef struct dnx_err_recovery_common_root_d
{
    /*
     * to enable/disable the feature. value 1 is enable.
     */
    uint32 is_enable;

    
        dnx_err_recovery_common_manager_t
        managers[DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION][DNX_ROLLBACK_JOURNAL_TYPE_NOF];

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
    /*
     * runtime flag indicating if validation has been enabled
     */
    uint8 validation_enabled;
#endif

} dnx_err_recovery_common_root_t;

/**
 * \brief
 *  error recovery common init
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_common_init(
    int unit);

/**
 * \brief
 *  Enable/disable error recovery
 * \param [in] unit    - Device Id
 * \param [in] is_enable   - to enable/disable
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_common_enable_disable(
    int unit,
    uint32 is_enable);

/**
 * \brief
 *  Enable/disable error recovery validation
 * \param [in] unit    - Device Id
 * \param [in] validation_enabled   - to enable/disable
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_common_validation_enable_disable_set(
    int unit,
    uint32 validation_enabled);

/**
 * \brief
 *  Check if error recovery is enabled.
 * \param [in] unit - Device Id
 * \return
 *   uint8 - indicates if error recovery is enabled
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_err_recovery_common_is_enabled(
    int unit);

/**
 * \brief
 *  Check if error recovery validation is enabled.
 * \param [in] unit - Device Id
 * \return
 *   uint8 - indicates if error recovery validation is enabled
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_err_recovery_common_is_validation_enabled(
    int unit);

/**
 * \brief
 *  Check if state rollback or comparison manager are on.
 * \param [in] unit - Unit id
 * \param [in] er_thread_id - The thread id in error recovery (not os) scope
 * \param [in] mngr_id - State rollbak or compare managers to check
 * \return
 *   uint8 - indicates if manager is on
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_err_recovery_common_is_on(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id);

/**
 * \brief
 *  Check an error recovery or state snapshot manager "is_on" flag.
 * \param [in] unit - Unit id
 * \param [in] er_thread_id - The thread id in error recovery (not os) scope
 * \param [in] mngr_id - State rollbak or compare managers to check
 * \param [in] flag - Flag to be get.
 * \return
 *   uint8 - boolean value indicating if journaling is on
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_err_recovery_common_flag_is_on(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

/**
 * \brief
 *  Check if a set of flags are on.
 * \param [in] unit - Unit id
 * \param [in] er_thread_id - The thread id in error recovery (not os) scope
 * \param [in] mngr_id - State rollbak or compare managers to check
 * \param [in] mask - Flag to be masked.
 * \return
 *   uint8 - boolean value indicating if journaling is on
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_err_recovery_common_flag_mask_is_on(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id,
    uint32 mask);

/**
 * \brief
 *  Set an error recovery manager flag.
 * \param [in] unit - Unit id
 * \param [in] er_thread_id - The thread id in error recovery (not os) scope
 * \param [in] mngr_id - State rollbak or compare managers to check
 * \param [in] flag - Flag to be set.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_common_flag_set(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

/**
 * \brief
 *  Clears an error recovery manager flag.
 * \param [in] unit - Unit id
 * \param [in] er_thread_id - The thread id in error recovery (not os) scope
 * \param [in] mngr_id - State rollbak or compare managers to check
 * \param [in] flag - Flag to be cleared.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_common_flag_clear(
    int unit,
    int er_thread_id,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

/* } */
#endif /*_DNX_ERR_RECOVERY_COMMON_MANAGER_H*/
