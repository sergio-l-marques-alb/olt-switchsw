
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

L7_RC_t  captivePortalDebugReset(void);
void     captivePortalDebugLevelSet(L7_uint32 level);
void     captivePortalDebugLevelClear(L7_uint32 level);
void     captivePortalDebugRegister(void);

#endif  /* INCLUDE_CAPTIVE_PORTAL_DEBUG_H */
