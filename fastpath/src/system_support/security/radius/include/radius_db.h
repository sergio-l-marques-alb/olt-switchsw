/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_db.h
*
* @purpose RADIUS Client data structures
*
* @component radius
*
* @comments none
*
* @create 03/28/2003
*
* @author spetriccione
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_RADIUS_DB_H
#define INCLUDE_RADIUS_DB_H

#ifdef L7_ROUTING_PACKAGE || L7_RLIM_PACKAGE
#define L7_RADIUS_ROUTING_LOOPBACK_SELECTED 
#endif


#ifdef  L7_RADIUS_ROUTING_LOOPBACK_SELECTED 
#define L7_RADIUS_MAX_NETWORK_INTERFACES (L7_RTR_MAX_RTR_INTERFACES+L7_MAX_NUM_LOOPBACK_INTF)
#endif


#define L7_RADIUS_MAX_AUTH_SERVERS    FD_RADIUS_MAX_AUTH_SERVERS
#define L7_RADIUS_MAX_ACCT_SERVERS    FD_RADIUS_MAX_ACCT_SERVERS
#define L7_RADIUS_MAX_SERVERS         L7_RADIUS_MAX_AUTH_SERVERS + L7_RADIUS_MAX_ACCT_SERVERS
#define L7_RADIUS_VALID_SERVER_WAIT   1 /* seconds to wait for a valid server configuration */
#define L7_RADIUS_NAS_IDENTIFIER_SIZE 17

#define RADIUS_MAX_TOKEN_SIZE 48

/*
** RADIUS authenticator and client secret field sizes
*/
#define RADIUS_PASSWORD_BLOCK_LENGTH     16
#define RADIUS_MAX_MESSAGE_LENGTH      4096
#define MIN_RADIUS_VSA_LENGTH             6 /* VS attr length = 1 + 1 + 4 */
#define RADIUS_VS_CODE_VALUE_LENGTH       4

/*
** Maximum password length allowed and above which ignored
*/
#define RADIUS_MAX_PASSWORD_LENGTH      64
#define RADIUS_MAX_USERNAME_LENGTH      L7_LOGIN_SIZE
#define L7_RADIUS_MAX_OCTET_STRING_SIZE 2048

/*
** RADIUS server rowstatus types
*/
typedef enum
{
  RADIUS_SERVER_NOTREADY = 0,
  RADIUS_SERVER_ACTIVE,
  RADIUS_SERVER_DESTROY,
  RADIUS_SERVER_NOTCONFIGURED

} radiusServerRowStatus_t;

/*
** RADIUS update global servers (ProcessRadiusConfigChange) types
*/
typedef enum
{
  RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH = 0,
  RADIUS_CONFIG_EVENT_TYPE_MODIFY_ACCT

} radiusUpdateCurrent_t;

/*
** Radius server configuration values
*
typedef enum
{
  RADIUS_SERVER_TYPE_UNKNOWN = 0, 
  RADIUS_SERVER_TYPE_AUTH,
  RADIUS_SERVER_TYPE_ACCT

} radiusServerType_t;
*/

/* Configuration mask values for the radius server */
typedef enum
{
  L7_RADIUS_SERVER_CONFIG_MAX_NUM_RETRANS   = (1 << 0),
  L7_RADIUS_SERVER_CONFIG_TIMEOUT           = (1 << 1),
  L7_RADIUS_SERVER_CONFIG_DEAD_TIME         = (1 << 2),
  L7_RADIUS_SERVER_CONFIG_SOURCE_IP_ADDRESS = (1 << 3),
  L7_RADIUS_SERVER_CONFIG_SECRET            = (1 << 4)
} RADIUS_SERVER_CONFIG_PARAMS_t;

/* 
 * Bit Positions of radius parameters in config mask 
 */
#define RADIUS_CONFIG_MAX_NUM_RETRANS_BIT_POS    0
#define RADIUS_CONFIG_TIMEOUT_BIT_POS            1
#define RADIUS_CONFIG_DEAD_TIME_BIT_POS          2
#define RADIUS_CONFIG_SOURCE_IP_ADDRESS_BIT_POS  3
#define RADIUS_CONFIG_SECRET_BIT_POS             4

