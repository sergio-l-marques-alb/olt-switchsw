
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    ospfv3_debug.c
* @purpose     OSPFv3 Debug functions
* @component   ospfv3
* @comments    none
* @create      09/15/2006
* @author      cpverne
* @end
*             
**********************************************************************/

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying ospfv3 packet debug info
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

L7_BOOL ospfv3MapDebugPacketTraceFlagGet();

/*********************************************************************
* @purpose  Turns on/off the displaying of ospfv3 packet debug info
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

L7_RC_t ospfv3MapDebugPacketTraceFlagSet(L7_BOOL flag);

#define OSPFV3_USER_TRACE(__fmt__, __args__... )                                      \
          if (ospfv3MapDebugPacketTraceFlagGet() == L7_TRUE)                          \
          {                                                                           \
            LOG_USER_TRACE(L7_FLEX_OSPFV3_MAP_COMPONENT_ID, __fmt__,##__args__);      \
          }

