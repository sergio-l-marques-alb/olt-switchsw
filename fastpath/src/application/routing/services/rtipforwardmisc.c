/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rtproto.c
*
* @purpose The purpose of this file is to forward IP traffic.
*          This code rplaces VxWorks IP forwarding function.
*
* @component VxWorks IPM
*
* @comments none
*
* @create 03/19/2001
*
* @author alt
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#define	_IP_VHL


#include "log.h"
#include "l7_common.h"
#include "l3end_api.h"
#include "dtlapi.h"
#include "l7_ipmap_arp_api.h"
#include "rto_api.h"
#include "sysapi.h"
#include "rtmbuf.h"
#include "rtip_var.h"    
#include "rtip.h"        
#include "osapi.h"
#include "rtiprecv.h"
#include "buff_api.h"
#include "l7_ip_api.h"


#define RTPROTO_DEBUG   0     /* change this definition to 1 for debugging */


/*********************************************************************
*
* @purpose  Create a copy of the original IP packet for adding it to 
*           the ICMP Error Packet.
*
* @param    *m      Pointer to a rtmbuf structure containg the original 
*                   IP Packet.
* @param    *mcopy  Pointer to a rtmbuf structure where the copy of the 
*                   original IP Packet is to be made.
*
* @notes    none 
*
* @end
*********************************************************************/
void rtcreateIcmpErrPckt(struct rtmbuf *m, struct rtmbuf *mcopy) 
{
  int header_len, pkt_len;
  struct ip *ip = rtmtod(m, struct ip *);
  L7_uchar8 *dataStart;

  mcopy->rtm_type = MT_DATA;
  mcopy->rtm_data = mcopy->rtm_pktdat;           
  mcopy->rtm_flags = m->rtm_flags ;     
  mcopy->rtm_pkthdr.csum_flags = m->rtm_pkthdr.csum_flags;
  mcopy->rtm_pkthdr.csum_data =  m->rtm_pkthdr.csum_data;
  /*MGET(mcopy, M_DONTWAIT, m->m_type);
  if (mcopy != NULL) {
      M_COPY_PKTHDR(mcopy, m);  */
  header_len = (IP_VHL_HL(ip->ip_vhl) << 2) + 8 ;
  pkt_len = (int)osapiNtohs(ip->ip_len);
  mcopy->rtm_len = mcopy->rtm_pkthdr.len = rtmin(header_len,pkt_len);
  /*rtm_copydata(m, 0, mcopy->rtm_len, rtmtod(mcopy, caddr_t));*/
  memcpy(mcopy->rtm_data, m->rtm_data, (unsigned int)mcopy->rtm_len);

  /* Save the ethernet header so that we can use it search for a VRRP IP address */
  SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)m->rtm_bufhandle, dataStart);
  if(dataStart != NULL)
     memcpy(mcopy->m_dmac, dataStart, L7_MAC_ADDR_LEN);
  else
     memset(mcopy->m_dmac, 0, L7_MAC_ADDR_LEN);
}

/*********************************************************************
*
* @purpose  Interface to call the system allocate routine for an rtmbuf 
*           structure.
*
* @param    waitFlag  Indicates whether to wait for a free buffer or not
*                     (M_WAIT, M_DONTWAIT).
* @param    mbufType  Indicates type of mbuf being allocated.
*
* @returns  *m      Pointer to allocated rtmbuf structure, or 0 if error.
*
* @notes    The M_WAIT flag option is not supported.
*
* @end
*********************************************************************/
struct rtmbuf *rtm_getm(short waitFlag, short mbufType)
{
  struct rtmbuf *m = 0;
  L7_uint32 buffer_pool_id = rtMemPoolIdGet ();
  L7_uchar8 *buffer_addr;

  /* NOTE: The waitFlag parameter value M_WAIT is not supported.  
   *       All allocations are treated as M_DONTWAIT.
   */

