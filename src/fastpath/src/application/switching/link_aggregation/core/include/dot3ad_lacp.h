/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3adlacp.h
*
* @purpose defines all data structures and prototypes for LACP
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @comments none
*
* @create 6/5/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DOT3AD_LACP_H
#define INCLUDE_DOT3AD_LACP_H


/**********************************************************************/



#ifndef DEFINES_DOT3AD
  #define DEFINES_DOT3AD


  #define DOT3AD_SLOW_PROTOCOL_TYPE      0x8809 /* Tbl 43B-2 */
  #define DOT3AD_ILLEGAL_SUBTYPE         0x00   /* Tbl 43B-3 */
  #define DOT3AD_ILLEGAL__SUBTYPE_MIN    0x0B   /* Tbl 43B-3 */

  #define DOT3AD_MARKER_SUBTYPE          0x02   /* 43.2.7.1.1 */
  #define DOT3AD_LACP_SUBTYPE            0x01   /* 43.2.9.1.1 */
  #define DOT3AD_LACP_VERSION            0x01   /* 43.4.2.2 */

  #define DOT3AD_MARKER_INFO             0x01   /* 43.2.7.1.1 */
  #define DOT3AD_MARKER_RESP_INFO        0x02   /* 43.2.7.1.1 */

  #define DOT3AD_LENGTHTYPE_OFFSET       12     /* 43.4.2.2 */
  #define DOT3AD_SUBTYPE_OFFSET          14     /* 43.4.2.2 */
  #define DOT3AD_TVLTYPE_OFFSET          16     /* 43.4.2.2 */

  #define DOT3AD_TVL_ACTOR_INFO          0x01   /* 43.4.2.2 */
  #define DOT3AD_TVL_PARTNER_INFO        0x02   /* 43.4.2.2 */
  #define DOT3AD_TVL_COLL_INFO           0x03   /* 43.4.2.2 */
  #define DOT3AD_TVL_TERM_INFO           0x00   /* 43.4.2.2 */
  #define DOT3AD_ACTOR_INFO_LEN          0x14   /* 43.4.2.2 */
  #define DOT3AD_PARTNER_INFO_LEN        0x14   /* 43.4.2.2 */
  #define DOT3AD_COLL_INFO_LEN           0x10   /* 43.4.2.2 */
  #define DOT3AD_TERM_INFO_LEN           0x00   /* 43.4.2.2 */

  #define DOT3AD_LACPDU_DATALENGTH       124    /* 43.4.2.2 */
  #define DOT3AD_MKRESP_DATALENGTH       124    /* 43.5.3.2 */

/*though the actual pdu (both LACPDU and Marker Response PDU) has 128 bytes,
 4 bytes are FCS to be inserted by layers below*/

/* 43.4.4 */
  #define DOT3AD_FAST_PER_TIME         1    /* in sec */
  #define DOT3AD_SLOW_PER_TIME         30   /* in sec */
  #define DOT3AD_SHORT_TIMEOUT_TIME    3    /* in sec */
  #define DOT3AD_LONG_TIMEOUT_TIME     90   /* in sec */
  #define DOT3AD_AGG_WAIT_TIME         2    /* in sec */

  #define DOT3AD_NTT_TIME              DOT3AD_FAST_PER_TIME /* 43.4.16 */


 


#endif /*DEFINES_DOT3AD*/

/*********************************************************************
* @purpose  enum for disabled and enabled 
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef enum 
{
  DISABLED = 0,
  ENABLED  = 1
} dot3ad_enable_disable_t;

/*********************************************************************
* @purpose  enum for mux machine variable 
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef enum
{
  UNSELECTED = 0,
  SELECTED   = 1,
  STANDBY    = 2
} dot3ad_selected_t;


/*********************************************************************
* @purpose  receive machine states 
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef enum
{
  INITIALIZE,
  PORT_DISABLED,
  EXPIRED,
  LACP_DISABLED,
  DEFAULTED,
  CURRENT,
  RX_STATES

} dot3ad_rx_machine_state_t;

/*********************************************************************
* @purpose  mux machine states 
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef enum
{
  DETACHED,
  WAITING,
  ATTACHED,
  COLL_DIST,
  MUX_STATES

} dot3ad_mux_machine_state_t;/*coupled control*/

