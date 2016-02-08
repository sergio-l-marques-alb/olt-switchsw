/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_strfr_fsm.c
*
* @purpose    PIM-DM State Refresh State machine Action routines
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
#include "mfc_api.h"
#include "pimdm_strfr_fsm.h"
#include "pimdm_debug.h"
#include "pimdm_util.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/

static L7_RC_t
pimdmStateRfrFsmActionSetSRTSetSAT (pimdmMrtEntry_t* mrtEntry,
                                    pimdmStrfrEventInfo_t *strfrEventInfo);

static L7_RC_t
pimdmStateRfrFsmActionResetSAT (pimdmMrtEntry_t* mrtEntry,
                                pimdmStrfrEventInfo_t *strfrEventInfo);

static L7_RC_t
pimdmStateRfrFsmActionSendStateRefreshResetSRT (pimdmMrtEntry_t* mrtEntry,
                                         pimdmStrfrEventInfo_t *strfrEventInfo);

static L7_RC_t
pimdmStateRfrFsmActionCancelSRT (pimdmMrtEntry_t* mrtEntry,
                                 pimdmStrfrEventInfo_t *strfrEventInfo);

static L7_RC_t
pimdmStateRfrFsmActionCancelSRTCancelSAT (pimdmMrtEntry_t* mrtEntry,
                                         pimdmStrfrEventInfo_t *strfrEventInfo);

static L7_RC_t
pimdmStateRfrFsmActionDoNothing (pimdmMrtEntry_t* mrtEntry,
                                 pimdmStrfrEventInfo_t *strfrEventInfo);

static L7_RC_t
pimdmStateRfrFsmActionError (pimdmMrtEntry_t* mrtEntry,
                             pimdmStrfrEventInfo_t *strfrEventInfo);

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/

/*********************************************************************
*       State Refresh State machine State table definition
*********************************************************************/
static pimdmStateRfrFsmState_t
pimdmStateRfrFsmState[PIMDM_STATE_RFR_MAX_EVENTS][PIMDM_STATE_RFR_MAX_STATES] =
{
  /* EVENT: Receive Data from S AND S directly connected. 
   */
  {{PIMDM_STATE_RFR_STATE_ORIG, pimdmStateRfrFsmActionSetSRTSetSAT},
   {PIMDM_STATE_RFR_STATE_ORIG, pimdmStateRfrFsmActionResetSAT} 
  },

  /* EVENT: SRT(S,G) Expires.
   */
  {{PIMDM_STATE_RFR_STATE_NO_ORIG, pimdmStateRfrFsmActionError},
   {PIMDM_STATE_RFR_STATE_ORIG, pimdmStateRfrFsmActionSendStateRefreshResetSRT} 
  },

  /* EVENT: SAT(S,G) Expires.
   */
  {{PIMDM_STATE_RFR_STATE_NO_ORIG, pimdmStateRfrFsmActionError},
   {PIMDM_STATE_RFR_STATE_NO_ORIG, pimdmStateRfrFsmActionCancelSRT} 
  },

  /* EVENT: S no longer directly connected.
   */
  {{PIMDM_STATE_RFR_STATE_NO_ORIG, pimdmStateRfrFsmActionDoNothing},
   {PIMDM_STATE_RFR_STATE_NO_ORIG, pimdmStateRfrFsmActionCancelSRTCancelSAT} 
  }
 };

