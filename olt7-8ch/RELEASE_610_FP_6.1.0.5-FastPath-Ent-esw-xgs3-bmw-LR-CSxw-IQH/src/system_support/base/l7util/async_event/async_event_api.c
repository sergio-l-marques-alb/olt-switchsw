/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   async_event_api.c
*
* @purpose    Asynchronous event handler utility
*
* @component  async
*
* @comments   none
*
* @create    07/30/2003
*
* @author    wjacobs
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "component_mask.h"
#include "async_event_api.h"
#include "async_event.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "log.h"          

static asyncEventHandlerDescr_t   *pAsyncHandlerDescr = L7_NULL;


/*********************************************************************
* @purpose  Initialize the Event Handler Resources
*          
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t asyncEventHandlerInit()
{
    L7_uint32 num_bytes;

    

    num_bytes =sizeof(asyncEventHandlerDescr_t) * (L7_ASYNC_EVENT_MAX_HANDLERS);

    pAsyncHandlerDescr = osapiMalloc(L7_SIM_COMPONENT_ID, num_bytes);

    if (pAsyncHandlerDescr == L7_NULL) 
    {
        LOG_MSG("Unable to initialize async event handler utility\n");
        L7_assert(1);
        return L7_FAILURE;
    }


    memset((void*)pAsyncHandlerDescr,0,
           ( sizeof(asyncEventHandlerDescr_t) * L7_ASYNC_EVENT_MAX_HANDLERS) );


    return L7_SUCCESS;
}



/*********************************************************************
* @purpose  De-initialize the Event Handler Resources
*          
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t asyncEventHdlrFini()
{
   osapiFree(L7_SIM_COMPONENT_ID, pAsyncHandlerDescr);

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create an asynchronous event handler 
*
* @param    pHandler    @b{(input)}pointer to asyncEventHandler_t structure
* @param    handlerId  @b{(output)}identifier to be stored by the caller
*
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @notes    This routine will also create the rwlock to be stored at the address
*           passed in the handler structure
*       
* @end
*********************************************************************/
L7_RC_t asyncEventHandlerCreate(asyncEventHandler_t *pHandler, 
                                ASYNC_EVENT_HANDLER_t *handlerId )
{

   L7_uint32 i;


   *handlerId = L7_NULL;


    if ( (pHandler == L7_NULLPTR) || (handlerId == L7_NULL) )
    {
       return L7_FAILURE;
    }

    for (i=0; i < L7_ASYNC_EVENT_MAX_HANDLERS; i++) 
    {
        if (pAsyncHandlerDescr[i].inUse == L7_FALSE) 
        {
            memset((void*)&(pAsyncHandlerDescr[i]),0,sizeof(asyncEventHandlerDescr_t));
            pAsyncHandlerDescr[i].inUse     = L7_TRUE;
            break;
        }

     }

    if (i == L7_ASYNC_EVENT_MAX_HANDLERS)
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, pHandler->componentId,
                "Failed to create event handler for %s.", pHandler->name);
        return L7_FAILURE;
    }

    pHandler->registered_rwlock = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

    /* configure the event handler */
    memcpy( (void *) &(pAsyncHandlerDescr[i].handler), pHandler, sizeof(asyncEventHandler_t) );

    
    pAsyncHandlerDescr[i].pCorrelators = 
            osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(asyncCorrelatorData_t) * (pHandler->maxCorrelators));
                                         
    memset((void*)pAsyncHandlerDescr[i].pCorrelators,0,
            sizeof(asyncCorrelatorData_t)* (pHandler->maxCorrelators));
                     
    pAsyncHandlerDescr[i].correlatorLock = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (pAsyncHandlerDescr[i].correlatorLock == L7_NULL)
    {

      L7_LOGF(L7_LOG_SEVERITY_ALERT, pHandler->componentId,
              "Unable to create correlator semaphore for async event handler %s.",
              pHandler->name);

      osapiFree(L7_SIM_COMPONENT_ID, pAsyncHandlerDescr[i].pCorrelators);

      return(L7_FAILURE);
    }

    /* Create sequence number to correlator index map */
    if (asyncEventCorrelatorMapCreate(&pAsyncHandlerDescr[i]) != L7_SUCCESS)
    {
      L7_LOGNC(L7_LOG_SEVERITY_ALERT, 
                "Failed to create event sequence number to correlator map.");
      osapiFree(L7_SIM_COMPONENT_ID, pAsyncHandlerDescr[i].pCorrelators);
      return L7_FAILURE;
    }

    *handlerId = (ASYNC_EVENT_HANDLER_t)&(pAsyncHandlerDescr[i]) ;

    return L7_SUCCESS;
}

  
/*********************************************************************
* @purpose  Delete an asynchronous event handler 
*
* @param    handlerID    @b{(input)}handlerID passed from create call
*
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
*       
* @end
*********************************************************************/
L7_RC_t asyncEventHandlerDelete(ASYNC_EVENT_HANDLER_t handlerId)
{

   asyncEventHandlerDescr_t *pHandlerEntry;
   L7_RC_t rc;

   rc = L7_SUCCESS;
                       
   
   if (handlerId == L7_NULL)
   {
      return L7_FAILURE;
   }

   pHandlerEntry = (asyncEventHandlerDescr_t *)handlerId;

   /* Delete sequence number to correlator index map */
   if (asyncEventCorrelatorMapDelete(pHandlerEntry) != L7_SUCCESS)
   {
     L7_LOGNC(L7_LOG_SEVERITY_ALERT, 
              "Failed to delete event sequence number to correlator map.");
     rc = L7_FAILURE;
   }

    if (pHandlerEntry->inUse == L7_TRUE) 
    {   
       osapiFree( L7_SIM_COMPONENT_ID, pHandlerEntry->pCorrelators);

        if (osapiSemaDelete(pHandlerEntry->handler.registered_rwlock) != L7_SUCCESS )
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, pHandlerEntry->handler.componentId,
                    "Unable to delete event handler rwlock for component async handler %s.",
                     pHandlerEntry->handler.name);
        }


        if (osapiSemaDelete (pHandlerEntry->correlatorLock ) != L7_SUCCESS)
        {

            L7_LOGF(L7_LOG_SEVERITY_ERROR, pHandlerEntry->handler.componentId,
                    "Unable to delete correlator semaphore for component async handler %s",
                    pHandlerEntry->handler.name);
        }

        pHandlerEntry->inUse = L7_FALSE;
    }
    else
    {
        rc = L7_FAILURE;
    }

    return rc;
}


