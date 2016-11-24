/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_util.c
*
* @purpose DNS client utility functions
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
#include <ctype.h>
#include "l7_common.h"
#include "osapi_support.h"
#include "dns_client_api.h"
#include "dns_client.h"
#include "dns_client_util.h"
#include "dns_client_cache.h"
#include "dns_client_packet.h"
#include "dns_client_txrx.h"

extern dnsCnfgrState_t         dnsCnfgrState;
extern dnsCfgData_t           *dnsCfgData;
extern dnsOprData_t           *dnsOprData;
extern dnsNotifyEntry_t       *dnsNotifyTbl;
extern void                   *dnsQueue;
extern void                   *dnsNameLookupQueue; /* used for blocking API only */
extern void                   *dnsSemaphore;

/* unique request id to match responses, this may wrap
   but its very unlikely we would have a response delayed
   enough to match to the next request with the same id */
static L7_ushort16             dnsRequestId = 1;

/* use one timer that wakes up every second, maintain a
   a reference count and delete the timer when we have no pending requests */
static osapiTimerDescr_t *dnsTimer       = L7_NULLPTR;
static L7_uint32          dnsTimerCount  = 0;  /* reference count for dnsTimer */

/* macros for case bit assignment and mask */
#define CASE_INDEX(index) (((index) < 8) ? 0 : ((index) / 8))
#define CASE_MASK(index)  (0x80 >> ((index) % 8)) 

extern void dnsDebugTrace(L7_char8 * format, ...);

/*********************************************************************
*
* @purpose  Maintain one timer for the component
*
* @returns  L7_SUCCESS, 
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static void dnsTimerAdd(void)
{

  if (dnsTimerCount == 0)
  {
    osapiTimerAdd((void *)dnsTimerCallback, L7_NULL, L7_NULL, 
                  DNS_TIMER_INTERVAL, &dnsTimer);
  }
  dnsTimerCount++;

  return;
}

/*********************************************************************
*
* @purpose  Maintain one timer for the component
*
* @returns  L7_SUCCESS, 
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static void dnsTimerDelete(void)
{

  dnsTimerCount--;
  if (dnsTimerCount == 0)
  {
    osapiTimerFree(dnsTimer);
    dnsTimer = L7_NULLPTR;
  }

  return;
}

/*********************************************************************
* @purpose  Clean up request buffer
*
* @param    dnsRequestEntry_t      *request     @b((input))  request state
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
static void dnsRequestDelete(dnsRequestEntry_t *request)
{
  if (request->requestTimer != 0)
  {
    dnsTimerDelete();
  }
  if (request->queryTimer != 0)
  {
    dnsTimerDelete();
  }
  memset(request, 0, sizeof(dnsRequestEntry_t));
  return;
}

/*********************************************************************
* @purpose  Send failure status back to requesting component
*
* @param    dnsRequestEntry_t      *request     @b((input))  request state
* @param    dnsClientLookupStatus_t status      @b((input))  failure status
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
static void dnsNameLookupFailureSend(dnsRequestEntry_t      *request, 
                                     dnsClientLookupStatus_t status)
{
  L7_inet_addr_t inetAddr;

  inetAddressZeroSet(L7_AF_INET, &inetAddr);

  if (dnsNotifyTbl[request->componentId].notifyFunction != L7_NULLPTR)
  {
    dnsNotifyTbl[request->componentId].notifyFunction(status, 
                                                      request->requestId, 
                                                      request->recordType,
                                                      L7_NULLPTR, 
                                                      &inetAddr);
  } else
  {
    LOG_MSG("DNS: Failed to send response to component %i, not registered", 
            request->componentId);
  }

  /* clean up request */
  dnsRequestDelete(request);
  return;
}

/*********************************************************************
* @purpose  Send successful response back to requesting component
*
* @param    dnsRequestEntry_t      *request     @b((input))  request state
* @param    L7_inet_addr_t         *inetAddr    @b((input))  inet address
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
static void dnsNameLookupResponseSend(dnsRequestEntry_t      *request, 
                                      L7_inet_addr_t         *inetAddr)
{
  /* send back the correct qualified hostname */
  L7_char8 hostname[DNS_DOMAIN_NAME_SIZE_MAX];

  if (dnsInternalNameToStringConvert(&request->nameList[request->nameIndex],
                                     L7_TRUE,
                                     hostname) != L7_SUCCESS)
  {
    /* should not ever happen */
    dnsNameLookupFailureSend(request, DNS_LOOKUP_STATUS_FAILURE);
    return;
  }
  if (dnsNotifyTbl[request->componentId].notifyFunction != L7_NULLPTR)
  {
    dnsNotifyTbl[request->componentId].notifyFunction(DNS_LOOKUP_STATUS_SUCCESS, 
                                                      request->requestId, 
                                                      request->recordType,
                                                      hostname, 
                                                      inetAddr);
  } else
  {
    LOG_MSG("DNS: Failed to send response to component %i, not registered", 
            request->componentId);
  }

  /* clean up request */
  dnsRequestDelete(request);
  return;
}

/*********************************************************************
* @purpose  Send successful response back to requesting component
*
* @param    dnsRequestEntry_t      *request     @b((input))  request state
* @param    L7_char8               *hostname    @b{{input}}  hostname
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
static void dnsReverseNameLookupResponseSend(dnsRequestEntry_t *request, 
                                             L7_char8          *hostname)
{
  if (dnsNotifyTbl[request->componentId].notifyFunction != L7_NULLPTR)
  {
    dnsNotifyTbl[request->componentId].notifyFunction(DNS_LOOKUP_STATUS_SUCCESS, 
                                                      request->requestId, 
                                                      request->recordType,
                                                      hostname, 
                                                      &request->inetIpAddr);
  } else
  {
    LOG_MSG("DNS: Failed to send response to component %i, not registered", 
            request->componentId);
  }

  /* clean up request */
  dnsRequestDelete(request);
  return;
}

