/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   async.c
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
#include "log.h"



#define ASYNC_MAX_SEQ_NO 0xFFFFFFFFUL


/*********************************************************************
* @purpose  Timer expiration handler for an asynchronous event
*
* @param    handlerID     @b{(input)}handlerID passed from create call
* @param    correlator    @b{(input)} correlator ( ASYNC_CORRELATOR_t handle)
*                                     passed during create
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    This starts synchronization process for re
*           When used in conjunction with notifying registered routines of events,
*           invoke just before the notification of routines. Complete the handling
*           in timeout functions.
*
*       
* @end             PRIVATE
*********************************************************************/
void asyncEventSyncTimerExpire(ASYNC_EVENT_HANDLER_t handlerId,
                               ASYNC_CORRELATOR_t correlator)
{
  asyncEventHandlerDescr_t *pHandlerEntry;
  asyncCorrelatorData_t *pCorrelator;
  L7_uchar8 clientList[255];
  L7_uint32 i;
  L7_uint32 seqno = (L7_uint32) correlator;

  if (handlerId == L7_NULL) 
  {
     return;
  }

  pHandlerEntry = (asyncEventHandlerDescr_t *)handlerId;

  osapiSemaTake(pHandlerEntry->correlatorLock, L7_WAIT_FOREVER);
  pCorrelator = asyncEventCorrelatorFind(pHandlerEntry, seqno);
  if (pCorrelator == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, pHandlerEntry->handler.componentId,
            "Failed to find correlator with seqno %u for expired async event from %s.",
            seqno, pHandlerEntry->handler.name);
    osapiSemaGive(pHandlerEntry->correlatorLock);
    return;
  }
            
  if (pCorrelator->inUse != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, pHandlerEntry->handler.componentId,
            "For async event handler %s, correlator with seqno %u for expired async event is not in use.",
            pHandlerEntry->handler.name, seqno);
    osapiSemaGive(pHandlerEntry->correlatorLock);
    return;
  }


  clientList[0] = '\0';
  for (i = 1; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (COMPONENT_ISMASKBITSET(pCorrelator->remainingMask, i) != 0)
    {
      L7_uchar8 buf[32];
      osapiSnprintf(buf, 32, "%u ", i);
      strcat(clientList, buf);
    }
  }
  L7_LOGF(L7_LOG_SEVERITY_ERROR, pHandlerEntry->handler.componentId,
          "Async event from %s timed out. No response from the following event clients: %s.",
          pHandlerEntry->handler.name, clientList);


  /* Fill in the completion data */
  memset(&(pCorrelator->completeData), 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t) );
  pCorrelator->completeData.handlerId             = handlerId;
  pCorrelator->completeData.correlator            = correlator;
  pCorrelator->completeData.async_rc.rc            = L7_FAILURE;
  pCorrelator->completeData.async_rc.reason        = ASYNC_EVENT_REASON_RC_TIMEOUT;
         
  pCorrelator->timerInUse = L7_FALSE;

  osapiSemaGive(pHandlerEntry->correlatorLock);

  /* Perform the notification to the event owner */
  (void)pHandlerEntry->handler.complete_notify((ASYNC_EVENT_COMPLETE_INFO_t *)&(pCorrelator->completeData),
                                               (COMPONENT_MASK_t *)&(pCorrelator->remainingMask), 
                                               (COMPONENT_MASK_t *)&(pCorrelator->failingMask));
}

