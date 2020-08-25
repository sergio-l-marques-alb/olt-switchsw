/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmUpStrmSGFSM.c
*
* @purpose Contains implementation of (S,G) Upstream State Machine
*
* @component
*
* @comments
*
* @create 08/18/2005
*
* @author vmurali
* @end
*
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmupstrmsgfsm.h"
#include "pimsmtimer.h"
#include "pimsmwrap.h"

static L7_RC_t  pimsmUpStrmSGJoinSend
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo);
static L7_RC_t  pimsmUpStrmSGPruneSend
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo);
static L7_RC_t  pimsmUpStrmSGJoinTimerIncrease
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo);
static L7_RC_t  pimsmUpStrmSGJoinTimerDecrease
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo);
static L7_RC_t  pimsmUpStrmSGJoinPruneSend
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo);
static L7_RC_t  pimsmUpStrmSGErrorReturn
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo);
typedef struct pimsmUpStrmSGSM_s
{
  pimsmUpStrmSGStates_t nextState;
  L7_RC_t (*pimsmUpStrmSGAction) (pimsmCB_t * pimsmCb,
                                  pimsmSGNode_t * pSGNode,
                                  pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo);
} pimsmUpStrmSGSM_t;

static pimsmUpStrmSGSM_t pimsmUpStrmSG
[PIMSM_UPSTRM_S_G_SM_EVENT_MAX]
[PIMSM_UPSTRM_S_G_SM_STATE_MAX] =
{
  {/*EVENT_JOIN_DESIRED_TRUE*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinSend},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGErrorReturn}
  },
  {/*EVENT_JOIN_DESIRED_FALSE*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGPruneSend}
  },
  {/*EVENT_TIMER_EXPIRES*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinSend}
  },
  {/*EVENT_SEE_JOIN*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinTimerIncrease}
  },
  {/*EVENT_SEE_PRUNE_S_G*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinTimerDecrease}
  },
  {/*EVENT_SEE_PRUNE_S_G_RPT*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinTimerDecrease}
  },
  {/*EVENT_SEE_PRUNE_STAR_G*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinTimerDecrease}
  },
  {/*EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinPruneSend}
  },
  {/*EVENT_GEN_ID_CHANGED*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinTimerDecrease}
  },
  {/*CHANGED_DUE_TO_ASSERT*/
    /*NOT_JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED,
      pimsmUpStrmSGErrorReturn},
    /*JOINED*/  {PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
      pimsmUpStrmSGJoinTimerDecrease}
  }
};

static void pimsmSGUpstreamJoinTimerExpiresHandler(void *pParam);
static L7_RC_t pimsmSGUpstreamJoinPruneSend (pimsmCB_t *pimsmCb,
                              pimsmSendJoinPruneData_t *pJPData,
                              pimsmSGNode_t *pSGNode);

