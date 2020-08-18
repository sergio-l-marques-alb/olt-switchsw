/** \file port_ingr_reassembly_alloc.h
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _PORT_INGR_REASSEMBLY_ALLOC_H_INCLUDED_
/** { */
#define _PORT_INGR_REASSEMBLY_ALLOC_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief - should alloc be called for the provided port and priority
 */
shr_error_e dnx_port_ingr_reassembly_should_call_alloc(
    int unit,
    bcm_port_t port,
    int priority,
    int *call_alloc);

/**
 * \brief - allocate reassembly context for the port
 * 
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_context_alloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *reassembly_context);

/**
 * \brief - deallocate reassembly context for the port
 * 
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_context_dealloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 reassembly_context,
    int force_dealloc,
    int *is_dealloc);

/**
 * \brief - mark reassembly context as allocated on all cores
 * 
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_context_mark_allocated(
    int unit,
    uint32 reassembly_context);

/**
 * \brief - allocate TDM reassembly context for the port
 * 
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_tdm_context_alloc(
    int unit,
    bcm_port_t port,
    uint32 *reassembly_context);
/**
 * \brief - deallocate TDM reassembly context for the port
 * 
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_tdm_context_dealloc(
    int unit,
    bcm_port_t port,
    uint32 reassembly_context);

/**
 * \brief - Initialize resource manager used by port_ingr_reassembly dnx module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_res_mngr_init(
    int unit);

/**
 * \brief - De-Initialize resource manager used by port_ingr_reassembly dnx module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_res_mngr_deinit(
    int unit);

/** } */
#endif /*_PORT_INGR_REASSEMBLY_ALLOC_H_INCLUDED_*/
