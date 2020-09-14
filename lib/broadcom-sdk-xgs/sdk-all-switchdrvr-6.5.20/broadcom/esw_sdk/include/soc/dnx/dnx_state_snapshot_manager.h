/** \file dnx_state_snapshot_manager.h
 * This module is the System State Snapshot manager.
 * It is the interface to System Snapshot feature.
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
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
#include <soc/dnx/recovery/rollback_journal_utils.h>
/*
 * }
 */

typedef struct dnx_state_snapshot_manager_d
{
    uint8 is_initialized;
    uint32 is_on_counter;
    uint32 entry_counter;
    uint32 is_suppressed_counter;
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
 * \param [in] type - rollback journal to be toggled.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_comparison_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type);

/**
 * \brief
 *   Toggle comparison journal logger.
 *   Each entry being inserted in the journal will be printed on a continuous basis.
 * \param [in] unit   - Device Id
 * \param [in] type - rollback journal to be toggled.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_state_comparison_journal_logger_state_change(
    int unit,
    dnx_rollback_journal_subtype_e type);

#define DNX_STATE_COMPARISON_SUPPRESS(_unit, _is_suppressed)\
    dnx_state_comparison_suppress(_unit, _is_suppressed)

#define DNX_STATE_COMPARISON_START(_unit)\
    SHR_IF_ERR_EXIT(dnx_state_comparison_start(_unit))

#define DNX_STATE_COMPARISON_END_AND_COMPARE(_unit)\
    SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(_unit))

#define DNX_STATE_COMPARISON_END_AND_COMPARE_NEGATIVE(_unit)\
do {                                                                                                              \
    if (_SHR_E_NONE == dnx_state_comparison_end_and_compare(_unit))                                               \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected system state difference for negative test, but none was found\n");\
    }                                                                                                             \
} while(0)

#endif /* _DNX_SYSTEM_SNAPSHOT_MANAGER_H */
