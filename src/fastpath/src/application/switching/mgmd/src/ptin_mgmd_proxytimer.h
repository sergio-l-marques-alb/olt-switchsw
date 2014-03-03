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
#ifndef _L7_MGMD_PROXYTIMER_H
#define _L7_MGMD_PROXYTIMER_H

#include "ptin_timer_api.h"
#include "snooping.h"


RC_t   ptin_mgmd_proxytimer_CB_set(PTIN_MGMD_TIMER_CB_t controlBlock);
RC_t   ptin_mgmd_proxytimer_CB_get(PTIN_MGMD_TIMER_CB_t* controlBlock);
RC_t   ptin_mgmd_proxytimer_start(mgmdProxyInterfaceTimer_t* pTimer, uint32 timeout, uint8 reportType, BOOL isInterface,uint32 noOfRecords, void* groupData);
RC_t   ptin_mgmd_proxytimer_stop(mgmdProxyInterfaceTimer_t *pTimer);
uint32 ptin_mgmd_proxytimer_timeleft(mgmdProxyInterfaceTimer_t *pTimer);
BOOL   ptin_mgmd_proxytimer_isRunning(mgmdProxyInterfaceTimer_t *pTimer);

RC_t   ptin_mgmd_event_proxytimer(mgmdProxyInterfaceTimer_t *timerData);

#endif //_L7_MGMD_PROXYTIMER_H
