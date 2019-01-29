/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   rip_map_outcalls.c
*
* @purpose    RIP component outcall functions.
*
* @component  RIP
*
* @comments   none
*
* @create     09/23/2003
*
* @author     skanchi
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_ripinclude.h"
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
L7_BOOL ripMapIsAclPresent(void)
{
  return cnfgrIsComponentPresent(L7_FLEX_QOS_ACL_COMPONENT_ID);
}

/*********************************************************************
* @purpose  Register RIP to be notified of ACL changes
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
L7_RC_t ripMapAclCallbackRegister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr,
                                  L7_uchar8 *funcNameStr)
{
    L7_RC_t rc = L7_NOT_EXIST;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  rc = aclRouteFilterCallbackRegister(funcPtr, L7_RIP_MAP_COMPONENT_ID, funcNameStr);
#endif

  return rc;
}

/*********************************************************************
* @purpose  Remove RIP from ACL component's list of users to notify
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
L7_RC_t ripMapAclCallbackUnregister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr)
{
    L7_RC_t rc = L7_NOT_EXIST;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  rc = aclRouteFilterCallbackUnregister(funcPtr);
#endif

  return rc;
}

/*********************************************************************
* @purpose  Indicate whether RIP's ACL change callback function is
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
L7_BOOL ripMapAclCallbackIsRegistered(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr)

{
    L7_BOOL rc = L7_FALSE;
    
#ifdef L7_QOS_FLEX_PACKAGE_ACL
    rc = aclRouteFilterCallbackIsRegistered(funcPtr);
#endif

    return rc;
}

/*********************************************************************
* @purpose  Check if the route is permitted by the ACL
*
* @param    aclnum  @b{(input)} number of access list to filter route
* @param    routePrefix  @b{(input)} Destination prefix of route
* @param    routeMask  @b{(input)} Destination mask of route
*
* @returns  L7_SUCCESS     The route is permitted
* @returns  L7_FAILURE     The route is denied
*
* @comments If the QoS package is not present, permits the route. 
*       
* @end
*********************************************************************/
L7_RC_t ripMapAclRouteFilter(L7_uint32 aclnum,
                              L7_uint32 routePrefix,
                              L7_uint32 routeMask)
{
  L7_RC_t rc = L7_SUCCESS;
  FILTER_ACTION_t action;

  action = FILTER_ACTION_PERMIT;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  rc = aclRouteFilter(aclnum, routePrefix, routeMask, &action);
#endif

  if (((rc == L7_SUCCESS) && (action == FILTER_ACTION_PERMIT)) || (rc == L7_FAILURE))
    return L7_SUCCESS;

  if ((rc == L7_SUCCESS) && (action == FILTER_ACTION_DENY))
    return L7_FAILURE;
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks to see if the interface type is valid to participate in RIP
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
L7_BOOL ripMapIntfIsValidType(L7_uint32 sysIntfType)
{
  if ((sysIntfType != L7_PHYSICAL_INTF)  &&
      (sysIntfType != L7_LAG_INTF)       &&
      (sysIntfType != L7_LOGICAL_VLAN_INTF))
    return L7_FALSE;

  return L7_TRUE;
}
/*********************************************************************
* @purpose  Checks to see if the interface is valid to participate in RIP
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
L7_BOOL ripMapIntfIsValid(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FALSE;

  return ripMapIntfIsValidType(sysIntfType);
}

