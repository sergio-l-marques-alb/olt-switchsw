/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_control.h
*
* @purpose RADIUS Client control header file
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
#ifndef INCLUDE_RADIUS_CONTROL_H
#define INCLUDE_RADIUS_CONTROL_H

#include "l7_cnfgr_api.h"

/*
** RADIUS timer constants for radius_timer_task
*/
#define RADIUS_TIMER_GRANULARITY 1000 /* milliseconds */
#define RADIUS_TIMER_TICK (RADIUS_TIMER_GRANULARITY / 2)

#define DEADTIME_EXTRA_GRANULARITY 60

/*
** RADIUS msg event types
*/
typedef enum radiusEvents_s
{
  /* Events from the UI tasks */
  RADIUS_MAX_RETRANS_SET = 0,
  RADIUS_TIMEOUT_SET,
  RADIUS_ACCT_ADMIN_MODE_SET,
  RADIUS_ACCT_IPADDR_ADD,
  RADIUS_ACCT_PORT_SET,
  RADIUS_ACCT_SECRET_SET,
  RADIUS_ACCT_IPADDR_DEL,
  RADIUS_DEADTIME_SET,
  RADIUS_SOURCEIP_SET,
  RADIUS_AUTH_SERVER_PRIORITY_SET,
  RADIUS_AUTH_IPADDR_ADD,
  RADIUS_AUTH_PORT_SET,
  RADIUS_AUTH_USAGE_TYPE_SET,
  RADIUS_AUTH_SECRET_SET,
  RADIUS_AUTH_IPADDR_DEL,
  RADIUS_ALL_SERVER_DEL,
  RADIUS_SERVER_TYPE_SET,
  RADIUS_SERVER_INC_MSG_AUTH_MODE_SET,
  RADIUS_REQUEST_INFO,
  RADIUS_AUTH_SERVER_NAME_SET,
  RADIUS_ACCT_SERVER_NAME_SET, 
  RADIUS_AUTH_SERVER_NAME_DEL,
  RADIUS_ACCT_SERVER_NAME_DEL, 

  /* Events from the radius_rx_task */
  RADIUS_RESPONSE_PACKET,

  /* Events from the radius_timer_task */
  RADIUS_TIMEOUT_CHECK,

  RADIUS_CNFGR_INIT,

  RADIUS_ATTRIBUTE_4_SET,

  RADIUS_ROUTING_INTF_ENABLE,
  RADIUS_ROUTING_INTF_DISABLE,

  RADIUS_EVENTS /* keep this last */

}radiusEvents_t;

typedef struct
{
  union
  {
    L7_char8                           str[L7_RADIUS_MAX_SECRET+1];  /* allow for NULL terminator on string */
    L7_uint32                          number;
  } val;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t localGlobal;
}radiusAuthServerInfo_t;

/* 
** RADIUS message structure for radius_task processing
*/
typedef struct
{
  L7_uint32             event;
  dnsHost_t             hostAddr;
  union
  {
    L7_uint32           number;
    L7_BOOL             mode;
    L7_char8            secret[L7_RADIUS_MAX_SECRET+1];
    radiusRequestInfo_t *requestInfo;
    radiusPacket_t      *responsePacket;
    L7_CNFGR_CMD_DATA_t cmdData;
    L7_uchar8           hostName[L7_DNS_HOST_NAME_LEN_MAX];
    L7_char8            serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];
    radiusAuthServerInfo_t authServerInfo;
  } data;

} radiusMsg_t;


/*
** Internal function prototypes
*/

L7_RC_t radiusStartTasks();

void radius_task(void);

void radius_rx_task(void);

L7_RC_t radiusIssueCmd(L7_uint32 event, 
                       dnsHost_t *hostAddr, 
                       void *data);

L7_RC_t radiusFillMsg(void *data, 
                      radiusMsg_t *msg);

void radiusDispatchCmd(radiusMsg_t *msg);

