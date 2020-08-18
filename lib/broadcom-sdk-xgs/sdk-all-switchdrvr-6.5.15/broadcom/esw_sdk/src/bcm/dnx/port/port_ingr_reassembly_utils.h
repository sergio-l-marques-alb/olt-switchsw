/** \file port_ingr_reassembly_utils.h
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
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

/** } */
#endif /*_PORT_INGR_REASSEMBLY_UTILS_H_INCLUDED_*/
