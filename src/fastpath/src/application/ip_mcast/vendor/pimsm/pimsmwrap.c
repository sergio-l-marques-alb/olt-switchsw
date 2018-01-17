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
#include "sdm_api.h"


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

/* Number of elements in IPv4 heap pool list, including dummy end-of-list element */
#define PIMSM_IPV4_POOL_LIST_SIZE  24
heapBuffPool_t pimsmV4HeapPoolList[PIMSM_IPV4_POOL_LIST_SIZE]; 

/*********************************************************************
*
* @purpose  Build the PIM-SM IPv4 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
*           When a pool is added here, increment PIMSM_IPV4_POOL_LIST_SIZE.
*
* @end
*********************************************************************/
static heapBuffPool_t *pimsmV4HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(pimsmV4HeapPoolList, 0, sizeof(heapBuffPool_t) * PIMSM_IPV4_POOL_LIST_SIZE);

  /* PIMSM Protocol Control Block Buffer */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmCB_t);
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_CB", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Join/Prune Message Buffers */
  pimsmV4HeapPoolList[i].buffSize = PIMSM_MAX_JP_MSG_SIZE;
  pimsmV4HeapPoolList[i].buffCount = PIMSM_MAX_JP_PACKETS;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_JPG_MSG_BUFFER", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Join/Prune Address List Buffers for IPv4 */
  pimsmV4HeapPoolList[i].buffSize = PIM_IPV4_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST * sizeof(pim_encod_src_addr_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_MAX_PROTOCOL_PACKETS;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_JP_ADDR_LIST_BUFFER", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Cache Entry Buffers */
  pimsmV4HeapPoolList[i].buffSize = sizeof(pimsmCache_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_MAX_IPV4_CACHE_ENTRIES;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_CACHE_BUFFERS", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Tree Heap Buffers for (S,G), (*,G) and (S,G,Rpt) Databases */
  pimsmV4HeapPoolList[i].buffSize = platIpv4McastRoutesMaxGet() * sizeof (avlTreeTables_t);
  pimsmV4HeapPoolList[i].buffCount = 3;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_SG_TREE_HEAP", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for (S,G) Database */
  pimsmV4HeapPoolList[i].buffSize = platIpv4McastRoutesMaxGet() * sizeof (pimsmSGNode_t);
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_SG_DATA_HEAP", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Downstream Interface Buffers for (S,G) and (*,G) Databases */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmSGIEntry_t);
  pimsmV4HeapPoolList[i].buffCount = ((PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * platIpv4McastRoutesMaxGet()) + 
                                      (PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * platIpv4McastRoutesMaxGet()));
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_SGI_*GI_BUFFERS", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for (S,G,Rpt) Database */
  pimsmV4HeapPoolList[i].buffSize = (platIpv4McastRoutesMaxGet() * sizeof (pimsmSGRptNode_t));
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_SG_RPT_DATA_HEAP", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Downstream Interface Buffers for (S,G,Rpt) Database */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmSGRptIEntry_t);
  pimsmV4HeapPoolList[i].buffCount = (PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * platIpv4McastRoutesMaxGet());
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_SGRptI_BUFFERS", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* AVL Data Heap Buffers for (*,G) Database */
  pimsmV4HeapPoolList[i].buffSize = platIpv4McastRoutesMaxGet() * sizeof (pimsmStarGNode_t);
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_*G_DATA_HEAP", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* (*,*,RP) Buffers */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmStarStarRpNode_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_**RP_BUFFERS", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Downstream Interface Buffers for (*,*,RP) Database */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmStarStarRpIEntry_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES * PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_**RPI_BUFFERS", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* RP Block Buffer */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmRpBlock_t);
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_RP_BLOCK", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* RADIX Tree Heap Buffers for RP Set Database */
  pimsmV4HeapPoolList[i].buffSize = (RADIX_TREE_HEAP_SIZE(PIMSM_RP_GRP_ENTRIES_MAX,sizeof(rpSetTreeKey_t)));
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_RP_SET_TREE_HEAP", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* RADIX Data Heap Buffers for RP Set Database */
  pimsmV4HeapPoolList[i].buffSize = PIMSM_RP_GRP_ENTRIES_MAX * sizeof (pimsmRpSetNode_t);
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_RP_SET_DATA_HEAP", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* RP Group List Buffers */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmRpGrpNode_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_RP_GRP_ENTRIES_MAX;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_RP_GRP_LIST", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* RP Group Address List Buffers */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmAddrList_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_RP_GRP_ADDR_LIST_NODES_MAX;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_MRT_RP_GRP_ADDR_LIST", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* BSR Block Buffer */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmBSRBlock_t);
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_BSR_BLOCK", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* BSR Per Scope Zone Buffer */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmBsrPerScopeZone_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_BSR_SCOPE_NODES_MAX;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_BSR_PER_SCOPE_ZONE", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* BSR Fragment Group-RP Node Buffers */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmFragGrpRpNode_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_BSR_FRAGMENT_GRP_ENTRIES_MAX;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_FRAG_GRP_RP_NODE", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* BSR Fragment RP Node Buffers */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmFragRpNode_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_BSR_FRAGMENT_RP_ENTRIES_MAX;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_FRAG_RP_NODE", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Neighbor Database Buffers */
  pimsmV4HeapPoolList[i].buffSize = sizeof (pimsmNeighborEntry_t);
  pimsmV4HeapPoolList[i].buffCount = PIMSM_MAX_NBR;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_NBR_DATABASE", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* RTO-IPv4 Route Change Buffers */
  pimsmV4HeapPoolList[i].buffSize = (PIMSM_BEST_ROUTE_CHANGES_MAX * sizeof(rtoRouteChange_t));
  pimsmV4HeapPoolList[i].buffCount = 1;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "PIMSM_RTO4_RtBuf", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = HEAP_BUFFPOOL_NO_OVERFLOW;
  i++;

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  pimsmV4HeapPoolList[i].buffSize = 0;
  pimsmV4HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(pimsmV4HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  pimsmV4HeapPoolList[i].flags = 0;
  i++;

  if (i != PIMSM_IPV4_POOL_LIST_SIZE)
  {
    L7_LOG_ERROR(i);
  }
  return pimsmV4HeapPoolList;
}

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
    pimsmHeapPoolList = pimsmV4HeapPoolListGet();
  }
  else if (addrFamily == L7_AF_INET6)
  {
    if (!sdmTemplateSupportsIpv6())
    {
      return L7_SUCCESS;
    }
    pimsmHeapPoolList = pimsmV6HeapPoolListGet();
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


