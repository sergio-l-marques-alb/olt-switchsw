/*
 * $Id: ramon_fabric_topology.c,v 1.16.24.1 Broadcom SDK $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC RAMON FABRIC TOPOLOGY
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/ramon/ramon_fabric_topology.h>
#include <soc/dnxc/error.h>
#include <soc/defs.h>
#include <soc/error.h>

/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
/*
 * Function:
 *      soc_ramon_fabric_topology_isolate_set
 * Purpose:
 *      Isolate / Acrivate fabric
 * Parameters:
 *      unit - (IN) Unit number.
 *      val  - (IN) Link Isolation Status
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
soc_ramon_fabric_topology_isolate_set(
    int unit,
    soc_dnxc_isolation_status_t val)
{
    uint32 reg_val32 = 0;
    uint64 reg_val64;
    uint32 gpd_rmgr_global_time = 0;
    uint32 gpd_rmgr_global_at_core_clock_steps = 0;
    uint32 gpd_spread_time = 0;
    uint32 gpd_rmgr = 0;
    uint32 max_base_index = 0;
    uint32 core_clock_speed;
    uint32 gpd_threshold;
    uint32 sleep_time_msec;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg_val64);

    if (soc_dnxc_isolation_status_isolated == val)
    {
        SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
        soc_reg64_field_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_STARTf, 0);
        SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

        sal_usleep(1000);

        /*
         * Initiate GPD process 
         */
        SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
        soc_reg64_field_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_STARTf, 1);
        SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

        /*
         * Wait for the GPD to spread in the system.
         */
        /*
         * full_cycle [nsec] = gpd_rmgr * 32 clocks * clock period 
         */
        gpd_rmgr = soc_reg64_field32_get(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, reg_val64, GRACEFUL_PD_RMGRf);
        gpd_rmgr_global_at_core_clock_steps = gpd_rmgr * dnxf_data_fabric.reachability.rmgr_units_get(unit);
        gpd_rmgr_global_time = ((gpd_rmgr_global_at_core_clock_steps * 1000) / (dnxf_data_device.general.core_clock_khz_get(unit))) * 1000;     /* [nsec] 
                                                                                                                                                 */

        SHR_IF_ERR_EXIT(READ_QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, 0, &reg_val32));
        max_base_index =
            soc_reg_field_get(unit, QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val32, MAX_BASE_INDEXf);

        /*
         * divide by 1000 to convert nsec to usec 
         */
        gpd_spread_time =
            (max_base_index * gpd_rmgr_global_time * dnxf_data_fabric.reachability.gpd_rmgr_time_factor_get(unit)) /
            1000;
        sal_usleep(gpd_spread_time);

        /*
         * Set RMGR=0 
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_rmgr_set, (unit, 0)));

        /*
         * wait for peer FE device (a device connected to the one we are currently isolating) to exit GPD mode. if an
         * FE device enters GPD mode by receiving a GPD message, after this time period it will get out of GPD mode (if 
         * no additional GPD messages were received during it). time[msec] = threshold[clocks] /
         * core_clock_speed[khz=kclocks/sec] 
         */
        core_clock_speed = dnxf_data_device.general.core_clock_khz_get(unit);
        SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
        gpd_threshold =
            soc_reg64_field32_get(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, reg_val64, GRACEFUL_PD_CNT_THf);
        sleep_time_msec = (gpd_threshold / core_clock_speed) * 10;      /* multiply by 10 to make sure we wait enough
                                                                         * time */
        sal_usleep(sleep_time_msec * 1000);

        SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
        soc_reg64_field_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_STARTf, 0);
        SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

    }
    else
    {
        /*
         * Reset mesh topology 
         */
        SHR_IF_ERR_EXIT(soc_ramon_fabric_topology_mesh_topology_reset(unit));

        /*
         * Reset RMGR value 
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_rmgr_set, (unit, 1)));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_topology_mesh_topology_reset
 * Purpose:
 *      Rest MESH TOPOLOGY block
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
soc_ramon_fabric_topology_mesh_topology_reset(
    int unit)
{
    uint32 reg_val32;
    soc_reg_above_64_val_t reg_above_64_val;
    int cch_index, nof_instances_cch;
    SHR_FUNC_INIT_VARS(unit);

    nof_instances_cch = dnxf_data_device.blocks.nof_instances_cch_get(unit);

    if (SOC_DNXF_IS_FE13(unit))
    {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        for (cch_index = 0; cch_index < nof_instances_cch; cch_index++)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, CCH_AUTO_DOC_NAME_15r, cch_index, 0, reg_above_64_val));
        }
    }

    /*
     * MESH_TOPOLOGY reset sequence
     */
    for (cch_index = 0; cch_index < nof_instances_cch; cch_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, cch_index, 0, &reg_val32));
        soc_reg_field_set(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, &reg_val32, FILTER_CELLSf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, cch_index, 0, reg_val32));
    }

    sal_usleep(1000);

    /*
     * reset MESH_TOPOLOGY
     */
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 0);
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, reg_val32));
    /*
     * Enable back 
     */
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 1);
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, reg_val32));

    for (cch_index = 0; cch_index < nof_instances_cch; cch_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, cch_index, 0, &reg_val32));
        soc_reg_field_set(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, &reg_val32, FILTER_CELLSf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, cch_index, 0, reg_val32));
    }

    if (SOC_DNXF_IS_FE13(unit))
    {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        for (cch_index = 0; cch_index < nof_instances_cch; cch_index++)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, CCH_AUTO_DOC_NAME_15r, cch_index, 0, reg_above_64_val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set static configuration of the multicast load balancing table (MCLBT).
 *   The MCLBT groups update periodically by HW. If user changed the
 *   update to be controlled by SW, he needs to statically set the
 *   configuration.
 * \param [in] unit -
 *   The unit number.
 * \param [in] destination_local_id -
 *   Local module id of the destination.
 * \param [in] links_count -
 *   Number of links to configure.
 * \param [in] links_array -
 *   Array of links to configure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   MC static config for a group can be done only if sw group link
 *   configuration was enabled for the group.
 * \see
 *   None.
 */
shr_error_e
soc_ramon_fabric_topology_mc_set(
    int unit,
    soc_module_t destination_local_id,
    int links_count,
    soc_port_t * links_array)
{
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 val;
    soc_port_t link = -1;
    int i_link = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * build bitmap
     */
    sal_memset(bmp, 0, sizeof(uint32) * DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32);

    for (i_link = 0; i_link < links_count; i_link++)
    {
        link = links_array[i_link];
        if (link != SOC_PORT_INVALID)   /* Skip invalid ports (virtual links support) */
        {
            SHR_BITSET(bmp, link);
        }
    }
    /*
     * maps destinations (LRCs) -> links (static configuration) 
     */
    SHR_IF_ERR_EXIT(WRITE_RTP_SGRm(unit, MEM_BLOCK_ALL, destination_local_id, bmp));

    /*
     * Force MCLBT calculation 
     */
    SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, &val));
    soc_reg_field_set(unit, RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr, &val, ALRC_FORCE_CALCULATIONf, 1);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, val));

    sal_usleep(1000);

    soc_reg_field_set(unit, RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr, &val, ALRC_FORCE_CALCULATIONf, 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get static configuration of the multicast load balancing table (MCLBT).
 *   The MCLBT groups update periodically by HW. If user changed the
 *   update to be controlled by SW, he needs to statically set the
 *   configuration.
 * \param [in] unit -
 *   The unit number.
 * \param [in] destination_local_id -
 *   Local module id of the destination.
 * \param [in] links_count_max -
 *   Maximum links in links_array.
 * \param [out] links_count -
 *   Number of configured links.
 * \param [out] links_array -
 *   Array of configured links.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   MC static config for a group can be done only if sw group link
 *   configuration was enabled for the group.
 * \see
 *   None.
 */
shr_error_e
soc_ramon_fabric_topology_mc_get(
    int unit,
    soc_module_t destination_local_id,
    int links_count_max,
    int *links_count,
    soc_port_t * links_array)
{
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    int i_link = 0;
    int link_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * build bitmap
     */
    sal_memset(bmp, 0, sizeof(uint32) * DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32);

    /*
     * maps destinations (LRCs) -> links 
     */
    SHR_IF_ERR_EXIT(READ_RTP_SGRm(unit, MEM_BLOCK_ANY, destination_local_id, bmp));

    *links_count = 0;

    SHR_BIT_ITER(bmp, dnxf_data_fabric.topology.nof_local_modid_get(unit), i_link)
    {
        if (link_index > links_count_max)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "links_array is too small for required links.");
        }

        links_array[link_index] = i_link;
        (link_index)++;
    }

    *links_count = link_index;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_topology_isolate_get
 * Purpose:
 *      Get fabric isolation state (Isolated / Activated)
 * Parameters:
 *      unit - (IN)  Unit number.
 *      val  - (OUT) Link Isolation Status
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
soc_ramon_fabric_topology_isolate_get(
    int unit,
    soc_dnxc_isolation_status_t * val)
{
    uint32 value;
    soc_reg_above_64_val_t reg_above64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

    value =
        soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGRf);

    if (value == 0)
        *val = soc_dnxc_isolation_status_isolated;
    else
        *val = soc_dnxc_isolation_status_active;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_topology_rmgr_set
 * Purpose:
 *      Setting recommended Reachability Messages Generation Period
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      enable      - (IN)  Reachability msgs enabled/disabled
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
soc_ramon_fabric_topology_rmgr_set(
    int unit,
    int enable)
{
    soc_reg_above_64_val_t reg_above64_val;
    uint32 core_clock_speed = 0;
    uint32 rmgr = 0;
    uint32 rmgr_global = 0;
    uint32 rmgr_global_at_core_clock_steps = 0;
    int rate_between_links = 0;
    int full_cycle_rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(reg_above64_val, 0, sizeof(reg_above64_val));

    /*
     * calc rmgr value
     */
    if (enable)
    {
        core_clock_speed = dnxf_data_device.general.core_clock_khz_get(unit);

        /*
         * generation between links = RMGR * clock period
         *
         * generation rate is in ns units, core_clock_speed is in khz units,
         * so we need to multiply the result by 10^-9 * 10^3 = 10^-6
         */
        rate_between_links = dnxf_data_fabric.reachability.gen_rate_link_delta_get(unit);
        rmgr = (rate_between_links * core_clock_speed) / 1000000;

        /*
         * full cycle = RMGRGlobal * 32 clocks * clock period
         *
         * The result is multiply by 10^-6 in 2 steps, in order to prevent
         * register overflow.
         */
        full_cycle_rate = dnxf_data_fabric.reachability.gen_rate_full_cycle_get(unit);
        rmgr_global_at_core_clock_steps = ((full_cycle_rate / 100) * core_clock_speed) / 10000;
        rmgr_global = rmgr_global_at_core_clock_steps / dnxf_data_fabric.reachability.rmgr_units_get(unit);
    }
    else
    {
        rmgr = 0;
        rmgr_global = 0;
    }

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGRf, rmgr);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGR_GLOBALf,
                                 rmgr_global);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_topology_lb_set
 * Purpose:
 *      Configure link topology for a destination
 * Parameters:
 *      unit                  - (IN) Unit number.
 *      destination_local_id  - (IN) Local module id of the destination
 *      links_count           - (IN) Number of links to configure
 *      links_array           - (IN) Array of links to configure
 * Returns:
 *      _SHR_E_xxx
 * Note:
 *      destination_local_id must be mask-out of LOCAL_MODID_BIT
 */
