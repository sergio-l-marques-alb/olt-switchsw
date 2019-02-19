
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  mcast_stub.c
*
* @purpose   MCAST Mapping system infrastructure
*
* @component MCAST Mapping Layer
*
* @comments  none
*
* @create    10/03/2006
*
* @author    nramu
* @end
*
**********************************************************************/

#include "l3_mcast_commdefs.h"
#include "mcast_api.h"
#include "mcast_debug.h"
#include "log.h"
#include "mcast_debug.h"

/********************************************************
* @purpose   This function is called to send IPv6 multicast control packets.
*
* @param     pSendPkt      @b{(input)}  structure containing the buffer 
*                                        to sendpacket out
*
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************/
L7_RC_t mcastMapV6PacketSend(mcastSendPkt_t *pSendPkt)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP -v4: Failed to send V6 packet in V4 build\n");
  return L7_FAILURE;
}
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
L7_RC_t mcastMapV6Register(L7_uint32 routerProtocol_ID, L7_char8 *name, 
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
L7_RC_t mcastMapV6Deregister(L7_uint32 routerProtocol_ID)
{
  return L7_FAILURE;
}


