/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cmd_logger_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  cmdLogger
*
* @comments   none
*
* @create     25-Oct-2004
*
* @author     gaunce
* @end
*
**********************************************************************/

#ifndef CMD_LOGGER_CNFGR_H
#define CMD_LOGGER_CNFGR_H

#include "l7_common.h"
#include "l7_cnfgr_api.h"

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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t cmdLoggerCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason );


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
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
L7_RC_t cmdLoggerCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
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
L7_RC_t cmdLoggerCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t cmdLoggerCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );


/*********************************************************************
* @purpose  Transitions the userMgr to configurator state WMU from PHASE 3
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t cmdLoggerCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                          L7_CNFGR_ERR_RC_t   *pReason );


/*********************************************************************
* @purpose  This function returns the phase that userMgr believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until userMgr has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
*
* @end
*********************************************************************/
L7_CNFGR_RQST_t cmdLoggerCnfgrPhaseGet(void);
#endif
