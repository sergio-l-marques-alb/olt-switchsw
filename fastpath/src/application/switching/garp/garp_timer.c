/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garp_timer.c
* @purpose     Implementaion of the GARP timer task
* @component   GARP
* @comments    none
* @create      10/02/2005
* @author
* @author
* @end
*
**********************************************************************/

#include <stdlib.h>
#include "l7_common.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "garp.h"
#include "garpcfg.h"
#include "gvrapi.h"
#include "gmrapi.h"
#include "garp_timer.h"
#include "garp_debug.h"
#include "garp_leaveall_timer.h"


garpTimerInfo_t garpTimers;
osapiTimerDescr_t   *pGarpTimerHolder;


extern GarpInfo_t garpInfo;
extern GARPCBptr GARPCB;
extern osapiTimerDescr_t   *pGarpLeaveAllTimerHolder;

extern void garpTraceQueueMsgSend(GarpPacket *msg);
void garpDebugTimerListPrint(garpTimerList_t *tempList, L7_uint32 limit);
void garpDebugUnitTestGarpTimerList(L7_uint32 action,L7_uint32 numEntries);

/*********************************************************************
* @purpose  Initialize the timer list
*
*
* @param    NONE
*
* @returns  NONE
*
*
* @notes
*
* @end
*********************************************************************/
void garpTimerListInit()
{
    GARP_TRACE_TIMER_EVENTS("In timer list init \n");
    garpTimers.garpTimersPendingList.sema = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    garpTimers.garpTimersPendingList.head = L7_NULLPTR;
    garpTimers.garpTimersPendingList.listCount = L7_NULL;
    garpTimers.garpTimersPendingList.listType = GARP_TIMER_PENDING;

    garpTimers.garpTimersPoppedList.sema = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    garpTimers.garpTimersPoppedList.head = L7_NULLPTR;
    garpTimers.garpTimersPoppedList.listCount = L7_NULL;
    garpTimers.garpTimersPoppedList.listType = GARP_TIMER_POPPED;

    GARP_TRACE_TIMER_EVENTS("Pending list type %d popped %d \n",
                            garpTimers.garpTimersPendingList.listType,
                            garpTimers.garpTimersPoppedList.listType);

}

/*********************************************************************
* @purpose  Clean up the timer list
*
*
* @param    NONE
*
* @returns  NONE
*
*
* @notes
*
* @end
*********************************************************************/
void garpTimerListFini()
{
    GARP_TRACE_TIMER_EVENTS("In function garpTimerListFini \n");

    /* Fini is unsupported as of this writing */
    LOG_ERROR(0);

    if (&garpTimers.garpTimersPendingList != L7_NULL)
    {
        if (garpTimers.garpTimersPendingList.sema != L7_NULLPTR)
        osapiSemaDelete(garpTimers.garpTimersPendingList.sema);
    }

    if (&garpTimers.garpTimersPendingList != L7_NULL)
    {
        if (garpTimers.garpTimersPoppedList.sema != L7_NULLPTR)
        osapiSemaDelete(garpTimers.garpTimersPoppedList.sema);
    }


}



