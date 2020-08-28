/** \file appl_ref_e2e_scheme.h
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $Id$
 */

#ifndef APPL_DNX_E2E_SCHEME_H_INCLUDED
/*
 * {
 */
#define APPL_DNX_E2E_SCHEME_H_INCLUDED

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
 * \brief - Cosq applicaion. Create default scheduling scheme: 
 * 1. Create multicast scheduling scheme 
 * 2. Create unicast scheduling scheme for each port 
 * 
 * \param [in] unit - Unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * appl_dnx_cosq_port_scheme_create()
 */
shr_error_e
appl_dnx_e2e_scheme_init(
    int unit);

/**
 * \brief - Cosq applicaion. Create default profiles:
 * 1. Create credit request profiles
 * 2. Create rate class profiles
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * appl_dnx_cosq_port_scheme_create()
 */
shr_error_e
appl_dnx_e2e_profiles_create(
    int unit);

/**
 * \brief - Create scheduling scheme for a given port:
 * 1. [Egress] Configure HR mode for the port.
 * 2. [Egress] Create Connectors for all VoQs pointing to the port on remote devices.
 * 3. [Ingress] Create VoQs pointing to the port on remote devices.
 * 4. [Egress] Connect the created connectors to the VoQs of the remote devices.
 * 5. [Ingress] Connect the created VoQs to the connectors of the port of each remote device. 
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - Logical port
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * 
 */
shr_error_e
appl_dnx_e2e_scheme_port_create(
    int unit,
    int port);

/**
 * \brief - Destroy scheduling scheme for a given port. 
 * Meaning, destroy each step created in appl_dnx_e2e_scheme_port_create().
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
 *   * appl_dnx_e2e_scheme_port_create() API
 */
shr_error_e
appl_dnx_e2e_scheme_port_destroy(
    int unit,
    int port);


/**
 * \brief - Convert modid index and logical port to base voq connector
 *
 * \param [in] unit - Unit ID
 * \param [in] local_port - Logical port
 * \param [in] remote_core_id - Core of remote voq
 * \param [in] ingress_modid - Ingress (remote device) modid
 * \param [in] nof_devices - nof devices in the system
 * \param [out] base_voq_connector -   Returned base voq connector
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
appl_dnx_logical_port_to_base_voq_connector_id_get(
    int unit,
    bcm_port_t port,
    bcm_module_t remote_modid,
    int *base_voq_connector);


/**
 * \brief - Convert modid and logical port to base voq id
 * 
 * \param [in] unit - Unit ID 
 * \param [in] modid - module id 
 * \param [in] logical_port - Logical port
 * \param [out] base_voq - Returned base voq
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
int
appl_dnx_e2e_scheme_logical_port_to_base_voq_get(int unit, bcm_module_t modid, bcm_port_t logical_port, int *base_voq);

/**
 * \brief - Get delay tolerance profile from port speed
 *
 * \param [in] unit - Unit ID
 * \param [in] speed - speed
 * \param [out] delay_tolerance_level - delay_tolerance_level
 * \param [out] rate_class - rate class profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_e2e_voq_profiles_get(
    int unit,
    int speed,
    int *delay_tolerance_level,
    int *rate_class);

/**
 * \brief - Create and set recommended thresholds of credit request profiles.
 *          It is recommended to create profiles according to the approximated port end to end rate
 *          and attributes: is low latency, is high bandwidth.
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
appl_dnx_e2e_credit_request_profiles_create(int unit);

/**
 * \brief - Create and set recommended thresholds of rate class profiles.
 *          It is recommended to create profiles according to the approximated port end to end rate.
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
appl_dnx_e2e_rate_class_profiles_create(int unit);


/*
 * }
 */
#endif /* APPL_DNX_E2E_SCHEME_H_INCLUDED */