/******************************************************************************
* @purpose   execute the Upstream (S,G) FSM and process the events received
*
* @param    pimsmCb              @b{(input)} PIMSM Control Block
* @param    pSGNode              @b{(input)} (S,G) node
* @param    pUpStrmSGEventInfo   @b{(input)} Upstream SG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmUpStrmSGExecute (pimsmCB_t * pimsmCb,
                             pimsmSGNode_t * pSGNode,
                             pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo)
{
  pimsmSGEntry_t        *pSGEntry;
  pimsmUpStrmSGStates_t  upStrmSGState;
  pimsmUpStrmSGEvents_t upStrmSGEvent;
  L7_RC_t         rc = L7_SUCCESS;

  if(pSGNode == (pimsmSGNode_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "Invalid input parameters");
    return L7_FAILURE;
  }
  if(pUpStrmSGEventInfo == (pimsmUpStrmSGEventInfo_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "Invalid event info");
    return L7_FAILURE;
  }
  if((pSGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
        "Node is deleted");
    return L7_FAILURE;
  }
  pSGEntry = &pSGNode->pimsmSGEntry;
  upStrmSGState = pSGEntry->pimsmSGUpstreamJoinState;
  upStrmSGEvent = pUpStrmSGEventInfo->eventType;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmUpStrmSGStateName[upStrmSGState],
              pimsmUpStrmSGEventName[upStrmSGEvent]);

  if(((pimsmUpStrmSG[upStrmSGEvent][upStrmSGState]).pimsmUpStrmSGAction
      (pimsmCb, pSGNode,pUpStrmSGEventInfo)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_NORMAL, "UpStrmSGAction Failed");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"UpStrmSGAction Failed. State = %s, \n Event = %s",
      pimsmUpStrmSGStateName[upStrmSGState],
      pimsmUpStrmSGEventName[upStrmSGEvent]);
    pimsmSGNodeTryRemove(pimsmCb, pSGNode);
    return L7_FAILURE;
  }
  pSGNode->pimsmSGEntry.pimsmSGUpstreamJoinState =
  (pimsmUpStrmSG[upStrmSGEvent][upStrmSGState]).nextState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO, "Next State = %s \n ",
              pimsmUpStrmSGStateName[pSGEntry->pimsmSGUpstreamJoinState]);
  pimsmSGNodeTryRemove(pimsmCb, pSGNode);
  return(rc);
}
/******************************************************************************
* @purpose   Send upstream (S,G) join
*
* @param    pimsmCb              @b{(input)} PIMSM Control Block
* @param    pSGNode              @b{(input)} (S,G) node
* @param    pUpStrmSGEventInfo   @b{(input)} Upstream SG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGJoinSend(pimsmCB_t * pimsmCb,
                                     pimsmSGNode_t * pSGNode,
                                     pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo)
{
  pimsmSGEntry_t      *pSGEntry;
  pimsmSendJoinPruneData_t jpData;
  pimsmTimerData_t *pTimerData;
  L7_uint32 jpInterval = 0;
  L7_uchar8 maskLen =0;

  MCAST_UNUSED_PARAM(pUpStrmSGEventInfo);
  pSGEntry = &pSGNode->pimsmSGEntry;
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  /* Send Join (S,G)*/
  jpData.rtrIfNum = pSGEntry->pimsmSGRPFIfIndex;

  if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,pSGEntry->pimsmSGRPFIfIndex) != L7_TRUE)
  {
    if (mcastIpMapIsDirectlyConnected(&pSGEntry->pimsmSGSrcAddress,
                                      L7_NULLPTR) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,
                  "Upstream is a local address");

      return L7_SUCCESS;
    }
  }


  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb,
                                        jpData.rtrIfNum,
                                     &jpData.holdtime) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune holdtime for rtrIfNum %d",
                jpData.rtrIfNum);
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,"Join prune Hold time for rtrIfNum = %d is %d",
                                pSGEntry->pimsmSGRPFIfIndex, jpData.holdtime);
  jpData.pNbrAddr = &pSGEntry->pimsmSGUpstreamNeighbor;
  jpData.addrFlags = 0;
  jpData.joinOrPruneFlag = L7_TRUE;
  jpData.pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.sendImmdFlag = pUpStrmSGEventInfo->sendImmdFlag;
  if(pimsmSGUpstreamJoinPruneSend (pimsmCb, &jpData, pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
/*    return L7_FAILURE; */
  }

  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb, pSGEntry->pimsmSGRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
                "Failed to get JoinPrune interval for rtrIfNum = %d",
                pSGEntry->pimsmSGRPFIfIndex);
    return L7_FAILURE;
  }
  /*Set Join Timer to t_periodic;*/
  pTimerData = &pSGEntry->pimsmSGUpstreamJoinTimerParam ;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGUpstreamJoinTimerExpiresHandler,
                            (void*)pSGEntry->pimsmSGUpstreamJoinTimerHandle,
                            jpInterval,
                            &(pSGEntry->pimsmSGUpstreamJoinTimer),
                            "SG-JT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "Failed to add (S,G) upstrm join timer");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Send upstream (S,G) prune