/*********************************************************************
* @purpose  Start an asynchronous event
*
* @param    handlerID    @b{(input)} handlerID passed from create call
* @param    correlator   @b{(input/output)}  correlator sequence number  
* @param    pEventInfo   @b{(input)} Pointer to event info to be associated with the correlator
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes   Caller must create a correlator prior to this call and pass 
*          its sequence number as the second argument.
*       
* @end
*********************************************************************/
L7_RC_t asyncEventSyncStart(ASYNC_EVENT_HANDLER_t handlerId,
                            ASYNC_CORRELATOR_t correlator,
                            void *pEventInfo)
{
  L7_BOOL   bits_in_mask;
  /* reference to correlator is now its sequence number. */
  L7_uint32 seqno = (L7_uint32) correlator;
  asyncEventHandlerDescr_t *pHandlerEntry;
  asyncCorrelatorData_t *pCorrelator;
  
  if (handlerId == L7_NULL) 
  {
     return L7_FAILURE;
  }

  pHandlerEntry = (asyncEventHandlerDescr_t *)handlerId;
          
  (void) osapiSemaTake(pHandlerEntry->correlatorLock, L7_WAIT_FOREVER);
  pCorrelator = asyncEventCorrelatorFind(pHandlerEntry, seqno);
  if (pCorrelator == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, pHandlerEntry->handler.componentId,
            "Failed to find correlator to start event from %s.", 
            pHandlerEntry->handler.name);
    osapiSemaGive(pHandlerEntry->correlatorLock);
    return L7_FAILURE;
  }
  
  pCorrelator->pData = pEventInfo;

  /* Initialize the list of components which are yet to respond. */
  memcpy( (void *)&(pCorrelator->remainingMask),
          (void *)(pHandlerEntry->handler.registeredMask), 
          sizeof(COMPONENT_MASK_t) );


  /*  The "no wait" option is provided for completeness. Its use would event the tally to a
      simple signal, but this would be a heavy handed usage of this event  */
  if (pHandlerEntry->handler.timeout == L7_NO_WAIT)
  {
      /* order of these two calls is intentional */
      osapiSemaGive(pHandlerEntry->correlatorLock);
      asyncEventSyncTimerExpire(handlerId, correlator); 
  }
  else if (pHandlerEntry->handler.timeout != L7_WAIT_FOREVER) 
  {
      /*  Return completion message immediately if there is no registered user */
      bits_in_mask = L7_FALSE;

      /* Mark responding components */
      COMPONENT_NONZEROMASK(pCorrelator->remainingMask, bits_in_mask);
      if (bits_in_mask == L7_TRUE) 
      {
          /* At least one registered user -  Start timer */
          pCorrelator->timerInUse = L7_TRUE;

          /* Start the timer for registered components respond */
          osapiTimerAdd( (void*)asyncEventSyncTimerExpire, handlerId, (L7_uint32) correlator, 
                         pHandlerEntry->handler.timeout, &pCorrelator->timer);

          osapiSemaGive(pHandlerEntry->correlatorLock);
      }
      else
      {
          /* No registered users. Force completion */
          memset( (L7_char8 *)&(pCorrelator->completeData), 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
          pCorrelator->completeData.handlerId       = handlerId;
          pCorrelator->completeData.correlator      = correlator;
          pCorrelator->completeData.async_rc.rc     = L7_SUCCESS;
          pCorrelator->completeData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;

          /* Must give this semaphore before complete_notify() call, because complete_notify()
           * may call an async event API which itself needs the lock. */
          osapiSemaGive(pHandlerEntry->correlatorLock);

          /* Perform the notification to the event owner */
          (void)pHandlerEntry->handler.complete_notify((ASYNC_EVENT_COMPLETE_INFO_t *)&(pCorrelator->completeData),
                                                       (COMPONENT_MASK_t *)&(pCorrelator->remainingMask),
                                                       (COMPONENT_MASK_t *)&(pCorrelator->failingMask));
      }
  }                                                    

  return L7_SUCCESS;
}
                               

