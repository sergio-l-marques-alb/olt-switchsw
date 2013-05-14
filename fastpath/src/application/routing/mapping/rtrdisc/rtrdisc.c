/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtrdisc.c
*
* @purpose   To discover router protocols
*
* @component
*
* @comments  none
*
* @create    11/02/2001
*
* @author    ksdesai
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include <stdlib.h>
#include <sys/times.h>
#include <string.h>

#include "l7_common.h"
#include "l7_common_l3.h"
#include "osapi.h"
#include "nimapi.h"
#include "nvstoreapi.h"
#include "log.h"
#include "simapi.h"
#include "dot3ad_api.h"
#include "mirror_api.h"
#include "l7_ip_api.h"
#include "l3_default_cnfgr.h"

#include "rtrdisc_api.h"
#include "rtrdisc.h"
#include "rtrdisc_config.h"
#include "rtrdisc.h"
#include "rtrdisc_util.h"
#include "rtrdisc_cnfgr.h"

extern rtrDiscCfgData_t         *rtrDiscCfgData;
extern rtrDiscCnfgrState_t      rtrDiscCnfgrState;
extern rtrDiscIntf_t            *rtrDiscIntf;
extern L7_uint32                *rtrDiscMapTbl;

extern osapiRWLock_t            rtrDiscIntfRWLock;

extern void rtrDiscGetTimeOfDay( struct timeval *tv , void *not_used );

rtrDiscInfo_t          *pRtrDiscInfo;
rtrDiscIntfInfo_t      *pRtrDiscIntfInfo;


/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t rtrDiscIntfCreate(L7_uint32 intIfNum)
{
    nimConfigID_t           configId;
    rtrDiscIntfCfgData_t    *pCfg = NULL;
    L7_uint32 index;

    if (rtrDiscIntfIsValid(intIfNum) != L7_TRUE)
        return L7_FAILURE;

    if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
        return L7_FAILURE;

    /* Shouldn't have this interface yet. Verify */
    index = rtrDiscMapTbl[intIfNum];
    if (index != 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RTR_DISC_COMPONENT_ID,
              "Got interface create event for existing interface");
      return L7_FAILURE;
    }

    if (rtrDiscMapIntfConfigEntryGet(intIfNum, &pCfg))
    {
        NIM_CONFIG_ID_COPY(&(pCfg->configId), &(configId));
        rtrDiscIntfBuildDefaultConfigData(pCfg);
        return L7_SUCCESS;
    }

    return L7_FAILURE;
}


/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t rtrDiscIntfDelete(L7_uint32 intIfNum)
{
    rtrDiscIntfCfgData_t *pCfg = L7_NULL;

    if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
        return L7_FAILURE;

    if ((pCfg==L7_NULL) || (rtrDiscCfgData==L7_NULL)) {
        return L7_FAILURE;
    }

    memset(&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset(&rtrDiscMapTbl[intIfNum], 0, sizeof(L7_uint32));
    memset(&pRtrDiscIntfInfo[intIfNum], 0, sizeof(rtrDiscIntfInfo_t));
    rtrDiscCfgData->cfgHdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This routine is called by IP Map if a routing event occurs
*           on the interface.
*
* @param    intIfnum    internal interface number
* @param    event    @b{(input)} an event listed in L7_RTR_EVENT_CHANGE_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @comments Enqueues the routing event onto the processing queue.
*           The processing task handles the event through
*           rtrDiscRoutingEventChangeProcess()
*
* @end
*********************************************************************/
L7_RC_t rtrDiscRoutingEventChangeCallBack (L7_uint32 intIfNum,
                                           L7_uint32 event,
                                           void *pData,
                                           ASYNC_EVENT_NOTIFY_INFO_t *response)
{
  L7_RC_t rc;
  rtrDiscMsg_t msg;

  if (pRtrDiscInfo->rtrDiscProcessingQueue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&msg, 0, sizeof(rtrDiscMsg_t) );
  msg.msgId                     = RTR_DISC_RTR_EVENT;
  msg.type.rtrEvent.intIfNum    = intIfNum;
  msg.type.rtrEvent.event       = event;
  msg.type.rtrEvent.pData       = pData;    /* Warning: when/if event-specific data is used,
                                               need to ensure that the information pointed to
                                               will still be valid (i.e. is fairly persistent
                                               until all tasks which need to use it can access
                                               it, or that, for a given event, the information
                                               is copied. */
  if (response != L7_NULL)
  {
    memcpy(&(msg.type.rtrEvent.response), response, sizeof(ASYNC_EVENT_NOTIFY_INFO_t) );
  }


  rc = osapiMessageSend( pRtrDiscInfo->rtrDiscProcessingQueue, &msg, sizeof(rtrDiscMsg_t),
                         L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RTR_DISC_COMPONENT_ID,
           "Failed to send message to Router Discovery message queue");
  }

  return rc;
}

/*********************************************************************
* @purpose  Handles routing interface enabled
*
* @param    intIfnum    internal interface number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscRtrIntfEnable(L7_uint32 intIfNum)
{
    struct timeval tv;

    L7_uint32 ticksLeft;
    rtrDiscIntf_t *present, *newPtr;
    rtrDiscIntfCfgData_t *pCfg;

    if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
        return;

    if (rtrDiscMayEnableInterface(intIfNum))
    {
        /* add routing interface to list of routing interfaces */
        osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
        present = rtrDiscIntf;

        /* checks if the interface is already in the linked list */
        while (present != L7_NULLPTR)
        {
            if (present->intIfNum == intIfNum)
            {
                osapiWriteLockGive(rtrDiscIntfRWLock);
                return;
            }
            present = present->next;
        }

        newPtr = (rtrDiscIntf_t*)osapiMalloc(L7_RTR_DISC_COMPONENT_ID, sizeof (rtrDiscIntf_t));
        newPtr->intIfNum = intIfNum;
        rtrDiscGetTimeOfDay(&tv, (struct timezone *)0);
        srand(tv.tv_sec);
        ticksLeft = rand();
        ticksLeft = (L7_uint32)((ticksLeft / ((L7_double64)RAND_MAX + 1)) * (pCfg->maxAdvInt - pCfg->minAdvInt));
        ticksLeft += pCfg->minAdvInt;
        if (ticksLeft >= MAX_INITIAL_ADVERT_INTERVAL)
        {
            ticksLeft = MAX_INITIAL_ADVERT_INTERVAL;
        }
        newPtr->ticksLeft = ticksLeft;
        newPtr->numInitialAdvLeft = MAX_INITIAL_ADVERTISEMENTS - 1;
        newPtr->next = rtrDiscIntf;
        newPtr->prev = L7_NULLPTR;
        newPtr->state = L7_TRUE;
        if (rtrDiscIntf != L7_NULLPTR)
        {
            /* If there is one node in the linked list. */
            rtrDiscIntf->prev = newPtr;
        }
        rtrDiscIntf = newPtr;

        osapiWriteLockGive(rtrDiscIntfRWLock);
    }
}

