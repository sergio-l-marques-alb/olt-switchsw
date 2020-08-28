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
#ifndef CHANNELIZE_H
#define CHANNELIZE_H

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
extern RET_STATUS channelize_levelclr_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS channelize_levelclr_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS channelize_bypass_en_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS channelize_bypass_en_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS channelize_b66replace_en_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS channelize_b66replace_en_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS channelize_b66replace_pattern_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS channelize_b66replace_pattern_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS channelize_pcschid_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter);
extern RET_STATUS channelize_pcschid_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS channelize_maxlevel_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);
extern RET_STATUS channelize_full_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif

#endif