/*********************************************************************
* @purpose  Finish an asynchronous event
*
* @param    handlerID    @b{(input)}handlerID passed from create call
* @param    correlator    @b{(input)} correlator ( ASYNC_CORRELATOR_t handle)
*                                     passed during create
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes   The correlator associated with the event will be deleted.
*
*       
* @end
*********************************************************************/
void asyncEventSyncFinish(ASYNC_EVENT_HANDLER_t handlerId,
                          ASYNC_CORRELATOR_t correlator)
{
  L7_RC_t   rc;
  L7_uint32 seqno = (L7_uint32) correlator;
  asyncEventHandlerDescr_t *pHandlerEntry;
  asyncCorrelatorData_t *pCorrelator;

  rc = L7_SUCCESS;

  pHandlerEntry = (asyncEventHandlerDescr_t *)handlerId;
  if (pHandlerEntry == L7_NULLPTR)
  {
     return;
  }
  (void)osapiSemaTake(pHandlerEntry->correlatorLock, L7_WAIT_FOREVER);
  pCorrelator = asyncEventCorrelatorFind(pHandlerEntry, seqno);
  if (pCorrelator == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, pHandlerEntry->handler.componentId,
            "Failed to find correlator for async event from %s.",
            pHandlerEntry->handler.name);
    osapiSemaGive(pHandlerEntry->correlatorLock);
    return;
  }
 
  if ( (pHandlerEntry->handler.timeout != L7_WAIT_FOREVER)  &&  
       (pHandlerEntry->handler.timeout != L7_NO_WAIT) )
  {
   if (pCorrelator->timerInUse == L7_TRUE)
   {
     /* Turn off the timer */
       osapiTimerFree(pCorrelator->timer);
       pCorrelator->timerInUse = L7_FALSE;
   }
  }

  osapiSemaGive(pHandlerEntry->correlatorLock);

  asyncEventCorrelatorDelete(pHandlerEntry, seqno);

  /* jpp: caller unlocks rwlock */
}

