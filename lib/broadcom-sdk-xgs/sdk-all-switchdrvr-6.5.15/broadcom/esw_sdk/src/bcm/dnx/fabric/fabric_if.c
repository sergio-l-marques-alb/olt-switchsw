/**
 * \file fabric_if.c $Id$ Fabric Ports Interfaces procedures for DNX. 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/common/link.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnxc/legacy/dnxc_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/legacy/mbcm.h>

#include <soc/portmod/portmod.h>
#include <phymod/phymod.h>

/** See .h file */
shr_error_e
dnx_fabric_if_port_first_link_in_fsrd_get(
    int unit,
    soc_port_t logical_port,
    int *is_first_link)
{
    bcm_port_t port;
    int fsrd_block, link = 0;
    int fabric_ports_counter = 0, ilkn_links_counter = 0, ilkn_links_counter_tmp = 0;
    pbmp_t mask_fsrd_fabric_ports_bmp, mask_fsrd_link_bmp;
    bcm_pbmp_t ilkn_links_bmp;
    uint32 nof_links_in_fsrd = 0;
    int is_over_fabric;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(mask_fsrd_link_bmp);
    BCM_PBMP_CLEAR(mask_fsrd_fabric_ports_bmp);

    nof_links_in_fsrd = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

    fsrd_block = SOC_DNX_FABRIC_PORT_TO_LINK(unit, logical_port) / nof_links_in_fsrd;

    /** create a mask for all the links in the fsrd*/
    for (link = fsrd_block * nof_links_in_fsrd; link < fsrd_block * nof_links_in_fsrd + nof_links_in_fsrd; link++)
    {
        /*
         * Set FSRD phy bitmap
         */
        BCM_PBMP_PORT_ADD(mask_fsrd_link_bmp, link);

        /*
         * Set FSRD fabric ports bitmap
         */
        BCM_PBMP_PORT_ADD(mask_fsrd_fabric_ports_bmp, SOC_DNX_FABRIC_LINK_TO_PORT(unit, link));
    }

    /*
     * Count the number of ILKN over fabric phys on FSRD
     */
    ilkn_links_counter = 0;
    PBMP_IL_ITER(unit, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
        if (is_over_fabric)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_links_bmp));
            BCM_PBMP_AND(ilkn_links_bmp, mask_fsrd_link_bmp);
            BCM_PBMP_COUNT(ilkn_links_bmp, ilkn_links_counter_tmp);
            ilkn_links_counter += ilkn_links_counter_tmp;
        }
    }

    /*
     * Count the number of links activated in that bmp,
     * not including the current logical_port
     */
    BCM_PBMP_AND(mask_fsrd_fabric_ports_bmp, PBMP_SFI_ALL(unit));
    BCM_PBMP_PORT_REMOVE(mask_fsrd_fabric_ports_bmp, logical_port);
    BCM_PBMP_COUNT(mask_fsrd_fabric_ports_bmp, fabric_ports_counter);

    if ((fabric_ports_counter != 0) || (ilkn_links_counter != 0))
    {
        *is_first_link = 0;
    }
    else
    {
        /*
         * all the ports in the FSRD (not including the current
         * logical_port) are off.
         */
        *is_first_link = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_port_probe(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * okay_pbmp)
{
    int counter_interval = 0;
    bcm_port_t logical_port = 0;
    uint32 counter_flags = 0;
    bcm_pbmp_t counter_pbmp;
    int broadcast_load_fabric = 0;
    phymod_firmware_load_method_t fw_load_method_fabric = phymodFirmwareLoadMethodNone;
    int is_init_sequence = 0;
    uint32 flags = 0;
    dnx_algo_port_fabric_add_t fabric_add_info;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*okay_pbmp);
    BCM_PBMP_CLEAR(counter_pbmp);

    is_init_sequence = !(dnx_init_is_init_done_get(unit));

    /*
     * Loading firmware can take relatively long time.
     * On init sequence the user probably want to load firmware for all
     * the FSRDs, which can take very long time.
     * If External Method is used, we can load firmware by broadcast, which
     * means that the firmware will be loaded to all FSRDs simultaneously.
     * In that case, we init the port the following way:
     * 1. Init the port until firmware load.
     * 2. Load the firmware to all FSRDs using broadcast.
     * 3. Resume init the port after firmware load.
     */
    if (is_init_sequence)
    {
        fw_load_method_fabric = dnx_data_port.static_add.fabric_fw_load_method_get(unit);
        if (fw_load_method_fabric == phymodFirmwareLoadMethodExternal)
        {
            broadcast_load_fabric = 1;
        }
    }

    if (broadcast_load_fabric)
    {
        /** Set flag to load port until firmware load */
        IMB_PORT_ADD_F_INIT_PASS1_SET(flags);
    }
    BCM_PBMP_ITER(pbmp, logical_port)
    {
        /*
         * Add Fabric port to port mgmt database.
         */
        sal_memset(&fabric_add_info, 0, sizeof(dnx_algo_port_fabric_add_t));
        fabric_add_info.link_id = SOC_DNX_FABRIC_PORT_TO_LINK(unit, logical_port);
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_add(unit, logical_port, &fabric_add_info));

        /*
         * Init the fabric port.
         */
        SHR_IF_ERR_EXIT(imb_port_add(unit, logical_port, flags));
    }

    /*
     * Update FSRD broadcast info.
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_brdc_fsrd_blk_id_set, (unit)));

    if (broadcast_load_fabric && !BCM_PBMP_IS_NULL(pbmp))
    {
        /*
         * Load fabric firmware
         */
        SHR_IF_ERR_EXIT(soc_dnxc_fabric_broadcast_firmware_loader(unit, &pbmp));
    }

    BCM_PBMP_ITER(pbmp, logical_port)
    {
        if (broadcast_load_fabric)
        {
            /*
             * Resume init the fabric port after firmware load.
             */
            IMB_PORT_ADD_F_INIT_PASS1_CLR(flags);
            IMB_PORT_ADD_F_INIT_PASS2_SET(flags);
            /*
             * Since it's the 2nd time we call imb_port_add for the port,
             * we want to skip the imb internal configurations.
             */
            IMB_PORT_ADD_F_SKIP_SETTINGS_SET(flags);
            SHR_IF_ERR_EXIT(imb_port_add(unit, logical_port, flags));
        }

        /*
         * Mark added fabric link as valid
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_add_process_done(unit, logical_port));

        BCM_PBMP_PORT_ADD(*okay_pbmp, logical_port);
    }

    /*
     * Init FMAC for the fabric link
     */
    BCM_PBMP_ITER(*okay_pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_link_mac_init, (unit, logical_port)));
    }

    if (!is_init_sequence)
    {
        /*
         * Update Counter thread
         */
        SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
        if (counter_interval > 0)
        {
            soc_counter_stop(unit);

            BCM_PBMP_ITER(*okay_pbmp, logical_port)
            {
                BCM_PBMP_PORT_ADD(counter_pbmp, logical_port);
            }

            SHR_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
        }
        else
        {
            soc_control_t *soc = SOC_CONTROL(unit);

            /** update counter bitmap directly */
            BCM_PBMP_ITER(*okay_pbmp, logical_port)
            {
                BCM_PBMP_PORT_ADD(soc->counter_pbmp, logical_port);
            }
        }

        /*
         * On init, each fabric link is configured to have llfc enabled.
         * So all the ports that are now probed should have the configurations
         * they would have get on init.
         */
        BCM_PBMP_ITER(*okay_pbmp, logical_port)
        {
            bcm_dnx_fabric_link_control_set(unit, logical_port, bcmFabricLLFControlSource, 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_if_port_detach(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * detached)
{
    shr_error_e rv;
    bcm_port_t logical_port = 0;
    int counter_interval = 0;
    uint32 counter_flags = 0;
    pbmp_t counter_pbmp;
    int counter_paused = 0, linkscan_paused = 0;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*detached);
    BCM_PBMP_CLEAR(counter_pbmp);

    /*
     * Mark as removed port
     */
    BCM_PBMP_ITER(pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_remove_process_start(unit, logical_port));
    }

    /*
     * Get counter status
     */
    SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

    /*
     * Pause counter thread
     */
    SHR_IF_ERR_EXIT(soc_counter_stop(unit));
    counter_paused = 1;

    /*
     * Remove the desired pbmp from counter_pbmp, so they won't exist in the
     * bitmap when we later call soc_counter_start().
     */
    BCM_PBMP_REMOVE(counter_pbmp, pbmp);

    /*
     * Pause linkscan thread
     */
    PBMP_ITER(pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_linkscan_set(unit, logical_port, BCM_LINKSCAN_MODE_NONE));
    }
    SHR_IF_ERR_EXIT(_bcm_linkscan_pause(unit));
    linkscan_paused = 1;

    /*
     * Remove port 
     */
    PBMP_ITER(pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(imb_port_remove(unit, logical_port));

        /** Remove from port mgmt */
        SHR_IF_ERR_EXIT(dnx_algo_port_remove(unit, logical_port));

        BCM_PBMP_PORT_ADD(*detached, logical_port);
    }
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_brdc_fsrd_blk_id_set, (unit)));

    /*
     * Continue linkscan thread
     */
    rv = _bcm_linkscan_continue(unit);
    linkscan_paused = 0;
    SHR_IF_ERR_EXIT(rv);

    /*
     * Continue counter thread
     */
    rv = soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp);
    counter_paused = 0;
    SHR_IF_ERR_EXIT(rv);

