
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  telnet_cnfgr.h
*
* @purpose   Specifies the data structures and interface for the new
*            configurator API
*
* @component 
*
* @comments  none
*
* @create    02/27/2003
*
* @author    anindya
*
* @end
*             
**********************************************************************/

/*************************************************************
		
*************************************************************/

#ifndef INCLUDE_TELNET_CNFGR_H
#define INCLUDE_TELNET_CNFGR_H

#include "l7_cnfgr_api.h"


/* Different phases of the unit */
typedef enum {
  TELNET_PHASE_INIT_0 = 0,
  TELNET_PHASE_INIT_1,
  TELNET_PHASE_INIT_2,
  TELNET_PHASE_WMU,
  TELNET_PHASE_INIT_3,
  TELNET_PHASE_EXECUTE,
  TELNET_PHASE_UNCONFIG_1,
  TELNET_PHASE_UNCONFIG_2,
  TELNET_PHASE_LAST_ELEMENT
} telnetCnfgrState_t;

#if 0
extern L7_char8 *ospfCfgPhaseNames[TELNET_PHASE_LAST_ELEMENT];
#endif


/* Outbound Telnet processing task only handles configurator 
   commands and restore and admin mode change events. So
   processing message queue size needs to be just big enough
   to handle all cnfgr commands and possibly 2 more messages
   for thie restore event and admin mode change event */

#define TELNET_PROC_MSG_COUNT (TELNET_PHASE_LAST_ELEMENT + 2)


/* Types of message passed to the Outbound Telnet processing
   task via the Outbound Telnet processing queue */

typedef enum {
    TELNET_CNFGR_CMD = 0,
    TELNET_ADMIN_MODE_EVENT,
    TELNET_RESTORE_EVENT
} telnetProcMessageType_t;


/* Structure for message carrying information about an interface event */
typedef struct telnetIntfEventMsg_s
{
    L7_uint32           intIfNum;       
    L7_PORT_EVENTS_t    event;          /* one of L7_PORT_EVENTS_t from NIM */
    NIM_CORRELATOR_t    correlator;
} telnetIntfEventMsg_t;


/* Structure for message carrying information about an admin mode event */

typedef struct telnetAdminModeEventMsg_s
{
    L7_uint32           intIfNum;       
    L7_uint32           mode;          /* L7_ENABLE or L7_DISABLE */
} telnetAdminModeEventMsg_t;


/* Message received by Outbound telnet processing task through
   Outbound Telnet processing queue  */

typedef struct
{
    telnetProcMessageType_t msgType;

    union
    {
        telnetAdminModeEventMsg_t adminModeEvent;
        L7_CNFGR_CMD_DATA_t cmdData;
    } type;

} telnetProcMsg_t;


#define TELNET_IS_READY (((telnetCnfgrState == TELNET_PHASE_INIT_3) || \
					   (telnetCnfgrState == TELNET_PHASE_EXECUTE) || \
					   (telnetCnfgrState == TELNET_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.       
*
* @returns  None
*
* @notes    This function completes synchronously. The return value 
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:   
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread.
*
* @notes    Handles only Phase 1 initialization on the configurator thread. 
*           Enqueues all other command/request pairs for processing task to handle.
*       
* @end
*********************************************************************/

void telnetApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Init Phase 1. Init phase 1 mainly invloves allocation of memory,
*           creation of semaphores, message queues, tasks and other resources 
*           used by the protocol
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. 
*       
* @end
*********************************************************************/

L7_RC_t telnetCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2. Phase 2 mainly involves callback registration.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs on the processing task.
*       
*       
* @end
*********************************************************************/

L7_RC_t telnetCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3. Phase 3 involves applying building and 
*           applying the configuration onto the hardware.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/

L7_RC_t telnetCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse - @b{(output)}  Response always command complete.
*     
* @param    pReason   - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           None.
*       
* @end
*********************************************************************/

L7_RC_t telnetCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse, 
                                 L7_CNFGR_ERR_RC_t   *pReason );


/*********************************************************************
* @purpose  This function undoes telnetCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

void telnetCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes telnetCnfgrInitPhase2Process, i.e. 
*           deregister from all callbacks.
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

void telnetCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes telnetCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    telnetCnfgrUnconfigPhase2 unapplies the configuration.
*           Hence, is reused to undo INIT_3
*
* @end
*********************************************************************/

void telnetCnfgrFiniPhase3Process();

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Unconfigure Phase 2. Mainly unapplies the current 
*           configuration
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
*           telnetRestoreProcess only builds default configuration and 
*           applies it, while UNCONFIG_2 requires configuration information 
*           to be reset. Hence, telnetCfgData is reset to 0.
*       
* @end
*********************************************************************/

L7_RC_t telnetCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
*
* @purpose  Handles initialization messages from the configurator. 
*
* @param    pCmdData - @b{(input)}  Indicates the command and request 
*                                   from the configurator
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/

void telnetCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_TELNET_CNFGR_H */
