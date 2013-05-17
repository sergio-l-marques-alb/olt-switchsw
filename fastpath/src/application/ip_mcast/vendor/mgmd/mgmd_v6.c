/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_v6.c
*
* @purpose   To support MLD (v1 + v2) specific functions 
*
* @component MGMD
*
* @comments  none
*
* @create    18/10/2007
*
* @author    ddevi
* @end
*
**********************************************************************/

/**********************************************************************
                  Includes
***********************************************************************/
#include <l7_socket.h>
#include "l3_mcast_defaultconfig.h"
#include "heap_api.h"
#include "mcast_api.h"
#include "l7_mgmdmap_include.h"
#include "mgmd.h"
#include "mgmd_proxy.h"
#include "snooping_exports.h"


/* Number of elements in MGMD IPV6 heap pool list, including dummy end-of-list element */
#define MGMD_IPV6_POOL_LIST_SIZE  9
heapBuffPool_t mgmdV6HeapPoolList[MGMD_IPV6_POOL_LIST_SIZE]; 

/* Number of elements in MGMD proxy IPv6 heap pool list, including dummy end-of-list element */
#define MGMD_PROXY_IPV6_POOL_LIST_SIZE  8
heapBuffPool_t mgmdProxyV6HeapPoolList[MGMD_PROXY_IPV6_POOL_LIST_SIZE];



/*********************************************************************
*
* @purpose  Build the MGMD IPv6 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment MGMD_IPV6_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
heapBuffPool_t *mgmdV6HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(mgmdV6HeapPoolList, 0, sizeof(heapBuffPool_t) * MGMD_IPV6_POOL_LIST_SIZE);

  /* MGMD Protocol Control Block Buffer */
  mgmdV6HeapPoolList[i].buffSize = sizeof (mgmd_cb_t);
  mgmdV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "MGMD_CB", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for MGMD Group Membership Database */
  mgmdV6HeapPoolList[i].buffSize = MGMD_MAX_GROUPS * sizeof (avlTreeTables_t);
  mgmdV6HeapPoolList[i].buffCount = 3;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "MGMD_GROUP_TREE_HEAP", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for MGMD Group Membership Database */
  mgmdV6HeapPoolList[i].buffSize = MGMD_MAX_GROUPS * sizeof (mgmd_group_t);
  mgmdV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "MGMD_GROUP_DATA_HEAP", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Group Membership Source Record Buffers */
  mgmdV6HeapPoolList[i].buffSize = sizeof (mgmd_source_record_t);
  mgmdV6HeapPoolList[i].buffCount = MGMD_MAX_SRC_RECORDS;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "MGMD_SOURCE_RECORD_BUFFERS", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Query Request Node Buffers */
  mgmdV6HeapPoolList[i].buffSize = sizeof(mgmd_query_req_t);
  mgmdV6HeapPoolList[i].buffCount = MGMD_MAX_QUERY_REQ_NODES;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "MGMD_QUERY_REQ_NODES_BUFFER", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Timer Buffers */
  mgmdV6HeapPoolList[i].buffSize = sizeof (mgmd_timer_event_info_t);
  mgmdV6HeapPoolList[i].buffCount = MGMD_MAX_TIMER_NODES;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "MGMD_TIMER_NODES_BUFFER", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Handle List Buffers */
  mgmdV6HeapPoolList[i].buffSize = (sizeof(handle_member_t) * MGMD_MAX_TIMERS);
  mgmdV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "MGMD_HANDLE_NODES_BUFFER", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Group Membership Source Address Buffers */
  mgmdV6HeapPoolList[i].buffSize = sizeof (L7_inet_addr_t);
  mgmdV6HeapPoolList[i].buffCount = MGMD_MAX_SRC_RECORDS;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "MGMD_SOURCE_ADDR_BUFFERS", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  mgmdV6HeapPoolList[i].buffSize = 0;
  mgmdV6HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(mgmdV6HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  mgmdV6HeapPoolList[i].flags = 0;
  i++;

  if (i != MGMD_IPV6_POOL_LIST_SIZE)
  {
    LOG_ERROR(i);
  }
  return mgmdV6HeapPoolList;
}