/*********************************************************************
*            Upstream Interface State machine Events List
*   (This array is indexed based on the PIMDM_STATE_RFR_EVENT_t enum)
*********************************************************************/
static L7_uchar8* pimdmStateRfrFsmEventList[] =
{
  "PIMDM_SRSM_EVT_DATA_ARVL_SRC_DIR_CONN",
  "PIMDM_SRSM_EVT_SRT_EXPIRY",
  "PIMDM_SRSM_EVT_SAT_EXPIRY",
  "PIMDM_SRSM_EVT_SRC_NOT_DIR_CONN",
};

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  State Refresh State machine Action routine invoker
*
* @param   strfrEvent       @b{ (input) } Type of the event
*          mrtEntry         @b{ (input) } Pointer MRT Entry
*          strfrEventInfo   @b{ (input) } Pointer to State Refresh Stream Info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimdmStateRfrFsmExecute (PIMDM_STATE_RFR_EVENT_t strfrEvent,
                                 pimdmMrtEntry_t* mrtEntry,
                                 pimdmStrfrEventInfo_t *strfrEventInfo)
{
  PIMDM_STATE_RFR_STATE_t currentState;
  pimdmStateRfrFsmState_t *strfrFsmState = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  if ((mrtEntry == L7_NULLPTR) || (strfrEvent >= PIMDM_STATE_RFR_MAX_EVENTS))
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
               "State Refresh FSM invoked with invalid parameters\n");
    return L7_FAILURE;
  }

  /* Get the current State and the State table's next State info. */
  currentState = mrtEntry->origStateInfo.origState;
  strfrFsmState = &(pimdmStateRfrFsmState[strfrEvent][currentState]);

  /* Execute the Action Routine pointed by the current State and event. */
  if ((strfrFsmState->fsmAction (mrtEntry, strfrEventInfo)) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"State RFR FSM State Transition failed "
            "State: [%d] ... Event [%s]", currentState,
             pimdmStateRfrFsmEventList[strfrEvent]);
    return L7_FAILURE;
  }

  /* Update the current State with that of the next State from the state table .*/
  mrtEntry->origStateInfo.origState = strfrFsmState->nextState;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR,
               "State RFR FSM State Transition Successful\n"
               "Curr State: [%d] ... Event: [%s] ... Next State: [%d]",
               currentState, pimdmStateRfrFsmEventList[strfrEvent],
               strfrFsmState->nextState);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  MRT Entry's State Refresh timer expiry Handler
*
* @param    mrtEntryUpstrmTmrHndlParam  @b{ (input) } Handle to get the MRT Entry. 
*                            
* @returns  None
*
* @comments    
*
* @end
*********************************************************************/
void pimdmStateRefreshTimerExpiryHandler (void *mrtEntryUpstrmTmrHndlParam)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  if ((mrtEntry  = (pimdmMrtEntry_t*)
                   handleListNodeRetrieve ((L7_uint32)mrtEntryUpstrmTmrHndlParam))
                == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  if (mrtEntry->origStateInfo.stateRfrTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMDM stateRfrTimer (SRT) is NULL, But Still Expired");
    return;
  }
  mrtEntry->origStateInfo.stateRfrTimer = L7_NULLPTR;

  pimdmStateRfrFsmExecute (PIMDM_STATE_RFR_EVT_SRT_EXPIRY, mrtEntry, 
                           L7_NULLPTR);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return;
}

/*********************************************************************
*
* @purpose  MRT Entry's Source Active Timer expiry Handler
*
* @param    mrtEntryUpstrmTmrHndlParam  @b{ (input) } Handle to get the MRT Entry. 
*                            
* @returns  None
*
* @comments    
*
* @end
*********************************************************************/
void pimdmSourceActiveTimerExpiryHandler (void *mrtEntryUpstrmTmrHndlParam)
{
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  mfcEntry_t mfcEntry;
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *grpAddr = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  if ((mrtEntry  = (pimdmMrtEntry_t*)
                   handleListNodeRetrieve ((L7_uint32)mrtEntryUpstrmTmrHndlParam))
                == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle Node retrieval Failed");
    return;
  }

  if (mrtEntry->origStateInfo.srcActiveTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMDM srcActiveTimer (SAT) is NULL, But Still Expired");
    return;
  }
  mrtEntry->origStateInfo.srcActiveTimer = L7_NULLPTR;

  srcAddr = &(mrtEntry->srcAddr);
  grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "SAT Expired for Src - %s, Grp - %s",
               inetAddrPrint (srcAddr, src), inetAddrPrint (grpAddr, grp));

  /* Check with MFC to see if the data has stopped.
   */
  memset (&mfcEntry, 0, sizeof (mfcEntry_t));
  inetCopy (&mfcEntry.source, srcAddr);
  inetCopy (&mfcEntry.group, grpAddr);

  if (mfcIsEntryInUse (&mfcEntry) == L7_TRUE)
  {
    /* Source is Still Active. Reset the State Active timer.
     */
    osapiSnprintf (errMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                   "SAT (S,G) ReSet Failed for Src - %s Grp - %s ",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                   "SAT (S,G) ReSet Success for Src - %s Grp - %s ",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

    if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                              pimdmSourceActiveTimerExpiryHandler,
                              (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                              PIMDM_DEFAULT_SOURCE_LIFETIME, 
                              errMsgBuf, sucMsgBuf,
                              &(mrtEntry->origStateInfo.srcActiveTimer),
                              "DM-SAT2")
                           != L7_SUCCESS)
    {
      PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                  "Failed to set source active Timer for Src - %s and Grp - %s",
                  inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
      return;
    }

    return;
  }

  pimdmStateRfrFsmExecute(PIMDM_STATE_RFR_EVT_SAT_EXPIRY, mrtEntry,
                          L7_NULLPTR);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return;
}

