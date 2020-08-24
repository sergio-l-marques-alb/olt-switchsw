/** \file appl_ref_sys_ports.h
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_SYS_PORTS_H_INCLUDED
/*
 * {
 */
#define APPL_REF_SYS_PORTS_H_INCLUDED

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
 * \brief - system ports application. Create system ports for all ports in the system:
 * 1. Each port in the system should get a uniaque ID (system port) 
 * 2. Map the ID to the physical port (modid, pp_dsp)
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
appl_dnx_sys_ports_init(
    int unit);

/**
 * \brief - Create a system port for a given port
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
 *   * None
 */
shr_error_e
appl_dnx_sys_ports_port_create(
    int unit,
    int port);

/**
 * \brief - Convert modid andlogical port to system port id.
 *
 * \param [in] unit - Unit ID
 * \param [in] modid - module id
 * \param [in] port - logical port
 * \param [Out] sysport - Returned system port index
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
appl_dnx_logical_port_to_sysport_get(
    int unit,
    bcm_module_t modid,
    bcm_port_t port,
    int *sysport);


/*
 * }
 */
#endif /* APPL_REF_SYS_PORTS_H_INCLUDED */
