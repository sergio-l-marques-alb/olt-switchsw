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
#ifndef FLEXE_ENV_H
#define FLEXE_ENV_H

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

extern RET_STATUS flexe_env_cfg1_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS flexe_env_cfg1_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS flexe_env_cfg2_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS flexe_env_cfg2_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS flexe_env_cfg3_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS flexe_env_cfg3_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS flexe_env_m_cfg_set(UINT_8 chip_id,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS flexe_env_m_cfg_get(UINT_8 chip_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS flexe_env_base_m_cfg_set(UINT_8 chip_id,UINT_8 grp_id,UINT_32 parameter);
extern RET_STATUS flexe_env_base_m_cfg_get(UINT_8 chip_id,UINT_8 grp_id,UINT_32* parameter);
extern RET_STATUS flexe_env_adj_in_alm_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS flexe_env_integral_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);

#ifdef __cplusplus
}
#endif


#endif

