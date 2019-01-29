
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  mgmd_stub.c
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
* @param    routerProtocol_ID  @b{(input)}  routine router protocol id
*                                           (See L7_IP6MAP_REGISTRANTS_t)
* @param    name               @b{(input)}   pointer to the name of the 
                               registered routine,
*                                 up to IP6MAP_NOTIFY_FUNC_NAME_SIZE 
*                                 characters will be stored.
* @param    notify             @b{(input)}   pointer to a routine to be invoked for
*                                 routing event changes as listed in
*                                 L7_RTR_EVENT_CHANGE_t
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
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get the operational administrative mode of the router
*
* @param    intIfNum   @b{(input)}  Internal Interface Number
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
  return L7_FAILURE;
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
  return (MGMD_MAP_IPV4_CB + 1);
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
  return L7_FALSE;
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
  *mgmdMapCbPtr = L7_NULLPTR;
  return L7_FAILURE;
}
