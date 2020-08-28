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
#ifndef OHOAM_EF_H
#define OHOAM_EF_H

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

extern RET_STATUS ohoam_ef_soh_ins_getpkt1_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter);
extern RET_STATUS ohoam_ef_soh_ins_getpkt1_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_loopback_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter);
extern RET_STATUS ohoam_ef_soam_loopback_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_alm_colection_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter);
extern RET_STATUS ohoam_ef_soam_alm_colection_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_asic_ohif_dasa_set(UINT_8 chip_id ,UINT_8 type ,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ohoam_ef_asic_ohif_dasa_get(UINT_8 chip_id ,UINT_8 type ,UINT_8 field_id, UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_ext_dscf_lvl_stat_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soh_ins_alarm_bit_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soh_ins_alarm_get(UINT_8 chip_id ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soh_ext_cffull_alarm_bit_get(UINT_8 chip_id ,UINT_8 type,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soh_ext_cffull_alarm_get(UINT_8 chip_id ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_ext_flexe_alam_bit_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_ext_flexe_alam_get(UINT_8 chip_id ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_ext_mac_alam_bit_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_ext_mac_alam_get(UINT_8 chip_id ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_flexe_type_tbl_set(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_flexe_type_tbl_get(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_mac_type_tbl_set(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_mac_type_tbl_get(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_ext_gbfifo_lvl_set(UINT_8 chip_id ,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ohoam_ef_ext_gbfifo_lvl_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ohoam_ef_ext_col_gen_m_set(UINT_8 chip_id ,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ohoam_ef_ext_col_gen_m_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ohoam_ef_b66_encode_decode_cnt_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ohoam_ef_pkt_cnt_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ohoam_ef_ins_crc_err_cnt_get(UINT_8 chip_id ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_ext_pkt_cnt_get(UINT_8 chip_id ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_ohif_asic_alm_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ohoam_ef_snap_ext_ram_get(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_snap_ext_ram_print(UINT_8 chip_id);
extern RET_STATUS ohoam_ef_snap_ins_ram_get(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_snap_ins_ram_print(UINT_8 chip_id);
extern RET_STATUS ohoam_ef_test_cfg_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter);
extern RET_STATUS ohoam_ef_test_cfg_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_test_oh_ext_err_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter);
extern RET_STATUS ohoam_ef_test_oh_ext_err_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_test_alm_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_test_all_ram_set(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS ohoam_ef_test_all_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS ohoam_ef_asic_test_flexe_oam_cmp_alm_get(UINT_8 chip_id ,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS ohoam_ef_asic_test_mac_oam_cmp_alm_get(UINT_8 chip_id ,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS ohoam_ef_asic_test_mac_ctrl_insert_oam_set(UINT_8 chip_id ,UINT_8 type );
extern RET_STATUS ohoam_ef_asic_test_1dm_test_get(UINT_8 chip_id ,UINT_8 type,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS ohoam_ef_soam_init(UINT_8 chip_id);
extern RET_STATUS ohoam_ef_oam_flexe_set_debug(UINT_8 start_chid,UINT_8 end_chid,UINT_8 loop_num);
extern RET_STATUS ohoam_ef_soh_inst_mode_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter);
extern RET_STATUS ohoam_ef_soh_inst_mode_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter);
#ifdef __cplusplus
}
#endif


#endif


