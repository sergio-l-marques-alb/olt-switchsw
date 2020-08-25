/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_debug.c
*
* @purpose DNS client Debug functions
*
* @component DNS client
*
* @comments none
*
* @create 01/16/2007
*
* @author ikiran
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "nvstoreapi.h"
#include "l7_product.h"
#include "osapi_support.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "dns_client_api.h"
#include "dns_client.h"
#include "dns_client_util.h"
#include "dns_client_cache.h"

L7_uint32 dnsTraceFlag = L7_DISABLE;
L7_uint32 dnsDebugFlag = L7_DISABLE;
extern dnsCfgData_t       *dnsCfgData;
extern dnsOprData_t       *dnsOprData;
extern L7_RC_t dnsPacketRRNameRead(L7_uchar8       *packet,
                                   L7_uchar8      **pptr,
                                   L7_ushort16      length,
                                   dnsDomainName_t *name);

static L7_int32 dnsDebugGetResponsecode(L7_uchar8 rcode, L7_uchar8 *val);
/*********************************************************************
* @purpose  Get DNS Client tracing mode
*
* @param    void
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dnsClientTraceModeGet(void)
{
  return(dnsTraceFlag);
}

/*********************************************************************
* @purpose  Set DNS Client tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dnsClientTraceModeSet(L7_uint32 mode)
{
  if(mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  dnsTraceFlag = mode;
  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Print current DNS client configuration values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dnsCfgDump(void)
{
  L7_uint32        i = 0;
  L7_inet_addr_t   serverAddr;
  L7_inet_addr_t   staticAddr;
  L7_char8         hostname[DNS_DOMAIN_NAME_SIZE_MAX];
  L7_char8         domain[DNS_DOMAIN_NAME_SIZE_MAX];
  L7_char8         inetAddr[IPV6_DISP_ADDR_LEN];
  L7_BOOL          mode = L7_FALSE;
  L7_ushort16      timeout = 0;

  inetAddressReset(&staticAddr);
  printf("\n");
  printf("DNS Client Configuration\n");
  printf("========================\n\n");

  if (dnsClientAdminModeGet(&mode) == L7_SUCCESS)
  {
    printf("Admin Mode: %s\n", mode == L7_TRUE ? "Enabled" : "Disabled");
  }
  if (dnsClientDefaultDomainGet(domain) == L7_SUCCESS)
  {
    printf("Default Domain: %s\n", domain);
  }
  if (dnsClientRequestTimeoutGet(&timeout) == L7_SUCCESS)
  {
    printf("Request Timeout: %i seconds\n", timeout);
  }
  if (dnsClientQueryTimeoutGet(&timeout) == L7_SUCCESS)
  {
    printf("Query Timeout: %i seconds\n", timeout);
  }

  printf("\n");

  printf("Domain List Entries\n");
  printf("===================\n");
  memset(domain, '\0', DNS_DOMAIN_NAME_SIZE_MAX);

  while(dnsClientDomainNameListEntryGetNext(domain) == L7_SUCCESS)
  {
    printf("%i: %s\n", i, domain);
    i++;
  }
  printf("\n");

  printf("Name Servers\n");
  printf("============\n");
  i = 0;
  inetAddressReset(&serverAddr);
  while (dnsClientNameServerEntryNextGet(&serverAddr) == L7_SUCCESS)
  {
    printf("%i: %s\n", i, inetAddrPrint(&dnsCfgData->servers[i], inetAddr));
    i++;
  }

  printf("\n");

  printf("Static Host Entries\n");
  printf("===================\n");
  strcpy(hostname, "");
  inetAddressReset(&staticAddr);
  while (dnsClientStaticHostEntryNextGet(hostname, &staticAddr) == L7_SUCCESS)
  {
    printf("%s  %s\n", hostname, inetAddrPrint(&staticAddr, inetAddr));
  }

  printf("\n======================\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print current DNS dynamic cache values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCacheDump(void)
{
  L7_uchar8      rrName[DNS_DOMAIN_NAME_SIZE_MAX];
  dnsRRType_t    rrType;
  L7_uint32      rrIndex;
  dnsRRClass_t   rrClass;
  L7_uchar8      qname[DNS_DOMAIN_NAME_SIZE_MAX];

  printf("\n");
  printf("DNS Cache Entries\n");
  printf("=================\n\n");

  memset(rrName, 0, DNS_DOMAIN_NAME_SIZE_MAX);
  rrType = 0;
  while (dnsClientCacheRRNextGet(rrName, &rrType, &rrIndex, &rrClass, qname) == L7_SUCCESS)
  {
    L7_uint32 total = 0;
    L7_uint32 elapsed = 0;
    L7_BOOL   primary = L7_FALSE;
    L7_char8  hostname[DNS_DOMAIN_NAME_SIZE_MAX + 1];
    L7_uchar8 prettyName[DNS_DOMAIN_NAME_SIZE_MAX];
    L7_uchar8 data[DNS_DOMAIN_NAME_SIZE_MAX];
    L7_inet_addr_t source;
    L7_uchar8      inetPrintBuf[IPV6_DISP_ADDR_LEN];

    if (dnsClientCacheRRPrettyNameGet(rrName, rrType, rrIndex, prettyName) != L7_SUCCESS)
    {
      printf("dnsClientCacheRRPrettyNameGet failed\n");
      return;
    }

    if (dnsClientDisplayNameGet(prettyName, hostname) != L7_SUCCESS)
    {
      printf("dnsClientDisplayNameGet failed\n");
      return;
    }

    if (dnsClientCacheRRTTLGet(rrName, rrType, rrIndex, &total) != L7_SUCCESS)
    {
      printf("dnsClientCacheRRTTLGet failed\n");
      return;
    }

    if (dnsClientCacheRRTTLElapsedGet(rrName, rrType, rrIndex, &elapsed) != L7_SUCCESS)
    {
      printf("dnsClientCacheRRTTLElapsedGet failed\n");
      return;
    }

    printf("Host:%s TTL: %i Elapsed: %i ", hostname, total, elapsed);

    if (dnsClientCacheRRDataGet(rrName, rrType, rrIndex, data, &primary) != L7_SUCCESS)
    {
      printf("dnsClientCacheRRDataGet failed\n");
      return;
    }

    if (rrType == DNS_RR_TYPE_CNAME)
    {
      L7_char8 cname[DNS_DOMAIN_NAME_SIZE_MAX + 1];
      if (dnsClientDisplayNameGet(data, cname) != L7_SUCCESS)
      {
        printf("dnsClientDisplayNameGet failed\n");
        return;
      }
      printf("cname: %s ", cname);
    }
    else
    {
      if (primary == L7_TRUE)
      {
        printf("ip: *%s", inetAddrPrint((L7_inet_addr_t *)data, inetPrintBuf));
      }
      else
      {
        printf("ip: %s", inetAddrPrint((L7_inet_addr_t *)data, inetPrintBuf));
      }
    }

    if (dnsClientCacheRRSourceGet(rrName, rrType, rrIndex, &source) != L7_SUCCESS)
    {
      printf("dnsClientCacheRRSourceGet failed\n");
      return;
    }
    printf("source: %s\n", inetAddrPrint(&source, inetPrintBuf));
  }

  printf("\n===============\n");

  return;
}

/*********************************************************************
* @purpose  Print DNS operational counters for IPv4 and IPv6
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCountersDump(void)
{
  printf("\n");
  printf("DNS IPv4 Counters\n");
  printf("=================\n\n");
  printf("\nIPv4 Queries: %d", dnsOprData->counters.v4queries);
  printf("\nIPv4 Responses: %d", dnsOprData->counters.v4responses);

  printf("\n");
  printf("DNS IPv6 Counters\n");
  printf("=================\n\n");
  printf("\nIPv6 Queries: %d", dnsOprData->counters.v6queries);
  printf("\nIPv6 Responses: %d", dnsOprData->counters.v6responses);
  return;
}

#define DNS_TRACE_LEN_MAX 1024
/*********************************************************************
* @purpose  Print DNS Request values to serial port
*
* @param    L7_inet_addr_t   srvAddr   @b((input))  v4 or v6 address of
*                                                   server
* @param    L7_uchar8       *packet    @b((input))  Request packet
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
void dnsRequestPacketTrace(L7_inet_addr_t *srvAddr, L7_uchar8 *packet)
{
 dnsMessageHeader_t *header = (dnsMessageHeader_t *)packet;
 L7_uchar8 *p = L7_NULLPTR;
 L7_uchar8 destStr[DNS_INET_ADDR_LEN];
 L7_uchar8 traceBuf[DNS_TRACE_LEN_MAX];
 L7_char8 hostname[DNS_DOMAIN_NAME_SIZE_MAX+1];
 dnsDomainName_t domainName;
 L7_uint32 length = 0;
 L7_inet_addr_t hostSrvAddr;


 inetAddressReset(&hostSrvAddr);
 memset(traceBuf, '\0', DNS_TRACE_LEN_MAX);

 if(dnsTraceFlag == L7_DISABLE)
 {
   return;
 }
 p = packet + sizeof(dnsMessageHeader_t);
 while ((*p) != 0)
 {
    length += (*p) + 1;
    p += (*p) + 1;
 }
 length = length + 1;
 p = packet + sizeof(dnsMessageHeader_t);

 memcpy( domainName.labels, p, length);
 dnsInternalNameToStringConvert(&domainName, L7_FALSE, hostname);
 if (inetAddrNtoh(srvAddr, &hostSrvAddr) != L7_SUCCESS)
 {
   L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
           "\r\nUnable to convert network to host order");
   return;
 }
 inetAddrHtop(&hostSrvAddr, destStr);
 sprintf(traceBuf, "\r\n Sending DNS Request to Server IP %s with id %u hostname %s",
               destStr, osapiNtohs(header->id), hostname);
 printf("%s\n", traceBuf);

 return;
}

/*********************************************************************
* @purpose  Print DNS Response values to serial port
*
* @param    L7_inet_addr_t serverAddr        @b((input))  ipv4 or ipv6
*                                                 address of server
* @param    L7_uchar8     *packet    @b((input))  Response packet
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
void dnsResponsePacketTrace(L7_inet_addr_t *serverAddr, L7_uchar8 *packet)
{
 dnsMessageHeader_t       *header = (dnsMessageHeader_t *)packet;
 L7_uchar8                *p = L7_NULLPTR, *q = L7_NULLPTR;
 L7_uchar8                destStr[DNS_INET_ADDR_LEN];
 L7_uchar8                ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
 L7_uchar8                traceBuf[DNS_TRACE_LEN_MAX];
 L7_char8                 hostname[DNS_DOMAIN_NAME_SIZE_MAX+1];
 L7_char8                 query[DNS_DOMAIN_NAME_SIZE_MAX+1];
 L7_uint32                ipAddr = 0;
 L7_uchar8                rcode[32];
 L7_uint32                anIndex = 0;
 L7_uint32                length = 0;
 dnsDomainName_t          anName, qName;
 L7_uint32                ttl = 0;
 L7_inet_addr_t hostSrvAddr;


 inetAddressReset(&hostSrvAddr);

 memset(traceBuf, '\0', DNS_TRACE_LEN_MAX);

 if(dnsTraceFlag == L7_DISABLE)
 {
   return;
 }
 if (inetAddrNtoh(serverAddr, &hostSrvAddr) != L7_SUCCESS)
 {
   L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
           "\r\nUnable to convert network to host order");
   return;
 }
 inetAddrHtop(&hostSrvAddr, destStr);
 memset(rcode, '\0', 32);
 if(dnsDebugGetResponsecode(header->rcode, rcode) != 0)
 {
   sprintf(traceBuf, "\r\n Received DNS Response from Server with IP %s with id %u %s\n",
               destStr, header->id, rcode);
   printf("%s", traceBuf);
   return;

 }

 p = packet + sizeof(dnsMessageHeader_t);
 q = packet + sizeof(dnsMessageHeader_t);
 /* read past the question section */
 while ((*p) != 0)
 {
    length += (*p) + 1;
    p += (*p) + 1;
 }
 p++;
 p +=4;

 memcpy(qName.labels, q, length + 1);
 dnsInternalNameToStringConvert(&qName, L7_FALSE, query);
 while(((*p) != 0) &&
         (anIndex < osapiNtohs(header->ancount)))
 {
   L7_uchar8       type = 0;
   L7_ushort16     rdlength = 0;
   L7_BOOL         skip = L7_FALSE;
   L7_uint32       temp = 0;
   if (dnsPacketRRNameRead(packet, &p, 0, &anName) != L7_SUCCESS)
   {
     sprintf(traceBuf, "\r\n Received DNS Response from Server with IP %s for hostname %s with id %u %s no: of answers received %d Unable to parse answer section",
               destStr, query, osapiNtohs(header->id), rcode, osapiNtohs(header->ancount));
     printf("%s", traceBuf);
     return;
   }
   dnsInternalNameToStringConvert(&anName, L7_FALSE, hostname);
   /* read resource record type */
   type = osapiNtohs(*(L7_ushort16 *)p);
   p += sizeof(L7_ushort16);

   /* class should always be DNS_RR_CLASS_INTERNET */
   if ((*(L7_ushort16 *)p) != osapiNtohs(DNS_RR_CLASS_INTERNET))
   {
     sprintf(traceBuf, "\r\n Received DNS Response from Server with IP %s for hostname %s with id %u %s no: of answers received %d Unable to parse answer section",
               destStr, query, osapiNtohs(header->id), rcode, osapiNtohs(header->ancount));
     printf("%s", traceBuf);
     return ;
   }
   p += sizeof(L7_ushort16);

   memcpy(&temp, p, sizeof(L7_uint32));
   ttl = osapiNtohl(temp);
   p += sizeof(L7_uint32);
   rdlength = osapiNtohs(*(L7_ushort16 *)p);
   p += sizeof(L7_ushort16);

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
      if (skip == L7_TRUE ||
         ((type != DNS_RR_TYPE_ADDRESS) && (type != DNS_RR_TYPE_IPV6_ADDRESS) && (type != DNS_RR_TYPE_CNAME)))
#else
      if (skip == L7_TRUE ||
          ((type != DNS_RR_TYPE_ADDRESS) &&(type != DNS_RR_TYPE_CNAME)))
#endif

    {
      /* record count of answers that we skip over */
      p += rdlength;
      continue;
    }

    /* read data for cname and address records */
    if (type == DNS_RR_TYPE_CNAME)
    {
      continue;
    }
    else
    {
    /* DNS_RR_TYPE_ADDRESS */
      if (rdlength != sizeof(L7_uint32))
      {
        return;
      }
      temp = 0;
      memcpy (&temp, p, sizeof(L7_uint32));
      ipAddr = temp;
      p += rdlength;
      osapiInetNtoa(ipAddr, ipAddrStr);
      sprintf(traceBuf, "\r\n Received DNS Response from Server with IP %s for hostname %s \r\nwith id %u %s no: of answers received %d for host name %s ipaddr %s\n",
               destStr, query, osapiNtohs(header->id), rcode, osapiNtohs(header->ancount), hostname, ipAddrStr);
     printf("%s", traceBuf);
     memset(hostname, '\0',DNS_DOMAIN_NAME_SIZE_MAX+1);
    }
    anIndex++;
  }

 return;
}

