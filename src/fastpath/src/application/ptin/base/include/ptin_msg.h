/**
 * ptin_msg.h
 *
 * Created on:
 * Author:
 * 
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_MSG_H
#define _PTIN_MSG_H

#include "ptin_include.h"
#include "ptin_msghandler.h"

typedef enum
{
  PTIN_MSG_OPER_NONE = 0,
  PTIN_MSG_OPER_INIT,
  PTIN_MSG_OPER_ADD,
  PTIN_MSG_OPER_REMOVE,
  PTIN_MSG_OPER_CLEAR,
  PTIN_MSG_OPER_DESTROY,
  PTIN_MSG_OPER_DEINIT
} ptin_msg_oper_t;

/******************************************************** 
 * EXTERNAL FUNCTIONS PROTOTYPES
 ********************************************************/

/* FastPath Misc Functions ****************************************************/ 
/**
 * Execute drivshell or devshell command string
 * 
 * @param str Input string used to call driv or devshell
 * 
 * @return L7_RC_t Return code L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_msg_ShellCommand_run(L7_char8 *str);

/**
 * Gets general info about FastPath firmware
 * 
 * @param msgFPInfo Pointer to output structure
 * 
 * @return L7_RC_t L7_SUCCESS (always)
 */
extern L7_RC_t ptin_msg_FPInfo_get(msg_FWFastpathInfo *msgFPInfo);


/* Reset Functions ************************************************************/
/**
 * Reset to default configuration 
 *  
 * Actions: 
 *  - EVCs are destroyed (including counter, bw profiles, clientes, etc)
 */
extern void ptin_msg_defaults_reset(msg_HwGenReq_t *msgPtr);

/**
 * Routine to asynchronously handle message processing if a 
 * given timeout is reached.
 *  
 * @param msgId   : Message Identifier 
 * @param msgPtr  : Message Pointer 
 * @param msgSize : Message Size
 * @param timeOut : Time Out (milliseconds) 
 *  
 * @notes: timeOut supported values: 
 *  - -1 (L7_WAIT_FOREVER)
 *  -  0 (L7_NO_WAIT)
 *  - >0 (wait for a short amount of period - typically less
 *    then IPC_LIB timeout ~3 seconds)
 *  
 **/
extern void ptin_msg_task_process(L7_uint32 msgId, void *msgPtr, L7_uint32 msgSize, L7_int32 timeOut);

/**
 * Reset alarms state
 *  
 */
extern void ptin_msg_alarms_reset(void);


/**
 * Read temperature sensors data
 * 
 * @param msg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_switch_temperature_get(msg_ptin_temperature_monitor_t *msg);

#endif /* _PTIN_MSG_H */

