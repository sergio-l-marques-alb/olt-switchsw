/**
 * ptin_prot_uplink.c
 *  
 * Implements the uplink protection for LAGs
 *
 * Created on: 2017/04/21 Author: Milton Ruas 
 *  
 * Notes:
 */

#include "ptin_prot_uplink.h"
#include "ptin_utils.h"
#include "ptin_intf.h"
#include "ptin_xconnect_api.h"
#include "nimapi.h"
#include "dot3ad_api.h"
#include "usmdb_mib_bridge_api.h"

#include "buff_api.h"
#include "l7apptimer_api.h"
#include "l7handle_api.h"
#include "ipc.h"

#define PROT_CALL_PROC_MS     10

#define IS_TIMER_RUNNING(protIdx) (uplinkprot[protIdx].wait_restore_timer_CMD == WTR_CMD_START)

typedef struct ptinProtTimerParams_s
{
  L7_uint32          dummy;
} ptinProtTimerParams_t;


typedef struct protTimerData_s
{
  L7_sll_member_t   *next;
  L7_uint32         protIdx;

  L7_uchar8         timerType;
  L7_APP_TMR_HNDL_t timer;
  L7_uint32         timerHandle;
} protTimerData_t;

typedef struct prot_timerMgmt_s
{
  L7_APP_TMR_CTRL_BLK_t timerCB;       /* Entry App Timer Control Block */
  L7_sll_t              ll_timerList;  /* Linked list of timer data nodes */
  L7_uint32             ctrlBlkBufferPoolId;
  L7_uint32             appTimerBufferPoolId;
  handle_list_t         *appTimer_handle_list;
  void                  *appTimer_handleListMemHndl;
} prot_timerMgmt_t;
prot_timerMgmt_t prot_timerMgmt;

/* Queue to process timer events */
void *protMngmt_queue  = L7_NULLPTR;
/* Task to process timer events */
L7_uint32 protMngmt_TaskId = L7_ERROR;

/* Semaphore for timers access */
void *ptin_prot_timers_sem = L7_NULLPTR;
void *ptin_prot_uplink_sem = L7_NULLPTR;

void ptin_prot_timersMng_task(void);
void ptin_prot_timerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData);
L7_RC_t prot_timer_dataDestroy(L7_sll_member_t *ll_member);
L7_int32 prot_timer_dataCmp(void *p, void *q, L7_uint32 key);
void prot_timer_expiry(void *param);

uplinkprot_st uplinkprot[MAX_UPLINK_PROT];

static PROT_OPCMD_t    operator_cmd[MAX_UPLINK_PROT];
static PROT_PortType_t operator_switchToPortType[MAX_UPLINK_PROT];

L7_RC_t uplinkprotFsmTransition(L7_uint16 protIdx, PROT_STATE_t state, L7_uint32 __line__);
L7_RC_t uplinkprotSwitchTo(L7_uint16 protIdx, PROT_PortType_t portType, PROT_LReq_t localRequest, L7_uint32 __line__);


/**
 * Initialize Module 
 *  
 * @return L7_RC_t   
 */
L7_RC_t ptin_prot_uplink_init(void)
{
  L7_uint32 bufferPoolId = 0;
  void *handleListMemHndl;
  handle_list_t *handle_list;
  L7_APP_TMR_CTRL_BLK_t timerCB;

  ptin_prot_timers_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_prot_timers_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_INTF, "Failed to create ptin_prot_timers_sem semaphore!");
    return L7_FAILURE;
  }
  ptin_prot_uplink_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_prot_uplink_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_INTF, "Failed to create ptin_prot_uplink_sem semaphore!");
    return L7_FAILURE;
  }

  /* Queue that will process timer events */
  protMngmt_queue = (void *) osapiMsgQueueCreate("PTin_ProtUplink_Timer_Queue", MAX_UPLINK_PROT, sizeof(ptinProtTimerParams_t));
  if (protMngmt_queue == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_INTF,"Prot Timer msgQueue creation error.");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF,"Prot Timer msgQueue created.");

  /* Create task for clients management */
  protMngmt_TaskId = osapiTaskCreate("ptin_protMngmt_task", ptin_prot_timersMng_task, 0, 0,
                                        L7_DEFAULT_STACK_SIZE,
                                        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                        L7_DEFAULT_TASK_SLICE);
  if (protMngmt_TaskId == L7_ERROR)
  {
    PT_LOG_FATAL(LOG_CTX_INTF, "Could not create task clientsMngmt_task");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF,"Task protMngmt_task created");

  /* Timer Initializations */
  /* Control block buffer pool */
  if (bufferPoolInit(MAX_UPLINK_PROT,
                     sizeof(timerNode_t) /*L7_APP_TMR_NODE_SIZE*/,
                     "PTin_PROT_CtrlBlk_Timer_Bufs",
                     &bufferPoolId) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_INTF, "Failed to allocate memory for PROT Control Block timer buffers");
    return L7_FAILURE;
  }
  prot_timerMgmt.ctrlBlkBufferPoolId = bufferPoolId;

  /* Timers buffer pool */
  if (bufferPoolInit(MAX_UPLINK_PROT,
                     sizeof(protTimerData_t),
                     "PTin_PROT_Timer_Bufs",
                     &bufferPoolId) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_INTF, "Failed to allocate memory for PROT timer buffers");
    return L7_FAILURE;
  }
  prot_timerMgmt.appTimerBufferPoolId = bufferPoolId;
  PT_LOG_TRACE(LOG_CTX_INTF,"Allocated buffer pools");

  /* Create SLL list for each PROT instance */
  if (SLLCreate(L7_PTIN_COMPONENT_ID, L7_SLL_NO_ORDER,
                sizeof(L7_uint32), prot_timer_dataCmp, prot_timer_dataDestroy,
                &(prot_timerMgmt.ll_timerList)) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_INTF,"Failed to create timer linked list");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF,"SLL list created");

  /* Create timer handles */
  /* Allocate memory for the Handle List */
  handleListMemHndl = (handle_member_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, MAX_UPLINK_PROT*sizeof(handle_member_t));
  if (handleListMemHndl == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_INTF,"Error allocating Handle List Buffers");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF,"Allocated memory for handle list");
  /* Create timers handle list for this PROT instance  */
  if (handleListInit(L7_PTIN_COMPONENT_ID, MAX_UPLINK_PROT,
                     &handle_list, handleListMemHndl) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_INTF,"Unable to create timer handle list");
    return L7_FAILURE;
  }
  prot_timerMgmt.appTimer_handleListMemHndl = handleListMemHndl;
  prot_timerMgmt.appTimer_handle_list = handle_list;
  PT_LOG_TRACE(LOG_CTX_INTF,"Handle list created");

  /* Initialize timer control blocks */
  timerCB = appTimerInit(L7_PTIN_COMPONENT_ID, ptin_prot_timerExpiryHdlr,
                         (void *) 0, L7_APP_TMR_1SEC,
                         prot_timerMgmt.ctrlBlkBufferPoolId);
  if (timerCB  == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_INTF,"snoopEntry App Timer Initialization Failed.");
    return L7_FAILURE;
  }
  prot_timerMgmt.timerCB = timerCB;

  return L7_SUCCESS;
}

/**
 * Blocking mechanism implemented here
 * 
 * @param intIfNum 
 * @param block_state 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_prot_intf_block(L7_uint32 intIfNum, L7_int block_state)
{
  ptin_intf_t ptin_intf;
  L7_uint32   ptin_port;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_intf_intIfNum2ptintf(intIfNum, &ptin_intf) != L7_SUCCESS ||
      ptin_intf_ptintf2port(&ptin_intf, &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error getting ptin_intf/ptin_port from intIfNum %u", intIfNum);
    return L7_FAILURE;
  }
  
  if (ptin_intf.intf_type == PTIN_EVC_INTF_LOGICAL)
  {
    rc = dot3adBlockedStateSet(intIfNum, block_state);
    PT_LOG_INFO(LOG_CTX_INTF, "ptin_intf %u/%u blocking state changed to %u value (rc=%d)", ptin_intf.intf_type, ptin_intf.intf_id, block_state, rc);
  }
  else if (ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL)
  {
    if (block_state)
    {
      /* Remove this port from all VLANs */
      rc = ptin_vlan_port_remove(ptin_port, 0);
      PT_LOG_INFO(LOG_CTX_INTF, "ptin_intf %u/%u removed from all existent VLANs (rc=%d)", ptin_intf.intf_type, ptin_intf.intf_id, rc);
    }
    else
    {
      /* Add this port to all configured VLANs */
      rc = ptin_vlan_port_add(ptin_port, 0);
      PT_LOG_INFO(LOG_CTX_INTF, "ptin_intf %u/%u added to all existent VLANs (rc=%d)", ptin_intf.intf_type, ptin_intf.intf_id, rc);
    }
  }

  return rc;
}


/**
 * Select active interface
 * 
 * @param protIdx : Protection group index
 * @param portType : Active port type
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_prot_select_intf(L7_uint32 protIdx, PROT_PortType_t portType)
{
  L7_BOOL block_intfW, block_intfP;

  /* Validate arguments */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid protIdx %u", protIdx);
    return L7_FALSE;
  }
  /* Check if Entry is valid */
  if (!uplinkprot[protIdx].admin)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "protIdx=%d empty", protIdx);
    return L7_FAILURE;
  }

  /* When portType is all, both LAGs must be restored to its original state */
  if (portType == PORT_ALL)
  {
    /* Undo any special schemes for protection */
    ptin_prot_intf_block(uplinkprot[protIdx].protParams.intIfNumW, -1);
    ptin_prot_intf_block(uplinkprot[protIdx].protParams.intIfNumP, -1);
    /* Flush MAC table */
    usmDbFdbFlushByPort(uplinkprot[protIdx].protParams.intIfNumW);
    usmDbFdbFlushByPort(uplinkprot[protIdx].protParams.intIfNumP);

    PT_LOG_INFO(LOG_CTX_INTF, "LAGs %u and %u restored!", uplinkprot[protIdx].protParams.intIfNumW, uplinkprot[protIdx].protParams.intIfNumP);
    return L7_SUCCESS;
  }
  
  if (portType == PORT_WORKING)
  {
    block_intfW = L7_FALSE;
    block_intfP = L7_TRUE;
  }
  else if (portType == PORT_PROTECTION)
  {
    block_intfW = L7_TRUE;
    block_intfP = L7_FALSE;
  }
  else if (portType == PORT_ALL)
  {
    block_intfW = L7_FALSE;
    block_intfP = L7_FALSE;
  }
  else
  {
    block_intfW = L7_TRUE;
    block_intfP = L7_TRUE;
  }

  PT_LOG_DEBUG(LOG_CTX_INTF, "Selected interface=%u (%u,%u)", portType, block_intfW, block_intfP);

  /* Firstly, block the inactive ports */
  if (block_intfW)
  {
    ptin_prot_intf_block(uplinkprot[protIdx].protParams.intIfNumW, L7_TRUE);
    usmDbFdbFlushByPort(uplinkprot[protIdx].protParams.intIfNumW);
  }
  if (block_intfP)
  {
    ptin_prot_intf_block(uplinkprot[protIdx].protParams.intIfNumP, L7_TRUE);
    usmDbFdbFlushByPort(uplinkprot[protIdx].protParams.intIfNumP);
  }
  /* Only then, unblock the active ones */
  if (!block_intfW)
  {
    ptin_prot_intf_block(uplinkprot[protIdx].protParams.intIfNumW, L7_FALSE);
  }
  if (!block_intfP)
  {
    ptin_prot_intf_block(uplinkprot[protIdx].protParams.intIfNumP, L7_FALSE);
  }

  return L7_SUCCESS;
}

/**
 * Check if timer is running
 * 
 * @param protIdx 
 * 
 * @return L7_BOOL : L7_TRUE / L7_FALSE
 */
L7_BOOL ptin_prot_timer_isrunning(L7_uint32 protIdx)
{
  protTimerData_t timerData;
  L7_BOOL timer_exists = L7_FALSE;

  /* Validate arguments */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid protIdx %u", protIdx);
    return L7_FALSE;
  }

  osapiSemaTake(ptin_prot_timers_sem, L7_WAIT_FOREVER);

  /* Check if this timer already exists */
  memset(&timerData, 0x00, sizeof(protTimerData_t));

  timerData.protIdx = protIdx;

  if ( (protTimerData_t *)SLLFind(&prot_timerMgmt.ll_timerList, (void *)&timerData) != L7_NULLPTR )
  {
    timer_exists = L7_TRUE;
  }

  osapiSemaGive(ptin_prot_timers_sem);

  return timer_exists;
}

