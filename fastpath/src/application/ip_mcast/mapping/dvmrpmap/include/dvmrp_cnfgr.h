/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_cnfgr.h
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


#ifndef _DVMRP_CNFGR_H
#define _DVMRP_CNFGR_H

#include "l7_cnfgr_api.h"

typedef enum
{
  DVMRP_PHASE_INIT_0 = 0,
  DVMRP_PHASE_INIT_1,
  DVMRP_PHASE_INIT_2,
  DVMRP_PHASE_WMU,
  DVMRP_PHASE_INIT_3,
  DVMRP_PHASE_EXECUTE,
  DVMRP_PHASE_UNCONFIG_1,
  DVMRP_PHASE_UNCONFIG_2,
} dvmrpCnfgrState_t;

#define DVMRP_IS_READY (dvmrpGblVar_g.dvmrpCnfgrState == DVMRP_PHASE_INIT_3) ? (L7_TRUE) : ((dvmrpGblVar_g.dvmrpCnfgrState == DVMRP_PHASE_EXECUTE) ? (L7_TRUE) : ((dvmrpGblVar_g.dvmrpCnfgrState == DVMRP_PHASE_UNCONFIG_1) ? (L7_TRUE) : (L7_FALSE)))

/*********************************************************************
*
* @purpose  CNFGR System Initialization for DVMRP component
*
* @param    pCmdData   @b{(input)}   Data structure for this   
*                                    CNFGR request
*                            
* @returns  void
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the dvmrp comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void dvmrpApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for DVMRP component
*
* @param    pCmdData    @b{(input)}  Data structure for this   
*                                    CNFGR request
*                            
* @returns  void
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the dvmrp comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void dvmrpCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
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
L7_RC_t dvmrpCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function undoes dvmrpCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpCnfgrFiniPhase1Process();

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
L7_RC_t dvmrpCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function undoes dvmrpCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
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
L7_RC_t dvmrpCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function undoes dvmrpCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpCnfgrFiniPhase3Process();

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
L7_RC_t dvmrpCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );

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
L7_RC_t dvmrpCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );
#endif /* _DVMRP_CNFGR_H */
