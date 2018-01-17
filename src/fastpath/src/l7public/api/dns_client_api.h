/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_api.h
*
* @purpose DNS client definitions
*
* @component DNS client
*
* @comments none
*
* @create 02/28/2005
*
* @author dfowler
* @end
*
**********************************************************************/
#ifndef DNS_CLIENT_API_H
#define DNS_CLIENT_API_H

#include "l7_common.h"
#include "l3_addrdefs.h"

#define DNS_DOMAIN_NAME_SIZE_MAX            255
#define DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX    DNS_DOMAIN_NAME_SIZE_MAX + 2
#define DNS_INET_ADDR_LEN                   40


typedef enum {
  DNS_LOOKUP_STATUS_SUCCESS = 0,
  DNS_LOOKUP_STATUS_TIMEOUT,            /* request timed out */
  DNS_LOOKUP_STATUS_NAME_ERROR,         /* received a name error response from server */
  DNS_LOOKUP_STATUS_SOCKET_ERROR,       /* failed to send request packet */
  DNS_LOOKUP_STATUS_RESPONSE_ERROR,     /* received error response from server */
  DNS_LOOKUP_STATUS_PACKET_ERROR,       /* failed to parse packet from server */
  DNS_LOOKUP_STATUS_NO_SERVER,          /* host not found in cache and no name servers configured */
  DNS_LOOKUP_STATUS_DISABLED,           /* client admin mode disabled during request processing */
  DNS_LOOKUP_STATUS_FAILURE             /* any other internal failure */
} dnsClientLookupStatus_t;

typedef enum {
  DNS_RR_TYPE_ADDRESS = 1,
  DNS_RR_TYPE_CNAME = 5,
  DNS_RR_TYPE_PTR = 12,
  DNS_RR_TYPE_IPV6_ADDRESS = 28
} dnsRRType_t;

typedef enum {
  DNS_RR_CLASS_INTERNET = 1
} dnsRRClass_t;

typedef enum {
  DNS_OPCODE_STANDARD_QUERY = 0
} dnsOpcode_t;

typedef enum
{
  DNS_RCODE_NO_ERROR = 0,
  DNS_RCODE_FORMAT_ERROR = 1,
  DNS_RCODE_SERVER_FAILURE = 2,
  DNS_RCODE_NAME_ERROR = 3,
  DNS_RCODE_NOT_IMPLEMENTED = 4,
  DNS_RCODE_REFUSED = 5,
  DNS_RCODE_RESERVED = 6  /* 6 - 15 not supported, reserved for future use */
} dnsRcodes_t;

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
L7_RC_t dnsClientAdminModeSet(L7_BOOL enable);

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
L7_RC_t dnsClientAdminModeGet(L7_BOOL *enable);

/*********************************************************************
* @purpose  Set the request timeout for the DNS client
*
* @param    L7_ushort16  timeout  @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dnsClientRequestTimeoutSet(L7_ushort16 timeout);

/*********************************************************************
* @purpose  Get the request timeout for the DNS client
*
* @param    L7_ushort16  *timeout  @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dnsClientRequestTimeoutGet(L7_ushort16 *timeout);

/*********************************************************************
* @purpose  Set the query timeout for the DNS client
*
* @param    L7_ushort16  timeout  @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dnsClientQueryTimeoutSet(L7_ushort16 timeout);

/*********************************************************************
* @purpose  Get the query timeout for the DNS client
*
* @param    L7_ushort16  *timeout  @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dnsClientQueryTimeoutGet(L7_ushort16 *timeout);

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
*           This will used only when no entry in domain list is configured
*       
* @end
*********************************************************************/
L7_RC_t dnsClientDefaultDomainSet(L7_char8 *domain);

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
L7_RC_t dnsClientDefaultDomainGet(L7_char8 *domain);

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
L7_RC_t dnsClientDefaultDomainEntryGetNext(L7_char8 *domain);

