/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    bgp_debug.c
* @purpose     BGO debug functions
* @component   bgp
* @comments    none
* @create      08/02/2006
* @author      cpverne
* @end
*             
**********************************************************************/

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying bgp packet debug info
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

L7_BOOL bgpMapDebugPacketTraceFlagGet();

/*********************************************************************
* @purpose  Turns on/off the displaying of bgp packet debug info
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

L7_RC_t bgpMapDebugPacketTraceFlagSet(L7_BOOL flag);

#define BGP_USER_TRACE(__fmt__, __args__... )                                     \
          if (bgpMapDebugPacketTraceFlagGet() == L7_TRUE)                         \
          {                                                                       \
            LOG_USER_TRACE(L7_FLEX_BGP_MAP_COMPONENT_ID, __fmt__,##__args__);     \
          }

