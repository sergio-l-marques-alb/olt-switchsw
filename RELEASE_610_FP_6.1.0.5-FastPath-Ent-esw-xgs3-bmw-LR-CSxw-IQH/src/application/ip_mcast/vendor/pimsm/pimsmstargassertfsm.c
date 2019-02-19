/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmStarGAssertFSM.c
*
* @purpose Contains PIM-SM per (*,G) assert State Machine 
* implementation
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
#include "pimsmstargassertfsm.h"
#include "pimsmtimer.h"
#include "pimsmcontrol.h"
#include "pimsmwrap.h"
#include "mcast_wrap.h"
static L7_RC_t  pimsmPerIntfStarGAssertActionA1 
(pimsmCB_t * pimsmCb,
 struct pimsmStarGNode_s * pStarGNode, 
 pimsmPerIntfStarGAssertEventInfo_t * perIntfStarGAssertEventInfo);
static L7_RC_t  pimsmPerIntfStarGAssertActionA2 
(pimsmCB_t * pimsmCb,
 struct pimsmStarGNode_s * pStarGNode,
 pimsmPerIntfStarGAssertEventInfo_t * perIntfStarGAssertEventInfo);
static L7_RC_t  pimsmPerIntfStarGAssertActionA3 
(pimsmCB_t * pimsmCb,
 struct pimsmStarGNode_s * pStarGNode,
 pimsmPerIntfStarGAssertEventInfo_t * perIntfStarGAssertEventInfo);
static L7_RC_t  pimsmPerIntfStarGAssertActionA4 
(pimsmCB_t * pimsmCb,
 struct pimsmStarGNode_s * pStarGNode,
 pimsmPerIntfStarGAssertEventInfo_t * perIntfStarGAssertEventInfo);
static L7_RC_t  pimsmPerIntfStarGAssertActionA5 
(pimsmCB_t * pimsmCb,
 struct pimsmStarGNode_s * pStarGNode,
 pimsmPerIntfStarGAssertEventInfo_t * perIntfStarGAssertEventInfo);
static L7_RC_t pimsmPerIntfStarGAssertReturnError
(pimsmCB_t * pimsmCb,
 struct pimsmStarGNode_s * pStarGNode,
 pimsmPerIntfStarGAssertEventInfo_t * perIntfStarGAssertEventInfo);

typedef struct pimsmPerIntfStarGAssertSM_s
{
  pimsmPerIntfStarGAssertStates_t nextState;
  L7_RC_t (*pimsmPerIntfStarGAssertAction) 
  (pimsmCB_t * pimsmCb,
   struct pimsmStarGNode_s * pStarGNode,
   pimsmPerIntfStarGAssertEventInfo_t * perIntfStarGAssertEventInfo); 
} pimsmPerIntfStarGAssertSM_t;



static pimsmPerIntfStarGAssertSM_t pimsmPerIntfStarGAssert 
[PIMSM_ASSERT_STAR_G_SM_EVENT_MAX]
[PIMSM_ASSERT_STAR_G_SM_STATE_MAX] = 
{
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT_COULD_ASSERT */
  {
    /* No Info */  {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertActionA1},
    /* Winner */    {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertReturnError} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_DATA_PKT*/
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertActionA1},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
       pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertReturnError} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_ACCEPT_ASSERT */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertActionA2},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertReturnError} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_ASSERT_TIMER_EXPIRES*/
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertActionA3},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertActionA5} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT*/
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertActionA3},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertReturnError} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_PREF_ASSERT */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertActionA2},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertActionA2} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertActionA4},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertReturnError} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_ACCEPT_ASSERT_FROM_CURR_WINNER */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER, 
      pimsmPerIntfStarGAssertActionA2} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT_FROM_CURR_WINNER */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertActionA5} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertActionA5} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_ASSERT_TRACKING */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertActionA5} 
  },
  /*PIMSM_ASSERT_STAR_G_SM_EVENT_METRIC_BETTER_THAN_WINNER */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertActionA5} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_STOP_BEING_RPF_IFACE*/
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertActionA5} 
  },
  /* PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_JOIN */
  {
    /* No Info */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertReturnError},
    /* Winner */ {PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER, 
      pimsmPerIntfStarGAssertReturnError},
    /* Loser */ {PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO, 
      pimsmPerIntfStarGAssertActionA5} 
  }
};

