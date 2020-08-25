/*
 * $Id: init.c,v 1.14 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 *   This module calls the initialization routine of each BCM module.
 *
 * Initial System Configuration
 *
 *   Each module should initialize itself without reference to other BCM
 *   library modules to avoid a chicken-and-the-egg problem.  To do
 *   this, each module should initialize its respective internal state
 *   and hardware tables to match the Initial System Configuration.  The
 *   Initial System Configuration is:
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT
#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/boot.h>

#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/counter.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm/init.h>

#include <bcm_int/control.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/common/family.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dnxf_dispatch.h>
#include <bcm_int/dnxf/stat.h>
#include <bcm_int/dnxf/stack.h>
#include <bcm_int/dnxf/port.h>
#include <bcm_int/dnxf/link.h>
#include <bcm_int/dnxf/fabric.h>
#include <bcm_int/dnxf/rx.h>

#include <bcm_int/dnxf/tune/tune.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif

int _bcm_dnxf_detach(
    int unit);

static int _bcm_dnxf_init_finished_ok[BCM_MAX_NUM_UNITS] = { 0 };

#define BCM_DNXF_DRV_INIT_LOG(_unit_, _msg_str_)\
                LOG_INFO(BSL_LS_BCM_INIT,\
                         (BSL_META_U(_unit_,\
                                     "    + %d: %s\n"), _unit_ , _msg_str_))

int
bcm_dnxf_init_check(
    int unit)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    rc = (_bcm_dnxf_init_finished_ok[unit] == 1 ? _SHR_E_NONE : _SHR_E_UNIT);
    if (rc == _SHR_E_UNIT)
    {
        /*
         * bcm init not finished - return _SHR_E_UNIT withot error printing
         */
        SHR_SET_CURRENT_ERR(rc);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rc);
    }
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_init_selective(
    int unit,
    uint32 module_number)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    switch (module_number)
    {
        case BCM_MODULE_PORT:
            rc = bcm_dnxf_port_init(unit);
            SHR_IF_ERR_EXIT(rc);
            break;
        case BCM_MODULE_LINKSCAN:
            rc = bcm_dnxf_linkscan_init(unit);
            SHR_IF_ERR_EXIT(rc);
            break;
        case BCM_MODULE_STAT:
            rc = bcm_dnxf_stat_init(unit);
            SHR_IF_ERR_EXIT(rc);
            break;
        case BCM_MODULE_STACK:
            rc = bcm_dnxf_stk_init(unit);
            SHR_IF_ERR_EXIT(rc);
            break;
        case BCM_MODULE_MULTICAST:
            rc = bcm_dnxf_multicast_init(unit);
            SHR_IF_ERR_EXIT(rc);
            break;
        case BCM_MODULE_FABRIC:
            rc = bcm_dnxf_fabric_init(unit);
            SHR_IF_ERR_EXIT(rc);
            break;
        case BCM_MODULE_RX:
            rc = bcm_dnxf_rx_init(unit);
            SHR_IF_ERR_EXIT(rc);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "module %d not supported", module_number);
    }

exit:
    SHR_FUNC_EXIT;
}

