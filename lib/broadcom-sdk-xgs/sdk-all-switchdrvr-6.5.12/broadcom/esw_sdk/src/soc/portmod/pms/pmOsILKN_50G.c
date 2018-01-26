/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <soc/types.h>
#include <soc/error.h>
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
int _pmOsILKN_50G_ilkn_enabled_lanes_set(int unit, int port, uint32 enabled_lanes)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    
    /* The register has reverse logic  - so we negate the lanes bitmap */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_LANEr, &reg_val, SLE_TX_LANE_DISABLEf, ~enabled_lanes);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_LANEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_LANEr, &reg_val, SLE_RX_LANE_DISABLEf, ~enabled_lanes);
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
int _pmOsILKN_50G_ilkn_lanes_config(int unit, int port, pm_info_t pm_info)
{
    portmod_pbmp_t mirror_bmp, phys_aligned;
    int core_id, port_i, lane, lane_pos, last_lane_pos, last_lane, mirror_lane;
    uint32 bmp, lane_count;
    pmOsIlkn_internal_t* ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* Lane Disable - Reverse Logic */
    PORTMOD_PBMP_CLEAR(mirror_bmp);
    PORTMOD_PBMP_CLEAR(phys_aligned);

    PORTMOD_PBMP_ITER(ilkn_data->phys, port_i)
    {
        /* Align to lane numbers per ILKN core */
        lane =  port_i % OS_ILKN_TOTAL_LANES_PER_CORE;
        PORTMOD_PBMP_PORT_ADD(phys_aligned, lane);
        /* For ILKN port 1 -> bitmap should be mirrored */
        mirror_lane = (core_id) ? OS_ILKN_TOTAL_LANES_PER_CORE - 1 - lane: lane;
        
        PORTMOD_PBMP_PORT_ADD(mirror_bmp, mirror_lane);
    }

    /* Save phys_aligned to WB engine */
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_SET(unit, pm_info, phys_aligned, core_id));

    bmp = SOC_PBMP_WORD_GET(mirror_bmp, 0);
    /* Set enabled lanes in HW */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_enabled_lanes_set(unit, port, bmp));

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
    uint32 data_pack, ilkn_core_id;
    portmod_port_add_info_t add_info_copy;
    int pm, is_port_over_fabric, is_pm_aggregated = 0;
    SOC_INIT_FUNC_DEFS;

    /* ILKN core id is the port index inside the ILKN core. (in JER2 core_id will always be 0) */
    ilkn_core_id = add_info->ilkn_core_id;
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[ilkn_core_id]);

    /* Connect the logical port to the ILKN core interface id in WB Engine */
    _SOC_IF_ERR_EXIT(PM_ILKN_PORT_SET(unit, pm_info, port, ilkn_core_id));

    /* Set ILKN over fabric indication in WB Engine */
    is_port_over_fabric = add_info->ilkn_port_is_over_fabric;
    _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, add_info->ilkn_port_is_over_fabric, ilkn_core_id));

    PORTMOD_PBMP_ASSIGN(ilkn_data->phys, add_info->phys);

    /* Update the aggregated PMs for the ILKN port */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_aggregated_pms_update(unit, port, pm_info));
    
    /**** Configure OpenSilicon ILKN core ****/
    /* Update the Active lanes of the ILKN port */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_lanes_config(unit, port, pm_info));

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

    /* Reset all SLE counters */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_counters_clear(unit, port, pm_info));

    /* Configure PMs below */
    PHYMOD_MEMCPY(&add_info_copy, add_info, sizeof(portmod_port_add_info_t));
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, ilkn_core_id, pm));
        if (is_pm_aggregated) {

            /* Set PM to work in Bypass mode */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_pm_bypass_set(unit, ilkn_data->pms[pm], 1));

            /* Call PM below port attach method */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_attach(unit, port, ilkn_data->pms[pm], &add_info_copy));
        }
    }
    /* Configure Burst parameters */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_burst_config_set(unit, port, pm_info, add_info->ilkn_burst_max, add_info->ilkn_burst_min, add_info->ilkn_burst_short));

    /* Set Data Pack for ELK ports */
    data_pack = (PORTMOD_PORT_ADD_F_ELK_GET(add_info)) ? 1 : 0;
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_data_pack_set(unit, port, pm_info, data_pack));

    

    

    

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
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_enabled_lanes_set(unit, port, 0));
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
    int pm, core_id, arr_inx, rv, is_pm_aggregated=0, first_active_phy_in_pm;
    int lane_index, active_lane_counter, lane_pm_found = 0, pm_of_lane_index = 0;
    pmOsIlkn_internal_t *ilkn_data;
    portmod_access_get_params_t params_copy;    
    portmod_pbmp_t phys_aligned, pm_active_phys, pm_phys;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &phys_aligned, core_id));

    PHYMOD_MEMCPY(&params_copy, params, sizeof(portmod_access_get_params_t));

    /* param.lane is counting active lanes for the port while PM below functions are counting
     * all lanes, including the non-active lanes. Therefore, the below loops translates:
     * params.lane (ignoring active lanes) => params_copy.lane (counting active lanes) */
    if (params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE)
    {
        active_lane_counter = -1;
        PORTMOD_PBMP_ITER(phys_aligned, lane_index) 
        {
            ++active_lane_counter;
            if (active_lane_counter == params->lane)
                break;
        }
        params_copy.lane = lane_index;
        pm_of_lane_index = lane_index / OS_ILKN_LANES_PER_PHY_CORE;
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

                /* get all the PM lanes and isolate only the ILKN active lanes in this PM */
                _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_pm_phys_get(unit, ilkn_data->pms[pm], &pm_phys));
                PORTMOD_PBMP_ASSIGN(pm_active_phys, pm_phys);
                PORTMOD_PBMP_AND(pm_active_phys, ilkn_data->phys);

                /* get the first active lane in this PM */
                PORTMOD_PBMP_ITER(pm_active_phys, first_active_phy_in_pm)
                {
                    break;
                }
                /* align the first phy to 0-24 base */
                first_active_phy_in_pm %= OS_ILKN_TOTAL_LANES_PER_CORE;

                /* subtract the first active lane from the params lane*/
                params_copy.lane -= first_active_phy_in_pm;
            }
            rv = __portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_phy_lane_access_get(unit, port, 
                                                                                    ilkn_data->pms[pm], &params_copy, max_phys - arr_inx, access + arr_inx, nof_phys, NULL);
            _SOC_IF_ERR_EXIT(rv);

            PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(access[arr_inx].device_op_mode);
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


#endif /* PORTMOD_PM_OS_ILKN_50G_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