/*********************************************************************
* @purpose  Send our own internal response message, this is used
*           to implement the blocking name lookup API.
*
* @param dnsClientLookupStatus_t  status    @b{{output}} specific L7_FAILURE status
* @param L7_ushort16              requestId @b{{input}}  name lookup request id
* @param L7_ucahr8                recordType @b{{input}} recordType
* @param L7_char8                *hostFQDN  @b{{output}} fully qualified hostname
* @param L7_inet_addr_t          *inetAddr  @b{(input)}  ipv4 or ipv6 address
*                                                        passed to the application.
* @param L7_uint32                ip        @b{{output}} ip address
*
* @returns  void  
*
* @notes   we register this notify function in cnfgr phase 1, using
*          the L7_DNS_CLIENT_COMPONENT_ID. 
*       
* @end
*********************************************************************/
L7_RC_t dnsNameLookupResponseMessageSend(dnsClientLookupStatus_t status, 
                                         L7_ushort16             requestId,
                                         L7_uchar8               recordType,
                                         L7_char8               *hostname,
                                         L7_inet_addr_t         *inetAddr)
{
  dnsNameLookupMsg_t msg;

  msg.msgId = 0;
  msg.status = status;
  msg.recordType = recordType;
  inetCopy (&msg.inetAddr, inetAddr);
  if (status == DNS_LOOKUP_STATUS_SUCCESS)
  {
    osapiStrncpy(msg.hostFQDN, hostname, DNS_DOMAIN_NAME_SIZE_MAX);
  }
  if (osapiMessageSend(dnsNameLookupQueue, 
                       &msg, 
                       DNS_NAME_LOOKUP_MSG_SIZE, 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("dnsNameLookupResponseMessageSend(): message send failed!\n");
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Applies the DNS client global admin mode
*
* @param    L7_BOOL  enable   @b((input)) enable DNS client services
*
* @returns  L7_SUCCESS
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t dnsClientAdminModeApply(L7_BOOL enable)
{
  /* nothing to do for enable, that just means we'll
     start taking requests */

  /* if disable, clean up all the operational data */
  if (enable == L7_FALSE)
  {
    L7_uint32 i = 0;

    /* send failures for any pending requests */
    for (i = 0; i < L7_DNS_CONCURRENT_REQUESTS; i++)
    {
      if (dnsOprData->requestTbl[i].id != 0)
      {
        dnsNameLookupFailureSend(&dnsOprData->requestTbl[i], 
                                 DNS_LOOKUP_STATUS_DISABLED); 
      }
    }
    /* close any open server sockets */
    for (i = 0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
    {
      if (dnsOprData->serverTbl[i].requests > 0)
      {
        dnsNameServerClose((dnsNameServerEntry_t *)&dnsOprData->serverTbl[i]);
      }
    }
    /* clear the dynamic cache */
    dnsCacheEntriesPurge();
    memset(dnsOprData, 0, sizeof(dnsOprData_t));   
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  compare domain name labels in internal format
*
* @param    L7_uchar8       *labels1    @b((input))   internal label format
* @param    L7_uchar8       *labels2    @b((input))   internal label format
*
* @returns  strcmp result on lowercase domain names
*
* @notes   
*       
* @end
*********************************************************************/
L7_int32 dnsNameCompare(dnsDomainName_t *name1, 
                        dnsDomainName_t *name2)
{
  L7_char8 hostname1[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  L7_char8 hostname2[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];

  /* unfortunately there is no strcasecmp in VxWorks */
  (void)dnsInternalNameToStringConvert(name1, L7_FALSE, hostname1);
  (void)dnsInternalNameToStringConvert(name2, L7_FALSE, hostname2);

  return strcmp(hostname1, hostname2);
}

/*********************************************************************
* @purpose  compare domain name labels in internal format
*
* @param    L7_uchar8       *labels1    @b((input))   internal label format
* @param    L7_uchar8       *labels2    @b((input))   internal label format
*
* @returns  L7_SUCCESS, names match
* @returns  L7_FAILURE, names different
*
* @notes  instead of a memcmp on the maximum length,
*          most of our names will be much < 255.   
*       
* @end
*********************************************************************/
static L7_RC_t dnsNameLabelsEqual(L7_uchar8 *labels1, 
                                  L7_uchar8 *labels2)
{
  L7_uchar8 *p1 = labels1;
  L7_uchar8 *p2 = labels2;

  while (((*p1) != 0) && ((*p2) != 0) && ((*p1) == (*p2)))
  {
    p1++;
    p2++;
  }
  if (((*p1) != 0) || ((*p2) != 0))
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert domain name internal "binary" format to string
*
* @param    dnsDomainName_t  *name      @b((input))   internal "binary" format
* @param    L7_BOOL           uppercase @b((input))   do uppercase conversion
* @param    L7_char8         *strName   @b((output))  dotted string format
*
* @returns  
*
* @notes
*       
* @end
*********************************************************************/
L7_RC_t dnsInternalNameToStringConvert(dnsDomainName_t  *name,
                                       L7_BOOL           uppercase,
                                       L7_char8         *strName)
{
  L7_uint32 srcIndex = 0;
  L7_uint32 dstIndex = 0;

  if ((strName == L7_NULLPTR) ||
      (name == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  memset(strName, 0, (DNS_DOMAIN_NAME_SIZE_MAX + 1));
  while (srcIndex < (DNS_NAME_LABELS_SIZE_MAX + 1))
  {
    L7_uint32 i = 0;
    L7_uchar8 length = name->labels[srcIndex++];
    if (length > (DNS_NAME_LABEL_SIZE_MAX + 1))
    {
      return L7_FAILURE;
    }
    if (length == 0)
    {
      /* root node, we are done */
      break;
    }
    if (srcIndex != 1)
    {
     /* If this is not first label or if you have other following labels  */
      strName[dstIndex++] = '.';
    }

    for (i = 0; i < length; i++)
    {
      if ((uppercase == L7_TRUE) &&
          ((name->uppercase[CASE_INDEX(srcIndex+i)] & ((L7_uchar8)CASE_MASK(srcIndex+i))) != 0))
      {
        strName[dstIndex++] = toupper(name->labels[srcIndex+i]);        
      } else
      {
        strName[dstIndex++] = name->labels[srcIndex+i];
      }
    }
    srcIndex += length;
  }
  strName[dstIndex] = '\0';

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert string to internal "binary" domain name format
*
* @param    L7_char8        *strName     @b((input))  dotted string format
* @param    L7_BOOL         lowercase    @b((input))  do lowercase conversion       
* @param    dnsDomainName_t *name        @b((output)) internal "binary" format
*
* @returns  
*
* @notes   we always store names in lowercase format for lookups,
*          but we send queries out using the input string.  
*       
* @end
*********************************************************************/
L7_RC_t dnsStringNameToInternalConvert(L7_char8        *strName,
                                       L7_BOOL          lowercase,
                                       dnsDomainName_t *name)
{
  L7_char8 *begin = L7_NULLPTR;
  L7_char8 *strEnd = L7_NULLPTR;
  L7_uint32 dstIndex = 0;
  L7_uint32 strLength = 0;

  if ((strName == L7_NULLPTR) ||
      (name == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  strLength = strlen(strName);
  if ((strLength < 1) ||
      (strLength > DNS_DOMAIN_NAME_SIZE_MAX + 1))
  {
    return L7_FAILURE;
  }

  memset(name, 0, sizeof(dnsDomainName_t));

  begin = strName;
  strEnd = strName + strLength;

  while (begin < strEnd)         
  {
    L7_uint32 i = 0;
    L7_uchar8 length = 0;
    L7_char8 *end = strchr(begin, '.'); 
    if (end == L7_NULLPTR) /* no dot left, convert last node */
    {
      end = strEnd;
    }
    length = end - begin;
    if (length > DNS_NAME_LABEL_SIZE_MAX)
    {
      return L7_FAILURE;
    }
    name->labels[dstIndex++] = length;
    for (i = 0; i < length; i++)
    {
      if ((lowercase == L7_FALSE) || (islower((int)*(begin+i)) != 0))
      {
        name->labels[dstIndex++] = *(begin+i);
      } else
      {
        name->uppercase[CASE_INDEX(dstIndex)] |= CASE_MASK(dstIndex);
        name->labels[dstIndex++] = tolower(*(begin+i)); 
      }
    }
    begin += length + 1;  /* skip past '.' or past strEnd */
  }
  name->labels[dstIndex] = 0;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert internal binary name to case specific label format
*
* @param    dnsDomainName_t *name     @b((input))   internal "binary" format
* @param    L7_uchar8       *labels   @b((output))  case specific label format
*
* @returns  
*
* @notes    
*       
* @end
*********************************************************************/
void dnsInternalNameToCaseConvert(dnsDomainName_t *name,
                                  L7_uchar8       *labels)
{
  L7_uint32 index = 0;

  memset(labels, 0, DNS_NAME_LABELS_SIZE_MAX);
  while (index < DNS_NAME_LABELS_SIZE_MAX)
  {
    L7_uint32 i = 0;
    L7_uchar8 length = name->labels[index];
    labels[index] = length;
    if (length == 0)
    {
      break;
    }
    index++;
    if (index >= DNS_NAME_LABELS_SIZE_MAX)
    {
      break;
    }
    for (i = 0; i < length; i++)
    {
      if ((name->uppercase[CASE_INDEX(index)] & CASE_MASK(index)) == 0)
      {
        labels[index] = name->labels[index];
      } else
      {
        labels[index] = toupper(name->labels[index]);
      }
      index++;
    }
  }
  return;
}

/*********************************************************************
* @purpose  Convert case specific labels to internal "binary" format
*
* @param    L7_uchar8       *labels   @b((input))   case specific labels
* @param    dnsDomainName_t *name     @b((output))  internal "binary" format
*
* @returns  
*
* @notes    
*       
* @end
*********************************************************************/
void dnsCaseNameToInternalConvert(L7_uchar8       *labels, 
                                  dnsDomainName_t *name)
{
  L7_uint32 index = 0;

  memset(name, 0, sizeof(dnsDomainName_t));
  while (index < DNS_NAME_LABELS_SIZE_MAX)
  {
    L7_uint32 i = 0;
    L7_uchar8 length = labels[index];
    name->labels[index] = length;
    if (length == 0)
    {
      break;
    }
    index++;
    if (index >= DNS_NAME_LABELS_SIZE_MAX)
    {
      break;
    }
    for (i = 0; i < length; i++)
    {
      if (islower((int)labels[index]) != 0)
      {
        name->labels[index] = labels[index];
      } else
      {
        name->uppercase[CASE_INDEX(index)] |= CASE_MASK(index);
        name->labels[index] = tolower(labels[index]); 
      }
      index++;
    }
  }
}

/*********************************************************************
* @purpose  convert configured default domain name into active search list
*
* @param    none
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void dnsDefaultDomainNameSearchListGenerate()
{
  L7_char8  *p = L7_NULLPTR;
  L7_char8  *prev = L7_NULLPTR;
  L7_uint32  index = 0;          
  L7_uint32  labels = 1;

  memset(dnsOprData->searchTbl, 0, 
         L7_DNS_SEARCH_LIST_ENTRIES * DNS_DOMAIN_NAME_SIZE_MAX); 

  if (strlen(dnsCfgData->domain) > 0)
  {
    /* first entry is always the entire domain name */
    osapiStrncpy(dnsOprData->searchTbl[index++], dnsCfgData->domain, DNS_DOMAIN_NAME_SIZE_MAX);

    /* count labels in domain name, when we get to two labels
       save label start, and then for every label more
       than two copy the previous to next search entry */

    p = dnsCfgData->domain;
    while (((p = strchr(p, '.')) != L7_NULLPTR) &&
           (index < L7_DNS_SEARCH_LIST_ENTRIES))
    {
      /* search list should look the same regardless of ending dot */
      p++;
      if ((*p) != '\0')
      {
        labels++;
        if (labels >= 2)
        {
          if (prev != L7_NULLPTR)
          {
            osapiStrncpy(dnsOprData->searchTbl[index++], prev, DNS_DOMAIN_NAME_SIZE_MAX);
          }
          prev = p;
        }
      }
    }

    /* we don't copy the last label, last entry has two labels */
  }

  return;
}

/*********************************************************************
* @purpose  convert configured domain name List into active search list
*
* @param    none
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void dnsDomainNameListSearchListGenerate()
{
  L7_uint32  index = 0;          

  memset(dnsOprData->searchTbl, 0, 
         L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES * DNS_DOMAIN_NAME_SIZE_MAX); 

  for (index=0; index < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES; index++)
  {
    if(dnsCfgData->domainList[index][0] != 0)
    {
      dnsAddDomainNameToSearchList(dnsCfgData->domainList[index]);
    }
    else
     break;
  }
  return;
}

/*********************************************************************
* @purpose  Add domainName to active search list
*
* @param    L7_char8 *domainname
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void dnsAddDomainNameToSearchList(L7_char8 *domainname)
{
  L7_uint32  index = 0;          

  while(index < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES)
  {
    if(dnsOprData->searchTbl[index][0] == 0)
    {
      osapiStrncpy(dnsOprData->searchTbl[index], domainname, DNS_DOMAIN_NAME_SIZE_MAX);
      break;
    }
    index++;
  }
  return;
}

/*********************************************************************
* @purpose  Remove configured domain name from opdata structure
*
* @param    L7_char8 *domainname
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void dnsRemoveDomainNameFromSearchList(L7_char8 *domainname)
{
  L7_uint32  i = 0;          
  L7_BOOL found = L7_FALSE;

  for (i=0; i < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES; i++)
  {
    if (found == L7_TRUE)
    {
      /* move entries after removed entry down one */
      if (dnsOprData->searchTbl[i][0] != 0)
      {
        osapiStrncpy(dnsOprData->searchTbl[i-1], dnsOprData->searchTbl[i], DNS_DOMAIN_NAME_SIZE_MAX);
        memset(dnsOprData->searchTbl[i], '\0', DNS_DOMAIN_NAME_SIZE_MAX);
      }
    }
    else if ((strcmp(dnsOprData->searchTbl[i], domainname) == 0))
    {
      memset(dnsOprData->searchTbl[i], '\0', DNS_DOMAIN_NAME_SIZE_MAX);
      found = L7_TRUE;
    }
  }
  /* if the last list entry is removed copy default domain
      name into search list */
  if(dnsOprData->searchTbl[0][0] == 0)
  {
    dnsDefaultDomainNameSearchListGenerate();
  }
  return;
}

/*********************************************************************
* @purpose  Create the list of qualified hostnames in internal "binary" format
*
* @param    L7_char8           *hostname    @b((input))   hostname
* @param    dnsDomainName_t    *nameList    @b((output))  list in internal format
*
* @returns  L7_SUCESS, list generated
*           L7_FAILURE, invalid hostname
*
* @notes    by the time we get here we assume arguments are okay. 
*       
* @end
*********************************************************************/
static L7_RC_t dnsQualifiedNameListGet(L7_char8        *hostname, 
                                       dnsDomainName_t *nameList)
{
  L7_char8  qualifiedName[DNS_DOMAIN_NAME_SIZE_MAX];
  L7_uint32 index = 0;
  L7_uint32 searchIndex = 0;
  L7_uint32 length = strlen(hostname);

  /* if hostname has at least one dot, its possible its already
     qualified and should be our first lookup attempt */
  memset(nameList, 0, sizeof(dnsDomainName_t) * L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES);

  if(strchr(hostname, '.') != L7_NULLPTR)
  {
    if (dnsStringNameToInternalConvert(hostname, 
                                     L7_FALSE, /* keep same case for query */
                                     &nameList[index++]) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    /* if hostname ends with a dot, its absolute, don't qualify */
    if (hostname[strlen(hostname)-1] == '.')
    {
      return L7_SUCCESS;
    }
  }

  /* add on each search list entry */
  /* based on size of searchTbl, we should never have > DNS_REQUEST_SEARCH_LIST_ENTRIES */
  /* We use the default domain list entries if no domain list entries are 
     configured */
  if(dnsCfgData->domainList[searchIndex][0] == 0)
  {
    while ((searchIndex < DNS_REQUEST_SEARCH_LIST_ENTRIES) &&
           (dnsOprData->searchTbl[searchIndex][0] != 0)&& 
           (index < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES))
    {
      /* make sure we don't create a name too big, include '.' & '\0' */
      if ((length + strlen(dnsOprData->searchTbl[searchIndex]) + 2) <= DNS_DOMAIN_NAME_SIZE_MAX) 
      {
        sprintf(qualifiedName, "%s.%s", hostname, dnsOprData->searchTbl[searchIndex]);
        if (dnsStringNameToInternalConvert(qualifiedName, 
                                           L7_FALSE,   /* keep same case for query */
                                           &nameList[index++]) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
      searchIndex++;
    }
  }
  else
  {
   /* use domain list entries instead of default domain name */
    while ((searchIndex < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES) &&
           (dnsOprData->searchTbl[searchIndex][0] != 0) && 
           (index < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES))
    {
      /* make sure we don't create a name too big, include '.' & '\0' */
      if ((length + strlen(dnsOprData->searchTbl[searchIndex]) + 2) <= DNS_DOMAIN_NAME_SIZE_MAX) 
      {
        sprintf(qualifiedName, "%s.%s", hostname, dnsOprData->searchTbl[searchIndex]);
        if (dnsStringNameToInternalConvert(qualifiedName, 
                                           L7_FALSE,   /* keep same case for query */
                                           &nameList[index++]) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
      searchIndex++;
    }
   
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Create the name lookup request state
*
* @param    L7_uchar8          family       @b((input)) family type
* @param    L7_COMPONENT_IDS_t  componentId @b((input))   component requesting data
* @param    L7_uint32           requestId   @b((input))   component's request id
* @param    L7_char8           *hostname    @b((input))   hostname
* @param    dnsRequestEntry_t **request     @b((output))  request state
*
* @returns  L7_SUCESS, request generated
*           L7_FAILURE, invalid hostname, or no request buffers available 
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dnsRequestGet(L7_uchar8           family,
                      L7_COMPONENT_IDS_t  componentId,
                      L7_uint32           requestId,
                      L7_char8           *hostname,
                      dnsRequestEntry_t **request)
{
  dnsDomainName_t    nameList[L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES];
  dnsRequestEntry_t *req = L7_NULLPTR;
  L7_uint32          i = 0;

  /* do as little as possible here, make sure we can generate 
     qualified hostnames for the lookup, and there is an
     available request, then let the DNS task do the rest */

  if (dnsQualifiedNameListGet(hostname, nameList) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i=0; i < L7_DNS_CONCURRENT_REQUESTS; i++)
  {
    if (dnsOprData->requestTbl[i].id == 0)
    {
      req = &dnsOprData->requestTbl[i];
      break;
    }
  }
  if (req == L7_NULLPTR)
  {
    /* no request buffers available */
    return L7_FAILURE;
  }
  memset(req, 0, sizeof(dnsRequestEntry_t));
  req->id = (dnsRequestId++ == 0) ? dnsRequestId++ : dnsRequestId;
  req->requestTimer = dnsCfgData->requestTimeout;
  req->retries = dnsCfgData->retries;
  req->componentId = componentId;
  req->requestId = requestId;
  req->family    = family;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  req->recordType =  (family == L7_AF_INET) ? DNS_RR_TYPE_ADDRESS : DNS_RR_TYPE_IPV6_ADDRESS;
#else
  req->recordType = DNS_RR_TYPE_ADDRESS;
#endif
  memcpy(req->nameList, nameList, L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES * sizeof(dnsDomainName_t));
  osapiStrncpy(req->hostName, hostname, L7_DNS_HOST_NAME_LEN_MAX);
  dnsTimerAdd();    

  *request = req;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create the reverse name lookup request state
*
* @param    L7_COMPONENT_IDS_t  componentId @b((input))   component requesting data
* @param    L7_uint32           requestId   @b((input))   component's request id
* @param    L7_inet_addr_t      inetIp      @b((input))   inet Ip address
* @param    dnsRequestEntry_t **request     @b((output))  request state
*
* @returns  L7_SUCESS, request generated
*           L7_FAILURE, invalid hostname, or no request buffers available 
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dnsReverseRequestGet(L7_COMPONENT_IDS_t  componentId,
                             L7_uint32           requestId,
                             L7_inet_addr_t      inetIp,
                             dnsRequestEntry_t **request)
{
  dnsRequestEntry_t *req = L7_NULLPTR;
  L7_uint32          i   = 0;

  /* get free request buffer */
  for (i=0; i < L7_DNS_CONCURRENT_REQUESTS; i++)
  {
    if (dnsOprData->requestTbl[i].id == 0)
    {
      req = &dnsOprData->requestTbl[i];
      break;
    }
  }
  if (req == L7_NULLPTR)
  {
    /* no request buffers available */
    dnsDebugTrace("[DNS client] %s (%d)  no request buffers available \n",__FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  
  memset(req, 0, sizeof(dnsRequestEntry_t));
  
  /* fill in request data */
  req->id           = (dnsRequestId++ == 0) ? dnsRequestId++ : dnsRequestId;
  req->requestTimer = dnsCfgData->requestTimeout;
  req->retries      = dnsCfgData->retries;
  req->componentId  = componentId;
  req->requestId    = requestId;
  req->requestType  = dnsReverseRequest;
  inetCopy(&req->inetIpAddr, &inetIp);
  
  dnsTimerAdd();    

  *request = req;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a standard DNS query from request state
*
* @param    dnsRequestEntry_t *request     @b((input))   request state
* @param    L7_uint32         *length      @b((output))  packet length
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
static void dnsRequestPacketWrite(dnsRequestEntry_t *request)
{
  dnsMessageHeader_t *header = (dnsMessageHeader_t *)request->packet;
  L7_uchar8 *p = L7_NULLPTR;
  L7_uchar8  length = 0;
  L7_ushort16 temp = 0;

  memset(request->packet, 0, DNS_UDP_MSG_SIZE_MAX);
  
  /* setup header */
  header->id = osapiHtons(request->id);
  /* query bit is zero, opcode for standard query is zero */
  /* indicate we want a recursive query */
  header->bits |= DNS_MSG_HDR_RD_BIT_MASK;
  header->qdcount = osapiHtons(1);

  /* now fill in question */
  switch (request->requestType)
  {
    case dnsDirectRequest:
      p = request->nameList[request->nameIndex].labels;
      while ((*p) != 0)
      {
        length += (*p) + 1;
        p += (*p) + 1;
      }
      length += 1;  /* include trailing zero */
      p = request->packet + sizeof(dnsMessageHeader_t);
      memcpy(p, request->nameList[request->nameIndex].labels, length); 
      p += length;
      if (request->family == L7_AF_INET)
      {
        temp = osapiHtons(DNS_RR_TYPE_ADDRESS); 
        memcpy(p, &temp, sizeof(L7_ushort16));
    
      }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
      else
      {
        temp = osapiHtons(DNS_RR_TYPE_IPV6_ADDRESS); 
        memcpy(p, &temp, sizeof(L7_ushort16));
      }
#endif
      p += sizeof(L7_ushort16);
      break;
      
    case dnsReverseRequest:
      {
        /* send a question with QNAME in format <IP>.in-addr.arpa */
        /*TBD : Need to add support for IPv6 for reverse request */
        L7_uint32       ipAddr         = osapiHtonl(request->inetIpAddr.addr.ipv4.s_addr);
        L7_uint8       *ipAddrPtr      = (L7_uint8 *)&(ipAddr); 
        L7_char8        inAddrArpaNameStr[DNS_DOMAIN_NAME_SIZE_MAX] = "";
        dnsDomainName_t inAddrArpaInternalName;
        
        /* generate in-addr.arpa domain name in internal format */        
        osapiSnprintf(inAddrArpaNameStr,
                      DNS_DOMAIN_NAME_SIZE_MAX,
                      "%u.%u.%u.%u.in-addr.arpa",
                      ipAddrPtr[0],
                      ipAddrPtr[1],
                      ipAddrPtr[2],
                      ipAddrPtr[3]);
        (void)dnsStringNameToInternalConvert(inAddrArpaNameStr, L7_FALSE, &inAddrArpaInternalName); 
        
        /* calculate length of generated name */
        p = inAddrArpaInternalName.labels;
        while ((*p) != 0)
        {
          length += (*p) + 1;
          p += (*p) + 1;
        }
        length += 1;  /* include trailing zero */

        /* now copy the name to packet buffer */
        p = request->packet + sizeof(dnsMessageHeader_t);
        memcpy(p, inAddrArpaInternalName.labels, length); 
        p += length;
        
        /* QTYPE = PTR */
        temp = 0;
        temp = osapiHtons(DNS_RR_TYPE_PTR);
        memcpy(p, &temp, sizeof(L7_ushort16));
        p += sizeof(L7_ushort16);
      }
      break;
      
    default:
      LOG_MSG("DNS Client: dnsRequestPacketWrite() - invalid request type - %d\n", request->requestType);
      break;
  }
  /* QCLASS = INTERNET */
  temp = 0;
  temp = osapiHtons(DNS_RR_CLASS_INTERNET);
  memcpy(p, &temp, sizeof(L7_ushort16));
  p += sizeof(L7_ushort16);

  request->length = p - request->packet;
  return;
}

/*********************************************************************
* @purpose  Send out a DNS query packet based on request state
*
* @param    dnsRequestEntry_t *request     @b((input))  request state
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
static void dnsRequestQuerySend(dnsRequestEntry_t *request)
{
  L7_uint32 i = 0;
  if (request->transmits == 0)
  {
    dnsRequestPacketWrite(request);
  }
  /* a server was removed, we received a server error, or we timed
     out on max retransmits to this server, start the request over
     at the next configured server entry */
  while (dnsNameServerPacketSend(&request->server,
                              request->packet,
                              request->length) != L7_SUCCESS)
 { 
   request->nameIndex = 0;
   request->transmits = 0;
   for (i = 0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
   {
    if ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->servers[i])
        != L7_TRUE) &&
        (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->servers[i],
                               &request->server) == L7_TRUE))
    {
      break;
    }
  }
  if ((++i < L7_DNS_NAME_SERVER_ENTRIES) &&
      (dnsClientIsInetAddressZero(&dnsCfgData->servers[i]) != L7_TRUE))
  {
    inetCopy(&request->server, &dnsCfgData->servers[i]);
  } else
  {
    /* we are out of servers */
    dnsNameLookupFailureSend(request, DNS_LOOKUP_STATUS_SOCKET_ERROR);
    break;
  }

 }

  request->transmits++;
  request->queryTimer = dnsCfgData->queryTimeout;
  dnsTimerAdd();

  return;
}

/*********************************************************************
* @purpose  Process a name lookup request
*
* @param    dnsRequestEntry_t *request     @b((input))  request state
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
void dnsRequestProcess(dnsRequestEntry_t *request)
{
  L7_uint32 i = 0;
  dnsDomainName_t nameList;
  dnsDomainName_t name;
  L7_inet_addr_t  inetAddr;

  inetAddressZeroSet(request->family, &inetAddr);
  /* This is added to check if hostname doesnot contain a dot 
     and we check the hostname in the cache list */
  if(strchr(request->hostName, '.') == L7_NULLPTR)
  {
    if(dnsStringNameToInternalConvert(request->hostName, L7_FALSE,
                                       &nameList) == L7_SUCCESS)
    {
      dnsCaseNameToInternalConvert(nameList.labels, &name);
      /* look for static mapping first, then dynamic cache */
      if (((dnsCacheStaticIpLookup(&name, request->family, &inetAddr) == L7_SUCCESS) ||
          (dnsCacheIpLookup(&name, request->family, &inetAddr) == L7_SUCCESS))&&
          (inetAddr.family == request->family))
      {
        request->nameIndex = i;
        dnsNameLookupResponseSend(request, &inetAddr); 
        return;
      }
    }
  }

  for (i = 0; i < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES; i++)
  {
    if (request->nameList[i].labels[0] != 0)
    {
      /* convert to lowercase, query names can still be mixed case */
      dnsCaseNameToInternalConvert(request->nameList[i].labels, &name);
      /* look for static mapping first, then dynamic cache */
      if (((dnsCacheStaticIpLookup(&name, request->family, &inetAddr) == L7_SUCCESS) ||
          (dnsCacheIpLookup(&name, request->family, &inetAddr) == L7_SUCCESS)) && 
          (inetAddr.family == request->family))
      {
        request->nameIndex = i;
        dnsNameLookupResponseSend(request, &inetAddr); 
        return;
      }
    } else
    {
      break;
    }
  }


  /* address not found locally, find a name server and send a query */
  /* start with the first name server, servers saved in preference order */
  request->nameIndex = 0;
  inetCopy(&request->server, &dnsCfgData->servers[0]);
  /* If either server is not found or no label is added in requestget
     send a failure notification. The case where labels would not be 
     present is when dns resolution is done for a top level domain which
     we may have stored in the cache 
   */
  if ((dnsClientIsInetAddressZero(&request->server) == L7_TRUE) || 
      (request->nameList[0].labels[0] == 0))
  {
    /* No DNS server configured. Let's try to broadcast request. */
    /* Knock, and someone will open the door. */
    L7_uint32  ip = L7_IP_LTD_BCAST_ADDR;
    inetAddressSet(L7_AF_INET, &ip, &request->server);
  }

  /* send query based on request state */
  dnsRequestQuerySend(request);

  return;
}

/*********************************************************************
* @purpose  Process a reverse name lookup request
*
* @param    dnsRequestEntry_t *request     @b((input))  request state
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
void dnsReverseRequestProcess(dnsRequestEntry_t *request)
{
  dnsDomainName_t name;
  L7_char8        hostname[DNS_DOMAIN_NAME_SIZE_MAX] = "";

  /* look for static mapping first, then dynamic cache */
  if ((dnsCacheStaticNameLookup(request->inetIpAddr, &name) == L7_SUCCESS) ||
      (dnsCacheNameLookup(request->inetIpAddr, &name) == L7_SUCCESS))
  {
    dnsInternalNameToStringConvert(&name, L7_FALSE, hostname);
    dnsReverseNameLookupResponseSend(request, hostname); 
    return;
  }

  /* address not found locally, find a name server and send a query */
  /* start with the first name server, servers saved in preference order */
  request->nameIndex = 0;
  inetCopy(&request->server, &dnsCfgData->servers[0]);
  /* If server is not found send a failure notification. */
  if (inetIsAddressZero(&request->server) == L7_TRUE)
  {
    /* No DNS server configured. Let's try to broadcast request. */
    /* Knock, and someone will open the door. */
    L7_uint32  ip = L7_IP_LTD_BCAST_ADDR;
    inetAddressSet(L7_AF_INET, &ip, &request->server);
  }

  /* send query based on request state */
  dnsRequestQuerySend(request);

  return;
}

/*********************************************************************
* @purpose  Read one name label from a packet pointer
*
* @param    L7_uchar8    *p       @b((input))  position in packet
* @param    L7_uchar8    *labels  @b((output)) ptr to store label
* @param    L7_ushort16   maxlen  @b((input))  max bytes to read
* @param    L7_uchar8    *bytes   @b((output)) actual bytes read
* 
* @returns  L7_SUCCESS, label read
* @returns  L7_FAILURE, not enough space for label  
*
* @notes   This will read zero octet, bytes returned is 1. 
*       
* @end
*********************************************************************/
static L7_RC_t dnsPacketLabelRead(L7_uchar8    *p, 
                                  L7_uchar8    *labels, 
                                  L7_ushort16   maxlen, 
                                  L7_uchar8    *bytes)
{
  L7_uchar8 length = (*p) + 1;
  if ((length <= maxlen) &&
      (length <= DNS_NAME_LABEL_SIZE_MAX))
  {
    memcpy(labels, p, length);
    *bytes = length;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Read labels from a packet pointer
*
* @param    L7_uchar8    *packet  @b((input))  start of packet
* @param    L7_ushort16  offset   @b((input))  offset for labels
* @param    L7_uchar8    *labels  @b((output)) ptr to store labels
* @param    L7_ushort16   maxlen  @b((input))  max bytes to read
* 
* @returns  L7_SUCCESS, labels read
* @returns  L7_FAILURE, not enough space for labels  
*
* @notes   This will recursively follow label pointers.
*       
* @end
*********************************************************************/
static L7_RC_t dnsPacketPtrRead(L7_uchar8  *packet,
                                L7_ushort16 offset,
                                L7_uchar8  *labels,
                                L7_ushort16 maxlen)
{
  L7_ushort16 i = 0;  /* total number of bytes read into labels */
  L7_uchar8  *p = packet + offset;

  /* labels may be 
     - a pointer
     - a sequence of labels ending with a ptr
     - sequence of labels ending in a zero octet
   */
  while (((*p) & DNS_MSG_RR_NAME_PTR_MASK) == 0)
  {
    L7_uchar8 bytes = 0;
    if (dnsPacketLabelRead(p, labels + i, maxlen - i, &bytes) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    p += bytes;
    i += bytes;
    if (bytes == 1)      /* reached zero octet */
    {
      return L7_SUCCESS;
    }
  }
  if (((*p) & DNS_MSG_RR_NAME_PTR_MASK) != 0)
  {
    L7_ushort16 offsetNbo;   /* net byte order */
    L7_ushort16 offset;      /* host byte order, with mask applied */

    /* can't count on two-byte alignment here; so use memcpy */
    memcpy(&offsetNbo, p, 2);     
    offset = osapiNtohs(offsetNbo) & DNS_MSG_RR_NAME_OFFSET_MASK;
    if (dnsPacketPtrRead(packet, offset, labels + i, maxlen - i) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Read name field from a resource record, converting
*           to our internal format.
*
* @param    L7_uchar8         *packet   @b((input))   start of packet
* @param    L7_uchar8         **p       @b((input/output))  pointer to current position pointer
* @param    L7_ushort16       length    @b((input))   length if known
* @param    dnsDomainName_t   *name     @b((output))  name converted to internal format
*
* @returns  L7_FAILURE, error reading name
* @returns  L7_SUCCESS, name returned, pointer advanced
*
* @notes    if length == 0, will read until zero octet or max size 
*       
* @end
*********************************************************************/
L7_RC_t dnsPacketRRNameRead(L7_uchar8       *packet, 
                                   L7_uchar8      **pptr, 
                                   L7_ushort16      length,
                                   dnsDomainName_t *name)
{
  L7_ushort16 maxlen = (length != 0) ? length : DNS_NAME_LABELS_SIZE_MAX;
  L7_uchar8   labels[DNS_NAME_LABELS_SIZE_MAX];
  L7_ushort16 i = 0;  /* count total number of bytes read from this name field */
  L7_uchar8  *p = *pptr;
  L7_BOOL     zero = L7_FALSE;  /* did we reach zero octet */

  memset((void *)labels, 0, DNS_NAME_LABELS_SIZE_MAX);

  /* a name may be 
     - a pointer
     - a sequence of labels ending with a ptr
     - sequence of labels ending in a zero octet
   */
  while (((*p) & DNS_MSG_RR_NAME_PTR_MASK) == 0)
  {
    L7_uchar8 bytes = 0;
    if (dnsPacketLabelRead(p, labels + i, maxlen - i, &bytes) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    p += bytes;
    i += bytes;
    if (bytes == 1)  /* reached zero octet */
    {
      zero = L7_TRUE;
      break;
    }
  }
  if ((zero == L7_FALSE) &&
      (((*p) & DNS_MSG_RR_NAME_PTR_MASK) != 0))
  {
    L7_ushort16  temp = 0;
    L7_ushort16 offset;

    memcpy (&temp, p, sizeof(L7_ushort16));

    offset = osapiNtohs(temp) & DNS_MSG_RR_NAME_OFFSET_MASK;
    if (dnsPacketPtrRead(packet, offset, labels + i, DNS_NAME_LABELS_SIZE_MAX - i) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    p += sizeof(L7_ushort16);
    i += sizeof(L7_ushort16);
  }

  /* if a specific length was specified, ensure that is what we read */
  if ((length != 0) && (length != i))
  {
    return L7_FAILURE;
  }

  /* do case conversion and store in internal format */
  dnsCaseNameToInternalConvert(labels, name);

  *pptr = p;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Read a response packet, cache cname and address records,
*           return primary ip address.
*
* @param    dnsRequestEntry_t *request     @b((input))  request state
* @param    L7_inet_addr_t    *inetAddr    @b((output)) ipv4 or ipv6 
*                                                       address
*
* @returns  L7_FAILURE, bad packet, or no ip resolved.
* @returns  L7_SUCCESS, good packet and ip returned.  
*
* @notes    
*       
* @end
*********************************************************************/
static L7_RC_t dnsResponsePacketRead(dnsRequestEntry_t *request,
                                     L7_inet_addr_t    *inetAddr)
{
  dnsMessageHeader_t *header = (dnsMessageHeader_t *)request->packet;
  L7_uchar8          *p = request->packet + sizeof(dnsMessageHeader_t);
  dnsDomainName_t     name;           /* query name or alias name to compare against answer(s) */
  dnsDomainName_t     qName;         /* name in current question record */
  dnsDomainName_t     anName;         /* name in current answer record */
  L7_uint32           ttl = 0;
  L7_uint32           ipIndex = 0;
  L7_ushort16         anIndex = 0;    /* answer records read */
  L7_uint32           cnameRefs = 0;
  L7_uint32           ipAddr;
  L7_uint32           addrIndex = 0;
  L7_uchar8           type = 0;
  L7_inet_addr_t      inetAddrList[DNS_CACHE_ENTRY_IP_ADDRESSES];
  L7_uint32           temp =0;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uchar8           ipv6Addr[IPV6_ADDRESS_LEN];
  L7_inet_addr_t      inetAddrNetwork;

  memset(ipv6Addr, L7_NULL, sizeof(ipv6Addr));
  inetAddressZeroSet(L7_AF_INET6, &inetAddrNetwork);
#endif
  for (addrIndex = 0; addrIndex < DNS_CACHE_ENTRY_IP_ADDRESSES; addrIndex++)
  {
    inetAddressZeroSet(request->family, &inetAddrList[addrIndex]);
  }
  /* initialize expected answer with query name */
  dnsCaseNameToInternalConvert(request->nameList[request->nameIndex].labels, &name);

  dnsCaseNameToInternalConvert(p, &qName);
  /* read past the question section */
  while ((*p) != 0)    /* QNAME is variable, ends with trailing zero */
  {
    p += (*p) + 1;
  }
  p++;     /* skip trailing zero */
  p += 4;  /* skip QTYPE and QCLASS */


  /* read answer section */
  while (((*p) != 0) &&
         (anIndex < osapiNtohs(header->ancount)) &&
         (ipIndex < DNS_CACHE_ENTRY_IP_ADDRESSES) &&
         (cnameRefs < DNS_CNAME_REFERENCES_MAX))
  {
    L7_ushort16     rdlength = 0;
    L7_ushort16     tempRdLen = 0;
    L7_BOOL         skip = L7_FALSE;
    L7_ushort16     tempType = 0;
    L7_ushort16     tempClass = 0;
    if (dnsPacketRRNameRead(request->packet, &p, 0, &anName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    /* make sure this is an answer we are expecting */
    if (dnsNameLabelsEqual(name.labels, anName.labels) != L7_SUCCESS)
    {
      /* keep reading, and skip over this answer */
      skip = L7_TRUE;
    }

    /* read resource record type */
    memcpy (&tempType, p, sizeof(L7_ushort16));
    type = osapiNtohs(tempType);
    p += sizeof(L7_ushort16);

    memcpy (&tempClass, p, sizeof(L7_ushort16));
    /* class should always be DNS_RR_CLASS_INTERNET */
    if (DNS_RR_CLASS_INTERNET != osapiNtohs(tempClass))
    {  
      return L7_FAILURE;
    }
    p += sizeof(L7_ushort16);
    temp = 0;
    memcpy(&temp, p, sizeof(L7_uint32)); 
    ttl = osapiNtohl(temp);
    p += sizeof(L7_uint32);
    memcpy(&tempRdLen, p, sizeof(L7_ushort16));
    rdlength = osapiNtohs(tempRdLen);
    p += sizeof(L7_ushort16);

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    if (skip == L7_TRUE ||
        (type != DNS_RR_TYPE_CNAME &&
         type != DNS_RR_TYPE_ADDRESS &&
         type != DNS_RR_TYPE_IPV6_ADDRESS))
#else
    if (skip == L7_TRUE ||
        (type != DNS_RR_TYPE_CNAME &&
         type != DNS_RR_TYPE_ADDRESS))
#endif
    {
      /* record count of answers that we skip over */
      dnsOprData->counters.badCaches++;
      p += rdlength;
      continue;
    }

    /* read data for cname and address records */
    if (type == DNS_RR_TYPE_CNAME)
    { 
      if (ipIndex != 0)
      {
        /* we already read an ip address, not expecting an alias record */
        dnsOprData->counters.badCaches++;
        p += rdlength;
        continue;
      }
      cnameRefs++;
      /* cname alias will replace the answer we are expecting */
      if (dnsPacketRRNameRead(request->packet, &p, rdlength, &name) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      if (ttl != 0)
      {
        /* cache this reference */
        if (dnsCacheEntryAdd(&qName, &anName, DNS_RR_TYPE_CNAME, 
                             ttl, &request->server, &name) != L7_SUCCESS)
        {
          LOG_MSG("DNS Client: failed to cache alias for request id %u\n", request->id);
        } else
        {
          dnsOprData->counters.goodCaches++;
        }
      }
    }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    else if (type == DNS_RR_TYPE_ADDRESS)
#else
    else
#endif
    {
      /* DNS_RR_TYPE_ADDRESS */
      if (rdlength != sizeof(L7_uint32))
      {
        return L7_FAILURE;
      }
      temp = 0;
      memcpy(&temp, p, sizeof(L7_uint32)); 
      ipAddr = osapiNtohl(temp);
      
      inetAddressSet(L7_AF_INET, &ipAddr, &inetAddrList[ipIndex++]);
      p += rdlength;
    }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    else
    {
      /* IPv6 Address (DNS_RR_TYPE_IPV6_ADDRESS) */
      if (rdlength != IPV6_ADDRESS_LEN)
      {
        /* Length mismatch */
        return L7_FAILURE;
      }
      /* copy 128 bit  address */
      memcpy(ipv6Addr, p, rdlength);
      inetAddressSet(L7_AF_INET6, ipv6Addr, &inetAddrNetwork);
      if (inetAddrNtoh(&inetAddrNetwork, &inetAddrList[ipIndex++]) != L7_SUCCESS)
      {
        /* Network to Host order conversion failed */
        return L7_FAILURE;
      }
      p += rdlength;
    }
#endif

    anIndex++;
  }

  /* fail if we didn't read an ip address */
  if (ipIndex == 0)
  {
    LOG_MSG("DNS Client: no ipv4 or ipv6 address found in response for request id %u\n", request->id);
    return L7_FAILURE;
  }

  /* cache the ip addresses for final answer name */
  if (ttl != 0)
  {
    if (dnsCacheEntryAdd(&qName, &anName, type, ttl, 
                         &request->server, inetAddrList) != L7_SUCCESS)
    {
      LOG_MSG("DNS Client: failed to add cache entry for request id %u\n", request->id);
    } else 
    {
      dnsOprData->counters.goodCaches += ipIndex;
    }
  }

  inetCopy(inetAddr, &inetAddrList[0]); /* return primary ipv4 or ipv6 address */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Read a response packet, cache cname and address records,
*           return resolved hostname.
*
* @param    dnsRequestEntry_t *request     @b((input))  request state
* @param    L7_char8           hostname    @b((output)) hostname
*
* @returns  L7_FAILURE, bad packet, or no hostname resolved.
* @returns  L7_SUCCESS, good packet and hostname returned.  
*
* @notes    
*       
* @end
*********************************************************************/
static L7_RC_t dnsReverseResponsePacketRead(dnsRequestEntry_t *request,
                                            L7_char8          *hostname)
{
  L7_uchar8       *p = request->packet + sizeof(dnsMessageHeader_t);
  dnsDomainName_t  name;           /* query name or alias name to compare against answer(s) */
  dnsDomainName_t  anName;         /* name in current answer record */
  L7_uint32        ttl = 0;
  L7_uchar8        type = 0;
  L7_ushort16      rdlength = 0;
  
  /* read past the question section */
  while ((*p) != 0)    /* QNAME is variable, ends with trailing zero */
  {
    p += (*p) + 1;
  }
  p++;     /* skip trailing zero */
  p += 4;  /* skip QTYPE and QCLASS */

  /* read answer section */
  if (dnsPacketRRNameRead(request->packet, &p, 0, &anName) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* read resource record type */
  type = osapiNtohs(*(L7_ushort16 *)p);
  p += sizeof(L7_ushort16);

  /* class should always be DNS_RR_CLASS_INTERNET */
  if ((*(L7_ushort16 *)p) != osapiNtohs(DNS_RR_CLASS_INTERNET))
  {  
    return L7_FAILURE;
  }
  p += sizeof(L7_ushort16);

  /* read TTL of RR */
  ttl = osapiNtohl(*(L7_uint32 *)p);
  p += sizeof(L7_uint32);
  
  /* length of RDATA */
  rdlength = osapiNtohs(*(L7_ushort16 *)p);
  p += sizeof(L7_ushort16);
 
  /* now read RDATA */
  if (dnsPacketRRNameRead(request->packet, &p, rdlength, &name) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  dnsInternalNameToStringConvert(&name, L7_FALSE, hostname);
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Modify request state and send query to the next server
*
* @param    dnsRequestEntry_t *request     @b((input))  request state
* @param    dnsClientLookupStatus_t status @b((input)) status 
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
void dnsRequestQueryNextServer(dnsRequestEntry_t *request,
                               dnsClientLookupStatus_t status)
{
  L7_uint32 i = 0;
  /* a server was removed, we received a server error, or we timed
     out on max retransmits to this server, start the request over 
     at the next configured server entry */
  request->nameIndex = 0;
  request->transmits = 0;
  for (i = 0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if ((dnsClientIsInetAddrFamilyNotSet(&dnsCfgData->servers[i]) 
        != L7_TRUE) && 
        (L7_INET_IS_ADDR_EQUAL(&dnsCfgData->servers[i], 
                               &request->server) == L7_TRUE))
    {
      break;
    }
  }
  if ((++i < L7_DNS_NAME_SERVER_ENTRIES) &&
      (dnsClientIsInetAddressZero(&dnsCfgData->servers[i]) != L7_TRUE))
  {
    inetCopy(&request->server, &dnsCfgData->servers[i]);
    dnsRequestQuerySend(request);
  } else
  {
    /* we are out of servers */
    dnsNameLookupFailureSend(request, status); 
  }

  return;
}

/*********************************************************************
* @purpose  Modify request state and send query with the next name entry
*
* @param    dnsRequestEntry_t *request     @b((input))  request state
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
static void dnsRequestQueryNextName(dnsRequestEntry_t *request)
{
  /* request failed, received name error, try next qualified domain name */
  request->nameIndex++;
  request->transmits = 0; /* As to generate next query */
  if ((request->nameIndex < L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES) &&
      (request->nameList[request->nameIndex].labels[0] != 0))
  {
    dnsRequestQuerySend(request);
  } else
  {
    dnsRequestQueryNextServer(request, DNS_LOOKUP_STATUS_NAME_ERROR);
  }

  return;
}

/*********************************************************************
* @purpose  Process a DNS query response
*
* @param    dnsRequestEntry_t *request     @b((input))  request state
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
void dnsResponseProcess(dnsRequestEntry_t *request)
{
  dnsMessageHeader_t *header = (dnsMessageHeader_t *)request->packet;
  L7_inet_addr_t      inetAddr;

  inetAddressZeroSet(request->family, &inetAddr);

  /* look at the response code */
  switch (header->rcode & DNS_MSG_HDR_RCODE_BIT_MASK)
  {
    case DNS_RCODE_NO_ERROR:
      dnsOprData->counters.rcodes[DNS_RCODE_NO_ERROR]++;
      break;
    case DNS_RCODE_NAME_ERROR:
      dnsOprData->counters.rcodes[DNS_RCODE_NAME_ERROR]++;
      dnsRequestQueryNextName(request);
      return;
    case DNS_RCODE_FORMAT_ERROR:
      dnsOprData->counters.rcodes[DNS_RCODE_FORMAT_ERROR]++;
    case DNS_RCODE_SERVER_FAILURE:
      dnsOprData->counters.rcodes[DNS_RCODE_SERVER_FAILURE]++;
    case DNS_RCODE_NOT_IMPLEMENTED:
      dnsOprData->counters.rcodes[DNS_RCODE_NOT_IMPLEMENTED]++;
    case DNS_RCODE_REFUSED:
      dnsOprData->counters.rcodes[DNS_RCODE_REFUSED]++;
      dnsRequestQueryNextServer(request, DNS_LOOKUP_STATUS_RESPONSE_ERROR);
      return;
    default:
      LOG_MSG("DNS Client: Received bad RCODE value\n");
      dnsOprData->counters.unparsed++;
      dnsRequestQueryNextServer(request, DNS_LOOKUP_STATUS_RESPONSE_ERROR);
      return;
  }

  if ((header->bits & DNS_MSG_HDR_AA_BIT_MASK) == 0)
  {
    dnsOprData->counters.nonAA++;
    if (header->ancount == 0)
    {
      dnsOprData->counters.nonAANoData++;
    }
  }

  switch (request->requestType)
  {
    case dnsDirectRequest:
      if (dnsResponsePacketRead(request, &inetAddr) == L7_SUCCESS)
      {
        /* return ip to requesting compoent */
        dnsNameLookupResponseSend(request, &inetAddr);
        return;
      }
      break;

    case dnsReverseRequest:
      {
        L7_char8 hostname[DNS_DOMAIN_NAME_SIZE_MAX] = "";
        if (dnsReverseResponsePacketRead(request, hostname) == L7_SUCCESS)
        {
          /* return ip to requesting compoent */
          dnsReverseNameLookupResponseSend(request, hostname);
          return;
        }
      }
      break;
    
    default:
      LOG_MSG("DNS Client: dnsResponseProcess() - invalid request type - %d\n", request->requestType);
      break;
  }

  /* If we fail to read the packet assume this server is bad and 
     try to send a query to the next server in list */
  if (dnsResponsePacketRead(request, &inetAddr) != L7_SUCCESS)
  {
    dnsOprData->counters.unparsed++;
    dnsRequestQueryNextServer(request, DNS_LOOKUP_STATUS_PACKET_ERROR);
    return;
  }

  dnsNameLookupResponseSend(request, &inetAddr);
}

/*********************************************************************
*
* @purpose callback to handle the request timer events
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void dnsTimerCallback()
{
  dnsMgmtMsg_t msg;

  if (!dnsQueue)
  {
    LOG_MSG("dnsTimerCallback(): dnsQueue has not been created!\n");
  }

  msg.msgId = dnsMsgTimer;
  if (osapiMessageSend(dnsQueue, 
                       &msg, 
                       DNS_CLIENT_MSG_SIZE, 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("dnsTimerCallback(): timer send failed\n");
  }
  osapiTimerAdd((void *)dnsTimerCallback, L7_NULL, L7_NULL, 
                DNS_TIMER_INTERVAL, &dnsTimer);
}

/*********************************************************************
* @purpose  Process a timer tick
*
* @param    void
*
* @returns  void  
*
* @notes    
*       
* @end
*********************************************************************/
void dnsTimerProcess(void)
{
  L7_uint32 i = 0;

  for (i = 0; i < L7_DNS_CONCURRENT_REQUESTS; i++)
  {
    dnsRequestEntry_t *request = &dnsOprData->requestTbl[i];
    if (request->id != 0)
    {
      if (request->requestTimer != 0)
      {
        request->requestTimer--;
        if (request->requestTimer == 0)
        {
          /* request has timed out */
          dnsTimerDelete();
          dnsNameServerRequestRemove(&request->server);
          dnsNameLookupFailureSend(request, DNS_LOOKUP_STATUS_TIMEOUT); 
          continue;
        }
      }
      if (request->queryTimer != 0)
      {
        request->queryTimer--;
        if (request->queryTimer == 0)
        {
          /* query has timed out */
          dnsTimerDelete();
          if (request->transmits > request->retries)
          {
            dnsNameServerRequestRemove(&request->server);
            /* Query the next server */
            dnsRequestQueryNextServer(request, DNS_LOOKUP_STATUS_TIMEOUT);
          } else
          {
            dnsNameServerRequestRemove(&request->server);
            /* retransmit packet */
            dnsRequestQuerySend(request);
          }
        }
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  checks given inet address is zero
*
* @param    L7_inet_addr_t   *inetAddr @b{(input/output)} ipv4 or 
*                                  ipv6 address of name server
*
* @returns  L7_TRUE, inetAddr is zero
* @returns  L7_FALSE,inetAddr is not zero 
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_BOOL dnsClientIsInetAddressZero(L7_inet_addr_t *inetAddr)
{
  /* checking family in inet address is initialized. */
  if (L7_INET_GET_FAMILY(inetAddr) == L7_NULL)
  {
    return L7_TRUE;
  }
  if (inetIsAddressZero(inetAddr) == L7_TRUE)
  {
    return L7_TRUE;
  } 
  /* if it is initialzed with family type, it is not zero*/
  return L7_FALSE;
}

/*********************************************************************
* @purpose  checks given inet address family is not set.
*
* @param    L7_inet_addr_t   *inetAddr @b{(input/output)} ipv4 or 
*                                  ipv6 address of name server
*
* @returns  L7_TRUE, inetAddr family is zero
* @returns  L7_FALSE,inetAddr family is not zero 
*
* @comments 
*
*       
* @end
*********************************************************************/
L7_BOOL dnsClientIsInetAddrFamilyNotSet(L7_inet_addr_t *inetAddr)
{
  /* checking family in inet address is initialized. */
  if (L7_INET_GET_FAMILY(inetAddr) == L7_NULL)
  {
    return L7_TRUE;
  }
  /* if it is initialzed with family type, it is not zero*/
  return L7_FALSE;
}
