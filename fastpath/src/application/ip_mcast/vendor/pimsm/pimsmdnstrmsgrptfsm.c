/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmdnstrmsgrptfsm.c
*
* @purpose Contains PIM-SM Down stream per interface State Machine
* implementation for (S,G,Rpt) entry.
*
* @component    pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author dsatyanarayana / vmurali
* @end
*
***********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmdnstrmsgrptfsm.h"
#include "pimsmtimer.h"

static L7_RC_t pimsmDnStrmPerIntfSGRptPrunePendTimersStart
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pimsmSGRptNode,
 pimsmDnStrmPerIntfSGRptEventInfo_t * pDnStrmPerIntfSGRptEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGRptPruneExpireTimerRestart
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pimsmSGRptNode,
 pimsmDnStrmPerIntfSGRptEventInfo_t * pDnStrmPerIntfSGRptEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGRptDoNothing
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pimsmSGRptNode,
 pimsmDnStrmPerIntfSGRptEventInfo_t * pDnStrmPerIntfSGRptEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGRptErrorReturn
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pimsmSGRptNode,
 pimsmDnStrmPerIntfSGRptEventInfo_t * pDnStrmPerIntfSGRptEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGRptTimersCancel
(pimsmCB_t        *pimsmCb,
 pimsmSGRptNode_t *pSGRptNode,
 pimsmDnStrmPerIntfSGRptEventInfo_t * pDnStrmPerIntfSGRptEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGRptExpireTimerCancel
(pimsmCB_t        *pimsmCb,
 pimsmSGRptNode_t *pSGRptNode,
 pimsmDnStrmPerIntfSGRptEventInfo_t * pDnStrmPerIntfSGRptEventInfo);

typedef struct pimsmDnStrmPerIntfSGRptSM_s
{
  pimsmDnStrmPerIntfSGRptStates_t nextState;
  L7_RC_t (*pimsmDnStrmPerIntfSGRptAction)
  (pimsmCB_t * pimsmCb,
   struct pimsmSGRptNode_s * pimsmSGRptNode,
   pimsmDnStrmPerIntfSGRptEventInfo_t* pDnStrmPerIntfSGRptEventInfo);
} pimsmDnStrmPerIntfSGRptSM_t;

static pimsmDnStrmPerIntfSGRptSM_t pimsmDnStrmPerIntfSGRpt
[PIMSM_DNSTRM_S_G_RPT_SM_EVENT_MAX]
[PIMSM_DNSTRM_S_G_RPT_SM_STATE_MAX] =
{
  {/*EVENT_RECV_JOIN_STAR_G*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_TMP,
      pimsmDnStrmPerIntfSGRptDoNothing},
    /*PRUNE_PENDING*/{PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING_TMP,
      pimsmDnStrmPerIntfSGRptDoNothing},
    /*PRUNE_TMP*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_TMP,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_PENDING_TMP*/
    {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING_TMP,
      pimsmDnStrmPerIntfSGRptErrorReturn}
  },
  {/*EVENT_RECV_JOIN_SG_RPT*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptTimersCancel},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptTimersCancel},
    /*PRUNE_TMP*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_TMP,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_PENDING_TMP*/
    {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING_TMP,
      pimsmDnStrmPerIntfSGRptErrorReturn}
  },
  {/*EVENT_RECV_PRUNE_SG_RPT*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING,
      pimsmDnStrmPerIntfSGRptPrunePendTimersStart},
    /*PRUNE*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE,
      pimsmDnStrmPerIntfSGRptPruneExpireTimerRestart},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_TMP*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE,
      pimsmDnStrmPerIntfSGRptPruneExpireTimerRestart},
    /*PRUNE_PENDING_TMP*/{PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING,
      pimsmDnStrmPerIntfSGRptPruneExpireTimerRestart}
  },
  {/*EVENT_END_OF_MSG*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_TMP*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptExpireTimerCancel},
    /*PRUNE_PENDING_TMP*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptTimersCancel}
  },
  {/*EVENT_PRUNE_PENDING_TIMER_EXPIRES*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE,
      pimsmDnStrmPerIntfSGRptDoNothing},
    /*PRUNE_TMP*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_TMP,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_PENDING_TMP*/
    {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING_TMP,
      pimsmDnStrmPerIntfSGRptErrorReturn}
  },
  {/*EVENT_EXPIRY_TIMER_EXPIRES*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGRptDoNothing},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_TMP*/ {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_TMP,
      pimsmDnStrmPerIntfSGRptErrorReturn},
    /*PRUNE_PENDING_TMP*/
    {PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING_TMP,
      pimsmDnStrmPerIntfSGRptErrorReturn}
  }
};

static void pimsmSGRptIPrunePendingTimerExpiresHandler(void *pParam);
static void  pimsmSGRptIPruneExpireTimerExpiresHandler(void *pParam);
/******************************************************************************
* @purpose   execute the DnStream per (S,G,Rpt) FSM and process the events received
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGRptNode                   @b{(input)} (S,G,Rpt) node
* @param    pDnStrmPerIntfSGRptEventInfo @b{(input)} Downstream SG Rpt FSM
*                                                    event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*
******************************************************************************/

L7_RC_t pimsmDnStrmPerIntfSGRptExecute(pimsmCB_t        *pimsmCb,
                              pimsmSGRptNode_t *pSGRptNode,
                               pimsmDnStrmPerIntfSGRptEventInfo_t
                               *pDnStrmPerIntfSGRptEventInfo)
{
  L7_uint32               rtrIfNum;
  pimsmSGRptIEntry_t     *pSGRptIEntry;
  pimsmDnStrmPerIntfSGRptStates_t dnStrmPerIntfSGRptState;
  pimsmDnStrmPerIntfSGRptEvents_t dnStrmPerIntfSGRptEvent;
  L7_RC_t   rc = L7_SUCCESS;


  if((pSGRptNode == (pimsmSGRptNode_t *)L7_NULLPTR)||
     (pDnStrmPerIntfSGRptEventInfo == (pimsmDnStrmPerIntfSGRptEventInfo_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR, "invalid input parameters");
    return L7_FAILURE;
  }
  if((pSGRptNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
        "Node is deleted");
    return L7_FAILURE;
  }
  rtrIfNum = pDnStrmPerIntfSGRptEventInfo->rtrIfNum;
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "rtrIfNum =%d ", rtrIfNum);
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if(pSGRptIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"(S,G,Rpt,%d) entry is not present",
                                 rtrIfNum);
    return L7_FAILURE;
  }

  dnStrmPerIntfSGRptEvent = pDnStrmPerIntfSGRptEventInfo->eventType;
  dnStrmPerIntfSGRptState = pSGRptIEntry->pimsmSGRptIJoinPruneState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "State = %s, \n Event = %s ",
              pimsmDnStrmPerIntfSGRptStateName[dnStrmPerIntfSGRptState],
              pimsmDnStrmPerIntfSGRptEventName[dnStrmPerIntfSGRptEvent]);

  rc =
  (pimsmDnStrmPerIntfSGRpt[dnStrmPerIntfSGRptEvent][dnStrmPerIntfSGRptState]).pimsmDnStrmPerIntfSGRptAction
   (pimsmCb, pSGRptNode,pDnStrmPerIntfSGRptEventInfo);

  if(rc == L7_FAILURE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_NORMAL, "DnStrmPerIntfSGRptAction Failed ");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"DnStrmPerIntfSGRptAction Failed.  State = %s, \n Event = %s",
      pimsmDnStrmPerIntfSGRptStateName[dnStrmPerIntfSGRptState],
      pimsmDnStrmPerIntfSGRptEventName[dnStrmPerIntfSGRptEvent]);
    pimsmSGRptNodeTryRemove(pimsmCb, pSGRptNode);
    return L7_FAILURE;
  }

  pSGRptIEntry->pimsmSGRptIJoinPruneState
   = (pimsmDnStrmPerIntfSGRpt[dnStrmPerIntfSGRptEvent][dnStrmPerIntfSGRptState]).nextState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "Next State = %s \n ",
            pimsmDnStrmPerIntfSGRptStateName[pSGRptIEntry->pimsmSGRptIJoinPruneState]);
  pimsmSGRptNodeTryRemove(pimsmCb, pSGRptNode);
  return rc;
}
/******************************************************************************
* @purpose  Start Prune pending expiry timer of (S,G, Rpt) entry supplied
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGRptNode                   @b{(input)} (S,G,Rpt) node
* @param    pDnStrmPerIntfSGRptEventInfo @b{(input)} Downstream SG Rpt FSM
*                                                    event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfSGRptPrunePendTimersStart
                                 (pimsmCB_t        *pimsmCb,
                                  pimsmSGRptNode_t *pSGRptNode,
                                  pimsmDnStrmPerIntfSGRptEventInfo_t
                                  *dnStrmPerIntfSGRptEventInfo)
{
  L7_uint32              rtrIfNum;
  pimsmSGRptIEntry_t    *pSGRptIEntry;
  L7_uint32              holdtime, timeOut = 0;
  pimsmInterfaceEntry_t *pIntfEntry;
  pimsmTimerData_t * pTimerData = L7_NULLPTR;

  rtrIfNum = dnStrmPerIntfSGRptEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if(pSGRptIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"(S,G,Rpt,%d) entry is not present",
                                 rtrIfNum);
    return L7_FAILURE;
  }
  /* Start Expiry Timer */
  holdtime = (L7_uint32) dnStrmPerIntfSGRptEventInfo->holdtime;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "(S,G,Rpt) dnstrm prune expire timer holdtime =%d",
                                 holdtime);
  pTimerData = &pSGRptIEntry ->pimsmSGRptIPruneExpiryTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->rtrIfNum = rtrIfNum;
  inetCopy(&pTimerData->addr1, &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
  pTimerData->pimsmCb = pimsmCb;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,"Adding Expire timer ");
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGRptIPruneExpireTimerExpiresHandler,
                            (void*)pSGRptIEntry ->pimsmSGRptIPruneExpiryTimerHandle,
                            holdtime,
                            &(pSGRptIEntry->pimsmSGRptIPruneExpiryTimer),
                            "SGr-PT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                  "Failed to add (S,G,Rpt,%d) prune expire timer",rtrIfNum);
    return L7_FAILURE;
  }

  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &pIntfEntry )
     !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"Failed to get interface entry for %d",rtrIfNum);
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "Neighbor count = %d", pIntfEntry->pimsmNbrCount);
  /* Start Prune-Pending Timer; */
  if(pIntfEntry->pimsmNbrCount > 1)
  {
    timeOut = pimsmJoinPruneOverrideIntervalGet(pimsmCb,rtrIfNum);
  }

  pTimerData = &pSGRptIEntry ->pimsmSGRptIPrunePendingTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->rtrIfNum = rtrIfNum;
  inetCopy(&pTimerData->addr1, &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGRptIPrunePendingTimerExpiresHandler,
                            (void*)pSGRptIEntry ->pimsmSGRptIPrunePendingTimerHandle,
                            timeOut,
                            &(pSGRptIEntry ->pimsmSGRptIPrunePendingTimer),
                            "SGr-PPT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "Failed to add (S,G,Rpt,%d) prune pending timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Restart expiry timer of (S,G, Rpt) entry supplied
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGRptNode                   @b{(input)} (S,G,Rpt) node
* @param    pDnStrmPerIntfSGRptEventInfo @b{(input)} Downstream SG Rpt FSM
*                                                    event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfSGRptPruneExpireTimerRestart(
                                                   pimsmCB_t        *pimsmCb,
                                                   pimsmSGRptNode_t *pSGRptNode,
                                                   pimsmDnStrmPerIntfSGRptEventInfo_t
                                                   * pDnStrmPerIntfSGRptEventInfo)
{

  L7_uint32           rtrIfNum, timeLeft = 0;
  pimsmSGRptIEntry_t *pSGRptIEntry;
  L7_uint32           holdtime;
  pimsmTimerData_t   *pTimerData = L7_NULLPTR;
  L7_RC_t             rc;

  rtrIfNum = pDnStrmPerIntfSGRptEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if(pSGRptIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"(S,G,Rpt,%d) entry is not present",
                                 rtrIfNum);
    return L7_FAILURE;
  }

  /* Restart Expiry Timer;*/
  /* set to maximum of its current value and the holdtime from the
   * triggering Join/Prune message.
   */
  holdtime = (L7_uint32) pDnStrmPerIntfSGRptEventInfo->holdtime;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "(S,G,Rpt) dnstrm Expire timer holdtime = %d",
                                 holdtime);

  rc = appTimerTimeLeftGet(pimsmCb->timerCb,
                      pSGRptIEntry->pimsmSGRptIPruneExpiryTimer, &timeLeft);

  holdtime = max(timeLeft, holdtime);
  pTimerData = &pSGRptIEntry ->pimsmSGRptIPruneExpiryTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->rtrIfNum = rtrIfNum;
  inetCopy(&pTimerData->addr1, &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
  pTimerData->pimsmCb = pimsmCb;

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGRptIPruneExpireTimerExpiresHandler,
                            (void*)pSGRptIEntry ->pimsmSGRptIPruneExpiryTimerHandle,
                            holdtime,
                            &(pSGRptIEntry->pimsmSGRptIPruneExpiryTimer),
                            "SGr-PT2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,
                "Failed to add (S,G,Rpt,%d) Expire timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Change the state and return
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGRptNode                   @b{(input)} (S,G,Rpt) node
* @param    pDnStrmPerIntfSGRptEventInfo @b{(input)} Downstream SG Rpt FSM
*                                                    event related data
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfSGRptDoNothing(pimsmCB_t        *pimsmCb,
                                     pimsmSGRptNode_t *pSGRptNode,
                                    pimsmDnStrmPerIntfSGRptEventInfo_t
                                    * pDnStrmPerIntfSGRptEventInfo)
{
  MCAST_UNUSED_PARAM(pSGRptNode);
  MCAST_UNUSED_PARAM(pDnStrmPerIntfSGRptEventInfo);
  return(L7_SUCCESS); /* Change to new state */
}
/******************************************************************************
* @purpose  Unexpected event, return Error
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGRptNode                   @b{(input)} (S,G,Rpt) node
* @param    pDnStrmPerIntfSGRptEventInfo @b{(input)} Downstream SG Rpt FSM
*                                                    event related data
*
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfSGRptErrorReturn(pimsmCB_t        *pimsmCb,
                                                  pimsmSGRptNode_t *pSGRptNode,
                                                  pimsmDnStrmPerIntfSGRptEventInfo_t
                                                  * pDnStrmPerIntfSGRptEventInfo)
{
  MCAST_UNUSED_PARAM(pSGRptNode);
  MCAST_UNUSED_PARAM(pDnStrmPerIntfSGRptEventInfo);
  return(L7_FAILURE); /* Change to new state */
}

