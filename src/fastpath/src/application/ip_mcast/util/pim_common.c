/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   pim_common.c
*
* @purpose    interface PIM utility functions
*
* @component  Mcast utilities
*
* @comments   none
*
* @create     04/17/2006
*
* @author     Ramanjaneyulu Y T (YTR)
*
* @end
**********************************************************************/
#include <stdio.h>
#include <string.h>

#include "pim_defs.h"
#include "mcast_v6.h"
#include "l7_socket.h"

/* the single source mask length */
#define PIM_SINGLE_SRC_MSKLEN_IPV4            32
#define PIM_SINGLE_SRC_MSKLEN_IPV6            128

/* the single group mask length  */
#define PIM_SINGLE_GRP_MSKLEN_IPV4            32
#define PIM_SINGLE_GRP_MSKLEN_IPV6            128


/*********************************************************************
* @purpose  Enables/Disables the PIM IPv6 Group address and interface ID with 
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
L7_RC_t pimV6SockChangeMembership(L7_uint32 rtrIfNum, L7_uint32 intfIfNum,
                              L7_uint32 addFlag, L7_uint32 sockfd)
{
  struct L7_ip_mreq6_s mreq6;
  L7_RC_t retVal = L7_SUCCESS;
  L7_int32 sockopt;
  L7_inet_addr_t addr;

  inetAllPimRouterAddressInit(L7_AF_INET6, &addr); 
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
L7_RC_t pimJoinPruneMsgLenGet(L7_uchar8 family,L7_uint32 *pJoinPruneMsgSize)
{
  if(L7_NULLPTR == pJoinPruneMsgSize)
  {
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
  return L7_SUCCESS;
}

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
L7_RC_t pimSrcMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen)
{
  if(L7_NULLPTR == pMaskLen)
  {
    return L7_FAILURE;
  }
  if(family == L7_AF_INET)
  {
    *pMaskLen = PIM_SINGLE_SRC_MSKLEN_IPV4;
  }
  else if(family == L7_AF_INET6)
  {
    *pMaskLen = PIM_SINGLE_SRC_MSKLEN_IPV6;
  }
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
L7_RC_t pimGrpMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen)
{
  if(L7_NULLPTR == pMaskLen)
  {
    return L7_FAILURE;
  }
  if(family == L7_AF_INET)
  {
    *pMaskLen = PIM_SINGLE_GRP_MSKLEN_IPV4;
  }
  else if(family == L7_AF_INET6)
  {
    *pMaskLen = PIM_SINGLE_GRP_MSKLEN_IPV6;
  }
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
L7_uint32 pimJoinPruneMaxAddrsInListGet(L7_uchar8 family)
{
  L7_uint32 maxAddrs = 0;
  
  if(family == L7_AF_INET)
  {
    maxAddrs = PIM_IPV4_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST; 
  }
  else if(family == L7_AF_INET6)
  {
    maxAddrs = PIM_IPV6_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST;    
  }
  return maxAddrs;
}
