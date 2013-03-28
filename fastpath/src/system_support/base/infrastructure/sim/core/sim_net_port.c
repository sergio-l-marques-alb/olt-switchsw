/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
* @filename   sim_net_port.c
*
* @purpose    Functions related to IP addressing on the system interface 
*             (aka, network port)
*
* @component  System Interface Manager (SIM)
*
* @comments   Some logic moved here from sim.c.
*
*             As this entire directory doesn't have an _api.c file, this file
*             includes both component APIs and implementation functions related
*             to the network port.
*
*             Functions for both IPv4 and IPv6 on the network port are here.
*
* @create     02/23/2009
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
#include "dhcp_client_api.h"

#ifdef L7_NSF_PACKAGE
  #include "sim_ckpt.h"
#endif

extern simCfgData_t      simCfgData;
extern simOperInfo_t     *simOperInfo;
extern L7_uint32         systemCurrentConfigMode;


#define          NUM_IPV6_ADDRS_FROM_STACK       L7_RTR6_MAX_INTF_ADDRS * 2


/*********************************************************************
* @purpose  Get the IPv4 address in use on the network port
*
* @param    none
*
* @returns  network port IPv4 address
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPAddr(void)
{
  if (simOperInfo)
    return simOperInfo->systemIpAddress;
  return 0;
}

/*********************************************************************
* @purpose  Get the configured IPv4 address on the network port
*
* @param    none
*
* @returns  network port IPv4 address
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simConfiguredSystemIPAddrGet(void)
{
  return simCfgData.systemIpAddress;
}

/*********************************************************************
* @purpose  Determine whether the network port IP address is a checkpointed
*           address, not yet confirmed as a valid address following failover.
*
* @param    none
*
* @returns  L7_TRUE if the network port address is a checkpointed address
*
* @comments  Only consider an address to be checkpointed if it is non-zero
*
* @end
*********************************************************************/
L7_BOOL simIsSystemAddrCheckpointed(void)
{
  return (simOperInfo->systemIpAddress && 
          (simOperInfo->systemAddrType == SIM_MGMT_ADDR_CKPT));
}

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
L7_RC_t simSystemOperAddrSet(L7_uint32 ipAddr, L7_uint32 netMask, 
                             e_simMgmtIpAddrType addrType)
{
  if ((simOperInfo->systemIpAddress == ipAddr) &&
      (simOperInfo->systemNetMask == netMask) &&
      (simOperInfo->systemAddrType == addrType))
  {
    return L7_SUCCESS;
  }

  simOperInfo->systemIpAddress = ipAddr;
  simOperInfo->systemNetMask = netMask;
  simOperInfo->systemAddrType = addrType;
  return simSystemIPAddrApply();
}

/*********************************************************************
* @purpose  Tell everyone who needs to know about a change to the IPv4
*           address in use on the network port.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments   
*
* @end
*********************************************************************/
L7_RC_t simSystemIPAddrApply(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Tell the IP stack about the new address */
  sysapiConfigSwitchIp(simOperInfo->systemIpAddress, simOperInfo->systemNetMask);
  if (!simOperInfo->systemIpAddress)
  {
    osapiCleanupIf(L7_DTL_PORT_IF);
  }

  /* Tell hardware about the new address */
  rc = dtlIPAddrSystemSet(simOperInfo->systemIpAddress);   /* no-op */

  /* Tell anyone else who cares */
  simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_NETWORK, L7_AF_INET);

  /* Reset the address conflict detected statistics for the new address */
  simOperInfo->networkPortConflictStats.numOfConflictsDetected = 0;

#ifdef L7_NSF_PACKAGE
    /* IP address change, make a checkpoint request */
  if (simOperInfo->systemAddrType == SIM_MGMT_ADDR_DHCP)
  {
    simCkptRequest(SIM_CKPT_IE_SYSTEM_IP_ADDR);
  }
#endif
 
  return rc;
}

