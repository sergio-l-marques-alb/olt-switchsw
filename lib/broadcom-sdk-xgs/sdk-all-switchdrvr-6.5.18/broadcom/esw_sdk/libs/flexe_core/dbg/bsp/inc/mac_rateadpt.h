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
*    dingyi    2017-1-17      1.0           initial
*
******************************************************************************/
#ifndef MAC_RATEADPT_ENV_H
#define MAC_RATEADPT_ENV_H

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
extern RET_STATUS mac_rateadpt_fifo_level_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS mac_rateadpt_fifo_level_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS mac_rateadpt_full_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS mac_rateadpt_empty_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS mac_rateadpt_ram_overflow_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS mac_rateadpt_block_type_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS mac_rateadpt_block_type_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mac_rateadpt_clr_hist_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS mac_rateadpt_clr_hist_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mac_rateadpt_lvl_hist_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mac_adj_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mac_adj_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mac_adj_cnt_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mac_adj_core_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mac_rateadp_block_num_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mac_rateadp_block_num_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mac_rateadp_chan_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mac_rateadp_chan_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mac_rateadp_map_reg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS mac_rateadp_map_reg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mac_rateadp_ind_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS mac_rateadp_ind_sel_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mac_rateadp_ins_del_gap_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS mac_rateadp_ins_del_gap_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
#ifdef __cplusplus
}
#endif

#endif

