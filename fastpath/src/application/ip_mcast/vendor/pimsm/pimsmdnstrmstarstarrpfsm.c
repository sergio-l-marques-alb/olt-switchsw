/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmDnStrmStarStarRpFSM.c
*
* @purpose Contains PIM-SM Down stream per interface State Machine
* implementation for (*,*,RP) entry.
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
#include "pimsmdnstrmstarstarrpfsm.h"
#include "pimsmtimer.h"
#include "pimsmwrap.h"

static L7_RC_t pimsmDnSmPerIntfStartStarRPExpireTimerStart
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s *pStarStarRpNode,
 pimsmDnStrmPerIntfStarStarRPEventInfo_t * pDnStrmPerIntfStarStarRpEventInfo);
static L7_RC_t pimsmDnSmPerIntfStartStarRPExpiryTimerRestart
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s *pStarStarRpNode,
 pimsmDnStrmPerIntfStarStarRPEventInfo_t * pDnStrmPerIntfStarStarRpEventInfo);
static L7_RC_t pimsmDnSmPerIntfStartStarRPPrunePendingTimerStart
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s *pStarStarRpNode,
 pimsmDnStrmPerIntfStarStarRPEventInfo_t  * pDnStrmPerIntfStarStarRpEventInfo);
static L7_RC_t pimsmDnSmPerIntfStartStarRPPruneEchoSend
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s *pStarStarRpNode,
 pimsmDnStrmPerIntfStarStarRPEventInfo_t  * pDnStrmPerIntfStarStarRpEventInfo);
static L7_RC_t pimsmDnSmPerIntfStartStarRPDoNothing
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s *pStarStarRpNode,
 pimsmDnStrmPerIntfStarStarRPEventInfo_t  * pDnStrmPerIntfStarStarRpEventInfo);
static L7_RC_t pimsmDnSmPerIntfStartStarRPErrorReturn
(pimsmCB_t * pimsmCb,
 struct pimsmStarStarRpNode_s *pStarStarRpNode,
 pimsmDnStrmPerIntfStarStarRPEventInfo_t  * pDnStrmPerIntfStarStarRpEventInfo);

typedef struct pimsmDnStrmPerIntfStarStarRPSM_s
{
  pimsmDnStrmPerIntfStarStarRPStates_t nextState;
  L7_RC_t (*pimsmDnStrmPerIntfStarStarRPAction)
  (pimsmCB_t * pimsmCb, struct pimsmStarStarRpNode_s * pStarStarRpNode,
   pimsmDnStrmPerIntfStarStarRPEventInfo_t * pDnStrmPerIntfStarStarRpEventInfo);
} pimsmDnStrmPerIntfStarStarRPSM_t;

