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
#ifndef _PTIN_MGMD_GROUPTIMER_H
#define _PTIN_MGMD_GROUPTIMER_H

#include "ptin_timer_api.h"
#include "snooping.h"


RC_t   ptin_mgmd_grouptimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock);
RC_t   ptin_mgmd_grouptimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock);
RC_t   ptin_mgmd_grouptimer_start(snoopPTinL3Grouptimer_t *pTimer, uint32 timeout, snoopPTinL3InfoDataKey_t groupData, uint32 interfaceIdx);
RC_t   ptin_mgmd_grouptimer_stop(snoopPTinL3Grouptimer_t *pTimer);
uint32 ptin_mgmd_grouptimer_timeleft(snoopPTinL3Grouptimer_t *pTimer);
BOOL   ptin_mgmd_grouptimer_isRunning(snoopPTinL3Grouptimer_t *pTimer);

RC_t   ptin_mgmd_event_grouptimer(snoopPTinL3Grouptimer_t *timerData);

#endif //_PTN_MGMD_GROUPTIMER_H
