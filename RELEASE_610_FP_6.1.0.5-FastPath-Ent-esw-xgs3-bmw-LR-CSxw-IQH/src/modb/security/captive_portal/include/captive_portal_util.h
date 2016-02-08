
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename captive_portal_util.h 
*
* @purpose
*
* @component object handlers
*
* @comments
*
* @create 03/9/2009
*
* @author Vamshi krishna Madap 
* @end
*
**********************************************************************/

#ifndef CAPTIVE_PORTAL_UTIL_H
#define CAPTIVE_PORTAL_UTIL_H

#include "xlib.h"
#include <dirent.h>

L7_RC_t ewaUrlCpHook(void* context, char *url,char* dsturl);
xLibRC_t cpGetNextDefaultImage(char* current,char* next);
xLibRC_t cpGetNexFSImage(char* current,char* next);

#endif
