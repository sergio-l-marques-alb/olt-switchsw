
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_upstrm_fsm.h
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

#ifndef _PIMDM_UPSTRM_FSM_H
#define _PIMDM_UPSTRM_FSM_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "pimdm_mrt.h"

/*******************************************************************************
**                     Data Structure Definitions                             **
*******************************************************************************/

/*********************************************************************
*          Possible Events in the Upstream State machine
*********************************************************************/
typedef enum
{
  PIMDM_UPSTRM_EVT_DATA_ARVL_OIF_NULL_PLT_NOTRUN, /* Data packet Arrival on RPF Interface(S) and OIF(S,G) is NULL and PLT(S,G) not running */
  PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_TRUE, /* State Refresh(S,G) Arrival from RPF'(S) and PruneIndicator is 1 */
  PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_NR, /* State Refresh(S,G) Arrival from RPF'(S) and PruneIndicator is 0 and PLT(S,G) not running */
  PIMDM_UPSTRM_EVT_JOIN_ARVL_RPF_SRC, /* Join(S,G) Arrival to RPF'(S) */
  PIMDM_UPSTRM_EVT_PRUNE_ARVL, /* Prune(S,G) Arrival */
  PIMDM_UPSTRM_EVT_OT_EXPIRY, /* OT(S,G) Expiry */
  PIMDM_UPSTRM_EVT_OIF_NULL, /* OIF(S,G) is NULL */
  PIMDM_UPSTRM_EVT_OIF_NOT_NULL, /* OIF(S,G) is not NULL */
  PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NOT_NULL, /* RPF'(S) Changes and OIF(S,G) not NULL */
  PIMDM_UPSTRM_EVT_RPF_CHNG_OIF_NULL, /* RPF'(S) Changes and OIF(S,G) is NULL */
  PIMDM_UPSTRM_EVT_SRC_DIR_CONN, /* S becomes directly connected */
  PIMDM_UPSTRM_EVT_GRT_EXPIRY, /* GRT(S,G) Expiry */
  PIMDM_UPSTRM_EVT_GRFT_ACK_ARVL, /* GraftAck(S,G) Arrival from RPF'(S) */
  PIMDM_UPSTRM_EVT_RPF_INTF_CHANGE, /* RPF Interface of the entry has changed */
  PIMDM_UPSTRM_EVT_STRFR_ARVL_PI_FALSE_PLT_RUNNING, /* State Refresh(S,G) Arrival from RPF'(S) and PruneIndicator is 0 and PLT(S,G) is running */
  PIMDM_UPSTRM_MAX_EVENTS
}PIMDM_UPSTRM_EVENT_t;

/*********************************************************************
*            Upstream State machine Event Information
*********************************************************************/
typedef struct pimdmUpstrmEventInfo_s
{
  L7_uint32 rtrIfNum;
}pimdmUpstrmEventInfo_t;

/*********************************************************************
*     Action routine type declaration for Assert State Machine
*********************************************************************/
typedef L7_RC_t
(*PIMDM_UPSTRM_FSM_ACTION_t) (pimdmMrtEntry_t* mrtEntry,
                              pimdmUpstrmEventInfo_t *upstrmEventInfo);

/*********************************************************************
*                 PIM-DM Upstream State Machine State
*********************************************************************/
typedef struct pimdmUpstrmFsmState_s
{
  PIMDM_UPSTRM_STATE_t      nextState; /* Next State */
  PIMDM_UPSTRM_FSM_ACTION_t fsmAction; /* FSM Action Routine */
}pimdmUpstrmFsmState_t;

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmUpstrmFsmExecute (PIMDM_UPSTRM_EVENT_t event,
                       pimdmMrtEntry_t* mrtEntry,
                       pimdmUpstrmEventInfo_t *upstrmEventInfo);

extern void
pimdmUpstreamAssertTimerExpiryHandler (void *handle);

#endif  /* _PIMDM_UPSTRM_FSM_H */
