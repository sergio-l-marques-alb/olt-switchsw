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
*    dingyi    2017-1-3      1.0           initial
*
******************************************************************************/
#ifndef B66SWITCH_H
#define B66SWITCH_H

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

extern RET_STATUS b66switch_levelclr_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66switch_levelclr_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66switch_maxlevel_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66switch_full_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS b66switch_swram_set(UINT_8 chip_id,UINT_8 channel_id,UINT_8 offset,UINT_8 ram_id,UINT_32* parameter);
extern RET_STATUS b66switch_swram_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 offset,UINT_8 ram_id,UINT_32* parameter);
extern RET_STATUS b66switch_protect_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS b66switch_protect_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS b66switch_chid_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter);
extern RET_STATUS b66switch_chid_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter);
extern RET_STATUS b66switch_protect_info_get(UINT_8 chip_id,UINT_8 size,UINT_32* parameter);

#ifdef __cplusplus
}
#endif

#endif
