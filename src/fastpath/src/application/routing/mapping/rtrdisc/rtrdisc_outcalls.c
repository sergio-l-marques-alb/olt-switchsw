/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rtrdisc_api.c
*
* @purpose   Router Discovery API functions
*
* @component 
*
* @comments  
*
* @create    11/19/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
        
*************************************************************/

#include "l7_common.h"
#include "l7_common_l3.h"
#include "nimapi.h"
#include "dot3ad_api.h"
#include "simapi.h"
#include "mirror_api.h"
#include "rtrdisc.h"
#include "rtrdisc_api.h"
#include "l7_ip_api.h"

/*********************************************************************
* @purpose  Checks to see if the interface type is valid
*          
* @param    sysIntfType   @b{(input)} interface type
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL rtrDiscIntfIsValidType(L7_uint32 sysIntfType)
{
  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
    return L7_FALSE;

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Checks to see if the interface is valid
*          
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_BOOL rtrDiscIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FALSE;

  return rtrDiscIntfIsValidType(sysIntfType);
}

/*********************************************************************
* @purpose  Checks if rtr disc adv pkts can be sent out of this rtr intf
*
* @param    intIfnum    internal interface number
*
* @returns  L7_TRUE		if rtrdisc can send out adv pkts
* @returns  L7_FALSE  	if rtrdisc cannot send out adv pkts
*       
* @notes    If the routing interface is operational, router discovery
*           should be able to use it. IP MAP handles LAG and probe 
*           port acquire stuff. Router interface can only be up if
*           NIM says the interface is active.
*
*           Let's only run router discovery on numbered interfaces.
*       
* @end
*********************************************************************/
L7_BOOL rtrDiscMayEnableInterface(L7_uint32 intIfNum)
{
  L7_uint32 ifState;
  L7_BOOL isUnnumbered;
  L7_uint32 unused;

  if ((ipMapUnnumberedGet(intIfNum, &isUnnumbered, &unused) != L7_SUCCESS) ||
      (ipMapRtrIntfOperModeGet(intIfNum, &ifState) != L7_SUCCESS))
    return L7_FALSE;

  if ((ifState == L7_ENABLE) && (!isUnnumbered))
    return L7_TRUE;
  else
    return L7_FALSE;
}


