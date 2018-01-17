
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   ipmap_outcalls.c
*
* @purpose    IP Map component outcall functions.
*
* @component  IP MAP
*
* @comments   
*
* @create     8/12/2003
*
* @author     markl
*
* @end
*
*********************************************************************/

#include "l7_ipinclude.h"

/*---------------------------------------*/
/*Applying bandwidth change to the OspfV3  */
/*---------------------------------------*/
extern L7_RC_t ip6MapIntfBandwidthApply(L7_uint32 intIfNum, L7_uint32 bandwidth);

/*********************************************************************
* @purpose  Determine if the interface type is valid for IPv4
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL ipMapIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
    case L7_LOGICAL_VLAN_INTF:
    case L7_LOOPBACK_INTF:
      return L7_TRUE;
      /*break;*/

    default:
      return L7_FALSE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for participation in the component
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL ipMapIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return ipMapIsValidIntfType(sysIntfType);
  }
  return L7_FALSE;
}


/*********************************************************************
 * @purpose  Set interface bandwidth
 *
 * @param    intIfNum  @b{(input)} Internal Interface Number
 * @param    bandwidth @b{(input)} bandwidth
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapv6IntfBandwidthApply(L7_uint32 intIfNum, L7_uint32 bandwidth)
{
  L7_RC_t rc = L7_SUCCESS;

#ifdef L7_IPV6_PACKAGE
  rc = ip6MapIntfBandwidthApply(intIfNum, bandwidth);
#endif

  return rc;
}

