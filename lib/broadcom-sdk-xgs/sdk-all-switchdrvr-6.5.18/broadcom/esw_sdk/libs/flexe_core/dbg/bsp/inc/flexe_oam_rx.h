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
#ifndef FLEXE_OAM_RX_H
#define FLEXE_OAM_RX_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "datatype.h"
#include "hal.h"

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/



/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/
extern CH_ALM flexe_bas_csf_lpi[DEV_NUM];
extern CH_ALM flexe_bas_cs_lf[DEV_NUM];
extern CH_ALM flexe_bas_cs_rf[DEV_NUM];
extern CH_ALM flexe_bas_no_receive_alm[DEV_NUM];
extern CH_ALM flexe_rx_sdbip[DEV_NUM];
extern CH_ALM flexe_bas_crc_err[DEV_NUM];
extern CH_ALM flexe_bas_rdi[DEV_NUM];
extern CH_ALM flexe_bas_period_alm[DEV_NUM];
extern CH_ALM flexe_rdi_lf_alm[DEV_NUM];
extern CH_ALM flexe_rf_alm[DEV_NUM];
extern CH_ALM flexe_rx_sfbip[DEV_NUM];
extern CH_ALM flexe_rx_sfbei[DEV_NUM];
extern CH_ALM flexe_rx_lpi[DEV_NUM];
extern CH_ALM flexe_rx_sdbei[DEV_NUM];


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
extern void flexe_oam_rx_ch_alm_init(UINT_8 chip_id);
extern RET_STATUS flexe_oam_rx_glb_biperr_bit_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_glb_biperr_bit_cfg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_glb_beierr_bit_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_glb_beierr_bit_cfg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_crc_invert_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_crc_invert_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_idle_rpl_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_idle_rpl_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_oam_bip_cla_en_set(UINT_8 chip_id,UINT_8 type,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_oam_bip_cla_en_get(UINT_8 chip_id,UINT_8 type,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_cfg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rdi_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_rdi_sel_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_oam_idfy_37_34_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_oam_idfy_37_34_cfg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_pos_cnt_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_pos_cnt_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bypass_chnl_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bypass_chnl_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_period_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_period_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_period_sel_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_period_sel_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_oam_cnt_clr_num_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_oam_cnt_clr_num_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_period_ab_set(UINT_8 chip_id,UINT_8 field_id, UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_period_ab_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bypass_oam_alm_ch_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bypass_oam_alm_ch_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_config_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_config_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_lf_manual_insert_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_lf_manual_insert_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rf_manual_insert_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_rf_manual_insert_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_lpi_manual_insert_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_lpi_manual_insert_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bypass_oam_alm_all_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bypass_oam_alm_all_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_los_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_los_sel_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_base_oam_cnt_clr_num_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_base_oam_cnt_clr_num_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_biperr_cnt_clr_pls_set(UINT_8 chip_id,UINT_8 ch_id);
extern RET_STATUS flexe_oam_rx_beierr_cnt_clr_pls_set(UINT_8 chip_id,UINT_8 ch_id);
extern RET_STATUS flexe_oam_rx_clr_ram_init_pls_set(UINT_8 chip_id,UINT_8 field_id);
extern RET_STATUS flexe_oam_rx_oam_cnt_clr_done_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_base_oam_cnt_clr_done_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_fifo_cnt_max_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_period_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdbip_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfbip_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfbei_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_crc_alm_out_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rdi_alm_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_rdi_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_csf_lpi_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_demux_client_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_crc_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_no_receive_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rdi_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_fifo_full_alm_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdbei_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_ecc_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_lpi_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_period_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_biperr_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_beierr_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdcfg_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdcfg_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sd_bip8_clr_thresh_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sd_bip8_clr_thresh_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sd_bip8_set_thresh_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sd_bip8_set_thresh_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sd_block_num_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sd_block_num_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sd_bei_clr_thresh_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sd_bei_clr_thresh_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sd_bei_set_thresh_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sd_bei_set_thresh_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sd_bei_block_num_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sd_bei_block_num_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sf_bip8_clr_thresh_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sf_bip8_clr_thresh_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sf_bip8_set_thresh_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sf_bip8_set_thresh_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sf_bip8_block_num_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sf_bip8_block_num_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sf_bei_clr_thresh_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sf_bei_clr_thresh_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sf_bei_set_thresh_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sf_bei_set_thresh_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sf_bei_block_num_set(UINT_8 chip_id,UINT_8 index,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sf_bei_block_num_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_chid_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_chid_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfcfg_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfcfg_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_oam_cnt_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdbeicfg_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdbeicfg_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfbeicfg_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfbeicfg_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_reixc_cfg_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_reixc_cfg_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_base_oam_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_base_oam_cnt_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_alm_en_set(UINT_8 chip_id,UINT_8 alm_type,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_alm_en_get(UINT_8 chip_id,UINT_8 alm_type,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_ram_init(UINT_8 chip_id);
extern RET_STATUS flexe_oam_rx_ts_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_ts_add(UINT_8 chip_id, int tunnel_id, int rate);
extern RET_STATUS flexe_oam_rx_ts_del(UINT_8 chip_id, int tunnel_id);
extern RET_STATUS flexe_oam_rx_bypass_xgmii_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bypass_xgmii_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_lpi_rpl_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_lpi_rpl_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_crc_en_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_crc_en_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdbip_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sdbip_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdbip_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sdbip_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_rdi_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_rdi_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_rdi_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_rdi_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_csf_lpi_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_csf_lpi_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_csf_lpi_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_csf_lpi_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_lf_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_lf_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_lf_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_lf_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_rf_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_rf_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_rf_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_cs_rf_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_crc_err_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_crc_err_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_crc_err_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_crc_err_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_no_receive_alm_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_no_receive_alm_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_no_receive_alm_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_no_receive_alm_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rdi_lf_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_rdi_lf_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rdi_lf_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_rdi_lf_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdbei_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sdbei_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sdbei_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sdbei_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rf_alm_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_rf_alm_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_rf_alm_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_rf_alm_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_lpi_alm_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_lpi_alm_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_lpi_alm_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_lpi_alm_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_period_alm_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_period_alm_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_bas_period_alm_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_bas_period_alm_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfbei_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sfbei_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfbei_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sfbei_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfbip_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sfbip_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_sfbip_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_oam_rx_sfbip_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_oam_rx_ts_sw_add(UINT_8 chip_id, int tunnel_id, int rate);
extern void flexe_oam_rx_ts_register_add(UINT_8 chip_id);
extern void flexe_oam_ts_debug(UINT_8 chip_id);
#ifdef __cplusplus
}
#endif


#endif

