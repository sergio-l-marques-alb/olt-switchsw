/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_control.c
*
* @purpose TACACS+ Client control file
*
* @component tacacs+
*
* @comments none
*
* @create 03/10/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#include <string.h>
#include <netinet/in.h>
#include "l7_common.h"
#include "log.h"
#include "tacacs_api.h"
#include "tacacs_cfg.h"
#include "tacacs_aaa.h"
#include "tacacs_authen.h"
#include "tacacs_control.h"
#include "tacacs_cnfgr.h"
#include "tacacs_txrx.h"
#include "osapi.h"
#include "osapi_support.h"
#include "dns_client_api.h"

extern void              *tacacsQueue;
extern void              *tacacsSemaphore;
extern tacacsCfg_t       *tacacsCfg;
extern tacacsOprData_t   *tacacsOprData;
extern tacacsCnfgrState_t tacacsCnfgrState;

L7_uint32 tacacsTaskId        = L7_ERROR;
L7_uint32 tacacsRxTaskId      = L7_ERROR;

/* use one timer that wakes up every second, maintain a
   a reference count and delete the timer when we have no pending sessions */
static osapiTimerDescr_t *tacacsTimer       = L7_NULLPTR;
static L7_uint32          tacacsTimerCount  = 0;  /* reference count for tacacsTimer */

