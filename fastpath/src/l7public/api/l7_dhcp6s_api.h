/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename   l7_dhcp6s_api.h
*
* @purpose    DHCPv6 Server Mapping Layer APIs
*
* @component  DHCPv6 Server Mapping Layer
*
* @comments   none
*
* @create     04/05/05
*
* @author     dinderieden
* @end
*
**********************************************************************/

#ifndef _DHCP6S_MAP_API_H_
#define _DHCP6S_MAP_API_H_

#include "ipv6_commdefs.h"

/* Begin Function Prototypes */
/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*********************************************************************
* @purpose  Get DHCPv6 Server admin mode
*
* @param    mode        @b{(output)} admin mode
*
* @returns  L7_SUCCESS  admin mode Get
* @returns  L7_FAILURE  Failed to Get admin mode
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapAdminModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set DHCPv6 Server admin mode
*
* @param    mode        @b{(input)} admin mode
*

* @returns  L7_FAILURE  Failed to set admin mode
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapAdminModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Get the DHCPv6 Server DUID
 *
 * @param    *pDuidStr   @b{(output)} string representation of DUID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapServerDuidGet(L7_char8 *pDuidStr);

/*********************************************************************
* @purpose  Get the DHCPv6 Relay Agent Information Option type value.
*           Currently, this DHCPv6 option is not assigned by IANA,
*           so this is a configured value.
*
* @param    pType   @b{(input)} option type value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRelayOptTypeGet(L7_uint32 *pType);

/*********************************************************************
* @purpose  Set the DHCPv6 Relay Agent Information Option type value.
*           Currently, this DHCPv6 option is not assigned by IANA,
*           so this routine allows for a configured value.  Otherwise
*           the default value FD_DHCP6S_DEFAULT_RELAYOPT_TYPE is used.
*
* @param    type   @b{(input)} option type value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRelayOptTypeSet(L7_uint32 type);

/*********************************************************************
* @purpose  Get the DHCPv6 Relay Agent Information option type
*           value for "remote-id" sub-option.
*           Currently, this DHCPv6 option is not assigned by IANA,
*           so this is a configured value.
*
* @param    pType   @b{(input)} option type value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRelayOptRemoteIdTypeGet(L7_uint32 *pType);

/*********************************************************************
* @purpose  Set the DHCPv6 Relay Agent Information option type
*           value for "remote-id" sub-option.
*           Currently, this DHCPv6 option is not assigned by IANA,
*           so this routine allows for a configured value.  Otherwise
*           the default value FD_DHCP6S_DEFAULT_RELAYOPT_REMOTEID_TYPE
*           is used.
*
* @param    type   @b{(input)} option type value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRelayOptRemoteIdTypeSet(L7_uint32 type);

/*********************************************************************
* @purpose  Get the number of Solicit Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapSolicitReceivedGet(L7_uint32 intIfNum,
                                    L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Request Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRequestReceivedGet(L7_uint32 intIfNum,
                                    L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Confirm Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapConfirmReceivedGet(L7_uint32 intIfNum,
                                    L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Renew Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRenewReceivedGet(L7_uint32 intIfNum,
                                  L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Rebind Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRebindReceivedGet(L7_uint32 intIfNum,
                                   L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Release Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapReleaseReceivedGet(L7_uint32 intIfNum,
                                    L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Decline Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapDeclineReceivedGet(L7_uint32 intIfNum,
                                    L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Inform Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapInformReceivedGet(L7_uint32 intIfNum,
                                   L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Relay-Forward Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRelayForwReceivedGet(L7_uint32 intIfNum,
                                      L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Relay-Reply Received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRelayReplyReceivedGet(L7_uint32 intIfNum,
                                      L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Advertise Sent statistics 
*
* @param    pSent @b{(output)} number sent         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapAdvertiseSentGet(L7_uint32 intIfNum,
                                  L7_uint32  *pSent);

/*********************************************************************
* @purpose  Get the number of Reply Sent statistics 
*
* @param    pSent @b{(output)} number sent         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapReplySentGet(L7_uint32 intIfNum,
                              L7_uint32  *pSent);

/*********************************************************************
* @purpose  Get the number of Reconfigure Sent statistics 
*
* @param    pSent @b{(output)} number sent         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapReconfigureSentGet(L7_uint32 intIfNum,
                                    L7_uint32  *pSent);

/*********************************************************************
* @purpose  Get the number of Relay-Reply Sent statistics 
*
* @param    pSent @b{(output)} number sent         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRelayReplySentGet(L7_uint32 intIfNum,
                                   L7_uint32  *pSent);

/*********************************************************************
* @purpose  Get the number of Relay-Forward Sent statistics 
*
* @param    pSent @b{(output)} number sent         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapRelayForwSentGet(L7_uint32 intIfNum,
                                  L7_uint32  *pSent);

/*********************************************************************
* @purpose  Get the Total number of DHCP6 packets sent
*
* @param    pSent @b{(output)} number sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapTotalPacketSentGet(L7_uint32 intIfNum,
                L7_uint32  *pSent);

/*********************************************************************
* @purpose  Get the number of Malformed received statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapMalformedReceivedGet(L7_uint32 intIfNum,
                                      L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the number of Discarded packets statistics 
*
* @param    pReceived @b{(output)} number received         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapDiscardedPacketsGet(L7_uint32 intIfNum,
                                     L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Get the Total number of DHCP6 packets received
*
* @param    pReceived @b{(output)} number received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapTotalPacketReceivedGet(L7_uint32 intIfNum,
                L7_uint32  *pReceived);

/*********************************************************************
* @purpose  Clear the  statistics for a DHCPv6 Server          
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Resets packet counters only     
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapStatisticsClear(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Create a DHCPv6 address pool
*
* @param    pPoolName    @b{(input)} Name of address pool
*
* @returns  L7_SUCCESS  Dhcp address pool created
* @returns  L7_FAILURE  Failed to create Dhcp address pool
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolCreate(L7_char8 *pPoolName);

/*********************************************************************
* @purpose  Delete a DHCPv6 address pool
*
* @param    pPoolName    @b{(input)} Name of address pool
*
* @returns  L7_SUCCESS  Dhcp address pool deleted
* @returns  L7_FAILURE  Failed to delete Dhcp address pool
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolDelete(L7_char8 *pPoolName);

/*************************************************************
* @purpose  Get the first entry in DHCPv6 address pool
*
* @param    pPoolIndex   @b{(input)} Unit for this operation
* @param    pPoolName     @b{(output)} Name of address pool
*
* @returns  L7_SUCCESS  Dhcp address pool first entry found
* @returns  L7_FAILURE  Dhcp address pool first entry not found
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolEntryFirst(L7_char8 *pPoolName, L7_uint32 *pPoolIndex );

/******************************************************************
* @purpose  Get the next entry in DHCPv6 address pool
*
* @param    pPoolIndex   @b{(input)} Unit for this operation
* @param    pPoolName     @b{(output)} Name of address pool
*
* @returns  L7_SUCCESS  Dhcp address pool first entry found
* @returns  L7_FAILURE  Dhcp address pool first entry not found
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolEntryNext(L7_char8 *pPoolName, L7_uint32 *pPoolIndex );

/*********************************************************************
* @purpose  Add a DHCPv6 host DNS Servers for a automatic pool
*
* @param    pPoolName    @b{(input)} Name of address pool
* @param    addr         @b{(input)} An IPv6 address of a DNS Servers
*
* @returns  L7_SUCCESS  DNS servers set
* @returns  L7_FAILURE  Failed to set DNS Servers
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolDnsServerAdd(L7_char8 *pPoolName, L7_in6_addr_t *pAddr);

/*********************************************************************
* @purpose  Delete a DHCPv6 host DNS Servers for a automatic pool
*
* @param    pPoolName    @b{(input)} Name of address pool
* @param    addr         @b{(input)} An IPv6 address of a DNS Servers
*
* @returns  L7_SUCCESS  DNS servers deleted
* @returns  L7_FAILURE  Failed to delete DNS Servers
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolDnsServerRemove(L7_char8 *pPoolName, L7_in6_addr_t *pAddr);

/*********************************************************************
* @purpose  Get DHCPv6 host DNS Servers  for a automatic pool
*
* @param    pPoolName    @b{(input)} Name of address pool
* @param    DnsServers  @b{(output)} Array of ipAddress of DNS Servers
*
* @returns  L7_SUCCESS  DNS servers Got
* @returns  L7_FAILURE  Failed to get DNS Servers
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolDnsServersGet(L7_char8 *pPoolName, L7_in6_addr_t  DnsServers[L7_DHCP6S_DNS_SERVER_MAX]);

/*********************************************************************
* @purpose  Add a DHCPv6 DNS Domain Name for a pool          
*
* @param    *pPoolName   @b{(input)} pool name
* @param    *pDomainName @b{(input)} DNS domain name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolDnsDomainAdd(L7_char8 *pPoolName, L7_char8 *pDomainName);

/*********************************************************************
* @purpose  Remove a DHCPv6 DNS Domain Name from a pool
*
* @param    *pPoolName   @b{(input)} pool name
* @param    *pDomainName @b{(input)} DNS domain name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolDnsDomainRemove(L7_char8 *pPoolName, L7_char8 *pDomainName);

/*********************************************************************
* @purpose  Get the DHCPv6 DNS Domain Names from a pool          
*
* @param    *pPoolName   @b{(input)} pool name
* @param    DnsDomains[] @b{(output)} array of DNS domain names
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolDnsDomainsGet(L7_char8 *pPoolName, 
                          L7_char8 DnsDomains[L7_DHCP6S_DNS_DOMAIN_MAX][L7_DHCP6S_DOMAIN_NAME_MAXLEN]);

/*********************************************************************
 * @purpose  Add a host within a DHCPv6 Server pool
 *
 * @param    *pPoolName     @b{(input)} pool name
 * @param    *pHostName     @b{(input)} host name
 * @param    *pDuidStr      @b{(input)} string representing host DUID
 * @param    iaid           @b{(input)} Identity Association ID
 * @param    *prefixAddr    @b{(input)} delegated prefix address
 * @param    prefixLen      @b{(input)} delegated prefix mask length
 * @param    validLifetime  @b{(input)} delegated prefix valid lifetime
 * @param    preferLifetime @b{(input)} delegated prefix prefer lifetime
 *
 * @returns  L7_SUCCESS          New host created
 * @returns  L7_TABLE_IS_FULL    No space left for a new host in pool
 * @returns  L7_FAILURE          Function failed
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapPoolHostAdd(L7_char8 *pPoolName, L7_char8 *pHostName,
                             L7_char8 *pDuidStr, L7_uint32 iaid,
                             L7_in6_addr_t *prefixAddr, L7_ushort16 prefixLen,
                             L7_uint32 validLifetime, L7_uint32 preferLifetime);

/*********************************************************************
 * @purpose  Delete the host within a DHCPv6 Server pool
 *
 * @param    *pPoolName   @b{(input)} pool name
 * @param    *pDuidStr    @b{(input)} string representing host DUID
 * @param    iaid         @b{(input)} Identity Association ID
 * @param    *prefixAddr  @b{(input)} delegated prefix address
 * @param    prefixLen    @b{(input)} delegated prefix mask length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapPoolHostRemove(L7_char8 *pPoolName,
                             L7_char8 *pDuidStr, L7_uint32 iaid,
                             L7_in6_addr_t *prefixAddr, L7_ushort16 prefixLen);

/*********************************************************************
 * @purpose  Set DHCPv6 Server parameters for an interface
 *
 * @param    intIfNum     @b{(input)} Interface number
 * @param    pPoolName    @b{(input)} DHCPv6 pool name
 * @param    serverPref   @b{(input)} Server preference
 * @param    optionFlags  @b{(input)} options flags
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapIntfModeServerSet(L7_uint32 intIfNum, L7_char8 *pPoolName,
                               L7_uint32 serverPref, L7_uint32 optionFlags);

/*********************************************************************
 * @purpose  Set DHCPv6 Relay parameters for an interface
 *
 * @param    intIfNum     @b{(input)} Interface number
 * @param    serverAddr   @b{(input)} Relay Server address
 * @param    serverIfNum  @b{(input)} Relay Server interface
 * @param    remoteId     @b{(input)} "remote-id" Relay option
 * @param    optionFlags  @b{(input)} options flags
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapIntfModeRelaySet(L7_uint32 intIfNum,
                                  L7_in6_addr_t *serverAddr, L7_uint32 serverIfNum,
                                  L7_char8 * remoteId, L7_uint32 optionFlags);

/*********************************************************************
 * @purpose  Delete DHCPv6 Server/Relay parameters on an interface
 *
 * @param    intIfNum     @b{(input)} Interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapIntfModeDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get DHCPv6 Interface parameters for an interface
*
* @param    intIfNum     @b{(input)} Interface number
* @param    mode         @b{(output)} DHCP6 mode on interface
* @param    pPoolName    @b{(output)} DHCPv6 pool name
* @param    serverPref   @b{(output)} Server preference
* @param    relayAddr    @b{(output)} Relay Server address
* @param    relayIfNum   @b{(output)} Relay Server interface
* @param    remoteId     @b{(output)} "remote-id" Relay option 
* @param    optionFlags  @b{(output)} options flags
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE 
*
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapIntfModeGet(L7_uint32 intIfNum,
               L7_DHCP6_MODE_t *mode, L7_char8 *pPoolName,
               L7_uint32 *serverPref, L7_in6_addr_t *relayAddr,
               L7_uint32 *relayIfNum, L7_char8 * remoteId, 
               L7_uint32 *optionFlags);

/*:ignore*/
/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  EXTRA(FOR SNMP)
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Check if a DHCPv6 pool corresponding to number is valid or not
*
* @param    poolNumber   @b{(input)} pool number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP WALK operation
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolValidate( L7_uint32 poolNumber);

