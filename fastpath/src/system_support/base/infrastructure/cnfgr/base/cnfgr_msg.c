/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: cnfgr_msg.c
*
* Purpose: Configurator component message handler functions.
*
* Component: Configurator (cnfgr)
*
* Comments:  The following is the function catalog for Message Handler:
*
*            1. Configurator Interface (prototype in cnfgr_api.h)
*
*               - None
*
*            2. Message Handler Internal Use Only (prototype in this file)
*
*               - cnfgrMsgEipIsSet  (CNFGR_EIP_f uses this function)
*
*            3. Message Handler Interface (prototype in cnfgr_msg.h)
*
*               - cnfgrMsgEventComplete
*               - cnfgrMsgFini
*               - cnfgrMsgHandler
*               - cnfgrMsgInitialize
*               - cnfgrMsgSend
*
* Created by: avasquez 03/17/2003
*
*********************************************************************/
#include "cnfgr_include.h"
#include "osapi.h"
#include "osapi_trace.h"

#if (CNFGR_MODULE_MSG == CNFGR_PRESENT)

/*
 *********************************************************************
 *             Static (local) Variables
 *********************************************************************
*/

static L7_BOOL cnfgrMsgEIP = L7_FALSE;


#define CNFGR_MSG_LOG_MAX_COUNT_f  cnfgrSidMsgLogMaxCountGet()

/* Message Queues */
#define CNFGR_MSG_NOOP_COUNT_f     cnfgrSidMsgNoopCountGet()

/* Message Q1 - Receive messages when !EIP.
 *
 */
#define CNFGR_MSG_Q1            "Cnfgr_Msg_Q1 "
#define CNFGR_MSG_Q1_COUNT_f     cnfgrSidMsgQ1CountGet()

static void *pCnfgrMsgQ1Id = L7_NULLPTR;

/* Message Q2 - Receive messages when EIP.
 *
 */
#define CNFGR_MSG_Q2            "Cnfgr_Msg_Q2 "
#define CNFGR_MSG_Q2_COUNT_f     cnfgrSidMsgQ2CountGet()

static void *pCnfgrMsgQ2Id = L7_NULLPTR;

/* Message Handler Thread -
 *
*/
#define CNFGR_MSG_HANDLER_THREAD_NAME          "Cnfgr_Thread "

#define CNFGR_MSG_HANDLER_THREAD_PRIO_f        cnfgrSidMsgHandlerThreadPrioGet()
#define CNFGR_MSG_HANDLER_THREAD_STACK_SIZE_f  cnfgrSidMsgHandlerThreadStackSizeGet()
#define CNFGR_MSG_HANDLER_THREAD_SLICE_f       cnfgrSidMsgHandlerThreadSliceGet()

static L7_int32 pCnfgrMsgHandlerTid = (L7_int32)L7_ERROR;


/*
 *********************************************************************
 *                      Message Handler intenal functions
 *********************************************************************
*/

/*
    Internal function prototypes
*/

L7_BOOL cnfgrMsgEipIsSet( void );
#define CNFGR_EIP_f cnfgrMsgEipIsSet()


/*
  Internal Funcions
*/
/*********************************************************************
* @purpose  This function test the state of the event in progress
*           flag (EIP). This function is Message Hander internal use
*           only.
*
* @param    None.
*
* @returns  L7_TRUE  - The EIP flag is set.
*           L7_FALSE - The EIP flag is not set.
*
* @notes    It is recommended to use CNFGR_EIP_f.
*
* @end
*********************************************************************/
L7_BOOL cnfgrMsgEipIsSet()
{
    return (cnfgrMsgEIP);
}



/*
 *********************************************************************
 *                      Message Handler interface functions
 *********************************************************************
*/

/*********************************************************************
* @purpose  This function indicates to the message handler that the
*           current event has been processed and can take the next
*           event. This function is a Message Handler interface
*           function.
*
* @param    None.
*
* @returns  None.
*
* @notes    None.
*
* @end
*********************************************************************/
void cnfgrMsgEventComplete()
{
    /* set CNFGR_EIP to L7_FALSE */
    cnfgrMsgEIP = L7_FALSE;

    /* return to caller */
    return;
}

/*********************************************************************
* @purpose  This function unconditionally terminates the Message Handler.
*           All Message Handler resources are returned. This function
*           is a Message Handler interface function.
*
* @param    None.
*
* @returns  None.
*
* @notes    None.
*
* @end
*********************************************************************/
void cnfgrMsgFini()
{
    /* set up variables and structures */

    /* remove the message handler thread */
    if (pCnfgrMsgHandlerTid != (L7_int32)L7_ERROR)
        (void)osapiTaskDelete(pCnfgrMsgHandlerTid);
    cnfgrMsgEIP = L7_FALSE;

    /* remove the message queues */
    if (pCnfgrMsgQ1Id != L7_NULLPTR)
        (void)osapiMsgQueueDelete( pCnfgrMsgQ1Id );

    if (pCnfgrMsgQ2Id != L7_NULLPTR)
        (void)osapiMsgQueueDelete( pCnfgrMsgQ2Id );

    /* return value to caller */
    return;
}

