#include "ptin_timerMng.h"
#include "osapi.h"
#include "buff_api.h"
#include "ptin_globaldefs.h"
#include "logger.h"
#include "unistd.h"

/* Maximum number of timers */
static L7_uint16 ptin_timers_max = 0;

struct
{
  L7_APP_TMR_CTRL_BLK_t timerCB;       /* Entry App Timer Control Block */
  L7_uint32             ctrlBlkBufferPoolId;
  L7_uint32             appTimerBufferPoolId;
  handle_list_t         *appTimer_handle_list;
  void                  *appTimer_handleListMemHndl;
  L7_uint32             global_number_of_timers;
} timersMng;



/* Semaphore for timers access */
void *ptin_timers_sem = L7_NULLPTR;

/* Timers variables */
L7_uint32 timersMngmt_TaskId = L7_ERROR;
void     *timersMngmt_queue  = L7_NULLPTR;

typedef struct ptinTimerParams_s
{
  //L7_uint32          igmp_idx;
  L7_uint32          dummy;
} ptinTimerParams_t;
#define PTIN_TIMER_MSG_SIZE  sizeof(ptinTimerParams_t)


/* Timer data */
typedef struct timerData_s
{
  L7_sll_member_t   *next;

  L7_uchar8         timerType;
  L7_APP_TMR_HNDL_t timer;
  L7_uint32         timerHandle;

  /* User defined */
  void *data;

} timerData_t;

/* Internal functions */
void ptin_timerMng_task(void);
void ptin_timerMng_expiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData);
void ptin_timerMng_expiryCallback(void *param);
L7_RC_t ptin_timerMng_data_destroy (L7_sll_member_t *ll_member);
L7_int32 ptin_timerMng_data_compare(void *p, void *q, L7_uint32 key);


/* Debug routine */
L7_BOOL ptin_timerMng_debug_snooping = 0;

void ptin_timerMng_debug_enable(L7_BOOL enable)
{
  ptin_timerMng_debug_snooping = enable;
}



/**
 * Task that makes the timer processing for the timers 
 * management 
 */
void ptin_timerMng_task(void)
{
  L7_uint32 status;
  ptinTimerParams_t msg;

  PT_LOG_TRACE(LOG_CTX_MISC,"Timer event process task started");

  if (osapiTaskInitDone(L7_PTIN_TIMERMNG_TASK_SYNC)!=L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  PT_LOG_TRACE(LOG_CTX_MISC,"Timer task ready to process events");

  /* Loop */
  while (1)
  {
    status = (L7_uint32) osapiMessageReceive(timersMngmt_queue,
                                             (void*)&msg,
                                             PTIN_TIMER_MSG_SIZE,
                                             L7_WAIT_FOREVER);

    /* Process message */
    if (status == L7_SUCCESS)
    {
      appTimerProcess( timersMng.timerCB );
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_MISC,"This is an invalid event");
    }
  }
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
void ptin_timerMng_expiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData)
{
  L7_RC_t rc;
  ptinTimerParams_t msg;

  msg.dummy = 0;

  rc = osapiMessageSend(timersMngmt_queue, &msg, PTIN_TIMER_MSG_SIZE, L7_NO_WAIT,L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MISC,"Timer tick send failed");
    return;
  }
}


