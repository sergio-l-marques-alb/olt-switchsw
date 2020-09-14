/** \file access.h
 * access related SOC functions and Access related CBs for init mechanism.
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_DNX_ACCESS_H
/*
 * { 
 */
#define _SOC_DNX_ACCESS_H

#include <bcm_int/dnx/init/init.h>
#include <soc/sand/sand_signals.h>

/**
 * Define for the maximum number of memories that are causing
 * ECC errors when accessed during traffic. The define is used
 * as an array size for a DNX Data array holding the fields that
 * are used in order to mask the ECC of the relevant memories.
 */
#define DNX_MAX_NOF_ECC_ERR_MEMS 13

/**
 * \brief - allocates memory mutexes
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
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
shr_error_e dnx_access_mem_mutex_init(
    int unit);

/**
 * \brief - free memory mutexes
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
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
shr_error_e dnx_access_mem_mutex_deinit(
    int unit);

/**
 * \brief - update the auxiliary memory/register flags on device structure
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   \param [in] device - device structure #
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
shr_error_e dnx_access_aux_init(
    int unit,
    device_t * device);

/*
 * } 
 */
#endif /* _SOC_DNX_ACCESS_H */
