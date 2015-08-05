/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: cnfgr_tally.c
*
* Purpose: Configurator component callback tally functions.
*
* Component: Configurator (cnfgr)
*
* Comments:  The following is the function catalog for Tally:
*
*            1. Configurator Interface (prototype in cnfgr_api.h)
*
*               - cnfgrApiCallback
*
*            2. Tally Internal Use Only (prototype in this file)
*
*               - cnfgrTallyLockResources 
*               - cnfgrTallyRemove 
*               - cnfgrTallyUnlockResources 
*               - cnfgrTallyWdTimerExpired
*
*            3. Tally Interface (prototype in cnfgr_tally.h)
* 
*               - cnfgrTallyAdd 
*               - cnfgrTallyAddComplete 
*               - cnfgrTallyCallbackReceive 
*               - cnfgrTallyClose
*               - cnfgrTallyDelete 
*               - cnfgrTallyFini
*               - cnfgrTallyInitialize 
*               - cnfgrTallyOpen 
*
* Created by: avasquez 03/17/2003 
*
*********************************************************************/
#include "cnfgr_include.h"
#include "log.h"

#include "logger.h"

#if (CNFGR_MODULE_TALLY == CNFGR_PRESENT)

/*
 *********************************************************************
 *             Static (local) Variables
 *********************************************************************
*/

/* Tally handle -  
 *
*/


typedef struct CNFGR_HANDLE
{
    L7_BOOL            free;
    L7_BOOL            addComplete;
    L7_uint32          msg; 
    L7_uint32          size;
    L7_int32           balance;
    osapiTimerDescr_t *pWatchdogTimer;
    L7_int32           watchdogSignature;
    L7_uint32          watchdogRetryCount;
    L7_CNFGR_STATE_t   nextState;
    L7_COMPONENT_IDS_t nextCid;
    L7_uint32          correlator;
    CNFGR_RSP_LIST_t   rspList;

} CNFGR_HANDLE_t;

#define CNFGR_HANDLE_MAX_f     cnfgrSidTallyHandleMaxGet()
#define CNFGR_RSP_LIST_MAX_f   cnfgrSidTallyRsplMaxGet()

static struct
{
    L7_uint32       used;
    L7_uint32       maxSize;
    CNFGR_HANDLE_t *pHandle;

} handleTable = { 0,0,0};


static void * handleLock = L7_NULLPTR;

typedef struct CNFGR_CORRELATOR 
{
    L7_BOOL            free;
    L7_uint32          ctHandle;
    L7_COMPONENT_IDS_t cid;
    L7_uint32          next;

} CNFGR_CORRELATOR_t;

/* Correlator pool -
 *
*/
#define CNFGR_CORRELATOR_MAX_f  cnfgrSidTallyCorrelatorMaxGet()

static struct
{
    L7_uint32           used;
    L7_uint32           maxSize;
    CNFGR_CORRELATOR_t *pCorrelator;

} correlatorTable = {0,0,0};

static void * correlatorLock = L7_NULLPTR;

/* Tally Resources 
 *
 * The following is used for locking and unlocking
 * Tally Resources
 */

typedef enum CNFGR_CT_RESOURCES
{
    CNFGR_CT_RES_NULL       = 0,  
    CNFGR_CT_RES_HANDLE,
    CNFGR_CT_RES_CORRELATOR

} CNFGR_CT_RESOURCES_t;


/* Tally Watchdog Timer -  
 *
*/

#define CNFGR_WATCHDOG_TIME_f   cnfgrSidTallyWatchdogTimeGet()
#define CNFGR_WATCHDOG_RETRY_f  cnfgrSidTallyWatchdogRetryGet()
 

/*
 *********************************************************************
 *             Configurator Interface Function Calls
 *********************************************************************
*/ 

 /*
    Configurator Callback function
 */

/*********************************************************************
* @purpose  This function handles request to components asycnchronous 
*           responses. The use of this function is mandatory.
*
* @param    pCbData    - @b{(input)} the completion response from the
*                                   component to the configurator.
*
* @returns  void
*
* @notes    This function is intended to be used by the component to
*           respond to a request issued by the configurator.
*           Registration to the component is not necessary.
*
* @notes    This function is running on the caller's thread. Do not process
*           the callback in the caller's thread, send message to Tally via
*           msg handler.           
*
*       
* @end
*********************************************************************/
void cnfgrApiCallback( CNFGR_IN L7_CNFGR_CB_DATA_t *pCbData )
{
    /* set up structures and variables */
    
    CNFGR_MSG_DATA_t  msgData,
                     *pMsgData = &msgData;
    L7_RC_t           rc;
 
    if ( cnfgrReadyAndInitialized != L7_TRUE )
        return;

    /* set up msg */
    pMsgData->msgType = CNFGR_MSG_CALLBACK;
    pMsgData->u.cbData.correlator = pCbData->correlator;
    rc = pMsgData->u.cbData.asyncResponse.rc = pCbData->asyncResponse.rc;

    PT_LOG_INFO(LOG_CTX_STARTUP,"allo: rc=%u response=%u", rc, pCbData->asyncResponse.u.response);

    if (rc == L7_SUCCESS) 
    {
      pMsgData->u.cbData.asyncResponse.u.response = pCbData->asyncResponse.u.response;
    }
    else
    {
#if 0
      printf("cnfgrApiCallback: failed by %s at line %d\n", func_name, line);
#endif

      pMsgData->u.cbData.asyncResponse.u.reason = pCbData->asyncResponse.u.reason;

      /* For now any failures in component configuration event processing
      ** causes a box reset.
      */
      L7_LOG_ERROR (correlatorTable.pCorrelator[pCbData->correlator].cid);

    }

    cnfgrTraceEventPerComp(L7_CNFGR_RQST_FIRST, L7_FALSE, L7_LAST_COMPONENT_ID, pCbData->correlator);
    cnfgrProfileEventCompStop(pCbData->correlator);
    /* send it ! */
    (void)cnfgrMsgSend(pMsgData);

    /* Return to caller */
    return;
}



/*
 *********************************************************************
 *                      Tally  intenal functions 
 *********************************************************************
*/


