/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmUpStrmStarStarRpFSM.c
*
* @purpose Contains implementation for (*,*,RP) Upstream Statemachine
*
* @component
*
* @comments
*
* @create 08/18/2005
*
* @author Muralidhar Vakkalagadda / Satya Dillikar
* @end
*
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmupstrmstarstarrpfsm.h"
#include "pimsmtimer.h"
#include "pimsmwrap.h"


struct pimsmStarStarRpEntry_s;


static L7_RC_t  pimsmUpStrmPerIntfStarStarRPJoinSend
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s * pStarStarRpNode,
 pimsmUpStrmStarStarRPEventInfo_t * pUpStrmStarStarRpEventInfo);
static L7_RC_t  pimsmUpStrmPerIntfStarStarRPPruneSend
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s * pStarStarRpNode,
 pimsmUpStrmStarStarRPEventInfo_t * pUpStrmStarStarRpEventInfo);
static L7_RC_t  pimsmUpStrmPerIntfStarStarRPJoinTimerIncrease
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s * pStarStarRpNode,
 pimsmUpStrmStarStarRPEventInfo_t * pUpStrmStarStarRpEventInfo);
static L7_RC_t  pimsmUpStrmPerIntfStarStarRPJoinTimerDecrease
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s * pStarStarRpNode,
 pimsmUpStrmStarStarRPEventInfo_t * pUpStrmStarStarRpEventInfo);
static L7_RC_t  pimsmUpStrmPerIntfStarStarRPJoinPruneSend
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s * pStarStarRpNode,
 pimsmUpStrmStarStarRPEventInfo_t * pUpStrmStarStarRpEventInfo);
static L7_RC_t pimsmUpStrmPerIntfStarStarRPErrorReturn
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s * pStarStarRpNode,
 pimsmUpStrmStarStarRPEventInfo_t * pUpStrmStarStarRpEventInfo);


typedef struct pimsmUpStrmStarStarRPFSM_s
{
  pimsmUpStrmStarStarRPStates_t nextState;
  L7_RC_t (*pimsmUpStrmStarStarRPAction)
  (pimsmCB_t * pimsmCb,
   struct pimsmStarStarRpNode_s * pStarStarRpEntry,
   pimsmUpStrmStarStarRPEventInfo_t * pUpStrmStarStarRpEventInfo);
} pimsmUpStrmStarStarRPFSM_t;

static pimsmUpStrmStarStarRPFSM_t pimsmUpStrmPerIntfStarStarRP
[PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_MAX]
[PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_MAX] =
{
  /*  join desired true*/
  {
    /* Not Joined */    {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_JOINED,
      pimsmUpStrmPerIntfStarStarRPJoinSend},
    /* Joined */        {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_JOINED,
      pimsmUpStrmPerIntfStarStarRPErrorReturn}
  },
  /* join desired false */
  {
    /* Not Joined */    {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED,
      pimsmUpStrmPerIntfStarStarRPErrorReturn},
    /* Joined */        {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED,
      pimsmUpStrmPerIntfStarStarRPPruneSend}
  },
  /* timer expires */
  {
    /* Not Joined */    {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED,
      pimsmUpStrmPerIntfStarStarRPErrorReturn},
    /* Joined */        {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_JOINED,
      pimsmUpStrmPerIntfStarStarRPJoinSend}
  },
  /* see join */
  {
    /* Not Joined */    {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED,
      pimsmUpStrmPerIntfStarStarRPErrorReturn},
    /* Joined */        {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_JOINED,
      pimsmUpStrmPerIntfStarStarRPJoinTimerIncrease}
  },
  /* see prune */
  {
    /* Not Joined */    {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED,
      pimsmUpStrmPerIntfStarStarRPErrorReturn},
    /* Joined */        {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_JOINED,
      pimsmUpStrmPerIntfStarStarRPJoinTimerDecrease}
  },
  /* NBR changed */
  {
    /* Not Joined */    {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED,
      pimsmUpStrmPerIntfStarStarRPErrorReturn},
    /* Joined */        {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_JOINED,
      pimsmUpStrmPerIntfStarStarRPJoinPruneSend}
  },
  /* GEN-ID changed*/
  {
    /* Not Joined */    {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED,
      pimsmUpStrmPerIntfStarStarRPErrorReturn},
    /* Joined */        {PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_JOINED,
      pimsmUpStrmPerIntfStarStarRPJoinTimerDecrease}
  }
};