shr_error_e
soc_ramon_fabric_topology_lb_set(
    int unit,
    soc_module_t destination_local_id,
    int links_count,
    soc_port_t * links_array)
{
    int i, nof_links;
    int max_link_score;
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 totsf_val = 0, slsct_val, score, sctinc_val, sctinc, last_score = 0;
    soc_port_t link;
    soc_field_t scrub_en;
    soc_reg_above_64_val_t reg_above64_val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If links_count>nof_links/2 only normal mode is allowed (limitation is caused by size of MULTI_TB, which is not
     * used for normal mode) 
     */
    BCM_PBMP_COUNT(dnxf_data_port.general.supported_phys_get(unit)->pbmp, nof_links);
    if (soc_dnxf_load_balancing_mode_normal != dnxf_data_fabric.topology.load_balancing_mode_get(unit)
        && links_count > nof_links / 2)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "links_count > %d is allowed only in normal load balancing mode", nof_links / 2);
    }

    /*
     * SCT_SCRUB_ENABLEf - disable dynamic calculation of link scores 
     */
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    scrub_en =
        soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val,
                                     SCT_SCRUB_ENABLEf);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val,
                                 SCT_SCRUB_ENABLEf, 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

    /*
     * totsf value - initial nof links towards the destination 
     */
    soc_mem_field_set(unit, RTP_TOTSFm, &totsf_val, TOTAL_LINKSf, (uint32 *) &links_count);

    /*
     * build bitmap
     */
    sal_memset(bmp, 0, sizeof(bmp));

    for (i = 0; i < links_count; i++)
    {
        link = links_array[i];
        if (link != SOC_PORT_INVALID)   /* Skip invalid ports (virtual links support) */
        {
            SHR_BITSET(bmp, link);
        }
    }

    /*
     * Save in sw_state the first link in the group, so we will be able to
     * access RCG load balancing table with the right index in the get function.
     */
    SHR_IF_ERR_EXIT(dnxf_state.lb.
                    group_to_first_link.set(unit, 0 /** core_idx */ , destination_local_id, links_array[0]));

    /*
     * write bitmap to registers
     */
    for (i = 0; i < links_count; i++)
    {
        link = links_array[i];
        if (link != SOC_PORT_INVALID)   /* Skip invalid ports (virtual links support) */
        {
            /*
             * RCGLBT - RCG load balancing table
             */
            SHR_IF_ERR_EXIT(WRITE_RTP_RCGLBTm(unit, MEM_BLOCK_ALL, link, bmp));

            /*
             * TOTSF - initial nof links towards the destination
             */
            SHR_IF_ERR_EXIT(WRITE_RTP_TOTSFm(unit, MEM_BLOCK_ALL, link, &totsf_val));

            /*
             * SLSCT - score of each link in the group
             */
            slsct_val = 0;
            max_link_score = dnxf_data_fabric.topology.max_link_score_get(unit);
            if (soc_dnxf_load_balancing_mode_destination_unreachable ==
                dnxf_data_fabric.topology.load_balancing_mode_get(unit))
            {
                score = max_link_score;
            }
            else
            {
                score = (max_link_score * (i + 1)) / links_count;
            }
            soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, SCORE_OF_LINKf, &score);
            SHR_IF_ERR_EXIT(WRITE_RTP_SLSCTm(unit, MEM_BLOCK_ALL, link, &slsct_val));

            /*
             * SCTINC - deltas between links scores
             */
            sctinc_val = 0;
            sctinc = score - last_score;
            last_score = score;
            soc_mem_field_set(unit, RTP_SCTINCm, &sctinc_val, SCORE_TO_INCf, &sctinc);
            SHR_IF_ERR_EXIT(WRITE_RTP_SCTINCm(unit, MEM_BLOCK_ALL, link, &sctinc_val));
        }
    }

    /*
     * SCT_SCRUB_ENABLEf - restore dynamic calculation of link scores 
     */
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val,
                                 SCT_SCRUB_ENABLEf, scrub_en);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_topology_lb_get
 * Purpose:
 *      Get link topology for a destination
 * Parameters:
 *      unit                  - (IN)  Unit number.
 *      destination_local_id  - (IN)  Local module id of the destination
 *      links_count_max       - (IN)  Max number of links in links_array
 *      links_count           - (OUT) Number of links pointing to destination
 *      links_array           - (OUT) Array of links pointing to destination
 * Returns:
 *      _SHR_E_xxx
 * Note:
 *      destination_local_id must be mask-out of LOCAL_MODID_BIT
 */
