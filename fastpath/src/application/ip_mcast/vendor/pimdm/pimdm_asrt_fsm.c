/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_asrt_fsm.c
*
* @purpose    PIM-DM Assert State machine Action routines
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
#include "pimdm_asrt_fsm.h"
#include "pimdm_dnstrm_fsm.h"
#include "pimdm_debug.h"
#include "pimdm_util.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/

static L7_RC_t
pimdmAssertFsmActionSendAsrtSetAT (pimdmMrtEntry_t* mrtEntry,
                                   pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionCancelAT (pimdmMrtEntry_t* mrtEntry,
                              pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionCancelATSendAssertCancel (pimdmMrtEntry_t* mrtEntry,
                                       pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionSendPrnSetAT (pimdmMrtEntry_t* mrtEntry,
                                  pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionSetAT (pimdmMrtEntry_t* mrtEntry,
                           pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionResetAT (pimdmMrtEntry_t* mrtEntry,
                             pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionSendAssert (pimdmMrtEntry_t* mrtEntry,
                                pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionResetAssertInfo (pimdmMrtEntry_t* mrtEntry,
                                     pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionSendPrune (pimdmMrtEntry_t* mrtEntry,
                               pimdmAssertEventInfo_t *assertEventInfo);
static L7_RC_t
pimdmAssertFsmActionDoNothing (pimdmMrtEntry_t* mrtEntry,
                               pimdmAssertEventInfo_t *assertEventInfo);

static L7_RC_t
pimdmAssertFsmActionError (pimdmMrtEntry_t* mrtEntry,
                           pimdmAssertEventInfo_t *assertEventInfo);

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/

/*********************************************************************
*          Assert State machine State table definition
*********************************************************************/

static pimdmAssertFsmState_t
pimdmAssertFsmState[PIMDM_ASSERT_MAX_EVENTS][PIMDM_ASSERT_MAX_STATES] =
{
  /* Data packet arrives on Downstream interface for (S,G) */
  {{PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionSendAsrtSetAT},
   {PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionSendAsrtSetAT},
   {PIMDM_ASSERT_STATE_ASSERT_LOSER, pimdmAssertFsmActionSendPrune}
  },

  /* Receive Inferior Assert from Winner */
  {{PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionError},
   {PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionError},
   {PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionCancelAT}
  },

  /* Receive Inferior Assert from non-Assert Winner AND couldAssert is TRUE */
  {{PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionSendAsrtSetAT},
   {PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionSendAsrtSetAT},
   {PIMDM_ASSERT_STATE_ASSERT_LOSER, pimdmAssertFsmActionDoNothing}
  },

  /* Receive Preferred Assert */
  {{PIMDM_ASSERT_STATE_ASSERT_LOSER, pimdmAssertFsmActionSendPrnSetAT},
   {PIMDM_ASSERT_STATE_ASSERT_LOSER, pimdmAssertFsmActionSendPrnSetAT},
   {PIMDM_ASSERT_STATE_ASSERT_LOSER, pimdmAssertFsmActionSetAT}
  },

  /* Send State Refresh */
  {{PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionDoNothing},
   {PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionResetAT},
   {PIMDM_ASSERT_STATE_ASSERT_LOSER, pimdmAssertFsmActionError}
  },

  /* Assert Timer for (S,G) expires */
  {{PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionError},
   {PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionResetAssertInfo},
   {PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionResetAssertInfo}
  },

  /* couldAssert is FALSE */
  {{PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionDoNothing},
   {PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionCancelATSendAssertCancel},
   {PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionCancelAT}
  },

  /* couldAssert is TRUE */
  {{PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionDoNothing},
   {PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionError},
   {PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionCancelAT}
  },

  /* Winner's Neighbor Liveness Timer expires */
  {{PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionError},
   {PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionError},
   {PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionCancelAT}
  },

  /* Prune(S,G) OR Join(S,G) OR Graft(S,G) Packet arrival */
  {{PIMDM_ASSERT_STATE_NOINFO, pimdmAssertFsmActionDoNothing},
   {PIMDM_ASSERT_STATE_ASSERT_WINNER, pimdmAssertFsmActionDoNothing},
   {PIMDM_ASSERT_STATE_ASSERT_LOSER, pimdmAssertFsmActionSendAssert}
  }
};

/*********************************************************************
*             Assert State machine Events List
*   (This array is indexed based on the PIMDM_ASSERT_EVENT_t enum)
*********************************************************************/
static L7_uchar8* pimdmAssertFsmEventList[] =
{
  "PIMDM_ASSERT_EVT_DATA_ARVL_DNSTRM_IF",
  "PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_WNR",
  "PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_LSR_COULD_ASRT_TRUE",
  "PIMDM_ASSERT_EVT_PRF_ASRT_ARVL",
  "PIMDM_ASSERT_EVT_SEND_STATE_RFR",
  "PIMDM_ASSERT_EVT_AT_EXPIRY",
  "PIMDM_ASSERT_EVT_COULD_ASRT_FALSE",
  "PIMDM_ASSERT_EVT_COULD_ASRT_TRUE",
  "PIMDM_ASSERT_EVT_ASRT_WNR_NLT_EXPIRY",
  "PIMDM_ASSERT_EVT_PRN_JOIN_GRAFT_ARVL",
};

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Assert State machine Action routine invoker
*
* @param   assertEvent       @b{ (input) } Type of the event
*          mrtEntry          @b{ (input) } Pointer MRT Entry
*          assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t pimdmAssertFsmExecute (PIMDM_ASSERT_EVENT_t assertEvent,
                               pimdmMrtEntry_t* mrtEntry,
                               pimdmAssertEventInfo_t *assertEventInfo)
{
  PIMDM_ASSERT_STATE_t currentState;
  pimdmAssertFsmState_t *assertFsmState = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Input Validations */
  if ((mrtEntry == L7_NULLPTR) ||
      (assertEvent >= PIMDM_ASSERT_MAX_EVENTS) ||
      (assertEventInfo == L7_NULLPTR))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Assert FSM invoked with invalid parameters");
    return L7_FAILURE;
  }

  rtrIfNum = assertEventInfo->rtrIfNum;
  if ((rtrIfNum < 1) || (rtrIfNum >= PIMDM_MAX_INTERFACES))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "rtrIfNum - [%d] Out of Bounds !!! for Event [%s]",
                 rtrIfNum, pimdmAssertFsmEventList[assertEvent]);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,L7_FLEX_PIMDM_MAP_COMPONENT_ID, "PIMDM FSM Action Invoke Failed; rtrIfNum"
            " - [%d] Out of Bounds for Event [%s]. The PIMDM FSM Action invocation Failed due to invalid Routing"
            " interface number. In such cases, the FSM Action routine can never be invoked which may result in "
            "abnormal behavior. The failed FSM-name can be identified from the specified Event name.",
            rtrIfNum, pimdmAssertFsmEventList[assertEvent]);
    return L7_FAILURE;
  }

  if (mrtEntry->downstrmStateInfo[rtrIfNum] == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Downstrm Intf - %d", rtrIfNum);
    return L7_FAILURE;
  }

  asrtIntfInfo = &mrtEntry->downstrmStateInfo[rtrIfNum]->downstrmAssertInfo;

  /* Get the current State and the State table's next State info.*/
  currentState = asrtIntfInfo->assertState;
  assertFsmState = &(pimdmAssertFsmState[assertEvent][currentState]);

 /* Update the current State with that of the next State from the state table.
  */
  asrtIntfInfo->assertState = assertFsmState->nextState;

  /* Execute the Action Routine pointed by the current State and event. */
  if ((assertFsmState->fsmAction (mrtEntry, assertEventInfo)) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FSM_ASSERT, "Assert FSM State Transition Failed "
                 "for S-%s, G-%s\n "
                 "State: [%d] ... Event [%s]",
                 inetAddrPrint(&mrtEntry->srcAddr,src),
                 inetAddrPrint(&mrtEntry->grpAddr,grp),
                 currentState, pimdmAssertFsmEventList[assertEvent]);
    /* Since the Action has failed, rollback to the previous State.
     */
    asrtIntfInfo->assertState = currentState;
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_ASSERT, "Assert FSM State Transition Successful "
               "for S-%s, G-%s\n "
               "Curr State: [%d] ... Event: [%s] ... Next State: [%d]",
               inetAddrPrint(&mrtEntry->srcAddr,src),
               inetAddrPrint(&mrtEntry->grpAddr,grp),
               currentState, pimdmAssertFsmEventList[assertEvent],
               assertFsmState->nextState);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  MRT Entry's Downstream interface Assert Expiry Handler
*
* @param    mrtEntryDnstrmTmrHndlParam  @b{ (input) } Handle to get the MRT's Downstream entry info
*
* @returns  None
*
* @comments
*
* @end
*********************************************************************/
static void pimdmDnstrmAssertTimerExpiryHandler (void *mrtEntryDnstrmTmrHndlParam)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertEventInfo_t asrtFsmEventInfo;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if ((dnstrmIntfInfo = (pimdmDownstrmIntfInfo_t*)
                         handleListNodeRetrieve ((L7_uint32)mrtEntryDnstrmTmrHndlParam))
                      == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  if ((mrtEntry = dnstrmIntfInfo->mrtEntry) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Failed to retrive MRT entry");
    return;
  }

  pimdmCB = mrtEntry->pimdmCB;

  asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;

  if (asrtIntfInfo->assertTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMDM_MAP_COMPONENT_ID,
            "PIMDM Downstream assertTimer (AT) is NULL, But Still Expired");
    return;
  }
  asrtIntfInfo->assertTimer = L7_NULLPTR;

  /*************** ASSERT FSM EVENTS *********************/

  /* Prepare the Assert Event Information.
   */
  memset(&asrtFsmEventInfo,0,sizeof(pimdmAssertEventInfo_t));
  asrtFsmEventInfo.rtrIfNum = dnstrmIntfInfo->rtrIfNum;
  asrtFsmEventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
  inetAddressZeroSet (pimdmCB->addrFamily, &(asrtFsmEventInfo.assertAddr));

  inetAddressZeroSet (pimdmCB->addrFamily, &asrtIntfInfo->assertWnrAddr);
  asrtIntfInfo->assertWnrAsrtMetric = PIMDM_ASSERT_METRIC_INFINITY;
  asrtIntfInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;

  /* Evaluate any possible changes in the upstream */
  pimdmMrtDnstrmAssertMaskEval (dnstrmIntfInfo->mrtEntry,
                                dnstrmIntfInfo->rtrIfNum, L7_FALSE);

  /* EVENT - Assert Timer for (S,G) expires.
   */
  pimdmAssertFsmExecute (PIMDM_ASSERT_EVT_AT_EXPIRY, dnstrmIntfInfo->mrtEntry,
                         &asrtFsmEventInfo);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return;
}

/*********************************************************************
*
* @purpose  Send Assert Message + Set Assert Timer
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to AssertInfo
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmAssertFsmActionSendAsrtSetAT (pimdmMrtEntry_t* mrtEntry,
                                   pimdmAssertEventInfo_t *assertEventInfo)
{
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t ipAddress;
  L7_uint32 rtrIfNum = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  rtrIfNum = assertEventInfo->rtrIfNum;
  dnstrmIntfInfo = mrtEntry->downstrmStateInfo[rtrIfNum];
  asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;

  /* send the assert packet */
  if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_ASSERT,
                                      L7_NULLPTR,
                                      &mrtEntry->pimdmCB->allRoutersAddr,
                                      rtrIfNum, 0, 0, 0, L7_FALSE)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to send Assert Message on %d",
                 assertEventInfo->rtrIfNum);
  }

  /* assign the winner and the metrics to self */
  inetAddressZeroSet(mrtEntry->pimdmCB->addrFamily,&ipAddress);
  if (pimdmIntfIpAddressGet (mrtEntry->pimdmCB, rtrIfNum, &ipAddress)
                          != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
               "Failed to get Ip Address on rtrIfNum %d", rtrIfNum);
    return L7_FAILURE;
  }

  inetCopy(&asrtIntfInfo->assertWnrAddr,&ipAddress);
  asrtIntfInfo->assertWnrAsrtMetric = mrtEntry->rpfInfo.rpfRouteMetric;
  asrtIntfInfo->assertWnrAsrtPref = mrtEntry->rpfInfo.rpfRouteMetricPref;

  /* Set the Assert Timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "AT (S,G,I) Set Failed for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 rtrIfNum);
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "AT (S,G,I) Set Success for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 rtrIfNum);

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmDnstrmAssertTimerExpiryHandler,
                            (void*) dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam,
                            assertEventInfo->interval,
                            errMsgBuf, sucMsgBuf,
                            &asrtIntfInfo->assertTimer,
                            "DM-AT2")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to set Assert Timer on rtrIfNum %d", rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Cancel Assert Timer
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmAssertFsmActionCancelAT(pimdmMrtEntry_t* mrtEntry,
                                     pimdmAssertEventInfo_t *assertEventInfo)
{
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  asrtIntfInfo =
    &mrtEntry->downstrmStateInfo[assertEventInfo->rtrIfNum]->downstrmAssertInfo;

  /*cancel the assert timer */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB, &(asrtIntfInfo->assertTimer));

  /* reset the assert winner and metrics */
  inetAddressZeroSet(mrtEntry->pimdmCB->addrFamily,&asrtIntfInfo->assertWnrAddr);
  asrtIntfInfo->assertWnrAsrtMetric = PIMDM_ASSERT_METRIC_INFINITY;
  asrtIntfInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;

  /* Evaluate any possible changes in the upstream */
  pimdmMrtDnstrmAssertMaskEval (mrtEntry,assertEventInfo->rtrIfNum,L7_TRUE);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Cancel Assert Timer
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmAssertFsmActionCancelATSendAssertCancel (pimdmMrtEntry_t* mrtEntry,
                                        pimdmAssertEventInfo_t *assertEventInfo)
{
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  asrtIntfInfo =
    &mrtEntry->downstrmStateInfo[assertEventInfo->rtrIfNum]->downstrmAssertInfo;

  /* Cancel the Assert Timer */
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB, &(asrtIntfInfo->assertTimer));

  /* Reset the Assert Winner Info */
  inetAddressZeroSet(mrtEntry->pimdmCB->addrFamily,&asrtIntfInfo->assertWnrAddr);
  asrtIntfInfo->assertWnrAsrtMetric = PIMDM_ASSERT_METRIC_INFINITY;
  asrtIntfInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;

  /* Evaluate any possible changes in the upstream */
  pimdmMrtDnstrmAssertMaskEval (mrtEntry,assertEventInfo->rtrIfNum, L7_TRUE);

  /* Send the Assert Cancel Message */
  if (pimdmAssertMsgSend (mrtEntry->pimdmCB, &(mrtEntry->pimdmCB->allRoutersAddr),
                          assertEventInfo->rtrIfNum, &mrtEntry->srcAddr,
                          &mrtEntry->grpAddr, PIMDM_ASSERT_METRIC_INFINITY,
                          PIMDM_ASSERT_METRIC_INFINITY)
                       != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to send Assert Cancel Message for Src - %s, Grp - %s "
                 "on interface %d", inetAddrPrint (&(mrtEntry->srcAddr), src),
                 inetAddrPrint (&(mrtEntry->grpAddr), grp), assertEventInfo->rtrIfNum);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Send Prune + Set Assert timer
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmAssertFsmActionSendPrnSetAT (pimdmMrtEntry_t* mrtEntry,
                                       pimdmAssertEventInfo_t *assertEventInfo)
{
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  dnstrmIntfInfo = mrtEntry->downstrmStateInfo[assertEventInfo->rtrIfNum];
  asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;

  /* store the assert winner and details */
  inetCopy (&asrtIntfInfo->assertWnrAddr,&assertEventInfo->assertAddr);
  asrtIntfInfo->assertWnrAsrtMetric = assertEventInfo->assertMetric;
  asrtIntfInfo->assertWnrAsrtPref = assertEventInfo->assertPref;

  /* Set the Assert Timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "AT (S,G,I) Set Failed for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 assertEventInfo->rtrIfNum);
  osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "AT (S,G,I) Set Success for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 assertEventInfo->rtrIfNum);

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmDnstrmAssertTimerExpiryHandler,
                            (void*) dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam,
                            assertEventInfo->interval,
                            errMsgBuf, sucMsgBuf,
                            &asrtIntfInfo->assertTimer,
                            "DM-AT3")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to set Assert Timer on rtrIfNum %d",
                 assertEventInfo->rtrIfNum);
    return L7_FAILURE;
  }

  /* send the prune with holdtime = assert timer if couldAssert = true*/
  if(asrtIntfInfo->couldAssert == L7_TRUE)
  {
    if (pimdmMrtEntryControlPacketSend (mrtEntry,PIMDM_CTRL_PKT_PRUNE,
                                        &asrtIntfInfo->assertWnrAddr,
                                        &mrtEntry->pimdmCB->allRoutersAddr,
                                        assertEventInfo->rtrIfNum,
                                        PIMDM_DEFAULT_ASSERT_TIMEOUT, 0 ,0, L7_FALSE)
                                     != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Failed to send Prune on rtrIfNum %d",
                   assertEventInfo->rtrIfNum);
    }
  }

  /* Evaluate any possible changes in the upstream */
  pimdmMrtDnstrmAssertMaskEval (mrtEntry,assertEventInfo->rtrIfNum,L7_FALSE);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set Assert timer
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmAssertFsmActionSetAT(pimdmMrtEntry_t* mrtEntry,
                                  pimdmAssertEventInfo_t *assertEventInfo)
{
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  dnstrmIntfInfo = mrtEntry->downstrmStateInfo[assertEventInfo->rtrIfNum];
  asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;

  /* Set the Assert Timer */
  osapiSnprintf (errMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "AT (S,G,I) Set Failed for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 assertEventInfo->rtrIfNum);
  osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "AT (S,G,I) Set Success for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 assertEventInfo->rtrIfNum);

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmDnstrmAssertTimerExpiryHandler,
                            (void*) dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam,
                            assertEventInfo->interval,
                            errMsgBuf, sucMsgBuf,
                            &asrtIntfInfo->assertTimer,
                            "DM-AT4")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"Failed to set Assert Timer on rtrIfNum %d",
            assertEventInfo->rtrIfNum);
    return L7_FAILURE;
  }

  /* If recieved metric is better then store it */
  if (pimdmMrtAsrtMetricCompare(asrtIntfInfo->assertWnrAsrtPref,
                                asrtIntfInfo->assertWnrAsrtMetric,
                                &asrtIntfInfo->assertWnrAddr,
                                assertEventInfo->assertPref,
                                assertEventInfo->assertMetric,
                                &assertEventInfo->assertAddr)
                             == L7_FALSE)
  {
    inetCopy (&asrtIntfInfo->assertWnrAddr,&assertEventInfo->assertAddr);
    asrtIntfInfo->assertWnrAsrtMetric = assertEventInfo->assertMetric;
    asrtIntfInfo->assertWnrAsrtPref = assertEventInfo->assertPref;

    /* send the prune with holdtime = assert timer if couldAssert = true*/
    if(asrtIntfInfo->couldAssert == L7_TRUE)
    {
      if (pimdmMrtEntryControlPacketSend (mrtEntry,PIMDM_CTRL_PKT_PRUNE,
                                          &asrtIntfInfo->assertWnrAddr,
                                          &mrtEntry->pimdmCB->allRoutersAddr,
                                          assertEventInfo->rtrIfNum,
                                          PIMDM_DEFAULT_ASSERT_TIMEOUT, 0 ,0, L7_FALSE)
                                       != L7_SUCCESS)
      {
        PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Failed to send Prune on rtrIfNum %d",
                     assertEventInfo->rtrIfNum);
      }
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  ReSet Assert timer
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t  pimdmAssertFsmActionResetAT (pimdmMrtEntry_t* mrtEntry,
                                      pimdmAssertEventInfo_t *assertEventInfo)
{
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  dnstrmIntfInfo = mrtEntry->downstrmStateInfo[assertEventInfo->rtrIfNum];
  asrtIntfInfo = &dnstrmIntfInfo->downstrmAssertInfo;

  /* Reset the Assert Timer */
  osapiSnprintf (errMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "AT (S,G,I) ReSet Failed for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 assertEventInfo->rtrIfNum);
  osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "AT (S,G,I) ReSet Success for Src - %s Grp - %s Intf - %d ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp),
                 assertEventInfo->rtrIfNum);

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmDnstrmAssertTimerExpiryHandler,
                            (void*) dnstrmIntfInfo->mrtEntryDnstrmTmrHndlParam,
                            assertEventInfo->interval, errMsgBuf, sucMsgBuf,
                            &asrtIntfInfo->assertTimer,
                            "DM-AT5")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,
                 "Failed to set Assert Timer on rtrIfNum %d",
                 assertEventInfo->rtrIfNum);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Send Assert packet
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmAssertFsmActionSendAssert (pimdmMrtEntry_t* mrtEntry,
                                        pimdmAssertEventInfo_t *assertEventInfo)
{
  pimdmDnstrmEventInfo_t dnstrmFsmEventInfo;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Send the Assert Message */
  if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_ASSERT, L7_NULLPTR,
                                      &(mrtEntry->pimdmCB->allRoutersAddr),
                                      assertEventInfo->rtrIfNum, 0, 0, 0, L7_FALSE)
                                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to send Assert on rtrIfNum %d",
                 assertEventInfo->rtrIfNum);
  }

  /* if graft packet was recieved  then send graft ack */
  if (assertEventInfo->type == PIMDM_CTRL_PKT_GRAFT)
  {
    /*************** DOWNSTREAM INTERFACE FSM EVENTS *********************/

    /* Prepare the Downstream Event Information.
     */
    dnstrmFsmEventInfo.rtrIfNum = assertEventInfo->rtrIfNum;
    inetCopy (&(dnstrmFsmEventInfo.nbrAddr), &assertEventInfo->assertAddr);

    /* EVENT - Receive Graft (S,G).
     */
    pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVT_GRAFT_ARVL, mrtEntry,
                           &dnstrmFsmEventInfo);
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Reset Assert Winner Information
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to AssertInfo
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmAssertFsmActionResetAssertInfo (pimdmMrtEntry_t* mrtEntry,
                                     pimdmAssertEventInfo_t *assertEventInfo)
{
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  asrtIntfInfo =
    &mrtEntry->downstrmStateInfo[assertEventInfo->rtrIfNum]->downstrmAssertInfo;

  inetAddressZeroSet (mrtEntry->pimdmCB->addrFamily, &asrtIntfInfo->assertWnrAddr);
  asrtIntfInfo->assertWnrAsrtMetric = PIMDM_ASSERT_METRIC_INFINITY;
  asrtIntfInfo->assertWnrAsrtPref = PIMDM_ASSERT_METRIC_INFINITY;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Reset Assert Winner Information
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to AssertInfo
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmAssertFsmActionSendPrune (pimdmMrtEntry_t* mrtEntry,
                               pimdmAssertEventInfo_t *assertEventInfo)
{
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;
  L7_uint32 rtrIfNum = 0;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  rtrIfNum = assertEventInfo->rtrIfNum;
  asrtIntfInfo = &mrtEntry->downstrmStateInfo[rtrIfNum]->downstrmAssertInfo;

  if (asrtIntfInfo->assertTimer != L7_NULLPTR)
  {
    if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_PRUNE,
                   &asrtIntfInfo->assertWnrAddr, &mrtEntry->pimdmCB->allRoutersAddr,
                   rtrIfNum, PIMDM_DEFAULT_ASSERT_TIMEOUT, 0, 0, L7_FALSE)
                != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Re-transmission of Prune Message on "
                   "rtrIfNum - %d to Assert Winner - %s Failed", rtrIfNum,
                   inetAddrPrint(&asrtIntfInfo->assertWnrAddr, src));
    }
  }

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Do Nothing
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t pimdmAssertFsmActionDoNothing (pimdmMrtEntry_t* mrtEntry,
                                       pimdmAssertEventInfo_t *assertEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  PIMDM_UNUSED_PARAM (mrtEntry);
  PIMDM_UNUSED_PARAM (assertEventInfo);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Action Error
*
* @param    mrtEntry          @b{ (input) } Pointer MRT Entry
*           assertEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t  pimdmAssertFsmActionError (pimdmMrtEntry_t* mrtEntry,
                                    pimdmAssertEventInfo_t *assertEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  PIMDM_UNUSED_PARAM (mrtEntry);
  PIMDM_UNUSED_PARAM (assertEventInfo);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_FAILURE;
}

