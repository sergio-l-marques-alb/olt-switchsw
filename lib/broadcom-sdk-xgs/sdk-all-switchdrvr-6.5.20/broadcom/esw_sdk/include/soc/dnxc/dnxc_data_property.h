/*
 * $Id:$
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXC Data Property
 */

#ifndef _SOC_DNXC_DATA_PROPERTY_H_
#define _SOC_DNXC_DATA_PROPERTY_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jr2) and DNXF family only!"
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
 *   Common code for reading fabric and nif firmware load method.
 * 
 * \param [in] unit - Unit #
 * \param [out] fw_load_method - data already set to default and should be modified according to property
 * \param [in] suffix - which firmware to load (nif/fabric)
 * 
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_property_fw_load_method_read(
    int unit,
    uint32 *fw_load_method,
    const char *suffix);

/**
 * \brief
 *   Common code for reading fabric and nif firmware crc check.
 * 
 * \param [in] unit - Unit #
 * \param [out] fw_crc_check - data already set to default and should be modified according to property
 * \param [in] suffix - on which firmware to check CRC (nif/fabric)
 * 
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_property_fw_crc_check_read(
    int unit,
    uint32 *fw_crc_check,
    const char *suffix);

/**
 * \brief
 *   Common code for reading fabric and nif firmware load verify.
 *
 * \param [in] unit - Unit #
 * \param [out] fw_load_verify - data already set to default and should be modified according to property
 * \param [in] suffix - which firmware to verify load (nif/fabric)
 *
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_property_fw_load_verify_read(
    int unit,
    uint32 *fw_load_verify,
    const char *suffix);

/**
 * \brief
 *   Common code for reading TX FIR parameters.
 *
 * \param [in] unit - Unit #
 * \param [in] port - logical port #
 * \param [in] propval - the given string to be parsed to tx_taps #
 * \param [out] tx - TX FIR parameters. Will be modified according to property, if property was set.
 *                   If property wasn't set - will return tx->main with invalid value, to signal that property wasn't set.
 *
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxc_data_property_serdes_tx_taps_read(
    int unit,
    int port,
    char *propval,
    bcm_port_phy_tx_t * tx);

#endif /*_SOC_DNXC_DATA_PROPERTY_H_*/
