/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
* @filename   sim_svc_port.c
*
* @purpose    Functions related to IP addressing on the service port 
*
* @component  System Interface Manager (SIM)
*
* @comments   Some logic moved here from sim.c.
*
*             As this entire directory doesn't have an _api.c file, this file
*             includes both component APIs and implementation functions related
*             to the service port.
*
*             Functions for both IPv4 and IPv6 on the service port are here.
*
* @create     02/27/2009
*
* @author     rrice
*
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "l7_cnfgr_api.h"
#include "l7_ip6_api.h"
#include "nimapi.h"
#include "sim.h"
#include "simapi.h"
#include "sysapi.h"
#include "l7_product.h"
#include "log.h"
#include "defaultconfig.h"
#include "dtlapi.h"
#include "ipstk_api.h"
#include "osapi_support.h"
#include "dhcp_bootp_api.h"
#include "dhcp6c_api.h"

#ifdef L7_NSF_PACKAGE
  #include "sim_ckpt.h"
#endif
#if L7_FEAT_DNI8541_BLADESERVER
  #include "bspapi_blade.h"
#endif

extern simCfgData_t      simCfgData;
extern simOperInfo_t     *simOperInfo;


#define          NUM_IPV6_ADDRS_FROM_STACK       L7_RTR6_MAX_INTF_ADDRS * 2

extern simRouteStorage_t  *servPortRoutingProtRoute;
extern simRouteStorage_t  *netPortRoutingProtRoute;


/*********************************************************************
* @purpose  Set the operational IPv4 address on the network port.
*
* @param    ipAddr    @b{(input)} IPv4 address in use on network port
* @param    netMask   @b{(input)} Associated network mask
* @param    addrType  @b{(input)} Address configured, from DHCP, or checkpointed
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simServPortOperAddrSet(L7_uint32 ipAddr, L7_uint32 netMask, 
                               e_simMgmtIpAddrType addrType)
{
  if ((simOperInfo->servPortIpAddress == ipAddr) &&
      (simOperInfo->servPortNetMask == netMask) &&
      (simOperInfo->servPortAddrType == addrType))
  {
    return L7_SUCCESS;
  }

  simOperInfo->servPortIpAddress = ipAddr;
  simOperInfo->servPortNetMask = netMask;
  simOperInfo->servPortAddrType = addrType;
  return simServPortIPAddrApply();
}

/*********************************************************************
* @purpose  Get configured value for service port IPv4 address
*
* @param    void
*
* @returns  configured service port IPv4 address
*
* @comments  Not necessarily the value being used operationally on 
*            the service port.
*
* @end
*********************************************************************/
L7_uint32 simConfiguredServPortIPAddrGet(void)
{
  return simCfgData.servPortIpAddress;
}

/*********************************************************************
* @purpose  Get the service port IPv4 address
*
* @param    void
*
* @returns  service port IPv4 Address
*
* @comments  This is the IPv4 address in use on the service port. May
*            differ from the configured address.
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPAddr(void)
{
  if (simOperInfo)
    return simOperInfo->servPortIpAddress;
  return 0;
}

/*********************************************************************
* @purpose  Determine whether the service port address is a checkpointed 
*           address, not yet confirmed to be valid following a failover.
*
* @param    none
*
* @returns  L7_TRUE if service port address is a checkpointed address
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL simIsServPortAddrCheckpointed(void)
{
  return (simOperInfo->servPortIpAddress && 
          (simOperInfo->servPortAddrType == SIM_MGMT_ADDR_CKPT));
}

/*********************************************************************
* @purpose  Configure an IPv4 address on the service port
*
* @param    ipAddr   @b{(input)} Service port IPv4 address
*
* @returns  L7_SUCCESS
*           L7_ERROR if DHCP is configured on the interface
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simServPortIPAddrConfigure(L7_uint32 ipAddr)
{
  /* For the very short term, don't allow user to configure an address
   * if DHCP is enabled on this interface. */
  if (simCfgData.servPortConfigMode != L7_SYSCONFIG_MODE_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Rejecting address configured on service port. DHCP enabled.");
    return L7_ERROR;
  }

  simCfgData.servPortIpAddress = ipAddr;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  /* Make the configured address the operational address. Return 
   * success even if apply fails. */
  simServPortOperAddrSet(ipAddr, simCfgData.servPortNetMask, SIM_MGMT_ADDR_CONF);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configure an IPv4 address with its network mask on the service port
