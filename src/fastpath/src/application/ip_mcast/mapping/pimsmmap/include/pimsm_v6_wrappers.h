/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  pimsm_v6_wrappers.h
* @purpose   PIMSM Mapping  IPV6 registration functions.
* @component PIMSM Mapping Layer
* @comments  none
* @create    13/04/2006
* @author    gkiran
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/
#ifndef _PIMSM_V6_WRAPPERS_H_
#define _PIMSM_V6_WRAPPERS_H_

/********************************************************
* @purpose   This function is called to register with ipv6Map
*
* @param    routerProtocol_ID @b{(input)} routine router protocol id
*                                        (See L7_IP6MAP_REGISTRANTS_t)
* @param    name              @b{(input)} pointer to the name of the registered
*                                         routine, up to IP6MAP_NOTIFY_FUNC_NAME_SIZE 
*                                         characters will be stored.
* @param    notify            @b{(input)} pointer to a routine to be invoked for
*                                         routing event changes as listed in
*                                         L7_RTR_EVENT_CHANGE_t
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments none
*
* @end
*********************************************************/
L7_RC_t pimsmMapV6Register(L7_uint32 routerProtocol_ID, L7_char8 *name, 
                           L7_RC_t (*notify)(L7_uint32 intIfNum, 
                           L7_uint32 event, void *pData, 
                           ASYNC_EVENT_NOTIFY_INFO_t *response));
/*********************************************************************
* @purpose  DeRegister a routine to be called for routing events
*
* @param    routerProtocol_ID     @b{(input)}  routine router protocol id
*                                              (See L7_IP6MAP_REGISTRANTS_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments Pass intIfNum as 0 for non-interface specific events.
*
* @end
*********************************************************************/
L7_RC_t pimsmMapV6Deregister(L7_uint32 routerProtocol_ID);
/*********************************************************************
* @purpose  Register a callback function to be called when best routes 
*           are added, changed, or deleted. 
*
* @param  clientName @b{(input)}  Name of the client. For debugging only.   
* @param  funcPtr    @b{(input)}     Pointer to the callback function.
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
                                                 void (*funcPtr)(void));
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
                                                  void (*funcPtr)(void));
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
L7_uint32  pimsmMapProtocolsMaxGet(void);

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
L7_BOOL pimsmV6HasDataChanged(void);

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
L7_RC_t pimsmMapV6CtrlBlockGet(pimsmMapCB_t **pimsmMapCbPtr);

#endif /*_PIMSM_V6_WRAPPERS_H_*/
