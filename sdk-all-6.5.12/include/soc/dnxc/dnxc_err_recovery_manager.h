/** \file dnxc_err_recovery_manager.h
 * This module is the Error Recovery manager.
 * It is the interface to the Error Recovery feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_ERR_RECOVERY_MANAGER_H
/* { */
#define _DNXC_ERR_RECOVERY_MANAGER_H

/*
 * Include files
 * {
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>

/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define DNXC_ERR_RECOVERY_TRANSACTION_START(unit) \
    do { \
        dnxc_err_recovery_api_counter_inc(unit); \
    } while(0)

#define DNXC_ERR_RECOVERY_TRANSACTION_END(unit) \
    do { \
        dnxc_err_recovery_api_counter_dec(unit); \
    } while(0)

/*
 * }
 */

/**
 * \brief - defines the set of fields used by the err recovery manager
 */
typedef struct err_recovery_manager_d
{
    uint8 is_started;
    uint8 is_suppressed;
    uint32 api_counter;
    uint32 start_api;
} err_recovery_manager_t;

/**
 * \brief
 *  Increases API counter
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_err_recovery_api_counter_inc(
    int unit);

/**
 * \brief
 *  Decreases API counter
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_err_recovery_api_counter_dec(
    int unit);

/**
 * \brief
 *  Decreases API counter
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_err_recovery_start(
    int unit);

/**
 * \brief
 *  Perform commit operations on roll-back journals for the current transation
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_err_recovery_commit(
    int unit);

/**
 * \brief
 *  Perform abort operations on roll-back journals for the current transaction
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_err_recovery_abort(
    int unit);

/**
 * \brief
 *  Check if error recovery feature is turned on
 * \param [in] unit    - Device Id
 * \return
 *   uint8 - boolean value indicating if journaling is on
 * \remark
 *   * must be called from journaling thread
 * \see
 *   * None
 */
uint8 dnxc_err_recovery_is_on(
    int unit);

/**
 * \brief
 *  Temprary turn on or off error recovery feature
 * \param [in] unit    - Device Id
 * \param [in] is_on   - Boolean on/off value
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_err_recovery_tmp_allow(
    int unit,
    uint8 is_on);

/**
 * \brief
 *  Temporary errer recovery start function.
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_err_recovery_test_support_start(
    int unit);

/**
 * \brief
 *  Temporary errer recovery test support roll back function.
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_err_recovery_test_support_roll_back(
    int unit);

/* } */
#endif /* _DNXC_ERR_RECOVERY_MANAGER_H */
