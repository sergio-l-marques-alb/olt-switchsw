/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtrdisc_cnfgr.h
*
* @purpose   Specifies the data structures and interface for the new
*            configurator API
*
* @component 
*
* @comments  none
*
* @create    11/02/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
		
*************************************************************/

#ifndef INCLUDE_RTR_DISC_CNFGR_H
#define INCLUDE_RTR_DISC_CNFGR_H

#include "l7_cnfgr_api.h"

/* Flag to turn on use of read-write lock */
#define RTR_DISC_USE_RWLOCK     1

/* Different phases of the unit */
typedef enum {
  RTR_DISC_PHASE_INIT_0 = 0,
  RTR_DISC_PHASE_INIT_1,
  RTR_DISC_PHASE_INIT_2,
  RTR_DISC_PHASE_WMU,
  RTR_DISC_PHASE_INIT_3,
  RTR_DISC_PHASE_EXECUTE,
  RTR_DISC_PHASE_UNCONFIG_1,
  RTR_DISC_PHASE_UNCONFIG_2,
} rtrDiscCnfgrState_t;


#define RTR_DISC_IS_READY (((rtrDiscCnfgrState == RTR_DISC_PHASE_INIT_3) || \
					   (rtrDiscCnfgrState == RTR_DISC_PHASE_EXECUTE) || \
					   (rtrDiscCnfgrState == RTR_DISC_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))


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
void rtrDiscApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );

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
L7_RC_t rtrDiscCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t rtrDiscCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t rtrDiscCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t rtrDiscCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse, 
                                  L7_CNFGR_ERR_RC_t   *pReason );


/*********************************************************************
* @purpose  This function undoes rtrDiscCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes rtrDiscCnfgrInitPhase2Process, i.e. 
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
void rtrDiscCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes rtrDiscCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    rtrDiscCnfgrUnconfigPhase2 unapplies the configuration.
*           Hence, is reused to undo INIT_3
*
* @end
*********************************************************************/
void rtrDiscCnfgrFiniPhase3Process();

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
*           rtrDiscRestoreProcess only builds default configuration and 
*           applies it, while UNCONFIG_2 requires configuration information 
*           to be reset. Hence, rtrDiscCfgData is reset to 0.
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function parses the cnfgr command/pair object and handles 
*           the command/request.          
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
* @notes    This function runs in the router discovery processing task thread.. 
*           
* @notes    In current release (H), INIT_1 never runs on this thread,
*           hence is not handled here.

* @end
*********************************************************************/

void rtrDiscCnfgrParse( L7_CNFGR_CMD_DATA_t *pCmdData );


#endif /* INCLUDE_RTR_DISC_CNFGR_H */