/*********************************************************************
* @purpose  This function handles messages to the component configurator.
*           This function is a Message Handler interface function.
*
* @param    None.
*
* @returns  None.
*
* @notes    None.
*
* @end
*********************************************************************/
void cnfgrMsgHandler()
{
    /* set up variables and structures */
    L7_RC_t           msgRC;
    CNFGR_MSG_DATA_t  msgData,
                     *pMsgData = &msgData;
    L7_uint32         loopCount,
                      logMsgCount = 0;

    do {
        /* receive command messages (!EIP) */
        msgRC = osapiMessageReceive(pCnfgrMsgQ1Id,
                                    pMsgData,
                                    sizeof(CNFGR_MSG_DATA_t),
                                    L7_WAIT_FOREVER);
        if ( msgRC == L7_SUCCESS && pMsgData != L7_NULLPTR ) {

            /* Message Handler Q1 dispatcher */
            switch (pMsgData->msgType) {

            case CNFGR_MSG_COMMAND:
                /* process command/event pair */
                cnfgrMsgEIP = L7_TRUE;
                cnfgrCCtlrProcessCommand(&pMsgData->u.cmdData);
                break;

            default:
                /* an invalid message has been received.
                 * log message and discard - receive next
                 * message.
                 */
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
                        "cnfgrMsgHandler: Discarded msg on Q1 %08x\n", pMsgData->msgType);
                cnfgrMsgEIP = L7_FALSE;
                break;   /* Let Q2 take a look for clean up, at least one pass */

            } /* endswitch command messages */

        } else {
            cnfgrMsgEIP = L7_FALSE;

            if (++logMsgCount <= CNFGR_MSG_LOG_MAX_COUNT_f ) {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
                        "cnfgrMsgHandler: Bad status on Q1 or invalid message pointer %08x\n", msgRC);

            } /* endif logMsgCount threshold */

        } /* endif status from Q1 */

        /* At this point a command has been processed (EIP) or Q1 received an
         * command that could not be processed (NOT EIP). Go ahead and Receive
         * callbacks or timer or NOOP messages, at least one (clean up timers).
        */
        loopCount = CNFGR_MSG_NOOP_COUNT_f;
        logMsgCount = 0;
        do {
            /* receive timers and callback messages
             *
             * NOTE: poll the Q2. don't wait for ever. EIP will
             *       indicated when to exit loop.
             *
             * NOTE: clear the msgData to force a NOOP. This action
             *       allows to poll the Q2, avoiding dead-lock.
             *
             * NOTE: Suspending the task may be required if and only
             *       if this thread is not splicing nor preemptable.
             *
            */
            (void *)memset(pMsgData,'\0',sizeof(CNFGR_MSG_DATA_t));
            msgRC = osapiMessageReceive(pCnfgrMsgQ2Id,
                                        pMsgData,
                                        sizeof(CNFGR_MSG_DATA_t),
                                        L7_NO_WAIT);
            if (msgRC == L7_SUCCESS && pMsgData != L7_NULLPTR) {

                /* Messge Handler Q2 Dispatcher */
                switch (pMsgData->msgType) {

                case CNFGR_MSG_CALLBACK:
                    /* process completion callback */
                    cnfgrTallyCallbackReceive( &pMsgData->u.cbData);
                    loopCount = CNFGR_MSG_NOOP_COUNT_f;
                    break;

                case CNFGR_MSG_TIMER:
                    /* process timer expired */
                    cnfgrTallyWdTimerExpiredReceive(pMsgData->u.tmrData.ctHandle,
                                                    pMsgData->u.tmrData.signature);
                    loopCount = CNFGR_MSG_NOOP_COUNT_f;
                    break;

                case CNFGR_MSG_NOOP:
                    if (--loopCount <= 0) {
                        (void)osapiTaskYield();
                        loopCount = CNFGR_MSG_NOOP_COUNT_f;
                    }
                    break;

                default:
                    /* an invalid message has been received.
                     * log message and discard - receive next
                     * message.
                    */
                    if (++logMsgCount <= CNFGR_MSG_LOG_MAX_COUNT_f ) {
                        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
                                "cnfgrMsgHandler: Discarded msg on Q2 %08x\n", pMsgData->msgType);
                    } /* endif logMsgCount threshold */

                    if (--loopCount <= 0) {
                        (void)osapiTaskYield();
                        loopCount = CNFGR_MSG_NOOP_COUNT_f;
                    } /* endif loop count */
                    break;

                } /* endswitch dispatch non-conditional messages */

            } else
            {
                OSAPI_TRACE_EVENT(L7_TRACE_EVENT_CNFGR_MAIN_DELAY, L7_NULLPTR, 0);
                osapiSleepMSec (10);
            }


        } while (CNFGR_EIP_f == L7_TRUE); /* do until NOT EIP */

    } while ( 1 ); /* do forever Q1 */
}

