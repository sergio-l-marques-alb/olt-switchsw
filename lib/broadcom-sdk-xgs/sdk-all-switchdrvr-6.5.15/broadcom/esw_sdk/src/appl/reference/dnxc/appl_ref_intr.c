/*
 * $Id: appl_dcmn_intr.c, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm_int/common/debug.h>
#include <appl/diag/system.h>
#include <appl/reference/dnxc/appl_ref_intr.h>
#include <shared/shrextend/shrextend_debug.h>
#if defined BCM_DNX_SUPPORT && defined DNX_EMULATION_1_CORE
#include <soc/sand/sand_aux_access.h>
#endif

/*************
 * DEFINES   *
 *************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR

/*************
 * TYPE DEFS *
 *************/

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - enable interrupt reference log except which explicit disabled
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_interrupt_appl_log_defaults_set(
    int unit)
{
    int rv;
    bcm_switch_event_control_t event;

    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))
#endif
    {
        event.action = bcmSwitchEventLog;
        event.index = 0;
        event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;

        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0x1);
        SHR_IF_ERR_EXIT(rv);

#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            SHR_IF_ERR_EXIT(dnxf_interrupt_appl_log_defaults_set(unit));
        } else
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_interrupt_appl_log_defaults_set(unit));
        } else
#endif
        {
            SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - mask/unmask interrupt which explicit declaired
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_interrupt_appl_mask_defaults_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: No interrupts for device\n")));
        SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))
#endif
    {
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            SHR_IF_ERR_EXIT(dnxf_interrupt_appl_mask_defaults_set(unit));
        } else
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_interrupt_appl_mask_defaults_set(unit));
        } else
#endif
        {
            LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: Only support DNX and DNXF device\n")));
            SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

void
appl_dnxc_interrupt_appl_cb(
    int unit,
    bcm_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3,
    void *userdata)
{
    if (BCM_SWITCH_EVENT_DEVICE_INTERRUPT != event)
    {
        return;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0))
    {
        LOG_DEBUG(BSL_LS_APPLDNX_INTR,
                  (BSL_META_U(unit, "user callback called for interrupt number %u, instance %u, reset %u\n"), arg1,
                   arg2, arg3));
    }
    else
    {
        LOG_DEBUG(BSL_LS_APPLDNX_INTR,
                  (BSL_META_U(unit, "user callback called for interrupt number %u, instance %u\n"), arg1, arg2));
    }
    return;
}
/**
 * \brief - init interrupt application for reference
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_interrupt_init(
    int unit)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    SHR_FUNC_INIT_VARS(unit);

#if defined BCM_DNX_SUPPORT && defined DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Not initializing interrupts application in emulation%s%s%s", EMPTY, EMPTY,
                          EMPTY);
    }
#endif
    memset((void *) &bcm_switch_event_control, 0, sizeof(bcm_switch_event_control_t));

    rc = bcm_switch_event_register(unit, appl_dnxc_interrupt_appl_cb, NULL);
    SHR_IF_ERR_EXIT(rc);

    rc = appl_dnxc_interrupt_appl_log_defaults_set(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = appl_dnxc_interrupt_appl_mask_defaults_set(unit);
    SHR_IF_ERR_EXIT(rc);

    /*
     * Enable all interrupts
     */
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x0);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

static int
appl_dnxc_interrupt_deinit_internal(
    int unit,
    int cache_upd_req)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    SHR_FUNC_INIT_VARS(unit);
    memset((void *) &bcm_switch_event_control, 0, sizeof(bcm_switch_event_control_t));
    rc = bcm_switch_event_unregister(unit, appl_dnxc_interrupt_appl_cb, NULL);
    SHR_IF_ERR_EXIT(rc);

    if (cache_upd_req)
    {
        /*
         * Update cached memories to detect ECC and parity errors 
         */
        rc = bcm_switch_control_set(unit, bcmSwitchCacheTableUpdateAll, 0);
        SHR_IF_ERR_EXIT(rc);
    }

    /*
     * Disable all interrupts
     */
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x1);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - deinit interrupt application for reference
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_interrupt_deinit(
    int unit)
{
    return (appl_dnxc_interrupt_deinit_internal(unit, 1));
}

int
appl_dnxc_interrupt_nocache_deinit(
    int unit)
{
    return (appl_dnxc_interrupt_deinit_internal(unit, 0));
}

#undef BSL_LOG_MODULE
