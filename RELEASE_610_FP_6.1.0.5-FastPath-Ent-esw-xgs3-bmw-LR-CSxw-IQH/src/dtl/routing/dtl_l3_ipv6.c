/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename   dtl_l3_ipv6.c
 *
 * @purpose    Dtl IPv6 layer3 functions
 *
 * @component  Device Transformation Layer
 *
 * @comments   none
 *
 * @create     03/18/2000
 *
 * @author     elund
 * @end
 *
 **********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "l3_comm_structs.h"
#include "dtlinclude.h"
#include "dtl_l3_api.h"
#include "l7_ip6_api.h"
#include "dtl_ip.h"
#include "l7_ipmap_arp_api.h"
#include "l7_rto6_api.h"
#include "l7_ip_api.h"

#include "l7_socket.h"

extern dtlInfo_t         dtlInfo;

/*
 * Static Utility functions.
 */
extern void ip6MapPrefixLenToMask(L7_in6_addr_t *mask, L7_uint32 prefix_len);

static L7_RC_t
dtlIpv6RouteEntryExtract(L7_route6Entry_t *pRoute,
                         DAPI_ROUTING_ROUTE_ENTRY_t *pRouteEntry)
{
  L7_uint32 i;
  nimUSP_t  nimUsp;
#if FUTURE_FUNC
  DAPI_ROUTING_ROUTE_DESTINATION_t *pEcrRow, *pEcr;
#endif

  /*  static const char *routine_name = "dtlIpv6RouteEntryExtract()"; */

  for (i=0; i < pRoute->ecmpRoutes.numOfRoutes; i++)
  {
    pRouteEntry->equalCostRoute[i].valid = L7_TRUE;
    memcpy(&pRouteEntry->equalCostRoute[i].ip6Addr,
           &pRoute->ecmpRoutes.equalCostPath[i].ip6Addr,
           sizeof(pRouteEntry->equalCostRoute[i].ip6Addr));
    if (nimGetUnitSlotPort(pRoute->ecmpRoutes.equalCostPath[i].intIfNum,
                           &nimUsp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    pRouteEntry->equalCostRoute[i].usp.unit = (L7_ushort16)nimUsp.unit;
    pRouteEntry->equalCostRoute[i].usp.slot = (L7_ushort16)nimUsp.slot;
    pRouteEntry->equalCostRoute[i].usp.port = (L7_ushort16)nimUsp.port - 1;
  }

  return L7_SUCCESS;
}

static DAPI_ROUTING_ROUTE_FLAG_t
dtlIpv6RouteToDapiFlags(L7_route6Entry_t *pRoute)
{
  L7_uint32 routeFlags;
  DAPI_ROUTING_ROUTE_FLAG_t dapiFlags = 0;

  routeFlags = pRoute->flags;

  if (routeFlags & L7_RTF_DEFAULT_ROUTE)
    dapiFlags |= DAPI_ROUTING_ROUTE_DEFAULT;

  if (routeFlags & L7_RTF_DISCARD)
    dapiFlags |= DAPI_ROUTING_ROUTE_DISCARD;

  /* NOTE: L7_RTF_REJECT routes get installed as
     cpu routes (due to no next hops) in order to
     generate icmp unreachable
  */

  return dapiFlags;
}

/*
 * Externs from here down.
 */

/*********************************************************************
 * @purpose  Enables or disables the routing function for IPv6,
 * @purpose  i.e. whether this entity has the ability to route IPv6
 * @purpose  or not.
 *
 * @param mode @b{(input)} L7_ENABLE, to enable forwarding of PDUs
 *                         in this router.
 *                         L7_DISABLE, to disable forwarding in this
 *                         router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes Forwarding - The indication of whether this entity is
 * @notes acting as an IP gateway in respect to the forwarding of PDUs
 * @notes received by, but not addressed to, this entity.  IP gateways
 * @notes forward PDUs.  IP hosts do not (except those source-routed via
 * @notes the host).
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6ForwardingModeSet(L7_uint32 mode)
{
  L7_RC_t rc;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  L7_BOOL     enable;
  DAPI_USP_t ddusp;
  L7_RC_t dr;


  static const char *routine_name = "dtlIpv6ForwardingModeSet()";

  DTL_IP_TRACE("%s %d: %s : mode =  %s\n",
                 __FILE__, __LINE__, routine_name,
                 mode?"enable":"disable");

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  if (mode == L7_ENABLE)
    enable = L7_TRUE;
  else
    enable = L7_FALSE;

  dapiCmd.cmdData.forwardingConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.forwardingConfig.enable = enable;

  dr = dapiCtl(&ddusp,
               DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG,
               &dapiCmd);
  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}


/*********************************************************************
 * @purpose  Enables or disables the ability to forward an based on
 * @purpose  IPv6 Traffic Class for the router
 *
 * @param    mode  @b{(input)} L7_ENABLE, to enable Traffic Class based
 *                             Forwarding in this router. 
 *                             L7_DISABLE, to disable Traffic Class based
 *                             forwarding in this router.
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6TrafficClassForwardingModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Configures the router whether to forward BOOTP/DHCP requests 
 * @purpose  through router or not.
 *
 * @param     @b{(input)} L7_ENABLE, to enabling forwarding of BOOTP/DHCP
 *                        requests. 
 *                        L7_DISABLE, to disable forwarding of BOOTP/DHCP
 *                        requests
 * @param     @b{(input)} The IP address of the BOOTP/DHCP server
 * 
 * @returns  L7_SUCCESS  on a successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes Forwarding BOOTP and DHCP frames is an exception to the
 * @notes specification that Layer2 broadcasts and multicasts packets
 * @notes should not be forwarded through the router.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6BootpDhcpRelaySet(L7_uint32 mode,
                         L7_in6_addr_t *addr)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Enables or disables the ability of the router to perform 
 * @purpose  address checking of the source address
 *
 * @param    mode     @b{(input)} L7_ENABLE, to enable checking of
 *                                source address in this router. 
 *                                L7_DISABLE, to disable checking of
 *                                source address in this router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6SourceAddressCheckingModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Enables or disables IP Spoofing check
 *
 * @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
 *
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes   none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6SpoofingModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Configure handling of IP Spoofing Errors
 *
 * @param    mode     @b{(input)} defined by L7_RT_NP_SPOOFING_HANDLER_MODE_t
 *
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes   This configuration is valid only if spoofing is enabled.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6SpoofingHandlingSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose Enables or disables the ability of the router to send
 * @purpose ICMP Redirect error when it receives a PDU that should have
 * @purpose been to a different router.
 *
 * @param    mode     @b{(input)} L7_ENABLE, to enable ICMP Redirect in
 *                                           this router. 
 *                                L7_DISABLE, to disable ICMP Redirect
 *                                            in this router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6ICMPRedirectModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Assigns the Layer 2 encapsulation type for the particular
 * @purpose  router interface.                   
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                which encapsulation type is assigned
 * @param    type     @b{(input)} L7_LL_ENCAP_ENET, to set the encapsulation
 *                                according to Ethernet Encapsulation
 *                                (RFC 894) 
 *                                L7_LL_ENCAP_802, to set encapsulation
 *                                according to IEE 802.2/3 Encapsulation
 *                                (RFC1042)
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6L2EncapsulationTypeSet(L7_uint32 intIfNum,
                              L7_uint32 type)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Assigns a MAC Address for a particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for 
 *                                which the MAC address is assigned
 * @param    *macAddr @b{(input)} A reference to the MAC address
 *                                to be assigned
 *
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   This command associates a MAC address with a router interface.
 * @notes   This command is valid with the layer 3 routing offering only.
 * @notes   It is completely distinct from the commands to store a MAC 
 * @notes   address in the layer 2 filtering database (i.e. address tables).
 * @notes   In particular, it is completely distinct from the commands
 * @notes   dtlFdbSystemMacSet() and dtlFdbMacAddrAdd().
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6IntfMacAddrSet(L7_uint32 intIfNum,
                      L7_enetMacAddr_t *macAddr)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Sets the MTU (Maximum Transmission Unit) for this particular
 *           router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for this
 *                                router interface
 * @param    mtu      @b{(input)} The size of the Maximum Transmission Unit
 *
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6MtuSet(L7_uint32 intIfNum, L7_uint32 mtu)
{
  L7_RC_t rc = L7_SUCCESS;
  /* MAYBE FILL THIS OUT FOR DEMO */

  return rc;
}


/*********************************************************************
 * @purpose  Enables or disables the ability to forward multicast traffic for 
 * @purpose  this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    mode     @b{(input)} L7_ENABLE, to enable forwarding of
 *                                           multicast traffic in this
 *                                           router interface. 
 *                                L7_DISABLE, to disable forwarding of
 *                                            multicast traffic in this
 *                                            router interface.
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6MulticastIntfForwardModeSet(L7_uint32 intIfNum,
                                   L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Adds an IP address to the Local Multicast List associated 
 * @purpose  with this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IPv6 address to be added to this list  
 *
 *
 * @returns  L7_SUCCESS  on successful addition
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @notes    The Local Multicast List contains the list Multicast IP addresses 
 *           that this interface will interact with, respond to, etc.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6LocalMulticastAddrAdd(L7_uint32 intIfNum,
                             L7_in6_addr_t *addr)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Deletes an IP address from the Local Multicast List 
 * @purpose  associated with this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IPv6 address to be deleted from
 *                                this list  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6LocalMulticastAddrDelete(L7_uint32 intIfNum,
                                L7_in6_addr_t *addr)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
 * @purpose  Adds an IPv6 address to a router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IPv6 address to be added to this list  
 *
 *
 * @returns  L7_SUCCESS  on successful addition
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6IntfAddrAdd(L7_uint32 intIfNum,
                   L7_in6_addr_t *addr,
                   L7_uint32 prefixLen)
{
  L7_uchar8 addr_str[64];
  L7_RC_t rc;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;


  static const char *routine_name = "dtlIpv6IntfAddrAdd()";

  inet_ntop(AF_INET6, addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: %s : IP Addr = %s, prefixLen = %d, intIfNum = %d\n",
                 __FILE__, __LINE__, routine_name,
                 addr_str, prefixLen, intIfNum);

  
  if (nimGetUnitSlotPort(intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  memset(&dapiCmd,0,sizeof(dapiCmd));
  dapiCmd.cmdData.rtrIntfIpAddrAddDelete.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.rtrIntfIpAddrAddDelete.ip6Addr,
         addr,
         sizeof(dapiCmd.cmdData.rtrIntfIpAddrAddDelete.ip6Addr));
  ip6MapPrefixLenToMask(&dapiCmd.cmdData.rtrIntfIpAddrAddDelete.ip6Mask,
                 prefixLen);

  rc = dapiCtl(&ddusp, DAPI_CMD_IPV6_INTF_ADDR_ADD, &dapiCmd);

  return rc;
}


/*********************************************************************
 * @purpose  Deletes an IPv6 address from a router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IPv6 address to be deleted from
 *                                this list  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6IntfAddrDelete(L7_uint32 intIfNum,
                      L7_in6_addr_t *addr,
                      L7_uint32 prefixLen)
{
  L7_uchar8 addr_str[64];
  L7_RC_t rc;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;


  static const char *routine_name = "dtlIpv6IntfAddrDelete()";

  inet_ntop(AF_INET6, addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: %s : IP Addr = %s, prefixLen = %d, intIfNum = %d\n",
                 __FILE__, __LINE__, routine_name,
                 addr_str, prefixLen, intIfNum);

  
  if (nimGetUnitSlotPort(intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  memset(&dapiCmd,0,sizeof(dapiCmd));
  dapiCmd.cmdData.rtrIntfIpAddrAddDelete.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.rtrIntfIpAddrAddDelete.ip6Addr,
         addr,
         sizeof(dapiCmd.cmdData.rtrIntfIpAddrAddDelete.ip6Addr));
  ip6MapPrefixLenToMask(&dapiCmd.cmdData.rtrIntfIpAddrAddDelete.ip6Mask,
                 prefixLen);

  rc = dapiCtl(&ddusp, DAPI_CMD_IPV6_INTF_ADDR_DELETE, &dapiCmd);

  return rc;
}

/*********************************************************************
 * @purpose  Harvest any hardware maintained IPv6 statistics from
 *           an interface.
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    stats     @b{(output)} A block to put the statistics in.
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   The stats block will contain the hardware values of the
 *          the statistics.  Any statistic that is not kept by hardware
 *          will have a zero value on return.
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6IntfStatsGet(L7_uint32 intIfNum,
                                    L7_ipv6InterfaceStats_t *stats)
{
  L7_RC_t rc;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;

  if (stats == NULL)
  {
    return L7_FAILURE;
  }

  if (nimGetUnitSlotPort(intIfNum, &nimusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  memset(stats, 0, sizeof(*stats));
  memset(&dapiCmd, 0, sizeof(dapiCmd));
  dapiCmd.cmdData.rtrL3IntfStatsGet.getOrSet = DAPI_CMD_GET;
  rc = dapiCtl(&ddusp, DAPI_CMD_IPV6_INTF_STATS_GET, &dapiCmd);
  if (rc == L7_SUCCESS)
  {
    *stats = dapiCmd.cmdData.rtrL3IntfStatsGet.stats.ipv6;
  }

  return rc;
}

/*********************************************************************
 * @purpose  Adds a Neighbor entry in the Neighbor table
 * @purpose  maintained in the Forwarding Information Base of the
 * @purpose  network-processing device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the Neighbor entry
 *                    object containing information needed to add an entry
 *                    to the Neighbor table.  
 *
 *
 * @returns  L7_SUCCESS  on a successful addition of the Neighbor entry 
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @notes    Neighbor table entries correlate an IPv6 address to
 *           a MAC address and Router Interface. The table is built as a
 *           result of static entries and information obtained from
 *           Neighbor Discovery requests and replies. Dynamic 
 *           entries are periodically aged.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6NeighEntryAdd(L7_ip6NbrTblEntry_t *pNeigh)
{
  L7_uchar8 addr_str[64];
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;


  static const char *routine_name = "dtlIpv6NeighEntryAdd()";

  inet_ntop(AF_INET6, &pNeigh->ip6Addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: %s : IP Addr = %s, intIfNum = %d, "
                 "mac addr = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x \n",
                 __FILE__, __LINE__, routine_name,
                 addr_str,
                 pNeigh->intIfNum,
                 pNeigh->macAddr.addr.enetAddr.addr[0],
                 pNeigh->macAddr.addr.enetAddr.addr[1],
                 pNeigh->macAddr.addr.enetAddr.addr[2],
                 pNeigh->macAddr.addr.enetAddr.addr[3],
                 pNeigh->macAddr.addr.enetAddr.addr[4],
                 pNeigh->macAddr.addr.enetAddr.addr[5]);

  /* an interface number of 0 is used for "system" neighbor entries,
   * such as the routing protocol multicast IP addresses (these are
   * typically static neighbor entries), so don't call NIP in this
   * case
   */
  if (pNeigh->intIfNum == 0)
  {
    return L7_FAILURE;
  }
  else
  {
    if (nimGetUnitSlotPort(pNeigh->intIfNum, &nimusp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    ddusp.unit = nimusp.unit;
    ddusp.slot = nimusp.slot;
    ddusp.port = nimusp.port - 1;
  }

  memset(&dapiCmd,0,sizeof(dapiCmd));
  dapiCmd.cmdData.arpAdd.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.arpAdd.ip6Addr,
         &pNeigh->ip6Addr,
         sizeof(dapiCmd.cmdData.arpAdd.ip6Addr));
  dapiCmd.cmdData.arpAdd.flags = DAPI_ROUTING_ARP_DEST_KNOWN;

  memcpy(&(dapiCmd.cmdData.arpAdd.macAddr),
         &(pNeigh->macAddr.addr.enetAddr),
         L7_MAC_ADDR_LEN);

  dr = dapiCtl(&ddusp, DAPI_CMD_IPV6_NEIGH_ENTRY_ADD, &dapiCmd);

  if (dr != L7_SUCCESS)
    rc = L7_FAILURE;
  else
  {
    rc = L7_SUCCESS;
  }

  return rc;
}


/*********************************************************************
 * @purpose  Modifies an existing Neighbor entry in the
 * @purpose  Neighbor table maintained in the Forwarding
 * @purpose  Information Base of the network-processing device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the Neighbor entry
 *                                object, which contains modified parameters.  
 *
 *
 * @returns  L7_SUCCESS  on a successful modification of the entry
 * @returns  L7_FAILURE  if the modification failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6NeighEntryModify(L7_ip6NbrTblEntry_t *pNeigh)
{
  L7_uchar8 addr_str[64];
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;

  static const char *routine_name = "dtlIpv6NeighEntryModify()";

  inet_ntop(AF_INET6, &pNeigh->ip6Addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: %s : IP Addr = %s, intIfNum = %d, "
                 "mac addr = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x \n",
                 __FILE__, __LINE__, routine_name,
                 addr_str,
                 pNeigh->intIfNum,
                 pNeigh->macAddr.addr.enetAddr.addr[0],
                 pNeigh->macAddr.addr.enetAddr.addr[1],
                 pNeigh->macAddr.addr.enetAddr.addr[2],
                 pNeigh->macAddr.addr.enetAddr.addr[3],
                 pNeigh->macAddr.addr.enetAddr.addr[4],
                 pNeigh->macAddr.addr.enetAddr.addr[5]);

  /* an interface number of 0 is used for "system" neighbor entries,
   * such as the routing protocol multicast IP addresses (these are
   * typically static neighbor entries), so don't call NIP in this
   * case
   */
  if (pNeigh->intIfNum == 0)
  {
    return L7_FAILURE;
  }
  else
  {
    if (nimGetUnitSlotPort(pNeigh->intIfNum, &nimusp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    ddusp.unit = nimusp.unit;
    ddusp.slot = nimusp.slot;
    ddusp.port = nimusp.port - 1;
  }

  memset(&dapiCmd,0,sizeof(dapiCmd));
  dapiCmd.cmdData.arpModify.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.arpModify.ip6Addr,
         &pNeigh->ip6Addr,
         sizeof(dapiCmd.cmdData.arpModify.ip6Addr));
  dapiCmd.cmdData.arpModify.flags = DAPI_ROUTING_ARP_DEST_KNOWN;
  memcpy(&(dapiCmd.cmdData.arpModify.macAddr),
         &(pNeigh->macAddr.addr.enetAddr),
         L7_MAC_ADDR_LEN);

  dr = dapiCtl(&ddusp, DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY, &dapiCmd);

  if (dr == L7_SUCCESS)
    rc = L7_SUCCESS;
  else
    rc = L7_FAILURE;

  return rc;
}


/*********************************************************************
 * @purpose  Deletes a Neighbor entry in the Neighbor table maintained
 * @purpose  in the Forwarding Information Base of the network-processing
 * @purpose  device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the Neighbor entry
 *                                object, which describes the entry to delete.
 *
 * @returns  L7_SUCCESS  on a successful deletion of the entry 
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes    A Neighbor entry is uniquely identified by the
 *           IPv6 address associated with it.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6NeighEntryDelete(L7_ip6NbrTblEntry_t *pNeigh)
{
  L7_uchar8 addr_str[64];
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;

  static const char *routine_name = "dtlIpv6NeighEntryDelete()";

  inet_ntop(AF_INET6, &pNeigh->ip6Addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: %s : IP Addr =  %s,  \n",
                 __FILE__, __LINE__, routine_name,
                 addr_str);

  /* an interface number of 0 is used for "system" neighbor entries,
   * such as the routing protocol multicast IP addresses (these are
   * typically static neighbor entries), so don't call NIP in this
   * case
   */
  if (pNeigh->intIfNum == 0)
  {
    return L7_FAILURE;
  }
  else
  {
    if (nimGetUnitSlotPort(pNeigh->intIfNum, &nimusp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    ddusp.unit = nimusp.unit;
    ddusp.slot = nimusp.slot;
    ddusp.port = nimusp.port - 1;
  }

  memset(&dapiCmd,0,sizeof(dapiCmd));
  dapiCmd.cmdData.arpDelete.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.arpDelete.ip6Addr,
         &pNeigh->ip6Addr,
         sizeof(dapiCmd.cmdData.arpDelete.ip6Addr));
  dapiCmd.cmdData.arpDelete.flags = 0;
  memcpy(&(dapiCmd.cmdData.arpDelete.macAddr),
         &(pNeigh->macAddr.addr.enetAddr),
         L7_MAC_ADDR_LEN);

  dr = dapiCtl(&ddusp, DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE, &dapiCmd);

  if (dr != L7_SUCCESS)
    rc = L7_FAILURE;
  else
  {
    rc = L7_SUCCESS;
  }

  return rc;
}


/*********************************************************************
* @purpose  Get the status of a NDP entry in the hardware
*
* @param    *pNdpQuery   @b{(input)} A reference to the NDP entry object
*           containing information about hit-bit and last-hit-time
*
* @returns  L7_SUCCESS  on a successful query for an NDP entry
* @returns  L7_FAILURE  if the query failed
*
* @notes    number of entries queried for in this API is 1.
*
*
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpv6NeighEntryQuery(L7_ndpQuery_t *pNdpQuery)
{
  L7_uchar8 addr_str[64];
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_ARP_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  L7_uint32 lhs,lhd;

  static const char *routine_name = "dtlIpv6NeighEntryQuery()";

  inet_ntop(AF_INET6, &pNdpQuery->ip6Addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: %s : IP Addr =  %s, Intf %d \n",
                 __FILE__, __LINE__, routine_name,
                 addr_str, pNdpQuery->intIfNum);

  if (pNdpQuery->intIfNum == 0)
  {
    return L7_FAILURE;
  }
  else
  {
    if (nimGetUnitSlotPort(pNdpQuery->intIfNum, &nimusp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    ddusp.unit = nimusp.unit;
    ddusp.slot = nimusp.slot;
    ddusp.port = nimusp.port - 1;
  }

  memset(&dapiCmd,0,sizeof(dapiCmd));

  dapiCmd.cmdData.arpQuery.getOrSet = DAPI_CMD_GET;
  dapiCmd.cmdData.arpQuery.numOfEntries    = 1;
  dapiCmd.cmdData.arpQuery.vlanID[0]       = pNdpQuery->vlanId;
  dapiCmd.cmdData.arpQuery.arpQueryFlags[0]= 0;

  memcpy(&dapiCmd.cmdData.arpQuery.ip6Addr[0],
         &pNdpQuery->ip6Addr, sizeof(L7_in6_addr_t));

  dr = dapiCtl(&ddusp, DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY, &dapiCmd);

  if (dr != L7_SUCCESS)
    rc = L7_FAILURE;
  else
  {
    rc = L7_SUCCESS;
    pNdpQuery->ndpQueryFlags &= ~(L7_NDP_HIT_ANY);

    if(dapiCmd.cmdData.arpQuery.arpQueryFlags[0] & DAPI_ROUTING_ARP_HIT_SOURCE)
    {
      pNdpQuery->ndpQueryFlags |= L7_NDP_HIT_SOURCE;
      lhs = dapiCmd.cmdData.arpQuery.lastHitSrc[0];
    }
    else
    {
      lhs=0;
    }

    if(dapiCmd.cmdData.arpQuery.arpQueryFlags[0] & DAPI_ROUTING_ARP_HIT_DEST)
    {
      pNdpQuery->ndpQueryFlags |= L7_NDP_HIT_DEST;
      lhd = dapiCmd.cmdData.arpQuery.lastHitDst[0];
    }
    else
    {
      lhd=0;
    }

    if(pNdpQuery->ndpQueryFlags & L7_NDP_HIT_ANY)
    {
      pNdpQuery->lastHitTime = (lhs > lhd ? lhs : lhd);
    }
  }

  return rc;
}


/*********************************************************************
 * @purpose  Gets count of subsystem Neighbor entries
 *
 * @param    void
 *
 *
 * @returns  count of Neighbor entries in subsystem
 *
 * @notes    This routine is used for internal component synchronization.
 *           It is not intended to be used for external user management
 *           (i.e. SNMP, Web) purposes.
 *
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_uint32
dtlIpv6NeighborEntryCountGet(void)
{
  L7_uint32 rv = 0;

  return rv;
}


/*********************************************************************
 * @purpose  Callback function from the driver for NDP work.
 *
 * @param    *ddusp         @b{(input)} Device driver reference to unit
 * @param                               slot and port
 * @param	  family         @b{(input)} Device Driver family type 
 * @param    cmd            @b{(input)} Command of type DAPI_CMD_t
 * @param    event          @b{(input)} Event indication of type DAPI_EVENT_t
 * @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent data
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6NeighEntryCallback(DAPI_USP_t *ddusp, 
                          DAPI_FAMILY_t family, 
                          DAPI_CMD_t cmd, 
                          DAPI_EVENT_t event,
                          void *dapiEventInfo)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
 * @purpose  Adds a route to the Routing Table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed for a route entry.  
 *
 *
 * @returns  L7_SUCCESS  on successful addition 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    Route table entries correlate a route to a router interface or 
 * @notes    network interface. The routes kept in the table originate from
 *           static 
 * @notes    configuration and routing protocols, such as RIP, OSPF, and BGP.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6RouteEntryAdd(L7_route6Entry_t *pRoute)
{
  L7_uchar8 addr_str[64];
  L7_RC_t rc;
  L7_RC_t dr;
  L7_in6_addr_t mask;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_ROUTING_ROUTE_ENTRY_t *pEcmpRoutes;
  DAPI_USP_t ddusp;
  
  inet_ntop(AF_INET6, &pRoute->ip6Addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: dtlIpv6RouteEntryAdd - IP Addr = %s, len = %d\n",
                 __FILE__, __LINE__, addr_str, pRoute->ip6PrefixLen);

  osapiSemaTake(dtlInfo.routeEntrySem, L7_WAIT_FOREVER);

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  /*
   * Compute a mask from this length.
   */
  ip6MapPrefixLenToMask(&mask, pRoute->ip6PrefixLen);
  inet_ntop(AF_INET6, &mask, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: dtlIpv6RouteEntryAdd - computed mask = %s\n",
                 __FILE__, __LINE__, addr_str);

  memset(&dapiCmd,0,sizeof(dapiCmd));

  pEcmpRoutes = &dapiCmd.cmdData.routeAdd.route;

  if (dtlIpv6RouteEntryExtract(pRoute,pEcmpRoutes) != L7_SUCCESS)
  {
	osapiSemaGive(dtlInfo.routeEntrySem);
    return L7_FAILURE;
  }

  dapiCmd.cmdData.routeAdd.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.routeAdd.ip6Addr,
         &pRoute->ip6Addr,
         sizeof(dapiCmd.cmdData.routeAdd.ip6Addr));
  memcpy(&dapiCmd.cmdData.routeAdd.ip6Mask,
         &mask,
         sizeof(dapiCmd.cmdData.routeAdd.ip6Mask));
  dapiCmd.cmdData.routeAdd.flags = dtlIpv6RouteToDapiFlags(pRoute);
  dapiCmd.cmdData.routeAdd.count = 0;

  dr = dapiCtl(&ddusp,DAPI_CMD_IPV6_ROUTE_ENTRY_ADD,&dapiCmd);

  if (dr == L7_SUCCESS)
  {
      dtlInfo.routeEntryCount++;
      rc = L7_SUCCESS;
  }
  else if (dr == L7_ALREADY_CONFIGURED)
  {
    dtlInfo.routeEntryCount++;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dtlInfo.routeEntrySem);
  return rc;
}


