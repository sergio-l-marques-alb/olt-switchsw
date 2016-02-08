/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2005-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
* @filename  usmdb_dhcp_client.c
*
* @purpose   DHCP client configuration APIs
*
* @component DHCP client
*
* @comments 
*
* @create 4/11/2008
*
* @author ddevi
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "l3_addrdefs.h"

/*********************************************************************
* @purpose  Gets the DHCP Vendor Option Mode
*
* @param    adminMode   @b((output)) DHCP vendor option admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDhcpVendorClassOptionAdminModeGet(L7_uint32 *adminMode);

/*********************************************************************
* @purpose  Sets the DHCP Vendor Option Mode
*
* @param    adminMode   @b((output)) DHCP vendor option admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbDhcpVendorClassOptionAdminModeSet(L7_uint32 adminMode);
/******************************************************************
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
L7_RC_t usmdbDhcpVendorClassOptionStringGet(L7_uchar8 * optionString);
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
L7_RC_t usmdbDhcpVendorClassOptionStringSet(L7_uchar8  *optionString);

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
L7_BOOL usmdbDhcpDNSOptionIsPresent(L7_inet_addr_t *option);

/*********************************************************************
* @purpose  Checks if the given domain name is learnt from DHCP.
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
L7_BOOL usmDbDhcpDomainNameOptionIsPresent(L7_char8 *domain);

