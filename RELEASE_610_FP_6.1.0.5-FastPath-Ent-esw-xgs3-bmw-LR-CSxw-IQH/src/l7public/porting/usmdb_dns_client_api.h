/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename usmdb_dns_client_api.h
*
* @purpose Provide interface to configure DNS client
*
* @component DNS client
*
* @comments none
*
* @create 03/19/05
*
* @author dfowler
* @end
*
**********************************************************************/
#ifndef USMDB_DNS_CLIENT_API_H
#define USMDB_DNS_CLIENT_API_H

#include "l7_common.h"
#include "dns_client_api.h"
#include "l3_addrdefs.h"

/*********************************************************************
*
* @purpose Request a blocking DNS lookup for a hostname.
*
* @param L7_char8                *hostname     @b((input))  request hostname 
* @param dnsClientLookupStatus_t *status       @b{{output}} specific L7_FAILURE status
* @param L7_char8                *hostFQDN     @b{{output}} fully qualified hostname
* @param L7_uint32               *ip           @b{{output}} ip address
*
* @returns L7_SUCCESS, hostFQDN and ip are returned.
* @returns L7_FAILURE, specific failure is indicated in status hostFQDN and ip are not valid.
*
* @comments  hostFQDN buffer must be DNS_DOMAIN_NAME_SIZE_MAX
*                                 
* @end
*
*********************************************************************/
L7_RC_t usmDbDNSClientNameLookup(L7_char8                *hostname,
                                 dnsClientLookupStatus_t *status,
                                 L7_char8                *hostFQDN,
                                 L7_uint32               *ip);
/*********************************************************************
*
* @purpose Request a blocking DNS lookup for a hostname.
*
* @param L7_uchar8                family       @b((input))  family type 
*                                                           (L7_AF_INET/L7_AF_INET6)
* @param L7_char8                *hostname     @b((input))  request hostname 
* @param dnsClientLookupStatus_t *status       @b{{output}} specific L7_FAILURE status
* @param L7_char8                *hostFQDN     @b{{output}} fully qualified hostname
* @param L7_inet_addr_t          *reslovedAddr @b{(input)} ipv4 or ipv6 address passed
*                                                          to the application. 
*
* @returns L7_SUCCESS, hostFQDN and ip are returned.
* @returns L7_FAILURE, specific failure is indicated in status hostFQDN and ip are not valid.
*
* @comments  hostFQDN buffer must be DNS_DOMAIN_NAME_SIZE_MAX
*                                 
* @end
*
*********************************************************************/
L7_RC_t usmDbDNSClientInetNameLookup(L7_uchar8               family,
                                     L7_char8                *hostname,
                                     dnsClientLookupStatus_t *status,
                                     L7_char8                *hostFQDN,
                                     L7_inet_addr_t          *reslovedAddr);
 
/*********************************************************************
 *                DNS Client Configuration APIs                            
 *********************************************************************/

