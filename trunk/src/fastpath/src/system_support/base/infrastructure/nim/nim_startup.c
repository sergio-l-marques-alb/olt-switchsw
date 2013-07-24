/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_startup.c
*
* @purpose    The functions that handle the nim startup notification
*
* @component  NIM
*
* @comments   none
*
* @create     09/24/2008
*
* @author     bradyr
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "avl_api.h"
#include "nim_util.h" /* needed for NIM_LOG_MSG */
#include "nim_startup.h"
#include "osapi_trace.h"
#include <string.h>

static avlTree_t              nimStartUpTree  = { 0 };
static void                   *nimStartUpSema = L7_NULL;
static void                   *nimStartUpCbSema = L7_NULL;
static void                   *nimStartUpEvSema = L7_NULL;
static L7_COMPONENT_IDS_t     nimStartUpCompId = L7_FIRST_COMPONENT_ID;

/* Macros for protecting the AVL tree during operations */
#define NIM_STARTUP_CRIT_SEC_ENTER()  \
{   \
    osapiSemaTake(nimStartUpSema,L7_WAIT_FOREVER);  \
}

#define NIM_STARTUP_CRIT_SEC_EXIT()  \
{   \
    osapiSemaGive(nimStartUpSema);  \
}

/*********************************************************************
*
* @purpose  Compare two Startup AVL keys and determine Greater, Less, or Equal
*
* @param    a    a void pointer to an AVL key comprised of a Startup structure
* @param    b    a void pointer to an AVL key comprised of a Startup structure
* @param    len  the length of the key in bytes (unused)
*
* @returns  1 if a > b
* @returns  -1 if a < b
* @returns  0 if a == b
*
* @notes    This function is used as a custom comparator in the
*           nimStartUpTreeData AVL tree. The 'len'
*           parameter is unused because the length of Startup data is
*           known, however, it is required for compatibility with
*           the comparator type definition.
*
* @end
*********************************************************************/
static int nimStartupCompare(const void *a, const void *b, size_t len)
{
  nimStartUpTreeData_t *sua = (nimStartUpTreeData_t *)a;
  nimStartUpTreeData_t *sub = (nimStartUpTreeData_t *)b;

  /* Sort priority highest -> lowest value */
  if (sua->priority > sub->priority)
  {
    return -1;
  }
  if (sua->priority < sub->priority)
  {
    return 1;
  }
  if (sua->componentId < sub->componentId)
  {
    return -1;
  }
  if (sua->componentId > sub->componentId)
  {
    return 1;
  }
  return 0;
}

/*********************************************************************
* @purpose  Allocate the memory for the ifIndex AVL tree
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimStartUpPhaseOneInit()
{
  L7_RC_t   rc = L7_SUCCESS;

  do
  {

    if (avlAllocAndCreateAvlTree(&nimStartUpTree,
                                 L7_NIM_COMPONENT_ID,
                                 L7_LAST_COMPONENT_ID,
                                 sizeof(nimStartUpTreeData_t),
                                 0x10, nimStartupCompare,
                                 sizeof(L7_uint32)*2) != L7_SUCCESS)
    {
      NIM_LOG_ERROR("NIM: Unable to allocate resources\n");
      break; /* goto while */
    }

    nimStartUpSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
    nimStartUpCbSema = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY,
                                        OSAPI_SEM_EMPTY);
    nimStartUpEvSema = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY,
                                        OSAPI_SEM_EMPTY);

    if (nimStartUpSema == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: unable to create the ifIndex Sema\n");
    }

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Create an StartUp
*
* @param    componentId   @b{(input)}   Component ID of startup function
* @param    priority      @b{(input)}   priority to execute startup function
* @param    startupFcn    @b{(input)}   Function pointer to startup routine
*
* @notes
*
* @end
*
*********************************************************************/
void nimStartUpCreate(L7_COMPONENT_IDS_t componentId,
                      L7_uint32 priority,
                      StartupNotifyFcn startupFcn)
{
  nimStartUpTreeData_t data;
  nimStartUpTreeData_t *pData;

  data.componentId = componentId;
  data.priority = priority;
  data.startupFunction = startupFcn;

  NIM_STARTUP_CRIT_SEC_ENTER();

  pData = avlInsertEntry(&nimStartUpTree, &data);

  NIM_STARTUP_CRIT_SEC_EXIT();

  if (pData != L7_NULL)
  {
    NIM_LOG_MSG("NIM: startup function for %d not added to the AVL tree\n",
                componentId);
  }

  return;
}

/*********************************************************************
* @purpose  Delete an StartUp
*
* @param    componentId   @b{(input)}   Component ID of startup function
* @param    priority      @b{(input)}   priority to execute startup function
*
* @notes
*
* @end
*
*********************************************************************/
void nimStartUpDelete(L7_COMPONENT_IDS_t componentId,
                      L7_uint32 priority)
{
  nimStartUpTreeData_t *pData;
  nimStartUpTreeData_t data;

  data.componentId = componentId;
  data.priority = priority;

  NIM_STARTUP_CRIT_SEC_ENTER();

  pData = avlDeleteEntry(&nimStartUpTree, &data);

  NIM_STARTUP_CRIT_SEC_EXIT();

  if (pData == L7_NULL)
  {
    NIM_LOG_MSG("NIM: startup %d not found, cannot delete it\n",componentId);
  }

  return;
}

