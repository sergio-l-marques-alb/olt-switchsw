/** \file port_match.h
 * $Id$
 *
 * Internal DNX Port APIs
 *
 * $Copyright: (c) 2018 Broadcom.
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

/*
 * }
 */

/*
 * }
 */
#endif /* PORT_MATCH_H_INCLUDED */
