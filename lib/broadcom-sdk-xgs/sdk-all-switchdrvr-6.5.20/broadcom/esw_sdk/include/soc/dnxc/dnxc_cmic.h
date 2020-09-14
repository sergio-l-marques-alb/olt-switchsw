/*
 * $Id: dnxc_cmic.h,v 1.0 Broadcom SDK $
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXC IPROC
 */

#ifndef _SOC_DNXC_CMIC_DRV_H_
#define _SOC_DNXC_CMIC_DRV_H_

#include <soc/defs.h>
#include <soc/dnxc/dnxc_defs.h>
#ifdef BCM_DNX_SUPPORT

#endif /* BCM_DNX_SUPPORT */

/*
 * Function:
 *      soc_dnxc_cmic_device_hard_reset
 * Purpose:
 *      if proper reset action is given, resets device and makes sure device is out of reset.
 *      reset flags are: SOC_DNXC_RESET_ACTION_IN_RESET, SOC_DNXC_RESET_ACTION_INOUT_RESET
 * Parameters:
 *      unit            - Device Number
 *      reset_action    - Action to perform
 * Returns:
 *      _SHR_E_XXX
 */
int soc_dnxc_cmic_device_hard_reset(
    int unit,
    int reset_action);

/*
 * Function:
 *      soc_dnxc_cmic_sbus_timeout_set
 * Purpose:
 *      setting the timeout value of the sbus
 * Parameters:
 *      unit            - Device Number
 *      core_freq_khz   - the freq of the core in khz
 *      schan_timeout   - time in microseconds
 * Returns:
 *      _SHR_E_XXX
 */
int soc_dnxc_cmic_sbus_timeout_set(
    int unit,
    uint32 core_freq_khz,
    int schan_timeout);
int soc_dnxc_cmic_mdio_config(
    int unit,
    int dividend,
    int divisor,
    int delay);

/*
 * Function:
 *      soc_dnxc_cmic_mdio_set
 * Purpose:
 *      setting the CMIC MDIO parameters
 * Parameters:
 *      unit            - Device Number
 * Returns:
 *      _SHR_E_XXX
 */
int soc_dnxc_cmic_mdio_set(
    int unit);

/**
 * \brief
 *   Sets the MDIO configuration parameters based on DNX/DNXF data
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
int soc_dnxc_mdio_config_set(
    int unit);

/**
 * \brief
 *   Get the MDIO configuration parameters based on DNX/DNXF data
 * \param [in] unit -
 *   The unit number.
 * \param [out] int_divisor -
 *   Divisor for internal access
 * \param [out] ext_divisor -
 *   Divisor for external access
 * \param [out] int_delay -
 *   Delaty for internal access
 *    * \param [out] ext_delay -
 *   Delaty for external access
 * \return
 *   VOID
 * \remark
 *   * None.
 * \see
 *   * None.
 */
void soc_dnxc_data_mdio_config_get(
    int unit,
    int *int_divisor,
    int *ext_divisor,
    int *int_delay,
    int *ext_delay);
#endif /* _SOC_DNXC_CMIC_DRV_H_ */
