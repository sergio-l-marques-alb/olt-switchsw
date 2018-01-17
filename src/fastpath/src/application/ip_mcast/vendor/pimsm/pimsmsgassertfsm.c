/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmSGAssertFSM.c
*
* @purpose Contains PIM-SM  per interface assert State Machine
* implementation for (S,G) entry
*
* @component
*
* @comments
*
* @create 08/18/2005
*
* @author Muralidhar Vakkalagadda  / Satya Dillikar
* @end
*
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmsgassertfsm.h"
#include "pimsmtimer.h"
#include "pimsmcontrol.h"
#include "pimsmwrap.h"
#include "mcast_wrap.h"
static L7_RC_t  pimsmPerIntfSGAssertActionA1
(pimsmCB_t * pimsmCb,
 struct pimsmSGNode_s * pSGNode,
 pimsmPerIntfSGAssertEventInfo_t * pPerIntfSGAssertEventInfo);
static L7_RC_t  pimsmPerIntfSGAssertActionA2
(pimsmCB_t * pimsmCb,
 struct pimsmSGNode_s * pSGNode,
 pimsmPerIntfSGAssertEventInfo_t * pPerIntfSGAssertEventInfo);
static L7_RC_t  pimsmPerIntfSGAssertActionA3
(pimsmCB_t * pimsmCb,
 struct pimsmSGNode_s * pSGNode,
 pimsmPerIntfSGAssertEventInfo_t * pPerIntfSGAssertEventInfo);
static L7_RC_t  pimsmPerIntfSGAssertActionA4
(pimsmCB_t * pimsmCb,
 struct pimsmSGNode_s * pSGNode,
 pimsmPerIntfSGAssertEventInfo_t * pPerIntfSGAssertEventInfo);
static L7_RC_t  pimsmPerIntfSGAssertActionA5
(pimsmCB_t * pimsmCb,
 struct pimsmSGNode_s * pSGNode,
 pimsmPerIntfSGAssertEventInfo_t * pPerIntfSGAssertEventInfo);
static L7_RC_t  pimsmPerIntfSGAssertActionA6
(pimsmCB_t * pimsmCb,
 struct pimsmSGNode_s * pSGNode,
 pimsmPerIntfSGAssertEventInfo_t * pPerIntfSGAssertEventInfo);
static L7_RC_t  pimsmPerIntfSGAssertErrorReturn
(pimsmCB_t * pimsmCb,
 struct pimsmSGNode_s * pSGNode,
 pimsmPerIntfSGAssertEventInfo_t * pPerIntfSGAssertEventInfo);


typedef struct pimsmPerIntfSGAssertSM_s
{
  pimsmPerIntfSGAssertStates_t nextState;
  L7_RC_t (*pimsmPerIntfSGAssertAction)
     (pimsmCB_t * pimsmCb,
     struct pimsmSGNode_s * pSGNode,
     pimsmPerIntfSGAssertEventInfo_t * pPerIntfSGAssertEventInfo);
} pimsmPerIntfSGAssertSM_t;

static pimsmPerIntfSGAssertSM_t pimsmPerIntfSGAssert
[PIMSM_ASSERT_S_G_SM_EVENT_MAX]
[PIMSM_ASSERT_S_G_SM_STATE_MAX] =
{
  /* PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT_COULD_ASSERT */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertActionA1},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertErrorReturn}
  },
  /*PIMSM_ASSERT_S_G_SM_EVENT_RECV_ASSERT_RPT  */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertActionA1},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertErrorReturn}
  },
  /*PIMSM_ASSERT_S_G_SM_EVENT_RECV_DATA_PKT   */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertActionA1},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn },
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertErrorReturn}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_RECV_ACCEPT_ASSERT */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertActionA6},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertErrorReturn}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TIMER_EXPIRES     */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertActionA3},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertActionA5}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertActionA3},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertErrorReturn}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_RECV_PREF_ASSERT   */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertActionA2},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertActionA2}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertActionA4},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertErrorReturn}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_RECV_ACCEPT_ASSERT_FROM_CURR_WINNER     */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
      pimsmPerIntfSGAssertActionA2}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT_FROM_CURR_WINNER */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertActionA5}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED     */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertActionA5}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TRACKING    */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertActionA5}
  },
  /* PIMSM_ASSERT_S_G_SM_EVENT_METRIC_BETTER_THAN_WINNER     */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertActionA5}
  },
  /*PIMSM_ASSERT_S_G_SM_EVENT_STOP_BEING_RPF_IFACE */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertActionA5}
  },
  /* PIMSM_ASSERT_S_G_RECV_JOIN_S_G  */
  {
    /* No Info */       {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Winner */    {PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
      pimsmPerIntfSGAssertErrorReturn},
    /* IAM Loser */     {PIMSM_ASSERT_S_G_SM_STATE_NO_INFO,
      pimsmPerIntfSGAssertActionA5}
  },
};

