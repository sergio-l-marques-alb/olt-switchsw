/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_cache.h
*
* @purpose DNS client cache entry definitions and function prototypes
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
#ifndef DNS_CLIENT_CACHE_H
#define DNS_CLIENT_CACHE_H

#include "dns_client_api.h"
#include "dns_client.h"

#define DNS_CACHE_ENTRY_BUF_DESC      "DNSCacheBufs"    /* 16 char max */
#define DNS_CNAME_REFERENCES_MAX      5  /* limit number of CNAME references followed */
#define DNS_CACHE_ENTRY_IP_ADDRESSES  32 /* cname is much bigger, this is plenty */

typedef struct dnsCacheEntry_s
{
  dnsDomainName_t          queryHostName;/* the queried host name */
  dnsDomainName_t          hostname;
  L7_uchar8                type;        /* dnsClientCacheEntryType_t */
  L7_uint32                timestamp;   /* sysUpTime at time received, determines unqiueness and elapsed */
  L7_uint32                ttl;         /* time-to-live in hours */
  L7_inet_addr_t           server;      /* response source server ip(v4 or v6) 
                                           address */
  union 
  {
    L7_inet_addr_t              inetAddrs[DNS_CACHE_ENTRY_IP_ADDRESSES];
    dnsDomainName_t        cname;
  } data;
  struct dnsCacheEntry_s  *listNext;    /* next in cache order, lexiographic */
  struct dnsCacheEntry_s  *next;        /* maintains recently used cache order */
  struct dnsCacheEntry_s  *prev;
} dnsCacheEntry_t;

typedef struct
{
  dnsCacheEntry_t      *cacheList;  /* lexiographic ordered list */
  dnsCacheEntry_t      *head;       /* most recently used entry */
  dnsCacheEntry_t      *tail;       /* least recently used entry */
} dnsCacheData_t;

/*********************************************************************
* @purpose  Return first cache entry
*
* @param    none
*
* @returns  none
*
* @notes 
*       
* @end
*********************************************************************/
L7_RC_t dnsCacheEntryFirstGet(dnsCacheEntry_t **first);

/*********************************************************************
* @purpose  Lookup cache entry based on internal "binary" name
*
* @param    dnsDomainName_t  *name      @b((input))   internal format
* @param    dnsCacheEntry_t **entry     @b((output))  pointer to cache entry
*
* @returns  L7_SUCCESS, entry returned
* @returns  L7_FAILURE, entry not found or entry found and expired
*
* @notes
*
* @end

*********************************************************************/
L7_RC_t dnsCacheEntryNameOnlyLookup(dnsDomainName_t  *name,
                                    dnsCacheEntry_t **entry);

/*********************************************************************
* @purpose  Lookup cache entry based on internal "binary" name
*
* @param    L7_uchar8       *labels       @b((input))   internal label format
* @param    dnsRRType_t     type          @b((input))   RR Type
* @param    dnsCacheEntry_t **entry       @b((output))  pointer to cache entry
*
* @returns  L7_SUCCESS, entry returned
* @returns  L7_FAILURE, entry not found or entry found and expired
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dnsCacheEntryLookup(L7_uchar8        *labels, 
                            dnsRRType_t       type,
                            dnsCacheEntry_t **entry);

/*********************************************************************
* @purpose  Get a specific RR from a cache entry 
*
* @param    L7_uchar8       *name       @b((input))   internal label format
* @param    dnsRRType_t     type        @b((input))   expected type
* @param    L7_uint32       index       @b((input))   entry index
* @param    dnsCacheEntry_t **entry     @b((output))  pointer to cache entry
*
* @returns  L7_SUCCESS, entry returned
* @returns  L7_FAILURE, entry not found or invalid
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dnsCacheEntryRRGet(L7_uchar8        *labels, 
                           dnsRRType_t       type,
                           L7_uint32         index, 
                           dnsCacheEntry_t **entry);

/*********************************************************************
* @purpose  Lookup next dynamic cache entry
*
* @param    L7_uchar8       *labels     @b((input))   internal name format
* @param    dnsRRType_t     type          @b((input))   RR Type
* @param    dnsCacheEntry_t *nextEntry  @b((output))  pointer to next cache entry
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dnsCacheEntryLookupNext(L7_uchar8        *labels, 
                                dnsRRType_t       type,
                                dnsCacheEntry_t **nextEntry);

/*********************************************************************
* @purpose  Lookup dynamic cache entry based on string hostname
*
* @param    L7_char8        *hostname   @b((input))   dotted string name
* @param    dnsRRType_t     type          @b((input)) RR Type
* @param    dnsCacheEntry_t **entry     @b((output))  pointer to cache entry
*
* @returns  void
*
* @notes    if timestamp == 0, will not verify entry timestamp
*       
* @end
*********************************************************************/
L7_RC_t dnsCacheEntryStringLookup(L7_char8         *hostname, 
                                  dnsRRType_t       type,
                                  dnsCacheEntry_t **entry);

