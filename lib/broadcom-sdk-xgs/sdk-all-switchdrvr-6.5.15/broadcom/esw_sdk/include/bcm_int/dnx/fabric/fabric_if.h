/*
 * ! \file bcm_int/dnx/fabric/fabric_if.h
 * Reserved.$ 
 */

#ifndef _DNX_FABRIC_IF_H_INCLUDED_
/*
 * { 
 */
#define _DNX_FABRIC_IF_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief
 *   Return wether a port is the first activated link in the FSRD.
 * \param [in] unit -
 *   The unit number.
 * \param [in] logical_port -
 *   Logical port.
 * \param [out] is_first_link -
 *   Is the port is the first link in the FSRD.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   This function can be called only for fabric ports.
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_port_first_link_in_fsrd_get(
    int unit,
    soc_port_t logical_port,
    int *is_first_link);

/**
 * \brief - 
 *   Power up fabric ports.
 *   The function adds all given ports to fabric ports bitmap, and power them
 *   up (probe them).
 *  
 * \param [in] unit - Unit-ID 
 * \param [in] pbmp - Bitmap of fabric ports to power up.
 * \param [out] okay_pbmp - Bitmap of fabric ports that were powered up.
 * \return
 *   See \ref shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_if_port_probe(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * okay_pbmp);

/**
 * \brief - 
 *   Power down fabric ports.
 *   The function removes all given ports from fabric ports bitmap, and power them
 *   down (detach them).
 *  
 * \param [in] unit - Unit-ID 
 * \param [in] pbmp - Bitmap of fabric ports to power down.
 * \param [out] detached - Bitmap of fabric ports that were powered down.
 * \return
 *   See \ref shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_if_port_detach(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * detached);

/**
 * \brief - 
 *   Set shaper on fabric link.
 *  
 * \param [in] unit - 
 *   The unit-ID 
 * \param [in] port -
 *   The port id to set shaper upon.
 * \param [in] pps -
 *   Packets Per Second limit.
 * \param [in] burst -
 *   Burst limit.
 * \return
 *   See \ref shr_error_e 
 * \remark
 *   * Used for connecting the device to a repeater.
 * \see
 *   * None
 */
shr_error_e bcm_dnx_fabric_if_link_rate_egress_pps_set(
    int unit,
    bcm_port_t port,
    uint32 pps,
    uint32 burst);

/**
 * \brief - 
 *   Get shaper of fabric link.
 *  
 * \param [in] unit - 
 *   The unit-ID 
 * \param [in] port -
 *   The port id to get its shaper.
 * \param [out] pps -
 *   Packets Per Second limit.
 *   0 means shaper is disabled.
 * \param [out] burst -
 *   Burst limit.
 *   0 means shaper is disabled.
 * \return
 *   See \ref shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e bcm_dnx_fabric_if_link_rate_egress_pps_get(
    int unit,
    bcm_port_t port,
    uint32 *pps,
    uint32 *burst);

#endif /*_DNX_FABRIC_IF_H_INCLUDED_*/
