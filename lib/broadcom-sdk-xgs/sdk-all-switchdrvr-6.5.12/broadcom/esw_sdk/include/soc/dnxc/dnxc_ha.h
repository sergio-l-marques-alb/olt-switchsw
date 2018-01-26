/** \file dnxc_ha.h
 * General HA routines.
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_HA_H
/* { */
#define _DNXC_HA_H

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

/*
 * }
 */

/**
 * \brief
 *  Init HA utilities module
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_ha_init(
    int unit);

/**
 * \brief
 *  Deinit HA utilities module
 * \param [in] unit    - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_ha_deinit(
    int unit);

/**
 * \brief
 *  Query if HW access is disabled
 * \param [in] unit    - Device Id
 * \return
 *   int - TRUE if disabled FALSE if enabled.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_is_hw_access_disabled(
    int unit);

/**
 * \brief
 *  Temporarily allow HW writes for current thread even if it's
 *  generally disabled
 * \param [in] unit    - Device Id
 * \return
 *   int - Error ID.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_allow_hw_write_enable(
    int unit);

/**
 * \brief
 *  disallow HW writes for current thread, to be used tho revert
 *  dnxc_allow_hw_write_enable after done writing
 *  
 * \param [in] unit    - Device Id
 * \return
 *   int - Error ID.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_allow_hw_write_disable(
    int unit);

/* } */
#endif /* _DNXC_HA_H */
