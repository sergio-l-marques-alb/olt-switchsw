/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  usmdb_dhcp6c_api.h
*
* @purpose   Provide interface to API's for DHCPv6 Client UI component
*
* @component DHCPv6 Client
*
* @comments
*
* @create    08/29/2008
*
* @author    Kiran Kumar Kella
*
* @end
*
**********************************************************************/

#include "dhcp6c_api.h"
#include "ipv6_commdefs.h"

/*********************************************************************
* @purpose  Get the DHCPv6 Client DUID on service port or network port
*
* @param    *pDuidStr   @b{(output)} string representation of DUID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  We use the same Client DUID for clients running on
*            service port and network port
*
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cDuidGet(L7_char8 *pDuidStr);

/*********************************************************************
* @purpose  Get the number of Solicit transmitted statistics 
*
* @param    portType      @b{(input)} service port or network port
* @param    pTransmitted  @b{(output)} number transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cSolicitTransmittedGet(L7_MGMT_PORT_TYPE_t portType,
                                         L7_uint32  *pTransmitted);

/*********************************************************************
* @purpose  Get the number of Request transmitted statistics 
*
* @param    portType      @b{(input)} service port or network port
* @param    pTransmitted  @b{(output)} number transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cRequestTransmittedGet(L7_MGMT_PORT_TYPE_t portType,
                                         L7_uint32  *pTransmitted);

/*********************************************************************
* @purpose  Get the number of Renew transmitted statistics 
*
* @param    portType      @b{(input)} service port or network port
* @param    pTransmitted  @b{(output)} number transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cRenewTransmittedGet(L7_MGMT_PORT_TYPE_t portType,
                                       L7_uint32  *pTransmitted);

/*********************************************************************
* @purpose  Get the number of Rebind transmitted statistics 
*
* @param    portType      @b{(input)} service port or network port
* @param    pTransmitted  @b{(output)} number transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cRebindTransmittedGet(L7_MGMT_PORT_TYPE_t portType,
                                        L7_uint32  *pTransmitted);

/*********************************************************************
* @purpose  Get the number of Release transmitted statistics 
*
* @param    portType      @b{(input)} service port or network port
* @param    pTransmitted  @b{(output)} number transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cReleaseTransmittedGet(L7_MGMT_PORT_TYPE_t portType,
                                         L7_uint32  *pTransmitted);

/*********************************************************************
* @purpose  Get the number of Advertisement received statistics 
*
* @param    portType      @b{(input)} service port or network port
* @param    pReceived   @b{(output)} number received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cAdvertiseReceivedGet(L7_MGMT_PORT_TYPE_t portType,
                                        L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Reply received statistics 
*
* @param    portType      @b{(input)} service port or network port
* @param    pReceived   @b{(output)} number received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cReplyReceivedGet(L7_MGMT_PORT_TYPE_t portType,
                                    L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Malformed packets received
*
* @param    portType      @b{(input)} service port or network port
* @param    pReceived   @b{(output)} number received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cMalformedReceivedGet(L7_MGMT_PORT_TYPE_t portType,
                                        L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Advertisement packets discarded
*
* @param    portType      @b{(input)} service port or network port
* @param    pDiscarded  @b{(output)} number discarded
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cAdvertiseDiscardedGet(L7_MGMT_PORT_TYPE_t portType,
                                         L7_uint32  *pDiscarded);

/*********************************************************************
* @purpose  Get the number of Reply packets discarded
*
* @param    portType      @b{(input)} service port or network port
* @param    pDiscarded  @b{(output)} number discarded
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cReplyDiscardedGet(L7_MGMT_PORT_TYPE_t portType,
                                     L7_uint32  *pDiscarded);

/*********************************************************************
* @purpose  Get the total number of DHCP6 packets received
*
* @param    portType      @b{(input)} service port or network port
* @param    pReceived   @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cTotalPacketsReceivedGet(L7_MGMT_PORT_TYPE_t portType,
                                           L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the total number of DHCP6 packets transmitted
*
* @param    portType      @b{(input)} service port or network port
* @param    pTransmitted  @b{(output)} number transmitted         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cTotalPacketsTransmittedGet(L7_MGMT_PORT_TYPE_t portType,
                                              L7_uint32  *pTransmitted);

/*********************************************************************
* @purpose  Clear the  statistics for a DHCPv6 Client          
*
* @param    portType      @b{(input)} service port or network port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Resets packet counters only     
*      
* @end
*********************************************************************/
L7_RC_t usmDbDhcp6cStatisticsClear(L7_MGMT_PORT_TYPE_t portType);

/*********************************************************************
* @purpose  Checks if the given name server is learnt from DHCPv6.
*
* @param    nameServer   @b((inout)) DHCPv6 DNS Server Option
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This is used by DNS show commands and running config commands
*           to differentiate between servers configured by user and servers
*           learnt from the DHCPv6.
*
* @end
*********************************************************************/
L7_BOOL usmDbDhcp6cDNSOptionIsPresent(L7_inet_addr_t *nameServer);

