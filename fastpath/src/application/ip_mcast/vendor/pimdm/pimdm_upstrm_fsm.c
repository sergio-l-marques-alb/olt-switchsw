/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_upstrm_fsm.c
*
* @purpose    PIM-DM Upstream Interface State machine Action routines
*
* @component  PIM-DM
*
* @comments   none
*
* @create     05-Aug-05
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
#include "pimdm_upstrm_fsm.h"
#include "pimdm_debug.h"
#include "pimdm_intf.h"
#include "pimdm_util.h"
#include "mfc_rxtx.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/

static L7_RC_t
pimdmUpstrmFsmActionSendPrnSetPLT (pimdmMrtEntry_t* mrtEntry,
                                   pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionCancelOT (pimdmMrtEntry_t* mrtEntry,
                              pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionSetOT (pimdmMrtEntry_t* mrtEntry,
                           pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionSendJoin (pimdmMrtEntry_t* mrtEntry,
                              pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionResetPLT (pimdmMrtEntry_t* mrtEntry,
                              pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionSendPruneResetPLT (pimdmMrtEntry_t* mrtEntry,
                              pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionCancelGRT (pimdmMrtEntry_t* mrtEntry,
                               pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionSendPrnSetPLTCancelGRT
                                      (pimdmMrtEntry_t* mrtEntry,
                                       pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionCancelPLTSendGraftSetGRT (pimdmMrtEntry_t* mrtEntry,
                                     pimdmUpstrmEventInfo_t *upstrmEventInfo);
static L7_RC_t
pimdmUpstrmFsmActionSendGraftSetGRT (pimdmMrtEntry_t* mrtEntry,
                                     pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionCancelPLT (pimdmMrtEntry_t* mrtEntry,
                               pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionCleanup (pimdmMrtEntry_t* mrtEntry,
                               pimdmUpstrmEventInfo_t *upstrmEventInfo);


static L7_RC_t
pimdmUpstrmFsmActionDoNothing (pimdmMrtEntry_t* mrtEntry,
                               pimdmUpstrmEventInfo_t *upstrmEventInfo);

static L7_RC_t
pimdmUpstrmFsmActionError (pimdmMrtEntry_t* mrtEntry,
                           pimdmUpstrmEventInfo_t *upstrmEventInfo);

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/

/*********************************************************************
*       Upstream Interface State machine State table definition
*********************************************************************/
static pimdmUpstrmFsmState_t
pimdmUpstrmFsmState[PIMDM_UPSTRM_MAX_EVENTS][PIMDM_UPSTRM_MAX_STATES] =
{
  /* EVENT: Data packet arrives on RPF_Interface(S) AND olist(S,G) == NULL AND
   * PLT(S,G) not running.
   */
  {{PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionSendPrnSetPLT},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionSendPrnSetPLT},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionError}
  },

  /* EVENT: State Refresh(S,G) received from RPF‘(S) AND
   * Prune Indicator == 1.
   */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionSetOT},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionResetPLT},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionSetOT}
  },

  /* EVENT: State Refresh(S,G) received from RPF‘(S) AND
   * Prune Indicator == 0 AND
   * PLT(S,G) not running.
   */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionSendPrnSetPLT},
   {PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionCancelGRT}
  },

  /* EVENT: See Join(S,G) to RPF’(S).
   */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionCancelOT},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionCancelOT}
  },

  /* EVENT: See Prune(S,G).
   */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionSetOT},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionSetOT}
  },

  /* EVENT: OT(S,G) Expires.
   */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionSendJoin},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionError},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionSendJoin}
  },

  /* EVENT: olist(S,G)->NULL.
   */
  {{PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionSendPrnSetPLT},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionError},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionSendPrnSetPLTCancelGRT},
  },

  /* EVENT: olist(S,G)->non-NULL.
   */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionError},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionCancelPLTSendGraftSetGRT},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionError}
  },

  /* EVENT: RPF’(S) Changes AND
   * olist(S,G) != NULL.
   */
  {{PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionSendGraftSetGRT},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionCancelPLTSendGraftSetGRT},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionSendGraftSetGRT}
  },

  /* EVENT: RPF’(S) Changes AND
   * olist(S,G) == NULL.
   */
  {{PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionCancelPLT},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionCancelGRT}
  },

  /* EVENT: S becomes directly connected.
   */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionCancelGRT}
  },

  /* EVENT: GRT(S,G) Expires.
   */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionError},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionError},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionSendGraftSetGRT}
  },

  /* EVENT: Receive GraftAck(S,G) from RPF’(S). */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionCancelGRT}
  },

  /* Event: PIMDM_UPSTRM_EVT_RPF_INTF_CHANGE */
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionCleanup},
   {PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionCleanup},
   {PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionCleanup}
  } ,
  /* EVENT:PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_RUNNING*/
  {{PIMDM_UPSTRM_STATE_FORWD, pimdmUpstrmFsmActionDoNothing},
   {PIMDM_UPSTRM_STATE_PRUNE, pimdmUpstrmFsmActionSendPruneResetPLT},
   {PIMDM_UPSTRM_STATE_ACKPN, pimdmUpstrmFsmActionDoNothing}
  },
};

