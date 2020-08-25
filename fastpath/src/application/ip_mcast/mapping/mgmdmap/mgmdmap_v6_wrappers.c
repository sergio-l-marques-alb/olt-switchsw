
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  mgmd_v6_wrappers.c
*
* @purpose   MGMD Mapping system infrastructure
*
* @component MGMD Mapping Layer
*
* @comments  none
*
* @create    12/04/2006
*
* @author    gkiran
* @end
*
**********************************************************************/

#include "l7_mgmdmap_include.h"

/********************************************************
* @purpose   This function is called to register with ipv6Map
*
* @param    routerProtocol_ID   @b{(input)}  routine router protocol id
*                                            (See L7_IP6MAP_REGISTRANTS_t)
* @param    name                @b{(input)}  pointer to the name of the 
*                                            registered routine,
*                                            up to IP6MAP_NOTIFY_FUNC_NAME_SIZE 
*                                            characters will be stored.
* @param    notify              @b{(input)}  pointer to a routine to be invoked for
*                                            routing event changes as listed in
*                                            L7_RTR_EVENT_CHANGE_t
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************/
L7_RC_t mgmdMapV6Register(L7_uint32 routerProtocol_ID, L7_char8 *name, 
                           L7_RC_t (*notify)(L7_uint32 intIfNum, 
                           L7_uint32 event, void *pData, 
                           ASYNC_EVENT_NOTIFY_INFO_t *response))
{

  /* register MGMD to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router
   * interface changes
   */
  if (ip6MapRegisterRoutingEventChange(routerProtocol_ID, name,
                                       notify) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error registering with ip6map for intf state change callback\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  DeRegister a routine to be called for routing events
*
* @param    routerProtocol_ID  @b{(input)} routine router protocol id
*                                          (See L7_IP6MAP_REGISTRANTS_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments Pass intIfNum as 0 for non-interface specific events.
*
* @end
*********************************************************************/
L7_RC_t mgmdMapV6Deregister(L7_uint32 routerProtocol_ID)
{
  if (ip6MapDeregisterRoutingEventChange(routerProtocol_ID) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error deregistering with ip6map for intf state change callback\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the operational administrative mode of the router
*
* @param    intIfNum       @b{(input)}  Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_BOOL  mgmdMapV6IpIntfExists(L7_uint32 intIfNum)
{
  return ip6MapIpIntfExists(intIfNum);
}

/*********************************************************************
* @purpose  To Get the Maximum No. Of Protocols.
*
* @param    void
*
* @returns  protoMax    maximum Number of Protocols present.
*
* @comments This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_uint32  mgmdMapProtocolsMaxGet(void)
{
  return (MGMD_MAP_IPV6_CB + 1);
}

/*********************************************************************
* @purpose  Checks if V6 mgmd user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdV6HasDataChanged(void)
{
  mgmdMapCB_t    *mgmd6MapCbPtr = L7_NULLPTR;

  (void)mgmdMapCtrlBlockGet(L7_AF_INET6, &mgmd6MapCbPtr);
  return (mgmd6MapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged);
}

/*********************************************************************
* @purpose  To get the mapping layer CB for IPV6
*
* @param    mgmdMapCbPtr    @b{(inout)} Mapping Control Block.
*
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapV6CtrlBlockGet(mgmdMapCB_t **mgmdMapCbPtr)
{
  *mgmdMapCbPtr = &mgmdMapCB_g[MGMD_MAP_IPV6_CB];
  return L7_SUCCESS;
}
