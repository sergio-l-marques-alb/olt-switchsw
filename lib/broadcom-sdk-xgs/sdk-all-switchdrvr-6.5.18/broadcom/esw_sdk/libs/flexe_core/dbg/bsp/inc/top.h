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
#ifndef TOP_H
#define TOP_H

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
extern RET_STATUS top_rst_glb_fuci_soft_n_reset(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS top_rst_glb_logic_n_reset(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS top_rst_logic_sys1_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_fpga_serdes_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_logic_sys3_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_125m_pps_n_reset(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS top_rst_fuci_module1_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_fuci_module2_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_pps_time_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS top_rst_pps_time_cfg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_num_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_type_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_len_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_tod_byte0_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_tod_sec_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS top_glb_cnt_clr_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS top_glb_alm_clr_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS top_glb_int_clr_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS top_pps_tod_rx_tod_nsec_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_tod_res_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_crc_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_real_time_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_cnt_tod_lost_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_cnt_pps_adjust_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_cnt_crc_eor_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_cnt_wave_broken_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_pps_tod_rx_reg_cnt_25m_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_fpga_info_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_fpga_function_info_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_rst_soh_ext_125m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_soh_ins_125m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_soam_ext_125m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_soam_ins_125m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_gearbox_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_psy_sel_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS top_psy_sel_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS top_rst_311m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_311m_upi_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_soh_upi_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_clk_measure_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_clk_measure_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS top_measure_cnt_max_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS top_measure_cnt_min_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS top_rst_sys_logic4_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_cdr_sel_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS top_cdr_sel_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS top_rst_ieee1588_1_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_rst_ieee1588_2_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_int1_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_int1_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS top_int1_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_int1_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS top_int2_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_int2_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS top_int2_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS top_int2_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
#ifdef __cplusplus
}
#endif

#endif

