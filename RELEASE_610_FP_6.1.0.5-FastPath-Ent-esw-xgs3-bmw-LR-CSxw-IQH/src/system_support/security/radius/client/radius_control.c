/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 * @filename radius_control.c
 *
 * @purpose RADIUS Client control file
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
#include "dns_client_api.h"
#ifdef L7_ROUTING_PACKAGE
#include "l7_ip_api.h"
#include "l7_socket.h"
#endif

void * radiusTaskSyncSema = L7_NULL;
void *radiusServerDbSyncSema = L7_NULL;

void *radius_queue = L7_NULL;
void *radiusClusteringQueue = L7_NULL;

L7_uint32 radius_task_id = L7_ERROR;
L7_uint32 radius_rx_task_id = L7_ERROR;
L7_uint32 radius_cluster_task_id = L7_ERROR;


extern radiusCfg_t radiusCfg;
extern L7_uint32 radius_sm_bp_id;
extern L7_uint32 radius_lg_bp_id;

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
static void radiusNwInterfaceSocketDeleteAll(void);
#endif

/*
 ** RADIUS client global data structures
 */
radiusGlobalInfo_t      radiusGlobalData;
radiusServerEntry_t     *radiusServerEntries;

/*server name arrays.*/
serverName_t  authServerNameArray[L7_RADIUS_MAX_AUTH_SERVERS];
serverName_t  acctServerNameArray[L7_RADIUS_MAX_ACCT_SERVERS];

