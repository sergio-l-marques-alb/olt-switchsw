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
#ifndef MCMACRX_H
#define MCMACRX_H

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

extern RET_STATUS mcmacrx_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS mcmacrx_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_enable_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mcmacrx_enable_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_statistic_clear_chan_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS mcmacrx_statistic_clear_chan_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_stat_enable_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mcmacrx_stat_enable_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_crc_cutoff_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mcmacrx_crc_cutoff_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_statistic_clear_pls_set(UINT_8 chip_id);
extern RET_STATUS mcmacrx_statistic_all_clr_pls_set(UINT_8 chip_id);
extern RET_STATUS mcmacrx_crc_errdrop_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS mcmacrx_crc_errdrop_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_decode_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS mcmacrx_decode_sel_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_sta_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_arm_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_arm_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_arm_decode_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_error_alm_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_sta_ram_ecc_correct_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_sta_ram_ecc_uncorrect_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_eop_abnor_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS mcmacrx_max_pkt_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS mcmacrx_all_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS mcmacrx_config(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 mode);
extern RET_STATUS mcmacrx_config1(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 mode,UINT_32 rx_max_pkt);
extern RET_STATUS mcmacrx_out_cnt_print(UINT_8 chip_id);

#ifdef __cplusplus
}
#endif


#endif

