/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmUpStrmStarGFSM.c
*
* @purpose Contains implementation for (*,G) Upstream state machine
*
* @component
*
* @comments
*
* @create 08/18/2005
*
* @author Muralidhar Vakkalagadda
* @end
*
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmupstrmstargfsm.h"
#include "pimsmtimer.h"
#include "pimsmwrap.h"


static L7_RC_t  pimsmUpStrmStarGJoinSend(pimsmCB_t * pimsmCb,
                 pimsmStarGNode_t * pStarGNode,
                 pimsmUpStrmStarGEventInfo_t * pUpStrmStarGEventInfo);
static L7_RC_t  pimsmUpStrmStarGPruneSend(pimsmCB_t * pimsmCb,
                 pimsmStarGNode_t * pStarGNode,
                 pimsmUpStrmStarGEventInfo_t * pUpStrmStarGEventInfo);
static L7_RC_t  pimsmUpStrmStarGJoinTimerIncrease(pimsmCB_t * pimsmCb,
                 pimsmStarGNode_t * pStarGNode,
                 pimsmUpStrmStarGEventInfo_t * pUpStrmStarGEventInfo);
static L7_RC_t  pimsmUpStrmStarGJoinTimerDecrease(pimsmCB_t * pimsmCb,
                 pimsmStarGNode_t * pStarGNode,
                 pimsmUpStrmStarGEventInfo_t * pUpStrmStarGEventInfo);
static L7_RC_t  pimsmUpStrmStarGJoinPruneSend( pimsmCB_t * pimsmCb,
                 pimsmStarGNode_t * pStarGNode,
                 pimsmUpStrmStarGEventInfo_t * pUpStrmStarGEventInfo);
static L7_RC_t pimsmUpStrmStarGErrorReturn(pimsmCB_t * pimsmCb,
                 pimsmStarGNode_t * pStarGNode,
                 pimsmUpStrmStarGEventInfo_t * pUpStrmStarGEventInfo);

typedef struct pimsmUpStrmStarGSM_s
{
  pimsmUpStrmStarGStates_t nextState;
  L7_RC_t (*pimsmUpStrmStarGAction) (pimsmCB_t * pimsmCb,
                                     struct pimsmStarGNode_s * pStarGNode,
                                     pimsmUpStrmStarGEventInfo_t
                                     * pUpStrmStarGEventInfo);
} pimsmUpStrmStarGSM_t;

