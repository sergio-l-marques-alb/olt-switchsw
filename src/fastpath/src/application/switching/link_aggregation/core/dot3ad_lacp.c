
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3adlacp.c
*
* @purpose LACP
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @notes none
*
* @create 6/5/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#define DOT3AD_LACP_GLOBALS               /* Enable global space   */
#include "dot3ad_include.h"
#include "defaultconfig.h"
#include "buff_api.h"
#include "dot3ad_debug.h"
#ifdef L7_NSF_PACKAGE
 #include "dot3ad_nsf.h"
#endif

#ifdef DOT3AD_COMPONENT_LACP

extern L7_BOOL dot3adCnfgrIsHelperPresent();
extern dot3ad_system_t dot3adSystem;
extern dot3ad_stats_t  dot3ad_stats[L7_MAX_INTERFACE_COUNT];
extern void * dot3adTimerSyncSema;
extern void * dot3adTaskSyncSema;
extern dot3ad_nsfFuncPtr_t dot3adNsfFuncTable;
const L7_uchar8 dot3adSlowProtocolsMulticastMacAddr[L7_MAC_ADDR_LEN] = 
{0x01,0x80,0xc2,0x00,0x00,0x02};                /* Tbl 43B-1 */


static dot3ad_rx_machine_state_t rxStateTable[lacpRxEvents][RX_STATES] =
{
  /*Ev/St   INITIALIZE     PORT_DISABLED   EXPIRED         LACP_DISABLED   DEFAULTED       CURRENT */
  /*E1*/   {INITIALIZE,    INITIALIZE,     INITIALIZE,     INITIALIZE,     INITIALIZE,     INITIALIZE},
  /*R2*/   {RX_STATES,     INITIALIZE,     RX_STATES,      RX_STATES,      RX_STATES,      RX_STATES},
  /*E2*/   {RX_STATES,     EXPIRED,        RX_STATES,      RX_STATES,      RX_STATES,      RX_STATES},
  /*E3*/   {RX_STATES,     LACP_DISABLED,  RX_STATES,      RX_STATES,      RX_STATES,      RX_STATES},
  /*E22*/  {RX_STATES,     EXPIRED,        RX_STATES,      RX_STATES,      RX_STATES,      RX_STATES},
  /*R1*/   {RX_STATES,     RX_STATES,      DEFAULTED,      RX_STATES,      RX_STATES,      EXPIRED},
  /*E4*/   {RX_STATES,     RX_STATES,      CURRENT,        RX_STATES,      CURRENT,        CURRENT},
  /*E5*/   {RX_STATES,     PORT_DISABLED,  PORT_DISABLED,  PORT_DISABLED,  PORT_DISABLED,  PORT_DISABLED}

};

static dot3ad_periodic_machine_state_t perStateTable[lacpPerEvents-lacpRxEvents][PER_STATES] =
{
  /*Ev/St   NO_PERIODIC    FAST_PERIODIC   SLOW_PERIODIC   PERIODIC_TX */
  /*E1*/   {NO_PERIODIC,   NO_PERIODIC,    NO_PERIODIC,    NO_PERIODIC},
  /*E6*/   {NO_PERIODIC,   NO_PERIODIC,    NO_PERIODIC,    NO_PERIODIC},
  /*E7*/   {NO_PERIODIC,   NO_PERIODIC,    NO_PERIODIC,    NO_PERIODIC},
  /*E8*/   {NO_PERIODIC,   NO_PERIODIC,    NO_PERIODIC,    NO_PERIODIC},
  /*E9*/   {PER_STATES,    PER_STATES,     PERIODIC_TX,    FAST_PERIODIC},
  /*E10*/  {PER_STATES,    SLOW_PERIODIC,  PER_STATES,     SLOW_PERIODIC},
  /*E11*/  {FAST_PERIODIC, PER_STATES,     PER_STATES,     PER_STATES},
  /*P3*/   {PER_STATES,    PERIODIC_TX,    PERIODIC_TX,    PER_STATES}

};

static dot3ad_mux_machine_state_t muxStateTable[lacpMuxEvents-lacpPerEvents][MUX_STATES] =
{
  /*Ev/St  DETACHED        WAITING         ATTACHED        COLL_DIST */
  /*E1*/   {DETACHED,      DETACHED,       DETACHED,       DETACHED},
  /*E12*/  {WAITING,       MUX_STATES,     MUX_STATES,     MUX_STATES},
  /*E13*/  {WAITING,       MUX_STATES,     DETACHED,       ATTACHED},
  /*E14*/  {MUX_STATES,    DETACHED,       DETACHED,       ATTACHED},
  /*E15*/  {MUX_STATES,    ATTACHED,       MUX_STATES,     MUX_STATES},
  /*E16*/  {MUX_STATES,    MUX_STATES,     COLL_DIST,      MUX_STATES},
  /*E17*/  {MUX_STATES,    MUX_STATES,     MUX_STATES,     ATTACHED},
  /*E18*/  {DETACHED,	   MUX_STATES,     MUX_STATES,     MUX_STATES},
  /*E19*/  {MUX_STATES,	   MUX_STATES,	   MUX_STATES,	   ATTACHED}

};

extern L7_uint32 dot3adBufferPoolId;

/**************************************************************************
*
* @purpose  Records the parameter values for the actor carried in the rx pdu
*
* @param   *p  pointer to the aggregator port structure
* @param   *pdu   pointer to the received lacpdu packet
*
* @returns  L7_SUCCESS - always as this fuction only copies information
*
* @notes    Also updates the partnerOperPortState.sync bit
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRecordPdu(dot3ad_port_t *p, dot3ad_pdu_t *pdu)
{
  /*copy actor values in p's partner values*/
  L7_uchar8 oldPartnerState;

  p->partnerOperPortNumber = pdu->actorPort;
  p->partnerOperPortPri = pdu->actorPortPri;

  memcpy(p->partnerOperSys.addr, pdu->actorSys.addr, L7_MAC_ADDR_LEN);
  p->partnerOperSysPri = pdu->actorSysPri;
  p->partnerOperKey = pdu->actorKey;

  p->partnerOperPortState = pdu->actorState;/*the partner sync bit is recaliberated later*/

  p->actorOperPortState &= ~DOT3AD_STATE_DEFAULTED;

  /* Store the old state */
  oldPartnerState = p->partnerOperPortState;

  if ((p->actorPortNum == pdu->partnerPort) &&
      (p->actorPortPri == pdu->partnerPortPri) &&
      (memcmp(dot3adSystem.actorSys.addr,pdu->partnerSys.addr,L7_MAC_ADDR_LEN) == 0) &&
      (dot3adSystem.actorSysPriority == pdu->partnerSysPri) &&
      (p->actorOperPortKey == pdu->partnerKey) &&
      (((p->actorOperPortState) & DOT3AD_STATE_AGGREGATION) == ((pdu->partnerState) & DOT3AD_STATE_AGGREGATION)) &&
      ((pdu->actorState & DOT3AD_STATE_SYNCHRONIZATION) == DOT3AD_STATE_SYNCHRONIZATION))
  {
    p->partnerOperPortState |= DOT3AD_STATE_SYNCHRONIZATION;
  }
  else if (!(pdu->actorState & DOT3AD_STATE_AGGREGATION))
  {
    p->partnerOperPortState |= DOT3AD_STATE_SYNCHRONIZATION;
  }
  else
  {
    p->partnerOperPortState &= ~DOT3AD_STATE_SYNCHRONIZATION;
  }
  return L7_SUCCESS;
}
/**************************************************************************
*
* @purpose  Records default values of admin in the operational values
*
* @param   *p   pointer to the agg. port structure
*
* @returns  L7_SUCCESS -always as this function copies default information
*
* @notes    only the partner values are defaulted
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRecordDefault(dot3ad_port_t *p)
{
  p->partnerOperPortNumber = p->partnerAdminPortNumber;
  p->partnerOperPortPri = p->partnerAdminPortPri;
  memcpy(p->partnerOperSys.addr,p->partnerAdminSys.addr,L7_MAC_ADDR_LEN);
  p->partnerOperSysPri = p->partnerAdminSysPri;
  p->partnerOperKey = p->partnerAdminKey;
  p->partnerOperPortState = p->partnerAdminPortState;

  p->actorOperPortState |= DOT3AD_STATE_DEFAULTED;

  dot3adTablesLastChangedRecord();

  return L7_SUCCESS;
}
/**************************************************************************
*
* @purpose   Updates the selected variable in the agg. port structure
*
* @param    *p          pointer to the agg. port structure
* @param    *pdu              pointer to the rx pdu
*
* @returns  L7_SUCCESS
*
* @notes  based on the parameters in the received pdu's actor values
*
* @end
*
*************************************************************************/
L7_RC_t dot3adUpdateSelected(dot3ad_port_t *p, dot3ad_pdu_t *pdu)
{
  L7_RC_t rc;

  if (!((p->partnerOperPortNumber == pdu->actorPort) &&
        (p->partnerOperPortPri == pdu->actorPortPri) &&
        (memcmp(p->partnerOperSys.addr,pdu->actorSys.addr,L7_MAC_ADDR_LEN) == 0) &&
        (p->partnerOperSysPri == pdu->actorSysPri) &&
        (p->partnerOperKey == pdu->actorKey)&&
        ((p->partnerOperPortState & DOT3AD_STATE_AGGREGATION) == (pdu->actorState & DOT3AD_STATE_AGGREGATION))
       ))
  {
    p->selected = UNSELECTED;


    /*call selection logic with unselect*/
    rc = dot3adSelectionLogicUnselect(p);
	dot3adTablesLastChangedRecord();
    
  }
  return L7_SUCCESS;
}



