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
*    dengjie    2015-10-22   1.0            initial
*
******************************************************************************/
#ifndef OAM_TX_H
#define OAM_TX_H

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

extern RET_STATUS oam_tx_oam_joit_bip_cfg_set(UINT_8 chip_id,UINT_8 type,UINT_32 parameter);
extern RET_STATUS oam_tx_oam_joit_bip_cfg_get(UINT_8 chip_id,UINT_8 type,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_brk_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_brk_cfg_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_lpi_insert_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_lpi_insert_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_lf_insert_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_lf_insert_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_rf_insert_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_rf_insert_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_brk_ch_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_brk_ch_cfg_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_ts_1dm_insert_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_ts_1dm_insert_en_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_ts_2dmm_insert_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_ts_2dmm_insert_en_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_ts_2dmr_insert_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_ts_2dmr_insert_en_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_ts_dm_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_ts_dm_sel_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_fault_rpl_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_fault_rpl_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_idty_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_idty_cfg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_period_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_period_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_seq_type_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_seq_type_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_seq_num_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_seq_num_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_crc_rev_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oam_tx_crc_rev_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oam_tx_idle_rpl_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_idle_rpl_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bypass_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bypass_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bip_err_mask_bit_set(UINT_8 chip_id,UINT_8 bit_sel,UINT_32 parameter);
extern RET_STATUS oam_tx_bip_err_mask_bit_get(UINT_8 chip_id,UINT_8 bit_sel,UINT_32* parameter);
extern RET_STATUS oam_tx_bip_err_mask_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bip_err_mask_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_manual_insert_csf_lpi_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_manual_insert_csf_lpi_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_manual_insert_csf_lf_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_manual_insert_csf_lf_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_manual_insert_csf_rf_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_manual_insert_csf_rf_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bip_err_level_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bip_err_level_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bypass_client_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bypass_client_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_manual_insert_rdi_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_manual_insert_rdi_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_period_cfg1_set(UINT_8 chip_id,UINT_8 period_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_period_cfg1_get(UINT_8 chip_id,UINT_8 period_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_en_cfg_asic_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_en_cfg_asic_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_oam_pri_encode_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_oam_pri_encode_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_source_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_source_sel_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_new_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oam_tx_new_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_fifo_num_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_fifo_num_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS oam_tx_lpi_setclr_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oam_tx_lpi_setclr_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oam_tx_cnt_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oam_tx_cnt_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oam_tx_lpi_idle_rpl_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_lpi_idle_rpl_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_insert_66bcode_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_insert_66bcode_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bip_err_pulse_set(UINT_8 chip_id,UINT_8 ch_id);
extern RET_STATUS oam_tx_bas_insert_cnt_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_oth_insert_cnt_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_lpi_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_fifo_full_rei_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_fifo_full_aps_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_fifo_full_low_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_fifo_full_bas_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_fifo_full_new_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_fifo_empty_bas_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_data_err_pulse_set(UINT_8 chip_id,UINT_8 ch_id);
extern RET_STATUS oam_tx_bas_brk_cnt_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_bas_cnt_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_aps_cnt_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_low_cnt_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_new_cnt_get(UINT_8 chip_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_timestamp_lock_get(UINT_8 chip_id,UINT_8 field_id,UINT_32 *parameter);
extern RET_STATUS oam_tx_rescfg_set(UINT_8 chip_id,UINT_8 offset,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oam_tx_rescfg_get(UINT_8 chip_id,UINT_8 offset,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_lpi_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_csf_lpi_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_lpi_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_csf_lpi_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_lf_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_csf_lf_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_lf_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_csf_lf_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_rf_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_csf_rf_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_csf_rf_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_csf_rf_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_insert_66bcode_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_insert_66bcode_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_lpi_bas_ins_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_lpi_bas_ins_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS oam_tx_bas_crc_err_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_crc_err_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter); 
extern RET_STATUS oam_tx_bas_crc_err_mask_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS oam_tx_bas_crc_err_mask_get(UINT_8 chip_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif


#endif