static pimsmUpStrmStarGSM_t pimsmUpStrmStarG
[PIMSM_UPSTRM_STAR_G_SM_EVENT_MAX]
[PIMSM_UPSTRM_STAR_G_SM_STATE_MAX] =
{
  {/*EVENT_JOIN_DESIRED_TRUE*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
      pimsmUpStrmStarGJoinSend},
    /*JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
     pimsmUpStrmStarGErrorReturn}
  },
  {/*EVENT_JOIN_DESIRED_FALSE*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmStarGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmStarGPruneSend}
  },
  {/*EVENT_TIMER_EXPIRES*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmStarGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
      pimsmUpStrmStarGJoinSend}
  },
  {/*EVENT_SEE_JOIN*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmStarGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
      pimsmUpStrmStarGJoinTimerIncrease}
  },
  {/*EVENT_SEE_PRUNE*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmStarGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
      pimsmUpStrmStarGJoinTimerDecrease}
  },
  {/*EVENT_RPF_CHANGED_DUE_TO_ASSERT*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmStarGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
      pimsmUpStrmStarGJoinTimerDecrease}
  },
  {/*EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmStarGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
      pimsmUpStrmStarGJoinPruneSend}
  },
  {/*EVENT_GEN_ID_CHANGED*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmStarGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
      pimsmUpStrmStarGJoinTimerDecrease}
  }
};

static void pimsmStarGUpstreamJoinTimerExpiresHandler(void *pParam);

/******************************************************************************
* @purpose   execute the Upstream (*,G) FSM and process the events received
*
* @param    pimsmCb                 @b{(input)} PIMSM Control Block
* @param    pStarGNode              @b{(input)} (S,G) node
* @param    pUpStrmStarGEventInfo   @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmUpStrmStarGExecute(pimsmCB_t * pimsmCb,
                                pimsmStarGNode_t * pStarGNode,
                                pimsmUpStrmStarGEventInfo_t
                                * pUpStrmStarGEventInfo)
{
  pimsmStarGEntry_t       *pStarGEntry = L7_NULLPTR;
  pimsmUpStrmStarGStates_t upStrmStarGState;
  pimsmUpStrmStarGEvents_t  upStrmStarGEvent;
  L7_RC_t             rc = L7_SUCCESS;

  if((pStarGNode == (pimsmStarGNode_t *)L7_NULLPTR) ||
     (pUpStrmStarGEventInfo == (pimsmUpStrmStarGEventInfo_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "Invalid input parameters");
    return L7_FAILURE;
  }
  if((pStarGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_ERROR,
        "Node is deleted");
    return L7_FAILURE;
  }
  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  upStrmStarGState = pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamJoinState;
  upStrmStarGEvent = pUpStrmStarGEventInfo->eventType;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmUpStrmStarGStateName[upStrmStarGState],
              pimsmUpStrmStarGEventName[upStrmStarGEvent]);

  if(((pimsmUpStrmStarG[upStrmStarGEvent][upStrmStarGState]).pimsmUpStrmStarGAction
      (pimsmCb, pStarGNode,pUpStrmStarGEventInfo)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "UpStrmStarGAction Failed");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"UpStrmStarGAction Failed. State = %s, \n Event = %s",
              pimsmUpStrmStarGStateName[upStrmStarGState],
              pimsmUpStrmStarGEventName[upStrmStarGEvent]);
    pimsmStarGNodeTryRemove(pimsmCb, pStarGNode);
    return L7_FAILURE;
  }
  pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamJoinState =
  (pimsmUpStrmStarG[upStrmStarGEvent][upStrmStarGState]).nextState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "Next State = %s \n ",
              pimsmUpStrmStarGStateName[pStarGEntry->pimsmStarGUpstreamJoinState]);

  pimsmStarGNodeTryRemove(pimsmCb, pStarGNode);
  return rc;
}
/******************************************************************************
* @purpose   Send upstream (*,G) join
*
* @param    pimsmCb                 @b{(input)} PIMSM Control Block
* @param    pStarGNode              @b{(input)} (S,G) node
* @param    pUpStrmStarGEventInfo   @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmStarGJoinSend(pimsmCB_t * pimsmCb,
                                         pimsmStarGNode_t * pStarGNode,
                                         pimsmUpStrmStarGEventInfo_t
                                         * pUpStrmStarGEventInfo)
{
  pimsmStarGEntry_t  *pStarGEntry;
  pimsmSendJoinPruneData_t jpData;
  pimsmTimerData_t *pTimerData;
  L7_uint32 jpInterval = 0;
  L7_uchar8 maskLen =0;

  MCAST_UNUSED_PARAM(pUpStrmStarGEventInfo);
  pStarGEntry = &pStarGNode->pimsmStarGEntry;

    /*Set Join Timer to t_periodic;*/
  if (pStarGEntry->pimsmStarGRPIsLocal == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"RP is Local");
   return L7_SUCCESS;
  }


  /* Send Join (*,G);*/
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum = pStarGEntry->pimsmStarGRPFIfIndex;

  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, pStarGEntry->pimsmStarGRPFIfIndex,
                                     &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  jpData.joinOrPruneFlag = L7_TRUE;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.pSrcAddr = &pStarGEntry->pimsmStarGRPAddress;
  jpData.pGrpAddr = &pStarGEntry->pimsmStarGGrpAddress;
  jpData.pNbrAddr = &pStarGEntry->pimsmStarGUpstreamNeighbor;
  jpData.sendImmdFlag = pUpStrmStarGEventInfo->sendImmdFlag;
  if (pStarGEntry->pimsmStarGRPFIfIndex == 0)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"No Upstream neighbor");
   return L7_SUCCESS;
  }

  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune holdtime for rtrIfNum %d",
                jpData.rtrIfNum);
   /* return L7_FAILURE;*/
  }
  pimsmSGRptIncludePruneInMsg(pimsmCb, pStarGNode);

  if (pStarGEntry->pimsmStarGRPIsLocal == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"RP is Local");
   return L7_SUCCESS;
  }

  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb, pStarGEntry->pimsmStarGRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune interval for rtrIfNum %d",
                pStarGEntry->pimsmStarGRPFIfIndex);
    jpInterval = 60;
   /* return L7_FAILURE; */
  }
  /*Set Join Timer to t_periodic;*/
  pTimerData = &pStarGEntry->pimsmStarGUpstreamJoinTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGUpstreamJoinTimerExpiresHandler,
                            (void*)pStarGEntry->pimsmStarGUpstreamJoinTimerHandle,
                            jpInterval,
                            &(pStarGEntry->pimsmStarGUpstreamJoinTimer),
                            "xG-JT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to add (*,G) upstrm join timer");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Send upstream (*,G) prune
