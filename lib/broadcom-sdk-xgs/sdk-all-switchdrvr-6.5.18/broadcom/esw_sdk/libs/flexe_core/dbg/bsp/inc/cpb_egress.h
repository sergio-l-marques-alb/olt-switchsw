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
*    dingyi    2017-12-21   1.0            initial
*
******************************************************************************/
#ifndef CPB_EGRESS_H
#define CPB_EGRESS_H

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

extern RET_STATUS cpb_egress_sel_tb_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS cpb_egress_sel_tb_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_sq_err_eg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS cpb_egress_sq_err_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_mon_en_eg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS cpb_egress_mon_en_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_clr_eg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS cpb_egress_clr_eg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_cnt_max_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_cpb_ilk_bp_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_link_full_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_link_empty_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_sar_pathdelay_chan_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS cpb_egress_sar_pathdelay_chan_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_dly_fifo_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_sta_clr_eg_pls_set(UINT_8 chip_id,UINT_8 filed_id);
extern RET_STATUS cpb_egress_cnt_depth_eg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_sar_pathdelay_stat_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_sar_pathdelay_real_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_sta_clr_ch_eg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS cpb_egress_sta_clr_ch_eg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_star_blk_eg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_star_blk_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_next_blk_eg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_next_blk_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_full_depth_egress_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_full_depth_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_bp_level_egress_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_bp_level_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_rd_level_egress_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_rd_level_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_cell_in_cnt_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_cell_out_cnt_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_high_depth_eg_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_high_depth_eg_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_low_depth_eg_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_low_depth_eg_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_aempty_depth_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_aempty_depth_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_bp_low_egress_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_bp_low_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_err_cnt_eg_tb_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_all_cnt_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS cpb_egress_eg_int_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS cpb_egress_eg_int_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS cpb_egress_eg_int_mask_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS cpb_egress_eg_int_mask_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif

#endif

