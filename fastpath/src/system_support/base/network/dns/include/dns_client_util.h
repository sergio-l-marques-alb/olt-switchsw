/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client_util.h
*
* @purpose DNS client utility function prototypes
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
#ifndef DNS_CLIENT_UTIL_H
#define DNS_CLIENT_UTIL_H


L7_RC_t dnsClientAdminModeApply(L7_BOOL enable);

L7_int32 dnsNameCompare(dnsDomainName_t *name1, dnsDomainName_t *name2);

L7_RC_t dnsInternalNameToStringConvert(dnsDomainName_t  *name,
                                       L7_BOOL           uppercase,
                                       L7_char8         *strName);

L7_RC_t dnsStringNameToInternalConvert(L7_char8        *strName, 
                                       L7_BOOL          lowercase,
                                       dnsDomainName_t *name);

void dnsInternalNameToCaseConvert(dnsDomainName_t *name,
                                  L7_uchar8       *labels);

void dnsCaseNameToInternalConvert(L7_uchar8       *labels, 
                                  dnsDomainName_t *name);

void dnsDefaultDomainNameSearchListGenerate(void);     

void dnsDomainNameListSearchListGenerate(void);

void dnsAddDomainNameToSearchList(L7_char8 *domainname);

void dnsRemoveDomainNameFromSearchList(L7_char8 *domainname);

L7_RC_t dnsRequestGet(L7_uchar8           family,
                      L7_COMPONENT_IDS_t  componentId,
                      L7_uint32           requestId,
                      L7_char8           *hostname,
                      dnsRequestEntry_t **request);
L7_RC_t dnsReverseRequestGet(L7_COMPONENT_IDS_t  componentId,
                             L7_uint32           requestId,
                             L7_inet_addr_t      inetIp,
                             dnsRequestEntry_t **request);

void dnsRequestProcess(dnsRequestEntry_t *request);

void dnsReverseRequestProcess(dnsRequestEntry_t *request);

void dnsResponseProcess(dnsRequestEntry_t *request);
void dnsRequestQueryNextServer(dnsRequestEntry_t *request, 
                               dnsClientLookupStatus_t status);
void dnsTimerProcess(void);
void dnsTimerCallback();

L7_RC_t dnsCfgDump(void);
void dnsCacheDump(void);

L7_RC_t dnsNameLookupResponseMessageSend(dnsClientLookupStatus_t status, 
                                         L7_ushort16             requestId,
                                         L7_uchar8               recordType,
                                         L7_char8               *hostname,
                                         L7_inet_addr_t         *inetAddr);

void dnsRequestPacketTrace(L7_inet_addr_t *srvAddr, L7_uchar8 *packet);
void dnsResponsePacketTrace(L7_inet_addr_t *serverAddr, L7_uchar8 *packet);
L7_BOOL dnsClientIsInetAddressZero(L7_inet_addr_t *inetAddr);
L7_BOOL dnsClientIsInetAddrFamilyNotSet(L7_inet_addr_t *inetAddr);
L7_RC_t dnsApplyConfigCompleteCallback(L7_uint32 event);
#endif /* DNS_CLIENT_UTIL_H */
