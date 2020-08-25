/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/wb_engine.h>
        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM_OS_ILKN_50G_SUPPORT

#include <soc/portmod/pmOsILKN_50G.h>
#include <soc/portmod/pmOsILKN_shared.h>

#define OS_ILKN_BURST_SHORT_32B_VAL                   (0)
#define OS_ILKN_BURST_SHORT_64B_VAL                   (1)
#define OS_ILKN_BURST_SHORT_96B_VAL                   (3)
#define OS_ILKN_BURST_SHORT_128B_VAL                  (6)
#define OS_ILKN_LANES_PER_PHY_CORE                    (8)
#define OS_ILKN_LANE_MAP_REGISTER_RESOLUTION          (5)

#define OS_ILKN_PM_NOT_FOUND    (-1)

extern soc_reg_t remap_lanes_rx_regs[];
extern soc_reg_t remap_lanes_tx_regs[];
extern soc_field_t remap_lanes_rx_fields[];
extern soc_field_t remap_lanes_tx_fields[];

/* Ilkn core lane map structure */
typedef struct pmOsIlkn_core_lane_map_s {
    int num_of_lanes;
    int rx_lane_map[OS_ILKN_TOTAL_LANES_PER_CORE];
    int tx_lane_map[OS_ILKN_TOTAL_LANES_PER_CORE];
} pmOsIlkn_core_lane_map_t;

/**
 * \brief - get number of pipes to configure ilkn core based on OS formula
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM information
 * \param [out] num_pipes - number of pipes
 *
 * \return
 *   STATIC int - see SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_port_num_pipes_get(int unit, int port, pm_info_t pm_info, uint32* num_pipes)
{
    pmOsIlkn_internal_t* ilkn_data;
    portmod_speed_config_t speed_config;
    int nof_lanes, serdes_speed=-1, core_id, i, is_pm_aggregated=0;
    uint32 core_clk_khz;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    core_clk_khz = ilkn_data->core_clk_khz;

    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    _SOC_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &speed_config));

    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(
                    __portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_speed_config_get(unit, port, ilkn_data->pms[i], &speed_config));
            break;
        }
    }

    serdes_speed = speed_config.speed;

    /* pipe number formula by OS */
    *num_pipes = PORTMOD_DIV_ROUND_UP( (10 * serdes_speed * nof_lanes ) , ( (core_clk_khz / 100) * 67) );

    if (*num_pipes > OS_ILKN_SLE_MAX_NOF_PIPES)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("pipe number exceeded limit (%d)"), *num_pipes));
    }

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - update the WB Engine DB with the aggregated PMs for 
 *        the ILKN IF.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM information. extract the phys in 
 *        each PM and compare with ILKN active phys. if there is
 *        a match - the PM is aggregated for this ILKN port.
 *   
 * \return
 *   STATIC int - see SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_aggregated_pms_update(int unit, int port, pm_info_t pm_info)
{
    pmOsIlkn_internal_t* ilkn_data;
    int core_id, pm, nof_pm_lanes, is_pm_aggregate = 0;
    portmod_pbmp_t pm_phys;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* In JER2 we expect only 3 PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm)
    {
        if (ilkn_data->pms[pm])
        {
            PORTMOD_PBMP_CLEAR(pm_phys);
            /* get phy bitmap from every PM */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_pm_phys_get(unit, ilkn_data->pms[pm], &pm_phys));
            /* match the PM phys with the ILKN phys */
            PORTMOD_PBMP_AND(pm_phys, ilkn_data->phys);
            /* count the matches */
            PORTMOD_PBMP_COUNT(pm_phys, nof_pm_lanes);
            /* PM is aggregated for the ILKN port if it has phys in the PM */
            is_pm_aggregate = (nof_pm_lanes > 0) ? 1 : 0;
            /* save info in WB engine DB */
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, is_pm_aggregate, core_id, pm));
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - ILKN retransmit Configuration. Static method to be 
 *        called only from port attach / detach methods, and
 *        only when retransmit is enabled.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] pm_info - pm specific DB
 * \param [in] rx_retransmit - is retransmit enabled on RX side
 * \param [in] tx_retransmit - is retransmit enabled on TX side
 * \param [in] reserved_channel_rx - channel reserved for RX 
 *        retransmit
 * \param [in] reserved_channel_tx - channel reserved for TX 
 *        retransmit
 *   
 * \return
 *   STATIC int - see _SHR_E_ *
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_retransmit_config_set(int unit, int port, pm_info_t pm_info, uint32 rx_retransmit, uint32 tx_retransmit, uint32 reserved_channel_rx, uint32 reserved_channel_tx)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_USERf, rx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_USERf, tx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val)); 

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_RETRANSMIT_CONFIGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_RETRANSMIT_CONFIGr, &reg_val, SLE_RX_RETRANSMIT_CONFIG_RETRANSMIT_ENABLEf, rx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_RETRANSMIT_CONFIGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_RETRANSMIT_CONFIGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_RETRANSMIT_CONFIGr, &reg_val, SLE_TX_RETRANSMIT_CONFIG_RETRANSMIT_ENABLEf, tx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_RETRANSMIT_CONFIGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr, &reg_val, SLE_RX_FCOB_RETRANSMIT_SLOT_RETRANSREQ_IDf, reserved_channel_rx);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr, &reg_val, SLE_TX_FCOB_RETRANSMIT_SLOT_RETRANSREQ_IDf, reserved_channel_tx);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;

}