*
* @param    pimsmCb                 @b{(input)} PIMSM Control Block
* @param    pStarGNode              @b{(input)} (S,G) node
* @param    pUpStrmStarGEventInfo   @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmStarGPruneSend(pimsmCB_t * pimsmCb,
                                          pimsmStarGNode_t * pStarGNode,
                                          pimsmUpStrmStarGEventInfo_t
                                          * pUpStrmStarGEventInfo)
{
  pimsmStarGEntry_t  *pStarGEntry;
  pimsmSendJoinPruneData_t jpData;
  L7_uchar8 maskLen =0;

  MCAST_UNUSED_PARAM(pUpStrmStarGEventInfo);
  pStarGEntry = &pStarGNode->pimsmStarGEntry;

  /*Set Join Timer to t_periodic;*/
  if (pStarGEntry->pimsmStarGRPIsLocal != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"RP is not Local");

     /* Send Prune (*,G);*/
     memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
     jpData.rtrIfNum = pStarGEntry->pimsmStarGRPFIfIndex;

     if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                        &jpData.holdtime)
                                        != L7_SUCCESS)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,
         PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
       return L7_FAILURE;
     }


     jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
     jpData.joinOrPruneFlag = L7_FALSE;
     pimGrpMaskLenGet(pimsmCb->family,&maskLen);
     jpData.grpAddrMaskLen = maskLen;
     pimSrcMaskLenGet(pimsmCb->family,&maskLen);
     jpData.srcAddrMaskLen = maskLen;
     jpData.pSrcAddr = &pStarGEntry->pimsmStarGRPAddress;
     jpData.pGrpAddr = &pStarGEntry->pimsmStarGGrpAddress;
     jpData.pNbrAddr = &pStarGEntry->pimsmStarGUpstreamNeighbor;
     jpData.sendImmdFlag = pUpStrmStarGEventInfo->sendImmdFlag;
     if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
     }
  }

  /* Cancel Join Timer */
  pimsmUtilAppTimerCancel (pimsmCb, &(pStarGEntry->pimsmStarGUpstreamJoinTimer));
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Increase (*,G) Join Timer
*
* @param    pimsmCb                 @b{(input)} PIMSM Control Block
* @param    pStarGNode              @b{(input)} (S,G) node
* @param    pUpStrmStarGEventInfo   @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
* t_joinsuppress be the minimum of t_suppressed and the
* HoldTime from the Join/Prune message triggering this event.
* If the Join Timer is set to expire in less than t_joinsuppress
* seconds, reset it so that it expires after t_joinsuppress
* seconds.  If the Join Timer is set to expire in more than
* t_joinsuppress seconds, leave it unchanged.
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmStarGJoinTimerIncrease(pimsmCB_t * pimsmCb,
                                                  pimsmStarGNode_t * pStarGNode,
                                                  pimsmUpStrmStarGEventInfo_t
                                                  * pUpStrmStarGEventInfo)
{
  L7_uint32         suppress, holdtime, joinSuppress, timerVal;
  pimsmStarGEntry_t  *pStarGEntry;
  L7_uint32           low, high;
  pimsmTimerData_t  * pTimerData;
  L7_uint32 jpInterval = 0;

  pStarGEntry = &pStarGNode->pimsmStarGEntry;

  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb, pStarGEntry->pimsmStarGRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune interval for rtrIfNum %d",
                pStarGEntry->pimsmStarGRPFIfIndex);
    return L7_FAILURE;
  }
  /* suppress  as per draft-11
   * suppress  =   rand(1.1 *  t_periodic, 1.4 *  t_periodic) when
   * Suppression_Enabled(I) is  true, 0 otherwise  */
  low =    (L7_uint32) ((11 *  jpInterval)/10);
  high =   (L7_uint32) ((14 *  jpInterval)/10);
  suppress  = (L7_uint32) (low + PIMSM_RANDOM() % ((high - low)));
  holdtime = pUpStrmStarGEventInfo->holdtime;
  joinSuppress = min(holdtime, suppress);

  if(pStarGEntry->pimsmStarGUpstreamJoinTimer == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,G) upstrm join timer is not started");
    return L7_FAILURE;
  }
  appTimerTimeLeftGet(pimsmCb->timerCb,
                      pStarGEntry->pimsmStarGUpstreamJoinTimer, &timerVal);
  if(timerVal < joinSuppress)
  {
    /* Increase Join Timer to joinSuppress */
    pTimerData = &pStarGEntry->pimsmStarGUpstreamJoinTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);

    pTimerData->pimsmCb = pimsmCb;
    if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGUpstreamJoinTimerExpiresHandler,
                              (void*)pStarGEntry->pimsmStarGUpstreamJoinTimerHandle,
                              joinSuppress,
                              &(pStarGEntry->pimsmStarGUpstreamJoinTimer),
                              "xG-JT2")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to update (*,G) upstrm join timer");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Decrease Join Timer