/*************************************************************************
* @purpose  API to destroy the group membership timer data node
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
L7_RC_t ptin_timerMng_data_destroy (L7_sll_member_t *ll_member)
{
  timerData_t *pTimerData;

  if (ptin_timerMng_debug_snooping)
    PT_LOG_TRACE(LOG_CTX_MISC,"Going to destroy timer");

  /* Validate argument */
  if (ll_member==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MISC,"Null arguments");
    return L7_FAILURE;
  }

  pTimerData = (timerData_t *)ll_member;

  if (ptin_timerMng_debug_snooping)
    PT_LOG_TRACE(LOG_CTX_MISC,"Ready to destroy timer");

  /* Destroy timer */
  if (pTimerData->timer != L7_NULL)
  {
    /* Delete the apptimer node */
    if (appTimerDelete(timersMng.timerCB, pTimerData->timer) != L7_SUCCESS)
    {
      if (ptin_timerMng_debug_snooping)
        PT_LOG_NOTICE(LOG_CTX_MISC,"Cannot delete timer");
    }
    pTimerData->timer = (L7_APP_TMR_HNDL_t) NULL;

    if (ptin_timerMng_debug_snooping)
      PT_LOG_TRACE(LOG_CTX_MISC,"Timer deleted");

    /* Delete the handle we had created */
    handleListNodeDelete(timersMng.appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;

    if (ptin_timerMng_debug_snooping)
      PT_LOG_TRACE(LOG_CTX_MISC,"Timer handle removed");
  }
  else
  {
    if (ptin_timerMng_debug_snooping)
      PT_LOG_NOTICE(LOG_CTX_MISC,"Timer not running");
  }

  bufferPoolFree(timersMng.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
  if (ptin_timerMng_debug_snooping)
    PT_LOG_TRACE(LOG_CTX_MISC,"Buffer node removed");

  if (ptin_timerMng_debug_snooping)
    PT_LOG_TRACE(LOG_CTX_MISC,"Timer destroyed successfully");

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Init ptin_timer module
* 
* @param    max_timers: Maximum number of timers
* @param    timer_resolution: 1, 10, 100 or 1000 (ms)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ptin_timerMng_init(L7_uint16 max_timers, L7_uint16 timer_resolution)
{
  L7_uint32 bufferPoolId = 0;
  void *handleListMemHndl;
  handle_list_t *handle_list;

  /* Validate arguments */
  if (max_timers == 0)
  {
    PT_LOG_WARN(LOG_CTX_CNFGR, "Requested 0 timers to be allocated!");
    return L7_SUCCESS;
  }
  if (timer_resolution != L7_APP_TMR_1MSEC &&
      timer_resolution != L7_APP_TMR_10MSEC &&
      timer_resolution != L7_APP_TMR_100MSEC &&
      timer_resolution != L7_APP_TMR_1SEC)
  {
    PT_LOG_WARN(LOG_CTX_CNFGR, "Invalid timer resolution value: only allowed 1, 10, 100 or 1000 (ms)");
    return L7_FAILURE;
  }

  /* Maximum number of timers */
  ptin_timers_max = max_timers;

  /* Create semaphores */
  ptin_timers_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_timers_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_timers_sem semaphore!");
    return L7_FAILURE;
  }

  /* Control block buffer pool */
  if (bufferPoolInit(ptin_timers_max,
                     sizeof(timerNode_t) /*L7_APP_TMR_NODE_SIZE*/,
                     "PTin_IGMP_CtrlBlk_Timer_Bufs",
                     &bufferPoolId) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to allocate memory for IGMP Control Block timer buffers");
    return L7_FAILURE;
  }
  timersMng.ctrlBlkBufferPoolId = bufferPoolId;

  /* Timers buffer pool */
  if (bufferPoolInit(ptin_timers_max,
                     sizeof(timerData_t),
                     "PTin_Timer_Bufs",
                     &bufferPoolId) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to allocate memory for timer buffers");
    return L7_FAILURE;
  }
  timersMng.appTimerBufferPoolId = bufferPoolId;
  PT_LOG_TRACE(LOG_CTX_CNFGR,"Allocated buffer pools");

  /* Create timer handles */
  /* Allocate memory for the Handle List */
  handleListMemHndl = (handle_member_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, ptin_timers_max*sizeof(handle_member_t));
  if (handleListMemHndl == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR,"Error allocating Handle List Buffers");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_CNFGR,"Allocated memory for handle list");
  /* Create timers handle list for this IGMP instance  */
  if (handleListInit(L7_PTIN_COMPONENT_ID, ptin_timers_max,
                     &handle_list, handleListMemHndl) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR,"Unable to create timer handle list");
    return L7_FAILURE;
  }
  timersMng.appTimer_handleListMemHndl = handleListMemHndl;
  timersMng.appTimer_handle_list = handle_list;
  PT_LOG_TRACE(LOG_CTX_CNFGR,"Handle list created");

  /* Queue that will process timer events */
  timersMngmt_queue = (void *) osapiMsgQueueCreate("PTin_IGMP_Timer_Queue",
                                                    ptin_timers_max, PTIN_TIMER_MSG_SIZE);
  if (timersMngmt_queue == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR,"PTIN Timer msgQueue creation error.");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_CNFGR,"PTIN Timer msgQueue created.");

  /* Create task for timers management */
  timersMngmt_TaskId = osapiTaskCreate("ptin_timersMng_task", ptin_timerMng_task, 0, 0,
                                        L7_DEFAULT_STACK_SIZE,
                                        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                        L7_DEFAULT_TASK_SLICE);

  if (timersMngmt_TaskId == L7_ERROR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Could not create task ptin_timersMng_task");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_CNFGR,"Task ptin_timersMng_task created");

  if (osapiWaitForTaskInit (L7_PTIN_TIMERMNG_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR,"Unable to initialize ptin_timersMng_task()\n");
    return(L7_FAILURE);
  }
  PT_LOG_TRACE(LOG_CTX_CNFGR,"Task ptin_timersMng_task initialized");

  /* Initialize timer control block */
  timersMng.timerCB = appTimerInit(L7_PTIN_COMPONENT_ID, ptin_timerMng_expiryHdlr,
                      (void *) 0, timer_resolution /*L7_APP_TMR_1SEC*/,
                      timersMng.ctrlBlkBufferPoolId);
  if (timersMng.timerCB == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR,"snoopEntry App Timer Initialization Failed.");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_CNFGR,"Initializations for IGMP timers finished");

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Deinit ptin_timer module
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
L7_RC_t ptin_timerMng_deinit(void)
{
  /* Check if initialization was done */
  if (ptin_timers_max == 0)
  {
    PT_LOG_ERR(LOG_CTX_CNFGR, "timerMng Module not initialized!");
    return L7_SUCCESS;
  }

  /* Check if there are created timers */
  if (timersMng.global_number_of_timers > 0)
  {
    PT_LOG_ERR(LOG_CTX_CNFGR,"There are still %u timers ... destroy them first", timersMng.global_number_of_timers);
    return L7_FAILURE;
  }

  PT_LOG_INFO(LOG_CTX_CNFGR,"Deinitializing timerMng Module");

  /* Deinitialize timer control blocks */
  if (timersMng.timerCB != (L7_APP_TMR_CTRL_BLK_t) NULL)
  {
    appTimerDeInit(timersMng.timerCB);
    timersMng.timerCB = (L7_APP_TMR_CTRL_BLK_t) NULL;
  }
  PT_LOG_INFO(LOG_CTX_CNFGR,"ControlBlock deinitialized");

    /* Delete task for timers management */
  if ( timersMngmt_TaskId != L7_ERROR )
  {
    osapiTaskDelete(timersMngmt_TaskId);
    timersMngmt_TaskId = L7_ERROR;
  }
  PT_LOG_INFO(LOG_CTX_CNFGR,"Task deleted");

  /* Queue that will process timer events */
  if (timersMngmt_queue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(timersMngmt_queue);
    timersMngmt_queue = L7_NULLPTR;
  }
  PT_LOG_INFO(LOG_CTX_CNFGR,"Queue deleted");

  /* Free memory for the Handle List */
  if (timersMng.appTimer_handleListMemHndl != L7_NULLPTR)
  {
    osapiFree(L7_PTIN_COMPONENT_ID, timersMng.appTimer_handleListMemHndl);
    timersMng.appTimer_handleListMemHndl = L7_NULLPTR;
  }
  PT_LOG_INFO(LOG_CTX_CNFGR,"HandleListMemHndl removed");

  /* Buffer pool termination */
  if (timersMng.appTimerBufferPoolId != 0)
  {
    bufferPoolTerminate(timersMng.appTimerBufferPoolId);
    timersMng.appTimerBufferPoolId = 0;
  }
  PT_LOG_INFO(LOG_CTX_CNFGR,"appTimerBufferPoolId terminated");

  /* Buffer pool termination */
  if (timersMng.ctrlBlkBufferPoolId != 0)
  {
    bufferPoolTerminate(timersMng.ctrlBlkBufferPoolId);
    timersMng.ctrlBlkBufferPoolId = 0;
  }
  PT_LOG_INFO(LOG_CTX_CNFGR,"ctrlBlkBufferPoolId terminated");

  osapiSemaDelete(ptin_timers_sem);
  ptin_timers_sem = L7_NULLPTR;

  ptin_timers_max = 0;

  PT_LOG_TRACE(LOG_CTX_CNFGR,"Deinitializations for PTIN timers finished");

  return(L7_SUCCESS);
}

