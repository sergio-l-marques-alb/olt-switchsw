
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename     captive_portal_debug.h
*
* @purpose      Captive portal debug routine header
*
* @component    captive portal
*
* @comments     none
*
* @create       06/25/2007
* 
* @author       dcaugherty
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_CAPTIVE_PORTAL_DEBUG_H
#define INCLUDE_CAPTIVE_PORTAL_DEBUG_H

#include "log.h"

#define  CP_DLOG(cp_log_level, __fmt__, __args__...) \
if (captivePortalDebugLevelCheck(cp_log_level) == L7_TRUE) \
{ \
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, __fmt__, ## __args__); \
} \

L7_RC_t  captivePortalDebugReset(void);
void     captivePortalDebugLevelSet(L7_uint32 level);
void     captivePortalDebugLevelClear(L7_uint32 level);
L7_BOOL  captivePortalDebugLevelCheck(L7_uint32 level);
void     captivePortalDebugRegister(void);

typedef enum {
  CP_LEVEL_TRACE = 0,           /* CP codepath tracing */
  CP_LEVEL_NON_FATAL_ERROR,
  CP_LEVEL_FATAL_ERROR,
  CP_LEVEL_LAST                 /* Must ALWAYS be last! */
} captive_portal_log_level_t;

#endif  /* INCLUDE_CAPTIVE_PORTAL_DEBUG_H */
