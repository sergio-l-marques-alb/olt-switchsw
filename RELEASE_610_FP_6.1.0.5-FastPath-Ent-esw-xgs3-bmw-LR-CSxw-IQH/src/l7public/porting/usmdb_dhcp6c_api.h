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

