
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dhcp_client_api.h
*
* @purpose dhcp client configuration information file
*
* @component dhcp client (src/system_support/base/network/dhcp)
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
#include "l7_cnfgr_api.h"
#include "l3_commdefs.h"


#define DHCP_VENDOR_CLASS_STRING_MAX  (128)  /* Max length of the vendor class string */

/* DHCP Interfaces Numbers */
#define DHCP_CLIENT_MAX_RTR_INTERFACES    (L7_RTR_MAX_RTR_INTERFACES)
#define DHCP_CLIENT_NETWORK_PORT_INTF_NUM (DHCP_CLIENT_MAX_RTR_INTERFACES + 1)
#define DHCP_CLIENT_SERVICE_PORT_INTF_NUM (DHCP_CLIENT_NETWORK_PORT_INTF_NUM + 1)
#define DHCP_CLIENT_MAX_INTERFACES        (DHCP_CLIENT_SERVICE_PORT_INTF_NUM)

/* DHCP Client State Machine States */
typedef enum
{
  DHCPSTATE_INIT = 1,
  DHCPSTATE_SELECTING,
  DHCPSTATE_REQUESTING,
  DHCPSTATE_REQUEST_RECV,
  DHCPSTATE_BOUND,
  DHCPSTATE_RENEWING,
  DHCPSTATE_RENEW_RECV,
  DHCPSTATE_REBINDING,
  DHCPSTATE_REBIND_RECV,
  DHCPSTATE_BOOTP_FALLBACK,
  DHCPSTATE_NOTBOUND,
  DHCPSTATE_FAILED,
  DHCPSTATE_DO_RELEASE,
  DHCPSTATE_INFORM_REQUEST,
  DHCPSTATE_INFORM_ACK_WAIT,
  DHCPSTATE_INFORM_FAILED,
  DHCPSTATE_INFORM_BOUND

} L7_DHCP_CLIENT_STATE_t;

/*********************************************************************
 * @purpose  Process configurator commands.
 *
 * @param    pCmdData  - @b{(input)}  command to be processed
 *
 * @returns  None
 *
 * @notes    This function completes synchronously. The return value
 *           is presented to the configurator by calling the cnfgrApiCallback().
 *           The following are the possible return codes:
 *           L7_SUCCESS - There were no errors. Response is available.
 *           L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    This function runs in the configurator's thread. 
 *
 * @end
 *********************************************************************/
void dhcpClientApiCnfgrCommand (L7_CNFGR_CMD_DATA_t *pCmdData);

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
L7_RC_t dhcpVendorClassOptionStringGet(L7_uchar8 * optionString );
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
                                L7_BOOL actImmediate);

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
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientIPAddressMethodSet (L7_uint32 intIfNum,
                              L7_INTF_IP_ADDR_METHOD_t method,
                              L7_MGMT_PORT_TYPE_t mgmtPortType,
                              L7_BOOL actImmediate);

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
*                       is not successful
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientConfiguredIPAddressSet (L7_MGMT_PORT_TYPE_t mgmtPortType,
                                  L7_uint32 ipAddr,
                                  L7_uint32 netMask,
                                  L7_uint32 ipGateway);

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
dhcpClientDebugTraceFlagGet (L7_BOOL *rxFlag,
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
dhcpClientDhcpServerIPAddressGet (L7_uint32 intIfNum,
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
dhcpClientIPAddressGet (L7_uint32 intIfNum,
                        L7_IP_ADDR_t *clientAddr);

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
dhcpClientStateGet (L7_uint32 intIfNum,
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
dhcpClientTransactionIdGet (L7_uint32 intIfNum,
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
dhcpClientLeaseTimeInfoGet (L7_uint32 intIfNum,
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
dhcpClientRetryCountGet (L7_uint32 intIfNum,
                         L7_uint32 *retryCount);

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
                                    L7_uint32 *defGwAddr);

#endif