/*********************************************************************
 *
 * @purpose Initialize RADIUS Client tasks and data
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusStartTasks()
{
  L7_RC_t rc = L7_SUCCESS;

  /* create radius_task */
  radius_task_id = (L7_uint32)osapiTaskCreate("radius_task",
      (void *)radius_task,
      L7_NULL,
      L7_NULL,
      L7_DEFAULT_STACK_SIZE,
      L7_DEFAULT_TASK_PRIORITY,
      L7_DEFAULT_TASK_SLICE);

  if (osapiWaitForTaskInit(L7_RADIUS_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    RADIUS_ERROR_SEVERE("RADIUS: Unable to initialize radius_task\n");
    return L7_FAILURE;
  }

  /* create radius_rx_task */
  radius_rx_task_id = (L7_uint32)osapiTaskCreate("radius_rx_task",
      (void *)radius_rx_task,
      L7_NULL,
      L7_NULL,
      L7_DEFAULT_STACK_SIZE,
      L7_DEFAULT_TASK_PRIORITY,
      L7_DEFAULT_TASK_SLICE);

  if (osapiWaitForTaskInit(L7_RADIUS_RX_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    RADIUS_ERROR_SEVERE("RADIUS: Unable to initialize radius_rx_task\n");
    return L7_FAILURE;
  }

  /* create radius_cluster_task */

  if( clusterSupportGet() == L7_SUCCESS)
  {
      LOG_MSG("RADIUS_CLUSTER: Cluster component present.\n");

      radius_cluster_task_id = (L7_uint32)osapiTaskCreate("radius_cluster_task",
          (void *)radius_cluster_task,
          L7_NULL,
          L7_NULL,
          L7_DEFAULT_STACK_SIZE,
          L7_DEFAULT_TASK_PRIORITY,
          L7_DEFAULT_TASK_SLICE);

      if (osapiWaitForTaskInit(L7_RADIUS_CLUSTER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
      {
        RADIUS_ERROR_SEVERE("RADIUS: Unable to initialize radius_cluster_task\n");
        return L7_FAILURE;
      }
  }

  if (radius_task_id == L7_ERROR ||
      radius_rx_task_id == L7_ERROR ||
      ((radius_cluster_task_id == L7_ERROR) && (clusterSupportGet() == L7_SUCCESS)))
  {
    RADIUS_ERROR_SEVERE("RADIUS: Failed to create a task\n");
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose RADIUS Client task which serves the message queue
 *
 * @returns void
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
void radius_task(void)
{
  radiusMsg_t msg;
  L7_uint32 status;
  L7_uint32 last_timer_check;

  (void)osapiTaskInitDone(L7_RADIUS_TASK_SYNC);

  /*
  ** Loop forever waiting for RADIUS messages
  */
  last_timer_check = osapiTimeMillisecondsGet();
  while (L7_TRUE)
  {
    status = osapiMessageReceive(radius_queue,
                                 (void *)&msg,
                                 (L7_uint32)sizeof(radiusMsg_t),
                                 RADIUS_TIMER_TICK);

    if ((status == L7_SUCCESS) &&
        (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS))
    {
      /*
      ** Process the message
      */
      radiusDispatchCmd(&msg);

      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
    }

    /* Check whether any timeouts need to be handled.
     */
    if ((osapiTimeMillisecondsGet() - last_timer_check) > RADIUS_TIMER_TICK)
    {
      (void) RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER);
      memset (&msg, 0, sizeof (msg));
      msg.event = RADIUS_TIMEOUT_CHECK;
      radiusDispatchCmd(&msg);
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      last_timer_check = osapiTimeMillisecondsGet();
    }
  }

  return;
}

/*********************************************************************
 *
 * @purpose Processes any messages received from RADIUS servers.
 *
 * @returns void
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
void radius_rx_task(void)
{
  (void)osapiTaskInitDone(L7_RADIUS_RX_TASK_SYNC);

  /* Loop forever checking for incomming RADIUS packets */
  while (L7_TRUE)
  {
    if (radiusServerEntries != L7_NULL)
    {
      /* Check for any packets comming from the RADIUS servers */
      radiusPacketReceive();
    }
    else
    {
      /* Wait for a valid server configuration */
      osapiSleep(L7_RADIUS_VALID_SERVER_WAIT);
    }
  }
}

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
L7_RC_t radiusIssueCmd(L7_uint32 event,
                       dnsHost_t *hostAddr,
                       void *data)
{
  L7_RC_t rc;
  radiusMsg_t msg;

  /* Copy event, server and data to msg struct */
  msg.event = event;
  memset(&msg.hostAddr, 0, sizeof(dnsHost_t));
  if(hostAddr != L7_NULL)
  {
    memcpy(&msg.hostAddr, hostAddr, sizeof(dnsHost_t));
  }

  if ((rc = radiusFillMsg(data, &msg)) == L7_SUCCESS)
  {
    /* Send msg */
    if ((rc = osapiMessageSend(radius_queue,
                               &msg,
                               (L7_uint32)sizeof(radiusMsg_t),
                               L7_NO_WAIT,
                               L7_MSG_PRIORITY_NORM)) != L7_SUCCESS)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: Msg Queue is full! Event: %u\n", event);
    }
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Save the data in a message to a shared memory
 *
 * @param event @b((input)) event type
 * @param *data @b((input)) pointer to data
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments Once the message is serviced, this variable size data will
 *           be retrieved
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusFillMsg(void *data,
    radiusMsg_t *msg)
{
  switch (msg->event)
  {
    /**************************************************/
    /* events originating from users, CLI, WEB, etc.. */
    /**************************************************/
    case RADIUS_ACCT_PORT_SET:
    case RADIUS_AUTH_PORT_SET:
    case RADIUS_AUTH_SERVER_PRIORITY_SET:
    case RADIUS_AUTH_USAGE_TYPE_SET:
    case RADIUS_SERVER_TYPE_SET:
      /* add to queue L7_uint32 size */
      if (data != L7_NULLPTR)
      {
        memcpy(&msg->data.number, data, sizeof(L7_uint32));
      }

      break;
    case RADIUS_AUTH_IPADDR_ADD:
    case RADIUS_ACCT_IPADDR_ADD:
    case RADIUS_AUTH_SERVER_NAME_SET:
    case RADIUS_ACCT_SERVER_NAME_SET:
    case RADIUS_AUTH_SERVER_NAME_DEL:
      msg->data.number = L7_NULL;
      if (data != L7_NULLPTR)
      {
        /* add serverName to msg. */
        memset(msg->data.serverName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
        osapiStrncpySafe(msg->data.serverName,(L7_char8*)data,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);
      }
      break;

    case RADIUS_ACCT_IPADDR_DEL:
    case RADIUS_AUTH_IPADDR_DEL:
      msg->data.number = L7_NULL;
      break;
    case RADIUS_ACCT_ADMIN_MODE_SET:
    case RADIUS_SERVER_INC_MSG_AUTH_MODE_SET:
      if (data != L7_NULLPTR)
      {
        /* add to queue L7_ENABLE/L7_DISABLE size */
        memcpy(&msg->data.mode, data, sizeof(L7_uint32));
      }
      break;
    case RADIUS_ACCT_SECRET_SET:
      if (data != L7_NULLPTR)
      {
        /* add to queue a secret */
        osapiStrncpySafe((L7_char8 *)&msg->data.secret, (L7_char8 *)data, (L7_uint32)L7_RADIUS_MAX_SECRET+1);
      }
      break;
    case RADIUS_ALL_SERVER_DEL:
      /* add to queue null data */
      msg->data.number = L7_NULL;

      break;
      /***************************************************/
      /* events originating from radius services api use */
      /***************************************************/
    case RADIUS_REQUEST_INFO:
      /* add to queue a RADIUS requestInfo */
      msg->data.requestInfo = (radiusRequestInfo_t *)data;

      break;
      /***************************************/
      /* events originating from osapiSocket */
      /***************************************/
    case RADIUS_RESPONSE_PACKET:
      /* add to queue a pointer to a RADIUS response */
      msg->data.responsePacket = (radiusPacket_t *)data;

      break;
      /*********************************************/
      /* events originating from radius_timer_task */
      /*********************************************/
    case RADIUS_TIMEOUT_CHECK:
      /* add to queue null data */
      msg->data.number = L7_NULL;

      break;

    case RADIUS_ATTRIBUTE_4_SET:
      {
        memcpy(&msg->data.mode,data, sizeof(L7_BOOL));
      }
      break;

    case RADIUS_MAX_RETRANS_SET:
    case RADIUS_TIMEOUT_SET:
    case RADIUS_DEADTIME_SET:
    case RADIUS_SOURCEIP_SET:
    case RADIUS_AUTH_SECRET_SET:
      /* add to queue radiusAuthServerInfo_t size */
      if (data != L7_NULLPTR)
      {
        memcpy(&msg->data.authServerInfo, data, sizeof(radiusAuthServerInfo_t));
      }
      break;
    case RADIUS_ROUTING_INTF_ENABLE:
    case RADIUS_ROUTING_INTF_DISABLE:
      memcpy(&msg->data.number, data, sizeof(L7_uint32));
      break;
      /*******************/
      /* unmatched event */
      /*******************/
    default:
      LOG_MSG("RADIUS: Invalid event received\n");
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Dispatch the event to a handling function
 *
 * @param msg @b((input)) message structure with event and parms
 *
 * @returns void
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
void radiusDispatchCmd(radiusMsg_t *msg)
{
  L7_RC_t rc;

  switch (msg->event)
  {
    case RADIUS_MAX_RETRANS_SET:
      rc = radiusEventMaxNoOfRetransSet(&msg->hostAddr,
                                        msg->data.authServerInfo.val.number,
                                        msg->data.authServerInfo.localGlobal);

      break;
    case RADIUS_TIMEOUT_SET:
      rc = radiusEventTimeOutDurationSet(&msg->hostAddr,
                                         msg->data.authServerInfo.val.number,
                                         msg->data.authServerInfo.localGlobal);

      break;
    case RADIUS_ACCT_ADMIN_MODE_SET:
      rc = radiusEventAccountingServerModeSet(msg->data.mode);

      break;
    case RADIUS_ACCT_IPADDR_ADD:
      rc = radiusEventServerHostNameAdd(&msg->hostAddr, RADIUS_SERVER_TYPE_ACCT,msg->data.serverName);
      (void)RADIUS_LOCK_GIVE(radiusServerDbSyncSema);

      break;

    case RADIUS_ACCT_PORT_SET:
      rc = radiusEventAccountingServerPortNumSet(&msg->hostAddr, msg->data.number);
      break;
    case RADIUS_ACCT_SECRET_SET:
      rc = radiusEventAccountingServerSharedSecretSet(&msg->hostAddr, msg->data.secret);
      break;
    case RADIUS_ACCT_IPADDR_DEL:
      rc = radiusEventServerHostNameRemove(&msg->hostAddr,
                                           RADIUS_SERVER_TYPE_ACCT);
      break;
    case RADIUS_AUTH_IPADDR_ADD:
      rc = radiusEventServerHostNameAdd(&msg->hostAddr,
                                        RADIUS_SERVER_TYPE_AUTH,msg->data.serverName);
      (void)RADIUS_LOCK_GIVE(radiusServerDbSyncSema);
      break;
    case RADIUS_AUTH_PORT_SET:
      rc = radiusEventServerPortNumSet(&msg->hostAddr, msg->data.number);
      break;
    case RADIUS_AUTH_USAGE_TYPE_SET:
      rc = radiusEventServerUsageTypeSet(&msg->hostAddr, msg->data.number);
      break;
    case RADIUS_AUTH_SECRET_SET:
      rc = radiusEventServerSharedSecretSet(&msg->hostAddr,
                                            msg->data.authServerInfo.val.str,
                                            msg->data.authServerInfo.localGlobal);
      break;
    case RADIUS_AUTH_IPADDR_DEL:
      rc = radiusEventServerHostNameRemove(&msg->hostAddr, RADIUS_SERVER_TYPE_AUTH);
      break;
    case RADIUS_ALL_SERVER_DEL:
      rc = radiusEventAllServerRemove();

      break;
    case RADIUS_SERVER_TYPE_SET:
      rc = radiusEventServerEntryTypeSet(&msg->hostAddr, msg->data.number);
      break;
    case RADIUS_SERVER_INC_MSG_AUTH_MODE_SET:
      rc = radiusEventServerIncMsgAuthModeSet(&msg->hostAddr, msg->data.mode);
      break;
    case RADIUS_REQUEST_INFO:
      /* requestInfo received from a radius services user */
      rc = radiusRequestInfoProcess(msg->data.requestInfo);

      break;
    case RADIUS_RESPONSE_PACKET:
      /* responsePacket received from a radius server */
      rc = radiusResponsePacketProcess(msg->hostAddr.host.ipAddr,
                                       msg->data.responsePacket);
      break;
    case RADIUS_TIMEOUT_CHECK:
      /* radius timer pop */
      rc = radiusClientTimer();

      break;
    case RADIUS_CNFGR_INIT:
      /* signal from radiusApiCnfgrCommand() */
      radiusCnfgrHandle(&msg->data.cmdData);

      break;
    case RADIUS_ATTRIBUTE_4_SET:
      radiusAttribute4SetHandle(msg->data.mode,msg->hostAddr.host.ipAddr);
      break;
    case RADIUS_AUTH_SERVER_NAME_SET:
      rc = radiusEventAuthServerNameSet(&msg->hostAddr, msg->data.serverName);
      break;

    case RADIUS_ACCT_SERVER_NAME_SET:
      rc = radiusEventAcctServerNameSet(&msg->hostAddr, msg->data.serverName);
      break;

    case RADIUS_DEADTIME_SET:
      rc = radiusEventDeadtimeSet(&msg->hostAddr,
                                  msg->data.authServerInfo.val.number,
                                  msg->data.authServerInfo.localGlobal);
      break;
    case RADIUS_SOURCEIP_SET:
      rc = radiusEventSourceIPSet(&msg->hostAddr,
                                  msg->data.authServerInfo.val.number,
                                  msg->data.authServerInfo.localGlobal);
      break;
    case RADIUS_AUTH_SERVER_PRIORITY_SET:
      rc = radiusEventServerPrioritySet(&msg->hostAddr,
                                        msg->data.number);
      break;

    case RADIUS_ROUTING_INTF_ENABLE:
    case RADIUS_ROUTING_INTF_DISABLE:
      rc = radiusEventRoutingIntfChangeProcess(msg->event, msg->data.number);
      break;

    default:
      LOG_MSG("RADIUS: Unknown message event %u\n", msg->event);
  }

  return;
}

/*********************************************************************
 *
 * @purpose Set the value of maxNumRetrans for a server or globally.
 *
 * @param hostAddr    @b((input)) address of the server (individual/all)
 * @param maxRetrans  @b((input)) maximum retransmission value to be set
 * @param paramStatus @b((input)) should be set for a specific server or globally
 *
 * @returns L7_SUCCESS
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventMaxNoOfRetransSet(dnsHost_t                           *hostAddr,
                                     L7_uint32                            maxRetrans,
                                     L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusServerEntry_t *serverEntry;

  if((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS))
  { /* global RADIUS parameter; ignore paramStatus */
    radiusClient.maxNumRetrans = maxRetrans;
    /* Apply it to all servers that don't have local values set */
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
          serverEntry = serverEntry->nextEntry)
    {
      if ((serverEntry->serverConfig.localConfigMask & L7_RADIUS_SERVER_CONFIG_MAX_NUM_RETRANS) == 0)
      {
        serverEntry->serverConfig.maxNumRetrans = radiusClient.maxNumRetrans;
      }
    }
  }
  else /* server specific setting*/
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }
    if(L7_NULLPTR == serverEntry)
    {
      LOG_MSG("RADIUS: radiusEventMaxNoOfRetransSet(): server entry not found \n");
      return L7_FAILURE;
    }

    if (paramStatus == L7_RADIUSGLOBAL)
    {
      /* Copy global value to server-specific value. Ignore maxRetrans arg. */
      serverEntry->serverConfig.localConfigMask &= ~L7_RADIUS_SERVER_CONFIG_MAX_NUM_RETRANS;
      serverEntry->serverConfig.maxNumRetrans = radiusClient.maxNumRetrans;
    }
    else
    {
      serverEntry->serverConfig.localConfigMask |= L7_RADIUS_SERVER_CONFIG_MAX_NUM_RETRANS;
      serverEntry->serverConfig.maxNumRetrans = maxRetrans;
    }
  }
  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose set the value of the global parameter - timeOutDuration in
 *          the global radiusClient config structure.
 *
 * @param hostAddr        @b((input)) address of the server (individual/all)
 * @param timeoutDuration @b((input)) time out duration to be set
 * @param paramStatus     @b((input)) should be set for a specific server or globally
 *
 * @returns L7_SUCCESS
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventTimeOutDurationSet(dnsHost_t                           *hostAddr,
                                      L7_uint32                            timeOutDuration,
                                      L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusServerEntry_t *serverEntry;

  if((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS))
  { /* global RADIUS parameter; ignore paramStatus */
    radiusClient.timeOutDuration = timeOutDuration * RADIUS_TIMER_GRANULARITY;
    /* Apply it to all servers that don't have local values set */
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
          serverEntry = serverEntry->nextEntry)
    {
      if ((serverEntry->serverConfig.localConfigMask & L7_RADIUS_SERVER_CONFIG_TIMEOUT) == 0)
      {
        serverEntry->serverConfig.timeOutDuration = radiusClient.timeOutDuration;
      }
    }
  }
  else /* server specific setting*/
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }
    if(L7_NULLPTR == serverEntry)
    {
      LOG_MSG("RADIUS: radiusEventTimeOutDurationSet(): server entry not found \n");
      return L7_FAILURE;
    }

    if (paramStatus == L7_RADIUSGLOBAL)
    {
      /* Copy global value to server-specific value. Ignore timeOutDuration arg. */
      serverEntry->serverConfig.localConfigMask &= ~L7_RADIUS_SERVER_CONFIG_TIMEOUT;
      serverEntry->serverConfig.timeOutDuration = radiusClient.timeOutDuration;
    }
    else
    {
      serverEntry->serverConfig.localConfigMask |= L7_RADIUS_SERVER_CONFIG_TIMEOUT;
      serverEntry->serverConfig.timeOutDuration = timeOutDuration * RADIUS_TIMER_GRANULARITY;
    }
  }
  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the accouting mode to enable or disable in
 *          the global radiusClient config structure.
 *
 * @param mode @b((input)) true/false to enable/disable accounting
 *
 * @returns L7_SUCCESS
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventAccountingServerModeSet(L7_BOOL mode)
{

  if (mode == L7_TRUE)
  {
    /* Enbale RADIUS services users to do Accounting */
    radiusClient.acctAdminMode = L7_ENABLE;
  }
  else
  {
    /* Disable Accounting */
    radiusClient.acctAdminMode = L7_DISABLE;
  }

  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the port number for the accounting server with specified
 *          ip address
 *
 * @param serverIPAddr @b((input)) IP Address of the accounting server
 * @param portNum @b((input)) new value of the port number
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventAccountingServerPortNumSet(dnsHost_t *hostAddr,
    L7_uint32 portNum)
{
  radiusServerEntry_t *serverEntry;

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
  }

  if (serverEntry == L7_NULL)
  {
    /* No entry with serverIPAddr was found */
    return L7_FAILURE;
  }

  serverEntry->radiusServerConfigUdpPort = (L7_ushort16)portNum;
  serverEntry->serverConfig.radiusServerConfigUdpPort = (L7_ushort16)portNum;

  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the shared secret for the accounting server with specified ip address
 *
 * @param serverIPAddr @b((input)) IP Address of the accounting server
 * @param secret @b((input)) new value of the shared secret
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventAccountingServerSharedSecretSet(dnsHost_t *hostAddr,
    L7_char8 *secret)
{
  radiusServerEntry_t *serverEntry;

  if (strlen(secret) > L7_RADIUS_MAX_SECRET)
  {
    /* Secret is longer than allowed */
    return L7_FAILURE;
  }

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
  }

  if (serverEntry == L7_NULL)
  {
    /* No entry with serverIPAddr was found */
    return L7_FAILURE;
  }

  osapiStrncpySafe(serverEntry->serverConfig.radiusServerConfigSecret,
                   secret,
                   sizeof(serverEntry->serverConfig.radiusServerConfigSecret));
  serverEntry->serverConfig.radiusServerIsSecretConfigured = L7_TRUE;

  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Add a RADIUS server entry with the Host name specified
 *          to the server list. This routine does the DNS name lookup
 *          If DNS lookup is successful serverIP address is updated.
 *          If DNS lookup is failed serverIP is resolved later
 *
 * @param serverAddr   @b((input)) Describes host name
 * @param serverType   @b((input)) The radius server type acct or auth
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventServerHostNameAdd(dnsHost_t *hostAddr,
    L7_uint32 serverType, L7_char8 *serverName)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 index=0;
  L7_uint32 ipaddr = 0;
  L7_char8  fqhostname[L7_DNS_DOMAIN_NAME_SIZE_MAX]; /* fully qualified host name */
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_RC_t rc = L7_FAILURE;
  radiusClient_t *radiusClientPtr;

  /*
   ** Does the entry already exist ? If yes, don't add.
   */
  for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
      serverEntry = serverEntry->nextEntry)
  {
    if((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_DNS) &&
       (strcmp(serverEntry->radiusServerDnsHostName.host.hostName, hostAddr->host.hostName) == 0) &&
       serverEntry->serverConfig.radiusServerConfigServerType == serverType)
    {
      break;
    }
    else if ((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) &&
    (serverEntry->serverConfig.radiusServerConfigIpAddress == hostAddr->host.ipAddr)  &&
        serverEntry->serverConfig.radiusServerConfigServerType == serverType)
    {
      break;
    }


    /* Identify the index value that is to be assigned to the new server being
     * configured
     */
    if (serverEntry->serverConfig.radiusServerConfigServerType == serverType)
      index++;
  }

  if (serverEntry != L7_NULL)
    return L7_FAILURE;

  /*
   ** Allocate memory for the entry and initialize the entry
   */
  if (bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&serverEntry) != L7_SUCCESS)
  {
    RADIUS_ERROR_SEVERE("RADIUS: Could not allocate ServerEntry\n");
    return L7_FAILURE;
  }

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_DNS)
  {
    rc = dnsClientNameLookup(hostAddr->host.hostName, &status, fqhostname, &ipaddr);

    if(rc == L7_FAILURE)
    {
      /* if this is a failure we do the DNS name resolution in server open */
      ipaddr = 0;
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): DNS resolution failed.. ",__FUNCTION__);
    }
    else
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): DNS Resolution success, host: %s - ip: %s\n",
                     hostAddr->host.hostName,osapiInet_ntoa(ipaddr));
    }
  }
  else
  {
    ipaddr = hostAddr->host.ipAddr;
  }

  (void)memset((void *)serverEntry, L7_NULL, RADIUS_SM_BUF_SIZE);

  serverEntry->status = RADIUS_SERVER_STATUS_DOWN;

  serverEntry->serverConfig.radiusServerConfigIpAddress = ipaddr;
  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_DNS)
  {
    osapiStrncpySafe(serverEntry->serverConfig.radiusServerDnsHostName.host.hostName,
                hostAddr->host.hostName,sizeof(hostAddr->host.hostName));
    serverEntry->serverConfig.radiusServerDnsHostName.hostAddrType =
                                            L7_IP_ADDRESS_TYPE_DNS;
    osapiStrncpySafe(serverEntry->radiusServerDnsHostName.host.hostName,
           hostAddr->host.hostName,sizeof(hostAddr->host.hostName));
    serverEntry->radiusServerDnsHostName.hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
  }
  else
  {
    serverEntry->radiusServerDnsHostName.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
    memset(serverEntry->serverConfig.radiusServerDnsHostName.host.hostName, '\0',
           sizeof(serverEntry->serverConfig.radiusServerDnsHostName.host.hostName));
    serverEntry->serverConfig.radiusServerDnsHostName.hostAddrType =
                                            L7_IP_ADDRESS_TYPE_IPV4;
  }
  serverEntry->serverConfig.radiusServerConfigServerType = serverType;

  serverEntry->serverConfig.radiusServerConfigServerEntryType = FD_RADIUS_SERVER_ENTRY_TYPE;

  radiusClientPtr = radiusClientGet();

  if ( L7_NULLPTR == radiusClientPtr )
  {
    RADIUS_ERROR_SEVERE( "RADIUS: Could not access Radius Client.\r\n" );
    return L7_FAILURE;
  }

  serverEntry->serverConfig.localConfigMask = 0; /* all parameters are global */

  /*
   * Use the global settings for these paramters:
   */
  serverEntry->serverConfig.maxNumRetrans        = radiusClientPtr->maxNumRetrans;
  serverEntry->serverConfig.timeOutDuration      = radiusClientPtr->timeOutDuration;
  serverEntry->serverConfig.radiusServerDeadTime = radiusClientPtr->radiusServerDeadTime;
  serverEntry->serverConfig.sourceIpAddress      = radiusClientPtr->sourceIpAddress;
  if ( strlen( radiusClientPtr->radiusServerSecret ) > 0 )
  {
    osapiStrncpySafe( serverEntry->serverConfig.radiusServerConfigSecret,
             radiusClientPtr->radiusServerSecret,
             sizeof(serverEntry->serverConfig.radiusServerConfigSecret) );
    serverEntry->serverConfig.radiusServerIsSecretConfigured = L7_TRUE;
  }
  else
  {
    serverEntry->serverConfig.radiusServerIsSecretConfigured = L7_FALSE;
  }

  /*
   * Use defaults for all others.
   */
  serverEntry->isServerMarkedDead = L7_FALSE;
  serverEntry->serverDeathTime = 0;

  if (serverType == RADIUS_SERVER_TYPE_AUTH)
  {
    serverEntry->serverConfig.radiusServerConfigUdpPort = FD_RADIUS_AUTH_PORT;
    serverEntry->radiusServerConfigUdpPort = FD_RADIUS_AUTH_PORT;
    serverEntry->radiusServerStats_t.authStats.radiusAuthServerIndex = index+1;
  }
  else if (serverType == RADIUS_SERVER_TYPE_ACCT)
  {
    serverEntry->serverConfig.radiusServerConfigUdpPort = FD_RADIUS_ACCT_PORT;
    serverEntry->radiusServerConfigUdpPort = FD_RADIUS_ACCT_PORT;
    serverEntry->radiusServerStats_t.acctStats.radiusAcctServerIndex = index+1;
  }

  serverEntry->radiusServerConfigIpAddress = ipaddr;
  if(ipaddr != 0)
  {
    serverEntry->serverConfig.radiusServerConfigRowStatus = RADIUS_SERVER_ACTIVE;
  }
  else
  {
    /* Domain Name resolution has failed */
    serverEntry->serverConfig.radiusServerConfigRowStatus = RADIUS_SERVER_NOTREADY;
  }
  serverEntry->serverConfig.radiusServerConfigPriority = FD_RADIUS_PRIORITY;
  serverEntry->serverConfig.usageType = FD_RADIUS_USAGE;
  serverEntry->serverConfig.oldUsageType = FD_RADIUS_USAGE;
  serverEntry->serverConfig.incMsgAuthMode = FD_RADIUS_INC_MSG_AUTH_MODE;

  /*
   ** Add the entry to the top of the list
   */
  serverEntry->nextEntry = radiusServerEntries;
  radiusServerEntries = serverEntry;

  if (serverType == RADIUS_SERVER_TYPE_AUTH)
  {
    serverEntry->serverNameIndex =0;

    if(radiusAuthServerNameSet(serverEntry, serverName) != L7_SUCCESS )
    {
       LOG_MSG("RADIUS: radiusEventServerHostNameAdd(): Failed to set the name \
                     to the server\n");
       return L7_FAILURE;
    }
    serverEntry->serverConfig.serverNameIndex = serverEntry->serverNameIndex;
  }
  else
  {
    if(radiusAcctServerNameAdd(serverEntry, serverName) != L7_SUCCESS )
    {
       LOG_MSG("RADIUS: radiusEventServerHostNameAdd(): Failed to set the name \
                     to the server\n");
       return L7_FAILURE;
    }

    /* Reselect only if the ipaddr is non zero */
    if( radiusConfigChangePropagate(RADIUS_CONFIG_EVENT_TYPE_MODIFY_ACCT,
                     serverEntry->serverNameIndex) != L7_SUCCESS)
    {
      LOG_MSG("RADIUS: %s() Failed to select the new Acct. current server\n",__FUNCTION__);
      return L7_FAILURE;
    }
  }
  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Remove a RADIUS server entry with the specified hostname
 *          and type from the server list.
 *
 * @param serverIPAddr @b((input)) HostName of the server to be removed
 * @param type @b((input)) Server type RADIUS_SERVER_TYPE_AUTH or
 *                                     RADIUS_SERVER_TYPE_ACCT
 *
 * @returns L7_SUCCESS - if an entry is found and deleted.
 * @returns L7_FAILURE - if an entry is not found
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventServerHostNameRemove(dnsHost_t *hostAddr,
    L7_uint32 type)
{
  radiusServerEntry_t *serverEntry;

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_DNS)
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, type);
  }
  else
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, type);
  }

  if (serverEntry == L7_NULL)
  {
    /* No entry with serverIPAddr was found */
    return L7_FAILURE;
  }

  serverEntry->serverConfig.radiusServerConfigRowStatus = RADIUS_SERVER_DESTROY;

  if( radiusServerEntryDelete() != L7_SUCCESS)
  {
    LOG_MSG("RADIUS: radiusEventServerHostNameRemove(): Failed to delete \
                     the server entry \n");
    return L7_FAILURE;
  }


  /* Recompute the index for all the servers after the server was deleted.
   */
  radiusUpdateIndexNumbers(type);

  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Remove all RADIUS auth and accounting servers
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventAllServerRemove(void)
{
  radiusServerEntry_t *serverEntry;

  while((serverEntry = radiusServerEntries) != L7_NULL)
  {
    serverEntry->serverConfig.radiusServerConfigRowStatus = RADIUS_SERVER_DESTROY;
    if( radiusServerEntryDelete() != L7_SUCCESS)
    {
      LOG_MSG("RADIUS: radiusEventAllServerRemove(): Failed to delete the server \
                     entry \n");
      return L7_FAILURE;
    }

  }
  /* Closes all sockets bound on routing/loopback network interfaces. */
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  radiusNwInterfaceSocketDeleteAll();
#endif

  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose Set the auth server port number
 *
 * @param serverIPAddr @b((input)) IP Address of the authorization server
 * @param portNum @b((input)) new value of the port number
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventServerPortNumSet(dnsHost_t *hostAddr,
    L7_uint32 portNum)
{
  radiusServerEntry_t *serverEntry;

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
  }

  if (serverEntry == L7_NULL)
  {
    /* No entry with serverIPAddr was found */
    return L7_FAILURE;
  }

  serverEntry->radiusServerConfigUdpPort = (L7_ushort16)portNum;
  serverEntry->serverConfig.radiusServerConfigUdpPort = (L7_ushort16)portNum;

  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the auth server priority
 *
 * @param serverIPAddr @b((input)) IP Address of the authorization server
 * @param priority     @b((input)) new value of the server priority
 *                                 (lower values mean higher priority)
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventServerPrioritySet(dnsHost_t *hostAddr,
                                     L7_uint32  priority)
{
  radiusServerEntry_t *serverEntry;

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
  }

  if (serverEntry == L7_NULL)
  {
    /* No entry with serverIPAddr was found */
    return L7_FAILURE;
  }

  if (serverEntry->serverConfig.radiusServerConfigPriority != priority)
  {
    serverEntry->serverConfig.radiusServerConfigPriority = priority;
    (void)radiusConfigChangePropagate(RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH, serverEntry->serverNameIndex);

    radiusCfg.hdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the auth server usage type (dot1x, login, all)
 *
 * @param hostAddr  @b((input)) server IP address
 * @param usageType @b((input)) the new value of server usage type
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventServerUsageTypeSet(dnsHost_t                     *hostAddr,
                                      L7_RADIUS_SERVER_USAGE_TYPE_t  usageType)
{
  radiusServerEntry_t *serverEntry;

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
  }

  if (serverEntry == L7_NULL)
  {
    /* No entry with serverIPAddr was found */
    return L7_FAILURE;
  }

  serverEntry->serverConfig.usageType = usageType;

  if (serverEntry->serverConfig.usageType != serverEntry->serverConfig.oldUsageType)
  {
    (void)radiusConfigChangePropagate(RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH, serverEntry->serverNameIndex);
    serverEntry->serverConfig.oldUsageType = usageType;

    radiusCfg.hdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the shared secret for the auth server with specified ip address
 *
 * @param hostAddr    @b((input)) address of the server (individual/all)
 * @param secret      @b((input)) new value of the shared secret
 * @param paramStatus @b((input)) should be set for a specific server or globally
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventServerSharedSecretSet(dnsHost_t                           *hostAddr,
                                         L7_char8                            *secret,
                                         L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusServerEntry_t *serverEntry;

  if (strlen(secret) > L7_RADIUS_MAX_SECRET)
  {
      return L7_FAILURE;
  }

  if((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS))
  { /* global parameter */
    osapiStrncpySafe(radiusClient.radiusServerSecret, secret, sizeof(radiusClient.radiusServerSecret));
    /* Apply it to all servers that don't have local values set */
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
          serverEntry = serverEntry->nextEntry)
    {
      if ((serverEntry->serverConfig.localConfigMask & L7_RADIUS_SERVER_CONFIG_SECRET) == 0)
      {
        osapiStrncpySafe(serverEntry->serverConfig.radiusServerConfigSecret, secret, sizeof(serverEntry->serverConfig.radiusServerConfigSecret));
        serverEntry->serverConfig.radiusServerIsSecretConfigured = L7_TRUE;
      }
    }
  }
  else
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }
    if(L7_NULLPTR == serverEntry)
    {
      LOG_MSG("RADIUS: radiusEventServerSharedSecretSet(): server entry not found \n");
      return L7_FAILURE;
    }

    if (paramStatus == L7_RADIUSGLOBAL)
    {
      serverEntry->serverConfig.localConfigMask &= ~L7_RADIUS_SERVER_CONFIG_SECRET;
      osapiStrncpySafe(serverEntry->serverConfig.radiusServerConfigSecret,
                       radiusClient.radiusServerSecret,
                       sizeof(serverEntry->serverConfig.radiusServerConfigSecret));
      serverEntry->serverConfig.radiusServerIsSecretConfigured = L7_TRUE;
    }
    else
    {
      serverEntry->serverConfig.localConfigMask |= L7_RADIUS_SERVER_CONFIG_SECRET;
      osapiStrncpySafe(serverEntry->serverConfig.radiusServerConfigSecret, secret, sizeof(serverEntry->serverConfig.radiusServerConfigSecret));
      serverEntry->serverConfig.radiusServerIsSecretConfigured = L7_TRUE;
    }
  }
  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the server entry type to primary or backup (secondary)
 *
 * @param serverIPAddr @b((input)) server ip address
 * @param entryType @b((input)) type of server to be changed to.
 *                               (primary/secondary).
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventServerEntryTypeSet(dnsHost_t *hostAddr,
    L7_uint32 entryType)
{
  radiusServerEntry_t *serverEntry, *prevPrimaryServerEntry;
  dnsHost_t prevPrimaryServer;
  L7_RC_t rc = L7_FAILURE;

  if( L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY != entryType &&
        L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY != entryType)
  {
    LOG_MSG("RADIUS(TypeSet): %s : Invalid inputs\n",__FUNCTION__);
    return L7_FAILURE;
  }
  if( L7_IP_ADDRESS_TYPE_IPV4 == hostAddr->hostAddrType )
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr,
                     RADIUS_SERVER_TYPE_AUTH);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName,
                     RADIUS_SERVER_TYPE_AUTH);
  }

  if ( L7_NULLPTR == serverEntry )
  {
    /* No entry with serverIPAddr was found */
    LOG_MSG("RADIUS(TypeSet): %s : Specified Server Not Found\n",__FUNCTION__);
    return L7_FAILURE;
  }

  if(serverEntry->serverConfig.radiusServerConfigServerEntryType == entryType)
  {
    return L7_SUCCESS;
  }

  if(L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY == entryType)
  {
    /* rc = L7_FAILURE/L7_SUCCESS/L7_NOT_EXIST*/
    rc = radiusPrimaryServerAddressGet(serverEntry->serverNameIndex,
                     &prevPrimaryServer);
    if(L7_FAILURE == rc)
    {
      LOG_MSG("RADIUS: %s() : Failed to fetch Existing Primary SErver\n",
                     __FUNCTION__);
      return rc;
    }
    else if ( /*L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY == entryType &&*/
            L7_SUCCESS == rc)
    {
      /* Toggle the the entry type of the server that was configured as primary
       * to secondary.
       */
      if(prevPrimaryServer.hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
      {
        prevPrimaryServerEntry = radiusServerEntryGet(
                     prevPrimaryServer.host.ipAddr,RADIUS_SERVER_TYPE_AUTH);
      }
      else
      {
        prevPrimaryServerEntry = radiusServerHostEntryGet(
                     prevPrimaryServer.host.hostName,RADIUS_SERVER_TYPE_AUTH);
      }
      if(L7_NULL != prevPrimaryServerEntry)
      {
        prevPrimaryServerEntry->serverConfig.radiusServerConfigServerEntryType =
                     L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY;

        /* When another dns entry becomes 'primary' which is going to open
         *  socket, its state becomes ACTIVE.  And, if previous primary is
         *  also the dns entry which has not been resolved yet, its state
         *  should be inactive (..NOTREADY). The philosophy is a dns
         *  server's state is active only when it has dns address resolved.
         */
        if(L7_IP_ADDRESS_TYPE_DNS ==
                     prevPrimaryServerEntry->radiusServerDnsHostName.
                     hostAddrType &&
           ( 0 == prevPrimaryServerEntry->serverConfig.
                     radiusServerConfigIpAddress)
          )
        {
          prevPrimaryServerEntry->serverConfig.radiusServerConfigRowStatus =
                     RADIUS_SERVER_NOTREADY ;
        }
      }
      else
      {
        LOG_MSG("RADIUS: %s() : Failed to fetch Existing Primary SErver\n",
                     __FUNCTION__);
        return L7_FAILURE;
      }
    }
    serverEntry->serverConfig.radiusServerConfigRowStatus =
                     RADIUS_SERVER_ACTIVE;
  }
  else if(L7_IP_ADDRESS_TYPE_DNS ==
                     serverEntry->radiusServerDnsHostName.
                     hostAddrType &&
           ( 0 == serverEntry->serverConfig.
                     radiusServerConfigIpAddress)
         )
  {
    serverEntry->serverConfig.radiusServerConfigRowStatus =
                     RADIUS_SERVER_NOTREADY ;
  }

  serverEntry->serverConfig.radiusServerConfigServerEntryType = entryType;

  radiusConfigChangePropagate(RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH,
                     serverEntry->serverNameIndex);


  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Enable/Disable the inclusion of a Message-Authenticator attribute
 *          in each Access-Request packet to a specified RADIUS server.
 *
 * @param serverIPAddr @b((input)) server ip address
 * @param mode @b((input)) L7_ENABLE/L7_DISABLE to include the attribute or not
 *
 * @returns L7_SUCCESS
 * @retruns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusEventServerIncMsgAuthModeSet(dnsHost_t *hostAddr,
    L7_uint32 mode)
{
  radiusServerEntry_t *serverEntry;

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
  }

  if (serverEntry == L7_NULL)
  {
    /* No entry with serverIPAddr was found */
    return L7_FAILURE;
  }

  serverEntry->serverConfig.incMsgAuthMode = mode;

  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Sends the notify response back to the registered component.
 *
 * @param requestInfo @b((input)) orig. request information
 * @param packet @b((input)) data received from the server
 *
 * @returns void
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
void radiusResponseNotify(radiusRequestInfo_t *requestInfo,
    radiusPacket_t *packet)
{
  L7_uint32 attributesLen = L7_NULL;
  L7_uchar8 *attributes = L7_NULL;

  if ((packet != (radiusPacket_t *)L7_NULL) &&
      (osapiNtohs(packet->header.msgLength) > (L7_ushort16)sizeof(radiusHeader_t)))
  {
    attributesLen = osapiNtohs(packet->header.msgLength) - sizeof(radiusHeader_t);
    attributes = (L7_uchar8 *)packet + sizeof(radiusHeader_t);
  }

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Notifying registered component, ID: %d status: %d correlator: %d",
                     __FUNCTION__,requestInfo->componentID,requestInfo->status,
                     requestInfo->correlator);

  /* Notify the RADIUS services user of the result of their request */
  if (radiusNotifyList[requestInfo->componentID].registrar_ID)
  {
    (void)(*radiusNotifyList[requestInfo->componentID].notify_radius_resp)(requestInfo->status,
                                                                           requestInfo->correlator,
                                                                           (L7_uchar8 *)attributes,
                                                                           attributesLen);
  }

  /* Free up the response buffer if one was passed in */
  if (packet != (radiusPacket_t *)L7_NULL)
    bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);

  return;
}

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
                     L7_uint32 serverNameIndex)
{
  radiusServerEntry_t *currentEntry = L7_NULL, *serverEntry= L7_NULL,
  *selectedEntry = L7_NULL;

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): entered into the function..",__FUNCTION__);

  if (serverNameIndex < L7_RADIUS_AUTH_SERVER_NAME_INDEX_MIN ||
      serverNameIndex  > L7_RADIUS_MAX_AUTH_SERVERS )
  {
    LOG_MSG("RADIUS: radiusServerSelect(): Inavalid servernameIndex: %d",
            serverNameIndex);
    return L7_NULL;
  }

  /*
   ** Is there any available entry among the PRIMARY servers ?
   */

  /* Selection of New server should happen only in the group of servers
     whose name is indexed by serverNameIndex */
  for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
      serverEntry = serverEntry->nextEntry)
  {
    if (serverEntry->serverConfig.radiusServerConfigServerType == serverType &&
        serverEntry->serverConfig.radiusServerConfigServerEntryType ==
        L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY &&
        ((serverEntry->serverConfig.usageType == usageType) || (serverEntry->serverConfig.usageType == L7_RADIUS_SERVER_USAGE_ALL)) &&
        serverEntry->serverConfig.radiusServerConfigRowStatus ==
        RADIUS_SERVER_ACTIVE &&
        serverEntry->serverNameIndex == serverNameIndex &&
        (serverEntry->isServerMarkedDead == L7_FALSE)
       )
    {
      if (selectedEntry == L7_NULL ||
          /* Lowest value is highest priority */
          (serverEntry->serverConfig.radiusServerConfigPriority <
           selectedEntry->serverConfig.radiusServerConfigPriority)
         )
      {
        selectedEntry = serverEntry;
      }
    }
  }
  if(selectedEntry)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): selected entry with type: primary, ConfigRowStat: Active.",
                     __FUNCTION__);
  }


  /*
   ** Is there any available entry among the BACKUP servers ?
   */
  /* Selection of New server should happen only in the group of servers
     whose name is indexed by serverNameIndex */

  if (!selectedEntry)
  {
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
        serverEntry = serverEntry->nextEntry)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType ==
          serverType &&
          serverEntry->serverConfig.radiusServerConfigServerEntryType ==
          L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY &&
          ((serverEntry->serverConfig.usageType == usageType) || (serverEntry->serverConfig.usageType == L7_RADIUS_SERVER_USAGE_ALL)) &&
          serverEntry->serverConfig.radiusServerConfigRowStatus ==
          RADIUS_SERVER_ACTIVE &&
          serverEntry->serverNameIndex == serverNameIndex &&
          (serverEntry->isServerMarkedDead == L7_FALSE)
         )
      {
        if (selectedEntry == L7_NULL ||
            /* Lowest value is highest priority */
            (serverEntry->serverConfig.radiusServerConfigPriority <
             selectedEntry->serverConfig.radiusServerConfigPriority))
        {
          selectedEntry = serverEntry;
        }
      }
    }
    if(selectedEntry)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): selected entry with type: Secondary, ConfigRowStat: Active.",
                     __FUNCTION__);
    }

  }

  /*
   ** Determine the current entry, so that can't be returned, in the following
   ** cases, to make sure the server which just went down, can't be returned.
   */
  if (serverType == RADIUS_SERVER_TYPE_AUTH)
  {
    /* Note: current server might not be there during first server entry,
       for which, it doesn't return failure.*/
    if ( radiusServerAuthCurrentEntryGet(serverNameIndex, usageType, &currentEntry) == L7_FAILURE )
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Failed to get current Auth. server entry.",
                     __FUNCTION__);
    }
  }
  else
  {
    if( radiusServerAcctCurrentEntryGet(serverNameIndex,&currentEntry) ==
                     L7_FAILURE )
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Failed to get current Acct. server entry.",
                     __FUNCTION__);
    }
  }

  /*
   ** Is there any available entry among the PRIMARY servers, which went down.
   ** It should be tried again, to see it has come back up.
   */
  /* Selection of New server should happen only in the group of servers
     whose name is indexed by serverNameIndex */

  if (!selectedEntry)
  {
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
        serverEntry = serverEntry->nextEntry)
    {
      if (serverEntry != currentEntry &&
          serverEntry->serverConfig.radiusServerConfigServerType == serverType &&
          serverEntry->serverConfig.radiusServerConfigServerEntryType ==
          L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY &&
          ((serverEntry->serverConfig.usageType == usageType) || (serverEntry->serverConfig.usageType == L7_RADIUS_SERVER_USAGE_ALL)) &&
          serverEntry->serverConfig.radiusServerConfigRowStatus == RADIUS_SERVER_NOTREADY &&
          serverEntry->serverNameIndex == serverNameIndex
         )
      {
        if (selectedEntry == L7_NULL ||
            /* Lowest value is highest priority */
            (serverEntry->serverConfig.radiusServerConfigPriority <
             selectedEntry->serverConfig.radiusServerConfigPriority))
        {
          selectedEntry = serverEntry;
        }

      }
    }
    if (selectedEntry)
    {
      selectedEntry->serverConfig.radiusServerConfigRowStatus =
                     RADIUS_SERVER_ACTIVE;

      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): selected entry with type: Primary, ConfigRowStat: Not Ready.",
                     __FUNCTION__);
    }
  }

  /*
   ** Is there any available entry among the BACKUP servers, which went down.
   ** It should be tried again, to see it has come back up.
   */
  /* Selection of New server should happen only in the group of servers
     whose name is indexed by serverNameIndex */

  if (!selectedEntry)
  {
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
        serverEntry = serverEntry->nextEntry)
    {
      if (serverEntry != currentEntry &&
          serverEntry->serverConfig.radiusServerConfigServerType == serverType &&
          serverEntry->serverConfig.radiusServerConfigServerEntryType ==
          L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY &&
          ((serverEntry->serverConfig.usageType == usageType) || (serverEntry->serverConfig.usageType == L7_RADIUS_SERVER_USAGE_ALL)) &&
          serverEntry->serverConfig.radiusServerConfigRowStatus == RADIUS_SERVER_NOTREADY &&
          serverEntry->serverNameIndex == serverNameIndex)
      {
        if (selectedEntry == L7_NULL ||
            /* Lowest value is highest priority */
            (serverEntry->serverConfig.radiusServerConfigPriority <
             selectedEntry->serverConfig.radiusServerConfigPriority))
        {
          selectedEntry = serverEntry;
        }
      }
    }

    if (selectedEntry)
    {
      selectedEntry->serverConfig.radiusServerConfigRowStatus =
                     RADIUS_SERVER_ACTIVE;
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): selected entry with type: Secondary, ConfigRowStat: Not Ready.",
                     __FUNCTION__);
    }
  }

  if(selectedEntry)
  {
    if(selectedEntry->serverConfig.radiusServerDnsHostName.hostAddrType ==
                     L7_IP_ADDRESS_TYPE_DNS )
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): selected entry: %s.",
                     __FUNCTION__,selectedEntry->serverConfig.
                     radiusServerDnsHostName.host.hostName);
    }
    else
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): selected entry: %s.",
                     __FUNCTION__,osapiInet_ntoa(selectedEntry->serverConfig.
                     radiusServerConfigIpAddress));
    }
  }

  return selectedEntry;
}


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
L7_RC_t radiusServerEntryDelete(void)
{
  radiusServerEntry_t *serverEntry, *prevEntry = L7_NULL;
  L7_uint32 index=0;
  L7_RC_t rc = L7_FAILURE;

  /*
   ** Does the specified entry exist ?
   */
  for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
      serverEntry = serverEntry->nextEntry)
  {
    if (serverEntry->serverConfig.radiusServerConfigRowStatus ==
                     RADIUS_SERVER_DESTROY)
    {
      /* Detach the entry from group tagged by the serverName&
       * Trigger the selection of new current(active) server, if this has
       * been the current server in the group.
       */

      if (serverEntry->serverConfig.radiusServerConfigServerType ==
                     RADIUS_SERVER_TYPE_AUTH)
      {
          rc = radiusConfigChangePropagate(RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH, serverEntry->serverNameIndex);
          if( L7_SUCCESS != rc)
          {
            LOG_MSG("RADIUS: radiusServerEntryDelete(): Failed to delete server\n");
            break;
          }

          /* As radiusServerNameUpdateRemove() shall clear this index,
           * taking backup of index for triggering selection of new
           * current server in that group
           */
          index = serverEntry->serverNameIndex;

          rc= radiusServerNameUpdateRemove(serverEntry);
          if( L7_SUCCESS != rc)
          {
            LOG_MSG("RADIUS: %s(): Failed remove the server \
                         name from the server entry\n",__FUNCTION__);
            break;
          }
      }
      else
      {
        rc = radiusAcctServerNameRemove(serverEntry);
        if( L7_SUCCESS != rc)
        {
            LOG_MSG("RADIUS: radiusServerEntryDelete(): Failed remove the \
                     server name from the Account server entry\n");
            break;
        }
        radiusServerClose(serverEntry,index);
      }

      if (prevEntry == L7_NULL)
      {
        radiusServerEntries = serverEntry->nextEntry;
      }
      else
      {
        prevEntry->nextEntry = serverEntry->nextEntry;
      }
      bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)serverEntry);

      break;
    }

    prevEntry = serverEntry;
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Change the active server from one to another
 *
 * @param eventType @b((input)) type of configuration change
 * @param serverNameIndex@b((input)) Index to the array of server names.
 * @returns void
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
void radiusServerChange(radiusServerEntry_t *newEntry, radiusServerEntry_t *currentEntry)
{
  radiusRequest_t *pendingRequests = L7_NULL;

  if ((newEntry != L7_NULL) && (newEntry != currentEntry))
  {
    if (currentEntry != L7_NULL)
    {
      pendingRequests = currentEntry->pendingRequests;
      currentEntry->pendingRequests = L7_NULL;
      radiusServerClose(currentEntry, currentEntry->serverNameIndex);
    }

    if (radiusServerOpen(newEntry) == L7_SUCCESS)
    {
      newEntry->pendingRequests = pendingRequests;
    }
    else
    {
      if(currentEntry != NULL)
      {
        radiusPendingRequestsRelease(currentEntry, pendingRequests);
      }
    }
  }
  else if ((newEntry == L7_NULL) && (currentEntry != L7_NULL))
  {
    radiusServerClose(currentEntry, currentEntry->serverNameIndex);
  }
}

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
 *           This routine does no locking. Caller must hold radiusTaskSyncSema.
 * @end
 *
 *********************************************************************/