/**
 * Create all the necessary data structure for a new group of 
 * timers
 * 
 * @param context : Data structure pointer 
 * @param expiry_callback_fcn : callback function to be 
 *                              associated to timers expiration
 * @param compare_callback_fcn: Function used to 
 *                                        compare different timers
 * @param max_timers : Maximum number of timers for this context
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_timerMng_context_create(ptin_timerMng_context_t *context,
                                     ptin_timerMng_expiryCallback_fcn_t expiry_callback_fcn,
                                     ptin_timerMng_data_compare_fcn_t compare_callback_fcn,
                                     L7_uint32 max_timers)
{
  /* Check if initialization was done */
  if (ptin_timers_max == 0)
  {
    PT_LOG_ERR(LOG_CTX_CNFGR, "timerMng Module not initialized!");
    return L7_FAILURE;
  }

  /* Check if it's possible to create these timers */
  if (timersMng.global_number_of_timers + max_timers > ptin_timers_max)
  {
    PT_LOG_ERR(LOG_CTX_CNFGR,"No timers available (max number=%u, current number of timers=%u)", ptin_timers_max, timersMng.global_number_of_timers);
    return L7_FAILURE;
  }

  /* Clear input structure */
  memset(context, 0x00, sizeof(ptin_timerMng_context_t));

  /* Create timer buffer bools, and timer control blocks */

  osapiSemaTake(ptin_timers_sem, L7_WAIT_FOREVER);

  /* Create SLL list for each IGMP instance */
  if (SLLCreate(L7_PTIN_COMPONENT_ID, L7_SLL_NO_ORDER,
                sizeof(L7_uint32)*2, compare_callback_fcn, ptin_timerMng_data_destroy,
                &(context->ll_timerList)) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_timers_sem);
    PT_LOG_FATAL(LOG_CTX_CNFGR,"Failed to create timer linked list");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_CNFGR,"SLL list created");

  /* Callback */
  context->expiry_callback_fcn = expiry_callback_fcn;
  /* Maximum number of timers */
  context->max_timers = max_timers;

  /* Update total number of timers */
  timersMng.global_number_of_timers += max_timers;

  osapiSemaGive(ptin_timers_sem);

  PT_LOG_TRACE(LOG_CTX_CNFGR,"Group of Timers initialized");

  return(L7_SUCCESS);
}

