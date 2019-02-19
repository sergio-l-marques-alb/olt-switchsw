/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   mcast_v6_stub.c
*
* @purpose    interface v6 stub functions
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
  return L7_DISABLE;
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
  /* return if IPv6 package is not defined. */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get inet ipv6 addresses of interface
*
* @param    intIfNum    @b{(input)}  router interface
* @param    numAddr     @b{(input)} number of addresses: in max, out actual
* @param    pAddr       @b{(input)} ptr to array of addresses
* @param    inetAddr    @b{(input)} ptr to inet address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*
*********************************************************************/
L7_RC_t mcastIp6MapRtrIntfGlobalIpAddressGet(L7_uint32 intIfNum, 
                                       L7_inet_addr_t *inetAddr)
{
  /* return if IPv6 package is not defined. */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get inet ipv6 addresses of interface
*
* @param    intIfNum    @b{(input)}  router interface
* @param    numAddr     @b{(input)} number of addresses: in max, out actual
* @param    pAddr       @b{(input)} ptr to array of addresses
* @param    inetAddr    @b{(input)} ptr to inet address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*
*********************************************************************/
L7_RC_t mcastIp6MapRtrIntfIpAddressGet(L7_uint32 intIfNum, 
                                       L7_inet_addr_t *inetAddr)
{
  /* return if IPv6 package is not defined. */
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get netmask of interface
*
* @param    intIfNum     @b{(input)} router interface
* @param    numAddr      @b{(input)}number of addresses: in max, out actual
* @param    pAddr        @b{(input)}ptr to array of addresses
* @param    netmask      @b{(input)}ptr to inet address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*
*********************************************************************/
L7_RC_t mcastIp6MapRtrIntfNetMaskGet(L7_uint32 intIfNum, L7_inet_addr_t *netMask)
{
  /* return if IPv6 package is not defined. */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Convert Internal Interface Number to Router Interface Number
*
* @param    intIfNum   @b{(input)}internal interface number
* @param    *rtrIntf   @b{(input)}pointer to router interface number
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
L7_RC_t mcastIp6MapIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32* rtrIfNum)
{
  /* return if IPv6 package is not defined. */
  return L7_FAILURE;
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
  /* return if IPv6 package is not defined. */
  return L7_FAILURE;
}



/******************************************************************************
* @purpose	This function is called to lookup for  the best route for the given address.
*
* @param     pIpAddr     @b{(input)}  
* @param     pRtoRoute     @b{(output)}   
*
* @returns	L7_SUCCESS or L7_FAILURE
*
* @comments 	 
*		
* @end
******************************************************************************/

L7_RC_t mcastV6BestRouteLookup(L7_in6_addr_t *pIpAddr, 
                               mcastRPFInfo_t *pRtoRoute)
{

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Determine if specified IP Address lies in the local subnet
*           for the given interface
*
* @param    intIfNum      Internal interface number
* @param    ipAddr        IP Address to be checked
*
* @returns  L7_TRUE     if source address is local
* @returns  L7_FALSE    if source address is non-local
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL mcastIp6MapIsLocalAddr(L7_uint32 intIfNum, L7_in6_addr_t *ipAddr)
{
	/* return if IPv6 package is not defined. */
	return L7_FALSE;
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
	/* return if IPv6 package is not defined. */
	return L7_FAILURE;
}

/*********************************************************************
* @purpose  creates mask from prefixlen
*
* @param    ip6Addr          IP6 address
* @param    prefix_len       mask length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mcastIp6MapPrefixLenToMask(L7_in6_addr_t *mask, L7_uint32 prefix_len)
{
	/* return if IPv6 package is not defined. */
	return L7_FAILURE;
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
  /* return if IPv6 package is not defined. */
  return L7_FAILURE;
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
  /* return if IPv6 package is not defined. */
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
  /* return if IPv6 package is not defined. */
  return L7_FAILURE;
}