/*********************************************************************
* @purpose  Set the global admin mode for the DNS client
*
* @param    L7_BOOL  enable  @b{(input)} L7_TRUE to enable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientAdminModeSet(L7_BOOL enable);

/*********************************************************************
* @purpose  Get the global admin mode for the DNS client
*
* @param    L7_BOOL  *enable  @b{(output)} L7_TRUE for enabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientAdminModeGet(L7_BOOL *enable);

/*********************************************************************
* @purpose  Set the default domain for the DNS client
*
* @param    L7_char8  domain @b{(input)} default domain for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Note that domains > two labels will create a search list,
*           i.e domain xxx.yyy.zzz's search list -> xxx.yyy.zzz, yyy.zzz
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDefaultDomainSet(L7_char8 *domain);

/*********************************************************************
* @purpose  clear the default domain for the DNS client
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDefaultDomainClear();

/*********************************************************************
* @purpose  Get the default domain for the DNS client
*
* @param    L7_char8  *domain  @b{(output)} default domain for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDefaultDomainGet(L7_char8 *domain);

/*********************************************************************
* @purpose  Get domain search list entries for the DNS client
*
* @param    L7_char8  *domain  @b{(input/output)} domain entry for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The search list is based on the default domain as explained 
*            in the comments above.
*           empty domain string will return first entry.
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDomainSearchListEntryNextGet(L7_char8 *domain);

/*********************************************************************
* @purpose  Add a name server for the DNS client
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} v4 or v6 address 
*                                                   of name server
*
* @returns  L7_SUCCESS, server added or already exists
* @returns  L7_FAILURE, server list contains max entries
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientNameServerEntryAdd(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Remove a name server for the DNS client
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} v4 or v6 address 
*                                                   of name server
*
* @returns  L7_SUCCESS, server removed
* @returns  L7_FAILURE, server not found
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientNameServerEntryRemove(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Determine if a name server entry exists.
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} v4 or v6 address 
*                                                   of name server
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry does not exist
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientNameServerEntryGet(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Get name servers for the DNS client
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} v4 or v6 address 
*                                                   of name server
*
* @returns  L7_SUCCESS, ip contains next server address
* @returns  L7_FAILURE, end of list
*
* @comments ip of zero will return first server ip
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientNameServerEntryNextGet(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Get Next Name Server entry for the DNS client
*           in the way the ordered list is entered
*
* @param    L7_inet_addr_t   *serverAddr @b{(input/output)} ipv4 or
*                                  ipv6 address of name server
*
* @returns  L7_SUCCESS, serverAddr contains next server address
* @returns  L7_FAILURE, end of list
*
* @comments The search list is NOT returned in lexicographic order
*           empty name server address will return first entry.
*
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientNameServerUnorderedListEntryGetNext(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Get the preference for a name server.
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} v4 or v6 address 
*                                                   of name server
* @param    L7_uint32   *preference @b{{output}} server preference
*
* @returns  L7_SUCCESS, preference returned
* @returns  L7_FAILURE, name server entry not found
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientNameServerEntryPreferenceGet(L7_inet_addr_t *serverAddr,
                                                   L7_uint32 *preference);

/*********************************************************************
* @purpose  Add a static hostname mapping for the DNS client
*
* @param    L7_char8       *hostname @b{(input)} hostname
* @param    L7_inet_addr_t *inetAddr @b{(input)} ipv4 or ipv6 address of
*                                                static host addr
*
* @returns  L7_SUCCESS, hostname mapping added, or entry exists
* @returns  L7_FAILURE, hostname exists with different ip address, 
*                       or static table contains max entries
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientStaticHostEntryAdd(L7_char8 *hostname, 
                                         L7_inet_addr_t *inetAddr);

/*********************************************************************
* @purpose  Remove a static hostname mapping for the DNS client
*
* @param    L7_char8   *hostname @b{(input)} hostname
* @param    L7_uchar8  family    @b{(input)} inet family
*
* @returns  L7_SUCCESS, hostname mapping removed
* @returns  L7_FAILURE, entry did not exist
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientStaticHostEntryRemove(L7_char8 *hostname, L7_uchar8 family);

/*********************************************************************
* @purpose  Determine whether a host entry exists
*
* @param    L7_char8       *hostname @b{(input)} static hostname
* @param    L7_inet_addr_t *inetAddr @b{(input)} ipv4 or ipv6 address of
*                                                static host addr
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry not found
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientStaticHostEntryGet(L7_char8  *hostname,
                                         L7_inet_addr_t *inetAddr);

/*********************************************************************
* @purpose  Get static host entries for the DNS client
*
* @param    L7_char8       *hostname @b{(input/output)} previous/next hostname
* @param    L7_inet_addr_t *inetAddr @b{(output)} ipv4 or ipv6 address of
*                                                 static host addr
*
* @returns  L7_SUCCESS, next entry returned
* @returns  L7_FAILURE, end of list
*
* @comments empty hostname('') will return first entry.
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientStaticHostEntryNextGet(L7_char8  *hostname,
                                             L7_inet_addr_t *inetAddr);

/*********************************************************************
* @purpose  Get static host entries for the DNS client
*
* @param    L7_char8       *hostname @b{(input/output)} previous/next hostname
* @param    L7_inet_addr_t *inetAddr @b{(output)} ipv4 or ipv6 address of
*                                                 static host addr
*
* @returns  L7_SUCCESS, next entry returned
* @returns  L7_FAILURE, end of list
*
* @comments empty hostname('') will return first entry.(XUI)To handle object combo-nature.
*
*
* @end
*********************************************************************/