/**
 * \brief - set ILKN enabled lanes to HW
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enabled_lanes - uint32 to represent enabled lanes 
 *        bitmap.
 *   
 * \return
 *   STATIC int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_enabled_lanes_set(int unit, int port, uint32 enabled_rx_serdes, uint32 enabled_tx_serdes)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* The register has reverse logic  - so we negate the lanes bitmap */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_LANEr, &reg_val, SLE_TX_LANE_DISABLEf, ~enabled_tx_serdes);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_LANEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_LANEr, &reg_val, SLE_RX_LANE_DISABLEf, ~enabled_rx_serdes);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_LANEr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Stall selection configuration for ILKN. stall 
 *        selection tells the ILKN core to sync all lanes
 *        according to a specific lane. if it is enabled it is
 *        suggested to set to the last enabled lane - it can
 *        help prevent deskew problems
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] stall_en - enable stalling according to specific 
 *        lane
 * \param [in] lane - which lane should be used for stalling
 *   
 * \return
 *   STATIC int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOSILKN_50G_ilkn_stall_selection_set(int unit, int port, int stall_en, int lane)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr, &reg_val, SLE_TX_SERDES_AFIFO_STALL_SEL_LANEf, lane); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr, &reg_val, SLE_TX_SERDES_AFIFO_STALL_SEL_ENf, stall_en);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Configure ILKN lanes. this is a sub function of port 
 *        attach method.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - pm information DB
 * \param [in] lane_map - lane map info for ILKN core
 *   
 * \return
 *   STATIC int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_lanes_config(int unit, int port, pm_info_t pm_info, pmOsIlkn_core_lane_map_t *lane_map)
{
    portmod_pbmp_t mirror_bmp, phys_aligned, serdes_rx_bmp, serdes_tx_bmp;
    int core_id, lane, lane_pos, last_lane_pos, last_lane, mirror_lane;
    uint32 rx_reg_val, tx_reg_val, lane_count;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    /* Lane Disable - Reverse Logic */
    PORTMOD_PBMP_CLEAR(mirror_bmp);
    /* Clear rx and tx serdes bmp */
    PORTMOD_PBMP_CLEAR(serdes_rx_bmp);
    PORTMOD_PBMP_CLEAR(serdes_tx_bmp);

    /* get phys_aligned from WB */
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &phys_aligned, core_id));

    PORTMOD_PBMP_ITER(phys_aligned, lane)
    {
        if ( lane > (OS_ILKN_TOTAL_LANES_PER_CORE - 1) )
        { /* coverity - protect from reading out of bounds */
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("lane %d is out of bounds"), lane));
        }

        /* For ILKN port 1 -> bitmap should be mirrored */
        mirror_lane = (core_id) ? OS_ILKN_TOTAL_LANES_PER_CORE - 1 - lane: lane;
        PORTMOD_PBMP_PORT_ADD(mirror_bmp, mirror_lane);
        /* Get the rx and tx serdes bmp */
        PORTMOD_PBMP_PORT_ADD(serdes_rx_bmp, lane_map->rx_lane_map[lane]);
        PORTMOD_PBMP_PORT_ADD(serdes_tx_bmp, lane_map->tx_lane_map[lane]);
    }

    rx_reg_val = SOC_PBMP_WORD_GET(serdes_rx_bmp, 0);
    tx_reg_val = SOC_PBMP_WORD_GET(serdes_tx_bmp, 0);
    /* Set enabled lanes in HW */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_enabled_lanes_set(unit, port, rx_reg_val, tx_reg_val));

    sal_usleep(1000);

    /* Get last lane for stall selection - Choose the last enabled lane in order to prevent skew probles between lanes*/
    PORTMOD_PBMP_COUNT(mirror_bmp, lane_count);
    last_lane_pos = (core_id) ? 0 : lane_count - 1;
    lane_pos = 0;
    PORTMOD_PBMP_ITER(mirror_bmp, last_lane)
    {
        if (lane_pos == last_lane_pos) 
        {
            break; 
        }
        ++lane_pos;
    }
    /* Set stall slection in HW */
    _SOC_IF_ERR_EXIT(_pmOSILKN_50G_ilkn_stall_selection_set(unit, port, 1, last_lane));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Configure ILKN bursts in HW.
 * 
 * \param [in] unit - chip unit ID.
 * \param [in] port - logical port
 * \param [in] pm_info - pm info DB
 * \param [in] burst_max - burst max in bytes
 * \param [in] burst_min - burst min in bytes
 * \param [in] burst_short - burst short in bytes
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_burst_config_set(int unit, int port, pm_info_t pm_info, int burst_max, int burst_min, int burst_short)
{
    uint32 reg_val, burst_short_val, burst_max_val, burst_min_val;
    SOC_INIT_FUNC_DEFS;

    switch (burst_short)
    {
    case 32:
        burst_short_val = OS_ILKN_BURST_SHORT_32B_VAL;
        break;
    case 64: 
        burst_short_val = OS_ILKN_BURST_SHORT_64B_VAL;
        break;
    case 96:
        burst_short_val = OS_ILKN_BURST_SHORT_96B_VAL;
        break;
    case 128:
        burst_short_val = OS_ILKN_BURST_SHORT_128B_VAL;
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Burst short value %d for port %d is invalid"), burst_short, port));
    }
    /*burst min has the following calculation in HW: (burst_min_bytes = (burst_min_val+1) * 32bytes). 
      must be lesser or equal to burst_max/2 and must be higher or equal to burst_short */
    burst_min_val = burst_min / 32 - 1;
    /*burst max has the following calculation in HW: (burst_max_bytes = (burst_max_val+1) * 64bytes). 
      i.e 0000 = 64 bytes   0001 = 128 bytes ... and so on*/
    burst_max_val = burst_max / 64 - 1;
    
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_BURSTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_SHORTf, burst_short_val);
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_MINf, burst_min_val);
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_MAXf, burst_max_val);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_BURSTr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_BURSTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_SHORTf, burst_short_val);
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_MINf, burst_min_val);
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_MAXf, burst_max_val);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_BURSTr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - configure metaframe period in HW
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] metaframe_period - metaframe period in bytes
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_metaframe_period_set(int unit, int port, pm_info_t pm_info, int metaframe_period)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_METAFRAMEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_METAFRAMEr, &reg_val, SLE_TX_METAFRAME_PERIODf, metaframe_period);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_METAFRAMEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_METAFRAMEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_METAFRAMEr, &reg_val, SLE_RX_METAFRAME_PERIODf, metaframe_period);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_METAFRAMEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - set number of segments in HW
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] nof_segments - number of segements for the ILKN 
 *        port. (2/4 or 0 to disable)
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_nof_segments_set(int unit, int port, pm_info_t pm_info, uint32 nof_segments)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* Segments are a resourse of the ILKN, which indicates the BW the ILKN port can pass. 
     * There are 4 segments in the ILKN core, which has to be divided between all ports of the same ILKN core.
     * there can be two divisions:
     * 1. 4 segments to ILKN0 (ILKN1 is not active)
     * 2. 2 segments to ILKN0 and 2 segments to ILKN1.
     * calling this function with 0 segments should only be done when the port is removed.
     */

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_0f, nof_segments > 0 ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_1f, nof_segments > 0 ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_2f, nof_segments > 2 ? 1 : 0); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_3f, nof_segments > 2 ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_0f, nof_segments > 0 ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_1f, nof_segments > 0 ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_2f, nof_segments > 2 ? 1 : 0); 
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_3f, nof_segments > 2 ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - set watermarks values to HW
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] wm_high - watermark high indication
 * \param [in] wm_low - watermark low indication
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_watermarks_set(int unit, int port, pm_info_t pm_info, uint32 wm_high, uint32 wm_low)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_FIFO_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_FIFO_CFGr, &reg_val, SLE_TX_FIFO_CFG_WM_HIGHf, wm_high);
    soc_reg_field_set(unit, ILKN_SLE_TX_FIFO_CFGr, &reg_val, SLE_TX_FIFO_CFG_WM_LOWf, wm_low);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_FIFO_CFGr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Enable inband flow control in ILKN core
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] tx_cal_len - Tx calender len
 * \param [in] rx_cal_len - Rx calender len
 *
 * \return
 *   STATIC int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_inband_fc_set(int unit, int port, pm_info_t pm_info, int tx_cal_len, int rx_cal_len)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_INBANDf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_INBANDf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CAL_INBANDr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CAL_INBANDr, &reg_val, SLE_TX_CAL_INBAND_LASTf, tx_cal_len - 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CAL_INBANDr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CAL_INBANDr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CAL_INBANDr, &reg_val, SLE_RX_CAL_INBAND_LASTf, rx_cal_len - 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CAL_INBANDr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - clear all counters in the ILKN core - for all 
 *        channels.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_counters_clear(int unit, int port, pm_info_t pm_info)
{
    uint32 reg_val;
    int ch;
    SOC_INIT_FUNC_DEFS;

    for (ch = 0; ch < OS_ILKN_MAX_NOF_CHANNELS; ++ch) 
    {
        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_CMDf, 0x4);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_ADDRf, ch);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_STATS_ACCr(unit, port, reg_val));

        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_CMDf, 0x4);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_ADDRf, ch);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_STATS_ACCr(unit, port, reg_val));
        
        sal_usleep(1000);
    }
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - ILKN general configuration to HW. here we want to 
 *        clear the configuration register just in case before
 *        we start configuring the requested values.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_general_cfg_set(int unit, int port, pm_info_t pm_info)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* SLE default values depend on LA-mode (on/off).
       just to make sure, we set these regs to 0, than configure required fields.*/
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, 0));

    /* this configuration tells the ILKN core how many cycles to wait before accessing the memory for the statistic counters*/
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_MEM_WAITf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_MEM_WAITf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val)); 
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Set Data pack indication in HW. Data pack allows the 
 *        core to pack the bursts to a specified size.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] is_data_pack - enable / disable data pack enabled
 *        logic.
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_data_pack_set(int unit, int port, pm_info_t pm_info, uint32 is_data_pack)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_DATA_PCKf, is_data_pack);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Set the default lane order for ILKN core.
 *       There is no logical lane swapping by default.
 *       lane_order[0] = 0, lane_order[1] = 1.....
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] lane_map - ILKN core logical to physical mapping info
 *
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_logical_lane_order_init(int unit, int port, pm_info_t pm_info)
{
    int core_id;
    int lane_count;
    int lane_order[OS_ILKN_TOTAL_LANES_PER_CORE] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
    portmod_pbmp_t phys_aligned;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &phys_aligned, core_id));

    PORTMOD_PBMP_COUNT(phys_aligned, lane_count);
    /* Set the default Lane order - No remapping*/
    _SOC_IF_ERR_EXIT(pmOsILKN_50G_port_logical_lane_order_set(unit, port, pm_info, lane_order, lane_count));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Get the PM lane map according to the ILKN core lane map
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] pm_id - PM id
 * \param [in] core_lane_map - ILKN core lane map
 * \param [in] phy_lane_map - PM lane map
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */

