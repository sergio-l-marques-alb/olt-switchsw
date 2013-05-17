/*
 * ptin_igmp_sourcetimer.c
 *
 *  Created on: 22 de Abr de 2013
 *      Author: Daniel Figueira
 */

#include "snooping_ptin_sourcetimer.h"
#include "snooping_ptin_util.h"
#include "snooping_db.h"
#include "snooping_ptin_db.h"

#include <unistd.h>

#define TIMER_COUNT L7_MAX_GROUP_REGISTRATION_ENTRIES*(L7_MAX_PORT_COUNT+L7_MAX_NUM_LAG_INTF)*PTIN_SYSTEM_MAXSOURCES_PER_IGMP_GROUP

/* Message placed in CB queue */
typedef struct snoopPtinSourcetimerParams_s
{
  L7_uint8 eventIdx;
} snoopPtinSourcetimerParams_t;
#define PTIN_IGMP_TIMER_MSG_SIZE  sizeof(snoopPtinSourcetimerParams_t)

/* Static variables */
static void                   *handleListMemHndl = L7_NULLPTR;
static void                   *cbEventQueue = L7_NULLPTR;
static void                   *timerSem = L7_NULLPTR;
static L7_sll_t               timerLinkedList;
static L7_uint32              cbTaskId = L7_ERROR;
static L7_uint32              cbBufferPoolId = 0;
static handle_list_t          *handleList;
static L7_APP_TMR_CTRL_BLK_t  cbTimer;

/* Static methods */
static void     cbEventqueueTask(void);
static void     cbtimerCallback(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData);
static L7_RC_t  timerDataDestroy(L7_sll_member_t *ll_member);
static L7_int32 timerDataCmp(void *p, void *q, L7_uint32 key);
static void     timerCallback(void *param);