/*
** The server socket status indicators
*/
#define RADIUS_SERVER_STATUS_DOWN  0
#define RADIUS_SERVER_STATUS_UP    1
#define RADIUS_SERVER_STATUS_UP_READING 2

/* Message codes */
#define RADIUS_CODE_ACCESS_REQUEST   1
#define RADIUS_CODE_ACCESS_ACCEPT    2
#define RADIUS_CODE_ACCESS_REJECT    3
#define RADIUS_CODE_ACCOUNT_REQUEST  4
#define RADIUS_CODE_ACCOUNT_RESPONSE 5
#define RADIUS_CODE_ACCESS_CHALLENGE 11

#define RADIUS_CODE_START            RADIUS_CODE_ACCESS_REQUEST
#define RADIUS_CODE_END              RADIUS_CODE_ACCESS_CHALLENGE

#define RADIUS_SERVER_PRIORITY_DEFAULT        0

#define L7_RADIUS_AUTH_SERVER_NAME_INDEX_MIN 1
#define L7_RADIUS_ACCT_SERVER_NAME_INDEX_MIN 1

/*
** The RADIUS Authentication/Accounting information data structure which
** contains the information as provided by the requesting client for
** authenticating node/user and accounting the resoure usage.
*/
typedef struct {
   L7_uint32         requestType;
   L7_COMPONENT_IDS_t   componentID;
   L7_uint32         correlator;
   L7_uchar8         userName[RADIUS_MAX_USERNAME_LENGTH];
   L7_uint32         userNameLength;
   L7_uchar8         userPassword[RADIUS_MAX_PASSWORD_LENGTH];
   L7_uint32         userPasswordLength;
   L7_uint32         acctStatusType;
   L7_uint32         serviceType;
   L7_uchar8         state[RADIUS_VALUE_LENGTH];
   L7_uint32         stateLength;
   L7_uint32         status;
   radiusValuePair_t *vpList;
   L7_uint32         serverNameIndex;/*To index the configured Name of the server 
                                        in (auth/acct)ServerNameArray[] 
                                       Non-zero indicates indexing to an entry*/
   L7_uint32         specificSocket; /*Socket bound to local address.*/
   L7_BOOL           tryAllServers; /* Try all servers.*/
}  radiusRequestInfo_t;

/*
** The RADIUS Authentication (Accounting) request data structure which contains
** a request (authentication/accounting) made by the clients. It contains all the
** information for the request, links to the next and previous requests on the
** chain, etc.
*/
typedef struct radiusRequest_s {
   struct radiusRequest_s *nextRequest;
   struct radiusRequest_s *prevRequest;
   L7_uint32              lastSentTime;
   L7_uint32              retries;
   L7_int32               tryAnotherServer;
   L7_uchar8              requestId;
   L7_char8               authenticator[RADIUS_AUTHENTICATOR_LENGTH];
   L7_char8               encryptedPassword[RADIUS_MAX_PASSWORD_LENGTH];
   L7_uint32              encryptedPasswordLength;
   L7_char8               messageAuthenticator[RADIUS_AUTHENTICATOR_LENGTH];
   radiusPacket_t         *requestMessage;
   radiusRequestInfo_t    *requestInfo;

}  radiusRequest_t;