*
* @param    pimsmCb              @b{(input)} PIMSM Control Block
* @param    pSGNode              @b{(input)} (S,G) node
* @param    pUpStrmSGEventInfo   @b{(input)} Upstream SG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGPruneSend(pimsmCB_t * pimsmCb,
                                       pimsmSGNode_t * pSGNode,
                                       pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo)
{
  pimsmSGEntry_t        *pSGEntry;
  pimsmSendJoinPruneData_t  jpData;
  L7_uchar8 maskLen =0;
  L7_BOOL unnumbered = L7_FALSE;


  MCAST_UNUSED_PARAM(pUpStrmSGEventInfo);
  pSGEntry = &pSGNode->pimsmSGEntry;

  /*Send Prune (S,G)*/
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum = pSGEntry->pimsmSGRPFIfIndex;

  unnumbered = mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,pSGEntry->pimsmSGRPFIfIndex);
  if ((unnumbered == L7_TRUE) ||
      (mcastIpMapIsDirectlyConnected(&pSGEntry->pimsmSGSrcAddress,
                                      L7_NULLPTR) != L7_TRUE))
   {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,
                  "Upstream is a not local address");

      if (pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb,
                                             pSGEntry->pimsmSGRPFIfIndex,
                                             &jpData.holdtime)
          != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,
                    PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
        return L7_FAILURE;
      }
      jpData.addrFlags = 0;
      pimSrcMaskLenGet(pimsmCb->family,&maskLen);
      jpData.srcAddrMaskLen = maskLen;
      pimGrpMaskLenGet(pimsmCb->family,&maskLen);
      jpData.grpAddrMaskLen = maskLen;
      jpData.joinOrPruneFlag = L7_FALSE;
      jpData.pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
      jpData.pNbrAddr = &pSGEntry->pimsmSGUpstreamNeighbor;
      jpData.pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
      jpData.sendImmdFlag = pUpStrmSGEventInfo->sendImmdFlag;
      /* Send Prune Echo (S,G); */
      if (pimsmSGUpstreamJoinPruneSend (pimsmCb, &jpData, pSGNode) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
        /*    return L7_FAILURE; */
      }
    }

  /*   Set SPTbit (S,G) to FALSE; */
  pSGEntry->pimsmSGSPTBit = L7_FALSE;

  /*Cancel Join Timer;*/
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGEntry->pimsmSGUpstreamJoinTimer));

  /****** NOTE :
    Deleting SG-MFC entry is a concern for ANVL 21.18 test case
   *******/
#if 0
  /*Delete Entry from MFC as SPT bit is set to FALSE */
  pimsmSGMFCUpdate(pimsmCb,pSGNode,MFC_DELETE_ENTRY, L7_FALSE);
