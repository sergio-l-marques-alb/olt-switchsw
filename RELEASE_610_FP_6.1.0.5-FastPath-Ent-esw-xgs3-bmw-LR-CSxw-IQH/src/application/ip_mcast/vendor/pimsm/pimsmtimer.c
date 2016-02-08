/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmTimer.c
*
* @purpose Contains all timer related APIs.
*
* @component pimsm
*
* @comments 
*
* @create 01/01/2006
*
* @author dsatyanarayana
* @end
*
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "l7apptimer_api.h"
#include "mcast_map.h"
#include "pimsmtimer.h" 
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmmrt.h"
#include "pimsmrp.h"
#include "pimsmbsr.h"
#include "pimsmintf.h"
#include "pimsmneighbor.h"
#include "pimsm_vend_ctrl.h"
/* Following definitions are used to calculated the optimal/max number
   of timers in PIMSM module */


/* (S,G) / (S,G,I) related */
#define PIMSM_S_G_NUM_TIMERS 3
#define PIMSM_S_G_I_NUM_TIMERS 3 

/* (S,G,rpt) / (S,G,rpt,I) related */
#define PIMSM_S_G_RPT_NUM_TIMERS 1
#define PIMSM_S_G_RPT_I_NUM_TIMERS 2

/* (*,G) / (*,G,I) related */
#define PIMSM_STAR_G_NUM_TIMERS 1
#define PIMSM_STAR_G_I_NUM_TIMERS 3

/* (*,*,RP) / (*,*,RP,I) related */
#define PIMSM_STAR_STAR_RP_NUM_TIMERS 1
#define PIMSM_STAR_STAR_RP_I_NUM_TIMERS 2

/*interface related */
#define PIMSM_PER_INTF_NUM_TIMERS 1

/*neighbor related */
#define PIMSM_PER_NBR_NUM_TIMERS 1


/* BSR related*/
#define PIMSM_BSR_NUM_TIMERS 2

/* RP related*/
#define PIMSM_RP_GRP_MAPPING_NUM_TIMERS   1
#define PIMSM_CAND_RP_NUM_TIMERS 1


/******************************************************************************
* @purpose  
*
* @param
*
* @returns  void
*
* @comments     
*     
* @end
******************************************************************************/
void pimsmSPTTimerExpiresHandler(void *pParam)
{
  pimsmStarGNode_t  *pStarGNode;
  L7_RC_t rc;
  pimsmCache_t  kernelRouteData, *kernelCacheNode;
  L7_inet_addr_t   *pSrcAddr;
  L7_inet_addr_t   *pGrpAddr;
  pimsmTimerData_t *pTimerData;
  L7_int32      handle = (L7_int32)pParam; 
  pimsmCB_t *pimsmCb = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "handle = %d",
              handle); 

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if(L7_NULLPTR == pTimerData)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"Invalid input parameter");
    return; 
  }

  pimsmCb = pTimerData->pimsmCb;

  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,"Control Block is NULL");
    return; 
  }

  if (pimsmCb->pimsmSPTTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM pimsmSPTTimer is NULL, But Still Expired");
    return;
  }
  pimsmCb->pimsmSPTTimer = L7_NULLPTR; 

  /* Cleanup any stale entries */
  pimsmMRTNodeCleanup(pimsmCb);

  rc = pimsmStarGFirstGet(pimsmCb,&pStarGNode);
  while(rc == L7_SUCCESS)
  {
    memset(&kernelRouteData, 0, sizeof(pimsmCache_t));
    kernelCacheNode = &kernelRouteData;
    rc = pimsmStarGCacheGetFirst(pimsmCb,pStarGNode, 
                                  &kernelCacheNode);
    while(rc == L7_SUCCESS)
    {
      pSrcAddr = &kernelCacheNode->pimsmSrcAddr;
      pGrpAddr = &kernelCacheNode->pimsmGrpAddr;
      pimsmCheckSwitchToSpt(pimsmCb, pSrcAddr, pGrpAddr);
           rc = pimsmStarGCacheNextGet(pimsmCb,pStarGNode, 
                                   kernelCacheNode, &kernelCacheNode);
    }
     rc = pimsmStarGNextGet(pimsmCb, pStarGNode, &pStarGNode);
  }

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSPTTimerExpiresHandler,
                            (void*)pimsmCb->pimsmSPTTimerHandle,
                            PIMSM_DEFAULT_CHECK_INTERVAL,
                            &(pimsmCb->pimsmSPTTimer),
                            "SM-SPT2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR, "pimsmSPTTimer appTimerAdd Failed");
    return;
  }
}

