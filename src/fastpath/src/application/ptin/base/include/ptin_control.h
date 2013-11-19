/*
 * ptin_control.h
 *
 * Created on: 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_CONTROL_H
#define _PTIN_CONTROL_H

#include "ptin_include.h"

extern L7_BOOL ptin_slot_present[PTIN_SYS_SLOTS_MAX+1];
extern L7_BOOL ptin_remote_link[PTIN_SYSTEM_N_INTERF];

/**
 * Initialize interface changes notifier
 * 
 * @author mruas (11/18/2013)
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_control_intf_init(void);

/**
 * Task that runs part of the PTin initialization and further periodic 
 * processing (alarms check) 
 * 
 * @param numArgs 
 * @param unit 
 */
extern void ptinTask(L7_uint32 numArgs, void *unit);

/**
 * Task that checks for Interface changes
 * 
 * @param numArgs 
 * @param unit 
 */
extern void ptinIntfTask(L7_uint32 numArgs, void *unit);

/**
 * Initialize alarms state
 * 
 */
extern void ptin_alarms_init(void);

/**
 * Schedule Multicast machine reset
 */
extern void schedule_matrix_query_send(void);

/**
 * Register board insertion/remotion
 *  
 * @param slot_id : slot id 
 * @param enable : slot port index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_intf_boardaction_set(L7_int slot_id, L7_uint8 enable);

/*********************************************************************
* @purpose  Update the current state of a given interface.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ptinIntfChangeCallback(L7_uint32 intIfNum,
                               L7_uint32 event,
                               NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase     create or activate
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ptinIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase);

#endif /* _PTIN_CONTROL_H */