/**
 * Start a new timer
 * 
 * @param protIdx
 * @param timeout
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_prot_timer_start(L7_uint32 protIdx, L7_uint32 timeout)
{
  L7_BOOL timer_exists = L7_FALSE;
  protTimerData_t *pTimerData, timerData;

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Going to start timer for protIdx=%u", protIdx);

  /* Validate arguments */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid protIdx %u", protIdx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_timers_sem, L7_WAIT_FOREVER);

  /* Check if this timer already exists */
  memset(&timerData, 0x00, sizeof(protTimerData_t));

  timerData.protIdx = protIdx;

  if ( (pTimerData = (protTimerData_t *)SLLFind(&prot_timerMgmt.ll_timerList, (void *)&timerData)) != L7_NULLPTR )
  {
    timer_exists = L7_TRUE;
  }

  if ( timer_exists )
  {
    //if (ptin_debug_igmp_snooping)
    PT_LOG_NOTICE(LOG_CTX_INTF,"Already exists a timer running for protIdx=%u", protIdx);

    if (pTimerData->timer != L7_NULL)
    {
      if (appTimerDelete(prot_timerMgmt.timerCB, (void *) pTimerData->timer) != L7_SUCCESS)
      {
        osapiSemaGive(ptin_prot_timers_sem);
        //if (ptin_debug_igmp_snooping)
        PT_LOG_ERR(LOG_CTX_INTF,"Failed removing timer");
        return L7_FAILURE;
      }
      pTimerData->timer = L7_NULLPTR;
      //if (ptin_debug_igmp_snooping)
      PT_LOG_TRACE(LOG_CTX_INTF,"Timer removed!");

      /* Remove timer handle */
      handleListNodeDelete(prot_timerMgmt.appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      //if (ptin_debug_igmp_snooping)
      PT_LOG_TRACE(LOG_CTX_INTF,"Removed node from handle list (protIdx=%u)", protIdx);
    }
  }
  else
  {
    /* Buffer pool allocation for pTimerData*/
    if (bufferPoolAllocate(prot_timerMgmt.appTimerBufferPoolId, (L7_uchar8 **) &pTimerData) != L7_SUCCESS)
    {
      osapiSemaGive(ptin_prot_timers_sem);
      PT_LOG_ERR(LOG_CTX_INTF,"Could not start timer. Insufficient memory.");
      return L7_FAILURE;
    }
    //if (ptin_debug_igmp_snooping)
    PT_LOG_TRACE(LOG_CTX_INTF,"Allocated one Buffer node (protIdx=%u)", protIdx);

    /* Timer description */
    pTimerData->protIdx = protIdx;
  }

  /* New timer handle */
  if ((pTimerData->timerHandle = handleListNodeStore(prot_timerMgmt.appTimer_handle_list, pTimerData)) == 0)
  {
    /* Free the previously allocated bufferpool */
    bufferPoolFree(prot_timerMgmt.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    osapiSemaGive(ptin_prot_timers_sem);
    PT_LOG_ERR(LOG_CTX_INTF,"Could not get the handle node to store the timer data.");
    return L7_FAILURE;
  }
  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Stored node in handle list (protIdx=%u)", protIdx);

  /* Add a new timer */
  pTimerData->timer = appTimerAdd( prot_timerMgmt.timerCB, prot_timer_expiry,
                                   (void *) pTimerData->timerHandle, timeout,
                                   "PROT_TIMER");
  if (pTimerData->timer == NULL)
  {
    /* Free the previously allocated bufferpool */
    handleListNodeDelete(prot_timerMgmt.appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;
    bufferPoolFree(prot_timerMgmt.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
    osapiSemaGive(ptin_prot_timers_sem);
    PT_LOG_ERR(LOG_CTX_INTF,"Could not Start the Client timer.");
    return L7_FAILURE;
  }

  if ( !timer_exists )
  {
    /* Add timer to SLL */
    if (SLLAdd(&prot_timerMgmt.ll_timerList, (L7_sll_member_t *)pTimerData) != L7_SUCCESS)
    {
      /* Free the previously allocated bufferpool */
//    PT_LOG_ERR(LOG_CTX_INTF,"Could not add new timer data node");
      if (appTimerDelete( prot_timerMgmt.timerCB, pTimerData->timer) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Failed to delete timer");
      }
      pTimerData->timer = L7_NULLPTR;
      handleListNodeDelete(prot_timerMgmt.appTimer_handle_list, &pTimerData->timerHandle);
      pTimerData->timerHandle = 0;
      bufferPoolFree(prot_timerMgmt.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
      osapiSemaGive(ptin_prot_timers_sem);
      return L7_FAILURE;
    }
  }

  osapiSemaGive(ptin_prot_timers_sem);

  //if (ptin_debug_igmp_snooping)
  PT_LOG_DEBUG(LOG_CTX_INTF,"Timer Started: protIdx=%u timerCB=%p timer=%p timerHandle=%p timeout=%u (s)", protIdx, prot_timerMgmt.timerCB, pTimerData->timer, pTimerData->timerHandle, timeout);

  return L7_SUCCESS;
}

/**
 * Stops a running timer, andf remove all related resources
 * 
 * @param ptin_port  : Interface port
 * @param client_idx : client index
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_prot_timer_stop(L7_uint32 protIdx)
{
  protTimerData_t timerData;

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Going to stop a timer (protIdx=%u)", protIdx);

  /* Validate arguments */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid protIdx %u", protIdx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_timers_sem, L7_WAIT_FOREVER);

  memset(&timerData, 0x00, sizeof(protTimerData_t));

  timerData.protIdx = protIdx;

  /* Check if timer exists */
  if ( ((protTimerData_t *)SLLFind(&prot_timerMgmt.ll_timerList, (void *)&timerData)) == L7_NULLPTR )
  {
    osapiSemaGive(ptin_prot_timers_sem);
    PT_LOG_TRACE(LOG_CTX_INTF,"No timer running... nothing to be done (protIdx=%u)", protIdx);
    return L7_SUCCESS;
  }

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Deleting SLL node (protIdx=%u)", protIdx);

  /* Remove node for SLL list */
  if (SLLDelete(&prot_timerMgmt.ll_timerList, (L7_sll_member_t *)&timerData) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_prot_timers_sem);
    //if (ptin_debug_igmp_snooping)
    PT_LOG_ERR(LOG_CTX_INTF,"Failed to delete timer node: protIdx=%u timerCB=%p timer=%p timerHandle=%p", protIdx, prot_timerMgmt.timerCB, timerData.timer, timerData.timerHandle);
    return L7_FAILURE;
  }

  osapiSemaGive(ptin_prot_timers_sem);

  //if (ptin_debug_igmp_snooping)
  PT_LOG_DEBUG(LOG_CTX_INTF,"Timer stopped successfully for protIdx=%u timerCB=%p timer=%p timerHandle=%p", protIdx, prot_timerMgmt.timerCB, timerData.timer, timerData.timerHandle);

  return L7_SUCCESS;
}

/**
 * Timer Timeout for an existent timer
 * 
 * @param protIdx  : 
 * @param timeleft : (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_prot_timer_timeout_get(L7_uint32 protIdx, L7_uint32 *timeLeft)
{
  L7_uint32 time_left = 0;
  protTimerData_t *pTimerData, timerData;

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Going to get the timer timeout (protIdx=%u)", protIdx);

  /* Validate arguments */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid protIdx %u", protIdx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_timers_sem, L7_WAIT_FOREVER);

  memset(&timerData, 0x00, sizeof(protTimerData_t));

  timerData.protIdx  = protIdx;

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Searching for an SLL node (protIdx=%u)", protIdx);

  /* Searching for the client timer */
  if ((pTimerData = (protTimerData_t *)SLLFind(&prot_timerMgmt.ll_timerList, (void *)&timerData)) == L7_NULLPTR)
  {
    osapiSemaGive(ptin_prot_timers_sem);
    //if (ptin_debug_igmp_snooping)
    PT_LOG_WARN(LOG_CTX_INTF,"Client timer not found: protIdx=%u",  protIdx);
    return L7_FAILURE;
  }

  if (pTimerData->timer == L7_NULL)
  {
    osapiSemaGive(ptin_prot_timers_sem);
    //if (ptin_debug_igmp_snooping)
    PT_LOG_ERR(LOG_CTX_INTF,"Client timer not running: protIdx=%u timerCB=%p timer=%p timerHandle=%p", protIdx, prot_timerMgmt.timerCB, pTimerData->timer, pTimerData->timerHandle);
    return L7_FAILURE;
  }

  appTimerTimeLeftGet(prot_timerMgmt.timerCB, pTimerData->timer, &time_left);

  //if (ptin_debug_igmp_snooping)
  PT_LOG_DEBUG(LOG_CTX_INTF,"Client Timer TimeOut: protIdx=%u timerCB=%p timer=%p timerHandle=%p time_left=%u (s))", protIdx, prot_timerMgmt.timerCB, pTimerData->timer, pTimerData->timerHandle, time_left);

  osapiSemaGive(ptin_prot_timers_sem);

  if (timeLeft != L7_NULLPTR)
  {
    *timeLeft = time_left;
  }

  return L7_SUCCESS;
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
void prot_timer_expiry(void *param)
{
  L7_uint32 timerHandle = (L7_uint32) param;
  protTimerData_t timerData;
  L7_uint32 protIdx;

  protTimerData_t *pTimerData;

  //if (ptin_debug_igmp_snooping)
  PT_LOG_DEBUG(LOG_CTX_INTF,"Expiration event ocurred for timerHandle %p!",timerHandle);

  osapiSemaTake(ptin_prot_timers_sem, L7_WAIT_FOREVER);

  /* Get timer handler */
  pTimerData = (protTimerData_t *) handleListNodeRetrieve(timerHandle);
  if (pTimerData == L7_NULLPTR)
  {
    //if (ptin_debug_igmp_snooping)
    PT_LOG_DEBUG(LOG_CTX_INTF,"Failed to retrieve handle");
    osapiSemaGive(ptin_prot_timers_sem);
    return;
  }

  if (timerHandle != pTimerData->timerHandle)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Weird situation!");
  }

  /* Save client information */
  protIdx = pTimerData->protIdx;

  memset(&timerData, 0x00, sizeof(protTimerData_t));
  timerData.protIdx = protIdx;

  PT_LOG_TRACE(LOG_CTX_INTF,"Deleting SLL node (protIdx=%u)", protIdx);
  /* Remove node for SLL list */
  if (SLLDelete(&prot_timerMgmt.ll_timerList, (L7_sll_member_t *)&timerData) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Failed to delete timer node: protIdx=%u timerCB=%p timer=%p timerHandle=%p", protIdx, prot_timerMgmt.timerCB, timerData.timer, timerData.timerHandle);
  }

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Expiration event ocurred for protIidx=%u timerCB=%p timer=%p timerHandle=%p", protIdx, prot_timerMgmt.timerCB, pTimerData->timer, pTimerData->timerHandle);

  osapiSemaGive(ptin_prot_timers_sem);

  /* Processing timeout */
  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);
 
  if (uplinkprot[protIdx].state_machine == PROT_STATE_Protection)
  {
    PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection => PROT_STATE_Normal state (%u)", protIdx);
    uplinkprotSwitchTo(protIdx, PORT_WORKING, PROT_LReq_WTRExp, __LINE__);
    uplinkprotFsmTransition(protIdx, PROT_STATE_Normal, __LINE__);
  }
  else if (uplinkprot[protIdx].state_machine == PROT_STATE_Normal)
  {
    PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal => PROT_STATE_Protection state (%u)", protIdx);
    uplinkprotSwitchTo(protIdx, PORT_PROTECTION, PROT_LReq_WTRExp, __LINE__);
    uplinkprotFsmTransition(protIdx, PROT_STATE_Protection, __LINE__);
  }
  osapiSemaGive(ptin_prot_uplink_sem);

  //if (ptin_debug_igmp_snooping)
  PT_LOG_DEBUG(LOG_CTX_INTF,"Timeout processed in protIdx=%u", protIdx);
}

/**
 * Task that makes the timer processing for the clients manageme
 */