/*
    Internal function prototypes
*/

L7_RC_t cnfgrTallyLockResources(
                                CNFGR_IN CNFGR_CT_RESOURCES_t arg1, 
                                CNFGR_IN CNFGR_CT_RESOURCES_t arg2
                               );
L7_BOOL cnfgrTallyRemove(
                         CNFGR_IN  L7_CNFGR_CORRELATOR_t correlator, 
                         CNFGR_OUT CNFGR_CT_HANDLE_t *pCtHandle
                        );
L7_RC_t cnfgrTallyUnlockResources(
                                  CNFGR_IN CNFGR_CT_RESOURCES_t arg1, 
                                  CNFGR_IN CNFGR_CT_RESOURCES_t arg2
                                 );
void    cnfgrTallyWdTimerExpired(CNFGR_IN L7_uint32 arg1, CNFGR_IN L7_uint32 arg2);

/*
  Internal Funcions 
*/  

/*********************************************************************
* @purpose  This function locks tally resources. This is an internal  
*           function.                                                     
*
* @param    Handle     - @b{(input)} indicate to lock handle resource.
*           correlator - @b{(input)} indicate to indicate the correlator resource
*
* @returns  L7_SUCCESS  - Resources were locked 
*
* @returns  L7_ERROR    - one or more Resources were not locked 
*
* @notes    This function can only be used after Tally has been
*           initialized successfully. 
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrTallyLockResources(
                                CNFGR_IN CNFGR_CT_RESOURCES_t arg1, 
                                CNFGR_IN CNFGR_CT_RESOURCES_t arg2)
{
    L7_RC_t  ctRC1 = L7_ERROR;
    
    do {

        if (arg1 == CNFGR_CT_RES_HANDLE || arg2 == CNFGR_CT_RES_HANDLE) {
            ctRC1 = osapiSemaTake(handleLock, L7_WAIT_FOREVER);
            if (ctRC1 != L7_SUCCESS) {
                /* log message */

                /* Force exit  (goto EXIT:) */
                break; 
            } /* endif resource locked */
        } /* endif lock handle resources */

        if (arg1 == CNFGR_CT_RES_CORRELATOR || arg2 == CNFGR_CT_RES_CORRELATOR)
        {
            ctRC1 = osapiSemaTake(correlatorLock, L7_WAIT_FOREVER);  
            if (ctRC1 != L7_SUCCESS) {
                /* log message */

            } /* endif resource locked */
        } /* endif correlator resources */

    } while ( 0 );   /* enddo once */

    /* EXIT: return value to caller */
    return ctRC1;

}

/*********************************************************************
* @purpose  This function removes a correlator from the handle list,
*           returning it to the free correlator pool. This function
*           is only used by Tally.        
*
* @param    correlator - @b{(input)} Indexes into the correlator pool.
*           pCtHandle  - @b{(output)} handle index.
*
* @returns  void
*
* @notes    This function is intended to be used by the Tally when a
*           valid callback completion response is received. 
*                      
* @notes    This function ensures that there are not orphan correlators.
*           Thus it will intent "garbage collect" such correlators.
*       
* @end
*********************************************************************/
L7_BOOL cnfgrTallyRemove(
                         CNFGR_IN  L7_CNFGR_CORRELATOR_t correlator, 
                         CNFGR_OUT CNFGR_CT_HANDLE_t *pCtHandle
                        )
{

    CNFGR_HANDLE_t          *pHandle;
    CNFGR_CORRELATOR_t      *pCorrelator;
    L7_CNFGR_CORRELATOR_t    current,
                             previous;
    CNFGR_CT_HANDLE_t        ctHandle;
    L7_COMPONENT_IDS_t       cid;
    L7_RC_t                  ctRC1;
    L7_BOOL                  ctRC = L7_FALSE;

    /* Lock tally resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyLockResources(
                                     CNFGR_CT_RES_HANDLE, 
                                     CNFGR_CT_RES_CORRELATOR
                                   ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif lock tally resources */

    pCorrelator = &correlatorTable.pCorrelator[correlator];

    ctHandle = *pCtHandle = pCorrelator->ctHandle;
    pHandle  = &handleTable.pHandle[ctHandle];
    cid      = pCorrelator->cid;

    current = pHandle->correlator;

    /* We got here because Tally received a completion callback.
     * Check if the handle that the correlator is claiming 
     * to be attached really exist.  If it does not, then clean 
     * the correlator!
     *
     * NOTE: It is possible not to receive all the callbacks. Tally
     *       MUST ensure that a msg to CCTlr indicating the completion
     *       be issued only when all the callbacks have arrived.
    */                                              

    if (pHandle->correlator != 0 && 
        pHandle->size != 0 && 
        pHandle->free != L7_TRUE) {

        if (correlatorTable.pCorrelator[current].cid != cid ) {

            /* Remove the correlator from some place in the handle list */
            for (current = previous = pHandle->correlator;
                 (correlatorTable.pCorrelator[current].next != 0) &&
                 (correlatorTable.pCorrelator[current].cid != cid);
                )
            {
                previous = current;
                current  = correlatorTable.pCorrelator[current].next;

            } /* endfor find position of correlator in the handle list */

            /* is the correlator the same in the list?
             * If it does not, then it is not in the handle list. Clean it!
            */
            if (correlatorTable.pCorrelator[current].cid == cid ) { 
                /* At this point, current and previous mark the position of
                 * the correlator in the list --- remove it!
                */
                correlatorTable.pCorrelator[previous].next = pCorrelator->next;

                /* Update the handle counters */
                if (pHandle->balance != 0)
                    pHandle->balance--;
                if (pHandle->size != 0)
                    pHandle->size--;
            } /* endif correlator in the handle list */

        } else {

            pHandle->correlator = correlatorTable.pCorrelator[current].next;

            /* Update the handle counters */
            if (pHandle->balance != 0)
                pHandle->balance--;
            if (pHandle->size != 0)
                pHandle->size--;
    
        } /* endif correlator first in handle list */
    } /* endif valid handle */
    
    if (cid == L7_PTIN_COMPONENT_ID)
    {
      PT_LOG_NOTICE(LOG_CTX_MISC, "One less (correlatorTable.used=%u pHandle->size=%u) pHandle->addComplete=%u", correlatorTable.used, pHandle->size, pHandle->addComplete);
    }

    /* Free up the correlator */
    pCorrelator->free     = L7_TRUE;
    pCorrelator->next     = 0;
    pCorrelator->ctHandle = 0;
    pCorrelator->cid      = L7_LAST_COMPONENT_ID;
    if (correlatorTable.used != 0)
        correlatorTable.used--;

    if (pHandle->size == 0 && pHandle->addComplete == L7_TRUE) 
        ctRC = L7_TRUE;

    /* unlock tally resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyUnlockResources(
                                      CNFGR_CT_RES_HANDLE, 
                                      CNFGR_CT_RES_CORRELATOR
                                     ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif unlock tally resources */

    /* return to caller */
    return (ctRC);
}


