
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   l7apptimer.c
*
* @purpose    Application Timer library functions
*
* @component  System support utilities
*
* @comments   none
*
* @create     06-Mar-06
*
* @author     Daniel M.V.
* @end
*
**********************************************************************/
#include "l7apptimer_api.h"
#include "osapi.h"
#include "string.h"
#include "buff_api.h"
#include "l7sll_api.h"
#include "l3_mcast_commdefs.h"
#include "l7_mcast_api.h"
#include "l7utils_api.h"
/*******************************************************************************
**                        Structure Definitions                               **
*******************************************************************************/

typedef struct appTmrCtrlBlk
{
  L7_COMPONENT_IDS_t       compId;
  L7_uint32                bufferPoolId;
  L7_APP_TMR_GRAN_TYPE_t   type;
  void                     *semId;
  L7_sll_t                 tmrList;
  osapiTimerDescr_t        *pSysTimer;
  L7_app_tmr_dispatcher_fn dispatchFn;
  void                     *pParam;
  struct appTmrCtrlBlk     *pSelf;   /* For sanity Sakes */
} appTmrCtrlBlk_t;


/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  To compare the two keys given based on the timer expiry value.
*
* @param    data1  @b{(input)}The pointer to the first key.
* @param    data2  @b{(input)}The pointer to the second key.
* @param    keyLen @b{(input)}The length of the key to be compared.
*
* @returns  Less than 0, if node1 < node 2.
* @returns  Zero, if node1 == node2
* @returns  More than 0, if node1 > node2.
*
* @notes    Key Length is ignored for this comparison.
*
* @end
*
*********************************************************************/
static
L7_int32 appTimerCompare(void *data1,
                         void *data2,
                         L7_uint32 keyLen)
{
  timerNode_t *pEntry1, *pEntry2;

  if(data1 == L7_NULLPTR || data2 == L7_NULLPTR)
    return 1;

  pEntry1 = (timerNode_t *)data1;
  pEntry2 = (timerNode_t *)data2;

  return (pEntry1->expiryTime - pEntry2->expiryTime);
}

/*********************************************************************
*
* @purpose  To destroy the node associated with the given node
*
* @param    node @b{(input)}The pointer to the node that is to be deleted.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    There is currently no need for this.
*
* @end
*
*********************************************************************/
static
L7_RC_t appTimerDestroy(L7_sll_member_t *node)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Function invoked by the system timer tick
*
* @param    param1 @b{(input)}First parameter from the system tick callback.
*                             This parameter is essentially the Application Timer instance's
*                             control block.
* @param    param2 @b{(input)}Second parameter from the system tick callback.
*                             This parameter is the user parameter registered with the library
*                    during instantiation of the timer module.
*
* @returns  None.
*
* @notes    I do not envisage that the control block would be freed very often.
*           So, acquiring the semaphore lock may not be needed while processing
*           this call. This would reduce the latency involved in processing the
*           system level timer.
*           In case the timer instance was actually deleted, then while really
*           processing the tick, we'll throw the call away.
*
* @end
*
*********************************************************************/
static
void appTimerTick(L7_uint32 param1, L7_uint32 param2)
{
  
  appTmrCtrlBlk_t          *pCtrlBlk;
  L7_app_tmr_dispatcher_fn pFunc;

  /* Some basic sanity checks */
  pCtrlBlk = (appTmrCtrlBlk_t *)param1;
  if(pCtrlBlk == L7_NULLPTR)
    return;
  if(pCtrlBlk->pSelf!= pCtrlBlk)
    return;

  /* Retrieve the Dispatch callback function */
  pFunc = pCtrlBlk->dispatchFn;

  /* Make sure that the control block is still valid;
     indirectly verifying the authenticity of pFunc */
  if(pCtrlBlk->pSelf != pCtrlBlk)
    return;

  /* Invoke the dispatcher function */
  pFunc((L7_APP_TMR_CTRL_BLK_t)pCtrlBlk, (void *)param2);
}