/*********************************************************************
* @purpose  This function initializes the Message Handler. This function
*           is a Message Handler interface.
*
* @param    None.
*
* @returns  L7_SUCCESS - Message Handler initialized succesfully.
* @returns  L7_ERROR   - Message Handler did not initialized succesfully.
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t cnfgrMsgInitialize()
{
    /* set up variables and structures */
    L7_RC_t  msgRC = L7_SUCCESS;

    /* create the message Queues -
     *
    */

    /* create the message Q1: FIFO */
    pCnfgrMsgQ1Id = (void *)osapiMsgQueueCreate( CNFGR_MSG_Q1,
                                                 CNFGR_MSG_Q1_COUNT_f,
                                                 sizeof(CNFGR_MSG_DATA_t) );
    /* Error ? */
    if (pCnfgrMsgQ1Id == L7_NULLPTR)
    {
      /*  Log Error Message - cnfgrMsgInitialize: initialization failed   */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
              "cnfgrMsgInitialize: Failed to create Garp Q1, exiting....\n");

      /* return Error value to caller */
      msgRC = L7_ERROR;

    } else {

        /* create the message Q2: FIFO */
        pCnfgrMsgQ2Id = (void *)osapiMsgQueueCreate( CNFGR_MSG_Q2,
                                                     CNFGR_MSG_Q2_COUNT_f,
                                                     sizeof(CNFGR_MSG_DATA_t) );
        /* Error ? */
        if (pCnfgrMsgQ2Id == L7_NULLPTR)
        {
          /*  Log Error Message - cnfgrMsgInitialize: initialization failed   */
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
                  "cnfgrMsgInitialize: Failed to create Message Handler Q2, exiting....\n");

          /* return error value to caller */
          msgRC = L7_ERROR;

        } else {
            /* Set Event In Progress (EIP) to L7_FALSE */
            cnfgrMsgEIP = L7_FALSE;

            /* create the Msg Handler thread. */
            pCnfgrMsgHandlerTid = osapiTaskCreate(CNFGR_MSG_HANDLER_THREAD_NAME,
                                                       cnfgrMsgHandler,
                                                       0, 0,
                                                       CNFGR_MSG_HANDLER_THREAD_STACK_SIZE_f,
                                                       CNFGR_MSG_HANDLER_THREAD_PRIO_f,
                                                       CNFGR_MSG_HANDLER_THREAD_SLICE_f);
            if (pCnfgrMsgHandlerTid == (L7_int32)L7_ERROR) {

                /* Log Error Message - cnfgrMsgInitialize: initialization failed */
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
                        "cnfgrMsgInitialize: Failed to create Message Handler Thread, exiting....\n");

                /* return error value to caller */
                msgRC = L7_ERROR;

            } /* endif create thread error */

        } /* endif create Q2 error */

    } /* endif create Q1 error */

    /* return value to caller */
    return (msgRC);
}

/*********************************************************************
* @purpose  This function sends internal messages to the configurator.
*           This function is a Message Handler interface.
*
* @param    None.
*
* @returns  L7_SUCCESS - Message sent succesfully.
* @returns  L7_ERROR   - Message was not sent successfully.
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t cnfgrMsgSend(CNFGR_IN CNFGR_MSG_DATA_t *pMsgData)
{
    /* set up variables and structures */
    L7_RC_t msgRC       = L7_ERROR;
    void * pCnfgrMsgQid = L7_NULLPTR;

    /* Determine which Queue to use */
    switch (pMsgData->msgType) {

    /* message for Q1 */
    case CNFGR_MSG_COMMAND:
        pCnfgrMsgQid = pCnfgrMsgQ1Id;
        break;

        /* message for Q2 */
    case CNFGR_MSG_CALLBACK:
    case CNFGR_MSG_TIMER:
    case CNFGR_MSG_NOOP:
        pCnfgrMsgQid = pCnfgrMsgQ2Id;
        break;

        /* invalid message */
    default:
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CNFGR_COMPONENT_ID,
                "cnfgrMsgSend: Invalid Message %08x\n", pMsgData->msgType);
        break;

    } /* endswitch queue to use */
    if (pCnfgrMsgQid != L7_NULLPTR)
        msgRC = osapiMessageSend( pCnfgrMsgQid,
                                  (void *)pMsgData,
                                  sizeof(CNFGR_MSG_DATA_t),
                                  L7_NO_WAIT,
                                  L7_MSG_PRIORITY_NORM);
    /* At this point the message may have been sent to the
     * Message Handler (cnfgrMsgHandler() in cnfgr_msg.c)
    */

    /* return value to caller */
    return (msgRC);
}

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*
 *********************************************************************
 *                      Stub Functions
 *********************************************************************
*/

#else


#endif /* end cnfgr_msg module */

