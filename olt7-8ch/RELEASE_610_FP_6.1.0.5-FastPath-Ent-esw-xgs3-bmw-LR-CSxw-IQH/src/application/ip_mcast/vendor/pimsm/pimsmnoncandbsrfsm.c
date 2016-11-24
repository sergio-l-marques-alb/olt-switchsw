/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmNonCandBsrFSM.c
*
* @purpose Contains PIM-SM Non-Cand Candidate BSR state Machine implementation
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
#include "pimsmbsr.h"
#include "pimsmnoncandbsrfsm.h"


static 
L7_RC_t pimsmPerScopeZoneNonCandBSRRpSetStoreBSMFwdTimersSet
 (pimsmCB_t *pimsmCb,
  pimsmPerScopeZoneNonCandBSREventInfo_t *nonCandBsrEventInfo);
static 
L7_RC_t pimsmPerScopeZoneNonCandBSRStateClear
 (pimsmCB_t * pimsmCb,
  pimsmPerScopeZoneNonCandBSREventInfo_t *nonCandBsrEventInfo);
static 
L7_RC_t pimsmPerScopeZoneNonCandBSRRpSetRefresh
 (pimsmCB_t * pimsmCb,
  pimsmPerScopeZoneNonCandBSREventInfo_t *nonCandBsrEventInfo);
static 
L7_RC_t pimsmPerScopeZoneNonCandBSRDoNothing
 (pimsmCB_t * pimsmCb,
  pimsmPerScopeZoneNonCandBSREventInfo_t *nonCandBsrEventInfo);
static 
L7_RC_t pimsmPerScopeZoneNonCandBSRErrorReturn
 (pimsmCB_t * pimsmCb,
  pimsmPerScopeZoneNonCandBSREventInfo_t *nonCandBsrEventInfo);

typedef struct pimsmPerScopeZoneNonCandBSRSM_s
{
  pimsmPerScopeZoneNonCandBSRStates_t nextState;
  L7_RC_t (*pimsmPerScopeZoneNonCandBSRAction) 
  (pimsmCB_t * pimsmCb,
   pimsmPerScopeZoneNonCandBSREventInfo_t *nonCandBsrEventInfo);
} pimsmPerScopeZoneNonCandBSRSM_t;

pimsmPerScopeZoneNonCandBSRSM_t pimsmPerScopeZoneNonCandBSR 
[PIMSM_NON_CANDIDATE_BSR_SM_EVENT_MAX]
[PIM_NON_CANDIDATE_BSR_SM_STATE_MAX] = 
{
  /* Receive BSM */
  {
    /* NoInfo */{PIM_NON_CANDIDATE_BSR_SM_STATE_PREFERRED,
      pimsmPerScopeZoneNonCandBSRRpSetStoreBSMFwdTimersSet},
    /* AA */    {PIM_NON_CANDIDATE_BSR_SM_STATE_PREFERRED,
      pimsmPerScopeZoneNonCandBSRRpSetStoreBSMFwdTimersSet},
    /*AP */     {PIM_NON_CANDIDATE_BSR_SM_STATE_PREFERRED,
      pimsmPerScopeZoneNonCandBSRErrorReturn} 
  },
  /* Scope-Zone Expiry Timer Expires*/
  {
    /* NoInfo */ {PIM_NON_CANDIDATE_BSR_SM_STATE_NO_INFO, 
      pimsmPerScopeZoneNonCandBSRErrorReturn},
    /* AA */     {PIM_NON_CANDIDATE_BSR_SM_STATE_NO_INFO,
      pimsmPerScopeZoneNonCandBSRStateClear},
    /*AP */      {PIM_NON_CANDIDATE_BSR_SM_STATE_PREFERRED, 
      pimsmPerScopeZoneNonCandBSRErrorReturn} 
  },
  /* Receive Preferred BSM */
  {
    /* NoInfo */   {PIM_NON_CANDIDATE_BSR_SM_STATE_NO_INFO, 
      pimsmPerScopeZoneNonCandBSRErrorReturn},
    /* AA */       {PIM_NON_CANDIDATE_BSR_SM_STATE_ACCEPT_ANY, 
      pimsmPerScopeZoneNonCandBSRErrorReturn},
    /*AP */        {PIM_NON_CANDIDATE_BSR_SM_STATE_PREFERRED,
      pimsmPerScopeZoneNonCandBSRRpSetStoreBSMFwdTimersSet} 
  },
  /* Bootstrap Timer Expires*/
  {
    /* NoInfo */   {PIM_NON_CANDIDATE_BSR_SM_STATE_NO_INFO, 
      pimsmPerScopeZoneNonCandBSRErrorReturn},
    /* AA */       {PIM_NON_CANDIDATE_BSR_SM_STATE_ACCEPT_ANY, 
      pimsmPerScopeZoneNonCandBSRErrorReturn},
    /*AP */        {PIM_NON_CANDIDATE_BSR_SM_STATE_ACCEPT_ANY,
      pimsmPerScopeZoneNonCandBSRRpSetRefresh}
  },
  /* Receive Non-Preferred BSM */
  {
    /* NoInfo */   {PIM_NON_CANDIDATE_BSR_SM_STATE_NO_INFO, 
      pimsmPerScopeZoneNonCandBSRErrorReturn},
    /* AA */       {PIM_NON_CANDIDATE_BSR_SM_STATE_ACCEPT_ANY, 
      pimsmPerScopeZoneNonCandBSRErrorReturn},
    /*AP */        {PIM_NON_CANDIDATE_BSR_SM_STATE_PREFERRED, 
      pimsmPerScopeZoneNonCandBSRDoNothing} 
  }
};

