/*

 * $Id: dnxf_drv.c,v 1.87 Broadcom SDK $

 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

#include <shared/bsl.h>
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/iproc_m0ssq.h>
#endif

#include <soc/mcm/driver.h>     /* soc_base_driver_table */
#include <soc/error.h>
#include <soc/ipoll.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/linkctrl.h>

#include <soc/dnxc/legacy/fabric.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnxc/legacy/dnxc_dev_feature_manager.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>
#include <soc/dnxf/cmn/dnxf_config_defs.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxc/legacy/dnxc_cmic.h>
#include <soc/sand/sand_mem.h>
#include <soc/dnxc/legacy/dnxc_mem.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <sal/appl/sal.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#define SOC_DNXF_DEFAULT_MDIO_DIVISOR	(20)

/* Protection mutexes for each unit (used by SOC_DNXF_LOCK). */
sal_mutex_t _soc_dnxf_lock[BCM_MAX_NUM_UNITS];

/* Dfe interrupt CB */
soc_interrupt_fn_t dnxf_intr_fn = soc_intr;

int
soc_dnxf_tbl_is_dynamic(
    int unit,
    soc_mem_t mem)
{
    return MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_tbl_is_dynamic, (unit, mem));
}

/* 
 * Mark all cacheable tables
 */
void
soc_dnxf_tbl_mark_cachable(
    int unit)
{
    soc_mem_t mem;

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            continue;
        }
        /*
         * Skip the Read-only/Write-Only/Signal tables 
         */
        if (soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem))
        {
            continue;
        }
        /*
         * dynamic tables are not cacheable
         */
        if (soc_dnxf_tbl_is_dynamic(unit, mem))
        {
            continue;
        }
        /*
         * uncacheable memories 
         */

        /*
         * this memory is updated from other memory. therefore it can not be cached with current cache mechanism 
         */
        if (mem == MCT_MCTm)
        {
            continue;
        }
        if (mem == RTP_SLSCTm)
        {
            if (soc_dnxf_load_balancing_mode_destination_unreachable !=
                dnxf_data_fabric.topology.load_balancing_mode_get(unit))
            {
                continue;
            }
        }
        if (mem == FSRD_FSRD_WL_EXT_MEMm)
        {
            continue;
        }

        /*
         * SER tables should be not cacheable
         */
        if ((mem == SER_ACC_TYPE_MAPm) ||
            (mem == SER_MEMORYm) ||
            (mem == SER_RESULT_0m) ||
            (mem == SER_RESULT_1m) ||
            (mem == SER_RESULT_DATA_0m) ||
            (mem == SER_RESULT_DATA_1m) || (mem == SER_RESULT_EXPECTED_0m) || (mem == SER_RESULT_EXPECTED_1m))
        {
            continue;
        }
        SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
    }
}


extern soc_controlled_counter_t soc_dnxf_controlled_counter[];

int
soc_dnxf_misc_init(
    int unit)
{
    return _SHR_E_NONE;
}

int
soc_dnxf_mmu_init(
    int unit)
{
    return _SHR_E_NONE;
}

soc_functions_t soc_dnxf_drv_funs = {
    soc_dnxf_misc_init,
    soc_dnxf_mmu_init,
    NULL,
    NULL,
    NULL,
};

STATIC void
soc_dnxf_soc_properties_control_set(
    int unit)
{

    SOC_CONTROL(unit)->schanTimeout = dnxf_data_device.access.schan_timeout_usec_get(unit);
    SOC_CONTROL(unit)->schanIntrEnb = dnxf_data_device.access.schan_intr_enable_get(unit);

    /*
     * MIIM access
     */
    SOC_CONTROL(unit)->miimTimeout = dnxf_data_device.access.miim_timeout_usec_get(unit);

}

