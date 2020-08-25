/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_internal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <soc/dnx/stat/jer2_stat.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/mbcm.h>
#include <soc/dnx/dbal/dbal.h>
#include "imb_ile_internal.h"
#include "imb_utils.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define IMB_NIF_QUEUE_LEVEL_FLOW_CTRL_NOF_CHANNELS  (16)
#define IMB_ILE_FEC_NOF_INSTANCES  (6)

/**
 * \brief - initialize the ILE required information. 
 * allso create a new PM for the ILE unit 
 * 
 * \param [in] unit - chip unit ID
 * \param [in] imb_info - IMB info required for 
 *        initalization.
 * \param [in] imb_specific_info - specific info required to 
 *        init the ILE
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    portmod_pm_create_info_t pm_info;
    bcm_pbmp_t ilkn_phys;
    bcm_port_t phy;
    portmod_pm_identifier_t ilkn_pms[DNX_DATA_MAX_NIF_ILKN_PMS_NOF];
    int pm, is_over_fabric, ilkn_id, max_nof_pms, nof_pms = 0, pm_id;
    int portmod_phy_offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_pms = dnx_data_nif.ilkn.pms_nof_get(unit);
    /*
     * Clear data 
     */
    BCM_PBMP_CLEAR(ilkn_phys);
    for (pm = 0; pm < max_nof_pms; ++pm)
    {
        portmod_pm_identifier_t_init(unit, &ilkn_pms[pm]);
    }

    /*
     * Specifically in Jer2, the ilkn id correlates with the ILKN unit id. otherwise, we need to iterate over all ILKN
     * IDs in the ILKN unit.
     */
    ilkn_id = imb_info->inst_id;
    /*
     * ELK is defined as static coonfiguration - it should be defined using SOC properties. so we initalize the ILKN on 
     * top of NIF or Fabric according to SOC properties 
     */
    is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->is_over_fabric;
    if (is_over_fabric)
    {
        portmod_phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);
        ilkn_phys = dnx_data_nif.ilkn.supported_phys_get(unit, ilkn_id)->fabric_phys;
        for (pm = 0; pm < max_nof_pms; ++pm)
        {
            pm_id = dnx_data_nif.ilkn.ilkn_pms_get(unit, ilkn_id)->fabric_pm_ids[pm];
            if (dnx_data_nif.ilkn.fabric_pms_get(unit, pm_id)->first_phy != IMB_ILE_PHY_INVALID)
            {
                ilkn_pms[pm].phy = dnx_data_nif.ilkn.fabric_pms_get(unit, pm_id)->first_phy + portmod_phy_offset;
                ilkn_pms[pm].type = dnx_data_nif.ilkn.fabric_pms_get(unit, pm_id)->dispatch_type;
                ++nof_pms;
            }
        }
    }
    else        /* ILKN is over NIF */
    {
        portmod_phy_offset = 0;
        ilkn_phys = dnx_data_nif.ilkn.supported_phys_get(unit, ilkn_id)->nif_phys;
        for (pm = 0; pm < max_nof_pms; ++pm)
        {
            pm_id = dnx_data_nif.ilkn.ilkn_pms_get(unit, ilkn_id)->nif_pm_ids[pm];
            if (dnx_data_nif.ilkn.nif_pms_get(unit, pm_id)->first_phy != IMB_ILE_PHY_INVALID)
            {
                ilkn_pms[pm].phy = dnx_data_nif.ilkn.nif_pms_get(unit, pm_id)->first_phy + portmod_phy_offset;
                ilkn_pms[pm].type = dnx_data_nif.ilkn.nif_pms_get(unit, pm_id)->dispatch_type;
                ++nof_pms;
            }
        }
    }

    SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN_50G;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = nof_pms;
    pm_info.pm_specific_info.os_ilkn.is_over_fabric = is_over_fabric;
    BCM_PBMP_ITER(ilkn_phys, phy)
    {
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, phy + portmod_phy_offset);
    }
    for (pm = 0; pm < dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit); ++pm)
    {
        pm_info.pm_specific_info.os_ilkn.wm_high[pm] = dnx_data_nif.ilkn.watermark_high_get(unit);
        pm_info.pm_specific_info.os_ilkn.wm_low[pm] = dnx_data_nif.ilkn.watermark_low_get(unit);
    }
    pm_info.pm_specific_info.os_ilkn.core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);
    /*
     * ILKN core lane map callback
     */
    pm_info.pm_specific_info.os_ilkn.ilkn_core_lane_map_get = imb_ile_port_ilkn_core_lane_map_get;
    SHR_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - De-init the ILE.
 * 
 * \param [in] unit - chip unit ID
 * \param [in] imb_info - IMB informatin required for de-init
 * \param [in] imb_specific_info - specific ILE information to 
 *        de-init.
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Nothing to do 
     */
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set Aggregated PMs reset state
 *
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] in_reset - reset state. 1:reset, 0:out of reset
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_ile_port_pms_reset(
    int unit,
    bcm_port_t port,
    uint32 in_reset)
{
    bcm_pbmp_t ile_phys, imb_phys;
    int phys_count = 0;
    int imb_id;
    uint32 nof_imbs;
    imb_dispatch_type_t type;
    uint32 in_reset_current;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get number of imb instances in use
     */
    SHR_IF_ERR_EXIT(imb_nof_get(unit, &nof_imbs));

    /*
     * Get ilkn phy bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ile_phys));

    for (imb_id = 0; imb_id < nof_imbs; imb_id++)
    {
        SHR_IF_ERR_EXIT(imb_id_type_get(unit, imb_id, &type));
        if (type != imbDispatchTypeImb_cdu)
        {
            continue;
        }

        /*
         * Get common ilkn-imb phys number
         */
        SHR_IF_ERR_EXIT(imb_phys_get(unit, imb_id, &imb_phys));
        _SHR_PBMP_AND(imb_phys, ile_phys);
        _SHR_PBMP_COUNT(imb_phys, phys_count);

        /*
         * Get current pms reset state
         */
        SHR_IF_ERR_EXIT(imb_pms_reset_get(unit, imb_id, &in_reset_current));

        /*
         * If at least one phy is common to ilkn-imb AND
         * requested reset state is different from current state -
         * set requested reset state.
         */
        if ((phys_count > 0) && (in_reset_current != in_reset))
        {
            SHR_IF_ERR_EXIT(imb_pms_reset_set(unit, imb_id, in_reset));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable FSRD blocks for fabric links
 *
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] enable - enable state. 1:enable, 0:disable
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_ile_port_fsrd_enable(
    int unit,
    bcm_port_t port,
    int enable)
{
    int fsrd_block = 0;
    int nof_fsrd = 0;
    int nof_links_in_fsrd = 0;
    int link = 0;
    int ilkn_phys_counter = 0;
    int fabric_ports_counter = 0;
    bcm_pbmp_t mask_fsrd_phy_bmp;
    bcm_pbmp_t mask_fsrd_fabric_bmp;
    bcm_pbmp_t phys;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));

    nof_fsrd = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    nof_links_in_fsrd = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

    for (fsrd_block = 0; fsrd_block < nof_fsrd; fsrd_block++)
    {
        BCM_PBMP_CLEAR(mask_fsrd_phy_bmp);
        BCM_PBMP_CLEAR(mask_fsrd_fabric_bmp);

        for (link = fsrd_block * nof_links_in_fsrd; link < fsrd_block * nof_links_in_fsrd + nof_links_in_fsrd; link++)
        {
            /*
             * Set FSRD phy bitmap 
             */
            BCM_PBMP_PORT_ADD(mask_fsrd_phy_bmp, link);

            /*
             * Set FSRD fabric ports bitmap 
             */
            BCM_PBMP_PORT_ADD(mask_fsrd_fabric_bmp, SOC_DNX_FABRIC_LINK_TO_PORT(unit, link));
        }

        /*
         * Count number ILKN lanes on FSRD 
         */
        BCM_PBMP_AND(mask_fsrd_phy_bmp, phys);
        BCM_PBMP_COUNT(mask_fsrd_phy_bmp, ilkn_phys_counter);

        /*
         * Count number of active fabric ports on FSRD 
         */
        BCM_PBMP_AND(mask_fsrd_fabric_bmp, PBMP_SFI_ALL(unit));
        BCM_PBMP_COUNT(mask_fsrd_fabric_bmp, fabric_ports_counter);

        /*
         * If no fabric ports on FSRD and ILKN lanes defined on FSRD --> change FSRD enable status 
         */
        if ((fabric_ports_counter == 0) && (ilkn_phys_counter > 0))
        {
            SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fsrd_block_enable_set, (unit, fsrd_block, enable)));
        }
    }

    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_brdc_fsrd_blk_id_set, (unit)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable RX of ILE port
 *
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] enable - enable state. 1:enable, 0:disable
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_rx_data_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int nof_segments = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_get(unit, port, &nof_segments));
    nof_segments = enable ? nof_segments : 0;

    SHR_IF_ERR_EXIT(imb_ile_port_segments_enable_set(unit, port, nof_segments));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initalize the add_info structure before calling 
 *        Portmod API
 * 
 * \param [in] unit - chip unit id. 
 * \param [in] port - logical port
 * \param [out] add_info - add_info information to init
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_ile_portmod_add_info_config(
    int unit,
    bcm_port_t port,
    portmod_port_add_info_t * add_info)
{
    int ilkn_id;
    bcm_core_t core;
    bcm_pbmp_t phys;
    int phy;
    int portmod_phy_offset = 0;
    int ilkn_port_is_over_fabric;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * initalize the structure to invalid values 
     */
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, add_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    add_info->init_config.lane_map_overwrite = 0;
    add_info->init_config.polarity_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    add_info->init_config.fw_load_method_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    add_info->init_config.ref_clk_overwrite = 0; /** same as in pm create, we don't need to overwrite*/

    PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_SET(add_info);
    PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_SET(add_info);
    PORTMOD_PORT_ADD_F_RX_SRIP_CRC_SET(add_info);
    PORTMOD_PORT_ADD_F_ELK_SET(add_info);

    add_info->interface_config.interface = SOC_PORT_IF_ILKN;

    /*
     * ilkn_core_id is the ilkn port index within the ILKN core. in Jer2 we only use one ILKN port in the core so
     * ilkn_core_id is always 0 
     */
    add_info->ilkn_core_id = 0;

    /*
     * Get ILKN id 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));

    /*
     * Set over fabric indication 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &ilkn_port_is_over_fabric));
    add_info->ilkn_port_is_over_fabric = ilkn_port_is_over_fabric;

    if (add_info->ilkn_port_is_over_fabric)
    {
        /** Get phy offset for ilkn over fabric */
        portmod_phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);
    }

    /** get port phys */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    BCM_PBMP_ITER(phys, phy)
    {
        BCM_PBMP_PORT_ADD(add_info->phys, phy + portmod_phy_offset);
    }

    /** get ilkn lanes */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_lanes_get(unit, port, (bcm_pbmp_t *) & (add_info->ilkn_lanes)));

    /*
     * Set Burst configurations 
     */
    add_info->ilkn_burst_max = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->burst_max;
    add_info->ilkn_burst_min = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->burst_min;
    add_info->ilkn_burst_short = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->burst_short;

    /*
     * Set nof segments 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_nof_segments_get(unit, port, &add_info->ilkn_nof_segments));

    /*
     * Set Metaframe period 
     */
    add_info->ilkn_metaframe_period = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->metaframe_period;

    /*
     * No retransmit in Jer2 
     */
    add_info->rx_retransmit = 0;
    add_info->tx_retransmit = 0;

    /*
     * Inband FC calender length
     */
    add_info->ilkn_inb_cal_len_tx = dnx_data_fc.inband.calender_length_get(unit, ilkn_id)->tx;
    add_info->ilkn_inb_cal_len_rx = dnx_data_fc.inband.calender_length_get(unit, ilkn_id)->rx;

    /*
     * Set Bypass flag
     */
    PORTMOD_PORT_ADD_F_PCS_BYPASSED_SET(add_info);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure a new ILE port. includes calling Portmod 
 *        API
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_attach(
    int unit,
    bcm_port_t port)
{
    portmod_port_add_info_t add_info;
    int ilkn_id, is_over_fabric, nof_segments;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN id 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));

    /*
     * Port main OOO 
     */
    SHR_IF_ERR_EXIT(imb_ile_port_reset_set(unit, port, 0));

    /*
     * Nof Segment calculation 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_nof_segments_get(unit, port, &nof_segments));
    SHR_IF_ERR_EXIT(imb_ile_port_nof_segments_set(unit, port, nof_segments));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_set(unit, port, nof_segments));

    /*
     * Disable RX data to avoid bad signals during alignment process
     */
    SHR_IF_ERR_EXIT(imb_ile_port_rx_data_enable_set(unit, port, 0));

    /*
     * Set ILKN over Fabric indication 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    SHR_IF_ERR_EXIT(imb_ile_port_ilkn_over_fabric_set(unit, port, is_over_fabric));

    /*
     * Enable ELK 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type))
    {
        SHR_IF_ERR_EXIT(imb_ile_port_ilkn_elk_set(unit, port, 1));
    }

    /*
     * get port properties before calling Portmod API 
     */
    SHR_IF_ERR_EXIT(imb_ile_portmod_add_info_config(unit, port, &add_info));

    if (!is_over_fabric)
    {
        /*
         *  Take aggregated PMs out of reset (CDU configuration)
         */
        SHR_IF_ERR_EXIT(imb_ile_port_pms_reset(unit, port, 0));
    }
    else
    {
        /*
         *  Enable fabric SerDes for ILKN over fabric
         */
        SHR_IF_ERR_EXIT(imb_ile_port_fsrd_enable(unit, port, 1));
    }

    /*
     * ILKN core configuration
     */

    SHR_IF_ERR_EXIT(portmod_port_add(unit, port, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - control enable/ disable ILE port credits - credits from SerDes Tx to ILKN core.
 *          this function is relevant only for ILKN over fabric
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] enable - enable / disable indication
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_ile_port_credits_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int is_over_fabric = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));

    if (is_over_fabric)
    {
        SHR_IF_ERR_EXIT(portmod_port_credits_enable_set(unit, port, enable));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - control enable/ disable ILE port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] enable - enable / disable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!enable)
    {
        /*
         * Disable credits from SerDes Tx to ILKN core
         */
        SHR_IF_ERR_EXIT(imb_ile_port_credits_enable_set(unit, port, 0));

        /*
         * Disable port in wrapper 
         */
        SHR_IF_ERR_EXIT(imb_ile_port_wrapper_enable_set(unit, port, 0));

        /*
         * Disable bypass interface 
         */
        SHR_IF_ERR_EXIT(imb_ile_port_bypass_if_enable_set(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 0));
    }

    /*
     * Enable / Disable port in Portmod 
     */
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, enable));

    if (enable)
    {
        /*
         * Enable bypass interface 
         */
        SHR_IF_ERR_EXIT(imb_ile_port_bypass_if_enable_set(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 1));

        /*
         * Enable port in wrapper 
         */
        SHR_IF_ERR_EXIT(imb_ile_port_wrapper_enable_set(unit, port, 1));

        /*
         * Enable credits from SerDes Tx to ILKN core
         */
        SHR_IF_ERR_EXIT(imb_ile_port_credits_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get enable status of the port
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - returned enable status 
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the indication from Portmod 
     */
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set the ILE port in loopback. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] loopback - loopback type: 
 * 0 => no loopback. 
 * 1 => MAC loopback (inside the ILKN core) 
 * 2 => PHY loopback (TX->RX, inside the PM) 
 * 3 => PHY Remote (RX->TX, inside the PM) 
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    portmod_loopback_mode_t portmod_loopback;
    int curr_loopback, enable, is_symmetric;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Since the lane map for PM8x50 has been moved to PMD, PHY/REMOTE
     * loopback cannot be configured when lane map is not symmetric.
     */
    if (loopback == BCM_PORT_LOOPBACK_PHY || loopback == BCM_PORT_LOOPBACK_PHY_REMOTE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_is_symmetric_get(unit, port, &is_symmetric));
        if (!is_symmetric)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Lane Map for port %d is not symmetric. Configuring PHY/REMOTE loopback on non-symmetric lane swapped port is forbidden, please adjust the lane map or use MAC loopback",
                         port);
        }
    }

    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, port, &curr_loopback));
    if (curr_loopback == loopback)
    {
        /*
         * Nothing to do
         */
        SHR_EXIT();
    }

    /*
     * Get Enable status to retrieve after setting loopack 
     */
    SHR_IF_ERR_EXIT(imb_ile_port_enable_get(unit, port, &enable));

    /*
     * Disable port before closing loopback 
     */
    SHR_IF_ERR_EXIT(imb_ile_port_enable_set(unit, port, 0));

    if (curr_loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Open current loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, curr_loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 0));
    }
    if (loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Close requested loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 1));
    }

    if (enable)
    {
        SHR_IF_ERR_EXIT(imb_ile_port_enable_set(unit, port, 1));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get loopback type on the port.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] loopback - loopback type. see loopback_set for 
 *        loopback types
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    int rv = 0, lb_enabled = 0;
    portmod_loopback_mode_t portmod_loopback;
    SHR_FUNC_INIT_VARS(unit);

    *loopback = 0;
    /*
     * Iterate over all portmod loopback types
     */
    for (portmod_loopback = portmodLoopbackMacOuter; portmod_loopback != portmodLoopbackCount; ++portmod_loopback)
    {
        rv = portmod_port_loopback_get(unit, port, portmod_loopback, &lb_enabled);
        if (rv == _SHR_E_UNAVAIL)
        {
            /*
             * Portmod loopback type is not supported for PM type
             */
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (lb_enabled)
        {
            SHR_IF_ERR_EXIT(imb_bcm_loopback_from_portmod_loopback_get(unit, port, portmod_loopback, loopback));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get link state indication. includes current link 
 *        state and latch down indication (whether the port was
 *        down since last call to this API)
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] clear_status - whether to clear the latch down 
 *        indication in DB.
 * \param [out] link_state - link state structure.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_link_state_get(
    int unit,
    bcm_port_t port,
    int clear_status,
    bcm_port_link_state_t * link_state)
{
    int db_latch_down, is_link_up;
    uint32 hw_latch_low_aligned;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * link up indication
     */
    SHR_IF_ERR_EXIT(portmod_port_link_get(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, &is_link_up));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_get(unit, port, &db_latch_down));

    if ((is_link_up) && !(db_latch_down))
    {
        SHR_IF_ERR_EXIT(imb_ile_port_latch_down_get(unit, port, &hw_latch_low_aligned));
        db_latch_down = hw_latch_low_aligned ? 0 : 1;
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_set(unit, port, db_latch_down));
    }

    if (clear_status)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_set(unit, port, 0));
    }

    link_state->status = is_link_up;
    link_state->latch_down = db_latch_down;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Measure the serdes rate for ILE ports. If the port is
 *    over fabric, use fabric measure mechanism. Otherwise, use
 *    NIF measure mechanism.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_rx - indicate the FIFO used to measure
 * \param [out] phy_measure - phy measure output
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    int is_over_fabric = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));

    if (is_over_fabric)
    {
        SHR_IF_ERR_EXIT(imb_diag_fabric_phy_measure_get(unit, port, is_rx, phy_measure));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_diag_nif_phy_measure_get(unit, port, is_rx, phy_measure));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * imb_ile_port_ilkn_counter_get
 *
 * \brief Get ilkn counter 
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [in]  counter_type    - counter type
 * \param [out]  counter_val     - ilkn counter value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_ilkn_counter_get(
    int unit,
    bcm_port_t port,
    int counter_type,
    uint64 *counter_val)
{
    int ilkn_id;
    int channel;
    uint32 ctr_low, ctr_high;
    uint64 tmp_counter;
    int speed, port_has_speed;
    uint32 fec_instance;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, &port_has_speed));
    if (port_has_speed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
    }
    else
    {
        speed = 0;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));

    switch (counter_type)
    {
        case soc_jer2_counters_ilkn_rx_pkt_counter:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_rx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_PACKETS_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_PACKETS_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case soc_jer2_counters_ilkn_rx_byte_counter:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_rx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_BYTES_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_BYTES_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case soc_jer2_counters_ilkn_rx_err_pkt_counter:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_rx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_ERR_PACKETS_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_ERR_PACKETS_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case soc_jer2_counters_ilkn_tx_pkt_counter:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_tx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_PACKETS_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_PACKETS_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case soc_jer2_counters_ilkn_tx_byte_counter:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_tx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_BYTES_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_BYTES_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case soc_jer2_counters_ilkn_tx_err_pkt_counter:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_tx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_ERR_PACKETS_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_ERR_PACKETS_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case soc_jer2_counters_nif_rx_fec_correctable_error:
            if (DNXC_PORT_PHY_SPEED_IS_PAM4(speed))
            {
                /*
                 * One FEC instance for every 2 adjacent lanes.
                 * There are total 6 FEC instance.
                 * The max lanenum is 12 for PAM4 ILKN.
                 */
                for (fec_instance = 0; fec_instance < IMB_ILE_FEC_NOF_INSTANCES; ++fec_instance)
                {
                    SHR_IF_ERR_EXIT(imb_ile_port_ilkn_fec_counter_dbal_get
                                    (unit, ilkn_id, fec_instance, DBAL_FIELD_RX_FEC_CORRECTED_CNT, &ctr_low));
                    COMPILER_64_SET(tmp_counter, 0, ctr_low);
                    COMPILER_64_ADD_64(*counter_val, tmp_counter);
                }
            }
            break;
        case soc_jer2_counters_nif_rx_fec_uncorrrectable_errors:
            if (DNXC_PORT_PHY_SPEED_IS_PAM4(speed))
            {
                /*
                 * One FEC instance for every 2 adjacent lanes.
                 * There are total 6 FEC instance.
                 * The max lanenum is 12 for PAM4 ILKN.
                 */
                for (fec_instance = 0; fec_instance < IMB_ILE_FEC_NOF_INSTANCES; ++fec_instance)
                {
                    SHR_IF_ERR_EXIT(imb_ile_port_ilkn_fec_counter_dbal_get
                                    (unit, ilkn_id, fec_instance, DBAL_FIELD_RX_FEC_UNCORRECTED_CNT, &ctr_low));
                    COMPILER_64_SET(tmp_counter, 0, ctr_low);
                    COMPILER_64_ADD_64(*counter_val, tmp_counter);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is invalid", counter_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * imb_ile_port_ilkn_counter_clear
 *
 * \brief Clear ilkn counter 
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_ilkn_counter_clear(
    int unit,
    bcm_port_t port)
{
    uint32 entry_handle_id_rx, entry_handle_id_tx;
    int channel;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_RX_STATS_ACC, &entry_handle_id_rx));
    dbal_entry_key_field32_set(unit, entry_handle_id_rx, DBAL_FIELD_LOGICAL_PORT, port);
    dbal_entry_value_field32_set(unit, entry_handle_id_rx, DBAL_FIELD_TYPE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id_rx, DBAL_FIELD_CMD, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id_rx, DBAL_FIELD_ADDR, INST_SINGLE, channel);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_rx, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_TX_STATS_ACC, &entry_handle_id_tx));
    dbal_entry_key_field32_set(unit, entry_handle_id_tx, DBAL_FIELD_LOGICAL_PORT, port);
    dbal_entry_value_field32_set(unit, entry_handle_id_tx, DBAL_FIELD_TYPE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id_tx, DBAL_FIELD_CMD, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id_tx, DBAL_FIELD_ADDR, INST_SINGLE, channel);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_tx, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get recommended default values of resource members for given port with speed
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - not used
 * \param [out] resource - each memeber other than speed with BCM_PORT_RESOURCE_DEFAULT_REQUEST
 *                         will be filled with default value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * bcm_port_resource_t
 *   * BCM_PORT_RESOURCE_DEFAULT_REQUEST
 */
