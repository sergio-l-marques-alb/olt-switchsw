
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_dnstrm_fsm.h
*
* @purpose    
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

#ifndef _PIMDM_DNSTRM_FSM_H
#define _PIMDM_DNSTRM_FSM_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "pimdm_mrt.h"

/*******************************************************************************
**                     Data Structure Definitions                             **
*******************************************************************************/

/*********************************************************************
*          Possible Events in the Assert State machine
*********************************************************************/
typedef enum
{
  PIMDM_DNSTRM_EVT_PRN_ARVL,    /* Receive Prune(S,G) */
  PIMDM_DNSTRM_EVT_JOIN_ARVL,   /* Receive Join(S,G) */
  PIMDM_DNSTRM_EVT_GRAFT_ARVL,  /* Receive Graft(S,G) */
  PIMDM_DNSTRM_EVT_PPT_EXPIRY,  /* PPT(S,G) Expiry */
  PIMDM_DNSTRM_EVT_PT_EXPIRY,   /* PT(S,G) Expiry */
  PIMDM_DNSTRM_EVT_RPF_IF_CHNG, /* RPF(S) becomes I */
  PIMDM_DNSTRM_EVT_SEND_ST_RFR, /* Send State Refresh(S,G) on I */
  PIMDM_DNSTRM_MAX_EVENTS
}PIMDM_DNSTRM_EVENT_t;

/*********************************************************************
*          Downstream Interface State machine Event Information
*********************************************************************/
typedef struct pimdmDnstrmEventInfo_s
{
  L7_uint32 rtrIfNum;
  L7_uint32 holdTime;
  L7_inet_addr_t nbrAddr;
}pimdmDnstrmEventInfo_t;

/*********************************************************************
*     Action routine type declaration for Downstream State Machine
*********************************************************************/
typedef L7_RC_t
(*PIMDM_DNSTRM_FSM_ACTION_t) (pimdmMrtEntry_t* mrtEntry,
                              pimdmDnstrmEventInfo_t *dnstrmEventInfo);

/*********************************************************************
*           PIM-DM Downstream Interface State Machine State
*********************************************************************/
typedef struct pimdmDnstrmFsmState_s
{
  PIMDM_DNSTRM_STATE_t       nextState; /* Next State */
  PIMDM_DNSTRM_FSM_ACTION_t  fsmAction; /* FSM Action Routine */
}pimdmDnstrmFsmState_t;

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmDnstrmFsmExecute (PIMDM_DNSTRM_EVENT_t event,
                       pimdmMrtEntry_t* mrtEntry,
                       pimdmDnstrmEventInfo_t *dnstrmEventInfo);

#endif  /* _PIMDM_DNSTRM_FSM_H */