static void pimsmStarStarRpUpstreamJoinTimerExpiresHandler(void *pParam);

/******************************************************************************
* @purpose   execute the Upstream (*,*,G) FSM and process the events received
*
* @param    pimsmCb                    @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode            @b{(input)} (S,G) node
* @param    pUpStrmStarStarrpEventInfo @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmUpStrmStarStarRpExecute(pimsmCB_t * pimsmCb,
                            pimsmStarStarRpNode_t * pStarStarRpNode,
                            pimsmUpStrmStarStarRPEventInfo_t
                            * pUpStrmStarStarRpEventInfo)
{
  struct pimsmStarStarRpEntry_s  * pStarStarRpEntry;
  pimsmUpStrmStarStarRPStates_t upStrmStarStarRpState ;
  pimsmUpStrmStarStarRPEvents_t upStrmStarStarRpEvent ;
  L7_RC_t             rc = L7_SUCCESS;


  if(pStarStarRpNode == (pimsmStarStarRpNode_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "Invalid (*,*,RP) node ");
    return L7_FAILURE;
  }
  if(pUpStrmStarStarRpEventInfo == (pimsmUpStrmStarStarRPEventInfo_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "Invalid event info");
    return L7_FAILURE;
  }
  if((pStarStarRpNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,PIMSM_TRACE_ERROR,
        "Node is deleted");
    return L7_FAILURE;
  }
  pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
  upStrmStarStarRpState = pStarStarRpEntry->pimsmStarStarRpUpstreamJoinState;
  upStrmStarStarRpEvent = pUpStrmStarStarRpEventInfo->eventType;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmUpStrmStarStarRPStateName[upStrmStarStarRpState],
              pimsmUpStrmStarStarRPEventName[upStrmStarStarRpEvent]);

  if((pimsmUpStrmPerIntfStarStarRP[upStrmStarStarRpEvent][upStrmStarStarRpState]).pimsmUpStrmStarStarRPAction
     (pimsmCb, pStarStarRpNode, pUpStrmStarStarRpEventInfo) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NORMAL, "UpStrmStarStarRPAction Failed");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"UpStrmStarStarRPAction Failed. State = %s, \n Event = %s",
              pimsmUpStrmStarStarRPStateName[upStrmStarStarRpState],
              pimsmUpStrmStarStarRPEventName[upStrmStarStarRpEvent]);
    /*pimsmStarStarRpNodeTryRemove(pimsmCb, pStarStarRpNode);*/
    return L7_FAILURE;
  }

  pStarStarRpEntry->pimsmStarStarRpUpstreamJoinState =
  (pimsmUpStrmPerIntfStarStarRP[upStrmStarStarRpEvent][upStrmStarStarRpState]).nextState;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "Next State = %s \n ",
  pimsmUpStrmStarStarRPStateName[pStarStarRpEntry->pimsmStarStarRpUpstreamJoinState]);

  /*pimsmStarStarRpNodeTryRemove(pimsmCb, pStarStarRpNode);*/
  return rc;
}

/******************************************************************************
* @purpose   Send upstream (*,*,G) join
*
* @param    pimsmCb                    @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode            @b{(input)} (S,G) node
* @param    pUpStrmStarStarrpEventInfo @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmPerIntfStarStarRPJoinSend(pimsmCB_t * pimsmCb,
                                        pimsmStarStarRpNode_t
                                        * pStarStarRpNode,
                                        pimsmUpStrmStarStarRPEventInfo_t
                                        * pUpStrmStarStarRpEventInfo)
{
  struct pimsmStarStarRpEntry_s  * pStarStarRpEntry;
  pimsmSendJoinPruneData_t jpData;
  pimsmTimerData_t *pTimerData;
  L7_uchar8 maskLen;
  L7_uint32 jpInterval = 0 ;

  MCAST_UNUSED_PARAM(pUpStrmStarStarRpEventInfo);
  pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;

  /*Send Join (*,*,RP);*/
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum
  = pStarStarRpEntry->pimsmStarStarRpRPFIfIndex;
  /*TODo: Update intfList access for CB */
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                     &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.joinOrPruneFlag = L7_TRUE;
  jpData.pSrcAddr =
  &pStarStarRpEntry->pimsmStarStarRpRPAddress;
  jpData.pNbrAddr =
  &pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor;
  jpData.sendImmdFlag = pUpStrmStarStarRpEventInfo->sendImmdFlag;
  jpData.isStarStarRpFlag = L7_TRUE;
  if (pStarStarRpEntry->pimsmStarStarRpRPFIfIndex == 0)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,"Neighbor is local ");
   return L7_SUCCESS;
  }
  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
    return L7_FAILURE;
  }
  /*Set Join Timer to t_periodic;*/

  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb,
                                          pStarStarRpEntry->pimsmStarStarRpRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune interval for rtrIfNum %d",
                pStarStarRpEntry->pimsmStarStarRpRPFIfIndex);
    return L7_FAILURE;
  }
  pTimerData = &pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1,
    &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);

  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarStarRpUpstreamJoinTimerExpiresHandler,
                            (void*)pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerHandle,
                            jpInterval,
                            &(pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer),
                            "xR-JT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "Failed to add (*,*,Rp) upstrm join timer");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Send upstream (*,*,G) prune
