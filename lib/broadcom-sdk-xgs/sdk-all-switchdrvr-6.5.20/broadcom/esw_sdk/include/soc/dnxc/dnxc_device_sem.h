/** \file dnxc_device_sem.h
 * device semaphore implementation
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_DEVICE_SEM_H
/* { */
#define _DNXC_DEVICE_SEM_H

/**
 * \brief
 *   Typedef of procedure used to create a device semaphore.
 * \par DIRECT INPUT
 *   \param [in]  unit - unit ID
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 */
typedef int (
    *dnxc_device_semaphore_create) (
    int unit);

/**
 * \brief
 *   Typedef of procedure used to destroy a device semaphore.
 * \par DIRECT INPUT
 *   \param [in]  unit - unit ID
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 */
typedef int (
    *dnxc_device_semaphore_destroy) (
    int unit);

/**
 * \brief
 *   Typedef of procedure used to take a device semaphore.
 * \par DIRECT INPUT
 *   \param [in]  unit - unit ID
 *   \param [in]  usec - timeout in useconds
 *   \param [in]  dbg_str - a string for debug purpose
 *          (typically the name of the API that aquired the
 *          lock)
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 */
typedef int (
    *dnxc_device_semaphore_take) (
    int unit,
    int usec,
    char *dbg_str);

/**
 * \brief
 *   Typedef of procedure used to give up on a device semaphore.
 * \par DIRECT INPUT
 *   \param [in]  unit - unit ID
 *   \param [in]  dbg_str - a string for debug purpose
 *          (typically the name of the API that aquired the
 *          lock)
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 */
typedef int (
    *dnxc_device_semaphore_give) (
    int unit,
    char *dbg_str);

/**
 * \brief - register the lock API
 * 
 * \par DIRECT_INPUT:
 *   \param [in]  unit - unit ID
 *   \param [in]  take - semaphore take function
 *   \param [in]  give - semaphore give function
 *   \param [in]  create - semaphore create function
 *   \param [in]  destroy - semaphore destroy function
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remarks
 *   * to be called at init.
 * \see
 *   * None
 */
int dnxc_device_sem_register_cbs(
    int unit,
    dnxc_device_semaphore_take take,
    dnxc_device_semaphore_give give,
    dnxc_device_semaphore_create create,
    dnxc_device_semaphore_destroy destroy);

#endif /* _DNXC_DEVICE_SEM_H */
