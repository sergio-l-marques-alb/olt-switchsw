/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   mcast_inet.c
*
* @purpose    layer 3 address defs  APIS
*
*
* @comments   
*
* @create     01/16/2006
*
* @author     Satya Dillikar
* @end
*
**********************************************************************/
#include <stdio.h>
#include "osapi.h"
#include "osapi_support.h"
#include "l7_ip_api.h"
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "mcast_wrap.h"
#include "mcast_v6.h"
#include "mcast_inet.h"

#define INET_IPV4_INADDR_ALL_PIM_ROUTERS  (L7_uint32)0xe000000D      
                                                        /* 224.0.0.13 */
#define INET_IPV6_INADDR_ALL_PIM_ROUTERS  "ff02::0d"      

#define INET_IPV4_INADDR_ALL_IGMPv3_ROUTERS   (L7_uint32)0xe0000016
#define INET_IPV6_INADDR_ALL_IGMPv3_ROUTERS   "ff02::16"

#define INET_IPV6_ALL_HOSTS_ADDR        "ff02::01"
#define INET_IPV6_ALL_ROUTERS_ADDR      "ff02::02"

#define INET_IPV4_ALL_MCAST_GROUPS_MASK   4
#define INET_IPV6_ALL_MCAST_GROUPS_MASK   8

/*********************************************************************
* @purpose  Is a local address
*
* @param    destAddr @b{(input)} Destination address
* @param    rtrIfNum @b{(input)} RouterInterfaceNumber
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @notes    
*           
*       
* @end
*
*********************************************************************/
L7_BOOL inetIsLocalAddress(L7_inet_addr_t *destAddr, L7_uint32 rtrIfNum)
{
    L7_uint32 intIfNum;
    L7_RC_t rc;
    
    if ( destAddr == L7_NULLPTR ) 
    {
        return L7_FALSE;
    }

    if ( destAddr->family == L7_AF_INET )
    {
      rc = ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum);
      if ( rc != L7_SUCCESS )
      {
          return L7_FALSE;
      }
      return ipMapIpAddrMatchesConfigured(intIfNum,destAddr->addr.ipv4.s_addr);
    } 
    else if (destAddr->family == L7_AF_INET6) 
    {
      rc = mcastIp6MapRtrIntfToIntIfNum(rtrIfNum, &intIfNum);
      if (rc != L7_SUCCESS)
      {
        return L7_FALSE;
      }
/*This check is not required as it returns true for any link local address(fe80)  .This will not give the required result.*/

 #if 0
      if(L7_IP6_IS_ADDR_LINK_LOCAL(&destAddr->addr.ipv6))
      {
        return L7_TRUE;
      }
 #endif
      return mcastIp6MapIsLocalAddr(intIfNum,&(destAddr->addr.ipv6));
    }
    return L7_FALSE; 
}

