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
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL rtrDiscLinkStateGet(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_uint32 state;

  /* Link becomes active only if the interface is not member of lag and
     not mirror dest port and the link is in forwarding state.
  */
  rc = dot3adIsLagMember(intIfNum);
  if (rc == L7_TRUE)
    return L7_FALSE;
  rc = mirrorIsActiveProbePort(intIfNum);
  if (rc == L7_TRUE)
    return L7_FALSE;
  rc = nimGetIntfActiveState(intIfNum,&state);
  if (rc == L7_SUCCESS && state == L7_INACTIVE)
    return L7_FALSE;

  return L7_TRUE;
}