/**************************************************************************
*
* @purpose  Updates the selected variable in the agg. port structure
*
* @param  *p          pointer to the agg. port structure
*
* @returns  L7_SUCCESS
*
* @notes    based on the admin and oper values of the partner
*
* @end
*
*************************************************************************/
L7_RC_t dot3adUpdateDefaultSelected(dot3ad_port_t *p)
{
  L7_RC_t rc;

  if (!((p->partnerAdminPortNumber == p->partnerOperPortNumber) &&
        (p->partnerAdminPortPri == p->partnerOperPortPri) &&
        (memcmp(p->partnerAdminSys.addr, p->partnerOperSys.addr, L7_MAC_ADDR_LEN) == 0) &&
        (p->partnerAdminKey == p->partnerOperKey) &&
        ((p->partnerAdminPortState & DOT3AD_STATE_AGGREGATION) == (p->partnerOperPortState & DOT3AD_STATE_AGGREGATION))
       ))
  {
    p->selected = UNSELECTED;

    /*call selection logic with unselect*/
    rc = dot3adSelectionLogicUnselect(p);
	dot3adTablesLastChangedRecord();
    
  }
  return L7_SUCCESS;
}
/**************************************************************************
*
* @purpose   Sets the Need to Transmit variable based on mismatch of the
*
* @param     *p     pointer to the agg. port structure
* @param     *pdu         pointer to the received lacpdu
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    perceived actor(this sides) info from the other side
*
* @end
*
*************************************************************************/
L7_RC_t dot3adUpdateNtt(dot3ad_port_t *p, dot3ad_pdu_t *pdu)
{
  L7_RC_t rc;

  if (!((p->actorPortNum == pdu->partnerPort) &&
        (p->actorPortPri == pdu->partnerPortPri) &&
        (memcmp(dot3adSystem.actorSys.addr, pdu->partnerSys.addr,L7_MAC_ADDR_LEN) == 0) &&
        (dot3adSystem.actorSysPriority == pdu->partnerSysPri) &&
        (p->actorOperPortKey == pdu->partnerKey) &&
        ((p->actorOperPortState & DOT3AD_STATE_LACP_ACTIVITY) == (pdu->partnerState & DOT3AD_STATE_LACP_ACTIVITY)) &&
        ((p->actorOperPortState & DOT3AD_STATE_LACP_TIMEOUT) == (pdu->partnerState & DOT3AD_STATE_LACP_TIMEOUT)) &&
        ((p->actorOperPortState & DOT3AD_STATE_SYNCHRONIZATION) == (pdu->partnerState & DOT3AD_STATE_SYNCHRONIZATION)) &&
        ((p->actorOperPortState & DOT3AD_STATE_AGGREGATION) == (pdu->partnerState & DOT3AD_STATE_AGGREGATION))
       ))
  {
    p->ntt = L7_TRUE;
    /*send msg to tx machine with ntt*/
    rc = dot3adTransmitLacpdu(p);
  }

  return L7_SUCCESS;
}
/**************************************************************************
*
* @purpose    Actions to be performed when the current while timer expires
*
* @param      portNum      internal interface number of the port
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes      depending on the state the receive machine is in- expired state move to defaulted state
*             if in current state move to expired state . if in any other state exception taken
*
* @end
*
*************************************************************************/
L7_RC_t dot3adCurrentWhileTimerExpired(L7_uint32 portNum, L7_uint32 nullParm)
{
  dot3ad_port_t *p;
  L7_RC_t rc;
  dot3ad_agg_t *agg;
  L7_uint32 nullBuf = 0;             /* buffer not needed in call to dot3adReceiveMachine */

  p = dot3adPortIntfFind(portNum);
  if (p == L7_NULLPTR)
  {
	return L7_FAILURE;
  }
  rc = osapiSemaTake(dot3adTimerSyncSema, L7_WAIT_FOREVER);
  if (p->currentWhileTimer != L7_NULLPTR)
  {
    p->currentWhileTimer = L7_NULLPTR;
    rc = osapiSemaGive(dot3adTimerSyncSema);

    /*trace call*/
    dot3adPortTrace(p, DOT3AD_TRACE_RX_CURRWHILE_EXP);
    
    /* add check that aggregator is not static before sending lacpCurrentWhileTimerExpired event*/
    agg = dot3adAggKeyFind(p->actorOperPortKey);
    if (agg == L7_NULLPTR)
    {
      return L7_FAILURE;
    }
    if (agg->isStatic == L7_FALSE)
    {
      rc = dot3adReceiveMachine(lacpCurrentWhileTimerExpired,p,(void *)&nullBuf);
    }/* mode == static*/
  }
  else
    rc = osapiSemaGive(dot3adTimerSyncSema);
  return L7_SUCCESS;
}
/**************************************************************************
*
* @purpose    Actions to be performed when the periodic timer expires
*
* @param      portNum      internal interface number of the port
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   move to the periodic Tx state irrespective which state current state except no periodic
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPeriodicTimerExpired(L7_uint32 portNum, L7_uint32 nullParm)
{
  dot3ad_port_t *p;
  L7_RC_t rc;

  p = dot3adPortIntfFind(portNum);
  if (p == L7_NULLPTR)
  {
	return L7_FAILURE;
  }
  rc = osapiSemaTake(dot3adTimerSyncSema, L7_WAIT_FOREVER);
  if (p->periodicTimer != L7_NULLPTR)
  {
    p->periodicTimer = L7_NULLPTR;
    rc = osapiSemaGive(dot3adTimerSyncSema);

    /*trace call*/
    dot3adPortTrace(p, DOT3AD_TRACE_RX_PERWHILE_EXP);
    rc = dot3adPeriodicMachine(lacpPeriodicTimeExpired,p);
  }
  else
    rc = osapiSemaGive(dot3adTimerSyncSema);
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose    Actions to be performed when the Wait While timer expires
*
* @param      portNum      internal interface number of the port
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   check the status on the selected variable of the agg. port and
*             the ready variable of the aggregator to be attached to this agg. port
*
* @end
*
*************************************************************************/
L7_RC_t dot3adWaitWhileTimerExpired(L7_uint32 portNum, L7_uint32 nullParm)
{
  dot3ad_port_t *p;
  L7_RC_t rc;

  p = dot3adPortIntfFind(portNum);
  if (p == L7_NULLPTR)
  {
	return L7_FAILURE;
  }
  /*assert the ready_n variable to L7_TURE, so that the selection logic can
  assert the Ready variable for the aggregator and move the mux machine from
  waiting to attached*/
  rc = osapiSemaTake(dot3adTimerSyncSema,L7_WAIT_FOREVER);
  p->waitWhileTimer = L7_NULLPTR;
  rc = osapiSemaGive(dot3adTimerSyncSema);

  p->readyN = L7_TRUE;
  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_RX_WAITWHILE_EXP);
  /*call selection logic for readyselected*/
  rc = dot3adSelectionLogicReady(p);

  return L7_SUCCESS;
}


/************************************************************************/
/* Receive Machine Actions in each state*/
/************************************************************************/


/**************************************************************************
*
* @purpose    Actions to be performed when the agg port is in initialize state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineInitializeAction(dot3ad_port_t *p)
{
  L7_RC_t rc;

  p->selected = UNSELECTED;
  rc = dot3adRecordDefault(p);
  p->actorOperPortState &= ~DOT3AD_STATE_EXPIRED;
  p->aggPortMoved= L7_FALSE;



  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_RX_INIT);

  /*set the state in the receive machine*/
  p->rxState = INITIALIZE;



  /*call selection logic with unselect*/
  rc = dot3adSelectionLogicUnselect(p);

  return rc;
}



/**************************************************************************
*
* @purpose    Actions to be performed when agg. port rx machine is in port disabled state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachinePortDisabledAction(dot3ad_port_t *p)
{
  L7_RC_t rc ;
  L7_BOOL moved;

  p->partnerOperPortState &= ~DOT3AD_STATE_SYNCHRONIZATION;


  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_RX_DISABLE);

  /*set the state in the receive machine*/
  p->rxState = PORT_DISABLED;
  moved = dot3adPortMovedCheck(p);
  /*need to check for port moved here*/
  if (moved == L7_TRUE)
  {
    rc = LACIssueCmd(lacpPortMoved,p->actorPortNum,L7_NULLPTR);
    if (L7_ERROR == rc)
    {
      /*trace call*/
    }
  } /*invoke generate events*/
  rc = dot3adReceiveMachineGenerateEvent(p);
  dot3adTablesLastChangedRecord();
  
  return rc;
}