static void  pimsmStarGIAssertTimerExpiresHandler(void *pParam);
/******************************************************************************
* @purpose execute the per interface (*,G) Assert FSM and process the events   
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGNode                      @b{(input)} (*,G) node
* @param    pPerIntfStarGAssertEventInfo @b{(input)} StarG Assert FSM event related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
L7_RC_t pimsmPerIntfStarGAssertExecute(pimsmCB_t * pimsmCb,
                              struct pimsmStarGNode_s * pStarGNode,
                              pimsmPerIntfStarGAssertEventInfo_t
                              *perIntfStarGAssertEventInfo)
{
  pimsmStarGIEntry_t *pStarGIEntry;
  L7_uint32 rtrIfNum;
  pimsmPerIntfStarGAssertStates_t perIntfStarGAssertState ;
  pimsmPerIntfStarGAssertEvents_t perIntfStarGAssertEvent ;
  L7_RC_t   rc = L7_SUCCESS;

  if((pStarGNode == (pimsmStarGNode_t *)L7_NULLPTR) ||
     (perIntfStarGAssertEventInfo ==(pimsmPerIntfStarGAssertEventInfo_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NORMAL, "Invalid input parameters");     
    return L7_SUCCESS;
  }
  if((pStarGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_ERROR, 
        "Node is deleted");            
    return L7_FAILURE;
  }  
  perIntfStarGAssertEvent = perIntfStarGAssertEventInfo->eventType;
  if ( perIntfStarGAssertEvent < 0 ||
      perIntfStarGAssertEvent >= PIMSM_ASSERT_STAR_G_SM_EVENT_MAX)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "Wrong perIntfStarGAssertEvent = %d",
                                 perIntfStarGAssertEvent);
    return L7_FAILURE;
  }
  
  rtrIfNum = perIntfStarGAssertEventInfo->rtrIfNum;
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);      
    return L7_FAILURE;
  }

  perIntfStarGAssertState = pStarGIEntry->pimsmStarGIAssertState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmPerIntfStarGAssertStateName[perIntfStarGAssertState],
              pimsmPerIntfStarGAssertEventName[perIntfStarGAssertEvent]);

  if((pimsmPerIntfStarGAssert[perIntfStarGAssertEvent][perIntfStarGAssertState]).pimsmPerIntfStarGAssertAction
     (pimsmCb, pStarGNode, perIntfStarGAssertEventInfo) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NORMAL, "PerIntfStarGAssertAction Failed");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"PerIntfStarGAssertAction Failed. State = %s, \n Event = %s",
      pimsmPerIntfStarGAssertStateName[perIntfStarGAssertState],
      pimsmPerIntfStarGAssertEventName[perIntfStarGAssertEvent]);
    pimsmStarGNodeTryRemove(pimsmCb, pStarGNode); 
    return L7_FAILURE;
  }
  pStarGIEntry->pimsmStarGIAssertState = 
  (pimsmPerIntfStarGAssert[perIntfStarGAssertEvent][perIntfStarGAssertState]).nextState;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "Next State = %s \n ", 
              pimsmPerIntfStarGAssertStateName[pStarGIEntry->pimsmStarGIAssertState]);
  pimsmStarGNodeTryRemove(pimsmCb, pStarGNode); 
  return(rc);
}
/******************************************************************************
* @purpose   Assert FSM Action Routine 1    =>
*           Send Assert(*,G)
*           Set Assert Timer to (Assert_Time - Assert_Override_Interval)
*           Store self as AssertWinner(*,G,I).
*           Store rpt_assert_metric(G,I) as AssertWinnerMetric(*,G,I).
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGNode                      @b{(input)} (*,G) node
* @param    pPerIntfStarGAssertEventInfo @b{(input)} StarG Assert FSM event related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfStarGAssertActionA1(pimsmCB_t * pimsmCb,
                                    struct pimsmStarGNode_s * pStarGNode,
                                    pimsmPerIntfStarGAssertEventInfo_t 
                                    * perIntfStarGAssertEventInfo)
{
  pimsmStarGEntry_t *pStarGEntry;
  pimsmStarGIEntry_t *pStarGIEntry;
  L7_inet_addr_t   *pGrpAddr;
  L7_inet_addr_t   srcAddr;
  L7_uint32 rtrIfNum;
  L7_uint32 localPreference;
  L7_uint32 localMetric;
  pimsmTimerData_t *pTimerData;
  pimsmInterfaceEntry_t * pIntfEntry; 
  /* Send Assert(*,G) */
  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  pGrpAddr = &pStarGEntry->pimsmStarGGrpAddress;
  /* NOTE : draft-11 sec 4.9.6.  
  For data  triggered Asserts the Source-Address field MAY be set to the IP
   source address of the data packet that triggered the Assert and is
   set to zero otherwise.
  */
  inetCopy(&srcAddr,&perIntfStarGAssertEventInfo->srcAddr);  
  rtrIfNum = perIntfStarGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);    
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);      
    return L7_FAILURE;
  }
  if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_ERROR,
      "IntfEntry is not available for rtrIfNum(%d)", rtrIfNum);
    return L7_FAILURE;
  } 
  localMetric = pStarGEntry->pimsmStarGRPFRouteMetric;
  localPreference = pStarGEntry->pimsmStarGRPFRouteMetricPref | PIMSM_ASSERT_RPT_BIT;
  if(pimsmAssertSend(pimsmCb, &srcAddr, pGrpAddr, rtrIfNum, 
                   localPreference, localMetric) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "Assert Send Failed on rtrIfNum %d",rtrIfNum); 
    return L7_FAILURE;
  }

  /* Set Assert Timer to (Assert_Time - Assert_Override_Interval); */
  pTimerData = &pStarGIEntry->pimsmStarGIAssertTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pTimerData->rtrIfNum = rtrIfNum;
  pTimerData->pimsmCb = pimsmCb; 
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGIAssertTimerExpiresHandler,
                            (void*)pStarGIEntry->pimsmStarGIAssertTimerHandle,
                            PIMSM_DEFAULT_ASSERT_TIME - PIMSM_DEFAULT_ASSERT_OVERRIDE_INTERVAL,
                            &(pStarGIEntry->pimsmStarGIAssertTimer),
                            "xGI-AT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, 
                "Failed to add (*,G,%d) assert timer",rtrIfNum);
    return L7_FAILURE;
  }

  /* Store self as AssertWinner(*,G,I). */
  inetCopy(&pStarGIEntry->pimsmStarGIAssertWinnerAddress, &pIntfEntry->pimsmInterfaceAddr);

  /*Store rpt_assert_metric(G,I) as AssertWinnerMetric(*,G,I).*/
  pStarGIEntry->pimsmStarGIAssertWinnerMetric 
  = localMetric;
  pStarGIEntry->pimsmStarGIAssertWinnerMetricPref 
  = localPreference;
  
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Assert FSM Action Routine 2    =>
*           Store new assert winner as AssertWinner(*,G,I) and assert
*           winner metric as AssertWinnerMetric(*,G,I).
*           Set Assert Timer to Assert_Time
*          
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGNode                      @b{(input)} (*,G) node
* @param    pPerIntfStarGAssertEventInfo @b{(input)} StarG Assert FSM event related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfStarGAssertActionA2(pimsmCB_t * pimsmCb,
                                    struct pimsmStarGNode_s *pStarGNode,
                                     pimsmPerIntfStarGAssertEventInfo_t 
                                     * perIntfStarGAssertEventInfo)
{
  pimsmStarGIEntry_t * pStarGIEntry;
  L7_uint32 rtrIfNum;
  pimsmTimerData_t *pTimerData;

  rtrIfNum = perIntfStarGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);    
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);     
    return L7_FAILURE;
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_INFO, "winnerAddress :", 
      &(perIntfStarGAssertEventInfo->winnerAddress));     
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "winnerMetric =%d", 
              perIntfStarGAssertEventInfo->winnerMetric);     
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "winnerMetricPref =%d", 
              perIntfStarGAssertEventInfo->winnerMetricPref);     
  /* Store new assert winner as AssertWinner(*,G,I)  */
  inetCopy(&pStarGIEntry->pimsmStarGIAssertWinnerAddress,
  &(perIntfStarGAssertEventInfo->winnerAddress));

  /*  assert winner metric as AssertWinnerMetric(*,G,I). */
  pStarGIEntry->pimsmStarGIAssertWinnerMetric = 
  perIntfStarGAssertEventInfo->winnerMetric;
  pStarGIEntry->pimsmStarGIAssertWinnerMetricPref =
  perIntfStarGAssertEventInfo->winnerMetricPref;

  /* Set Assert Timer to Assert_Time */
  pTimerData = &pStarGIEntry->pimsmStarGIAssertTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pTimerData->rtrIfNum = rtrIfNum;
  pTimerData->pimsmCb = pimsmCb;    
  
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGIAssertTimerExpiresHandler,
                            (void*)pStarGIEntry->pimsmStarGIAssertTimerHandle,
                            PIMSM_DEFAULT_ASSERT_TIME,
                            &(pStarGIEntry->pimsmStarGIAssertTimer),
                            "xGI-AT2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, 
                "Failed to add (*,G,%d) assert timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Assert FSM Action Routine 3    =>
