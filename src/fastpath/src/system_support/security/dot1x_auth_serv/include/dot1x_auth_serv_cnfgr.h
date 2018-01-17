/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dot1x_auth_serv_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  Dot1x Authentication Server component
*
* @comments   none
*
* @create     11/17/2009
*
* @author     pradeepk
* @end
*
**********************************************************************/

#ifndef DOT1X_AUTH_SERV_CNFGR_H
#define DOT1X_AUTH_SERV_CNFGR_H

#include "l7_common.h"
#include "l7_cnfgr_api.h"

/*********************************************************************
 * @purpose Initialize the Dot1x user db for Phase 1
 *
 * @param   void
 *
 * @returns L7_SUCCESS  Phase 1 completed
 * @returns L7_FAILURE  Phase 1 incomplete
 *
 * @notes  If phase 1 is incomplete, it is up to the caller to call the fini
 *         function if desired.  If this fails, it is due to an inability to
 *         to acquire resources.
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServPhaseOneInit(void);

/*********************************************************************
 * @purpose  Release all resources collected during phase 1
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void dot1xAuthServPhaseOneFini(void);

/*********************************************************************
* @purpose  This function undoes dot1xAuthServCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dot1xAuthServPhaseTwoFini();

/*********************************************************************
* @purpose  This function undoes dot1xAuthServCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dot1xAuthServPhaseThreeFini();
/*********************************************************************
 * @purpose  Phase userMgr to configurator phase unconfigurator
 *
 * @returns  void
 *
 * @notes    Nothing to apply for config, only reset the memory
 *
 * @end
 *********************************************************************/
void dot1xAuthServUnconfigure(void);

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
L7_RC_t dot1xAuthServCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t dot1xAuthServCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason);

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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t dot1xAuthServCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 1.
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
L7_RC_t dot1xAuthServCnfgrUnConfigPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t dot1xAuthServCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason );

#endif

