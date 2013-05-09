/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dhcp_api.c
*
* @purpose dhcp configuration information file
*
* @component dhcp
*
* @comments 
*
* @create  3/03/2008
*
* @author  ddevi
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "l7_dhcp.h"
#include "usmdb_sim_api.h"
#include "dhcp_debug.h"

/*********************************************************************
                         Globals
**********************************************************************/
extern struct bootp l7_eth0_bootp_data;
extern struct bootp l7_eth1_bootp_data;

/*********************************************************************
* @purpose  Gets the DHCP Vendor Class Option status
*
* @param    optionAdminMode   @b((output)) DHCP Vendor Class Option admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpVendorClassOptionAdminModeGet(L7_uint32 * optionAdminMode )
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (optionAdminMode == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (osapiSemaTake (dhcpClientCB.cbMembersProtectionSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Release Sem");
    return L7_FAILURE;
  }

  *optionAdminMode = dhcpClientCB.dhcpOptions.vendorClassOption.optionMode;

  osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "optionAdminMode - %d", *optionAdminMode);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the DHCP Vendor Class Option status
*
* @param    optionAdminMode   @b((input)) DHCP Vendor Class Option admin mode
* @param    actImmediate      @b((input)) Immediate action flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If actImmediate is True, the setting will be applied
*           immediately without posting an event to the DHCP Client
*           task.  Use this option only when there are synchronization
*           issues, especially with SETS.
*
* @end
*********************************************************************/
L7_RC_t
dhcpVendorClassOptionAdminModeSet (L7_uint32 optionAdminMode,
                                   L7_BOOL actImmediate)
{
  dhcpClientQueueMsg_t msg;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  /* TODO: Need to protect these */
  if (actImmediate == L7_TRUE)
  {
    if (dhcpVendorClassOptionAdminModeUpdate (optionAdminMode) != L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE "
                   "Event Processing Failed");
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }

  memset (&msg, 0, sizeof(dhcpClientQueueMsg_t));
  msg.u.dhcpClientUIEventParms.event = DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE;
  msg.u.dhcpClientUIEventParms.vendorClassOptionMode = optionAdminMode;

  if (dhcpClientUIEventPost (&msg) != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING "
                 "Event Post Failed");
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gets the DHCP Vendor Class Option string
*
* @param    optionString  @b((output)) DHCP Vendor Class Option string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpVendorClassOptionStringGet(L7_uchar8 * optionString )
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (optionString == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (osapiSemaTake (dhcpClientCB.cbMembersProtectionSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Release Sem");
    return L7_FAILURE;
  }

  osapiStrncpy(optionString,
               (L7_uchar8 *)(dhcpClientCB.dhcpOptions.vendorClassOption.vendorClassString),
               DHCP_VENDOR_CLASS_STRING_MAX);

  osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "optionString - %s", optionString);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the DHCP Vendor Class Option string
*
* @param    optionString   @b((input)) DHCP Vendor Class Option string
* @param    actImmediate   @b((input)) Immediate action flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If actImmediate is True, the setting will be applied
*           immediately without posting an event to the DHCP Client
*           task.  Use this option only when there are synchronization
*           issues, especially with SETS.
*
* @end
*********************************************************************/
L7_RC_t
dhcpVendorClassOptionStringSet (L7_uchar8 *optionString,
                                L7_BOOL actImmediate)
{
  dhcpClientQueueMsg_t msg;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  /* TODO: Need to protect these */
  if (actImmediate == L7_TRUE)
  {
    if (dhcpVendorClassOptionStringUpdate (optionString) != L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING "
                   "Event Processing Failed");
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }

  memset (&msg, 0, sizeof(dhcpClientQueueMsg_t));
  msg.u.dhcpClientUIEventParms.event = DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING;
  osapiStrncpy (msg.u.dhcpClientUIEventParms.vendorClassOptionString,
                optionString, DHCP_VENDOR_CLASS_STRING_MAX);

  if (dhcpClientUIEventPost (&msg) != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING "
                 "Event Post Failed");
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gets the DHCP Specific Option.
*
* @param    option   @b((inout)) DHCP Vendor Specific Option
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The caller is responsible for decode suboptions under this 
*           option as the content is vendor specific. DHCP client to be generic 
*           does not decode them.
*
* @end
*********************************************************************/
L7_RC_t dhcpVendorSpecificOptionGet(dhcpVendorSpecificOption_t *option)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.cbMembersProtectionSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Release Sem");
    return L7_FAILURE;
  }

  osapiStrncpy((L7_uchar8 *)option, (L7_uchar8 *)(&dhcpClientCB.dhcpOptions.vendorSpecificOption), 
              sizeof (dhcpVendorSpecificOption_t));

  osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Checks if the given Ip is learnt from DHCP DNS Option(6) value.
*
* @param    option   @b((inout)) DHCP DNS Option - 6 Value
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This is used by DNS show commands and running config commands
*           to differentiate between servers configured by user and servers 
*           learnt from the DHCP  .
*
* @end
*********************************************************************/
L7_BOOL dhcpDNSOptionIsPresent(L7_inet_addr_t *option)
{
  L7_uint32 spMode, npMode, dnsServerIpAddr[L7_DNS_NAME_SERVER_ENTRIES] = {0};
  L7_inet_addr_t inetAddr;
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_MGMT_PORT_TYPE_t mgmtPortType;
  L7_uint32 count =0;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &spMode);
  if (spMode == L7_SYSCONFIG_MODE_DHCP)
  {
    mgmtPortType = L7_MGMT_SERVICEPORT;
  }
  else
  {
    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &npMode);
    if (npMode == L7_SYSCONFIG_MODE_DHCP)
    {
      mgmtPortType = L7_MGMT_NETWORKPORT;
    }
    else
    {
      return L7_FALSE;
    }
  }

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }
  if ((clientIntfInfo = dhcpClientIntfInfoGet (0, mgmtPortType)) == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for Mgmt Port - %d",
                 mgmtPortType);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }
  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on Mgmt Port - %d",
                 mgmtPortType);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (l7_get_bootp_option(&clientIntfInfo->txBuf, TAG_DOMAIN_SERVER, dnsServerIpAddr,
                          sizeof(dnsServerIpAddr) )!= L7_TRUE)
  {
    dnsServerIpAddr[0] = 0;
  }

  for (count=0; count<L7_DNS_NAME_SERVER_ENTRIES; count++) 
  { 
    if (dnsServerIpAddr[count]== 0)
    {
      break;
    }
    inetAddressSet(L7_AF_INET, &dnsServerIpAddr[count], &inetAddr);
    if (L7_INET_ADDR_COMPARE(option, &inetAddr) == L7_NULL)
    {
      osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
      return L7_TRUE;
    }
  }

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Checks if the given domain name is learnt from DHCP
*
* @param    domain   @b((inout)) Domain Name
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This is used by DNS show commands and running config commands
*           to differentiate between domain names configured by user
*           and servers learnt from the DHCP  .
*
* @end
*********************************************************************/
L7_BOOL dhcpDomainNameOptionIsPresent(L7_char8 *domain)
{
  L7_uint32 spMode, npMode;
  L7_char8  domainName[L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX+1];
  L7_MGMT_PORT_TYPE_t mgmtPortType;
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  memset(domainName, 0, sizeof(domainName));

  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &spMode);
  if (spMode == L7_SYSCONFIG_MODE_DHCP)
  {
    mgmtPortType = L7_MGMT_SERVICEPORT;
  }
  else
  {
    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &npMode);
    if (npMode == L7_SYSCONFIG_MODE_DHCP)
    {
      mgmtPortType = L7_MGMT_NETWORKPORT;
    }
    else
    {
      return L7_FALSE;
    }
  }

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }
  if ((clientIntfInfo = dhcpClientIntfInfoGet (0, mgmtPortType)) == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for Mgmt Port - %d",
                 mgmtPortType);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }
  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on Mgmt Port - %d",
                 mgmtPortType);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  (void)l7_get_bootp_option(&clientIntfInfo->txBuf, TAG_DOMAIN_NAME, domainName,
                            sizeof(domainName));

  if (osapiStrncmp(domain, domainName, sizeof(domainName)) == 0)
  {
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_TRUE;
  }

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Acquire/Release an IP Address on the interface from the 
*           DHCP Server
*
* @param    intIfNum     @b((input)) Internal Interface Number
* @param    method       @b((input)) IP Address Configuration method
* @param    mgmtPortType @b((input)) Mgmt Interface Type
* @param    actImmediate @b((input)) Immediate action flag
*
* @returns  L7_SUCCESS, if the Acquire/Release/Renew Event Send process
*                       is successful
* @returns  L7_FAILURE, if the Acquire/Release/Renew Event Send process
*                       failed
*
* @notes    If actImmediate is True, the setting will be applied
*           immediately without posting an event to the DHCP Client
*           task.  Use this option only when there are synchronization
*           issues, especially with SETS.
*           NOTE: Set actImmediate flag is currently supported for
*                 Release event only.  Use this option cautiously 
*                 only in case of need.
*           NOTE: If the actImmediate flag is set and if the 
*                 mgmtPortType is IPPORT, intIfNum points to rtrIfNum.
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientIPAddressMethodSet (L7_uint32 intIfNum,
                              L7_INTF_IP_ADDR_METHOD_t method,
                              L7_MGMT_PORT_TYPE_t mgmtPortType,
                              L7_BOOL actImmediate)
{
  DHCP_CLIENT_UI_EVENT_TYPE_t eventType;
  dhcpClientQueueMsg_t msg;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (method == L7_INTF_IP_ADDR_METHOD_DHCP)
  {
    eventType = DHCP_CLIENT_ADDRESS_ACQUIRE;
  }
  else if (method == L7_INTF_IP_ADDR_METHOD_NONE)
  {
    if (actImmediate == L7_TRUE)
    {
      if (mgmtPortType == L7_MGMT_IPPORT)
      {
        dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
        if ((clientIntfInfo = dhcpClientIntfInfoByRtrIfNumGet (intIfNum, mgmtPortType))
                                                            == L7_NULLPTR)
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for rtrIfNum-%d",
                       intIfNum);
          osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
          return L7_FAILURE;
        }
        clientIntfInfo->actImmediate = actImmediate;
        dhcpClientImmediateReleaseInitiate (clientIntfInfo);
        clientIntfInfo->actImmediate = L7_FALSE;
      }
      else
      {
        return dhcpClientReleaseProcess (intIfNum, mgmtPortType);
      }
    }
    eventType = DHCP_CLIENT_ADDRESS_RELEASE;
  }
  else if (method == L7_INTF_IP_ADDR_RENEW)
  {
    eventType = DHCP_CLIENT_ADDRESS_RENEW;
  }
  else
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Invalid Method-%d", method);
    return L7_FAILURE;
  }

  memset (&msg, 0, sizeof(dhcpClientQueueMsg_t));

  msg.u.dhcpClientUIEventParms.intIfNum = intIfNum;
  msg.u.dhcpClientUIEventParms.event = eventType;
  msg.u.dhcpClientUIEventParms.mgmtPortType = mgmtPortType;

  if (dhcpClientUIEventPost (&msg) != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "UI Event-%d post Failed", eventType);
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Acquire an IP Address on the interface from DHCP Server
*
* @param    intIfNum  @b((input)) Internal Interface Number
* @param    ipAddr    @b((input)) IP Address to be configured
* @param    ipAddr    @b((input)) Network Mask to be configured
* @param    ipGateway @b((input)) Default Gateway to be configured
*
* @returns  L7_SUCCESS, if the Configuration Set Event Send process
*                       is successful
* @returns  L7_FAILURE, if the Configuration Set Event Send process
*                       is un-successful
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientConfiguredIPAddressSet (L7_MGMT_PORT_TYPE_t mgmtPortType,
                                  L7_uint32 ipAddr,
                                  L7_uint32 netMask,
                                  L7_uint32 ipGateway)
{
  dhcpClientQueueMsg_t msg;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  memset (&msg, 0, sizeof(dhcpClientQueueMsg_t));

  msg.u.dhcpClientUIEventParms.intIfNum = 0;
  msg.u.dhcpClientUIEventParms.event = DHCP_CLIENT_ADDRESS_CONFIG;
  msg.u.dhcpClientUIEventParms.mgmtPortType = mgmtPortType;

  msg.u.dhcpClientUIEventParms.ipAddr = ipAddr;
  msg.u.dhcpClientUIEventParms.netMask = netMask;
  msg.u.dhcpClientUIEventParms.ipGateway = ipGateway;

  if (dhcpClientUIEventPost (&msg) != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "UI Event-%d post Failed",
                 msg.u.dhcpClientUIEventParms.event);
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Debug Packet Tracing Flags in the DHCP Client
*
* @param    rxFlag    @b((input)) Debug the incoming DHCP Packets
* @param    txFlag    @b((input)) Debug the outgoing DHCP Packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientDebugTraceFlagSet (L7_BOOL rxFlag,
                             L7_BOOL txFlag)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (rxFlag == L7_TRUE)
  {
    dhcpClientDebugFlagSet (DHCPC_DEBUG_PKT_SRVC_RX);
  }
  if (txFlag == L7_TRUE)
  {
    dhcpClientDebugFlagSet (DHCPC_DEBUG_PKT_SRVC_TX);
  }

  if (rxFlag == L7_FALSE)
  {
    dhcpClientDebugFlagReset (DHCPC_DEBUG_PKT_SRVC_RX);
  }
  if (txFlag == L7_FALSE)
  {
    dhcpClientDebugFlagReset (DHCPC_DEBUG_PKT_SRVC_TX);
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Debug Packet Tracing Flags in the DHCP Client
*
* @param    rxFlag    @b((output)) Pointer to the Reception Debug Flag
* @param    txFlag    @b((output)) Pointer to the Transmission Debug Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientDebugTraceFlagGet (L7_BOOL *rxFlag,
                             L7_BOOL *txFlag)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  *rxFlag = dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_RX);
  *txFlag = dhcpClientDebugServiceabilityFlagCheck (DHCPC_DEBUG_PKT_SRVC_TX);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DHCP Server IP Address on an interface
*
* @param    intIfNum    @b((input))  Internal Interface Number
* @param    serverAddr  @b((output)) Pointer to the DHCP Server IP Address
*                                    on an interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientDhcpServerIPAddressGet (L7_uint32 intIfNum,
                                  L7_IP_ADDR_t *serverAddr)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_bootp_dhcp_t dhcpParams;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, L7_MGMT_IPPORT))
                                            == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                               &dhcpParams)
                            != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP Params Get Failed on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  *serverAddr = (L7_IP_ADDR_t) clientIntfInfo->dhcpInfo.server_ip.s_addr;

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "serverAddr - 0x%x", *serverAddr);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DHCP Client IP Address on an interface
*
* @param    intIfNum    @b((input))  Internal Interface Number
* @param    clientAddr  @b((output)) Pointer to the DHCP Client IP Address
*                                    of an interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientIPAddressGet (L7_uint32 intIfNum,
                        L7_IP_ADDR_t *clientAddr)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_bootp_dhcp_t dhcpParams;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, L7_MGMT_IPPORT))
                                            == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                               &dhcpParams)
                            != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP Params Get Failed on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  *clientAddr = (L7_IP_ADDR_t) dhcpParams.ip;

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "clientAddr - 0x%x", *clientAddr);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DHCP Client Subnet Mask on an interface
*
* @param    intIfNum    @b((input))  Internal Interface Number
* @param    clientMask  @b((output)) Pointer to the Subnet Mask of the 
*                                    interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientNetworkMaskGet (L7_uint32 intIfNum,
                          L7_IP_MASK_t *clientMask)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_bootp_dhcp_t dhcpParams;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, L7_MGMT_IPPORT))
                                            == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                               &dhcpParams)
                            != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP Params Get Failed on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  *clientMask = (L7_IP_MASK_t) dhcpParams.netMask;

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "clientMask - 0x%x", *clientMask);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the current state of the DHCP Client
*
* @param    intIfNum    @b((input))  Internal Interface Number
* @param    clientState @b((output)) Pointer to the DHCP Client
*                                    State
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientStateGet (L7_uint32 intIfNum,
                    L7_DHCP_CLIENT_STATE_t *clientState)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, L7_MGMT_IPPORT))
                                            == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  *clientState = clientIntfInfo->dhcpState;

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "clientState - %d", *clientState);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Trasaction ID of the DHCP Client
*
* @param    intIfNum    @b((input))  Internal Interface Number
* @param    transId     @b((output)) Pointer to the DHCP Client
*                                    Transaction ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientTransactionIdGet (L7_uint32 intIfNum,
                            L7_uint32 *transId)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, L7_MGMT_IPPORT))
                                            == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  *transId = clientIntfInfo->transID;

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "transId - 0x%x", *transId);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Lease Time information of the DHCP Client
*
* @param    intIfNum    @b((input))  Internal Interface Number
* @param    leaseTime   @b((output)) Pointer to the DHCP Client
*                                    Lease Time
* @param    renewalTime @b((output)) Pointer to the DHCP Client
*                                    Renewal Time
* @param    rebindTime  @b((output)) Pointer to the DHCP Client
*                                    Rebinding Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientLeaseTimeInfoGet (L7_uint32 intIfNum,
                            L7_uint32 *leaseTime,
                            L7_uint32 *renewalTime,
                            L7_uint32 *rebindTime)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_uint32 currentTime = osapiUpTimeRaw();
  L7_uint32 expiry, T1, T2, uptime;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, L7_MGMT_IPPORT))
                                            == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  uptime = clientIntfInfo->leaseInfo.expiry_uptime/1000;
  expiry = clientIntfInfo->leaseInfo.net_expiry + uptime;
  T1 = clientIntfInfo->leaseInfo.net_t1 + uptime;
  T2 = clientIntfInfo->leaseInfo.net_t2 + uptime;

  *leaseTime = ((int)(expiry - currentTime) > 0 )?(expiry - currentTime):0;
  *renewalTime = ((int)(T1 - currentTime) > 0 )?(T1 - currentTime):0;
  *rebindTime = ((int)(T2 - currentTime) > 0 )?(T2 - currentTime):0;

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "leaseTime - %d secs", *leaseTime);
  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "renewalTime - %d secs", *renewalTime);
  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "rebindTime - %d secs", *rebindTime);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Trasaction ID of the DHCP Client
