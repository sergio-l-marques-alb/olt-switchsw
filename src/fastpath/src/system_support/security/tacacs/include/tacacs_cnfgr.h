/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename tacacs_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component tacacs
*
* @comments none
*
* @create 03/21/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_TACACS_CNFGR_H
#define INCLUDE_TACACS_CNFGR_H


typedef enum {
  TACACS_PHASE_INIT_0 = 0,
  TACACS_PHASE_INIT_1,
  TACACS_PHASE_INIT_2,
  TACACS_PHASE_WMU,
  TACACS_PHASE_INIT_3,
  TACACS_PHASE_EXECUTE,
  TACACS_PHASE_UNCONFIG_1,
  TACACS_PHASE_UNCONFIG_2,
} tacacsCnfgrState_t;


/******************************************************
**        Internal function prototypes               **
******************************************************/
void tacacsApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t tacacsHandleCnfgrCmd(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t tacacsCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t tacacsCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t tacacsCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason );
void tacacsCnfgrFiniPhase1Process();
void tacacsCnfgrFiniPhase2Process();
void tacacsCnfgrFiniPhase3Process();
L7_RC_t tacacsCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t tacacsCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );

#endif /* INCLUDE_TACACS_CNFGR_H */
