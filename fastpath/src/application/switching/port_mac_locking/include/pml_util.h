/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename pml_util.h
*
* @purpose Port MAC Locking utility function prototypes
*
* @component Port MAC Locking
*
* @comments none
*
* @create 05/21/2004
*
* @author colinw
* @end
*
**********************************************************************/

/*********************************************************************
 *                   
 ********************************************************************/
#ifndef PML_UTIL_H
#define PML_UTIL_H

#include "comm_mask.h"

L7_RC_t pmlIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator);

void pmlStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);

L7_RC_t pmlIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator);

void pmlStartupNotifyProcess(NIM_STARTUP_PHASE_t startupPhase);

L7_RC_t pmlIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode, L7_BOOL warmStart);

L7_RC_t pmlAdminModeApply(L7_uint32 adminMode);

L7_RC_t pmlCfgDump(void);

void pmlSemaTake(void);
void pmlSemaGive(void);

L7_BOOL pmlMapIntfIsConfigurable(L7_uint32 intIfNum, pmlIntfCfgData_t **pCfg);
L7_BOOL pmlMapIntfIsOperational(L7_uint32 intIfNum, pmlIntfOprData_t **pOpr);
L7_BOOL pmlMapIntfIsAcquired(L7_uint32 intIfNum);

L7_BOOL pmlMapIntfConfigEntryGet(L7_uint32 intIfNum, pmlIntfCfgData_t **pCfg);

L7_RC_t pmlIntfCreate(L7_uint32 intIfNum);

L7_RC_t pmlApplyIntfConfigData(L7_uint32 intIfNum, L7_BOOL warmStart);

L7_RC_t pmlIntfDetach(L7_uint32 intIfNum);

L7_RC_t pmlIntfDelete(L7_uint32 intIfNum);

L7_RC_t pmlIntfAllStaticEntriesAdd(L7_uint32 intIfNum);
L7_RC_t pmlIntfAllStaticEntriesDelete(L7_uint32 intIfNum);

void pmlSyncCompleteSet(L7_BOOL syncComplete);
L7_BOOL pmlSyncCompleteGet();

#endif /* PML_UTIL_H */
