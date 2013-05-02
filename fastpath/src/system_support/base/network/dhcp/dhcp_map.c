/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  dhcp_map.c
*
* @purpose   DHCP Mapping system infrastructure
*
* @component DHCP Client
*
* @comments  none
*
* @create    11-Jan-2010
*
* @author    Krishna Mohan CS
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l3_commdefs.h"
#include "osapi.h"
#include "commdefs.h"
#include "l3_mcast_commdefs.h"
#include "async_event_api.h"
#include "l7_socket.h"
#include "osapi_support.h"
#include "l7_dhcp.h"
#include "dhcp_map.h"
#include "dhcp_debug.h"

static void
dhcpClientUIEventProcess (dhcpClientQueueMsg_t *msg);

/*********************************************************************
* @purpose  Post a message to the DHCP Client Task
*
* @param    msg   @b{(input)} Message to be sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/
L7_RC_t
dhcpClientMessageSend (dhcpClientQueueMsg_t *msg)
{
  if (osapiMessageSend (dhcpClientCB.dhcpClientMsgQ.queuePtr, (L7_VOIDPTR) msg,
                        DHCP_CLIENT_QUEUE_MSG_SIZE, L7_NO_WAIT,
                        L7_MSG_PRIORITY_NORM)
                     != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Message Send Failed for Message-%d",
                 msg->msgId);
    if (osapiSemaTake (dhcpClientCB.dhcpClientMsgQ.queueCountersSem,
                       L7_WAIT_FOREVER)
                    == L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Queue Counters Sem Take Failed");
      dhcpClientCB.dhcpClientMsgQ.queueSendFailedCnt++;
      osapiSemaGive (dhcpClientCB.dhcpClientMsgQ.queueCountersSem);
    }
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Task to handle all DHCP Client messages
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*
*********************************************************************/
void
dhcpClientTask (void)
{
  dhcpClientQueueMsg_t msg;
  dhcpClientQueue_t *msgQueue = L7_NULLPTR;
  fd_set  readFds;
  L7_uint32 maxFDs = 0;
  L7_CNFGR_STATE_t systemState = L7_CNFGR_STATE_IDLE;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  msgQueue = &dhcpClientCB.dhcpClientMsgQ;
  msgQueue->queueRecvBuffer = &msg;

  do
  {
    /* DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP Client Task Processing ..."); */

    /* only run when system is in EXECUTE state */
    cnfgrApiSystemStateGet(&systemState);
    if (systemState != L7_CNFGR_STATE_E)
    {
      DHCPC_TRACE (DHCPC_DEBUG_TASK, "System is not yet in Execute State");
      osapiSleep (DHCP_CLIENT_SELECT_TIMEOUT_SEC);
      continue;
    }

    /* Wait for Events - UI and IPMAP */
    while (osapiMessageReceive (msgQueue->queuePtr, msgQueue->queueRecvBuffer,
                                msgQueue->queueMsgSize, L7_NO_WAIT)
                             == L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_TASK, "Message Queue message Received");
      if (osapiSemaTake (msgQueue->queueCountersSem, L7_WAIT_FOREVER)
                      == L7_SUCCESS)
      {
        msgQueue->queueRxSuccessCnt++;
        osapiSemaGive (msgQueue->queueCountersSem);
      }

      switch (msg.msgId)
      {
        case (DHCP_CLIENT_CNFGR_MSG):
          DHCPC_TRACE (DHCPC_DEBUG_TASK, "CNFGR_MSG message Received");
          /* TODO: This API right now is being invoked directly by Cnfgr thread.
           * Need to change this to event based mechanism.
           */
          dhcpClientApiCnfgrCommand (&msg.u.cnfgrCmdData);
          break;

        case (DHCP_CLIENT_UI_EVENT_MSG):
          DHCPC_TRACE (DHCPC_DEBUG_TASK, "UI_EVENT_MSG message Received");
          dhcpClientUIEventProcess (&msg);
          break;

        default:
          DHCPC_TRACE (DHCPC_DEBUG_TASK, "Invalid message Received");
          continue;
      }
    }

    /* Wait for DHCP Messages from the socket */
    DHCPC_TRACE (DHCPC_DEBUG_TASK, "Setting Socket FDs of all Active Interfaces");
    dhcpClientSocketFdsSet (&maxFDs, &readFds);

    /* Block for the event at the Socket as specified by the timeval */
    DHCPC_TRACE (DHCPC_DEBUG_TASK, "Blocking on Select for %d secs and %d msecs",
                 DHCP_CLIENT_SELECT_TIMEOUT_SEC, DHCP_CLIENT_SELECT_TIMEOUT_MSEC);
    if (osapiSelect (maxFDs + 1, &readFds, NULL, NULL, DHCP_CLIENT_SELECT_TIMEOUT_SEC,
                     DHCP_CLIENT_SELECT_TIMEOUT_MSEC)
                  <= 0)
    {
      DHCPC_TRACE (DHCPC_DEBUG_TASK, "Select has timed out, No messages received");
      continue;
    }
    else
    {
      DHCPC_TRACE (DHCPC_DEBUG_TASK, "Message has been received");
      dhcpClientMessageReceive (&readFds);
    }
  }
  while (1);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/*********************************************************************
* @purpose  This routine is called when a configuration event occurs
*           from UI
*
* @param    pData @b{(input)} Event Message
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
static void
dhcpClientUIEventProcess (dhcpClientQueueMsg_t *msg)
{
  DHCP_CLIENT_UI_EVENT_TYPE_t eventType;
  L7_MGMT_PORT_TYPE_t mgmtPortType;
  L7_uint32 intIfNum;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  intIfNum = msg->u.dhcpClientUIEventParms.intIfNum;
  eventType = msg->u.dhcpClientUIEventParms.event;
  mgmtPortType = msg->u.dhcpClientUIEventParms.mgmtPortType;

  switch (eventType)
  {
    case DHCP_CLIENT_ADDRESS_ACQUIRE:
    {
      DHCPC_TRACE (DHCPC_DEBUG_EVENTS, "DHCP_CLIENT_ADDRESS_ACQUIRE Event Received "
                   "for intIfNum - %d, mgmtPortType - %s", intIfNum,
                   dhcpDebugMgmtPortType[mgmtPortType]);

      if (dhcpClientDiscoverInitiate (intIfNum, mgmtPortType) != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_ADDRESS_ACQUIRE Event "
                     "Processing Failed");
        return;
      }
    }
    break;

    case DHCP_CLIENT_ADDRESS_RENEW:
    {
      DHCPC_TRACE (DHCPC_DEBUG_EVENTS, "DHCP_CLIENT_ADDRESS_RENEW Event Received "
                   "for intIfNum - %d, mgmtPortType - %s", intIfNum,
                   dhcpDebugMgmtPortType[mgmtPortType]);
      if (dhcpClientRenewInitiate (intIfNum, mgmtPortType) != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_ADDRESS_RENEW Event "
                     "Processing Failed");
        return;
      }
    }
    break;

    case DHCP_CLIENT_ADDRESS_RELEASE:
    {
      DHCPC_TRACE (DHCPC_DEBUG_EVENTS, "DHCP_CLIENT_ADDRESS_RELEASE Event Received "
                   "for intIfNum - %d, mgmtPortType - %s", intIfNum,
                   dhcpDebugMgmtPortType[mgmtPortType]);
      if (dhcpClientReleaseProcess (intIfNum, mgmtPortType) != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_ADDRESS_RELEASE Event "
                     "Processing Failed");
        return;
      }
    }
    break;

    case DHCP_CLIENT_ADDRESS_CONFIG:
    {
      L7_uint32 ipAddr = msg->u.dhcpClientUIEventParms.ipAddr;
      L7_uint32 netMask = msg->u.dhcpClientUIEventParms.netMask;
      L7_uint32 ipGateway = msg->u.dhcpClientUIEventParms.ipGateway;

      DHCPC_TRACE (DHCPC_DEBUG_EVENTS, "DHCP_CLIENT_ADDRESS_CONFIG Event Received "
                   "for mgmtPortType - %s", dhcpDebugMgmtPortType[mgmtPortType]);
      if (dhcpClientConfiguredIpAddressProcess (mgmtPortType, ipAddr, netMask,
                                                ipGateway)
                                             != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_ADDRESS_CONFIG Event "
                     "Processing Failed");
        return;
      }
    }
    break;

    case DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING:
    {
      L7_uchar8 *vendorClassString = msg->u.dhcpClientUIEventParms.vendorClassOptionString;

      DHCPC_TRACE (DHCPC_DEBUG_EVENTS, "DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING Event Received");
      if (dhcpVendorClassOptionStringUpdate (vendorClassString) != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_VENDOR_CLASS_OPTION_STRING Event "
                     "Processing Failed");
        return;
      }
    }
    break;

    case DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE:
    {
      L7_BOOL mode = msg->u.dhcpClientUIEventParms.vendorClassOptionMode;

      DHCPC_TRACE (DHCPC_DEBUG_EVENTS, "DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE Event Received");
      if (dhcpVendorClassOptionAdminModeUpdate (mode) != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP_CLIENT_VENDOR_CLASS_OPTION_MODE Event "
                     "Processing Failed");
        return;
      }
    }
    break;

    default:
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Invalid UI Event - %d received", eventType);
      break;
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/*********************************************************************
* @purpose  Set the Socket FDs for all the active DHCP Interfaces
*
* @param    maxFds   @b((output)) Total number of FDs to Set
* @param    readFds  @b((output)) Total number of FDs to Read
*
* @returns  void
*
* @notes    This API should only be invoked by the DHCP Client Task
*
* @end
*********************************************************************/
void
dhcpClientSocketFdsSet (L7_uint32 *maxFds,
                        fd_set *readFds)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;

  /* DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry"); */

  FD_ZERO (readFds);

  /* Set FDs for Router Interfaces */
  for (rtrIfNum = 0; rtrIfNum < DHCP_CLIENT_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if ((clientIntfInfo = dhcpClientCB.clientIntfInfo[rtrIfNum]) != L7_NULLPTR)
    {
      if ((clientIntfInfo->inUse == L7_TRUE) && (clientIntfInfo->sockFD != 0))
      {
        if (*maxFds < clientIntfInfo->sockFD)
        {
          *maxFds = clientIntfInfo->sockFD;
        }
        FD_SET (clientIntfInfo->sockFD, readFds);
        DHCPC_TRACE (DHCPC_DEBUG_TASK, "Read FD is Set for rtrIfNum-%d, "
                     "Router Interface", rtrIfNum);
      }
    }
  }

  /* Set FDs for Service Port */
  if ((clientIntfInfo = dhcpClientIntfInfoGet (rtrIfNum, L7_MGMT_SERVICEPORT))
                     != L7_NULLPTR)
  {
    if ((clientIntfInfo->inUse == L7_TRUE) && (clientIntfInfo->sockFD != 0))
    {
      if (*maxFds < clientIntfInfo->sockFD)
      {
        *maxFds = clientIntfInfo->sockFD;
      }
      FD_SET (clientIntfInfo->sockFD, readFds);
      DHCPC_TRACE (DHCPC_DEBUG_TASK, "Read FD is Set for Service Port");
    }
  }

  /* Set FDs for Network Port */
  if ((clientIntfInfo = dhcpClientIntfInfoGet (rtrIfNum, L7_MGMT_NETWORKPORT))
                     != L7_NULLPTR)
  {
    if ((clientIntfInfo->inUse == L7_TRUE) && (clientIntfInfo->sockFD != 0))
    {
      if (*maxFds < clientIntfInfo->sockFD)
      {
        *maxFds = clientIntfInfo->sockFD;
      }
      FD_SET (clientIntfInfo->sockFD, readFds);
      DHCPC_TRACE (DHCPC_DEBUG_TASK, "Read FD is Set for Network Port");
    }
  }

  /* DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit"); */
  return;
}