/*********************************************************************
* @purpose  Inserts pTimer in the list provided.
*
*
* @param    *pTimer          @b{{input}} pointer to a garpTimerDescr_t structure
* @param    *timerList       @b{{input}} pointer to the timer list
*
* @returns  L7_SUCCESS on success
*           L7_FAILURE otherwise
*
*
* @notes    Inserts the element in the list sorted by the expiry time
*           with the earliest to expire first.
*
* @end
*********************************************************************/
static L7_RC_t garpTimerListAddElement(garpTimerDescr_t *pTimer, garpTimerList_t *timerList)
{
    garpTimerDescr_t *curr;
    garpTimerDescr_t *prev;


    if ((pTimer == L7_NULL) || (timerList == L7_NULL))
    {
        return L7_FAILURE;
    }

    GARP_TRACE_TIMER_EVENTS("Timer(%x) expiry time(%d) type(%d) port(%d) vid(%d) status(%d) added to %d queue \n",
                            pTimer,pTimer->expiryTime,pTimer->timerType,pTimer->port_no,
                            pTimer->vid, pTimer->timer_status,timerList->listType);
    curr = timerList->head;
    prev = timerList->head;

    if (curr == L7_NULL)
    {
       /* Timer is first element to populate list, thus automatically becomes the head */
        timerList->head = pTimer;
        pTimer->next    = L7_NULL;
    }
    else if (pTimer->expiryTime < curr->expiryTime)
    {
        /* Timer expires sooner than the first element in the list.
           Thus make it the head of the existing list */
       pTimer->next = timerList->head;
       timerList->head = pTimer;

    }
    else
    {

        /* Timer expires after the first element in the list.
           Search for where to insert it, either in the middle or at the end of the list,
           as appropiate. */
        curr = timerList->head;
        prev = timerList->head;

        while (curr != L7_NULL)
        {
            if (pTimer->expiryTime >= curr->expiryTime)
            {
                /* continue search */
                prev = curr;
                curr = curr->next;
            }
            else
                break;
        }

        /* Insert between previous and current */
        pTimer->next = curr;
        prev->next = pTimer;
    }

    /* Counter maintained for debug only */
    timerList->listCount++;

    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Removes first element from the head of the list and returns
*           a pointer to that element to the user.
*
*
* @param    *timerList      @b{{input}} pointer to garpTimerList_t structure
 *
* @returns  on success, returns pointer to the timer descriptor removed from the list
*           on failure, returns L7_NULLPTR on failure or if list is empty
*
* @notes
*
* @end
*********************************************************************/
garpTimerDescr_t * garpTimerListDeleteFirst(garpTimerList_t *timerList)
{
    garpTimerDescr_t *curr;

    if (timerList == L7_NULL)
    {
        return L7_NULL;
    }

    if (timerList->head == L7_NULL)
    {
        return L7_NULL;
    }

    curr = timerList->head;
    GARP_TRACE_TIMER_EVENTS("Timer(%x) expiry time(%d) type(%d) port(%d) vid(%d) status(%d) deleted from %d queue \n",
                          curr,curr->expiryTime,curr->timerType,curr->port_no,
                          curr->vid, curr->timer_status,timerList->listType);

    timerList->head = curr->next;
    curr->next = L7_NULL;


    return curr;

}

/*********************************************************************
* @purpose  Deletes the a particular timer from the list .
*
* @param    *pTimer       @b{{input}} pointer to a garpTimerDescr_t structure
* @param    *timerList       @b{{input}} pointer to the timer list
*
* @returns  void
*
* @notes    L7_LOGF on error. Note the responsibility of this function is to
*           simply clear the contents of the memory location pointed to by
*           the pTimer and update its list. It will not free up the memory.
*
* @end
*********************************************************************/
static L7_RC_t garpTimerListDeleteElement(garpTimerDescr_t *pTimer, garpTimerList_t *timerList)
{
    L7_RC_t rc = L7_FAILURE;
    garpTimerDescr_t *curr = L7_NULL;
    garpTimerDescr_t *prev = L7_NULL;

    curr = L7_NULL;
    prev = L7_NULL;

    if (pTimer == L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "Error: pTimer is L7_NULL\n");
        return rc;
    }


    if (timerList == L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "Error:Timer Type (%d) of the timer (%x) invalid :timerList cannot be found \n", pTimer->timerType , pTimer);
        return rc;
    }

    if (timerList->head == L7_NULL)
    {

        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "Error: attempting to delete from an empty timer list: list type %d\n",
                timerList->listType);
        return rc;
    }

    if (pTimer == timerList->head)
    {
        timerList->head = pTimer->next;
    }
    else
    {
        curr = timerList->head;
        prev = curr;
        while (curr != L7_NULL)
        {
            if (curr == pTimer)
            {
                /* Remove pTimer from the chain */
                prev->next = curr->next;
                break;
            }
            else
            {
                prev = curr;
                curr = curr->next;
            }

        } /*  (curr != L7_NULL) */

    } /* else (pTimer != timerList->head) */


    /* Counter maintained for debug only */
    timerList->listCount--;

    return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Get the expiry time for the first element in the list.
