/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pimsmwrap.c
*
* @purpose   pimsm wrapper file to handle IPv4 and IPv6.
*
* @component 
*
* @comments  none
*
* @create   04/15/2006
*
* @author   nramu
*
* @end
*
**********************************************************************/

#include "pimsmdefs.h"
#include "pimsmmain.h"
#include "osapi_support.h"
#include "rto_api.h"
#include "rtmbuf.h"
#include "l7_ip_api.h"
#include "l3end_api.h"
#include "l3_mcast_commdefs.h"
#include "l7_mcast_api.h"
#include "buff_api.h"
#include "heap_api.h"
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmtimer.h"
#include "pimsmbsr.h"
#include "pimsmcontrol.h"
#include "pimsmintf.h"
#include "l7apptimer_api.h"
#include "pimsmwrap.h"
#include "pimsmv6util.h"
#include "pimsm_vend_ctrl.h"
#include "mfc_api.h"
/* the single source mask length */
#define PIMSM_SINGLE_SRC_MSKLEN_IPV4            32
#define PIMSM_SINGLE_SRC_MSKLEN_IPV6            128

/* the single group mask length  */
#define PIMSM_SINGLE_GRP_MSKLEN_IPV4            32
#define PIMSM_SINGLE_GRP_MSKLEN_IPV6            128

#define PIMSM_STAR_STAR_RP_MSKLEN_IPV4     4       /* Masklen for 224.0.0.0 
                                                   to encode (*,*,RP) */
#define PIMSM_STAR_STAR_RP_MSKLEN_IPV6     8       /* Masklen for `ff00::/8'
                                                   to encode (*,*,RP) */

