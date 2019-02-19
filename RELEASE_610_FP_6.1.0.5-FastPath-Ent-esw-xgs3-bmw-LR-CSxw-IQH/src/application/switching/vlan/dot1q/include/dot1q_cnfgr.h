/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename dot1q_cnfgr.c
*
* @purpose cnfgr data for dot1q
*
* @component dot1q
*
* @comments none
*
* @create 11/1/2005
*
* @author akulkarni
*
* @end
*             
**********************************************************************/
#ifndef __DOT1Q_CNFGR_H__
#define __DOT1Q_CNFGR_H__

#include "l7_cnfgr_api.h"


extern dot1qCnfgrState_t dot1qCnfgrState;
void dot1qApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

L7_RC_t dot1qInit(L7_CNFGR_CMD_DATA_t *pCmdData);
void dot1qInitUndo();
void dot1qParseCnfgr(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_BOOL dot1qCnfgrStateCheck (void);

L7_RC_t dot1qCnfgrInitPhase1Process(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t dot1qCnfgrInitPhase2Process(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t dot1qCnfgrInitPhase3Process(L7_CNFGR_CMD_DATA_t *pCmdData);
void dot1qCnfgrFiniPhase1Process(void);
void dot1qCnfgrFiniPhase2Process(void);
void dot1qCnfgrFiniPhase3Process(void);
L7_RC_t dot1qCnfgrNoopProcess(void);

void dot1qVlanUnconfigure(L7_CNFGR_CMD_DATA_t *pCmdData);
void dot1qVlanEnterSteadyState(L7_CNFGR_CMD_DATA_t *pCmdData);
void dot1qVlanTerminate(L7_CNFGR_CMD_DATA_t *pCmdData);
void dot1qVlanSuspend(L7_CNFGR_CMD_DATA_t *pCmdData);
void dot1qVlanResume(L7_CNFGR_CMD_DATA_t *pCmdData);
void dot1qVlanUnCfgPhase1(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t dot1qVlanUnCfgPhase2(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* __DOT1Q_CNFGR_H__ */
