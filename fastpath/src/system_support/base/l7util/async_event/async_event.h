/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename async_event.h
*
* @purpose Asynchronous Event Handler Utility
*
* @component async
*
* @comments none
*
* @create 08/03/2002
*
* @author wjacobs
* @end
*
**********************************************************************/


#ifndef INCLUDE_ASYNC_EVENT_H
#define INCLUDE_ASYNC_ENT_H

#include "l7_common.h"
#include "osapi.h"
#include "default_cnfgr.h"
#include "component_mask.h"
#include "async_event_api.h"

/*---------------------------------------------------------*/
/*                                                         */
/*                                                         */
/*    CONSTANTS and ENUMS                                  */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/



/*---------------------------------------------------------*/
/*                                                         */
/*    TYPEDEFS                                             */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/



/*---------------------------------------------------------*/
/*                                                         */
/*    STRUCTURES                                           */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/



/*---------------------------------------*/
/*  Structures used to correlate events
 */
/*---------------------------------------*/

typedef struct asyncCorrelatorData_s
{

  L7_BOOL                       inUse;
  L7_BOOL                       timerInUse;
  osapiTimerDescr_t             *timer;
  COMPONENT_MASK_t              remainingMask;  /* Mask of components which have yet to respond */
  COMPONENT_MASK_t              failingMask;    /* Mask of components with unsuccessful returns */
  void                          *pData;         /* Meaningful data to event handler */
  ASYNC_EVENT_COMPLETE_INFO_t   completeData;


}asyncCorrelatorData_t;


/* Type used to create an async event handler descriptor */
typedef struct asyncEventHandlerEntry_s
{

  L7_BOOL                   inUse;      /* entry in use */
  asyncEventHandler_t       handler;  /* Handler registered by caller */

  /* an array of correlators. When the handler generates an event, the 
   * handler finds a free element in the arry and uses it to track 
   * event responses. */
  asyncCorrelatorData_t     *pCorrelators;
  L7_uint32                 correlatorsInUse;
  L7_uint32                 correlatorsHighWater;   /* to help adjust max */

  /* Since multiple threads signal event completion, a semaphore to synchronize
   * access to correlator data. */
  void                      *correlatorLock;

  /* Each event is assigned a unique sequence number. Each handler has its own 
   * sequence number space. The handler can map the sequence number to the
   * correlator for the event. A sequence number becomes invalid after the 
   * event has completed, either because all clients responded or because 
   * the event timed out. Thus, if a client responds after the timeout, 
   * the handler can recognize the error and ignore the response. The value 
   * stored here is the next sequence number to be assigned to an event. */
  L7_uint32 nextSeqNo;

  /* AVL tree to map event sequence number to correlator. Tree allows up to 
   * maxCorrelators entries. */
  asyncEventCorrMap_t correlatorMap;

}asyncEventHandlerDescr_t, *asyncEventHandlerTable_t;




/*---------------------------------------------------------*/
/*                                                         */
/*    PROTOTYPES                                           */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/



void asyncEventSyncTimerExpire(ASYNC_EVENT_HANDLER_t handlerId,
                         ASYNC_CORRELATOR_t correlator);
L7_RC_t asyncEventCorrelatorMapCreate(asyncEventHandlerDescr_t *ehd);
L7_RC_t asyncEventCorrelatorMapDelete(asyncEventHandlerDescr_t *ehd);
L7_uint32 asyncEventNextSequenceNo(asyncEventHandlerDescr_t *ehd);
L7_RC_t asyncEventCorrMapInsert(asyncEventHandlerDescr_t *ehd,
                                L7_uint32 seqno, L7_uint32 corrIndex);
L7_RC_t asyncEventCorrMapRemove(asyncEventHandlerDescr_t *ehd,
                                L7_uint32 seqno);
asyncCorrelatorData_t *asyncEventCorrelatorFind(asyncEventHandlerDescr_t *eventHandlerDescr,
                                                L7_uint32 seqno);
void asyncEventCorrelatorDelete(asyncEventHandlerDescr_t *eventHandlerDescr,
                                L7_uint32 seqno);
  

#endif  /* INCLUDE_ASYNC_EVENT_H */