*
*
* @param    *timerList       @b{{input}}  pointer to the timer list
* @param    *pTimer          @b{{output}} pointer to integer to store the expiry time
*
* @returns  L7_SUCCESS on success
*           L7_ERROR   if there are no elements in the list
*           L7_FAILURE otherwise
*
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t garpTimerListFirstExpiryTimeGet(garpTimerList_t *timerList, L7_uint64 *expiryTime)
{

    L7_RC_t rc = L7_SUCCESS;

    if (timerList == L7_NULL)
    {
        return L7_FAILURE;
    }

    osapiSemaTake(timerList->sema, L7_WAIT_FOREVER);


    if (timerList->head == L7_NULL)
    {
       rc = L7_ERROR; /*Not really a failure case Just that no elements in the list */
    }
    else
    {
        *expiryTime = timerList->head->expiryTime;
    }


    osapiSemaGive(timerList->sema);

    return rc;

}

/*********************************************************************
* @purpose  Gets the count of elements in the list
*
*
* @param    *timerList       @b{{input}} pointer to the timer list
*
* @returns  num of elements  in the list
*
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 garpTimerListCountGet(garpTimerList_t *timerList)
{
    L7_uint32 count;
    osapiSemaTake(timerList->sema, L7_WAIT_FOREVER);

    count = timerList->listCount;

    osapiSemaGive(timerList->sema);

    return count;

}


/*********************************************************************
* @purpose  Removes pTimer from the queue pTimer->timer_status.
*
*
* @param    *pTimer       @b{{input}} pointer to a garpTimerDescr_t structure
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t garpTimerFree(garpTimerDescr_t *pTimer)
{
    garpTimerList_t *timerList;

    if (pTimer == L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "Error: pTimer is L7_NULL\n");
        return L7_FAILURE;
    }

    timerList = (pTimer->timer_status == GARP_TIMER_PENDING) ? &garpTimers.garpTimersPendingList :
                (pTimer->timer_status == GARP_TIMER_POPPED) ? &garpTimers.garpTimersPoppedList : L7_NULL;

    /* delete pTimer from listToSearch */
    if (garpTimerListDeleteElement(pTimer,timerList)!= L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "Failed to free timer: timerType %d, port %d, vid %d, status %d, expiry %d\n",
                pTimer->timerType, pTimer->port_no, pTimer->vid, pTimer->timer_status,
                pTimer->expiryTime);
    }

    /* Mark the timer as being unused */
    pTimer->timer_status = GARP_TIMER_UNDEFINED;

    return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Adds pTimer to the pending timers queue.
*
* @param     vid            @b{{input}} vlanID, if any
* @param     port_no        @b{{input}} intIfNum
* @param     milliseconds   @b{{input}} timeout period
* @param    *pTimer         @b{{input}} pointer to a garpTimerDescr_t structure
* @param     timer_type     @b{{input}} one of GARP_TIMER_TYPE_t
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void garpTimerAdd(L7_uint32 vid, L7_uint32 port_no,
                  L7_int32 milliseconds, garpTimerDescr_t *pTimer,
                  GARP_TIMER_TYPE_t timer_type)
{

    L7_int64        newExpiryTime;

    newExpiryTime = milliseconds + osapiTimeMillisecondsGet64();

    if (pTimer->timer_status != GARP_TIMER_UNDEFINED)
    {
        /* Validate that the timer is the same */
        if ( (pTimer->timerType == timer_type)  &&
             (pTimer->port_no == port_no) &&
             (pTimer->vid == vid) )
        {

            GARP_TRACE_TIMER_EVENTS("Updating timer %x type  %d vid(%d) port(%d) expiry(%d) \n",
                                    pTimer,timer_type,vid,port_no,pTimer->expiryTime);

            /* Timer is being updated.  Unchain, update, then rechain the timer */
            garpTimerFree(pTimer);
            pTimer->expiryTime   = newExpiryTime;
            pTimer->timer_status = GARP_TIMER_PENDING;
            garpTimerListAddElement(pTimer,&garpTimers.garpTimersPendingList);
        }
        else
        {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                    "Attempt to update an unrecognized timer: status %d, type %d, port %d, vid %d\n",
                    pTimer->timer_status, pTimer->timerType, pTimer->port_no, pTimer->vid);
        }
    }
    else
    {
        /* A timer is being added */
        memset(pTimer, 0, sizeof(pTimer));

        pTimer->timerType    = timer_type;
        pTimer->vid          = vid;
        pTimer->port_no      = port_no;
        pTimer->expiryTime   = newExpiryTime;
        pTimer->timer_status = GARP_TIMER_PENDING;

        GARP_TRACE_TIMER_EVENTS("Adding timer %x type  %d vid(%d) port(%d) expiry(%d) \n",
                                pTimer,timer_type,vid,port_no,pTimer->expiryTime);

        garpTimerListAddElement(pTimer,&garpTimers.garpTimersPendingList);
    } /* pTimer->timer_status == GARP_TIMER_UNDEFINED */


    return;
}