/*********************************************************************
* @purpose  Checks if the given domain name is learnt from DHCPv6
*
* @param    domain   @b((inout)) Domain Name
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This is used by DNS show commands and running config commands
*           to differentiate between domain names configured by user
*           and servers learnt from the DHCPv6.
*
* @end
*********************************************************************/
L7_BOOL usmDbDhcp6cDomainNameOptionIsPresent(L7_char8 *domain);

/*********************************************************************
* @purpose  Get the DHCPv6 Client Mode on an interface
*
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    clientMode @b{(output)} Mode of the DHCPv6 Client
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientModeGet (L7_uint32 intIfNum,
                         L7_DHCP6_MODE_t *clientMode);

/*********************************************************************
* @purpose  Get the DHCPv6 Client State on an interface
*
* @param    intIfNum  @b{(input)}  Internal Interface Number
* @param    state     @b{(output)} State of the DHCPv6 Client
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientStateGet (L7_uint32 intIfNum,
                          L7_DHCP6_CLIENT_STATE_t *state);

/*********************************************************************
* @purpose  Get the DHCPv6 Client Renewal Time on an interface
*
* @param    intIfNum  @b{(input)}  Internal Interface Number
* @param    renewTime @b{(output)} Renewal Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientRenewTimeGet (L7_uint32 intIfNum,
                              L7_uint32 *renewTime);


/*********************************************************************
* @purpose  Get the DHCPv6 Server DUID on an interface
*
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    serverDUID @b{(output)} DHCPv6 Server DUID- printable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientDhcpServerDUIDGet (L7_uint32 intIfNum,
                                   L7_uchar8 *serverDUID);


/*********************************************************************
* @purpose  Get the DHCPv6 Prefix Type on an interface
*
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    serverAddr @b{(input)}  DHCPv6 Server Address
* @param    prefixType @b{(output)} DHCPv6 Server Prefix Type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientPrefixTypeGet (L7_uint32 intIfNum,
                               L7_in6_addr_t *serverAddr,
                               L7_DHCP6_PREFIX_TYPE_t *prefixType);

/*********************************************************************
* @purpose  Get the DHCPv6 Server IAID on an interface
*
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    serverIAID @b{(output)} DHCPv6 Server DUID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientDhcpServerIAIDGet (L7_uint32 intIfNum,
                                   L7_uint32 *serverIAID);

/*********************************************************************
* @purpose  Get the DHCPv6 Server T1 Time on an interface
*
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    serverT1Time @b{(output)} DHCPv6 Server T1 Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientDhcpServerT1T2TimeGet (L7_uint32 intIfNum,
                                     L7_uint32 *serverT1Time,
                                     L7_uint32 *serverT2Time);

/*********************************************************************
* @purpose  Get the DHCPv6 Client Address on an interface
*
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    clientAddr @b{(output)} DHCPv6 Client Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientAddressGet (L7_uint32 intIfNum,
                            L7_in6_addr_t *clientAddr);

/*********************************************************************
* @purpose  Get the DHCPv6 Client Prefix Length on an interface
*
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    clientPrefixLen @b{(output)} DHCPv6 Client Prefix Length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientPrefixLengthGet (L7_uint32 intIfNum,
                                 L7_uint32 *clientPrefixLen);

/*********************************************************************
* @purpose  Get the DHCPv6 Client Preferred Lifetime on an interface
*
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    prefLifeTime @b{(output)} DHCPv6 Client Preferred Lifetime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientPreferredLifeTimeGet (L7_uint32 intIfNum,
                                      L7_uint32 *prefLifeTime);

/*********************************************************************
* @purpose  Get the DHCPv6 Client Valid Lifetime on an interface
*
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    validLifeTime @b{(output)} DHCPv6 Client Valid Lifetime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientValidLifeTimeGet (L7_uint32 intIfNum,
                                  L7_uint32 *validLifeTime);

/*********************************************************************
* @purpose  Get the DHCPv6 Client Expiry Time on an interface
*
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    expiryTime @b{(output)} DHCPv6 Client Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientExpiryTimeGet (L7_uint32 intIfNum,
                               L7_uint32 *expiryTime);

/*********************************************************************
* @purpose  Set the debug trace flag
*
* @param    mode   @b{(input)}  L7_TRUE/L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientDebugTraceFlagSet (L7_BOOL  mode);

/*********************************************************************
* @purpose  Get the debug trace flag
*
* @param    mode   @b{(input)}  L7_TRUE/L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  None
*
* @end
*********************************************************************/
L7_RC_t
usmDbDhcp6ClientDebugTraceFlagGet (L7_BOOL  *mode);