/*********************************************************************
* @purpose  Handle routing interface disable
*
* @param    intIfnum    internal interface number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscRtrIntfDisable(L7_uint32 intIfNum)
{
    rtrDiscIntf_t *present, *prev = 0;

    /* When routing is disabled on a interface, no router discovery messages
       should be sent out of that interface. So remove from storage */

    osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
    if (!rtrDiscIntf)
    {
      osapiWriteLockGive(rtrDiscIntfRWLock);
      return;
    }
    present = rtrDiscIntf;
    prev = rtrDiscIntf->prev;
    while (present != L7_NULLPTR)
    {
        if (present->intIfNum == intIfNum)
        {
            if (present->prev == L7_NULLPTR)
            {
                /* It is the head node. The linked list can contain more than
                   1 node or only 1 node*/
                rtrDiscIntf = present->next;
                if (present->next != L7_NULLPTR)
                {
                    /* Linked list contains more than one node*/
                    (present->next)->prev = L7_NULLPTR;
                }
                osapiFree(L7_RTR_DISC_COMPONENT_ID, present);
                break;
            }
            /* Linked list contains more than 1 node. If the control came here,
               we are either trying to delete an internal node or the last node
            */
            prev->next = present->next;
            if (present->next != L7_NULLPTR)
            {
                /* If it is internal node */
                (present->next)->prev = prev;
            }
            osapiFree(L7_RTR_DISC_COMPONENT_ID, present);
            break;
        }
        prev = present;
        present = present->next;
    }

    osapiWriteLockGive(rtrDiscIntfRWLock);
}

