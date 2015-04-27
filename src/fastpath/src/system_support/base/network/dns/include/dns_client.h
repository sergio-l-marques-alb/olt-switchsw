/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client.h
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
#ifndef DNS_CLIENT_H
#define DNS_CLIENT_H

#include "l7_common.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "dns_client_api.h"
#include "dns_client_packet.h"
#include "l7_cnfgr_api.h"

#define DNS_CLIENT_QUEUE		    "dns_client_Queue"
#define DNS_CLIENT_MSG_COUNT	    1024
#define DNS_CLIENT_MSG_SIZE         sizeof(dnsMgmtMsg_t)

#define DNS_NAME_LOOKUP_QUEUE       "dns_lookup_Queue"
#define DNS_NAME_LOOKUP_MSG_COUNT   1        /* dnsNameLookup API will block for each message */
#define DNS_NAME_LOOKUP_MSG_SIZE    sizeof(dnsNameLookupMsg_t)

#define DNS_CLIENT_CFG_FILENAME     "dnsCfgData.cfg"
#define DNS_CLIENT_CFG_VER_1        0x1
#define DNS_CLIENT_CFG_VER_CURRENT  DNS_CLIENT_CFG_VER_1 

#define DNS_CLIENT_IS_READY (((dnsCnfgrState == DNS_CLIENT_PHASE_INIT_3) || \
					          (dnsCnfgrState == DNS_CLIENT_PHASE_EXECUTE) || \
						      (dnsCnfgrState == DNS_CLIENT_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

#define DNS_CLIENT_IMPLEMENT_ID             "DNS-Client-1.0"
#define DNS_CLIENT_SERVICE_RECURSIVE_ONLY   1
#define DNS_NAME_LABEL_SIZE_MAX             63
#define DNS_NAME_LABELS_SIZE_MAX            DNS_DOMAIN_NAME_SIZE_MAX
#define DNS_NAME_CASE_BITS_SIZE_MAX         32

#define DNS_UDP_PORT                    53
#define DNS_TRANSMITS_DEFAULT_PER_SERVER    2     /* transmits before trying next server */
#define DNS_TRANSMITS_PER_SERVER_MAX        100     /* transmits before trying next server */
#define DNS_TIMER_INTERVAL              1000  /* wake up every second when there are pending requests */

#define DNS_REQUEST_SEARCH_LIST_ENTRIES (L7_DNS_SEARCH_LIST_ENTRIES+1)
#define DNS_AF_BOTH	    (L7_AF_INET|L7_AF_INET6)

typedef enum {
  DNS_CLIENT_PHASE_INIT_0 = 0,
  DNS_CLIENT_PHASE_INIT_1,
  DNS_CLIENT_PHASE_INIT_2,
  DNS_CLIENT_PHASE_WMU,
  DNS_CLIENT_PHASE_INIT_3,
  DNS_CLIENT_PHASE_EXECUTE,
  DNS_CLIENT_PHASE_UNCONFIG_1,
  DNS_CLIENT_PHASE_UNCONFIG_2,
} dnsCnfgrState_t;

typedef struct
{
  L7_uchar8             labels[DNS_NAME_LABELS_SIZE_MAX];
  L7_uchar8             uppercase[DNS_NAME_CASE_BITS_SIZE_MAX];
} dnsDomainName_t;

typedef struct
{
  dnsDomainName_t       hostname;
  L7_inet_addr_t        inetAddr;
} dnsStaticHostEntry_t;

typedef struct 
{
  L7_fileHdr_t          cfgHdr;    
  L7_BOOL               enabled;        /* global admin mode */
  L7_ushort16           queryTimeout;   /* per query timeout in seconds */
  L7_ushort16           requestTimeout; /* total request timeout in seconds */
  L7_uint32             retries;        /* total number of retries*/
  L7_char8              domain[DNS_DOMAIN_NAME_SIZE_MAX + 1];  /* will convert to labels in search list. This is default domain list*/
  L7_char8              domainList[L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES][DNS_DOMAIN_NAME_SIZE_MAX + 1];  /* will convert to labels and do search on this list */
  L7_inet_addr_t        servers[L7_DNS_NAME_SERVER_ENTRIES]; /* server ip (both ipv4 & ipv6)*/
  dnsStaticHostEntry_t  hosts[L7_DNS_STATIC_HOST_ENTRIES];
  L7_uint32             checkSum;
} dnsCfgData_t;

typedef struct
{
  L7_RC_t            (*notifyFunction)(dnsClientLookupStatus_t status, 
                                       L7_ushort16  requestId,
                                       L7_uchar8    family,
                                       L7_char8    *hostname,
                                       L7_inet_addr_t    *inetAddr);
} dnsNotifyEntry_t;

typedef struct
{
  L7_inet_addr_t        serverAddr; /* server v4 or v6 address */
  L7_uint32             socket;
  L7_uchar8             requests;          /* number of pending requests for this server */
} dnsNameServerEntry_t;

/* DNS Client Request Type */
typedef enum
{
  dnsDirectRequest = 0,
  dnsReverseRequest
} dnsRequestType_t;

typedef struct
{
  L7_ushort16           id;                /* used to match response packet, increments from 1 at reset */
  L7_COMPONENT_IDS_t    componentId;       /* request component */
  L7_ushort16           requestId;         /* component generated request id */
  dnsRequestType_t      requestType;
  L7_ushort16           requestTimer;      /* seconds until request timeout */
  L7_ushort16           queryTimer;        /* seconds until current query timeout */
  L7_uint32             retries;          /* number of retries */
  dnsDomainName_t       nameList[L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES];  /* possible qualified hostnames */
  L7_uchar8             nameIndex;         /* nameList index for current query */
  L7_inet_addr_t        server;            /* current server address */
  L7_uchar8             packet[DNS_UDP_MSG_SIZE_MAX];   /* request/response packet */
  L7_ushort16           length;            /* packet length */
  L7_uchar8             transmits;         /* number of transmissions to current server */
  L7_char8              hostName[L7_DNS_HOST_NAME_LEN_MAX]; /* the original host name received by the dns resolver */
  L7_uchar8             family;            /* L7_AF_INET or L7_AF_INET6 */
  L7_uchar8             recordType;        /* A (IPv4) or AAAA (IPv6) */
  L7_inet_addr_t        inetIpAddr;         /* an ip addres to resolve hostname in reverse requests */
} dnsRequestEntry_t;

typedef struct
{
  L7_uint32             v4queries;
  L7_uint32             v6queries;
  L7_uint32             v4responses;
  L7_uint32             v6responses;
  L7_uint32             rcodes[DNS_RCODE_RESERVED];   /* response count for each rcode */   
  L7_uint32             nonAA;           /* non-authoritative responses */
  L7_uint32             nonAANoData;     /* non-authoritative reponses with no data */
  L7_uint32             martians;        /* no matching request or server */
  L7_uint32             unparsed;        /* failed to parse responses */
  L7_uint32             goodCaches;      /* RRs successfully cached */
  L7_uint32             badCaches;       /* RRs that were not relavant */
} dnsCounters_t;

typedef struct
{
  dnsRequestEntry_t     requestTbl[L7_DNS_CONCURRENT_REQUESTS];
  dnsNameServerEntry_t  serverTbl[L7_DNS_NAME_SERVER_ENTRIES];
  L7_char8              searchTbl[L7_DNS_DOMAIN_SEARCH_LIST_ENTRIES][DNS_DOMAIN_NAME_SIZE_MAX];
  dnsCounters_t         counters;
} dnsOprData_t;

/* DNS Client Management Message IDs */
typedef enum
{
  dnsMsgCnfgr = 1,
  dnsMsgRequest = 2,
  dnsMsgResponse = 3,
  dnsMsgTimer = 4,
  dnsMsgServerRemove = 5,
  dnsMsgReverseRequest = 6
} dnsMgmtMessages_t;

typedef struct
{
  L7_uint32 msgId;    /* Of type dnsMgmtMessages_t */
  union {
    L7_CNFGR_CMD_DATA_t CmdData;
    dnsRequestEntry_t  *request;
    L7_inet_addr_t      address; /* ipv4 or ipv6 address */
  } u;
} dnsMgmtMsg_t;

typedef struct
{
  L7_uint32               msgId;    /* 0 - only one message type */
  dnsClientLookupStatus_t status;
  L7_char8                hostFQDN[DNS_DOMAIN_NAME_SIZE_MAX];
  L7_inet_addr_t          inetAddr;
  L7_uchar8               recordType;
} dnsNameLookupMsg_t;

/*********************************************************************
*
* @purpose  System Initialization for DNS client component
*
* @param    none
*                            
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dnsInit(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  System Init Undo for DNS client component
*
* @param    none
*                            
* @notes    none
*
* @end
*********************************************************************/
void dnsInitUndo();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*       
* @end
*********************************************************************/
L7_RC_t dnsCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* 
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t dnsCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t dnsCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function undoes dns_clientCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes dns_clientCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes dns_clientCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCnfgrFiniPhase3Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse - @b{(output)}  Response always command complete.
*     
* @param    pReason   - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
L7_RC_t dnsCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/
L7_RC_t dnsCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
*
* @purpose  To parse the configurator commands send to dns_clientTask
*
* @param    none
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void dnsCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  Saves DNS client configuration file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t dnsSave(void);

/*********************************************************************
* @purpose  Checks if DNS client configuration data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dnsHasDataChanged(void);
void dnsResetDataChanged(void);

/*********************************************************************
* @purpose  Apply static DNS client Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dnsApplyConfigData(void);

/*********************************************************************
* @purpose  Build default DNS client configuration data  
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    none 
*       
* @end
*********************************************************************/
void dnsBuildDefaultConfigData(L7_uint32 version);

/*********************************************************************
* @purpose  Start DNS client task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dnsTaskStart();

/*********************************************************************
*
* @purpose Task to handle all DNS client management messages
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
void dnsTask();

/*********************************************************************
*
* @purpose Task to receive DNS query responses
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
void dnsRxTask();

/*********************************************************************
*
* @functions dnsInfoShow
*
* @purpose   Displays all config and nonconfig info for DNS client
*
* @param     none
*
* @returns   L7_SUCCESS
*
* @comments  Used for debug
*
* @end
*
*********************************************************************/
L7_RC_t dnsInfoShow();

#endif /* DNS_CLIENT_H */

