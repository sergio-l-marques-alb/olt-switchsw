/******************************************************************************
*
* FILE
*
*
* DESCRIPTION
*
*
* NOTE
*
*
******************************************************************************/



/******************************************************************************
*HISTORY OF CHANGES
*******************************************************************************
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    2015-10-22   1.0            initial
*
******************************************************************************/
#ifndef FLEXE_DEMUX_H
#define FLEXE_DEMUX_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "datatype.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/



/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/


/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/

#ifdef __cplusplus
extern "C" 
{
#endif

extern RET_STATUS flexe_demux_ts_en_set(UINT_8 chip_id,UINT_8 ts_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_ts_en_get(UINT_8 chip_id,UINT_8 ts_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_group_info_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_group_info_sync_set(UINT_8 chip_id, UINT_32 parameter);
extern RET_STATUS flexe_demux_group_info_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_group_info_sync_get(UINT_8 chip_id, UINT_32* parameter);
extern RET_STATUS flexe_demux_group_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_group_cfg_sync_set(UINT_8 chip_id, UINT_32 parameter);
extern RET_STATUS flexe_demux_group_cfg_sync_get(UINT_8 chip_id, UINT_32* parameter);
extern RET_STATUS flexe_demux_group_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_reorder_asic_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_reorder_asic_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_autoset_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_autoset_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_mode_info_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_mode_info_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_reorder_info_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_reorder_info_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_calendar_cfg_set(UINT_8 chip_id,UINT_8 ts_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_calendar_cfg_get(UINT_8 chip_id,UINT_8 ts_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_channel_own_tsgroup_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_channel_own_tsgroup_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_ram_full_alm_get(UINT_8 chip_id,UINT_8 ram_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_fifo_full_alm_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_fifo_empty_alm_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_fp_check_alm_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_head_check_phy_alm_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_head_check_channel_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_wcfgram_set(UINT_8 chip_id,UINT_8 table_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_wcfgram_get(UINT_8 chip_id,UINT_8 table_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_rdrule_set(UINT_8 chip_id,UINT_32 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_rdrule_get(UINT_8 chip_id,UINT_32 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_ts_num_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_ts_num_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_blk_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_blk_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_cycle_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_cycle_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_maxlevel_interleave_get(UINT_8 chip_id,UINT_8 fifo_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_maxlevel_clr_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_maxlevel_clr_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_interleave_rst_set(UINT_8 chip_id,UINT_8 fifo_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_interleave_rst_sync_set(UINT_8 chip_id, UINT_32 parameter);
extern RET_STATUS flexe_demux_ts_en_b_set(UINT_8 chip_id,UINT_8 ts_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_ts_en_b_get(UINT_8 chip_id,UINT_8 ts_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_calendar_cfg_b_set(UINT_8 chip_id,UINT_8 ts_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_calendar_cfg_b_get(UINT_8 chip_id,UINT_8 ts_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_channel_own_tsgroup_b_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_channel_own_tsgroup_b_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_enable_ab_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_enable_ab_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_verify_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_verify_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_localfault_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_localfault_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_threshold_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_threshold_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_threshold_full_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_threshold_full_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_threshold_empty_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_demux_threshold_empty_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_wcfgram_b_set(UINT_8 chip_id,UINT_8 table_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_wcfgram_b_get(UINT_8 chip_id,UINT_8 table_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_rdrule_b_set(UINT_8 chip_id,UINT_32 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_rdrule_b_get(UINT_8 chip_id,UINT_32 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_b_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS flexe_demux_property_b_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif


#endif


