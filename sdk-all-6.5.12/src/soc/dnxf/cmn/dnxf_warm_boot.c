/*
 * $Id: dnxf_port.c,v 1.13 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXF warm_boot
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/scache.h>
#include <soc/ha.h>

#include <sal/compiler.h>

#ifdef BCM_DNXF_SUPPORT

#include <soc/dnxc/legacy/error.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
/*dnxf sw state*/
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#define DNXF_VER(_ver) (_ver)


shr_error_e
soc_dnxf_warm_boot_sync(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Warm boot support requires compilation with warm boot flag.\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_warm_boot_deinit(int unit)
{
    int rv;
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    int stable_location;
    uint32 stable_flags;
#endif

    SHR_FUNC_INIT_VARS(unit);
    
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = soc_scache_detach(unit);
    SHR_IF_ERR_EXIT(rv);
#if !defined(__KERNEL__) && defined (LINUX)
    SHR_IF_ERR_EXIT(soc_stable_get(unit,&stable_location,&stable_flags));
    if (stable_location == 4) {
        rv = ha_destroy(unit, 0);
        SHR_IF_ERR_CONT(rv);
    }
#endif
#endif

    /* sw state memory free is done automatically at device deinit */

exit: 
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_warm_boot_init(int unit)
{
    int rv;
    uint32 mc_table_size;
#if defined(BCM_WARM_BOOT_SUPPORT) && !defined(__KERNEL__) && defined (LINUX)
    int stable_location;
    uint32 stable_flags;
    uint32 stable_size;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
#if !defined(__KERNEL__) && defined (LINUX)
    stable_size = soc_property_get(unit, spn_STABLE_SIZE, 100000000);

    /* create new file if not SOC_WARM_BOOT */
    SHR_IF_ERR_EXIT(soc_stable_get(unit,&stable_location,&stable_flags));
    if (stable_location == 4) {
        char *ha_name = soc_property_get_str(unit, "stable_filename");
        /* init HA only if stable location is shared memory (4) */
        SHR_IF_ERR_EXIT(ha_init(unit, 1 /* HA enabled */, ha_name, stable_size, SOC_WARM_BOOT(unit) ? 0 : 1));
    }
#endif

    /* Recover stored Level 2 Warm Boot cache */
    /* The stable and stable size must be selected first */
    if (SOC_WARM_BOOT(unit)) 
    {
        rv = soc_scache_recover(unit);
        if (SOC_FAILURE(rv)) 
        {
            /* Fall back to Level 1 Warm Boot recovery */
            SHR_IF_ERR_EXIT(soc_stable_size_set(unit, 0));
            SHR_IF_ERR_EXIT(soc_stable_set(unit, _SHR_SWITCH_STABLE_NONE, 0));
            /* Error report */
            SHR_IF_ERR_EXIT(soc_event_generate(unit, SOC_SWITCH_EVENT_STABLE_ERROR, 
                                    SOC_STABLE_CORRUPT,
                                    SOC_STABLE_FLAGS(unit), 0));
            LOG_VERBOSE(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Unit %d: Corrupt stable cache.\n"),
                                    unit));
        }
    }
#endif

    rv = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_multicast_table_size_get,(unit, &mc_table_size));
    SHR_IF_ERR_EXIT(rv);
    dnxf_state.init(unit);
    dnxf_state.modid.local_map.alloc(unit, SOC_DNXF_MODID_LOCAL_NOF(unit));
    dnxf_state.modid.group_map.alloc(unit, SOC_DNXF_MODID_GROUP_NOF(unit));
    dnxf_state.mc.id_map.alloc(unit, _shr_div32r(mc_table_size,32));
    dnxf_state.lb.group_to_first_link.alloc(unit, dnxf_data_fabric.topology.nof_local_modid_get(unit));
    dnxf_state.port.cl72_conf.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.phy_lane_config.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.port.is_connected_to_repeater.alloc_bitmap(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.intr.flags.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.intr.storm_timed_count.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.intr.storm_timed_period.alloc(unit, dnxf_data_device.interrupts.nof_interrupts_get(unit));
    dnxf_state.lane_map_db.lane2serdes.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    dnxf_state.lane_map_db.serdes2lane.alloc(unit, dnxf_data_port.general.nof_links_get(unit));
    SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.map_size.set(unit, dnxf_data_port.general.nof_links_get(unit)));

exit: 
    SHR_FUNC_EXIT;
}


#endif /*BCM_DNXF_SUPPORT*/