*
* @param    pimsmCb                    @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode            @b{(input)} (S,G) node
* @param    pUpStrmStarStarrpEventInfo @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmPerIntfStarStarRPPruneSend(pimsmCB_t * pimsmCb,
                                        pimsmStarStarRpNode_t
                                        * pStarStarRpNode,
                                        pimsmUpStrmStarStarRPEventInfo_t
                                        * pUpStrmStarStarRpEventInfo)
{
  struct pimsmStarStarRpEntry_s  * pStarStarRpEntry;
  pimsmSendJoinPruneData_t jpData;
  L7_uchar8 maskLen;

  MCAST_UNUSED_PARAM(pUpStrmStarStarRpEventInfo);
  pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
  /*Send Prune (*,*,,RP);*/
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum =
  pStarStarRpEntry->pimsmStarStarRpRPFIfIndex;
  /*TODo: Update intfList access for CB */
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                     &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }
  jpData.pNbrAddr =
  &pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor;
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  jpData.pSrcAddr =
  &pStarStarRpEntry->pimsmStarStarRpRPAddress;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.joinOrPruneFlag = L7_FALSE;
  jpData.isStarStarRpFlag = L7_TRUE;
  jpData.sendImmdFlag = pUpStrmStarStarRpEventInfo->sendImmdFlag;
  if (pStarStarRpEntry->pimsmStarStarRpRPFIfIndex == 0)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,"Neighbor is local ");
   return L7_SUCCESS;
  }
  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
    return L7_FAILURE;
  }

  /*Cancel Join Timer;*/
  pimsmUtilAppTimerCancel (pimsmCb, &(pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer));
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Increase (*,*,G) Join Timer
*
* @param    pimsmCb                    @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode            @b{(input)} (S,G) node
* @param    pUpStrmStarStarrpEventInfo @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmUpStrmPerIntfStarStarRPJoinTimerIncrease(pimsmCB_t * pimsmCb,
                                              pimsmStarStarRpNode_t
                                              * pStarStarRpNode,
                                              pimsmUpStrmStarStarRPEventInfo_t
                                              * pUpStrmStarStarRpEventInfo)
{
  L7_uint32 suppress, joinSuppress, holdtime, timerVal;
  struct pimsmStarStarRpEntry_s  * pStarStarRpEntry;
  L7_uint32 low, high;
  pimsmTimerData_t *pTimerData;
  L7_uint32 jpInterval = 0;

  MCAST_UNUSED_PARAM(pUpStrmStarStarRpEventInfo);
  pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb,
                                          pStarStarRpEntry->pimsmStarStarRpRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune interval for rtrIfNum %d",
                pStarStarRpEntry->pimsmStarStarRpRPFIfIndex);
    return L7_FAILURE;
  }
  /* suppress  as per draft-11
  suppress  =   rand(1.1 *  t_periodic, 1.4 *  t_periodic) when
  Suppression_Enabled(I) is  true, 0 otherwise  */
  low =    (L7_uint32) ((11 *  jpInterval)/10);
  high =   (L7_uint32) ((14 *  jpInterval)/10);
  suppress  = (L7_uint32) (low + PIMSM_RANDOM() % ((high - low)));
  holdtime = pUpStrmStarStarRpEventInfo->holdtime;
  joinSuppress = min(holdtime, suppress);

  if(pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "(*,*,RP) upstrm join timer is not started");
    return L7_FAILURE;
  }
  appTimerTimeLeftGet(pimsmCb->timerCb,
                      pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer, &timerVal);
  if(timerVal < joinSuppress)
  {
    /*Increase Join Timer to joinSuppress;*/
    pTimerData = &pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    inetCopy(&pTimerData->addr1,
      &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);

    pTimerData->pimsmCb = pimsmCb;
    if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarStarRpUpstreamJoinTimerExpiresHandler,
                              (void*)pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerHandle,
                              joinSuppress,
                              &(pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer),
                              "xR-JT2")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "Failed to update (*,*,Rp) upstrm join timer");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Decrease (*,*,G) Join Timer