  if (bufferPoolAllocate (buffer_pool_id, &buffer_addr) == L7_SUCCESS)
  {
    m = (struct rtmbuf *)buffer_addr;
    memset (m, 0, sizeof(struct rtmbuf));
    m->rtm_type = mbufType;
  }
  return m;
}

/*********************************************************************
*
* @purpose  Interface to call the system free routine for a rtmbuf 
*           structure , that had been allocated by system malloc.
*
* @param    *m      Pointer to a rtmbuf structure which is to be freed.
*
* @notes    none 
*
* @end
*********************************************************************/
void rtm_freem(struct rtmbuf *m)
{
  L7_uint32 buffer_pool_id = rtMemPoolIdGet ();

  bufferPoolFree (buffer_pool_id, (L7_uchar8 *)m);
}

/*********************************************************************
*
* @purpose  Create a copy of the rtmbuf structure
*
* @param    *m      Pointer to a rtmbuf structure containg the original 
*                   IP Packet.
* @param    *mcopy  Pointer to a rtmbuf structure where the copy 
*                   is to be made.
*
* @notes    none 
*
* @end
*********************************************************************/
void rtcopyMbuf(struct rtmbuf *m, struct rtmbuf *mcopy) 
{

  /*memset(mcopy, 0, sizeof( *mcopy));*/  /* commented because mcopy may lose pointers if set to zero */ 
  mcopy->rtm_type = MT_DATA;
  mcopy->rtm_flags = m->rtm_flags;
  mcopy->rtm_data = m->rtm_data;           
  /*mcopy->rtm_pkthdr.csum_flags = m->rtm_pkthdr.csum_flags;
  mcopy->rtm_pkthdr.csum_data =  m->rtm_pkthdr.csum_data;*/
  mcopy->rtm_len = mcopy->rtm_pkthdr.len = m->rtm_len;
  mcopy->rtm_bufhandle = m->rtm_bufhandle;
}


/*********************************************************************
*
* @purpose  Find a route from the routing table to given destination.
*
* @param    outgoingIntf @b{(output)} internal interface number of 
*                                      next hop interface.
* @param    nextHopIP @b{(output)} next hop IP address
*
* @param    dstAddr  @b{(input)} destination IP address.
*
* @param    srcAddr @b{(input)} source IP address.
*
* @returns  IP_NONLOCAL_ROUTE  If the destination IP is not attached
* @returns  IP_LOCAL_ROUTE     If the destination IP is attached to a local subnet
*
* @notes    If there are multiple next hops to the destination, selects one. 
*           The source IP address may be considered in selecting a next hop.
*
* @end
*********************************************************************/
L7_uint32 rtFindRoute(L7_uint32 *outgoingIntf, L7_uint32 *nextHopIP, 
                      L7_uint32 dstAddr, L7_uint32 srcAddr)
{
  L7_uint32 matchFlag = IP_NONLOCAL_ROUTE;
  L7_routeEntry_t bestRoute;
  
    if (rtoBestRouteLookup(dstAddr, &bestRoute, L7_FALSE) != L7_SUCCESS)
    {
      *nextHopIP = L7_NULL_IP_ADDR;
      return matchFlag;
    }
    if (bestRoute.protocol == RTO_LOCAL)
    {
        matchFlag = IP_LOCAL_ROUTE;
    }

    rtSelectNextHop(&bestRoute, dstAddr, srcAddr, outgoingIntf, nextHopIP);

  return matchFlag;
}

