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
#ifndef FLEXE_MACRX_H
#define FLEXE_MACRX_H

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

extern RET_STATUS flexe_macrx_statistic_clear_chan_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_macrx_statistic_clear_chan_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_stat_enable_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_macrx_stat_enable_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_decode_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_macrx_decode_sel_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_statistic_clear_pls_set(UINT_8 chip_id);
extern RET_STATUS flexe_macrx_statistic_all_clr_pls_set(UINT_8 chip_id);
extern RET_STATUS flexe_macrx_sta_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_arm_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_arm_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_arm_decode_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_error_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_sta_ram_ecc_correct_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_sta_ram_ecc_uncorrect_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_eop_abnor_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_macrx_max_pkt_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_macrx_max_pkt_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_macrx_decode_err_code_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_macrx_all_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_macrx_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode);
#ifdef __cplusplus
}
#endif


#endif

