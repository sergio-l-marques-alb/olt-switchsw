/** \file mpls_l2vpn_ingress.h
 *      Used by mpls_l2vpn.c
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _MPLS_L2VPN_INGRESS_INCLUDED__
          /*
           * {
           */
#define _MPLS_L2VPN_INGRESS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

#include <soc/dnx/dbal/dbal.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>

/**
* \brief
* Creates the mpls port object described above with its ingress objects.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] l2vpn_info  -  Holds all the relevant information for the different schemes that might be defined on a L2VPN LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_add_ingress_only(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info);

/**
* \brief
* Get the mpls port object described above with its ingress objects.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] l2vpn_info  -  Holds all the relevant information for the different schemes that might be defined on a L2VPN LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_get_ingress_only(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info);

/**
* \brief
* Delete the mpls port object with its ingress objects.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_l2vpn_id  - The port to be deleted.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_delete_ingress_only(
    int unit,
    bcm_gport_t mpls_l2vpn_id);

#endif  /*_MPLS_L2VPN_INGRESS_INCLUDED__*/