#endif
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose   Increase Join Timer
*
* @param    pimsmCb              @b{(input)} PIMSM Control Block
* @param    pSGNode              @b{(input)} (S,G) node
* @param    pUpStrmSGEventInfo   @b{(input)} Upstream SG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
* t_joinsuppress be the minimum of t_suppressed and the   HoldTime from the
* Join/Prune message triggering this event.   If the Join Timer is set to
* expire in less than t_joinsuppress   seconds, reset it so that it expires
* after t_joinsuppress   seconds. If the Join Timer is set to expire in more
* than   t_joinsuppress seconds, leave it unchanged.
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGJoinTimerIncrease(pimsmCB_t * pimsmCb,
                                               pimsmSGNode_t * pSGNode,
                                               pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo)
{
  L7_uint32     suppress, joinSuppress, timerVal = 0 ;
  pimsmSGEntry_t  *pSGEntry;
  L7_uint32     low, high, holdtime;
  pimsmTimerData_t *pTimerData;
  L7_uint32 jpInterval = 0;

  MCAST_UNUSED_PARAM(pUpStrmSGEventInfo);
  pSGEntry = &pSGNode->pimsmSGEntry;
  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb, pSGEntry->pimsmSGRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune holdtime for rtrIfNum %d",
               pSGEntry->pimsmSGRPFIfIndex);
    return L7_FAILURE;
  }
  /* suppress  as per draft-11
  suppress  =   rand(1.1 *  t_periodic, 1.4 *  t_periodic)
  when  Suppression_Enabled(I) is  true, 0 otherwise  */
  low =    (L7_uint32) ((11 *  jpInterval)/10);
  high =   (L7_uint32) ((14 *  jpInterval)/10);
  suppress  = (L7_uint32) (low + PIMSM_RANDOM() % ((high - low)));

  holdtime = pUpStrmSGEventInfo->holdtime;

  joinSuppress = min(holdtime, suppress);

  if(pSGEntry->pimsmSGUpstreamJoinTimer == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "(S,G) upstrm join timer is not started");
    return L7_FAILURE;
  }

  appTimerTimeLeftGet(pimsmCb->timerCb,
                      pSGEntry->pimsmSGUpstreamJoinTimer, &timerVal);
  if(timerVal < joinSuppress)
  {
    /*Increase Join Timer to joinSuppress;*/
    pTimerData = &pSGEntry->pimsmSGUpstreamJoinTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
    inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
    pTimerData->pimsmCb = pimsmCb;
    if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGUpstreamJoinTimerExpiresHandler,
                              (void*)pSGEntry->pimsmSGUpstreamJoinTimerHandle,
                              joinSuppress,
                              &(pSGEntry->pimsmSGUpstreamJoinTimer),
                              "SG-JT2")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "Failed to update (S,G) upstrm join timer");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Decrease Join Timer
*
* @param    pimsmCb              @b{(input)} PIMSM Control Block
* @param    pSGNode              @b{(input)} (S,G) node
* @param    pUpStrmSGEventInfo   @b{(input)} Upstream SG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
* If the Join Timer is set to expire in more than t_override
* seconds, reset it so that it expires after t_override seconds.
* If the Join Timer is set to expire in less than t_override
* seconds, leave it unchanged.
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGJoinTimerDecrease (pimsmCB_t * pimsmCb,
                                               pimsmSGNode_t * pSGNode,
                                               pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo)
{
  pimsmTimerData_t *pTimerData;
  pimsmSGEntry_t   *pSGEntry;
  L7_uint32  joinPruneOverrideInterval;
  L7_RC_t  rc;
  L7_uint32  timeLeft = 0;

  MCAST_UNUSED_PARAM(pUpStrmSGEventInfo);

  pSGEntry = &pSGNode->pimsmSGEntry;

  if(pSGEntry->pimsmSGUpstreamJoinTimer == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "(S,G) upstrm join timer is not started");
    return L7_FAILURE;
  }
  pimsmEffectiveOverrideInterval(pimsmCb, pSGEntry->pimsmSGRPFIfIndex,
                                 &joinPruneOverrideInterval);

  rc = appTimerTimeLeftGet(pimsmCb->timerCb,
                           pSGEntry->pimsmSGUpstreamJoinTimer, &timeLeft);
  if((L7_SUCCESS == rc) &&
     (timeLeft > joinPruneOverrideInterval))
  {
    /*Decrease Join Timer to t_override;*/
    pTimerData = &pSGEntry->pimsmSGUpstreamJoinTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
    inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
    pTimerData->pimsmCb = pimsmCb;
    if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGUpstreamJoinTimerExpiresHandler,
                              (void*)pSGEntry->pimsmSGUpstreamJoinTimerHandle,
                              joinPruneOverrideInterval,
                              &(pSGEntry->pimsmSGUpstreamJoinTimer),
                              "SG-JT3")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "Failed to update (S,G) upstrm join timer");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Send upstream (S,G) prune to old Nexthop
