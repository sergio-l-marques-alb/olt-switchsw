/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_dnstrm_fsm.c
*
* @purpose    PIM-DM Downstream Interface State machine Action routines
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "sysapi.h"
#include "pimdm_dnstrm_fsm.h"
#include "pimdm_debug.h"
#include "pimdm_intf.h"
#include "pimdm_util.h"

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/

static L7_RC_t
pimdmDnstrmFsmActionSetPPT (pimdmMrtEntry_t* mrtEntry,
                            pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionResetPT (pimdmMrtEntry_t* mrtEntry,
                             pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionCancelPPT (pimdmMrtEntry_t* mrtEntry,
                               pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionCancelPT (pimdmMrtEntry_t* mrtEntry,
                              pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionSendGraftAck (pimdmMrtEntry_t* mrtEntry,
                                  pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionSendGraftAckCancelPPT (pimdmMrtEntry_t* mrtEntry,
                                       pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionSendGraftAckCancelPT (pimdmMrtEntry_t* mrtEntry,
                                       pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionSetPT (pimdmMrtEntry_t* mrtEntry,
                           pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionDoNothing (pimdmMrtEntry_t* mrtEntry,
                               pimdmDnstrmEventInfo_t *dnstrmEventInfo);

static L7_RC_t
pimdmDnstrmFsmActionError (pimdmMrtEntry_t* mrtEntry,
                           pimdmDnstrmEventInfo_t *dnstrmEventInfo);

/*********************************************************************
*      Downstream Interface State machine State table definition
*********************************************************************/
static pimdmDnstrmFsmState_t
pimdmDnstrmFsmState[PIMDM_DNSTRM_MAX_EVENTS][PIMDM_DNSTRM_MAX_STATES] =
{
  /* Receive Prune(S,G) */
  {{PIMDM_DNSTRM_STATE_PRUNE_PENDING, pimdmDnstrmFsmActionSetPPT},
   {PIMDM_DNSTRM_STATE_PRUNE_PENDING, pimdmDnstrmFsmActionDoNothing},
   {PIMDM_DNSTRM_STATE_PRUNED, pimdmDnstrmFsmActionResetPT}
  },
  
  /* Receive Join(S,G) */
  {{PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionDoNothing},
   {PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionCancelPPT},
   {PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionCancelPT}
  },
  
  /* Receive Graft(S,G) */
  {{PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionSendGraftAck},
   {PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionSendGraftAckCancelPPT},
   {PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionSendGraftAckCancelPT}
  },
  
  /* PPT(S,G) Expiry */
  {{PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionError},
   {PIMDM_DNSTRM_STATE_PRUNED, pimdmDnstrmFsmActionSetPT},
   {PIMDM_DNSTRM_STATE_PRUNED, pimdmDnstrmFsmActionError}
  },
  
  /* PT(S,G) Expiry */ 
  {{PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionError},
   {PIMDM_DNSTRM_STATE_PRUNE_PENDING, pimdmDnstrmFsmActionError},
   {PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionDoNothing}
  },
  
  /* RPF(S) becomes I */ 
  {{PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionDoNothing},
   {PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionCancelPPT},
   {PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionCancelPT}
  },
  
  /* Send State Refresh(S,G) on I */ 
  {{PIMDM_DNSTRM_STATE_NOINFO, pimdmDnstrmFsmActionDoNothing},
   {PIMDM_DNSTRM_STATE_PRUNE_PENDING, pimdmDnstrmFsmActionDoNothing},
   {PIMDM_DNSTRM_STATE_PRUNED, pimdmDnstrmFsmActionResetPT}
  }
};

/*********************************************************************
*            Downstream Interface State machine Events List
*   (This array is indexed based on the PIMDM_DNSTRM_EVENT_t enum)
*********************************************************************/
static L7_uchar8* pimdmDnstrmFsmEventList[] =
{
  "PIMDM_DNSTRM_EVT_PRUNED_ARVL",
  "PIMDM_DNSTRM_EVT_JOIN_ARVL",
  "PIMDM_DNSTRM_EVT_GRAFT_ARVL",
  "PIMDM_DNSTRM_EVT_PPT_EXPIRY",
  "PIMDM_DNSTRM_EVT_PT_EXPIRY",
  "PIMDM_DNSTRM_EVT_RPF_IF_CHNG",
  "PIMDM_DNSTRM_EVT_SEND_ST_RFR",
};

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Downstream Interface State machine Action routine invoker
*
* @param   dnstrmEvent      @b{ (input) } Type of the event
*          mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments   
*
* @end
*********************************************************************/

L7_RC_t pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVENT_t dnstrmEvent,
                               pimdmMrtEntry_t* mrtEntry,
                               pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  PIMDM_DNSTRM_STATE_t currentState;
  pimdmDnstrmFsmState_t *dnstrmFsmState = L7_NULLPTR;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");

  /* Input Validations */
  if ((mrtEntry == L7_NULLPTR) ||
      (dnstrmEvent >= PIMDM_DNSTRM_MAX_EVENTS) ||
      (dnstrmEventInfo == L7_NULLPTR))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Downstream FSM invoked with invalid parameters\n");
    return L7_FAILURE;
  }

  rtrIfNum = dnstrmEventInfo->rtrIfNum;

  if ((rtrIfNum < 1) || (rtrIfNum >= PIMDM_MAX_INTERFACES))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "rtrIfNum - [%d] Out of Bounds !!! for Event [%s]",
                 rtrIfNum, pimdmDnstrmFsmEventList[dnstrmEvent]);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,L7_FLEX_PIMDM_MAP_COMPONENT_ID, "PIMDM FSM Action Invoke Failed; rtrIfNum "
				"- [%d] Out of Bounds for Event [%s]. The PIMDM FSM Action invocation Failed due to invalid "
				"Routing interface number. In such cases, the FSM Action routine can never be invoked which "
				"may result in abnormal behavior. The failed FSM-name can be identified from the specified "
				"Event name.", rtrIfNum, pimdmDnstrmFsmEventList[dnstrmEvent]);
    return L7_FAILURE;
  }

  if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum]) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Downstrm Intf - %d", rtrIfNum);
    return L7_FAILURE;
  }

  /* Get the current State and the State table's next State info.*/
  currentState = dnstrmIntfInfo->pruneState;

  dnstrmFsmState = &(pimdmDnstrmFsmState[dnstrmEvent][currentState]);

 /* Update the current State with that of the next State from the state table.*/
  dnstrmIntfInfo->pruneState = dnstrmFsmState->nextState;

  /* Execute the Action Routine pointed by the current State and event. */
  if ((dnstrmFsmState->fsmAction (mrtEntry, dnstrmEventInfo)) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Dnstrm FSM State Transition Failed "
                 "for S-%s, G-%s\n "
                 "State: [%d] ... Event [%s]",
                 inetAddrPrint(&mrtEntry->srcAddr,src),
                 inetAddrPrint(&mrtEntry->grpAddr,grp),
                 currentState, pimdmDnstrmFsmEventList[dnstrmEvent]);
    /* Since the Action has failed, rollback to the previous State.
     */
    dnstrmIntfInfo->pruneState = currentState;
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Dnstrm FSM State Transition Successful "
               "for S-%s, G-%s\n "
               "Curr State: [%d] ... Event: [%s] ... Next State: [%d]",
               inetAddrPrint(&mrtEntry->srcAddr,src),
               inetAddrPrint(&mrtEntry->grpAddr,grp),
               currentState, pimdmDnstrmFsmEventList[dnstrmEvent],
               dnstrmFsmState->nextState);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  MRT Entry's Downstream interface PPT expiry Handler
