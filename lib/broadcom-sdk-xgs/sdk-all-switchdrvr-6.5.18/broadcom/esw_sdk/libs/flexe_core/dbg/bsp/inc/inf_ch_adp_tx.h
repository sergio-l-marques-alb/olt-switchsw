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
#ifndef INF_CH_ADP_TX_H
#define INF_CH_ADP_TX_H

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

extern RET_STATUS inf_ch_adp_tx_port_fifo_cfg2_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_port_fifo_cfg2_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_port_fifo_cfg1_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_port_fifo_cfg1_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_max_min_clr_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_max_min_clr_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_port_protect_en_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_port_protect_en_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_port_restart_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_port_restart_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_map_table_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_map_table_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_map_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_map_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_external_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_external_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_port_fifo_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_gearbox_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_port_fifo_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_map_ram_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_map_ram_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_mode_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_mode_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_mode_cfg_400g_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_mode_cfg_400g_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_mode_cfg_200g_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_mode_cfg_200g_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_mode_cfg_bypass_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_mode_cfg_bypass_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_port_buffer_int_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_port_buffer_int_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS inf_ch_adp_tx_port_buffer_int_mask_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS inf_ch_adp_tx_port_buffer_int_mask_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif

#endif