/**************************************************************************
*
* @purpose     Actions to be performed when agg. port rx machine is in lacp disabled state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineLacpDisabledAction(dot3ad_port_t *p)
{
  L7_RC_t rc;

  p->selected = UNSELECTED;
  rc = dot3adRecordDefault(p);
  p->partnerOperPortState &= ~DOT3AD_STATE_AGGREGATION;
  p->actorOperPortState &= ~DOT3AD_STATE_EXPIRED;


  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_RX_LACP_DISABLED);

  /*set the state in the receive machine*/
  p->rxState = LACP_DISABLED;
  /*call selection logic with unselect*/
  rc = dot3adSelectionLogicUnselect(p);
  dot3adTablesLastChangedRecord();
  
  return rc;
}
/**************************************************************************
*
* @purpose    Actions to be performed when agg. port rx machine is in expired state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineExpiredAction(dot3ad_port_t *p)
{
  L7_RC_t rc;
  L7_uint32 timeOut;
  dot3ad_agg_t *agg;

  
  p->partnerOperPortState &= ~DOT3AD_STATE_SYNCHRONIZATION;
  /*set the lacp time to short time out encoded as 1*/
  p->partnerOperPortState |=  DOT3AD_STATE_LACP_TIMEOUT;

  /*free current while timer if executing*/
  /*
  osapiTimerFree(p->currentWhileTimer);
  p->currentWhileTimer = L7_NULLPTR;
  */

  /* Start current while timer only if the port is member of a non-static LAG.
     For Static LAG, we don't want to wait before transitioning to the 
     DEFAULTED state. */
  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  if (agg->isStatic == L7_FALSE)
  {
    timeOut = DOT3AD_SHORT_TIMEOUT_TIME * 1000; /*must be passed in milliseconds*/
    rc = osapiSemaTake(dot3adTimerSyncSema,L7_WAIT_FOREVER);
    if (p->currentWhileTimer)
    {
      rc = osapiChangeUserTimer(p->currentWhileTimer,timeOut);
    }
    else
    {
      /*start fresh current while timer*/
      osapiTimerAdd(dot3adCurrentWhileTimerExpiredToQueue,(L7_uint32)p->actorPortNum,L7_NULL,(L7_int32)timeOut,&p->currentWhileTimer);
    }
    rc = osapiSemaGive(dot3adTimerSyncSema);
    
  }
  rc = L7_SUCCESS;

  p->actorOperPortState |= DOT3AD_STATE_EXPIRED;

  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_RX_EXPIRED);

  /*set the state in the receive machine*/
  p->rxState = EXPIRED;
  /*invoke generate events*/
  rc = dot3adReceiveMachineGenerateEvent(p);

  dot3adTablesLastChangedRecord();
   
  return rc;
}
/**************************************************************************
*
* @purpose    Actions to be performed when agg. port rx machine is in defaulted state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineDefaultedAction(dot3ad_port_t *p)
{
  L7_RC_t rc;
  dot3ad_agg_t *a;
  rc = dot3adUpdateDefaultSelected(p);
  rc = dot3adRecordDefault(p);
  /*the sync bit is set to true as per the discussion in the 43.4.12*/
  p->partnerOperPortState |= DOT3AD_STATE_SYNCHRONIZATION;

  p->actorOperPortState &= ~DOT3AD_STATE_EXPIRED;

  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_RX_DEFAULTED);

  /*set the state in the receive machine*/
  p->rxState = DEFAULTED;

  /* The static feature is LAG specific and not system-wide so
     check if the LAG is static. */
  a = dot3adAggKeyFind(p->actorOperPortKey);
  if (a == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (p->selected == UNSELECTED &&
	  a->isStatic == L7_TRUE)

    /* if (p->selected == UNSELECTED &&
	  dot3adSystem.staticLag == L7_ENABLE)   */
      rc = dot3adSelectionLogicSelect(p);
  /*invoke generate events*/
  rc = dot3adReceiveMachineGenerateEvent(p);
  return rc;
}
/**************************************************************************
*
* @purpose    Actions to be performed when agg. port rx machine is in current state
*
* @param      *p      pointer to the port structure
* @param      *pdu          pointer to the LACPDU
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineCurrentAction(dot3ad_port_t *p, dot3ad_pdu_t *pdu)
{
  L7_RC_t rc;
  L7_uint32 timeOut;
  dot3ad_agg_t *agg;

  rc = dot3adUpdateSelected(p,pdu);
  rc = dot3adUpdateNtt(p,pdu);
  rc = dot3adRecordPdu(p,pdu);
  /*invoke selection logic here after record pdu*/
  /*this will make sure that the port has selected an
  aggregator every time it receives a pdu, after it records the iformation
  there. calling the selection logic from any other place does not
  mimic the selection logic seeming to run continuously */
  if (p->selected == UNSELECTED)
  {
    rc = dot3adSelectionLogicSelect(p);
    if (L7_FAILURE == rc)
    {
      /*selection failed hence lacpUnselected is propagated
      to ensure the mux state machine can jump states to detached
      depending on the current mux state which must be only ATTACHED*/
      if (dot3adMuxMachineIsAttachedState(p) == L7_TRUE)
        rc = LACIssueCmd(lacpUnselected,p->actorPortNum,L7_NULLPTR);
    }
  }


  if (p->actorOperPortState & DOT3AD_STATE_LACP_TIMEOUT)
    timeOut = DOT3AD_SHORT_TIMEOUT_TIME * 1000;/*must be passed in milliseconds*/
  else
    timeOut = DOT3AD_LONG_TIMEOUT_TIME * 1000; /*must be passed in milliseconds*/
  /*set the time out value of the current while timer*/
  /*reset current while timer if executing*/
  /*the osapiChangeUserTimer sets the time out value and starts the timer*/
  /*
  osapiTimerFree(p->currentWhileTimer);
  p->currentWhileTimer = L7_NULLPTR;
  */

  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (agg->isStatic == L7_FALSE)
  {
    rc = osapiSemaTake(dot3adTimerSyncSema,L7_WAIT_FOREVER);
    if (p->currentWhileTimer)
    {
      rc = osapiChangeUserTimer(p->currentWhileTimer,timeOut);
    }
    else
    {
      osapiTimerAdd(dot3adCurrentWhileTimerExpiredToQueue,(L7_uint32)p->actorPortNum,L7_NULL,(L7_int32)timeOut,&p->currentWhileTimer);
    }
    rc = osapiSemaGive(dot3adTimerSyncSema);
  }/* mode ! = static */

  p->actorOperPortState &= ~DOT3AD_STATE_EXPIRED;


  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_RX_CURRENT);

  /*set the state in the receive machine*/
  p->rxState = CURRENT;

  /*generate neccessary events*/
  rc = dot3adReceiveMachineGenerateEvent(p);

  return rc;
}
/************************************************************************/
/* Receive Machine */
/************************************************************************/


