/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename async_event_api.h
*
* @purpose Asynchronous Event Handler Utility
*
* @component async
*
* @comments none
*
* @create 08/03/2003
*
* @author wjacobs
* @end
*
**********************************************************************/

/* WRR - Notes added October 2007. 
*
*  New uses of this library are not encouraged.
*
*  Purpose of the library is to provide a common facility to manage 
*  responses to asynchronous events. A sender of events can use the
*  library to collect responses from all event clients, know when all
*  clients have responded to an event, and know whether all clients processed
*  the event successfully. Event response collection times out after
*  a specified period of time. 
* 
*  Each sender of events creates an "event handler" using asyncEventHandlerCreate().
*  Some attributes of the event handler (members of asyncEventHandlerDescr_t) are opaque 
*  to the event sender. The event sender sets other attributes on the event 
*  handler (members of asyncEventHandler_t). 
*
*  When an event sender generates an asynchronous event, it creates an "event 
*  correlator." The event correlator uniquely identifies the event and provides
*  the data structures to tabulate event responses. asyncEventCorrelatorCreate()
*  returns an integer which the event sender and event clients use to identify
*  the event. The implementation of the correlator data structures is hidden
*  from the event sender and clients. 
*  
*  Some changes being made to fix the crash reported in defect 67360. 
*  Crash is a result of an async event timing out and an event client
*  subsequently responding late. With the original design, the client had a handle
*  to the actual correlator data element. The correlator could have been 
*  returned to the pool (inUse == false) or even reused by the time the 
*  tardy client responded. This could cause either two notifies to the event
*  sender for the same event, or a premature notify for the unlucky event
*  that reused the correlator. So instead of giving clients a handle to the 
*  correlator data, we stamp each correlator with a sequence number. There is a 
*  new AVL tree that maps correlator sequence numbers to correlator entries. 
*  Each handler has its own sequence number space and seqno map. A late response
*  won't be found in the tree and can be ignored. 
*/

#ifndef INCLUDE_ASYNC_EVENT_API_H
#define INCLUDE_ASYNC_EVENT_API_H

#include "l7_common.h"
#include "component_mask.h"
#include "default_cnfgr.h"
#include "avl_api.h"


/*---------------------------------------------------------*/
/*                                                         */
/*                                                         */
/*    CONSTANTS and ENUMS                                  */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/
  

#define ASYNC_MAX_HANDLER_NAME 16


/*-------------------------*/
/*                         */
/*    ASYNC EVENT ERRNOs   */
/*                         */
/*-------------------------*/
  

/* 
 Type used to indicate various reasons for a failure on 
 Async Event Notifications 
 */

typedef enum
{
  ASYNC_EVENT_REASON_RC_SUCCESS = 0,
  ASYNC_EVENT_REASON_RC_TIMEOUT,
  ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE,
  ASYNC_EVENT_REASON_RC_IGNORED,
  ASYNC_EVENT_REASON_RC_LACK_OF_RESOURCES,
  ASYNC_EVENT_REASON_RC_NOT_AVAILABLE,
  ASYNC_EVENT_REASON_RC_BUSY,
  ASYNC_EVENT_REASON_RC_LAST
} ASYNC_EVENT_REASON_RC_t;



/*---------------------------------------------------------*/
/*                                                         */
/*    TYPEDEFS                                             */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/
  

/*
 * Types used for Correlators and Handles for the event notification mechanism
 */
/* This used to be a pointer to an element in the async library's 
 * correlator array. It is now simply the event sequence number. Retain
 * the naming to avoid changing all references to this. */
typedef L7_uint32 ASYNC_CORRELATOR_t;

typedef L7_uint32  ASYNC_EVENT_HANDLER_t;



/*---------------------------------------------------------*/
/*                                                         */
/*    STRUCTURES FOR EVENTS                                */
/*          Events should be used when multiple            */
/*          respondants must act on the same event.        */
/*                                                         */
/*---------------------------------------------------------*/


/*---------------------------------------*/
/*  Structures passed from notifier to 
    registered recipients.
 */ 
/*---------------------------------------*/

/*
 * Structure used to pass data to registered event recipients
 * to instruct the recipients as to how to return asynchronous
 * completion data.
 *
 */
