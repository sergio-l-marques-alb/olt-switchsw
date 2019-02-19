
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dhcp_client_api.c
*
* @purpose dhcp client configuration information file
*
* @component dhcp client (src/system_support/base/network)
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
#ifndef INCLUDE_DHCP_CLIENT_API_H
#define INCLUDE_DHCP_CLIENT_API_H
#include "l7_common.h"
#include "l3_addrdefs.h"

#define DHCP_VENDOR_CLASS_STRING_MAX  (128)  /* Max length of the vendor class string */

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
L7_RC_t dhcpVendorClassOptionAdminModeGet(L7_uint32 * optionAdminMode );
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
L7_RC_t dhcpVendorClassOptionAdminModeSet(L7_uint32  optionAdminMode );
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
L7_RC_t dhcpVendorClassOptionStringGet(L7_uchar8 * optionString );
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
L7_RC_t dhcpVendorClassOptionStringSet(L7_uchar8  *optionString);
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
L7_RC_t dhcpVendorSpecificOptionGet(dhcpVendorSpecificOption_t *option);

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
L7_BOOL dhcpDNSOptionIsPresent(L7_inet_addr_t *option);

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
L7_BOOL dhcpDomainNameOptionIsPresent(L7_char8 *domain);

#endif