/*********************************************************************
* @purpose  Configure an IPv4 address on the network port
*
* @param    ipAddr   @b{(input)} System IPv4 Address
*
* @returns  L7_SUCCESS
*           L7_ERROR if DHCP is configured on the interface
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSystemIPAddrConfigure(L7_uint32 ipAddr)
{
  /* For the very short term, don't allow user to configure an address
   * if DHCP is enabled on this interface. */
  if (simCfgData.systemConfigMode != L7_SYSCONFIG_MODE_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Rejecting address configured on network port. DHCP enabled.");
    return L7_ERROR;
  }

  simCfgData.systemIpAddress = ipAddr;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  /* Make the configured address the operational address. Return 
   * success even if apply fails. */
  simSystemOperAddrSet(ipAddr, simCfgData.systemNetMask, SIM_MGMT_ADDR_CONF);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configure an IPv4 address with its network mask on the network port
*
* @param    ipAddr   @b{(input)} System IPv4 Address
* @param    netMask  @b{(input)} network mask
*
* @returns  L7_SUCCESS
*           L7_ERROR if DHCP is configured on the interface
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simConfigureSystemIPAddrWithMask(L7_uint32 ipAddr, L7_uint32 netMask)
{
  /* For the very short term, don't allow user to configure an address
   * if DHCP is enabled on this interface. */
  if (simCfgData.systemConfigMode != L7_SYSCONFIG_MODE_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Rejecting address configured on network port. DHCP enabled.");
    return L7_ERROR;
  }

  simCfgData.systemIpAddress = ipAddr;
  simCfgData.systemNetMask = netMask;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  /* Make the configured address the operational address. Return 
   * success even if apply fails. */
  simSystemOperAddrSet(ipAddr, netMask, SIM_MGMT_ADDR_CONF);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the IPv4 address and network mask learned from DCHP
*           or BOOTP on the network port.
*
* @param    dhcpAddr  @b{(input)} System IP Address
* @param    netMask   @b{(input)} network mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t simSetSystemAddrWithMask(L7_uint32 dhcpAddr, L7_uint32 netMask)
{
  if (simCfgData.systemIpAddress != 0)
  {
    /* The DHCP client should only request an address from the DHCP
     * server when no address is configured.  */
    L7_uchar8 dhcpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 cfgAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(dhcpAddr, dhcpAddrStr);
    osapiInetNtoa(simCfgData.systemIpAddress, cfgAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID,
            "DHCP assigned IPv4 address %s to network port, but network"
            " port already has configured address %s.",
            dhcpAddrStr, cfgAddrStr);
    return L7_FAILURE;
  }

  return simSystemOperAddrSet(dhcpAddr, netMask, SIM_MGMT_ADDR_DHCP);
}

/*********************************************************************
* @purpose  Get the network mask in use on the network port
*
* @param    void
*
* @returns  network mask on the network port
*
* @comments   This is the mask in operational use on the network port. May
*             be different from the configured value.
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPNetMask(void)
{
  if (simOperInfo)
    return simOperInfo->systemNetMask;
  return 0;
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
L7_uint32 simConfiguredSystemIPNetMaskGet(void)
{
  return simCfgData.systemNetMask;
}

/*********************************************************************
* @purpose  Configure the network mask on the network port
*
* @param    netMask   @b{(input)} System NetMask
*
* @returns  none
*
* @comments  
*
* @end
*********************************************************************/
void simSystemIPNetMaskConfigure(L7_uint32 netMask)
{
  /* For the very short term, don't allow user to configure an address
   * if DHCP is enabled on this interface. */
  if (simCfgData.systemConfigMode != L7_SYSCONFIG_MODE_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Rejecting network mask configured on network port. DHCP enabled.");
    return;
  }

  if (netMask == simCfgData.systemNetMask)
  {
    return;
  }

  simCfgData.systemNetMask = netMask;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;
  
  /* Make the configured address the operational address. Return 
   * success even if apply fails. */
  simSystemOperAddrSet(simCfgData.systemIpAddress, netMask, SIM_MGMT_ADDR_CONF);
  return;
}

/*********************************************************************
* @purpose  Get the IPv4 default gateway in use on the network port
*
* @param    void
*
* @returns  IPv4 address of the default gateway  
*
* @comments  This is the operational value, not the configured value.
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPGateway(void)
{
  if (simOperInfo)
    return simOperInfo->systemGateway;
  return 0;
}

/*********************************************************************
* @purpose  Get the configured IPv4 default gateway on the network port
*
* @param    void
*
* @returns  IPv4 address of the default gateway  
*
* @comments  This is the configured value, not the operational value.
*
* @end
*********************************************************************/
L7_uint32 simConfiguredSystemIPGatewayGet(void)
{
  return simCfgData.systemGateway;
}

/*********************************************************************
* @purpose  Sets the default gateway associated with the network port 
*           as learned from DHCP or BOOTP.
*
* @param    newGateway  @b{(input)} new System Gateway
*
* @returns  none
*
* @comments Note that we intentionally do not check if newGateway is 
*           different from the current gateway. After a warm restart,
*           the value might not change, but we still need to update
*           the IP stack.
*
* @end
*********************************************************************/
void simSetSystemIPGateway(L7_uint32 newGateway)
{
  simSystemOperGatewaySet(newGateway);
}

/*********************************************************************
* @purpose  Set the operational IPv4 default gateway for the network port.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSystemGatewayApply(void)
{
  if (ipstkDefGwUpdate() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
            "Failed to set the default gateway in the IP stack.");
  }

#ifdef L7_NSF_PACKAGE
    /* IP gateway change, make a checkpoint request */
  if (simOperInfo->systemAddrType == SIM_MGMT_ADDR_DHCP)
  {
    simCkptRequest(SIM_CKPT_IE_SYSTEM_IP_ADDR);
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the operational IPv4 default gateway for the network port.
*
* @param    newGateway  @b{(input)} new IPv4 default gateway on network port
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSystemOperGatewaySet(L7_uint32 newGateway)
{
  simOperInfo->systemGateway = newGateway;
  return simSystemGatewayApply();
}

/*********************************************************************
* @purpose  Configure the IPv4 default gateway for the network port.
*
* @param    newGateway  @b{(input)} new System Gateway
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSystemIPGatewayConfigure(L7_uint32 newGateway)
{
  /* For the very short term, don't allow user to configure an address
   * if DHCP is enabled on this interface. */
  if (simCfgData.systemConfigMode != L7_SYSCONFIG_MODE_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Rejecting default gateway configured on network port. DHCP enabled.");
    return;
  }

  if (newGateway != simCfgData.systemGateway)
  {
    simCfgData.systemGateway = newGateway;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
    simSystemOperGatewaySet(newGateway);
  }
}

/*********************************************************************
* @purpose  Configure the network port to use DHCP, BOOTP, or static
*           IP addresses.
*
* @param    mode  @b{(input)} DHCP, BOOTP, or none
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void simSetSystemConfigMode(L7_SYSCFG_MODE_t mode)
{
  L7_RC_t rc;

  if (simCfgData.systemConfigMode == mode)
    return;

  switch (mode)
  {
    /* These DTL calls all no-ops on current hardware */
  case L7_SYSCONFIG_MODE_NONE:
    rc = dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_NONE);
    break;
  case L7_SYSCONFIG_MODE_BOOTP:
    rc = dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_BOOTP);
    break;
  case L7_SYSCONFIG_MODE_DHCP:
    rc = dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_DHCP);
    break;
  default:
    rc = dtlBootPDhcpSystemConfig(DTL_DHCP_CONFIG_NONE);
    break;

  }

  if (simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    /* Disabling DHCP on the network port. Release address. */
    dhcpClientIPAddressMethodSet (0, L7_INTF_IP_ADDR_METHOD_NONE,
                                  L7_MGMT_NETWORKPORT, L7_TRUE);
  }

  /* If address is checkpointed, retain it. If network port address is 
   * configured, the configured addresses will overwrite the checkpointed
   * address. If enabling DHCP, DHCP will request checkpointed address. */
  if (!simIsSystemAddrCheckpointed())
  {
    if (simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_NONE)
    {
      /* Clear configured address */
      /* ROBRICE - could leave configured address in place. If DHCP is 
       * enabled, will use configured address and send INFORM to get 
       * other parameters. */
      simSystemIPGatewayConfigure(0);
      simSystemIPAddrConfigure(0);
      simSystemIPNetMaskConfigure(0);
    }
    else if ((simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
             (simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_BOOTP))
    {
      simSetSystemIPGateway(0);
      simSetSystemAddrWithMask(0, 0);
    }
  }

  simCfgData.systemConfigMode = mode;
  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  if (simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    /* Enable DHCP on the network port. */
    dhcpClientIPAddressMethodSet (0, L7_INTF_IP_ADDR_METHOD_DHCP,
                                  L7_MGMT_NETWORKPORT, L7_FALSE);
  }
}

