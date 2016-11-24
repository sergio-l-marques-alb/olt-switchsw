/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename openssl_cnfgr.h
*
* @purpose Header file to support the new configurator API
*
* @component openssl
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
#ifndef INCLUDE_OPENSSL_CNFGR_H
#define INCLUDE_OPENSSL_CNFGR_H

/*
** Internal function prototypes
*/

void opensslApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
void opensslInitUndo();

L7_RC_t opensslCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason);

L7_RC_t opensslCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason);

L7_RC_t opensslCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason);

void opensslCnfgrFiniPhase1Process();

void opensslCnfgrFiniPhase2Process();

void opensslCnfgrFiniPhase3Process();

L7_RC_t opensslCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t *pReason);

L7_RC_t opensslCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t *pReason);

void opensslCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_OPENSSL_CNFGR_H */