L7_RC_t radiusConfigChangePropagate(L7_uint32 eventType, L7_uint32 serverNameIndex)
{
  radiusServerEntry_t *newEntry = L7_NULL;
  radiusServerEntry_t *currentEntry = L7_NULL;
  radiusServerEntry_t *usageAllNewEntry = L7_NULL;
  radiusServerEntry_t *usageAllCurrentEntry = L7_NULL;
  radiusServerEntry_t *usageLoginNewEntry = L7_NULL;
  radiusServerEntry_t *usageLoginCurrentEntry = L7_NULL;
  radiusServerEntry_t *usageDot1xNewEntry = L7_NULL;
  radiusServerEntry_t *usageDot1xCurrentEntry = L7_NULL;

  if(serverNameIndex < L7_RADIUS_AUTH_SERVER_NAME_INDEX_MIN  ||
                     serverNameIndex  > L7_RADIUS_MAX_AUTH_SERVERS )
  {
    LOG_MSG("RADIUS: %s(): \
                     Invalid servernameIndex: %d", __FUNCTION__, serverNameIndex);
    return L7_NULL;
  }

  if (eventType == RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH)
  {
    /* Select a server for usageType ALL. */
    usageAllNewEntry = radiusServerSelect(RADIUS_SERVER_TYPE_AUTH, L7_RADIUS_SERVER_USAGE_ALL, serverNameIndex);
    if( radiusServerAuthCurrentEntryGet(serverNameIndex, L7_RADIUS_SERVER_USAGE_ALL, &usageAllCurrentEntry) == L7_FAILURE )
    {
      LOG_MSG("RADIUS: %s(): Failed to get current \
                      Auth. server from the indexed group", __FUNCTION__);
    }

    /* Select a server for usageType Login. */
    usageLoginNewEntry = radiusServerSelect(RADIUS_SERVER_TYPE_AUTH, L7_RADIUS_SERVER_USAGE_LOGIN, serverNameIndex);
    if( radiusServerAuthCurrentEntryGet(serverNameIndex, L7_RADIUS_SERVER_USAGE_LOGIN, &usageLoginCurrentEntry) == L7_FAILURE )
    {
      LOG_MSG("RADIUS: %s(): Failed to get current \
                      Auth. server from the indexed group", __FUNCTION__);
    }

    /* Select a server for usageType Dot1x. */
    usageDot1xNewEntry = radiusServerSelect(RADIUS_SERVER_TYPE_AUTH, L7_RADIUS_SERVER_USAGE_DOT1X, serverNameIndex);
    if( radiusServerAuthCurrentEntryGet(serverNameIndex, L7_RADIUS_SERVER_USAGE_DOT1X, &usageDot1xCurrentEntry) == L7_FAILURE )
    {
      LOG_MSG("RADIUS: %s(): Failed to get current \
                      Auth. server from the indexed group", __FUNCTION__);
    }

    /* Avoid opening/closing a server more than once. */
    if (usageAllCurrentEntry == usageLoginCurrentEntry)
    {
      usageLoginCurrentEntry = L7_NULL;
    }
    if (usageAllCurrentEntry == usageDot1xCurrentEntry)
    {
      usageDot1xCurrentEntry = L7_NULL;
    }
    if (usageAllNewEntry == usageLoginNewEntry)
    {
      usageLoginNewEntry = L7_NULL;
    }
    if (usageAllNewEntry == usageDot1xNewEntry)
    {
      usageDot1xNewEntry = L7_NULL;
    }

    /* Determine which servers need to be closed. A server should be closed if
       the usage type has changed. */
    if (usageAllCurrentEntry)
    {
      if (usageAllCurrentEntry->serverConfig.usageType != L7_RADIUS_SERVER_USAGE_ALL)
      {
        radiusServerChange(L7_NULL, usageAllCurrentEntry);
        usageAllCurrentEntry = L7_NULL;
      }
    }
    if (usageLoginCurrentEntry)
    {
      if (usageLoginCurrentEntry->serverConfig.usageType != L7_RADIUS_SERVER_USAGE_LOGIN)
      {
        radiusServerChange(L7_NULL, usageLoginCurrentEntry);
        usageLoginCurrentEntry = L7_NULL;
      }
    }
    if (usageDot1xCurrentEntry)
    {
      if (usageDot1xCurrentEntry->serverConfig.usageType != L7_RADIUS_SERVER_USAGE_DOT1X)
      {
        radiusServerChange(L7_NULL, usageDot1xCurrentEntry);
        usageDot1xCurrentEntry = L7_NULL;
      }
    }

    radiusServerChange(usageAllNewEntry, usageAllCurrentEntry);
    radiusServerChange(usageLoginNewEntry, usageLoginCurrentEntry);
    radiusServerChange(usageDot1xNewEntry, usageDot1xCurrentEntry);
  }
  else if (eventType == RADIUS_CONFIG_EVENT_TYPE_MODIFY_ACCT)
  {
    newEntry = radiusServerSelect(RADIUS_SERVER_TYPE_ACCT, L7_RADIUS_SERVER_USAGE_ALL, serverNameIndex);
    radiusServerChange(newEntry, currentEntry);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Process a RADIUS packet received from a server
 *
 * @param
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusResponsePacketProcess(L7_uint32 serverAddress, radiusPacket_t *packet)
{
  radiusRequest_t *request;
  L7_uint32 roundTripTime = L7_NULL;
  radiusServerEntry_t * serverEntry = L7_NULL;

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): entered into the function.. ",__FUNCTION__);
  /*
   ** Lookup the corresponding server for this response
   */
  if (packet->header.code == RADIUS_CODE_ACCOUNT_RESPONSE)
  {
    serverEntry = radiusServerEntryGet(serverAddress, RADIUS_SERVER_TYPE_ACCT);
  }
  else
  {
    serverEntry = radiusServerEntryGet(serverAddress, RADIUS_SERVER_TYPE_AUTH);
  }

  if (serverEntry == L7_NULL)
  {
    LOG_MSG("RADIUS: radiusResponsePacketProcess: Server not found \
                     with given IP address.\n");
    return L7_FAILURE;
  }

  /*
   ** Lookup the corresponding request for this response
   */
  for (request = serverEntry->pendingRequests; request != L7_NULL;
      request = request->nextRequest)
  {
    if (request->requestId == packet->header.id)
      break;
  }

  if (request == L7_NULL)
  {

    bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);

    if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
    {
      serverEntry->radiusServerStats_t.authStats.radiusAuthClientPacketsDropped++;
    }
    else
    {
      serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPacketsDropped++;
    }
    LOG_MSG("%s(): Request not found for the response! Dropped the packet with ID:%d.",
                     __FUNCTION__,packet->header.id);
    return L7_FAILURE;
  }

  roundTripTime = ((osapiTimeMillisecondsGet() - request->lastSentTime) / 10);

  /*
   ** Increment appropriate counter and process the packet.
   */
  switch (packet->header.code)
  {
    case RADIUS_CODE_ACCESS_ACCEPT:
      serverEntry->radiusServerStats_t.authStats.radiusAuthClientRoundTripTime = roundTripTime;
      serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessAccepts++;
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Received packet with type: Access-Accept with ID:%d",
                     __FUNCTION__,packet->header.id);
      radiusAccessAcceptProcess(serverEntry, packet, request);

      break;
    case RADIUS_CODE_ACCESS_REJECT:
      serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRejects++;
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Received packet with type: Access-Reject, ID; %d",
                     __FUNCTION__,packet->header.id);
      radiusAccessRejectProcess(serverEntry, packet, request);

      break;
    case RADIUS_CODE_ACCESS_CHALLENGE:
      serverEntry->radiusServerStats_t.authStats.radiusAuthClientRoundTripTime = roundTripTime;
      serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessChallenges++;
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Received packet with type: Access-Challenge, ID: %d",
                     __FUNCTION__,packet->header.id);
      radiusAccessChallengeProcess(serverEntry, packet, request);

      break;
    case RADIUS_CODE_ACCOUNT_RESPONSE:
      serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRoundTripTime = roundTripTime;
      serverEntry->radiusServerStats_t.acctStats.radiusAcctClientResponses++;
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Received packet with type: Account-Response, ID: %d",
                     __FUNCTION__,packet->header.id);
      radiusAccountResponseProcess(serverEntry, packet, request);

      break;
    default:
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
      {
        serverEntry->radiusServerStats_t.authStats.radiusAuthClientUnknownTypes++;
        RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Unknown Type to process, dropping packet received from AUTH Server with ID:%d",
                     __FUNCTION__,packet->header.id);
      }
      else
      {
        RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Unknown Type to process, dropping packet received from ACCT Server with ID:%d",
                     __FUNCTION__,packet->header.id);
        serverEntry->radiusServerStats_t.acctStats.radiusAcctClientUnknownTypes++;
      }

      bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);

      break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Close all the RADIUS client sockets
 *
 * @param
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusClientShutdown(void)
{
  L7_RC_t rc = L7_SUCCESS;
  radiusServerEntry_t *serverEntry, *nextEntry;

  for (serverEntry = radiusServerEntries; serverEntry != L7_NULL; serverEntry = nextEntry)
  {
    rc= radiusServerNameUpdateRemove(serverEntry);
    if( L7_SUCCESS != rc)
    {
       LOG_MSG("RADIUS: radiusClientShutdown(): Failed remove the server \
                         name from the server entry\n");
    }

    radiusServerClose(serverEntry,serverEntry->serverNameIndex);

    nextEntry = serverEntry->nextEntry;
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)serverEntry);
  }

  radiusDictionaryFree();

  return rc;
}

