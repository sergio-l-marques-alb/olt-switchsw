
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename mcast_debug_api.h
*
* @purpose mcast APIs
*
* @component mcast
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


#ifndef MCAST_DEBUG_API_H
#define MCAST_DEBUG_API_H

#include "l7_common.h"
#include "statsapi.h"
#include "comm_structs.h"

typedef enum
{
  MCAST_DEBUG_PACKET_RX_TRACE = 0,
  MCAST_DEBUG_PACKET_TX_TRACE,
  MCAST_DEBUG_EVENT_TRACE,
  MCAST_DEBUG_TIMER_TRACE,
  MCAST_DEBUG_LAST_TRACE
}MCAST_DEBUG_TRACE_FLAG_t;

/*********************************************************************
* @purpose  Get the current status of displaying mcast packet debug info
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
L7_RC_t mcastDebugTraceFlagGet(L7_uchar8 family,
                               MCAST_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL *flag);
/*********************************************************************
* @purpose  Turns on/off the displaying of mcast packet debug info
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
L7_RC_t mcastDebugTraceFlagSet(L7_uchar8 family, 
                               MCAST_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag);
#endif
