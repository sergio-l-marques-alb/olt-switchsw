/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename nim_outcalls.h
*
* @purpose Network Interface Manager Call Out Headers
*
* @component nim
*
* @comments none
*
* @create 12/20/2002
*
* @author wjacobs
*
* @end
**********************************************************************/

#ifndef NIM_OUTCALLS_H
#define NIM_OUTCALLS_H

#include "l7_common.h"
/*********************************************************************
* @purpose Callout from NIM interface notifications
*
* @param   intIfNum    internal interface number
* @param   event       one of L7_PORT_EVENTS_t
*
* @return  none
*
* @notes   Hooks for interactions with other components or for platform-specific 
*          extensions to the standard dot1q offering.  This outcall occurs for 
*          NIM notifications of interface changes. The NIM callout may cause the 
*          event to be propagated.
*
*          The LVL7 hooks to interact are specific hooks for forwarding/not 
*          forwarding and link active/link inactive  notifications to be issued 
*          when spanning tree is not enabled.
*
*          These hooks also ensure that the underlying platform is appropriately 
*          configured with respect to spanning tree configuration.
*
*          These hooks also ensure that the filtering database is appropriately 
*          configured with respect to a port's MAC address.
*       
* @end
*********************************************************************/
void nimNotifyUserOfIntfChangeOutcall(L7_uint32 intIfNum, L7_uint32 event);

/*********************************************************************
*
* @purpose Callout from NIM active state check for the specified interface.
*
* @param intIfNum @b((input)) Internal Interface Number 
* @param activeResult @b((output)) pointer to result of outcalls, 
*          L7_YES or L7_NO
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*      
* @end
*
*********************************************************************/
L7_RC_t nimGetIntfActiveStateOutcall(L7_uint32 intIfNum, L7_uint32 *activeResult);


/*********************************************************************
* @purpose  Get the instance number associated with an interface
*
* @param    configId  @b{(input)} an instance of a nimConfigID_t structure
* @param    instance  @b{(output)} instance number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    Only supported for VLAN interfaces at this point
*
* @notes   Returns a relative instance number for the interface,
*          from 1 to the maximum number of ports for the interface type
*   
*       
* @end
*********************************************************************/
L7_RC_t nimPortInstanceNumGet(nimConfigID_t configId, L7_uint32 *instance);

#endif