*
* @param    pimsmCb                 @b{(input)} PIMSM Control Block
* @param    pStarGNode              @b{(input)} (S,G) node
* @param    pUpStrmStarGEventInfo   @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmStarGJoinTimerDecrease(pimsmCB_t * pimsmCb,
                                                  pimsmStarGNode_t * pStarGNode,
                                                  pimsmUpStrmStarGEventInfo_t
                                                  * pUpStrmStarGEventInfo)
{
  pimsmStarGEntry_t      *pStarGEntry;
  L7_RC_t rc;
  pimsmTimerData_t *pTimerData;
  L7_uint32  jpOverrideInterval;
  L7_uint32  timeLeft = 0;

  MCAST_UNUSED_PARAM(pUpStrmStarGEventInfo);
  pStarGEntry = &pStarGNode->pimsmStarGEntry;

  if(pStarGEntry->pimsmStarGUpstreamJoinTimer == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,G) upstrm join timer is not started");
    return L7_FAILURE;
  }
  pimsmEffectiveOverrideInterval(pimsmCb, pStarGEntry->pimsmStarGRPFIfIndex,
                                 &jpOverrideInterval);
  rc = appTimerTimeLeftGet(pimsmCb->timerCb,
                           pStarGEntry->pimsmStarGUpstreamJoinTimer, &timeLeft);
  if((L7_SUCCESS == rc) &&
     (timeLeft > jpOverrideInterval))
  {
    /*Decrease Join Timer to t_override;*/
    pTimerData = &pStarGEntry->pimsmStarGUpstreamJoinTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
    pTimerData->pimsmCb = pimsmCb;

    if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGUpstreamJoinTimerExpiresHandler,
                              (void*)pStarGEntry->pimsmStarGUpstreamJoinTimerHandle,
                              jpOverrideInterval,
                              &(pStarGEntry->pimsmStarGUpstreamJoinTimer),
                              "xG-JT3")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to update (*,G) upstrm join timer");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Send upstream (*,G) join to new nexthop and (*,G) Prune to old Nexthop
