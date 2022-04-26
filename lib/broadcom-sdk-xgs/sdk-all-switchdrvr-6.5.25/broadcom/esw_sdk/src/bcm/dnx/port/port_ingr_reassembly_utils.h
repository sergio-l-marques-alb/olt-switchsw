/** \file port_ingr_reassembly_utils.h
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _PORT_INGR_REASSEMBLY_UTILS_H_INCLUDED_
/** { */
#define _PORT_INGR_REASSEMBLY_UTILS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief - Returns TRUE iff port belongs to interleaved interface
 * 
 */
shr_error_e dnx_port_ingr_reassembly_is_interface_interleaved(
    int unit,
    bcm_port_t port,
    int *is_interleaved);

/**
 * \brief - Returns true in should_handle_tdm argument iff 
 * reassembly context is configured for TDM ports of this port type
 */
shr_error_e dnx_port_ingr_reassembly_port_should_handle_tdm(
    int unit,
    bcm_port_t port,
    int *should_handle_tdm);

/** } */
#endif /*_PORT_INGR_REASSEMBLY_UTILS_H_INCLUDED_*/