L7_RC_t usmDbDNSClientStaticHostEntry(L7_char8  *hostname,
                                             L7_inet_addr_t *inetAddr);

/*********************************************************************
* @purpose  Get case specific hostname to display in UI.
*
* @param    L7_char8       *hostname @b{(input)} static hostname
* @param    L7_inet_addr_t *inetAddr @b{(input)} ipv4 or ipv6 address of
*                                                static host addr
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry not found
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientStaticHostEntryDisplayNameGet(L7_char8  *hostname,
                                                    L7_inet_addr_t *inetAddr);

/*********************************************************************
 *  Additional APIs to provide read-only support for dnsResConfigGroup                            
 *********************************************************************/

/*********************************************************************
* @purpose  Get string implementation identifier for the DNS client
*
* @param    L7_char8   *identifier @b{(output)}  implementation id
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientIdentifierGet(L7_char8 *identifier);

/*********************************************************************
* @purpose  Get resolver service supported by DNS client
*
* @param    L7_uint32 *service @b{(output)}  service supported
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientServiceGet(L7_uint32 *service);

/*********************************************************************
* @purpose  Get limit on CNAME recursion
*
* @param    L7_uint32 *cnames @b{(output)}  recursion limit
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientMaxCnamesGet(L7_uint32 *cnames);

/*********************************************************************
* @purpose  Get DNS client current configuration uptime.
*
* @param    L7_uint32 *upTime @b{(output)}  system uptime
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientUpTimeGet(L7_uint32 *upTime);

/*********************************************************************
 *  DNS Cache APIs - to support dnsResCacheGroup
 *********************************************************************/

