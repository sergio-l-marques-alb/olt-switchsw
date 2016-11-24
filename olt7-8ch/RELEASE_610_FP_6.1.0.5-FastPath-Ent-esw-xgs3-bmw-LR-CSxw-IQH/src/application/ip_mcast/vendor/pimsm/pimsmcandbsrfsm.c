/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmCandBsrFSM.c
*
* @purpose Contains PIM-SM Candidate BSR state Machine implementation
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
#include "pimsmcandbsrfsm.h"
#include "pimsmtimer.h"


/* forward declaration */                            
static L7_RC_t pimsmPerScopeZoneCandBSRAction1
(pimsmCB_t * pimsmCb,
 pimsmPerScopeZoneCandBSREventInfo_t *candBsrEventInfo);
static L7_RC_t pimsmPerScopeZoneCandBSRAction2
(pimsmCB_t * pimsmCb,
 pimsmPerScopeZoneCandBSREventInfo_t *candBsrEventInfo);
static L7_RC_t pimsmPerScopeZoneCandBSRAction3 
(pimsmCB_t * pimsmCb,
 pimsmPerScopeZoneCandBSREventInfo_t *candBsrEventInfo);
static L7_RC_t pimsmPerScopeZoneCandBSRAction4 
(pimsmCB_t * pimsmCb,
 pimsmPerScopeZoneCandBSREventInfo_t *candBsrEventInfo);
static L7_RC_t pimsmPerScopeZoneCandBSRDoNothing
(pimsmCB_t * pimsmCb,
 pimsmPerScopeZoneCandBSREventInfo_t *candBsrEventInfo);
static L7_RC_t pimsmPerScopeZoneCandBSRErrorReturn
(pimsmCB_t * pimsmCb,
 pimsmPerScopeZoneCandBSREventInfo_t *candBsrEventInfo);


typedef struct pimsmPerScopeZoneCandBSRSM_s
{
  pimsmPerScopeZoneCandBSRStates_t nextState;
  L7_RC_t (*pimsmPerScopeZoneCandBSRAction)(pimsmCB_t * pimsmCb,
                        pimsmPerScopeZoneCandBSREventInfo_t *candBsrEventInfo); 
} pimsmPerScopeZoneCandBSRSM_t;


static pimsmPerScopeZoneCandBSRSM_t 
pimsmPerScopeZoneCandBSR[PIMSM_CANDIDATE_BSR_SM_EVENT_MAX]
[PIMSM_CANDIDATE_BSR_SM_STATE_MAX] =
{
/* Recv Preffered BSR*/
  {
    /* C-BSR state */ {PIMSM_CANDIDATE_BSR_SM_STATE_CANDIDATE_BSR, 
      pimsmPerScopeZoneCandBSRAction1},
    /* P-BSR state */ {PIMSM_CANDIDATE_BSR_SM_STATE_CANDIDATE_BSR,
      pimsmPerScopeZoneCandBSRAction1},
    /* E-BSR state */ {PIMSM_CANDIDATE_BSR_SM_STATE_CANDIDATE_BSR, 
      pimsmPerScopeZoneCandBSRAction1}
  },
/* Bootstrap Timer Expires */
  {
    /* C-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_PENDING_BSR,
      pimsmPerScopeZoneCandBSRAction2},
    /* P-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_ELECTED_BSR,
      pimsmPerScopeZoneCandBSRAction3},
    /* E-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_ELECTED_BSR,
      pimsmPerScopeZoneCandBSRAction3}
  },
/* Recv Non-Preffered BSR*/
  {
    /* C-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_CANDIDATE_BSR,
      pimsmPerScopeZoneCandBSRDoNothing},
    /* P-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_PENDING_BSR,
      pimsmPerScopeZoneCandBSRAction4},
    /* E-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_ELECTED_BSR,
      pimsmPerScopeZoneCandBSRAction3}
  },
/* Receive Non-preferred BSM from Elected BSR */
  {
    /* C-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_PENDING_BSR,
      pimsmPerScopeZoneCandBSRAction2},
    /* P-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_PENDING_BSR,
      pimsmPerScopeZoneCandBSRErrorReturn},
    /* E-BSR state */  {PIMSM_CANDIDATE_BSR_SM_STATE_ELECTED_BSR,
      pimsmPerScopeZoneCandBSRErrorReturn}
  }
};
/******************************************************************************
*
* @purpose   execute the Cand-BSR FSM and process the events received
*
* @param    pimsmCb            @b{(input)} PIMSM Control Block
* @param    candBsrEventInfo   @b{(input)} candidate BSR FSM Event Info
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
*
******************************************************************************/
L7_RC_t pimsmPerScopeZoneCandBSRExecute(pimsmCB_t * pimsmCb,
                                        pimsmPerScopeZoneCandBSREventInfo_t
                                        *candBsrEventInfo) 
{
  pimsmBsrPerScopeZone_t  *bsrNode = candBsrEventInfo->pBsrNode;
  pimsmPerScopeZoneCandBSRStates_t  candBsrState = 
                                     bsrNode->zone.cbsr.pimsmCandBSRState;
  pimsmPerScopeZoneCandBSREvents_t candBsrEvent = candBsrEventInfo->eventType;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmPerScopeZoneCandBSRStateName[candBsrState], 
              pimsmPerScopeZoneCandBSREventName[candBsrEvent]);


  /*Execute the action routine for the candBsrEvent received */
  if((pimsmPerScopeZoneCandBSR[candBsrEvent][candBsrState]).pimsmPerScopeZoneCandBSRAction 
     (pimsmCb, candBsrEventInfo) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR, " action returned failure ");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR,"PerScopeZoneCandBSRAction failed. State = %s, \n Event = %s",
       pimsmPerScopeZoneCandBSRStateName[candBsrState], 
       pimsmPerScopeZoneCandBSREventName[candBsrEvent]);
    return(L7_FAILURE);
  }
  /* Change to the new candBsrState */
  bsrNode->zone.cbsr.pimsmCandBSRState = 
               (pimsmPerScopeZoneCandBSR[candBsrEvent][candBsrState]).nextState; 

  PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_INFO, "Next State = %s",
              pimsmPerScopeZoneCandBSRStateName[bsrNode->zone.cbsr.pimsmCandBSRState]) 
  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose  Handler for BSR BootStrapTimerExpires
