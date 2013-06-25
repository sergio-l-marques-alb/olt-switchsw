/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_cnfgr.h
*
* @purpose    Contains prototypes of APIs used during component init
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_SNOOPING_CNFGR_H
#define INCLUDE_SNOOPING_CNFGR_H

#include "l7_cnfgr_api.h"

typedef enum {
  SNOOP_PHASE_INIT_0 = 0,
  SNOOP_PHASE_INIT_1,
  SNOOP_PHASE_INIT_2,
  SNOOP_PHASE_WMU,
  SNOOP_PHASE_INIT_3,
  SNOOP_PHASE_EXECUTE,
  SNOOP_PHASE_UNCONFIG_1,
  SNOOP_PHASE_UNCONFIG_2,
} snoopCnfgrState_t;
struct snoop_cb_s;

void        snoopApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

L7_RC_t     snoopInit(L7_CNFGR_CMD_DATA_t *pCmdData);
void        snoopInitUndo();
L7_RC_t     snoopCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t     snoopCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t     snoopCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                        L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason);
void        snoopCnfgrFiniPhase1Process();
void        snoopCnfgrFiniPhase2Process();
void        snoopCnfgrFiniPhase3Process();
L7_RC_t     snoopCnfgrTerminateProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t     snoopCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t     snoopCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );
void        snoopCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t     snoopEBInit();
void        snoopEBInitUndo();
L7_RC_t     snoopCBInit(L7_uint32 cbIndex, L7_uchar8 family);
void        snoopCBInitUndo(L7_uint32 cbIndex);
L7_RC_t     snoopStartTask(void);
L7_BOOL     snoopIsReady();
L7_RC_t     snoopCnfgrConfigFileReadApply(struct snoop_cb_s *pSnoopCB,
                                          L7_BOOL warmRestart,
                                          L7_CNFGR_RESPONSE_t *pResponse,
                                          L7_CNFGR_ERR_RC_t   *pReason);
#include "ptin_globaldefs.h"

/*PTIN Added 
  MGDM Memory Initialization*/
#if SNOOP_PTIN_IGMPv3_ROUTER
L7_RC_t     snoopPtinRouterAVLTreeInit(void);
#endif

#if SNOOP_PTIN_IGMPv3_PROXY
L7_RC_t     snoopPtinProxyAVLTreeDBInit(void);
L7_RC_t     snoopPtinProxyAVLTreeGroupRecordInit(void);

L7_RC_t     snoopPtinProxyAVLTreeInterfacetimerInit(void);
L7_RC_t     snoopPtinProxyAVLTreeGrouptimerInit(void);
L7_RC_t     snoopPtinProxyAVLTreeSourcetimerInit(void);
#endif

#endif

#endif /* INCLUDE_SNOOPING_CNFGR_H */