*           Send Assert(*,G)
*           Set Assert Timer to (Assert_Time - Assert_Override_Interval)
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGNode                      @b{(input)} (*,G) node
* @param    pPerIntfStarGAssertEventInfo @b{(input)} StarG Assert FSM event related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfStarGAssertActionA3(pimsmCB_t * pimsmCb,
                                    struct pimsmStarGNode_s * pStarGNode,
                                    pimsmPerIntfStarGAssertEventInfo_t 
                                    * perIntfStarGAssertEventInfo)
{
  pimsmStarGEntry_t  *pStarGEntry;
  pimsmStarGIEntry_t *pStarGIEntry;
  L7_inet_addr_t   *pGrpAddr;
  L7_inet_addr_t   srcAddr;
  L7_uint32 rtrIfNum;
  L7_uint32 localPreference;
  L7_uint32 localMetric;
  pimsmTimerData_t *pTimerData;
  
  /* Send Assert(*,G) */
  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  pGrpAddr = &pStarGEntry->pimsmStarGGrpAddress;
  /* NOTE : draft-11 sec 4.9.6.  
  For data  triggered Asserts the Source-Address field MAY be set to the IP
   source address of the data packet that triggered the Assert and is
   set to zero otherwise.
  */
  inetCopy(&srcAddr,&perIntfStarGAssertEventInfo->srcAddr);
  rtrIfNum = perIntfStarGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);    
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);      
    return L7_FAILURE;
  }

  localMetric = pStarGEntry->pimsmStarGRPFRouteMetric;
  localPreference = pStarGEntry->pimsmStarGRPFRouteMetricPref | PIMSM_ASSERT_RPT_BIT;
  if(pimsmAssertSend(pimsmCb, &srcAddr, pGrpAddr, rtrIfNum,
                   localPreference, localMetric) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "Assert Send Failed on rtrIfNum %d",rtrIfNum); 
    return L7_FAILURE;
  }

  /* Set Assert Timer to (Assert_Time - Assert_Override_Interval) */
  pTimerData = &pStarGIEntry->pimsmStarGIAssertTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pTimerData->rtrIfNum = rtrIfNum;
  pTimerData->pimsmCb = pimsmCb;    

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGIAssertTimerExpiresHandler,
                            (void*)pStarGIEntry->pimsmStarGIAssertTimerHandle,
                            (PIMSM_DEFAULT_ASSERT_TIME - PIMSM_DEFAULT_ASSERT_OVERRIDE_INTERVAL),
                            &(pStarGIEntry->pimsmStarGIAssertTimer),
                            "xGI-AT3")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, 
                "Failed to add (*,G,%d) assert timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Assert FSM Action Routine 4    =>