/*********************************************************************
* @purpose  periodic machine states 
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef enum
{
  NO_PERIODIC,
  FAST_PERIODIC,
  SLOW_PERIODIC,
  PERIODIC_TX,
  PER_STATES

} dot3ad_periodic_machine_state_t;

/*********************************************************************
* @purpose  events controlling LACP 
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef enum
{
  /*Common events meant for more than one state machine*/
  /* 0*/
  lacpBegin,                    /*E1*/ /*Rx,Per,Mux*/  


  /*Events meant for Receive Machine alone*/
  /* 1*/lacpPortMoved,                            /*R2*/
  /* 2*/lacpPortEnabledLacpEnabled,               /*E2*/
  /* 3*/lacpPortEnabledLacpDisabled,              /*E3*/
  /* new event */
  /* 4*/lacpPortEnabledStaticLag,                 /*E22*/ 
  /* 5*/lacpCurrentWhileTimerExpired,             /*R1*/
  /* 6*/lacpPduRx,                                /*E4*/
  /* 7*/lacpBeginFalsePortDisabledPortMovedFalse, /*E5*/
  /* 8*/lacpRxEvents,

  /*Events meant for Periodic machine alone*/
  /* 9*/lacpDisabled,                 /*E6*/ 
  /*10*/lacpPortDisabled,             /*E7*/
  /*11*/lacpDisablePerMachine,        /*E8*/ /*lacp_activity of actor AND partner is set to 0 -- both passive*/
  /*12*/lacpShortTimeOut,             /*E9*//*P1*/
  /*13*/lacpLongTimeOut,              /*E10*//*P2*/
  /*14*/lacpEnablePerMachine,         /*E11*/ /*lacp_activity of actor OR partner is set to 1-- one of them active*/
  /*15*/lacpPeriodicTimeExpired,      /*P3*/
  /*16*/lacpPerEvents,

  /*Events meant for Mux Machine alone*/
  /*17*/lacpSelected,                 /*E12*/
  /*18*/lacpStandby,                  /*E13*/
  /*19*/lacpUnselected,               /*E14*/
  /*20*/lacpSelectedReady,            /*E15*/
  /*21*/lacpSelectedPartnerSyncTrue,  /*E16*/
  /*22*/lacpPartnerSyncFalse,         /*E17*/
  /*23*/lacpDetached,				  /*E18*/
  /*24*/lacpAttached,				  /*E19*/
  /*25*/lacpMuxEvents,

  /*Events meant for LAC*/
  /*26*/lacAttach,                    /*E19*/
  /*27*/lacDetach,                    /*E18*/
  /*28*/lacCollDistEnable,            /*E20*/
  /*29*/lacCollDistDisable,           /*E21*/
  /*30*/lacDeletePort,
  /*31*/lacNimIntfChange,  
  /*32*/lacNimIntfStartup,  
  /*33*/lacCnfgrChange,


  /*34*/lacpLacEvents

} dot3ad_lacp_event_t;


/*************************************************************************
                            FUNCTION PROTOTYPES
*************************************************************************/

/**************************************************************************
*
* @purpose  records the parameter values for the actor carried in the rx pdu
*
* @param   *p  pointer to the aggregator port structure          
* @param   *pdu   pointer to the received lacpdu packet            
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    Also updates the partnerOperPortState.sync bit
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRecordPdu(dot3ad_port_t *p, dot3ad_pdu_t *pdu);

/**************************************************************************
*
* @purpose  records default values of admin in the operational values 
*
* @param   *p   pointer to the agg. port structure           
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    only the partner valus are defaulted
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRecordDefault(dot3ad_port_t *p);
/**************************************************************************
*
* @purpose   updates the selected variable in the agg. port structure
*
* @param    *p          pointer to the agg. port structure
* @param    *pdu              pointer to the rx pdu
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments  based on the parameters in the received pdu's actor values
*
* @end
*
*************************************************************************/
L7_RC_t dot3adUpdateSelected(dot3ad_port_t *p, dot3ad_pdu_t *pdu);

