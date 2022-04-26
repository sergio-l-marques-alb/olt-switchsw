/**
 * \file algo_instru.h
 * Internal DNX instru Managment APIs
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ 
 */

#ifndef ALGO_INSTRU_H_INCLUDED
/*
 * { 
 */
#define ALGO_INSTRU_H_INCLUDED

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - Print an entry of the UDP tunnel profile. Fields are UDP tunnel and RAW outlif
 *          entries.
 * 
 * \param [in] unit - unit id
 * \param [in] data - Pointer to the struct to be printed.
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_algo_instru_sflow_destination_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Intialize instru algorithms.
 * 
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_instru_init(
    int unit);

/*
 * } 
 */
#endif/*_ALGO_INSTRU_API_INCLUDED__*/
