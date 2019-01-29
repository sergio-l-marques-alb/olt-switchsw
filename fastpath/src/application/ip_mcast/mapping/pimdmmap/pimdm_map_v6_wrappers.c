
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  pimdm_v6_wrappers.c
*
* @purpose   PIMDM Mapping V6 Wrappers.
*
* @component PIMDM Mapping Layer
*
* @comments  none
*
* @create    12/04/2006
*
* @author    gkiran
*
* @end
*
**********************************************************************/

#include "async_event_api.h"
#include "l7_ip6_api.h"
#include "l7_rto6_api.h"
#include "log.h"
#include "pimdm_map_util.h"
#include "pimdm_map_debug.h"

/**********************************************************************
* @purpose   This function is called to register with ipv6Map
*
* @param    routerProtocol_ID  @b{(input)}  routine router protocol id
*                                           (See L7_IP6MAP_REGISTRANTS_t)
* @param    name               @b{(input)}  pointer to the name of the 
                                            registered routine,
*                                           up to IP6MAP_NOTIFY_FUNC_NAME_SIZE 
*                                           characters will be stored.
* @param    notify             @b{(input)}  pointer to a routine to be 
*                                           invoked for routing event changes 
*                                           as listed in L7_RTR_EVENT_CHANGE_t
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
***********************************************************************/
L7_RC_t pimdmMapV6Register(L7_uint32 routerProtocol_ID, L7_char8 *name, 
                           L7_RC_t (*notify)(L7_uint32 intIfNum, 
                           L7_uint32 event, void *pData, 
                           ASYNC_EVENT_NOTIFY_INFO_t *response))
{

  /* register PIMDM to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router
   * interface changes
   */
  if (ip6MapRegisterRoutingEventChange(routerProtocol_ID, name,
                                       notify) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error registering with ip6map for \
                      intf state change callback");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  DeRegister a routine to be called for routing events
*
* @param    routerProtocol_ID  @b{(input)}  routine router protocol id
*                                           (See L7_IP6MAP_REGISTRANTS_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments Pass intIfNum as 0 for non-interface specific events.
*
* @end
*********************************************************************/
L7_RC_t pimdmMapV6Deregister(L7_uint32 routerProtocol_ID)
{
  if (ip6MapDeregisterRoutingEventChange(routerProtocol_ID) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error Deregistering with ip6map \
            for intf state change callback");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register a callback function to be called when best routes 
*           are added, changed, or deleted. 
*
* @param  clientName @b{(input)}  Name of the client. For debugging only.   
* @param  funcPtr    @b{(input)}  Pointer to the callback function.
*
* @returns  L7_SUCCESS  if the callback function is registered.
* @returns  L7_FAILURE  if the callback function is not registered, 
*                       either because of an error in the input 
*                       parameters or because the maximum number of
*                       callbacks are already registered.
*
* @comments If a caller attempts to register a callback function 
*           that is already registered, the function returns 
*           L7_SUCCESS without registering the function a second 
*           time.
*
* @end
*******************************************************************/
L7_RC_t pimdmMapRto6BestRouteRegister (L7_uchar8 *clientName, 
                                                 void (*funcPtr)(void))
{

  if (rto6BestRouteClientRegister (clientName, funcPtr) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error registering with RTO6 for intf \
                      state change callback");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deregister a callback function to be called when best routes 
*           are added, changed, or deleted. 
*
* @param  clientName  @b{(input)}  Name of the client. For debugging only.   
* @param  funcPtr     @b{(input)}  Pointer to the callback function.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments none    
*
* @end
*******************************************************************/
L7_RC_t pimdmMapRto6BestRouteDeRegister (L7_uchar8 *clientName, 
                                                  void (*funcPtr)(void))
{
  if (rto6BestRouteClientDeregister (clientName, funcPtr) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error Deregistering with RTO6 for intf state \
            change callback");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To Get the Maximum No. Of Protocols.
*
* @param    none
*
* @returns  protoMax    @b{(output)} maximum Number of Protocols present.
*
* @comments This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_uint32  pimdmMapProtocolsMaxGet(void)
{
  return (PIMDM_MAP_IPV6_CB + 1);
}

/*********************************************************************
* @purpose  Check if PIMDM V6 user config data has changed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimdmV6HasDataChanged(void)
{
  pimdmMapCB_t    *pimdm6MapCbPtr = L7_NULLPTR;

  (void)pimdmMapCtrlBlockGet(L7_AF_INET6, &pimdm6MapCbPtr);

  return (pimdm6MapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged);
}

/*********************************************************************
* @purpose  To get the mapping layer CB for IPV6
*
* @param    pimdmMapCbPtr  @b{(output)} Mapping Control Block.
*
* @returns  L7_SUCCESS  if success
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapV6CtrlBlockGet(pimdmMapCB_t **pimdmMapCbPtr)
{
   *pimdmMapCbPtr = &pimdmMapCB_g[PIMDM_MAP_IPV6_CB];
   return L7_SUCCESS;
}