STATIC
int _pmOsILKN_50G_ilkn_pm_lane_map_get(
    int unit,
    int port,
    pm_info_t pm_info,
    int pm_index,
    pmOsIlkn_core_lane_map_t *core_lane_map,
    phymod_lane_map_t *phy_lane_map)
{
    int lane_id;

    SOC_INIT_FUNC_DEFS;

    for (lane_id = 0; lane_id < OS_ILKN_LANES_PER_PHY_CORE; ++lane_id)
    {
        phy_lane_map->lane_map_rx[lane_id] = 
            core_lane_map->rx_lane_map[lane_id + pm_index * OS_ILKN_LANES_PER_PHY_CORE] % OS_ILKN_LANES_PER_PHY_CORE;
        phy_lane_map->lane_map_tx[lane_id] = 
            core_lane_map->tx_lane_map[lane_id + pm_index * OS_ILKN_LANES_PER_PHY_CORE] % OS_ILKN_LANES_PER_PHY_CORE;
    }
    phy_lane_map->num_of_lanes = OS_ILKN_LANES_PER_PHY_CORE;

    SOC_FUNC_RETURN;
}

/**
 * \brief - attach a new ILKN port. this is the method to 
 *        configure a new port in the ILKN port
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] add_info - information about the new port from 
 *        the user.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{        
    pmOsIlkn_internal_t* ilkn_data;
    pmOsIlkn_core_lane_map_t lane_map;
    uint32 data_pack, ilkn_core_id;
    portmod_port_add_info_t add_info_copy;
    int pm, is_pm_aggregated = 0;
    SOC_INIT_FUNC_DEFS;

    /* ILKN core id is the port index inside the ILKN core. (in JER2 core_id will always be 0) */
    ilkn_core_id = add_info->ilkn_core_id;
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[ilkn_core_id]);

    /* Connect the logical port to the ILKN core interface id in WB Engine */
    _SOC_IF_ERR_EXIT(PM_ILKN_PORT_SET(unit, pm_info, port, ilkn_core_id));

    /* Set ILKN over fabric indication in WB Engine */
    _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, add_info->ilkn_port_is_over_fabric, ilkn_core_id));

    PORTMOD_PBMP_ASSIGN(ilkn_data->phys, add_info->phys);

    /* Update the aggregated PMs for the ILKN port */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_aggregated_pms_update(unit, port, pm_info));

    /* Get the ILKN core lane map */
    _SOC_IF_ERR_EXIT(
        pm_info->pm_data.pmOsIlkn_db->ilkn_core_lane_map_get(unit, port, OS_ILKN_TOTAL_LANES_PER_CORE, lane_map.rx_lane_map, lane_map.tx_lane_map));

    /* Save phys_aligned to WB engine */
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_SET(unit, pm_info, add_info->ilkn_lanes, ilkn_core_id));

    /**** Configure OpenSilicon ILKN core ****/
    /* Update the Active lanes of the ILKN port */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_lanes_config(unit, port, pm_info, &lane_map));

    /* Common CFG regs */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_general_cfg_set(unit, port, pm_info));

    /* Configure Retransmit if enabled */
    if (add_info->rx_retransmit || add_info->tx_retransmit)
    {
        _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_retransmit_config_set(unit, port, pm_info, add_info->rx_retransmit, 
                                                                 add_info->tx_retransmit, add_info->reserved_channel_rx, add_info->reserved_channel_tx)); 
    }

    /* Metaframe period */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_metaframe_period_set(unit, port, pm_info, add_info->ilkn_metaframe_period));
    
    /* Segments enable */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_nof_segments_set(unit, port, pm_info, add_info->ilkn_nof_segments));

    /* Set Watermarks */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_watermarks_set(unit, port, pm_info, ilkn_data->wm_high, ilkn_data->wm_low));

    /* Set Flow Control */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_inband_fc_set(unit, port, pm_info, add_info->ilkn_inb_cal_len_tx, add_info->ilkn_inb_cal_len_rx));

    /* Reset all SLE counters */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_counters_clear(unit, port, pm_info));

    /* Configure PMs below */
    sal_memcpy(&add_info_copy, add_info, sizeof(portmod_port_add_info_t));
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, ilkn_core_id, pm));
        if (is_pm_aggregated) {
            /* Get the PM lane map from ILKN core lane map */
            _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_pm_lane_map_get(unit, port, pm_info, pm, &lane_map, add_info_copy.init_config.lane_map));
            add_info_copy.init_config.lane_map_overwrite=1;

            /* call PM below core add method */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_core_add(unit, ilkn_data->pm_ids[pm], ilkn_data->pms[pm], &add_info_copy));
            /* Call PM below port attach method */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_attach(unit, port, ilkn_data->pms[pm], &add_info_copy));
        }
    }
    /* Configure Burst parameters */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_burst_config_set(unit, port, pm_info, add_info->ilkn_burst_max, add_info->ilkn_burst_min, add_info->ilkn_burst_short));

    /* Set Data Pack for ELK ports */
    data_pack = (PORTMOD_PORT_ADD_F_ELK_GET(add_info)) ? 1 : 0;
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_data_pack_set(unit, port, pm_info, data_pack));

    

    /* Set the default ILKN logcial lane mapping */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_logical_lane_order_init(unit, port, pm_info));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Detach an existing ILKN port from the ILKN core.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_detach(int unit, int port, pm_info_t pm_info)
{        
    int enable, core_id, pm, is_pm_aggregated, temp_enable, burst_max, burst_min, burst_short;
    int wm_high, wm_low, metaframe_period;
    pmOsIlkn_internal_t* ilkn_data;
    portmod_pbmp_t temp_phys_aligned;
    uint64 reg_default;
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    _SOC_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, &enable));
    if (enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }

    

    

    /* Set Data Pack to 0 */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_data_pack_set(unit, port, pm_info, 0));

    /* Configure Burst parameters  to default values */
    SOC_REG_RST_VAL_GET(unit, ILKN_SLE_RX_BURSTr, reg_default);
    reg_val = COMPILER_64_LO(reg_default);
    burst_short = soc_reg_field_get(unit, ILKN_SLE_RX_BURSTr, reg_val, SLE_RX_BURST_SHORTf);
    burst_min = soc_reg_field_get(unit, ILKN_SLE_RX_BURSTr, reg_val, SLE_RX_BURST_MINf);
    burst_max = soc_reg_field_get(unit, ILKN_SLE_RX_BURSTr, reg_val, SLE_RX_BURST_MAXf);
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_burst_config_set(unit, port, pm_info, burst_max, burst_min, burst_short));

    /* call PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_detach(unit, port, ilkn_data->pms[pm])); 
        }
        temp_enable = 0;
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, temp_enable, core_id, pm));
    }

    /* Set Watermarks */
    SOC_REG_RST_VAL_GET(unit, ILKN_SLE_TX_FIFO_CFGr, reg_default);
    reg_val = COMPILER_64_LO(reg_default);
    wm_high = soc_reg_field_get(unit, ILKN_SLE_TX_FIFO_CFGr, reg_val, SLE_TX_FIFO_CFG_WM_HIGHf);
    wm_low = soc_reg_field_get(unit, ILKN_SLE_TX_FIFO_CFGr, reg_val, SLE_TX_FIFO_CFG_WM_LOWf);
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_watermarks_set(unit, port, pm_info, wm_high, wm_low));

    /* Segments enable */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_nof_segments_set(unit, port, pm_info, 0));

    /* Metaframe period */
    SOC_REG_RST_VAL_GET(unit, ILKN_SLE_TX_METAFRAMEr, reg_default);
    reg_val = COMPILER_64_LO(reg_default);
    metaframe_period = soc_reg_field_get(unit, ILKN_SLE_TX_METAFRAMEr, reg_val, SLE_TX_METAFRAME_PERIODf);
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_metaframe_period_set(unit, port, pm_info, metaframe_period));

    /* Configure Retransmit Properties to 0 */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_retransmit_config_set(unit, port, pm_info, 0, 0, 0, 0)); 

    /* Update the Active lanes of the ILKN port to 0 */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_enabled_lanes_set(unit, port, 0, 0));
    _SOC_IF_ERR_EXIT(_pmOSILKN_50G_ilkn_stall_selection_set(unit, port, 0, 0));

    /* Disable - clean DB */
    PORTMOD_PBMP_CLEAR(ilkn_data->phys);
    /* Clear WB engine */
    PORTMOD_PBMP_CLEAR(temp_phys_aligned);
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_SET(unit, pm_info, temp_phys_aligned, core_id));
    temp_enable = -1; /* -1 = not_applicable */
    _SOC_IF_ERR_EXIT(PM_ILKN_PORT_SET(unit, pm_info, temp_enable, core_id));
    temp_enable = 0;
    _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, temp_enable, core_id));
        
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - method to retrieve phy access information, which can 
 *        be used to call Phymod APIs
 * 
 * \param [in] unit - chip unti id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] params - specifications for access structure to 
 *        be returned
 * \param [in] max_phys - how many phy_access elements are given 
 *        in output array
 * \param [out] access - output phy access array
 * \param [out] nof_phys - how many phy access elements were 
 *        returned
 * \param [out] is_most_ext - this pointer can be NULL. it is 
 *        expected to return an indication whether the output
 *        phy access include the most external phy as well. in
 *        JER2 there is no account for external phys so this
 *        indication will always return true (internal phy is
 *        always the most external..)
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* access, int* nof_phys, int* is_most_ext)
{        
    int pm, core_id, arr_inx, rv, is_pm_aggregated=0;
    int pm_index, lane_index, active_lane_in_ilkn_port = 0, lane_pm_found = 0, active_lane_in_pm = 0, pm_of_lane_index = OS_ILKN_PM_NOT_FOUND;
    pmOsIlkn_internal_t *ilkn_data;
    portmod_access_get_params_t params_copy;    
    portmod_pbmp_t phys_aligned;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &phys_aligned, core_id));

    sal_memcpy(&params_copy, params, sizeof(portmod_access_get_params_t));

    if (params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE)
    {
        
        pm_of_lane_index = OS_ILKN_PM_NOT_FOUND;

        active_lane_in_ilkn_port = 0;
        for (pm_index = 0 ; pm_index < OS_ILKN_MAX_ILKN_AGGREGATED_PMS ; pm_index++)
        {
            active_lane_in_pm = 0;
            for (lane_index = 0 ; lane_index < OS_ILKN_LANES_PER_PHY_CORE ; lane_index++)
            {
                if (PORTMOD_PBMP_MEMBER(phys_aligned, (pm_index * OS_ILKN_LANES_PER_PHY_CORE + lane_index)))
                {
                    if (params->lane== active_lane_in_ilkn_port)
                    {
                        params_copy.lane = active_lane_in_pm;
                        pm_of_lane_index = pm_index;
                        break;
                    }
                    active_lane_in_ilkn_port++;
                    active_lane_in_pm++;
                }
            }

            if (pm_of_lane_index != OS_ILKN_PM_NOT_FOUND)
            {
                break;
            }
        }
    }

    if (is_most_ext) 
    {
        /* Jer2 does not account for external phys.
           Hence, internal_phy is always the most external */
        *is_most_ext = 1;
    }

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&access[0]));
    *nof_phys = arr_inx = 0;

    /* call PMs below - aggregate here */    
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) 
    {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) 
        {
            if (arr_inx >= max_phys) 
            {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("access size %d isn't big enough"), max_phys));
            }
            /* ILKN counts lanes starting from first actual lane, 
               as oppose to the PM below which counts from first active lane.
               here we subtract the first non-active lanes before calling PM below*/
            if (params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE)
            {
                if (pm != pm_of_lane_index)
                {
                    /*the lane we are looking for is not in this PM*/
                    continue;
                }

                /* mark we found the lane position */
                lane_pm_found = 1;
            }
            rv = __portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_phy_lane_access_get(unit, port, 
                                                                                    ilkn_data->pms[pm], &params_copy, max_phys - arr_inx, access + arr_inx, nof_phys, NULL);
            _SOC_IF_ERR_EXIT(rv);

            arr_inx += *nof_phys;
        }
    }

    if ((params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE) && (!lane_pm_found))
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("No PM was found for lane %d of port %d"), params->lane, port));
    }

    *nof_phys = arr_inx; 

