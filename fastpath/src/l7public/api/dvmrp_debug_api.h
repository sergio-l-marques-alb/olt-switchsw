

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dvmrp_debug_api.h
*
* @purpose DVMRP APIs
*
* @component DVMRP
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


#ifndef DVMRP_DEBUG_API_H
#define DVMRP_DEBUG_API_H

#include "l7_common.h"
#include "statsapi.h"
#include "comm_structs.h"


typedef enum
{
  DVMRP_DEBUG_PACKET_RX_TRACE = 0,
  DVMRP_DEBUG_PACKET_TX_TRACE,
  DVMRP_DEBUG_EVENT_TRACE,
  DVMRP_DEBUG_TIMER_TRACE,
  DVMRP_DEBUG_LAST_TRACE
}DVMRP_DEBUG_TRACE_FLAG_t;

/*********************************************************************
* @purpose  Get the current status of displaying dvmrp packet debug info
*            
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
L7_RC_t dvmrpDebugTraceFlagGet(DVMRP_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL *flag);
/*********************************************************************
* @purpose  Turns on/off the displaying of dvmrp packet debug info
*            
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
L7_RC_t dvmrpDebugTraceFlagSet(DVMRP_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag);

#endif