shr_error_e
soc_ramon_fabric_topology_lb_get(
    int unit,
    soc_module_t destination_local_id,
    int links_count_max,
    int *links_count,
    soc_port_t * links_array)
{
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 totsf_val;
    int i, link_index;
    soc_port_t link = -1;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The RCG load balancing table index is link, but we get group param
     * from the API. Need to get the first link of the group from sw_state
     * and then access with it to the table.
     */
    SHR_IF_ERR_EXIT(dnxf_state.lb.group_to_first_link.get(unit, 0 /** core_idx */ , destination_local_id, &link));
    SHR_IF_ERR_EXIT(READ_RTP_RCGLBTm(unit, MEM_BLOCK_ANY, link, bmp));
    *links_count = 0;

    PBMP_SFI_ITER(unit, i)
    {
        if (SHR_BITGET(bmp, i)) /* read TOTSF LINK_NUM for first link */
        {
            SHR_IF_ERR_EXIT(READ_RTP_TOTSFm(unit, MEM_BLOCK_ALL, i, &totsf_val));
            soc_mem_field_get(unit, RTP_TOTSFm, &totsf_val, TOTAL_LINKSf, (uint32 *) links_count);
            break;
        }
    }

    if ((*links_count) > links_count_max)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "links_array is too small");
    }

    link_index = 0;
    PBMP_SFI_ITER(unit, i)
    {
        if (SHR_BITGET(bmp, i))
        {
            links_array[link_index] = i;
            (link_index)++;
        }
    }

    for (; link_index < (*links_count); link_index++)
    {
        links_array[link_index] = -1;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_fabric_topology_local_route_control_cells_enable_set(
    int unit,
    int enable)
{
    uint32 reg_val;
    int first_qrh = 0, last_qrh = 0, qrh_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_DNXF_IS_FE13(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Disabling LR is allowed only on FE in FE13 mode");
    }

    /**
     * The configuration is relevant only for QRH0 to QRH3 because we aim to disable LR control cells generation only on FE1
     */
    first_qrh = 0;
    last_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit) / 2;

    for (qrh_index = first_qrh; qrh_index < last_qrh; qrh_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, qrh_index, 0, &reg_val));
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val, ENABLE_CTRL_LOCAL_FE_1_ROUTINGf, enable);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, qrh_index, 0, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
