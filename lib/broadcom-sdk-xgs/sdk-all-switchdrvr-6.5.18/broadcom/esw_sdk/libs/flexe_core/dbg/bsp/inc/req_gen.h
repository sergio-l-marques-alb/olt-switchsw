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
#ifndef REQ_GEN_H
#define REQ_GEN_H

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

extern RET_STATUS req_gen_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS req_gen_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS req_gen_base_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS req_gen_base_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS req_gen_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS req_gen_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS req_gen_ohif_1588_src_dst_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS req_gen_ohif_1588_src_dst_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS req_gen_ohif_1588_ratio_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS req_gen_ohif_1588_ratio_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS req_gen_ohif_1588_rate_limit_m_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS req_gen_ohif_1588_rate_limit_m_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS req_gen_ohif_1588_rate_limit_base_m_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS req_gen_ohif_1588_rate_limit_base_m_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS req_gen_ohif_1588_rate_limit_ratio_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS req_gen_ohif_1588_rate_limit_ratio_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS req_gen_extra_m_n_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS req_gen_extra_m_n_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS req_gen_plus_set(UINT_8 chip_id,UINT_8 field_id);
extern RET_STATUS req_gen_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS req_gen_ram_set(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter);
extern RET_STATUS req_gen_ram_get(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter);

#ifdef __cplusplus
}
#endif


#endif