/*********************************************************************
*
* @purpose  Set SRT Timer + Set SAT Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          strfrEventInfo   @b{ (input) } Pointer to State Refresh Stream Info
*           
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/
static L7_RC_t pimdmStateRfrFsmActionSetSRTSetSAT (pimdmMrtEntry_t* mrtEntry,
                                       pimdmStrfrEventInfo_t *strfrEventInfo)
{
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  /* Set the Refresh timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "SRT (S,G) Set Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "SRT (S,G) Set Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmStateRefreshTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_DEFAULT_REFRESH_INTERVAL,
                            errMsgBuf, sucMsgBuf,
                            &(mrtEntry->origStateInfo.stateRfrTimer),
                            "DM-SRT")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to set State Refresh Timer for Src - %s and Grp - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return (L7_FAILURE);
  }

  /* Set the State Active timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "SAT (S,G) Set Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "SAT (S,G) Set Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmSourceActiveTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_DEFAULT_SOURCE_LIFETIME, 
                            errMsgBuf, sucMsgBuf,
                            &(mrtEntry->origStateInfo.srcActiveTimer),
                            "DM-SAT")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to set source active Timer for Src - %s and Grp - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return (L7_FAILURE);
  }

  /* Store the TTL for future purpose while sending SRM's */
  if (strfrEventInfo != L7_NULLPTR)
  {
    mrtEntry->origStateInfo.ttl = strfrEventInfo->ttl;
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return (L7_SUCCESS);
}