/* Radius server config structure */
typedef struct {
   L7_uint32               radiusServerConfigIpAddress;
   dnsHost_t               radiusServerDnsHostName;
   L7_ushort16             radiusServerConfigUdpPort;
   radiusServerType_t      radiusServerConfigServerType; /* RADIUS_SERVER_TYPE_AUTH,
                                                            RADIUS_SERVER_TYPE_ACCT */ 
   L7_char8                radiusServerConfigSecret[L7_RADIUS_MAX_SECRET+1];
   L7_BOOL                 radiusServerIsSecretConfigured; /* L7_TRUE if secret has been configured
                                                              L7_FALSE if no secret configured */
   radiusServerRowStatus_t radiusServerConfigRowStatus; /* RADIUS_SERVER_ACTIVE,
                                                           RADIUS_SERVER_NOTREADY,
                                                           RADIUS_SERVER_DESTROY,
                                                           RADIUS_SERVER_NOTCONFIGURED */
   L7_RADIUS_SERVER_ENTRY_TYPE_t radiusServerConfigServerEntryType; /* RADIUS_SERVER_ENTRY_TYPE_PRIMARY,
                                                                       RADIUS_SERVER_ENTRY_TYPE_SECONDARY */
   L7_uint32               radiusServerConfigPriority;
   L7_uint32               incMsgAuthMode; /* L7_ENABLE if to be included in all Access-Requests,
                                              L7_DISABLE if not to be included */
  /* To index the configured Name of the server in 
    (auth/acct)ServerNameArray[]. Non-zero indicates indexing to an entry*/
   L7_uint32               serverNameIndex;

   L7_RADIUS_SERVER_USAGE_TYPE_t usageType;
   L7_RADIUS_SERVER_USAGE_TYPE_t oldUsageType;
   L7_uint32               maxNumRetrans;
   L7_uint32               timeOutDuration;
   L7_uint32               radiusServerDeadTime;
   L7_uint32               sourceIpAddress;
   L7_uint32               localConfigMask; /* Mask specifying if the config params are local(non-zero) or global (zero) see RADIUS_SERVER_CONFIG_PARAMS_t*/
} radiusServerConfigEntry_t;

/* Client Statistics - Authentication Servers */
typedef struct 
{
    L7_uint32   radiusAuthServerIndex;
    L7_uint32   radiusAuthServerAddress;
    L7_ushort16 radiusAuthClientServerPortNumber;
    L7_uint32   radiusAuthClientAccessRequests;
    L7_uint32   radiusAuthClientAccessAccepts;
    L7_uint32   radiusAuthClientAccessRejects;
    L7_uint32   radiusAuthClientAccessChallenges;
    L7_uint32   radiusAuthClientPendingRequests;
    L7_uint32   radiusAuthClientBadAuthenticators;
    L7_uint32   radiusAuthClientMalformedAccessResponses;
    L7_uint32   radiusAuthClientUnknownTypes;
    L7_uint32   radiusAuthClientPacketsDropped;
    L7_uint32   radiusAuthClientRoundTripTime;
    L7_uint32   radiusAuthClientTimeouts;
    L7_uint32   radiusAuthClientAccessRetransmissions;

} radiusAuthServerEntry_t;

/* Client Statistics - Accounting Servers */
typedef struct 
{
    L7_uint32   radiusAcctServerIndex;
    L7_uint32   radiusAcctServerAddress;
    L7_ushort16 radiusAcctClientServerPortNumber;
    L7_uint32   radiusAcctClientRequests;
    L7_uint32   radiusAcctClientPendingRequests;
    L7_uint32   radiusAcctClientMalformedResponses;
    L7_uint32   radiusAcctClientUnknownTypes;
    L7_uint32   radiusAcctClientRoundTripTime;
    L7_uint32   radiusAcctClientResponses;
    L7_uint32   radiusAcctClientTimeouts;
    L7_uint32   radiusAcctClientRetransmissions;
    L7_uint32   radiusAcctClientBadAuthenticators;
    L7_uint32   radiusAcctClientPacketsDropped;

} radiusAcctServerEntry_t;

/*
** The RADIUS server configuration and statistics data structure. This uses
** the structures generated by MIB compiler. If any extra information is
** needed, it can be added to the following structure. An instance of this
** exists for every RADIUS server and server type pair.
*/
typedef struct radiusServerEntry_s {
   struct radiusServerEntry_s  *nextEntry;
   L7_uint32                   radiusServerConfigIpAddress;
   dnsHost_t                   radiusServerDnsHostName;
   L7_ushort16                 radiusServerConfigUdpPort;
   L7_uint32                   serverSocket;
   L7_uchar8                   requestId;
   L7_uint32                   status;
   radiusRequest_t             *pendingRequests;
   radiusServerConfigEntry_t   serverConfig;
   L7_BOOL                     isServerMarkedDead;
   L7_uint32                   serverDeathTime; /* Time instant when the server was declared dead */
   union 
   {
     radiusAuthServerEntry_t   authStats;
     radiusAcctServerEntry_t   acctStats;

   } radiusServerStats_t;
  /* To index the configured Name of the server in 
    (auth/acct)ServerNameArray[]. Non-zero indicates indexing to an entry*/
   L7_uint32                   serverNameIndex;                                       
} radiusServerEntry_t;

