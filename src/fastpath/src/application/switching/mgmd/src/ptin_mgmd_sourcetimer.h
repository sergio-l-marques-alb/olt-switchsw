/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    21/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/
#ifndef _L7_MGMD_SOURCETIMER_H
#define _L7_MGMD_SOURCETIMER_H

#include "ptin_timer_api.h"
#include "ptin_mgmd_core.h"


RC_t   ptin_mgmd_sourcetimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock);
RC_t   ptin_mgmd_sourcetimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock);
RC_t   ptin_mgmd_sourcetimer_start(snoopPTinL3Sourcetimer_t *timer, uint32 timeout, snoopPTinL3InfoDataKey_t groupKey, uint32 interfaceIdx,  snoopPTinL3Source_t *sourcePtr);
RC_t   ptin_mgmd_sourcetimer_stop(snoopPTinL3Sourcetimer_t *pTimer);
uint32 ptin_mgmd_sourcetimer_timeleft(snoopPTinL3Sourcetimer_t *pTimer);
BOOL   ptin_mgmd_sourcetimer_isRunning(snoopPTinL3Sourcetimer_t *pTimer);

RC_t   ptin_mgmd_event_sourcetimer(snoopPTinL3Sourcetimer_t *timerData);

#endif //_L7_MGMD_SOURCETIMER_H