#define TACACS_TASK         "tacacs_task"
#define TACACS_RX_TASK      "tacacs_rx_task"

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
static void tacacsTimerCallback()
{
  tacacsMsg_t msg;

  memset(&msg, 0, sizeof(tacacsMsg_t));
  if (!tacacsQueue)
  {
    LOG_MSG("tacacsTimerCallback(): tacacsQueue has not been created!\n");
  }

  msg.event = TACACS_EVENT_TIMEOUT;
  if (osapiMessageSend(tacacsQueue, 
                       &msg, 
                       sizeof(tacacsMsg_t), 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("TACACS+: timer event send failed.\n");
  }
  osapiTimerAdd((void *)tacacsTimerCallback, L7_NULL, L7_NULL, 
                TACACS_TIMER_INTERVAL, &tacacsTimer);
}

/*********************************************************************
*
* @purpose  Maintain one timer resource for the component
*
* @returns  L7_SUCCESS, 
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void tacacsTimerAdd(void)
{

  if (tacacsTimerCount == 0)
  {
    osapiTimerAdd((void *)tacacsTimerCallback, L7_NULL, L7_NULL, 
                  TACACS_TIMER_INTERVAL, &tacacsTimer);
  }
  tacacsTimerCount++;

  return;
}

/*********************************************************************
*
* @purpose  Maintain one timer resource for the component
*
* @returns  L7_SUCCESS, 
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void tacacsTimerDelete(void)
{

  tacacsTimerCount--;
  if (tacacsTimerCount == 0)
  {
    osapiTimerFree(tacacsTimer);
    tacacsTimer = L7_NULLPTR;
  }

  return;
}

/*********************************************************************
*
* @purpose Process timer events, handle connection timeouts.
*
* @param   none
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
static void tacacsTimeoutProcess(void)
{
  L7_uint32 i = 0;

  for (i = 0; i < L7_TACACS_MAX_AUTH_SESSIONS; i++)
  {
    tacacsAuthSession_t *session = &tacacsOprData->sessions[i];
    if ((session->sessionId != 0) &&
        (session->state == TACACS_AUTH_STATE_CONNECT_WAIT))
    {
      if (session->connectTimer != 0)
      {
        session->connectTimer--;
        if (session->connectTimer == 0)
        {
          LOG_MSG("TACACS+: connection timeout to server %s.", osapiInet_ntoa(session->server));
          tacacsTimerDelete();
          tacacsContactNextServer(session);
        }
      }
    }
  }

  return;
}

/*********************************************************************
*
* @purpose Initialize TACACS+ Client tasks and data
*
* @param   none 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t tacacsStartTasks()
{
  /* create TACACS application task */
  tacacsTaskId = (L7_uint32)osapiTaskCreate(TACACS_TASK,
                                            (void *)tacacsTask,
                                            L7_NULL,
                                            L7_NULL,
                                            L7_DEFAULT_STACK_SIZE,
                                            L7_DEFAULT_TASK_PRIORITY,
                                            L7_DEFAULT_TASK_SLICE);

  if ((tacacsTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit(L7_TACACS_TASK_SYNC, 
                            L7_WAIT_FOREVER) != L7_SUCCESS))
  {
    TACACS_ERROR_SEVERE("TACACS+: Unable to initialize tacacsTask\n");
    return L7_FAILURE;
  }

  /* create TACACS task to receive session packets */
  tacacsRxTaskId = (L7_uint32)osapiTaskCreate(TACACS_RX_TASK,
                                              (void *)tacacsRxTask,
                                              L7_NULL,
                                              L7_NULL,
                                              L7_DEFAULT_STACK_SIZE,
                                              L7_DEFAULT_TASK_PRIORITY,
                                              L7_DEFAULT_TASK_SLICE);

  if ((tacacsRxTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit(L7_TACACS_RX_TASK_SYNC, 
                            L7_WAIT_FOREVER) != L7_SUCCESS))
  {
    TACACS_ERROR_SEVERE("TACACS+: Unable to initialize tacacsRxTask\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose TACACS+ Client task which reads the messages from 
*          tacacs queue
*
* @param   none 
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void tacacsTask(void)
{
  tacacsMsg_t  msg;

  (void)osapiTaskInitDone(L7_TACACS_TASK_SYNC);

  /*
  ** Loop forever waiting for TACACS+ messages
  */
  while (L7_TRUE)
  {
    memset(&msg, 0, sizeof(tacacsMsg_t));
    if (osapiMessageReceive(tacacsQueue,
                            (void *)&msg,
                            (L7_uint32)sizeof(tacacsMsg_t),
                            L7_WAIT_FOREVER) == L7_SUCCESS)    
    {
      osapiSemaTake(tacacsSemaphore, L7_WAIT_FOREVER);
      switch (msg.event)
      {
        case TACACS_EVENT_CNFGR_INIT:
          tacacsHandleCnfgrCmd(&(msg.data.cmdData));
          break;
        case TACACS_EVENT_TIMEOUT:
          tacacsTimeoutProcess();
          break;
        case TACACS_EVENT_AUTH_REQUEST:
          tacacsAuthenticationStart(msg.data.session);
          break;
        case TACACS_EVENT_RESPONSE_RECEIVED:
          tacacsAuthResponseProcess(msg.data.packet);
          /* done with response packet */
          memset(msg.data.packet, 0, sizeof(tacacsRxPacket_t)); 
          break;
        case TACACS_EVENT_SOCKET_ERROR:
          /* socket error on connection, just move to next server */
          /* to add single-connection, would need more logic here to cleanup
             socket open to server and other sessions using same connection */
          tacacsContactNextServer(msg.data.session);
          break;
        case TACACS_EVENT_SERVER_CONNECTED:
          tacacsServerConnectedProcess(msg.data.connectMsg.ip,
                                       msg.data.connectMsg.port,
                                       msg.data.connectMsg.socket);
          break;
        case TACACS_EVENT_SERVER_CONNECT_FAILED:
          tacacsServerConnectFailedProcess(msg.data.connectMsg.ip,
                                           msg.data.connectMsg.port,
                                           msg.data.connectMsg.socket);
          break;
        case TACACS_EVENT_SERVER_REMOVE:
          /* check for sessions with open sockets to this server,
             or in connection wait state for this server */
          tacacsServerRemoveProcess(msg.data.ip);
          break;
        case TACACS_EVENT_HOSTNAME_RESOLVE:
          /* Resolve host name by contacting the DNS server */
          tacacsServerResolveHostName(msg.data.hostName);
          break;
        default:
          break;
      } /* end of switch  event */
      osapiSemaGive(tacacsSemaphore);
    }
  } /* while (L7_TRUE) */

  return;
}

/*********************************************************************
*
* @purpose This task attempts to establish a server connection
*
*
* @param   L7_uint32 argc    @b{(input)}  number of args, 3
* @param   void      *argv   @b{(input)}  server ip, port, socket
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This function must be spawned as its own task, hence the name,
*           the connect call is a blocking call, the connection timeout 
*           must be handled by client task, this task must be deleted on 
*           a timeout and socket closed, this function cannot hold the semaphore, 
*           so argv cannot contain pointers to configuration data.
*
* @end
*
*********************************************************************/
void tacacsConnectTask(L7_uint32 argc, void *argv)
{
  tacacsMsg_t       msg;
  L7_sockaddr_in_t  s_addr;

  memset(&msg, 0, sizeof(tacacsMsg_t));
  memset(&s_addr, 0, sizeof(L7_sockaddr_in_t));

  msg.event = TACACS_EVENT_SERVER_CONNECT_FAILED; 
  msg.data.connectMsg.ip = ((L7_uint32 *)argv)[0];
  msg.data.connectMsg.port = ((L7_uint32 *)argv)[1];
  msg.data.connectMsg.socket = ((L7_uint32 *)argv)[2];

  s_addr.sin_family = L7_AF_INET;
  s_addr.sin_addr.s_addr = osapiHtonl(msg.data.connectMsg.ip);
  s_addr.sin_port = osapiHtons(msg.data.connectMsg.port);

  if (osapiConnect(msg.data.connectMsg.socket, 
                   (L7_sockaddr_t *)&s_addr, 
                   sizeof(s_addr)) == L7_SUCCESS)
  {
    msg.event = TACACS_EVENT_SERVER_CONNECTED;
  }

  if (osapiMessageSend(tacacsQueue, 
                       &msg, 
                       sizeof(tacacsMsg_t), 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    /* connection will time out if message isn't received */
    LOG_MSG("TACACS+: Failed to send server connect message.\n");
  }
  return;
}

/*********************************************************************
*
* @purpose Processes messages received on TACACS connections.
*
* @param   none
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void tacacsRxTask(void)
{
  (void)osapiTaskInitDone(L7_TACACS_RX_TASK_SYNC);

  /* Loop forever checking for incoming TACACS packets */
  while (L7_TRUE)
  {
    /* This would be better if it was waiting on a condition
       variable instead of waking up every second to check
       if we are expecting any responses... */
    if (tacacsCnfgrState == TACACS_PHASE_EXECUTE)
    {
      if (tacacsAuthSessionsPoll() != L7_SUCCESS)
      {
        /* sleep, then check for responses again */
        osapiSleep(1);
      }
    } else
    {
      osapiSleep(1);
    }
  }
}

/*********************************************************************
*
* @purpose Processes DNS host name resolution
*
* @param   void      *hostname   @b{(input)}  server domain name
*
* @returns void
*
* @comments This function is called when from user interface a tacacs
*           server HostName is added
*
* @end
*
*********************************************************************/
void tacacsServerResolveHostName(L7_uchar8 *hostName)
{
  L7_uint32 serverIdx = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_int32 ipaddr = 0;
  /* fully qualified host name */
  L7_char8  fqhostname[L7_DNS_DOMAIN_NAME_SIZE_MAX]; 
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;

  if (tacacsServerIPHostNameIndexGet(L7_IP_ADDRESS_TYPE_DNS, hostName, 
                                     &serverIdx) != L7_SUCCESS)
  {
    return;
  }

  rc = dnsClientNameLookup(hostName,&status, fqhostname, &ipaddr);

  if(rc == L7_FAILURE)
  {
    return;
  }
  tacacsCfg->servers[serverIdx].ip = ipaddr;

  return;
}

