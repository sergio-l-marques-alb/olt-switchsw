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
*    dingyi    2017-12-26   1.0            initial
*
******************************************************************************/
#ifndef FLEXE_MULTI_DESKEW_H
#define FLEXE_MULTI_DESKEW_H

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

extern RET_STATUS flexe_multi_deskew_grp_cfg_set(UINT_8 chip_id,UINT_8 group_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_grp_cfg_get(UINT_8 chip_id,UINT_8 group_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_dsw_margin_cfg_set(UINT_8 chip_id,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_dsw_margin_cfg_get(UINT_8 chip_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_ssf_cfg_set(UINT_8 chip_id,UINT_8 group_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_ssf_cfg_get(UINT_8 chip_id,UINT_8 group_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_phy_ssf_en_set(UINT_8 chip_id,UINT_8 group_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_phy_ssf_en_get(UINT_8 chip_id,UINT_8 group_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_grp_ini_ctl_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 group_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_grp_ini_ctl_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 group_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_fifo_addr_offset_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_fifo_addr_offset_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_grp_mode_set(UINT_8 chip_id,UINT_8 group_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_grp_mode_get(UINT_8 chip_id,UINT_8 group_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_bypass_50g_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_bypass_50g_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_bypass_100g_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_bypass_100g_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_grp_dsw_en_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_grp_dsw_en_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_rate_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS flexe_multi_deskew_rate_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_grp_ssf_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_dsw_measure_get(UINT_8 chip_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS flexe_multi_deskew_dsw_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif

#endif

