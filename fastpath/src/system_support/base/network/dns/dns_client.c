/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dns_client.c
*
* @purpose DNS client main functions
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
#include "l7_common.h"
#include "log.h"
#include "dns_client.h"
#include "dns_client_api.h"
#include "dns_client_util.h"
#include "dns_client_txrx.h"
#include "dns_sid/dns_client_sid.h"


L7_int32               dnsTaskId = 0;
L7_int32               dnsRxTaskId = 0;
extern void           *dnsQueue;
extern void           *dnsSemaphore;
extern dnsCnfgrState_t dnsCnfgrState;

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
L7_RC_t dnsTaskStart()
{

  dnsTaskId = osapiTaskCreate("dnsTask", dnsTask, 0, 0,
                              dnsSidDefaultStackSize(),
                              dnsSidDefaultTaskPriority(),
                              dnsSidDefaultTaskSlice());
  if (dnsTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "Could not create task dnsTask\n");
    return L7_FAILURE;
  }
  if (osapiWaitForTaskInit(L7_DNS_CLIENT_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "Unable to initialize dnsTask()\n");
    return L7_FAILURE;
  }

  /* create task to receive DNS UDP packets */
  dnsRxTaskId = osapiTaskCreate("dnsRxTask", dnsRxTask, 0, 0,
                                dnsSidDefaultStackSize(),
                                dnsSidDefaultTaskPriority(),
                                dnsSidDefaultTaskSlice());
  if (osapiWaitForTaskInit(L7_DNS_CLIENT_RX_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "Unable to initialize dnsRxTask()\n");
    return L7_FAILURE;
  }


  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose main task to handle all DNS client management messages
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
void dnsTask()
{
  dnsMgmtMsg_t msg;
  L7_RC_t rc;

  osapiTaskInitDone(L7_DNS_CLIENT_TASK_SYNC);

  do
  {
    rc = osapiMessageReceive(dnsQueue, (void *)&msg,
                             DNS_CLIENT_MSG_SIZE, L7_WAIT_FOREVER);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
              "dnsTask(): osapiMessageReceive failed. %s:%d\n",
              __FILE__, __LINE__);
      continue;
    }
    switch (msg.msgId)
    {
      case (dnsMsgCnfgr):
        dnsCnfgrParse(&msg.u.CmdData);
        break;

      case (dnsMsgRequest):
        dnsRequestProcess(msg.u.request);
        break;

      case (dnsMsgResponse):
        dnsResponseProcess(msg.u.request);
        break;

      case (dnsMsgTimer):
        dnsTimerProcess();
        break;

      case (dnsMsgServerRemove):
        osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
        dnsNameServerActiveEntryRemove(&msg.u.address);
        osapiSemaGive(dnsSemaphore);
        break;

      case (dnsMsgReverseRequest):
        dnsReverseRequestProcess(msg.u.request);
        break;

      default:
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "dnsTask(): invalid message type:%d. %s:%d\n",
                msg.msgId, __FILE__, __LINE__);
        break;
    }
  } while (1);
}

/*********************************************************************
*
* @purpose Task to receive UDP packets from name servers
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void dnsRxTask(void)
{
  (void)osapiTaskInitDone(L7_DNS_CLIENT_RX_TASK_SYNC);

  /* Loop forever checking for incoming DNS packets */
  while (L7_TRUE)
  {
    /* This would be better if it was waiting on a condition
       variable instead of waking up every second to check
       if we are expecting any responses... */
    if (dnsCnfgrState == DNS_CLIENT_PHASE_EXECUTE)
    {
      if (dnsNameServerPacketReceive() != L7_SUCCESS)
      {
        /* sleep, then check for responses again */
        osapiSleep(DNS_SERVER_SOCKET_TIMEOUT);
      }
    } else
    {
      osapiSleep(DNS_SERVER_SOCKET_TIMEOUT);
    }
  }
}