/*********************************************************************
* @purpose  Get the Unit's System Config Mode
*
* @param    none
*
* @returns  mode  System Config Mode
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemConfigMode(void)
{
  return(simCfgData.systemConfigMode);
}

/*********************************************************************
* @purpose  Get the Unit's System Current Config Mode
*
* @param    none
*
* @returns  mode  System Current Config Mode
*
* @comments   This function is deprecated. I believe it is only called 
*             from a dead web page. Use simGetSystemConfigMode();
*
* @end
*********************************************************************/
L7_uint32 simGetSystemCurrentConfigMode(void)
{
  return L7_SYSCONFIG_MODE_NONE;
}

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
/*********************************************************************
* @purpose  Set the DHCPv6 operational address on the network port.
*
* @param    ip6Addr  @b{(input)} IPv6 address on network port
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSystemDhcpv6AddrSet(L7_in6_addr_t* ip6Addr)
{
  memcpy(&simOperInfo->systemDhcpv6Addr, ip6Addr, sizeof(L7_in6_addr_t));

#ifdef L7_NSF_PACKAGE
  /* Dynamic IPv6 address change, make a checkpoint request */
  simCkptRequest(SIM_CKPT_IE_SYSTEM_IP6_ADDR);
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear the DHCPv6 operational address on the network port.
*           This is called when we disable DHCPv6 Client protocol on
*           the network port once we released the address.
*
* @param    None
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSystemDhcpv6AddrClear(void)
{
  memset(&simOperInfo->systemDhcpv6Addr, 0, sizeof(L7_in6_addr_t));

#ifdef L7_NSF_PACKAGE
  /* Dynamic IPv6 address change, make a checkpoint request */
  simCkptRequest(SIM_CKPT_IE_SYSTEM_IP6_ADDR);
#endif

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DHCPv6 operational address on the network port
*
* @param    ip6Addr  @b{(output)} IPv6 address on network port
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSystemDhcpv6AddrGet(L7_in6_addr_t* ip6Addr)
{
  memcpy(ip6Addr, &simOperInfo->systemDhcpv6Addr, sizeof(L7_in6_addr_t));

  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Activates/Deactivates one of the Unit's network port IPv6 prefixes.
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
static L7_RC_t simActivateSystemIPV6Prefix (L7_in6_addr_t *ip6Addr,
                                L7_uint32 ip6PrefixLen,
                                L7_uint32 enable_flag)
{
  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (enable_flag)
  {
    if (sysapiConfigSystemIPV6PrefixAdd(ip6Addr, ip6PrefixLen)
        != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    (void)sysapiConfigSystemIPV6PrefixRemove(ip6Addr, ip6PrefixLen);
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the Unit's System (Network Port) IPV6 Admin maode
*
* @param    none
*
* @returns  adminMode    L7_ENABLE or L7_DISABLE 
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPV6AdminMode(void)
{
  return simCfgData.systemIpv6AdminMode;
}

/*********************************************************************
* @purpose  Sets the Unit's System (Network Port) IPV6 Admin Mode
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
L7_RC_t simSetSystemIPV6AdminMode(L7_uint32 adminMode)
{
  L7_uchar8 *ifName = L7_DTL_PORT_IF;
  L7_uchar8 qName[L7_DTL_PORT_IF_LEN +2];
  L7_uint32 i = 0;

  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  sprintf(qName,"%s%d",ifName,0);

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
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
        {
          continue;
        }

        (void)simActivateSystemIPV6Prefix(&simCfgData.systemIpv6Intfs[i].ip6Addr,
                                          simCfgData.systemIpv6Intfs[i].ip6PrefixLen,
                                          L7_TRUE);
      }

      /* Enable DHCPv6 operationally on the network port if config-enabled */
      if((simCfgData.systemIpv6AdminMode != adminMode) &&
         (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP))
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_DHCP, qName,0);
      }

      break;

    case L7_DISABLE:

      /* De-activate configured IPv6 prefixes */
      for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
      {
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
        {
          continue;
        }

        (void)simActivateSystemIPV6Prefix(&simCfgData.systemIpv6Intfs[i].ip6Addr,
                                          simCfgData.systemIpv6Intfs[i].ip6PrefixLen,
                                          L7_FALSE);
      }

      /* Disable DHCPv6 operationally on the network port if config-enabled */
      if((simCfgData.systemIpv6AdminMode != adminMode) &&
         (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP))
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_NONE, qName,0);
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

  if (simCfgData.systemIpv6AdminMode != adminMode)
  {
    simCfgData.systemIpv6AdminMode = adminMode;
    ipstkIpv6DefGwUpdate();
    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on network port interface
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
L7_RC_t simGetSystemIPV6Addrs( L7_in6_prefix_t *addrs, L7_uint32 *acount)
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

    if(simCfgData.systemIpv6AdminMode == L7_DISABLE)
    {
      *acount = 0;
      return L7_SUCCESS;
    }
    sprintf(ifName,"%s%d",L7_DTL_PORT_IF,0);
    rc = osapiIfIpv6AddrsGet(ifName, ip6Addr,&count);
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
        if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
        {
          continue;
        }
        memcpy(&addrs[totalcount].in6Addr, &simCfgData.systemIpv6Intfs[i].ip6Addr, 
                   sizeof(L7_in6_addr_t));
        addrs[totalcount].in6PrefixLen = 
                           simCfgData.systemIpv6Intfs[i].ip6PrefixLen;
        totalcount++;
      }

       /* Next get either the autoconfigured or dhcpv6 client assigned global ipv6 addresses */
       if((simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
          (simCfgData.systemIPv6AddrAutoConfig == L7_ENABLE))
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
             if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[j].ip6Addr))
             {
               continue;
             }
             if(memcmp(&simCfgData.systemIpv6Intfs[j].ip6Addr, &ip6Addr[i], sizeof(L7_in6_addr_t)) == 0)
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
 * @purpose  Get next of the Unit's network port IPv6 configured prefixes.
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
L7_RC_t simGetNextSystemIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 *ip6PrefixLen,
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
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
    {
      continue;
    }

    if (foundIndex != -1)
    {
      /* This is the "next" entry, so copy and return */
      memcpy(ip6Addr, &simCfgData.systemIpv6Intfs[i].ip6Addr, sizeof(L7_in6_addr_t));
      *ip6PrefixLen = simCfgData.systemIpv6Intfs[i].ip6PrefixLen;
      *eui_flag = simCfgData.systemIpv6Intfs[i].eui_flag;
      return L7_SUCCESS;
    }

    if ((memcmp(&simCfgData.systemIpv6Intfs[i].ip6Addr, ip6Addr, sizeof(L7_in6_addr_t)) == 0) &&
        (simCfgData.systemIpv6Intfs[i].ip6PrefixLen == *ip6PrefixLen) &&
        (simCfgData.systemIpv6Intfs[i].eui_flag == *eui_flag))
    {
        /* Found the "previous" entry */
        foundIndex = i;
    }
  }

  /* "next" entry not found */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  get network port NDisc info
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
L7_RC_t simGetSystemIPV6Ndp( L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr)
{
    L7_uchar8 *ifName = L7_DTL_PORT_IF;
    L7_uchar8 qName[L7_DTL_PORT_IF_LEN +2];

    sprintf(qName,"%s%d",ifName,0);
    return simGetNextMgmtPortIPV6Ndp(qName, ipv6NetAddress, ipv6PhysAddrLen,
                                     ipv6PhysAddress, ipv6Type, ipv6State,
                                     ipv6LastUpdated, ipv6IsRtr);
}