/*********************************************************************
 *
 * @purpose Determine if any of the requests have timed out and need to be
 *          retransmitted.
 *
 * @param
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments This routine does no locking. Caller must hold radiusTaskSyncSema.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusClientTimer(void)
{
  radiusServerEntry_t *serverEntry, *nextServerEntry, *tmpEntry,*currentEntry=L7_NULL;
  radiusRequest_t *request, *nextRequest, *pendRequest;
  dnsHost_t serverAddr;
  radiusServerEntry_t *primaryServerEntry;
  L7_uint32 index;
  L7_RADIUS_SERVER_USAGE_TYPE_t usageType;
  L7_BOOL resurrectAuthServers, resurrectAcctServers;

  /*
  ** Set the current time of this timer event
  */
  radiusGlobalData.currentTime = osapiTimeMillisecondsGet();
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  radiusNwInterfaceSocketUpdate();
#endif

  /* If all the servers are 'dead', resurrect them so that we can try them again. */
  resurrectAuthServers = L7_TRUE;
  resurrectAcctServers = L7_TRUE;
  for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
       serverEntry = serverEntry->nextEntry)
  {
    if ((serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH) && (serverEntry->isServerMarkedDead == L7_FALSE))
    {
      resurrectAuthServers = L7_FALSE;
    }
    if ((serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT) && (serverEntry->isServerMarkedDead == L7_FALSE))
    {
      resurrectAcctServers = L7_FALSE;
    }
  }
  for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
       serverEntry = serverEntry->nextEntry)
  {
    if ((serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH) && (resurrectAuthServers))
    {
      serverEntry->isServerMarkedDead = L7_FALSE;
    }
    if ((serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_ACCT) && (resurrectAcctServers))
    {
      serverEntry->isServerMarkedDead = L7_FALSE;
    }
  }

  /*
  ** Look up the requests which have timed out for all the auth & acct servers,
  ** whose status is UP (with which this client is connected).
  */
  for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
       serverEntry = serverEntry->nextEntry)
  {
    if (serverEntry->isServerMarkedDead == L7_TRUE)
    {
      /* Check if the deadtime for this server has elapsed. */
      if ((radiusGlobalData.currentTime - serverEntry->serverDeathTime) >
          serverEntry->serverConfig.radiusServerDeadTime)
      {
        serverEntry->isServerMarkedDead = L7_FALSE;
      }
      else
      {
        continue;
      }
    }

    if (serverEntry->status == RADIUS_SERVER_STATUS_DOWN)
      continue;

    /*
    ** Lookup the timed out authentication requests for this server
    */
    for (request = serverEntry->pendingRequests; request != L7_NULL;
         request = nextRequest)
    {
      nextRequest = request->nextRequest;

      /*
      ** Has a request timed out?
      */
      if ((radiusGlobalData.currentTime - request->lastSentTime) >
          (serverEntry->serverConfig.timeOutDuration)) /* regardless of global/local setting, this is the value in use */
      {
        /*
        ** A request has timed out, see if retransmissions are necessary
        */
        if (request->retries++ < serverEntry->serverConfig.maxNumRetrans) /* regardless of global/local setting, this is the value in use */
        {
          if (serverEntry->serverConfig.radiusServerConfigServerType
              == RADIUS_SERVER_TYPE_AUTH)
          {
            serverEntry->radiusServerStats_t.authStats.radiusAuthClientTimeouts++;
            serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRetransmissions++;
          }
          else
          {
            serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRetransmissions++;
            serverEntry->radiusServerStats_t.acctStats.radiusAcctClientTimeouts++;
          }

          /* Retransmit the packet */
          (void)radiusPacketSend(serverEntry, request);

          continue;
        }
        else if (request->tryAnotherServer > L7_NULL)
        {
          /*
          ** Try another server, mark this server as notReady and lookup another
          ** primary or backup server and connect to that entry.
          */
          serverEntry->serverConfig.radiusServerConfigRowStatus = RADIUS_SERVER_NOTREADY;
          serverEntry->isServerMarkedDead = L7_TRUE;
          serverEntry->serverDeathTime = radiusGlobalData.currentTime;
          if (request->requestInfo->componentID == L7_USER_MGR_COMPONENT_ID)
          {
            usageType = L7_RADIUS_SERVER_USAGE_LOGIN;
          }
          else if (request->requestInfo->componentID == L7_DOT1X_COMPONENT_ID)
          {
            usageType = L7_RADIUS_SERVER_USAGE_DOT1X;
          }
          else
          {
            usageType = L7_RADIUS_SERVER_USAGE_ALL;
          }
          nextServerEntry = radiusServerSelect(
            serverEntry->serverConfig.radiusServerConfigServerType,
            usageType,
            serverEntry->serverNameIndex);
          if (nextServerEntry)
          {
            /*
            ** All pending requests are considered to have timed out and will
            ** either be copied to another server or released.
            */
            for (pendRequest = serverEntry->pendingRequests; pendRequest != L7_NULL;
                 pendRequest = pendRequest->nextRequest)
            {
              if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
              {
                serverEntry->radiusServerStats_t.authStats.radiusAuthClientTimeouts++;
              }
              else
              {
                serverEntry->radiusServerStats_t.acctStats.radiusAcctClientTimeouts++;
              }
            }

            /*
            ** Move the pending requests so they can be retried on the
            ** next server entry and properly cleaned up if no response
            ** is received.
            */
            nextServerEntry->pendingRequests = serverEntry->pendingRequests;
            serverEntry->pendingRequests = L7_NULL;

            /*
            ** Pick up where the last server left off with the radius id.
            */
            nextServerEntry->requestId = serverEntry->requestId;

            /*
            ** These packets were built for another server so they may have a different
            ** secret. If so convert the requests to the correct secret before
            ** attempting to transmit them.
            */
            if (strcmp(serverEntry->serverConfig.radiusServerConfigSecret,
                       nextServerEntry->serverConfig.radiusServerConfigSecret) != L7_NULL)
            {
              radiusSecretConvert(nextServerEntry, nextServerEntry->pendingRequests);
            }

            /* Update the new and reset the old pending requests stat */
            if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
            {
              nextServerEntry->radiusServerStats_t.authStats.radiusAuthClientPendingRequests =
                serverEntry->radiusServerStats_t.authStats.radiusAuthClientPendingRequests;
              serverEntry->radiusServerStats_t.authStats.radiusAuthClientPendingRequests = L7_NULL;
            }
            else
            {
              nextServerEntry->radiusServerStats_t.acctStats.radiusAcctClientPendingRequests =
                serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPendingRequests;
              serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPendingRequests = L7_NULL;
            }

            radiusServerClose(serverEntry,serverEntry->serverNameIndex);
            serverEntry = nextServerEntry;

            if (radiusServerOpen(serverEntry) == L7_SUCCESS)
            {
              /* Reset the retry count & retry another server flags for all requests */
              for (pendRequest = serverEntry->pendingRequests; pendRequest != L7_NULL;
                   pendRequest = pendRequest->nextRequest)
              {
                /* Make sure this request should be retransmitted */
                if (pendRequest->tryAnotherServer != L7_NULL)
                {
                  pendRequest->retries = L7_NULL;
                  pendRequest->tryAnotherServer--;

                  /* Update the servers request stats */
                  if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
                    serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRequests++;
                  else
                    serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRequests++;

                  /* Retransmit the packet */
                  (void)radiusPacketSend(serverEntry, pendRequest);
                }
              }
            }
            else
            {
              /* Can't connect to new server, release all the pending requests */
              radiusPendingRequestsRelease(serverEntry, serverEntry->pendingRequests);
            }
          }
          else
          {
            /* No other server is configured, stick with this one */
            serverEntry->serverConfig.radiusServerConfigRowStatus = RADIUS_SERVER_ACTIVE;

            /* release all the pending requests */
            radiusPendingRequestsRelease(serverEntry, serverEntry->pendingRequests);
          }
          break;
        } /* end if tryAnotherServer true */

        /*
        ** Exceeded the maximum number of retries for one request
        ** and there is no other server to try.
        */
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
        index = request->requestInfo->serverNameIndex;
        /* Remove all resources associated with the request */
        radiusRequestDelink(serverEntry, request);

        /* Mark all Auth. servers to be tried again on next request */
        for (tmpEntry = radiusServerEntries; tmpEntry != L7_NULL;
             tmpEntry = tmpEntry->nextEntry)
        {
          if(tmpEntry->serverNameIndex == index &&
             tmpEntry->serverConfig.
             radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH
            )
            tmpEntry->serverConfig.
              radiusServerConfigRowStatus = RADIUS_SERVER_ACTIVE;
        }

      } /* end if request has timed out */

    } /* end for loop if there are pending requests */

    /*
    ** If the current server is a secondary and its pending request queue
    ** is now empty, check if enough time has elasped to allow a Primary
    ** server to be tried again on the next Authentication attempt.
    */
    if( serverEntry->serverConfig.
        radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
    {
      (void)radiusServerAuthCurrentEntryGet(serverEntry->serverNameIndex, serverEntry->serverConfig.usageType, &currentEntry);

      if ((serverEntry == currentEntry) &&
          (serverEntry->pendingRequests == L7_NULL) &&
          (serverEntry->serverConfig.radiusServerConfigServerEntryType ==
           L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY) &&
          ((radiusGlobalData.currentTime - radiusGlobalData.serverEntryChangeTime) >
           radiusClient.retryPrimaryTime))
      {
        if (radiusPrimaryServerAddressGet(serverEntry->serverNameIndex,
                                          &serverAddr) == L7_SUCCESS)
        {
          if(serverAddr.hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
          {
            primaryServerEntry = radiusServerEntryGet(serverAddr.host.ipAddr,
                                                      RADIUS_SERVER_TYPE_AUTH);
          }
          else
          {
            primaryServerEntry  = radiusServerHostEntryGet(
              serverAddr.host.hostName, RADIUS_SERVER_TYPE_AUTH);
          }
          if(L7_NULL != primaryServerEntry)
          {
            /* Ensure that it will become the current server entry */
            primaryServerEntry->serverConfig.radiusServerConfigRowStatus =
              RADIUS_SERVER_ACTIVE;
            radiusConfigChangePropagate(RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH,
                                        primaryServerEntry->serverNameIndex);
          }
        }
      }
    }/* if servertype is Auth? */

  } /* end for loop if there are servers */

  return L7_SUCCESS;
}

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
                     dnsHost_t *serverAddr)
{
  radiusServerEntry_t * serverEntry;
  L7_char8 sName[L7_RADIUS_SERVER_NAME_LENGTH+1];/*dummy*/

  if( L7_NULLPTR == serverAddr )
  {
    return L7_FAILURE;
  }

  memset(sName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
  if(radiusServerNameEntryGet(nameIndex,sName) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
      serverEntry = serverEntry->nextEntry)
  {
    if ( (serverEntry->serverConfig.radiusServerConfigServerEntryType ==
          L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY) &&
         (serverEntry->serverConfig.radiusServerConfigServerType ==
          RADIUS_SERVER_TYPE_AUTH) &&
         (serverEntry->serverNameIndex == nameIndex)
       )
    {
      if ( serverEntry->radiusServerDnsHostName.hostAddrType ==
                     L7_IP_ADDRESS_TYPE_DNS)
      {
        serverAddr->hostAddrType = L7_IP_ADDRESS_TYPE_DNS;
        strcpy(serverAddr->host.hostName, serverEntry->radiusServerDnsHostName.host.hostName);
      }
      else
      {
        serverAddr->hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
        serverAddr->host.ipAddr =  serverEntry->radiusServerConfigIpAddress;
      }
      return L7_SUCCESS;
    }
  }

  serverAddr = L7_NULL;
  return L7_NOT_EXIST;
}

/*********************************************************************
 *
 * @purpose Set the value of the RADIUS NAS-IP Attributes
 *
 * @param    mode    @b{(input)} Radius NAS-IP Mode.
 * @param    ipAddr  @b{(input)} Radius NAS-IP address.
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAttribute4SetHandle(L7_BOOL mode, L7_uint32 ipAddr)
{
  radiusClient_t *radiusClient;
  L7_RC_t        rc=L7_FAILURE;

  radiusClient = radiusClientGet();
  if (radiusClient != L7_NULLPTR)
  {
    radiusClient->nasIpMode = mode;
    if (ipAddr != L7_NULL)
    {
      radiusClient->nasIpAddress = ipAddr;
    }
    if (mode == L7_FALSE)
    {
      radiusClient->nasIpAddress = L7_NULL;
    }
    radiusCfg.hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  }
  return rc;
}

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
L7_RC_t radiusServerNameEntryIndexGet(L7_char8 *serverName, L7_uint32 *index )
{
  L7_uint32 ind;
  L7_RC_t rc = L7_NOT_EXIST;

  if( L7_NULLPTR == serverName || L7_NULLPTR == index)
  {
    return L7_FAILURE;
  }

  for(ind=0; ind<L7_RADIUS_MAX_AUTH_SERVERS; ind++)
  {
    /*Non-zero count indicates there are servers tagged by this name*/
    if( (osapiStrncmp(serverName,authServerNameArray[ind].serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH) == 0 ) &&
        (authServerNameArray[ind].serverCount >0 )
      )
    {
      *index = ind+1;  /*callers expects to receive the index in range 1-32.*/
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;

}

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
L7_RC_t radiusServerNameEntryGet(L7_uint32 index,L7_char8 *serverName)
{
  if( 0 == index || index > L7_RADIUS_MAX_AUTH_SERVERS ||
                     L7_NULLPTR == serverName ||
                     0 == authServerNameArray[index-1].serverCount  )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Invalid Parameters: \
                    index=%d serverName(ptr):%u serverCount:%d\n",__FUNCTION__,
                    index,serverName,authServerNameArray[index-1].serverCount);
    return L7_FAILURE;
  }

  osapiStrncpySafe(serverName,authServerNameArray[index-1].serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);
  return L7_SUCCESS;

}

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
L7_RC_t radiusAcctServerNameEntryIndexGet(L7_char8 *serverName, L7_uint32 *index )
{
  L7_uint32 ind;
  L7_RC_t rc = L7_SUCCESS;


  if( L7_NULLPTR == serverName || L7_NULLPTR == index)
  {
    return L7_FAILURE;
  }

  *index = 0;
  for(ind=0; ind<L7_RADIUS_MAX_ACCT_SERVERS; ind++)
  {
    /*Non-zero count indicates there are servers tagged by this name*/
    if( (osapiStrncmp(serverName,acctServerNameArray[ind].serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH) == 0 ) &&
        (1 == acctServerNameArray[ind].serverCount)
      )
    {
      *index = ind+1;  /*callers' expected index is 1-32.*/
      break;
    }
  }

  if(L7_RADIUS_MAX_ACCT_SERVERS == ind )
  {
    rc = L7_FAILURE;
  }

  return rc;
}

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
L7_RC_t radiusAcctServerNameEntryGet(L7_uint32 index,L7_char8 *serverName)
{
  if( 0 == index || index > L7_RADIUS_MAX_ACCT_SERVERS ||
                     L7_NULLPTR == serverName )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Invalid Parameters\n",__FUNCTION__);
    return L7_FAILURE;
  }

  if( acctServerNameArray[index-1].serverCount == 0)
  {
    LOG_MSG("RADIUS: radiusAcctServerNameEntryGet(): No RADIUS Acct. server \
                     configured with this name \n");
    return L7_FAILURE;
  }
  osapiStrncpySafe(serverName,acctServerNameArray[index-1].serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);
  return L7_SUCCESS;
}

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
L7_RC_t radiusServerAuthCurrentEntryGet(L7_uint32 index, L7_RADIUS_SERVER_USAGE_TYPE_t usageType, radiusServerEntry_t **serverEntry)
{
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1]; /*dummy*/

  /* Since current entry pointer of name array can be cleared in some
     situations, the queries to get() current entry may return NULL*/

  /* If some body uses this serverName means, there is a currentServer
     in the named group.*/
  if(radiusServerNameEntryGet(index,serverName) == L7_SUCCESS )
  {
    if (usageType == L7_RADIUS_SERVER_USAGE_LOGIN)
    {
      *serverEntry = authServerNameArray[index-1].currentEntry.auth.authLoginServer;
      if (*serverEntry == L7_NULL)
      {
        *serverEntry = authServerNameArray[index-1].currentEntry.auth.authServer;
      }
    }
    else if (usageType == L7_RADIUS_SERVER_USAGE_DOT1X)
    {
      *serverEntry = authServerNameArray[index-1].currentEntry.auth.authDot1xServer;
      if (*serverEntry == L7_NULL)
      {
        *serverEntry = authServerNameArray[index-1].currentEntry.auth.authServer;
      }
    }
    else if (usageType == L7_RADIUS_SERVER_USAGE_ALL)
    {
      *serverEntry = authServerNameArray[index-1].currentEntry.auth.authServer;
    }
    else
    {
      return L7_FAILURE;
    }
  }
  else
  {
    *serverEntry = L7_NULL;
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Indexed name entry not found!\n",__FUNCTION__);
  }
  return L7_SUCCESS;  /*Temp for first server entry fails.*/
}

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
L7_RC_t radiusServerAcctCurrentEntryGet(L7_uint32 index,
                     radiusServerEntry_t **serverEntry)
{
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1]; /*dummy*/

  /*Instantaneous value of acctServerNameArray[index-1].currentEntry.acctServer
    can also be NULL.*/

  /* Since current entry pointer of name array can be cleared in some
     situations, the queries to get() current entry may return NULL*/

  /*Verifying the validity of index*/
  if(radiusAcctServerNameEntryGet(index,serverName) == L7_SUCCESS )
  {
    *serverEntry = acctServerNameArray[index-1].currentEntry.acct.acctServer;
  }
  else
  {
    serverEntry = L7_NULL;
    RADIUS_DLOG(RD_LEVEL_INFO, "RADIUS: %s() Indexed name entry not found!",__FUNCTION__);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To update the reference to currentEntry in indexed Auth.
*          server name entry.
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
L7_RC_t radiusServerAuthCurrentEntrySet(L7_uint32 index, L7_RADIUS_SERVER_USAGE_TYPE_t usageType, radiusServerEntry_t *serverEntry)
{
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1]; /*dummy*/

 /* serverEntry can also be NULL since there are situations
    we may want to clear pointer to current server entry.*/

  /*is the indexed serverNameEntry configured?*/
  if(radiusServerNameEntryGet(index,serverName) == L7_SUCCESS )
  {
    if (usageType == L7_RADIUS_SERVER_USAGE_LOGIN)
    {
      authServerNameArray[index-1].currentEntry.auth.authLoginServer = serverEntry;
    }
    else if (usageType == L7_RADIUS_SERVER_USAGE_DOT1X)
    {
      authServerNameArray[index-1].currentEntry.auth.authDot1xServer = serverEntry;
    }
    else if (usageType == L7_RADIUS_SERVER_USAGE_ALL)
    {
      authServerNameArray[index-1].currentEntry.auth.authServer = serverEntry;
    }
    else
    {
      LOG_MSG("RADIUS: radiusServerAuthCurrentEntrySet(): Invalid usageType\n");
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  else
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s(): Failed to get the indexed serverName!",__FUNCTION__);
    return L7_FAILURE;
  }
}

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
L7_RC_t radiusServerAcctCurrentEntrySet(L7_uint32 index,
                     radiusServerEntry_t *serverEntry)
{
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1]; /*dummy*/

 /* serverEntry can also be NULL since there are situations
    we may want to clear pointer to current server entry.*/

  /*Validating the index of serverNameEntry */
  if(radiusAcctServerNameEntryGet(index,serverName) == L7_SUCCESS )
  {
    acctServerNameArray[index-1].currentEntry.acct.acctServer = serverEntry;
    return L7_SUCCESS;
  }
  else
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s(): Failed to get the indexed serverName!",__FUNCTION__);
    return L7_FAILURE;
  }
}

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
L7_RC_t radiusServerNameAddUpdate(radiusServerEntry_t *serverEntry, L7_char8 *serverName)
{
  L7_uint32 ind;
  L7_RC_t rc = L7_FAILURE;

  if( L7_NULLPTR == serverEntry )
  {
    return L7_FAILURE;
  }
  /*If this name is already configured to a server?*/
  rc = radiusServerNameEntryIndexGet(serverName,&ind);
  if(L7_FAILURE == rc)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to verify the serverName\n",__FUNCTION__);
    return L7_FAILURE;
  }
  if( rc == L7_NOT_EXIST)
  {
    /* finding the name entry which hasn't been configured*/
    for(ind=0;ind<L7_RADIUS_MAX_AUTH_SERVERS;ind++)
    {
      if( 0 == authServerNameArray[ind].serverCount )
      {
        break;
      }
    }
    if( ind == L7_RADIUS_MAX_AUTH_SERVERS )
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() All of the name entries\
                     are configured. There can't be more than %d",__FUNCTION__,
                  L7_RADIUS_MAX_AUTH_SERVERS);
      return L7_FAILURE;
    }

    memset(authServerNameArray[ind].serverName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
    osapiStrncpySafe(authServerNameArray[ind].serverName,serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);
    ind++;
  }
  /* 1 <= serverEntry->serverNameIndex <= 32 &
     actual index being used to access array[] is 0-31 */

  serverEntry->serverNameIndex = ind;
  ind--;

  authServerNameArray[ind].serverCount++;
  memcpy(&radiusClient.authServerNameArray[ind],
         &authServerNameArray[ind],sizeof(serverName_t));

  return L7_SUCCESS;
}


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
* @comments Caller has to take care to prevent the same name being
*           assigned to another server in order to h'v the unique
*           names assigned acct. servers.
*
* @end
*
*********************************************************************/
L7_RC_t radiusAcctServerNameAdd(radiusServerEntry_t *serverEntry, L7_char8 *serverName)
{
  L7_uint32 ind;

  if (L7_NULLPTR == serverEntry || L7_NULLPTR == serverName)
  {
    LOG_MSG("RADIUS: Invalid Parameters! \n");
    return L7_FAILURE;
  }

  /* finding the name entry which hasn't been configured*/
  for (ind = 0; ind < L7_RADIUS_MAX_ACCT_SERVERS; ind++)
  {
    if( 0 == acctServerNameArray[ind].serverCount)
    {
      break;
    }
  }
  if( ind == L7_RADIUS_MAX_ACCT_SERVERS )
  {
    LOG_MSG("RADIUS: radiusAcctServerNameAdd(): All of the name entries\
                     are configured. There can't be more than %d",
            L7_RADIUS_MAX_ACCT_SERVERS);
    return L7_FAILURE;
  }

  memset(acctServerNameArray[ind].serverName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
  osapiStrncpySafe(acctServerNameArray[ind].serverName,serverName,L7_RADIUS_SERVER_NAME_LENGTH+1);

  acctServerNameArray[ind].serverCount  = 1;
  acctServerNameArray[ind].currentEntry.acct.acctServer = serverEntry;
  /* 1 <= serverEntry->serverNameIndex <= 32 &
     actual index being used to access array[] is 0-31 */
  serverEntry->serverNameIndex = (ind+1);

  return L7_SUCCESS;
}

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
L7_RC_t radiusServerNameEntryCountGet(L7_uint32 index, L7_uint32 *count)
{
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];/*dummy*/

  /* Finding out the validity of serverEntry->index */
  if( radiusServerNameEntryGet(index, serverName) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  if( L7_NULL == count)
  {
    return L7_FAILURE;
  }
  *count = authServerNameArray[index-1].serverCount;
  return L7_SUCCESS;
}

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
L7_RC_t radiusServerNameUpdateRemove(radiusServerEntry_t *serverEntry)
{
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];/*dummy*/
  L7_uint32 index;

  if( L7_NULLPTR == serverEntry)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Invalid parameters\n",__FUNCTION__);
    return L7_FAILURE;
  }
  index = serverEntry->serverNameIndex;

  /* Finding out the validity of serverEntry->index */
  if( radiusServerNameEntryGet(index, serverName) != L7_SUCCESS )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to find the server name \
                     entry indexed by the server\n",__FUNCTION__);
    return L7_FAILURE;
  }
  authServerNameArray[index-1].serverCount--;
  if(0 == authServerNameArray[index-1].serverCount )
  {
    authServerNameArray[index-1].currentEntry.auth.authServer      = L7_NULL;
    authServerNameArray[index-1].currentEntry.auth.authDot1xServer = L7_NULL;
    authServerNameArray[index-1].currentEntry.auth.authLoginServer = L7_NULL;
    memset(authServerNameArray[index-1].serverName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
  }
  memcpy(&radiusClient.authServerNameArray[index-1],
         &authServerNameArray[index-1],sizeof(serverName_t));
  /* clearing the reference to serverName*/
  serverEntry->serverNameIndex = 0;

  return L7_SUCCESS;
}

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
L7_RC_t radiusAcctServerNameUpdateRemove(radiusServerEntry_t *serverEntry)
{
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];/*dummy*/

  /* Finding out the validity of serverEntry->index */
  if( radiusAcctServerNameEntryGet(serverEntry->serverNameIndex,
                     serverName) != L7_SUCCESS )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to find the server name \
                     entry indexed by the server\n",__FUNCTION__);
    return L7_FAILURE;
  }
  acctServerNameArray[(serverEntry->serverNameIndex)-1].serverCount = 0;
  acctServerNameArray[(serverEntry->serverNameIndex)-1].currentEntry.acct.acctServer = L7_NULL;
  memset(acctServerNameArray[(serverEntry->serverNameIndex)-1].serverName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);

  /* clearing the reference to serverName*/
  serverEntry->serverNameIndex = 0;

  return L7_SUCCESS;
}

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
L7_RC_t radiusAcctServerNameRemove(radiusServerEntry_t *serverEntry)
{
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];/*dummy*/

  /* Finding out the validity of serverEntry->index */
  if( radiusAcctServerNameEntryGet(serverEntry->serverNameIndex,
                     serverName) != L7_SUCCESS )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to find the server name entry indexed by the server\n",__FUNCTION__);
    return L7_FAILURE;
  }

  acctServerNameArray[(serverEntry->serverNameIndex)-1].serverCount = 0;
  memset(acctServerNameArray[(serverEntry->serverNameIndex)-1].serverName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);

  /* clearing the reference to serverName*/
  serverEntry->serverNameIndex = 0;
  acctServerNameArray[(serverEntry->serverNameIndex)-1].currentEntry.acct.acctServer = L7_NULLPTR;

  return L7_SUCCESS;
}

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
L7_RC_t radiusEventAuthServerNameSet(dnsHost_t *hostAddr, L7_char8 *serverName)
{
  radiusServerEntry_t *serverEntry = L7_NULL;

  if(L7_NULLPTR == serverName)
  {
    return L7_FAILURE;
  }

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
  }
  if(L7_NULLPTR == serverEntry)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() server entry not found \n",__FUNCTION__);
    return L7_FAILURE;
  }

  if( radiusAuthServerNameSet(serverEntry,serverName) != L7_SUCCESS )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() FAiled to set the name:%s \
                     to server, serverEntry->index:%d\n",__FUNCTION__,
                     serverName,serverEntry->serverNameIndex);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

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
L7_RC_t radiusEventAcctServerNameSet(dnsHost_t *hostAddr, L7_char8 *serverName)
{
  radiusServerEntry_t *serverEntry = L7_NULL;

  if(L7_NULLPTR == serverName)
  {
    LOG_MSG("RADIUS: Invalid parameters \n");
    return L7_FAILURE;
  }

  if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_ACCT);
  }
  else
  {
    serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_ACCT);
  }
  if(L7_NULLPTR == serverEntry)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() server entry not found \n",__FUNCTION__);
    return L7_FAILURE;
  }

  if( radiusAcctServerNameUpdateRemove(serverEntry) != L7_SUCCESS  ||
      radiusAcctServerNameAdd(serverEntry,serverName) != L7_SUCCESS )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() FAiled to set the name:%s \
      to server, serverEntry->index:%d\n",__FUNCTION__,serverName,
      serverEntry->serverNameIndex);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Set the value of deadtime for a server or globally.
