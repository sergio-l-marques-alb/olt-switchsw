/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename mirror_cnfgr.h
*
* @purpose Provide the Configurator hooks for Initialization and Teardown
*
* @component mirroring
*
* @comments
*
* @create 06/03/2003
*
* @author mbaucom
* @end
*
**********************************************************************/

#ifndef MIRROR_CNFGR_H
#define MIRROR_CNFGR_H

#include "l7_common.h"
#include "l7_cnfgr_api.h"


typedef enum {
  MIRROR_PHASE_INIT_0 = 0,
  MIRROR_PHASE_INIT_1,
  MIRROR_PHASE_INIT_2,
  MIRROR_PHASE_WMU,
  MIRROR_PHASE_INIT_3,
  MIRROR_PHASE_EXECUTE,
  MIRROR_PHASE_UNCONFIG_1,
  MIRROR_PHASE_UNCONFIG_2,
} mirrorPhases_t;


#define MIRRORING_IS_READY (((mirrorState == MIRROR_PHASE_INIT_3) || \
                            (mirrorState == MIRROR_PHASE_EXECUTE) || \
                            (mirrorState == MIRROR_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

L7_RC_t mirrorCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t mirrorCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t mirrorCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t mirrorCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t mirrorCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason );

L7_CNFGR_STATE_t mirrorCnfgrPhaseGet(void);

L7_RC_t   mirrorPhaseOneInit(void);
void mirrorPhaseOneFini(void);

L7_RC_t   mirrorPhaseTwoInit(void);
void mirrorPhaseTwoFini(void);

L7_RC_t   mirrorPhaseThreeInit(void);
void mirrorPhaseThreeFini(void);

#endif /* _MIRROR_CNFGR_H */
