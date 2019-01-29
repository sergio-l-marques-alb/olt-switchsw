/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    dot3ad_debug_api.h
* @purpose     802.1s Spanning tree debug functions
* @component   dot1s
* @comments    none
* @create      08/20/2006
* @author      ssuvarna
* @end
*             
**********************************************************************/

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying dot1s packet debug info
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
void dot1sDebugPacketTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag);

/*********************************************************************
* @purpose  Turns on/off the displaying of dot1s packet debug info
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
L7_RC_t dot1sDebugPacketTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag);
