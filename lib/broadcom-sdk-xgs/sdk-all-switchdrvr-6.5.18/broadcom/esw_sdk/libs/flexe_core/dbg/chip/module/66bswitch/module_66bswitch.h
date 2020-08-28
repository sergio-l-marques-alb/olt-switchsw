
#ifndef MODULE_66BSWITCH_H
#define MODULE_66BSWITCH_H


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

extern RET_STATUS b66switch_init(UINT_8 chip_id);
extern RET_STATUS b66switch_dump(UINT_8 chip_id);
extern RET_STATUS b66switch_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 ram_position,UINT_8 src_ch,UINT_8 dest_ch);
extern RET_STATUS b66switch_debug(void);


#ifdef __cplusplus
}
#endif
#endif