typedef struct ASYNC_EVENT_DESCR_s
{

  ASYNC_EVENT_HANDLER_t handlerId;      
  ASYNC_CORRELATOR_t    correlator;     

} ASYNC_EVENT_DESCR_t;     



/*---------------------------------------*/
/*  Structures passed back from 
    registered recipients to notifier
    to record event completion status.
 */ 
/*---------------------------------------*/


/* 
 * Structure used to pass async event notification information to a registered recipient
 */


typedef struct ASYNC_EVENT_NOTIFY_INFO_s
{

  ASYNC_EVENT_HANDLER_t handlerId;    /* handlerId passed in event notification */
  ASYNC_CORRELATOR_t    correlator;   /* event correlator passed in event notification */
  void                  *pData;    /* Pointer to  data */ 

} ASYNC_EVENT_NOTIFY_INFO_t;



/* 
 * Structures used to pass async event completion return code ane errno
 * from a registered recipient
 */

typedef struct ASYNC_RESPONSE_s
{
    L7_RC_t                 rc;
    ASYNC_EVENT_REASON_RC_t reason;
} ASYNC_RESPONSE_t;


typedef struct
{

  L7_COMPONENT_IDS_t    componentId;  /* responding component */
  ASYNC_EVENT_HANDLER_t handlerId;    /* handlerId passed in event notification */
  ASYNC_CORRELATOR_t    correlator;   /* event correlator passed in event notification */
  ASYNC_RESPONSE_t      async_rc;

} ASYNC_EVENT_COMPLETE_INFO_t;



/*---------------------------------------*/
/*  Structure used to register an event
    handler.
 */ 
/*---------------------------------------*/

typedef struct asyncEventCorrMapEntry_s
{
  /* NOTE:  The search key MUST be the first item in this structure */
  L7_uint32       seqno;               /* SEARCH KEY: event sequence number */
  L7_uint32       corrIndex;           /* index into correlators array */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the last element */
  void            *next;                /* (RESERVED FOR AVL USE) */
} asyncEventCorrMapEntry_t;

typedef struct asyncEventCorrMap_t
{
  avlTreeTables_t     *treeHeap;
  L7_uint32           treeHeapSize;
  asyncEventCorrMapEntry_t    *dataHeap;
  L7_uint32           dataHeapSize;
  avlTree_t           avlTree;
} asyncEventCorrMap_t;

/* Type used to create an async event handler */
/* User must use a read/write lock around registering/deregistering components */
typedef struct asyncEventHandler_s
{
  /* Name of the event sender. Used in log messages. */
  L7_uchar8         name[ASYNC_MAX_HANDLER_NAME];  

  /* Component ID of the event sender. Used in log messages, memory allocation. */
  L7_COMPONENT_IDS_t  componentId;

  /* The maximum number of events that may be outstanding for this handler. */
  L7_uint32         maxCorrelators;     

  /* L7_NO_WAIT, L7_WAIT_FOREVER, Timeout value in milliseconds */
  L7_int32          timeout;            

  /* routine to call on completion or timeout of the event */	
  void             (*complete_notify)(ASYNC_EVENT_COMPLETE_INFO_t *pComplete, 
                                       COMPONENT_MASK_t *pRemainingMask,
                                       COMPONENT_MASK_t *pFailingMask);

  /* Read write lock used to prevent changes to the set of event clients while
   * a handler is generating an event. */
  void              *registered_rwlock; 

  COMPONENT_MASK_t  *registeredMask;    /* registered components for notification:
                                            Must point to a data area maintained by
                                            owner of the event handler */

}asyncEventHandler_t;




/*---------------------------------------------------------*/
/*                                                         */
/*    STRUCTURES FOR SIGNAL                                */
/*                                                         */
/*          Signals should be used when there is only      */
/*          one point of completion.                       */
/*                                                         */
/*---------------------------------------------------------*/

/*---------------------------------------*/
/*  Structure used to define an
    asynchronous signal.
    
    
    NOTES: 
    
    Unlike events, the data structure describing
    a signal is locally held, managed, and interpreted 
    by the owning components.
    
    The exception is the semaphore structure.
    
    
    
 */ 
/*---------------------------------------*/