/*********************************************************************
* @purpose  Get the Unit's System IPv6 Config Mode
*
* @param    none
*
* @returns  mode  System Config Mode
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 simGetSystemIPv6ConfigMode(void)
{
  return(simCfgData.systemIPv6ConfigMode);
}

/*********************************************************************
 *
 * @purpose Retrieve the IPv6 AutoConfig Mode of Network port
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
L7_RC_t simSystemIPv6AddrAutoConfigGet(L7_uint32 *val)
{
  *val = simCfgData.systemIPv6AddrAutoConfig;
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the IPv6 AutoConfig Mode of Network port
 *
 * @param L7_uint32 val   @b((input)) Whether autoconfiguration is
 *                                    enabled or disabled
 *
 * @returns L7_FAILURE    When any of this is true
 *                        - the dhcpv6 protocol is enabled on the network port
 *                        - the dhcpv6 protocol is enabled on the service port
 *                        - the ipv6 address autoconfiguration is enabled on the service port
 *          L7_SUCCESS    otherwise
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t simSystemIPv6AddrAutoConfigSet(L7_uint32 val)
{
  L7_char8 ifname[32];

  if(val != simCfgData.systemIPv6AddrAutoConfig)
  {
    simCfgData.systemIPv6AddrAutoConfig = val;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

    sprintf(ifname, "%s%d", L7_DTL_PORT_IF, 0);
    osapiIfIpv6AddrAutoConfigSet(ifname, simCfgData.systemIPv6AddrAutoConfig);

    /* If ipv6 mode is enabled on network port, disable and re-enable ipv6 mode to -
     *
     * (a) flush already autoconfigured ipv6 addresses (while disabling autoconfig mode)
     * (b) trigger the stack to send router solicitation packet out to be able
     *     to quickly facilitate autoconfiguration of ipv6 addresses (while enabling autoconfig mode)
     */
    if(simCfgData.systemIpv6AdminMode == L7_ENABLE)
    {
      if (osapiIfIpv6Disable(ifname) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
               "Failed to disable ipv6 mode on Network Port while changing ipv6 autoconfig mode.\n");
      }
      if (osapiIfIpv6Enable(ifname) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
               "Failed to enable ipv6 mode on Network Port while changing ipv6 autoconfig mode.\n");
      }
      /* may need to cycle dhcp socket too */
      if(simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP);
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_NOT_COMPLETE,ifname,0);
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_COMPLETE,ifname,0);
      }
    }
  }
  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Get the auto-configured link-local address of the management port
 *
 * @param    ip6Addr        @b{(input)}   IPv6 address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    This function updates the ip6Addr argument with the
 *              autoconfigured eui-64 format link local address of
 *              the management port (service port or network port),
 *              irrespective of the ipv6 state of the mgmt interface.
 *
 * @end
 *************************************************************************/