STATIC int
soc_dnxf_soc_properties_validate(
    int unit)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_DNXF_IS_FE2(unit) && dnxf_data_fabric.general.local_routing_enable_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Local routing can be enabled only if device is in FE13 mode!");
    }

    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit))
    {
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_fabric_cell_validate, (unit));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_dnxf_info_soc_properties(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set common SOC_CONTROL variables
     */
    soc_dnxf_soc_properties_control_set(unit);

    /*
     * Validate soc properties values 
     */
     /** 
       * NOTE! This logic can be moved to DNX data but the decision
       * was to add this second vaidation function for better
       * readability and debugging.
       */
    SHR_IF_ERR_EXIT(soc_dnxf_soc_properties_validate(unit));

    SHR_IF_ERR_EXIT(soc_dnxf_drv_soc_property_serdes_qrtt_read(unit));

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_drv_soc_property_serdes_qrtt_read(
    int unit)
{
    soc_pbmp_t          pbmp_disabled_links;
    int                 enabled_pbmp_control, disabled_pbmp_control;
    int                 quad, quad_inner_link, port, rc;
    uint32              quad_active;

    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(pbmp_disabled_links);
    for (quad = 0; quad < dnxf_data_port.general.nof_links_get(unit)/dnxf_data_device.blocks.nof_links_in_fmac_get(unit); quad++) {

        /* Check if a given quad is also disabled by a soc_property
           If so add it to disabled links pbmp*/
        quad_active = dnxf_data_port.static_add.quad_info_get(unit, quad)->quad_enable; 

         if (!quad_active) {
            for (quad_inner_link = 0; quad_inner_link < dnxf_data_device.blocks.nof_links_in_fmac_get(unit); quad_inner_link++) {
                SOC_PBMP_PORT_ADD(pbmp_disabled_links, quad*dnxf_data_device.blocks.nof_links_in_fmac_get(unit) + quad_inner_link);
            }
        }
    }

    for (port = 0; port < dnxf_data_port.general.nof_links_get(unit) ; port++) {
        /* If port is valid ADD     to    enabled pbmp
                        and REMOVE  from  disabled pbmp*/

        enabled_pbmp_control =  SOC_DNXF_PBMP_PORT_ADD;
        disabled_pbmp_control = SOC_DNXF_PBMP_PORT_REMOVE;

        /* SKU SUPPORT - disabled in init time all not supported links*/

        /* If port is invalid or disabled REMOVE from enabled pbmp
                                      and ADD    to   disabled pbmp*/
        if (!(PBMP_MEMBER(dnxf_data_port.general.supported_phys_get(unit)->pbmp, port)) || PBMP_MEMBER(pbmp_disabled_links, port)) {
            enabled_pbmp_control =  SOC_DNXF_PBMP_PORT_REMOVE;
            disabled_pbmp_control = SOC_DNXF_PBMP_PORT_ADD;
        }

        /*
         * Remove/add to enabled bmp.
         */
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_sfi));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_port));            
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_all));            
        SHR_IF_ERR_EXIT(rc);

        /*
         * Add/remove to disabled bmp.
         */
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_sfi_disabled));           
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_port_disabled));            
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_all_disabled));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_control_init(
    int unit)
{
    int rv;
    uint16 dev_id;
    uint8 rev_id;
    soc_dnxf_control_t *dnxf = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * dnxf info config
     */
    /*
     * prepare config info for the next init sequnace
     */
    dnxf = SOC_DNXF_CONTROL(unit);
    if (dnxf == NULL)
    {
        dnxf = (soc_dnxf_control_t *) sal_alloc(sizeof(soc_dnxf_control_t), "soc_dnxf_control");
        if (dnxf == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate soc_dnxf_control");
        }
        sal_memset(dnxf, 0, sizeof(soc_dnxf_control_t));

        SOC_CONTROL(unit)->drv = dnxf;
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    rv = soc_dnxf_info_config(unit, dev_id);
    SHR_IF_ERR_CONT(rv);

exit:
    SHR_FUNC_EXIT;
}

int
dnxf_tbl_mem_cache_mem_set(
    int unit,
    soc_mem_t mem,
    void *en)
{
    int rc;
    int enable = *(int *) en;

    SHR_FUNC_INIT_VARS(unit);

    SOC_MEM_ALIAS_TO_ORIG(unit, mem);
    if (!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
    {
        return _SHR_E_NONE;
    }

    if ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)
    {

        rc = soc_mem_cache_set(unit, mem, COPYNO_ALL, enable);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;

}

int
dnxf_tbl_mem_cache_enable_parity_tbl(
    int unit,
    soc_mem_t mem,
    void *en)
{
    int rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If table has valid PARITY field - it should be cached 
     */
    if (SOC_MEM_FIELD_VALID(unit, mem, PARITYf))
        rc = dnxf_tbl_mem_cache_mem_set(unit, mem, en);

    LOG_INFO(BSL_LS_SOC_MEM, (BSL_META_U(unit, "parity memory %s cache\n"), SOC_MEM_NAME(unit, mem)));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;

}

int
dnxf_tbl_mem_cache_enable_ecc_tbl(
    int unit,
    soc_mem_t mem,
    void *en)
{
    int rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If table has valid ECC field - it should be cached 
     */
    if (SOC_MEM_FIELD_VALID(unit, mem, ECCf))
        rc = dnxf_tbl_mem_cache_mem_set(unit, mem, en);

    LOG_INFO(BSL_LS_SOC_MEM, (BSL_META_U(unit, "ecc memory %s cache\n"), SOC_MEM_NAME(unit, mem)));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_cache_enable_init(
    int unit)
{
    uint32 cache_enable = 1;
    int enable_all, enable_parity, enable_ecc;
    SHR_FUNC_INIT_VARS(unit);

    enable_all = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "all", 0);
    enable_parity = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "parity", 1);
    enable_ecc = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "ecc", 1);

    if (enable_all)
    {
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_mem_set, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM,
                      (BSL_META_U(unit, "soc_dpe_cache_enable_init: unit %d all_cache enable failed\n"), unit));
    }

    if (enable_parity)
    {
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_enable_parity_tbl, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM,
                      (BSL_META_U(unit, "soc_dnxf_cache_enable_init: unit %d parity cache enable failed\n"), unit));
    }

    if (enable_ecc)
    {
        if (soc_mem_iterate(unit, dnxf_tbl_mem_cache_enable_ecc_tbl, &cache_enable) < 0)
            LOG_ERROR(BSL_LS_APPL_SOCMEM,
                      (BSL_META_U(unit, "soc_dnxf_cache_enable_init: unit %d ecc cache enable failed\n"), unit));
    }

    SHR_FUNC_EXIT;
}

int
soc_dnxf_init_reset(
    int unit)
{
    int rc = _SHR_E_NONE;
    soc_control_t *soc;
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    soc = SOC_CONTROL(unit);

    /*
     * CMIC endianess
     */
    soc_endian_config(unit);

    /*
     * properties init
     */
    rc = soc_dnxf_info_soc_properties(unit);
    SHR_IF_ERR_EXIT(rc);

    /*
     * mark cacheble tables 
     */
    soc_dnxf_tbl_mark_cachable(unit);

    rc = soc_dnxf_cache_enable_init(unit);
    SHR_IF_ERR_EXIT(rc);

    /*
     * warm boot mechanism init
     */
    rc = soc_dnxf_warm_boot_init(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_linkctrl_init);
    SHR_IF_ERR_EXIT(rc);

    /*
     * Reset
     */
    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {
        SOC_DNXF_DRV_INIT_LOG(unit, "Device Reset");
        rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_reset_device);
        SHR_IF_ERR_EXIT(rc);

    }
    else
    {
        /*
         * Interrupts init need to be called if it skiped because of WB if above
         */
#ifdef BCM_WARM_BOOT_SUPPORT
        uint32 value;
        /*
         * Warm boot buffer recovery
         */
        SHR_IF_ERR_EXIT(dnxf_state.intr.storm_nominal.get(unit, &value));
        soc->switch_event_nominal_storm = value;
#endif /* BCM_WARM_BOOT_SUPPORT */
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_interrupts_init, (unit));
        SHR_IF_ERR_CONT(rc);
    }

    /*
     * counter init
     */
    rc = soc_counter_attach(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_port_soc_init);
    SHR_IF_ERR_EXIT(rc);

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {
        /*
         * need to be at the end of soc_init in order to prevent ecc errors from un initialized memories 
         */
        rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_ser_init);
        SHR_IF_ERR_EXIT(rc);
    }

    soc->soc_flags |= SOC_F_INITED;

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    if (SHR_FAILURE(_func_rv))
    {
        soc_dnxf_deinit(unit);
    }
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Soft reset should be done only when there is no traffic, and links
 *   are down.
 *
 *   To Shutdown:
 *   1) Isolate the device.
 *   2) Disable links.
 *
 *   To Power Up:
 *   1) Enable links.
 *   2) Unisolate the device (if needed).
 */
