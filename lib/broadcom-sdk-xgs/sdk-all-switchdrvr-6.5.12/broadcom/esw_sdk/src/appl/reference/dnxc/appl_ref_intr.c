/*
 * $Id: appl_dcmn_intr.c, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2017 Broadcom.
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
static intr_common_params_t interrupt_common_params[SOC_MAX_NUM_DEVICES];

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
    int rv, i;
    soc_interrupt_db_t *interrupts;
    soc_block_types_t block;
    soc_reg_t reg;
    int blk;
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

        interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
        for (i = 0; interrupt_common_params[unit].int_disable_print_on_init[i] != INVALIDr; i++)
        {
            event.event_id = interrupt_common_params[unit].int_disable_print_on_init[i];
            reg = interrupts[event.event_id].reg;

            /*
             * Unsupported interrupts
             */
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block)
            {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP
                               || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit,
                                                                                             blk) :
                    SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0);
                SHR_IF_ERR_EXIT(rv);
            }
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
    int rv, i;
    soc_interrupt_db_t *interrupts;
    soc_block_types_t block;
    soc_reg_t reg;
    int blk;
    bcm_switch_event_control_t event;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: No interrupts for device\n")));
        SHR_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))
#endif
    {
        for (i = 0; interrupt_common_params[unit].int_disable_on_init[i] != INVALIDr; i++)
        {
            event.event_id = interrupt_common_params[unit].int_disable_on_init[i];
            event.action = bcmSwitchEventMask;
            reg = interrupts[event.event_id].reg;

            /*
             * Unsupported interrupts
             */
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block)
            {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP
                               || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit,
                                                                                             blk) :
                    SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 1);
                SHR_IF_ERR_EXIT(rv);
            }
        }

        for (i = 0; interrupt_common_params[unit].int_active_on_init[i] != INVALIDr; i++)
        {
            event.event_id = interrupt_common_params[unit].int_active_on_init[i];
            event.action = bcmSwitchEventMask;
            reg = interrupts[event.event_id].reg;

            /*
             * Unsupported interrupts
             */
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block)
            {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP
                               || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit,
                                                                                             blk) :
                    SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0);
                SHR_IF_ERR_EXIT(rv);
            }
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

    memset((void *) &bcm_switch_event_control, 0, sizeof(bcm_switch_event_control_t));
    if (SOC_IS_RAMON(unit))
    {
#ifdef BCM_DNXF_SUPPORT
        rc = ramon_interrupt_cmn_param_init(unit, &interrupt_common_params[unit]);
        SHR_IF_ERR_EXIT(rc);
#endif
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "ERROR: Interrupt not supported\n")));
        SHR_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

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

uint32 *
appl_dnxc_interrupt_active_on_intr_get(
    int unit)
{
    return interrupt_common_params[unit].int_active_on_init;
}

#undef BSL_LOG_MODULE
