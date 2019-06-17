/** \file bcm_int/dnx/dram/dram.h
 *
 * This file contains DRAM main structure and routine declarations for the Dram operation.
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_H_INCLUDED
/*
 * {
 */
#define _BCMINT_DNX_DRAM_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

typedef struct
{
    hbm_dram_vendor_info_hbm_t hbm_info;
} dnx_dram_vendor_info_t;

/**
 * \brief - Implementation of API bcm_switch_dram_power_down_cb_register
 *
 * \param [in] unit - unit number
 * \param [in] flags - NONE
 * \param [in] callback - pointer to callback function
 * \param [in] userdata - pointer to the user data. NULL if not exist.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_power_down_cb_register(
    int unit,
    uint32 flags,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata);

/**
 * \brief - implementation of API bcm_switch_dram_power_down_cb_unregister
 *
 * \param [in] unit - unit number
 * \param [in] callback - pointer to callback function
 * \param [in] userdata - pointer to the user data. NULL if not exist.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_power_down_cb_unregister(
    int unit,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata);

/**
 * \brief - power dowm dram procedure when temperature exceed the "power down" threshold.
 *
 * \param [in] unit - unit number
 * \param [in] flags - API flags (currently NONE)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This functionality should be used when Soc property: dram_temperature_monitor_enable=FALSE
 * \see
 *   * None
 */
shr_error_e dnx_dram_power_down(
    int unit,
    uint32 flags);

/**
 * \brief - init the dram after it was power down and powered up again by the user.
 *
 * \param [in] unit - unit number
 * \param [in] flags - API flags (currently NONE)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_reinit(
    int unit,
    uint32 flags);

/**
 * \brief - enable or disable traffic into DRAM. once the temperature exceed a threshold, user should disable traffic to DRAM. 
 *   When temperature reduce below threshold, it should enable it back.
 *
 * \param [in] unit - unit number
 * \param [in] flags - API flags (currently NONE)
 * \param [in] enable - true for enable traffic into DRAM, false for disable
 * \return
 *   shr_error_e
 *
 * \remark
 *   This functionality should be used when Soc property: dram_temperature_monitor_enable=FALSE
 * \see
 *   * None
 */
shr_error_e dnx_dram_traffic_enable_set(
    int unit,
    uint32 flags,
    uint32 enable);

/**
 * \brief - read from HW the dram themperature
 *
 * \param [in] unit - unit number
 * \param [in] interface_id - interface id
 * \param [out] sensor_data - output temperature (in parameter thermo_sensor_current) 
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_thermo_sensor_read(
    int unit,
    int interface_id,
    bcm_switch_thermo_sensor_t * sensor_data);

/**
 * \brief - read vendor info for given interface
 *
 * \param [in] unit - unit number
 * \param [in] interface_id - interface id
 * \param [out] vendor_info - output vendor info (only fields that are relevant for the specific dram are updated, the rest are 0);
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_vendor_info_get(
    int unit,
    int interface_id,
    dnx_dram_vendor_info_t * vendor_info);

#endif /* _BCMINT_DNX_DRAM_H_INCLUDED */
