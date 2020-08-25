/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    dot3ad_debug_api.h
* @purpose     802.3ad link aggregation, LAC debug functions
* @component   dot3ad
* @comments    none
* @create      08/02/2006
* @author      cpverne
* @end
*             
**********************************************************************/
#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying dot3ad packet debug info
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
L7_BOOL dot3adDebugPacketTraceFlagGet();


/*********************************************************************
* @purpose  Turns on/off the displaying of dot3ad packet debug info
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
L7_RC_t dot3adDebugPacketTraceFlagSet(L7_BOOL flag);