static pimsmDnStrmPerIntfStarStarRPSM_t pimsmDnStrmPerIntfStarStarRP
[PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_MAX]
[PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_MAX] =
{
  /* Recv Join */
  {
    /* No Info */  {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_JOIN,
      pimsmDnSmPerIntfStartStarRPExpireTimerStart},
    /* join */     {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_JOIN,
      pimsmDnSmPerIntfStartStarRPExpiryTimerRestart},
    /*   PP */       {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_JOIN,
      pimsmDnSmPerIntfStartStarRPExpiryTimerRestart}
  },
  /* Recv Prune */
  {
    /* No Info */   {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO,
      pimsmDnSmPerIntfStartStarRPDoNothing},
    /* join */  {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_PRUNE_PENDING,
      pimsmDnSmPerIntfStartStarRPPrunePendingTimerStart},
    /*   PP */   {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_PRUNE_PENDING,
      pimsmDnSmPerIntfStartStarRPDoNothing}
  },
  /* PP timer expires */
  {
    /* No Info */   {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO,
      pimsmDnSmPerIntfStartStarRPErrorReturn},
    /* join */      {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_JOIN,
      pimsmDnSmPerIntfStartStarRPErrorReturn},
    /*   PP */       {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO,
      pimsmDnSmPerIntfStartStarRPPruneEchoSend}
  },
  /* Expiry timer expries */
  {
    /* No Info */   {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO,
      pimsmDnSmPerIntfStartStarRPErrorReturn},
    /* join */      {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO,
      pimsmDnSmPerIntfStartStarRPDoNothing},
    /*   PP */       {PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_NO_INFO,
      pimsmDnSmPerIntfStartStarRPDoNothing}
  }
};
static void pimsmStarStarRpIPrunePendingTimerExpiresHandler(void *pParam);
static void pimsmStarStarRpIJoinExpiryTimerExpiresHandler(void *pParam);
/******************************************************************************
*
* @purpose   execute the Down Stream per (*,*,RP) FSM and process the events
*
* @param    pimsmCb                           @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode                   @b{(input)} (*,*,RP)  node
* @param    pDnStrmPerIntfStarStarRpEventInfo @b{(input)} Downstream (*,G) FSM event
*                                                          related data
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*
******************************************************************************/
L7_RC_t pimsmDnStrmPerIntfStarStarRPExecute(
                                         pimsmCB_t * pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                pimsmDnStrmPerIntfStarStarRPEventInfo_t
                                * pDnStrmPerIntfStarStarRpEventInfo)
{
  pimsmDnStrmPerIntfStarStarRPStates_t dnStrmPerIntfStarStarRpState;
  pimsmDnStrmPerIntfStarStarRPEvents_t dnStrmPerIntfStarStarRpEvent;
  L7_uint32 rtrIfNum;
  pimsmStarStarRpIEntry_t *pStarStarRPIEntry;
  L7_RC_t rc = L7_SUCCESS;

  if((pStarStarRpNode == (pimsmStarStarRpNode_t *)L7_NULLPTR)||
     (pDnStrmPerIntfStarStarRpEventInfo ==
       (pimsmDnStrmPerIntfStarStarRPEventInfo_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "invalid input parameters");
    return L7_FAILURE;
  }
  if((pStarStarRpNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,PIMSM_TRACE_ERROR,
        "Node is deleted");
    return L7_FAILURE;
  }
  rtrIfNum = pDnStrmPerIntfStarStarRpEventInfo->rtrIfNum;
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "RtrIfNum = %d",rtrIfNum);
  pStarStarRPIEntry = pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
  if(pStarStarRPIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "(*,*,Rp,%d) entry is not present",
                                 rtrIfNum);
    return L7_FAILURE;
  }

  dnStrmPerIntfStarStarRpState = pStarStarRPIEntry->pimsmStarStarRpIJoinPruneState;
  dnStrmPerIntfStarStarRpEvent = pDnStrmPerIntfStarStarRpEventInfo->eventType;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmDnStrmPerIntfStarStarRPStateName[dnStrmPerIntfStarStarRpState],
              pimsmDnStrmPerIntfStarStarRPEventName[dnStrmPerIntfStarStarRpEvent]);

  if((pimsmDnStrmPerIntfStarStarRP[dnStrmPerIntfStarStarRpEvent][dnStrmPerIntfStarStarRpState]).
     pimsmDnStrmPerIntfStarStarRPAction
     (pimsmCb, pStarStarRpNode, pDnStrmPerIntfStarStarRpEventInfo) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NORMAL,
                "DownStrmPerIntfStarStarRPAction Failed");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"DownStrmPerIntfStarStarRPAction Failed. State = %s, \n Event = %s",
      pimsmDnStrmPerIntfStarStarRPStateName[dnStrmPerIntfStarStarRpState],
      pimsmDnStrmPerIntfStarStarRPEventName[dnStrmPerIntfStarStarRpEvent]);
    /*pimsmStarStarRpNodeTryRemove(pimsmCb, pStarStarRpNode);*/
    return L7_FAILURE;
  }

  pStarStarRPIEntry->pimsmStarStarRpIJoinPruneState =
  (pimsmDnStrmPerIntfStarStarRP[dnStrmPerIntfStarStarRpEvent][dnStrmPerIntfStarStarRpState]).nextState;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "Next State = %s \n ",
              pimsmDnStrmPerIntfStarStarRPStateName
              [pStarStarRPIEntry->pimsmStarStarRpIJoinPruneState]);

  /*pimsmStarStarRpNodeTryRemove(pimsmCb, pStarStarRpNode);*/
  return rc;
}