/* serverCount: Helps in tracking whether the serverName_t entry is configured
 *              and to know how many servers are configured.
 * currentEntry: 
 *              To keep track of the server being used currently, out of the 
 *              servers tagged by this serverName.
 */
typedef struct authServerName_s
{
  L7_char8             serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];
  union
  {
    struct 
    {
      radiusServerEntry_t  *authServer;      /* This server can accommodate all requests */
      radiusServerEntry_t  *authDot1xServer; /* This server can accommodate requests for dot1x */
      radiusServerEntry_t  *authLoginServer; /* This server can accommodate requests for local login */
    } auth;

    struct
    {
      radiusServerEntry_t  *acctServer;                
    } acct;
  }currentEntry;
  
  L7_uint32            serverCount;
}serverName_t;

typedef struct nwInterface_s
{
  L7_IP_ADDRESS_TYPE_t ipAddr;
  L7_uint32 socket;  
  L7_uint32 socketLastUsed; /*Time stamp*/
  

}nwInterface_t;

/* Common RADIUS params */
typedef struct radius_client_s
{
 L7_uint32                 authOperMode;
 L7_uint32                 acctAdminMode;
 L7_uint32                 maxNumRetrans;
 L7_uint32                 timeOutDuration;
 radiusServerConfigEntry_t serverConfigEntries[L7_RADIUS_MAX_SERVERS];
 L7_uint32                 retryPrimaryTime;
 L7_uint32                 numAuthServers;
 L7_uint32                 radiusServerDeadTime;
 L7_uint32                 sourceIpAddress;
 L7_char8                  radiusServerSecret[L7_RADIUS_MAX_SECRET + 1]; /* Allow for null terminator */
 L7_BOOL	               nasIpMode;    /* To include attribute 4 or not */
 L7_uint32	               nasIpAddress;	 /* configured NAS IP address */

 serverName_t              authServerNameArray[L7_RADIUS_MAX_AUTH_SERVERS];
 serverName_t              acctServerNameArray[L7_RADIUS_MAX_ACCT_SERVERS];
} radiusClient_t;

/*
** The RADIUS client global information structure which contains all the 
** information accessed globally.
*/
typedef struct {
   L7_uint32 radiusAuthClientInvalidServerAddresses;
   L7_uint32 radiusAcctClientInvalidServerAddresses;
   L7_uint32 currentTime;
   L7_uint32 serverEntryChangeTime;
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED

   /* sockets Data base is built per IPAddr. per Loop-back/Routing 
   interface in this array.*/
   nwInterface_t nwInterfaces[L7_RADIUS_MAX_NETWORK_INTERFACES];
   L7_uint32 totalNwInterfaces;
   L7_uint32 socketInactiveTime;
#endif
} radiusGlobalInfo_t;

/*
** Extern declerations
*/
extern radiusGlobalInfo_t  radiusGlobalData;
extern radiusServerEntry_t *radiusServerEntries;

extern radiusClient_t radiusClient;

/* RADIUS Client Registrar Data */
typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID; 
  L7_RC_t (*notify_radius_resp)(L7_uint32 status, 
                                  L7_uint32 correlator, 
                                  L7_uchar8 *attributes,
                                  L7_uint32 attributesLen);
} radiusNotifyList_t;

extern radiusNotifyList_t radiusNotifyList[L7_LAST_COMPONENT_ID];

