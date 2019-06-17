/**
 * \file algo_ptp.h Internal DNX Managment APIs 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef ALGO_PTP_H_INCLUDED
/*
 * { 
 */
#define ALGO_PTP_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/** template manager defines */
#define DNX_PTP_PORT_TEMPLATE_NAME "ptp port profile"

/**
 * \brief
 *      callback print function for template manager. print the ptp port profile
 * \param [in] unit -unit id
 * \param [in] data -const pointer to profile data
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 *  void
 * \remark
 *   NONE
 * \see
 *   NONE
 */
void dnx_algo_ptp_tm_port_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Init function, init all ptp resources
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_ptp_init(
    int unit);

/**
 * \brief - de-Init ptp function
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_ptp_deinit(
    int unit);

/*
 * } 
 */
#endif /* ALGO_PTP_H_INCLUDED */