static void pimsmSGIAssertTimerExpiresHandler(void *pParam);
/******************************************************************************
* @purpose execute the per interface (S,G) Assert FSM and process the events
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmPerIntfSGAssertExecute (pimsmCB_t * pimsmCb,
                                     struct pimsmSGNode_s * pSGNode,
                                     pimsmPerIntfSGAssertEventInfo_t
                                     * pPerIntfSGAssertEventInfo)
{
  pimsmSGIEntry_t * pimsmSGIEntry;
  L7_uint32 rtrIfNum;
  pimsmPerIntfSGAssertStates_t perIntfSGAssertState ;
  pimsmPerIntfSGAssertEvents_t perIntfSGAssertEvent ;
  L7_RC_t     rc= L7_SUCCESS;

  if((pSGNode == (pimsmSGNode_t *)L7_NULLPTR) ||
     (pPerIntfSGAssertEventInfo == (pimsmPerIntfSGAssertEventInfo_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NORMAL, "invalid input parameters");
    return L7_FAILURE;
  }
  if((pSGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
        "Node is deleted");
    return L7_FAILURE;
  }
  perIntfSGAssertEvent = pPerIntfSGAssertEventInfo->eventType;
  if ( perIntfSGAssertEvent < 0 ||
      perIntfSGAssertEvent >= PIMSM_ASSERT_S_G_SM_EVENT_MAX)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "Wrong perIntfSGAssertEvent = %d",
              perIntfSGAssertEvent);
    return L7_FAILURE;
  }
  rtrIfNum = pPerIntfSGAssertEventInfo->rtrIfNum;
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pimsmSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pimsmSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }
  perIntfSGAssertState = pimsmSGIEntry->pimsmSGIAssertState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmPerIntfSGAssertStateName[perIntfSGAssertState],
              pimsmPerIntfSGAssertEventName[perIntfSGAssertEvent]);

  if((pimsmPerIntfSGAssert[perIntfSGAssertEvent][perIntfSGAssertState]).pimsmPerIntfSGAssertAction
     (pimsmCb, pSGNode, pPerIntfSGAssertEventInfo) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NORMAL, "PerIntfSGAssertAction Failed");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"PerIntfSGAssertAction Failed. State = %s, \n Event = %s",
      pimsmPerIntfSGAssertStateName[perIntfSGAssertState],
      pimsmPerIntfSGAssertEventName[perIntfSGAssertEvent]);
    pimsmSGNodeTryRemove(pimsmCb, pSGNode);
    return L7_FAILURE;
  }

  pimsmSGIEntry->pimsmSGIAssertState = (pimsmPerIntfSGAssert[perIntfSGAssertEvent][perIntfSGAssertState]).nextState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "Next State = %s \n ",
              pimsmPerIntfSGAssertStateName[pimsmSGIEntry->pimsmSGIAssertState]);
  pimsmSGNodeTryRemove(pimsmCb, pSGNode);
  return(rc);
}
/******************************************************************************
* @purpose   Assert FSM Action Routine 1    =>
*           Send Assert(S,G)
*           Set Assert Timer to (Assert_Time - Assert_Override_Interval)
*           Store self as AssertWinner(S,G,I)
*           Store spt_assert_metric(S,I) as AssertWinnerMetric(S,G,I)
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfSGAssertActionA1 (pimsmCB_t * pimsmCb,
                                              struct pimsmSGNode_s * pSGNode,
                                              pimsmPerIntfSGAssertEventInfo_t
                                              * pPerIntfSGAssertEventInfo)
{
  pimsmSGEntry_t *pSGEntry;
  pimsmSGIEntry_t *pSGIEntry;
  L7_inet_addr_t  *pGrpAddr;
  L7_inet_addr_t  *pSrcAddr;
  L7_uint32 rtrIfNum;
  L7_uint32 localPreference;
  L7_uint32 localMetric;
  pimsmTimerData_t *pTimerData;
  pimsmInterfaceEntry_t * pIntfEntry;
  /* Send Assert(S,G);*/

  pSGEntry = &pSGNode->pimsmSGEntry;
  pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
  pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
  rtrIfNum = pPerIntfSGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }

  if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_ERROR,
      "IntfEntry is not available for rtrIfNum(%d)", rtrIfNum);
    return L7_FAILURE;
  }
  localMetric = pSGEntry->pimsmSGRPFRouteMetric;
  localPreference =pSGEntry->pimsmSGRPFRouteMetricPref;

  if(pimsmAssertSend(pimsmCb, pSrcAddr, pGrpAddr, rtrIfNum, localPreference,
                     localMetric) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "Assert Send Failed on rtrIfNum %d",rtrIfNum);
    return L7_FAILURE;
  }

  /* Set Assert Timer to (Assert_Time - Assert_Override_Interval); */
  pTimerData = &pSGIEntry->pimsmSGIAssertTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->pimsmCb = pimsmCb;
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);

  pTimerData->rtrIfNum = rtrIfNum;

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGIAssertTimerExpiresHandler,
                            (void*)pSGIEntry->pimsmSGIAssertTimerHandle,
                            PIMSM_DEFAULT_ASSERT_TIME - PIMSM_DEFAULT_ASSERT_OVERRIDE_INTERVAL,
                            &(pSGIEntry->pimsmSGIAssertTimer),
                            "SGI-AT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
                "Failed to add (S,G,%d) Assert timer", rtrIfNum);
    return L7_FAILURE;
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_INFO, "Source Address :",
                   &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_INFO, "Group Address :",
                   &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);

  /* Store self as AssertWinner(S,G,I);*/
  inetCopy(&pSGIEntry->pimsmSGIAssertWinnerAddress, &pIntfEntry->pimsmInterfaceAddr);

  /* Store spt_assert_metric(S,I) as AssertWinnerMetric(S,G,I); */
  pSGIEntry->pimsmSGIAssertWinnerMetric = localMetric;
  pSGIEntry->pimsmSGIAssertWinnerMetricPref = localPreference;

  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose   Assert FSM Action Routine 2   =>