*
* @param hostAddr    @b((input)) address of the server (individual/all)
* @param deadtime    @b((input)) deadtime to be set
* @param paramStatus @b((input)) should be set for a specific server or globally
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*******************************************************************************/
L7_RC_t radiusEventDeadtimeSet(dnsHost_t                           *hostAddr,
                               L7_uint32                            deadtime,
                               L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus)
{
  radiusServerEntry_t *serverEntry;

  if((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS))
  { /* global RADIUS parameter; ignore paramStatus */
    radiusClient.radiusServerDeadTime =
      deadtime * DEADTIME_EXTRA_GRANULARITY * RADIUS_TIMER_GRANULARITY;  /* minutes */
    /* Apply it to all servers that don't have local values set */
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
          serverEntry = serverEntry->nextEntry)
    {
      if ((serverEntry->serverConfig.localConfigMask & L7_RADIUS_SERVER_CONFIG_DEAD_TIME) == 0)
      {
        serverEntry->serverConfig.radiusServerDeadTime = radiusClient.radiusServerDeadTime;
      }
    }
  }
  else /* server specific setting*/
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }
    if(L7_NULLPTR == serverEntry)
    {
      LOG_MSG("RADIUS: radiusEventDeadtimeSet(): server entry not found \n");
      return L7_FAILURE;
    }

    if (paramStatus == L7_RADIUSGLOBAL)
    {
      /* Copy global value to server-specific value. Ignore deadtime arg. */
      serverEntry->serverConfig.localConfigMask &= ~L7_RADIUS_SERVER_CONFIG_DEAD_TIME;
      serverEntry->serverConfig.radiusServerDeadTime = radiusClient.radiusServerDeadTime;
    }
    else
    {
      serverEntry->serverConfig.localConfigMask |= L7_RADIUS_SERVER_CONFIG_DEAD_TIME;
      serverEntry->serverConfig.radiusServerDeadTime =
        deadtime * DEADTIME_EXTRA_GRANULARITY * RADIUS_TIMER_GRANULARITY; /* minutes */
    }
  }
  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Set the source IP address for a server or globally.