/*********************************************************************
* @purpose  Router Discovery's processing task. Handles cnfgr commands,
*           routing events (from IP MAP)
*           and the RESTORE event (invoked asynchronously on execution
*           of the "clear config" command).
*
* @param    intIfnum    internal interface number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscProcessingTask(void)
{
    rtrDiscMsg_t  Message;
    L7_uint32     status;
    L7_uint32     rtrDisc_service_count = 0;  /* Number of messages serviced
                                                 on current invocation of task */

    osapiTaskInitDone(L7_RTR_DISC_PROCESSING_TASK_SYNC);

    /* Main body of task */
    for (;;)
    {
        /* zero the message */
        memset(&Message,0x00,sizeof(rtrDiscMsg_t));

        /* Don't try process anything unless the Queue has been created */
        if (pRtrDiscInfo->rtrDiscProcessingQueue != L7_NULLPTR)
        {
            /*  Get a Message From The Queue */
            status = osapiMessageReceive(pRtrDiscInfo->rtrDiscProcessingQueue,
                                         &Message,
                                         sizeof(rtrDiscMsg_t),
                                         L7_WAIT_FOREVER);

            if (status == L7_SUCCESS)
            {
                switch (Message.msgId)
                {

                case RTR_DISC_RTR_EVENT:
                    (void)rtrDiscRoutingEventChangeProcess (Message.type.rtrEvent.intIfNum,
                                                            Message.type.rtrEvent.event,
                                                            Message.type.rtrEvent.pData,
                                                            &(Message.type.rtrEvent.response));
                    break;

                case RTR_DISC_CNFGR:
                    rtrDiscCnfgrParse(&Message.type.cnfgrCmdData);
                    break;

                case RTR_DISC_RESTORE_EVENT:
                    rtrDiscRestoreProcess();
                    break;

                case RTR_DISC_TICK:
                    rtrDiscTimerTickProcess();
                    break;

                default:
                    break;
                } /* endswitch */
            } else
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RTR_DISC_COMPONENT_ID,
                        "Bad message queue status %08x", status);
            }
        }
        /* Let other tasks run, if required.                */
        rtrDisc_service_count++;
        if (rtrDisc_service_count == FD_CNFGR_RTR_DISC_MAX_MSGS_SERVICED)
        {
            rtrDisc_service_count = 0;
            (void) osapiTaskYield();
        }

    } /* for (;,;,;) */
}

/*********************************************************************
* @purpose  This routine is called asynchronously when a routing event
*           occurs that may change the operational state of the router
*           discovery component.
*
* @param    intIfnum    port changing state
* @param    event       one of L7_PORT_EVENTS_t
* @param    *pData      pointer to event specific data
* @param    *pResponse  pointer to event specific data
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Indicates to IP MAP completion of event handling through
*           asyncEventCompleteTally().
*
* @end
*********************************************************************/
L7_RC_t rtrDiscRoutingEventChangeProcess (L7_uint32 intIfNum,
                                          L7_uint32 event,
                                          void *pData,
                                          ASYNC_EVENT_NOTIFY_INFO_t *pResponse)
{

    L7_BOOL          must_respond;
    L7_BOOL          event_completed;
    ASYNC_EVENT_COMPLETE_INFO_t event_completion;
    L7_RC_t rc;
    L7_uint32 i;

    /*------------------*/
    /* Initialize data  */
    /*------------------*/

    must_respond      = L7_FALSE;
    event_completed   = L7_FALSE;
    memset(&event_completion, 0, sizeof( ASYNC_EVENT_COMPLETE_INFO_t) );

    if (pResponse != L7_NULLPTR)
    {
        if ( (pResponse->handlerId != L7_NULL)  && (pResponse->correlator != L7_NULL) )
        {
            must_respond = L7_TRUE;

            /* Initialize event completion data */
            /* NOTE:  Purposely overloading (over-riding?) componentID with L7_IPMAP_REGISTRANTS_t */
            event_completion.componentId  = L7_IPRT_RTR_DISC;
            event_completion.handlerId    = pResponse->handlerId;
            event_completion.correlator   = pResponse->correlator;

            /* Default to return codes  and status.  Only select issues are not
               completed on this thread. */
            event_completion.async_rc.rc      = L7_SUCCESS;
            event_completion.async_rc.reason  = ASYNC_EVENT_REASON_RC_SUCCESS;
            event_completed                   = L7_TRUE;

        } /*  non null handlerId and correlator */
    }

    if (intIfNum == 0 || rtrDiscIntfIsValid(intIfNum) == L7_TRUE)
    {
        switch (event)
        {
        case L7_RTR_ENABLE:
            break;

        case L7_RTR_DISABLE_PENDING:
          /* implies all routing interfaces down */
          for (rc = nimFirstValidIntfNumber(&i); rc == L7_SUCCESS;
                rc = nimNextValidIntfNumber(i, &i))
          {
            rtrDiscRtrIntfDisable(i);
          }
          break;

        case L7_RTR_DISABLE:
          break;

        case L7_RTR_INTF_CREATE:
          rtrDiscIntfCreate(intIfNum);
          break;

        case L7_RTR_INTF_DELETE:
          rtrDiscIntfDelete(intIfNum);
          break;

        case L7_RTR_INTF_ENABLE:
            rtrDiscRtrIntfEnable(intIfNum);
            break;

        case L7_RTR_INTF_DISABLE_PENDING:
            rtrDiscRtrIntfDisable(intIfNum);
            break;

        default:
            break;
        }
    }

    /*-------------------*/
    /* Event completion  */
    /*-------------------*/

    if ( (must_respond == L7_TRUE) && (event_completed == L7_TRUE) )
    {
        /* Return event completion status  */
        asyncEventCompleteTally(&event_completion);
    }

    return L7_SUCCESS;
}