/**
 * Destroy all the data structures associated to a group of 
 * timers 
 * 
 * @param context 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_timerMng_context_destroy(ptin_timerMng_context_t *context)
{
  /* Check if initialization was done */
  if (ptin_timers_max == 0)
  {
    PT_LOG_ERR(LOG_CTX_CNFGR, "timerMng Module not initialized!");
    return L7_FAILURE;
  }

  /* Validate arguments */
  if (context == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MISC,"Context not provided");
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_timers_sem, L7_WAIT_FOREVER);

  /* Destroy SLL list for each IGMP instance */
  SLLDestroy(L7_PTIN_COMPONENT_ID, &context->ll_timerList);
  memset(&context->ll_timerList, 0x00, sizeof(L7_sll_t));

  /* Update total number of timers */
  if (timersMng.global_number_of_timers >= context->max_timers)
  {
    timersMng.global_number_of_timers -= context->max_timers;
  }
  else
  {
    timersMng.global_number_of_timers = 0;
  }

  osapiSemaGive(ptin_timers_sem);

  PT_LOG_TRACE(LOG_CTX_CNFGR,"Deinitializations for IGMP timers finished");

  return(L7_SUCCESS);
}


/**
 * Start a new timer
 * 
 * @param context : Group of Timers context
 * @param data    : timer data 
 * @param timeout : timeout 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_timerMng_start(ptin_timerMng_context_t *context, void *data, L7_uint16 timeout)
{
  L7_BOOL timer_exists = L7_FALSE;
  timerData_t *pTimerData, timerData;

  /* Check if initialization was done */
  if (ptin_timers_max == 0)
  {
    PT_LOG_ERR(LOG_CTX_CNFGR, "timerMng Module not initialized!");
    return L7_FAILURE;
  }

  /* Validate arguments */
  if (context == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MISC,"Context not provided");
    return L7_FAILURE;
  }

  if (ptin_timerMng_debug_snooping)
    PT_LOG_TRACE(LOG_CTX_MISC,"Going to start timer");

  memset(&timerData, 0x00, sizeof(timerData_t));
  timerData.data = data;

  osapiSemaTake(ptin_timers_sem, L7_WAIT_FOREVER);

  /* Check if this timer already exists */
  if ( (pTimerData = (timerData_t *)SLLFind(&context->ll_timerList, (void *)&timerData)) != L7_NULLPTR )
  {
    timer_exists = L7_TRUE;
  }

  if ( timer_exists )
  {
    if (ptin_timerMng_debug_snooping)
      PT_LOG_NOTICE(LOG_CTX_MISC,"Already exists a timer running");

    if (pTimerData->timer != L7_NULL)
    {
      if (appTimerDelete(timersMng.timerCB, (void *) pTimerData->timer) != L7_SUCCESS)
      {
        osapiSemaGive(ptin_timers_sem);
        if (ptin_timerMng_debug_snooping)
          PT_LOG_ERR(LOG_CTX_MISC,"Failed removing timer");
        return L7_FAILURE;
      }
      pTimerData->timer = L7_NULLPTR;
      if (ptin_timerMng_debug_snooping)
        PT_LOG_TRACE(LOG_CTX_MISC,"Timer removed!");

      /* Remove timer handle */
      handleListNodeDelete(timersMng.appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      if (ptin_timerMng_debug_snooping)
        PT_LOG_TRACE(LOG_CTX_MISC,"Removed node from handle list");
    }
  }
  else
  {
    /* Buffer pool allocation for pTimerData*/
    if (bufferPoolAllocate(timersMng.appTimerBufferPoolId, (L7_uchar8 **) &pTimerData) != L7_SUCCESS)
    {
      osapiSemaGive(ptin_timers_sem);
      PT_LOG_ERR(LOG_CTX_MISC,"Could not start timer. Insufficient memory.");
      return L7_FAILURE;
    }
    if (ptin_timerMng_debug_snooping)
      PT_LOG_TRACE(LOG_CTX_MISC,"Allocated one Buffer node");

    /* Copy reference to input data */
    pTimerData->data = timerData.data;
  }

  /* New timer handle */
  if ((pTimerData->timerHandle = handleListNodeStore(timersMng.appTimer_handle_list, pTimerData)) == 0)
  {
    /* Free the previously allocated bufferpool */
    bufferPoolFree(timersMng.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    osapiSemaGive(ptin_timers_sem);
    PT_LOG_ERR(LOG_CTX_MISC,"Could not get the handle node to store the timer data.");
    return L7_FAILURE;
  }
  if (ptin_timerMng_debug_snooping)
    PT_LOG_TRACE(LOG_CTX_MISC,"Stored node in handle list");

  /* Add a new timer */
  pTimerData->timer = appTimerAdd( timersMng.timerCB, context->expiry_callback_fcn,
                                   (void *) pTimerData->timerHandle, timeout,
                                   "PTIN_TIMER");
  if (pTimerData->timer == NULL)
  {
    /* Free the previously allocated bufferpool */
    handleListNodeDelete(timersMng.appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;
    bufferPoolFree(timersMng.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    osapiSemaGive(ptin_timers_sem);
    PT_LOG_ERR(LOG_CTX_MISC,"Could not Start the timer.");
    return L7_FAILURE;
  }

  if ( !timer_exists )
  {
    /* Add timer to SLL */
    if (SLLAdd(&context->ll_timerList, (L7_sll_member_t *)pTimerData) != L7_SUCCESS)
    {
      /* Free the previously allocated bufferpool */
//    PT_LOG_ERR(LOG_CTX_MISC,"Could not add new timer data node");
      if (appTimerDelete( timersMng.timerCB, pTimerData->timer) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MISC,"Failed to delete timer");
      }
      pTimerData->timer = L7_NULLPTR;
      handleListNodeDelete(timersMng.appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      bufferPoolFree(timersMng.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
      osapiSemaGive(ptin_timers_sem);
      return L7_FAILURE;
    }
  }

  osapiSemaGive(ptin_timers_sem);

  if (ptin_timerMng_debug_snooping)
    PT_LOG_DEBUG(LOG_CTX_MISC,"Timer Started: timerCB=%p timer=%p timerHandle=%p timeout=%u (s)", timersMng.timerCB, pTimerData->timer, pTimerData->timerHandle, timeout);

  return L7_SUCCESS;
}

/**
 * Stops a running timer, andf remove all related resources
 * 
 * @param context : Group of Timers context
 * @param data : timer data 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_timerMng_stop(ptin_timerMng_context_t *context, void *data)
{
  timerData_t timerData;

  /* Check if initialization was done */
  if (ptin_timers_max == 0)
  {
    PT_LOG_ERR(LOG_CTX_CNFGR, "timerMng Module not initialized!");
    return L7_FAILURE;
  }

  /* Validate arguments */
  if (context == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MISC,"Context not provided");
    return L7_FAILURE;
  }

  if (ptin_timerMng_debug_snooping)
    PT_LOG_TRACE(LOG_CTX_MISC,"Going to stop a timer");

  memset(&timerData, 0x00, sizeof(timerData_t));
  timerData.data = data;

  osapiSemaTake(ptin_timers_sem, L7_WAIT_FOREVER);

  if (ptin_timerMng_debug_snooping)
    PT_LOG_TRACE(LOG_CTX_MISC,"Deleting SLL node");

  /* Remove node for SLL list */
  if (SLLDelete(&context->ll_timerList, (L7_sll_member_t *) &timerData) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_timers_sem);
    if (ptin_timerMng_debug_snooping)
      PT_LOG_ERR(LOG_CTX_MISC,"Failed to delete timer node");
    return L7_FAILURE;
  }

  osapiSemaGive(ptin_timers_sem);

  if (ptin_timerMng_debug_snooping)
    PT_LOG_DEBUG(LOG_CTX_MISC,"Timer stopped successfully");

  return L7_SUCCESS;
}

/**
 * Use this function to get timer data pointer within the expiry
 * callback function 
 * 
 * @param param : param given by the expiry callback
 * 
 * @return void* : pointer to data
 */
void *ptin_timerMng_expiryCallback_data_get(void *param)
{
  L7_uint32 timerHandle = (L7_uint32) param;
  timerData_t *pTimerData;

  if (param == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MISC, "param is a null pointer");
    return L7_NULLPTR;
  }

  osapiSemaTake(ptin_timers_sem, L7_WAIT_FOREVER);

  /* Get timer handler */
  pTimerData = (timerData_t *) handleListNodeRetrieve(timerHandle);
  if (pTimerData == L7_NULLPTR)
  {
    osapiSemaGive(ptin_timers_sem);
    if (ptin_timerMng_debug_snooping)
      PT_LOG_DEBUG(LOG_CTX_MISC,"Failed to retrieve handle");
    return L7_NULLPTR;
  }

  if (timerHandle != pTimerData->timerHandle)
  {
    PT_LOG_ERR(LOG_CTX_MISC,"Weird situation!");
  }

  osapiSemaGive(ptin_timers_sem);

  /* Return pointer to data */
  return pTimerData->data;
}