*
* @param    pimsmCb              @b{(input)} PIMSM Control Block
* @param    pSGNode              @b{(input)} (S,G) node
* @param    pUpStrmSGEventInfo   @b{(input)} Upstream SG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGJoinPruneSend(pimsmCB_t * pimsmCb,
                       pimsmSGNode_t * pSGNode,
                       pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo)
{
  pimsmSGEntry_t   *pSGEntry;
  pimsmSendJoinPruneData_t  jpData;
  pimsmTimerData_t *pTimerData;
  L7_uint32 jpInterval = 0;
  L7_uchar8 maskLen =0;

  pSGEntry = &pSGNode->pimsmSGEntry;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,"newRPFIfIndex =%d ",
                                pSGEntry->pimsmSGRPFIfIndex);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"newRPF NextHop = ",
                                      &pSGEntry->pimsmSGRPFNextHop);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,"newRPFRouteProtocol = %d",
                                pSGEntry->pimsmSGRPFRouteProtocol);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,"newRPF RouteAddress :",
                                      &pSGEntry->pimsmSGRPFRouteAddress);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,"newRPFRouteMetricPref = %d",
                                pSGEntry->pimsmSGRPFRouteMetricPref);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,"newRPFRouteMetric =%d",
                                 pSGEntry->pimsmSGRPFRouteMetric);

  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum =  pUpStrmSGEventInfo->rpfInfo.rpfIfIndex;
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                     &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }

  jpData.pNbrAddr
  =  &pUpStrmSGEventInfo->upStrmNbrAddr;
  jpData.addrFlags  = 0;
  jpData.joinOrPruneFlag = L7_FALSE;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
  jpData.pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
  jpData.sendImmdFlag = pUpStrmSGEventInfo->sendImmdFlag;
  /* Send Prune (S,G); */
  if(pimsmSGUpstreamJoinPruneSend (pimsmCb, &jpData, pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
  }

  if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,pSGEntry->pimsmSGRPFIfIndex) != L7_TRUE)
  {
    if (mcastIpMapIsDirectlyConnected(&pSGEntry->pimsmSGSrcAddress,
                                      L7_NULLPTR) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,
                  "Upstream is a local address");
      return L7_SUCCESS;
    }
  }

 /* TBD : May be the pimsmSGJoinDesired function need not be called
          over here */

  if (pimsmSGJoinDesired(pimsmCb,pSGNode) == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_INFO,"SG Join Desired is false ");
   return L7_SUCCESS;
  }


  /*Send Join (S,G) to new next hop;*/
  jpData.rtrIfNum   = pSGEntry->pimsmSGRPFIfIndex;
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                     &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }

  jpData.addrFlags  = 0;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.joinOrPruneFlag = L7_TRUE;
  jpData.pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
  jpData.pNbrAddr =  &pSGEntry->pimsmSGUpstreamNeighbor;
  jpData.pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
  jpData.sendImmdFlag = pUpStrmSGEventInfo->sendImmdFlag;
  if(pimsmSGUpstreamJoinPruneSend (pimsmCb, &jpData, pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
    return L7_FAILURE;
  }

  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb, pSGEntry->pimsmSGRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
                "Failed to get JoinPrune interval for rtrIfNum = %d",
                pSGEntry->pimsmSGRPFIfIndex);
    return L7_FAILURE;
  }
  /*Set Join Timer to t_periodic;*/
  pTimerData = &pSGEntry->pimsmSGUpstreamJoinTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGUpstreamJoinTimerExpiresHandler,
                            (void*)pSGEntry->pimsmSGUpstreamJoinTimerHandle,
                            jpInterval,
                            &(pSGEntry->pimsmSGUpstreamJoinTimer),
                            "SG-JT4")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "Failed to add (S,G) upstrm join timer");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Unexpected event, return Error