/*********************************************************************
* @purpose  This function unlocks tally resources.This is an internal 
*           function.                                           
*
* @param    Handle     - @b{(input)} indicate to lock handle resource.
*           correlator - @b{(input)} indicate to indicate the correlator resource
*
* @returns  L7_SUCCESS  - Resources were unlocked 
*
* @returns  L7_ERROR    - one or more Resources were not unlocked 
*
* @notes    This function can only be used after Tally has been
*           initialized successfully. 
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrTallyUnlockResources(
                                  CNFGR_IN CNFGR_CT_RESOURCES_t arg1, 
                                  CNFGR_IN CNFGR_CT_RESOURCES_t arg2
                                 )
{
    L7_RC_t  ctRC1 = L7_ERROR;
    
    do {
        if (arg1 == CNFGR_CT_RES_HANDLE || arg2 == CNFGR_CT_RES_HANDLE) {
            ctRC1 = osapiSemaGive(handleLock);
            if (ctRC1 != L7_SUCCESS) {
                /* log message */

                /* force an exit (goto EXIT:) */
                break;          
            } /* endif resource unlocked */
        } /* endif lock handle resources */

        if (arg1 == CNFGR_CT_RES_CORRELATOR || arg2 == CNFGR_CT_RES_CORRELATOR) 
        {
            ctRC1 = osapiSemaGive(correlatorLock); 
            if (ctRC1 != L7_SUCCESS) {
                /* log message */

            } /* endif resource unlocked */
        } /* endif correlator resources */

    } while ( 0 );  /* enddo once */

    /* EXIT: return value to caller */
    return ctRC1;

}


/*********************************************************************
* @purpose  This function handles the watchdog timer callback. 
*           The use of this function is mandatory. This function is
*           Tally internal use only.
*
* @param    arg1    - @b{(input)} contains handle index (cthandle)
*                                 being timed.                    
* @param    arg2    - @b{(input)} timer signature for identification.
*                                                     
*
* @returns  void
*
* @notes    This function could be in an ISR thread. Use only
*           allowable OS operations. Do minimum work by placing
*           message to the message handler.
*
*       
* @end
*********************************************************************/
void cnfgrTallyWdTimerExpired( CNFGR_IN L7_uint32 arg1, CNFGR_IN L7_uint32 arg2)
{
    /* set up structures and variables */
    
    CNFGR_MSG_DATA_t  msgData,
                     *pMsgData = &msgData;
 
    if ( cnfgrReadyAndInitialized != L7_TRUE )
        return;

    /* set up msg */
    pMsgData->msgType = CNFGR_MSG_TIMER;
    pMsgData->u.tmrData.ctHandle  = arg1;
    pMsgData->u.tmrData.signature = (L7_int32)arg2;

    /* send it ! */
    (void)cnfgrMsgSend(pMsgData);

    /* return to caller */
    return;
}

/*
 *********************************************************************
 *                      Tally  Interface functions 
 *********************************************************************
*/


/*********************************************************************
* @purpose  This function adds an entry to the handle list and creates 
*           a correlator. This function is a Tally interface.
*
* @param    ctHandle     - @b{(input)}  The handle for the group of
*                                       completion callbacks -correlators.
*
* @param    cid          - @b{(input)}  The component identifier.
*       
* @param    pCorrellator - @b{(output)} The correlator
*
* @returns  L7_SUCCESS - Tally created and added a correlator to the         
*                        handle list.
* @returns  L7_ERROR   - Tally failed to create or add a correlator
*                        to the handle list. This COULD be a fatal error.
*
*
* @notes    The CCtlr MUST call this function to obtain a correlator
*           before issues the assynchronous request.
*  
* @notes    If the asynchronous call fails, the CCtlr MUST delete this
*           correlator or close the handle per error handling policies.
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrTallyAdd(
                      CNFGR_IN  CNFGR_CT_HANDLE_t      ctHandle, 
                      CNFGR_IN  L7_COMPONENT_IDS_t     cid,
                      CNFGR_OUT L7_CNFGR_CORRELATOR_t *pCorrelator
                    )
{
    CNFGR_HANDLE_t        *pHandle;
    L7_CNFGR_CORRELATOR_t  correlator,
                           savedCorrelator;
    L7_RC_t                ctRC1,
                           ctRC       = L7_ERROR;


    /* Lock tally resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyLockResources(
                                     CNFGR_CT_RES_HANDLE, 
                                     CNFGR_CT_RES_CORRELATOR
                                   ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif lock tally resources */

    /* get a correlator */
    for ( correlator = correlatorTable.maxSize; 
          correlator > 0 && (correlatorTable.pCorrelator[correlator].free == L7_FALSE); 
          correlator--
        ) 
    { } /* endfor find a correlator */

    /* if found */
    if (correlator != 0) {

     /* Initialize the correlator) */
        correlatorTable.pCorrelator[correlator].cid      = cid;
        correlatorTable.pCorrelator[correlator].ctHandle = ctHandle;
        correlatorTable.pCorrelator[correlator].free     = L7_FALSE;

     /* Insert the correlator into the handleTable 
      * 
     */
        pHandle = &handleTable.pHandle[ctHandle];

        /* connect correlator top of handle table */
        savedCorrelator = pHandle->correlator;
        pHandle->correlator = correlator;

        /* connect correlators in the chain */
        correlatorTable.pCorrelator[correlator].next = savedCorrelator;

        /* update counters */
        correlatorTable.used++;
        pHandle->size++;

        if (cid == L7_PTIN_COMPONENT_ID)
        {
          PT_LOG_NOTICE(LOG_CTX_MISC, "One plus (correlatorTable.used=%u pHandle->size=%u)", correlatorTable.used, pHandle->size);
        }
        /* return value */
        ctRC = L7_SUCCESS;
        *pCorrelator = correlator;

    } /* endif correlator available */

    /* unlock tally resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyUnlockResources(
                                      CNFGR_CT_RES_HANDLE, 
                                      CNFGR_CT_RES_CORRELATOR
                                     ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif unlock tally resources */

    /* return value to caller */
    return (ctRC);
}


