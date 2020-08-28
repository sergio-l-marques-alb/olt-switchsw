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
*    taos    2017-1-11      1.0           initial
*
******************************************************************************/
#ifndef PTP_OVER_OH_H
#define PTP_OVER_OH_H

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
extern RET_STATUS global_ieee1588_global_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS global_ieee1588_global_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS global_ieee1588_ptp_ts_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS global_ieee1588_ptp_ts_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS global_ieee1588_ptp_glb_int1_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS global_ieee1588_ptp_glb_int1_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS global_ieee1588_ptp_glb_int1_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS global_ieee1588_ptp_glb_int1_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_pkts_en_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ptp_tx_pkts_en_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_asym_comp_data_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS ptp_tx_asym_comp_data_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_asym_comp_sign_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS ptp_tx_asym_comp_sign_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_asym_comp_frac_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS ptp_tx_asym_comp_frac_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_cf_set_threshold_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS ptp_tx_cf_set_threshold_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_port_tx_frm_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_port_tx_ptp_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_port_tx_ssm_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_port_tx_drop_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_port_2step_timestamp_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_oh_alm_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_oh_alm_all_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_ptp_port_buf_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_ptp_txoh_int_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ptp_tx_ptp_txoh_int_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_tx_ptp_txoh_int_mask_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ptp_tx_ptp_txoh_int_mask_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_pkts_en_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS ptp_rx_pkts_en_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_comp_frac_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS ptp_rx_comp_frac_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_comp_ns_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS ptp_rx_comp_ns_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_comp_sign_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter);
extern RET_STATUS ptp_rx_comp_sign_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_pkts_rx_frm_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_pkts_bank_frm_all_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_pkts_bank_frm_err_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_pkts_bank_drop_frm_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_pkts_rx_ptp_port_buf_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_portrx_cap_ts_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_ptp_rxoh_alm_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_ptp_rxoh_alm_all_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_ptp_rxoh_int_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ptp_rx_ptp_rxoh_int_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ptp_rx_ptp_rxoh_int_mask_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ptp_rx_ptp_rxoh_int_mask_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter);
#ifdef __cplusplus
}
#endif

#endif