void ptin_prot_timersMng_task(void)
{
  L7_uint32 status;
  ptinProtTimerParams_t msg;

  PT_LOG_TRACE(LOG_CTX_INTF,"Timer event process task started");

  /* Loop */
  while (1)
  {
    //PT_LOG_TRACE(LOG_CTX_INTF,"Timer task ready for new events");

    status = (L7_uint32) osapiMessageReceive(protMngmt_queue,
                                             (void*)&msg,
                                             sizeof(ptinProtTimerParams_t),
                                             L7_WAIT_FOREVER);

    //PT_LOG_TRACE(LOG_CTX_INTF,"Timer task received an event");

    /* TODO: Process message */
    if (status == L7_SUCCESS)
    {
      appTimerProcess( prot_timerMgmt.timerCB );
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_INTF,"This is an invalid event");
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
void ptin_prot_timerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData)
{
  L7_RC_t rc;
  ptinProtTimerParams_t msg;

  //PT_LOG_TRACE(LOG_CTX_INTF,"Received a timer event");

  msg.dummy = 0;

  //PT_LOG_TRACE(LOG_CTX_INTF,"Sending timer event to queue");

  rc = osapiMessageSend(protMngmt_queue, &msg, sizeof(ptinProtTimerParams_t), L7_NO_WAIT,L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Prot timer tick send failed");
    return;
  }

  //PT_LOG_TRACE(LOG_CTX_INTF,"Timer event sent to queue");
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
L7_RC_t prot_timer_dataDestroy (L7_sll_member_t *ll_member)
{
  protTimerData_t *pTimerData;

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Going to destroy timer");

  /* Validate argument */
  if (ll_member==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Null arguments");
    return L7_FAILURE;
  }

  pTimerData = (protTimerData_t *)ll_member;

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Ready to destroy timer for protIdx=%u", pTimerData->protIdx);

  /* Destroy timer */
  if (pTimerData->timer != L7_NULL)
  {
    /* Delete the apptimer node */
    if (appTimerDelete(prot_timerMgmt.timerCB, pTimerData->timer)!=L7_SUCCESS)
    {
      //if (ptin_debug_igmp_snooping)
      PT_LOG_ERR(LOG_CTX_INTF,"Cannot delete timer (protIdx=%u)", pTimerData->protIdx);
    }
    pTimerData->timer = (L7_APP_TMR_HNDL_t) NULL;

    //if (ptin_debug_igmp_snooping)
    PT_LOG_TRACE(LOG_CTX_INTF,"Timer deleted for protIdx=%u", pTimerData->protIdx);

    /* Delete the handle we had created */
    handleListNodeDelete(prot_timerMgmt.appTimer_handle_list, &pTimerData->timerHandle);
    pTimerData->timerHandle = 0;

    //if (ptin_debug_igmp_snooping)
    PT_LOG_TRACE(LOG_CTX_INTF,"Timer handle removed for protIdx=%u", pTimerData->protIdx);
  }
  else
  {
    //if (ptin_debug_igmp_snooping)
    PT_LOG_ERR(LOG_CTX_INTF,"Timer not running for protIdxdx=%u", pTimerData->protIdx);
  }

  bufferPoolFree(prot_timerMgmt.appTimerBufferPoolId, (L7_uchar8 *)pTimerData);
  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Buffer node removed for protIdx=%u", pTimerData->protIdx);

  //if (ptin_debug_igmp_snooping)
  PT_LOG_TRACE(LOG_CTX_INTF,"Timer destroyed successfully for protIdx=%u", pTimerData->protIdx);

  return L7_SUCCESS;
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
L7_int32 prot_timer_dataCmp(void *p, void *q, L7_uint32 key)
{
  if ( ((protTimerData_t *) p)->protIdx < ((protTimerData_t *) q)->protIdx )
    return -1;
  if ( ((protTimerData_t *) p)->protIdx > ((protTimerData_t *) q)->protIdx )
    return 1;

  return 0;
}




/**
 * WRT command
 * 
 * @author joaom (04/06/2014)
 * 
 * @param protIdx 
 * 
 * @return PROT_RC_t 
 */
L7_RC_t uplinkprotWtr(L7_uint16 protIdx, PROT_WTR_CMD_t cmd, L7_uint32 __line__)
{
  PT_LOG_TRACE(LOG_CTX_INTF, "(protIdx %d, cmd %d) [__line__ %d]", protIdx, cmd, __line__);

  if (cmd == WTR_CMD_STOP)
  {
    /* Reset Counter */
    uplinkprot[protIdx].wait_restore_timer = 0;
    uplinkprot[protIdx].wait_restore_timer_CMD = WTR_CMD_STOP;
  }
  if (cmd == WTR_CMD_START)
  {
    uplinkprot[protIdx].wait_restore_timer_CMD = WTR_CMD_START;
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "done.");

  return (L7_SUCCESS);
}

/**
 * Uplink Protection FSM Transition
 * 
 * @author joaom (10/14/2013)
 * 
 * @param protIdx 
 * @param state 
 * @param __line__ 
 * 
 * @return L7_RC_t 
 */
L7_RC_t uplinkprotFsmTransition(L7_uint16 protIdx, PROT_STATE_t state, L7_uint32 __line__)
{
  PT_LOG_DEBUG(LOG_CTX_INTF, "Changing State (protIdx %d): Old State %u -> State %u [__line__ %d]", protIdx, uplinkprot[protIdx].state_machine, state, __line__);

  uplinkprot[protIdx].state_machine_h = uplinkprot[protIdx].state_machine;
  uplinkprot[protIdx].state_machine   = state;

  return (L7_SUCCESS);
}

/**
 * Switch Protection Port
 * 
 * @author joaom (10/14/2013)
 * 
 * @param protIdx 
 * @param port 
 * 
 * @return PROT_RC_t 
 */
L7_RC_t uplinkprotSwitchTo(L7_uint16 protIdx, PROT_PortType_t portType, PROT_LReq_t localRequest, L7_uint32 __line__)
{
  PT_LOG_DEBUG(LOG_CTX_INTF, "protIdx %d, portType %d, LReq %d [__line__ %d]", protIdx, portType, localRequest, __line__);

  uplinkprot[protIdx].activePortType_h = uplinkprot[protIdx].activePortType;

  if (portType == PORT_WORKING)
  {
    /* keep Status */
    uplinkprot[protIdx].activePortType = PORT_WORKING;

    /* Keep information about the last switchover cause */
    if (localRequest == PROT_LReq_LINK || localRequest == PROT_LReq_WTRExp)
    {
      if ((uplinkprot[protIdx].hAlarms[PORT_PROTECTION] & MASK_PORT_LINK))
      {
        uplinkprot[protIdx].lastSwitchoverCause = PROT_LReq_LINK;
      }
      else if ((uplinkprot[protIdx].hAlarms[PORT_PROTECTION] & MASK_PORT_BW))
      {
        uplinkprot[protIdx].lastSwitchoverCause = PROT_LReq_BW;
      }
    }
    else
    {
      uplinkprot[protIdx].lastSwitchoverCause = localRequest;
    }

    /* Block Protection LAG, and activate Working LAG */
    ptin_prot_select_intf(protIdx, PORT_WORKING);

    send_trap_alarm_sncp(protIdx, TRAP_ALARM_ETHITFPROT_SWITCH_TO_W, uplinkprot[protIdx].lastSwitchoverCause, 0, 0);
    PT_LOG_INFO(LOG_CTX_INTF, "Trap sent: Switchover to Working - protIdx %d, lastSwitchoverCause %u", protIdx, uplinkprot[protIdx].lastSwitchoverCause);

  }
  else if (portType == PORT_PROTECTION)
  {
    /* keep Status */
    uplinkprot[protIdx].activePortType = PORT_PROTECTION;

    /* Keep information about the last switchover cause */
    if (localRequest == PROT_LReq_LINK || localRequest == PROT_LReq_WTRExp)
    {
      if ((uplinkprot[protIdx].hAlarms[PORT_WORKING] & MASK_PORT_LINK))
      {
        uplinkprot[protIdx].lastSwitchoverCause = PROT_LReq_LINK;
      }
      else if ((uplinkprot[protIdx].hAlarms[PORT_WORKING] & MASK_PORT_BW))
      {
        uplinkprot[protIdx].lastSwitchoverCause = PROT_LReq_BW;
      }
    }
    else
    {
      uplinkprot[protIdx].lastSwitchoverCause = localRequest;
    }

    /* Block Working LAG, and activate Protection LAG */
    ptin_prot_select_intf(protIdx, PORT_PROTECTION);

    send_trap_alarm_sncp(protIdx, TRAP_ALARM_ETHITFPROT_SWITCH_TO_P, uplinkprot[protIdx].lastSwitchoverCause, 0, 0);
    PT_LOG_INFO(LOG_CTX_INTF, "Trap sent: Switchover to Protection - protIdx %d, lastSwitchoverCause %d", protIdx, uplinkprot[protIdx].lastSwitchoverCause);
  }

  PT_LOG_DEBUG(LOG_CTX_INTF, "Switching done to protIdx %d (lastSwitchoverCause %d)", protIdx, uplinkprot[protIdx].lastSwitchoverCause);

  return (L7_SUCCESS);
}

/**
 * Uplink Protection State Machine Update.
 *  
 * After starting an instance, send a query to the 
 * LCs to synchronize the FSM. 
 * 
 * @author joaom (11/12/2013)
 * 
 * @param protIdx 
 * 
 * @return PROT_RC_t 
 */
L7_RC_t uplinkprotInitStateMachine(L7_uint16 protIdx)
{
  L7_uint32 linkState[2];
  L7_uint32 bandwidth[2];
  L7_BOOL SF[2]={0,0}, SD[2]={0,0};

  PT_LOG_INFO(LOG_CTX_INTF, "Initializing machine (protIdx %d)", protIdx);

  /* Check link state of both LAGs */
  if (nimGetIntfLinkState(uplinkprot[protIdx].protParams.intIfNumW, &linkState[PORT_WORKING]) != L7_SUCCESS ||
      nimGetIntfLinkState(uplinkprot[protIdx].protParams.intIfNumP, &linkState[PORT_PROTECTION]) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error reading link state");
    return L7_FAILURE;
  }

  /* Check active bandwidth  */
  if (ptin_intf_active_bandwidth(uplinkprot[protIdx].protParams.intIfNumW, &bandwidth[PORT_WORKING]) != L7_SUCCESS ||
      ptin_intf_active_bandwidth(uplinkprot[protIdx].protParams.intIfNumP, &bandwidth[PORT_PROTECTION]) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error reading bandwidth values");
    return L7_FAILURE;
  }

  /* Check for link-down */
  /* Only evaluate links, if LINK alarm flag is enabled */
  if ((uplinkprot[protIdx].protParams.alarmsEnFlag & MASK_PORT_LINK))
  {
    if (linkState[PORT_WORKING] == L7_UP)
    {
      SF[PORT_WORKING] = L7_FALSE;
    }
    else
    {
      SF[PORT_WORKING] = L7_TRUE;
    }
    if (linkState[PORT_PROTECTION] == L7_UP)
    {
      SF[PORT_PROTECTION] = L7_FALSE;
    }
    else
    {
      SF[PORT_PROTECTION] = L7_TRUE;
    }
  }

  /* Check for Signal Degradation issues */
  /* Only evaluate them if SD alarm flag is enabled, and if both LAGs are linked-up */
  if ((uplinkprot[protIdx].protParams.alarmsEnFlag & MASK_PORT_BW) && !SF[PORT_WORKING] && !SF[PORT_PROTECTION]) 
  {
    if (bandwidth[PORT_WORKING] < bandwidth[PORT_PROTECTION])
    {
      SD[PORT_WORKING] = L7_TRUE;
      SD[PORT_PROTECTION] = L7_FALSE;
    }
    else if (bandwidth[PORT_WORKING] > bandwidth[PORT_PROTECTION])
    {
      SD[PORT_WORKING] = L7_FALSE;
      SD[PORT_PROTECTION] = L7_TRUE;
    }
  }

  PT_LOG_INFO(LOG_CTX_INTF, "SF={%u,%u} SD={%u,%u}",SF[PORT_WORKING],SF[PORT_PROTECTION],SD[PORT_WORKING],SD[PORT_PROTECTION]);

  /* Save new values */
  uplinkprot[protIdx].statusSF[PORT_WORKING]    = SF[PORT_WORKING];
  uplinkprot[protIdx].statusSF[PORT_PROTECTION] = SF[PORT_PROTECTION];

  uplinkprot[protIdx].statusSD[PORT_WORKING]    = SD[PORT_WORKING];
  uplinkprot[protIdx].statusSD[PORT_PROTECTION] = SD[PORT_PROTECTION];

  uplinkprot[protIdx].LAG_bw_active[PORT_WORKING]   = bandwidth[PORT_WORKING];
  uplinkprot[protIdx].LAG_bw_active[PORT_PROTECTION]= bandwidth[PORT_PROTECTION];

  /* Timer disabled */
  uplinkprot[protIdx].wait_restore_timer_CMD = WTR_CMD_STOP;
  uplinkprot[protIdx].wait_restore_timer = 0;

  /* If Protection interface is better, go to protection state */
  if (!SF[PORT_PROTECTION] && (SF[PORT_WORKING] || SD[PORT_WORKING]))
  {
    PT_LOG_INFO(LOG_CTX_INTF, "Going to PROT_STATE_Protection");

    uplinkprotFsmTransition(protIdx, PROT_STATE_Protection, __LINE__); 
    uplinkprot[protIdx].activePortType = PORT_PROTECTION;

    PT_LOG_INFO(LOG_CTX_INTF, "Selecting Protection LAG");

    ptin_prot_select_intf(protIdx, PORT_PROTECTION);
  }
  /* Otherwise, goto normal state */
  else
  {
    PT_LOG_INFO(LOG_CTX_INTF, "Going to PROT_STATE_Normal");

    /* Goto normal state */
    uplinkprotFsmTransition(protIdx, PROT_STATE_Normal, __LINE__); 
    uplinkprot[protIdx].activePortType = PORT_WORKING;

    PT_LOG_INFO(LOG_CTX_INTF, "Selecting Working LAG");

    /* In the beginning the active LAG is the Working one */
    ptin_prot_select_intf(protIdx, PORT_WORKING);
  }

  PT_LOG_INFO(LOG_CTX_INTF, "Machine initialized for protIndex %u", protIdx);

  return (L7_SUCCESS);
}

/**
 * Update machine state according to operator commands
 * 
 * @param protIdx 
 * 
 * @return L7_RC_t : Always L7_SUCCESS
 */
L7_RC_t uplinkProtApplyOperator(L7_uint protIdx)
{
  /* Priority 0: Clear command */
  if (operator_cmd[protIdx] & OPCMD_OC)
  {
    if (uplinkprot[protIdx].state_machine != PROT_STATE_Normal && uplinkprot[protIdx].state_machine != PROT_STATE_Protection)
    {
      /* If Protection interface is better, go to protection state */
      if ((!uplinkprot[protIdx].statusSF[PORT_PROTECTION]) &&
          ((uplinkprot[protIdx].statusSF[PORT_WORKING]) || (uplinkprot[protIdx].statusSD[PORT_WORKING])))
      {
        PT_LOG_INFO(LOG_CTX_INTF, "Going to PROT_STATE_Protection");
    
        uplinkprotFsmTransition(protIdx, PROT_STATE_Protection, __LINE__); 
        uplinkprot[protIdx].activePortType = PORT_PROTECTION;
    
        PT_LOG_INFO(LOG_CTX_INTF, "Selecting Protection LAG");
    
        ptin_prot_select_intf(protIdx, PORT_PROTECTION);
      }
      /* Otherwise, goto normal state */
      else
      {
        PT_LOG_INFO(LOG_CTX_INTF, "Going to PROT_STATE_Normal");
    
        /* Goto normal state */
        uplinkprotFsmTransition(protIdx, PROT_STATE_Normal, __LINE__); 
        uplinkprot[protIdx].activePortType = PORT_WORKING;
    
        PT_LOG_INFO(LOG_CTX_INTF, "Selecting Working LAG");
    
        /* In the beginning the active LAG is the Working one */
        ptin_prot_select_intf(protIdx, PORT_WORKING);
      }

      /* Command returns to NoRequest state */
      operator_cmd[protIdx] = OPCMD_NR;
      uplinkprot[protIdx].operator_cmd = OPCMD_NR;

      ptin_prot_timer_stop(protIdx);

      return L7_SUCCESS;
    }
  }
  /* Priority 1: Lockout command */
  else if (operator_cmd[protIdx] & OPCMD_LO)
  {
    /* Only apply to non default states */
    if (uplinkprot[protIdx].state_machine != PROT_STATE_WorkAdmin)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"=> PROT_STATE_WorkAdmin state (%u)", protIdx);
      uplinkprotSwitchTo(protIdx, PORT_WORKING, PROT_LReq_LO, __LINE__);
      uplinkprotFsmTransition(protIdx, PROT_STATE_WorkAdmin, __LINE__); 

      ptin_prot_timer_stop(protIdx);

      return L7_SUCCESS;
    }
  }
  /* Priority 2: Forced Switch command */
  else if (operator_cmd[protIdx] & OPCMD_FS)
  {
    if ((operator_switchToPortType[protIdx] == PORT_WORKING) &&
        (uplinkprot[protIdx].state_machine != PROT_STATE_WorkAdmin))
    {
      PT_LOG_INFO(LOG_CTX_INTF,"=> PROT_STATE_WorkAdmin state (%u)", protIdx);
      uplinkprotSwitchTo(protIdx, PORT_WORKING, PROT_LReq_FS, __LINE__);
      uplinkprotFsmTransition(protIdx, PROT_STATE_WorkAdmin, __LINE__); 

      ptin_prot_timer_stop(protIdx);

      return L7_SUCCESS;
    }
    else if ((operator_switchToPortType[protIdx] == PORT_PROTECTION) &&
             (uplinkprot[protIdx].state_machine != PROT_STATE_Protection))
    {
      PT_LOG_INFO(LOG_CTX_INTF,"=> PROT_STATE_ProtAdmin state (%u)", protIdx);
      uplinkprotSwitchTo(protIdx, PORT_PROTECTION, PROT_LReq_FS, __LINE__);
      uplinkprotFsmTransition(protIdx, PROT_STATE_ProtAdmin, __LINE__);

      ptin_prot_timer_stop(protIdx);

      return L7_SUCCESS;
    }
  }
  /* Priority 3: Manual Switch command with no SF */
  else if (operator_cmd[protIdx] & OPCMD_MS)
  {
    if ((operator_switchToPortType[protIdx] == PORT_WORKING) &&
        (uplinkprot[protIdx].state_machine != PROT_STATE_WorkAdmin) &&
        (!uplinkprot[protIdx].statusSF[PORT_WORKING]))
    {
      PT_LOG_INFO(LOG_CTX_INTF,"=> PROT_STATE_WorkAdmin state (%u)", protIdx);
      uplinkprotSwitchTo(protIdx, PORT_WORKING, PROT_LReq_MS, __LINE__);
      uplinkprotFsmTransition(protIdx, PROT_STATE_WorkAdmin, __LINE__); 

      ptin_prot_timer_stop(protIdx);

      return L7_SUCCESS;
    }
    else if ((operator_switchToPortType[protIdx] == PORT_PROTECTION) &&
             (uplinkprot[protIdx].state_machine != PROT_STATE_Protection) &&
             (!uplinkprot[protIdx].statusSF[PORT_PROTECTION]))
    {
      PT_LOG_INFO(LOG_CTX_INTF,"=> PROT_STATE_ProtAdmin state (%u)", protIdx);
      uplinkprotSwitchTo(protIdx, PORT_PROTECTION, PROT_LReq_MS, __LINE__);
      uplinkprotFsmTransition(protIdx, PROT_STATE_ProtAdmin, __LINE__);

      ptin_prot_timer_stop(protIdx);

      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/**
 * Process event and update state machine
 * 
 * @author mruas (5/10/2017)
 * 
 * @param intIfNum 
 * @param event 
 * 
 * @return L7_RC_t 
 */
L7_RC_t uplinkProtEventProcess(L7_uint32 intIfNum, L7_uint16 event)
{
  L7_uint8 i, portType;
  L7_uint32 intIfNumW, intIfNumP;
  L7_uint32 bandwidth[2] = {0, 0};
  L7_uint8  SF[2] = {0, 0}, SD[2] = {0, 0};
  PROT_STATE_t state_machine;

  PT_LOG_DEBUG(LOG_CTX_INTF, "Processing event %u for LAG intIfNum %u", event, intIfNum);

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  /* Run all instances to find where the input interface belongs*/
  for (i = 0; i < MAX_UPLINK_PROT; i++)
  {
    /* Skip inactive instances */
    if (!uplinkprot[i].admin)
      continue;

    /* Search for this interface, and break loop when found */
    if (uplinkprot[i].protParams.intIfNumW == intIfNum)
    {
      portType = PORT_WORKING;
      break;
    }
    else if (uplinkprot[i].protParams.intIfNumP == intIfNum)
    {
      portType = PORT_PROTECTION;
      break;
    }
  }
  /* If interface does not belong to any protection instance, leave */
  if (i >= MAX_UPLINK_PROT)
  {
    osapiSemaGive(ptin_prot_uplink_sem);
    PT_LOG_WARN(LOG_CTX_INTF, "No protection group found!");
    return L7_SUCCESS;
  }

  /* Extract useful parameters for further processing */
  state_machine = uplinkprot[i].state_machine;

  /* If state machine is disabled, there is nothing to do */
  if (state_machine == PROT_STATE_Disabled)
  {
    osapiSemaGive(ptin_prot_uplink_sem);
    PT_LOG_DEBUG(LOG_CTX_INTF, "State machine disabled");
    return L7_SUCCESS;
  }
  /* If machine state is in undefined state, initialize it! */
  if (state_machine == PROT_STATE_Undefined)
  {
    PT_LOG_INFO(LOG_CTX_INTF, "State machine undefined. initializing machine...");
    uplinkprotInitStateMachine(i);
    osapiSemaGive(ptin_prot_uplink_sem);
    PT_LOG_INFO(LOG_CTX_INTF, "State machine initialized");
    return L7_SUCCESS;
  }

  /* Protection interfaces */
  intIfNumW = uplinkprot[i].protParams.intIfNumW;
  intIfNumP = uplinkprot[i].protParams.intIfNumP;

  /* Recover previous state values */
  SF[PORT_WORKING]    = uplinkprot[i].statusSF[PORT_WORKING];
  SF[PORT_PROTECTION] = uplinkprot[i].statusSF[PORT_PROTECTION];

  SD[PORT_WORKING]    = uplinkprot[i].statusSD[PORT_WORKING];
  SD[PORT_PROTECTION] = uplinkprot[i].statusSD[PORT_PROTECTION];

  bandwidth[PORT_WORKING]   = uplinkprot[i].LAG_bw_active[PORT_WORKING];
  bandwidth[PORT_PROTECTION]= uplinkprot[i].LAG_bw_active[PORT_PROTECTION];

  /* Update state values and alarms */
  switch (event)
  {
    case L7_DOWN:
      /* Check if Link-down enable flag is active */
      if (!(uplinkprot[i].protParams.alarmsEnFlag & MASK_PORT_LINK))
      {
        SF[portType] = L7_FALSE;
        uplinkprot[i].hAlarms[portType] &= ~((L7_uint32) MASK_PORT_LINK);
      }
      else
      {
        /* Update Signal-fail status */
        SF[portType] = L7_TRUE;
        uplinkprot[i].hAlarms[portType] |= MASK_PORT_LINK;
        PT_LOG_DEBUG(LOG_CTX_INTF, "Link down event");
      }
      break;

    case L7_UP:
      /* Check if Link-down enable flag is active */
      if (!(uplinkprot[i].protParams.alarmsEnFlag & MASK_PORT_LINK))
      {
        SF[portType] = L7_FALSE;
        uplinkprot[i].hAlarms[portType] &= ~((L7_uint32) MASK_PORT_LINK);
      }
      else
      {
        /* Update Signal-fail status */
        SF[portType] = L7_FALSE;
        uplinkprot[i].hAlarms[portType] &= ~((L7_uint32) MASK_PORT_LINK);
        PT_LOG_DEBUG(LOG_CTX_INTF, "Link up event");
      }
      break;

    case L7_LAG_ACTIVE_MEMBER_ADDED:
    case L7_LAG_ACTIVE_MEMBER_REMOVED:
      /* Check if Signal-degrade enable flag is active */
      if (!(uplinkprot[i].protParams.alarmsEnFlag & MASK_PORT_BW))
      {
        uplinkprot[i].hAlarms[PORT_WORKING]    &= ~((L7_uint32) MASK_PORT_BW);
        uplinkprot[i].hAlarms[PORT_PROTECTION] &= ~((L7_uint32) MASK_PORT_BW);

        SD[PORT_WORKING]    = L7_FALSE;
        SD[PORT_PROTECTION] = L7_FALSE;

        break;
      }

      /* Check for available bandwidth for both interfaces */
      if (ptin_intf_active_bandwidth(intIfNum, &bandwidth[portType]) != L7_SUCCESS)
        bandwidth[portType] = 0;

      /* Update Signal-degrade status value */
      if (bandwidth[PORT_WORKING] < bandwidth[PORT_PROTECTION]) 
      {
        uplinkprot[i].hAlarms[PORT_WORKING]    |= MASK_PORT_BW;
        uplinkprot[i].hAlarms[PORT_PROTECTION] &= ~((L7_uint32) MASK_PORT_BW);

        SD[PORT_WORKING]    = L7_TRUE;
        SD[PORT_PROTECTION] = L7_FALSE;

        PT_LOG_DEBUG(LOG_CTX_INTF, "SD for Working Port");
      }
      else if (bandwidth[PORT_WORKING] > bandwidth[PORT_PROTECTION])
      {
        uplinkprot[i].hAlarms[PORT_WORKING]    &= ~((L7_uint32) MASK_PORT_BW);
        uplinkprot[i].hAlarms[PORT_PROTECTION] |= MASK_PORT_BW;

        SD[PORT_WORKING]    = L7_FALSE;
        SD[PORT_PROTECTION] = L7_TRUE;

        PT_LOG_DEBUG(LOG_CTX_INTF, "SD for Protection Port");
      }
      /* Bandwidth is the same on both sides */
      else
      {
        uplinkprot[i].hAlarms[PORT_WORKING]    &= ~((L7_uint32) MASK_PORT_BW);
        uplinkprot[i].hAlarms[PORT_PROTECTION] &= ~((L7_uint32) MASK_PORT_BW);

        SD[PORT_WORKING]    = L7_FALSE;
        SD[PORT_PROTECTION] = L7_FALSE;
      }
      break;

    default:
      osapiSemaGive(ptin_prot_uplink_sem);
      PT_LOG_DEBUG(LOG_CTX_INTF, "Unknown event: %u", event);
      return L7_SUCCESS;
  }

  /* Save new state values */
  uplinkprot[i].statusSF[PORT_WORKING]    = SF[PORT_WORKING];
  uplinkprot[i].statusSF[PORT_PROTECTION] = SF[PORT_PROTECTION];

  uplinkprot[i].statusSD[PORT_WORKING]    = SD[PORT_WORKING];
  uplinkprot[i].statusSD[PORT_PROTECTION] = SD[PORT_PROTECTION];

  uplinkprot[i].LAG_bw_active[PORT_WORKING]   = bandwidth[PORT_WORKING];
  uplinkprot[i].LAG_bw_active[PORT_PROTECTION]= bandwidth[PORT_PROTECTION];

  /* Save commands */
  uplinkprot[i].operator_cmd = operator_cmd[i];
  uplinkprot[i].operator_switchToPortType = operator_switchToPortType[i];

  /* If there are no errors, and there is no need to switch, stop existent timer */
  if (!SF[PORT_WORKING] && !SF[PORT_PROTECTION] && !SD[PORT_WORKING] && !SD[PORT_PROTECTION])
  {
    if (( uplinkprot[i].activePortType == PORT_WORKING) ||
        (!uplinkprot[i].protParams.revert2working))
    {
      PT_LOG_DEBUG(LOG_CTX_INTF, "Stopping counter");
      ptin_prot_timer_stop(i);
    }
  }

  /* Check if the current command should manage the state machine */
  if (uplinkProtApplyOperator(i) == L7_SUCCESS)
  {
    osapiSemaGive(ptin_prot_uplink_sem);
    PT_LOG_INFO(LOG_CTX_INTF, "Command updated state machine!");
    return L7_SUCCESS;
  }

  /* Update state values and alarms */
  switch (event)
  {
    /* Link-down event */
    case L7_DOWN:
      /* Not applicable when this flag is not enabled */
      if (!(uplinkprot[i].protParams.alarmsEnFlag & MASK_PORT_LINK))
      {
        PT_LOG_DEBUG(LOG_CTX_INTF,"Link-down event will not affect instance %u (Port Link flag is disabled)", i);
        break;
      }

      /* In case a link-down occurs, and a timer is already running to switch to this LAG (now in link-down), cancel it! */
      if ((uplinkprot[i].activePortType != portType) && (ptin_prot_timer_isrunning(i)))
      {
        PT_LOG_DEBUG(LOG_CTX_INTF,"Stoping timer for protIdx %u", i);
        ptin_prot_timer_stop(i);
      }

      /* link down @ working port */
      if (portType == PORT_WORKING)
      {
        if ((state_machine == PROT_STATE_Normal) && !SF[PORT_PROTECTION])
        {
          /* Normal state, and no SF in protection -> Instant switch to protection machine-state */
          PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal => PROT_STATE_Protection state (%u)", i);
          uplinkprotSwitchTo(i, PORT_PROTECTION, PROT_LReq_LINK, __LINE__);
          uplinkprotFsmTransition(i, PROT_STATE_Protection, __LINE__);

          ptin_prot_timer_stop(i);
        }
        else if ((state_machine == PROT_STATE_WorkAdmin) && (operator_cmd[i] == OPCMD_MS))
        {
          /* WorkAdmin state and in Manual-Switch mode... */
          if (!SF[PORT_PROTECTION])
          {
            /* If no SF in protection -> Instant switch to protection machine-state */
            PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal => PROT_STATE_Protection state (%u)", i);
            uplinkprotSwitchTo(i, PORT_PROTECTION, PROT_LReq_LINK, __LINE__);
            uplinkprotFsmTransition(i, PROT_STATE_Protection, __LINE__);
          }
          else
          {
            /* Instant switch to Normal machine-state */
            PT_LOG_INFO(LOG_CTX_INTF, "PROT_STATE_Normal => PROT_STATE_Normal state (%u)", i); 
            uplinkprotFsmTransition(i, PROT_STATE_Normal, __LINE__);
          }

          ptin_prot_timer_stop(i);
        }
      }
      /* link down @ protection */
      else
      {
        if (state_machine == PROT_STATE_Protection)
        {
          /* If in protection state -> Instant switch to Normal machine-state */
          PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal => PROT_STATE_Protection state (%u)", i);
          uplinkprotSwitchTo(i, PORT_WORKING, PROT_LReq_LINK, __LINE__);
          uplinkprotFsmTransition(i, PROT_STATE_Normal, __LINE__);

          ptin_prot_timer_stop(i);
        }
        else if ((state_machine == PROT_STATE_ProtAdmin) && (operator_cmd[i] == OPCMD_MS))
        {
          /* ProtAdmin state and in Manual-Switch mode -> Instant switch to Normal machine-state */
          PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal => PROT_STATE_Protection state (%u)", i);
          uplinkprotSwitchTo(i, PORT_WORKING, PROT_LReq_LINK, __LINE__);
          uplinkprotFsmTransition(i, PROT_STATE_Normal, __LINE__);

          ptin_prot_timer_stop(i);
        }
      }
      break; 

    /* Link-up event */
    case L7_UP:
      /* Not applicable when this flag is not enabled */
      if (!(uplinkprot[i].protParams.alarmsEnFlag & MASK_PORT_LINK))
      {
        PT_LOG_DEBUG(LOG_CTX_INTF,"Link-up event will not affect instance %u (Port Link flag is disabled)", i);
        break;
      }

      /* link up @ working port */
      if (portType == PORT_WORKING)
      {
        /* Check if revert (to working) should be applied */
        if ((state_machine == PROT_STATE_Protection) && (uplinkprot[i].protParams.revert2working) && (!SD[PORT_WORKING]))
        {
          if (!ptin_prot_timer_isrunning(i))
          {
            PT_LOG_DEBUG(LOG_CTX_INTF,"Going to start timer for protIdx %u", i);
            ptin_prot_timer_start(i, uplinkprot[i].protParams.WaitToRestoreTimer);
          }
        }
      }
      /* link up @ protection port */
      else
      {
        if ((state_machine == PROT_STATE_Normal) && SF[PORT_WORKING])
        {
          /* In normal machine-state and SF in working port -> Instant switch to protection */
          PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal => PROT_STATE_Protection state (%u)", i);
          uplinkprotSwitchTo(i, PORT_PROTECTION, PROT_LReq_LINK, __LINE__);
          uplinkprotFsmTransition(i, PROT_STATE_Protection, __LINE__);

          ptin_prot_timer_stop(i);
        }
      }

      break;

    /* Lag members update event */
    case L7_LAG_ACTIVE_MEMBER_ADDED:
    case L7_LAG_ACTIVE_MEMBER_REMOVED:
      /* Not applicable when this flag is not enabled */
      if (!(uplinkprot[i].protParams.alarmsEnFlag & MASK_PORT_BW))
      {
        PT_LOG_DEBUG(LOG_CTX_INTF,"Lag active member event will not affect instance %u (Port Link flag is disabled)", i);
        break;
      }

      /* Stopping timer if no alarm regarding to current por is detected */
      if ((!SF[portType] && !SD[portType]) && (uplinkprot[i].activePortType == portType) && (ptin_prot_timer_isrunning(i)))
      {
        PT_LOG_DEBUG(LOG_CTX_INTF, "protIdx=%u: SD[%u]=0... stoping timer", portType, i);
        ptin_prot_timer_stop(i);
      }

      if (state_machine == PROT_STATE_Normal)
      {
        /* Is protection port better? -> allocate timer to switch */
        if (SD[PORT_WORKING])
        {
          PT_LOG_DEBUG(LOG_CTX_INTF, "protIdx=%u: stateMachine=PROT_STATE_Normal, SD[W]=1", i);
          if (!ptin_prot_timer_isrunning(i))
          {
            PT_LOG_DEBUG(LOG_CTX_INTF,"Going to start timer for protIdx %u", i);
            ptin_prot_timer_start(i, uplinkprot[i].protParams.WaitToRestoreTimer); 
          }
        }
      }
      else if (state_machine == PROT_STATE_Protection)
      {
        /* Is Working port better? -> allocate timer to switch */
        if (SD[PORT_PROTECTION])
        {
          PT_LOG_DEBUG(LOG_CTX_INTF, "protIdx=%u: stateMachine=PROT_STATE_Protection, SD[P]=1", i);
          if (!ptin_prot_timer_isrunning(i))
          {
            PT_LOG_DEBUG(LOG_CTX_INTF,"Going to start timer for protIdx %u", i);
            ptin_prot_timer_start(i, uplinkprot[i].protParams.WaitToRestoreTimer); 
          }
        }
        /* Check if revert (to working) should be applied */
        else if ((!SD[PORT_WORKING]) && (!SF[PORT_WORKING]) && (uplinkprot[i].protParams.revert2working))
        {
          PT_LOG_DEBUG(LOG_CTX_INTF, "protIdx=%u: stateMachine=PROT_STATE_Protection, going to revert to working", i);
          if (!ptin_prot_timer_isrunning(i))
          {
            PT_LOG_DEBUG(LOG_CTX_INTF,"Going to start timer for protIdx %u", i);
            ptin_prot_timer_start(i, uplinkprot[i].protParams.WaitToRestoreTimer);
          }
        }
      }
      break;
  }

  osapiSemaGive(ptin_prot_uplink_sem);

  PT_LOG_DEBUG(LOG_CTX_INTF,"Finished processing event");

  return L7_SUCCESS;
}


/**
 * Uplink Protection Instance Processing
 * 
 * @author joaom (10/14/2013)
 * 
 * @param i 
 * 
 * @return L7_RC_t 
 */
L7_RC_t uplinkprotFsmProc(L7_uint16 i)
{
  PROT_LReq_t localRequest = PROT_LReq_NONE;
  L7_uint8    SF[2] = {0, 0}, SD[2] = {0, 0};
  L7_uint8    update_done = 0;

  L7_uint32 linkState[2];

  L7_uint32 intIfNumW, intIfNumP;
  L7_uint32 numMembers[2], bandwidth[2];

  /*--------------------------------------------------------------------------------------*/
  /*                                 Local Request logic                                  */
  /*--------------------------------------------------------------------------------------*/
  /* The different local requests that may be output from                                 */
  /* the Local Request logic are as follows:                                              */
  /*                                                                                      */
  /* o  Clear - if the operator cancels an active local administrative                    */
  /*    command, i.e., LO/FS/MS.                                                          */
  /*                                                                                      */
  /* o  Lockout of protection (LO) - if the operator requested to prevent                 */
  /*    switching data traffic to the protection path, for any purpose.                   */
  /*                                                                                      */
  /* o  Signal Fail (SF) - if any of the server-layer, control-plane, or                  */
  /*    OAM indications signaled a failure condition on either the                        */
  /*    protection path or one of the working paths.                                      */
  /*                                                                                      */
  /* o  Signal Degrade (SD) - if any of the server-layer, control-plane,                  */
  /*    or OAM indications signaled a degraded transmission condition on                  */
  /*    either the protection path or one of the working paths.  The                      */
  /*    determination and actions for SD are for further study and may                    */
  /*    appear in a separate document.  All references to SD input are                    */
  /*    placeholders for this extension.                                                  */
  /*                                                                                      */
  /* o  Clear Signal Fail (SFc) - if all of the server-layer, control-                    */
  /*    plane, or OAM indications are no longer indicating a failure                      */
  /*    condition on a path that was previously indicating a failure                      */
  /*    condition.                                                                        */
  /*                                                                                      */
  /* o  Forced Switch (FS) - if the operator requested that traffic be                    */
  /*    switched from one of the working paths to the protection path.                    */
  /*                                                                                      */
  /* o  Manual Switch (MS) - if the operator requested that traffic be                    */
  /*    switched from the working path to the protection path.  This is                   */
  /*    only relevant if there is no currently active fault condition or                  */
  /*    operator command.                                                                 */
  /*                                                                                      */
  /* o  WTR Expires (WTRExp) - generated by the WTR timer completing its                  */
  /*    period.                                                                           */
  /*--------------------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------------------*/
  /* Priority of Inputs:                                                                  */
  /* 1.   Clear (operator command)                                                        */
  /* 2.   Lockout of protection (operator command)                                        */
  /* 3.   Forced Switch (operator command)                                                */
  /* 4.   Signal Fail on protection (OAM / control-plane / server indication)             */
  /* 5.   Signal Fail on working (OAM / control-plane / server indication)                */
  /* 6.   Signal Degrade on working (OAM / control-plane / server indication)             */
  /* 7.   Clear Signal Fail/Degrade (OAM / control-plane / server indication)             */
  /* 8.   Manual Switch (operator command)                                                */
  /* 9.   WTR Expires (WTR timer)                                                         */
  /* 10.  No Request (default)                                                            */
  /*--------------------------------------------------------------------------------------*/

  /* Validate prot index */
  if (i >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid protIdx=%d", i);
    return L7_FAILURE;
  }
  /* Check if Entry is valid */
  if (!uplinkprot[i].admin)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "protIdx=%d empty", i);
    return L7_FAILURE;
  }

  intIfNumW = uplinkprot[i].protParams.intIfNumW;
  intIfNumP = uplinkprot[i].protParams.intIfNumP;

  /* Check link state of both LAGs */
  if (nimGetIntfLinkState(intIfNumW, &linkState[PORT_WORKING]) != L7_SUCCESS ||
      nimGetIntfLinkState(intIfNumP, &linkState[PORT_PROTECTION]) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

#if 0
  /* Update total number of members (LAG may be modified) */
  if (dot3adLagNumMembersGet(IntIfNumW, &uplinkprot[i].LAG_members_total[PORT_WORKING]) != L7_SUCCESS)
  return L7_FAILURE;
  if (dot3adLagNumMembersGet(IntIfNumP, &uplinkprot[i].LAG_members_total[PORT_PROTECTION]) != L7_SUCCESS)
  return L7_FAILURE;
  /* Update total bandwidth (LAG may be modified) */
  if (ptin_intf_max_bandwidth(IntIfNumW, &uplinkprot[i].LAG_bw_total[PORT_WORKING]) != L7_SUCCESS)
  return L7_FAILURE;
  if (ptin_intf_max_bandwidth(IntIfNumP, &uplinkprot[i].LAG_bw_total[PORT_PROTECTION]) != L7_SUCCESS)
  return L7_FAILURE;
#endif

  /* Check number of active members */
  if (dot3adLagNumActiveMembersGet(intIfNumW, &numMembers[PORT_WORKING]) != L7_SUCCESS) return L7_FAILURE;
  if (dot3adLagNumActiveMembersGet(intIfNumP, &numMembers[PORT_PROTECTION]) != L7_SUCCESS) return L7_FAILURE;

  /* Check active bandwidth  */
  if (ptin_intf_active_bandwidth(intIfNumW, &bandwidth[PORT_WORKING]) != L7_SUCCESS) return L7_FAILURE;
  if (ptin_intf_active_bandwidth(intIfNumP, &bandwidth[PORT_PROTECTION]) != L7_SUCCESS) return L7_FAILURE;

  /* Check for link-down */
  /* Only evaluate links, if LINK alarm flag is enabled */
  if ((uplinkprot[i].protParams.alarmsEnFlag & MASK_PORT_LINK))
  {
    if (linkState[PORT_WORKING] == L7_UP)
    {
      SF[PORT_WORKING] = L7_FALSE;
      uplinkprot[i].hAlarms[PORT_WORKING] &= ~((L7_uint32) MASK_PORT_LINK);
    }
    else
    {
      SF[PORT_WORKING] = L7_TRUE;
      uplinkprot[i].hAlarms[PORT_WORKING] |= MASK_PORT_LINK;
    }
    if (linkState[PORT_PROTECTION] == L7_UP)
    {
      SF[PORT_PROTECTION] = L7_FALSE;
      uplinkprot[i].hAlarms[PORT_PROTECTION] &= ~((L7_uint32) MASK_PORT_LINK);
    }
    else
    {
      SF[PORT_PROTECTION] = L7_TRUE;
      uplinkprot[i].hAlarms[PORT_PROTECTION] |= MASK_PORT_LINK;
    }
  }

  /* Check for Signal Degradation issues */
  /* Only evaluate them if SD alarm flag is enabled, and if both LAGs are linked-up */
  if ((uplinkprot[i].protParams.alarmsEnFlag & MASK_PORT_BW) && !SF[PORT_WORKING] && !SF[PORT_PROTECTION]) 
  {
    if (bandwidth[PORT_WORKING] < bandwidth[PORT_PROTECTION])
    {
      uplinkprot[i].hAlarms[PORT_WORKING]    |= MASK_PORT_BW;
      uplinkprot[i].hAlarms[PORT_PROTECTION] &= ~((L7_uint32) MASK_PORT_BW);

      SD[PORT_WORKING] = L7_TRUE;
      SD[PORT_PROTECTION] = L7_FALSE;
    }
    else if (bandwidth[PORT_WORKING] > bandwidth[PORT_PROTECTION])
    {
      uplinkprot[i].hAlarms[PORT_WORKING]    &= ~((L7_uint32) MASK_PORT_BW);
      uplinkprot[i].hAlarms[PORT_PROTECTION] |= MASK_PORT_BW;

      SD[PORT_WORKING] = L7_FALSE;
      SD[PORT_PROTECTION] = L7_TRUE;
    }
  }

  /* Save LAG members and bandwidth */
  uplinkprot[i].LAG_members_active[PORT_WORKING]    = numMembers[PORT_WORKING];
  uplinkprot[i].LAG_members_active[PORT_PROTECTION] = numMembers[PORT_PROTECTION];
  uplinkprot[i].LAG_bw_active[PORT_WORKING]         = bandwidth[PORT_WORKING];
  uplinkprot[i].LAG_bw_active[PORT_PROTECTION]      = bandwidth[PORT_PROTECTION];

  /*--------------------------------------------------------------------------------------*/
  /* Operator Command                                                                     */

  uplinkprot[i].operator_cmd = operator_cmd[i];
  uplinkprot[i].operator_switchToPortType = operator_switchToPortType[i];

  /* Priority 1, 2 & 3 */
  if ((uplinkprot[i].operator_cmd) && (localRequest == PROT_LReq_NONE))
  {
    if (uplinkprot[i].operator_cmd & OPCMD_OC)
    {
      localRequest = PROT_LReq_OC;
    } else if (uplinkprot[i].operator_cmd & OPCMD_LO)
    {
      localRequest = PROT_LReq_LO;
    } else if (uplinkprot[i].operator_cmd & OPCMD_FS)
    {
      localRequest = PROT_LReq_FS;
    }
  }

  /*--------------------------------------------------------------------------------------*/
  /* SF                                                                                   */
  if (localRequest == PROT_LReq_NONE)
  {
    /* Working */
    if (uplinkprot[i].activePortType == PORT_WORKING)
    {
      if (SF[PORT_WORKING] && !SF[PORT_PROTECTION])
      {
        localRequest = PROT_LReq_LINK;
        PT_LOG_TRACE(LOG_CTX_INTF, "protIdx=%d, SF[PORT_PROTECTION] set", i);
      }
    }
    /* Protection */
    else
    {
      if (SF[PORT_PROTECTION])
      {
        localRequest = PROT_LReq_LINK;
        PT_LOG_TRACE(LOG_CTX_INTF, "protIdx=%d, SF[PORT_PROTECTION] set", i);
      }
    }
  }

  /*--------------------------------------------------------------------------------------*/
  /* Operator Command                                                                     */
  if ((uplinkprot[i].operator_cmd & OPCMD_MS) && (localRequest == PROT_LReq_NONE))
  {
    /* Signal Fail & Signal Degrade > Manual Switch */
    if (!SF[PORT_WORKING] || !SF[PORT_PROTECTION])
    {
      localRequest = PROT_LReq_MS;
    }
  }

  /*--------------------------------------------------------------------------------------*/
  /* WTR                                                                                  */
  if (IS_TIMER_RUNNING(i))
  {

    /* WaitToRestore in 1min steps              */
    /* wait_restore_timer in 10 ms steps!       */

    /* Wait_restore_timer/60000 converts to ms! */

    uplinkprot[i].wait_restore_timer += PROT_CALL_PROC_MS;
    if ((uplinkprot[i].wait_restore_timer / 60000) >= uplinkprot[i].protParams.WaitToRestoreTimer)
    {
      localRequest = PROT_LReq_WTRExp;
      PT_LOG_TRACE(LOG_CTX_INTF, "protIdx=%d, WTRExp", i);

      uplinkprotWtr(i, WTR_CMD_STOP, __LINE__);
    }
  } else
  {
    /* Reset Counter */
    uplinkprot[i].wait_restore_timer = 0;
  }

  /*--------------------------------------------------------------------------------------*/
  /* Operation of States                                                                  */
  /*--------------------------------------------------------------------------------------*/

  if ((localRequest != PROT_LReq_NONE) && (uplinkprot[i].localRequest != localRequest))
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "protIdx=%d, localRequest: change from %u to %u", i, uplinkprot[i].localRequest, localRequest);
  }

  switch (uplinkprot[i].state_machine)
  {


  case PROT_STATE_Disabled:
    break;


  case PROT_STATE_Undefined:

    PT_LOG_INFO(LOG_CTX_INTF,"Initializing uplink protection %u", i);

    /* Update current LC State and jump to the correct UPLINK_STATE */
    uplinkprotInitStateMachine(i);
    break;


  case PROT_STATE_Normal:

    /* PROT_LReq_LO */
    if (localRequest == PROT_LReq_LO)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal: Going to PROT_STATE_WorkAdmin state (%u)", i);
      uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
      uplinkprotFsmTransition(i, PROT_STATE_WorkAdmin, __LINE__);
    }
    /* PROT_LReq_FS */
    else if (localRequest == PROT_LReq_FS)
    {
      /* PORT_PROTECTION */
      if (uplinkprot[i].operator_switchToPortType == PORT_PROTECTION)
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal: Going to PROT_STATE_ProtAdmin state (%u)", i);
        uplinkprotSwitchTo(i, PORT_PROTECTION, localRequest, __LINE__);
        uplinkprotFsmTransition(i, PROT_STATE_ProtAdmin, __LINE__);
      }
      /* PORT_WORKING */
      else
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal: Going to PROT_STATE_WorkAdmin state (%u)", i);
        uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
        uplinkprotFsmTransition(i, PROT_STATE_WorkAdmin, __LINE__);
      }
    }
    /* PROT_LReq_LINK */
    else if (localRequest == PROT_LReq_LINK)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal: Going to PROT_STATE_Protection state (%u)", i);
      uplinkprotSwitchTo(i, PORT_PROTECTION, localRequest, __LINE__);
      uplinkprotFsmTransition(i, PROT_STATE_Protection, __LINE__);
    }
    /* PROT_LReq_MS */
    else if (localRequest == PROT_LReq_MS)
    {
      if ((uplinkprot[i].operator_switchToPortType == PORT_PROTECTION) && !SF[PORT_PROTECTION])
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal: Going to PROT_STATE_ProtAdmin state (%u)", i);
        uplinkprotSwitchTo(i, PORT_PROTECTION, localRequest, __LINE__);
        uplinkprotFsmTransition(i, PROT_STATE_ProtAdmin, __LINE__);
      }

      if ((uplinkprot[i].operator_switchToPortType == PORT_WORKING) && !SF[PORT_WORKING])
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Normal: Going to PROT_STATE_WorkAdmin state (%u)", i);
        uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
        uplinkprotFsmTransition(i, PROT_STATE_WorkAdmin, __LINE__);
      }
    }
    /* PROT_LReq_WTRExp */
    else if (localRequest == PROT_LReq_WTRExp)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection: Timer expired... Going to PROT_STATE_Protection state (%u)", i);
      uplinkprotSwitchTo(i, PORT_PROTECTION, localRequest, __LINE__);
      uplinkprotFsmTransition(i, PROT_STATE_Protection, __LINE__);
    }
    else
    {
      /* All Other SHALL be ignored. */
    }

    /* Manage timer for revertibility */
    if (!IS_TIMER_RUNNING(i))
    {
      /* If Working LAG is better, start counter */
      if (SD[PORT_WORKING])
      {
        uplinkprotWtr(i, WTR_CMD_START, __LINE__);
      }
    }
    else
    {
      /* If Working LAG is worst, stop counter */
      if (SF[PORT_PROTECTION] || SD[PORT_PROTECTION])
      {
        uplinkprotWtr(i, WTR_CMD_STOP, __LINE__);
      }
    }

    break;


  case PROT_STATE_Protection:

    /* PROT_LReq_LO */
    if (localRequest == PROT_LReq_LO)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection: Going to PROT_STATE_WorkAdmin state (%u)", i);
      uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
      uplinkprotFsmTransition(i, PROT_STATE_WorkAdmin, __LINE__);

      uplinkprotWtr(i, WTR_CMD_STOP, __LINE__);
    }
    /* PROT_LReq_FS */
    else if (localRequest == PROT_LReq_FS)
    {
      /* PORT_PROTECTION */
      if (uplinkprot[i].operator_switchToPortType == PORT_PROTECTION)
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection: Going to PROT_STATE_ProtAdmin state (%u)", i);
        uplinkprotFsmTransition(i, PROT_STATE_ProtAdmin, __LINE__);
      }
      /* PORT_WORKING */
      else
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection: Going to PROT_STATE_WorkAdmin state (%u)", i);
        uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
        uplinkprotFsmTransition(i, PROT_STATE_WorkAdmin, __LINE__);
      }

      uplinkprotWtr(i, WTR_CMD_STOP, __LINE__);
    }
    /* PROT_LReq_LINK */
    else if (localRequest == PROT_LReq_LINK)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection: Going to PROT_STATE_Normal state (%u)", i);
      uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
      uplinkprotFsmTransition(i, PROT_STATE_Normal, __LINE__);

      uplinkprotWtr(i, WTR_CMD_STOP, __LINE__);
    }
    /* PROT_LReq_MS */
    else if (localRequest == PROT_LReq_MS)
    {
      if (uplinkprot[i].operator_switchToPortType == PORT_PROTECTION)
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection: Going to PROT_STATE_ProtAdmin state (%u)", i);
        uplinkprotFsmTransition(i, PROT_STATE_ProtAdmin, __LINE__);
      }

      if ((uplinkprot[i].operator_switchToPortType == PORT_WORKING) && !SF[PORT_WORKING])
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection: Going to PROT_STATE_WorkAdmin state (%u)", i);
        uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
        uplinkprotFsmTransition(i, PROT_STATE_WorkAdmin, __LINE__);
      }

      uplinkprotWtr(i, WTR_CMD_STOP, __LINE__);
    }
    /* PROT_LReq_WTRExp */
    else if (localRequest == PROT_LReq_WTRExp)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_Protection: Timer expired... Going to PROT_STATE_Normal state (%u)", i);
      uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
      uplinkprotFsmTransition(i, PROT_STATE_Normal, __LINE__);
    }
    else 
    {
      /* All Other SHALL be ignored. */
    }

    /* Manage timer for revertibility */
    if (!IS_TIMER_RUNNING(i))
    {
      /* If Working LAG is better, start counter */
      if ((uplinkprot[i].protParams.revert2working && !SF[PORT_WORKING] && !SD[PORT_WORKING]) ||
          (SD[PORT_PROTECTION]))
      {
        uplinkprotWtr(i, WTR_CMD_START, __LINE__);
      }
    }
    else
    {
      /* If Working LAG is worst, stop counter */
      if (SF[PORT_WORKING] || SD[PORT_WORKING])
      {
        uplinkprotWtr(i, WTR_CMD_STOP, __LINE__);
      }
    }

    break;


  case PROT_STATE_WorkAdmin:

    /* PROT_LReq_OC */
    if (localRequest == PROT_LReq_OC)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_WorkAdmin: Going to PROT_STATE_Normal state (%u)", i);
      uplinkprotFsmTransition(i, PROT_STATE_Normal, __LINE__);
      //uplinkprotCmdPortClear(i, __LINE__);
    }
    /* PROT_LReq_LINK */
    else if ( localRequest == PROT_LReq_LINK )
    {
      if ( (uplinkprot[i].operator_cmd & OPCMD_MS) && (uplinkprot[i].operator_switchToPortType == PORT_WORKING) )
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_WorkAdmin: Going to PROT_STATE_Protection state (%u)", i);
        uplinkprotSwitchTo(i, PORT_PROTECTION, localRequest, __LINE__);
        uplinkprotFsmTransition(i, PROT_STATE_Protection, __LINE__);
      }
      else
      {
        /* TODO: Re-enable link Tx */
      }
    }
