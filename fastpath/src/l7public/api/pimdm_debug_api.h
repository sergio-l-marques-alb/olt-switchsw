

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename pimdm_debug_api.h
*
* @purpose pimdm APIs
*
* @component pimdm
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


#ifndef PIMDM_DEBUG_API_H
#define PIMDM_DEBUG_API_H

#include "l7_common.h"
#include "statsapi.h"
#include "comm_structs.h"

typedef enum
{
  PIMDM_DEBUG_PACKET_RX_TRACE = 0,
  PIMDM_DEBUG_PACKET_TX_TRACE,
  PIMDM_DEBUG_EVENT_TRACE,
  PIMDM_DEBUG_TIMER_TRACE,
  PIMDM_DEBUG_LAST_TRACE
}PIMDM_DEBUG_TRACE_FLAG_t;

/*********************************************************************
* @purpose  Get the current status of displaying pimdm packet debug info
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
L7_RC_t pimdmDebugTraceFlagGet(L7_uchar8 family,
                               PIMDM_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL *flag);
/*********************************************************************
* @purpose  Turns on/off the displaying of pimdm packet debug info
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
L7_RC_t pimdmDebugTraceFlagSet(L7_uchar8 family, 
                               PIMDM_DEBUG_TRACE_FLAG_t traceFlag,
                               L7_BOOL flag);
#endif

