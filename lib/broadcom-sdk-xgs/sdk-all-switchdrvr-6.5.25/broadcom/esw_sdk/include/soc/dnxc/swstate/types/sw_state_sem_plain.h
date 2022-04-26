/** \file sw_state_sem_plain.h
 * This module contains the plain implementation of the basic sw state sem functions
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_SEM_PLAIN_H_
/* { */
#define _DNXC_SW_STATE_SEM_PLAIN_H_

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <soc/error.h>
#include <soc/dnxc/swstate/types/sw_state_sem.h>
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
 * \param [out] ptr_sem - Result sem
 * \param [in] desc - Semaphore description
 * \param [in] is_binary - Semaphore is binary
 * \param [in] initial_count - Semaphore initial count
 * \param [in] flags - Semaphore flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_sem_create(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    int is_binary,
    int initial_count,
    char *desc,
    uint32 flags);

/**
 * \brief
 *  Allocates a chuck of memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] ptr_sem - Semaphore to be destroyed
 * \param [in] flags - Semaphore flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_sem_destroy(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    uint32 flags);

/**
 * \brief
 *  Allocates a chuck of memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] ptr_sem - Pointer to swstate sem
 * \param [in] usec - Semaphore usecs
 * \param [in] flags - Semaphore flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_sem_take(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    int usec,
    uint32 flags);

/**
 * \brief
 *  Allocates a chuck of memory in the sw state
 * \param [in] unit - Device Id
 * \param [in] module_id - module id
 * \param [in] ptr_sem - Pointer to swstate sem
 * \param [in] flags - Semaphore flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_sem_give(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    uint32 flags);

/* } */
#endif /* _DNXC_SW_STATE_SEM_PLAIN_H_ */
