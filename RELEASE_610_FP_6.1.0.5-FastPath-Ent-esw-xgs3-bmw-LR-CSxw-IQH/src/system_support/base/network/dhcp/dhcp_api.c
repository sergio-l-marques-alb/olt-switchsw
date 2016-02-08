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

/*********************************************************************
                         Globals
**********************************************************************/
extern struct bootp l7_eth0_bootp_data;
extern struct bootp l7_eth1_bootp_data;
extern dhcpOptions_t dhcpOptions;
extern dhcpNotifyList_t notifyList[DHCP_CLIENT_NOTIFY_LIST_MAX];

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
  if (optionAdminMode == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *optionAdminMode = dhcpOptions.vendorClassOption.optionMode;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the DHCP Vendor Class Option status
*
* @param    optionAdminMode   @b((input)) DHCP Vendor Class Option admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpVendorClassOptionAdminModeSet(L7_uint32  optionAdminMode )
{
  dhcpOptions.vendorClassOption.optionMode = optionAdminMode;
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
  if (optionString == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiStrncpy(optionString,
               (L7_uchar8 *)(dhcpOptions.vendorClassOption.vendorClassString),
               DHCP_VENDOR_CLASS_STRING_MAX);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the DHCP Vendor Class Option string
*
* @param    optionString   @b((input)) DHCP Vendor Class Option string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpVendorClassOptionStringSet(L7_uchar8  *optionString)
{
  memset(dhcpOptions.vendorClassOption.vendorClassString,0x00,sizeof(dhcpOptions.vendorClassOption.vendorClassString));
  osapiStrncpy(dhcpOptions.vendorClassOption.vendorClassString,
               optionString, DHCP_VENDOR_CLASS_STRING_MAX);
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
  osapiStrncpy((L7_uchar8 *)option, (L7_uchar8 *)(&dhcpOptions.vendorSpecificOption), 
              sizeof (dhcpVendorSpecificOption_t));
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
  L7_uint32 spMode, npMode, dnsServerIpAddr = 0;
  L7_inet_addr_t inetAddr;


  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &spMode);
  if (spMode == L7_SYSCONFIG_MODE_DHCP)
  {
    if (l7_get_bootp_option(&l7_eth0_bootp_data, TAG_DOMAIN_SERVER, &dnsServerIpAddr )!= L7_TRUE)
      dnsServerIpAddr = 0;
  }
  else
  {
    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &npMode);
    if (npMode == L7_SYSCONFIG_MODE_DHCP)
    {
      if (l7_get_bootp_option(&l7_eth1_bootp_data, TAG_DOMAIN_SERVER, &dnsServerIpAddr )!= L7_TRUE)
        dnsServerIpAddr = 0;
    }
  }

  inetAddressSet(L7_AF_INET, &dnsServerIpAddr, &inetAddr);
  if (L7_INET_ADDR_COMPARE(option, &inetAddr) == L7_NULL)
  {
    return L7_TRUE;
  }
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

  memset(domainName, 0, sizeof(domainName));
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &spMode);
  if (spMode == L7_SYSCONFIG_MODE_DHCP)
  {
    (void)l7_get_bootp_option(&l7_eth0_bootp_data, TAG_DOMAIN_NAME, domainName);
  }
  else
  {
    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &npMode);
    if (npMode == L7_SYSCONFIG_MODE_DHCP)
    {
      (void)l7_get_bootp_option(&l7_eth1_bootp_data, TAG_DOMAIN_NAME, domainName);
    }
  }

  if (osapiStrncmp(domain, domainName, sizeof(domainName)) == 0)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}
