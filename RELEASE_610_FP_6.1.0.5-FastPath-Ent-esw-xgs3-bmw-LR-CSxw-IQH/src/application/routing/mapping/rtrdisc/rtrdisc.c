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
* @purpose  To process NIM events.
*
* @param    L7_uint32  intIfNum   internal interface number
* @param    L7_uint32  event      event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Enqueues the interface event onto the processing queue.
*           The processing task handles the event through 
*           rtrDiscIntfChangeProcess()
*
* @end
*
*********************************************************************/
L7_RC_t rtrDiscIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{

    L7_RC_t rc;
    rtrDiscMsg_t msg;
    NIM_EVENT_COMPLETE_INFO_t status;

    if (pRtrDiscInfo->rtrDiscProcessingQueue == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    /* If this is an event we are not interested in perform an early return*/
    if (event != L7_CREATE &&
        event != L7_DELETE &&
        event != L7_ACTIVE &&
        event != L7_INACTIVE &&
        event != L7_FORWARDING &&
        event != L7_NOT_FORWARDING &&
        event != L7_LAG_ACQUIRE &&
        event != L7_PROBE_SETUP &&
        event != L7_LAG_RELEASE &&
        event != L7_PROBE_TEARDOWN)
    {
      status.intIfNum     = intIfNum;
      status.component    = L7_RTR_DISC_COMPONENT_ID;
      status.response.rc  = L7_SUCCESS;
      status.event        = event;
      status.correlator   = correlator;
      nimEventStatusCallback(status);
      return L7_SUCCESS;
    }

    memset(&msg, 0, sizeof(rtrDiscMsg_t) );
    msg.msgId                     = RTR_DISC_INTF_EVENT;
    msg.type.intfEvent.intIfNum   = intIfNum;
    msg.type.intfEvent.event      = event;
    msg.type.intfEvent.correlator = correlator;

    rc = osapiMessageSend( pRtrDiscInfo->rtrDiscProcessingQueue, &msg, sizeof(rtrDiscMsg_t), 
                           L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

    if (rc != L7_SUCCESS)
    {
        LOG_MSG("rtrDiscIntfChangeCallback: osapiMessageSend failed\n");
    }

    return rc;
}


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
    L7_RC_t                 rtrDiscRC = L7_SUCCESS;
    nimConfigID_t           configId;
    rtrDiscIntfCfgData_t    *pCfg;
    L7_uint32               i;

    if (rtrDiscIntfIsValid(intIfNum) != L7_TRUE)
        return L7_FAILURE;

    if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
        return L7_FAILURE;

    for (i = 1; i < L7_RTR_DISC_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&rtrDiscCfgData->rtrDiscIntfCfgData[i].configId, &configId))
      {
        rtrDiscMapTbl[intIfNum] = i;
        break;
      }
    }
    
    
    pCfg = L7_NULL;

    /* If an interface configuration entry is not already assigned to the interface,
       assign one */

    /* We previously checked whether the interface is valid so if this function returns
    ** L7_FALSE we can safely assume its due to the fact that the interface is not present
    */
    if (rtrDiscMapIntfIsConfigurable(intIfNum, L7_NULLPTR) == L7_FALSE)
    {
        if (rtrDiscMapIntfConfigEntryGet(intIfNum, &pCfg) == L7_FALSE)
        {
            rtrDiscRC = L7_FAILURE;
        }
    }

    if ((pCfg != L7_NULL) && (rtrDiscRC == L7_SUCCESS))
    {
        NIM_CONFIG_ID_COPY(&(pCfg->configId), &(configId));
        rtrDiscIntfBuildDefaultConfigData(pCfg);
    }
    /* Update the configuration structure with the config id */

    return rtrDiscRC;
}


/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE_PENDING
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
L7_RC_t rtrDiscIntfDeletePending(L7_uint32 intIfNum)
{
    rtrDiscIntfCfgData_t *pCfg;

    if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
        return L7_FAILURE;

    rtrDiscIntfBuildDefaultConfigData(pCfg);

    if (rtrDiscApplyIntfConfigData(intIfNum) != L7_SUCCESS)
        return L7_FAILURE;

    return L7_SUCCESS;
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
    }; 
    
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
    static const char *routine_name = "rtrDiscRoutingEventChangeCallBack()";


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
        LOG_MSG("%s: %d: %s: Unable to send message on Router Dicovery processing message queue\n",
                __FILE__, __LINE__, routine_name);
    }

    return rc;
}

