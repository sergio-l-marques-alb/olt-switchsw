/** \file dnx_err_recovery_manager_common.h
 * This module is a common module between
 * the Error Recovery manager and
 * the State Snapshot manager.
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_ERR_RECOVERY_COMMON_MANAGER_H
/* { */
#define _DNX_ERR_RECOVERY_COMMON_MANAGER_H

/*
 * Include files
 * {
 */
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

    /*
     * number of nested dbal access regions.
     * low level counter used to validate hw access
     * and check jurisdiction of swstate journal.
     */
    uint32 dbal_access_counter;

    /*
     * represents the Error Recovery and State Snapshot managers
     */
    dnx_err_recovery_common_manager_t managers[DNX_ROLLBACK_JOURNAL_TYPE_NOF];

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
 *  Check if error recovery is enabled.
 *  Thread check must not taken under consideration and must be done internally.
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
 *  Check if state rollback or comparison manager are on.
 *  Thread check must not taken under consideration and must be done internally.
 * \param [in] unit - Device Id
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
    dnx_rollback_journal_type_e mngr_id);

/**
 * \brief
 *  Check an error recovery or state snapshot manager "is_on" flag.
 * \param [in] unit - Device Id
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
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

/**
 * \brief
 *  Check if a set of flags are on.
 * \param [in] unit - Device Id
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
    dnx_rollback_journal_type_e mngr_id,
    uint32 mask);

/**
 * \brief
 *  Set an error recovery manager flag.
 * \param [in] unit - Device Id
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
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

/**
 * \brief
 *  Clears an error recovery manager flag.
 * \param [in] unit - Device Id
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
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

/**
 * \brief
 *  Check if the dbal access counter is greater than zero.
 *  Indicates if currently DBAL operation is considered to be in progress.
 * \param [in] unit - Unit id
 * \return
 *   uint32 - Value of dbal access counter
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_err_recovery_common_is_dbal_access_region(
    int unit);

/**
 * \brief
 *  Increment the dbal access region counter.
 *  Assumes that journals are on.
 * \param [in] unit - Unit id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_common_dbal_access_region_start(
    int unit);

/**
 * \brief
 *  Decrement the dbal access region counter.
 *  Assumes that journals are on.
 * \param [in] unit - Unit id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_err_recovery_common_dbal_access_region_end(
    int unit);

/* } */
#endif /*_DNX_ERR_RECOVERY_COMMON_MANAGER_H*/
