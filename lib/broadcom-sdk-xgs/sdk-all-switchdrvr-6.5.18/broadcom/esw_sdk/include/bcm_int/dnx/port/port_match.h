/** \file port_match.h
 * $Id$
 *
 * Internal DNX Port APIs
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef PORT_MATCH_H_INCLUDED
/* { */
#define PORT_MATCH_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
/*
 * Defines {
 */

/** }*/
/*
 * Structures {
 */

/**
 * \brief -
 *  Get VLAN domain and indication whther the assosiated in_lif_profile is LIF scope.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport_hw_resources - contains the global lif information
 * \param [out] vlan_domain - VLAN domain that can be used for key construction.
 * \param [out] is_intf_namespace - indicates whether the assosiated in_lif_profile is LIF scope.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_ingress_native_ac_interface_namespace_check(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    uint32 *vlan_domain,
    uint32 *is_intf_namespace);

/**
 * \brief -
 *  Allocate or deallocate an SVTAG ESEM command call on the port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport -the tunnel ID or the TM port ID
 * \param [in] enable - enable or disable the ESEM CMD).
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_port_match_svtag_esem_cmd_set(
    int unit,
    bcm_gport_t gport,
    int enable);

/**
 * \brief -
 *  Get the esem access command from a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, or out-lif-id in gport format.
 * \param [out] esem_cmd - esem access command that the port was using before.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
shr_error_e dnx_port_match_esem_cmd_port_get(
    int unit,
    bcm_gport_t port,
    int *esem_cmd);

/*
 * }
 */

/*
 * }
 */
#endif /* PORT_MATCH_H_INCLUDED */