/*********************************************************************
*            Upstream Interface State machine Events List
*   (This array is indexed based on the PIMDM_UPSTRM_EVENT_t enum)
*********************************************************************/
static L7_uchar8* pimdmUpstrmFsmEventList[] =
{
  "PIMDM_UPSTRM_EVT_DATA_ARVL_OIF_NULL_PLT_NOTRUN",
  "PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_TRUE",
  "PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_NR",
  "PIMDM_UPSTRM_EVT_JOIN_ARVL_RPF_SRC",
  "PIMDM_UPSTRM_EVT_PRUNE_ARVL",
  "PIMDM_UPSTRM_EVT_OT_EXPIRY",
  "PIMDM_UPSTRM_EVT_OIF_NULL",
  "PIMDM_UPSTRM_EVT_OIF_NOT_NULL",
  "PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NOT_NULL",
  "PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NULL",
  "PIMDM_UPSTRM_EVT_SRC_DIR_CONN",
  "PIMDM_UPSTRM_EVT_GRT_EXPIRY",
  "PIMDM_UPSTRM_EVT_GRFT_ACK_ARVL",
  "PIMDM_UPSTRM_EVT_RPF_INTF_CHANGE",
  "PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_RUNNING"
};

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Upstream State machine Action routine invoker
*
* @param   upstrmEvent      @b{ (input) } Type of the event
*          mrtEntry         @b{ (input) } Pointer to MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVENT_t upstrmEvent,
                               pimdmMrtEntry_t *mrtEntry,
                               pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  PIMDM_UPSTRM_STATE_t currentState;
  pimdmUpstrmFsmState_t *upstrmFsmState = L7_NULLPTR;
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *grpAddr = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_FSM_UPSTRM, "Entry");

  if ((mrtEntry == L7_NULLPTR) || (upstrmEvent >= PIMDM_UPSTRM_MAX_EVENTS) ||
      (upstrmEventInfo == L7_NULLPTR))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Upstream FSM State Invalid parameters "
                 "for Event [%s]", pimdmUpstrmFsmEventList[upstrmEvent]);
    return L7_FAILURE;
  }

  rtrIfNum = upstrmEventInfo->rtrIfNum;
  srcAddr = &mrtEntry->srcAddr;
  grpAddr = &mrtEntry->grpAddr;

  /* Get the current State and the State table's next State info.
   */
  currentState = mrtEntry->upstrmGraftPruneStateInfo.grfPrnState;
  upstrmFsmState = &(pimdmUpstrmFsmState[upstrmEvent][currentState]);

  /* Do not act on Stale entries */
  if (mrtEntry->upstrmRtrIfNum == 0)
  {
    mrtEntry->upstrmGraftPruneStateInfo.grfPrnState = upstrmFsmState->nextState;
    PIMDM_TRACE (PIMDM_DEBUG_FSM_UPSTRM, "Upstrm FSM State/Action Transition, "
                 "No action Taken as Upstream Interface is ZERO, But Transitioning "
                 "to Next State ... \n"
                 "for S-%s, G-%s\n Curr State: [%d] ... Event: [%s] ... Next State: [%d]",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 currentState, pimdmUpstrmFsmEventList[upstrmEvent],
                 upstrmFsmState->nextState);
    return L7_SUCCESS;
  }

  /* If Source is Directly Connected, no need to execute the Action.
   * Just leave as is in the Default State.
   */
  if (mrtEntry->sourceDirectlyConnected == L7_TRUE)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FSM_UPSTRM, "Upstrm FSM State/Action Transition, "
                 "No action Taken as Source is Directly Connected ... \n"
                 "for S-%s, G-%s\n Curr State: [%d] ... Event: [%s]",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 currentState, pimdmUpstrmFsmEventList[upstrmEvent]);
    return L7_SUCCESS;
  }

  /* Execute the Action Routine pointed by the current State
   * and event.
   */
  if ((upstrmFsmState->fsmAction (mrtEntry, upstrmEventInfo)) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FSM_UPSTRM, "Upstrm FSM State Transition Failed "
                 "for S-%s, G-%s\n State: [%d] ... Event [%s]",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 currentState, pimdmUpstrmFsmEventList[upstrmEvent]);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_UPSTRM, "Upstrm FSM State Transition Successful "
               "for S-%s, G-%s\n Curr State: [%d] ... Event: [%s] ... Next State: [%d]",
               inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
               currentState, pimdmUpstrmFsmEventList[upstrmEvent],
               upstrmFsmState->nextState);

  /* Update the current State with that of the next State from the state table.
   */
  mrtEntry->upstrmGraftPruneStateInfo.grfPrnState = upstrmFsmState->nextState;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_UPSTRM, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  MRT Entry's Override timer expiry Handler