/*********************************************************************
*
* @purpose  Initialize/Instantiate an Application Timer Module
*
* @param    compId       @b{(input)}Component ID of the owner
* @param    dispatchFn   @b{(input)}Function dispatcher that would off-load processing
*                                   to the application context
* @param    pParam       @b{(input)}Opaque user parameter that is to be returned in the tick
*                                   dispatcher for the application's use
* @param    timerType    @b{(input)}Granularity of the timer tick (applies to all timers
*                                   associated with this instance)
* @param    buffPoolID   @b{(input)}Buffer Pool ID to be associated with this instance
*
* @returns  appTmrCtrlBlk    Opaque Control Block for the timer module instance, if successful
* @returns  L7_NULLPTR       If timer module instantiation failed
*
* @notes    Each application timer instance acts as a base timer tick. This is
*           equivalent to a system timer (eg. osapiTimer). So, it is highly
*           advisable that each application module/component instantiate
*           only one application timer using this routine to avoid depleting
*           (or overloading) the system timer. Timing within the application
*           is achieved by hooking the expiry function along with the timeout
*           to this basic timer instance by using the appTimerAdd() routine.
*
* @end
*
*********************************************************************/
L7_APP_TMR_CTRL_BLK_t appTimerInit(L7_COMPONENT_IDS_t       compId,
                                   L7_app_tmr_dispatcher_fn dispatchFn,
                                   void                     *pParam,
                                   L7_APP_TMR_GRAN_TYPE_t   timerType,
                                   L7_uint32                buffPoolID)
{
  appTmrCtrlBlk_t *pCtrlBlk = L7_NULLPTR;

  /* Some sanity checks */
  if(dispatchFn == L7_NULLPTR)
    return (L7_APP_TMR_CTRL_BLK_t)L7_NULLPTR;
  switch(timerType)
  {
    case L7_APP_TMR_1MSEC:
    case L7_APP_TMR_10MSEC:
    case L7_APP_TMR_100MSEC:
    case L7_APP_TMR_1SEC:
      break;
    default:
      return (L7_APP_TMR_CTRL_BLK_t)L7_NULLPTR;
  }
  if(buffPoolID == L7_NULL)
    return (L7_APP_TMR_CTRL_BLK_t)L7_NULLPTR;

  /* Allocate/Create the resources */
  pCtrlBlk = (appTmrCtrlBlk_t *)osapiMalloc(compId, sizeof(appTmrCtrlBlk_t));
  if(pCtrlBlk == L7_NULLPTR)
    return (L7_APP_TMR_CTRL_BLK_t)L7_NULLPTR;
  memset(pCtrlBlk, 0, sizeof(appTmrCtrlBlk_t));

	pCtrlBlk->bufferPoolId = buffPoolID;
  
  pCtrlBlk->semId = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if(pCtrlBlk->semId == L7_NULLPTR)
  {
    osapiFree(compId, pCtrlBlk);
    return (L7_APP_TMR_CTRL_BLK_t)L7_NULLPTR;
  }
  if(SLLCreate(compId, L7_SLL_ASCEND_ORDER, sizeof(L7_uint32), appTimerCompare,
               appTimerDestroy, &(pCtrlBlk->tmrList)) != L7_SUCCESS)
  {
    osapiSemaDelete(pCtrlBlk->semId);
    osapiFree(compId, pCtrlBlk);
    return (L7_APP_TMR_CTRL_BLK_t)L7_NULLPTR;
  }
  /* SLLCreate() creates an instance of the Singly Linked List that does not
     allow duplicates. Since for our application, we need support for multiple
     entries with the same expiry time, we need to explicitly enable nodes with
     duplicate key values in the list */
  SLLFlagsSet(&(pCtrlBlk->tmrList), L7_SLL_FLAG_ALLOW_DUPLICATES, L7_TRUE);
  pCtrlBlk->compId     = compId;
  pCtrlBlk->type       = timerType;
  pCtrlBlk->dispatchFn = dispatchFn;
  pCtrlBlk->pParam     = pParam;
  pCtrlBlk->pSelf      = pCtrlBlk;

  /* Start the base system tick timer */
  osapiTimerAdd(appTimerTick,
                (L7_uint32)pCtrlBlk,
                (L7_uint32)(pCtrlBlk->pParam),
                pCtrlBlk->type,
                &(pCtrlBlk->pSysTimer));

  return (L7_APP_TMR_CTRL_BLK_t)pCtrlBlk;
}

