/** \file bcm_int/dnx/stat/mib/mib_stat.h
 * 
 * Internal DNX STAT MIB 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _DNX_MIB_STAT_INCLUDED__
/*
 * { 
 */
#define _DNX_MIB_STAT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * Functions prototypes
 * {
 */

/**
* \brief
*   Get the specified statistics by counter_idx
* \param [in] unit - unit id
* \param [in] port - logic port
* \param [in] type - stat type
* \param [in] counter_idx - counter index
* \param [out] value - counter value
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_stat_get_by_counter_idx(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int counter_idx,
    uint64 *value);

/**
* \brief
*   initialize dnx stat mib module
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mib_init(
    int unit);

/**
* \brief
*   deinitialize dnx mib module
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mib_deinit(
    int unit);

/**
 * \brief - Configure counter for the port addition
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_counter_port_add(
    int unit);

/**
 * \brief - Configure counter for the port removal
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_counter_port_remove(
    int unit);

#endif/*_DNX_mib_MGMT_INCLUDED__*/
