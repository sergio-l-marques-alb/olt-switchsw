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

typedef struct
{
  L7_BOOL   updated;      /* Shows this struct was updated */
  L7_uint8  enable;
  L7_uint8  link;
  L7_uint64 tx_packets;
  L7_uint64 rx_packets;
  L7_uint64 rx_error;
} struct_linkStatus_t;

/* Traffic activity bits for external module access */
extern L7_uint32 ptin_control_port_activity[PTIN_SYSTEM_N_PORTS];

#if (PTIN_BOARD == PTIN_BOARD_CXO160G || PTIN_BOARD == PTIN_BOARD_CXO640G)
#if (PHY_RECOVERY_PROCEDURE)
extern L7_BOOL slots_to_be_reseted[1+PTIN_SYS_SLOTS_MAX];
#endif
#endif


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
 * Change suppressed alarm state
 * 
 * @author mruas (8/12/2015)
 * 
 * @param port : ptin_port format
 * @param state : TRUE or FALSE
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_alarms_suppress(L7_uint32 port, L7_BOOL state);

/**
 * Get suppressed alarm state
 * 
 * @param port : ptin_port format
 * 
 * @return L7_BOOL : TRUE or FALSE
 */
extern L7_BOOL ptin_alarms_is_suppressed(L7_uint32 port);


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

extern L7_int get_linkStatus(L7_uint32 port);

/**
 * Reboot system
 * 
 * @author mruas (13/10/17)
 * 
 * @param void 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_control_reboot(void);

#endif /* _PTIN_CONTROL_H */

