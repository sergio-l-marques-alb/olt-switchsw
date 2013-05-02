/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename     src/usmdb/base/llpf/usmdb_llpf.c
*
* @purpose      Provide User Interface API's for LLPF Configuration
*
* @component    Usmdb
*
* @comments 
*
* @create       24/9/2009
*
* @author       vijayanand K
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "llpf_exports.h"
#include "llpf_api.h"
#include "usmdb_llpf_api.h"

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
                 L7_LLPF_BLOCK_TYPE_t protocolType, L7_BOOL mode)
{
    return llpfIntfBlockModeSet(intIfNum,protocolType,mode);
}

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
                 L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 *mode)
{
    return llpfIntfBlockModeGet(intIfNum,protocolType,mode);
}

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
L7_BOOL usmDbLlpfIntfIsValid(L7_uint32 intIfNum)
{
   return llpfIntfIsValid(intIfNum);
}