*           Store new assert winner as AssertWinner(S,G,I) and assert
*           winner metric as AssertWinnerMetric(S,G,I).
*           Set Assert Timer to Assert_Time
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfSGAssertActionA2Common (pimsmCB_t * pimsmCb,
                                              struct pimsmSGNode_s * pSGNode,
                                              pimsmPerIntfSGAssertEventInfo_t
                                              * pPerIntfSGAssertEventInfo)
{
  pimsmSGIEntry_t * pSGIEntry;
  L7_uint32 rtrIfNum;
  pimsmTimerData_t *pTimerData;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NORMAL, "Enter");
  rtrIfNum = pPerIntfSGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_INFO, "winnerAddress :",
                     &(pPerIntfSGAssertEventInfo->winnerAddress));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "winnerMetric =%d",
              pPerIntfSGAssertEventInfo->winnerMetric);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "winnerMetricPref =%d",
              pPerIntfSGAssertEventInfo->winnerMetricPref);

  /* Store new assert winner as AssertWinner(S,G,I) */
  inetCopy(&pSGIEntry->pimsmSGIAssertWinnerAddress,
           &pPerIntfSGAssertEventInfo->winnerAddress);
  /* Set assert winner metric as AssertWinnerMetric(S,G,I); */
  pSGIEntry->pimsmSGIAssertWinnerMetric
  = pPerIntfSGAssertEventInfo->winnerMetric;
  pSGIEntry->pimsmSGIAssertWinnerMetricPref
  = pPerIntfSGAssertEventInfo->winnerMetricPref;

  /* Set Assert Timer to Assert_Time; */
  pTimerData = &pSGIEntry->pimsmSGIAssertTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->pimsmCb = pimsmCb;
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  pTimerData->rtrIfNum = rtrIfNum;

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGIAssertTimerExpiresHandler,
                            (void*)pSGIEntry->pimsmSGIAssertTimerHandle,
                            PIMSM_DEFAULT_ASSERT_TIME,
                            &(pSGIEntry->pimsmSGIAssertTimer),
                            "SGI-AT2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
                "Failed to add (S,G,%d) Assert timer", rtrIfNum);
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "Added Assert timerHandle for (S,G) rtrIfNum =%d",
              rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_INFO, "Source Address :",
                   &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_INFO, "Group Address :",
                   &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);

  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose   Assert FSM Action Routine 2
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfSGAssertActionA2 (pimsmCB_t * pimsmCb,
                                              struct pimsmSGNode_s * pSGNode,
                                              pimsmPerIntfSGAssertEventInfo_t
                                              * pPerIntfSGAssertEventInfo)
{

  return  pimsmPerIntfSGAssertActionA2Common(pimsmCb, pSGNode, pPerIntfSGAssertEventInfo);
}
/******************************************************************************
* @purpose   Assert FSM Action Routine 3   =>
*           Send Assert(S,G)
*           Set Assert Timer to (Assert_Time - Assert_Override_Interval)
*
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfSGAssertActionA3 (pimsmCB_t * pimsmCb,
                                              struct pimsmSGNode_s * pSGNode,
                                              pimsmPerIntfSGAssertEventInfo_t
                                              * pPerIntfSGAssertEventInfo)
{

  pimsmSGEntry_t *pSGEntry;
  L7_inet_addr_t *pGrpAddr;
  L7_inet_addr_t *pSrcAddr;
  pimsmSGIEntry_t * pSGIEntry;
  L7_uint32 rtrIfNum;
  L7_uint32 localPreference;
  L7_uint32 localMetric;
  pimsmTimerData_t *pTimerData;

  /* Send Assert(S,G);*/
  pSGEntry = &pSGNode->pimsmSGEntry;
  pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
  pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
  rtrIfNum = pPerIntfSGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }
  localMetric = pSGEntry->pimsmSGRPFRouteMetric;
  localPreference = pSGEntry->pimsmSGRPFRouteMetricPref;

  if(pimsmAssertSend(pimsmCb, pSrcAddr, pGrpAddr, rtrIfNum, localPreference,
                     localMetric) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "Assert Send Failed on rtrIfNum %d",rtrIfNum);
    return L7_FAILURE;
  }

  /* Set Assert Timer to (Assert_Time - Assert_Override_Interval); */
  pTimerData = &pSGIEntry->pimsmSGIAssertTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->pimsmCb = pimsmCb;
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  pTimerData->rtrIfNum = rtrIfNum;

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGIAssertTimerExpiresHandler,
                            (void*)pSGIEntry->pimsmSGIAssertTimerHandle,
                            PIMSM_DEFAULT_ASSERT_TIME - PIMSM_DEFAULT_ASSERT_OVERRIDE_INTERVAL,
                            &(pSGIEntry->pimsmSGIAssertTimer),
                            "SGI-AT3")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
                "Failed to add (S,G,%d) Assert timer", rtrIfNum);
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "Added Assert timerHandle for (S,G) rtrIfNum =%d",
              rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_INFO, "Source Address :",
                   &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_INFO, "Group Address :",
                   &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Assert FSM Action Routine 4   =>
