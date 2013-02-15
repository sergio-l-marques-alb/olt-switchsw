/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  pimdm_stub.c
*
* @purpose   PIMDM Mapping system infrastructure
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

/**********************************************************************
* @purpose   This function is called to register with ipv6Map
*
* @param    routerProtocol_ID  @b{(input)}  routine router protocol id
*                                           (See L7_IP6MAP_REGISTRANTS_t)
* @param    name               @b{(input)}  pointer to the name of the 
*                                           registered routine,
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
  return L7_FAILURE;
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
* @comments none    
*
* @end
*******************************************************************/
L7_RC_t pimdmMapRto6BestRouteRegister (L7_uchar8 *clientName, 
                                                 void (*funcPtr)(void))
{
  return L7_FAILURE;
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
  return L7_FAILURE;
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
  return (PIMDM_MAP_IPV4_CB + 1);
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
  return L7_FALSE;
}

/*********************************************************************
* @purpose  To get the mapping layer CB for IPV6
*
* @param    pimdmMapCbPtr  @b{(output)} Mapping Control Block.
*
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapV6CtrlBlockGet(pimdmMapCB_t **pimdmMapCbPtr)
{
   *pimdmMapCbPtr = L7_NULLPTR;
   return L7_FAILURE;
}

