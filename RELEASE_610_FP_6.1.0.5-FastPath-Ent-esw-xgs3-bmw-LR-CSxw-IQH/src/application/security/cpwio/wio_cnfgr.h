/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_cnfgr.h
*
* @purpose   Handle configurator events for this component
*
* @component captive portal wired interface owner
*
* @comments  none
*
* @create    2/27/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#ifndef _WIO_CNFGR_H_
#define _WIO_CNFGR_H_

#include "l7_cnfgr_api.h"

/*********************************************************************
* @purpose  Handles initialization messages from the configurator on
*           our own thread.
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
void wioCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
* @purpose  Do Init Phase 1.
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
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t wioCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  Do Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
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
* @notes    This component has no persistent data
*
* @end
*********************************************************************/
L7_RC_t wioCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason);

/*********************************************************************
* @purpose  Do Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
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
* @end
*********************************************************************/
L7_RC_t wioCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t *pReason);

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
* @end
*********************************************************************/
L7_RC_t wioCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
* @purpose  This function undoes wioCnfgrPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wioCnfgrFiniPhase1Process( void );

/*********************************************************************
* @purpose  This function undoes wioCnfgrPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wioCnfgrFiniPhase2Process(void);

/*********************************************************************
* @purpose  This function undoes wioCnfgrPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wioCnfgrFiniPhase3Process(void);

#endif   /* _WIO_CNFGR_H_ */
