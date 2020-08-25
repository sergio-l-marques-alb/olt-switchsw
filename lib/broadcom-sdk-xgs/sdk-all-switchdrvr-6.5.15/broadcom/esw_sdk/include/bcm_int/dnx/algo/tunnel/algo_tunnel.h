/**
 * \file algo_tunnel.h Internal tunnel APIs
PIs $Copyright: (c) 2018 Broadcom.
PIs Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef ALGO_TUNNEL_H_INCLUDED
/*
 * { 
 */
#define ALGO_TUNNEL_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Resource name defines for algo tunnel 
 * {
 */

#define DNX_ALGO_TUNNEL_IPV6_SIP_PROFILE "TUNNEL IPV6 SIP PROFILE"

#define DNX_ALGO_TUNNEL_UDP_PORTS_PROFILE "TUNNEL UDP PORTS PROFILE"

#define DNX_ALGO_TUNNEL_IPV6_SIP_PROFILE_TEMPLATE_SIZE 80
/**
 * }
 */
/**
 * \brief 
 *   allocate resource pool for all TUNNEL profiles types
 * \param [in] unit - 
 *   The unit number. 
 * \return  
 *   \retval Negative in case of an error. 
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_algo_tunnel_init(
    int unit);

/**
 * \brief 
 *   deallocate resource pool for all TUNNEL profiles types
 * \param [in] unit - 
 *   The unit number. 
 * \return  
 *   \retval Negative in case of an error. 
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_algo_tunnel_deinit(
    int unit);

/**
* \brief
* Print an entry of ipv6 sip tunnel profile template. 
* \param [in] unit - the unit number 
* \param [in] data - Pointer of the struct to be printed.
* \param [in] print_cb_data - Pointer of the print callback data.
* \return 
*     None 
*/
void dnx_algo_tunnel_ipv6_sip_profile_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
* \brief
* Print an entry of UDP ports tunnel profile template.
* \param [in] unit - the unit number
* \param [in] data - Pointer of the struct to be printed.
* \param [in] print_cb_data - Pointer of the print callback data.
* \return
*     None
*/
void dnx_algo_tunnel_udp_ports_profile_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/*
 * } 
 */
#endif/*_ALGO_QOS_API_INCLUDED__*/