static int
soc_dnxf_shutdown_set(
    int unit,
    soc_pbmp_t active_links,
    int shutdown,
    int isolate_device)
{
    int rv;
    soc_port_t port;
    SHR_FUNC_INIT_VARS(unit);

    if (shutdown)
    {

        /*
         * Isolate the device 
         */
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_isolate_set,
                                   (unit, soc_dnxc_isolation_status_isolated));
        SHR_IF_ERR_EXIT(rv);

        /*
         * Disable links 
         */
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_dnxc_port_enable_set(unit, port, 0);
            SHR_IF_ERR_EXIT(rv);
        }

        sal_usleep(50000);      /* sleep 50 mili sec */

    }
    else
    {   /* power up */

        /*
         * Enable link
         */
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_dnxc_port_enable_set(unit, port, 1);
            SHR_IF_ERR_EXIT(rv);
        }

        sal_usleep(500000);     /* sleep 0.5 second */

        if (!isolate_device)
        {
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_isolate_set,
                                       (unit, soc_dnxc_isolation_status_active));
            SHR_IF_ERR_EXIT(rv);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Perform Soft Reset.
 *
 *   1) Make sure there is no traffic and links are down (isolate the
 *      device and disable links).
 *   2) Perform Soft Reset.
 *   3) Enable links and unisolate the device (if needed).
 */
int
soc_dnxf_init_no_reset(
    int unit)
{
    int rc = _SHR_E_NONE;
    soc_control_t *soc;
    DNXF_UNIT_LOCK_INIT(unit);

    soc_pbmp_t pbmp_enabled;
    soc_port_t port;
    int enable;
    soc_dnxc_isolation_status_t device_isolation_status;

    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    soc = SOC_CONTROL(unit);

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {
        SOC_PBMP_CLEAR(pbmp_enabled);
        SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
        {
            rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_enable_get, (unit, port, &enable));
            SHR_IF_ERR_EXIT(rc);
            if (enable)
            {
                SOC_PBMP_PORT_ADD(pbmp_enabled, port);
            }
        }

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_isolate_get, (unit, &device_isolation_status));
        SHR_IF_ERR_EXIT(rc);

        rc = soc_dnxf_shutdown_set(unit, pbmp_enabled, 1, 0);
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_soft_init, (unit, SOC_DNXC_RESET_ACTION_INOUT_RESET));
        SHR_IF_ERR_EXIT(rc);

        sal_usleep(10000);      /* wait 10 mili sec */

        rc = soc_dnxf_shutdown_set(unit, pbmp_enabled, 0,
                                   device_isolation_status == soc_dnxc_isolation_status_isolated ? 1 : 0);
        SHR_IF_ERR_EXIT(rc);
    }

    soc->soc_flags |= SOC_F_INITED;

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

int
soc_dnxf_init(
    int unit,
    int reset)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    if (reset)
    {
        /*
        * Update dnxf_data values after the rc_load
        */
        dnxc_data_mgmt_values_set(unit);

        rc = soc_dnxf_init_reset(unit);
        SHR_IF_ERR_EXIT(rc);
    }
    else
    {
        rc = soc_dnxf_init_no_reset(unit);
        SHR_IF_ERR_EXIT(rc);
    }

exit:

    SHR_FUNC_EXIT;
}

soc_driver_t *
soc_dnxf_chip_driver_find(
    int unit,
    uint16 pci_dev_id,
    uint8 pci_rev_id)
{
    uint16 driver_dev_id;
    uint8 driver_rev_id;
    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id) < 0)
    {
        return NULL;
    }

    if (SOC_IS_RAMON_TYPE(driver_dev_id))
    {
        return &soc_driver_bcm88790_a0;
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_dnxf_chip_driver_find: driver in devid table "
                              "not in soc_base_driver_table\n")));
    }

    return NULL;
}

