/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pimsm_v6_stub.c
*
* @purpose   PIM-SM stub functions for IPv6.
*
* @component
*
* @comments  none
*
* @create   11/04/2006
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
#include "rtiprecv.h"
#include "l3_mcast_commdefs.h"
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmtimer.h"
#include "pimsmbsr.h"
#include "pimsmcontrol.h"
#include "pimsmintf.h"
#include "l7apptimer_api.h"
#include "pimsmv6util.h"
#include "heap_api.h"


/* Number of elements in IPv6 heap pool list, including dummy end-of-list element */
#define PIMSM_IPV6_POOL_LIST_SIZE  1
heapBuffPool_t pimsmV6HeapPoolList[PIMSM_IPV6_POOL_LIST_SIZE]; 

/*********************************************************************
*
* @purpose  Build the PIM-SM IPv6 buffer pool list
*
* @param    void
*
* @returns  pointer to first array element
*
* @comments Build list at run time since buffer pool sizes depend on 
*           SDM template scaling parameters.
*
* @end
*********************************************************************/
heapBuffPool_t *pimsmV6HeapPoolListGet(void)
{
  L7_uint32 i = 0;

  memset(pimsmV6HeapPoolList, 0, sizeof(heapBuffPool_t) * PIMSM_IPV6_POOL_LIST_SIZE);

  /* Last item in the Heap Pool ... DO NOT REMOVE/MODIFY IT */
  pimsmV6HeapPoolList[i].buffSize = 0;
  pimsmV6HeapPoolList[i].buffCount = 0;
  osapiStrncpySafe(pimsmV6HeapPoolList[i].name, "LAST POOL INDICATOR", HEAP_POOLNAME_SIZE);
  pimsmV6HeapPoolList[i].flags = 0;
  i++;

  return pimsmV6HeapPoolList;
}

/******************************************************************************
* @purpose  Wrapper to get Source, Group address &  pkt length
            given the packet buffer
*
* @param        family    @b{(input)}
* @param        pData     @b{(input)}   ip pkt start
* @param        pSrcAddr  @b{(output)}  ip source address
* @param        pGrpAddr  @b{(output)}  ip destination address
                pPktLen   @b{(output)}  payload length
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmV6SourceAndGroupAddressGet(L7_uchar8 *pData,L7_inet_addr_t *pSrcAddr,
                          L7_inet_addr_t *pGrpAddr,L7_uint32 *pPktLen)
{
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  This function is called to lookup for  the best route for
            the given address.
*
* @param     pIpAddr       @b{(input)}    ip address
* @param     pRtoRoute     @b{(output)}   RTO Best route entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmV6BestRouteLookup(L7_in6_addr_t *pIpAddr,
                               mcastRPFInfo_t *pRtoRoute)
{
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  Wrapper to build IP Hdr
*
* @param        pData      @b{(input)}  ip pkt start
* @param        pSrcAddr   @b{(input)}  ip source address
* @param        pGrpAddr   @b{(input)}  ip destination address
* @param        pPktLen    @b{(output)} ip pkt length
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmV6IpHdrFrame(L7_uchar8 *pData,L7_uint32 *pPktLen,
                          L7_inet_addr_t *pSrcAddr,L7_inet_addr_t *pGrpAddr)
{
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  Function to get  best route
*
* @param    pimsmCb        @b{(input)}  control block
* @param    pMoreChanges   @b{(output)} any more changes avaiable
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments
* @end
******************************************************************************/
L7_RC_t pimsmV6BestRoutesGet(pimsmCB_t *pimsmCb,
                             L7_BOOL *pMoreChanges)
{
  return L7_FAILURE;
}
/******************************************************************************
* @purpose   This function is called to set the socket options for
               the Ipv6 socket descriptor passed as input.
*
* @param     sockFd      @b{(input)}  socket fd
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments      none
*
* @end
******************************************************************************/
L7_RC_t pimsmV6SocketOptionsSet(L7_uint32 sockfd)
{
  return L7_FAILURE;
}
/******************************************************************************
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
******************************************************************************/
L7_RC_t pimsmV6RouteChangeBufMemAlloc(pimsmCB_t *pimsmCb)
{
  return L7_FAILURE;
}
