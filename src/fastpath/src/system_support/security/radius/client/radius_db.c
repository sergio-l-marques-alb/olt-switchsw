/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_db.c
*
* @purpose RADIUS Client database access code
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
#include "radius_include.h"
#include "l7utils_api.h"

static unsigned char radius_lock_file[128];
static L7_uint32 radius_lock_line;
static L7_uint32 radius_lock_taken;

extern L7_uint32 radius_sm_bp_id;

/* RADIUS callback array */
radiusNotifyList_t radiusNotifyList[L7_LAST_COMPONENT_ID];

/*
** The static global objects which contain the definitions of the standard
** and vendor specific attributes, attribute values and vendor codes, etc.
*/
static radiusDictAttr_t *radiusAttrsDefs, *vendorAttrsDefs;
static radiusDictValue_t *attrValueDefs;

/*
** The dictionary translations for parsing requests and generating responses.
** All transactions are composed of Attribute/Value Pairs.  The value of each
** attribute is specified as one of 4 data types.  Valid data types are:
**
** string  - 0-253 octets
** ipaddr  - 4 octets in network byte order
** integer - 32 bit value in big endian order (high byte first)
** date    - 32 bit value in big endian order - seconds since
**           00:00:00 GMT,  Jan.  1, 1970
**
** The attributes, values and vendor codes are defined in the following format.
** The vendor code must be in the beginning, so the VSA can be parsed properly.
** Vendor-Specific attributes use the Private Enterprise Numbers as defined at
** www.iana.org.
**
** VENDOR     vendor-name vendor-id
** ATTRIBUTE  attr-name   attr-id     attr-data-type {vendor-name}
** VALUE      attr-name   value-name  valude-id
**
*/
L7_char8 *radiusDictionary[] =
{
   /* Vendor codes */
   "VENDOR LVL7 6132",
   "VENDOR Microsoft 311",
   "VENDOR WISPr 14122",

   /* Vendor-Specific Attributes */
   "ATTRIBUTE LVL7-Wireless-AP-Location        101 string  LVL7",
   "ATTRIBUTE LVL7-Wireless-AP-Mode            102 integer LVL7",
   "ATTRIBUTE LVL7-Wireless-AP-Profile-ID      103 integer LVL7",
   "ATTRIBUTE LVL7-Wireless-AP-Switch-IP       104 ipaddr  LVL7",
   "ATTRIBUTE LVL7-Wireless-AP-Radio-1-Chan    105 integer LVL7",
   "ATTRIBUTE LVL7-Wireless-AP-Radio-2-Chan    106 integer LVL7",
   "ATTRIBUTE LVL7-Wireless-AP-Radio-1-Power   107 integer LVL7",
   "ATTRIBUTE LVL7-Wireless-AP-Radio-2-Power   108 integer LVL7",
   "ATTRIBUTE LVL7-Wireless-Client-ACL-Dn      120 string  LVL7",
   "ATTRIBUTE LVL7-Wireless-Client-ACL-Up      121 string  LVL7",
   "ATTRIBUTE LVL7-Wireless-Client-Policy-Dn   122 string  LVL7",
   "ATTRIBUTE LVL7-Wireless-Client-Policy-Up   123 string  LVL7",

   "ATTRIBUTE MS-MPPE-Send-Key 16 string Microsoft",
   "ATTRIBUTE MS-MPPE-Recv-Key 17 string Microsoft",

   "ATTRIBUTE WISPr-Bandwidth-Max-Up   7 integer WISPr",
   "ATTRIBUTE WISPr-Bandwidth-Max-Down 8 integer WISPr",

   /* Standard Attributes */
   "ATTRIBUTE User-Name                 1 string",
   "ATTRIBUTE Password                  2 string",
   "ATTRIBUTE CHAP-Password             3 string",
   "ATTRIBUTE NAS-IP-Address            4 ipaddr",
   "ATTRIBUTE NAS-Port                  5 integer",
   "ATTRIBUTE Service-Type              6 integer",
   "ATTRIBUTE Framed-Protocol           7 integer",
   "ATTRIBUTE Framed-IP-Address         8 ipaddr",
   "ATTRIBUTE Framed-IP-Netmask         9 ipaddr",
   "ATTRIBUTE Framed-Routing           10 integer",
   "ATTRIBUTE Filter-Id                11 string",
   "ATTRIBUTE Framed-MTU               12 integer",
   "ATTRIBUTE Framed-Compression       13 integer",
   "ATTRIBUTE Login-IP-Host            14 ipaddr",
   "ATTRIBUTE Login-Service            15 integer",
   "ATTRIBUTE Login-TCP-Port           16 integer",
   "ATTRIBUTE Reply-Message            18 string",
   "ATTRIBUTE Callback-Number          19 string",
   "ATTRIBUTE Callback-Id              20 string",
   "ATTRIBUTE Framed-Route             22 string",
   "ATTRIBUTE Framed-IPX-Network       23 ipaddr",
   "ATTRIBUTE State                    24 string",
   "ATTRIBUTE Class                    25 string",
   "ATTRIBUTE Vendor-Specific          26 string",
   "ATTRIBUTE Session-Timeout          27 integer",
   "ATTRIBUTE Idle-Timeout             28 integer",
   "ATTRIBUTE Termination-Action       29 integer",
   "ATTRIBUTE Called-Station-Id        30 string",
   "ATTRIBUTE Calling-Station-Id       31 string",
   "ATTRIBUTE NAS-Identifier           32 string",
   "ATTRIBUTE Proxy-State              33 string",
   "ATTRIBUTE Login-LAT-Service        34 string",
   "ATTRIBUTE Login-LAT-Node           35 string",
   "ATTRIBUTE Login-LAT-Group          36 string",
   "ATTRIBUTE Framed-AppleTalk-Link    37 integer",
   "ATTRIBUTE Framed-AppleTalk-Network 38 integer",
   "ATTRIBUTE Framed-AppleTalk-Zone    39 string",
   "ATTRIBUTE Acct-Status-Type         40 integer",
   "ATTRIBUTE Acct-Delay-Time          41 integer",
   "ATTRIBUTE Acct-Input-Octets        42 integer",
   "ATTRIBUTE Acct-Output-Octets       43 integer",
   "ATTRIBUTE Acct-Session-Id          44 string",
   "ATTRIBUTE Acct-Authentic           45 integer",
   "ATTRIBUTE Acct-Session-Time        46 integer",
   "ATTRIBUTE Acct-Input-Packets       47 integer",
   "ATTRIBUTE Acct-Output-Packets      48 integer",
   "ATTRIBUTE Acct-Terminate-Cause     49 integer",
   "ATTRIBUTE Acct-Multi-Session-Id    50 string",
   "ATTRIBUTE Acct-Link-Count          51 integer",
   "ATTRIBUTE Acct-Input-Gigawords     52 integer",
   "ATTRIBUTE Acct-Output-Gigawords    53 integer",
   "ATTRIBUTE Event-Timestamp          55 integer",
   "ATTRIBUTE CHAP-Challenge           60 string",
   "ATTRIBUTE NAS-Port-Type            61 integer",
   "ATTRIBUTE Port-Limit               62 integer",
   "ATTRIBUTE Login-LAT-Port           63 string",
   "ATTRIBUTE Tunnel-Type              64 integer",
   "ATTRIBUTE Tunnel-Medium-Type       65 integer",
   "ATTRIBUTE Tunnel-Client-Endpoint   66 string",
   "ATTRIBUTE Tunnel-Server-Endpoint   67 string",
   "ATTRIBUTE Acct-Tunnel-Connection   68 string",
   "ATTRIBUTE Tunnel-Password          69 string",
   "ATTRIBUTE ARAP-Password            70 string",
   "ATTRIBUTE ARAP-Features            71 string",
   "ATTRIBUTE ARAP-Zone-Access         72 integer",
   "ATTRIBUTE ARAP-Security            73 integer",
   "ATTRIBUTE ARAP-Security-Data       74 string",
   "ATTRIBUTE Password-Retry           75 integer",
   "ATTRIBUTE Prompt                   76 integer",
   "ATTRIBUTE Connect-Info             77 string",
   "ATTRIBUTE Configuration-Token      78 string",
   "ATTRIBUTE EAP-Message              79 string",
   "ATTRIBUTE Signature                80 string",
   "ATTRIBUTE Tunnel-Private-Group-ID  81 string",
   "ATTRIBUTE Tunnel-Assignment-ID     82 string",
   "ATTRIBUTE Tunnel-Preference        83 integer",
   "ATTRIBUTE ARAP-Challenge-Response  84 string",
   "ATTRIBUTE Acct-Interim-Interval    85 integer",
   "ATTRIBUTE Acct-Tunnel-Packets-Lost 86 integer",
   "ATTRIBUTE NAS-Port-Id              87 string",
   "ATTRIBUTE Framed-Pool              88 string",

   /* Service Type values */
   "VALUE Service-Type Login-User                1",
   "VALUE Service-Type Framed-User               2",
   "VALUE Service-Type Callback-Login-User       3",
   "VALUE Service-Type Callback-Framed-User      4",
   "VALUE Service-Type Outbound-User             5",
   "VALUE Service-Type Administrative-User       6",
   "VALUE Service-Type NAS-Prompt-User           7",
   "VALUE Service-Type Authenticate-Only         8",
   "VALUE Service-Type Callback-NAS-Prompt       9",
   "VALUE Service-Type Call-Check               10",
   "VALUE Service-Type Callback-Administrative  11",
   "VALUE Service-Type Call-Check-User         129",

   /* Framed Protocol values */
   "VALUE Framed-Protocol PPP               1",
   "VALUE Framed-Protocol SLIP              2",
   "VALUE Framed-Protocol ARAP              3",
   "VALUE Framed-Protocol GANDALF-SLMLP     4",
   "VALUE Framed-Protocol XYLOGICS-IPX-SLIP 5",
   "VALUE Framed-Protocol X75               6",

   /* Framed Routing Values */
   "VALUE Framed-Routing None             0",
   "VALUE Framed-Routing Broadcast        1",
   "VALUE Framed-Routing Listen           2",
   "VALUE Framed-Routing Broadcast-Listen 3",

   /* Framed Compression Type values */
   "VALUE Framed-Compression None                0",
   "VALUE Framed-Compression Van-Jacobson-TCP-IP 1",
   "VALUE Framed-Compression IPX-Header          2",
   "VALUE Framed-Compression Stac-LZS            3",

   /* Login Service values */
   "VALUE Login-Service Telnet          0",
   "VALUE Login-Service Rlogin          1",
   "VALUE Login-Service TCP-Clear       2",
   "VALUE Login-Service PortMaster      3",
   "VALUE Login-Service LAT             4",
   "VALUE Login-Service X.25-PAD        5",
   "VALUE Login-Service X.25-T3POS      6",
   "VALUE Login-Service TCP-Clear-Quiet 8",

   /* Status Type values */
   "VALUE Acct-Status-Type Start               1",
   "VALUE Acct-Status-Type Stop                2",
   "VALUE Acct-Status-Type Interim-Update      3",
   "VALUE Acct-Status-Type Accounting-On       7",
   "VALUE Acct-Status-Type Accounting-Off      8",
   "VALUE Acct-Status-Type Tunnel-Start        9",
   "VALUE Acct-Status-Type Tunnel-Stop        10",
   "VALUE Acct-Status-Type Tunnel-Reject      11",
   "VALUE Acct-Status-Type Tunnel-Link-Start  12",
   "VALUE Acct-Status-Type Tunnel-Link-Stop   13",
   "VALUE Acct-Status-Type Tunnel-Link-Reject 14",
   "VALUE Acct-Status-Type Failed             15",

   /* Authentication Type values */
   "VALUE Acct-Authentic RADIUS 1",
   "VALUE Acct-Authentic Local  2",
   "VALUE Acct-Authentic Remote 3",

   /* Termination Option values */
   "VALUE Termination-Action Default        0",
   "VALUE Termination-Action RADIUS-Request 1",

   /* NAS Port Type values */
   "VALUE NAS-Port-Type Async               0",
   "VALUE NAS-Port-Type Sync                1",
   "VALUE NAS-Port-Type ISDN                2",
   "VALUE NAS-Port-Type ISDN-V120           3",
   "VALUE NAS-Port-Type ISDN-V110           4",
   "VALUE NAS-Port-Type Virtual             5",
   "VALUE NAS-Port-Type PIAFS               6",
   "VALUE NAS-Port-Type HDLC-Clear-Channel  7",
   "VALUE NAS-Port-Type X.25                8",
   "VALUE NAS-Port-Type X.75                9",
   "VALUE NAS-Port-Type G.3-Fax            10",
   "VALUE NAS-Port-Type SDSL               11",
   "VALUE NAS-Port-Type ADSL-CAP           12",
   "VALUE NAS-Port-Type ADSL-DMT           13",
   "VALUE NAS-Port-Type IDSL               14",
   "VALUE NAS-Port-Type Ethernet           15",

   /* Acct Terminate Cause values */
   "VALUE Acct-Terminate-Cause User-Request         1",
   "VALUE Acct-Terminate-Cause Lost-Carrier         2",
   "VALUE Acct-Terminate-Cause Lost-Service         3",
   "VALUE Acct-Terminate-Cause Idle-Timeout         4",
   "VALUE Acct-Terminate-Cause Session-Timeout      5",
   "VALUE Acct-Terminate-Cause Admin-Reset          6",
   "VALUE Acct-Terminate-Cause Admin-Reboot         7",
   "VALUE Acct-Terminate-Cause Port-Error           8",
   "VALUE Acct-Terminate-Cause NAS-Error            9",
   "VALUE Acct-Terminate-Cause NAS-Request         10",
   "VALUE Acct-Terminate-Cause NAS-Reboot          11",
   "VALUE Acct-Terminate-Cause Port-Unneeded       12",
   "VALUE Acct-Terminate-Cause Port-Preempted      13",
   "VALUE Acct-Terminate-Cause Port-Suspended      14",
   "VALUE Acct-Terminate-Cause Service-Unavailable 15",
   "VALUE Acct-Terminate-Cause Callback            16",
   "VALUE Acct-Terminate-Cause User-Error          17",
   "VALUE Acct-Terminate-Cause Host-Request        18",

   /* Tunnel Type values */
   "VALUE Tunnel-Type PPTP       1",
   "VALUE Tunnel-Type L2F        2",
   "VALUE Tunnel-Type L2TP       3",
   "VALUE Tunnel-Type ATM        4",
   "VALUE Tunnel-Type VTP        5",
   "VALUE Tunnel-Type AH         6",
   "VALUE Tunnel-Type IP-IP      7",
   "VALUE Tunnel-Type MIN-IP-IP  8",
   "VALUE Tunnel-Type ESP        9",
   "VALUE Tunnel-Type GRE       10",
   "VALUE Tunnel-Type DVS       11",
   "VALUE Tunnel-Type IP-in-IP  12",

   /* Tunnel Medium Type values */
   "VALUE Tunnel-Medium-Type IP         1",
   "VALUE Tunnel-Medium-Type IP6        2",
   "VALUE Tunnel-Medium-Type NSAP       3",
   "VALUE Tunnel-Medium-Type HDLC       4",
   "VALUE Tunnel-Medium-Type BBN        5",
   "VALUE Tunnel-Medium-Type Ether_802  6",
   "VALUE Tunnel-Medium-Type E.163      7",
   "VALUE Tunnel-Medium-Type E.164      8",
   "VALUE Tunnel-Medium-Type F.69       9",
   "VALUE Tunnel-Medium-Type X.121     10",
   "VALUE Tunnel-Medium-Type IPX       11",
   "VALUE Tunnel-Medium-Type AppleTalk 12",
   "VALUE Tunnel-Medium-Type DECnet    13",
   "VALUE Tunnel-Medium-Type Banyan    14",

   /* Prompt values */
   "VALUE Prompt No-Echo 0",
   "VALUE Prompt Echo    1",

   /* Prompt values */
   "VALUE LVL7-Wireless-AP-Mode          WS-Managed     1",
   "VALUE LVL7-Wireless-AP-Mode          Standalone     2",
   "VALUE LVL7-Wireless-AP-Mode          Rogue          3",
   "VALUE LVL7-Wireless-AP-Radio-1-Chan  Auto           0",
   "VALUE LVL7-Wireless-AP-Radio-2-Chan  Auto           0",
   "VALUE LVL7-Wireless-AP-Radio-1-Power Auto           0",
   "VALUE LVL7-Wireless-AP-Radio-1-Power Minimum        1",
   "VALUE LVL7-Wireless-AP-Radio-1-Power Maximum      100",
   "VALUE LVL7-Wireless-AP-Radio-2-Power Auto           0",
   "VALUE LVL7-Wireless-AP-Radio-2-Power Minimum        1",
   "VALUE LVL7-Wireless-AP-Radio-2-Power Maximum      100",

   /* Prompt values */
   "VALUE WISPr-Bandwidth-Max-Up         Minimum                 0",
   "VALUE WISPr-Bandwidth-Max-Up         Maximum        4294967295",
   "VALUE WISPr-Bandwidth-Max-Down       Minimum                 0",
   "VALUE WISPr-Bandwidth-Max-Down       Maximum        4294967295",

   /* NULL string must be present at the end */
   L7_NULL
};

