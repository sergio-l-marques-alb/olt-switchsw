/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_cache.c
*
* @purpose DNS client cache management functions
*
* @component DNS client
*
* @comments
*
* @create 02/28/2005
*
* @author dfowler
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "buff_api.h"
#include "dns_client.h"
#include "dns_client_api.h"
#include "dns_client_cache.h"
#include "dns_client_util.h"

extern dnsCacheData_t *dnsCacheData;
extern dnsCfgData_t   *dnsCfgData;
extern L7_uint32       dnsCachePoolId;

/*********************************************************************
* @purpose  Determine if a cache entry has expired
*
* @param    dnsCacheEntry_t *entry   @b((input)) cache entry pointer
*
* @returns  L7_TRUE, entry has expired
* @returns  L7_FALSE,
*
* @notes
*
* @end
*********************************************************************/
static L7_BOOL dnsCacheEntryExpiredCheck(dnsCacheEntry_t *entry)
{
  /* add one second, expired if this entry will expire within the next second */
  L7_uint32 elapsed = (osapiUpTimeRaw()+1) - entry->timestamp;

  if (elapsed >= entry->ttl)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Lookup cache entry based on internal "binary" name
*
* @param    dnsDomainName_t  *name      @b((input))   internal format
* @param    dnsRRType_t      type       @b((input))   RR Type
* @param    dnsCacheEntry_t **entry     @b((output))  pointer to cache entry
*
* @returns  L7_SUCCESS, entry returned
* @returns  L7_FAILURE, entry not found or entry found and expired
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t dnsCacheEntryNameLookup(dnsDomainName_t  *name,
                                       dnsRRType_t       type,
                                       dnsCacheEntry_t **entry)
{
  dnsCacheEntry_t *p = L7_NULLPTR;

  p = dnsCacheData->cacheList;
  while (p != L7_NULLPTR)
  {
    L7_int32 result = 0;
    result = dnsNameCompare(name, &p->hostname);
    if (result == 0 && (type == p->type))
    {
      break;
    }
    if (result < 0)
    {
      return L7_FAILURE;
    }
    p = p->listNext;
  }
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* do not return expired entires */
  if (dnsCacheEntryExpiredCheck(p) == L7_TRUE)
  {
    dnsCacheEntryDelete(p);
    return L7_FAILURE;
  }

  *entry = p;
  return L7_SUCCESS;
}

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
                                    dnsCacheEntry_t **entry)
{
  dnsCacheEntry_t *p = L7_NULLPTR;

  p = dnsCacheData->cacheList;
  while (p != L7_NULLPTR)
  {
    L7_int32 result = 0;
    result = dnsNameCompare(name, &p->hostname);
    if (result == 0)
    {
      break;
    }
    if (result < 0)
    {
      return L7_FAILURE;
    }
    p = p->listNext;
  }
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* do not return expired entires */
  if (dnsCacheEntryExpiredCheck(p) == L7_TRUE)
  {
    dnsCacheEntryDelete(p);
    return L7_FAILURE;
  }

  *entry = p;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Lookup cache entry based on ip address
*
* @param    L7_inet_addr_t  inetIp  @b((input))  inet address
* @param    dnsCacheEntry_t **entry  @b((output)) pointer to cache entry
*
* @returns  L7_SUCCESS, entry returned
* @returns  L7_FAILURE, entry not found or entry found and expired
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t dnsCacheEntryIpLookup(L7_inet_addr_t    inetIp,
                                     dnsCacheEntry_t **entry)
{
  dnsCacheEntry_t *p = L7_NULLPTR;

  p = dnsCacheData->cacheList;
  while (p != L7_NULLPTR)
  {
    if (L7_INET_ADDR_COMPARE(&(p->data.inetAddrs[0]),
                               &inetIp) == L7_NULL)
    {
      break;
    }
    p = p->listNext;
  }
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* do not return expired entires */
  if (dnsCacheEntryExpiredCheck(p) == L7_TRUE)
  {
    dnsCacheEntryDelete(p);
    return L7_FAILURE;
  }

  *entry = p;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Lookup cache entry based on internal "binary" labels
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
                            dnsCacheEntry_t **entry)
{
  dnsDomainName_t name;

  memset(&name, 0, sizeof(dnsDomainName_t));
  memcpy(name.labels, labels, DNS_NAME_LABELS_SIZE_MAX);

  return dnsCacheEntryNameLookup(&name, type, entry);
}

/*********************************************************************
* @purpose  Get specific RR from a cache entry
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
                           dnsCacheEntry_t **entry)
{
  dnsCacheEntry_t *p = L7_NULLPTR;

  if (dnsCacheEntryLookup(labels, type, &p) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* verify input type and index */
  if ((p->type != type) ||
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
      (((p->type == DNS_RR_TYPE_IPV6_ADDRESS)||
        (p->type == DNS_RR_TYPE_ADDRESS)) &&
       (dnsClientIsInetAddressZero(&p->data.inetAddrs[index]) == L7_TRUE)) ||
#else
      ((p->type == DNS_RR_TYPE_ADDRESS) &&
       (dnsClientIsInetAddressZero(&p->data.inetAddrs[index]) == L7_TRUE)) ||
#endif
      ((p->type == DNS_RR_TYPE_CNAME) && (index != 0)))
  {
    return L7_FAILURE;
  }

  *entry = p;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Lookup next dynamic cache entry
*
* @param    L7_uchar8       *labels     @b((input))   internal name format
* @param    dnsRRType_t     type        @b((input))   RR Type
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
                                dnsCacheEntry_t **nextEntry)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;
  if (dnsCacheEntryLookup(labels, type, &entry) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (entry->listNext != L7_NULLPTR)
  {
    *nextEntry = entry->listNext;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Lookup dynamic cache entry based on string hostname
*
* @param    L7_char8        *hostname   @b((input))   dotted string name
* @param    dnsRRType_t     type        @b((input))   RR Type
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
                                  dnsCacheEntry_t **entry)
{
  dnsDomainName_t  name;

  if (dnsStringNameToInternalConvert(hostname, L7_TRUE, &name) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return dnsCacheEntryNameLookup(&name, type, entry);
}

/*********************************************************************
* @purpose  Remove entry from recently used ordered list.
*
* @param    dnsCacheEntry_t *entry   @b((input))  cache entry
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dnsCacheListEntryRemove(dnsCacheEntry_t *entry)
{
  if (entry->prev == L7_NULLPTR)
  {
    dnsCacheData->head = entry->next;
  } else
  {
    entry->prev->next = entry->next;
  }
  if (entry->next == L7_NULLPTR)
  {
    dnsCacheData->tail = entry->prev;
  } else
  {
    entry->next->prev = entry->prev;
  }
  return;
}

/*********************************************************************
* @purpose  Add entry to head of recently used ordered list.
*
* @param    dnsCacheEntry_t *entry   @b((input))  cache entry
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dnsCacheListEntryHeadAdd(dnsCacheEntry_t *entry)
{
  entry->prev = L7_NULLPTR;
  if (dnsCacheData->head != L7_NULLPTR)
  {
    dnsCacheData->head->prev = entry;
  }
  entry->next = dnsCacheData->head;
  dnsCacheData->head = entry;
  if (dnsCacheData->tail == L7_NULLPTR)
  {
    dnsCacheData->tail = entry;
  }
  return;
}

/*********************************************************************
* @purpose  Move entry to beginning of recently used ordered list.
*
* @param    dnsCacheEntry_t *entry   @b((input))  cache entry
*
* @returns  void
*
* @notes    This ensures we remove least used entries first.
*
* @end
*********************************************************************/
static void dnsCacheEntryUsed(dnsCacheEntry_t *entry)
{
  if (entry != dnsCacheData->head)
  {
    dnsCacheListEntryRemove(entry);
    dnsCacheListEntryHeadAdd(entry);
  }
  return;
}

/*********************************************************************
* @purpose  Lookup ip address(es) for a host, follow cname entries.
*
* @param    dnsDomainName_t *name     @b((input))  internal name
* @param    type                      @b((input))  inet family
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
                         L7_uchar8 type,
                         L7_inet_addr_t  *inetAddr)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;
  L7_uint32 refs = 0;
  dnsRRType_t rrType;
  L7_BOOL found = L7_FALSE;

  if (type == L7_AF_INET)
  {
    rrType = DNS_RR_TYPE_ADDRESS;
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if (type == L7_AF_INET6)
  {
     rrType = DNS_RR_TYPE_IPV6_ADDRESS;
  }
#endif
  else
  {
     return L7_FAILURE;
  }
  if (dnsCacheEntryNameLookup (name, rrType, &entry ) == L7_SUCCESS)
  {
     found = L7_TRUE;
  }
  if ((found != L7_TRUE) && (dnsCacheEntryNameOnlyLookup(name, &entry) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  while ((found = L7_TRUE) &&(entry->type == DNS_RR_TYPE_CNAME))
  {
    if (refs++ > DNS_CNAME_REFERENCES_MAX)
    {
      return L7_FAILURE;
    }
    dnsCacheEntryUsed(entry);
    if (dnsCacheEntryNameOnlyLookup(&entry->data.cname, &entry) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  if ((type == L7_AF_INET && (entry->type != DNS_RR_TYPE_ADDRESS))
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
      ||(type == L7_AF_INET6 && (entry->type != DNS_RR_TYPE_IPV6_ADDRESS))
#endif
)
  {
     return L7_FAILURE;
  }
  dnsCacheEntryUsed(entry);
  inetCopy(inetAddr, &entry->data.inetAddrs[0]);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Lookup hostname for an ip address.
*
* @param    L7_inet_addr_t  inetIp  @b((input))  inet address
* @param    dnsDomainName_t *name   @b((output)) internal name
*
* @returns  L7_SUCCESS, ip address found
* @returns  L7_FAILURE, no ip address for hostname
*
* @end
*********************************************************************/
L7_RC_t dnsCacheNameLookup(L7_inet_addr_t  inetIp,
                           dnsDomainName_t *name)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;

  if (dnsCacheEntryIpLookup(inetIp, &entry) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  dnsCacheEntryUsed(entry);
  memcpy(name, &(entry->hostname), sizeof(dnsDomainName_t));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clean up cache entry pointers.
*
* @param    dnsCacheEntry_t *entry   @b((input)) cache entry pointer
*
* @returns  void
*
* @notes    we can reuse the cache entry after this call
*
* @end
*********************************************************************/
static void dnsCacheEntryClear(dnsCacheEntry_t *entry)
{
  dnsCacheEntry_t *prev = L7_NULLPTR;
  dnsCacheEntry_t *curr = L7_NULLPTR;

  /* remove from lexiographical ordered cache list */
  curr = dnsCacheData->cacheList;
  while ((curr != L7_NULLPTR) &&
         (curr != entry))
  {
    prev = curr;
    curr = curr->listNext;
  }
  if (curr == L7_NULLPTR)
  {
    /* we were asked to remove an entry we can't find */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "dnsCacheEntryDelete: received a bad cache entry!\n");
    return;
  }

  if (prev == L7_NULLPTR)
  {
    dnsCacheData->cacheList = curr->listNext;
  } else
  {
    prev->listNext = curr->listNext;
  }

  /* remove from the recently used cache list */
  dnsCacheListEntryRemove(entry);

  return;
}

/*********************************************************************
* @purpose  Add a cache entry and associated data
*
* @param    dnsDomainName_t           *name  @b((input)) internal name format
* @param    dnsClientCacheEntryType_t type   @b((input)) ip or cname
* @param    L7_uint32                 ttl    @b((input)) time to live
* @param    L7_inet_addr_t            server @b((input)) server addr (v4 or v6)
*                                                        data was received from
* @param    void                      *data  @b((input)) ip list or cname
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
                         void                     *data)
{
  dnsCacheEntry_t *entry = dnsCacheData->cacheList;
  dnsCacheEntry_t *prev = L7_NULLPTR;  /* insert after previous */

  /* go through cache list, delete expired entries while we
      are looking, resolve matching entry, find lexiographic order
       to insert the new entry */
  while (entry != L7_NULLPTR)
  {
    L7_int32 result = 0;
    if (dnsCacheEntryExpiredCheck(entry) == L7_TRUE)
    {
      dnsCacheEntry_t *next = entry->listNext;  /* keep next for delete */
      dnsCacheEntryDelete(entry);
      entry = next;
      continue;
    }
    result = dnsNameCompare(name, &entry->hostname);
    if (result == 0)
    {
      /* we could have a collision if we are processing two
         requests for the same hostname at the same time, if the
         first request result was not cached before the second
         request was generated.  Also, cname entries often have
         longer TTLs than IPs, we may delete the IP entry and
         still have a cname entry in the cache, in either case
         keep the entry that expires later, and move it to the front */
      if (entry->type == type)
      {
        if ((entry->timestamp + entry->ttl) >= (osapiUpTimeRaw() + ttl))
        {
          dnsCacheEntryUsed(entry);
          return L7_SUCCESS;
        }
        dnsCacheEntryDelete(entry);
        break;
      }
      else if (entry->type < type )
      {
        prev = entry;  /* insert after */
      }
    }
    if (result > 0)
    {
      prev = entry;  /* insert after */
    }
    entry = entry->listNext;
  }

  /* allocate a cache buffer,
     if there are no buffers then use
     the oldest or least used entry */
  if (bufferPoolAllocate(dnsCachePoolId, (L7_uchar8 **)&entry) != L7_SUCCESS)
  {
    /* clear up the tail cache entry and use it */
    if (dnsCacheData->tail != L7_NULLPTR)
    {
      entry = dnsCacheData->tail;
      dnsCacheEntryClear(entry);
    }
    else
    {
      return L7_FAILURE;
    }
  }
  memset((void *)entry, 0, sizeof(dnsCacheEntry_t));
  memcpy(&entry->queryHostName, qname, sizeof(dnsDomainName_t));
  memcpy(&entry->hostname, name, sizeof(dnsDomainName_t));
  entry->timestamp = osapiUpTimeRaw();
  inetCopy(&entry->server, server);
  entry->type = type;
  entry->ttl = ttl;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if ((type == DNS_RR_TYPE_ADDRESS)||(type == DNS_RR_TYPE_IPV6_ADDRESS))
#else
  if (type == DNS_RR_TYPE_ADDRESS)
#endif
  {
    memcpy(entry->data.inetAddrs, data, (sizeof(L7_inet_addr_t) * DNS_CACHE_ENTRY_IP_ADDRESSES));
  }
  else
  {
    memcpy(&entry->data.cname, data, sizeof(dnsDomainName_t));
  }

  /* add to cache list */
  if (prev == L7_NULLPTR)
  {
    /* add to beginning of list */
    if (dnsCacheData->cacheList != L7_NULLPTR)
    {
      entry->listNext = dnsCacheData->cacheList;
    }
    dnsCacheData->cacheList = entry;
  }
  else
  {
    /* add after previous */
    entry->listNext = prev->listNext;
    prev->listNext = entry;
  }

  /* add to beginning of recently used list */
  dnsCacheListEntryHeadAdd(entry);

  return L7_SUCCESS;
}

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
void dnsCacheEntryDelete(dnsCacheEntry_t *entry)
{
  /* clean up entry pointers */
  dnsCacheEntryClear(entry);

  /* release the cache entry buffer */
  memset(entry, 0, sizeof(dnsCacheEntry_t));
  bufferPoolFree(dnsCachePoolId, (L7_uchar8 *)entry);

  return;
}

/*********************************************************************
* @purpose  Delete all expired cache entries
*
* @param    none
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void dnsCacheExpiredEntriesDelete(void)
{
  dnsCacheEntry_t *entry = dnsCacheData->head;
  dnsCacheEntry_t *next = L7_NULLPTR;

  while (entry != L7_NULLPTR)
  {
    next = entry->next;
    if (dnsCacheEntryExpiredCheck(entry) == L7_TRUE)
    {
      dnsCacheEntryDelete(entry);
    }
    entry = next;
  }
  return;
}

/*********************************************************************
* @purpose  Purge all cache entries
*
* @param    none
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void dnsCacheEntriesPurge(void)
{
  dnsCacheEntry_t *entry = dnsCacheData->head;
  while (entry != L7_NULLPTR)
  {
    dnsCacheEntry_t *next = entry->next;
    dnsCacheEntryDelete(entry);
    entry = next;
  }
  memset(dnsCacheData, 0, sizeof(dnsCacheData_t));
  return;
}

/*********************************************************************
* @purpose  Return first cache entry in lexiographic ordered list.
*
* @param    none
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dnsCacheEntryFirstGet(dnsCacheEntry_t **first)
{
  if (dnsCacheData->cacheList != L7_NULLPTR)
  {
    *first = dnsCacheData->cacheList;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

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
                               L7_inet_addr_t   *inetAddr)
{
  L7_uint32 i = 0;
  L7_int32  j = 0;

  /* if last entry is specified, table is full */
  if (dnsClientIsInetAddressZero(&dnsCfgData->hosts[L7_DNS_STATIC_HOST_ENTRIES-1].inetAddr) != L7_TRUE)
  {
    return L7_TABLE_IS_FULL;
  }

  for (i = 0; i < L7_DNS_STATIC_HOST_ENTRIES-1; i++)
  {
    L7_int32 result = 0;
    if (dnsClientIsInetAddressZero(&dnsCfgData->hosts[i].inetAddr) == L7_TRUE)
    {
      /* insert at end */
      break;
    }
    result = dnsNameCompare(name, &dnsCfgData->hosts[i].hostname);
    if (result == 0)
    {
      if ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->hosts[i].inetAddr)
           != L7_TRUE) &&
          (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->hosts[i].inetAddr,
                                 inetAddr) == L7_TRUE))
      {
        /* entry already exists, do nothing */
        return L7_SUCCESS;
      } else if ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->hosts[i].inetAddr)
           != L7_TRUE) && L7_INET_GET_FAMILY(inetAddr) == L7_INET_GET_FAMILY(&dnsCfgData->hosts[i].inetAddr) )
      {
        /* entry exists with another host Address */
        inetCopy(&dnsCfgData->hosts[i].inetAddr, inetAddr);
        return L7_SUCCESS;
      }
      else if(L7_INET_GET_FAMILY(inetAddr) < L7_INET_GET_FAMILY(&dnsCfgData->hosts[i].inetAddr))
      {
        break;
      }
    }
    if (result < 0)
    {
      break;
    }
  }

  /* need to move up all entries above index */
  for (j = (L7_DNS_STATIC_HOST_ENTRIES - 1); j > i; j--)
  {
    if (dnsClientIsInetAddressZero(&dnsCfgData->hosts[j-1].inetAddr) == L7_TRUE)
    {
      continue;
    }
    memcpy(&dnsCfgData->hosts[j],
           &dnsCfgData->hosts[j-1],
           sizeof(dnsStaticHostEntry_t));
  }
  memcpy(&dnsCfgData->hosts[i].hostname, name, sizeof(dnsDomainName_t));
  inetCopy(&dnsCfgData->hosts[i].inetAddr, inetAddr);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Delete a static hostname mapping.