/**
 * Use this function to get timer data pointer within the 
 * compare callback function 
 * 
 * @param ptr : param given by the compare function
 * 
 * @return void* : pointer to data
 */
void *ptin_timerMng_compare_data_get(void *ptr)
{
  if (ptr == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MISC, "param is a null pointer");
    return L7_NULLPTR;
  }

  return ((timerData_t *) ptr)->data;
}

/*************************************************************************
* @purpose  Process the expiry timer events
*
* @param    param    @b{(input)}  Pointer to added group member interface
*                                 timer handle
*
* @returns  void
*
* @comments none
*
* @end
*************************************************************************/
void ptin_timerMng_expiryCallback(void *param)
{
  void *data;

  data = ptin_timerMng_expiryCallback_data_get(param);

  if (data == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MISC,"Error retrieving data related to this timer");
    return;
  }

  /* User defined */
  /* ... */
}

/*************************************************************************
* @purpose  Helper API to compare two group membership timer nodes  and
*           return the result
*
* @param     p  @b{(input)}  Pointer to Candidate 1 for comparison
* @param     q  @b{(input)}  Pointer to Candidate 2 for comparison
*
* @returns   0   p = q
* @returns  -1   p < q
* @returns  +1   p > q
*
* @comments This is called by SLL library when a nodes are compared
*
* @end
*************************************************************************/
L7_int32 ptin_timerMng_data_compare(void *p, void *q, L7_uint32 key)
{
  /* User defined */
  /* Use ptin_timerMng_compare_data_get(...) function to get the data pointer */

  /* Return -1 if p < q */
  /* Return +1 if p > q */
  /* Return 0 if p == q */
  return 0;
}