/*********************************************************************
*
* @purpose  Tally registered users completion for an asynchronous event
*
* @param    pComplete  @b{(input)}  pointer to ASYNC_EVENT_COMPLETE_INFO_t structure
*
* @returns  L7_SUCCESS if response tallied
*           L7_FAILURE if response was invalid
*           L7_ERROR if parameters were bad    
*
* @notes    At the conclusion of processing an event, each component must
*           must call this function 
*          
* @end
*********************************************************************/
L7_RC_t asyncEventCompleteTally(ASYNC_EVENT_COMPLETE_INFO_t *pComplete)
{
  L7_BOOL   done;
  L7_BOOL   bits_in_mask;
  L7_BOOL   failures;
  
  asyncEventHandlerDescr_t *pHandlerEntry;
  asyncCorrelatorData_t *pCorrelator;
                             
  pHandlerEntry = (asyncEventHandlerDescr_t *)pComplete->handlerId;
  if (pHandlerEntry == NULL)
  {
    return L7_ERROR;
  }

  (void)osapiSemaTake(pHandlerEntry->correlatorLock, L7_WAIT_FOREVER);

  pCorrelator = asyncEventCorrelatorFind(pHandlerEntry, pComplete->correlator);
  if (pCorrelator == L7_NULLPTR)
  {
    osapiSemaGive(pHandlerEntry->correlatorLock);
    L7_LOGFNC(L7_LOG_SEVERITY_ERROR,
              "Client %d of async event handler %s responded to an invalid event "
              "with event sequence number %u.",
              pComplete->componentId, pHandlerEntry->handler.name, pComplete->correlator);
    return L7_FAILURE;
  }
      
  
  done          = L7_FALSE;
  failures      = L7_FALSE;
  bits_in_mask  = L7_FALSE;

  /* This check is now somewhat redundant with asyncEventCorrelatorFind(). If we've 
   * coded this correctly, Find() should never find a correlator that's not "in use."
   * But the double check is cheap. Belt and suspenders, as Jeff R. says. */
  if (!pCorrelator->inUse)
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, pHandlerEntry->handler.componentId,
              "Found an idle correlator when handler %s processed event %u from client %u.",
              pHandlerEntry->handler.name, pComplete->correlator, pComplete->componentId);
      osapiSemaGive(pHandlerEntry->correlatorLock);
      return L7_FAILURE;
  }
  
  /* Check if the component has already responded. This has been observed
   * when an event times out and the component subsequently responds. The
   * response may be incorrectly correlated to a later event, for which the
   * component may also send a response. 
   */
  if (COMPONENT_ISMASKBITSET(pCorrelator->remainingMask, 
                             pComplete->componentId) == 0)
  {
      L7_LOGFNC(L7_LOG_SEVERITY_ERROR,
                "Async event client %d of handler %s unexpectedly signalled event %u completion.",
                pComplete->componentId, pHandlerEntry->handler.name, pComplete->correlator);
      osapiSemaGive(pHandlerEntry->correlatorLock);
      return L7_FAILURE;
  }

  /* Mark responding components */
  COMPONENT_CLRMASKBIT(pCorrelator->remainingMask, pComplete->componentId);
  COMPONENT_NONZEROMASK(pCorrelator->remainingMask, bits_in_mask);
  if (bits_in_mask == L7_FALSE) 
  {
      done = L7_TRUE;
  }

  /* Mark components with unsuccessful returns */
  if (pComplete->async_rc.rc != L7_SUCCESS) 
  {
      COMPONENT_SETMASKBIT(pCorrelator->failingMask, pComplete->componentId);
  }
  if (done == L7_TRUE)
  {
      if ( (pHandlerEntry->handler.timeout != L7_WAIT_FOREVER)  &&  
           (pHandlerEntry->handler.timeout != L7_NO_WAIT) )
      {
          if (pCorrelator->timerInUse == L7_TRUE)
          {
            /* Turn off the timer */
              osapiTimerFree(pCorrelator->timer);
              pCorrelator->timerInUse = L7_FALSE;
          }
      }

      /* Fill in the completion data.  The component ID is effectively null for the final 
         completion */
      memset(&(pCorrelator->completeData), 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t) );
      pCorrelator->completeData.handlerId         = pComplete->handlerId;
      pCorrelator->completeData.correlator        = pComplete->correlator;


      COMPONENT_NONZEROMASK(pCorrelator->failingMask, failures);

      if (failures == L7_TRUE)
      {
          pCorrelator->completeData.async_rc.rc     = L7_FAILURE;
          pCorrelator->completeData.async_rc.reason  = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
      }
      else
      {
          pCorrelator->completeData.async_rc.rc     = L7_SUCCESS;
          pCorrelator->completeData.async_rc.reason  = ASYNC_EVENT_REASON_RC_SUCCESS;
      }
      /* Must give semaphore before notifying event owner. complete_notify() may call back
       * into this library and need to take the correlator lock. */

      osapiSemaGive(pHandlerEntry->correlatorLock);

      /* Perform the notification to the event owner */
      (void)pHandlerEntry->handler.complete_notify((ASYNC_EVENT_COMPLETE_INFO_t *)&(pCorrelator->completeData),
                                                   (COMPONENT_MASK_t *)&(pCorrelator->remainingMask),
                                                   (COMPONENT_MASK_t *)&(pCorrelator->failingMask));

      return L7_SUCCESS;
  }
 
   /* Tally is not complete.  Give semaphore */
   osapiSemaGive(pHandlerEntry->correlatorLock);
   return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Create a correlator for an event