else
    {
      /* All Other SHALL be ignored. */
    }

    break;


  case PROT_STATE_ProtAdmin:

    /* PROT_LReq_OC */
    if (localRequest == PROT_LReq_OC)
    {
      PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_ProtAdmin: Going to PROT_STATE_Protection state (%u)", i);
      uplinkprotFsmTransition(i, PROT_STATE_Protection, __LINE__);
      //uplinkprotCmdPortClear(i, __LINE__);
      break;
    }
    /* PROT_LReq_LINK */
    else if ( localRequest == PROT_LReq_LINK )
    {
      if ( (uplinkprot[i].operator_cmd & OPCMD_MS) && (uplinkprot[i].operator_switchToPortType == PORT_PROTECTION) )
      {
        PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_ProtAdmin: Going to PROT_STATE_Normal state (%u)", i);
        uplinkprotSwitchTo(i, PORT_WORKING, localRequest, __LINE__);
        uplinkprotFsmTransition(i, PROT_STATE_Normal, __LINE__);
      }
      else
      {
        /* TODO: Re-enable link Tx */
      }
    }
else
    {
      /* All Other SHALL be ignored. */
    }

    break;

    //Only for compile WARNING removal. Not used.
  case PROT_STATE_FailRecover:
    PT_LOG_INFO(LOG_CTX_INTF,"PROT_STATE_FailRecover (%u)", i);
    break;

  } //switch

  if (localRequest != PROT_LReq_NONE)
  {
    uplinkprot[i].localRequest = localRequest;
  }

  /* If both Working and Protection are alarmed refresh was done previously */
  if (!update_done)
  {
    uplinkprot[i].statusSF[PORT_WORKING]    = SF[PORT_WORKING];
    uplinkprot[i].statusSF[PORT_PROTECTION] = SF[PORT_PROTECTION];

    uplinkprot[i].statusSD[PORT_WORKING]    = SD[PORT_WORKING];
    uplinkprot[i].statusSD[PORT_PROTECTION] = SD[PORT_PROTECTION];
  }

  if ((uplinkprot[i].operator_cmd & OPCMD_OC))
  {
    PT_LOG_INFO(LOG_CTX_INTF,"Clear command final steps (%u)", i);
    uplinkprot[i].operator_cmd = OPCMD_NR;
    operator_cmd[i] =            OPCMD_NR;
    //uplinkprot[i].statusSF[PORT_WORKING] = !(SF[PORT_WORKING]);        /* Force a trigger */
    //uplinkprot[i].statusSF[PORT_PROTECTION] = !(SF[PORT_PROTECTION]);

    /* Reset WTR Timer */
    uplinkprotWtr(i, WTR_CMD_STOP, __LINE__);
  }

  return (L7_SUCCESS);
}