int dnxf_sw_state_init(int unit)
{
    uint32 warmboot = 0;
    uint32 sw_state_max_size = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* check in DNX Data if we are enabling wb mode for this unit */
    warmboot = dnxf_data_device.ha.warmboot_support_get(unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
    dnxc_sw_state_is_warmboot_supported_set(unit, warmboot);
    sw_state_max_size = dnxf_data_device.ha.sw_state_max_size_get(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_init(unit, warmboot, sw_state_max_size));

exit:
    SHR_FUNC_EXIT; 
}

int
bcm_dnxf_init(
    int unit)
{

    int rc = SOC_E_NONE;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = soc_dnxf_ha_init(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxf_sw_state_init(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = soc_dnxf_init(unit, 1);
    SHR_IF_ERR_EXIT(rc);

    rc = bcm_chip_family_set(unit, BCM_FAMILY_DNXF);
    SHR_IF_ERR_EXIT(rc);

    BCM_DNXF_DRV_INIT_LOG(unit, "Port");
    rc = bcm_dnxf_init_selective(unit, BCM_MODULE_PORT);
    SHR_IF_ERR_EXIT(rc);

    BCM_DNXF_DRV_INIT_LOG(unit, "Linkscan");
    rc = bcm_dnxf_init_selective(unit, BCM_MODULE_LINKSCAN);
    SHR_IF_ERR_EXIT(rc);

    BCM_DNXF_DRV_INIT_LOG(unit, "Stat");
    rc = bcm_dnxf_init_selective(unit, BCM_MODULE_STAT);
    SHR_IF_ERR_EXIT(rc);

    BCM_DNXF_DRV_INIT_LOG(unit, "Multicast");
    rc = bcm_dnxf_init_selective(unit, BCM_MODULE_MULTICAST);
    SHR_IF_ERR_EXIT(rc);

    BCM_DNXF_DRV_INIT_LOG(unit, "Fabric");
    rc = bcm_dnxf_init_selective(unit, BCM_MODULE_FABRIC);
    SHR_IF_ERR_EXIT(rc);

    BCM_DNXF_DRV_INIT_LOG(unit, "Stack");
    rc = bcm_dnxf_init_selective(unit, BCM_MODULE_STACK);
    SHR_IF_ERR_EXIT(rc);

    BCM_DNXF_DRV_INIT_LOG(unit, "Rx");
    rc = bcm_dnxf_init_selective(unit, BCM_MODULE_RX);
    SHR_IF_ERR_EXIT(rc);

    /*
     * ALDWP configuration
     */
    if (!SOC_WARM_BOOT(unit))
    {
        rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_fabric_links_aldwp_init);
        SHR_IF_ERR_EXIT(rc);
    }

    /*
     * Tuning init
     */
     if(!SOC_WARM_BOOT(unit)) {
         SHR_IF_ERR_EXIT(dnxf_tune_init(unit));
     }
    /*
     * Update dnxf data that init done 
     */
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE));
    _bcm_dnxf_init_finished_ok[unit] = 1;

    /*
     * Mark soc control as done
     */
    SOC_CONTROL(unit)->soc_flags |= SOC_F_INITED;
    SOC_CONTROL(unit)->soc_flags |= SOC_F_ALL_MODULES_INITED;
 
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    if (SHR_FAILURE(_func_rv))
    {
        _bcm_dnxf_detach(unit);
    }
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_info_get(
    int unit,
    bcm_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    info->vendor = SOC_PCI_VENDOR(unit);
    info->device = SOC_PCI_DEVICE(unit);
    info->revision = SOC_PCI_REVISION(unit);
    info->capability = BCM_CAPA_LOCAL | BCM_CAPA_FABRIC;
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *         _bcm_dnxf_modules_deinit
 * Purpose:
 *         De-initialize bcm modules
 * Parameters:
 *     unit - (IN) BCM device number.
 * Returns:
 *     _SHR_E_XXX
 */
STATIC int
_bcm_dnxf_modules_deinit(
    int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);
    
    /*
     * Tuning deinit
     */
    SHR_IF_ERR_CONT(dnxf_tune_deinit(unit));

    rc = _bcm_dnxf_stk_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    rc = _bcm_dnxf_fabric_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    rc = bcm_dnxf_multicast_detach(unit);
    SHR_IF_ERR_CONT(rc);

    rc = _bcm_dnxf_stat_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    rc = bcm_dnxf_linkscan_detach(unit);
    SHR_IF_ERR_CONT(rc);

    rc = _bcm_dnxf_port_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    if (SOC_IS_RAMON(unit) && (SOC_DNXF_CONTROL(unit)->rx_thread_fifo_dma_semaphore != NULL))
    {
        rc = _bcm_dnxf_rx_deinit(unit);
        SHR_IF_ERR_CONT(rc);
    }

    /*
     * declare that driver is not detaching 
     */
    SOC_DETACH(unit, 0);
    
    SHR_FUNC_EXIT;
}

int
_bcm_dnxf_attach(
    int unit,
    char *subtype)
{
    int dunit, rc;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(subtype);

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;

    dunit = BCM_CONTROL(unit)->unit;

    if (SOC_UNIT_VALID(dunit))
    {
        BCM_CONTROL(unit)->chip_vendor = SOC_PCI_VENDOR(dunit);
        BCM_CONTROL(unit)->chip_device = SOC_PCI_DEVICE(dunit);
        BCM_CONTROL(unit)->chip_revision = SOC_PCI_REVISION(dunit);
        BCM_CONTROL(unit)->capability |= BCM_CAPA_FABRIC;
    }

    rc = bcm_dnxf_init(unit);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnxf_threads_shutdown
 * Purpose:
 *      Terminate all the spawned threads for specific unit.
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *      _SHR_E_XXX
 */
int
_bcm_dnxf_threads_shutdown(
    int unit)
{
    int rc;                     /* Operation return status. */
    SHR_FUNC_INIT_VARS(unit);

    rc = bcm_dnxf_linkscan_enable_set(unit, 0);
    SHR_IF_ERR_CONT(rc);

    rc = soc_counter_stop(unit);
    SHR_IF_ERR_CONT(rc);

    SHR_FUNC_EXIT;
}

int dnxf_sw_state_deinit(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_deinit(unit));

exit:
    SHR_FUNC_EXIT; 

}

int
_bcm_dnxf_detach(
    int unit)
{
    int rc;                     /* Operation return status. */
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Shut down all the spawned threads. 
     */
    rc = _bcm_dnxf_threads_shutdown(unit);
    SHR_IF_ERR_CONT(rc);

     /*
     * declare that driver is detaching 
     */
    SOC_DETACH(unit, 1);

    /*
     * Mark soc control as not done
     */
    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_INITED;
    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_ALL_MODULES_INITED;
    
    /*
     *  Don't move up, holding lock or disabling hw operations
     *  might prevent theads clean exit.
     */
    DNXF_UNIT_LOCK_TAKE(unit);

    rc = _bcm_dnxf_modules_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    _bcm_dnxf_init_finished_ok[unit] = 0;
    /* Update dnxf data that deinit begin */
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_reset(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE));

    rc = soc_dnxf_deinit(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxf_sw_state_deinit(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = soc_dnxf_ha_deinit(unit);
    SHR_IF_ERR_EXIT(rc);

#ifdef BCM_WARM_BOOT_SUPPORT
    rc = soc_scache_detach(unit);
    SHR_IF_ERR_EXIT(rc);
#endif

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
_bcm_dnxf_match(
    int unit,
    char *subtype_a,
    char *subtype_b)
{
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_REFERENCE(unit);
    SHR_IF_ERR_EXIT(sal_strcmp(subtype_a, subtype_b));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if device is member in a group 'member_type'.
 *          Allows to write generic c-int and applications.
 *
 * \param [in] unit - unit #
 * \param [in] flags - set to 0
 * \param [in] member_type - group
 * \param [in] is_member - 1 if device is a member in the group (otherwise 0)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnxf_device_member_get(
    int unit,
    uint32 flags,
    bcm_device_member_t member_type,
    int *is_member)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_NULL_CHECK(is_member, _SHR_E_PARAM, "is_member");
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "flags must be 0\n");

    /** get is_member */
    switch (member_type)
    {
        case bcmDeviceMemberDNXF:
        case bcmDeviceMemberDNXC:
            *is_member = 1;
            break;

        default:
            *is_member = 0;
            break;
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