/*********************************************************************
* @purpose  Handles an ip address being added or changed
*
* @param    intIfnum    internal interface number
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscRtrIntfIpAddressChange(L7_uint32 intIfNum)
{
    struct timeval tv;

    L7_IP_ADDR_t ipAddr;
    L7_IP_MASK_t mask;
    L7_uint32 rtrIfNum;
    L7_uint32 mode, ticksLeft;
    rtrDiscIntf_t *present, *newPtr;
    rtrDiscIntfCfgData_t *pCfg;

    if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
        return;

    /* Get the rtr interface number */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS)
        {

            if ( (ipMapRtrIntfModeGet(intIfNum, &mode) == L7_SUCCESS) && 
                 (mode == L7_ENABLE) &&
                 pCfg->advertise == L7_TRUE)
            {
                /* store in storage */

                osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
                present = rtrDiscIntf;

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
                newPtr->state = rtrDiscLinkStateGet(intIfNum);
                if (rtrDiscIntf != L7_NULLPTR)
                {
                    /* If there is one node in the linked list. */
                    rtrDiscIntf->prev = newPtr;
                }
                rtrDiscIntf = newPtr;

                osapiWriteLockGive(rtrDiscIntfRWLock);
            }
        }
    }
}



/*********************************************************************
* @purpose  Handles an ip address being deleted    
*
* @param    intIfnum    internal interface number
*
* @returns  void
*       
* @notes    none
*       
* @end
*********************************************************************/
void rtrDiscRtrIntfIpAddressDelete(L7_uint32 intIfNum)
{
    L7_uint32 rtrIfNum;
    rtrDiscIntf_t *present, *prev = 0;

    /* Get the rtr interface number */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
        /* remove in storage */

        osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
        present = rtrDiscIntf;
        if (rtrDiscIntf != L7_NULLPTR)
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
}



/*********************************************************************
* @purpose  Handles router being enabled    
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

    L7_IP_ADDR_t ipAddr;
    L7_IP_MASK_t mask;
    L7_uint32 rtrIfNum, ticksLeft;
    rtrDiscIntf_t *present, *newPtr;
    rtrDiscIntfCfgData_t *pCfg;

    if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
        return;

    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS &&
            pCfg->advertise == L7_TRUE)
        {
            if (ipAddr != L7_NULL_IP_ADDR)
            {
                /* store in storage */

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
                newPtr->state = rtrDiscLinkStateGet(intIfNum);
                if (rtrDiscIntf != L7_NULLPTR)
                {
                    /* If there is one node in the linked list. */
                    rtrDiscIntf->prev = newPtr;
                }
                rtrDiscIntf = newPtr;

                osapiWriteLockGive(rtrDiscIntfRWLock);
            }
        }
    }
}

/*********************************************************************
* @purpose  Handles router being disabled    
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
* @purpose  Handles link becoming active
*
* @param    intIfnum    internal interface number
*
* @returns  L7_SUCCESS	if rtrdisc starts sending out adv pkts
* @returns  L7_FAILURE  if rtrdisc cannot start sending out adv pkts
*       
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscLinkActive(L7_uint32 intIfNum)
{
    struct timeval tv;

    rtrDiscIntf_t *present;
    L7_uint32 state;
    L7_uint32 ticksLeft;
    rtrDiscIntfCfgData_t *pCfg;

    if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
        return L7_FAILURE;

    /* Link becomes active only if the interface is not member of lag and
       not mirror dest port and the link is in forwarding state.
    */
    state = rtrDiscLinkStateGet(intIfNum);
    if (state == L7_FALSE)
        return L7_SUCCESS;

    /* If the interface is enabled for routing and has an ip address
       then it would have a node in the rtrDiscIntf linked list.
     Re-initialize it and start sending out adv packets.
    */

    osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
    present = rtrDiscIntf;
    while (present != L7_NULLPTR)
    {
        if (present->intIfNum == intIfNum)
        {
            present->state = L7_TRUE;
            rtrDiscGetTimeOfDay(&tv, (struct timezone *)0);
            srand(tv.tv_sec);
            ticksLeft = rand();
            ticksLeft = (L7_uint32)((ticksLeft / ((L7_double64)RAND_MAX + 1)) * (pCfg->maxAdvInt - pCfg->minAdvInt));
            ticksLeft += pCfg->minAdvInt;
            if (ticksLeft >= MAX_INITIAL_ADVERT_INTERVAL)
            {
                ticksLeft = MAX_INITIAL_ADVERT_INTERVAL;
            }
            present->ticksLeft = ticksLeft;
            present->numInitialAdvLeft = MAX_INITIAL_ADVERTISEMENTS - 1;
            present->state = state;

            osapiWriteLockGive(rtrDiscIntfRWLock);
            return L7_SUCCESS;
        }
        present = present->next;
    }

    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handles link becoming inactive
