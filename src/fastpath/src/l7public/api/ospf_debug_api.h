
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    ospf_debug.c
* @purpose     OSPF Debug functions
* @component   ospf
* @comments    none
* @create      08/02/2006
* @author      cpverne
* @end
*             
**********************************************************************/

#include "l7_common.h"

/*********************************************************************
* @purpose  Get the current status of displaying ospf packet debug info
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

L7_BOOL ospfMapDebugPacketTraceFlagGet();

/*********************************************************************
* @purpose  Turns on/off the displaying of ospf packet debug info
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

L7_RC_t ospfMapDebugPacketTraceFlagSet(L7_BOOL flag);


#define OSPF_USER_TRACE(__fmt__, __args__... )                              \
          if (ospfMapDebugPacketTraceFlagGet() == L7_TRUE)                  \
          {                                                                 \
            LOG_USER_TRACE(L7_OSPF_MAP_COMPONENT_ID, __fmt__,##__args__);   \
          }



void ospfMapDebugPacketRxTrace(L7_char8 *interface, L7_uint32 source, L7_uint32 dest,L7_char8 *buff);
void ospfMapDebugPacketTxTrace(L7_char8 *interface, L7_uint32 source, L7_uint32 dest,L7_char8 *buff);

