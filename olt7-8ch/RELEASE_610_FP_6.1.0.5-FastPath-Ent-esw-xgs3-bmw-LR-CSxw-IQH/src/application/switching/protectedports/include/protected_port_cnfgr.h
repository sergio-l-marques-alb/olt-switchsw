/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_cnfgr.h
*
* @purpose   Protected Port configurator file
*
* @component protectedPort
*
* @comments 
*
* @create    6/5/2005
*
* @author    ryadagiri 
*
* @end
*             
**********************************************************************/

#ifndef PROTECTED_PORT_CNFGR_H
#define PROTECTED_PORT_CNFGR_H

#include "l7_cnfgr_api.h"

#define PROTECTED_PORT_IS_READY \
    (((protectedPortCnfgrState == PROTECTED_PORT_PHASE_WMU) || \
    (protectedPortCnfgrState == PROTECTED_PORT_PHASE_INIT_3) || \
    (protectedPortCnfgrState == PROTECTED_PORT_PHASE_EXECUTE) || \
    (protectedPortCnfgrState == PROTECTED_PORT_PHASE_UNCONFIG_1)) \
    ? (L7_TRUE) : (L7_FALSE))

typedef enum
{
    PROTECTED_PORT_PHASE_INIT_0 = 0,
    PROTECTED_PORT_PHASE_INIT_1,
    PROTECTED_PORT_PHASE_INIT_2,
    PROTECTED_PORT_PHASE_WMU,
    PROTECTED_PORT_PHASE_INIT_3,
    PROTECTED_PORT_PHASE_EXECUTE,
    PROTECTED_PORT_PHASE_UNCONFIG_1,
    PROTECTED_PORT_PHASE_UNCONFIG_2,
} protectedPortCnfgrState_t;


/* Begin Function Prototypes */

void protectedPortApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);  
L7_RC_t protectedPortCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t * pResponse,
                                            L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t protectedPortCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t * pResponse,
                                            L7_CNFGR_ERR_RC_t * pReason);                                   
L7_RC_t protectedPortCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t * pResponse,
                                            L7_CNFGR_ERR_RC_t * pReason);                                   
void protectedPortCnfgrFiniPhase1Process();                         
void protectedPortCnfgrFiniPhase2Process();                         
void protectedPortCnfgrFiniPhase3Process();                         
L7_RC_t protectedPortCnfgrNoopProcess(L7_CNFGR_RESPONSE_t * pResponse,
                                      L7_CNFGR_ERR_RC_t * pReason);
L7_RC_t protectedPortCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t * pResponse,
                                        L7_CNFGR_ERR_RC_t * pReason);

/* End Function Prototypes */
#endif /* PROTECTED_PORT_CNFGR_H */
