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
*    dingyi    2017-1-3      1.0           initial
*
******************************************************************************/
#ifndef CCU_RX_TX_H
#define CCU_RX_TX_H

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
extern RET_STATUS ccu_rx_ssm_req_set(UINT_8 chip_id);
extern RET_STATUS ccu_rx_frame_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_drop_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_filter_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_frm_err_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_len_err_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_pkt_in_num_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_sync_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_delay_req_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_pdelay_req_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_pdelay_resp_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_follow_up_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_delay_resp_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_pdelay_follow_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_announce_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_signaling_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_management_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_ssm_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_resv_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_decode_err_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_decode_spkt_byte_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_decode_spkt_num_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_ssm_message_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_alarm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_alarm_all_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_rx_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_rx_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_ssm_req_set(UINT_8 chip_id);
extern RET_STATUS ccu_tx_frame_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_drop_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_filter_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_err_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_len_err_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_ssm_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_ptp_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_sync_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_delay_req_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_pdelay_req_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_pdelay_resp_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_follow_up_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_delay_resp_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_pdelay_follow_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_announce_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_signaling_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_management_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_resv_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_ssm_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_encode_err_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_ssm_message_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_encode_err_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_encode_err_alm_all_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_encode_err_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_tx_encode_err_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_tx_encode_err_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_tx_encode_err_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_rx_cnt_get(UINT_8 chip_id,UINT_8 cnt_type,UINT_8 field_id,UINT_32* parameter);
#if 0 
extern RET_STATUS ccu_tx_cnt_get(UINT_8 chip_id,UINT_8 cnt_type,UINT_32* parameter);
#endif

#ifdef __cplusplus
}
#endif

#endif

