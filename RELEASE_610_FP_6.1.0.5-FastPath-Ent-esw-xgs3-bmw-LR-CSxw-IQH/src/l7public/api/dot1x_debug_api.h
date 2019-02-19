/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    dot1x_debug_api.h
* @purpose     dot1x debug functions
* @component   DOT1X
* @comments    none
* @create      
* @author      
* @end
*             
**********************************************************************/

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying dot1x packet debug info
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

void dot1xDebugPacketTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag);

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
L7_RC_t dot1xDebugPacketTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag);