/*********************************************************************
* @purpose  Add a name server for the DNS client
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} ipv4 or ipv6 address of
*                                           name server
*
* @returns  L7_SUCCESS, server added or already exists
* @returns  L7_FAILURE, server list contains max entries
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientNameServerEntryAdd(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Remove a name server for the DNS client
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} ipv4 or ipv6 address of 
*                                           name server
*
* @returns  L7_SUCCESS, server removed
* @returns  L7_FAILURE, server not found
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientNameServerEntryRemove(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Determine if a name server entry exists.
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} ipv4 or ipv6 address of 
*                                           name server
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry does not exist
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientNameServerEntryGet(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Get name servers for the DNS client
*
* @param    L7_inet_addr_t   *serverAddr @b{(input/output)} ipv4 or 
*                                  ipv6 address of name server
*
* @returns  L7_SUCCESS, ip contains next server address
* @returns  L7_FAILURE, end of list
*
* @comments ip of zero will return first server ip
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientNameServerEntryNextGet(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Get Next Name Server for the DNS client
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
L7_RC_t dnsClientNameServerUnorderedListEntryGetNext(L7_inet_addr_t *serverAddr);

/*********************************************************************
* @purpose  Get the preference for a name server.
*
* @param    L7_inet_addr_t  serverAddr @b{(input)} ipv4 or ipv6 address of 
*                                          name server
* @param    L7_uint32       *preference @b{{output}} server preference
*
* @returns  L7_SUCCESS, preference returned
* @returns  L7_FAILURE, name server entry not found
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientNameServerEntryPreferenceGet(L7_inet_addr_t *serverAddr,
                                              L7_uint32 *preference);
/*********************************************************************
* @purpose  Add a static hostname mapping for the DNS client
*
* @param    L7_char8        *hostname @b{(input)} hostname
* @param    L7_inet_addr_t  *inetAddr @b{(input)} ipv4 or ipv6 address of
*                                                 static host addr
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
L7_RC_t dnsClientStaticHostEntryAdd(L7_char8 *hostname, L7_inet_addr_t  *inetAddr);

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
L7_RC_t dnsClientStaticHostEntryRemove(L7_char8 *hostname, L7_uchar8 family);

/*********************************************************************
* @purpose  Determine whether a host entry exists
*
* @param    L7_char8        *hostname @b{(input)} static hostname
* @param    L7_inet_addr_t  *inetAddr @b{(input)} ipv4 or ipv6 address of
*                                                 static host addr
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry not found
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientStaticHostEntryGet(L7_char8  *hostname,
                                    L7_inet_addr_t  *inetAddr);

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
L7_RC_t dnsClientStaticHostEntryNextGet(L7_char8  *hostname,
                                        L7_inet_addr_t  *inetAddr);

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
* @comments empty hostname('') will return first entry.This one if to handle XUI Combo key
                               Issue.
*
*
* @end
*********************************************************************/
L7_RC_t dnsClientStaticHostEntry (L7_char8  *hostname,
                                  L7_inet_addr_t  *inetAddr);