*
* @param    ipAddr   @b{(input)} Service port IPv4 address
* @param    netMask  @b{(input)} network mask
*
* @returns  L7_SUCCESS
*           L7_ERROR if DHCP is configured on the interface
*
* @comments  Better to set the address and mask together when possible
*
* @end
*********************************************************************/
L7_RC_t simConfigureServPortIPAddrWithMask(L7_uint32 ipAddr, L7_uint32 netMask)
{
  /* For the very short term, don't allow user to configure an address
   * if DHCP is enabled on this interface. */
  if (simCfgData.servPortConfigMode != L7_SYSCONFIG_MODE_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Rejecting address configured on service port. DHCP enabled.");
    return L7_ERROR;
  }

  simCfgData.servPortIpAddress = ipAddr;
  simCfgData.servPortNetMask = netMask;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  /* Make the configured address the operational address. Return 
   * success even if apply fails. */
  simServPortOperAddrSet(ipAddr, netMask, SIM_MGMT_ADDR_CONF);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set DHCP or BOOTP address and network mask on the service port.
*
* @param    dhcpAddr   @b{(input)} IPv4 address from DHCP
* @param    netMask    @b{(input)} network mask
*
* @returns  
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetServPortAddrWithMask(L7_uint32 dhcpAddr, L7_uint32 netMask)
{
  if (simCfgData.servPortIpAddress != 0)
  {
    /* The DHCP client should only request an address from the DHCP
     * server when no address is configured.  */
    L7_uchar8 dhcpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 cfgAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(dhcpAddr, dhcpAddrStr);
    osapiInetNtoa(simCfgData.servPortIpAddress, cfgAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID,
            "DHCP assigned IPv4 address %s to service port, but service"
            " port already has configured address %s.",
            dhcpAddrStr, cfgAddrStr);
    return L7_FAILURE;
  }

  return simServPortOperAddrSet(dhcpAddr, netMask, SIM_MGMT_ADDR_DHCP);
}

/*********************************************************************
* @purpose  Apply the service port IP address
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments   
*
* @end
*********************************************************************/
L7_RC_t simServPortIPAddrApply(void)
{
  /* Tell IP stack about new address */
  sysapiConfigServicePortIp(simOperInfo->servPortIpAddress, 
                            simOperInfo->servPortNetMask);
  if (!simOperInfo->servPortIpAddress)
  {
    osapiCleanupIf(bspapiServicePortNameGet());
  }

  simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_SERVICEPORT, L7_AF_INET);
  
  /* Reset the address conflict detected statistics for the new address */
  simOperInfo->servicePortConflictStats.numOfConflictsDetected = 0;

#if L7_FEAT_DNI8541_BLADESERVER
  bspapiIomIpConfigChanged();
#endif
#ifdef L7_NSF_PACKAGE
  /* IP address change, make a checkpoint request */
  if (simOperInfo->servPortAddrType == SIM_MGMT_ADDR_DHCP)
  {
    simCkptRequest(SIM_CKPT_IE_SERV_PORT_IP_ADDR);
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Unit's service port NetMask
*
* @param    none
*
* @returns  netMask   service port NetMask
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPNetMask(void)
{
  if (simOperInfo)
    return simOperInfo->servPortNetMask;
  return 0;
}

/*********************************************************************
* @purpose  Configure the network mask on the service port
*
* @param    netMask   @b{(input)} service port network mask 
*
* @returns  none
*
* @comments  
*
* @end
*********************************************************************/
void simServPortIPNetMaskConfigure(L7_uint32 netMask)
{
  /* For the very short term, don't allow user to configure an address
   * if DHCP is enabled on this interface. */
  if (simCfgData.servPortConfigMode != L7_SYSCONFIG_MODE_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Rejecting network mask configured on service port. DHCP enabled.");
    return;
  }

  if (netMask == simCfgData.servPortNetMask)
  {
    return;
  }

  simCfgData.servPortNetMask = netMask;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;
  
  /* Make the configured address the operational address. Return 
   * success even if apply fails. */
  simServPortOperAddrSet(simCfgData.servPortIpAddress, netMask, SIM_MGMT_ADDR_CONF);
  return;
}

/*********************************************************************
* @purpose  Get the network mask configured on the network port
*
* @param    void
*
* @returns  network mask on the network port
*
* @comments   
*
* @end
*********************************************************************/
L7_uint32 simConfiguredServPortIPNetMaskGet(void)
{
  return simCfgData.servPortNetMask;
}

/*********************************************************************
* @purpose  Get the configured IPv4 default gateway on the service port
*
* @param    void
*
* @returns  IPv4 address of the default gateway  
*
* @comments  This is the configured value, not the operational value.
*
* @end
*********************************************************************/
L7_uint32 simConfiguredServPortIPGatewayGet(void)
{
  return simCfgData.servPortGateway;
} 

/*********************************************************************
* @purpose  Configure the IPv4 default gateway for the service port.
*
* @param    newGateway  @b{(input)} IPv4 address of the default gw 
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simServPortIPGatewayConfigure(L7_uint32 newGateway)
{
  /* For the very short term, don't allow user to configure an address
   * if DHCP is enabled on this interface. */
  if (simCfgData.servPortConfigMode != L7_SYSCONFIG_MODE_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Rejecting default gateway configured on service port. DHCP enabled.");
    return;
  }

  if (newGateway != simCfgData.servPortGateway)
  {
    simCfgData.servPortGateway = newGateway;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
    simServPortOperGatewaySet(newGateway);
  }
}

/*********************************************************************
* @purpose  Set the operational IPv4 default gateway for the service port.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simServPortGatewayApply(void)
{
  if (ipstkDefGwUpdate() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
            "Failed to set the default gateway in the IP stack.");
  }

#ifdef L7_NSF_PACKAGE
    /* IP gateway change, make a checkpoint request */
  if (simOperInfo->servPortAddrType == SIM_MGMT_ADDR_DHCP)
  {
    simCkptRequest(SIM_CKPT_IE_SERV_PORT_IP_ADDR);
  }
#endif

#if L7_FEAT_DNI8541_BLADESERVER
  bspapiIomIpConfigChanged();
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the operational IPv4 default gateway for the service port.
*
* @param    newGateway  @b{(input)} new IPv4 default gateway on service port
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simServPortOperGatewaySet(L7_uint32 newGateway)
{
  simOperInfo->servPortGateway = newGateway;
  return simServPortGatewayApply();
}

/*********************************************************************
* @purpose  Get the IPv4 address of the default gateway on the service port
*
* @param    none
*
* @returns  gateway  service port Gateway
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPGateway(void)
{
  if (simOperInfo)
    return simOperInfo->servPortGateway;
  return 0;
}

/*********************************************************************
* @purpose  Set the IPv4 address of the default gateway on the service port
*           as learned from DHCP.
*
* @param    newGateway  @b{(input)} new service port default gateway
*
* @returns  none
*
* @comments  Note that we intentionally do not check if newGateway is 
*            different from the current gateway. After a warm restart,
*            the value might not change, but we still need to update
*            the IP stack.
*
* @end
*********************************************************************/
void simSetServPortIPGateway(L7_uint32 newGateway)
{
  simServPortOperGatewaySet(newGateway);
}

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
/*********************************************************************
* @purpose  Set the DHCPv6 operational address on the service port.
*
* @param    ip6Addr  @b{(input)} IPv6 address on service port
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simServPortDhcpv6AddrSet(L7_in6_addr_t* ip6Addr)
{
  memcpy(&simOperInfo->servPortDhcpv6Addr, ip6Addr, sizeof(L7_in6_addr_t));

#ifdef L7_NSF_PACKAGE
  /* Dynamic IPv6 address added, make a checkpoint request */
  simCkptRequest(SIM_CKPT_IE_SERV_PORT_IP6_ADDR);
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear the DHCPv6 operational address on the service port.
*           This is called when we disable DHCPv6 Client protocol on
*           the service port once we released the address.
*
* @param    None
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simServPortDhcpv6AddrClear(void)
{
  memset(&simOperInfo->servPortDhcpv6Addr, 0, sizeof(L7_in6_addr_t));

#ifdef L7_NSF_PACKAGE
  /* Dynamic IPv6 address removed, make a checkpoint request */
  simCkptRequest(SIM_CKPT_IE_SERV_PORT_IP6_ADDR);
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DHCPv6 operational address on the service port
*
* @param    ip6Addr  @b{(output)} IPv6 address on service port
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simServPortDhcpv6AddrGet(L7_in6_addr_t* ip6Addr)
{
  memcpy(ip6Addr, &simOperInfo->servPortDhcpv6Addr, sizeof(L7_in6_addr_t));

  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Activates/Deactivates one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    enable_flag    @b{(input)}   enable=L7_TRUE, disable=L7_FALSE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
static L7_RC_t simActivateServPortIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                L7_uint32 ip6PrefixLen,
                                L7_uint32 enable_flag)
{

  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (enable_flag)
  {
    if (sysapiConfigServicePortIPV6PrefixAdd(ip6Addr, ip6PrefixLen)
        != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    (void)sysapiConfigServicePortIPV6PrefixRemove(ip6Addr, ip6PrefixLen);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Unit's Service Port IPV6 Admin maode
*
* @param    none
*
* @returns  adminMode    L7_ENABLE or L7_DISABLE 
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetServPortIPV6AdminMode(void)
{
  return simCfgData.servPortIpv6AdminMode;
}

/*********************************************************************
* @purpose  Sets the Unit's Service Port IPV6 Admin Mode
*
* @param    adminMode     @b{(input)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetServPortIPV6AdminMode(L7_uint32 adminMode)
{
  L7_uchar8 *ifName = bspapiServicePortNameGet();
  L7_uchar8 qName[32];
  L7_uint32 i = 0;

  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  sprintf(qName,"%s%d",ifName, bspapiServicePortUnitGet());

  switch (adminMode)
  {
    case L7_ENABLE:
      if (osapiIfIpv6Enable(qName) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* Re-activate configured IPv6 prefixes */
      for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
      {
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
        {
          continue;
        }

        (void)simActivateServPortIPV6Prefix(&simCfgData.servPortIpv6Intfs[i].ip6Addr,
                                            simCfgData.servPortIpv6Intfs[i].ip6PrefixLen,
                                            L7_TRUE);
      }

      /* Enable DHCPv6 operationally on the service port if config-enabled */
      if((simCfgData.servPortIpv6AdminMode != adminMode) &&
         (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP))
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_DHCP,qName,0);
      }

      break;

    case L7_DISABLE:
      /* De-activate configured IPv6 prefixes */
      for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
      {
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
        {
          continue;
        }

        (void)simActivateServPortIPV6Prefix(&simCfgData.servPortIpv6Intfs[i].ip6Addr,
                                            simCfgData.servPortIpv6Intfs[i].ip6PrefixLen,
                                            L7_FALSE);
      }

      /* Disable DHCPv6 operationally on the service port if config-enabled */
      if((simCfgData.servPortIpv6AdminMode != adminMode) &&
         (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP))
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_NONE,qName,0);
      }

      if (osapiIfIpv6Disable(qName) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      break;

    default:
      return L7_FAILURE;
      break;
  }

  if (simCfgData.servPortIpv6AdminMode != adminMode)
  {
    simCfgData.servPortIpv6AdminMode = adminMode;
    ipstkIpv6DefGwUpdate();
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on service port interface
*
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetServPortIPV6Addrs( L7_in6_prefix_t *addrs, L7_uint32 *acount)
{
  L7_RC_t rc = L7_FAILURE;
  L7_in6_prefix_t ip6Addr[NUM_IPV6_ADDRS_FROM_STACK];
  L7_uint32 i, j, count, totalcount = 0;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE];

  count = *acount;

  *acount = 0;
  if ((count == 0) || (addrs == L7_NULLPTR) || (acount == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

    if(simCfgData.servPortIpv6AdminMode == L7_DISABLE)
    {
      *acount = 0;
      return L7_SUCCESS;
    }
    sprintf(ifName, "%s%d",bspapiServicePortNameGet(),bspapiServicePortUnitGet());
    rc =  osapiIfIpv6AddrsGet(ifName, ip6Addr,&count);
    if ((rc != L7_SUCCESS) || (count > NUM_IPV6_ADDRS_FROM_STACK))
    {
      *acount = 0;
      return L7_FAILURE;
    }    

    if(count > 0)
    {
       /* First get the link local address configured in the stack */
      for(i = 0; i < count; i++)
      {
        if(L7_IP6_IS_ADDR_LINK_LOCAL(&ip6Addr[i].in6Addr))
          break;
        else
          continue;
      }
      memcpy(&addrs[totalcount], &ip6Addr[i], sizeof(L7_in6_prefix_t));
      totalcount++;

       /* Next get the manually configured global addresses */
       /* Also check that the total count of addresses filled in addrs array 
        * doesn't cross the available address count 'count' */
      for(i = 0; (i < L7_RTR6_MAX_INTF_ADDRS) && (totalcount < count); i++)
      {
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
        {
          continue;
        }
        memcpy(&addrs[totalcount].in6Addr, &simCfgData.servPortIpv6Intfs[i].ip6Addr, 
                   sizeof(L7_in6_addr_t));
        addrs[totalcount].in6PrefixLen = 
                           simCfgData.servPortIpv6Intfs[i].ip6PrefixLen;
        totalcount++;
      }

       /* Next get either the autoconfigured or dhcpv6 client assigned global ipv6 addresses */
       if((simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
          (simCfgData.servPortIPv6AddrAutoConfig == L7_ENABLE))
       {
         /* Also check that the total count of addresses filled in addrs array 
          * doesn't cross the available address count 'count' */
         for(i = 0; (i < count) && (totalcount < count); i++)
         {
           if(L7_IP6_IS_ADDR_LINK_LOCAL(&ip6Addr[i].in6Addr))
           {
             continue;
           }
           for(j = 0; j < L7_RTR6_MAX_INTF_ADDRS; j++)
           {
             if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[j].ip6Addr))
             {
               continue;
             }
             if(memcmp(&simCfgData.servPortIpv6Intfs[j].ip6Addr, &ip6Addr[i], sizeof(L7_in6_addr_t)) == 0)
             {
               break;
             }
           }
           if(j == L7_RTR6_MAX_INTF_ADDRS)
           {
             memcpy(&addrs[totalcount].in6Addr, &ip6Addr[i], sizeof(L7_in6_prefix_t));
             totalcount++;
           }
         }
       }
      *acount = totalcount;
     }
     return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Get next of the Unit's service port IPv6 configured prefixes.
 *
 * @param    ip6Addr        @b{(input/output)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input/output)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input/output)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    Set ip6Addr to 0's in order to get first IPv6 prefix.
 *
 * @end
 *************************************************************************/
L7_RC_t simGetNextServPortIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 *ip6PrefixLen,
                                  L7_uint32 *eui_flag)
{
  L7_int32 i = 0, foundIndex = -1;

  if (ip6Addr == L7_NULLPTR || ip6PrefixLen == L7_NULLPTR || eui_flag == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (L7_IP6_IS_ADDR_UNSPECIFIED(ip6Addr))
  {
    /* This indicates to get the first entry */
    foundIndex = 0;
  }

  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
    {
      continue;
    }

    if (foundIndex != -1)
    {
      /* This is the "next" entry, so copy and return */
      memcpy(ip6Addr, &simCfgData.servPortIpv6Intfs[i].ip6Addr, sizeof(L7_in6_addr_t));
      *ip6PrefixLen = simCfgData.servPortIpv6Intfs[i].ip6PrefixLen;
      *eui_flag = simCfgData.servPortIpv6Intfs[i].eui_flag;
      return L7_SUCCESS;
    }

    if ((memcmp(&simCfgData.servPortIpv6Intfs[i].ip6Addr, ip6Addr, sizeof(L7_in6_addr_t)) == 0) &&
        (simCfgData.servPortIpv6Intfs[i].ip6PrefixLen == *ip6PrefixLen) &&
        (simCfgData.servPortIpv6Intfs[i].eui_flag == *eui_flag))
    {
        /* Found the "previous" entry */
        foundIndex = i;
    }
  }

  /* "next" entry not found */
  return L7_FAILURE;
}

/**************************************************************************
 * @purpose  Sets one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    force          @b{(input)}   Issue set without updating config
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Check that the IPv6 address being added on the serviceport
 *           is not conflicting with the following addresses:-
 *
 *           (a) Already configured IPv6 prefix on service port
 *           (b) Autoconfigured or dhcpv6 learnt IPv6 address on service port
 *           (c) Configured service port IPv6 gateway
 *           (d) Above 3 cases on network port
 *           (e) IPv6 prefix on any routing interface
 *
 * @end
 *************************************************************************/
L7_RC_t simSetServPortIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
                                  L7_uint32 eui_flag, L7_uint32 force)
{
  L7_int32 i = 0, emptyIndex = -1;
  L7_in6_addr_t tempIp6Addr;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  memset(&tempIp6Addr,0,sizeof(tempIp6Addr));
  memcpy(&tempIp6Addr,ip6Addr,sizeof(tempIp6Addr));

  if (eui_flag == L7_TRUE)
  {
    if(ip6PrefixLen != 64)
       return L7_FAILURE;

    simGetServicePortBurnedInMac(mac);
    
    tempIp6Addr.in6.addr8[8] = mac[0] ^ 0x02;
    tempIp6Addr.in6.addr8[9] = mac[1];
    tempIp6Addr.in6.addr8[10] = mac[2];
    tempIp6Addr.in6.addr8[11] = 0xff;
    tempIp6Addr.in6.addr8[12] = 0xfe;
    tempIp6Addr.in6.addr8[13] = mac[3];
    tempIp6Addr.in6.addr8[14] = mac[4];
    tempIp6Addr.in6.addr8[15] = mac[5];

    memcpy (ip6Addr, &tempIp6Addr, sizeof(tempIp6Addr));
  }

  if (force == L7_FALSE)
  {
    /* Check for conflict with network port prefix */
    if((simSystemIPV6AddressConflictCheck(ip6Addr, ip6PrefixLen, L7_TRUE)) != L7_SUCCESS)
    {
      return L7_ADDR_INUSE;
    }

    /* Check for conflict with configured network port ipv6 gateway */
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
    {
      if(L7_IP6_IS_ADDR_EQUAL(&simCfgData.systemIpv6Gateway, ip6Addr))
      {
        return L7_ADDR_INUSE;
      }
    }

    /* Check for conflict with service port prefix */
    if((simServPortIPV6AddressConflictCheck(ip6Addr, ip6PrefixLen, L7_TRUE)) != L7_SUCCESS)
    {
      return L7_ADDR_INUSE;
    }

    /* Check for conflict with configured service port ipv6 gateway */
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
    {
      if(L7_IP6_IS_ADDR_EQUAL(&simCfgData.servPortIpv6Gateway, ip6Addr))
      {
        return L7_ADDR_INUSE;
      }
    }

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
    /* Check IPv6 prefix for conflicts with routing interface */
    if (ip6MapRtrIntfAddressConflict(0, 0, ip6Addr, ip6PrefixLen, 0, L7_TRUE)
               != L7_SUCCESS)
    {
      return L7_ADDR_INUSE;
    }
#endif
#endif

    for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
    {
      if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
      {
        if (emptyIndex == -1)
            emptyIndex = i;
        break;
      }
    }

    if (emptyIndex == -1)
    {
      /* No empty slots found to store the IPv6 prefix! */
      return L7_FAILURE;
    }
  }

  if (force == L7_FALSE)
  {
    /* Save our new IPv6 prefix in the config */
    memcpy(&simCfgData.servPortIpv6Intfs[emptyIndex].ip6Addr, ip6Addr,
           sizeof(L7_in6_addr_t));
    simCfgData.servPortIpv6Intfs[emptyIndex].ip6PrefixLen = ip6PrefixLen;
    simCfgData.servPortIpv6Intfs[emptyIndex].eui_flag = eui_flag;

    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  (void)simActivateServPortIPV6Prefix(ip6Addr, ip6PrefixLen, L7_TRUE);

  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Removes one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t simDeleteServPortIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
                                     L7_uint32 eui_flag)
{
  L7_int32 i = 0, foundIndex = -1;
  L7_in6_addr_t tempIp6Addr;
   L7_uchar8 mac[L7_MAC_ADDR_LEN];

   memset(&tempIp6Addr, 0, sizeof(tempIp6Addr));
   memcpy(&tempIp6Addr, ip6Addr, sizeof(tempIp6Addr));

   if (eui_flag == L7_TRUE)
   {
     if(ip6PrefixLen != 64)
       return L7_FAILURE;

     simGetServicePortBurnedInMac(mac);

     tempIp6Addr.in6.addr8[8] = mac[0] ^ 0x02;
     tempIp6Addr.in6.addr8[9] = mac[1];
     tempIp6Addr.in6.addr8[10] = mac[2];
     tempIp6Addr.in6.addr8[11] = 0xff;
     tempIp6Addr.in6.addr8[12] = 0xfe;
     tempIp6Addr.in6.addr8[13] = mac[3];
     tempIp6Addr.in6.addr8[14] = mac[4];
     tempIp6Addr.in6.addr8[15] = mac[5];
     memcpy (ip6Addr, &tempIp6Addr, sizeof(tempIp6Addr));
   }


  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
    {
        continue;
    }

    if ((memcmp(&simCfgData.servPortIpv6Intfs[i].ip6Addr, ip6Addr, sizeof(L7_in6_addr_t)) == 0) &&
        (simCfgData.servPortIpv6Intfs[i].ip6PrefixLen == ip6PrefixLen))
    {
       /* Found the address, so just break. */
        foundIndex = i;
        break;
    }
  }

  if (foundIndex == -1)
  {
    /* Didn't find the IPv6 prefix! */
    return L7_FAILURE;
  }

  (void) simActivateServPortIPV6Prefix(ip6Addr, ip6PrefixLen, L7_FALSE);

  /* Remove the IPv6 prefix from the config */
  memset(&simCfgData.servPortIpv6Intfs[foundIndex].ip6Addr, 0, sizeof(L7_in6_addr_t));
  simCfgData.servPortIpv6Intfs[foundIndex].ip6PrefixLen = 0;
  simCfgData.servPortIpv6Intfs[foundIndex].eui_flag = 0;

  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on service port interface
*
* @param    addrs   pointer to default router array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetServPortIPV6DefaultRouters( L7_in6_addr_t *addrs, L7_uint32 *acount)
{
  if (addrs == L7_NULLPTR || acount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return osapiIfIpv6DefaultRoutersGet(bspapiServicePortNameGet(), addrs,acount);
}

/**************************************************************************
*
* @purpose  Get configured IPv6 gateway on service port interface
*
* @param    gateway   pointer to gateway address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t simGetServPortIPV6Gateway( L7_in6_addr_t *gateway)
{
  if (gateway == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
  {
    return L7_FAILURE;
  }
  else
  {
    memcpy(gateway, &simCfgData.servPortIpv6Gateway, sizeof(L7_in6_addr_t));
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Set configured IPv6 gateway on service port interface
*
* @param    gateway   @b{(input)}   pointer to gateway address
* @param    force     @b{(input)}   Issue set without updating config
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Check that the IPv6 gateway being added on the network port
*           is not conflicting with the following addresses:-
*
*           (a) Already configured IPv6 addresses on network port
*           (b) Autoconfigured or dhcpv6 learnt IPv6 address on network port
*           (c) Above 2 cases on service port
*           (d) Configured network port IPv6 gateway
*           (e) IPv6 address on any routing interface
*
* @end
*
*************************************************************************/
L7_RC_t simSetServPortIPV6Gateway( L7_in6_addr_t *gateway, L7_uint32 force)
{
  if (gateway == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (L7_IP6_IS_ADDR_UNSPECIFIED(gateway))
  {
    /* We are attempting to delete the existing gateway */
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
    {
        if (force == L7_FALSE)
        {
          memset(&simCfgData.servPortIpv6Gateway, 0, sizeof(L7_in6_addr_t));
          simCfgData.cfgHdr.dataChanged = L7_TRUE;
        }
    }
  }
  else
  {
    /* Check the conflict of the IPv6 gateway address with any other
     * IPv6 address in the switch */

    /* Check for conflict with service port addresses */
    if((simServPortIPV6AddressConflictCheck(gateway, 128, L7_FALSE)) 
                                            != L7_SUCCESS)
    {
      return L7_ADDR_INUSE;
    }
    /* Check for conflict with network port addresses */
    if((simSystemIPV6AddressConflictCheck(gateway, 128, L7_FALSE)) != L7_SUCCESS)
    {
      return L7_ADDR_INUSE;
    }

    /* Check for conflict with configured network port ipv6 gateway */
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
    {
      if(L7_IP6_IS_ADDR_EQUAL(&simCfgData.systemIpv6Gateway, gateway))
      {
        return L7_ADDR_INUSE;
      }
    }

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
    /* Check IPv6 prefix for conflicts with routing interface */
    if (ip6MapRtrIntfAddressConflict(0, 0, gateway, 128, 0, L7_FALSE)
               != L7_SUCCESS)
    {
      return L7_ADDR_INUSE;
    }
#endif
#endif
    
    if (force == L7_FALSE)
    {
        memcpy(&simCfgData.servPortIpv6Gateway, gateway, sizeof(L7_in6_addr_t));
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
  }
  ipstkIpv6DefGwUpdate();

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  get management port NDisc info from stack
*
* @param    ifName            management interface name
* @param    ipv6NetAddress    {in/out} ipv6 neighbor address
* @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
* @param    ipv6PhysAddress   {out} ipv6 neighbor mac
* @param    ipv6Type          {out} ipv6 neighbor mac type
* @param    ipv6State         {out} ipv6 neighbor state 
* @param    ipv6LastUpdated   {out} ipv6 neighbor last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simGetNextMgmtPortIPV6Ndp( L7_uchar8 *ifName,
                               L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr)
{
    ipstkIpv6NetToMediaEntry_t vars;
    L7_uint32 ifIndex;
    L7_BOOL icarry;

    /* increment index ala snmp */
    if(!L7_IP6_IS_ADDR_UNSPECIFIED(ipv6NetAddress))
    {
        L7_IP6_ADDR_INCREMENT(ipv6NetAddress,icarry);
        if(icarry == L7_TRUE)
            return L7_FAILURE;
    }

    /* get interface index */
    if(ipstkStackIfIndexGet(ifName, &ifIndex) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    vars.ipv6IfIndex = ifIndex;
    vars.ipv6NetToMediaNetAddress = *ipv6NetAddress;

    
    if(osapiIpv6NetToMediaEntryGet(L7_MATCH_GETNEXT, L7_TRUE, &vars) != L7_SUCCESS){
        return L7_FAILURE;
    }
    if(vars.ipv6IfIndex != ifIndex){
        return L7_FAILURE;
    }
    if(vars.ipv6NetToMediaPhysAddress.len > *ipv6PhysAddrLen){
        return L7_FAILURE;
    }
    *ipv6NetAddress = vars.ipv6NetToMediaNetAddress;
    *ipv6PhysAddrLen = vars.ipv6NetToMediaPhysAddress.len;
    memcpy(ipv6PhysAddress, vars.ipv6NetToMediaPhysAddress.addr, *ipv6PhysAddrLen);
    *ipv6Type = vars.ipv6IfNetToMediaType;
    *ipv6State = vars.ipv6IfNetToMediaState;
    *ipv6LastUpdated = vars.ipv6IfNetToMediaLastUpdated;
    *ipv6IsRtr = vars.ipv6IfNetToMediaIsRouter;
   
    
    return L7_SUCCESS;
}
    
/*********************************************************************
* @purpose  get service port NDisc info
*
* @param    ipv6NetAddress    {in/out} ipv6 neighbor address
* @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
* @param    ipv6PhysAddress   {out} ipv6 neighbor mac
* @param    ipv6Type          {out} ipv6 neighbor mac type
* @param    ipv6State         {out} ipv6 neighbor state 
* @param    ipv6LastUpdated   {out} ipv6 neighbor last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simGetServPortIPV6Ndp( L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr)
{
    L7_uchar8 *ifName = bspapiServicePortNameGet();
    L7_uchar8 qName[32];

    sprintf(qName,"%s%d",ifName,bspapiServicePortUnitGet());
    return simGetNextMgmtPortIPV6Ndp(qName, ipv6NetAddress, ipv6PhysAddrLen,
                                     ipv6PhysAddress, ipv6Type, ipv6State,
                                     ipv6LastUpdated, ipv6IsRtr);
}

/*********************************************************************
 *
 * @purpose Retrieve the IPv6 AutoConfig Mode of Service port
 *
 * @param L7_uint32 *val   @b((output)) Whether autoconfiguration is
 *                                      enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t simServPortIPv6AddrAutoConfigGet(L7_uint32 *val)
{
  *val = simCfgData.servPortIPv6AddrAutoConfig;
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the IPv6 AutoConfig Mode of Service port
 *
 * @param L7_uint32 val   @b((input)) Whether autoconfiguration is
 *                                    enabled or disabled
 *
 * @returns L7_FAILURE    When any of this is true
 *                        - the dhcpv6 protocol is enabled on the service port
 *                        - the dhcpv6 protocol is enabled on the network port
 *                        - the ipv6 address autoconfiguration is enabled on the network port
 *          L7_SUCCESS    otherwise
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t simServPortIPv6AddrAutoConfigSet(L7_uint32 val)
{
  L7_uchar8 ifname[32];

  osapiSnprintf(ifname, sizeof(ifname), "%s%d", bspapiServicePortNameGet(),bspapiServicePortUnitGet());

  if(val != simCfgData.servPortIPv6AddrAutoConfig)
  {
    simCfgData.servPortIPv6AddrAutoConfig = val;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

    osapiIfIpv6AddrAutoConfigSet(ifname, simCfgData.servPortIPv6AddrAutoConfig);

    /* If ipv6 mode is enabled on service port, disable and re-enable ipv6 mode to -
     *
     * (a) flush already autoconfigured ipv6 addresses (while disabling autoconfig mode)
     * (b) trigger the stack to send router solicitation packet out to be able
     *     to quickly facilitate autoconfiguration of ipv6 addresses (while enabling autoconfig mode)
     */
    if(simCfgData.servPortIpv6AdminMode == L7_ENABLE)
    {
      if (osapiIfIpv6Disable(ifname) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
               "Failed to disable ipv6 mode on Service Port while changing ipv6 autoconfig mode.\n");
      }
      if (osapiIfIpv6Enable(ifname) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
               "Failed to enable ipv6 mode on Service Port while changing ipv6 autoconfig mode.\n");
      }
      /* may need to cycle dhcp socket too */
      if(simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP);
      {
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_NOT_COMPLETE,ifname,0);
        dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_COMPLETE,ifname,0);
      }
    }
  }
  return L7_SUCCESS;
}
#endif
