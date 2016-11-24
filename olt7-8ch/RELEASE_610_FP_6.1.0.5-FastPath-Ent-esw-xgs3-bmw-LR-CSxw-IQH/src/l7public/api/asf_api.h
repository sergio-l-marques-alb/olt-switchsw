/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2001-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename  asf_api.h
*
* @purpose   Provide interface to Cut-Though (ASF) module
* @component asf
*
* @comments
*
* @create    17-Jan-2007
*
* @author    NarasimhaRaju
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/
#ifndef ASF_API_H
#define ASF_API_H

#include "l7_common.h"

/*********************************************************************
* @purpose  Set (Enable/Disable) the ASF mode.
*          
* @param    mode        @b{(input)} mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t asfModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get (Enable/Disable) the ASF mode.
*          
* @param    currMode    @b{(input)} Current mode L7_ENABLE/L7_DISABLE
* @param    configMode  @b{(input)} Configured mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t asfModeGet(L7_uint32 *currMode, L7_uint32 *configMode);

#endif