/******************************************************************************
* @purpose  Start (*,*,RP) entry Expiry timer
*
* @param    pimsmCb                           @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode                   @b{(input)} (*,*,RP)  node
* @param    pDnStrmPerIntfStarStarRpEventInfo @b{(input)} Downstream (*,G) FSM event
*                                                          related data
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnSmPerIntfStartStarRPExpireTimerStart(
                                         pimsmCB_t * pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                pimsmDnStrmPerIntfStarStarRPEventInfo_t
                                * pDnStrmPerIntfStarStarRpEventInfo)
{
  L7_uint32 holdtime;
  L7_uint32 rtrIfNum;
  pimsmStarStarRpIEntry_t *pStarStarRPIEntry;
  pimsmTimerData_t *pTimerData;

  rtrIfNum = pDnStrmPerIntfStarStarRpEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pStarStarRPIEntry = pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
  if(pStarStarRPIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "(*,*,Rp,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }

  /* Start Expiry Timer;
   * set to the HoldTime from the triggering  Join/Prune message
   */
  holdtime = pDnStrmPerIntfStarStarRpEventInfo->holdtime;
  pTimerData = &pStarStarRPIEntry->pimsmStarStarRpIJoinExpiryTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1,
      &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);
  pTimerData->rtrIfNum = rtrIfNum;
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarStarRpIJoinExpiryTimerExpiresHandler,
                            (void*)pStarStarRPIEntry->pimsmStarStarRpIJoinExpiryTimerHandle,
                            holdtime,
                            &(pStarStarRPIEntry->pimsmStarStarRpIJoinExpiryTimer),
                            "xRI-JT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
              "Failed to add (*,*,Rp,%d) join expire timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Restart (*,*,RP) entry Expiry timer
*
* @param    pimsmCb                           @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode                   @b{(input)} (*,*,RP)  node
* @param    pDnStrmPerIntfStarStarRpEventInfo @b{(input)} Downstream (*,G) FSM event
*                                                          related data
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnSmPerIntfStartStarRPExpiryTimerRestart(
                                         pimsmCB_t * pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                pimsmDnStrmPerIntfStarStarRPEventInfo_t
                                * pDnStrmPerIntfStarStarRpEventInfo)
{
  L7_uint32 holdtime;
  L7_uint32 timeLeft = 0;
  L7_uint32 rtrIfNum;
  pimsmStarStarRpIEntry_t *pStarStarRPIEntry;
  pimsmTimerData_t *pTimerData;

  rtrIfNum = pDnStrmPerIntfStarStarRpEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pStarStarRPIEntry = pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
  if(pStarStarRPIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "(*,*,Rp,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }
  pimsmUtilAppTimerCancel (pimsmCb, &(pStarStarRPIEntry->pimsmStarStarRpIPrunePendingTimer));

  /* Restart Expiry Timer;*/
  /* set to maximum of its current value and the HoldTime from the
        triggering Join/Prune message.*/
  holdtime = pDnStrmPerIntfStarStarRpEventInfo->holdtime;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "holdtime =%d", holdtime);
  /* TODO: update the comparision
  holdtime = max (pStarStarRPIEntry->pimsmStarStarRpIJoinExpiryTimer,
          holdtime);
  */
  pTimerData = &pStarStarRPIEntry->pimsmStarStarRpIJoinExpiryTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1,
      &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);
  pTimerData->rtrIfNum = rtrIfNum;
  pTimerData->pimsmCb = pimsmCb;

  if (appTimerTimeLeftGet (pimsmCb->timerCb, pStarStarRPIEntry->pimsmStarStarRpIJoinExpiryTimer,
                           &timeLeft) == L7_SUCCESS)
  {
    holdtime = max (timeLeft, holdtime);
  }

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarStarRpIJoinExpiryTimerExpiresHandler,
                            (void*)pStarStarRPIEntry->pimsmStarStarRpIJoinExpiryTimerHandle,
                            holdtime,
                            &(pStarStarRPIEntry->pimsmStarStarRpIJoinExpiryTimer),
                            "xRI-JT2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
                "Failed to add (*,*,Rp,%d) join expire timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Restart (*,*,RP) entry Expiry timer