/*********************************************************************
*
* @purpose  Build the MGMD Proxy IPv6 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment MGMD_PROXY_IPV6_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
heapBuffPool_t *mgmdProxyV6HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(mgmdProxyV6HeapPoolList, 0, sizeof(heapBuffPool_t) * MGMD_PROXY_IPV6_POOL_LIST_SIZE);

  /* MGMD-Proxy Host Information Control Block */
  mgmdProxyV6HeapPoolList[i].buffSize = sizeof (mgmd_host_info_t);
  mgmdProxyV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "MGMD_PROXY_CB", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for MGMD-Proxy Group Membership Database Database*/
  mgmdProxyV6HeapPoolList[i].buffSize = sizeof (avlTreeTables_t) * MGMD_HOST_MAX_GROUPS;
  mgmdProxyV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "MGMD_PROXY_GRP_TREE_HEAP", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for MGMD-Proxy Group Membership Database */
  mgmdProxyV6HeapPoolList[i].buffSize = MGMD_HOST_MAX_GROUPS * sizeof (mgmd_host_group_t);
  mgmdProxyV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "MGMD_PROXY_GROUP_DATA_HEAP", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* MGMD-Proxy Host Source Record Buffers */
  mgmdProxyV6HeapPoolList[i].buffSize = sizeof (mgmd_host_source_record_t);
  mgmdProxyV6HeapPoolList[i].buffCount = MGMD_HOST_MAX_SRC_RECORDS;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "MGMD_PROXY_SOURCE_RECORD_BUFFERS", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* MGMD-Proxy Unsolicited Reports Buffer */
  mgmdProxyV6HeapPoolList[i].buffSize = sizeof (mgmd_proxy_unsolicited_rpt_t);
  mgmdProxyV6HeapPoolList[i].buffCount = MGMD_MAX_UNSOLICITED_REPORTS;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "MGMD_PROXY_REPORTS_BUFFER", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for MGMD-Proxy MRT (S,G) Database*/
  mgmdProxyV6HeapPoolList[i].buffSize = sizeof (avlTreeTables_t) * platIpv6McastRoutesMaxGet();
  mgmdProxyV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "MGMD_PROXY_MRT_TREE_HEAP", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for MGMD-Proxy MRT (S,G) Database */
  mgmdProxyV6HeapPoolList[i].buffSize = platIpv6McastRoutesMaxGet() * sizeof (mgmdProxyCacheEntry_t);
  mgmdProxyV6HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "MGMD_PROXY_MRT_DATA_HEAP", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  mgmdProxyV6HeapPoolList[i].buffSize = 0;
  mgmdProxyV6HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(mgmdProxyV6HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  mgmdProxyV6HeapPoolList[i].flags = 0;
  i++;

  if (i != MGMD_PROXY_IPV6_POOL_LIST_SIZE)
  {
    LOG_ERROR(i);
  }
  return mgmdProxyV6HeapPoolList;
}