/*********************************************************************
*
* @purpose  Given a route, select one next hop from among all the next
*           hops in the route.
*
* @param    bestRoute  @b{(input)} Best route to dstAddr
* @param    dstAddr  @b{(input)} destination IP address.
* @param    srcAddr  @b{(input)} source IP address.
* @param    outgoingIntf @b{(output)} internal interface number of 
*                                      next hop interface.
* @param    nextHopIP @b{(output)} next hop IP address
*
* @returns  L7_SUCCESS if a next hop is selected
* @returns  L7_FAILURE otherwise
*
* @notes    If there are multiple next hops to the destination, selects one. 
*           Attempt to keep packets from the same flow on the same next
*           hop to avoid getting packets in the flow out of order at the
*           destination. Attempt to use a next hop which is resolved to
*           a MAC address. If next hop is not resolved, try to resolve
*           all next hops and select the last one that is resolved.
*
* @end
*********************************************************************/
L7_RC_t rtSelectNextHop(L7_routeEntry_t *bestRoute, L7_uint32 dstAddr,
                        L7_uint32 srcAddr, L7_uint32 *outgoingIntf, 
                        L7_uint32 *nextHopIP)
{
    L7_RC_t rc;
    L7_uchar8 mac[L7_MAC_ADDR_LEN];
    L7_uint32 intf;
    L7_uint32 nextHopAddr;
    L7_uint32 nhIndex;

    L7_uint32 hash = dstAddr ^ srcAddr;
    L7_uint32 hashIndex = hash % bestRoute->ecmpRoutes.numOfRoutes;  

    *outgoingIntf = bestRoute->ecmpRoutes.equalCostPath[hashIndex].arpEntry.intIfNum;
    /* If the best route to the destination is a local route, then 
     * the next hop IP address is the destination itself. */
    if (bestRoute->protocol == RTO_LOCAL)
        *nextHopIP = dstAddr;
    else
        *nextHopIP = bestRoute->ecmpRoutes.equalCostPath[hashIndex].arpEntry.ipAddr;

    rc = ipMapArpAddrResolve (*outgoingIntf, *nextHopIP, mac, L7_NULLPTR, 0, 0);
    if (rc == L7_SUCCESS)
    {
        /* Next hop already resolved. Use it. */
        return L7_SUCCESS;
    }

    /* The next hop we hashed to is not resolved. Trigger ARP resolution for 
     * all next hops. */
    for (nhIndex = 0; nhIndex < bestRoute->ecmpRoutes.numOfRoutes; nhIndex++)
    {
        if (nhIndex == hashIndex)
            continue;   /* already tried this one */

        intf = bestRoute->ecmpRoutes.equalCostPath[nhIndex].arpEntry.intIfNum;
        if (bestRoute->protocol == RTO_LOCAL)
            nextHopAddr = dstAddr;
        else
            nextHopAddr = bestRoute->ecmpRoutes.equalCostPath[nhIndex].arpEntry.ipAddr;

        rc = ipMapArpAddrResolve (intf, nextHopAddr, mac, L7_NULLPTR, 0, 0);
        if (rc == L7_SUCCESS)
        {
            /* Next hop already resolved. Use it. */
            *outgoingIntf = intf;
            *nextHopIP = nextHopAddr;
            /* continue to trigger ARP resolution for remaining next hops */
        }
    }
    /* If no next hops resolved, then outgoingIntf and nextHopIP are left as
     * the next hop we hashed to. That's as good a choice as any. Continue in
     * the hope that the next hop router responds to our ARP request. ARP 
     * resolution will be checked when we try to actually transmit. */
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Find a route from the routing table to given destination
*           for a given TOS value.
*
* @param    *destIntf            Pointer for storing the internal interface
*                                number associated with the next hop IP.
* @param    *routerIP            Pointer for storing the next hop IP.
*
* @param    *netMask             Pointer for storing the net mask of 
*                                the gateway.
*
* @param    dstAddr              destination IP Address.
*
* @param    srcAddr              source IP address
*
* @param    tosvalue             TOS value for which the route is being 
*                                searched .
*
* @returns  L7_TRUE              If the destination IP is attached to 
*                                one of our subnet
*
* @returns  L7_FALSE             otherwise

* @notes    TOS routing is not supported. This calls the normal 
*           route lookup.
*
* @end
*********************************************************************/
L7_uint32 rtFindRouteWithTOS(L7_uint32 *destIntf, L7_uint32 *routerIP, 
                             L7_uint32 dstAddr, L7_uint32 srcAddr,
                             L7_uint32 tosvalue)
{
    return rtFindRoute(destIntf, routerIP, dstAddr, srcAddr);
}

/*********************************************************************
*
* @purpose  Check if the destination IP Address matches any of our 
*           Routing Interfaces.
*
* @param    *MyLocalIfIndex      Pointer for storing the internal interface
*                                number if the destination IP Address 
*                                matches the IP address of the interface.
*
* @param    dstAddr              destination IP Address.
*
* @notes    MyLocalIfIndex should be set to 0 before this function is called. 
*
* @end
*********************************************************************/
void rtCheckOurIntf(L7_uint32 *MyLocalIfIndex, L7_uint32 dstAddr)
{
  ipMapIpAddressToIntf(dstAddr, MyLocalIfIndex);
}

/*********************************************************************
*
* @purpose  Interface to call the API for retrieving the IP address of 
*           an interface.
*
* @param    intIfNum   internal interface number
*
* @returns  IP address of the interface
*
* @notes    none 
*
* @end
*********************************************************************/
L7_uint32 rtGetIPAddrOfIntf(L7_uint32 intIfNum)
{
  L7_uint32 ipAddr = 0;
  L7_uint32 netMask = 0;
  L7_uint32 numberedIfc;
  L7_BOOL   isUnnumbered = L7_FALSE;

  /* If unnumbered ifc, return IP address on corresponding numbered ifc */
  ipMapUnnumberedGet(intIfNum, &isUnnumbered, &numberedIfc);
  if ( isUnnumbered == L7_TRUE)
  {
    intIfNum = numberedIfc;
  }

  if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &netMask) == L7_SUCCESS)
    return ipAddr;
  return 0;
}

