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
*    dingyi    2017-1-10      1.0           initial
*
******************************************************************************/
#ifndef INF_CH_ADP_RX_H
#define INF_CH_ADP_RX_H

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

extern RET_STATUS inf_ch_adp_rx_map_table_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_rx_map_table_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_rx_ts_delta_set(UINT_8 chip_id,UINT_8 low_high,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_rx_ts_delta_get(UINT_8 chip_id,UINT_8 low_high,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_rx_mode_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_rx_mode_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_rx_mode_cfg_400g_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_rx_mode_cfg_400g_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_rx_mode_cfg_200g_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_rx_mode_cfg_200g_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_rx_lf_sel_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_rx_lf_sel_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_rx_port_restart_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_rx_port_restart_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_rx_gearbox_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif

#endif