/*********************************************************************
*
* @purpose Release all the pending requests of a server entry.
*
* @param serverEntry @b((input)) pointer to the server entry
* @param pendingRequests @b((input)) pointer to the request list
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusPendingRequestsRelease(radiusServerEntry_t *serverEntry,
                                  radiusRequest_t *pendingRequests)
{
  radiusRequest_t *request, *nextRequest;

  for (request = pendingRequests; request != L7_NULL; request = nextRequest)
  {
    nextRequest = request->nextRequest;

    if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
    {
      /* Access-Request */
      request->requestInfo->status = RADIUS_STATUS_REQUEST_TIMED_OUT;
      serverEntry->radiusServerStats_t.authStats.radiusAuthClientTimeouts++;
      radiusResponseNotify(request->requestInfo, L7_NULL);
    }
    else
    {
      /* Accounting-Request */
      serverEntry->radiusServerStats_t.acctStats.radiusAcctClientTimeouts++;
    }

    radiusRequestDelink(serverEntry, request);
  }

  serverEntry->pendingRequests = L7_NULL;

  return;
}

/*********************************************************************
*
* @purpose Delink the request from the request chain and release all
*          the resources associated with the request.
*
* @param serverEntry @b((input)) server with pending requests
* @param request @b((input)) the request to be released
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusRequestDelink(radiusServerEntry_t *serverEntry,
                         radiusRequest_t *request)
{
  /* If the first element in the queue is being deleted and there are
   * additional entries in the list, update the pendingRequest pointer
   * in the serverEntry structure
   */
  if ((request->prevRequest == L7_NULL) && (request->nextRequest != L7_NULL))
  {
    serverEntry->pendingRequests = request->nextRequest;
  }

  if (request->nextRequest != L7_NULL)
  {
    request->nextRequest->prevRequest = request->prevRequest;
  }

  if (request->prevRequest != L7_NULL)
  {
    request->prevRequest->nextRequest = request->nextRequest;
  }
  else
  {
    if (request->nextRequest == L7_NULL)
    {
      serverEntry->pendingRequests = L7_NULL;
    }
  }

  /*
  ** Decrement the pending requests client statistic
  */
  if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
  {
    /* Make sure stats have not been cleared by a user request */
    if (serverEntry->radiusServerStats_t.authStats.radiusAuthClientPendingRequests > L7_NULL)
    {
      serverEntry->radiusServerStats_t.authStats.radiusAuthClientPendingRequests--;
    }
  }
  else
  {
    /* Make sure stats have not been cleared by a user request */
    if (serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPendingRequests > L7_NULL)
    {
      serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPendingRequests--;
    }
  }

  /*
  ** Free up all memory associated with this request
  */
  if (request->requestInfo != L7_NULL)
  {
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)request->requestInfo);
  }

  if (request->requestMessage != L7_NULL)
  {
    osapiFree(L7_RADIUS_COMPONENT_ID, request->requestMessage);
  }

  bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)request);

  return;
}

