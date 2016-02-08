/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pimsm_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component
*
* @comments
*
* @create 06/23/2003
*
* @author    gkiran/dsatyanarayana
* @end
*
**********************************************************************/


#ifndef INCLUDE_PIMSM_CNFGR_H
#define INCLUDE_PIMSM_CNFGR_H

#include "l7_cnfgr_api.h"

typedef enum {
  PIMSM_PHASE_INIT_0 = 0,
  PIMSM_PHASE_INIT_1,
  PIMSM_PHASE_INIT_2,
  PIMSM_PHASE_WMU,
  PIMSM_PHASE_INIT_3,
  PIMSM_PHASE_EXECUTE,
  PIMSM_PHASE_UNCONFIG_1,
  PIMSM_PHASE_UNCONFIG_2,
} pimsmCnfgrState_t;

#define PIMSM_IS_READY (pimsmGblVariables_g.pimsmCnfgrState == PIMSM_PHASE_INIT_3) ? (L7_TRUE) : ((pimsmGblVariables_g.pimsmCnfgrState == PIMSM_PHASE_EXECUTE) ? (L7_TRUE) : ((pimsmGblVariables_g.pimsmCnfgrState == PIMSM_PHASE_UNCONFIG_1) ?(L7_TRUE) : (L7_FALSE)))

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PIMSM component
*
* @param    pCmdData   @b{(input)   Data structure for this
*                                    CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the pimsm comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pimsmApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PIMSM component
*
* @param    pCmdData    @b{(input)   Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the pimsm comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pimsmCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse   @b{(output)}  Response if L7_SUCCESS.
* @param    pReason     @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - If other failures.
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
L7_RC_t pimsmCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function undoes pimsmCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimsmCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse    @b{(output)}  Response if L7_SUCCESS.
* @param    pReason      @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - If other failures.
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
L7_RC_t pimsmCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function undoes pimsmCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimsmCnfgrFiniPhase2Process();

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
L7_RC_t pimsmCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function undoes pimsmCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimsmCnfgrFiniPhase3Process();

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse   @b{(output)}  Response if L7_SUCCESS.
* @param    pReason     @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
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
L7_RC_t pimsmCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, 
                                 L7_CNFGR_ERR_RC_t *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse   @b{(output)}  Response always command complete.
* @param    pReason     @b{(output)}  Always 0
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
L7_RC_t pimsmCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse, 
                               L7_CNFGR_ERR_RC_t *pReason );
#endif /* INCLUDE_PIMSM_CNFGR_H */