*
* @param    mrtEntryDnstrmTmrHndlParam  @b{ (input) } Handle to get the MRT's Downstream entry info 
*                            
* @returns  None
*
* @comments    
*
* @end
*********************************************************************/
static void pimdmDnstrmPrunePendingTimerExpiryHandler (void *mrtEntryDnstrmTmrHndlParam)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo =L7_NULLPTR;
  pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");

  if ((dnstrmIntfInfo = (pimdmDownstrmIntfInfo_t*)
                         handleListNodeRetrieve ((L7_uint32)mrtEntryDnstrmTmrHndlParam))
                      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  if (dnstrmIntfInfo->prunePndTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMDM Downstream prunePndTimer (PPT) is NULL, But Still Expired");
    return;
  }
  dnstrmIntfInfo->prunePndTimer = L7_NULLPTR;

  if ((mrtEntry = dnstrmIntfInfo->mrtEntry) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "mrtEntry is NULLPTR");
    return;
  }

  memset(&dnstrmFsmEventInfo,0,sizeof(pimdmDnstrmEventInfo_t));
  dnstrmFsmEventInfo.rtrIfNum = dnstrmIntfInfo->rtrIfNum;
  dnstrmFsmEventInfo.holdTime = dnstrmIntfInfo->maxPruneHoldTime;
  inetAddressZeroSet (mrtEntry->pimdmCB->addrFamily, &(dnstrmFsmEventInfo.nbrAddr));

  pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_PPT_EXPIRY, mrtEntry,
                         &dnstrmFsmEventInfo);

  PIMDM_TRACE(PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return;
}