/*********************************************************************
*
* @purpose Retrieve the global radius client structure
*
* @returns a pointer to the radiusClient structure
*
* @comments
*
* @end
*
*********************************************************************/
radiusClient_t *radiusClientGet(void)
{
  return &radiusClient;
}

/*********************************************************************
*
* @purpose Retrieve a server entry structure
*
* @param serverAddr @b((input)) IP Address of the server
* @param serverType @b((input)) AUTH OR ACCT server
*
* @returns the RadiusServerEntry structure corresponding to the above or
*          returns L7_NULL if it does not find any match
*
* @comments
*
* @end
*
*********************************************************************/
radiusServerEntry_t *radiusServerEntryGet(L7_uint32 serverAddr,
                                          L7_uint32 serverType)
{
  radiusServerEntry_t *serverEntry = radiusServerEntries;

  while(serverEntry != L7_NULL)
  {
    if ((serverEntry->radiusServerConfigIpAddress == serverAddr) &&
        (serverEntry->serverConfig.radiusServerConfigServerType == serverType))
    {
      return serverEntry;
    }

    serverEntry = serverEntry->nextEntry;
  }

  return L7_NULL;
}

/*********************************************************************
*
* @purpose Retrieve a server entry structure
*
* @param serverAddr @b((input)) Host Name of the server
* @param serverType @b((input)) AUTH OR ACCT server
*
* @returns the RadiusServerEntry structure corresponding to the above or
*          returns L7_NULL if it does not find any match
*
* @comments
*
* @end
*
*********************************************************************/
radiusServerEntry_t *radiusServerHostEntryGet(L7_uchar8 *hostname,
                                          L7_uint32 serverType)
{
  radiusServerEntry_t *serverEntry = radiusServerEntries;

  while(serverEntry != L7_NULL)
  {
    if ((strcmp(serverEntry->radiusServerDnsHostName.host.hostName, hostname) == 0) &&
        (serverEntry->serverConfig.radiusServerConfigServerType == serverType))
    {
      return serverEntry;
    }

    serverEntry = serverEntry->nextEntry;
  }

  return L7_NULL;
}