*
* @param    handlerID     @b{(input)}handlerID passed from create call
* @param    pCorrelator   @b{(output)} event sequence number
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*                                   
* @end
*********************************************************************/
L7_RC_t asyncEventCorrelatorCreate(ASYNC_EVENT_HANDLER_t handlerId ,
                                   ASYNC_CORRELATOR_t *pCorrelator)
{
  asyncEventHandlerDescr_t *pHandlerEntry;
  L7_uint32 index;
  L7_uint32 seqno;
  L7_RC_t rc;

  rc = L7_SUCCESS;


  pHandlerEntry =  (asyncEventHandlerDescr_t *)handlerId;

  if ((pHandlerEntry == L7_NULLPTR) || (pCorrelator == L7_NULLPTR))
  {
    L7_LOGFNC(L7_LOG_SEVERITY_ERROR, 
              "Bad parameters to %s.", __FUNCTION__);
    return L7_FAILURE;
  }
                                          
  osapiSemaTake(pHandlerEntry->correlatorLock, L7_WAIT_FOREVER);

   for (index = 0; 
        (index < pHandlerEntry->handler.maxCorrelators) && 
        (pHandlerEntry->pCorrelators[index].inUse);
        index++);

  if (index == pHandlerEntry->handler.maxCorrelators)
  {
    *pCorrelator = 0;
    rc = L7_FAILURE;
  }
  else
  {
    rc = L7_SUCCESS;
    memset((L7_char8 *) &(pHandlerEntry->pCorrelators[index]), 0, sizeof(asyncCorrelatorData_t));
    pHandlerEntry->pCorrelators[index].inUse = L7_TRUE;
    pHandlerEntry->correlatorsInUse++;
    if (pHandlerEntry->correlatorsInUse > pHandlerEntry->correlatorsHighWater)
      pHandlerEntry->correlatorsHighWater = pHandlerEntry->correlatorsInUse;
    seqno = asyncEventNextSequenceNo(pHandlerEntry);
    asyncEventCorrMapInsert(pHandlerEntry, seqno, index);
    *pCorrelator = (ASYNC_CORRELATOR_t)seqno;
  }

  osapiSemaGive(pHandlerEntry->correlatorLock);
  return(rc);
}


