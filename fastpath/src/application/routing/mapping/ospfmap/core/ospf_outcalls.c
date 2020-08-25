/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   ospf_outcalls.c
*
* @purpose    OSPF component outcall functions.
*
* @component  OSPF
*
* @comments   none
*
* @create     07/15/2003
*
* @author     rrice
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_ospfinclude.h"
#include "acl_api.h"


/*********************************************************************
* @purpose  Check if the ACL component is present
*
* @param    Void
*
* @returns  L7_TRUE, if ACL component is present
* @returns  L7_FALSE, if ACL component is absent
*
* @comments None 
*       
* @end
*********************************************************************/
L7_BOOL ospfMapIsAclPresent(void)
{
  return cnfgrIsComponentPresent(L7_FLEX_QOS_ACL_COMPONENT_ID);
}

/*********************************************************************
* @purpose  Register OSPF to be notified of ACL changes
*
* @param    funcPtr - Pointer to callback function that should be called
*                     for ACL events.
* @param    funcNameStr - Name of callback function (for debugging).
*
* @returns  L7_SUCCESS if registration is successful
* @returns  L7_FAILURE if registration fails
* @returns  L7_REQUEST_DENIED if the ACL component is not yet initialized
* @returns  L7_NOT_EXIST if the QoS package is not present or the ACL
*                        component is not yet initialized
*
* @comments None 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAclCallbackRegister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr,
                                   L7_uchar8 *funcNameStr)
{
    L7_RC_t rc = L7_NOT_EXIST;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  rc = aclRouteFilterCallbackRegister(funcPtr, L7_OSPF_MAP_COMPONENT_ID, funcNameStr);
#endif

  return rc;
}

/*********************************************************************
* @purpose  Remove OSPF from ACL component's list of users to notify
*           of ACL changes.
*
* @param    funcPtr - Pointer to callback function that should be called
*                     for ACL events.
*
* @returns  L7_SUCCESS if de-registration is successful
* @returns  L7_FAILURE if de-registration fails
* @returns  L7_NOT_EXIST if the QoS package is not present
*
* @comments None 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAclCallbackUnregister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr)
{
    L7_RC_t rc = L7_NOT_EXIST;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  rc = aclRouteFilterCallbackUnregister(funcPtr);
#endif

  return rc;
}

/*********************************************************************
* @purpose  Indicate whether OSPF's ACL change callback function is
*           registered with the ACL component.
*
* @param    funcPtr - Pointer to callback function that should be called
*                     for ACL events.
*
* @returns  L7_TRUE if registered
* @returns  L7_FALSE if not registered (or ACL component not in build)
*
* @comments None 
*       
* @end
*********************************************************************/
L7_BOOL ospfMapAclCallbackIsRegistered(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr)
{
    L7_BOOL rc = L7_FALSE;
    
#ifdef L7_QOS_FLEX_PACKAGE_ACL
    rc = aclRouteFilterCallbackIsRegistered(funcPtr);
#endif

    return rc;
}

/*********************************************************************
* @purpose  Apply an ACL as a route filter
*
* @param    aclnum  @b{(input)} number of access list to filter route
* @param    routePrefix  @b{(input)} Destination prefix of route
* @param    routeMask  @b{(input)} Destination mask of route
* @param    action  @b{(output)} FILTER_ACTION_PERMIT if the route passes the filter 
*                                FILTER_ACTION_DENY if the route does not pass 
*
* @returns  L7_SUCCESS 
*
* @comments If the QoS package is not present, permits the route. 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAclRouteFilter(L7_uint32 aclnum,
                              L7_uint32 routePrefix,
                              L7_uint32 routeMask,
                              FILTER_ACTION_t *action)
{
    L7_RC_t rc = L7_SUCCESS;
    *action = FILTER_ACTION_PERMIT;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  rc = aclRouteFilter(aclnum, routePrefix, routeMask, action);
#endif

  return rc;
}

/*********************************************************************
* @purpose  Determine if the interface type is valid in ospf
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
L7_BOOL ospfMapIsValidIntfType(L7_uint32 sysIntfType)
{
  if (sysIntfType == L7_TUNNEL_INTF)
  {
    return L7_FALSE;
  }

  return ipMapIsValidIntfType(sysIntfType);
}

/*********************************************************************
* @purpose  Determine if the interface is valid in ospf
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
L7_BOOL ospfMapIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  return ospfMapIsValidIntfType(sysIntfType);
}

