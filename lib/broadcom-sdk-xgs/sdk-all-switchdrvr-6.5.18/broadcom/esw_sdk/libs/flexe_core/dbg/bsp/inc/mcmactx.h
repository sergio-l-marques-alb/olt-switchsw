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
#ifndef MCMACTX_H
#define MCMACTX_H

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

extern RET_STATUS mcmactx_work_mode_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mcmactx_work_mode_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_statistic_clear_chan_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS mcmactx_statistic_clear_chan_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmactx_encode_config_set(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS mcmactx_encode_config_get(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mcmactx_stat_enable_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mcmactx_stat_enable_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_avg_ipg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS mcmactx_avg_ipg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmactx_minimum_mode_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS mcmactx_minimum_mode_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmactx_crc_inc_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mcmactx_crc_inc_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_crc_recal_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mcmactx_crc_recal_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_decode_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS mcmactx_decode_sel_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmactx_all_pls_set(UINT_8 chip_id, UINT_8 pls_sel);
extern RET_STATUS mcmactx_ts_init_done_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmactx_sta_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmactx_env_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmactx_bussw_underflow_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_sar_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_sar_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_encode_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_decode_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmactx_sta_ram_ecc_correct_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mcmactx_sta_ram_ecc_uncorrect_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mcmactx_ts_config_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS mcmactx_ts_config_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS mcmactx_decode_err_code_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS mcmactx_encode_err_code_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS mcmactx_max_pkt_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS mcmactx_all_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS mcmactx_config_init(UINT_8 chip_id);
extern RET_STATUS mcmactx_config(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_16 ts_num,UINT_32 mode);
extern RET_STATUS mcmactx_config1(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,
                      UINT_16 ts_num,UINT_32 mode,UINT_32 tx_max_pkt);

#ifdef __cplusplus
}
#endif


#endif




