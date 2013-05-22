
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  dhcps_cnfgr.h
*
* @purpose   Specifies the data structures and interface for the new
*            configurator API
*
* @component
*
* @comments  none
*
* @create    11/18/2003
*
* @author    ksriniv
*
* @end
*
**********************************************************************/

/*************************************************************
    
*************************************************************/

#ifndef INCLUDE_DHCPS_CNFGR_H
#define INCLUDE_DHCPS_CNFGR_H

#include "l7_cnfgr_api.h"
#include "nimapi.h"

/* Different phases of the unit */
typedef enum {
  DHCPS_PHASE_INIT_0 = 0,
  DHCPS_PHASE_INIT_1,
  DHCPS_PHASE_INIT_2,
  DHCPS_PHASE_WMU,
  DHCPS_PHASE_INIT_3,
  DHCPS_PHASE_EXECUTE,
  DHCPS_PHASE_UNCONFIG_1,
  DHCPS_PHASE_UNCONFIG_2,
  DHCPS_PHASE_LAST_ELEMENT
} dhcpsCnfgrState_t;

#if 0
extern L7_char8 *ospfCfgPhaseNames[DHCPS_PHASE_LAST_ELEMENT];
#endif

/* DHCPS processing task only handles configurator commands
   and restore and admin mode change events. So processing
   message queue size needs to be just big enough to handle
   all cnfgr commands and possibly 2 more messages for the
   restore event and admin mode change event */
#define DHCPS_PROC_MSG_COUNT (DHCPS_PHASE_LAST_ELEMENT + 2)

/* Types of message passed to the DHCPS processing task
   via the DHCPS processing queue */
typedef enum {
    DHCPS_CNFGR_CMD = 0,
    DHCPS_ADMIN_MODE_EVENT,
    DHCPS_RESTORE_EVENT
} dhcpsProcMessageType_t;

/* Structure for message carrying information about an interface event */
typedef struct dhcpsIntfEventMsg_s
{
    L7_uint32           intIfNum;
    L7_PORT_EVENTS_t    event;          /* one of L7_PORT_EVENTS_t from NIM */
    NIM_CORRELATOR_t    correlator;
} dhcpsIntfEventMsg_t;


/* Structure for message carrying information about an admin mode event */
typedef struct dhcpsAdminModeEventMsg_s
{
    L7_uint32           intIfNum;
    L7_uint32           mode;          /* L7_ENABLE or L7_DISABLE */
} dhcpsAdminModeEventMsg_t;


/* Message received by DHCPS processing task through DHCPS processing queue  */
typedef struct
{
    dhcpsProcMessageType_t msgType;

    union
    {
        dhcpsAdminModeEventMsg_t adminModeEvent;
        L7_CNFGR_CMD_DATA_t cmdData;
    } type;

} dhcpsProcMsg_t;



#define DHCPS_IS_READY (((dhcpsCnfgrState == DHCPS_PHASE_INIT_3) || \
             (dhcpsCnfgrState == DHCPS_PHASE_EXECUTE) || \
             (dhcpsCnfgrState == DHCPS_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))


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
void dhcpsApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );

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
L7_RC_t dhcpsCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t dhcpsCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t dhcpsCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t dhcpsCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason );


/*********************************************************************
* @purpose  This function undoes dhcpsCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpsCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes dhcpsCnfgrInitPhase2Process, i.e.
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
void dhcpsCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes dhcpsCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    dhcpsCnfgrUnconfigPhase2 unapplies the configuration.
*           Hence, is reused to undo INIT_3
*
* @end
*********************************************************************/
void dhcpsCnfgrFiniPhase3Process();

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
*           dhcpsRestoreProcess only builds default configuration and
*           applies it, while UNCONFIG_2 requires configuration information
*           to be reset. Hence, dhcpsCfgData is reset to 0.
*
* @end
*********************************************************************/
L7_RC_t dhcpsCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
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
void dhcpsCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_DHCPS_CNFGR_H */