/**
 * State Machine Reset.
 *  
 * @param protIdx 
 * 
 * @return L7_RC_t 
 */
L7_RC_t uplinkprotResetStateMachine(L7_uint16 protIdx)
{
  PT_LOG_INFO(LOG_CTX_INTF, "Resetting protIdx %d", protIdx);

  /* Validate prot index */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid protIdx=%d", protIdx);
    return L7_FAILURE;
  }

  /* Check if Entry is valid */
  if (!uplinkprot[protIdx].admin)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "protIdx=%d empty", protIdx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  /* Stop any timer */
  ptin_prot_timer_stop(protIdx);

  /* Clear entry data */
  uplinkprot[protIdx].statusSF[PORT_WORKING] = uplinkprot[protIdx].statusSF[PORT_PROTECTION] = L7_FALSE;
  uplinkprot[protIdx].statusSD[PORT_WORKING] = uplinkprot[protIdx].statusSD[PORT_PROTECTION] = L7_FALSE;

  uplinkprot[protIdx].operator_cmd = OPCMD_NR;
  uplinkprot[protIdx].operator_switchToPortType = PORT_WORKING;

  uplinkprot[protIdx].localRequest = PROT_LReq_NONE;
  uplinkprot[protIdx].lastSwitchoverCause = PROT_LReq_NONE;

  uplinkprot[protIdx].state_machine   = PROT_STATE_Disabled;
  uplinkprot[protIdx].state_machine_h = PROT_STATE_Disabled;
  uplinkprot[protIdx].activePortType  = PORT_WORKING;

  uplinkprot[protIdx].wait_restore_timer_CMD = WTR_CMD_STOP;
  uplinkprot[protIdx].wait_restore_timer = 0;

  /* Initialize operator commands */
  operator_cmd[protIdx] = OPCMD_NR;
  operator_switchToPortType[protIdx] = PORT_WORKING;

  /* Block both LAGs */
  ptin_prot_select_intf(protIdx, PORT_ALL);
  uplinkprotInitStateMachine(protIdx);

  osapiSemaGive(ptin_prot_uplink_sem);

  PT_LOG_INFO(LOG_CTX_INTF, "Machine resetted for protIndex %u", protIdx);

  return (L7_SUCCESS);
}