*
* @param    mrtEntryUpstrmTmrHndlParam  @b{ (input) } Handle to get the MRT Entry.
*
* @returns  None
*
* @comments
*
* @end
*********************************************************************/

static void pimdmUpstrmOverrideTimerExpiryHandler (void *mrtEntryUpstrmTmrHndlParam)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

  if ((mrtEntry  = (pimdmMrtEntry_t*)
                   handleListNodeRetrieve ((L7_uint32)mrtEntryUpstrmTmrHndlParam))
                == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  if (mrtEntry->upstrmGraftPruneStateInfo.overrideTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM overrideTimer (OT) is NULL, But Still Expired");
    return;
  }
  mrtEntry->upstrmGraftPruneStateInfo.overrideTimer = L7_NULLPTR;

  /*************** UPSTREAM INTERFACE FSM EVENTS *********************/
  /* Prepare the Upstream Event Information.
   */
  upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

  /* EVENT: OT(S,G) Expires.
   */
  pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_OT_EXPIRY, mrtEntry,
                         &upstrmFsmEventInfo);

  return;
}


/*********************************************************************
*
* @purpose  MRT Entry's Graft Retry timer expiry Handler
*
* @param    mrtEntryUpstrmTmrHndlParam  @b{ (input) } Handle to get the MRT Entry.
*
* @returns  None
*
* @comments
*
* @end
*********************************************************************/