*
* @param    intIfNum    @b((input))  Internal Interface Number
* @param    retryCount  @b((output)) Pointer to the DHCP Client's
*                                    Retry Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientRetryCountGet (L7_uint32 intIfNum,
                         L7_uint32 *retryCount)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, L7_MGMT_IPPORT))
                                            == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on intIfnum-%d",
                 intIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  *retryCount = clientIntfInfo->retryCount;

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "retryCount - %d", *retryCount);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Default Gateway associated with the given
*           Management port and interface
*
* @param    rtrIfNum     @b((input)) Router Interface Number
* @param    mgmtPortType @b((input)) Mgmt Interface Type
* @param    defGwAddr    @b((output)) Default Gateway Address
*
* @returns  L7_SUCCESS, if the Default Gateway Address is found
* @returns  L7_FAILURE, if the Default Gateway Address is not found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientRtrIntfDefaultGatewayGet (L7_uint32 rtrIfNum,
                                    L7_MGMT_PORT_TYPE_t mgmtPortType,
                                    L7_uint32 *defGwAddr)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_bootp_dhcp_t dhcpParams;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoByRtrIfNumGet (rtrIfNum, mgmtPortType))
                                                      == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for rtrIfNum-%d",
                 rtrIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if ((clientIntfInfo->inUse != L7_TRUE) ||
      (clientIntfInfo->dhcpState < DHCPSTATE_BOUND))
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client is not Active on rtrIfNum-%d",
                 rtrIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                               &dhcpParams)
                            != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP Params Get Failed on rtrIfNum-%d",
                 rtrIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }
  if ((*defGwAddr = dhcpParams.gateway) == 0)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "defGwAddr is Zero on rtrIfNum-%d", rtrIfNum);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "defGwAddr - 0x%x", *defGwAddr);
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