/*********************************************************************
* @purpose  Get case specific hostname to display in UI.
*
* @param    L7_char8        *hostname @b{(input)} static hostname
* @param    L7_inet_addr_t  *inetAddr @b{(input)} ipv4 or ipv6 address of
*                                                 static host addr
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry not found
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientStaticHostEntryDisplayNameGet(L7_char8  *hostname,
                                               L7_inet_addr_t  *inetAddr);

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
L7_RC_t dnsClientIdentifierGet(L7_char8 *identifier);

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
L7_RC_t dnsClientServiceGet(L7_uint32 *service);

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
L7_RC_t dnsClientMaxCnamesGet(L7_uint32 *cnames);

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
L7_RC_t dnsClientUpTimeGet(L7_uint32 *upTime);

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
L7_RC_t dnsClientCacheMaxTTLGet(L7_uint32 *ttl);

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
L7_RC_t dnsClientCacheGoodCachesGet(L7_uint32 *caches);

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
L7_RC_t dnsClientCacheBadCachesGet(L7_uint32 *caches);

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
L7_RC_t dnsClientCacheRRNextGet(L7_uchar8    *rrName,
                                dnsRRType_t  *rrType,
                                L7_uint32    *rrIndex,
                                dnsRRClass_t *rrClass,
                                L7_uchar8    *qname);

/*********************************************************************
* @purpose  Verify a dynamic cache entry RR exists
*
* @param    L7_uchar8    *rrName   @b{(input)}   entry RR name
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
L7_RC_t dnsClientCacheRRGet(L7_uchar8    *rrName,
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
L7_RC_t dnsClientCacheRRTTLGet(L7_uchar8    *rrName,
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
L7_RC_t dnsClientCacheRRTTLElapsedGet(L7_uchar8    *rrName,
                                      dnsRRType_t   rrType,
                                      L7_uint32     rrIndex,
                                      L7_uint32    *elapsed);

/*********************************************************************
* @purpose  Get DNS cache RR source server ip address
*
* @param    L7_uchar8      *rrName     @b{(input)}   DnsNameAsIndex type
* @param    dnsRRType_t    rrType      @b{(input)}   address or cname
* @param    L7_uint32      rrIndex     @b{{input}}   index
* @param    L7_inet_addr_t *sourceAddr @b{(output)}  source server ip (v4 
*                                                    or v6)address
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
L7_RC_t dnsClientCacheRRSourceGet(L7_uchar8        *rrName,
                                  dnsRRType_t      rrType,
                                  L7_uint32        rrIndex,
                                  L7_inet_addr_t   *sourceAddr);

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
L7_RC_t dnsClientCacheRRDataGet(L7_uchar8    *rrName,
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
L7_RC_t dnsClientCacheRRPrettyNameGet(L7_uchar8    *rrName,
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
L7_RC_t dnsClientDisplayNameGet(L7_uchar8    *dnsName,
                                L7_char8     *hostname);

/*********************************************************************
* @purpose  Flush cache entries for a specific hostname
*
* @param    L7_char8    *hostname   @b{(input)}   string hostname
*
* @returns  L7_SUCCESS, entry flushed
* @returns  L7_FAILURE, invalid entry
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientCacheHostFlush(L7_char8  *hostname);

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
L7_RC_t dnsClientCacheFlush(void);

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
L7_RC_t dnsClientCounterByOpcodeQueriesGet(dnsOpcode_t   opcode,
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
L7_RC_t dnsClientCounterByOpcodeResponsesGet(dnsOpcode_t   opcode,
                                             L7_uint32    *responses);

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
L7_RC_t dnsClientCounterByRcodeResponsesGet(dnsRcodes_t   rcode,
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
L7_RC_t dnsClientCounterNonAuthDataResponsesGet(L7_uint32 *responses);

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
L7_RC_t dnsClientCounterNonAuthNoDataResponsesGet(L7_uint32 *responses);

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
L7_RC_t dnsClientCounterMartiansGet(L7_uint32 *martians);

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
L7_RC_t dnsClientCounterUnparsedResponsesGet(L7_uint32 *unparsed);

/*********************************************************************
 *                DNS Client Request APIs                            
 *********************************************************************/

/*********************************************************************
*
* @purpose Register a routine to be called when a name lookup completes.
*
* @param L7_COMPONENT_IDS_t componentId  @b((input)) component ID      
* @param *notify      @b((input)) pointer to a routine to be invoked with a response.
*                                 Each routine accepts the following parameters:
*                                 dnsClientLookupStatus_t status
*                                 L7_ushort16             requestId  
*                                 L7_uchar8               recordType
*                                 L7_char8                *hostname (qualified hostname)
*                                 L7_inet_addr_t          *inetAddr 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, hostname is NULL and ip is 0
*
* @comments  
*                                 
* @end
*
*********************************************************************/
L7_RC_t dnsClientNameLookupResponseRegister(L7_COMPONENT_IDS_t componentId, 
                                            L7_RC_t (*notify)(dnsClientLookupStatus_t status,
                                                              L7_ushort16 requestId,
                                                              L7_uchar8 recordType,
                                                              L7_char8   *hostname,
                                                              L7_inet_addr_t   *inetAddr));

/*********************************************************************
*
* @purpose Deregister a routine to be called for a name lookup response.
*
* @param L7_COMPONENT_IDS_t componentId  @b((input)) component ID      
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, invalid argument
*
* @comments
*                                 
* @end
*
*********************************************************************/
L7_RC_t dnsClientNameLookupResponseDeregister(L7_COMPONENT_IDS_t componentId);

/*********************************************************************
*
* @purpose Request a non-blocking DNS lookup for hostname.
*
* @param L7_uchar8          family       @b((input)) family type
* @param L7_COMPONENT_IDS_t componentId  @b((input)) component ID  
* @param L7_ushort16        requestId    @b((input)) caller's id to identify response     
* @param L7_char8          *hostname     @b((input)) hostname     
*
* @returns L7_SUCCESS, request generated
* @returns L7_FAILURE, client disabled, invalid parameter or no available request buffers
*
* @comments caller must have registered a notify function using 
*           dnsClientNameLookupResponseRegister,
*           the name lookup response is asynchronously returned via the
*           registered notify function for the component.
*                                 
* @end
*
*********************************************************************/
L7_RC_t dnsClientNameLookupRequest(L7_uchar8          family,
                                   L7_COMPONENT_IDS_t componentId, 
                                   L7_ushort16        requestId,
                                   L7_char8          *hostname);

