/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   mcast_v6_util.c
*
* @purpose    interface v6 utility functions
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
#include "mcast_v6.h"
#include "l7_ip6_api.h"
#include "l3_addrdefs.h"
#include "dtl_l3_api.h"
#include "log.h"
#include "sysapi.h"
#include "l3_mcast_commdefs.h"
#include <string.h> /* memset()  */

/*********************************************************************
* @purpose  Get ipv6 routing admin mode
*
* @returns  routing-v6 admin mode
*
* @end
*
*********************************************************************/
L7_uint32 mcastIp6MapRtrAdminModeGet()
{
  return ip6MapRtrAdminModeGet();
}
                    
/*********************************************************************
* @purpose  Get inet ipv6 addresses of interface
*
* @param    intIfNum @b{(input)}    router interface
* @param    inetAddr @b{(input)}    ptr to inet address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*
*********************************************************************/
L7_RC_t mcastIp6MapRtrIntfIpAddrListGet(L7_uint32 intIfNum, L7_uint32 *numAddr,
                                       L7_inet_addr_t *ipAddrList,
                                       L7_uint32 *ipPrefixLen)
{
  L7_ipv6IntfAddr_t ip6Address[L7_MCAST_MAX_INTF_ADDRS];
  L7_uint32 j;
  L7_inet_addr_t ipAddr;

  *numAddr = L7_MCAST_MAX_INTF_ADDRS;
  /* call Ipv6map function to get the address for specifed interface */
  if (ip6MapRtrIntfAddressesGet(intIfNum, numAddr, ip6Address) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

   if (ipAddrList == L7_NULLPTR) 
   {
     return L7_FAILURE;
   }
  for (j=0; j <*numAddr; j++)
  {

    if (L7_IP6_IS_ADDR_UNSPECIFIED(&ip6Address[j].ip6Addr))
    {
      if ((ipAddrList+j) != L7_NULLPTR) 
      {
         inetAddressZeroSet(L7_AF_INET6, (ipAddrList+j));
         ipPrefixLen[j] = 0;
      }
     continue;
    }
    if (inetAddressSet(L7_AF_INET6,  &ip6Address[j].ip6Addr, &ipAddr) 
          != L7_SUCCESS) 
     {
         continue;
     }
     if ((ipAddrList+j) != L7_NULLPTR) 
     {
         inetCopy((ipAddrList+j), &ipAddr);
         ipPrefixLen[j] = ip6Address[j].ip6PrefixLen;
     }
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get inet global ipv6 address of interface
*
* @param    intIfNum    @b{(input)}  router interface
* @param    inetAddr    @b{(input)}  ptr to inet address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*
*********************************************************************/
L7_RC_t mcastIp6MapRtrIntfGlobalIpAddressGet(L7_uint32 intIfNum, 
                                       L7_inet_addr_t *inetAddr)
{
  L7_ipv6IntfAddr_t ip6Address[L7_RTR6_MAX_INTF_ADDRS];
  L7_uint32       j,  numAddr = L7_RTR6_MAX_INTF_ADDRS;
  L7_RC_t rc = L7_FAILURE;
  
  /* call Ipv6map function to get the address for specifed interface */
  if (ip6MapRtrIntfAddressesGet(intIfNum, &numAddr, ip6Address) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  for (j=1; j <numAddr; j++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&ip6Address[j].ip6Addr))
    {
      continue;
    }
    rc = inetAddressSet(L7_AF_INET6, &(ip6Address[j].ip6Addr), inetAddr);
    break;
  }
  return rc;

}

/*********************************************************************
* @purpose  Get inet link local ipv6 address of interface
*
* @param    intIfNum    @b{(input)}  router interface
* @param    inetAddr    @b{(input)}  ptr to inet address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*
*********************************************************************/
L7_RC_t mcastIp6MapRtrIntfIpAddressGet(L7_uint32 intIfNum, 
                                       L7_inet_addr_t *inetAddr)
{
  L7_ipv6IntfAddr_t ip6Address;
  L7_uint32         numAddr = 1;

  /* call Ipv6map function to get the address for specifed interface */
  if (ip6MapRtrIntfAddressesGet(intIfNum, &numAddr, &ip6Address) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (inetAddressSet(L7_AF_INET6, &(ip6Address.ip6Addr), inetAddr)!= L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
  
}

/*********************************************************************
* @purpose  Get netmask of interface
*
* @param    intIfNum  @b{(input)}  router interface
* @param    netmask   @b{(input)}  ptr to inet address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*
*********************************************************************/
L7_RC_t mcastIp6MapRtrIntfNetMaskGet(L7_uint32 intIfNum, L7_inet_addr_t *netMask)
{
  L7_ipv6IntfAddr_t ip6Address;
  L7_in6_addr_t     in6NetMask;
  L7_uint32         numAddr = 1;

  /* call Ipv6map function to get the address for specifed interface */
  if (ip6MapRtrIntfAddressesGet(intIfNum, &numAddr, &ip6Address) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ip6MapPrefixLenToMask(&in6NetMask, ip6Address.ip6PrefixLen);

  if (inetAddressSet(L7_AF_INET6, &in6NetMask, netMask)!= L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert Internal Interface Number to Router Interface Number
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    rtrIntf   @b{(input)} pointer to router interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
*
* @end
*
*********************************************************************/
L7_RC_t mcastIp6MapIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32* rtrIfNum)
{
  if (ip6MapIntIfNumToRtrIntf(intIfNum, rtrIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert Router If Number to Internal Interface Number
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *rtrIntf  @b{(input)} pointer to router interface number
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
L7_RC_t mcastIp6MapRtrIntfToIntIfNum(L7_uint32 rtrIfNum, L7_uint32* intIfNum)
{
  if (ip6MapRtrIntfToIntIfNum(rtrIfNum, intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  This function is called to lookup for  the best route for the given address.
*
* @param     pIpAddr       @b{(input)}inetaddress  
* @param     pRtoRoute     @b{(input)}pointer to structure mcastRPFInfo_t   
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t mcastV6BestRouteLookup(L7_in6_addr_t *pIpAddr, 
                               mcastRPFInfo_t *pRtoRoute)
{
  L7_route6Entry_t bestV6Route;
  L7_uint32 rtrIfNum =0,routeIntIntf;
  if((L7_NULLPTR == pIpAddr) || (L7_NULLPTR == pRtoRoute))
  {
    return L7_FAILURE;
  }

  memset(&bestV6Route,0,sizeof(L7_route6Entry_t));
  if(rto6BestRouteLookup(pIpAddr,&bestV6Route, L7_FALSE) == L7_SUCCESS)
  {
    routeIntIntf = bestV6Route.ecmpRoutes.equalCostPath[0].intIfNum;
    pRtoRoute->rpfRouteMetric = bestV6Route.metric;
    pRtoRoute->rpfRouteMetricPref = bestV6Route.pref;
    pRtoRoute->rpfRouteProtocol = bestV6Route.protocol;
    pRtoRoute->prefixLength= bestV6Route.ip6PrefixLen;
    if(mcastIp6MapIntIfNumToRtrIntf(routeIntIntf, &rtrIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    inetAddressSet(L7_AF_INET6, (void*)&bestV6Route.ip6Addr, 
                      &pRtoRoute->rpfRouteAddress);

    inetAddressSet(L7_AF_INET6,
             (void *)&bestV6Route.ecmpRoutes.equalCostPath[0].ip6Addr,
              &pRtoRoute->rpfNextHop);
      
    pRtoRoute->rpfIfIndex = rtrIfNum;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine if specified IP Address lies in the local subnet
*           for the given interface
*
* @param    intIfNum      Internal interface number
* @param    ipAddr        IP Address to be checked
*
* @returns  L7_TRUE    The IP Address lies in the local subnet
* @returns  L7_FALSE   The IP Address does not lie in the local subnet
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL mcastIp6MapIsLocalAddr(L7_uint32 intIfNum, L7_in6_addr_t *ipAddr)
{
  return (ip6MapIsLocalAddr(intIfNum, ipAddr));
}

/*********************************************************************
* @purpose  Get addresses of interface
*
* @param    rtrIntf      router interface
*           numAddr      number of addresses: in max, out actual
*           pAddr        ptr to array of addresses
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t mcastIp6MapRtrIntfAddressesGet(L7_uint32 intIfNum, L7_uint32 *numAddr, L7_ipv6IntfAddr_t *pAddr)
{
    if (ip6MapRtrIntfAddressesGet(intIfNum, numAddr, pAddr) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  creates mask from prefixlen
*
* @param    ip6Addr          IP6 address
* @param    prefix_len       mask length
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mcastIp6MapPrefixLenToMask(L7_in6_addr_t *mask, L7_uint32 prefix_len)
{
    ip6MapPrefixLenToMask(mask, prefix_len);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Add/Delete the Multicast address in the hardware table.
*
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
L7_RC_t mcastIp6LocalMulticastAddrUpdate(L7_uint32 intIfNum,
                                      L7_inet_addr_t *inetAddr,L7_uchar8 actionFlag)
{
  L7_RC_t rc = L7_FAILURE;

  if (actionFlag == L7_TRUE)
  {
    rc = dtlIpv6LocalMulticastAddrAdd(intIfNum, &(inetAddr->addr.ipv6));
  }
  else if (actionFlag == L7_FALSE)
  {
    rc = dtlIpv6LocalMulticastAddrDelete(intIfNum, &(inetAddr->addr.ipv6));
  } 
  return rc;
}


/******************************************************************************
* @purpose  This function is called to lookup for  the best route for the given address.
*
* @param     pIpAddr       @b{(input)}inetaddress  
* @param     pRtoRoute     @b{(input)}pointer to structure mcastRPFInfo_t   
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t mcastV6RPFNeighborGet(L7_in6_addr_t *pIpAddr, mcastRPFInfo_t *rpfRouteInfo,
                              L7_inet_addr_t *nbrAddr,  mrpCallback IsNbrPresent)
{
  L7_route6Entry_t bestV6Route;
  L7_uint32 rtrIfNum =0,routeIntIntf;
  L7_uint32   i; 
  L7_inet_addr_t tempAddr;  
  if((L7_NULLPTR == pIpAddr) || (L7_NULLPTR == nbrAddr))
  {
    return L7_FAILURE;
  }

  memset(&bestV6Route,0,sizeof(L7_route6Entry_t));
  if(rto6BestRouteLookup(pIpAddr,&bestV6Route, L7_FALSE) == L7_SUCCESS)
  {
    for (i = 1; i < bestV6Route.ecmpRoutes.numOfRoutes; i++)
    {  
        routeIntIntf = bestV6Route.ecmpRoutes.equalCostPath[i].intIfNum;
        if(mcastIp6MapIntIfNumToRtrIntf(routeIntIntf, &rtrIfNum) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        inetAddressSet(L7_AF_INET6,
                 (void *)&bestV6Route.ecmpRoutes.equalCostPath[i].ip6Addr,
                  &tempAddr);
        if (IsNbrPresent(L7_AF_INET6,rtrIfNum, &tempAddr) == L7_FALSE)
        {
           continue;
        }
        inetCopy(nbrAddr,&tempAddr);
        rpfRouteInfo->rpfRouteMetric = bestV6Route.metric;
        rpfRouteInfo->rpfRouteMetricPref = bestV6Route.pref;
        rpfRouteInfo->rpfRouteProtocol = bestV6Route.protocol;
        rpfRouteInfo->prefixLength= bestV6Route.ip6PrefixLen;
        inetAddressSet(L7_AF_INET6, (void*)&bestV6Route.ip6Addr, 
                          &rpfRouteInfo->rpfRouteAddress);
        inetAddressSet(L7_AF_INET6,
                 (void *)&bestV6Route.ecmpRoutes.equalCostPath[i].ip6Addr,
                  &rpfRouteInfo->rpfNextHop);
        rpfRouteInfo->rpfIfIndex = rtrIfNum;        
        return L7_SUCCESS;;
     }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find the nexthop interface for a global address
*
* @param    ipAddr   @b{(input)}  Inet IP Address to resolve
* @param    intIfNum @b{(output)} Internal interface number out of which address
*                                 is reachable
*
* @returns  L7_SUCCESS      no conflict
* @returns  L7_FAILURE      conflict with any of the above mentioned ip6
*                           addresses
*
* @notes
*       
* @end
*
*********************************************************************/
L7_RC_t
mcastV6IpAddrIntfResolve (L7_inet_addr_t *ipAddr,
                          L7_uint32 *intIfNum)
{
  if ((ipAddr == L7_NULLPTR) || (intIfNum == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (ip6MapRouterIfResolve (&ipAddr->addr.ipv6, intIfNum) != L7_SUCCESS)
  {
    *intIfNum = 0;
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

