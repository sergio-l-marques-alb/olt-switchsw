/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename sslt_cnfgr.h
*
* @purpose Header file to support the new configurator API
*
* @component sslt
*
* @comments none
*
* @create 10/22/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_SSLT_CNFGR_H
#define INCLUDE_SSLT_CNFGR_H

#include "l7_cnfgr_api.h"

/*
** Internal function prototypes
*/

void ssltApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

L7_RC_t ssltInit(L7_CNFGR_CMD_DATA_t *pCmdData);

void ssltInitUndo();

L7_RC_t ssltCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse, 
                                   L7_CNFGR_ERR_RC_t *pReason);

L7_RC_t ssltCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, 
                                   L7_CNFGR_ERR_RC_t *pReason);

L7_RC_t ssltCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse, 
                                   L7_CNFGR_ERR_RC_t *pReason);

void ssltCnfgrFiniPhase1Process();

void ssltCnfgrFiniPhase2Process();

void ssltCnfgrFiniPhase3Process();

L7_RC_t ssltCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse, 
                              L7_CNFGR_ERR_RC_t *pReason);

L7_RC_t ssltCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse, 
                               L7_CNFGR_ERR_RC_t *pReason);

void ssltCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_SSLT_CNFGR_H */

