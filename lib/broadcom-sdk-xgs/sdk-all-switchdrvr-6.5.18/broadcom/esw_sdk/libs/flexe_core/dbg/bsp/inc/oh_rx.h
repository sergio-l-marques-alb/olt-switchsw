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
#ifndef OH_RX_H
#define OH_RX_H

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
extern RET_STATUS oh_rx_flexe_oh_loopback_cfg_set(UINT_8 chip_id ,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oh_rx_flexe_oh_loopback_cfg_get(UINT_8 chip_id ,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oh_rx_flexe_rx_mode_set(UINT_8 chip_id ,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oh_rx_flexe_rx_mode_get(UINT_8 chip_id ,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oh_rx_crc_rx_mode_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_crc_rx_mode_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_alm_en_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_32 parameter);
extern RET_STATUS oh_rx_alm_en_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type,UINT_32* parameter);
extern RET_STATUS oh_rx_exccc_exsc_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32 parameter);
extern RET_STATUS oh_rx_exccc_exsc_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter);
extern RET_STATUS oh_rx_exgid_expid_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oh_rx_exgid_expid_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oh_rx_fp_pattern_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_fp_pattern_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_lof_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_lof_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_lof_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_lof_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_oof_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_oof_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_oof_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_oof_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_lom_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_lom_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_lom_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_lom_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_oom_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_oom_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_oom_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_oom_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_lop_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_lop_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_lop_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_lop_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_oop_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_oop_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_oop_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter);
extern RET_STATUS oh_rx_oop_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_pad_alm_en_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_32 parameter);
extern RET_STATUS oh_rx_pad_alm_en_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type,UINT_32* parameter);
extern RET_STATUS oh_rx_cc_rx_value_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter);
extern RET_STATUS oh_rx_id_rx_value_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter);
extern RET_STATUS oh_rx_acmc_setion_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 field_id , UINT_32* parameter);
extern RET_STATUS oh_rx_ac_shim2shim_get(UINT_8 chip_id , UINT_8 phy_id,UINT_8 field_id ,UINT_32* parameter);
extern RET_STATUS oh_rx_ac_reserved1_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 field_id, UINT_32* parameter);
extern RET_STATUS oh_rx_ac_reserved2_get(UINT_8 chip_id , UINT_8 phy_id,UINT_32* parameter);
extern RET_STATUS oh_rx_alm_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter);
extern RET_STATUS oh_rx_ccm_rx_alm_get(UINT_8 chip_id ,UINT_8 phy_id, UINT_32* parameter);
extern RET_STATUS oh_rx_oh_alm_int_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32 parameter);
extern RET_STATUS oh_rx_oh_alm_int_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter);
extern RET_STATUS oh_rx_oh_alm_int_mask_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32 parameter);
extern RET_STATUS oh_rx_oh_alm_int_mask_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter);
extern RET_STATUS oh_rx_phymap_rx_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS oh_rx_phymap_rx_exp_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS oh_rx_phymap_rx_exp_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS oh_rx_calendar_rx_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS oh_rx_ccab_rx_exp_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS oh_rx_ccab_rx_exp_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_8 offset ,UINT_32* parameter);
extern RET_STATUS oh_rx_alm_debug(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type);
#ifdef __cplusplus
}
#endif


#endif