/*********************************************************************
* @purpose  Get subnet for a IPv6 packet
*
* @param    IPv6Addr       @b{(input)} Ipv6 address
* @param    subnet Mask    @b{(input)} Subnet Mask
* @param    Subnet Address @b{(input)} Subnet Address
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes    
*           
* @end
*
*********************************************************************/
static L7_RC_t inetV6SubnetAddrGet(L7_in6_addr_t *pAddr,L7_in6_addr_t *pMask,
                                   L7_in6_addr_t *pSubnetAddr)
{
  if((pAddr == L7_NULLPTR) || (pMask == L7_NULLPTR) || 
      (pSubnetAddr == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  pSubnetAddr->in6.addr32[0] = pAddr->in6.addr32[0] & pMask->in6.addr32[0];  
  pSubnetAddr->in6.addr32[1] = pAddr->in6.addr32[1] & pMask->in6.addr32[1];  
  pSubnetAddr->in6.addr32[2] = pAddr->in6.addr32[2] & pMask->in6.addr32[2];  
  pSubnetAddr->in6.addr32[3] = pAddr->in6.addr32[3] & pMask->in6.addr32[3];
  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  Is a directly connected address
*
* @param    destAddr @b{(input)} Destination address
* @param    rtrIfNum @b{(input)} RouterInterfaceNumber
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @notes    
*           
* @end
*
*********************************************************************/
L7_BOOL inetIsDirectlyConnected(L7_inet_addr_t *destAddr, L7_uint32 rtrIfNum)
{
    L7_uint32   intIfNum, destIpAddr;
    L7_RC_t rc;
    L7_in6_addr_t localAddr;
    L7_in6_addr_t destIpv6Addr;
    L7_ipv6IntfAddr_t ip6Address[L7_RTR6_MAX_INTF_ADDRS];
    L7_in6_addr_t     in6NetMask;
    L7_uint32         numAddr = L7_RTR6_MAX_INTF_ADDRS;
    L7_uint32       j;
    
    if ( destAddr == L7_NULLPTR ) 
    {
        return L7_FALSE;
    }


    if (destAddr->family == L7_AF_INET) 
    {
        destIpAddr = destAddr->addr.ipv4.s_addr;
        rc = ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum);
        if ( rc != L7_SUCCESS )
        {
            return L7_FALSE;
        }
        return ipMapIpAddrIsLocal(intIfNum,destIpAddr);
    } 
    else if (destAddr->family == L7_AF_INET6) 
    {
      if(L7_IP6_IS_ADDR_LINK_LOCAL(&destAddr->addr.ipv6))
      {
        return L7_TRUE;
      }
      if (mcastIp6MapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) != L7_SUCCESS)
      {
        return L7_FALSE;
      }
      
      /* call Ipv6map function to get the address for specifed interface */
      if (mcastIp6MapRtrIntfAddressesGet(intIfNum, &numAddr, ip6Address) != L7_SUCCESS)
      {
        return L7_FALSE;
      }
      for (j=0; j <numAddr; j++)
      {
  
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&ip6Address[j].ip6Addr))
        {
          continue;
        }
       
        mcastIp6MapPrefixLenToMask(&in6NetMask, ip6Address[j].ip6PrefixLen);
  
        memset(&localAddr,0,sizeof(L7_in6_addr_t));
        memset(&destIpv6Addr,0,sizeof(L7_in6_addr_t));
        
        if(inetV6SubnetAddrGet(&ip6Address[j].ip6Addr,&in6NetMask,
                               &localAddr) 
                                 != L7_SUCCESS)
        {
          continue;
        }
        if(inetV6SubnetAddrGet(&destAddr->addr.ipv6,&in6NetMask,
                               &destIpv6Addr) 
                                 != L7_SUCCESS)
        {
          continue;
        }
        if(L7_IP6_IS_ADDR_EQUAL(&localAddr,&destIpv6Addr))
        {
          return L7_TRUE;
        }
      }
    }
    return L7_FALSE; 
}

