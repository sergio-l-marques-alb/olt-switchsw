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
*    jxma    2015-10-22   1.0            initial
*
******************************************************************************/
#ifndef OH_TX_H
#define OH_TX_H

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

extern RET_STATUS oh_tx_mode_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oh_tx_mode_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oh_tx_oh_ins_src_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oh_tx_oh_ins_src_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oh_tx_res_ins_src_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oh_tx_res_ins_src_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oh_tx_oh_ins_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oh_tx_oh_ins_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oh_tx_res_ins_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS oh_tx_res_ins_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS oh_tx_oh_sh_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_32 parameter);
extern RET_STATUS oh_tx_oh_sh_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_32* parameter);
extern RET_STATUS oh_tx_b66code_err_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_32* parameter);
extern RET_STATUS oh_tx_phymap_cca_ccb_ins_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,
						   UINT_8 oh_id,UINT_8 offset,UINT_32* parameter);
extern RET_STATUS oh_tx_phymap_cca_ccb_ins_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,
						   UINT_8 oh_id,UINT_8 offset,UINT_32* parameter);

#ifdef __cplusplus
}
#endif


#endif