int
soc_dnxf_info_config(
    int unit,
    int dev_id)
{
    soc_info_t *si;
    soc_control_t *soc;
    int mem, blk, blktype;
    char instance_string[3];
    int rv, port, phy_port, bindex;

    SHR_FUNC_INIT_VARS(unit);
    soc = SOC_CONTROL(unit);

    /*
     * set chip string
     */
    if (SOC_IS_RAMON_TYPE(dev_id))
    {
        SOC_CHIP_STRING(unit) = "ramon";
    } 
    else
    {
            SOC_CHIP_STRING(unit) = "???";
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit, "soc_dnxf_info_config: driver device %04x unexpected\n"), dev_id));
            SHR_ERR_EXIT(_SHR_E_UNIT, "failed to find device id");
    }

    si = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    SHR_IF_ERR_CONT(soc_dnxf_defines_init(unit));

    SHR_IF_ERR_CONT(soc_dnxf_implementation_defines_init(unit));

    si->fe.min = si->fe.max = -1;
    si->fe.num = 0;
    si->ge.min = si->ge.max = -1;
    si->ge.num = 0;
    si->xe.min = si->xe.max = -1;
    si->xe.num = 0;
    si->hg.min = si->hg.max = -1;
    si->hg.num = 0;
    si->hg_subport.min = si->hg_subport.max = -1;
    si->hg_subport.num = 0;
    si->hl.min = si->hl.max = -1;
    si->hl.num = 0;
    si->st.min = si->st.max = -1;
    si->st.num = 0;
    si->gx.min = si->gx.max = -1;
    si->gx.num = 0;
    si->xg.min = si->xg.max = -1;
    si->xg.num = 0;
    si->spi.min = si->spi.max = -1;
    si->spi.num = 0;
    si->spi_subport.min = si->spi_subport.max = -1;
    si->spi_subport.num = 0;
    si->sci.min = si->sci.max = -1;
    si->sci.num = 0;
    si->sfi.min = si->sfi.max = -1;
    si->sfi.num = 0;
    si->port.min = si->port.max = -1;
    si->port.num = 0;
    si->ether.min = si->ether.max = -1;
    si->ether.num = 0;
    si->all.min = si->all.max = -1;
    si->all.num = 0;

    si->port_num = 0;

    sal_memset(si->has_block, 0, sizeof(soc_block_t) * COUNTOF(si->has_block));

    for (blk = 0; blk < SOC_MAX_NUM_BLKS; blk++)
    {
        si->block_port[blk] = REG_PORT_ANY;
        si->block_valid[blk] = 0;
    }

    SOC_PBMP_CLEAR(si->cmic_bitmap);

    si->cmic_block = -1;

    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
    {
        blktype = SOC_BLOCK_INFO(unit, blk).type;

        if (blk >= SOC_MAX_NUM_BLKS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much blocks for device \n");
        }
        si->has_block[blk] = blktype;
        sal_snprintf(instance_string, sizeof(instance_string), "%d", SOC_BLOCK_INFO(unit, blk).number);

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_block_valid_get,
                                   (unit, blktype, SOC_BLOCK_INFO(unit, blk).number, &(si->block_valid[blk])));
        SHR_IF_ERR_EXIT(rv);

        switch (blktype)
        {
            case SOC_BLK_ECI:
                si->eci_block = blk;
                break;
            case SOC_BLK_MESH_TOPOLOGY:
                si->mesh_topology_block = blk;
                break;
            case SOC_BLK_CMIC:
                si->cmic_block = blk;
                break;
            case SOC_BLK_IPROC:
                si->iproc_block = blk;
                break;
            case SOC_BLK_FMAC:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_FMAC_BLKS)
                {
                    si->fmac_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much FMAC blocks");
                }
                break;
            case SOC_BLK_FSRD:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_FSRD_BLKS)
                {
                    si->fsrd_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much FSRD blocks");
                }
                break;
            case SOC_BLK_RTP:
                si->rtp_block = blk;
                break;
            case SOC_BLK_OCCG:
                si->occg_block = blk;
                break;
            case SOC_BLK_DCH:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCH_BLKS)
                {
                    si->dch_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much DCH blocks");
                }
                break;
            case SOC_BLK_DCML:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCML_BLKS)
                {
                    si->dcml_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much DCML blocks");
                }
                break;
            case SOC_BLK_MCT:
                si->mct_block = blk;
                break;
            case SOC_BLK_QRH:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_QRH_BLKS)
                {
                    si->qrh_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much QRH blocks");
                }
                break;
            case SOC_BLK_CCH:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_CCH_BLKS)
                {
                    si->cch_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much CCH blocks");
                }
                break;
            case SOC_BLK_LCM:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_LCM_BLKS)
                {
                    si->lcm_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: too much LCM blocks");
                }
                break;
            case SOC_BLK_BRDC_FSRD:
                si->brdc_fsrd_block = blk;
                break;
            case SOC_BLK_BRDC_DCH:
                si->brdc_dch_block = blk;
                break;
            case SOC_BLK_BRDC_FMAC:
                si->brdc_fmac_block = blk;
                break;
            case SOC_BLK_BRDC_CCH:
                si->brdc_cch_block = blk;
                break;
            case SOC_BLK_BRDC_DCML:
                si->brdc_dcml_block = blk;
                break;
            case SOC_BLK_BRDC_LCM:
                si->brdc_lcm_block = blk;
                break;
            case SOC_BLK_BRDC_QRH:
                si->brdc_qrh_block = blk;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_info_config: unknown block type");
                break;
        }

        sal_snprintf(si->block_name[blk], sizeof(si->block_name[blk]),
                     "%s%s", soc_block_name_lookup_ext(blktype, unit), instance_string);
    }
    si->block_num = blk;

    /*
     * Calculate the mem_block_any array for this configuration
     * The "any" block is just the first one enabled
     */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        si->mem_block_any[mem] = -1;
        if (SOC_MEM_IS_VALID(unit, mem))
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk)
            {
                si->mem_block_any[mem] = blk;
                break;
            }
        }
    }

    for (phy_port = 0;; phy_port++)
    {

        blk = SOC_DRIVER(unit)->port_info[phy_port].blk;
        bindex = SOC_DRIVER(unit)->port_info[phy_port].bindex;
        if (blk < 0 && bindex < 0)
        {       /* end of list */
            break;
        }

        port = phy_port;

        if (blk < 0)
        {       /* empty slot */
            blktype = 0;
        }
        else
        {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
        }

        if (blktype == 0)
        {
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]), "sfi%d", port);
            si->port_offset[port] = port;
            continue;
        }

        switch (blktype)
        {
            case SOC_BLK_CMIC:
                si->cmic_port = port;
                sal_sprintf(SOC_PORT_NAME(unit, port), "CMIC");
                SOC_PBMP_PORT_ADD(si->cmic_bitmap, port);
                break;
            default:
                si->port_num_lanes[port] = 1;
                sal_sprintf(SOC_PORT_NAME(unit, port), "sfi%d", port);
                sal_sprintf(SOC_PORT_NAME_ALTER(unit, port), "fabric%d", port);
                SOC_PORT_NAME_ALTER_VALID(unit, port) = 1;
                DNXF_ADD_PORT(sfi, port);
                DNXF_ADD_PORT(port, port);
                DNXF_ADD_PORT(all, port);
                break;
        }

        si->port_type[phy_port] = blktype;

    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_detach(
    int unit)
{
    soc_control_t *soc;
    int mem;
    int cmc;
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        SHR_EXIT();
    }

    if (soc->miimMutex)
    {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    if (soc->counterMutex)
    {
        sal_mutex_destroy(soc->counterMutex);
        soc->counterMutex = NULL;
    }

    if (soc->schanMutex)
    {
        sal_mutex_destroy(soc->schanMutex);
        soc->schanMutex = NULL;
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++)
    {
        if (soc->schanIntr[cmc])
        {
            sal_sem_destroy(soc->schanIntr[cmc]);
            soc->schanIntr[cmc] = NULL;
        }
    }

    /*
     * De-Initialize DMA 
     */
    if ((soc_mem_dmaable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm)) || soc_mem_slamable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm))) &&  /* check 
                                                                                                                                                                                                                                         * if 
                                                                                                                                                                                                                                         * DMA 
                                                                                                                                                                                                                                         * is 
                                                                                                                                                                                                                                         * enabled 
                                                                                                                                                                                                                                         */
        soc_feature(unit, soc_feature_sbusdma))
    {

        SHR_IF_ERR_EXIT(sand_deinit_fill_table(unit));

        SHR_IF_ERR_EXIT(soc_dma_detach(unit));
    }
    (void) soc_sbusdma_lock_deinit(unit);
    /*
     * Memory mutex release
     */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (SOC_MEM_IS_VALID(unit, mem))
        {
            /*
             * Mutexes created only for valid memories. 
             */
            if (soc->memState[mem].lock)
            {
                sal_mutex_destroy(soc->memState[mem].lock);
                soc->memState[mem].lock = NULL;
            }
        }
    }

    if (SOC_PERSIST(unit))
    {
        sal_free(SOC_PERSIST(unit));
        SOC_PERSIST(unit) = NULL;
    }

    if (soc->socControlMutex)
    {
        sal_mutex_destroy(soc->socControlMutex);
        soc->socControlMutex = NULL;
    }

    if (_soc_dnxf_lock[unit] != NULL)
    {
        sal_mutex_destroy(_soc_dnxf_lock[unit]);
        _soc_dnxf_lock[unit] = NULL;
    }
    if (soc->schan_wb_mutex != NULL)
    {
        sal_mutex_destroy(soc->schan_wb_mutex);
        soc->schan_wb_mutex = NULL;
    }

    
    rc = soc_dnxf_implementation_defines_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    rc = soc_dnxf_defines_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    /*
     * dnxf data init
     */
    rc = dnxf_data_deinit(unit);
    SHR_IF_ERR_EXIT(rc);

    if (SOC_CONTROL(unit)->drv != NULL)
    {
        sal_free((soc_dnxf_control_t *) SOC_CONTROL(unit)->drv);
        SOC_CONTROL(unit)->drv = NULL;
    }

    sal_free(soc);
    SOC_CONTROL(unit) = NULL;

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_chip_type_set(
    int unit,
    uint16 dev_id)
{
    soc_info_t *si;
    SHR_FUNC_INIT_VARS(unit);
    si = &SOC_INFO(unit);

    /*
     * Used to implement the SOC_IS_*(unit) macros
     */
     if (SOC_IS_RAMON_TYPE(dev_id))
     {
        si->chip_type = SOC_INFO_CHIP_TYPE_RAMON;
     } else {
        si->chip_type = 0;

        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit, "soc_dnxf_chip_type_set: driver device %04x unexpected\n"), dev_id));
        SHR_ERR_EXIT(_SHR_E_UNIT, "failed to find device id");
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_attach(
    int unit)
{
    soc_control_t *soc;
    soc_persist_t *sop;
    soc_info_t *si;
    uint16 dev_id;
    uint8 rev_id;
    int rc = _SHR_E_NONE, mem;
    int cmc;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate soc_control and soc_persist if not already. 
     */
    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        soc = sal_alloc(sizeof(soc_control_t), "soc_control");
        if (soc == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate soc_control");
        }
        sal_memset(soc, 0, sizeof(soc_control_t));
        SOC_CONTROL(unit) = soc;
    }
    else
    {
        if (soc->soc_flags & SOC_F_ATTACHED)
        {
            SHR_EXIT();
        }
    }

    soc->soc_link_pause = 0;

    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);