int
imb_ile_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    bcm_pbmp_t phys;
    int num_lane;
    int lane_rate;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    if ((resource->flags != 0) || (resource->physical_port != 0) || (resource->encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: flags, physical_port and encap fields of bcm_port_resource_t are not in use. They should all be initialized to 0",
                     port);
    }

    if ((resource->port != 0) && (resource->port != port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d: resource->port should be 0 or equal to port passed to the API", port);
    }

    if (resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: there is no given default value for speed. speed is mandatory as input to this API",
                     port);
    }

    /*
     *  calculate lane rate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    BCM_PBMP_COUNT(phys, num_lane);
    lane_rate = resource->speed / num_lane;

    /*
     * in case user didn't configure the FEC already - give default value
     */
    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->fec_type = bcmPortPhyFecNone;
    }

    /*
     * in case user didn't configure CL72 - by default it is enabled
     */
    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = 1;
    }

    /*
     * get default lane_config
     */
    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->phy_lane_config = 0;
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                     SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_BACKPLANE_COPPER_TRACE);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(resource->phy_lane_config);

        /*
         * FORCE_ES, FORCE_NS must be clear for {NRZ} and for {PAM4 with link training}
         */
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(resource->phy_lane_config);

        if (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate) && (resource->link_training == 0))
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(resource->phy_lane_config);
        }

        /*
         * internal fields that will be set in resource_set before calling portmod. Not exposed to user.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(resource->phy_lane_config);
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(resource->phy_lane_config);
        /*
         * this field should always be 0 in normal operation, can be changed only for debug.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(resource->phy_lane_config, 0);

    }

    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