/*********************************************************************
*
* @purpose  Interface to call the API for retrieving the Subnet Mask of 
*           an interface.
*
* @param    intIfNum   internal interface number
*
* @returns  Subnet Mask of the interface
*
* @notes    none 
*
* @end
*********************************************************************/
L7_uint32 rtGetSubnetMaskOfIntf(L7_uint32 intIfNum)
{
  L7_uint32 ipAddr = 0;
  L7_uint32 netMask = 0;
  if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &netMask) == L7_SUCCESS)
    return netMask;
  return 0;
}

/*********************************************************************
*
* @purpose  Interface to call the API for retrieving the Net-Directed 
*           Broadcast address of an interface.
*
* @param    intIfNum   internal interface number
* @param    pBcastAddr net-directed broadcast address output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    assumes we want the network directed broadcast address for
*           the primary IP address 
*
* @end
*********************************************************************/
L7_RC_t rtGetNetDirBcastAddrOfIntf(L7_uint32 intIfNum, L7_uint32 *pBcastAddr)
{
  L7_uint32 netDirBcastAddr;
  L7_RC_t   rc = L7_FAILURE;

  rc = ipMapRouterIfIPBroadcastGet(intIfNum, &netDirBcastAddr);
  if (rc != L7_SUCCESS)
    netDirBcastAddr = L7_NULL_IP_ADDR;

  *pBcastAddr = netDirBcastAddr;
  return rc;
}

/*********************************************************************
*
* @purpose  Interface to call the API for retrieving the Status of 
*           an interface.
*
* @param    intIfNum   internal interface number
*
* @returns  Status of the interface
*
* @notes    none 
*
* @end
*********************************************************************/
L7_uint32 rtGetUpStatusOfIntf(L7_uint32 intIfNum)
{
  L7_BOOL isUp;

  if(osapiRouterIfStatusGet(intIfNum,&isUp) != L7_SUCCESS)
     return L7_FAILURE;
  return (isUp == L7_TRUE)? L7_SUCCESS:L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Interface to call the API for retrieving the maximum no.   
*           of interfaces.
*
* @returns  Maximum number of interfaces
*
* @notes    none 
*
* @end
*********************************************************************/
L7_uint32 rtGetMaxCountOfIntf()
{
  return ipmRouterIfMaxCountGet();
}