*           Send AssertCancel(*,G)
*           Delete assert info (AssertWinner(*,G,I) and
*           AssertWinnerMetric(*,G,I) will then return their default
*           values).
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGNode                      @b{(input)} (*,G) node
* @param    pPerIntfStarGAssertEventInfo @b{(input)} StarG Assert FSM event related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfStarGAssertActionA4(pimsmCB_t * pimsmCb,
                                    struct pimsmStarGNode_s * pStarGNode,
                                    pimsmPerIntfStarGAssertEventInfo_t
                                    * perIntfStarGAssertEventInfo)
{
  pimsmStarGEntry_t *pStarGEntry;
  pimsmStarGIEntry_t *pStarGIEntry;
  L7_inet_addr_t   *pGrpAddr;
  L7_inet_addr_t   srcAddr;
  L7_uint32 rtrIfNum;

  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  pGrpAddr = &pStarGEntry->pimsmStarGGrpAddress;
  /* NOTE : draft-11 sec 4.9.6.  
    For data  triggered Asserts the Source-Address field MAY be set to the IP
   source address of the data packet that triggered the Assert and is
   set to zero otherwise.
  */
  inetAddressZeroSet(pimsmCb->family, &srcAddr);
  rtrIfNum = perIntfStarGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);    
  /* Send AssertCancel(*,G) */
  pimsmAssertCancelSend(pimsmCb, &srcAddr, pGrpAddr,  rtrIfNum);

  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);     
    return L7_FAILURE;
  }

  /* delete assert info (AssertWinner(*,G,I) */
  inetAddressZeroSet(pimsmCb ->family, &pStarGIEntry->pimsmStarGIAssertWinnerAddress);

  /* AssertWinnerMetric(*,G,I) will then return their default  values). */
  pStarGIEntry->pimsmStarGIAssertWinnerMetric = PIMSM_DEFAULT_ASSERT_METRIC;
  pStarGIEntry->pimsmStarGIAssertWinnerMetricPref = PIMSM_DEFAULT_ASSERT_PREFERENCE;
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Assert FSM Action Routine 5    =>
*           Delete assert info (AssertWinner(*,G,I) and
*           AssertWinnerMetric(*,G,I) will then return their default
*           values).
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGNode                      @b{(input)} (*,G) node
* @param    pPerIntfStarGAssertEventInfo @b{(input)} StarG Assert FSM event related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmPerIntfStarGAssertActionA5(pimsmCB_t * pimsmCb,
                                    struct pimsmStarGNode_s * pStarGNode,
                                    pimsmPerIntfStarGAssertEventInfo_t
                                    *perIntfStarGAssertEventInfo)
{
  pimsmStarGIEntry_t *pStarGIEntry;
  L7_uint32 rtrIfNum;

  rtrIfNum = perIntfStarGAssertEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);    
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);   
    return L7_FAILURE;
  }

  /* Delete assert info (AssertWinner(*,G,I) */
  inetAddressZeroSet(pimsmCb ->family,
                     &pStarGIEntry->pimsmStarGIAssertWinnerAddress);

  /* AssertWinnerMetric(*,G,I) will then return their default values) */
  pStarGIEntry->pimsmStarGIAssertWinnerMetric 
  = PIMSM_DEFAULT_ASSERT_METRIC;
  pStarGIEntry->pimsmStarGIAssertWinnerMetricPref 
  = PIMSM_DEFAULT_ASSERT_PREFERENCE;
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Return Error (Unexpected event received)   
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pSGNode                      @b{(input)} (*,G) node
* @param    pPerIntfStarGAssertEventInfo @b{(input)} StarG Assert FSM event related data 
*
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t pimsmPerIntfStarGAssertReturnError(pimsmCB_t * pimsmCb,
                                      struct pimsmStarGNode_s * pStarGNode,
                                      pimsmPerIntfStarGAssertEventInfo_t 
                                      * perIntfStarGAssertEventInfo)
{
  MCAST_UNUSED_PARAM(pStarGNode);
  MCAST_UNUSED_PARAM(perIntfStarGAssertEventInfo);
  return(L7_FAILURE);    
}