/*********************************************************************
* @purpose  Get the DHCPv6 Server pool type
*
* @param    *pPoolName   @b{(input)} pool name
* @param    *pPoolType   @b{(output)} pool type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolTypeGet(L7_char8 *pPoolName, L7_uint32 *pPoolType);

/*********************************************************************
* @purpose  Get the next valid DHCPv6 pool number
*
* @param    poolNumber       @b{(in/out)} pool number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  Set poolNumber to 0 to retrieve first entry.
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolGetNext(L7_uint32 *poolNumber);

/*********************************************************************
* @purpose  Get the pool name corresponding to a valid pool number
*
* @param    poolNumber       @b{(input)} pool Number
* @param    pPoolName        @b{(output)} pool Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP GET / SET operation, where it is required
* @notes  to convert pool number to pool name for calling the USMDB apis
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapNumberToPoolNameGet(L7_uint32 poolNumber, L7_uchar8* pPoolName);

/*********************************************************************
* @purpose  Get the DHCPv6 Server pool name by index
*
* @param    poolIndex    @b{(input)} pool Index
* @param    *pPoolName   @b{(output)} pool name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolNameByIndexGet(L7_uint32 poolindex, L7_char8 *pPoolName );

/*********************************************************************
* @purpose  Get the number of pool for a DHCPv6 Server
*
* @param    pNoOfPool @b{(output)} number of pool
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolNumGet( L7_uint32  *pNoOfPool);

/*********************************************************************
* @purpose  Get next DHCPv6 DNS Server addresses for a pool          
*
* @param    *pPoolName   @b{(input)} pool name
* @param    *pAddr       @b{(in/out)} DNS server IPv6 address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  Set pAddr to unspecified address to retrieve first entry.
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolDnsServerGetNext(L7_char8 *pPoolName, L7_in6_addr_t *pAddr);

/*********************************************************************
 * @purpose  Get the next DHCPv6 DNS Domain Name for a pool
 *
 * @param    *pPoolName       @b{(input)} pool name
 * @param    *pDomainName     @b{(in/out)} DNS domain name
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  Set pDomainName to empty string to retrieve first entry.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapPoolDnsDomainGetNext(L7_char8 *pPoolName, L7_char8 *pDomainName);

/*********************************************************************
* @purpose  Get next host within a DHCPv6 Server pool 
*
* @param    *pPoolName      @b{(input)} pool name
* @param    *pDuidStr       @b{(in/out)} string representing host DUID
* @param    *pHostName      @b{(in/out)} host name
* @param    *iaid           @b{(in/out)} Identity Association ID
* @param    *prefixAddr     @b{(in/out)} delegated prefix address
* @param    *prefixLen      @b{(in/out)} delegated prefix mask length
* @param    *validLifetime  @b{(in/out)} delegated prefix valid lifetime
* @param    *preferLifetime @b{(in/out)} delegated prefix prefer lifetime
*
* @returns  L7_SUCCESS          Host found
* @returns  L7_FAILURE          Function failed
*
* @comments  Set pDuidStr to empty string to retrieve first entry.
*      
* @end
*********************************************************************/
L7_RC_t dhcp6sMapPoolHostGetNext(L7_char8 *pPoolName, L7_char8 *pDuidStr,
                             L7_char8 *pHostName, L7_uint32 *iaid,
                             L7_in6_addr_t *prefixAddr, L7_ushort16 *prefixLen,
                             L7_uint32 *validLifetime, L7_uint32 *preferLifetime);

