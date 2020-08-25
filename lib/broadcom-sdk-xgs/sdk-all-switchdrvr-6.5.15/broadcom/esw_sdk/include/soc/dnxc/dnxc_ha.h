/** \file dnxc_ha.h
 * General HA routines.
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
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
 * types
 * {
 */

/* If you add entries to this enum you will need to update .c file with some strings
   representing the access name and mutex name */
typedef enum
{
    DNXC_HA_ALLOW_DBAL = 0,
    DNXC_HA_ALLOW_SW_STATE,
    DNXC_HA_ALLOW_SCHAN,
    DNXC_HA_ALLOW_NOF
} dnxc_ha_allow_access_e;

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
 * \param [in] access_type - Access Type
 * \return
 *   int - TRUE if disabled FALSE if enabled.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_ha_is_access_disabled(
    int unit,
    dnxc_ha_allow_access_e access_type);

/**
 * \brief
 *  Temporarily allow HW writes for current thread even if it's
 *  generally disabled
 * \param [in] unit    - Device Id 
 * \param [in] access_type - Access Type
 * \return
 *   int - Error ID.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_ha_tmp_allow_access_enable(
    int unit,
    dnxc_ha_allow_access_e access_type);

/**
 * \brief
 *  disallow HW writes for current thread, to be used tho revert
 *  dnxc_allow_hw_write_enable after done writing
 * \param [in] unit    - Device Id
 * \param [in] access_type - Access Type
 * \return
 *   int - Error ID.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_ha_tmp_allow_access_disable(
    int unit,
    dnxc_ha_allow_access_e access_type);

/* } */
#endif /* _DNXC_HA_H */