L7_RC_t simGetSystemLinkLocalIPV6Addr (L7_in6_addr_t *ip6Addr)
{
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  simMacAddrGet(mac);
  memset(ip6Addr, 0, sizeof(L7_in6_addr_t));

  ip6Addr->in6.addr8[0] = 0xfe;
  ip6Addr->in6.addr8[1] = 0x80;
  ip6Addr->in6.addr8[8] = mac[0] ^ 0x02;
  ip6Addr->in6.addr8[9] = mac[1];
  ip6Addr->in6.addr8[10] = mac[2];
  ip6Addr->in6.addr8[11] = 0xff;
  ip6Addr->in6.addr8[12] = 0xfe;
  ip6Addr->in6.addr8[13] = mac[3];
  ip6Addr->in6.addr8[14] = mac[4];
  ip6Addr->in6.addr8[15] = mac[5];

  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Sets one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 * @param    force          @b{(input)}   Issue set without updating config
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Check that the IPv6 address being added on the network port
 *           is not conflicting with the following addresses:-
 *
 *           (a) Already configured IPv6 prefix on network port
 *           (b) Autoconfigured or dhcpv6 learnt IPv6 address on network port
 *           (c) Configured network port IPv6 gateway
 *           (d) Above 3 cases on service port
 *           (e) IPv6 prefix on any routing interface
 *
 * @end
 *************************************************************************/
L7_RC_t simSetSystemIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
                                L7_uint32 eui_flag, L7_uint32 force)
{
  L7_int32 i = 0, emptyIndex = -1;
  L7_in6_addr_t tempIp6Addr;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&tempIp6Addr, 0, sizeof(tempIp6Addr)); 
  memcpy(&tempIp6Addr, ip6Addr, sizeof(tempIp6Addr));

  if (eui_flag == L7_TRUE)
  {
    if(ip6PrefixLen != 64)
    {
      return L7_FAILURE;
    }
    simMacAddrGet(mac);
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
    /* Check for conflict with service port prefix */
    if(SERVICE_PORT_PRESENT)
    {
      if((simServPortIPV6AddressConflictCheck(ip6Addr, ip6PrefixLen, L7_TRUE)) 
                             != L7_SUCCESS)
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
    }

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

    /* find a empty slot for network port */
    for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
    {
      if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
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
    memcpy(&simCfgData.systemIpv6Intfs[emptyIndex].ip6Addr, ip6Addr,
           sizeof(L7_in6_addr_t));
    simCfgData.systemIpv6Intfs[emptyIndex].ip6PrefixLen = ip6PrefixLen;
    simCfgData.systemIpv6Intfs[emptyIndex].eui_flag = eui_flag;

    simCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  (void)simActivateSystemIPV6Prefix(ip6Addr, ip6PrefixLen,  L7_TRUE);

  return L7_SUCCESS;
}

/**************************************************************************
 * @purpose  Removes one of the Unit's network port IPv6 prefixes.
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
L7_RC_t simDeleteSystemIPV6Prefix (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
                                   L7_uint32 eui_flag)
{
  L7_int32 i = 0, foundIndex = -1;
  L7_in6_addr_t tempIp6Addr;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

  if (ip6Addr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&tempIp6Addr,0,sizeof(tempIp6Addr));
  memset(mac, 0, L7_MAC_ADDR_LEN);
  memcpy(&tempIp6Addr,ip6Addr,sizeof(tempIp6Addr));

  if (eui_flag == L7_TRUE)
  {
    if(ip6PrefixLen != 64)
      return L7_FAILURE;

    simMacAddrGet(mac);
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
    if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
    {
        continue;
    }

    if ((memcmp(&simCfgData.systemIpv6Intfs[i].ip6Addr, ip6Addr, sizeof(L7_in6_addr_t)) == 0) &&
        (simCfgData.systemIpv6Intfs[i].ip6PrefixLen == ip6PrefixLen))
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

  (void)simActivateSystemIPV6Prefix(ip6Addr, ip6PrefixLen, L7_FALSE);

  /* Remove the IPv6 prefix from the config */
  memset(&simCfgData.systemIpv6Intfs[foundIndex].ip6Addr, 0, sizeof(L7_in6_addr_t));
  simCfgData.systemIpv6Intfs[foundIndex].ip6PrefixLen = 0;
  simCfgData.systemIpv6Intfs[foundIndex].eui_flag = 0;

  simCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on network port interface
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
L7_RC_t simGetSystemIPV6DefaultRouters( L7_in6_addr_t *addrs, L7_uint32 *acount)
{
  if (addrs == L7_NULLPTR || acount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return osapiIfIpv6DefaultRoutersGet(L7_DTL_PORT_IF, addrs,acount);
}

/**************************************************************************
*
* @purpose  Get configured IPv6 gateway on network port interface
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
L7_RC_t simGetSystemIPV6Gateway( L7_in6_addr_t *gateway)
{
  if (gateway == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
  {
    return L7_FAILURE;
  }
  else
  {
    memcpy(gateway, &simCfgData.systemIpv6Gateway, sizeof(L7_in6_addr_t));
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Set configured IPv6 gateway on network port interface
*
* @param    gateway   @b{(input)}   pointer to gateway address
* @param    force     @b{(input)}   Issue set without updating config
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Check that the IPv6 gateway being added on the serviceport
*           is not conflicting with the following addresses:-
*
*           (a) Already configured IPv6 addresses on service port
*           (b) Autoconfigured or dhcpv6 learnt IPv6 address on service port
*           (c) Above 2 cases on network port
*           (d) Configured service port IPv6 gateway
*           (e) IPv6 address on any routing interface
*
* @end
*
*************************************************************************/
L7_RC_t simSetSystemIPV6Gateway( L7_in6_addr_t *gateway, L7_uint32 force)
{

  if (gateway == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (L7_IP6_IS_ADDR_UNSPECIFIED(gateway))
  {
    /* We are attempting to delete the existing gateway */
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
    {
        if (force == L7_FALSE)
        {
          memset(&simCfgData.systemIpv6Gateway, 0, sizeof(L7_in6_addr_t));
          simCfgData.cfgHdr.dataChanged = L7_TRUE;
        }
    }
  }
  else
  {
    /* Check the conflict of the IPv6 gateway address with any other
     * IPv6 address in the switch */

    if(SERVICE_PORT_PRESENT)
    {
      /* Check for conflict with service port addresses */
      if((simServPortIPV6AddressConflictCheck(gateway, 128, L7_FALSE))
                                              != L7_SUCCESS)
      {
        return L7_ADDR_INUSE;
      }

      /* Check for conflict with configured service port ipv6 gateway */
      if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
      {
        if(L7_IP6_IS_ADDR_EQUAL(&simCfgData.servPortIpv6Gateway, gateway))
        {
          return L7_ADDR_INUSE;
        }
      }
    }

    /* Check for conflict with network port addresses */
    if((simSystemIPV6AddressConflictCheck(gateway, 128, L7_FALSE)) != L7_SUCCESS)
    {
      return L7_ADDR_INUSE;
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
        memcpy(&simCfgData.systemIpv6Gateway, gateway, sizeof(L7_in6_addr_t));
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
  }
  ipstkIpv6DefGwUpdate();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Unit's System IPv6 Config Mode
*
* @param    mode  @b{(input)} System Config Mode
*
* @returns L7_FAILURE    When any of this is true
*                        - the ipv6 address autoconfiguration is enabled on the network port
*                        - the ipv6 address autoconfiguration is enabled on the service port
*                        - the dhcpv6 protocol is enabled on the service port
*          L7_SUCCESS    otherwise
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t simSetSystemIPv6ConfigMode(L7_uint32 mode)
{
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE];

  snprintf(ifName,L7_NIM_IFNAME_SIZE,"%s0",L7_DTL_PORT_IF);

  if(mode != simCfgData.systemIPv6ConfigMode)
  {
    simCfgData.systemIPv6ConfigMode = mode;
    simCfgData.cfgHdr.dataChanged = L7_TRUE;

    if (simCfgData.systemIpv6AdminMode == L7_ENABLE)
    {
      if (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_DHCP, ifName,0);
      }
      else if(simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_NONE)
      {
        dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_NONE, ifName,0);
      }
    }
  }

  return L7_SUCCESS;
}
#endif