/*********************************************************************
*
* @purpose  MRT Entry's Downstream interface Prune expiry Handler
*
* @param    mrtEntryDnstrmTmrHndlParam  @b{ (input) } Handle to get the MRT's Downstream entry info 
*                            
* @returns  None
*
* @comments   
*
* @end
*********************************************************************/
static void pimdmDnstrmPruneTimerExpiryHandler (void *mrtEntryDnstrmTmrHndlParam)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");

  if ((dnstrmIntfInfo = (pimdmDownstrmIntfInfo_t*)
                         handleListNodeRetrieve ((L7_uint32)mrtEntryDnstrmTmrHndlParam))
                      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  if (dnstrmIntfInfo->pruneTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMDM Downstream pruneTimer (PT) is NULL, But Still Expired");
    return;
  }
  dnstrmIntfInfo->pruneTimer = L7_NULLPTR;

  if ((mrtEntry = dnstrmIntfInfo->mrtEntry) == L7_NULLPTR)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Failed to retrive MRT entry");
    return;
  }

  memset(&dnstrmFsmEventInfo,0,sizeof(pimdmDnstrmEventInfo_t));
  dnstrmFsmEventInfo.rtrIfNum = dnstrmIntfInfo->rtrIfNum;
  dnstrmFsmEventInfo.holdTime = dnstrmIntfInfo->maxPruneHoldTime;
  inetAddressZeroSet (mrtEntry->pimdmCB->addrFamily, &(dnstrmFsmEventInfo.nbrAddr));

  pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_PT_EXPIRY, mrtEntry,
                         &dnstrmFsmEventInfo);

  /*Evaluate OifList as the downstream state changed */
  pimdmMrtDnstrmPruneMaskEval (dnstrmIntfInfo->mrtEntry);

  PIMDM_TRACE(PIMDM_DEBUG_FSM_DNSTRM,"Exit");
  return;
}

/*********************************************************************
*
* @purpose Set the prune pending Timer.
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*
* @end
*********************************************************************/

