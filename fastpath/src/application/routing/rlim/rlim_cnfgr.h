/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename rlim_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/16/2005
*
* @author eberge
* @end
*
**********************************************************************/


#ifndef INCLUDE_RLIM_CNFGR_H
#define INCLUDE_RLIM_CNFGR_H

#include "defaultconfig.h"
#include "l7_cnfgr_api.h"

typedef enum {
  RLIM_PHASE_INIT_0 = 0,
  RLIM_PHASE_INIT_1,
  RLIM_PHASE_INIT_2,
  RLIM_PHASE_WMU,
  RLIM_PHASE_INIT_3,
  RLIM_PHASE_EXECUTE,
  RLIM_PHASE_UNCONFIG_1,
  RLIM_PHASE_UNCONFIG_2,
} rlimCnfgrState_t;

typedef struct {
  L7_BOOL rlimSave;
  L7_BOOL rlimRestore;
  L7_BOOL rlimHasDataChanged;
} rlimDeregister_t;

#define RLIM_IS_READY \
          (((rlimCnfgrState == RLIM_PHASE_INIT_3) || \
            (rlimCnfgrState == RLIM_PHASE_EXECUTE) || \
            (rlimCnfgrState == RLIM_PHASE_UNCONFIG_1)) \
           ? (L7_TRUE) : (L7_FALSE))

/*
 * Function prototypes
 */
void rlimApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_BOOL rlimHasDataChanged(void);
void rlimResetDataChanged(void);
L7_uint32 rlimSave(void);
L7_uint32 rlimRestore(void);
void rlimRestoreProcess(void);
void rlimCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t rlimCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t rlimCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t rlimCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason);
void rlimCnfgrFiniPhase1Process();
void rlimCnfgrFiniPhase2Process();
void rlimCnfgrFiniPhase3Process();
L7_RC_t rlimCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t rlimCnfgrUconfigPhase1(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t rlimCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason);

#endif /* INCLUDE_RLIM_CNFGR_H */
