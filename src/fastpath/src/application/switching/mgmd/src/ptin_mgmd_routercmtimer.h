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
#ifndef _PTIN_MGMD_ROUTERCMTIMER_H
#define _PTIN_MGMD_ROUTERCMTIMER_H

#include "ptin_timer_api.h"
#include "snooping.h"


RC_t   ptin_mgmd_routercmtimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock);
RC_t   ptin_mgmd_routercmtimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock);
RC_t   ptin_mgmd_routercmtimer_start(snoopPTinL3InfoData_t* groupData, uint32 portId);
RC_t   ptin_mgmd_routercmtimer_stop(snoopPTinCMtimer_t* pTimer);
uint32 ptin_mgmd_routercmtimer_timeleft(snoopPTinCMtimer_t* pTimer);
BOOL   ptin_mgmd_routercmtimer_isRunning(snoopPTinCMtimer_t* pTimer);

RC_t   ptin_mgmd_event_routercmtimer(snoopPTinCMtimer_t **timerData);

#endif //_PTIN_MGMD_ROUTERCMTIMER_H