static 
L7_RC_t pimdmDnstrmFsmActionSetPPT(pimdmMrtEntry_t* mrtEntry,
                                   pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  pimdmCB_t *pimdmCB = mrtEntry->pimdmCB;
  L7_int32  lpdTimeout=0,oiTimeout=0,timeout =-1;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");

  rtrIfNum = dnstrmEventInfo->rtrIfNum;
  dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum];

  /* Get the number of neighbors in this interface if less than two
   * then set timeout to zero */
  if(pimdmIntfNumNeighborsGet(pimdmCB, rtrIfNum) > 1)
  {
    /* Get Override interval + Propogation interval of the Interface API */
    lpdTimeout = pimdmIntfLanPropagationDelayGet(pimdmCB, rtrIfNum);
    if(lpdTimeout == -1)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                  "Failed to get LPD delay on rtrIfNum - %d", rtrIfNum);
      return L7_FAILURE;
    }
    oiTimeout = pimdmIntfOverrideIntervalGet(pimdmCB, rtrIfNum);
    if(oiTimeout == -1)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                  "Failed to get Override Interval on rtrIfNum - %d", rtrIfNum);
      return L7_FAILURE;
    }
  }

  /* Set the PPT Timer */
  osapiSnprintf(errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                "PPT (S,G,I) Set Failed for Src - %s Grp - %s Intf - %d ",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp), rtrIfNum);
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PPT (S,G,I) Set Success for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp), rtrIfNum);

  /*Converting oiTimeout and lpdTimeout in seconds and ceil the value */
  timeout = ((oiTimeout + lpdTimeout) + 999)/1000;

  if (pimdmUtilAppTimerSet (pimdmCB, pimdmDnstrmPrunePendingTimerExpiryHandler,
                            (void*) dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam,
                            timeout, errMsgBuf, sucMsgBuf,
                            &dnstrmIntfInfo->prunePndTimer,
                            "DM-PPT")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to set Prune Pending Timer on rtrIfNum - %d", rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Reset the prune Timer.
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/

static 
L7_RC_t pimdmDnstrmFsmActionResetPT (pimdmMrtEntry_t* mrtEntry,
                                     pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  pimdmCB_t *pimdmCB = mrtEntry->pimdmCB;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);
  L7_uint32 timeLeft;
  L7_uint32 rtrIfNum = 0;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");

  rtrIfNum = dnstrmEventInfo->rtrIfNum;
  dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum];

  if(appTimerTimeLeftGet(pimdmCB->appTimer, dnstrmIntfInfo->pruneTimer,
                         &timeLeft) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Prune Timer not running on rtrIfNum %d",
                rtrIfNum);
    return L7_FAILURE;
  }

  if (dnstrmEventInfo->holdTime < timeLeft)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, 
                 "Prune Hold time in message < Prune Timer value running\n");
    return L7_SUCCESS;
  }

  /* Reset the prune timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PT (S,G,I) ReSet Failed for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 rtrIfNum);
  osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "PT (S,G,I) ReSet Success for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 rtrIfNum);

  if (pimdmUtilAppTimerSet (pimdmCB, pimdmDnstrmPruneTimerExpiryHandler,
                            (void*) dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam,
                            dnstrmEventInfo->holdTime, errMsgBuf, sucMsgBuf,
                            &dnstrmIntfInfo->pruneTimer,
                            "DM-DnPT")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Resetting Prune Timer Failed on rtrIfNum %d",
                rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Cancel the prune pending Timer.
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/
static 
L7_RC_t pimdmDnstrmFsmActionCancelPPT(pimdmMrtEntry_t* mrtEntry,
                                      pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");
  
  /*cancel the Prune Pending timer */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
      &(mrtEntry->downstrmStateInfo[dnstrmEventInfo->rtrIfNum]->prunePndTimer));
  
  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Cancel the prune Timer.
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/
static 
L7_RC_t pimdmDnstrmFsmActionCancelPT(pimdmMrtEntry_t* mrtEntry,
                                     pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");
  
  /*cancel the Prune timer */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
         &(mrtEntry->downstrmStateInfo[dnstrmEventInfo->rtrIfNum]->pruneTimer));

  /* evaluate the prunemask Change */
  pimdmMrtDnstrmPruneMaskEval (mrtEntry);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Send Graft Ack 
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/
static 
L7_RC_t pimdmDnstrmFsmActionSendGraftAck(pimdmMrtEntry_t* mrtEntry,
                                        pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");

  if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_GRAFT_ACK,
                           &dnstrmEventInfo->nbrAddr, &dnstrmEventInfo->nbrAddr,
                           dnstrmEventInfo->rtrIfNum, 0, 0, 0)
                        != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to send GraftAck on rtrIfNum %d",
                 dnstrmEventInfo->rtrIfNum);
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Send Graft Ack  +cancel ppt timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/
static 
L7_RC_t pimdmDnstrmFsmActionSendGraftAckCancelPPT(
                                       pimdmMrtEntry_t* mrtEntry,
                                       pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");

  if (pimdmMrtEntryControlPacketSend (mrtEntry,
                                      PIMDM_CTRL_PKT_GRAFT_ACK,
                                      &dnstrmEventInfo->nbrAddr,
                                      &dnstrmEventInfo->nbrAddr,
                                      dnstrmEventInfo->rtrIfNum, 0,  0,  0)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to send Graft Ack on rtrIfNum %d",
                 dnstrmEventInfo->rtrIfNum);
  }

  /*cancel the Prune Pending timer */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
      &(mrtEntry->downstrmStateInfo[dnstrmEventInfo->rtrIfNum]->prunePndTimer));

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Send Graft Ack  + cancel prune timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/
static 
L7_RC_t pimdmDnstrmFsmActionSendGraftAckCancelPT(
                                      pimdmMrtEntry_t* mrtEntry,
                                      pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");
  
  if (pimdmMrtEntryControlPacketSend (mrtEntry,
                                      PIMDM_CTRL_PKT_GRAFT_ACK,
                                      &dnstrmEventInfo->nbrAddr,
                                      &dnstrmEventInfo->nbrAddr,
                                      dnstrmEventInfo->rtrIfNum,0,0,0)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to send Graft Ack on rtrIfNum %d",
                 dnstrmEventInfo->rtrIfNum);
  }

  /*cancel the timer */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
         &(mrtEntry->downstrmStateInfo[dnstrmEventInfo->rtrIfNum]->pruneTimer));

  /* evaluate the prunemask Change */
  pimdmMrtDnstrmPruneMaskEval (mrtEntry);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set prune timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*