*
* @param    pimsmCb                    @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode            @b{(input)} (S,G) node
* @param    pUpStrmStarStarrpEventInfo @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmPerIntfStarStarRPJoinTimerDecrease(pimsmCB_t * pimsmCb,
                                               pimsmStarStarRpNode_t
                                               * pStarStarRpNode,
                                                pimsmUpStrmStarStarRPEventInfo_t
                                                * pUpStrmStarStarRpEventInfo)
{
  struct pimsmStarStarRpEntry_s  * pStarStarRpEntry;
  pimsmTimerData_t * pTimerData;
  L7_uint32  jpOverrideInterval;
  L7_uint32  timeLeft = 0;
  L7_RC_t rc;

  MCAST_UNUSED_PARAM(pUpStrmStarStarRpEventInfo);
  pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;

  if(pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "(*,*,RP) upstrm join timer is not started");
    return L7_FAILURE;
  }

  pimsmEffectiveOverrideInterval(pimsmCb,
                                 pStarStarRpEntry->pimsmStarStarRpRPFIfIndex,
                                 &jpOverrideInterval);
  rc = appTimerTimeLeftGet(pimsmCb->timerCb,
                           pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer,
                           &timeLeft);
  if((L7_SUCCESS == rc) &&
     (timeLeft > jpOverrideInterval))
  {
    /*Decrease Join Timer to t_override;*/
    pTimerData = &pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerParam;
    memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
    inetCopy(&pTimerData->addr1,
      &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);
    pTimerData->pimsmCb = pimsmCb;

    if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarStarRpUpstreamJoinTimerExpiresHandler,
                              (void*)pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerHandle,
                              jpOverrideInterval,
                              &(pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer),
                              "xR-JT3")
                           != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "Failed to update (*,*,Rp) upstrm join timer");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose Send upstream (*,*,G) jointo new Nexthop and prune to old new nexthop
