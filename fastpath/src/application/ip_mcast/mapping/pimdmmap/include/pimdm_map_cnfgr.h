/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pimdm_map_cnfgr.h
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
*         gkiran
* @end
*
**********************************************************************/
#ifndef INCLUDE_PIMDM_CNFGR_H
#define INCLUDE_PIMDM_CNFGR_H

#include "l7_cnfgr_api.h"

typedef enum {
  PIMDM_PHASE_INIT_0 = 0,
  PIMDM_PHASE_INIT_1,
  PIMDM_PHASE_INIT_2,
  PIMDM_PHASE_WMU,
  PIMDM_PHASE_INIT_3,
  PIMDM_PHASE_EXECUTE,
  PIMDM_PHASE_UNCONFIG_1,
  PIMDM_PHASE_UNCONFIG_2,
} pimdmCnfgrState_t;


#define PIMDM_IS_READY (pimdmMapCbPtr->gblVars->pimdmCnfgrState == PIMDM_PHASE_INIT_3) ? (L7_TRUE) : ((pimdmMapCbPtr->gblVars->pimdmCnfgrState == PIMDM_PHASE_EXECUTE) ? (L7_TRUE) : ((pimdmMapCbPtr->gblVars->pimdmCnfgrState == PIMDM_PHASE_UNCONFIG_1) ? (L7_TRUE) : (L7_FALSE)))

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PIM-DM component
*
* @param    pCmdData     @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the pimdm comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pimdmApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PIMDM component
*
* @param    pCmdData     @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the pimdm comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pimdmCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse  - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason    - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - In case of other failures.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function undoes pimdmCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pimdmCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param   pResponse  - @b{(output)}  Response if L7_SUCCESS.
*
* @param   pReason    - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - In case of other failures.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function undoes pimdmCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pimdmCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse  - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason    - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors.
* @returns  L7_ERROR   - There were errors.
* @returns  L7_FAILURE - In case of other failures.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                    L7_CNFGR_RESPONSE_t *pResponse, 
                                    L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  This function undoes pimdmCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pimdmCnfgrFiniPhase3Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse  - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason    - @b{(output)}  Reason if L7_ERROR.
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
L7_RC_t pimdmCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, 
                                 L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse  - @b{(output)}  Response always command complete.
*
* @param    pReason    - @b{(output)}  Always 0
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
L7_RC_t pimdmCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse, 
                               L7_CNFGR_ERR_RC_t *pReason );
#endif /* INCLUDE_PIMDM_CNFGR_H */
