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

#ifndef _USMDB_DHCP_CLIENT_H_
#define _USMDB_DHCP_CLIENT_H_

#include "l7_common.h"
#include "l3_addrdefs.h"
#include "dhcp_client_api.h"

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
usmdbDhcpVendorClassOptionAdminModeSet (L7_uint32 adminMode,
                                        L7_BOOL actImmediate);

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
usmdbDhcpVendorClassOptionStringSet (L7_uchar8 *optionString,
                                     L7_BOOL actImmediate);

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
usmDbIpDhcpClientDebugTraceFlagSet (L7_BOOL rxFlag,
                                    L7_BOOL txFlag);

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
usmDbIpDhcpClientDebugTraceFlagGet (L7_BOOL *rxFlag,
                                    L7_BOOL *txFlag);

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
usmDbIpDhcpClientDhcpServerIPAddressGet (L7_uint32 intIfNum,
                                         L7_IP_ADDR_t *serverAddr);

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
usmDbIpDhcpClientIPAddressGet (L7_uint32 intIfNum,
                               L7_IP_ADDR_t *clientAddr);

/*********************************************************************
* @purpose  Renews the DHCP Client Network Mask on an interface
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
usmDbIpDhcpClientNetworkMaskGet (L7_uint32 intIfNum,
                                 L7_IP_MASK_t *clientMask);

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
usmDbIpDhcpClientStateGet (L7_uint32 intIfNum,
                           L7_DHCP_CLIENT_STATE_t *clientState);

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
usmDbIpDhcpClientTransactionIdGet (L7_uint32 intIfNum,
                                   L7_uint32 *transId);

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
usmDbIpDhcpClientLeaseTimeInfoGet (L7_uint32 intIfNum,
                                   L7_uint32 *leaseTime,
                                   L7_uint32 *renewalTime,
                                   L7_uint32 *rebindTime);

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
usmDbIpDhcpClientRetryCountGet (L7_uint32 intIfNum,
                                L7_uint32 *retryCount);

/*********************************************************************
* @purpose  Acquire an IP Address on the interface from DHCP Server
*
* @param    intIfNum   @b((input)) Internal Interface Number
*
* @returns  L7_SUCCESS, if the Acquire process is successful
* @returns  L7_FAILURE, if the Acquire process failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbIpDhcpClientConfiguredIPAddressSet (L7_MGMT_PORT_TYPE_t mgmtPortType,
                                         L7_uint32 ipAddr,
                                         L7_uint32 netMask,
                                         L7_uint32 ipGateway);
#endif /* _USMDB_DHCP_CLIENT_H_ */

