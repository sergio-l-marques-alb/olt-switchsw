/** \file sw_state_mutex_plain.h
 * This module contains the plain implementation of the basic sw state mutex functions
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_MUTEX_H_
/* { */
#define _DNXC_SW_STATE_MUTEX_H_

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/sync.h>
/*
 * }
 */

/**
 * \brief
 *  Allocates a chuck of memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [out] mtx - Result mutex
 * \param [in] desc - Mutex description
 * \param [in] flags - Mutex flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_mutex_create(
    int unit,
    uint32 module_id,
    sal_mutex_t * mtx,
    char *desc,
    uint32 flags);

/**
 * \brief
 *  Allocates a chuck of memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] ptr_mtx - Mutex to be destroyed
 * \param [in] flags - Mutex flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_mutex_destroy(
    int unit,
    uint32 module_id,
    sal_mutex_t * ptr_mtx,
    uint32 flags);

/**
 * \brief
 *  Allocates a chuck of memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] mtx - Mutex to take
 * \param [in] usec - Mutex usecs
 * \param [in] flags - Mutex flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_mutex_take(
    int unit,
    uint32 module_id,
    sal_mutex_t mtx,
    int usec,
    uint32 flags);

/**
 * \brief
 *  Allocates a chuck of memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] mtx - Mutex to give
 * \param [in] flags - Mutex flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_mutex_give(
    int unit,
    uint32 module_id,
    sal_mutex_t mtx,
    uint32 flags);

/* } */
#endif /* _DNXC_SW_STATE_MUTEX_H_ */