/**
 * Apply a command to a protection group
 * 
 * @param protIdx : Protection group
 * @param cmd : Command
 * @param portType : port type
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_prot_uplink_command(L7_uint8 protIdx, PROT_OPCMD_t cmd, PROT_PortType_t portType)
{
  if (protIdx >= MAX_UPLINK_PROT)
  {
    return L7_FAILURE;
  }

  PT_LOG_INFO(LOG_CTX_INTF, "Applying command %u to portType %u (protIdx=%u)", cmd, portType, protIdx);

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  operator_cmd[protIdx] = cmd;
  operator_switchToPortType[protIdx] = portType;

  if (uplinkProtApplyOperator(protIdx) == L7_SUCCESS)
  {
    PT_LOG_INFO(LOG_CTX_INTF, "State machine was updated to %u state (active port is %u)", uplinkprot[protIdx].state_machine, uplinkprot[protIdx].activePortType);
  }
  else
  {
    PT_LOG_INFO(LOG_CTX_INTF, "State machine was not updated");
  }

  osapiSemaGive(ptin_prot_uplink_sem);

  return L7_SUCCESS;
}


/**
 * Create a protection group
 * 
 * @param protIdx : Protection group
 * @param intf1 : First interface
 * @param intf2 : Second interface
 * @param restore_time : Restoration time
 * @param operationMode : Should revert to working?
 * @param alarmFlagsEn : Alarm flags enable 
 * @param flags : Other flags
 * @param force : Force (re)creation if group already exists
 *  
 * @return L7_RC_t  
 */
