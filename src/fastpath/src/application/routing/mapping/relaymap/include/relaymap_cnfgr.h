/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  relaymap_cnfgr.h
*
* @purpose   Specifies the interface for the new configurator API
*            for the DHCP relay component.
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

#ifndef INCLUDE_DHCP_RELAY_CNFGR_H
#define INCLUDE_DHCP_RELAY_CNFGR_H

/* Different phases of the unit */
typedef enum {
  DHCPRELAYMAP_PHASE_INIT_0 = 0,
  DHCPRELAYMAP_PHASE_INIT_1,
  DHCPRELAYMAP_PHASE_INIT_2,
  DHCPRELAYMAP_PHASE_WMU,
  DHCPRELAYMAP_PHASE_INIT_3,
  DHCPRELAYMAP_PHASE_EXECUTE,
  DHCPRELAYMAP_PHASE_UNCONFIG_1,
  DHCPRELAYMAP_PHASE_UNCONFIG_2,
} dhcpRelayMapCnfgrState_t;

#define DHCPRELAY_IS_READY ((( dhcpRelayMapCnfgrState  == DHCPRELAYMAP_PHASE_INIT_3) || \
                                           (dhcpRelayMapCnfgrState ==  DHCPRELAYMAP_PHASE_EXECUTE) || \
                                           (dhcpRelayMapCnfgrState ==  DHCPRELAYMAP_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

void dhcpRelayCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

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
void dhcpRelayApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );

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
L7_RC_t dhcpRelayCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
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
* @notes    DHCP relay does not require any interface information,hence
*           does not register either NIM or IP Map.
*
* @end
*********************************************************************/
L7_RC_t dhcpRelayCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t dhcpRelayCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t dhcpRelayCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes dhcpRelayCnfgrInitPhase1Process.
*
* @param    none
*     
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
void dhcpRelayCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes dhcpRelayCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpRelayCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes dhcpRelayCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    dhcpRelayCnfgrUnconfigPhase2 unapplies the configuration.
*           Hence, is reused to undo INIT_3
*
* @end
*********************************************************************/
void dhcpRelayCnfgrFiniPhase3Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2. Mainly unapplies the current 
*           configuration 
*           
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
*           dhcpRelayRestoreProcess only builds default configuration and 
*           applies it, while UNCONFIG_2 requires configuration information 
*           to be reset. Hence, pDhcpRelayCfgData is reset to 0.
*        
*     
* @end
*********************************************************************/
L7_RC_t dhcpRelayCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function facilitates to know the state of DHCP RELAY
*                      
*
* @returns  L7_TRUE  -  DHCP Relay componet is available.
* @returns  L7_FALSE -  DHCP Relay componet is not available.
*
*
* @end
*********************************************************************/
L7_BOOL dhcpRelay_is_ready();

#endif /* INCLUDE_DHCP_RELAY_CNFGR_H */