/*********************************************************************
*
* @purpose Function to find out the number of authservers configured
*
* @returns the number of authservers that have been configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAuthActiveServerCount()
{
  radiusServerEntry_t *serverEntry = radiusServerEntries;
  L7_uint32 count = L7_NULL;

  /*
  ** Identify the number of auth server entries that already exist
  */
  while(serverEntry != L7_NULL)
  {
    if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH &&
       serverEntry->serverConfig.radiusServerConfigRowStatus == RADIUS_SERVER_ACTIVE)
    {
      count++;
    }

    serverEntry = serverEntry->nextEntry;
  }

  return count;
}

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
L7_uint32 radiusAuthActiveServerCountGet(L7_uint32 index, L7_RADIUS_SERVER_USAGE_TYPE_t usageType)
{
  radiusServerEntry_t *serverEntry = radiusServerEntries;
  L7_uint32 count = L7_NULL;

  /*
  ** Identify the number of auth server entries that already exist
  */
  while(serverEntry != L7_NULL)
  {
    if (serverEntry->serverConfig.
                     radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH &&
        serverEntry->serverConfig.
                     radiusServerConfigRowStatus == RADIUS_SERVER_ACTIVE &&
        (serverEntry->serverConfig.usageType == L7_RADIUS_SERVER_USAGE_ALL || serverEntry->serverConfig.usageType == usageType) &&
        serverEntry->serverNameIndex == index
       )
    {
      count++;
    }

    serverEntry = serverEntry->nextEntry;
  }

  return count;
}