L7_RC_t ptin_prot_uplink_create(L7_uint8 protIdx, ptin_intf_t *intf1, ptin_intf_t *intf2,
                                L7_uint32 restore_time, L7_uint8 operationMode, L7_uint32 alarmFlagsEn, L7_uint32 flags, L7_BOOL force)
{
  L7_uint32 intIfNum1, intIfNum2;

  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid index. Max index is %u", MAX_UPLINK_PROT);
    return L7_FAILURE;
  }

  /* Validate interfaces */
  if (intf1->intf_type != PTIN_EVC_INTF_PHYSICAL && intf1->intf_type != PTIN_EVC_INTF_LOGICAL)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid type for the first interface");
    return L7_FAILURE;
  }
  if (intf2->intf_type != PTIN_EVC_INTF_PHYSICAL && intf2->intf_type != PTIN_EVC_INTF_LOGICAL)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid type for the second interface");
    return L7_FAILURE;
  }
  if ((intf1->intf_type == PTIN_EVC_INTF_PHYSICAL && intf1->intf_id >= PTIN_SYSTEM_N_PORTS) ||
      (intf1->intf_type == PTIN_EVC_INTF_LOGICAL  && intf1->intf_id >= PTIN_SYSTEM_N_LAGS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid id for the first interface");
    return L7_FAILURE;
  }
  if ((intf2->intf_type == PTIN_EVC_INTF_PHYSICAL && intf2->intf_id >= PTIN_SYSTEM_N_PORTS) ||
      (intf2->intf_type == PTIN_EVC_INTF_LOGICAL  && intf2->intf_id >= PTIN_SYSTEM_N_LAGS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid id for the second interface");
    return L7_FAILURE;
  }

  /* If the protection group is already active, clear it */
  if (uplinkprot[protIdx].admin)
  {
    if (force)
    {
      if (ptin_prot_uplink_clear(protIdx) != L7_SUCCESS) 
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error clearing protection group (protIdx=%u)", protIdx);
        return L7_FAILURE;
      }
    }
    else
    {
      PT_LOG_WARN(LOG_CTX_INTF, "Protection group %u alrteady exists", protIdx);
      return L7_FAILURE;
    }
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  if (ptin_intf_ptintf2intIfNum(intf1, &intIfNum1) != L7_SUCCESS ||
      ptin_intf_ptintf2intIfNum(intf2, &intIfNum2) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_prot_uplink_sem);
    PT_LOG_ERR(LOG_CTX_INTF, "Error obtaining intIfNum values for ptin_intf %u/%u and %u/%u", intf1->intf_type, intf1->intf_id, intf2->intf_type, intf2->intf_id);
    return L7_FAILURE;
  }

  /* Clear entry */
  memset(&uplinkprot[protIdx], 0x00, sizeof(uplinkprot[protIdx]));
  /* Fill entry */

  uplinkprot[protIdx].protParams.revert2working     = operationMode;
  uplinkprot[protIdx].protParams.WaitToRestoreTimer = restore_time;
  uplinkprot[protIdx].protParams.alarmsEnFlag       = alarmFlagsEn;
  uplinkprot[protIdx].protParams.flags              = flags;
  uplinkprot[protIdx].protParams.HoldOffTimer       = 0;
  uplinkprot[protIdx].protParams.intIfNumW          = intIfNum1;
  uplinkprot[protIdx].protParams.intIfNumP          = intIfNum2;

  uplinkprot[protIdx].activePortType = uplinkprot[protIdx].activePortType_h = PORT_WORKING;
  uplinkprot[protIdx].state_machine  = uplinkprot[protIdx].state_machine_h  = PROT_STATE_Disabled;

  uplinkprot[protIdx].admin = L7_TRUE;

  operator_cmd[protIdx] = OPCMD_NR;
  operator_switchToPortType[protIdx] = PORT_WORKING;

  /* Set first port as ACTIVE */
  ptin_prot_select_intf(protIdx, PORT_ALL);
  uplinkprotInitStateMachine(protIdx);

  osapiSemaGive(ptin_prot_uplink_sem);

  PT_LOG_INFO(LOG_CTX_INTF, "Protection created. You still have to initiate the state machine.");

  return L7_SUCCESS;
}