/*********************************************************************
*
* @purpose  Reset SAT Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          strfrEventInfo   @b{ (input) } Pointer to State Refresh Stream Info
*           
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*
* @end
*********************************************************************/
static L7_RC_t pimdmStateRfrFsmActionResetSAT (pimdmMrtEntry_t* mrtEntry,
                                    pimdmStrfrEventInfo_t *strfrEventInfo)
{
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  /* Reset the State Active timer */
  osapiSnprintf (errMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "SAT (S,G) ReSet Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf, PIMDM_MAX_DBG_MSG_SIZE,
                 "SAT (S,G) ReSet Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (mrtEntry->pimdmCB,
                            pimdmSourceActiveTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_DEFAULT_SOURCE_LIFETIME, 
                            errMsgBuf, sucMsgBuf,
                            &(mrtEntry->origStateInfo.srcActiveTimer),
                            "DM-SAT3")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to set source active Timer for Src - %s and Grp - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return (L7_FAILURE);
  }

  /* Store the TTL for future purpose while sending SRM's */
  if (strfrEventInfo != L7_NULLPTR)
  {
    mrtEntry->origStateInfo.ttl = strfrEventInfo->ttl;
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose   Send State Refresh Message + reset SRT Timer (SRT Timer Expiry)
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          strfrEventInfo   @b{ (input) } Pointer to State Refresh Stream Info
*           
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments This is a deviation from RFC.Here,we are checking for the existence
*           of mfc entry.If it exists.we are resetting the SAT timer.If the 
*           entry does not exist,we are invoking the State Refresh FSM with 
*           SAT Timer expiry event. 
*
* @end
*********************************************************************/
static L7_RC_t pimdmStateRfrFsmActionSendStateRefreshResetSRT (
                                          pimdmMrtEntry_t* mrtEntry,
                                          pimdmStrfrEventInfo_t *strfrEventInfo)
{
  pimdmCB_t *pimdmCB = mrtEntry->pimdmCB;
  L7_uchar8 errMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 sucMsgBuf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t *srcAddr = &(mrtEntry->srcAddr);
  L7_inet_addr_t *grpAddr = &(mrtEntry->grpAddr);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  /* Send the SRM packet */
  if (pimdmMrtEntryControlPacketSend (mrtEntry, PIMDM_CTRL_PKT_STATE_REFRESH,
                                      L7_NULLPTR, &(pimdmCB->allRoutersAddr),
                                      mrtEntry->upstrmRtrIfNum,
                                      PIMDM_DEFAULT_REFRESH_INTERVAL,
                                      mrtEntry->origStateInfo.ttl -1,
                                      STRFR_MSG_ORIGINATOR)
                                      != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to send State Refresh Message on Some/All of the "
                "Downstream interfaces");
  }

  /* Reset the SRT Timer */
  osapiSnprintf (errMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "SRT (S,G) ReSet Failed for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
  osapiSnprintf (sucMsgBuf,PIMDM_MAX_DBG_MSG_SIZE,
                 "SRT (S,G) ReSet Success for Src - %s Grp - %s ",
                 inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));

  if (pimdmUtilAppTimerSet (pimdmCB, pimdmStateRefreshTimerExpiryHandler,
                            (void*) mrtEntry->mrtEntryUpstrmTmrHndlParam,
                            PIMDM_DEFAULT_REFRESH_INTERVAL,
                            errMsgBuf, sucMsgBuf,
                            &(mrtEntry->origStateInfo.stateRfrTimer),
                            "DM-SRT2")
                         != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,
                "Failed to reset State Refresh Timer for Src - %s and Grp - %s",
                inetAddrPrint(srcAddr,src), inetAddrPrint(grpAddr,grp));
    return (L7_FAILURE);
  }

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Cancel State Refresh Timer (SAT Expiry handle processing)
*
* @param    mrtEntry         @b{ (input) } Pointer MRT Entry
*           strfrEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*           
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None 
*
* @end
*********************************************************************/
static L7_RC_t pimdmStateRfrFsmActionCancelSRT (pimdmMrtEntry_t* mrtEntry,
                                          pimdmStrfrEventInfo_t *strfrEventInfo)
{
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  /* Cancel State Refresh Timer */
  PIMDM_TRACE (PIMDM_DEBUG_TIMER,
              "Cancelling State Refresh Timer for Src - %s and Grp - %s",
              inetAddrPrint(&mrtEntry->srcAddr,src),
              inetAddrPrint(&mrtEntry->grpAddr,grp));

  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
                           &(mrtEntry->origStateInfo.stateRfrTimer));

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Cancel State Refresh Timer + Cancel SAT Timer
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          strfrEventInfo   @b{ (input) } Pointer to State Refresh Stream Info
*           
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments   
*
* @end
*********************************************************************/
static L7_RC_t pimdmStateRfrFsmActionCancelSRTCancelSAT (
                                          pimdmMrtEntry_t* mrtEntry,
                                          pimdmStrfrEventInfo_t *strfrEventInfo)
{
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR,"Entry");

  /* Cancel State Refresh Timer */
  PIMDM_TRACE (PIMDM_DEBUG_TIMER,
              "Cancelling State Refresh Timer for Src - %s and Grp - %s",
              inetAddrPrint(&mrtEntry->srcAddr,src),
              inetAddrPrint(&mrtEntry->grpAddr,grp));
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
                           &(mrtEntry->origStateInfo.stateRfrTimer));

  /* Cancel Source Active Timer */
  PIMDM_TRACE (PIMDM_DEBUG_TIMER,
              "Cancelling Source Active Timer for Src - %s and Grp - %s",
              inetAddrPrint(&mrtEntry->srcAddr,src),
              inetAddrPrint(&mrtEntry->grpAddr,grp));
  pimdmUtilAppTimerCancel (mrtEntry->pimdmCB,
                           &(mrtEntry->origStateInfo.srcActiveTimer));

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Do Nothing 
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          strfrEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/

static L7_RC_t pimdmStateRfrFsmActionDoNothing (pimdmMrtEntry_t* mrtEntry,
                                          pimdmStrfrEventInfo_t *strfrEventInfo)
{
  L7_uchar8 src[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uchar8 grp[PIMDM_MAX_DBG_ADDR_SIZE];

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  PIMDM_UNUSED_PARAM (strfrEventInfo);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Refresh FSM State in Action DoNothing"
               "for Src - %s and Grp - %s",
               inetAddrPrint (&(mrtEntry->srcAddr), src),
               inetAddrPrint (&(mrtEntry->grpAddr), grp));

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return (L7_SUCCESS);
}


/*********************************************************************
*
* @purpose Error Action
*
* @param   mrtEntry         @b{ (input) } Pointer MRT Entry
*          strfrEventInfo   @b{ (input) } Pointer to Refresh Stream Info
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments    
*
* @end
*********************************************************************/

static L7_RC_t  pimdmStateRfrFsmActionError (pimdmMrtEntry_t* mrtEntry,
                                          pimdmStrfrEventInfo_t *strfrEventInfo)
{
  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Entry");

  PIMDM_UNUSED_PARAM (mrtEntry);
  PIMDM_UNUSED_PARAM (strfrEventInfo);

  PIMDM_TRACE (PIMDM_DEBUG_FSM_STRFR, "Exit");
  return (L7_FAILURE);
}
