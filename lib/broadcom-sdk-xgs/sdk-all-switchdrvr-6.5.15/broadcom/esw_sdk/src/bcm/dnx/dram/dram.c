/** \file src/bcm/dnx/dram/dram.c
 *
 *
 *  This file contains implimentation of functions for DRAM
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

/*
 * Include files.
 * {
 */

#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnx/drv.h>

/*
 * }
 */

/*
 * defines
 * {
 */

/*
 * }
 */

/** See header file */
shr_error_e
dnx_dram_power_down_cb_register(
    int unit,
    uint32 flags,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_power_down_cb_register(unit, flags, callback, userdata));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }
exit:
    SHR_FUNC_EXIT;
}

 /** See header file */
shr_error_e
dnx_dram_power_down_cb_unregister(
    int unit,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_power_down_cb_unregister(unit, callback, userdata));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_dram_power_down(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        int temp_monitoring_is_active;

        /** pause temp monitoring if it was activated */
        SHR_IF_ERR_EXIT(dnx_hbmc_temp_monitor_is_active(unit, &temp_monitoring_is_active));
        if (temp_monitoring_is_active)
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_temp_monitor_pause(unit));
        }
        /** power down HBM */
        SHR_IF_ERR_EXIT(dnx_hbmc_power_down(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_dram_reinit(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_reinit(unit, flags));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_dram_traffic_enable_set(
    int unit,
    uint32 flags,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_redirect_traffic_to_ocb(unit, (enable == FALSE), NULL));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_dram_thermo_sensor_read(
    int unit,
    int interface_id,
    bcm_switch_thermo_sensor_t * sensor_data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_temp_get(unit, interface_id, &sensor_data->thermo_sensor_current));
    }
exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_dram_vendor_info_get(
    int unit,
    int interface_id,
    dnx_dram_vendor_info_t * vendor_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vendor_info, _SHR_E_PARAM, "vendor_info");

    /** clear memory */
    sal_memset(vendor_info, 0, sizeof(*vendor_info));

    /** act according to dram type */
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        uint32 vendor_info_is_valid;
        /** Verify interface id valid */
        if (0 == (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap & (1 << interface_id)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "interface_id %d is invalid\n", interface_id);
        }

        /** try to get from SW-State first */
        SHR_IF_ERR_EXIT(dnx_dram_db.vendor_info.is_init.get(unit, interface_id, &vendor_info_is_valid));
        if(vendor_info_is_valid)
        {
            /** if vendor info is valid - SW-State has updated info, return it. */
            SHR_IF_ERR_EXIT(dnx_hbmc_vendor_info_from_sw_state_get(unit, interface_id, &vendor_info->hbm_info));
        }
        else
        {
            /** SW-State was never updated with vendor info, read from HW and update SW-State */
            soc_dnx_hbm_device_id_t device_id;

            /** get vendor info */
            SHR_IF_ERR_EXIT(soc_read_jr2_hbm_device_id(unit, interface_id, &device_id));
            SHR_IF_ERR_EXIT(dnx_hbmc_device_id_to_vendor_info_decode(unit, &device_id, &vendor_info->hbm_info));

            /** set info to SW-State */
            SHR_IF_ERR_EXIT(dnx_hbmc_vendor_info_to_sw_state_set(unit, interface_id, &vendor_info->hbm_info));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No known Dram types are supported for this device\n");
    }

exit:
    SHR_FUNC_EXIT;
}