/*********************************************************************
* @purpose  This function tells Tally that CCtlr has completed a  
*           bath of asynchronous request and to issue a completion  
*           message to CCtlr when all completion callback associated 
*           with the handle are received. This function is a Tally interface.
*
* @param    ctHandle     - @b{(input)} The handle for the group of
*                                      completion callbacks -correlators.
* @param    nextState    - @b{(input)} value for next message
* @param    nextCid      - @b{(input}) value for next message
*
* @returns  void
*
* @notes    This function is intended to be used by the CCtlr when it
*           has finish issuing asynchronous requests. 
*
*       
* @end
*********************************************************************/
void cnfgrTallyAddComplete(CNFGR_IN CNFGR_CT_HANDLE_t ctHandle, 
                           CNFGR_IN L7_int32 nextState,
                           CNFGR_IN L7_COMPONENT_IDS_t nextCid
                          )
{
    L7_uint32  watchdogTime;
    L7_RC_t    ctRC1;

    /* Lock tally Handle resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyLockResources(
                                     CNFGR_CT_RES_HANDLE, 
                                     CNFGR_CT_RES_NULL
                                   ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif lock tally resources */

    /* indicate that no more correlator to be added to this handle and
     * set internal handle structures
     * 
     * Start watchdog timer
     */
    handleTable.pHandle[ctHandle].addComplete = L7_TRUE;
    handleTable.pHandle[ctHandle].nextState   = nextState;
    handleTable.pHandle[ctHandle].nextCid     = nextCid;

    /* NOTE: Set the watchdog time and check for range. If value
     * is out of range, set to default value.
     *
     * NOTE: Set the watchdogSignature for validation on a timer
     * expired condition. The signature SHALL change during
     * cnfgrTallyClose. If timer expires and the signature in
     * the handle are not the same, then ignore that event, log
     * a message, and exit.
    */ 
    watchdogTime = CNFGR_WATCHDOG_TIME_f;
    
    handleTable.pHandle[ctHandle].watchdogSignature = rand();
    osapiTimerAdd(
                  cnfgrTallyWdTimerExpired, 
                  ctHandle,
                  (L7_uint32)handleTable.pHandle[ctHandle].watchdogSignature,
                  watchdogTime*1000,  /* N seconds EQ N000 milliseconds */
                  &handleTable.pHandle[ctHandle].pWatchdogTimer
                 );

    /* NOTE: it's possible that the system runs out of timer and the
     *       timer value is NULL.  Check, but let the system run in this
     *       case.
    */
    if (handleTable.pHandle[ctHandle].pWatchdogTimer == NULL) {
        /* log message */
    }

    /* unlock tally Handle resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyUnlockResources(
                                      CNFGR_CT_RES_HANDLE, 
                                      CNFGR_CT_RES_NULL
                                     ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif unlock tally resources */

    /* return to caller */
    return;
}