/******************************************************************************
* @purpose  Main routine called when timer expire.
*
* @param
*
* @returns  void
*
* @comments     
*     
* @end
******************************************************************************/
static void pimsmAppTimerCallback(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk,void *pimsmCb)
{
  pimsmCB_t *pimsmCB =L7_NULLPTR;

  pimsmCB = (pimsmCB_t *)pimsmCb;
  /* post event to PIMSM Queue */
  if (pimsmMapMessageQueueSend(pimsmCB->family,
                           MCAST_EVENT_PIMSM_TIMER_EXPIRY,
                           0,
                           L7_NULLPTR) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR, "PIMSM message Queue Send  failed.");
    return;
  }
  return;
}

/******************************************************************************
* @purpose  All PIMSM Timer Initialization.
*
* @param    pimsmCb     @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmTimersInit(pimsmCB_t *pimsmCb)
{
  L7_uint32     maxTimers;
  L7_APP_TMR_CTRL_BLK_t      timerCb;
  L7_uint32     appTimerbufPoolId;
  void *handleListMemHndl = L7_NULLPTR;
  L7_uint32 mrtSGTableSize = 0;
  L7_uint32 mrtStarGTableSize = 0;
  L7_uint32 mrtSGRptTableSize = 0;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Enter")

  if (pimsmCb->family == L7_AF_INET)
  {
    mrtSGTableSize = PIMSM_S_G_IPV4_TBL_SIZE_TOTAL;
    mrtStarGTableSize = PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL;
    mrtSGRptTableSize = PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL;
  }
  else if (pimsmCb->family == L7_AF_INET6)
  {
    mrtSGTableSize = PIMSM_S_G_IPV6_TBL_SIZE_TOTAL;
    mrtStarGTableSize = PIMSM_STAR_G_IPV6_TBL_SIZE_TOTAL;
    mrtSGRptTableSize = PIMSM_S_G_RPT_IPV6_TBL_SIZE_TOTAL;
  }
  else
  {
    return L7_FAILURE;
  }

  maxTimers = (PIMSM_S_G_NUM_TIMERS + 
               PIMSM_S_G_I_NUM_TIMERS * PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES) * 
              mrtSGTableSize +
              (PIMSM_STAR_G_NUM_TIMERS + 
               PIMSM_STAR_G_I_NUM_TIMERS * PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES) * 
              mrtStarGTableSize + 
              (PIMSM_STAR_STAR_RP_NUM_TIMERS + 
               PIMSM_STAR_STAR_RP_I_NUM_TIMERS * PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES) * 
              PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL +
              (PIMSM_S_G_RPT_NUM_TIMERS + 
               PIMSM_S_G_RPT_I_NUM_TIMERS * PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES) * 
              mrtSGRptTableSize +
              (PIMSM_PER_INTF_NUM_TIMERS * PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES) +
              (PIMSM_PER_NBR_NUM_TIMERS * PIMSM_MAX_NBR) + 
              (PIMSM_BSR_NUM_TIMERS * PIMSM_BSR_SCOPE_NODES_MAX) +
              (PIMSM_RP_GRP_MAPPING_NUM_TIMERS *
               PIMSM_RP_GRP_ENTRIES_MAX) +
              (PIMSM_CAND_RP_NUM_TIMERS * PIMSM_CAND_RP_GROUPS_MAX)+
              PIMSM_GLOBAL_NUM_TIMERS;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Number of timers= %d",maxTimers);
  pimsmCb->maxNumOfTimers = maxTimers;

    if (mcastMapGetAppTimerBufPoolId(&appTimerbufPoolId) != L7_SUCCESS)
    {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                    "PIMSM: Unable to get Buffer Pool ID for App Timer\n");
        return L7_FAILURE;
    }

  timerCb = appTimerInit(L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                         pimsmAppTimerCallback,(void *)pimsmCb,
                         L7_APP_TMR_1SEC, appTimerbufPoolId);

  if(L7_NULLPTR ==  timerCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                "PIMSM: AppTimer Initialization failed\n");
    return L7_FAILURE;
  }
  pimsmCb->timerCb = timerCb;

  if (mcastMapGetHandleListHandle (pimsmCb->family, &handleListMemHndl) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                "PIMSM: Failed to get HandleList Handle");
    return L7_FAILURE;
  }
  if(handleListInit(L7_FLEX_PIMSM_MAP_COMPONENT_ID, pimsmCb->maxNumOfTimers,
                    &pimsmCb->handleList, (handle_member_t*) handleListMemHndl)
                 != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                "PIMSM: handleList Initialization failed\n");
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Exit")
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  All PIMSM Timer De-initialization.
*
* @param    pimsmCb     @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmTimersDeInit(pimsmCB_t *pimsmCb)
{
  L7_RC_t  rc;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Enter")

  handleListDeinit(L7_FLEX_PIMSM_MAP_COMPONENT_ID,pimsmCb->handleList);

  rc = appTimerDeInit(pimsmCb->timerCb);
  if(L7_SUCCESS != rc)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR, "Timer De-Initialization Failed");
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_DEBUG, "Exit")
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Function to Start the Timer (Both Set and Reset Timer)
*
* @param   pimsmCB   @b{(input)} Control Block.
*          pFunc     @b{(input)} Function Pointer.
*          pParam    @b{(input)} Function Parameter
*          timeout   @b{(input)} Timeout in seconds
*          errMessage@b{(input)}  Error Message
*          successMessage@b{(input)}  Error Message
*          tmrHandle     @b{(output)} Timer Handle
*           
*                            
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t
pimsmUtilAppTimerSet (pimsmCB_t *pimsmCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_uint32 timeOut,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uchar8 *timerName)
{
  if(*tmrHandle != NULL)
  {
    if (appTimerUpdate (pimsmCB->timerCb,tmrHandle,pFunc,pParam,timeOut,timerName)
                        != L7_SUCCESS)
    {
      *tmrHandle = 0;
      return L7_FAILURE;
    }
  }
  else
  {
    if ((*tmrHandle = appTimerAdd (pimsmCB->timerCb, pFunc, (void*) pParam,
                                       timeOut,timerName))
                                    == L7_NULLPTR)
    {
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Function to Cancel the Timer
*
* @param   pimsmCB   @b{(input)} Control Block.
*          handle    @b{(output)} Timer Handle
*           
*                            
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t
pimsmUtilAppTimerCancel (pimsmCB_t *pimsmCB,
                         L7_APP_TMR_HNDL_t *tmrHandle)

{
  L7_APP_TMR_HNDL_t   timerHandle =*tmrHandle;
  L7_RC_t retVal = L7_FAILURE;

  if(*tmrHandle != L7_NULLPTR)
  {
    if( appTimerDelete (pimsmCB->timerCb, timerHandle) == L7_SUCCESS)
    {
      retVal = L7_SUCCESS;
    }
    else
    {
      PIMSM_TRACE (PIMSM_DEBUG_BIT_MISC,PIMSM_TRACE_ERROR,
                   "Failed to Delete the Timer");
      retVal = L7_FAILURE;
    }
    *tmrHandle = L7_NULLPTR;
  }

  return retVal;
}