/*********************************************************************
* @purpose  Get DNS Response code value in text format
*
* @param    L7_uchar8     rcode     @b((input))  enum for rcode
* @param    L7_uchar8     *val      @b((input))  Response code in text format
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
static L7_int32 dnsDebugGetResponsecode(L7_uchar8 rcode, L7_uchar8 *val)
{
  L7_int32 retval = -1;
   /* look at the response code */
  switch (rcode & DNS_MSG_HDR_RCODE_BIT_MASK)
  {
    case DNS_RCODE_NO_ERROR:
      strcpy(val, "Response code NO_ERROR");
      retval = 0;
      break;
    case DNS_RCODE_NAME_ERROR:
      strcpy(val, "Response code NAME_ERROR");
      break;
    case DNS_RCODE_FORMAT_ERROR:
      strcpy(val, "Response code FORMAT_ERROR");
      break;
    case DNS_RCODE_SERVER_FAILURE:
      strcpy(val, "Response code SERVER_FAILURE");
      break;
    case DNS_RCODE_NOT_IMPLEMENTED:
      strcpy(val, "Response code NOT_IMPLEMENTED");
      break;
    case DNS_RCODE_REFUSED:
      strcpy(val, "Response code REFUSED");
      break;
    default:
      strcpy(val, "Response code UNKNOWN");
      break;
  }
  return retval;
}