/*********************************************************************
*
* @purpose  Initialize the MLD socket Fd ( ICMPV6)
*
* @param    mgmdCB    @b{ (input) } MGMD Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmd_init6_sockFd (mgmd_cb_t *mgmdCB)
{
  L7_uint32 sockOpt;
  L7_uint32 sockFD = MGMD_FAILURE;

  /* Open the socket for communication */
  if (osapiSocketCreate(L7_AF_INET6, L7_SOCK_RAW, IPPROTO_ICMPV6, &sockFD) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Socket Open Failed \n");
  }

  sockOpt = 1;  /* # define this value */
  if (osapiSetsockopt(sockFD, IPPROTO_IPV6, L7_IPV6_MULTICAST_HOPS,
                      (L7_uchar8 *) &sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Multicast Hops Socket Option Failed \n");
    osapiSocketClose(sockFD);
    return(L7_FAILURE);
  }

  sockOpt = L7_TRUE;
  if (osapiSetsockopt(sockFD, IPPROTO_IPV6,  L7_IPV6_RECVPKTINFO, ( L7_uchar8 *)&sockOpt, sizeof(sockOpt)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:L7_IPV6_RECVPKTINFO Option Failed \n");
    osapiSocketClose(sockFD);
    return(L7_FAILURE);
  }

  sockOpt = L7_TRUE;
  if (osapiSetsockopt(sockFD, IPPROTO_IPV6,  L7_IPV6_RECVHOPLIMIT, ( L7_uchar8 *)&sockOpt, sizeof(sockOpt)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "\n MGMD:L7_IPV6_RECVHOPLIMIT Option Failed \n");
  }

  /* When receiving with osapiPktInfoRecv(), tell OS to have the hop-by-hop
     options delivered in the control message, so we can verify the packet 
     has router-alert set. */
  sockOpt= L7_TRUE;
  if (osapiSetsockopt(sockFD, IPPROTO_IPV6,L7_IPV6_RECVHOPOPTS, 
                      (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD: IPV6_RECVHOPOPTS socket option Failed \n");
  }

  sockOpt = L7_FALSE;
  if (osapiSetsockopt(sockFD, IPPROTO_IPV6, L7_IPV6_MULTICAST_LOOP,
                      (L7_uchar8 *) & sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:L7_IPV6_MULTICAST_LOOP socket option failed\n");
    osapiSocketClose(sockFD);
    return L7_FAILURE;
  }

  sockOpt = L7_TRUE;
  if (osapiSetsockopt(sockFD, IPPROTO_IPV6, L7_IPV6_ROUTER_ALERT,
                      (L7_uchar8 *) & sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:L7_IPV6_ROUTER_ALERT socket option failed\n");
  }

  /* Memory needed to send a flood of reports in the worst-case */  
  /* /2 because kernel gives us twice the space we ask for */
  sockOpt = (28+16*(1+MGMD_MAX_QUERY_SOURCES))*MGMD_MAX_GROUPS/2;
  sockOpt += 32768; /* For breathing room */
  if (osapiSetsockopt(sockFD, L7_SOL_SOCKET, L7_SO_SNDBUF,
                      (L7_uchar8 *) & sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:L7_SO_SNDBUF socket option failed\n");
    osapiSocketClose(sockFD);
    return L7_FAILURE;
  }

  /* Register the created socket with the packet receiver for Rx waiting */
  if (mcastMapPktRcvrSocketFdRegister (sockFD, MCAST_EVENT_MLD_CTRL_PKT_RECV, 
                                       mcastCtrlPktBufferPoolIdGet(L7_AF_INET6),
                                       L7_IPV6_PKT_RCVR_ID_MGMD) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Register with Packet Receiver Failed \n");
    osapiSocketClose(sockFD);
    return L7_FAILURE;
  }
  mgmdCB->sockfd = sockFD;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  De inialize the MLD socket Fd ( ICMPV6)
*
* @param    mgmdCB    @b{ (input) } MGMD Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmd_deinit6_sockFD (mgmd_cb_t *mgmdCB)
{
  if (mcastMapPktRcvrSocketFdDeRegister (L7_IPV6_PKT_RCVR_ID_MGMD) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Socket FD De-Regis'tion Failure");
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enables/Disables the MLD-V2 IPv6 Group address and interface ID with 
*           interpeak stack.
*  
* @param    rtrIfNum   @b{(input)}interface number
* @param    addFlag    @b{(input)}Indicates whether (group, Ifidx) needs to enable or 
*                      disable.
* @param    sockFd     @b{(input)}Socket Desc.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  on failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t mgmdV6SockChangeMembership(L7_uint32 rtrIfNum, L7_uint32 intfIfNum,
                              L7_uint32 addFlag, L7_uint32 sockfd)
{
  struct L7_ip_mreq6_s mreq6;
  L7_RC_t retVal = L7_SUCCESS;
  L7_int32 sockopt;
  L7_inet_addr_t addr;

  inetIgmpv3RouterAddressInit(L7_AF_INET6,&addr); 
  memset(&mreq6,0,sizeof(struct L7_ip_mreq6_s));
  memcpy(&mreq6.imr6_multiaddr, &addr.addr.ipv6, L7_IP6_ADDR_LEN);

  mreq6.imr6_intIfNum = intfIfNum;
  if (L7_ENABLE == addFlag)
  {
    sockopt = L7_IPV6_ADD_MEMBERSHIP;
  }
  else
  {
    sockopt = L7_IPV6_DROP_MEMBERSHIP;
  }
  if (L7_FAILURE == (osapiSetsockopt(sockfd, IPPROTO_IPV6, sockopt,
      (L7_uchar8 *) & mreq6, sizeof(struct L7_ip_mreq6_s))))
  {
      retVal = L7_FAILURE;
  }
  return retVal;
}


/*********************************************************************
* @purpose  Receive IPV6 MLD packet receive
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  on failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmd_ipv6_pkt_receive(mcastControlPkt_t *message)
{
  ipv6pkt_t *ipv6pkt;
  mgmd_cb_t *mgmdCB = L7_NULLPTR; 

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  if (mgmdMapProtocolCtrlBlockGet(message->family, 
                                  (MCAST_CB_HNDL_t *)&mgmdCB) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Failed to access control block\n");
    return L7_FAILURE;
  }

  MGMD_DEBUG(MGMD_DEBUG_RX, "Recd. an ICMP V6 packet on iface = %d ", message->rtrIfNum);

  ipv6pkt = (ipv6pkt_t *)(mgmdCB->pktRxBuf);
  
  inetAddressGet(L7_AF_INET6, &(message->srcAddr), &(ipv6pkt->srcAddr));
  inetAddressGet(L7_AF_INET6, &(message->destAddr), &(ipv6pkt->dstAddr));
  ipv6pkt->dataLen = message->length;
  ipv6pkt->zero[0] = 0;
  ipv6pkt->zero[1] = 0;
  ipv6pkt->zero[2] = 0;
  ipv6pkt->nxtHdr = 58;

  if (L7_IP6_IS_ADDR_LINK_LOCAL(&(ipv6pkt->srcAddr)) != L7_TRUE)
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_RX, "Rx'ed with non-link-local source address = ",
                    &message->srcAddr);
    return L7_FAILURE;
  }

  MGMD_DEBUG(MGMD_DEBUG_RX, "Rx'ed with Hoplimit = %d ",
                    message->numHops);

  if (inetIsDirectlyConnected(&message->srcAddr, message->rtrIfNum) != L7_TRUE)
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_RX, "Rx'ed with non-local source address = ",
                    &message->srcAddr);
    return L7_FAILURE;
  }
  if (message->numHops != MGMD_ONE)
  {
    MGMD_DEBUG(MGMD_DEBUG_RX, "Rx'ed with hoplimit greater than 1, hopLimit= %d",
                    message->numHops);
    return L7_FAILURE;
  }
  
  memcpy ((ipv6pkt->icmpv6pkt), (L7_uchar8 *)(message->payLoad), message->length);

  if (inetChecksum (ipv6pkt, (L7_IP6_HEADER_LEN + message->length))!=0)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:IP V6 cksum validation failed\r\n");
    if (message->payLoad)
    {
      /*
         Get Buffer Pool ID and then FREE the Buffer POOL
         bufferPoolFree(mcastGblVariables.mcastPktPoolId,
         message->mcastPktBuffer);
       */
    }
    mgmdCB->counters.BadChecksumPackets++;                           
    mgmdCB->counters.MalformedPackets++;                             
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Process a MLD (v6) packet
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL mgmd_ipv6_is_scope_id_valid(L7_inet_addr_t *pGroup)
{
   L7_int32           scopeId;
     
  /* MLD messages are never sent for multicast addresses whose scope is 0
   (reserved) or 1 (node-local). */
  scopeId = inetAddrGetMulticastScopeId(pGroup);
  
  MGMD_DEBUG(MGMD_DEBUG_REPORTS,"MLD packet scope Id = %d ",scopeId);
  if ((scopeId == MLD_RESERVED_SCOPE) || 
      (scopeId == MLD_INTERFACE_LOCAL_SCOPE))
  {
    MGMD_DEBUG_ADDR(MGMD_DEBUG_FAILURE,"MLD packet received for reserved/node-local grp addr = ",
                pGroup);
    MGMD_DEBUG(MGMD_DEBUG_FAILURE, "Scope-Id =%d \n", scopeId);
    return L7_FALSE;
  }
  return L7_TRUE;

}
/*********************************************************************
* @purpose  Process a MLD (v6) packet
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mgmd_process_v6_packet(mgmd_cb_t *mgmdCB, mcastControlPkt_t * message,
                                   L7_BOOL proxyInterface)
{
  L7_uint32          type, maxRespCode = 0, maxRespTime = 0, code;
  L7_inet_addr_t     source, group;
  L7_ushort16        checksum;
  L7_in6_addr_t      mcastAddr;
  L7_mgmdv3Report_t  *v3Report = L7_NULLPTR;
  L7_mgmdv3Query_t   v3Query;
  L7_uint32          mgmdVersion, rtrIfNum = message->rtrIfNum;
  mgmd_info_t       *mgmd_info;
  L7_int32           mgmdHostVersion = 0;
  L7_uchar8         *payload = message->payLoad;

  MGMD_DEBUG (MGMD_DEBUG_APIS, " Entered");

  v3Report = &mgmdCB->v3Report;
  /* TODO: Findout the right place to init the following*/
  /*mgmd_v3_report_init (mgmdCB, v3Report);
  mgmd_v3_query_init (mgmdCB, &v3Query);*/

  if (inetCopy(&source, &(message->srcAddr)) != L7_SUCCESS)
  {
    MGMD_DEBUG(MGMD_DEBUG_FAILURE, "Copy operation Failed \n");
    return;
  }
  inetAddressZeroSet (mgmdCB->proto, &group);

  MGMD_DEBUG_ADDR (MGMD_DEBUG_RX, "Packet from ", &source);

  /* Validate total length value */
  if (message->length < MLD_PKT_MIN_LENGTH)
    return;

  if ((cnfgrIsFeaturePresent(L7_FLEX_MGMD_MAP_COMPONENT_ID, 
                             L7_MGMD_FEATURE_CHECK_RTR_ALERT_OPTION) == L7_FALSE))
  {
    /* When the platform does not support ROUTER_ALERT option at L2, MLD router also relaxes its check for router alert (deviation from RFCs (2710 and 3810) and registers the group even if the join does not have Router-alert option.*/
  }
  else
  {
    if ((mgmdCB->checkRtrAlert == L7_TRUE) &&
        !(message->ipRtrAlert))
    {
      MGMD_DEBUG (MGMD_DEBUG_RX, "Recd without router alert type %d on %d len"
                  " %d, ignore!\n", message->ipRtrAlert, rtrIfNum, message->length);
      return;
    }
  }


  MCAST_GET_BYTE(type, payload);
  v3Report->mgmdReportType = type;
  v3Query.mgmdMsg.mgmdType = type;

  if (type < MLD_LISTENER_QUERY || type > MLD_V2_LISTENER_REPORT)
  {
    MGMD_DEBUG (MGMD_DEBUG_RX, "Recd unsupported type %d on %d len %d,"
                " ignore!\n", type, rtrIfNum, message->length);
    return;
  }

  MCAST_GET_BYTE(code, payload);
  v3Report->mgmdReportReserved1 = code;
  v3Query.mgmdMsg.igmpMaxRespTime = code;

  /* Convert the igmp-Header fields from Network to Host Byte Order */
  MCAST_GET_SHORT (checksum, payload);
  checksum = osapiNtohs (checksum);
  v3Report->mgmdReportChecksum = checksum;
  v3Query.mgmdMsg.mgmdChecksum = checksum;

  mgmdCB->counters.TotalPacketsReceived++;

  if (type == MLD_V2_LISTENER_REPORT)
  {
    if (inetIsAddressIgmpv3Routers(&message->destAddr) != L7_TRUE)
    {
      MGMD_DEBUG (MGMD_DEBUG_RX, "Recd v3 report with dest'n not equal to all v3 reports");
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, " MGMD:Recd v3 report with dest'n not equal to all v3 reports");
      return;
    }
    MCAST_GET_SHORT(v3Report->mgmdReportReserved2, payload);
    MCAST_GET_SHORT(v3Report->mgmdReportNumGroupRecs, payload);
  }
  else
  {
    MCAST_GET_SHORT(maxRespCode, payload);
    /* As the packet has the max-respons-time in milli-secs, convert it to seconds 
       for further processing */
  
    maxRespTime = mgmd_decode_max_resp_code(mgmdCB->proto,maxRespCode);
    v3Query.mgmdMsg.mldMaxRespTime = maxRespTime;
    MCAST_GET_SHORT(v3Query.mgmdMsg.mgmdReserved, payload);

    MCAST_GET_ADDR6(&mcastAddr, payload);
    inetAddressSet(mgmdCB->proto, &mcastAddr, &group);

    MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,"MLD packet received for grp addr = ",
                &group);

    /* Ignore MLD packets for non-multicast groups */
    if (!((type == MLD_LISTENER_QUERY) && (inetIsAddressZero(&group) == L7_TRUE)))
    {
      if (inetIsInMulticast(&group) != L7_TRUE)
      {
        MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,"MLD packet received for num-multicastaddr = ",
                        &group);
        return;
      }
    }
    if (mgmd_ipv6_is_scope_id_valid(&group) ==L7_FALSE )
    {
      MGMD_DEBUG_ADDR(MGMD_DEBUG_REPORTS,
        "Discarding MLD packet rx'ed for invalid scope grp addr = ",
                  &group);
      return;
    }

    if (inetIsMulticastReservedSiteLocalAddress (&group) == L7_TRUE)
    {
      MGMD_DEBUG_ADDR (MGMD_DEBUG_REPORTS, "Discarding MLD Packet received "
                       "with a Reserved Site Local Group Address - ", &group);
      return;
    }
  }

  mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
  if (proxyInterface == L7_TRUE)
  {
    mgmdHostVersion = mgmdCB->mgmd_host_info->configVersion;
    switch (type)
    {
      case MLD_LISTENER_QUERY:   
        /* Determine the query version by checking the length of the query. */
        /* RFC 3376 - Section 7.1 */
        mgmdCB->counters.QueriesReceived++;                    
        if (message->length >= MLD_V2_PKT_MIN_LENGTH)
        {
          if (mgmdHostVersion < L7_MGMD_VERSION_3)
          {
            MGMD_DEBUG (MGMD_DEBUG_RX," Rx'ed Query with higher version for v= %d", mgmdHostVersion);
            return;
          }
          mgmd_v3_query_form(mgmdCB->proto, &v3Query, payload);
          mgmd_proxy_v3membership_query_process(mgmdCB, group, source,
                                                maxRespCode, &v3Query);
        }
        else if (message->length == MLD_PKT_MIN_LENGTH)
        {
          mgmd_proxy_membership_query_process(mgmdCB,group,source,maxRespTime);
        }
        break;

      case MLD_V1_LISTENER_REPORT:
        mgmd_proxy_v1v2report_process(mgmdCB, group, source);
        mgmdCB->counters.ReportsReceived++; 
        break;
    case MLD_V2_LISTENER_REPORT:
        MGMD_DEBUG (MGMD_DEBUG_RX, " V3 report rx'ed");
        /* NOT Processed */
        break;

      case MLD_LISTENER_DONE:
        /* Ignore Leave messages when configured for v1 */
        /* Update the stats */ 
        mgmdCB->counters.LeavesReceived++;  
        break;
      default:
        mgmdCB->counters.WrongTypePackets++;
        break;
    }  /*End-of-Switch*/
  }
  else
  {
    switch (type)
    {
      case MLD_LISTENER_QUERY:   
        /* Determine the query version by checking the length of the query. */
        /* RFC 3810 - Section 8.2 */
        mgmd_info = &(mgmdCB->mgmd_info[rtrIfNum]);
        mgmdVersion = mgmd_info->intfConfig.version;
 
        if (message->length == MLD_PKT_MIN_LENGTH)
        {
          /* IGMP v2 = MLD v1 */
          if (mgmdVersion != L7_MGMD_VERSION_2)
            mgmdCB->mgmd_info[rtrIfNum].Wrongverqueries++;
        }
        else if (message->length >= MLD_V2_PKT_MIN_LENGTH)
        {
          /* IGMP v3 = MLD v2 */
          if (mgmdVersion != L7_MGMD_VERSION_3)
            mgmdCB->mgmd_info[rtrIfNum].Wrongverqueries++;
        }

        if (message->length >= MLD_V2_PKT_MIN_LENGTH)
        {
          mgmd_v3_query_form(mgmdCB->proto, &v3Query, payload);
          mgmd_v3membership_query (&(mgmdCB->mgmd_info[rtrIfNum]), group, source, maxRespCode, &v3Query);
        }
        else if (message->length == MLD_PKT_MIN_LENGTH)
        {
          mgmd_membership_query (&(mgmdCB->mgmd_info[rtrIfNum]), group, source, maxRespTime);
        }
        mgmdCB->counters.QueriesReceived++;                     
        break;

    case MLD_V1_LISTENER_REPORT:
        mgmd_v2membership_report(&(mgmdCB->mgmd_info[rtrIfNum]), 
                                 group, source);
        mgmdCB->counters.ReportsReceived++;                             
        break;

    case MLD_V2_LISTENER_REPORT:
        if (mgmd_v3_report_form(mgmdCB->proto, v3Report, payload) != L7_SUCCESS)
        {
          MGMD_DEBUG (MGMD_DEBUG_RX," IPV6: Rx'ed invalid srcs in src lists ");
          return;
        }
        mgmd_v3membership_report(&(mgmdCB->mgmd_info[rtrIfNum]), 
                                 v3Report, source);
        mgmdCB->counters.ReportsReceived++;                            
        break;
    case MLD_LISTENER_DONE:
        mgmd_group_leave(&(mgmdCB->mgmd_info[rtrIfNum]),group);
        mgmdCB->counters.LeavesReceived++;                                     
        break;
      default:
        MGMD_DEBUG (MGMD_DEBUG_RX," Unknown MLD packet type = %d ", type);
        mgmdCB->counters.WrongTypePackets++;
        break;
    }/*End-of-Switch*/
  }/*End-of-ELSE-Block*/
}
/*********************************************************************
* @purpose  Enables/Disables the interface Mcast mode in stack
*  
* @param    mode     @b{(input)} L7_ENABLE/ L7_DISABLE
*
* @returns  none
*
* @notes    none
*
* @end
*
*********************************************************************/
void mgmdProxyV6InterfacesMcastModeSet(L7_uint32 mode)
{
  L7_uint32 i;
  for (i= 0 ; i< MCAST_MAX_INTERFACES; i++)
  {
    L7_uint32 intIfNum = 0;
    if (mcastIpMapRtrIntfToIntIfNum(L7_AF_INET6, i, &intIfNum) != L7_SUCCESS)
    {
      continue;
    }
    mgmd_proxy_MfcIntfEventQueue(L7_AF_INET6, i, mode);
  }
}


