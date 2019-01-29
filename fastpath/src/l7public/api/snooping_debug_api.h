/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    snooping_debug_api.h
* @purpose     snoop debug functions
* @component   SNOOPING
* @comments    none
* @create      08/22/2006
* @author      ssuvarna
* @end
*             
**********************************************************************/
#ifndef SNOOPING_DEBUG_API_H
#define SNOOPING_DEBUG_API_H

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying snoop packet debug info
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

L7_RC_t snoopDebugPacketTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag,
                                     L7_uchar8 family);

/*********************************************************************
* @purpose  Turns on/off the displaying of snoop packet debug info
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
L7_RC_t snoopDebugPacketTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag,
                                     L7_uchar8 family);
#endif /* SNOOPING_DEBUG_API_H */
