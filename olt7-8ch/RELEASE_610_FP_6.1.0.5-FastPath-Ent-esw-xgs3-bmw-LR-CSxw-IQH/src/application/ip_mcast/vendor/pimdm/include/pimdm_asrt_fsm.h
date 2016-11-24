
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_asrt_fsm.h
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

#ifndef _PIMDM_ASRT_FSM_H
#define _PIMDM_ASRT_FSM_H

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
  PIMDM_ASSERT_EVT_DATA_ARVL_DNSTRM_IF, /* Data packet arrives on Downstream interface for (S,G) */
  PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_WNR, /* Receive Inferior Assert from Winner */
  PIMDM_ASSERT_EVT_INF_ASRT_ARVL_FROM_LSR_COULD_ASRT_TRUE, /* Receive Inferior Assert from non-Assert Winner AND couldAssert is TRUE */
  PIMDM_ASSERT_EVT_PRF_ASRT_ARVL, /* Receive Preferred Assert */
  PIMDM_ASSERT_EVT_SEND_STATE_RFR, /* Send State Refresh */
  PIMDM_ASSERT_EVT_AT_EXPIRY, /* Assert Timer for (S,G) expires */
  PIMDM_ASSERT_EVT_COULD_ASRT_FALSE, /* couldAssert is FALSE */
  PIMDM_ASSERT_EVT_COULD_ASRT_TRUE, /* couldAssert is TRUE */
  PIMDM_ASSERT_EVT_ASRT_WNR_NLT_EXPIRY, /* Winner's Neighbor Liveness Timer expires */
  PIMDM_ASSERT_EVT_PRN_JOIN_GRAFT_ARVL, /* Prune(S,G) OR Join(S,G) OR Graft(S,G) Packet arrival */
  PIMDM_ASSERT_MAX_EVENTS
}PIMDM_ASSERT_EVENT_t;

/*********************************************************************
*           Assert Interface State machine Event Information
*********************************************************************/
typedef struct pimdmAssertEventInfo_s
{
  L7_uint32         rtrIfNum;   /*router Interface Number */
  L7_inet_addr_t    assertAddr;   /* Assert IP Address */
  L7_uint32         assertMetric; /* Assert Metric */
  L7_uint32         assertPref;   /* Assert preference */
  PIMDM_CTRL_PKT_TYPE_t type;     /* type of packet that was sent */
  L7_uint32         interval;     /* interval to set the AT timer */
}pimdmAssertEventInfo_t;

/*********************************************************************
*       Action routine type declaration for Assert State Machine
*********************************************************************/
typedef L7_RC_t
(*PIMDM_ASSERT_FSM_ACTION_t) (pimdmMrtEntry_t* mrtEntry,
                              pimdmAssertEventInfo_t *assertEventInfo);

/*********************************************************************
*                 PIM-DM Assert State Machine State
*********************************************************************/
typedef struct pimdmAssertFsmState_s
{
  PIMDM_ASSERT_STATE_t      nextState; /* Next State */
  PIMDM_ASSERT_FSM_ACTION_t fsmAction; /* FSM Action Routine */
}pimdmAssertFsmState_t;

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmAssertFsmExecute (PIMDM_ASSERT_EVENT_t event,
                       pimdmMrtEntry_t* mrtEntry,
                       pimdmAssertEventInfo_t *assertEventInfo);

#endif  /* _PIMDM_ASRT_FSM_H */
