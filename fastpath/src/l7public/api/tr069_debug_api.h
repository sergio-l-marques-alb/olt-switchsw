/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    tr069_debug_api.h
* @purpose     TR069 debug functions
* @component   tr069
* @comments    none
* @create      10-Apr-2008
* @author      drajendra
* @end
*             
**********************************************************************/
#ifndef TR069_DEBUG_API_H
#define TR069_DEBUG_API_H

#include "l7_common.h"
/************************************************************************
* @purpose  Get the current status of displaying tr069 packet debug info
*
* @param    flag  @b{(output)}  Packet Debug Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
************************************************************************/
L7_RC_t tr069DebugPacketTraceFlagGet(L7_BOOL *flag);
/*****************************************************************************
* @purpose  Turns on/off the displaying of tr069 packet debug info of
*           a tr069 client
*
* @param    flag  @b{(input)}  Packet Debug Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t tr069DebugPacketTraceFlagSet(L7_BOOL flag);
#endif /* TR069_DEBUG_API_H */
