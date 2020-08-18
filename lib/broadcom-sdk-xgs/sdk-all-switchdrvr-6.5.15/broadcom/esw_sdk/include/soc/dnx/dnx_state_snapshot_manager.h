/** \file dnx_state_snapshot_manager.h
 * This module is the System State Snapshot manager.
 * It is the interface to System Snapshot feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_SYSTEM_SNAPSHOT_MANAGER_H
/* { */
#define _DNX_SYSTEM_SNAPSHOT_MANAGER_H

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

typedef struct dnx_state_snapshot_manager_d
{
    uint8 is_initialized;
    uint32 is_on_counter;
    uint32 entry_counter;
    uint32 is_suppressed_counter;
    uint32 dbal_access_region_counter;
} dnx_state_snapshot_manager_t;

/**
 * \brief
 *  Initialize snapshot manager
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_snapshot_manager_init(
    int unit);

/**
 * \brief
 *  Deinitialize snapshot manager
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_snapshot_manager_deinit(
    int unit);

/**
 * \brief
 *  Start snapshot manager comparison tool
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_comparison_start(
    int unit);

/**
 * \brief
 *  End snapshot manager comparison tool for ctests. Compare the state before and after the end.
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_comparison_end_and_compare(
    int unit);

/**
 * \brief
 *  End snapshot manager comparison tool for ctests. Compare the state before and after the end.
 * \param [in] unit - Device Id
 * \return
 *   uint8 - boolean value indicating if journaling is on
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_state_snapshot_manager_is_on(
    int unit);

/**
 * \brief
 *  Increment the dbal access region counter for comparison journal
 *  Assumes that dbal rollback journal is on.
 * \param [in] unit - Unit id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_comparison_dbal_access_region_inc(
    int unit);

/**
 * \brief
 *  Decrements the dbal access region counter for comparison journal
 *  Assumes that dbal rollback journal is on.
 * \param [in] unit - Unit id
 * \return
 *   int - Error Type
 * \remark
 *   * Used to indicate the begin or end dbal journal access region.
 * \see
 *   * None
 */
shr_error_e dnx_state_comparison_dbal_access_region_dec(
    int unit);

/**
 * \brief
 *  Temprary turn off snapshot comparison feature
 * \param [in] unit    - Device Id
 * \param [in] is_suppressed - Boolean on/off value
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_comparison_suppress(
    int unit,
    uint8 is_suppressed);

/**
 * \brief
 *  Add comparison journal exception for swstate or dbal
 * \param [in] unit - Device Id
 * \param [in] stamp - Associated string stamp
 * \param [in] is_swstate - Indicate if exclusion is relevant for swstate or dbal journal
 */
shr_error_e dnx_state_comparison_exclude_by_stamp(
    int unit,
    char *stamp,
    int is_swstate);

/**
 * \brief
 *  Print contents of comparison swstate and dbal journals
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_comparison_print_journals(
    int unit);

#endif /* _DNX_SYSTEM_SNAPSHOT_MANAGER_H */
