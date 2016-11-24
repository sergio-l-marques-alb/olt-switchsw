/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3ad_laih.c                        
*
* @purpose LAG interface handler - and interface to NIM
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @notes none
*
* @create 6/5/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "dot3ad_include.h"
#ifdef DOT3AD_COMPONENT_LAIH

/*********************************************************************
* @purpose  Sets the interface admin state 
*           
*
* @param    intIfNum    internal interface number
* @param    state       admin state L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfAdminStateSet(L7_uint32 intIfNum, L7_uint32 state)
{
  L7_RC_t rc = L7_FAILURE;
  rc = nimSetIntfAdminState(intIfNum,state);
  return rc;
}
/*********************************************************************
* @purpose  Notifies the system of a particular event regarding an interface
*           
*
* @param    intIfNum    internal interface number
* @param    event       the event that needs to be propagated
*                       L7_PORT_DISABLE = 0,
*                       L7_PORT_ENABLE,
*                       L7_DOWN,
*                       L7_UP,
*                       L7_INACTIVE,
*                       L7_ACTIVE,
*                       L7_FORWARDING,
*                       L7_NOT_FORWARDING,
*                       L7_CREATE,
*                       L7_DELETE,
*                       L7_LAG_ACQUIRE,
*                       L7_LAG_RELEASE,
*                       L7_SPEED_CHANGE,
*                       L7_LAG_CFG_CREATE,
*                       L7_LAG_CFG_MEMBER_CHANGE,
*                       L7_LAG_CFG_REMOVE,
*                       L7_LAG_CFG_END,
*                       L7_PROBE_SETUP,
*                       L7_PROBE_TEARDOWN,
*                       L7_SET_INTF_SPEED,                                                      
*                       L7_SET_MTU_SIZE,                                                        
*                       L7_PORT_ROUTING_ENABLED,                                             
*                       L7_PORT_ROUTING_DISABLED,                                              
*                       L7_PORT_BRIDGING_ENABLED,                                             
*                       L7_PORT_BRIDGING_DISABLED,                                              
*                       L7_LAST_PORT_EVENT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    this notifiacation is sent to NIM which in turn passes it to all the registered
*           routines
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihNotifySystem(L7_uint32 intIfNum, L7_uint32 event)
{
  L7_RC_t rc = L7_FAILURE;
  NIM_HANDLE_t           handle;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;

  eventInfo.component     = L7_DOT3AD_COMPONENT_ID;
  eventInfo.pCbFunc       = L7_NULLPTR;
  eventInfo.event         = event;
  eventInfo.intIfNum      = intIfNum;

  rc = nimEventIntfNotify(eventInfo,&handle);
  return rc;
}
/*********************************************************************
* @purpose  Sets the trapping  of the interface
*           
*
* @param    intIfNum    internal interface number
* @param    trapState   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfLinkTrapSet(L7_uint32 intIfNum, L7_uint32 trapState)
{
  L7_RC_t rc = L7_FAILURE;
  rc = nimSetIntfLinkTrap(intIfNum,trapState);
  return rc;
}
/*********************************************************************
* @purpose  Gets the trapping  of the interface
*           
*
* @param    intIfNum    internal interface number
* @param    trapState   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfLinkTrapGet(L7_uint32 intIfNum, L7_uint32 *trapState)
{
  L7_RC_t rc = L7_FAILURE;
  rc = nimGetIntfLinkTrap(intIfNum,trapState);
  return rc;
}
/*********************************************************************
* @purpose  Sets an interface as a member interface of a logical macroport
*           
*
* @param    aggId     Macroport internal interface number
* @param    intIfNum  internal interface number of the member interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihMacroPortAssignmentSet(L7_uint32 aggId, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  nimMacroPort_t  macroPortInfo;

  macroPortInfo.macroInfo = L7_NULLPTR;
  macroPortInfo.macroPort = aggId;
  macroPortInfo.macroType = L7_LAG_INTF;
  rc = nimGetIntfMtuSize(aggId, &macroPortInfo.macroMtu);
  rc = nimGetIntfConfigMaxFrameSize(aggId, &macroPortInfo.macroMaxFrame); 

  rc = nimSetMacroPortAssignment(intIfNum, &macroPortInfo);
  return rc;


}
/*********************************************************************
* @purpose  Resets the macroport association for this interface
*           
*
* @param    intIfNum    internal interface number
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihMacroPortAssignmentReset(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  rc = nimResetMacroPortAssignment(intIfNum);
  return rc;


}
/*********************************************************************
* @purpose  Gets the dataRate for a particular interface
*           
*
* @param    intIfNum    internal interface number
* @param    *dataRate      pointer to the dataRate
*                       
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfSpeedDataRateGet(L7_uint32 intIfNum, L7_uint32 *dataRate)
{
  L7_RC_t rc = L7_FAILURE;

  rc = nimGetIntfSpeedDataRate(intIfNum,dataRate);
  return rc;

}
/*********************************************************************
* @purpose  Checks if an interface is full duplex
*           
*
* @param    intIfNum    internal interface number
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot3adLihIsIntfSpeedFullDuplex(L7_uint32 intIfNum)
{
  L7_BOOL rc = L7_FALSE;

  rc = nimIsIntfSpeedFullDuplex(intIfNum);
  return rc;
}
/*********************************************************************
* @purpose  Checks if the interface type is LAG
*           
*
* @param    intIfNum    internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot3adLihIsIntfTypeLag(L7_uint32 intIfNum)
{
  L7_BOOL rc = L7_FALSE;
  L7_RC_t nimrc = L7_FAILURE;
  L7_INTF_TYPES_t type;

  nimrc = nimGetIntfType(intIfNum,&type);
  if (type == L7_LAG_INTF)
  {
    return L7_TRUE;
  }
  return rc;
}
/*********************************************************************
* @purpose  Checks if the interface type is physical
*           
*
* @param    intIfNum    internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot3adLihIsIntfTypePhy(L7_uint32 intIfNum)
{
  L7_BOOL rc = L7_FALSE;
  L7_RC_t nimrc = L7_FAILURE;
  L7_INTF_TYPES_t type;

  nimrc = nimGetIntfType(intIfNum,&type);
  if (type == L7_PHYSICAL_INTF)
  {
    return L7_TRUE;
  }
  return rc;
}
/*********************************************************************
* @purpose  
*           
*
* @param    intIfNum    internal interface number
* @param    *speed      pointer to speed
*                       L7_PORTCTRL_PORTSPEED_AUTO_NEG   = 1,
*                       L7_PORTCTRL_PORTSPEED_HALF_100TX,                              
*                       L7_PORTCTRL_PORTSPEED_FULL_100TX,                              
*                       L7_PORTCTRL_PORTSPEED_HALF_10T,                                
*                       L7_PORTCTRL_PORTSPEED_FULL_10T,                                
*                       L7_PORTCTRL_PORTSPEED_FULL_100FX,                              
*                       L7_PORTCTRL_PORTSPEED_FULL_1000SX,
*                       L7_PORTCTRL_PORTSPEED_AAL5_155,
*                       L7_PORTCTRL_PORTSPEED_LAG,
*                       L7_PORTCTRL_PORTSPEED_UNKNOWN
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfSpeedGet(L7_uint32 intIfNum, L7_uint32 *speed)
{
  L7_RC_t rc = L7_FAILURE;
  rc = nimGetIntfSpeedStatus(intIfNum,speed);
  return rc;
}
/*********************************************************************
* @purpose  Gets the link state of the interface
*           
*
* @param    intIfNum    internal interface number
* @param    *state      L7_UP or L7_DOWN
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfLinkStateGet(L7_uint32 intIfNum, L7_uint32 *state)
{
  L7_RC_t rc = L7_FAILURE;
  rc = nimGetIntfLinkState(intIfNum,state);
  return rc;
}
/*********************************************************************
* @purpose  Gets the MAC Address associated with an interface
*           
*
* @param    intIfNum    internal interface number
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured 
*                       MAC Address
* @param    *macAddr    pointer to mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfAddrGet(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_FAILURE;
  rc = nimGetIntfAddress(intIfNum,addrType,macAddr);
  return rc;
}

/*********************************************************************
* @purpose  Checks if the interface type is already associated with
*           another logical interface
*           
*
* @param    intIfNum    internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot3adLihIsLifMember(L7_uint32 intIfNum)
{
  L7_BOOL rc = L7_FALSE;

  return rc;


}

/*********************************************************************
* @purpose  Retreives the unit slot and port of a given interface
*           
*           
*
* @param    intIfNum    internal interface number
* @param    *usp        pointer to the usp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfUspGet(L7_uint32 intIfNum, dot3ad_usp_t *usp)
{
  L7_RC_t rc = L7_FAILURE;
  nimUSP_t nimUsp;

  rc = nimGetUnitSlotPort(intIfNum, &nimUsp);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  usp->unit = nimUsp.unit;
  usp->slot = nimUsp.slot;
  usp->port = nimUsp.port;
  return rc;

}
/*********************************************************************
* @purpose  Retreives auto negosiation status of a given interface
*           
*           
*
* @param    intIfNum        internal interface number
* @param    *autoNegoStatus pointer to the status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adLihIntfAutoNegStatusGet(L7_uint32 intIfNum, L7_uint32 *autoNegoStatus)
{
  return nimGetIntfAutoNegAdminStatus(intIfNum, autoNegoStatus);
}

/*********************************************************************
* @purpose  Check if a given interface is a part of any MAC filter
*           
* @param    intIfNum        internal interface number
*
* @returns  L7_TRUE, if the interface is a part (dst list or src list) of a filter
* @returns  L7_FALSE, if the interface in not a part of any filter
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot3adLihIsFilterMember(L7_uint32 intIfNum)
{
#ifdef L7_STATIC_FILTERING_PACKAGE
    return filterIsIntfInAnyFilter(intIfNum);
#else
    return L7_FALSE;
#endif
}
/*********************************************************************
* @purpose  return the highest assigned inteface number 
*          
* @param    highestIntfNumber   pointer to return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t dot3adLihHighestIntfNumGet(L7_uint32 *highestIntfNumber)
{
  return nimGetHighestIntfNumber(highestIntfNumber);
}
#endif /*DOT3AD_COMPONENT_LAIH*/