static void pimdmUpstrmGraftRetryTimerExpiryHandler (void *mrtEntryUpstrmTmrHndlParam)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

  if ((mrtEntry  = (pimdmMrtEntry_t*)
                   handleListNodeRetrieve ((L7_uint32)mrtEntryUpstrmTmrHndlParam))
                == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  if (mrtEntry->upstrmGraftPruneStateInfo.grftRetryTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM grftRetryTimer (GRT) is NULL, But Still Expired");
    return;
  }
  mrtEntry->upstrmGraftPruneStateInfo.grftRetryTimer = L7_NULLPTR;

  /*************** UPSTREAM INTERFACE FSM EVENTS *********************/
  /* Prepare the Upstream Event Information.
   */
  upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

  /* EVENT: GRT(S,G) Expires.
   */
  pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_GRT_EXPIRY, mrtEntry,
                         &upstrmFsmEventInfo);

  return;
}

/*********************************************************************
*
* @purpose  MRT Entry's Prune Limit timer expiry Handler
*
* @param    mrtEntryUpstrmTmrHndlParam  @b{ (input) } Handle to get the MRT Entry.
*
* @returns  None
*
* @comments
*
* @end
*********************************************************************/

static void pimdmUpstrmPruneLimitTimerExpiryHandler (void *mrtEntryUpstrmTmrHndlParam)
{
  pimdmMrtEntry_t *mrtEntry =L7_NULLPTR;
  L7_BOOL isEmpty = L7_FALSE;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;

  if ((mrtEntry  = (pimdmMrtEntry_t*)
                   handleListNodeRetrieve ((L7_uint32)mrtEntryUpstrmTmrHndlParam))
                == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  if (mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM prnLmtTimer (PLT) is NULL, But Still Expired");
    return;
  }
  mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer = L7_NULLPTR;

  /* NOTE: This is implementation specific.
   * Once the PLT expires and if the OIF list is NULL, to optimize the
   * network bandwidth, Send a Prune and Reset the PLT(S,G).
   * If a new Host or Router joins downstream, the Source Tree can be
   * re-built with a Graft/Graft-Ack.
   */
  BITX_IS_EMPTY (&mrtEntry->oifList, isEmpty);
  if (isEmpty == L7_TRUE)
  {
    /*************** UPSTREAM INTERFACE FSM EVENTS *********************/
    /* Prepare the Upstream Event Information.
     */
    upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

    /* EVENT: Data packet arrives on RPF_Interface(S) AND olist(S,G) == NULL AND
     * PLT(S,G) not running.
     */
    pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_DATA_ARVL_OIF_NULL_PLT_NOTRUN,
                           mrtEntry, &upstrmFsmEventInfo);
  }

  return;
}

/*********************************************************************
*
* @purpose  MRT Entry's Upstream Assert timer expiry Handler
*
* @param    mrtEntryUpstrmTmrHndlParam  @b{ (input) } Handle to get the MRT Entry.
*
* @returns  None
*
* @comments
*
* @end
*********************************************************************/

void
pimdmUpstreamAssertTimerExpiryHandler (void *mrtEntryUpstrmTmrHndlParam)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmAssertInfo_t *upstrmNbrInfo = L7_NULLPTR;
  pimdmUpstrmEventInfo_t upstrmFsmEventInfo;
  L7_BOOL isNull = L7_FALSE;

  if ((mrtEntry  = (pimdmMrtEntry_t*)
                   handleListNodeRetrieve ((L7_uint32)mrtEntryUpstrmTmrHndlParam))
                == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  upstrmNbrInfo = &(mrtEntry->upstrmNbrInfo);

  if (upstrmNbrInfo->assertTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM Upstream assertTimer (AT) is NULL, But Still Expired");
    return;
  }
  upstrmNbrInfo->assertTimer = L7_NULLPTR;

  /* Assert Winner's Timer has expired.  Restore the RPF(S) Assert Info to the
   * Upstream Assert Info.
   */
  inetCopy (&(upstrmNbrInfo->assertWnrAddr), &(mrtEntry->rpfInfo.rpfNextHop));
  upstrmNbrInfo->assertWnrAsrtPref = mrtEntry->rpfInfo.rpfRouteMetricPref;
  upstrmNbrInfo->assertWnrAsrtMetric = mrtEntry->rpfInfo.rpfRouteMetric;

  /*************** UPSTREAM FSM EVENTS *********************/

  /* Prepare the Upstream Event Information.
   */
  upstrmFsmEventInfo.rtrIfNum = mrtEntry->upstrmRtrIfNum;

  BITX_IS_EMPTY (&mrtEntry->oifList, isNull);
  if (isNull == L7_FALSE)
  {
    /* EVENT - RPF'(S) Changes AND olist(S,G) != NULL.
     */
    pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NOT_NULL, mrtEntry,
                           &upstrmFsmEventInfo);
  }
  else
  {
    /* EVENT - RPF'(S) Changes AND olist(S,G) == NULL.
     */
    pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NULL, mrtEntry,
                           &upstrmFsmEventInfo);
  }

  return;
}