/******************************************************************************
* @purpose  Action Routine when Prune Pending Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmSGRptIPrunePendingTimerExpiresHandler(void *pParam)
{
  pimsmDnStrmPerIntfSGRptEventInfo_t pDnStrmPerIntfSGRptEventInfo;
  pimsmSGRptNode_t   *pSGRptNode;
  pimsmSGRptIEntry_t *pSGRptIEntry= L7_NULLPTR;
  L7_uint32           rtrIfNum;
  pimsmTimerData_t   *pTimerData;
  pimsmCB_t          *pimsmCb;
  L7_int32            handle = (L7_int32)pParam;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_BOOL pruneDesired;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;
  pimsmStarGNode_t * pStarGNode;
  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb;
  pSrcAddr = &pTimerData->addr1;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
      "Source Address :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);
  if (pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_NORMAL,
        "(S,G,rpt) Find failed");
    return;
  }
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if (pSGRptIEntry->pimsmSGRptIPrunePendingTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM Downstream pimsmSGRptIPrunePendingTimer is NULL, But Still Expired");
    return;
  }
  pSGRptIEntry->pimsmSGRptIPrunePendingTimer = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_NOTICE, "(S,G,I) Prune Pending Timer Expired ");
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_DEBUG, "RtrIfNum: %d ",rtrIfNum);

  memset(&pDnStrmPerIntfSGRptEventInfo, 0, sizeof(pimsmDnStrmPerIntfSGRptEventInfo_t));

  pDnStrmPerIntfSGRptEventInfo.eventType =
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES;
  pDnStrmPerIntfSGRptEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfSGRptExecute(pimsmCb, pSGRptNode, &pDnStrmPerIntfSGRptEventInfo);

  memset(&upStrmSGRptEventInfo,0,sizeof(pimsmUpStrmSGRptEventInfo_t));
  pruneDesired = pimsmSGRptPruneDesired(pimsmCb,pSGRptNode);
  if(pruneDesired ==  L7_TRUE)
  {
    upStrmSGRptEventInfo.eventType
          = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
  }
  else
  {
    upStrmSGRptEventInfo.eventType
          = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE;
  }
  pimsmUpStrmSGRptExecute(pimsmCb,pSGRptNode,&upStrmSGRptEventInfo);
  /* Update MFC and cleanup the corresponding cache entry in (*,G) */
  if(pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode, MFC_UPDATE_ENTRY, L7_TRUE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"MFC Updation failed");
    return ;
  }
  if (pimsmStarGFind(pimsmCb, pGrpAddr,
                     &pStarGNode) == L7_SUCCESS)
  {
    pimsmCache_t  *pKernelCacheEntry = L7_NULLPTR;
    if (pimsmStarGCacheFind(pimsmCb,pStarGNode,pSrcAddr,pGrpAddr,&pKernelCacheEntry)
               == L7_SUCCESS)
    {
      pimsmStarGCacheDelete(pimsmCb,pStarGNode,pKernelCacheEntry);
    }
  }
}