/*********************************************************************
*
* @purpose Function to find out the number of authservers configured
*
* @returns the number of authservers that have been configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAuthServerCount()
{
  radiusServerEntry_t *serverEntry = radiusServerEntries;
  L7_uint32 count = L7_NULL;

  /*
  ** Identify the number of auth server entries that already exist
  */
  while(serverEntry != L7_NULL)
  {
    if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
    {
      count++;
    }

    serverEntry = serverEntry->nextEntry;
  }

  return count;
}

/*********************************************************************
*
* @purpose Function to find out the number of Accounting servers configured
*
* @returns the number of Accounting servers that have been configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 radiusAcctServerCount()
{
  radiusServerEntry_t *serverEntry = radiusServerEntries;
  L7_uint32 count = L7_NULL;

  /*
  ** Identify the number of Accounting server entries that already exist
  */
  while(serverEntry != L7_NULL)
  {
    if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT)
    {
      count++;
    }
    serverEntry = serverEntry->nextEntry;
  }

  return count;
}

/*********************************************************************
*
* @purpose Function to update the Index values of all the servers after
*          a server of a particular type has been deleted
*
* @param type @b((input)) ACCOUNTING or AUTHORIZATION server that was deleted
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusUpdateIndexNumbers(L7_uint32 type)
{
  radiusServerEntry_t *serverEntry = radiusServerEntries;
  L7_uint32 index=1;

  while(serverEntry != L7_NULL)
  {
    if ((serverEntry->serverConfig.radiusServerConfigServerType == type) &&
        (type == RADIUS_SERVER_TYPE_AUTH))
    {
      serverEntry->radiusServerStats_t.authStats.radiusAuthServerIndex = index;
      index++;
    }
    else if ((serverEntry->serverConfig.radiusServerConfigServerType == type) &&
             (type == RADIUS_SERVER_TYPE_ACCT))
    {
      serverEntry->radiusServerStats_t.acctStats.radiusAcctServerIndex = index;
      index++;
    }

    serverEntry = serverEntry->nextEntry;
  }

  return;
}

/*********************************************************************
*
* @purpose Read all the ATTRIBUTES, VALUES, VENDOR SPECIFIC ATTRIBUTES
*          from the dictionary file and make parsed copy of them in the
*          data structures.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusDictionaryInit(void)
{
   radiusDictAttr_t *dictAttr;
   radiusDictValue_t *dictValue;
   L7_int32 lineNumber = L7_NULL;
   L7_char8 *buffer;
   L7_char8 *skipName, *attrName, *attrId, *attrType;
   L7_char8 *valueName, *valueId, *vendorName, *vendorCode;
   L7_int32 attr, value, type, code;
   L7_RC_t rc = L7_SUCCESS;

   if (bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&skipName)   != L7_SUCCESS ||
       bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&attrName)   != L7_SUCCESS ||
       bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&attrId)     != L7_SUCCESS ||
       bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&attrType)   != L7_SUCCESS ||
       bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&valueName)  != L7_SUCCESS ||
       bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&valueId)    != L7_SUCCESS ||
       bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&vendorName) != L7_SUCCESS ||
       bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&vendorCode) != L7_SUCCESS)
   {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
             "RADIUS: radiusDictionaryInit() could not allocate memory");
     rc = L7_FAILURE;
     return rc;
   }

   (void)memset((void *)skipName, L7_NULL, RADIUS_SM_BUF_SIZE);
   (void)memset((void *)attrName, L7_NULL, RADIUS_SM_BUF_SIZE);
   (void)memset((void *)attrId, L7_NULL, RADIUS_SM_BUF_SIZE);
   (void)memset((void *)attrType, L7_NULL, RADIUS_SM_BUF_SIZE);
   (void)memset((void *)valueName, L7_NULL, RADIUS_SM_BUF_SIZE);
   (void)memset((void *)valueId, L7_NULL, RADIUS_SM_BUF_SIZE);
   (void)memset((void *)vendorName, L7_NULL, RADIUS_SM_BUF_SIZE);
   (void)memset((void *)vendorCode, L7_NULL, RADIUS_SM_BUF_SIZE);

   while ((buffer = radiusDictionary[lineNumber]) != L7_NULL)
   {
      /*
      ** Increment lineNumber and skip comments and empty lines (if empty).
      */
      lineNumber++;
      if (strlen(buffer) <= L7_NULL)
         continue;

      if(strncmp(buffer, RADIUS_TOKEN_ATTRIBUTE, strlen(RADIUS_TOKEN_ATTRIBUTE)) == L7_NULL)
      {
         L7_int32 tokenCount;

         /*
         ** Read the ATTRIBUTE line
         */
         tokenCount = sscanf(buffer, "%s%s%s%s%s", skipName, attrName, attrId, attrType,
                             vendorName);
         if (tokenCount < RADIUS_ATTR_LINE_TOKEN_COUNT)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: ATTRIBUTE entry invalid on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         /*
         ** Validate all the tokens and if OK, create a DictAttr entry and
         ** insert into the radiusAttrsDefs list.
         */
         if (strlen(attrName) > RADIUS_TOKEN_LENGTH)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: Attribute name is too long on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         if ((attr = atoi(attrId)) <= L7_NULL)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: Attribute id is invalid on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         if (strcmp(attrType, RADIUS_DICT_ATTR_TYPE_STRING) == L7_NULL)
            type = RADIUS_ATTR_VALUE_TYPE_STRING;
         else if (strcmp(attrType, RADIUS_DICT_ATTR_TYPE_INTEGER) == L7_NULL)
            type = RADIUS_ATTR_VALUE_TYPE_INTEGER;
         else if (strcmp(attrType, RADIUS_DICT_ATTR_TYPE_IP_ADDR) == L7_NULL)
            type = RADIUS_ATTR_VALUE_TYPE_IP_ADDR;
         else if (strcmp(attrType, RADIUS_DICT_ATTR_TYPE_DATE) == L7_NULL)
            type = RADIUS_ATTR_VALUE_TYPE_DATE;
         else
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: Attribute type is invalid on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         if (bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&dictAttr) != L7_SUCCESS)
         {
            rc = L7_FAILURE;
            break;
         }

         (void)memset((void *)dictAttr, L7_NULL, RADIUS_SM_BUF_SIZE);

         strcpy(dictAttr->attrName, attrName);
         dictAttr->attrType = type;
         if (tokenCount > RADIUS_ATTR_LINE_TOKEN_COUNT)
         {
            if (strlen(vendorName) > RADIUS_TOKEN_LENGTH)
            {
               L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                       "RADIUS: Vendor name is too long on line (%d) in RADIUS dictionary", lineNumber);
               rc = L7_FAILURE;
               break;
            }
            dictAttr->attrId = RADIUS_ATTR_TYPE_VENDOR;
            dictAttr->vendorCode = radiusVendorCodeGet(vendorName);
            dictAttr->vsAttrId = attr;
         }
         else
         {
            dictAttr->attrId = attr;
            dictAttr->vendorCode = dictAttr->vsAttrId = L7_NULL;
         }

         dictAttr->nextAttr = radiusAttrsDefs;
         radiusAttrsDefs = dictAttr;
      }
      else if(strncmp(buffer, RADIUS_TOKEN_VALUE, strlen(RADIUS_TOKEN_VALUE)) == L7_NULL)
      {
         /*
         ** Read the VALUE line
         */
         if (sscanf(buffer, "%s%s%s%s", skipName, attrName, valueName, valueId) !=
                    RADIUS_VALUE_LINE_TOKEN_COUNT)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: VALUE entry invalid on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         /*
         ** Validate all the tokens and if OK, create a DictValue entry and
         ** insert into the attrVelueDefs list.
         */
         if (strlen(attrName) > RADIUS_TOKEN_LENGTH)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: Attribute name is too long on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         if (strlen(valueName) > RADIUS_TOKEN_LENGTH)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: Value name is too long on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         if ((value = atoi(valueId)) < L7_NULL)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: Value id is invalid on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         if(bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&dictValue) != L7_SUCCESS)
         {
            rc = L7_FAILURE;
            break;
          }

         (void)memset((void *)dictValue, L7_NULL, RADIUS_SM_BUF_SIZE);

         strcpy(dictValue->attrName, attrName);
         strcpy(dictValue->valueName, valueName);
         dictValue->valueId = value;

         dictValue->nextValue = attrValueDefs;
         attrValueDefs = dictValue;
      }
      else if(strncmp(buffer, RADIUS_TOKEN_VENDOR, strlen(RADIUS_TOKEN_VENDOR)) == L7_NULL)
      {
         /*
         ** Read the VENDOR line
         */
         if (sscanf(buffer, "%s%s%s", skipName, vendorName, vendorCode) !=
                    RADIUS_VENDOR_LINE_TOKEN_COUNT)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: VENDOR entry invalid on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         /*
         ** Validate all the tokens and if OK, create a DictAttr entry and
         ** insert into the vendorAttrsDefs list.
         */
         if (strlen(vendorName) > RADIUS_TOKEN_LENGTH)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: Vendor name is too long on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         if ((code = atoi(vendorCode)) <= L7_NULL)
         {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                    "RADIUS: Vendor Code is invalid on line (%d) in dictionary", lineNumber);
            rc = L7_FAILURE;
            break;
         }

         if (bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&dictAttr) != L7_SUCCESS)
         {
            rc = L7_FAILURE;
            break;
         }

         (void)memset((void *)dictAttr, L7_NULL, RADIUS_SM_BUF_SIZE);

         strcpy(dictAttr->attrName, vendorName);
         dictAttr->attrType = L7_NULL;
         dictAttr->attrId = L7_NULL;
         dictAttr->vendorCode = code;
         dictAttr->vsAttrId = L7_NULL;

         dictAttr->nextAttr = vendorAttrsDefs;
         vendorAttrsDefs = dictAttr;
      }
   }

   bufferPoolFree(radius_sm_bp_id, skipName);
   bufferPoolFree(radius_sm_bp_id, attrName);
   bufferPoolFree(radius_sm_bp_id, attrId);
   bufferPoolFree(radius_sm_bp_id, attrType);
   bufferPoolFree(radius_sm_bp_id, valueName);
   bufferPoolFree(radius_sm_bp_id, valueId);
   bufferPoolFree(radius_sm_bp_id, vendorName);
   bufferPoolFree(radius_sm_bp_id, vendorCode);

   return rc;
}