extern serverName_t  authServerNameArray[L7_RADIUS_MAX_AUTH_SERVERS];
extern serverName_t  acctServerNameArray[L7_RADIUS_MAX_ACCT_SERVERS];


/*
** Some dictionary file specific defines
*/
#define RADIUS_TOKEN_ATTRIBUTE   "ATTRIBUTE"
#define RADIUS_TOKEN_VALUE       "VALUE"
#define RADIUS_TOKEN_VENDOR      "VENDOR"

#define RADIUS_ATTR_LINE_TOKEN_COUNT      4
#define RADIUS_VALUE_LINE_TOKEN_COUNT     4
#define RADIUS_VENDOR_LINE_TOKEN_COUNT    3

#define RADIUS_DICT_ATTR_TYPE_STRING   "string"
#define RADIUS_DICT_ATTR_TYPE_INTEGER  "integer"
#define RADIUS_DICT_ATTR_TYPE_IP_ADDR  "ipaddr"
#define RADIUS_DICT_ATTR_TYPE_DATE     "date"

typedef struct radiusDictValue_s
{
   struct radiusDictValue_s *nextValue;
   L7_char8                 attrName[RADIUS_TOKEN_LENGTH + 1];
   L7_char8                 valueName[RADIUS_TOKEN_LENGTH + 1];
   L7_int32                 valueId;

}  radiusDictValue_t;

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
extern nwInterface_t nwInterfaces[L7_RADIUS_MAX_NETWORK_INTERFACES];
#endif
/*
** Internal function prototypes
*/

void radiusPendingRequestsRelease(radiusServerEntry_t *serverEntry, 
                                  radiusRequest_t *pendingRequests);

void radiusRequestDelink(radiusServerEntry_t *serverEntry, 
                         radiusRequest_t *request);

L7_RC_t radiusDictionaryInit(void);

void radiusDictionaryFree(void);

L7_int32 radiusVendorCodeGet(L7_char8 *vendorName);

radiusDictAttr_t *radiusAttributeByNameGet(L7_char8 *attrName);

radiusDictAttr_t *radiusVSAttributeGet(L7_uint32 vendorCode, 
                                       L7_uint32 vsAttrId);

radiusDictValue_t *radiusAttrValueByValueNameGet(L7_char8 *valueName);

radiusDictValue_t *radiusAttrValueByValueIdGet(L7_int32 valueId, 
                                               L7_char8 *attrName);

radiusClient_t *radiusClientGet(void);

radiusServerEntry_t *radiusServerEntryGet(L7_uint32 serverAddr, 
                                          L7_uint32 serverType);

radiusServerEntry_t *radiusServerHostEntryGet(L7_uchar8 *hostname,
                                          L7_uint32 serverType);
L7_uint32 radiusAuthServerCount();

L7_uint32 radiusAuthActiveServerCount();

L7_uint32 radiusAcctServerCount();

void radiusUpdateIndexNumbers(L7_uint32 type);

/*********************************************************************
*
* @purpose Function to find out the number of authservers configured
*          in the specified named servers group.
*
* @returns the number of authservers that have been configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAuthActiveServerCountGet(L7_uint32 index, L7_RADIUS_SERVER_USAGE_TYPE_t usageType);

/*********************************************************************
*
* @purpose  Take the write lock.
*
* @param   rwLock - WDM Read/Write Lock.
* @param   timeout - Timeout option
* @param   file - File that got the lock.
* @param   line_num - Line number that got the lock.
*
* returns   return code from osapiWriteLockTake
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusSemaTake (void *sem, L7_int32 timeout,
                L7_uchar8 *file, L7_uint32 line_num);

/*********************************************************************
*
* @purpose  Give the write lock.
*
* @param   rwLock - WDM Read/Write Lock.
*
* returns   return code from osapiWriteLockGive
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusSemaGive (void *sem);

#define RADIUS_LOCK_TAKE(lock, wait) radiusSemaTake(lock, wait, __FILE__,__LINE__)
#define RADIUS_LOCK_GIVE(lock) radiusSemaGive(lock)

#endif /* INCLUDE_RADIUS_DB_H */