#ifdef BCM_SBUSDMA_SUPPORT
    SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DNXC_MAX_SBUSDMA_CHANNELS;
    SOC_CONTROL(unit)->tdma_ch = SOC_DNXC_TDMA_CHANNEL;
    SOC_CONTROL(unit)->tslam_ch = SOC_DNXC_TSLAM_CHANNEL;
    SOC_CONTROL(unit)->desc_ch = SOC_DNXC_DESC_CHANNEL;
    /*
     * maximum possible memory entry size used for clearing memory, should be a multiple of 32bit words, 
     */
    SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, soc_property_get(unit, spn_MEM_CLEAR_CHUNK_SIZE, SOC_DNXC_MEM_CLEAR_CHUNK_SIZE));
#endif

    /*
     * Abort and disable previous DMA operations 
     */
    SHR_IF_ERR_EXIT(soc_cmicx_dma_abort(unit, 0));

    /*
     * Create mutexes and semaphores.
     */

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate MIIM lock");
    }

    if (_soc_dnxf_lock[unit] == NULL)
    {
        if ((_soc_dnxf_lock[unit] = sal_mutex_create("bcm_dnxf_config_lock")) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate DNXF lock (_soc_dnxf_lock)");
        }
    }

    if ((soc->socControlMutex = sal_mutex_create("SOC_CONTROL")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate socControlMutex");
    }

    soc->counterMutex = sal_mutex_create("Counter");
    if (soc->counterMutex == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate counterMutex");
    }

    soc->schanMutex = sal_mutex_create("SCHAN");
    if (soc->schanMutex == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate schanMutex");
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++)
    {
        if ((soc->schanIntr[cmc] = sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate schanSem");
        }
    }

    SOC_PERSIST(unit) = sal_alloc(sizeof(soc_persist_t), "soc_persist");
    if (SOC_PERSIST(unit) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate soc_persist");
    }
    sal_memset(SOC_PERSIST(unit), 0, sizeof(soc_persist_t));
    sop = SOC_PERSIST(unit);
    sop->version = 1;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    SHR_IF_ERR_EXIT(soc_dnxf_chip_type_set(unit, dev_id));

    /*
     * Instantiate the driver -- Verify chip revision matches driver compilation revision. 
     */
    soc->chip_driver = soc_dnxf_chip_driver_find(unit, dev_id, rev_id);
    if (soc->chip_driver == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "unit has no driver (device 0x%04x rev 0x%02x)", dev_id, rev_id);
    }

    /*
     * feature init
     */
    soc_feature_init(unit);

    si = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    /*
     * Required In order to read soc properties
     */
    soc->soc_flags |= SOC_F_ATTACHED;

    /*
     * dnxf data init
     */
    rc = dnxf_data_init(unit);
    SHR_IF_ERR_EXIT(rc);

    /*
     * Must call mbcm init first to ensure driver properly installed 
     */
    rc = mbcm_dnxf_init(unit);
    if (rc != _SHR_E_NONE)
    {
        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "soc_dnxf_init error in mbcm_dnxf_init\n")));
    }
    SHR_IF_ERR_EXIT(rc);

    /*
     * allocate counter module resources 
     */
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_controlled_counter_set, (unit));
    SHR_IF_ERR_EXIT(rc);

    /*
     * dnxf info config
     */
    /*
     * should be at the attach function to enable register access without chip init
     */
    rc = soc_dnxf_control_init(unit);
    SHR_IF_ERR_EXIT(rc);

    /*
     * Required In order to read soc properties
     */
    soc->soc_flags |= SOC_F_ATTACHED;

    /*
     * init interrupt 
     */
    rc = soc_dnxc_intr_init(unit);
    SHR_IF_ERR_EXIT(rc);

    /*
     * DMA
     */
    rc = soc_sbusdma_lock_init(unit);
    if (rc != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(rc, "failed to Initialize SBUSDMA Locks");
    }
    /*
     * Initialize DMA 
     */
    if ((soc_mem_dmaable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm)) || soc_mem_slamable(unit, RTP_MULTI_CAST_TABLE_UPDATEm, SOC_MEM_BLOCK_ANY(unit, RTP_MULTI_CAST_TABLE_UPDATEm))) &&  /* check 
                                                                                                                                                                                                                                         * if 
                                                                                                                                                                                                                                         * DMA 
                                                                                                                                                                                                                                         * is 
                                                                                                                                                                                                                                         * enabled 
                                                                                                                                                                                                                                         */
        soc_feature(unit, soc_feature_sbusdma))
    {

        /*
         * Setup DMA structures when a device is attached 
         */
        SHR_IF_ERR_EXIT(soc_dma_attach(unit, 1 /* Reset */ ));

        /*
         * Initialize TABLEDMA/ SLAMDMA /SBUSDMA Locks 
         */

        rc = soc_sbusdma_init(unit, soc_property_get(unit, spn_DMA_DESC_TIMEOUT_USEC, 0),
                              soc_property_get(unit, spn_DMA_DESC_INTR_ENABLE, 0));
        if (rc != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(rc, "failed to Initialize SBUSDMA");
        }
        SHR_IF_ERR_EXIT(sand_init_fill_table(unit));

    }

    /*
     * Initialize memory index_maxes. Chip specific overrides follow.
     */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (SOC_MEM_IS_VALID(unit, mem))
        {
            sop->memState[mem].index_max = SOC_MEM_INFO(unit, mem).index_max;
            /*
             * should only create mutexes for valid memories. 
             */
            if ((soc->memState[mem].lock = sal_mutex_create(SOC_MEM_NAME(unit, mem))) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memState lock");
            }

            /*
             * Set cache copy pointers to NULL 
             */
            sal_memset(soc->memState[mem].cache, 0, sizeof(soc->memState[mem].cache));
        }
        else
        {
            sop->memState[mem].index_max = -1;
        }
    }

    if ((soc->schan_wb_mutex = sal_mutex_create("SchanWB")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate SchanWB");
    }

    /*
     * Initialize SCHAN 
     */
    rc = soc_schan_init(unit);
    if (rc != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(rc, "failed to Initialize SCHAN");
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        soc_iproc_m0ssq_reset(unit, 1);
    }
#endif

exit:
    if (SHR_FAILURE(_func_rv))
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "soc_dnxf_attach: unit %d failed (%s)\n"), unit, soc_errmsg(rc)));
        soc_dnxf_detach(unit);
    }

    SHR_FUNC_EXIT;
}