exit:
    SOC_FUNC_RETURN;
}


/**
 * \brief - Get the logical lane order info.
 *    Using lane order of Rx side is enough, as we can get the
 *    same order from tx side.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] lane_order_max_size - max lanes of the ILKN port
 * \param [out] lane_order - lane order array.
 * \param [out] lane_order_actual_size - actual lane order size
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_logical_lane_order_get(int unit, int port, pm_info_t pm_info, int lane_order_max_size, int* lane_order, int* lane_order_actual_size)
{
    int field_index, reg_index;
    int core_id, lane_id, serdes_id, count, nof_lanes;
    int rx_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int reverse_rx_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int active_rx_serdes[OS_ILKN_TOTAL_LANES_PER_CORE] = { 0 };
    uint32 reg_val;
    pmOsIlkn_core_lane_map_t lane_map;
    portmod_pbmp_t active_rx_serdes_bmp;
    portmod_pbmp_t phys_aligned;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &phys_aligned, core_id));
    /*
     * Set the default mapping, one to one mapping
     * This can ensure it is still one to one mapping after
     * reversing rx_lane_order.
     */
    for (serdes_id = 0; serdes_id < OS_ILKN_TOTAL_LANES_PER_CORE; ++serdes_id)
    {
        rx_lane_order[serdes_id] = serdes_id;
    }
    /*
     * Get the lane map info for ILKN core
     */
    _SOC_IF_ERR_EXIT(
        pm_info->pm_data.pmOsIlkn_db->ilkn_core_lane_map_get(unit, port, OS_ILKN_TOTAL_LANES_PER_CORE, lane_map.rx_lane_map, lane_map.tx_lane_map));

    PORTMOD_PBMP_CLEAR(active_rx_serdes_bmp);
    count = 0;
    PORTMOD_PBMP_ITER(phys_aligned, lane_id)
    {
        /* coverity protection */
        if (lane_id > OS_ILKN_TOTAL_LANES_PER_CORE - 1) {
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("lane_id %d is out of bounds"), lane_id));
        }
        /* calc relevant reg and field */
        field_index = lane_map.rx_lane_map[lane_id];
        reg_index = field_index / OS_ILKN_LANE_MAP_REGISTER_RESOLUTION;

        /* get rx remap */
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[reg_index], port, 0, &reg_val));
        rx_lane_order[field_index] = soc_reg_field_get(unit, remap_lanes_rx_regs[reg_index], reg_val, remap_lanes_rx_fields[field_index]);
        /*
         * Get the active Rx serdes bitmap
         */
        PORTMOD_PBMP_PORT_ADD(active_rx_serdes_bmp, lane_map.rx_lane_map[lane_id]);
        count++;
    }

    /*
     * Reverse the rx serdes lane order to conclude the logical lane order,
     * only using rx mapping is enough, as we can get the same result using
     * tx mapping.
     */
    for (serdes_id = 0; serdes_id < OS_ILKN_TOTAL_LANES_PER_CORE; ++serdes_id)
    {
        reverse_rx_lane_order[rx_lane_order[serdes_id]] = serdes_id;
    }
    /*
     * Set the serdes array by order
     * For exampe, the active serdes bitmap is b'101010,
     * the following info should be stored:
     * active_rx_serdes[1] = 0;
     * active_rx_serdes[3] = 1;
     * active_rx_serdes[5] = 2;
     */
    count = 0;
    PORTMOD_PBMP_ITER(active_rx_serdes_bmp, serdes_id)
    {
        active_rx_serdes[serdes_id] = count;
        count++;
    }
    /*
     * Get the ILKN mapping info
     */
    count = 0;
    PORTMOD_PBMP_ITER(phys_aligned, lane_id)
    {
        /* coverity protection */
        if (lane_id > OS_ILKN_TOTAL_LANES_PER_CORE - 1) {
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("lane_id %d is out of bounds"), lane_id));
        }
        serdes_id = reverse_rx_lane_order[lane_map.rx_lane_map[lane_id]];
        lane_order[count] = active_rx_serdes[serdes_id];
        count++;
    }
    _SOC_IF_ERR_EXIT(portmod_port_nof_lanes_get(unit, port, &nof_lanes));
    *lane_order_actual_size = lane_order_max_size < nof_lanes ? lane_order_max_size : nof_lanes;

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Write the rx/tx lane order info to registers.
 *     All the invalid lanes should be one to one mapping.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] lane_order - lane order array, logical lane order
 *     For example, lane_oder[0] represents for the 1st lane of
 *     the ILKN port.
 * \param [in] lane_order_size - active lanes in for ILKN port
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_logical_lane_order_set(int unit, int port, pm_info_t pm_info, const int* lane_order, int lane_order_size)
{
    int rx_field_index, tx_field_index, rx_reg_index, tx_reg_index;
    int core_id, lane_id, serdes_id;
    int count, remap_lane;
    int active_rx_serdes[OS_ILKN_TOTAL_LANES_PER_CORE] = { 0 };
    int active_tx_serdes[OS_ILKN_TOTAL_LANES_PER_CORE] = { 0 };
    int rx_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int tx_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    uint32 reg_val;
    pmOsIlkn_core_lane_map_t lane_map;
    portmod_pbmp_t active_rx_serdes_bmp, active_tx_serdes_bmp;
    portmod_pbmp_t phys_aligned;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &phys_aligned, core_id));

    /*
     * Get the lane map info for ILKN core
     */
    _SOC_IF_ERR_EXIT(
        pm_info->pm_data.pmOsIlkn_db->ilkn_core_lane_map_get(unit, port, OS_ILKN_TOTAL_LANES_PER_CORE, lane_map.rx_lane_map, lane_map.tx_lane_map));

    /*
     * Convert the active lanes to active serdes bitmap
     */
    PORTMOD_PBMP_CLEAR(active_rx_serdes_bmp);
    PORTMOD_PBMP_CLEAR(active_tx_serdes_bmp);
    PORTMOD_PBMP_ITER(phys_aligned, lane_id)
    {
        /* coverity protection */
        if (lane_id > OS_ILKN_TOTAL_LANES_PER_CORE - 1) {
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("lane_id %d is out of bounds"), lane_id));
        }
        /* Store rx and tx serdes bmp */
        PORTMOD_PBMP_PORT_ADD(active_rx_serdes_bmp, lane_map.rx_lane_map[lane_id]);
        PORTMOD_PBMP_PORT_ADD(active_tx_serdes_bmp, lane_map.tx_lane_map[lane_id]);
    }

    /*
     * Store the serdes ID by order
     * For exampe, the active serdes bitmap is b'101010,
     * the following info should be stored:
     * active_rx_serdes[0] = 1;
     * active_rx_serdes[1] = 3;
     * active_rx_serdes[2] = 5;
     */
    count = 0;
    PORTMOD_PBMP_ITER(active_rx_serdes_bmp, serdes_id)
    {
        active_rx_serdes[count] = serdes_id;
        count++;
    }
    count = 0;
    PORTMOD_PBMP_ITER(active_tx_serdes_bmp, serdes_id)
    {
        active_tx_serdes[count] = serdes_id;
        count++;
    }
    count = 0;
    PORTMOD_PBMP_ITER(phys_aligned, lane_id)
    {
        /* coverity protection */
        if (lane_id > OS_ILKN_TOTAL_LANES_PER_CORE - 1) {
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("lane_id %d is out of bounds"), lane_id));
        }
        /*
         * Get the remap logical lane id
         * For exampe:
         * lane_order[4] = 2 (all the values are zero-based)
         * this means the 5th lane of the ILKN port is remapped
         * to the 3rd lane.
         * remap_lane = 2
         *
         */
        remap_lane = lane_order[count];
        /*
         * Set the lane order according to the remapped lane id
         */

        rx_lane_order[active_rx_serdes[remap_lane]] = lane_map.rx_lane_map[lane_id];
        tx_lane_order[active_tx_serdes[remap_lane]] = lane_map.tx_lane_map[lane_id];

        /*
         * Config lane remap register, rx and tx register should be configured seperately
         */
        rx_field_index = active_rx_serdes[remap_lane];
        tx_field_index = active_tx_serdes[remap_lane];

        rx_reg_index = rx_field_index / OS_ILKN_LANE_MAP_REGISTER_RESOLUTION;
        tx_reg_index = tx_field_index / OS_ILKN_LANE_MAP_REGISTER_RESOLUTION;

        /* configure rx remap */
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[rx_reg_index], port, 0, &reg_val));
        soc_reg_field_set(unit, remap_lanes_rx_regs[rx_reg_index], &reg_val, remap_lanes_rx_fields[rx_field_index], rx_lane_order[rx_field_index]);
        _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_rx_regs[rx_reg_index], port, 0, reg_val));

        /* configure tx remap */
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_tx_regs[tx_reg_index], port, 0, &reg_val));
        soc_reg_field_set(unit, remap_lanes_tx_regs[tx_reg_index], &reg_val, remap_lanes_tx_fields[tx_field_index], tx_lane_order[tx_field_index]);
        _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_tx_regs[tx_reg_index], port, 0, reg_val));

        count++;
    }