*
* @param    pParam    @b{(input)}  timer handle
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmBsrCandidateBootStrapTimerExpiresHandler (void *pParam)
{
  L7_int32      handle = (L7_int32)pParam;
  pimsmBsrPerScopeZone_t                *bsrNode; 
  pimsmPerScopeZoneCandBSREventInfo_t    candEventInfo;

  bsrNode = (pimsmBsrPerScopeZone_t *)handleListNodeRetrieve(handle);

  if(bsrNode == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR, " bsrNode is NULL ");
    return;
  }
  if (bsrNode->pimsmBSRTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM pimsmBSRTimer is NULL, But Still Expired");
    return;
  }
  bsrNode->pimsmBSRTimer = L7_NULLPTR;
  memset(&candEventInfo,0,sizeof(candEventInfo));
  candEventInfo.eventType = PIMSM_CANDIDATE_BSR_SM_EVENT_BOOTSTRAP_TIMER_EXPIRES;
  candEventInfo.pBsrNode = bsrNode;

  pimsmPerScopeZoneCandBSRExecute(bsrNode->pimsmCb,&candEventInfo);
  return;
}


/******************************************************************************
*
* @purpose   Forward the BootStrap Msg, store RP-Set received and set BSR timer
*
* @param    pimsmCb            @b{(input)} PIMSM Control Block
* @param    candBsrEventInfo   @b{(input)} candidate BSR FSM Event Info
*
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
*
******************************************************************************/
static 
L7_RC_t  pimsmPerScopeZoneCandBSRAction1(pimsmCB_t * pimsmCb,
                                         pimsmPerScopeZoneCandBSREventInfo_t 
                                         *candBsrEventInfo)
{
  pimsmBsrPerScopeZone_t      *bsrNode;

  bsrNode = candBsrEventInfo->pBsrNode;
  /*Forward BSM;*/
  pimsmBsrPacketForward(pimsmCb, 
                     bsrNode,
                     candBsrEventInfo->pimsmPktInfo.pimHeader,  
                     candBsrEventInfo->pimsmPktInfo.pimPktLen,
                     candBsrEventInfo->pimsmPktInfo.rtrIfNum);

  /*Store RP-Set;*/
  pimsmBsrPacketRpGrpMappingStore(pimsmCb,
                     bsrNode,
                     candBsrEventInfo->pimsmPktInfo.pimHeader, 
                     candBsrEventInfo->pimsmPktInfo.pimPktLen); 

  /*Set Bootstrap Timer to BS_timeout;*/
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmBsrCandidateBootStrapTimerExpiresHandler,
                            (void*)bsrNode->pimsmBSRTimerHandle,
                            PIMSM_DEFAULT_BOOTSTRAP_TIMEOUT,
                            &(bsrNode->pimsmBSRTimer),
                            "SM-BSR2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR, "bsrCandidateBSRBootstrapTimer Add Failed");
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
*
* @purpose   Set the BootStrap Timer, and update the BSR entry
*
* @param    pimsmCb            @b{(input)} PIMSM Control Block
* @param    candBsrEventInfo   @b{(input)} candidate BSR FSM Event Info
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
*
******************************************************************************/
static 
L7_RC_t  pimsmPerScopeZoneCandBSRAction2(pimsmCB_t * pimsmCb,
                                         pimsmPerScopeZoneCandBSREventInfo_t 
                                         *candBsrEventInfo)
{
  pimsmBsrPerScopeZone_t      *bsrNode= candBsrEventInfo->pBsrNode;

  /*Forward BSM;*/
  pimsmBsrPacketForward (pimsmCb, bsrNode, candBsrEventInfo->pimsmPktInfo.pimHeader,
                         candBsrEventInfo->pimsmPktInfo.pimPktLen,
                         candBsrEventInfo->pimsmPktInfo.rtrIfNum);

  /*Set Bootstrap Timer to BS_timeout;*/
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmBsrCandidateBootStrapTimerExpiresHandler,
                            (void*)bsrNode->pimsmBSRTimerHandle,
                            pimsmBsrRandOverrideIntervalGet(pimsmCb,bsrNode),
                            &(bsrNode->pimsmBSRTimer),
                            "SM-BSR3")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR,"bsrCandidateBSRBootstrapTimer Add Failed");
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
*
* @purpose   Originate BootStrap Mesg and set BS timer
*
* @param    pimsmCb            @b{(input)} PIMSM Control Block
* @param    candBsrEventInfo   @b{(input)} candidate BSR FSM Event Info
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
*
******************************************************************************/
static 
L7_RC_t  pimsmPerScopeZoneCandBSRAction3(pimsmCB_t * pimsmCb,
                                         pimsmPerScopeZoneCandBSREventInfo_t 
                                         *candBsrEventInfo)
{

  pimsmBsrPerScopeZone_t      *bsrNode= candBsrEventInfo->pBsrNode;

  /* originate the bootstrp message */
  inetCopy(&bsrNode->zone.cbsr.pimsmElectedBSRAddress,
           &(pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRAddress));
  bsrNode->zone.cbsr.pimsmElectedBSRPriority = 
                pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRPriority;
  bsrNode->zone.cbsr.pimsmElectedBSRHashMaskLen =
                pimsmCb->pimsmBsrInfo->pimsmCandBSRInfo.pimsmBSRCandidateBSRHashMaskLength;
  pimsmBsrCandRpToRpGrpListGet(pimsmCb,bsrNode);
  pimsmBsrPacketOriginate(pimsmCb,bsrNode,L7_NULLPTR,0);

  /*Set Bootstrap Timer to BS_period;*/
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmBsrCandidateBootStrapTimerExpiresHandler,
                            (void*)bsrNode->pimsmBSRTimerHandle,
                            PIMSM_DEFAULT_BOOTSTRAP_PERIOD,
                            &(bsrNode->pimsmBSRTimer),
                            "SM-BSR4")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_BSR_FSM,  PIMSM_TRACE_ERROR,"bsrCandidateBSRBootstrapTimer Add Failed");
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}

