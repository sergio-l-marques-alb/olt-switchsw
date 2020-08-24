/** \file appl_ref_dram_init.h
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_DRAM_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_DRAM_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/**
 * \brief
 * The function is doing registration on dram power down callback function
 * 
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_dram_cb_register(
    int unit);

/**
 * \brief
 * The function is doing un-registration on dram power down callback function
 *
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_dram_cb_unregister(
    int unit);

/**
* \brief
*   This function is doing dram power up.
*   The function should be called part of dram temperature procedure.
*   After wper down was made and all condition fulfil, 
*   user may power up the DRAM and after that call API bcm_switch_dram_init.
* \param [in] unit - The unit number.
*
* \return
*   Error code (as per 'shr_error_e').
* \see
*   shr_error_e
*/
shr_error_e appl_dnx_dram_power_up_handle(
    int unit);
/*
 * }
 */
#endif /* APPL_REF_DRAM_INIT_H_INCLUDED */