int
soc_dnxf_dump(
    int unit,
    char *pfx)
{
    soc_control_t *soc;
    soc_persist_t *sop;
    soc_stat_t *stat;
    uint16 dev_id;
    uint8 rev_id;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    soc = SOC_CONTROL(unit);
    sop = SOC_PERSIST(unit);

    stat = &soc->stat;

    LOG_CLI((BSL_META_U(unit, "%sUnit %d Driver Control Structure:\n"), pfx, unit));

    soc_cm_get_id(unit, &dev_id, &rev_id);

    LOG_CLI((BSL_META_U(unit,
                        "%sChip=%s Rev=0x%02x Driver=%s\n"),
             pfx, soc_dev_name(unit), rev_id, SOC_CHIP_NAME(soc->chip_driver->type)));
    LOG_CLI((BSL_META_U(unit, "%sFlags=0x%x:"), pfx, soc->soc_flags));
    if (soc->soc_flags & SOC_F_ATTACHED)
    {
        LOG_CLI((BSL_META_U(unit, " attached")));
    }
    if (soc->soc_flags & SOC_F_INITED)
    {
        LOG_CLI((BSL_META_U(unit, " initialized")));
    }
    if (soc->soc_flags & SOC_F_LSE)
    {
        LOG_CLI((BSL_META_U(unit, " link-scan")));
    }
    if (soc->soc_flags & SOC_F_SL_MODE)
    {
        LOG_CLI((BSL_META_U(unit, " sl-mode")));
    }
    if (soc->soc_flags & SOC_F_POLLED)
    {
        LOG_CLI((BSL_META_U(unit, " polled")));
    }
    if (soc->soc_flags & SOC_F_URPF_ENABLED)
    {
        LOG_CLI((BSL_META_U(unit, " urpf")));
    }
    if (soc->soc_flags & SOC_F_MEM_CLEAR_USE_DMA)
    {
        LOG_CLI((BSL_META_U(unit, " mem-clear-use-dma")));
    }
    if (soc->soc_flags & SOC_F_IPMCREPLSHR)
    {
        LOG_CLI((BSL_META_U(unit, " ipmc-repl-shared")));
    }
    if (soc->remote_cpu)
    {
        LOG_CLI((BSL_META_U(unit, " rcpu")));
    }
    LOG_CLI((BSL_META_U(unit, "; board type 0x%x"), soc->board_type));
    LOG_CLI((BSL_META_U(unit, "\n")));
    LOG_CLI((BSL_META_U(unit, "%s"), pfx));
    soc_cm_dump(unit);

    LOG_CLI((BSL_META_U(unit,
                        "%sDisabled: reg_flags=0x%x mem_flags=0x%x\n"),
             pfx, soc->disabled_reg_flags, soc->disabled_mem_flags));
    LOG_CLI((BSL_META_U(unit,
                        "%sSchanOps=%d MMUdbg=%d LinkPause=%d\n"),
             pfx, stat->schan_op, sop->debugMode, soc->soc_link_pause));
    LOG_CLI((BSL_META_U(unit,
                        "%sCounter: int=%dus per=%dus dmaBuf=%p\n"),
             pfx,
             soc->counter_interval,
             SAL_USECS_SUB(soc->counter_coll_cur, soc->counter_coll_prev), (void *) soc->counter_buf32));
    LOG_CLI((BSL_META_U(unit,
                        "%sTimeout: Schan=%d(%dus) MIIM=%d(%dus)\n"),
             pfx, stat->err_sc_tmo, soc->schanTimeout, stat->err_mii_tmo, soc->miimTimeout));
    LOG_CLI((BSL_META_U(unit,
                        "%sIntr: Total=%d Sc=%d ScErr=%d MMU/ARLErr=%d\n"
                        "%s      LinkStat=%d PCIfatal=%d PCIparity=%d\n"
                        "%s      ARLdrop=%d ARLmbuf=%d ARLxfer=%d ARLcnt0=%d\n"
                        "%s      TableDMA=%d TSLAM-DMA=%d\n"
                        "%s      MemCmd[BSE]=%d MemCmd[CSE]=%d MemCmd[HSE]=%d\n"
                        "%s      ChipFunc[0]=%d ChipFunc[1]=%d ChipFunc[2]=%d\n"
                        "%s      ChipFunc[3]=%d ChipFunc[4]=%d\n"
                        "%s      I2C=%d MII=%d StatsDMA=%d Desc=%d Chain=%d\n"),
             pfx, stat->intr, stat->intr_sc, stat->intr_sce, stat->intr_mmu,
             pfx, stat->intr_ls,
             stat->intr_pci_fe, stat->intr_pci_pe,
             pfx, stat->intr_arl_d, stat->intr_arl_m,
             stat->intr_arl_x, stat->intr_arl_0,
             pfx, stat->intr_tdma, stat->intr_tslam,
             pfx, stat->intr_mem_cmd[0],
             stat->intr_mem_cmd[1], stat->intr_mem_cmd[2],
             pfx, stat->intr_chip_func[0], stat->intr_chip_func[1],
             stat->intr_chip_func[2],
             pfx, stat->intr_chip_func[3], stat->intr_chip_func[4],
             pfx, stat->intr_i2c, stat->intr_mii, stat->intr_stats, stat->intr_desc, stat->intr_chain));
exit:
    SHR_FUNC_EXIT;
}