/*********************************************************************
* @purpose  Moves all expired timers from pending timers queue
*           to popped timers queue.
*
* @param    void
*
* @returns  void
*
* @notes    If there are any timers that expired and were moved to the
*           expired timers queue, it sends a message to the Garp Task.
*
* @end
*********************************************************************/
void garpTimerAction()
{
    garpTimerList_t *pTimerList;
    garpTimerDescr_t *pTimer;
    L7_BOOL timersPopped;
    L7_uint64 expiryTime = 0;

    /* ordered list:
     * walk the garpTimersPendingList list until an entry is
     * found that has not yet popped */

    timersPopped = L7_FALSE;
    pTimerList   = &garpTimers.garpTimersPendingList ;

    while ((garpTimerListFirstExpiryTimeGet(pTimerList,&expiryTime) ==L7_SUCCESS) &&
           (expiryTime <= osapiTimeMillisecondsGet64()))
    {

            /* Timer has popped
             * Remove the timer from the garpTimersPendingList
             * add timer to the list of garpTimersPoppedList
             * set timersPopped flag to L7_TRUE;
             */
            pTimer = garpTimerListDeleteFirst(pTimerList);

            if (pTimer != L7_NULL)
            {
                pTimer->timer_status = GARP_TIMER_POPPED;
                garpTimerListAddElement(pTimer,&garpTimers.garpTimersPoppedList);
            timersPopped = L7_TRUE;
        }

    }

    return;

}