/******************************************************************************
* @purpose  Action Routine when Prune Expire Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void  pimsmSGRptIPruneExpireTimerExpiresHandler(void *pParam)
{
  pimsmDnStrmPerIntfSGRptEventInfo_t pDnStrmPerIntfSGRptEventInfo;
  pimsmSGRptNode_t    *pSGRptNode;
  pimsmSGRptIEntry_t  *pSGRptIEntry= L7_NULLPTR;
  L7_uint32            rtrIfNum;
  pimsmTimerData_t    *pTimerData;
  pimsmCB_t           *pimsmCb;
  L7_int32             handle = (L7_int32)pParam;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_BOOL pruneDesired;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb;
  pSrcAddr = &pTimerData->addr1;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
      "Source Address :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);
  if (pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_NORMAL,
        "(S,G,rpt) Find failed");
    return;
  }
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];
  if (pSGRptIEntry->pimsmSGRptIPruneExpiryTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM Downstream pimsmSGRptIPruneExpiryTimer is NULL, But Still Expired");
    return;
  }
  pSGRptIEntry->pimsmSGRptIPruneExpiryTimer = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_NORMAL, "(S,G,Rpt, I) Prune Pending Timer Expired ");
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_DEBUG, "RtrIfNum: %d ",rtrIfNum);

  memset(&pDnStrmPerIntfSGRptEventInfo, 0, sizeof(pimsmDnStrmPerIntfSGRptEventInfo_t));

  pDnStrmPerIntfSGRptEventInfo.eventType =
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_EXPIRY_TIMER_EXPIRES;
  pDnStrmPerIntfSGRptEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfSGRptExecute(pimsmCb, pSGRptNode, &pDnStrmPerIntfSGRptEventInfo);

  memset(&upStrmSGRptEventInfo,0,sizeof(pimsmUpStrmSGRptEventInfo_t));
  pruneDesired = pimsmSGRptPruneDesired(pimsmCb,pSGRptNode);
  if (pruneDesired ==  L7_TRUE)
  {
    upStrmSGRptEventInfo.eventType
    = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
  }
  else
  {
    upStrmSGRptEventInfo.eventType
    = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE;
  }
  pimsmUpStrmSGRptExecute(pimsmCb,pSGRptNode,&upStrmSGRptEventInfo);

  if(pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_ERROR,"MFC Updation failed");
    return;
  }
}

/******************************************************************************
* @purpose  Cancel (S,G,Rpt) per interface expire and prune pending timers
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGRptNode                   @b{(input)} (S,G,Rpt) node
* @param    pDnStrmPerIntfSGRptEventInfo @b{(input)} Downstream SG Rpt FSM
*                                                    event related data
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfSGRptTimersCancel(pimsmCB_t        *pimsmCb,
                                     pimsmSGRptNode_t *pSGRptNode,
                                    pimsmDnStrmPerIntfSGRptEventInfo_t
                                    * pDnStrmPerIntfSGRptEventInfo)
{
  /* TBD : To cancel ET and PPT as per RFC and ANVL */
  L7_uint32              rtrIfNum;
  pimsmSGRptIEntry_t    *pSGRptIEntry;

  rtrIfNum = pDnStrmPerIntfSGRptEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];

  /* Cancel the prune pending timer */
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptIEntry->pimsmSGRptIPrunePendingTimer));

  /* Cancel the expiry timer */
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptIEntry->pimsmSGRptIPruneExpiryTimer));
  return(L7_SUCCESS); /* Change to new state */
}


/******************************************************************************
* @purpose  Cancel (S,G,Rpt) per interface expire timer.
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGRptNode                   @b{(input)} (S,G,Rpt) node
* @param    pDnStrmPerIntfSGRptEventInfo @b{(input)} Downstream SG Rpt FSM
*                                                    event related data
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfSGRptExpireTimerCancel(pimsmCB_t        *pimsmCb,
                                     pimsmSGRptNode_t *pSGRptNode,
                                    pimsmDnStrmPerIntfSGRptEventInfo_t
                                    * pDnStrmPerIntfSGRptEventInfo)
{
  L7_uint32              rtrIfNum;
  pimsmSGRptIEntry_t    *pSGRptIEntry;

  rtrIfNum = pDnStrmPerIntfSGRptEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGRptIEntry = pSGRptNode->pimsmSGRptIEntry[rtrIfNum];

  /* Cancel the expiry timer */
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptIEntry->pimsmSGRptIPruneExpiryTimer));
  return(L7_SUCCESS); /* Change to new state */
}
