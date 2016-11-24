/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  auto_install_cnfgr.h
*
* @purpose   auto_istall configurator API
*
* @component auto_install
*
* @comments 
*
* @create    28/12/2007
*
* @author    lkhedyk
*
* @end
*             
**********************************************************************/
#ifndef AUTO_INSTALL_CNFGR_H
#define AUTO_INSTALL_CNFGR_H

#include "l7_cnfgr_api.h"

typedef enum
{
  AUTO_INSTALL_PHASE_INIT_0 = 0,
  AUTO_INSTALL_PHASE_INIT_1,
  AUTO_INSTALL_PHASE_INIT_2,
  AUTO_INSTALL_PHASE_WMU,
  AUTO_INSTALL_PHASE_INIT_3,
  AUTO_INSTALL_PHASE_EXECUTE,
  AUTO_INSTALL_PHASE_UNCONFIG_1,
  AUTO_INSTALL_PHASE_UNCONFIG_2,
} autoInstallCnfgrState_t;


#define AUTO_INSTALL_STACK_SIZE ((128 * 1024) + platTaskStackDefaultGet())
/* Changed from L7_DEFAULT_STACK_SIZE because of stack overflow crash when
   applying downloaded configuration. Scripts should be executed only in
   emWeb context to ensure sufficient stack size. For now this has been
   updated to match emWeb's stack size */
                                   

/* Begin Function Prototypes */

void    autoInstallApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );

/*********************************************************************
*
* @purpose  CNFGR System Initialization for auto-install component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the isdp comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void autoInstallApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );

/*********************************************************************
*
* @purpose  System Initialization for auto_install
*
* @param    none
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallInit(L7_CNFGR_CMD_DATA_t *pCmdData);

/*********************************************************************
*
* @purpose  System Init Undo for auto_install
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallInitUndo();

/*********************************************************************
* @purpose  This function process the configurator control
*            commands/request pair Init Phase 1.
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
* @end
*********************************************************************/
L7_RC_t autoInstallCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                           L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control 
*           commands/request pair Init Phase 2.
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
* @end
*********************************************************************/
L7_RC_t autoInstallCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                           L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control 
*           commands/request pair Init Phase 3.
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
* @end
*********************************************************************/
L7_RC_t autoInstallCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                           L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes autoInstallCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes autoInstallCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes autoInstallCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallCnfgrFiniPhase3Process();

/*********************************************************************
* @purpose  This function process the configurator control 
*           commands/request pair Unconfigure Phase 2.
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

L7_RC_t autoInstallCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control
*           commands/request pair as a NOOP.
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
L7_RC_t autoInstallCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
*
* @purpose  To parse the configurator commands sent to autoInstallTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

/* End Function Prototypes */

#endif /* AUTO_INSTALL_CNFGR_H */