/**
 * Update alarmFlagsEn value
 * 
 * @param protIdx 
 * @param alarmFlagsEn 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_prot_uplink_alarmFlagsEn_set(L7_uint8 protIdx, L7_uint32 alarmFlagsEn)
{
  /* Validate protIndex */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid index. Max index is %u", MAX_UPLINK_PROT);
    return L7_FAILURE;
  }
  /* If the protection group is already active, clear it */
  if (!uplinkprot[protIdx].admin)
  {
    PT_LOG_WARN(LOG_CTX_INTF, "Protection group %u does not exists", protIdx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  uplinkprot[protIdx].protParams.alarmsEnFlag = alarmFlagsEn;

  osapiSemaGive(ptin_prot_uplink_sem);

  return L7_SUCCESS;
}

/**
 * Update operationMode value
 * 
 * @param protIdx 
 * @param operationMode 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_prot_uplink_operationMode_set(L7_uint8 protIdx, L7_uint32 operationMode)
{
  /* Validate protIndex */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid index. Max index is %u", MAX_UPLINK_PROT);
    return L7_FAILURE;
  }
  /* If the protection group is already active, clear it */
  if (!uplinkprot[protIdx].admin)
  {
    PT_LOG_WARN(LOG_CTX_INTF, "Protection group %u does not exists", protIdx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  uplinkprot[protIdx].protParams.revert2working = operationMode;

  osapiSemaGive(ptin_prot_uplink_sem);

  return L7_SUCCESS;
}

/**
 * Update restore_time value
 * 
 * @param protIdx 
 * @param restore_time 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_prot_uplink_restoreTime_set(L7_uint8 protIdx, L7_uint32 restore_time)
{
  /* Validate protIndex */
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid index. Max index is %u", MAX_UPLINK_PROT);
    return L7_FAILURE;
  }
  /* If the protection group is already active, clear it */
  if (!uplinkprot[protIdx].admin)
  {
    PT_LOG_WARN(LOG_CTX_INTF, "Protection group %u does not exists", protIdx);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  uplinkprot[protIdx].protParams.WaitToRestoreTimer = restore_time;

  osapiSemaGive(ptin_prot_uplink_sem);

  return L7_SUCCESS;
}

/**
 * Remove protection group
 * 
 * @param protIdx 
 *  
 * @return L7_RC_t  
 */
L7_RC_t ptin_prot_uplink_clear(L7_uint8 protIdx)
{
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid index. Max index is %u", MAX_UPLINK_PROT);
    return L7_FAILURE;
  }

  if (!uplinkprot[protIdx].admin)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Nothing to do... already disabled!");
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  /* Stop any timer */
  ptin_prot_timer_stop(protIdx);

  /* Disable state machine */
  uplinkprotFsmTransition(protIdx, PROT_STATE_Disabled, __LINE__);

  /* Initialize operator commands */
  operator_cmd[protIdx] = OPCMD_NR;
  operator_switchToPortType[protIdx] = PORT_WORKING;

  /* Activate both LAGs */
  ptin_prot_select_intf(protIdx, PORT_ALL);

  /* Disable entry */
  uplinkprot[protIdx].admin = L7_FALSE;

  /* Clear entry */
  memset(&uplinkprot[protIdx], 0x00, sizeof(uplinkprot[protIdx]));

  osapiSemaGive(ptin_prot_uplink_sem);

  PT_LOG_INFO(LOG_CTX_INTF, "ProtIndex %u initialized!\r\n", protIdx);

  return L7_SUCCESS;
}

/**
 * Get protection group configuration
 * 
 * @param protIdx 
 * @param config
 *  
 * @return L7_RC_t  
 */
L7_RC_t ptin_prot_uplink_config_get(L7_uint8 protIdx, uplinkprotParams_st *config)
{
  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid index. Max index is %u", MAX_UPLINK_PROT);
    return L7_FAILURE;
  }

  if (!uplinkprot[protIdx].admin)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Nothing to do... already disabled!");
    return L7_FAILURE;
  }

  if (config == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid pointer");
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  *config = uplinkprot[protIdx].protParams;

  osapiSemaGive(ptin_prot_uplink_sem);

  return L7_SUCCESS;
}

/**
 * Get protection group status
 * 
 * @param protIdx 
 * @param status 
 *  
 * @return L7_RC_t  
 */
L7_RC_t ptin_prot_uplink_status(L7_uint8 protIdx, uplinkprot_status_st *status)
{
  L7_uint32 timeLeft;

  if (protIdx >= MAX_UPLINK_PROT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid index. Max index is %u", MAX_UPLINK_PROT);
    return L7_FAILURE;
  }

  if (!uplinkprot[protIdx].admin)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Nothing to do... already disabled!");
    return L7_FAILURE;
  }

  if (status == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid pointer");
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  status->activePortType      = uplinkprot[protIdx].activePortType;
  status->alarmsW             = uplinkprot[protIdx].hAlarms[PORT_WORKING];
  status->alarmsP             = uplinkprot[protIdx].hAlarms[PORT_PROTECTION];
  status->alarmsMaskW         = uplinkprot[protIdx].protParams.alarmsEnFlag;
  status->alarmsMaskP         = uplinkprot[protIdx].protParams.alarmsEnFlag;
  status->lastSwitchoverCause = uplinkprot[protIdx].lastSwitchoverCause;

  /* Get time left */
  if (ptin_prot_timer_timeout_get(protIdx, &timeLeft) == L7_SUCCESS)
  {
    status->WaitToRestoreTimer = timeLeft;
  }
  else
  {
    status->WaitToRestoreTimer = 0;
  }

  status->HoldOffTimer = 0;

  osapiSemaGive(ptin_prot_uplink_sem);

  return L7_SUCCESS;
}


void ptin_prot_uplink_dump(L7_uint8 protIdx)
{
  if (protIdx >= MAX_UPLINK_PROT)
  {
    printf("Invalid index. Max index is %u\r\n",MAX_UPLINK_PROT);
    return;
  }

  osapiSemaTake(ptin_prot_uplink_sem, L7_WAIT_FOREVER);

  printf("ProtIndex %u information\r\n", protIdx);
  printf(" admin = %u\r\n", uplinkprot[protIdx].admin);
  printf(" protParams.revert2working    = %u\r\n", uplinkprot[protIdx].protParams.revert2working);
  printf(" protParams.HoldOffTimer      = %u\r\n", uplinkprot[protIdx].protParams.HoldOffTimer);
  printf(" protParams.WaitToRestoreTimer= %u\r\n", uplinkprot[protIdx].protParams.WaitToRestoreTimer);
  printf(" protParams.alarmsEnFlag      = 0x%08x\r\n", uplinkprot[protIdx].protParams.alarmsEnFlag);
  printf(" protParams.intIfNumW         = %u\r\n", uplinkprot[protIdx].protParams.intIfNumW);
  printf(" protParams.intIfNumP         = %u\r\n", uplinkprot[protIdx].protParams.intIfNumP);
  printf(" activePortType   = %u\r\n", uplinkprot[protIdx].activePortType);
  printf(" activePortType_h = %u\r\n", uplinkprot[protIdx].activePortType_h);
  printf(" hAlarms  = { 0x%02x 0x%02x }\r\n", uplinkprot[protIdx].hAlarms[0], uplinkprot[protIdx].hAlarms[1]);
  printf(" statusSF = { %u %u }\r\n", uplinkprot[protIdx].statusSF[0], uplinkprot[protIdx].statusSF[1]);
  printf(" statusSD = { %u %u }\r\n", uplinkprot[protIdx].statusSD[0], uplinkprot[protIdx].statusSD[1]);
  printf(" LAG_members_total  = { %6u %6u }\r\n", uplinkprot[protIdx].LAG_members_total[0], uplinkprot[protIdx].LAG_members_total[1]);
  printf(" LAG_bw_total       = { %6u %6u }\r\n", uplinkprot[protIdx].LAG_bw_total[0], uplinkprot[protIdx].LAG_bw_total[1]);
  printf(" LAG_members_active = { %6u %6u }\r\n", uplinkprot[protIdx].LAG_members_active[0], uplinkprot[protIdx].LAG_members_active[1]);
  printf(" LAG_bw_active      = { %6u %6u }\r\n", uplinkprot[protIdx].LAG_bw_active[0], uplinkprot[protIdx].LAG_bw_active[1]);
  printf(" operatorCMD              = %u\r\n", uplinkprot[protIdx].operator_cmd);
  printf(" operator_switchToPortType= %u\r\n", uplinkprot[protIdx].operator_switchToPortType);
  printf(" localRequest             = %u\r\n", uplinkprot[protIdx].localRequest);
  printf(" lastSwitchoverCause      = %u\r\n", uplinkprot[protIdx].lastSwitchoverCause);
  printf(" state_machine    = %u\r\n", uplinkprot[protIdx].state_machine);
  printf(" state_machine_h  = %u\r\n", uplinkprot[protIdx].state_machine_h);
  printf(" wait_restore_timer_CMD = %u\r\n", uplinkprot[protIdx].wait_restore_timer_CMD);
  printf(" wait_restore_timer     = %u\r\n", uplinkprot[protIdx].wait_restore_timer);
  printf("operator_cmd              => %u\r\n", operator_cmd[protIdx]);
  printf("operator_switchToPortType => %u\r\n", operator_switchToPortType[protIdx]);

  osapiSemaGive(ptin_prot_uplink_sem);

  printf("Done!\r\n");
}

