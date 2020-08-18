/** \file appl_ref_dynamic_port.h
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_DYNAMIC_PORT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_DYNAMIC_PORT_H_INCLUDED

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
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * Typedefs
 * {
 */


/*
 * }
 */

/*
 * DEFINEs
 * {
 */


/*
 * }
 */

/**
 * \brief - Create application for a new port: 
 * 1. Create system port 
 * 2. Create scheduling scheme 
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - Port
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_dynamic_port_create(
    int unit,
    int port);


/**
 * \brief - Destroy application for an existing port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - Port
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * appl_dnx_dynamic_port_create() function
 */
shr_error_e
appl_dnx_dynamic_port_destroy(
    int unit,
    int port);

/**
 * \brief - Set initial port settings per ETH port
 * Set vlan domain and vlan membership-namespace per port
 * Add the port to BCM_VLAN_DEFAULT
 * Enable same interface filter per port
 * Set untagged packet default VLAN per port
 * Enable learning per poert
 * Set port default-LIF to simple bridge
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_dyn_ports_packet_processing_eth_init(
    int unit);

#endif /* APPL_REF_DYNAMIC_PORT_H_INCLUDED */