/*********************************************************************
*
* @purpose Request a non-blocking DNS reverse lookup for ip.
*
* @param L7_COMPONENT_IDS_t componentId  @b((input)) component ID  
* @param L7_ushort16        requestId    @b((input)) caller's id to identify response     
* @param L7_inet_addr_t     inetIp       @b{{input}}  ip address
*
* @returns L7_SUCCESS, request generated
* @returns L7_FAILURE, client disabled, invalid parameter or no available request buffers
*
* @comments caller must have registered a notify function using 
*           dnsClientNameLookupResponseRegister,
*           the name lookup response is asynchronously returned via the
*           registered notify function for the component.
*                                 
* @end
*
*********************************************************************/
L7_RC_t dnsClientReverseNameLookupRequest(L7_COMPONENT_IDS_t componentId, 
                                          L7_ushort16        requestId,
                                          L7_inet_addr_t     inetIp);

/*********************************************************************
*
* @purpose Request a blocking DNS reverse lookup for an ip.
*
* @param L7_inet_addr_t           inetIp       @b{{input}}  ip address
* @param dnsClientLookupStatus_t *status       @b{{output}} specific L7_FAILURE status
* @param L7_char8                *hostname     @b((output)) request hostname 
* @param L7_char8                *hostFQDN     @b{{output}} fully qualified hostname
*
* @returns L7_SUCCESS, hostFQDN and ip are returned.
* @returns L7_FAILURE, specific failure is indicated in status hostFQDN and ip are not valid.
*
* @comments hostname buffer must be DNS_DOMAIN_NAME_SIZE_MAX
* @comments hostFQDN buffer must be DNS_DOMAIN_NAME_SIZE_MAX
*                                 
* @end
*
*********************************************************************/
L7_RC_t dnsClientReverseNameLookup(L7_inet_addr_t           inetIp,
                                   L7_char8                *hostname,
                                   dnsClientLookupStatus_t *status,
                                   L7_char8                *hostFQDN);
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
* @comments hostFQDN buffer must be DNS_DOMAIN_NAME_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t dnsClientNameLookup(L7_char8                *hostname,
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
* @param L7_inet_addr_t          *inetAddr     @b{(input)} ipv4 or ipv6 address passed
*                                                          to the application. 
*
* @returns L7_SUCCESS, hostFQDN and ip are returned.
* @returns L7_FAILURE, specific failure is indicated in status hostFQDN and ip are not valid.
*
* @comments hostFQDN buffer must be DNS_DOMAIN_NAME_SIZE_MAX
*                                 
* @end
*
*********************************************************************/
L7_RC_t dnsClientInetNameLookup(L7_uchar8               family,
                                L7_char8                *hostname,
                                dnsClientLookupStatus_t *status,
                                L7_char8                *hostFQDN,
                                L7_inet_addr_t          *inetAddr);

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
L7_RC_t dnsClientRequestRetransmitsGet(L7_uint32 *retries);

/*********************************************************************
* @purpose  Set number of DNS request retransmits
*
* @param    L7_uint32    retries  @b{(input)}   retries
*
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dnsClientRequestRetransmitsSet(L7_uint32 retries);

/*********************************************************************
* @purpose  Add the domain name for the DNS client to the list
*
* @param    L7_char8  domain @b{(input)} domain for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Domain will be directly appended to the unqualifed name
*           without splitting in to multiple labels as in default domain name
*       
* @end
*********************************************************************/
L7_RC_t dnsClientDomainNameListAdd(L7_char8 *domainname);

/*********************************************************************
* @purpose  Remove the domain name from the name list
*
* @param    L7_char8  domain @b{(input)} domain for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t dnsClientDomainNameListRemove(L7_char8 *domainname);

/*********************************************************************
* @purpose  Get the configured domain list entry for the DNS client
*
* @param    L7_char8  *domain  @b{(output)} domain for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dnsClientDomainNameListGet(L7_char8 *domain);

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
L7_RC_t dnsClientDomainNameListEntryGetNext(L7_char8 *domain);

#endif /* DNS_CLIENT_API_H */