/******************************************************************************
* @purpose  Action Routine when (*,G,I) Assert Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments     
*      
* @end
******************************************************************************/
static void  pimsmStarGIAssertTimerExpiresHandler(void *pParam)
{
  pimsmPerIntfStarGAssertEventInfo_t perIntfStarGAssertEventInfo;
  pimsmStarGNode_t *pStarGNode;
  pimsmStarGIEntry_t *pStarGIEntry = L7_NULLPTR;
  L7_uint32 rtrIfNum;
  pimsmCB_t  * pimsmCb;
  pimsmTimerData_t *pTimerData;
  L7_int32      handle = (L7_int32)pParam; 
  L7_inet_addr_t *pGrpAddr;
  
  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);  
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }  
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);  
  if (pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM,PIMSM_TRACE_NORMAL,
        "(*,G) Find failed");
    return;
  }   

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "RtrIfNum: %d",rtrIfNum); 

  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);
   return;
  }
  if (pStarGIEntry->pimsmStarGIAssertTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM pimsmStarGIAssertTimer is NULL, But Still Expired");
    return;
  }
  pStarGIEntry->pimsmStarGIAssertTimer = L7_NULLPTR;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_NOTICE, "(*,G,I) Assert Timer Expired"); 
  memset(&perIntfStarGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));

  inetAddressZeroSet(pimsmCb->family,&perIntfStarGAssertEventInfo.srcAddr);
  perIntfStarGAssertEventInfo.eventType = 
  PIMSM_ASSERT_STAR_G_SM_EVENT_ASSERT_TIMER_EXPIRES;
  perIntfStarGAssertEventInfo.rtrIfNum = rtrIfNum;

  pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode, &perIntfStarGAssertEventInfo);
  if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode,MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
      "Failed to update (*,G)");
    return;
  }
}