typedef struct
{
    void        *semaB;      /* binary semaphore */
    L7_uint32   flags;      /* defined by ASYNC_EVENT_SIGNAL_FLAG* */
    void        (*callback_func)(ASYNC_RESPONSE_t *pResponse, L7_char8 *eventData); 
    ASYNC_RESPONSE_t  async_response;
    union
    {
        L7_char8            eventData[L7_ASYNC_EVENT_MAX_STORED_DATA];  /* Use for Generic data */
        ASYNC_EVENT_DESCR_t async_event;      /* Use to pass async event utility */
    } signalInfo;
} asyncEventSignalDescr_t;


#define ASYNC_EVENT_SIGNAL_FLAG_SEMAB_EMPTY      1  /* Create an empty semaphore */


/*---------------------------------------------------------*/
/*                                                         */
/*    FUNCTION PROTOTYPES                                  */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/



/* Begin Function Prototypes */

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
L7_RC_t asyncEventHandlerInit();



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
L7_RC_t asyncEventHdlrFini();


/*********************************************************************
* @purpose  Create an asynchronous event handler 
*
* @param    pHandler    @b;pointer to asyncEventHandler_t structure
* @param    handlerId  @b;identifier to be stored by the caller
*
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
*       
* @end
*********************************************************************/
L7_RC_t asyncEventHandlerCreate(asyncEventHandler_t *pHandler, 
                                ASYNC_EVENT_HANDLER_t *handlerId );


/*********************************************************************
* @purpose  Delete an asynchronous event handler 
*
* @param    handlerID    @b;handlerID passed from create call
*
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
*       
* @end
*********************************************************************/
L7_RC_t asyncEventHandlerDelete(ASYNC_EVENT_HANDLER_t handlerId);


/*********************************************************************
* @purpose  Start an asynchronous event
*
* @param    handlerID    @b{(input)}handlerID passed from create call
* @param    correlator   @b{(input/output)}  sequence number of event correlator
* @param    pEventInfo   @b{(input)} Pointer to event info to be associated with the correlator
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes   A correlator previously created by asyncEventCorrelatorCreate() may be passed.
*          Otherwise, a correlator will be created if the pointer is null.
*       
* @end
*********************************************************************/
L7_RC_t asyncEventSyncStart(ASYNC_EVENT_HANDLER_t handlerId,
                            ASYNC_CORRELATOR_t correlator,
                            void *pEventInfo);
                               

/*********************************************************************
* @purpose  Finish an asynchronous event
*
* @param    handlerID    @b;handlerID passed from create call
* @param    correlator    @b; correlator ( ASYNC_CORRELATOR_t handle)
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
                         ASYNC_CORRELATOR_t correlator);
  


/*********************************************************************
*
* @purpose  Tally registered users completion for an asynchronous event
*
* @param    pComplete  @b;  pointer to ASYNC_EVENT_COMPLETE_INFO_t structure
*
* @returns  L7_SUCCESS or L7_FAILURE     
*
* @notes    At the conclusion of processing an asynchronous event, each client of
*           the event must call this function to indicate that it has completed
*           processing the event.
*          
* @end
*********************************************************************/
L7_RC_t asyncEventCompleteTally(ASYNC_EVENT_COMPLETE_INFO_t *pComplete);


/*********************************************************************
*
* @purpose  Create a correlator for an event
*
* @param    handlerID     @b{(input)}handlerID passed from create call
* @param    pCorrelator   @b{(output)} Pointer to a ASYNC_CORRELATOR_t
*
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*                               
* @notes    
*
* @end
*********************************************************************/
L7_RC_t asyncEventCorrelatorCreate(ASYNC_EVENT_HANDLER_t handlerId ,
                                    ASYNC_CORRELATOR_t *pCorrelator);

/* Correlator is automatically deleted through asyncEventSyncFinish(). */
     

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
                                 void *pData);

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
                                 void **pData);

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
L7_RC_t asyncEventSignalCreate(asyncEventSignalDescr_t  *pSignal, L7_uint32 flags);


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
L7_RC_t asyncEventSignalDelete(asyncEventSignalDescr_t  *pSignal);

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
L7_RC_t asyncEventSignalWait(asyncEventSignalDescr_t *pSignal);


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
void asyncEventSignalSend(asyncEventSignalDescr_t *pSignal, ASYNC_RESPONSE_t *pResponse);

/* End Function Prototypes */


#endif /* INCLUDE_ASYNC_EVENT_API_H */