*           Send AssertCancel(S,G)
*           Delete assert info (AssertWinner(S,G,I) and
*           AssertWinnerMetric(S,G,I) will then return their default
*           values).
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfSGAssertActionA4 (pimsmCB_t * pimsmCb,
                                              struct pimsmSGNode_s * pSGNode,
                                              pimsmPerIntfSGAssertEventInfo_t
                                              * pPerIntfSGAssertEventInfo)
{
  pimsmSGEntry_t *pSGEntry;
  L7_inet_addr_t *pGrpAddr;
  L7_inet_addr_t *pSrcAddr;

  pimsmSGIEntry_t *pSGIEntry;
  L7_uint32 rtrIfNum;

  pSGEntry = &pSGNode->pimsmSGEntry;
  pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
  pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
  rtrIfNum = pPerIntfSGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  /* Send AssertCancel(S,G) */
  pimsmAssertCancelSend(pimsmCb, pSrcAddr, pGrpAddr,  rtrIfNum);

  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }

  /* delete assert info (AssertWinner(S,G,I) */
  inetAddressZeroSet(pimsmCb ->family, &pSGIEntry->pimsmSGIAssertWinnerAddress);
  /* Set AssertWinnerMetric(S,G,I) to default values; */
  pSGIEntry->pimsmSGIAssertWinnerMetric = PIMSM_DEFAULT_ASSERT_METRIC;
  pSGIEntry->pimsmSGIAssertWinnerMetricPref = PIMSM_DEFAULT_ASSERT_PREFERENCE;
  return(L7_SUCCESS);

}
/******************************************************************************
* @purpose   Assert FSM Action Routine 5   =>
*           Delete assert info (AssertWinner(S,G,I) and
*           AssertWinnerMetric(S,G,I) will then return their default
*           values).
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfSGAssertActionA5 (pimsmCB_t * pimsmCb,
                                              struct pimsmSGNode_s * pSGNode,
                                              pimsmPerIntfSGAssertEventInfo_t
                                              * pPerIntfSGAssertEventInfo)
{
  pimsmSGIEntry_t *pSGIEntry;
  L7_uint32 rtrIfNum;

  rtrIfNum = pPerIntfSGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];

  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }

  /* Delete assert info (AssertWinner(S,G,I) */
  inetAddressZeroSet(pimsmCb ->family, &pSGIEntry->pimsmSGIAssertWinnerAddress);

  /* AssertWinnerMetric(S,G,I) will then return their default values */
  pSGIEntry->pimsmSGIAssertWinnerMetric = PIMSM_DEFAULT_ASSERT_METRIC;
  pSGIEntry->pimsmSGIAssertWinnerMetricPref = PIMSM_DEFAULT_ASSERT_PREFERENCE;
  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose   Assert FSM Action Routine 6 =>
