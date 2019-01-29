/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    event_manager.h
* @purpose     Event Manager  definitions
* @component   Event Manager
* @comments    none
* @create      11/14/2003
* @author      betsyt
* @end
*             
**********************************************************************/
#ifndef _EVENTMANAGER_H_
#define _EVENTMANAGER_H_

#include "l7_common.h"

typedef enum
{
    SYSEVENT_TRAP = 0x01000000,
    SYSEVENT_EVENT = 0x02000000,
    SYSEVENT_ERROR = 0x04000000,
    SYSEVENT_ALARM = 0x08000000,
    SYSEVENT_LOGMSG = 0x10000000
} sysEventType_t;

#ifdef L7_EVENTMANAGER

void EM_LOG(sysEventType_t event, L7_char8 *strEvent);
void EM_Register(void (*extEMHandler)(sysEventType_t event, L7_char8 *eventMessage) );
void EM_DeRegister(void (*extEMHandler)(sysEventType_t event, L7_char8 *eventMessage) );

#else

#define EM_LOG(args...) {}
#define EM_Register(args...) {}
#define EM_DeRegister(args...) {}

#endif /* L7_EVENTMANAGER */

#endif /* _EVENTMANAGER_H_ */
