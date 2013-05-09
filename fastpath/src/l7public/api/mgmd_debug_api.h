
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename mgmd_debug_api.h
*
* @purpose mgmd APIs
*
* @component mgmd
*
* @comments none
*
* @create 
*
* @author 
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef MGMD_DEBUG_API_H
#define MGMD_DEBUG_API_H

#include "l7_common.h"
#include "statsapi.h"
#include "comm_structs.h"

typedef enum
{
  MGMD_DEBUG_PACKET_RX_TRACE = 0,
  MGMD_DEBUG_PACKET_TX_TRACE,
  MGMD_DEBUG_EVENT_TRACE,
  MGMD_DEBUG_TIMER_TRACE,
  MGMD_DEBUG_LAST_TRACE
}MGMD_DEBUG_TRACE_FLAG_t;

/*********************************************************************
* @purpose  Get the current status of displaying mgmd packet debug info
*            
* @param    familyType   @b{(input)} Address Family type
* @param    traceFlag    @b{(input)} type of trace flag
* @param    flag         @b{(output)} trace flag value
*                      
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mgmdDebugTraceFlagGet(L7_uchar8 family,
                              MGMD_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL *flag);
/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*            
* @param    familyType   @b{(input)} Address Family type
* @param    traceFlag    @b{(input)} type of trace flag
* @param    flag         @b{(input)} trace flag value
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t mgmdDebugTraceFlagSet(L7_uchar8 family, 
                              MGMD_DEBUG_TRACE_FLAG_t traceFlag,
                              L7_BOOL flag);

#endif


