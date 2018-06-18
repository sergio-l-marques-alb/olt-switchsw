/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename nim_debug.c
*
* @purpose NIM debug routines
*
* @component nim
*
* @comments none
*
* @create 08/30/2000
*
* @author mbaucom
*
* @end
*             
**********************************************************************/

#ifndef NIM_DEBUG_H
#define NIM_DEBUG_H

#include "l7_common.h"


void nimDebugDataDump();


void nimLoggingSet(L7_uint32 value);


void nim_log_msg_fmt(L7_char8 * file, L7_ulong32 line, L7_char8 * msg);

#endif