/******************************************************************************
*
* @purpose     execute the Per Scope Zone Non-Cand BSR FSM and process the events 
*
* @param    pimsmCb               @b{(input)} PIMSM Control Block
* @param    nonCandBsrEventInfo   @b{(input)} Non candidate BSR FSM Event Info
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @comments       
*          
* @end
*
******************************************************************************/
L7_RC_t pimsmPerScopeZoneNonCandBSRExecute (pimsmCB_t * pimsmCb,
                                            pimsmPerScopeZoneNonCandBSREventInfo_t
                                            *nonCandBsrEventInfo) 
{
  pimsmPerScopeZoneNonCandBSRStates_t nonCandBsrState;
  pimsmPerScopeZoneNonCandBSREvents_t nonCandBsrEvent;

  if((pimsmCb == (pimsmCB_t *)L7_NULLPTR) ||(nonCandBsrEventInfo == L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_NORMAL, "nonCandBSR FSM recvd NULLPTR");
    return L7_FAILURE;
  }
  nonCandBsrState = nonCandBsrEventInfo->pBsrNode->zone.nbsr.pimsmNonCandBSRState;
  nonCandBsrEvent = nonCandBsrEventInfo->eventType;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmPerScopeZoneNonCandBSRStateName[nonCandBsrState], 
              pimsmPerScopeZoneNonCandBSREventName[nonCandBsrEvent]);


  if((pimsmPerScopeZoneNonCandBSR[nonCandBsrEvent][nonCandBsrState]).
     pimsmPerScopeZoneNonCandBSRAction(pimsmCb, nonCandBsrEventInfo) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR, " action returned failure ");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR,"PerScopeZoneNonCandBSRAction failed. State = %s, \n Event = %s",
              pimsmPerScopeZoneNonCandBSRStateName[nonCandBsrState], 
              pimsmPerScopeZoneNonCandBSREventName[nonCandBsrEvent]);
    return(L7_FAILURE);
  }

  nonCandBsrEventInfo->pBsrNode->zone.nbsr.pimsmNonCandBSRState = 
  (pimsmPerScopeZoneNonCandBSR[nonCandBsrEvent][nonCandBsrState]).nextState;

  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_INFO, "Next State = %s",
              pimsmPerScopeZoneNonCandBSRStateName[nonCandBsrEventInfo->pBsrNode->zone.cbsr.pimsmCandBSRState]) 

  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose  Handler for bsrBootStrapTimerExpires
*
* @param    pParam   @b{(input)}  timer handle
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmBsrNonCandidateBootStrapTimerExpiresHandler(void *pParam)
{
  L7_int32      handle = (L7_int32)pParam;
  pimsmBsrPerScopeZone_t  *bsr_node;
  pimsmPerScopeZoneNonCandBSREventInfo_t       nonCandEventInfo;

  bsr_node = (pimsmBsrPerScopeZone_t*)handleListNodeRetrieve(handle);

  if(bsr_node == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_NORMAL, "bsr_node == L7_NULLPTR");
    return ;
  }

  if (bsr_node->pimsmBSRTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM pimsmBSRTimer is NULL, But Still Expired");
    return;
  }
  bsr_node->pimsmBSRTimer = L7_NULLPTR;

  memset(&nonCandEventInfo,0,sizeof(nonCandEventInfo));
  nonCandEventInfo.eventType = PIMSM_NON_CANDIDATE_BSR_SM_EVENT_BOOTSTRAP_TIMER_EXPIRES;
  nonCandEventInfo.pBsrNode = bsr_node;

  pimsmPerScopeZoneNonCandBSRExecute(bsr_node->pimsmCb,&nonCandEventInfo);
  return;
}