/*********************************************************************
* @purpose  This function receives and processes completion callbacks.
*           This function is a Tally interface.
*
* @param    ctHandle   - @b{(input)} the handle index into the handle
*                                   list.                         
*
* @returns  void
*
*       
* @end
*********************************************************************/
void cnfgrTallyCallbackReceive( CNFGR_IN L7_CNFGR_CB_DATA_t *pCbData)
{
    /* set up structures and variables */
    CNFGR_CT_HANDLE_t      ctHandle;
    L7_COMPONENT_IDS_t     cid;
    CNFGR_CORRELATOR_t    *pCorrelator;
    L7_CNFGR_CORRELATOR_t  correlator;
    L7_BOOL                rspListIsEmpty;
    CNFGR_RSP_LIST_t      *pRspList;
    osapiTimerDescr_t     *pWatchdogTimer;
    L7_CNFGR_STATE_t       nextState;
    L7_COMPONENT_IDS_t     nextCid;
    L7_RC_t                ctRC1;
 
    /* At this point pCbData is valid.. check for valid correlator 
     * range!  --- 
     *
     * NOTE: if out of range --NOOP. The watchdog timer
     *       should catch the component that responded with
     *       bad correlator.
    */
    correlator  = pCbData->correlator;
    if (correlator > 0 || correlator <= CNFGR_CORRELATOR_MAX_f) {

        /* Lock tally resources, if error log FATAL error message and 
         * follow system error handling 
        */
        ctRC1 = cnfgrTallyLockResources(
                                         CNFGR_CT_RES_HANDLE, 
                                         CNFGR_CT_RES_CORRELATOR
                                       ); 
        if (ctRC1 != L7_SUCCESS) {
            /* call the error handling routine 
             *
             * NOTE: log message has been issue already, no need to reissue.
            */
        } /* endif lock tally resources */

        /* <get relevant values> */
        pCorrelator    = &correlatorTable.pCorrelator[correlator];
        ctHandle       = pCorrelator->ctHandle;
        cid            = pCorrelator->cid;
        pRspList       = (CNFGR_RSP_LIST_t *)&handleTable.pHandle[ctHandle].rspList;
        pWatchdogTimer = handleTable.pHandle[ctHandle].pWatchdogTimer;
        nextState      = handleTable.pHandle[ctHandle].nextState;
        nextCid        = handleTable.pHandle[ctHandle].nextCid;

        /* <Set the global return code. If error previously occurred, do not set> */
        if (pRspList->aRsp.rc != L7_ERROR) {

            pRspList->aRsp.rc = pCbData->asyncResponse.rc;
            if (pCbData->asyncResponse.rc == L7_SUCCESS)
                pRspList->aRsp.u.response  = pCbData->asyncResponse.u.response;
            else
                pRspList->aRsp.u.reason    = pCbData->asyncResponse.u.reason;

        } /* endif error */

        /* <set the response entry for this correlator> 
         *
         * NOTE: if there are more responses than the size of the list, then some is
         *       gone bad.  Stop, and notify error handling routine.
         *
         *       cnfgrTallyClose will reset the 
        */
        if ( pRspList->size >= pRspList->maxSize ) {

            pRspList->aRsp.rc        = L7_ERROR;
            pRspList->aRsp.u.reason  = L7_CNFGR_ERR_RC_FATAL;

            /* error, log message */

        } else { 

            pRspList->pRlEntry[pRspList->size].cid      = cid;
            pRspList->pRlEntry[pRspList->size].aRsp.rc = pCbData->asyncResponse.rc;
            if (pCbData->asyncResponse.rc == L7_SUCCESS)
                pRspList->pRlEntry[pRspList->size].aRsp.u.response  = pCbData->asyncResponse.u.response;
            else
                pRspList->pRlEntry[pRspList->size].aRsp.u.reason    = pCbData->asyncResponse.u.reason;

            pRspList->size++;
    
        } /* endif rspList full */

        /* unlock tally resources, if error log FATAL error message and 
         * follow system error handling 
        */
        ctRC1 = cnfgrTallyUnlockResources(
                                          CNFGR_CT_RES_HANDLE, 
                                          CNFGR_CT_RES_CORRELATOR
                                         ); 
        if (ctRC1 != L7_SUCCESS) {
            /* call the error handling routine 
             *
             * NOTE: log message has been issue already, no need to reissue.
            */
        } /* endif unlock tally resources */

        /* remove this correlator */
        rspListIsEmpty = cnfgrTallyRemove(pCbData->correlator, (CNFGR_CT_HANDLE_t *)&ctHandle);
     
        /* issue tally complete, if required
         *
         * NOTE: tally complete has the final completion
         *       code for the command/request. If one
         *       or more requests completion callback 
         *       return error, tally complete will 
         *       indicate so to CCtlr. This will be
         *       done only once.
        */

        /* Is the response list empty */
        if (rspListIsEmpty) {

            /* Stop the watchdog timer 
             * 
             * NOTE: It is possible that watchdog timer expired before
             *       reaching here (queued after this last callback). in
             *       that case, the timer event will be ignored in 
             *       cnfgrTallyWdTimerExpired() since the ctHandle no
             *       longer exists.
             *
             * NOTE: do not change timer signature here, but at cnfgrTallyClose
             *       function call.
             *
            */
             osapiTimerFree(pWatchdogTimer);

            /* Tell CCTlr that we are done with this handle. 
             *
             * NOTE: The CCTlr will issue a close handle,
             *       at that point, the handle will be freed.
            */
            cnfgrCCtlrTallyComplete( ctHandle, nextState, nextCid, pRspList );

        } /* endif response list empty */

    } /* endif valid correlator */

    /* return to caller */
    return;
}

/*********************************************************************
* @purpose  This function releases a handle to the free handle pool.
*           This function is a Tally interface.  
*
* @param    ctHandle   - @b{(input)} the handle index into the handle
*                                   list.                         
*
* @returns  void
*
* @notes    This function is intended to be used by the a module authorized
*           to clean up the handle. 
*
* @notes    If there are correlators attached to this handle, they 
*           SHALL be released as well.
*
*       
* @end
*********************************************************************/
void cnfgrTallyClose(CNFGR_IN CNFGR_CT_HANDLE_t ctHandle)
{

   CNFGR_HANDLE_t          *pHandle;
   CNFGR_CORRELATOR_t      *pCorrelator;
   L7_CNFGR_CORRELATOR_t    correlator;
   L7_RC_t                  ctRC1;


   /* Lock tally resources, if error log FATAL error message and 
    * follow system error handling 
   */
   ctRC1 = cnfgrTallyLockResources(
                                    CNFGR_CT_RES_HANDLE, 
                                    CNFGR_CT_RES_CORRELATOR
                                  ); 
   if (ctRC1 != L7_SUCCESS) {
       /* call the error handling routine 
        *
        * NOTE: log message has been issue already, no need to reissue.
        */
   } /* endif lock tally resources */

   pHandle = &handleTable.pHandle[ctHandle];

    /* Is the handle list NOT empty */
   if (pHandle->size != 0) {

        /* Remove remaining correlators
         *
         * NOTE: It seems that the CCTrl wanted to stop.
         *       if Tally receive callback with these
         *       correlators, they will be cleaned up by Remove! 
        */

       for (;pHandle->size < 0; pHandle->size--) {
           

           correlator = pHandle->correlator;
           pCorrelator = &correlatorTable.pCorrelator[correlator];

           pHandle->correlator = pCorrelator->next;

           /* Free up the correlator */
           pCorrelator->free     = L7_TRUE;
           pCorrelator->next     = 0;
           pCorrelator->ctHandle = 0;
           pCorrelator->cid      = L7_LAST_COMPONENT_ID;
           if (correlatorTable.used != 0)
               correlatorTable.used--;

       } /* endfor free correlators */

    } /* endif handle list NOT empty */

   /* At this point the handle is empty -- final cleanup! */
   pHandle->addComplete       = L7_FALSE;
   pHandle->balance           = 
   pHandle->size              = 0;
   pHandle->correlator        = 0;
   pHandle->free              = L7_TRUE;
   pHandle->msg               = 0;
   pHandle->rspList.aRsp.rc   = L7_SUCCESS;
   pHandle->rspList.size      = 0;
   pHandle->watchdogSignature = rand();

   /* unlock tally resources, if error log FATAL error message and 
    * follow system error handling 
   */
   ctRC1 = cnfgrTallyUnlockResources(
                                     CNFGR_CT_RES_HANDLE, 
                                     CNFGR_CT_RES_CORRELATOR
                                    ); 
   if (ctRC1 != L7_SUCCESS) {
       /* call the error handling routine. 
        *
        * NOTE: log message has been issue already, no need to reissue.
        */
   } /* endif unlock tally resources */

    /* return to caller */
    return;
}

