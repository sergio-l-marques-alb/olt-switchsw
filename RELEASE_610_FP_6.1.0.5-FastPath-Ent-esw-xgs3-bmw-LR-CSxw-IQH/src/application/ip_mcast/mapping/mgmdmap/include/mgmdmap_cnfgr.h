/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename mgmd_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component
*
* @comments
*
* @create 06/23/2003
*
* @author jeffr
* @end
*
**********************************************************************/


#ifndef INCLUDE_MGMD_CNFGR_H
#define INCLUDE_MGMD_CNFGR_H

typedef enum {
  MGMD_PHASE_INIT_0 = 0,
  MGMD_PHASE_INIT_1,
  MGMD_PHASE_INIT_2,
  MGMD_PHASE_WMU,
  MGMD_PHASE_INIT_3,
  MGMD_PHASE_EXECUTE,
  MGMD_PHASE_UNCONFIG_1,
  MGMD_PHASE_UNCONFIG_2,
} mgmdCnfgrState_t;


#define MGMD_IS_READY (mgmdMapCb->gblVars->mgmdCnfgrState == MGMD_PHASE_INIT_3) ? (L7_TRUE) : ((mgmdMapCb->gblVars->mgmdCnfgrState == MGMD_PHASE_EXECUTE) ? (L7_TRUE) : ((mgmdMapCb->gblVars->mgmdCnfgrState == MGMD_PHASE_UNCONFIG_1) ? (L7_TRUE) : (L7_FALSE)))

/*********************************************************************
*
* @purpose  CNFGR System Initialization for MGMD component
*
* @param    pCmdData    @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  void
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the mgmd comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void mgmdApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for MGMD component
*
* @param    pCmdData    @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  void
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the mgmd comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void mgmdCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS  There were no errors. Response is available.
* @returns  L7_ERROR    There were errors. Reason code is available.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread. 
*           This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                    L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes mgmdCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mgmdCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - In case of other failures.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread.
*           This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                    L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes mgmdCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mgmdCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse   @b{(output)}  Response if L7_SUCCESS.
* @param    pReason     @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                    L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes mgmdCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mgmdCnfgrFiniPhase3Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, 
                                L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse  @b{(output)}  Response always command complete.
* @param    pReason    @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           None.
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse, 
                              L7_CNFGR_ERR_RC_t   *pReason );
#endif /* INCLUDE_MGMD_CNFGR_H */
