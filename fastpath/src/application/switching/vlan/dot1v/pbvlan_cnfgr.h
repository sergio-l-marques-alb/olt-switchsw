/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pbvlan_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component 
*
* @comments
*
* @create 07/28/2003
*
* @author msmith
* @end
*
**********************************************************************/


#ifndef INCLUDE_PBVLAN_CNFGR_H
#define INCLUDE_PBVLAN_CNFGR_H

typedef enum {
  PBVLAN_PHASE_INIT_0 = 0,
  PBVLAN_PHASE_INIT_1,
  PBVLAN_PHASE_INIT_2,
  PBVLAN_PHASE_WMU,
  PBVLAN_PHASE_INIT_3,
  PBVLAN_PHASE_EXECUTE,
  PBVLAN_PHASE_UNCONFIG_1,
  PBVLAN_PHASE_UNCONFIG_2,
} pbVlanCnfgrState_t;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for pbVlan component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the pbVlan comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pbVlanApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pbVlanCnfgrInitPhase1Process(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pbVlanCnfgrInitPhase2Process(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pbVlanCnfgrInitPhase3Process(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function undoes pbVlanCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pbVlanCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes pbVlanCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pbVlanCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes pbVlanCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pbVlanCnfgrFiniPhase3Process();


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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pbVlanCnfgrNoopProcess(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pbVlanCnfgrUconfigPhase2(L7_CNFGR_CMD_DATA_t *pCmdData);


#endif /* INCLUDE_PBVLAN_CNFGR_H */