* @end
*********************************************************************/
static 
L7_RC_t pimdmDnstrmFsmActionSetPT(pimdmMrtEntry_t* mrtEntry,
                                  pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  pimdmCB_t *pimdmCB = mrtEntry->pimdmCB;
  L7_uint32 timeout=0;
  L7_uint32 lpdTimeout=0,oiTimeout=0;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr),myAddress;
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);
  L7_uint32 rtrIfNum = 0;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");

  rtrIfNum = dnstrmEventInfo->rtrIfNum;
  dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum];

  /* Get Override interval + Propogation interval of the Interface API */
  lpdTimeout = pimdmIntfLanPropagationDelayGet (pimdmCB, rtrIfNum);
  if(lpdTimeout == -1)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to get LPD Delay on rtrIfNum %d",
                dnstrmEventInfo->rtrIfNum);
    return L7_FAILURE;
  }
  oiTimeout = pimdmIntfOverrideIntervalGet(pimdmCB, rtrIfNum);
  if(oiTimeout == -1)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to get Override Interval on rtrIfNum %d", rtrIfNum);
    return L7_FAILURE;
  }
  /* pruneholdtime - JP interval of the interface */
  /*Convert lpdtimeout and oiTimeout to seconds and floor the value */
  timeout = dnstrmEventInfo->holdTime - ((lpdTimeout + oiTimeout)/1000);

  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PT (S,G,I) Set Failed for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 rtrIfNum);
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PT (S,G,I) Set Success for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 rtrIfNum);

  if (pimdmUtilAppTimerSet (pimdmCB, pimdmDnstrmPruneTimerExpiryHandler,
                            (void*) dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam,
                            timeout, errMsgBuf, sucMsgBuf,
                            &dnstrmIntfInfo->pruneTimer,
                            "DM-DnPT2")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Resetting Prune Timer Failed on rtrIfNum %d", rtrIfNum);
    return L7_FAILURE;
  }

  /* Send Prune Echo if more than one neighbor */
  if(pimdmIntfNumNeighborsGet(pimdmCB, rtrIfNum) > 1)
  {
    /*Fill the interface address as the upstream neighbor address */
    if(pimdmIntfIpAddressGet (pimdmCB, rtrIfNum, &myAddress) != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Failed to get Ip address on rtrIfNum %d",
                  rtrIfNum);
      return L7_FAILURE;
    }  

    if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_PRUNE,
                                        &myAddress, &pimdmCB->allRoutersAddr,
                                        rtrIfNum, 0, 0, 0)
                                     != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to send PruneEcho on rtrIfNum %d",
                   rtrIfNum);
    }
  }

  /* Evaluate the prunemask Change */
  pimdmMrtDnstrmPruneMaskEval (mrtEntry);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Do Nothing 
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/
static 
L7_RC_t  pimdmDnstrmFsmActionDoNothing(pimdmMrtEntry_t* mrtEntry,
                                       pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Entry");
  
  PIMDM_UNUSED_PARAM (mrtEntry);
  PIMDM_UNUSED_PARAM (dnstrmEventInfo);
  
  PIMDM_TRACE (PIMDM_DEBUG_FSM_DNSTRM, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Action Error
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          dnstrmEventInfo  @b{ (input) } Downstream info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/
static 
L7_RC_t pimdmDnstrmFsmActionError(pimdmMrtEntry_t* mrtEntry,
                                  pimdmDnstrmEventInfo_t *dnstrmEventInfo)
{
  PIMDM_UNUSED_PARAM (mrtEntry);
  PIMDM_UNUSED_PARAM (dnstrmEventInfo);

  return L7_FAILURE;
}