/*********************************************************************
* @purpose  This function deletes an entry from the handle list. This 
*           function is a Tally interface.
*
* @param    correllator - @b{(input)} The correlator to be removed.
*
* @returns  L7_SUCCESS - Tally deleted the correlator from the         
*                        handle list.
* @returns  L7_ERROR   - Tally failed to delete the correlator
*                        from the handle list. This COULD be a fatal error.
*
*
* @notes    The CCtlr COULD issue this function call at any time if and
*           only if Tally is initialized and ready.
*       
* @end
*********************************************************************/
L7_RC_t cnfgrTallyDelete(
                         CNFGR_IN L7_CNFGR_CORRELATOR_t  correlator
                        )
{
    CNFGR_HANDLE_t          *pHandle;
    CNFGR_CORRELATOR_t      *pCorrelator;
    L7_CNFGR_CORRELATOR_t    current,
                             previous;
    CNFGR_CT_HANDLE_t        ctHandle;
    L7_COMPONENT_IDS_t       cid;
    L7_RC_t                  ctRC1,
                             ctRC = L7_ERROR;


    /* Lock tally resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyLockResources(
                                     CNFGR_CT_RES_HANDLE, 
                                     CNFGR_CT_RES_CORRELATOR
                                   ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif lock tally resources */

    pCorrelator = &correlatorTable.pCorrelator[correlator];

    ctHandle = pCorrelator->ctHandle;
    pHandle  = &handleTable.pHandle[ctHandle];
    cid      = pCorrelator->cid;

    current = pHandle->correlator;

    /* We got here because CCTlr decided to delete the correlator
     * from the handle list.
     *
     * Check if the handle that the correlator is claiming 
     * to be attached really exist.  If it does not, then clean it up! 
     *
    */                                              

    if (pHandle->correlator != 0 && 
        pHandle->size != 0 && 
        pHandle->free != L7_TRUE) {

        if (correlatorTable.pCorrelator[current].cid != cid ) {

            /* Remove the correlator from some place in the handle list */
            for (current = previous = pHandle->correlator;
                 (correlatorTable.pCorrelator[current].next != 0) &&
                 (correlatorTable.pCorrelator[current].cid != cid);
                )
            {
                previous = current;
                current  = correlatorTable.pCorrelator[current].next;

            } /* endfor find position of correlator in the handle list */

            /* is the correlator the same in the list?
             * If it does not, then it is not in the handle list. Clean it!
            */
            if (correlatorTable.pCorrelator[current].cid == cid ) { 
                /* At this point, current and previous mark the position of
                 * the correlator in the list --- remove it!
                */
                correlatorTable.pCorrelator[previous].next = pCorrelator->next;

                /* Update the handle counters */
                if (pHandle->balance != 0)
                    pHandle->balance--;
                if (pHandle->size != 0)
                    pHandle->size--;
            } /* endif correlator in the handle list */

        } else {

            pHandle->correlator = correlatorTable.pCorrelator[current].next;

            /* Update the handle counters */
            if (pHandle->balance != 0)
                pHandle->balance--;
            if (pHandle->size != 0)
                pHandle->size--;
    
        } /* endif correlator first in handle list */
    } /* endif valid handle */
    
    /* Free up the correlator */
    pCorrelator->free     = L7_TRUE;
    pCorrelator->next     = 0;
    pCorrelator->ctHandle = 0;
    pCorrelator->cid      = L7_LAST_COMPONENT_ID;
    if (correlatorTable.used != 0)
        correlatorTable.used--;

    /* unlock tally resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyUnlockResources(
                                      CNFGR_CT_RES_HANDLE, 
                                      CNFGR_CT_RES_CORRELATOR
                                     ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif unlock tally resources */

    ctRC = L7_SUCCESS;

    /* return to caller */
    return (ctRC);
}


/*********************************************************************
* @purpose  This function terminates tally module. The use of this  
*           function is optional. This function is a Tally interface.                          
*
* @param    None
*
* @returns  None.          
*
* @notes    None                                              
*
*       
* @end
*********************************************************************/
void cnfgrTallyFini(void)
{
    L7_int32 i;
    
    /* Free the handle table
     * 
    */

    if ( handleLock != L7_NULLPTR ) {
        (void)osapiSemaDelete(handleLock);
        handleLock = L7_NULLPTR;

    } /* endif free handle lock */
    

    if (handleTable.pHandle != L7_NULLPTR ) {

        /* Initialize entries */
        for (i = 0; i < handleTable.maxSize+1; i++)
            if (handleTable.pHandle[i].rspList.pRlEntry != L7_NULLPTR )
                (void)osapiFree(L7_CNFGR_COMPONENT_ID, handleTable.pHandle[i].rspList.pRlEntry);

        (void)osapiFree ( L7_CNFGR_COMPONENT_ID, handleTable.pHandle  );
         handleTable.pHandle = L7_NULLPTR;

    } /* endif free the handle table resources */

    /* Free the correlator table resources
     *
    */

    if (correlatorLock != L7_NULLPTR) {
        (void)osapiSemaDelete(correlatorLock);
        correlatorLock = L7_NULLPTR;

    } /* endif free correlator lock */
    
    if (correlatorTable.pCorrelator  != L7_NULLPTR){
        (void)osapiFree( L7_CNFGR_COMPONENT_ID, correlatorTable.pCorrelator );
        correlatorTable.pCorrelator = L7_NULLPTR;

    } /* endif free correlator table */


    return;
}