/*********************************************************************
* @purpose  Create an AVL tree to store the mapping from event 
*           sequence number to correlator.
*
* @param    ehd     @b{(input)}  event handler descriptor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t asyncEventCorrelatorMapCreate(asyncEventHandlerDescr_t *ehd)
{
  asyncEventHandler_t *eventHandler = &ehd->handler;
  L7_uint32 treeHeapSize, dataNodeSize, dataHeapSize;
  asyncEventCorrMap_t *correlatorMap = &ehd->correlatorMap;


  /* calculate the amount of memory needed... */
  treeHeapSize = eventHandler->maxCorrelators * (L7_uint32)sizeof(avlTreeTables_t);
  dataNodeSize = (L7_uint32)sizeof(asyncEventCorrMapEntry_t);
  dataHeapSize = eventHandler->maxCorrelators * dataNodeSize;

  /* ...and allocate it from the system heap */
  correlatorMap->treeHeapSize = treeHeapSize;
  correlatorMap->treeHeap = osapiMalloc(ehd->handler.componentId, treeHeapSize);
  if (correlatorMap->treeHeap == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, ehd->handler.componentId,
            "Unable to allocate tree heap for async event correlator tree for %s.",
            ehd->handler.name);
    return L7_FAILURE;
  }

  correlatorMap->dataHeapSize = dataHeapSize;
  correlatorMap->dataHeap = osapiMalloc(ehd->handler.componentId, dataHeapSize);
  if (correlatorMap->dataHeap == L7_NULLPTR)
  {
    osapiFree(ehd->handler.componentId, correlatorMap->treeHeap);
    correlatorMap->treeHeap = L7_NULLPTR;
    L7_LOGF(L7_LOG_SEVERITY_ALERT, ehd->handler.componentId,
            "Unable to allocate data heap for async event correlator tree for %s.",
            ehd->handler.name);
    return L7_FAILURE;
  }

  memset(correlatorMap->treeHeap, 0, (size_t)treeHeapSize);
  memset(correlatorMap->dataHeap, 0, (size_t)dataHeapSize);
  memset(&correlatorMap->avlTree, 0, sizeof(correlatorMap->avlTree));

  avlCreateAvlTree(&correlatorMap->avlTree, correlatorMap->treeHeap, 
                   correlatorMap->dataHeap, 
                   eventHandler->maxCorrelators, dataNodeSize, 0x10, 
                   (L7_uint32)(sizeof(L7_uint32)));
  (void)avlSetAvlTreeComparator(&correlatorMap->avlTree, avlCompareULong32);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the event sequence number to correlator map.
*
* @param    ehd     @b{(input)}  event handler descriptor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t asyncEventCorrelatorMapDelete(asyncEventHandlerDescr_t *ehd)
{
  asyncEventCorrMap_t *correlatorMap = &ehd->correlatorMap;


  /* remove all entries from the AVL tree */
  avlPurgeAvlTree(&correlatorMap->avlTree, ehd->handler.maxCorrelators); 

  /* NOTE:  The AVL utility currently does not have a deletion function,
   *        so the semaphore needs to be deleted manually here.
   */
  if (correlatorMap->avlTree.semId != L7_NULLPTR)
  {
    if (osapiSemaDelete(correlatorMap->avlTree.semId) == L7_SUCCESS)
    {
      correlatorMap->avlTree.semId = L7_NULLPTR;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, ehd->handler.componentId,
              "Could not delete AVL tree semaphore for async event handler %s.",
              ehd->handler.name);
      /* keep going */
    }
  }

  /* free the data heap memory */
  if (correlatorMap->dataHeap != L7_NULLPTR)
  {
    osapiFree(ehd->handler.componentId, correlatorMap->dataHeap);
    correlatorMap->dataHeap = L7_NULLPTR;
  }

  /* free the tree heap memory */
  if (correlatorMap->treeHeap != L7_NULLPTR)
  {
    osapiFree(ehd->handler.componentId, correlatorMap->treeHeap);
    correlatorMap->treeHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next sequence number for a new event for a given event handler.
*
* @param    ehd     @b{(input)}  event handler descriptor
*
* @returns  next sequence number
*
* @notes    Takes care of 32 bit integer wrap
*       
* @end             
*********************************************************************/
L7_uint32 asyncEventNextSequenceNo(asyncEventHandlerDescr_t *ehd)
{
  if (ehd->nextSeqNo == ASYNC_MAX_SEQ_NO)
  {
    ehd->nextSeqNo = 0;
    return 0;
  }
  ehd->nextSeqNo++;
  return ehd->nextSeqNo;
}


/*********************************************************************
* @purpose  Create a mapping from an event sequence number to a 
*           correlator array index.
*
* @param    ehd            @b{(input)} event handler descriptor
* @param    seqno          @b{(input)} event sequence number
* @param    corrIndex      @b{(input)} correlator array index
*
* @returns  L7_SUCCESS if mapping created
*           L7_FAILURE otherwise
*
* @notes    
*       
* @end             
*********************************************************************/
L7_RC_t asyncEventCorrMapInsert(asyncEventHandlerDescr_t *ehd,
                                L7_uint32 seqno, L7_uint32 corrIndex)
{
  asyncEventCorrMapEntry_t mapEntry, *pNode;

  mapEntry.seqno = seqno;
  mapEntry.corrIndex = corrIndex;
  mapEntry.next = NULL;

  pNode = avlInsertEntry(&ehd->correlatorMap.avlTree, (void*) &mapEntry);
  if (pNode != NULL)
  {
    if (pNode != &mapEntry)
    {
      /* mapping with this sequence number already exists */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, ehd->handler.componentId,
              "Tried to insert a duplicate correlator map entry for sequence number %u "
              "for async event handler %s.", seqno, ehd->handler.name);
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, ehd->handler.componentId,
              "Failed to insert a correlator map entry for sequence number %u "
              "for async event handler %s.", seqno, ehd->handler.name);
    }
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete a mapping from the sequence number to  
*           correlator array index table.
*
* @param    ehd            @b{(input)} event handler descriptor
* @param    seqno          @b{(input)} event sequence number
*
* @returns  L7_SUCCESS if mapping created
*           L7_FAILURE otherwise
*
* @notes    
*       
* @end             
*********************************************************************/
L7_RC_t asyncEventCorrMapRemove(asyncEventHandlerDescr_t *ehd,
                                L7_uint32 seqno)
{
  asyncEventCorrMapEntry_t mapEntry, *pNode;

  /* Find the node to delete */
  mapEntry.seqno = seqno;
  mapEntry.corrIndex = 0;   /* not used */
  mapEntry.next = NULL;
  pNode = avlSearchLVL7(&ehd->correlatorMap.avlTree, &mapEntry, L7_MATCH_EXACT);
  if (pNode == NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, ehd->handler.componentId,
            "Failed to find and delete a correlator map entry for sequence number %u "
            "for async event handler %s.", seqno, ehd->handler.name);
    return L7_FAILURE;
  }

  /* Delete the entry */
  if (avlDeleteEntry(&ehd->correlatorMap.avlTree, pNode) != pNode)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, ehd->handler.componentId,
            "Failed to delete a correlator map entry for sequence number %u "
            "for async event handler %s.", seqno, ehd->handler.name);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Given an async event sequence number, get the corresponding