exit:
    if (linkscan_paused)
    {
        _bcm_linkscan_continue(unit);
    }

    if (counter_paused)
    {
        rv = soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp);
        if (SHR_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_BCMDNX_FABRIC, (BSL_META_U(unit, "soc_counter_start failed\n")));
        }
    }
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
bcm_dnx_fabric_if_link_rate_egress_pps_set(
    int unit,
    bcm_port_t port,
    uint32 pps,
    uint32 burst)
{
    uint32 core_clock_speed = 0;
    uint32 nof_tiks = 0;
    uint64 nof_tiks64;
    soc_port_t link = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), port))
    {
        SHR_ERR_EXIT(BCM_E_PORT, "Port %d is not a fabric port. Only Fabric ports supported for this API", port);
    }

    if ((pps == 0 && burst != 0) || (burst == 0 && pps != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Both pps and burst must equal to 0 in order to disable the shaper");
    }

    link = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port);

    /*
     * Calculate packets per tiks:
     * nof tiks = burst * core_clock / pps
     */
    /** KHz to Hz */
    core_clock_speed = dnx_data_device.general.core_clock_khz_get(unit) * 1000;
    COMPILER_64_SET(nof_tiks64, 0, burst);
    /** packets per tiks to packets per time */
    COMPILER_64_UMUL_32(nof_tiks64, core_clock_speed);

    nof_tiks = 0;
    /** pps == 0 --> disables rate limiting */
    if (pps != 0)
    {
        uint64 pps64;

        COMPILER_64_SET(pps64, 0, pps);
        COMPILER_64_UDIV_64(nof_tiks64, pps64);
        nof_tiks = COMPILER_64_LO(nof_tiks64);
    }

    /*
     * Config shaper
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_port_rate_egress_ppt_set, (unit, link, burst, nof_tiks)));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
bcm_dnx_fabric_if_link_rate_egress_pps_get(
    int unit,
    bcm_port_t port,
    uint32 *pps,
    uint32 *burst)
{
    uint64 pps64;
    uint64 nof_tiks64;
    uint32 nof_tiks = 0;
    uint32 core_clock_speed = 0;
    soc_port_t link = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pps, _SHR_E_PARAM, "pps");
    SHR_NULL_CHECK(burst, _SHR_E_PARAM, "burst");

    if (!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), port))
    {
        SHR_ERR_EXIT(BCM_E_PORT, "Port %d is not a fabric port. Only Fabric ports supported for this API", port);
    }

    link = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port);

    /*
     * Get shaper info
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_port_rate_egress_ppt_get, (unit, link, burst, &nof_tiks)));

    if (nof_tiks == 0)
    {
        *pps = 0;
        SHR_EXIT();
    }

    /*
     * Calculate packets per sec
     * pps = burst * core_clock / nof_tiks
     */
    core_clock_speed = dnx_data_device.general.core_clock_khz_get(unit);
    COMPILER_64_SET(pps64, 0, *burst);
    /** packets per tiks to packets per time */
    COMPILER_64_UMUL_32(pps64, core_clock_speed);
    /** KHz to Hz */
    COMPILER_64_UMUL_32(pps64, 1000);

    COMPILER_64_SET(nof_tiks64, 0, nof_tiks);
    COMPILER_64_UDIV_64(pps64, nof_tiks64);

    *pps = COMPILER_64_LO(pps64);

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
