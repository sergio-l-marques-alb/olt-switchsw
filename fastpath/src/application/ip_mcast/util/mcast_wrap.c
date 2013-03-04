/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   mcast_wrap.c
*
* @purpose    interface (unified IPMAP-related) APIs
*
* @component  Mcast utilities
*
* @comments   none
*
* @create     04/17/2006
*
* @author     gkk/dsatyanarayana
*
* @end
**********************************************************************/
#include "mcast_inet.h"
#include "mcast_wrap.h"
#include "mcast_v6.h"
#include "l7_ip_api.h"
#include "dtl_l3_api.h"
#include "mcast_api.h"
#include "mcast_defs.h"
#include "l3_mcast_commdefs.h"
#include "rto_api.h"
#include "mcast_util.h"

/* The last 3 bytes in the MAC-DA and IPv6 DA should match */
#define MCAST_MAC_IP4_DA_MATCH_BYTES  3

/* The first 3 bytes in the Mcast MAC are 0x3333 */
#define MCAST_MAC_IP4_DA_PREFIX_LEN   3

static L7_uchar8 MCAST_MAC_IP4_DA_PREFIX[] = {0x01, 0x00, 0x5E};

/*********************************************************************
* @purpose  Get the operational administrative mode of the router
*
* @param    none
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @comments This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_uint32  mcastIpMapRtrAdminModeGet(L7_uint32 familyType)
{
  L7_uint32        adminMode = L7_NULL;

   if (familyType == L7_AF_INET)
  {
    adminMode = ipMapRtrAdminModeGet();
  }
  else if (familyType == L7_AF_INET6)
  {
    adminMode = mcastIp6MapRtrAdminModeGet();
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_MCAST_MAP_COMPONENT_ID,
            "Invalid familyType %d",familyType);
  }
 return adminMode;
}
/*********************************************************************
* @purpose  Convert Internal Interface Number to Router Interface Number
*
* @param    familyType @b{(input)}Address Family type
* @param    intIfNum   @b{(input)}internal interface number
* @param    rtrIntf   @b{(input)}pointer to router interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t mcastIpMapIntIfNumToRtrIntf(L7_uchar8 familyType,
                                L7_uint32 intIfNum, L7_uint32* rtrIfNum)
{
   L7_RC_t rc;

   if (familyType == L7_AF_INET)
     rc = ipMapIntIfNumToRtrIntf(intIfNum, rtrIfNum);
   else if (familyType == L7_AF_INET6)
     rc = mcastIp6MapIntIfNumToRtrIntf(intIfNum , rtrIfNum);
   else
     return L7_FAILURE;
   return rc;
}

/*********************************************************************
* @purpose  Convert Router Interface Number to Internal Interface Number
*
* @param    familyType @b{(input)}Address Family type
* @param    rtrIntf    @b{(input)}pointer to router interface number
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t mcastIpMapRtrIntfToIntIfNum(L7_uchar8 familyType,
                                L7_uint32 rtrIfNum, L7_uint32* intIfNum)
{
  L7_RC_t rc;

  if (familyType == L7_AF_INET)
    rc = ipMapRtrIntfToIntIfNum(rtrIfNum, intIfNum);
  else if (familyType == L7_AF_INET6)
    rc = mcastIp6MapRtrIntfToIntIfNum(rtrIfNum, intIfNum);
  else
    return L7_FAILURE;

  return rc;
}
/*********************************************************************
* @purpose  Get the IP address based on family.
*
* @param    familyType @b{(input)}Address Family type
* @param    intIfNum   @b{(input)}internal interface number
* @param    *ipAddr    @b{(input)}inet IP address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    for ipv6, get global ip addr
*
* @end
*
*********************************************************************/
L7_RC_t mcastIpMapRtrIntfGlobalIpAddressGet(L7_uchar8 familyType,
                                L7_uint32 intIfNum, L7_inet_addr_t *ipAddr)
{
  L7_uint32  myIpAddr;
  L7_uint32  myMask;

  if (familyType == L7_AF_INET)
  {
    if (ipMapIntfIsUnnumbered(intIfNum) == L7_TRUE)
    {
      myIpAddr = ipMapBorrowedAddr(intIfNum);
      myMask =  L7_IP_SUBMASK_EXACT_MATCH_MASK;
    }
    else
    {
      if (ipMapRtrIntfIpAddressGet(intIfNum,
                                   &myIpAddr, &myMask) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
    if (inetAddressSet(L7_AF_INET, &myIpAddr, ipAddr) != L7_SUCCESS)
      return L7_FAILURE;
  }
  else if (familyType == L7_AF_INET6)
  {
    if(mcastIp6MapRtrIntfGlobalIpAddressGet(intIfNum, ipAddr) != L7_SUCCESS)
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the IP address based on family.
*
* @param    familyType @b{(input)}Address Family type
* @param    intIfNum   @b{(input)}internal interface number
* @param    *ipAddr    @b{(input)}inet IP address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    for ipv6, get link-local ip addr
*
*
* @end
*
*********************************************************************/
L7_RC_t mcastIpMapRtrIntfIpAddressGet(L7_uchar8 familyType,
                                L7_uint32 intIfNum, L7_inet_addr_t *ipAddr)
{
  L7_uint32  myIpAddr;
  L7_uint32  myMask;

  if (familyType == L7_AF_INET)
  {
    if (ipMapIntfIsUnnumbered(intIfNum) == L7_TRUE)
    {
      myIpAddr = ipMapBorrowedAddr(intIfNum);
      myMask =  L7_IP_SUBMASK_EXACT_MATCH_MASK;
    }
    else
    {
      if (ipMapRtrIntfIpAddressGet(intIfNum,
                                   &myIpAddr, &myMask) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }

    if (inetAddressSet(L7_AF_INET, &myIpAddr, ipAddr) != L7_SUCCESS)
      return L7_FAILURE;
  }
  else if (familyType == L7_AF_INET6)
  {
    if(mcastIp6MapRtrIntfIpAddressGet(intIfNum, ipAddr) != L7_SUCCESS)
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the NetMask based on family.
*
* @param    familyType @b{(input)}Address Family type
* @param    intIfNum   @b{(input)}internal interface number
* @param    *netMask   @b{(input)}inet NetMask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t mcastIpMapRtrIntfNetMaskGet(L7_uchar8 familyType,
                           L7_uint32 intIfNum, L7_inet_addr_t *netMask)
{
  L7_uint32  myIpAddr;
  L7_uint32  myMask;

  if (familyType == L7_AF_INET)
  {
    if (ipMapIntfIsUnnumbered(intIfNum) == L7_TRUE)
    {
      myMask =  L7_IP_SUBMASK_EXACT_MATCH_MASK;
    }
    else
    {
      if (ipMapRtrIntfIpAddressGet(intIfNum,
                                   &myIpAddr, &myMask) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }

    if (inetAddressSet(L7_AF_INET, &myMask, netMask) != L7_SUCCESS)
      return L7_FAILURE;

  }
  else if (familyType == L7_AF_INET6)
  {
    if(mcastIp6MapRtrIntfNetMaskGet(intIfNum, netMask) != L7_SUCCESS)
      return L7_FAILURE;
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the IP address based on family.
*
* @param    familyType @b{(input)}Address Family type
* @param    intIfNum   @b{(input)}internal interface number
* @param    *ipAddr    @b{(input)}inet IP address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t mcastIpMapRtrIntfIpAddressesGet(L7_uchar8 familyType,
                                L7_uint32 rtrIfNum, L7_uint32 *numAddr,
                                L7_inet_addr_t *ipAddrList,
                                L7_uint32 *ipPrefixLen)
{
  L7_uint32 prefixlen, count = 0;
  L7_uint32 intIfNum;

  if (mcastIpMapRtrIntfToIntIfNum(familyType, rtrIfNum, &intIfNum) != L7_SUCCESS)
     return L7_FAILURE;
  if (familyType == L7_AF_INET)
  {
    L7_rtrIntfIpAddr_t  addrs[L7_L3_NUM_IP_ADDRS];
    L7_inet_addr_t ipAddr;
    if (ipMapRtrIntfIpAddrListGet(intIfNum, addrs) == L7_SUCCESS)
    {
      L7_uint32 j;
      for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
      {
         if (addrs[j].ipAddr != L7_NULL_IP_ADDR)
          {
             ++count;
             if (inetAddressSet(L7_AF_INET, &addrs[j].ipAddr, &ipAddr)
                 != L7_SUCCESS)
             {
                  return L7_FAILURE;
             }
             MCAST_MASK_TO_MASKLEN(addrs[j].ipMask, prefixlen);
             if ((ipAddrList+j) != L7_NULLPTR) {
                 inetCopy((ipAddrList+j), &ipAddr);
                 ipPrefixLen[j] = prefixlen;
             }
         }
      }
      *numAddr = count;
    }
  }
  else if (familyType == L7_AF_INET6)
  {
      if(mcastIp6MapRtrIntfIpAddrListGet(intIfNum, numAddr,
            ipAddrList, ipPrefixLen) != L7_SUCCESS)
      return L7_FAILURE;
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the IP address based on family.
*
* @param    familyType @b{(input)}Address Family type
* @param    rtrIfNum   @b{(input)}router interface number
* @param    *ipAddr    @b{(input)}inet IP address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    for ipv6, get global ip addr
*
* @end
*
*********************************************************************/
L7_RC_t mcastIpMapGlobalIpAddressGet(L7_uchar8 family, L7_uint32 rtrIfNum,
                              L7_inet_addr_t *intfAddr)
{
  L7_RC_t  pRetCode = L7_FAILURE;
  L7_uint32 intIfNum = 0;

   pRetCode = mcastIpMapRtrIntfToIntIfNum(family, rtrIfNum,
             &intIfNum);
  if(pRetCode != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pRetCode = mcastIpMapRtrIntfGlobalIpAddressGet(family, intIfNum, intfAddr);
  if(pRetCode != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the IP address based on family.
*
* @param    familyType @b{(input)}Address Family type
* @param    rtrIfNum   @b{(input)}router interface number
* @param    *ipAddr    @b{(input)}inet IP address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    for ipv6, get link-local ip addr
*
* @end
*
*********************************************************************/
L7_RC_t mcastIpMapIpAddressGet(L7_uchar8 family, L7_uint32 rtrIfNum,
                              L7_inet_addr_t *intfAddr)
{
  L7_RC_t  pRetCode = L7_FAILURE;
  L7_uint32 intIfNum = 0;

   pRetCode = mcastIpMapRtrIntfToIntIfNum(family, rtrIfNum,
             &intIfNum);
  if(pRetCode != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pRetCode = mcastIpMapRtrIntfIpAddressGet(family, intIfNum, intfAddr);
  if(pRetCode != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Is a directly connected address
*
* @param  address @b{(input)} inet address
* @param  rtrIfNum @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL mcastIpMapIsDirectlyConnected(L7_inet_addr_t * address,
   L7_uint32 *rtrIfNum)
{
    L7_uint32 i;

    for (i = 1 ; i < MAX_INTERFACES; i++)
  {
        if(inetIsDirectlyConnected(address, i) == L7_TRUE)
    {
            if (rtrIfNum != L7_NULLPTR )
      {
                *rtrIfNum = i;
            }
            return L7_TRUE;
        }
    }
    return (L7_FALSE);
}

/*********************************************************************
* @purpose  Is a local address
*
* @param    address  @b{(input)} inet address
* @param    rtrIfNum @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL mcastIpMapIsLocalAddress(L7_inet_addr_t * address,
   L7_uint32 *rtrIfNum)
{
  L7_uint32 i;

  for (i = 1 ; i < MAX_INTERFACES; i++)
  {
    if (inetIsLocalAddress(address, i) == L7_TRUE)
    {
      if (rtrIfNum != L7_NULLPTR )
      {
        *rtrIfNum = i;
      }
      return L7_TRUE;
    }
  }
  return(L7_FALSE);
}

/******************************************************************************
* @purpose      Wrapper for best route lookup
*
* @param        family   @b{(input)}Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param          src_addr @b{(input)}inet address
* @param        rtoRoute  @b{(input)}pointer to the structure mcastRPFInfo_t
*
* @returns       L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*
******************************************************************************/
L7_RC_t mcastRPFInfoGet(L7_inet_addr_t *src_addr,
                             mcastRPFInfo_t *bestRPFRoute)
{
  L7_uint32 routeNextHopIp;
  L7_uint32 localAddr =0,routeMask =0,rtrIfNum =0;
  L7_uint32 intIfNum =0;
  L7_inet_addr_t subnetMask;
  L7_uchar8 family,maskLen=0;
  L7_routeEntry_t rtoRoute;

  if((L7_NULLPTR == src_addr) || (L7_NULLPTR == bestRPFRoute))
  {
    return L7_FAILURE;
  }

  family = src_addr->family;
  inetAddressReset(&subnetMask);

  memset(bestRPFRoute,0,sizeof(mcastRPFInfo_t));
  inetAddressZeroSet(family, &bestRPFRoute->rpfNextHop);
  inetAddressZeroSet(family, &bestRPFRoute->rpfRouteAddress);

  if (mcastMapStaticMRouteBestRouteLookup (family, src_addr, bestRPFRoute)
                                        == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  if(family == L7_AF_INET)
  {
    memset(&rtoRoute,0,sizeof(L7_routeEntry_t));
    if (rtoBestRouteLookup(src_addr->addr.ipv4.s_addr,
                           &rtoRoute, L7_FALSE) == L7_SUCCESS)
    {
      /* Use only first split for now */
      intIfNum = rtoRoute.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
      routeNextHopIp =rtoRoute.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr;
      if(ipMapRtrIntfIpAddressGet(intIfNum, &localAddr, &routeMask)
           != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      if (ipMapIntIfNumToRtrIntf(intIfNum,&rtrIfNum) != L7_SUCCESS) {
        return L7_FAILURE;
      }
      inetAddressSet(family, (void*)&rtoRoute.ipAddr,
                      &(bestRPFRoute->rpfRouteAddress));
      bestRPFRoute->rpfIfIndex = rtrIfNum;
      inetAddressSet(family, &routeNextHopIp, &(bestRPFRoute->rpfNextHop));
      bestRPFRoute->rpfRouteMetric =  rtoRoute.metric;
      bestRPFRoute->rpfRouteMetricPref = rtoRoute.pref;
      bestRPFRoute->rpfRouteProtocol = rtoRoute.protocol;
      inetAddressSet(L7_AF_INET,(void *)&rtoRoute.subnetMask,&subnetMask);
      inetMaskToMaskLen(&subnetMask,&maskLen);
      bestRPFRoute->prefixLength = (L7_uint32 )maskLen;
    }
    else
    {
      return L7_FAILURE;
    }
  }
  else if(family == L7_AF_INET6)
  {
    if(mcastV6BestRouteLookup(&src_addr->addr.ipv6,bestRPFRoute) !=
         L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  if (mcastIpMapIsIntfUnnumbered(family,rtrIfNum) != L7_TRUE)
  {
    if (mcastIpMapIsDirectlyConnected(src_addr,&rtrIfNum) == L7_TRUE)
    {
      inetCopy(&bestRPFRoute->rpfNextHop,src_addr);
      bestRPFRoute->rpfIfIndex = rtrIfNum;
      bestRPFRoute->rpfRouteMetric =  0;
      bestRPFRoute->rpfRouteMetricPref = 0;
      bestRPFRoute->rpfRouteProtocol = RTO_LOCAL;
    }
    if (mcastIpMapIsLocalAddress(src_addr,L7_NULLPTR) == L7_TRUE)
    {
      inetAddressZeroSet(family, &bestRPFRoute->rpfNextHop);
      inetAddressZeroSet(family, &bestRPFRoute->rpfRouteAddress);
      bestRPFRoute->rpfIfIndex = 0;
      bestRPFRoute->rpfRouteMetric =  0;
      bestRPFRoute->rpfRouteMetricPref = 0;
    }
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  return the RPF neighbor information for the given inet addr
*
* @param    address    @b{(input)} inetAddr
* @param    rtrIfNum   @b{(input)} Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
* RPF_interface(S) is the interface the MRIB indicates would be used
* to route packets to S.
* RPF_interface(S) is zero if S is a local address.
*
* @end
*
******************************************************************************/
L7_RC_t mcastRPFInterfaceGet ( L7_inet_addr_t * address,
                               L7_uint32 * rtrIfNum )
{
  mcastRPFInfo_t    bestRPFRoute;

  if((L7_NULLPTR == address) || (L7_NULLPTR == rtrIfNum))
  {
    return L7_FAILURE;
  }

  if ( mcastIpMapUnnumberedIsLocalAddress(address, L7_NULLPTR ) == L7_TRUE )
  {
      *rtrIfNum = 0;
      return L7_SUCCESS;
  }

  if ( mcastRPFInfoGet(address, &bestRPFRoute ) != L7_SUCCESS )
  {
      *rtrIfNum = MAX_INTERFACES;
      return L7_FAILURE;
  }
  *rtrIfNum = bestRPFRoute.rpfIfIndex;
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  To check whether the MAC address is formatted properly or not
*
* @param    family    @b{(input)} family type

* @param    ipAddrBuf   @b{(input)} Ip Address buffer
* @param    macAddrBuf  @b{(input)} Mac Address buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
******************************************************************************/
L7_RC_t mcastMacAddressCheck (L7_uchar8 family,L7_uchar8 *ipAddrBuf  ,
                               L7_uchar8 *macAddrBuf  )
{
  L7_uint32 ipAddressIndex;
  L7_uint32 macAddressIndex;
  L7_RC_t rc = L7_FAILURE;

  if((L7_NULLPTR == ipAddrBuf  ) || (L7_NULLPTR == macAddrBuf))
  {
    return L7_FAILURE;
  }

  ipAddressIndex = (L7_IP_ADDR_LEN - MCAST_MAC_IP4_DA_MATCH_BYTES);

  macAddressIndex = (L7_MAC_ADDR_LEN - MCAST_MAC_IP4_DA_MATCH_BYTES);

  if(family == L7_AF_INET)
  {
    if ((memcmp(macAddrBuf, MCAST_MAC_IP4_DA_PREFIX, MCAST_MAC_IP4_DA_PREFIX_LEN) == 0))

    {
      if((((ipAddrBuf[ipAddressIndex])& 0x7f) == ((macAddrBuf[macAddressIndex]) & 0x7f )) &&
         ((ipAddrBuf[ipAddressIndex +1]) == (macAddrBuf[macAddressIndex+1])) &&
         ((ipAddrBuf[ipAddressIndex +2]) == (macAddrBuf[macAddressIndex+2])))
      {
        rc=L7_SUCCESS;
      }
    }
  }
  else if(family == L7_AF_INET6)
  {
   /* TODO: update the correct conditions */
    rc=L7_SUCCESS;
  }

  return rc;
}

/******************************************************************************
* @purpose  Sets the forwarding mode for the given interface
*
* @param    family    @b{(input)}  family type
* @param    intIfNum  @b{(input)} Interface Number
* @param    mode      @b{(input)}  ENABLE/DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
* RPF_interface(S) is the interface the MRIB indicates would be used
* to route packets to S.
* RPF_interface(S) is zero if S is a local address.
*
* @end
*
******************************************************************************/
L7_RC_t mcastRtrIntfMcastFwdModeSet(L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;

  if(family == L7_AF_INET)
  {
    if (ipMapRtrIntfMcastFwdModeSet(intIfNum, mode) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
  }

  else if(family == L7_AF_INET6)
  {
    /*TODO - call the corresponding routing when the support for v6 is given.*/
    rc = L7_SUCCESS;
  }

  return rc;
}
/*********************************************************************
* @purpose  To Add/Delete the Multicast address in the hardware table.
*
* @param    familyType - L7_AF_INET/L7_AF_INET6
* @param    intIfNum   - Interface Number
* @param    inetAddr   - Address to be added
* @param    actionFlag - TRUE/FALSE
*
* @returns  mode     @b{(output)} L7_SUCCESS or L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t mcastLocalMulticastAddrUpdate(L7_uint32 familyType,L7_uint32 intIfNum,
                                      L7_inet_addr_t *inetAddr,L7_uchar8 actionFlag)
{
  L7_RC_t rc = L7_FAILURE;

  if(familyType == L7_AF_INET)
  {
    if(actionFlag == L7_TRUE)
    {
      rc = dtlIpv4LocalMulticastAddrAdd(intIfNum, osapiNtohl(inetAddr->addr.ipv4.s_addr));
    }
    else if(actionFlag == L7_FALSE)
    {
      rc = dtlIpv4LocalMulticastAddrDelete(intIfNum, osapiNtohl(inetAddr->addr.ipv4.s_addr));
    }
  }
  else if (familyType == L7_AF_INET6)
  {
    rc = mcastIp6LocalMulticastAddrUpdate(intIfNum, inetAddr, actionFlag);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_MCAST_MAP_COMPONENT_ID,
            "Invalid familyType, family = %d", familyType);
  }
  return rc;
}

/*********************************************************************
* @purpose  Is a local address
*
* @param    address  @b{(input)} inet address
* @param    rtrIfNum @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Takes care of unnumbered interfaces also
*
* @end
*
*********************************************************************/
L7_BOOL mcastIpMapUnnumberedIsLocalAddress(L7_inet_addr_t * address,
                                           L7_uint32 *rtrIfNum)
{
  L7_uint32 i;
  L7_uint32 intIfNum = 0;
  L7_uchar8 family;

  family = address->family;
  for (i = 1 ; i < MAX_INTERFACES; i++)
  {
    if (family == L7_AF_INET)
    {
      if ((ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS) &&
          (ipMapIntfIsUnnumbered(intIfNum) == L7_TRUE))
      {
        continue;
      }
    }
    if (inetIsLocalAddress(address, i) == L7_TRUE)
    {
      if (rtrIfNum != L7_NULLPTR )
      {
        *rtrIfNum = i;
      }
      return L7_TRUE;
    }
  }
  return(L7_FALSE);
}

/**********************************************************************
* @purpose  Checks given router interface is unnumbered or not.
*
* @param    family     @b{(input)} Ip family type
* @param    rtrIfnum   @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments  The family type is  included so that for IPv6 we need not
*            call IPMap ("the external component") and Unnumbered
*            interfaces is concept for IPv4 and all Ipv6 interfaces
*            are numbered.
*            This avoids the family checks for the callers.
*
* @end
**********************************************************************/
L7_BOOL mcastIpMapIsRtrIntfUnnumbered(L7_uchar8 family, L7_uint32 rtrIfNum)
{
  L7_uint32 intIfNum = 0;

  if (family != L7_AF_INET)
  {
    return L7_FALSE;
  }

  if (ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) == L7_SUCCESS)
  {
    return (ipMapIntfIsUnnumbered(intIfNum));
  }
  return L7_FALSE;
}
/**********************************************************************
* @purpose  Checks given router interface is unnumbered or not.
*
* @param    family     @b{(input)} Ip family type
* @param    intIfnum   @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments  The family type is  included so that for IPv6 we need not
*            call IPMap ("the external component") and Unnumbered
*            interfaces is concept for IPv4 and all Ipv6 interfaces
*            are numbered.
*            This avoids the family checks for the callers.
*
* @end
**********************************************************************/
L7_BOOL mcastIpMapIsIntfUnnumbered(L7_uchar8 family, L7_uint32 intIfNum)
{
  if (family != L7_AF_INET)
  {
    return L7_FALSE;
  }

  return (ipMapIntfIsUnnumbered(intIfNum));
}

/*********************************************************************
* @purpose
*
* @param    address  @b{(input)} inet address
* @param    rtrIfNum @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Takes care of unnumbered interfaces also
*
* @end
*
*********************************************************************/
L7_RC_t mcastRPFNeighborGet(L7_inet_addr_t *srcAddr,  mcastRPFInfo_t *rpfRouteInfo,
                        L7_inet_addr_t *nbrAddr,  mrpCallback IsNbrPresent)
{
  L7_uint32 routeNextHopIp;
  L7_uint32 rtrIfNum =0, i;
  L7_uint32 intIfNum =0;
  L7_uchar8 family;
  L7_inet_addr_t tempAddr;
  L7_routeEntry_t rtoRoute;
  L7_uint32 localAddr =0,routeMask =0;
  L7_inet_addr_t subnetMask;
  L7_uchar8 maskLen=0;


  if((L7_NULLPTR == srcAddr) || (L7_NULLPTR == nbrAddr))
  {
    return L7_FAILURE;
  }
  family = srcAddr->family;

  if(family == L7_AF_INET)
  {
    memset(&rtoRoute,0,sizeof(L7_routeEntry_t));
    if (rtoBestRouteLookup(srcAddr->addr.ipv4.s_addr,
                           &rtoRoute, L7_FALSE) == L7_SUCCESS)
    {
      for (i = 1; i < rtoRoute.ecmpRoutes.numOfRoutes; i++)
      {
          intIfNum = rtoRoute.ecmpRoutes.equalCostPath[i].arpEntry.intIfNum;
          routeNextHopIp =rtoRoute.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr;
          if (ipMapIntIfNumToRtrIntf(intIfNum,&rtrIfNum) != L7_SUCCESS) {
            return L7_FAILURE;
          }
          inetAddressSet(family, &routeNextHopIp, &tempAddr);
          if (IsNbrPresent(family,rtrIfNum, &tempAddr) == L7_FALSE)
          {
            continue;
          }
          inetCopy(nbrAddr,&tempAddr);
          if(ipMapRtrIntfIpAddressGet(intIfNum, &localAddr, &routeMask)
               != L7_SUCCESS)
          {
            return L7_FAILURE;
          }
          inetAddressSet(family, (void*)&rtoRoute.ipAddr,
                          &(rpfRouteInfo->rpfRouteAddress));
          rpfRouteInfo->rpfIfIndex = rtrIfNum;
          inetAddressSet(family, &routeNextHopIp, &(rpfRouteInfo->rpfNextHop));
          rpfRouteInfo->rpfRouteMetric =  rtoRoute.metric;
          rpfRouteInfo->rpfRouteMetricPref = rtoRoute.pref;
          rpfRouteInfo->rpfRouteProtocol = rtoRoute.protocol;
          inetAddressSet(L7_AF_INET,(void *)&rtoRoute.subnetMask,&subnetMask);
          inetMaskToMaskLen(&subnetMask,&maskLen);
          rpfRouteInfo->prefixLength = (L7_uint32 )maskLen;
          return L7_SUCCESS;;
      }
    }
  }
  else if(family == L7_AF_INET6)
  {
    return mcastV6RPFNeighborGet(&srcAddr->addr.ipv6,rpfRouteInfo, nbrAddr, IsNbrPresent);
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose
*
* @param
*
* @notes
*
* @end
*
*********************************************************************/
L7_uchar8* mcastIntfBitSetToString(interface_bitset_t *pOif,
        L7_uchar8 *pStr, L7_uint32 strSize)
{
  L7_uint32 index;
  L7_BOOL isEmpty = L7_TRUE;
#define MCAST_TEMP_BUF_SIZE 10
  L7_uchar8 tempBuff[MCAST_TEMP_BUF_SIZE];

  memset(pStr,0, strSize);
  for (index = 0; index <= sizeof(interface_bitset_t); index++)
  {
    if ((BITX_TEST (pOif, index)) !=L7_NULL)
    {
      osapiSnprintf(tempBuff, MCAST_TEMP_BUF_SIZE, "%d ", index);
      osapiStrncat(pStr, tempBuff, sizeof(tempBuff));
      isEmpty = L7_FALSE;
    }
  }
  if (isEmpty == L7_TRUE)
  {
    osapiStrncpy(pStr, "NULL", sizeof("NULL"));
  }
  return pStr;
}

/*********************************************************************
* @purpose  Find the nexthop interface for a IP address
*
* @param    ipAddr   @b{(input)}  Inet IP Address to resolve
* @param    rtrIfNum @b{(output)} Router interface number out of which address
*                                 is reachable
*
* @returns  L7_SUCCESS      no conflict
* @returns  L7_FAILURE      conflict with any of the above mentioned Ip
*                           addresses
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
mcastIpAddrRtrIntfResolve (L7_inet_addr_t *ipAddr,
                           L7_uint32 *rtrIfNum)
{
  L7_uchar8 addrFamily = 0;
  L7_uint32 intIfNum = 0;

  if ((ipAddr == L7_NULLPTR) || (rtrIfNum == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  addrFamily = L7_INET_GET_FAMILY(ipAddr);

  if (addrFamily == L7_AF_INET)
  {
    if (ipMapRouterIfResolve (ipAddr->addr.ipv4.s_addr, &intIfNum) != L7_SUCCESS)
    {
      *rtrIfNum = 0;
      return L7_FAILURE;
    }
  }
  else if (addrFamily == L7_AF_INET6)
  {
    if (mcastV6IpAddrIntfResolve (ipAddr, &intIfNum) != L7_SUCCESS)
    {
      *rtrIfNum = 0;
      return L7_FAILURE;
    }
  }
  else
  {
    *rtrIfNum = 0;
    return L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf (addrFamily, intIfNum, rtrIfNum) != L7_SUCCESS)
  {
    *rtrIfNum = 0;
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