/*********************************************************************
 * @purpose  Modifies a route in the routing table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed to identify a route and modify it.  
 *
 *
 * @returns  L7_SUCCESS  on successful modification 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6RouteEntryModify(L7_route6Entry_t *pRoute)
{
  L7_uchar8 addr_str[64];
  L7_in6_addr_t mask;
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_ROUTING_ROUTE_ENTRY_t *pEcmpRoutes;
  DAPI_USP_t ddusp;
  
  inet_ntop(AF_INET6, &pRoute->ip6Addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: dtlIpv6RouteEntryModify - IP Addr = %s, len = %d\n",
                 __FILE__, __LINE__, addr_str, pRoute->ip6PrefixLen);

  osapiSemaTake(dtlInfo.routeEntrySem, L7_WAIT_FOREVER);

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  ip6MapPrefixLenToMask(&mask, pRoute->ip6PrefixLen);
  inet_ntop(AF_INET6, &mask, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: dtlIpv6RouteEntryModify - computed mask = %s\n",
                 __FILE__, __LINE__, addr_str);

  memset(&dapiCmd,0,sizeof(dapiCmd));

  pEcmpRoutes = &dapiCmd.cmdData.routeModify.route;

  if (dtlIpv6RouteEntryExtract(pRoute,pEcmpRoutes) != L7_SUCCESS)
  {
	osapiSemaGive(dtlInfo.routeEntrySem);
    return L7_FAILURE;
  }

  dapiCmd.cmdData.routeModify.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.routeModify.ip6Addr,
         &pRoute->ip6Addr,
         sizeof(dapiCmd.cmdData.routeModify.ip6Addr));
  memcpy(&dapiCmd.cmdData.routeModify.ip6Mask,
         &mask,
         sizeof(dapiCmd.cmdData.routeModify.ip6Mask));
  dapiCmd.cmdData.routeModify.flags = dtlIpv6RouteToDapiFlags(pRoute);
  dapiCmd.cmdData.routeModify.count = 0;

  dr = dapiCtl(&ddusp, DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY, &dapiCmd);

  if (dr == L7_SUCCESS)
  {
      rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dtlInfo.routeEntrySem);
  return rc;
}


/*********************************************************************
 * @purpose  Deletes a route in the routing table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed to identify a route and delete it.  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion 
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6RouteEntryDelete(L7_route6Entry_t *pRoute)
{
  L7_uchar8 addr_str[64];
  L7_in6_addr_t mask;
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;
  
  inet_ntop(AF_INET6, &pRoute->ip6Addr, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: dtlIpv6RouteEntryDelete - IP Addr = %s, len = %d\n",
                 __FILE__, __LINE__, addr_str, pRoute->ip6PrefixLen);

  osapiSemaTake(dtlInfo.routeEntrySem, L7_WAIT_FOREVER);

  ddusp.unit = -1;
  ddusp.slot = -1;
  ddusp.port = -1;

  /*
   * Compute a mask from this length.
   */
  ip6MapPrefixLenToMask(&mask, pRoute->ip6PrefixLen);
  inet_ntop(AF_INET6, &mask, addr_str, sizeof(addr_str));
  DTL_IP_TRACE("%s %d: dtlIpv6RouteEntryDelete - computed mask = %s\n",
                 __FILE__, __LINE__, addr_str);

  memset(&dapiCmd,0,sizeof(dapiCmd));

  dapiCmd.cmdData.routeDelete.getOrSet = DAPI_CMD_SET;
  memcpy(&dapiCmd.cmdData.routeDelete.ip6Addr,
         &pRoute->ip6Addr,
         sizeof(dapiCmd.cmdData.routeDelete.ip6Addr));
  memcpy(&dapiCmd.cmdData.routeDelete.ip6Mask,
         &mask,
         sizeof(dapiCmd.cmdData.routeDelete.ip6Mask));

  dr = dapiCtl(&ddusp, DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE, &dapiCmd);

  if (dr == L7_SUCCESS)
  {
    dtlInfo.routeEntryCount--;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  osapiSemaGive(dtlInfo.routeEntrySem);
  return rc;
}