/**************************************************************************
*
* @purpose    All lacp events meant for the receive machine are parsed here
*             depending on the current state of the machine different actions are taken
*
* @param      lacpEvent     lacp event that thre rx machine need to respond
* @param      *p            pointer to the port structure
* @param      *pdu          pointer to the LACPDU
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   a L7_NULLPTR is passed for the *pdu if the event is any other than lacpRxPdu
*
* @end
*
*************************************************************************/
L7_RC_t dot3adReceiveMachine(dot3ad_lacp_event_t lacpEvent,
                             dot3ad_port_t *p,
                             void* bufHandle)
{
  L7_RC_t rc=L7_SUCCESS;
  dot3ad_rx_machine_state_t nextState;
  dot3ad_pdu_t *pdu;
  
  if (lacpEvent < lacpRxEvents)/*check if event meant for rx machine*/
  {
    /*get the next event based on the lacp event and the current rx machine state*/
    nextState = rxStateTable[lacpEvent][p->rxState];

    switch (nextState)
    {
      case INITIALIZE:
        rc =dot3adRxMachineInitializeAction(p);
        /*UCT to PORT_DISABLED*/
      case PORT_DISABLED:
        rc = dot3adRxMachinePortDisabledAction(p);
        break;
      case EXPIRED:
        rc = dot3adRxMachineExpiredAction(p);
        break;
      case LACP_DISABLED:
        rc = dot3adRxMachineLacpDisabledAction(p);
        break;
      case DEFAULTED:
        rc = dot3adRxMachineDefaultedAction(p);
        break;
      case CURRENT:
        pdu = (dot3ad_pdu_t *)bufHandle;
        rc = dot3adRxMachineCurrentAction(p,pdu);
        break;
      default:
        break;
    }

  }
  return rc;
}
/**************************************************************************
*
* @purpose    Receive machine generated events
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adReceiveMachineGenerateEvent(dot3ad_port_t *p)
{
  L7_RC_t rc = L7_FAILURE;


  /* For static LAG, the Periodic machine should not be enabled. 
     The isStatic variable is set prior to lacpBegin event so we are fine to 
     check its value here */
  dot3ad_agg_t *agg;

  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (agg->isStatic == L7_TRUE)
  {
    rc = L7_SUCCESS;
    /* add check if lag is static and receive machine is in expired state, 
       directly issue the lacpCurrentWhileTimerExpired event for that port */
    if (p->rxState == EXPIRED)
    {
       rc = LACIssueCmd(lacpCurrentWhileTimerExpired,p->actorPortNum,L7_NULLPTR);
       if (L7_ERROR == rc)
       {
         rc = L7_FAILURE;
       }

    }
  }
  else
  {
   if (!(p->actorOperPortState & DOT3AD_STATE_LACP_ACTIVITY)
       && !(p->partnerOperPortState & DOT3AD_STATE_LACP_ACTIVITY))
   {
     /*both actor and partner are passive lacp (encoded as 0)*/
     /*send msg to LAC queue with lacpDisablePerMachine*/
     /*message E8*/
     if (dot3adPerMachineIsEnabled(p) == L7_TRUE)
     {
       rc = LACIssueCmd(lacpDisablePerMachine, p->actorPortNum, L7_NULLPTR);
       if (L7_ERROR == rc)
       {
         rc = L7_FAILURE;
       }
     }
   }
   else
   {

     /*one of them (actor or partner) is set to active */
     /*check for further condition of begin == false, port==enabled, and LACP == enabled*/

     if (!p->begin && p->portEnabled && p->lacpEnabled)
     {
       /*send msg to LAC queue with lacpEnablePerMachine*/
       /*message E11*/
       if (dot3adPerMachineIsEnabled(p) == L7_FALSE )
       {
         rc = LACIssueCmd(lacpEnablePerMachine, p->actorPortNum, L7_NULLPTR);
         if (L7_ERROR == rc)
         {
           rc = L7_FAILURE;
         }
       }
     }

   

   if (p->partnerOperPortState & DOT3AD_STATE_LACP_TIMEOUT)
   {
     /*partner expects short time outs*/
     /*send msg to LAC queue with lacpShortTimeOut*/
     /*message E9*/
     if (dot3adPerMachineIsSlow(p) == L7_TRUE)
     {
       rc = LACIssueCmd(lacpShortTimeOut, p->actorPortNum, L7_NULLPTR);
       if (L7_ERROR == rc)
       {
         rc = L7_FAILURE;
       }
     }
   }
   else
   {
     /*partner expects long time outs*/
     /*send msg to LAC queue with lacpLongTimeOut*/
     /*message E10*/
     if (dot3adPerMachineIsFast(p) == L7_TRUE)
     {
       rc = LACIssueCmd(lacpLongTimeOut, p->actorPortNum, L7_NULLPTR);
       if (L7_ERROR == rc)
       {
         rc = L7_FAILURE;
       }
     }
   }
  }
  

 }/*admin mode != static*/

  if (p->partnerOperPortState & DOT3AD_STATE_SYNCHRONIZATION)
  {
    if (p->selected == SELECTED)
    {
      /*partner is in sysnc with actor and actor has selected its aggregator*/
      /*send msg to LAC queue with lacpSelectedPartnerSyncTrue*/
      /*message E16*/
      if (dot3adMuxMachineIsAttachedState(p) == L7_TRUE)
      {

        rc = LACIssueCmd(lacpSelectedPartnerSyncTrue, p->actorPortNum, L7_NULLPTR);
        if (L7_ERROR == rc)
        {
          rc = L7_FAILURE;
        }
      }
    }
  }
  else
  {
    /*partner is not in sync*/
    /*send msg to LAC queue with lacpPartnerSyncFalse*/
    /*message E17*/
    if (dot3adMuxMachineIsCollDistState(p) == L7_TRUE)
    {
      rc = LACIssueCmd(lacpPartnerSyncFalse, p->actorPortNum, L7_NULLPTR);
      if (L7_ERROR == rc)
      {
        rc = L7_FAILURE;
      }
    }
  }



  return rc;
}
/************************************************************************/
/* Periodic Machine Actions in each state*/
/************************************************************************/
/**************************************************************************
*
* @purpose    Stops the periodic timer and the ntt timer
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachineNoPeriodicAction(dot3ad_port_t *p)
{
  L7_RC_t rc;
  /*no harm done to stop timers even if it is not running*/
  rc = osapiSemaTake(dot3adTimerSyncSema, L7_WAIT_FOREVER);
  if (p->periodicTimer)
  {
    rc = osapiStopUserTimer(p->periodicTimer);
  }

  if (dot3adCnfgrIsHelperPresent() == L7_TRUE)
  {
    dot3adLacpmNotifyHelperPduStop(p->actorPortNum);
  }
  rc = osapiSemaGive(dot3adTimerSyncSema);

  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_PER_NO_PERIODIC);

  p->perState = NO_PERIODIC;


  return rc;
}
/**************************************************************************
*
* @purpose    Starts the periodic timer with fast periodic time interval
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   actions performed when the state machine is in fast periodic state
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachineFastPeriodicAction(dot3ad_port_t *p)
{
  L7_RC_t rc;
  L7_uint32 perTime;
 
  perTime = DOT3AD_FAST_PER_TIME * 1000; /*passed as milliseconds*/
  
  rc = osapiSemaTake(dot3adTimerSyncSema,L7_WAIT_FOREVER);