/*********************************************************************
* @purpose  Activate timers
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Activate timers if GVRP or GMRP is enabled
*
* @end
*********************************************************************/
L7_RC_t garpTimerActivateProcess(void)
{

    if (garpInfo.timersRunning != L7_TRUE)
    {
        garpInfo.timersRunning = L7_TRUE;
        osapiTimerAdd ( (void*)garpTimerCallback,L7_NULL,L7_NULL,
                        GARP_TIMER_GRANULARITY_SHORT,&pGarpTimerHolder );
    }

    if (garpInfo.leaveAllTimersRunning != L7_TRUE)
    {
        garpInfo.leaveAllTimersRunning = L7_TRUE;
        osapiTimerAdd ( (void*)garpLeaveAllTimerCallback,L7_NULL,L7_NULL,
                        GARP_TIMER_GRANULARITY_MS,&pGarpLeaveAllTimerHolder );
    }

    return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Deactivate timers
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Deactivate timers if neither GVRP or GMRP is enabled
*
* @end
*********************************************************************/
L7_RC_t garpTimerDeactivateProcess(GARPApplication application)
{
  GARP_TIMER_TYPE_t t1, t2;
  garpTimerDescr_t *pTimer;
  garpTimerDescr_t *pTimerTemp;

    /* Do nothing.  When a GARP application is disabled, timers will still
       need to pop in order to do all cleanup. Thus, osapiTimers still need
       to fire.  When no more timers need to be serviced after all GARP
       applications are disabled, the timers will naturally not fire.

       Reference garpTimerCallback().
    */
    /* Based on the application remove all the pTimers  that are in the pending list
    * for GVRP - GARP_TIMER_GVRP_LEAVE and GARP_TIMER_GVRP_JOIN
    * for GMRP - GARP_TIMER_GMRP_LEAVE and GARP_TIMER_GMRP_JOIN
    */


  if (application == GARP_GVRP_APP)
  {
     t1 = GARP_TIMER_GVRP_LEAVE;
     t2 = GARP_TIMER_GVRP_JOIN;
  }
  else if (application == GARP_GMRP_APP)
  {
     t1 = GARP_TIMER_GMRP_LEAVE;
     t2 = GARP_TIMER_GMRP_JOIN;
  }
  else
    return L7_FAILURE;

  pTimer = garpTimers.garpTimersPendingList.head;
  while (pTimer != L7_NULL)
  {
    if ((pTimer->timerType == t1) || (pTimer->timerType == t2))
    {
      pTimerTemp = pTimer->next;
      GARP_TRACE_TIMER_EVENTS("Deleteing in deactivate timer %x type  %d vid(%d) port(%d) expiry(%d) \n",
                                    pTimer,pTimer->timerType,pTimer->vid,pTimer->port_no,pTimer->expiryTime);
      (void)garpTimerFree(pTimer);
      pTimer = pTimerTemp;

    }else
    {
        /* error should not get here*/
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_GARP_COMPONENT_ID, "Invalid value of timer type %d", pTimer->timerType);
        break;

    }
  }


    return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose  osapi timer callback function for GARP
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
void garpTimerCallback(void)
{
  GarpPacket    msg;
  L7_BOOL       restart_timer;

  restart_timer  = L7_FALSE;

  /* If either GARP application is enabled, restart the timer.
     Else, if the pending or popped list is not empty, restart the timer */


   if ((GARP_GVRP_IS_ENABLED == L7_TRUE) || (GARP_GMRP_IS_ENABLED == L7_TRUE))
   {
     restart_timer  = L7_TRUE;
   }
   else if (  (garpTimers.garpTimersPendingList.head != L7_NULL) ||
              (garpTimers.garpTimersPoppedList.head  != L7_NULL) )
   {
       restart_timer  = L7_TRUE;
   }
   else
   {
       restart_timer  = L7_FALSE;
       pGarpTimerHolder = L7_NULLPTR;
       GARP_TRACE_TIMER_EVENTS("%s:  Stopping timers\n", __FUNCTION__);
   }

   /* Debug:  Count the number of times the timer expires while garp is disabled */
   if ((GARP_GVRP_IS_ENABLED == L7_FALSE) && (GARP_GMRP_IS_ENABLED == L7_FALSE))
   {
       garpInfo.infoCounters.timerExpiriesAfterGARPDisable++;
   }

   /* Always send a timer expiry message */
   memset((void *)&msg, 0, sizeof(msg));
   msg.msgId = G_TIMER;

   garpTraceQueueMsgSend(&msg);
   /* Callbacks from the timer are sent as L7_NO_WAIT to avoid backing up the osapiTimerTask
      if the application has an issue.  However, restart the timer if there is a problem
      to ensure that the application services any pending timeouts when it can */
   if (osapiMessageSend(garpQueue, (void*)&msg, sizeof(msg),L7_NO_WAIT, L7_MSG_PRIORITY_NORM)
       != L7_SUCCESS)
   {
       L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_GARP_COMPONENT_ID,
           "garpTimerCallback(): unable to send message to GARP queue. "
           "The garpQueue is full, logs specifics of the message"
           " content like internal interface number, type of message etc");
       restart_timer  = L7_TRUE;
   }
   else
   {
       osapiSemaGive(garpInfo.msgQSema);
   }

   if (restart_timer == L7_TRUE)
   {
       osapiTimerAdd ( (void*)garpTimerCallback,L7_NULL,L7_NULL,
                       GARP_TIMER_GRANULARITY_SHORT,&pGarpTimerHolder );
   }
   else
   {
       /* No timer is running */
       garpInfo.timersRunning = L7_FALSE;
   }


}