/*********************************************************************
* @purpose  Associate data with a specific event correlator
*
* @param    handlerID    @b{(input)}handlerID passed from create call
* @param    correlator   @b{(input)} correlator ( ASYNC_CORRELATOR_t handle)
*                                    passed during create
* @param    *pData      @b{(input)} pointer to data area to be associated
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes   
*
*       
* @end
*********************************************************************/
L7_RC_t asyncEventCorrelatorDataSet(ASYNC_EVENT_HANDLER_t handlerId,
                                    ASYNC_CORRELATOR_t correlator,
                                    void *pData)
{
  asyncEventHandlerDescr_t *pHandlerEntry;
  asyncCorrelatorData_t *pCorrelator;

  pHandlerEntry = (asyncEventHandlerDescr_t *)handlerId;
  if (pHandlerEntry == L7_NULLPTR)
  {
     return L7_FAILURE;
  }
  (void)osapiSemaTake(pHandlerEntry->correlatorLock ,L7_WAIT_FOREVER);
  pCorrelator = asyncEventCorrelatorFind(pHandlerEntry, correlator);
  if (pCorrelator == L7_NULLPTR)
  {
    osapiSemaGive(pHandlerEntry->correlatorLock);
    return L7_FAILURE;
  }

  pCorrelator->pData = pData;

  osapiSemaGive(pHandlerEntry->correlatorLock);
  return L7_SUCCESS;                      
}

/*********************************************************************
* @purpose  Associate data with a specific event correlator
*
* @param    handlerID    @b{(input)}handlerID passed from create call
* @param    correlator   @b{(input)} correlator ( ASYNC_CORRELATOR_t handle)
*                                    passed during create
* @param    *pData      @b{(output)} pointer to data area to be associated
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes   
*
*       
* @end
*********************************************************************/
L7_RC_t asyncEventCorrelatorDataGet(ASYNC_EVENT_HANDLER_t handlerId,
                                    ASYNC_CORRELATOR_t correlator,
                                    void **pData)
{
  L7_RC_t   rc;
  asyncEventHandlerDescr_t *pHandlerEntry;
  asyncCorrelatorData_t *pCorrelator;

  rc = L7_SUCCESS;


  pHandlerEntry = (asyncEventHandlerDescr_t *)handlerId;
  if (pHandlerEntry == L7_NULLPTR)
  {
     return L7_FAILURE;
  }
  (void)osapiSemaTake(pHandlerEntry->correlatorLock ,L7_WAIT_FOREVER);
  pCorrelator = asyncEventCorrelatorFind(pHandlerEntry, correlator);
  if (pCorrelator == L7_NULLPTR)
  {
    osapiSemaGive(pHandlerEntry->correlatorLock);
    return L7_FAILURE;
  }
         
  *pData = pCorrelator->pData;
  
  osapiSemaGive(pHandlerEntry->correlatorLock);

  return L7_SUCCESS;                      
}