/*start the periodic timer*/
  if (p->periodicTimer)
  {
    rc = osapiChangeUserTimer(p->periodicTimer,perTime);
    if (dot3adCnfgrIsHelperPresent() == L7_TRUE)
    {
      dot3adLacpmNotifyHelperPduModify(p->actorPortNum,perTime);
    }
  }
  else
  {
    osapiTimerAdd(dot3adPeriodicTimerExpiredToQueue,(L7_uint32)p->actorPortNum,L7_NULL,(L7_int32)perTime,&p->periodicTimer);
  }
  rc = osapiSemaGive(dot3adTimerSyncSema);
  rc = L7_SUCCESS;



  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_PER_FAST_PERIODIC);


  p->perState = FAST_PERIODIC;

  return rc;
}
/**************************************************************************
*
* @purpose    Starts the periodic timer with slow periodic time interval
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachineSlowPeriodicAction(dot3ad_port_t *p)
{
  L7_RC_t rc;
  L7_uint32 perTime;


  perTime = DOT3AD_SLOW_PER_TIME * 1000; /*passed as milliseconds*/
  /*change the periodic timer to new value slow periodic time and start it*/
  /*this assumes that if the timer was already running then a fresh timer is started*/

  rc = osapiSemaTake(dot3adTimerSyncSema,L7_WAIT_FOREVER);
  if (p->periodicTimer)
  {
    rc = osapiChangeUserTimer(p->periodicTimer,perTime);
    if (dot3adCnfgrIsHelperPresent() == L7_TRUE)
    {
      dot3adLacpmNotifyHelperPduModify(p->actorPortNum,perTime);
    }
  }
  else
  {
    osapiTimerAdd(dot3adPeriodicTimerExpiredToQueue,(L7_uint32)p->actorPortNum,L7_NULL,(L7_int32)perTime,&p->periodicTimer);
  }
  rc = osapiSemaGive(dot3adTimerSyncSema);
  rc = L7_SUCCESS;


  /*trace call*/
  dot3adPortTrace(p,  DOT3AD_TRACE_PER_SLOW_PERIODIC);

  /*set the state of the perState*/
  p->perState = SLOW_PERIODIC;


  return rc;
}
/**************************************************************************
*
* @purpose    Asserts the ntt to L7_TRUE and calls the transmit fuction
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachinePeriodicTxAction(dot3ad_port_t *p)
{
  L7_RC_t rc ;

  p->ntt = L7_TRUE;
  /* Do not transmit the PDUs let the helper function handle it*/
  rc = dot3adTransmitLacpdu(p);
  /*stop the periodic timer*/
  /*periodic machines responsibilty to start the timer all over again*/
  /*either in slow or fast periodic states*/

  rc = osapiSemaTake(dot3adTimerSyncSema, L7_WAIT_FOREVER);
  if (p->periodicTimer)
  {
    rc = osapiStopUserTimer(p->periodicTimer);
  }
  rc = osapiSemaGive(dot3adTimerSyncSema);

  /*set the state of the perState*/
  p->perState = PERIODIC_TX;
  rc = dot3adPerMachineGenerateEvents(p);
  return rc;
}
/**************************************************************************
*
* @purpose    Generates events for the state machine
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachineGenerateEvents(dot3ad_port_t *p)
{
  L7_RC_t rc;

  if (p->partnerOperPortState & DOT3AD_STATE_LACP_TIMEOUT)
  {
    /*partner expects short time outs*/
    /*send msg to LAC queue with lacpShortTimeOut*/
    /*message P1*/
    rc = LACIssueCmd(lacpShortTimeOut, p->actorPortNum, L7_NULLPTR);
    if (L7_ERROR == rc)
    {
      rc = L7_FAILURE;
    }
  }
  else
  {
    /*partner expects long time outs*/
    /*send msg to LAC queue with lacpLongTimeOut*/
    /*message P2*/
    rc = LACIssueCmd(lacpLongTimeOut, p->actorPortNum, L7_NULLPTR);
    if (L7_ERROR == rc)
    {
      rc = L7_FAILURE;

    }
  }
  return rc;
}
/**************************************************************************
*
* @purpose    State machine for the periodic timer
*
* @param      *p          pointer to the port structure
* @param      lacpEvent   the lacp event
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   processes the incoming lacp event meant for the periodic machine
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPeriodicMachine(dot3ad_lacp_event_t lacpEvent, dot3ad_port_t *p)
{
  L7_RC_t rc = L7_SUCCESS;
  dot3ad_periodic_machine_state_t nextState;
  L7_uint32 normalizedEvent;

  if (lacpEvent == lacpBegin)
    normalizedEvent = lacpEvent;

  else
    normalizedEvent = lacpEvent - lacpRxEvents;/*since there is one set of lacpevents*/

  nextState =  perStateTable[normalizedEvent][p->perState];

  switch (nextState)
  {
  case NO_PERIODIC:
    rc = dot3adPerMachineNoPeriodicAction(p);
    break;
  case FAST_PERIODIC:
    rc = dot3adPerMachineFastPeriodicAction(p);
    break;
  case SLOW_PERIODIC:
    rc = dot3adPerMachineSlowPeriodicAction(p);
    break;
  case PERIODIC_TX:
    rc = dot3adPerMachinePeriodicTxAction(p);
    break;
  default:
    break;
  }
  return rc;
}
/**************************************************************************
*
* @purpose    Checks if the Periodic machine is enabled
*
* @param      *p          pointer to the port structure
*
* @returns    L7_TRUE   if the Periodic Machine is enabled
* @returns    L7_FALSE  if the Periodic Machine is disabled
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL dot3adPerMachineIsEnabled(dot3ad_port_t *p)
{

  return(p->perState == NO_PERIODIC ? L7_FALSE : L7_TRUE);

}
/**************************************************************************
*
* @purpose    Checks if the Periodic machine is in Fast periodic state
*
* @param      *p          pointer to the port structure
*
* @returns    L7_TRUE   if the Periodic Machine is in Fast periodic state
* @returns    L7_FALSE  if the Periodic Machine is not in Fast periodic state
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL dot3adPerMachineIsFast(dot3ad_port_t *p)
{
  return(p->perState == FAST_PERIODIC ? L7_TRUE : L7_FALSE);
}
/**************************************************************************
*
* @purpose    Checks if the Periodic machine is in Slow periodic state
*
* @param      *p          pointer to the port structure
*
* @returns    L7_TRUE   if the Periodic Machine is in Slow periodic state
* @returns    L7_FALSE  if the Periodic Machine is not in Slow periodic state
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL dot3adPerMachineIsSlow(dot3ad_port_t *p)
{
  return(p->perState == SLOW_PERIODIC ? L7_TRUE : L7_FALSE);
}
/**************************************************************************
*
* @purpose    Checks if the Periodic machine has timeout as short timeout
*
* @param      *p          pointer to the port structure
*
* @returns    L7_TRUE   if the Periodic Machine has short Timeout
* @returns    L7_FALSE  if the Periodic Machine does not have short Timeout
*
* @notes   Assumes that if if it is not short timeout it is set to long timeout
*
* @end
*
*************************************************************************/
L7_BOOL dot3adPerMachineShortTimeout(dot3ad_port_t *p)
{
  return (p->actorOperPortState & DOT3AD_STATE_LACP_TIMEOUT);
}
/************************************************************************/
/* Mux Machine Actions (coupled) in each state*/
/************************************************************************/
/**************************************************************************
*
* @purpose    Actions to be performed when the mux machine is in detached state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineDetachedAction(dot3ad_port_t *p)
{
  L7_RC_t rc;

  rc = dot3adDetachMuxFromAgg(p);
  p->actorOperPortState &= ~DOT3AD_STATE_SYNCHRONIZATION;
  p->actorOperPortState &= ~DOT3AD_STATE_COLLECTING;
  rc = dot3adDisableCollDist(p);
  if(rc == L7_REQUEST_DENIED)
  {
	p->muxState = DETACHED;
    rc = LACIssueCmd(lacpDetached,p->actorPortNum,L7_NULLPTR);
	p->ntt = L7_TRUE;
    rc = dot3adTransmitLacpdu(p);
	
	/*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_MUX_CD_DIS_FAIL);
	return L7_SUCCESS;
  }
  rc = dot3adLihMacroPortAssignmentReset(p->actorPortNum);
  rc = dot3adLihNotifySystem(p->actorPortNum, L7_LAG_RELEASE);

  p->actorOperPortState &= ~DOT3AD_STATE_DISTRIBUTING;
  p->ntt = L7_TRUE;

  /*call the tx function to tx a lacpdu in this port*/
  rc = dot3adTransmitLacpdu(p);

  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_MUX_DETACHED);
  /*set mux state to detached*/
  p->muxState = DETACHED;

  /*call selection logic to select an aggregator for this port*/
  /* dot3adSelectionLogicSelect(p);*/
  dot3adTablesLastChangedRecord();
  
  return rc;
}
/**************************************************************************
*
* @purpose    Actions to be performed when mux machine is in waiting state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineWaitingAction(dot3ad_port_t *p)
{
  L7_RC_t rc;
  L7_uint32 waitTime;
  dot3ad_agg_t *agg;

  agg= dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  if (agg->isStatic == L7_FALSE)
  {
    /*re-start the wait while timer */

    waitTime = DOT3AD_AGG_WAIT_TIME * 1000; /*to be passed in miliseconds*/
    rc = osapiSemaTake(dot3adTimerSyncSema,L7_WAIT_FOREVER);
    if (p->waitWhileTimer)
    {
      rc = osapiChangeUserTimer(p->waitWhileTimer,waitTime);
    }
    else
    {
      /*to be executed only the first time*/
      /*rest of the time restart will take care of the initiation*/
      osapiTimerAdd(dot3adWaitWhileTimerExpiredToQueue,(L7_uint32)p->actorPortNum,L7_NULL,(L7_int32)waitTime,&p->waitWhileTimer);
    }
    rc = osapiSemaGive(dot3adTimerSyncSema);
    rc = L7_SUCCESS;
  }/* mode != static */



  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_MUX_WAITING);
  /*set mux state to waiting*/
  p->muxState = WAITING;

  if (agg->isStatic == L7_TRUE)
  {
    /*call selection logic for readyselected*/
    p->readyN = L7_TRUE;
     /*trace call*/
    dot3adPortTrace(p, DOT3AD_TRACE_RX_WAITWHILE_EXP);

    rc = dot3adSelectionLogicReady(p);

  }
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose    Actions to be performed when mux machine is in attached state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineAttachedAction(dot3ad_port_t *p)
{
  L7_RC_t rc;
  dot3ad_agg_t *agg;

  rc = dot3adAttachMuxToAgg(p);
  p->actorOperPortState |= DOT3AD_STATE_SYNCHRONIZATION;
  p->actorOperPortState &= ~DOT3AD_STATE_COLLECTING;
  rc = dot3adDisableCollDist(p);
  p->actorOperPortState &= ~DOT3AD_STATE_DISTRIBUTING;
  p->ntt = L7_TRUE;

  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_MUX_ATTACHED);
  /*set mux state to attached*/
  p->muxState = ATTACHED;
  /*call the tx function to tx a lacpdu with the latest parm values*/

  rc = dot3adTransmitLacpdu(p);
  
  /* Check if the LAG is Static and not the system. The static
     feature is LAG specific.
  */
  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  dot3adTablesLastChangedRecord();
  
  return rc;
}

/**************************************************************************
*
* @purpose    Actions to be performed when mux machine is in collecting_distributing state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineCollDistAction(dot3ad_port_t *p)
{
  L7_RC_t rc;

  p->actorOperPortState |= DOT3AD_STATE_DISTRIBUTING;
  rc = dot3adEnableCollDist(p);
  if (rc == L7_REQUEST_DENIED || rc == L7_FAILURE)
  {
	p->muxState = COLL_DIST;
	rc = LACIssueCmd(lacpAttached,p->actorPortNum, L7_NULLPTR);
	p->ntt = L7_TRUE;
	rc = dot3adTransmitLacpdu(p);
	/*trace call*/
    dot3adPortTrace(p, DOT3AD_TRACE_MUX_CD_EN_FAIL);
	return L7_SUCCESS;
  }

  p->actorOperPortState |= DOT3AD_STATE_COLLECTING;
  p->ntt = L7_TRUE;

  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_MUX_COLLDIST);
  /*set mux state to collDist*/
  p->muxState = COLL_DIST;

  /*call the tx function to tx a lacpdu with the latest parm values*/

  rc = dot3adTransmitLacpdu(p);
  dot3adTablesLastChangedRecord();
  
  return rc;
}

