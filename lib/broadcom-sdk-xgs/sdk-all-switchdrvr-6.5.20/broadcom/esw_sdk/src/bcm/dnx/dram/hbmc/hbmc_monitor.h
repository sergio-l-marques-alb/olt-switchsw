/** \file src/bcm/dnx/dram/hbmc/hbmc_monitor.h
 * 
 * Internal DNX hbmc monitoring APIs to be used in hbmc
 * module 
 * 
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCM_DNX_DRAM_HBMC_HBMCMONITOR_H_INCLUDED
/*
 * { 
 */
#define _BCM_DNX_DRAM_HBMC_HBMCMONITOR_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>

/**
 * \brief - thread CB for HBM temperature logging
 *
 * \param [in] unit - unit id
 * \param [in] user_data - opaque user data, currently not used
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_temp_log(
    int unit,
    void *user_data);

/**
 * \brief - thread CB for HBM temperature monitoring
 *
 * \param [in] unit - unit id
 * \param [in] user_data - opaque user data, currently not used
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_temp_monitor(
    int unit,
    void *user_data);

/**
 * \brief - restore traffic to the hbm
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_traffic_to_hbm_restore(
    int unit);

/**
 * \brief - stop traffic from going to the hbm
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_traffic_to_hbm_stop(
    int unit);

/**
 * \brief - get indication if hbm is empty. after traffic was stopped to hbm
 *
 * \param [in] unit - unit number
 * \param [out] hbm_is_empty - indication that hbm is empty after traffic was stopped to hbm
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_traffic_is_empty_get(
    int unit,
    uint8 *hbm_is_empty);

/**
 * \brief - generate temperature stub. Used for debug and testing only!!!
 *   build link-list of "fake" temperature that the driver will read each call to function "dnx_hbmc_temp_get".
 *
 * \param [in] unit - unit number
 * \param [in] nof_elements - number of elements to produce in the stub temperature link-list.
 * \param [in] temp_arr - pointer to array of temperature values, that will be used for the stub.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_temp_stub_set(
    int unit,
    int nof_elements,
    uint32 *temp_arr);

#endif /* _BCM_DNX_DRAM_HBMC_HBMCMONITOR_H_INCLUDED */