L7_RC_t radiusEventMaxNoOfRetransSet(dnsHost_t                           *hostAddr,
                                     L7_uint32                            maxRetrans,
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

L7_RC_t radiusEventTimeOutDurationSet(dnsHost_t                           *hostAddr,
                                      L7_uint32                            timeOutDuration,
                                      L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

L7_RC_t radiusEventServerSharedSecretSet(dnsHost_t                           *hostAddr,
                                         L7_char8                            *secret,
                                         L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

L7_RC_t radiusEventServerPrioritySet(dnsHost_t *hostAddr,
                                     L7_uint32  priority);

L7_RC_t radiusEventServerUsageTypeSet(dnsHost_t                     *hostAddr,
                                      L7_RADIUS_SERVER_USAGE_TYPE_t  usageType);

L7_RC_t radiusEventDeadtimeSet(dnsHost_t                           *hostAddr,
                               L7_uint32                            deadtime,
                               L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus);

L7_RC_t radiusEventSourceIPSet(dnsHost_t *hostAddr,
                               L7_uint32  sourceIPAddr,
                               L7_uint32  paramStatus);



L7_RC_t radiusEventAccountingServerModeSet(L7_BOOL mode);

L7_RC_t radiusEventAccountingServerPortNumSet(dnsHost_t *hostAddr, 
                                              L7_uint32  portNum);

L7_RC_t radiusEventAccountingServerSharedSecretSet(dnsHost_t *hostAddr, 
                                                   L7_char8  *secret);

L7_RC_t radiusEventServerHostNameAdd(dnsHost_t *hostAddr,
    L7_uint32 serverType, L7_char8 *serverName);

L7_RC_t radiusEventServerHostNameRemove(dnsHost_t *hostAddr,
    L7_uint32 type);

L7_RC_t radiusEventAllServerRemove(void);

L7_RC_t radiusEventServerPortNumSet(dnsHost_t *hostAddr, 
                                    L7_uint32 portNum);

L7_RC_t radiusEventServerEntryTypeSet(dnsHost_t *hostAddr, 
                                      L7_uint32 entryType);

L7_RC_t radiusEventServerIncMsgAuthModeSet(dnsHost_t *hostAddr, 
                                           L7_uint32 mode);

void radiusResponseNotify(radiusRequestInfo_t *requestInfo, 
                          radiusPacket_t *packet);

/*********************************************************************
 *
 * @purpose Select a RADIUS server of the specified type (AUTH or ACCT),
 *          preferably the PRIMARY server in the named servers group 
 *          referenced by the serverNameIndex.
 *
 * @param L7_uint32 serverType @b((input))
 * @param L7_uint32 serverNameIndex@b((input)) Index to the array of 
 *          server names.
 *
 * @returns selected RadiusServerEntry pointer
 *
 * @comments This routine does no locking. Caller must hold 
 *          radiusTaskSyncSema.
 *           
 * @end
 *
 *********************************************************************/
radiusServerEntry_t *radiusServerSelect(L7_uint32 serverType, L7_RADIUS_SERVER_USAGE_TYPE_t usageType, 
                     L7_uint32 serverNameIndex);

radiusServerEntry_t *radiusServerReSelect(L7_uint32 serverType);
/*********************************************************************
 *
 * @purpose Delete the marked RadiusServer entry from the server list.
 *
 * @returns void
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusServerEntryDelete(void);

/*********************************************************************
 *
 * @purpose Propagate a RADIUS server configuration change in the named 
 *          servers group referenced by the serverNameIndex.
 *
 * @param eventType @b((input)) type of configuration change
 * @param serverNameIndex@b((input)) Index to the array of server names.
 * @returns void
 *
 * @comments Scan through the entries and depending on the status of the
 *           entries, either keep the existing connections, open or
 *           close and open new connections with a different server.
 *           This routine does no locking. Caller must hold 
 *           radiusTaskSyncSema.
 * @end
 *
 *********************************************************************/
L7_RC_t radiusConfigChangePropagate(L7_uint32 eventType, L7_uint32 serverNameIndex);

L7_RC_t radiusResponsePacketProcess(L7_uint32 serverAddress, radiusPacket_t *packet);

L7_RC_t radiusClientShutdown(void);

L7_RC_t radiusClientTimer(void);

/*********************************************************************
 *
 * @purpose Retrieve the IP address of the primary server from the 
 *          named servers group referenced by nameIndex.
 *
 * @param serverNameIndex@b((input)) Index to the array of server names.
 * @param serverAddr @b((output)) IP address of the radius server
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold 
 *          radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusPrimaryServerAddressGet(L7_uint32 nameIndex, 
                     dnsHost_t *serverAddr);

L7_RC_t radiusAttribute4SetHandle(L7_BOOL mode, L7_uint32 ipAddr);

/*********************************************************************
* @purpose Get the index of the named server.
*
* @param L7_char8 *serverName @b((input)) Pointer to the Name of the server.
* @param L7_uint32 *index@b((output)) Pointer to the Index to the array of 
*           server names.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This can also be used to verify whether a specified serverName 
*           entry exists.
*           Range for index of nameArray[], outside these APIs is 1-32.  
*           And inside 0-31.

* @end
*
*********************************************************************/
L7_RC_t radiusServerNameEntryIndexGet(L7_char8 *serverName, L7_uint32 *index );

/*********************************************************************
*
* @purpose Get the indexed server's name.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This can also be used to verify whether a specified serverName 
*           entry exists.
*           Range for index of nameArray[], outside the API is 1-32.  
*           And inside 0-31.
*       
* @end
*
*********************************************************************/
L7_RC_t radiusServerNameEntryGet(L7_uint32 index,L7_char8 *serverName);
L7_RC_t radiusServerAuthCurrentEntryGet(L7_uint32 index, L7_RADIUS_SERVER_USAGE_TYPE_t usageType, radiusServerEntry_t **serverEntry);

/*********************************************************************
*
* @purpose To update the reference to currentEntry in indexed
*          server name entry.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Callers h'v to ensure that the same name being not set again.
*       
* @end
*
*********************************************************************/
L7_RC_t radiusServerAuthCurrentEntrySet(L7_uint32 index, L7_RADIUS_SERVER_USAGE_TYPE_t usageType, radiusServerEntry_t *serverEntry);

/*********************************************************************
*
* @purpose Add/Update the given server to the group of servers tagged by 
*           the specified serverName.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Callers h'v to ensure that the same name being not set again.
*       
* @end
*
*********************************************************************/
L7_RC_t radiusServerNameAddUpdate(radiusServerEntry_t *serverEntry, L7_char8 *serverName);

/*********************************************************************
 *
 * @purpose To remove the name from the specified server.
 *
 * @param serverEntry @b((input)) pointer to server entry. 
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusServerNameUpdateRemove(radiusServerEntry_t *serverEntry);
L7_RC_t radiusEventAuthServerNameSet(dnsHost_t *hostAddr, L7_char8 *serverName);
L7_RC_t radiusAuthServerNameSet(radiusServerEntry_t *serverEntry, L7_char8 *serverName);
/*********************************************************************
 *
 * @purpose To get the next assinged Name entry.
 *
 * @param index @b((input)) index to the server entry
 * @param nextIndex @b((input)) index to the next assigned server entry 
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments The index range being used while actually accessing array[] 
 *           is 0-31. 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusServerNameEntryNextGet(L7_uint32 index, L7_uint32 *nextIndex);

L7_RC_t radiusAuthServerNameEntryDetailedGet(L7_uint32 ind,L7_char8 *serverName,L7_char8  *currentServer,L7_uint32 *count, void *currentEntry);

/*********************************************************************
*
* @purpose Add/Update the given server to the group of servers tagged by 
*           the specified serverName.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Callers h'v to ensure that the same name being not set again.
*       
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerNameAdd(radiusServerEntry_t *serverEntry, L7_char8 *serverName);

/*********************************************************************
 *
 * @purpose To remove the name from the specified server.
 *
 * @param serverEntry @b((input)) pointer to server entry. 
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAcctServerNameRemove(radiusServerEntry_t *serverEntry);

/*********************************************************************
*
* @purpose Get the indexed server's name.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This can also be used to verify whether a specified serverName 
*           entry exists.
*           Range for index of nameArray[], outside the API is 1-32.  
*           And inside 0-31.
*       
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerNameEntryGet(L7_uint32 index,L7_char8 *serverName);

/*********************************************************************
 *
 * @purpose To get the next assinged Name entry.
 *
 * @param index @b((input)) index to the server entry
 * @param nextIndex @b((input)) index to the next assigned server entry 
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments The index range being used while actually accessing array[] 
 *           is 0-31. 
 *
 * @end
 *
 *********************************************************************/

L7_RC_t radiusAcctServerNameEntryIndexNextGet(L7_uint32 index, L7_uint32 *nextIndex);

/*********************************************************************
 *
 * @purpose To set the name to the specified server.
 *
 * @param hostAddr @b((input)) host address.
 * @param serverName @b((input)) server name.
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventAcctServerNameSet(dnsHost_t *hostAddr, L7_char8 *serverName);

/*********************************************************************
*
* @purpose Get the index of the named server.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This can also be used to verify whether a specified serverName 
*           entry exists.
*           Range for index of nameArray[], outside these APIs is 1-32.  
*           And inside 0-31.

* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerNameEntryIndexGet(L7_char8 *serverName, L7_uint32 *index );

/*********************************************************************
 *
 * @purpose Add a command to the RADIUS Client msg queue
 *
 * @param event @b((input)) event type
 * @param serverEntry @b((input)) pointer to the current server
 * @param data @b((input)) pointer to data
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments command will be queued for service
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAcctServerNameEntryDetailedGet(L7_uint32 index,L7_char8 *serverName,L7_uint32 *count);

/*********************************************************************
 *
 * @purpose To remove the name attribute from the specified RADIUS 
 *          Acct. server.
 *
 * @param radiusServerEntry_t *serverEntry @b((input)) pointer to server entry. 
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAcctServerNameUpdateRemove(radiusServerEntry_t *serverEntry);

/*********************************************************************
*
* @purpose Get the current server from indexed name entry of servernamearray[].
*
* @param serverEntry @b((input)) RADIUS server
* @param serverEntry @b((output)) pointer to the current server entry.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments 
index points to the server name entry in servernameArray[]
*           from which curr Group of servers tagged This can also be used to verify whether a specified serverName 
*           entry exists.
*           Range for index of nameArray[], outside the API is 1-32.  
*           And inside 0-31.
*       
* @end
*
*********************************************************************/
L7_RC_t radiusServerAcctCurrentEntryGet(L7_uint32 index,radiusServerEntry_t **serverEntry);

/*********************************************************************
*
* @purpose To update the reference to currentEntry in indexed
*          Acct. server name entry.
*
* @param serverEntry @b((input)) RADIUS server
* @param serverName @b((input)) Name of the server.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments NOne.
*       
* @end
*
*********************************************************************/
L7_RC_t radiusServerAcctCurrentEntrySet(L7_uint32 index,radiusServerEntry_t *serverEntry);

/*********************************************************************
*
* @purpose Get the No. of radius servers configured with this name.
*
* @param L7_uint32 index @b((input)) Index to RADIUS server name entry in 
*                  server name array.
* @param L7_uint32 *count @b((output)) Pointer to the count.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t radiusServerNameEntryCountGet(L7_uint32 index, L7_uint32 *count);

/*********************************************************************
 *
 * @purpose House keeping for sockets bound on ipaddresses specifc to
 *          routing/loopback network interfaces.
 *
 * @param  none.
 * @returns none
 *
 * @comments none.
 *
 * @end
 *
 *********************************************************************/
void radiusNwInterfaceSocketUpdate(void);

/*********************************************************************
 *
 * @purpose To get the next RADIUS Auth. server Name entry.
 *
 * @param L7_char8 *name b((input)) Pointer to the name of the server
 * @param L7_char8 *nextName b((output)) Pointer to the name of the next server
 * @param radiusServerEntry_t **serverEntry @b((output)) Pointer to 
 *           pointer to next server entry.
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments The index range being used while actually accessing array[] 
 *           is 0-31. 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAuthServerNameEntryOrderlyGetNext(L7_char8 *name,
                     L7_char8 *nextName, radiusServerEntry_t **serverEntry);

/*********************************************************************
 *
 * @purpose To get the next RADIUS Acct. server Name entry 
 *           in alphabetical order.
 *
 * @param L7_char8 *name b((input)) Pointer to the name of the server
 * @param L7_char8 *nextName b((output)) Pointer to the name of the next server
 * @param radiusServerEntry_t **serverEntry @b((output)) Pointer to 
 *           pointer to next server entry.
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments None.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAcctServerNameEntryOrderlyGetNext(L7_char8 *name, 
                     L7_char8 *nextName, radiusServerEntry_t **serverEntry);



/*********************************************************************
 *
 * @purpose Checks to see if any RADIUS servers need to have their
 *          socket rebound. This can happen if a server is configured
 *          to use a particular source IP address. In this case, we 
 *          need to re-bind the socket once the interface becomes available.
 *
 * @comments 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventRoutingIntfChangeProcess(radiusEvents_t event, L7_uint32 intIfNum);

#endif /* INCLUDE_RADIUS_CONTROL_H */

