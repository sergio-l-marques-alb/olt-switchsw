
/** \file dnxc_sw_state.c
 * This is the main dnxc sw state module.
 * Module is in charge for general sw state functions that are 
 * not implementation specific (like init/deinit). 
 *  
 * Implementation specific functions will be found in: 
 * dnxc_sw_state_plain.c 
 * dnxc_sw_state_wb.c 
 * */ 

/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

/*
 * Include files
 * {
 */
#include <assert.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <shared/shrextend/shrextend_debug.h>
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
 * see .h file for description
 */
int dnxc_sw_state_init(int unit, uint32 warmboot, uint32 sw_state_max_size)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    /* if WB Boot flags are on but DNX Data shows wb is not supported for the unit return error */
    if (SOC_WARM_BOOT(unit) && !warmboot) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,
                                 "device is loading in Warm Boot although Warmboot is not supported\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    /* if compiled without WB and wb soc property is on return error */
#ifndef BCM_WARM_BOOT_SUPPORT
    if (warmboot) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,
                                 "driver compiled without BCM_WARM_BOOT_SUPPORT but trying to enable warmboot\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    /* init HA HW Access validation tools */
    SHR_IF_ERR_EXIT(dnxc_ha_init(unit));

    /* Init sw state verifications */
    DNX_SW_STATE_VERIFICATIONS_ONLY(dnxc_sw_state_verifications_init(unit));

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

    DNX_SW_STATE_FUNC_RETURN;

}

/*
 * see .h file for description
 */
int dnxc_sw_state_deinit(int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

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
    DNX_SW_STATE_VERIFICATIONS_ONLY(dnxc_sw_state_verifications_deinit(unit));

    /* deinit HA HW Access validation tools */
    SHR_IF_ERR_EXIT(dnxc_ha_deinit(unit));

    DNX_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