/**************************************************************************
*
* @purpose  updates the selected variable in the agg. port structure
*
* @param  *p          pointer to the agg. port structure            
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    based on the admin and oper values of the partner
*
* @end
*
*************************************************************************/
L7_RC_t dot3adUpdateDefaultSelected(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose   sets the Need to Transmit variable based on mismatch of the 
*
* @param              
* @param                  
* @param       
* @param    
*
* @returns  none.
*
* @comments    perceived actor(this sides) info from the other side
*
* @end
*
*************************************************************************/
L7_RC_t dot3adUpdateNtt(dot3ad_port_t *p, dot3ad_pdu_t *pdu);

/**************************************************************************
*
* @purpose  enable the microcode to add this port in the LAG
*
* @param    *p      pointer to the agg. port structure          
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adAttachMuxToAgg(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose  enable the microcode to remove this port in the LAG
*
* @param   *p      pointer to the agg. port structure           
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adDetachMuxFromAgg(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose  enables the port to receive and transmit in the microcode
*
* @param    *p      pointer to the agg. port structure          
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adEnableCollDist(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose  disables the port to receive and transmit in the microcode
*
* @param    *p      pointer to the agg. port structure          
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adDisableCollDist(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    actions to be performed when the current while timer expires
*
* @param      portNum      internal interface number of the port        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   depending on the state the receive machine is in- expired state move to defaulted state
*             if in current state move to expired state . if in any other state exception taken
*
* @end
*
*************************************************************************/ 
L7_RC_t dot3adCurrentWhileTimerExpired(L7_uint32 portNum, L7_uint32 nullParm);

/**************************************************************************
*
* @purpose    actions to be performed when the periodic timer expires
*
* @param      portNum      internal interface number of the port        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   move to the periodic Tx state irrespective which state current state except no periodic
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPeriodicTimerExpired(L7_uint32 portNum, L7_uint32 nullParm);

/**************************************************************************
*
* @purpose    actions to be performed when the Wait While timer expires
*
* @param      portNum      internal interface number of the port        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   check the status on the selected variable of the agg. port and
*             the ready variable of the aggregator to be attached to this agg. port
*
* @end
*
*************************************************************************/
L7_RC_t dot3adWaitWhileTimerExpired(L7_uint32 portNum, L7_uint32 nullParm);




/************************************************************************/
/* Receive Machine Actions in each state*/
/************************************************************************/
/**************************************************************************
*
* @purpose    actions to be performed when the agg port is in initialize state
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineInitializeAction(dot3ad_port_t *p);



/**************************************************************************
*
* @purpose    actions to be performed when agg. port rx machine is in port disabled state
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachinePortDisabledAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose     actions to be performed when agg. port rx machine is in lacp disabled state
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineLacpDisabledAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    actions to be performed when agg. port rx machine is in expired state
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineExpiredAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    actions to be performed when agg. port rx machine is in defaulted state 
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineDefaultedAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    actions to be performed when agg. port rx machine is in current state
*
* @param      *p      pointer to the port structure        
* @param      *pdu          pointer to the LACPDU
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adRxMachineCurrentAction(dot3ad_port_t *p, dot3ad_pdu_t *pdu);



/**************************************************************************
*
* @purpose    all lacp events meant for the receive machine are parsed here
*             depending on the current state of the machine different actions are taken
*
* @param      lacpEvent     lacp event that thre rx machine need to respond
* @param      *p            pointer to the port structure        
* @param      *pdu          pointer to the LACPDU
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adReceiveMachine(dot3ad_lacp_event_t lacpEvent, 
                             dot3ad_port_t *p, 
                             void* bufHandle);

/**************************************************************************
*
* @purpose    receive machine generated events
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adReceiveMachineGenerateEvent(dot3ad_port_t *p);


/************************************************************************/
/* Mux Machine Actions (coupled) in each state*/
/************************************************************************/
/**************************************************************************
*
* @purpose    actions to be performed when the mux machine is in detached state
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineDetachedAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    actions to be performed when mux machine is in waiting state
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineWaitingAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    actions to be performed when mux machine is in attached state
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineAttachedAction(dot3ad_port_t *p);
/**************************************************************************
*
* @purpose    actions to be performed when mux machine is in collecting_distributing state
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachineCollDistAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    this processes lacpevents meant for the mux machine
*
* @param      lacpEvent lacp event
* @param      *p        pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   retrives the next state and executes the actions of the next state
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMuxMachine(dot3ad_lacp_event_t lacpEvent, dot3ad_port_t *p);

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
L7_BOOL dot3adMuxMachineIsAttachedState(dot3ad_port_t *p);

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
L7_BOOL dot3adMuxMachineIsCollDistState(dot3ad_port_t *p);

/************************************************************************/
/* Transmit Machine Actions in each state*/
/************************************************************************/
/**************************************************************************
*
* @purpose    set the txCount to zero 
*
* @param      portNum      internal interface number of the port        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   whenever NTT Timer expires this routine is called this in turn
*             calls the tx function
*
* @end
*
*************************************************************************/
L7_RC_t dot3adNttTimerExpired(L7_uint32 portNum, L7_uint32 nullParm);

/**************************************************************************
*
* @purpose    transmits a lacpdu through dtl
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adTransmitLacpdu(dot3ad_port_t *p);




/************************************************************************/
/* Periodic Machine Actions in each state*/
/************************************************************************/
/**************************************************************************
*
* @purpose    stop the periodic timer and the ntt timer
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachineNoPeriodicAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    start the periodic timer with fast periodic time interval
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   actions performed when the state machine is in fast periodic state
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachineFastPeriodicAction(dot3ad_port_t *p);
/**************************************************************************
*
* @purpose    start the periodic timer with slow periodic time interval    
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments      
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachineSlowPeriodicAction(dot3ad_port_t *p);
/**************************************************************************
*
* @purpose    assert the ntt to L7Ture and call the transmit fuction
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachinePeriodicTxAction(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    the state machine for the periodic timer
*
* @param      *p          pointer to the port structure   
* @param      lacpEvent   the lacp event   
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   processes the incoming lacp event meant for the periodic machine
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPeriodicMachine(dot3ad_lacp_event_t lacpEvent, dot3ad_port_t *p);

/**************************************************************************
*
* @purpose    generate events for the state machine
*
* @param      *p      pointer to the port structure        
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPerMachineGenerateEvents(dot3ad_port_t *p);

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
L7_BOOL dot3adPerMachineIsEnabled(dot3ad_port_t *p);

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
L7_BOOL dot3adPerMachineShortTimeout(dot3ad_port_t *p);
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
L7_BOOL dot3adPerMachineIsFast(dot3ad_port_t *p);
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
L7_BOOL dot3adPerMachineIsSlow(dot3ad_port_t *p);

/**************************************************************************
*
* @purpose   classifies an lacp event received from the LAC
*
* @param      lacpEvent the lacp event to classify
* @param      *p      pointer to the port structure
* @param      *pdu    pointer to the received LACPDU     
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   this is the entry point of all lacp events. events meant for Rx,Per,Mux
*             machines are received here and then parsed to the individual machines
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpClassifier(dot3ad_lacp_event_t lacpEvent, 
                             dot3ad_port_t *p, 
                             void * bufHandle);


/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*place the selection logic prototypes in an appropriate place*/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

L7_RC_t dot3adSelectionLogicUnselect(dot3ad_port_t *p);
/**************************************************************************
*
* @purpose    to compute the ready variable which when set sends the mux  
*                  
*
* @param      *p    pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   machine to the attached state. thus enabling the links to be 
*             attached to the aggregator.
*             the ready variable is only set when all the ports waiting 
*             to attach to the aggregator have their wait while timers expired
*            
*             
*
* @end
*
*************************************************************************/
L7_RC_t dot3adSelectionLogicReady(dot3ad_port_t *p);
/**************************************************************************
*
* @purpose    to remove the port from the selected list maintained in the aggregator
*                  
*
* @param      *p    pointer to the port structure
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @comments   also sends a message of unselected to the other state machines 
*             through the LAC queue
*            
*             
*
* @end
*
*************************************************************************/
L7_RC_t dot3adSelectionLogicSelect(dot3ad_port_t *p);
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
L7_BOOL dot3adPortMovedCheck(dot3ad_port_t *p);

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
L7_RC_t dot3adResumePortStateMachine(dot3ad_port_t *p);

#endif /*INCLUDE_DOT3AD_H*/
