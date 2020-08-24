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
*    liming    2015-10-22   1.0            initial
*
******************************************************************************/
#ifndef B66SAR_RX_TX_H
#define B66SAR_RX_TX_H

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

extern RET_STATUS b66sar_rx_loopback_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_loopback_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_tx_b66size_29b_set(UINT_8 chip_id,UINT_8 direction,UINT_32 parameter);
extern RET_STATUS b66sar_rx_tx_b66size_29b_get(UINT_8 chip_id,UINT_8 direction,UINT_32* parameter);
extern RET_STATUS b66sar_rx_pkt_cnt_probe_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_pkt_cnt_probe_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_pkt_cnt_probe_256_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_pkt_cnt_probe_256_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_bdcome_fc_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_bdcome_fc_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_pkt_cnt_256_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_ctrl_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_ctrl_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_cpb2sar_cfg4_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_cpb2sar_cfg4_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_cpb2sar_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_cpb2sar_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_pkt_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_path_delay_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_sar_rx_ch_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 alm_id,UINT_32* parameter);
extern RET_STATUS b66sar_sar_rx_glb_alm_get(UINT_8 chip_id,UINT_8 alm_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_cpb2sar_table_switch_set(UINT_8 chip_id,UINT_8 field_id);
extern RET_STATUS b66sar_rx_cpb2sar_cfg3_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_cpb2sar_cfg3_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_cpb2sar_ram_set(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter);
extern RET_STATUS b66sar_rx_cpb2sar_ram_get(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter);
extern RET_STATUS b66sar_rx_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_fifo_err_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_fifo_err_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_fifo_err_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_fifo_err_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_loopback_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_loopback_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_cnt_probe_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_cnt_probe_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_bdcome_loopback_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_bdcome_loopback_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_ra2sar_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_ra2sar_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_ctrl_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_ctrl_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_pkt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_pkt_256_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_ra2sar_table_switch_set(UINT_8 chip_id,UINT_8 field_id);
extern RET_STATUS b66sar_tx_ra2sar_env_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_int_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_int_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_int_mask_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_int_mask_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_ra2sar_cfg3_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_ra2sar_cfg3_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_ra2sar_ram_set(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter);
extern RET_STATUS b66sar_tx_ra2sar_ram_get(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter);
extern RET_STATUS b66sar_rx_cpb_m_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_cpb_m_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_rx_cpb2sar_cfg_311m_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_rx_cpb2sar_cfg_311m_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_ra_m_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_ra_m_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66sar_tx_ra2sar_cfg_311m_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS b66sar_tx_ra2sar_cfg_311m_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif


#endif