/*********************************************************************
* @purpose  Prints the timer list provided
*
*
* @param    *tempList       @b{{input}} pointer to the timer list
* @param    limit           @b{{input}} Max num entries to print
*
* @returns  NONE
*
*
*
* @notes
*
*
* @end
*********************************************************************/
void garpDebugTimerListPrint(garpTimerList_t *tempList, L7_uint32 limit)
{
    garpTimerDescr_t *pTimer;
    L7_uint32 i =0;


    if (limit == 0)
    {
        limit = 100;
    }

    pTimer = tempList->head;
    if (pTimer == L7_NULLPTR)
    {
      sysapiPrintf("tempList->head is NULL\n");
    }
    sysapiPrintf(" %p tempList->head value 0x%x\n", tempList, tempList->head );
    while ((pTimer) && (i <= limit))
    {
        sysapiPrintf("Timer type(%d)", pTimer->timerType);
        sysapiPrintf (" vid(%d)",pTimer->vid);
        sysapiPrintf (" port_no(%d) \n",pTimer->port_no) ;
        sysapiPrintf( "Expire in (%d) ", pTimer->expiryTime);
        sysapiPrintf(" status %d \n", pTimer->timer_status);

        i++;
        pTimer = pTimer->next;
    }

    sysapiPrintf("total number of elements printed %d \n", i);
    sysapiPrintf("total number of elements in list %d \n", tempList->listCount);

}

garpTimerDescr_t timerList[20];

/*********************************************************************
* @purpose  Unit test for the timer list created
*
*
* @param    action          @b{{input}} 1-Add 2 - deleteFirst 3->delete 4 print pending 5 print popped
* @param    numEntries      @b{{input}} Entries to add for action add entry index to
*                                       delete for action delete, entries to print for rest
*
* @returns  NONE
*
*
* @notes
*
*
* @end
*********************************************************************/
void garpDebugUnitTestGarpTimerList(L7_uint32 action,L7_uint32 numEntries)
{
    garpTimerDescr_t *pTimer = L7_NULL;
    garpTimerList_t *tempList = &garpTimers.garpTimersPendingList;
    L7_uint32 milliseconds;
    L7_uint32 deleteIndex;
    static int index = 0;

    srand(osapiTimeMillisecondsGet());
     switch (action)
     {
        case 1:
            while (numEntries != 0 )
            {

                milliseconds = rand();

                pTimer = &timerList[index % 20];
                index++;

                memset(pTimer, 0, sizeof(pTimer));

                pTimer->timerType    = 4;
                pTimer->vid          = 1;
                pTimer->port_no      = 21;
                pTimer->expiryTime   = milliseconds + osapiTimeMillisecondsGet64();
                pTimer->timer_status = GARP_TIMER_PENDING;


                sysapiPrintf("Adding timer with expiry time %d index %d \n", pTimer->expiryTime, index);


                garpTimerListAddElement(pTimer,tempList);
                numEntries--;
            }
            break;
        case 2:
            garpTimerListDeleteFirst(tempList);
            break;
        case 3:
            if (numEntries == 0)
            {
                deleteIndex = index %20 ;
            }
            else
                deleteIndex = numEntries;
            pTimer=&timerList[deleteIndex];
            sysapiPrintf("Deleting timer with expiry time %d idelete index %d \n", pTimer->expiryTime, deleteIndex);
            index--;
            garpTimerListDeleteElement(pTimer,tempList);
            break;
        case 4:
            garpDebugTimerListPrint(tempList,numEntries);
            break;
        case 5:
            garpDebugTimerListPrint(&garpTimers.garpTimersPoppedList ,numEntries);
            break;
        default:
            sysapiPrintf("Invalid action: 1-Add 2 - deleteFirst 3->delete 4 print pending 5 print popped \n");
            break;

     }

}