*
* @param    intIfnum    internal interface number
*
* @returns  L7_SUCCESS	if rtrdisc stops sending out adv pkts
* @returns  L7_FAILURE  if rtrdisc cannot stop sending out adv pkts
*       
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscLinkInActive(L7_uint32 intIfNum)
{
    rtrDiscIntf_t *present;


    osapiWriteLockTake(rtrDiscIntfRWLock, L7_WAIT_FOREVER);
    present = rtrDiscIntf;
    while (present != L7_NULLPTR)
    {
        if (present->intIfNum == intIfNum)
        {
            present->state = L7_FALSE;

            osapiWriteLockGive(rtrDiscIntfRWLock);
            return L7_SUCCESS;
        }
        present = present->next;
    }

    osapiWriteLockGive(rtrDiscIntfRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Router Discovery's processing task. Handles cnfgr commands,
*           interface events (from NIM), routing events (from IP map)
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

    static const char *routine_name = "rtrDiscProcessingTask()";

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
                
                case RTR_DISC_INTF_EVENT:
                    (void)rtrDiscIntfChangeProcess(Message.type.intfEvent.intIfNum,
                                                   Message.type.intfEvent.event,
                                                   Message.type.intfEvent.correlator);
                    break;

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
                LOG_MSG("%s %d: %s: Bad message queue status %08x\n",
                        __FILE__, __LINE__, routine_name, status);
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
* @purpose  Process interface-related events
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes    L7_PORT_ROUTING_ENABLED and L7_PORT_ROUTING_DISABLED events 
*           (other than updating information to reflect the interface 
*           being acquired/released by IP map for routing)
*           are not handled by this routine, as these events originate
*           from inside the IP Mapping layer. Since the router discovery mapping layer 
*           is a "slave" to the IP Mapping layer, these events are 
*           handled via its registration with IP map.
*
* @notes    Gets invoked asynchronously. nimEventStatusCallback() is invoked to
*           to tell NIM the event has been handled.
*       
* @end
*********************************************************************/

L7_RC_t rtrDiscIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, 
                                 NIM_CORRELATOR_t correlator)
{
    L7_RC_t rtrDiscRC = L7_SUCCESS;
    NIM_EVENT_COMPLETE_INFO_t status;    

    L7_BOOL currAcquired, prevAcquired;

    status.intIfNum     = intIfNum;
    status.component    = L7_RTR_DISC_COMPONENT_ID;
    status.response.rc  = L7_SUCCESS;
    status.event        = event;
    status.correlator   = correlator;


    if (rtrDiscIntfIsValid(intIfNum) != L7_TRUE)
    {
        nimEventStatusCallback(status);
        return L7_SUCCESS;
    }

    if (RTR_DISC_IS_READY == L7_TRUE)
    {
        switch (event)
        {
        case L7_CREATE:
            rtrDiscRC = rtrDiscIntfCreate(intIfNum);
            break;

        case L7_ATTACH:
            /* Do nothing.  Wait for routing to be enabled on the interface */
            break;

        case L7_DETACH:
            /* Do nothing.  Wait for routing to be enabled on the interface */
            break;

        case L7_DELETE:
            rtrDiscRC = rtrDiscIntfDelete(intIfNum);
            break;

        case L7_UP:
            /* do nothing here - key off of L7_ACTIVE instead */
            break;

        case L7_DOWN:
            /* do nothing here - key off of L7_INACTIVE instead */
            break;

        case L7_ACTIVE:
            COMPONENT_ACQ_NONZEROMASK(pRtrDiscIntfInfo[intIfNum].acquiredList, currAcquired);

            /* If the interface is not currently acquired by some component, activate
             * router discovery on the interface
             */

            if (currAcquired == L7_FALSE)
            {
                rtrDiscRC = rtrDiscLinkActive(intIfNum);
            }
            break;

        case L7_INACTIVE:
            rtrDiscRC = rtrDiscLinkInActive(intIfNum);
            break;

        case L7_FORWARDING:
            rtrDiscRC = rtrDiscLinkActive(intIfNum);
            break;

        case L7_NOT_FORWARDING:
            rtrDiscRC = rtrDiscLinkInActive(intIfNum);
            break;

        case L7_LAG_ACQUIRE:
            /* check if previously acquired */
            COMPONENT_ACQ_NONZEROMASK(pRtrDiscIntfInfo[intIfNum].acquiredList, prevAcquired);

            /* update aquired list */
            COMPONENT_ACQ_SETMASKBIT(pRtrDiscIntfInfo[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);

            if (prevAcquired == L7_FALSE)
            {
                /* LAG members do not act independently as router interfaces */
                rtrDiscRC = rtrDiscLinkInActive(intIfNum);
            }
            break;

        case L7_PROBE_SETUP:
            /* check if previously acquired */
            COMPONENT_ACQ_NONZEROMASK(pRtrDiscIntfInfo[intIfNum].acquiredList, prevAcquired);

            /* update aquired list */
            COMPONENT_ACQ_SETMASKBIT(pRtrDiscIntfInfo[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

            if (prevAcquired == L7_FALSE)
            {
                rtrDiscRC = rtrDiscLinkInActive(intIfNum);
            }
            break;

        case L7_LAG_RELEASE:
            /* update aquired list */
            COMPONENT_ACQ_CLRMASKBIT(pRtrDiscIntfInfo[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);

            /* check if currently acquired */
            COMPONENT_ACQ_NONZEROMASK(pRtrDiscIntfInfo[intIfNum].acquiredList, currAcquired);

            if (currAcquired == L7_FALSE)
            {
                rtrDiscRC = rtrDiscLinkActive(intIfNum);
            }
            break;

        case L7_PROBE_TEARDOWN:
            /* update aquired list */
            COMPONENT_ACQ_CLRMASKBIT(pRtrDiscIntfInfo[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

            /* check if currently acquired */
            COMPONENT_ACQ_NONZEROMASK(pRtrDiscIntfInfo[intIfNum].acquiredList, currAcquired);

            if (currAcquired == L7_FALSE)
            {
                rtrDiscRC = rtrDiscLinkActive(intIfNum);
            }
            break;

        case L7_SPEED_CHANGE:
            /* Nothing to do for router discovery??? */
            break;

        case L7_PORT_ROUTING_ENABLED:
            break;

        case L7_PORT_ROUTING_DISABLED:
            break;

        default:
            rtrDiscRC = L7_SUCCESS; /* If its an event we dont care about, return SUCCESS */
            break;
        }
    } else
    {
        LOG_MSG("%s: %d: rtrDiscIntfChangeProcess(): Received a interface callback while outside the EXECUTE state", 
            __FILE__, __LINE__);
        rtrDiscRC = L7_FAILURE;
    }

    status.response.rc = rtrDiscRC;
    nimEventStatusCallback(status);

    return rtrDiscRC;

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
* @notes    Indicates to IP map, completion of event handling through 
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

    
    rtrDiscIntfCfgData_t *pCfg;

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
    
        case L7_RTR_INTF_ENABLE:
            rtrDiscRtrIntfEnable(intIfNum); 
            break;
    
        case L7_RTR_INTF_DISABLE_PENDING:
            rtrDiscRtrIntfDisable(intIfNum); 
            break;
    
        case L7_RTR_INTF_DISABLE:
            /* Nothing to do. We cleaned up on the disable pending */
            break;
    
        case L7_RTR_INTF_IPADDR_ADD:
            /* deprecated */
            break;
    
        case L7_RTR_INTF_SECONDARY_IP_ADDR_ADD:
            /* Ignore the event. The new IP address advertisements will be 
               automatically done during the next timer tick event.*/
            break;
    
        case L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE:
            /* Ignore the event. Advertisemnet of deletion will be 
               automatically done during the next timer tick event.*/
               
            break;
    
        case L7_RTR_INTF_IPADDR_DELETE_PENDING:
            /* deprecated */
            break;
    
        case L7_RTR_INTF_IPADDR_DELETE:
            /* deprecated */
            break;
    
        case L7_RTR_INTF_CONFIG_CLEAR:
            if (rtrDiscMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
                break;
            rtrDiscIntfBuildDefaultConfigData(pCfg);
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

