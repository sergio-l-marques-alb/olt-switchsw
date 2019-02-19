/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2008
*
**********************************************************************
*
* @filename    dot3ah_debug_api.h
* @purpose     dot3ah debug functions
* @component   DOT3AH
* @comments    none
* @create      
* @author      
* @end
*             
**********************************************************************/

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying dot3ah packet debug info
*            
* @param    none
*                      
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/

void dot3ahDebugPacketTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag);

/*********************************************************************
* @purpose  Turns on/off the displaying of dot1x packet debug info
*            
* @param    flag         new value of the Packet Debug flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
 *********************************************************************/
L7_RC_t dot3ahDebugPacketTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag);