/*********************************************************************
*
* @purpose Release all the memory allocated for the dictionary contents
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDictionaryFree(void)
{
  radiusDictAttr_t *dictAttr, *nextAttr;
  radiusDictValue_t *dictValue, *nextValue;

  for (dictAttr = radiusAttrsDefs; dictAttr; dictAttr = nextAttr)
  {
    nextAttr = dictAttr->nextAttr;
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)dictAttr);
  }

  for (dictAttr = vendorAttrsDefs; dictAttr; dictAttr = nextAttr)
  {
    nextAttr = dictAttr->nextAttr;
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)dictAttr);
  }

  for (dictValue = attrValueDefs; dictValue; dictValue = nextValue)
  {
    nextValue = dictValue->nextValue;
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)dictValue);
  }

  return;
}

/*********************************************************************
*
* @purpose Get the attribute definition for the given attribute id
*
* @param attrId @b((input)) attribute ID
*
* @returns pointer to the attribute definition
*
* @comments
*
* @end
*
*********************************************************************/
radiusDictAttr_t *radiusAttributeByIdGet(L7_uint32 attrId)
{
  radiusDictAttr_t *attr;

  for (attr = radiusAttrsDefs; attr != L7_NULL; attr = attr->nextAttr)
  {
    if (attr->attrId == (L7_int32)attrId)
    {
      break;
    }
  }

  return attr;
}

