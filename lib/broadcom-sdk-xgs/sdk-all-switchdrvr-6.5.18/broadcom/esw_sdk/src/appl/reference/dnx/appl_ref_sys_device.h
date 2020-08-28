/** \file appl_ref_sys_device.h
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#define APPL_DNX_NOF_MODIDS_PER_DEVICE(sys_params)  ((sys_params)->nof_modids_per_device)
#define APPL_DNX_NOF_REMOTE_CORES 2
#define APPL_DNX_NOF_PORTS_PER_MODID 256
#define APPL_DNX_NOF_SYSPORTS_PER_DEVICE(sys_params) ((sys_params)->nof_modids_per_device * APPL_DNX_NOF_PORTS_PER_MODID)
#define APPL_DNX_NOF_MODIDS 2048
/*
 * }
 */

/**
 * \brief dnx system device params
 */
typedef struct
{
    int nof_devices;
    int nof_modids_per_device;
    int nof_modids_per_core;
    int modid;
    int device_index;

    int index_to_modid[APPL_DNX_NOF_MODIDS];
    int modid_to_index[APPL_DNX_NOF_MODIDS];
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
 * \brief - Configure visibility for the port
 *
 * 1. Enable a port
 * 2. Force a port
 *
 * \param [in] unit - Unit ID
 * \param [in] logical_port - port ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_vis_port_enable(
    int unit,
    bcm_port_t logical_port);

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

/**
 * \brief - Convert FAP index + TM port to 
 * modid
 *
 */
shr_error_e appl_dnx_fap_and_tm_port_to_modid_convert(
    int unit,
    int fap_index,
    int core_id,
    int tm_port,
    int *modid);
/**
 * \brief - Convert FAP index + TM port to 
 * modid
 *
 */
shr_error_e appl_dnx_modid_to_fap_and_core_convert(
    int unit,
    int modid,
    int *fap_index,
    int *core_id);

/**
 * \brief - Convert FAP index + TM port to 
 * modid + ftmh_pp_dsp
 *
 */
shr_error_e appl_dnx_fap_and_tm_port_to_modport_convert(
    int unit,
    int fap_index,
    int core_id,
    int tm_port,
    int *modid,
    int *ftmh_pp_dsp);

/*
 * }
 */
#endif /* APPL_REF_SYS_DEVICE_H_INCLUDED */