*
* @param    pimsmCb                           @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode                   @b{(input)} (*,*,RP)  node
* @param    pDnStrmPerIntfStarStarRpEventInfo @b{(input)} Downstream (*,G) FSM event
*                                                          related data
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnSmPerIntfStartStarRPPrunePendingTimerStart(
                                         pimsmCB_t * pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                pimsmDnStrmPerIntfStarStarRPEventInfo_t
                                * pDnStrmPerIntfStarStarRpEventInfo)
{
  /* Start Prune Pending Timer;*/
  L7_uint32 rtrIfNum, timeOut = 0;
  pimsmStarStarRpIEntry_t *pStarStarRPIEntry;
  pimsmTimerData_t *pTimerData;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;

  rtrIfNum = pDnStrmPerIntfStarStarRpEventInfo->rtrIfNum;
  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &pIntfEntry )
     !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"Failed to get interface entry for %d",rtrIfNum);
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pStarStarRPIEntry = pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
  if(pStarStarRPIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "(*,*,Rp,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }
  /* The Prune-Pending Timer is started; it is set to the
   * J/P_Override_Interval(I) if the router has more than one
   * neighbor on that interface; otherwise it is set to zero.
   */
  if(pIntfEntry->pimsmNbrCount > 1)
  {
    timeOut = pimsmJoinPruneOverrideIntervalGet(pimsmCb,rtrIfNum);
  }
  pTimerData =
  &pStarStarRPIEntry->pimsmStarStarRpIPrunePendingTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1,
      &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);
  pTimerData->rtrIfNum = rtrIfNum;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO," J/P Override intvl = %d ",
        pimsmJoinPruneOverrideIntervalGet(pimsmCb,rtrIfNum));

  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarStarRpIPrunePendingTimerExpiresHandler,
                            (void*)pStarStarRPIEntry->pimsmStarStarRpIPrunePendingTimerHandle,
                            timeOut,
                            &(pStarStarRPIEntry->pimsmStarStarRpIPrunePendingTimer),
                            "xRI-PPT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
            "Failed to add (*,*,Rp,%d) prune pending timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Send (*,*,RP) Prune Echo