exit:
    SOC_FUNC_RETURN;

}

int pmOsILKN_50G_port_speed_config_set(int unit, int port, pm_info_t pm_info, const portmod_speed_config_t* speed_config)
{
    pmOsIlkn_internal_t* ilkn_data;
    int core_id, pm, is_pm_aggregated = 0;
    uint32 nof_pipes = 0;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_speed_config_set(unit, port, ilkn_data->pms[pm], speed_config));
        }
    }

    /* pipe enable */
    /* enable num_lanes pipes, or all pipes */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_port_num_pipes_get(unit, port, pm_info, &nof_pipes));
    _SOC_IF_ERR_EXIT(pmOsILKN_port_pipe_config(unit, port, pm_info, nof_pipes));

exit:
	SOC_FUNC_RETURN;
}

int pmOsILKN_50G_port_speed_config_get(int unit, int port, pm_info_t pm_info, portmod_speed_config_t* speed_config)
{
	pmOsIlkn_internal_t* ilkn_data;
	int core_id, pm, is_pm_aggregated = 0;
	int pm_found = 0;
	SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_speed_config_get(unit, port, ilkn_data->pms[pm], speed_config));
            pm_found = 1;
            break;
        }
    }

    
    speed_config->fec = PORTMOD_PORT_PHY_FEC_NONE;

    if (!pm_found) {
        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("aggregated PM not found for port %d"), port));
    }

exit:
	SOC_FUNC_RETURN;
}

/* enable credits for ILKN over fabric port - credits from SerDes Tx to ILKN core */
int pmOsILKN_50G_port_credits_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    pmOsIlkn_internal_t* ilkn_data;
    int core_id, pm, is_pm_aggregated = 0;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_credits_enable_set(unit, port, ilkn_data->pms[pm], enable));
        }
    }

exit:
	SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM_OS_ILKN_50G_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