/*********************************************************************
* @purpose  Find the first startup function
*
* @param    pRetData     @b{(input)}  pointer to return data
*
* @returns  L7_SUCCESS if an entry exists
* @returns  L7_FAILURE if no entry exists
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimStartUpFirstGet(nimStartUpTreeData_t *pRetData)
{
  L7_RC_t rc = L7_FAILURE;
  nimStartUpTreeData_t searchData;
  nimStartUpTreeData_t *pData;

  searchData.priority = 0xffffffff;
  searchData.componentId = 0;

  NIM_STARTUP_CRIT_SEC_ENTER();

  pData = avlSearchLVL7 (&nimStartUpTree, &searchData, AVL_NEXT);

  NIM_STARTUP_CRIT_SEC_EXIT();

  if (pData != L7_NULL)
  {
    memcpy(pRetData, pData, sizeof(nimStartUpTreeData_t));
    rc = L7_SUCCESS;

  }
  return rc;
}

/*********************************************************************
* @purpose  Find the next startup function
*
* @param    pRetData     @b{(input)}  pointer to search/return data
*
* @returns  L7_SUCCESS if a next entry exists
* @returns  L7_FAILURE if no next entry exists
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimStartUpNextGet(nimStartUpTreeData_t *pRetData)
{
  L7_RC_t rc = L7_FAILURE;
  nimStartUpTreeData_t *pData;

  NIM_STARTUP_CRIT_SEC_ENTER();

  pData = avlSearchLVL7 (&nimStartUpTree, pRetData, AVL_NEXT);

  NIM_STARTUP_CRIT_SEC_EXIT();

  if (pData != L7_NULL)
  {
    memcpy(pRetData, pData, sizeof(nimStartUpTreeData_t));
    rc = L7_SUCCESS;

  }
  return rc;
}

/*********************************************************************
*
* @purpose  Invoke startup callbacks for registered components
*
* @param    phase     @b{(input)}  startup phase - create or activate
*
* @returns  void
*
* @notes    Startup's are invoked serially, waiting for each one to
*           complete before invoking the next component's startup.
*
* @end
*********************************************************************/
void nimStartupCallbackInvoke(NIM_STARTUP_PHASE_t phase)
{
  L7_RC_t              rc;
  nimStartUpTreeData_t startupData;
  nimPdu_t             startupMsg;

#ifdef L7_TRACE_ENABLED
  osapiTraceEvents     traceEventBegin = (phase == NIM_INTERFACE_CREATE_STARTUP) ? 
    L7_TRACE_EVENT_NIM_CREATE_STARTUP_START :
    L7_TRACE_EVENT_NIM_ACTIVATE_STARTUP_START;
  osapiTraceEvents     traceEventEnd = (phase == NIM_INTERFACE_CREATE_STARTUP) ? 
    L7_TRACE_EVENT_NIM_CREATE_STARTUP_END :
    L7_TRACE_EVENT_NIM_ACTIVATE_STARTUP_END;
#endif

  OSAPI_TRACE_EVENT(traceEventBegin, (L7_uchar8 *)L7_NULLPTR, 0);

  rc = nimStartUpFirstGet(&startupData);
  startupMsg.msgType = START_MSG;

  while (rc == L7_SUCCESS)
  {
    if (nimCtlBlk_g->nimPhaseStatus != L7_CNFGR_STATE_E)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
          "Terminating %s startup callbacks. No longer in EXECUTE state.",
          phase == NIM_INTERFACE_CREATE_STARTUP ? "CREATE" : "ACTIVATE");
      return;
    }
    nimStartUpCompId = startupData.componentId;

    /* Send the startup function to NIM to execute on NIM's task */
    startupMsg.data.nimStartMsg.componentId = nimStartUpCompId;
    startupMsg.data.nimStartMsg.startupFunction = startupData.startupFunction;
    startupMsg.data.nimStartMsg.phase = phase;

    /* send the message to NIM_QUEUE */
    if (osapiMessageSend(nimCtlBlk_g->nimMsgQueue, (void *)&startupMsg,
                         (L7_uint32)sizeof(nimPdu_t), L7_WAIT_FOREVER,
                         L7_MSG_PRIORITY_NORM ) == L7_ERROR)
    {
      NIM_LOG_MSG("NIM: failed to send START message to NIM message Queue.\n");
      rc = L7_FAILURE;
    }
    else
    {
      /* Wait until startup has completed */
      osapiSemaTake(nimStartUpEvSema, L7_WAIT_FOREVER);
    }

    rc = nimStartUpNextGet(&startupData);
  }
  OSAPI_TRACE_EVENT(traceEventEnd, (L7_uchar8 *)L7_NULLPTR, 0 );
}

/*********************************************************************
*
* @purpose  Status callback from components to NIM for startup complete
*           Notifications
*
* @param    componentId  @b{(input)} component ID of the caller.
*
* @returns  void
*
* @notes    At the conclusion of processing a startup Event, each component
*           must call this function.
*
* @end
*********************************************************************/
extern void nimStartupEventDone(L7_COMPONENT_IDS_t componentId)
{
    if (componentId == nimStartUpCompId)
    {
      nimStartUpCompId = L7_FIRST_COMPONENT_ID;
      osapiSemaGive(nimStartUpCbSema);
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_NIM_COMPONENT_ID,
              "Event Done received for component %d, expecting %d\n",
              componentId, nimStartUpCompId);
    }
}

/*********************************************************************
* @purpose  Debug support - indicate which component's startup callback
*           is outstanding.
*
* @param    void
*
* @returns  component ID
*
* @notes
*
* @end
*********************************************************************/
extern L7_COMPONENT_IDS_t nimStartupEventInProcess(void)
{
  return nimStartUpCompId;
}

/*********************************************************************
*
* @purpose  Waits for the component to complete its STARTUP processing,
*           then gives the nimStartUpEvSema signaling cardmgr to proceed
*           to the next component.
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
extern void nimStartupEventWait(void)
{
  osapiSemaTake(nimStartUpCbSema, L7_WAIT_FOREVER);
  osapiSemaGive(nimStartUpEvSema);
}
