/*
 * $Id:$
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXC Data Property
 */

#ifndef _SOC_DNXC_DATA_PROPERTY_H_
#define _SOC_DNXC_DATA_PROPERTY_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jer2) and DNXF family only!" 
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/port.h>
/*
 * }
 */

/**
 * \brief
 *   Common code for reading fabric firmware load method.
 * 
 * \param [in] unit - Unit #
 * \param [out] fabric_fw_load_method - data already set to default and should be modifeid according to property
 * 
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnxc_data_property_fabric_fw_load_method_read(
    int unit,
    uint32 *fabric_fw_load_method);

/**
 * \brief
 *   Common code for reading fabric firmware load verify.
 * 
 * \param [in] unit - Unit #
 * \param [out] fabric_fw_load_method - data already set to default and should be modifeid according to property
 * 
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnxc_data_property_fabric_fw_load_verify_read(
    int unit,
    uint32 *fabric_fw_load_verify);

/**
 * \brief
 *   Common code for reading TX FIR parameters.
 *
 * \param [in] unit - Unit #
 * \param [in] port - logical port #
 * \param [out] serdes_tx_taps - TX FIR parameters. Will be modified according to property, if property was set.
 *                               If property wasn't set - will return tx->main with invalid value, to signal that property wasn't set.
 *
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnxc_data_property_port_static_add_serdes_tx_taps_read(
    int unit,
    int port,
    bcm_port_phy_tx_t* tx);

#endif /*_SOC_DNXC_DATA_PROPERTY_H_*/


