/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_api.c
*
* @purpose DNS client API functions
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
#include <string.h>
#include "l7_common.h"
#include "osapi_support.h"
#include "dns_client.h"
#include "dns_client_api.h"
#include "dns_client_util.h"
#include "dns_client_cache.h"
#include "dhcp_client_api.h"

extern void               *dnsQueue;
extern dnsCfgData_t       *dnsCfgData;
extern dnsOprData_t       *dnsOprData;
extern dnsNotifyEntry_t   *dnsNotifyTbl;
extern void               *dnsSemaphore;

/* used to implement blocking dnsNameLookup API */
extern void *dnsNameLookupSemaphore;
extern void *dnsNameLookupQueue;

extern void dnsDebugTrace(L7_char8 * format, ...);

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
L7_RC_t dnsClientAdminModeSet(L7_BOOL enable)
{
  if (enable != L7_TRUE &&
      enable != L7_FALSE)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (enable == dnsCfgData->enabled)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_SUCCESS;
  }

  dnsCfgData->enabled = enable;
  dnsCfgData->cfgHdr.dataChanged = L7_TRUE;

  dnsClientAdminModeApply(dnsCfgData->enabled);
  if (enable == L7_TRUE)
  {
    /* need to regenerate this list from the default domain, 
       since it is cleared out if DNS client is disabled. 
       Regeneration is done only if domainList is not configured*/
    if(dnsCfgData->domainList[0][0] == 0) 
    {
      dnsDefaultDomainNameSearchListGenerate();
    }
  }
  osapiSemaGive(dnsSemaphore);

  return L7_SUCCESS;
}

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
L7_RC_t dnsClientAdminModeGet(L7_BOOL *enable)
{
  if (enable == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *enable = dnsCfgData->enabled;
  osapiSemaGive(dnsSemaphore);

  return L7_SUCCESS;
}

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
L7_RC_t dnsClientRequestTimeoutSet(L7_ushort16 timeout)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);

  if (timeout <= dnsCfgData->queryTimeout)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  if (dnsCfgData->requestTimeout != timeout)
  {
    dnsCfgData->requestTimeout = timeout;
    dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientRequestTimeoutGet(L7_ushort16 *timeout)
{
  if (timeout == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *timeout = dnsCfgData->requestTimeout;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientQueryTimeoutSet(L7_ushort16 timeout)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);

  if(timeout > L7_DNS_DOMAIN_TIMEOUT_MAX)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  } 
  if (dnsCfgData->queryTimeout != timeout)
  {
    dnsCfgData->queryTimeout = timeout;
    dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientQueryTimeoutGet(L7_ushort16 *timeout)
{
  if (timeout == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *timeout = dnsCfgData->queryTimeout;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the default domain for the DNS client
*
* @param    L7_char8  domain @b{(input)} default domain for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments set domain to NULL to clear default domain
*           Note that domains > two labels will create a search list,
*           i.e domain xxx.yyy.zzz's search list -> xxx.yyy.zzz, yyy.zzz
*       
* @end
*********************************************************************/
L7_RC_t dnsClientDefaultDomainSet(L7_char8 *domain)
{
  if ((domain != L7_NULLPTR) &&
      (strlen(domain) > DNS_DOMAIN_NAME_SIZE_MAX))
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (domain != L7_NULLPTR)
  {
    if (strcmp(domain, dnsCfgData->domain) == 0)
    {
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
    strcpy(dnsCfgData->domain, domain); 
  } else
  {
    if (strlen(dnsCfgData->domain) == 0)
    {
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
    memset(dnsCfgData->domain, 0, DNS_DOMAIN_NAME_SIZE_MAX);
  }
  /* Search List generation is done only if domainList is not configured*/
  if(dnsCfgData->domainList[0][0] == 0) 
  {
    dnsDefaultDomainNameSearchListGenerate();
  }
  dnsCfgData->cfgHdr.dataChanged = L7_TRUE;  
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientDefaultDomainGet(L7_char8 *domain)
{
  if (domain == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  strcpy(domain, dnsCfgData->domain);
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientDefaultDomainEntryGetNext(L7_char8 *domain)
{
  L7_uint32 i = 0;

  if (domain == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (strlen(domain) == 0)
  {
    if (strlen(dnsOprData->searchTbl[0]) != 0)
    {
      strcpy(domain, dnsOprData->searchTbl[0]);
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
  } else
  {
    for (i = 0; i < L7_DNS_SEARCH_LIST_ENTRIES; i++)
    {
      if (strlen(dnsOprData->searchTbl[i]) == 0)
      {
        break;
      }
      if (strcmp(domain, dnsOprData->searchTbl[i]) == 0)
      {
        i++;
        if (strlen(dnsOprData->searchTbl[i]) != 0)
        {
          strcpy(domain, dnsOprData->searchTbl[i]);
          osapiSemaGive(dnsSemaphore);
          return L7_SUCCESS;
        }
        break;
      }
    }
  }
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Add a name server for the DNS client
*
* @param    L7_inet_addr_t   serverAddr @b{(input)} ipv4 or ipv6
*                                         address of name server.
*
* @returns  L7_SUCCESS, server added or already exists
* @returns  L7_FAILURE, server list contains max entries
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientNameServerEntryAdd(L7_inet_addr_t *serverAddr)
{
  L7_uint32 i = 0;

  if (inetIsAddressZero(serverAddr) == L7_TRUE)
  {
    /* not a valid address */
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->servers[i]) != L7_TRUE)&& 
        (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->servers[i], serverAddr)) == L7_TRUE)
    {
      /* entry already exists */
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
    if (dnsClientIsInetAddressZero(&dnsCfgData->servers[i]) == L7_TRUE)
    {
      break;
    }
  }
  if (i < L7_DNS_NAME_SERVER_ENTRIES)
  {
    inetCopy(&dnsCfgData->servers[i], serverAddr);

    if (dhcpDNSOptionIsPresent(serverAddr) != L7_TRUE)
    { 
      dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
    }
    osapiSemaGive(dnsSemaphore);
    return L7_SUCCESS;
  }
  /* list is full */
  osapiSemaGive(dnsSemaphore);
  return L7_TABLE_IS_FULL;
}

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
L7_RC_t dnsClientNameServerEntryRemove(L7_inet_addr_t *serverAddr)
{
  L7_uint32 i = 0;
  L7_BOOL found = L7_FALSE;

  if (inetIsAddressZero(serverAddr) == L7_TRUE)
  {
    /* not a valid address */
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if (found == L7_TRUE)
    {
      /* move entries after removed entry down one */
      if (dnsClientIsInetAddressZero(&dnsCfgData->servers[i]) != L7_TRUE)
      {
        inetCopy(&dnsCfgData->servers[i-1], &dnsCfgData->servers[i]);
        inetAddressZeroSet(L7_INET_GET_FAMILY(&dnsCfgData->servers[i]),
                           &dnsCfgData->servers[i]);
      }
    }
    else if ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->servers[i]) != 
            L7_TRUE) && (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->servers[i], 
                         serverAddr) == L7_TRUE))
    {
      inetAddressZeroSet(L7_INET_GET_FAMILY(&dnsCfgData->servers[i]),
                           &dnsCfgData->servers[i]);
      found = L7_TRUE;
    }
  }
  if (found == L7_TRUE)
  {
    dnsMgmtMsg_t msg;
    /* we need to take care of any requests waiting on this server */
    msg.msgId = dnsMsgServerRemove;
    inetCopy(&msg.u.address, serverAddr);
    if (osapiMessageSend(dnsQueue, 
                         &msg, 
                         DNS_CLIENT_MSG_SIZE, 
                         L7_NO_WAIT, 
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      LOG_MSG("DNS Client: failed to send server remove message\n");
    }

    dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
    osapiSemaGive(dnsSemaphore);
    return L7_SUCCESS;     
  }
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

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
L7_RC_t dnsClientNameServerEntryGet(L7_inet_addr_t *serverAddr)
{
  L7_uint32 i = 0;

  if (inetIsAddressZero(serverAddr) == L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->servers[i]) != L7_TRUE)&& 
        (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->servers[i], serverAddr) == L7_TRUE))
    {
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
  }
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get name servers for the DNS client
*
* @param    L7_inet_addr_t   *serverAddr @b{(input/output)} ipv4 or 
*                                  ipv6 address of name server
*
* @returns  L7_SUCCESS, serverAddr contains next server address
* @returns  L7_FAILURE, end of list
*
* @comments serverAddr of zero will return first server address
*
*       
* @end
*********************************************************************/
L7_RC_t dnsClientNameServerEntryNextGet(L7_inet_addr_t *serverAddr)
{
  L7_BOOL found = L7_FALSE;
  L7_uint32 i = 0;
  L7_inet_addr_t prev_addr;

  inetAddressReset(&prev_addr);

  if (serverAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if (dnsClientIsInetAddressZero(&dnsCfgData->servers[i]) != L7_TRUE)
    {
      if (L7_INET_ADDR_COMPARE(&dnsCfgData->servers[i], serverAddr) > 0)
      {
        found = L7_TRUE;
        if (dnsClientIsInetAddressZero(&prev_addr) == L7_TRUE)
        {
          inetCopy(&prev_addr, &dnsCfgData->servers[i]);
        }
        else if (L7_INET_ADDR_COMPARE(&dnsCfgData->servers[i],&prev_addr) < 0)
        {
          inetCopy(&prev_addr, &dnsCfgData->servers[i]);
        }
      }
      else
      {
        continue;
      }
    }
    else
    {
      break;
    }
  }

  if (found == L7_TRUE)      
  {
    inetCopy(serverAddr, &prev_addr);
    osapiSemaGive(dnsSemaphore);
    return L7_SUCCESS;
  }
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;

}

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
L7_RC_t dnsClientNameServerUnorderedListEntryGetNext(L7_inet_addr_t *serverAddr)
{

  L7_BOOL found = L7_FALSE;
  L7_uint32 i = 0;

  if (serverAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);

  if(dnsClientIsInetAddressZero(serverAddr) == L7_TRUE)
  {
    if(dnsClientIsInetAddressZero(&dnsCfgData->servers[i]) != L7_TRUE)
    {
      inetCopy(serverAddr, &dnsCfgData->servers[i]);
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
    else
    {
      osapiSemaGive(dnsSemaphore);
      return L7_FAILURE;
    }
  }

  for (i=0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if (dnsClientIsInetAddressZero(&dnsCfgData->servers[i]) != L7_TRUE)
    {
      if (found == L7_TRUE)
      {
        inetCopy(serverAddr, &dnsCfgData->servers[i]);
        osapiSemaGive(dnsSemaphore);
        return L7_SUCCESS;
      }

      if (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->servers[i], serverAddr))
      {
        found = L7_TRUE;
      }
    }
    else
    {
      break;
    }
  }

  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the preference for a name server.
*
* @param    L7_inet_addr_t  serverAddr @b{(input)} ipv4 or ipv6 address of 
*                                          name server
* @param    L7_uint32       preference @b{{output}} server preference
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
                                              L7_uint32 *preference)
{
  L7_uint32 i = 0;

  if (preference == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->servers[i]) != L7_TRUE)&& 
        (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->servers[i], serverAddr) == L7_TRUE))
    {
      *preference = i;
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
  }
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

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
L7_RC_t dnsClientStaticHostEntryAdd(L7_char8 *hostname, L7_inet_addr_t  *inetAddr)
{
  dnsDomainName_t name;
  L7_RC_t         rc = L7_FAILURE;

  if ((hostname == L7_NULLPTR) ||
      (strlen(hostname) == 0) ||
      (inetIsAddressZero(inetAddr) == L7_TRUE))
  {
    return L7_FAILURE;
  }

  /* convert to internal format */
  if (dnsStringNameToInternalConvert(hostname, 
                                     L7_TRUE, 
                                     &name) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  rc = dnsCacheStaticEntryAdd(&name, inetAddr);
  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return rc;
  }
  dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientStaticHostEntryRemove(L7_char8 *hostname, L7_uchar8 family)
{
  dnsDomainName_t name;

  if ((hostname == L7_NULLPTR) ||
      (strlen(hostname) == 0))
  {
    return L7_FAILURE;
  }

  /* convert to internal format */
  if (dnsStringNameToInternalConvert(hostname, 
                                     L7_TRUE, 
                                     &name) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (dnsCacheStaticEntryDelete(&name, family) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                    L7_inet_addr_t  *inetAddr)  
{
  L7_uint32 index = 0;  

  if ((hostname == L7_NULLPTR) ||
      (strlen(hostname) == 0))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if ((dnsCacheStaticEntryStringLookup(hostname, inetAddr->family, &index) != L7_SUCCESS) ||
      ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->hosts[index].inetAddr) 
        != L7_TRUE) && 
       (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->hosts[index].inetAddr, 
                              inetAddr) != L7_TRUE)))
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                        L7_inet_addr_t  *inetAddr)
{
  dnsStaticHostEntry_t *entry;
  L7_uchar8 family;
  
  family = inetAddr->family;

  if ((hostname == L7_NULLPTR) ||
      (inetAddr == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (strlen(hostname) == 0)
  {
    if (dnsCacheStaticEntryFirstGet(&entry) != L7_SUCCESS)
    {
      osapiSemaGive(dnsSemaphore);
      return L7_FAILURE;
    }
  } else {
    if (dnsCacheStaticEntryStringLookupNext(hostname, family, &entry) != L7_SUCCESS)
    {
      osapiSemaGive(dnsSemaphore);
      return L7_FAILURE;
    }
  }
  /* return all lowercase to maintain lexiographic ordering in SNMP table */
  dnsInternalNameToStringConvert(&entry->hostname, L7_FALSE, hostname);
  inetCopy(inetAddr, &entry->inetAddr);
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}
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
                                  L7_inet_addr_t  *inetAddr)
{
  dnsStaticHostEntry_t *entry;
  L7_uint32 index;
  if ((hostname == L7_NULLPTR) ||
      (inetAddr == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (strlen(hostname) == 0)
  {
    if (dnsCacheStaticEntryFirstGet(&entry) != L7_SUCCESS)
    {
      osapiSemaGive(dnsSemaphore);
      return L7_FAILURE;
    }
  }
  else
  {
    if (inetAddr->family == 0)
    {
      if (dnsCacheStaticEntryStringLookup(hostname, L7_AF_INET, &index) != L7_SUCCESS)
      {
        if (dnsCacheStaticEntryStringLookup(hostname, L7_AF_INET6, &index) != L7_SUCCESS)
        {
          osapiSemaGive(dnsSemaphore);
          return L7_FAILURE;
        }
      }
    }
    else
    {
      if (dnsCacheStaticEntryStringLookup(hostname, inetAddr->family, &index) != L7_SUCCESS)
      {
        osapiSemaGive(dnsSemaphore);
        return L7_FAILURE;
      }
    }
  }
   if ((index < L7_DNS_STATIC_HOST_ENTRIES) &&
      (dnsClientIsInetAddressZero(&(dnsCfgData->hosts[index].inetAddr)) != L7_TRUE))
  {
    entry = &dnsCfgData->hosts[index];

  /* return all lowercase to maintain lexiographic ordering in SNMP table */
  dnsInternalNameToStringConvert(&entry->hostname, L7_FALSE, hostname);
  inetCopy(inetAddr, &entry->inetAddr);
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
  }
 
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}




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
                                               L7_inet_addr_t  *inetAddr)
{
  L7_uint32 index = 0;  

  if ((hostname == L7_NULLPTR) ||
      (strlen(hostname) == 0))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if ((dnsCacheStaticEntryStringLookup(hostname, inetAddr->family, &index) != L7_SUCCESS) ||
      ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->hosts[index].inetAddr) != 
        L7_TRUE) && 
       (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->hosts[index].inetAddr, 
                              inetAddr) != L7_TRUE)))
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }
  /* return case specific hostname as entered by user */
  dnsInternalNameToStringConvert(&dnsCfgData->hosts[index].hostname, L7_TRUE, hostname);
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientIdentifierGet(L7_char8 *identifier)
{
  strcpy(identifier, DNS_CLIENT_IMPLEMENT_ID); 
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientServiceGet(L7_uint32 *service)
{
  *service = DNS_CLIENT_SERVICE_RECURSIVE_ONLY;
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientMaxCnamesGet(L7_uint32 *cnames)
{
  *cnames = DNS_CNAME_REFERENCES_MAX;
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientUpTimeGet(L7_uint32 *upTime)
{
  *upTime = osapiUpTimeRaw();
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCacheMaxTTLGet(L7_uint32 *ttl)
{
  *ttl = 0;
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCacheGoodCachesGet(L7_uint32 *caches)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *caches = dnsOprData->counters.goodCaches;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCacheBadCachesGet(L7_uint32 *caches)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *caches = dnsOprData->counters.badCaches;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCacheRRGet(L7_uchar8   *rrName,
                            dnsRRType_t  rrType,
                            L7_uint32    rrIndex,
                            dnsRRClass_t rrClass)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;

  if (rrClass != DNS_RR_CLASS_INTERNET)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);

  if (dnsCacheEntryRRGet(rrName, rrType, rrIndex, &entry) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                L7_uchar8    *qname)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;
  dnsCacheEntry_t *entryByName = L7_NULLPTR;
  dnsRRType_t prev_type = *rrType;
  dnsDomainName_t name;

  if ((rrName == L7_NULLPTR) || 
      (rrType == L7_NULLPTR) ||      
      (rrIndex == L7_NULLPTR) ||
      (rrClass == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);

  if (rrName[0] == 0)
  {
    /* remove any expired entries on the first read */
    dnsCacheExpiredEntriesDelete();
    if (dnsCacheEntryFirstGet(&entry) != L7_SUCCESS)
    {
      osapiSemaGive(dnsSemaphore);
      return L7_FAILURE;
    }
  } else
  {
    if ((*rrClass) <= DNS_RR_CLASS_INTERNET)
    {
      *rrClass = DNS_RR_CLASS_INTERNET;

      if ((*rrType) == 0)
      {
        /* If (*rrType) is 0, return the first cache entry corresponding to this (*rrName) */

        memset(&name, 0, sizeof(dnsDomainName_t));
        memcpy(name.labels, rrName, DNS_NAME_LABELS_SIZE_MAX);

        if(dnsCacheEntryNameOnlyLookup(&name, &entryByName) == L7_SUCCESS)
        {
          (*rrType) = entryByName->type;
          (*rrIndex) = 0;
          memcpy(qname, entryByName->queryHostName.labels, DNS_DOMAIN_NAME_SIZE_MAX);
          memcpy(rrName, entryByName->hostname.labels, DNS_DOMAIN_NAME_SIZE_MAX);
          osapiSemaGive(dnsSemaphore);
          return L7_SUCCESS;
        }
        else
        {
          osapiSemaGive(dnsSemaphore);
          return L7_FAILURE;
        }
      }
      else
      {
        (*rrIndex)++;
      }
      if ((*rrType) <= DNS_RR_TYPE_ADDRESS)
      {
        *rrType = DNS_RR_TYPE_ADDRESS;

        for (; (*rrIndex) < DNS_CACHE_ENTRY_IP_ADDRESSES; (*rrIndex)++)
        {
          if (dnsCacheEntryRRGet(rrName, *rrType, *rrIndex, &entry) == L7_SUCCESS)    
          {
            memcpy(qname, entry->queryHostName.labels, DNS_DOMAIN_NAME_SIZE_MAX);
            osapiSemaGive(dnsSemaphore);
            return L7_SUCCESS;
          }
        }
        *rrType = prev_type;
        *rrIndex = 0;
      }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
      else if ((*rrType) <= DNS_RR_TYPE_IPV6_ADDRESS)
      {
        *rrType = DNS_RR_TYPE_IPV6_ADDRESS;

        for (; (*rrIndex) < DNS_CACHE_ENTRY_IP_ADDRESSES; (*rrIndex)++)
        {
          if (dnsCacheEntryRRGet(rrName, *rrType, *rrIndex, &entry) == L7_SUCCESS)    
          {
            memcpy(qname, entry->queryHostName.labels, DNS_DOMAIN_NAME_SIZE_MAX);
            osapiSemaGive(dnsSemaphore);
            return L7_SUCCESS;
          }
        }
        *rrType = prev_type;
        *rrIndex = 0;
      }
#endif
    }
    /* return next entry in cache */
    if (dnsCacheEntryLookupNext(rrName, *rrType, &entry) != L7_SUCCESS)
    {
      osapiSemaGive(dnsSemaphore);
      return L7_FAILURE;
    }
  }

  memcpy(qname, entry->queryHostName.labels, DNS_DOMAIN_NAME_SIZE_MAX);
  memcpy(rrName, entry->hostname.labels, DNS_DOMAIN_NAME_SIZE_MAX);
  *rrType = entry->type;
  *rrIndex = 0;
  *rrClass = DNS_RR_CLASS_INTERNET;   /* same for all RRs */
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                               L7_uint32    *ttl)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;

  if ((rrName == L7_NULLPTR) ||
      (ttl == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (dnsCacheEntryRRGet(rrName, rrType, rrIndex, &entry) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  *ttl = entry->ttl;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                      L7_uint32    *elapsed)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;

  if ((rrName == L7_NULLPTR) ||
      (elapsed == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (dnsCacheEntryRRGet(rrName, rrType, rrIndex, &entry) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  *elapsed = osapiUpTimeRaw() - entry->timestamp;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                  L7_inet_addr_t   *sourceAddr)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;
  L7_inet_addr_t   networkSrcAddr;

  if ((rrName == L7_NULLPTR) ||
      (sourceAddr == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  inetAddressReset(&networkSrcAddr);

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (dnsCacheEntryRRGet(rrName, rrType, rrIndex, &entry) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  if (inetAddrHton(&entry->server, &networkSrcAddr) != L7_SUCCESS)
  {
    LOG_MSG("\r\nUnable to convert host to network order");
    return L7_FAILURE;
  }
  inetCopy(sourceAddr, &networkSrcAddr);
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                L7_BOOL      *primary)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;

  if ((rrName == L7_NULLPTR) ||
      (data == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (dnsCacheEntryRRGet(rrName, rrType, rrIndex, &entry) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  *primary = L7_FALSE;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if ((entry->type == DNS_RR_TYPE_ADDRESS)||(entry->type == DNS_RR_TYPE_IPV6_ADDRESS))
#else
  if (entry->type == DNS_RR_TYPE_ADDRESS)
#endif
  { 
    /* inet address in network order */
    L7_inet_addr_t inetAddrnl; 

    inetAddressReset(&inetAddrnl);

    inetAddrHton(&entry->data.inetAddrs[rrIndex], &inetAddrnl);

    memcpy(data, &inetAddrnl, sizeof(L7_inet_addr_t));
    if (rrIndex == 0)
    {
      /* we always return the first ip in a name lookup */
      *primary = L7_TRUE;
    }
  } else
  { 
    /* cname as received in packet */
    dnsInternalNameToCaseConvert(&entry->data.cname, data);
  }

  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                      L7_uchar8    *prettyName)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;

  if ((rrName == L7_NULLPTR) ||
      (prettyName == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (dnsCacheEntryRRGet(rrName, rrType, rrIndex, &entry) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  dnsInternalNameToCaseConvert(&entry->hostname, prettyName); 
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                L7_char8     *hostname)
{
  dnsDomainName_t name;

  dnsCaseNameToInternalConvert(dnsName, &name); 
  dnsInternalNameToStringConvert(&name, L7_TRUE, hostname);

  /* remove trailing '.' if it exists */
  if (hostname[strlen(hostname) - 1] == '.')
  {
    hostname[strlen(hostname) - 1] = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Flush cache values for a specific hostname
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
L7_RC_t dnsClientCacheHostFlush(L7_char8  *hostname)
{
  dnsCacheEntry_t *entry = L7_NULLPTR;
  L7_BOOL flag = L7_FALSE;

  if ((hostname == L7_NULLPTR) ||
      (strlen(hostname) == 0))
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if (dnsCacheEntryStringLookup(hostname, DNS_RR_TYPE_ADDRESS, &entry) == L7_SUCCESS)    
  {
     dnsCacheEntryDelete(entry); 
     flag = L7_TRUE;
  }
  
  if (dnsCacheEntryStringLookup(hostname, DNS_RR_TYPE_CNAME, &entry) == L7_SUCCESS)
  {
    dnsCacheEntryDelete(entry);    
    flag = L7_TRUE;
  }
  
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if (dnsCacheEntryStringLookup(hostname, DNS_RR_TYPE_IPV6_ADDRESS, &entry) == L7_SUCCESS)
  {
    dnsCacheEntryDelete(entry);    
    flag = L7_TRUE;
  }
#endif

  if(flag == L7_FALSE)
  {  
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }
  else 
  {  
    osapiSemaGive(dnsSemaphore);
    return L7_SUCCESS;
  }
}

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
L7_RC_t dnsClientCacheFlush(void)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  dnsCacheEntriesPurge();
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                           L7_uint32    *queries)
{
  if (opcode != DNS_OPCODE_STANDARD_QUERY)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *queries = dnsOprData->counters.v4queries;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                             L7_uint32    *responses)
{
  if (opcode != DNS_OPCODE_STANDARD_QUERY)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *responses = dnsOprData->counters.v4responses;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCounterByRcodeResponsesGet(dnsOpcode_t   rcode,
                                            L7_uint32    *responses)
{
  if (rcode >= DNS_RCODE_RESERVED)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *responses = dnsOprData->counters.rcodes[rcode];
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCounterNonAuthDataResponsesGet(L7_uint32 *responses)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *responses = dnsOprData->counters.nonAA;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCounterNonAuthNoDataResponsesGet(L7_uint32 *responses)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *responses = dnsOprData->counters.nonAANoData;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCounterMartiansGet(L7_uint32 *martians)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *martians = dnsOprData->counters.martians;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientCounterUnparsedResponsesGet(L7_uint32 *unparsed)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *unparsed = dnsOprData->counters.unparsed;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
                                                              L7_inet_addr_t   *inetAddr))
{
  if (componentId >= L7_LAST_COMPONENT_ID)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  dnsNotifyTbl[componentId].notifyFunction = notify;
  osapiSemaGive(dnsSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Deregister the routine to be called when a name lookup completes.
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
L7_RC_t dnsClientNameLookupResponseDeregister(L7_COMPONENT_IDS_t componentId)
{
  if ((componentId >= L7_LAST_COMPONENT_ID) ||
      (componentId == L7_DNS_CLIENT_COMPONENT_ID))  /* we use our own componentId */
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  dnsNotifyTbl[componentId].notifyFunction = L7_NULLPTR;
  osapiSemaGive(dnsSemaphore);

  return L7_SUCCESS;
}

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
                                   L7_char8          *hostname)
{
  dnsRequestEntry_t *request;
  dnsMgmtMsg_t       msg;

  if ((hostname == L7_NULLPTR) ||
      (strlen(hostname) < 1) ||
      (componentId > L7_LAST_COMPONENT_ID))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if ((dnsCfgData->enabled != L7_TRUE) ||
      (dnsNotifyTbl[componentId].notifyFunction == L7_NULLPTR))
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  if (dnsRequestGet(family, componentId, requestId, hostname, &request) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  /* request processing takes place on the client task */
  msg.msgId = dnsMsgRequest;
  msg.u.request = request;
  if (osapiMessageSend(dnsQueue, 
                       &msg, 
                       DNS_CLIENT_MSG_SIZE, 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}
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
                                          L7_inet_addr_t     inetIp)
{
  dnsRequestEntry_t *request;
  dnsMgmtMsg_t       msg;

  if (componentId > L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("[DNS client] %s (%d) invalid component ID %d \n",__FUNCTION__, __LINE__, componentId);
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if ((dnsCfgData->enabled != L7_TRUE) ||
      (dnsNotifyTbl[componentId].notifyFunction == L7_NULLPTR))
  {
    LOG_MSG("[DNS client] %s (%d) component is disabled \n",__FUNCTION__, __LINE__);
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  if (dnsReverseRequestGet(componentId, requestId, inetIp, &request) != L7_SUCCESS)
  {
    LOG_MSG("[DNS client] %s (%d) failed to generate request \n",__FUNCTION__, __LINE__);
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  /* request processing takes place on the client task */
  msg.msgId = dnsMsgReverseRequest;
  msg.u.request = request;
  if (osapiMessageSend(dnsQueue, 
                       &msg, 
                       DNS_CLIENT_MSG_SIZE, 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("[DNS client] %s (%d) failed to send request \n",__FUNCTION__, __LINE__);
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
* @comments hostFQDN buffer must be DNS_DOMAIN_NAME_SIZE_MAX.
*           This functions can be used in ipv4 only applications.
*
* @end
*
*********************************************************************/
L7_RC_t dnsClientNameLookup(L7_char8                *hostname,
                            dnsClientLookupStatus_t *status,
                            L7_char8                *hostFQDN,
                            L7_uint32               *ip)
{
  L7_inet_addr_t resolvedAddr;
  L7_RC_t rc = L7_FAILURE;

  inetAddressZeroSet(L7_AF_INET, &resolvedAddr);
  rc = dnsClientInetNameLookup(L7_AF_INET, hostname, status, hostFQDN, &resolvedAddr);
  inetAddressGet(L7_AF_INET, &resolvedAddr, ip);
  return rc;
}


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
                                L7_inet_addr_t          *inetAddr)
{
  dnsNameLookupMsg_t msg;

  if ((hostname == L7_NULLPTR) ||
      (strlen(hostname) == 0) ||
      (status == L7_NULLPTR) ||
      (hostFQDN == L7_NULLPTR) ||
      (inetAddr == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
#if !defined(L7_IPV6_PACKAGE) && !defined(L7_IPV6_MGMT_PACKAGE)
  if (family == DNS_AF_BOTH)
  {
    /* When IPv6 package is not available, A query (IPv4) only should be 
     * sent for non protocol specific applicaitons 
     */
    family = L7_AF_INET;
  }
#endif
  if (family == DNS_AF_BOTH)
  {
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    osapiSemaTake(dnsNameLookupSemaphore, L7_WAIT_FOREVER);
    if (dnsClientNameLookupRequest(L7_AF_INET6, L7_DNS_CLIENT_COMPONENT_ID, 0, hostname) != L7_SUCCESS)
    {
      *status = DNS_LOOKUP_STATUS_FAILURE;
      osapiSemaGive(dnsNameLookupSemaphore);
      return L7_FAILURE;
    }
    if (osapiMessageReceive(dnsNameLookupQueue, 
                            (void *)&msg, 
                            DNS_NAME_LOOKUP_MSG_SIZE, 
                            L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      *status = DNS_LOOKUP_STATUS_FAILURE;
      osapiSemaGive(dnsNameLookupSemaphore);
      return L7_FAILURE;
    }
    
    if (msg.recordType == DNS_RR_TYPE_IPV6_ADDRESS)  
    {
      if (msg.status == DNS_LOOKUP_STATUS_SUCCESS)
      {
        inetCopy(&inetAddr[1], &msg.inetAddr);
      }
      if (dnsClientNameLookupRequest(L7_AF_INET, L7_DNS_CLIENT_COMPONENT_ID, 0, hostname) != L7_SUCCESS)
      {
        *status = DNS_LOOKUP_STATUS_FAILURE;
        osapiSemaGive(dnsNameLookupSemaphore);
        return L7_FAILURE;
      }

      if (osapiMessageReceive(dnsNameLookupQueue, 
                              (void *)&msg, 
                              DNS_NAME_LOOKUP_MSG_SIZE, 
                              L7_WAIT_FOREVER) != L7_SUCCESS)
      {
        *status = DNS_LOOKUP_STATUS_FAILURE;
        osapiSemaGive(dnsNameLookupSemaphore);
        return L7_FAILURE;
      }
      if (msg.recordType == DNS_RR_TYPE_ADDRESS)
      {
        *status = msg.status;
        if (msg.status == DNS_LOOKUP_STATUS_SUCCESS)
        {
          strcpy(hostFQDN, msg.hostFQDN);
          inetCopy(&inetAddr[0], &msg.inetAddr);
          osapiSemaGive(dnsNameLookupSemaphore);
          return L7_SUCCESS;
        }
      }
    }
#endif
  }
  else
  {
    osapiSemaTake(dnsNameLookupSemaphore, L7_WAIT_FOREVER);
    if (dnsClientNameLookupRequest(family, L7_DNS_CLIENT_COMPONENT_ID, 0, hostname) != L7_SUCCESS)
    {
      *status = DNS_LOOKUP_STATUS_FAILURE;
      osapiSemaGive(dnsNameLookupSemaphore);
      return L7_FAILURE;
    }

    if (osapiMessageReceive(dnsNameLookupQueue, 
                            (void *)&msg, 
                            DNS_NAME_LOOKUP_MSG_SIZE, 
                            L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      *status = DNS_LOOKUP_STATUS_FAILURE;
      osapiSemaGive(dnsNameLookupSemaphore);
      return L7_FAILURE;
    }

    *status = msg.status;
    if (msg.status == DNS_LOOKUP_STATUS_SUCCESS)
    {
      strcpy(hostFQDN, msg.hostFQDN);
      inetCopy(inetAddr, &msg.inetAddr);
      osapiSemaGive(dnsNameLookupSemaphore);
      return L7_SUCCESS;
    }
  }
  *hostFQDN = '\0';
  inetAddressZeroSet(family, inetAddr);
  osapiSemaGive(dnsNameLookupSemaphore);
  return L7_FAILURE;
}

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
                                   L7_char8                *hostFQDN)
{
  dnsNameLookupMsg_t msg;

  if ((hostname == L7_NULLPTR) ||
      (status == L7_NULLPTR)   ||
      (hostFQDN == L7_NULLPTR))
  {
    dnsDebugTrace("[DNS client] %s (%d) invalid input params\n",__FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  osapiSemaTake(dnsNameLookupSemaphore, L7_WAIT_FOREVER);

  if (dnsClientReverseNameLookupRequest(L7_DNS_CLIENT_COMPONENT_ID, 0, inetIp) != L7_SUCCESS)
  {
    dnsDebugTrace("[DNS client] %s (%d) failed to send request \n",__FUNCTION__, __LINE__);
    *status = DNS_LOOKUP_STATUS_FAILURE;
    osapiSemaGive(dnsNameLookupSemaphore);
    return L7_FAILURE;
  }

  if (osapiMessageReceive(dnsNameLookupQueue, 
                          (void *)&msg, 
                          DNS_NAME_LOOKUP_MSG_SIZE, 
                          L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    dnsDebugTrace("[DNS client] %s (%d) failed to get response \n",__FUNCTION__, __LINE__);
    *status = DNS_LOOKUP_STATUS_FAILURE;
    osapiSemaGive(dnsNameLookupSemaphore);
    return L7_FAILURE;
  }

  *status = msg.status;
  if (msg.status == DNS_LOOKUP_STATUS_SUCCESS)
  {
    L7_char8 *end    = strchr(msg.hostFQDN, '.');
    L7_uint8  length = end - msg.hostFQDN;
    
    osapiStrncpy(hostname, msg.hostFQDN, length);
    osapiStrncpy(hostFQDN, msg.hostFQDN, DNS_DOMAIN_NAME_SIZE_MAX);
    osapiSemaGive(dnsNameLookupSemaphore);
    return L7_SUCCESS;
  }

  dnsDebugTrace("[DNS client] %s (%d) response status %d \n",__FUNCTION__, __LINE__, msg.status);
  *hostFQDN = '\0';
  *hostname = '\0';
  osapiSemaGive(dnsNameLookupSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set number of DNS request retransmits
*
* @param    L7_uint32  retries  @b{(input)} number of retries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dnsClientRequestRetransmitsSet(L7_uint32 retries)
{
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);

  if (retries < L7_DNS_DOMAIN_RETRY_NUMBER_MIN || 
      retries > L7_DNS_DOMAIN_RETRY_NUMBER_MAX)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }

  if (dnsCfgData->retries != retries)
  {
    dnsCfgData->retries = retries;
    dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get number of DNS request retransmits
*
* @param     L7_uint32    *retries  @b{(output)}   retries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dnsClientRequestRetransmitsGet(L7_uint32 *retries)
{
  if (retries == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  *retries = dnsCfgData->retries;
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

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
L7_RC_t dnsClientDomainNameListAdd(L7_char8 *domainname)
{
  L7_uint32 i = 0;

  if (domainname == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES; i++)
  {
    if ((strcmp(dnsCfgData->domainList[i], domainname) == 0))
    {
      /* entry already exists */
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
    if (dnsCfgData->domainList[i][0] == 0)
    {
      break;
    }
  }
  if (i < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES)
  {
    strcpy(dnsCfgData->domainList[i], domainname);
    if (dhcpDomainNameOptionIsPresent(domainname)!= L7_TRUE)
    {
      dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
    }
    if( i == 0)
    {
      /* This is done so if any configured search list from default
         domain list is set to Null in the oprData structure*/
      memset(dnsOprData->searchTbl, 0,
         L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES * DNS_DOMAIN_NAME_SIZE_MAX);
    }
    dnsAddDomainNameToSearchList(domainname);
    osapiSemaGive(dnsSemaphore);
    return L7_SUCCESS;
  }
  /* list is full */
  osapiSemaGive(dnsSemaphore);
  return L7_TABLE_IS_FULL;
}

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
L7_RC_t dnsClientDomainNameListRemove(L7_char8 *domainname)
{
  L7_uint32 i = 0;
  L7_BOOL found = L7_FALSE;

  if (domainname == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES; i++)
  {
    if (found == L7_TRUE)
    {
      /* move entries after removed entry down one */
      if (dnsCfgData->domainList[i][0] != 0)
      {
        strcpy(dnsCfgData->domainList[i-1], dnsCfgData->domainList[i]);
        memset(dnsCfgData->domainList[i], '\0', DNS_DOMAIN_NAME_SIZE_MAX);
      }
    }
    else if ((strcmp(dnsCfgData->domainList[i], domainname) == 0))
    {
      memset(dnsCfgData->domainList[i], '\0', DNS_DOMAIN_NAME_SIZE_MAX);
      found = L7_TRUE;
    }
  }
  if(found == L7_TRUE)
  {
    dnsCfgData->cfgHdr.dataChanged = L7_TRUE;
    dnsRemoveDomainNameFromSearchList(domainname);
    osapiSemaGive(dnsSemaphore);
    return L7_SUCCESS;
  }
  /* domain name not found in the list */
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

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
L7_RC_t dnsClientDomainNameListGet(L7_char8 *domainname)
{
  L7_uint32 i = 0;

  if (domainname == L7_NULLPTR || domainname[0] == 0)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  if(dnsCfgData->domainList[i][0] == 0)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }
  for (i=0; i < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES; i++)
  {
    if ((strcmp(dnsCfgData->domainList[i], domainname) == 0))
    {
      osapiSemaGive(dnsSemaphore);
      strcpy(domainname, dnsCfgData->domainList[i]); 
      return L7_SUCCESS;
    }
  }
  /* domain name not found in the list */
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get Next domain search list entries for the DNS client
*
* @param    L7_char8  *domainName  @b{(input/output)} domain entry for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The search list is returned in lexicographic order
*           empty domain string will return first entry.
*
* @end
*********************************************************************/
L7_RC_t dnsClientDomainNameListEntryGetNext(L7_char8 *domainName)
{
  L7_BOOL found = L7_FALSE;
  L7_uint32 i = 0;
  L7_char8 prev_domain[255];

  memset(prev_domain, '\0', 255);

  if (domainName == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i=0; i < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES; i++)
  {
    if (dnsCfgData->domainList[i][0] != 0)
    {
      if (strcmp(dnsCfgData->domainList[i], domainName) > 0)
      {
        found = L7_TRUE;
        if(prev_domain[0] == 0)
        {
          strcpy(prev_domain, dnsCfgData->domainList[i]);
        }
        else if(strcmp(dnsCfgData->domainList[i], prev_domain) < 0)
        {
          strcpy(prev_domain, dnsCfgData->domainList[i]);
        }
      }
      else
      {
        continue;
      }
    }
    else
    {
      break;
    }
  }

  if (found == L7_TRUE)      
  {
    strcpy(domainName, prev_domain);
    osapiSemaGive(dnsSemaphore);
    return L7_SUCCESS;
  }
  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get Next domain search list entries for the DNS client 
*           in the way the ordered list is entered 
*
* @param    L7_char8  *domainName  @b{(input/output)} domain entry for unqualified lookups
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The search list is NOT returned in lexicographic order
*           empty domain string will return first entry.
*
* @end
*********************************************************************/
L7_RC_t dnsClientDomainNameUnorderedListEntryGetNext(L7_char8 *domainName)
{
  L7_BOOL found = L7_FALSE;
  L7_uint32 i = 0;

  if (domainName == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);

  if(domainName[0] == 0)
  {
    if(dnsCfgData->domainList[i][0] != 0)
    {
      strcpy(domainName, dnsCfgData->domainList[i]);
      osapiSemaGive(dnsSemaphore);
      return L7_SUCCESS;
    }
    else
    {
      osapiSemaGive(dnsSemaphore);
      return L7_FAILURE;
    }
  }

  for (i=0; i < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES; i++)
  {
    if (dnsCfgData->domainList[i][0] != 0)
    {
      if(found == L7_TRUE)
      {
        strcpy(domainName, dnsCfgData->domainList[i]);
        osapiSemaGive(dnsSemaphore);
        return L7_SUCCESS;
      }
      if (strcmp(dnsCfgData->domainList[i], domainName) == 0)
      {
        found = L7_TRUE;
      }
    }
    else
    {
      break;
    }
  }

  osapiSemaGive(dnsSemaphore);
  return L7_FAILURE;
}