/*********************************************************************
* @purpose  This function initializes tally module. The use of this  
*           function is mandatory. This function is a Tally interface.                          
*
* @param    None
*
* @returns  L7_SUCCESS - Tally initialized successfully and it is         
*                        ready to receive and tally callbacks.
* @returns  L7_ERROR   - Tally had problems and did not initialized.
*                        This is a fatal error.
*
*
* @notes    None                                              
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrTallyInitialize(void)
{
    L7_uint32 rlMaxSize;
    int       i;

    /* Initialize the handle table
     * 
    */
    handleTable.used    = 0;
    handleTable.maxSize = CNFGR_HANDLE_MAX_f;


    if ((handleLock = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == (void *)NULL)
    {
        /* FATAL ERROR: log message and return to caller */
        return (L7_ERROR); 
    }
    
    handleTable.pHandle =
        (CNFGR_HANDLE_t *)osapiMalloc( L7_CNFGR_COMPONENT_ID, (L7_uint32)((handleTable.maxSize+1)*sizeof(CNFGR_HANDLE_t)));

    if (handleTable.pHandle != (CNFGR_HANDLE_t *)L7_NULLPTR ) {

        /* Initialize entries */
        (void *)memset(handleTable.pHandle,'\0',(handleTable.maxSize+1)*sizeof(CNFGR_HANDLE_t));
        for (i = 0; i < handleTable.maxSize+1; i++) {
            handleTable.pHandle[i].size               =
                handleTable.pHandle[i].balance        =
                handleTable.pHandle[i].msg            = 
                handleTable.pHandle[i].correlator     = 0;
            handleTable.pHandle[i].addComplete        = L7_FALSE;
            handleTable.pHandle[i].free               = L7_TRUE;
            handleTable.pHandle[i].pWatchdogTimer     = (osapiTimerDescr_t *)NULL;
            handleTable.pHandle[i].watchdogRetryCount = 0;
            handleTable.pHandle[i].watchdogSignature  = rand();

            /* initialize the handle response list 
             *
             * NOTE: All relevant fields have been initialized, others will be done
             *       during normal operation.
            */
            rlMaxSize = handleTable.pHandle[i].rspList.maxSize = CNFGR_RSP_LIST_MAX_f;
            handleTable.pHandle[i].rspList.aRsp.rc             = L7_SUCCESS;
            handleTable.pHandle[i].rspList.size                = 0;
            handleTable.pHandle[i].rspList.pRlEntry            =
                (CNFGR_RSP_LIST_ENTRY_t *)osapiMalloc(L7_CNFGR_COMPONENT_ID, (L7_uint32)((rlMaxSize)*sizeof(CNFGR_RSP_LIST_ENTRY_t)));

            if (handleTable.pHandle[i].rspList.pRlEntry == (void *)NULL)
            {

                /* FATAL ERROR: Response list does not exist, 
                 *              log message, and return error to caller 
                */
                return (L7_ERROR);

            } /* endif response list exists */ 
            
        } /* endfor initialize entries */

    } else { 
        /* FATAL ERROR:  Callback table does not exist, 
         *               log message and return error to caller 
        */
        return (L7_ERROR);

    } /* endif NULL callback table array */

    /* Initialize the correlator table
     *
    */
     correlatorTable.used    = 0;
     correlatorTable.maxSize = CNFGR_CORRELATOR_MAX_f;


    if ((correlatorLock = osapiSemaMCreate(OSAPI_SEM_Q_FIFO)) == (void *)NULL)
    {
        /* FATAL ERROR: Could not create lock, 
         *              log message and return error to caller 
        */
        return (L7_ERROR);  
    }
    
    correlatorTable.pCorrelator =
        (CNFGR_CORRELATOR_t *)osapiMalloc( L7_CNFGR_COMPONENT_ID, (L7_uint32)((correlatorTable.maxSize+1)*sizeof(CNFGR_CORRELATOR_t)));

    if (correlatorTable.pCorrelator != (CNFGR_CORRELATOR_t *)L7_NULLPTR ) {

        /* Initialize entries */
        for (i = 0; i < correlatorTable.maxSize+1; i++) {
            correlatorTable.pCorrelator[i].free     = L7_TRUE;
            correlatorTable.pCorrelator[i].ctHandle = 0;
            correlatorTable.pCorrelator[i].cid      = L7_LAST_COMPONENT_ID;
            correlatorTable.pCorrelator[i].next     = 0;
        } /* endfor initialize entries */

    } else {
        /* FATAL ERROR: correlator table does not exist, log message and return error to caller */
        return (L7_ERROR); 
    } /* endif NULL callback table array */

     /* set Tally initialized and ready in the Configurator control block */

    /* Return value to caller */
    return (L7_SUCCESS);
}

/*********************************************************************
* @purpose  This function creates a handle that will host one or more 
*           correlators (completion callbacks). This is a Tally 
*           interface.
*
* @param    cbHandle   - @b{(input)} handle to callers callback function.
*           correlator - @b{(input)} value provided by component.
*
* @returns  ctHandle         --  a non-zero value that represent a
*                                Tally handle. If value is zero
*                                no handle is available.
*
* @notes    This function can only be used after Tally has been
*           initialized successfully. 
*
*       
* @end
*********************************************************************/
CNFGR_CT_HANDLE_t cnfgrTallyOpen(CNFGR_IN L7_CNFGR_CORRELATOR_t correlator,
                                 CNFGR_IN L7_CNFGR_CB_HANDLE_t cbHandle
                                )
{
    CNFGR_CT_HANDLE_t ctHandle;
    L7_RC_t           ctRC1;


    /* Lock tally handle resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyLockResources(
                                     CNFGR_CT_RES_NULL, 
                                     CNFGR_CT_RES_HANDLE
                                   ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif lock tally handle resources */

    /* find a handle */
    for ( ctHandle = handleTable.maxSize; 
          ctHandle > 0 && (handleTable.pHandle[ctHandle].free == L7_FALSE); 
          ctHandle--
        ) 
    { } /* endfor find a handle */

    if (ctHandle != 0) {

        /* Initialize the handle */
        handleTable.pHandle[ctHandle].free               = L7_FALSE;
        handleTable.pHandle[ctHandle].addComplete        = L7_FALSE;
        handleTable.pHandle[ctHandle].balance            = 
            handleTable.pHandle[ctHandle].size           =
            handleTable.pHandle[ctHandle].correlator     = 0;
        handleTable.pHandle[ctHandle].msg                = 0;
        handleTable.pHandle[ctHandle].rspList.aRsp.rc    = L7_SUCCESS;
        handleTable.pHandle[ctHandle].rspList.size       = 0;
        handleTable.pHandle[ctHandle].rspList.correlator = correlator;
        handleTable.pHandle[ctHandle].rspList.cbHandle   = cbHandle;
        /* update the handle table */
        handleTable.used++;

    } /* endif handle available */

    /* unlock tally handle resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyUnlockResources(
                                      CNFGR_CT_RES_NULL, 
                                      CNFGR_CT_RES_HANDLE
                                     ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif unlock tally handle resources */

    /* return value to caller */
    return (ctHandle);
}