/*********************************************************************
* @purpose  Start Snooping task
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
L7_RC_t snoop_ptin_sourcetimer_init(void)
{
   /* Create queue semaphore */
   timerSem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
   if (timerSem == L7_NULLPTR)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create timers_sem semaphore!");
     return L7_FAILURE;
   }

   /* Queue that will process timer events */
   cbEventQueue = (void *) osapiMsgQueueCreate("cb_event_queue_sourcetimer", TIMER_COUNT, PTIN_IGMP_TIMER_MSG_SIZE);
   if (cbEventQueue == L7_NULLPTR)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR,"cb_event_queue creation error.");
     return L7_FAILURE;
   }

   /* Create task for sourcetimer management */
   cbTaskId = osapiTaskCreate("ptin_igmp_sourcetimer_task", cbEventqueueTask, 0, 0,
                                         L7_DEFAULT_STACK_SIZE,
                                         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                         L7_DEFAULT_TASK_SLICE);
   if (cbTaskId == L7_ERROR)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Could not create task ptin_igmp_sourcetimer_task");
     return L7_FAILURE;
   }
   if (osapiWaitForTaskInit (L7_PTIN_IGMP_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to initialize ptin_igmp_sourcetimer_task");
     return(L7_FAILURE);
   }

   /* Create CB buffer pool */
   cbBufferPoolId = 0;
   if(bufferPoolInit(TIMER_COUNT,
                     sizeof(timerNode_t) /*L7_APP_TMR_NODE_SIZE*/,
                     "PTin_IGMP_CB_SourceTimer_Buffer",
                     &cbBufferPoolId) != L7_SUCCESS)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to allocate memory for IGMP Control Block timer buffers");
     return L7_FAILURE;
   }

   /* Create SLL list for each IGMP instance */
   if (SLLCreate(L7_PTIN_COMPONENT_ID, L7_SLL_NO_ORDER,
                sizeof(L7_uint32)*2, timerDataCmp, timerDataDestroy,
                &timerLinkedList) != L7_SUCCESS)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Failed to create timer linked list");
     return L7_FAILURE;
   }

   /* Allocate memory for the Handle List */
   handleListMemHndl = (handle_member_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, TIMER_COUNT*sizeof(handle_member_t));
   if (handleListMemHndl == L7_NULLPTR)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Error allocating Handle List Buffers");
     return L7_FAILURE;
   }

   /* Create timers handle list for this IGMP instance  */
   if(handleListInit(L7_PTIN_COMPONENT_ID, TIMER_COUNT, &handleList, handleListMemHndl) != L7_SUCCESS)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to create timer handle list");
     return L7_FAILURE;
   }

   /* Initialize timer control blocks */
   cbTimer = appTimerInit(L7_PTIN_COMPONENT_ID, (void *) cbtimerCallback,
                          L7_NULLPTR, L7_APP_TMR_1SEC,
                          cbBufferPoolId);
   if (cbTimer  == L7_NULLPTR)
   {
     LOG_FATAL(LOG_CTX_PTIN_CNFGR,"snoopEntry App Timer Initialization Failed.");
     return L7_FAILURE;
   }

   LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Initializations for IGMPv3 source timers finished");

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Stop Snooping task
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
L7_RC_t snoop_ptin_sourcetimer_deinit(void)
{
  /* Deinitialize timer control blocks */
  if (cbTimer != (L7_APP_TMR_CTRL_BLK_t) NULL)
  {
    appTimerDeInit(cbTimer);
    cbTimer = (L7_APP_TMR_CTRL_BLK_t) NULL;
  }

  /* Remove timers handle list for this IGMP instance  */
  handleListDeinit(L7_PTIN_COMPONENT_ID, handleList);

  /* Free memory for the Handle List */
  if (handleListMemHndl != L7_NULLPTR)
  {
    osapiFree(L7_PTIN_COMPONENT_ID, handleListMemHndl);
    handleListMemHndl = L7_NULLPTR;
  }

  /* Destroy SLL list for each IGMP instance */
  SLLDestroy(L7_PTIN_COMPONENT_ID, &timerLinkedList);

  /* Buffer pool termination */
  if (cbBufferPoolId != 0)
  {
    bufferPoolTerminate(cbBufferPoolId);
    cbBufferPoolId = 0;
  }

  /* Delete task for sourcetimer management */
  if ( cbTaskId != L7_ERROR )
  {
    osapiTaskDelete(cbTaskId);
    cbTaskId = L7_ERROR;
  }

  /* Queue that will process timer events */
  if (cbEventQueue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(cbEventQueue);
    cbEventQueue = L7_NULLPTR;
  }

  osapiSemaDelete(timerSem);
  timerSem = L7_NULLPTR;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose   This function is used to send timer events
*
* @param     timerCtrlBlk    @b{(input)}   Timer Control Block
* @param     ptrData         @b{(input)}   Ptr to passed data
*
* @returns   None
*
* @notes     None
* @end
*********************************************************************/
void cbtimerCallback(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData)
{
  snoopPtinSourcetimerParams_t msg = {0};
  L7_RC_t                      rc;

  rc = osapiMessageSend(cbEventQueue, &msg, PTIN_IGMP_TIMER_MSG_SIZE, L7_NO_WAIT,L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Sourcetimer timer tick send failed");
    return;
  }
}

/*************************************************************************
* @purpose Task that makes the timer processing for the sourcetimer
*          management 
*  
* @param    none
*
* @notes    none
* @end
*************************************************************************/
void cbEventqueueTask(void)
{
  L7_uint32                    status;
  snoopPtinSourcetimerParams_t msg;

  if (osapiTaskInitDone(L7_PTIN_IGMP_TASK_SYNC)!=L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  while (1)
  {
    status = (L7_uint32) osapiMessageReceive(cbEventQueue, (void*)&msg,
                                             PTIN_IGMP_TIMER_MSG_SIZE, L7_WAIT_FOREVER);
    if (status == L7_SUCCESS)
    {
        appTimerProcess(cbTimer);
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"This is an invalid event");
    }
  }
}

/*************************************************************************
* @purpose  Helper API to compare two sourcetimer nodes and return the result
*
* @param     p  @b{(input)}  Pointer to Candidate 1 for comparison
* @param     q  @b{(input)}  Pointer to Candidate 2 for comparison
*
* @returns   0   p = q
* @returns  -1   p < q
* @returns  +1   p > q
*
* @comments This is called by SLL library when nodes are compared
*
* @end
*************************************************************************/
L7_int32 timerDataCmp(void *p, void *q, L7_uint32 key)
{
  L7_uint32 pMcastGroupAddr, pVlanId, pInterfaceIdx, pSourceIdx;
  L7_uint32 qMcastGroupAddr, qVlanId, qInterfaceIdx, qSourceIdx;

  /* Validate argument */
  if (p == L7_NULLPTR || ((snoopPTinL3Sourcetimer_t *) p)->groupData == L7_NULLPTR || 
      q == L7_NULLPTR || ((snoopPTinL3Sourcetimer_t *) q)->groupData == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null arguments");
    return 1;
  }

  pMcastGroupAddr = ((snoopPTinL3Sourcetimer_t *) p)->groupData->snoopPTinL3InfoDataKey.mcastGroupAddr;
  pVlanId         = ((snoopPTinL3Sourcetimer_t *) p)->groupData->snoopPTinL3InfoDataKey.vlanId;
  pInterfaceIdx   = ((snoopPTinL3Sourcetimer_t *) p)->interfaceIdx;
  pSourceIdx      = ((snoopPTinL3Sourcetimer_t *) p)->sourceIdx;

  qMcastGroupAddr = ((snoopPTinL3Sourcetimer_t *) q)->groupData->snoopPTinL3InfoDataKey.mcastGroupAddr;
  qVlanId         = ((snoopPTinL3Sourcetimer_t *) q)->groupData->snoopPTinL3InfoDataKey.vlanId;
  qInterfaceIdx   = ((snoopPTinL3Sourcetimer_t *) q)->interfaceIdx;
  qSourceIdx      = ((snoopPTinL3Sourcetimer_t *) q)->sourceIdx;

  if ( pMcastGroupAddr == qMcastGroupAddr && 
       pVlanId         == qVlanId         &&
       pInterfaceIdx   == qInterfaceIdx   &&
       pSourceIdx      == qSourceIdx      )
    return 0;

  return 1;
}

/*************************************************************************
* @purpose  API to destroy the sourcetimer data node
*
* @param    ll_member  @b{(input)}  Linked list node containing the
*                                   timer to be destroyed
*
* @returns  L7_SUCCESS
*
* @comments This is called by SLL library when a node is being deleted
*
* @end
*************************************************************************/
L7_RC_t timerDataDestroy (L7_sll_member_t *ll_member)
{
  snoopPTinL3Sourcetimer_t *pTimerData;

  /* Validate argument */
  if (ll_member==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Null arguments");
    return L7_FAILURE;
  }

  pTimerData = (snoopPTinL3Sourcetimer_t *)ll_member;

  /* Destroy timer */
  if (pTimerData->timer != L7_NULL)
  {
    /* Delete the apptimer node */
    if (appTimerDelete(cbTimer, pTimerData->timer)!=L7_SUCCESS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Cannot delete timer");
    }
    pTimerData->timer = (L7_APP_TMR_HNDL_t) NULL;

    /* Delete the handle we had created */
    handleListNodeDelete(handleList, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;
  }

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Callback method that is called when the sourcetimer expires.
 *
 * @param   param  Timer Handle
 * 
 * @see     RFC 3376 6.3
 *
 * @todo    When the filter-mode is EXCLUDE, are the clients removed?
 *************************************************************************/
void timerCallback(void *param)
{
  L7_uint32                interfaceIdx, sourceIdx;
  char                     debug_buf[46];
  snoopPTinL3Interface_t   *interfacePtr;
  L7_uint32                timerHandle;
  snoopPTinL3Sourcetimer_t *pTimerData;
  snoopPTinL3Source_t      *sourcePtr;
  snoopPTinL3InfoData_t*   groupData;

  timerHandle = (L7_uint32) param;
  osapiSemaTake(timerSem, L7_WAIT_FOREVER);

  /* Get timer handler */
  pTimerData = (snoopPTinL3Sourcetimer_t*) handleListNodeRetrieve(timerHandle);
  if (pTimerData == L7_NULLPTR)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Failed to retrieve handle");
    osapiSemaGive(timerSem);
    return;
  }
  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Sourcetimer expired (group:%s vlan:%u ifId:%u)", 
            snoopPTinIPv4AddrPrint(pTimerData->groupData->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf), pTimerData->groupData->snoopPTinL3InfoDataKey.vlanId, pTimerData->interfaceIdx);

  /* Check if our handle is OK*/
  if (timerHandle != pTimerData->timerHandle)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"timerHandle and pTimerData->timerHandle do not match!");
    osapiSemaGive(timerSem);
    return;
  }

  //Save grouptimer's internal data
  groupData    = pTimerData->groupData;
  interfaceIdx = pTimerData->interfaceIdx;
  sourceIdx    = pTimerData->sourceIdx;
  interfacePtr = &groupData->interfaces[interfaceIdx];
  sourcePtr    = &groupData->interfaces[interfaceIdx].sources[sourceIdx];
  osapiSemaGive(timerSem);

  if (interfacePtr->filtermode == PTIN_SNOOP_FILTERMODE_INCLUDE)
  {
    /* Remove source */
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Removing source %s", snoopPTinIPv4AddrPrint(sourcePtr->sourceAddr, debug_buf));
    snoopPTinSourceRemove(interfacePtr, sourcePtr);

    /* If no more sources remain, remove group */
    if (interfacePtr->numberOfSources == 0)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group has no more sources, thus it is being removed.");
      snoopPTinInterfaceRemove(interfacePtr);
    }
  }

  osapiSemaTake(timerSem, L7_WAIT_FOREVER);
  pTimerData->isRunning = L7_FALSE;
  osapiSemaGive(timerSem);
}

