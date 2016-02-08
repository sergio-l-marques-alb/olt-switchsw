/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  pimsm_map_v6utils.c
*
* @purpose   PIMSM Mapping system infrastructure
*
* @component PIMSM Mapping Layer
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

#include "l7_pimsminclude.h"
#include "pimsm_map.h"
/********************************************************
* @purpose   This function is called to register with ipv6Map
*
* @param    routerProtocol_ID  @b{(input)} routine router protocol id
*                                          (See L7_IP6MAP_REGISTRANTS_t)
* @param    name               @b{(input)} pointer to the name of the registered
*                                          routine, up to 
*                                          IP6MAP_NOTIFY_FUNC_NAME_SIZE 
*                                          characters will be stored.
* @param    notify             @b{(input)} pointer to a routine to be invoked for
*                                          routing event changes as listed in
*                                          L7_RTR_EVENT_CHANGE_t
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************/
L7_RC_t pimsmMapV6Register(L7_uint32 routerProtocol_ID, L7_char8 *name, 
                           L7_RC_t (*notify)(L7_uint32 intIfNum, 
                           L7_uint32 event, void *pData, 
                           ASYNC_EVENT_NOTIFY_INFO_t *response))
{

  /* register PIMSM to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router
   * interface changes
   */
  if (ip6MapRegisterRoutingEventChange(routerProtocol_ID, name,
                                       notify) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error registering with ip6map for intf state \
                      change callback");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  DeRegister a routine to be called for routing events
*
* @param    routerProtocol_ID   @b{(input)}  routine router protocol id
*                                            (See L7_IP6MAP_REGISTRANTS_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments Pass intIfNum as 0 for non-interface specific events.
*
* @end
*********************************************************************/
L7_RC_t pimsmMapV6Deregister(L7_uint32 routerProtocol_ID)
{
  if (ip6MapDeregisterRoutingEventChange(routerProtocol_ID) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error Deregistering with ip6map \
                      for intf state change callback");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register a callback function to be called when best routes 
*           are added, changed, or deleted. 
*
* @param  clientName   @b{(input)}  Name of the client. For debugging only.   
* @param  funcPtr      @b{(input)}  Pointer to the callback function.
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
* @comments none    
*
* @end
*******************************************************************/
L7_RC_t pimsmMapRto6BestRouteRegister (L7_uchar8 *clientName, 
                                                 void (*funcPtr)(void))
{

  if (rto6BestRouteClientRegister (clientName, funcPtr) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error registering with RTO6 for intf state \
                      change callback");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deregister a callback function to be called when best routes 
*           are added, changed, or deleted. 
*
* @param  clientName @b{(input)}  Name of the client. For debugging only.   
* @param  funcPtr    @b{(input)}  Pointer to the callback function.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments none    
*
* @end
*******************************************************************/
L7_RC_t pimsmMapRto6BestRouteDeRegister (L7_uchar8 *clientName, 
                                                  void (*funcPtr)(void))
{
  if (rto6BestRouteClientDeregister (clientName, funcPtr) != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error Deregistering with RTO6 \
                      for intf state change callback");
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
L7_uint32  pimsmMapProtocolsMaxGet(void)
{
  return (PIMSM_MAP_IPV6_CB + 1);
}

/*********************************************************************
* @purpose  Check if PIM-SM V6 user config data has changed
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
L7_BOOL pimsmV6HasDataChanged(void)
{
  pimsmMapCB_t    *pimsm6MapCbPtr = L7_NULLPTR;

  (void)pimsmMapCtrlBlockGet(L7_AF_INET, &pimsm6MapCbPtr);

  return (pimsm6MapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged);
}

/*********************************************************************
* @purpose  Get PIM-SM V6 control block
*
* @param    none
*
* pimsmMapCbPtr @b{(inout)} Mapping Control Block.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapV6CtrlBlockGet(pimsmMapCB_t **pimsmMapCbPtr)
{
  extern pimsmMapCB_t        *pimsmMapCB;

   *pimsmMapCbPtr = &pimsmMapCB[PIMSM_MAP_IPV6_CB];
   return L7_SUCCESS;
}