/*********************************************************************
* @purpose  Set addr to ALL_MCAST_GROUPS_ADDR
*
* @param   family @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param	 addr  @b{(input)} InetAddress
*
* @returns   L7_FAILURE 
* @returns   L7_SUCCESS
*
* @notes    
*           
* @end
*********************************************************************/
L7_RC_t inetAllMcastGroupAddressInit(L7_uchar8 family, L7_inet_addr_t *addr)
{
  L7_in6_addr_t Ipv6Addr;
    if (addr == L7_NULLPTR) 
    {
        return L7_FAILURE;
    }
    inetAddressReset(addr); 
    if (family == L7_AF_INET) 
    {
        addr->family = L7_AF_INET;
        addr->addr.ipv4.s_addr = INET_IPV4_ALL_MCAST_GROUPS_ADDR;
    } else if (family == L7_AF_INET6) 
    {
        addr->family = L7_AF_INET6;
        memset(&Ipv6Addr,0,sizeof(L7_in6_addr_t));
        osapiInetPton(L7_AF_INET6,INET_IPV6_ALL_MCAST_GROUPS_ADDR,
                       (L7_uchar8 *)&Ipv6Addr);
        memcpy(&addr->addr.ipv6,&Ipv6Addr,sizeof(L7_in6_addr_t));
        
    }
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Set mask to ALL_MCAST_GROUPS_ADDR
*
* @param   family @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param   addr  @b{(input)} Pointer to MaskLen
*
* @returns   L7_FAILURE 
* @returns   L7_SUCCESS
*
* @notes   The same functionality is present in  pimsmStarStarRpMaskLenGet
*          which has to be moved to this function.
*           
* @end
*********************************************************************/
L7_RC_t inetAllMcastGroupPrefixLenInit(L7_uchar8 family, L7_uchar8  *pMaskLen)
{
  if(L7_NULLPTR == pMaskLen)
  {
    return L7_FAILURE;
  }
  if(family == L7_AF_INET)
  {
    *pMaskLen = INET_IPV4_ALL_MCAST_GROUPS_MASK;
  }
  else if(family == L7_AF_INET6)
  {
    *pMaskLen = INET_IPV6_ALL_MCAST_GROUPS_MASK;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Test addr to match ALL_MCAST_GROUPS_ADDR
*
* @param    addr @b{(input)} InetAddress
*
* @returns   L7_TRUE  
* @returns   L7_FALSE
*
* @notes    
*           
* @end
*********************************************************************/
L7_BOOL inetIsAllMcastGroupAddress(L7_inet_addr_t *addr)
{
    L7_in6_addr_t Ipv6Addr;
	
    if (addr == L7_NULLPTR) 
    {
        return L7_FALSE;
    }
    if (addr->family == L7_AF_INET) 
    {
        if (addr->addr.ipv4.s_addr == INET_IPV4_ALL_MCAST_GROUPS_ADDR) 
        {
            return L7_TRUE;
        }
    } 
    else if (addr->family == L7_AF_INET6) 
    {
      memset(&Ipv6Addr,9,sizeof(L7_in6_addr_t));
      osapiInetPton(L7_AF_INET6,INET_IPV6_ALL_MCAST_GROUPS_ADDR,
        (L7_uchar8 *)&Ipv6Addr);
      if(L7_IP6_IS_ADDR_EQUAL(&addr->addr.ipv6,&Ipv6Addr))
      {
        return L7_TRUE;
      }
      /*
        if(L7_IP6_IS_ADDR_MULTICAST(&addr->addr.ipv6))
          return L7_TRUE; */
    }
    return L7_FALSE;
}
/*********************************************************************
* @purpose  Test addr to match ALL_MCAST_GROUPS_PREFIX_LEN
*
* @param    mask @b{(input)} prefix length
*
* @returns   L7_TRUE  
* @returns   L7_FALSE
*
* @notes    
*           
* @end
*********************************************************************/
L7_BOOL inetIsAllMcastGroupPrefixLen(L7_uchar8 family,L7_uchar8 mask)
{
  if (family == L7_AF_INET) 
  {
    if (mask == INET_IPV4_ALL_MCAST_GROUPS_MASK) 
    {
      return L7_TRUE;
    }
  }
  else if (family == L7_AF_INET6) 
  {
    if (mask == INET_IPV6_ALL_MCAST_GROUPS_MASK)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Test addr to match INADDR_ALL_PIM_ROUTERS
*
* @param    addr @b{(input)} InetAddress
*
* @returns   L7_TRUE 
* @returns   L7_FALSE
*
* @notes    
*           
* @end
*
*********************************************************************/
L7_BOOL inetIsAllPimRouterAddress(L7_inet_addr_t *addr)
{
  L7_in6_addr_t Ipv6Addr;

  if (addr == L7_NULLPTR) 
  {
    return L7_FALSE;
  }
  if (addr->family == L7_AF_INET) 
  {
    if (addr->addr.ipv4.s_addr == INET_IPV4_INADDR_ALL_PIM_ROUTERS) 
    {
      return L7_TRUE;
    }
  } 
  else if (addr->family == L7_AF_INET6) 
  {
    memset(&Ipv6Addr,9,sizeof(L7_in6_addr_t));
    osapiInetPton(L7_AF_INET6,INET_IPV6_INADDR_ALL_PIM_ROUTERS,
        (L7_uchar8 *)&Ipv6Addr);
    if(L7_IP6_IS_ADDR_EQUAL(&addr->addr.ipv6,&Ipv6Addr))
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Set addr to INADDR_ALL_PIM_ROUTERS
*
* @param   family @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param   addr   @b{(input)} InetAddress
*
* @returns   L7_FAILURE 
* @returns   L7_SUCCESS
*
* @notes    
*           
* @end
*
*********************************************************************/
L7_RC_t inetAllPimRouterAddressInit(L7_uchar8 family, L7_inet_addr_t *addr)
{
  L7_in6_addr_t Ipv6Addr;
    if (addr == L7_NULLPTR) 
    {
        return L7_FAILURE;
    }
    if (family == L7_AF_INET) 
    {
        addr->family = L7_AF_INET;
        addr->addr.ipv4.s_addr = osapiNtohl(INET_IPV4_INADDR_ALL_PIM_ROUTERS);
    } 
    else if (family == L7_AF_INET6) 
    {
        memset(&Ipv6Addr,0,sizeof(L7_in6_addr_t));
        osapiInetPton(L7_AF_INET6,INET_IPV6_INADDR_ALL_PIM_ROUTERS,
                      (L7_uchar8 *)&Ipv6Addr);
        addr->family = L7_AF_INET6;
        memcpy(&addr->addr.ipv6,&Ipv6Addr,sizeof(L7_in6_addr_t));
    }
    return L7_SUCCESS;
}

/********************************************************************
* @purpose  Convert the ip address into a digest value
*
* @param    addr @b{(input)} inetaddress
*
* @returns  Digest value.
*
* @notes    
*           
* @end
*
*********************************************************************/
L7_uint32 inetPimsmDigestGet(L7_inet_addr_t *addr)
{
  if(addr == L7_NULL)
    return 0;

  if(addr->family == L7_AF_INET)
    return addr->addr.ipv4.s_addr;

  if(addr->family == L7_AF_INET6)
    return ( osapiNtohl(addr->addr.ipv6.in6.addr32[0]) ^ osapiNtohl(addr->addr.ipv6.in6.addr32[1]) ^
             osapiNtohl(addr->addr.ipv6.in6.addr32[2]) ^ osapiNtohl(addr->addr.ipv6.in6.addr32[3]) );
  return 0;
}
/*********************************************************************
* @purpose  Test addr to match INADDR_ALL_IGMPv3_ROUTERS
*
* @param    addr @b{(input)} InetAddress
*
* @returns   L7_TRUE 
* @returns   L7_FALSE
*
* @notes    
*           
* @end
*
*********************************************************************/
L7_BOOL inetIsAddressIgmpv3Routers(L7_inet_addr_t *addr)
{
  L7_in6_addr_t Ipv6Addr;

  if (addr == L7_NULLPTR)
  {
    return L7_FALSE;
  }
  if (addr->family == L7_AF_INET)
  {
    if (addr->addr.ipv4.s_addr == INET_IPV4_INADDR_ALL_IGMPv3_ROUTERS)
    {
      return L7_TRUE;
    }
  }
  else if (addr->family == L7_AF_INET6)
  {
    memset(&Ipv6Addr,9,sizeof(L7_in6_addr_t));
    osapiInetPton(L7_AF_INET6,INET_IPV6_INADDR_ALL_IGMPv3_ROUTERS,
                  (L7_uchar8 *)&Ipv6Addr);
    if (L7_IP6_IS_ADDR_EQUAL(&addr->addr.ipv6,&Ipv6Addr))
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;

}
/*********************************************************************
* @purpose  Set addr to INADDR_ALL_IGMPv3_ROUTERS
*
* @param   family @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param   addr   @b{(input)} InetAddress
*
* @returns   L7_FAILURE 
* @returns   L7_SUCCESS
*
* @notes    
*           
* @end
*
*********************************************************************/
L7_RC_t inetIgmpv3RouterAddressInit(L7_uchar8 family, L7_inet_addr_t *addr)
{
  L7_in6_addr_t Ipv6Addr;
  if (addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (family == L7_AF_INET)
  {
    addr->family = L7_AF_INET;
    addr->addr.ipv4.s_addr = osapiNtohl(INET_IPV4_INADDR_ALL_IGMPv3_ROUTERS);
  }
  else if (family == L7_AF_INET6)
  {
    memset(&Ipv6Addr,0,sizeof(L7_in6_addr_t));
    osapiInetPton(L7_AF_INET6,INET_IPV6_INADDR_ALL_IGMPv3_ROUTERS,
                  (L7_uchar8 *)&Ipv6Addr);
    addr->family = L7_AF_INET6;
    memcpy(&addr->addr.ipv6,&Ipv6Addr,sizeof(L7_in6_addr_t));
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  set addr to L7_IP_ALL_HOSTS_ADDR
*
* @param    family  @b{ (input) } Family Type
* @param    addr    @b{ (input) } All Hosts Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*           
*       
* @end
*********************************************************************/
L7_RC_t inetAllHostsAddressInit(L7_uchar8 family, L7_inet_addr_t *addr)
{
  L7_in6_addr_t Ipv6Addr;
  if (addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  inetAddressReset(addr); 
  if (family == L7_AF_INET)
  {
    addr->family = L7_AF_INET;
    addr->addr.ipv4.s_addr = L7_IP_ALL_HOSTS_ADDR;
  }
  else if (family == L7_AF_INET6)
  {
    memset(&Ipv6Addr,0,sizeof(L7_in6_addr_t));
    osapiInetPton(L7_AF_INET6,INET_IPV6_ALL_HOSTS_ADDR,
                  (L7_uchar8 *)&Ipv6Addr);
    addr->family = L7_AF_INET6;
    memcpy(&addr->addr.ipv6,&Ipv6Addr,sizeof(L7_in6_addr_t));
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  set addr to L7_IP_ALL_RTRS_ADDR
*
* @param    family  @b{ (input) } Family Type
* @param    addr    @b{ (input) } All Routers Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*           
*       
* @end
*********************************************************************/
L7_RC_t inetAllRoutersAddressInit(L7_uchar8 family, L7_inet_addr_t *addr)
{
  L7_in6_addr_t Ipv6Addr;
  if (addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  inetAddressReset(addr); 
  if (family == L7_AF_INET)
  {
    addr->family = L7_AF_INET;
    addr->addr.ipv4.s_addr = L7_IP_ALL_RTRS_ADDR;
  }
  else if (family == L7_AF_INET6)
  {
    memset(&Ipv6Addr,0,sizeof(L7_in6_addr_t));
    osapiInetPton(L7_AF_INET6,INET_IPV6_ALL_ROUTERS_ADDR,
                  (L7_uchar8 *)&Ipv6Addr);
    addr->family = L7_AF_INET6;
    memcpy(&addr->addr.ipv6,&Ipv6Addr,sizeof(L7_in6_addr_t));
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Checks given group is in admin-scope range or not.
*
* @param    mode       @b{(output)} admin mode
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
**********************************************************************/
L7_BOOL mcastIsAddrAdminScopeRange(L7_inet_addr_t *grpAddr)
{
  if ((grpAddr->addr.ipv4.s_addr < L7_MCAST_ADMINSCOPE_ADDR_MIN) ||
      (grpAddr->addr.ipv4.s_addr > L7_MCAST_ADMINSCOPE_ADDR_MAX))
  {
    return L7_FALSE;
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Is a local address
*
* @param    destAddr @b{(input)} Destination address
* @param    rtrIfNum @b{(input)} RouterInterfaceNumber
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @notes    Takes care of unnumbered interfaces also   
*           
*       
* @end
*
*********************************************************************/
L7_BOOL inetUnNumberedIsLocalAddress(L7_inet_addr_t *destAddr, L7_uint32 rtrIfNum)
{
  L7_uint32 intIfNum, addr = 0; 
  L7_inet_addr_t  borrowedAddr;
   
  if ( destAddr == L7_NULLPTR )
  {
    return L7_FALSE;
  }
  if ((destAddr->family == L7_AF_INET) &&
      (ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) == L7_SUCCESS) &&
      (ipMapIntfIsUnnumbered(intIfNum) == L7_TRUE))
  {
     addr = ipMapBorrowedAddr(intIfNum);
     inetAddressSet(destAddr->family, &addr, &borrowedAddr);
     if (L7_INET_ADDR_COMPARE( destAddr, &borrowedAddr) == L7_NULL)
     {
       return L7_TRUE;
     }
     else
     {
       return L7_FALSE;
     }
  }
  else
  {
    return inetIsLocalAddress(destAddr, rtrIfNum);
  }
  return L7_FALSE;
}