*
* @param    pimsmCb                 @b{(input)} PIMSM Control Block
* @param    pStarGNode              @b{(input)} (S,G) node
* @param    pUpStrmStarGEventInfo   @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmStarGJoinPruneSend(pimsmCB_t * pimsmCb,
                                              pimsmStarGNode_t * pStarGNode,
                                              pimsmUpStrmStarGEventInfo_t
                                              * pUpStrmStarGEventInfo)
{
  pimsmStarGEntry_t     * pStarGEntry;
  pimsmSendJoinPruneData_t jpData;
  pimsmTimerData_t *pTimerData;
  L7_uint32 jpInterval = 0;
  L7_uchar8 maskLen =0;

  pStarGEntry = &pStarGNode->pimsmStarGEntry;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"newRPFIfIndex =%d ",
                                pStarGEntry->pimsmStarGRPFIfIndex);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G,  PIMSM_TRACE_INFO,"newRPF NextHop = ",
                                      &pStarGEntry->pimsmStarGRPFNextHop);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"newRPFRouteProtocol = %d",
                                pStarGEntry->pimsmStarGRPFRouteProtocol);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G,  PIMSM_TRACE_INFO,"newRPF RouteAddress :",
                                      &pStarGEntry->pimsmStarGRPFRouteAddress);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"newRPFRouteMetricPref = %d",
                                pStarGEntry->pimsmStarGRPFRouteMetricPref);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"newRPFRouteMetric =%d",
                                 pStarGEntry->pimsmStarGRPFRouteMetric);



  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  /*Send Prune (*,G) to old next hop;*/
  jpData.rtrIfNum =  pUpStrmStarGEventInfo->rpfInfo.rpfIfIndex;
  if(jpData.rtrIfNum != L7_NULL)
  {
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                     &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }
  jpData.pNbrAddr = &pUpStrmStarGEventInfo->upStrmNbrAddr;
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  jpData.joinOrPruneFlag = L7_FALSE;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.pGrpAddr = &pStarGEntry->pimsmStarGGrpAddress;
  jpData.pSrcAddr = &pUpStrmStarGEventInfo->rpAddr;
  jpData.sendImmdFlag = pUpStrmStarGEventInfo->sendImmdFlag;
  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
  }
  }
  if (pStarGEntry->pimsmStarGRPIsLocal == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"RP is Local");
   return L7_SUCCESS;
  }

 /* TBD : May be the pimsmStarGJoinDesired function need not be called
          over here */

  if (pimsmStarGJoinDesired(pimsmCb,pStarGNode) == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,"Star G Join Desired is false ");
   return L7_SUCCESS;
  }

  /*Send Join (*,G) to new next hop;*/
  jpData.rtrIfNum = pStarGEntry->pimsmStarGRPFIfIndex;
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                     &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  jpData.joinOrPruneFlag = L7_TRUE;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.pNbrAddr = &pStarGEntry->pimsmStarGUpstreamNeighbor;
  jpData.pGrpAddr = &pStarGEntry->pimsmStarGGrpAddress;
  jpData.pSrcAddr = &pStarGEntry->pimsmStarGRPAddress;
  jpData.sendImmdFlag = pUpStrmStarGEventInfo->sendImmdFlag;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO," new RP address ",
                    &pStarGEntry->pimsmStarGRPAddress);

  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to send  Join/Prune Message");
    return L7_FAILURE;
  }
  pimsmSGRptIncludePruneInMsg(pimsmCb, pStarGNode);

  /*Set Join Timer to t_periodic;*/
  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb,
                                          pStarGEntry->pimsmStarGRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune interval for rtrIfNum %d",
                pStarGEntry->pimsmStarGRPFIfIndex);
    return L7_FAILURE;
  }
  pTimerData = &pStarGEntry->pimsmStarGUpstreamJoinTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);

  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGUpstreamJoinTimerExpiresHandler,
                            (void*)pStarGEntry->pimsmStarGUpstreamJoinTimerHandle,
                            jpInterval,
                            &(pStarGEntry->pimsmStarGUpstreamJoinTimer),
                            "xG-JT4")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to add (*,G) upstrm join timer");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Unexpected event, return Error
*
* @param    pimsmCb                 @b{(input)} PIMSM Control Block
* @param    pStarGNode              @b{(input)} (S,G) node
* @param    pUpStrmStarGEventInfo   @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmUpStrmStarGErrorReturn(pimsmCB_t * pimsmCb,
                                           pimsmStarGNode_t * pStarGNode,
                                           pimsmUpStrmStarGEventInfo_t
                                           * pUpStrmStarGEventInfo)
{
  MCAST_UNUSED_PARAM(pStarGNode);
  MCAST_UNUSED_PARAM(pUpStrmStarGEventInfo);
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  Action Routine when (*,G) Upstream Join Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmStarGUpstreamJoinTimerExpiresHandler(void *pParam)
{
  pimsmUpStrmStarGEventInfo_t upStrmStarGEventInfo;
  pimsmStarGNode_t *pStarGNode;
  pimsmCB_t * pimsmCb;
  L7_inet_addr_t *pGrpAddr;
  L7_int32      handle = (L7_int32)pParam;
  pimsmTimerData_t *pTimerData;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  pimsmCb = pTimerData->pimsmCb;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);
  if (pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_NORMAL,
        "(*,G) Find failed");
    return;
  }
  if (pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamJoinTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmStarGUpstreamJoinTimer is NULL, But Still Expired");
    return;
  }
  pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamJoinTimer = L7_NULLPTR;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "(*,G) Upstream JoinTimer Expired");

  memset(&upStrmStarGEventInfo, 0, sizeof(pimsmUpStrmStarGEventInfo_t));

  upStrmStarGEventInfo.eventType = PIMSM_UPSTRM_STAR_G_SM_EVENT_TIMER_EXPIRES;
  upStrmStarGEventInfo.sendImmdFlag = L7_FALSE;

  pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &upStrmStarGEventInfo);
}