/*********************************************************************
*
* @purpose Get the attribute definition for the given attribute name
*
* @param attrName @b((input)) pointer to the attribute name string
*
* @returns pointer to the attribute definition
*
* @comments
*
* @end
*
*********************************************************************/
radiusDictAttr_t *radiusAttributeByNameGet(L7_char8 *attrName)
{
  radiusDictAttr_t *attr;

  for (attr = radiusAttrsDefs; attr != L7_NULL; attr = attr->nextAttr)
  {
    if (strcmp(attr->attrName, attrName) == L7_NULL)
    {
      break;
    }
  }

  return attr;
}

/*********************************************************************
*
* @purpose Get the vendor code for the given vendor name
*
* @param vendorName @b((input)) pointer to a vendor name string
*
* @returns vendor code or L7_NULL if not found
*
* @comments
*
* @end
*
*********************************************************************/
L7_int32 radiusVendorCodeGet(L7_char8 *vendorName)
{
  radiusDictAttr_t *attr;

  for (attr = vendorAttrsDefs; attr != L7_NULL; attr = attr->nextAttr)
  {
    if (strcmp(attr->attrName, vendorName) == L7_NULL)
    {
      return attr->vendorCode;
    }
  }

  return L7_NULL;
}

/*********************************************************************
*
* @purpose Get the attribute definition for the given vendor code and
*          vendor sub attribute id.
*
* @param vendorCode @b((input))
* @param vsAttrId @b((input))
*
* @returns pointer to the attribute definition
*
* @comments
*
* @end
*
*********************************************************************/
radiusDictAttr_t *radiusVSAttributeGet(L7_uint32 vendorCode,
                                       L7_uint32 vsAttrId)
{
  radiusDictAttr_t *attr;

  for (attr = radiusAttrsDefs; attr != L7_NULL; attr = attr->nextAttr)
  {
    if ((attr->attrId == RADIUS_ATTR_TYPE_VENDOR) &&
        (attr->vendorCode == (L7_int32)vendorCode) &&
        (attr->vsAttrId == (L7_int32)vsAttrId))
    {
      break;
    }
  }

  return attr;
}