/******************************************************************************
* @purpose  Handler for Scope Zone Timer Expires
*
* @param    pParam   @b{(input)}  timer handle
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmBsrNonCandidateScopeZoneTimerExpiresHandler(void *pParam)
{
  L7_int32      handle = (L7_int32)pParam;
  pimsmBsrPerScopeZone_t  *bsr_node;
  pimsmPerScopeZoneNonCandBSREventInfo_t       nonCandEventInfo;

  bsr_node = (pimsmBsrPerScopeZone_t *)handleListNodeRetrieve(handle);

  if(bsr_node == L7_NULLPTR)
    return;

  if (bsr_node->zone.nbsr.pimsmBSRScopeZoneExpireTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM pimsmBSRScopeZoneExpireTimer is NULL, But Still Expired");
    return;
  }
  bsr_node->zone.nbsr.pimsmBSRScopeZoneExpireTimer = L7_NULLPTR;

  memset(&nonCandEventInfo,0,sizeof(nonCandEventInfo));
  nonCandEventInfo.eventType = PIMSM_NON_CANDIDATE_BSR_SM_EVENT_SCOPEZONE_TIMER_EXPIRES;
  nonCandEventInfo.pBsrNode = bsr_node;

  pimsmPerScopeZoneNonCandBSRExecute(bsr_node->pimsmCb,&nonCandEventInfo);
  return;
}

/******************************************************************************
* @purpose  Forward Boot Strap Msg, Store the RP-Set     
*
* @param    pimsmCb               @b{(input)} PIMSM Control Block
* @param    nonCandBsrEventInfo   @b{(input)} Non candidate BSR FSM Event Info
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @comments       
*          
* @end
******************************************************************************/
static 
L7_RC_t pimsmPerScopeZoneNonCandBSRRpSetStoreBSMFwdTimersSet(pimsmCB_t * pimsmCb,
                                                             pimsmPerScopeZoneNonCandBSREventInfo_t
                                                             *nonCandBsrEventInfo)
{
  /* Forward BSM; */
  pimsmBsrPacketForward(pimsmCb,
                     nonCandBsrEventInfo->pBsrNode,
                     nonCandBsrEventInfo->pimsmPktInfo.pimHeader,
                     nonCandBsrEventInfo->pimsmPktInfo.pimPktLen,
                     nonCandBsrEventInfo->pimsmPktInfo.rtrIfNum);

  /* Store RP-Set; */
  pimsmBsrPacketRpGrpMappingStore(pimsmCb,
                     nonCandBsrEventInfo->pBsrNode,
                     nonCandBsrEventInfo->pimsmPktInfo.pimHeader,
                     nonCandBsrEventInfo->pimsmPktInfo.pimPktLen);

  /* set the Bootstrap Timer */
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmBsrNonCandidateBootStrapTimerExpiresHandler,
                            (void*)nonCandBsrEventInfo->pBsrNode->pimsmBSRTimerHandle,
                            PIMSM_DEFAULT_BOOTSTRAP_TIMEOUT,
                            &(nonCandBsrEventInfo->pBsrNode->pimsmBSRTimer),
                            "NC-BSR")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR,"bsrCandidateBSRBootstrapTimer Add Failed");
    return L7_FAILURE;
  }

  /* Set SZT to SZ_Timeout; */
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmBsrNonCandidateScopeZoneTimerExpiresHandler,
                            (void*)nonCandBsrEventInfo->pBsrNode,
                            PIMSM_DEFAULT_BOOTSTRAP_SZ_TIMEOUT,
                            &(nonCandBsrEventInfo->pBsrNode->zone.nbsr.pimsmBSRScopeZoneExpireTimer),
                            "NC-SZT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR,"bsrCandidateBSRScopeZoneTimer Add Failed");
    return L7_FAILURE;
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Cancel the timers and clear the state     
*
* @param    pimsmCb               @b{(input)} PIMSM Control Block
* @param    nonCandBsrEventInfo   @b{(input)} Non candidate BSR FSM Event Info
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @comments       
*          
* @end
******************************************************************************/
static 
L7_RC_t  pimsmPerScopeZoneNonCandBSRStateClear(pimsmCB_t * pimsmCb,
                                               pimsmPerScopeZoneNonCandBSREventInfo_t
                                               *nonCandBsrEventInfo)
{
  pimsmBsrPerScopeZone_t *bsr_node = nonCandBsrEventInfo->pBsrNode;
  pimsmFragGrpRpNode_t *frag_node = L7_NULLPTR;

  /* Cancel Timers; */
  pimsmUtilAppTimerCancel (pimsmCb, &(bsr_node->pimsmBSRTimer));
  pimsmUtilAppTimerCancel (pimsmCb, &(bsr_node->zone.nbsr.pimsmBSRScopeZoneExpireTimer));

  /* TBD: Clear State - Delete the bsr node and also the grp-rp mappings */
  while((frag_node = (pimsmFragGrpRpNode_t *)
         SLLFirstGet(&(bsr_node->pimsmBSRFragInfo.pimsmGrpRpList))) != L7_NULLPTR)
  {
    SLLNodeDelete(&(bsr_node->pimsmBSRFragInfo.pimsmGrpRpList),
                (L7_sll_member_t*)frag_node);
  }
  bsr_node->pimsmBSRFragInfo.pimsmFragTag = 0;

  pimsmBsrInfoNodeDelete(pimsmCb, bsr_node);

  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose  Refresh the RP-Set and Remove BSR state info     
*
* @param    pimsmCb               @b{(input)} PIMSM Control Block
* @param    nonCandBsrEventInfo   @b{(input)} Non candidate BSR FSM Event Info
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @comments       
*          
* @end
******************************************************************************/
static 
L7_RC_t  pimsmPerScopeZoneNonCandBSRRpSetRefresh(pimsmCB_t * pimsmCb,
                                             pimsmPerScopeZoneNonCandBSREventInfo_t
                                             *nonCandBsrEventInfo)
{

  pimsmBsrPerScopeZone_t *bsr_node = nonCandBsrEventInfo->pBsrNode;
  pimsmFragGrpRpNode_t       *frag_node;

  /* Refresh RP-Set; */
  pimsmBsrRpGrpMappingRefresh(pimsmCb,bsr_node);

  /* Remove BSR State - Clear the BSR info */
  inetAddressZeroSet (pimsmCb->family, &bsr_node->pimsmBSRAddr);
  bsr_node->pimsmBSRHashMasklen= 0;
  bsr_node->pimsmBSRPriority= 0;

  /* Remove all last received BSM fragments */
  memset(bsr_node->pimsmLastBSMPktNode, 0, sizeof(bsr_node->pimsmLastBSMPktNode));
  
  while((frag_node = (pimsmFragGrpRpNode_t *)
         SLLFirstGet(&(bsr_node->pimsmBSRFragInfo.pimsmGrpRpList))) != L7_NULLPTR)
  {
    SLLNodeDelete(&(bsr_node->pimsmBSRFragInfo.pimsmGrpRpList),
                (L7_sll_member_t*)frag_node);
  }
  bsr_node->pimsmBSRFragInfo.pimsmFragTag = 0;
  return(L7_SUCCESS);
}


/******************************************************************************
* @purpose  Change the state and return     
*
* @param    pimsmCb               @b{(input)} PIMSM Control Block
* @param    nonCandBsrEventInfo   @b{(input)} Non candidate BSR FSM Event Info
*
* @returns     L7_SUCCESS
*
* @comments       
*          
* @end
******************************************************************************/
static 
L7_RC_t pimsmPerScopeZoneNonCandBSRDoNothing(pimsmCB_t * pimsmCb,
                                             pimsmPerScopeZoneNonCandBSREventInfo_t
                                             *nonCandBsrEventInfo)
{
  return(L7_SUCCESS);  
}
/******************************************************************************
* @purpose     Unexpected Event and return error
*
* @param    pimsmCb               @b{(input)} PIMSM Control Block
* @param    nonCandBsrEventInfo   @b{(input)} Non candidate BSR FSM Event Info
*
* @returns     L7_FAILURE
*
* @comments       
*          
* @end
******************************************************************************/
static 
L7_RC_t pimsmPerScopeZoneNonCandBSRErrorReturn(pimsmCB_t * pimsmCb,
                                               pimsmPerScopeZoneNonCandBSREventInfo_t
                                               *nonCandBsrEventInfo)
{
  return(L7_FAILURE);
}
