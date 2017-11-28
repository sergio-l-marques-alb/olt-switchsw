/** \file dnxc_ha.c
 * General HA routines.
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <sal/core/sync.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/dnxc/dnxc_ha.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#define DNXC_ALLOW_HW_WRITE_TIME_OUT            (5000000)       /* 5 sec */

sal_thread_t dnxc_allow_write_thread_id[SOC_MAX_NUM_DEVICES];
sal_mutex_t dnxc_allow_write_mutex[SOC_MAX_NUM_DEVICES];

int
dnxc_ha_init(
    int unit)
{

    dnxc_allow_write_thread_id[unit] = SAL_THREAD_ERROR;
    dnxc_allow_write_mutex[unit] = sal_mutex_create("HaAllowWrite");

    if (dnxc_allow_write_mutex[unit] == NULL)
    {
        LOG_ERROR(BSL_LS_SWSTATEDNX_GENERAL, (BSL_META_U(unit, "Failed to create dnxc_allow_write_mutex.\n")));
        return SOC_E_RESOURCE;
    }

    return SOC_E_NONE;
}

int
dnxc_ha_deinit(
    int unit)
{
    dnxc_allow_write_thread_id[unit] = SAL_THREAD_ERROR;
    sal_mutex_destroy(dnxc_allow_write_mutex[unit]);

    return SOC_E_NONE;
}

int
dnxc_is_hw_access_disabled(
    int unit)
{

    /*
     * If thread has temporary permission to write, return True 
     */
    if (dnxc_allow_write_thread_id[unit] == sal_thread_self())
    {
        return FALSE;
    }

    if (SOC_WARM_BOOT(unit))
    {
        return TRUE;
    }

    if (SOC_IS_RELOADING(unit))
    {
        return TRUE;
    }

    if (SOC_IS_DETACHING(unit))
    {
        return TRUE;
    }

    return FALSE;
}

int
dnxc_allow_hw_write_enable(
    int unit)
{

    /*
     * check that unit is valid 
     */
    if (unit < 0 || unit >= SOC_MAX_NUM_DEVICES)
    {
        return SOC_E_UNIT;
    }

    /*
     * if HW access is allowed then nothing to do 
     */
    if (!dnxc_is_hw_access_disabled(unit))
    {
        return SOC_E_NONE;
    }

    /*
     * take the mutex 
     */
    if (sal_mutex_take(dnxc_allow_write_mutex[unit], DNXC_ALLOW_HW_WRITE_TIME_OUT))
    {
        LOG_ERROR(BSL_LS_SWSTATEDNX_GENERAL, (BSL_META_U(unit, "Failed to take dnxc_allow_write_mutex.\n")));
        return SOC_E_INTERNAL;
    }

    dnxc_allow_write_thread_id[unit] = sal_thread_self();

    return SOC_E_NONE;
}

/*******************************************
* @function soc_schan_override_disable
* @Purpose Disable schan override
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
int
dnxc_allow_hw_write_disable(
    int unit)
{

    /*
     * check that unit is valid 
     */
    if (unit < 0 || unit >= SOC_MAX_NUM_DEVICES)
    {
        return SOC_E_UNIT;
    }

    if (dnxc_allow_write_thread_id[unit] == sal_thread_self())
    {

        /*
         * mark dnxc_allow_write_thread_id as empty 
         */
        dnxc_allow_write_thread_id[unit] = SAL_THREAD_ERROR;

        /*
         * give away the mutex 
         */
        if (sal_mutex_give(dnxc_allow_write_mutex[unit]))
        {
            LOG_ERROR(BSL_LS_SWSTATEDNX_GENERAL, (BSL_META_U(unit, "Failed to release dnxc_allow_write_mutex.\n")));
            return SOC_E_INTERNAL;
        }
    }

    return SOC_E_NONE;
}

#undef _ERR_MSG_MODULE_NAME