/*********************************************************************
*
* @purpose Get the attribute value definition for the given value name
*
* @param valueName @b((input)) pointer to the name of the attribute
*
* @returns pointer to the attribute value
*
* @comments
*
* @end
*
*********************************************************************/
radiusDictValue_t *radiusAttrValueByValueNameGet(L7_char8 *valueName)
{
  radiusDictValue_t *value;

  for (value = attrValueDefs; value != L7_NULL; value = value->nextValue)
  {
    if (strcmp(value->valueName, valueName) == L7_NULL)
    {
      break;
    }
  }

  return value;
}

/*********************************************************************
*
* @purpose Get the attribute value definition for the given attribute
*          name and the value.
*
* @param valueId @b((input)) ID of the attribute
* @param attrName @b((input)) pointer to the name in the dictionary
*
* @returns pointer to the attribute value
*
* @comments
*
* @end
*
*********************************************************************/
radiusDictValue_t *radiusAttrValueByValueIdGet(L7_int32 valueId,
                                               L7_char8 *attrName)
{
  radiusDictValue_t *value;

  for (value = attrValueDefs; value != L7_NULL; value = value->nextValue)
  {
    if (strcmp(value->attrName, attrName) == L7_NULL && value->valueId == valueId)
    {
      break;
    }
  }

  return value;
}


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
                L7_uchar8 *file, L7_uint32 line_num)
{
  L7_RC_t rc;

  rc = osapiSemaTake(sem, L7_WAIT_FOREVER);

  radius_lock_taken = 1;
  l7utilsFilenameStrip((L7_char8 **)&file);
  osapiStrncpy (radius_lock_file,
          file,
          sizeof (radius_lock_file));
  radius_lock_line = line_num;

  return (rc);
}

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
L7_RC_t radiusSemaGive (void *sem)
{
  L7_RC_t rc;

  radius_lock_taken = 0;
  rc = osapiSemaGive(sem);

  return (rc);
}

/*********************************************************************
*
* @purpose  Debug function to show last function that took the
*           write lock.
*
* @param   none
*
* returns   none
*
* @comments
*
* @end
*
*********************************************************************/
int radiusDebugLock(void)
{
  sysapiPrintf("Lock Taken: %d\n", radius_lock_taken);
  sysapiPrintf("RADIUS Lock Last Take: %s (%d)\n",
            radius_lock_file,
            radius_lock_line);
  osapiSleepMSec (500);
  return 0;
}