/**************************************************************************
*
* @purpose    Generates events for the state machine
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineGenerateEvents(dot3ad_port_t *p)
{
  L7_RC_t rc = L7_SUCCESS;

  if (p->muxState == ATTACHED)
  {
    if ((p->partnerOperPortState & DOT3AD_STATE_SYNCHRONIZATION) &&
        (p->selected == SELECTED))
    {
      rc = LACIssueCmd(lacpSelectedPartnerSyncTrue, p->actorPortNum, L7_NULLPTR);
      
    }
  }
  return rc;
}

/**************************************************************************
*
* @purpose    Processes lacpevents meant for the mux machine
*
* @param      lacpEvent lacp event
* @param      *p        pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   retrives the next state and executes the actions of the next state
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachine(dot3ad_lacp_event_t lacpEvent, dot3ad_port_t *p)
{
  L7_RC_t rc = L7_SUCCESS;
  dot3ad_mux_machine_state_t nextState;
  L7_uint32   normalizedEvent;

  if (lacpEvent == lacpBegin)
    normalizedEvent = lacpEvent;
  else
    normalizedEvent = lacpEvent - lacpPerEvents;

  nextState = muxStateTable[normalizedEvent][p->muxState];

  switch (nextState)
  {
  case DETACHED:
    rc = dot3adMuxMachineDetachedAction(p);
    break;
  case WAITING:
    rc = dot3adMuxMachineWaitingAction(p);
    break;
  case ATTACHED:
    rc = dot3adMuxMachineAttachedAction(p);
    break;
  case COLL_DIST:
    rc = dot3adMuxMachineCollDistAction(p);
    break;
  default:
    break;
  }

  dot3adMuxMachineGenerateEvents(p);

  return rc;
}
/*************************************************************************
*
* @purpose    Checks if the Mux machines is in ATTACHED state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_TRUE if the Mux Machine is in ATTACHED state
* @returns    L7_FALSE if the Mux Machine is not in ATTACHED state
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL dot3adMuxMachineIsAttachedState(dot3ad_port_t *p)
{
  return(p->muxState == ATTACHED ? L7_TRUE : L7_FALSE);

}
/*************************************************************************
*
* @purpose    Checks if the Mux machines is in COLL_DIST state
*
* @param      *p      pointer to the port structure
*
* @returns    L7_TRUE if the Mux Machine is in COLL_DIST state
* @returns    L7_FALSE if the Mux Machine is not in COLL_DIST state
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL dot3adMuxMachineIsCollDistState(dot3ad_port_t *p)
{
  return(p->muxState == COLL_DIST ? L7_TRUE : L7_FALSE);

}


/************************************************************************/
/* Transmit Machine Actions in each state*/
/************************************************************************/
/**************************************************************************
*
* @purpose    Sets the txCount to zero
*
* @param      portNum      internal interface number of the port
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   whenever NTT Timer expires this routine is called this in turn
*             calls the tx function if ntt is true and restarts the ntt timer
*
* @end
*
*************************************************************************/
L7_RC_t dot3adNttTimerExpired(L7_uint32 portNum,
                              L7_uint32 nullParm)
{
  L7_uint32 nttTime;
  dot3ad_port_t *p;
  L7_RC_t rc;

  p = dot3adPortIntfFind(portNum);
  if (p == L7_NULLPTR)
  {
	return L7_FAILURE;
  }
  rc = osapiSemaTake(dot3adTimerSyncSema, L7_WAIT_FOREVER);
  if (p->nttTimer!=L7_NULLPTR)
  {
    p->nttTimer = L7_NULLPTR;
    nttTime = DOT3AD_NTT_TIME * 1000;
    
    p->txCount = 0;
    /*
     * if (p->ntt)
     *   rc = dot3adTransmitLacpdu(p);
     */
    /*
    osapiTimerFree(p->nttTimer);
    */

    osapiTimerAdd(dot3adNttTimerExpiredToQueue,(L7_uint32)p->actorPortNum,L7_NULL,(L7_int32)nttTime,&p->nttTimer);
    rc = osapiSemaGive(dot3adTimerSyncSema);
  }
  else
    rc = osapiSemaGive(dot3adTimerSyncSema);
  return L7_SUCCESS;
}




/**************************************************************************
*
* @purpose    Transmits a lacpdu through dtl
*
* @param      *p      pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t dot3adTransmitLacpdu(dot3ad_port_t *p)
{
  L7_netBufHandle bufHandle;
  dot3ad_pdu_t *pdu;
  L7_uchar8 *data;
  L7_RC_t rc;
  dot3ad_agg_t *agg;

  /* check explicitly for static lag */
  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (agg->isStatic == L7_TRUE)
  {
    /* do not transmit any LACP PDUs for static lags*/
    return L7_SUCCESS; 
  }


  if (p->perState != NO_PERIODIC)
  {

    if (p->lacpEnabled && p->ntt)
    {
      if (p->txCount < 3) /*43.4.16*/
      {


        /*trace call*/
        dot3adPortTrace(p, DOT3AD_TRACE_TX_LACPDU);


        SYSAPI_NET_MBUF_GET(bufHandle);
		if (bufHandle == L7_NULL)
		{
		  return L7_FAILURE;
		}
		SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
        SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, DOT3AD_LACPDU_DATALENGTH);
        pdu = (dot3ad_pdu_t *)data;

        memset(pdu,0,sizeof(dot3ad_pdu_t));

        memcpy(pdu->da.addr, dot3adSlowProtocolsMulticastMacAddr, L7_MAC_ADDR_LEN);
        if (dot3adLihIntfAddrGet(p->actorPortNum, L7_NULL,pdu->sa.addr) == L7_SUCCESS)
        {
          pdu->lengthType = DOT3AD_SLOW_PROTOCOL_TYPE;
          pdu->subType = DOT3AD_LACP_SUBTYPE;
          pdu->verNum = DOT3AD_LACP_VERSION;

          /*actor information*/
          pdu->tvlTypeActor = DOT3AD_TVL_ACTOR_INFO;
          pdu->actorInfoLength = DOT3AD_ACTOR_INFO_LEN;
          pdu->actorSysPri = (L7_ushort16)dot3adSystem.actorSysPriority;
          memcpy(pdu->actorSys.addr, dot3adSystem.actorSys.addr, L7_MAC_ADDR_LEN);
          pdu->actorKey = (L7_ushort16)p->actorOperPortKey;
          pdu->actorPortPri = (L7_ushort16)p->actorPortPri;
          pdu->actorPort = (L7_ushort16)p->actorPortNum;
          pdu->actorState = p->actorOperPortState;

          /*reserved 3 bytes sent as zeros to comply with Ver1*/
          memset(pdu->actorReserved, 0x00, 3);

          /*partner information*/
          pdu->tvlTypePartner = DOT3AD_TVL_PARTNER_INFO;
          pdu->partnerInfoLength = DOT3AD_PARTNER_INFO_LEN;
          pdu->partnerSysPri = (L7_ushort16)p->partnerOperSysPri;
          memcpy(pdu->partnerSys.addr,p->partnerOperSys.addr, L7_MAC_ADDR_LEN);
          pdu->partnerKey = (L7_ushort16)p->partnerOperKey;
          pdu->partnerPortPri = (L7_ushort16)p->partnerOperPortPri;
          pdu->partnerPort = (L7_ushort16)p->partnerOperPortNumber;
          pdu->partnerState = p->partnerOperPortState;

          /*reserved 3 bytes sent as zeros to comply with Ver1*/
          memset(pdu->partnerReserved, 0x00, 3);

          /*collector information*/
          pdu->tvlTypeCollector = DOT3AD_TVL_COLL_INFO;
          pdu->collectorInfoLength = DOT3AD_COLL_INFO_LEN;
          pdu->collectorMaxDelay = agg->collectorMaxDelay;

          /*reserved 12 bytes sent as zeros to comply with Ver1*/
          memset(pdu->collectorReserved, 0x00, 12);

          /*Terminator Information*/
          pdu->tvlTypeTerminator = DOT3AD_TVL_TERM_INFO;
          pdu->terminatorLength = DOT3AD_TERM_INFO_LEN;

          /*reserved 50 bytes sent as zeros to comply with Ver1*/
          memset(pdu->reserved, 0x00, 50);

          /*FCS calculation*/
          /*done by lower layers*/

          /*pass this pdu to LACPM*/
          /*uncomment this just for testing without microcode*/
          rc = dot3adLacpmTransmit(p->actorPortNum,bufHandle);

          /***************************************************************/
          /*if (p->actorPortNum == 1)
            pduInfo.intIfNum = 7;
          else
            pduInfo.intIfNum = 1;
            */
          /* dot3adPduReceive(bufHandle, &pduInfo); */
          /*
          if (p->actorPortNum==1)
            dtlPduLoopback(7,bufHandle);
          else
            dtlPduLoopback(1,bufHandle);
            */



          /***************************************************************/

          p->txCount++;
          p->ntt = L7_FALSE;

        }
      }
      /*case when txCount is 3 */
      /*do not set ntt to false as we have to tx a pdu when the nttTimer expires*/
    }
    else
      p->ntt = L7_FALSE;
  }
  else
    p->ntt = L7_FALSE;
  return L7_SUCCESS;
}