/*********************************************************************
*
* @purpose Send Prune and Start the PLT Timer.
*
* @param   mrtEntry         @b{ (input) } Pointer to MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

static
L7_RC_t pimdmUpstrmFsmActionSendPrnSetPLT(
                                       pimdmMrtEntry_t* mrtEntry,
                                       pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");
  /* Send the Prune */
  if (pimdmMrtEntryControlPacketSend (mrtEntry,PIMDM_CTRL_PKT_PRUNE,
                                      &mrtEntry->upstrmNbrInfo.assertWnrAddr,
                                      &mrtEntry->pimdmCB->allRoutersAddr,
                                      mrtEntry->upstrmRtrIfNum,
                                      PIMDM_MRT_ENTRY_HOLDTIME, 0, 0, L7_FALSE)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to send Prune Packet on upstream index %d\n",
                mrtEntry->upstrmRtrIfNum);
  }

  /* Set the PLT Timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PLT (S,G) Set Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PLT (S,G) Set Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmUpstrmPruneLimitTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_PRUNE_LIMIT_TIMER_TIMEOUT,
                            errMsgBuf, sucMsgBuf,
                            &mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer,
                            "DM-PLT")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to set Prune Limit Timer");
    return L7_FAILURE;
  }

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Cancel the override timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmUpstrmFsmActionCancelOT (pimdmMrtEntry_t* mrtEntry,
                                      pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");

  /* cancel the override timer */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
                          &(mrtEntry->upstrmGraftPruneStateInfo.overrideTimer));

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose set the override timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmUpstrmFsmActionSetOT (pimdmMrtEntry_t* mrtEntry,
                                   pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  L7_int32 timeout = -1;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");
  /* Call the interface API to get the random(0,OverideInterval) */

  if ((timeout = pimdmIntfOverrideIntervalGet (mrtEntry->pimdmCB,
                                               upstrmEventInfo->rtrIfNum))
                                            == -1)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
               "Failed to get the override interval on rtrIfNum %d",
               upstrmEventInfo->rtrIfNum);
    return L7_FAILURE;
  }

  /*Convert the timeout in seconds and floor the value */
  timeout = timeout/1000;
  timeout = pimdmUtilRandomGet (0, (L7_uint32 )timeout);
  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"timeout is %d",timeout);

  /* If OT is not running set the OT */
  if (mrtEntry->upstrmGraftPruneStateInfo.overrideTimer == L7_NULLPTR)
  {
    /*Set the override timer */
    osapiSnprintf(errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                  "OT (S,G) Set Failed for Src - %s Grp - %s ",
                  inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                   "OT (S,G) Set Success for Src - %s Grp - %s ",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

    if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                              pimdmUpstrmOverrideTimerExpiryHandler,
                              (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                              timeout, errMsgBuf, sucMsgBuf,
                              &mrtEntry->upstrmGraftPruneStateInfo.overrideTimer,
                              "DM-OT")
                           != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to set Override Timer");
      return L7_FAILURE;
    }
  }

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose send join to the upstream neighbor
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmUpstrmFsmActionSendJoin(pimdmMrtEntry_t* mrtEntry,
                                     pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM, "Entry");

  if (mrtEntry->sourceDirectlyConnected != L7_TRUE)
  {
    /* Send the Join */
    if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_JOIN,
                                        &mrtEntry->upstrmNbrInfo.assertWnrAddr,
                                        &mrtEntry->pimdmCB->allRoutersAddr,
                                        mrtEntry->upstrmRtrIfNum, 0, 0, 0, L7_FALSE)
                                     != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to send Join Packet on upstream index %d\n",
                mrtEntry->upstrmRtrIfNum);
    }
  }

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM, "Exit");
  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Reset the PLT Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmUpstrmFsmActionResetPLT (pimdmMrtEntry_t* mrtEntry,
                                      pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");

  /* Reset the PLT Timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PLT (S,G) ReSet Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "PLT (S,G) ReSet Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmUpstrmPruneLimitTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_PRUNE_LIMIT_TIMER_TIMEOUT,
                            errMsgBuf, sucMsgBuf,
                            &mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer,
                            "DM-PLT2")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to reset Prune Limit Timer");
    return L7_FAILURE;
  }

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Send Prune.Reset the PLT Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmUpstrmFsmActionSendPruneResetPLT (pimdmMrtEntry_t* mrtEntry,
                                      pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_UPSTRM, "Entry");

  if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_PRUNE,
                                      &mrtEntry->upstrmNbrInfo.assertWnrAddr,
                                      &mrtEntry->pimdmCB->allRoutersAddr,
                                      mrtEntry->upstrmRtrIfNum,
                                      PIMDM_MRT_ENTRY_HOLDTIME, 0, 0, L7_FALSE)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to send Prune Packet on upstream index %d\n",
                 mrtEntry->upstrmRtrIfNum);
  }

  /* Reset the PLT Timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PLT (S,G) ReSet Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "PLT (S,G) ReSet Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmUpstrmPruneLimitTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_PRUNE_LIMIT_TIMER_TIMEOUT,
                            errMsgBuf, sucMsgBuf,
                            &mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer,
                            "DM-PLT3")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE, "Failed to reset Prune Limit Timer");
    return L7_FAILURE;
  }

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM, "Exit");
  return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose Cancel the Graft Retry Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

static
L7_RC_t pimdmUpstrmFsmActionCancelGRT(pimdmMrtEntry_t* mrtEntry,
                                      pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM, "Entry");

  /* cancel the GRT Timer */
  pimdmUtilAppTimerCancel(mrtEntry->pimdmCB,
                         &(mrtEntry->upstrmGraftPruneStateInfo.grftRetryTimer));

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Send Prune + Set PLT Timer + Cancel the Graft Retry Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

static
L7_RC_t pimdmUpstrmFsmActionSendPrnSetPLTCancelGRT
                                    (pimdmMrtEntry_t* mrtEntry,
                                     pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");

  /* Send the prune */
  if (pimdmMrtEntryControlPacketSend (mrtEntry,PIMDM_CTRL_PKT_PRUNE,
                                      &mrtEntry->upstrmNbrInfo.assertWnrAddr,
                                      &mrtEntry->pimdmCB->allRoutersAddr,
                                      mrtEntry->upstrmRtrIfNum,
                                      PIMDM_MRT_ENTRY_HOLDTIME, 0, 0, L7_FALSE)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to send Prune Packet on upstream index %d\n",
                mrtEntry->upstrmRtrIfNum);
  }

  /* Set the PLT Timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PLT (S,G) Set Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "PLT (S,G) Set Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmUpstrmPruneLimitTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_PRUNE_LIMIT_TIMER_TIMEOUT,
                            errMsgBuf, sucMsgBuf,
                            &mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer,
                            "DM-PLT4")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to set Prune Limit Timer");
    return L7_FAILURE;
  }

  /* Cancel the GRT Timer */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
                           &(mrtEntry->upstrmGraftPruneStateInfo.grftRetryTimer));

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Cancel PLT + Send Graft + Set the Graft Retry Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

