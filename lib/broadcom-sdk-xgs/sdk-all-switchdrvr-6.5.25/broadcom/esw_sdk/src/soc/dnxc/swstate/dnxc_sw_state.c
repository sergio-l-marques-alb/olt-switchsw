/** \file dnxc_sw_state.c
 * This is the main dnxc sw state module.
 * Module is in charge for general non access interface sw state
 * functions  (like init/deinit). 
 *  
 * Implementation specific access functions will be found in: 
 * dnxc_sw_state_plain.c 
 * dnxc_sw_state_wb.c 
 *  
 * Access functions that are common to all implementations will 
 * be at: 
 * dnxc_sw_state_common.c 
 * */ 

/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
/* ---------- */

/*
 * Include files
 * {
 */
#include <assert.h>
#include <soc/drv.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnx_sw_state_logging.h>
#include <soc/dnxc/swstate/sw_state_diagnostics.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#endif
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/auto_generated/access/wb_engine_access.h>

/*
 * }
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * Defines
 * {
 */


/*
 * }
 */

/*
 * indicates if diagnostics are enabled for the unit
 */
int dnxc_sw_state_diagnostic_is_on[SOC_MAX_NUM_DEVICES] = {0};

/*
 * see .h file
 */
int dnxc_sw_state_diag_init(int unit) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /* for FE diag is always enabled */
    if (soc_is(unit, DNXF_DEVICE))
    {
        dnxc_sw_state_diagnostic_is_on[unit] = 1;
    }
    else
    {
        dnxc_sw_state_diagnostic_is_on[unit] = 0;
    }

#ifdef BCM_DNX_SUPPORT
#ifdef DNX_SW_STATE_DIAGNOSTIC
    if (soc_is(unit, DNX_DEVICE) && dnx_data_dev_init.ha.feature_get(unit, dnx_data_dev_init_ha_sw_state_diagnostics)) {
        dnxc_sw_state_diagnostic_is_on[unit] = 1;
    }
#endif
#endif

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file
 */
int dnxc_sw_state_diag_is_on(int unit) {
    return dnxc_sw_state_diagnostic_is_on[unit];
}


/*
 * see .h file for description
 */
int dnxc_sw_state_init(int unit, uint32 warmboot, uint32 sw_state_max_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /* if WB Boot flags are on but DNX Data shows wb is not supported for the unit return error */
    if (SOC_WARM_BOOT(unit) && !warmboot) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,
                                 "device is loading in Warm Boot although Warmboot is disabled by SoC property or compilation flag\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    /* if compiled without WB and wb soc property is on return error */
#ifndef BCM_WARM_BOOT_SUPPORT
    if (warmboot) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,
                                 "driver compiled without BCM_WARM_BOOT_SUPPORT but trying to enable warmboot (by soc property warmboot_support)\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /* init HA HW Access validation tools */
    SHR_IF_ERR_EXIT(dnxc_ha_init(unit));

#ifdef DNX_SW_STATE_VERIFICATIONS
    /* Init sw state verifications */
    SHR_IF_ERR_EXIT(dnxc_sw_state_verifications_init(unit));
#endif

    /*
     * Init sw state logging.
     */
#ifdef DNX_SW_STATE_LOGGING
    dnx_sw_state_log_init(unit);
#endif /* DNX_SW_STATE_LOGGING */

    /*
     * Init sw state diagnostics.
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_diag_init(unit));


    /* 
     * call init sequence for the plain implementation,
     * Should be always called, because even if unit support warmboot, 
     * some modles may opt out and use plain implementation
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_init_plain(unit, 0));

    /* 
     * call init sequence for the wb implementation,
     * should be called only if unit supports wb. 
     * In case of a warm reboot, restoration will be happening inside this function 
     */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (warmboot) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_init_wb(unit, 0, sw_state_max_size));
    }
#endif

    /* needed in order to save wb_engine in sw state */
    if (!SOC_WARM_BOOT(unit)) {
        SHR_IF_ERR_EXIT(sw_state_wb_engine.init(unit));
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_deinit(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /* 
     * call deinit sequence for the wb implementation,
     * should be always called, if not needed it will do nothing and exit. 
     * Will free all Warmbootable memoory allocated by SW State infrastructures 
     */
#ifdef BCM_WARM_BOOT_SUPPORT
        SHR_IF_ERR_EXIT(dnxc_sw_state_deinit_wb(unit, 0));
#endif

    /* 
     * call deinit sequence for the plain implementation,
     * Will free all memoory allocated by SW State infrastructures
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_deinit_plain(unit, 0));

    /* Deinit sw state verifications */
    DNXC_SW_STATE_VERIFICATIONS_ONLY(dnxc_sw_state_verifications_deinit(unit));

#ifdef DNX_SW_STATE_LOGGING
    dnx_sw_state_log_deinit(unit);
#endif /* DNX_SW_STATE_LOGGING */

    /* deinit HA HW Access validation tools */
    SHR_IF_ERR_EXIT(dnxc_ha_deinit(unit));

    /* 
     * Something is peculiar here but it is like that for all devices. 
     * scache is initialized as part of the diag_shell in very early stages of 
     * init, it's detached by the soc layer here. when performing init-deinit cycle (tr 141) 
     * scache will be detached in soc_deinit but will never be re-attached. 
     * things still work fine however because the detach leave scache at the same state as after init. 
     * I leave this peculiar behaviour of scache because it's a very old and heavily used common code 
     */
#ifdef BCM_WARM_BOOT_SUPPORT
    SHR_IF_ERR_EXIT(soc_scache_detach(unit));
#endif

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
