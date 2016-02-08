/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  mcast_v6_packet.c
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
*
* @end
*
**********************************************************************/
#include "l7_socket.h"
#include "osapi_support.h"
#include "l3_mcast_commdefs.h"
#include "log.h"
#include "l3_defaultconfig.h"
#include "mcast_debug.h"
#include "mcast_api.h"
#define MCAST_MAX_PKT_LEN               1500

/********************************************************
* @purpose   Send IPv6 multicast control packets.
*
* @param     pSendPkt   @b{(input)}  structure containing the buffer 
*                                    to sendpacket out
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************/
L7_RC_t mcastMapV6PacketSend(mcastSendPkt_t *pSendPkt)
{
  L7_int32 intIfNum =-1;
  L7_uint32 bytesSent =0;
  L7_RC_t rc;
  L7_sockaddr_in6_t sockFrom, sockTo;
  L7_uchar8 addrBuffStr[IPV6_DISP_ADDR_LEN];

  
  if (L7_NULLPTR == pSendPkt)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP-v6: NULL Packet info to send IPv6 packet\n");
    return L7_FAILURE;
  }

  /*Convert router Interface Number to Internal Interface Number */
  if (ip6MapRtrIntfToIntIfNum(pSendPkt->rtrIfNum,&intIfNum) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP -v6: conversion to intIfNum from rtrIfNum = %d failed.\n",
             pSendPkt->rtrIfNum);
    return L7_FAILURE;
  }

  memset(&sockFrom, 0, sizeof(sockFrom));
  sockFrom.sin6_family = L7_AF_INET6;
  sockFrom.sin6_len = sizeof(sockFrom);
  memcpy(&sockFrom.sin6_addr, &pSendPkt->srcAddr.addr.ipv6, sizeof(L7_in6_addr_t));
  
  memset(&sockTo, 0, sizeof(sockTo));
  sockTo.sin6_family = L7_AF_INET6;
  sockTo.sin6_len = sizeof(sockTo);
  sockTo.sin6_port = osapiHtons(pSendPkt->port);
  memcpy(&sockTo.sin6_addr, &pSendPkt->destAddr.addr.ipv6, sizeof(L7_in6_addr_t));
  
  rc = osapiPktInfoSend(pSendPkt->sockFd,
   (L7_uchar8 *)pSendPkt->payLoad, 
   pSendPkt->length, pSendPkt->flags,
   (L7_sockaddr_t *)&sockFrom, sizeof(sockFrom),
   (L7_sockaddr_t *)&sockTo, sizeof(sockTo), intIfNum,
   &bytesSent, 0, pSendPkt->ttl, 0,
   L7_FALSE, L7_TRUE, L7_FALSE, pSendPkt->rtrAlert);
  if(rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_MCAST_MAP_COMPONENT_ID, "Mcast IPv6 Control Packet Send Failed; "
			 "rtrIfNum - %d, DestIP - %s, sockFD - %d, Length - %d, Flags - 0x%x. IPv6 Multicast Control Packet"
			 " Transmission Failed. This is because of the Socket sendto() failure. This message can be seen when "
			 "a load of control packets are sent at once and it results in the non-availability of socket buffers.",
             intIfNum, inetAddrPrint(&pSendPkt->destAddr,addrBuffStr),
             pSendPkt->sockFd, pSendPkt->length, pSendPkt->flags);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
      "osapiPktInfoSend() failed intIfNum = %d", intIfNum);
    return L7_FAILURE;
  }

  return L7_SUCCESS; 

}

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
L7_RC_t mcastMapV6Register(L7_uint32 routerProtocol_ID, L7_char8 *name, 
                           L7_RC_t (*notify)(L7_uint32 intIfNum, 
                           L7_uint32 event, void *pData, 
                           ASYNC_EVENT_NOTIFY_INFO_t *response))
{

  /* register MCASTMAP to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router
   * interface changes
   */
  if (ip6MapRegisterRoutingEventChange(routerProtocol_ID, name,
                                       notify) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "MCAST MAP-v6:Error registering with ip6map for \
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
L7_RC_t mcastMapV6Deregister(L7_uint32 routerProtocol_ID)
{
  if (ip6MapDeregisterRoutingEventChange(routerProtocol_ID) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "MCAST MAP-v6:Error Deregistering with ip6map \
            for intf state change callback");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