*           correlator entry.
*
* @param    ehd            @b{(input)} event handler descriptor
* @param    seqno          @b{(input)} event sequence number
*
* @returns  correlator or NULL
*
* @notes    assumes handler's correlator lock is taken
*       
* @end             
*********************************************************************/
asyncCorrelatorData_t *asyncEventCorrelatorFind(asyncEventHandlerDescr_t *ehd,
                                                L7_uint32 seqno)
{
  asyncEventCorrMapEntry_t mapEntry, *pNode;

  /* Find the node in the AVL tree */
  mapEntry.seqno = seqno;
  mapEntry.corrIndex = 0;
  mapEntry.next = NULL;
  pNode = avlSearchLVL7(&ehd->correlatorMap.avlTree, &mapEntry, L7_MATCH_EXACT);

  if (pNode == NULL)
  {
    return NULL;
  }

  return &ehd->pCorrelators[pNode->corrIndex];
} 

/*********************************************************************
*
* @purpose  Delete a correlator for an event
*
* @param    ehd     @b{(input)} event handler descriptor
* @param    seqno   @b{(input)} event sequence number
*                                    
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE 
*
* @notes   Correlators are automatically deleted from asyncEventSyncFinish() 
*                                       
* @end
*********************************************************************/
void asyncEventCorrelatorDelete(asyncEventHandlerDescr_t *ehd,
                                L7_uint32 seqno)
{
  asyncCorrelatorData_t *pCorrelator;
  
  if (ehd == L7_NULLPTR)
  {
     return;
  }
  (void)osapiSemaTake(ehd->correlatorLock ,L7_WAIT_FOREVER);
  pCorrelator = asyncEventCorrelatorFind(ehd, seqno);
  if (pCorrelator == L7_NULLPTR)
  {
    osapiSemaGive(ehd->correlatorLock);
    return;
  }

  if (pCorrelator->inUse != L7_TRUE) 
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, ehd->handler.componentId,
              "Async event handler %s attempting to delete a correlator %u which is not in use",
              ehd->handler.name, seqno);

      osapiSemaGive(ehd->correlatorLock);
      return;
  }


  /* Purposely not bzeroing the information during deletion as it
     will be bzeroed during creation.  In this manner, debugging information
     may be left around even after the event has passed. */
  pCorrelator->inUse  = L7_FALSE;
  ehd->correlatorsInUse--;

  asyncEventCorrMapRemove(ehd, seqno);
  
  osapiSemaGive(ehd->correlatorLock);

  return;
}

