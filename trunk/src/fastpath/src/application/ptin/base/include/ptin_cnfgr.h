/*
 * ptin_cnfgr.h
 *
 * Created on: 2010/04/08
 * Author: Andre Brizido
 * 
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_CNFGR_H
#define _PTIN_CNFGR_H

#include "ptin_include.h"

typedef enum {
  PTIN_PHASE_INIT_0 = 0,
  PTIN_PHASE_INIT_1,
  PTIN_PHASE_INIT_2,
  PTIN_PHASE_WMU,
  PTIN_PHASE_INIT_3,
  PTIN_PHASE_EXECUTE,
  PTIN_PHASE_UNCONFIG_1,
  PTIN_PHASE_UNCONFIG_2,
} ptinCnfgrState_t;


L7_RC_t ptinCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ptinCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ptinCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );

void    ptinCnfgrFiniPhase1Process();
void    ptinCnfgrFiniPhase2Process();
void    ptinCnfgrFiniPhase3Process();

L7_RC_t ptinCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ptinCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason );

/* Semaphore to synchronize PTin task execution */
extern void *ptin_ready_sem;

#endif /* _PTIN_CNFGR_H */