*
* @param    pimsmCb              @b{(input)} PIMSM Control Block
* @param    pSGNode              @b{(input)} (S,G) node
* @param    pUpStrmSGEventInfo   @b{(input)} Upstream SG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmUpStrmSGErrorReturn(pimsmCB_t * pimsmCb,
                                        pimsmSGNode_t * pSGNode,
                                        pimsmUpStrmSGEventInfo_t * pUpStrmSGEventInfo)
{
  MCAST_UNUSED_PARAM(pSGNode);
  MCAST_UNUSED_PARAM(pUpStrmSGEventInfo);
  return(L7_FAILURE);
}


/******************************************************************************
* @purpose  Action Routine when (S,G) Upstream Join Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmSGUpstreamJoinTimerExpiresHandler(void *pParam)
{
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  pimsmSGNode_t *pSGNode;
  pimsmCB_t   * pimsmCb;
  L7_int32      handle = (L7_int32)pParam;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;
  pimsmTimerData_t *pTimerData;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  pimsmCb = pTimerData->pimsmCb;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_NORMAL, "(S,G) Upstream Join Timer Expired");

  pSrcAddr = &pTimerData->addr1;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
      "Source Address :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);

  if (pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_NORMAL,
        "(S,G) Find failed");
    return;
  }

  if (pSGNode->pimsmSGEntry.pimsmSGUpstreamJoinTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmSGUpstreamJoinTimer is NULL, But Still Expired");
    return;
  }
  pSGNode->pimsmSGEntry.pimsmSGUpstreamJoinTimer = L7_NULLPTR;


  memset(&upStrmSGEventInfo, 0, sizeof(pimsmUpStrmSGEventInfo_t));
  upStrmSGEventInfo.eventType = PIMSM_UPSTRM_S_G_SM_EVENT_TIMER_EXPIRES;

  pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
}


/******************************************************************************
* @purpose  Action Routine when (S,G) Keepalive Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
void pimsmSGKeepaliveTimerExpiresHandler(void *pParam)
{
  pimsmSGNode_t *pSGNode;
  L7_BOOL        rc;
  L7_RC_t        rc2;
  pimsmPerSGRegisterEventInfo_t perSGRegisterEventInfo;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_BOOL joinDesired;
  L7_int32      handle = (L7_int32)pParam;
  pimsmTimerData_t *pTimerData ;
  pimsmSGEntry_t  *pSGEntry;
  pimsmCB_t * pimsmCb;
  L7_inet_addr_t  rpAddr;
  mfcEntry_t mfcEntry;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  pimsmCb = pTimerData->pimsmCb;
  pSrcAddr = &pTimerData->addr1;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
      "Source Address :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);
  if (pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_NORMAL,
        "(S,G) Find failed");
    return;
  }
  pSGEntry = &pSGNode->pimsmSGEntry;

  if (pSGEntry->pimsmSGKeepaliveTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmSGKeepaliveTimer is NULL, But Still Expired");
    return;
  }
  pSGEntry->pimsmSGKeepaliveTimer = L7_NULLPTR;

  memset(&mfcEntry,0, sizeof(mfcEntry_t));
  inetCopy(&mfcEntry.group, &pSGEntry->pimsmSGGrpAddress);
  inetCopy(&mfcEntry.source, &pSGEntry->pimsmSGSrcAddress);

  if (mfcIsEntryInUse(&mfcEntry) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "MFC Entry is active" );

    if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGKeepaliveTimerExpiresHandler,
                              (void*) pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimerHandle,
                              PIMSM_DEFAULT_KEEPALIVE_PERIOD,
                              &(pSGNode->pimsmSGEntry.pimsmSGKeepaliveTimer),
                              "SM-KAT3")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Failed to add (S,G)KeepaliveTimer ");
      return;
    }
    return;
  }
  rc = pimsmCouldRegister(pimsmCb, pSGNode);
  if(rc == L7_TRUE)
  {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
                   "Error: pimsmCouldRegister() is true");
  }
  else
  {
    perSGRegisterEventInfo.eventType =
    PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_FALSE;
    pimsmPerSGRegisterExecute(pimsmCb, pSGNode, &perSGRegisterEventInfo);
  }
  memset(&upStrmSGEventInfo, 0, sizeof(pimsmUpStrmSGEventInfo_t));
  joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
  if(joinDesired == L7_TRUE)
  {
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
  /*TODO: JoinDesired(S,G) triggers SGIAssertTrackingDesired
    Do I need to loop for all interfaces ?*/
