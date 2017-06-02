/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    l7_rip_debug_api.h
* @purpose     rip debug functions
* @component   RIPMAP
* @comments    none
* @create      08/22/2006
* @author      ssuvarna
* @end
*             
**********************************************************************/

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying rip packet debug info
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

L7_BOOL ripMapDebugPacketTraceFlagGet();

/*********************************************************************
* @purpose  Turns on/off the displaying of rip packet debug info
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

L7_RC_t ripMapDebugPacketTraceFlagSet(L7_BOOL flag);