/* PIM-SM V4 Heap Pool Items */
heapBuffPool_t pimsmV4HeapPoolList[] ={

  /* PIMDM Protocol Control Block Buffer */
  {sizeof (pimsmCB_t), 1,"PIMSM_CB", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Join/Prune Message Buffers */
  {PIMSM_MAX_JP_MSG_SIZE,PIMSM_MAX_JP_PACKETS,"PIMSM_JPG_MSG_BUFFER", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Join/Prune Address List Buffers for IPv4 */
  {PIM_IPV4_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST * sizeof(pim_encod_src_addr_t), PIMSM_MAX_PROTOCOL_PACKETS,"PIMSM_JP_ADDR_LIST_BUFFER", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Cache Entry Buffers */
  {sizeof(pimsmCache_t), PIMSM_MAX_IPV4_CACHE_ENTRIES,"PIMSM_CACHE_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Tree Heap Buffers for (S,G), (*,G) and (S,G,Rpt) Databases */
  {PIMSM_S_G_IPV4_TBL_SIZE_TOTAL * sizeof (avlTreeTables_t), 3,"PIMSM_SG_TREE_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Data Heap Buffers for (S,G) Database */
  {PIMSM_S_G_IPV4_TBL_SIZE_TOTAL * sizeof (pimsmSGNode_t), 1,"PIMSM_SG_DATA_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Downstream Interface Buffers for (S,G) and (*,G) Databases */
  {sizeof (pimsmSGIEntry_t), ((PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_S_G_IPV4_TBL_SIZE_TOTAL) + (PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL)),"PIMSM_SGI_*GI_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Data Heap Buffers for (S,G,Rpt) Database */
  {(PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL * sizeof (pimsmSGRptNode_t)), 1,"PIMSM_SG_RPT_DATA_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Downstream Interface Buffers for (S,G,Rpt) Database */
  {sizeof (pimsmSGRptIEntry_t), (PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL),"PIMSM_SGRptI_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* AVL Data Heap Buffers for (*,G) Database */
  {PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL * sizeof (pimsmStarGNode_t), 1,"PIMSM_*G_DATA_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* (*,*,RP) Buffers */
  {sizeof (pimsmStarStarRpNode_t), PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL,"PIMSM_**RP_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Downstream Interface Buffers for (*,*,RP) Database */
  {sizeof (pimsmStarStarRpIEntry_t),PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL,"PIMSM_**RPI_BUFFERS", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RP Block Buffer */
  {sizeof (pimsmRpBlock_t), 1,"PIMSM_RP_BLOCK", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RADIX Tree Heap Buffers for RP Set Database */
  {(RADIX_TREE_HEAP_SIZE(PIMSM_RP_GRP_ENTRIES_MAX,sizeof(rpSetTreeKey_t))), 1,"PIMSM_RP_SET_TREE_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RADIX Data Heap Buffers for RP Set Database */
  {PIMSM_RP_GRP_ENTRIES_MAX * sizeof (pimsmRpSetNode_t), 1,"PIMSM_RP_SET_DATA_HEAP", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RP Group List Buffers */
  {sizeof (pimsmRpGrpNode_t), PIMSM_RP_GRP_ENTRIES_MAX,"PIMSM_RP_GRP_LIST", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RP Group Address List Buffers */
  {sizeof (pimsmAddrList_t), PIMSM_RP_GRP_ADDR_LIST_NODES_MAX,"PIMSM_MRT_RP_GRP_ADDR_LIST", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* BSR Block Buffer */
  {sizeof (pimsmBSRBlock_t), 1,"PIMSM_BSR_BLOCK", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* BSR Per Scope Zone Buffer */
  {sizeof (pimsmBsrPerScopeZone_t), PIMSM_BSR_SCOPE_NODES_MAX,"PIMSM_BSR_PER_SCOPE_ZONE", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* BSR Fragment Group-RP Node Buffers */
  {sizeof (pimsmFragGrpRpNode_t), PIMSM_BSR_FRAGMENT_GRP_ENTRIES_MAX,"PIMSM_FRAG_GRP_RP_NODE", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* BSR Fragment RP Node Buffers */
  {sizeof (pimsmFragRpNode_t), PIMSM_BSR_FRAGMENT_RP_ENTRIES_MAX,"PIMSM_FRAG_RP_NODE", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Neighbor Database Buffers */
  {sizeof (pimsmNeighborEntry_t), PIMSM_MAX_NBR,"PIMSM_NBR_DATABASE", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* RTO-IPv4 Route Change Buffers */
  {(PIMSM_BEST_ROUTE_CHANGES_MAX *sizeof(rtoRouteChange_t)), 1,"PIMSM_RTO4_RtBuf", HEAP_BUFFPOOL_NO_OVERFLOW},

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  {0,0,"LAST POOL INDICATOR",0}
};

#if 0
/******************************************************************************
* @purpose      Wrapper to get Source Mask Len 
*
* @param        family   @b{(input)}
* @param        pMaskLen  @b{(output)} mask length
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmSrcMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen)
{
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Enter");
  if(L7_NULLPTR == pMaskLen)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
                "Invalid input parameter");
    return L7_FAILURE;
  }
  if(family == L7_AF_INET)
  {
    *pMaskLen = PIMSM_SINGLE_SRC_MSKLEN_IPV4;
  }
  else if(family == L7_AF_INET6)
  {
    *pMaskLen = PIMSM_SINGLE_SRC_MSKLEN_IPV6;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose      Wrapper to get Group Mask Len 
*
* @param        family   @b{(input)}
* @param        pMaskLen  @b{(output)} mask length
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmGrpMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen)
{
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Enter");
  if(L7_NULLPTR == pMaskLen)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
                "Invalid input parameter");
    return L7_FAILURE;
  }
  if(family == L7_AF_INET)
  {
    *pMaskLen = PIMSM_SINGLE_GRP_MSKLEN_IPV4;
  }
  else if(family == L7_AF_INET6)
  {
    *pMaskLen = PIMSM_SINGLE_GRP_MSKLEN_IPV6;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}
#endif
/******************************************************************************
* @purpose      Wrapper to get Source, Group address &  pkt length
                given the packet buffer 
*
* @param        family    @b{(input)}
* @param        pData     @b{(input)}   ip pkt start
* @param        pSrcAddr  @b{(output)}  ip source address
* @param        pGrpAddr  @b{(output)}  ip destination address
                pPktLen   @b{(output)}  payload length
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmSourceAndGroupAddressGet(L7_uchar8 family,L7_uchar8 *pData,
                                      L7_inet_addr_t *pSrcAddr,
                                      L7_inet_addr_t *pGrpAddr,
                                      L7_uint32 *pPktLen)
{
  struct ip *ip;
  L7_uint32 len =0;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Enter");
  if((L7_NULLPTR == pSrcAddr) || (L7_NULLPTR == pGrpAddr)|| 
     (L7_NULLPTR == pData) || (L7_NULLPTR == pPktLen))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Invalid input parameters");
    return L7_FAILURE;
  }
  if(family == L7_AF_INET)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "IPv4 packet");
    ip = (struct ip *)pData;

    if(ip->ip_v != IPVERSION)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Version is not IPv4");
      return(L7_FAILURE);
    }

    inetAddressSet(L7_AF_INET, &ip->ip_src.s_addr, pSrcAddr);
    inetAddressSet(L7_AF_INET, &ip->ip_dst.s_addr, pGrpAddr);
    *pPktLen = (L7_uint32 )ip->ip_len;
  }
  else if(family == L7_AF_INET6)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "IPv6 packet");
    if(pimsmV6SourceAndGroupAddressGet(pData,pSrcAddr,pGrpAddr,&len) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,"pimsmV6AddressSet() failed");
      return L7_FAILURE;
    }
    *pPktLen = len;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose      Wrapper to validate pim message length 
*
* @param        family   @b{(input)}
* @param        dataLen  @b{(input)} recv pim pkt length
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmMessageLengthValidate(L7_uchar8 family, L7_uint32 dataLen)
{
  L7_int32 ipPktSize = 0;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Enter");
  if(family == L7_AF_INET)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "IPv4 packet");
    ipPktSize = sizeof(struct ip);
  }
  else if(family == L7_AF_INET6)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "IPv6 packet");
    ipPktSize = sizeof(L7_ip6Header_t);
  }
  /* Message length validation.    */
  if(dataLen < 
     PIM_HEARDER_SIZE + PIMSM_REGISTER_HEARDER_SIZE + ipPktSize)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Message length validation failed");
    return(L7_FAILURE);
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;

}
/******************************************************************************
* @purpose      Wrapper to send out decapsulated pkt
*
* @param       family   @b{(input)}
* @param       sockfd   @b{(input)} socket fd
* @param       pOif     @b{(input)} list of outgoing router interfaces
* @param       pData    @b{(input)} ip pkt start
* @param       dataLen  @b{(input)} ip pkt length
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmDecapsulateAndSend(L7_uchar8 family, L7_uint32 sockFd,
                                interface_bitset_t *pOif, 
                                L7_uchar8 *pData, L7_uint32 dataLen, L7_uchar8 ipTOS)
{
  L7_uint32 rtrIfNum;
  struct ip *ip;
 /* L7_short16 checksum=0;*/
  L7_uchar8 ecnBits; 
    
  if((L7_NULLPTR == pOif) || (L7_NULLPTR == pData))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Invalid input parameter");
    return(L7_FAILURE);
  }
  
  if(family == L7_AF_INET)
  {
    L7_uint32 checkSum = 0;

    ip = (struct ip *)pData;

    ecnBits = ipTOS & 0x3;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO," Rx'ed IpTOS = %d ",ipTOS);
    ip->ip_tos &= 0xFC; 
    ip->ip_tos |= ecnBits; 
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO," New inner IPTos = %d ",ip->ip_tos);
   /* memset(&ip->ip_sum,0,2);
    checksum = inetChecksum((void *)ip, L7_IP_HDR_LEN);
    ip->ip_sum = checksum;*/

    /* Decrement the TTL and Re-calculate the Checksum */
    ip->ip_ttl -= 1;

    ip->ip_sum = 0;
    checkSum = inetChecksum (ip, (ip->ip_hl * 4));
    ip->ip_sum = checkSum;
  }

  for(rtrIfNum=1;rtrIfNum<MCAST_MAX_INTERFACES;rtrIfNum++)
  {

    if(BITX_TEST(pOif,rtrIfNum) != L7_NULL)
    {
      mfcIpPktDtlTransmitIntf(family,rtrIfNum, pData, dataLen);
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose      Wrapper to get start star rp mask len 
*
* @param        family   @b{(input)}
* @param        pMaskLen  @b{(output)} mask length
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmStarStarRpMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen)
{
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Enter");
  if(L7_NULLPTR == pMaskLen)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
                "Invalid input parameter");
    return L7_FAILURE;
  }
  if(family == L7_AF_INET)
  {
    *pMaskLen = PIMSM_STAR_STAR_RP_MSKLEN_IPV4;
  }
  else if(family == L7_AF_INET6)
  {
    *pMaskLen = PIMSM_STAR_STAR_RP_MSKLEN_IPV6;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose      Wrapper to build IP Hdr 
*
* @param        family     @b{(input)}
* @param        pData      @b{(input)}  ip pkt start
* @param        pSrcAddr   @b{(input)}  ip source address
* @param        pGrpAddr   @b{(input)}  ip destination address
* @param        pPktLen    @b{(output)} ip pkt length
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 

For (S,G) Null-Registers,
When generating an IPv4 Null-Register
     message, the fields in the dummy IPv4 header SHOULD be filled in
     according to the following table.  Other IPv4 header fields may
     contain any value that is valid for that field.

     Field                  Value
     ---------------------------------------
     IP Version             4
     Header Length          5
     Checksum               Header checksum
     Fragmentation offset   0
     More Fragments         0
     Total Length           20
     IP Protocol            103 (PIM)

When generating an IPv6 Null-Register
     Header Field   Value
     --------------------------------------
     IP Version     6
     Next Header    103 (PIM)
     Length         4
     PIM Version    0
     PIM Type       0
     PIM Reserved   0
     PIM Checksum   PIM checksum including
                    IPv6 "pseudo-header";
* @end
******************************************************************************/
L7_RC_t pimsmIpHdrFrame(L7_uchar8 family,L7_uchar8 *pData,L7_inet_addr_t *pSrcAddr,
                        L7_inet_addr_t *pGrpAddr,
                        L7_uint32 *pPktLen)
{
  struct ip *ip = L7_NULLPTR;
  L7_uint32 addr, ipPktLen;

  if((L7_NULLPTR == pData) ||
     (L7_NULLPTR == pSrcAddr) ||
     (L7_NULLPTR == pGrpAddr) ||
     (L7_NULLPTR == pPktLen))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Invalid input parameter");
    return(L7_FAILURE);
  }

  if(family == L7_AF_INET)
  {
    ip = (struct ip *)pData;
    ipPktLen = sizeof(struct ip);
    ip->ip_v     = IPVERSION;
    ip->ip_hl    = (ipPktLen >> 2);
    ip->ip_tos   = 0;
    ip->ip_id    = 0;
    ip->ip_off   = 0;
    ip->ip_p     = IP_PROT_PIM;  
    ip->ip_len   = osapiHtons(ipPktLen);
    ip->ip_ttl   = PIMSM_DEFAULT_MCAST_TTL;
    inetAddressGet(L7_AF_INET, pSrcAddr,&addr);
    ip->ip_src.s_addr = osapiHtonl(addr);  
    inetAddressGet(L7_AF_INET, pGrpAddr,&addr);
    ip->ip_dst.s_addr = osapiHtonl(addr);  
    ip->ip_sum   = 0;  /* prepare to compute checksum*/
    /* following line to compute checksum should be after 
      the entire ip-hdr is filled*/
    ip->ip_sum   = inetChecksum((L7_short16 *)ip, ipPktLen);
    *pPktLen =  ipPktLen;
  }
  else if(family == L7_AF_INET6)
  {
    pimsmV6IpHdrFrame(pData,pPktLen,pSrcAddr,pGrpAddr);
  }
  return L7_SUCCESS;

}   

/*********************************************************************
* @purpose  get the best route changes - ipv4 specific
*           
*
* @param    pimsmCb        @b{(input)}  control block 
* @param    routeEntry     @b{(input)}  Route information about the 
                              network that was added/ deleted/ modified
* @param    routeStatus    @b{(input)}  Information regarding the reason
                              the callback was issued.  Route_status 
                              events are enumerated in RTO_ROUTE_EVENT_t.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    
*
* @end
*********************************************************************/
static L7_RC_t pimsmV4BestRouteChanges(pimsmCB_t *pimsmCb,
                                L7_routeEntry_t *routeEntry, 
                                L7_uint32 routeStatus)
{
  L7_uint32 intIfNum =0;
  mcastRPFInfo_t routeInfo;
  L7_uchar8 maskLen=0;
  L7_inet_addr_t subnetMask;

  if(L7_NULLPTR == routeEntry)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Invalid input parameter");
    return L7_FAILURE;
  }

  inetAddressReset(&subnetMask);
  if (routeStatus == RTO_ADD_ROUTE  ||
      routeStatus == RTO_DELETE_ROUTE)
  {  
    if ((routeEntry->flags & L7_RTF_REJECT) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, 
         PIMSM_TRACE_INFO, "Recvd Reject Route - Ignore");
      return L7_SUCCESS;
    }
  } 
  else if (routeStatus == RTO_CHANGE_ROUTE )
  {
    if ((routeEntry->flags & L7_RTF_REJECT) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, 
         PIMSM_TRACE_INFO, "Recvd Reject Route - Delete");
      routeStatus = RTO_DELETE_ROUTE;
    }
  }
  memset(&routeInfo,0,sizeof(mcastRPFInfo_t));
  inetAddressZeroSet(pimsmCb->family, &routeInfo.rpfNextHop);
  inetAddressZeroSet(pimsmCb->family, &routeInfo.rpfRouteAddress);
  if (routeStatus == RTO_ADD_ROUTE  ||
      routeStatus == RTO_CHANGE_ROUTE)
  {
    /* select only one path, let it be the first one index by zero*/
    inetAddressSet(pimsmCb->family, 
                   &(routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.ipAddr),
                   &routeInfo.rpfNextHop);
  
    intIfNum = routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
    if(ipMapIntIfNumToRtrIntf(intIfNum, &routeInfo.rpfIfIndex) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "ipMapIntIfNumToRtrIntf() failed");
      return L7_FAILURE;
    }
  
    routeInfo.rpfRouteMetricPref =routeEntry->pref;
    routeInfo.rpfRouteMetric = routeEntry->metric;
    routeInfo.rpfRouteProtocol =routeEntry->protocol;
  } 
  else if (routeStatus == RTO_DELETE_ROUTE)
  {
      routeInfo.rpfIfIndex = MCAST_MAX_INTERFACES;
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
      "Wrong routeStatus = %d", routeStatus);
    return L7_FAILURE;
  }
  routeInfo.status = routeStatus;
  inetAddressSet(L7_AF_INET,(void *)&routeEntry->subnetMask,&subnetMask);
  inetMaskToMaskLen(&subnetMask,&maskLen);
  routeInfo.prefixLength = (L7_uint32)maskLen; 

  inetAddressSet(pimsmCb->family, 
                 &routeEntry->ipAddr, 
                 &(routeInfo.rpfRouteAddress));

  if(pimsmRtoBestRouteChangeProcess(pimsmCb,&routeInfo) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
      "pimsmRtoBestRouteChangeProcess() failed");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Function to get  best route changes 
*
* @param    pimsmCb        @b{(input)}  control block 
* @param    pMoreChanges   @b{(output)} any more changes avaiable
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmRtoBestRoutesGet(pimsmCB_t *pimsmCb,
                              L7_BOOL *pMoreChanges)
{
  L7_uint32 numChanges =0,ii=0;
  rtoRouteChange_t *routeChange = L7_NULLPTR,*routeChangeBuf = L7_NULLPTR;

  if((L7_NULLPTR == pMoreChanges) || (L7_NULLPTR == pimsmCb))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "Invalid input parameter");
    return L7_FAILURE;
  }
  *pMoreChanges = L7_FALSE;
  
  if(pimsmCb->family == L7_AF_INET)
  {
    routeChangeBuf = (rtoRouteChange_t *)pimsmCb->pimsmRouteChangeBuf;
    if(rtoRouteChangeRequest(pimsmMapRto4BestRouteClientCallback,
                             PIMSM_BEST_ROUTE_CHANGES_MAX,
                             &numChanges,pMoreChanges,
                             routeChangeBuf) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, "rtoRouteChangeRequest() failed");
      return L7_FAILURE;
    }
    for(ii =0;ii < numChanges;ii++)
    {
      routeChange = &routeChangeBuf[ii];
      pimsmV4BestRouteChanges(pimsmCb,&routeChange->routeEntry,
                              routeChange->changeType);
    }
  }
  else if(pimsmCb->family == L7_AF_INET6)
  {
    pimsmV6BestRoutesGet(pimsmCb,pMoreChanges);
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  set IPv4 socket options
*
* @param    sockfd   @b{(input)} socket fd
*
* @returns  void
*
* @comments     
*     
* @end
******************************************************************************/
static void pimsmV4SocketOptionsSet(L7_uint32 sockfd)
{
  L7_char8   mloop =0;

  /* Set socket options for multicast loop.Setting the values to 0 implies 
     we do not want to receive the multicast packet sent out*/
  mloop = 0;
  if(osapiSetsockopt(sockfd,
                     IPPROTO_IP,
                     L7_IP_MULTICAST_LOOP,
                     &mloop,
                     sizeof (mloop))!=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                "PIMSM: Set socket options failed\n");
  }
  return;  
}

/******************************************************************************
* @purpose      Wrapper to set socket options
*
* @param        family   @b{(input)}
* @param        sockFd  @b{(input)} socket fd
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmSocketOptionsSet(L7_uchar8 family,L7_uint32 sockfd)
{

  if(family == L7_AF_INET)
  {
    pimsmV4SocketOptionsSet(sockfd);
  }
  else if(family == L7_AF_INET6)
  {
    pimsmV6SocketOptionsSet(sockfd);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Intialize memory to receive RTO route changes 
*
* @param    pimsmCb     @b{(input)}  control block 
* @param    
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmRouteChangeBufMemAlloc(pimsmCB_t *pimsmCb)
{
  rtoRouteChange_t *routeChangeBuf = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG,"Entry");
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,"Invalid input parameters");
    return L7_FAILURE;
  }
  if(pimsmCb->family == L7_AF_INET)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG,"ipv4 process");
    routeChangeBuf = (rtoRouteChange_t *)
         PIMSM_ALLOC (pimsmCb->family,
                      (PIMSM_BEST_ROUTE_CHANGES_MAX *sizeof(rtoRouteChange_t)));

    if(routeChangeBuf == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                  "PIMSM: Memory allocation failed for RTO Buffer\n");
      return L7_FAILURE;
    }
    pimsmCb->pimsmRouteChangeBuf = (void *)routeChangeBuf;
  }
  else if(pimsmCb->family == L7_AF_INET6)
  {
    if(pimsmV6RouteChangeBufMemAlloc(pimsmCb) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,"pimsmV6RouteChangeBufMemAlloc failed");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize the PIM-SM Heap Memory
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments Enable the HEAP_LOCK_GUARD so that the Heap Buffer Pools
*           are protected when multiple tasks try to access the same 
*           heap buffer pools.
*           This has been enabled to resolve the race condition in the
*           IPv6 control packet reception path, where-in PktRcvr Task
*           tries to allocate and PIM-SM Task tries to free the buffers.
*
* @end
*********************************************************************/
L7_RC_t
pimsmUtilMemoryInit (L7_uint32 addrFamily)
{
  heapBuffPool_t *pimsmHeapPoolList = L7_NULLPTR;

  PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "Entry");

  if (addrFamily == L7_AF_INET)
  {
    pimsmHeapPoolList = pimsmV4HeapPoolList;
  }
  else if (addrFamily == L7_AF_INET6)
  {
    extern heapBuffPool_t pimsmV6HeapPoolList[];

    pimsmHeapPoolList = pimsmV6HeapPoolList;
  }
  else
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                 "Bad Address Family - %d", addrFamily);
    return L7_FAILURE;
  }

  if (heapInit (mcastMapHeapIdGet(addrFamily), pimsmHeapPoolList,
                HEAP_LOCK_GUARD)
             != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
                 "PIM-SM Heap Initialization Failed");
    return L7_FAILURE;
  }

  PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose      Wrapper to get the Join Prune Msg Size 
*
* @param        family   @b{(input)}
* @param        pJoinPruneMsgSize  @b{(output)} Join Prune Msg Size
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments 
* @end
******************************************************************************/
L7_RC_t pimsmJoinPruneMsgLenGet(L7_uchar8 family,L7_uint32 *pJoinPruneMsgSize)
{
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Enter");
  if(L7_NULLPTR == pJoinPruneMsgSize)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR, 
                "Invalid input parameter");
    return L7_FAILURE;
  }
  *pJoinPruneMsgSize = 0;
  if(family == L7_AF_INET)
  {
    *pJoinPruneMsgSize = 4 + sizeof(L7_in_addr_t) ;
  }
  else if(family == L7_AF_INET6)
  {
    *pJoinPruneMsgSize = 4 + sizeof(L7_in6_addr_t) ;
    /* same as *pJoinPruneMsgSize = sizeof(pim_encod_src_addr_t) ;*/
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Exit");
  return L7_SUCCESS;
}