/**************************************************************************
*
* @purpose   Classifies an lacp event received from the LAC
*
* @param      lacpEvent the lacp event to classify
* @param      *p      pointer to the port structure
* @param      *pdu    pointer to the received LACPDU
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   this is the entry point of all lacp events. events meant for Rx,Per,Mux
*             machines are received here and then parsed to the individual machines
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpClassifier(dot3ad_lacp_event_t lacpEvent,
                             dot3ad_port_t *p,
                             void * bufHandle)
{
  L7_RC_t rc;

  if (lacpEvent == lacpBegin)
  {
    rc = aggPortResetValues(p);

    /*whenever a port is added to the LAG, a ntt timer is created*/
    /*it is created here as this is a central place for add port*/
    /*this ntt timer is freed only when the port is deleted from the lag*/
    rc = osapiSemaTake(dot3adTimerSyncSema, L7_WAIT_FOREVER);
    osapiTimerAdd(dot3adNttTimerExpiredToQueue,(L7_uint32)p->actorPortNum,L7_NULL,(L7_int32)(DOT3AD_NTT_TIME * 1000),&p->nttTimer);
    rc = osapiSemaGive(dot3adTimerSyncSema);


    rc = dot3adReceiveMachine(lacpEvent,p,bufHandle);
    rc = dot3adPeriodicMachine(lacpEvent,p);
    rc = dot3adMuxMachine(lacpEvent,p);
  }
  else if (lacpEvent < lacpRxEvents)
  {
    rc = dot3adReceiveMachine(lacpEvent,p,bufHandle);
  }
  else if (lacpEvent < lacpPerEvents && lacpEvent > lacpRxEvents)
  {
    rc = dot3adPeriodicMachine(lacpEvent,p);
  }
  else if (lacpEvent <lacpMuxEvents && lacpEvent > lacpPerEvents)
  {
    rc = dot3adMuxMachine(lacpEvent,p);
  }
  else
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*remove this when u move this function to lac*/
/******************************/
extern dot3ad_agg_t dot3adAgg[L7_MAX_INTERFACE_COUNT];
extern dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];
/******************************/
/**************************************************************************
*
* @purpose  Adds this port to the attached list of the selected aggregator
*
* @param    *p      pointer to the agg. port structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adAttachMuxToAgg(dot3ad_port_t *p)
{
  dot3ad_agg_t *agg;
  /*L7_RC_t rc;*/


  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (p->actorPortAggId == agg->aggId)
  {
    /*port is already attached to this aggregator*/
    return L7_SUCCESS;
  }

  if (dot3adAggAttachedAdd(agg->aggId,p->actorPortNum)==L7_SUCCESS)
  {

    p->actorPortAggId = agg->aggId;
    /*send msg to LAC queue with lacAttach*/
    /*message E19*/
    /*
    rc = LACIssueCmd(lacAttach, p->actorPortNum, L7_NULLPTR);
    if (L7_ERROR == rc)
    {
      rc = L7_FAILURE;
      */
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Removes this port from the attached list of the aggregator
*
* @param   *p      pointer to the agg. port structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adDetachMuxFromAgg(dot3ad_port_t *p)
{
  dot3ad_agg_t *agg;
  L7_RC_t rc;

  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  rc = dot3adAggAttachedDelete(agg->aggId,p->actorPortNum);
  p->actorPortAggId = 0;
  /*send msg to LAC queue with lacDetach*/
  /*message E18*/
  /*
  rc = LACIssueCmd(lacDetach, p->actorPortNum, L7_NULLPTR);
  if (L7_ERROR == rc)
  {
    rc = L7_FAILURE;
    */
  /*trace call*/

  
  return L7_SUCCESS;
}
/**************************************************************************
*
* @purpose  Enables the port to receive and transmit in the microcode
*
* @param    *p      pointer to the agg. port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes    none.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adEnableCollDist(dot3ad_port_t *p)
{
  L7_RC_t rc;
  /*send msg to LAC queue with lacCollDistEnable*/
  /*message E20*/
  /*this will call the aggCollDistEnable directly without getting queued in LAC*/
  rc = aggCollDistEnable(p->actorPortNum);

  return rc;
}
/**************************************************************************
* @purpose  Disables the port to receive and transmit in the microcode
*
* @param    *p      pointer to the agg. port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes    none.
*
* @end
*************************************************************************/
L7_RC_t dot3adDisableCollDist(dot3ad_port_t *p)
{
  L7_RC_t rc;
  /*send msg to LAC queue with lacCollDistDisable*/
  /*message E21*/
  /*this will call the aggCollDistEnable directly without getting queued in LAC*/
  rc = aggCollDistDisable(p->actorPortNum);
  return rc;
}


/**************************************************************************
* @purpose    A port selects an aggregator to be finally attached to.
*
* @param      *p    pointer to the port that is trying to select an aggregator
* @param
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes  there are NO individual aggregators i.e. an individual
*            port shall not select any aggregator 43.4.14.1 (j)
*
* @end
*************************************************************************/
L7_RC_t dot3adSelectionLogicSelect(dot3ad_port_t *p)
{
  dot3ad_agg_t *agg;
  dot3ad_port_t *port;
  L7_BOOL isSameSys;
  L7_BOOL isSameKey;
  L7_BOOL isSamePartnerPort;
  L7_BOOL isSame = L7_TRUE, firstPort = L7_FALSE;
  L7_BOOL isSameSpeed;
  L7_BOOL isFullDuplex;
  L7_uint32 portSpeed, currentSpeed;
  L7_RC_t rc;

  dot3adPortTrace(p, DOT3AD_TRACE_SEL_SELECT_END);
  /*check to see if the mux state is detached*/
  /*check to see if the port has still not selected an aggregator*/
  /*43.4.15*/
  if (p->muxState != DETACHED || p->selected != UNSELECTED)
  {
    return L7_FAILURE;
  }
  /*check to see if the port is fullduplex*/
  isFullDuplex = dot3adLihIsIntfSpeedFullDuplex(p->actorPortNum);
  if (isFullDuplex == L7_FALSE)
  {
	return L7_FALSE;
  }
  /*get the current port speed*/
  (void)dot3adLihIntfSpeedGet(p->actorPortNum, &currentSpeed);

  /*check to see if the port is individual or not*/
  /*check if the partner is individual or not*/
  /*L7_TRUE for agg, L7_FALSE for individual*/
  if (p->portIndividualAgg == L7_FALSE ||
	  ((p->partnerOperPortState & DOT3AD_STATE_AGGREGATION) == 0))
  {
    return L7_FAILURE;
  }

  /*check to see if there is an aggregator with the same operational key*/
  /*43.4.14.1 (e)*/

  agg = dot3adAggKeyFind(p->actorOperPortKey);

  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /*agg has been found*/

  /*now check if the already selected ports have the same actor sys id
  actor key partner sys id and partner key and are not individual
  -- compared with this ports values*//*43.4.14.1 (f)*/
  port = dot3adAggSelectedGetFirst(agg->aggId);
  if (port == L7_NULLPTR)
  {
    firstPort = L7_TRUE;
  }

  while (port != L7_NULLPTR)
  {
    /*the actor system id is not explicitly checked as they have
    to be the same, as the ports are in the same system. the actor key is also
    the same as they would not have selected the same aggregator, only the partner
    values are compared*/
    isSameKey = L7_FALSE;/*not the same key*/
    if ((memcmp(port->partnerOperSys.addr,p->partnerOperSys.addr,L7_MAC_ADDR_LEN) == 0) &&
		(port->partnerOperSysPri == p->partnerOperSysPri))
	{
	  isSameSys = L7_TRUE;
	}
	else
	{
	  isSameSys = L7_FALSE;
	}
    if (port->partnerOperKey == p->partnerOperKey)
      isSameKey = L7_TRUE;/*the same key*/
	
	isSameSpeed = L7_TRUE;
	(void)dot3adLihIntfSpeedGet(port->actorPortNum, &portSpeed);
	if (portSpeed != currentSpeed)
	{
	  isSameSpeed = L7_FALSE;
	}

    if (isSameSys == L7_TRUE && isSameKey == L7_TRUE && isSameSpeed == L7_TRUE)
    {
      port = dot3adAggSelectedGetNext(agg->aggId,port->actorPortNum);
    }
    else
    {
      isSame = L7_FALSE;
      break;/*get out this while loop*/
    }
  }

  if ((firstPort != L7_TRUE) && (!isSame))
  {
    return L7_FAILURE;/*, as this ports values do not match the already selected ports values*/
  }

  /*now check for loopback condition*/
  /*
  Any pair of ports that are members of the same link aggregation group and
  hence want to select the same aggregator, but are connected together by
  the same link SHALL NOT select the same aggregator.
  i.e. for both ports (the port that is trying to select this aggregator and
  each port that has already selected this aggregator {checked one by one})
  the actor sys id is the same as the partner sys id; also for this port's(port A)
  partner port number is port X (one of the ports in the selected list) and port X's
  partner port number is this port (port A)
  */

  if (memcmp(dot3adSystem.actorSys.addr,p->partnerOperSys.addr,L7_MAC_ADDR_LEN)==0 &&
	  dot3adSystem.actorSysPriority == p->partnerOperSysPri)
  { /*
    get in only if this ports actor and partner sys id are the same only then would
    it be a possible loopback condition
    */

    port = dot3adAggSelectedGetFirst(agg->aggId);
	if (port == L7_NULLPTR)/*this is the first port to be in the selected list then return false*/
	{
	  return L7_FAILURE;
	}

    isSamePartnerPort = L7_FALSE;
    while (port != L7_NULLPTR)
    {

      if (p->actorPortNum == port->partnerOperPortNumber &&
          p->partnerOperPortNumber == port->actorPortNum)
      {
        isSamePartnerPort = L7_TRUE;
      }
      if (isSamePartnerPort == L7_TRUE)
      {
        return L7_FAILURE;
      }
      else
        port = dot3adAggSelectedGetNext(agg->aggId, port->actorPortNum);


    }


  }/*if loop for same actor partner sys id*/

  /*check to see if this is the first port to try and select this aggregator*/
  /*if yes then copy actor and partner sys id and key into the aggregators
    structure */
  /*if (agg->currNumSelectedMembers == 0)*/
  {
    memcpy(agg->partnerSys.addr,p->partnerOperSys.addr,L7_MAC_ADDR_LEN);
    agg->partnerSysPri = p->partnerOperSysPri;
    agg->partnerOperAggKey = p->partnerOperKey;
  }


  /*now check if there is space for this port to be addedd*/
  /*check against L7_MAX_MEMBERS_PER_LAG*/
  if (dot3adAggSelectedAdd(agg->aggId,p->actorPortNum) == L7_FAILURE)
  {
    /*put this port on STANDBY*/
    p->selected = STANDBY;
    /*put a msg in lac queue with lacpStandby*/
    rc = LACIssueCmd(lacpStandby, p->actorPortNum,L7_NULLPTR);
    if (L7_ERROR == rc)
    {
      rc = L7_FAILURE;
    }
  }
  else
  {
    /*select the aggregator*/
    p->selected = SELECTED;
    p->actorPortSelectedAggId = agg->aggId;
    /*put a msg in lac queue with lacpSelected*/
    rc = LACIssueCmd(lacpSelected,p->actorPortNum,L7_NULLPTR);
    if (L7_ERROR == rc)
    {
      rc = L7_FAILURE;
    }
  }

  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_SEL_SELECT_BEGIN);
  return rc;
}