/*********************************************************************
 * @purpose  Get the next interface with active DHCPv6 configuration
 *
 * @param    pIntfNumber      @b{(in/out)} interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  Set *pIntfNumber to 0 to retrieve first entry.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapIntfGetNext(L7_uint32 *pIntfNumber);

/*********************************************************************
 * @purpose  Validate the interface has active DHCPv6 configuration
 *
 * @param    intfNumber       @b{(in/out)} interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapIntfValidate(L7_uint32 intfNumber);

/*********************************************************************
 * @purpose  Get next active binding from the DHCPv6 server.
 *
 * @param    *pDuidStr       @b{(in/out)} string representing client DUID
 * @param    *iaid           @b{(in/out)} Identity Association ID
 * @param    *prefixAddr     @b{(in/out)} binding prefix address
 * @param    *prefixLen      @b{(in/out)} binding prefix mask length
 * @param    *prefixType     @b{(in/out)} binding prefix type
 * @param    *clientAddr     @b{(output)} client address
 * @param    *clientIntfNum  @b{(output)} client interface number
 * @param    *expiration     @b{(output)} binding expiration (seconds)
 * @param    *validLifetime  @b{(output)} binding prefix valid lifetime
 * @param    *preferLifetime @b{(output)} binding prefix prefer lifetime
 *
 * @returns  L7_SUCCESS          Host found
 * @returns  L7_FAILURE          Function failed
 *
 * @comments   Set pDuidStr to empty string to retrieve first entry.
 *
 * @end
 *********************************************************************/
L7_RC_t dhcp6sMapBindingGetNext(L7_char8 *pDuidStr, L7_uint32 *iaid,
                                L7_in6_addr_t *prefixAddr, L7_ushort16 *prefixLen,
                                L7_uint32 *prefixType, L7_in6_addr_t *clientAddr,
                                L7_uint32 *clientIntfNum, L7_uint32 *expiration,
                                L7_uint32 *validLifetime, L7_uint32 *preferLifetime);


/*********************************************************************
* @purpose  Determine if the interface type is valid in DHCPv6
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL dhcp6sMapIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in DHCPv6
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL dhcp6sMapIsValidIntf(L7_uint32 intIfNum);

#endif /* _DHCP6S_MAP_API_H_*/