/*********************************************************************
 * @purpose  Gets count of subsystem route entries
 *
 * @param    void
 *
 *
 * @returns  count of route entries in subsystem
 *
 * @notes    This routine is used for internal component synchronization.
 *           It is not intended to be used for external user management
 *           (i.e. SNMP, Web) purposes.
 *
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_uint32
dtlIpv6RouteEntryCountGet(void)
{
  L7_uint32 rv = 0;

  return rv;
}


/*********************************************************************
 * @purpose  Add a VRRP Virtural Router ID to an interface
 *
 * @param    intIfNum @b{(input)} an internal interface number
 * @param    vrID     @b{(input)} VRRP Virtual Router ID
 * @param    ipAddr   @b((input)) VRRP IPv6 address
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 *
 * @notes    This function is valid only for routing interfaces.
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6VrrpVridAdd(L7_uint32 intIfNum,
                   L7_uint32 vrID,
                   L7_in6_addr_t *addr)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}
 

/*********************************************************************
 * @purpose  Remove a VRRP Virtural Router ID from an interface
 *
 * @param    intIfNum @b{(input)} an internal interface number
 * @param    vrID     @b{(input)} VRRP Virtual Router ID
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 *
 * @notes    This function is valid only for routing interfaces.
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t
dtlIpv6VrrpVridDelete(L7_uint32 intIfNum, L7_uint32 vrID)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}