/************************************************************************/
/* Selection Logic-- Select, Unselect, Compute Ready*/
/************************************************************************/

/**************************************************************************
* @purpose    Computes the ready variable which when set sends the mux
*
* @param      *p    pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   machine to the attached state. thus enabling the links to be
*             attached to the aggregator.
*             the ready variable is only set when all the ports waiting
*             to attach to the aggregator have their wait while timers expired
*
* @end
*************************************************************************/
L7_RC_t dot3adSelectionLogicReady(dot3ad_port_t *p)
{
  dot3ad_agg_t *agg;
  dot3ad_port_t *port;
  L7_BOOL ready;
  L7_RC_t rc;


  /*sanity check*/
  /*is this ports ready_n set to true*/
  if (p->readyN == L7_FALSE)
  {
    return L7_FAILURE;
  }

  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /*run through the aggregator's selected port list,
    check if each ports ready_n is L7_TRUE,
    if all of them are L7_TRUE, and set the ready flag to true
    send a series of msgs to LAC queue with lacpSelectedReady
    with individual port intf num
    as every port that is still selected but not attached needs to receive this
    msg for its mux state machine (in WAITING state) to transition to the
    ATTACHED state
   */
  port = dot3adAggSelectedGetFirst(agg->aggId);
  ready = L7_TRUE;
  while (port != L7_NULLPTR)
  {
    if (!port->readyN)
    {
      ready = L7_FALSE;
      break;
    }
    port = dot3adAggSelectedGetNext(agg->aggId,port->actorPortNum);
  }
  if (ready)
  {  /*
     means that all the selected ports have their ready_n set to L7_TRUE
     selection logic now needs to send notification to the mux machine
     of this ready status.
     this needs to be done for all the ports that have selected but not
     yet attached i.e. for all the ports that have their mux state in
     WAITING state only.
     */

    port = dot3adAggSelectedGetFirst(agg->aggId);
    while (port != L7_NULLPTR)
    {
      if (port->muxState == WAITING && p->selected == SELECTED)
      {
        /*includes notification for this port as well!*/
        /*since this port is already in the selected list*/
        rc = LACIssueCmd(lacpSelectedReady,port->actorPortNum,L7_NULLPTR);
        if (L7_ERROR == rc)
        {
          /*trace call*/
          dot3adPortTrace(p, lacpSelectedReady);
        }
      }
      port = dot3adAggSelectedGetNext(agg->aggId,port->actorPortNum);
    }



  }
  /*if not ready -- means some port's, which has selected this aggregator,
  wait while timer has not yet expired
  when the final timer expires all the others have already set their ready_n
  to L7_TRUE and with this final one each of them can go ahead move their mux
  state to attached-- hence do nothing now*/


  /*what to do of the ready_n???
  one possible solution- make the ready_n L7_FALSE only when the port
  is unselected since till that time the port is technically selected and
  attached to this aggregator
  */
  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_SEL_READY);

  return L7_SUCCESS;
}


/**************************************************************************
* @purpose    Removes the port from the selected list maintained in the aggregator
*
*
* @param      *p    pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes   also sends a message of unselected to the other state machines
*             through the LAC queue
*
* @end
*************************************************************************/
L7_RC_t dot3adSelectionLogicUnselect(dot3ad_port_t *p)
{
  dot3ad_agg_t *agg;
  L7_RC_t rc;

  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /*trace call*/
  dot3adPortTrace(p, DOT3AD_TRACE_SEL_UNSELECT);
  /*
  remove the port from the aggregators selected list
  */
  rc = dot3adAggSelectedDelete(agg->aggId,p->actorPortNum);
  p->actorPortSelectedAggId = 0;
  if (agg->currNumSelectedMembers == 0)
  {
    bzero((char *)agg->partnerSys.addr,L7_MAC_ADDR_LEN);
    agg->partnerSysPri = 0;
    agg->partnerOperAggKey = 0;
  }
  /*p->readyN = L7_FALSE;*/
  p->selected = UNSELECTED;
  /*send msg to LAC queue with lacpUnselected*/
  /*message E14*/
  rc = LACIssueCmd(lacpUnselected,p->actorPortNum,L7_NULLPTR);
  if (L7_ERROR == rc)
  {
    rc = L7_FAILURE;
  }

  /*check for STANDBY ports select as many as possible*/
  return L7_SUCCESS;
}

/**************************************************************************
* @purpose    Checks whether the port information has been received in any other phy interface
*
* @param      *p    pointer to the port structure
*
* @returns    L7_TRUE     if port has moved
* @returns    L7_FALSE    if port has not moved
*
* @notes
*
*
* @end
*************************************************************************/
L7_BOOL dot3adPortMovedCheck(dot3ad_port_t *p)
{
  L7_uint32 i, max;

  if (p->partnerOperPortNumber == p->partnerAdminPortNumber &&
      memcmp(p->partnerAdminSys.addr,p->partnerOperSys.addr,L7_MAC_ADDR_LEN)==0)
  {
    /*this port has default admin values installed it has just come from the init state
    hence port moved is set to false*/
    p->aggPortMoved = L7_FALSE;
    return L7_FALSE;
  }

  max = L7_MAX_PORT_COUNT + 1; 
  for (i = 0 ;i < max ;i++)
    if (p->partnerOperPortNumber == dot3adPort[i].partnerOperPortNumber &&
        memcmp(dot3adPort[i].partnerOperSys.addr,p->partnerOperSys.addr,L7_MAC_ADDR_LEN)==0)
      if (p->rxState == PORT_DISABLED && p->actorPortNum != dot3adPort[i].actorPortNum)
      {
        p->aggPortMoved = L7_TRUE;
        return L7_TRUE;
      }
/*return L7_FALSE as, if the port had moved, it would have been caught in the above
loop*/

  return L7_FALSE;
}

/**************************************************************************
* @purpose    Resumes the port state machine from steady state(not begin)
*
* @param      *p    pointer to the port structure
*
* @returns    L7_SUCCESS   if port state machine can be started
* @returns    L7_FAILURE   otherwise
*
* @notes      This method is used by NSF after applying switchover data.
*
*
* @end
*************************************************************************/
L7_RC_t dot3adResumePortStateMachine(dot3ad_port_t *p)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 timeOut;
  dot3ad_agg_t *agg;

  dot3adPerMachineFastPeriodicAction(p);
  /* start current while timer*/
    if (p->actorOperPortState & DOT3AD_STATE_LACP_TIMEOUT)
    timeOut = DOT3AD_SHORT_TIMEOUT_TIME * 1000;/*must be passed in milliseconds*/
  else
    timeOut = DOT3AD_LONG_TIMEOUT_TIME * 1000; /*must be passed in milliseconds*/
  /*set the time out value of the current while timer*/
  /*reset current while timer if executing*/
  
  

  agg = dot3adAggKeyFind(p->actorOperPortKey);
  if (agg == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (agg->isStatic == L7_FALSE)
  {
    rc = osapiSemaTake(dot3adTimerSyncSema,L7_WAIT_FOREVER);
    if (p->currentWhileTimer)
    {
      rc = osapiChangeUserTimer(p->currentWhileTimer,timeOut);
    }
    else
    {
      osapiTimerAdd(dot3adCurrentWhileTimerExpiredToQueue,(L7_uint32)p->actorPortNum,L7_NULL,(L7_int32)timeOut,&p->currentWhileTimer);
    }

    timeOut = (L7_uint32)(DOT3AD_NTT_TIME * 1000);
    if (p->nttTimer)
    {
      rc = osapiChangeUserTimer(p->nttTimer,timeOut);
    }
    else
    {
      osapiTimerAdd(dot3adNttTimerExpiredToQueue,(L7_uint32)p->actorPortNum,
                    L7_NULL, timeOut, &p->nttTimer);
    }

    /* waitwhile timer is not applicable since this logic is called 
    for ports in collecting/distributing state only.
    */

    rc = osapiSemaGive(dot3adTimerSyncSema);
  }

  DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_RECONCILE,
                                 "Port state machine resumed succesfully %d \n",
                                 p->actorPortNum);
  return rc;

}

#endif /* DOT3AD_COMPONENT_LACP*/