*           Store new assert winner as AssertWinner(S,G,I) and assert
*           winner metric as AssertWinnerMetric(S,G,I).
*           Set Assert Timer to Assert_Time
*           If (I is RPF_interface(S)) AND (UpstreamJPState(S,G) == true)
*           set SPTbit(S,G) to TRUE.
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfSGAssertActionA6 (pimsmCB_t * pimsmCb,
                                              struct pimsmSGNode_s * pSGNode,
                                              pimsmPerIntfSGAssertEventInfo_t
                                              * pPerIntfSGAssertEventInfo)
{
  pimsmSGEntry_t *pSGEntry;
  L7_uint32 rtrIfNum;

  rtrIfNum = pPerIntfSGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pimsmPerIntfSGAssertActionA2Common(pimsmCb, pSGNode, pPerIntfSGAssertEventInfo);

  /* if (I is RPF_interface(S)) AND (UpstreamJPState(S,G) == true)
          set SPTbit(S,G) to TRUE */
  pSGEntry = &pSGNode->pimsmSGEntry;

  if((rtrIfNum == pSGEntry->pimsmSGRPFIfIndex) &&
     (pSGEntry->pimsmSGUpstreamJoinState == PIMSM_UPSTRM_S_G_SM_STATE_JOINED))
  {
    pSGEntry->pimsmSGSPTBit = L7_TRUE;
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Return Error (Unexpected event received)
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pPerIntfSGAssertEventInfo   @b{(input)} SG Assert FSM event related data
*
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfSGAssertErrorReturn (pimsmCB_t * pimsmCb,
                                                 struct pimsmSGNode_s * pSGNode,
                                                 pimsmPerIntfSGAssertEventInfo_t
                                                 * pPerIntfSGAssertEventInfo)
{
  MCAST_UNUSED_PARAM(pSGNode);
  MCAST_UNUSED_PARAM(pPerIntfSGAssertEventInfo);
  return(L7_FAILURE);
}


/******************************************************************************
* @purpose  Action Routine when (S,G,I) Assert Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmSGIAssertTimerExpiresHandler(void *pParam)
{
  pimsmPerIntfSGAssertEventInfo_t perIntfSGAssertEventInfo;
  pimsmSGNode_t *pSGNode;
  pimsmSGIEntry_t *pSGIEntry= L7_NULLPTR;
  L7_uint32 rtrIfNum;
  pimsmCB_t * pimsmCb;
  pimsmTimerData_t *pTimerData;
  L7_int32      handle = (L7_int32)pParam;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  rtrIfNum = pTimerData->rtrIfNum;
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
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "RtrIfNum: %d",rtrIfNum);

  pSGIEntry= pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);
    return;
  }
  if (pSGIEntry->pimsmSGIAssertTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmSGIAssertTimer is NULL, But Still Expired");
    return;
  }
  pSGIEntry->pimsmSGIAssertTimer = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NOTICE, "(S,G,%d) Assert Timer Expired",rtrIfNum);

  memset(&perIntfSGAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));

  perIntfSGAssertEventInfo.eventType = PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TIMER_EXPIRES;
  perIntfSGAssertEventInfo.rtrIfNum = rtrIfNum;

  pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode, &perIntfSGAssertEventInfo);
  if (pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
      "Failed to update (S,G) exact");
    return;
  }
}