*
* @param    pimsmCb                    @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode            @b{(input)} (S,G) node
* @param    pUpStrmStarStarrpEventInfo @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmPerIntfStarStarRPJoinPruneSend(pimsmCB_t * pimsmCb,
                                         pimsmStarStarRpNode_t
                                         * pStarStarRpNode,
                                         pimsmUpStrmStarStarRPEventInfo_t
                                         * pUpStrmStarStarRpEventInfo)
{
  struct pimsmStarStarRpEntry_s  * pStarStarRpEntry;
  pimsmSendJoinPruneData_t jpData;
  pimsmTimerData_t *pTimerData;
  L7_uint32 jpInterval = 0;
  L7_uchar8  maskLen;

  pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,"newRPFIfIndex =%d ",
                                pStarStarRpEntry->pimsmStarStarRpRPFIfIndex);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP,   PIMSM_TRACE_INFO,"newRPF NextHop = ",
                                      &pStarStarRpEntry->pimsmStarStarRpRPFNextHop);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,"newRPFRouteProtocol = %d",
                                pStarStarRpEntry->pimsmStarStarRpRPFRouteProtocol);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP,   PIMSM_TRACE_INFO,"newRPF RouteAddress :",
                                      &pStarStarRpEntry->pimsmStarStarRpRPFRouteAddress);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,"newRPFRouteMetricPref = %d",
                                pStarStarRpEntry->pimsmStarStarRpRPFRouteMetricPref);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,"newRPFRouteMetric =%d",
                                 pStarStarRpEntry->pimsmStarStarRpRPFRouteMetric);



  /*Send Prune (*,*,RP) to old next hop;*/
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum
  = pUpStrmStarStarRpEventInfo->rpfInfo.rpfIfIndex;
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                     &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }
  jpData.pNbrAddr =
  &pUpStrmStarStarRpEventInfo->upStrmNbrAddr;
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  jpData.pSrcAddr =
  &pStarStarRpEntry->pimsmStarStarRpRPAddress;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen= maskLen;
  jpData.joinOrPruneFlag = L7_FALSE;
  jpData.isStarStarRpFlag = L7_TRUE;
  jpData.sendImmdFlag = pUpStrmStarStarRpEventInfo->sendImmdFlag;
  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
  }

  if (pimsmStarStarRpJoinDesired(pimsmCb,pStarStarRpNode) == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,"StarStarRp Join Desired is false ");
   return L7_SUCCESS;
  }
  /*Send Join (*,*,RP) to new next hop;*/
  jpData.rtrIfNum = pStarStarRpEntry->pimsmStarStarRpRPFIfIndex;
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum,
                                   &jpData.holdtime)
                                     != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
    return L7_FAILURE;
  }
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  jpData.joinOrPruneFlag = L7_TRUE;
  jpData.isStarStarRpFlag = L7_TRUE;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen= maskLen;
  jpData.pSrcAddr =
  &pStarStarRpEntry->pimsmStarStarRpRPAddress;
  jpData.pNbrAddr =
  &pStarStarRpEntry->pimsmStarStarRpUpstreamNeighbor;
  jpData.sendImmdFlag = pUpStrmStarStarRpEventInfo->sendImmdFlag;
  if (pStarStarRpEntry->pimsmStarStarRpRPFIfIndex == 0)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,"Neighbor is local ");
   return L7_SUCCESS;
  }
  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
    return L7_FAILURE;
  }

  /*Set Join Timer to t_periodic;*/
  if (pimsmInterfaceJoinPruneIntervalGet(pimsmCb, pStarStarRpEntry->pimsmStarStarRpRPFIfIndex,
                                          &jpInterval)  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
               "Failed to get JoinPrune interval for rtrIfNum %d",
                pStarStarRpEntry->pimsmStarStarRpRPFIfIndex);
    return L7_FAILURE;
  }
  pTimerData = &pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1,
    &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);

  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarStarRpUpstreamJoinTimerExpiresHandler,
                            (void*)pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimerHandle,
                            jpInterval,
                            &(pStarStarRpEntry->pimsmStarStarRpUpstreamJoinTimer),
                            "xR-JT4")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "Failed to add (*,*,Rp) upstrm join timer");;
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose   Unexpected event, return Error
*
* @param    pimsmCb                    @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode            @b{(input)} (S,G) node
* @param    pUpStrmStarStarrpEventInfo @b{(input)} Upstream StarG FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmUpStrmPerIntfStarStarRPErrorReturn(pimsmCB_t * pimsmCb,
                                          pimsmStarStarRpNode_t
                                          * pStarStarRpNode,
                                          pimsmUpStrmStarStarRPEventInfo_t
                                          * pUpStrmStarStarRpEventInfo)
{
  MCAST_UNUSED_PARAM(pStarStarRpNode);
  MCAST_UNUSED_PARAM(pUpStrmStarStarRpEventInfo);
  return L7_FAILURE;
}

/******************************************************************************
* @purpose  Action Routine when (*,*,G) Upstream Join Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmStarStarRpUpstreamJoinTimerExpiresHandler(void *pParam)
{
  pimsmUpStrmStarStarRPEventInfo_t pUpStrmStarStarRpEventInfo;
  pimsmStarStarRpNode_t *pStarStarRpNode;
  pimsmTimerData_t *pTimerData;
  pimsmCB_t  * pimsmCb;
  L7_int32      handle = (L7_int32)pParam;
  L7_inet_addr_t *pRpAddr;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NORMAL,
        "pTimerData == L7_NULLPTR");
    return ;
  }
  pimsmCb = pTimerData->pimsmCb;
  pRpAddr = &pTimerData->addr1;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_STAR_RP, PIMSM_TRACE_INFO,
     "RP Address :", pRpAddr);
  if (pimsmStarStarRpFind(pimsmCb, pRpAddr, &pStarStarRpNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,PIMSM_TRACE_NORMAL,
        "(*,*,RP) Find failed");
    return;
  }

  if (pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpUpstreamJoinTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmStarStarRpUpstreamJoinTimer is NULL, But Still Expired");
    return;
  }
  pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpUpstreamJoinTimer = L7_NULLPTR;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NORMAL,
      "(*,*,Rp) Upstream JoinTimer Expired");
  pUpStrmStarStarRpEventInfo.eventType =
     PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_TIMER_EXPIRES;

  pimsmUpStrmStarStarRpExecute(pimsmCb, pStarStarRpNode, &pUpStrmStarStarRpEventInfo);
}

