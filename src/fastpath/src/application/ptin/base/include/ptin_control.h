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

#if (PTIN_BOARD_IS_MATRIX)
#ifdef PTIN_LINKSCAN_CONTROL
/**
 * Task that checks for Matrix Switchovers
 * 
 * @param numArgs 
 * @param unit 
 */
extern void ptinSwitchoverTask(L7_uint32 numArgs, void *unit);
#endif
#endif

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