/*************************************************************************
 * @purpose Start a sourcetimer
 * 
 * @param   pTimer          Pointer to timer
 * @param   timeout         Timer's timeout
 * @param   groupData       Pointer to multicast group data
 * @param   interfaceIdx    Interface index
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 *************************************************************************/
L7_RC_t snoop_ptin_sourcetimer_start(snoopPTinL3Sourcetimer_t *pTimer, L7_uint16 timeout, snoopPTinL3InfoData_t *groupData, L7_uint32 interfaceIdx, L7_uint32 sourceIdx)
{
  L7_BOOL                  restart_timer = L7_FALSE;
  snoopPTinL3Sourcetimer_t pTimerData;
  char                     debug_buf[46];

  /* Argument validation */
  if (pTimer == L7_NULLPTR || groupData == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  osapiSemaTake(timerSem, L7_WAIT_FOREVER);

  /* Check if this timer already exists */
  memset(&pTimerData, 0x00, sizeof(snoopPTinL3Sourcetimer_t));
  pTimerData.groupData    = groupData;
  pTimerData.interfaceIdx = interfaceIdx;
  pTimerData.sourceIdx    = sourceIdx;
  if (SLLFind(&timerLinkedList, (void *)&pTimerData) != L7_NULL)
  {
    restart_timer = L7_TRUE;
    if (appTimerDelete(cbTimer, (void *) pTimer->timer) != L7_SUCCESS)
    {
      osapiSemaGive(timerSem);
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed restarting timer");
      return L7_FAILURE;
    }
    pTimer->isRunning = L7_FALSE;
    pTimer->timer = L7_NULLPTR;

    /* Remove timer handle */
    handleListNodeDelete(handleList, &pTimer->timerHandle);
    pTimer->timerHandle = 0;
  }
  else 
  {
    pTimer->groupData    = groupData;
    pTimer->interfaceIdx = interfaceIdx;
    pTimer->sourceIdx    = sourceIdx;
  }

  LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Starting sourcetimer (group:%s vlan:%u ifIdx:%u srcIdx:%u)",
            snoopPTinIPv4AddrPrint(groupData->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf), groupData->snoopPTinL3InfoDataKey.vlanId, interfaceIdx, sourceIdx);

  /* If timeout was configured as 0, do not set up the timer */
  if(timeout == 0)
  {
    pTimer->isRunning = L7_FALSE;
    osapiSemaGive(timerSem);
    return L7_SUCCESS;
  }

  /* New timer handle */
  if ((pTimer->timerHandle = handleListNodeStore(handleList, pTimer)) == 0)
  {
    osapiSemaGive(timerSem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not get the handle node to store the timer data.");
    return L7_FAILURE;
  }

  /* Add a new timer */
  pTimer->timer = appTimerAdd(cbTimer, timerCallback,
                              (void *) pTimer->timerHandle, timeout,
                              "PTIN_TIMER");
  if (pTimer->timer == NULL)
  {
    handleListNodeDelete(handleList, &pTimer->timerHandle);
    pTimer->timerHandle = 0;
    osapiSemaGive(timerSem);
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Could not start the sourcetimer.");
    return L7_FAILURE;
  }

  /* Add timer to SLL */
  if (!restart_timer && (SLLAdd(&timerLinkedList, (L7_sll_member_t *)pTimer) != L7_SUCCESS))
  {
    if (appTimerDelete(cbTimer, pTimer->timer) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to delete timer");
    }
    pTimer->timer = L7_NULLPTR;
    handleListNodeDelete(handleList, &pTimer->timerHandle);
    pTimer->timerHandle = 0;
    osapiSemaGive(timerSem);
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Could not add new timer data node");
    return L7_FAILURE;
  }

  /* Set timer status as running */
  pTimer->isRunning = L7_TRUE;
  osapiSemaGive(timerSem);
  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Stop a sourcetimer
 * 
 * @param   pTimer  Pointer to timer
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 *************************************************************************/
L7_RC_t snoop_ptin_sourcetimer_stop(snoopPTinL3Sourcetimer_t *pTimer)
{
  char debug_buf[46];

  /* Argument validation */
  if (pTimer == L7_NULLPTR || pTimer->groupData == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Removing sourcetimer (group:%s vlan:%u ifIdx:%u srcIdx:%u)",
            snoopPTinIPv4AddrPrint(pTimer->groupData->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf), pTimer->groupData->snoopPTinL3InfoDataKey.vlanId, pTimer->interfaceIdx, pTimer->sourceIdx);

  osapiSemaTake(timerSem, L7_WAIT_FOREVER);

  /* Remove node for SLL list */
  if (SLLDelete(&timerLinkedList, (L7_sll_member_t *)pTimer) != L7_SUCCESS)
  {
    osapiSemaGive(timerSem);
    LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Failed to delete timer node");
    return L7_FAILURE;
  }

  osapiSemaGive(timerSem);

  return L7_SUCCESS;
}

/*************************************************************************
 * @purpose Get the timeleft for a given sourcetimer
 * 
 * @param   pTimer  Pointer to timer
 *
 * @returns Timer's time left
 *
 *************************************************************************/
L7_uint32 snoop_ptin_sourcetimer_timeleft(snoopPTinL3Sourcetimer_t *pTimer)
{
  L7_uint32 time_left = 0;

  /* Argument validation */
  if (pTimer == L7_NULLPTR || pTimer->timer == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return 0;
  }
  else if(pTimer->isRunning == L7_FALSE)
  {
     return 0;
  }

  appTimerTimeLeftGet(cbTimer, pTimer->timer, &time_left);

  return time_left;
}