void
soc_dnxf_chip_dump(
    int unit,
    soc_driver_t * d)
{
    soc_info_t *si;
    int i, count = 0;
    soc_port_t port;
    char pfmt[SOC_PBMP_FMT_LEN];
    uint16 dev_id;
    uint8 rev_id;
    int blk, bindex;
    char instance_string[3], block_name[14];
    DNXC_LEGACY_FIXME_ASSERT;

    if (d == NULL)
    {
        LOG_CLI((BSL_META_U(unit, "unit %d: no driver attached\n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit, "driver %s (%s)\n"), SOC_CHIP_NAME(d->type), d->chip_string));
    LOG_CLI((BSL_META_U(unit, "\tregsfile\t\t%s\n"), d->origin));
    LOG_CLI((BSL_META_U(unit,
                        "\tpci identifier\t\tvendor 0x%04x device 0x%04x rev 0x%02x\n"),
             d->pci_vendor, d->pci_device, d->pci_revision));
    LOG_CLI((BSL_META_U(unit, "\tclasses of service\t%d\n"), d->num_cos));
    LOG_CLI((BSL_META_U(unit,
                        "\tmaximums\t\tblock %d ports %d mem_bytes %d\n"),
             SOC_MAX_NUM_BLKS, SOC_MAX_NUM_PORTS, SOC_MAX_MEM_BYTES));

    if (unit < 0)
    {
        return;
    }
    si = &SOC_INFO(unit);
    for (blk = 0; d->block_info[blk].type >= 0; blk++)
    {
        sal_snprintf(instance_string, sizeof(instance_string), "%d", d->block_info[blk].number);
        if (d->block_info[blk].type == SOC_BLK_PORT_GROUP4 || d->block_info[blk].type == SOC_BLK_PORT_GROUP5)
        {
            sal_strncpy(instance_string, d->block_info[blk].number ? "_y" : "_x", 2);
            instance_string[2] = '\0';
        }
        sal_snprintf(block_name, sizeof(block_name), "%s%s",
                     soc_block_name_lookup_ext(d->block_info[blk].type, unit), instance_string);
        LOG_CLI((BSL_META_U(unit,
                            "\tblk %d\t\t%-14s schan %d cmic %d\n"),
                 blk, block_name, d->block_info[blk].schan, d->block_info[blk].cmic));
    }
    for (port = 0;; port++)
    {
        blk = d->port_info[port].blk;
        bindex = d->port_info[port].bindex;
        if (blk < 0 && bindex < 0)
        {       /* end of list */
            break;
        }
        if (blk < 0)
        {       /* empty slot */
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "\tport %d\t\t%s\tblk %d %s%d.%d\n"),
                 soc_feature(unit, soc_feature_logical_port_num) ?
                 si->port_p2l_mapping[port] : port,
                 soc_block_port_name_lookup_ext(d->block_info[blk].type, unit),
                 blk, soc_block_name_lookup_ext(d->block_info[blk].type, unit), d->block_info[blk].number, bindex));
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    LOG_CLI((BSL_META_U(unit, "unit %d:\n"), unit));
    LOG_CLI((BSL_META_U(unit, "\tpci\t\t\tdevice %04x rev %02x\n"), dev_id, rev_id));
    LOG_CLI((BSL_META_U(unit,
                        "\tdriver\t\t\ttype %d (%s) group %d (%s)\n"),
             si->driver_type, SOC_CHIP_NAME(si->driver_type),
             si->driver_group, soc_chip_group_names[si->driver_group]));
    LOG_CLI((BSL_META_U(unit, "\tchip\t\t\t\n")));
    LOG_CLI((BSL_META_U(unit,
                        "\tGE ports\t%d\t%s (%d:%d)\n"),
             si->ge.num, SOC_PBMP_FMT(si->ge.bitmap, pfmt), si->ge.min, si->ge.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tXE ports\t%d\t%s (%d:%d)\n"),
             si->xe.num, SOC_PBMP_FMT(si->xe.bitmap, pfmt), si->xe.min, si->xe.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tHG ports\t%d\t%s (%d:%d)\n"),
             si->hg.num, SOC_PBMP_FMT(si->hg.bitmap, pfmt), si->hg.min, si->hg.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tST ports\t%d\t%s (%d:%d)\n"),
             si->st.num, SOC_PBMP_FMT(si->st.bitmap, pfmt), si->st.min, si->st.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tETHER ports\t%d\t%s (%d:%d)\n"),
             si->ether.num, SOC_PBMP_FMT(si->ether.bitmap, pfmt), si->ether.min, si->ether.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tPORT ports\t%d\t%s (%d:%d)\n"),
             si->port.num, SOC_PBMP_FMT(si->port.bitmap, pfmt), si->port.min, si->port.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tALL ports\t%d\t%s (%d:%d)\n"),
             si->all.num, SOC_PBMP_FMT(si->all.bitmap, pfmt), si->all.min, si->all.max));
    LOG_CLI((BSL_META_U(unit, "\tIPIC port\t%d\tblock %d\n"), si->ipic_port, si->ipic_block));
    LOG_CLI((BSL_META_U(unit,
                        "\tCMIC port\t%d\t%s block %d\n"), si->cmic_port,
             SOC_PBMP_FMT(si->cmic_bitmap, pfmt), si->cmic_block));
    LOG_CLI((BSL_META_U(unit,
                        "\tother blocks\t\tARL %d MMU %d MCU %d\n"), si->arl_block, si->mmu_block, si->mcu_block));
    LOG_CLI((BSL_META_U(unit,
                        "\t            \t\tIPIPE %d IPIPE_HI %d EPIPE %d EPIPE_HI %d BSAFE %d ESM %d\n"),
             si->ipipe_block, si->ipipe_hi_block, si->epipe_block, si->epipe_hi_block, si->bsafe_block, si->esm_block));

    for (i = 0; i < COUNTOF(si->has_block); i++)
    {
        if (si->has_block[i])
        {
            count++;
        }
    }
    LOG_CLI((BSL_META_U(unit, "\thas blocks\t%d\t"), count));
    for (i = 0; i < COUNTOF(si->has_block); i++)
    {
        if (si->has_block[i])
        {
            LOG_CLI((BSL_META_U(unit, "%s "), soc_block_name_lookup_ext(si->has_block[i], unit)));
            if ((i) && !(i % 6))
            {
                LOG_CLI((BSL_META_U(unit, "\n\t\t\t\t")));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit, "\n")));
    LOG_CLI((BSL_META_U(unit, "\tport names\t\t")));
    for (port = 0; port < si->port_num; port++)
    {
        if (port > 0 && (port % 5) == 0)
        {
            LOG_CLI((BSL_META_U(unit, "\n\t\t\t\t")));
        }
        LOG_CLI((BSL_META_U(unit, "%d=%s\t"), port, si->port_name[port]));
    }
    LOG_CLI((BSL_META_U(unit, "\n")));
    i = 0;
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
    {
        if (SOC_PBMP_IS_NULL(si->block_bitmap[blk]))
        {
            continue;
        }
        if (++i == 1)
        {
            LOG_CLI((BSL_META_U(unit, "\tblock bitmap\t")));
        }
        else
        {
            LOG_CLI((BSL_META_U(unit, "\n\t\t\t")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%-2d  %-14s %s (%d ports)"),
                 blk, si->block_name[blk], SOC_PBMP_FMT(si->block_bitmap[blk], pfmt), si->block_valid[blk]));
    }
    if (i > 0)
    {
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    {
        soc_feature_t f;

        LOG_CLI((BSL_META_U(unit, "\tfeatures\t")));
        i = 0;
        for (f = 0; f < soc_feature_count; f++)
        {
            if (soc_feature(unit, f))
            {
                if (++i > 3)
                {
                    LOG_CLI((BSL_META_U(unit, "\n\t\t\t")));
                    i = 1;
                }
                LOG_CLI((BSL_META_U(unit, "%s "), soc_feature_name[f]));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }
}

int
soc_dnxf_nof_interrupts(
    int unit,
    int *nof_interrupts)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    SHR_NULL_CHECK(nof_interrupts, _SHR_E_PARAM, "nof_interrupts");

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_nof_interrupts, (unit, nof_interrupts));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_nof_block_instances(
    int unit,
    soc_block_types_t block_types,
    int *nof_block_instances)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    SHR_NULL_CHECK(nof_block_instances, _SHR_E_PARAM, "nof_block_instances");

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_nof_block_instance, (unit, block_types, nof_block_instances));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/*
 * soc_dnxf_deinit - 
 * This function main role is to resotore the status to be identical to the status after soc_dnxf_attach. 
 * Important! - update deinit function each time soc_dnxf_attach is updated. 
 */
int
soc_dnxf_deinit(
    int unit)
{
    int rc;
    soc_control_t *soc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);

    soc = SOC_CONTROL(unit);

    soc->soc_flags &= ~SOC_F_INITED;

    if (SOC_FAILURE(soc_linkctrl_deinit(unit)))
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Failed in soc_linkctrl_deinit\n")));
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        /* Iproc M0 exit */
        soc_iproc_m0_exit(unit);
    }
#endif /* BCM_CMICX_SUPPORT */
    /*
     * warmboot deinit
     */
    rc = soc_dnxf_warm_boot_deinit(unit);
    SHR_IF_ERR_CONT(rc);

    /*
     * Interrupts deinit
     */
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_interrupts_deinit, (unit));
    SHR_IF_ERR_CONT(rc);

    /*
     * counters detach
     */
    rc = soc_counter_detach(unit);
    if (SOC_FAILURE(rc))
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Failed to detach counter\n")));
    }

    /*
     * dnxf info config
     */
    /*
     * prepare config info for the next init sequnace
     */
    rc = soc_dnxf_control_init(unit);
    SHR_IF_ERR_CONT(rc);

exit:

    DNXF_UNIT_LOCK_RELEASE(unit);

    /*
     * inform that detaching device is done
     */
    if (SOC_UNIT_NUM_VALID(unit))
    {
        SOC_DETACH(unit, 0);
    }

    SHR_FUNC_EXIT;
}

int
soc_dnxf_compiler_64_div_32(
    uint64 x,
    uint32 y,
    uint32 *result)
{
    uint64 rem;
    uint64 b;
    uint64 res, d;
    uint32 high;

    COMPILER_64_SET(rem, COMPILER_64_HI(x), COMPILER_64_LO(x));
    COMPILER_64_SET(b, 0, y);
    COMPILER_64_SET(d, 0, 1);

    high = COMPILER_64_HI(rem);

    COMPILER_64_ZERO(res);
    if (high >= y)
    {
        /*
         * NOTE: Follow code is used to handle 64bits result high /= y; res = (uint64_t) (high << 32); rem -=
         * (uint64_t)((high * y) << 32); 
         */
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META("soc_dnxf_compiler_64_div_32: result > 32bits\n")));
        return _SHR_E_PARAM;
    }

    while ((!COMPILER_64_BITTEST(b, 63)) && (COMPILER_64_LT(b, rem)))
    {
        COMPILER_64_ADD_64(b, b);
        COMPILER_64_ADD_64(d, d);
    }

    do
    {
        if (COMPILER_64_GE(rem, b))
        {
            COMPILER_64_SUB_64(rem, b);
            COMPILER_64_ADD_64(res, d);
        }
        COMPILER_64_SHR(b, 1);
        COMPILER_64_SHR(d, 1);
    }
    while (!COMPILER_64_IS_ZERO(d));

    *result = COMPILER_64_LO(res);

    return _SHR_E_NONE;
}

/*
 * Memory bist test
 */
int
soc_dnxf_drv_mbist(
    int unit,
    int skip_errors)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_mbist, (unit, skip_errors));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_device_reset(
    int unit,
    int mode,
    int action)
{
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    switch (mode)
    {
        case SOC_DNXC_RESET_MODE_BLOCKS_RESET:
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_blocks_reset, (unit, action, NULL));
            SHR_IF_ERR_EXIT(rv);
            break;
        case SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET:
            rv = soc_dnxf_init(unit, 0);
            SHR_IF_ERR_EXIT(rv);
            break;
        case SOC_DNXC_RESET_MODE_INIT_RESET:
            rv = soc_dnxf_init(unit, 1);
            SHR_IF_ERR_EXIT(rv);
            break;
        case SOC_DNXC_RESET_MODE_REG_ACCESS:
            /*
             * read soc properties
             */
            rv = soc_dnxf_info_soc_properties(unit);
            SHR_IF_ERR_EXIT(rv);
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_reg_access_only_reset, (unit));
            SHR_IF_ERR_EXIT(rv);
            break;
        case SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET:
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_soft_init, (unit, action));
            SHR_IF_ERR_EXIT(rv);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown/Unsupported Reset Mode");
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
