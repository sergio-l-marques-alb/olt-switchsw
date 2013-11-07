/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename llpf.h
*
* @purpose     Link Local Protocol Filtering API's
*
* @component  LLPF
*
* @comments   This file contains all the defines, function prototypes
*             and include files required by llpf.c
*
* @create     10/08/2009
*
* @author     Vijayanand K(kvijayan)
* @end
*
**********************************************************************/

#ifndef LLPF_API_H
#define LLPF_API_H

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
* @notes
*
* @end
*********************************************************************/
L7_RC_t llpfIntfBlockModeSet(L7_uint32 intIfNum,
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
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t llpfIntfBlockModeGet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 *mode);

/*********************************************************************
* @purpose  Checks to see if the interface type is valid for LLPF
*
*
* @param    intIfNum      @b{(input)} interface type
* @param    sysIntfType   @b{(input)} interface type
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL llpfIntfTypeIsValid(L7_uint32 intIfNum, L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Checks to see if the interface is valid for LLPF
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE or L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL llpfIntfIsValid(L7_uint32 intIfNum);
#endif