/*********************************************************************
* @purpose  This function handles the watchdog timer expired message. 
*           The use of this function is mandatory. This function is
*           Tally interface.
*
* @param    ctHandle  - @b{(input)} contains handle index (cthandle)
*                                   being timed.                    
* @param    signature - @b{(input)} timer signature for identification.
*                                                     
*
* @returns  void
*
* @notes    This function runs in the configurator thread and
*           handles the watchdog timer.
*
*       
* @end
*********************************************************************/
void cnfgrTallyWdTimerExpiredReceive(
                                     CNFGR_IN CNFGR_CT_HANDLE_t ctHandle, 
                                     CNFGR_IN L7_int32 signature
                                    )
{
    CNFGR_HANDLE_t          *pHandle;
    L7_RC_t                  ctRC1;
    L7_uint32                watchdogRetryCount,
                             watchdogTime;
    CNFGR_RSP_LIST_t        *pRspList;
    L7_CNFGR_STATE_t         nextState;
    L7_COMPONENT_IDS_t       nextCid;
    L7_BOOL                  issueTallyComplete = L7_FALSE;

    /* set up variables and structures */
    /* Lock tally handle resources, if error log FATAL error message and 
     * follow system error handling 
    */
    ctRC1 = cnfgrTallyLockResources(
                                     CNFGR_CT_RES_HANDLE, 
                                     CNFGR_CT_RES_NULL
                                   ); 
    if (ctRC1 != L7_SUCCESS) {
        /* call the error handling routine 
         *
         * NOTE: log message has been issue already, no need to reissue.
         */
    } /* endif lock tally resources */

    pHandle  = &handleTable.pHandle[ctHandle];
    pRspList = &pHandle->rspList;


    /* Check for valid timer.
     * 
     * NOTE: it is possible that timer expired, but the configurator
     *       had the chance to complete its job. In that case
     *       discard the message, and return. Do not log message
     *       because this will be a normal condition.
    */

     if(pHandle->size != 0 && 
        pHandle->free != L7_TRUE && 
        pHandle->watchdogSignature == signature) 
     {

         /* At this point there is handle that does not have
          * all callbacks accounted for.  Check to see if
          * retry is needed.
          *
         */
         watchdogRetryCount = CNFGR_WATCHDOG_RETRY_f;

         if ( ++pHandle->watchdogRetryCount <= watchdogRetryCount ) {
             /* Restart the timer again and exit */

             watchdogTime = CNFGR_WATCHDOG_TIME_f;

             pHandle->watchdogSignature = rand();
             osapiTimerAdd(
                           cnfgrTallyWdTimerExpired, 
                           ctHandle,
                           (L7_uint32)pHandle->watchdogSignature,
                           watchdogTime*1000,  /* N seconds EQ N000 milliseconds */
                           &pHandle->pWatchdogTimer
                          );

             /* NOTE: it's possible that the system runs out of timer and the
              *       timer value is NULL.  Check, but let the system run in this
              *       case.
             */
             if (pHandle->pWatchdogTimer == NULL) {
                 /* At this point could not get a timer. log message
                  * and process as if the handle did not complete.
                  */

                 /* prepare the global return code and issue tally complete
                  * to CCTrl. Override any error in the response block.
                 */
                 pRspList->aRsp.rc        = L7_ERROR;
                 pRspList->aRsp.u.reason  = L7_CNFGR_ERR_RC_FATAL;

                 /* set internal flag. This is done to release the handle
                  * resuource before calling CCTlr
                 */ 
                 issueTallyComplete = L7_TRUE;

             } /* endif valid watchdog timer */

         }  else {

             /* there was an error... Log message and Handle it! 
              *
              * prepare the global return code and issue tally complete
              * to CCTrl. Override any error in the response block.
             */                 
             pRspList->aRsp.rc        = L7_ERROR;
             pRspList->aRsp.u.reason  = L7_CNFGR_ERR_RC_FATAL;

             /* set internal flag. This is done to release the handle
              * resuource before calling CCTlr
             */
             issueTallyComplete = L7_TRUE;

             /* For now log error.
             */
             L7_LOG_ERROR (correlatorTable.pCorrelator[pHandle->correlator].cid);

         } /* endif retry */

     } /* check handle signature */


     /* get relevant values */
     nextState = pHandle->nextState;
     nextCid   = pHandle->nextCid;

     /* unlock tally Handle resources, if error log FATAL error message and 
      * follow system error handling 
     */
     ctRC1 = cnfgrTallyUnlockResources(
                                       CNFGR_CT_RES_HANDLE, 
                                       CNFGR_CT_RES_NULL
                                      ); 
     if (ctRC1 != L7_SUCCESS) {
         /* call the error handling routine: fatal error. 
          *
          * NOTE: log message has been issue already, no need to reissue.
          */
     } /* endif unlock tally resources */

     /* Tell CCTlr that we are done with this handle, if required. */
     if (issueTallyComplete == L7_TRUE) 
         cnfgrCCtlrTallyComplete(ctHandle,nextState, nextCid, pRspList);

    return;
}

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*
 *********************************************************************
 *                      Stub Functions
 *********************************************************************
*/

#else

void cnfgrApiCallback( CNFGR_IN L7_CNFGR_CB_DATA_t *pCbData )
{


    /* Return to caller */
    return;
}

#endif /* end cnfgr_tally module */