/** Testing routines (to be used as dev calls) */
#if 1
L7_uint32 timer_data[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
ptin_timerMng_context_t context;

void ptin_timerMng_teste_expiryCallback(void *param)
{
  L7_uint32 *timer_data;

  timer_data = ptin_timerMng_expiryCallback_data_get(param);
  if (timer_data == L7_NULLPTR)
  {
    printf("Can't get callback data!\r\n");
    return;
  }

  printf("Callback called: Timer associated is %u!\r\n", (L7_uint32) *timer_data);
}

L7_int32 ptin_timerMng_teste_compare(void *p, void *q, L7_uint32 key)
{
  if (*((L7_uint32 *) ptin_timerMng_compare_data_get(p)) < *((L7_uint32 *) ptin_timerMng_compare_data_get(q)))
  {
    return -1;
  }
  else if (*((L7_uint32 *) ptin_timerMng_compare_data_get(p)) > *((L7_uint32 *) ptin_timerMng_compare_data_get(q)))
  {
    return 1;
  }
  
  return 0;
}


void ptin_timerMng_teste_init(L7_uint max_timers)
{
  L7_RC_t rc;
  
  rc = ptin_timerMng_context_create(&context, ptin_timerMng_teste_expiryCallback, ptin_timerMng_teste_compare, max_timers);
  printf("Created timers group with %u timers: rc=%d\r\n", max_timers, rc);
}

void ptin_timerMng_teste_deinit(void)
{
  L7_RC_t rc;

  rc = ptin_timerMng_context_destroy(&context);
  printf("Context destryed: rc=%d\r\n", rc);

  memset(&context, 0x00, sizeof(ptin_timerMng_context_t));

  printf("Done!\r\n");
}

void ptin_timerMng_teste_start(L7_uint32 timer_idx, L7_uint32 timeout)
{
  L7_RC_t rc;

  if (timer_idx >= 10)
  {
    printf("Invalid index\r\n");
    return;
  }

  rc = ptin_timerMng_start(&context, (void *) &timer_data[timer_idx], timeout);
  printf("Started timer %u with a timeout=%u: rc=%d\r\n", timer_idx, timeout, rc);
}

void ptin_timerMng_teste_stop(L7_uint32 timer_idx)
{
  L7_RC_t rc;

  if (timer_idx >= 10)
  {
    printf("Invalid index\r\n");
    return;
  }

  rc = ptin_timerMng_stop(&context, (void *) &timer_data[timer_idx]);
  printf("Stopped timer %u: rc=%d\r\n", timer_idx, rc);
}
#endif