*
* @param    pimsmCb                           @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode                   @b{(input)} (*,*,RP)  node
* @param    pDnStrmPerIntfStarStarRpEventInfo @b{(input)} Downstream (*,G) FSM event
*                                                          related data
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnSmPerIntfStartStarRPPruneEchoSend(
                                         pimsmCB_t * pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                pimsmDnStrmPerIntfStarStarRPEventInfo_t
                                * pDnStrmPerIntfStarStarRpEventInfo)
{
  pimsmStarStarRpIEntry_t *pStarStarRPIEntry;
  pimsmStarStarRpEntry_t  *pStarStarRpEntry;
  pimsmSendJoinPruneData_t jpData;
  L7_uint32 rtrIfNum;
  L7_uchar8 maskLen;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NORMAL, "Enter");

  rtrIfNum = pDnStrmPerIntfStarStarRpEventInfo->rtrIfNum;
  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &pIntfEntry )
     !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"Failed to get interface entry for %d",rtrIfNum);
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pStarStarRPIEntry
  = pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
  if(pStarStarRPIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR, "(*,*,Rp,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }
  /* A PruneEcho(*,*,RP) need not be sent on an interface
        that contains only a single PIM neighbor */
  if(pIntfEntry->pimsmNbrCount <= 1)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO, "nbrCount = %d for rtrIfNum = %d",
                pIntfEntry->pimsmNbrCount, rtrIfNum);
    return(L7_SUCCESS);
  }

  /* Send Prune Echo(*,*,RP);*/
  pStarStarRpEntry = &pStarStarRpNode->pimsmStarStarRpEntry;
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum = rtrIfNum;
  jpData.holdtime = pIntfEntry->pimsmInterfaceJoinPruneHoldtime;
  /* put router's  own address in  the Upstream Neighbor Address field*/
  jpData.pNbrAddr = &pIntfEntry->pimsmInterfaceAddr;
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen= maskLen;
  jpData.joinOrPruneFlag = L7_FALSE;
  jpData.pSrcAddr =
  &pStarStarRpEntry->pimsmStarStarRpRPAddress;
  jpData.isStarStarRpFlag = L7_TRUE;
  jpData.pruneEchoFlag = L7_TRUE;

  /*TODO: check if rpAddr is in network order */

  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,
              "Failed to send (*,*,RP) Prune Echo Message on rtrIfNum =%d",
              rtrIfNum);
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Change the state and return
*
* @param    pimsmCb                           @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode                   @b{(input)} (*,*,RP)  node
* @param    pDnStrmPerIntfStarStarRpEventInfo @b{(input)} Downstream (*,G) FSM event
*                                                          related data
* @returns   L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnSmPerIntfStartStarRPDoNothing(
                                         pimsmCB_t * pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                pimsmDnStrmPerIntfStarStarRPEventInfo_t
                                * pDnStrmPerIntfStarStarRpEventInfo)
{
  MCAST_UNUSED_PARAM(pStarStarRpNode);
  MCAST_UNUSED_PARAM(pDnStrmPerIntfStarStarRpEventInfo);
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Unexpected event and return error
*
* @param    pimsmCb                           @b{(input)} PIMSM Control Block
* @param    pStarStarRpNode                   @b{(input)} (*,*,RP)  node
* @param    pDnStrmPerIntfStarStarRpEventInfo @b{(input)} Downstream (*,G) FSM event
*                                                          related data
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmDnSmPerIntfStartStarRPErrorReturn(
                                         pimsmCB_t * pimsmCb,
                                    pimsmStarStarRpNode_t *pStarStarRpNode,
                                pimsmDnStrmPerIntfStarStarRPEventInfo_t
                                * pDnStrmPerIntfStarStarRpEventInfo)
{
  MCAST_UNUSED_PARAM(pStarStarRpNode);
  MCAST_UNUSED_PARAM(pDnStrmPerIntfStarStarRpEventInfo);
  return(L7_FAILURE);
}

/******************************************************************************
* @purpose  Action Routine when (*,*,Rp) Prune Pending Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmStarStarRpIPrunePendingTimerExpiresHandler(void *pParam)
{
  pimsmDnStrmPerIntfStarStarRPEventInfo_t dnStrmPerIntfStarStarRpEventInfo;
  pimsmTimerData_t *pTimerData;
  pimsmStarStarRpNode_t * pStarStarRpNode;
  pimsmStarStarRpIEntry_t *pimsmStarStarRpIEntry = L7_NULLPTR;
  L7_uint32  rtrIfNum;
  pimsmCB_t *pimsmCb;
  L7_int32   handle = (L7_int32)pParam;
  pimsmUpStrmStarStarRPEventInfo_t upStrmStarStarRpEventInfo;
  L7_BOOL joinDesired = L7_FALSE;
  L7_inet_addr_t *pRpAddr;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  rtrIfNum = pTimerData->rtrIfNum;
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
  pimsmStarStarRpIEntry =
  pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
  if (pimsmStarStarRpIEntry->pimsmStarStarRpIPrunePendingTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM Downstream pimsmStarStarRpIPrunePendingTimer is NULL, But Still Expired");
    return;
  }
  pimsmStarStarRpIEntry->pimsmStarStarRpIPrunePendingTimer =L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NOTICE, "(*,*,Rp,I) PrunePending Timer Expired");
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_DEBUG, "RtrIfNum: %d",rtrIfNum);
  memset(&dnStrmPerIntfStarStarRpEventInfo, 0,
          sizeof(pimsmDnStrmPerIntfStarStarRPEventInfo_t));
  dnStrmPerIntfStarStarRpEventInfo.eventType =
  PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES;
  dnStrmPerIntfStarStarRpEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfStarStarRPExecute(pimsmCb, pStarStarRpNode,
                                        &dnStrmPerIntfStarStarRpEventInfo);
  memset(&upStrmStarStarRpEventInfo, 0 ,
             sizeof(pimsmUpStrmStarStarRPEventInfo_t));
  joinDesired = pimsmStarStarRpJoinDesired(pimsmCb,pStarStarRpNode);
  if(joinDesired == L7_TRUE)
  {
    upStrmStarStarRpEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_TRUE;

  }
  else
  {
    upStrmStarStarRpEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_FALSE;
  }

  pimsmUpStrmStarStarRpExecute(pimsmCb,pStarStarRpNode,
                               &upStrmStarStarRpEventInfo);
  if (pimsmStarStarRpMFCUpdate (pimsmCb, pStarStarRpNode, MFC_UPDATE_ENTRY, L7_FALSE)
                             != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_ERROR,"MFC Updation failed");
    return ;
  }
}
/******************************************************************************
* @purpose  Action Routine when (*,*,Rp) Join Expiry Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmStarStarRpIJoinExpiryTimerExpiresHandler(void *pParam)
{
  pimsmDnStrmPerIntfStarStarRPEventInfo_t dnStrmPerIntfStarStarRpEventInfo;
  pimsmTimerData_t *pTimerData;
  pimsmStarStarRpNode_t *pStarStarRpNode;
  pimsmStarStarRpIEntry_t *pimsmStarStarRpIEntry = L7_NULLPTR;
  L7_uint32 rtrIfNum;
  pimsmCB_t *pimsmCb;
  L7_int32   handle = (L7_int32)pParam;
  pimsmUpStrmStarStarRPEventInfo_t upStrmStarStarRpEventInfo;
  L7_BOOL joinDesired = L7_FALSE;
  L7_inet_addr_t *pRpAddr;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  rtrIfNum = pTimerData->rtrIfNum;
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
  pimsmStarStarRpIEntry =
  pStarStarRpNode->pimsmStarStarRpIEntry[rtrIfNum];
  if (pimsmStarStarRpIEntry->pimsmStarStarRpIJoinExpiryTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM Downstream pimsmStarStarRpIJoinExpiryTimer is NULL, But Still Expired");
    return;
  }
  pimsmStarStarRpIEntry->pimsmStarStarRpIJoinExpiryTimer =L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_NOTICE,
        "(*,*,Rp,I) JoinExpiry Timer Expired");
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_DEBUG,
      "RtrIfNum: %d",rtrIfNum);
  memset(&dnStrmPerIntfStarStarRpEventInfo, 0, sizeof(pimsmDnStrmPerIntfStarStarRPEventInfo_t));
  dnStrmPerIntfStarStarRpEventInfo.eventType =
  PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_EXPIRY_TIMER_EXPIRES;
  dnStrmPerIntfStarStarRpEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfStarStarRPExecute(pimsmCb, pStarStarRpNode,
                                           &dnStrmPerIntfStarStarRpEventInfo);

  memset(&upStrmStarStarRpEventInfo, 0 ,
             sizeof(pimsmUpStrmStarStarRPEventInfo_t));
  joinDesired = pimsmStarStarRpJoinDesired(pimsmCb,pStarStarRpNode);
  if(joinDesired == L7_TRUE)
  {
    upStrmStarStarRpEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_TRUE;

  }
  else
  {
    upStrmStarStarRpEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmStarStarRpExecute(pimsmCb,pStarStarRpNode,
                               &upStrmStarStarRpEventInfo);

  if (pimsmStarStarRpMFCUpdate(pimsmCb,pStarStarRpNode, MFC_UPDATE_ENTRY, L7_FALSE)
      != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_STAR_RP,  PIMSM_TRACE_INFO,
         "Failed to delete (*,*,RP) from MFC");
     return ;
  }
}


