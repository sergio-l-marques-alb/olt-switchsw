/** \file cosq_ingress_port.h
 * $Id$
 * 
 * Internal API of COSQ control set / get
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

/*
 * $Copyright:.$
 */

#ifndef _DNX_COSQ_INGRESS_PORT_H_INCLUDED_
/** { */
#define _DNX_COSQ_INGRESS_PORT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - sub function of bcm_cosq_control_set, to handle 
 *        types where the port is not GPORT (port = local port)
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - type of command to perform (see 
 *        bcm_cosq_control_t)
 * \param [in] arg - argument to set in the given command
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_ingress_port_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int arg);
/**
 * \brief - sub function of bcm_cosq_contol_get, to handle types 
 *        of commands where the expected port is not GPORT (port
 *        = local port)
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - type of command to perform
 * \param [in] arg - returned value from given command
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_ingress_port_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int *arg);

#endif /*_DNX_COSQ_INGRESS_PORT_H_INCLUDED_*/