*
* @param hostAddr @b((input)) address of the server (individual/all)
* @param sourceIPAddr @b((input)) source IP address to be set
* @param paramStatus @b((input)) should be set for a specific server or globally
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*******************************************************************************/
L7_RC_t radiusEventSourceIPSet(dnsHost_t *hostAddr,
                               L7_uint32  sourceIPAddr,
                               L7_uint32  paramStatus)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 previousSrcIpAddr;
#ifdef L7_ROUTING_PACKAGE
  L7_uint32 intIfNum;
  L7_uint32 ifState = L7_DISABLE;
#endif

  if((hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4) && (hostAddr->host.ipAddr == ALL_RADIUS_SERVERS))
  { /* global RADIUS parameter; ignore paramStatus */
    radiusClient.sourceIpAddress = sourceIPAddr;

#ifdef L7_ROUTING_PACKAGE
    if (ipMapIpAddressToIntf(sourceIPAddr, &intIfNum) == L7_SUCCESS)
    {
      (void)ipMapRtrIntfOperModeGet(intIfNum, &ifState);
    }
#endif

    /* Apply it to all servers that don't have local values set */
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
          serverEntry = serverEntry->nextEntry)
    {
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
      {
        if ((serverEntry->serverConfig.localConfigMask & L7_RADIUS_SERVER_CONFIG_SOURCE_IP_ADDRESS) == 0)
        {
          previousSrcIpAddr = serverEntry->serverConfig.sourceIpAddress;
          serverEntry->serverConfig.sourceIpAddress = radiusClient.sourceIpAddress;
#ifdef L7_ROUTING_PACKAGE
          /* If the corresponding routing interface is up, and this server is up, we
             need to rebind the socket using the new source-ip. Also re-bind if
             changing the source-ip to zero (which means use the default src-ip). */
          if (serverEntry->status != RADIUS_SERVER_STATUS_DOWN)
          {
            if ((ifState == L7_ENABLE) ||
                ((previousSrcIpAddr != 0) && (sourceIPAddr == 0)))
            {
              radiusServerClose(serverEntry, serverEntry->serverNameIndex);
              (void)radiusServerOpen(serverEntry);
            }
          }
#endif
        }
      }
    }
  }
  else /* server specific setting*/
  {
    if(hostAddr->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(hostAddr->host.ipAddr, RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(hostAddr->host.hostName, RADIUS_SERVER_TYPE_AUTH);
    }
    if(L7_NULLPTR == serverEntry)
    {
      LOG_MSG("RADIUS: radiusEventSourceIPSet(): server entry not found \n");
      return L7_FAILURE;
    }

    previousSrcIpAddr = serverEntry->serverConfig.sourceIpAddress;
    if (paramStatus == L7_RADIUSGLOBAL)
    {
      /* Copy global value to server-specific value. Ignore sourceIPAddr arg. */
      serverEntry->serverConfig.localConfigMask &= ~L7_RADIUS_SERVER_CONFIG_SOURCE_IP_ADDRESS;
      serverEntry->serverConfig.sourceIpAddress = radiusClient.sourceIpAddress;
    }
    else
    {
      serverEntry->serverConfig.localConfigMask |= L7_RADIUS_SERVER_CONFIG_SOURCE_IP_ADDRESS;
      serverEntry->serverConfig.sourceIpAddress = sourceIPAddr;
    }
#ifdef L7_ROUTING_PACKAGE
    /* If the corresponding routing interface is up, and this server is up, we
       need to rebind the socket using the new source-ip. Also re-bind if
       changing the source-ip to zero (which means use the default src-ip). */
    if (ipMapIpAddressToIntf(sourceIPAddr, &intIfNum) == L7_SUCCESS)
    {
      (void)ipMapRtrIntfOperModeGet(intIfNum, &ifState);
    }
    if (serverEntry->status != RADIUS_SERVER_STATUS_DOWN)
    {
      if ((ifState == L7_ENABLE) ||
          ((previousSrcIpAddr != 0) && (sourceIPAddr == 0)))
      {
        radiusServerClose(serverEntry, serverEntry->serverNameIndex);
        (void)radiusServerOpen(serverEntry);
      }
    }
#endif
  }
  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose To set the name to the specified server.
 *
 * @param serverEntry @b((input)) Pointer to the server entry.
 * @param serverName @b((input)) server name.
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @comments Callers should ensure that serverEntry posseses index to
 *           ther serverName before invoking this function to remove
 *           the name from it.
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAuthServerNameSet(radiusServerEntry_t *serverEntry,
                     L7_char8 *serverName)
{
  L7_uint32 index=0;
  L7_char8 sName[L7_RADIUS_SERVER_NAME_LENGTH+1];

  if( L7_NULLPTR == serverEntry || L7_NULLPTR == serverName)
  {
    return L7_FAILURE;
  }

  memset(sName,0,sizeof(sName));
  /* This shall be true only if we try to configure another name to the
     server that has already been configured with a name.*/
  if(radiusServerNameEntryGet(serverEntry->serverNameIndex,sName)== L7_SUCCESS )
  {
    /* Verify the case of same name being set again..b4 detaching
       the server from that named group*/
    if(osapiStrncmp(sName,serverName,L7_RADIUS_SERVER_NAME_LENGTH)== 0)
    {
      LOG_MSG("RADIUS: radiusAuthServerNameSet(): Trying to set the same \
                     name to server\n");
      return L7_REQUEST_DENIED;
    }

    index = serverEntry->serverNameIndex; /*Remove() shall clear this index*/
    if( radiusServerNameUpdateRemove(serverEntry) != L7_SUCCESS )
    {
      LOG_MSG("RADIUS: radiusAuthServerNameSet(): Failed to detach the server from\
                     current group");
      return L7_FAILURE;
    }

    /* Trigger selection new current server for the old group. */
    if( radiusConfigChangePropagate(RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH,
                     index) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  if( radiusServerNameAddUpdate(serverEntry, serverName)!= L7_SUCCESS )
  {
    LOG_MSG("RADIUS: radiusAuthServerNameSet(): Failed to set the name to the server\n");
    return L7_FAILURE;
  }

  /* Trigger selection new current server for the new group*/
  if( radiusConfigChangePropagate(RADIUS_CONFIG_EVENT_TYPE_MODIFY_AUTH,
                     serverEntry->serverNameIndex) != L7_SUCCESS)
  {
    LOG_MSG("RADIUS: radiusAuthServerNameSet(): Failed to select the new current Auth. server\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

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
L7_RC_t radiusServerNameEntryNextGet(L7_uint32 index, L7_uint32 *nextIndex)
{
  L7_char8 sName[L7_RADIUS_SERVER_NAME_LENGTH+1];/*dummy*/
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  if(index >= L7_RADIUS_MAX_AUTH_SERVERS || L7_NULLPTR == nextIndex)
  {
    return L7_FAILURE;
  }

  if(index == 0)
  {
    index++;
  }
  else
  {
      /* Validating the index to name entry*/
      memset(sName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
      if(radiusServerNameEntryGet(index,sName) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }
  }

  for(i=index;i<(L7_RADIUS_MAX_AUTH_SERVERS);i++)
  {
    if(authServerNameArray[i].serverCount >0)
    {
      *nextIndex = i;
      rc = L7_SUCCESS;
      break;
    }
  }
  return rc;
}
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
L7_RC_t radiusAcctServerNameEntryIndexNextGet(L7_uint32 index, L7_uint32 *nextIndex)
{
  L7_char8 sName[L7_RADIUS_SERVER_NAME_LENGTH+1];/*dummy*/
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  if(index >= L7_RADIUS_MAX_ACCT_SERVERS || L7_NULLPTR == nextIndex)
  {
    return L7_FAILURE;
  }

  if(index == 0)
  {
    index++;
  }
  else
  {
      /* Validating the index to name entry*/
      memset(sName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
      if(radiusAcctServerNameEntryGet(index,sName) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }
  }

  for(i=index;i<L7_RADIUS_MAX_ACCT_SERVERS;i++)
  {
    /* Verifying the index of nameEntry being returned is valid.*/
    if( 1 == acctServerNameArray[i].serverCount)
    {
      *nextIndex = i;
      rc = L7_SUCCESS;
      break;
    }
  }
  return rc;
}

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
                                                L7_char8 *nextName,
                                                radiusServerEntry_t **serverEntry)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i, nextIndex = 0;
  L7_char8 tmpName[L7_RADIUS_SERVER_NAME_LENGTH+1];

  if (L7_NULLPTR == name || L7_NULLPTR == nextName || L7_NULLPTR == serverEntry)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Invalid inputs !",__FUNCTION__);
    return L7_FAILURE;
  }
  /* First orderly name should be returned upon receiving all zeroes in the name*/
  memset(tmpName, 0, sizeof(tmpName));
  osapiStrncpySafe(tmpName, name, L7_RADIUS_SERVER_NAME_LENGTH + 1);

  for (i = 0; i < L7_RADIUS_MAX_AUTH_SERVERS; i++)
  {
    if (authServerNameArray[i].serverCount > 0)
    {
      if (memcmp(authServerNameArray[i].serverName, tmpName, L7_RADIUS_SERVER_NAME_LENGTH) > 0)
      {
        memset(tmpName, 0, sizeof(tmpName));
        osapiStrncpySafe(tmpName, authServerNameArray[i].serverName, L7_RADIUS_SERVER_NAME_LENGTH + 1);
        nextIndex = i;
        break;
      }
    }
  }
  if (i == L7_RADIUS_MAX_AUTH_SERVERS)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < L7_RADIUS_MAX_AUTH_SERVERS; i++)
  {
    if (authServerNameArray[i].serverCount > 0)
    {
      if ((memcmp(authServerNameArray[i].serverName, name, L7_RADIUS_SERVER_NAME_LENGTH) > 0) &&
          (memcmp(tmpName, authServerNameArray[i].serverName, L7_RADIUS_SERVER_NAME_LENGTH) > 0))
      {
        memset(tmpName, 0, sizeof(tmpName));
        osapiStrncpySafe(tmpName, authServerNameArray[i].serverName, L7_RADIUS_SERVER_NAME_LENGTH + 1);
        nextIndex = i;
      }
    }
  }

  if (memcmp(tmpName, name, L7_RADIUS_SERVER_NAME_LENGTH) != 0)
  {
    osapiStrncpySafe(nextName, tmpName, L7_RADIUS_SERVER_NAME_LENGTH + 1);
    *serverEntry = authServerNameArray[nextIndex].currentEntry.auth.authServer;
    if (L7_NULL == *serverEntry)
    {
      *serverEntry = authServerNameArray[nextIndex].currentEntry.auth.authDot1xServer;
    }
    if (L7_NULL == *serverEntry)
    {
      *serverEntry = authServerNameArray[nextIndex].currentEntry.auth.authLoginServer;
    }
    rc = L7_SUCCESS;
  }
  else
  {
    RADIUS_DLOG(RD_LEVEL_INFO, "RADIUS: %s() Failed to get next Name\n", __FUNCTION__);
  }
  return rc;
}

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
L7_RC_t radiusAcctServerNameEntryOrderlyGetNext(L7_char8 *name, L7_char8 *nextName,
                     radiusServerEntry_t **serverEntry)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i,/*index,*/nextIndex=0;
  L7_char8 tmpName[L7_RADIUS_SERVER_NAME_LENGTH+1];

  if(L7_NULLPTR == name || L7_NULLPTR == nextName || L7_NULLPTR == serverEntry )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Invalid inputs \n",__FUNCTION__);
    return L7_FAILURE;
  }
  /* First orderly name shoudl be returned upon receiving
     all zeroes in the name*/
  memset(tmpName,0,sizeof(tmpName));

  osapiStrncpySafe(tmpName,name,L7_RADIUS_SERVER_NAME_LENGTH+1);

  for(i=0;i<L7_RADIUS_MAX_ACCT_SERVERS;i++)
  {
    if( acctServerNameArray[i].serverCount > 0)
    {
      if( memcmp(acctServerNameArray[i].serverName,tmpName,
                     L7_RADIUS_SERVER_NAME_LENGTH) > 0 )
      {
        memset(tmpName,0,sizeof(tmpName));
        osapiStrncpySafe(tmpName,acctServerNameArray[i].serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);
        nextIndex = i;
        break;
      }
    }
  }
  if(i == L7_RADIUS_MAX_ACCT_SERVERS)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s Could not get next name \n",
                     __FUNCTION__);
    return L7_FAILURE;
  }

  for(i=0;i<L7_RADIUS_MAX_ACCT_SERVERS;i++)
  {
    if( acctServerNameArray[i].serverCount > 0)
    {
      if( (memcmp(acctServerNameArray[i].serverName,name,
                     L7_RADIUS_SERVER_NAME_LENGTH) > 0 ) &&
          (memcmp(tmpName,acctServerNameArray[i].serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH) > 0 ) )
      {
        memset(tmpName,0,sizeof(tmpName));
        osapiStrncpySafe(tmpName,acctServerNameArray[i].serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);
        nextIndex = i;
      }
    }
  }

  if(memcmp(tmpName,name,L7_RADIUS_SERVER_NAME_LENGTH) != 0)
  {
    osapiStrncpySafe(nextName,tmpName,L7_RADIUS_SERVER_NAME_LENGTH+1);
    *serverEntry = acctServerNameArray[nextIndex].currentEntry.acct.acctServer;
    rc = L7_SUCCESS;
  }
  else
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to get next Name!",
                     __FUNCTION__);
  }

  return rc;
}


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
 * @comments Added only for debug purpose.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAuthServerNameEntryDetailedGet(L7_uint32 index,
                                             L7_char8 *serverName,
                                             L7_char8 *currentServer,
                                             L7_uint32 *count,
                                             void *currentEntry)
{
  radiusServerEntry_t *entry;
  radiusServerEntry_t *server;

  if (0 == index ||
      index > L7_RADIUS_MAX_AUTH_SERVERS ||
      L7_NULLPTR == serverName ||
      L7_NULLPTR == currentServer ||
      L7_NULLPTR == count)
  {
    return L7_FAILURE;
  }
  osapiStrncpySafe(serverName, authServerNameArray[index-1].serverName,
                   L7_RADIUS_SERVER_NAME_LENGTH + 1);
  *count = authServerNameArray[index - 1].serverCount;
  currentEntry = (void *)authServerNameArray[index-1].currentEntry.auth.authServer;

  if ((L7_NULL == (server = authServerNameArray[index - 1].currentEntry.auth.authServer)) &&
      (L7_NULL == (server = authServerNameArray[index - 1].currentEntry.auth.authDot1xServer)) &&
      (L7_NULL == (server = authServerNameArray[index - 1].currentEntry.auth.authLoginServer)))
  {
    memset(currentServer, 0, L7_RADIUS_SERVER_NAME_LENGTH + 1);
    osapiStrncpySafe(currentServer, "No Current Entry", L7_RADIUS_SERVER_NAME_LENGTH + 1);
  }
  else
  {
    for (entry = radiusServerEntries; entry != L7_NULL; entry = entry->nextEntry)
    {
      if (entry == server)
      {
        osapiStrncpySafe(currentServer,
                         osapiInet_ntoa(entry->radiusServerConfigIpAddress),
                         L7_RADIUS_SERVER_NAME_LENGTH+1);
        break;
      }
    }
    if (entry == L7_NULL)
    {
      osapiStrncpySafe(currentServer, "Entry Not Found in serverList", L7_RADIUS_SERVER_NAME_LENGTH + 1);
    }
  }
  return L7_SUCCESS;
}

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
 * @comments * Added for debug purpose.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t radiusAcctServerNameEntryDetailedGet(L7_uint32 index,
                     L7_char8 *serverName,L7_uint32 *count)
{
  if(0 == index|| index > L7_RADIUS_MAX_ACCT_SERVERS ||
                     L7_NULLPTR == serverName || L7_NULLPTR == count)
  {
    return L7_FAILURE;
  }
  osapiStrncpySafe(serverName, acctServerNameArray[index-1].serverName,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);
   *count = acctServerNameArray[index-1].serverCount;


  return L7_SUCCESS;
}

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
/*********************************************************************
 *
 * @purpose House keeping for sockets bound on ipaddresses specifc to
 *          routing/loopback network interfaces.
 *
 * @param  none.
 * @returns none
 *
 * @comments Closes sockets that are inactive for more than last 3 minutes.
 *
 * @end
 *
 *********************************************************************/
