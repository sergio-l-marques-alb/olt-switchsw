/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename     src/l7public/porting/usmdb_llpf_api.h
*
* @purpose      Provide User Interface API's for LLPF Configuration
*
* @component    Usmdb
*
* @comments     None
*
* @create       24/9/2009
*
* @author       vijayanand K
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#ifndef USMDB_LLPF_API_H
#define USMDB_LLPF_API_H

#include <string.h>
#include "l7_common.h"
#include "llpf_exports.h"

/*********************************************************************
* @purpose  Set LLPF Protocol Filter Blocking Mode.
*
* @param    intfNum       @{{input}}The Current Interface Number
* @param    protocolType  @{{input}}  Type of Protocol
* @param    mode          @{{input}}  Enable/Disable LLPF
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbLlpfIntfBlockModeSet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType, L7_BOOL mode);
/*********************************************************************
* @purpose  Get LLPF Protocol Filter Blocking Mode.
*
* @param    intfNum       @{{input}}The Current Interface Number
* @param    protocolType  @{{input}}  Type of Protocol
* @param    *mode         @{{output}} Enable/Disable LLPF
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbLlpfIntfBlockModeGet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 *mode);

/*********************************************************************
* @purpose  Checks to see if the interface is valid for LLPF
*          
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL usmDbLlpfIntfIsValid(L7_uint32 intIfNum);
#endif /* end of USMDB_LLPF_API_H */
