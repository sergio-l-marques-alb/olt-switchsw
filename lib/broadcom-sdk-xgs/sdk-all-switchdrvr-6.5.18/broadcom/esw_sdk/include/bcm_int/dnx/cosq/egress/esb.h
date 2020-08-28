/*
 * \file esb.h
 *
 *  Created on: March 26, 2019
 *      Author: vg889851
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _ESB_H_INCLUDED_
#define _ESB_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>

#define ESB_QUEUE_ID_INVALID -1

/**
 * \brief - Initiallize the Egress Shared Buffers
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_init(
    uint32 unit);

/**
 * \brief - Configure the Egress Shared Buffers for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_port_add(
    uint32 unit,
    bcm_port_t logical_port);

/**
 * \brief - Configure port speed dependent settings of the Egress Shared Buffers for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_port_speed_config_add(
    uint32 unit,
    bcm_port_t logical_port);

/**
 * \brief - Configure TXI logic and IRDY threshold for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 * \param [in] is_add - indication whether port is added (otherwise it's removed)
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_port_txi_config_set(
    uint32 unit,
    bcm_port_t logical_port,
    int is_add);

/**
 * \brief - Clear the configuration of the Egress Shared Buffers for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_port_remove(
    uint32 unit,
    bcm_port_t logical_port);

/**
 * \brief - Function to set an entry in the ESB MUX calendar
 *
 * \param [in] unit -  Unit-ID
 * \param [in] slot - Calendar slot ID
 * \param [in] cal_entry - Calendar entry value
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_cal_entry_set(
    uint32 unit,
    uint32 slot,
    uint32 cal_entry);

/**
 * \brief - Function to enable hardware to load new calendar settings to internal usage
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_cal_load(
    uint32 unit);

/**
 * \brief -
 *   Based on setup of rate of master ports, load interface calendar fot ESB
 *
 * \param [in] unit -
 *   HW Unit-ID
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect input is the 'rate' setup for all master ports (assigned per interface).
 *     See dnx_algo_port_interface_rate_get()
 * \see
 *   * None
 */
shr_error_e dnx_esb_nif_calendar_set(
    int unit);

/**
 * \brief -
 *   configure the initiate credit size for ilkn port.
 * \param [in] unit - HW Unit-ID
 * \param [in] port - logical port
 * \param [in] max_credits - maximum credits allowed to configure
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   None 
 * \see
 *   None
 */
shr_error_e dnx_esb_ilkn_credits_set(
    int unit,
    bcm_port_t port,
    int max_credits);

#endif /* _ESB_H_INCLUDED_ */