static L7_RC_t pimdmUpstrmFsmActionCancelPLTSendGraftSetGRT (
                                     pimdmMrtEntry_t* mrtEntry,
                                     pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");

  /*Cancel the PLT */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
                           &(mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer));

  /* Send the Graft */
  if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_GRAFT,
                                      &mrtEntry->upstrmNbrInfo.assertWnrAddr,
                                      &mrtEntry->upstrmNbrInfo.assertWnrAddr,
                                      mrtEntry->upstrmRtrIfNum, 0, 0, 0, L7_FALSE)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to send Graft Packet to address %s",
                inetAddrPrint(srcAddr,src));
  }

  /* Set the Graft timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "GRT (S,G) Set Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "GRT (S,G) Set Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmUpstrmGraftRetryTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_GRAFT_RETRY_TIMER_TIMEOUT, errMsgBuf,
                            sucMsgBuf,
                            &mrtEntry->upstrmGraftPruneStateInfo.grftRetryTimer,
                            "DM-GRT")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to set Graft Timer");
    return L7_FAILURE;
  }

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return (L7_SUCCESS);
}
/*********************************************************************
*
* @purpose Send Graft + Set the Graft Retry Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

static L7_RC_t pimdmUpstrmFsmActionSendGraftSetGRT(
                                     pimdmMrtEntry_t* mrtEntry,
                                     pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");

  /* Send the Graft */
  if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_GRAFT,
                                      &mrtEntry->upstrmNbrInfo.assertWnrAddr,
                                      &mrtEntry->upstrmNbrInfo.assertWnrAddr,
                                      mrtEntry->upstrmRtrIfNum, 0, 0, 0, L7_FALSE)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to send Graft Packet to address %s",
                inetAddrPrint(srcAddr,src));
  }

  /* Set the Graft timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "GRT (S,G) Set Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "GRT (S,G) Set Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmUpstrmGraftRetryTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_GRAFT_RETRY_TIMER_TIMEOUT, errMsgBuf,
                            sucMsgBuf,
                            &mrtEntry->upstrmGraftPruneStateInfo.grftRetryTimer,
                            "DM-GRT2")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to set Graft Timer");
    return L7_FAILURE;
  }

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Cancel Prune Limit Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

static
L7_RC_t pimdmUpstrmFsmActionCancelPLT(pimdmMrtEntry_t* mrtEntry,
                                      pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");

  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
                           &(mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer));

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose Cancel All timers
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

static
L7_RC_t pimdmUpstrmFsmActionCleanup(pimdmMrtEntry_t* mrtEntry,
                                    pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");
  /*Cancel Graft Retry Timer*/
  pimdmUtilAppTimerCancel(mrtEntry->pimdmCB,
                          &(mrtEntry->upstrmGraftPruneStateInfo.grftRetryTimer));

  /*Cancel Prune Limit Timer */
  pimdmUtilAppTimerCancel(mrtEntry->pimdmCB,
                          &(mrtEntry->upstrmGraftPruneStateInfo.prnLmtTimer));

  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Exit");
  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Do Nothing
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

static
L7_RC_t pimdmUpstrmFsmActionDoNothing(pimdmMrtEntry_t* mrtEntry,
                                      pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  PIMDM_TRACE(PIMDM_DEBUG_FSM_UPSTRM,"Entry");
  PIMDM_UNUSED_PARAM (mrtEntry);
  PIMDM_UNUSED_PARAM (upstrmEventInfo);

  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Error Action
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          upstrmEventInfo  @b{ (input) } Upstream info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/

static
L7_RC_t pimdmUpstrmFsmActionError(pimdmMrtEntry_t* mrtEntry,
                                  pimdmUpstrmEventInfo_t *upstrmEventInfo)
{
  PIMDM_UNUSED_PARAM (mrtEntry);
  PIMDM_UNUSED_PARAM (upstrmEventInfo);

  return (L7_FAILURE);
}