/*********************************************************************
*
* @purpose  Create an asynchronous signal 
*
* @param    pSignal  @b{(input)}  pointer to ASYNC_EVENT_SIGNAL_t structure
* @param    flags    @b{(input)}  Valid values defined by ASYNC_EVENT_SIGNAL_FLAG* 
*
* @returns  void     
*
* @notes    
*          
* @end
*********************************************************************/
L7_RC_t asyncEventSignalCreate(asyncEventSignalDescr_t  *pSignal, L7_uint32 flags)
{
    
    if (pSignal == L7_NULLPTR) 
     {
        return L7_FAILURE;
     }


    if (flags & ASYNC_EVENT_SIGNAL_FLAG_SEMAB_EMPTY) 
    {
        pSignal->semaB = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
    }
    else
    {
        pSignal->semaB = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    }

    return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose  Delete an asynchronous signal 
*
* @param    pSignal  @b{(input)}  pointer to ASYNC_EVENT_SIGNAL_t structure
*
* @returns  void     
*
* @notes    
*          
* @end
*********************************************************************/
L7_RC_t asyncEventSignalDelete(asyncEventSignalDescr_t  *pSignal)
{
    
    if (pSignal == L7_NULLPTR) 
     {
        return L7_FAILURE;
     }


    if (pSignal->semaB != L7_NULL)
    {
        return( osapiSemaDelete(pSignal->semaB) );
    }

    return L7_SUCCESS;
}





/*********************************************************************
*
* @purpose  Wait for an asynchronous signal 
*
* @param    pSignal  @b{(input)}  pointer to ASYNC_EVENT_SIGNAL_t structure
*
* @returns  void     
*
* @notes    
*          
* @end
*********************************************************************/
L7_RC_t asyncEventSignalWait(asyncEventSignalDescr_t *pSignal)
{
    if (pSignal == L7_NULLPTR) 
    {
        return L7_FAILURE;
      
    }
  
    if (pSignal->semaB == L7_NULLPTR) 
    {
        return L7_FAILURE;
    }

    (void)osapiSemaTake(pSignal->semaB, L7_WAIT_FOREVER);

    return L7_SUCCESS;
     
}



/*********************************************************************
*
* @purpose  Send an asynchronous signal 
*
* @param    pSignal     @b{(input)}  pointer to ASYNC_EVENT_SIGNAL_t structure
* @param    pResponse   @b{(input)}  pointer to an ASYNC_RESPONSE_t structure
*
* @returns  void     
*
* @notes    The callback function, if any, is invoked for this signal.
*           If the signal is blocked, it is unblocked.
*          
* @end
*********************************************************************/
void asyncEventSignalSend(asyncEventSignalDescr_t *pSignal, ASYNC_RESPONSE_t *pResponse)
{

    if (pSignal->callback_func != L7_NULLPTR) 
    {
        pSignal->callback_func(pResponse, (L7_char8 *)&(pSignal->signalInfo.eventData));
    }

    if (pSignal->semaB != L7_NULLPTR)
    {
        osapiSemaGive(pSignal->semaB) ;
    }
}




/*********************************************************************
*
* @purpose  Print information for debugging purposes.
*
* @param    void
*                                    
* @returns  void
*
* @notes   
*                                       
* @end
*********************************************************************/
void asyncEventDebugInfo(void)
{
  L7_uint32 i;

  for (i = 0; i < L7_ASYNC_EVENT_MAX_HANDLERS; i++) 
  {
    if (pAsyncHandlerDescr[i].inUse) 
    {
      asyncEventHandlerDescr_t *ehd = &pAsyncHandlerDescr[i];
      asyncEventCorrMap_t *corrMap = &ehd->correlatorMap;

      osapiSemaTake(ehd->correlatorLock, L7_WAIT_FOREVER);

      printf("\n\nHandler %d, name %s, component ID %u.",
             i, ehd->handler.name, ehd->handler.componentId);
      printf("\nEvent timeout is %d milliseconds.", ehd->handler.timeout);
      printf("\nCorrelators in use %u, high water %u, next sequence number %u.",
             ehd->correlatorsInUse, ehd->correlatorsHighWater, ehd->nextSeqNo);

      if (avlTreeCount(&corrMap->avlTree) == 0)
      {
        printf("\nNo outstanding events");
      }
      else
      {
        asyncEventCorrMapEntry_t mapEntry, *pNode = NULL;
        asyncCorrelatorData_t *correlator;
        memset((L7_uchar8*) &mapEntry, 0, sizeof(mapEntry));
        while ((pNode = avlSearchLVL7(&corrMap->avlTree, &mapEntry, AVL_NEXT)) != NULL)
        {
          mapEntry.seqno = pNode->seqno;
          mapEntry.corrIndex = pNode->corrIndex;
          correlator = asyncEventCorrelatorFind(ehd, pNode->seqno);
          if (correlator)
          {
            L7_uint32 j;
            L7_uchar8 maskStr[(2 * COMPONENT_INDICES) + 1];
            L7_uchar8 buf[32];
            maskStr[0] = '\0';
            for (j = 0; j < COMPONENT_INDICES; j++)
            {
              sprintf(buf, "%02x", (L7_uint32) correlator->remainingMask.value[j]);
              strcat(maskStr, buf);
            }
            printf("\n Event %u: remainingMask %s",
                   pNode->seqno, maskStr);
          }
        }
      }
      osapiSemaGive(ehd->correlatorLock);
    } 
  }
}

   
/*********************************************************************
*
* @purpose  Check for inconsistencies in async event library data.
*
* @param    void
*                                    
* @returns  L7_SUCCESS if valid. L7_FAILURE if errors.
*
* @notes   
*                                       
* @end
*********************************************************************/
L7_RC_t asyncEventValidate(void)
{
  L7_uint32 i, j;
  L7_uint32 correlatorsInUse = 0;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 treeCount;
  asyncEventCorrMapEntry_t mapEntry, *pNode = NULL;
  asyncCorrelatorData_t *correlator;

  for (i = 0; i < L7_ASYNC_EVENT_MAX_HANDLERS; i++) 
  {
    if (pAsyncHandlerDescr[i].inUse) 
    {
      asyncEventHandlerDescr_t *ehd = &pAsyncHandlerDescr[i];
      asyncEventCorrMap_t *corrMap = &ehd->correlatorMap;

      if (!ehd->pCorrelators)
      {
        printf("\nEvent handler for %s has no correlator array.", 
               ehd->handler.name);
        return L7_FAILURE;
      }

      osapiSemaTake(ehd->correlatorLock, L7_WAIT_FOREVER);

      for (j = 0; j < ehd->handler.maxCorrelators; j++)
      {
        if (ehd->pCorrelators[j].inUse)
        {
          correlatorsInUse++;
        }
      }
      if (correlatorsInUse != ehd->correlatorsInUse)
      {
        printf("\nHandler %s says %u correlators in use, but %u correlators are marked inUse.",
               ehd->handler.name, ehd->correlatorsInUse, correlatorsInUse);
        rc = L7_FAILURE;
      }

      treeCount = avlTreeCount(&corrMap->avlTree);
      if (treeCount != ehd->correlatorsInUse)
      {
        printf("\nHandler %s says %u correlators in use, but %u correlators in tree.",
               ehd->handler.name, ehd->correlatorsInUse, treeCount);
        rc = L7_FAILURE;
      }

      memset((L7_uchar8*) &mapEntry, 0, sizeof(mapEntry));
      while ((pNode = avlSearchLVL7(&corrMap->avlTree, &mapEntry, AVL_NEXT)) != NULL)
      {
        mapEntry.seqno = pNode->seqno;
        mapEntry.corrIndex = pNode->corrIndex;
        correlator = asyncEventCorrelatorFind(ehd, pNode->seqno);
        if (!correlator)
        {
          printf("\nFor handler %s, cannot find correlator for seqno %u.",
                 ehd->handler.name, pNode->seqno);
          rc = L7_FAILURE;
          continue;
        }

        if (!correlator->inUse)
        {
          printf("\nFor handler %s, correlator with seqno %u in tree but not in use.",
                 ehd->handler.name, pNode->seqno);
          rc = L7_FAILURE;
        }

        if (correlator->timerInUse && !correlator->timer)
        {
          printf("\nFor handler %s, correlator with seqno %u has NULL timer.",
                 ehd->handler.name, pNode->seqno);
          rc = L7_FAILURE;
        }
      }
      osapiSemaGive(ehd->correlatorLock);
    } 
  }
  if (rc == L7_SUCCESS)
    printf("\nAsync Event library data passes validation.");
  return rc;
}


