
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_strfr_fsm.h
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

#ifndef _PIMDM_STRFR_FSM_H
#define _PIMDM_STRFR_FSM_H

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
  PIMDM_STATE_RFR_EVT_DATA_ARVL_SRC_DIR_CONN, /* Data packet arrives and source directly connected */
  PIMDM_STATE_RFR_EVT_SRT_EXPIRY,             /* State Refresh Timer expires */
  PIMDM_STATE_RFR_EVT_SAT_EXPIRY,             /* Source Active Timer expires */
  PIMDM_STATE_RFR_EVT_SRC_NOT_DIR_CONN,       /* Source not directly connected */
  PIMDM_STATE_RFR_MAX_EVENTS
}PIMDM_STATE_RFR_EVENT_t;

/*********************************************************************
*             State Refresh State machine Event Information
*********************************************************************/
typedef struct pimdmStrfrEventInfo_s
{
  L7_uint32 ttl;
}pimdmStrfrEventInfo_t;

/*********************************************************************
*     Action routine type declaration for Downstream State Machine
*********************************************************************/
typedef L7_RC_t
(*PIMDM_STRFR_FSM_ACTION_t) (pimdmMrtEntry_t* mrtEntry,
                             pimdmStrfrEventInfo_t *strfrEventInfo);

/*********************************************************************
*             PIM-DM State Refresh State Machine State
*********************************************************************/
typedef struct pimdmStateRfrFsmState_s
{
  PIMDM_STATE_RFR_STATE_t  nextState; /* Next State */
  PIMDM_STRFR_FSM_ACTION_t fsmAction; /* FSM Action Routine */
}pimdmStateRfrFsmState_t;

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmStateRfrFsmExecute (PIMDM_STATE_RFR_EVENT_t event,
                         pimdmMrtEntry_t* mrtEntry,
                         pimdmStrfrEventInfo_t *strfrEventInfo);

#endif  /* _PIMDM_STRFR_FSM_H */