/*********************************************************************
* @purpose  Get DNS Client tracing mode
*
* @param    void
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dnsClientDebugModeGet(void)
{
  return(dnsDebugFlag);
}

/*********************************************************************
* @purpose  Set DNS Client tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dnsClientDebugModeSet(L7_uint32 mode)
{
  if(mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  dnsDebugFlag = mode;
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Record a dns event trace if trace configuration
*           permits
*
* @param    format      @b{(input)} format string
* @param    ...         @b{(input)} additional arguments (per format
*           string)
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dnsDebugTrace(L7_char8 * format, ...)
{
  va_list arg;

  if (dnsDebugFlag == L7_DISABLE)
  {
    return;
  }

  va_start (arg, format);
  vprintf(format, arg);
  va_end (arg);
}


/*********************************************************************
* @purpose  Perform reverse lookup of a hotname for a specified ip.
*
* @param    L7_uchar8   *ipStr   @b((input))  ip address in string format
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dnsDebugReverseLookup(L7_uchar8 *ipStr)
{
  L7_inet_addr_t          inetIp;
  L7_char8                fqhostname[L7_DNS_DOMAIN_NAME_SIZE_MAX] = "";
  L7_char8                hostname[L7_DNS_DOMAIN_NAME_SIZE_MAX]   = "";
  dnsClientLookupStatus_t status;

  inetAddressZeroSet(L7_AF_INET, &inetIp);
  if (ipStr == L7_NULLPTR)
  {
    sysapiPrintf("\n[DNS client]: Invalid IP parameter \n");
    return L7_FAILURE;
  }

  if (osapiInetPton(L7_AF_INET, ipStr, (L7_uchar8 *)(&inetIp.addr.ipv4.s_addr)) != L7_SUCCESS)
  {
    sysapiPrintf("\n[DNS client]: Failed to convert string IP (%s) to uint32 \n", ipStr);
    return L7_FAILURE;
  }

  if (dnsClientReverseNameLookup(inetIp, hostname, &status, fqhostname) != L7_SUCCESS)
  {
    sysapiPrintf("\n[DNS client]: Reverse lookup failed IP ( %s ) \n", ipStr);
    return L7_FAILURE;
  }


  sysapiPrintf("\nIP( %s )\n", ipStr);
  sysapiPrintf("\nHOSTNAME( %s )\n", hostname);
  sysapiPrintf("\nFQDN( %s )\n", fqhostname);

  return L7_SUCCESS;
}

/* Test function to add few Dynamic RR entries to the cache */
void dnsDebugRRTestAdd()
{
  dnsDomainName_t     qName;
  dnsDomainName_t     anName;
  L7_inet_addr_t      inetAddrList[DNS_CACHE_ENTRY_IP_ADDRESSES];
  L7_uint32 addrIndex;
  L7_uint32 ipv4Addr1, ipv4Addr;
  L7_in6_addr_t ipv6Addr1;
  L7_in6_addr_t ipv6Addr2;
  L7_uchar8 *addr1 = "2003::6";
  L7_uchar8 *addr2 = "2010::6";
  L7_uchar8 *addr3 = "2022::6";
  L7_uchar8 *addr4 = "3036::9";
  for (addrIndex = 0; addrIndex < DNS_CACHE_ENTRY_IP_ADDRESSES; addrIndex++)
  {
    inetAddressZeroSet(L7_AF_INET6, &inetAddrList[addrIndex]);
  }

  osapiInetPton(L7_AF_INET6, (L7_uchar8 *)addr1, (L7_uchar8 *)&ipv6Addr1);
  inetAddressSet(L7_AF_INET6, &ipv6Addr1, &inetAddrList[0]);

  memset(&qName, 0, sizeof(qName));
  memset(&anName, 0, sizeof(anName));
  memcpy(&qName.labels[1], "google", 6);
  memcpy(&anName.labels[1], "google", 6);
  qName.labels[0] = 6;
  anName.labels[0] = 6;
  dnsCacheEntryAdd(&qName, &anName, DNS_RR_TYPE_IPV6_ADDRESS, 60000, &inetAddrList[0], inetAddrList);

  for (addrIndex = 0; addrIndex < DNS_CACHE_ENTRY_IP_ADDRESSES; addrIndex++)
  {
    inetAddressZeroSet(L7_AF_INET6, &inetAddrList[addrIndex]);
  }

  osapiInetPton(L7_AF_INET6, (L7_uchar8 *)addr2, (L7_uchar8 *)&ipv6Addr2);
  inetAddressSet(L7_AF_INET6, &ipv6Addr2, &inetAddrList[0]);

  memset(&qName, 0, sizeof(qName));
  memset(&anName, 0, sizeof(anName));
  memcpy(&qName.labels[1], "yahoo", 5);
  memcpy(&anName.labels[1], "yahoo", 5);
  qName.labels[0] = 5;
  anName.labels[0] = 5;
  dnsCacheEntryAdd(&qName, &anName, DNS_RR_TYPE_IPV6_ADDRESS, 60000, &inetAddrList[0], inetAddrList);

  for (addrIndex = 0; addrIndex < DNS_CACHE_ENTRY_IP_ADDRESSES; addrIndex++)
  {
    inetAddressZeroSet(L7_AF_INET, &inetAddrList[addrIndex]);
  }

  addr2 = "10.123.45.6";
  osapiInetPton(L7_AF_INET, (L7_uchar8 *)addr2, (L7_uchar8 *)&ipv4Addr1);
  ipv4Addr = osapiNtohl(ipv4Addr1);
  inetAddressSet(L7_AF_INET, &ipv4Addr, &inetAddrList[0]);

  memset(&qName, 0, sizeof(qName));
  memset(&anName, 0, sizeof(anName));
  memcpy(&qName.labels[1], "google", 6);
  memcpy(&anName.labels[1], "google", 6);
  qName.labels[0] = 6;
  anName.labels[0] = 6;
  dnsCacheEntryAdd(&qName, &anName, DNS_RR_TYPE_ADDRESS, 50000, &inetAddrList[0], inetAddrList);

  for (addrIndex = 0; addrIndex < DNS_CACHE_ENTRY_IP_ADDRESSES; addrIndex++)
  {
    inetAddressZeroSet(L7_AF_INET, &inetAddrList[addrIndex]);
  }

  osapiInetPton(L7_AF_INET6, (L7_uchar8 *)addr3, (L7_uchar8 *)&ipv6Addr2);
  inetAddressSet(L7_AF_INET6, &ipv6Addr2, &inetAddrList[0]);

  osapiInetPton(L7_AF_INET6, (L7_uchar8 *)addr4, (L7_uchar8 *)&ipv6Addr2);
  inetAddressSet(L7_AF_INET6, &ipv6Addr2, &inetAddrList[1]);

  memset(&qName, 0, sizeof(qName));
  memset(&anName, 0, sizeof(anName));
  memcpy(&qName.labels[1], "wazee", 5);
  memcpy(&anName.labels[1], "wazee", 5);
  qName.labels[0] = 5;
  anName.labels[0] = 5;
  dnsCacheEntryAdd(&qName, &anName, DNS_RR_TYPE_IPV6_ADDRESS, 50000, &inetAddrList[0], inetAddrList);
  addr2 = "11.11.11.11";
  osapiInetPton(L7_AF_INET, (L7_uchar8 *)addr2, (L7_uchar8 *)&ipv4Addr1);
  ipv4Addr = osapiNtohl(ipv4Addr1);
  inetAddressSet(L7_AF_INET, &ipv4Addr, &inetAddrList[0]);

  addr2 = "51.61.71.81";
  osapiInetPton(L7_AF_INET, (L7_uchar8 *)addr2, (L7_uchar8 *)&ipv4Addr1);
  ipv4Addr = osapiNtohl(ipv4Addr1);
  inetAddressSet(L7_AF_INET, &ipv4Addr, &inetAddrList[1]);

  memset(&qName, 0, sizeof(qName));
  memset(&anName, 0, sizeof(anName));
  memcpy(&qName.labels[1], "goto", 4);
  memcpy(&anName.labels[1], "goto", 4);
  qName.labels[0] = 4;
  anName.labels[0] = 4;
  dnsCacheEntryAdd(&qName, &anName, DNS_RR_TYPE_ADDRESS, 76000, &inetAddrList[0], inetAddrList);
}

