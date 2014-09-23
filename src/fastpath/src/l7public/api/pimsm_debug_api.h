

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename pimsm_debug_api.h
*
* @purpose pimsm APIs
*
* @component pimsm
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


#ifndef PIMSM_DEBUG_API_H
#define PIMSM_DEBUG_API_H

#include "l7_common.h"
#include "statsapi.h"
#include "comm_structs.h"


typedef enum
{
  PIMSM_DEBUG_PACKET_RX_TRACE = 0,
  PIMSM_DEBUG_PACKET_TX_TRACE,
  PIMSM_DEBUG_EVENT_TRACE,
  PIMSM_DEBUG_TIMER_TRACE,
  PIMSM_DEBUG_LAST_TRACE
}PIMSM_DEBUG_TRACE_FLAG_t;

/*********************************************************************
* @purpose  Get the current status of displaying pimsm packet debug info
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
L7_RC_t pimsmDebugTraceFlagGet(L7_uchar8 family,
                               PIMSM_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL *flag);
/*********************************************************************
* @purpose  Turns on/off the displaying of pimsm packet debug info
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
L7_RC_t pimsmDebugTraceFlagSet(L7_uchar8 family, 
                               PIMSM_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag);
#endif