/******************************************************************************
*
* @purpose   Forward the BSM
*
* @param    pimsmCb            @b{(input)} PIMSM Control Block
* @param    candBsrEventInfo   @b{(input)} candidate BSR FSM Event Info
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
*
******************************************************************************/
static 
L7_RC_t  pimsmPerScopeZoneCandBSRAction4(pimsmCB_t * pimsmCb,
                                         pimsmPerScopeZoneCandBSREventInfo_t 
                                         *candBsrEventInfo)
{
  pimsmBsrPerScopeZone_t      *bsrNode= candBsrEventInfo->pBsrNode;

  /*Forward BSM;*/
  pimsmBsrPacketForward (pimsmCb, bsrNode, candBsrEventInfo->pimsmPktInfo.pimHeader,
                         candBsrEventInfo->pimsmPktInfo.pimPktLen,
                         candBsrEventInfo->pimsmPktInfo.rtrIfNum);

  return(L7_SUCCESS);
}

/******************************************************************************
*
* @purpose   Change the FSM state and return
*
* @param    pimsmCb            @b{(input)} PIMSM Control Block
* @param    candBsrEventInfo   @b{(input)} candidate BSR FSM Event Info
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
*
******************************************************************************/
static 
L7_RC_t pimsmPerScopeZoneCandBSRDoNothing(pimsmCB_t * pimsmCb,
                                          pimsmPerScopeZoneCandBSREventInfo_t 
                                          *candBsrEventInfo)
{
  MCAST_UNUSED_PARAM(pimsmCb);
  MCAST_UNUSED_PARAM(candBsrEventInfo);
  return(L7_SUCCESS);
}
/******************************************************************************
*
* @purpose   Return Error as it is not an expected event
*
* @param    pimsmCb            @b{(input)} PIMSM Control Block
* @param    candBsrEventInfo   @b{(input)} candidate BSR FSM Event Info
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
*
******************************************************************************/
static 
L7_RC_t pimsmPerScopeZoneCandBSRErrorReturn(pimsmCB_t * pimsmCb,
                                            pimsmPerScopeZoneCandBSREventInfo_t
                                            *candBsrEventInfo)
{
  MCAST_UNUSED_PARAM(pimsmCb);
  MCAST_UNUSED_PARAM(candBsrEventInfo);
  return(L7_FAILURE);
}