/*********************************************************************
* @purpose  Add a cache entry and associated data
*
* @param    dnsDomainName_t           *name @b((input)) internal name format of the host name queried
* @param    dnsDomainName_t           *name @b((input)) internal name format
* @param    dnsClientCacheEntryType_t type  @b((input)) ip or cname
* @param    L7_uint32                 ttl   @b((input)) time to live
* @param    L7_inet_addr_t            server@b((input)) server ip (v4 or v6) 
*                                                       data was received from
* @param    void                      *data @b((input)) ip list or cname
*
* @returns  L7_SUCCESS, entry added
* @returns  L7_FAILURE, 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t dnsCacheEntryAdd(dnsDomainName_t          *qname,
                         dnsDomainName_t          *name,
                         dnsRRType_t               type,
                         L7_uint32                 ttl,
                         L7_inet_addr_t           *server,
                         void                     *data);

/*********************************************************************
* @purpose  Delete a cache entry and release associated buffer
*
* @param    dnsCacheEntry_t *entry   @b((input)) cache entry pointer
*
* @returns  void
*
* @notes    entry is invalid after this call 
*       
* @end
*********************************************************************/
void dnsCacheEntryDelete(dnsCacheEntry_t *entry);
/*********************************************************************
* @purpose  Lookup ip address(es) for a host, follow cname entries.
*
* @param    dnsDomainName_t *name     @b((input))  internal name
* @param    L7_uchar8       type      @b((input))  inet family
* @param    L7_inet_addr_t  *inetAddr @b((output)) inet address
*
* @returns  L7_SUCCESS, ip address found
* @returns  L7_FAILURE, no ip address for hostname
*
* @notes    hosts can have multiple ip addresses in the cache
*           this will always return the first ip, if we decide to
*           choose a default ip based on some criteria this is
*           the function to modify...
*       
* @end
*********************************************************************/
L7_RC_t dnsCacheIpLookup(dnsDomainName_t *name,
                         L7_uchar8       type,         
                         L7_inet_addr_t  *inetAddr);

/*********************************************************************
* @purpose  Lookup hostname foe an ip address.
*
* @param    L7_inet_addr_t  inetIp  @b((input))  inet address
* @param    dnsDomainName_t *name   @b((output)) internal name
*
* @returns  L7_SUCCESS, ip address found
* @returns  L7_FAILURE, no ip address for hostname
*
* @end
*********************************************************************/
L7_RC_t dnsCacheNameLookup(L7_inet_addr_t   inetIp,
                           dnsDomainName_t *name);

/*********************************************************************
* @purpose  Purge all dynamic cache entries
*
* @param    none
*
* @returns  none
*
* @notes 
*       
* @end
*********************************************************************/
void dnsCacheEntriesPurge(void);

/*********************************************************************
* @purpose  Delete all expired dynamic cache entries
*
* @param    none
*
* @returns  number of entries deleted
*
* @notes 
*       
* @end
*********************************************************************/
void dnsCacheExpiredEntriesDelete(void);

/*********************************************************************
*
* @purpose  Add a static hostname mapping.
*
* @param    dnsDomainName_t  *hostname   @b((input)) hostname in internal format
* @param    L7_inet_addr_t   *inetAddr   @b{(input)} ipv4 or ipv6 address of
*                                                    static host addr
*
* @returns  L7_SUCCESS, entry added
*           L7_FAILURE, table full
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dnsCacheStaticEntryAdd(dnsDomainName_t  *name,
                               L7_inet_addr_t   *inetAddr);

/*********************************************************************
*
* @purpose  Remove a static hostname mapping.
*
* @param    dnsDomainName_t  *hostname   @b((input)) hostname in internal format
* @param    L7_uchar8       type      @b((input))  inet family
*
* @returns  L7_SUCCESS, entry removed
*           L7_FAILURE,
*
* @notes    maintain static hosts in the order configured
*
* @end
*********************************************************************/
L7_RC_t dnsCacheStaticEntryDelete(dnsDomainName_t  *name, L7_uchar8 family);

/*********************************************************************
*
* @purpose  Lookup an entry in the static configuration
*
* @param    L7_char8  *hostname   @b((input)) hostname
* @param    L7_uchar8 family      @b((input))  inet family
* @param    L7_uint32 *index      @b((output)) configuration index
*
* @returns  L7_SUCCESS, entry found
*           L7_FAILURE, entry not found
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dnsCacheStaticEntryStringLookup(L7_char8  *hostname,
                                        L7_uchar8 family,
                                        L7_uint32 *index);

/*********************************************************************
*
* @purpose  Lookup an entry in the static configuration
*
* @param    L7_char8             *hostname  @b((input))  hostname
* @param    L7_uchar8 family      @b((input))  inet family
* @param    dnsStaticHostEntry_t **entry    @b((output)) static host entry
*
* @returns  L7_SUCCESS, entry found
*           L7_FAILURE, entry not found
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dnsCacheStaticEntryStringLookupNext(L7_char8             *hostname,
                                            L7_uchar8             family,
                                            dnsStaticHostEntry_t **entry);

/*********************************************************************
*
* @purpose  Lookup an inetAddr in the static configuration
*
* @param    dnsDomainName_t *name @b((input))   name in internal format
* @param    L7_uchar8 family      @b((input))  inet family
* @param    L7_inet_addr_t  *inetAddr   @b{(output)} ipv4 or ipv6 address of
*                                                   static host addr
*
* @returns  L7_SUCCESS, 
* @returns  L7_FAILURE, did not find host in static configuration
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dnsCacheStaticIpLookup(dnsDomainName_t *name,
                               L7_uchar8       family,
                               L7_inet_addr_t  *inetAddr);

/*********************************************************************
*
* @purpose  Lookup a name in the static configuration
*
* @param    L7_inet_addr_t  inetIp @b((input))  inet ip address
* @param    dnsDomainName_t *name @b((output)) name in internal format
*
* @returns  L7_SUCCESS, 
* @returns  L7_FAILURE, did not find host in static configuration
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dnsCacheStaticNameLookup(L7_inet_addr_t   inetIp,
                                 dnsDomainName_t *name);

/*********************************************************************
* @purpose  Return first configured static host
*
* @param    dnsStaticHostEntry_t **first    @b((output)) ptr to entry pointer 
*
* @returns  none
*
* @notes 
*       
* @end
*********************************************************************/
L7_RC_t dnsCacheStaticEntryFirstGet(dnsStaticHostEntry_t **first);


#endif /* DNS_CLIENT_CACHE_H */
