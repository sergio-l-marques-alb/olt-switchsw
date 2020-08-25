/** \file appl_ref_sys_device.h
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_SYS_DEVICE_H_INCLUDED
/*
 * {
 */
#define APPL_REF_SYS_DEVICE_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/*
 * Typedefs
 * {
 */

/*
 * DEFINEs
 * {
 */

/**
 * \brief - Each device represents to modid's (cores)
 */
#define APPL_DNX_NOF_MODIDS_PER_DEVICE 2

/*
 * }
 */

/**
 * \brief dnx system device params
 */
typedef struct
{
    int nof_devices;
    int base_modid;
    int modid;
} appl_dnx_sys_device_t;

/*
 * }
 */

/**
 * \brief - Configure general device level attributes:
 * 1. Confiugre my_modid. 
 * 
 * \param [in] unit - Unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_device_init(
    int unit);

/**
 * \brief - Return pointer to the struct that represents system device parameters
 * 
 * \param [in] unit - Unit ID
 * \param [out] sys_params - Pointer to the device system parameters
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_device_params_get(
    int unit,
    appl_dnx_sys_device_t ** sys_params);

/**
 * \brief - Return a bitmap of all confiugred ports
 * 
 * \param [in] unit - Unit ID
 * \param [in] pbmp - bitmap of ports
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_device_ports_get(
    int unit,
    bcm_pbmp_t * pbmp);

/**
 * \brief - Parse and verify system parameters
 * 
 * \param [in] unit - Unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_params_parse(
    int unit);

/**
 * \brief - Enable traffic (should be the last applications): 
 * 1. Enable traffic within the fap
 * 2. Enable control cells 
 * 
 * \param [in] unit - Unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_traffic_enable(
    int unit);

/**
 * \brief - Configure visibility
 * 1. Enable all ports besides OLP
 * 2. Force all ports besides OLP
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_vis_enable(
    int unit);

/**
 * \brief - Init Done step, used to update relevant modules that
 *          the application init was done.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_init_done_init(
    int unit);

/**
 * \brief - Deinit Done step, used to update relevant modules that
 *          the application deinit was done.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_init_done_deinit(
    int unit);

/*
 * }
 */
#endif /* APPL_REF_SYS_DEVICE_H_INCLUDED */