void radiusNwInterfaceSocketUpdate(void)
{
  L7_uint32 i;
  nwInterface_t *interface = radiusGlobalData.nwInterfaces;

  for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
  {
    if(interface[i].ipAddr > 0)
    {
      if ( (radiusGlobalData.currentTime - interface[i].socketLastUsed) >
           (radiusGlobalData.socketInactiveTime) )
      {
        (void)radiusNwInterfaceSocketClose(interface[i].ipAddr);
      }

    }
  }
}

/*********************************************************************
 *
 * @purpose Close all sockets bound on ipaddresses specifc to
 *          routing/loopback network interfaces.
 *
 * @param  none.
 * @returns none
 *
 * @comments Closes all sockets bound on routing/loopback network interfaces.
 *
 * @end
 *
 *********************************************************************/
static void radiusNwInterfaceSocketDeleteAll(void)
{
  L7_uint32 i;
  nwInterface_t *interface = radiusGlobalData.nwInterfaces;

  for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
  {
    if(interface[i].ipAddr > 0)
    {
      (void)radiusNwInterfaceSocketClose(interface[i].ipAddr);
    }
  }
}
#endif

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
L7_RC_t radiusEventRoutingIntfChangeProcess(radiusEvents_t event, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
#ifdef L7_ROUTING_PACKAGE
  L7_IP_ADDR_t rtrIntfIpAddr;
  L7_IP_MASK_t rtrIntfIpMask;
  radiusServerEntry_t *serverEntry;

  if (ipMapRtrIntfIpAddressGet(intIfNum, &rtrIntfIpAddr, &rtrIntfIpMask) == L7_SUCCESS)
  {
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULL;
         serverEntry = serverEntry->nextEntry)
    {
      if (serverEntry->status != RADIUS_SERVER_STATUS_DOWN)
      {
        if (serverEntry->serverConfig.sourceIpAddress == rtrIntfIpAddr)
        {
          radiusServerClose(serverEntry, serverEntry->serverNameIndex);
          (void)radiusServerOpen(serverEntry);
        }
      }
    }
  }
#endif

  return rc;
}
