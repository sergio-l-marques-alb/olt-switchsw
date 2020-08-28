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
*    dengj    2015-10-22   1.0            initial
*
******************************************************************************/
#ifndef FLEXE_MUX_H
#define FLEXE_MUX_H

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

extern RET_STATUS flexe_mux_mode_set(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32 parameter);
extern RET_STATUS flexe_mux_mode_get(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32* parameter);
extern RET_STATUS flexe_mux_env_mode_set(UINT_8 chip_id, UINT_32 parameter);
extern RET_STATUS flexe_mux_env_mode_get(UINT_8 chip_id, UINT_32* parameter);
extern RET_STATUS flexe_mux_group_cfg_set(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32 parameter);
extern RET_STATUS flexe_mux_group_cfg_get(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32* parameter);
extern RET_STATUS flexe_mux_sch_cfg_set(UINT_8 chip_id,UINT_8 sch_opp,UINT_32 parameter);
extern RET_STATUS flexe_mux_sch_cfg_get(UINT_8 chip_id,UINT_8 sch_opp,UINT_32* parameter);
extern RET_STATUS flexe_mux_msi_cfg_set(UINT_8 chip_id,UINT_8 ts_sel,UINT_32 parameter);
extern RET_STATUS flexe_mux_msi_cfg_get(UINT_8 chip_id,UINT_8 ts_sel,UINT_32* parameter);
extern RET_STATUS flexe_mux_tsnum_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_tsnum_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_cycle_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_cycle_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_block_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_block_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_ch_belong_flexe_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_ch_belong_flexe_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_flexe_enable_set(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32 parameter);
extern RET_STATUS flexe_mux_flexe_enable_get(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32* parameter);
extern RET_STATUS flexe_mux_pcs_enable_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_pcs_enable_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_pcs_num_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_pcs_num_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_ts_enable_set(UINT_8 chip_id,UINT_8 ts_sel,UINT_32 parameter);
extern RET_STATUS flexe_mux_ts_enable_get(UINT_8 chip_id,UINT_8 ts_sel,UINT_32* parameter);
extern RET_STATUS flexe_mux_pcs_sch_cfg_set(UINT_8 chip_id,UINT_8 sch_opp,UINT_32 parameter);
extern RET_STATUS flexe_mux_pcs_sch_cfg_get(UINT_8 chip_id,UINT_8 sch_opp,UINT_32* parameter);
extern RET_STATUS flexe_mux_pcs_chid_cfg_set(UINT_8 chip_id,UINT_8 pcs_sel,UINT_32 parameter);
extern RET_STATUS flexe_mux_pcs_chid_cfg_get(UINT_8 chip_id,UINT_8 pcs_sel,UINT_32* parameter);
extern RET_STATUS flexe_mux_reqcnt_slvl_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_reqcnt_slvl_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_c_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_c_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_msi_cfg_b_set(UINT_8 chip_id,UINT_8 ts_sel,UINT_32 parameter);
extern RET_STATUS flexe_mux_msi_cfg_b_get(UINT_8 chip_id,UINT_8 ts_sel,UINT_32* parameter);
extern RET_STATUS flexe_mux_tsnum_cfg_b_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_tsnum_cfg_b_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_cycle_cfg_b_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_cycle_cfg_b_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_block_cfg_b_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_block_cfg_b_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_ts_enable_b_set(UINT_8 chip_id,UINT_8 ts_sel,UINT_32 parameter);
extern RET_STATUS flexe_mux_ts_enable_b_get(UINT_8 chip_id,UINT_8 ts_sel,UINT_32* parameter);
extern RET_STATUS flexe_mux_cfg_pls_set(UINT_8 chip_id);
extern RET_STATUS flexe_mux_ram_conflict_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_sys_signal_fifo_alm_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_wr_cfg_ram_set(UINT_8 chip_id,UINT_16 index,UINT_32* parameter);
extern RET_STATUS flexe_mux_wr_cfg_ram_get(UINT_8 chip_id,UINT_16 index,UINT_32* parameter);
extern RET_STATUS flexe_mux_rd_cfg_ram_set(UINT_8 chip_id,UINT_8 line_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_mux_rd_cfg_ram_get(UINT_8 chip_id,UINT_8 line_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_mux_wr_cfg_b_ram_set(UINT_8 chip_id,UINT_16 index,UINT_32* parameter);
extern RET_STATUS flexe_mux_wr_cfg_b_ram_get(UINT_8 chip_id,UINT_16 index,UINT_32* parameter);
extern RET_STATUS flexe_mux_rd_cfg_b_ram_set(UINT_8 chip_id,UINT_8 line_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_mux_rd_cfg_b_ram_get(UINT_8 chip_id,UINT_8 line_sel,UINT_8 index,UINT_32* parameter);
extern RET_STATUS flexe_mux_cphy2_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_cphy2_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_cnt8_sch_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_cnt8_sch_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_m_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_m_cfg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_base_m_cfg_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_base_m_cfg_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_en_sel_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_en_sel_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS flexe_mux_inst_sel_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS flexe_mux_inst_sel_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif


#endif