/*********************************************************************
* @purpose  Get max TTL allowed for the DNS cache
*
* @param    L7_uint32    *ttl   @b{(output)}   TTL ceiling
*
* @returns  L7_SUCCESS
*
* @comments  TTL ceiling is not supported, therefore we return zero
*            as indicated in the RFC.
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheMaxTTLGet(L7_uint32 *ttl);

/*********************************************************************
* @purpose  Get counter of RR answers successfully cached
*
* @param    L7_uint32   *caches   @b{(output)}  count of successful caches
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheGoodCachesGet(L7_uint32 *caches);

/*********************************************************************
* @purpose  Get counter of RR answers not cached
*
* @param    L7_uint32   *caches   @b{(output)}  count of unexpected answers
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheBadCachesGet(L7_uint32 *caches);

/*********************************************************************
* @purpose  Read dynamic cache entry RRs from the DNS client
*
* @param    L7_uchar8    *rrName    @b{(input/output)}   previous/next RR name
* @param    dnsRRType_t  *rrType    @b{(input/output)}   previous/next type
* @param    L7_uint32    *rrIndex   @b{{input/output}}   previous/next index
* @param    dnsRRClass_t *rrClass   @b{(output)}         entry class
* @param    L7_uchar8    *qname     @b{(output)}         qname
*
* @returns  L7_SUCCESS, next entry returned
* @returns  L7_FAILURE, end of list or DNS client not enabled
*
* @comments empty name (rrName[0] == 0) will return first cache entry RR.
*           name, type, & index uniquely identify a cache RR.
*           rrName is the DnsNameAsIndex type in binary lowercase format,
*           use dnsClientCacheRRPrettyNameGet for correct case format,
*           use dnsClientDisplayNameGet for dotted string format.
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheRRNextGet(L7_uchar8    *rrName,
                                     dnsRRType_t  *rrType,
                                     L7_uint32    *rrIndex,
                                     dnsRRClass_t *rrClass,
                                     L7_uchar8    *qname);

/*********************************************************************
* @purpose  Verify a dynamic cache entry RR exists
*
* @param    L7_uchar8    rrName    @b{(input)}   entry RR name
* @param    dnsRRType_t  rrType    @b{(input)}   entry type
* @param    L7_uint32    rrIndex   @b{{input}}   entry index
* @param    dnsRRClass_t rrClass   @b{(input)}   entry class
*
* @returns  L7_SUCCESS, entry is valid
* @returns  L7_FAILURE, 
*
* @comments name, type, & index uniquely identify a cache RR.
*           rrName is the DnsNameAsIndex type in binary lowercase format,
*           use dnsClientCacheRRPrettyNameGet for correct case format,
*           use dnsClientDisplayNameGet for dotted string format.
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheRRGet(L7_uchar8    *rrName,
                                 dnsRRType_t   rrType,
                                 L7_uint32     rrIndex,
                                 dnsRRClass_t  rrClass);

/*********************************************************************
* @purpose  Get DNS cache RR TTL value
*
* @param    L7_uchar8    *rrName   @b{(input)}   DnsNameAsIndex type
* @param    dnsRRType_t   rrType   @b{(input)}   address or cname
* @param    L7_uint32     rrIndex  @b{{input}}   index
* @param    L7_uint32    *ttl      @b{(output)}  total TTL seconds
*
* @returns  L7_SUCCESS, 
* @returns  L7_FAILURE, invalid entry
*
* @comments name, type, & index uniquely identify a cache entry.
*           
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheRRTTLGet(L7_uchar8    *rrName,
                                    dnsRRType_t   rrType,
                                    L7_uint32     rrIndex,
                                    L7_uint32    *ttl);

/*********************************************************************
* @purpose  Get DNS cache RR TTL elapsed value
*
* @param    L7_uchar8    *rrName   @b{(input)}   DnsNameAsIndex type
* @param    dnsRRType_t   rrType   @b{(input)}   address or cname
* @param    L7_uint32     rrIndex  @b{{input}}   index
* @param    L7_uint32    *elapsed  @b{(output)}  TTL seconds elapsed
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE, invalid entry
*
* @comments name, type, & index uniquely identify a cache entry.
*           
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheRRTTLElapsedGet(L7_uchar8    *rrName,
                                           dnsRRType_t   rrType,
                                           L7_uint32     rrIndex,
                                           L7_uint32    *elapsed);

/*********************************************************************
* @purpose  Get DNS cache RR source server ip address
*
* @param    L7_uchar8      *rrName   @b{(input)}   DnsNameAsIndex type
* @param    dnsRRType_t     rrType   @b{(input)}   address or cname
* @param    L7_uint32       rrIndex  @b{{input}}   index
* @param    L7_inet_addr_t *sourceAddr @b{(output)}  source server address
*
* @returns  L7_SUCCESS, 
* @returns  L7_FAILURE, invalid entry
*
* @comments name, type, & index uniquely identify a cache entry.
*           
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheRRSourceGet(L7_uchar8    *rrName,
                                       dnsRRType_t   rrType,
                                       L7_uint32     rrIndex,
                                       L7_inet_addr_t    *sourceAddr);

/*********************************************************************
* @purpose  Get data associated with an RR cache entry
*
* @param    L7_uchar8    *rrName   @b{(input)}   DnsNameAsIndex type
* @param    dnsRRType_t   rrType   @b{(input)}   address or cname
* @param    L7_uint32     rrIndex  @b{{input}}   index
* @param    L7_uchar8    *data     @b{(output)}  address or cname data
* @param    L7_BOOL      *primary  @b{(output)}  indicates RR value is 
*                                                returned in a name lookup
*
* @returns  L7_SUCCESS, data returned
* @returns  L7_FAILURE, invalid entry
*
* @comments name, type, & index uniquely identify a cache entry.
*           cname data is SNMP DnsName format, can use dnsClientDisplayNameGet
*           ip data is SNMP IpAddress format, network order
*
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheRRDataGet(L7_uchar8    *rrName,
                                     dnsRRType_t   rrType,
                                     L7_uint32     rrIndex,
                                     L7_uchar8    *data,
                                     L7_BOOL      *primary);

/*********************************************************************
* @purpose  Get case specific DnsName associated with an RR cache entry
*
* @param    L7_uchar8    *rrName     @b{(input)}   DnsNameAsIndex type
* @param    dnsRRType_t   rrType     @b{(input)}   address or cname
* @param    L7_uint32     rrIndex    @b{{input}}   index
* @param    L7_uchar8    *prettyName @b{(output)}  case specific DnsName
*
* @returns  L7_SUCCESS, data returned
* @returns  L7_FAILURE, invalid entry
*
* @comments name, type, & index uniquely identify a cache entry.
*
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheRRPrettyNameGet(L7_uchar8    *rrName,
                                           dnsRRType_t   rrType,
                                           L7_uint32     rrIndex,
                                           L7_uchar8    *prettyName);

/*********************************************************************
* @purpose  Get dotted string name from a case specific DnsName
*
* @param    L7_uchar8    *dnsName   @b{(input)}   DnsName type
* @param    L7_char8     *hostname  @b{(output)}  dotted string domain name
*
* @returns  L7_SUCCESS,
*
* @comments This can be used to translate prettyName or CNAME data.
*
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDisplayNameGet(L7_uchar8    *dnsName,
                                     L7_char8     *hostname);

/*********************************************************************
* @purpose  Flush cache values for hostname
*
* @param    L7_char8   *hostname  @b{(input)}  hostname
*
* @returns  L7_SUCCESS, entry flushed
* @returns  L7_FAILURE, no entry found for hostname
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheHostFlush(L7_char8  *hostname);

/*********************************************************************
* @purpose  Flush entire DNS cache
*
* @returns  L7_SUCCESS, entry flushed
* @returns  L7_FAILURE, invalid entry
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCacheFlush(void);

/*********************************************************************
 *      APIs to provide read-only support for dnsResCounterGroup                            
 *********************************************************************/