*
* @param    dnsDomainName_t  *hostname   @b((input)) hostname in internal format
* @param    family                       @b((input)) inet family
*
* @returns  L7_SUCCESS, entry removed
*           L7_FAILURE,
*
* @notes    maintain static hosts in the order configured
*
* @end
*********************************************************************/
L7_RC_t dnsCacheStaticEntryDelete(dnsDomainName_t  *name, L7_uchar8 family)
{
  L7_uint32 i = 0;
  L7_uint32 j = 0;
  L7_BOOL found = L7_FALSE;

  for (i = 0; i < L7_DNS_STATIC_HOST_ENTRIES; i++)
  {
    L7_int32 result = 0;
    if (dnsClientIsInetAddressZero(&dnsCfgData->hosts[i].inetAddr) == L7_TRUE)
    {
      break;
    }
    result = dnsNameCompare(name, &dnsCfgData->hosts[i].hostname);
    if (result == 0 && (family == dnsCfgData->hosts[i].inetAddr.family))
    {
      found = L7_TRUE;
      break;
    }
    if (result < 0)
    {
      break;
    }
  }

  if (found == L7_FALSE)
  {
    return L7_FAILURE;
  }
  memset(&dnsCfgData->hosts[i], 0, sizeof(dnsStaticHostEntry_t));

  /* move entries after removed entry down one */
  for (j=i+1; j < L7_DNS_STATIC_HOST_ENTRIES; j++)
  {
    if (dnsClientIsInetAddressZero(&dnsCfgData->hosts[j].inetAddr) == L7_TRUE)
    {
      break;
    }
    memcpy(&dnsCfgData->hosts[j-1], &dnsCfgData->hosts[j],
           sizeof(dnsStaticHostEntry_t));
    memset(&dnsCfgData->hosts[j], 0, sizeof(dnsStaticHostEntry_t));
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Lookup an entry in the static configuration
*
* @param    L7_uchar8   *nameLabels  @b((input))  internal format
* @param    L7_uchar8   family       @b((input))  inet family
* @param    L7_uint32   *index       @b((output)) configuration index
*
* @returns  L7_SUCCESS, entry found
*           L7_FAILURE, entry not found
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t dnsCacheStaticEntryLookup(dnsDomainName_t *name,
                                         L7_uchar8       family,
                                         L7_uint32       *index)
{
  L7_uint32 i = 0;

  for (i = 0; i < L7_DNS_STATIC_HOST_ENTRIES; i++)
  {
    L7_int32 result = 0;
    if (dnsClientIsInetAddressZero(&dnsCfgData->hosts[i].inetAddr) == L7_TRUE)
    {
      break;
    }
    result = dnsNameCompare(name, &dnsCfgData->hosts[i].hostname);
    if (result == 0 && (family == dnsCfgData->hosts[i].inetAddr.family))
    {
      *index = i;
      return L7_SUCCESS;
    }
    if (result < 0)
    {
      break;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Lookup an entry in the static configuration by ip address
*
* @param    L7_inet_addr_t  inetIp @b((input))  inet ip address
* @param    L7_uint32       *index @b((output)) cashe index
*
* @returns  L7_SUCCESS, entry found
*           L7_FAILURE, entry not found
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t dnsCacheStaticEntryIpLookup(L7_inet_addr_t  inetIp,
                                           L7_uint32 *index)
{
  L7_uint32 i = 0;

  for (i = 0; i < L7_DNS_STATIC_HOST_ENTRIES; i++)
  {
    if (L7_INET_ADDR_COMPARE(&(dnsCfgData->hosts[i].inetAddr),&inetIp) == L7_NULL)
    {
      *index = i;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Lookup an entry in the static configuration
*
* @param    L7_char8  *hostname   @b((input)) hostname
* @param    L7_uchar8   family       @b((input))  inet family
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
                                        L7_uint32 *index)
{
  dnsDomainName_t  name;

  if (dnsStringNameToInternalConvert(hostname, L7_TRUE, &name) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return dnsCacheStaticEntryLookup(&name, family, index);
}

/*********************************************************************
*
* @purpose  Lookup an entry in the static configuration
*
* @param    L7_char8             *hostname    @b((input))  hostname
* @param    L7_uchar8   family       @b((input))  inet family
* @param    dnsStaticHostEntry_t **nextEntry  @b((output)) next static host entry
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
                                            dnsStaticHostEntry_t **nextEntry)
{
  L7_uint32 index = 0;
  if (dnsCacheStaticEntryStringLookup(hostname, family, &index) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if ((++index < L7_DNS_STATIC_HOST_ENTRIES) &&
      (dnsClientIsInetAddressZero(&(dnsCfgData->hosts[index].inetAddr)) != L7_TRUE))
  {
    *nextEntry = &dnsCfgData->hosts[index];
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Lookup an inetAddr in the static configuration
*
* @param    dnsDomainName_t *name @b((input))   name in internal format
* @param    L7_uchar8   family       @b((input))  inet family
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
                               L7_inet_addr_t  *inetAddr)
{
  L7_uint32 index = 0;

  if (dnsCacheStaticEntryLookup(name, family, &index) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  inetCopy(inetAddr, &dnsCfgData->hosts[index].inetAddr);
  return L7_SUCCESS;
}

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
                                 dnsDomainName_t *name)
{
  L7_uint32 index = 0;

  if (dnsCacheStaticEntryIpLookup(inetIp, &index) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  memcpy(name, &(dnsCfgData->hosts[index].hostname), sizeof(dnsDomainName_t));

  return L7_SUCCESS;
}

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
L7_RC_t dnsCacheStaticEntryFirstGet(dnsStaticHostEntry_t **first)
{
  if (dnsClientIsInetAddressZero(&dnsCfgData->hosts[0].inetAddr) != L7_TRUE)
  {
    *first = &dnsCfgData->hosts[0];
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/* DEBUG Routines for Static and Dynamic DNS Cache DUMP */
void DebugDnsCacheDump()
{
  dnsCacheEntry_t *p = L7_NULLPTR;
  L7_char8 strAddr[DNS_DOMAIN_NAME_SIZE_MAX];

   printf ("\r\n Dynamic Cache DUMP");
  p = dnsCacheData->cacheList;
  while (p != L7_NULLPTR)
  {
    int j =0;
     printf ("\r\n   New HOstName %s Type %d Records:  ", (char *)&p->hostname.labels, p->type);
     printf ("\r\n      Index  IP/Cname");
    for (j = 0; j< DNS_CACHE_ENTRY_IP_ADDRESSES; j++)
    {
      if (dnsClientIsInetAddressZero(&p->data.inetAddrs[j]) != L7_TRUE)
      {
        inetAddrHtop(&p->data.inetAddrs[j], strAddr);
        printf ("\r\n %d  %s",j, strAddr);
      }
      else
      {
        printf ("\r\n %d  %s",j, (char *)&p->data.cname.labels);
      }
    }
    p = p->listNext;
  }

  return;
}
void DebugDnsCacheStaticDump()
{
  L7_uint32 i = 0;
  L7_char8 strAddr[DNS_DOMAIN_NAME_SIZE_MAX];

  printf ("\r\n DebugDnsCacheStaticDump ::");
  printf ("\r\n Index    HOSTNAME  IP TYPE ");
  for (i = 0; i < L7_DNS_STATIC_HOST_ENTRIES; i++)
  {
    if (dnsClientIsInetAddressZero(&dnsCfgData->hosts[i].inetAddr) == L7_TRUE)
    {
      continue;
    }
    printf ("\r\n %d  %s               %d ", i, (char *)&dnsCfgData->hosts[i].hostname.labels, dnsCfgData->hosts[i].inetAddr.family);
   inetAddrHtop(&dnsCfgData->hosts[i].inetAddr, strAddr);
   printf ("IP is %s", strAddr);
  }
  return;
}

/* END OF Debug Routines*/