#ifdef PIMSM_TBD
  pimsmPerIntfSGAssertEventInfo_t sgAsrtEvData;

  rc = pimsmSGIAssertTrackingDesired(pimsmCb, pSGNode, rtrIfNum);
  if(rc == L7_FALSE)
  {
    sgAsrtEvData.eventType =
    PIMSM_ASSERT_S_G_ASSERT_TRCKNG_DESRD_FALSE;
    pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode, &sgAsrtEvData);
  }
#endif


  rc2 = pimsmRpAddressGet(pimsmCb, &pSGEntry->pimsmSGGrpAddress, &rpAddr);
  if(rc2 != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Failed to get RP address for Group : ",
              &pSGEntry->pimsmSGGrpAddress);
    return ;
  }
  if(mcastIpMapUnnumberedIsLocalAddress(&rpAddr, L7_NULLPTR) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "RP address is local");
    inetAddressZeroSet(pimsmCb->family,
                       &pSGEntry->pimsmSGRPRegisterPMBRAddress);
  }
  if(pimsmSGDelete(pimsmCb, &pSGEntry->pimsmSGGrpAddress,
                   &pSGEntry->pimsmSGSrcAddress) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, "Failed to delete (S,G) node");
  }
  return;
}

/******************************************************************************
* @purpose  Send a Join/Prune Message
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns  none
*
* @comments This API does the following...
*            If the Group Address is not in SSM-Range:
*            > If you are the RP:
*              * Imdt Send if Register State is JOIN.
*              * Bundle Send if Register State is otherthan JOIN.
*            > If you not the RP:
*              * Bundle Send always.
*
* @end
******************************************************************************/
static L7_RC_t
pimsmSGUpstreamJoinPruneSend (pimsmCB_t *pimsmCb,
                              pimsmSendJoinPruneData_t *pJPData,
                              pimsmSGNode_t *pSGNode)
{
  L7_inet_addr_t *pGrpAddr = L7_NULLPTR;
  L7_inet_addr_t rpAddr;
  L7_BOOL isRpLocal = L7_FALSE;

  pGrpAddr = pJPData->pGrpAddr;

  /* Check if Group is in SSM Range */
  if (pimsmMapIsInSsmRange (pimsmCb->family, pGrpAddr) != L7_TRUE)
  {
    /* Check if you are the RP for this Group */
    if (pimsmRpAddressGet (pimsmCb, pGrpAddr, &rpAddr) != L7_SUCCESS)
    {
      PIMSM_TRACE (PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "RP not found" );
      return L7_FAILURE;
    }

    if (mcastIpMapIsLocalAddress (&rpAddr, L7_NULLPTR) == L7_TRUE)
    {
      PIMSM_TRACE (PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "I am the RP for this group ");
      isRpLocal = L7_TRUE;
    }
  }

  if (isRpLocal == L7_TRUE)
  {
    if (pSGNode->pimsmSGEntry.pimsmSGDRRegisterState == PIMSM_REG_PER_S_G_SM_STATE_JOIN)
    {
      if(pimsmJoinPruneImdtSend (pimsmCb, pJPData) != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
        return L7_FAILURE;
      }

      return L7_SUCCESS;
    }
  }

  if (pimsmJoinPruneSend (pimsmCb, pJPData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