/*********************************************************************
* @purpose  Get DNS client query counter by opcode
*
* @param    dnsOpcode_t   opcode    @b{(input)}    DNS opcode
* @param    L7_uint32     *queries  @b{(output)}   query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid opcode
*
* @comments  The only supported opcode is DNS_OPCODE_STANDARD_QUERY 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterByOpcodeQueriesGet(dnsOpcode_t   opcode,
                                                L7_uint32    *queries);

/*********************************************************************
* @purpose  Get DNS client response counter by opcode
*
* @param    dnsOpcode_t   opcode      @b{(input)}    DNS opcode
* @param    L7_uint32     *responses  @b{(output)}   response count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid opcode
*
* @comments  The only supported opcode is DNS_OPCODE_STANDARD_QUERY 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterByOpcodeResponsesGet(dnsOpcode_t   opcode,
                                                  L7_uint32    *responses);

/*********************************************************************
* @purpose  Get first DNS client rcode.
*
* @param    dnsRcode_t   rcode       @b{(output)}    DNS rcode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid rcode
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterByRcodeFirstGet(dnsOpcode_t *rcode);

/*********************************************************************
* @purpose  Get DNS client rcode.
*
* @param    dnsRcode_t   rcode       @b{(input)}  DNS rcode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid rcode
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterByRcodeGet(dnsOpcode_t rcode);

/*********************************************************************
* @purpose  Get next DNS client rcode.
*
* @param    dnsRcode_t   rcode       @b{(input/output)}  DNS rcode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid rcode
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterByRcodeNextGet(dnsOpcode_t *rcode);

/*********************************************************************
* @purpose  Get DNS client response counter by rcode
*
* @param    dnsRcode_t   rcode       @b{(input)}    DNS rcode
* @param    L7_uint32    *responses  @b{(output)}   response count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid rcode
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterByRcodeResponsesGet(dnsOpcode_t   rcode,
                                                 L7_uint32    *responses);

/*********************************************************************
* @purpose  Get DNS client non-AA responses received counter
*
* @param    L7_uint32    *responses  @b{(output)}   response count
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterNonAuthDataResponsesGet(L7_uint32 *responses);

/*********************************************************************
* @purpose  Get DNS client non-AA responses received with no data counter
*
* @param    L7_uint32    *responses  @b{(output)}   response count
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterNonAuthNoDataResponsesGet(L7_uint32 *responses);

/*********************************************************************
* @purpose  Get DNS client martian responses, these are responses we
*           received that we don't think matched a query we sent.
*
* @param    L7_uint32    *martians  @b{(output)}   martian count
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterMartiansGet(L7_uint32 *martians);

/*********************************************************************
* @purpose  Get DNS client unparsed responses, these are responses we
*           quit parsing and discarded for any reason.
*
* @param    L7_uint32    *unparsed  @b{(output)}   unparsed responses
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterUnparsedResponsesGet(L7_uint32 *unparsed);

/*********************************************************************
* @purpose  Get DNS client total received responses.
*
* @param    L7_uint32    *responses  @b{(output)}   responses
*
* @returns  L7_SUCCESS
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterReceivedResponsesGet(L7_uint32 *responses);

/*********************************************************************
* @purpose  Get DNS client total fallbacks to safety belt name servers
*
* @param    L7_uint32    *fallbacks  @b{(output)}   fallbacks
*
* @returns  L7_SUCCESS
*
* @comments   safety belt is the same as configured name servers
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientCounterFallbacksGet(L7_uint32 *fallbacks);

/*********************************************************************
* @purpose  Get DNS request timeout value
*
* @param    L7_ushort16    *timeout  @b{(output)}   timeout
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientRequestTimeoutGet(L7_ushort16 *timeout);

/*********************************************************************
* @purpose  Set DNS request timeout value
*
* @param    L7_ushort16    timeout  @b{(output)}   timeout
*
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientRequestTimeoutSet(L7_ushort16 timeout);

/*********************************************************************
* @purpose  Get DNS query timeout value
*
* @param    L7_ushort16    *timeout  @b{(output)}   timeout
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientQueryTimeoutGet(L7_ushort16 *timeout);

/*********************************************************************
* @purpose  Set DNS query timeout value
*
* @param    L7_ushort16    timeout  @b{(output)}   timeout
*
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientQueryTimeoutSet(L7_ushort16 timeout);
  
/*********************************************************************
* @purpose  Get number of DNS request retransmits 
*
* @param    L7_uint32    *retries  @b{(output)}   retries
*
* @returns  L7_SUCCESS
*
* @comments   
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientRetransmitsGet(L7_uint32 *retries);

/*********************************************************************
* @purpose  Set number of DNS request retransmits 
*
* @param    L7_uint32    retries  @b{(output)}   retries
*
* @returns  L7_SUCCESS
*
* @comments   
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientRetransmitsSet(L7_uint32 retries);

/*********************************************************************
* @purpose  Get Configured domain name list
*
* @param    L7_char    *nameList  @b{(output)}   nameList
*
* @returns  L7_SUCCESS
*
* @comments   
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDomainNameListGet(L7_char8 *nameList);

/*********************************************************************
* @purpose  Get Next Configured domain name list in Lexicographical order
* 
* @param    L7_char    *next_domainName @b{(input)}   *next_domainName
*
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDomainNameListGetNext(L7_char8 *next_domainname);

/*********************************************************************
* @purpose  Set the client domain name
*
* @param    L7_char    *domainName @b{(input)}   *domainName
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDomainNameListSet (L7_char8 *domainname);

/*********************************************************************
* @purpose  Delete the client domain name
*
* @param    L7_char    *domainName @b{(input)}   *domainName
*
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDomainNameListRemove(L7_char8 *domainname);

/*********************************************************************
* @purpose  Get domain name list entries for the DNS client
*
* @param    L7_char8  *domainList  @b{(input/output)} domain entry for 
*                                                 unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*          
*       
* @end
*********************************************************************/
L7_RC_t usmDbDNSClientDomainNameListEntryNextGet(L7_char8 *domainList);

#endif /* USMDB_DNS_CLIENT_API_H */