/*********************************************************************
*
* @purpose  De/Un-Initialize/Instantiate an Application Timer Module
*
* @param    timerCtrlBlk @b{(input)}Application Timer Control Block associated with the
*                                  instance that is to be destroyed
*
* @returns  L7_SUCCESS  If operation was successful
* @returns  L7_FAILURE  If operation failed
*
* @notes    The operation would clean-out any and all timers already in the
*           instance and also free-up any resources (memory,etc).
*
* @end
*
*********************************************************************/
L7_RC_t appTimerDeInit(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk)
{
  appTmrCtrlBlk_t    *pCtrlBlk;
  void               *semId;
  L7_COMPONENT_IDS_t compId;

  /* Basic sanity Checks */
  if(timerCtrlBlk == L7_NULLPTR)
    return L7_FAILURE;
  pCtrlBlk = (appTmrCtrlBlk_t *)timerCtrlBlk;
  if(pCtrlBlk->pSelf != pCtrlBlk)
    return L7_FAILURE;

  /* Lock the timer module to prevent concurrent operations */
  semId = pCtrlBlk->semId;
  if(osapiSemaTake(semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Stop the system timer ticks */
  osapiTimerFree(pCtrlBlk->pSysTimer);
  pCtrlBlk->pSysTimer = L7_NULLPTR;

  /* Release the resources */
  compId = pCtrlBlk->compId;
  SLLDestroy(compId, &(pCtrlBlk->tmrList));
  pCtrlBlk->type       = 0;
  pCtrlBlk->dispatchFn = L7_NULLPTR;
  pCtrlBlk->pSelf      = L7_NULLPTR;
  osapiFree(compId, pCtrlBlk);
  osapiSemaDelete(semId);

  /* All done */
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add/Create a Timer Node
*
* @param    timerCtrlBlk  @b{(input)}Application Timer Control Block associated with the
*                                    instance on which the new timer is being added to
* @param    pFunc         @b{(input)}Pointer to the function that is to be invoked upon
*                                    the timer's expiry
* @param    pParam        @b{(input)}A parameter (opaque to the timer lib) that the application
*                                    prefers to be passed along to the expiry function
* @param    timeOut       @b{(input)}Timeout,  in Application Timer base ticks.
*
* @returns  tmrHandle     Opaque Application Timer handle, if successful.
*                         This handle, alongwith the Timer Module Control Block,
*                         uniquely identifies the specific timer within a timer
*                         module instantance and must be used to reference this
*                         timer for all other operations (eg.appTmrDelete,
*                         appTmrUpdate, etc).
* @return    L7_NULLPTR    If operation failed
*
* @notes    The timeOut parameter MUST be in reference to the base application
*           timer instance's ticks. Please refer to the appTimerInit() call for
*           further reference.
*           As an example, if the appTimerInit() was invoked with L7_APP_TMR_100MSEC
*           for the timerType, then to get a timeout of 1 second, this function
*           must be called with a timeOut value of 10 (1 second = 10 * 10 msec).
*           This API is unprotected and it is the caller's (only this library) 
*           responsibility to acquire/release the timerCtrlBlk semaphore before 
*           invoking this.
*
* @end
*
*********************************************************************/
static L7_APP_TMR_HNDL_t
appTimerAddNode (L7_APP_TMR_CTRL_BLK_t timerCtrlBlk,
                 L7_app_tmr_fn         pFunc,
                 void                  *pParam,
                 L7_uint32             timeOut,
                 L7_uchar8             *timerName,
                 L7_uchar8             *fileName,
                 L7_uint32             lineNum)
{
  appTmrCtrlBlk_t *pCtrlBlk;
  L7_uint32       currTime;
  timerNode_t     *pTimerNode;

  pCtrlBlk = (appTmrCtrlBlk_t *)timerCtrlBlk;

  /* Retrieve the current timer tick */
  currTime = osapiTimeMillisecondsGet(); /* We must use the raw System Uptime for our
                                            time references to avoid problems due to
                                            user adjustments of the calender time */

  /* Get a new timer entry, populate it and put it into the timer list */
  if(bufferPoolAllocate(pCtrlBlk->bufferPoolId, (L7_uchar8 **)(&pTimerNode)) != L7_SUCCESS)
  {
    return (L7_APP_TMR_HNDL_t)L7_NULLPTR;
  }
  memset(pTimerNode, 0, sizeof(timerNode_t));
  pTimerNode->expiryFn   = pFunc;
  l7utilsFilenameStrip((L7_char8 **)&fileName);
#ifdef APPTIMER_DEBUG    
  osapiStrncpy(pTimerNode->name, timerName, APPTIMER_STR_LEN);
#endif  
  pTimerNode->expiryTime = currTime + (timeOut * pCtrlBlk->type);
  pTimerNode->pParam     = pParam;
  if(SLLAdd(&(pCtrlBlk->tmrList), (L7_sll_member_t *)pTimerNode) != L7_SUCCESS)
  {
    memset(pTimerNode, 0, sizeof(timerNode_t));
    bufferPoolFree(pCtrlBlk->bufferPoolId, (L7_uchar8 *)pTimerNode);
    return (L7_APP_TMR_HNDL_t)L7_NULLPTR;
  }
  return (L7_APP_TMR_HNDL_t)pTimerNode;
}

/*********************************************************************
*
* @purpose  Delete an Application Timer
*
* @param    timerCtrlBlk @b{(input)}Application Timer Control Block associated with the
*                                   instance from which the timer is being deleted from
*           timerHandle  @b{(input)}Timer Handle associated with the specific timer
*                                   being deleted
*
* @returns  L7_SUCCESS    Timer deletion succesful
*           L7_FAILURE    If timer deletion failed; either due to invalid parameters
*                         or that the timer had already popped
*
* @notes    None
*
* @end
*
*********************************************************************/
L7_RC_t appTimerDelete(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk,
                       L7_APP_TMR_HNDL_t     timerHandle)
{
  appTmrCtrlBlk_t *pCtrlBlk;
  timerNode_t     *pTimerNode;

  /* Basic sanity Checks */
  pCtrlBlk = (appTmrCtrlBlk_t *)timerCtrlBlk;
  pTimerNode = (timerNode_t *)timerHandle;
  if((pCtrlBlk == L7_NULLPTR) || (pTimerNode == L7_NULLPTR))
    return L7_FAILURE;
  if(pCtrlBlk->pSelf != pCtrlBlk)
    return L7_FAILURE;

  /* Lock the module */
  if(osapiSemaTake(pCtrlBlk->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Remove the entry from the timer list */
  if(SLLNodeRemove(&(pCtrlBlk->tmrList), (L7_sll_member_t *)pTimerNode) == L7_NULL)
  {
    osapiSemaGive(pCtrlBlk->semId);
    return L7_SUCCESS;
  }

  /* Free-up the resources */
  memset(pTimerNode, 0, sizeof(timerNode_t));
  bufferPoolFree(pCtrlBlk->bufferPoolId, (L7_uchar8 *)pTimerNode);
  osapiSemaGive(pCtrlBlk->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add/Create an Application Timer
*
* @param    timerCtrlBlk  @b{(input)}Application Timer Control Block associated with the
*                                    instance on which the new timer is being added to
* @param    pFunc         @b{(input)}Pointer to the function that is to be invoked upon
*                                    the timer's expiry
* @param    pParam        @b{(input)}A parameter (opaque to the timer lib) that the application
*                                    prefers to be passed along to the expiry function
* @param    timeOut       @b{(input)}Timeout,  in Application Timer base ticks.
*
* @returns  tmrHandle     Opaque Application Timer handle, if successful.
*                         This handle, alongwith the Timer Module Control Block,
*                         uniquely identifies the specific timer within a timer
*                         module instantance and must be used to reference this
*                         timer for all other operations (eg.appTmrDelete,
*                         appTmrUpdate, etc).
* @return    L7_NULLPTR    If operation failed
*
* @notes    The timeOut parameter MUST be in reference to the base application
*           timer instance's ticks. Please refer to the appTimerInit() call for
*           further reference.
*           As an example, if the appTimerInit() was invoked with L7_APP_TMR_100MSEC
*           for the timerType, then to get a timeout of 1 second, this function
*           must be called with a timeOut value of 10 (1 second = 10 * 10 msec).
*
* @end
*
*********************************************************************/
L7_APP_TMR_HNDL_t appTimerAdd_track(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk,
                              L7_app_tmr_fn         pFunc,
                              void                  *pParam,
                              L7_uint32             timeOut,
                              L7_uchar8             *timerName,
                              L7_uchar8             *fileName,
                              L7_uint32             lineNum)
{
  appTmrCtrlBlk_t *pCtrlBlk;
  timerNode_t     *pTimerNode;

  /* Basic sanity Checks */
  pCtrlBlk = (appTmrCtrlBlk_t *)timerCtrlBlk;
  if(pCtrlBlk == L7_NULLPTR)
    return (L7_APP_TMR_HNDL_t)L7_NULLPTR;
  if(pFunc == L7_NULLPTR)
    return (L7_APP_TMR_HNDL_t)L7_NULLPTR;
  if(pCtrlBlk->pSelf != pCtrlBlk)
    return (L7_APP_TMR_HNDL_t)L7_NULLPTR;

  /* Lock the module */
  if(osapiSemaTake(pCtrlBlk->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return (L7_APP_TMR_HNDL_t)L7_NULLPTR;

  pTimerNode = appTimerAddNode (timerCtrlBlk, pFunc, pParam, timeOut, timerName,
                    fileName, lineNum);

  osapiSemaGive(pCtrlBlk->semId);
  return (L7_APP_TMR_HNDL_t)pTimerNode;
}

/*********************************************************************
*
* @purpose  Update an Application Timer
*
* @param    timerCtrlBlk  @b{(input)}Application Timer Control Block associated with the
*                                    instance on which the timer is being updated
* @param    timerHandle  @b{(input)}Timer Handle associated with the specific timer
*                                   being updated
* @param    pFunc        @b{(input)}New Timer Expiry function. If L7_NULLPTR is given,
*                                   then the existing function pointer is left as is
* @param    pParam       @b{(input)}New parameter to be passed to the expiry function.
*                                  If L7_NULLPTR is given, then the existing parameter
*                                  value is left as is
* @param    timeOut      @b{(input)}New timeout value (in App Timer Ticks). The new timeOut
*                                  value is effective from the time this function is invoked
*                                  and not from the original addition time
*
* @returns  L7_SUCCESS    Timer updation succesful
* @returns  L7_FAILURE    If timer updation failed; either due to invalid parameters
*                         or that the timer had already popped
*
* @notes    Due to the nature of the definition of the pParam above, one
*           cannot update the parameter to a NULL pointer or to a 0 value (if
*           the pParam was actually a L7_uint32 being typecasted). To achieve
*           this the caller is forced to do a appTimerDelete(), followed by a
*           appTimerAdd().
*           This has been enhanced to add a new node if the node to be
*           updated is not found.
*
* @end
*
*********************************************************************/
L7_RC_t appTimerUpdate_track(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk,
                       L7_APP_TMR_HNDL_t     *timerHandle,
                       void                  *pFunc,
                       void                  *pParam,
                       L7_uint32             timeOut,
                       L7_uchar8             *timerName,
                       L7_uchar8             *fileName,
                       L7_uint32             lineNum)
{
  appTmrCtrlBlk_t *pCtrlBlk;
  timerNode_t     *pTimerNode = L7_NULLPTR;
  L7_uint32       currTime;

  /* Basic sanity Checks */
  pCtrlBlk = (appTmrCtrlBlk_t *)timerCtrlBlk;
  if(pCtrlBlk == L7_NULLPTR)
    return L7_FAILURE;
  if(pCtrlBlk->pSelf != pCtrlBlk)
    return L7_FAILURE;
  if (timerHandle == L7_NULLPTR)
    return L7_FAILURE;

  currTime = osapiTimeMillisecondsGet(); /* We must use the raw System Uptime for our
                                            time references to avoid problems due to
                                            user adjustments of the calender time */

  /* Lock the module */
  if(osapiSemaTake(pCtrlBlk->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;


  if (*timerHandle == L7_NULLPTR)
  {
    if ((*timerHandle = appTimerAddNode (timerCtrlBlk, pFunc, pParam, timeOut,timerName,
                        fileName, lineNum))
                      == L7_NULLPTR)
    {
      osapiSemaGive(pCtrlBlk->semId);
      return L7_FAILURE;
    }
    osapiSemaGive(pCtrlBlk->semId);
    return L7_SUCCESS;
  }

  pTimerNode = (timerNode_t *)*timerHandle;
  /* Remove the entry from the timer list */
  if(SLLNodeRemove(&(pCtrlBlk->tmrList), (L7_sll_member_t *)pTimerNode) == L7_NULL)
  {
    if ((*timerHandle = appTimerAddNode (timerCtrlBlk, pFunc, pParam, timeOut,timerName,
                fileName, lineNum))
                      == L7_NULLPTR)
    {
      osapiSemaGive(pCtrlBlk->semId);
      return L7_FAILURE;
    }
    osapiSemaGive(pCtrlBlk->semId);
    return L7_SUCCESS;
  }

  /* Update the timer entry */
  if(pFunc != L7_NULLPTR)
    pTimerNode->expiryFn = pFunc;
  if(pParam != L7_NULLPTR)
    pTimerNode->pParam = pParam;
  pTimerNode->expiryTime = currTime + (pCtrlBlk->type * timeOut);

  /* Add the entry back to the timer list */
  if(SLLAdd(&(pCtrlBlk->tmrList), (L7_sll_member_t *)pTimerNode) != L7_SUCCESS)
  {
    memset(pTimerNode, 0, sizeof(timerNode_t));
    bufferPoolFree(pCtrlBlk->bufferPoolId, (L7_uchar8 *)pTimerNode);
    osapiSemaGive(pCtrlBlk->semId);
    return L7_FAILURE;
  }
  osapiSemaGive(pCtrlBlk->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the time left for the given timer to expire
*
* @param    timerCtrlBlk @b{(input)}Application Timer Control Block associated with the
*                                   instance on which the timer is active
*           timerHandle  @b{(input)}Timer Handle associated with the specific timer
*           pTimeLeft    @b{(input)}Pointer to the output parameter that would be filled-in
*                                   with the time remaining (in multiples of the base
*                                   timer instance's granularity) for the timer to expire
*
* @returns  L7_SUCCESS   Operation succesful
*           L7_FAILURE   Operation failed; either due to invalid parameters
*                        or that the timer had already popped
*
* @end
*
*********************************************************************/
L7_RC_t appTimerTimeLeftGet(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk,
                            L7_APP_TMR_HNDL_t     timerHandle,
                            L7_uint32             *pTimeLeft)
{
  appTmrCtrlBlk_t *pCtrlBlk;
  timerNode_t     *pTimerNode;
  timerNode_t     *pTmpNode = L7_NULLPTR;
  L7_uint32       currTime;

  /* 
   * if timer does not exist or this api fails for whatever reason
   * just return 0 in timeleft
   */
  *pTimeLeft = 0;
  /* Basic sanity Checks */
  pCtrlBlk = (appTmrCtrlBlk_t *)timerCtrlBlk;
  pTimerNode = (timerNode_t *)timerHandle;
  if((pCtrlBlk == L7_NULLPTR) || (pTimerNode == L7_NULLPTR))
    return L7_FAILURE;
  if(pCtrlBlk->pSelf != pCtrlBlk)
    return L7_FAILURE;

  currTime = osapiTimeMillisecondsGet(); /* We must use the raw System Uptime for our
                                            time references to avoid problems due to
                                            user adjustments of the calender time */

  /* Lock the module */
  if(osapiSemaTake(pCtrlBlk->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Retrieve the entry from the timer list */
  pTmpNode = (timerNode_t *)SLLNodeFind(&(pCtrlBlk->tmrList), (L7_sll_member_t *)pTimerNode);
  if(pTmpNode == L7_NULLPTR)
  {
    osapiSemaGive(pCtrlBlk->semId);
    return L7_FAILURE;
  }


  /* Some times, if the timer tick events are still in the queue and not 
     processed yet means and during this instance if appTimerTimeLeftGet 
     is called from mgmt layer, there is a chance of pTmpNode->expiryTime being
     lesser than the current time and so may result in the negative value if 
     directly subtracted and so explicitly initialize it to 0 when expirytime is less than
     current time 
   */
     
  if(pTmpNode->expiryTime < currTime)
  {
    *pTimeLeft =0; 
  }
  else
  {
    /* Compute the time left based on the expiry time */
    *pTimeLeft = (pTmpNode->expiryTime - currTime) / pCtrlBlk->type;
  }

  osapiSemaGive(pCtrlBlk->semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Process an Application Timer tick
*
* @param    timerCtrlBlk @b{(input)}Application Timer Control Block associated with the
*                                   instance for which the timer tick is to be processed
*
* @returns  None
*
* @notes    This function MUST be invoked by the application (in its own context)
*           in response to a callback from the timer instance using dispatchFn
*           that was earlier registered with the library. The actual timer
*           expiry evaluations and the associated function invokations are
*           carried out in this function's caller's context.
*
* @end
*
*********************************************************************/
void appTimerProcess(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk)
{
  appTmrCtrlBlk_t *pCtrlBlk;
  timerNode_t     *pTimerNode = L7_NULLPTR;
  L7_uint32       currTime;

  /* Basic sanity Checks */
  pCtrlBlk = (appTmrCtrlBlk_t *)timerCtrlBlk;
  if(pCtrlBlk == L7_NULLPTR)
    return;
  if(pCtrlBlk->pSelf != pCtrlBlk)
    return;

  while(L7_TRUE)
  {
    currTime = osapiTimeMillisecondsGet(); /* We must use the raw System Uptime for our
                                              time references to avoid problems due to
                                              user adjustments of the calender time */

    /* Lock the module */
    if(osapiSemaTake(pCtrlBlk->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
      break;

    /* Get the top of list entry */
    pTimerNode = (timerNode_t *)SLLAtStartPop(&(pCtrlBlk->tmrList));
    if(pTimerNode != L7_NULLPTR)
    {
      if(pTimerNode->expiryTime > currTime)
      {
        SLLAtStartPush(&(pCtrlBlk->tmrList), (L7_sll_member_t *)pTimerNode);
        osapiSemaGive(pCtrlBlk->semId);
        break;
      }
      else
      {
        L7_app_tmr_fn pFunc = pTimerNode->expiryFn;
        void          *pParam = pTimerNode->pParam;

        /* Free-up the timer entry as we are popping it */
        memset(pTimerNode, 0, sizeof(timerNode_t));
        bufferPoolFree(pCtrlBlk->bufferPoolId, (L7_uchar8 *)pTimerNode);
        osapiSemaGive(pCtrlBlk->semId);

        /* Invoke the expiry function */
        if(pFunc != L7_NULLPTR)
        {
          pFunc(pParam);
        }
      }
    }
    else
    {
      osapiSemaGive(pCtrlBlk->semId);
      break;
    }
  };

  /* Restart the base system tick timer */
  osapiTimerAdd(appTimerTick,
                (L7_uint32)pCtrlBlk,
                (L7_uint32)(pCtrlBlk->pParam),
                pCtrlBlk->type,
                &(pCtrlBlk->pSysTimer));
}

/*********************************************************************
*
* @purpose  
*
* @param    timerCtrlBlk @b{(input)}Application Timer Control Block associated with the
*                                   instance for which the timer tick is to be processed
*
* @returns  None
*
* @notes    
*
* @end
*
*********************************************************************/
void appTimerDebugShow(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk)
{
#ifdef APPTIMER_DEBUG  

  appTmrCtrlBlk_t *pCtrlBlk;
  timerNode_t     *pTimerNode = L7_NULLPTR;
  L7_uint32       currTime;
  
  /* Basic sanity Checks */
  pCtrlBlk = (appTmrCtrlBlk_t *)timerCtrlBlk;
  if(pCtrlBlk == L7_NULLPTR)
    return;
  if(pCtrlBlk->pSelf != pCtrlBlk)
    return;
  sysapiPrintf("\n--------------------------------\n" );
  sysapiPrintf("TimerName  expiryTime expiryFn\n");
  sysapiPrintf("----------------------------------\n" );  

  /* Lock the module */
  if(osapiSemaTake(pCtrlBlk->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;
  /* Retrieve the current timer tick */
  currTime = osapiTimeMillisecondsGet();
  for (pTimerNode = (timerNode_t *)SLLFirstGet(&(pCtrlBlk->tmrList));
       pTimerNode != L7_NULLPTR;
       pTimerNode = (timerNode_t *)SLLNextGet(&(pCtrlBlk->tmrList), 
            (L7_sll_member_t *)pTimerNode))
  {
    sysapiPrintf("%-8s       %-4d      0x%x      \n",pTimerNode->name,
                ((pTimerNode->expiryTime - currTime)/pCtrlBlk->type), 
                pTimerNode->expiryFn);
  }
  osapiSemaGive(pCtrlBlk->semId);
#endif
       
}
/******************************************************************************
* @purpose 
*
* @param   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_uint32 appTimerCbMemSizeGet(L7_uint32   maxAppTimers)
{
 L7_uint32 size;

  size = sizeof(appTmrCtrlBlk_t) + maxAppTimers * sizeof(timerNode_t);
  return size;
}
